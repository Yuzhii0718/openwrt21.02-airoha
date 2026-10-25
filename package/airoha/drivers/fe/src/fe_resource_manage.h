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
#ifndef _FE_RESOURCE_MANAGE_H_
#define _FE_RESOURCE_MANAGE_H_
        
        
/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include "fe.h"
    
/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
            
#define unchar 			unsigned char
        
#define TRAFFIC_QOS_NUM     	(8) /*Traffic flow qos used for test*/
#define QOS_CAR_NUM         	(6) /*queue num used in QoS car mode*/
    
#define	WIFI_24G_OFFSET		    (12)/*single wifi */
#define	WIFI_5G_OFFSET			(4)
    
#define IF_MAX_NUM          	(20)/*max interface num ,eth0.1-0.4,rai0-rai7,ra0-ra7*/
    
#define IP_MAX_NUM          	(8) /*max IP num if need ip_ratelimit extend*/
#define NAME_LENGTH         	(16) /*meter binded name */
#define TRAFFIC_FLOW_MAX_NUM    (8)/*traffic flow account num and bind in acnt 1*/
#define UNDER_DEV_MAX_NUM		(30)/*acnt0 */

#define FORCEBW_WRR_NUM         (8)

#define fe_ratelimit_up_info_path   "tc3162/up_stream_traffic_ratelimit"
#define fe_ratelimit_dw_info_path   "tc3162/down_stream_traffic_ratelimit"
#define fe_acnt2_mapping_path       "tc3162/fe_acnt2_mapping"
#define fe_acnt1_mapping_path       "tc3162/fe_acnt1_mapping"
#define fe_acnt0_mapping_path       "tc3162/fe_acnt0_mapping"
            
#define MAX_DEV_BANDWIDTH_NUM 30

#if defined(TCSUPPORT_XPON_HAL_API_EXT) || defined(TCSUPPORT_XPON_HAL_API)
#define WAN_ITF_NAME_PRE         "pon0."
#define WAN_ITF_NAME_PRE_LEN     (5)
#else
#define WAN_ITF_NAME_PRE         "nas"
#define WAN_ITF_NAME_PRE_LEN     (3)
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
#define LAYER2_HEADER(skb)		skb_mac_header(skb)
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21)
#define LAYER2_HEADER(skb)		(skb)->mac_header
#else
#define LAYER2_HEADER(skb)		(skb)->mac.raw
#endif
#endif

#define GENERAL_INGRESS_NO_RATELIMIT_INDEX    (127)   /*EN7580: 128 ratelimit.*/
#define GENERAL_INGRESS_NO_TRTCM_INDEX    (63)   /*EN7580:  64 trtcm*/

/* acnt0:
	When changed the mode of upstream to ether_wan or mesh, 
	the upstream packets go through QDMA_LAN, 
	the number of meter in QDMA_LAN is not enough, only 47 left, 
	so upstream and downstream allocate 20 meters each
*/
#define UNDER_DEV_MAX_NUM_20	(20)
    
/************************************************************************
*               M A C R O S
*************************************************************************
*/

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
typedef struct {
    uint    byteLow;
    uint    byteHigh;
    uint    pktCnt;
}Account_T;

typedef struct {
	unchar	  CAR_QOS_QUEUE_UP[QOS_CAR_NUM];/* store real meter id  used for up stream*/
	char	  CAR_QOS_QUEUE_NAME_UP[QOS_CAR_NUM][NAME_LENGTH];
    
	unchar	  CAR_QOS_QUEUE_DOWN[QOS_CAR_NUM];/* store real meter id  used for down stream*/
	char	  CAR_QOS_QUEUE_NAME_DOWN[QOS_CAR_NUM][NAME_LENGTH];

	unchar	  TRAFFIC_QOS_UP[TRAFFIC_QOS_NUM];/* store real meter id  used for up stream, used for traffic flow*/
    char      TRAFFIC_QOS_Name_UP[TRAFFIC_QOS_NUM][NAME_LENGTH];
    
    unchar    TRAFFIC_QOS_DW[TRAFFIC_QOS_NUM];/* store real meter id  used for down stream, used for traffic flow*/
    char      TRAFFIC_QOS_Name_DW[TRAFFIC_QOS_NUM][NAME_LENGTH];

    unchar    Meter_OLT_DOWN[OLT_CTL_NUM];/* store real meter id  , olt ctl for down stream */
    char      OLTNameDOWN[OLT_CTL_NUM][NAME_LENGTH];

    unchar    Meter_OLT_UP[OLT_CTL_NUM];/* store real meter id  ,olt ctl for up stream */
    char      OLTNameUP[OLT_CTL_NUM][NAME_LENGTH];  

    unchar    Meter_IF_DOWN[IF_MAX_NUM];/* store real meter id  , interface based down stream */
    char      IFNameDOWN[IF_MAX_NUM][NAME_LENGTH];

    unchar    Meter_IF_UP[IF_MAX_NUM];/* store real meter id ,interface based up stream*/
    char      IFNameUP[IF_MAX_NUM][NAME_LENGTH];

    unchar    Meter_IP_DOWN[IP_MAX_NUM];/* store real meter id, ip based down stream*/
    char      IPNameDOWN[IP_MAX_NUM][NAME_LENGTH];

	unchar	  Meter_IP_UP[IP_MAX_NUM];/* store real meter id, , ip based up stream */
    char      IPNameUP[IP_MAX_NUM][NAME_LENGTH];

    unchar    FORCEBW_WRR_QUEUE_UP[FORCEBW_WRR_NUM];
    char	  FORCEBW_WRR_QUEUE_NAME_UP[FORCEBW_WRR_NUM][NAME_LENGTH];
    
    unchar    Meter_DEV_FLOW_UP[UNDER_DEV_MAX_NUM];/* store real meter id, ip based down stream*/

    unchar    Meter_DEV_FLOW_DW[UNDER_DEV_MAX_NUM];/* store real meter id, , ip based down stream */

    unchar    Meter_WAN_FLOW_UP[WAN_ITF_ACNT_NUM];/* store real meter id, ip based down stream*/

    unchar    Meter_WAN_FLOW_DW[WAN_ITF_ACNT_NUM];/* store real meter id, , ip based down stream */

    FE_Meter_Mode_t    up_stream_mode;/*up stream Rx ratelimit mode*/
    FE_Meter_Mode_t    dw_stream_mode;/*down stream Rx ratelimit mode*/
    FE_ACNT_Mode_t     acnt2_up_mode;
    FE_ACNT_Mode_t     acnt2_dw_mode;
    FE_ACNT_Mode_t     acnt1_up_mode;
    FE_ACNT_Mode_t     acnt1_dw_mode;
    FE_ACNT_Mode_t     acnt0_up_mode;
    FE_ACNT_Mode_t     acnt0_dw_mode;
	GENERAL_TrtcmMode_T	  ingress_current_trtcm_mode[2];/* 0 for down stream, 1 for up stream */
	FE_Dir_t	  ingress_ratelimit_trtcm_invalid_index[2];/* 0 for down stream, 1 for up stream */

#if defined(TCSUPPORT_CPU_EN7527)
    Account_T          acnt0_statistic[GRP0_MAX_NUM];
    Account_T          acnt1_statistic[GRP1_MAX_NUM];
    Account_T          acnt2_statistic[GRP2_MAX_NUM];
#endif

    unchar    ACNT2_TRAFFIC_FLOW_UP[TRAFFIC_FLOW_MAX_NUM];
    char      ACNT2_TRAFFIC_FLOW_UP_NAME[TRAFFIC_FLOW_MAX_NUM][NAME_LENGTH];

    unchar    ACNT2_TRAFFIC_FLOW_DOWN[TRAFFIC_FLOW_MAX_NUM];
    char      ACNT2_TRAFFIC_FLOW_DOWN_NAME[TRAFFIC_FLOW_MAX_NUM][NAME_LENGTH];

    unchar    ACNT2_WAN_ITF_UP[WAN_ITF_ACNT_NUM];
    unchar    ACNT2_WAN_ITF_DOWN[WAN_ITF_ACNT_NUM];

    unchar    ACNT1_TRAFFIC_FLOW_UP[TRAFFIC_FLOW_MAX_NUM];
    char      ACNT1_TRAFFIC_FLOW_UP_NAME[TRAFFIC_FLOW_MAX_NUM][NAME_LENGTH];

    unchar    ACNT1_TRAFFIC_FLOW_DOWN[TRAFFIC_FLOW_MAX_NUM];
    char      ACNT1_TRAFFIC_FLOW_DOWN_NAME[TRAFFIC_FLOW_MAX_NUM][NAME_LENGTH];

    unchar    ACNT1_WAN_ITF_UP[WAN_ITF_ACNT_NUM];
    unchar    ACNT1_WAN_ITF_DOWN[WAN_ITF_ACNT_NUM];

    unchar    ACNT0_DEV_FLOW_UP[UNDER_DEV_MAX_NUM];
    unchar    ACNT0_DEV_FLOW_DOWN[UNDER_DEV_MAX_NUM];

    unchar    Meter_Ctl_By_Olt; /*0: meter ctl by webpage, 1 :meter ctl by olt*/
}Fe_Resource_Manage_T;

typedef struct {
    unchar mac[6];
    unchar acnt_idx;
    unchar idx;
    uint    RX_byteCnt_L;
    uint    RX_byteCnt_H;
    uint    RX_pktCnt;
    uint    TX_byteCnt_L;
    uint    TX_byteCnt_H;
    uint    TX_pktCnt;

}Dev_Mac_T;

typedef struct devBandwidth_s{
    unsigned char mac[6];
    unsigned long long upBytes;
    unsigned long long downBytes;
	unsigned int upRate;
	unsigned int downRate;
    int valid;
}devBandwidth_t;

typedef struct dev_Bandwidth_s{
	unsigned char mac[6];
	unsigned long upBytes_lo;
	unsigned long upBytes_hi;
	unsigned long upPackets;
	unsigned long downBytes_lo;
	unsigned long downBytes_hi;
	unsigned long downPackets;
	int valid;
}dev_Bandwidth_t;

typedef struct devBandwidthList_s{  
    int enable;
    int portMultiMacEn;
    struct devBandwidth_s bandwidthList[MAX_DEV_BANDWIDTH_NUM];
}devBandwidthList_t;

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

extern Fe_Resource_Manage_T *feResManage;

extern int (*fe_resource_mark_meter_hook)( struct sk_buff *skb, int dir );
extern int (*fe_resource_mark_acnt_hook)( struct sk_buff *skb, int dir );
extern int (*fe_resource_mark_wan_idx_hook)( struct sk_buff *skb, u8 wan_index, u8 dir);


/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/


    
#endif


