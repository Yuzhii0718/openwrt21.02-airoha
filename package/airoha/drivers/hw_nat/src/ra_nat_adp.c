 /************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
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
#include <asm/tc3162/tc3162.h>
#include <ecnt_hook/ecnt_hook_hsgmii_mac.h>

#include <ecnt_hook/ecnt_hook_multicast.h>
#include <ecnt_hook/ecnt_hook_multicast_general.h>
#include <ecnt_hook/ecnt_hook_asym.h>

#include "ra_nat_cust.h"
#include "ra_nat_adp.h"
#include "ra_nat_gen.h"
#include "ra_nat_dev.h"
#include "foe_fdb.h"
#include "ppe_api.h"
#include "ppe2_reg.h"
#include <linux/foe_hook.h>
#include <linux/ecnt_utility.h>
#include "ra_nat_ic_dis.h"
#include "ra_nat_offload.h"

#include <linux/if_pppox.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
static int vlan_isOneToN = 0;

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern struct FoeEntryExt*	foe_ext;
extern int macqueue_start_idx;

extern PktParseResult		PpeParseResult;

extern unsigned char bind_dir;

extern uint32 dsoffload_presetting;
extern unsigned int g_mul_high_priority;
extern int udp_bypass_enable;
extern wan_virtualdev_ifname wan_virtualdev_ifname_info[16];
extern void ecnt_hwnat_ety_init(void);
extern void ecnt_hwnat_ety_deinit(void);
extern void ecnt_hwnat_offload_init(void);
extern void ecnt_hwnat_offload_deinit(void);
extern void ecnt_hwnat_gen_init(void);
extern void ecnt_hwnat_gen_deinit(void);
extern void ecnt_hwnat_multicast_init(void);
extern void ecnt_hwnat_multicast_deinit(void);
extern void ecnt_hwnat_cust_init(void);
extern void ecnt_hwnat_cust_deinit(void);
extern void ecnt_hwnat_npu_init(void);
extern void ecnt_hwnat_npu_deinit(void);
extern void ecnt_hwnat_wifi_init(void);
extern void ecnt_hwnat_wifi_deinit(void);
extern void ecnt_hwnat_entry_init(void);
extern void ecnt_hwnat_entry_deinit(void);

extern int (*soft_ratelimit_enqueue_hook) (struct sk_buff * skb,unsigned int queue_idx);

extern void FtpCtlPacketHandle(struct sk_buff* skb);
extern int setPPeTxACNTGRP(struct sk_buff * skb, struct FoeEntry * foe_entry);
extern void ppeCalcFlowCnt(struct sk_buff * skb, int magic, struct port_info * pinfo);
extern int set_l2tp_vpn_force_info(struct sk_buff *skb,struct FoeEntry *foe_entry, struct port_info *pinfo, int magic);
extern void PpeSetLanIpAcnt(struct FoeEntry * foe_entry, struct sk_buff * skb, struct port_info* pinfo, int magic);
extern void store_offload_info(struct sk_buff *skb,struct port_info * pinfo, int magic);
extern void PpeSetPortInfo(struct FoeEntry * foe_entry, struct port_info * pinfo, int magic);
extern void PpeSetInfoBlk2(struct FoeEntry * foe_entry, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag, uint32_t ds_to_qdma);

extern int ppe_is_multicast_entry(struct FoeEntry *foe_entry);
extern int ecnt_multicast_hwnat_data_handler(struct sk_buff* skb);
extern  int setPPeRxACNTGRP(struct sk_buff * skb);
extern int hwnat_lan_wan_rx(unsigned int idx,struct sk_buff* sk);
extern void PpeBlack_ip_foe_handle(struct sk_buff *skb, int direction);

extern void RegModifyBits(uint32_t Addr, uint32_t Data, uint32_t Offset, uint32_t Len);
extern int ppe_set_ether_type(uint index, PPE_Enable_t enable, PPE_PPPOE_t is_pppoe, uint value);
extern void Ppe_Vip_Tmp_Handler(struct sk_buff* skb);
extern void ecnt_br_Mul_To_LAN_Config(struct sk_buff* skb);
extern void (*wlan_to_lan_hook)(struct sk_buff *skb, u8 local);

extern int ecnt_asym_update_entry(struct sk_buff* skb,int magic);

extern int ppeCheckIpv6Nat(struct sk_buff * skb,struct FoeEntry * foe_entry,int magic,struct port_info * pinfo);

extern int ppeCheckIfcHit(struct sk_buff * skb,struct FoeEntry * foe_entry, int magic, struct port_info * pinfo);

extern int PpeTxHandler(struct sk_buff * skb,struct port_info * pinfo,int magic);
extern int PpeRxHandler(struct sk_buff * skb);
extern void PpeSetFqos(struct FoeEntry * foe_entry,unsigned char value);
extern uint8 *GetMacAddr(void);

extern unsigned int wan_conn_type;
extern char wan_itf_name[];


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
#ifdef CONFIG_TP_IMAGE
extern void*  ppe_get_ppp_network_header(struct sk_buff* skb);
extern int ppe_get_packet_type(struct sk_buff* skb, int *offlen);
int is_skb_ppp_udp_session(struct sk_buff* skb)
{
	
	struct iphdr *iph = NULL;
	int offlen = 0;
	unsigned int eth_type = 0;

	/* get packet type, if ETH_P_PPP_SES it will return IPV4/IPV6 */
	eth_type = ppe_get_packet_type(skb,&offlen);

	if (PACKET_IPV4 == eth_type)
	{
		/* only get ETH_P_PPP_SES network header */
		iph = (struct iphdr* ) ppe_get_ppp_network_header(skb);

		if (iph && IPPROTO_UDP == iph->protocol)
		{
			if(ip_is_fragment(iph))
			{
				return 1;
			}
		}
	}
	else if (PACKET_IPV6 == eth_type)
	{
		//hardware do not support ipv6 udp fragment accelate
	}
	else
	{
		//we do not need process this
	}

	return 0;
}
#endif /*CONFIG_TP_IMAGE*/  
	
int ecnt_ppe_tx_hanlder_start_hook(struct FoeEntry *foe_entry,struct sk_buff* skb,struct port_info * pinfo, int magic)
{

	struct iphdr *iph = NULL;


    
    if((magic == FOE_MAGIC_WLAN) || (magic == FOE_MAGIC_PCI)){
        if(wlan_to_lan_hook != NULL)
            wlan_to_lan_hook(skb, 0);
    }


	if (udp_bypass_enable == 1){
		iph = (struct iphdr *)skb_network_header(skb);
		if ((iph->protocol == IPPROTO_UDP) && ((magic == FOE_MAGIC_WLAN) || (magic == FOE_MAGIC_PCI))) {
			return 1;
		}
	}
	
#ifdef CONFIG_TP_IMAGE
	/* 上行禁止ppp udp 分片包加速, XSI发出由于目前未区分wan/lan，无法判断是否上行，统一认为可能是上行需要判断 */
	if((isUpStreamFlow(magic, pinfo) || FOE_MAGIC_XSI == magic) && is_skb_ppp_udp_session(skb))
	{
		return 1;
	}
#endif /*CONFIG_TP_IMAGE*/

	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting)
	{	
		if(dsoffload_presetting&DS_FTP)
			FtpCtlPacketHandle(skb);
	}

	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL||TCSUPPORT_CF_JOYMEV2_PON_VAL||TCSUPPORT_XPON_HAL_API_EXT_VAL || TCSUPPORT_CRJO_VAL)
		setPPeTxACNTGRP(skb, foe_entry);


	if((TCSUPPORT_CT_PPPOEPROXY_VAL&&TCSUPPORT_CT_PON_CN_JS_VAL)&&(FOE_MAGIC_TAG(skb) == magic)&&(FOE_SP(skb) != SP_PDMA))
		//workaroud, do not go hwnat.
		return 1;

	if(TCSUPPORT_XPON_HAL_API_EXT_VAL||TCSUPPORT_XPON_HAL_API_VAL||TCSUPPORT_CMCC_VAL||TCSUPPORT_CSC_EEUROPE_VAL)
		ppeCalcFlowCnt(skb, magic, pinfo);

	
	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL||TCSUPPORT_XPON_HAL_API_EXT_VAL || TCSUPPORT_CRJO_VAL)
	{
		if ((FOE_AI(skb) == HIT_UNBIND_RATE_REACH)&&((magic == FOE_MAGIC_PCI) || (magic == FOE_MAGIC_WLAN))) 
			foe_ext[FOE_ENTRY_NUM(skb)].rx_dev = 1;
	}

	if(TCSUPPORT_ASYM_VOD_VAL)
		ecnt_asym_update_entry(skb,magic);

	if((magic == FOE_MAGIC_USBNET)&&TCSUPPORT_CPU_EN758X_VAL)
	{
		pinfo->tsid = 0x7f;
		pinfo->nbq = 1;
	}

	return 0;
}

int ecnt_ppe_tx_hanlder_end_hook(struct FoeEntry *foe_entry,struct sk_buff* skb,struct port_info * pinfo, int magic)
{
	if(skb->ecnt_asym_mark&ECNT_ASYM_MARK_KA)
		ECNT_ASYM_API_KA_HANDLER(skb,ECNT_ASYM_KA_POINT_LAN_BIND);

	if((magic == FOE_MAGIC_GE) || (magic == FOE_MAGIC_WLAN)|| (magic == FOE_MAGIC_WLAN_TX))
	{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
		struct nf_conntrack *nfct = skb_nfct(skb);
		if(nfct)
			nfct->foe_index = FOE_ENTRY_NUM(skb);
#else
		if(skb->nfct)
			skb->nfct->foe_index = FOE_ENTRY_NUM(skb);
#endif		
	}
	return 0;
}

int ecnt_ppe_set_force_port_info_start_hook(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,u_int16_t meterGrpId,u_int16_t acntGrpIndex)
{
	unsigned int foe_index = FOE_ENTRY_NUM(skb);
	
	ppeCheckIpv6Nat(skb,foe_entry,magic,pinfo);

    ppeCheckIfcHit(skb, foe_entry, magic, pinfo);
	if (set_l2tp_vpn_force_info(skb,foe_entry,pinfo,magic))
	{
		if(TCSUPPORT_XPON_HAL_API_EXT_VAL)
			PpeSetLanIpAcnt(foe_entry, skb, pinfo, magic);
		return 0;
	}

	if(PpeParseResult.map_e == 1   ){
		if(foe_entry->ipv4_hnapt.bfib1.pkt_type == IPV4_DSLITE){
			FoeSetEntryMac(GetMacAddr(), (foe_entry->ipv4_dslite.dmac_hi), (foe_entry->ipv4_dslite.dmac_lo));
			foe_entry->ipv4_dslite.ts_id = 0x7F;
			foe_entry->ipv4_dslite.vlan1 = DP_MAP_E_DS;
			/* MAP-E download first section to PPE, chnl=0 for performance */
			foe_entry->ipv4_dslite.channel = 0;
		}
		else{
			FoeSetEntryMac(GetMacAddr(), (foe_entry->ipv4_hnapt.dmac_hi), (foe_entry->ipv4_hnapt.dmac_lo));
			foe_entry->ipv4_hnapt.ts_id = 0x7F;
			foe_entry->ipv4_hnapt.vlan1 = DP_MAP_E_US;
			/* MAP-E upload first section to fast for performance */
			set_info2_fast(foe_entry,IPV4_HNAPT,1);
		}
		if(PPE2_SUPPORTED && ppe2IsEnabled()){
			if((foe_index>=(PPE2_SRAM_HASH_IDX_BASE)&&(foe_index<DRAM_HASH_IDX_BASE))){
				PpeSetInfoBlk2(foe_entry, FP_PPE2, meterGrpId, acntGrpIndex, 1);
			}
			else{
				PpeSetInfoBlk2(foe_entry, FP_PPE, meterGrpId, acntGrpIndex, 1);
			}
		}
		else{
			PpeSetInfoBlk2(foe_entry, FP_PPE, meterGrpId, acntGrpIndex, 1);
        }
        PpeSetFqos(foe_entry, 1);
		PpeParseResult.vlan_layer = 1;
		foe_entry->bfib1.vpm = 1;
		return 0;
	}

    //#define SKB_FROM_WIFI_BIT (1<<15), defined in skb
   	if(skb->mark2 & SKB_FROM_WIFI_BIT && pinfo->udf != SKB_FROM_WIFI) //means skb from wifi ,set in hostadpt and hsgmii
		pinfo->fast = 1;
	if(macqueue_start_idx!=-1  && soft_ratelimit_enqueue_hook){
		if(skb->macEnque & 1<<6){
			if((magic == FOE_MAGIC_PTM) ||(magic == FOE_MAGIC_EPON) ||(magic == FOE_MAGIC_GPON)){
						//printk("\nhwnat TX, set ForcePortInfo!!");
				store_offload_info(skb,pinfo, magic);
				if (IS_IPV4_GRP(foe_entry)) {
					PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_OFFLOAD); 
					PpeSetInfoBlk2(foe_entry, FP_PDMA, meterGrpId, acntGrpIndex, 1);	
					foe_ext[foe_index].qos_que_index = skb->macEnque&0xf;
					return 0;
				}
				else if(IS_IPV6_GRP(foe_entry)){
					PpeSetPortInfo(foe_entry, pinfo, FOE_MAGIC_OFFLOAD); 
					PpeSetInfoBlk2(foe_entry, FP_PDMA, meterGrpId, acntGrpIndex, 1);
					foe_entry->ipv6_5t_route.channel = (skb->macEnque&0xf)<<4;
					return 0;
				}
			}	
		}
	}

	return 1;
}

void PpeSetFlowAcntInfo(struct FoeEntry * foe_entry, struct sk_buff * skb)
{
    /* from en751627, tsid is used for wan account */
    if (IS_IPV4_GRP(foe_entry))
        foe_entry->ipv4_hnapt.ts_id = FOE_FLOW_ACNT(skb);
    else if (IS_IPV6_GRP(foe_entry))
        foe_entry->ipv6_5t_route.ts_id = FOE_FLOW_ACNT(skb);
    else if (IS_L2_RRIDGE(foe_entry))
        foe_entry->l2_bridge32.ts_id = FOE_FLOW_ACNT(skb);

    return;
}

inline void ecnt_ppe_set_fqos_enable(struct FoeEntry * foe_entry, int enable)
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

	iblk2->fqos = enable;

	return;
}

inline void ecnt_ppe_check_tdma_wdam_fqos(struct FoeEntry * foe_entry)
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

	if((iblk2->fpidx == FP_WDMA)&&(iblk2->nbq != WDMA_RX0_OQ)&&(iblk2->nbq != WDMA_RX1_OQ))
		iblk2->fqos = 0;

	return;
}

int ecnt_ppe_set_force_port_info_end_hook(struct sk_buff * skb,
		    struct FoeEntry * foe_entry, struct port_info * pinfo, int magic,unsigned short meterGrpId,unsigned short acntGrpIndex)
{
	if ((TCSUPPORT_FWC_MCST_VAL && (ecnt_mc_api_vlan_IsOneToN(skb) > 0))
		|| (TCSUPPORT_XPON_HAL_API_MCST_VAL && (ECNT_HOOK_MC_API_IS_ONE_TO_N(skb) > 0)))
	{
		if((magic == FOE_MAGIC_GE)&&(!pinfo->eth_is_wan)&&((bind_dir == DOWNSTREAM_ONLY) || (bind_dir == BIDIRECTION)))
		{
			/* vlan 1:N send to QDMA LAN SW RX */
			vlan_isOneToN = 1;
			PpeSetInfoBlk2(foe_entry,FP_QDMA1_SW,meterGrpId,acntGrpIndex, 1);
			ecnt_ppe_set_fqos_enable(foe_entry, 0);
		}
	}
	
	if(TCSUPPORT_HWNAT_WAN_ACCOUNT_VAL && isEN751627)
		PpeSetFlowAcntInfo(foe_entry, skb);
        
	if(TCSUPPORT_XPON_HAL_API_EXT_VAL)
		PpeSetLanIpAcnt(foe_entry, skb, pinfo, magic);

	if(foe_ext[FOE_ENTRY_NUM(skb)].map_pingpong == 1 && (magic == FOE_MAGIC_GE)){
		PpeSetFqos(foe_entry, 0);
	}

	if(TDMA_WDAM_SAME_PORT)
		ecnt_ppe_check_tdma_wdam_fqos(foe_entry);

	if(skb->ppe_magic == FOE_MAGIC_APP_SHORTCUT)
	{
		ecnt_ppe_set_fqos_enable(foe_entry, 0);
	}
	
    return 0;
}

int ecnt_ppe_set_entry_bind_hook(struct sk_buff *skb,  struct FoeEntry *foe_entry,struct bf_info_blk1* p_blk1)
{
	if (TCSUPPORT_FWC_MCST_VAL || TCSUPPORT_XPON_HAL_API_MCST_VAL)
	{
		if (!vlan_isOneToN){
			if(PpeParseResult.is_L2_tunnel){
				p_blk1->vlan_layer = PpeParseResult.vlan_layer_intunnel;
			}else{
				p_blk1->vlan_layer = PpeParseResult.vlan_layer;
			}
		}
		/* restore to default value*/
		vlan_isOneToN = 0;
	}

	if (ppe_is_multicast_entry(foe_entry)&&(TCSUPPORT_XPON_IGMP_VAL ||TCSUPPORT_XPON_HAL_API_MCST_VAL)) 
	{
		p_blk1->ttl = 0;
		if(SUPPORT_FOE_FAST_CFG) 
		{
			if (IS_IPV4_GRP(foe_entry)) 
			{
				set_info2_fast(foe_entry, IPV4_HNAPT, 0);
			} 
			else if (IS_IPV6_GRP(foe_entry))
			{
				set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
			} 
			else 
			{
				set_info2_fast(foe_entry, L2_BRIDGE, 0);
			}
		} 
		else
		{
			SetInfo2Cache((*p_blk1), 0); 
		}
	}
	
	return 0;
}

void ecnt_ppe_rx_handler_hook(struct sk_buff* skb)
{
  	int i = 0;

	if(g_mul_high_priority)
	{
		unsigned char *dstAddr = NULL;
		if(NULL != eth_hdr(skb))
		{
	  		dstAddr = eth_hdr(skb)->h_dest;
			if((FOE_AI(skb) == UN_HIT ) && (dstAddr[0]&1)) 
			{
				Ppe_Vip_Tmp_Handler(skb);
			}
		}		
	}
	if(NULL != eth_hdr(skb) && skb->dev != NULL)
		ecnt_br_Mul_To_LAN_Config(skb);

	ecnt_multicast_hwnat_data_handler(skb);

	if(SUPPORT_FOE_SMAC_CFG) {
		for(i=0; i<ETH_ALEN; i++)
			skb->smac[i] = eth_hdr(skb)->h_source[i];
	}
	
	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL
		|| TCSUPPORT_CRJO_VAL){
		if ((FOE_AI(skb) != HIT_BIND_FORCE_TO_CPU)) {
		setPPeRxACNTGRP(skb);
		}
	}

	if (skb->dev != NULL && (TCSUPPORT_XPON_HAL_API_EXT_VAL == 1) && (FOE_AI(skb) == HIT_UNBIND_RATE_REACH) ) {
		hwnat_lan_wan_rx(FOE_ENTRY_NUM(skb),skb);
	}
	
	if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_EPON || FOE_MAGIC_TAG(skb) == FOE_MAGIC_GPON || FOE_MAGIC_TAG(skb) == FOE_MAGIC_AE_WAN)
		PpeBlack_ip_foe_handle(skb,1);	
	if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_GE)
		PpeBlack_ip_foe_handle(skb,0);	

	skb->ecnt_sk_buff.foe_index = FOE_ENTRY_NUM(skb);
    if(((FOE_MAGIC_TAG(skb) == FOE_MAGIC_GE && wan_conn_type == 3) 
	    || ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN || FOE_MAGIC_TAG(skb) == FOE_MAGIC_GE) && (wan_conn_type == 1 || wan_conn_type == 2)))
        && strncmp(skb->dev->name, wan_itf_name, strlen(skb->dev->name)) == 0)
    {
        skb->fe_resource_mark |= ETHER_WAN_MASK;
    }
	if(FOE_MAGIC_TAG(skb) == FOE_MAGIC_GE)
	{
		for (i = 0; i < 16; i++)
		{
			if (wan_virtualdev_ifname_info[i].valid == 1)
			{
				if (skb->dev != NULL && strncmp(skb->dev->name, wan_virtualdev_ifname_info[i].dev_name, strlen(skb->dev->name)) == 0)
				{				 
					skb->fe_resource_mark |= ETHER_WAN_MASK;				
					break;
				}
			}
		}			
	}
	else if((FOE_MAGIC_TAG(skb) == FOE_MAGIC_EPON) || (FOE_MAGIC_TAG(skb) == FOE_MAGIC_GPON) 
		||(FOE_MAGIC_TAG(skb)== FOE_MAGIC_ATM) || (FOE_MAGIC_TAG(skb)== FOE_MAGIC_PTM)|| (FOE_MAGIC_TAG(skb)== FOE_MAGIC_AE_WAN))
	{	 
		skb->fe_resource_mark |= ETHER_WAN_MASK;
	}

	foe_ext[FOE_ENTRY_NUM(skb)].natv6.natv6_src_wlan = 0;
	
	return;
}

int ecnt_ppe_set_port_info_magic_ge_hook(struct FoeEntry * foe_entry, struct port_info * pinfo,char type,char fast)
{
	if(TCSUPPORT_FWC_QOS_VAL || TCSUPPORT_DZS_VAL)
	{
		//set lan qdma channel and queue according to rule
		//			printk("PpeSetPortInfo:---get bind Lan Qdma Info channel = %d, queue = %d, pinfo->stag= %d\r\n", pinfo->channel, pinfo->txq, pinfo->stag);
		if(type == 1){
			/* to LAN traffic channel by destination port */
			foe_entry->ipv4_hnapt.channel = pinfo->channel;
			foe_entry->ipv4_hnapt.fqos = 1;
			foe_entry->ipv4_hnapt.qid = pinfo->txq;
			foe_entry->ipv4_hnapt.ts_id = pinfo->tsid;
			
			if (FOE_NBQ_CFG_SUPPORT){
				foe_entry->ipv4_hnapt.etype = pinfo->stag;
				set_info2_fast(foe_entry, IPV4_HNAPT, fast);
				set_info2_nbq(foe_entry, IPV4_HNAPT, pinfo->nbq);
			}
		}
		else if(type == 2){
			/* to LAN traffic channel by destination port */
			foe_entry->ipv6_5t_route.channel = pinfo->channel;
			foe_entry->ipv6_5t_route.fqos = 1;
			foe_entry->ipv6_5t_route.qid = pinfo->txq;
			foe_entry->ipv6_5t_route.ts_id = pinfo->tsid;
			
			if (FOE_NBQ_CFG_SUPPORT){
				foe_entry->ipv6_5t_route.etype = pinfo->stag;
				set_info2_fast(foe_entry, IPV6_5T_ROUTE, fast);
				set_info2_nbq(foe_entry, IPV6_5T_ROUTE, pinfo->nbq);
			}
		}
		else if(type == 3){
			/* to LAN traffic channel by destination port */
			foe_entry->l2_bridge32.channel = pinfo->channel;
			foe_entry->l2_bridge32.fqos = 1;
			foe_entry->l2_bridge32.qid = pinfo->txq;
			foe_entry->l2_bridge32.ts_id = pinfo->tsid;
			
			if (FOE_NBQ_CFG_SUPPORT){
				foe_entry->l2_bridge32.etype = pinfo->stag;
				set_info2_fast(foe_entry, L2_BRIDGE, fast);
				set_info2_nbq(foe_entry, L2_BRIDGE, pinfo->nbq);
			}
		}

		return 0;
	}

	return 1;
}

void ecnt_ppe_set_age_out_hook(void)
{	
	mm_segment_t orgfs;
	struct file	*srcf=NULL;
	char *src=NULL;	
	uint8_t get_buf[32];
	int8_t num=0; 
	int32_t tcp_val=0, udp_val=0, fin_val=0, ntu_val=0;
	
	if(TCSUPPORT_CDS_VAL) {
		orgfs = get_fs();
		set_fs(KERNEL_DS);
		src = "/tmp/etc/hwnat_timeout";
		srcf = filp_open(src, O_RDONLY, 0);
		if (IS_ERR(srcf))
		{
			printk("--> Error opening \n");
		}
		else
		{
			ecnt_kernel_fs_read(srcf, get_buf, 31, &srcf->f_pos);
			filp_close(srcf, NULL);
			num = sscanf(get_buf, "%d %d %d %d", &tcp_val, &udp_val, &fin_val, &ntu_val);
			if((num == 4) && (tcp_val != -1)) {
				/* set Delta time for aging out an bind UDP FOE entry */
				RegModifyBits(PPE_FOE_BND_AGE0, (uint32_t)udp_val, 0, 15);
					
				/* set Delta time for aging out an bind Non-TCP/UDP FOE entry */
				RegModifyBits(PPE_FOE_BND_AGE0, (uint32_t)ntu_val, 16, 15);

				/* set Delta time for aging out an bind TCP FIN FOE entry */
				RegModifyBits(PPE_FOE_BND_AGE1, (uint32_t)fin_val, 16, 15);

				/* set Delta time for aging out an bind TCP FOE entry */
				RegModifyBits(PPE_FOE_BND_AGE1, (uint32_t)tcp_val, 0, 15);
			}
		}
		set_fs(orgfs);
	}
}

void ecnt_ppe_set_eth_type_hook(void)
{
	if(TCSUPPORT_CDS_VAL ||TCSUPPORT_XPON_HAL_API_NG_VAL)
	{
		if(SUPPORT_ETYPE_CFG)
		{
			/*force ipv4/ipv6 to learn 3t or 5t,otherwise dscp remark can not work*/
			ppe_set_ether_type(3, PPE_DISABLE, PPE_NOT_PPPOE, 0x0800);
			ppe_set_ether_type(4, PPE_DISABLE, PPE_NOT_PPPOE, 0x86dd);
		}
		else
		{
			/*force ipv4/ipv6 to learn 3t or 5t,otherwise dscp remark can not work*/
	    		FE_API_SET_ETHER_TYEP(3, FE_DISABLE, FE_NOT_PPPOE, 0x0800);
	    		FE_API_SET_ETHER_TYEP(4, FE_DISABLE, FE_NOT_PPPOE, 0x86dd);
		}
	}
}

void ecnt_hwnat_adp_init(void)
{
    ecnt_hwnat_entry_init();
	ecnt_hwnat_ety_init();
	ecnt_hwnat_wifi_init();
	ecnt_hwnat_offload_init();
	ecnt_hwnat_gen_init();
	ecnt_hwnat_multicast_init();
	ecnt_hwnat_cust_init();
    ecnt_hwnat_npu_init();
	
	/* Register RX/TX hook point */
	ra_sw_nat_hook_tx = PpeTxHandler;
	ra_sw_nat_hook_rx = PpeRxHandler;	
}

void ecnt_hwnat_adp_deinit(void)
{
	/* Unregister RX/TX hook point */
	ra_sw_nat_hook_rx = NULL;
	ra_sw_nat_hook_tx = NULL;	

    ecnt_hwnat_entry_deinit();
	ecnt_hwnat_ety_deinit();
	ecnt_hwnat_offload_deinit();
	ecnt_hwnat_gen_deinit();
	ecnt_hwnat_multicast_deinit();	
	ecnt_hwnat_cust_deinit();
    ecnt_hwnat_npu_deinit();
	ecnt_hwnat_wifi_deinit();
}

