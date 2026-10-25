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
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>
#include <linux/proc_fs.h>
#include <net/gre.h>
#include <linux/if_tunnel.h>
#include <ecnt_hook/ecnt_hook_ifc.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_asym.h>
#include <ecnt_hook/ecnt_hook_crypto.h>
#include <ecnt_hook/ecnt_hook_bbf247.h>
#include <modules/npu/tunnel_mail.h>
#include <ecnt_hook/ecnt_hook_hsgmii_mac.h>
#include <modules/ra_hwnat/hwnat_npu_tunnel_offload.h>

#include "ra_nat_ety.h"
#include "ra_nat_gen.h"
#include "ra_nat_cust.h"
#include "ra_nat_offload.h"
#include "ra_nat_dev.h"
#include "ra_nat_npu.h"
#include "util.h"
#include "foe_fdb.h"
#include "ppe_api.h"
#include "ppe2_reg.h"
#include "ra_nat_ic_dis.h"

#include <linux/neighbour.h>
#include <net/arp.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
PktParseResult PpeParseResult;

//7581 fe tunne software table,for gre/vxlan/l2tp hw acc
tunnelTable_t TUNNEL_SWTABLE[TUNNEL_TABLE_NUM];

int hwnat_broadcast_en = 0;

int hwnat_fast_slow_down_stream_flag = 0;
int hwnat_fast_slow_up_stream_flag = 0;


#define LAN_PORT_VLAN_ID	CONFIG_RA_HW_NAT_LAN_VLANID
#define WAN_PORT_VLAN_ID	CONFIG_RA_HW_NAT_WAN_VLANID

char *crsnArr[32] = {
    "No such CPU Reason", /* 0x0 */
    "ipv4, tcp udp checksum fail",
    "IPv4(IPv6) TTL(hop limit)",
    "Ipv4(IPv6) has option(extension) header",
    "No such CPU Reason",
    "No such CPU Reason", /* 0x5 */
    "No such CPU Reason",
    "No flow is assigned",
    "IPv4 HNAT doesn't support IPv4 /w fragment",
    "IPv4 HNAPT/DS-Lite doesn't support IPv4 /w fragment",
    "IPv4 HNAPT/DS-Lite can't find TCP/UDP sport/dport", /* 0xA */
    "IPv6 5T-route/6RD can't find TCP/UDP sport/dport",
    "Ingress packet is TCP fin/syn/rst",
    "FOE Un-hit",
    "FOE Hit unbind",
    "FOE Hit unbind & rate reach", /* 0xf */
    "Hit bind PPE TCP FIN entry",
    "Hit bind PPE entry and TTL(hop limit) = 1 and TTL(hot limit) - 1",
    "Hit bind and VLAN replacement violation",
    "Hit bind and keep alive with unicast old-header packet",
    "Hit bind and keep alive with multicast new-header packet", /* 0x14 */
    "Hit bind and keep alive with duplicate old-header packet",
    "FOE Hit bind & force to CPU",
    "(6RD/DS-LITE) inner IP has option/extension fields",
    "FOE Hit bind & duplicate Multicast packet to CPU",
    "FOE Hit bind & force origigal Multicast packet to CPU", /* 0x19 */
    "FOE Hit PreBind",
    "FOE UnHit Class Packet",
    "Hit bind and exceed MTU",
    "Hit bind and exceed LLC MTU 1500 Byte",
    "Packet not go through PPE", /* 0x1e */
    "No CRSN (means hit bind and not to CPU)",
};

static int ppeIsIpv6Nat(unsigned int foe_num,struct FoeEntry * foe_entry);
static unsigned char is_Ipv6_nat66_offload(unsigned int attr);

uint hwnat_fast_bind = 0;
uint hwnat_fast_bind_hash_conflict = 0;

static ppeNptV6Prefix_s_t nptv6_prefix[PPE_NPTV6_PREFIX_MAX_NUM];

/*tunnel info*/
uint tunnel_en = 0;
uint tunnel_id = 0;
uint hop0 = 0;
uint hop1 = 0;
uint hop2 = 0;
uint hop3 = 0;
uint tunnel_mtu=0;
uint ipsec_en = 0;
uint sa_id = 0;

static int hwnat_npu_vxlan_offload_shortcut = 0;

static unsigned char is_Ipv6_nat66_offload(unsigned int attr);

unsigned int recover_info_flag = 0;
int enable_nat66_flag=0;
static int map_tunnel_max_num = 8;
static int current_map_tunnel_num = 0;

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int (*hwnat_is_alive_pkt_hook)(struct sk_buff* skb);
extern int (*hwnat_skb_to_foe_hook)(struct sk_buff* skb);
extern int (*hwnat_set_recover_info_hook)(struct sk_buff* skb,struct sock *sk,int flag);
extern int (*hwnat_is_bind_and_L3_pkt_hook)(struct sk_buff* skb);
extern int (*hwnat_is_from_force_cpu_pkt_hook)(struct sk_buff* skb);
extern int (*xsi_fast_tx_hook)(struct sk_buff *skb);

extern int (*hwnat_skb_cpu_reason_hook)(struct sk_buff* skb);
extern int ecnt_multicast_drop(struct sk_buff *skb);
extern int is_tr471_pkt(struct sk_buff *skb);

extern struct net_device *l2tp_vpn_dev;
extern int macqueue_start_idx;
extern int DebugLevel;
extern unsigned int hwnat_dbg_bindfail_idx;
extern unsigned int hwnat_dgb_drop_idx;
extern struct FoeEntryExt*  foe_ext;

#ifdef CONFIG_TP_IMAGE
extern struct FoeEntryIntf *foe_intf;
#endif /* CONFIG_TP_IMAGE */

extern struct FoeEntry *PpeFoeBase;

extern uint32 dsoffload_presetting;

extern unsigned int ds_need_offload;
extern uint32 pkttype;

extern uint8_t		bind_dir;
extern int udp_bypass_enable;
extern int g_wan_mode;

extern short int ip_proto_chk;
extern unsigned int hwnat_dgb_free_idx;
extern unsigned int hwnat_dbg_parse_dump;

extern unsigned long long unknow_mc_byte_cnt;
extern unsigned int unknow_mc_pkt_cnt;

extern int etherwan_use_qdma;
extern int use_etherwan;

extern struct spinlock hw_nat_lock;

extern int ppeSupportPreBind;

extern unchar fast_path_dir[];

extern int dualband_interrupt_enable;
extern int dualband_interrupt_enable_V2;
extern int dualband_index_tmp;

extern NATV6_WAN_RATE_s natv6_wan_rate[2];
extern unsigned char NATv6_USE_HIGH_PRIORITY;
extern unsigned char natv6_2544_check_flag;

extern unsigned int sysctl_hwnat_off;

extern unsigned char natv6_411_test_flag;
extern unsigned char natv6_411_epon_flag;
extern unsigned int natv6_411_rate_limit;
extern unsigned int natv6_411_timer;
extern unsigned int natv6_411_timeout_time;

extern int check_gemport;

extern unsigned char multicast_en;
extern unsigned int hwnat_dram_idx_base;
extern unsigned int g_mul_high_priority;

extern void FtpCtlPacketHandle(struct sk_buff* skb);
extern void ppeCalcFlowCnt(struct sk_buff * skb, int magic, struct port_info * pinfo);
extern int setPPeTxACNTGRP(struct sk_buff * skb, struct FoeEntry * foe_entry);
extern int hwnat_lan_wan_rx(unsigned int idx,struct sk_buff* sk);
extern void PpeBlack_ip_foe_handle(struct sk_buff *skb, int direction);

extern int ecnt_multicast_hwnat_drop_flow(struct sk_buff* skb);

extern int getPpeMeterBindInfo(struct sk_buff * skb, u_int16_t *meterGrpId);
extern int (*soft_ratelimit_enqueue_hook) (struct sk_buff * skb,unsigned int queue_idx);

extern int judgeinterface(struct sk_buff *skb);
extern int32_t isLanTcpBlackList(struct sk_buff * skb, int magic, struct port_info * pinfo);
extern void ipv6TcpPkttypeMark(struct tcphdr *th);
extern void ipv4TcpPkttypeMark(struct iphdr *iph, struct tcphdr *th, struct sk_buff * skb);

extern void PpeSetInfoBlk2(struct FoeEntry * foe_entry, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag, uint32_t ds_to_qdma);
extern void PpeSetEntryBind(struct sk_buff *skb, struct FoeEntry *foe_entry);

extern int isUpStreamFlow(int magic, struct port_info* pinfo);

extern void PpeSetRxForceRingIdx(struct FoeEntry * foe_entry, uint8_t ring_index);

extern struct net_device	*DstPort[MAX_IF_NUM];

extern int ppe_trans_tpid_to_vpm(uint16_t eth_type);

extern int (*xpon_igmp_learn_flow_hook)(struct sk_buff* skb);

extern void wdog_kick_api(int source);

extern void softQdmaSetForcePort(struct FoeEntry * foe_entry, struct port_info * pinfo, u_int16_t meterGrpId, u_int16_t acntGrpIndex);
extern int PpeHitBindForceToCpuHandler(struct sk_buff *skb, struct FoeEntry *foe_entry);
extern int PpeHitBindMULToCpuHanler(struct sk_buff *skb, struct FoeEntry *foe_entry);
extern uint32_t PpeExtIfRxFastHandler(struct sk_buff * skb);
extern int PpeDLFRxHandler(struct sk_buff * skb);
extern uint32_t PpeExtIfRxHandler(struct sk_buff * skb);
extern uint32_t PpeExtIfPingPongHandler(struct sk_buff * skb);

extern int ecnt_ppe_set_port_info_magic_ge_hook(struct FoeEntry * foe_entry, struct port_info * pinfo,char type,char fast);

extern int ecnt_ppe_rx_handler_hook(struct sk_buff* skb);
extern int ecnt_ppe_tx_hanlder_start_hook(struct FoeEntry *foe_entry,struct sk_buff* skb,struct port_info * pinfo, int magic);
extern int ecnt_ppe_tx_hanlder_end_hook(struct FoeEntry *foe_entry,struct sk_buff* skb,struct port_info * pinfo, int magic);

extern int  ecnt_ppe_get_meter_and_account_id_hook(struct FoeEntry * foe_entry,struct sk_buff * skb,
	struct port_info * pinfo, int magic,unsigned short *p_meter_id,unsigned short *p_account_id);
extern int ecnt_ppe_set_force_port_info_start_hook(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,u_int16_t meterGrpId,u_int16_t acntGrpIndex);
extern int ecnt_ppe_set_force_port_info_end_hook(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,unsigned short meterGrpId,unsigned short acntGrpIndex);

extern int ecnt_ppe_set_entry_bind_hook(struct sk_buff *skb, struct FoeEntry *foe_entry, struct bf_info_blk1* p_blk1);

extern int ecnt_multicast_hwnat_learn_flow(struct sk_buff* skb);

extern void (*wlan_to_lan_hook)(struct sk_buff *skb, u8 local);
extern int ppeCheckFastRuleBind(struct sk_buff* skb,struct FoeEntry *foe_entry);
extern int xpon_is_multicast_entry(struct FoeEntry *foe_entry);

#if defined  (CONFIG_RA_HW_NAT_VPN_PASSTHROUGH)
extern struct net_device *vpn_decrypt_src_dev[DP_CRYPTO_D_MAX - DP_CRYPTO_D_0];
#endif

extern int is_l2lu_pkt_exist;
extern int PpeL2luKeyUpdateBindTime(struct sk_buff * skb);
extern int PpeL2luKeySetStatusBind(int PpeIndex);

extern struct net_device* vxlan_getlink(int vni);
extern int ecnt_hwnat_npu_add_session_to_vxlan_hdr_list(unsigned char* if_name,unsigned int foe_num);

extern int hwnat_delete_foe_entry(int index);

extern int ipv6_skip_exthdr(const struct sk_buff *skb, int start, u8 *nexthdrp,__be16 *frag_offp);

extern struct neigh_table arp_tbl;
extern int (*mulif_get_dev_by_source_mac_hook)(struct sk_buff *skb, struct net_device **dev);

extern int find_and_update_wan_ip6_table(uint32_t* ip6_addr);
extern unsigned int wan_conn_type;
extern char wan_itf_name[];

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int32_t GetPppoeSid(struct sk_buff * skb,uint16_t * sid, uint16_t * ppp_tag)
{
	struct pppoe_hdr *peh = NULL;

	peh = (struct pppoe_hdr *)(skb->data);

	if (DebugLevel >= 6) {
		NAT_PRINT("\n==============\n");
		NAT_PRINT(" Ver=%d\n", peh->ver);
		NAT_PRINT(" Type=%d\n", peh->type);
		NAT_PRINT(" Code=%d\n", peh->code);
		NAT_PRINT(" sid=%x\n", ntohs(peh->sid));
		NAT_PRINT(" Len=%d\n", ntohs(peh->length));
		NAT_PRINT(" tag_type=%x\n", ntohs(peh->tag[0].tag_type));
		NAT_PRINT(" tag_len=%d\n", ntohs(peh->tag[0].tag_len));
		NAT_PRINT("=================\n");
	}

	*ppp_tag = peh->tag[0].tag_type;
	
	if (peh->ver != 1 || peh->type != 1){
		return 1;
	}

	*sid = peh->sid;
	return 0;
}

int ppe_get_vlan_tag_num(struct sk_buff* skb)
{
	uint16* tmp = NULL;
	int vlan_tag_num = 0; 

	tmp = (uint16 *)(skb->data);
	if(NULL == tmp)
		return 0;
	tmp += 6;

	if(skb->ecnt_sk_buff.stag_inserted)
	{
		vlan_tag_num++;/* special tag*/
		tmp+=2;
	}

	while(*tmp == htons(0x8100) || *tmp == htons(0x88a8) || *tmp == htons(0x9100) || *tmp == htons(0x884c))
	{
		vlan_tag_num++;
		tmp+=2;
	}

	return vlan_tag_num;
}

int ppe_get_packet_type(struct sk_buff* skb, int *offlen)
{
	int vlan_tag_num = ppe_get_vlan_tag_num(skb);
	unsigned char* buff = skb->data;
	uint32 eth_type = 0;
	uint16 pppoe_proto = 0;
	unsigned char* buff_off = NULL;

	if(NULL == buff)
		return 0;
	buff_off = buff;
	buff += (12+4*vlan_tag_num);
	eth_type = *(uint16*)buff;

	buff += 2;/*skip ether type*/

	if (eth_type == htons(ETH_P_PPP_SES))
	{
		buff+= sizeof(struct pppoe_hdr);
		pppoe_proto = *(uint16*)buff;

		if (pppoe_proto == htons(PPP_IP))
			eth_type = htons(ETH_P_IP);
		else if (pppoe_proto == htons(PPP_IPV6))
			eth_type = htons(ETH_P_IPV6);

		buff += 2;
	}

	*offlen = buff - buff_off;

	return htons(eth_type);
}

#ifdef CONFIG_TP_IMAGE
/* only ppp session return */
void*  ppe_get_ppp_network_header(struct sk_buff* skb)
{
	unsigned char* buff = skb->data;
	int vlan_tag_num = ppe_get_vlan_tag_num(skb);
	uint32 eth_type = 0;

	buff += (12+4*vlan_tag_num);
	eth_type = *(uint16*)buff;

	buff += 2;/*skip ether type*/

	if (eth_type == htons(ETH_P_PPP_SES))
	{
		buff += sizeof(struct pppoe_hdr);
		buff += 2;/*skip ppp header*/
		return buff;
	}	
	
	return NULL;
}

static unsigned int tp_get_dev_ifindex(struct net_device *dev, struct FoeEntry *entry)
{
	struct net_device *vlan = NULL;
	char ifname[IFNAMSIZ];
	unsigned int ifindex = 0;

	if (NULL == dev || NULL == entry)
	{
		return 0;
	}

	ifindex = dev->ifindex;

	if (entry->ipv4_hnapt.vlan1)
	{
		memset(ifname, 0, sizeof(ifname));
		snprintf(ifname, sizeof(ifname), "%s.%d", dev->name, entry->ipv4_hnapt.vlan1);
		ifname[sizeof(ifname) - 1] = '\0';

		if (NULL != (vlan = dev_get_by_name(&init_net, ifname)))
		{
			ifindex = vlan->ifindex;
			dev_put(vlan);
		}
	}

	return ifindex;
}

static int tp_update_hnat_entry(struct net_device *dev, struct FoeEntry *ul_entry, uint32_t ul)
{
	const char *bhIntfs[] = {"ra5", "rai5", "rax5", "raix5"};
	struct FoePriKey key;
	enum HashSelect hashSel;
	uint32_t dl = 0;
	uint32_t act_sp = 0;
	int index = 0;
	int update = 0;
	int ret = 0;

	if (NULL == dev || NULL == ul_entry ||
		NULL == foe_intf || ul >= FOE_4TB_SIZ_TOTAL)
	{
		return -1;
	}

	act_sp = foe_intf[ul].act_sp;
	foe_intf[ul].act_sp = tp_get_dev_ifindex(dev, ul_entry);

	for (index = 0; index < ARRAY_SIZE(bhIntfs); ++index)
	{
		if (0 == strncmp(dev->name, bhIntfs[index], strlen(bhIntfs[index])))
		{
			if (act_sp && act_sp != foe_intf[ul].act_sp)
			{
				update = 1;
				break;
			}
		}
	}

	if (update)
	{
		memset(&key, 0, sizeof(struct FoePriKey));

		key.ipv4_hnapt.sip = ul_entry->ipv4_hnapt.new_dip;
		key.ipv4_hnapt.dip = ul_entry->ipv4_hnapt.new_sip;
		key.ipv4_hnapt.sport = ul_entry->ipv4_hnapt.new_dport;
		key.ipv4_hnapt.dport = ul_entry->ipv4_hnapt.new_sport;
		key.ipv4_hnapt.is_udp = 0;
		key.pkt_type = ul_entry->bfib1.pkt_type;
		key.sport_mask = 0xFFFF;
		key.ifc_idx = 0xFF;
		key.ppe2 = 0;

		if (isSramFlowTableEnable())
		{
			hashSel = HashMode_Sram0;
		}
		else if (isSramHash1ModeEnable())
		{
			hashSel = HashMode_Sram1;
		}
		else if (isDramFlowTableEnable())
		{
			hashSel = HashMode_Dram;
		}
		else if (isDramHash1ModeEnable())
		{
			hashSel = HashMode_Dram1;
		}
		else
		{
			hashSel = HashMode_Sram0;
		}

		/* this entry may use ppe1 or ppe2
		 * hw_nat -c [foe_idx] to confirm which ppe this entry is used
		 */
		if ((ret = FoeHashFun(&key, BIND, hashSel)) < 0)
		{
			key.ppe2 = 1;

			if ((ret = FoeHashFun(&key, BIND, hashSel)) < 0)
			{
				/* can't find revert entry */
				return 0;
			}
		}

		dl = (uint32_t)ret;

		if (dl < FOE_4TB_SIZ_TOTAL)
		{
			if (DebugLevel)
			{
				printk("[%s][%d]: sp change (%d)->(%d), del foe entry[%d][%d]\n",
						__FUNCTION__, __LINE__, act_sp, foe_intf[ul].act_sp, ul, dl);
			}

			/* follow re, clean ul & dl */
			hwnat_delete_foe_entry(ul);
			hwnat_delete_foe_entry(dl);
		}
	}

	return 0;
}
#endif /*CONFIG_TP_IMAGE*/

void*  ppe_get_network_header(struct sk_buff* skb)
{
	unsigned char* buff = skb->data;
	int vlan_tag_num = ppe_get_vlan_tag_num(skb);
	uint32 eth_type = 0;

	buff += (12+4*vlan_tag_num);
	eth_type = *(uint16*)buff;

	buff += 2;/*skip ether type*/

	if (eth_type == htons(ETH_P_PPP_SES))
	{
		buff += sizeof(struct pppoe_hdr);
		buff += 2;/*skip ppp header*/
	}	
	
	return buff;
}


int ppe_get_ip_type(struct sk_buff* skb)
{	
	int offlen = 0;
	unsigned int eth_type = ppe_get_packet_type(skb,&offlen);
	unsigned char* buff;
	struct iphdr*  ih;
	struct ipv6hdr* i6h;

	if (eth_type==0x0800)
	{
		ih = (struct iphdr* ) ppe_get_network_header(skb);
		return ih->protocol;
	}

	if (eth_type==0x86dd)
	{
		buff = ppe_get_network_header(skb);
		i6h = (struct ipv6hdr*)buff;
		return (i6h->nexthdr==0) ? buff[40]:i6h->nexthdr;
	}

	return 0;
}

int ppe_get_src_and_dst_addr(struct sk_buff* skb,unsigned int* proto,unsigned char*  src,unsigned char* dst)
{
	int offlen = 0;
	unsigned int eth_type = ppe_get_packet_type(skb,&offlen);
	struct iphdr*  ih;
	struct ipv6hdr* i6h;

	if (eth_type==PACKET_IPV4)
	{
		ih = (struct iphdr* ) ppe_get_network_header(skb);
		*proto = PPE_PROTO_IPV4;
		memcpy(src,(unsigned char*)&ih->saddr,4);
		memcpy(dst,(unsigned char*)&ih->daddr,4);
	}

	if (eth_type==PACKET_IPV6)
	{
		i6h = (struct ipv6hdr*) ppe_get_network_header(skb);
		*proto = PPE_PROTO_IPV6;
		memcpy(src,i6h->saddr.s6_addr,16);
		memcpy(dst, i6h->daddr.s6_addr,16);
	}

	return 0;
}

int ppe_get_ipv4_5t(struct sk_buff* skb,uint8_t* proto,uint32_t* sip,uint32_t* dip,uint16_t* sport,uint16_t* dport)
{
	int offlen = 0;
	unsigned int eth_type = ppe_get_packet_type(skb,&offlen);	
	struct iphdr*  ih;
	struct udphdr* uh;
	struct tcphdr* th;

	if(PACKET_IPV4 != eth_type)
		return -1;

	ih = (struct iphdr* ) ppe_get_network_header(skb);
	*proto = ih->protocol;
	*sip = htonl(ih->saddr);
	*dip = htonl(ih->daddr);

	if(ih->protocol == IPPROTO_UDP)
	{
		uh = (struct udphdr*)((unsigned char*)ih + ih->ihl*4);
		*sport = htons(uh->source);
		*dport = htons(uh->dest);
		return 0;
	}
	else if(ih->protocol == IPPROTO_TCP)
	{
		th = (struct tcphdr*)((unsigned char*)ih + ih->ihl*4);
		*sport = htonl(th->source);
		*dport = htonl(th->dest);
		return 0;
	}

	return -1;
}

/*Using return value to judge
0:is not multicast entry
1:is multicast entry and is for ipv4
2:is multicast entry and is for ipv6
*/
int ppe_is_multicast_entry(struct FoeEntry *foe_entry)
{
	unsigned char dst_mac[ETH_ALEN] = {0};
	
	if (foe_entry->bfib1.pkt_type < 2 )
	{
		if(foe_entry->ipv4_hnapt.dip > 0xe0000000)
			return 1;
	}
	else if (foe_entry->bfib1.pkt_type == L2_BRIDGE)
	{
		FoeGetEntryMac(dst_mac, foe_entry->l2_bridge.in_dmac_hi, foe_entry->l2_bridge.in_dmac_lo);
		if(dst_mac[0] == 0x01 && dst_mac[1] == 0x00 && dst_mac[2] == 0x5E) {
			return 1;
		} else if(dst_mac[0] == 0x33 && dst_mac[1] == 0x33) {
			return 1;
		} else {
			return 0;
		}
	}
	else if((4 == foe_entry->bfib1.pkt_type)||(5 == foe_entry->bfib1.pkt_type))
	{
		if (foe_entry->ipv6_5t_route.ipv6_dip0 > 0xff000000)
			return 2;
	}

	return 0;
}

static int ppe_is_multicast_data_pkt(struct sk_buff* skb)
{
	int type = ppe_get_ip_type(skb);

	if (type==0x11)
		return 1;

	return 0;
}

int ppe_is_multicast_data_pack(struct sk_buff* skb)
{
	unsigned char *dest = NULL; 
	
	if(NULL == skb || NULL == skb->dev)
		return false;
	dest = eth_hdr(skb)->h_dest;
	
	if (!is_multicast_ether_addr(dest))
		return false;

	return  ppe_is_multicast_data_pkt(skb);
}

int ppe_is_rtspv6_response(struct sk_buff* skb)
{
	unsigned char *cp = NULL;	
	unsigned int offset = 0;
	unsigned char nexthdr = 0;
	unsigned short frag_off = 0;
	struct ipv6hdr* ip6h = NULL;

	ip6h = (struct ipv6hdr*)ppe_get_network_header(skb);

	if(0 == ip6h->nexthdr)
	{
		nexthdr = ip6h->nexthdr;
		offset = ipv6_skip_exthdr(skb, sizeof(struct ipv6hdr), &nexthdr, &frag_off);
		cp = (unsigned char*)ip6h+offset+sizeof(struct tcphdr);
	}
	else
		cp = (unsigned char*)ip6h+sizeof(struct ipv6hdr)+sizeof(struct tcphdr);
	
	if(cp&&('R'==*cp)&&('T'==*(cp+1))&&('S'==*(cp+2))&&('P'==*(cp+3))&&('/'==*(cp+4)))
	{
		return 1;
	}
	if(cp&&('v'==*cp)&&('='==*(cp+1)))
	{
		return 1;
	}
	
	return 0;
}

static void dump_skb(struct sk_buff *skb) 
{
	char tmp[80];
	char *p = NULL;
	char *t = tmp;
	int i, n = 0;

	if(skb == NULL)
		return;
	
	p = skb->data;

	printk("ERR skb=%08lx data=%08lx len=%d, data_len=%d, hdr_len=%d\n", (unsigned long)skb, (unsigned long)skb->data, skb->len,skb->data_len, skb->hdr_len);
	for (i = 0; i < skb->len + 4; i++) {
		t += sprintf(t, "%02x ", *p++ & 0xff);
		if ((i & 0x0f) == 0x0f) {
			printk("%04x: %s\n", n, tmp);
			n += 16;
			t = tmp;
		}
	}
	if (i & 0x0f)
		printk("%04x: %s\n", n, tmp);
	return;
}

int PpeParseTunnelSwInfo(int index)
{
	
	tunnelTable_t tunnel_table;
	
	if(index<0 || index >= TUNNEL_TABLE_NUM){
		return 0;
	}

	memcpy(&tunnel_table,&(TUNNEL_SWTABLE[index]) , sizeof(tunnelTable_t));
	printk("=============<TUNNEL TABLE %d>=============\n", index);

	printk("config format:is_layer2_tunnel(1)|use_Inner_TTL(1)|use_Inner_DSCP(1)|GRE_KEY_FLAG(1) |L2TP_OVER_IP(1)|L2TPV3(1)|RSV(6)| PPPOE_EN(1)|vlan_en(1)|VPM(2) |TUN0(4)|TUN1(4)|TUN2(4)|TUN3(4) \n");

	printk("\n%08x ", tunnel_table.config.word);

	printk("\n%08x \t",	tunnel_table.mac_header0.word);
	printk("/		 DMAC[47:16]		  /");

	printk("\n%08x \t",	tunnel_table.mac_header1.word);
	printk("/	DMAC[15:0]|	DMAC[47:32]	  /");

	printk("\n%08x \t",	tunnel_table.mac_header2.word);
	printk("/		 SMAC[31:0]		  	  /");

	printk("\n%08x \t",	tunnel_table.mac_header3.word);
	printk("/	VLAN ID	  |	PPPOE ID	  /");


	printk("\n%08x \t",	tunnel_table.ip_header0.word);
	printk("/TTL(8)|DSCP(8)|RSV(16)	for ipv4 \t/");
	printk("/HOP_LIMIT(8)|TRAFFIC_CLASS(8)|RSV(16)	for ipv6 \t/");

	printk("\n%08x \t",	tunnel_table.ip_header1.word);
	printk("/	Identification(16)|	ipv4 pre CKS(16) for ipv4 \t /");
	printk("/	flow label(24)|RSV(5)|SIP_ID(3) for ipv6  /");

	printk("\n%08x \t",	tunnel_table.ip_header2.word);
	printk("/		 SIP for ipv4	  	\t  /");
	printk("/		 DIP[127:96] for ipv6\t  /");

	printk("\n%08x \t",	tunnel_table.ip_header3.word);
	printk("/		 DIP for ipv4	  	\t  /");
	printk("/		 DIP[95:64] for ipv6\t  /");


	printk("\n%08x \t",	tunnel_table.ip_header4.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 DIP[63:32] for ipv6\t  /");

	printk("\n%08x \t",	tunnel_table.ip_header5.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 DIP[31:0] for ipv6\t  /");

	
	printk("\n%08x \t",	tunnel_table.ip_header6.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 RSV for ipv6\t  /");

	printk("\n%08x \t",	tunnel_table.ip_header7.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 RSV for ipv6\t  		/");


	printk("\n%08x \t",	tunnel_table.l2tp_header.word);
	printk("/		TUNNEL id + session id for L2TP Header	  	\t  /");

	printk("\n%08x \t",	tunnel_table.gre_header.word);
	printk("/		Virtual subnet id(24)|Flow id(8) for GRE Header	  	\t  /");


	printk("\n%08x \t",	tunnel_table.vxlan_header.word);
	printk("/		VNI(24)|RSV(8)	  for VXLAN Header	\t  /");

	printk("\n%08x \t",	tunnel_table.udp_header.word);
	printk("/		udp src(16)|udp dst(16))	 for USP Header 	\t  /");
	
	printk("\n");

	return 0;
}

int PpeSetTunnel_table_ToHW(int index){

	tunnelTable_t tunnel_table ;
	
	if(index<0 || index >= TUNNEL_TABLE_NUM){
		return 0;
	}

	memcpy(&tunnel_table,&(TUNNEL_SWTABLE[index]) , sizeof(tunnelTable_t));

	FE_API_SET_TUNNEL_CFG(index, 4, 0,0,0,tunnel_table.config.word);
	FE_API_SET_TUNNEL_CFG(index, 0, tunnel_table.mac_header0.word,tunnel_table.mac_header1.word,tunnel_table.mac_header2.word,tunnel_table.mac_header3.word);
	FE_API_SET_TUNNEL_CFG(index, 1,	tunnel_table.ip_header0.word,tunnel_table.ip_header1.word,tunnel_table.ip_header2.word,tunnel_table.ip_header3.word);
	FE_API_SET_TUNNEL_CFG(index, 2, tunnel_table.ip_header4.word,tunnel_table.ip_header5.word,tunnel_table.ip_header6.word,tunnel_table.ip_header7.word);
	FE_API_SET_TUNNEL_CFG(index, 3, tunnel_table.l2tp_header.word,tunnel_table.gre_header.word,tunnel_table.vxlan_header.word,tunnel_table.udp_header.word);


	return -1;
}

int PpeClearTunnel_table_ToHW(int index){

	
	if(index<0 || index >= TUNNEL_TABLE_NUM){
		return 0;
	}

	FE_API_SET_TUNNEL_CFG(index, 4, 0,0,0,0);
	FE_API_SET_TUNNEL_CFG(index, 0, 0,0,0,0);
	FE_API_SET_TUNNEL_CFG(index, 1,	0,0,0,0);
	FE_API_SET_TUNNEL_CFG(index, 2, 0,0,0,0);
	FE_API_SET_TUNNEL_CFG(index, 3, 0,0,0,0);

	return -1;
}




static void ppeTunnelSessionListFree(void* ptr)
{
	if (ptr)
	{
		kfree(ptr);
		ptr = NULL;
	} 
}


static  void* ppeTunnelSessionListAlloc(int size)
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


int ppe_update_tunnel_session_list(void){


    PpeHw_Tunnel_Session_t *entry = NULL;
    PpeHw_Tunnel_Session_t *entry_tmp = NULL;

	int i = 0;

	for(i = TUNNEL_DOWNSTART; i < TUNNEL_TABLE_NUM ; i ++){

		if((!TUNNEL_SWTABLE[i].is_table_add_valid) && (!TUNNEL_SWTABLE[i].is_table_rm_valid))
			continue;

		list_for_each_entry_safe(entry,entry_tmp, &(TUNNEL_SWTABLE[i].session_list), list)
		{
			if(!hwnat_check_entry_is_bind(entry->foe_num))
			{
				list_del(&entry->list);
				ppeTunnelSessionListFree(entry);
			}

		}


		if(list_empty(&(TUNNEL_SWTABLE[i].session_list)))
		{
		
			TUNNEL_SWTABLE[i].is_table_add_valid = 0;
			if(TUNNEL_SWTABLE[i].map_flag){
				TUNNEL_SWTABLE[i].map_flag = 0;
				current_map_tunnel_num--;
			}
			PpeClearTunnel_table_ToHW(i);
			memset(&(TUNNEL_SWTABLE[i]),0,sizeof(tunnelTable_t));
			INIT_LIST_HEAD(&(TUNNEL_SWTABLE[i].session_list));
		}

	}

	 return 0;    
}

int ppe_add_session_to_tunnel_list(unsigned int index,unsigned int foe_num ){
	PpeHw_Tunnel_Session_t *entry_tmp = NULL;

	if(index >= TUNNEL_TABLE_NUM)
		return -1;

	list_for_each_entry(entry_tmp, &(TUNNEL_SWTABLE[index].session_list), list)	 
	{
		if(entry_tmp->foe_num == foe_num)
		{
			return -1;
		}
	}

	entry_tmp = (PpeHw_Tunnel_Session_t*)ppeTunnelSessionListAlloc(sizeof(PpeHw_Tunnel_Session_t));
    if(entry_tmp)
    {
        entry_tmp->foe_num = foe_num;
        list_add_tail(&entry_tmp->list,&(TUNNEL_SWTABLE[index].session_list));
    }

	return 0;

}


static int ppeCompareTunnelTableIp(int index)
{
    int saddr_id = 0;

    if((PpeParseResult.pkt_type == IPV6_5T_ROUTE)||(PpeParseResult.pkt_type == IPV6_3T_ROUTE))
    {
        saddr_id = find_and_update_wan_ip6_table(PpeParseResult.ip6h.saddr.s6_addr32);
        if((saddr_id == -1) ||(saddr_id != TUNNEL_SWTABLE[index].ip_header1.ipv6_raw.sip_id))
            return 0;

        if((ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]) != TUNNEL_SWTABLE[index].ip_header2.ipv6_dip0)||\
            (ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]) != TUNNEL_SWTABLE[index].ip_header3.ipv6_dip1)||\
            (ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]) != TUNNEL_SWTABLE[index].ip_header4.ipv6_dip2)||\
            (ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]) != TUNNEL_SWTABLE[index].ip_header5.ipv6_dip3))
            return 0;
    }
    else
    {
        if((ntohl(PpeParseResult.iph.saddr) != TUNNEL_SWTABLE[index].ip_header2.ipv4_sip)||\
            (ntohl(PpeParseResult.iph.daddr) != TUNNEL_SWTABLE[index].ip_header3.ipv4_dip))
            return 0;
    }

    return 1;
}

/* 0=upstream, 1=downstream,*/
/* allocate 0 for l2tp down stream, 1 for vxlan down stream,2 for L2GRE down stream, 3 for L3GRE down stream*/
/* allocate 3~63 for up stream for different stream*/
int PpeGetTunnel_table_index(int dir, int tunnel_type){

	int i = 0;
	unsigned int vni = 0;
	unsigned short srcport = 0,dstport = 0;
	unsigned int tunnel = 0, session = 0,tunnel_session = 0;

	ppe_update_tunnel_session_list();
	if(dir == 1){
		if(tunnel_type == L2GRE_TUNNEL)
			return L2GRE_DOWN_TBL_IDX;
		else if(tunnel_type == VXLAN_TUNNEL)
		{
		    if((PpeParseResult.pkt_type == IPV6_5T_ROUTE)||(PpeParseResult.pkt_type == IPV6_3T_ROUTE))
                return VXLAN_V6_DOWN_TBL_IDX;
            else
		        return VXLAN_DOWN_TBL_IDX;
        }
		else if(tunnel_type == L2TP_TUNNEL)
			return L2TP_DOWN_TBL_IDX;
		else if(tunnel_type == L3GRE_TUNNEL)
			return L3GRE_DOWN_TBL_IDX;
		else
			return -1;
	
	}else{
		vni =(ntohl(PpeParseResult.vxlanh.vx_vni) >> 8 )& 0x00FFFFFF;
		srcport = ntohs(PpeParseResult.uh.source);
		dstport = ntohs(PpeParseResult.uh.dest);

		tunnel =  ntohs(PpeParseResult.l2tp_tunnelid);
		session = ntohs(PpeParseResult.l2tp_sessionid);
		tunnel_session = (tunnel<<16) + session;


		if(tunnel_type == L2GRE_TUNNEL || tunnel_type == L3GRE_TUNNEL){
			/*L2gre only check sip& dip*/
			for(i = TUNNEL_DOWNSTART;i < TUNNEL_TABLE_NUM; i ++ ){
				if (ppeCompareTunnelTableIp(i))
					break;	

			}


		}else if(tunnel_type == VXLAN_TUNNEL){
			/*vxlan  check sip& dip src/dst port and vni*/
			for(i = TUNNEL_DOWNSTART;i < TUNNEL_TABLE_NUM; i ++ ){
				if ( ppeCompareTunnelTableIp(i)
					 && (srcport == TUNNEL_SWTABLE[i].udp_header.raw.srcport)&&(dstport == TUNNEL_SWTABLE[i].udp_header.raw.dstport)
					 && (vni == TUNNEL_SWTABLE[i].vxlan_header.raw.vni))
					break;	
			}



		}else if(tunnel_type == L2TP_TUNNEL){
			/*vxlan  check sip& dip src/dst port and tunnel_session*/
			for(i = TUNNEL_DOWNSTART;i < TUNNEL_TABLE_NUM; i ++ ){
				if ( ppeCompareTunnelTableIp(i)
					 && (srcport == TUNNEL_SWTABLE[i].udp_header.raw.srcport)&&(dstport == TUNNEL_SWTABLE[i].udp_header.raw.dstport)
					 && (tunnel_session == TUNNEL_SWTABLE[i].l2tp_header.tunnel_session))
					break;	
			}	
		}
		else if(tunnel_type == DSLITE_TUNNEL || tunnel_type == T6RD_TUNNEL){
			for(i = TUNNEL_DOWNSTART;i < TUNNEL_TABLE_NUM; i ++ ){
				if ( ppeCompareTunnelTableIp(i))
					break;	
			}
		}
		
		else 
			return -1;


		if( i < TUNNEL_TABLE_NUM)
			return i;

		else{
			for(i = TUNNEL_DOWNSTART;i < TUNNEL_TABLE_NUM; i ++ ){
				if (TUNNEL_SWTABLE[i].is_table_add_valid == 0)
					break;	

			}
			if( i < TUNNEL_TABLE_NUM)
				return i;

		}

	}
	return -1;
}


unsigned short PpeCalipv6_prechk(uint16_t* saddr16, uint16_t* daddr16){

	unsigned int sum = 0, sum1 = 0;
    int i = 0;

    for(i=0;i<8;i++)
	    sum +=htons(saddr16[i]);

	while(sum>>16){
		sum = (sum & 0xffff) + (sum >> 16);
	}

    for(i=0;i<8;i++)
	    sum1+=htons(daddr16[i]);
    
	while(sum1>>16){
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	}

	sum = sum+sum1;

	while(sum>>16){
		sum = (sum & 0xffff) + (sum >> 16);
	}

	return sum;
}

unsigned short PpeCalipv4_prechk(uint32_t ipv4_sip, uint32_t ipv4_dip){

	unsigned int sum = 0, sum1 = 0;

	sum +=ipv4_sip;

	while(sum>>16){
		sum = (sum & 0xffff) + (sum >> 16);
	}

	sum1+=ipv4_dip;
	while(sum1>>16){
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	}

	sum = sum+sum1;

	while(sum>>16){
		sum = (sum & 0xffff) + (sum >> 16);
	}

	return sum;
}

//
int PpeUpdateGRETunnel_swtable(int index,int dir, int is_l2_tunnel){
	char vpm = 0, vlan_en = 0, pppoe_en = 0;
	unsigned short prechk = 0;
	//parse gre tunnel config
	if(index<0 || index >= TUNNEL_TABLE_NUM){
		//HWNAT_BINDFAIL_DPRINT("index not in range \n");
		return 0;
	}

	if(dir == 0 ){
	//tunnelTable_t TUNNEL_SWTABLE[TUNNEL_TABLE_NUM] ;
		TUNNEL_SWTABLE[index].config.raw.is_l2_tunnel = is_l2_tunnel;
		TUNNEL_SWTABLE[index].config.raw.use_inner_ttl = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_DSCP = 0;
		TUNNEL_SWTABLE[index].config.raw.gre_key_flag = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_over_ip = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_v3 = 0;
		TUNNEL_SWTABLE[index].config.raw.ipv4_id = 1;
		TUNNEL_SWTABLE[index].config.raw.udp_cks = 0;
		if(is_l2_tunnel){
			TUNNEL_SWTABLE[index].config.raw.tun0 = 1;
			TUNNEL_SWTABLE[index].config.raw.tun1 = 2;
			TUNNEL_SWTABLE[index].config.raw.tun2 = 6;
			TUNNEL_SWTABLE[index].config.raw.tun3 = 0;
		}
		else{
			TUNNEL_SWTABLE[index].config.raw.tun0 = 2;
			TUNNEL_SWTABLE[index].config.raw.tun1 = 6;
			TUNNEL_SWTABLE[index].config.raw.tun2 = 0;
			TUNNEL_SWTABLE[index].config.raw.tun3 = 0;
		}

		vpm = ppe_trans_tpid_to_vpm(PpeParseResult.vlan_tag);
		TUNNEL_SWTABLE[index].config.raw.vpm = vpm;
		
		if (PpeParseResult.vlan_layer == 0)
       		vlan_en = 0;
		else
			vlan_en = 1;
		TUNNEL_SWTABLE[index].config.raw.vlan_en = vlan_en;

		if(PpeParseResult.eth_type == htons(ETH_P_PPP_SES))
			pppoe_en = 1;
		TUNNEL_SWTABLE[index].config.raw.pppoe_en = pppoe_en;

		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[3] = PpeParseResult.dmac[0];
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[2] = PpeParseResult.dmac[1];
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[1] = PpeParseResult.dmac[2];
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[0] = PpeParseResult.dmac[3];
		
		TUNNEL_SWTABLE[index].mac_header1.dmac_lo[1] = PpeParseResult.dmac[4];
		TUNNEL_SWTABLE[index].mac_header1.dmac_lo[0] = PpeParseResult.dmac[5];

		TUNNEL_SWTABLE[index].mac_header1.smac_hi[1] = PpeParseResult.smac[0];
		TUNNEL_SWTABLE[index].mac_header1.smac_hi[0] = PpeParseResult.smac[1];



		TUNNEL_SWTABLE[index].mac_header2.smac_lo[3] = PpeParseResult.smac[2];
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[2] = PpeParseResult.smac[3];
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[1] = PpeParseResult.smac[4];
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[0] = PpeParseResult.smac[5];
		
		TUNNEL_SWTABLE[index].mac_header3.vlan_id = ntohs(PpeParseResult.vlan1);
		TUNNEL_SWTABLE[index].mac_header3.pppoe_id= ntohs(PpeParseResult.pppoe_sid);

		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.ttl = PpeParseResult.iph.ttl;
		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.dscp = PpeParseResult.iph.tos;

		prechk = PpeCalipv4_prechk(ntohl(PpeParseResult.iph.saddr),ntohl(PpeParseResult.iph.daddr));
		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.pre_chk = prechk;

		TUNNEL_SWTABLE[index].ip_header1.ipv4_raw.iden = ntohs(PpeParseResult.iph.id);
		
		TUNNEL_SWTABLE[index].ip_header2.ipv4_sip = ntohl(PpeParseResult.iph.saddr);
		TUNNEL_SWTABLE[index].ip_header3.ipv4_dip = ntohl(PpeParseResult.iph.daddr);


		
	}else{
		//down stream set config 
		TUNNEL_SWTABLE[index].config.raw.is_l2_tunnel = is_l2_tunnel;
		TUNNEL_SWTABLE[index].config.raw.use_inner_ttl = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_DSCP = 0;
		TUNNEL_SWTABLE[index].config.raw.gre_key_flag = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_over_ip = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_v3 = 0;
		TUNNEL_SWTABLE[index].config.raw.ipv4_id = 0;
		TUNNEL_SWTABLE[index].config.raw.udp_cks = 0;
		if(is_l2_tunnel){
			TUNNEL_SWTABLE[index].config.raw.tun0 = 1;
			TUNNEL_SWTABLE[index].config.raw.tun1 = 2;
			TUNNEL_SWTABLE[index].config.raw.tun2 = 6;
			TUNNEL_SWTABLE[index].config.raw.tun3 = 0;
		}
		else{
			TUNNEL_SWTABLE[index].config.raw.tun0 = 2;
			TUNNEL_SWTABLE[index].config.raw.tun1 = 6;
			TUNNEL_SWTABLE[index].config.raw.tun2 = 0;
			TUNNEL_SWTABLE[index].config.raw.tun3 = 0;
		}

		TUNNEL_SWTABLE[index].ip_header2.ipv4_sip = ntohl(PpeParseResult.iph.daddr);
		TUNNEL_SWTABLE[index].ip_header3.ipv4_dip = ntohl(PpeParseResult.iph.saddr);

	}

	if (hwnat_dbg_parse_dump >= 2) 
		PpeParseTunnelSwInfo(index);

	return 0 ;
}



int PpeUpdateVXLANTunnel_swtable(int index,int dir){
	char vpm = 0, vlan_en = 0, pppoe_en = 0;
	unsigned short prechk = 0;
    int saddr_id = 0;

	//parse gre tunnel config
	if(index<0 || index >= TUNNEL_TABLE_NUM){
		//HWNAT_BINDFAIL_DPRINT("index not in range \n");
		return 0;
	}

	if(dir == 0 ){
	//tunnelTable_t TUNNEL_SWTABLE[TUNNEL_TABLE_NUM] ;
		TUNNEL_SWTABLE[index].config.raw.is_l2_tunnel = 1;
		TUNNEL_SWTABLE[index].config.raw.use_inner_ttl = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_DSCP = 0;
		TUNNEL_SWTABLE[index].config.raw.gre_key_flag = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_over_ip = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_v3 = 0;
		TUNNEL_SWTABLE[index].config.raw.ipv4_id = 1;
		TUNNEL_SWTABLE[index].config.raw.udp_cks = 1;
		TUNNEL_SWTABLE[index].config.raw.tun0 = 1;
		TUNNEL_SWTABLE[index].config.raw.tun2 = 4;
		TUNNEL_SWTABLE[index].config.raw.tun3 = 5;

		vpm = ppe_trans_tpid_to_vpm(PpeParseResult.vlan_tag);
		TUNNEL_SWTABLE[index].config.raw.vpm = vpm;
		
		if (PpeParseResult.vlan_layer == 0)
       		vlan_en = 0;
		else
			vlan_en = 1;
		TUNNEL_SWTABLE[index].config.raw.vlan_en = vlan_en;

		if(PpeParseResult.eth_type == htons(ETH_P_PPP_SES))
			pppoe_en = 1;
		TUNNEL_SWTABLE[index].config.raw.pppoe_en = pppoe_en;

		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[3] = PpeParseResult.dmac[0];
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[2] = PpeParseResult.dmac[1];
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[1] = PpeParseResult.dmac[2];
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[0] = PpeParseResult.dmac[3];
		
		TUNNEL_SWTABLE[index].mac_header1.dmac_lo[1] = PpeParseResult.dmac[4];
		TUNNEL_SWTABLE[index].mac_header1.dmac_lo[0] = PpeParseResult.dmac[5];

		TUNNEL_SWTABLE[index].mac_header1.smac_hi[1] = PpeParseResult.smac[0];
		TUNNEL_SWTABLE[index].mac_header1.smac_hi[0] = PpeParseResult.smac[1];



		TUNNEL_SWTABLE[index].mac_header2.smac_lo[3] = PpeParseResult.smac[2];
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[2] = PpeParseResult.smac[3];
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[1] = PpeParseResult.smac[4];
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[0] = PpeParseResult.smac[5];
		

		TUNNEL_SWTABLE[index].mac_header3.vlan_id = ntohs(PpeParseResult.vlan1);
		TUNNEL_SWTABLE[index].mac_header3.pppoe_id= ntohs(PpeParseResult.pppoe_sid);

        if((PpeParseResult.pkt_type == IPV6_5T_ROUTE)||(PpeParseResult.pkt_type == IPV6_3T_ROUTE))
        {
            TUNNEL_SWTABLE[index].config.raw.tun1 = 3;

            TUNNEL_SWTABLE[index].ip_header0.ipv6_raw.hop_limit = PpeParseResult.ip6h.hop_limit;
    		TUNNEL_SWTABLE[index].ip_header0.ipv6_raw.traffic_class = (PpeParseResult.ip6h.priority<<4)|\
                ((PpeParseResult.ip6h.flow_lbl[2]&0xf0)>>4);

            
            prechk = PpeCalipv6_prechk(PpeParseResult.ip6h.saddr.s6_addr16,PpeParseResult.ip6h.daddr.s6_addr16);
    		TUNNEL_SWTABLE[index].ip_header0.ipv6_raw.pre_chk = prechk;
                    if (DebugLevel >= 1) {
                printk("7552 precks = 0x%x\n",prechk);
		    }

            saddr_id = find_and_update_wan_ip6_table(PpeParseResult.ip6h.saddr.s6_addr32);
            if(saddr_id != -1) 
                TUNNEL_SWTABLE[index].ip_header1.ipv6_raw.sip_id = saddr_id;

            TUNNEL_SWTABLE[index].ip_header1.ipv6_raw.flow_lable = ((PpeParseResult.ip6h.flow_lbl[2]&0xf)<<16)|\
                (PpeParseResult.ip6h.flow_lbl[1]<<8)|PpeParseResult.ip6h.flow_lbl[0];

            TUNNEL_SWTABLE[index].ip_header2.ipv6_dip0 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]);
            TUNNEL_SWTABLE[index].ip_header3.ipv6_dip1 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]);
            TUNNEL_SWTABLE[index].ip_header4.ipv6_dip2 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]);
            TUNNEL_SWTABLE[index].ip_header5.ipv6_dip3 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]);
        }
        else
        {
            TUNNEL_SWTABLE[index].config.raw.tun1 = 2;
            
            TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.ttl = PpeParseResult.iph.ttl;
    		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.dscp = PpeParseResult.iph.tos;

    		prechk = PpeCalipv4_prechk(ntohl(PpeParseResult.iph.saddr),ntohl(PpeParseResult.iph.daddr));
    		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.pre_chk = prechk;

    		TUNNEL_SWTABLE[index].ip_header1.ipv4_raw.iden = ntohs(PpeParseResult.iph.id);
    		
    		TUNNEL_SWTABLE[index].ip_header2.ipv4_sip = ntohl(PpeParseResult.iph.saddr);
    		TUNNEL_SWTABLE[index].ip_header3.ipv4_dip = ntohl(PpeParseResult.iph.daddr);
        }
		TUNNEL_SWTABLE[index].vxlan_header.raw.vni =(ntohl(PpeParseResult.vxlanh.vx_vni) >> 8 )& 0x00FFFFFF;
		
		TUNNEL_SWTABLE[index].udp_header.raw.srcport = ntohs(PpeParseResult.uh.source);
		TUNNEL_SWTABLE[index].udp_header.raw.dstport = ntohs(PpeParseResult.uh.dest);


		
	}else{
		//down stream set config 
		TUNNEL_SWTABLE[index].config.raw.is_l2_tunnel = 1;
		TUNNEL_SWTABLE[index].config.raw.use_inner_ttl = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_DSCP = 0;
		TUNNEL_SWTABLE[index].config.raw.gre_key_flag = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_over_ip = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_v3 = 0;
		TUNNEL_SWTABLE[index].config.raw.ipv4_id = 0;
		TUNNEL_SWTABLE[index].config.raw.udp_cks = 0;
		TUNNEL_SWTABLE[index].config.raw.tun0 = 1;
		TUNNEL_SWTABLE[index].config.raw.tun2 = 4;
		TUNNEL_SWTABLE[index].config.raw.tun3 = 5;

        if((PpeParseResult.pkt_type == IPV6_5T_ROUTE)||(PpeParseResult.pkt_type == IPV6_3T_ROUTE))
        {
            TUNNEL_SWTABLE[index].config.raw.tun1 = 3;
        }
        else
		{
            TUNNEL_SWTABLE[index].config.raw.tun1 = 2;
    		TUNNEL_SWTABLE[index].ip_header2.ipv4_sip = ntohl(PpeParseResult.iph.daddr);
    		TUNNEL_SWTABLE[index].ip_header3.ipv4_dip = ntohl(PpeParseResult.iph.saddr);
        }

		TUNNEL_SWTABLE[index].vxlan_header.raw.vni =(ntohl(PpeParseResult.vxlanh.vx_vni) >> 8 )& 0x00FFFFFF;


	}
	
	if (hwnat_dbg_parse_dump >= 2) 
		PpeParseTunnelSwInfo(index);

	return 0 ;
}




/*L2tp v2 tunnel setting*/
int PpeUpdateL2TPTunnel_swtable(int index,int dir){

	unsigned short prechk = 0;
	unsigned int tunnel = 0, session = 0,tunnel_session = 0;
	//parse gre tunnel config
	if(index<0 || index >= TUNNEL_TABLE_NUM){
		//HWNAT_BINDFAIL_DPRINT("index not in range \n");
		return 0;
	}

	if(dir == 0 ){
	//tunnelTable_t TUNNEL_SWTABLE[TUNNEL_TABLE_NUM] ;
		TUNNEL_SWTABLE[index].config.raw.is_l2_tunnel = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_ttl = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_DSCP = 0;
		TUNNEL_SWTABLE[index].config.raw.gre_key_flag = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_over_ip = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_v3 = 0;
		TUNNEL_SWTABLE[index].config.raw.ipv4_id = 1;
		TUNNEL_SWTABLE[index].config.raw.udp_cks = 1;
		TUNNEL_SWTABLE[index].config.raw.tun0 = 2;
		TUNNEL_SWTABLE[index].config.raw.tun1 = 4;
		TUNNEL_SWTABLE[index].config.raw.tun2 = 7;
		TUNNEL_SWTABLE[index].config.raw.tun3 = 0;


		TUNNEL_SWTABLE[index].config.raw.vpm = 0;
		TUNNEL_SWTABLE[index].config.raw.vlan_en = 0;
		TUNNEL_SWTABLE[index].config.raw.pppoe_en = 0;

		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[3] = 0;
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[2] = 0;
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[1] = 0;
		TUNNEL_SWTABLE[index].mac_header0.dmac_hi[0] = 0;
		
		TUNNEL_SWTABLE[index].mac_header1.dmac_lo[1] = 0;
		TUNNEL_SWTABLE[index].mac_header1.dmac_lo[0] = 0;

		TUNNEL_SWTABLE[index].mac_header1.smac_hi[1] = 0;
		TUNNEL_SWTABLE[index].mac_header1.smac_hi[0] = 0;


		TUNNEL_SWTABLE[index].mac_header2.smac_lo[3] = 0;
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[2] = 0;
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[1] = 0;
		TUNNEL_SWTABLE[index].mac_header2.smac_lo[0] = 0;
		
		TUNNEL_SWTABLE[index].mac_header3.vlan_id = 0;
		TUNNEL_SWTABLE[index].mac_header3.pppoe_id= 0;

		
		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.ttl = PpeParseResult.iph.ttl;
		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.dscp = PpeParseResult.iph.tos;

		prechk = PpeCalipv4_prechk(ntohl(PpeParseResult.iph.saddr),ntohl(PpeParseResult.iph.daddr));
		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.pre_chk = prechk;

		TUNNEL_SWTABLE[index].ip_header1.ipv4_raw.iden = ntohs(PpeParseResult.iph.id);
		
		TUNNEL_SWTABLE[index].ip_header2.ipv4_sip = ntohl(PpeParseResult.iph.saddr);
		TUNNEL_SWTABLE[index].ip_header3.ipv4_dip = ntohl(PpeParseResult.iph.daddr);

		TUNNEL_SWTABLE[index].udp_header.raw.srcport = ntohs(PpeParseResult.uh.source);
		TUNNEL_SWTABLE[index].udp_header.raw.dstport = ntohs(PpeParseResult.uh.dest);

		tunnel =  ntohs(PpeParseResult.l2tp_tunnelid);
		session = ntohs(PpeParseResult.l2tp_sessionid);
		tunnel_session = (tunnel<<16) + session;
		TUNNEL_SWTABLE[index].l2tp_header.tunnel_session = tunnel_session;
		
	}else{
		//down stream set config 
		TUNNEL_SWTABLE[index].config.raw.is_l2_tunnel = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_ttl = 0;
		TUNNEL_SWTABLE[index].config.raw.use_inner_DSCP = 0;
		TUNNEL_SWTABLE[index].config.raw.gre_key_flag = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_over_ip = 0;
		TUNNEL_SWTABLE[index].config.raw.l2tp_v3 = 0;
		TUNNEL_SWTABLE[index].config.raw.ipv4_id = 0;
		TUNNEL_SWTABLE[index].config.raw.udp_cks = 0;
		TUNNEL_SWTABLE[index].config.raw.tun0 = 2;
		TUNNEL_SWTABLE[index].config.raw.tun1 = 4;
		TUNNEL_SWTABLE[index].config.raw.tun2 = 7;
		TUNNEL_SWTABLE[index].config.raw.tun3 = 0;

		TUNNEL_SWTABLE[index].ip_header2.ipv4_sip = ntohl(PpeParseResult.iph.daddr);
		TUNNEL_SWTABLE[index].ip_header3.ipv4_dip = ntohl(PpeParseResult.iph.saddr);


	}
	if (hwnat_dbg_parse_dump >= 2) 
		PpeParseTunnelSwInfo(index);

	return 0 ;
}


//skb data local in after tcp/udp/or gre
int PpeParseL4V4Info_InTunnel(struct sk_buff * skb, struct port_info * pinfo, int magic)
{
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;

	skb_reset_inner_transport_header(skb);

	if (PpeParseResult.iph_intunnel.protocol == IPPROTO_TCP) 
	{
		

		th = (struct tcphdr *)(skb->data);
		memcpy(&PpeParseResult.th_intunnel, th, sizeof(struct tcphdr));
		PpeParseResult.pkt_type_intunnel= IPV4_HNAPT;
	}
	else if(PpeParseResult.iph_intunnel.protocol == IPPROTO_UDP)
	{
		uh = (struct udphdr *)(skb->data);
		memcpy(&PpeParseResult.uh_intunnel, uh, sizeof(struct udphdr));
		PpeParseResult.pkt_type_intunnel= IPV4_HNAPT;
		
	}
	else 
	{
		PpeParseResult.pkt_type_intunnel= IPV4_HNAT;
	}
	
	return 0;
}




int PpeParseL4V6Info_InTunnel(struct sk_buff * skb, struct port_info * pinfo, int magic)
{
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;
	
	if (PpeParseResult.ip6h_intunnel.nexthdr == NEXTHDR_TCP) 
	{
		th = (struct tcphdr *)(skb->data);
		memcpy(&PpeParseResult.th_intunnel, th, sizeof(struct tcphdr));
		PpeParseResult.pkt_type_intunnel= IPV6_5T_ROUTE;
	} 
	else if (PpeParseResult.ip6h_intunnel.nexthdr == NEXTHDR_UDP) 
	{
		uh = (struct udphdr *)(skb->data);
		memcpy(&PpeParseResult.uh_intunnel, uh, sizeof(struct udphdr));
		PpeParseResult.pkt_type_intunnel= IPV6_5T_ROUTE;
	}
	else if (PpeParseResult.ip6h.nexthdr == NEXTHDR_IPIP) 
	{
		return 1;
	}
	else 
	{
		PpeParseResult.pkt_type_intunnel = IPV6_3T_ROUTE;
	}
		
	return 0;
}


int PpeParseL3AndL4Info_InTunnel(struct sk_buff * skb, struct port_info * pinfo, int magic)
{	
	struct iphdr *iph = NULL;	
	struct ipv6hdr *ip6h = NULL;
	
	skb_reset_inner_network_header(skb);

#if 0
	if(IS_L2_RRIDGE(foe_entry))
	{
		PpeParseResult.pkt_type = L2_BRIDGE;
		return 0;
	}
#endif
	//tunel_type need add 
	if ((PpeParseResult.eth_type_intunnel== htons(ETH_P_IP)) || (PpeParseResult.eth_type_intunnel== htons(ETH_P_PPP_SES)
		&& PpeParseResult.ppp_tag_intunnel== htons(PPP_IP)) || (PpeParseResult.l2tp_ppp_tag== htons(PPP_IP))) 
	{		
		PpeParseResult.pkt_type_intunnel= IPV4_HNAT;
		
		iph = (struct iphdr *)(skb->data);
		
		memcpy(&PpeParseResult.iph_intunnel, iph, sizeof(struct iphdr));

		skb->data += (iph->ihl * 4);

		if(PpeParseL4V4Info_InTunnel(skb,pinfo,magic))
		{
			HWNAT_BINDFAIL_DPRINT("parse L4 V4 fail\n");
			return 1;
		}
	}
	else if (PpeParseResult.eth_type_intunnel== htons(ETH_P_IPV6) || (PpeParseResult.eth_type_intunnel== htons(ETH_P_PPP_SES)
		        &&PpeParseResult.ppp_tag_intunnel== htons(PPP_IPV6)) || (PpeParseResult.l2tp_ppp_tag== htons(PPP_IPV6)))
	{		
		PpeParseResult.pkt_type_intunnel = IPV6_3T_ROUTE;
				
		ip6h = (struct ipv6hdr *)(skb->data);
	
		memcpy(&PpeParseResult.ip6h_intunnel, ip6h, sizeof(struct ipv6hdr));
		
		skb->data += sizeof(struct ipv6hdr);

		if(PpeParseL4V6Info_InTunnel(skb,pinfo,magic))
		{
			HWNAT_BINDFAIL_DPRINT("parse L4 V6 fail\n");
			return 1;
		}

		
	}
	else
	{
		HWNAT_BINDFAIL_DPRINT("htons(PpeParseResult.eth_type) = 0x%x,htons(PpeParseResult.ppp_tag) = 0x%x\n",htons(PpeParseResult.eth_type_intunnel),htons(PpeParseResult.ppp_tag_intunnel));
		return 1;
	}

	return 0;
}



int PpeParseL2Info_InTunnel(struct sk_buff * skb, struct port_info * pinfo, int magic)
{
	struct ethhdr *eth = NULL;
	struct vlan_hdr *vh = NULL;
	unsigned int vlan_tag_num = ppe_get_vlan_tag_num(skb);	

	eth = (struct ethhdr *)skb->data;
	memcpy(PpeParseResult.dmac_intunnel, eth->h_dest, ETH_ALEN);
	memcpy(PpeParseResult.smac_intunnel, eth->h_source, ETH_ALEN);
	PpeParseResult.vlan_layer_intunnel = vlan_tag_num;
	PpeParseResult.eth_type_intunnel= eth->h_proto;		
	skb->data += ETH_HLEN;
	if(vlan_tag_num > 0)
	{
		PpeParseResult.vlan_tag_intunnel= eth->h_proto;
		vh = (struct vlan_hdr *)(skb->data);
		PpeParseResult.vlan1_intunnel= vh->h_vlan_TCI;
		PpeParseResult.eth_type_intunnel= vh->h_vlan_encapsulated_proto;
		skb->data += VLAN_HLEN;
		if(vlan_tag_num > 1)
		{
			vh = (struct vlan_hdr *)(skb->data);
			PpeParseResult.vlan2_intunnel= vh->h_vlan_TCI;				
			PpeParseResult.eth_type_intunnel= vh->h_vlan_encapsulated_proto;				
			skb->data += VLAN_HLEN;
		}

		if(vlan_tag_num > 2)
		{
			HWNAT_BINDFAIL_DPRINT("tunnel not support vlan_tag_num>2\n");
			return 1;
		}
	}

	if(PpeParseResult.eth_type_intunnel== htons(ETH_P_PPP_SES)){
		PpeParseResult.pppoe_gap_intunnel= 8;
		if (GetPppoeSid(skb, &PpeParseResult.pppoe_sid_intunnel,&PpeParseResult.ppp_tag_intunnel)) {
				HWNAT_BINDFAIL_DPRINT("get pppoe sid fail\n");
				return 1;
		}
		skb->data += PpeParseResult.pppoe_gap_intunnel;
	}



	return 0;
}




int PpeParseL2Info(struct sk_buff * skb, struct port_info * pinfo, int magic)
{
	struct ethhdr *eth = NULL;
	struct vlan_hdr *vh = NULL;
	unsigned int vlan_tag_num = ppe_get_vlan_tag_num(skb);	

	skb_reset_mac_header(skb);

	if((magic == FOE_MAGIC_ATM) && ((pinfo->atm_ipoa) || (pinfo->atm_pppoa)))
	{
		if((pinfo->atm_ipoa) && (pinfo->atm_vc_mux))
			eth = (struct ethhdr *)(skb->data - (ETH_ALEN<<1) - 2);//dummymac header offset
		else
			eth = (struct ethhdr *)(skb->data - (ETH_ALEN<<1));//dummymac header offset
		PpeParseResult.eth_type = skb->protocol;
	}
	else
	{
		eth = (struct ethhdr *)skb->data;
		memcpy(PpeParseResult.dmac, eth->h_dest, ETH_ALEN);
		memcpy(PpeParseResult.smac, eth->h_source, ETH_ALEN);
		PpeParseResult.vlan_layer = vlan_tag_num;
		PpeParseResult.eth_type = eth->h_proto;		
		skb->data += ETH_HLEN;
		if(vlan_tag_num > 0)
		{
			PpeParseResult.vlan_tag = eth->h_proto;
			vh = (struct vlan_hdr *)(skb->data);
			PpeParseResult.vlan1 = vh->h_vlan_TCI;
			PpeParseResult.eth_type = vh->h_vlan_encapsulated_proto;
			skb->data += VLAN_HLEN;
			if(vlan_tag_num > 1)
			{
				vh = (struct vlan_hdr *)(skb->data);
				PpeParseResult.vlan2 = vh->h_vlan_TCI;				
				PpeParseResult.eth_type = vh->h_vlan_encapsulated_proto;				
				skb->data += VLAN_HLEN;
			}

			if(vlan_tag_num > 2)
			{
				HWNAT_BINDFAIL_DPRINT("not support vlan_tag_num>2\n");
				return 1;
			}
		}
	}

	if ((sysctl_hwnat_off & 0x1) &&  ((eth->h_dest[0] & 0x1) == 0))   //unicast packet
	{	
		HWNAT_BINDFAIL_DPRINT("sysctl_hwnat_off = 0x%x\n",sysctl_hwnat_off);
		return 1;
	}
	if ((sysctl_hwnat_off & 0x2) && (eth->h_dest[0] == 0x01) && (eth->h_dest[1] == 0x0) && (eth->h_dest[2] == 0x5e)) //ipv4 multicast
	{
		HWNAT_BINDFAIL_DPRINT("sysctl_hwnat_off = 0x%x\n",sysctl_hwnat_off);
		return 1;
	}
	if ((sysctl_hwnat_off & 0x4) && (eth->h_dest[0] == 0x33) && (eth->h_dest[1] == 0x33))  //ipv6 multicast
	{
		HWNAT_BINDFAIL_DPRINT("sysctl_hwnat_off = 0x%x\n",sysctl_hwnat_off);
		return 1;
	}

	if(hwnat_broadcast_en == 0)
	{
		if(is_broadcast_ether_addr(&eth->h_dest[0]))
		{
			HWNAT_BINDFAIL_DPRINT("broadcast packet did't set bind state\n");
			return 1;
		}
	}

	if((TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting) || TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
	{
		pkttype = 0;
		if(is_multicast_ether_addr(&eth->h_dest[0]))
		{
			pkttype |= DS_MULTICAST;
			if(TCSUPPORT_VLAN_ACCESS_TRUNK_VAL && magic != FOE_MAGIC_GE){
				/* only lan packet learn hw_nat entry */
				return 1;
			}
		}
		if(skb->macEnque&dsoffload_presetting)
			pkttype |= DS_MAC;
	}

	if ((multicast_en == 0)&&(is_multicast_ether_addr(&eth->h_dest[0])))
	{
		HWNAT_BINDFAIL_DPRINT("multicast_en is 0\n");
		return 1;
	}

	if(PpeParseResult.eth_type == htons(ETH_P_PPP_SES)){
		PpeParseResult.pppoe_gap = 8;
		if (GetPppoeSid(skb, &PpeParseResult.pppoe_sid,&PpeParseResult.ppp_tag)) {
				HWNAT_BINDFAIL_DPRINT("get pppoe sid fail\n");
				return 1;
		}
		skb->data += PpeParseResult.pppoe_gap;
	}

	if(skb->dev && isWiFiInterface(skb->dev) && judgeinterface(skb)/* && is_multicast_ether_addr(&eth->h_dest[0])*/)
	{
		PpeParseResult.vlan_layer++;
	}

	return 0;
}

int PpeParse6rdInfo(struct sk_buff * skb, struct port_info * pinfo, int magic)
{	
	struct tcphdr *th = NULL;	
	struct udphdr *uh = NULL;
	
	if (PpeParseResult.ip6h.nexthdr == NEXTHDR_TCP) 
	{
		if (TCSUPPORT_XPON_HAL_API_EXT_VAL == 1)
		{
			if(isLanTcpBlackList(skb, magic, pinfo))
				return 1;
		}

		skb->data += sizeof(struct ipv6hdr);
		th = (struct tcphdr *)(skb->data);
		
		if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
			ipv6TcpPkttypeMark(th);
		
		memcpy(&PpeParseResult.th.source, &th->source, sizeof(th->source));
		memcpy(&PpeParseResult.th.dest, &th->dest, sizeof(th->dest));
	} 
	else if (PpeParseResult.ip6h.nexthdr == NEXTHDR_UDP) 
	{
		skb->data += sizeof(struct ipv6hdr);

		uh = (struct udphdr *)(skb->data);
		memcpy(&PpeParseResult.uh.source, &uh->source, sizeof(uh->source));
		memcpy(&PpeParseResult.uh.dest, &uh->dest, sizeof(uh->dest));

		if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
			pkttype |= DS_UDP;
	}
	else
		return 1;

	PpeParseResult.pkt_type = IPV6_6RD;
	return 0;
}



//parse gre tunnel mac ip tcp/udp header

int PpeParseGreTunnelInfo(struct sk_buff * skb, struct port_info * pinfo, int magic)
{
	struct gre_base_hdr *greh;
	int table_index = -1;
	unsigned int foe_num = FOE_ENTRY_NUM(skb);

	greh = (struct gre_base_hdr *)skb->data;
	memcpy(&PpeParseResult.greh, greh, sizeof(struct gre_base_hdr));

	if(magic == FOE_MAGIC_GRE_HWDOWN_1){
		if(PpeParseResult.greh.protocol == htons(ETH_P_TEB)){
			PpeParseResult.is_L2_tunnel =1;
			skb->data += sizeof(struct gre_base_hdr);
			
			if(PpeParseL2Info_InTunnel(skb,pinfo,magic)){
				HWNAT_BINDFAIL_DPRINT("PpeParseL2Info_InTunnel fail\n");
				return 1;
			}


			if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
			{		
				HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
				return 1;
			}
			
			//set gre_tunnel table info 
			table_index = PpeGetTunnel_table_index(1,L2GRE_TUNNEL);
			if(table_index != -1 ){
				PpeParseResult.tunnel_en = 1;
				PpeParseResult.tunnel_id = table_index;
				PpeParseResult.hop_0 = 7; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel

				if(TUNNEL_SWTABLE[table_index].is_table_rm_valid == 0){
					PpeUpdateGRETunnel_swtable( table_index, 1, 1);
					PpeSetTunnel_table_ToHW( table_index);
					TUNNEL_SWTABLE[table_index].is_table_rm_valid = 1;
				}
				ppe_add_session_to_tunnel_list(table_index,foe_num);
			}else{
				HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
				return 1;
			}
			
		}else if(PpeParseResult.greh.protocol == htons(ETH_P_IP)){
			PpeParseResult.is_L3_tunnel =1;
			skb->data += sizeof(struct gre_base_hdr);
			PpeParseResult.eth_type_intunnel = htons(ETH_P_IP);

			if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
			{		
				HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
				return 1;
			}
			
			//set gre_tunnel table info 
			table_index = PpeGetTunnel_table_index(1,L3GRE_TUNNEL);
			if(table_index != -1 ){
				PpeParseResult.tunnel_en = 1;
				PpeParseResult.tunnel_id = table_index;
				PpeParseResult.hop_0 = 7; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel

				if(TUNNEL_SWTABLE[table_index].is_table_rm_valid == 0){
					PpeUpdateGRETunnel_swtable( table_index, 1, 0);
					PpeSetTunnel_table_ToHW( table_index);
					TUNNEL_SWTABLE[table_index].is_table_rm_valid = 1;
				}
				ppe_add_session_to_tunnel_list(table_index,foe_num);
			}else{
				HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
				return 1;
			}
			
		}else{

			HWNAT_BINDFAIL_DPRINT("not support gre proto %x\n", ntohs(PpeParseResult.greh.protocol));
			return 1;
		}


	}else{
		if(PpeParseResult.greh.protocol == htons(ETH_P_TEB)){
			PpeParseResult.is_L2_tunnel =1;
			skb->data += sizeof(struct gre_base_hdr);
			
			if(PpeParseL2Info_InTunnel(skb,pinfo,magic)){

				HWNAT_BINDFAIL_DPRINT("PpeParseL2Info_InTunnel fail\n");
				return 1;
			}


			if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
			{		
				HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
				return 1;
			}
			
			//set gre_tunnel table info 
			table_index = PpeGetTunnel_table_index(0,L2GRE_TUNNEL);

			if(table_index != -1 ){
				PpeParseResult.tunnel_en = 1;
				PpeParseResult.tunnel_id = table_index;
				PpeParseResult.hop_0 = 6; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel

				if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0){
					PpeUpdateGRETunnel_swtable( table_index, 0, 1);
					PpeSetTunnel_table_ToHW( table_index );
					TUNNEL_SWTABLE[table_index].is_table_add_valid = 1;
				}
				ppe_add_session_to_tunnel_list(table_index,foe_num);
				
			}else{
				HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
				return 1;
			}
			
		}else if(PpeParseResult.greh.protocol == htons(ETH_P_IP)){
			PpeParseResult.is_L3_tunnel =1;
			skb->data += sizeof(struct gre_base_hdr);
			PpeParseResult.eth_type_intunnel = htons(ETH_P_IP);
			
			if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
			{		
				HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
				return 1;
			}
			
			//set gre_tunnel table info 
			table_index = PpeGetTunnel_table_index(0, L3GRE_TUNNEL);

			if(table_index != -1 ){
				PpeParseResult.tunnel_en = 1;
				PpeParseResult.tunnel_id = table_index;
				PpeParseResult.hop_0 = 6; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
				PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel

				if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0){
					PpeUpdateGRETunnel_swtable( table_index, 0, 0);
					PpeSetTunnel_table_ToHW( table_index );
					TUNNEL_SWTABLE[table_index].is_table_add_valid = 1;
				}
				ppe_add_session_to_tunnel_list(table_index,foe_num);
				
			}else{
				HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
				return 1;
			}
			
		}else{
			
			HWNAT_BINDFAIL_DPRINT("not support gre proto %x\n", ntohs(PpeParseResult.greh.protocol));
			return 1;
		}
	}

	return 0;
}




//parse vxlan tunnel mac ip tcp/udp header
int PpeParseVXLANTunnelInfo(struct sk_buff * skb, struct port_info * pinfo, int magic)
{

	struct vxlanhdr *vxlan_header;
	//skb data in ip header
	int table_index = -1;
	unsigned int foe_num = FOE_ENTRY_NUM(skb);
	skb->data += sizeof(struct udphdr);
	vxlan_header = (struct vxlanhdr *)skb->data;
 
	memcpy(&PpeParseResult.vxlanh, vxlan_header, sizeof(struct vxlanhdr));


	skb->data += sizeof(struct vxlanhdr);

	if(magic == FOE_MAGIC_VXLAN_HWDOWN_1){
		PpeParseResult.is_L2_tunnel =1;

		//skb->data in mac parse
		if(PpeParseL2Info_InTunnel(skb,pinfo,magic)){
			HWNAT_BINDFAIL_DPRINT("PpeParseL2Info_InTunnel fail\n");
			return 1;
		}


		if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
		{		
			HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
			return 1;
		}
		
		//set gre_tunnel table info 
		table_index = PpeGetTunnel_table_index(1,VXLAN_TUNNEL);

		if(table_index != -1 ){
			PpeParseResult.tunnel_en = 1;
			PpeParseResult.tunnel_id = table_index;
			PpeParseResult.hop_0 = 7; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel
	
			if(TUNNEL_SWTABLE[table_index].is_table_rm_valid == 0){
				PpeUpdateVXLANTunnel_swtable( table_index, 1);
				PpeSetTunnel_table_ToHW( table_index);
				TUNNEL_SWTABLE[table_index].is_table_rm_valid = 1;
			}
			ppe_add_session_to_tunnel_list(table_index,foe_num);
		}else{
			HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
			return 1;
		}

	}else{
		PpeParseResult.is_L2_tunnel =1;

		//skb->data in mac parse
		if(PpeParseL2Info_InTunnel(skb,pinfo,magic)){
			HWNAT_BINDFAIL_DPRINT("PpeParseL2Info_InTunnel fail\n");
			return 1;
		}


		if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
		{		
			HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
			return 1;
		}
		
		//set gre_tunnel table info 
		table_index = PpeGetTunnel_table_index(0,VXLAN_TUNNEL);

		if(table_index != -1 ){
			PpeParseResult.tunnel_en = 1;
			PpeParseResult.tunnel_id = table_index;
			PpeParseResult.hop_0 = 6; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel
			
			if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0){
				PpeUpdateVXLANTunnel_swtable( table_index, 0);
				PpeSetTunnel_table_ToHW( table_index );
				TUNNEL_SWTABLE[table_index].is_table_add_valid = 1;
			}

			ppe_add_session_to_tunnel_list(table_index,foe_num);
		}else{
			HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
			return 1;
		}
	}


	return 0;

}

inline int get_l2tp_hlen(struct l2tp_hdr *ph)
{
	int l2tp_hdr_len = L2TP_MIN_HDR_LEN;
	
	/* type bit info */
	/* 1) if control packet , send to kernel handle */
	/* 2) if not L2TP v2 , send to kernel handle */
	if ( TYP_BIT(ph->ver) || (VER_BIT(ph->ver) !=L2TP_HDR_VER_2) )
	{
		return 0;
	}
		
	/* length bit info */
	if (LEN_BIT(ph->ver))
	{
		l2tp_hdr_len += 2;
	}

	/* Ns and Nr bit info */
	if (SEQ_BIT(ph->ver))
	{
		/* Ns or Nr bit, should send to kernel handle */
		return 0;
	}

	 /* offset bit info */
	if (OFS_BIT(ph->ver))
	{
		l2tp_hdr_len += 2;
	}

	return l2tp_hdr_len;
}


int PpeParseL2TPTunnelInfo(struct sk_buff * skb, struct port_info * pinfo, int magic)
{
	struct l2tp_hdr *l2tph = NULL;
	struct ppphdr *ppph = NULL;
	int l2tp_hdr_len = 0;
	unsigned int foe_num = FOE_ENTRY_NUM(skb);
	uint16 tunnel_id = 0;
	uint16 session_id = 0;
	int table_index = -1;

	//dump_skb(skb);
	//skb data in ip header
	skb->data += sizeof(struct udphdr);
	
	l2tph = (struct l2tp_hdr *)skb->data;

	l2tp_hdr_len = get_l2tp_hlen(l2tph);
	if(l2tp_hdr_len == 0){
		HWNAT_BINDFAIL_DPRINT("PpeParse L2tp tunnel info fail\n");
		return 1;
	}

	if (LEN_BIT(l2tph->ver) && (magic != FOE_MAGIC_L2TP_HWDOWN_1)){
		HWNAT_BINDFAIL_DPRINT("PpeParse L2tp tunnel info fail,bot support length bit \n");
		return 1;
	}else{
		tunnel_id = (l2tph->length);
		session_id = (l2tph->tid);
		PpeParseResult.l2tp_tunnelid = tunnel_id;
		PpeParseResult.l2tp_sessionid = session_id;
	}


	//skip L2tp header and ppp header
	skb->data += l2tp_hdr_len;
	ppph =  (struct ppphdr *)skb->data;
	PpeParseResult.l2tp_ppp_tag = ppph->protocol;
	skb->data += sizeof(struct ppphdr);

	if(FOE_MAGIC_L2TP_HWDOWN_1 == magic){
		PpeParseResult.is_L3_tunnel =1;
		if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
		{		
			HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
			return 1;
		}

		table_index = PpeGetTunnel_table_index(1,L2TP_TUNNEL);
		if(table_index != -1 ){
			PpeParseResult.tunnel_en = 1;
			PpeParseResult.tunnel_id = table_index;
			PpeParseResult.hop_0 = 7; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel

			if(TUNNEL_SWTABLE[table_index].is_table_rm_valid == 0){
				PpeUpdateL2TPTunnel_swtable( table_index, 1);
				PpeSetTunnel_table_ToHW( table_index);
				TUNNEL_SWTABLE[table_index].is_table_rm_valid = 1;
			}
			ppe_add_session_to_tunnel_list(table_index,foe_num);

		}else{
			HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
			return 1;
		}


	}else{
		PpeParseResult.is_L3_tunnel =1;
		if(PpeParseL3AndL4Info_InTunnel(skb,pinfo,magic))
		{		
			HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
			return 1;
		}

		table_index = PpeGetTunnel_table_index(0,L2TP_TUNNEL);
		if(table_index != -1 ){
			PpeParseResult.tunnel_en = 1;
			PpeParseResult.tunnel_id = table_index;
			PpeParseResult.hop_0 = 6; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel
			if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0){
				PpeUpdateL2TPTunnel_swtable( table_index, 0);
				PpeSetTunnel_table_ToHW( table_index );
				TUNNEL_SWTABLE[table_index].is_table_add_valid = 1;
			}
			ppe_add_session_to_tunnel_list(table_index,foe_num);

		}else{
			HWNAT_BINDFAIL_DPRINT("there is none tunnel table use for gre tunnel\n");
			return 1;
		}

	}


	return 0;

}

static inline int PpeCheckVxlanTunnel(u16 dport, int magic, struct FoeEntry *foe_entry)
{
	if(dport != htons(VXLAN_DEFAULT_UDP_PORT))
		return 0;

	if(magic == FOE_MAGIC_VXLAN_HWDOWN_1)
		return 1;

	/* Vxlan Passthrough */
	if(IS_IPV4_HNAPT(foe_entry) && foe_entry->bfib1.udp && 
		foe_entry->ipv4_hnapt.dport == VXLAN_DEFAULT_UDP_PORT)
		return 0;

	if(IS_IPV6_5T_ROUTE(foe_entry) && foe_entry->bfib1.udp && 
		foe_entry->ipv6_5t_route.dport == VXLAN_DEFAULT_UDP_PORT)
		return 0;

	return 1;
}

static inline int PpeCheckL2tpTunnel(u16 dport, u16 sport, int magic, struct FoeEntry *foe_entry)
{
	if(dport != htons(L2TP_UDP_LISTEN_PORT) && sport != htons(L2TP_UDP_LISTEN_PORT))
		return 0;

	if(magic == FOE_MAGIC_L2TP_HWDOWN_1)
		return 1;

	/* L2TP Passthrough */
	if(IS_IPV4_HNAPT(foe_entry) && foe_entry->bfib1.udp && 
		foe_entry->ipv4_hnapt.dport == L2TP_UDP_LISTEN_PORT)
		return 0;

	if(IS_IPV6_5T_ROUTE(foe_entry) && foe_entry->bfib1.udp && 
		foe_entry->ipv6_5t_route.dport == L2TP_UDP_LISTEN_PORT)
		return 0;

	return 1;
}

static inline int PpeCheckGreTunnel(u8 proto, int magic, struct FoeEntry *foe_entry)
{
	if(proto != IPPROTO_GRE)
		return 0;

	if(magic == FOE_MAGIC_GRE_HWDOWN_1)
		return 1;

	/* GRE Passthrough */
	if(IS_IPV4_HNAT(foe_entry) &&
		(foe_entry->ipv4_hnapt.dport&0xFF) == IPPROTO_GRE)
		return 0;

	if(IS_IPV6_3T_ROUTE(foe_entry) &&
		foe_entry->ipv6_3t_route.prot == IPPROTO_GRE)
		return 0;

	return 1;
}

//skb data local in after tcp/udp/or gre
int PpeParseL4V4Info(struct sk_buff * skb, struct port_info * pinfo, int magic, struct FoeEntry *foe_entry)
{
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;
	struct ipv6hdr *ip6h = NULL;
	struct iphdr *iph = NULL;
	int offset = 0;
	skb_reset_transport_header(skb);

	if (PpeParseResult.iph.protocol == IPPROTO_TCP) 
	{
		if (TCSUPPORT_XPON_HAL_API_EXT_VAL == 1)
		{
			if(isLanTcpBlackList(skb, magic, pinfo))
			{
				HWNAT_BINDFAIL_DPRINT("isLanTcpBlackList fail\n");
				return 1;
			}
		}

		th = (struct tcphdr *)(skb->data);
		
		if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting){
				ipv4TcpPkttypeMark(&PpeParseResult.iph, th, skb);
			}
		
		memcpy(&PpeParseResult.th, th, sizeof(struct tcphdr));
		PpeParseResult.pkt_type = IPV4_HNAPT;
	}
	else if(PpeParseResult.iph.protocol == IPPROTO_UDP)
	{
		uh = (struct udphdr *)(skb->data);
		memcpy(&PpeParseResult.uh, uh, sizeof(struct udphdr));
		PpeParseResult.pkt_type = IPV4_HNAPT;
		if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
			pkttype |= DS_UDP;	


		if(SUPPORT_TUNNEL_HW_ACCELE)
		{
			if(PpeCheckVxlanTunnel(uh->dest, magic, foe_entry)){
			
				if(PpeParseVXLANTunnelInfo(skb,pinfo,magic))
				{
					HWNAT_BINDFAIL_DPRINT("parse gre tunnel info fail\n");
					return 1;
				}
		
			}


			if(PpeCheckL2tpTunnel(uh->dest, uh->source, magic, foe_entry))
			{
				if(PpeParseL2TPTunnelInfo(skb,pinfo,magic))
				{
					HWNAT_BINDFAIL_DPRINT("parse L2tp tunnel info fail\n");
					return 1;
				}
			}

		}else{
	        if(uh->dest == htons(VXLAN_DEFAULT_UDP_PORT))
	            PpeParseResult.vxlan_vni = (ntohl(*(int*)((uint8_t *)uh+8+4)) >> 8) & 0x00FFFFFF;
		}
	}
	else if ((ip_proto_chk == WHITE_LIST) && ((PpeParseResult.iph.protocol == IPPROTO_GRE) || (PpeParseResult.iph.protocol == IPPROTO_ESP)))
	{
			PpeParseResult.pkt_type = IPV4_HNAT;
			/* do nothing */
	}
	else if (PpeParseResult.iph.protocol == IPPROTO_IPV6)
	{
		ip6h = (struct ipv6hdr *)(skb->data);
		memcpy(&PpeParseResult.ip6h, ip6h, sizeof(struct ipv6hdr));

		if(PpeParse6rdInfo(skb,pinfo,magic))
		{
			HWNAT_BINDFAIL_DPRINT("parse 6rd fail\n");
			return 1;
		}
	}
	else 
	{
		if(SUPPORT_TUNNEL_HW_ACCELE){
			if(PpeParseResult.iph.protocol == IPPROTO_GRE)
	        {
				if(PpeParseGreTunnelInfo(skb,pinfo,magic))
				{
					HWNAT_BINDFAIL_DPRINT("parse gre tunnel info fail\n");
					return 1;
				}
			}

		}else{

	
	        if(PpeCheckGreTunnel(PpeParseResult.iph.protocol, magic, foe_entry))
	        {
	            struct tnl_ptk_info *gre_header;
				iph = (struct iphdr *)LAYER3_HEADER(skb);
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
	            offset = ((uint8_t *) iph + iph->ihl * 4) - skb->data;
	            skb_set_transport_header(skb,offset);
#else
	            LAYER4_HEADER(skb) = ((uint8_t *) iph + iph->ihl * 4);
#endif
	            gre_header = (struct tnl_ptk_info *)LAYER4_HEADER(skb);
	            PpeParseResult.gre_call_id = ntohl(gre_header->key) & 0xFFFF;
	        }
		}

		if(ip_proto_chk == BLACK_LIST)
			PpeParseResult.pkt_type = IPV4_HNAT;
		else
		{
			/* Packet format is not supported */
			HWNAT_BINDFAIL_DPRINT("Packet format is not supported\n");
			return 1;
		}
	}
	
	return 0;
}

int PpeParseL4V6Info(struct sk_buff * skb, struct port_info * pinfo, int magic, struct FoeEntry *foe_entry)
{
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;
	struct iphdr *iph = NULL;	
	
	if (PpeParseResult.ip6h.nexthdr == NEXTHDR_TCP) 
	{
		if (TCSUPPORT_XPON_HAL_API_EXT_VAL == 1)
		{
			if(isLanTcpBlackList(skb, magic, pinfo))
			{
				HWNAT_BINDFAIL_DPRINT("isLanTcpBlackList fail\n");
				return 1;
			}
		}

		th = (struct tcphdr *)(skb->data);
		memcpy(&PpeParseResult.th, th, sizeof(struct tcphdr));
		PpeParseResult.pkt_type = IPV6_5T_ROUTE;
		
		if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
			ipv6TcpPkttypeMark(th);
	} 
	else if (PpeParseResult.ip6h.nexthdr == NEXTHDR_UDP) 
	{
		uh = (struct udphdr *)(skb->data);
		memcpy(&PpeParseResult.uh, uh, sizeof(struct udphdr));
		PpeParseResult.pkt_type = IPV6_5T_ROUTE;

        if(SUPPORT_TUNNEL_HW_ACCELE)
		{
			if(PpeCheckVxlanTunnel(uh->dest, magic, foe_entry)){
				if(PpeParseVXLANTunnelInfo(skb,pinfo,magic))
				{
					HWNAT_BINDFAIL_DPRINT("parse vxlan tunnel info fail\n");
					return 1;
				}
		
			}


			if(PpeCheckL2tpTunnel(uh->dest, uh->source, magic, foe_entry))
			{
				if(PpeParseL2TPTunnelInfo(skb,pinfo,magic))
				{
					HWNAT_BINDFAIL_DPRINT("parse L2tp tunnel info fail\n");
					return 1;
				}
			}

		}else{
	        if(uh->dest == htons(VXLAN_DEFAULT_UDP_PORT))
	            PpeParseResult.vxlan_vni = (ntohl(*(int*)((uint8_t *)uh+8+4)) >> 8) & 0x00FFFFFF;
		}

		
		if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
			pkttype |= DS_UDP;
	}
	else if (PpeParseResult.ip6h.nexthdr == NEXTHDR_IPIP) 
	{
		iph = (struct iphdr *)(skb->data);

		memcpy(&PpeParseResult.iph, iph, sizeof(struct iphdr));
		if(iph->protocol == IPPROTO_TCP)
		{
			memcpy(&PpeParseResult.th, skb->data+iph->ihl*4, sizeof(struct tcphdr));
			PpeParseResult.iph.protocol = IPPROTO_TCP;
		}
		else if(iph->protocol == IPPROTO_UDP)
		{
			memcpy(&PpeParseResult.uh, skb->data+iph->ihl*4, sizeof(struct udphdr));
			PpeParseResult.iph.protocol = IPPROTO_UDP;
		}

		PpeParseResult.pkt_type = IPV4_DSLITE;
	}
	else 
	{
		PpeParseResult.pkt_type = IPV6_3T_ROUTE;
	}
		
	return 0;
}

int PpeParseL3AndL4Info(struct sk_buff * skb, struct port_info * pinfo, int magic,struct FoeEntry *foe_entry)
{	
	struct iphdr *iph = NULL;	
	struct ipv6hdr *ip6h = NULL;
	
	skb_reset_network_header(skb);

	if(IS_L2_RRIDGE(foe_entry))
	{
		PpeParseResult.pkt_type = L2_BRIDGE;
		return 0;
	}

	if ((PpeParseResult.eth_type == htons(ETH_P_IP)) || (PpeParseResult.eth_type == htons(ETH_P_PPP_SES)
		&& PpeParseResult.ppp_tag == htons(PPP_IP))) 
	{		
		PpeParseResult.pkt_type = IPV4_HNAT;
		
		iph = (struct iphdr *)(skb->data);
				
        if ( (DISABLE_SPECIAL_CRSN_FOR_IFC) 
                && ((5 != iph->ihl) || (ntohs(iph->frag_off) & IP_MF) || (ntohs(iph->frag_off) & IP_OFFSET)) )
        {
            return 1;
        }
        
		memcpy(&PpeParseResult.iph, iph, sizeof(struct iphdr));

		skb->data += (iph->ihl * 4);

		if(PpeParseL4V4Info(skb,pinfo,magic,foe_entry))
		{
			HWNAT_BINDFAIL_DPRINT("parse L4 V4 fail\n");
			return 1;
		}
	}
	else if (PpeParseResult.eth_type == htons(ETH_P_IPV6) || (PpeParseResult.eth_type == htons(ETH_P_PPP_SES)
		        &&PpeParseResult.ppp_tag == htons(PPP_IPV6)))
	{		
		PpeParseResult.pkt_type = IPV6_3T_ROUTE;
		
		ip6h = (struct ipv6hdr *)(skb->data);
        if( (DISABLE_SPECIAL_CRSN_FOR_IFC)
                && ((NEXTHDR_HOP == ip6h->nexthdr) || (NEXTHDR_ROUTING == ip6h->nexthdr) 
                        || (NEXTHDR_FRAGMENT == ip6h->nexthdr) || (NEXTHDR_ESP == ip6h->nexthdr) 
                        || (NEXTHDR_AUTH == ip6h->nexthdr)     || (NEXTHDR_DEST == ip6h->nexthdr)) )
        {
            return 1;
        }
        
		memcpy(&PpeParseResult.ip6h, ip6h, sizeof(struct ipv6hdr));
		
		skb->data += sizeof(struct ipv6hdr);

		if(PpeParseL4V6Info(skb,pinfo,magic,foe_entry))
		{
			HWNAT_BINDFAIL_DPRINT("parse L4 V6 fail\n");
			return 1;
		}
	}
	else
	{
		HWNAT_BINDFAIL_DPRINT("htons(PpeParseResult.eth_type) = 0x%x,htons(PpeParseResult.ppp_tag) = 0x%x\n",htons(PpeParseResult.eth_type),htons(PpeParseResult.ppp_tag));
		return 1;
	}

	return 0;
}




int PpeParseLayerInfoDebug_InTunnel(void)
{
	if (hwnat_dbg_parse_dump >= 1 && SUPPORT_TUNNEL_HW_ACCELE) {
		printk("-------InTunnel Info--------\n");
		printk("DMAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
			   PpeParseResult.dmac_intunnel[0], PpeParseResult.dmac_intunnel[1],
			   PpeParseResult.dmac_intunnel[2], PpeParseResult.dmac_intunnel[3],
			   PpeParseResult.dmac_intunnel[4], PpeParseResult.dmac_intunnel[5]);
		printk("SMAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
			   PpeParseResult.smac_intunnel[0], PpeParseResult.smac_intunnel[1],
			   PpeParseResult.smac_intunnel[2], PpeParseResult.smac_intunnel[3],
			   PpeParseResult.smac_intunnel[4], PpeParseResult.smac_intunnel[5]);
		printk("Eth_Type=%x\n", PpeParseResult.eth_type_intunnel);
		if (PpeParseResult.vlan1_gap_intunnel> 0) {
			printk("VLAN1 ID=%x\n", ntohs(PpeParseResult.vlan1_intunnel));
		}

		if (PpeParseResult.vlan2_gap_intunnel> 0) {
			printk("VLAN2 ID=%x\n", ntohs(PpeParseResult.vlan2_intunnel));
		}

		if (PpeParseResult.pppoe_gap_intunnel> 0) {
			printk("PPPOE Session ID=%x\n",
				   PpeParseResult.pppoe_sid_intunnel);
			printk("PPP Tag=%x\n", ntohs(PpeParseResult.ppp_tag_intunnel));
		}
		printk("PKT_TYPE=%s\n",
			   PpeParseResult.pkt_type_intunnel==
			   L2_BRIDGE? "L2_BRIDGE" : PpeParseResult.pkt_type_intunnel ==
			   IPV4_HNAT ? "IPV4_HNAT" : PpeParseResult.pkt_type_intunnel ==
			   IPV4_HNAPT ? "IPV4_HNAPT" : PpeParseResult.pkt_type_intunnel ==
			   IPV4_DSLITE? "IPV4_DSLITE" : PpeParseResult.pkt_type_intunnel ==
			   IPV6_3T_ROUTE? "IPV6_3T_ROUTE" : PpeParseResult.pkt_type_intunnel ==
			   IPV6_5T_ROUTE? "IPV6_5T_ROUTE" : PpeParseResult.pkt_type_intunnel ==
			   IPV6_6RD? "IPV6_6RD" : "Unknown");



		if (PpeParseResult.pkt_type_intunnel == IPV4_HNAT) {
			printk("SIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph_intunnel.saddr)));
			printk("DIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph_intunnel.daddr)));
			printk("TOS=%x\n", ntohs(PpeParseResult.iph_intunnel.tos));
		} else if (PpeParseResult.pkt_type_intunnel == IPV4_HNAPT) {
			printk("SIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph_intunnel.saddr)));
			printk("DIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph_intunnel.daddr)));
			printk("TOS=%x\n", ntohs(PpeParseResult.iph_intunnel.tos));
			
			if (PpeParseResult.iph_intunnel.protocol == IPPROTO_TCP) {
				printk("TCP SPORT=%d\n", ntohs(PpeParseResult.th_intunnel.source));
				printk("TCP DPORT=%d\n", ntohs(PpeParseResult.th_intunnel.dest));
			}else if(PpeParseResult.iph_intunnel.protocol == IPPROTO_UDP) {
				printk("UDP SPORT=%d\n", ntohs(PpeParseResult.uh_intunnel.source));
				printk("UDP DPORT=%d\n", ntohs(PpeParseResult.uh_intunnel.dest));
			}
		}

	}
		
	return 0;
}



int PpeParseLayerInfoDebug(struct sk_buff* skb)
{
	if (hwnat_dbg_parse_dump >= 1) {
		printk("--------------\n");
		printk("DMAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
			   PpeParseResult.dmac[0], PpeParseResult.dmac[1],
			   PpeParseResult.dmac[2], PpeParseResult.dmac[3],
			   PpeParseResult.dmac[4], PpeParseResult.dmac[5]);
		printk("SMAC:%02X:%02X:%02X:%02X:%02X:%02X\n",
			   PpeParseResult.smac[0], PpeParseResult.smac[1],
			   PpeParseResult.smac[2], PpeParseResult.smac[3],
			   PpeParseResult.smac[4], PpeParseResult.smac[5]);
		printk("Eth_Type=%x\n", PpeParseResult.eth_type);
		if (PpeParseResult.vlan1_gap > 0) {
			printk("VLAN1 ID=%x\n", ntohs(PpeParseResult.vlan1));
		}

		if (PpeParseResult.vlan2_gap > 0) {
			printk("VLAN2 ID=%x\n", ntohs(PpeParseResult.vlan2));
		}

		if (PpeParseResult.pppoe_gap > 0) {
			printk("PPPOE Session ID=%x\n",
				   PpeParseResult.pppoe_sid);
			printk("PPP Tag=%x\n", ntohs(PpeParseResult.ppp_tag));
		}
		printk("PKT_TYPE=%s\n",
			   PpeParseResult.pkt_type ==
			   L2_BRIDGE? "L2_BRIDGE" : PpeParseResult.pkt_type ==
			   IPV4_HNAT ? "IPV4_HNAT" : PpeParseResult.pkt_type ==
			   IPV4_HNAPT ? "IPV4_HNAPT" : PpeParseResult.pkt_type ==
			   IPV4_DSLITE? "IPV4_DSLITE" : PpeParseResult.pkt_type ==
			   IPV6_3T_ROUTE? "IPV6_3T_ROUTE" : PpeParseResult.pkt_type ==
			   IPV6_5T_ROUTE? "IPV6_5T_ROUTE" : PpeParseResult.pkt_type ==
			   IPV6_6RD? "IPV6_6RD" : "Unknown");



		if (PpeParseResult.pkt_type == IPV4_HNAT) {
			printk("SIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph.saddr)));
			printk("DIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph.daddr)));
			printk("TOS=%x\n", ntohs(PpeParseResult.iph.tos));
		} else if (PpeParseResult.pkt_type == IPV4_HNAPT) {
			printk("SIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph.saddr)));
			printk("DIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph.daddr)));
			printk("TOS=%x\n", ntohs(PpeParseResult.iph.tos));
			
			if (PpeParseResult.iph.protocol == IPPROTO_TCP) {
				printk("TCP SPORT=%d\n", ntohs(PpeParseResult.th.source));
				printk("TCP DPORT=%d\n", ntohs(PpeParseResult.th.dest));
			}else if(PpeParseResult.iph.protocol == IPPROTO_UDP) {
				printk("UDP SPORT=%d\n", ntohs(PpeParseResult.uh.source));
				printk("UDP DPORT=%d\n", ntohs(PpeParseResult.uh.dest));
			}
		}
		else if (PpeParseResult.pkt_type == IPV6_6RD) {
			/* fill in ipv4 6rd entry */
			printk("SIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph.saddr)));
			printk("DIP=%s\n",
				   Ip2Str(ntohl(PpeParseResult.iph.daddr)));
			printk("Checksum=%x\n",
				   ntohs(PpeParseResult.iph.check));
			printk("Flag=%x\n", ntohs(PpeParseResult.iph.frag_off) >> 13);
			printk("TTL=%x\n", PpeParseResult.iph.ttl);
			printk("TOS=%x\n", PpeParseResult.iph.tos);
		}
	}
		
	return 0;
}

int32_t PpeParseLayerInfo(struct sk_buff * skb, struct port_info * pinfo, int magic,struct FoeEntry *foe_entry)
{	
	memset(&PpeParseResult, 0, sizeof(PpeParseResult));

	if(hwnat_dbg_parse_dump >= 2)
		dump_skb(skb);

	if(PpeParseL2Info(skb,pinfo,magic))
	{
		skb->data = skb_mac_header(skb);
		HWNAT_BINDFAIL_DPRINT("parse L2 layer info fail\n");
		return 1;
	}

	if(hwnat_dbg_parse_dump >= 2)
	{
		printk("parse L3&L4\n");
		dump_skb(skb);
	}

	if(PpeParseL3AndL4Info(skb,pinfo,magic,foe_entry))
	{		
		skb->data = skb_mac_header(skb);
		HWNAT_BINDFAIL_DPRINT("parse L3/L4 layer info fail\n");
		return 1;
	}

	PpeParseLayerInfoDebug(skb);

	PpeParseLayerInfoDebug_InTunnel();

	skb->data = skb_mac_header(skb);
	return 0;
}

void PpeClearEntryInfo(struct FoeEntry *foe_entry)
{
	char *foe_entry_point = (char *)foe_entry;

	if ((PPE_L2B_ENTRY_SUPPORT) && IS_L2_RRIDGE(foe_entry))
	{	
		memset(foe_entry_point+PPE_CLEAR_OFFSET4, 0, SIZE_OF_FOE_ENTRY-PPE_CLEAR_OFFSET4);
	}
	else if (SUPPORT_DSLITE_HSK_16B && IS_IPV4_DSLITE(foe_entry))
	{	
		memset(foe_entry_point+PPE_CLEAR_OFFSET1, 0, SIZE_OF_FOE_ENTRY-PPE_CLEAR_OFFSET4);
	}
	else if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry) || IS_L2_RRIDGE(foe_entry))
	{	
		memset(foe_entry_point+PPE_CLEAR_OFFSET1, 0, SIZE_OF_FOE_ENTRY-PPE_CLEAR_OFFSET1);
	}
	else if(IS_IPV6_3T_ROUTE(foe_entry) || IS_IPV6_5T_ROUTE(foe_entry))
	{
		memset(foe_entry_point+PPE_CLEAR_OFFSET2, 0, SIZE_OF_FOE_ENTRY-PPE_CLEAR_OFFSET2);
	}
	else if(IS_IPV4_DSLITE(foe_entry) || IS_IPV6_6RD(foe_entry))
	{
        memset(foe_entry_point+PPE_CLEAR_OFFSET3, 0, SIZE_OF_FOE_ENTRY-PPE_CLEAR_OFFSET3);
	}
	else
	{
		printk("HNAT: unknow packet type\n");
	}
}

u_int16_t PpeGetAcntGrpIdFromEntry(struct FoeEntry * foe_entry)
{
    _info_blk2 *iblk2;

    if (IS_IPV4_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
    else if (IS_IPV6_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
    else if (IS_L2_RRIDGE(foe_entry))
	iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
    else
        return HWNAT_DEFAULT_GRP_ID;

    return iblk2->port_ag;
}

void PpeSetAcntGrpId(struct FoeEntry * foe_entry, u_int16_t acnt_grp_index)
{
    _info_blk2 *iblk2;

    if (IS_IPV4_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
    else if (IS_IPV6_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
    else if (IS_L2_RRIDGE(foe_entry))
	iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
    else
        return;

    iblk2->port_ag = acnt_grp_index;

    return;
}




void PpeSetTunnel_Info(struct FoeEntry * foe_entry)
{
	if(!SUPPORT_TUNNEL_HW_ACCELE)
		return;

	if(!PpeParseResult.tunnel_en)
	{
		return;
	}
	
	if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry)){
		set_ppe_entry_tunnel_info(foe_entry,IPV4_HNAPT, PpeParseResult.tunnel_en, PpeParseResult.tunnel_id);
		set_ppe_entry_hop_info(foe_entry,IPV4_HNAPT, PpeParseResult.hop_0, PpeParseResult.hop_1,PpeParseResult.hop_2,PpeParseResult.hop_3);
	}else if(IS_L2_RRIDGE(foe_entry)){
		set_ppe_entry_tunnel_info(foe_entry,L2_BRIDGE, PpeParseResult.tunnel_en, PpeParseResult.tunnel_id);
		set_ppe_entry_hop_info(foe_entry,L2_BRIDGE, PpeParseResult.hop_0, PpeParseResult.hop_1,PpeParseResult.hop_2,PpeParseResult.hop_3);
	}else if(IS_IPV6_3T_ROUTE(foe_entry))
	{
		set_ppe_entry_tunnel_info(foe_entry,IPV6_3T_ROUTE, PpeParseResult.tunnel_en, PpeParseResult.tunnel_id);
		set_ppe_entry_hop_info(foe_entry,IPV6_3T_ROUTE, PpeParseResult.hop_0, PpeParseResult.hop_1,PpeParseResult.hop_2,PpeParseResult.hop_3);
	}else if(IS_IPV6_5T_ROUTE(foe_entry)){
		set_ppe_entry_tunnel_info(foe_entry,IPV6_5T_ROUTE, PpeParseResult.tunnel_en, PpeParseResult.tunnel_id);
		set_ppe_entry_hop_info(foe_entry,IPV6_5T_ROUTE, PpeParseResult.hop_0, PpeParseResult.hop_1,PpeParseResult.hop_2,PpeParseResult.hop_3);
	}

    return;
}


int32_t PpeFillInL2Info(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	int i=0, index=0;
	struct hwnat_shrink_field shrinkField;
	u_int16_t acnt_grp_index = HWNAT_DEFAULT_GRP_ID;

	//if this entry is already in binding state, skip it 
	if (foe_entry->bfib1.state == BIND) {
		HWNAT_BINDFAIL_DPRINT("entry is already bind\n");
		return 1;
	}
	//Clear Entry software info when binding the entry.
	if(TCSUPPORT_HWNAT_WAN_ACCOUNT_VAL)
		acnt_grp_index = PpeGetAcntGrpIdFromEntry(foe_entry);
	PpeClearEntryInfo(foe_entry);
	if(TCSUPPORT_HWNAT_WAN_ACCOUNT_VAL)
		PpeSetAcntGrpId(foe_entry, acnt_grp_index);

	/* Set VLAN Info - VLAN1/VLAN2 */
	/* Set Layer2 Info - DMAC, SMAC */
	if ((PpeParseResult.pkt_type == IPV4_HNAT) || (PpeParseResult.pkt_type == IPV4_HNAPT)) {
		if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE) { //DS-Lite WAN->LAN
			FoeSetMacInfo(foe_entry->ipv4_dslite.smac_hi, PpeParseResult.smac);
			FoeSetMacInfo(foe_entry->ipv4_dslite.dmac_hi, PpeParseResult.dmac);
			foe_entry->ipv4_dslite.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv4_dslite.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
			foe_entry->ipv4_dslite.vlan2 = ntohs(PpeParseResult.vlan2);
			foe_entry->ipv4_dslite.etype = ntohs(PpeParseResult.vlan_tag);
		}else { //IPv4 WAN<->LAN
			FoeSetMacInfo(foe_entry->ipv4_hnapt.dmac_hi, PpeParseResult.dmac);
			FoeSetMacInfo(foe_entry->ipv4_hnapt.smac_hi, PpeParseResult.smac);
			foe_entry->ipv4_hnapt.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv4_hnapt.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
			foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan2);
			foe_entry->ipv4_hnapt.etype = ntohs(PpeParseResult.vlan_tag);
		}
	} 
	else if(PpeParseResult.pkt_type == L2_BRIDGE){
			foe_entry->l2_bridge32.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->l2_bridge32.vlan2 = ntohs(PpeParseResult.vlan2);       
			foe_entry->l2_bridge32.etype = ntohs(PpeParseResult.vlan_tag);
			if (TCSUPPORT_XPON_HAL_API_EXT_VAL == 1) {
				if (0 != memcmp(foe_entry->l2_bridge32.in_dmac_hi, PpeParseResult.dmac, 6))
				{
					HWNAT_BINDFAIL_DPRINT("check dmac fail\n");
					return 1;		
				}
			}
	}
	else {
		if((SUPPORT_FOE_SMAC_CFG) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE) && (PpeParseResult.pkt_type != IPV4_DSLITE)) {
			// if smac doesn't change,then smac_idx[4]=1; else, smac_idx[3:0] =0~15
			if(cmpMacInfo(PpeParseResult.smac, skb->smac) == HWNAT_SUCCESS) {
				set_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE, 0x10);
			} else {
				//shrinkField.smac[1] = (PpeParseResult.smac[5]<<8) | PpeParseResult.smac[4];
				//shrinkField.smac[0] = (PpeParseResult.smac[3]<<24) | (PpeParseResult.smac[2]<<16)
				//					| (PpeParseResult.smac[1]<<8) | PpeParseResult.smac[0];
				for(i=0; i<UPDMEM_SMAC_CNT; i++)
					shrinkField.smac[i] = PpeParseResult.smac[i];
				index = find_and_update_shrink_table(PPE_UPDMEM_SEL_SMAC, &shrinkField);
				if(index != -1) {
					set_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE, index);
				} else {
					printk("smac: find and update shrink table failed!\n");
				}
			}
			FoeSetMacInfo(foe_entry->ipv6_5t_route.dmac_hi, PpeParseResult.dmac);
		}
		else
		{
			FoeSetMacInfo(foe_entry->ipv4_dslite.dmac_hi, PpeParseResult.dmac);
			FoeSetMacInfo(foe_entry->ipv4_dslite.smac_hi, PpeParseResult.smac);
		}
        
		if((SUPPORT_L2_INFO_CFG) && (PpeParseResult.pkt_type == IPV4_DSLITE)) {
			foe_entry->ipv4_dslite.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv4_dslite.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
			foe_entry->ipv4_dslite.vlan2 = ntohs(PpeParseResult.vlan2);
			foe_entry->ipv4_dslite.etype = ntohs(PpeParseResult.vlan_tag);
		} else {
			foe_entry->ipv6_5t_route.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv6_5t_route.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
			foe_entry->ipv6_5t_route.vlan2 = ntohs(PpeParseResult.vlan2);
			foe_entry->ipv6_5t_route.etype = ntohs(PpeParseResult.vlan_tag);
		}
	}

	return 0;
}



int32_t PpeFillInL2Info_InTunnel(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	int i=0, index=0;
	struct hwnat_shrink_field shrinkField;

	//if this entry is already in binding state, skip it 
	if (foe_entry->bfib1.state == BIND) {
		HWNAT_BINDFAIL_DPRINT("entry is already bind\n");
		return 1;
	}
	//Clear Entry software info when binding the entry.

	PpeClearEntryInfo(foe_entry);

	/*layer 2 tunnel info set inner mac header */
	/*layer 3 tunnel info set out mac header */
	if(PpeParseResult.is_L3_tunnel){
		if ((PpeParseResult.pkt_type_intunnel == IPV4_HNAT) || (PpeParseResult.pkt_type_intunnel == IPV4_HNAPT)) {
			FoeSetMacInfo(foe_entry->ipv4_hnapt.dmac_hi, PpeParseResult.dmac);
			FoeSetMacInfo(foe_entry->ipv4_hnapt.smac_hi, PpeParseResult.smac);
			foe_entry->ipv4_hnapt.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv4_hnapt.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
			foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan2);
			foe_entry->ipv4_hnapt.etype = ntohs(PpeParseResult.vlan_tag);
		} 
		else if(PpeParseResult.pkt_type_intunnel == L2_BRIDGE){
				foe_entry->l2_bridge32.vlan1 = ntohs(PpeParseResult.vlan1);
				foe_entry->l2_bridge32.vlan2 = ntohs(PpeParseResult.vlan2);       
				foe_entry->l2_bridge32.etype = ntohs(PpeParseResult.vlan_tag);
		}
		else {

			if((SUPPORT_FOE_SMAC_CFG) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE) && (PpeParseResult.pkt_type != IPV4_DSLITE)) {
			// if smac doesn't change,then smac_idx[4]=1; else, smac_idx[3:0] =0~15
			if(cmpMacInfo(PpeParseResult.smac, skb->smac) == HWNAT_SUCCESS) {
				set_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE, 0x10);
			} else {
				//shrinkField.smac[1] = (PpeParseResult.smac[5]<<8) | PpeParseResult.smac[4];
				//shrinkField.smac[0] = (PpeParseResult.smac[3]<<24) | (PpeParseResult.smac[2]<<16)
				//					| (PpeParseResult.smac[1]<<8) | PpeParseResult.smac[0];
				for(i=0; i<UPDMEM_SMAC_CNT; i++)
					shrinkField.smac[i] = PpeParseResult.smac[i];
				index = find_and_update_shrink_table(PPE_UPDMEM_SEL_SMAC, &shrinkField);
				if(index != -1) {
					set_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE, index);
				} else {
					printk("smac: find and update shrink table failed!\n");
				}
			}
				FoeSetMacInfo(foe_entry->ipv6_5t_route.dmac_hi, PpeParseResult.dmac);
			}
			else
			{
				return 1;
			}

			if((SUPPORT_L2_INFO_CFG) && (PpeParseResult.pkt_type_intunnel == IPV4_DSLITE)) {
				return 1;
			} else {
				foe_entry->ipv6_5t_route.vlan1 = ntohs(PpeParseResult.vlan1);
				foe_entry->ipv6_5t_route.pppoe_id = ntohs(PpeParseResult.pppoe_sid);
				foe_entry->ipv6_5t_route.vlan2 = ntohs(PpeParseResult.vlan2);
				foe_entry->ipv6_5t_route.etype = ntohs(PpeParseResult.vlan_tag);
			}

		}


	}else{
		if ((PpeParseResult.pkt_type_intunnel== IPV4_HNAT) || (PpeParseResult.pkt_type_intunnel== IPV4_HNAPT)) {
			FoeSetMacInfo(foe_entry->ipv4_hnapt.dmac_hi, PpeParseResult.dmac_intunnel);
			FoeSetMacInfo(foe_entry->ipv4_hnapt.smac_hi, PpeParseResult.smac_intunnel);
			foe_entry->ipv4_hnapt.vlan1 = ntohs(PpeParseResult.vlan1_intunnel);
			foe_entry->ipv4_hnapt.pppoe_id = ntohs(PpeParseResult.pppoe_sid_intunnel);
			foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan2_intunnel);
			foe_entry->ipv4_hnapt.etype = ntohs(PpeParseResult.vlan_tag_intunnel);
		} 
		else if(PpeParseResult.pkt_type_intunnel == L2_BRIDGE){
				foe_entry->l2_bridge32.vlan1 = ntohs(PpeParseResult.vlan1_intunnel);
				foe_entry->l2_bridge32.vlan2 = ntohs(PpeParseResult.vlan2_intunnel);       
				foe_entry->l2_bridge32.etype = ntohs(PpeParseResult.vlan_tag_intunnel);
		}
		else {
			if((SUPPORT_FOE_SMAC_CFG) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE) && (PpeParseResult.pkt_type != IPV4_DSLITE)) {
			// if smac doesn't change,then smac_idx[4]=1; else, smac_idx[3:0] =0~15
			if(cmpMacInfo(PpeParseResult.smac_intunnel, skb->smac) == HWNAT_SUCCESS) {
				set_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE, 0x10);
			} else {
				//shrinkField.smac[1] = (PpeParseResult.smac[5]<<8) | PpeParseResult.smac[4];
				//shrinkField.smac[0] = (PpeParseResult.smac[3]<<24) | (PpeParseResult.smac[2]<<16)
				//					| (PpeParseResult.smac[1]<<8) | PpeParseResult.smac[0];
				for(i=0; i<UPDMEM_SMAC_CNT; i++)
					shrinkField.smac[i] = PpeParseResult.smac_intunnel[i];
				index = find_and_update_shrink_table(PPE_UPDMEM_SEL_SMAC, &shrinkField);
				if(index != -1) {
					set_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE, index);
				} else {
					printk("smac: find and update shrink table failed!\n");
				}
			}
				FoeSetMacInfo(foe_entry->ipv6_5t_route.dmac_hi, PpeParseResult.dmac_intunnel);
			}
			else
			{
				return 1;
			}

			if((SUPPORT_L2_INFO_CFG) && (PpeParseResult.pkt_type_intunnel == IPV4_DSLITE)) {
				return 1;
			} else {
				foe_entry->ipv6_5t_route.vlan1 = ntohs(PpeParseResult.vlan1_intunnel);
				foe_entry->ipv6_5t_route.pppoe_id = ntohs(PpeParseResult.pppoe_sid_intunnel);
				foe_entry->ipv6_5t_route.vlan2 = ntohs(PpeParseResult.vlan2_intunnel);
				foe_entry->ipv6_5t_route.etype = ntohs(PpeParseResult.vlan_tag_intunnel);
			}

		}


	}



	return 0;
}


static uint16_t PpeGetChkBase(struct iphdr *iph)
{
	uint16_t org_chksum = ntohs(iph->check);
	uint16_t org_tot_len = ntohs(iph->tot_len);
	uint16_t org_id = ntohs(iph->id);
	uint32_t tmp = 0;
	uint16_t chksum_base = 0;

	tmp = (~org_chksum&0xFFFF) + (~org_tot_len&0XFFFF);
	tmp = (tmp >> 16) + (tmp & 0xFFFF);
	tmp += (~org_id&0XFFFF);
	tmp = (tmp >> 16) + (tmp & 0xFFFF);
	chksum_base = tmp & 0xFFFF;

	return chksum_base;
}
void PpeGemportCheck(struct sk_buff * skb, struct FoeEntry *foe_entry)
{
    int idx = FOE_ENTRY_NUM(skb);
    
    if(skb->gem_port != foe_ext[idx].gemport){
		
        printk("Gemport changed, delete the original entry idx  %d  new gem %d, store gem %d \n",
			idx, skb->gem_port, foe_ext[idx].gemport);
        foe_ext[idx].gemport = -1;
        hwnat_delete_foe_entry(idx);
    }
	return;
}

int32_t PpeFillInL3Info(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	int index=0;
	struct hwnat_shrink_field shrinkField;
	struct neighbour *n = NULL;	
	struct net_device *dev_out = NULL;
	uint32 dip_be;
	
	PpeParseResult.rmt = 0;

	
	/* IPv4 or IPv4 over PPPoE */
	if ((PpeParseResult.eth_type == htons(ETH_P_IP)) || 
		(PpeParseResult.eth_type == htons(ETH_P_PPP_SES) && 
		 PpeParseResult.ppp_tag == htons(PPP_IP))) {

		if ((PpeParseResult.pkt_type == IPV4_HNAT) || (PpeParseResult.pkt_type == IPV4_HNAPT)) {

			if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE) { //DS-Lite WAN->LAN
				PpeParseResult.rmt = 1;	
				foe_entry->ipv4_dslite.dscp = PpeParseResult.iph.tos;
				if(foe_entry->ipv4_dslite.dip != ntohl(PpeParseResult.iph.daddr)){
					PpeParseResult.map_e = 1;
				}

			} else {
				foe_entry->ipv4_hnapt.new_sip = ntohl(PpeParseResult.iph.saddr);
				foe_entry->ipv4_hnapt.new_dip = ntohl(PpeParseResult.iph.daddr);
				foe_entry->ipv4_hnapt.dscp = PpeParseResult.iph.tos;
			}
            if(xpon_is_multicast_entry(foe_entry))
            {
                if( (foe_entry->bfib1.pkt_type >= 2)
                     && (foe_entry->bfib1.pkt_type != IPV4_DSLITE) )
                {
                    return 1;
                }
                if ( (foe_entry->bfib1.pkt_type != IPV4_DSLITE)
                       && ((foe_entry->ipv4_hnapt.new_sip != foe_entry->ipv4_hnapt.sip)
                            || (foe_entry->ipv4_hnapt.new_dip != foe_entry->ipv4_hnapt.dip)) )
                {
                    return 1;
                }
            }
			
			if(TCSUPPORT_TEST_WLAN_SHORTCUT_VAL) {
				if(recover_info_flag && foe_entry->bfib1.pkt_type != IPV4_DSLITE) {
					if(foe_entry->ipv4_hnapt.new_sip != foe_entry->ipv4_hnapt.sip) {
						printk("bind index =%d\n", FOE_ENTRY_NUM(skb));
						printk("new SIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.new_sip)); 
						printk("old SIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.sip));  					
						foe_entry->ipv4_hnapt.new_sip = foe_entry->ipv4_hnapt.sip;
					}
					if(foe_entry->ipv4_hnapt.new_dip != foe_entry->ipv4_hnapt.dip) {
						printk("new DIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.new_dip)); 
						if(mulif_get_dev_by_source_mac_hook && mulif_get_dev_by_source_mac_hook(skb, &dev_out) == 0){
							if (dev_out) {
								dip_be = htonl(foe_entry->ipv4_hnapt.dip);
								n = neigh_lookup(&arp_tbl, &dip_be, dev_out);
								if (n && n->nud_state & NUD_VALID) {
									FoeSetMacInfo(foe_entry->ipv4_hnapt.dmac_hi, n->ha);
									printk("DMAC=%02X:%02X:%02X:%02X:%02X:%02X \n",
										 NIPQUAD(*(unsigned int *)(&foe_entry->ipv4_hnapt.dmac_hi[0])), 
										 NIPTWOBYTE(*(unsigned short *)(&foe_entry->ipv4_hnapt.dmac_lo[0])));
								}
							}
						}	
						printk("old DIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.dip)); 
						foe_entry->ipv4_hnapt.new_dip = foe_entry->ipv4_hnapt.dip;						
					}
				}
			}		
			if(xpon_is_multicast_entry(foe_entry))
			{
				if( (foe_entry->bfib1.pkt_type >= 2)
				     && (foe_entry->bfib1.pkt_type != IPV4_DSLITE) )
				{
				     return 1;
				}
				if ( (foe_entry->bfib1.pkt_type != IPV4_DSLITE)
				       && ((foe_entry->ipv4_hnapt.new_sip != foe_entry->ipv4_hnapt.sip)
				            || (foe_entry->ipv4_hnapt.new_dip != foe_entry->ipv4_hnapt.dip)) )
				{
				     return 1;
				}
			}
		}
		else if (PpeParseResult.pkt_type == IPV6_6RD) {
            if((PPE_SHNK_TABLE_SUPPORT) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE)) {
			    /* fill in ipv4 6rd entry */
    			shrinkField.eg_ipv4[0] = ntohl(PpeParseResult.iph.daddr);
    			shrinkField.eg_ipv4[1] = ntohl(PpeParseResult.iph.saddr);
    			index = find_and_update_shrink_table(PPE_UPDMEM_SEL_IPv4, &shrinkField);
    			if(index != -1) {
    				set_ppe_entry_tunnel_ip_index(foe_entry, IPV6_6RD, index);
    			} else {
    				printk("ipv4: find and update shrink table failed!\n");
    			}
            } else {
    			set_ppe_entry_tunnel_ip(foe_entry, IPV6_6RD, (void *)&PpeParseResult, 0);
    			set_ppe_entry_chksum(foe_entry, IPV6_6RD, PpeGetChkBase(&PpeParseResult.iph));
    			set_ppe_entry_ttl(foe_entry, IPV6_6RD, PpeParseResult.iph.ttl);
            }
			foe_entry->ipv6_6rd.flag = (ntohs(PpeParseResult.iph.frag_off) >> 13);
			foe_entry->ipv6_6rd.tunnel_dscp = PpeParseResult.iph.tos;

			/* IPv4 DS-Lite and IPv6 6RD shall be turn on by SW during initialization */
			foe_entry->bfib1.pkt_type = IPV6_6RD;
		}
	}
	/* IPv6 or IPv6 over PPPoE */
	else if (PpeParseResult.eth_type == htons(ETH_P_IPV6) || 
		(PpeParseResult.eth_type == htons(ETH_P_PPP_SES) && 
		 PpeParseResult.ppp_tag == htons(PPP_IPV6))) {
		if (PpeParseResult.pkt_type == IPV6_3T_ROUTE || PpeParseResult.pkt_type == IPV6_5T_ROUTE) {
			
			// incoming packet is 6RD and need to remove outer IPv4 header
			if(foe_entry->bfib1.pkt_type == IPV6_6RD) {
				PpeParseResult.rmt = 1;					
				foe_entry->ipv6_3t_route.dscp = (PpeParseResult.ip6h.priority << 4 | (PpeParseResult.ip6h.flow_lbl[0]>>4));
			} else {
				#if 0
				/* fill in ipv6 routing entry */
				foe_entry->ipv6_3t_route.ipv6_sip0 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[0]);
				foe_entry->ipv6_3t_route.ipv6_sip1 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[1]);
				foe_entry->ipv6_3t_route.ipv6_sip2 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[2]);
				foe_entry->ipv6_3t_route.ipv6_sip3 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[3]);
				
				foe_entry->ipv6_3t_route.ipv6_dip0 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]);
				foe_entry->ipv6_3t_route.ipv6_dip1 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]);
				foe_entry->ipv6_3t_route.ipv6_dip2 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]);
				foe_entry->ipv6_3t_route.ipv6_dip3 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]);
				#endif
				foe_entry->ipv6_3t_route.dscp = (PpeParseResult.ip6h.priority << 4 | (PpeParseResult.ip6h.flow_lbl[0]>>4));
			}
		} else if (PpeParseResult.pkt_type == IPV4_DSLITE) {
            if((PPE_SHNK_TABLE_SUPPORT) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE)) {
    			/* fill in DSLite entry */
    			shrinkField.eg_ipv6[0] = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]);
    			shrinkField.eg_ipv6[1] = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]);
    			shrinkField.eg_ipv6[2] = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]);
    			shrinkField.eg_ipv6[3] = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]);
    			
    			shrinkField.eg_ipv6[4] = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[0]);
    			shrinkField.eg_ipv6[5] = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[1]);
    			shrinkField.eg_ipv6[6] = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[2]);
    			shrinkField.eg_ipv6[7] = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[3]);
    			index = find_and_update_shrink_table(PPE_UPDMEM_SEL_IPv6, &shrinkField);
    			if(index != -1) {
    				set_ppe_entry_tunnel_ip_index(foe_entry, IPV4_DSLITE, index);
    			} else {
    				printk("ipv6: find and update shrink table failed!\n");
    			}
            } else {
    			set_ppe_entry_tunnel_ip(foe_entry, IPV4_DSLITE, (void *)&PpeParseResult, 0);
    			set_ppe_entry_ttl(foe_entry, IPV4_DSLITE, PpeParseResult.ip6h.hop_limit);
            }
			memcpy(foe_entry->ipv4_dslite.flow_lbl, PpeParseResult.ip6h.flow_lbl, sizeof(PpeParseResult.ip6h.flow_lbl));
			foe_entry->ipv4_dslite.priority = PpeParseResult.ip6h.priority;
			/* IPv4 DS-Lite and IPv6 6RD shall be turn on by SW during initialization */
			if(ntohl(PpeParseResult.iph.saddr) != foe_entry->ipv4_hnapt.sip){	
				PpeParseResult.map_e = 1;
				foe_entry->ipv4_hnapt.new_sip = ntohl(PpeParseResult.iph.saddr);
				foe_entry->ipv4_hnapt.new_dip = ntohl(PpeParseResult.iph.daddr);
				foe_entry->ipv4_hnapt.dscp = PpeParseResult.iph.tos;
				PpeParseResult.pkt_type = foe_entry->bfib1.pkt_type;
			}
			else{
				foe_entry->bfib1.pkt_type = IPV4_DSLITE;
            }
		}
	}
	else if(PpeParseResult.pkt_type == L2_BRIDGE){
		// do nothing
	}
	else {
		return 1;
	}

	return 0;
}



int32_t PpeFillInL3Info_InTunnel(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	//int index=0;
	//struct hwnat_shrink_field shrinkField;
	PpeParseResult.rmt = 0;
	/* IPv4 or IPv4 over PPPoE */
	if ((PpeParseResult.eth_type_intunnel== htons(ETH_P_IP)) || 
		(PpeParseResult.eth_type_intunnel== htons(ETH_P_PPP_SES) && 
		 PpeParseResult.ppp_tag_intunnel== htons(PPP_IP)) ||
		 (PpeParseResult.l2tp_ppp_tag== htons(PPP_IP))) {

		if ((PpeParseResult.pkt_type_intunnel== IPV4_HNAT) || (PpeParseResult.pkt_type_intunnel== IPV4_HNAPT)) {

			if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE) { //DS-Lite WAN->LAN
				return 1;
			} else {
				foe_entry->ipv4_hnapt.new_sip = ntohl(PpeParseResult.iph_intunnel.saddr);
				foe_entry->ipv4_hnapt.new_dip = ntohl(PpeParseResult.iph_intunnel.daddr);
				foe_entry->ipv4_hnapt.dscp = PpeParseResult.iph_intunnel.tos;
			}
        
		}
		else if (PpeParseResult.pkt_type_intunnel == IPV6_6RD) {
			/*not support 6rd*/
            return 1;
		}
	}
	/* IPv6 or IPv6 over PPPoE */
	else if (PpeParseResult.eth_type_intunnel == htons(ETH_P_IPV6) || 
		(PpeParseResult.eth_type_intunnel== htons(ETH_P_PPP_SES) && 
		 PpeParseResult.ppp_tag_intunnel== htons(PPP_IPV6)) ||
		 (PpeParseResult.l2tp_ppp_tag== htons(PPP_IPV6))) {
		if (PpeParseResult.pkt_type_intunnel == IPV6_3T_ROUTE || PpeParseResult.pkt_type_intunnel == IPV6_5T_ROUTE) {
			
			// incoming packet is 6RD and need to remove outer IPv4 header
			if(foe_entry->bfib1.pkt_type == IPV6_6RD) {
				return 1;
			} else {
				#if 0
				/* fill in ipv6 routing entry */
				foe_entry->ipv6_3t_route.ipv6_sip0 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[0]);
				foe_entry->ipv6_3t_route.ipv6_sip1 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[1]);
				foe_entry->ipv6_3t_route.ipv6_sip2 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[2]);
				foe_entry->ipv6_3t_route.ipv6_sip3 = ntohl(PpeParseResult.ip6h.saddr.s6_addr32[3]);
				
				foe_entry->ipv6_3t_route.ipv6_dip0 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]);
				foe_entry->ipv6_3t_route.ipv6_dip1 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]);
				foe_entry->ipv6_3t_route.ipv6_dip2 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]);
				foe_entry->ipv6_3t_route.ipv6_dip3 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]);
				#endif
				foe_entry->ipv6_3t_route.dscp = (PpeParseResult.ip6h_intunnel.priority << 4 | (PpeParseResult.ip6h_intunnel.flow_lbl[0]>>4));
			}
		} 
	}
	else if(PpeParseResult.pkt_type == L2_BRIDGE){
		// do nothing
	}
	else {
		return 1;
	}

	return 0;
}


int32_t PpeFillInL4Info(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	if (PpeParseResult.pkt_type == IPV4_HNAPT) {
		// DS-LIte WAN->LAN
		if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE) {
			return 0;
		}

		/* Set Layer4 Info - NEW_SPORT, NEW_DPORT */
		if (PpeParseResult.iph.protocol == IPPROTO_TCP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.th.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.th.dest);
			foe_entry->ipv4_hnapt.bfib1.udp = TCP;
		} else if (PpeParseResult.iph.protocol == IPPROTO_UDP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.uh.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.uh.dest);
			foe_entry->ipv4_hnapt.bfib1.udp = UDP;

			/* if UDP checksum is zero, it cannot be accelerated by HNAT */
			/* we found some protocols, such as IPSEC-NAT-T, are possible to hybrid udp zero checksum 
			 * and non-zero checksum in the same session, so we disable HNAT acceleration for all UDP flows 
			 */
			//if(foe_entry->ipv4_hnapt.new_sport==4500 && foe_entry->ipv4_hnapt.new_dport==4500)  
			//	return 1;
		}
	} else if (PpeParseResult.pkt_type == IPV4_HNAT) {
        if(PpeParseResult.iph.protocol == IPPROTO_GRE)
        {
            foe_entry->ipv4_hnapt.new_sport = PpeParseResult.gre_call_id;
        }
		/* do nothing */
	} else if (PpeParseResult.pkt_type == L2_BRIDGE) {
	
		/* do nothing */

	} else if (PpeParseResult.pkt_type == IPV6_3T_ROUTE) {
		/* do nothing */
	} else if (PpeParseResult.pkt_type == IPV6_5T_ROUTE) {
		/* do nothing */

	}

	return 0;
}



int32_t PpeFillInL4Info_InTunnel(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	if (PpeParseResult.pkt_type_intunnel== IPV4_HNAPT) {
		// DS-LIte WAN->LAN

		/* Set Layer4 Info - NEW_SPORT, NEW_DPORT */
		if (PpeParseResult.iph_intunnel.protocol == IPPROTO_TCP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.th_intunnel.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.th_intunnel.dest);
			foe_entry->ipv4_hnapt.bfib1.udp = TCP;
		} else if (PpeParseResult.iph_intunnel.protocol == IPPROTO_UDP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.uh_intunnel.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.uh_intunnel.dest);
			foe_entry->ipv4_hnapt.bfib1.udp = UDP;

			/* if UDP checksum is zero, it cannot be accelerated by HNAT */
			/* we found some protocols, such as IPSEC-NAT-T, are possible to hybrid udp zero checksum 
			 * and non-zero checksum in the same session, so we disable HNAT acceleration for all UDP flows 
			 */
			//if(foe_entry->ipv4_hnapt.new_sport==4500 && foe_entry->ipv4_hnapt.new_dport==4500)  
			//	return 1;
		}
	} else if (PpeParseResult.pkt_type_intunnel == IPV4_HNAT) {
        
		/* do nothing */
	} else if (PpeParseResult.pkt_type_intunnel == L2_BRIDGE) {

		/* do nothing */

	} else if (PpeParseResult.pkt_type_intunnel == IPV6_3T_ROUTE) {
		/* do nothing */
	} else if (PpeParseResult.pkt_type_intunnel == IPV6_5T_ROUTE) {
		/* do nothing */

	}

	return 0;
}


static int ppeCheckNpuOffload(int magic)
{
    if(((magic == FOE_MAGIC_VXLAN_UP_1)||(magic == FOE_MAGIC_VXLAN_DOWN_1))&&(npu_tunnel_offload_proto&NPU_OFFLOAD_VXLAN))
        return 1;
    
    return 0;
}

static unsigned char ppeTransExtIfNumToNpuUdf(int extIfNum)
{
    if((extIfNum >= DP_VXLAN_0)&&(extIfNum <= DP_VXLAN_19))
        return NPU_UDF_VXLAN_0+extIfNum-DP_VXLAN_0;
    
    return 0;
}

static int ppeGetDownStreamVxlanDev(struct sk_buff* skb,struct FoeEntry *foe_entry)
{   
    int i;
    struct net_device* dev_tmp;
    
    if ((foe_entry->bfib1.pkt_type == IPV4_HNAPT) || (foe_entry->bfib1.pkt_type == IPV4_HNAT))
    {
        dev_tmp = vxlan_getlink(PpeParseResult.vxlan_vni);
        if (dev_tmp)
        {
            for (i = DP_VXLAN_0; i<= DP_VXLAN_19; i++)
            {
                if (dev_tmp == DstPort[i])
                    return i;
            }
        }
    }

    return -1;
}

/*set udf to eg_dscp,for npu tunnle offload shortcut*/
static int ppeSetUdfTo1stVlan(struct FoeEntry * foe_entry,uint8 udf)
{
    if(!hwnat_npu_vxlan_offload_shortcut)
        return 0;
    
    if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry))
    {
        PpeParseResult.vlan_layer = 1;
        foe_entry->bfib1.vpm = 2;
        foe_entry->ipv4_hnapt.vlan1 = udf;
    }

    return 0;
}

uint32_t PpeSetExtIfNum(struct sk_buff * skb, struct FoeEntry * foe_entry,int magic)
{
	int offset = 0;
	int len=0;
	int i = 0;
    int idx = FOE_ENTRY_NUM(skb);
    int npu_offload = 0;
    int vxlan_dev_id = 0;

	if(!skb->dev)
	{
		return 0;
	}

    if(ppeCheckNpuOffload(magic))
        npu_offload = 1;

	//must set at the beginning
	if(FOE_MAGIC_CRYPTO_D_1 == magic)
	{
		offset = DP_CRYPTO_D_0 +  skb->cb[IPSEC_SKB_CB];
        if (offset < DP_CRYPTO_D_MAX)
        {
            vpn_decrypt_src_dev[offset - DP_CRYPTO_D_0] = skb->dev;
        }
	}
	else if(FOE_MAGIC_CRYPTO_E_1 == magic)
	{
		offset = DP_CRYPTO_E_0 + skb->cb[IPSEC_SKB_CB];
	}
	else
	{
		len = strlen(skb->dev->name);

		if (is24GWiFiInterface(skb->dev) &&
			((get24GWifiIndex(skb->dev) >= 0) && (get24GWifiIndex(skb->dev) <= 7))) 
		{
	         offset = DP_RA0 + get24GWifiIndex(skb->dev);
		}
		else if (is5GWiFiInterface(skb->dev) &&
			((get5GWifiIndex(skb->dev) >= 0) && (get5GWifiIndex(skb->dev) <= 7))) 
		{
         		offset = DP_RAI0 + get5GWifiIndex(skb->dev);
		}
		else if (is6GWiFiInterface(skb->dev) &&
			((get6GWifiIndex(skb->dev) >= 0) && (get6GWifiIndex(skb->dev) <= 7))) 
		{
			offset = DP_RAX0 + get6GWifiIndex(skb->dev);
		}
		else if ((len == 4) && (strncmp(skb->dev->name,"wds", 3) == 0) &&
			((skb->dev->name[3] >= '0') && (skb->dev->name[3] <= '3'))) 
		{
         		offset = DP_WDS0 + (skb->dev->name[3] - '0');
		}
		else if ((len == 5) && (strncmp(skb->dev->name,"wdsi", 4) == 0) &&
			((skb->dev->name[4] >= '0') && (skb->dev->name[4] <= '3'))) 
		{
         		offset = DP_WDSI0 + (skb->dev->name[4] - '0');
		}
		else if ((len == 6) && (strncmp(skb->dev->name,"apcli", 5) == 0) &&
			((skb->dev->name[5] >= '0') && (skb->dev->name[5] <= '3'))) 
		{
         		offset = DP_APCLI0 + (skb->dev->name[5] - '0');
		}
		else if ((len == 7) && (strncmp(skb->dev->name,"apclii", 6) == 0) &&
			((skb->dev->name[6] >= '0') && (skb->dev->name[6] <= '3'))) 
		{
         		offset = DP_APCLII0 + (skb->dev->name[6] - '0');
		}
		else if ((len == 7) && (strncmp(skb->dev->name,"apclix", 6) == 0) &&
			((skb->dev->name[6] >= '0') && (skb->dev->name[6] <= '3'))) 
		{
			offset = DP_APCLIX0 + (skb->dev->name[6] - '0');
		}

		/*interface is not up ,not allow to speed*/
		 if(ra_sw_wifi_hook_is_wifi_down)
		 	if(ra_sw_wifi_hook_is_wifi_down(skb,0) == 1)
				return 1;
	}

	if(macqueue_start_idx!=-1  && soft_ratelimit_enqueue_hook){
		if(magic == FOE_MAGIC_PTM){
			if ((strlen(skb->dev->name) == 4) && (strncmp(skb->dev->name,"ptm", 3) == 0) &&
			((skb->dev->name[3] >= '0') && (skb->dev->name[3] <= '1'))) 
			{
					offset = DP_PTM0 + (skb->dev->name[3] - '0');
			}
		}
		if((magic == FOE_MAGIC_EPON) || (magic == FOE_MAGIC_GPON)){
			offset = DP_PON;
		}
	}

	if (TCSUPPORT_VLAN_ACCESS_TRUNK_VAL && (pkttype & DS_MULTICAST))
	{
		for (i = DP_MCAST_NAS0; i <= DP_MCAST_PPP7; i++){
			if (skb->wan_dev == DstPort[i])
				break;
		}
		if (i <= DP_MCAST_PPP7){
			offset = i;
		}else{
			printk("PpeSetExtIfNum: wrong device!\n");
		}

		if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry)){
			foe_entry->ipv4_hnapt.mcast = 1;
		}else if (IS_L2_RRIDGE(foe_entry)){
			foe_entry->l2_bridge.mcast = 1;
		}else if (IS_IPV4_DSLITE(foe_entry) || IS_IPV6_3T_ROUTE(foe_entry) 
				|| IS_IPV6_5T_ROUTE(foe_entry) || IS_IPV6_6RD(foe_entry)){
			foe_entry->ipv6_5t_route.mcast = 1;
		}

		PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
	}
	
	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
	{
		if(ds_need_offload&&((magic == FOE_MAGIC_GE) || (magic == FOE_MAGIC_PCI) || (magic == FOE_MAGIC_WLAN))){
			u8 forcetxq = 0;
			if(pkttype&RTSP){
				if(magic == FOE_MAGIC_GE) {
					forcetxq = RTSP_OFFLOAD;
				} else {
					forcetxq = RTSP_OFFLOAD_WLAN;
				}
			}else if((magic == FOE_MAGIC_PCI) || (magic == FOE_MAGIC_WLAN)){
				forcetxq = 0;   /* wan/lan->wifi non-rtsp packets, no need to go to sort qdma path */
			}else if(pkttype&DS_MULTICAST){
				forcetxq = 3 + 1;
			}else{
				forcetxq = 2 + 1;
			}
		
			foe_ext[idx].ds_offload = forcetxq;
		}
	}

	if (TCSUPPORT_FWC_MCST_VAL || TCSUPPORT_XPON_HAL_API_MCST_VAL)
	{
		if (FOE_MAGIC_GE == magic)
		{
			offset = DP_GMAC;
		}
	}

	if(FOE_MAGIC_LOCAL == magic)
		offset = DP_LOCAL_HOST;

	if ((FOE_MAGIC_GRE_UP_1 == magic))
	{
		for (i = DP_GRE_TUNNEL_0; i <= DP_GRE_TUNNEL_7; i++)
		{
			if (skb->dev == DstPort[i])
			break;
		}

		if (i <= DP_GRE_TUNNEL_7)
		{
			offset = i;
		}else{
			for (i = DP_GRE_L2_PRI_TUNNEL_0; i <= DP_GRE_L2_SEC_TUNNEL_7; i++)
	        {
	            if (skb->dev == DstPort[i])
	                break;
	        }

	        if (i <= DP_GRE_L2_SEC_TUNNEL_7)
	        {
	            offset = i;
	        }
        }
	}


	if ((FOE_MAGIC_GRE_HWDOWN_1 == magic))
	{
		for (i = DP_GRE_TUNNEL_0; i <= DP_GRE_TUNNEL_7; i++)
		{
			if (skb->dev == DstPort[i])
			break;
		}

		if (i <= DP_GRE_TUNNEL_7)
		{
			offset = i;
		}else{
			for (i = DP_GRE_L2_PRI_TUNNEL_0; i <= DP_GRE_L2_SEC_TUNNEL_7; i++)
	        {
	            if (skb->dev == DstPort[i])
	                break;
	        }

	        if (i <= DP_GRE_L2_SEC_TUNNEL_7)
	        {
	            offset = i;
	        }
        }
		
	}


	if ((FOE_MAGIC_VXLAN_HWDOWN_1 == magic))
	{
		for (i = DP_VXLAN_0; i <= DP_VXLAN_19; i++)
        {
            if (skb->dev == DstPort[i])
                break;
        }

        if (i <= DP_VXLAN_19)
        {
                offset = i;
        }
		
	}

	if(FOE_MAGIC_L2TP_HWDOWN_1 == magic){
		offset = DP_L2TP_HWDOWN;
	}

		
    if ((FOE_MAGIC_VXLAN_UP_1 == magic))
    {
        for (i = DP_VXLAN_0; i <= DP_VXLAN_19; i++)
        {
            if (skb->dev == DstPort[i])
                break;
        }

        if (i <= DP_VXLAN_19)
        {
            if(npu_offload)
            {
                offset = ppeTransExtIfNumToNpuUdf(i);
                ppeSetUdfTo1stVlan(foe_entry,offset);
                ecnt_hwnat_npu_add_session_to_vxlan_hdr_list(skb->dev->name,idx);
            }
            else
                offset = i;
        }
    }

    if ((FOE_MAGIC_GRE_DOWN_1 == magic) || (FOE_MAGIC_VXLAN_DOWN_1 == magic))
	{
	    if(FOE_MAGIC_VXLAN_DOWN_1 == magic)
            vxlan_dev_id = ppeGetDownStreamVxlanDev(skb,foe_entry);
        
		for (i = DP_NAS0; i <= DP_NAS7; i++)
		{
			if (skb->dev == DstPort[i])
			break;
		}

		if (i <= DP_NAS7)
		{
		    if(npu_offload)
            {
                if(-1 != vxlan_dev_id)
                    offset = NPU_UDF_VXLAN_DOWN_0+vxlan_dev_id-DP_VXLAN_0;
                else
                    offset = NPU_UDF_MAX_NUM;/*drop at npu*/

                ppeSetUdfTo1stVlan(foe_entry,offset);
            }
            else
			    offset = i;
		}
		else
		{
			for (i = DP_PPP0; i <= DP_PPP7; i++)
			{
				if (skb->dev == DstPort[i])
				break;
			}

			if (i <= DP_PPP7)
			{
                if(npu_offload)
                {
                    if(-1 != vxlan_dev_id)
                        offset = NPU_UDF_VXLAN_DOWN_0+vxlan_dev_id-DP_VXLAN_0;
                    else
                        offset = NPU_UDF_MAX_NUM;/*drop at npu*/
                    
                    ppeSetUdfTo1stVlan(foe_entry,offset);
                }
                else
				    offset = i;
			}
		}
	}

	if(FOE_MAGIC_ASYM_DOWNSTREAM == magic)
	{
		if(skb->dev)
			offset = DP_ASYM_D1+(skb->dev->name[5]-'1');
		else
			offset = 0;

		skb->ecnt_asym_mark |= ECNT_ASYM_MARK_BINDED;
		skb->ecnt_asym_mark |= FOE_ENTRY_NUM(skb);
	}
	else if(FOE_MAGIC_ASYM_UPSTREAM == magic)
	{
		if(skb->dev)
			offset = DP_ASYM_U1+(skb->dev->name[5]-'1');
		else
			offset = 0;
	}

	
	if(FOE_MAGIC_USBNET == magic) //add for usb dongle
	{	
		if (isUSBInterface(skb->dev)) 
		{
			offset = DP_USBNET0 + getUSBIndex(skb->dev);
		}
		else
			offset = DP_USBNET2;
	}

	if(FOE_MAGIC_OOKLA == magic){
		_info_blk2 *iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
		offset = DP_OOKLA_RX;
		foe_entry->ipv4_hnapt.etype = 0xAAAA;
		if(!(iblk2->nbq&0x1)){
			iblk2->fpidx = FP_QDMA_SW;
			if(iblk2->nbq == 6)
				iblk2->nbq = 5;
		}
	}

	foe_ext[idx].dp = offset; // store in foe_ext for cache acces
#ifdef CONFIG_TP_IMAGE
	if (skb && skb->dev && foe_intf && idx < FOE_4TB_SIZ_TOTAL)
	{
		foe_intf[idx].act_dp = tp_get_dev_ifindex(skb->dev, foe_entry);
	}
#endif /* CONFIG_TP_IMAGE */
	if(offset == 0){ // no find any device no hwnat device
		return 0;
	}
	
	if (TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL) &&
		(isWiFiInterface(skb->dev) || isAPCLIInterface(skb->dev)) && magic == FOE_MAGIC_WLAN && 
		((offset >= DP_RAI0 && offset <=DP_RAI7) || (offset >= DP_RA0 && offset <= DP_RA7) ||
		(offset >= DP_RAX0 && offset <= DP_APCLIX0) || (offset >= DP_APCLI0 && offset <= DP_APCLII0)) ){
		return 0;
	}
	else if (IS_IPV4_HNAT(foe_entry) || IS_IPV4_HNAPT(foe_entry)) {
		foe_entry->ipv4_hnapt.act_dp = offset;
	}
	else if IS_L2_RRIDGE(foe_entry){
		foe_entry->l2_bridge32.act_dp = offset;
	}
	else if(IS_IPV4_DSLITE(foe_entry) || IS_IPV6_3T_ROUTE(foe_entry) || IS_IPV6_5T_ROUTE(foe_entry) || IS_IPV6_6RD(foe_entry)){
		foe_entry->ipv6_5t_route.act_dp = offset;
	}
	else{
		return 1;
	}

	return 0;
}

static int hwnat_tunnel_info_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	printk(" HIP info HOP0: %d, HOP1 %d HOP2 %d HOP3 %d \n", hop0, hop1, hop2 , hop3);

	printk("tunnel_enable %d tunnel_id %d \n",tunnel_en, tunnel_id);
	printk("tunnel_mtu %d \n",tunnel_mtu);
	printk("IPSec %s SA Index %d\n", ipsec_en?"Enable":"Disable", sa_id);
	
	return 0;			
}

static int hwnat_tunnel_info_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char val_string[64], cmd[16], subcmd[16] ;
	int value0, value1, value2, value3 = 0; 

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;
    
	sscanf(val_string, "%s %s %d %d %d %d ", cmd, subcmd,  &value0, &value1,&value2,&value3) ;

	if(!strcmp(cmd, "set")){

		if(!strcmp(subcmd, "hop")){
			hop0 = value0;
			hop1 = value1;
			hop2 = value2;
			hop3 = value3;

			printk("HIP info HOP0: %d, HOP1 %d HOP2 %d HOP3 %d \n", hop0, hop1, hop2 , hop3);
		}else if(!strcmp(subcmd, "tunnelid")){
			tunnel_en = value0;
			tunnel_id = value1;

			printk("tunnel_enable %d tunnel_id %d \n", tunnel_en, tunnel_id);

		}else if(!strcmp(subcmd, "mtu")){
			tunnel_mtu = value0;

			printk("tunnel_mtu %d \n", tunnel_mtu);

		}else if(!strcmp(subcmd, "sa")){
			ipsec_en = value0;
			sa_id = value1;
		}

	}

	return count;
}
static int hwnat_prefix_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int i = 0,j = 0,k = 0;
	unsigned short tmp[8];
	int num = 0;
	unsigned int len = 0;

	printk("----------------------------------\n");
	for(i = 0;i < PPE_NPTV6_PREFIX_MAX_NUM;i+=2)
	{
		if(nptv6_prefix[i].used)
		{
			printk("index:%d\n",i);
			printk("interace %s SNPT\n",nptv6_prefix[i].ifname);
			for(k = 0;k < 8;k++)
				tmp[k] = nptv6_prefix[i].s_prefix[k];
			len = nptv6_prefix[i].s_prefix_len;
	
			printk("prefix:");
			num = len/16;
			for(j = 0;j < num;j++)
			{
				printk("%x",tmp[j]);
				if(j == (num-1))
					printk("::/%d\n",len);
				else
					printk(":");
			}
			printk("----------------------------------\n");
			printk("index:%d\n",i+1);
			printk("interace %s DNPT\n",nptv6_prefix[i+1].ifname);
			for(k = 0;k < 8;k++)
				tmp[k] = nptv6_prefix[i+1].d_prefix[k];
			len = nptv6_prefix[i+1].d_prefix_len;
	
			printk("prefix:");
			num = len/16;
			for(j = 0;j < num;j++)
			{
				printk("%x",tmp[j]);
				if(j == (num-1))
					printk("::/%d\n",len);
				else
					printk(":");
			}			
			printk("----------------------------------\n");
		}
	}

	return 0;	
}

static int hwnat_npu_vxlan_offload_shortcut_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
    printk("hwnat_npu_vxlan_offload_shortcut:%d\n",hwnat_npu_vxlan_offload_shortcut);
    
	return 0;	
}

static int hwnat_npu_vxlan_offload_shortcut_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)

{
    char get_buf[9];
	unsigned int enable = 0;
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d",&enable);

	hwnat_npu_vxlan_offload_shortcut = enable;
	
	return count;
}

ecnt_ret_val PpeCopyFoeInfo(struct ecnt_data *in_data)
{
	struct net_data_s *net_data = (struct net_data_s *)in_data;
	struct sk_buff * toskb = NULL;
	struct sk_buff * fromskb = NULL;

	if( (net_data->pskb == NULL) || (*(net_data->pskb) == NULL) 
		|| net_data->foe_copy_data.fromskb == NULL
		|| (*(net_data->foe_copy_data.fromskb) == NULL))
	{
		return ECNT_CONTINUE ;
	}
	
	toskb = (*(net_data->pskb));
	fromskb = (*(net_data->foe_copy_data.fromskb));

	if (IS_SPACE_AVAILABLED(toskb) && IS_SPACE_AVAILABLED(fromskb) &&
		IS_MAGIC_TAG_VALID(fromskb)) 
	{      
		memcpy(FOE_INFO_START_ADDR(toskb), FOE_INFO_START_ADDR(fromskb), FOE_INFO_LEN);
	} 

	return ECNT_CONTINUE ;
}

struct ecnt_hook_ops ecnt_copy_foe_info_op = {
	.name = "PpeCopyFoeInfo",
	.is_execute = 1,
	.hookfn = PpeCopyFoeInfo,
	.maintype = ECNT_NET_AF,
	.subtype = ECNT_NET_AF_PACKET_RCV,
	.priority = 1
};

static inline int PpeIsKeepAlive(struct sk_buff * skb)
{
	if( (FOE_AI(skb) == HIT_BIND_KEEPALIVE_UC_OLD_HDR) 			/* Hit bind and keep alive with unicast old-header packet */
		|| (FOE_AI(skb) == HIT_BIND_KEEPALIVE_MC_NEW_HDR) 		/* Hit bind and keep alive with multicast new-header packet */
		|| (FOE_AI(skb) == HIT_BIND_KEEPALIVE_DUP_OLD_HDR) )	/* Hit bind and keep alive with duplicate old-header packet */
	{
		return 1 ;
	}

	return 0 ;
}

int ppe_crsn_valid(struct sk_buff * skb)
{
	if((FOE_AI(skb) == UN_HIT) && (g_mul_high_priority || (hwnat_fast_bind == 2)))
	{
		return 0;
	}
	
	if((FOE_AI(skb) > UN_HIT) && (FOE_AI(skb) < NOT_THROUGH_PPE))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

__IMEM int PpeFreeHandler(struct sk_buff * skb)
{
	if(hwnat_dgb_free_idx == FOE_ENTRY_NUM(skb))
	{
		dump_stack();
		hwnat_dgb_free_idx = FOE_ENTRY_NUM_INVALID+1;
	}
	
	if((skb->ppe_info_flag == 0)&&(TCSUPPORT_PON_VLAN_VAL||TCSUPPORT_PON_MAC_FILTER_VAL||
		(TCSUPPORT_GPON_MAPPING_VAL&&TCSUPPORT_GPON_DOWNSTREAM_MAPPING_VAL)))
	{
		skb->ppe_info_flag = 1;
		skb->ppe_magic = FOE_MAGIC_TAG(skb);
		skb->ppe_ai = FOE_AI(skb);
		skb->ppe_foe_entry = FOE_ENTRY_NUM(skb);
	}
	
	if (IS_SPACE_AVAILABLED(skb)  &&  
		(IS_MAGIC_TAG_VALID(skb) || (FOE_MAGIC_TAG(skb) == FOE_MAGIC_PPE))) 
	{
		if( !PpeIsKeepAlive(skb) )
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
	}

	skb->sw_nat_flag = 0;

	return 0;
}

__IMEM int PpeRxinfoHandler(struct sk_buff * skb, int magic, char *data, int data_length)
{
		//printk("PpeRxinfoHandler\n");
        FOE_MAGIC_TAG(skb) = magic;
        if ((magic == FOE_MAGIC_GE) || (magic == FOE_MAGIC_ATM) // no WLAN
			|| (magic == FOE_MAGIC_PTM) || (magic == FOE_MAGIC_EPON)
			|| (magic == FOE_MAGIC_GPON)|| (magic == FOE_MAGIC_AE_WAN)
            || (magic == FOE_MAGIC_PTM_LAN)||(magic == FOE_MAGIC_XSI)||(magic == FOE_MAGIC_XSI_GDM4)||(magic == FOE_MAGIC_OLT)
        )
                memcpy(FOE_INFO_START_ADDR(skb)+2, data, data_length);

		if(ppe_crsn_valid(skb))
		{
			/* last entry */
			FOE_ENTRY_NUM(skb) = (FOE_4TB_SIZ_TOTAL-1);
		}

		if (DebugLevel >= 3) {
			printk("\nrxmsg1:0x%08x.\n", *(unsigned int*)data);
		}

        return 0;
}

__IMEM int32_t PpeXsiFastTxHandler(struct sk_buff * skb)
{
    uint32 dest_port = 0, entry_type = 0;
    struct FoeEntry *foe_entry = NULL;
    
    if (!skb)
        return 0;
    foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + FOE_ENTRY_NUM(skb)*SIZE_OF_FOE_ENTRY);
    
    if (TCSUPPORT_NEW_WDOG_VAL)
        wdog_kick_api(2);

    entry_type = foe_entry->bfib1.pkt_type;
    switch(entry_type)
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
    if(dest_port == DP_XSI) 
        xsi_fast_tx_hook(skb);
    else
        return 1;
    return 0;
}

__IMEM int PpeMagicHandler(struct sk_buff * skb, int magic)
{
        if (FOE_MAGIC_TAG(skb) == magic)
                return 1;
        else
                return 0;
}

int PpeSetMagicHandler(struct sk_buff * skb, int magic)
{
        FOE_MAGIC_TAG(skb) = magic;
        return 0;
}

int PpeXferHandler(struct sk_buff *new, const struct sk_buff *old)
{
	if (IS_SPACE_AVAILABLED(new) && IS_SPACE_AVAILABLED(old) &&
		(IS_MAGIC_TAG_VALID(old) || (FOE_MAGIC_TAG(old) == FOE_MAGIC_PPE))) 
	{      
		memcpy(FOE_INFO_START_ADDR(new), FOE_INFO_START_ADDR(old), FOE_INFO_LEN);
	} 
	else 
	{
		if (IS_SPACE_AVAILABLED(new))
			memset(FOE_INFO_START_ADDR(new), 0, FOE_INFO_LEN);
	}

	return 0;
}

/*0 continue*/

int PpeExtIfTUNNELDOWNHandler(struct sk_buff * skb, uint rx_len, unsigned short sptag, unsigned short udf, int magic, char *data, int data_length)
{

	uint16_t VirIfIdx = 0;
	
	if(sptag == FOE_MAGIC_GRE_HWDOWN_1 ||sptag == FOE_MAGIC_VXLAN_HWDOWN_1 || sptag == FOE_MAGIC_L2TP_HWDOWN_1){
		VirIfIdx = udf;
		if (((VirIfIdx >= DP_GRE_TUNNEL_0) && (VirIfIdx <= DP_GRE_TUNNEL_7))
		||((VirIfIdx >= DP_GRE_L2_PRI_TUNNEL_0) && (VirIfIdx <= DP_GRE_L2_SEC_TUNNEL_7))
			||((VirIfIdx >= DP_VXLAN_0) && (VirIfIdx <= DP_VXLAN_19))){

			if(unlikely(skb == NULL)) {
				return 0;
			}

			if(unlikely((rx_len < 52) || (rx_len > 2000))){
				dev_kfree_skb_any(skb);
				return 1;
			}
			skb_put(skb, rx_len);
			PpeRxinfoHandler(skb,magic,data,data_length);

			skb->dev = DstPort[VirIfIdx];

			if ((NULL == skb->dev) || (0 == (skb->dev->flags & IFF_UP)))
			{
				dev_kfree_skb_any(skb);
				return 1;
			}

			skb->protocol = eth_type_trans(skb, skb->dev);
			if((VirIfIdx >= DP_GRE_TUNNEL_0) && (VirIfIdx <= DP_GRE_TUNNEL_7))
				skb->pkt_type = PACKET_HOST;
			
			netif_rx(skb);
			return 1;

		}else if(VirIfIdx == DP_L2TP_HWDOWN){


			if(unlikely(skb == NULL)) {
				return 0;
			}

			if(unlikely((rx_len < 52) || (rx_len > 2000))){
				dev_kfree_skb_any(skb);
				return 1;
			}
			skb_put(skb, rx_len);
			PpeRxinfoHandler(skb,magic,data,data_length);

			if( l2tp_vpn_dev )
				skb->dev = l2tp_vpn_dev ;
			else{
				skb->dev = dev_get_by_name(&init_net,L2TP_VPN_PPP_NAME);
				if(skb->dev)
					dev_put(skb->dev);
			}
			if ((NULL == skb->dev) || (0 == (skb->dev->flags & IFF_UP)))
			{
				dev_kfree_skb_any(skb);
				return 1;
			}

			skb->protocol = eth_type_trans(skb, skb->dev);

			skb->pkt_type = PACKET_HOST;
			netif_rx(skb);
			return 1;

		}

	}


	return 0 ;

}


int PpeFoeEntryHandler(void * inputvalue,int operation)
{
	int ret = -1;
	switch(operation)
	{	
		case FOE_OPE_GETENTRYNUM:
			ret = FOE_ENTRY_NUM((struct sk_buff *)inputvalue);
			break;

		case FOE_OPE_CLEARENTRY:
			ret = FoeDelEntryByNum(	*((unsigned int*)inputvalue));
			break;

		default:
			printk("\r\n not support such operation(%d)",operation);
			break;
	}

	return ret;
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

void ppeSetAcntMeterLengthMode(struct FoeEntry * foe_entry, 
						enum ppeLengthMode acntMode, enum ppeLengthMode meterMode)
{
	_info_blk2 *iblk2;

	if (IS_IPV4_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
	else if (IS_IPV6_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
	else if (IS_L2_RRIDGE(foe_entry))
	{
		iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
	}
	else
		return;
	
    set_ppe_acnt_meter_len_mode(iblk2, acntMode, meterMode);

	return;
}

int32_t PpeDropPacketHandler(struct sk_buff * skb)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	unsigned int foe_entry_idx=0;
	__u16 Ppe_Magic = 0;
	__u8 Ppe_AI = 0;
	int ret = 0;
	unsigned long lock_flag ;
	__u8 force_port = FP_DROP;
	__u8 drop_meter_id = 0x3f;
	__u8 drop_account_id = 0x3f;  

	if(skb == NULL)
	return 0;

	if(hwnat_dgb_drop_idx == FOE_ENTRY_NUM(skb))
	{
		dump_stack();
		/*dump stack once*/
		hwnat_dgb_drop_idx = FOE_ENTRY_NUM_INVALID+1;
	}


	
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
    
    if (TCSUPPORT_UNKNOW_MULTICAST_COUNT_VAL && xpon_is_multicast_entry(foe_entry))
    {
        unknow_mc_byte_cnt += (skb->len + VLAN_ETH_HLEN);
        unknow_mc_pkt_cnt++;
        force_port = FP_GDMA1;
        drop_meter_id = UNKNOW_MC_METER_ID;
        drop_account_id = UNKNOW_MC_ACNT_ID;
    }

	if(check_bind(Ppe_Magic,foe_entry,Ppe_AI))
	{
		//if this entry is already in binding, skip it
		if(foe_entry->bfib1.state == BIND)
			return 0;
		PpeClearEntryInfo(foe_entry);
		
		spin_lock_irqsave(&hw_nat_lock, lock_flag);
		if(IS_IPV4_GRP(foe_entry) || IS_L2_RRIDGE(foe_entry))
			PpeSetInfoBlk2(foe_entry, force_port, drop_meter_id, drop_account_id, 1);
		else if(IS_IPV6_GRP(foe_entry))
			PpeSetInfoBlk2(foe_entry, force_port, drop_meter_id, drop_account_id, 1);
		else 
		{
			if (DebugLevel >= 3) 
				NAT_PRINT ("\r\nfoe_entry error,return -1");
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
			return -1;
		}

		if(TCSUPPORT_UNKNOW_MULTICAST_COUNT_VAL)
			ppeSetAcntMeterLengthMode(foe_entry, PRE_MODE, PRE_MODE);
		
		if(check_gemport && ((FOE_MAGIC_EPON == FOE_MAGIC_TAG(skb)) || (FOE_MAGIC_GPON == FOE_MAGIC_TAG(skb)))){
			foe_ext[FOE_ENTRY_NUM(skb)].gemport = skb->gem_port;
		}

        skb->bridge_flag = 1;

		/* Enter binding state */
		PpeSetEntryBind(skb, foe_entry);

		ecnt_multicast_hwnat_drop_flow(skb);

		spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
		
		/* Dump Binding Entry */
		if (DebugLevel >= 1)
			FoeDumpEntry(FOE_ENTRY_NUM(skb));
		ret = 1;
		goto finish;
	}
	ret = 0;
	
finish:
	if(SUPPORT_SRAM_ENTRY && (foe_entry_idx < hwnat_dram_idx_base)) {
		setPpeEntry(foe_entry_idx, foe_entry);
	}
	return ret;
}

unsigned int ppeUpdateMeterInfo( struct FoeEntry * foe_entry ,unsigned int meterGrpId)
{
	_info_blk2 *iblk2;
	
	if (IS_IPV4_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
	else if (IS_IPV6_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
	else if (IS_L2_RRIDGE(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
	else
		return 0;

	if( GetInfo2Mgrp(iblk2) == HWNAT_DEFAULT_GRP_ID)/*not add any meter group*/
		SetInfo2Mgrp(iblk2,meterGrpId & HWNAT_DEFAULT_GRP_ID);

	return 0 ;
}

unsigned int ppeUpdateAcntInfo(struct FoeEntry *foe_entry, struct sk_buff * skb, 
	int magic, struct port_info * pinfo)
{
    _info_blk2 *iblk2;
    unchar acnt2 = 0x0;
    unchar acnt1 = 0x0;
    unchar acnt0 = HWNAT_DEFAULT_GRP_ID;
    unchar meter0 = HWNAT_DEFAULT_MTRGRP, meter1 = HWNAT_DEFAULT_MTRGRP, meter2 = HWNAT_DEFAULT_MTRGRP;
    FE_Dir_t dir = DOWN_STREAM;
	
	if (IS_IPV4_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
	else if (IS_IPV6_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
	else if (IS_L2_RRIDGE(foe_entry))
	{
		if ((IS_IC_EN751X2X) || (PPE_L2B_ENTRY_SUPPORT))
			iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
		else
			iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge.info_blk2);
	}
	else
		return 0;

	if(TCSUPPORT_ACCOUNT_METER_V2_VAL){
		FE_API_GET_ACNT2_IDX(skb, DOWN_STREAM, &acnt2);
		FE_API_GET_ACNT1_IDX(skb, DOWN_STREAM, &acnt1);
		FE_API_GET_ACNT0_IDX(skb, DOWN_STREAM, &acnt0);
		FE_API_GET_METER_IDX(skb, DOWN_STREAM, &meter0, 0);
		FE_API_GET_METER_IDX(skb, DOWN_STREAM, &meter1, 1);
		FE_API_GET_METER_IDX(skb, DOWN_STREAM, &meter2, 2);
		if (DebugLevel >= 1) 
			NAT_PRINT("\nacnt0 is %d,acnt1 is %d,acnt2 is %d, meter0 is %d,meter1 is %d,meter2 is %d,", acnt0,acnt1,acnt2,meter0,meter1,meter2);
	}
	
	if(isEN7580)
	{
		if(!TCSUPPORT_ACCOUNT_METER_V2_VAL){	
			acnt1 = (unchar)((skb->fe_resource_mark & ACNT1_MASK)>>ACNT1_OFFSET);
			acnt0 = (unchar)((skb->fe_resource_mark & ACNT0_MASK)>>ACNT0_OFFSET);
		}
		SetInfo2Mgrp(iblk2, acnt0 & HWNAT_DEFAULT_ACNT0GRP);
		iblk2->port_ag = acnt1 & HWNAT_DEFAULT_ACNT1GRP ;
	}
	else if(ACNT_GRP_SUPPORT)
	{
		if(!TCSUPPORT_ACCOUNT_METER_V2_VAL){
			acnt1 = (unchar)((skb->fe_resource_mark & ACNT1_MASK)>>ACNT1_OFFSET);
			acnt0 = (unchar)((skb->fe_resource_mark & ACNT0_MASK)>>ACNT0_OFFSET);
		}

		iblk2->port_ag = (((acnt1 & HWNAT_DEFAULT_ACNT1GRP) << ACNT0_MASK_LEN) | (acnt0 & HWNAT_DEFAULT_GRP_ID));
		if(TCSUPPORT_ACCOUNT_METER_V2_VAL){
			if (IS_IPV4_GRP(foe_entry)){
				set_ppe_entry_acnt_grp3(foe_entry, IPV4_HNAPT, acnt2);
				foe_entry->ipv4_hnapt.ts_id = meter0;
				set_ppe_entry_mtr_grp2(foe_entry, IPV4_HNAPT, meter1);
				set_ppe_entry_mtr_grp3(foe_entry, IPV4_HNAPT, meter2);
			}else if (IS_IPV6_GRP(foe_entry)){
				set_ppe_entry_acnt_grp3(foe_entry, IPV6_5T_ROUTE, acnt2);
				foe_entry->ipv6_5t_route.ts_id = meter0;
				set_ppe_entry_mtr_grp2(foe_entry, IPV6_5T_ROUTE, meter1);
				set_ppe_entry_mtr_grp3(foe_entry, IPV6_5T_ROUTE, meter2);		
			}else if (IS_L2_RRIDGE(foe_entry)){
				set_ppe_entry_acnt_grp3(foe_entry, L2_BRIDGE, acnt2);
				foe_entry->l2_bridge32.ts_id = meter0;
				set_ppe_entry_mtr_grp2(foe_entry, L2_BRIDGE, meter1);
				set_ppe_entry_mtr_grp3(foe_entry, L2_BRIDGE, meter2);
			}
		}
	}
	else if(isEN751627 || isEN7528)
	{
		if(isUpStreamFlow(magic, pinfo))
			dir = UP_STREAM;
		
		FE_API_GET_ACNT1_IDX(skb, dir, &acnt1);
		FE_API_GET_ACNT0_IDX(skb, dir, &acnt0);

		iblk2->port_ag = acnt0 & HWNAT_DEFAULT_GRP_ID;
        
		if (IS_IPV4_GRP(foe_entry))
			foe_entry->ipv4_hnapt.ts_id = (acnt1 & HWNAT_DEFAULT_ACNT1GRP);
		else if (IS_IPV6_GRP(foe_entry))
			foe_entry->ipv6_5t_route.ts_id = (acnt1 & HWNAT_DEFAULT_ACNT1GRP);
		else if (IS_L2_RRIDGE(foe_entry))
			foe_entry->l2_bridge32.ts_id = (acnt1 & HWNAT_DEFAULT_ACNT1GRP);

#ifdef EN7528_FDB
		FE_API_GET_ACNT2_IDX(skb, dir, &acnt2);

		if (IS_IPV4_GRP(foe_entry))
			foe_entry->ipv4_hnapt.cnt_g2 = (acnt2 & 0x7f);
		else if (IS_IPV6_GRP(foe_entry))
			foe_entry->ipv6_5t_route.cnt_g2 = (acnt2 & 0x7f);
		else if (IS_L2_RRIDGE(foe_entry))
			foe_entry->l2_bridge32.cnt_g2 = (acnt2 & 0x7f);
#endif
	}

	return 0;
}	

void PpeSetFqos(struct FoeEntry * foe_entry,unsigned char value)
{
	 _info_blk2 *iblk2;

	 if((0 != value)&&(1 != value))
	 	return;

	 if (IS_IPV4_GRP(foe_entry))
	 	 iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
	 else if (IS_IPV6_GRP(foe_entry))
	 	 iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
	 else if (IS_L2_RRIDGE(foe_entry))
		 iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
	 else
	 	 return;

	iblk2->fqos = value;

	return;
}
extern int wifi_ratelimit_enable;
void PpeSetInfoBlk2(struct FoeEntry * foe_entry, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag, uint32_t ds_to_qdma)
{
    _info_blk2 *iblk2;
    struct bf_info_blk1 bfib1_temp;

    if (IS_IPV4_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
    else if (IS_IPV6_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
    else if (IS_L2_RRIDGE(foe_entry))
	iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
    else
        return;

    if ((FP_QDMA_HW == fpidx) ||  (FP_QDMA1_HW == fpidx) 
		|| ((SUPPORT_QDMA_MULTICAST_1toN) && (FP_QDMA_MCAST == fpidx))
		|| ((SUPPORT_WDMA) && (FP_WDMA == fpidx)) 
		|| ((SUPPORT_ETHERNET_SERDES) && (FP_ETH_SERDES == fpidx))
		|| ((SUPPORT_METER_ACNT_IN_QDMA) && (FP_PDMA == fpidx) && (iblk2->fqos == 1) && !ppe_is_multicast_entry(foe_entry)))
    {
        iblk2->fqos = 1;

        if(left_to_right_test_mode && (FP_QDMA1_HW == fpidx))
            iblk2->fqos = 0;
    }
    else
    {
        iblk2->fqos = 0;
    }
	if(wifi_ratelimit_enable && FP_TDMA == fpidx)
		iblk2->fqos = 1;
		
    if((SUPPORT_DS_NTO_QDMA) && (ds_to_qdma == 0) && ((FP_GDMA1 == fpidx) || (FP_GDMA3 == fpidx) || (FP_GDMA4 == fpidx)))
		iblk2->fqos = 0;

	/* redirect WDMA fport */
	/* EN7528 WDMA => GDM1: FP-1&CHN-4 => WDMA-0&RX-0, FP-1&CHN-5 => WDMA-0&RX-1 */
	/* EN7528 WDMA => GDM1: FP-1&CHN-6 => WDMA-1&RX-0, FP-1&CHN-7 => WDMA-1&RX-1 */
	/* EN7580 WDMA => GDM3: FP-3&Nbq-0 => WDMA-0&RX-0, FP-3&Nbq-1 => WDMA-0&RX-1 */
	/* EN7580 WDMA => GDM3: FP-3&Nbq-2 => WDMA-1&RX-0, FP-3&Nbq-3 => WDMA-1&RX-1 */
	if((isEN7528) && (FP_GDMA3 == fpidx)){
		fpidx = FP_GDMA1;
	}

	iblk2->fpidx = fpidx;
	if(0x3f != port_mg)
		SetInfo2Mgrp(iblk2,port_mg);
	
    /* PCP should be enabled in 247-test */
    if(TCSUPPORT_BBF_247_VAL)
        iblk2->pcp = ENABLE;

    if(0x3f != port_ag)
    {
        if(TCSUPPORT_HWNAT_WAN_ACCOUNT_VAL)
        {
            if(SUPPORT_FLOW_ACNT){
                uint8_t acnt_mode = 0;
                FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt_mode);
                if(WAN_ITF_MODE != acnt_mode)
                {
                    /*input para port_ag is only for acnt0,do not modify acnt1 here*/
                    iblk2->port_ag &= ~HWNAT_DEFAULT_GRP_ID;
                    iblk2->port_ag |= (port_ag&HWNAT_DEFAULT_GRP_ID);
                }
        	}else if(!iblk2->port_ag)
                iblk2->port_ag = port_ag;
        }
        else
            iblk2->port_ag = port_ag;
    }
	
	if(fpidx == FP_DROP) {
		/* word copy from foe_entry info1 */
		(*((uint32_t*)&bfib1_temp)) = (*((uint32_t*)&foe_entry->bfib1));
	
		bfib1_temp.ttl = 0;
	
		/* word copy back to foe_entry info1 */
		(*((uint32_t*)&foe_entry->bfib1)) = (*((uint32_t*)&bfib1_temp));
	}

	return ;
}

int PpeGetInfoBlk2(struct FoeEntry * foe_entry, _info_blk2 * iblk2)
{
    if (IS_IPV4_GRP(foe_entry))
        memcpy(iblk2, (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2), sizeof(_info_blk2));
    else if (IS_IPV6_GRP(foe_entry))
        memcpy(iblk2, (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2), sizeof(_info_blk2));
    else if (IS_L2_RRIDGE(foe_entry))
	memcpy(iblk2, (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2), sizeof(_info_blk2));
    else
        return 0;

    return 1;
}

uint32_t PpeKeepAliveHandler(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	struct ethhdr *eth = NULL;
	uint16_t eth_type = ntohs(skb->protocol);
	uint32_t vlan1_gap = 0;
	uint32_t vlan2_gap = 0;
	uint32_t pppoe_gap = 0;
	struct vlan_hdr *vh;
	struct iphdr *iph = NULL;
	struct tcphdr *th = NULL;
	struct udphdr *uh = NULL;

	/*
	 * try to recover to original SMAC/DMAC, but we don't have such information.
	 * just use SMAC as DMAC and set Multicast address as SMAC.
	 */
	eth = (struct ethhdr *)(skb->data - ETH_HLEN);

	FoeGetMacInfo(eth->h_dest, eth->h_source);
	FoeGetMacInfo(eth->h_source, eth->h_dest);
	eth->h_source[0] = 0x1;	//change to multicast packet, make bridge not learn this packet
	if (eth_type == ETH_P_8021Q) {
		vlan1_gap = VLAN_HLEN;
		vh = (struct vlan_hdr *)skb->data;
		if (ntohs(vh->h_vlan_TCI) == WAN_PORT_VLAN_ID) {
			/* It make packet like coming from LAN port */
			vh->h_vlan_TCI = htons(LAN_PORT_VLAN_ID);
		}else{
			/* It make packet like coming from WAN port */
			vh->h_vlan_TCI = htons(WAN_PORT_VLAN_ID);

		}	
		if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
			pppoe_gap = 8;
		} else if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_8021Q) {
			vlan2_gap = VLAN_HLEN;
			vh = (struct vlan_hdr *)(skb->data + VLAN_HLEN);

			/* VLAN + VLAN + PPPoE */
			if (ntohs(vh->h_vlan_encapsulated_proto) ==
			    ETH_P_PPP_SES) {
				pppoe_gap = 8;
			} else {
				/* VLAN + VLAN + IP */
				eth_type = ntohs(vh->h_vlan_encapsulated_proto);
			}
		} else {
			/* VLAN + IP */
			eth_type = ntohs(vh->h_vlan_encapsulated_proto);
		}
	}

	/* Only Ipv4 NAT need KeepAlive Packet to refresh iptable */
	if (eth_type == ETH_P_IP) {
		iph =
		    (struct iphdr *)(skb->data + vlan1_gap + vlan2_gap +
				     pppoe_gap);

		//Recover to original layer 4 header 
		if (iph->protocol == IPPROTO_TCP) {
			th = (struct tcphdr *)((uint8_t *) iph + iph->ihl * 4);
			FoeToOrgTcpHdr(foe_entry, iph, th);

		} else if (iph->protocol == IPPROTO_UDP) {
			uh = (struct udphdr *)((uint8_t *) iph + iph->ihl * 4);
			FoeToOrgUdpHdr(foe_entry, iph, uh);
		}
		//Recover to original layer 3 header 
		FoeToOrgIpHdr(foe_entry, iph);
	} else if (eth_type == ETH_P_IPV6) {
		/* Nothing to do */
	} else {
		return 1;
	}

	/*
	 * Ethernet driver will call eth_type_trans() to update skb->pkt_type.
	 * If(destination mac != my mac) 
	 *   skb->pkt_type=PACKET_OTHERHOST;
	 *
	 * In order to pass ip_rcv() check, we change pkt_type to PACKET_HOST here
	 */
	skb->pkt_type = PACKET_HOST;
	return 1;

}

/*
dir -- 0:upstream 1:downstream
*/
 int32_t ppeCpuPathRatelimitByMeter(struct sk_buff* skb,FETxMsg_T* txMsg,struct port_info* pInfo,unsigned char dir,unsigned short mtrIndex)
{
	int ret = -1;

	if(!isEN7526c && !isEN751627)
		return 0;

	if(4 == txMsg->raw.fport)/*to ppe*/
		return 0;
	else if((0 == dir)&&(2 == txMsg->raw.fport))
		txMsg->raw1.fport_ppe = 6;/*upstream send to QDMA_WAN HWF*/
	else if((1 == dir)&&(1 == txMsg->raw.fport))
		txMsg->raw1.fport_ppe = 3;/*downstream send to QDMA_LAN HWF*/
	else
		txMsg->raw1.fport_ppe = txMsg->raw.fport;
	
	txMsg->raw1.mtr = 1;
	txMsg->raw1.channel_ppe = txMsg->raw.channel;
	if(0 == dir)
		txMsg->raw1.channel = 7;/*fix to 7 at wan side for upstream*/
	else if(1 == dir)
		txMsg->raw1.channel = 6;/*fix to 6 at lan side for downstream*/
	txMsg->raw1.fport = 4;
	txMsg->raw1.mtr_index = mtrIndex;
	
	ret = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN, skb, txMsg->word[0], txMsg->word[1], pInfo) ;

	return ret;
}

int hwnat_is_alive_pkt(struct sk_buff* skb)
{
	if (IS_MAGIC_TAG_VALID(skb) && FOE_AI(skb) >= 0x13 && FOE_AI(skb) <= 0x15)
		return 1;

	return 0;
}


int hwnat_skb_cpu_rason(struct sk_buff* skb)
{
    if(IS_MAGIC_TAG_VALID(skb))
    {
        return FOE_AI(skb);
    }

    return -1;
}

int hwnat_skb_to_foe(struct sk_buff* skb)
{
	if (IS_MAGIC_TAG_VALID(skb))
		return FOE_ENTRY_NUM(skb);

	return -1;
}


int hwnat_set_recover_info(struct sk_buff* skb,	struct sock* sk,int flag)
{
	switch(flag)
	{
		case FOEINFOSTORE:
			if(TCSUPPORT_CDS_VAL&&skb->ppe_info_flag)
			{
				sk->sk_foe_info.ppe_ai = skb->ppe_ai;
				sk->sk_foe_info.ppe_foe_entry =  skb->ppe_foe_entry;
				sk->sk_foe_info.ppe_magic = skb->ppe_magic;
			}
			else
			{
				sk->sk_foe_info.ppe_ai = FOE_AI(skb);
				sk->sk_foe_info.ppe_foe_entry = FOE_ENTRY_NUM(skb);
				sk->sk_foe_info.ppe_magic = FOE_MAGIC_TAG(skb);
			}
		break;

		case FOEINFORECOVER:
			FOE_AI(skb) = sk->sk_foe_info.ppe_ai;
			FOE_ENTRY_NUM(skb) = sk->sk_foe_info.ppe_foe_entry;
			FOE_MAGIC_TAG(skb) = sk->sk_foe_info.ppe_magic;
		break;
	}
	return 0;
}

int hwnat_is_bind_and_L3_pkt(struct sk_buff* skb){
	int index = -1;
	struct FoeEntry *foe_entry ;
	
	if (IS_MAGIC_TAG_VALID(skb))
		index = FOE_ENTRY_NUM(skb);

	if (index <0 || index >= FOE_4TB_SIZ_TOTAL)
		return -1;
	
	foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + index*SIZE_OF_FOE_ENTRY);

	if ((foe_entry->bfib1.state == BIND) &&((foe_entry->bfib1.pkt_type == IPV4_HNAPT) ||(foe_entry->bfib1.pkt_type == IPV6_5T_ROUTE))){
		return index;
	}
	else{
		return -1;
	}
}

int hwnat_is_from_force_cpu_pkt(struct sk_buff* skb){
	int index = -1;

	index = hwnat_is_bind_and_L3_pkt(skb);
	if(index == -1)
		return -1;
	if (FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU){
		return index;
	}
	else{
		return -1;
	}
	
}

uint32_t FoeDumpPkt(struct sk_buff * skb)
{
	struct FoeEntry *foe_entry;

	foe_entry = getPpeEntry(FOE_ENTRY_NUM(skb));
	if(foe_entry == NULL)
		return 0;
	
	NAT_PRINT("\nRx===<FOE_Entry=%d>=====\n", FOE_ENTRY_NUM(skb));
	NAT_PRINT("RcvIF=%s\n", skb->dev->name);
	NAT_PRINT("FOE_Entry=%d\n", FOE_ENTRY_NUM(skb));
	NAT_PRINT("CPU Reason=%s (0x%x)\n", crsnArr[FOE_AI(skb)], FOE_AI(skb));
	NAT_PRINT("UDF=%d\n", FOE_UDF(skb));

	NAT_PRINT("SP=%d\n", FOE_SP(skb));

	/* PPE: IPv4 packet=IPV4_HNAT IPv6 packet=IPV6_ROUTE */
	if (IS_IPV4_GRP(foe_entry) || IS_IPV4_DSLITE(foe_entry)) {
		NAT_PRINT("Information Block 1=%x\n", foe_entry->ipv4_hnapt.info_blk1);
		NAT_PRINT("SIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.sip));
		NAT_PRINT("DIP=%s\n", Ip2Str(foe_entry->ipv4_hnapt.dip));
		NAT_PRINT("SPORT=%d\n", foe_entry->ipv4_hnapt.sport);
		NAT_PRINT("DPORT=%d\n", foe_entry->ipv4_hnapt.dport);
		if (IS_IPV4_DSLITE(foe_entry)) {
			NAT_PRINT("Information Block 2=%x\n",
				  foe_entry->ipv4_dslite.info_blk2);
		} else {
			NAT_PRINT("Information Block 2=%x\n",
				  foe_entry->ipv4_hnapt.info_blk2);
		}
	}

	else if (IS_IPV6_GRP(foe_entry)) {
		NAT_PRINT("Information Block 1=%x\n", foe_entry->ipv6_5t_route.info_blk1);
		NAT_PRINT("IPv6_SIP=%08X:%08X:%08X:%08X\n",
			  foe_entry->ipv6_5t_route.ipv6_sip0,
			  foe_entry->ipv6_5t_route.ipv6_sip1,
			  foe_entry->ipv6_5t_route.ipv6_sip2,
			  foe_entry->ipv6_5t_route.ipv6_sip3);
		NAT_PRINT("IPv6_DIP=%08X:%08X:%08X:%08X\n",
			  foe_entry->ipv6_5t_route.ipv6_dip0,
			  foe_entry->ipv6_5t_route.ipv6_dip1,
			  foe_entry->ipv6_5t_route.ipv6_dip2,
			  foe_entry->ipv6_5t_route.ipv6_dip3);
		if(IS_IPV6_FLAB_EBL()) {
			NAT_PRINT("Flow Label=%08X\n", (foe_entry->ipv6_5t_route.sport << 16) | 
							(foe_entry->ipv6_5t_route.dport));
		} else {
			NAT_PRINT("SPORT=%d\n", foe_entry->ipv6_5t_route.sport);
			NAT_PRINT("DPORT=%d\n", foe_entry->ipv6_5t_route.dport);
		}
		NAT_PRINT("Information Block 2=%x\n",
			  foe_entry->ipv6_5t_route.info_blk2);
	}

	else if (IS_L2_RRIDGE(foe_entry)) {
        if (PPE_L2B_ENTRY_SUPPORT)
        {
    		NAT_PRINT("Information Block 1=%08X\n", foe_entry->l2_bridge32.info_blk1);
    		NAT_PRINT("Information Block 2=%08X\n", foe_entry->l2_bridge32.info_blk2);
    		NAT_PRINT("L2 Bridge entry\n");
    		NAT_PRINT("IN DMAC=%02X:%02X:%02X:%02X:%02X:%02X IN SMAC=%02X:%02X:%02X:%02X:%02X:%02X\n",
    		 NIPQUAD(*(unsigned int *)(&foe_entry->l2_bridge32.in_dmac_hi[0])),
    		 NIPTWOBYTE(*(unsigned short *)(&foe_entry->l2_bridge32.in_dmac_lo[0])),
    		 NIPQUAD(*(unsigned int *)(&foe_entry->l2_bridge32.in_smac_hi[0])),
    		 NIPTWOBYTE(*(unsigned short *)(&foe_entry->l2_bridge32.in_smac_lo[0])));
    		NAT_PRINT("IN_VLAN: %x\n", foe_entry->l2_bridge32.in_vlan);
    		NAT_PRINT("HSK_L2: %x\n", foe_entry->l2_bridge32.info_hsk_l2);
        }
        else
        {
    		NAT_PRINT("Information Block 1=%x\n", foe_entry->l2_bridge32.info_blk1);
    		NAT_PRINT("Information Block 2: %08X\n", foe_entry->l2_bridge32.info_blk2);
    		NAT_PRINT("L2 Bridge entry\n");
        
            if (isL2bSmacEnable())
            {
        		NAT_PRINT("IN DMAC=%02X:%02X:%02X:%02X:%02X:%02X IN SMAC=%02X:%02X:%02X:%02X:%02X:%02X\n",
        		     		 NIPQUAD(*(unsigned int *)(&foe_entry->l2_bridge32.in_dmac_hi[0])),
                    		 NIPTWOBYTE(*(unsigned short *)(&foe_entry->l2_bridge32.in_dmac_lo[0])),
                    		 NIPTWOBYTE(*(unsigned short *)(&foe_entry->l2_bridge32.in_smac_hi[0])),
                    		 NIPQUAD(*(unsigned int *)(&foe_entry->l2_bridge32.in_smac_lo[0])));            
            }
            else
            {
        		NAT_PRINT("IN DMAC=%02X:%02X:%02X:%02X:%02X:%02X\n",
                            NIPQUAD(*(unsigned int *)(&foe_entry->l2_bridge32.in_dmac_hi[0])),
                            NIPTWOBYTE(*(unsigned short *)(&foe_entry->l2_bridge32.in_dmac_lo[0])));
                NAT_PRINT("IN_VLAN: %x\n", foe_entry->l2_bridge32.in_vlan);
                NAT_PRINT("HSK_L2: %x\n", foe_entry->l2_bridge32.info_hsk_l2);
            }
        }

	}
	else {
		NAT_PRINT("unknown Pkt_type=%d\n", foe_entry->bfib1.pkt_type);
	}

	NAT_PRINT("==================================\n");

	return 1;

}

void PpeSetWifiForceQos(struct FoeEntry * foe_entry)
{
	char type = 0;

	if (IS_IPV4_GRP(foe_entry))
	    type = 1;
	else if (IS_IPV6_GRP(foe_entry))
	    type = 2;

	if(type == 1){
		foe_entry->ipv4_hnapt.fqos = 1;
		foe_entry->ipv4_hnapt.qid = 0;
		foe_entry->ipv4_hnapt.ts_id = 0x3;
	}
	else if(type == 2){
		foe_entry->ipv6_5t_route.fqos = 1;
		foe_entry->ipv6_5t_route.qid = 0;
		foe_entry->ipv6_5t_route.ts_id = 0x3;
	}

	return;
}

void PpeSetRxForceRingIdx(struct FoeEntry * foe_entry, uint8_t ring_index)
{
	char type = 0;
	char ring_mask = 0x7;
	
	if (IS_IPV4_GRP(foe_entry))
		type = 1;
	else if (IS_IPV6_GRP(foe_entry))
		type = 2;
	else if (IS_L2_RRIDGE(foe_entry))
		type = 3;

	if(isEN7528)
		ring_mask = 0xf;
	else if(FOE_NBQ_CFG_SUPPORT)
		ring_mask = 0x1f;
	
	if(isEN751627){
		if(type == 1){
			foe_entry->ipv4_hnapt.qid = ring_index & ring_mask;
		}else if(type == 2){
			foe_entry->ipv6_5t_route.qid = ring_index & ring_mask;
		}else if(type ==3){
			foe_entry->l2_bridge32.qid = ring_index & ring_mask;
		}
	}
	else if(FOE_NBQ_CFG_SUPPORT){
		if(type == 1){
            set_info2_nbq(foe_entry, IPV4_HNAPT, ring_index & ring_mask);
		}else if(type == 2){
		    set_info2_nbq(foe_entry, IPV6_5T_ROUTE, ring_index & ring_mask);
		}else if(type ==3){
            set_info2_nbq(foe_entry, L2_BRIDGE, ring_index & ring_mask);
		}
	}

	return;
}

void PpeSetPortInfo(struct FoeEntry * foe_entry, struct port_info * pinfo, int magic)
{
    char vpm = 0;
    char type = 0;
    unchar fast_lan = 0, fast_wan = 0, fast_xfi = 0, fast_olt = 0;
    unsigned char wifi_tsid_enable = 0; 
	unchar hqos_lan_en = 0, hqos_wan_en = 0;
	int tmpQid = -1;
    
    if (IS_IPV4_GRP(foe_entry))
        type = 1;
    else if (IS_IPV6_GRP(foe_entry))
        type = 2;
    else if (IS_L2_RRIDGE(foe_entry))
        type = 3;

	//setup TPID
    if(SUPPORT_VPM_FROM_PARSER) {
        vpm = ppe_trans_tpid_to_vpm(PpeParseResult.vlan_tag);
    } else {
        if(type == 1){
            vpm = ppe_trans_tpid_to_vpm(htons(foe_entry->ipv4_hnapt.etype));
        }else if(type == 2){
            vpm = ppe_trans_tpid_to_vpm(htons(foe_entry->ipv6_5t_route.etype));
        }
        else if(type == 3){
            vpm = ppe_trans_tpid_to_vpm(htons(foe_entry->l2_bridge32.etype));
        }
    }
	foe_entry->bfib1.vpm = vpm;

	if (SUPPORT_FAST_PATH){
		// fast_wan = fast_path_dir[FAST_PATH_DIR_WAN];
		// fast_lan = fast_path_dir[FAST_PATH_DIR_LAN];
		fast_xfi = fast_path_dir[FAST_PATH_DIR_XSI_ETHER];
		fast_olt = fast_path_dir[FAST_PATH_DIR_OLT];
		if(pinfo->fast) {
			fast_wan = 1;
			fast_lan = 1;
		} else {
			fast_wan = fast_path_dir[FAST_PATH_DIR_WAN];
			fast_lan = fast_path_dir[FAST_PATH_DIR_LAN];
		}
		if (hwnat_fast_slow_down_stream_flag == 1) {
			fast_xfi = 1;
			fast_lan = 1;
		} else if (hwnat_fast_slow_down_stream_flag == 2) {
			fast_xfi = 0;
			fast_lan = 0;
		} 
		if (hwnat_fast_slow_up_stream_flag == 1) {
			fast_wan = 1;
		} else if (hwnat_fast_slow_up_stream_flag == 2) {
			fast_wan = 0;
		}

	}
	if(SUPPORT_QDMA_HQOS){
		hqos_lan_en = QDMA_API_GET_HQOS_EN(ECNT_QDMA_LAN);
		hqos_wan_en = QDMA_API_GET_HQOS_EN(ECNT_QDMA_WAN);
	}
	
	//setup Stag to wan define
	switch(magic){
        case FOE_MAGIC_ATM:
        case FOE_MAGIC_PTM:
        case FOE_MAGIC_PTM_LAN:
        case FOE_MAGIC_EPON:
        case FOE_MAGIC_GPON:
		case FOE_MAGIC_AE_WAN:
            if(type == 1){
                foe_entry->ipv4_hnapt.channel = pinfo->channel;
                foe_entry->ipv4_hnapt.etype = pinfo->stag;
                foe_entry->ipv4_hnapt.fqos = 1;
                foe_entry->ipv4_hnapt.qid = pinfo->txq;
                foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
                if (SUPPORT_PSE_NBQ) {
                    set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->nbq);
                }
                if (SUPPORT_FAST_PATH) {
                    set_info2_fast(foe_entry, IPV4_HNAPT, fast_wan);
                }
				if(hqos_wan_en && pinfo->channel<16){
					foe_entry->ipv4_hnapt.channel = pinfo->channel+16;
					set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->channel);	
				}
				
            }
            else if(type == 2){
                foe_entry->ipv6_5t_route.channel = pinfo->channel;
                foe_entry->ipv6_5t_route.etype = pinfo->stag;
                foe_entry->ipv6_5t_route.fqos = 1;
                foe_entry->ipv6_5t_route.qid = pinfo->txq;
                foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
                if (SUPPORT_PSE_NBQ) {
                    set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->nbq);
                }
                if (SUPPORT_FAST_PATH) {
                    set_info2_fast(foe_entry, IPV6_5T_ROUTE, fast_wan);
                }
				if(hqos_wan_en && pinfo->channel<16){
					foe_entry->ipv4_hnapt.channel = pinfo->channel+16;
					set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->channel);
				}
            }
            else if(type == 3){
                foe_entry->l2_bridge32.channel = pinfo->channel;
                foe_entry->l2_bridge32.etype = pinfo->stag;
                foe_entry->l2_bridge32.fqos = 1;
                foe_entry->l2_bridge32.qid = pinfo->txq;
                foe_entry->l2_bridge32.ts_id = pinfo->tsid;
                if (SUPPORT_PSE_NBQ) {
                    set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->nbq);
                }
                if (SUPPORT_FAST_PATH) {
                    set_info2_fast(foe_entry, L2_BRIDGE, fast_wan);
                }
				if(hqos_wan_en && pinfo->channel<16){
					foe_entry->ipv4_hnapt.channel = pinfo->channel+16;
					set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->channel);
				}
            }
            break;
		case FOE_MAGIC_XSI:
		case FOE_MAGIC_XSI_GDM4:
            if(type == 1){
                if (SUPPORT_XSI_INTERFACE) {
                    foe_entry->ipv4_hnapt.channel = pinfo->channel;        
                    foe_entry->ipv4_hnapt.etype = pinfo->stag;
                    foe_entry->ipv4_hnapt.fqos = 1;
                    foe_entry->ipv4_hnapt.qid = pinfo->txq;
                    foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
                    if (SUPPORT_FAST_PATH) {
						if(SUPPORT_QDMA_AUTOMODE)
							set_info2_fast(foe_entry, IPV4_HNAPT, pinfo->fast);
						else{
							if (pinfo->fast && !ppe_is_multicast_entry(foe_entry)) { 
								set_info2_fast(foe_entry, IPV4_HNAPT, 1);
							} else {
						        set_info2_fast(foe_entry, IPV4_HNAPT, fast_xfi);
							}
						}
                        if(pinfo->udf == SKB_FROM_WIFI){ //means skb from wifi ,set in hsgmii
				set_info2_fast(foe_entry, IPV4_HNAPT, pinfo->fast);
			}
                    } else {
                        set_info2_fast(foe_entry, IPV4_HNAPT, 0);
                    }
                    set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->nbq);
                }
            }
            else if(type == 2){
                if (SUPPORT_XSI_INTERFACE) {
                    foe_entry->ipv6_5t_route.channel = pinfo->channel;            
                    foe_entry->ipv6_5t_route.etype = pinfo->stag;
                    foe_entry->ipv6_5t_route.fqos = 1;
                    foe_entry->ipv6_5t_route.qid = pinfo->txq;
                    foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
                    if (SUPPORT_FAST_PATH) {
			if (pinfo->fast && !ppe_is_multicast_entry(foe_entry)) {	
				set_info2_fast(foe_entry, IPV6_5T_ROUTE, 1);
			} else {
                        	set_info2_fast(foe_entry, IPV6_5T_ROUTE, fast_xfi);
			}
			if(pinfo->udf == SKB_FROM_WIFI){ //means skb from wifi ,set in hsgmii
				set_info2_fast(foe_entry, IPV6_5T_ROUTE, pinfo->fast);
			}
                    } else {
                        set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
                    }
                    set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->nbq);
                }
            }
            else if(type == 3){
                if (SUPPORT_XSI_INTERFACE) {
                    foe_entry->l2_bridge32.channel = pinfo->channel;
                    foe_entry->l2_bridge32.etype = pinfo->stag;
                    foe_entry->l2_bridge32.fqos = 1;
                    foe_entry->l2_bridge32.qid = pinfo->txq;
                    foe_entry->l2_bridge32.ts_id = pinfo->tsid;
                    if (SUPPORT_FAST_PATH) {
			if (pinfo->fast && !ppe_is_multicast_entry(foe_entry)) {
				set_info2_fast(foe_entry, L2_BRIDGE, 1);
			} else {
                        	set_info2_fast(foe_entry, L2_BRIDGE, fast_xfi);
			}
			if(pinfo->udf == SKB_FROM_WIFI){ //means skb from wifi ,set in hsgmii
				set_info2_fast(foe_entry, L2_BRIDGE, pinfo->fast);
			}
                    } else {
                        set_info2_fast(foe_entry, L2_BRIDGE, 0);
                    }
                    set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->nbq);
                }
            }
            break;
     case FOE_MAGIC_OLT:
            if(type == 1){
                if (SUPPORT_OLT_INTERFACE) {
                    foe_entry->ipv4_hnapt.channel = pinfo->channel; 	   
                    foe_entry->ipv4_hnapt.etype = pinfo->stag;
                    foe_entry->ipv4_hnapt.fqos = 1;
                    foe_entry->ipv4_hnapt.qid = pinfo->txq;
                    foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
                    if (SUPPORT_FAST_PATH)
                        set_info2_fast(foe_entry, IPV4_HNAPT, fast_olt);
                    else
                        set_info2_fast(foe_entry, IPV4_HNAPT, 0);
                    set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->nbq);
                }
            }
            else if(type == 2){
                if (SUPPORT_OLT_INTERFACE) {
                    foe_entry->ipv6_5t_route.channel = pinfo->channel;			  
                    foe_entry->ipv6_5t_route.etype = pinfo->stag;
                    foe_entry->ipv6_5t_route.fqos = 1;
                    foe_entry->ipv6_5t_route.qid = pinfo->txq;
                    foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
                    if (SUPPORT_FAST_PATH)
                        set_info2_fast(foe_entry, IPV6_5T_ROUTE, fast_olt);
                    else
                        set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
                    set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->nbq);
                }
            }
            else if(type == 3){
                if (SUPPORT_OLT_INTERFACE) {
                    foe_entry->l2_bridge32.channel = pinfo->channel;
                    foe_entry->l2_bridge32.etype = pinfo->stag;
                    foe_entry->l2_bridge32.fqos = 1;
                    foe_entry->l2_bridge32.qid = pinfo->txq;
                    foe_entry->l2_bridge32.ts_id = pinfo->tsid;
                    if (SUPPORT_FAST_PATH)
                        set_info2_fast(foe_entry, L2_BRIDGE, fast_olt);
                    else
                        set_info2_fast(foe_entry, L2_BRIDGE, 0);
                    set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->nbq);
                }
            }
            break;
        case FOE_MAGIC_GE:
			if(ecnt_ppe_set_port_info_magic_ge_hook(foe_entry,pinfo,type,fast_lan))
			{
				if(type == 1){
					/* to LAN traffic channel by destination port */
					if (magic == FOE_MAGIC_GE){
						foe_entry->ipv4_hnapt.channel = pinfo->channel;
					}
					if ( pinfo->txq_is_valid){
						foe_entry->ipv4_hnapt.fqos = 1;
						foe_entry->ipv4_hnapt.qid = pinfo->txq;
					}
					else {
						foe_entry->ipv4_hnapt.fqos = 0;
						foe_entry->ipv4_hnapt.qid = 0;
					}

					ECNT_API_BBF247_SET_HWNAT_QID(&tmpQid,pinfo->txq);	//can't & xx.qid
					if(tmpQid != -1)
						foe_entry->ipv4_hnapt.qid = pinfo->txq;

					foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
					
					if (SUPPORT_STAG_FROM_INFO) {
						foe_entry->ipv4_hnapt.etype = pinfo->stag;
					}
					if (SUPPORT_FAST_PATH) {
						set_info2_fast(foe_entry, IPV4_HNAPT, fast_lan);
					}
					if (SUPPORT_PSE_NBQ) {
						set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->nbq);
					}

					if(hqos_lan_en && pinfo->channel<16){
						foe_entry->ipv4_hnapt.channel = pinfo->channel+16;
						set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->channel);	
					}
				}
				else if(type == 2){
					/* to LAN traffic channel by destination port */
					if (magic == FOE_MAGIC_GE){
						foe_entry->ipv6_5t_route.channel = pinfo->channel;
					}
					if (pinfo->txq_is_valid){
						foe_entry->ipv6_5t_route.fqos = 1;
						foe_entry->ipv6_5t_route.qid = pinfo->txq;
					}
					else{
						foe_entry->ipv6_5t_route.fqos = 0;
						foe_entry->ipv6_5t_route.qid = 0;
					}
					foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
					if (SUPPORT_STAG_FROM_INFO) {
						foe_entry->ipv6_5t_route.etype = pinfo->stag;
					}
					if (SUPPORT_FAST_PATH) {
						set_info2_fast(foe_entry, IPV6_5T_ROUTE, fast_lan);
					}
					if (SUPPORT_PSE_NBQ) {
						set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->nbq);
					}

					if(hqos_lan_en && pinfo->channel<16){
						foe_entry->ipv4_hnapt.channel = pinfo->channel+16;
						set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->channel);	
					}
				}
				else if(type == 3){
					/* to LAN traffic channel by destination port */
					if (magic == FOE_MAGIC_GE){
						foe_entry->l2_bridge32.channel = pinfo->channel;
					}
					if (pinfo->txq_is_valid){
						foe_entry->l2_bridge32.fqos = 1;
						foe_entry->l2_bridge32.qid = pinfo->txq;
					}
					else{
						foe_entry->l2_bridge32.fqos = 0;
						foe_entry->l2_bridge32.qid = 0;
					}
					
					ECNT_API_BBF247_SET_HWNAT_QID(&tmpQid,pinfo->txq);  //can't & xx.qid
					if(tmpQid != -1)
						foe_entry->l2_bridge32.qid = pinfo->txq;
					
					foe_entry->l2_bridge32.ts_id = pinfo->tsid;
					if (SUPPORT_STAG_FROM_INFO) {
						foe_entry->l2_bridge32.etype = pinfo->stag;
					}
					if (SUPPORT_FAST_PATH) {
						set_info2_fast(foe_entry, L2_BRIDGE, fast_lan);
					}
					if (SUPPORT_PSE_NBQ) {
						set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->nbq);
					}

					if(hqos_lan_en && pinfo->channel<16){
						foe_entry->ipv4_hnapt.channel = pinfo->channel+16;
						set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->channel);	
					}
				}
			}
			
			break;
        case FOE_MAGIC_NPU_OFFLOAD:
            break;
		case FOE_MAGIC_GRE_HWDOWN_1:
		case FOE_MAGIC_VXLAN_HWDOWN_1:
		case FOE_MAGIC_L2TP_HWDOWN_1:
			if(type == 1){
  
                foe_entry->ipv4_hnapt.channel = pinfo->channel;        
                foe_entry->ipv4_hnapt.etype = magic;
                foe_entry->ipv4_hnapt.fqos = 1;
                foe_entry->ipv4_hnapt.qid = pinfo->txq;
                foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
                set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->nbq);
            }
            else if(type == 2){
                foe_entry->ipv6_5t_route.channel = pinfo->channel;            
                foe_entry->ipv6_5t_route.etype = magic;
                foe_entry->ipv6_5t_route.fqos = 1;
                foe_entry->ipv6_5t_route.qid = pinfo->txq;
                foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
                set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->nbq);
            }
            else if(type == 3){
                foe_entry->l2_bridge32.channel = pinfo->channel;
                foe_entry->l2_bridge32.etype = magic;
                foe_entry->l2_bridge32.fqos = 1;
                foe_entry->l2_bridge32.qid = pinfo->txq;
                foe_entry->l2_bridge32.ts_id = pinfo->tsid;
                set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->nbq);
            }
            break;
        case FOE_MAGIC_OFFLOAD://fall through
        default:
            //should not run here
            if(type == 1){
                /* EN7516 & EN7580 set to rx ring 1, use this bit to set */
                /* qid for EN7516 rx ring index, nbq for EN7580 rx ring index*/
                if (SUPPORT_RX_RING_FROM_QID) {
                    foe_entry->ipv4_hnapt.qid = 1;
                }else{
                    foe_entry->ipv4_hnapt.qid = 0;
                }
                if ( magic == FOE_MAGIC_WLAN_7615 ){
                    foe_entry->ipv4_hnapt.channel = pinfo->channel;
                    foe_entry->ipv4_hnapt.etype = pinfo->stag;
                    foe_entry->ipv4_hnapt.fqos = 1;
                } else {
                	if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL) 
                		&& magic == FOE_MAGIC_WLAN ) {
						foe_entry->ipv4_hnapt.etype = pinfo->stag;
						foe_entry->ipv4_hnapt.act_dp = pinfo->udf;
					}
                    if(SUPPORT_METER_ACNT_IN_QDMA){
                        FE_API_GET_METER_ENABLE(&wifi_tsid_enable, pinfo->tsid, DOWN_STREAM);
                        if(pinfo->tsid != HWNAT_DEFAULT_MTRGRP && wifi_tsid_enable == 1)
                            foe_entry->ipv4_hnapt.fqos = 1;
                        else
                            foe_entry->ipv4_hnapt.fqos = 0;
                    }else{
                        foe_entry->ipv4_hnapt.fqos = 0;
                    }
                }
                if (SUPPORT_PSE_NBQ) {                   
                    foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
                    if((pinfo->nbq==0)&&(FOE_MAGIC_WLAN_7615 != magic))
                        set_info2_nbq(foe_entry, IPV4_HNAPT, RING_IDX_1);
                    else
                        set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->nbq);
                }
            }
            else if(type == 2){
                if (SUPPORT_RX_RING_FROM_QID) {
                    foe_entry->ipv6_5t_route.qid = 1;
                }else{
                    foe_entry->ipv6_5t_route.qid = 0;
                }
                if ( magic == FOE_MAGIC_WLAN_7615 ){
                    foe_entry->ipv6_5t_route.channel = pinfo->channel;
                    foe_entry->ipv6_5t_route.etype = pinfo->stag;
                    foe_entry->ipv6_5t_route.fqos = 1;
                } else {
					if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL) 
						&& magic == FOE_MAGIC_WLAN ) {
						foe_entry->ipv6_5t_route.etype = pinfo->stag;
						foe_entry->ipv6_5t_route.act_dp = pinfo->udf;
					}
                    if(SUPPORT_METER_ACNT_IN_QDMA){
                        FE_API_GET_METER_ENABLE(&wifi_tsid_enable, pinfo->tsid, DOWN_STREAM);
                        if(pinfo->tsid != HWNAT_DEFAULT_MTRGRP && wifi_tsid_enable)
                            foe_entry->ipv6_5t_route.fqos = 1;
                        else
                            foe_entry->ipv6_5t_route.fqos = 0;
                    }else{
                        foe_entry->ipv6_5t_route.fqos = 0;
                    }
                }
                if (SUPPORT_PSE_NBQ) {                    
                    foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
                    if((pinfo->nbq==0)&&(FOE_MAGIC_WLAN_7615 != magic))
                        set_info2_nbq(foe_entry, IPV6_5T_ROUTE, RING_IDX_1);
                    else
                        set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->nbq);
                }
            }
            else if(type == 3){
                if (SUPPORT_RX_RING_FROM_QID) {
                    foe_entry->l2_bridge32.qid = 1;
                }else{
                    foe_entry->l2_bridge32.qid = 0;
                }
                if ( magic == FOE_MAGIC_WLAN_7615 ){
                    foe_entry->l2_bridge32.channel = pinfo->channel;
                    foe_entry->l2_bridge32.etype = pinfo->stag;
                    foe_entry->l2_bridge32.fqos = 1;
                } else {
					if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL) 
						&& magic == FOE_MAGIC_WLAN ) {
						foe_entry->l2_bridge32.etype = pinfo->stag;
						foe_entry->l2_bridge32.act_dp = pinfo->udf;
					}
                    if(SUPPORT_METER_ACNT_IN_QDMA){
                        FE_API_GET_METER_ENABLE(&wifi_tsid_enable, pinfo->tsid, DOWN_STREAM);
                        if(pinfo->tsid != HWNAT_DEFAULT_MTRGRP && wifi_tsid_enable)
                            foe_entry->l2_bridge32.fqos = 1;
                        else
                            foe_entry->l2_bridge32.fqos = 0;
                    }else{
                        foe_entry->l2_bridge32.fqos = 0;
                    }
                }
                if (SUPPORT_PSE_NBQ) {
                    foe_entry->l2_bridge32.ts_id = pinfo->tsid;
                    if((pinfo->nbq==0)&&(FOE_MAGIC_WLAN_7615 != magic))
                        set_info2_nbq(foe_entry, L2_BRIDGE, RING_IDX_1);
                    else
                        set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->nbq);
                }
            }
            break;
	}
		
}

__IMEM int PpeIfcHitinfoHandler(struct sk_buff * skb, unsigned char hit, unsigned int ifc_id)
{
    if(SUPPORT_HIT_IFC_ACTION)
    {
        FOE_Hit_IFC(skb) =  (hit  & 0x1);
        if(FOE_Hit_IFC(skb))
        {
            FOE_IFC_IDX(skb) = ifc_id;
        }
    }
    
    return 0;
}

int ppeIsIfcHit(unsigned int foe_num,  struct FoeEntry * foe_entry)
{	
    if (SUPPORT_HIT_IFC_ACTION && (IS_L2_RRIDGE(foe_entry)) 
            && (foe_ext[foe_num].ifc_hit_info.match_action))
    {
        return 1;
    }

    return 0;
}

int ppeCheckIfcHit(struct sk_buff * skb,struct FoeEntry * foe_entry, int magic, struct port_info * pinfo)
{   
    int ret = 0;
	unsigned int foe_num = FOE_ENTRY_NUM(skb);
    unsigned char action = 0;
    
    if(!SUPPORT_HIT_IFC_ACTION)
    {
        return 0;
    }
    
    if((FOE_MAGIC_ATM == magic) || (FOE_MAGIC_PTM == magic))
    {
        return 0;
    }
    
#if 0
    printk("IS_L2_RRIDGE(foe_entry) = %d, foe_num = %u, FOE_Hit_IFC(skb)  = %d\n", 
            IS_L2_RRIDGE(foe_entry), foe_num, FOE_Hit_IFC(skb));
#endif

    if(IS_L2_RRIDGE(foe_entry) &&  (FOE_Hit_IFC(skb)) 
        && ((FOE_MAGIC_GE == magic) || (FOE_MAGIC_WLAN_7615 == magic) || (FOE_MAGIC_WLAN == magic)))
    {
        ret = IFC_API_GET_HIT_IDX_ACTION(FOE_IFC_IDX(skb), &action);
        if ((0 != ret)|| (0 == action))
        {
            return 0;
        }
        foe_ext[foe_num].ifc_hit_info.ifc_hit      = FOE_Hit_IFC(skb);
        foe_ext[foe_num].ifc_hit_info.ifc_id       = FOE_IFC_IDX(skb);
        foe_ext[foe_num].ifc_hit_info.match_action = action;
        if(pinfo)
        {
            foe_ext[foe_num].ifc_hit_info.chn  = pinfo->channel;
            foe_ext[foe_num].ifc_hit_info.stag = pinfo->stag;
            foe_ext[foe_num].ifc_hit_info.txq  = pinfo->txq;
        }
        switch(magic)
    	{
    		case FOE_MAGIC_GPON:
    			foe_ext[foe_num].ifc_hit_info.dir = IFC_HIT_DIR_GPON;
    			break;
    		case FOE_MAGIC_EPON:
    			foe_ext[foe_num].ifc_hit_info.dir = IFC_HIT_DIR_EPON;
    			break;
    		case FOE_MAGIC_GE:
    			foe_ext[foe_num].ifc_hit_info.dir = IFC_HIT_DIR_GE;
    			break;
    		case FOE_MAGIC_WLAN:	
    		case FOE_MAGIC_WLAN_7615:
    			foe_ext[foe_num].ifc_hit_info.dir = IFC_HIT_DIR_WLAN;
    			break;
    		default:
    			break;
    	}
        
        return 1;
    }
    else if(IS_L2_RRIDGE(foe_entry))
    {
        memset(&foe_ext[foe_num].ifc_hit_info, 0, sizeof(ifc_hit_info_t));
    }
    
	return 0;
}


static int PpeWlan7615OffloadToCpu(struct sk_buff* skb,struct FoeEntry * foe_entry)
{
	if(ppeIsIpv6Nat(FOE_ENTRY_NUM(skb),foe_entry))
		return 1;
    
    if(ppeIsIfcHit(FOE_ENTRY_NUM(skb),foe_entry))
        return 1;
    
	if(is_multicast_ether_addr(&(skb->data[0])))
		return 1;
	
	return 0;
}

int32_t PpeSetForcePortInfoOffload(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,u_int16_t meterGrpId,u_int16_t acntGrpIndex)
{
	unsigned char wifi_tsid = HWNAT_DEFAULT_MTRGRP;
	unsigned char wifi_acnt1_id = HWNAT_DEFAULT_ACNT1GRP;
	unsigned char wifi_acnt0_id = HWNAT_DEFAULT_ACNT0GRP; 
	struct iphdr *iph = NULL;
	int wlan7615OffloadToCpu = PpeWlan7615OffloadToCpu(skb,foe_entry);
    int npu_offload = 0;

	if(isEN7528 && ppeIsIpv6Nat(FOE_ENTRY_NUM(skb),foe_entry) &&
		!foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan)
	{
		if(meterGrpId == HWNAT_DEFAULT_GRP_ID)
		{
			if(FOE_MAGIC_GPON == magic || FOE_MAGIC_EPON == magic)
				meterGrpId = LEFT_TO_RIGHT_NPTV6_UP_METER;
			else if(FOE_MAGIC_GE == magic)
				meterGrpId = LEFT_TO_RIGHT_NPTV6_DOWN_METER;
		}
	}  

    if(ppeCheckNpuOffload(magic))
        npu_offload = 1;

    if(xpon_is_multicast_entry(foe_entry))      //7615 multicast go the same path as general wlan chip.
        magic = FOE_MAGIC_WLAN;
    	//need check , accord to source to decide what port need send
 	if (IS_IPV4_GRP(foe_entry)|| IS_L2_RRIDGE(foe_entry))
    	{
		if((SUPPORT_METER_ACNT_IN_QDMA) && ((FOE_MAGIC_WLAN_7615 == magic) || (FOE_MAGIC_WLAN == magic)|| (FOE_MAGIC_WLAN_TX == magic)))
		{
			if((wan_conn_type == 1 || wan_conn_type == 2) && strncmp(skb->dev->name, wan_itf_name, strlen(skb->dev->name)) == 0)
			{
			    FE_API_GET_METER_IDX(skb, UP_STREAM, &wifi_tsid, 0);
			    FE_API_GET_ACNT0_IDX(skb, UP_STREAM, &wifi_acnt0_id);
			}else{
			    FE_API_GET_METER_IDX(skb, DOWN_STREAM, &wifi_tsid, 0);
			    FE_API_GET_ACNT0_IDX(skb, DOWN_STREAM, &wifi_acnt0_id); 
			}
			FE_API_GET_ACNT1_IDX(skb, DOWN_STREAM, &wifi_acnt1_id);
			
			pinfo->tsid = wifi_tsid;
		}
		
		if( magic == FOE_MAGIC_WLAN_7615 && !wlan7615OffloadToCpu )
			PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_WLAN_7615);
        else if(npu_offload)
            PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_NPU_OFFLOAD);
		else if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL) 
			&& FOE_MAGIC_WLAN == magic){
			PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_WLAN);
		}
		else
			PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_OFFLOAD);
		
		if(TCSUPPORT_WAN_ETHER_VAL&&etherwan_use_qdma&&use_etherwan)
		{
			if((skb->dev) && (strlen(skb->dev->name) >= 2) && ((isWiFiInterface(skb->dev)) 
				|| (strncmp(skb->dev->name, "wds", 3) == 0) || (strncmp(skb->dev->name, "apcli", 5) == 0)))
				PpeSetWifiForceQos(foe_entry);
		}
		ds_need_offload = 1;
		if((SUPPORT_WDMA) && (magic == FOE_MAGIC_WLAN_7615) && !wlan7615OffloadToCpu)
			PpeSetInfoBlk2(foe_entry, FP_WDMA, meterGrpId, acntGrpIndex, 1);
        else if(npu_offload)
            PpeSetInfoBlk2(foe_entry, FP_NPU, meterGrpId, acntGrpIndex,0);
		else if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL)
			&& FOE_MAGIC_WLAN == magic) {
			PpeSetInfoBlk2(foe_entry, FP_TDMA, meterGrpId, acntGrpIndex, 1);
		}
		else
			PpeSetInfoBlk2(foe_entry, FP_PDMA, meterGrpId, acntGrpIndex, 1);
	}
	else if (IS_IPV6_GRP(foe_entry))
	{
		if(( magic == FOE_MAGIC_WLAN_7615 )&&!wlan7615OffloadToCpu)
			PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_WLAN_7615);
		else if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL) 
			&& FOE_MAGIC_WLAN == magic){
			PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_WLAN);
		}
		else
			PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_OFFLOAD);
		
		if(TCSUPPORT_WAN_ETHER_VAL&&etherwan_use_qdma&&use_etherwan)
		{
			if((skb->dev) && (strlen(skb->dev->name) >= 2) && ((isWiFiInterface(skb->dev)) 
				|| (strncmp(skb->dev->name, "wds", 3) == 0) || (strncmp(skb->dev->name, "apcli", 5) == 0)))
				PpeSetWifiForceQos(foe_entry);
		}
		ds_need_offload = 1;
		if((SUPPORT_WDMA) && (magic == FOE_MAGIC_WLAN_7615) && !wlan7615OffloadToCpu)
			PpeSetInfoBlk2(foe_entry, FP_WDMA, meterGrpId, acntGrpIndex, 1);
		else if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && (TCSUPPORT_WLAN_INODE_VAL || TCSUPPORT_NPU_WIFI_TX_VAL)
			&& FOE_MAGIC_WLAN == magic) {
			PpeSetInfoBlk2(foe_entry, FP_TDMA, meterGrpId, acntGrpIndex, 1);
		}
		else{
			PpeSetInfoBlk2(foe_entry, FP_PDMA, meterGrpId, acntGrpIndex, 1);
			if(enable_nat66_flag && SUPPORT_NAT66_CPU_OFFLOAD)
				PpeSetFqos(foe_entry,0);	
		}
	}
	else
		return 1;
	
	/*wifi send to special rx ring*/
	if(magic == FOE_MAGIC_WLAN)
	{
		if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && TCSUPPORT_WLAN_INODE_VAL)
		{
			if(( is5GWiFiInterface(skb->dev) ) && !ppe_is_multicast_entry(foe_entry)){
				PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
			}
			else if(( is24GWiFiInterface(skb->dev) ) && !ppe_is_multicast_entry(foe_entry)){
				PpeSetRxForceRingIdx(foe_entry, RING_IDX_0);
			}
		}
		else if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL && TCSUPPORT_NPU_WIFI_TX_VAL)
		{
			if((is24GWiFiInterface(skb->dev) || is24GAPCLIInterface(skb->dev))
				&& !ppe_is_multicast_entry(foe_entry)){
				PpeSetRxForceRingIdx(foe_entry, RING_IDX_0);
			}
			else if(( is5GWiFiInterface(skb->dev) || is5GAPCLIInterface(skb->dev) ||
				 is6GWiFiInterface(skb->dev) || is6GAPCLIInterface(skb->dev)) && 
				!ppe_is_multicast_entry(foe_entry)){
				PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
			}
		}
		else if( ( is5GWiFiInterface(skb->dev) ) && !ppe_is_multicast_entry(foe_entry) )
		{/*5G send to ring-6*/
			if(TCSUPPORT_NPU_WIFI_OFFLOAD_VAL)
				PpeSetRxForceRingIdx(foe_entry, RING_IDX_5);
			else{
				if(dualband_interrupt_enable)
				{
					if (dualband_interrupt_enable_V2 == 0)
					{
						PpeSetRxForceRingIdx(foe_entry, FOE_ENTRY_NUM(skb) & 4 ? RING_IDX_6 : RING_IDX_5);
					}
					else
					{
						//clear_rxring_cnt = 0;
						if (dualband_index_tmp==0)
						{
							PpeSetRxForceRingIdx(foe_entry,RING_IDX_6);
							dualband_index_tmp = 1;
						}
						else
						{
							PpeSetRxForceRingIdx(foe_entry,RING_IDX_5);
							dualband_index_tmp = 0;
						}
					}
				}
				else
					PpeSetRxForceRingIdx(foe_entry, RING_IDX_6);
                        }			
		}
		else if( ( is24GWiFiInterface(skb->dev) ) && !ppe_is_multicast_entry(foe_entry) ) /*2.4G send to ring-5*/
			PpeSetRxForceRingIdx(foe_entry,RING_IDX_5);
                if(udp_bypass_enable == 2){
                    iph = (struct iphdr *)skb_network_header(skb);
                    if (iph->protocol == IPPROTO_UDP) {
                        PpeSetRxForceRingIdx(foe_entry, RING_IDX_4);
                    }
                }
	}
	if(magic == FOE_MAGIC_WLAN_TX)
	{
		if( ( is5GWiFiInterface(skb->dev) ) && !ppe_is_multicast_entry(foe_entry) )
		{/*5G send to ring-6*/
			if(dualband_interrupt_enable)
			{
				if (dualband_interrupt_enable_V2 == 0)
				{
					PpeSetRxForceRingIdx(foe_entry, FOE_ENTRY_NUM(skb) & 4 ? RING_IDX_6 : RING_IDX_5);
				}
				else
				{
					//clear_rxring_cnt = 0;
					if (dualband_index_tmp==0)
					{
						PpeSetRxForceRingIdx(foe_entry,RING_IDX_6);
						dualband_index_tmp = 1;
					}
					else
					{
						PpeSetRxForceRingIdx(foe_entry,RING_IDX_5);
						dualband_index_tmp = 0;
					}
				}
			}
			else
				PpeSetRxForceRingIdx(foe_entry, RING_IDX_6);
		}
		else if( ( is24GWiFiInterface(skb->dev) ) && !ppe_is_multicast_entry(foe_entry) ) /*2.4G send to ring-5*/
			PpeSetRxForceRingIdx(foe_entry,RING_IDX_5);
        if(udp_bypass_enable == 2){
            iph = (struct iphdr *)skb_network_header(skb);
            if (iph->protocol == IPPROTO_UDP) {
                PpeSetRxForceRingIdx(foe_entry, RING_IDX_4);
            }
        }
	}
    /* vxlan offload to special rx ring*/
    if (isEN751627 && ((FOE_MAGIC_VXLAN_UP_1 == magic) || (FOE_MAGIC_VXLAN_DOWN_1 == magic)))
    {
        PpeSetRxForceRingIdx(foe_entry, RING_IDX_5);
    }

    if(ppeIsIpv6Nat(FOE_ENTRY_NUM(skb),foe_entry) &&
        !foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan &&
        (FOE_MAGIC_GE == magic || FOE_MAGIC_GPON == magic || FOE_MAGIC_EPON == magic || FOE_MAGIC_AE_WAN == magic || FOE_MAGIC_USBNET == magic))
	    PpeSetRxForceRingIdx(foe_entry,RING_IDX_6); 


	if(enable_nat66_flag && SUPPORT_NAT66_CPU_OFFLOAD && ppeIsIpv6Nat(FOE_ENTRY_NUM(skb),foe_entry) &&
        !foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan &&
        (FOE_MAGIC_GE == magic || FOE_MAGIC_GPON == magic || FOE_MAGIC_EPON == magic || FOE_MAGIC_AE_WAN == magic))
    {
		if(FOE_MAGIC_GE == magic){
			PpeSetInfoBlk2(foe_entry, FP_PDMA, meterGrpId, acntGrpIndex, 1);
			PpeSetRxForceRingIdx(foe_entry,RING_IDX_1); 
			PpeSetFqos(foe_entry,0);	
		}
		else{
			PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, meterGrpId, acntGrpIndex, 1);
			PpeSetRxForceRingIdx(foe_entry,RING_IDX_1); 
		}	
	}

    if(ppeIsIfcHit(FOE_ENTRY_NUM(skb),foe_entry))
    {
        PpeSetRxForceRingIdx(foe_entry, RING_IDX_5); 
    }
    
	if(SUPPORT_WDMA && ppeIsIpv6Nat(FOE_ENTRY_NUM(skb),foe_entry))
	{
		int dir = isUpStreamFlow(magic, pinfo);
		if(natv6_wan_rate[dir].enable)
			foe_entry->ipv6_5t_route.fqos = 0;
	}

    if(is_natv6_offload_short_cut() && ppeIsIpv6Nat(FOE_ENTRY_NUM(skb),foe_entry) &&
        !foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan &&
        (FOE_MAGIC_GE == magic || FOE_MAGIC_GPON == magic || FOE_MAGIC_EPON == magic || FOE_MAGIC_AE_WAN == magic || FOE_MAGIC_USBNET == magic)
		&& (0 == is_Ipv6_nat66_offload(foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_attr)))
    {
        foe_ext[FOE_ENTRY_NUM(skb)].forward_left_to_right_type = TYPE_NATV6;
		foe_ext[FOE_ENTRY_NUM(skb)].natv6.pppoe = !!PpeParseResult.pppoe_gap; 
#ifdef TWO_WAY_LEFT_TO_RIGHT
        if(isEN7580)
        {
            if(FOE_MAGIC_GE == magic)
				PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING); 
            else{
                PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, meterGrpId, acntGrpIndex, 1);
                PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING_2); 
            }   
        }
        else
        {
            if(FOE_MAGIC_GE == magic || two_way_left_to_right_flag == 0)
    	        PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING); 
            else{
				PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, meterGrpId, acntGrpIndex, 1);
                PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING_2); 
            }
        }
#else
        PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING); 
#endif
    }

    if(is_vxlan_offload_short_cut() && 
        (FOE_MAGIC_VXLAN_DOWN_1 == magic || FOE_MAGIC_VXLAN_UP_1 == magic))
    {
        memset(&foe_ext[FOE_ENTRY_NUM(skb)].vxlan,0,sizeof(struct VxlanFlow_kernel));
        foe_ext[FOE_ENTRY_NUM(skb)].forward_left_to_right_type = TYPE_VXLAN;
#ifdef TWO_WAY_LEFT_TO_RIGHT
        if(FOE_MAGIC_VXLAN_DOWN_1 == magic || two_way_left_to_right_flag == 0)
            PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING); 
        else{
			PpeSetInfoBlk2(foe_entry, FP_QDMA_SW, meterGrpId, acntGrpIndex, 1);
            PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING_2); 
        }
#else
        PpeSetRxForceRingIdx(foe_entry,OFFLOAD_RX_RING); 
#endif
    }
	if(FOE_MAGIC_TR471_NPU_DOWNSTREAM == magic){
		PpeSetInfoBlk2(foe_entry, FP_TDMA, meterGrpId, acntGrpIndex, 1);
		//PpeSetInfoBlk2(foe_entry, FP_GDMA1, meterGrpId, acntGrpIndex, 1);
		/*should be the same with TDMA_TR471_RX_RING in tdma_v1.h*/
		PpeSetRxForceRingIdx(foe_entry,RING_IDX_2); 

	}
	
	return 0;
}


/*set force port to ppe, and set udf and special tag*/
int32_t PpeSetForcePortInfoTunnel(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,u_int16_t meterGrpId,u_int16_t acntGrpIndex)
		  
{
	PpeSetPortInfo(foe_entry, pinfo, magic);

	PpeSetInfoBlk2(foe_entry, FP_PPE, 0x3f, 0x3f, 1);
	return 0;

}

int32_t PpeSetForcePortInfoV4(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,u_int16_t meterGrpId,u_int16_t acntGrpIndex)
{	
	if((magic == FOE_MAGIC_ATM) || (magic == FOE_MAGIC_PTM) || (magic == FOE_MAGIC_EPON) || (magic == FOE_MAGIC_GPON) || (magic == FOE_MAGIC_AE_WAN))
	{
		if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
			PpeSetPortInfo(foe_entry, pinfo, magic);
			if(FOE_MAGIC_TR471_NPU_UPSTREAM == FOE_MAGIC_TAG(skb) && SUPPORT_FAST_PATH){
				if (IS_IPV4_GRP(foe_entry))
					set_info2_fast(foe_entry, IPV4_HNAPT, 1);
			}

			PpeSetInfoBlk2(foe_entry, FP_QDMA_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
		}else{			
			HWNAT_BINDFAIL_DPRINT("upstream bind_dir %d is error\n",bind_dir);
			return 1;
		}	
	}
	else if(magic == FOE_MAGIC_GE)
	{
		if(pinfo->eth_is_wan)//check is etherwan
		{
			if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) {
				PpeSetPortInfo(foe_entry, pinfo, magic);
				if(USE_QDMA_WAN_ETHER == g_wan_mode)
					PpeSetInfoBlk2(foe_entry, FP_QDMA_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
				else{
					if(pinfo->ds_to_qdma){
						PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
					}else{
						PpeSetInfoBlk2(foe_entry, FP_GDMA1, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
					}
				}
			}else{				
				HWNAT_BINDFAIL_DPRINT("etherwan upstream  bind_dir %d is error\n",bind_dir);
				return 1;
			}
		}
		else
		{
			if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)) 
			{
				PpeSetPortInfo(foe_entry, pinfo, magic);

				if(pinfo->ds_to_qdma)
				{
					if (TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && dsoffload_presetting)
						softQdmaSetForcePort(foe_entry, pinfo, meterGrpId, acntGrpIndex);
					else
						PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
				}
				else
					PpeSetInfoBlk2(foe_entry, FP_GDMA1, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
			}
			else
			{
				HWNAT_BINDFAIL_DPRINT("downstream  bind_dir %d is error\n",bind_dir);
				return 1;
			}
		}
    	}
	else if(magic == FOE_MAGIC_PTM_LAN)
	{
		if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)) 
		{
			PpeSetPortInfo(foe_entry, pinfo, magic);
			PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
		}
		else
		{			
			HWNAT_BINDFAIL_DPRINT("PTM_LAN downstream  bind_dir %d is error\n",bind_dir);
			return 1;
		}
	}
	else if(magic == FOE_MAGIC_XSI)
	{
		PpeSetPortInfo(foe_entry, pinfo, magic);
		if(TCSUPPORT_WAN_XSI_VAL)
		    PpeSetInfoBlk2(foe_entry, FP_GDMA2, 0x3f, 0x3f, pinfo->ds_to_qdma);
		else
		    PpeSetInfoBlk2(foe_entry, FP_GDMA3, 0x3f, 0x3f, pinfo->ds_to_qdma);
	}
	else if(magic == FOE_MAGIC_OLT)
	{
		PpeSetPortInfo(foe_entry, pinfo, magic);
		PpeSetInfoBlk2(foe_entry, FP_GDMA3, 0x3f, 0x3f, pinfo->ds_to_qdma);
	}
	else if(magic == FOE_MAGIC_XSI_GDM4)
	{
		PpeSetPortInfo(foe_entry, pinfo, magic);
		PpeSetInfoBlk2(foe_entry, FP_GDMA4, 0x3f, 0x3f, pinfo->ds_to_qdma);
	}else
	{		
		HWNAT_BINDFAIL_DPRINT("magic %x is error\n",magic);
		return 1;
	}

	return 0;
}

int32_t PpeSetForcePortInfoV6(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,u_int16_t meterGrpId,u_int16_t acntGrpIndex)
{	
	if((magic == FOE_MAGIC_ATM) || (magic == FOE_MAGIC_PTM) || (magic == FOE_MAGIC_EPON) || (magic == FOE_MAGIC_GPON) || (magic == FOE_MAGIC_AE_WAN))
	{
		if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) 
		{
			PpeSetPortInfo(foe_entry, pinfo, magic);
			if(FOE_MAGIC_TR471_NPU_UPSTREAM == FOE_MAGIC_TAG(skb) && SUPPORT_FAST_PATH){
				if (IS_IPV6_GRP(foe_entry))
					set_info2_fast(foe_entry, IPV6_5T_ROUTE, 1);
			}
			PpeSetInfoBlk2(foe_entry, FP_QDMA_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
		}
		else
		{			
			HWNAT_BINDFAIL_DPRINT("v6 upstream bind_dir %d is error\n",bind_dir);
			return 1;
		}
	}
	else if(magic == FOE_MAGIC_GE)
	{
		if(pinfo->eth_is_wan)
		{//check is etherwan
			if ((bind_dir == UPSTREAM_ONLY) || (bind_dir == BIDIRECTION)) 
			{
				PpeSetPortInfo(foe_entry, pinfo, magic);
				if(USE_QDMA_WAN_ETHER == g_wan_mode)
					PpeSetInfoBlk2(foe_entry, FP_QDMA_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
				else{
					if(pinfo->ds_to_qdma){
						PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
					}else{
						PpeSetInfoBlk2(foe_entry, FP_GDMA1, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
					}
				}
					
			}
			else
			{				
				HWNAT_BINDFAIL_DPRINT("v6 etherwan upstream  bind_dir %d is error\n",bind_dir);
				return 1;
			}
		}
		else
		{
			if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION))
			{
				PpeSetPortInfo(foe_entry, pinfo, magic);

				if(pinfo->ds_to_qdma)
				{
					if (TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && dsoffload_presetting)
						softQdmaSetForcePort(foe_entry, pinfo, meterGrpId, acntGrpIndex);
					else
						PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
				}
				else
					PpeSetInfoBlk2(foe_entry, FP_GDMA1, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
			}
			else
			{
				HWNAT_BINDFAIL_DPRINT("v6 downstream  bind_dir %d is error\n",bind_dir);
				return 1;
			}
           	 }
	}
	else if(magic == FOE_MAGIC_PTM_LAN)
	{
			if ((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)) 
			{
				PpeSetPortInfo(foe_entry, pinfo, magic);
				PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, meterGrpId, acntGrpIndex, pinfo->ds_to_qdma);
			}
			else
			{				
				HWNAT_BINDFAIL_DPRINT("v6 PTM_LAN downstream  bind_dir %d is error\n",bind_dir);
				return 1; 
			}
	}
	else if(magic == FOE_MAGIC_XSI)
	{
		PpeSetPortInfo(foe_entry, pinfo, magic);
		if(TCSUPPORT_WAN_XSI_VAL)
		    PpeSetInfoBlk2(foe_entry, FP_GDMA2, 0x3f, 0x3f, pinfo->ds_to_qdma);
		else
		    PpeSetInfoBlk2(foe_entry, FP_GDMA3, 0x3f, 0x3f, pinfo->ds_to_qdma);
	}
	else if(magic == FOE_MAGIC_OLT)
	{
		PpeSetPortInfo(foe_entry, pinfo, magic);
		PpeSetInfoBlk2(foe_entry, FP_GDMA3, 0x3f, 0x3f, pinfo->ds_to_qdma);
	}
	else if(magic == FOE_MAGIC_XSI_GDM4)
	{
		PpeSetPortInfo(foe_entry, pinfo, magic);
		PpeSetInfoBlk2(foe_entry, FP_GDMA4, 0x3f, 0x3f, pinfo->ds_to_qdma);
	}
	else
	{		
		HWNAT_BINDFAIL_DPRINT("v6 magic %x is error\n",magic);
		return 1;
	}

	return 0;
}


static void ppeNptV6PrefixInit(void)
{
	memset(nptv6_prefix,0,sizeof(ppeNptV6Prefix_s_t)*PPE_NPTV6_PREFIX_MAX_NUM);
	return;
}

static void ppeNptv6SetPrefixToReg(int index,unsigned short* prefix)
{
	unsigned int value = 0;

	RegWrite(PPE_NPTV6_PREFIX_DATA,prefix[0]);	
	value = (index<<PREFIX_ID_SHIFT)|(3<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
	RegWrite(PPE_NPTV6_PREFIX_CTRL,value);

	value = (prefix[1]<<16)|prefix[2];
	RegWrite(PPE_NPTV6_PREFIX_DATA,value);	
	value = (index<<PREFIX_ID_SHIFT)|(2<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
	RegWrite(PPE_NPTV6_PREFIX_CTRL,value);

	value = (prefix[3]<<16)|prefix[4];
	RegWrite(PPE_NPTV6_PREFIX_DATA,value);	
	value = (index<<PREFIX_ID_SHIFT)|(1<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
	RegWrite(PPE_NPTV6_PREFIX_CTRL,value);

	value = (prefix[5]<<16)|prefix[6];
	RegWrite(PPE_NPTV6_PREFIX_DATA,value);	
	value = (index<<PREFIX_ID_SHIFT)|(0<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
	RegWrite(PPE_NPTV6_PREFIX_CTRL,value);

        if(PPE2_SUPPORTED&&ppe2IsEnabled())
        {
            RegWrite(PPE2_NPTV6_PREFIX_DATA,prefix[0]);	
            value = (index<<PREFIX_ID_SHIFT)|(3<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
            RegWrite(PPE2_NPTV6_PREFIX_CTRL,value);

            value = (prefix[1]<<16)|prefix[2];
            RegWrite(PPE2_NPTV6_PREFIX_DATA,value);	
            value = (index<<PREFIX_ID_SHIFT)|(2<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
            RegWrite(PPE2_NPTV6_PREFIX_CTRL,value);

            value = (prefix[3]<<16)|prefix[4];
            RegWrite(PPE2_NPTV6_PREFIX_DATA,value);	
            value = (index<<PREFIX_ID_SHIFT)|(1<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
            RegWrite(PPE2_NPTV6_PREFIX_CTRL,value);

            value = (prefix[5]<<16)|prefix[6];
            RegWrite(PPE2_NPTV6_PREFIX_DATA,value);	
            value = (index<<PREFIX_ID_SHIFT)|(0<<PREFIX_DATA_SEL_SHIFT)|(1<<PREFIX_WR_REQ_SHIFT);
            RegWrite(PPE2_NPTV6_PREFIX_CTRL,value);
        }
	
	return;
}

int ppeNptV6AddPrefixHw(unsigned char* ifname,unsigned short* s_prefix,unsigned int s_prefix_len,unsigned short* d_prefix,unsigned int d_prefix_len)
{
	int i = 0,j = 0;
	int find = 0;

	if(!SUPPORT_NPTV6)
		return 0;

	for(i = 0;i < PPE_NPTV6_PREFIX_MAX_NUM;i+=2)
	{
		if(nptv6_prefix[i].used&&!strcmp(ifname,nptv6_prefix[i].ifname))
		{
			nptv6_prefix[i].s_prefix_len = s_prefix_len;
			for(j = 0;j < 8;j++)
				nptv6_prefix[i].s_prefix[j] = s_prefix[j];
			ppeNptv6SetPrefixToReg(i,s_prefix);

			nptv6_prefix[i+1].d_prefix_len = d_prefix_len;
			for(j = 0;j < 8;j++)
				nptv6_prefix[i+1].d_prefix[j] = d_prefix[j];	
			ppeNptv6SetPrefixToReg(i+1,d_prefix);

            if(s_prefix_len >= d_prefix_len)
            {
                nptv6_prefix[i].hw_prefix_len = s_prefix_len;
                nptv6_prefix[i+1].hw_prefix_len = s_prefix_len;
            }
            else
            {
                nptv6_prefix[i].hw_prefix_len = d_prefix_len;
                nptv6_prefix[i+1].hw_prefix_len = d_prefix_len;
            }

			find = 1;
			break;
		}
	}

	if(!find)
	{
		find = 0;
		for(i = 0;i < PPE_NPTV6_PREFIX_MAX_NUM;i+=2)
		{
			if(!nptv6_prefix[i].used)
			{
				nptv6_prefix[i].used = 1;
				strncpy(nptv6_prefix[i].ifname,ifname, sizeof(nptv6_prefix[i].ifname)-1);
				nptv6_prefix[i].snpt = 1;
				nptv6_prefix[i].s_prefix_len = s_prefix_len;
				for(j = 0;j < 8;j++)
					nptv6_prefix[i].s_prefix[j] = s_prefix[j];
				ppeNptv6SetPrefixToReg(i,s_prefix);
				
				nptv6_prefix[i+1].used = 1;				
				strncpy(nptv6_prefix[i+1].ifname,ifname, sizeof(nptv6_prefix[i+1].ifname)-1);				
				nptv6_prefix[i+1].dnpt = 1;
				nptv6_prefix[i+1].d_prefix_len = d_prefix_len;
				for(j = 0;j < 8;j++)
					nptv6_prefix[i+1].d_prefix[j] = d_prefix[j];					
				ppeNptv6SetPrefixToReg(i+1,d_prefix);

                if(s_prefix_len >= d_prefix_len)
                {
                    nptv6_prefix[i].hw_prefix_len = s_prefix_len;
                    nptv6_prefix[i+1].hw_prefix_len = s_prefix_len;
                }
                else
                {
                    nptv6_prefix[i].hw_prefix_len = d_prefix_len;
                    nptv6_prefix[i+1].hw_prefix_len = d_prefix_len;
                }

				find = 1;
				break;
			}
		}
	}

	if(!find)
	{
		printk("set nptv6 hw prefix fail,ifname = %s\n",ifname);
		return -1;
	}

	return 0;
}

int ppeNptV6DelPrefixHw(unsigned char* ifname)
{
	int i = 0;

	if(!SUPPORT_NPTV6)
		return 0;

	for(i = 0;i < PPE_NPTV6_PREFIX_MAX_NUM;i+=2)
	{
		if(nptv6_prefix[i].used&&!strcmp(ifname,nptv6_prefix[i].ifname))
		{
			memset(&nptv6_prefix[i],0,sizeof(ppeNptV6Prefix_s_t));
			ppeNptv6SetPrefixToReg(i,nptv6_prefix[i].s_prefix);
			
			memset(&nptv6_prefix[i+1],0,sizeof(ppeNptV6Prefix_s_t));
			ppeNptv6SetPrefixToReg(i+1,nptv6_prefix[i+1].d_prefix);

			break;
		}
	}

	return 0;
}

int ppeNptV6DelPrefixHwAll(void)
{
	int i = 0;

	if(!SUPPORT_NPTV6)
		return 0;

	for(i = 0;i < PPE_NPTV6_PREFIX_MAX_NUM;i+=2)
	{
		if(nptv6_prefix[i].used)
		{
			memset(&nptv6_prefix[i],0,sizeof(ppeNptV6Prefix_s_t));
			ppeNptv6SetPrefixToReg(i,nptv6_prefix[i].s_prefix);
			
			memset(&nptv6_prefix[i+1],0,sizeof(ppeNptV6Prefix_s_t));
			ppeNptv6SetPrefixToReg(i+1,nptv6_prefix[i+1].d_prefix);
		}
	}

	return 0;
}



#define CHECK_V6_REV_SADDR(n) (foe_entry_tmp->ipv6_3t_route.ipv6_sip##n != ntohl(PpeParseResult.ip6h.daddr.s6_addr32[n]))
#define CHECK_V6_REV_DADDR(n) (foe_entry_tmp->ipv6_3t_route.ipv6_dip##n != ntohl(PpeParseResult.ip6h.saddr.s6_addr32[n]))
static int ppeNatV6SetInfoHw(struct FoeEntry * foe_entry,unsigned int attr,int magic)
{
	int i = 0;
	struct FoeEntry * foe_entry_tmp = NULL;
	
	if(!SUPPORT_NPTV6)
		return 0;

	if(0 == ppeGetNptV6Mode())/*reverse mode*/
	{
		for(i = 0;i < FOE_4TB_SIZ_TOTAL;i++)
		{
			foe_entry_tmp = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + i*SIZE_OF_FOE_ENTRY);

			if(foe_entry_tmp->bfib1.state == INVALID)
				continue;

			if(!IS_IPV6_3T_ROUTE(foe_entry_tmp)&&!IS_IPV6_5T_ROUTE(foe_entry_tmp))
				continue;

			if(CHECK_V6_REV_SADDR(0)||CHECK_V6_REV_SADDR(1)||CHECK_V6_REV_SADDR(2)||CHECK_V6_REV_SADDR(3))
				continue;

			if(CHECK_V6_REV_DADDR(0)||CHECK_V6_REV_DADDR(1)||CHECK_V6_REV_DADDR(2)||CHECK_V6_REV_DADDR(3))
				continue;
			
			if(IS_IPV6_5T_ROUTE(foe_entry_tmp))
			{
				if(foe_entry_tmp->ipv6_5t_route.bfib1.udp)
				{					
					if((foe_entry_tmp->ipv6_5t_route.sport != ntohs(PpeParseResult.uh.dest))||(foe_entry_tmp->ipv6_5t_route.dport != ntohs(PpeParseResult.uh.source)))
						continue;
				}
				else
				{					
					if((foe_entry_tmp->ipv6_5t_route.sport != ntohs(PpeParseResult.th.dest))||(foe_entry_tmp->ipv6_5t_route.dport != ntohs(PpeParseResult.th.source)))
						continue;
				}
			}

			set_ppe_nptv6_info_mode0(foe_entry,(attr&(1<<NATV6_ATTR_SIP))?1:0,(attr&(1<<NATV6_ATTR_DIP))?1:0,i);

			break;
		}


		if(FOE_4TB_SIZ_TOTAL == i){
			return 0;
                }else{
                        return 1;
                }
	}
	else/*nptv6 prefix mode*/
	{
		if(attr&(1<<NATV6_ATTR_SIP))
		{
			for(i = 0;i < PPE_NPTV6_PREFIX_MAX_NUM;i++)
			{
				if(nptv6_prefix[i].snpt)
				{
					set_ppe_nptv6_info_mode1(foe_entry,1,0,i,nptv6_prefix[i].hw_prefix_len);
					return 1;
				}
			}
		}
		else if(attr&(1<<NATV6_ATTR_DIP))
		{
			for(i = 0;i < PPE_NPTV6_PREFIX_MAX_NUM;i++)
			{
				if(nptv6_prefix[i].dnpt)
				{
					set_ppe_nptv6_info_mode1(foe_entry,0,1,i,nptv6_prefix[i].hw_prefix_len);
					return 1;
				}
			}
		}
		else
			return 0;
	}
	
	return 0;
}

static void ppeNatV6SetInfo(unsigned int foe_num,struct FoeEntry * foe_entry,unsigned int attr,int magic)
{
	if(ppeNatV6SetInfoHw(foe_entry,attr,magic) && !enable_nat66_flag)
		return;
	
	switch(magic)
	{
		case FOE_MAGIC_GPON:
			foe_ext[foe_num].natv6.natv6_dir = NATV6_DIR_GPON;
			break;
		case FOE_MAGIC_EPON:
			foe_ext[foe_num].natv6.natv6_dir = NATV6_DIR_EPON;
			break;
		case FOE_MAGIC_GE:
			foe_ext[foe_num].natv6.natv6_dir = NATV6_DIR_GE;
			break;
		case FOE_MAGIC_WLAN:	
		case FOE_MAGIC_WLAN_TX:	
		case FOE_MAGIC_WLAN_7615:
			foe_ext[foe_num].natv6.natv6_dir = NATV6_DIR_WLAN;
			break;
		case FOE_MAGIC_AE_WAN:
			foe_ext[foe_num].natv6.natv6_dir = NATV6_DIR_AE_WAN;
			break;
		case FOE_MAGIC_USBNET:
			foe_ext[foe_num].natv6.natv6_dir = NATV6_DIR_USB;
			break;
		case FOE_MAGIC_XSI_GDM4:
			foe_ext[foe_num].natv6.natv6_dir = NATV6_DIR_XSI_GDM4;
			break;
		default:
			break;
	}
	
	foe_ext[foe_num].natv6.natv6_attr = attr;

	return;
}

static int ppeIsIpv6Nat(unsigned int foe_num,struct FoeEntry * foe_entry)
{	
	if((IPV6_3T_ROUTE == foe_entry->bfib1.pkt_type)&&foe_ext[foe_num].natv6.natv6_attr)
		return 1;
	else if((IPV6_5T_ROUTE == foe_entry->bfib1.pkt_type)&&foe_ext[foe_num].natv6.natv6_attr)
		return 1;

	return 0;
}

#define CHECK_V6_SADDR(n) (foe_entry->ipv6_3t_route.ipv6_sip##n != ntohl(PpeParseResult.ip6h.saddr.s6_addr32[n]))
#define CHECK_V6_DADDR(n) (foe_entry->ipv6_3t_route.ipv6_dip##n != ntohl(PpeParseResult.ip6h.daddr.s6_addr32[n]))
int ppeCheckIpv6Nat(struct sk_buff * skb,struct FoeEntry * foe_entry,int magic,struct port_info * pinfo)
{
	unsigned int attr = 0;
	ppeNatV6Info_t info;
	unsigned int foe_num = FOE_ENTRY_NUM(skb);

	if((FOE_MAGIC_ATM == magic) ||(FOE_MAGIC_PTM == magic))/*not support atm&ptm yet*/
		return 0;

	if((IPV6_3T_ROUTE != PpeParseResult.pkt_type)&&(IPV6_5T_ROUTE != PpeParseResult.pkt_type))
		return 0;

	if((IPV6_3T_ROUTE != foe_entry->bfib1.pkt_type)&&(IPV6_5T_ROUTE != foe_entry->bfib1.pkt_type))
		return 0;

	if(ppe_is_multicast_entry(foe_entry))
		return 0;
	
	memset(&info,0,sizeof(ppeNatV6Info_t));
	if(CHECK_V6_SADDR(0)||CHECK_V6_SADDR(1)||CHECK_V6_SADDR(2)||CHECK_V6_SADDR(3))
	{
		attr |= 1<<NATV6_ATTR_SIP;
		memmove(info.ipv6_sip,PpeParseResult.ip6h.saddr.s6_addr16,8*sizeof(unsigned short));
	}

	if(CHECK_V6_DADDR(0)||CHECK_V6_DADDR(1)||CHECK_V6_DADDR(2)||CHECK_V6_DADDR(3))
	{
		attr |= 1<<NATV6_ATTR_DIP;
		memmove(info.ipv6_dip,PpeParseResult.ip6h.daddr.s6_addr16,8*sizeof(unsigned short));
	}

	#if 1
	if((IPV6_5T_ROUTE == PpeParseResult.pkt_type)&&(IPV6_5T_ROUTE == foe_entry->bfib1.pkt_type))
	{
		if(foe_entry->ipv6_5t_route.bfib1.udp)
		{
			if(foe_entry->ipv6_5t_route.sport != htons(PpeParseResult.uh.source))
			{
				attr |= 1<<NATV6_ATTR_UDP_SPORT;
				info.sport = PpeParseResult.uh.source;
			}

			if(foe_entry->ipv6_5t_route.dport != htons(PpeParseResult.uh.dest))
			{
				attr |= 1<<NATV6_ATTR_UDP_DPORT;
				info.dport = PpeParseResult.uh.dest;
			}
		}
		else
		{
			if(foe_entry->ipv6_5t_route.sport != htons(PpeParseResult.th.source))
			{
				attr |= 1<<NATV6_ATTR_TCP_SPORT;
				info.sport = PpeParseResult.th.source;
			}

			if(foe_entry->ipv6_5t_route.dport != htons(PpeParseResult.th.dest))
			{
				attr |= 1<<NATV6_ATTR_TCP_DPORT;
				info.dport = PpeParseResult.th.dest;
			}
		}

	}
	#endif

	if(!attr)
	{
		memset(&foe_ext[foe_num].natv6,0,sizeof(struct natv6_s));
		return 0;
	}

	ppeNatV6SetInfo(foe_num,foe_entry,attr,magic);
	memmove(&foe_ext[foe_num].natv6.natv6_info,&info,sizeof(ppeNatV6Info_t));
	if(pinfo)
	{
		foe_ext[foe_num].natv6.chn = pinfo->channel;
		foe_ext[foe_num].natv6.stag = pinfo->stag;
		if(NATv6_USE_HIGH_PRIORITY){
			foe_ext[foe_num].natv6.txq = pinfo->txq?pinfo->txq:5;
		}else{
			foe_ext[foe_num].natv6.txq = pinfo->txq;
		}
	}

	if(pinfo && (left_to_right_test_mode == TEST_MODE_NATV6) && update_nptv6_info_hook)
	{
		if(FOE_MAGIC_GE == magic)
			update_nptv6_info_hook(pinfo->channel, pinfo->stag, (unsigned int *)&info.ipv6_dip[0], 1);
		else
			update_nptv6_info_hook(pinfo->channel, pinfo->stag, (unsigned int *)&info.ipv6_sip[0], 0);
	}

	return 1;
}


static unsigned char is_Ipv6_nat66_offload(unsigned int attr)
{
    /*
        prot Translate
    */
	if( (attr & (1<<NATV6_ATTR_TCP_DPORT)) || (attr & (1<<NATV6_ATTR_TCP_SPORT))
			|| (attr & (1 << NATV6_ATTR_UDP_DPORT)) || (attr & ( 1 << NATV6_ATTR_UDP_SPORT))  )
	{
		return 1;
	}
    return 0;        
}

int32_t PpeSetForcePortInfo(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic)
{
   	 u_int16_t meterGrpId = HWNAT_DEFAULT_GRP_ID, acntGrpId = HWNAT_DEFAULT_GRP_ID;

    	if(!skb->dev && (magic != FOE_MAGIC_CRYPTO_E_1))
        	{
		HWNAT_BINDFAIL_DPRINT("skb->dev is NULL\n");
        		return 1;
    	}

	ecnt_ppe_get_meter_and_account_id_hook(foe_entry,skb,pinfo,magic,&meterGrpId,&acntGrpId);

	if(magic != FOE_MAGIC_CRYPTO_D_1 && magic != FOE_MAGIC_CRYPTO_E_1)
	{
		if((skb->dev) && (strlen(skb->dev->name) < 3))//do nothing
			return 0;
	}

	if(0 == ecnt_ppe_set_force_port_info_start_hook(skb,foe_entry,pinfo,magic,meterGrpId,acntGrpId))
		return 0;

	if (((skb->dev) && (strlen(skb->dev->name) >= 2) && ((isWiFiInterface(skb->dev)) || (strncmp(skb->dev->name, "wds", 3) == 0) || (strncmp(skb->dev->name, "apcli", 5) == 0)))		
		|| (FOE_MAGIC_CRYPTO_D_1 == magic)			
		|| (FOE_MAGIC_CRYPTO_E_1 == magic)
		|| (FOE_MAGIC_LOCAL == magic)
		|| (FOE_MAGIC_LOCAL_MCAST == magic)
		|| (FOE_MAGIC_GRE_UP_1 == magic)          
		|| (FOE_MAGIC_GRE_DOWN_1 == magic)
		|| (FOE_MAGIC_VXLAN_UP_1 == magic)          
		|| (FOE_MAGIC_VXLAN_DOWN_1 == magic)
		|| (FOE_MAGIC_ASYM_DOWNSTREAM == magic)
		|| (FOE_MAGIC_ASYM_UPSTREAM == magic)
		|| (FOE_MAGIC_USBNET == magic)
		|| ppeIsIpv6Nat(FOE_ENTRY_NUM(skb),foe_entry)
		|| ppeIsIfcHit(FOE_ENTRY_NUM(skb),foe_entry)
		|| (FOE_MAGIC_TR471_NPU_DOWNSTREAM == magic)
		|| (FOE_MAGIC_OOKLA == magic))
	{
		if(PpeSetForcePortInfoOffload(skb,foe_entry,pinfo,magic,meterGrpId,acntGrpId))
		{
			HWNAT_BINDFAIL_DPRINT("set force port offload fail\n");
			return 1;
		}
	}
	else if((FOE_MAGIC_GRE_HWDOWN_1 == magic) || (FOE_MAGIC_VXLAN_HWDOWN_1 == magic) || (FOE_MAGIC_L2TP_HWDOWN_1 == magic)){
	
		if(PpeSetForcePortInfoTunnel(skb,foe_entry,pinfo,magic,meterGrpId,acntGrpId))
		{
			HWNAT_BINDFAIL_DPRINT("set force port offload fail\n");
			return 1;
		}

	}else 
	{
		if(IS_IPV4_GRP(foe_entry) || IS_L2_RRIDGE(foe_entry))
		{
			if(PpeSetForcePortInfoV4(skb,foe_entry,pinfo,magic,meterGrpId,acntGrpId))
			{
				HWNAT_BINDFAIL_DPRINT("set force port v4 fail\n");
				return 1;
			}
		} 
		else if(IS_IPV6_GRP(foe_entry))
		{
			if(PpeSetForcePortInfoV6(skb,foe_entry,pinfo,magic,meterGrpId,acntGrpId))
			{
				HWNAT_BINDFAIL_DPRINT("set force port v6 fail\n");
				return 1;
			}
		}
	}

	ecnt_ppe_set_force_port_info_end_hook(skb,foe_entry,pinfo,magic,meterGrpId,acntGrpId);


	PpeSetTunnel_Info(foe_entry);

	return 0;
}

int isValidPpeEntry(struct sk_buff *skb, struct FoeEntry *foe_entry)
{
	unsigned char dmac[ETH_ALEN];
	memset(dmac, 0, sizeof(dmac));

	if (xpon_is_multicast_entry(foe_entry)) {
		if (foe_entry->bfib1.pkt_type < 2) { 
			if ((foe_entry->ipv4_hnapt.dip != foe_entry->ipv4_hnapt.new_dip) 
				|| (foe_entry->ipv4_hnapt.sip != foe_entry->ipv4_hnapt.new_sip))
				return 0;
			else
				return 1;
		}
	}
	
	if (IS_IPV4_GRP(foe_entry)) {
		if ((foe_entry->ipv4_hnapt.sip == 0) && (foe_entry->ipv4_hnapt.dip == 0))
			return 0;
	} else if (IS_IPV6_GRP(foe_entry)) {
		if ((foe_entry->ipv6_5t_route.ipv6_sip0 == 0) && (foe_entry->ipv6_5t_route.ipv6_sip1 == 0)
			&& (foe_entry->ipv6_5t_route.ipv6_sip2 == 0) && (foe_entry->ipv6_5t_route.ipv6_sip3 == 0)
			&& (foe_entry->ipv6_5t_route.ipv6_dip0 == 0) && (foe_entry->ipv6_5t_route.ipv6_dip1 == 0)
			&& (foe_entry->ipv6_5t_route.ipv6_dip2 == 0) && (foe_entry->ipv6_5t_route.ipv6_dip3 == 0))
			return 0;
	} else if (IS_L2_RRIDGE(foe_entry)) {
		FoeGetEntryMac(dmac, foe_entry->l2_bridge.in_dmac_hi, foe_entry->l2_bridge.in_dmac_lo);
		if(!ether_addr_equal(dmac, PpeParseResult.dmac))
			return 0;
	} else {
		return 1;
	}

	return 1;
}

void PpeSetEntryBind(struct sk_buff *skb, struct FoeEntry *foe_entry)
{
	uint32_t current_time;
	struct bf_info_blk1 bfib1_temp;
	_info_blk2 *iblk2;
	int ttl = -1;

	if (IS_IPV4_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
	else if (IS_IPV6_GRP(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
	else if (IS_L2_RRIDGE(foe_entry))
		iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
	else
		return;

	/* word copy from foe_entry info1 */
	(*((uint32_t*)&bfib1_temp)) = (*((uint32_t*)&foe_entry->bfib1));

	/* Set Current time to time_stamp field in information block 1 */
	FE_API_GET_TIEM_STAMP(&(current_time));
	bfib1_temp.time_stamp = (uint16_t) current_time;

	/* Ipv4: TTL / Ipv6: Hot Limit filed */
	if (skb->bridge_flag == 1)
		bfib1_temp.ttl = 0;
	else if (iblk2->fpidx == FP_DROP)
		bfib1_temp.ttl = 0;
	else
		bfib1_temp.ttl = DFL_FOE_TTL_REGEN;
	
	ECNT_API_BBF247_SET_HWNAT_TTL(&ttl);  //set ttl=0, Note: can't &bfib1_temp.ttl
	if(ttl == 0 || ttl == 1){
		bfib1_temp.ttl = ttl;
	}
	
	/* Change Foe Entry State to Binding State */
	bfib1_temp.state = BIND;

	//some info will be update by hardware when state is unbind
	bfib1_temp.rmt = PpeParseResult.rmt;

	if(PpeParseResult.is_L2_tunnel){
		bfib1_temp.vlan_layer = PpeParseResult.vlan_layer_intunnel;
		if (PpeParseResult.pppoe_gap_intunnel) {
			bfib1_temp.psn = 1;
		}else{	
			bfib1_temp.psn = 0;
		}
		
	}else{ 
		bfib1_temp.vlan_layer = PpeParseResult.vlan_layer;
		if (PpeParseResult.pppoe_gap) {
			bfib1_temp.psn = 1;
		}else{	
			bfib1_temp.psn = 0;
		}
	}

	if(!PPE_CACHE_NOT_SUPPORT) 
		SetInfo2Cache(bfib1_temp, 1);

	if (ppe_is_multicast_entry(foe_entry)&& (!PPE_CACHE_NOT_SUPPORT))
		SetInfo2Cache(bfib1_temp, 0);

	ecnt_ppe_set_entry_bind_hook(skb,foe_entry,&bfib1_temp);

	/* word copy back to foe_entry info1 */
	(*((uint32_t*)&foe_entry->bfib1)) = (*((uint32_t*)&bfib1_temp));

	return;
}

int ppe_check_start_to_bind(struct sk_buff* skb,struct FoeEntry *foe_entry)
{
	if(ppeSupportPreBind)
	{
		if(IS_MAGIC_TAG_VALID(skb) && (FOE_AI(skb) == HIT_UNBIND_RATE_REACH))
			PPESETPREBIND(foe_entry); 
		else if(IS_MAGIC_TAG_VALID(skb) && (FOE_AI(skb) == HIT_PREBIND))
			return 1;
	}
	else if(IS_MAGIC_TAG_VALID(skb) && (FOE_AI(skb) == HIT_UNBIND_RATE_REACH))
		return 1;
	else if(IS_MAGIC_TAG_VALID(skb) && ppeCheckFastRuleBind(skb,foe_entry))
		return 1;
	
	return 0;
}

extern udpSessionTest_t udpSessionTest;
extern int udpSessionTestRestoreTime;
static void udp_session_test_packet_check(struct sk_buff * skb, struct FoeEntry *foe_entry)
{
	uint32_t sip = 0, dip = 0;
	uint16_t sport = 0, dport = 0;
	unsigned char dir = 0;

	if(!udpSessionTest.enable)
		return;
	
	if(udpSessionTest.start_flag)
		return;

	if(FOE_AI(skb) != HIT_UNBIND)
		return;

	if(!foe_entry->bfib1.udp)
		return;

	if(FOE_MAGIC_GE == FOE_MAGIC_TAG(skb))
		dir = 0;
	else if(FOE_MAGIC_GPON == FOE_MAGIC_TAG(skb) || FOE_MAGIC_EPON == FOE_MAGIC_TAG(skb) || FOE_MAGIC_AE_WAN == FOE_MAGIC_TAG(skb))
		dir = 1;
	else
		return;
	
	if(IS_IPV4_HNAPT(foe_entry))
	{
		sip = foe_entry->ipv4_hnapt.sip;
		dip = foe_entry->ipv4_hnapt.dip;
		sport = foe_entry->ipv4_hnapt.sport;
		dport = foe_entry->ipv4_hnapt.dport;
	}
	else if(IS_IPV6_5T_ROUTE(foe_entry)){
		sip = foe_entry->ipv6_5t_route.ipv6_sip0^foe_entry->ipv6_5t_route.ipv6_sip1\
			^foe_entry->ipv6_5t_route.ipv6_sip2^foe_entry->ipv6_5t_route.ipv6_sip3;
		dip = foe_entry->ipv6_5t_route.ipv6_dip0^foe_entry->ipv6_5t_route.ipv6_dip1\
			^foe_entry->ipv6_5t_route.ipv6_dip2^foe_entry->ipv6_5t_route.ipv6_dip3;
		sport = foe_entry->ipv6_5t_route.sport;
		dport = foe_entry->ipv6_5t_route.dport;
	}
	else
		return;

	if((udpSessionTest.info[dir].sip != sip) || (udpSessionTest.info[dir].dip != dip))
	{
		udpSessionTest.info[dir].sip = sip;
		udpSessionTest.info[dir].dip = dip;   
		udpSessionTest.count = 0;
	}
	else
	{
		if((udpSessionTest.info[dir].sport != sport) || (udpSessionTest.info[dir].dport != dport))
		{
			udpSessionTest.count++;
			udpSessionTest.info[dir].sport = sport;
			udpSessionTest.info[dir].dport = dport;
		}
	}

	if(udpSessionTest.count > 2000)
	{
		printk("udp session test start\n");
		udpSessionTest.start_flag = 1;
		RegModifyBits(PPE_IP_PROT_0, 0, 8, 8);
		udpSessionTest.restore_time = udpSessionTestRestoreTime;
	}
	
	return;
}

void set_entry(struct FoeEntry *foe_entry, struct sk_buff *skb)
{
    struct ethhdr *eth = NULL;
	unsigned char *pdata = NULL;
	unsigned int foe_idx = 0;

	if ((NULL == foe_entry) || (NULL == skb))
	{
		return;
	}

	foe_idx = FOE_ENTRY_NUM(skb);

	/* get original SMAC */
	pdata = skb->data;
	skb->data = skb_mac_header(skb);
	eth = (struct ethhdr *)skb->data;
	foe_ext[foe_idx].sp = FOE_SP(skb);
	memcpy(foe_ext[foe_idx].dmac, eth->h_dest, ETH_ALEN);
	memcpy(foe_ext[foe_idx].smac, eth->h_source, ETH_ALEN);
	skb->data = pdata;
}

void ppe_rx_fast_bind(struct FoeEntry *foe_entry, struct sk_buff *skb)
{
	struct vlan_hdr *vh = NULL;
    struct iphdr *iph = NULL;
    struct tcphdr *th = NULL;

	/* check protocol */
    if (htons(ETH_P_8021Q) == skb->protocol)
    {
        vh = (struct vlan_hdr *)skb->data;
        if (htons(ETH_P_IP) == vh->h_vlan_encapsulated_proto)
        {
            iph = (struct iphdr *)(skb->data + VLAN_HLEN);
        }
    }
    else if (htons(ETH_P_IP) == skb->protocol)
    {
        iph = (struct iphdr *)skb->data;
    }
	else
	{
		return;
	}

	/* process TCP */
	if ((NULL == iph) || (iph->protocol != IPPROTO_TCP))
	{
		return;
	}

    /* for EN7523: save original SMAC and SP */
    th = (struct tcphdr *)((uint8_t *)iph + (iph->ihl * 4));
	if ((3 == hwnat_fast_bind) && (th->syn) && (FOE_SP(skb) != SP_GDMA2))
	{
		set_entry(foe_entry, skb);
	}

	if (th->syn && (((ntohl(iph->saddr) != foe_entry->ipv4_hnapt.sip)) 
		|| ((ntohl(iph->daddr) != foe_entry->ipv4_hnapt.dip)) 
		|| ((ntohs(th->source) != foe_entry->ipv4_hnapt.sport)) 
		|| ((ntohs(th->dest) != foe_entry->ipv4_hnapt.dport))))
	{
		hwnat_fast_bind_hash_conflict++;
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
	}
}

static struct PktLenMonitor pkt_len_monitor;
static inline void pkt_len_monitor_for_2544_test(struct sk_buff * skb)
{
	int i = 0;
	__u32 layer1_rate = 0, skb_total_len = 0;
	struct LenRate *len_rate;
	GENERAL_TrtcmRatelimitCfg_T glbRateLimitCfg;
	GENERAL_TrtcmRatelimitSet_T glbRateLimitSet;

	if(DebugLevel >= 2)
		printk("received KA, FOE_MAGIC_TAG(skb) %x, pkt len %d\n",\
		FOE_MAGIC_TAG(skb), skb->len);

	if(FOE_MAGIC_TAG(skb) != FOE_MAGIC_GE)
		return;

	skb_total_len = skb->len + ETH_HLEN + 4;
		
	for(i=0; i<PKT_LEN_MONITOR_NUM; i++)
	{
		len_rate = &pkt_len_monitor.len_rate[i];
		if(len_rate->len != skb_total_len)
			continue;

		if(pkt_len_monitor.setted_rate == len_rate->rate)
			return;

		memset(&glbRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
		memset(&glbRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ; 

		glbRateLimitCfg.trtcmModule = GLB_RATECTL ;
		glbRateLimitCfg.MeterEn = len_rate->rate?GENERAL_METER_ENABLE:GENERAL_METER_DISABLE;
		QDMA_API_SET_GENERAL_RATELIMIT_CFG(ECNT_QDMA_WAN, &glbRateLimitCfg);  

		glbRateLimitSet.trtcmModule = GLB_RATECTL ;
		QDMA_API_GET_GENERAL_RATELIMIT_VALUE(ECNT_QDMA_WAN, &glbRateLimitSet);  
		layer1_rate = (len_rate->rate/skb_total_len)*(skb_total_len+20);
		glbRateLimitSet.RateLimitValue = layer1_rate;
		QDMA_API_SET_GENERAL_RATELIMIT_VALUE(ECNT_QDMA_WAN, &glbRateLimitSet);

		pkt_len_monitor.setted_rate = len_rate->rate;

		if(DebugLevel >= 2)
			printk("Set QDMA WAN glb tx ratelimit %dkbps\n", layer1_rate);

		return;
	}

	return;
}

static int pkt_len_monitor_2544_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int i = 0;
	struct LenRate *len_rate;

	printk("usage:	echo <enable> <pkt len> <L2_rate:kbps> > /proc/tc3162/2544_test_pkt_rate\n");
	printk("for example:	echo 1 1280 2400000 > /proc/tc3162/2544_test_pkt_rate\n\n");

	printk("enable %d\n", pkt_len_monitor.enable);
	printk("setted_rate %dkbps\n", pkt_len_monitor.setted_rate);
	for(i=0; i<PKT_LEN_MONITOR_NUM; i++)
	{
		len_rate = &pkt_len_monitor.len_rate[i];
		if(len_rate->len)
			printk("pktlen %d: ratelimit %dkb2ps\n", len_rate->len, len_rate->rate);
	}
	
	return 0;	
}

static int pkt_len_monitor_2544_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	int i = 0, value0 = 0, value1 = 0, value2 = 0;
	static int netx_id = 0;
	struct LenRate *len_rate;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d %d %d",&value0,&value1,&value2);

	pkt_len_monitor.enable = value0;

	/* set pkt len rate */
	for(i=0; i<PKT_LEN_MONITOR_NUM; i++)
	{
		len_rate = &pkt_len_monitor.len_rate[i];
		if(value1 == len_rate->len)
		{
			len_rate->rate = value2;
			return count;
		}
	}

	for(i=0; i<PKT_LEN_MONITOR_NUM; i++)
	{
		len_rate = &pkt_len_monitor.len_rate[i];
		if(0 == len_rate->len)
		{
			len_rate->len = value1;
			len_rate->rate = value2;
			return count;
		}
	}

	/* means full, replace the next */
	len_rate = &pkt_len_monitor.len_rate[netx_id];
	len_rate->len = value1;
	len_rate->rate = value2;
	netx_id++;
	netx_id = (netx_id%PKT_LEN_MONITOR_NUM);
	
	return count;
}

static void natv6_411_test_handle_for_epon(int foe_entry_idx, struct FoeEntry *foe_entry, int magic)
{
	int chnl_idx;
	QDMA_TxRateLimitSet_T txRateLimitSet;

	if( natv6_411_test_flag && natv6_411_epon_flag 
		&& (FOE_MAGIC_GE == magic) && ppeIsIpv6Nat(foe_entry_idx, foe_entry) ) 
	{
		natv6_411_timer = natv6_411_timeout_time;
		for(chnl_idx = 1; chnl_idx < 5; chnl_idx++) 
		{
			txRateLimitSet.chnlIdx = chnl_idx;
			txRateLimitSet.chnlRateLimitEn = 1;
			if(chnl_idx == foe_ext[foe_entry_idx].natv6.chn)
				txRateLimitSet.rateLimitValue = (1000 * 1000 );		/* unit: kbps */
			else
				txRateLimitSet.rateLimitValue = (natv6_411_rate_limit * 1000 );		/* unit: kbps */
			QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitSet);
		}

	}
		
	return;
}

__IMEM int32_t PpeRxHandler(struct sk_buff * skb)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int foe_entry_idx=0, ret=0;
#ifdef CONFIG_TP_IMAGE
	struct net_device *dev = NULL;
#endif /* CONFIG_TP_IMAGE */

    if(!HWNAT_IS_ENABLE()){
		return -1;
    }

	if (!skb)
		return -1;

	if (DebugLevel >= 3) 
	{
		printk("Rx Magic %x\n",FOE_MAGIC_TAG(skb));
		FoeDumpPkt(skb);
	}

#ifdef CONFIG_TP_IMAGE
	if (skb->dev)
	{
		/* backup net device, skb->dev will be overwrite in some code path */
		dev = skb->dev;
	}
#endif /* CONFIG_TP_IMAGE */

	if ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_PCI)  || (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN)) 
	{
		if (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN)
		{ 
			/* come from WIFI RX */
			if (TCSUPPORT_NEW_WDOG_VAL)
				wdog_kick_api(1);
        		}

		if(wlan_to_lan_hook != NULL)
			wlan_to_lan_hook(skb, 1);

		if(TCSUPPORT_ASYM_VOD_VAL)
			ECNT_ASYM_API_PKT_HANDLER(&skb,ECNT_ASYM_PKT_PHASE_SW_U);
		
		return PpeExtIfRxFastHandler(skb);
    	}
	else if  ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_E_2)|| (FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_D_2) 
		|| (FOE_MAGIC_TAG(skb) == FOE_MAGIC_USBNET)){
		if (TCSUPPORT_NEW_WDOG_VAL)
				wdog_kick_api(1);
        		
		ret = PpeExtIfRxHandler(skb);
		goto finish;
		
	}

	foe_entry_idx = FOE_ENTRY_NUM(skb);
	if((FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU) || 
        (FOE_AI(skb) == HIT_BIND_MUL_CPU) || 
        (FOE_AI(skb) == HIT_BIND_MUL_CPUR) )
		foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + foe_entry_idx*SIZE_OF_FOE_ENTRY);
	else
	{
		foe_entry = getPpeEntryLocal(foe_entry_idx, foe_entry);
		if(foe_entry == NULL)
			return -1;
		udp_session_test_packet_check(skb, foe_entry);
	}
	
    if ((((hwnat_fast_bind == 1) || (3 == hwnat_fast_bind)) && (FOE_AI(skb) == TCP_FIN_SYN_RST))
        || ((hwnat_fast_bind == 2) && (FOE_AI(skb) == UN_HIT)))
    {
		ppe_rx_fast_bind(foe_entry, skb);
    }

/* TCSUPPORT_DLF_CTRL begin */
	if( TCSUPPORT_DLF_CTRL_VAL ){
		PpeDLFRxHandler(skb) ;
		if(TCSUPPORT_HWNAT_L2VID_VAL && is_l2lu_pkt_exist)
			PpeL2luKeyUpdateBindTime(skb);
	}
/* TCSUPPORT_DLF_CTRL end */

	if((FOE_AI(skb) == HIT_BIND_MUL_CPU)){
		ret = PpeHitBindMULToCpuHanler(skb,foe_entry);
		goto finish;
	}
	else if ((FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU)||((FOE_AI(skb) == HIT_BIND_MUL_CPUR)&&TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)){
		/* offload for Mcast WAN->WIFI */
		if (TCSUPPORT_NEW_WDOG_VAL){
           wdog_kick_api(2);
        }
		ret = PpeHitBindForceToCpuHandler(skb, foe_entry);
		goto finish_no_need_set_entry;
	} else if (((FOE_SP(skb) == SP_PDMA)||(FOE_SP(skb) == SP_NPU)||(is_tr471_pkt(skb) &&  FOE_SP(skb) == SP_TDMA))&&(FOE_AI(skb) != 0) && (FOE_AI(skb) != NOT_THROUGH_PPE)){ 
		/* handle the incoming packet which came back from PPE */
		ret = PpeExtIfPingPongHandler(skb);
		goto finish_no_need_set_entry;
	} else if (FOE_AI(skb) == HIT_BIND_KEEPALIVE_UC_OLD_HDR) {
		if (DebugLevel >= 2) {
			printk("Got HIT_BIND_KEEPALIVE_UC_OLD_HDR packet (hash index=%d)\n", FOE_ENTRY_NUM(skb));
		}
		ret = 1;
		goto finish;
	} else if (FOE_AI(skb) == HIT_BIND_KEEPALIVE_MC_NEW_HDR) {
		if (DebugLevel >= 2) {
			printk("Got HIT_BIND_KEEPALIVE_MC_NEW_HDR packet (hash index=%d)\n", FOE_ENTRY_NUM(skb));
		}
		if (PpeKeepAliveHandler(skb, foe_entry)) {
			ret = 1;
			goto finish;
		}
	} else if (FOE_AI(skb) == HIT_BIND_KEEPALIVE_DUP_OLD_HDR) {
		if (DebugLevel >= 2) {
			printk("Got HIT_BIND_KEEPALIVE_DUP_OLD_HDR packe (hash index=%d)\n", FOE_ENTRY_NUM(skb));
		}
		if(TCSUPPORT_HWNAT_L2VID_VAL && is_l2lu_pkt_exist)
			PpeL2luKeyUpdateBindTime(skb);
		ret = 1;
		if(check_gemport && ((FOE_MAGIC_EPON == FOE_MAGIC_TAG(skb)) || (FOE_MAGIC_GPON == FOE_MAGIC_TAG(skb))))
            PpeGemportCheck(skb,foe_entry);
		if(IS_IPV4_GRP(foe_entry))
		{
			if((DP_ASYM_D1 <= foe_entry->ipv4_hnapt.act_dp)&&( foe_entry->ipv4_hnapt.act_dp <= DP_ASYM_D5))
			{
				ECNT_ASYM_API_PKT_HANDLER(&skb,ECNT_ASYM_PKT_PHASE_HW_D);
				ECNT_ASYM_API_KA_HANDLER(skb,ECNT_ASYM_KA_POINT_WLAN);
			}
		}
		if(pkt_len_monitor.enable)
			pkt_len_monitor_for_2544_test(skb);
		if((foe_ext[foe_entry_idx].natv6.natv6_dir == NATV6_DIR_GE)||(foe_ext[foe_entry_idx].natv6.natv6_dir == NATV6_DIR_XSI_GDM4))
		{
			if (DebugLevel >= 2) {
				printk(" Drop downstream keepalive packets for 2544 latency test.\n" );
			}
			/* for JOYME4 CloudVR 2544 test, 106-4-14 = 88 */
			if(TCSUPPORT_CT_JOYME4_VAL && (skb->len > 80) && (skb->len < 100))
			{
				natv6_2544_check_flag = 1;
			}
			dev_kfree_skb_any(skb);
			ret = 0;
		}		
		if(ret && (!ecnt_multicast_drop(skb)))
			return 0;               
		goto finish;
	}
    else if(aggressive_offload_short_cut_mode && 
		((FOE_SP(skb) == SP_QDMA)&&(FOE_AI(skb) != 0))){
		ret = PpeExtIfPingPongHandler(skb);
		goto finish_no_need_set_entry;
	}

	ecnt_ppe_rx_handler_hook(skb);
	if(!ecnt_multicast_drop(skb))
		return 0;
	ret = 1;
finish:	
	if(SUPPORT_SRAM_ENTRY && (foe_entry_idx < hwnat_dram_idx_base)) {
		setPpeEntry(foe_entry_idx, foe_entry);
	}

finish_no_need_set_entry:
#ifdef CONFIG_TP_IMAGE
	if (foe_entry && BIND == foe_entry->bfib1.state)
	{
		tp_update_hnat_entry(dev, foe_entry, foe_entry_idx);
	}
#endif /* CONFIG_TP_IMAGE */

	return ret;
}

int ecnt_asym_update_entry(struct sk_buff* skb,int magic)
{	
	int foe_index = skb->ecnt_asym_mark&ECNT_ASYM_MARK_FOE_INDEX_MASK;
	struct FoeEntry *foe_entry = NULL;
	unsigned int offset = 0;
	
	if((FOE_MAGIC_WLAN == magic)&&( skb->ecnt_asym_mark&ECNT_ASYM_MARK_BINDED))
	{
		foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + foe_index*SIZE_OF_FOE_ENTRY);
		
		if(!IS_IPV4_GRP(foe_entry))
			return 0;

		if (is24GWiFiInterface(skb->dev) &&
			((get24GWifiIndex(skb->dev)>=0) && (get24GWifiIndex(skb->dev)<=7))) 
		{
	         	offset = DP_RA0 + get24GWifiIndex(skb->dev);
		}
		else if (is5GWiFiInterface(skb->dev) &&
			((get5GWifiIndex(skb->dev)>=0) && (get5GWifiIndex(skb->dev)<=7))) 
		{
         		offset = DP_RAI0 + get5GWifiIndex(skb->dev);
		}
		else if (is6GWiFiInterface(skb->dev) &&
			((get6GWifiIndex(skb->dev)>=0) && (get6GWifiIndex(skb->dev)<=7))) 
		{
			offset = DP_RAX0 + get6GWifiIndex(skb->dev);
		}
		else
			return 0;

		foe_ext[FOE_ENTRY_NUM(skb)].asym_wlan_dp = offset;

		ECNT_ASYM_API_KA_HANDLER(skb,ECNT_ASYM_KA_POINT_WLAN);
	}
		
	return 0;
}

void ppeSetDualEntryInfo(unsigned int hash_index, struct ipv4_hnapt_dual_info1 *dual_info1, struct ipv4_hnapt_dual_info2 *dual_info2)
{
	unsigned char sp = 0;

	sp = (foe_ext[hash_index].sp & 0xf);
	
	/* fill in the info1 of other direction */
	dual_info1->new_info_blk2 = 0xffe221;
	dual_info1->new_udf = 0;
	dual_info1->new_ts_id = 0x7f;
	dual_info1->new_channel = sp;
	dual_info1->new_qid = 1;
	dual_info1->new_resv = 0;
	
	/* fill in the info2 of other direction */
	FoeSetMacInfo(dual_info2->new_dmac_hi, foe_ext[hash_index].smac);
	FoeSetMacInfo(dual_info2->new_smac_hi, foe_ext[hash_index].dmac);
	dual_info2->new_sp_tag = (1 << sp);
	dual_info2->new_pppoe_id = 0;
}

int ppe_tx_fast_bind(struct sk_buff *skb, struct port_info *pinfo, int magic)
{
   	struct ethhdr *eth = NULL;
    struct vlan_hdr *vh = NULL;
    struct iphdr *iph = NULL;
    struct tcphdr *th = NULL;
	unsigned char syc_flag = 0;

    eth = (struct ethhdr *)skb->data;        
    if ((eth->h_proto == htons(ETH_P_8021Q)) || ((magic == FOE_MAGIC_GE) && pinfo->eth_macSTagEn))
    {
	    vh = (struct vlan_hdr *)(skb->data + ETH_HLEN);
	    if (vh->h_vlan_encapsulated_proto == htons(ETH_P_IP))
	    {
	        iph = (struct iphdr *)(skb->data + ETH_HLEN + VLAN_HLEN);
	    }
    }
    else if (eth->h_proto == htons(ETH_P_IP))
    {
        iph = (struct iphdr *)(skb->data + ETH_HLEN);
    }      
    else
	{
		return 0;
	}

	/* process TCP */
	if ((NULL == iph) || (iph->protocol != IPPROTO_TCP))
	{
		return 0;
	}

    th = (struct tcphdr *)((uint8_t *)iph + (iph->ihl * 4));
    if (th->syn)
    {
    	syc_flag = 1;
        FOE_AI(skb) = HIT_UNBIND_RATE_REACH;
    }

	return syc_flag;
}


int PpeUpdateDsliteTunnel_swtable(int index,int rem){
	unsigned short prechk = 0;
	int saddr_id = 0;

	if(index<0 || index >= TUNNEL_TABLE_NUM){
		return 0;
	}

	if(rem == 0 ){
		TUNNEL_SWTABLE[index].config.word = 0x3000;
		
		TUNNEL_SWTABLE[index].ip_header0.ipv6_raw.hop_limit = PpeParseResult.ip6h.hop_limit;
		TUNNEL_SWTABLE[index].ip_header0.ipv6_raw.traffic_class = (PpeParseResult.ip6h.priority<<4)|\
			((PpeParseResult.ip6h.flow_lbl[2]&0xf0)>>4);

		
		prechk = PpeCalipv6_prechk(PpeParseResult.ip6h.saddr.s6_addr16,PpeParseResult.ip6h.daddr.s6_addr16);
		TUNNEL_SWTABLE[index].ip_header0.ipv6_raw.pre_chk = prechk;

		saddr_id = find_and_update_wan_ip6_table(PpeParseResult.ip6h.saddr.s6_addr32);
		if(saddr_id != -1) 
			TUNNEL_SWTABLE[index].ip_header1.ipv6_raw.sip_id = saddr_id;

		TUNNEL_SWTABLE[index].ip_header1.ipv6_raw.flow_lable = ((PpeParseResult.ip6h.flow_lbl[2]&0xf)<<16)|\
			(PpeParseResult.ip6h.flow_lbl[1]<<8)|PpeParseResult.ip6h.flow_lbl[0];

		TUNNEL_SWTABLE[index].ip_header2.ipv6_dip0 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[0]);
		TUNNEL_SWTABLE[index].ip_header3.ipv6_dip1 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[1]);
		TUNNEL_SWTABLE[index].ip_header4.ipv6_dip2 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[2]);
		TUNNEL_SWTABLE[index].ip_header5.ipv6_dip3 = ntohl(PpeParseResult.ip6h.daddr.s6_addr32[3]);
	}

	return 0 ;
}

int PpeUpdate6rdTunnel_swtable(int index,int rem){
	unsigned short prechk = 0;

	if(index<0 || index >= TUNNEL_TABLE_NUM){
		return 0;
	}

	if(rem == 0 ){
		TUNNEL_SWTABLE[index].config.word = 0x2000;

		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.ttl = PpeParseResult.iph.ttl;
		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.dscp = PpeParseResult.iph.tos;

		prechk = PpeCalipv4_prechk(ntohl(PpeParseResult.iph.saddr),ntohl(PpeParseResult.iph.daddr));
		TUNNEL_SWTABLE[index].ip_header0.ipv4_raw.pre_chk = prechk;

		TUNNEL_SWTABLE[index].ip_header1.ipv4_raw.iden = ntohs(PpeParseResult.iph.id);
		
		TUNNEL_SWTABLE[index].ip_header2.ipv4_sip = ntohl(PpeParseResult.iph.saddr);
		TUNNEL_SWTABLE[index].ip_header3.ipv4_dip = ntohl(PpeParseResult.iph.daddr);
	}

	return 0 ;
}

/* MAP-T HWNAT */
static inline int PpeIsMaptUpstream(struct FoeEntry *foe_entry)
{
	return (PpeParseResult.pkt_type == IPV6_5T_ROUTE) && 
		(foe_entry->bfib1.pkt_type == IPV4_HNAPT);
}

static inline int PpeIsMaptDownstream(struct FoeEntry *foe_entry)
{
	return (PpeParseResult.pkt_type == IPV4_HNAPT) && 
		(foe_entry->bfib1.pkt_type == IPV6_5T_ROUTE);
}

static inline int PpeCheckMapT(struct sk_buff * skb, struct FoeEntry *foe_entry)
{
	return PpeIsMaptUpstream(foe_entry) || PpeIsMaptDownstream(foe_entry);
}

static int PpeMapTunnelSwtable(struct sk_buff * skb, struct FoeEntry *foe_entry)
{
	int table_index = -1;

	if(PpeIsMaptUpstream(foe_entry)){
		table_index = PpeGetTunnel_table_index(0, DSLITE_TUNNEL);
		if(table_index != -1 ){
			if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0
				&& current_map_tunnel_num >= map_tunnel_max_num){
				if(DebugLevel >= 2)
					printk("mapt goes offload\n");
				return 1;
			}
			if(DebugLevel >= 2)
				printk("mapt goes tunnel\n");
			PpeParseResult.tunnel_en = 1;
			PpeParseResult.tunnel_id = table_index;
			PpeParseResult.hop_0 = 6; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel
		
			if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0){
				PpeUpdateDsliteTunnel_swtable( table_index, 0);
				PpeSetTunnel_table_ToHW( table_index );
				TUNNEL_SWTABLE[table_index].map_flag = 1;
				current_map_tunnel_num++;
				TUNNEL_SWTABLE[table_index].is_table_add_valid = 1;
			}
			ppe_add_session_to_tunnel_list(table_index,FOE_ENTRY_NUM(skb));
		}else{
			if(DebugLevel >= 2)
				printk("there is none tunnel table use for MAP DSLITE tunnel\n");
			return 1;
		}
	}
	else{
		table_index = PpeGetTunnel_table_index(0, T6RD_TUNNEL);
		if(table_index != -1 ){
			if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0
				&& current_map_tunnel_num >= map_tunnel_max_num){
				if(DebugLevel >= 2)
					printk("mapt down goes offload\n");
				return 1;
			}
			if(DebugLevel >= 2)
				printk("mapt down goes tunnel\n");
			PpeParseResult.tunnel_en = 1;
			PpeParseResult.tunnel_id = table_index;
			PpeParseResult.hop_0 = 6; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_1 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_2 = 0; //6 add runnel 7 rm tunnel
			PpeParseResult.hop_3 = 0; //6 add runnel 7 rm tunnel

			if(TUNNEL_SWTABLE[table_index].is_table_add_valid == 0){
				PpeUpdate6rdTunnel_swtable(table_index, 0);
				PpeSetTunnel_table_ToHW( table_index);
				TUNNEL_SWTABLE[table_index].map_flag = 1;
				current_map_tunnel_num++;
				TUNNEL_SWTABLE[table_index].is_table_add_valid = 1;
			}
			ppe_add_session_to_tunnel_list(table_index,FOE_ENTRY_NUM(skb));

		}else{
			if(DebugLevel >= 2)
				printk("there is none tunnel table use for MAP 6RD tunnel\n");
			return 1;
		}
	}
	
	return 0;
}

extern unsigned char *GetMacAddr(void);
ppeMapt *ppe_mapt_info = NULL;
dma_addr_t mapt_info_phy_addr;
static int mapt_npu_core_num = 1;
static int mapt_npu_core[8] = {NPU_BRIDGE_CHNL7,6,5,4,3,2,1,0};

static inline int PpeMaptGetNpuCore(void)
{
	static int cur_npu_core_idx = 0;
	int cur_npu_core = mapt_npu_core[cur_npu_core_idx];
	
	if(++cur_npu_core_idx == mapt_npu_core_num)
		cur_npu_core_idx = 0;

	return cur_npu_core;
}

static int PpeTunnelMapFillEntry(struct sk_buff * skb, struct FoeEntry *foe_entry)
{
	if (foe_entry->bfib1.state == BIND)
		return 1;

	PpeClearEntryInfo(foe_entry);

	if(PpeIsMaptUpstream(foe_entry)){
		FoeSetMacInfo(foe_entry->ipv4_hnapt.smac_hi, PpeParseResult.smac);
		FoeSetMacInfo(foe_entry->ipv4_hnapt.dmac_hi, GetMacAddr());
		
		if (PpeParseResult.ip6h.nexthdr == IPPROTO_TCP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.th.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.th.dest);
		} else if (PpeParseResult.ip6h.nexthdr == IPPROTO_UDP) {
			foe_entry->ipv4_hnapt.new_sport = ntohs(PpeParseResult.uh.source);
			foe_entry->ipv4_hnapt.new_dport = ntohs(PpeParseResult.uh.dest);
		}else{ 
			HWNAT_BINDFAIL_DPRINT("protocol %d,MAP-T up only suooprt TCP/UDP\n", PpeParseResult.ip6h.nexthdr);
			return 1;
		}
	}
	else{
		set_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE, 0x10);
		FoeSetMacInfo(foe_entry->ipv6_5t_route.dmac_hi, GetMacAddr());

		if (PpeParseResult.iph.protocol != IPPROTO_TCP && 
			PpeParseResult.iph.protocol != IPPROTO_UDP) {
			HWNAT_BINDFAIL_DPRINT("protocol %d,MAP-T down only suooprt TCP/UDP\n", PpeParseResult.iph.protocol);
			return 1;
		}	
	}

	PpeMapTunnelSwtable(skb, foe_entry);

	return 0;
}

static int ppeMaptSaveHeader(struct sk_buff * skb, struct FoeEntry *foe_entry, int down)
{
	int foe_index = FOE_ENTRY_NUM(skb);

	if(unlikely(ppe_mapt_info == NULL)){
		HWNAT_BINDFAIL_DPRINT("ppe_mapt_info is NULL, CPU path.\n");
		return 1;
	}

	if(down == 0){
		memcpy(&ppe_mapt_info[foe_index].ip6h, &PpeParseResult.ip6h, sizeof(struct ipv6hdr));
		return 0;
	}

	memcpy(&ppe_mapt_info[foe_index].v4IpPort.iph, &PpeParseResult.iph, sizeof(struct iphdr));
	
	if (PpeParseResult.iph.protocol == IPPROTO_TCP) {
		ppe_mapt_info[foe_index].v4IpPort.sport = PpeParseResult.th.source;
		ppe_mapt_info[foe_index].v4IpPort.dport = PpeParseResult.th.dest;
	} else if (PpeParseResult.iph.protocol == IPPROTO_UDP) {
		ppe_mapt_info[foe_index].v4IpPort.sport = PpeParseResult.uh.source;
		ppe_mapt_info[foe_index].v4IpPort.dport =PpeParseResult.uh.dest;
	}	

	return 0;
}

static int PpeMaptHandle(struct sk_buff * skb, struct FoeEntry *foe_entry)
{
	int tunnel = PpeParseResult.tunnel_en;

	if(!PpeCheckMapT(skb, foe_entry))
		return 0;
	
	if(PpeIsMaptUpstream(foe_entry)){
		if(!tunnel){
			if(ppeMaptSaveHeader(skb, foe_entry, 0))
				return 1;
		}
		foe_entry->ipv4_hnapt.fqos = 0;
		foe_entry->ipv4_hnapt.nbq = PpeMaptGetNpuCore(); // NPU core
		foe_entry->ipv4_hnapt.fpidx = FP_NPU;
		foe_entry->ipv4_hnapt.act_dp = tunnel?NPU_UDF_MAP_T_TUNNEL_UP:NPU_UDF_MAP_T_OFFLOAD_UP;
	}
	else if(PpeIsMaptDownstream(foe_entry)){
		if(!tunnel){
			if(ppeMaptSaveHeader(skb, foe_entry, 1))
				return 1;
		}
		foe_entry->ipv6_5t_route.fqos = 0;
		foe_entry->ipv6_5t_route.nbq = PpeMaptGetNpuCore();
		foe_entry->ipv6_5t_route.fpidx = FP_NPU;
		foe_entry->ipv6_5t_route.act_dp = tunnel?NPU_UDF_MAP_T_TUNNEL_DOWN:NPU_UDF_MAP_T_OFFLOAD_DOWN;

		/* save dest port in the ppe entry etype field for NPU */
		if (PpeParseResult.iph.protocol == IPPROTO_TCP) {
			foe_entry->ipv6_5t_route.etype = ntohs(PpeParseResult.th.dest);
		} else if (PpeParseResult.iph.protocol == IPPROTO_UDP) {
			foe_entry->ipv6_5t_route.etype = ntohs(PpeParseResult.uh.dest);
		}		
	}

	PpeParseResult.vlan_layer = 0;
	PpeParseResult.pppoe_gap = 0;

	return 0;
}

static int mapt_info_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int i = 0;
	
	printk("ppe_mapt_info: %pK\n",ppe_mapt_info);
	printk("map_tunnel_max_num: %d\nmapt_npu_core_num: %d\nmapt_npu_core:",map_tunnel_max_num, mapt_npu_core_num);
	for(i=0; i<mapt_npu_core_num ;i++)
		printk("%d ", mapt_npu_core[i]);
	
	return 0;	
}

static int mapt_info_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[64], cmd[16], cmd1[32];
	int flag = 0;
	struct device *dev = NULL;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL;
	
	memset(get_buf, 0, sizeof(get_buf));
	memset(cmd, 0, sizeof(cmd));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%s %d %s", cmd, &flag, cmd1);

	if(!strcmp(cmd, "enable")){
		if(NULL == (dev=get_frame_engine_dev()))
		{
			printk("hw_nat get_frame_engine_dev failed.\n") ; 
			return count;
		}
		
		if(flag){
			if(ppe_mapt_info)
				return count;
			ppe_mapt_info = dma_alloc_coherent(dev, FOE_4TB_SIZ_TOTAL*sizeof(ppeMapt), &mapt_info_phy_addr, GFP_KERNEL);
			if(ppe_mapt_info == NULL){
				printk("Fail to dma_alloc_coherent ppe_mapt_info!\n");	
				return count;
			}
			tunnel_mail_store_mapt_info_addr(mapt_info_phy_addr);
		}
		else{
			if(ppe_mapt_info){
				dma_free_coherent(dev, FOE_4TB_SIZ_TOTAL*sizeof(ppeMapt), ppe_mapt_info, mapt_info_phy_addr);
				ppe_mapt_info = NULL;
				mapt_info_phy_addr = 0;
			}
		}
	}
	else if(!strcmp(cmd, "tunnel")){
		map_tunnel_max_num = flag;
	}
	else if(!strcmp(cmd, "npu_core")){
		mapt_npu_core_num = flag;
		sscanf(cmd1, "%d,%d,%d,%d,%d,%d,%d,%d", &mapt_npu_core[0],&mapt_npu_core[1],&mapt_npu_core[2],\
			&mapt_npu_core[3],&mapt_npu_core[4],&mapt_npu_core[5],&mapt_npu_core[6],&mapt_npu_core[7]);
	}
	
	return count;
}
__IMEM int32_t PpeTxHandler(struct sk_buff * skb, struct port_info * pinfo, int magic)
{
	unsigned char syc_flag = 0;
	struct ipv4_hnapt_dual_info1 dual_info1;
	struct ipv4_hnapt_dual_info2 dual_info2;
	
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int foe_entry_idx=0, ret=0;
	unsigned long lock_flag;

    if(!HWNAT_IS_ENABLE()){
		return -1;
    }

	if((FOE_AI(skb) == HIT_BIND_FORCE_TO_CPU)||
        (FOE_AI(skb) == HIT_BIND_MUL_CPU) || 
        (FOE_AI(skb) == HIT_BIND_MUL_CPUR) )
		return 1;

	if(TCSUPPORT_GPON_MAPPING_VAL&&TCSUPPORT_GPON_DOWNSTREAM_MAPPING_VAL&&
		(skb->pon_mark & DS_PKT_MAPPING_TO_ONE)&&(skb->ppe_info_flag == 1))
	{
		foe_entry_idx = skb->ppe_foe_entry;
		FOE_MAGIC_TAG(skb) = skb->ppe_magic;
		FOE_AI(skb) = skb->ppe_ai;
		FOE_ENTRY_NUM(skb) = foe_entry_idx;
	}
	else
		foe_entry_idx = FOE_ENTRY_NUM(skb);

	foe_entry = getPpeEntryLocal(foe_entry_idx, foe_entry);

	if(foe_entry == NULL)
		return -1;

	if(INVALID == foe_entry->bfib1.state)
	{	
		return -1;
	}

	if(ecnt_ppe_tx_hanlder_start_hook(foe_entry,skb,pinfo,magic))
	{
		HWNAT_BINDFAIL_DPRINT("return at tx handler start\n");
		return 1;
	}

	if (DebugLevel >= 3) {
		printk("PpeTxHandler FOE_MAGIC %x FOE_AI %x magic %x \n",FOE_MAGIC_TAG(skb), FOE_AI(skb),magic);
	}

	if((0 == FOE_MAGIC_TAG(skb))&&(0 == FOE_AI(skb)))
	{
		if((hwnat_dbg_bindfail_idx == skb->ecnt_sk_buff.foe_index)&&printk_ratelimit())
			printk("%s,%d, skb->foe is set to 0\n",__FUNCTION__,__LINE__);
	}

    if (((((hwnat_fast_bind == 1) || (3 == hwnat_fast_bind)) && (FOE_AI(skb) == TCP_FIN_SYN_RST)) 
		|| ((hwnat_fast_bind == 2) && (FOE_AI(skb) == UN_HIT))) && (foe_entry->bfib1.state != BIND))
    {
		syc_flag = ppe_tx_fast_bind(skb, pinfo, magic);
	}

	if(ppe_check_start_to_bind(skb,foe_entry))	
	{
		spin_lock_irqsave(&hw_nat_lock, lock_flag);

		/* get start addr for each layer */
		if (PpeParseLayerInfo(skb, pinfo, magic,foe_entry))
		{
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
			HWNAT_BINDFAIL_DPRINT("parse layer info fail\n");
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			ret = 1;
			goto finish;
		}

		if(unlikely(left_to_right_test_mode == TEST_MODE_VXLAN)
			&& update_vxlan_up_info_hook && (foe_ext[foe_entry_idx].vxlan_up_2_flag == 1))
		{ 
			// update tcont, gemport, vlan, mac
			update_vxlan_up_info_hook((unsigned int)pinfo->channel, (unsigned int)pinfo->stag, (unsigned int)ntohs(PpeParseResult.vlan1), 
										(unsigned char *)PpeParseResult.smac, (unsigned char *)PpeParseResult.dmac);
			foe_ext[foe_entry_idx].vxlan_up_2_flag = 0; /* if msg0 & ms1 is set zero, info should be update new */
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
			ret = 1;
			goto finish;
		}
		//tunnel set inner tunnel info
		if((PpeParseResult.is_L2_tunnel||PpeParseResult.is_L3_tunnel)
			&&(( magic!=FOE_MAGIC_GRE_HWDOWN_1) && ( magic!=FOE_MAGIC_VXLAN_HWDOWN_1)&& ( magic!=FOE_MAGIC_L2TP_HWDOWN_1))){
			
			/* Set Layer2 Info */

			if (PpeFillInL2Info_InTunnel(skb, foe_entry)) {
				spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
				HWNAT_BINDFAIL_DPRINT("fill L2 info fail\n");
				memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
				ret = 1;
				goto finish;
			}

			/* Set Layer3 Info */
			if (PpeFillInL3Info_InTunnel(skb, foe_entry)) {
				spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
				HWNAT_BINDFAIL_DPRINT("fill L3 info fail\n");
				memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
				ret = 1;
				goto finish;
			}

			/* Set Layer4 Info */
			if (PpeFillInL4Info_InTunnel(skb, foe_entry)) {		
				spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
				HWNAT_BINDFAIL_DPRINT("fill L4 info fail\n");
				memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
				ret = 1;
				goto finish;
			}
	
		}
		else if(PpeCheckMapT(skb, foe_entry)){
			if(PpeTunnelMapFillEntry(skb, foe_entry)){
				spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
				HWNAT_BINDFAIL_DPRINT("PpeTunnelMapFillEntry fail\n");
				ret = 1;
				goto finish;
			}
		}		
		else{
			/* Set Layer2 Info */
			if (PpeFillInL2Info(skb, foe_entry)) {
				spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
				HWNAT_BINDFAIL_DPRINT("fill L2 info fail\n");
				memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
				ret = 1;
				goto finish;
			}

		/* Set Layer3 Info */
		if (PpeFillInL3Info(skb, foe_entry)) {
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
			HWNAT_BINDFAIL_DPRINT("fill L3 info fail\n");
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			ret = 1;
			goto finish;
		}

			/* Set Layer4 Info */
			if (PpeFillInL4Info(skb, foe_entry)) {		
				spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
				HWNAT_BINDFAIL_DPRINT("fill L4 info fail\n");
				memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
				ret = 1;
				goto finish;
			}

		}

		/* Set force port info */
		if (PpeSetForcePortInfo(skb, foe_entry, pinfo, magic))
		{
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
			HWNAT_BINDFAIL_DPRINT("set force port fail\n");
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			ret = 1;
			goto finish;
		}

		/* Set Pseudo Interface info in Foe entry */
		if (PpeSetExtIfNum(skb, foe_entry,magic))
		{	
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);	
			HWNAT_BINDFAIL_DPRINT("set ext if num fail\n");
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			ret = 1;
			goto finish;
		}

		ppeUpdateAcntInfo(foe_entry, skb, magic, pinfo);
        if(check_gemport){
            foe_ext[FOE_ENTRY_NUM(skb)].gemport = skb->gem_port;
        }

		if(PpeMaptHandle(skb, foe_entry)){
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);	
			ret = 1;
			goto finish;
		}
		
		/* Enter binding state */
		if(isValidPpeEntry(skb, foe_entry)) {
			PpeSetEntryBind(skb, foe_entry);
			foe_ext[FOE_ENTRY_NUM(skb)].tx_dev = skb->dev;
		} else {
			spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
			if (DebugLevel >= 1) {
				printk("Invalid PPE entry, not bind !!!, FOE index = %d\n", foe_entry_idx);
			}
			memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
			ret = 1;
			goto finish;
		}
		
		if(TCSUPPORT_HWNAT_L2VID_VAL)
			PpeL2luKeySetStatusBind(FOE_ENTRY_NUM(skb));

		/* only upstream to set Dual entry */
		if((3 == hwnat_fast_bind) && (1 == syc_flag) && (FOE_SP(skb) != SP_GDMA2))
		{
			ppeSetDualEntryInfo(foe_entry_idx, &dual_info1,&dual_info2);
			ppeSetDualEntry(foe_entry_idx, foe_entry, &dual_info1, &dual_info2);
		}
		else
		{
			if(SUPPORT_SRAM_ENTRY && (foe_entry_idx < hwnat_dram_idx_base)) 
			{
				setPpeEntry(foe_entry_idx, foe_entry);
			}
		}

		if (xpon_igmp_learn_flow_hook && ppe_is_multicast_entry(foe_entry))
			xpon_igmp_learn_flow_hook(skb);
		else
			ecnt_multicast_hwnat_learn_flow(skb);

		spin_unlock_irqrestore(&hw_nat_lock, lock_flag);

		ecnt_ppe_tx_hanlder_end_hook(foe_entry,skb,pinfo,magic);
		/* For JOYME4 4.1.1 EPON Test */
		if(TCSUPPORT_CT_JOYME4_VAL && isEN7528) {
			natv6_411_test_handle_for_epon(foe_entry_idx, foe_entry, magic);
		}
		
		/* Dump Binding Entry */
		if (DebugLevel >= 1) {
			FoeDumpEntry(FOE_ENTRY_NUM(skb));
		}
	} else if (IS_MAGIC_TAG_VALID(skb)
		   && (FOE_AI(skb) == HIT_BIND_KEEPALIVE_MC_NEW_HDR
		       || (FOE_AI(skb) == HIT_BIND_KEEPALIVE_DUP_OLD_HDR))) {
		/* this is duplicate packet in keepalive new header mode, 
		 * just drop it */
		memset(FOE_INFO_START_ADDR(skb), 0, FOE_INFO_LEN);
		ret = 0;
		goto finish;	
	}

	ret = 1;
finish:
	return ret;
}

static int hwnat_recover_info_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("HWNAT Modify Info Flag: %d \n",recover_info_flag);
	return 0;	
}

static int hwnat_recover_info_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	unsigned int info_flag = 0;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d",&info_flag);

	recover_info_flag = info_flag;
	
	return count;
}

/*****************************************************************************
******************************************************************************/

static int hwnat_fast_slow_info_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data){
	int index = 0;

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_ITTIM)
	off_t begin = 0;
	off_t pos = 0;
#endif /* defined(CONFIG_TP_IMAGE) && defined(INCLUDE_ITTIM) */
	if (hwnat_fast_slow_up_stream_flag == 1) {
		printk("hwnat_fast_slow_up_stream_flag value is %d, gpon/xgpon up stream fast mode enable\n", hwnat_fast_slow_up_stream_flag);
	} else if(hwnat_fast_slow_up_stream_flag == 2) {
		printk("hwnat_fast_slow_up_stream_flag value is %d, gpon/xgpon up stream fast mode disable\n", hwnat_fast_slow_up_stream_flag);
	} else {
		printk("hwnat_fast_slow_up_stream_flag value is %d, gpon/xgpon up stream fast mode contrled by others\n", hwnat_fast_slow_up_stream_flag);
	}
	if (hwnat_fast_slow_down_stream_flag == 1) {
		printk("hwnat_fast_slow_down_stream_flag value is %d, eth/xfi/8811 down stream fast mode enable\n", hwnat_fast_slow_down_stream_flag);
	} else if (hwnat_fast_slow_down_stream_flag == 2){
		printk("hwnat_fast_slow_down_stream_flag value is %d, eth/xfi/8811 down stream fast mode disable\n", hwnat_fast_slow_down_stream_flag);
	} else {
		printk("hwnat_fast_slow_down_stream_flag value is %d, eth/xfi/8811 down stream fast mode contrled by others\n", hwnat_fast_slow_down_stream_flag);
	}

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_ITTIM)
	index += sprintf(buf+index, "hwnat_fast_slow_up_stream_flag=%d\nhwnat_fast_slow_down_stream_flag=%d\n", hwnat_fast_slow_up_stream_flag, hwnat_fast_slow_down_stream_flag);

	CHK_BUF();

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index < 0) 
	{
		index = 0;
	}
	if (index > count) 
	{
		index = count;
	}
#endif /* defined(CONFIG_TP_IMAGE) && defined(INCLUDE_ITTIM) */

	return index;
}


/*****************************************************************************
******************************************************************************/

static int hwnat_fast_slow_info_write_proc(struct file *file, const char *buffer,unsigned long count, void *data)	
{
	char val_string[96] = {0}, cmd[32]={0}, subcmd[32]={0};
	// int value = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	// sscanf(val_string, "%d", &value);
	sscanf(val_string, "%31s %31s", cmd, subcmd) ;
	val_string[count] = '\0';
	// up && down
	if(!strcmp(cmd, "up")){
		if(!strcmp(subcmd, "enable") || !strcmp(subcmd, "1")) {
			hwnat_fast_slow_up_stream_flag = 1;
		} else if(!strcmp(subcmd, "disable") || !strcmp(subcmd, "2")) {
			hwnat_fast_slow_up_stream_flag = 2;
		} else {
			hwnat_fast_slow_up_stream_flag = 0;
		}
	} else {
		if(!strcmp(subcmd, "enable") || !strcmp(subcmd, "1")) {
			hwnat_fast_slow_down_stream_flag = 1;
		} else if(!strcmp(subcmd, "disable") || !strcmp(subcmd, "2")) {
			hwnat_fast_slow_down_stream_flag = 2;
		} else {
			hwnat_fast_slow_down_stream_flag = 0;
		}
	}
	
	// printk("fast_mode value is written %d\n", fast_mode);
	return count;
}

/******************************************************************************
******************************************************************************/

static int hwnat_nat66_enable_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("HWNAT nat66 enable Flag: %d \n",enable_nat66_flag);
	return 0;	
}

static int hwnat_nat66_enable_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	unsigned int flag = 0;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d",&flag);

	enable_nat66_flag = flag;
	
	return count;
}

void ecnt_hwnat_entry_init(void)
{	
	struct proc_dir_entry *hwnat_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_nptv6_prefix", 0, NULL);
	hwnat_proc->read_proc = hwnat_prefix_read_proc;

	hwnat_proc = create_proc_entry("tc3162/2544_test_pkt_rate", 0, NULL);
	hwnat_proc->read_proc = pkt_len_monitor_2544_read_proc;
	hwnat_proc->write_proc =  pkt_len_monitor_2544_write_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_fastmode", 0, NULL);
	hwnat_proc->read_proc = hwnat_fast_slow_info_read_proc;
	hwnat_proc->write_proc =  hwnat_fast_slow_info_write_proc;

	if(SUPPORT_TUNNEL_HW_ACCELE)
	{
		hwnat_proc = create_proc_entry("tc3162/hwnat_tunnel_info", 0, NULL);
		hwnat_proc->read_proc = hwnat_tunnel_info_read_proc;
		hwnat_proc->write_proc =  hwnat_tunnel_info_write_proc;
	}

    hwnat_proc = create_proc_entry("tc3162/hwnat_npu_vxlan_offload_shortcut", 0, NULL);
	hwnat_proc->read_proc = hwnat_npu_vxlan_offload_shortcut_read_proc;
    hwnat_proc->write_proc = hwnat_npu_vxlan_offload_shortcut_write_proc;

	if(TCSUPPORT_TEST_WLAN_SHORTCUT_VAL){
		hwnat_proc = create_proc_entry("tc3162/hwnat_recover_info", 0, NULL);
		hwnat_proc->read_proc = hwnat_recover_info_read_proc;
		hwnat_proc->write_proc =  hwnat_recover_info_write_proc;
	}
	hwnat_proc = create_proc_entry("tc3162/hwnat_nat66_enable", 0, NULL);
	hwnat_proc->read_proc = hwnat_nat66_enable_read_proc;
	hwnat_proc->write_proc =  hwnat_nat66_enable_write_proc;

	hwnat_proc = create_proc_entry("tc3162/mapt_info", 0, NULL);
	hwnat_proc->read_proc = mapt_info_read_proc;
	hwnat_proc->write_proc =  mapt_info_write_proc;
	
	ra_sw_nat_hook_free = PpeFreeHandler;

	ra_sw_nat_hook_rxinfo = PpeRxinfoHandler;
    
    if(SUPPORT_HIT_IFC_ACTION)
    {
        ra_sw_nat_hook_ifc_hit_info = PpeIfcHitinfoHandler;
    }

	ra_sw_nat_to_xsi_fast_tx = PpeXsiFastTxHandler;
	
	ra_sw_nat_hook_magic = PpeMagicHandler;

	ra_sw_nat_hook_set_magic = PpeSetMagicHandler;

	ra_sw_nat_hook_xfer = PpeXferHandler;
	
	ra_sw_nat_hook_foeentry = PpeFoeEntryHandler;
	
	ra_sw_nat_hook_drop_packet = PpeDropPacketHandler;
			
    hwnat_is_alive_pkt_hook = hwnat_is_alive_pkt;

    hwnat_skb_to_foe_hook = hwnat_skb_to_foe;

    hwnat_set_recover_info_hook = hwnat_set_recover_info;
    
    hwnat_skb_cpu_reason_hook = hwnat_skb_cpu_rason;
	 
	ra_sw_nat_hook_cpu_meter = ppeCpuPathRatelimitByMeter;
	
	if(SUPPORT_TUNNEL_HW_ACCELE){
		int i = 0;
		ra_sw_nat_hook_tunnel_down_acc_hook = PpeExtIfTUNNELDOWNHandler;

		memset(TUNNEL_SWTABLE,0,TUNNEL_TABLE_NUM*sizeof(tunnelTable_t));
	    for(i= 0;i < TUNNEL_TABLE_NUM;i++)    
		    INIT_LIST_HEAD(&(TUNNEL_SWTABLE[i].session_list));
	}

	ecnt_register_hook(&ecnt_copy_foe_info_op);

	ppeNptV6PrefixInit();

	memset(&pkt_len_monitor,0x0,sizeof(pkt_len_monitor));
	/* init 3 packet len */
	pkt_len_monitor.len_rate[0].len = 1024;
	pkt_len_monitor.len_rate[0].rate = 0;
	pkt_len_monitor.len_rate[1].len = 1280;
	pkt_len_monitor.len_rate[1].rate = 0;
	pkt_len_monitor.len_rate[2].len = 1518;
	pkt_len_monitor.len_rate[2].rate = 0;
	
	return;
}

void ecnt_hwnat_entry_deinit(void)
{	
	struct device *dev = NULL;

#ifdef TCSUPPORT_CPU_ARMV8
	if(NULL == (dev=get_frame_engine_dev()))
	{
		printk("hw_nat get_frame_engine_dev failed.\n") ; 
		return;
	}
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)

	static const u64 dmamask = DMA_BIT_MASK(32);
	
	dev=hnat_device;
	dev->dma_mask = (u64 *)&dmamask;
	dev->coherent_dma_mask = DMA_BIT_MASK(32);
#endif
#endif	
	remove_proc_entry("tc3162/hwnat_fastmode", NULL);

	remove_proc_entry("tc3162/hwnat_nptv6_prefix", NULL);

	remove_proc_entry("tc3162/2544_test_pkt_rate", NULL);
		
	if(SUPPORT_TUNNEL_HW_ACCELE)
		remove_proc_entry("tc3162/hwnat_tunnel_info", NULL);
	
	remove_proc_entry("tc3162/hwnat_npu_vxlan_offload_shortcut",NULL);

	if(TCSUPPORT_TEST_WLAN_SHORTCUT_VAL){
		remove_proc_entry("tc3162/hwnat_recover_info",NULL);
	}

	remove_proc_entry("tc3162/hwnat_nat66_enable",NULL);

	remove_proc_entry("tc3162/mapt_info", NULL);
	
	ra_sw_nat_hook_free = NULL;
    if(SUPPORT_HIT_IFC_ACTION)
    {
        ra_sw_nat_hook_ifc_hit_info = NULL;
    }

	ra_sw_nat_hook_magic = NULL;
	
	ra_sw_nat_hook_set_magic = NULL;
	
	ra_sw_nat_hook_xfer = NULL;	
	
	ra_sw_nat_hook_foeentry = NULL;

	ra_sw_nat_hook_drop_packet = NULL;
			
    hwnat_is_alive_pkt_hook = NULL;

    hwnat_skb_to_foe_hook = NULL;

	hwnat_set_recover_info_hook = NULL;

    hwnat_skb_cpu_reason_hook = NULL;
	
	ecnt_unregister_hook(&ecnt_copy_foe_info_op);

	ra_sw_nat_hook_cpu_meter = NULL;

	if(SUPPORT_TUNNEL_HW_ACCELE)
		ra_sw_nat_hook_tunnel_down_acc_hook = NULL;
			
	ra_sw_nat_hook_rxinfo = NULL;
	
	ra_sw_nat_hook_txq = NULL;
	
	ra_sw_nat_to_xsi_fast_tx = NULL;

	if(ppe_mapt_info)
		dma_free_coherent(dev, FOE_4TB_SIZ_TOTAL*sizeof(ppeMapt), ppe_mapt_info, mapt_info_phy_addr);
	
	return;
}
