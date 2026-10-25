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
#ifdef CONFIG_TP_IMAGE
#ifdef CONFIG_IPT_STAT_POLL_TP_ECONET
#include <linux/ppp_defs.h>
#endif /* CONFIG_IPT_STAT_POLL_TP_ECONET */
#endif /* CONFIG_TP_IMAGE */


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define AcntManage_T QDMA_AcntManage_T 
#define     FE_METER_FILTER_MARK    0xf0    /*original mark in skb->mark*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
int traffic_flow_filter = 0x03f00;
int traffic_flow_shift = 8;
int wan_remark_hook_enable=0;
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern struct devBandwidthList_s *gHwBandwidthList;
extern struct devBandwidthList_s *gDevBandwidthList;
extern Wan_itf_name wan_itf_info[WAN_ITF_ACNT_NUM];
extern int aewan_lan_mode;
extern int fe_account_mode;



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
/******************************************************************************
 Descriptor:	It's used to get meter mark from skb->fe_resource_mark
 Input Args:	
 Ret Value: 
******************************************************************************/
int qdma_qos_mark(struct sk_buff *skb)
{
	int priority = 0;
	priority = (skb->mark & FE_METER_FILTER_MARK)>> 4;
	
	return priority;
}


int fe_set_resource_mark(struct sk_buff *skb, QDMA_AcntType_T acnt_type, QDMA_MeterType_T meter_type, int index)
{
	/*****************fe_resource_mark bitmap format******************************/
	/*reserve			acnt1&meter2		acnt0&meter1	acnt2&meter0
	/*00000000			0000000 			00000000		000000000*/
	/*****************fe_resource_mark format*************************************/

	if(acnt_type == ACNT0_TYPE){
		skb->fe_resource_mark &= ~(ACCOUNT0_MASK << ACCOUNT0_OFFSET);
		skb->fe_resource_mark |= ((index & ACCOUNT0_MASK) << ACCOUNT0_OFFSET);
		skb->fe_resource_mark |= (0x1 << ACCOUNT0_ENABLE_OFFSET);
		if(meter_type == METER1_TYPE)
			skb->fe_resource_mark |= (0x1 << METER1_ENABLE_OFFSET);
	}else if(acnt_type == ACNT1_TYPE){
		skb->fe_resource_mark &= ~(ACCOUNT1_MASK << ACCOUNT1_OFFSET);
		skb->fe_resource_mark |= ((index & ACCOUNT1_MASK) << ACCOUNT1_OFFSET);
		skb->fe_resource_mark |= (0x1 << ACCOUNT1_ENABLE_OFFSET);
		if(meter_type == METER2_TYPE)
			skb->fe_resource_mark |= (0x1 << METER2_ENABLE_OFFSET);
 	}
 	else if(acnt_type == ACNT2_TYPE){
		skb->fe_resource_mark &= ~(ACCOUNT2_MASK);
		skb->fe_resource_mark |= (index & ACCOUNT2_MASK);
		skb->fe_resource_mark |= (0x1 << ACCOUNT2_ENABLE_OFFSET);
		if(meter_type == METER0_TYPE)
			skb->fe_resource_mark |= (0x1 << METER0_ENABLE_OFFSET);
    }
	
	FE_DEBUG(FE_DBG_MSG, "set acnt[%d] & meter[%d], use acnt_index %d, fe_resource_mark is %x\n",  acnt_type, meter_type, index, skb->fe_resource_mark);
	
	return 0;
}

unchar fe_get_meter_index_by_type(struct sk_buff *skb, QDMA_MeterType_T meter_type, int *meter_enable, int *idx)
{
	/*****************fe_resource_mark bitmap format******************************/
	/*reserve			acnt1&meter2		acnt0&meter1	acnt2&meter0*/
	/*00000000			0000000 			00000000		000000000*/
	/*****************fe_resource_mark format*************************************/

	unchar meter_id = GENERAL_INGRESS_NO_RATELIMIT_INDEX;

	if(meter_type == METER0_TYPE){
		*meter_enable = (skb->fe_resource_mark >> METER0_ENABLE_OFFSET) & 0x1;
		*idx =  skb->fe_resource_mark & METER0_MASK;
		meter_id = ACNT2_DEFAULT;
	}else if(meter_type == METER1_TYPE){
		*meter_enable = (skb->fe_resource_mark >> METER1_ENABLE_OFFSET) & 0x1;
		*idx =  (skb->fe_resource_mark >> ACCOUNT0_OFFSET) & METER1_MASK;
		meter_id = ACNT0_DEFAULT;
	}else if(meter_type == METER2_TYPE){
		*meter_enable = (skb->fe_resource_mark >> METER2_ENABLE_OFFSET) & 0x1;
		*idx =  (skb->fe_resource_mark >> ACCOUNT1_OFFSET) & METER2_MASK;
		meter_id = ACNT1_DEFAULT;
	}

	*idx -= 1;

    return meter_id;
}

unchar fe_get_acnt_index_by_type(struct sk_buff *skb, QDMA_AcntType_T acnt_type, uint *acnt_enable, uint *idx)
{
	unchar acnt_id = 0x7f;

	if(acnt_type == ACNT0_TYPE){
		*acnt_enable = (skb->fe_resource_mark >> ACCOUNT0_ENABLE_OFFSET) & 0x1;
		*idx =  (skb->fe_resource_mark >> ACCOUNT0_OFFSET) & ACCOUNT0_MASK;
		acnt_id = ACNT0_DEFAULT;
	}else if(acnt_type == ACNT1_TYPE){
		*acnt_enable = (skb->fe_resource_mark >> ACCOUNT1_ENABLE_OFFSET) & 0x1;
		*idx =  (skb->fe_resource_mark >> ACCOUNT1_OFFSET) & ACCOUNT1_MASK;
		acnt_id = ACNT1_DEFAULT;
	}else if(acnt_type == ACNT2_TYPE){
		*acnt_enable = (skb->fe_resource_mark >> ACCOUNT2_ENABLE_OFFSET) & 0x1;
		*idx =  skb->fe_resource_mark & ACCOUNT2_MASK;
		acnt_id = ACNT2_DEFAULT;
	}

	*idx -= 1;

    return acnt_id;
}
 
int fe_get_meter_group(QDMA_MeterType_T *meter_type, FE_Dir_t dir, FE_Meter_Mode_t mode)
{

    if(dir == UP_STREAM){
    	if(feResManage->up_stream_mode[METER0_TYPE] == mode){
        	*meter_type = METER0_TYPE;
	    }else if(feResManage->up_stream_mode[METER1_TYPE] == mode){
	        *meter_type = METER1_TYPE;
	    }else if(feResManage->up_stream_mode[METER2_TYPE] == mode){
	        *meter_type = METER2_TYPE;
	    }else{
	    	*meter_type = 3;
	        return 1;
	    }
    }else if(dir == DOWN_STREAM){
    	if(feResManage->dw_stream_mode[METER0_TYPE] == mode){
        	*meter_type = METER0_TYPE;
	    }else if(feResManage->dw_stream_mode[METER1_TYPE] == mode){
	        *meter_type = METER1_TYPE;
	    }else if(feResManage->dw_stream_mode[METER2_TYPE] == mode){
	        *meter_type = METER2_TYPE;
	    }else{
	    	*meter_type = 3;
	        return 1;
	    }
    }
	
	FE_DEBUG(FE_DBG_TRACE, "meter_type=%d mode=%d", *meter_type, dir, mode);

    return 0;
}

int fe_get_acnt_group(QDMA_AcntType_T *acnt_type, FE_Dir_t dir, FE_ACNT_Mode_t mode)
{

    if(dir == UP_STREAM){
	    if(feResManage->acnt_up_mode[ACNT0_TYPE] == mode){
	        *acnt_type = ACNT0_TYPE;
	    }else if(feResManage->acnt_up_mode[ACNT1_TYPE] == mode){
	        *acnt_type = ACNT1_TYPE;
	    }else if(feResManage->acnt_up_mode[ACNT2_TYPE] == mode){
	        *acnt_type = ACNT2_TYPE;
	    }else{
	        *acnt_type = 3;
	        return 1;
	    }
    }else if(dir == DOWN_STREAM){
	    if(feResManage->acnt_dw_mode[ACNT0_TYPE] == mode){
	        *acnt_type = ACNT0_TYPE;
	    }else if(feResManage->acnt_dw_mode[ACNT1_TYPE] == mode){
	        *acnt_type = ACNT1_TYPE;
	    }else if(feResManage->acnt_dw_mode[ACNT2_TYPE] == mode){
	        *acnt_type = ACNT2_TYPE;
	    }else{
	        *acnt_type = 3;
	        return 1;
	    }
    }

	FE_DEBUG(FE_DBG_TRACE, "acnt_type=%d mode=%d", *acnt_type, dir, mode);

    return 0;
}

int fe_check_acnt_group(FE_Dir_t dir, QDMA_AcntType_T acnt_type, FE_ACNT_Mode_t mode)
{
	int i = 0;
	/*****************fe_resource_mark bitmap format******************************/
	/*reserve			acnt1&meter2		acnt0&meter1	acnt2&meter0
	/*00000000			0000000 			00000000		000000000*/
	/*****************fe_resource_mark format*************************************/

	FE_DEBUG(FE_DBG_MSG, "acnt_type=%d mode=%d", acnt_type, mode);

	for(i=0; i<3; i++){
	    if(acnt_type != i){
			if((dir == UP_STREAM) && (feResManage->acnt_up_mode[i] == mode)){
//				feResManage->acnt_up_mode[i] = DEFAULT_NULL_MODE;
				return i;
			}else if((dir == DOWN_STREAM) && (feResManage->acnt_dw_mode[i] == mode)){
//				feResManage->acnt_dw_mode[i] = DEFAULT_NULL_MODE;
				return i;
			}
	    }
	}

    return acnt_type;
}

int fe_check_meter_group(FE_Dir_t dir, QDMA_MeterType_T meter_type, FE_Meter_Mode_t mode)
{
	int i = 0;

	FE_DEBUG(FE_DBG_MSG, "meter_type=%d mode=%d", meter_type, mode);

	for(i=0; i<3; i++){
	    if((meter_type != i) ){
			if ((dir == UP_STREAM) && (feResManage->up_stream_mode[i] == mode)){
				return i;
			}else if((dir == DOWN_STREAM) && (feResManage->dw_stream_mode[i] == mode)){			
				return i;
			}
	    }
	}

    return meter_type;
}

int fe_remark_skb_mark_from_traffic_mark(struct sk_buff *skb, int dir )
{ 
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    char *pname = NULL;
    int mask = 0, i = 0;
    QDMA_AcntType_T acnt_type = 3; 
	QDMA_MeterType_T meter_type = 3;
	int qos_idx = 0;

    if( (NULL == skb) || (NULL == feResManage) )
        return -1 ;

    FE_DEBUG(FE_DBG_TRACE, "start remark account by skb_mark!\n");

    
    pname = skb->dev->name;
    if(NULL == pname)
        return -1;
    else{
        if(('r' != pname[0]) && ('e' != pname[0])){
			FE_DEBUG(FE_DBG_TRACE, "set %s not support!\n", pname);
            return -1;
        }
    }
	
	fe_get_acnt_group(&acnt_type, dir, TRAFFIC_FLOW_MODE);
	fe_get_meter_group(&meter_type, dir, TRAFFIC_QOS_MODE);

	mask = (skb->mark & traffic_flow_filter) >> traffic_flow_shift;
	
	if(meter_type == 3){
		fe_get_meter_group(&meter_type, dir, CAR_QOS_MODE);
		if(meter_type == METER1_TYPE){
	        qos_idx = qdma_qos_mark(skb);
			if((qos_idx > 0) && (qos_idx <= QOS_CAR_NUM))
				mask = qos_idx;				
		}
	}
	if(mask == 0){
		FE_DEBUG(FE_DBG_MSG, "This skb not have mark, skb->mark is %x, fe_resource_mark is %x\n", skb->mark, skb->fe_resource_mark);
		return -1;
	}
	else{
		if(aewan_lan_mode){
			if(mask >TRAFFIC_FLOW_MAX_NUM/2 -1)
				return -1;
			if(UP_STREAM == dir){
				mask=TRAFFIC_FLOW_MAX_NUM/2 + mask;
			}
		fe_set_resource_mark(skb, acnt_type, meter_type, mask);
		}else{
			fe_set_resource_mark(skb, acnt_type, meter_type, mask);
		}
	}
	
	FE_DEBUG(FE_DBG_TRACE, "set acnt%d meter%d, fe_resource_mark is %x\n",  acnt_type, meter_type, skb->fe_resource_mark);

    return 0;
}

/*check mac*/
int isMacMatch(uint8_t * Dst, uint8_t * Src){
    if((Dst[0] == Src[0]) 
        && (Dst[1] == Src[1]) 
        && (Dst[2] == Src[2]) 
        && (Dst[3] == Src[3])   
        && (Dst[4] == Src[4]) 
        && (Dst[5] == Src[5])){
            return 1;
        }
    
    return 0;
}

/*find bandwitdh item by mac*/
int findMacMatchItem(unsigned char *mac){
    struct devBandwidth_s *devBandWidthPtr = NULL;
    int i = 0;
    int max=MAX_DEV_BANDWIDTH_NUM;
    if(gHwBandwidthList == NULL)
        return -1;
	if(aewan_lan_mode)
		max = max/2;

    for(i = 0; i < max; i++){
        devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i]);
        if(devBandWidthPtr != NULL && isMacMatch(mac,devBandWidthPtr->mac)){
            return i;
        }       
    }

    return -1;  
}

/*find mac by bandwitdh item*/
int findItemMatchMac(int index, unsigned char *mac){
    struct devBandwidth_s *devBandWidthPtr = NULL;
    
    if(gHwBandwidthList == NULL)
        return -1;
    
    devBandWidthPtr = &(gHwBandwidthList->bandwidthList[index]);

	if( devBandWidthPtr != NULL ){
		memcpy(mac, devBandWidthPtr->mac, ETH_ALEN);
		return 1;
    }

    return -1;  
}

#ifdef CONFIG_TP_IMAGE
#ifdef CONFIG_IPT_STAT_POLL_TP_ECONET
#define PPP_PROTOCOL_LENGTH 2

int fe_get_vlan_tag_num(struct sk_buff* skb)
{
	uint16* buff = NULL;
	int vlan_tag_num = 0;

	buff = (uint16 *)((struct ethhdr *)LAYER2_HEADER(skb));

	if(NULL == buff)
	{
		return 0;
	}

	/* skip dst and src mac */
	buff += ETH_ALEN;

	if(skb->ecnt_sk_buff.stag_inserted)
	{
		/*Come from switch, skip special tag*/
		vlan_tag_num++;
		buff += ETH_TLEN;
	}

	/* We use sdk's parse vlan tag method */
	while(*buff == htons(ETH_P_8021Q) || *buff == htons(ETH_P_QinQ_88a8) || *buff == htons(ETH_P_QinQ_9100) || *buff == htons(ETH_P_ATMMPOA))
	{
		vlan_tag_num ++;
		buff += ETH_TLEN;
	}

	return vlan_tag_num;
}

int fe_get_packet_type(struct sk_buff* skb, int *offlen)
{
	int vlan_tag_num = fe_get_vlan_tag_num(skb);
	unsigned char* buff = (unsigned char*)LAYER2_HEADER(skb);
	uint32 eth_type = 0;
	uint16 pppoe_proto = 0;
	unsigned char* buff_off = NULL;

	if(NULL == buff)
	{
		return 0;
	}

	buff_off = buff;
	buff += (ETH_HLEN - ETH_TLEN + VLAN_HLEN * vlan_tag_num);/*skip dst and src mac and vlan struct*/
	eth_type = *(uint16*)buff;
	buff += ETH_TLEN;/*skip ether type*/

	if (eth_type == htons(ETH_P_PPP_SES))
	{
		buff += sizeof(struct pppoe_hdr);
		pppoe_proto = *(uint16*)buff;

		if (pppoe_proto == htons(PPP_IP))
		{
			eth_type = htons(ETH_P_IP);
		}
		else if (pppoe_proto == htons(PPP_IPV6))
		{
			eth_type = htons(ETH_P_IPV6);
		}
		buff += PPP_PROTOCOL_LENGTH;/*skip ppp protocol*/
	}

	*offlen = buff - buff_off;

	return htons(eth_type);
}

void* fe_get_network_header(struct sk_buff* skb)
{
	unsigned char* buff = (unsigned char*)LAYER2_HEADER(skb);
	int vlan_tag_num = fe_get_vlan_tag_num(skb);
	uint32 eth_type = 0;

	buff += (ETH_HLEN - ETH_TLEN + VLAN_HLEN * vlan_tag_num);/*skip dst and src mac and vlan struct*/
	eth_type = *(uint16*)buff;
	buff += ETH_TLEN;/*skip ether type*/

	if (eth_type == htons(ETH_P_PPP_SES))
	{
		buff += sizeof(struct pppoe_hdr);
		buff += PPP_PROTOCOL_LENGTH; /*skip ppp protocol*/
	}

	return buff;
}

int fe_get_ip_proto(struct sk_buff* skb)
{
	int offlen = 0;
	unsigned int eth_type = 0;
	struct iphdr* ih = NULL;
	struct ipv6hdr* ip6h = NULL;

	if(!skb)
	{
		return -1;
	}

	eth_type = fe_get_packet_type(skb, &offlen);

	if(ETH_P_IP == eth_type)
	{
		ih = (struct iphdr* )fe_get_network_header(skb);
		return ih->protocol;
	}
	else if(ETH_P_IPV6 == eth_type)
	{
		ip6h = (struct ipv6hdr*)fe_get_network_header(skb);
		return ip6h->nexthdr;
	}
	return -1;
}
#endif /* CONFIG_IPT_STAT_POLL_TP_ECONET */
#endif /* CONFIG_TP_IMAGE */

int fe_remark_skb_from_dev_mac(struct sk_buff *skb, int dir)
{ 
    unsigned char devMacAddr[ETH_ALEN] = {0};
    char *pname = NULL ;
    struct ethhdr *eth = NULL;
    int idx = -1;
    int mask = 0;
    QDMA_AcntType_T acnt_type = 3; 
	QDMA_MeterType_T meter_type = 3;  

    if( (NULL == skb) || (NULL == feResManage) )
        return -1 ;

    FE_DEBUG(FE_DBG_TRACE, "start remark account by mac!\n");

    pname = skb->dev->name ;
    eth = (struct ethhdr *)LAYER2_HEADER(skb);
    if(NULL == pname || NULL == eth){
        return -1;
	}
    else{
        if(('r' != pname[0]) && ('e' != pname[0])){
			FE_DEBUG(FE_DBG_TRACE, "set %s not support!\n", pname);
            return -1;
		}
    }

	fe_get_acnt_group(&acnt_type, dir, DEV_MODE);
	fe_get_meter_group(&meter_type, dir, DEV_MAC_MODE);
	
    if((DOWN_STREAM == dir) && (((skb->fe_resource_mark & ETHER_WAN_MASK) >> ETHER_WAN_OFFSET)==1)){
        memcpy(devMacAddr, eth->h_dest, ETH_ALEN);
    }else if(UP_STREAM == dir){
        memcpy(devMacAddr, eth->h_source, ETH_ALEN);
    }else{
        return -1;
	}

	if(gHwBandwidthList && gHwBandwidthList->enable == 0)
		return -1;

    idx = findMacMatchItem(devMacAddr);
    if(idx < 0){
		FE_DEBUG(FE_DBG_MSG, "Can not find mac in gHwBandwidthList, List maybe full!\n");
        return -1;
	}
    else{
		if(aewan_lan_mode){
			if(idx > MAX_DEV_BANDWIDTH_NUM/2 -1){
				return -1;
			}
        mask = idx + 1;
			if(UP_STREAM == dir)
				mask = mask + MAX_DEV_BANDWIDTH_NUM/2;
		}else{
			mask = idx + 1;
		}
	}
	FE_DEBUG(FE_DBG_TRACE, "set mac %02x:%02x:%02x:%02x:%02x:%02x use acnt%d mask %d\n", devMacAddr[0], 
		devMacAddr[1],devMacAddr[2],devMacAddr[3],devMacAddr[4],devMacAddr[5], acnt_type, mask);

#ifdef CONFIG_TP_IMAGE
#ifdef CONFIG_IPT_STAT_POLL_TP_ECONET
	//printk("proto:%x set mac %02x:%02x:%02x:%02x:%02x:%02x use acnt%d mask %d,dir:%d\n", fe_get_ip_proto(skb), devMacAddr[0], 
	//		devMacAddr[1],devMacAddr[2],devMacAddr[3],devMacAddr[4],devMacAddr[5], acnt_type, mask, dir);

	if(fe_get_ip_proto(skb) != IPPROTO_UDP)
	{
		return 0;
	}

#endif /* CONFIG_IPT_STAT_POLL_TP_ECONET */
#endif /* CONFIG_TP_IMAGE */

	fe_set_resource_mark(skb, acnt_type, meter_type, mask);
    
    return 0;
}

int fe_remark_skb_from_wan_idx(struct sk_buff *skb, u8 wan_index, u8 dir)
{
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    QDMA_AcntType_T acnt_type = 3;
	QDMA_MeterType_T meter_type = 3;
	u8 is_multicast = 0;
	int mask = 0;	
	
    if( (NULL == skb) || (NULL == feResManage) )
        return -1 ;

	fe_get_acnt_group(&acnt_type, dir, WAN_ITF_MODE);
	fe_get_meter_group(&meter_type, dir, WAN_MODE);

	FE_DEBUG(FE_DBG_MSG, " wan_index %d dir %d\n", wan_index, dir);

	is_multicast = (wan_index >> 7) & 0x1;		// bit7 use for multicast flag
	wan_index &= ~(0x1<<7);						// bit0~6 use for wan_index

	FE_DEBUG(FE_DBG_MSG, " wan_index %d is_multicast %d\n", wan_index, is_multicast);

	if(wan_index > WAN_ITF_ACNT_NUM-1){
		printk("WAN index %d not in list !\n", wan_index);
		return -1;
	}else if(aewan_lan_mode){
		if(wan_index > WAN_ITF_ACNT_NUM/2-1)
			return -1;
		mask = 2*wan_index + is_multicast + 1;
		if(UP_STREAM==dir)
			mask = mask + WAN_ITF_ACNT_NUM/2;
	}
	else{
		mask = 2*wan_index + is_multicast + 1;  
	}

	FE_DEBUG(FE_DBG_MSG, "set wan %s %s use acnt%d acnt mask %d\n", 
		wan_itf_info[wan_index].wan_dev->name, is_multicast?"multicast":"unicast", acnt_type, mask);

	fe_set_resource_mark(skb, acnt_type, meter_type, mask);
	
    return 0;
}
void fe_get_set_wan_idx(struct sk_buff *skb, u8 dir){
    int wan_index = -1;
	
	if(!wan_remark_hook_enable)
		return;
	
    if(skb == NULL)
        return;
	
    if(TCSUPPORT_ACCOUNT_METER_V2_VAL){
    	FE_API_GET_WAN_ITF_INDEX(skb, &wan_index);
    	if(TCSUPPORT_HWNAT_WAN_ACCOUNT_VAL){
    		if(ra_sw_nat_set_wan_acntid_hook){
    			ra_sw_nat_set_wan_acntid_hook(skb, wan_index, dir);
    		}
    	}
    	return;
    }

	return;
}

int fe_remark_skb_from_llid(struct sk_buff *skb, u8 llid, u8 dir)
{
    FE_Meter_Mode_t mode = NULL_BIND_MODE;
    QDMA_AcntType_T acnt_type = 3;
	QDMA_MeterType_T meter_type = 3;
	int mask = 0;	
	
    if( (NULL == skb) || (NULL == feResManage) )
        return -1 ;

	if(2 != fe_account_mode)
		return -1;
	
	fe_get_acnt_group(&acnt_type, dir, LLID_MODE);

	FE_DEBUG(FE_DBG_MSG, " llid %d dir %d\n", llid, dir);

	if(llid > 7){
#ifndef CONFIG_TP_IMAGE
		/* hsgmii as ewan, do not print error log */
		printk("llid index %d not in list !\n", llid);
#endif /* CONFIG_TP_IMAGsE */
		return -1;
	}
	else
		mask = llid + 1;  //acnt0 1-8

	FE_DEBUG(FE_DBG_MSG, "set llid %d use acnt%d acnt mask %d\n", llid, acnt_type, mask);

	fe_set_resource_mark(skb, acnt_type, meter_type, mask);
	
    return 0;
}

void fe_account_meter_api_init(void)
{
	fe_resource_mark_meter_hook = fe_remark_skb_mark_from_traffic_mark;
	fe_resource_mark_acnt_hook = fe_remark_skb_from_dev_mac;
	fe_resource_mark_wan_idx_hook = fe_remark_skb_from_wan_idx;
  	fe_resource_get_set_wan_idx_hook=fe_get_set_wan_idx; 
	fe_resource_mark_llid_hook = fe_remark_skb_from_llid;

	return;
}

void fe_account_meter_api_deinit(void)
{
	rcu_assign_pointer(fe_resource_mark_meter_hook, NULL);
	rcu_assign_pointer(fe_resource_mark_acnt_hook, NULL);
	rcu_assign_pointer(fe_resource_mark_wan_idx_hook, NULL);
	rcu_assign_pointer(fe_resource_get_set_wan_idx_hook, NULL);
	rcu_assign_pointer(fe_resource_mark_llid_hook, NULL);
	return;
}


