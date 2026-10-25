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
  

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define 	IP_VERSION_V4			0
#define 	IP_VERSION_V6			1

/*For CDS BC/MC ratelimit end*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern Fe_Resource_Manage_T *feResManage;
extern struct devBandwidthList_s *gHwBandwidthList;
extern struct devBandwidthList_s *gDevBandwidthList;
extern int traffic_flow_filter;
extern int traffic_flow_shift;
extern struct  Fe_Ratelimit_IP_Info ip_info_up[IP_MAX_NUM];
extern struct  Fe_Ratelimit_IP_Info ip_info_dw[IP_MAX_NUM];
extern struct  Fe_Ratelimit_Itf_Info itf_info_up[IF_MAX_NUM];
extern struct  Fe_Ratelimit_Itf_Info itf_info_dw[IF_MAX_NUM];
extern Wan_itf_name wan_itf_info[WAN_ITF_ACNT_NUM];
extern int aewan_lan_mode;
extern int wan_remark_hook_enable;
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
static int default_wan_itf=ACNT1_DEFAULT;
extern wan_virtualdev_ifname wan_virtualdev_ifname_info[16];
static int update_mul_cnt_en = 0;

int fe_account_mode = 1; // 1 for flow acnt, 2 for llid acnt

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
 
static int fe_ratelimit_dw_info_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int i = 0 ;
    int j = 0 ;
    
    printk("TrafficRatelimitInfoDisplay\n");

    if(feResManage == NULL)
        return 0;
    
    printk("\nInterface Info: %s\n", (feResManage->dw_stream_mode[0] == IF_BIND_MODE)? "ENABLE":"DISABLE");
    printk("Interface-Name\tActive\tRate\n");
    for( i  = 0 ; i < IF_MAX_NUM ; i++ )
    {
        if( i < 4 )
        {
            printk("LAN%d\t\t%s\t%d Kbps\n\r",
                (i+1) ,
                itf_info_dw[i].enable?"Yes":"No",
                itf_info_dw[i].rate);
        }
        else
        {
            printk("SSID%d\t\t%s\t%d Kbps\n\r",
                (i-3) ,
                itf_info_dw[i].enable?"Yes":"No",
                itf_info_dw[i].rate);
        }
    }

    printk("\nIP range Info: %s\n", (feResManage->dw_stream_mode[0] == IP_BIND_MODE)? "ENABLE":"DISABLE");
    printk("No\tActive\tRate\tMode\tIP range\n");
    for( i  = 0 ; i < IP_MAX_NUM ; i++ )
    {
        printk("%d\t%s\t%d Kbps\t%s\t",
                (i+1) ,
                ip_info_dw[i].enable?"Yes":"No",
                ip_info_dw[i].rate,
                ip_info_dw[i].ip_version?"IPv6":"IPv4");
        if( IP_VERSION_V6 == ip_info_dw[i].ip_version )/*IPv6*/
        {
            for( j = 0 ; j < 16 ; )
            {
                printk("%02X%02X",ip_info_dw[i].v6_pair_l[j],ip_info_dw[i].v6_pair_l[j+1]);
                j = j + 2 ;
                if( j < 16 )
                    printk(":");
            }
            printk("~");
            for( j = 0 ; j < 16 ; )
            {
                printk("%02X%02X",ip_info_dw[i].v6_pair_h[j],ip_info_dw[i].v6_pair_h[j+1]);
                j = j + 2 ;
                if( j < 16 )
                    printk(":");
            }
        }
        else if( IP_VERSION_V4 == ip_info_dw[i].ip_version )/*IPv4*/
        {
            for( j = 0 ; j < 4 ; j ++ )
            {
                printk("%d",ip_info_dw[i].v4_pair_l[j]);
                if( j < 3 )
                    printk(".");
            }
            printk("~");
            for( j = 0 ; j < 4 ; j ++ )
            {
                printk("%d",ip_info_dw[i].v4_pair_h[j]);
                if( j < 3 )
                    printk(".");
            }
        }
        printk("\n\r");
    }

    return 0;
}

static int fe_traffic_flow_mapping_state(QDMA_AcntType_T acnt_type)
{
    int idx = 0;
	int num = TRAFFIC_FLOW_MAX_NUM;
	if(aewan_lan_mode)
		num = num/2;
    printk("acnt[%d] mapping state: flow id and acnt id mapping relationship\n", acnt_type);

    printk("\nDownstream ACNT[%d] mapping state \n", acnt_type);

    if(feResManage == NULL)
        return 0;

    if(feResManage->acnt_dw_mode[acnt_type] == TRAFFIC_FLOW_MODE){
        printk("down stream ,acnt[%d] mode is traffic qos flow mode\n", acnt_type);
        printk("NUM\tTRAFFIC_FLOW_IDX\tMARK\tAcnt%d_id\n", acnt_type);
        for(idx = 0; idx < num; idx ++){
            printk("%d\t%s\t\t0x%x\t%d\n", idx, feResManage->ACNT_TRAFFIC_FLOW_DOWN_NAME[acnt_type][idx],
				(idx+1)<<traffic_flow_shift, feResManage->ACNT_TRAFFIC_FLOW_DOWN[acnt_type][idx]);
        }
    }
    
    printk("\nUpstream ACNT[%d] mapping state \n", acnt_type);
    if(feResManage->acnt_up_mode[acnt_type] == TRAFFIC_FLOW_MODE){
        printk("up stream ,acnt[%d] mode is traffic qos flow mode\n", acnt_type);
        printk("NUM\tTRAFFIC_FLOW_IDX\tMARK\tAcnt%d_id\n", acnt_type);
        for(idx = 0; idx < num; idx ++){
            printk("%d\t%s\t\t0x%x\t%d\n", idx, feResManage->ACNT_TRAFFIC_FLOW_UP_NAME[acnt_type][idx],
				(idx+1)<<traffic_flow_shift, feResManage->ACNT_TRAFFIC_FLOW_UP[acnt_type][idx]);
        }
    }
    
    return 0;

}

static int fe_llid_mapping_state(QDMA_AcntType_T acnt_type)
{
    int idx = 0;

    printk("acnt[%d] mapping state: llid id and acnt id mapping relationship\n", acnt_type);

    printk("\nDownstream ACNT[%d] mapping state \n", acnt_type);

    if(feResManage == NULL)
        return 0;

    if(feResManage->acnt_dw_mode[acnt_type] == LLID_MODE){
        printk("down stream ,acnt[%d] mode is llid mode\n", acnt_type);
        printk("NUM\tLLID_IDX\tMARK\tAcnt%d_id\n", acnt_type);
        for(idx = 0; idx < LLID_MAX_NUM; idx ++){
            printk("%d\t%s\t\t0x%x\t%d\n", idx, feResManage->ACNT_TRAFFIC_FLOW_DOWN_NAME[acnt_type][idx],
				(idx+1)<<traffic_flow_shift, feResManage->ACNT_TRAFFIC_FLOW_DOWN[acnt_type][idx]);
        }
    }
    
    printk("\nUpstream ACNT[%d] mapping state \n", acnt_type);
    if(feResManage->acnt_up_mode[acnt_type] == TRAFFIC_FLOW_MODE){
        printk("up stream ,acnt[%d] mode is traffic qos flow mode\n", acnt_type);
        printk("NUM\tTRAFFIC_FLOW_IDX\tMARK\tAcnt%d_id\n", acnt_type);
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            printk("%d\t%s\t\t%d\n", idx, feResManage->ACNT_LLID_UP_NAME[acnt_type][idx],feResManage->ACNT_LLID_UP[acnt_type][idx]);
        }
    }
    
    return 0;

}

static int fe_wan_itf_mapping_state(QDMA_AcntType_T acnt_type)
{
    int idx = 0;
	int num = WAN_ITF_ACNT_NUM;
	if(aewan_lan_mode)
		num = num/2;

    printk("acnt[%d] mapping state: wan_itf and acnt id mapping relationship\n", acnt_type);

    printk("\nDownstream ACNT[%d] mapping state \n", acnt_type);

    if(feResManage == NULL)
        return 0;

	if(feResManage->acnt_dw_mode[acnt_type] == WAN_ITF_MODE){
		printk("up stream ,acnt[%d] mode is WAN_ITF_MODE\n", acnt_type);
        printk("NUM\tWAN_ITF\tTYPE\t\tWAN_NAME\tAcnt%d_id\n", acnt_type);
        for(idx = 0; idx < num; idx ++){
            printk("%d\twan_%d %s\t\t%s\t\t%d\n", idx, idx/2, idx%2?"multicast":"unicast",
			wan_itf_info[idx/2].wan_dev->name,feResManage->ACNT_WAN_ITF_DOWN[acnt_type][idx]);
        }
    }
    
    printk("\nUpstream ACNT[%d] mapping state \n", acnt_type);
    if(feResManage->acnt_up_mode[acnt_type] == WAN_ITF_MODE){
		printk("up stream ,acnt[%d] mode is WAN_ITF_MODE\n", acnt_type);
        printk("NUM\tWAN_ITF\tTYPE\t\tWAN_NAME\tAcnt%d_id\n", acnt_type);
        for(idx = 0; idx < num; idx ++){
            printk("%d\twan_%d %s\t\t%s\t\t%d\n", idx, idx/2, idx%2?"multicast":"unicast" ,
			wan_itf_info[idx/2].wan_dev->name,feResManage->ACNT_WAN_ITF_UP[acnt_type][idx]);
        }
    }
    
    return 0;

}


int fe_dev_mac_mapping_state(QDMA_AcntType_T acnt_type)
{
    int idx = 0;
    struct devBandwidth_s *devBandWidthPtr = NULL;
	int num = UNDER_DEV_MAX_NUM;
	if(aewan_lan_mode)
		num = num/2;
    
    printk("acnt[%d] mapping state: dev mac and acnt id mapping relationship\n", acnt_type);
    printk("\nDownstream ACNT[%d] mapping state \n", acnt_type);
    printk("idx\tdev_name\tMAC\t\t\tAcnt%d_id\n", acnt_type);

    if(feResManage == NULL)
        return 0;

    if(gHwBandwidthList == NULL){
        printk("gHwBandwidthList is NULL return!\n");
        return 0;
    }
    
    for(idx = 0; idx < num; idx ++){
        devBandWidthPtr = &(gHwBandwidthList->bandwidthList[idx]);
        if(devBandWidthPtr != NULL)
            printk("%d\tMAC%d\t\t%02x:%02x:%02x:%02x:%02x:%02x\t%d\n", idx, idx, devBandWidthPtr->mac[0],
            devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],
            devBandWidthPtr->mac[4],devBandWidthPtr->mac[5],feResManage->ACNT_DEV_FLOW_DOWN[acnt_type][idx]);
    }
    
    printk("\nUpstream ACNT[%d] mapping state \n", acnt_type);
    printk("idx\tdev_name\tMAC\t\t\tAcnt%d_id\n", acnt_type);
    for(idx = 0; idx < num; idx ++){
        devBandWidthPtr = &(gHwBandwidthList->bandwidthList[idx]);
        if(devBandWidthPtr != NULL)
            printk("%d\tMAC%d\t\t%02x:%02x:%02x:%02x:%02x:%02x\t%d\n", idx, idx, devBandWidthPtr->mac[0],
            devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],
            devBandWidthPtr->mac[4],devBandWidthPtr->mac[5],feResManage->ACNT_DEV_FLOW_UP[acnt_type][idx]);
    }

    return 0;
}
 
static int fe_dump_one_meter(FE_Dir_t dir, int idx, int meter_idx, unsigned char *meter_name)
{
    int is_need_tab_flag = 0;
    uint mainType_val=0;
    int meter_type = -1;
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

    if(strlen(meter_name) < 8)
        is_need_tab_flag = 1;
    
    if (dir == UP_STREAM)
        mainType_val = ECNT_QDMA_WAN;
    else
        mainType_val = ECNT_QDMA_LAN;
    
    meter_type = (meter_idx >> 8) & 0x3;
	
    rxTrtcmCfg.trtcmModule = INGRESS_TRTCM ;
    QDMA_API_GET_GENERAL_TRTCM_INIT(mainType_val, &rxTrtcmCfg);

    if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
        /* if meter_idx below than RX_RING_NUM, means this meter is disable and value is 0 */
        if((meter_type == 0) && ((meter_idx < RX_RING_NUM) || (meter_idx >= GENERAL_INGRESS_NO_RATELIMIT_INDEX))) {
            rxTrtcmCbsPbsSet.Index = GENERAL_INGRESS_NO_RATELIMIT_INDEX;
            rxRateLimitCfg.MeterEn = GENERAL_METER_DISABLE;
            rxRateLimitSet.RateLimitValue = 0;
            rxRateLimitCfg.PktMode = TRTCM_BYTE_MODE;
        }else{
            rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;
            rxRateLimitCfg.Index = meter_idx;
            QDMA_API_GET_GENERAL_RATELIMIT_CFG(mainType_val, &rxRateLimitCfg);
        
            rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
            rxRateLimitSet.Index = meter_idx;
            QDMA_API_GET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);
        }
        
        printk("%d\t%s%s\t%d\t\t%s\t%d%s\n", idx, meter_name, (is_need_tab_flag==1)?("\t"):(""), meter_idx, 
        (rxRateLimitCfg.MeterEn == GENERAL_METER_ENABLE) ? ("Yes"):("No"),
        rxRateLimitSet.RateLimitValue,
        (rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
        );
    } else {        
        /* if meter_idx below than RX_RING_NUM, means this meter is disable and value is 0 */
        if((meter_type == 0) && ((meter_idx < RX_RING_NUM) || (meter_idx >= GENERAL_INGRESS_NO_TRTCM_INDEX))) {
            rxTrtcmCbsPbsSet.Index = GENERAL_INGRESS_NO_TRTCM_INDEX;
            rxTrtcmCbsPbsCfg.CbsMeterEn = GENERAL_METER_DISABLE;
            rxTrtcmCbsPbsCfg.PbsMeterEn = GENERAL_METER_DISABLE;
            rxTrtcmCbsPbsSet.CbsParamValue = 0;
            rxTrtcmCbsPbsSet.PbsParamValue = 0;
            rxTrtcmCbsPbsCfg.CbsPktMode = TRTCM_BYTE_MODE;
            rxTrtcmCbsPbsCfg.PbsPktMode = TRTCM_BYTE_MODE;
        }else{
            rxTrtcmCbsPbsCfg.trtcmModule = INGRESS_TRTCM ;
            rxTrtcmCbsPbsCfg.Index = meter_idx ;
            QDMA_API_GET_GENERAL_TRTCM_CFG(mainType_val, &rxTrtcmCbsPbsCfg);
        
            rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
            rxTrtcmCbsPbsSet.Index = meter_idx;
            QDMA_API_GET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);
        }
        
        if(rxTrtcmCbsPbsSet.CbsParamValue == rxTrtcmCbsPbsSet.PbsParamValue) {
            printk("%d\t%s%s\t%d\t\t%s\t%d%s\n", idx, meter_name, (is_need_tab_flag==1)?("\t"):(""), meter_idx, 
            ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
            rxTrtcmCbsPbsSet.PbsParamValue,
            (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
            );
        } else {
            printk("%d\t%s%s\t%d\t\t%s\t%d%s~%d%s\n", idx, meter_name, (is_need_tab_flag==1)?("\t"):(""), meter_idx, 
            ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
            rxTrtcmCbsPbsSet.CbsParamValue,
            (rxTrtcmCbsPbsCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
            rxTrtcmCbsPbsSet.PbsParamValue,
            (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
            );
        }
    }

    return 0;
}

static int fe_meter_qdmawan_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int idx = 0;
    int meter_type = -1;
    int meter_idx = 0x7F;
    unsigned char devName[16];
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ; 

    if(feResManage == NULL)
        return 0;

	printk("up stream ratelimit, Meter-0 mode is %d, Meter-1 mode is %d, Meter-2 mode is %d,\n",
		feResManage->up_stream_mode[0], feResManage->up_stream_mode[1], feResManage->up_stream_mode[2]);
	printk("(0:CAR_QOS_MODE 1:IP_BIND_MODE 2:IF_BIND_MODE 3:OLT_CTL_MODE 4:TRAFFIC_QOS_MODE 5:DEV_MAC_MODE 6:FORCEVW_WRR_MODE 7:WAN_MODE 8:NULL_BIND_MODE) \n");
    printk("OLT CTL mode:olt control mapping state \n");
    printk("Num\tOltCtlIdx\tmeter_idx\tenable\trate\n");
    for(idx = 0; idx < OLT_CTL_NUM; idx ++){
        fe_dump_one_meter(UP_STREAM, idx, feResManage->Meter_OLT_UP[idx], feResManage->OLTNameUP[idx]);
    }
    
    printk("QoS CAR mode: queue id and meter id mapping state\n");
    printk("que_id\tque_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < QOS_CAR_NUM; idx++){
        fe_dump_one_meter(UP_STREAM, idx, feResManage->CAR_QOS_QUEUE_UP[idx], feResManage->CAR_QOS_QUEUE_NAME_UP[idx]);
    }
	
    printk("IF mode: IF id and meter id mapping state\n");
    printk("Num\tItf_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < IF_MAX_NUM; idx++){
        fe_dump_one_meter(UP_STREAM, idx, feResManage->Meter_IF_UP[idx], feResManage->IFNameUP[idx]);
    }

    printk("IP mode: IP id and meter id mapping state\n");
    printk("Num\tip_range\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < IP_MAX_NUM; idx++){
        fe_dump_one_meter(UP_STREAM, idx, feResManage->Meter_IP_UP[idx], feResManage->IPNameUP[idx]);
    }

    printk("FORCE BANDWIDTH WRR QOS mode:queue id and meter id mapping state \n");
    printk("que_id\tque_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < FORCEBW_WRR_NUM; idx++){
        fe_dump_one_meter(UP_STREAM, idx, feResManage->FORCEBW_WRR_QUEUE_UP[idx], feResManage->FORCEBW_WRR_QUEUE_NAME_UP[idx]);
    }	

    fe_get_meter_group(&meter_type, UP_STREAM, DEV_MAC_MODE);
    printk("DEV MAC mode: Use meter[%d], DEV MAC ratelimit state \n", meter_type);
    printk("Num\tMacIdx\t\ttype&index\tenable\trate\n");
    for(idx = 0; idx < UNDER_DEV_MAX_NUM; idx ++){
        sprintf(devName, "MAC%d", idx);
        meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_DEV_FLOW_UP[idx]);
        fe_dump_one_meter(UP_STREAM, idx, meter_idx, devName);
    }
    
    fe_get_meter_group(&meter_type, UP_STREAM, TRAFFIC_QOS_MODE);
    printk("TRAFFIC QoS mode: Use meter[%d], queue id and meter id mapping state\n", meter_type);
    printk("que_id\tque_name\ttype&index\tenable\trate\n");
    for(idx = 0; idx < TRAFFIC_QOS_NUM; idx++){
        meter_idx = (((meter_type & 0x3) << 8) | feResManage->TRAFFIC_QOS_UP[idx]);
        fe_dump_one_meter(UP_STREAM, idx, meter_idx, feResManage->TRAFFIC_QOS_Name_UP[idx]);
    }

    fe_get_meter_group(&meter_type, UP_STREAM, WAN_MODE);
    printk("WAN mode: Use meter[%d], WAN ratelimit state \n", meter_type);
    printk("Num\tWanIdx\t\ttype&index\tenable\trate\n");
    for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
        sprintf(devName, "WAN%d", idx);
        meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_WAN_FLOW_UP[idx]);		
        fe_dump_one_meter(UP_STREAM, idx, meter_idx, devName);
    }
	
    return 0; 
}

static int fe_meter_qdmalan_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int idx = 0;
    int meter_type = -1;
    int meter_idx = 0x7F;
    unsigned char devName[16];
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ;   

    if(feResManage == NULL)
        return 0;

	printk("down stream ratelimit, Meter-0 mode is %d, Meter-1 mode is %d, Meter-2 mode is %d,\n",
		feResManage->dw_stream_mode[0], feResManage->dw_stream_mode[1], feResManage->dw_stream_mode[2]);
	printk("(0:CAR_QOS_MODE 1:IP_BIND_MODE 2:IF_BIND_MODE 3:OLT_CTL_MODE 4:TRAFFIC_QOS_MODE 5:DEV_MAC_MODE 6:WAN_MODE 7:NULL_BIND_MODE) \n");
		    
    printk("OLT CTL mode:olt control mapping state \n");
    printk("Num\tOltCtlIdx\tmeter_idx\tenable\trate\n");
    for(idx = 0; idx < OLT_CTL_NUM; idx ++){
        fe_dump_one_meter(DOWN_STREAM, idx, feResManage->Meter_OLT_DOWN[idx], feResManage->OLTNameDOWN[idx]);
    }

    printk("QoS CAR mode: queue id and meter id mapping state\n");
    printk("que_id\tque_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < QOS_CAR_NUM; idx++){
        fe_dump_one_meter(DOWN_STREAM, idx, feResManage->CAR_QOS_QUEUE_DOWN[idx], feResManage->CAR_QOS_QUEUE_NAME_DOWN[idx]);
    }

    printk("IF mode: IF id and meter id mapping state\n");
    printk("Num\tItf_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < IF_MAX_NUM; idx++){
        fe_dump_one_meter(DOWN_STREAM, idx, feResManage->Meter_IF_DOWN[idx], feResManage->IFNameDOWN[idx]);
    }

    printk("IP mode: IP id and meter id mapping state\n");
    printk("Num\tip_range\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < IP_MAX_NUM; idx++){
        fe_dump_one_meter(DOWN_STREAM, idx, feResManage->Meter_IP_DOWN[idx], feResManage->IPNameDOWN[idx]);
    }
	
    fe_get_meter_group(&meter_type, DOWN_STREAM, DEV_MAC_MODE);
    printk("DEV MAC mode: Use meter[%d], DEV MAC ratelimit state \n", meter_type);
    printk("Num\tMacIdx\t\ttype&index\tenable\trate\n");
    for(idx = 0; idx < UNDER_DEV_MAX_NUM; idx ++){
        sprintf(devName, "MAC%d", idx);
        meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_DEV_FLOW_DW[idx]);
        fe_dump_one_meter(DOWN_STREAM, idx, meter_idx, devName);
    }
	
    fe_get_meter_group(&meter_type, DOWN_STREAM, TRAFFIC_QOS_MODE);
    printk("TRAFFIC QoS mode: Use meter[%d], queue id and meter id mapping state\n", meter_type);
    printk("que_id\tque_name\ttype&index\tenable\trate\n");
    for(idx = 0; idx < TRAFFIC_QOS_NUM; idx++){
        meter_idx = (((meter_type & 0x3) << 8) | feResManage->TRAFFIC_QOS_DW[idx]);
        fe_dump_one_meter(DOWN_STREAM, idx, meter_idx, feResManage->TRAFFIC_QOS_Name_DW[idx]);
    }
	
    fe_get_meter_group(&meter_type, DOWN_STREAM, WAN_MODE);
    printk("WAN mode: Use meter[%d], WAN ratelimit state \n", meter_type);
    printk("Num\tWanIdx\t\ttype&index\tenable\trate\n");
    for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
        sprintf(devName, "WAN%d", idx);
        meter_idx = (((meter_type & 0x3) << 8) | feResManage->Meter_WAN_FLOW_DW[idx]);
        fe_dump_one_meter(DOWN_STREAM, idx, meter_idx, devName);
    }

    return 0; 
}

static int fe_ratelimit_up_info_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int i = 0 ;
    int j = 0 ;
    
    printk("TrafficRatelimitInfoDisplay\n");

    if(feResManage == NULL)
        return 0;
    
    printk("\nInterface Info: %s\n", (feResManage->up_stream_mode[0] == IF_BIND_MODE)? "ENABLE":"DISABLE");
    printk("Interface-Name\tActive\tRate\n");
    for( i  = 0 ; i < IF_MAX_NUM ; i++ )
    {
        if( i < 4 )
        {
            printk("LAN%d\t\t%s\t%d Kbps\n\r",
                (i+1) ,
                itf_info_up[i].enable?"Yes":"No",
                itf_info_up[i].rate);
        }
        else
        {
            printk("SSID%d\t\t%s\t%d Kbps\n\r",
                (i-3) ,
                itf_info_up[i].enable?"Yes":"No",
                itf_info_up[i].rate);
        }
    }

    printk("\nIP range Info: %s\n", (feResManage->up_stream_mode[0] == IP_BIND_MODE)? "ENABLE":"DISABLE");
    printk("No\tActive\tRate\tMode\tIP range\n");
    for( i  = 0 ; i < IP_MAX_NUM ; i++ )
    {
        printk("%d\t%s\t%d Kbps\t%s\t",
                (i+1) ,
                ip_info_up[i].enable?"Yes":"No",
                ip_info_up[i].rate,
                ip_info_up[i].ip_version?"IPv6":"IPv4");
        if( IP_VERSION_V6 == ip_info_up[i].ip_version )/*IPv6*/
        {
            for( j = 0 ; j < 16 ; )
            {
                printk("%02X%02X",ip_info_up[i].v6_pair_l[j],ip_info_up[i].v6_pair_l[j+1]);
                j = j + 2 ;
                if( j < 16 )
                    printk(":");
            }
            printk("~");
            for( j = 0 ; j < 16 ; )
            {
                printk("%02X%02X",ip_info_up[i].v6_pair_h[j],ip_info_up[i].v6_pair_h[j+1]);
                j = j + 2 ;
                if( j < 16 )
                    printk(":");
            }
        }
        else if( IP_VERSION_V4 == ip_info_up[i].ip_version )/*IPv4*/
        {
            for( j = 0 ; j < 4 ; j ++ )
            {
                printk("%d",ip_info_up[i].v4_pair_l[j]);
                if( j < 3 )
                    printk(".");
            }
            printk("~");
            for( j = 0 ; j < 4 ; j ++ )
            {
                printk("%d",ip_info_up[i].v4_pair_h[j]);
                if( j < 3 )
                    printk(".");
            }
        }
        printk("\n\r");
    }

    return 0;
}


static int fe_acnt_mapping_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return 0 ;
}

static int fe_acnt_mapping_state_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    char val_string[64] = {0}, cmd[32] = {0}, subcmd[32] = {0};
    int acnt_type = -1, old_acnt = -1, mode = -1;
    int wanIndex = -1;
    int i = 0;
    struct ecnt_fe_data fe_data;

    memset(&fe_data,0,sizeof(struct ecnt_fe_data));
    if (count > sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    if(!sscanf(val_string, "%s %s %d %d", cmd, subcmd, &mode, &acnt_type))
    {
		goto usage;
    }

	if(!strcmp(cmd, "set") && !strcmp(subcmd, "mode")){
		if((mode < TRAFFIC_FLOW_MODE) || (mode > DEFAULT_NULL_MODE))
			goto usage;
	    if((acnt_type < ACNT0_TYPE) || (acnt_type > ACNT2_TYPE))
			goto usage;

		fe_data.api_data.meter_cfg.acnt_mode = mode;
		if(acnt_type == ACNT0_TYPE){
			fe_data.api_data.meter_cfg.dir = 0;
			fe_api_set_acnt0_mode(&fe_data);
			fe_data.api_data.meter_cfg.dir = 1;
			fe_api_set_acnt0_mode(&fe_data);			
		}else if(acnt_type == ACNT1_TYPE){
			fe_data.api_data.meter_cfg.dir = 0;
			fe_api_set_acnt1_mode(&fe_data);
			fe_data.api_data.meter_cfg.dir = 1;
			fe_api_set_acnt1_mode(&fe_data);			
		}else if(acnt_type == ACNT2_TYPE){
			fe_data.api_data.meter_cfg.dir = 0;
			fe_api_set_acnt2_mode(&fe_data);
			fe_data.api_data.meter_cfg.dir = 1;
			fe_api_set_acnt2_mode(&fe_data);
		}
	}else if(!strcmp(cmd, "set") && !strcmp(subcmd, "debug")){
		feResManage->dbgLevel = mode;
		printk("feResManage->dbgLevel is %d\n", feResManage->dbgLevel);
	}else if(!strcmp(cmd, "dump") && !strcmp(subcmd, "mode")){
		fe_get_acnt_group(&acnt_type, 0, mode);
		if((acnt_type < ACNT0_TYPE) || (acnt_type > ACNT2_TYPE))
		{
			printk("Can not find mode %d in Account group list!\n", mode);
			goto usage;
		}

		if(mode == TRAFFIC_FLOW_MODE){
			fe_traffic_flow_mapping_state(acnt_type);
		}else if(mode == LLID_MODE){
			fe_llid_mapping_state(acnt_type);
		}else if(mode == DEV_MODE){
			fe_dev_mac_mapping_state(acnt_type);
		}else if(mode == WAN_ITF_MODE){
			fe_wan_itf_mapping_state(acnt_type);
		}else{
			printk("feResManage->acnt_up_mode[%d] is \n", acnt_type, feResManage->acnt_up_mode[acnt_type]);
			goto usage;
		}
	}else if(!strcmp(cmd, "dump") && !strcmp(subcmd, "acnt")){
	    if((acnt_type < ACNT0_TYPE) || (acnt_type > ACNT2_TYPE))
			goto usage;
		
		if(feResManage->acnt_up_mode[acnt_type] == TRAFFIC_FLOW_MODE){
			fe_traffic_flow_mapping_state(acnt_type);
		}else if(feResManage->acnt_up_mode[acnt_type] == LLID_MODE){
			fe_llid_mapping_state(acnt_type);
		}else if(feResManage->acnt_up_mode[acnt_type] == DEV_MODE){
			fe_dev_mac_mapping_state(acnt_type);
		}else if(feResManage->acnt_up_mode[acnt_type] == WAN_ITF_MODE){
			fe_wan_itf_mapping_state(acnt_type);
		}else{
			printk("feResManage->acnt_up_mode[%d] is %d\n", acnt_type, feResManage->acnt_up_mode[acnt_type]);
			goto usage;
		}
	}

	return count;
	
usage:
	printk("usage:\necho set debug <level[0|1|2]> 0 > /proc/tc3162/fe_acnt_mapping\n");
	printk("echo set mode <mode[0|1|2]> <acnt_group[0|1|2]> > /proc/tc3162/fe_acnt_mapping\n");
	printk("echo dump mode <mode[0|1|2] <acnt_group[0|1|2]> > /proc/tc3162/fe_acnt_mapping\n");
	printk("echo dump acnt <mode[0|1|2] <acnt_group[0|1|2]> > /proc/tc3162/fe_acnt_mapping\n");
	printk("acnt_group:	0 Account-0; 1 Account-1; 2 Account-2\n");
	printk("mode:		0 TRAFFIC_FLOW_MODE; 1 DEV_MODE; 2 WAN_ITF_MODE; 5 DEFAULT_NULL_MODE\n");

	return -EFAULT;
}


static int fe_default_wan_itf_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("default_wan_itf: %d \n",default_wan_itf);

	return 0;
}

static int fe_default_wan_itf_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
    char val_string[64] = {0}, cmd[32] = {0};
    int num1=0, num2=0;
    int wanIndex = -1;
    int i = 0;

    if (count > sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    if(!sscanf(val_string, "nas%d_%d", &num1, &num2))
    {
        printk("usage: echo nasx_x > /proc/tc3162/ppe_default_wan_itf");
        return -EFAULT;
    }    

#ifndef CONFIG_TP_IMAGE
    if(TCSUPPORT_CT_VAL || TCSUPPORT_OPENWRT_MULTI_WAN_VAL)
        wanIndex = num1 * 8 + num2;
    else
#endif /* CONFIG_TP_IMAGE */
        wanIndex = num1 * 8;

    for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
        if(!wan_itf_info[i].valid)
            continue;
        
        if(wanIndex == wan_itf_info[i].wanIndex)
            default_wan_itf = i;
    }

	return count;
}
static int fe_set_wan_itf_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{

	return 0;
}

static int fe_set_wan_itf_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
    char val_string[64] = {0}, cmd[32] = {0}, wan_name[32] = {0};
    int num1=0, num2=0;
    int wanIndex = -1;
    int i = 0,idx=0,size=0;
	struct net_device *dev=NULL;
    if (count > sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;
    if(!sscanf(val_string, "%s %s %d", cmd,wan_name, &wanIndex))
    {
        printk("usage: echo add wan_name index > /proc/tc3162/ppe_set_wan_itf");
        printk("usage: echo del wan_name index > /proc/tc3162/ppe_set_wan_itf");
        return -EFAULT;
    }    
	size=sizeof(wan_itf_info[0].wan_dev->name);
	if(!strcmp(cmd, "add")){
#if KERNEL_2_6_36
		dev = dev_get_by_name(&init_net,wan_name);	
#endif
		if(dev)
			dev_put(dev);
		else{
			printk("Wrong Dev!\n");
        	return -EFAULT;
		}
        for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
            if(wan_itf_info[i].valid == 0 && wanIndex==i){
				
				wan_itf_info[i].wan_dev=dev;
                wan_itf_info[i].valid = 1;
                wan_itf_info[i].wanIndex = wanIndex;
                for(idx=0; idx<WAN_ITF_ACNT_NUM; idx++)
                    printk("NETDEV_UP: wan_itf_info[%d].valid=%d, wan_itf_info[%d].wan_dev=%s\n"
                    , idx, wan_itf_info[idx].valid, idx, (wan_itf_info[idx].valid==0)?("NULL"):wan_itf_info[idx].wan_dev->name);
              	break;
            }
        }

	}else if(!strcmp(cmd, "del")){
		for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
			if(!wan_itf_info[i].valid)
				continue;
			if(strcmp(wan_itf_info[i].wan_dev->name, wan_name) == 0){
				wan_itf_info[i].valid = 0;
				wan_itf_info[i].wan_dev=NULL;
                wan_itf_info[i].wanIndex = -1;
				break;
			}
		}
	}else if(!strcmp(cmd, "enable")){
		wan_remark_hook_enable=1;
	}else if(!strcmp(cmd, "disable")){
		wan_remark_hook_enable=0;
	}else if(!strcmp(cmd, "lan")){
		aewan_lan_mode=1;
	}else if(!strcmp(cmd, "wan")){
		aewan_lan_mode=0;
	}

	return count;
}
static int fe_set_wan_virtual_itf_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{

	return 0;
}


static int fe_set_wan_virtual_itf_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
    char val_string[64] = {0}, cmd[32] = {0}, wan_name[32] = {0};
    int i = 0,idx=0,size=0;
	struct net_device *dev=NULL;
    if (count > sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;
    if(!sscanf(val_string, "%s %s", cmd,wan_name))
    {
        printk("usage: echo add wan_name > /proc/tc3162/ppe_set_wan_virtual_itf");
        printk("usage: echo del wan_name > /proc/tc3162/ppe_set_wan_virtual_itf");
        return -EFAULT;
    }    
	if(!strcmp(cmd, "add")){
#if KERNEL_2_6_36
		dev = dev_get_by_name(&init_net,wan_name);	
#endif
		if(dev)
			dev_put(dev);
		else{
			printk("Wrong Dev!\n");
        	return -EFAULT;
		}
        for(i = 0; i < 16; i++){
            if(wan_virtualdev_ifname_info[i].valid == 0 ){
				memcpy(wan_virtualdev_ifname_info[i].virtual_name, wan_name, strlen(wan_name));
				memcpy(wan_virtualdev_ifname_info[i].dev_name, dev->name, strlen(dev->name));
				wan_virtualdev_ifname_info[i].valid = 1;
              	break;
            }
        }

	}else if(!strcmp(cmd, "del")){
		for(i = 0; i < 16; i++){
			if(!wan_virtualdev_ifname_info[i].valid)
				continue;
			if(strcmp(wan_virtualdev_ifname_info[i].virtual_name, wan_name) == 0){
				memset(wan_virtualdev_ifname_info[i].virtual_name, 0, sizeof(wan_virtualdev_ifname_info[i].virtual_name));				
				memset(wan_virtualdev_ifname_info[i].dev_name, 0, sizeof(wan_virtualdev_ifname_info[i].dev_name));					
				wan_virtualdev_ifname_info[i].valid = 0;
				break;
			}
		}
	}
	

	return count;
}

static int fe_acnt_mode_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("fe_account_mode: %d, 1 is flow acnt, 2 is llid acnt\n",fe_account_mode);

	return 0;
}

static int fe_acnt_mode_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
    char val_string[64] = {0};

    if (count > sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    if(!sscanf(val_string, "%d", &fe_account_mode))
    {
        printk("usage: echo [1|2] > /proc/tc3162/fe_acnt_mode\n");
        return -EFAULT;
    }

	return count;

}

static int update_multicast_cnt_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("update_mul_cnt_en: %x \n", update_mul_cnt_en);

	return 0;
}

static int update_multicast_cnt_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
    char val_string[64] = {0};

    if (count > sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    if(!sscanf(val_string, "%x", &update_mul_cnt_en))
    {
        printk("usage: echo [0|1|2|3|...|F] > /proc/tc3162/update_multicast_cnt \n");
		printk("LAN1:bit0,LAN2:bit1,LAN3:bit2,LAN4:bit3\n");
        return -EFAULT;
    }

	return count;
}

static int traffic_flow_acnt_meter_mask_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{

	printk("traffic_flow_filter %x,  traffic_flow_shift %d\n", traffic_flow_filter, traffic_flow_shift);

	return 0;	
}

static int traffic_flow_acnt_meter_mask_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[64] = {0};
	unsigned int flag = 0;
	int shift = 0;

    if (count > sizeof(get_buf) - 1)
        return -EINVAL;
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	if(!sscanf(get_buf, "%x", &flag))
	{
        printk("usage: echo [mask] > /proc/tc3162/traffic_flow_acnt_meter_mask \n");
        return -EFAULT;
	}
	traffic_flow_filter = flag;

	while(shift < 32)
	{
		if((1<<shift) & traffic_flow_filter)
			break;
		shift++;
	}
	traffic_flow_shift = shift;
	printk("traffic_flow_filter: %x, traffic_flow_shift %d\n", traffic_flow_filter, traffic_flow_shift);

	return count;
}

void fe_account_meter_proc_init(void)
{
    struct proc_dir_entry *fe_resource_manage_proc = NULL;
	
    fe_resource_manage_proc = create_proc_entry(meter_qdmawan_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_meter_qdmawan_state_read_proc;
    }
    
    fe_resource_manage_proc = create_proc_entry(meter_qdmalan_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_meter_qdmalan_state_read_proc;
    }

    fe_resource_manage_proc = create_proc_entry(fe_ratelimit_up_info_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_ratelimit_up_info_read_proc;
    }

    fe_resource_manage_proc = create_proc_entry(fe_ratelimit_dw_info_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_ratelimit_dw_info_read_proc;
    }

	fe_resource_manage_proc = create_proc_entry(fe_acnt_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_acnt_mapping_state_read_proc;		
        fe_resource_manage_proc->write_proc = fe_acnt_mapping_state_write_proc;
    }
	
    fe_resource_manage_proc = create_proc_entry("tc3162/fe_default_wan_itf", 0, NULL);
    fe_resource_manage_proc->read_proc = fe_default_wan_itf_read_proc;
    fe_resource_manage_proc->write_proc =  fe_default_wan_itf_write_proc;

	fe_resource_manage_proc = create_proc_entry("tc3162/fe_set_wan_itf", 0, NULL);
    if(fe_resource_manage_proc) {
		fe_resource_manage_proc->read_proc = fe_set_wan_itf_read_proc;
		fe_resource_manage_proc->write_proc =  fe_set_wan_itf_write_proc;
    }
	fe_resource_manage_proc = create_proc_entry("tc3162/fe_set_wan_virtual_itf", 0, NULL);
    if(fe_resource_manage_proc) {
		fe_resource_manage_proc->read_proc = fe_set_wan_virtual_itf_read_proc;
		fe_resource_manage_proc->write_proc =  fe_set_wan_virtual_itf_write_proc;
    }
	
	fe_resource_manage_proc = create_proc_entry("tc3162/fe_acnt_mode", 0, NULL);
	fe_resource_manage_proc->read_proc = fe_acnt_mode_read_proc;
	fe_resource_manage_proc->write_proc =  fe_acnt_mode_write_proc;
	
#if defined(TCSUPPORT_CT_JOYME2)
	fe_resource_manage_proc = create_proc_entry("tc3162/update_multicast_cnt", 0, NULL);
    fe_resource_manage_proc->read_proc = update_multicast_cnt_read_proc;
    fe_resource_manage_proc->write_proc =  update_multicast_cnt_write_proc;
#endif

	fe_resource_manage_proc = create_proc_entry("tc3162/traffic_flow_acnt_meter_mask", 0, NULL);
	fe_resource_manage_proc->read_proc = traffic_flow_acnt_meter_mask_read_proc;
	fe_resource_manage_proc->write_proc =  traffic_flow_acnt_meter_mask_write_proc;
    return 0;

}

void fe_account_meter_proc_exit(void)
{

	remove_proc_entry(meter_qdmawan_mapping_path, NULL) ;
	remove_proc_entry(meter_qdmalan_mapping_path, NULL) ;
	remove_proc_entry(fe_ratelimit_up_info_path, NULL) ;
	remove_proc_entry(fe_ratelimit_dw_info_path, NULL) ;
	remove_proc_entry(fe_acnt_mapping_path, NULL) ;
	remove_proc_entry("tc3162/fe_default_wan_itf", NULL) ;
	remove_proc_entry("tc3162/fe_acnt_mode", NULL) ;
#if defined(TCSUPPORT_CT_JOYME2)
	remove_proc_entry("tc3162/update_multicast_cnt", NULL);
#endif
	remove_proc_entry("tc3162/traffic_flow_acnt_meter_mask", NULL);

	remove_proc_entry("tc3162/fe_set_wan_virtual_itf", NULL);
	remove_proc_entry("tc3162/fe_set_wan_itf", NULL);
	return 0;

}

