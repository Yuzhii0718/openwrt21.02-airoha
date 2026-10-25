 /************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/libcompileoption.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <net/ipv6.h>
#include <net/ip.h>
#include <net/vxlan.h>
#include <linux/proc_fs.h>

#include <ecnt_hook/ecnt_hook_qdma.h>

#include <modules/npu/tunnel_mail.h>

#include "ra_nat_npu.h"
#include "ra_nat_offload.h"
#include "ra_nat_gen.h"
#include "ra_nat_dev.h"
#include "foe_fdb.h"
#include "hwnat_ioctl.h"
#include "util.h"
#include "ra_nat_ic_dis.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define NPU_BRIGE_VERIFY	0

#define MAX_NPU_VXLAN_HDR_NUM 20

#define MAX_SRH_SEG_NUM 4

typedef struct  {
	__u8                    next_hdr;
	__u8                    hdr_len;
	__u8                    rt_type;
	__u8                    segs_left;
	__u8                    first_seg;
	__u16                   flags;
	__u8                    reserved;
}__packed Ipv6_Sr_Hdr_t;

typedef struct
{
    /*Mac header*/
    struct ethhdr eth;

	/* IPv6 header */
    struct ipv6hdr ip6h;

	/* SR header */
	Ipv6_Sr_Hdr_t srh;

	/* segmets */
	struct in6_addr	segment[MAX_SRH_SEG_NUM];	
} __packed Npu_Srv6_Hdr_t;

typedef struct
{
    /*Mac header*/
    struct ethhdr eth;
	/* IP header */
    struct iphdr iph;

	/* UDP header */
    struct udphdr uh;

    /* Vxlan header */
    struct vxlanhdr vxlanh;
} __packed Npu_Vxlan_Hdr_V4_t;

typedef struct
{
	struct list_head  list;
	unsigned int foe_num;
}Npu_Vxlan_Hdr_Session_t;

typedef struct
{
    int valid;
    unsigned char ifname[32];    
    struct list_head session_list;
}Npu_Vxlan_Hdr_valid_t;

Npu_Vxlan_Hdr_valid_t npu_vxlan_hdr_valid[MAX_NPU_VXLAN_HDR_NUM];

unsigned int npu_tunnel_offload_proto = 0;

static DEFINE_RWLOCK(vxlan_hdr_lock);
/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int FoeAddEntry(struct hwnat_tuple *opt);
extern struct sk_buff *__vlan_put_tag(struct sk_buff *skb, u16 vlan_tci);
extern unsigned char *GetMacAddr(void);
extern struct device* get_frame_engine_dev(void);
extern struct FoeEntryExt*  foe_ext;
extern int hwnat_check_entry_is_bind(int index);
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
#if NPU_BRIGE_VERIFY
static int hwnat_add_static_rule_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	unsigned int rule_num = 0;
	unsigned int channel_id = 0;
	struct hwnat_tuple tuple;
	int i = 0;
	unsigned int in_sip = 0x07070707;
	unsigned int in_dip = 0x08080808;
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d %d", &rule_num, &channel_id);

	printk("add %d rules, to channel-%d\n", rule_num, channel_id);

	in_sip = in_sip | (channel_id<<20) | (channel_id<<12);
	in_dip = in_dip | (channel_id<<20);

	for(i = 0;i < rule_num;i++)
	{
		memset(&tuple,0,sizeof(struct hwnat_tuple));
		tuple.pkt_type = IPV4_HNAPT;
		tuple.is_udp = 1;
		tuple.ing_sipv4 = in_sip+i;
		tuple.ing_dipv4 = in_dip+i*3;
		tuple.ing_sp = 1024+i*7;
		tuple.ing_dp = 1024+i*9;
		tuple.eg_sipv4 = in_sip+i+1;
		tuple.eg_dipv4 = in_dip+(i+1)*3;
		tuple.eg_sp = 1024+(i+1)*7;
		tuple.eg_dp = 1024+(i+1)*9;
		tuple.smac[2] = 0x2;
		tuple.smac[5] = 0x2;
		tuple.dmac[2] = 0x1+i;
		tuple.dmac[5] = 0x3;
		tuple.tsid = 0x7f;		
		tuple.ifc_idx= 0xff;
		if(i == (rule_num-1))
		{
			tuple.channel = 1;
			tuple.stag = 0x2;
			tuple.info2 = 0xffe221;
		}
		else	
		{
			tuple.info2 = (0xffe0c0 | channel_id);
		}
		FoeAddEntry(&tuple);
	}

	printk("done\n");
	
	return count;
}	

static int hwnat_send_pkt_to_npu_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	unsigned int qdma_type = 0,pkt_type = 0;
	unsigned char pkt_type_name[][32] = 
	{
		"untag ipoe pkt",
		"single tag ipoe pkt",
		"double tag ipoe pkt",
		"untag pppoe pkt",		
		"single tag pppoe pkt",		
		"double tag pppoe pkt",
	};
	struct sk_buff* skb= skbmgr_alloc_skb2k();
	unsigned char ipoe_data[ ]=
	{
		0x00,0x00,0x01,0x00,0x00,0x04,0x00,0x00,
		0x01,0x00,0x00,0x02,0x08,0x00,0x45,0x00,
		0x00,0x14,0x00,0x00,0x00,0x00,0xff,0x11,
		0x9d,0xbb,0x07,0x07,0x07,0x07,0x08,0x08,
		0x08,0x08,0x04,0x00,0x04,0x00,0x00,0x00,
		0xd9,0xd0
	};

	unsigned char pppoe_data[ ]=
	{
		0x00,0x00,0x01,0x00,0x00,0x04,0x00,0x00,
		0x01,0x00,0x00,0x02,0x88,0x64,0x11,0x00,
		0x00,0x00,0x00,0x00,0x00,0x21,0x45,0x00,
		0x00,0x14,0x00,0x00,0x00,0x00,0xff,0x11,
		0x9d,0xbb,0x07,0x07,0x07,0x07,0x08,0x08,
		0x08,0x08,0x04,0x00,0x04,0x00,0x00,0x00,
		0xd9,0xd0
	};

	QDMA_txMsgWord0_t msg0;
	QDMA_txMsgWord1_t msg1;
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d %d",&qdma_type,&pkt_type);

	if(((qdma_type != 1)&&(qdma_type != 2))||(pkt_type > 5))
		goto help;
	
	printk("send %s from %s to npu\n",pkt_type_name[pkt_type],(qdma_type == 1)?"QDMA_LAN":"QDMA_WAN");

	skb_put(skb,128);

	switch(pkt_type)
	{
		case 0:
			memmove(skb->data,ipoe_data,sizeof(ipoe_data));
			break;
		case 1:
			memmove(skb->data,ipoe_data,sizeof(ipoe_data));
			__vlan_put_tag(skb,0x64);
			break;
		case 2:
			memmove(skb->data,ipoe_data,sizeof(ipoe_data));			
			__vlan_put_tag(skb,0x64);
			__vlan_put_tag(skb,0x65);
			break;
		case 3:
			memmove(skb->data,pppoe_data,sizeof(pppoe_data));
			break;
		case 4:
			memmove(skb->data,pppoe_data,sizeof(pppoe_data));
			__vlan_put_tag(skb,0x64);
			break;
		case 5:
			memmove(skb->data,pppoe_data,sizeof(pppoe_data));
			__vlan_put_tag(skb,0x64);
			__vlan_put_tag(skb,0x65);
			break;
		default:
			break;
	}

	msg0.word = 0;
	msg1.word = 0;
	msg0.raw0.ico = 1;
	msg0.raw0.uco = 1;
	msg1.raw0.mtr_g = 0x7f;
	msg1.raw0.fport = 6;

	if(1 == qdma_type)
	{
		msg0.raw0.gem_port_id = 0x2345;
		msg1.raw0.acnt_g0 = 0x1f;
		QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN,skb,msg0.word,msg1.word,NULL);
	}
	else
	{
		msg0.raw0.gem_port_id = 0x3456;
		msg1.raw0.acnt_g1= 0x1;
		msg1.raw0.acnt_g0 = 0x2;
		QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_WAN,skb,msg0.word,msg1.word,NULL);
	}
	
	return count;

	help:
		printk("echo [qdma_type] [pkt_type] > /proc/tc3162/hwnat_send_pkt_to_npu\n");
		printk("[qdma_type]:1.QDMA_LAN 2.QDMA_WAN\n");
		printk("[pkt_type]:\n");
		printk("0.untag ipoe pkt\n");		
		printk("1.ingle tag ipoe pkt\n");
		printk("2.double tag ipoe pkt\n");
		printk("3.untag pppoe pkt\n");
		printk("4.single tag pppoe pkt\n");
		printk("5.double tag pppoe pkt\n");
	return count;
}
#endif

static unsigned char myip[16] = {0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static unsigned char dip[16] = {0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
static unsigned char seg[MAX_SRH_SEG_NUM][16] = { {0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11},
                                                  {0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22},
                                                  {0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33},
                                                  {0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44} };

int ecnt_hwnat_npu_srv6_hdr_structure(Npu_Srv6_Hdr_t *npu_srv6_hdr, unsigned int seg_num)
{
	int i = 0;
	unsigned char tmp_src_mac_up[] = {0x00,0x00,0x00,0xff,0xee,0xdd};
    unsigned char* tmp_dst_mac_up = NULL;

	if( seg_num > MAX_SRH_SEG_NUM ) {
		printk("seg_num = %d, should less than MAX_SRH_SEG_NUM = %d\n", seg_num, MAX_SRH_SEG_NUM);
		return -1;
	}

    memset(npu_srv6_hdr, 0, sizeof(Npu_Srv6_Hdr_t));

    tmp_dst_mac_up = GetMacAddr();
    for(i = 0; i < 6; i++) {
        npu_srv6_hdr->eth.h_dest[i] = tmp_dst_mac_up[i];
        npu_srv6_hdr->eth.h_source[i] = tmp_src_mac_up[i];
    }
    npu_srv6_hdr->eth.h_proto= htons(0x86DD);

	npu_srv6_hdr->ip6h.version = 6;
	npu_srv6_hdr->ip6h.nexthdr = 0x2B; /* IPv6-Route */
	npu_srv6_hdr->ip6h.hop_limit = 255;
	memcpy(npu_srv6_hdr->ip6h.saddr.s6_addr, myip, 16);
	memcpy(npu_srv6_hdr->ip6h.daddr.s6_addr, dip, 16);

	npu_srv6_hdr->srh.next_hdr = 0x04;
	npu_srv6_hdr->srh.rt_type = 0x04;
	npu_srv6_hdr->srh.flags = 0;

	if( seg_num > 0) {
		npu_srv6_hdr->srh.segs_left = seg_num - 1 ;
		npu_srv6_hdr->srh.first_seg = seg_num - 1;
		for(i = 0; i < seg_num; i++) {
			memcpy(npu_srv6_hdr->segment[i].s6_addr, &seg[i][0], 16);
		}

		npu_srv6_hdr->srh.hdr_len = (seg_num * 16) / 8; /* not include the first 8 octets */
	}
    
    return 0;
}

static int ipv6_addr_convert(unsigned char *to, unsigned int *from)
{
	int i = 0;

	for(i = 0; i < 8; i++)
	{
		printk("from[%d]:%04X ", i, from[i]);
		to[i*2] = (from[i] >> 8) & 0xFF;
		to[i*2 + 1] = from[i] & 0xFF;
	}
	printk("\n");

	return 0;
}

static int hwnat_npu_srv6_verify_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[128];
	char cmd[16];
	int idx = 0;
	int seg_num = 0;
	unsigned int ip_tmp[8]; 
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	memset(ip_tmp, 0, sizeof(ip_tmp));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%s %d %d ", cmd, &idx, &seg_num);

	if(!strcmp(cmd, "head")) {
		Npu_Srv6_Hdr_t npu_srv6_hdr;
		int total_len = 0;

		printk("head-%d: seg_num=%d\n", idx, seg_num);

		if(0 == ecnt_hwnat_npu_srv6_hdr_structure(&npu_srv6_hdr, seg_num)) {
			total_len = 14 + 40 + 8 + (16 * seg_num) ;
			tunnel_mail_store_srv6_hdr(idx, (unsigned char*)&npu_srv6_hdr, total_len);
		}
	} else if(!strcmp(cmd, "myip")) {
		sscanf(get_buf, "%s %x:%x:%x:%x:%x:%x:%x:%x ", cmd, &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3], &ip_tmp[4], &ip_tmp[5], &ip_tmp[6], &ip_tmp[7]);
		ipv6_addr_convert(myip, ip_tmp);
		
		printk("myip: %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n", 
			myip[0], myip[1], myip[2], myip[3], myip[4], myip[5], myip[6], myip[7],
			myip[8], myip[9], myip[10], myip[11], myip[12], myip[13], myip[14], myip[15]);
		tunnel_mail_set_srv6_myip(myip);
	} else if(!strcmp(cmd, "dip")) {
		sscanf(get_buf, "%s %x:%x:%x:%x:%x:%x:%x:%x ", cmd, &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3], &ip_tmp[4], &ip_tmp[5], &ip_tmp[6], &ip_tmp[7]);
		ipv6_addr_convert(dip, ip_tmp);
		
		printk("dip: %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n", 
			dip[0], dip[1], dip[2], dip[3], dip[4], dip[5], dip[6], dip[7],
			dip[8], dip[9], dip[10], dip[11], dip[12], dip[13], dip[14], dip[15]);
	} else if(!strcmp(cmd, "seg")) {
		sscanf(get_buf, "%s %d %x:%x:%x:%x:%x:%x:%x:%x ", cmd, &seg_num, &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3], &ip_tmp[4], &ip_tmp[5], &ip_tmp[6], &ip_tmp[7]);
		
		if((seg_num < 0) || (seg_num > 3))
			goto help;

		ipv6_addr_convert(&seg[seg_num][0], ip_tmp);
		
		printk("seg-%d: %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n", 
			seg_num, seg[seg_num][0], seg[seg_num][1], seg[seg_num][2], seg[seg_num][3], seg[seg_num][4], seg[seg_num][5], seg[seg_num][6], seg[seg_num][7],
			seg[seg_num][8], seg[seg_num][9], seg[seg_num][10], seg[seg_num][11], seg[seg_num][12], seg[seg_num][13], seg[seg_num][14], seg[seg_num][15]);
	}else {
		goto help;
	}

	return count;

help:
	printk("echo head [idx, 0~7] [seg_num, 0~4] > /proc/tc3162/hwnat_npu_srv6_verify\n");
	printk("echo myip/dip [2020:0000:0000:0000:0000:0000:0000:0001] > /proc/tc3162/hwnat_npu_srv6_verify\n");
	printk("echo seg [0~3] [2020:0000:0000:0000:0000:0000:0000:0001] > /proc/tc3162/hwnat_npu_srv6_verify\n");

	return count;
}

static int hwnat_npu_vxlan_verify_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[128];
	char cmd[16];
	int idx = 0, i = 0;
	Npu_Vxlan_Hdr_V4_t npu_vxlan_hdr_v4;
	unsigned char tmp_src_mac_up[] = {0x00,0x00,0x00,0xff,0xee,0xdd};
    unsigned char* tmp_dst_mac_up = NULL;
	uint8 vxlan_hdr[50] = { 
		0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x10, 0x94,0x00,0x00,0x02,0x08,0x00,0x45,0x00,\
		0x00,0x14,0x00,0x00,0x00,0x00,0xff,0x11, 0x3a,0x80,0x07,0x07,0x07,0x08,0x08,0x08,\
		0x08,0x0b,0x04,0x07,0x04,0x09,0x00,0x00, 0x3d,0x2c,0x08,0x00,0x00,0x00,0x00,0x00,\
		0x06,0x00
		};
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%s %d ", cmd, &idx);

	if(idx >= MAX_NPU_VXLAN_HDR_NUM) {
		goto help;
	}

	if(!strcmp(cmd, "add")) {
		memset(&npu_vxlan_hdr_v4, 0, sizeof(Npu_Vxlan_Hdr_V4_t));
		memmove(&npu_vxlan_hdr_v4, vxlan_hdr, VXLAN_HDR_SIZE);

		tmp_dst_mac_up = GetMacAddr();
		for(i = 0;i < 6;i++)
		{
			npu_vxlan_hdr_v4.eth.h_dest[i] = tmp_dst_mac_up[i];
			npu_vxlan_hdr_v4.eth.h_source[i] = tmp_src_mac_up[i];
		}
		npu_vxlan_hdr_v4.eth.h_proto= htons(0x0800);
		npu_vxlan_hdr_v4.uh.source = htons(0x12B5);
		npu_vxlan_hdr_v4.uh.dest = htons(0x12B5);

	    tunnel_mail_store_vxlan_hdr(idx, (unsigned char*)&npu_vxlan_hdr_v4);
	} else {
		goto help;
	}

	return count;

help:
	printk("echo add [idx, 0~%d] > /proc/tc3162/hwnat_npu_vxlan_verify\n", MAX_NPU_VXLAN_HDR_NUM-1);

	return count;
}

static int hwnat_npu_frag_mtu_verify_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	int idx = 0;
	int mtu = 0;

	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;

	memset(get_buf, 0, sizeof(get_buf));

	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d %d ", &idx, &mtu);

	tunnel_mail_set_frag_mtu(idx, mtu);

	return count;
}

static int hwnat_npu_debug_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	char cmd[32];
	NPU_BRIDGE_DEBUG_CMD_t debug_cmd = 0;
	int debug_level = 0;

	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;

	memset(get_buf, 0, sizeof(get_buf));

	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%s %d ", cmd, &debug_level);

	if(!strcmp(cmd, "dump"))
	{
		debug_cmd = NPU_BRIDGE_DEBUG_CMD_DUMP_CNT;
		printk("FE: \n");
		printk("CDMA3_FWD_CFG         (0x%08x) = 0x%08x\n", CDMA3_FWD_CFG, RegRead(CDMA3_FWD_CFG));
		printk("CDMA3_TSO_SHPR_CFG    (0x%08x) = 0x%08x\n", CDMA3_TSO_SHPR_CFG, RegRead(CDMA3_TSO_SHPR_CFG));
		printk("CDMA3_TSO_FLAG_MASK   (0x%08x) = 0x%08x\n", CDMA3_TSO_FLAG_MASK, RegRead(CDMA3_TSO_FLAG_MASK));
		printk("CDMA3_TX_OK_CNT       (0x%08x) = 0x%08x\n", CDMA3_TX_OK_CNT, RegRead(CDMA3_TX_OK_CNT));
		printk("CDMA3_RXCPU_KA_CNT    (0x%08x) = 0x%08x\n", CDMA3_RXCPU_KA_CNT, RegRead(CDMA3_RXCPU_KA_CNT));
		printk("CDMA3_RX_OQ0_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ0_OK_CNT, RegRead(CDMA3_RX_OQ0_OK_CNT));
		printk("CDMA3_RX_OQ1_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ1_OK_CNT, RegRead(CDMA3_RX_OQ1_OK_CNT));
		printk("CDMA3_RX_OQ2_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ2_OK_CNT, RegRead(CDMA3_RX_OQ2_OK_CNT));
		printk("CDMA3_RX_OQ3_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ3_OK_CNT, RegRead(CDMA3_RX_OQ3_OK_CNT));
		printk("CDMA3_RX_OQ4_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ4_OK_CNT, RegRead(CDMA3_RX_OQ4_OK_CNT));
		printk("CDMA3_RX_OQ5_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ5_OK_CNT, RegRead(CDMA3_RX_OQ5_OK_CNT));
		printk("CDMA3_RX_OQ6_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ6_OK_CNT, RegRead(CDMA3_RX_OQ6_OK_CNT));
		printk("CDMA3_RX_OQ7_OK_CNT   (0x%08x) = 0x%08x\n", CDMA3_RX_OQ7_OK_CNT, RegRead(CDMA3_RX_OQ7_OK_CNT));
		printk("CDMA3_RX_OQ0_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ0_DROP_CNT, RegRead(CDMA3_RX_OQ0_DROP_CNT));
		printk("CDMA3_RX_OQ1_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ1_DROP_CNT, RegRead(CDMA3_RX_OQ1_DROP_CNT));
		printk("CDMA3_RX_OQ2_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ2_DROP_CNT, RegRead(CDMA3_RX_OQ2_DROP_CNT));
		printk("CDMA3_RX_OQ3_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ3_DROP_CNT, RegRead(CDMA3_RX_OQ3_DROP_CNT));
		printk("CDMA3_RX_OQ4_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ4_DROP_CNT, RegRead(CDMA3_RX_OQ4_DROP_CNT));
		printk("CDMA3_RX_OQ5_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ5_DROP_CNT, RegRead(CDMA3_RX_OQ5_DROP_CNT));
		printk("CDMA3_RX_OQ6_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ6_DROP_CNT, RegRead(CDMA3_RX_OQ6_DROP_CNT));
		printk("CDMA3_RX_OQ7_DROP_CNT (0x%08x) = 0x%08x\n", CDMA3_RX_OQ7_DROP_CNT, RegRead(CDMA3_RX_OQ7_DROP_CNT));
		printk("\nNPU: \n");
	}
	else if(!strcmp(cmd, "clear"))
	{
		debug_cmd = NPU_BRIDGE_DEBUG_CMD_CLEAR_CNT;
	}
	else if(!strcmp(cmd, "release"))
	{
		debug_cmd = NPU_BRIDGE_DEBUG_CMD_RELEASE_BUFF;
	}
	else if(!strcmp(cmd, "level"))
	{
		debug_cmd = NPU_BRIDGE_DEBUG_CMD_SET_LEVEL;
	}
	else if(!strcmp(cmd, "loopback"))
	{
		debug_cmd = NPU_BRIDGE_DEBUG_CMD_SET_LOOPBACK;
	}
	else
	{
		printk("echo dump/clear/release/level(0/1/2/3)/loopback(0/1/2) > /proc/tc3162/hwnat_npu_debug \n");
		return -EFAULT;
	}

	tunnel_mail_set_npu_bridge_debug(debug_cmd, debug_level);

	return count;
}

int ecnt_hwnat_npu_store_vxlan_hdr(struct sk_buff* skb,unsigned int type)
{
    Npu_Vxlan_Hdr_V4_t npu_vxlan_hdr_v4;
	unsigned char tmp_src_mac_up[] = {0x00,0x00,0x00,0xff,0xee,0xdd};
    unsigned char* tmp_dst_mac_up = NULL;
    int i = 0;
    int first_emtpy_id = -1;

	if(SUPPORT_TUNNEL_HW_ACCELE)
		return 0;

    if(!skb->dev)
        return -1;

    for(i = 0;i < MAX_NPU_VXLAN_HDR_NUM;i++)
    {
        if((npu_vxlan_hdr_valid[i].valid)&&(!strcmp(npu_vxlan_hdr_valid[i].ifname,skb->dev->name)))
            return 0;

        if((-1 == first_emtpy_id)&&(0 == npu_vxlan_hdr_valid[i].valid))
            first_emtpy_id = i;
    }

    if(-1 == first_emtpy_id)
        return -1;

    npu_vxlan_hdr_valid[first_emtpy_id].valid = 1;
    strcpy(npu_vxlan_hdr_valid[first_emtpy_id].ifname,skb->dev->name);

    memset(&npu_vxlan_hdr_v4,0,sizeof(Npu_Vxlan_Hdr_V4_t));

    if(0 == type)/*ipv4*/
    {
		memmove(&(npu_vxlan_hdr_v4.iph),skb->data,sizeof(struct iphdr));
		skb_pull(skb, sizeof(struct iphdr));
		memmove(&(npu_vxlan_hdr_v4.uh),skb->data,sizeof(struct udphdr));
		skb_pull(skb, sizeof(struct udphdr));
		memmove(&(npu_vxlan_hdr_v4.vxlanh),skb->data,sizeof(struct vxlanhdr));
		skb_push(skb, sizeof(struct iphdr)+sizeof(struct udphdr));
    }
    else
        return -1;

    tmp_dst_mac_up = GetMacAddr();
    for(i = 0;i < 6;i++)
    {
        npu_vxlan_hdr_v4.eth.h_dest[i] = tmp_dst_mac_up[i];
        npu_vxlan_hdr_v4.eth.h_source[i] = tmp_src_mac_up[i];
    }
    npu_vxlan_hdr_v4.eth.h_proto= htons(0x800);


    tunnel_mail_store_vxlan_hdr(first_emtpy_id,(unsigned char*)&npu_vxlan_hdr_v4);
    
    return 0;
}

static  void* ppeVxlanHdrSessionListAlloc(int size)
{
	void* ptr = NULL;
	
	if (size>0)
	{
		ptr = kmalloc(size, GFP_ATOMIC);
		if(ptr == NULL) {
			printk("Alloc memory for ppeVxlanHdrSessionListAlloc failed\n") ;
			return NULL ;
		}
		memset(ptr,0,size);
	}
	
	return ptr;
} 

static void ppeVxlanHdrSessionListFree(void* ptr)
{
	if (ptr)
	{
		kfree(ptr);
		ptr = NULL;
	} 
}

int ecnt_hwnat_npu_add_session_to_vxlan_hdr_list(unsigned char* if_name,unsigned int foe_num)
{
    Npu_Vxlan_Hdr_Session_t *entry_tmp = NULL;
    int i = 0;

    for(i = 0;i < MAX_NPU_VXLAN_HDR_NUM;i++)
    {
        if((npu_vxlan_hdr_valid[i].valid)&&(!strcmp(npu_vxlan_hdr_valid[i].ifname,if_name)))
            break;
    }

    if(MAX_NPU_VXLAN_HDR_NUM == i)
        return -1;

	write_lock(&vxlan_hdr_lock);
    
    list_for_each_entry(entry_tmp, &(npu_vxlan_hdr_valid[i].session_list), list)	 
    {
        if(entry_tmp->foe_num == foe_num)
        {
            printk("same foe_num %d exsit in %s hdr list,add fail\n",foe_num,if_name);
			write_unlock(&vxlan_hdr_lock);
            return -1;
        }
    }

    entry_tmp = (Npu_Vxlan_Hdr_Session_t*)ppeVxlanHdrSessionListAlloc(sizeof(Npu_Vxlan_Hdr_Session_t));
    if(entry_tmp)
    {
        entry_tmp->foe_num = foe_num;
        list_add_tail(&entry_tmp->list,&(npu_vxlan_hdr_valid[i].session_list));
    }
	write_unlock(&vxlan_hdr_lock);
    
    return 0;    
}

int ecnt_hwnat_npu_check_vxlan_hdr_list(void)
{
    int i = 0;        
    Npu_Vxlan_Hdr_Session_t *entry = NULL;
    Npu_Vxlan_Hdr_Session_t *entry_tmp = NULL;
    int session_exist = 0;

    for(i = 0;i < MAX_NPU_VXLAN_HDR_NUM;i++)
    {
        if(!npu_vxlan_hdr_valid[i].valid)
            continue;

        write_lock(&vxlan_hdr_lock);
        list_for_each_entry_safe(entry,entry_tmp, &(npu_vxlan_hdr_valid[i].session_list), list)
        {
            if(!hwnat_check_entry_is_bind(entry->foe_num))
            {
                list_del(&entry->list);
                ppeVxlanHdrSessionListFree(entry);
            }
            session_exist = 1;
        }

        if(list_empty(&(npu_vxlan_hdr_valid[i].session_list)))
        {
            npu_vxlan_hdr_valid[i].valid = 0;
            if(session_exist)
                tunnel_mail_release_vxlan_hdr(i);
        }
        
        write_unlock(&vxlan_hdr_lock);        
    }
    
	return 0;	
}

static int ecnt_hwnat_npu_restore_pingpong_info(struct sk_buff* skb,unsigned short cpu_info)
{
    foe_ext[FOE_ENTRY_NUM(skb)].npu_pingpong = 1;
    foe_ext[FOE_ENTRY_NUM(skb)].npu_pingpong_data.raw = cpu_info;
    
    return 0;
}

static int hwnat_npu_offload_proto_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[16];
	unsigned int value = 0;

    if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%x",&value);

	printk("set npu offload proto value %x\n",value);

    npu_tunnel_offload_proto = value;
	
	return count;
}	

static int hwnat_npu_offload_proto_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("npu_tunnel_offload_proto = %x\n",npu_tunnel_offload_proto);
    printk("bit1:vxlan\n");
	return 0;	
}

static int hwnat_npu_offload_vxlan_hdr_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
    int i = 0;    
    Npu_Vxlan_Hdr_Session_t *entry_tmp = NULL;

    for(i = 0;i < MAX_NPU_VXLAN_HDR_NUM;i++)
    {
        if(!npu_vxlan_hdr_valid[i].valid)
            continue;

        printk("index:%d\n",i);
        printk("interface:%s\n",npu_vxlan_hdr_valid[i].ifname);
        printk("foe num list:\n");
        
        read_lock_bh(&vxlan_hdr_lock);
        list_for_each_entry(entry_tmp, &(npu_vxlan_hdr_valid[i].session_list), list)
            printk("%d\n",entry_tmp->foe_num);
        read_unlock_bh(&vxlan_hdr_lock);
        
        printk("------------------------------\n");
    }
    
	return 0;	
}

static int ecnt_hwnat_npu_offload_vxlan_enable(void)
{
    return npu_tunnel_offload_proto&NPU_OFFLOAD_VXLAN;
}

void ecnt_hwnat_npu_init(void)
{	    
	struct proc_dir_entry *hwnat_proc;
    int i = 0;
	#if NPU_BRIGE_VERIFY        
	hwnat_proc = create_proc_entry("tc3162/hwnat_add_static_rule", 0, NULL);
	hwnat_proc->write_proc =  hwnat_add_static_rule_write_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_send_pkt_to_npu", 0, NULL);
	hwnat_proc->write_proc =  hwnat_send_pkt_to_npu_write_proc;
	#endif

    if(!TCSUPPORT_NPU_TUNNEL_OFFLOAD_VAL) {
        return;
    }

    hwnat_proc = create_proc_entry("tc3162/hwnat_npu_offload_proto", 0, NULL);
	hwnat_proc->write_proc =  hwnat_npu_offload_proto_write_proc;
    hwnat_proc->read_proc = hwnat_npu_offload_proto_read_proc;

    hwnat_proc = create_proc_entry("tc3162/hwnat_npu_offload_vxlan_hdr", 0, NULL);
    hwnat_proc->read_proc = hwnat_npu_offload_vxlan_hdr_read_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_npu_srv6_verify", 0, NULL);
	hwnat_proc->write_proc =  hwnat_npu_srv6_verify_write_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_npu_vxlan_verify", 0, NULL);
	hwnat_proc->write_proc =  hwnat_npu_vxlan_verify_write_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_npu_frag_mtu_verify", 0, NULL);
	hwnat_proc->write_proc =  hwnat_npu_frag_mtu_verify_write_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_npu_debug", 0, NULL);
	hwnat_proc->write_proc =  hwnat_npu_debug_write_proc;

    npu_tunnel_offload_proto |= NPU_OFFLOAD_VXLAN;

    memset(npu_vxlan_hdr_valid,0,MAX_NPU_VXLAN_HDR_NUM*sizeof(Npu_Vxlan_Hdr_valid_t));
    for(i= 0;i < MAX_NPU_VXLAN_HDR_NUM;i++)    
	    INIT_LIST_HEAD(&(npu_vxlan_hdr_valid[i].session_list));

    ra_sw_nat_npu_offload_vxlan_enable = ecnt_hwnat_npu_offload_vxlan_enable;
    ra_sw_nat_npu_store_vxlan_hdr = ecnt_hwnat_npu_store_vxlan_hdr;
    ra_sw_nat_restore_npu_pingpong_info = ecnt_hwnat_npu_restore_pingpong_info;
    ra_sw_nat_npu_check_vxlan_hdr_list_hook = ecnt_hwnat_npu_check_vxlan_hdr_list;
	
	return;
}

void ecnt_hwnat_npu_deinit(void)
{
	#if NPU_BRIGE_VERIFY
	remove_proc_entry("tc3162/hwnat_add_static_rule",NULL);	
	remove_proc_entry("tc3162/hwnat_send_pkt_to_npu",NULL);
	#endif

    if(!TCSUPPORT_NPU_TUNNEL_OFFLOAD_VAL) {
        return;
    }

	remove_proc_entry("tc3162/hwnat_npu_offload_proto",NULL);
	remove_proc_entry("tc3162/hwnat_npu_offload_vxlan_hdr",NULL);
	remove_proc_entry("tc3162/hwnat_npu_srv6_verify",NULL);
	remove_proc_entry("tc3162/hwnat_npu_vxlan_verify",NULL);
	remove_proc_entry("tc3162/hwnat_npu_frag_mtu_verify",NULL);
	remove_proc_entry("tc3162/hwnat_npu_debug",NULL);

    ra_sw_nat_npu_offload_vxlan_enable = NULL;
    ra_sw_nat_npu_store_vxlan_hdr = NULL;
    ra_sw_nat_restore_npu_pingpong_info = NULL;
    ra_sw_nat_npu_check_vxlan_hdr_list_hook = NULL;
	
	return;
}

