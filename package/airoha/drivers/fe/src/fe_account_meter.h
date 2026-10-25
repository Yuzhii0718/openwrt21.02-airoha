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
#include "fe_ic_dis.h"
    
#if SUPPORT_AEWAN_LAN_MODE
#include "ecnt_event_global/ecnt_event_system.h"
#endif
/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
            
#define unchar 			unsigned char
        
#define TRAFFIC_QOS_NUM     	(8) /*Traffic flow qos used for test*/
#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
#define QOS_CAR_NUM         	(8) /*queue num used in QoS car mode*/
#else
#define QOS_CAR_NUM         	(6) /*queue num used in QoS car mode*/
#endif

#define	WIFI_24G_OFFSET		    (12)/*single wifi */
#define	WIFI_5G_OFFSET			(4)
    
#define PON_LAN_VALID 		((get_serdes_ether_type_sel(ECNT_EVENT_SERDES_SEL_PON) == ECNT_EVENT_SERDES_ETHER_TYPE_LAN) ? 1 : 0)
#if SUPPORT_AEWAN_LAN_MODE 
#define IF_MAX_NUM          	(20)/*max interface num ,eth0.1-0.4,rai0-rai7,ra0-ra7*/
#define IP_MAX_NUM          	(8) /*max IP num if need ip_ratelimit extend*/
#define NAME_LENGTH         	(16) /*meter binded name */
#define TRAFFIC_FLOW_MAX_NUM    (8)
#define LLID_MAX_NUM            (8)
#define UNDER_DEV_MAX_NUM		(30)
#else
#define IF_MAX_NUM          	(20)/*max interface num ,eth0.1-0.4,rai0-rai7,ra0-ra7*/
#define IP_MAX_NUM          	(8) /*max IP num if need ip_ratelimit extend*/
#define NAME_LENGTH         	(16) /*meter binded name */
//#define TRAFFIC_FLOW_MAX_NUM    (8)/*traffic flow account num and bind in acnt 1*/
#define TRAFFIC_FLOW_MAX_NUM    (64)
#define LLID_MAX_NUM            (8)
#define UNDER_DEV_MAX_NUM		(30)/*acnt0 */
#endif
#define FORCEBW_WRR_NUM         (8)

#define fe_ratelimit_up_info_path   "tc3162/up_stream_traffic_ratelimit"
#define fe_ratelimit_dw_info_path   "tc3162/down_stream_traffic_ratelimit" 
#define fe_acnt_mapping_path       "tc3162/fe_acnt_mapping"

            
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

    
/************************************************************************
*               M A C R O S
*************************************************************************
*/

/*****************fe_resource_mark bitmap format******************************/
/*reserve			acnt1&meter2		acnt0&meter1	acnt2&meter0*/
/*00000000			0000000 			00000000		000000000*/
/*****************fe_resource_mark format*************************************/
#define ACCOUNT2_OFFSET    	(0)
#define ACCOUNT2_MASK       	(0x7F)
#define METER0_MASK       	(0x7F)
#define METER0_ENABLE_OFFSET	(7)
#define ACCOUNT2_ENABLE_OFFSET	(8)
	
#define ACCOUNT0_MASK_LEN  	(6)
#define ACCOUNT0_OFFSET    	(9)
#define ACCOUNT0_MASK      	(0x3F)
#define METER1_MASK       	(0x1F)
#define METER1_ENABLE_OFFSET	(15)
#define ACCOUNT0_ENABLE_OFFSET	(16)
	
#define ACCOUNT1_OFFSET    	(17)
#define ACCOUNT1_MASK      	(0x1F)
#define METER2_MASK       	(0xF)
#define METER2_ENABLE_OFFSET	(22)
#define ACCOUNT1_ENABLE_OFFSET	(23)
#define ETHER_WAN_OFFSET 	(24)
#define ETHER_WAN_MASK  	(1<<ETHER_WAN_OFFSET)
#define DFT_ITF_OFFSET   	(25)
#define DFT_ITF_MASK     	(0x1<<DFT_ITF_OFFSET)
#define DFT_ITF_MASK_LEN 	(1)



#define FE_DEBUG(level, F, args...) { \
										if(feResManage->dbgLevel >= level)  \
											printk("\n%s_%d: " F, __func__, __LINE__, ##args);  \
									}



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

    FE_Meter_Mode_t    up_stream_mode[3];/*up stream Rx ratelimit mode*/
    FE_Meter_Mode_t    dw_stream_mode[3];/*down stream Rx ratelimit mode*/
    FE_ACNT_Mode_t     acnt_up_mode[3];
    FE_ACNT_Mode_t     acnt_dw_mode[3];
	GENERAL_TrtcmMode_T	  ingress_current_trtcm_mode[2];/* 0 for down stream, 1 for up stream */
	FE_Dir_t	  ingress_ratelimit_trtcm_invalid_index[2];/* 0 for down stream, 1 for up stream */

    unchar    ACNT_TRAFFIC_FLOW_UP[3][TRAFFIC_FLOW_MAX_NUM];
    char      ACNT_TRAFFIC_FLOW_UP_NAME[3][TRAFFIC_FLOW_MAX_NUM][NAME_LENGTH];

    unchar    ACNT_TRAFFIC_FLOW_DOWN[3][TRAFFIC_FLOW_MAX_NUM];
    char      ACNT_TRAFFIC_FLOW_DOWN_NAME[3][TRAFFIC_FLOW_MAX_NUM][NAME_LENGTH];

	unchar    ACNT_LLID_UP[3][LLID_MAX_NUM];
    char      ACNT_LLID_UP_NAME[3][LLID_MAX_NUM][NAME_LENGTH];

    unchar    ACNT_LLID_DOWN[3][LLID_MAX_NUM];
    char      ACNT_LLID_DOWN_NAME[3][LLID_MAX_NUM][NAME_LENGTH];
	
    unchar    ACNT_WAN_ITF_UP[3][WAN_ITF_ACNT_NUM];
    unchar    ACNT_WAN_ITF_DOWN[3][WAN_ITF_ACNT_NUM];

    unchar    ACNT_DEV_FLOW_UP[3][UNDER_DEV_MAX_NUM];
    unchar    ACNT_DEV_FLOW_DOWN[3][UNDER_DEV_MAX_NUM];

    unchar    Meter_Ctl_By_Olt; /*0: meter ctl by webpage, 1 :meter ctl by olt*/

	unchar    dbgLevel ;
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
extern int (*fe_resource_get_set_wan_idx_hook)( struct sk_buff *skb, u8 dir) ;
extern int (*fe_resource_mark_llid_hook)( struct sk_buff *skb, u8 llid, u8 dir);

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

int fe_get_meter_group(QDMA_MeterType_T *meter_type, FE_Dir_t dir, FE_Meter_Mode_t mode);
int fe_get_acnt_group(QDMA_AcntType_T *acnt_type, FE_Dir_t dir, FE_ACNT_Mode_t mode);
int fe_check_acnt_group(FE_Dir_t dir, QDMA_AcntType_T acnt_type, FE_ACNT_Mode_t mode);
int fe_check_meter_group(FE_Dir_t dir, QDMA_MeterType_T meter_type, FE_Meter_Mode_t mode);
unchar fe_get_meter_index_by_type(struct sk_buff *skb, QDMA_MeterType_T meter_type, int *meter_enable, int *idx);
unchar fe_get_acnt_index_by_type(struct sk_buff *skb, QDMA_AcntType_T acnt_type, uint *acnt_enable, uint *idx);
int fe_add_meter(FE_Dir_t dir, FE_Meter_Mode_t mode, QDMA_MeterType_T meter_type);
int fe_remove_meter(FE_Dir_t dir, FE_Meter_Mode_t mode, QDMA_MeterType_T meter_type);
int fe_deinit_acnt_resource_from_mode(int dir,  FE_ACNT_Mode_t acnt_mode, QDMA_AcntType_T acnt_type);
int fe_init_acnt_resource_from_mode(int dir, FE_ACNT_Mode_t acnt_mode, QDMA_AcntType_T acnt_type);
int fe_remark_skb_mark_from_traffic_mark(struct sk_buff *skb, int dir );
int fe_remark_skb_from_dev_mac(struct sk_buff *skb, int dir);
int fe_remark_skb_from_wan_idx(struct sk_buff *skb, u8 wan_index, u8 dir);
int fe_remark_skb_from_llid(struct sk_buff *skb, u8 llid, u8 dir);
void fe_account_meter_api_init(void);
void fe_account_meter_api_deinit(void);


#ifdef CONFIG_TP_IMAGE
#ifdef CONFIG_IPT_STAT_POLL_TP_ECONET
typedef int (* IPT_STAT_POLL_ECONET_CB)(int dir, unsigned char *mac,
								uint32_t packets, uint64_t bytes, int istcp);

typedef int (* IPT_POLL_STAT_ECONET_FUNC)(IPT_STAT_POLL_ECONET_CB poll_cb);
#endif /* CONFIG_IPT_STAT_POLL_TP_ECONET */
#endif /* CONFIG_TP_IMAGE */


#endif


