 /************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/spinlock.h>
#include <linux/spinlock_types.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/libcompileoption.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <net/mtk_esp.h>
#include <linux/foe_hook.h>
#include <linux/proc_fs.h>
#include <net/ipv6.h>
#include <net/ip.h>
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>

#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_lro.h>
#include <ecnt_hook/ecnt_hook_crypto.h>
#include <ecnt_hook/ecnt_hook_asym.h>
#include "ecnt_hook/ecnt_hook_tso.h"
#include <ecnt_hook/ecnt_hook_ifc_type.h>

#include <modules/ra_hwnat/hwnat_npu_tunnel_offload.h>

#include "ra_nat_offload.h"
#include "ra_nat_gen.h"
#include "ra_nat_dev.h"
#include "ra_nat_ety.h"
#include "ra_nat_cust.h"
#include "ra_nat_ic_dis.h"
#include "util.h"
#include "foe_fdb.h"
#include "ra_nat_ic_dis.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef CONFIG_TP_IMAGE
#define USB_4G_DEV	"lte0"
#endif

struct dsOffloadFtp_t dsOffloadFtp = {0};
struct SoftQdmaUDFTcpPort udfTcpPort = {0};
uint32 pkttype = 0;  

unsigned int ds_need_offload = 0;

int rx_wifi_len = 0;
int rx_wifi_len_counter = 0;

static int rps_enable = 0;
int dualband_interrupt_enable = 1;
int dualband_index_tmp = 0;
int dualband_interrupt_enable_V2 = 0;

struct net_device	*DstPort[MAX_IF_NUM];

int dlf_control_enable =  0; 	/*switch for dlf function*/
static struct list_head DLFEntryTable ;
static struct list_head DLFNFEntryTable;
static struct timer_list dlf_update_timer;
DEFINE_SPINLOCK(dlf_fdb_lock);
DEFINE_SPINLOCK(dlf_nf_lock);
static DEFINE_RWLOCK(Dportlock);

int us_meter_pps=0;
int ds_meter_pps=0;
int us_meter_pps_buffer=0;
int ds_meter_pps_buffer=0;
int timer_for_1s=0;
extern int setPPeRxACNTGRP(struct sk_buff * skb);

static int hwnat_local_in_enable = 1;
static int hwnat_local_in_dport_all = 0;
static struct list_head local_in_dport_list;
#ifdef CONFIG_TP_IMAGE
static struct list_head local_in_dport_backup_list;
#endif /* CONFIG_TP_IMAGE */

static l2tp_important_info l2tp_rx;
static l2tp_important_info l2tp_tx;
struct net_device *l2tp_vpn_dev;/*ppp dev for l2tp*/

/*TCSUPPORT_VLAN_ACCESS_TRUNK start*/
static DEFINE_SPINLOCK(wan_mcast_vlan_lock);
struct net_device	*g_veth_lan_device[4095];
struct list_head	hwnat_wan_mcast_vlan_list[DP_MCAST_PPP7 - DP_MCAST_NAS0 + 1];
/*TCSUPPORT_VLAN_ACCESS_TRUNK end*/

extern struct net *__get_net_ns_by_pid(pid_t pid);
extern void wdog_kick_api(int source);
static unsigned int NS_PID = 0;
module_param(NS_PID, uint, 0);
unsigned char dlf_flag = 0;
unsigned char unknown_mul_flag = 0;

static gre_tx_info gre_tx;
static vxlan_tx_info vxlan_tx;
struct VxlanLink g_vxlan_link[VXLAN_MAX_LINK_NUM];

static int rtsp_v6_alg_offload = 0;/*default disable*/
static int ppe_vpnenable = 0;/*default disable*/

struct sk_buff *ppe_vlan_put_tag(struct sk_buff *skb, u16 vlan_tci);

struct offload_softnet_data{
    struct sk_buff_head     process_queue;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
    struct call_single_data csd;
#else
    call_single_data_t csd;
#endif

    unsigned int          processed;
    unsigned int        dropped;
    unsigned int          max_queue_len;
    unsigned int        input_num;
    struct sk_buff_head     input_pkt_queue;
    struct napi_struct      napi;
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
extern void smp_call_function_single_ext(int cpu, struct call_single_data *csd);
#else
extern void smp_call_function_single_ext(int cpu, call_single_data_t *csd);
#endif

extern int ppeIsIfcHit(unsigned int foe_num,  struct FoeEntry * foe_entry);
static void ppePPPOEHeaderLengthTranslate(struct sk_buff* skb);

static int enqueue_to_cpu_queue(struct sk_buff *skb, int cpu, int queue);
static int offload_rps_cpu = -1;
static int offload_rps_max_queue = 4000;
static struct offload_softnet_data pkt_queue[4];
static int offload_rps_handle(struct sk_buff *skb);
NATV6_WAN_RATE_s natv6_wan_rate[2];

int ecnt_wlan_force_to_cpu_handler(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_ipsec_passthrough(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_ds_offload(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_local_in(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_multicast_hwnat_force_to_cpu_handler(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_soft_ratelimit(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_gre_vxlan_wan_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_gre_tunnel_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_vxlan_tunnel_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_asym_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_default_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_natv6(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_hit_ifc(struct sk_buff* skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);
int ecnt_offload_ookla(struct sk_buff* skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);

typedef int (*force_to_cpu_opt)(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry);

struct net_device* vxlan_getlink(int vni);

force_to_cpu_opt force_to_cpu_handler[ ] =
{
	ecnt_wlan_force_to_cpu_handler,
	ecnt_offload_ipsec_passthrough,
	ecnt_offload_ds_offload,
	ecnt_offload_local_in,
	ecnt_multicast_hwnat_force_to_cpu_handler,
	ecnt_offload_soft_ratelimit,
	ecnt_offload_gre_vxlan_wan_offload,
	ecnt_offload_gre_tunnel_offload,
    	ecnt_offload_vxlan_tunnel_offload,
    	ecnt_offload_asym_offload,
	ecnt_offload_default_offload,		
    ecnt_offload_natv6,
    ecnt_offload_hit_ifc,
    ecnt_offload_ookla,
};

#if defined  (CONFIG_RA_HW_NAT_VPN_PASSTHROUGH)
struct net_device *vpn_decrypt_src_dev[DP_CRYPTO_D_MAX - DP_CRYPTO_D_0] = {0};
#endif

#define	QDMA_RX_RING_FOR_DLF	(SUPPORT_QDMA_32_RX_RING?RING_IDX_9:RING_IDX_2)

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int DebugLevel;
extern unsigned int hwnat_dram_idx_base;

extern PktParseResult PpeParseResult;

extern int (*wifi_eth_fast_tx_hook)(struct sk_buff *skb);
extern int (*offload_eth_fast_tx_hook)(struct sk_buff *skb, int channel);
extern int (*ra_sw_nat_get_wifi_dev_name) (struct sk_buff * skb);

extern struct FoeEntry		*PpeFoeBase;
extern struct spinlock hw_nat_lock;

extern int wifiqueue_start_idx;
extern int macqueue_start_idx;

extern struct FoeEntryExt*	foe_ext;

extern br_ip br_ip_info;

/*TCSUPPORT_MULTI_SWITCH_EXT start*/
unsigned int max_eth_itf_num = 6;
extern eth_port_vlan_t eth_port_vlan[];
extern char extend_switch_port;
/*TCSUPPORT_MULTI_SWITCH_EXT end*/
extern char switch_port_map[];
/**dev name define   come from net_utility **/
extern int isLANInterface(struct net_device* dev);
extern int isWANInterface(struct net_device* dev);
extern int isPONInterface(struct net_device* dev);

extern int isPPPInterface(struct net_device* dev);
extern int isPPTPInterface(struct net_device* dev);
extern int isL2TPInterface(struct net_device* dev);

/**dev name define   come from net_utility **/

extern int (*ra_sw_nat_local_in_tx) (struct sk_buff * skb,unsigned short port);
extern int (*ra_hit_bind_force_to_cpu) (struct sk_buff *skb);
extern int (*ra_sw_nat_mcst_offload)(struct sk_buff *skb, int *dp);
extern int (*ra_sw_nat_ds_offload)(struct sk_buff *skb, int *dp);

extern int (*wan_speed_test_pinpong_handle_hook)(struct sk_buff*);
extern int (*wan_tr471_pinpong_handle_hook)(struct sk_buff*);

extern int (*soft_ratelimit_enqueue_hook) (struct sk_buff * skb,unsigned int queue_idx);

//TCSUPPORT_WLAN_SW_RPS
extern int (*toWlan5GPktRpsHandle_hook)(struct sk_buff *skb);
extern int (*toWlan2GPktRpsHandle_hook)(struct sk_buff *skb);
extern int TxShortFlag;
extern int (*hwnat_gre_fast_down_hook)(struct sk_buff* skb);
extern int (*hwnat_gre_fast_up_hook)(struct sk_buff* skb);

extern uint32 dsoffload_presetting;

extern int ppeSupportPreBind;
extern int ecnt_multicast_hwnat_drop_flow(struct sk_buff* skb);

extern unsigned char NATv6_USE_HIGH_PRIORITY;

extern void PpeSetRxForceRingIdx(struct FoeEntry * foe_entry, uint8_t ring_index);
extern void PpeSetEntryBind(struct sk_buff *skb, struct FoeEntry *foe_entry);
extern int hwnat_skb_to_foe(struct sk_buff* skb);
extern void PpeSetInfoBlk2(struct FoeEntry * foe_entry, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag, uint32_t ds_to_qdma);
extern int hwnat_delete_foe_entry(int index);
extern int ecnt_ppe_meter_add (struct ppe_meter_cfg *meterCfg);
extern int localInShortCut(struct sk_buff *skb, int protocol);
extern int32_t PpeTxHandler(struct sk_buff * skb, struct port_info * pinfo, int magic);
extern uint8 *GetMacAddr(void);
extern int getWifiQueueIndex(struct net_device *dev,unsigned char direction);
extern void PpeBlack_ip_foe_handle(struct sk_buff *skb, int direction);
extern uint8 ptm_etherwan_coexist;
extern void PpeAWifiClearAWifiInfo(void);
extern void SetEtherWANQdma(int enable);
extern void PpeClearEntryInfo(struct FoeEntry *foe_entry);
/*TCSUPPORT_VLAN_ACCESS_TRUNK start*/
extern int  (*multicast_xmit_packet_hook)(struct sk_buff* skb, unsigned short vid, 
                                          struct net_device *skip_eth_dev, unsigned short skip_eth_vid);
extern int PpeL2luKeySetStatusBind(int PpeIndex);
/*TCSUPPORT_VLAN_ACCESS_TRUNK end*/
extern void wdog_kick_api(int source);
extern void*  ppe_get_network_header(struct sk_buff* skb);
extern int ppe_is_rtspv6_response(struct sk_buff* skb);
extern int PpeFreeHandler(struct sk_buff * skb);
extern int (*soft_ratelimit_allocatequeue_hook)( unsigned int num);
extern int (*soft_ratelimit_set_queue_hook)(unsigned int queue,unsigned int rate,int(* func)(struct sk_buff * skb));
extern int (*soft_ratelimit_recyclequeue_hook)( unsigned int start,unsigned num);

extern __IMEM struct net_bridge_fdb_entry *__br_fdb_get(struct net_bridge *br,const unsigned char *addr,__u16 vid);

extern int ppe_get_packet_type(struct sk_buff* skb, int *offlen);
extern void hwnat_offload_mark_lan_rx(unsigned int idx, unsigned short stag);
extern int ppe_get_vlan_tag_num(struct sk_buff* skb);

extern int (*ookla_offload_rcv_hook)(struct sk_buff *skb);
extern int (*ookla_offload_pingpong_hook)(struct sk_buff *skb);
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/***************softqdma start***********************/
void getftpIpPort(const char *data, int datalen, int plen)
{
	int i = 0;
	u_int32_t array[6];
	
	for (i = plen; data[i] != '('; i++)
		if (i == datalen - 1) return;
		
	i++;

	ppe_try_get_number(data + i, datalen - i, array, 6, ',', ')');

	dsOffloadFtp.ipaddr =  htonl((array[0] << 24) | (array[1] << 16) |
				    (array[2] << 8) | array[3]);
	dsOffloadFtp.port = htons((array[4] << 8) | array[5]);
}

void FtpCtlPacketHandle(struct sk_buff* skb)
{
	int offlen, datalen;
	unsigned int eth_type = ppe_get_packet_type(skb, &offlen);
	struct iphdr*  ih;
	struct tcphdr *th = NULL;
	const char *pattern = "227 ";
	char *cp = NULL;

	if (eth_type!=PACKET_IPV4)
	{
		return;
	}
	
	ih = (struct iphdr* )(skb->data + offlen);
	if (ih->protocol != IPPROTO_TCP) {
		return;
	}
	
	th = (struct tcphdr *)((uint8_t *)ih + (ih->ihl * 4));
	if(21 != th->source){
		return;
	}
	
	cp = (char *)th;
	if (0 != strnicmp(cp + 20, pattern, sizeof(pattern) - 1)) {
		return;
	}
	
	datalen = skb->data_len - offlen - 40;
	getftpIpPort(cp, datalen, sizeof(pattern) - 1);

	return;
}

void checkTcpSrcPort(u_int16_t port)
{
	int i;
	
	if(80 == port){
		pkttype |= DS_HTTP80;
	}
	else if(8080 == port){
		pkttype |= DS_HTTP8080;
	}
	for(i=0; i<udfTcpPort.udfPortNum; i++)
	{
		if(i >= USERDEFINEDNUM)
			return;
		if(udfTcpPort.udfTcpPort[i] == port)
			pkttype |= DS_UDFPORT;
	}
	
	return;
}

void ipv4TcpPkttypeMark(struct iphdr *iph, struct tcphdr *th, struct sk_buff * skb)
{
	if(((iph->saddr == dsOffloadFtp.ipaddr)&&(th->source == dsOffloadFtp.port))||20 == th->source){
		pkttype |= DS_FTP;
	}
	else if((RTSP_PORT == th->source)&&(ROUTE_INTERFACE == skb->bridge_flag)){	/* RTSP && route */
		pkttype |= RTSP;
	}
	else
		checkTcpSrcPort(th->source);
	
	pkttype |= DS_TCP;

	return;
}

void ipv6TcpPkttypeMark(struct tcphdr *th)
{
	if(20 == th->source){
		pkttype |= DS_FTP;
	}
	else
		checkTcpSrcPort(th->source);
	
	pkttype |= DS_TCP;

	return;
}

static int softqdma_udf_port_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int i;
	
	printk("userDefinedNum %d\n\n",udfTcpPort.udfPortNum);

	printk("userDefinedPort:\n");

	for(i=0;(i<udfTcpPort.udfPortNum)&&(i<USERDEFINEDNUM);i++)
	{
		printk("                %d\n", udfTcpPort.udfTcpPort[i]);
	}

	return 0;	
}

static int softqdma_udf_port_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[128];
	int i, num = 0, port[USERDEFINEDNUM] = {0};
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
	&num,&port[0],&port[1],&port[2],&port[3],&port[4],&port[5],&port[6],&port[7],
	&port[8],&port[9],&port[10],&port[11],&port[12],&port[13],&port[14],&port[15]);

	udfTcpPort.udfPortNum = (uint8_t)num;
	for(i=0;(i<udfTcpPort.udfPortNum)&&(i<USERDEFINEDNUM);i++)
	{
		udfTcpPort.udfTcpPort[i] = (uint16_t)port[i];
	}
	
	return count;
}

void softQdmaSetForcePort(struct FoeEntry * foe_entry, struct port_info * pinfo, u_int16_t meterGrpId, u_int16_t acntGrpIndex)
{
    if((pkttype & dsoffload_presetting) & PROTOCOL_WHITE_LIST)
    {
        ds_need_offload = 1;
    }
    else/* 100M port or en7512 100M/1G port */
    {
        ds_need_offload = pkttype & dsoffload_presetting;
    }
    
    if(ds_need_offload)
    {
	PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);

        if(isEN751627)
        {
            PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
        }
    }
    else
    {   
        if(pinfo->ds_to_qdma)
        {
            PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
        }
        else
        {
            PpeSetInfoBlk2(foe_entry, FP_GDMA1, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
        }
    }

	return;

}

/**************softqdma end************************************/

/**************dlf start****************************************/
int hwnat_is_alive_index(int index)
{
	struct FoeEntry *foe_entry ;

	if (index <0 || index >= FOE_4TB_SIZ_TOTAL)
		return 0;

	//foe_entry = &PpeFoeBase[index];
	foe_entry = getPpeEntry(index);
	if(foe_entry == NULL)
		return -1;

	if (foe_entry->bfib1.state != BIND)
		return 0;

	return 1;
}

inline int PpeDLFRxHandler(struct sk_buff * skb)
{
	int vlan_layer=0, tpid=0;

	/*if the flow is bind , no need do anything*/
	if( (!dlf_control_enable) || hwnat_is_alive_index(FOE_ENTRY_NUM(skb)) )
		return 0 ;

	//save vlan info into dlf_resv only for dlf stream rate limit
	//skb->dlf_resv0 : special tag    /*saved in eth_lan & femac*/
	//skb->dlf_resv1 : bit0~15:vlan 1 ; bit16~31 :vlan2
	//skb->dlf_resv2 : bit 0-1:vpm  ; bit 2-3 :vlan_layer
	/*init resv1&resv2*/

	skb->dlf_resv1 = 0 ; skb->dlf_resv2 = 0 ; 
    if(!(TCSUPPORT_MULTI_SWITCH_EXT_VAL && (skb->dlf_resv0 & 0x7) == extend_switch_port)){
        skb->dlf_resv1 = 0 ; 
	/*store vlan1 and vlan2*/
	tpid = skb->protocol;
	if((tpid == htons(0x8100)) ||(tpid == htons(0x9100)) ||(tpid == htons(0x88a8)))
	{
		vlan_layer+=1;
		skb->dlf_resv1 = *(uint16 *)(skb->data);
	}
    }
    if(TCSUPPORT_MULTI_SWITCH_EXT_VAL && (skb->dlf_resv0 & 0x7) == extend_switch_port){
        vlan_layer+=1;
        tpid = skb->protocol;
    }
    else
	tpid = *(uint16 *)(skb->data+2);
	if((tpid==htons(0x8100)) || (tpid==htons(0x9100)) || (tpid==htons(0x88a8)))
	{
		vlan_layer+=1;
		skb->dlf_resv1 %= 0xFFFF;
        if(TCSUPPORT_MULTI_SWITCH_EXT_VAL && (skb->dlf_resv0 & 0x7) == extend_switch_port)
            skb->dlf_resv1 |= *(uint16 *)(skb->data)<<16;
        else
		skb->dlf_resv1 |= *(uint16 *)(skb->data+4)<<16;
	}
	/*store vpm and vlan_layer*/
	if(vlan_layer)
	{
		switch(skb->protocol)
		{
			case htons(0x8100):
				skb->dlf_resv2 = 1 ;
				break;
			case htons(0x88a8):
				skb->dlf_resv2 = 2 ;
				break;
			default:
				skb->dlf_resv2 = 3 ;
				break;
		}
		skb->dlf_resv2 |= (vlan_layer & 0x3) << 2 ;
	} 

	if (DebugLevel >= 3) 
	{
		printk("PpeDLFRxHandler:index = %d , resv0= %x , resv1= %x , resv2= %x\n" , FOE_ENTRY_NUM(skb) ,
			skb->dlf_resv0 , skb->dlf_resv1 , skb->dlf_resv2);
	}

	return 0 ;
}

static inline int32_t PpeDLFFillInfo_for_L2B(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	if((isLANInterface(skb->dev)) || (FOE_SP(skb) == 3)){
		// upstream use meter for DLF  ratelimit
        if(isEN7580){
            foe_entry->l2_bridge.ts_id = HWNAT_DEFAULT_MTRGRP;
			if(dlf_flag)/*set All to QDMA_WAN for CDS All ratelimit*/
			{
				PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3f, 0x3f, 1);
			}
			else{
	            PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
			}
			PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
			foe_entry->l2_bridge.bfib1.vpm = skb->dlf_resv2 & 0x3;
        }
		else{
		PpeSetInfoBlk2(foe_entry, FP_PDMA, DLF_UP_METER_INDEX , 0x3F, 1);  					  
		foe_entry->l2_bridge.etype = ntohs(skb->dlf_resv0 & 0xFFFF);  /*special tag*/ 
		foe_entry->l2_bridge.bfib1.vpm = 0 ;/*0:special tag ; 1:8100; 2:88a8; 3:TPID CSR*/
		}
		PpeParseResult.vlan_layer =  (skb->dlf_resv2 >> 2) & 0x3 ;
		if( PpeParseResult.vlan_layer)
		{
			foe_entry->l2_bridge.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF); 		/*vlan 1 */
			foe_entry->l2_bridge.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);/*vlan2*/
		}
		else if(!isEN7580)
		{
			foe_entry->l2_bridge.vlan1 = 0x0000;
			PpeParseResult.vlan_layer = 1;
		}
	}else if((isWANInterface(skb->dev)) || (isPONInterface(skb->dev))){
		// downstream use meter for DLF  ratelimit , no special tag
		foe_entry->l2_bridge.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF);
		foe_entry->l2_bridge.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);
		foe_entry->l2_bridge.bfib1.vpm = skb->dlf_resv2 & 0x3;
		PpeParseResult.vlan_layer =  (skb->dlf_resv2 >> 2) & 0x3;
        if(SUPPORT_METER_RATELIMIT){
            foe_entry->l2_bridge.ts_id = HWNAT_DEFAULT_MTRGRP;
            PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
            PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
        }
        else
		PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, DLF_DOWN_METER_INDEX, 0x3F, 1);
	}
	else
		return 1;
	
	foe_entry->l2_bridge.act_dp = 0x0 ;
	FoeSetMacInfo(((struct _l2_bridge *)foe_entry)->dmac_hi, eth_hdr(skb)->h_dest);
	FoeSetMacInfo(((struct _l2_bridge *)foe_entry)->smac_hi, eth_hdr(skb)->h_source);

	return 0; 
}

static inline int32_t PpeDLFFillInfo_for_L2B32(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	if((isLANInterface(skb->dev))|| (FOE_SP(skb) == 3))
	{
		// upstream use meter for DLF  ratelimit
        if(SUPPORT_METER_RATELIMIT){
            foe_entry->l2_bridge32.ts_id = HWNAT_DEFAULT_MTRGRP;
			if(dlf_flag)/*set All to QDMA_WAN for CDS All ratelimit*/
			{
				PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3f, 0x3f, 1);
			}
			else{
	            PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
			}
			PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
			foe_entry->l2_bridge32.bfib1.vpm = skb->dlf_resv2 & 0x3;
			foe_entry->l2_bridge32.etype = (skb->dlf_resv0 & 0xFFFF);
        }
		else{
		PpeSetInfoBlk2(foe_entry, FP_PDMA, DLF_UP_METER_INDEX , 0x3F, 1);                    
		foe_entry->l2_bridge32.etype = ntohs(skb->dlf_resv0 & 0xFFFF);  /*special tag*/
		foe_entry->l2_bridge32.bfib1.vpm = 0 ;/*0:special tag ; 1:8100; 2:88a8; 3:TPID CSR*/
		}
		PpeParseResult.vlan_layer =  (skb->dlf_resv2 >> 2) & 0x3 ;
		if( PpeParseResult.vlan_layer)
		{
			foe_entry->l2_bridge32.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF); 		/*vlan 1 */
			foe_entry->l2_bridge32.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);/*vlan2*/
		}
		else if(!isEN7580)
		{
			foe_entry->l2_bridge32.vlan1 = 0x0000;
			PpeParseResult.vlan_layer = 1;
		}
	}
	else if((isWANInterface(skb->dev)) || (isPONInterface(skb->dev)))
	{
		// downstream use meter for DLF  ratelimit , no special tag
		foe_entry->l2_bridge32.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF);
		foe_entry->l2_bridge32.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);
		foe_entry->l2_bridge32.bfib1.vpm = skb->dlf_resv2 & 0x3;
		PpeParseResult.vlan_layer =  (skb->dlf_resv2 >> 2) & 0x3;
        if(SUPPORT_METER_RATELIMIT){
            foe_entry->l2_bridge32.ts_id = HWNAT_DEFAULT_MTRGRP;
			PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3f, 0x3f, 1);
            PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
			foe_entry->l2_bridge32.etype = skb->gem_port;
        }
        else
		PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, DLF_DOWN_METER_INDEX, 0x3F, 1);
	}
	else
		return 1;
	
	foe_entry->l2_bridge32.act_dp = 0x0 ;

	return 0 ;
}

static inline int32_t PpeDLFFillInfo_for_IPv4(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	struct iphdr *iph = NULL;
	iph = ip_hdr(skb);
	if((isLANInterface(skb->dev))|| (FOE_SP(skb) == 3))				
	{					
		// upstream use meter for DLF  ratelimit 				  
		if(SUPPORT_METER_RATELIMIT){
            foe_entry->ipv4_hnapt.ts_id = HWNAT_DEFAULT_MTRGRP;
			if(dlf_flag)/*set All to QDMA_WAN for CDS All ratelimit*/
			{
				PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3f, 0x3f, 1);
			}
			else{
	            PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
			}
			PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
			foe_entry->ipv4_hnapt.bfib1.vpm = skb->dlf_resv2 & 0x3;
			foe_entry->ipv4_hnapt.etype = (skb->dlf_resv0 & 0xFFFF);
        }
        else
		{
			PpeSetInfoBlk2(foe_entry, FP_PDMA, DLF_UP_METER_INDEX , 0x3F, 1);
			foe_entry->ipv4_hnapt.etype = ntohs(skb->dlf_resv0 & 0xFFFF);	/*special tag*/
			foe_entry->ipv4_hnapt.bfib1.vpm = 0 ;/*0:special tag ; 1:8100; 2:88a8; 3:TPID CSR*/
		}
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3 ;					
		if(PpeParseResult.vlan_layer)
		{						
			foe_entry->ipv4_hnapt.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF) ; 				 
			foe_entry->ipv4_hnapt.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16) ;								 
		}
		else if(!SUPPORT_METER_RATELIMIT)
		{						
			foe_entry->ipv4_hnapt.vlan1 = 0x0000;						
			PpeParseResult.vlan_layer = 1;					  
		} 
	}		
	else if((isWANInterface(skb->dev)) || (isPONInterface(skb->dev)))				
	{					 
		// downstream use meter for DLF	ratelimit									  
		foe_entry->ipv4_hnapt.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF);					 
		foe_entry->ipv4_hnapt.vlan2 = ntohs(((skb->dlf_resv1) & 0xFFFF0000)>>16);				 
		foe_entry->ipv4_hnapt.bfib1.vpm = (skb->dlf_resv2) & 0x3; 				  
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3;					
        if(SUPPORT_METER_RATELIMIT){
            foe_entry->ipv4_hnapt.ts_id = HWNAT_DEFAULT_MTRGRP;
 		    PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
            PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
			foe_entry->ipv4_hnapt.etype = skb->gem_port;
        }
        else
		PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, DLF_DOWN_METER_INDEX, 0x3F, 1); 
	} 
	else
		return 1;
	
	/*set IPv4 new info*/				
	foe_entry->ipv4_hnapt.new_sip = foe_entry->ipv4_hnapt.sip;				
	foe_entry->ipv4_hnapt.new_dip = foe_entry->ipv4_hnapt.dip;
	if( IS_IPV4_HNAPT(foe_entry) )
	{
		foe_entry->ipv4_hnapt.new_sport = foe_entry->ipv4_hnapt.sport ;
		foe_entry->ipv4_hnapt.new_dport = foe_entry->ipv4_hnapt.dport ;
	}
	foe_entry->ipv4_hnapt.dscp = iph->tos; 
	foe_entry->ipv4_hnapt.act_dp = 0x0 ;
	FoeSetMacInfo(((struct _ipv4_hnapt *)foe_entry)->dmac_hi, eth_hdr(skb)->h_dest);				
	FoeSetMacInfo(((struct _ipv4_hnapt *)foe_entry)->smac_hi, eth_hdr(skb)->h_source);	

	return 0 ;

}

static inline int32_t PpeDLFFillInfo_for_IPv6(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	int i=0, index=0;
	struct hwnat_shrink_field shrinkField;
	struct ipv6hdr *ip6h = NULL;
	ip6h = ipv6_hdr(skb);
    
	if((isLANInterface(skb->dev))|| (FOE_SP(skb) == 3))				
	{					
		// upstream use meter for DLF  ratelimit					  
		if(SUPPORT_METER_RATELIMIT){
            foe_entry->ipv6_3t_route.ts_id = HWNAT_DEFAULT_MTRGRP;
			if(dlf_flag)/*set All to QDMA_WAN for CDS All ratelimit*/
			{
				PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3f, 0x3f, 1);
			}
			else{
	            PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
			}
			PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
			foe_entry->ipv6_3t_route.bfib1.vpm = skb->dlf_resv2 & 0x3;
			foe_entry->ipv6_3t_route.etype = (skb->dlf_resv0 & 0xFFFF);
        }
		else
		{
			PpeSetInfoBlk2(foe_entry, FP_PDMA, DLF_UP_METER_INDEX , 0x3F, 1);
			foe_entry->ipv6_3t_route.etype = ntohs(skb->dlf_resv0 & 0xFFFF);	/*special tag*/
			foe_entry->ipv6_3t_route.bfib1.vpm = 0 ;/*0:special tag ; 1:8100; 2:88a8; 3:TPID CSR*/
		}
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3 ;					
		if(PpeParseResult.vlan_layer)
		{						
			foe_entry->ipv6_3t_route.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF); /*vlan 1 */					
			foe_entry->ipv6_3t_route.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);/*vlan2*/									
		}
		else if(!SUPPORT_METER_RATELIMIT)
		{						
			foe_entry->ipv6_3t_route.vlan1 = 0x0000;						
			PpeParseResult.vlan_layer = 1;					  
		}
	}				
	else if((isWANInterface(skb->dev)) || (isPONInterface(skb->dev)))				
	{					 
		// downstream use meter for DLF  ratelimit					 				  
		foe_entry->ipv6_3t_route.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF);					
		foe_entry->ipv6_3t_route.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);					
		foe_entry->ipv6_3t_route.bfib1.vpm = skb->dlf_resv2 & 0x3;					 
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3;				
		if(SUPPORT_METER_RATELIMIT){
            foe_entry->ipv6_3t_route.ts_id = HWNAT_DEFAULT_MTRGRP;
			PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3f, 0x3f, 1);
            PpeSetRxForceRingIdx(foe_entry,QDMA_RX_RING_FOR_DLF);
			foe_entry->ipv6_3t_route.etype = skb->gem_port;
        }
        else
		PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, DLF_DOWN_METER_INDEX , 0x3F, 1); 
	} 
	else
		return 1;
	
	/*set IPv6 new info*/				
	/*foe_entry->ipv6_3t_route.dscp = (PpeParseResult.ip6h.priority << 4 | (PpeParseResult.ip6h.flow_lbl[0]>>4));*/
	/*not support*/
	foe_entry->ipv6_3t_route.dscp = ((ip6h->priority << 4) | (ip6h->flow_lbl[0]>>4));
	foe_entry->ipv6_3t_route.act_dp = 0x0 ;
	if((SUPPORT_FOE_SMAC_CFG) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE)) {
	    // if smac doesn't change,then smac_idx[4]=1; else, smac_idx[3:0] =0~15
    	if(cmpMacInfo(eth_hdr(skb)->h_source, skb->smac) == HWNAT_SUCCESS) {
            set_ppe_entry_smac_index(foe_entry, IPV6_3T_ROUTE, 0x10);
    	} else {
    		for(i=0; i<UPDMEM_SMAC_CNT; i++)
    			shrinkField.smac[i] = eth_hdr(skb)->h_source[i];
    		index = find_and_update_shrink_table(PPE_UPDMEM_SEL_SMAC, &shrinkField);
    		if(index != -1) {
    			set_ppe_entry_smac_index(foe_entry, IPV6_3T_ROUTE, index);
    		} else {
    			printk("smac: find and update shrink table failed!\n");
    		}
    	}
    } else {
	    FoeSetMacInfo(get_ppe_entry_dmac_addr(foe_entry, IPV6_3T_ROUTE), eth_hdr(skb)->h_source);
    }
	FoeSetMacInfo(((struct _ipv6_3t_route *)foe_entry)->dmac_hi, eth_hdr(skb)->h_dest);

	return 0 ;
}

static int check_bind(unsigned short Ppe_Magic,struct FoeEntry *foe_entry,unsigned short Ppe_AI)
{
	if(ppeSupportPreBind)
	{
		if (CHECK_MAGIC(Ppe_Magic) && (Ppe_AI == HIT_UNBIND_RATE_REACH))
			PPESETPREBIND(foe_entry); 
		else if (CHECK_MAGIC(Ppe_Magic) && (Ppe_AI == HIT_PREBIND))
			return 1;
	}
	else if (CHECK_MAGIC(Ppe_Magic) && (Ppe_AI == HIT_UNBIND_RATE_REACH))
		return 1;

	return 0;
}

int32_t PpeDLFBindHandler(struct sk_buff * skb)
{
	struct FoeEntry *foe_entry = NULL;
	__u16 Ppe_Magic = 0;
	__u8 Ppe_AI = 0;
	unsigned long lock_flag;

	if(skb == NULL)
		return 0;
	
	if (DebugLevel >= 3) 
	{
		NAT_PRINT ("\r\nDLF HANDLE:skb->ppe_info_flag is %d",skb->ppe_info_flag);
		NAT_PRINT ("\r\nDLF HANDLE:Magic is %x,AI is %x,Entry Index is %d,skb->dev is %s",skb->ppe_magic,skb->ppe_ai,skb->ppe_foe_entry,
		    (skb->dev==NULL)?("NULL"):("not NULL"));
	}

    if((RA_NAT_EXE_VAL && (skb->ppe_info_flag == 0)) || (!RA_NAT_EXE_VAL))
	{
		//foe_entry = &PpeFoeBase[FOE_ENTRY_NUM(skb)];
		foe_entry = getPpeEntry(FOE_ENTRY_NUM(skb));
		if(foe_entry == NULL)
			return -1;
		Ppe_Magic = FOE_MAGIC_TAG(skb);
		Ppe_AI = FOE_AI(skb);
	}
	else if(skb->ppe_info_flag == 1 && (skb->pon_mark & DS_PKT_MAPPING_TO_ONE)&&\
		TCSUPPORT_GPON_MAPPING_VAL&&TCSUPPORT_GPON_DOWNSTREAM_MAPPING_VAL)
	{
		//foe_entry = &PpeFoeBase[skb->ppe_foe_entry];
		foe_entry = getPpeEntry(skb->ppe_foe_entry);
		if(foe_entry == NULL)
			return -1;
		Ppe_Magic = skb->ppe_magic;
		Ppe_AI = skb->ppe_ai;
	}

	if(check_bind(Ppe_Magic,foe_entry,Ppe_AI))
    	{
		//if this entry is already in binding state, skip it 
		if (foe_entry->bfib1.state == BIND) 
			return 0;
        
        PpeClearEntryInfo(foe_entry);
        
    		spin_lock_irqsave(&hw_nat_lock, lock_flag);
		if(IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry)) 		
		{							
			if(PpeDLFFillInfo_for_IPv4(skb , foe_entry ))
			{
				spin_unlock_irqrestore(&hw_nat_lock,lock_flag);
				return 0;
			}					
		}
		else if(IS_IPV6_3T_ROUTE(foe_entry) || IS_IPV6_5T_ROUTE(foe_entry)) 		
		{							
			if(PpeDLFFillInfo_for_IPv6(skb , foe_entry ))
			{
				spin_unlock_irqrestore(&hw_nat_lock,lock_flag);
				return 0;
			}					
		}
		else if(IS_L2_RRIDGE(foe_entry)) /*for752112*/
		{
			if(PpeDLFFillInfo_for_L2B32(skb , foe_entry ))
			{
				spin_unlock_irqrestore(&hw_nat_lock,lock_flag);
				return 0;
			}
		}
		else if(IS_L2_RRIDGE(foe_entry))/*for752x*/
		{
			if(PpeDLFFillInfo_for_L2B(skb , foe_entry ))
			{
				spin_unlock_irqrestore(&hw_nat_lock,lock_flag);
				return 0;
			}
		}
		else 
		{
			if (DebugLevel >= 3) 
				NAT_PRINT ("\r\nfoe_entry error,return -1");
			spin_unlock_irqrestore(&hw_nat_lock,lock_flag);
			return -1;
		}
		/* Enter binding state */
		PpeSetEntryBind(skb, foe_entry);		
        if(SUPPORT_SRAM_ENTRY && (FOE_ENTRY_NUM(skb)<hwnat_dram_idx_base)) {
            setPpeEntry(FOE_ENTRY_NUM(skb), foe_entry);
        }
		if(TCSUPPORT_HWNAT_L2VID_VAL)
			PpeL2luKeySetStatusBind(FOE_ENTRY_NUM(skb));  
		spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
		
		/* Dump Binding Entry */
		if (DebugLevel >= 3)
			FoeDumpEntry(FOE_ENTRY_NUM(skb));
		return 1;
	}
	
	return 0;
}

static inline void PpeDLF_free(void* ptr)
{
	if (ptr)
    {
        kfree(ptr);
        ptr = NULL;
    } 
	return;
}

struct DLFHwEntry *PpeDLF_fdb_find(const unsigned char *mac, const int index)
{
	struct DLFHwEntry* entry = NULL;

	list_for_each_entry_rcu(entry,&DLFEntryTable,list)
	{
		if( (mac[0] == entry->dstMac[0]) && (mac[1] == entry->dstMac[1]) && (mac[2] == entry->dstMac[2]) 
			&& (mac[3] == entry->dstMac[3]) && (mac[4] == entry->dstMac[4]) && (mac[5] == entry->dstMac[5]) && (entry->entryIndex == index) )
		{
			return entry;
		}
	}

	return NULL;
}

struct DLFHwEntry *PpeDLF_fdb_create(struct sk_buff *skb)
{
	struct DLFHwEntry *fdb=NULL;
	const unsigned char *mac = eth_hdr(skb)->h_dest;	

	fdb = (struct DLFHwEntry *)kmalloc(sizeof(struct DLFHwEntry), GFP_ATOMIC);
	
	if (fdb) 
	{
		memcpy(fdb->dstMac, mac, ETH_ALEN);
		fdb->entryIndex = hwnat_skb_to_foe(skb);
	}
	else
	{
		printk("PpeDLF_fdb_create:---kmalloc fail!\n");	
	}
	return fdb;
}

int PpeDLF_fdb_add(struct sk_buff *skb)
{
	struct DLFHwEntry *new_entry=NULL;	

	new_entry = PpeDLF_fdb_create(skb);
	if(new_entry == NULL)
	{
		printk("PpeDLF_fdb_add:---PpeDLF_fdb_create fail!\n"); 	
		return -1;
	}

	list_add_rcu(&(new_entry->list),&DLFEntryTable);

	return 0;
}

ecnt_ret_val PpeDLF_fdb_update(struct ecnt_data *in_data)
{
	struct net_data_s *net_data = (struct net_data_s *)in_data;
	struct sk_buff * skb = NULL ;
	unsigned char* mac = NULL ;
	struct DLFHwEntry *fdb = NULL ;
	int entry_index = 0;
	int ret = 0;

	if( dlf_control_enable == 0 )
		return ECNT_CONTINUE ;

	if( (net_data->pskb == NULL) || (*(net_data->pskb) == NULL) )
	{
		printk("PpeDLF_fdb_update: net_data->pskb = NULL!\n");
		return ECNT_CONTINUE ;
	}
	skb = (*(net_data->pskb)) ;
	if( skb == NULL )
	{
		printk("PpeDLF_fdb_update: skb = NULL!\n");
		return ECNT_CONTINUE ;
	}
	
	/*read dest mac*/
	mac = eth_hdr(skb)->h_dest ;
	/*check mac first*/
	if( is_multicast_ether_addr(mac) || is_broadcast_ether_addr(mac) || is_zero_ether_addr(mac) )
		return ECNT_CONTINUE ;
	/*read entry num*/
	entry_index =  hwnat_skb_to_foe(skb);
	if( hwnat_is_alive_index(entry_index) )
		return ECNT_CONTINUE ;

	spin_lock(&dlf_fdb_lock);
	fdb = PpeDLF_fdb_find(mac, entry_index);
	/*if find ,no need to add dlf entry*/
	if (fdb) 
	{	
		spin_unlock(&dlf_fdb_lock);
		return ECNT_CONTINUE;
	}
	/*if not find, add a new dlf entry*/
	ret = PpeDLFBindHandler(skb);
	if(ret == 1)
	{	
		PpeDLF_fdb_add(skb);/*add to dlf table*/
		if (DebugLevel >= 1) 
		{
			printk("PpeDLF_fdb_update: add a dlf entry, mac = %02X-%02X-%02X-%02X-%02X-%02X !\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		}
	}

	spin_unlock(&dlf_fdb_lock);
    
	return ECNT_CONTINUE;
}

ecnt_ret_val PpeDLF_fdb_clear_by_dstmac(struct ecnt_data *in_data)
{
	struct net_data_s *net_data = (struct net_data_s *)in_data ;
	struct DLFHwEntry* tmp = NULL ;
	struct DLFHwEntry* entry = NULL ;
	struct sk_buff * skb = NULL ;
	unsigned char* mac = NULL ;
	
	if( dlf_control_enable == 0 )
		return ECNT_CONTINUE;

	if( (net_data->pskb == NULL) || (*(net_data->pskb) == NULL) )
	{
		if (DebugLevel >= 1) 
		{
			printk("PpeDLF_fdb_clear_by_dstmac: net_data->pskb = NULL!\n");
		}
		return ECNT_CONTINUE ;
	}
	skb = (*(net_data->pskb)) ;
	if( skb == NULL )
	{
		if (DebugLevel >= 1) 
		{
			printk("PpeDLF_fdb_clear_by_dstmac: skb = NULL!\n");
		}
		return ECNT_CONTINUE ;
	}
	mac = eth_hdr(skb)->h_dest ;/*read dest mac*/

	spin_lock(&dlf_fdb_lock);	

	list_for_each_entry_safe(entry,tmp,&DLFEntryTable,list)
	{
		if( (mac[0] == entry->dstMac[0]) && (mac[1] == entry->dstMac[1]) && (mac[2] == entry->dstMac[2]) 
			&& (mac[3] == entry->dstMac[3]) && (mac[4] == entry->dstMac[4]) && (mac[5] == entry->dstMac[5]) )
		{
			if (DebugLevel >= 1) 
			{
				printk("PpeDLF_fdb_clear_by_dstmac:---delete dlf fdb entry %d ,  mac = %02X-%02X-%02X-%02X-%02X-%02X !\n", 
					entry->entryIndex,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
			}
			hwnat_delete_foe_entry(entry->entryIndex);
			list_del(&(entry->list));
			PpeDLF_free(entry);
		}
	}
	
	spin_unlock(&dlf_fdb_lock);
    
	return ECNT_CONTINUE;

}

ecnt_ret_val PpeDLF_fdb_clear_by_srcmac(struct ecnt_data *in_data)
{
	struct net_data_s *net_data = (struct net_data_s *)in_data ;
	struct DLFHwEntry* tmp = NULL ;
	struct DLFHwEntry* entry = NULL ;
	struct sk_buff * skb = NULL ;
	unsigned char* mac = NULL ;
	
	if( dlf_control_enable == 0 )
		return ECNT_CONTINUE;

	if( (net_data->pskb == NULL) || (*(net_data->pskb) == NULL) )
	{
		printk("PpeDLF_fdb_clear_by_srcmac: net_data->pskb = NULL!\n");
		return ECNT_CONTINUE ;
	}
	skb = (*(net_data->pskb)) ;
	if( skb == NULL )
	{
		printk("PpeDLF_fdb_clear_by_srcmac: skb = NULL!\n");
		return ECNT_CONTINUE ;
	}
	mac = eth_hdr(skb)->h_source ;/*read src mac*/
	
	spin_lock(&dlf_fdb_lock);	

	list_for_each_entry_safe(entry,tmp,&DLFEntryTable,list)
	{
		if( (mac[0] == entry->dstMac[0]) && (mac[1] == entry->dstMac[1]) && (mac[2] == entry->dstMac[2]) 
			&& (mac[3] == entry->dstMac[3]) && (mac[4] == entry->dstMac[4]) && (mac[5] == entry->dstMac[5]) )
		{
			if (DebugLevel >= 1) 
			{
				printk("PpeDLF_fdb_clear_by_srcmac:---delete dlf fdb entry %d ,  mac = %02X-%02X-%02X-%02X-%02X-%02X !\n", 
					entry->entryIndex,entry->dstMac[0],entry->dstMac[1],entry->dstMac[2],entry->dstMac[3],entry->dstMac[4],entry->dstMac[5]);
			}
			hwnat_delete_foe_entry(entry->entryIndex);
			list_del(&(entry->list));
			PpeDLF_free(entry);
		}
	}
	
	spin_unlock(&dlf_fdb_lock);				
	return ECNT_CONTINUE;

}

void PpeDLF_fdb_clear(void)
{
	struct DLFHwEntry *tmp = NULL ;
	struct DLFHwEntry *entry = NULL ;

	spin_lock_bh(&dlf_fdb_lock);	

	list_for_each_entry_safe(entry,tmp,&DLFEntryTable,list)
	{
		hwnat_delete_foe_entry(entry->entryIndex);
		list_del(&(entry->list));
		PpeDLF_free(entry);
	}

	spin_unlock_bh(&dlf_fdb_lock);
	return;	
}

void PpeDLF_fdb_clear_invalid_index(void)
{
	struct DLFHwEntry *tmp = NULL ;
	struct DLFHwEntry *entry = NULL ;

	if( dlf_control_enable == 0 )
		return ;

	spin_lock(&dlf_fdb_lock);	

	list_for_each_entry_safe(entry,tmp,&DLFEntryTable,list)
	{
		if( hwnat_is_alive_index(entry->entryIndex) == 0 )
		{
			if (DebugLevel >= 1) 
			{
				printk("PpeDLF_fdb_clear_invalid_index:---delete dlf fdb entry %d , mac = %02X-%02X-%02X-%02X-%02X-%02X !\n", 
					entry->entryIndex,entry->dstMac[0],entry->dstMac[1],entry->dstMac[2],entry->dstMac[3],entry->dstMac[4],entry->dstMac[5]);
			}
			list_del(&(entry->list));
			PpeDLF_free(entry);
		}
	}

	spin_unlock(&dlf_fdb_lock);
	return ;	
}

void PpeDLF_fdb_dump(void)
{
	struct DLFHwEntry *entry = NULL ;

	if( dlf_control_enable == 0 )
		return ;

	printk("dlf entry table:\n");
	printk("index             mac address          \n");
	spin_lock_bh(&dlf_fdb_lock);	

	list_for_each_entry_rcu(entry,&DLFEntryTable,list)
	{
		printk("%d             %02x:%02x:%02x:%02x:%02x:%02x          \n", entry->entryIndex, entry->dstMac[0], 
			entry->dstMac[1], entry->dstMac[2], entry->dstMac[3], entry->dstMac[4], entry->dstMac[5]);
	}

	spin_unlock_bh(&dlf_fdb_lock);
	return ;	
}
struct DLFNFHwEntry *PpeDLF_nf_find(const uint32_t ip, const int index)
{
	struct DLFNFHwEntry* entry = NULL;

	list_for_each_entry_rcu(entry,&DLFNFEntryTable,list)
	{
		if((entry->dstIp == ip) && (entry->entryIndex == index) )
		{
            if(DebugLevel >= 1)
                printk("PpeDLF_nf_find:entry ip=%x,ip=%x.\n", entry->dstIp, ip);
			return entry;
		}
	}

	return NULL;
}

struct DLFNFHwEntry *PpeDLF_nf_create(struct sk_buff *skb)
{
	struct DLFNFHwEntry *nf=NULL;
	uint32 ip = ip_hdr(skb)->daddr;	

	nf = (struct DLFNFHwEntry *)kmalloc(sizeof(struct DLFNFHwEntry), GFP_ATOMIC);
	
	if (nf) 
	{

		nf->dstIp = ip;
		nf->entryIndex = hwnat_skb_to_foe(skb);
        if(DebugLevel >= 1)
            printk("PpeDLF_nf_create:ip=%x,index=%x.\n",nf->dstIp,nf->entryIndex);
	}
	else
	{
		printk("PpeDLF_nf_create:---kmalloc fail!\n");	
	}
	return nf;
}

int PpeDLF_nf_add(struct sk_buff *skb)
{
	struct DLFNFHwEntry *new_entry=NULL;	

	new_entry = PpeDLF_nf_create(skb);
	if(new_entry == NULL)
	{
		printk("PpeDLF_fnf_add:---PpeDLF_nf_create fail!\n"); 	
		return -1;
	}

	list_add_rcu(&(new_entry->list),&DLFNFEntryTable);

	return 0;
}

ecnt_ret_val PpeDLF_nf_update(struct ecnt_data *in_data)
{
	struct net_data_s *net_data = (struct net_data_s *)in_data;
	struct sk_buff * skb = NULL ;
	uint32_t ip = 0 ;
	struct DLFNFHwEntry *nf = NULL ;
	int entry_index = 0;
	int ret = 0;

	if( dlf_control_enable == 0 )
		return ECNT_CONTINUE ;

	if( (net_data->pskb == NULL) || (*(net_data->pskb) == NULL) )
	{
		printk("PpeDLF_nf_update: net_data->pskb = NULL!\n");
		return ECNT_CONTINUE ;
	}
	skb = (*(net_data->pskb)) ;
	if( skb == NULL )
	{
		printk("PpeDLF_nf_update: skb = NULL!\n");
		return ECNT_CONTINUE ;
	}
    if(ip_hdr(skb) == NULL)
    {
        printk("PpeDLF_nf_update: ip_hdr = NULL!\n");
        return ECNT_CONTINUE ;
    }
	/*read dest ip*/
    if(ip_hdr(skb)==NULL){
        printk("ip_hdr is NULL.\n");
        return ECNT_CONTINUE;
    }

	if(ip_hdr(skb)->version == 6)
		return ECNT_CONTINUE;
	
	ip = ip_hdr(skb)->daddr;

	if(ipv4_is_multicast(ip))
		return ECNT_CONTINUE ;

	/*read entry num*/
	entry_index =  hwnat_skb_to_foe(skb);
	if( hwnat_is_alive_index(entry_index) )
		return ECNT_CONTINUE ;

	spin_lock_bh(&dlf_nf_lock);
	nf = PpeDLF_nf_find(ip, entry_index);
	spin_unlock_bh(&dlf_nf_lock);
	
	/*if find ,no need to add dlf entry*/
	if (nf) 
	{	
		return ECNT_CONTINUE;
	}
	/*if not find, add a new dlf entry*/
	ret = PpeDLFBindHandler(skb);
	if(ret == 1)
	{	
		spin_lock_bh(&dlf_nf_lock);
		PpeDLF_nf_add(skb);/*add to dlf table*/
		spin_unlock_bh(&dlf_nf_lock);
		if (DebugLevel >= 1) 
		{
			printk("PpeDLF_nf_update: add a dlf entry, ip = %d.%d.%d.%d !\n",(ip >> 24),((ip & 0xff0000) >> 16),((ip & 0xff00) >> 8),(ip & 0xff));
		}

		if(-1 != entry_index)
			foe_ext[entry_index].dlf = 1;
	}
    
	return ECNT_CONTINUE;
}
void PpeDLF_nf_clear(void)
{
	struct DLFNFHwEntry *tmp = NULL ;
	struct DLFNFHwEntry *entry = NULL ;

	spin_lock_bh(&dlf_nf_lock);	

	list_for_each_entry_safe(entry,tmp,&DLFNFEntryTable,list)
	{
		hwnat_delete_foe_entry(entry->entryIndex);
		list_del(&(entry->list));
		PpeDLF_free(entry);
	}

	spin_unlock_bh(&dlf_nf_lock);
	return;	
}

ecnt_ret_val PpeDLF_nf_clear_by_entryindex(struct ecnt_data *in_data)
{
	struct net_data_s *net_data = (struct net_data_s *)in_data ;
	struct DLFNFHwEntry* tmp = NULL ;
	struct DLFNFHwEntry* entry = NULL ;
	struct sk_buff * skb = NULL ;
	uint entry_index = 0;
	uint entry_find = 0;
    
	if( dlf_control_enable == 0 )
		return ECNT_CONTINUE;
    
    if( (net_data->pskb == NULL) || (*(net_data->pskb) == NULL) )
    {
        printk("PpeDLF_nf_update: net_data->pskb = NULL!\n");
        return ECNT_CONTINUE ;
    }
    skb = (*(net_data->pskb)) ;
    if( skb == NULL )
    {
        printk("PpeDLF_nf_update: skb = NULL!\n");
        return ECNT_CONTINUE ;
    }
    if(ip_hdr(skb) == NULL)
    {
        printk("PpeDLF_nf_update: ip_hdr = NULL!\n");
        return ECNT_CONTINUE ;
    }
    entry_index =  hwnat_skb_to_foe(skb);

	if((-1 != entry_index)&&!foe_ext[entry_index].dlf)
		return ECNT_CONTINUE;

	spin_lock_bh(&dlf_nf_lock);	
	list_for_each_entry_safe(entry,tmp,&DLFNFEntryTable,list)
	{
		if((entry != NULL) && (entry->entryIndex == entry_index))
		{
			if (DebugLevel >= 1) 
			{
				printk("PpeDLF_nf_clear_by_entryindex:---delete dlf nf entry %d ,  ip = %d.%d.%d.%d !\n", 
					entry->entryIndex, (entry->dstIp>> 24),((entry->dstIp & 0xff0000) >> 16),((entry->dstIp & 0xff00) >> 8),(entry->dstIp & 0xff));
			}
			
			list_del(&(entry->list));
			PpeDLF_free(entry);
			entry_find = 1;
			break;
		}
	}
	spin_unlock_bh(&dlf_nf_lock);

	if(entry_find) {
		hwnat_delete_foe_entry(entry_index);
	}
	
	return ECNT_CONTINUE;

}

void PpeDLF_nf_clear_invalid_index(void)
{
	struct DLFNFHwEntry *tmp = NULL ;
	struct DLFNFHwEntry *entry = NULL ;

	if( dlf_control_enable == 0 )
		return ;

	spin_lock_bh(&dlf_nf_lock);	
	list_for_each_entry_safe(entry,tmp,&DLFNFEntryTable,list)
	{
		if( hwnat_is_alive_index(entry->entryIndex) == 0 )
		{
			if (DebugLevel >= 1) 
			{
				printk("PpeDLF_nf_clear_invalid_index:---delete dlf nf entry %d ,dstIp=%x, ip = %d.%d.%d.%d !\n", 
					entry->entryIndex, entry->dstIp, ((entry->dstIp) >> 24),(((entry->dstIp) & 0xff0000) >> 16),(((entry->dstIp) & 0xff00) >> 8),((entry->dstIp) & 0xff));
			}
			list_del(&(entry->list));
			PpeDLF_free(entry);
		}
	}
	spin_unlock_bh(&dlf_nf_lock);
	
	return ;	
}

void PpeDLF_nf_dump(void)
{
	struct DLFNFHwEntry *entry = NULL ;

	if( dlf_control_enable == 0 )
		return ;

	printk("dlf entry table:\n");
	printk("index             ip          \n");
	spin_lock_bh(&dlf_nf_lock);	

	list_for_each_entry_rcu(entry,&DLFNFEntryTable,list)
	{
		printk("%d             %d.%d.%d.%d           \n", entry->entryIndex, 
            (entry->dstIp >> 24),((entry->dstIp & 0xff0000) >> 16),((entry->dstIp & 0xff00) >> 8),(entry->dstIp & 0xff));
	}

	spin_unlock_bh(&dlf_nf_lock);
	return ;
}

static int dlf_control_enable_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("%d\n",dlf_control_enable);
	
	return 0;
}
/******************************************************************************
******************************************************************************/
static int dlf_control_enable_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] ;
	uint cmd ;
	
	if (count > sizeof(val_string) - 1)
		return -EFAULT ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%d", &cmd) ;

	if(cmd == 1) 
	{
		dlf_control_enable = 1;
	}
	else
	{
		dlf_control_enable = 0;
		PpeDLF_fdb_clear();
        PpeDLF_nf_clear();
	}
	
	return count ;
}
/******************************************************************************
******************************************************************************/
static int dlf_hwnat_meter_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char val_string[32];
    char meter_dir[10] ;
	uint meter_para = 0 , mode = 0, tickSel = 0, meter_idx = 0;
    uint mainType_val=0;
	struct ppe_meter_cfg ppeMeterCfg;
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ;

	printk("Enter dlf_hwnat_meter_write_proc.\n");
    if (count > sizeof(val_string) - 1)
        return -EINVAL;
    
    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;
    
    val_string[count] = '\0';
    
    if(SUPPORT_METER_RATELIMIT){
        sscanf(val_string, "%9s %d %d %d", meter_dir, &mode, &meter_para, &tickSel) ;
	    printk("Set %s_dlf %d/%s, Select %s tick.\n" , meter_dir , meter_para, (mode?"pps":"Kbps"), (tickSel?"slow":"fast") );
    }
    else{
    sscanf(val_string, "%9s %d %d", meter_dir, &mode, &meter_para) ;
	printk("Set %s_dlf %d/%s.\n" , meter_dir , meter_para, (mode?"Kbps":"pps") );
    }

    if(SUPPORT_METER_RATELIMIT){
        if( 0 == strcmp(meter_dir,"up") ){
			if(dlf_flag){
				mainType_val = ECNT_QDMA_WAN;
			}
			else{
				mainType_val = ECNT_QDMA_LAN;
			}
            meter_idx = QDMA_RX_RING_FOR_DLF;
            
        }
        else if( 0 == strcmp(meter_dir,"down") ){
			mainType_val = ECNT_QDMA_WAN;
            meter_idx = QDMA_RX_RING_FOR_DLF;
        }
        else
            goto exit;
        rxRateLimitCfg.Index = meter_idx;
        rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;

        rxRateLimitCfg.MeterEn = GENERAL_METER_ENABLE;

        rxRateLimitCfg.PktMode = mode;

        rxRateLimitCfg.TickSel = tickSel;

        QDMA_API_SET_GENERAL_RATELIMIT_CFG(mainType_val, &rxRateLimitCfg);   

        /*call qdma ratelimit api to do meter ratelimit */
        rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
        rxRateLimitSet.Index = meter_idx;
        QDMA_API_GET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);   

        rxRateLimitSet.RateLimitValue = meter_para;
        QDMA_API_SET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);
    }
	else if(IS_IC_EN751X2X)
	{
	ppeMeterCfg.enable = 1;
	if( mode == 1 )/*byte mode*/	
		ppeMeterCfg.mode = METER_MODE_BYTE ;
	else
		ppeMeterCfg.mode = METER_MODE_PACKET ;
	if( 0 == strcmp(meter_dir,"up") )
		ppeMeterCfg.group = DLF_UP_METER_INDEX ;
	else if( 0 == strcmp(meter_dir,"down") )
		ppeMeterCfg.group = DLF_DOWN_METER_INDEX ;
	else
	{
		printk("set error\n");
		goto exit ;
	}
	ppeMeterCfg.scale = METER_SCALE_KBPS ;//Unit: Kbps
	ppeMeterCfg.rate = meter_para ;
	ppeMeterCfg.vlan = 0 ;			//just  config register
	ppeMeterCfg.addFlowOnly = 2 ;	// only set the meter register

	ecnt_ppe_meter_add(&ppeMeterCfg);
	}
	else
	{
		if( meter_para > 1270 )
		{
	        printk("PPS parameter is too big!\n");
	        goto exit;
        }
	/*set meter para*/
	if( 0 == strcmp(meter_dir,"up") )
		us_meter_pps = meter_para;
	else if( 0 == strcmp(meter_dir,"down") )
		ds_meter_pps = meter_para;
	else
	{
		printk("set error\n");
		goto exit ;
	}
	}
	
exit:
    return count;
}

void auto_dlf_update(TIMER_FUN_PAAM time)
{
	/* Schedule for the next time */
	dlf_update_timer.expires = jiffies + msecs_to_jiffies(100);
	mod_timer(&dlf_update_timer, dlf_update_timer.expires);	

	if( !dlf_control_enable )
		return ; 
 
	if((++timer_for_1s) == 10)
	{
		PpeDLF_fdb_clear_invalid_index() ; /*clear ageing entry*/
		timer_for_1s = 0;
	}
}

/*operation for flood*/
struct ecnt_hook_ops ecnt_dlf_update_flood_op = {
	.name = "dlf_update_flood_hook",
	.is_execute = 1,
	.hookfn = PpeDLF_fdb_update,
	.maintype = ECNT_NET_BR_FORWARD,
	.subtype = ECNT_BR_FORWARD_BR_FLOOD,
	.priority = 1
};
/*operation for portbind flood*/
struct ecnt_hook_ops ecnt_dlf_update_pb_flood_op = {
	.name = "dlf_update_flood_hook",
	.is_execute = 1,
	.hookfn = PpeDLF_fdb_update,
	.maintype = ECNT_NET_BR_FORWARD,
	.subtype = ECNT_BR_FORWARD_BR_PB_FLOOD,
	.priority = 1
};
/*clear by mac operation for fdb forward*/
struct ecnt_hook_ops ecnt_dlf_clear_by_dstmac_forward_op = {
	.name = "dlf_clear_by_dstmac_forward_hook",
	.is_execute = 1,
	.hookfn = PpeDLF_fdb_clear_by_dstmac,
	.maintype = ECNT_NET_BR_FORWARD,
	.subtype = ECNT_BR_FORWARD_BR_FORWARD,
	.priority = 1
};
/*clear by mac operation for fdb create*/
struct ecnt_hook_ops ecnt_dlf_clear_by_srcmac_create_op = {
	.name = "dlf_clear_by_srcmac_create_hook",
	.is_execute = 1,
	.hookfn = PpeDLF_fdb_clear_by_srcmac,
	.maintype = ECNT_NET_BR_FDB,
	.subtype = ECNT_BR_FDB_CREATE,
	.priority = 1
};

struct ecnt_hook_ops ecnt_dlf_update_nf_op = {
    .name = "dlf_update_nf_hook",
    .is_execute = 1,
    .hookfn = PpeDLF_nf_update,
    .maintype = ECNT_NET_NF,
    .subtype = ECNT_NET_NF_DLF,
    .priority = 1
};

struct ecnt_hook_ops ecnt_dlf_clear_nf_op = {
    .name = "dlf_clear_by_dstip_nf_hook",
    .is_execute = 1,
    .hookfn = PpeDLF_nf_clear_by_entryindex,
    .maintype = ECNT_NET_NF,
    .subtype = ECNT_NET_NF_DLF_CLR,
    .priority = 1
};

int DlfHookFuncInit(void)
{
	ecnt_register_hook(&ecnt_dlf_update_flood_op);
	ecnt_register_hook(&ecnt_dlf_update_pb_flood_op);
	ecnt_register_hook(&ecnt_dlf_clear_by_dstmac_forward_op);
	ecnt_register_hook(&ecnt_dlf_clear_by_srcmac_create_op); 
    ecnt_register_hook(&ecnt_dlf_update_nf_op);
    ecnt_register_hook(&ecnt_dlf_clear_nf_op);

	return 0;
}

void DlfHookFuncDeInit(void)
{
	ecnt_unregister_hook(&ecnt_dlf_update_flood_op);
	ecnt_unregister_hook(&ecnt_dlf_update_pb_flood_op);
	ecnt_unregister_hook(&ecnt_dlf_clear_by_dstmac_forward_op);
	ecnt_unregister_hook(&ecnt_dlf_clear_by_srcmac_create_op);
    ecnt_unregister_hook(&ecnt_dlf_update_nf_op);
    ecnt_unregister_hook(&ecnt_dlf_clear_nf_op);
	
	return;
}
/**************dlf end*****************************************/
int Ppe_set_CDS_dlf_mode(unsigned char mode)
{
	dlf_flag = mode;
	return 1;
}

int Ppe_set_CDS_Unmul_mode(unsigned char mode)
{
	unknown_mul_flag = mode;
	return 1;
}


static inline int PpeUnmulFillInfo_for_L2B(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	if((skb->dev->name[0]=='e')|| (FOE_SP(skb) == 3)){
		// upstream use meter for Unknown Multcast  ratelimit 
		if(2 == unknown_mul_flag)
			PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
		else
			PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
		PpeSetRxForceRingIdx(foe_entry, 7);/*unknown mul  to ring7*/					  
		if(isEN7580){
			foe_entry->l2_bridge32.bfib1.vpm = skb->dlf_resv2 & 0x3;
		}
		else{
			foe_entry->l2_bridge32.etype = ntohs(skb->dlf_resv0 & 0xFFFF);	/*special tag*/
			foe_entry->l2_bridge32.bfib1.vpm = 0 ;/*0:special tag ; 1:8100; 2:88a8; 3:TPID CSR*/
		}
		PpeParseResult.vlan_layer =  (skb->dlf_resv2 >> 2) & 0x3 ;
		if(PpeParseResult.vlan_layer)
		{
			foe_entry->l2_bridge32.vlan1 =ntohs(skb->dlf_resv1 & 0xFFFF); 		/*vlan 1 */
			foe_entry->l2_bridge32.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);/*vlan2*/
		}
		else if(!isEN7580)
		{
			foe_entry->l2_bridge32.vlan1 = 0x0000;
			PpeParseResult.vlan_layer = 1;
		}
	}else if((skb->dev->name[0]=='n') || (skb->dev->name[0]=='p')){
		// downstream use meter for Unknown Multcast ratelimit
		foe_entry->l2_bridge32.vlan1 =ntohs(skb->dlf_resv1 & 0xFFFF);
		foe_entry->l2_bridge32.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);
		foe_entry->l2_bridge32.bfib1.vpm = skb->dlf_resv2 & 0x3;
		PpeParseResult.vlan_layer =  (skb->dlf_resv2 >> 2) & 0x3;
		PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
		PpeSetRxForceRingIdx(foe_entry, 7);/*unknown mul  to ring7*/

	}
	foe_entry->l2_bridge32.act_dp = 0x0 ;

	return 1; 
}

static inline int32_t PpeUnmulFillInfo_for_IPv4(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	struct iphdr *iph = NULL;
	iph = ip_hdr(skb);
	if((skb->dev->name[0]=='e')|| (FOE_SP(skb) == 3))				
	{					
		// upstream use meter for Unknown Multcast  ratelimit 
		if(2 == unknown_mul_flag)
			PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
		else
			PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
		PpeSetRxForceRingIdx(foe_entry, 7);/*unknown mul  to ring7*/											
		if(isEN7580){
			foe_entry->ipv4_hnapt.bfib1.vpm = skb->dlf_resv2 & 0x3;
		}
		else{
			foe_entry->ipv4_hnapt.etype = ntohs(skb->dlf_resv0 & 0xFFFF);	/*special tag*/
			foe_entry->ipv4_hnapt.bfib1.vpm = 0 ;/*0:special tag ; 1:8100; 2:88a8; 3:TPID CSR*/
		}
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3 ;
		if(PpeParseResult.vlan_layer)
		{
			foe_entry->ipv4_hnapt.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF) ;
			foe_entry->ipv4_hnapt.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16) ;
		}
		else if(!isEN7580)
		{
			foe_entry->ipv4_hnapt.vlan1 = 0x0000;
			PpeParseResult.vlan_layer = 1;
		}
	}
	else if((skb->dev->name[0]=='n') || (skb->dev->name[0]=='p'))
	{
		// downstream use meter for Unknown Multcast ratelimit
		foe_entry->ipv4_hnapt.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF);
		foe_entry->ipv4_hnapt.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);
		foe_entry->ipv4_hnapt.bfib1.vpm = skb->dlf_resv2 & 0x3; 				  
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3;
		PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
		PpeSetRxForceRingIdx(foe_entry, 7);/*unknown mul  to ring7*/	

	} 
	/*set IPv4 new info*/				
	foe_entry->ipv4_hnapt.new_sip = foe_entry->ipv4_hnapt.sip;				
	foe_entry->ipv4_hnapt.new_dip = foe_entry->ipv4_hnapt.dip;
	if( IS_IPV4_HNAPT(foe_entry) )
	{
		foe_entry->ipv4_hnapt.new_sport = foe_entry->ipv4_hnapt.sport ;
		foe_entry->ipv4_hnapt.new_dport = foe_entry->ipv4_hnapt.dport ;
	}
	foe_entry->ipv4_hnapt.dscp = iph->tos; 
	foe_entry->ipv4_hnapt.act_dp = 0x0 ;
	FoeSetMacInfo(((struct _ipv4_hnapt *)foe_entry)->dmac_hi, eth_hdr(skb)->h_dest);				
	FoeSetMacInfo(((struct _ipv4_hnapt *)foe_entry)->smac_hi, eth_hdr(skb)->h_source);	

	return 1 ;

}

static inline int32_t PpeUnmulFillInfo_for_IPv6(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	int i=0, index=0;
	struct hwnat_shrink_field shrinkField;
	struct ipv6hdr *ip6h = NULL;
	ip6h = ipv6_hdr(skb);
    
	if((skb->dev->name[0]=='e')|| (FOE_SP(skb) == 3))				
	{					
		// upstream use meter for Unknown Multcast  ratelimit 
		if(2 == unknown_mul_flag)
			PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
		else
			PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
		PpeSetRxForceRingIdx(foe_entry, 7);/*unknown mul  to ring7*/												
		if(isEN7580){
			foe_entry->ipv6_3t_route.bfib1.vpm = skb->dlf_resv2 & 0x3;
		}
		else{
			foe_entry->ipv6_3t_route.etype = ntohs(skb->dlf_resv0 & 0xFFFF);	/*special tag*/
			foe_entry->ipv6_3t_route.bfib1.vpm = 0 ;/*0:special tag ; 1:8100; 2:88a8; 3:TPID CSR*/
		}
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3 ;
		if(PpeParseResult.vlan_layer)
		{
			foe_entry->ipv6_3t_route.vlan1 =ntohs(skb->dlf_resv1 & 0xFFFF); /*vlan 1 */
			foe_entry->ipv6_3t_route.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);/*vlan2*/
		}
		else if(!isEN7580)
		{
			foe_entry->ipv6_3t_route.vlan1 = 0x0000;
			PpeParseResult.vlan_layer = 1;
		}
	}
	else if((skb->dev->name[0]=='n') || (skb->dev->name[0]=='p'))
	{
		// downstream use meter for Unknown Multcast ratelimit
		foe_entry->ipv6_3t_route.vlan1 = ntohs(skb->dlf_resv1 & 0xFFFF);
		foe_entry->ipv6_3t_route.vlan2 = ntohs((skb->dlf_resv1 & 0xFFFF0000)>>16);
		foe_entry->ipv6_3t_route.bfib1.vpm = skb->dlf_resv2 & 0x3;
		PpeParseResult.vlan_layer = (skb->dlf_resv2 >> 2) & 0x3;
		PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, 0x3F, 0x3F, 1);
		PpeSetRxForceRingIdx(foe_entry, 7);/*unknown mul  to ring7*/	
	} 
	/*set IPv6 new info*/				
	/*foe_entry->ipv6_3t_route.dscp = (PpeParseResult.ip6h.priority << 4 | (PpeParseResult.ip6h.flow_lbl[0]>>4));*/
	/*not support*/
	foe_entry->ipv6_3t_route.dscp = ((ip6h->priority << 4) | (ip6h->flow_lbl[0]>>4));
	foe_entry->ipv6_3t_route.act_dp = 0x0 ;
	if((isEN7580) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE)) {
	    // if smac doesn't change,then smac_idx[4]=1; else, smac_idx[3:0] =0~15
    	if(cmpMacInfo(eth_hdr(skb)->h_source, skb->smac) == HWNAT_SUCCESS) {
            set_ppe_entry_smac_index(foe_entry, IPV6_3T_ROUTE, 0x10);
    	} else {
    		for(i=0; i<UPDMEM_SMAC_CNT; i++)
    			shrinkField.smac[i] = eth_hdr(skb)->h_source[i];
    		index = find_and_update_shrink_table(PPE_UPDMEM_SEL_SMAC, &shrinkField);
    		if(index != -1) {
    			set_ppe_entry_smac_index(foe_entry, IPV6_3T_ROUTE, index);
    		} else {
    			printk("smac: find and update shrink table failed!\n");
    		}
    	}
    } else {
	    FoeSetMacInfo(get_ppe_entry_dmac_addr(foe_entry, IPV6_3T_ROUTE), eth_hdr(skb)->h_source);
    }
	FoeSetMacInfo(((struct _ipv6_3t_route *)foe_entry)->dmac_hi, eth_hdr(skb)->h_dest);

	return 1 ;
}

int PpeSetMulDropToRatelimitHandler(struct sk_buff* skb)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	unsigned int foe_entry_idx=0;
	__u16 Ppe_Magic = 0;
	__u8 Ppe_AI = 0;
	int ret = 0;
	unsigned long lock_flag ;


	if(skb == NULL)
		return 0;

	if(!unknown_mul_flag)
		return 0;
	
	if (DebugLevel >= 3) 
		NAT_PRINT ("\r\nMagic is %x,AI is %x,Entry Index is %d",skb->ppe_magic,skb->ppe_ai,skb->ppe_foe_entry);

	if((skb->ppe_info_flag == 0)&&(TCSUPPORT_PON_VLAN_VAL||TCSUPPORT_PON_MAC_FILTER_VAL||\
		(TCSUPPORT_GPON_MAPPING_VAL&&TCSUPPORT_GPON_DOWNSTREAM_MAPPING_VAL)))
	{
		foe_entry_idx = (unsigned int)FOE_ENTRY_NUM(skb);
		Ppe_Magic = FOE_MAGIC_TAG(skb);
		Ppe_AI = FOE_AI(skb);
	}
	else if(skb->ppe_info_flag == 1 && (skb->pon_mark & DS_PKT_MAPPING_TO_ONE)&&\
		TCSUPPORT_GPON_MAPPING_VAL&&TCSUPPORT_GPON_DOWNSTREAM_MAPPING_VAL)
	{
		foe_entry_idx = (unsigned int)skb->ppe_foe_entry;
		Ppe_Magic = skb->ppe_magic;
		Ppe_AI = skb->ppe_ai;
	}
	foe_entry = getPpeEntryLocal(foe_entry_idx, foe_entry);
	if(foe_entry == NULL)
		return -1;
	
	if(check_bind(Ppe_Magic,foe_entry,Ppe_AI))
	{
        PpeClearEntryInfo(foe_entry);
		spin_lock_irqsave(&hw_nat_lock, lock_flag);
		if(IS_IPV4_GRP(foe_entry)){
			PpeUnmulFillInfo_for_IPv4(skb, foe_entry);
		}
		else if(IS_L2_RRIDGE(foe_entry))//only support 7580 , so
		{
			PpeUnmulFillInfo_for_L2B(skb, foe_entry);
		}
		else if(IS_IPV6_GRP(foe_entry)){
			PpeUnmulFillInfo_for_IPv6(skb, foe_entry);
		}
		else 
		{
			if (DebugLevel >= 3) 
				NAT_PRINT ("\r\nfoe_entry error,return -1");
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
			return -1;
		}
	
		/* Enter binding state */
		PpeSetEntryBind(skb, foe_entry);

		ecnt_multicast_hwnat_drop_flow(skb);

		spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
		
		/* Dump Binding Entry */
		if (DebugLevel >= 3)
			FoeDumpEntry(FOE_ENTRY_NUM(skb));
		ret = 1;
		goto finish;
	}
	ret = 0;
	
finish:
	if(SUPPORT_SRAM_ENTRY && (foe_entry_idx<hwnat_dram_idx_base)) {
		setPpeEntry(foe_entry_idx, foe_entry);
	}
	return ret;

}

/*************local in start*************************************/

static void ppeLocalInListInit(void)
{
	INIT_LIST_HEAD(&local_in_dport_list);
#ifdef CONFIG_TP_IMAGE
	INIT_LIST_HEAD(&local_in_dport_backup_list);
#endif /* CONFIG_TP_IMAGE */
}

static void* ppeLocalInListAlloc(int size)
{
	void* ptr = NULL;
	
	if (size>0)
	{
		ptr = kmalloc(size, GFP_ATOMIC);
		if(ptr == NULL) {
			printk("Alloc memory for ppeLocalInlist failed\n") ;
			return NULL ;
		}
		memset(ptr,0,size);
	}
	
	return ptr;
} 

static void ppeLocalInListFree(void* ptr)
{
	if (ptr)
	{
		kfree(ptr);
		ptr = NULL;
	} 
}

void ppeLocalInEnable(void)
{
    hwnat_local_in_enable = 1;
}

void ppeLocalInDisable(void)
{
    hwnat_local_in_enable = 0;
}

static int ppeLocalInAddDportOne(unsigned short dport)
{
    PpeLocalInDportList_t *local_in_entry = NULL;

	write_lock_bh(&Dportlock);
    list_for_each_entry(local_in_entry, &local_in_dport_list, list)	 
    {
        if(local_in_entry->dport == dport)
        {
            printk("same dport %d exsit,add fail\n",dport);
			write_unlock_bh(&Dportlock);
            return -1;
        }
    }

    local_in_entry = (PpeLocalInDportList_t*)ppeLocalInListAlloc(sizeof(PpeLocalInDportList_t));
    if(local_in_entry)
    {
        local_in_entry->dport = dport;
        list_add_tail(&local_in_entry->list,&local_in_dport_list);
    }
	write_unlock_bh(&Dportlock);
    return 0;
}

int ppeLocalInAddDport(unsigned short *dport_ptr,unsigned int port_num)
{
    int i = 0;

    for(i = 0;i < port_num;i++)
        ppeLocalInAddDportOne(*(dport_ptr+i));

    return 0;
}

int ppeLocalInAddAll(void)
{
    hwnat_local_in_dport_all = 1;
    return 0;
}

int ppeLocalInDelDportOne(unsigned short dport)
{
    PpeLocalInDportList_t *local_in_entry = NULL;
    PpeLocalInDportList_t *tmp = NULL;

	write_lock_bh(&Dportlock);
    list_for_each_entry_safe(local_in_entry, tmp,&local_in_dport_list, list)	 
    {
        if(local_in_entry->dport == dport)
        {
            list_del(&local_in_entry->list);
            ppeLocalInListFree(local_in_entry);
			write_unlock_bh(&Dportlock);
            return 0;
        }
    }
	write_unlock_bh(&Dportlock);
    printk("dport %d not found,delete fail\n",dport);
    return -1;
}

int ppeLocalInDelDport(unsigned short *dport_ptr,unsigned int port_num)
{
    int i = 0;

    for(i = 0;i < port_num;i++)
        ppeLocalInDelDportOne(*(dport_ptr+i));

    return 0;
}

int ppeLocalInDelAll(void)
{
    PpeLocalInDportList_t *local_in_entry = NULL;
    PpeLocalInDportList_t *tmp = NULL;

    hwnat_local_in_dport_all = 0;
	write_lock_bh(&Dportlock);

    list_for_each_entry_safe(local_in_entry, tmp,&local_in_dport_list, list)	 
    {
        list_del(&local_in_entry->list);
        ppeLocalInListFree(local_in_entry);
    }
	write_unlock_bh(&Dportlock);
    return 0;
}

int ppeLocalInShowDport(void)
{
    PpeLocalInDportList_t *local_in_entry = NULL;

    if(hwnat_local_in_enable)
        printk("hw_nat local in is enable\n");
    else
        printk("hw_nat local in is disable\n");

    printk("dport:\n");

    if(hwnat_local_in_dport_all)
        printk("all\n");
    else
    {
    	read_lock_bh(&Dportlock);
        list_for_each_entry(local_in_entry, &local_in_dport_list, list)	 
        {
                printk("%d\n",local_in_entry->dport);
        }
    	read_unlock_bh(&Dportlock);
    }

    return 0;
}

#ifdef CONFIG_TP_IMAGE
void ppeLocalInDelCopyList(void)
{
    PpeLocalInDportList_t *local_in_entry = NULL;
    PpeLocalInDportList_t *tmp = NULL;

	write_lock_bh(&Dportlock);

    list_for_each_entry_safe(local_in_entry, tmp,&local_in_dport_backup_list, list)	 
    {
        list_del(&local_in_entry->list);
        ppeLocalInListFree(local_in_entry);
    }
	write_unlock_bh(&Dportlock);
    return;
}

int ppeLocalInBackupDport(void)
{
	ppeLocalInDelCopyList();
	PpeLocalInDportList_t *local_in_entry = NULL;
	PpeLocalInDportList_t *tmp = NULL;
	unsigned int dport;
  
	write_lock_bh(&Dportlock);
    list_for_each_entry(local_in_entry, &local_in_dport_list, list)	 
    {
		dport = local_in_entry->dport;
		tmp = (PpeLocalInDportList_t*)ppeLocalInListAlloc(sizeof(PpeLocalInDportList_t));
		if(tmp)
		{
			tmp->dport = dport;
			list_add_tail(&tmp->list,&local_in_dport_backup_list);
			tmp = NULL;
		}
    }
	write_unlock_bh(&Dportlock);

    return 0;
}

int ppeLocalInRestoreDport(void)
{
	ppeLocalInDelAll();
        PpeLocalInDportList_t *local_in_entry = NULL;
	PpeLocalInDportList_t *tmp = NULL;
	unsigned int dport;
	write_lock_bh(&Dportlock);
    list_for_each_entry(local_in_entry, &local_in_dport_backup_list, list)	 
    {
		dport = local_in_entry->dport;
		tmp = (PpeLocalInDportList_t*)ppeLocalInListAlloc(sizeof(PpeLocalInDportList_t));
		if(tmp)
		{
			tmp->dport = dport;
			list_add_tail(&tmp->list,&local_in_dport_list);
			tmp = NULL;
		}
    }
	write_unlock_bh(&Dportlock);

    return 0;
}

int ppeLocalInShowBackupList(void)
{
    PpeLocalInDportList_t *local_in_entry = NULL;

    printk("backup port list dport:\n");

    if(hwnat_local_in_dport_all)
        printk("all\n");
    else
    {
    	read_lock_bh(&Dportlock);
        list_for_each_entry(local_in_entry, &local_in_dport_backup_list, list)	 
        {
                printk("%d\n",local_in_entry->dport);
        }
    	read_unlock_bh(&Dportlock);
    }

    return 0;
}
#endif /* CONFIG_TP_IMAGE */

int ppeLocalInFindDport(unsigned short dport)
{
    PpeLocalInDportList_t *local_in_entry = NULL;

	read_lock(&Dportlock);
    list_for_each_entry(local_in_entry, &local_in_dport_list, list)	 
    {
        if(local_in_entry->dport == dport)
        {
			read_unlock(&Dportlock);
            return 1;
        }
    }
	read_unlock(&Dportlock);

    return 0;
}

int ppeLocalInRcv(struct sk_buff * skb)
{
	struct FoeEntry *foe_entry;
	int foe_entry_idx=0;
    int protocol = 0;

        if(!hwnat_local_in_enable)
            return ECNT_CONTINUE;

	if(FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU)
		return ECNT_CONTINUE;
	
	foe_entry_idx = FOE_ENTRY_NUM(skb);
	foe_entry = getPpeEntry(foe_entry_idx);
	if(foe_entry == NULL)
		return ECNT_CONTINUE;

	if(!IS_IPV4_HNAPT(foe_entry)) 
		return ECNT_CONTINUE;

    protocol = (foe_entry->ipv4_hnapt.info_blk1 >> 30) & 0x1;
    
	if(DP_LOCAL_HOST == foe_entry->ipv4_hnapt.act_dp){
		if(0 == localInShortCut(skb, protocol))	
			return ECNT_RETURN;
	}

	return ECNT_CONTINUE;
}

static int checkLocalInDstPort(unsigned short port)
{
    if(hwnat_local_in_dport_all)
        return 1;

    if(ppeLocalInFindDport(port))
        return 1;

    return 0;
}

int ppeLocalInTcpTx(struct sk_buff * skb,unsigned short port)
{
    struct port_info pinfo;
    struct FoeEntry *foe_entry;
    int foe_entry_idx=0;
    int ring_idx = 0;
    uint8_t ethHdr[ETH_HLEN] = {0x00,0x00,0x00,0x00,0x00,0x01, 0x00,0x00,0x00,0x00,0x00,0x02, 0x08,0x00};
    int protocol = 0, magic = FOE_MAGIC_LOCAL;

    if (!hwnat_local_in_enable)
        return ECNT_CONTINUE;

    if (IS_MAGIC_TAG_VALID(skb))
    {
        if ((FOE_AI(skb) == HIT_BIND_KEEPALIVE_DUP_OLD_HDR) || (FOE_AI(skb) == HIT_BIND_KEEPALIVE_MC_NEW_HDR))
        {
            return ECNT_RETURN_DROP;
        }
    }
    
    if (FOE_AI(skb) != HIT_UNBIND_RATE_REACH)
    {
        return ECNT_CONTINUE;
    }

    if (!checkLocalInDstPort(port))
        return ECNT_CONTINUE;

    if (NULL == skb_mac_header(skb))
        return ECNT_CONTINUE;

    foe_entry_idx = FOE_ENTRY_NUM(skb);
    foe_entry = getPpeEntry(foe_entry_idx);
    if (foe_entry == NULL)
        return ECNT_CONTINUE;
    
    if (!IS_IPV4_HNAPT(foe_entry)) 
        return ECNT_CONTINUE;

    protocol = (foe_entry->ipv4_hnapt.info_blk1 >> 30) & 0x1;
    if (protocol)
    {
        if ((foe_entry->ipv4_hnapt.dip > 0xe0000000))
        {
            foe_ext[foe_entry_idx].dev_info = skb->dev;
        }
        
        if ((foe_entry->ipv4_hnapt.dip >= 0xe0000100) && (foe_entry->ipv4_hnapt.dip < 0xef000000))
        {
            magic = FOE_MAGIC_LOCAL_MCAST;
            ethHdr[0] = 0x01;
						ethHdr[1] = 0x00;
						ethHdr[2] = 0x5E;
						ethHdr[3] = (foe_entry->ipv4_hnapt.dip>>16) & 0xFF;
						ethHdr[4] = (foe_entry->ipv4_hnapt.dip>>8) & 0xFF;
						ethHdr[5] = (foe_entry->ipv4_hnapt.dip) & 0xFF;
        }
    }

    if (skb_mac_header(skb) == skb_network_header(skb))  /* like pppoe packet, skb MAC header is not setted correctly */
    {                                                   /* We insert pseudo MAC header in this case */
        if(skb_headroom(skb)<ETH_HLEN)
            return ECNT_CONTINUE;

        skb->data = skb_network_header(skb) - ETH_HLEN;
        memcpy(skb->data, ethHdr, ETH_HLEN);
        skb_reset_mac_header(skb);
    }
    else
    {
        skb->data = skb_mac_header(skb);
    }

    memset(&pinfo, 0, sizeof(pinfo));

    /* get a LRO ring index , and fill into nbq fild. if a invalid value returned set to default ring 0.  */
    if (TCSUPPORT_LRO_ENABLE_VAL == 1 && skb->len > 256)
    {
        if((SP_GDMA1 == FOE_SP(skb)) || (3 == FOE_SP(skb)) || (FOE_SP(skb) & 0x10))
        {
            ring_idx = LRO_GET_FREE_RING(ECNT_LRO_LAN, FOE_ENTRY_NUM(skb));
        }
        else{
            ring_idx = LRO_GET_FREE_RING(ECNT_LRO_WAN, FOE_ENTRY_NUM(skb));
        }
        if(ring_idx < 0 || ring_idx >= RX_RING_NUM)
        {
            ring_idx = 1;
        }
        pinfo.nbq = ring_idx;
        
        if (DebugLevel >= 7) 
        {
            printk("entry_idx :%d, ringidx:%d\n", FOE_ENTRY_NUM(skb), pinfo.nbq);
        }
    }
    else 
    {
        pinfo.nbq = 1;
    }

    if(SUPPORT_METER_RATELIMIT)
    {
        pinfo.tsid = HWNAT_DEFAULT_MTRGRP;
    }
	protocol = (foe_entry->ipv4_hnapt.info_blk1 >> 30) & 0x1;
    if ((foe_entry->ipv4_hnapt.dip > 0xe0000000) && protocol)
    {
        foe_ext[foe_entry_idx].dev_info = skb->dev;
    }

    PpeTxHandler(skb, &pinfo, magic);

    skb->data = skb_transport_header(skb);
    
    return ECNT_CONTINUE;
}
/*************local in end*************************************/

/*************l2tp start**************************************/
/*___________________________________________________________________________
**      function name: PpeRestoreRxInfo
**      descriptions:
**      	it's used to restore l2tp control packet information
** 
**      input parameters:
**      	skb:	the packet
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
__IMEM int PpeRestoreRxInfo(struct sk_buff *skb)
{
	if(SUPPORT_TUNNEL_HW_ACCELE){
		if (l2tp_rx.dst)
		{
			skb_dst_set(skb,dst_clone(l2tp_rx.dst));
		}
	}else{

		if (l2tp_rx.foe_idx != FOE_ENTRY_NUM(skb))
		{
			return 0;
		}

		if (skb->dev)
		{
			skb->dev = l2tp_rx.dev;
		}

		if (l2tp_rx.dst)
		{
			skb_dst_set(skb,dst_clone(l2tp_rx.dst));
		}
	}
	return 1;
}

/*___________________________________________________________________________
**      function name: PpeSaveTxInfo
**      descriptions:
**      	it's used to store l2tp packet tx information
** 
**      input parameters:
**      	skb:	the packet
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
__IMEM int PpeSaveTxInfo(struct sk_buff *skb)
{
	if( l2tp_tx.dst != skb_dst(skb) ){
		l2tp_tx.dst = dst_clone(skb_dst(skb));
	}
	
	return 1;
}

/*___________________________________________________________________________
**      function name: PpeRestoreTxInfo
**      descriptions:
**      	it's used to restore LAN->WAN step2 info
** 
**      input parameters:
**      	skb:	the packet
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
__IMEM int PpeRestoreTxInfo(struct sk_buff *skb)
{
	skb->dev = NULL ;
	skb_dst_set(skb,dst_clone(l2tp_tx.dst));

	return 1;
}

__IMEM int PpeSetL2tpVPNDev(struct net_device *dev)
{
	if( dev != l2tp_vpn_dev )
	{
		l2tp_vpn_dev = dev;
		printk("PpeSetL2tpVPNDev: update dev = %s\n\r",l2tp_vpn_dev->name);
	}
	
	return 1;
}

__IMEM struct net_device* PpeReadL2tpVPNDev(void)
{
	return l2tp_vpn_dev;
}

/*___________________________________________________________________________
**      function name: InitGreTxInfo
**      descriptions:
**      	it's used to init foe_tx_info
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void InitGreTxInfo(void)
{
	gre_tx.foe_idx = -1;
	gre_tx.dev = NULL;
	gre_tx.dst = NULL;
}


/*___________________________________________________________________________
**      function name: CleanGreTxInfo
**      descriptions:
**      	it's used to clean foe_tx_info
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void CleanGreTxInfo(void)
{
	gre_tx.foe_idx = -1;
	gre_tx.dev = NULL;
	gre_tx.dst = NULL;
}

/*___________________________________________________________________________
**      function name: PpeSaveTxInfo
**      descriptions:
**      	it's used to store l2tp packet tx information
** 
**      input parameters:
**      	skb:	the packet
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
__IMEM int PpeSaveGreTxInfo(struct sk_buff *skb)
{
	if(gre_tx.dst != skb_dst(skb) ){
		gre_tx.dst = dst_clone(skb_dst(skb));
	}
	
	return 1;
}

/*___________________________________________________________________________
**      function name: PpeRestoreTxInfo
**      descriptions:
**      	it's used to restore LAN->WAN step2 info
** 
**      input parameters:
**      	skb:	the packet
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
__IMEM int PpeRestoreGreTxInfo(struct sk_buff *skb)
{
	skb->dev = NULL ;
	skb_dst_set(skb,dst_clone(gre_tx.dst));

	return 1;
}

__IMEM int PpeIsGreOffloadPacket(struct sk_buff * skb)
{
	if (FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU)
	{
		return 1;
	}
	
	return 0;
}

/*___________________________________________________________________________
**      function name: InitL2tp
**      descriptions:
**      	init L2TP rx and hook information
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void InitGre(void)
{
	InitGreTxInfo();

	ra_sw_nat_hook_is_gre_offload_pkt = PpeIsGreOffloadPacket;
	ra_sw_nat_hook_save_gre_txinfo = PpeSaveGreTxInfo;
	ra_sw_nat_hook_restore_gre_txinfo = PpeRestoreGreTxInfo;
}


/*___________________________________________________________________________
**      function name: CleanL2tp
**      descriptions:
**      	clean L2TP rx and hook information
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void CleanGre(void)
{
	CleanGreTxInfo();
    
	ra_sw_nat_hook_is_gre_offload_pkt = NULL;
	ra_sw_nat_hook_save_gre_txinfo = NULL;
	ra_sw_nat_hook_restore_gre_txinfo = NULL;
}

void InitVxlanTxInfo(void)
{
	vxlan_tx.foe_idx = -1;
	vxlan_tx.dev = NULL;
	vxlan_tx.dst = NULL;
}

void CleanVxlanTxInfo(void)
{
	vxlan_tx.foe_idx = -1;
	vxlan_tx.dev = NULL;
	vxlan_tx.dst = NULL;
}

__IMEM int PpeSaveVxlanTxInfo(struct sk_buff *skb)
{
	if(vxlan_tx.dst != skb_dst(skb) ){
		vxlan_tx.dst = dst_clone(skb_dst(skb));
	}
	
	return 1;
}

__IMEM int PpeRestoreVxlanTxInfo(struct sk_buff *skb)
{
	skb->dev = NULL ;
	skb_dst_set(skb,dst_clone(vxlan_tx.dst));

	return 1;
}

__IMEM int PpeIsVxlanOffloadPacket(struct sk_buff * skb)
{
	if (FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU)
	{
		return 1;
	}
	
	return 0;
}

ecnt_ret_val ecnt_hook_vxlan_newlink(struct ecnt_data *in_data)
{
    int vni, i;
    struct net_device *dev;
	struct net_data_s *netData;

	netData = (struct net_data_s *)in_data;
    dev = (struct net_device *)netData->device;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
	vni = netData->vni;
#else
	vni = htonl(netData->vni);
#endif
    for (i = 0; i < VXLAN_MAX_LINK_NUM; i++)
    {
        if (g_vxlan_link[i].dev == NULL)
        {
            break;
        }
    }

    if (i < VXLAN_MAX_LINK_NUM)
    {
        g_vxlan_link[i].dev = dev;
        g_vxlan_link[i].vni = vni;
    }

    return ECNT_RETURN;
}

ecnt_ret_val ecnt_hook_vxlan_dellink(struct ecnt_data *in_data)
{
    int i;
    struct net_device *dev;
	struct net_data_s *netData;

	netData = (struct net_data_s *)in_data;
    dev = (struct net_device *)netData->device;

    for (i = 0; i < VXLAN_MAX_LINK_NUM; i++)
    {
        if (g_vxlan_link[i].dev == dev)
        {
            break;
        }
    }

    if (i < VXLAN_MAX_LINK_NUM)
    {
        g_vxlan_link[i].dev = NULL;
        g_vxlan_link[i].vni = 0;
    }

	return ECNT_RETURN;
}

struct ecnt_hook_ops ecnt_vxlan_newlink_ops = {
    .name = "ecnt_hook_vxlan_newlink",
    .hookfn = ecnt_hook_vxlan_newlink,
    .is_execute = 1,
    .maintype = ECNT_NET_VXLAN,
    .subtype = ECNT_NET_VXLAN_NEWLINK,
};

struct ecnt_hook_ops ecnt_vxlan_dellink_ops = {
    .name = "ecnt_hook_vxlan_dellink",
    .hookfn = ecnt_hook_vxlan_dellink,
    .is_execute = 1,
    .maintype = ECNT_NET_VXLAN,
    .subtype = ECNT_NET_VXLAN_DELLINK,
};

struct net_device* vxlan_getlink(int vni)
{
    int i;

    if (!vni)
    {
        return NULL;
    }
    
    for (i = 0; i < VXLAN_MAX_LINK_NUM; i++)
    {
        if (g_vxlan_link[i].vni == vni)
        {
            break;
        }
    }

    if (i < VXLAN_MAX_LINK_NUM)
    {
        return (struct net_device*)g_vxlan_link[i].dev;
    }

	return NULL;
}

/*___________________________________________________________________________
**      function name: InitVxlan
**      descriptions:
**      	init InitVxlan rx and hook information
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void InitVxlan(void)
{
    memset(g_vxlan_link, 0, sizeof(g_vxlan_link));

	InitVxlanTxInfo();

	ra_sw_nat_hook_is_vxlan_offload_pkt = PpeIsVxlanOffloadPacket;
	ra_sw_nat_hook_save_vxlan_txinfo = PpeSaveVxlanTxInfo;
	ra_sw_nat_hook_restore_vxlan_txinfo = PpeRestoreVxlanTxInfo;

    ecnt_register_hook(&ecnt_vxlan_newlink_ops);
    ecnt_register_hook(&ecnt_vxlan_dellink_ops);
}


/*___________________________________________________________________________
**      function name: CleanL2tp
**      descriptions:
**      	clean InitVxlan rx and hook information
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void CleanVxlan(void)
{
	CleanVxlanTxInfo();
    
	ra_sw_nat_hook_is_vxlan_offload_pkt = NULL;
	ra_sw_nat_hook_save_vxlan_txinfo = NULL;
	ra_sw_nat_hook_restore_vxlan_txinfo = NULL;

    ecnt_unregister_hook(&ecnt_vxlan_newlink_ops);
    ecnt_unregister_hook(&ecnt_vxlan_dellink_ops);
}

/*___________________________________________________________________________
**      function name: PpeGetIpVer
**      descriptions:
**      	it's used to get the ip version
** 
**      input parameters:
**      	skb: skb data should point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	IPv4 or IPv6
**___________________________________________________________________________
*/
inline int PpeGetIpVer(struct sk_buff *skb)
{
	struct iphdr *iph = NULL;
 	uint8 ip_ver = 0;

	iph = (struct iphdr *)skb->data;
 	ip_ver = iph->version;
 	
 	return ip_ver;
}

/*___________________________________________________________________________
**      function name: SendToPpe
**      descriptions:
**      	send the packet to HW PPE
** 
**      input parameters:
**      	skb:	skb data point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
int SendToPpe(struct sk_buff * skb)
{
	int i;
	u16 VirIfIdx = 0;
	uint8 ip_ver = 0;
	uint8 tmp_src_mac_down[] = {0x00,0x00,0x00,0xdd,0xee,0xff};
	uint8 tmp_src_mac_up[] = {0x00,0x00,0x00,0xff,0xee,0xdd};
	uint8 l2vpn_mac[ETH_HLEN] = {0};
	uint8 orig_mac[ETH_HLEN] = {0};
	struct vlan_ethhdr *veth = NULL;
	unsigned int paddingLength = 0;
	unsigned int skbLenTmp = 0;
	struct sk_buff * skb2 = NULL;
	
	/* set offload pkt idx */
	if( FOE_MAGIC_TAG(skb) == FOE_MAGIC_SPEED_TEST_UPSTREAM ){
		VirIfIdx = DP_SPEED_UP;
	}else if( FOE_MAGIC_TAG(skb) ==FOE_MAGIC_TR471_TEST_UPSTREAM){
		VirIfIdx = DP_TR471_UP;
	}else if( FOE_MAGIC_TAG(skb) ==FOE_MAGIC_TR471_HW_TEST_UPSTREAM){
		VirIfIdx = DP_TR471_UP;
		if(offload_eth_fast_tx_hook){
			offload_eth_fast_tx_hook(skb, 7);
			return 1;
		}
		else
			return 0;
	}else if( FOE_MAGIC_TAG(skb) == FOE_MAGIC_L2TP_VPN_DOWNSTREAM ){
		VirIfIdx = DP_L2TP_DOWN_2;
	}else if( FOE_MAGIC_TAG(skb) == FOE_MAGIC_L2TP_VPN_UPSTREAM ){
		skb->ppe_magic = FOE_MAGIC_L2TP_VPN_UPSTREAM;
		VirIfIdx = DP_L2TP_UP_2;
	}else if( FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_UP_2 ){
		VirIfIdx = DP_GRE_UP_2;
		skb->ppe_magic = FOE_MAGIC_GRE_UP_2;
	}else if( FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_DOWN_2 ){
		for (i = DP_GRE_TUNNEL_0; i <= DP_GRE_TUNNEL_7; i++)
		{
			if (skb->dev == DstPort[i])
				break;
		}

		if (i <= DP_GRE_TUNNEL_7)
		{
			VirIfIdx = i;
		}
		else
		{
			for(i = DP_GRE_L2_PRI_TUNNEL_0; i <= DP_GRE_L2_SEC_TUNNEL_7; i++)
			{
				if (skb->dev == DstPort[i])
					break;
			}
			if (i <= DP_GRE_L2_SEC_TUNNEL_7)
			{
				VirIfIdx = i;
				skb_push(skb, ETH_HLEN);
				memcpy(l2vpn_mac, skb->data, ETH_HLEN);
				skb_pull(skb, ETH_HLEN);
			}else
            	return 0;
		}
    }else if( (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_UP_2) || (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_UP_FRAG) ){
        for (i = DP_VXLAN_0; i <= DP_VXLAN_19; i++)
        {
            if (skb->dev == DstPort[i])
                break;
        }
    
        if (i <= DP_VXLAN_19)
        {
            VirIfIdx = i - DP_VXLAN_0 + DP_VXLAN_UP_2_0;
        }
        else
        {
            return 0;
        }
    }else if( FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_DOWN_2 ){
        for (i = DP_VXLAN_0; i <= DP_VXLAN_19; i++)
        {
            if (skb->dev == DstPort[i])
                break;
        }
    
        if (i <= DP_VXLAN_19)
        {
            VirIfIdx = i;
            memcpy(l2vpn_mac, skb->data, ETH_HLEN);
            skb_pull(skb, ETH_HLEN);
        }
        else
        {
            return 0;
        }
    }else if( (FOE_MAGIC_TAG(skb) == FOE_MAGIC_APP_SHORTCUT)){        
        for (i = DP_RA0; (i <= DP_RA7) && (0 == VirIfIdx); i++)
        {
            if (skb->dev == DstPort[i])
            {
                VirIfIdx = i;
                break;
            }
        }
        for (i = DP_RAI0; (i <= DP_RAI7) && (0 == VirIfIdx); i++)
        {
            if (skb->dev == DstPort[i])
            {
                VirIfIdx = i;
                break;
            }
        }
        for (i = DP_RAX0; (i <= DP_RAX7) && (0 == VirIfIdx); i++)
        {
            if (skb->dev == DstPort[i])
            {
                VirIfIdx = i;
                break;
            }
        }
        for (i = DP_GMAC1; (i <= DP_GMAC8) && (0 == VirIfIdx); i++)
        {
            if (skb->dev == DstPort[i])
            {
                VirIfIdx = i;
                break;
            }
        }
        
        if ((VirIfIdx >= DP_RA0) && (VirIfIdx <= DP_RA7))
        {
            VirIfIdx = VirIfIdx - DP_RA0 + DP_APP_SHORTCUT_RA0;
        }
        else if ((VirIfIdx >= DP_RAI0) && (VirIfIdx <= DP_RAI7))
        {
            VirIfIdx = VirIfIdx - DP_RAI0 + DP_APP_SHORTCUT_RAI0;
        }
        else if ((VirIfIdx >= DP_GMAC1) && (VirIfIdx <= DP_GMAC8))
        {
            VirIfIdx = VirIfIdx - DP_GMAC1 + DP_APP_SHORTCUT_GMAC1;
        }
        else
        {
            return 0;
        }
        memcpy(orig_mac, skb->data, ETH_HLEN);
        skb_pull(skb, ETH_HLEN);
	}else{
		return 0;
	}

	ip_ver = PpeGetIpVer(skb);

	/* 1. add virtual vlan */
	if(FOE_MAGIC_TAG(skb) != FOE_MAGIC_WLAN){
		skb = skb_unshare(skb, GFP_ATOMIC);
		if (!skb) {
			return 0;
		}
	}
	/* dmac+smac+vlan_proto+vlan+proto */
	veth = (struct vlan_ethhdr *)skb_push(skb, ETH_HLEN+VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	skb_reset_mac_header(skb);

	/* first, the ethernet type */
	veth->h_vlan_proto = htons(ETH_P_8021Q);

	/* now, the TCI */
	veth->h_vlan_TCI = htons(VirIfIdx);

	skb->protocol = htons(ETH_P_8021Q);

	/* 2. fill in layer2 information */
    if ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_DOWN_2)
		||((VirIfIdx >= DP_GRE_L2_PRI_TUNNEL_0) && (VirIfIdx <=DP_GRE_L2_SEC_TUNNEL_7)))
    {
    	memcpy(skb->data, l2vpn_mac, 2 * ETH_ALEN);
    }
    else if ((VirIfIdx >= DP_APP_SHORTCUT_RA0) && (VirIfIdx <= DP_APP_SHORTCUT_GMAC8))
    {
    	memcpy(skb->data, orig_mac, 2 * ETH_ALEN);
    }
    else
    {
    	memcpy(skb->data, GetMacAddr(), ETH_ALEN);
    	if ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_L2TP_VPN_DOWNSTREAM ) 
    		|| ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_DOWN_2 ))){
    		memcpy(skb->data + ETH_ALEN, tmp_src_mac_down, ETH_ALEN);
    	}else{
    		memcpy(skb->data + ETH_ALEN, tmp_src_mac_up, ETH_ALEN);
    	}
    }
		
	if (IP_VER_V4 == ip_ver){
		veth->h_vlan_encapsulated_proto = htons(ETH_P_IP);
	}else if (IP_VER_V6 == ip_ver){
		veth->h_vlan_encapsulated_proto = htons(ETH_P_IPV6);
	}

    if ((VirIfIdx >= DP_VXLAN_UP_2_0) && (VirIfIdx <= DP_VXLAN_UP_2_19))
    {
        if( FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_UP_FRAG )
        {
            skb->ppe_magic = FOE_MAGIC_VXLAN_UP_FRAG;
        }
        else
            skb->ppe_magic = FOE_MAGIC_VXLAN_UP_2;
    }
    else if ((VirIfIdx >= DP_VXLAN_0) && (VirIfIdx <= DP_VXLAN_19))
    {
        skb->ppe_magic = FOE_MAGIC_VXLAN_DOWN_2;
    }
    else if ((VirIfIdx >= DP_APP_SHORTCUT_RA0) && (VirIfIdx <= DP_APP_SHORTCUT_GMAC8))
    {
        skb->ppe_magic = FOE_MAGIC_APP_SHORTCUT;
    }

	if (skb->len < 64)
	{
		paddingLength = 64 - skb->len;
		skbLenTmp = skb->len;
	
		if(skb_tailroom(skb) < paddingLength){
			skb2 = skb_copy_expand(skb, skb_headroom(skb), paddingLength, GFP_ATOMIC);
			if (skb2) {
				kfree_skb(skb);
				skb = skb2;
				skb_put(skb, paddingLength);
			}
			else {
				if (DebugLevel >= 1)
					printk("\nskb_copy_expand failed at SendToPpe\n");
				return 0;
			}
		}
		else {
			skb_put(skb, paddingLength);
		}
		memset(skb->data + skbLenTmp, 0, paddingLength);
	}

	/* 3.redirect to PPE, send packet to PPE directly */
	if(offload_eth_fast_tx_hook)
		offload_eth_fast_tx_hook(skb, 7);
	else
		return 0;

	return 1;
}


/*___________________________________________________________________________
**      function name: PpeIsHwNatPacket
**      descriptions:
**      	it's used to check the packet is upstream or downstream after half hw_nat
** 
**      input parameters:
**      	skb:	the packet
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1=HWNAT_PKT_UPSTREAM; 2=HWNAT_PKT_DOWNSTREAM
**		failure:	0
**___________________________________________________________________________
*/
__IMEM int PpeIsHwNatPacket(struct sk_buff * skb)
{
	struct FoeEntry *foe_entry = NULL;
	unsigned int foe_index = FOE_ENTRY_NUM(skb);

	if (FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU)
	{
		return HWNAT_PKT_ERROR;
	}
	
	foe_entry = getPpeEntry(foe_index);
	if (NULL == foe_entry)
	{
		return HWNAT_PKT_ERROR;
	}
	
	if (IS_IPV4_GRP(foe_entry)||IS_IPV6_GRP(foe_entry))
	{
		return foe_ext[foe_index].pkt_dir;
	}
	
	return HWNAT_PKT_ERROR;
}

/*___________________________________________________________________________
**      function name: set_l2tp_vpn_force_info
**      descriptions:
**      	it's used to set the PSE force port and packet direction
** 
**      input parameters:
**      	foe_entry: foe entry
**		pinfo: 	  port information
**		magic: 	  foe magic number
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
int set_l2tp_vpn_force_info(struct sk_buff *skb,struct FoeEntry *foe_entry, struct port_info *pinfo, int magic)
{
	unsigned int foe_index = FOE_ENTRY_NUM(skb);
	
	if ((FOE_MAGIC_TAG(skb) != FOE_MAGIC_L2TP_VPN_UPSTREAM) 
		&& (FOE_MAGIC_TAG(skb) != FOE_MAGIC_L2TP_VPN_DOWNSTREAM))
	{
		return 0;
	}
	
	if (IS_IPV4_GRP(foe_entry)||IS_IPV6_GRP(foe_entry))
	{
		/* l2tp vpn half hwnat, set the FP to PDMA */
		PpeSetInfoBlk2(foe_entry,FP_PDMA,0x3F,0x3F, pinfo->ds_to_qdma);

		/* set l2tp vpn pkt direction */
		if (FOE_MAGIC_L2TP_VPN_UPSTREAM == FOE_MAGIC_TAG(skb))
		{
			foe_ext[foe_index].pkt_dir  = HWNAT_PKT_UPSTREAM;
		}
		else if (FOE_MAGIC_L2TP_VPN_DOWNSTREAM == FOE_MAGIC_TAG(skb))
		{
			foe_ext[foe_index].pkt_dir = HWNAT_PKT_DOWNSTREAM;
		}

		if(SUPPORT_METER_RATELIMIT)
		{
			if(IS_IPV4_GRP(foe_entry))
			{
				foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
			}
			else
			{
				foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
			}
		}
		
		if(isEN751627 || FOE_NBQ_CFG_SUPPORT)
			PpeSetRxForceRingIdx(foe_entry,RING_IDX_1);
	
		return 1;
	}
	
	return 0;
}
/*___________________________________________________________________________
**      function name: InitL2tpImportantInfo
**      descriptions:
**      	it's used to init foe_rx_info & foe_tx_info
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void InitL2tpImportantInfo(void)
{
	l2tp_rx.foe_idx = -1;
	l2tp_rx.dev = NULL;
	l2tp_rx.dst = NULL;

	l2tp_tx.foe_idx = -1 ;
	l2tp_tx.dev = NULL ;
	l2tp_tx.dst = NULL ;

	l2tp_vpn_dev = NULL ;
}

/*___________________________________________________________________________
**      function name: CleanL2tpImportantInfo
**      descriptions:
**      	it's used to clean foe_rx_info & foe_tx_info
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void CleanL2tpImportantInfo(void)
{
	l2tp_rx.foe_idx = -1;
	l2tp_rx.dev = NULL;
	l2tp_rx.dst = NULL;

	l2tp_tx.foe_idx = -1 ;
	l2tp_tx.dev = NULL ;
	l2tp_tx.dst = NULL ;

	l2tp_vpn_dev = NULL ;
}

/*___________________________________________________________________________
**      function name: PpeSaveRxInfo
**      descriptions:
**      	it's used to store l2tp packet rx information
** 
**      input parameters:
**      	skb:	the packet
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
__IMEM int PpeSaveRxInfo(struct sk_buff *skb)
{
	/* if existed and same, return */
	if ((l2tp_rx.dev == skb->dev) && (l2tp_rx.foe_idx == FOE_ENTRY_NUM(skb)))
	{
		return 0;
	}

	/* save l2tp rx info */
	l2tp_rx.foe_idx = FOE_ENTRY_NUM(skb);
	l2tp_rx.dev = skb->dev;

	if( l2tp_rx.dst != skb_dst(skb) ){
		l2tp_rx.dst = dst_clone(skb_dst(skb));
	}
	
	return 1;
}


/*___________________________________________________________________________
**      function name: InitL2tp
**      descriptions:
**      	init L2TP rx and hook information
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void InitL2tp(void)
{
	if(isEN7528)
		ppeSetL2tpParserEnable();
	
	InitL2tpImportantInfo();

	ra_sw_nat_hook_is_hwnat_pkt = PpeIsHwNatPacket;
	ra_sw_nat_hook_save_rxinfo = PpeSaveRxInfo;
	ra_sw_nat_hook_restore_rxinfo = PpeRestoreRxInfo;
	ra_sw_nat_hook_save_txinfo = PpeSaveTxInfo;
	ra_sw_nat_hook_restore_txinfo = PpeRestoreTxInfo;
	ra_sw_nat_hook_sendto_ppe = SendToPpe;

	ra_sw_nat_hook_set_l2tp_dev = PpeSetL2tpVPNDev;
	ra_sw_nat_hook_read_l2tp_dev = PpeReadL2tpVPNDev;

	return;
}


/*___________________________________________________________________________
**      function name: CleanL2tp
**      descriptions:
**      	clean L2TP rx and hook information
** 
**      input parameters:
**      	N/A
**
**      output parameters:
**      	N/A
**
**      return:
**		N/A
**___________________________________________________________________________
*/
void CleanL2tp(void)
{
	CleanL2tpImportantInfo();
	
	ra_sw_nat_hook_is_hwnat_pkt = NULL;
	ra_sw_nat_hook_save_rxinfo = NULL;
	ra_sw_nat_hook_restore_rxinfo = NULL;
	ra_sw_nat_hook_save_txinfo = NULL;
	ra_sw_nat_hook_restore_txinfo = NULL;
	ra_sw_nat_hook_sendto_ppe = NULL;

	ra_sw_nat_hook_set_l2tp_dev = NULL;
	ra_sw_nat_hook_read_l2tp_dev = NULL;
}

/*************l2tp end**************************************/

__IMEM int32_t PpeGetOffload(struct sk_buff *skb, int *dp)
{
	uint16_t ret = 0;
	struct FoeEntry *foe_entry = NULL;
	
	foe_entry = getPpeEntry(FOE_ENTRY_NUM(skb));
	if(foe_entry == NULL)
		return -1;

	if (FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU)
	{
		return ret;
	}

	if(!is_multicast_ether_addr(&(skb->data[0])))
	{
		return ret;
	}	
	
	if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry))
	{
		if ((DP_RA0 <= foe_entry->ipv4_hnapt.act_dp) && (foe_entry->ipv4_hnapt.act_dp <= DP_GMAC))
		{
			ret = 1;
		}
	}
	else if (IS_L2_RRIDGE(foe_entry))
	{
		if ((DP_RA0 <= foe_entry->l2_bridge32.act_dp) && (foe_entry->l2_bridge32.act_dp <= DP_GMAC))
		{
			ret = 1;
		}
	}
	else if (IS_IPV4_DSLITE(foe_entry) || IS_IPV6_3T_ROUTE(foe_entry) || IS_IPV6_5T_ROUTE(foe_entry) || IS_IPV6_6RD(foe_entry))
	{
		if ((DP_RA0 <= foe_entry->ipv6_5t_route.act_dp) && (foe_entry->ipv6_5t_route.act_dp <= DP_GMAC))
		{
			ret = 1;
		}
	}
	
	return ret;
}

__IMEM int PpeTsoPingPongHandle(struct sk_buff *skb)
{
	tsoRuleEntry_v4_t skbEntry;
	struct FoeEntry *foe_entry;
	
	foe_entry = (struct FoeEntry*)K1_TO_K0((((unsigned char*)&PpeFoeBase[0]) + FOE_ENTRY_NUM(skb)*SIZE_OF_FOE_ENTRY));
	if (IS_IPV4_HNAPT(foe_entry)){
		skbEntry.sport = ntohs(foe_entry->ipv4_hnapt.sport);
		skbEntry.dport = ntohs(foe_entry->ipv4_hnapt.dport);
		skbEntry.saddr_v4 = ntohl(foe_entry->ipv4_hnapt.sip);
		skbEntry.daddr_v4 = ntohl(foe_entry->ipv4_hnapt.dip);
		TSO_SEND_PINGPONG_V4(&skbEntry, skb);
	}
	else
	{
		printk("[%s][%d]DP_TSO_LOCALOUT pkt. pkt_type:%x\n", __FUNCTION__, __LINE__, foe_entry->bfib1.pkt_type);
	}

	return 1;
}

static int ppe_vpnenable_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("ppe vpn test %s\n",ppe_vpnenable?"enable":"disable");
	
	return 0;	
}

static int ppe_vpnenable_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';
	
	sscanf(get_buf, "%d",&ppe_vpnenable);
	
	printk("%s ppe vpn test\n",ppe_vpnenable?"enable":"disable");

	return count;
}

__IMEM int32_t PpeDownstreamOffload(struct sk_buff * skb, int *dp)
{
	int ret = 0;
	struct FoeEntry *foe_entry;
	unsigned int foe_index = 0;

	foe_index = FOE_ENTRY_NUM(skb);
	if(0==foe_ext[foe_index].ds_offload)
		return ret;
	
	foe_entry = getPpeEntry(foe_index);
	if(foe_entry == NULL)
		return -1;

	if((FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU) || (FOE_AI(skb) == HIT_BIND_MUL_CPUR)) {
		if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry)) {
			*dp = foe_entry->ipv4_hnapt.etype;
			ret = foe_ext[foe_index].ds_offload;
		}
		else if (IS_L2_RRIDGE(foe_entry)){
			*dp = foe_entry->l2_bridge.etype;
			ret = foe_ext[foe_index].ds_offload;
		}
		else if(IS_IPV4_DSLITE(foe_entry) || IS_IPV6_3T_ROUTE(foe_entry) || IS_IPV6_5T_ROUTE(foe_entry) || IS_IPV6_6RD(foe_entry)){
			*dp = foe_entry->ipv6_5t_route.etype;
			ret = foe_ext[foe_index].ds_offload;
		}
	}

	return ret;
}

int PpeRestoreRtspIpPort(struct sk_buff * skb, int calc_sum)
{
	struct iphdr *iph = NULL;
	struct tcphdr *th = NULL;
	int datalen;
	struct FoeEntry *foe_entry;
	foe_entry = getPpeEntry(FOE_ENTRY_NUM(skb));
	if(foe_entry == NULL)
		return -1;

	if(skb->protocol == htons(ETH_P_IP))
	{
		iph = (struct iphdr *)skb->data;
		datalen = skb->len - iph->ihl*4;
		iph->daddr = foe_entry->ipv4_hnapt.dip;
		iph->saddr = foe_entry->ipv4_hnapt.sip;
		iph->check = 0;
        if (calc_sum)
        {
		    iph->check = ip_fast_csum((u8 *)iph, iph->ihl);
        }
		if (iph->protocol == IPPROTO_TCP)
		{
			th = (struct tcphdr *)((uint8_t *) iph + (iph->ihl * 4));
			th->dest = foe_entry->ipv4_hnapt.dport;
			th->source = foe_entry->ipv4_hnapt.sport;
			th->check = 0;
            if (calc_sum)
            {
			    th->check = csum_tcpudp_magic(iph->saddr, iph->daddr, datalen, IPPROTO_TCP, csum_partial(th, datalen, 0));
            }
		}
	}

	return 0;
}

struct net_device *ra_dev_get_by_name(const char *name)
{
	struct net *net_ns = NULL;
	if ((1 == TCSUPPORT_SDN_OVS_VAL) && (0 != NS_PID) && ('r' == name[0])) {
		net_ns = __get_net_ns_by_pid(NS_PID);
		if(NULL != net_ns)
		{
			return dev_get_by_name(net_ns, name);
		}		
	}
	return dev_get_by_name(&init_net, name);
}

static void PpeSetDstPort(uint32_t Ebl)
{
	int i;
	char eth_name[10];
    
	if (Ebl) {
		if((1 == TCSUPPORT_SDN_OVS_VAL) && (0 != NS_PID))
		{
			DstPort[DP_RA0] = ra_dev_get_by_name("ra0");
			DstPort[DP_RA1] = ra_dev_get_by_name("ra1");
			DstPort[DP_RA2] = ra_dev_get_by_name("ra2");
			DstPort[DP_RA3] = ra_dev_get_by_name("ra3");
			DstPort[DP_RA4] = ra_dev_get_by_name("ra4");
			DstPort[DP_RA5] = ra_dev_get_by_name("ra5");
			DstPort[DP_RA6] = ra_dev_get_by_name("ra6");
			DstPort[DP_RA7] = ra_dev_get_by_name("ra7");
		}
		else
		{
			for(i = 0 ; i <= 7 ; i++)
			{
				DstPort[DP_RA0 + i] = get24GWifiName(i);
			}
		}

		if(TCSUPPORT_FH_SDN_PON_VAL)
		{
			DstPort[DP_GMAC] = ra_dev_get_by_name("eth");
		}
		else
		{
			DstPort[DP_GMAC] = ra_dev_get_by_name("eth0");
		}

		DstPort[DP_PTM0] = ra_dev_get_by_name("ptm0");
		DstPort[DP_PTM1] = ra_dev_get_by_name("ptm1");
		DstPort[DP_PON] = ra_dev_get_by_name("pon");

		DstPort[DP_WDS0] = ra_dev_get_by_name("wds0");
		DstPort[DP_WDS1] = ra_dev_get_by_name("wds1");
		DstPort[DP_WDS2] = ra_dev_get_by_name("wds2");
		DstPort[DP_WDS3] = ra_dev_get_by_name("wds3");
		DstPort[DP_WDSI0] = ra_dev_get_by_name("wdsi0");
		DstPort[DP_WDSI1] = ra_dev_get_by_name("wdsi1");
		DstPort[DP_WDSI2] = ra_dev_get_by_name("wdsi2");
		DstPort[DP_WDSI3] = ra_dev_get_by_name("wdsi3");

		DstPort[DP_APCLI0] = ra_dev_get_by_name("apcli0");
		DstPort[DP_APCLII0] = ra_dev_get_by_name("apclii0");
		DstPort[DP_APCLIX0] = ra_dev_get_by_name("apclix0");

        memset(eth_name, 0, sizeof(eth_name));
        for(i=0;i<8;i++)
        {
            sprintf(eth_name, "%s%d", lanNamePre(), i+itf_start_idx);
            DstPort[DP_GMAC1 + i] = ra_dev_get_by_name(eth_name);
        }


	}else{
		for(i=0;i<WLAN_IF_NUM;i++){
			if(DstPort[DP_RA0 + i] != NULL){	
				dev_put(DstPort[DP_RA0 + i]);
				DstPort[DP_RA0 + i] = NULL;
			}
		}
		
		if(DstPort[DP_GMAC] != NULL){
			dev_put(DstPort[DP_GMAC]);
			DstPort[DP_GMAC] = NULL;
		}

        if(DstPort[DP_GMAC1] != NULL){
			dev_put(DstPort[DP_GMAC1]);
			DstPort[DP_GMAC1] = NULL;
		}

        if(DstPort[DP_GMAC2] != NULL){
			dev_put(DstPort[DP_GMAC2]);
			DstPort[DP_GMAC2] = NULL;
		}
        
        if(DstPort[DP_GMAC3] != NULL){
			dev_put(DstPort[DP_GMAC3]);
			DstPort[DP_GMAC3] = NULL;
		}
        
        if(DstPort[DP_GMAC4] != NULL){
			dev_put(DstPort[DP_GMAC4]);
			DstPort[DP_GMAC4] = NULL;
		}
        
        if(DstPort[DP_GMAC5] != NULL){
			dev_put(DstPort[DP_GMAC5]);
			DstPort[DP_GMAC5] = NULL;
		}
        
        if(DstPort[DP_GMAC6] != NULL){
			dev_put(DstPort[DP_GMAC6]);
			DstPort[DP_GMAC6] = NULL;
		}

		if(DstPort[DP_GMAC7] != NULL){
			dev_put(DstPort[DP_GMAC7]);
			DstPort[DP_GMAC7] = NULL;
		}
        
        if(DstPort[DP_GMAC8] != NULL){
			dev_put(DstPort[DP_GMAC8]);
			DstPort[DP_GMAC8] = NULL;
		}

		DstPort[DP_WDS0] = NULL;
		DstPort[DP_WDS1] = NULL;
		DstPort[DP_WDS2] = NULL;
		DstPort[DP_WDS3] = NULL;
		DstPort[DP_WDSI0] = NULL;
		DstPort[DP_WDSI1] = NULL;
		DstPort[DP_WDSI2] = NULL;
		DstPort[DP_WDSI3] = NULL;

		DstPort[DP_APCLI0] = NULL;
		DstPort[DP_APCLII0] = NULL;
		DstPort[DP_APCLIX0] = NULL;
	}	
}

struct net_device* ppeGetDstPort(unsigned int dp)
{
	if(dp >= MAX_IF_NUM)
		return NULL;
	
	return DstPort[dp];
}

static int ppe_device_event(struct notifier_block *unused, unsigned long event, void *ptr);

struct notifier_block ppe_device_notifier = {
        .notifier_call = ppe_device_event
};

static int ppe_device_event(struct notifier_block *unused, unsigned long event, void *ptr)
{
	int i;
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

        switch (event) {
        case NETDEV_REGISTER:
		if (is24GWiFiInterface(dev) && //for ra
			((get24GLogicWifiIndex(dev) >= 0) && (get24GLogicWifiIndex(dev) <= 7))) 
		{
			if ( DstPort[DP_RA0 + get24GLogicWifiIndex(dev)] == NULL)
				  DstPort[DP_RA0 + get24GLogicWifiIndex(dev)]=ra_dev_get_by_name(dev->name);
		} 
		if (is5GWiFiInterface(dev) && //for rai
			((get5GLogicWifiIndex(dev) >= 0) && (get5GLogicWifiIndex(dev) <= 7))) 
		{
				DstPort[DP_RAI0 + get5GLogicWifiIndex(dev)]=ra_dev_get_by_name(dev->name);
		}
		if (is6GWiFiInterface(dev) && //for rax 6G
			((get6GLogicWifiIndex(dev) >= 0) && (get6GLogicWifiIndex(dev) <= 7))) 
		{
			DstPort[DP_RAX0 + get6GLogicWifiIndex(dev)] = ra_dev_get_by_name(dev->name);
		}

		if (isLANInterface(dev) && (getLogicLANIndex(dev) >= 0)) 
		{
			if (DstPort[DP_GMAC1 + getLogicLANIndex(dev)] == NULL)
			{
				  DstPort[DP_GMAC1 + getLogicLANIndex(dev)] = ra_dev_get_by_name(dev->name);
			}
		} 

		if ((strlen(dev->name) == 4) && (strncmp(dev->name,"wds", 3) == 0) && //for wds
			((dev->name[3] >= '0') && (dev->name[3] <= '3'))) 
		{
       			DstPort[DP_WDS0 + (dev->name[3] - '0')]=ra_dev_get_by_name(dev->name);
		} 
		if ((strlen(dev->name) == 5) && (strncmp(dev->name,"wdsi", 4) == 0) && //for wdsi
			((dev->name[4] >= '0') && (dev->name[4] <= '3'))) 
		{
       			DstPort[DP_WDSI0 + (dev->name[4] - '0')]=ra_dev_get_by_name(dev->name);
		} 
		if ((strlen(dev->name) == 6) && (strncmp(dev->name,"apcli", 5) == 0) && //for apcli
			((dev->name[5] >= '0') && (dev->name[5] <= '3'))) 
		{
       			DstPort[DP_APCLI0+ (dev->name[5] - '0')]=ra_dev_get_by_name(dev->name);
		} 
		if ((strlen(dev->name) == 7) && (strncmp(dev->name,"apclii", 6) == 0) && //for apclii
			((dev->name[6] >= '0') && (dev->name[6] <= '3'))) 
		{
       			DstPort[DP_APCLII0 + (dev->name[6] - '0')]=ra_dev_get_by_name(dev->name);
		} 
		if ((strlen(dev->name) == 7) && (strncmp(dev->name,"apclix", 6) == 0) && //for apclix
			((dev->name[6] >= '0') && (dev->name[6] <= '3'))) 
		{
			DstPort[DP_APCLIX0 + (dev->name[6] - '0')] = ra_dev_get_by_name(dev->name);
		}

		if ((dev->type == ARPHRD_IPGRE) && (0 != strcmp(dev->name, "gre0")))
		{
			for (i = DP_GRE_TUNNEL_0; i <= DP_GRE_TUNNEL_7; i++)
			{
				if (NULL == DstPort[i])
					break;
			}

			if (i <= DP_GRE_TUNNEL_7)
			{
				DstPort[i] = ra_dev_get_by_name(dev->name);
			}
		}

		if ((0 == strncmp(dev->name, "gre_p",5))&& (dev->name[7]!='.'))
        {
            for (i = DP_GRE_L2_PRI_TUNNEL_0; i <= DP_GRE_L2_PRI_TUNNEL_7; i++)
            {
                if (NULL == DstPort[i])
                    break;
            }

            if (i <= DP_GRE_L2_PRI_TUNNEL_7)
            {
                DstPort[i] = ra_dev_get_by_name(dev->name);
            }
        }

		if ((0 == strncmp(dev->name, "gre_s",5))&& (dev->name[7]!='.'))
        {
            for (i = DP_GRE_L2_SEC_TUNNEL_0; i <= DP_GRE_L2_SEC_TUNNEL_7; i++)
            {
                if (NULL == DstPort[i])
                    break;
            }

            if (i <= DP_GRE_L2_SEC_TUNNEL_7)
            {
                DstPort[i] = ra_dev_get_by_name(dev->name);
            }
        }
	
        
        if (dev->dev.type && !strcmp(dev->dev.type->name, "vxlan"))
        {
            for (i = DP_VXLAN_0; i <= DP_VXLAN_19; i++)
            {
                if (NULL == DstPort[i])
                    break;
            }

            if (i <= DP_VXLAN_19)
            {
                DstPort[i] = ra_dev_get_by_name(dev->name);
            }
        }

		if (0 == strncmp(dev->name, "eth1", 4))
		{
			DstPort[DP_USBNET2] = ra_dev_get_by_name("eth1");//add for usb dongle			
		}

		if (0 == strncmp(dev->name, "nas", 3))
		{
			for (i = DP_NAS0; i <= DP_NAS7; i++)
			{
				if (NULL == DstPort[i])
					break;
			}

			if (i <= DP_NAS7)
			{
				DstPort[i] = ra_dev_get_by_name(dev->name);
			}

			if(TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
            {
                for (i = DP_MCAST_NAS0; i <= DP_MCAST_NAS7; i++)
                {
                    if (NULL == DstPort[i])
                        break;
                }
            
                if (i <= DP_MCAST_NAS7)
                {
                    DstPort[i] = ra_dev_get_by_name(dev->name);
                }
            }
		}

		if ((isPPPInterface(dev) || isPPTPInterface(dev) || isL2TPInterface(dev))&&(TCSUPPORT_GRE_TUNNEL_HWNAT_VAL||\
			TCSUPPORT_VXLAN_HW_OFFLOAD_VAL))
		{
			for (i = DP_PPP0; i <= DP_PPP7; i++)
			{
				if (NULL == DstPort[i])
					break;
			}

			if (i <= DP_PPP7)
			{
				DstPort[i] = ra_dev_get_by_name(dev->name);
			}

			if(TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
            {
                for (i = DP_MCAST_PPP0; i <= DP_MCAST_PPP7; i++)
                {
                    if (NULL == DstPort[i])
                        break;
                }
	        
                if (i <= DP_MCAST_PPP7)
                {
                    DstPort[i] = ra_dev_get_by_name(dev->name);
                }
            }
		}

		if(TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
        {
            /* LAN: vethX, WAN: vethX_Y */
            if ( (0 == strncmp(dev->name, "veth", 4)) && (NULL == strstr(dev->name, "_")) )
            {
                sscanf(dev->name + 4, "%d", &i);
                if ((i >= 1) && (i <= 4094))
                {
                    g_veth_lan_device[i] = ra_dev_get_by_name(dev->name);
                    printk("Info: Device %s registered %d!\n", dev->name, i);
                }
            }
        }

#ifdef CONFIG_TP_IMAGE
		if ((isUSBInterface(dev) || (0 == strcmp(dev->name, USB_4G_DEV)))
		 && (getUSBIndex(dev) >= 0 && getUSBIndex(dev) < 1))
#else
		if (isUSBInterface(dev) && (getUSBIndex(dev) >= 0 && getUSBIndex(dev) < 1))
#endif /* CONFIG_TP_IMAGE */
		{
			DstPort[DP_USBNET0 + getUSBIndex(dev)] = dev;//add for usb dongle        
		}

		if(isXSIInterface(dev))
			DstPort[DP_XSI] = dev;

        if (isMAPInterface(dev))
        {
            DstPort[DP_MAP_E_US] = ra_dev_get_by_name(dev->name);
        }

			
		break;
        case NETDEV_UNREGISTER:
		if (is24GWiFiInterface(dev) &&
			((get24GLogicWifiIndex(dev) >= 0) && (get24GLogicWifiIndex(dev) <= 7))) 
		{
			if (DstPort[DP_RA0 + get24GLogicWifiIndex(dev)] != NULL){
				dev_put(DstPort[DP_RA0 + get24GLogicWifiIndex(dev)]);
				DstPort[DP_RA0 + get24GLogicWifiIndex(dev)] = NULL;
			}
		}
		if (is5GWiFiInterface(dev) &&
			((get5GLogicWifiIndex(dev) >= 0) && (get5GLogicWifiIndex(dev) <= 7))) 
		{
			if (DstPort[DP_RAI0 + get5GLogicWifiIndex(dev)] != NULL){
				dev_put(DstPort[DP_RAI0 + get5GLogicWifiIndex(dev)]);
				DstPort[DP_RAI0 + get5GLogicWifiIndex(dev)] = NULL;
			}
		} 
		if (is6GWiFiInterface(dev) &&
			((get6GLogicWifiIndex(dev) >= 0) && (get6GLogicWifiIndex(dev) <= 7))) 
		{
			if (DstPort[DP_RAX0 + get6GLogicWifiIndex(dev)] != NULL){
				dev_put(DstPort[DP_RAX0 + get6GLogicWifiIndex(dev)]);
				DstPort[DP_RAX0 + get6GLogicWifiIndex(dev)] = NULL;
			}
		}
		if (isLANInterface(dev) && (getLogicLANIndex(dev) >= 0)) 
		{
			if (DstPort[DP_GMAC1 + getLogicLANIndex(dev)] != NULL)
			{
				dev_put(DstPort[DP_GMAC1 + getLogicLANIndex(dev)]);
				DstPort[DP_GMAC1 + getLogicLANIndex(dev)] = NULL;
			}
		} 

		if ((strlen(dev->name) == 4) && (strncmp(dev->name,"wds", 3) == 0) && 
			((dev->name[3] >= '0') && (dev->name[3] <= '3'))) 
		{
			if (DstPort[DP_WDS0 + (dev->name[3] - '0')] != NULL){
				dev_put(DstPort[DP_WDS0 + (dev->name[3] - '0')]);
				DstPort[DP_WDS0 + (dev->name[3] - '0')] = NULL;
			}
		}
		if ((strlen(dev->name) == 5) && (strncmp(dev->name,"wdsi", 4) == 0) && 
			((dev->name[4] >= '0') && (dev->name[4] <= '3'))) 
		{
			if (DstPort[DP_WDSI0 + (dev->name[4] - '0')] != NULL){
				dev_put(DstPort[DP_WDSI0 + (dev->name[4] - '0')]);
				DstPort[DP_WDSI0 + (dev->name[4] - '0')] = NULL;
			}
		} 
		if ((strlen(dev->name) == 6) && (strncmp(dev->name,"apcli", 5) == 0) && 
			((dev->name[5] >= '0') && (dev->name[5] <= '3'))) 
		{
			if (DstPort[DP_APCLI0 + (dev->name[5] - '0')] != NULL){
				dev_put(DstPort[DP_APCLI0 + (dev->name[5] - '0')]);
				DstPort[DP_APCLI0 + (dev->name[5] - '0')] = NULL;
			}
		}
		if ((strlen(dev->name) == 7) && (strncmp(dev->name,"apclii", 6) == 0) && 
			((dev->name[6] >= '0') && (dev->name[6] <= '3'))) 
		{
			if (DstPort[DP_APCLII0 + (dev->name[6] - '0')] != NULL){
				dev_put(DstPort[DP_APCLII0 + (dev->name[6] - '0')]);
				DstPort[DP_APCLII0 + (dev->name[6] - '0')] = NULL;
			}
		} 
		if ((strlen(dev->name) == 7) && (strncmp(dev->name,"apclix", 6) == 0) && 
			((dev->name[6] >= '0') && (dev->name[6] <= '3'))) 
		{
			if (DstPort[DP_APCLIX0 + (dev->name[6] - '0')] != NULL){
				dev_put(DstPort[DP_APCLIX0 + (dev->name[6] - '0')]);
				DstPort[DP_APCLIX0 + (dev->name[6] - '0')] = NULL;
			}
		}

		if (0 == strncmp(dev->name, "eth1", 4))
		{
			dev_put(DstPort[DP_USBNET2]);
			DstPort[DP_USBNET2] = NULL;
		}

		if ((dev->type == ARPHRD_IPGRE) && (0 != strcmp(dev->name, "gre0")))
		{
			for (i = DP_GRE_TUNNEL_0; i <= DP_GRE_TUNNEL_7; i++)
			{
				if (dev == DstPort[i])
					break;
			}

			if (i <= DP_GRE_TUNNEL_7)
			{
				dev_put(DstPort[i]);
				DstPort[i] = NULL;
			}
		}


		if (0 == strncmp(dev->name, "gre_p",5))
        {
           for (i = DP_GRE_L2_PRI_TUNNEL_0; i <= DP_GRE_L2_PRI_TUNNEL_7; i++)
            {
                if (dev == DstPort[i])
                    break;
            }

            if (i <= DP_GRE_L2_PRI_TUNNEL_7)
            {
                dev_put(DstPort[i]);
                DstPort[i] = NULL;
            }
        }

		if (0 == strncmp(dev->name, "gre_s",5))
        {
           for (i = DP_GRE_L2_SEC_TUNNEL_0; i <= DP_GRE_L2_SEC_TUNNEL_7; i++)
            {
                if (dev == DstPort[i])
                    break;
            }

            if (i <= DP_GRE_L2_SEC_TUNNEL_7)
            {
                dev_put(DstPort[i]);
                DstPort[i] = NULL;
            }
        }

        if (dev->dev.type && !strcmp(dev->dev.type->name, "vxlan"))
        {
            for (i = DP_VXLAN_0; i <= DP_VXLAN_19; i++)
            {
                if (dev == DstPort[i])
                    break;
            }

            if (i <= DP_VXLAN_19)
            {
                dev_put(DstPort[i]);
                DstPort[i] = NULL;
            }
        }

		if (0 == strncmp(dev->name, "nas", 3))
		{
			for (i = DP_NAS0; i <= DP_NAS7; i++)
			{
				if (dev == DstPort[i])
					break;
			}

			if (i <= DP_NAS7)
			{
				dev_put(DstPort[i]);
				DstPort[i] = NULL;
			}

			if(TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
            {
                for (i = DP_MCAST_NAS0; i <= DP_MCAST_NAS7; i++)
                {
                    if (dev == DstPort[i])
                        break;
                }

                if (i <= DP_MCAST_NAS7)
                {
                    dev_put(DstPort[i]);
                    DstPort[i] = NULL;
                }
            }
            
            #if defined  (CONFIG_RA_HW_NAT_VPN_PASSTHROUGH)     
            for (i = 0; i < (DP_CRYPTO_D_MAX - DP_CRYPTO_D_0); i++)
            {
                if (dev == vpn_decrypt_src_dev[i])
                    vpn_decrypt_src_dev[i] = NULL;
            }
            #endif
		}


		if ((isPPPInterface(dev) || isPPTPInterface(dev) || isL2TPInterface(dev))&&(TCSUPPORT_GRE_TUNNEL_HWNAT_VAL||\
			TCSUPPORT_VXLAN_HW_OFFLOAD_VAL))
		{
			for (i = DP_PPP0; i <= DP_PPP7; i++)
			{
				if (dev == DstPort[i])
					break;
			}

			if (i <= DP_PPP7)
			{
				dev_put(DstPort[i]);
				DstPort[i] = NULL;
			}

			if(TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
            {
                for (i = DP_MCAST_PPP0; i <= DP_MCAST_PPP7; i++)
                {
                    if (dev == DstPort[i])
                        break;
                }
	            
                if (i <= DP_MCAST_PPP7)
                {
                    dev_put(DstPort[i]);
                    DstPort[i] = NULL;
                }
            }
            #if defined  (CONFIG_RA_HW_NAT_VPN_PASSTHROUGH)     
            for (i = 0; i < (DP_CRYPTO_D_MAX - DP_CRYPTO_D_0); i++)
            {
                if (dev == vpn_decrypt_src_dev[i])
                    vpn_decrypt_src_dev[i] = NULL;
            }
            #endif
		}

		if(TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
        {
            /* LAN: vethX, WAN: vethX_Y */
            if ( (0 == strncmp(dev->name, "veth", 4)) && (NULL == strstr(dev->name, "_")) )
            {
                sscanf(dev->name + 4, "%d", &i);
                if ((i >= 1) && (i <= 4094))
                {
                    printk("Info: Device %s unregistered %d!\n", dev->name, i);
                    dev_put(g_veth_lan_device[i]);
                    g_veth_lan_device[i] = NULL;
                }
            }
        }

#ifdef CONFIG_TP_IMAGE
		if ((isUSBInterface(dev) || (0 == strcmp(dev->name, USB_4G_DEV)))
		 && (getUSBIndex(dev) >= 0 && getUSBIndex(dev) < 1))
#else
		if (isUSBInterface(dev) && (getUSBIndex(dev) >= 0 && getUSBIndex(dev) < 1))
#endif /* CONFIG_TP_IMAGE */
		{
			DstPort[DP_USBNET0 + getUSBIndex(dev)] = NULL;
		}
		if(isXSIInterface(dev))
			DstPort[DP_XSI] = NULL;

        if (isMAPInterface(dev))
        {
            DstPort[DP_MAP_E_US] = NULL;
        }

		if(dev == l2tp_vpn_dev)
			l2tp_vpn_dev = NULL;
						
		break;

		case NETDEV_UP:
		if(TCSUPPORT_WAN_ETHER_VAL && !ptm_etherwan_coexist && !isEN7523){
			if ((strlen(dev->name) == 5) && (strncmp(dev->name,ETHERWAN_ITF, 5) == 0)){
				SetEtherWANQdma(1); /* Ether Wan Mode, use QDMA WAN */ 
			}
			else if ((strlen(dev->name) == 4) && (strncmp(dev->name,XDSL_ITF_PRE, 3) == 0)){
				SetEtherWANQdma(0);
			}
		}
		if(TCSUPPORT_XPON_HAL_API_EXT_VAL){
			if ((strlen(dev->name) == 6) && (strncmp(dev->name,AWIFI_ITF, 6) == 0)){
				PpeAWifiClearAWifiInfo();
				br_ip_info.brlan_exist = 1;
			}
		}
			if(ra_sw_wifi_hook_wifi_up_handle)
				ra_sw_wifi_hook_wifi_up_handle(dev);
		break;

		case NETDEV_DOWN:
		if(TCSUPPORT_XPON_HAL_API_EXT_VAL){
			if ((strlen(dev->name) == 6) && (strncmp(dev->name,AWIFI_ITF, 6) == 0)){
				br_ip_info.brlan_exist = 0;
			}
		}
		break;
		
		case NETDEV_GOING_DOWN:
			if(ra_sw_wifi_hook_wifi_down_handle)
				ra_sw_wifi_hook_wifi_down_handle(dev);
		break;
		
		default :      
		break;
        }

        return NOTIFY_DONE;
}

void remove_skb_data_vlan_tag(struct sk_buff *skb)
{
	unsigned short ether_type = 0;
	struct ethhdr *ehdr = NULL;

	ehdr = (struct ethhdr *)(skb->data);
	ether_type = ntohs(ehdr->h_proto);
	
	if (ether_type == ETH_P_8021Q) {
		memmove(skb->data+4, skb->data, 12);
		skb_pull(skb, 4);
	}
}

int ecnt_wlan_force_to_cpu_handler(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para,struct FoeEntry *foe_entry)
{
	int devtype = 0;
	int ratelimit_index = -1;
	
	skb_set_network_header(skb, 0);

	skb_push(skb, ETH_HLEN);	//pointer to layer2 header

	skb->dev = ppeGetDstPort(dp);

	if ((skb->dev == NULL) || (!(skb->dev->flags & IFF_UP)))
	{
	    if(printk_ratelimit())
		    printk("Error: the static rule is error!dp = %d\n",dp);
		kfree_skb(skb);
		return 0;
	}
	
	if (rps_enable)
	{
		if (is24GWiFiInterface(skb->dev))
			devtype = 1;
		if (is5GWiFiInterface(skb->dev))
			devtype = 2;
	
		if ((devtype == rps_enable) && (p_para->ds_offload != RTSP_OFFLOAD_WLAN))
		{
			FOE_MAGIC_TAG(skb) = 0xbeaf;
			return 1;
		}
	}

	if(soft_ratelimit_enqueue_hook &&(isWiFiInterface(skb->dev))&&(wifiqueue_start_idx!=-1))
	{
		ratelimit_index = getWifiQueueIndex(skb->dev,WIFI_RATELIMIT_TX);

		if(ratelimit_index!=-1)
			if(1 == soft_ratelimit_enqueue_hook(skb,ratelimit_index))/*wifi tx*/
				return 0;

	}

	if(TxShortFlag == 0 && TCSUPPORT_WLAN_SW_RPS_VAL && toWlan5GPktRpsHandle_hook){ 
		if(0==toWlan5GPktRpsHandle_hook(skb))
		return 0;
	} 

	if(TCSUPPORT_WLAN_SW_RPS_VAL && toWlan2GPktRpsHandle_hook){ 
		if(0==toWlan2GPktRpsHandle_hook(skb))
		return 0;
	} 

    if ( is_multicast_ether_addr(eth_hdr(skb)->h_dest) ) {
	remove_skb_data_vlan_tag(skb);
	}
    skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);

	return 0;
}

int ecnt_offload_ipsec_passthrough(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{		
    skb->pkt_type = PACKET_HOST;
    if ((dp >= DP_CRYPTO_D_0) && vpn_decrypt_src_dev[dp - DP_CRYPTO_D_0])
    {
        skb->dev = vpn_decrypt_src_dev[dp - DP_CRYPTO_D_0];
    }
    
	if(ECNT_RETURN == CRYPTO_API_ESP_PKTPUT_PT_SPEED(ECNT_CRYPTO, skb, dp)){
		return 0;
	}

	kfree_skb(skb);
	return 0;
}

int ecnt_offload_ds_offload(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
		skb->dev = ppeGetDstPort(DP_GMAC);

	skb_set_network_header(skb, 0);

	skb_push(skb, ETH_HLEN);	//pointer to layer2 header

	if (skb->dev == NULL || (!(skb->dev->flags & IFF_UP)))
	{
		kfree_skb(skb);
		return 0;
	}

	skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
	
	return 0;
}

int ecnt_offload_local_in(struct sk_buff *skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
    int protocol = 0;

    if (IPV4_HNAPT == foe_entry->bfib1.pkt_type)
    {
        protocol = (foe_entry->ipv4_hnapt.info_blk1 >> 30) & 0x1;
    }
    
    if ((IPV4_HNAPT == foe_entry->bfib1.pkt_type) && (foe_entry->ipv4_hnapt.dip > 0xe0000000) && protocol)
    {
        skb->dev = foe_ext[FOE_ENTRY_NUM(skb)].dev_info;
    }
    
	return localInShortCut(skb, protocol);
}

int ecnt_offload_soft_ratelimit(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
	if(macqueue_start_idx!=-1  && soft_ratelimit_enqueue_hook)
		skb->dev = ppeGetDstPort(dp);
	
	if(skb->macEnque & (1<<7) && soft_ratelimit_enqueue_hook)
	{
		if(DebugLevel==1)
			printk("mac_queue_start:%d  nacqueue:%d\n",macqueue_start_idx,skb->macEnque&0xf);
		if(1 == soft_ratelimit_enqueue_hook(skb,(skb->macEnque&0xf)+macqueue_start_idx))
			return 0;
	}

	if (skb->dev == NULL || (!(skb->dev->flags & IFF_UP)))
	{
		kfree_skb(skb);
		return 0;
	}

	skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);

	return 0;
}

int ecnt_offload_gre_vxlan_wan_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
    if ((foe_entry->bfib1.pkt_type == IPV4_HNAPT) || (foe_entry->bfib1.pkt_type == IPV4_HNAT))
    {
        const struct iphdr *iph;
        const struct udphdr *uh;
		int ret = 0;

        skb->protocol = htons(ETH_P_IP);

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
        skb_set_network_header(skb, 0);
#else
        LAYER3_HEADER(skb) = skb->data;
#endif

        iph = ip_hdr(skb);
        if ((iph->protocol == IPPROTO_UDP) 
            && (!(ntohs(iph->frag_off) & IP_MF))) /* fragment pkt goes to kernel*/
        {
            int i, vni;
            struct net_device* dev_tmp;

            uh = (struct udphdr *)((uint8_t *) iph + iph->ihl * 4);
            vni = (ntohl(*(int*)((uint8_t *)uh+8+4)) >> 8) & 0x00FFFFFF;

            
            dev_tmp = vxlan_getlink(vni);
            if (dev_tmp)
            {
                for (i = DP_VXLAN_0; i<= DP_VXLAN_19; i++)
                {
                    if (dev_tmp == DstPort[i])
                        break;
                }
                if (i <= DP_VXLAN_19)
                {
                    skb_pull(skb, 20 + 8 + 8);
                    skb->dev = DstPort[i];
                    ra_sw_nat_hook_set_magic(skb, FOE_MAGIC_VXLAN_DOWN_2);
                    ra_sw_nat_hook_sendto_ppe(skb);
                    return 0;
                }
            }
        }else if(iph->protocol == IPPROTO_GRE){
			if(hwnat_gre_fast_down_hook){
				ret = hwnat_gre_fast_down_hook(skb);
				if(ECNT_RETURN ==ret )
					return 0;	
			}
		}
    }
    
	skb->dev = ppeGetDstPort(dp);
    skb->pkt_type = PACKET_HOST;
	netif_receive_skb(skb);
	return 0;
}

int ecnt_offload_gre_tunnel_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
	int ret=0;
	u32 len;
	const struct iphdr *iph;
	if ((dp >= DP_GRE_L2_PRI_TUNNEL_0) && (dp <= DP_GRE_L2_SEC_TUNNEL_7))
	{
		if(hwnat_gre_fast_up_hook){
			ret = hwnat_gre_fast_up_hook(skb);
			if (ECNT_RETURN == ret)
			{
				return 0;
			}
		}
	}
	iph = ip_hdr(skb);
	len = ntohs(iph->tot_len);
	
	if (pskb_trim_rcsum(skb, len))
	{
		kfree_skb(skb);
		return 0;
	}

	skb_set_network_header(skb, 0);
	
	skb->dev = ppeGetDstPort(dp);
	
	if((skb->dev == NULL) || (skb->dev->type != ARPHRD_IPGRE))
		skb_push(skb, ETH_HLEN);	//pointer to layer2 header

	if ((skb->dev == NULL) || (!(skb->dev->flags & IFF_UP)))
	{
	    if(printk_ratelimit())
		    printk("Error: the static rule is error!dp = %d\n",dp);
		kfree_skb(skb);
		return 0;
	}

	skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);

	return 0;
}

int ecnt_offload_vxlan_tunnel_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
    int ret;
    struct net_data_s net_data;

	skb->dev = ppeGetDstPort(dp);

    net_data.pskb = &skb;
    ret =  ECNT_VXLAN_GENERIC_HOOK(ECNT_NET_VXLAN_FAST_XMIT, &net_data);
    if (ECNT_RETURN == ret)
    {
        return 0;
    }

	skb_set_network_header(skb, 0);
	
	if((skb->dev == NULL) || (skb->dev->type != ARPHRD_IPGRE))
		skb_push(skb, ETH_HLEN);	//pointer to layer2 header

	if ((skb->dev == NULL) || (!(skb->dev->flags & IFF_UP)))
	{
	    if(printk_ratelimit())
		    printk("Error: the static rule is error!dp = %d\n",dp);
		kfree_skb(skb);
		return 0;
	}

	skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);

	return 0;
}

int ecnt_offload_asym_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
	if((DP_ASYM_D1 <= foe_entry->ipv4_hnapt.act_dp)&&( foe_entry->ipv4_hnapt.act_dp <= DP_ASYM_D5))
	{
		ECNT_ASYM_API_PKT_HANDLER(&skb,ECNT_ASYM_PKT_PHASE_HW_D);
	
		if(foe_ext[FOE_ENTRY_NUM(skb)].asym_wlan_dp)
		{
			skb->dev = DstPort[foe_ext[FOE_ENTRY_NUM(skb)].asym_wlan_dp];
			skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
			return 0;
		}
		
		skb = ppe_vlan_put_tag(skb,foe_entry->ipv4_hnapt.act_dp);
		if(!skb)
			return 0;
	
		QDMA_API_TRANSMIT_PACKETS_WIFI_FAST(ECNT_QDMA_LAN,skb,0);
		
		return 0;
	}
	else if((DP_ASYM_U1 <= foe_entry->ipv4_hnapt.act_dp)&&(foe_entry->ipv4_hnapt.act_dp <= DP_ASYM_U5))
	{
		ECNT_ASYM_API_PKT_HANDLER(&skb,ECNT_ASYM_PKT_PHASE_HW_U);
		skb = ppe_vlan_put_tag(skb,foe_entry->ipv4_hnapt.act_dp);
		if(!skb)
			return 0;
	
		QDMA_API_TRANSMIT_PACKETS_WIFI_FAST(ECNT_QDMA_LAN,skb,0);
		
		return 0;
	}

	return 0;
}

int ecnt_offload_default_offload(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
	skb_set_network_header(skb, 0);

	skb->dev = ppeGetDstPort(dp);
	
	if (skb->dev == NULL || (!(skb->dev->flags & IFF_UP)))
	{
		kfree_skb(skb);
		return 0;
	}

	skb_push(skb, ETH_HLEN);

	skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);

	return 0;
}

/*****************natv6 offload start************************/
static int  ppeNatV6ReSumUdpCheck(struct ipv6hdr* ip6h,struct udphdr* uh,ppeNatV6Info_t *pinfo,unsigned int attr)
{
	uint16 org_chksum = htons(uh->check);
	uint16 org_source = htons(uh->source);
	uint16 org_dest = htons(uh->dest);
	uint32 tmp = ~org_chksum&0xFFFF;
	int i = 0;

	if(attr&(1<<NATV6_ATTR_SIP))
	{
		for(i = 0;i < 8;i++)
		{	
			tmp+=(~(htons(ip6h->saddr.s6_addr16[i]))&0xFFFF);
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
			tmp+=(htons(pinfo->ipv6_sip[i])&0xFFFF);			
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
		}
	}

	if(attr&(1<<NATV6_ATTR_DIP))
	{	
		for(i = 0;i < 8;i++)
		{
			tmp+=(~(htons(ip6h->daddr.s6_addr16[i]))&0xFFFF);
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
			tmp+=(htons(pinfo->ipv6_dip[i])&0xFFFF);
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
		}
	}

	if(attr&(1 << NATV6_ATTR_UDP_SPORT))
	{		
		tmp+=(~org_source&0XFFFF);		
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
		tmp+=(htons(pinfo->sport)&0xFFFF);		
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
	}

	if(attr&(1<<NATV6_ATTR_UDP_DPORT))
	{
		tmp+=(~org_dest&0XFFFF);		
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
		tmp+=(htons(pinfo->dport)&0xFFFF);
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
	}
	
	tmp = ~tmp & 0xFFFF;
	uh->check = htons(tmp);

	return 0;
}

static int  ppeNatV6ReSumTcpCheck(struct ipv6hdr* ip6h,struct tcphdr* th,ppeNatV6Info_t *pinfo,unsigned int attr)
{
	uint16 org_chksum = htons(th->check);
	uint16 org_source = htons(th->source);
	uint16 org_dest = htons(th->dest);
	uint32 tmp = ~org_chksum&0xFFFF;
	int i = 0;

	if(attr&(1<<NATV6_ATTR_SIP))
	{
		for(i = 0;i < 8;i++)
		{
			tmp+=(~(htons(ip6h->saddr.s6_addr16[i]))&0xFFFF);
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
			tmp+=(htons(pinfo->ipv6_sip[i])&0xFFFF);			
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
		}
	}

	if(attr&(1<<NATV6_ATTR_DIP))
	{
		for(i = 0;i < 8;i++)
		{
			tmp+=(~(htons(ip6h->daddr.s6_addr16[i]))&0xFFFF);
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
			tmp+=(htons(pinfo->ipv6_dip[i])&0xFFFF);
			tmp = (tmp >> 16) + (tmp & 0xFFFF);	
		}
	}

	if(attr&(1 << NATV6_ATTR_TCP_SPORT))
	{
		tmp+=(~org_source&0XFFFF);		
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
		tmp+=(htons(pinfo->sport)&0xFFFF);		
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
	}

	if(attr&(1<<NATV6_ATTR_TCP_DPORT))
	{
		tmp+=(~org_dest&0XFFFF);		
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
		tmp+=(htons(pinfo->dport)&0xFFFF);
		tmp = (tmp >> 16) + (tmp & 0xFFFF);
	}
	
	tmp = ~tmp & 0xFFFF;
	th->check = htons(tmp);

	return 0;
}

int ppeNatV6Translate(struct sk_buff* skb,unsigned int natv6_attr,int checksum)
{	
	ppeNatV6Info_t *natv6_info = &foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_info;
	int is_udp = 0,is_tcp = 0;
	struct ipv6hdr *ip6h = NULL;
	struct udphdr* uh = NULL;
	struct tcphdr* th = NULL;
		

	ip6h = (struct ipv6hdr*)ppe_get_network_header(skb);
	
	if(IPPROTO_UDP == ip6h->nexthdr)
	{
		uh = (struct udphdr*)((unsigned char*)ip6h+sizeof(struct ipv6hdr));
		is_udp = 1;

		if(checksum)
			ppeNatV6ReSumUdpCheck(ip6h,uh,natv6_info,natv6_attr);
	}
	else if(IPPROTO_TCP == ip6h->nexthdr)
	{
		th = (struct tcphdr*)((unsigned char*)ip6h+sizeof(struct ipv6hdr));
		is_tcp = 1;

		if(checksum)
			ppeNatV6ReSumTcpCheck(ip6h,th,natv6_info,natv6_attr);
	}
					
	if(natv6_attr&(1<<NATV6_ATTR_SIP))
		memmove(ip6h->saddr.s6_addr16,natv6_info->ipv6_sip,8*sizeof(unsigned short));

	if(natv6_attr&(1<<NATV6_ATTR_DIP))	
		memmove(ip6h->daddr.s6_addr16,natv6_info->ipv6_dip,8*sizeof(unsigned short));

	if(is_udp)
	{
		if(natv6_attr&(1<<NATV6_ATTR_UDP_SPORT))
			uh->source = natv6_info->sport;
		
		if(natv6_attr&(1<<NATV6_ATTR_UDP_DPORT))
			uh->dest = natv6_info->dport;
	}

	if(is_tcp)
	{
		if(natv6_attr&(1<<NATV6_ATTR_TCP_SPORT))
			th->source  = natv6_info->sport;
		
		if(natv6_attr&(1<<NATV6_ATTR_TCP_DPORT))
			th->dest = natv6_info->dport;
	}
	
	return 0;
}

int ppeNatV6RtspResponseHandler(struct sk_buff* skb)
{	
	skb->dev = ra_dev_get_by_name("eth0");

	if(skb->ecnt_sk_buff.stag_inserted)
	{
		memmove(skb->data+4, skb->data, 12);
		skb_pull(skb, 4);
	}

	skb->protocol = eth_type_trans(skb, skb->dev);
	
	PpeFreeHandler(skb);

	skb->pkt_type = PACKET_HOST;

	netif_receive_skb(skb);
		
	return 0;
}

int ppeNatV6RtspAlgHandler(struct sk_buff* skb)
{
	unsigned int foe_num = FOE_ENTRY_NUM(skb);

	if(rtsp_v6_alg_offload)
		foe_ext[foe_num].natv6.natv6_rtsp = 1;
	else
		PpeFreeHandler(skb);
	
	return 0;
}

int ecnt_offload_natv6(struct sk_buff* skb,unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
	QDMA_txMsgWord0_t w0;
	QDMA_txMsgWord1_t w1;
	struct port_info pinfo;
	int error = 0;
	int checksum = 0;
	unsigned int foe_num = FOE_ENTRY_NUM(skb);
	
	if(((NATV6_DIR_GE == p_para->natv6_dir)||(NATV6_DIR_XSI_GDM4== p_para->natv6_dir))&&(!SUPPORT_NEW_TXMSG)){
		skb->ecnt_sk_buff.stag_inserted = 1;
	}
	if((NATV6_DIR_WLAN == p_para->natv6_dir) || NATV6_DIR_USB == p_para->natv6_dir)
		checksum = 1;

	skb_push(skb,ETH_HLEN);

	if(foe_ext[foe_num].natv6.natv6_rtsp&&ppe_is_rtspv6_response(skb))
		return ppeNatV6RtspResponseHandler(skb);

	ppeNatV6Translate(skb,p_para->natv6_attr,checksum);

	if(NATV6_DIR_GE == p_para->natv6_dir)
	{				
		w0.word = 0;
		if(SUPPORT_NEW_TXMSG)
		{
			w0.raw0.ico = 1;			
			w0.raw0.uco = 1;			
			w0.raw0.tco = 1;
			w0.raw0.channel = foe_ext[foe_num].natv6.chn;
			w0.raw0.queue = foe_ext[foe_num].natv6.txq;
			w0.raw0.gem_port_id = foe_ext[foe_num].natv6.stag;
		}
		else
		{
			w0.raw1.channel = foe_ext[foe_num].natv6.chn;
			w0.raw1.queue = foe_ext[foe_num].natv6.txq;
		}

		w1.word = 0;
		if(SUPPORT_NEW_TXMSG)
		{
			w1.raw0.fport = FP_GDMA1;
			w1.raw0.mtr_g = 0x7f;
		}
		else
		{
			w1.raw1.fport = FP_GDMA1;
			w1.raw1.ico = 1;
			w1.raw1.uco = 1;
			w1.raw1.tco = 1;
		}

		memset(&pinfo,0,sizeof(struct port_info));
		error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN,skb, w0.word, w1.word, &pinfo);

		if(unlikely(error))
			kfree_skb(skb);
	}
	else if(NATV6_DIR_XSI_GDM4== p_para->natv6_dir)
	{				
		w0.word = 0;
		if(SUPPORT_NEW_TXMSG)
		{
			w0.raw0.ico = 1;			
			w0.raw0.uco = 1;			
			w0.raw0.tco = 1;
			w0.raw0.channel = foe_ext[foe_num].natv6.chn;
			w0.raw0.queue = foe_ext[foe_num].natv6.txq;
			w0.raw0.gem_port_id = foe_ext[foe_num].natv6.stag;
		}
		else
		{
			w0.raw1.channel = foe_ext[foe_num].natv6.chn;
			w0.raw1.queue = foe_ext[foe_num].natv6.txq;
		}

		w1.word = 0;
		if(SUPPORT_NEW_TXMSG)
		{
			w1.raw0.fport = FP_GDMA4;
			w1.raw0.mtr_g = 0x7f;
		}
		else
		{
			w1.raw1.fport = FP_GDMA4;
			w1.raw1.ico = 1;
			w1.raw1.uco = 1;
			w1.raw1.tco = 1;
		}

		memset(&pinfo,0,sizeof(struct port_info));
		error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN,skb, w0.word, w1.word, &pinfo);

		if(unlikely(error))
			kfree_skb(skb);
	}
	else if((NATV6_DIR_GPON == p_para->natv6_dir)||(NATV6_DIR_EPON == p_para->natv6_dir)||(NATV6_DIR_AE_WAN == p_para->natv6_dir))
	{				
		w0.word = 0;
		if(SUPPORT_NEW_TXMSG)
		{
			w0.raw0.ico = 1;			
			w0.raw0.uco = 1;			
			w0.raw0.tco = 1;
			w0.raw0.channel = foe_ext[foe_num].natv6.chn;
			w0.raw0.queue = foe_ext[foe_num].natv6.txq;
			w0.raw0.gem_port_id = foe_ext[foe_num].natv6.stag;
		}
		else
		{
			w0.raw1.channel = foe_ext[foe_num].natv6.chn;
			w0.raw1.sp_tag = foe_ext[foe_num].natv6.stag;		
			w0.raw1.queue = foe_ext[foe_num].natv6.txq;
		}


		w1.word = 0;
		if(SUPPORT_NEW_TXMSG)
		{
			w1.raw0.fport = FP_GDMA2;
			w1.raw0.mtr_g = 0x7f;
		}
		else
		{
			w1.raw1.fport = FP_GDMA2;
			w1.raw1.ico = 1;
			w1.raw1.uco = 1;
			w1.raw1.tco = 1;	
		}

		memset(&pinfo,0,sizeof(struct port_info));
		error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_WAN,skb, w0.word,w1.word , &pinfo);

		if(unlikely(error))
			kfree_skb(skb);
	}
	else if(NATV6_DIR_WLAN == p_para->natv6_dir)
	{		
		skb_pull(skb,ETH_HLEN);
		ecnt_wlan_force_to_cpu_handler(skb,dp,p_para,foe_entry);
	}
	else if(NATV6_DIR_USB == p_para->natv6_dir)
	{		
		skb_pull(skb,ETH_HLEN);
		ecnt_offload_default_offload(skb,dp,p_para,foe_entry);
	}
	
	return 0;
}
/*****************natv6 offload end************************/
static void ppePPPOEHeaderLengthTranslate(struct sk_buff* skb)
{	
    int vlan_tag_num = 0;
    struct iphdr*    ih = NULL;
    struct pppoe_hdr *ph = NULL;
    unsigned short length   = 0;
    unsigned short eth_type = 0;
    unsigned short pppoe_proto = 0;
    unsigned char* buff = skb->data;

    if(NULL == buff)
    {
        return ;
    }

    vlan_tag_num = ppe_get_vlan_tag_num(skb);
    buff += (12 + 4 * vlan_tag_num);
    eth_type = *(uint16*)buff;
    /*skip ether type*/
    buff += 2;
    if (eth_type == htons(ETH_P_PPP_SES))
    {
        /*skip ppp header*/
        ph = (struct pppoe_hdr *)buff;

        /*skip ppp header*/
        buff += sizeof(struct pppoe_hdr);
        pppoe_proto = *(uint16*)buff;
        if (pppoe_proto == htons(PPP_IP))
        {
            buff += 2;
            ih = (struct iphdr*)buff;
            length = (ntohs(ih->tot_len)+ 2);
            ph->length = htons(length);
        } 
        else
        {
            
        }
    }

    return ;
}

int ecnt_offload_hit_ifc(struct sk_buff* skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
    QDMA_txMsgWord0_t w0;
    QDMA_txMsgWord1_t w1;
    struct port_info pinfo;
    int error = 0;
    unsigned int foe_num = FOE_ENTRY_NUM(skb);

    if((IFC_HIT_DIR_GE == foe_ext[foe_num].ifc_hit_info.dir) && (!SUPPORT_NEW_TXMSG))
    {
        skb->ecnt_sk_buff.stag_inserted = 1;
    }

    skb_push(skb, ETH_HLEN);
    if(PPPOE_HEADER_LENGTH_TRANSLATE == foe_ext[foe_num].ifc_hit_info.match_action)
    {
        /*check pppoe header and ip header*/
        ppePPPOEHeaderLengthTranslate(skb);
    }
    
    /*Now only support IFC_HIT_DIR_GE*/
    if(IFC_HIT_DIR_GE == foe_ext[foe_num].ifc_hit_info.dir)
    {				
        w0.word = 0;
        if(SUPPORT_NEW_TXMSG)
        {
            w0.raw0.ico = 1;			
            w0.raw0.uco = 1;			
            w0.raw0.tco = 1;
            w0.raw0.channel     = foe_ext[foe_num].ifc_hit_info.chn;
            w0.raw0.queue       = foe_ext[foe_num].ifc_hit_info.txq;
            w0.raw0.gem_port_id = foe_ext[foe_num].ifc_hit_info.stag;
        }
        else
        {
            w0.raw1.channel = foe_ext[foe_num].ifc_hit_info.chn;
            w0.raw1.queue   = foe_ext[foe_num].ifc_hit_info.txq;
        }
        w1.word = 0;
        if(SUPPORT_NEW_TXMSG)
        {
            w1.raw0.fport = FP_GDMA1;
            w1.raw0.mtr_g = 0x7f;
            hwnat_offload_mark_lan_rx(foe_num, foe_ext[foe_num].ifc_hit_info.stag);
        }
        else
        {
            w1.raw1.fport = FP_GDMA1;
            w1.raw1.ico = 1;
            w1.raw1.uco = 1;
            w1.raw1.tco = 1;
        }
        memset(&pinfo,0,sizeof(struct port_info));
        error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN, skb, w0.word, w1.word, &pinfo);
        if(unlikely(error))
        {
            kfree_skb(skb);
        }
    }
    else if((IFC_HIT_DIR_GPON == foe_ext[foe_num].ifc_hit_info.dir)
        ||(IFC_HIT_DIR_EPON == foe_ext[foe_num].ifc_hit_info.dir))
    {				
        w0.word = 0;
        if(SUPPORT_NEW_TXMSG)
        {
            w0.raw0.ico = 1;			
            w0.raw0.uco = 1;			
            w0.raw0.tco = 1;
            w0.raw0.channel     = foe_ext[foe_num].ifc_hit_info.chn;
            w0.raw0.queue       = foe_ext[foe_num].ifc_hit_info.txq;
            w0.raw0.gem_port_id = foe_ext[foe_num].ifc_hit_info.stag;
        }
        else
        {
            w0.raw1.channel = foe_ext[foe_num].ifc_hit_info.chn;
            w0.raw1.sp_tag  = foe_ext[foe_num].ifc_hit_info.stag;		
            w0.raw1.queue   = foe_ext[foe_num].ifc_hit_info.txq;
        }
        w1.word = 0;
        if(SUPPORT_NEW_TXMSG)
        {
            w1.raw0.fport = FP_GDMA2;
            w1.raw0.mtr_g = 0x7f;
        }
        else
        {
            w1.raw1.fport = FP_GDMA2;
            w1.raw1.ico = 1;
            w1.raw1.uco = 1;
            w1.raw1.tco = 1;	
        }
        memset(&pinfo,0,sizeof(struct port_info));
        error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_WAN,skb, w0.word,w1.word , &pinfo);
        if(unlikely(error))
        {
            kfree_skb(skb);
        }
    }
    else if(IFC_HIT_DIR_WLAN == foe_ext[foe_num].ifc_hit_info.dir)
    {		
        skb_pull(skb,ETH_HLEN);
        ecnt_wlan_force_to_cpu_handler(skb,dp,p_para,foe_entry);
    }

	return 0;
}
int ecnt_offload_ookla(struct sk_buff* skb, unsigned int dp, struct force_to_cpu_para* p_para, struct FoeEntry *foe_entry)
{
	if(ookla_offload_rcv_hook){
		return ookla_offload_rcv_hook(skb);
	}

	return 1;
}
struct sk_buff *ppe_vlan_put_tag(struct sk_buff *skb, u16 vlan_tci)
{
	struct vlan_ethhdr *veth;
    if (skb_headroom(skb) < VLAN_HLEN) {
        struct sk_buff *sk_tmp = skb;
        skb = skb_realloc_headroom(sk_tmp, VLAN_HLEN);
        kfree_skb(sk_tmp);
        if (!skb) {
            printk(KERN_ERR "vlan: failed to realloc headroom\n");
            return NULL;
        }
    } else if((FOE_MAGIC_TAG(skb) != FOE_MAGIC_WLAN) && (FOE_MAGIC_TAG(skb) != FOE_MAGIC_USBNET)){
        skb = skb_unshare(skb, GFP_ATOMIC);
        if (!skb) {
            return NULL;
        }
    }
	
	veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	memmove(skb->data, skb->data + VLAN_HLEN, 2 * VLAN_ETH_ALEN);
	skb->mac_header -= VLAN_HLEN;

	/* first, the ethernet type */
	veth->h_vlan_proto = htons(ETH_P_8021Q);

	/* now, the TCI */
	veth->h_vlan_TCI = htons(vlan_tci);

	skb->protocol = htons(ETH_P_8021Q);

	return skb;
}

int is_tr471_pkt(struct sk_buff *skb){
	unsigned char * udppayload;
	uint8 ip_proto=0;
	
	ip_proto = PpeGetIpVer(skb);
	if( IP_VER_V4 == ip_proto && skb->len >=30 ){
		udppayload = (unsigned char *)(skb->data + 28);
		if (udppayload[0] == 0xBE && udppayload[1] == 0XEF){
				return 1;	
		} 
	}
	else if(IP_VER_V6 == ip_proto && skb->len >=50){
		udppayload = (unsigned char *)(skb->data + 48);
		if (udppayload[0] == 0xBE && udppayload[1] == 0XEF){
				return 1;	
		} 
	}

	return 0;

}
int ppe_remove_vlan_tag(struct sk_buff *skb)
{
	struct ethhdr *eth;
	struct vlan_ethhdr *veth;
	uint16_t VirIfIdx;

	veth = (struct vlan_ethhdr *)LAYER2_HEADER(skb);

	//something wrong
	if (veth->h_vlan_proto != htons(ETH_P_8021Q)) {
		/*tr471 format*/
		if(is_tr471_pkt(skb)){
			VirIfIdx = DP_TR471_UP;
			return VirIfIdx;
		}
		
		printk("HNAT: Reentry packet is untagged frame?\n");
		return 65535;
	}

	VirIfIdx = ntohs(veth->h_vlan_TCI);

	if (skb_cloned(skb) || skb_shared(skb)) {

		struct sk_buff *new_skb;
		new_skb = skb_copy(skb, GFP_ATOMIC);
		kfree_skb(skb);
		if (!new_skb)
			return 65535;
		skb = new_skb;
	}

	/* remove VLAN tag */
	skb->data = LAYER2_HEADER(skb);
	skb->mac_header +=VLAN_HLEN;

	memmove(LAYER2_HEADER(skb), skb->data, ETH_ALEN * 2);
	
	skb_pull(skb, VLAN_HLEN);
	skb->data += ETH_HLEN;	//pointer to layer3 header
	eth = (struct ethhdr *)LAYER2_HEADER(skb);
	skb->protocol = eth->h_proto;

	return VirIfIdx;

}


inline uint32_t PpeExtIfRxFastHandler(struct sk_buff * skb)
{
	uint16_t VirIfIdx = 0;
	int i,ratelimit_index;
	int paddingLength = 0;
	struct sk_buff * skb2;
	struct ethhdr *eth_header = NULL;

	if(NULL == skb)
		return 1;

	if(left_to_right_test_mode)
		return 1;

	eth_header = (struct ethhdr*)LAYER2_HEADER(skb);
	
	if(eth_header&&is_multicast_ether_addr(&eth_header->h_dest[0]))
		return 1;	

	if ((skb->protocol != htons(ETH_P_IP)) && (skb->protocol != htons(ETH_P_8021Q)) &&
	    (skb->protocol != htons(ETH_P_IPV6)) && (skb->protocol != htons(ETH_P_PPP_SES)) &&
	    (skb->protocol != htons(ETH_P_PPP_DISC))) {
		return 1;
	}

	if(rx_wifi_len!=0){
		if(skb->len == (rx_wifi_len-18)) 
			rx_wifi_len_counter++;
	}
		
	for(i=0;i<WLAN_IF_NUM;i++){
		if (skb->dev == DstPort[ DP_RA0 + i]) {
			VirIfIdx = DP_RA0 + i;
			goto DEVFIND;
		}	
	}
	
	for(i=0;i<WLAN_IF_I_NUM;i++){
		if (skb->dev == DstPort[ DP_RAI0 + i]) {
			VirIfIdx = DP_RAI0 + i;
			goto DEVFIND;
		}	
	}

	for(i=0;i<WLAN_IF_I_NUM;i++){
		if (skb->dev == DstPort[ DP_RAX0 + i]) {
			VirIfIdx = DP_RAX0 + i;
			goto DEVFIND;
		}	
	}

        if (skb->dev == DstPort[DP_APCLI0]) {
            VirIfIdx = DP_APCLI0;
            goto DEVFIND;
        }

        if (skb->dev == DstPort[DP_APCLII0]) {
            VirIfIdx = DP_APCLII0;
            goto DEVFIND;
        }
	if (skb->dev == DstPort[DP_APCLIX0]) {
		VirIfIdx = DP_APCLIX0;
		goto DEVFIND;
	}

	if(VirIfIdx == 0)
		return 1;//dont send to learning
	
	DEVFIND:
	skb_push(skb, ETH_HLEN);	

	if(skb->len < 60)
	{
		paddingLength = 60 - skb->len;
	
		if(skb_tailroom(skb) < paddingLength){
			skb2 = skb_copy_expand(skb, skb_headroom(skb), paddingLength, GFP_ATOMIC);
			if (skb2) {
				kfree_skb(skb);
				skb = skb2;
				skb_put(skb, paddingLength);
			}
			else {
				if (DebugLevel >= 1)
					printk("\nskb_copy_expand failed at %s, %d\n", __func__, __LINE__);
				return 1; /* go back to netif_rx */
			}
		}
		else {
			skb_put(skb, paddingLength);
		}
	}

	skb = ppe_vlan_put_tag(skb, VirIfIdx);

	if(skb == NULL)
		return 0;

	if(soft_ratelimit_enqueue_hook &&(isWiFiInterface(skb->dev))&&(wifiqueue_start_idx!=-1))
	{
		ratelimit_index = getWifiQueueIndex(skb->dev,WIFI_RATELIMIT_RX);

		if(ratelimit_index!=-1)
			if(1 == soft_ratelimit_enqueue_hook(skb,ratelimit_index))/*wifi rx*/
				return 0;
	}

	if(wifi_eth_fast_tx_hook)
		wifi_eth_fast_tx_hook(skb);
	else
		return 1;
	
	return 0;
}

/* push different VID for WiFi pseudo interface or USB external NIC */
inline uint32_t PpeExtIfRxHandler(struct sk_buff * skb)
{
	int paddingLength = 0;
	struct sk_buff * skb2;
	struct ethhdr *eth_header = NULL;
	uint16_t VirIfIdx = 0;
	int i;
	
	if(NULL == skb)
		return 1;

    if ((FOE_MAGIC_TAG(skb) != FOE_MAGIC_CRYPTO_E_2) && (FOE_MAGIC_TAG(skb) != FOE_MAGIC_CRYPTO_D_2))
    {
    	eth_header = (struct ethhdr*)LAYER2_HEADER(skb);

    	if(eth_header&&is_multicast_ether_addr(&eth_header->h_dest[0]))
    		return 1;	
    }
	
	/* PPE only can handle IPv4/VLAN/IPv6/PPP packets */
	if ((skb->protocol != htons(ETH_P_IP)) && (skb->protocol != htons(ETH_P_8021Q)) &&
	    (skb->protocol != htons(ETH_P_IPV6)) && (skb->protocol != htons(ETH_P_PPP_SES)) &&
	    (skb->protocol != htons(ETH_P_PPP_DISC))) {
		return 1;
	}

	if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_E_2)
	{
		VirIfIdx = DP_CRYPTO_E_0 + skb->cb[IPSEC_SKB_CB];
	}
	else if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_D_2)
	{
		VirIfIdx = DP_CRYPTO_D_0 + skb->cb[IPSEC_SKB_CB];
	}
	else if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_USBNET) //add for USB dongle
	{
#ifdef CONFIG_TP_IMAGE
		for (i = 0; i < 1; i++)
#else
		for(i=0;i<HWNAT_USB_IF_NUM;i++)
#endif /* CONFIG_TP_IMAGE */
		{
			if (skb->dev == DstPort[DP_USBNET0 + i])
				VirIfIdx = DP_USBNET0 + i;
		}
	
		if(skb->dev == DstPort[DP_USBNET2])
			VirIfIdx = DP_USBNET2;
	}
	else
	{
		for(i=0;i<WLAN_IF_NUM;i++){
			if (skb->dev == DstPort[ DP_RA0 + i]) {
				VirIfIdx = DP_RA0 + i;
				goto DEVFIND;
			}	
		}
		for(i=0;i<WLAN_IF_I_NUM;i++){
			if (skb->dev == DstPort[ DP_RAI0 + i]) {
				VirIfIdx = DP_RAI0 + i;
				goto DEVFIND;
			}	
		}
		for(i=0;i<WLAN_IF_I_NUM;i++){
			if (skb->dev == DstPort[ DP_RAX0 + i]) {
				VirIfIdx = DP_RAX0 + i;
				goto DEVFIND;
			}
		}
		for(i=0;i<WLAN_IF_NUM;i++){
			if (skb->dev == DstPort[ DP_WDS0 + i]) {
				VirIfIdx = DP_WDS0 + i;
				goto DEVFIND;
				}	
			}
		for(i=0;i<WLAN_IF_NUM;i++){
			if (skb->dev == DstPort[ DP_WDSI0 + i]) {
				VirIfIdx = DP_WDSI0 + i;
				goto DEVFIND;
			}	
	    }
                for(i=0;i<WLAN_IF_NUM;i++)
                {
                    if (skb->dev == DstPort[ DP_APCLI0 + i]) 
                    {
                        VirIfIdx = DP_APCLI0+ i;
                        goto DEVFIND;
                    }	

 		}

		for(i=0;i<WLAN_IF_NUM;i++){

			if (skb->dev == DstPort[ DP_APCLII0 + i]) {

				VirIfIdx = DP_APCLII0 + i;

				goto DEVFIND;

			}	

	    }

		for(i=0;i<WLAN_IF_NUM;i++){
			if (skb->dev == DstPort[ DP_APCLIX0 + i]) {
				VirIfIdx = DP_APCLIX0 + i;
				goto DEVFIND;
			}
	    }
	}

	if(VirIfIdx == 0){
		printk("HNAT[%s]: The interface %s is unknown\n",__FUNCTION__, skb->dev->name);
		return 1;//dont send to learning
	}
	
DEVFIND:	
	//push vlan tag to stand for actual incoming interface,
	//so HNAT module can know the actual incoming interface from vlan id.
	skb_set_network_header(skb, 0);

	skb_push(skb, ETH_HLEN);	//pointer to layer2 header before calling hard_start_xmit
	
	/* Workaround for WIFI->GPON chariot test failure in 3~4 mins.
	   Root cause: EN751221 deault uses L2B table to speed up WIFI->GPON traffic,
	   therefore, all packets will be speeded up, including 54-byte packets. Unfortunately,
	   PPE and GDM2 can't pad for those 54-byte packets, so they will be dropped at OLT or WAN PC.
	   Note: MT751020 won't have this problem because it default uses IPv4 NAPT table to speed up
	   WIFI->GPON packets, so 54-byte packets won't be speeded up because those packets use
	   different TCP port with the mainly speeded-up WIFI->GPON packets, and CPU will pad for
	   those 54-byte packets. */

	/* hwnat_v3 support the following chips, no need for this logic 
	if (((isEN751221) || (isEN7580) || (isEN751627) || (isEN7523)) && (skb->len < 60))
	*/
    if (skb->len < 60)
    {
        paddingLength = 60 - skb->len;
    
        if(skb_tailroom(skb) < paddingLength){
            skb2 = skb_copy_expand(skb, skb_headroom(skb), paddingLength, GFP_ATOMIC);
            if (skb2) {
                kfree_skb(skb);
                skb = skb2;
                skb_put(skb, paddingLength);
            }
            else {
				if (DebugLevel >= 1)
                	printk("\nskb_copy_expand failed at %s, %d\n", __func__, __LINE__);
				return 1; /* go back to netif_rx */
			}
        }
        else {
            skb_put(skb, paddingLength);
        }
    }
	
	if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_E_2 || FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_D_2)
	{
		//not let hw_nat l2b learn
		memcpy(skb->data,GetMacAddr(),ETH_ALEN);
		//memcpy(skb->data+ETH_ALEN,srcMacAddr,ETH_ALEN);
		*(unsigned short *)(skb->data+(ETH_ALEN<<1)) = htons(0x0800); 
	}

/*because use __vlan_put_tag() in new kernel will lead to headrom is not enough and reduce performance,so we rewrite __vlan_put_tag_hwnat() instead of it*/	
	skb = ppe_vlan_put_tag(skb, VirIfIdx);

	/* 0 -->FWD & without netif_rx ,*/
	if(skb == NULL)
		return 0;

	if(offload_eth_fast_tx_hook)
		offload_eth_fast_tx_hook(skb,7);
	else
		return 1;

	return 0;
}

static uint16_t ppeTransNpuUdfToVirIfIdx(unsigned char npu_udf)
{
    if((npu_udf>=NPU_UDF_VXLAN_0)&&(npu_udf<=NPU_UDF_VXLAN_19))
        return DP_VXLAN_UP_2_0+npu_udf-NPU_UDF_VXLAN_0;

    if((npu_udf>=NPU_UDF_VXLAN_DOWN_0)&&(npu_udf<=NPU_UDF_VXLAN_DOWN_19))
        return DP_VXLAN_0+npu_udf-NPU_UDF_VXLAN_DOWN_0;

    if((npu_udf>=NPU_UDF_MAP_T_TUNNEL_UP)&&(npu_udf<=NPU_UDF_MAP_T_OFFLOAD_DOWN))
        return DP_MAP_T;

    return 0xffff;
}

static int leftToRightTestModePingPong(struct sk_buff * skb)
{
	struct ethhdr *eth = NULL;
	struct net_bridge_fdb_entry *dst;
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
	struct net_device *dev = NULL;

	if(!isEN7580)
		return 1;

	if(left_to_right_test_mode == TEST_MODE_VXLAN)
	{
		if(FOE_SP(skb) == SP_QDMA)
		{
			PpeRestoreVxlanTxInfo(skb);
			skb_reset_network_header(skb);
			skb->dev = DstPort[DP_VXLAN_0];
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,21)
			skb_dst(skb)->output(NULL, skb->sk, skb);
#else
			skb_dst(skb)->output(skb->sk,skb);	
#endif
			return 0 ; 
		}
		else if (FOE_SP(skb) == SP_PDMA)
		{
			skb->dev = DstPort[DP_VXLAN_0];

			eth = (struct ethhdr *)LAYER2_HEADER(skb);
			if (eth->h_dest[0] & 1) 
			{
				if (memcmp(eth->h_dest, skb->dev->broadcast, ETH_ALEN) == 0) 
				{
					skb->pkt_type = PACKET_BROADCAST;
				}
				else
				{
					skb->pkt_type = PACKET_MULTICAST;
				}
			} 
			else
			{
				skb->pkt_type = PACKET_OTHERHOST;
				if (memcmp(eth->h_dest, skb->dev->dev_addr, ETH_ALEN) == 0) 
				{
					skb->pkt_type = PACKET_HOST;
				}
			}

			netif_rx(skb);
			return 0;
		}
	}
	else if(left_to_right_test_mode == TEST_MODE_NATV6)
	{
		if (FOE_SP(skb) == SP_PDMA)
		{
			const unsigned char *dest = skb->data - ETH_HLEN;
			ndev = dev_get_by_name(&init_net, "br0");
			if (ndev)
				br = netdev_priv(ndev);
			else
				return 1;

			if ((dst = __br_fdb_get(br, dest, 0)) != NULL)
			{
				if(IS_FDB_LOCAL(dst)){
					dev_put(ndev);
					return 1;
				}
				
				dev = dst->dst->dev;
				if(dev)
				{
					skb->dev = dst->dst->dev;
					skb_push(skb, ETH_HLEN);
					skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
					dev_put(ndev);
	
					return 0;
				}
			}
		
			dev_put(ndev);
		}
	}
	
	return 1;
}


uint32_t PpeExtIfPingPongHandler(struct sk_buff * skb)
{
	struct ethhdr *eth = NULL;
	uint16_t VirIfIdx = 0;
	struct net_device *dev;

	if(0 == leftToRightTestModePingPong(skb))
		return 0;

    if(foe_ext[FOE_ENTRY_NUM(skb)].npu_pingpong)
        VirIfIdx = ppeTransNpuUdfToVirIfIdx(foe_ext[FOE_ENTRY_NUM(skb)].npu_pingpong_data.udf);
    else
	    VirIfIdx = ppe_remove_vlan_tag(skb);

	
	if(ECNT_RETURN == CRYPTO_API_ESP_PKTPUT_FINISH_PT(ECNT_CRYPTO, skb, VirIfIdx)){
		return 0;
	}
	
	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL){
		if ((FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU)) {
			setPPeRxACNTGRP(skb);
		}
	}

	if((DP_ASYM_D1 <= VirIfIdx)&&(VirIfIdx <= DP_ASYM_D5))
	{
		if(HIT_BIND_KEEPALIVE_DUP_OLD_HDR == FOE_AI(skb))
			ECNT_ASYM_API_KA_HANDLER(skb,ECNT_ASYM_KA_POINT_LAN_PINGPONG);
		skb->dev = DstPort[DP_GMAC1+VirIfIdx-DP_ASYM_D1];
		skb->ecnt_asym_mark |= ECNT_ASYM_MARK_HANDLER_DONE;		
		skb->ecnt_asym_mark |= ECNT_ASYM_MARK_KA;
		return 1;
	}
	else if((DP_ASYM_U1 <= VirIfIdx)&&(VirIfIdx <= DP_ASYM_U5))
	{
		skb->dev = DstPort[DP_GMAC1+VirIfIdx-DP_ASYM_U1];		
		skb->ecnt_asym_mark |= ECNT_ASYM_MARK_HANDLER_DONE;
		return 1;
	}

	if (DP_SPEED_UP == VirIfIdx){
		if(wan_speed_test_pinpong_handle_hook){
			wan_speed_test_pinpong_handle_hook(skb);
		}else{
			printk("HNAT: DP_SPEED_UP, wan_speed_test_pinpong_handle_hook is null\n");
			dev_kfree_skb_any(skb);
		}
		return 0;
	}
	else if (DP_TR471_UP == VirIfIdx ){
		if(wan_tr471_pinpong_handle_hook){
			wan_tr471_pinpong_handle_hook(skb);
		}else{
			printk("HNAT: DP_TR471_UP, wan_tr471_pinpong_handle_hook is null\n");
			dev_kfree_skb_any(skb);
		}
		return 0;
	}
	else if (DP_L2TP_DOWN_2 == VirIfIdx)
	{
		if( l2tp_vpn_dev )
			skb->dev = l2tp_vpn_dev ;
		else{
			skb->dev = dev_get_by_name(&init_net,L2TP_VPN_PPP_NAME);
			if(skb->dev)
				dev_put(skb->dev);
		}
		if ((NULL == skb->dev) || (0 == (skb->dev->flags & IFF_UP)))
		{
			printk("HNAT: error skb->dev is null or skb->dev is not up\n");
			dev_kfree_skb_any(skb);
			return 0;
		}
		
		return 1;
	}
	else if( DP_L2TP_UP_2 == VirIfIdx )
	{
		PpeRestoreTxInfo(skb);
		skb_reset_network_header(skb);
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,21)
        skb_dst(skb)->output(dev_net(skb_dst(skb)->dev), skb->sk, skb);
#else
        skb_dst(skb)->output(skb->sk,skb);  
#endif  
		/*send to dst directly, no need be received by netif_rx*/
		/*so return 0*/
		return 0 ; 
	}
	else if(DP_GRE_UP_2 == VirIfIdx)
	{
	    PpeRestoreGreTxInfo(skb);
	    skb_reset_network_header(skb);
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,21)
        skb_dst(skb)->output(dev_net(skb_dst(skb)->dev), skb->sk, skb);
#else
        skb_dst(skb)->output(skb->sk,skb);  
#endif  
	    return 0 ; 
	}
	else if (((VirIfIdx >= DP_GRE_TUNNEL_0) && (VirIfIdx <= DP_GRE_TUNNEL_7))
		||((VirIfIdx >= DP_GRE_L2_PRI_TUNNEL_0) && (VirIfIdx <= DP_GRE_L2_SEC_TUNNEL_7)))
	{
		skb->dev = DstPort[VirIfIdx];
		netif_rx(skb);
		return 0;
	}
    else if ((VirIfIdx >= DP_VXLAN_UP_2_0) && (VirIfIdx <= DP_VXLAN_UP_2_19) 
        && (DstPort[VirIfIdx - DP_VXLAN_UP_2_0 + DP_VXLAN_0] != NULL))
    {
		// mark for VXLAN UP 2 stream
		if(left_to_right_test_mode == TEST_MODE_VXLAN)
			foe_ext[FOE_ENTRY_NUM(skb)].vxlan_up_2_flag = 1;
        PpeRestoreVxlanTxInfo(skb);
        skb_reset_network_header(skb);
        skb->dev = DstPort[VirIfIdx - DP_VXLAN_UP_2_0 + DP_VXLAN_0];
        if(skb_dst(skb) == NULL)
        {
            dev_kfree_skb_any(skb);
            return 0;
        }
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,21)
        skb_dst(skb)->output(dev_net(skb_dst(skb)->dev), skb->sk, skb);
#elif LINUX_VERSION_CODE > KERNEL_VERSION(2,6,36)
        skb_dst(skb)->output(skb->sk,skb);  
#else
        skb_dst(skb)->output(skb);
#endif
        return 0 ; 
    }
    else if ((VirIfIdx >= DP_VXLAN_0) && (VirIfIdx <= DP_VXLAN_19) && (DstPort[VirIfIdx] != NULL))
    {
        skb->dev = DstPort[VirIfIdx];
        eth = (struct ethhdr *)LAYER2_HEADER(skb);
        if (eth->h_dest[0] & 1) 
        {
            if (memcmp(eth->h_dest, skb->dev->broadcast, ETH_ALEN) == 0) 
            {
                skb->pkt_type = PACKET_BROADCAST;
            }
            else
            {
                skb->pkt_type = PACKET_MULTICAST;
            }
        } 
        else
        {
            skb->pkt_type = PACKET_OTHERHOST;
            if (memcmp(eth->h_dest, skb->dev->dev_addr, ETH_ALEN) == 0) 
            {
                skb->pkt_type = PACKET_HOST;
            }
        }

        netif_rx(skb);
        return 0;
    }
	else if(((VirIfIdx >= DP_USBNET0) && (VirIfIdx < DP_USBNET0 + HWNAT_USB_IF_NUM))||(VirIfIdx == DP_USBNET2))//add for usb dongle
	{
		skb->dev = DstPort[VirIfIdx];
		
		eth = (struct ethhdr *)LAYER2_HEADER(skb);
	
		if(skb->dev == NULL)
		{
			printk("skb->dev of DstPort[%d] == NULL \n",VirIfIdx);
			return 0;
		}
		if(eth == NULL)
		{
			printk("eth of DstPort[%d] == NULL \n",VirIfIdx);
			return 0;
		}
		 if (eth->h_dest[0] & 1) 
        {
            if (memcmp(eth->h_dest, skb->dev->broadcast, ETH_ALEN) == 0) 
            {
                skb->pkt_type = PACKET_BROADCAST;
            }
            else
            {
                skb->pkt_type = PACKET_MULTICAST;
            }
        } 
        else
        {
            skb->pkt_type = PACKET_OTHERHOST;
            if (memcmp(eth->h_dest, skb->dev->dev_addr, ETH_ALEN) == 0) 
            {
                skb->pkt_type = PACKET_HOST;
            }
        }
				
		netif_rx(skb);
		
		return 0;
	}
    else if(DP_TSO_LOCALOUT == VirIfIdx)
    {
        if (DebugLevel >= 2) {
            printk("[%s][%d]DP_TSO_LOCALOUT pkt.\n", __FUNCTION__, __LINE__);
        }
        PpeTsoPingPongHandle(skb);
        return 0;
    }
    else if ((VirIfIdx >= DP_APP_SHORTCUT_RA0) && (VirIfIdx <= DP_APP_SHORTCUT_RA7))
    {
        skb->dev = DstPort[VirIfIdx - DP_APP_SHORTCUT_RA0 + DP_RA0];
        skb_push(skb, ETH_HLEN);
        skb->dev->netdev_ops->ndo_start_xmit(skb, skb->dev);
        return 0;
    }
    else if ((VirIfIdx >= DP_APP_SHORTCUT_RAI0) && (VirIfIdx <= DP_APP_SHORTCUT_RAI7))
    {
        skb->dev = DstPort[VirIfIdx - DP_APP_SHORTCUT_RAI0 + DP_RAI0];
        skb_push(skb, ETH_HLEN);
        skb->dev->netdev_ops->ndo_start_xmit(skb, skb->dev);
        return 0;
    }
    else if ((VirIfIdx >= DP_APP_SHORTCUT_GMAC1) && (VirIfIdx <= DP_APP_SHORTCUT_GMAC8))
    {
        skb->dev = DstPort[VirIfIdx - DP_APP_SHORTCUT_GMAC1 + DP_GMAC1];
        skb_push(skb, ETH_HLEN);
        skb->dev->netdev_ops->ndo_start_xmit(skb, skb->dev);
        return 0;
    }
    else if(DP_FORWARD_LEFT_TO_RIGHT == VirIfIdx){
        skb->dev = dev_get_by_name(&init_net,PON_DEV_NAME);
        if (NULL == skb->dev)
        {
            printk("HNAT: error skb->dev is null, VirIfIdx %d\n",VirIfIdx);
            dev_kfree_skb_any(skb);
            return 0;
        }
        // upstream cloudvr for queue-5, upstream ack for queue-6
        if(TCSUPPORT_CT_JOYME4_VAL){
            if(NATv6_USE_HIGH_PRIORITY){
                skb->mark = (skb->mark & (~QOS_FILTER_MARK)) | 0x30;
            }
        }

        skb_push(skb, ETH_HLEN);
        skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
        return 0;
    }
	else if(DP_MAP_E_US== VirIfIdx){
		foe_ext[FOE_ENTRY_NUM(skb)].map_pingpong = 1;  
		skb->pkt_type = PACKET_HOST;
        skb->dev = DstPort[DP_MAP_E_US];
        skb_reset_network_header(skb);        
        skb->dev->netdev_ops->ndo_start_xmit(skb, skb->dev); //go to ip6_tnl_start_xmit if dev is map-e
        return 0;
    }
	else if(DP_MAP_E_DS== VirIfIdx){
		foe_ext[FOE_ENTRY_NUM(skb)].map_pingpong = 1;  
		skb->pkt_type = PACKET_HOST;
        return 1;
    }	
	else if(DP_MAP_T == VirIfIdx){
		skb->dev = DstPort[DP_GMAC1];
		if(unlikely(skb->dev == NULL)){
			printk("HNAT: dev error\n");
			dev_kfree_skb_any(skb);
			return 0;
		}
		skb->pkt_type = PACKET_HOST;
		return 1;
	}
	else if (VirIfIdx == DP_OOKLA_TX)
	{
		if(ookla_offload_pingpong_hook){
			return ookla_offload_pingpong_hook(skb);
		}
		
		return 1;
	}
	 
	//recover to right incoming interface
	if (VirIfIdx < MAX_IF_NUM) {
		skb->dev = DstPort[VirIfIdx];
	} else {
		printk("HNAT: unknow interface (VirIfIdx=%d)\n",
				VirIfIdx);
	}
	if(skb->dev == NULL){
		printk("HNAT: error interface (VirIfIdx=%d)\n",
				VirIfIdx);
		dev_kfree_skb_any(skb);
		return 0;
		//return 1;
	}
	
	if (TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL || TCSUPPORT_CRJO_VAL) {
		if (isWiFiInterface(skb->dev))
			foe_ext[FOE_ENTRY_NUM(skb)].rx_dev = 1;
	}
	
	if (isWiFiInterface(skb->dev))
	{
		PpeBlack_ip_foe_handle(skb,0);
	}
	else
	{
		foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan = 0;
	}
	
	if( ((VirIfIdx >= DP_RA0)&&(DP_RAI7 >= VirIfIdx)) || 
		((VirIfIdx >= DP_RAX0)||(VirIfIdx <= DP_RAX7)) )
		return 1;

	eth = (struct ethhdr *)LAYER2_HEADER(skb);

	if (eth->h_dest[0] & 1) {
		if (memcmp(eth->h_dest, skb->dev->broadcast, ETH_ALEN) == 0) {
			skb->pkt_type = PACKET_BROADCAST;
		} else {
			skb->pkt_type = PACKET_MULTICAST;
		}
	} else {

		skb->pkt_type = PACKET_OTHERHOST;
		for(VirIfIdx=0; VirIfIdx < MAX_IF_NUM; VirIfIdx++) {
			dev = DstPort[VirIfIdx];
			if (dev !=NULL && memcmp(eth->h_dest, dev->dev_addr, ETH_ALEN) == 0) {
				skb->pkt_type = PACKET_HOST;
				break;
			}
		}
	}

	return 1;

}

static int hwnat_rpsenable_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{

	printk("rps_enable: %d, dualband interupt %d. dualband_interrupt_enable_V2: %d \n", 
        rps_enable, dualband_interrupt_enable, dualband_interrupt_enable_V2);

	return 0;	
}
static int hwnat_rpsenable_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	int len = 0;
	unsigned int flag = 0;
	
	if (count > 8)
		len = 8;
	else 
		len = count;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;

	get_buf[len] = '\0';

	
	sscanf(get_buf, "%d %d %d",&flag, &dualband_interrupt_enable,&dualband_interrupt_enable_V2);

	rps_enable = flag;
	
	printk("rps_enable: %d, dualband interupt: %d, dualband_interrupt_enable_V2: %d.\n",	\
			rps_enable, dualband_interrupt_enable,dualband_interrupt_enable_V2);
	
	return len;
}

__IMEM int32_t wifi_tx_hook(struct sk_buff * skb, unsigned int rx_len)
{
	struct FoeEntry *foe_entry;
	unsigned char act_dp;
	
	if(FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU && ((*(skb->data) & 0x1) == 0x0)){
		foe_entry = (struct FoeEntry *)K1_TO_K0((struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + FOE_ENTRY_NUM(skb)*SIZE_OF_FOE_ENTRY));
		if (IS_IPV4_HNAPT(foe_entry)){
			act_dp = (unsigned char)foe_entry->ipv4_hnapt.act_dp;
			if(DP_RAI0 == act_dp){
				skb_put(skb, rx_len);
				skb->dev = DstPort[DP_RAI0];
				skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
				return 1;
			}
		}
	}
	return 0;
}

static int rx_wifi_len_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("rx_wifi_len :%d\n",rx_wifi_len);
	
	printk("rx_wifi_len_counter :%d\n",rx_wifi_len_counter);
	
	rx_wifi_len_counter=0;
	
	return 0;	
}

static int rx_wifi_len_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[128];
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';
	
	sscanf(get_buf, "%d",&rx_wifi_len);
	
	printk("rx_wifi_len:%d\n",rx_wifi_len);

	return count;
}

__IMEM int32_t PpeWifiFastTxHandler(struct sk_buff * skb)
{
    uint32_t dest_port = 0, entry_type = 0, ratelimit_index=0;
    struct FoeEntry *foe_entry = NULL;
    
    if (!skb)
        return 0;

    if(foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_dir)
        return 1;

    if (TCSUPPORT_WLAN_MT76_MAC80211_VAL) {
        skb_set_network_header(skb,0);
        skb_push(skb, ETH_HLEN);
    }

    foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + FOE_ENTRY_NUM(skb)*SIZE_OF_FOE_ENTRY);
    
    if (TCSUPPORT_NEW_WDOG_VAL)
        wdog_kick_api(2);
    
    if(ppeIsIfcHit(FOE_ENTRY_NUM(skb), foe_entry))
    {
        return 1;
    }
    
    entry_type = foe_entry->bfib1.pkt_type;
    if(((IPV6_3T_ROUTE == entry_type) || (IPV6_5T_ROUTE == entry_type)) 
            && (foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_dir))
    {
        return 1;
    }
    
    switch(entry_type)
    {
        case L2_BRIDGE:
            dest_port = foe_entry->l2_bridge32.act_dp;
            break;
        case IPV4_HNAPT:
        case IPV4_HNAT:
            dest_port = foe_entry->ipv4_hnapt.act_dp;
            break;
        case IPV4_DSLITE:
            dest_port = foe_entry->ipv4_dslite.act_dp;
            break;
        case IPV6_3T_ROUTE:
            dest_port = foe_entry->ipv6_3t_route.act_dp;
            break;
        case IPV6_5T_ROUTE:
            dest_port = foe_entry->ipv6_5t_route.act_dp;
            break;
        case IPV6_6RD:
            dest_port = foe_entry->ipv6_6rd.act_dp;
            break;
        default:
            dest_port =0;
            break;
    }
    
    if(((dest_port >= DP_RA0) && (dest_port <= DP_RA7)) || 
        ((dest_port >= DP_RAI0) && (dest_port <= DP_RAI7)) ||
        ((dest_port >= DP_RAX0) && (dest_port <= DP_RAX7)))
        skb->dev = DstPort[dest_port]; 
    else
        return 1;

    if ((skb->dev == NULL) || (!(skb->dev->flags & IFF_UP)))
    {
	if(printk_ratelimit())
	    printk("Error: the static rule is error!dp = %d\n",dest_port);
        kfree_skb(skb);
        return 0;
    }

    if(soft_ratelimit_enqueue_hook &&(isWiFiInterface(skb->dev))&&(wifiqueue_start_idx!=-1))
    {
        ratelimit_index = getWifiQueueIndex(skb->dev,WIFI_RATELIMIT_TX);

        if(ratelimit_index!=-1)
            if(1 == soft_ratelimit_enqueue_hook(skb,ratelimit_index))/*wifi tx*/
                return 0;
    }

    if(TxShortFlag == 0 && TCSUPPORT_WLAN_SW_RPS_VAL && toWlan5GPktRpsHandle_hook){ 
        if(0==toWlan5GPktRpsHandle_hook(skb))
            return 0;
    } 

    if(TCSUPPORT_WLAN_SW_RPS_VAL && toWlan2GPktRpsHandle_hook){ 
        if(0==toWlan2GPktRpsHandle_hook(skb))
            return 0;
    }

    skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);

    return 0;
}

__IMEM int32_t getWifiDevName(struct sk_buff * skb)
{
	uint32 dest_port;
	struct FoeEntry *foe_entry = NULL;

	if (!skb)
        	return 0;

	foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + FOE_ENTRY_NUM(skb)*SIZE_OF_FOE_ENTRY);
    
	switch(foe_entry->bfib1.pkt_type)
	{
        	case L2_BRIDGE:
            		dest_port = foe_entry->l2_bridge32.act_dp;
            		break;
        	case IPV4_HNAPT:
        	case IPV4_HNAT:
            		dest_port = foe_entry->ipv4_hnapt.act_dp;
            		break;
#if defined (CONFIG_RA_HW_NAT_IPV6)
        	case IPV4_DSLITE:
            		dest_port = foe_entry->ipv4_dslite.act_dp;
            		break;
        	case IPV6_3T_ROUTE:
            		dest_port = foe_entry->ipv6_3t_route.act_dp;
            		break;
        	case IPV6_5T_ROUTE:
            		dest_port = foe_entry->ipv6_5t_route.act_dp;
            		break;
        	case IPV6_6RD:
            		dest_port = foe_entry->ipv6_6rd.act_dp;
            		break;
#endif
        	default:
            		dest_port =0;
            		break;
	}
    
	if((dest_port >= DP_RA0) && (dest_port <= DP_RA7)){
		skb->dev = DstPort[dest_port];
		return 1;
	}
	else if((dest_port >= DP_RAI0) && (dest_port <= DP_RAI7)){
		skb->dev = DstPort[dest_port];
		return 2;
	}
	else if((dest_port >= DP_RAX0) && (dest_port <= DP_RAX7)){
		skb->dev = DstPort[dest_port];
		printk("[WARN][hw_nat] to-do for rax \n");
		return 0;
	}

	return 0;

}

static int ppe_natv6_rstp_alg_offload_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("nptv6 rtsp alg offload is %s\n",rtsp_v6_alg_offload?"enable":"disable");
	
	return 0;	
}

static int ppe_natv6_rstp_alg_offload_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';
	
	sscanf(get_buf, "%d",&rtsp_v6_alg_offload);
	
	printk("%s rtsp alg offload\n",rtsp_v6_alg_offload?"enable":"disable");

	return count;
}

/* TCSUPPORT_VLAN_ACCESS_TRUNK_VAL start */
int multicast_vlan_stag_handle(struct sk_buff *skb, char *eth_idx, uint16 *eth_vlan_id)
{
	uint8 *cp=NULL;
	uint16 sp_tag = 0;
	uint8 vpm = 0;
	uint8 dst_port = 0;
	uint16 vlan_1 = 0;
	uint16 vlan_2 = 0;
	uint16 offset = 0;
	uint8 gsw_port = 0;
	uint8 lan_port = 0;

	cp = skb->data;
	sp_tag = ntohs(*(uint16 *)(cp+12));
	vpm = (sp_tag & 0x300) >> 8;
	dst_port = sp_tag & 0x3F;

	switch(vpm){
		case 0:/*untag*/
			memmove(skb->data+4, skb->data, 12);
			skb_pull(skb, 4);
			break;
			
		case 1:/*8100*/
		case 2:/*88a8*/
		case 3:/*predefined*/
			vlan_1 = ntohs(*(uint16 *)(cp+14)) & 0xFFF;
			offset = 4;
			if(ntohs(*(uint16 *)(cp+16) == 0x8100) || ntohs(*(uint16 *)(cp+16) == 0x88a8)){
				vlan_2 = ntohs(*(uint16 *)(cp+18)) & 0xFFF;
				offset += 4;
			}

			memmove(skb->data+offset, skb->data, 12);
			skb_pull(skb, offset);
			break;
			
		default:
			printk("[multicast_vlan_stag_handle] wrong vpm\n");
			return -1;
	}

	for(gsw_port = 0; gsw_port < 6; gsw_port ++){
		/* find gsw port */
		if((dst_port) & (1<<gsw_port)){
			if(gsw_port == extend_switch_port){
				for(lan_port = 0; lan_port < max_eth_itf_num; lan_port ++){
					if((eth_port_vlan[lan_port].eth_port_state == 1) && (eth_port_vlan[lan_port].vlan_id == vlan_1)){
						*eth_idx = lan_port;
						break;
					}
				}
				*eth_vlan_id = vlan_2;
			}else{
				*eth_idx = switch_port_map[gsw_port];
				*eth_vlan_id = vlan_1;
			}
			break;
		}
	}
	
	/* reset protocol(0x0800 or 0x86dd) & mac header */
	skb->protocol = *(uint16 *)(skb->data+12);
	skb_reset_mac_header(skb);

	/* reset IP header */
	skb->data = skb->data + ETH_HLEN;
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
	skb_set_network_header(skb, 0);
#else
	LAYER3_HEADER(skb) = skb->data;
#endif
	skb->data = skb->data - ETH_HLEN;

	return 0;
}

void multicast_vlan_dump_skb(struct sk_buff *skb)
{
	char tmp[80];
	char *p = skb->data;
	char *t = tmp;
	int i, n = 0;

	printk("ERR skb=%08lx skb->head=%08lx \n data=%08lx len=%d mark=%08x\n cb=%08lx \n", 
	    (unsigned long) skb, (unsigned long) skb->head, (unsigned long) skb->data, skb->len, skb->mark, 
	    *(unsigned long int *)(skb->cb));
	for (i = 0; i < skb->len; i++) {
		t += sprintf(t, "%02x ", *p++ & 0xff);
		if ((i & 0x0f) == 0x0f) {
			printk("%04x: %s\n", n, tmp);
			n += 16;
			t = tmp;
		}
	}
	if (i & 0x0f)
		printk("%04x: %s\n", n, tmp);
}

int inline multicast_vlan_packet_handle(struct sk_buff *skb, uint32_t dest_port)
{
	HwnatWanMcastVlan_t * entry = NULL;
	struct sk_buff *skb2;
	struct net_device *eth_dev = NULL;
	char eth_idx = 0;
	uint16 eth_vlan_id = 0;
	int relearn_flag = 1; 
	int index = FOE_ENTRY_NUM(skb);
    
	if (unlikely(!multicast_xmit_packet_hook)){
		kfree_skb(skb);
		return 0;
	}

	/* 1. pointer to layer2 header */
	skb_push(skb, ETH_HLEN);
	if (DebugLevel > 6) {
		printk("[multicast_vlan_packet_handle] original packet\n");
		multicast_vlan_dump_skb(skb);
	}

	/* 2. translate sp_tag to eth device, and read vlan info */
	if( 0 > multicast_vlan_stag_handle(skb, &eth_idx, &eth_vlan_id) ) {
		goto multicast_vlan_out;
	}
	
	eth_dev = DstPort[DP_GMAC1+eth_idx];
	if (DebugLevel > 1){
		printk("[multicast_vlan_packet_handle] edited packet\n");
		if (DebugLevel > 6)
			multicast_vlan_dump_skb(skb);
		printk("[multicast_vlan_packet_handle] packet has gone eth0.%d, vlan=%d\n", eth_idx+1, eth_vlan_id);
	}

	/* 3. flood packet to all vethXX devices, if been binded to the wan device */
	spin_lock(&wan_mcast_vlan_lock);
	list_for_each_entry_rcu(entry, &hwnat_wan_mcast_vlan_list[dest_port - DP_MCAST_NAS0], list) {
		if ((entry->vid > 0) && (entry->vid < 4095) && g_veth_lan_device[entry->vid]) {
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if (skb2) {
				skb2->dev = g_veth_lan_device[entry->vid];

				if (DebugLevel > 1) {
					printk("[multicast_vlan_packet_handle] send to vid %d dev %s\n", entry->vid, skb2->dev->name);
				}
				
				relearn_flag &= multicast_xmit_packet_hook(skb2, entry->vid, eth_dev, eth_vlan_id);
			} else {
				printk("[multicast_vlan_packet_handle] skb copy fail!\n");
			}
		}
	}
	spin_unlock(&wan_mcast_vlan_lock);

multicast_vlan_out:
	/* 4. if the full acceleration packet should not forward, clear hw_nat entry */
	if (relearn_flag) {
		hwnat_delete_foe_entry(index);
		printk("[multicast_vlan_packet_handle] relearn hw_nat entry, foe_index = %d!\n", index);
	}

	kfree_skb(skb);
	return 0;
}

int hwnat_get_wan_multicast_device_index(char * dev_name)
{
	int i;
	struct net_device *dev = ra_dev_get_by_name(dev_name);

	if (!dev)
	{
		return -1;
	}

	for (i = DP_MCAST_NAS0; i <= DP_MCAST_PPP7; i++)
	{
		if (dev == DstPort[i])
		{
			dev_put(dev);
			return (i - DP_MCAST_NAS0);
		}
	}

	dev_put(dev);
	return -1;
}

void hwnat_add_wan_multicast_vlan_mapping(uint index, uint16 vid)
{
	HwnatWanMcastVlan_t * entry = NULL;

	spin_lock_bh(&wan_mcast_vlan_lock);
	list_for_each_entry_rcu(entry, &hwnat_wan_mcast_vlan_list[index], list)
	{
		if(entry->vid == vid)
		{
			printk("Info: vlan %d already bound!\n", vid);
			spin_unlock_bh(&wan_mcast_vlan_lock);
			return;
		}
	}
	spin_unlock_bh(&wan_mcast_vlan_lock);

	entry = (HwnatWanMcastVlan_t*)kmalloc(sizeof(HwnatWanMcastVlan_t), GFP_ATOMIC);
	if (!entry)
	{
		printk("Error: not enough memory!\n");
		return;
	}
	memset(entry, 0, sizeof(HwnatWanMcastVlan_t));
	entry->vid = vid;

	spin_lock_bh(&wan_mcast_vlan_lock);
	list_add_rcu(&(entry->list), &(hwnat_wan_mcast_vlan_list[index]));
	spin_unlock_bh(&wan_mcast_vlan_lock);

}

void hwnat_del_wan_multicast_vlan_mapping(uint index, uint16 vid)
{
	HwnatWanMcastVlan_t * entry = NULL;
	HwnatWanMcastVlan_t * tmp = NULL;

	spin_lock_bh(&wan_mcast_vlan_lock);
	list_for_each_entry_safe(entry, tmp, &hwnat_wan_mcast_vlan_list[index], list)
	{
		if(entry->vid == vid)
		{
			list_del(&entry->list);
			kfree(entry);
			spin_unlock_bh(&wan_mcast_vlan_lock);
			return;
		}
	}
	spin_unlock_bh(&wan_mcast_vlan_lock);

	printk("Info: vlan %d not bound!\n", vid);
}

void hwnat_clear_wan_multicast_vlan_mapping(uint index)
{
	HwnatWanMcastVlan_t * entry = NULL;
	HwnatWanMcastVlan_t * tmp = NULL;

	spin_lock_bh(&wan_mcast_vlan_lock);
	list_for_each_entry_safe(entry, tmp, &hwnat_wan_mcast_vlan_list[index], list)
	{
		list_del(&entry->list);
		kfree(entry);
		entry = NULL;
	}
	spin_unlock_bh(&wan_mcast_vlan_lock);
}

static int hwnat_wan_multicast_vlan_mapping_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
	int i;
	struct net_device *dev;
	HwnatWanMcastVlan_t * entry = NULL;

	for (i = DP_MCAST_NAS0; i <= DP_MCAST_PPP7; i++)
	{
		dev = DstPort[i];
		if (dev)
		{
			printk("Dev: %s\n", dev->name);
			printk("Vlan:");
			spin_lock_bh(&wan_mcast_vlan_lock);
			list_for_each_entry_rcu(entry, &hwnat_wan_mcast_vlan_list[i - DP_MCAST_NAS0], list)
			{
				printk(" %d", entry->vid);
			}
			spin_unlock_bh(&wan_mcast_vlan_lock);
			printk("\n\n");
		}
	}

	return 0;   
}

static int hwnat_wan_multicast_vlan_mapping_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
	char val_string[65];
	char action[32];
	char dev_name[32] ;
	uint vid;
	int index;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
    
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
    
	val_string[count] = '\0';
	sscanf(val_string, "%s %s %d", action, dev_name, &vid);

	index = hwnat_get_wan_multicast_device_index(dev_name);
	if (index < 0)
	{
		printk("Wrong device name!\n");
		return -EFAULT;
	}

	if (!strcmp(action, "add"))
	{
		hwnat_add_wan_multicast_vlan_mapping(index, vid);
	}
	else if (!strcmp(action, "del"))
	{
		hwnat_del_wan_multicast_vlan_mapping(index, vid);
	}
	else if (!strcmp(action, "reset"))
	{
		hwnat_clear_wan_multicast_vlan_mapping(index);
	}
	else
	{
		printk("Usage: echo (add|del|reset) (dev_name) (vid) > /proc/tc3162/wan_multicast_vlan_mapping\n");
		return -EFAULT;
	}

	return count;
}
/* TCSUPPORT_VLAN_ACCESS_TRUNK_VAL end */

static FORCE_TO_CPU_IF ppeHitBindForceToCpuGetDp(struct sk_buff *skb, struct FoeEntry *foe_entry,unsigned int* p_dp, struct force_to_cpu_para *p_para)
{
	unsigned int dp=0,dsoffload= 0,natv6_dir = 0,natv6_attr = 0,natv6_src_wlan = 0;
	unsigned int entry_type = foe_entry->bfib1.pkt_type;
	unsigned int foe_index = FOE_ENTRY_NUM(skb);

	switch(entry_type)
	{
		case L2_BRIDGE:
			dp = foe_entry->l2_bridge32.act_dp;
			break;
		case IPV4_HNAPT:
		case IPV4_HNAT:
			dp = foe_entry->ipv4_hnapt.act_dp;
			dsoffload = foe_ext[foe_index].ds_offload;
			break;
		case IPV4_DSLITE:
			dp = foe_entry->ipv4_dslite.act_dp;			
			break;
		case IPV6_3T_ROUTE:
			dp = foe_entry->ipv6_3t_route.act_dp;
			natv6_dir = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_dir;
			natv6_attr = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_attr;
			natv6_src_wlan = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan;
			break;
		case IPV6_5T_ROUTE:
			dp = foe_entry->ipv6_5t_route.act_dp;
			dsoffload = foe_ext[foe_index].ds_offload;
			natv6_dir = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_dir;
			natv6_attr = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_attr;
			natv6_src_wlan = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan;
			break;
		case IPV6_6RD:
			dp = foe_entry->ipv6_6rd.act_dp;
			break;
		default:
			return FORCE_TO_CPU_INVALID;
    	}

	*p_dp = dp;
	p_para->ds_offload = dsoffload;
	p_para->natv6_dir = natv6_dir;
	p_para->natv6_attr = natv6_attr;
	p_para->natv6_src_wlan = natv6_src_wlan;

	if((dsoffload) && (dsoffload != RTSP_OFFLOAD_WLAN))
		return FORCE_TO_CPU_DS_OFFLOAD;

	if(natv6_dir)
		return FORCE_TO_CPU_NATV6;

    if(ppeIsIfcHit(FOE_ENTRY_NUM(skb), foe_entry))
    {
        return FORCE_TO_CPU_HIT_IFC;
    }

	if(ppe_is_multicast_entry(foe_entry) && (DP_LOCAL_HOST != dp))
		return FORCE_TO_CPU_MULTICAST;
	
	if(((dp >= DP_RA0) && (dp <= DP_RA7)) || ((dp >= DP_RAI0) && (dp <= DP_RAI7)) || 
		((dp >= DP_RAX0) && (dp <= DP_RAX7)) || (dp == DP_APCLI0) || (dp == DP_APCLII0) || (dp == DP_APCLIX0))
		return FORCE_TO_CPU_WLAN;
	else if(((dp >= DP_CRYPTO_E_0) &&(dp < DP_CRYPTO_E_MAX))||((dp >= DP_CRYPTO_D_0) && (dp < DP_CRYPTO_D_MAX)))
		return FORCE_TO_CPU_IPSEC;
        else if(DP_OOKLA_RX == dp)
		return FORCE_TO_CPU_OOKLA;
	else if(DP_LOCAL_HOST == dp)
		return FORCE_TO_CPU_LOCAL_IN;
	else if((dp >= DP_NAS0) && (dp <= DP_PPP7))
		return FORCE_TO_CPU_GRE_VXLAN_WAN_OFFLOAD;
	else if (((entry_type ==IPV4_HNAPT) || (entry_type ==IPV4_HNAT) || (entry_type ==L2_BRIDGE)) 
	&& (((dp >= DP_GRE_TUNNEL_0) && (dp <= DP_GRE_TUNNEL_7))
	|| ((dp >= DP_GRE_L2_PRI_TUNNEL_0) && (dp <= DP_GRE_L2_SEC_TUNNEL_7))))
		return FORCE_TO_CPU_GRE_TUNNEL_OFFLOAD;
	else if ((dp >= DP_VXLAN_0) && (dp <= DP_VXLAN_19))
		return FORCE_TO_CPU_VXLAN_TUNNEL_OFFLOAD;
	else if((dp == DP_PTM0) ||(dp == DP_PTM1) || (dp == DP_PON))
	{
		if((IPV4_HNAPT == entry_type)||(IPV4_HNAT == entry_type))		
		{
			skb->macEnque = foe_ext[foe_index].qos_que_index;			
			skb->macEnque|= 1<<7;
		}
		else if((IPV4_DSLITE == entry_type)||(IPV6_3T_ROUTE == entry_type)||(IPV6_5T_ROUTE == entry_type)||(IPV6_6RD == entry_type))
		{
			skb->macEnque = (foe_entry->ipv6_5t_route.channel&0xf0)>>4;
			skb->macEnque|= 1<<7;
		}
			
		return FORCE_TO_CPU_SOFT_RATELIMIT;
	}
	else if(((DP_ASYM_D1 <= dp)&&(dp <= DP_ASYM_D5))||((DP_ASYM_U1 <= dp)&&(dp <= DP_ASYM_U5)))
		return FORCE_TO_CPU_ASYM_OFFLOAD;
    else if(((dp >= DP_USBNET0) && (dp < DP_USBNET0 + HWNAT_USB_IF_NUM))||(dp == DP_USBNET2))
        return FORCE_TO_CPU_DEFAULT_OFFLOAD;
			
	return FORCE_TO_CPU_INVALID;
	
}

inline int PpeHitBindForceToCpuHandler(struct sk_buff *skb, struct FoeEntry *foe_entry)
{
	int dir = FORCE_TO_CPU_INVALID;
	unsigned int dp = 0;
	struct force_to_cpu_para para;

	memset(&para,0,sizeof(struct force_to_cpu_para));
	dir = ppeHitBindForceToCpuGetDp(skb,foe_entry,&dp,&para);
	
	if(FORCE_TO_CPU_INVALID == dir)
		return 1;

	if((FORCE_TO_CPU_NATV6) == dir 
		&& (NATV6_DIR_WLAN != para.natv6_dir)
		&& (NATV6_DIR_USB != para.natv6_dir)
		&& (!para.natv6_src_wlan))
	{
		if(0 ==offload_rps_handle(skb))
			return 0;
	}
	/*all the handler func,if return 0,make sure skb is freed*/
	return force_to_cpu_handler[dir](skb,dp,&para, foe_entry);
}

int PpeHitBindForceToCpuExternal(struct sk_buff *skb)
{
	struct FoeEntry *foe_entry;	
	foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + FOE_ENTRY_NUM(skb)*SIZE_OF_FOE_ENTRY);
	return PpeHitBindForceToCpuHandler(skb, foe_entry);
}

int PpeNatv6WanRate(struct sk_buff * skb, unsigned int natv6_dir)
{
    int dir = 1;

    if(!isEN7580)
        return -1;
    
    if(NULL == soft_ratelimit_enqueue_hook)
        return -1;
    
    if(NATV6_DIR_GPON == natv6_dir || NATV6_DIR_EPON == natv6_dir|| NATV6_DIR_AE_WAN == natv6_dir)
        dir = 0;

    if(0 == natv6_wan_rate[dir].enable)
        return -1;
    
    if(1 == soft_ratelimit_enqueue_hook(skb,natv6_wan_rate[dir].soft_queue_id))
        return 0;

    return -1;
}

int PpeNatv6FastHandler(struct sk_buff * skb)
{
	unsigned int natv6_dir = 0, natv6_attr = 0, dest_port = 0, natv6_src_wlan = 0;
	struct FoeEntry *foe_entry = NULL;
	int foe_entry_idx = FOE_ENTRY_NUM(skb);
	struct force_to_cpu_para para;

	natv6_attr = foe_ext[foe_entry_idx].natv6.natv6_attr;
	if(!natv6_attr)
		return 1;

	if(unlikely(FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU))
		return 1;

	natv6_dir = foe_ext[foe_entry_idx].natv6.natv6_dir;
    
    if(0 == PpeNatv6WanRate(skb, natv6_dir))
        return 0;
    
	natv6_src_wlan = foe_ext[foe_entry_idx].natv6.natv6_src_wlan;

    skb_reset_mac_header(skb);
    skb_pull(skb, ETH_HLEN);
	if((NATV6_DIR_WLAN != natv6_dir)
		&& (!natv6_src_wlan))
	{
		if(0 ==offload_rps_handle(skb))
			return 0;
	}


	foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + foe_entry_idx*SIZE_OF_FOE_ENTRY);

	memset(&para,0,sizeof(struct force_to_cpu_para));	 
	para.natv6_attr = natv6_attr;
	para.natv6_dir = natv6_dir;
	para.natv6_src_wlan = natv6_src_wlan;
	para.ds_offload = foe_ext[foe_entry_idx].ds_offload;	
	dest_port = foe_entry->ipv6_3t_route.act_dp;

	return ecnt_offload_natv6(skb,dest_port,&para,foe_entry);
}

int PpeHitIfcFastHandler(struct sk_buff * skb)
{
    int foe_entry_idx = FOE_ENTRY_NUM(skb);
    struct FoeEntry *foe_entry = NULL;
    struct force_to_cpu_para para;
    int dest_port = 0;
    if(unlikely(FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU))
    {
        return 1;
    }
    foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + foe_entry_idx*SIZE_OF_FOE_ENTRY);
    if(!ppeIsIfcHit(foe_entry_idx, foe_entry))
    {
        return 1;
    }

    skb_reset_mac_header(skb);
    skb_pull(skb, ETH_HLEN);

    dest_port = foe_entry->l2_bridge32.act_dp;
    memset(&para,0,sizeof(struct force_to_cpu_para));	 
    para.ds_offload = foe_ext[foe_entry_idx].ds_offload;

    return ecnt_offload_hit_ifc(skb, dest_port, &para, foe_entry);
}

static int PpeNatv6SoftRateDeque(struct sk_buff * skb)
{
	unsigned int natv6_dir = 0, natv6_attr = 0, dest_port = 0;
	struct FoeEntry *foe_entry = NULL;
	int foe_entry_idx = FOE_ENTRY_NUM(skb);
	struct force_to_cpu_para para;

	natv6_attr = foe_ext[foe_entry_idx].natv6.natv6_attr;
	natv6_dir = foe_ext[foe_entry_idx].natv6.natv6_dir;
	
	skb_reset_mac_header(skb);
	skb_pull(skb, ETH_HLEN);

	foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + foe_entry_idx*SIZE_OF_FOE_ENTRY);

	memset(&para,0,sizeof(struct force_to_cpu_para));	 
	para.natv6_attr = natv6_attr;
	para.natv6_dir = natv6_dir;
	para.ds_offload = foe_ext[foe_entry_idx].ds_offload;	
	dest_port = foe_entry->ipv6_3t_route.act_dp;

	return ecnt_offload_natv6(skb,dest_port,&para,foe_entry);
}

static int offload_rps_handle(struct sk_buff *skb)
{
    if ((offload_rps_cpu >= 0) && (offload_rps_cpu < 4))
    {
        enqueue_to_cpu_queue(skb, offload_rps_cpu, offload_rps_cpu);
        return 0;
    }

    return -1;
}

static int enqueue_to_cpu_queue(struct sk_buff *skb, int cpu, int queue)
{
    struct offload_softnet_data *sd;
    unsigned long flags;
    unsigned int queue_len;

    sd = &pkt_queue[queue];

    local_irq_save(flags);
    spin_lock(&sd->input_pkt_queue.lock);
    queue_len = skb_queue_len(&sd->input_pkt_queue);
    if(queue_len > sd->max_queue_len)
        sd->max_queue_len = queue_len;
    if (queue_len <= offload_rps_max_queue) {
        if (queue_len){
enqueue:
            __skb_queue_tail(&sd->input_pkt_queue, skb);
	        sd->input_num++;
            spin_unlock(&sd->input_pkt_queue.lock);
            local_irq_restore(flags);
            return NET_RX_SUCCESS;
        }
        if((!__test_and_set_bit(NAPI_STATE_SCHED, &sd->napi.state)))
        {
            if (cpu != get_cpu()){
                if(cpu_online(cpu)){
                    smp_call_function_single_ext(cpu, &sd->csd);
                }
            }
            else{
                __napi_schedule(&sd->napi);
            }
            put_cpu();
        }
        goto enqueue;
    }

    sd->dropped++;
    spin_unlock(&sd->input_pkt_queue.lock);

    local_irq_restore(flags);

    kfree_skb(skb);
    return NET_RX_DROP;
}

static int offload_rps_process(struct sk_buff *skb)
{
    struct force_to_cpu_para para;

    memset(&para,0,sizeof(struct force_to_cpu_para));

    para.natv6_dir = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_dir;
    para.natv6_attr = foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_attr;

    ecnt_offload_natv6(skb, 0, &para, NULL);
    
    return 0;
}

static int offload_rps_poll(struct napi_struct *napi, int quota, struct offload_softnet_data *sd, 
    int (*offload_rps_pkt_process)(struct sk_buff * skb))
{
    int work = 0;
	
    napi->weight = quota;
    local_irq_disable();
    while (1) {
        struct sk_buff *skb;
        while ((skb = __skb_dequeue(&sd->process_queue))) {
            local_irq_enable();
            offload_rps_pkt_process(skb);
            local_irq_disable();
            if (++work >= quota) {
                local_irq_enable();
                return work;
            }
        }

        spin_lock(&sd->input_pkt_queue.lock);
        if (skb_queue_empty(&sd->input_pkt_queue)) {
		/*
		 * Inline a custom version of __napi_complete().
		 * only current cpu owns and manipulates this napi,
		 * and NAPI_STATE_SCHED is the only possible flag set
		 * on backlog.
		 * We can use a plain write instead of clear_bit(),
		 * and we dont need an smp_mb() memory barrier.
		 */
		list_del(&napi->poll_list);
		napi->state = 0;
		spin_unlock(&sd->input_pkt_queue.lock);
		break;
	}

	skb_queue_splice_tail_init(&sd->input_pkt_queue,&sd->process_queue);
        spin_unlock(&sd->input_pkt_queue.lock);
    }
    local_irq_enable();
    return work;
}

static int offload_poll_cpu0(struct napi_struct *napi, int quota)
{
	struct offload_softnet_data *sd = &pkt_queue[0];
	return offload_rps_poll(napi, quota, sd, offload_rps_process);
}

static int offload_poll_cpu1(struct napi_struct *napi, int quota)
{
	struct offload_softnet_data *sd = &pkt_queue[1];
	return offload_rps_poll(napi, quota, sd, offload_rps_process);
}

static int offload_poll_cpu2(struct napi_struct *napi, int quota)
{
	struct offload_softnet_data *sd = &pkt_queue[2];
	return offload_rps_poll(napi, quota, sd, offload_rps_process);
}

static int offload_poll_cpu3(struct napi_struct *napi, int quota)
{
	struct offload_softnet_data *sd = &pkt_queue[3];
	return offload_rps_poll(napi, quota, sd, offload_rps_process);
}

static void offload_rps_napi_init(void)
{
    int i;

    for(i=0;i<4;i++)
    {
        memset(&pkt_queue[i], 0, sizeof(pkt_queue[i]));
        skb_queue_head_init(&pkt_queue[i].input_pkt_queue);
        skb_queue_head_init(&pkt_queue[i].process_queue);

        pkt_queue[i].csd.func = (void *)__napi_schedule;
        pkt_queue[i].csd.info = &pkt_queue[i].napi;
        pkt_queue[i].csd.flags = 0;

        pkt_queue[i].napi.weight = 16;
        pkt_queue[i].napi.gro_list = NULL;
        pkt_queue[i].napi.gro_count = 0;
    }

    pkt_queue[0].napi.poll = offload_poll_cpu0;
    pkt_queue[1].napi.poll = offload_poll_cpu1;
    pkt_queue[2].napi.poll = offload_poll_cpu2;
    pkt_queue[3].napi.poll = offload_poll_cpu3;
    return;
}

static int offload_rps_cpu_read_proc(char *buf, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("offload_rps_cpu %d\n", offload_rps_cpu);	
	printk("offload_rps_max_queue %d\n", offload_rps_max_queue);

    printk("               CPU0       CPU1       CPU2       CPU3\n");
    printk("enqueue: %10d %10d %10d %10d\n", 
        pkt_queue[0].input_num, pkt_queue[1].input_num, pkt_queue[2].input_num, pkt_queue[3].input_num);
    printk("cur_len: %10d %10d %10d %10d\n", 
        skb_queue_len(&pkt_queue[0].input_pkt_queue), skb_queue_len(&pkt_queue[1].input_pkt_queue), 
        skb_queue_len(&pkt_queue[2].input_pkt_queue), skb_queue_len(&pkt_queue[3].input_pkt_queue));
    printk("dropped: %10d %10d %10d %10d\n", 
        pkt_queue[0].dropped, pkt_queue[1].dropped, pkt_queue[2].dropped, pkt_queue[3].dropped);
    printk("max_len: %10d %10d %10d %10d\n", 
        pkt_queue[0].max_queue_len, pkt_queue[1].max_queue_len, pkt_queue[2].max_queue_len, pkt_queue[3].max_queue_len);

	return 0;
}

int PpeVxlanFastHandler(struct sk_buff * skb)
{
    int foe_entry_idx = FOE_ENTRY_NUM(skb);
    int dest_port = foe_ext[foe_entry_idx].dp;
    struct FoeEntry *foe_entry = NULL;


   if(unlikely(FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU))
       return 1;
   if (((dest_port >= DP_NAS0) && (dest_port <= DP_PPP7)) || 
      	((dest_port >= DP_VXLAN_0) && (dest_port <= DP_VXLAN_19)))
   {
     foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + foe_entry_idx*SIZE_OF_FOE_ENTRY);
     skb->dev = DstPort[dest_port];
     skb->protocol = eth_type_trans(skb, skb->dev);
     skb->dev->last_rx = jiffies;
     skb->pkt_type = PACKET_HOST;

     if ((dest_port >= DP_NAS0) && (dest_port <= DP_PPP7))
     {
          return ecnt_offload_gre_vxlan_wan_offload(skb, dest_port, NULL, foe_entry);
     }else if ((dest_port >= DP_VXLAN_0) && (dest_port <= DP_VXLAN_19))
     {
          return ecnt_offload_vxlan_tunnel_offload(skb, dest_port, NULL, foe_entry);
     }
   }
   return 1;
   
}


static int offload_rps_cpu_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64];
	int len = 0;
	int i;
	int value = 0;
	char cmd[10];

	if ( count > sizeof(val_string) - 1 )
		len = sizeof(val_string) - 1;
	else
		len = count;

	memset(val_string, 0, sizeof(val_string));
	if ( copy_from_user(val_string, buffer, len) )
		return -EFAULT;

	memset(cmd,0,sizeof(cmd));
	sscanf(val_string, "%s %d", cmd, &value);

	if(strncmp(cmd, "cpu", 3) == 0)
	{
		offload_rps_cpu = value;
	}
	else if(strncmp(cmd, "max", 3) == 0)
	{
		offload_rps_max_queue = value;
	}

    for (i = 0; i < 4; i++)
    {
        pkt_queue[i].input_num = 0;
        pkt_queue[i].dropped = 0;
        pkt_queue[i].max_queue_len = 0;
    }

	return len;
}

static int natv6_wan_ratelimit_read_proc(
                char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
    printk("natv6 up ratelimit:\n");
    printk("\tenable:%d ratelimit:%d softqueue:%d\n", \
        natv6_wan_rate[0].enable,natv6_wan_rate[0].ratelimit,natv6_wan_rate[0].soft_queue_id);

    printk("natv6 down ratelimit:\n");
    printk("\tenable:%d ratelimit:%d softqueue:%d\n", \
        natv6_wan_rate[1].enable,natv6_wan_rate[1].ratelimit,natv6_wan_rate[1].soft_queue_id);

    return 0;
}

static int natv6_wan_ratelimit_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[32];
    unsigned char wan_itf[8];
    unsigned char dir[8];
    unsigned int ratelimit = 0;
    unsigned int enable;
    int queue_idx = -1;
    unsigned char direction = 0;

    if (count > sizeof(get_buf) - 1)
        return -EINVAL ;

    if(NULL == soft_ratelimit_allocatequeue_hook || 
        NULL == soft_ratelimit_set_queue_hook)   
        return -EFAULT;
    
    memset(get_buf, 0, sizeof(get_buf));
    
    if(copy_from_user(get_buf, buffer, count))
        return -EFAULT;

    memset(wan_itf,0,8);
    memset(dir,0,8);
    
    sscanf(get_buf, "%7s %7s %d %d",wan_itf, dir, &ratelimit, &enable);

    if(!strcmp(dir, "up"))
        direction = 0;
    else if(!strcmp(dir, "down"))
        direction = 1; 
    else
        goto help;

    if(-1 == natv6_wan_rate[direction].soft_queue_id)
        natv6_wan_rate[direction].soft_queue_id = soft_ratelimit_allocatequeue_hook(1);
    queue_idx = natv6_wan_rate[direction].soft_queue_id;

    if(-1 == queue_idx)
    {
        printk("alloc soft queue failed\n");
        return -EFAULT;
    }

    natv6_wan_rate[direction].ratelimit= ratelimit; 
    natv6_wan_rate[direction].enable = enable;

    soft_ratelimit_set_queue_hook(queue_idx,ratelimit,PpeNatv6SoftRateDeque);

    return count;

    help:
    printk("echo wan_itf(nasx_x) dir(up/down) ratelimit(kbps) enable(0/1) > /proc/tc3162/natv6_wan_ratelimit\n");
    printk("Example:nas1_0 down 1000 1 > /proc/tc3162/natv6_wan_ratelimit\n");
    return count;
}

static void natv6_wan_ratelimit_recycle(void)
{
    int i = 0;

    if(NULL == soft_ratelimit_recyclequeue_hook)
        return;
    
    for(i=0;i<2;i++)
    {
        if(natv6_wan_rate[i].soft_queue_id != -1)
            soft_ratelimit_recyclequeue_hook(natv6_wan_rate[i].soft_queue_id, 1);
    }
    
    return;
}

static struct offload_softnet_data left_to_right_pkt_queue[2];
static int vxlan_left_to_right_up_process(struct sk_buff *skb)
{
    if(skb == NULL) {
		return 0;
    }
	
    if(unlikely(skb->dev == NULL)) {
        kfree_skb(skb);
		return 0;
    }
    
    skb->protocol = eth_type_trans(skb, skb->dev);
    skb_set_network_header(skb, 0);
    
    if((skb->dev == NULL) || (skb->dev->type != ARPHRD_IPGRE))
        skb_push(skb, ETH_HLEN);    //pointer to layer2 header
    
    if (unlikely((skb->dev == NULL) || (!(skb->dev->flags & IFF_UP))))
    {
        if(printk_ratelimit())
            printk("%s skb->dev %s is error!\n",\
            __FUNCTION__, skb->dev->name);
        kfree_skb(skb);
        return 0;
    }

    skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
    
    return 0;
}

static int vxlan_left_to_right_down_process(struct sk_buff *skb)
{
    if(skb == NULL) {
		return 0;
    }
	
    if(unlikely(skb->dev == NULL)) {
        kfree_skb(skb);
		return 0;
    }
	
    skb->protocol = eth_type_trans(skb, skb->dev);
    skb_set_network_header(skb, 0);
    skb->pkt_type = PACKET_HOST;

    netif_receive_skb(skb);
    
    return 0;
}

static int vxlan_left_to_right_enqueue(struct sk_buff *skb, int cpu, int queue)
{
    struct offload_softnet_data *sd;
    unsigned long flags;
    unsigned int queue_len;

    sd = &left_to_right_pkt_queue[queue];

    local_irq_save(flags);
    spin_lock(&sd->input_pkt_queue.lock);
    queue_len = skb_queue_len(&sd->input_pkt_queue);
    if(queue_len > sd->max_queue_len)
        sd->max_queue_len = queue_len;
    if (queue_len <= 100) {
        if (queue_len){
enqueue:
            __skb_queue_tail(&sd->input_pkt_queue, skb);
            sd->input_num++;
            spin_unlock(&sd->input_pkt_queue.lock);
            local_irq_restore(flags);
            return NET_RX_SUCCESS;
        }
        if((!__test_and_set_bit(NAPI_STATE_SCHED, &sd->napi.state)))
        {
            if (cpu != get_cpu()){
                if(cpu_online(cpu)){
                    smp_call_function_single_ext(cpu, &sd->csd);
                }
            }
            else{
                __napi_schedule(&sd->napi);
            }
            put_cpu();
        }
        goto enqueue;
    }

    sd->dropped++;
    spin_unlock(&sd->input_pkt_queue.lock);

    local_irq_restore(flags);

    kfree_skb(skb);
    return NET_RX_DROP;
}

static inline struct sk_buff * vxlan_left_to_right_alloc_skb(void)
{
    int offset=0 ;
    struct sk_buff * skb=NULL ;
    
    /* allocate the packet buffer */
    skb = skbmgr_dev_alloc_skb2k() ;
    if(unlikely(skb == NULL)) {
        if(printk_ratelimit()){
            printk("vxlan_left_to_right_alloc_skb failed.\n");
        }
        goto finish ;
    }
    
    /* Shift to 4 byte alignment */
    offset = ((uint)(skb->tail) & (8-1)) ;
    if(offset) {
        skb_reserve(skb, (8 - offset)) ;
    }

finish:
    return skb;    
}

static inline void vxlan_left_to_right_down_to_kernel
    (char *data_ptr, unsigned short *data_len_ptr, unsigned char dp)
{
    struct sk_buff *skb = NULL;
    unsigned short data_len =  *data_len_ptr;

    skb = vxlan_left_to_right_alloc_skb();
    if(unlikely(!skb))
        return;
    memcpy(skb->data,data_ptr,data_len);
    skb_put(skb, data_len);  
    skb->dev = ppeGetDstPort(dp);

    vxlan_left_to_right_enqueue(skb, 0, 1);

    return;
}

static inline void vxlan_left_to_right_up_to_kernel(char *data_ptr, 
    unsigned short *data_len_ptr, unsigned short foe_index, unsigned char dp)
{
    struct sk_buff *skb = NULL;
    unsigned short data_len =  *data_len_ptr;

    skb = vxlan_left_to_right_alloc_skb();
    if(unlikely(!skb))
        return;
    memcpy(skb->data,data_ptr,data_len);
    skb_put(skb, data_len);  
    FOE_ENTRY_NUM(skb) = foe_index;
    FOE_AI(skb) = HIT_BIND_FORCE_TO_CPU;
    skb->dev = ppeGetDstPort(dp);

    vxlan_left_to_right_enqueue(skb, 0, 0);

    return;
}

static int vxlan_left_to_right_handle(char *data_ptr, unsigned short *data_len_ptr, 
    unsigned short foe_index, int *data_offset, unsigned short *VirIfIdx)
{
    const struct iphdr *iph;
    const struct udphdr *uh;
    unsigned char dp = foe_ext[foe_index].dp;
    int ret = LEFT_TO_RIGHT_RET_FAIL;

    if((dp >= DP_NAS0) && (dp <= DP_PPP7))/* downstream */
    {
        iph = (struct iphdr *)(data_ptr+ETH_HLEN);
        if ((iph->protocol == IPPROTO_UDP) && (!(ntohs(iph->frag_off) & IP_MF))) 
        {
            int i, vni;
            struct net_device* dev_tmp;

            uh = (struct udphdr *)((uint8_t *) iph + iph->ihl * 4);
            vni = (ntohl(*(int*)((uint8_t *)uh+8+4)) >> 8) & 0x00FFFFFF;
            dev_tmp = vxlan_getlink(vni);
            if (dev_tmp)
            {
                for (i = DP_VXLAN_0; i<= DP_VXLAN_19; i++)
                {
                    if (dev_tmp == DstPort[i])
                        break;
                }
                
                if (i <= DP_VXLAN_19)
                {
                    *VirIfIdx = i;
                    *data_offset = ETH_HLEN+20+16;
                    return DIR_DOWN_SUCCESS;
                }
            }
        }
        
        vxlan_left_to_right_down_to_kernel(data_ptr, data_len_ptr, dp);
    }
    else/* upstream */
    {
        if(likely(vxlan_left_to_right_xmit_hook))
            ret = vxlan_left_to_right_xmit_hook(data_ptr, data_len_ptr, foe_index, DstPort[dp]);
        if(likely(DIR_UP_SUCCESS == ret || DIR_UP_FRAG_SUCCESS == ret))
        {
            *VirIfIdx = dp - DP_VXLAN_0 + DP_VXLAN_UP_2_0;
            *data_offset = -(36 + ETH_HLEN);
            return ret;
        }
        else
            vxlan_left_to_right_up_to_kernel(data_ptr, data_len_ptr, foe_index, dp);
    }

    return LEFT_TO_RIGHT_RET_FAIL;
}

static int vxlan_left_to_right_poll_up(struct napi_struct *napi, int quota)
{
    struct offload_softnet_data *sd = &left_to_right_pkt_queue[0];
    return offload_rps_poll(napi, quota, sd, vxlan_left_to_right_up_process);
}

static int vxlan_left_to_right_poll_down(struct napi_struct *napi, int quota)
{
    struct offload_softnet_data *sd = &left_to_right_pkt_queue[1];
    return offload_rps_poll(napi, quota, sd, vxlan_left_to_right_down_process);
}

static void vxlan_left_to_right_napi_init(void)
{
    int i;

    for(i=0;i<2;i++)
    {
        memset(&left_to_right_pkt_queue[i], 0, sizeof(left_to_right_pkt_queue[i]));
        skb_queue_head_init(&left_to_right_pkt_queue[i].input_pkt_queue);
        skb_queue_head_init(&left_to_right_pkt_queue[i].process_queue);

        left_to_right_pkt_queue[i].csd.func = (void *)__napi_schedule;
        left_to_right_pkt_queue[i].csd.info = &left_to_right_pkt_queue[i].napi;
        left_to_right_pkt_queue[i].csd.flags = 0;

        left_to_right_pkt_queue[i].napi.weight = 16;
        left_to_right_pkt_queue[i].napi.gro_list = NULL;
        left_to_right_pkt_queue[i].napi.gro_count = 0;
    }

    left_to_right_pkt_queue[0].napi.poll = vxlan_left_to_right_poll_up;
    left_to_right_pkt_queue[1].napi.poll = vxlan_left_to_right_poll_down;

    return;
}

void ecnt_hwnat_offload_init(void)
{	
	struct proc_dir_entry *hwnat_proc;
	
	//Get net_device structure of Dest Port 
	PpeSetDstPort(1);
	
	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL){
		hwnat_proc = create_proc_entry("tc3162/softqdma_udf_tcpport", 0, NULL);
		hwnat_proc->read_proc = softqdma_udf_port_read_proc;
		hwnat_proc->write_proc =  softqdma_udf_port_write_proc;	
	}
	
	if(TCSUPPORT_DLF_CTRL_VAL)
	{
		spin_lock_init(&dlf_fdb_lock);
        spin_lock_init(&dlf_nf_lock);
		INIT_LIST_HEAD(&DLFEntryTable);
        INIT_LIST_HEAD(&DLFNFEntryTable);
		/* auto update dlf table */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		init_timer(&dlf_update_timer) ;
		dlf_update_timer.expires = jiffies + msecs_to_jiffies(100); 
		dlf_update_timer.function = auto_dlf_update ;
#else
		timer_setup(&dlf_update_timer, auto_dlf_update, 0);
		dlf_update_timer.expires = jiffies + msecs_to_jiffies(100);
#endif
		add_timer(&dlf_update_timer);

		DlfHookFuncInit();
		
		/* To create dlf_enable proc  */
		hwnat_proc = create_proc_entry("tc3162/dlf_enable", 0, NULL);
		if (hwnat_proc)
		{
			hwnat_proc->read_proc = dlf_control_enable_read_proc ;
			hwnat_proc->write_proc = dlf_control_enable_write_proc ;
		} 
		else 
		{
			printk("proc_entry tc3162/dlf_enable alloc fail\n");
		}
	    /* To create hwnat_meter proc  */
		hwnat_proc = create_proc_entry("tc3162/dlf_hwnat_meter", 0, NULL);
		if (hwnat_proc)
		{
			hwnat_proc->write_proc = dlf_hwnat_meter_write_proc;
		} 
		else 
		{
			printk("proc_entry tc3162/dlf_hwnat_meter alloc fail\n");
		}
	}

	ppeLocalInListInit();
	ppeLocalInAddDportOne(SMB_PORT1);
	ppeLocalInAddDportOne(SMB_PORT2);
	
	ra_sw_nat_local_in_tx = ppeLocalInTcpTx;
	ra_hit_bind_force_to_cpu = PpeHitBindForceToCpuExternal;

	hwnat_proc = create_proc_entry("tc3162/vpn_enable", 0, NULL);
	hwnat_proc->read_proc = ppe_vpnenable_read_proc;
	hwnat_proc->write_proc = ppe_vpnenable_write_proc;
	
	ra_sw_nat_mcst_offload = PpeGetOffload;
	ra_sw_nat_ds_offload = PpeDownstreamOffload;
	ra_sw_nat_rtsp_offload_restore = PpeRestoreRtspIpPort;

	/* l2tp */
	InitL2tp();

	InitGre();

    InitVxlan();

	register_netdevice_notifier(&ppe_device_notifier);

	hwnat_proc = create_proc_entry("tc3162/rps_enable", 0, NULL);
	hwnat_proc->read_proc = hwnat_rpsenable_read_proc;
	hwnat_proc->write_proc =  hwnat_rpsenable_write_proc;

	ra_sw_nat_hook_wifi_tx = wifi_tx_hook;
  
  	ra_sw_nat_to_wifi_fast_tx = PpeWifiFastTxHandler;
  	ra_sw_nat_get_wifi_dev_name = getWifiDevName;

	ra_sw_nat_natv6_fast_handler = PpeNatv6FastHandler;
	vxlan_left_to_right_handle_hook = vxlan_left_to_right_handle;
    
    if(SUPPORT_HIT_IFC_ACTION)
    {
        ra_sw_nat_hit_ifc_fast_handler = PpeHitIfcFastHandler;
    }
    
	hwnat_proc = create_proc_entry("tc3162/rx_wifi_len", 0, NULL);
	hwnat_proc->read_proc = rx_wifi_len_read_proc;
	hwnat_proc->write_proc =  rx_wifi_len_write_proc;
	ra_sw_nat_cds_all_ratelimit_hook = PpeSetMulDropToRatelimitHandler;
	if (TCSUPPORT_VLAN_ACCESS_TRUNK_VAL){
		int i;

		hwnat_proc = create_proc_entry("tc3162/wan_multicast_vlan_mapping", 0, NULL);
		hwnat_proc->read_proc = hwnat_wan_multicast_vlan_mapping_read_proc;
		hwnat_proc->write_proc = hwnat_wan_multicast_vlan_mapping_write_proc;

		for (i = 0; i <= (DP_MCAST_PPP7 - DP_MCAST_NAS0); i++)
		{
			INIT_LIST_HEAD(&hwnat_wan_mcast_vlan_list[i]);
		}
		memset(g_veth_lan_device, 0, sizeof(g_veth_lan_device));
	}

	if(TCSUPPORT_MULTI_SWITCH_EXT_VAL){
		max_eth_itf_num = 11;
	}

	hwnat_proc = create_proc_entry("tc3162/ppe_natv6_rtsp_alg_offload", 0, NULL);
	hwnat_proc->read_proc = ppe_natv6_rstp_alg_offload_read_proc;	
	hwnat_proc->write_proc = ppe_natv6_rstp_alg_offload_write_proc;

	ra_sw_nat_rtspv6_npt_data_handle = ppeNatV6RtspAlgHandler;
    ra_sw_nat_vxlan_fast_handler = PpeVxlanFastHandler;
    
    offload_rps_napi_init();
    if(TCSUPPORT_CT_JOYME4_VAL)
        offload_rps_cpu = 1;
    
    hwnat_proc = create_proc_entry("tc3162/offload_rps_cpu", 0, NULL) ;
    if(hwnat_proc) {
        hwnat_proc->read_proc = offload_rps_cpu_read_proc ;
        hwnat_proc->write_proc = offload_rps_cpu_write_proc ;
    }

    if(isEN7580){
        memset(natv6_wan_rate,0,sizeof(natv6_wan_rate));
        natv6_wan_rate[1].soft_queue_id = -1;
        natv6_wan_rate[0].soft_queue_id = -1;
        hwnat_proc = create_proc_entry("tc3162/natv6_wan_ratelimit", 0, NULL) ;
        if(hwnat_proc) {
            hwnat_proc->read_proc = natv6_wan_ratelimit_read_proc ;
            hwnat_proc->write_proc = natv6_wan_ratelimit_write_proc ;
        }
    }

    vxlan_left_to_right_napi_init();
	        
	return;
}

void ecnt_hwnat_offload_deinit(void)
{

	//Release net_device structure of Dest Port 
	PpeSetDstPort(0);
		
	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL){
		remove_proc_entry("tc3162/softqdma_udf_tcpport", NULL);
	}

	if(TCSUPPORT_DLF_CTRL_VAL)
	{
		/* To delete timer */
		del_timer_sync(&dlf_update_timer);
		DlfHookFuncDeInit();
		PpeDLF_fdb_clear();/*release the space*/
        PpeDLF_nf_clear();
	}   
	
	ppeLocalInDelAll();
	
	ra_sw_nat_local_in_tx = NULL;
	ra_hit_bind_force_to_cpu = NULL;
	ra_sw_nat_mcst_offload = NULL;
	ra_sw_nat_ds_offload = NULL;
	ra_sw_nat_rtsp_offload_restore = NULL;
	ra_sw_nat_to_wifi_fast_tx = NULL;
	ra_sw_nat_get_wifi_dev_name = NULL;
	ra_sw_nat_rtsp_offload_restore = NULL;

	ra_sw_nat_natv6_fast_handler = NULL;
    
    if(SUPPORT_HIT_IFC_ACTION)
    {
        ra_sw_nat_hit_ifc_fast_handler = NULL;
    }
    
	/* l2tp */
	CleanL2tp();

	CleanGre();

    CleanVxlan();
	
	unregister_netdevice_notifier(&ppe_device_notifier);

	remove_proc_entry("tc3162/vpn_enable", NULL);
	
	remove_proc_entry("tc3162/rps_enable", NULL);
	
	ra_sw_nat_hook_wifi_tx = NULL;
	
	remove_proc_entry("tc3162/rx_wifi_len", NULL);
	
	if(TCSUPPORT_DLF_CTRL_VAL)
	{
		/* To remove dlf_enable proc  */
		remove_proc_entry("tc3162/dlf_enable",NULL);
		/* To remove hwnat_meter proc  */
		remove_proc_entry("tc3162/dlf_hwnat_meter", NULL);	
	}

	if (TCSUPPORT_VLAN_ACCESS_TRUNK_VAL){
		remove_proc_entry("tc3162/wan_multicast_vlan_mapping", NULL);
	}

	remove_proc_entry("tc3162/ppe_natv6_rtsp_alg_offload", NULL);

	ra_sw_nat_rtspv6_npt_data_handle = NULL;
    ra_sw_nat_vxlan_fast_handler = NULL;
	ra_sw_nat_cds_all_ratelimit_hook = NULL;
    vxlan_left_to_right_handle_hook = NULL;

	remove_proc_entry("tc3162/offload_rps_cpu", NULL);

    if(isEN7580){
	    remove_proc_entry("tc3162/natv6_wan_ratelimit", NULL); 
        natv6_wan_ratelimit_recycle();
    }
	return;
}

