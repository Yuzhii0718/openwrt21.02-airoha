/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/libcompileoption.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/foe_hook.h>
#include <asm/tc3162/tc3162.h>
#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include "fe_account_meter.h"
#include "fe.h"
#include "fe_reg_en7512.h"
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/if_pppox.h>
#include <linux/if_ether.h>
#include <linux/soft_dma.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define AcntManage_T QDMA_AcntManage_T 

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define		QOS_SW_CAR				2
#define     FE_METER_FILTER_MARK    0xf0    /*original mark in skb->mark*/

#define 	IP_VERSION_V4			0
#define 	IP_VERSION_V6			1
#define 	ENABLE 					1
#define 	DISABLE 				0
/*For CDS BC/MC ratelimit start*/
#define     BC_FP_OFFSET            8
#define     BC_FP_MASK              (0x1f<<BC_FP_OFFSET)
#define     BC_FQ_OFFSET            16
#define     BC_FQ_MASK              (0x1f<<BC_FQ_OFFSET)

#define     MC_FP_OFFSET            4
#define     MC_FP_MASK              (0xf<<MC_FP_OFFSET)
#define     MC_FQ_OFFSET            8
#define     MC_FQ_MASK              (0xf<<MC_FQ_OFFSET)

#define     WAN_PORT                5
#define     LAN_PORT                0

#define     DEFAULT_PORT            4
#define     TOTAL                   1
#define     SEPARATE                2

/*For CDS BC/MC ratelimit end*/

#define GET_METER_IDX(index)    (index&0xff)
#define GET_METER_GRP(index)    ((index>>8)&0x3)

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
 
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern struct devBandwidthList_s *gHwBandwidthList;
extern struct devBandwidthList_s *gDevBandwidthList;
extern Wan_itf_name wan_itf_info[WAN_ITF_ACNT_NUM];
extern struct  Fe_Ratelimit_IP_Info ip_info_up[IP_MAX_NUM];
extern struct  Fe_Ratelimit_IP_Info ip_info_dw[IP_MAX_NUM];
extern struct  Fe_Ratelimit_Itf_Info itf_info_up[IF_MAX_NUM];
extern struct  Fe_Ratelimit_Itf_Info itf_info_dw[IF_MAX_NUM];
extern int aewan_lan_mode;
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
 
int get_meter_idx_from_iprange_v4(__be32 ipaddr, int dir)/*    DOWN_STREAM = 0, UP_STREAM=1*/
{
    int idx = 0 ;
    __be32 ip4_l = 0 ;
    __be32 ip4_h = 0 ;
    struct Fe_Ratelimit_IP_Info *RateLimitIPInfo = NULL;

    if(DOWN_STREAM == dir)
        RateLimitIPInfo = ip_info_dw;
    else
        RateLimitIPInfo = ip_info_up;

    for(idx = 0 ; idx < IP_MAX_NUM ; idx++)
    {
        /*active && ipv4*/
        if( (ENABLE == RateLimitIPInfo[idx].enable) && (IP_VERSION_V4 == RateLimitIPInfo[idx].ip_version) )
        {
            ip4_l = *(unsigned int *)RateLimitIPInfo[idx].v4_pair_l ;
            ip4_h = *(unsigned int *)RateLimitIPInfo[idx].v4_pair_h ;

            if( ( ntohl(ipaddr) <= ntohl(ip4_h)) && (ntohl(ipaddr) >= ntohl(ip4_l)) )
            {
                return (idx + 1);
            }
        }
    }

    return -1 ;
}
/******************************************************************************
 Descriptor:    It's used to get idx from ipv6 rule
 Input Args:    ip address , dir 
 Ret Value: 1: ignore -1 :error 0  success
******************************************************************************/
int get_meter_idx_from_iprange_v6(__u8 *ip6addr, int dir)
{
    int idx = 0 ;

    struct Fe_Ratelimit_IP_Info *RateLimitIPInfo = NULL;

    if(DOWN_STREAM == dir)
        RateLimitIPInfo = ip_info_dw;
    else
        RateLimitIPInfo = ip_info_up;

    for(idx = 0 ; idx < IP_MAX_NUM ; idx++)
    {
        /*active && ipv6*/
        if( (ENABLE == RateLimitIPInfo[idx].enable) && (IP_VERSION_V6 == RateLimitIPInfo[idx].ip_version) )
        {
            if ( (0 <= memcmp(ip6addr, RateLimitIPInfo[idx].v6_pair_l, 16)) 
                && (0 >= memcmp(ip6addr, RateLimitIPInfo[idx].v6_pair_h, 16)) )
            {
                return (idx + 1);
            }
        }
    }

    return -1 ;
}
/******************************************************************************
 Descriptor:    It's used to get meter idx from ip address 
 Input Args:    ip address , dir and dev
 Ret Value: 1: ignore -1 :error 0  success
******************************************************************************/
int fe_get_meter_idx_from_ip(struct sk_buff *skb, int dir, char *pname)
{
    ushort etherType = 0;
    ushort ppp_proto = 0;
    struct iphdr *iph = NULL;
    struct ipv6hdr *ip6h = NULL;
    struct pppoe_hdr *ppph = NULL;
    unsigned char *cp = NULL;
    __be32 check_addr = 0;
    __u8 check_u6_addr8[16] = {0};
    int iptype = -1; /*0:IPv4  1:IPv6*/
    int ipidx = 0 ; /*0~7*/

#define PPP_IP		0x21	/* Internet Protocol */
#define PPP_IPV6	0x57	/* Internet Protocol Version 6 */
#define IPTYPE_IPV4	0
#define IPTYPE_IPV6	1

    if (skb == NULL || pname == NULL)
    {
        return -1 ;
    }

    if ( ('e' != pname[0])&& ('r' != pname[0]) )
        return 0; /*ignore*/

    cp = skb_mac_header(skb) ;
    /* skip DA and SA mac address */
    cp += 12 ;
    /* get ether type */
    etherType = *(ushort *) cp ;
    /* skip ether type */
    cp += 2 ;
    
    /*parse if vlan exists*/
    if( (etherType == htons(ETH_P_8021Q)) || (etherType == htons(ETH_P_QinQ_88a8)) || (etherType == htons(ETH_P_QinQ_9100)) ) 
    {
        /*skip 802.1q tag field*/
        cp += 2;
        /*re-parse ether type*/
        etherType = *(ushort *) cp;
        /* skip ether type */
        cp += 2;
    }

    /*parse if second vlan exists*/
    if( (etherType == htons(ETH_P_8021Q)) || (etherType == htons(ETH_P_QinQ_88a8)) || (etherType == htons(ETH_P_QinQ_9100)) ) 
    {
        /*skip 802.1q tag field*/
        cp += 2;
        /*re-parse ether type*/
        etherType = *(ushort *) cp;
        /* skip ether type */
        cp += 2;
    }

    if( etherType == htons(ETH_P_IP) )      /*IPv4*/
    {
        iph = (struct iphdr*)(cp) ;
        if(dir == 1)
            check_addr = iph->saddr ;
        else
            check_addr = iph->daddr ;
        iptype = IPTYPE_IPV4 ;
    }
    else if( etherType == htons(ETH_P_IPV6) )   /*IPv6*/
    {
        ip6h = (struct ipv6hdr* )(cp);
        if(dir == 1)
            memcpy(check_u6_addr8, ip6h->saddr.s6_addr,sizeof(check_u6_addr8));
        else
            memcpy(check_u6_addr8, ip6h->daddr.s6_addr,sizeof(check_u6_addr8));
        iptype = IPTYPE_IPV6 ;
    }
    else if( etherType == htons(ETH_P_PPP_SES) )        /*PPPoe session*/
    {
        ppph = (struct pppoe_hdr*)(cp) ;
        cp = (unsigned char *)(ppph + 1) ;      /*skip pppoe head*/
        ppp_proto = *(ushort *)cp;
        cp += 2;                        /*skip ppp protocol*/

        if( ppp_proto == htons(PPP_IP) )
        {
            iph = (struct iphdr* )(cp);
            if(dir == 1)
                check_addr = iph->saddr ;
            else
                check_addr = iph->daddr ;
            iptype = IPTYPE_IPV4 ;
        }
        else if( ppp_proto == htons(PPP_IPV6) )
        {
            ip6h = (struct ipv6hdr* )(cp);
            if(dir == 1)
                memcpy(check_u6_addr8, ip6h->saddr.s6_addr,sizeof(check_u6_addr8));
            else
                memcpy(check_u6_addr8, ip6h->daddr.s6_addr,sizeof(check_u6_addr8));
            iptype = IPTYPE_IPV6 ;
        }
        else
        {
            return -1 ;
        }
    }
    else
    {
        return -1 ;
    }

    /*get meter index by ip*/
    if ( IPTYPE_IPV4 == iptype )
        ipidx = get_meter_idx_from_iprange_v4(check_addr, dir);
    else 
        ipidx = get_meter_idx_from_iprange_v6(check_u6_addr8, dir);

    return ipidx ;/*1-8*/
}

int fe_api_get_meter_idx(struct ecnt_fe_data *fe_data)
{
    unchar meter_id = 0x7F;
	unchar meter_id_default = 0x7F;
    uint mainType_val = 0;
    int qos_flag_num = -1;
    int idx = 0, meter_enable = 0;
    QDMA_MeterType_T meter_group_id = 3;
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    FE_Dir_t dir = 2;
    
    struct sk_buff *skb = fe_data->api_data.meter_cfg.skb ;
    dir = fe_data->api_data.meter_cfg.dir;
    meter_group_id = fe_data->api_data.meter_cfg.idx;
    
    if((NULL == skb) || (NULL == feResManage))
        return -1;

	if(meter_group_id < METER0_TYPE || meter_group_id > METER2_TYPE){
		printk("meter group is error!\n");
		return -1;
	}
		
    if (dir == UP_STREAM){
        mainType_val = ECNT_QDMA_WAN;
        mode = feResManage->up_stream_mode[meter_group_id];
    }else{
        mainType_val = ECNT_QDMA_LAN;
        mode = feResManage->dw_stream_mode[meter_group_id];
    }
    meter_id_default = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
	meter_id = meter_id_default;

	fe_get_meter_index_by_type(skb, meter_group_id, &meter_enable, &idx);

    switch (mode) {
        case CAR_QOS_MODE:
			if((meter_enable != 0) && ((idx >= 0) && (idx < QOS_CAR_NUM))){
                QDMA_API_GET_QOS_FLAG(mainType_val, &qos_flag_num);
                if(qos_flag_num == QOS_SW_CAR){
                    if(dir == UP_STREAM)
                        meter_id = feResManage->CAR_QOS_QUEUE_UP[idx]; 
                    else
                        meter_id = feResManage->CAR_QOS_QUEUE_DOWN[idx];
                }
			}
            break;
        case IP_BIND_MODE:
			if((meter_enable != 0) && ((idx >= 0) && (idx < IP_MAX_NUM))){
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_IP_UP[idx]; 
                else
                    meter_id = feResManage->Meter_IP_DOWN[idx]; 
            }
            break;
        case IF_BIND_MODE:
			if((meter_enable != 0) && ((idx >= 0) && (idx < IF_MAX_NUM))){
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_IF_UP[idx]; 
                else
                    meter_id = feResManage->Meter_IF_DOWN[idx]; 
            }
            break;
        case OLT_CTL_MODE:
            if(dir == UP_STREAM){
                if(skb->pon_mark & QOS_TSE_MARK){
                    idx = skb->pon_mark & QOS_TSID_MARK;
                    if(idx < OLT_CTL_NUM)
                        meter_id = feResManage->Meter_OLT_UP[idx]; 
                }
            }
            else{
                if(skb->pon_mark & DS_TRTCM_ENABLE_MARK){
                    idx = skb->pon_mark & QOS_TSID_MARK;
                    if(idx < OLT_CTL_NUM)
                        meter_id = feResManage->Meter_OLT_DOWN[idx]; 
                }
            }
            break;
        case TRAFFIC_QOS_MODE:
			if((meter_enable != 0) && ((idx >= 0) && (idx < TRAFFIC_QOS_NUM))){
                if(dir == UP_STREAM)
                    meter_id = feResManage->TRAFFIC_QOS_UP[idx]; 
                else
                    meter_id = feResManage->TRAFFIC_QOS_DW[idx]; 
			}
            break;

        case DEV_MAC_MODE:
            if((meter_enable != 0) && ((idx >= 0) && (idx < UNDER_DEV_MAX_NUM))){
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_DEV_FLOW_UP[idx]; 
                else
                    meter_id = feResManage->Meter_DEV_FLOW_DW[idx]; 
            }
            break;
        case FORCEBW_WRR_MODE:
			if((meter_enable != 0) && ((idx >= 0) && (idx < FORCEBW_WRR_NUM))){
                if(dir == UP_STREAM)
                    meter_id = feResManage->FORCEBW_WRR_QUEUE_UP[idx]; 
            }        
            break;

        case WAN_MODE:
            if((meter_enable != 0) && ((idx >= 0) && (idx < WAN_ITF_ACNT_NUM))){
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_WAN_FLOW_UP[idx]; 
                else
                    meter_id = feResManage->Meter_WAN_FLOW_DW[idx]; 

                /* for clouvr WAN, IPv4 flows */
                if(wan_itf_info[idx].is_vr_wan)
                {
                    if(dir == UP_STREAM)
                        meter_id = OFFLOAD_RX_RING_2;
                    else
                        meter_id = OFFLOAD_RX_RING;
                }
            }
            break;
        default:
            meter_id = meter_id_default;
            break;
    }
  
    fe_data->api_data.meter_cfg.meter_id = meter_id;
	FE_DEBUG(FE_DBG_MSG, "get meter_group_%d meter_id %d and fe_resource_mark is %x", meter_group_id, meter_id, skb->fe_resource_mark);
    
    return 0;
}

int fe_api_get_acnt_idx(QDMA_AcntType_T acnt_type, Meter_cfg_t *meter_data)
{
    unchar acnt_id = ACNT2_DEFAULT;
	FE_ACNT_Mode_t acnt_mode = DEFAULT_NULL_MODE;
	FE_Dir_t dir = 2;
    int idx = 0;

	idx = meter_data->idx;
	dir = meter_data->dir;
	acnt_mode = meter_data->acnt_mode;

	if(acnt_mode == TRAFFIC_FLOW_MODE)
	{
		if((idx >= 0) && (idx < TRAFFIC_FLOW_MAX_NUM)){
			if(dir == UP_STREAM){
				acnt_id = feResManage->ACNT_TRAFFIC_FLOW_UP[acnt_type][idx]; 
			}else{
				acnt_id = feResManage->ACNT_TRAFFIC_FLOW_DOWN[acnt_type][idx]; 
			}
		}
	}else if(acnt_mode == LLID_MODE){
		if((idx >= 0) && (idx < LLID_MAX_NUM)){
			if(dir == UP_STREAM){
				acnt_id = feResManage->ACNT_LLID_UP[acnt_type][idx]; 
			}else{
				acnt_id = feResManage->ACNT_LLID_DOWN[acnt_type][idx]; 
			}
		}
	}else if(acnt_mode == DEV_MODE){
		if((idx >= 0) && (idx < UNDER_DEV_MAX_NUM)){
			if(dir == UP_STREAM){
				acnt_id = feResManage->ACNT_DEV_FLOW_UP[acnt_type][idx]; 
			}else{
				acnt_id = feResManage->ACNT_DEV_FLOW_DOWN[acnt_type][idx];
			}
		}
	}else if(acnt_mode == WAN_ITF_MODE){
		if((idx >= 0) && (idx < WAN_ITF_ACNT_NUM)){
			if(dir == UP_STREAM){
				acnt_id = feResManage->ACNT_WAN_ITF_UP[acnt_type][idx]; 
			}else{
				acnt_id = feResManage->ACNT_WAN_ITF_DOWN[acnt_type][idx]; 
			}
		}
	}
	
	return acnt_id;
}
int fe_api_get_wan_itf_index(struct ecnt_fe_data *fe_data)
{
    int i = 0;
    char wan_itf_name[IFNAMSIZ] = {0};
    unchar itf_idx = 0xFF;
	
    memcpy(wan_itf_name, fe_data->api_data.meter_cfg.itf_name, IFNAMSIZ);
	
    for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
        if(!wan_itf_info[i].valid)
            continue;
        if(wan_itf_info[i].wan_dev && strcmp(wan_itf_info[i].wan_dev->name, wan_itf_name) == 0){
            itf_idx = i;
            break;
        }
    }
    fe_data->api_data.meter_cfg.idx = itf_idx;
    FE_DEBUG(FE_DBG_MSG, "find wan %s in wan_itf_info list[%d]", wan_itf_name, itf_idx);

    return 0;
}

int fe_get_index_offset(int mode){

	if(!aewan_lan_mode)
		return 0;
	
	if(TRAFFIC_QOS_MODE==mode)
	   return TRAFFIC_FLOW_MAX_NUM/2;
	else if(DEV_MAC_MODE==mode)
		return MAX_DEV_BANDWIDTH_NUM/2;
	else if(WAN_MODE==mode)
		return WAN_ITF_ACNT_NUM/2;
	else 
		return 0;
}


void set_qdma_meter_ratelimit(uint mainType_val,unchar meter_id,uint ratelimit,FE_TickSel_t tick_sel,FE_Packet_Mode_t pkt_mode,QDMA_MeterType_T meter_type ,
	FE_Enable_t en_mode){

	
    GENERAL_TrtcmCfg_T rxTrtcmCfg;
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    GENERAL_TrtcmCbsPbsCfg_T rxTrtcmCbsPbsCfg;
    GENERAL_TrtcmCbsPbsSet_T rxTrtcmCbsPbsSet;
	
    memset(&rxTrtcmCfg, 0, sizeof(GENERAL_TrtcmCfg_T)) ;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ;
    memset(&rxTrtcmCbsPbsCfg, 0, sizeof(GENERAL_TrtcmCbsPbsCfg_T)) ;
    memset(&rxTrtcmCbsPbsSet, 0, sizeof(GENERAL_TrtcmCbsPbsSet_T)) ;
	
    rxTrtcmCfg.trtcmModule = INGRESS_TRTCM ;
    QDMA_API_GET_GENERAL_TRTCM_INIT(mainType_val, &rxTrtcmCfg);
	
	if(((rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) && (meter_id == QDMA_INGRESS_INDEX_MAX))
		|| ((rxTrtcmCfg.trtcmMode == TRTCM_MODE) && (meter_id == (QDMA_INGRESS_INDEX_MAX>>1)))){
		FE_DEBUG(FE_DBG_MSG, "rxTrtcmCfg.trtcmMode=%d ratelimit=%d\n", rxTrtcmCfg.trtcmMode, meter_id);
		return 0;
	}
        
    if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
        rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;
        rxRateLimitCfg.Index = (((meter_type & 0x3) << 8) | meter_id);

        if(en_mode == FE_ENABLE)
            rxRateLimitCfg.MeterEn = GENERAL_METER_ENABLE;
        else
            rxRateLimitCfg.MeterEn = GENERAL_METER_DISABLE;

        if(pkt_mode == METER_BYTE_MODE)
            rxRateLimitCfg.PktMode = TRTCM_BYTE_MODE;
        else
            rxRateLimitCfg.PktMode = TRTCM_PACKET_MODE;

        if(tick_sel == FAST_TICK)
            rxRateLimitCfg.TickSel = TRTCM_FAST_TICK;
        else
            rxRateLimitCfg.TickSel = TRTCM_SLOW_TICK;
        QDMA_API_SET_GENERAL_RATELIMIT_CFG(mainType_val, &rxRateLimitCfg);
    } else {
        rxTrtcmCbsPbsCfg.trtcmModule = INGRESS_TRTCM ;
        rxTrtcmCbsPbsCfg.Index = (((meter_type & 0x3) << 8) | meter_id);
        
        if(en_mode == FE_ENABLE) {
            rxTrtcmCbsPbsCfg.CbsMeterEn = GENERAL_METER_ENABLE;
            rxTrtcmCbsPbsCfg.PbsMeterEn = GENERAL_METER_ENABLE;
        } else {
            rxTrtcmCbsPbsCfg.CbsMeterEn = GENERAL_METER_DISABLE;
            rxTrtcmCbsPbsCfg.PbsMeterEn = GENERAL_METER_DISABLE;
        }

        if(pkt_mode == METER_BYTE_MODE) {
            rxTrtcmCbsPbsCfg.CbsPktMode = TRTCM_BYTE_MODE;
            rxTrtcmCbsPbsCfg.PbsPktMode = TRTCM_BYTE_MODE;
        } else {
            rxTrtcmCbsPbsCfg.CbsPktMode = TRTCM_PACKET_MODE;
            rxTrtcmCbsPbsCfg.PbsPktMode = TRTCM_PACKET_MODE;
        }

        if(tick_sel == FAST_TICK) {
            rxTrtcmCbsPbsCfg.CbsTickSel = TRTCM_FAST_TICK;
            rxTrtcmCbsPbsCfg.PbsTickSel = TRTCM_FAST_TICK;
        } else {
            rxTrtcmCbsPbsCfg.CbsTickSel = TRTCM_SLOW_TICK;
            rxTrtcmCbsPbsCfg.PbsTickSel = TRTCM_SLOW_TICK;
        }
        QDMA_API_SET_GENERAL_TRTCM_CFG(mainType_val, &rxTrtcmCbsPbsCfg);
    }

    /*call qdma ratelimit api to do meter ratelimit */
    if (en_mode == FE_ENABLE){
        if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
            rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
            rxRateLimitSet.Index =  (((meter_type & 0x3) << 8) | meter_id);			
			
            QDMA_API_GET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);   

            rxRateLimitSet.RateLimitValue = ratelimit;
            QDMA_API_SET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);
        } else {
            rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
            rxTrtcmCbsPbsSet.Index =  (((meter_type & 0x3) << 8) | meter_id);
			
            QDMA_API_GET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);   

            rxTrtcmCbsPbsSet.CbsParamValue = ratelimit;
            rxTrtcmCbsPbsSet.PbsParamValue = ratelimit;
            rxTrtcmCbsPbsSet.CbsBucketSize = rxTrtcmCbsPbsSet.CbsParamValue/1000;
            rxTrtcmCbsPbsSet.PbsBucketSize = rxTrtcmCbsPbsSet.PbsParamValue/1000;
            QDMA_API_SET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);
        }
    }


}


int fe_api_set_meter_ratelimit(struct ecnt_fe_data *fe_data)
{
    uint mainType_val = 0, ratelimit = 0, bucket_size = 0;
	uint tmp_mainType_val=0;
    unchar meter_id = 0x7f,meter_id_2=0x7f;
    unchar idx = 0;
	int offset=0;
    QDMA_MeterType_T meter_type = 3;
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    FE_Dir_t dir = 2;
    FE_TickSel_t tick_sel;
    FE_Packet_Mode_t pkt_mode;
    FE_Enable_t en_mode;
    
    ratelimit = fe_data->api_data.meter_cfg.rate;/*ratelimit value.unit KBps*/
    en_mode = fe_data->api_data.meter_cfg.enable;/*0 disable, or 1enable meter ratelimit*/
    pkt_mode = fe_data->api_data.meter_cfg.pkt_mode;
    tick_sel = fe_data->api_data.meter_cfg.tick_sel;
    idx = fe_data->api_data.meter_cfg.idx;/*idx */
    dir = fe_data->api_data.meter_cfg.dir;/*0:down stream 1:up stream*/
    mode = fe_data->api_data.meter_cfg.mode;/*0: car_qos_mode ;1:IP bind ratelimit 2:If bind ratelimit */
    bucket_size = fe_data->api_data.meter_cfg.bucket_size;
	if(aewan_lan_mode){
		offset=fe_get_index_offset(mode);
	}
    if(feResManage == NULL)
        return -1;

    switch(mode)
    {
        case CAR_QOS_MODE:
            if(idx < QOS_CAR_NUM){
                if (dir == UP_STREAM)
                    meter_id = feResManage->CAR_QOS_QUEUE_UP[idx];
                else
                    meter_id = feResManage->CAR_QOS_QUEUE_DOWN[idx]; 
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", QOS_CAR_NUM-1, idx);
                return -1;
            }
            break;
        case IP_BIND_MODE:
            if(idx < IP_MAX_NUM){
                if (dir == UP_STREAM)
                    meter_id = feResManage->Meter_IP_UP[idx]; 
                else
                    meter_id = feResManage->Meter_IP_DOWN[idx]; 
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", IP_MAX_NUM-1, idx);
                return -1;
            }
            break;
        case IF_BIND_MODE:
            if(idx < IF_MAX_NUM){
                if (dir == UP_STREAM)
                    meter_id = feResManage->Meter_IF_UP[idx]; 
                else
                    meter_id = feResManage->Meter_IF_DOWN[idx]; 
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", IF_MAX_NUM-1, idx);
                return -1;
            }
            break;
        case OLT_CTL_MODE:
            if(idx < OLT_CTL_NUM){
                if (dir == UP_STREAM)
                    meter_id = feResManage->Meter_OLT_UP[idx]; 
                else
                    meter_id = feResManage->Meter_OLT_DOWN[idx]; 
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", OLT_CTL_NUM-1, idx);
                return -1;
            }
            break;
        case TRAFFIC_QOS_MODE:
            if(idx < TRAFFIC_QOS_NUM){
                if (dir == UP_STREAM){
                    meter_id = feResManage->TRAFFIC_QOS_UP[idx] ;
					meter_id_2 =  feResManage->TRAFFIC_QOS_UP[idx + offset] ;
                }
                else
                    meter_id = feResManage->TRAFFIC_QOS_DW[idx]; 
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", TRAFFIC_QOS_NUM-1, idx);
                return -1;
            }
            break;
        case DEV_MAC_MODE:
            if(idx < UNDER_DEV_MAX_NUM){
                if (dir == UP_STREAM){
                    meter_id = feResManage->Meter_DEV_FLOW_UP[idx];
                    meter_id_2 = feResManage->Meter_DEV_FLOW_UP[idx + offset]; 
                }else{
                    meter_id = feResManage->Meter_DEV_FLOW_DW[idx]; 
                }
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", UNDER_DEV_MAX_NUM-1, idx);
                return -1;
            }
            break;
        case FORCEBW_WRR_MODE:
            if(idx < FORCEBW_WRR_NUM){
                if (dir == UP_STREAM)
                    meter_id = feResManage->FORCEBW_WRR_QUEUE_UP[idx];
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", FORCEBW_WRR_NUM-1, idx);
                return -1;
            }
            break;
        case WAN_MODE:		
            if(idx < WAN_ITF_ACNT_NUM){	
                if (dir == UP_STREAM){
                    meter_id = feResManage->Meter_WAN_FLOW_UP[idx]  ;
                    meter_id_2 = feResManage->Meter_WAN_FLOW_UP[idx+ offset]  ; 
                }else{
                    meter_id = feResManage->Meter_WAN_FLOW_DW[idx]; 
                }
            }else{
                printk("wan %s not exist\n", fe_data->api_data.meter_cfg.itf_name);
                return -1;
            }
            break;
        default:
            printk("mode value error, correct range: 0-3\n");
            return -1;
    }

    fe_get_meter_group(&meter_type, dir, mode);
    if(meter_type > METER2_TYPE)
    {
    	FE_DEBUG(FE_DBG_MSG, "Error type %d, Can not find mode %d in meter group!\n", meter_type, mode);
        return 0;
    }

    if((meter_type == METER0_TYPE) && meter_id < RX_RING_NUM)
    {
        printk("Error id %d, Meter group-0 id 0~%d already use!\n", meter_id, RX_RING_NUM-1);
        return 0;
    }

	FE_DEBUG(FE_DBG_MSG, "#dir=%d mode=%d meter_type=%d meter_id=%d ratelimit=%d en_mode=%d", dir, mode, meter_type, meter_id, ratelimit, en_mode);

set_ratelimit:
	if(!aewan_lan_mode){
	    if (dir == UP_STREAM)
	        mainType_val = ECNT_QDMA_WAN;
	    else
	        mainType_val = ECNT_QDMA_LAN;
	}else if(aewan_lan_mode){
	    if (dir == UP_STREAM){
	        mainType_val = ECNT_QDMA_LAN;
		}
	    else{
	        mainType_val = ECNT_QDMA_WAN;
			tmp_mainType_val = ECNT_QDMA_LAN;
		}	
	}
	if(!aewan_lan_mode)	{
		set_qdma_meter_ratelimit(mainType_val, meter_id,ratelimit,tick_sel, pkt_mode, meter_type ,
			 en_mode );
	}else if(aewan_lan_mode){
	    if (dir == UP_STREAM){
			set_qdma_meter_ratelimit(mainType_val, meter_id_2,ratelimit,tick_sel, pkt_mode, meter_type ,
				 en_mode );
		}else{
			set_qdma_meter_ratelimit(mainType_val, meter_id,ratelimit,tick_sel, pkt_mode, meter_type ,
				 en_mode );
			set_qdma_meter_ratelimit(tmp_mainType_val, meter_id,ratelimit,tick_sel, pkt_mode, meter_type ,
				 en_mode );
		}
	}
    PPE_API_CLEAN_FOE_TABLE();
    return 0;
}

/******************************************************************************
 Descriptor:    It's used to get meter ratalimit value different bind mode (set meter ratelimit according meter id)
 Input Args:    args need in GENERAL_TrtcmRatelimitCfg_T and GENERAL_TrtcmRatelimitSet_T
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_get_meter_ratelimit(struct ecnt_fe_data *fe_data)
{
    uint mainType_val = 0;
    unchar meter_id = 0x7f;
    unchar idx=0;
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    FE_Dir_t dir = -1;
    int offset=0;
    GENERAL_TrtcmCfg_T rxTrtcmCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    GENERAL_TrtcmCbsPbsSet_T rxTrtcmCbsPbsSet;
    memset(&rxTrtcmCfg, 0, sizeof(GENERAL_TrtcmCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ;
    memset(&rxTrtcmCbsPbsSet, 0, sizeof(GENERAL_TrtcmCbsPbsSet_T)) ;

    idx = fe_data->api_data.meter_cfg.idx;/*idx */
    dir = fe_data->api_data.meter_cfg.dir;/*0:down stream 1:up stream*/
    mode = fe_data->api_data.meter_cfg.mode;/*0: car_qos_mode ;1:IP bind ratelimit 2:If bind ratelimit */
    
    if(feResManage == NULL)
        return -1;
	if(aewan_lan_mode){
		offset=fe_get_index_offset(mode);
	}

    if(mode == CAR_QOS_MODE){
        if(idx < QOS_CAR_NUM){
            if (dir == UP_STREAM)
                meter_id = feResManage->CAR_QOS_QUEUE_UP[idx];
            else
                meter_id = feResManage->CAR_QOS_QUEUE_DOWN[idx]; 
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", IP_MAX_NUM, idx);
            return -1;
        }
    }else if(mode == IP_BIND_MODE){
        if(idx < IP_MAX_NUM){
            if (dir == UP_STREAM)
                meter_id = feResManage->Meter_IP_UP[idx]; 
            else
                meter_id = feResManage->Meter_IP_DOWN[idx]; 
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", IP_MAX_NUM, idx);
            return -1;
        }
    }else if(mode == IF_BIND_MODE){
        if(idx < IF_MAX_NUM){
            if (dir == UP_STREAM)
                meter_id = feResManage->Meter_IF_UP[idx]; 
            else
                meter_id = feResManage->Meter_IF_DOWN[idx]; 
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", IF_MAX_NUM, idx);
            return -1;
        }
    }
	else if(mode == OLT_CTL_MODE){
		if(idx < OLT_CTL_NUM){
			if (dir == UP_STREAM)
				meter_id = feResManage->Meter_OLT_UP[idx];
			else
				meter_id = feResManage->Meter_OLT_DOWN[idx];
		}else{
			printk("idx value error, correct range: 0-%d\n ,real idx :%d", OLT_CTL_NUM-1, idx);
			return -1;
		}
	}
	else if(mode == TRAFFIC_QOS_MODE){
        if(idx < TRAFFIC_QOS_NUM){
            if (dir == UP_STREAM)
                meter_id = feResManage->TRAFFIC_QOS_UP[idx+offset]; 
            else
                meter_id = feResManage->TRAFFIC_QOS_DW[idx]; 
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", TRAFFIC_QOS_NUM-1, idx);
            return -1;
        }
    }else if(DEV_MAC_MODE == mode){
        if(idx < UNDER_DEV_MAX_NUM){
            if (dir == UP_STREAM)
                meter_id = feResManage->Meter_DEV_FLOW_UP[idx+offset]; 
            else
                meter_id = feResManage->Meter_DEV_FLOW_DW[idx]; 
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", UNDER_DEV_MAX_NUM-1, idx);
            return -1;
        }
    }else if(FORCEBW_WRR_MODE == mode){
        if(idx < FORCEBW_WRR_NUM){
            if (dir == UP_STREAM)
                meter_id = feResManage->FORCEBW_WRR_QUEUE_UP[idx];
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", FORCEBW_WRR_NUM, idx);
            return -1;
        }
        
    }else{
        printk("mode value error, correct range: 0-5\n");
        return -1;
    }

	if(!aewan_lan_mode){
	    if (dir == UP_STREAM)
	        mainType_val = ECNT_QDMA_WAN;
	    else
	        mainType_val = ECNT_QDMA_LAN;
	}else if(aewan_lan_mode){
	    if (dir == UP_STREAM){
	        mainType_val = ECNT_QDMA_LAN;
		}
	    else{
	        mainType_val = ECNT_QDMA_WAN;
		}	
	}
 
    rxTrtcmCfg.trtcmModule = INGRESS_TRTCM ;
    QDMA_API_GET_GENERAL_TRTCM_INIT(mainType_val, &rxTrtcmCfg);
    
    if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
        rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
        rxRateLimitSet.Index = meter_id;
        QDMA_API_GET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);
        fe_data->api_data.meter_cfg.rate = rxRateLimitSet.RateLimitValue;
    } else {
        rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
        rxTrtcmCbsPbsSet.Index = meter_id;
        QDMA_API_GET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);
        fe_data->api_data.meter_cfg.rate = rxTrtcmCbsPbsSet.CbsParamValue;
    }
    return 0;
}

/******************************************************************************
 Descriptor:    It's used to get meter if is enable or disable
 Input Args:    args need in GENERAL_TrtcmRatelimitCfg_T and GENERAL_TrtcmRatelimitSet_T
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_get_meter_enable(struct ecnt_fe_data *fe_data)
{
    uint mainType_val=0;
    unchar meter_id = 0x7f;
    unchar idx=0;
    FE_Meter_Mode_t mode;
    FE_Dir_t dir;

    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;

    idx = fe_data->api_data.meter_cfg.idx;
    dir = fe_data->api_data.meter_cfg.dir;

	/* cloud VR WAN, should not set fqos bit */
	if((dir == UP_STREAM && idx == OFFLOAD_RX_RING_2)
		|| (dir == DOWN_STREAM && idx == OFFLOAD_RX_RING))
	{
		fe_data->api_data.meter_cfg.enable = 0;

		return 0;
	}
    
    if (dir == UP_STREAM)
        mainType_val = ECNT_QDMA_WAN;
    else
        mainType_val = ECNT_QDMA_LAN;
 
    rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;
    rxRateLimitCfg.Index = idx;

    QDMA_API_GET_GENERAL_RATELIMIT_CFG(mainType_val, &rxRateLimitCfg);

    fe_data->api_data.meter_cfg.enable = rxRateLimitCfg.MeterEn;
    return 0;
}


int fe_api_get_acnt_id_by_skb(struct sk_buff *skb, FE_Dir_t dir, QDMA_AcntType_T acnt_type)
{
	unchar acnt_id = ACNT2_DEFAULT;
	uint idx = 0, acnt_enable = 0;
	Meter_cfg_t meter_data;
	
	memset(&meter_data, 0, sizeof(Meter_cfg_t));

	if((acnt_type < ACNT0_TYPE) || (acnt_type > ACNT2_TYPE)){
		printk("acnt_type is error!\n");
		return acnt_id;
	}

	if (dir == UP_STREAM){
		meter_data.acnt_mode = feResManage->acnt_up_mode[acnt_type];
	}else{
		meter_data.acnt_mode = feResManage->acnt_dw_mode[acnt_type];
	}
	
	acnt_id = fe_get_acnt_index_by_type(skb, acnt_type, &acnt_enable, &idx);
	FE_DEBUG(FE_DBG_MSG, "get acnt%d_enable is %x, idx is %d, acnt_id is %x", acnt_type, acnt_enable, idx, acnt_id);
	
	if(acnt_enable)
	{
		meter_data.dir = dir;
		meter_data.idx = idx;
		acnt_id = fe_api_get_acnt_idx(acnt_type, &meter_data);
	}

	return acnt_id;
}

int fe_api_get_acnt2_idx(struct ecnt_fe_data *fe_data)
{
    unchar acnt2_id = ACNT2_DEFAULT;
    int idx = 0;
    FE_ACNT_Mode_t acnt2_mode;
    
    struct sk_buff *skb = fe_data->api_data.meter_cfg.skb ;
    FE_Dir_t dir = fe_data->api_data.meter_cfg.dir;

    if(NULL == skb)
        return -1 ;
    
    if(NULL == feResManage){
        fe_data->api_data.meter_cfg.acnt2_id = acnt2_id;
        return -1 ;
    }
	
	acnt2_id = fe_api_get_acnt_id_by_skb(skb, dir, ACNT2_TYPE);
    fe_data->api_data.meter_cfg.acnt2_id = acnt2_id;
	FE_DEBUG(FE_DBG_TRACE, "get acnt2 acnt-id %d and fe_resource_mark is %x", acnt2_id, skb->fe_resource_mark);

    return 0;
}

int fe_api_get_acnt1_idx(struct ecnt_fe_data *fe_data)
{
    unchar acnt1_id = ACNT1_DEFAULT;
    int idx = 0, idx2 = 0;
    FE_ACNT_Mode_t acnt1_mode;
    
    struct sk_buff *skb = fe_data->api_data.meter_cfg.skb ;
    FE_Dir_t dir = fe_data->api_data.meter_cfg.dir;

    if(NULL == skb)
        return -1 ;
    
    if(NULL == feResManage){
        fe_data->api_data.meter_cfg.acnt1_id = acnt1_id;
        return -1 ;
    }
	
	acnt1_id = fe_api_get_acnt_id_by_skb(skb, dir, ACNT1_TYPE);
    fe_data->api_data.meter_cfg.acnt1_id = acnt1_id;
	FE_DEBUG(FE_DBG_MSG, "get acnt1 acnt-id %d and fe_resource_mark is %x", acnt1_id, skb->fe_resource_mark);

    return 0;
}

int fe_api_get_acnt0_idx(struct ecnt_fe_data *fe_data)
{
    unchar acnt0_id = ACNT0_DEFAULT;
    int idx = 0;
    FE_ACNT_Mode_t acnt0_mode;
    
    struct sk_buff *skb = fe_data->api_data.meter_cfg.skb ;
    FE_Dir_t dir = fe_data->api_data.meter_cfg.dir;

    if(NULL == skb)
        return -1 ;
    
    if(NULL == feResManage){
        fe_data->api_data.meter_cfg.acnt0_id = acnt0_id;
        return -1 ;
    }
	
	acnt0_id = fe_api_get_acnt_id_by_skb(skb, dir, ACNT0_TYPE);
    fe_data->api_data.meter_cfg.acnt0_id = acnt0_id;
	FE_DEBUG(FE_DBG_TRACE, "get acnt0 acnt-id %d and fe_resource_mark is %x", acnt0_id, skb->fe_resource_mark);

    return 0;
}
/******************************************************************************
 Descriptor:    It's used to get rx ratelimit mode of down stream and up stream 
 Input Args:    dir
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_get_rx_ratelimit_mode(struct ecnt_fe_data *fe_data)
{
	FE_Meter_Mode_t mode = NULL_BIND_MODE;
	FE_Dir_t dir = -1;
	QDMA_MeterType_T meter_group_id = 3;

	dir = fe_data->api_data.meter_cfg.dir;
    meter_group_id = fe_data->api_data.meter_cfg.idx;		

	if(feResManage == NULL)
		return -1;
	
	if(meter_group_id < METER0_TYPE || meter_group_id > METER2_TYPE){
		printk("meter group is error!");
		return -1;
	}

	if(dir == DOWN_STREAM) {
		mode = feResManage->dw_stream_mode[meter_group_id];
	} else {
		mode = feResManage->up_stream_mode[meter_group_id];
	}

	fe_data->api_data.meter_cfg.mode = mode;
	FE_DEBUG(FE_DBG_MSG, "get meter group-%d mode is %d!", meter_group_id, mode);
	
	return 0;
}

int fe_api_set_rx_ratelimit_rule(struct ecnt_fe_data *fe_data)
{
    FE_Dir_t dir = -1;
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    unchar idx = 0;
    
    dir = fe_data->api_data.meter_cfg.dir;
    mode = fe_data->api_data.meter_cfg.mode;
    idx = fe_data->api_data.meter_cfg.idx;/*idx */
    
    switch (mode){
        case IP_BIND_MODE:
            if(idx < IP_MAX_NUM){
                if(dir == UP_STREAM)
                    ip_info_up[idx] = fe_data->api_data.meter_cfg.ip_info ;
                else
                    ip_info_dw[idx] = fe_data->api_data.meter_cfg.ip_info ;
            }
            break;
        case IF_BIND_MODE:
            if(idx < IF_MAX_NUM){
                if(dir == UP_STREAM)
                    itf_info_up[idx] = fe_data->api_data.meter_cfg.if_info ;
                else
                    itf_info_dw[idx] = fe_data->api_data.meter_cfg.if_info ;
            }
            break;
        default:
            printk("mode is not correct, %d\n", mode);
            break;
    }
    return 0;
}

int fe_api_set_rx_ratelimit_mode(struct ecnt_fe_data *fe_data){
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    FE_Meter_Mode_t old_mode = NULL_BIND_MODE;
    FE_Dir_t dir = 2;
	int old_meter = -1;
	QDMA_MeterType_T meter_group_id = 3;

    dir = fe_data->api_data.meter_cfg.dir;
    mode = fe_data->api_data.meter_cfg.mode;
    meter_group_id = fe_data->api_data.meter_cfg.idx;

    if(feResManage == NULL)
        return -1;

    if(meter_group_id < METER0_TYPE || meter_group_id > METER2_TYPE){
        printk("meter group is error!");
        return -1;
    }

    old_meter = fe_check_meter_group(dir, meter_group_id, mode);

    FE_DEBUG(FE_DBG_MSG, "%s set meter%d mode is %d, old_meter is %d\n", dir?"UP":"DOWN", meter_group_id, mode, old_meter);
    if((old_meter != meter_group_id) && (old_meter <= METER2_TYPE)){
        fe_remove_meter(dir, mode, old_meter);
		if(dir == DOWN_STREAM)
		    feResManage->dw_stream_mode[old_meter] = NULL_BIND_MODE;
		else
			feResManage->up_stream_mode[old_meter] = NULL_BIND_MODE;
    }

    if(dir == DOWN_STREAM) {
        old_mode = feResManage->dw_stream_mode[meter_group_id];
    } else {
        old_mode = feResManage->up_stream_mode[meter_group_id];
    }
	
    FE_DEBUG(FE_DBG_MSG, " meter%d old mode is %d, new mode is %d\n",  meter_group_id, old_mode, mode);
    if(mode != old_mode){
        fe_remove_meter(dir, old_mode, meter_group_id);
        if(dir == DOWN_STREAM)
        	feResManage->dw_stream_mode[meter_group_id] = mode;
        else
        	feResManage->up_stream_mode[meter_group_id] = mode;
        fe_add_meter(dir, mode, meter_group_id);
    }
	
    PPE_API_CLEAN_FOE_TABLE();

    return 0;
}

int fe_api_set_acnt_group_mode(FE_Dir_t dir, QDMA_AcntType_T acnt_type, FE_ACNT_Mode_t new_acnt_mode){
    QDMA_AcntType_T old_acnt_type = 3;
    FE_ACNT_Mode_t old_acnt_mode = DEFAULT_NULL_MODE;
	
    old_acnt_type = fe_check_acnt_group(dir, acnt_type, new_acnt_mode);

    if((old_acnt_type != acnt_type) && (old_acnt_type <= ACNT2_TYPE)){
        fe_deinit_acnt_resource_from_mode(dir, new_acnt_mode, old_acnt_type);
        if(dir == UP_STREAM){
        	feResManage->acnt_up_mode[old_acnt_type] = DEFAULT_NULL_MODE;
        }else if(dir == DOWN_STREAM){
        	feResManage->acnt_dw_mode[old_acnt_type] = DEFAULT_NULL_MODE;
        }
    }

    if (dir == UP_STREAM){ 
        old_acnt_mode = feResManage->acnt_up_mode[acnt_type];
    }else{
        old_acnt_mode = feResManage->acnt_dw_mode[acnt_type];
    }
	
    if(new_acnt_mode != old_acnt_mode){
        fe_deinit_acnt_resource_from_mode(dir, old_acnt_mode, acnt_type);
        if (dir == UP_STREAM){ 
        	feResManage->acnt_up_mode[acnt_type] = new_acnt_mode;
        }else{
        	feResManage->acnt_dw_mode[acnt_type] = new_acnt_mode;
        }
        fe_init_acnt_resource_from_mode(dir, new_acnt_mode, acnt_type);
	}
    return 0;
}


int fe_api_set_acnt0_mode(struct ecnt_fe_data *fe_data){
    FE_ACNT_Mode_t new_acnt0_mode = DEFAULT_NULL_MODE;
    FE_ACNT_Mode_t old_acnt0_mode = DEFAULT_NULL_MODE;
    QDMA_AcntType_T old_acnt_type = 3;
    FE_Dir_t dir = -1;

    dir = fe_data->api_data.meter_cfg.dir;
    new_acnt0_mode = fe_data->api_data.meter_cfg.acnt_mode;
    if(feResManage == NULL)
        return -1;

	fe_api_set_acnt_group_mode(dir, ACNT0_TYPE, new_acnt0_mode);

    return 0;
}

int fe_api_set_acnt1_mode(struct ecnt_fe_data *fe_data){
    FE_ACNT_Mode_t new_acnt1_mode = DEFAULT_NULL_MODE;
    FE_ACNT_Mode_t old_acnt1_mode = DEFAULT_NULL_MODE;
    QDMA_AcntType_T old_acnt_type = 3;
	FE_Dir_t dir = -1;

    dir = fe_data->api_data.meter_cfg.dir;
    new_acnt1_mode = fe_data->api_data.meter_cfg.acnt_mode;
    if(feResManage == NULL)
        return -1;
	fe_api_set_acnt_group_mode(dir, ACNT1_TYPE, new_acnt1_mode);

    return 0;
}

int fe_api_set_acnt2_mode(struct ecnt_fe_data *fe_data){
    FE_ACNT_Mode_t new_acnt2_mode = DEFAULT_NULL_MODE;
    FE_ACNT_Mode_t old_acnt2_mode = DEFAULT_NULL_MODE;
    QDMA_AcntType_T old_acnt_type = 3;
    FE_Dir_t dir = -1;

    dir = fe_data->api_data.meter_cfg.dir;
    new_acnt2_mode = fe_data->api_data.meter_cfg.acnt_mode;
    if(feResManage == NULL)
        return -1;
	fe_api_set_acnt_group_mode(dir, ACNT2_TYPE, new_acnt2_mode);

    return 0;
}

int fe_api_get_acnt0_mode(struct ecnt_fe_data *fe_data){
    FE_ACNT_Mode_t acnt0_mode = DEFAULT_NULL_MODE;
    FE_Dir_t dir = -1;

    if(feResManage == NULL)
        return -1;

    dir = fe_data->api_data.meter_cfg.dir;
    if (dir == UP_STREAM){ 
        acnt0_mode = feResManage->acnt_up_mode[ACNT0_TYPE];
    }else{
        acnt0_mode = feResManage->acnt_dw_mode[ACNT0_TYPE];
    }

    fe_data->api_data.meter_cfg.acnt_mode = acnt0_mode;
    return 0;
}

int fe_api_get_acnt1_mode(struct ecnt_fe_data *fe_data){
    FE_ACNT_Mode_t acnt1_mode = DEFAULT_NULL_MODE;
    FE_Dir_t dir = -1;

    if(feResManage == NULL)
        return -1;

    dir = fe_data->api_data.meter_cfg.dir;
    if (dir == UP_STREAM){ 
        acnt1_mode = feResManage->acnt_up_mode[ACNT1_TYPE];
    }else{
        acnt1_mode = feResManage->acnt_dw_mode[ACNT1_TYPE];
    }
	
    fe_data->api_data.meter_cfg.acnt_mode = acnt1_mode;
    return 0;
}


int fe_api_get_dev_mac_index(struct ecnt_fe_data *fe_data){
    unsigned char eth_mac[ETH_ALEN] = {0};
    int mac_idx = -1;
	
    memcpy(eth_mac, fe_data->api_data.meter_cfg.mac, ETH_ALEN);
    mac_idx = findMacMatchItem(eth_mac);

	FE_DEBUG(FE_DBG_MSG, "find mac %02x:%02x:%02x:%02x:%02x:%02x in mac list[%d]", eth_mac[0], 
		eth_mac[1],eth_mac[2],eth_mac[3],eth_mac[4],eth_mac[5], mac_idx);
	
    if(mac_idx < 0){
        mac_idx = 0;
    }else
        mac_idx = mac_idx+1;

    fe_data->api_data.meter_cfg.idx = (unsigned char)mac_idx;

    return 0;
}

int fe_api_set_meter_ctl_by_olt(struct ecnt_fe_data *fe_data){
    unchar idx = 0;
    idx = fe_data->api_data.meter_cfg.idx;

    if(feResManage == NULL)
        return -1;

    if(idx)
        feResManage->Meter_Ctl_By_Olt = 1;
    else
        feResManage->Meter_Ctl_By_Olt = 0;

    return 0;
}

int fe_api_get_flow_cnt(struct ecnt_fe_data *fe_data){
    FE_Dir_t dir = -1;
    unchar idx = 0;
    unchar acnt_id = ACNT2_DEFAULT;
    QDMA_AcntType_T acnt_type = 3;
    int mainType_val = 0;
    QDMA_FlowCntValueGet_T flowCntValueGet;
    FE_ACNT_Mode_t acnt_mode = DEFAULT_NULL_MODE;
	Meter_cfg_t meter_data;
	unsigned int tmp=0;

    memset(&flowCntValueGet, 0, sizeof(QDMA_FlowCntValueGet_T));
    memset(&meter_data, 0, sizeof(Meter_cfg_t));

    meter_data = fe_data->api_data.meter_cfg;
    dir = meter_data.dir;
    idx = meter_data.idx;
    acnt_mode = meter_data.acnt_mode;

    if(feResManage == NULL)
        return -1;

	if(((acnt_mode < TRAFFIC_FLOW_MODE) || (acnt_mode > WAN_ITF_MODE)) && (acnt_mode != LLID_MODE)){
		printk("Not support, flow acnt_mode is %d!\n", acnt_mode);
        return -1;
	}

	FE_DEBUG(FE_DBG_MSG, "%s flow acnt_mode is %d, flow id is %d", dir?"UP":"DOWN", acnt_mode, idx);
	
	fe_get_acnt_group(&acnt_type, dir, acnt_mode);
	if((acnt_type < ACNT0_TYPE) || (acnt_type > ACNT2_TYPE)){
		printk("acnt_type is error! Can not find mode %d in Account group list!", acnt_mode);
		return acnt_id;
	}
		
	acnt_id = fe_api_get_acnt_idx(acnt_type, &meter_data);
	if(!aewan_lan_mode){
		if(dir == UP_STREAM){
			 mainType_val = ECNT_QDMA_WAN;				 
		 }else{
			 mainType_val = ECNT_QDMA_LAN;
		 }

		//	FE_DEBUG(FE_DBG_MSG, "%s read acnt%d acnt-id %d\n", dir?"UP":"DOWN", acnt_type, acnt_id);

		flowCntValueGet.grpIdx = acnt_type;
		flowCntValueGet.cntIdx = acnt_id;
		QDMA_API_GET_FLOW_CNTR_VALUE(mainType_val, &flowCntValueGet);
		fe_data->api_data.meter_cfg.byteCnt_H = flowCntValueGet.byteHigh;
		fe_data->api_data.meter_cfg.byteCnt_L = flowCntValueGet.byteLow;
		fe_data->api_data.meter_cfg.pktCnt = flowCntValueGet.pktCnt; 
		fe_data->api_data.meter_cfg.pktCnt_H = flowCntValueGet.pktHigh;
	}else if(aewan_lan_mode){
		if(dir == UP_STREAM){
			 mainType_val = ECNT_QDMA_LAN;
			 if(TRAFFIC_FLOW_MODE==acnt_mode)
			 	acnt_id=acnt_id+TRAFFIC_FLOW_MAX_NUM/2;
			 else if(DEV_MODE==acnt_mode)
				 acnt_id=acnt_id+MAX_DEV_BANDWIDTH_NUM/2;
			 else if(WAN_ITF_MODE==acnt_mode)
				 acnt_id=acnt_id+WAN_ITF_ACNT_NUM/2;

			 flowCntValueGet.grpIdx = acnt_type;
			 flowCntValueGet.cntIdx = acnt_id;
			 QDMA_API_GET_FLOW_CNTR_VALUE(mainType_val, &flowCntValueGet);
			 fe_data->api_data.meter_cfg.byteCnt_H = flowCntValueGet.byteHigh;
			 fe_data->api_data.meter_cfg.byteCnt_L = flowCntValueGet.byteLow;
			 fe_data->api_data.meter_cfg.pktCnt = flowCntValueGet.pktCnt; 
			 fe_data->api_data.meter_cfg.pktCnt_H = flowCntValueGet.pktHigh;
		 }else{
		 	/*downstream to switch*/
			 mainType_val = ECNT_QDMA_LAN;
			 flowCntValueGet.grpIdx = acnt_type;
			 flowCntValueGet.cntIdx = acnt_id;
			 QDMA_API_GET_FLOW_CNTR_VALUE(mainType_val, &flowCntValueGet);
			 fe_data->api_data.meter_cfg.byteCnt_H = flowCntValueGet.byteHigh;
			 fe_data->api_data.meter_cfg.byteCnt_L = flowCntValueGet.byteLow;
			 fe_data->api_data.meter_cfg.pktCnt = flowCntValueGet.pktCnt; 
			 fe_data->api_data.meter_cfg.pktCnt_H = flowCntValueGet.pktHigh;
			 /*downstream to pon_serdes*/
			 mainType_val = ECNT_QDMA_WAN;
			 flowCntValueGet.grpIdx = acnt_type;
			 flowCntValueGet.cntIdx = acnt_id;
			 QDMA_API_GET_FLOW_CNTR_VALUE(mainType_val, &flowCntValueGet);

			 tmp=fe_data->api_data.meter_cfg.byteCnt_L + flowCntValueGet.byteLow;
			 fe_data->api_data.meter_cfg.byteCnt_H += flowCntValueGet.byteHigh;
			 if(tmp < fe_data->api_data.meter_cfg.byteCnt_L || tmp < flowCntValueGet.byteLow){
				 fe_data->api_data.meter_cfg.byteCnt_H++;
			 }
			 fe_data->api_data.meter_cfg.byteCnt_L = tmp;
			
			 tmp=fe_data->api_data.meter_cfg.pktCnt  + flowCntValueGet.pktCnt;
			 fe_data->api_data.meter_cfg.pktCnt_H += flowCntValueGet.pktHigh;
			 if(tmp < fe_data->api_data.meter_cfg.pktCnt || tmp < flowCntValueGet.pktCnt){
				 fe_data->api_data.meter_cfg.pktCnt_H++;
			 }
			 fe_data->api_data.meter_cfg.pktCnt = tmp; 
		 }

	}
    return 0;
}

#ifdef CONFIG_TP_IMAGE
#ifdef CONFIG_IPT_STAT_POLL_TP_ECONET
int fe_get_dev_mac_stat(int idx, struct devBandwidth_s *devBandWidthPtr, int dir, IPT_STAT_POLL_ECONET_CB poll_cb)
{
	int ret = 0;
	struct ecnt_fe_data para;

	if(!devBandWidthPtr || !poll_cb)
	{
		return -1;
	}

	memset(&para, 0, sizeof(struct ecnt_fe_data));
	para.function_id = FE_GET_FLOW_CNT ;
	para.api_data.meter_cfg.dir = dir;
	para.api_data.meter_cfg.idx = idx;
	para.api_data.meter_cfg.acnt_mode = DEV_MODE;

	ret = fe_api_get_flow_cnt(&para);

	if(ret < 0)
	{
		return ret;
	}

	//printk("devBandWidthPtr->mac:%2x%2x,dir:%d,idx:%d,pkt:%d,byte:%llu\n",devBandWidthPtr->mac[0],devBandWidthPtr->mac[1],dir,idx,para.api_data.meter_cfg.pktCnt, (uint64_t)(para.api_data.meter_cfg.byteCnt_L  + para.api_data.meter_cfg.byteCnt_H << 32));

	poll_cb(dir == UP_STREAM ? 0 : 1, devBandWidthPtr->mac, para.api_data.meter_cfg.pktCnt, (uint64_t)(para.api_data.meter_cfg.byteCnt_L  + para.api_data.meter_cfg.byteCnt_H << 32), 0);

	para.function_id = FE_CLEAR_FLOW_CNT ;
	ret = fe_api_clear_flow_cnt(&para);

	return ret;
}

int fe_poll_ipt_stat(IPT_STAT_POLL_ECONET_CB poll_cb)
{
	int ret = 0;
	int i;
	struct devBandwidth_s *devBandWidthPtr = NULL;

	if(!gHwBandwidthList || gHwBandwidthList->enable == 0 || !poll_cb)
	{
		return -1;
	}

	for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++)
	{
		devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i]);
		if(devBandWidthPtr == NULL || !devBandWidthPtr->valid)
		{
			continue;
		}

		fe_get_dev_mac_stat(i, devBandWidthPtr, UP_STREAM, poll_cb);
		fe_get_dev_mac_stat(i, devBandWidthPtr, DOWN_STREAM, poll_cb);
	}

	return 0;
}
EXPORT_SYMBOL(fe_poll_ipt_stat);
#endif /* CONFIG_IPT_STAT_POLL_TP_ECONET */
#endif /* CONFIG_TP_IMAGE */


int fe_api_clear_flow_cnt(struct ecnt_fe_data *fe_data){
    FE_Dir_t dir ;
    unchar idx = 0;
    unchar acnt_id = 0;
    QDMA_AcntType_T acnt_type = 3;
    int mainType_val = 0;
    QDMA_FlowCntValueClear_T flowCntValueClear;
    FE_ACNT_Mode_t acnt_mode = DEFAULT_NULL_MODE;
	Meter_cfg_t meter_data;
	
    memset(&flowCntValueClear, 0, sizeof(QDMA_FlowCntValueClear_T));
	memset(&meter_data, 0, sizeof(Meter_cfg_t));

	meter_data = fe_data->api_data.meter_cfg;
    dir = fe_data->api_data.meter_cfg.dir;
    idx = fe_data->api_data.meter_cfg.idx;
    acnt_mode = fe_data->api_data.meter_cfg.acnt_mode;

    if(feResManage == NULL)
        return -1;
	
	if((acnt_mode > WAN_ITF_MODE) && (acnt_mode != LLID_MODE)){
		printk("Not support %d, flow acnt_mode is %d!\n", idx, acnt_mode);
        return -1;
	}
	
//	FE_DEBUG(FE_DBG_ERR, "%s clear flow, acnt_mode is %d, flow id is %d\n", dir?"UP":"DOWN", acnt_mode, idx);
	
	acnt_id = fe_get_acnt_group(&acnt_type, dir, acnt_mode);
	acnt_id = fe_api_get_acnt_idx(acnt_type, &meter_data);

	if(dir == UP_STREAM){
		 mainType_val = ECNT_QDMA_WAN;				 
	}else{
		 mainType_val = ECNT_QDMA_LAN;
	}
 
    flowCntValueClear.grpIdx = acnt_type;
    flowCntValueClear.cntIdx = acnt_id;
    flowCntValueClear.clearMode = FLOWCNT_CLEAR_ALL;

    QDMA_API_CLEAR_FLOW_CNTR_VALUE(mainType_val, &flowCntValueClear);

    return 0;

}

int fe_api_set_pse_oq_threshold(struct ecnt_fe_data *fe_data) 
{
	unsigned int queue_idx = 0;
	FE_PonMode_Sel_t pon_mode = 0;

	pon_mode = fe_data->api_data.pon_mode;
	if (pon_mode == FE_MAC_GPON)
	{
		#if defined(TCSUPPORT_CPU_EN7523)
		/* all oqs of port-2 share one setting */
		pse_set_oq_rsv(2, 0, 0x80);
		/* Port-3 for XFI, has 4 OQs */
		for(queue_idx = 0; queue_idx < 4; queue_idx++)
		{
			pse_set_oq_rsv(3, queue_idx, 0x80);
		}
		IO_SREG(PSE_OQ_TOTAL_RSV, 0x1a30);
		#else
		write_reg_word(PSE_OQ_RSV2, 0x00800080);
		write_reg_word(PSE_OQ_THD, 0x1a300078);
		#endif
		write_reg_word(PSE_SHARE_USED_THD, 0x10002330);
	}
	else
	{
#if defined(TCSUPPORT_CPU_EN7523)
		/* all oqs of port-2 share one setting */
		pse_set_oq_rsv(2, 0, 0x80);
		/* Port-3 for XFI, has 4 OQs */
		for(queue_idx = 0; queue_idx < 4; queue_idx++){
			pse_set_oq_rsv(3, queue_idx, 0x80);
		}
		IO_SREG(PSE_OQ_TOTAL_RSV, 0x0a20);
#else
		write_reg_word(PSE_OQ_RSV2, 0x00800080);
		write_reg_word(PSE_OQ_THD, 0x0a200078);
#endif
		write_reg_word(PSE_SHARE_USED_THD, 0x10003340);
	}
    
    return 0;
}

static void fe_api_set_forward_port(unsigned char pkt_formate, unsigned char gdma_sel, unsigned char port)
{
	unsigned int reg=0, value=0;
	if(0 == pkt_formate)
	{
		switch(gdma_sel){
			case 1:
				reg = GDMA1_FWD_CFG;
				break;
			case 2:
				reg = GDMA2_FWD_CFG;
				break;
			case 3:
				reg = GDMA3_FWD_CFG;
				break;
			default:
				return;
		}
		value = read_reg_word(reg);
		value = (value &~BC_FP_MASK) | (port << BC_FP_OFFSET);
		write_reg_word(reg, value);
	}
	else if(1 == pkt_formate)
	{
		switch(gdma_sel){
			case 1:
				reg = GDMA1_FWD_CFG;
				break;
			case 2:
				reg = GDMA2_FWD_CFG;
				break;
			case 3:
				reg = GDMA3_FWD_CFG;
				break;
			default:
				return;
		}
		value = read_reg_word(reg);
		value = (value &~MC_FP_MASK) | (port << MC_FP_OFFSET);
		write_reg_word(reg, value);
	}
	return;
}

static void fe_api_set_force_ring(unsigned char pkt_formate, unsigned char gdma_sel, unsigned char ring)
{
	unsigned int reg=0, value=0;
	if(0 == pkt_formate)
	{
		switch(gdma_sel){
			case 1:
				reg = GDMA1_FWD_FQ;
				break;
			case 2:
				reg = GDMA2_FWD_FQ;
				break;
			case 3:
				reg = GDMA3_FWD_FQ;
				break;
			default:
				return;
		}
		value = read_reg_word(reg);
		value = (value &~BC_FQ_MASK) | (ring << BC_FQ_OFFSET);
		write_reg_word(reg, value);
	}
	else if(1 == pkt_formate)
	{
		switch(gdma_sel){
			case 1:
				reg = GDMA1_FWD_FQ;
				break;
			case 2:
				reg = GDMA2_FWD_FQ;
				break;
			case 3:
				reg = GDMA3_FWD_FQ;
				break;
			default:
				return;
		}
		value = read_reg_word(reg);
		value = (value &~MC_FQ_MASK) | (ring << MC_FQ_OFFSET);
		write_reg_word(reg, value);
	}
	return;
}

int fe_api_set_ratelimit_for_pkt_formate(struct ecnt_fe_data *fe_data)
{
	if(0 == fe_data->api_data.pkt_ratelimit.pkt_formate)/*BC*/
	{
		switch(fe_data->api_data.pkt_ratelimit.mode)
		{
			case DISABLE:
				fe_api_set_forward_port(0, 1, DEFAULT_PORT); /*GDMA1*/
				fe_api_set_force_ring(0, 1, 0);
				fe_api_set_forward_port(0, 3, DEFAULT_PORT); /*GDMA3*/
				fe_api_set_force_ring(0, 3, 0);
				fe_api_set_forward_port(0, 2, DEFAULT_PORT); /*GDMA2*/
				fe_api_set_force_ring(0, 2, 0);
				break;
			case TOTAL:
				fe_api_set_forward_port(0, 1, WAN_PORT); /*GDMA1*/
				fe_api_set_force_ring(0, 1, 4);
				fe_api_set_forward_port(0, 3, WAN_PORT); /*GDMA3*/
				fe_api_set_force_ring(0, 3, 4);
				fe_api_set_forward_port(0, 2, WAN_PORT); /*GDMA2*/
				fe_api_set_force_ring(0, 2, 4);
				break;
			case SEPARATE:
				fe_api_set_forward_port(0, 1, LAN_PORT); /*GDMA1*/
				fe_api_set_force_ring(0, 1, 4);
				fe_api_set_forward_port(0, 3, LAN_PORT); /*GDMA3*/
				fe_api_set_force_ring(0, 3, 4);
				fe_api_set_forward_port(0, 2, WAN_PORT); /*GDMA2*/
				fe_api_set_force_ring(0, 2, 4);
				break;
			default:
				printk("Not Support this mode yet\n");
				return -1;
		}
	}
	else if(1 == fe_data->api_data.pkt_ratelimit.pkt_formate)/*MC*/
	{
		switch(fe_data->api_data.pkt_ratelimit.mode)
		{
			case DISABLE:
				fe_api_set_forward_port(1, 1, DEFAULT_PORT); /*GDMA1*/
				fe_api_set_force_ring(1, 1, 0);
				fe_api_set_forward_port(1, 3, DEFAULT_PORT); /*GDMA3*/
				fe_api_set_force_ring(1, 3, 0);
				fe_api_set_forward_port(1, 2, DEFAULT_PORT); /*GDMA2*/
				fe_api_set_force_ring(1, 2, 0);
				break;
			case TOTAL:
				fe_api_set_forward_port(1, 1, WAN_PORT); /*GDMA1*/
				fe_api_set_force_ring(1, 1, 7);
				fe_api_set_forward_port(1, 3, WAN_PORT); /*GDMA3*/
				fe_api_set_force_ring(1, 3, 7);
				fe_api_set_forward_port(1, 2, WAN_PORT); /*GDMA2*/
				fe_api_set_force_ring(1, 2, 7);
				break;
			case SEPARATE:
				fe_api_set_forward_port(1, 1, LAN_PORT); /*GDMA1*/
				fe_api_set_force_ring(1, 1, 7);
				fe_api_set_forward_port(1, 3, LAN_PORT); /*GDMA3*/
				fe_api_set_force_ring(1, 3, 7);
				fe_api_set_forward_port(1, 2, WAN_PORT); /*GDMA2*/
				fe_api_set_force_ring(1, 2, 7);
				break;
			default:
				printk("Not Support this mode yet\n");
				return -1;
		}
	}
	else
	{
		printk("Not Support this formate\n");
		return -1;
	}
	return 0;
}
 
/******************************************************************************
 Descriptor : It's used to set device statistics and ratelimit mode 
 Input Args :     
 Ret Value: 0 : successful 
******************************************************************************/
int fe_api_set_dev_stat_ratelimit_mode(struct ecnt_fe_data *fe_data)
{
	return 0;
}
