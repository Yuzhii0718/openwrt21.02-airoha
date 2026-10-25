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
#ifndef _API_LIB_QOSMGR_H
#define _API_LIB_QOSMGR_H

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/
typedef unsigned char           unchar;
typedef unsigned short          ushort;
typedef unsigned int            uint;
typedef unsigned long           ulong;
struct sk_buff;


typedef enum{
    ECNT_QOSMGR_SUCCESS=0,
    ECNT_QOSMGR_FAILED,
}ECNT_QOSMGR_RET;

typedef enum {
    PPE_METHOD_ADD = 1,
    PPE_METHOD_DEL,
    PPE_METHOD_SHOW,
    PPE_METHOD_GET,
    PPE_METHOD_GET_ALL,
}PPE_METHOD_TYPE_t;

typedef enum {
	METER_DISABLE = 0 ,
	METER_ENABLE 
} METER_Enable_t;

typedef enum{
    METER_FLOW_TYPE = 0,
    FLOW_TYPE,
    METER_TYPE,
}METER_ADD_TYPE_t;

typedef enum
{
    BYTE_MODE_SCALE_K = 0,	/**<  1Kbps */
    BYTE_MODE_SCALE_M,      /**<  1Mbps */
    BYTE_MODE_SCALE_G,      /**<  1Gbps */  
}METER_SCALE_TYPE;

struct ppe_meter_cfg {
    unsigned int trtcm      :1;
    unsigned int grpOrFlow  :1; /* 0: delete/show by group, 1: delete/show by flow(vlan,dir) */
    unsigned int method     :3; /* command method from user */
    unsigned int dir        :1; /* 0: upstream, 1:downstream */
    unsigned int vlan       :16; /* including DEI, Pbits, and vlanID */
    unsigned int queue      :4;	/* qos queue index;0xf:not care qos queue*/
    unsigned int scale      :2; /* 0:Kbps, 1:Mbps, 2:Gbps (K is 1000, M is 1000x1024, G is 1000x1024x1024) */
    unsigned int scale2     :2; /* in TRTCM, scale for pir, scale2 for cir */
    unsigned int mode       :1; /* 0:byte, 1:packet */
    unsigned int group      :6; /* 0~62 for Meter, 0~30 for TRTCM */

    unsigned int resv       :28;
    unsigned int retVal     :1; /* 0:success, 1: fail */
    unsigned int addFlowOnly:2; /* 0:add Meter & flow, 1: add flow only, 2: add meter only */
    unsigned int enable     :1; /* 0:disable, 1:enable */

    unsigned int rate; /*  16K ~ 2G bps for byte mode, 1 ~ 2047000 fps for packet mode */
    unsigned int rate2; /* in TRTCM, rate for pir, rate2 for cir */
};


/************************************************************************
*                  I N C L U D E S
************************************************************************/
#include "fe_type.h"
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/
//#define QDMA_MAGIC					(0xAB)
//#define IO_QDMA_CMD_OPT				_IOW(QDMA_MAGIC, 0, struct ECNT_QDMA_Data *)

#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
#define	WRR_QUEUE_NUM				(8)
#define	CAR_QUEUE_NUM				(8)
#else
#define	WRR_QUEUE_NUM				(4)
#define	CAR_QUEUE_NUM				(6)
#endif
#define WRR_QUEUE_MASK              (0xF)
#define CAR_QUEUE_MASK              (0x3F)

#define PPE_COMMAND_METER_GROUP    (0x2f)
#define PPE_COMMAND_METER_FLOW     (0x30)

/************************************************************************
*                  M A C R O S
************************************************************************/
#define hw_nat_path			"/dev/hwnat0"
#define fe_path             "/dev/fe"
#define pon_path	        "/dev/pon"
#define qdma_path           "/dev/qdma_wan"


/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/
int qosmgr_lib_set_qos_config_NULLQOS(char queue_mask);
int qosmgr_lib_set_qos_config_PQ(char queue_mask);
int qosmgr_lib_set_qos_config_WRR(char queue_mask,  unsigned int *weight);
int qosmgr_lib_set_qos_config_CAR(char queue_mask,  unsigned int *ratelimit);
int qosmgr_lib_set_qos_config_meter(unsigned int ratelimit, int mode, char index );
int qosmgr_lib_set_qos_config_rate(unsigned int uplinkBandwidth);
int qosmgr_lib_add_qos_config_meter_flow(unsigned int dir, unsigned int vlan, unsigned int queue, unsigned int group_id);
int qosmgr_lib_del_qos_config_meter_flow(unsigned int dir, unsigned int vlan);

#define ecnt_qosmgr_lib_set_qos_config_NULLQOS qosmgr_lib_set_qos_config_NULLQOS
#define ecnt_qosmgr_lib_set_qos_config_PQ qosmgr_lib_set_qos_config_PQ
#define ecnt_qosmgr_lib_set_qos_config_WRR qosmgr_lib_set_qos_config_WRR
#define ecnt_qosmgr_lib_set_qos_config_CAR qosmgr_lib_set_qos_config_CAR
#define ecnt_qosmgr_lib_set_qos_config_meter qosmgr_lib_set_qos_config_meter
#define ecnt_qosmgr_lib_set_qos_config_rate qosmgr_lib_set_qos_config_rate
#define ecnt_qosmgr_lib_add_qos_config_meter_flow qosmgr_lib_add_qos_config_meter_flow
#define ecnt_qosmgr_lib_del_qos_config_meter_flow qosmgr_lib_del_qos_config_meter_flow

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

#endif
