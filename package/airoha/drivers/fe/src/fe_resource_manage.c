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
#include "fe_resource_manage.h"
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
#if defined(TCSUPPORT_CPU_EN7580)
#define AcntManage_T QDMA_AcntManage_T
#define FE_GENERAL_ALLOC_ACNT(x, y)   \
    do {  \
            (UP_STREAM==(x))? QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_WAN, (y)): \
                QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_LAN, (y)); \
       }while(0)

        
#define FE_GENERAL_FREE_ACNT(x, y)   \
    do {  \
            (UP_STREAM==(x))? QDMA_API_FREE_ACNT(ECNT_QDMA_WAN, (y)): \
                QDMA_API_FREE_ACNT(ECNT_QDMA_LAN, (y)); \
       }while(0)
       
#else if defined(TCSUPPORT_CPU_EN7527)
#define AcntManage_T PPE_AcntManage_T
#define FE_GENERAL_ALLOC_ACNT(x, y) PPE_API_ALLOCATE_ACNT((y))
#define FE_GENERAL_FREE_ACNT(x, y) PPE_API_FREE_ACNT((y))
       
#endif

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

#define 	MAX(a, b) ((a>b)?(a):(b))

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

unsigned int old_wan_conn_type = 4;
extern unsigned int wan_conn_type;
extern char wan_itf_name[];

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

Dev_Mac_T dev_mac_info[UNDER_DEV_MAX_NUM];

extern Wan_itf_name wan_itf_info[WAN_ITF_ACNT_NUM];

static int default_wan_itf=ACNT1_DEFAULT;
extern wan_virtualdev_ifname wan_virtualdev_ifname_info[16];
static int update_mul_cnt_en = 0;

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
/******************************************************************************
 Descriptor:    It's used to get meter mark from skb->fe_resource_mark
 Input Args:    
 Ret Value: 
******************************************************************************/
int qdma_qos_mark(struct sk_buff *skb)
{
    int priority = 0;
    priority = (skb->mark & FE_METER_FILTER_MARK)>> 4;
    
    return priority;
}
/******************************************************************************
 Descriptor:    It's used to get meter mark from skb->fe_resource_mark
 Input Args:    
 Ret Value: 
******************************************************************************/
int fe_get_traffic_queue_mark(struct sk_buff *skb)
{
    int priority = 0;
    if ( TCSUPPORT_CMCCV2_VAL ){
        priority = (skb->mark2 & 0x0f);
    }else{
        priority = (skb->mark & FE_METER_FILTER_MARK)>> 4;
    }
    
    return priority;
}

/******************************************************************************
 Descriptor:    It's used to get meter mark from skb->fe_resource_mark
 Input Args:    
 Ret Value: 
******************************************************************************/
int fe_get_meter_mark(struct sk_buff *skb)
{
    int priority = 0;
    priority = (skb->fe_resource_mark & MTR_MASK);

    return priority;
}
/******************************************************************************
 Descriptor:    It's used to get acnt1 mark from skb->fe_resource_mark
 Input Args:    
 Ret Value: 
******************************************************************************/
int fe_get_acnt0_mark(struct sk_buff *skb)
{
    int priority = 0;
    priority = (skb->fe_resource_mark & ACNT0_MASK) >> ACNT0_OFFSET;

    return priority;
}
/******************************************************************************
 Descriptor:    It's used to get acnt1 mark from skb->fe_resource_mark
 Input Args:    
 Ret Value: 
******************************************************************************/
int fe_get_acnt1_mark(struct sk_buff *skb)
{
    int priority = 0;
    priority = (skb->fe_resource_mark & ACNT1_MASK) >> ACNT1_OFFSET;

    return priority;
}

/******************************************************************************
 Descriptor:    It's used to get acnt2 mark from skb->fe_resource_mark
 Input Args:    
 Ret Value: 
******************************************************************************/
int fe_get_acnt2_mark(struct sk_buff *skb)
{
    int priority = 0;
    priority = (skb->fe_resource_mark & ACNT2_MASK) >> ACNT2_OFFSET;

    return priority;
}

#if defined(TCSUPPORT_CPU_EN7580)
static int fe_dump_one_meter(FE_Dir_t dir, int idx, int meter_idx, unsigned char *meter_name)
{
    int is_need_tab_flag = 0;
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

    if(strlen(meter_name) < 8)
        is_need_tab_flag = 1;
    
    if (dir == UP_STREAM)
        mainType_val = ECNT_QDMA_WAN;
    else
        mainType_val = ECNT_QDMA_LAN;
    
    rxTrtcmCfg.trtcmModule = INGRESS_TRTCM ;
    QDMA_API_GET_GENERAL_TRTCM_INIT(mainType_val, &rxTrtcmCfg);

    if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
        /* if meter_idx below than 16, means this meter is disable and value is 0 */
        if((meter_idx < RX_RING_NUM) || (meter_idx >= GENERAL_INGRESS_NO_RATELIMIT_INDEX)) {
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
        /* if meter_idx below than 16, means this meter is disable and value is 0 */
        if((meter_idx < RX_RING_NUM) || (meter_idx >= GENERAL_INGRESS_NO_TRTCM_INDEX)) {
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

/*******************************************************************************************
**function name
    meter_qdmawan_state_read_proc
**description:
    read meter_id and queue_id mapping state in qdma_wan
 **retrun 
    0:success
    -1:failure
**parameter:
********************************************************************************************/
static int fe_meter_qdmawan_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int idx = 0;
    unsigned char devName[16];
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ; 

    if(feResManage == NULL)
        return 0;
    
	printk("up stream ratelimit mode is %d, (0:CAR_QOS_MODE 1:IP_BIND_MODE 2:IF_BIND_MODE 3:OLT_CTL_MODE \n\
            4:TRAFFIC_QOS_MODE 5:DEV_MAC_MODE 6:FORCEVW_WRR_MODE 7:WAN_MODE 8:NULL_BIND_MODE) \n",feResManage->up_stream_mode);
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

	printk("TRAFFIC QoS  mode: queue id and meter id mapping state\n");
    printk("que_id\tque_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < TRAFFIC_QOS_NUM; idx++){
        fe_dump_one_meter(UP_STREAM, idx, feResManage->TRAFFIC_QOS_UP[idx], feResManage->TRAFFIC_QOS_Name_UP[idx]);
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

    printk("DEV MAC mode:DEV MAC ratelimit state \n");
    printk("Num\tMacIdx\t\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx ++)
    {
        sprintf(devName, "MAC%d", idx);
        if(wan_conn_type == 4)
        fe_dump_one_meter(UP_STREAM, idx, feResManage->Meter_DEV_FLOW_UP[idx], devName);
        else
        	fe_dump_one_meter(DOWN_STREAM, idx, feResManage->Meter_DEV_FLOW_UP[idx], devName);
    }
    
    printk("FORCE BANDWIDTH WRR QOS mode:queue id and meter id mapping state \n");
    printk("que_id\tque_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < FORCEBW_WRR_NUM; idx++){
        fe_dump_one_meter(UP_STREAM, idx, feResManage->FORCEBW_WRR_QUEUE_UP[idx], feResManage->FORCEBW_WRR_QUEUE_NAME_UP[idx]);
    }

    printk("WAN mode:WAN ratelimit state \n");
    printk("Num\tWanIdx\t\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
        sprintf(devName, "WAN%d", idx);
        fe_dump_one_meter(UP_STREAM, idx, feResManage->Meter_WAN_FLOW_UP[idx], devName);
    }
	
    return 0; 
}

/*******************************************************************************************
**function name
    meter_qdmalan_state_read_proc
**description:
    read meter_id and queue_id mapping state in qdma_lan
 **retrun 
    0:success
    -1:failure
**parameter:
********************************************************************************************/
static int fe_meter_qdmalan_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int idx = 0;
    unsigned char devName[16];
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ;   

    if(feResManage == NULL)
        return 0;

	printk("down stream ratelimit mode is %d,((0:CAR_QOS_MODE 1:IP_BIND_MODE 2:IF_BIND_MODE 3:OLT_CTL_MODE \n\
		    4:TRAFFIC_QOS_MODE 5:DEV_MAC_MODE 6:WAN_MODE 7:NULL_BIND_MODE) \n",feResManage->dw_stream_mode);
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

	printk("TRAFFIC QoS  mode: queue id and meter id mapping state\n");
    printk("que_id\tque_name\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < TRAFFIC_QOS_NUM; idx++){
        fe_dump_one_meter(DOWN_STREAM, idx, feResManage->TRAFFIC_QOS_DW[idx], feResManage->TRAFFIC_QOS_Name_DW[idx]);
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

    printk("DEV MAC mode:DEV MAC ratelimit state \n");
    printk("Num\tMacIdx\t\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx ++){
        sprintf(devName, "MAC%d", idx);
        fe_dump_one_meter(DOWN_STREAM, idx, feResManage->Meter_DEV_FLOW_DW[idx], devName);
    }

    printk("WAN mode:WAN ratelimit state \n");
    printk("Num\tWanIdx\t\tmeter_index\tenable\trate\n");
    for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
        sprintf(devName, "WAN%d", idx);
        fe_dump_one_meter(DOWN_STREAM, idx, feResManage->Meter_WAN_FLOW_DW[idx], devName);
    }

    return 0; 
}
#endif
/******************************************************************************
 Descriptor:    It's used to display ratelimit rule of up stream
 Input Args:    
 Ret Value: 
******************************************************************************/
static int fe_ratelimit_up_info_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int i = 0 ;
    int j = 0 ;
    
    printk("TrafficRatelimitInfoDisplay\n");

    if(feResManage == NULL)
        return 0;
    
    printk("\nInterface Info: %s\n", (feResManage->up_stream_mode == IF_BIND_MODE)? "ENABLE":"DISABLE");
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

    printk("\nIP range Info: %s\n", (feResManage->up_stream_mode == IP_BIND_MODE)? "ENABLE":"DISABLE");
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
/******************************************************************************
 Descriptor:    It's used to display ratelimit rule of down stream
 Input Args:    
 Ret Value: 
******************************************************************************/
static int fe_ratelimit_dw_info_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int i = 0 ;
    int j = 0 ;
    
    printk("TrafficRatelimitInfoDisplay\n");

    if(feResManage == NULL)
        return 0;
    
    printk("\nInterface Info: %s\n", (feResManage->dw_stream_mode == IF_BIND_MODE)? "ENABLE":"DISABLE");
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

    printk("\nIP range Info: %s\n", (feResManage->dw_stream_mode == IP_BIND_MODE)? "ENABLE":"DISABLE");
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

/******************************************************************************
 Descriptor:    It's used to display acnt2 mapping state
 Input Args:    
 Ret Value: 
******************************************************************************/
static int fe_acnt2_mapping_state_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int idx = 0;

    printk("acnt2 mapping state: \n");

    printk("\nDownstream ACNT2 mapping state \n");

    if(feResManage == NULL)
        return 0;

    if(feResManage->acnt2_dw_mode == TRAFFIC_FLOW_MODE){
        printk("down stream ,acnt2 mode is traffic qos flow mode\n");
        printk("NUM\tTRAFFIC_FLOW_IDX\tAcnt2_id\n");
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            printk("%d\t%s\t%d\n", idx, feResManage->ACNT2_TRAFFIC_FLOW_DOWN_NAME[idx], feResManage->ACNT2_TRAFFIC_FLOW_DOWN[idx]);
        }
    }else if(feResManage->acnt2_dw_mode == WAN_ITF_MODE){
        printk("NUM\tWAN_ITF_IDX\tAcnt2_id\n");
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, wan_itf_info[idx].wan_dev->name, feResManage->ACNT2_WAN_ITF_DOWN[idx]);
        }
    }else{
        printk("down stream ,acnt2 mode is NULL\n");
    }
    
    printk("\nUpstream ACNT2 mapping state \n");
    if(feResManage->acnt2_up_mode == TRAFFIC_FLOW_MODE){
        printk("up stream ,acnt2 mode is traffic qos flow mode\n");
        printk("NUM\tTRAFFIC_FLOW_IDX\tAcnt2_id\n");
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            printk("%d\t%s\t%d\n", idx, feResManage->ACNT2_TRAFFIC_FLOW_UP_NAME[idx], feResManage->ACNT2_TRAFFIC_FLOW_UP[idx]);
        }
    }else if(feResManage->acnt2_up_mode == WAN_ITF_MODE){
        printk("NUM\tWAN_ITF_IDX\tAcnt2_id\n");
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, wan_itf_info[idx].wan_dev->name, feResManage->ACNT2_WAN_ITF_UP[idx]);
        }

    }else{
        printk("up stream ,acnt2 mode is NULL\n");
    }
    
    return 0;

}

/******************************************************************************
 Descriptor:    It's used to display acnt1 mapping state
 Input Args:    
 Ret Value: 
******************************************************************************/
static int fe_acnt1_mapping_state_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int idx = 0;

    printk("acnt1 mapping state: flow id and acnt1 id mapping relationship\n");

    printk("\nDownstream ACNT1 mapping state \n");

    if(feResManage == NULL)
        return 0;

    if(feResManage->acnt1_dw_mode == TRAFFIC_FLOW_MODE){
        printk("down stream ,acnt1 mode is traffic qos flow mode\n");
        printk("NUM\tTRAFFIC_FLOW_IDX\tAcnt1_id\n");
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            printk("%d\t%s\t%d\n", idx, feResManage->ACNT1_TRAFFIC_FLOW_DOWN_NAME[idx], feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx]);
        }
    }else if(feResManage->acnt1_dw_mode == WAN_ITF_MODE){
        printk("NUM\tWAN_ITF_IDX\tAcnt1_id\n");
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
#if defined(TCSUPPORT_CPU_EN7580)
            printk("%d\twan_%d %s\t%d\n", idx, idx/2, idx%2?"multicast":"unicast" ,feResManage->ACNT1_WAN_ITF_DOWN[idx]);
#else if defined(TCSUPPORT_CPU_EN7527)
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, wan_itf_info[idx].wan_dev->name, feResManage->ACNT1_WAN_ITF_DOWN[idx]);
#endif
        }
    }else if(feResManage->acnt1_dw_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
        printk("NUM\tWAN_ITF_IDX\tAcnt1_id\n");
        for(idx = 0; idx < WAN_ITF_HAS_FLOW_ACNT_NUM; idx ++){
#if defined(TCSUPPORT_CPU_EN7580)
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, "mul & uni", feResManage->ACNT1_WAN_ITF_DOWN[idx]);
#else if defined(TCSUPPORT_CPU_EN7527)
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, wan_itf_info[idx].wan_dev->name, feResManage->ACNT1_WAN_ITF_DOWN[idx]);
#endif
        }
        printk("down stream ,acnt1 mode is traffic qos flow mode\n");
        printk("NUM\tTRAFFIC_FLOW_IDX\tAcnt1_id\n");
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            printk("%d\t%s\t%d\n", idx, feResManage->ACNT1_TRAFFIC_FLOW_DOWN_NAME[idx], feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx]);
        }
    }else{
        printk("down stream ,acnt1 mode is NULL\n");
    }
    
    printk("\nUpstream ACNT1 mapping state \n");
    if(feResManage->acnt1_up_mode == TRAFFIC_FLOW_MODE){
        printk("up stream ,acnt1 mode is traffic qos flow mode\n");
        printk("NUM\tTRAFFIC_FLOW_IDX\tAcnt1_id\n");
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            printk("%d\t%s\t%d\n", idx, feResManage->ACNT1_TRAFFIC_FLOW_UP_NAME[idx], feResManage->ACNT1_TRAFFIC_FLOW_UP[idx]);
        }
    }else if(feResManage->acnt1_up_mode == WAN_ITF_MODE){
        printk("NUM\tWAN_ITF_IDX\tAcnt1_id\n");
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
#if defined(TCSUPPORT_CPU_EN7580)
            printk("%d\twan_%d %s\t%d\n", idx, idx/2, idx%2?"multicast":"unicast" ,feResManage->ACNT1_WAN_ITF_UP[idx]);
#else if defined(TCSUPPORT_CPU_EN7527)
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, wan_itf_info[idx].wan_dev->name, feResManage->ACNT1_WAN_ITF_UP[idx]);
#endif
        }

    }else if(feResManage->acnt1_up_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
        printk("NUM\tWAN_ITF_IDX\tAcnt1_id\n");
        for(idx = 0; idx < WAN_ITF_HAS_FLOW_ACNT_NUM; idx ++){
#if defined(TCSUPPORT_CPU_EN7580)
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, "mul & uni" ,feResManage->ACNT1_WAN_ITF_UP[idx]);
#else if defined(TCSUPPORT_CPU_EN7527)
            printk("%d\twan_%d(%s)\t%d\n", idx, idx, wan_itf_info[idx].wan_dev->name, feResManage->ACNT1_WAN_ITF_UP[idx]);
#endif
        }
        printk("up stream ,acnt1 mode is traffic qos flow mode\n");
        printk("NUM\tTRAFFIC_FLOW_IDX\tAcnt1_id\n");
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            printk("%d\t%s\t%d\n", idx, feResManage->ACNT1_TRAFFIC_FLOW_UP_NAME[idx], feResManage->ACNT1_TRAFFIC_FLOW_UP[idx]);
        }
    }else{
        printk("up stream ,acnt1 mode is NULL\n");
    }
    
    return 0;

}

/******************************************************************************
 Descriptor:    It's used to display acnt0 mapping state
 Input Args:    account0 is uesd for mac
 Ret Value: 
******************************************************************************/
static int fe_acnt0_mapping_state_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int idx = 0;
    struct devBandwidth_s *devBandWidthPtr = NULL;
    
    printk("acnt0 mapping state: flow id and acnt1 id mapping relationship\n");

    printk("\nDownstream ACNT0 mapping state \n");
    printk("idx\titf_name\tmac\t\tAcnt0_id\n");

    if(feResManage == NULL)
        return 0;

    if(gHwBandwidthList == NULL){
        printk("gHwBandwidthList is NULL return!\n");
        return 0;
    }
    
    for(idx = 0; idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx ++){
        devBandWidthPtr = &(gHwBandwidthList->bandwidthList[idx]);
        if(devBandWidthPtr != NULL)
            printk("%d\tmac%d\t%02x:%02x:%02x:%02x:%02x:%02x\t%d\n", idx, idx, devBandWidthPtr->mac[0],
            devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],
            devBandWidthPtr->mac[4],devBandWidthPtr->mac[5],feResManage->ACNT0_DEV_FLOW_DOWN[idx]);
    }
    
    printk("\nUpstream ACNT0 mapping state \n");
    printk("idx\titf_name\tmac\t\tAcnt0_id\n");
    for(idx = 0; idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx ++){
        devBandWidthPtr = &(gHwBandwidthList->bandwidthList[idx]);
        if(devBandWidthPtr != NULL)
            printk("%d\tmac%d\t%02x:%02x:%02x:%02x:%02x:%02x\t%d\n", idx, idx, devBandWidthPtr->mac[0],
            devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],
            devBandWidthPtr->mac[4],devBandWidthPtr->mac[5],feResManage->ACNT0_DEV_FLOW_UP[idx]);
    }

    return 0;
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
    char val_string[64], cmd[32] ;
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

static int update_multicast_cnt_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("update_mul_cnt_en: %#x \n", update_mul_cnt_en);

	return 0;
}

static int update_multicast_cnt_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
    char val_string[64];

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

/******************************************************************************
 Descriptor:    It's used to get idx from itf rule
 Input Args:    itf dev name , dir 
 Ret Value: 1: ignore -1 :error 0  success
******************************************************************************/
int fe_get_meter_idx_from_if( char *pname )
{
    int lanidx = 0 ;

    if( NULL == pname )
        return -1 ;

    if( ('e' == pname[0]) && ('.' == pname[4]) )/*eth0.1~eth0.4*/
    {
        lanidx = pname[5] - '0' ;
    }
    else if( 'e' == pname[0] )/*eth0~eth3*/
    {
        lanidx = pname[3] - '0' + 1 ;
    }
    else if( ('r' == pname[0]) && ('a' == pname[1]) && ('i' == pname[2]) )/*rai0~rai3, 7580 is rai0~rai7*/
    {
        lanidx = pname[3] - '0' + WIFI_5G_OFFSET +1;
    }
    else if( ('r' == pname[0]) && ('a' == pname[1]) )/*ra0~ra3, 7580 is ra0~ra7*/
    {
        lanidx = pname[2] - '0' + WIFI_24G_OFFSET +1 ;
    }
    else
    {
        return -1 ;
    }
    return lanidx ;/*1~8*/
}
/******************************************************************************
 Descriptor:    It's used to get idx from ipv4 rule
 Input Args:    ip address , dir 
 Ret Value: 1: ignore -1 :error 0  success
******************************************************************************/
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

/******************************************************************************
 Descriptor:    It's used to remark skb according meter (array idx)  used for FH, 
 Input Args:    skb and dir(0: down stream 1:up stream)
 Ret Value: 1: ignore -1 :error 0  success
 in some mode such as traffic qos or QoS car ratelimit ,remark is did by cunstomers, no need no remark 
******************************************************************************/
int fe_remark_skb_mark_from_meter(struct sk_buff *skb, int dir )
{ 
    FE_Meter_Mode_t mode;
    char *pname = NULL ;
    int mask = 0;
    
    if( (NULL == skb) || (NULL == feResManage) )
        return -1 ;
    
    pname = skb->dev->name ;
    if(NULL == pname)
        return -1;
    else{
        if(('r' != pname[0]) && ('e' != pname[0]))
            return -1;
    }

    if(DOWN_STREAM == dir){
        mode = feResManage->dw_stream_mode;
    }else if(UP_STREAM == dir){
        mode = feResManage->up_stream_mode;
    }else
        return -1;
    
    if(IP_BIND_MODE == mode){
        mask = fe_get_meter_idx_from_ip(skb, dir, pname);
    }
    else if(IF_BIND_MODE == mode)
    {
        mask = fe_get_meter_idx_from_if(pname);
    }
    else if(NULL_BIND_MODE == mode)
    {
        /*not do mark if it is disable*/
        return 0;
    }
    else{
        return -1;
    }
    
    if ( mask <= 0 )
        return -1;

    skb->fe_resource_mark &= ~(MTR_MASK);/* clear  mark, zero mean no mark*/ 
    skb->fe_resource_mark |= ((mask) & MTR_MASK);

    skb->fe_resource_mark &= ~(ACNT1_MASK) ;
    skb->fe_resource_mark |= ((mask << ACNT1_OFFSET) & ACNT1_MASK);
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
    
    if(gHwBandwidthList == NULL)
        return -1;

    for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++){
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

int findGswIdxByMac(unsigned char *mac)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
	struct net_bridge_fdb_entry *dst;
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
	struct net_device *dev = NULL;
	unsigned char lanPortId = 0;
	unsigned char gswPortId = 0;

	const unsigned char *dest = mac;
	ndev = dev_get_by_name(&init_net, "br0");
	if (ndev)
		br = netdev_priv(ndev);
	else
		return -1;
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
	if ((dst = __br_fdb_get(br, dest, 0)) != NULL)
#else
	if ((dst = __br_fdb_get(br, dest)) != NULL)
#endif
	{
		if(IS_FDB_LOCAL(dst)){
			dev_put(ndev);
			return -1;
		}
		
		dev = dst->dst->dev;
		if(dev)
		{
			if(isLANInterface(dev)){
				lanPortId = dev->name[5] - '1';
				if(update_mul_cnt_en & (1 << lanPortId)) /*update_mul_cnt_en: bit0(LAN1)...bit3(LAN4)*/
				{
					dev_put(ndev);
					return lanPortId;
				}
			}
		}
	}

	dev_put(ndev);
#endif

	return -1;
}

/******************************************************************************
 Descriptor:    It's used to remark skb according mac dev
 Input Args:    skb and dir(0: down stream 1:up stream)
 Ret Value: 1: ignore -1 :error 0  success
******************************************************************************/
int fe_remark_skb_from_dev_mac(struct sk_buff *skb, int dir )
{ 
    unsigned char devMacAddr[ETH_ALEN];
    char *pname = NULL ;
    struct ethhdr *eth = NULL;
    int idx = -1;
    int mask = 0;
    FE_ACNT_Mode_t acnt_mode;
    FE_Meter_Mode_t meter_mode;
    
    if( (NULL == skb) || (NULL == feResManage) )
        return -1 ;

	if (!skb_mac_header_was_set(skb))
		return -1;
	
    pname = skb->dev->name ;
    eth = (struct ethhdr *)LAYER2_HEADER(skb);
  
    if(NULL == pname || NULL == eth)
        return -1;
    else{
        if(('r' != pname[0]) && ('e' != pname[0]))
            return -1;
    }

    if((DOWN_STREAM == dir) && (((skb->fe_resource_mark & ETHER_WAN_MASK) >> ETHER_WAN_OFFSET)==1)){
        memcpy(devMacAddr, eth->h_dest, ETH_ALEN);
        acnt_mode = feResManage->acnt0_dw_mode;
        meter_mode = feResManage->dw_stream_mode;
    }else if(UP_STREAM == dir){
        memcpy(devMacAddr, eth->h_source, ETH_ALEN);
        acnt_mode = feResManage->acnt0_up_mode;
        meter_mode = feResManage->up_stream_mode;
    }else
        return -1;

    if((acnt_mode != DEV_MODE) && (meter_mode != DEV_MAC_MODE)){
        return -1;
    }

	if(gHwBandwidthList && gHwBandwidthList->enable == 0)
		return -1;

    idx = findMacMatchItem(devMacAddr);
    if(idx < 0)
        return -1;
    else
        mask = idx+1;

    if(acnt_mode == DEV_MODE){
        skb->fe_resource_mark &= ~(ACNT0_MASK) ;
        skb->fe_resource_mark |= ((mask << ACNT0_OFFSET) & ACNT0_MASK);/*if mask is zero ,mean bind to 1f */
    }
    if(meter_mode == DEV_MAC_MODE){
        skb->fe_resource_mark &= ~(MTR_MASK) ;
        skb->fe_resource_mark |= (mask & MTR_MASK);
    }
    
    return 0;
}

int fe_remark_skb_from_wan_idx( struct sk_buff *skb, u8 wan_index, u8 dir)
{ 
    FE_Meter_Mode_t mode;
    
    if( (NULL == skb) || (NULL == feResManage) )
        return -1 ;

    if(DOWNSTREAM_ONLY == dir)
        mode = feResManage->dw_stream_mode;
    else if(UPSTREAM_ONLY == dir)
        mode = feResManage->up_stream_mode;
    else
        return -1;

    if(mode != WAN_MODE){
        return -1;
    }    
    skb->fe_resource_mark &= ~(MTR_MASK) ;
    skb->fe_resource_mark |= ((wan_index+1) & MTR_MASK);
    return 0;
}


/******************************************************************************
 Descriptor:    It's used to init acnt0 resource
 Input Args:  
 Ret Value: 
******************************************************************************/
int fe_init_acnt0_resource_from_mode(int dir, FE_ACNT_Mode_t acnt_mode)
{ 
    int idx = 0;
    AcntManage_T acntManage;

    if(feResManage == NULL)
        return 0;

    if(acnt_mode == DEV_MODE)
    {
	    for(idx = 0; idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx++)
	    {
            acntManage.acnt_type = ACNT0_TYPE;
            if (dir == UP_STREAM)
            {
                strlcpy(acntManage.bind_func_name, "devBandWidth", NAME_LENGTH); 
	        	if(wan_conn_type == 4)
	        	{
	        		QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_WAN, &acntManage);
	        	}else{
	        		QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_LAN, &acntManage);
	        	}
	        	feResManage->ACNT0_DEV_FLOW_UP[idx] = acntManage.acnt_id;
            }
            if (dir == DOWN_STREAM)
            {
                strlcpy(acntManage.bind_func_name, "devBandWidth", NAME_LENGTH);
	        	QDMA_API_ALLOCATE_ACNT(ECNT_QDMA_LAN, &acntManage);
	        	feResManage->ACNT0_DEV_FLOW_DOWN[idx] = acntManage.acnt_id;
            }
        } 
    }
    else{
        return 0;
    }

    return 0;
}

/******************************************************************************
 Descriptor:    It's used to deinit acnt0 resource
 Input Args:    
 Ret Value:
******************************************************************************/
int fe_deinit_acnt0_resource_from_mode(int dir, FE_ACNT_Mode_t acnt_mode)
{ 
    int idx = 0;
    AcntManage_T acntManage;

     if(feResManage == NULL)
        return 0;
     
    if(acnt_mode == DEV_MODE)
    {
        for(idx = 0; idx < (old_wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx++)
        {
            acntManage.acnt_type = ACNT0_TYPE;
            if (dir == UP_STREAM)
            {
                acntManage.acnt_id = feResManage->ACNT0_DEV_FLOW_UP[idx];
                if(old_wan_conn_type == 4)
                {
	        		QDMA_API_FREE_ACNT(ECNT_QDMA_WAN, &acntManage);
	        	}else{
	        		QDMA_API_FREE_ACNT(ECNT_QDMA_LAN, &acntManage);
	        	}
                feResManage->ACNT0_DEV_FLOW_UP[idx] = ACNT0_DEFAULT;
            }
            if (dir == DOWN_STREAM)
            {
                acntManage.acnt_id = feResManage->ACNT0_DEV_FLOW_DOWN[idx];
                QDMA_API_FREE_ACNT(ECNT_QDMA_LAN, &acntManage);
                feResManage->ACNT0_DEV_FLOW_DOWN[idx] = ACNT0_DEFAULT;
            }
        }
    }else{
        return 0;
    }

    return 0;
}

/******************************************************************************


/******************************************************************************
 Descriptor:    It's used to init acnt1 resource
 Input Args:  
 Ret Value: 
******************************************************************************/
int fe_init_acnt1_resource_from_mode(int dir,  FE_ACNT_Mode_t acnt_mode)
{ 
    int idx = 0;
    AcntManage_T  acntManage;

    if(feResManage == NULL)
        return 0;

    if(acnt_mode == TRAFFIC_FLOW_MODE){
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE;
            if (dir == UP_STREAM){
                strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_UP", NAME_LENGTH);    
                FE_GENERAL_ALLOC_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_UP[idx] = acntManage.acnt_id ; 
                sprintf(feResManage->ACNT1_TRAFFIC_FLOW_UP_NAME[idx], "TRAFFIC_FLOW_%d", idx);
            }
            if (dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_DW", NAME_LENGTH);   
                FE_GENERAL_ALLOC_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx] = acntManage.acnt_id ; 
                sprintf(feResManage->ACNT1_TRAFFIC_FLOW_DOWN_NAME[idx], "TRAFFIC_FLOW_%d", idx);
            }
        } 
    }else if(acnt_mode == WAN_ITF_MODE){
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE;
            if (dir == UP_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_UP", NAME_LENGTH);    
                FE_GENERAL_ALLOC_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_UP[idx] = acntManage.acnt_id ; 
            }
            if (dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_DW", NAME_LENGTH);     
                FE_GENERAL_ALLOC_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_DOWN[idx] = acntManage.acnt_id ; 
            }
        } 
    }else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
        for(idx = 0; idx < WAN_ITF_HAS_FLOW_ACNT_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE;
            if (dir == UP_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_UP", NAME_LENGTH);    
                FE_GENERAL_ALLOC_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_UP[idx] = acntManage.acnt_id ; 
            }
            if (dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_DW", NAME_LENGTH);     
                FE_GENERAL_ALLOC_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_DOWN[idx] = acntManage.acnt_id ; 
            }
        }
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE;
            if (dir == UP_STREAM){
                strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_UP", NAME_LENGTH);    
                FE_GENERAL_ALLOC_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_UP[idx] = acntManage.acnt_id ; 
                sprintf(feResManage->ACNT1_TRAFFIC_FLOW_UP_NAME[idx], "TRAFFIC_FLOW_%d", idx);
            }
            if (dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_DW", NAME_LENGTH);   
                FE_GENERAL_ALLOC_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx] = acntManage.acnt_id ; 
                sprintf(feResManage->ACNT1_TRAFFIC_FLOW_DOWN_NAME[idx], "TRAFFIC_FLOW_%d", idx);
            }
        } 
    }else{
        return 0;
    }

    return 0;
}


/******************************************************************************
 Descriptor:    It's used to deinit acnt1 resource
 Input Args:    
 Ret Value:
******************************************************************************/
int fe_deinit_acnt1_resource_from_mode(int dir,  FE_ACNT_Mode_t acnt_mode)
{ 
    int idx = 0;
    AcntManage_T  acntManage;

     if(feResManage == NULL)
        return 0;
     
    if(acnt_mode == TRAFFIC_FLOW_MODE){
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE ;
            if (dir == UP_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx] ; 
                FE_GENERAL_FREE_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_UP[idx] =ACNT1_DEFAULT;
            }
            if (dir == DOWN_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx] ; 
                FE_GENERAL_FREE_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx]= ACNT1_DEFAULT;
            }
        }

    }else if(acnt_mode == WAN_ITF_MODE){
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE;
            if (dir == UP_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_WAN_ITF_UP[idx] ; 
                FE_GENERAL_FREE_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_UP[idx] = ACNT1_DEFAULT; 
            }
            if (dir == DOWN_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_WAN_ITF_DOWN[idx] ; 
                FE_GENERAL_FREE_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_DOWN[idx] =ACNT1_DEFAULT; 
            }

        } 

    }else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
        for(idx = 0; idx < WAN_ITF_HAS_FLOW_ACNT_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE;
            if (dir == UP_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_WAN_ITF_UP[idx] ; 
                FE_GENERAL_FREE_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_UP[idx] = ACNT1_DEFAULT; 
            }
            if (dir == DOWN_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_WAN_ITF_DOWN[idx] ; 
                FE_GENERAL_FREE_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_WAN_ITF_DOWN[idx] =ACNT1_DEFAULT; 
            }

        }
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            acntManage.acnt_type = ACNT1_TYPE ;
            if (dir == UP_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx] ; 
                FE_GENERAL_FREE_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_UP[idx] =ACNT1_DEFAULT;
            }
            if (dir == DOWN_STREAM){
                acntManage.acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx] ; 
                FE_GENERAL_FREE_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx]= ACNT1_DEFAULT;
            }
        }
    }else{
        return 0;
    }

    return 0;
}

/******************************************************************************
 Descriptor:    It's used to init acnt2 resource
 Input Args:  
 Ret Value: 
******************************************************************************/
int fe_init_acnt2_resource_from_mode(int dir,  FE_ACNT_Mode_t acnt_mode)
{ 
    int idx = 0;
    AcntManage_T  acntManage;

    if(feResManage == NULL)
        return 0;

    if(acnt_mode == TRAFFIC_FLOW_MODE){
        for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
            acntManage.acnt_type = PPE_ACNT2_TYPE;           
            if (dir == UP_STREAM){
                strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_UP", NAME_LENGTH);    
                FE_GENERAL_ALLOC_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT2_TRAFFIC_FLOW_UP[idx] = acntManage.acnt_id ; 
                sprintf(feResManage->ACNT2_TRAFFIC_FLOW_UP_NAME[idx], "TRAFFIC_FLOW_%d", idx);                
            }
            if (dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "TRAFFIC_FLOW_DW", NAME_LENGTH);   
                FE_GENERAL_ALLOC_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT2_TRAFFIC_FLOW_DOWN[idx] = acntManage.acnt_id ; 
                sprintf(feResManage->ACNT2_TRAFFIC_FLOW_DOWN_NAME[idx], "TRAFFIC_FLOW_%d", idx);
            }
        } 
    } else if(acnt_mode == WAN_ITF_MODE){
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            acntManage.acnt_type = PPE_ACNT2_TYPE;
            if (dir == UP_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_UP", NAME_LENGTH);    
                FE_GENERAL_ALLOC_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT2_WAN_ITF_UP[idx] = acntManage.acnt_id ; 
            }
            if (dir == DOWN_STREAM){
                strlcpy(acntManage.bind_func_name, "WAN_ITF_DW", NAME_LENGTH);     
                FE_GENERAL_ALLOC_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT2_WAN_ITF_DOWN[idx] = acntManage.acnt_id ; 
            }
        } 
    }else{
        return 0;
    }

    return 0;
}


/******************************************************************************
 Descriptor:    It's used to deinit acnt1 resource
 Input Args:    
 Ret Value:
******************************************************************************/
int fe_deinit_acnt2_resource_from_mode(int dir,  FE_ACNT_Mode_t acnt_mode)
{ 
    int idx = 0;
    AcntManage_T  acntManage;

    if(feResManage == NULL)
        return 0;

    if(acnt_mode == TRAFFIC_FLOW_MODE){
         for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
             acntManage.acnt_type = PPE_ACNT2_TYPE ;
             if (dir == UP_STREAM){
                 acntManage.acnt_id = feResManage->ACNT2_TRAFFIC_FLOW_UP[idx] ; 
                 FE_GENERAL_FREE_ACNT(UP_STREAM, &acntManage);
                 feResManage->ACNT2_TRAFFIC_FLOW_UP[idx] =ACNT2_DEFAULT;
             }
             if (dir == DOWN_STREAM){
                 acntManage.acnt_id = feResManage->ACNT2_TRAFFIC_FLOW_DOWN[idx] ; 
                 FE_GENERAL_FREE_ACNT(DOWN_STREAM, &acntManage);
                 feResManage->ACNT2_TRAFFIC_FLOW_DOWN[idx]= ACNT2_DEFAULT;
             }
         }
    }else if(acnt_mode == WAN_ITF_MODE){
        for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
            acntManage.acnt_type = PPE_ACNT2_TYPE;
            if (dir == UP_STREAM){
                acntManage.acnt_id = feResManage->ACNT2_WAN_ITF_UP[idx] ; 
                FE_GENERAL_FREE_ACNT(UP_STREAM, &acntManage);
                feResManage->ACNT2_WAN_ITF_UP[idx] = ACNT2_DEFAULT; 
            }
            if (dir == DOWN_STREAM){
                acntManage.acnt_id = feResManage->ACNT2_WAN_ITF_DOWN[idx] ; 
                FE_GENERAL_FREE_ACNT(DOWN_STREAM, &acntManage);
                feResManage->ACNT2_WAN_ITF_DOWN[idx] =ACNT2_DEFAULT; 
            }
        } 
    }else{
        return 0;
    }

    return 0;
}

int fe_disable_meter(FE_Dir_t dir, unchar meter_id)
{
#if defined(TCSUPPORT_CPU_EN7580)
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
    {
    	if(old_wan_conn_type == 4)
        mainType_val = ECNT_QDMA_WAN;
    else
        mainType_val = ECNT_QDMA_LAN;
    }else
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
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}
int fe_add_meter(FE_Dir_t dir, FE_Meter_Mode_t mode)
{
#if defined(TCSUPPORT_CPU_EN7580)
    int idx = 0;
    QDMA_MeterManage_T meterManage;

    if(feResManage == NULL)
        return -1;
    
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
    		for(idx = 0; idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx++)
	    	{
	            if(dir == UP_STREAM) 
	            {
        		strlcpy(meterManage.bind_func_name, "DEV_MAC_UP", NAME_LENGTH);
	            	if(wan_conn_type == 4)
	            	{
        		QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
	                }else{
	                	QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
	                }
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
                strlcpy(meterManage.bind_func_name, "WAN_UP", NAME_LENGTH);
        		QDMA_API_ALLOCATE_METER(ECNT_QDMA_WAN, &meterManage);
        		feResManage->Meter_WAN_FLOW_UP[idx] = meterManage.meter_id;
            } else {
                strlcpy(meterManage.bind_func_name, "WAN_DOWN", NAME_LENGTH);
        		QDMA_API_ALLOCATE_METER(ECNT_QDMA_LAN, &meterManage);
        		feResManage->Meter_WAN_FLOW_DW[idx] = meterManage.meter_id;
            }
        }
        break;  
		
        default:
        break;
    }
    
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

int fe_remove_meter(FE_Dir_t dir, FE_Meter_Mode_t mode)
{
#if defined(TCSUPPORT_CPU_EN7580)
    int idx = 0;

    
    if(feResManage == NULL)
        return -1;
    
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
        		QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->TRAFFIC_QOS_UP[idx]);
                fe_disable_meter(dir, feResManage->TRAFFIC_QOS_UP[idx]);
                feResManage->TRAFFIC_QOS_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
        		QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->TRAFFIC_QOS_DW[idx]);
                fe_disable_meter(dir, feResManage->TRAFFIC_QOS_DW[idx]);
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
        	for(idx = 0; idx < (old_wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx++)
	        {
	            if(dir == UP_STREAM) 
	            {
	            	if(old_wan_conn_type == 4)
	            	{
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->Meter_DEV_FLOW_UP[idx]);
	                }else{
	                	QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->Meter_DEV_FLOW_UP[idx]);
	                }
                fe_disable_meter(dir, feResManage->Meter_DEV_FLOW_UP[idx]);
                feResManage->Meter_DEV_FLOW_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->Meter_DEV_FLOW_DW[idx]);
                fe_disable_meter(dir, feResManage->Meter_DEV_FLOW_DW[idx]);
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
                QDMA_API_FREE_METER(ECNT_QDMA_WAN, feResManage->Meter_WAN_FLOW_UP[idx]);
                fe_disable_meter(dir, feResManage->Meter_WAN_FLOW_UP[idx]);
                feResManage->Meter_WAN_FLOW_UP[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            } else {
                QDMA_API_FREE_METER(ECNT_QDMA_LAN, feResManage->Meter_WAN_FLOW_DW[idx]);
                fe_disable_meter(dir, feResManage->Meter_WAN_FLOW_DW[idx]);
                feResManage->Meter_WAN_FLOW_DW[idx] = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
            }
        }
        break;
		
        default:
        break;
    }
    
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif

}

int fe_api_get_wan_itf_index(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    int i;
    char wan_itf_name[IFNAMSIZ];
    unchar itf_idx = 0xFF;
    memcpy(wan_itf_name, fe_data->api_data.meter_cfg.itf_name, IFNAMSIZ);

    for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
        if(!wan_itf_info[i].valid)
            continue;
        if(strcmp(wan_itf_info[i].wan_dev->name, wan_itf_name) == 0){
            itf_idx = i;
            break;
        }
    }
    
    fe_data->api_data.meter_cfg.idx = itf_idx;

    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

#if defined(TCSUPPORT_CPU_EN7527)
static int fe_mark_wan_itf_idx(struct sk_buff * skb, struct net_device *dev, int dir)
{
    int i;
    
    if(dev == NULL)
        return 0;

    if(!is_wan_interface(dev))
        return 0;

    if(feResManage == NULL)
        return 0;

    if(((dir == UP_STREAM) && (feResManage->acnt1_up_mode == WAN_ITF_MODE)) || 
        ((dir == DOWN_STREAM) && (feResManage->acnt1_dw_mode == WAN_ITF_MODE))){
        for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
            if((wan_itf_info[i].valid) && 
                (dev == wan_itf_info[i].wan_dev)){
                skb->fe_resource_mark &= ~(ACNT1_MASK) ;
                skb->fe_resource_mark |= (((i+1) << ACNT1_OFFSET) & ACNT1_MASK);
            }    
        }
    }
    
    if(((dir == UP_STREAM) && (feResManage->acnt2_up_mode == WAN_ITF_MODE)) || 
        ((dir == DOWN_STREAM) && (feResManage->acnt2_dw_mode == WAN_ITF_MODE))){
        for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
            if((wan_itf_info[i].valid) && 
                (dev == wan_itf_info[i].wan_dev)){
                skb->fe_resource_mark &= ~(ACNT2_MASK) ;
                skb->fe_resource_mark |= (((i+1) << ACNT2_OFFSET) & ACNT2_MASK);
            }    
        }
    }

    return 0;
}

ecnt_ret_val ecnt_core_wanItfToAcntUp(struct ecnt_data *in_data){
    struct net_data_s * net_data = (struct net_data_s *)in_data;
    struct sk_buff * skb = *(net_data->pskb);       
    struct net_device *dev = skb->dev;

    fe_mark_wan_itf_idx(skb, dev, UP_STREAM);

    return ECNT_CONTINUE;
}

ecnt_ret_val ecnt_core_wanItfToAcntDown(struct ecnt_data *in_data){
    struct net_data_s * net_data = (struct net_data_s *)in_data;
    struct sk_buff * skb = *(net_data->pskb);       
    struct net_device *dev = skb->dev;

    fe_mark_wan_itf_idx(skb, dev, DOWN_STREAM);

    return ECNT_CONTINUE;
}

ecnt_ret_val ecnt_vlan_wanItfToAcntDown(struct ecnt_data *in_data){
    struct net_data_s * net_data = (struct net_data_s *)in_data;
    struct sk_buff * skb = *(net_data->pskb);       
    struct net_device *dev = net_data->vlan_skb_rcv_data.vlan_dev;

    fe_mark_wan_itf_idx(skb, dev, DOWN_STREAM);

    return ECNT_CONTINUE;
}

struct ecnt_hook_ops ecnt_core_wanItfToAcntDown_op = {
    .name = "ecnt_core_wanItfToAcntDown",
    .is_execute = 1,
    .hookfn = ecnt_core_wanItfToAcntDown,
    .maintype = ECNT_NET_CORE_DEV,
    .subtype = ECNT_NETIF_RCV_SKB,
    .priority = 1
};

struct ecnt_hook_ops ecnt_core_wanItfToAcntUp_op = {
    .name = "ecnt_core_wanItfToAcntUp",
    .is_execute = 1,
    .hookfn = ecnt_core_wanItfToAcntUp,
    .maintype = ECNT_NET_CORE_DEV,
    .subtype = ECNT_DEV_QUE_XMIT,
    .priority = 1
};

struct ecnt_hook_ops ecnt_vlan_wanItfToAcntDown_op = {
    .name = "ecnt_vlan_wanItfToAcntUp",
    .is_execute = 1,
    .hookfn = ecnt_vlan_wanItfToAcntDown,
    .maintype = ECNT_NET_VLAN_DEV,
    .subtype = ECNT_VLAN_SKB_RECV,
    .priority = 1
};
#endif
/******************************************************************************
 Descriptor:    It's used to init fe_resource_manage variables 
 Input Args:    
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_init_resource_manage(struct ecnt_fe_data *fe_data)
{
#if	defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    int idx = 0;
    struct proc_dir_entry *fe_resource_manage_proc = NULL;
    QDMA_MeterManage_T meterManage;
    QDMA_AcntManage_T  acntManage;
    GENERAL_TrtcmCfg_T rxTrtcmCfg; 
    uint mainType_val=0;
    FE_Dir_t dir;
    
    printk("fe resource init start\n");

	if(NULL == feResManage)
		feResManage = (Fe_Resource_Manage_T *)kzalloc(sizeof(Fe_Resource_Manage_T), GFP_KERNEL);   
    memset(feResManage, 0x0, sizeof(Fe_Resource_Manage_T));
    memset(&meterManage, 0, sizeof(QDMA_MeterManage_T)) ;
    memset(&acntManage, 0, sizeof(QDMA_AcntManage_T)) ;
    memset(&rxTrtcmCfg, 0, sizeof(GENERAL_TrtcmCfg_T)) ;
    memset(ip_info_up, 0x0, sizeof(ip_info_up));
    memset(ip_info_dw, 0x0, sizeof(ip_info_dw));
    memset(itf_info_up, 0x0, sizeof(itf_info_up));
    memset(itf_info_dw, 0x0, sizeof(itf_info_dw));

    feResManage->up_stream_mode = NULL_BIND_MODE;
    feResManage->dw_stream_mode = NULL_BIND_MODE;
    feResManage->acnt0_up_mode = DEV_MODE;
    feResManage->acnt0_dw_mode = DEV_MODE;
#if defined(TCSUPPORT_CPU_EN7580)
#if defined(TCSUPPORT_CMCCV2)
    feResManage->acnt1_up_mode = WAN_ITF_AND_TRAFFIC_FLOW_MODE;
    feResManage->acnt1_dw_mode = WAN_ITF_AND_TRAFFIC_FLOW_MODE;
#else
    feResManage->acnt1_up_mode = TRAFFIC_FLOW_MODE;
    feResManage->acnt1_dw_mode = TRAFFIC_FLOW_MODE;
#endif
	feResManage->acnt2_up_mode = DEFAULT_NULL_MODE;
	feResManage->acnt2_dw_mode = DEFAULT_NULL_MODE;
#elif defined(TCSUPPORT_CPU_EN7527)
    feResManage->acnt1_up_mode = WAN_ITF_MODE;
    feResManage->acnt1_dw_mode = WAN_ITF_MODE;
    feResManage->acnt2_up_mode = TRAFFIC_FLOW_MODE;
    feResManage->acnt2_dw_mode = TRAFFIC_FLOW_MODE;
#endif
    feResManage->Meter_Ctl_By_Olt = 0;


#if defined(TCSUPPORT_CPU_EN7580)
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

    for(idx = 0; idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); idx ++){
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
#endif

    fe_init_acnt0_resource_from_mode(UP_STREAM, feResManage->acnt0_up_mode);
	fe_init_acnt0_resource_from_mode(DOWN_STREAM, feResManage->acnt0_dw_mode);

    fe_init_acnt1_resource_from_mode(UP_STREAM, feResManage->acnt1_up_mode);
    fe_init_acnt1_resource_from_mode(DOWN_STREAM, feResManage->acnt1_dw_mode);
    fe_init_acnt2_resource_from_mode(UP_STREAM, feResManage->acnt2_up_mode);
    fe_init_acnt2_resource_from_mode(DOWN_STREAM, feResManage->acnt2_dw_mode);

    /*init acnt0  resource for , for dev mac  stream*/
#if defined(TCSUPPORT_CPU_EN7580)
    fe_resource_manage_proc = create_proc_entry(meter_qdmawan_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_meter_qdmawan_state_read_proc;
    }
    
    fe_resource_manage_proc = create_proc_entry(meter_qdmalan_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_meter_qdmalan_state_read_proc;
    }
#endif
    fe_resource_manage_proc = create_proc_entry(fe_ratelimit_up_info_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_ratelimit_up_info_read_proc;
    }

    fe_resource_manage_proc = create_proc_entry(fe_ratelimit_dw_info_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_ratelimit_dw_info_read_proc;
    }

    fe_resource_manage_proc = create_proc_entry(fe_acnt2_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_acnt2_mapping_state_read_proc;
    }

    fe_resource_manage_proc = create_proc_entry(fe_acnt1_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_acnt1_mapping_state_read_proc;
    }
    
    fe_resource_manage_proc = create_proc_entry(fe_acnt0_mapping_path, 0, NULL) ;
    if(fe_resource_manage_proc) {
        fe_resource_manage_proc->read_proc = fe_acnt0_mapping_state_read_proc;
    }
    
    fe_resource_manage_proc = create_proc_entry("tc3162/fe_default_wan_itf", 0, NULL);
    fe_resource_manage_proc->read_proc = fe_default_wan_itf_read_proc;
    fe_resource_manage_proc->write_proc =  fe_default_wan_itf_write_proc;

#if defined(TCSUPPORT_CT_JOYME2)
	fe_resource_manage_proc = create_proc_entry("tc3162/update_multicast_cnt", 0, NULL);
    fe_resource_manage_proc->read_proc = update_multicast_cnt_read_proc;
    fe_resource_manage_proc->write_proc =  update_multicast_cnt_write_proc;
#endif

    rcu_assign_pointer(fe_resource_mark_meter_hook, fe_remark_skb_mark_from_meter);
    rcu_assign_pointer(fe_resource_mark_acnt_hook, fe_remark_skb_from_dev_mac);
    rcu_assign_pointer(fe_resource_mark_wan_idx_hook, fe_remark_skb_from_wan_idx);

#if defined(TCSUPPORT_CPU_EN7527)
    ecnt_register_hook(&ecnt_core_wanItfToAcntUp_op);
    ecnt_register_hook(&ecnt_core_wanItfToAcntDown_op);
    ecnt_register_hook(&ecnt_vlan_wanItfToAcntDown_op);

    //register_netdevice_notifier(&fe_res_notifier_block);
#endif 

    /*ecnt_register_hook(&ecnt_fe_resource_ops);*/

    return 0;
#else
    rcu_assign_pointer(fe_resource_mark_meter_hook, NULL);
    rcu_assign_pointer(fe_resource_mark_acnt_hook, NULL);
    rcu_assign_pointer(fe_resource_mark_wan_idx_hook, NULL);
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to deinit fe_resource 
 Input Args:    
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_deinit_resource_manage(struct ecnt_fe_data *fe_data)
{
#if	defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    int idx = 0;
    AcntManage_T  acntManage;
    memset(&acntManage, 0, sizeof(AcntManage_T)) ;

    if(feResManage == NULL)
        return -1;
    
    fe_remove_meter(DOWN_STREAM, feResManage->dw_stream_mode);
    fe_remove_meter(UP_STREAM, feResManage->up_stream_mode);

    for(idx = 0; idx < TRAFFIC_FLOW_MAX_NUM; idx ++){
        acntManage.acnt_type = ACNT1_TYPE ;
        acntManage.acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx] ; 
        FE_GENERAL_FREE_ACNT(UP_STREAM, &acntManage);

        acntManage.acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx] ; 
        FE_GENERAL_FREE_ACNT(DOWN_STREAM, &acntManage);
    }

#if defined(TCSUPPORT_CPU_EN7580)
    remove_proc_entry(meter_qdmawan_mapping_path, NULL) ;
    remove_proc_entry(meter_qdmalan_mapping_path, NULL) ;
#endif
    remove_proc_entry(fe_ratelimit_up_info_path, NULL) ;
    remove_proc_entry(fe_ratelimit_dw_info_path, NULL) ;
    remove_proc_entry(fe_acnt2_mapping_path, NULL) ;
    remove_proc_entry(fe_acnt1_mapping_path, NULL) ;
    remove_proc_entry(fe_acnt0_mapping_path, NULL) ;
    remove_proc_entry("tc3162/fe_default_wan_itf", NULL) ;
#if defined(TCSUPPORT_CT_JOYME2)
	remove_proc_entry("tc3162/update_multicast_cnt", NULL);
#endif

    rcu_assign_pointer(fe_resource_mark_meter_hook, NULL);
    rcu_assign_pointer(fe_resource_mark_acnt_hook, NULL);

#if defined(TCSUPPORT_CPU_EN7527) 
    ecnt_unregister_hook(&ecnt_core_wanItfToAcntUp_op);
    ecnt_unregister_hook(&ecnt_core_wanItfToAcntDown_op);
    ecnt_unregister_hook(&ecnt_vlan_wanItfToAcntDown_op);

    //unregister_netdevice_notifier(&fe_res_notifier_block);
#endif

    /*ecnt_unregister_hook(&ecnt_fe_resource_ops);*/
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

static inline int get_wan_itf_idx_by_name(char *wan_name)
{
    int i;
    
    for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
        if(wan_itf_info[i].valid == 0)
            continue;
        
        if(strcmp(wan_itf_info[i].wan_dev->name, wan_name) == 0)
            return i;
    }

    return WAN_ITF_ACNT_NUM;
}

/******************************************************************************
 Descriptor:    It's used to set meter ratalimit according different bind mode (set meter ratelimit according meter id)
 Input Args:    args need in GENERAL_TrtcmRatelimitCfg_T and GENERAL_TrtcmRatelimitSet_T
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_set_meter_ratelimit(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580)
    uint mainType_val=0, ratelimit = 0,bucket_size=0;
    unchar meter_id = 0x7f;
    unchar idx=0;
    FE_Meter_Mode_t mode;
    FE_Dir_t dir;
    FE_TickSel_t tick_sel;
    FE_Packet_Mode_t pkt_mode;
    FE_Enable_t en_mode;
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
    
    ratelimit = fe_data->api_data.meter_cfg.rate;/*ratelimit value.unit KBps*/
    en_mode = fe_data->api_data.meter_cfg.enable;/*0 disable, or 1enable meter ratelimit*/
    pkt_mode = fe_data->api_data.meter_cfg.pkt_mode;
    tick_sel = fe_data->api_data.meter_cfg.tick_sel;
    idx = fe_data->api_data.meter_cfg.idx;/*idx */
    dir = fe_data->api_data.meter_cfg.dir;/*0:down stream 1:up stream*/
    mode = fe_data->api_data.meter_cfg.mode;/*0: car_qos_mode ;1:IP bind ratelimit 2:If bind ratelimit */
    bucket_size = fe_data->api_data.meter_cfg.bucket_size;

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
                if (dir == UP_STREAM)
                    meter_id = feResManage->TRAFFIC_QOS_UP[idx]; 
                else
                    meter_id = feResManage->TRAFFIC_QOS_DW[idx]; 
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", TRAFFIC_QOS_NUM-1, idx);
                return -1;
            }
            break;
        case DEV_MAC_MODE:
            if(idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20)){
                if (dir == UP_STREAM){
                    meter_id = feResManage->Meter_DEV_FLOW_UP[idx]; 
                }else{
                    meter_id = feResManage->Meter_DEV_FLOW_DW[idx]; 
                }
            }else{
                printk("idx value error, correct range: 0-%d\n ,real idx :%d", (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20)-1, idx);
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
            idx = get_wan_itf_idx_by_name(fe_data->api_data.meter_cfg.itf_name);		
            if(idx < WAN_ITF_ACNT_NUM){
                if(wan_itf_info[idx].is_vr_wan)
                {
                    if(dir == UP_STREAM)
                        meter_id = OFFLOAD_RX_RING_2;
                    else
                        meter_id = OFFLOAD_RX_RING;
                    
                    goto set_ratelimit;
                }	
                if (dir == UP_STREAM){
                    meter_id = feResManage->Meter_WAN_FLOW_UP[idx]; 
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

    if(meter_id < RX_RING_NUM)
        return 0;

set_ratelimit:
    if (dir == UP_STREAM)
    {
    	if(wan_conn_type == 4)
        mainType_val = ECNT_QDMA_WAN;
    else
        mainType_val = ECNT_QDMA_LAN;
    }else
        mainType_val = ECNT_QDMA_LAN;
    
    rxTrtcmCfg.trtcmModule = INGRESS_TRTCM ;
    QDMA_API_GET_GENERAL_TRTCM_INIT(mainType_val, &rxTrtcmCfg);
	
	if(((rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) && (meter_id == QDMA_INGRESS_INDEX_MAX))
		|| ((rxTrtcmCfg.trtcmMode == TRTCM_MODE) && (meter_id == (QDMA_INGRESS_INDEX_MAX>>1)))){
		return 0;
	}
        
    if(rxTrtcmCfg.trtcmMode == TRTCM_RATELIMIT_MODE) {
        rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;
        rxRateLimitCfg.Index = meter_id;

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
        rxTrtcmCbsPbsCfg.Index = meter_id;
        
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
            rxRateLimitSet.Index = meter_id;
            QDMA_API_GET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);   

            rxRateLimitSet.RateLimitValue = ratelimit;
            QDMA_API_SET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);
        } else {
            rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
            rxTrtcmCbsPbsSet.Index = meter_id;
            QDMA_API_GET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);   

            rxTrtcmCbsPbsSet.CbsParamValue = ratelimit;
            //if(mode == CAR_QOS_MODE)
                //rxTrtcmCbsPbsSet.PbsParamValue = MAX(ratelimit*10, 1000000);
            //else
                rxTrtcmCbsPbsSet.PbsParamValue = ratelimit;
            rxTrtcmCbsPbsSet.CbsBucketSize = rxTrtcmCbsPbsSet.CbsParamValue/1000;
            rxTrtcmCbsPbsSet.PbsBucketSize = rxTrtcmCbsPbsSet.PbsParamValue/1000;
            QDMA_API_SET_GENERAL_TRTCM_VALUE(mainType_val, &rxTrtcmCbsPbsSet);
        }
    }

    PPE_API_CLEAN_FOE_TABLE();
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to get meter ratalimit value different bind mode (set meter ratelimit according meter id)
 Input Args:    args need in GENERAL_TrtcmRatelimitCfg_T and GENERAL_TrtcmRatelimitSet_T
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_get_meter_ratelimit(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580)
    uint mainType_val=0;
    unchar meter_id = 0x7f;
    unchar idx=0;
    FE_Meter_Mode_t mode;
    FE_Dir_t dir;
    
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
                meter_id = feResManage->TRAFFIC_QOS_UP[idx]; 
            else
                meter_id = feResManage->TRAFFIC_QOS_DW[idx]; 
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", TRAFFIC_QOS_NUM-1, idx);
            return -1;
        }
    }else if(DEV_MAC_MODE == mode){
        if(idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20)){
            if (dir == UP_STREAM)
                meter_id = feResManage->Meter_DEV_FLOW_UP[idx]; 
            else
                meter_id = feResManage->Meter_DEV_FLOW_DW[idx]; 
        }else{
            printk("idx value error, correct range: 0-%d\n ,real idx :%d", (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20)-1, idx);
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

    if (dir == UP_STREAM)
        mainType_val = ECNT_QDMA_WAN;
    else
        mainType_val = ECNT_QDMA_LAN;
 
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
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to get meter if is enable or disable
 Input Args:    args need in GENERAL_TrtcmRatelimitCfg_T and GENERAL_TrtcmRatelimitSet_T
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_get_meter_enable(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580)
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
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}


/******************************************************************************
 Descriptor:    It's used to get real meter index in qdma lan or qdma wan.
 Input Args:    
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_get_meter_idx(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580)
    unchar meter_id = 0x7F;
	unchar meter_id_default = 0x7F;
    uint mainType_val = 0;
    int qos_flag_num = -1;
    int qos_idx = 0;
    unchar idx = 0;
    FE_Meter_Mode_t mode; /*changed in other mode ,could be extended */
    FE_Dir_t dir;
    
    struct sk_buff *skb = fe_data->api_data.meter_cfg.skb ;
    dir = fe_data->api_data.meter_cfg.dir;
    idx = fe_data->api_data.meter_cfg.idx;
    
    if((NULL == skb) || (NULL == feResManage))
        return -1 ;
    
    if (dir == UP_STREAM){
        mainType_val = ECNT_QDMA_WAN;
        mode = feResManage->up_stream_mode;
    }else{
        mainType_val = ECNT_QDMA_LAN;
        mode = feResManage->dw_stream_mode;
    }
    meter_id_default = feResManage->ingress_ratelimit_trtcm_invalid_index[dir];
	meter_id = meter_id_default;

    switch (mode) {
        case CAR_QOS_MODE:
            QDMA_API_GET_QOS_FLAG(mainType_val, &qos_flag_num);
            if(qos_flag_num == QOS_SW_CAR){
                qos_idx = qdma_qos_mark(skb)-1;
                if((qos_idx < 0) || (qos_idx >= QOS_CAR_NUM)){
                    meter_id = meter_id_default;
                }else{
                    if(dir == UP_STREAM)
                        meter_id = feResManage->CAR_QOS_QUEUE_UP[qos_idx]; 
                    else
                        meter_id = feResManage->CAR_QOS_QUEUE_DOWN[qos_idx]; 
                }
            }else{
                meter_id = meter_id_default;
            }

            break;
        case IP_BIND_MODE:
            qos_idx = fe_get_meter_mark(skb)-1;
            if((qos_idx < 0) || (qos_idx >= IP_MAX_NUM)){
                meter_id = meter_id_default;
            }else{
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_IP_UP[qos_idx]; 
                else
                    meter_id = feResManage->Meter_IP_DOWN[qos_idx]; 
            }
            
            break;
        case IF_BIND_MODE:
            qos_idx = fe_get_meter_mark(skb)-1;
            if((qos_idx < 0) || (qos_idx >= IF_MAX_NUM)){
                meter_id = meter_id_default;
            }else{
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_IF_UP[qos_idx]; 
                else
                    meter_id = feResManage->Meter_IF_DOWN[qos_idx]; 
            }
            break;
        case OLT_CTL_MODE:
            if(dir == UP_STREAM){
                if(skb->pon_mark & QOS_TSE_MARK){
                    qos_idx = skb->pon_mark & QOS_TSID_MARK;
                    if(qos_idx < OLT_CTL_NUM)
                        meter_id = feResManage->Meter_OLT_UP[qos_idx]; 
                }
            }
            else{
                if(skb->pon_mark & DS_TRTCM_ENABLE_MARK){
                    qos_idx = skb->pon_mark & QOS_TSID_MARK;
                    if(qos_idx < OLT_CTL_NUM)
                        meter_id = feResManage->Meter_OLT_DOWN[qos_idx]; 
                }
            }
            break;
        case TRAFFIC_QOS_MODE:
            qos_idx = fe_get_traffic_queue_mark(skb)-1;
            if((qos_idx < 0) || (qos_idx >= TRAFFIC_QOS_NUM)){
                meter_id = meter_id_default;
            }else{
                if(dir == UP_STREAM)
                    meter_id = feResManage->TRAFFIC_QOS_UP[qos_idx]; 
                else
                    meter_id = feResManage->TRAFFIC_QOS_DW[qos_idx]; 
            }
            break;

        case DEV_MAC_MODE:
            qos_idx = fe_get_meter_mark(skb)-1;
            if((qos_idx < 0) || (qos_idx >= (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20))){
                meter_id = meter_id_default;
            }else{
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_DEV_FLOW_UP[qos_idx]; 
                else
                    meter_id = feResManage->Meter_DEV_FLOW_DW[qos_idx]; 
            }
            break;
        case FORCEBW_WRR_MODE:
            qos_idx = qdma_qos_mark(skb)-1;
            if((qos_idx < 0) || (qos_idx >= FORCEBW_WRR_NUM)){
                meter_id = meter_id_default;
            }else{
                if(dir == UP_STREAM)
                    meter_id = feResManage->FORCEBW_WRR_QUEUE_UP[qos_idx]; 
            }        
            break;

        case WAN_MODE:
            qos_idx = fe_get_meter_mark(skb)-1;
            if((qos_idx < 0) || (qos_idx >= WAN_ITF_ACNT_NUM)){
                meter_id = 0x7F;
            }else{
                if(dir == UP_STREAM)
                    meter_id = feResManage->Meter_WAN_FLOW_UP[qos_idx]; 
                else
                    meter_id = feResManage->Meter_WAN_FLOW_DW[qos_idx]; 

                /* for clouvr WAN, IPv4 flows */
                if(wan_itf_info[qos_idx].is_vr_wan)
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
    
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}
/******************************************************************************
 Descriptor:    It's used to get acnt2 mark  from skb->fe_resource_mark 
            and then set real acnt_grp2 id into fe_resource_mark and use it in ra_nat.c
 Input Args:    dir and skb
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_get_acnt2_idx(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
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

    if (dir == UP_STREAM){
        acnt2_mode = feResManage->acnt2_up_mode;
    }else{
        acnt2_mode = feResManage->acnt2_dw_mode;
    }
    if(acnt2_mode == TRAFFIC_FLOW_MODE)
    {
    	if(TCSUPPORT_CF_UNIFIED_PLATFORM_VAL && is_multicast_ether_addr(eth_hdr(skb)->h_dest))
    	{
            acnt2_id = (fe_get_acnt2_mark(skb) | (1<<6));
        }
        else
        {
	        idx = fe_get_traffic_queue_mark(skb)-1;
	        if((idx < 0) || (idx >= TRAFFIC_FLOW_MAX_NUM)){
	            acnt2_id = ACNT2_DEFAULT;
	        }else{
	            if(dir == UP_STREAM){
	                acnt2_id = feResManage->ACNT2_TRAFFIC_FLOW_UP[idx] | (1<<6); 
	            }else{
	                acnt2_id = feResManage->ACNT2_TRAFFIC_FLOW_DOWN[idx] | (1<<6); 
	            }
	        }
		}
    }else if(acnt2_mode == WAN_ITF_MODE)
    {
        idx = fe_get_acnt2_mark(skb) -1 ;
        if((idx < 0) || (idx >= WAN_ITF_ACNT_NUM)){
            acnt2_id = ACNT2_DEFAULT;
        }else{
            if(dir == UP_STREAM){
                acnt2_id = feResManage->ACNT2_WAN_ITF_UP[idx] | (1<<6); 
            }else{
                acnt2_id = feResManage->ACNT2_WAN_ITF_DOWN[idx] | (1<<6); 
            }
        }
    }else{
        acnt2_id = ACNT2_DEFAULT;
    }

    fe_data->api_data.meter_cfg.acnt2_id = acnt2_id;

    skb->fe_resource_mark &= ~(ACNT2_MASK) ;
    skb->fe_resource_mark |= (((acnt2_id)<< ACNT2_OFFSET ) & ACNT2_MASK);

    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif

}

/******************************************************************************
 Descriptor:    It's used to get acnt1 mark  from skb->fe_resource_mark 
            and then set real acnt_grp1 id into fe_resource_mark and use it in ra_nat.c
 Input Args:    dir and skb
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_get_acnt1_idx(struct ecnt_fe_data *fe_data)
{
#if	defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
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

    if (dir == UP_STREAM){
        acnt1_mode = feResManage->acnt1_up_mode;
    }else{
        acnt1_mode = feResManage->acnt1_dw_mode;
    }
    if(acnt1_mode == TRAFFIC_FLOW_MODE)
    {
        idx = fe_get_traffic_queue_mark(skb)-1;
        if((idx < 0) || (idx >= TRAFFIC_FLOW_MAX_NUM)){
            acnt1_id = ACNT1_DEFAULT;
        }else{
            if(dir == UP_STREAM){
#if defined(TCSUPPORT_CPU_EN7527)
                acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx]  | (1<<4); 
#else
                acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx]; 
#endif
            }else{
#if defined(TCSUPPORT_CPU_EN7527)
                acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx] | (1<<4); 
#else
                acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx];
#endif
            }
        }
    }else if(acnt1_mode == WAN_ITF_MODE)
    {
        idx = fe_get_acnt1_mark(skb) -1 ;
        if((idx < 0) || (idx >= WAN_ITF_ACNT_NUM)){
            acnt1_id = ACNT1_DEFAULT;
        }else{
            if(dir == UP_STREAM){
#if defined(TCSUPPORT_CPU_EN7527)  
                acnt1_id = feResManage->ACNT1_WAN_ITF_UP[idx] | (1<<4); 
#else
                acnt1_id = feResManage->ACNT1_WAN_ITF_UP[idx]; 
#endif
            }else{
#if defined(TCSUPPORT_CPU_EN7527) 
                acnt1_id = feResManage->ACNT1_WAN_ITF_DOWN[idx]  | (1<<4); 
#else
                acnt1_id = feResManage->ACNT1_WAN_ITF_DOWN[idx]; 
#endif
            }
        }
    } else if(acnt1_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE)
    {
        idx = fe_get_acnt1_mark(skb) -1 ;
        if((idx < 0) || (idx >= WAN_ITF_HAS_FLOW_ACNT_NUM)){
            acnt1_id = ACNT1_DEFAULT;
        }else{
            if(idx == default_wan_itf) {
                idx2 = fe_get_traffic_queue_mark(skb)-1;
                if((idx2 < 0) || (idx2 >= TRAFFIC_FLOW_MAX_NUM)){
                    idx = default_wan_itf;
                } else {
                    idx = WAN_ITF_HAS_FLOW_ACNT_NUM + idx2;
                }
            }
            if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM) {
                if(dir == UP_STREAM){
#if defined(TCSUPPORT_CPU_EN7527)  
                    acnt1_id = feResManage->ACNT1_WAN_ITF_UP[idx] | (1<<4); 
#else
                    acnt1_id = feResManage->ACNT1_WAN_ITF_UP[idx]; 
#endif
                }else{
#if defined(TCSUPPORT_CPU_EN7527) 
                    acnt1_id = feResManage->ACNT1_WAN_ITF_DOWN[idx]  | (1<<4); 
#else
                    acnt1_id = feResManage->ACNT1_WAN_ITF_DOWN[idx]; 
#endif
                }
            } else {
                if(dir == UP_STREAM){
#if defined(TCSUPPORT_CPU_EN7527)  
                    acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx-WAN_ITF_HAS_FLOW_ACNT_NUM] | (1<<4); 
#else
                    acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx-WAN_ITF_HAS_FLOW_ACNT_NUM]; 
#endif
                }else{
#if defined(TCSUPPORT_CPU_EN7527) 
                    acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx-WAN_ITF_HAS_FLOW_ACNT_NUM] | (1<<4); 
#else
                    acnt1_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx-WAN_ITF_HAS_FLOW_ACNT_NUM]; 
#endif
                }
            }
        }
    }else{
        acnt1_id = ACNT1_DEFAULT;
    }

    fe_data->api_data.meter_cfg.acnt1_id = acnt1_id;

    skb->fe_resource_mark &= ~(ACNT1_MASK) ;
    skb->fe_resource_mark |= (((acnt1_id)<< ACNT1_OFFSET ) & ACNT1_MASK);

    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}
/******************************************************************************
 Descriptor:    It's used to get acnt0 mark  from skb->fe_resource_mark /acnt0 used for dev
 Input Args:    dir and skb
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_get_acnt0_idx(struct ecnt_fe_data *fe_data)
{
#if	defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
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

    if (dir == UP_STREAM){ 
        acnt0_mode = feResManage->acnt0_up_mode;
    }else{
        acnt0_mode = feResManage->acnt0_dw_mode;
    }

    if(acnt0_mode == DEV_MODE)
    {
        idx = fe_get_acnt0_mark(skb)-1;
        if((idx < 0) || (idx >= (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20))){
            acnt0_id = ACNT0_DEFAULT;
        }else{
            if(dir == UP_STREAM)
                acnt0_id = feResManage->ACNT0_DEV_FLOW_UP[idx]; 
            else
                acnt0_id = feResManage->ACNT0_DEV_FLOW_DOWN[idx]; 
        }
    }
    

    fe_data->api_data.meter_cfg.acnt0_id = acnt0_id;

    skb->fe_resource_mark &= ~(ACNT0_MASK) ;
    skb->fe_resource_mark |= (((acnt0_id)<< ACNT0_OFFSET ) & ACNT0_MASK);
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}
/******************************************************************************
 Descriptor:    It's used to get rx ratelimit mode of down stream and up stream 
 Input Args:    dir
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_get_rx_ratelimit_mode(struct ecnt_fe_data *fe_data)
{
#if	defined(TCSUPPORT_CPU_EN7580)
	FE_Meter_Mode_t mode;
	FE_Dir_t dir ;

	dir = fe_data->api_data.meter_cfg.dir;

	if(feResManage == NULL)
		return -1;

	if(dir == DOWN_STREAM) {
		mode = feResManage->dw_stream_mode;
	} else {
		mode = feResManage->up_stream_mode;
	}

	fe_data->api_data.meter_cfg.mode = mode;
	
	return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to set rx ratelimit rule of down stream and up stream 
 Input Args:    dir and mode
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_set_rx_ratelimit_rule(struct ecnt_fe_data *fe_data)
{
#if	defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    FE_Dir_t dir ;
    FE_Meter_Mode_t mode;
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
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}
/******************************************************************************
 Descriptor:    It's used to set rx ratelimit mode of down stream and up stream 
 Input Args:    dir and mode
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_set_rx_ratelimit_mode(struct ecnt_fe_data *fe_data){
#if	defined(TCSUPPORT_CPU_EN7580)
    FE_Meter_Mode_t mode;
    FE_Meter_Mode_t old_mode;
    FE_Dir_t dir ;

    dir = fe_data->api_data.meter_cfg.dir;
    mode = fe_data->api_data.meter_cfg.mode;

    if(feResManage == NULL)
        return -1;

    if(dir == DOWN_STREAM) {
        old_mode = feResManage->dw_stream_mode;
    } else {
        old_mode = feResManage->up_stream_mode;
    }

    if(dir == DOWN_STREAM)
        feResManage->dw_stream_mode = mode;
    else    
        feResManage->up_stream_mode = mode;

    PPE_API_CLEAN_FOE_TABLE();

    if(dir == DOWN_STREAM) {
        mode = feResManage->dw_stream_mode;
    } else {
        mode = feResManage->up_stream_mode;
    }
    
    if(mode != old_mode) {
        fe_remove_meter(dir, old_mode);
        fe_add_meter(dir, mode);
    }

    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to set acnt 0 mode 
 Input Args:     
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_set_acnt0_mode(struct ecnt_fe_data *fe_data){
#if	defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    FE_ACNT_Mode_t new_acnt0_mode;
    FE_Dir_t dir ;
    FE_ACNT_Mode_t old_acnt0_mode;

    dir = fe_data->api_data.meter_cfg.dir;
    new_acnt0_mode = fe_data->api_data.meter_cfg.acnt_mode;

    
    if(feResManage == NULL)
        return -1;

    if (dir == UP_STREAM){ 
        old_acnt0_mode = feResManage->acnt0_up_mode;
        feResManage->acnt0_up_mode = new_acnt0_mode;
    }else{
        old_acnt0_mode = feResManage->acnt0_dw_mode;
        feResManage->acnt0_dw_mode = new_acnt0_mode;
    }
    
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to set acnt 1mode 
 Input Args:     
 Ret Value:  
******************************************************************************/
int fe_api_set_acnt2_mode(struct ecnt_fe_data *fe_data){
#if	defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    FE_ACNT_Mode_t new_acnt2_mode;
    FE_Dir_t dir ;
    FE_ACNT_Mode_t old_acnt2_mode;

    if(feResManage == NULL)
        return -1;

    dir = fe_data->api_data.meter_cfg.dir;
    new_acnt2_mode = fe_data->api_data.meter_cfg.acnt_mode;
    if (dir == UP_STREAM){ 
        old_acnt2_mode = feResManage->acnt2_up_mode;
        feResManage->acnt2_up_mode = new_acnt2_mode;
    }else{
        old_acnt2_mode = feResManage->acnt2_dw_mode;
        feResManage->acnt2_dw_mode = new_acnt2_mode;
    }
    fe_deinit_acnt2_resource_from_mode(dir,  old_acnt2_mode);
    fe_init_acnt2_resource_from_mode(dir,  new_acnt2_mode);


    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to set acnt 1mode 
 Input Args:     
 Ret Value:  
******************************************************************************/
int fe_api_set_acnt1_mode(struct ecnt_fe_data *fe_data){
#if	defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    FE_ACNT_Mode_t new_acnt1_mode;
    FE_Dir_t dir ;
    FE_ACNT_Mode_t old_acnt1_mode;

    if(feResManage == NULL)
        return -1;

    dir = fe_data->api_data.meter_cfg.dir;
    new_acnt1_mode = fe_data->api_data.meter_cfg.acnt_mode;
    if (dir == UP_STREAM){ 
        old_acnt1_mode = feResManage->acnt1_up_mode;
        feResManage->acnt1_up_mode = new_acnt1_mode;
    }else{
        old_acnt1_mode = feResManage->acnt1_dw_mode;
        feResManage->acnt1_dw_mode = new_acnt1_mode;
    }
    fe_deinit_acnt1_resource_from_mode(dir,  old_acnt1_mode);
    fe_init_acnt1_resource_from_mode(dir,  new_acnt1_mode);


    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to get acnt 0 mode 
 Input Args:    
 Ret Value: 
******************************************************************************/
int fe_api_get_acnt0_mode(struct ecnt_fe_data *fe_data){
#if	defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    FE_ACNT_Mode_t acnt0_mode;
    FE_Dir_t dir ;

    if(feResManage == NULL)
        return -1;

    dir = fe_data->api_data.meter_cfg.dir;
    if (dir == UP_STREAM){ 
        acnt0_mode = feResManage->acnt0_up_mode;
    }else{
        acnt0_mode = feResManage->acnt0_dw_mode;
    }

    fe_data->api_data.meter_cfg.acnt_mode = acnt0_mode;
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:     It's used to get acnt 1 mode 
 Input Args:   
 Ret Value: 
******************************************************************************/
int fe_api_get_acnt1_mode(struct ecnt_fe_data *fe_data){
#if	defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    FE_ACNT_Mode_t acnt1_mode;
    FE_Dir_t dir ;

    if(feResManage == NULL)
        return -1;

    dir = fe_data->api_data.meter_cfg.dir;
    if (dir == UP_STREAM){ 
        acnt1_mode = feResManage->acnt1_up_mode;
    }else{
        acnt1_mode = feResManage->acnt1_dw_mode;
    }
    fe_data->api_data.meter_cfg.acnt_mode = acnt1_mode;
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to set rx ratelimit mode owner ,by olt or webpage
 Input Args:    dir and mode
 Ret Value: 0: successful 
******************************************************************************/
int fe_api_set_meter_ctl_by_olt(struct ecnt_fe_data *fe_data){
#if defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    unchar idx = 0;
    idx = fe_data->api_data.meter_cfg.idx;

    if(feResManage == NULL)
        return -1;

    if(idx)
        feResManage->Meter_Ctl_By_Olt = 1;
    else
        feResManage->Meter_Ctl_By_Olt = 0;

    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

#if defined(TCSUPPORT_CPU_EN7527)
int fe_api_get_pkt_cnt(unchar grpIdx, unchar acnt_id, uint *byteHigh, uint *byteLow, uint *pktCnt)
{
    uint byteHigh_t = 0, byteLow_t = 0, pktLow_t = 0, pktHigh_t = 0;
    uint pre_byteL = 0;

    if(feResManage == NULL)
        return -1;

    if(0 == grpIdx)
    {
        if(acnt_id >= GRP0_MAX_NUM)
            return 0;
#if defined(TCSUPPORT_CPU_EN7528)
        PPE_API_GET_ACNT0_CNT(acnt_id, &pktLow_t, &pktHigh_t, &byteLow_t, &byteHigh_t);
#else
        FE_API_GET_AC_GROUP_PKT_CNT(acnt_id, &pktLow_t);
        FE_API_GET_AC_GROUP_BYTE_CNT(acnt_id, &byteLow_t, &byteHigh_t);
#endif
        pre_byteL = feResManage->acnt0_statistic[acnt_id].byteLow;
        feResManage->acnt0_statistic[acnt_id].pktCnt += pktLow_t;
        feResManage->acnt0_statistic[acnt_id].byteLow += byteLow_t;
        feResManage->acnt0_statistic[acnt_id].byteHigh += byteHigh_t;
        if(feResManage->acnt0_statistic[acnt_id].byteLow < pre_byteL)
            feResManage->acnt0_statistic[acnt_id].byteHigh += 1;
        *pktCnt = feResManage->acnt0_statistic[acnt_id].pktCnt;
        *byteLow = feResManage->acnt0_statistic[acnt_id].byteLow;
        *byteHigh = feResManage->acnt0_statistic[acnt_id].byteHigh;
    }
    else if(1 == grpIdx)
    {
        if(acnt_id >= GRP1_MAX_NUM)
            return 0;
        PPE_API_GET_ACNT1_CNT(acnt_id, &pktLow_t, &pktHigh_t, &byteLow_t, &byteHigh_t);
#if defined(TCSUPPORT_CPU_EN7528)
        pre_byteL = feResManage->acnt1_statistic[acnt_id].byteLow;
        feResManage->acnt1_statistic[acnt_id].pktCnt += pktLow_t;
        feResManage->acnt1_statistic[acnt_id].byteLow += byteLow_t;
        feResManage->acnt1_statistic[acnt_id].byteHigh += byteHigh_t;
        if(feResManage->acnt1_statistic[acnt_id].byteLow < pre_byteL)
            feResManage->acnt1_statistic[acnt_id].byteHigh += 1;
#else
        feResManage->acnt1_statistic[acnt_id].pktCnt = pktLow_t;
        feResManage->acnt1_statistic[acnt_id].byteLow = byteLow_t;
        feResManage->acnt1_statistic[acnt_id].byteHigh = byteHigh_t;
#endif
        *pktCnt = feResManage->acnt1_statistic[acnt_id].pktCnt;
        *byteLow = feResManage->acnt1_statistic[acnt_id].byteLow;
        *byteHigh = feResManage->acnt1_statistic[acnt_id].byteHigh;
    }
    else if(2 == grpIdx)
    {
        if(acnt_id >= GRP2_MAX_NUM)
            return 0;
#if defined(TCSUPPORT_CPU_EN7528)
        PPE_API_GET_ACNT2_CNT(acnt_id, &pktLow_t, &pktHigh_t, &byteLow_t, &byteHigh_t);
        pre_byteL = feResManage->acnt2_statistic[acnt_id].byteLow;
        feResManage->acnt2_statistic[acnt_id].pktCnt += pktLow_t;
        feResManage->acnt2_statistic[acnt_id].byteLow += byteLow_t;
        feResManage->acnt2_statistic[acnt_id].byteHigh += byteHigh_t;
        if(feResManage->acnt2_statistic[acnt_id].byteLow < pre_byteL)
            feResManage->acnt2_statistic[acnt_id].byteHigh += 1;
        *pktCnt = feResManage->acnt2_statistic[acnt_id].pktCnt;
        *byteLow = feResManage->acnt2_statistic[acnt_id].byteLow;
        *byteHigh = feResManage->acnt2_statistic[acnt_id].byteHigh;
#endif
    }

    return 0;
}

int fe_api_clear_pkt_cnt(unchar grpIdx, unchar acnt_id)
{
    uint byteHigh_t = 0, byteLow_t = 0, pktLow_t = 0, pktHigh_t = 0;

    if(feResManage == NULL)
        return -1;

    if(0 == grpIdx)
    {
        /* read clear first */
#if defined(TCSUPPORT_CPU_EN7528) 
        PPE_API_GET_ACNT0_CNT(acnt_id, &pktLow_t, &pktHigh_t, &byteLow_t, &byteHigh_t);
#else
        FE_API_GET_AC_GROUP_PKT_CNT(acnt_id, &pktLow_t);
        FE_API_GET_AC_GROUP_BYTE_CNT(acnt_id, &byteLow_t, &byteHigh_t);
#endif
        memset(&feResManage->acnt0_statistic[acnt_id], 0x0, sizeof(Account_T));
    }
    else if(1 == grpIdx)
    {
#if defined(TCSUPPORT_CPU_EN7528) 
        PPE_API_GET_ACNT1_CNT(acnt_id, &pktLow_t, &pktHigh_t, &byteLow_t, &byteHigh_t);
#else
        PPE_API_CLEAR_ACNT1_CNT(acnt_id);
#endif
        memset(&feResManage->acnt1_statistic[acnt_id], 0x0, sizeof(Account_T));
    }
    else if(2 == grpIdx)
    {
#if defined(TCSUPPORT_CPU_EN7528) 
        PPE_API_GET_ACNT2_CNT(acnt_id, &pktLow_t, &pktHigh_t, &byteLow_t, &byteHigh_t);
        memset(&feResManage->acnt2_statistic[acnt_id], 0x0, sizeof(Account_T));
#endif   
    }

    return 0;
}

int findSwMacMatchItem(unsigned char *mac)
{
	struct devBandwidth_s *devBandWidthPtr = NULL;
	int i = 0;
	
	if(gDevBandwidthList == NULL)
		return -1;

	for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++){
		devBandWidthPtr = &(gDevBandwidthList->bandwidthList[i]);
		if(devBandWidthPtr != NULL && isMacMatch(mac,devBandWidthPtr->mac)){
			return i;
		}		
	}

	return -1;	
}

int fe_get_dev_mac_swidx_by_hwidx(int hwidx)
{
	return findSwMacMatchItem(gHwBandwidthList->bandwidthList[hwidx].mac);
}

int fe_get_dev_mac_sw_cnt(int hwidx, int dir, unsigned int *byteCnt_L, unsigned int *byteCnt_H)
{
	int swidx = -1;
	struct dev_Bandwidth_s *dev_BandWidthPtr = NULL;
	
	swidx = fe_get_dev_mac_swidx_by_hwidx(hwidx);
	if(swidx < 0 || swidx >= MAX_DEV_BANDWIDTH_NUM)
		return -1;

	dev_BandWidthPtr = (dev_Bandwidth_t*)(&gDevBandwidthList->bandwidthList[swidx]);

	if(dir == DOWN_STREAM)
	{
		*byteCnt_H = (uint)((dev_BandWidthPtr->downBytes_hi) & 0xFFFFFFFFUL);
		*byteCnt_L = (uint)(dev_BandWidthPtr->downBytes_lo & 0xFFFFFFFFUL);

	}
	else
	{
		*byteCnt_H = (uint)((dev_BandWidthPtr->upBytes_hi) & 0xFFFFFFFFUL);
		*byteCnt_L = (uint)(dev_BandWidthPtr->upBytes_lo & 0xFFFFFFFFUL); 
	}

	return 0;
}
#endif

int updateMulticastCntToDevCnt(unchar idx, struct ecnt_fe_data *fe_data)
{
	struct devBandwidth_s *devBandWidthPtr = NULL;
	int i = 0;
	unsigned char mac[6];
	int gswPortId = 0;
	unsigned int gswMcPktCnt = 0;
	unsigned int gswPktCnt = 0;
	unsigned int gswAllByteLoCnt = 0;
	unsigned int gswAllByteHiCnt = 0;
	unsigned long long byteCnt = 0;
	unsigned long long uniAllByteCnt = 0;
	unsigned long long uniAllPktCnt = 0;
	unsigned long long gswAllByteCnt = 0;
	QDMA_FlowCntValueGet_T flowCntValueGet;
	unsigned int byteHigh = 0, byteLow = 0, pktCnt = 0;

	if(update_mul_cnt_en == 0){
		return 0;
	}

	memset(mac, 0, 6 * sizeof(unsigned char));
	memset(&flowCntValueGet, 0, sizeof(QDMA_FlowCntValueGet_T));

    if(feResManage == NULL)
        return -1;

	if( findItemMatchMac(idx, mac) > 0 ){
		gswPortId = findGswIdxByMac(mac);
		if( gswPortId < 0 ){
			return 0;
		}

		/* 1. add multicast packet cnt to packet cnt */
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_MCAST_CNT, gswPortId, &gswMcPktCnt);
		fe_data->api_data.meter_cfg.pktCnt += gswMcPktCnt;

		/* 2. add multicast byte cnt to packet cnt */

		/* read this port all tx unicast byte cnt */
		for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++){
			devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i]);
			if( (devBandWidthPtr != NULL) &&  (gswPortId == findGswIdxByMac(devBandWidthPtr->mac)) ){
				flowCntValueGet.grpIdx = 0;
				flowCntValueGet.cntIdx = feResManage->ACNT0_DEV_FLOW_DOWN[i];
#if defined(TCSUPPORT_CPU_EN7580)
				QDMA_API_GET_FLOW_CNTR_VALUE(ECNT_QDMA_LAN, &flowCntValueGet);
				uniAllByteCnt += (((unsigned long long)flowCntValueGet.byteHigh) << 32) + (unsigned long long)flowCntValueGet.byteLow;
				uniAllPktCnt += ((unsigned long long)flowCntValueGet.pktCnt);
#elif defined(TCSUPPORT_CPU_EN7527)
    			fe_api_get_pkt_cnt(0, feResManage->ACNT0_DEV_FLOW_DOWN[i], &byteHigh, &byteLow, &pktCnt);
				uniAllByteCnt += (((unsigned long long)byteHigh) << 32) + (unsigned long long)byteLow;
#endif				
			}		
		}

#if defined(TCSUPPORT_CPU_EN7580)
		uniAllByteCnt -= uniAllPktCnt * 28;
#endif

		/* read this port all tx byte cnt */
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_OCT_CNT_L, gswPortId, &gswAllByteLoCnt);
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_OCT_CNT_H, gswPortId, &gswAllByteHiCnt);
		gswAllByteCnt = (((unsigned long long)gswAllByteHiCnt) << 32) + (unsigned long long)gswAllByteLoCnt;
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_UCAST_CNT, gswPortId, &gswPktCnt); 
		gswAllByteCnt += 20 * gswPktCnt;
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_MCAST_CNT, gswPortId, &gswPktCnt); 
		gswAllByteCnt += 20 * gswPktCnt;
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_BCAST_CNT, gswPortId, &gswPktCnt); 
		gswAllByteCnt += 20 * gswPktCnt;

		/* this port multicast cnt = all tx byte cnt -  all tx unicast byte cnt */
		byteCnt = (((unsigned long long)fe_data->api_data.meter_cfg.byteCnt_H) << 32) + (unsigned long long)fe_data->api_data.meter_cfg.byteCnt_L;
		if( gswAllByteCnt > uniAllByteCnt){
			byteCnt += gswAllByteCnt - uniAllByteCnt;
		}else{
			byteCnt += gswAllByteCnt + ((~(unsigned long long)0) - uniAllByteCnt);
		}

		fe_data->api_data.meter_cfg.byteCnt_H = (byteCnt >> 32); 
		fe_data->api_data.meter_cfg.byteCnt_L = byteCnt & 0xFFFFFFFF;
	}

	return 0;
}

/******************************************************************************
 Descriptor:    It's used to get byte cnt of flow id 
 Input Args:    
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_get_flow_cnt(struct ecnt_fe_data *fe_data){
#if defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    FE_Dir_t dir ;
    int i = 0;
    unchar idx = 0;
    unchar acnt_id = 0;
    unchar grpIdx = 0;
    uint pre_byteL = 0;
	uint pre_pktL = 0;
    int mainType_val = 0;
    QDMA_FlowCntValueGet_T flowCntValueGet;
    FE_ACNT_Mode_t acnt_mode;
    FE_ACNT_Mode_t acnt1_mode;
    FE_ACNT_Mode_t acnt2_mode;
    memset(&flowCntValueGet, 0, sizeof(QDMA_FlowCntValueGet_T));
	unsigned int regVal;
	unsigned int rate_byte;
	
	regVal = read_reg_word(FE_DMA_GLO_CFG);
	rate_byte =(regVal&RATE_BYTE_MASK)>>RATE_BYTE_OFFSET;

    dir = fe_data->api_data.meter_cfg.dir;
    idx = fe_data->api_data.meter_cfg.idx;
    acnt_mode = fe_data->api_data.meter_cfg.acnt_mode;

    if(feResManage == NULL)
        return -1;
    
    if (dir == UP_STREAM){
        acnt1_mode = feResManage->acnt1_up_mode;
        acnt2_mode = feResManage->acnt2_up_mode;
    }else{
        acnt1_mode = feResManage->acnt1_dw_mode;
        acnt2_mode = feResManage->acnt2_dw_mode;
    }

    switch(acnt_mode)
    {
        case TRAFFIC_FLOW_MODE:
            if(acnt_mode == acnt1_mode)
                grpIdx = 1;
            else if(acnt_mode == acnt2_mode)
                grpIdx = 2;
            else
                return 0;
            if(grpIdx == 1) {
                if(dir == DOWN_STREAM){
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            } else {
                if(dir == DOWN_STREAM){
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT2_TRAFFIC_FLOW_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT2_TRAFFIC_FLOW_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            }
            break;
        case DEV_MODE:
            grpIdx = 0;
            if(dir == DOWN_STREAM){
                if(idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20))
                    acnt_id = feResManage->ACNT0_DEV_FLOW_DOWN[idx];
                else
                    return 0;
                mainType_val = ECNT_QDMA_LAN;
            }else{
                if(idx < (wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20))
                    acnt_id = feResManage->ACNT0_DEV_FLOW_UP[idx];
                else
                    return 0;
                if(wan_conn_type == 4)
                mainType_val = ECNT_QDMA_WAN;
		        else
		        	mainType_val = ECNT_QDMA_LAN;
            }
            break;
        case WAN_ITF_MODE:
            if(acnt_mode == acnt1_mode)
                grpIdx = 1;
            else if(acnt_mode == acnt2_mode)
                grpIdx = 2;
            else
                return 0;
            if(grpIdx == 1) {
                if(dir == DOWN_STREAM){
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            } else {
                if(dir == DOWN_STREAM){
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT2_WAN_ITF_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT2_WAN_ITF_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            }
            break;
        case WAN_ITF_AND_TRAFFIC_FLOW_MODE:
            if(acnt_mode == acnt1_mode)
                grpIdx = 1;
            else
                return 0;
            if(grpIdx == 1) {
                if(dir == DOWN_STREAM){
                    if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_DOWN[idx];
                    else if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM + TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx-WAN_ITF_HAS_FLOW_ACNT_NUM];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_UP[idx];
                    else if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM + TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx-WAN_ITF_HAS_FLOW_ACNT_NUM];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            } 
            break;
        case MULTICAST_MODE:
            if(TCSUPPORT_CF_UNIFIED_PLATFORM_VAL)
            {
                grpIdx = 2;
                acnt_id = idx;
            }
            else
            {
            	grpIdx = 0;
            	acnt_id = MULTICAST_ACNTGRP;
            }
            if(dir == DOWN_STREAM)
                mainType_val = ECNT_QDMA_LAN;
            else
                mainType_val = ECNT_QDMA_WAN;
            break;
        default:
            grpIdx = 0;
            acnt_id = ACNT0_DEFAULT;
            break;
    }

#if defined(TCSUPPORT_CPU_EN7580)
    flowCntValueGet.grpIdx = grpIdx;
    flowCntValueGet.cntIdx = acnt_id;
    QDMA_API_GET_FLOW_CNTR_VALUE(mainType_val, &flowCntValueGet) ;
    fe_data->api_data.meter_cfg.byteCnt_H = flowCntValueGet.byteHigh ;
    fe_data->api_data.meter_cfg.byteCnt_L = flowCntValueGet.byteLow ;
    fe_data->api_data.meter_cfg.pktCnt = flowCntValueGet.pktCnt ; 
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	fe_data->api_data.meter_cfg.pktCnt_H = flowCntValueGet.pktHigh ;
#endif
    if((acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE) && (idx == default_wan_itf)) {
        for(i=0; i<TRAFFIC_FLOW_MAX_NUM; i++) {
            flowCntValueGet.grpIdx = grpIdx;
            if(dir == DOWN_STREAM){
                flowCntValueGet.cntIdx = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[i];
            } else {
                flowCntValueGet.cntIdx = feResManage->ACNT1_TRAFFIC_FLOW_UP[i];
            }
            QDMA_API_GET_FLOW_CNTR_VALUE(mainType_val, &flowCntValueGet) ;
            fe_data->api_data.meter_cfg.byteCnt_H += flowCntValueGet.byteHigh ;
            pre_byteL = fe_data->api_data.meter_cfg.byteCnt_L;
            fe_data->api_data.meter_cfg.byteCnt_L += flowCntValueGet.byteLow ;
            if(fe_data->api_data.meter_cfg.byteCnt_L < pre_byteL)
                fe_data->api_data.meter_cfg.byteCnt_H += 1;

			fe_data->api_data.meter_cfg.pktCnt_H += flowCntValueGet.pktHigh;
            pre_pktL = fe_data->api_data.meter_cfg.pktCnt;
            fe_data->api_data.meter_cfg.pktCnt += flowCntValueGet.pktCnt ;
            if(fe_data->api_data.meter_cfg.pktCnt < pre_pktL)
                fe_data->api_data.meter_cfg.pktCnt_H += 1;
             
        }
    }
#if defined(TCSUPPORT_CT_JOYME2)
    else if ((acnt_mode == DEV_MODE) && (dir == DOWN_STREAM)) {
        updateMulticastCntToDevCnt(idx, fe_data);
    }
#endif
#elif defined(TCSUPPORT_CPU_EN7527)
    uint byteHigh = 0, byteLow = 0, pktCnt = 0;
    fe_api_get_pkt_cnt(grpIdx, acnt_id, &byteHigh, &byteLow, &pktCnt);
    fe_data->api_data.meter_cfg.byteCnt_H = byteHigh ;
    fe_data->api_data.meter_cfg.byteCnt_L = byteLow ;
    fe_data->api_data.meter_cfg.pktCnt = pktCnt ; 

	/* get CPU path statistics */
	if(WAN_ITF_MODE == acnt_mode)
	{
		byteHigh = byteLow = pktCnt = 0;
		fe_get_wan_itf_cnt(idx, (dir==0)?DOWNSTREAM_ONLY:UPSTREAM_ONLY, &byteLow, &byteHigh, &pktCnt);
		fe_data->api_data.meter_cfg.byteCnt_H += byteHigh;
		pre_byteL = fe_data->api_data.meter_cfg.byteCnt_L;
		fe_data->api_data.meter_cfg.byteCnt_L += byteLow;
		if(fe_data->api_data.meter_cfg.byteCnt_L < pre_byteL)
			fe_data->api_data.meter_cfg.byteCnt_H += 1;
		fe_data->api_data.meter_cfg.pktCnt += pktCnt;
	}
	else if(TRAFFIC_FLOW_MODE == acnt_mode)
	{
		uint softCnt = 0;
		uint flowid = (dir == DOWN_STREAM)?(idx+TRAFFIC_FLOW_MAX_NUM):idx;
		PPE_API_GET_SOFT_CNT(flowid, &softCnt);
		pre_byteL = fe_data->api_data.meter_cfg.byteCnt_L;
		fe_data->api_data.meter_cfg.byteCnt_L += softCnt ;
		#if defined(TCSUPPORT_CPU_EN7528) 
		/*7528 cal l2 packets, but ratelimit is l1 packets */
		fe_data->api_data.meter_cfg.byteCnt_L += pktCnt*rate_byte;
		#endif
		if(fe_data->api_data.meter_cfg.byteCnt_L < pre_byteL)
			fe_data->api_data.meter_cfg.byteCnt_H += 1;
	}
	else if(DEV_MODE == acnt_mode)
	{
#if defined(TCSUPPORT_CT_JOYME2)
		if ( dir == DOWN_STREAM ) {
			updateMulticastCntToDevCnt(idx, fe_data);
		}
#endif
		byteHigh = byteLow = 0;
		fe_get_dev_mac_sw_cnt(idx, dir, &byteLow, &byteHigh);
		fe_data->api_data.meter_cfg.byteCnt_H += byteHigh;
		pre_byteL = fe_data->api_data.meter_cfg.byteCnt_L;
		fe_data->api_data.meter_cfg.byteCnt_L += byteLow;
		if(fe_data->api_data.meter_cfg.byteCnt_L < pre_byteL)
			fe_data->api_data.meter_cfg.byteCnt_H += 1;
	}
#endif

    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}
    
/******************************************************************************
 Descriptor:    It's used to clear account cnt
 Input Args:    
 Ret Value: 0: successful, otherwise failed.
******************************************************************************/
int fe_api_clear_flow_cnt(struct ecnt_fe_data *fe_data){
#if defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    FE_Dir_t dir ;
    int i = 0;
    unchar idx = 0;
    unchar acnt_id = 0;
    unchar grpIdx = 0;
    int mainType_val = 0;
    QDMA_FlowCntValueClear_T flowCntValueClear;
    FE_ACNT_Mode_t acnt_mode;
    FE_ACNT_Mode_t acnt1_mode;
    FE_ACNT_Mode_t acnt2_mode;
    memset(&flowCntValueClear, 0, sizeof(QDMA_FlowCntValueClear_T));

    dir = fe_data->api_data.meter_cfg.dir;
    idx = fe_data->api_data.meter_cfg.idx;
    acnt_mode = fe_data->api_data.meter_cfg.acnt_mode;

    if(feResManage == NULL)
        return -1;

    if (dir == UP_STREAM){
        acnt1_mode = feResManage->acnt1_up_mode;
        acnt2_mode = feResManage->acnt2_up_mode;
    }else{
        acnt1_mode = feResManage->acnt1_dw_mode;
        acnt2_mode = feResManage->acnt2_dw_mode;
    }

    switch(acnt_mode)
    {
        case TRAFFIC_FLOW_MODE:
            if(acnt_mode == acnt1_mode)
                grpIdx = 1;
            else if(acnt_mode == acnt2_mode)
                grpIdx = 2;
            else
                return 0;
            if(grpIdx == 1) {
                if(dir == DOWN_STREAM){
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            } else {
                if(dir == DOWN_STREAM){
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT2_TRAFFIC_FLOW_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT2_TRAFFIC_FLOW_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            }
            break;
        case DEV_MODE:
            grpIdx = 0;
            if(dir == DOWN_STREAM){
                if(idx < (old_wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20))
                    acnt_id = feResManage->ACNT0_DEV_FLOW_DOWN[idx];
                else
                    return 0;
                mainType_val = ECNT_QDMA_LAN;
            }else{
                if(idx < (old_wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20))
                    acnt_id = feResManage->ACNT0_DEV_FLOW_UP[idx];
                else
                    return 0;

                if(old_wan_conn_type == 4)
                mainType_val = ECNT_QDMA_WAN;
                else
                	mainType_val = ECNT_QDMA_LAN;
            }
            break;
        case WAN_ITF_MODE:
            if(acnt_mode == acnt1_mode)
                grpIdx = 1;
            else if(acnt_mode == acnt2_mode)
                grpIdx = 2;
            else
                return 0;
            if(grpIdx == 1) {
                if(dir == DOWN_STREAM){
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            } else {
                if(dir == DOWN_STREAM){
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT2_WAN_ITF_DOWN[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < WAN_ITF_ACNT_NUM)
                        acnt_id = feResManage->ACNT2_WAN_ITF_UP[idx];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            }
            break;
        case WAN_ITF_AND_TRAFFIC_FLOW_MODE:
            if(acnt_mode == acnt1_mode)
                grpIdx = 1;
            else
                return 0;
            if(grpIdx == 1) {
                if(dir == DOWN_STREAM){
                    if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_DOWN[idx];
                    else if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM + TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[idx-WAN_ITF_HAS_FLOW_ACNT_NUM];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_LAN;
                }else{
                    if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM)
                        acnt_id = feResManage->ACNT1_WAN_ITF_UP[idx];
                    else if(idx < WAN_ITF_HAS_FLOW_ACNT_NUM + TRAFFIC_FLOW_MAX_NUM)
                        acnt_id = feResManage->ACNT1_TRAFFIC_FLOW_UP[idx-WAN_ITF_HAS_FLOW_ACNT_NUM];
                    else
                        return 0;
                    mainType_val = ECNT_QDMA_WAN;
                }
            } 
            break;
        case MULTICAST_MODE:
            if(TCSUPPORT_CF_UNIFIED_PLATFORM_VAL)
            {
                grpIdx = 2;
                acnt_id = idx;
            }
            else
            {
            	grpIdx = 0;
            	acnt_id = MULTICAST_ACNTGRP;
            }
            if(dir == DOWN_STREAM)
                mainType_val = ECNT_QDMA_LAN;
            else
                mainType_val = ECNT_QDMA_WAN;
            break;
        default:
            return 0;
    }
#if defined(TCSUPPORT_CPU_EN7580)  
    flowCntValueClear.grpIdx = grpIdx;
    flowCntValueClear.cntIdx = acnt_id;
    flowCntValueClear.clearMode = FLOWCNT_CLEAR_ALL;
    printk("dir is %d, group is %d, acnd is %d\n", dir, flowCntValueClear.grpIdx, acnt_id);
    QDMA_API_CLEAR_FLOW_CNTR_VALUE(mainType_val, &flowCntValueClear);
    if((acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE) && (idx == default_wan_itf)) {
        for(i=0; i<TRAFFIC_FLOW_MAX_NUM; i++) {
            flowCntValueClear.grpIdx = grpIdx;
            if(dir == DOWN_STREAM){
                flowCntValueClear.cntIdx = feResManage->ACNT1_TRAFFIC_FLOW_DOWN[i];
            } else {
                flowCntValueClear.cntIdx = feResManage->ACNT1_TRAFFIC_FLOW_UP[i];
            }
            printk("dir is %d, group is %d, acnd is %d\n", dir, flowCntValueClear.grpIdx, flowCntValueClear.cntIdx);
            QDMA_API_CLEAR_FLOW_CNTR_VALUE(mainType_val, &flowCntValueClear) ; 
        }
    }
#else if defined(TCSUPPORT_CPU_EN7527)
    fe_api_clear_pkt_cnt(grpIdx, acnt_id);
    if(WAN_ITF_MODE == acnt_mode)
    {
        fe_clear_wan_itf_cnt(idx, (dir==0)?DOWNSTREAM_ONLY:UPSTREAM_ONLY);
    }
#endif
    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif

}
    
/******************************************************************************
 Descriptor:    It's used get mac idx if it is under mac dev.
 Input Args:    
 Ret Value: 0: not find  1~30 really mac index in gHwBandwidthList
******************************************************************************/
int fe_api_get_dev_mac_index(struct ecnt_fe_data *fe_data){
#if defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
    unsigned char eth_mac[6];
    int mac_idx = -1;
    memcpy(eth_mac, fe_data->api_data.meter_cfg.mac, sizeof(eth_mac));
    mac_idx = findMacMatchItem(eth_mac);
    
    if(mac_idx < 0){
        mac_idx = 0;
    }else
        mac_idx = mac_idx+1;

    
    fe_data->api_data.meter_cfg.idx =(unsigned char)mac_idx;

    return 0;
#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to set pse oq threshold. 
 Input Args:     
 Ret Value:  
******************************************************************************/
int fe_api_set_pse_oq_threshold(struct ecnt_fe_data *fe_data) 
{
	unsigned int queue_idx = 0;
	
#if defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
	FE_PonMode_Sel_t pon_mode;

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

#else
	printk("%s %d Not Support this IC yet\n",__func__,__LINE__);
	return -1;
#endif
}

/******************************************************************************
 Descriptor:    It's used to set MC BC pkt through assignation ring Idx. 
 Input Args:     
 Ret Value:  
******************************************************************************/
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
#if defined(TCSUPPORT_CPU_EN7580)
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
#else
	return -1;
#endif
}

/******************************************************************************
 Descriptor : It's used to set device statistics and ratelimit mode 
 Input Args :     
 Ret Value: 0 : successful 
******************************************************************************/
int fe_api_set_dev_stat_ratelimit_mode(struct ecnt_fe_data *fe_data)
{
#if	defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523)
	int idx = 0;

    wan_conn_type = fe_data->api_data.dev_statistics_cfg.wan_conn_type;
	memcpy(wan_itf_name, fe_data->api_data.dev_statistics_cfg.wan_itf_name, 16);

    if(feResManage == NULL)
        return -1;

	/* clear flow count */
	for(; idx < (old_wan_conn_type == 4 ? UNDER_DEV_MAX_NUM : UNDER_DEV_MAX_NUM_20); ++idx)
	{
		FE_API_CLEAR_FlOW_CNT(idx, DOWN_STREAM, DEV_MODE);
		FE_API_CLEAR_FlOW_CNT(idx, UP_STREAM, DEV_MODE);
	}

	/* re-allocate acnt0 group id */
    fe_deinit_acnt0_resource_from_mode(DOWN_STREAM, DEV_MODE);
    fe_deinit_acnt0_resource_from_mode(UP_STREAM, DEV_MODE);
    fe_init_acnt0_resource_from_mode(DOWN_STREAM, DEV_MODE);
    fe_init_acnt0_resource_from_mode(UP_STREAM, DEV_MODE);
    
	/* reallocate meter id */
	if(old_wan_conn_type != wan_conn_type && (old_wan_conn_type == 4 || wan_conn_type == 4))
	{		
		fe_remove_meter(UP_STREAM, feResManage->dw_stream_mode);
		fe_remove_meter(DOWN_STREAM, feResManage->dw_stream_mode);
		
		feResManage->up_stream_mode = DEV_MAC_MODE;
		feResManage->dw_stream_mode = DEV_MAC_MODE;
		
		PPE_API_CLEAN_FOE_TABLE();			
		
    	fe_add_meter(UP_STREAM, DEV_MAC_MODE);
    	fe_add_meter(DOWN_STREAM, DEV_MAC_MODE);
    }

    old_wan_conn_type = wan_conn_type;
    
    return 0;
#else
	printk("%s %d Not Support this IC yet!\n", __func__, __LINE__);
	return -1;
#endif
}


