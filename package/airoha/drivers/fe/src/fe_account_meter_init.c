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

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
Fe_Resource_Manage_T *feResManage = NULL;
struct  Fe_Ratelimit_IP_Info ip_info_up[IP_MAX_NUM];
struct  Fe_Ratelimit_IP_Info ip_info_dw[IP_MAX_NUM];
struct  Fe_Ratelimit_Itf_Info itf_info_up[IF_MAX_NUM];
struct  Fe_Ratelimit_Itf_Info itf_info_dw[IF_MAX_NUM];
int aewan_lan_mode =0;
extern int fe_account_mode;
/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/


int fe_disable_meter(FE_Dir_t dir, unchar meter_id)
{
    uint mainType_val=0;
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


    if(meter_id < RX_RING_NUM)
        return 0;
    
    if (dir == UP_STREAM)
        mainType_val = ECNT_QDMA_WAN;
    else
        mainType_val = ECNT_QDMA_LAN;
    
    rxTrtcmCfg.trtcmModule = INGRESS_TRTCM ;
    QDMA_API_GET_GENERAL_TRTCM_INIT(mainType_val, &rxTrtcmCfg);
    
    if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
        rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;
        rxRateLimitCfg.Index = meter_id;
        QDMA_API_GET_GENERAL_RATELIMIT_CFG(mainType_val, &rxRateLimitCfg);
        rxRateLimitCfg.MeterEn = GENERAL_METER_DISABLE;
        QDMA_API_SET_GENERAL_RATELIMIT_CFG(mainType_val, &rxRateLimitCfg);
        
        rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
        rxRateLimitSet.Index = meter_id;
        QDMA_API_GET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);    
        rxRateLimitSet.RateLimitValue = 0;
        QDMA_API_SET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);
        
    } else {
        rxTrtcmCbsPbsCfg.trtcmModule = INGRESS_TRTCM ;
        rxTrtcmCbsPbsCfg.Index = meter_id;
        QDMA_API_SET_GENERAL_TRTCM_CFG(mainType_val, &rxTrtcmCbsPbsCfg);
        rxTrtcmCbsPbsCfg.CbsMeterEn = GENERAL_METER_DISABLE;
        rxTrtcmCbsPbsCfg.PbsMeterEn = GENERAL_METER_DISABLE;
        QDMA_API_SET_GENERAL_TRTCM_CFG(mainType_val, &rxTrtcmCbsPbsCfg);
        
        rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
        rxTrtcmCbsPbsSet.Index = meter_id;
        QDMA_API_GET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);
        rxTrtcmCbsPbsSet.CbsParamValue = 0;
        rxTrtcmCbsPbsSet.PbsParamValue = 0;
        rxTrtcmCbsPbsSet.CbsBucketSize = 0;
        rxTrtcmCbsPbsSet.PbsBucketSize = 0;
        QDMA_API_SET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);
    }

    return 0;
}

int fe_add_meter(FE_Dir_t dir, FE_Meter_Mode_t mode, QDMA_MeterType_T meter_type)
{
    int idx = 0;
    QDMA_MeterManage_T meterManage;

    if(feResManage == NULL)
        return -1;
	
	memset(&meterManage, 0, sizeof(QDMA_MeterManage_T));

	meterManage.meter_type = meter_type;
	if((meter_type < METER0_TYPE) || (meter_type > METER2_TYPE)){
		printk("ERROR, Can not find meter group for mode %d!", mode);
		return -1;
	}
	
	FE_DEBUG(FE_DBG_MSG, "config %s meter %d with mode %d\n", dir?"QDMA_WAN":"QDMA_LAN", meter_type, mode);

    switch(mode){
        case OLT_CTL_MODE:
        /*init meter resource for Olt ctl, up and down stream*/
        for(idx = 0; idx < OLT_CTL_NUM; idx ++){
            if(dir == UP_STREAM) {
                strlcpy(meterManage.bind_func_name, "OLT_SET_UP", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);/*return 0 :normal return -1 :cannot get idx*/
                feResManage->Meter_OLT_UP[idx] = meterManage.meter_id;
                sprintf(feResManage->OLTNameUP[idx],"OLT_SET_%d",idx+1);
            } else {
                strlcpy(meterManage.bind_func_name, "OLT_SET_DOWN", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
                feResManage->Meter_OLT_DOWN[idx] = meterManage.meter_id;
                sprintf(feResManage->OLTNameDOWN[idx],"OLT_SET_%d",idx+1);
            }
        }
        break;
        
        case CAR_QOS_MODE:
        /*init meter resource for car mode,only for up stream*/
        for(idx = 0; idx < QOS_CAR_NUM; idx++){
            if(dir == UP_STREAM) {
                strlcpy(meterManage.bind_func_name, "QoS_CAR_Mode", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
                feResManage->CAR_QOS_QUEUE_UP[idx] = meterManage.meter_id;
                sprintf(feResManage->CAR_QOS_QUEUE_NAME_UP[idx],"Queue_%d",idx+1);
            } else {
                strlcpy(meterManage.bind_func_name, "QoS_CAR_Mode_DOWN", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
                feResManage->CAR_QOS_QUEUE_DOWN[idx] = meterManage.meter_id;
                sprintf(feResManage->CAR_QOS_QUEUE_NAME_DOWN[idx],"Queue_%d",idx+1);
            }
        }
        break;

        case TRAFFIC_QOS_MODE:
    	/*init meter resource for traffic qos mode ,both for up stream and dw stream*/
    	for(idx = 0; idx < TRAFFIC_QOS_NUM; idx++){
            if(dir == UP_STREAM) {
                strlcpy(meterManage.bind_func_name, "TRAFFIC_QOS_UP", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
                feResManage->TRAFFIC_QOS_UP[idx] = meterManage.meter_id;
                sprintf(feResManage->TRAFFIC_QOS_Name_UP[idx],"TRAFFIC_QOS_%d",idx+1);
            } else {
        		strlcpy(meterManage.bind_func_name, "TRAFFIC_QOS_DW", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
                feResManage->TRAFFIC_QOS_DW[idx] = meterManage.meter_id;
                sprintf(feResManage->TRAFFIC_QOS_Name_DW[idx],"TRAFFIC_QOS_%d",idx+1);
            }
        }
        break;

        case IF_BIND_MODE:
        /*init meter resource for IF based, up and down stream*/
        for(idx = 0; idx < IF_MAX_NUM; idx ++){
            if(dir == UP_STREAM) {
                strlcpy(meterManage.bind_func_name, "IF_BASED_UP", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
                feResManage->Meter_IF_UP[idx] = meterManage.meter_id;
                if(idx < 4)
                    sprintf(feResManage->IFNameUP[idx],"Lan_%d", idx+1);
                else
                    sprintf(feResManage->IFNameUP[idx],"SSID_%d", idx-3);
            } else {
                strlcpy(meterManage.bind_func_name, "IF_BASED_DOWN", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
                feResManage->Meter_IF_DOWN[idx] = meterManage.meter_id;
                if(idx < 4)
                    sprintf(feResManage->IFNameDOWN[idx],"Lan_%d", idx+1);
                else
                    sprintf(feResManage->IFNameDOWN[idx],"SSID_%d", idx-3);
            }
        }
        break;
        
        case IP_BIND_MODE:
        /*init meter resource for IP based, up and down stream*/
        for(idx = 0; idx < IP_MAX_NUM; idx ++){
            if(dir == UP_STREAM) {
                strlcpy(meterManage.bind_func_name, "IP_BASED_UP", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
                feResManage->Meter_IP_UP[idx] = meterManage.meter_id;
                sprintf(feResManage->IPNameUP[idx], "IP_range_%d", idx+1);
            } else {
                strlcpy(meterManage.bind_func_name, "IP_BASED_DOWN", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
                feResManage->Meter_IP_DOWN[idx] = meterManage.meter_id;
                sprintf(feResManage->IPNameDOWN[idx], "IP_range_%d", idx+1);
            }
        }
        break;

        case DEV_MAC_MODE:
    	for(idx = 0; idx < UNDER_DEV_MAX_NUM; idx ++){
            if(dir == UP_STREAM) {
        		strlcpy(meterManage.bind_func_name, "DEV_MAC_UP", NAME_LENGTH);
        		QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
        		feResManage->Meter_DEV_FLOW_UP[idx] = meterManage.meter_id;
            } else {
        		strlcpy(meterManage.bind_func_name, "DEV_MAC_DW", NAME_LENGTH);
        		QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
        		feResManage->Meter_DEV_FLOW_DW[idx] = meterManage.meter_id;
            }
        }
        break;

        case FORCEBW_WRR_MODE:
        /*init meter resource for qos force bandwidth wrr mode,only for up stream*/
        for(idx = 0; idx < FORCEBW_WRR_NUM; idx++){
            if(dir == UP_STREAM) {
                strlcpy(meterManage.bind_func_name, "QoS_FORCE_WRR", NAME_LENGTH);
                QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
                feResManage->FORCEBW_WRR_QUEUE_UP[idx] = meterManage.meter_id;
                sprintf(feResManage->FORCEBW_WRR_QUEUE_NAME_UP[idx],"Queue_%d",idx+1);
            }
        }
        break;
		
        case WAN_MODE:
    	for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            if(dir == UP_STREAM) {
                strlcpy(meterManage.bind_func_name, "WAN_ITF_UP", NAME_LENGTH);
        		QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
        		feResManage->Meter_WAN_FLOW_UP[idx] = meterManage.meter_id;
            } else {
                strlcpy(meterManage.bind_func_name, "WAN_ITF_DOWN", NAME_LENGTH);
        		QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
        		feResManage->Meter_WAN_FLOW_DW[idx] = meterManage.meter_id;
            }
        }
        break;  
		
        default:
        break;
    }
    
    return 0;
}

int fe_remove_meter(FE_Dir_t dir, FE_Meter_Mode_t mode, QDMA_MeterType_T meter_type)
{
    int idx = 0;
    int meter_idx = 0x7F;
	
    if(feResManage == NULL)
        return -1;
	
	if((meter_type < METER0_TYPE) || (meter_type > METER2_TYPE)){
		printk("ERROR, Can not find meter group %d for mode %d!\n", meter_type, mode);
		return -1;
	}
	
	FE_DEBUG(FE_DBG_MSG, "remove %s meter %d with mode %d\n", dir?"QDMA_WAN":"QDMA_LAN", meter_type, mode);
   
    switch(mode){
        case OLT_CTL_MODE:
        for(idx = 0; idx < OLT_CTL_NUM; idx ++){
            if(dir == UP_STREAM) {
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->Meter_OLT_UP[idx]);
                fe_disable_meter(dir, feResManage->Meter_OLT_UP[idx]);
                feResManage->Meter_OLT_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->Meter_OLT_DOWN[idx]);
                fe_disable_meter(dir, feResManage->Meter_OLT_DOWN[idx]);
                feResManage->Meter_OLT_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;
        
        case CAR_QOS_MODE:
        for(idx = 0; idx < QOS_CAR_NUM; idx++){
            if(dir == UP_STREAM) {
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->CAR_QOS_QUEUE_UP[idx]);
                fe_disable_meter(dir, feResManage->CAR_QOS_QUEUE_UP[idx]);
                feResManage->CAR_QOS_QUEUE_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->CAR_QOS_QUEUE_DOWN[idx]);
                fe_disable_meter(dir, feResManage->CAR_QOS_QUEUE_DOWN[idx]);
                feResManage->CAR_QOS_QUEUE_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;

        case TRAFFIC_QOS_MODE:
    	for(idx = 0; idx < TRAFFIC_QOS_NUM; idx++){ 	
            if(dir == UP_STREAM) {
                meter_idx = (((meter_type & 0x3) << 8) | feResManage->TRAFFIC_QOS_UP[idx]);
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, meter_idx);
                fe_disable_meter(dir, meter_idx);
                feResManage->TRAFFIC_QOS_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                meter_idx = (((meter_type & 0x3) << 8) | feResManage->TRAFFIC_QOS_DW[idx]);
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, meter_idx);
                fe_disable_meter(dir, meter_idx);
                feResManage->TRAFFIC_QOS_DW[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
    	}
        break;

        case IF_BIND_MODE:
        for(idx = 0; idx < IF_MAX_NUM; idx ++){
            if(dir == UP_STREAM) {
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->Meter_IF_UP[idx]);
                fe_disable_meter(dir, feResManage->Meter_IF_UP[idx]);
                feResManage->Meter_IF_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->Meter_IF_DOWN[idx]);
                fe_disable_meter(dir, feResManage->Meter_IF_DOWN[idx]);
                feResManage->Meter_IF_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;
        
        case IP_BIND_MODE:
        for(idx = 0; idx < IP_MAX_NUM; idx ++){
            if(dir == UP_STREAM) {
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->Meter_IP_UP[idx]);
                fe_disable_meter(dir, feResManage->Meter_IP_UP[idx]);
                feResManage->Meter_IP_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->Meter_IP_DOWN[idx]);
                fe_disable_meter(dir, feResManage->Meter_IP_DOWN[idx]);
                feResManage->Meter_IP_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;

        case DEV_MAC_MODE:
        for(idx = 0; idx < UNDER_DEV_MAX_NUM; idx ++){
            if(dir == UP_STREAM) {
                meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_DEV_FLOW_UP[idx]);
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, meter_idx);
                fe_disable_meter(dir, meter_idx);
                feResManage->Meter_DEV_FLOW_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_DEV_FLOW_DW[idx]);
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, meter_idx);
                fe_disable_meter(dir, meter_idx);
                feResManage->Meter_DEV_FLOW_DW[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;

        case FORCEBW_WRR_MODE:
        for(idx = 0; idx < FORCEBW_WRR_NUM; idx++){
            if(dir == UP_STREAM) {
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->FORCEBW_WRR_QUEUE_UP[idx]);
                fe_disable_meter(dir, feResManage->FORCEBW_WRR_QUEUE_UP[idx]);
                feResManage->FORCEBW_WRR_QUEUE_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;

        case WAN_MODE:
    	for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            if(dir == UP_STREAM) {
                meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_WAN_FLOW_UP[idx]);
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, meter_idx);
                fe_disable_meter(dir, meter_idx);
                feResManage->Meter_WAN_FLOW_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_WAN_FLOW_DW[idx]);
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, meter_idx);
                fe_disable_meter(dir, meter_idx);
                feResManage->Meter_WAN_FLOW_DW[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;
		
        default:
        break;
    }
    
    return 0;
}
 
int fe_init_acnt_resource_from_mode(int dir, FE_ACNT_Mode_t acnt_mode, QDMA_AcntType_T acnt_type)
{ 
    int idx = 0;
    QDMA_AcntManage_T  acntManage;

    if(feResManage == NULL)
        return 0;
	
    if((acnt_type < ACNT0_TYPE) || (acnt_type > ACNT2_TYPE))
    {
    	printk("acnt_type is error!");
        return 0;
	}
	else
		acntManage.acnt_type = acnt_type;

	FE_DEBUG(FE_DBG_MSG, "acnt_mode is %d, acnt_type is %d, dir is %d!", acnt_mode, acnt_type, dir);
	
	if(acnt_mode == TRAFFIC_FLOW_MODE){
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx++){
	        if(dir == UP_STREAM){
	            strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_UP", NAME_LENGTH);
	            QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_LAN, &acntManage);
	            feResManage->ACNT_TRAFFIC_FLOW_UP[acnt_type][idx] = acntManage.acnt_id;
	            sprintf(feResManage->ACNT_TRAFFIC_FLOW_UP_NAME[acnt_type][idx], "TRAFFIC_FLOW_%d", idx);
	        }else if(dir == DOWN_STREAM){
	            strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_DW", NAME_LENGTH);
	            QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_WAN, &acntManage);
	            feResManage->ACNT_TRAFFIC_FLOW_DOWN[acnt_type][idx] = acntManage.acnt_id;  
	            sprintf(feResManage->ACNT_TRAFFIC_FLOW_DOWN_NAME[acnt_type][idx], "TRAFFIC_FLOW_%d", idx);
	        }
    	}
	}else if(acnt_mode == LLID_MODE){
        for(idx = 0; idx < LLID_MAX_NUM; idx++){
	        if(dir == UP_STREAM){
	            strlcpy(acntManage.bind_func_name, "LLID_UP", NAME_LENGTH);
	            QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_LAN, &acntManage);
	            feResManage->ACNT_LLID_UP[acnt_type][idx] = acntManage.acnt_id;	
	            sprintf(feResManage->ACNT_LLID_UP_NAME[acnt_type][idx], "LLID_%d", idx);
			}else if(dir == DOWN_STREAM){
	            strlcpy(acntManage.bind_func_name, "LLID_DW", NAME_LENGTH);
	            QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_WAN, &acntManage);
	            feResManage->ACNT_LLID_DOWN[acnt_type][idx] = acntManage.acnt_id;  
	            sprintf(feResManage->ACNT_LLID_DOWN_NAME[acnt_type][idx], "LLID_%d", idx);
	        }
    	}
	}else if(acnt_mode == DEV_MODE){
        for(idx = 0; idx < UNDER_DEV_MAX_NUM; idx++){
            if(dir == UP_STREAM){ 
                strlcpy(acntManage.bind_func_name, "DEV_UP", NAME_LENGTH);
            	QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_LAN, &acntManage);
            	feResManage->ACNT_DEV_FLOW_UP[acnt_type][idx] = acntManage.acnt_id;
            }else if(dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "DEV_DW", NAME_LENGTH);
            	QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_WAN, &acntManage);
            	feResManage->ACNT_DEV_FLOW_DOWN[acnt_type][idx] = acntManage.acnt_id;
			}
        }
    }else if(acnt_mode == WAN_ITF_MODE){
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx++){
            if(dir == UP_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_UP", NAME_LENGTH);    
                QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_LAN, &acntManage);
                feResManage->ACNT_WAN_ITF_UP[acnt_type][idx] = acntManage.acnt_id;
            }else if(dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_DW", NAME_LENGTH);     
                QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_WAN, &acntManage);
                feResManage->ACNT_WAN_ITF_DOWN[acnt_type][idx] = acntManage.acnt_id; 
            }
        } 
    }

    return 0;
}

int fe_deinit_acnt_resource_from_mode(int dir,  FE_ACNT_Mode_t acnt_mode, QDMA_AcntType_T acnt_type)
{ 
    int idx = 0;
    AcntManage_T  acntManage;

    if(feResManage == NULL)
        return 0;
	
    if((acnt_type < ACNT0_TYPE) || (acnt_type > ACNT2_TYPE))
    {
    	printk("acnt_type is error!");
        return 0;
	}
	else	
		acntManage.acnt_type = acnt_type;

	FE_DEBUG(FE_DBG_MSG, "acnt_mode is %d, acnt_type is %d, dir is %d!", acnt_mode, acnt_type, dir);
	
	if(acnt_mode == TRAFFIC_FLOW_MODE){
		for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx++){
		    if(dir == UP_STREAM){
		        acntManage.acnt_id = feResManage->ACNT_TRAFFIC_FLOW_UP[acnt_type][idx];
		        QDMA_API_FREE_ACNT(ECNT_QDMA_LAN, &acntManage);
		        feResManage->ACNT_TRAFFIC_FLOW_UP[acnt_type][idx] = ACNT2_DEFAULT;
		    }else if(dir == DOWN_STREAM){
		        acntManage.acnt_id = feResManage->ACNT_TRAFFIC_FLOW_DOWN[acnt_type][idx]; 
		        QDMA_API_FREE_ACNT(ECNT_QDMA_WAN, &acntManage);
		        feResManage->ACNT_TRAFFIC_FLOW_DOWN[acnt_type][idx] = ACNT2_DEFAULT;
		    }
		}
	}else if(acnt_mode == LLID_MODE){
	    for(idx = 0; idx < LLID_MAX_NUM; idx++){
			if(dir == UP_STREAM){
				acntManage.acnt_id = feResManage->ACNT_LLID_UP[acnt_type][idx];
		        QDMA_API_FREE_ACNT(ECNT_QDMA_LAN, &acntManage);
				feResManage->ACNT_LLID_UP[acnt_type][idx] = ACNT2_DEFAULT;
			}else if(dir == DOWN_STREAM){
				acntManage.acnt_id = feResManage->ACNT_LLID_DOWN[acnt_type][idx];
		        QDMA_API_FREE_ACNT(ECNT_QDMA_WAN, &acntManage);
				feResManage->ACNT_LLID_DOWN[acnt_type][idx] = ACNT2_DEFAULT;
			}
	    }
	}else if(acnt_mode == DEV_MODE){
	    for(idx = 0; idx < UNDER_DEV_MAX_NUM; idx++){
			if(dir == UP_STREAM){
				acntManage.acnt_id = feResManage->ACNT_DEV_FLOW_UP[acnt_type][idx];
		        QDMA_API_FREE_ACNT(ECNT_QDMA_LAN, &acntManage);
				feResManage->ACNT_DEV_FLOW_UP[acnt_type][idx] = ACNT2_DEFAULT;
			}else if(dir == DOWN_STREAM){
				acntManage.acnt_id = feResManage->ACNT_DEV_FLOW_DOWN[acnt_type][idx];
		        QDMA_API_FREE_ACNT(ECNT_QDMA_WAN, &acntManage);
				feResManage->ACNT_DEV_FLOW_DOWN[acnt_type][idx] = ACNT2_DEFAULT;
			}
	    }
	}else if(acnt_mode == WAN_ITF_MODE){
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx++){
            if(dir == UP_STREAM){
                acntManage.acnt_id = feResManage->ACNT_WAN_ITF_UP[acnt_type][idx]; 
                QDMA_API_FREE_ACNT(ECNT_QDMA_LAN, &acntManage);
                feResManage->ACNT_WAN_ITF_UP[acnt_type][idx] = ACNT2_DEFAULT;
            }else if(dir == DOWN_STREAM){
                acntManage.acnt_id = feResManage->ACNT_WAN_ITF_DOWN[acnt_type][idx]; 
                QDMA_API_FREE_ACNT(ECNT_QDMA_WAN, &acntManage);
                feResManage->ACNT_WAN_ITF_DOWN[acnt_type][idx] = ACNT2_DEFAULT;
            }
        }
	}

    return 0;
}

void fe_account_meter_init_by_chip(void)
{
	/*****************fe_resource_mark bitmap format******************************/
	/*reserve			acnt1&meter2		acnt0&meter1	acnt2&meter0
	/*00000000			0000000 			00000000		000000000*/
	/*****************fe_resource_mark format*************************************/

	if(isAN7552){
		feResManage->acnt_up_mode[1] = WAN_ITF_MODE;
		feResManage->acnt_dw_mode[1] = WAN_ITF_MODE;
		if(fe_account_mode == 1){
			feResManage->acnt_up_mode[0] = TRAFFIC_FLOW_MODE;
			feResManage->acnt_dw_mode[0] = TRAFFIC_FLOW_MODE;
		}else if(fe_account_mode == 2){
			feResManage->acnt_up_mode[0] = LLID_MODE;
			feResManage->acnt_dw_mode[0] = LLID_MODE;
		}
		feResManage->acnt_up_mode[2] = DEV_MODE;
		feResManage->acnt_dw_mode[2] = DEV_MODE;
		feResManage->up_stream_mode[2] = WAN_MODE;
		feResManage->dw_stream_mode[2] = WAN_MODE;
		feResManage->up_stream_mode[1] = TRAFFIC_QOS_MODE;
		feResManage->dw_stream_mode[1] = TRAFFIC_QOS_MODE;
		feResManage->up_stream_mode[0] = DEV_MAC_MODE;
		feResManage->dw_stream_mode[0] = DEV_MAC_MODE;
	}else if(isEN7581 || isAN7583){
		feResManage->acnt_up_mode[1] = WAN_ITF_MODE;
		feResManage->acnt_dw_mode[1] = WAN_ITF_MODE;
		if(fe_account_mode == 1){
			feResManage->acnt_up_mode[0] = TRAFFIC_FLOW_MODE;
			feResManage->acnt_dw_mode[0] = TRAFFIC_FLOW_MODE;
		}else if(fe_account_mode == 2){
			feResManage->acnt_up_mode[0] = LLID_MODE;
			feResManage->acnt_dw_mode[0] = LLID_MODE;
		}
		feResManage->acnt_up_mode[2] = DEV_MODE;
		feResManage->acnt_dw_mode[2] = DEV_MODE;
		feResManage->up_stream_mode[2] = WAN_MODE;
		feResManage->dw_stream_mode[2] = WAN_MODE;
		feResManage->up_stream_mode[1] = TRAFFIC_QOS_MODE;
		feResManage->dw_stream_mode[1] = TRAFFIC_QOS_MODE;
		feResManage->up_stream_mode[0] = DEV_MAC_MODE;
		feResManage->dw_stream_mode[0] = DEV_MAC_MODE;
	}else if(isEN7523){
		feResManage->acnt_up_mode[0] = DEV_MODE;
		feResManage->acnt_dw_mode[0] = DEV_MODE;
		if(fe_account_mode == 1){
			feResManage->acnt_up_mode[1] = TRAFFIC_FLOW_MODE;
			feResManage->acnt_dw_mode[1] = TRAFFIC_FLOW_MODE;
		}else if(fe_account_mode == 2){
			feResManage->acnt_up_mode[1] = LLID_MODE;
			feResManage->acnt_dw_mode[1] = LLID_MODE;
		}
		feResManage->acnt_up_mode[2] = WAN_ITF_MODE;
		feResManage->acnt_dw_mode[2] = WAN_ITF_MODE;
		feResManage->up_stream_mode[0] = DEV_MAC_MODE;
		feResManage->dw_stream_mode[0] = DEV_MAC_MODE;
		feResManage->up_stream_mode[1] = NULL_BIND_MODE;
		feResManage->dw_stream_mode[1] = NULL_BIND_MODE;
		feResManage->up_stream_mode[2] = NULL_BIND_MODE;
		feResManage->dw_stream_mode[2] = NULL_BIND_MODE;
	}else if(isEN7580){
		feResManage->acnt_up_mode[1] = WAN_ITF_MODE;
		feResManage->acnt_dw_mode[1] = WAN_ITF_MODE;
		if(fe_account_mode == 1){
			feResManage->acnt_up_mode[0] = TRAFFIC_FLOW_MODE;
			feResManage->acnt_dw_mode[0] = TRAFFIC_FLOW_MODE;
		}else if(fe_account_mode == 2){
			feResManage->acnt_up_mode[0] = LLID_MODE;
			feResManage->acnt_dw_mode[0] = LLID_MODE;
		}
		feResManage->acnt_up_mode[2] = DEV_MODE;
		feResManage->acnt_dw_mode[2] = DEV_MODE;
		feResManage->up_stream_mode[2] = NULL_BIND_MODE;
		feResManage->dw_stream_mode[2] = NULL_BIND_MODE;
		feResManage->up_stream_mode[1] = NULL_BIND_MODE;
		feResManage->dw_stream_mode[1] = NULL_BIND_MODE;
		feResManage->up_stream_mode[0] = DEV_MAC_MODE;
		feResManage->dw_stream_mode[0] = DEV_MAC_MODE;
	}
	
	fe_init_acnt_resource_from_mode(UP_STREAM, feResManage->acnt_up_mode[0], ACNT0_TYPE);
	fe_init_acnt_resource_from_mode(DOWN_STREAM, feResManage->acnt_dw_mode[0], ACNT0_TYPE);
	fe_init_acnt_resource_from_mode(UP_STREAM, feResManage->acnt_up_mode[1], ACNT1_TYPE);
	fe_init_acnt_resource_from_mode(DOWN_STREAM, feResManage->acnt_dw_mode[1], ACNT1_TYPE);
	fe_init_acnt_resource_from_mode(UP_STREAM, feResManage->acnt_up_mode[2], ACNT2_TYPE);
	fe_init_acnt_resource_from_mode(DOWN_STREAM, feResManage->acnt_dw_mode[2], ACNT2_TYPE);
	fe_add_meter(UP_STREAM, feResManage->up_stream_mode[2], METER2_TYPE);
	fe_add_meter(DOWN_STREAM, feResManage->dw_stream_mode[2], METER2_TYPE);
	fe_add_meter(UP_STREAM, feResManage->up_stream_mode[1], METER1_TYPE);
	fe_add_meter(DOWN_STREAM, feResManage->dw_stream_mode[1], METER1_TYPE);
	fe_add_meter(UP_STREAM, feResManage->up_stream_mode[0], METER0_TYPE);
	fe_add_meter(DOWN_STREAM, feResManage->dw_stream_mode[0], METER0_TYPE);
		
	return;
}

int fe_api_init_resource_manage(struct ecnt_fe_data *fe_data)
{
    int idx = 0;
    QDMA_MeterManage_T meterManage;
    QDMA_AcntManage_T  acntManage;
    GENERAL_TrtcmCfg_T rxTrtcmCfg; 
    uint mainType_val=0;
    FE_Dir_t dir;
    
    printk("fe resource init start v2\n");
#if SUPPORT_AEWAN_LAN_MODE
	if(PON_LAN_VALID)
	{
		aewan_lan_mode=1;
	}
#endif	
	if(NULL == feResManage){
		feResManage = (Fe_Resource_Manage_T *)kzalloc(sizeof(Fe_Resource_Manage_T), GFP_ATOMIC);
		if(feResManage == NULL){
			printk("fe resource init fail\n");
			return -1;
		}
		memset(feResManage, 0x0, sizeof(Fe_Resource_Manage_T));
	}
    
    memset(&meterManage, 0, sizeof(QDMA_MeterManage_T)) ;
    memset(&acntManage, 0, sizeof(QDMA_AcntManage_T)) ;
    memset(&rxTrtcmCfg, 0, sizeof(GENERAL_TrtcmCfg_T)) ;
    memset(ip_info_up, 0x0, sizeof(ip_info_up));
    memset(ip_info_dw, 0x0, sizeof(ip_info_dw));
    memset(itf_info_up, 0x0, sizeof(itf_info_up));
    memset(itf_info_dw, 0x0, sizeof(itf_info_dw));

	feResManage->dbgLevel = 0;
    feResManage->Meter_Ctl_By_Olt = 0;

    for(dir=0; dir<2; dir++) {
        if (dir == UP_STREAM)
            mainType_val = ECNT_QDMA_WAN;
        else
            mainType_val = ECNT_QDMA_LAN;
        
        rxTrtcmCfg.trtcmModule = INGRESS_TRTCM ;
        QDMA_API_GET_GENERAL_TRTCM_INIT(mainType_val, &rxTrtcmCfg);
        feResManage->ingress_current_trtcm_mode[dir] = rxTrtcmCfg.trtcmMode;
        if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
            feResManage->ingress_ratelimit_trtcm_invalid_index[dir] = GENERAL_INGRESS_NO_RATELIMIT_INDEX;
        } else {
            feResManage->ingress_ratelimit_trtcm_invalid_index[dir] = GENERAL_INGRESS_NO_TRTCM_INDEX;
        }
    }

    /*init meter resource for Olt ctl, up and down stream*/
    for(idx = 0; idx < OLT_CTL_NUM; idx ++){
        feResManage->Meter_OLT_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];
        sprintf(feResManage->OLTNameUP[idx],"OLT_SET_%d",idx+1);

        feResManage->Meter_OLT_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[DOWN_STREAM];
        sprintf(feResManage->OLTNameDOWN[idx],"OLT_SET_%d",idx+1);
    }
    /*init meter resource for car mode,only for up stream*/
    for(idx = 0; idx < QOS_CAR_NUM; idx++){
        feResManage->CAR_QOS_QUEUE_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];
        sprintf(feResManage->CAR_QOS_QUEUE_NAME_UP[idx],"Queue_%d",idx+1);
        
        feResManage->CAR_QOS_QUEUE_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[DOWN_STREAM];
        sprintf(feResManage->CAR_QOS_QUEUE_NAME_DOWN[idx],"Queue_%d",idx+1);
    }

    /*init meter resource for traffic qos mode ,both for up stream and dw stream*/
    for(idx = 0; idx < TRAFFIC_QOS_NUM; idx++){
        feResManage->TRAFFIC_QOS_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];
        sprintf(feResManage->TRAFFIC_QOS_Name_UP[idx],"TRAFFIC_QOS_%d",idx+1);
        
        feResManage->TRAFFIC_QOS_DW[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[DOWN_STREAM];
        sprintf(feResManage->TRAFFIC_QOS_Name_DW[idx],"TRAFFIC_QOS_%d",idx+1);
    }

    /*init meter resource for IF based, up and down stream*/
    for(idx = 0; idx < IF_MAX_NUM; idx ++){
        feResManage->Meter_IF_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];
        if(idx < 4)
            sprintf(feResManage->IFNameUP[idx],"Lan_%d", idx+1);
        else
            sprintf(feResManage->IFNameUP[idx],"SSID_%d", idx-3);

        feResManage->Meter_IF_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[DOWN_STREAM];
        if(idx < 4)
            sprintf(feResManage->IFNameDOWN[idx],"Lan_%d", idx+1);
        else
            sprintf(feResManage->IFNameDOWN[idx],"SSID_%d", idx-3);
    }
    /*init meter resource for IP based, up and down stream*/
    for(idx = 0; idx < IP_MAX_NUM; idx ++){
        feResManage->Meter_IP_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];
        sprintf(feResManage->IPNameUP[idx], "IP_range_%d", idx+1);

        feResManage->Meter_IP_DOWN[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[DOWN_STREAM];
        sprintf(feResManage->IPNameDOWN[idx], "IP_range_%d", idx+1);
    }

    for(idx = 0; idx < UNDER_DEV_MAX_NUM; idx ++){
        feResManage->Meter_DEV_FLOW_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];

        feResManage->Meter_DEV_FLOW_DW[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[DOWN_STREAM];
    }

    /*init meter resource for qos force bandwidth wrr mode,only for up stream*/
    for(idx = 0; idx < FORCEBW_WRR_NUM; idx++){
        feResManage->FORCEBW_WRR_QUEUE_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];
        sprintf(feResManage->FORCEBW_WRR_QUEUE_NAME_UP[idx],"Queue_%d",idx+1);
    }

    for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
        feResManage->Meter_WAN_FLOW_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[UP_STREAM];

        feResManage->Meter_WAN_FLOW_DW[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[DOWN_STREAM];
    }
	
	printk("fe resource init account & meter!\n");

	fe_account_meter_init_by_chip();

	fe_account_meter_api_init();

    return 0;

}


int fe_api_deinit_resource_manage(struct ecnt_fe_data *fe_data)
{
    int idx = 0;
    AcntManage_T  acntManage;
    memset(&acntManage, 0, sizeof(AcntManage_T)) ;

    if(feResManage == NULL)
        return -1;
    
    fe_remove_meter(DOWN_STREAM, feResManage->dw_stream_mode[0], METER0_TYPE);
    fe_remove_meter(UP_STREAM, feResManage->up_stream_mode[0], METER0_TYPE);
    fe_remove_meter(DOWN_STREAM, feResManage->dw_stream_mode[1], METER1_TYPE);
    fe_remove_meter(UP_STREAM, feResManage->up_stream_mode[1], METER1_TYPE);
	fe_remove_meter(DOWN_STREAM, feResManage->dw_stream_mode[2], METER2_TYPE);
    fe_remove_meter(UP_STREAM, feResManage->up_stream_mode[2], METER2_TYPE);
	
	fe_deinit_acnt_resource_from_mode(UP_STREAM, feResManage->acnt_up_mode[0], ACNT0_TYPE);
	fe_deinit_acnt_resource_from_mode(DOWN_STREAM, feResManage->acnt_dw_mode[0], ACNT0_TYPE);
	fe_deinit_acnt_resource_from_mode(UP_STREAM, feResManage->acnt_up_mode[1], ACNT1_TYPE);
	fe_deinit_acnt_resource_from_mode(DOWN_STREAM, feResManage->acnt_dw_mode[1], ACNT1_TYPE);
	fe_deinit_acnt_resource_from_mode(UP_STREAM, feResManage->acnt_up_mode[2], ACNT2_TYPE);
	fe_deinit_acnt_resource_from_mode(DOWN_STREAM, feResManage->acnt_dw_mode[2], ACNT2_TYPE);

	fe_account_meter_api_deinit();

    return 0;
}

