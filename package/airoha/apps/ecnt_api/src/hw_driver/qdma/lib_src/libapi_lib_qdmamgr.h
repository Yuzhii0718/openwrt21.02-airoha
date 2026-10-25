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
///@file libapi_lib_qdmamgr.h
///@brief QDMA Module API
#ifndef _API_LIB_QDMAMGR_H
#define _API_LIB_QDMAMGR_H

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/
///@brief enable/disable
typedef enum {
	ECNT_QDMAMGR_DISABLE = 0,		/**< disable */
	ECNT_QDMAMGR_ENABLE,			/**< enable */
}ECNT_QDMAMGR_ENABLE_MODE_TYPE;

///@brief Return value of QDMA API
typedef enum
{
    ECNT_QDMAMGR_SUCCESS = 0,       /**< api execution succeed */
    ECNT_QDMAMGR_INVALID_PARAM,     /**< parameters not valid */
    ECNT_QDMAMGR_NOT_SUPPORT,       /**< api not supported */
    ECNT_QDMAMGR_FAIL,              /**< api execution failed */
}ECNT_QDMAMGR_RET;

///@brief QDMA1/QDMA2 selection
typedef enum
{
    ECNT_QDMAMGR_QDMA_SEL_LAN = 0,   /**< QDMA1 */
    ECNT_QDMAMGR_QDMA_SEL_WAN,       /**< QDMA2 */
    ECNT_QDMAMGR_QDMA_SEL_MAX,
}ECNT_QDMAMGR_QDMA_SEL;

///@brief QDMA WRR scheduling base
typedef enum 
{
    ECNT_QDMAMGR_WRR_WEIGHT_BASE_BY_PACKET = 0, /**< scheduling by packet */
    ECNT_QDMAMGR_WRR_WEIGHT_BASE_BY_BYTE,       /**< scheduling by byte */
    ECNT_QDMAMGR_WRR_WEIGHT_BASE_MAX,
}ECNT_QDMAMGR_WRR_WEIGHT_BASE;

///@brief QDMA WRR weight scale for byte base scheduling
typedef enum 
{
    ECNT_QDMAMGR_WRR_WEIGHT_SCALE_1B = 0, /**< weight unit is 1B (EN758x only) */
    ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B,    /**< weight unit is 16B */ 
    ECNT_QDMAMGR_WRR_WEIGHT_SCALE_64B,    /**< weight unit is 64B (EN751x2x only) */ 
    ECNT_QDMAMGR_WRR_WEIGHT_SCALE_MAX,
}ECNT_QDMAMGR_WRR_WEIGHT_SCALE;

///@brief QDMA channel QoS mode
typedef enum 
{
    ECNT_QDMAMGR_QOS_MODE_WRR = 0,  /**< weight round robin */
    ECNT_QDMAMGR_QOS_MODE_SP,       /**< strict priority */
    ECNT_QDMAMGR_QOS_MODE_SPWRR7,   /**< SP: Q7, WRR: Q6-Q0 */
    ECNT_QDMAMGR_QOS_MODE_SPWRR6,   /**< SP: Q7-Q6, WRR: Q5-Q0 */
    ECNT_QDMAMGR_QOS_MODE_SPWRR5,   /**< SP: Q7-Q5, WRR: Q4-Q0 */
    ECNT_QDMAMGR_QOS_MODE_SPWRR4,   /**< SP: Q7-Q4, WRR: Q3-Q0 */
    ECNT_QDMAMGR_QOS_MODE_SPWRR3,   /**< SP: Q7-Q3, WRR: Q2-Q0 */
    ECNT_QDMAMGR_QOS_MODE_SPWRR2,   /**< SP: Q7-Q2, WRR: Q1-Q0 */
    ECNT_QDMAMGR_QOS_MODE_MAX
}ECNT_QDMAMGR_QOS_MODE;

///@brief QDMA ratectl module type
typedef enum
{
    ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM = 0, /**< ingress TRTCM */
    ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM,		   /**< sla TRTCM */
    ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT,      /**< egress queue ratelimit*/
    ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM,      /**< egress TRTCM */
    ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL,             /**< global ratelimit */
    ECNT_QDMAMGR_RATECTL_MODULE_MAX,
}ECNT_QDMAMGR_RATECTL_MODULE;

///@brief QDMA TRTCM ratelimit module works as trtcm mode or ratelimit mode
typedef enum
{
    ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT = 0,  /**< ratelimit mode */
    ECNT_QDMAMGR_TRTCM_MODE_TRTCM,          /**< trtcm mode */
    ECNT_QDMAMGR_TRTCM_MODE_MAX,
}ECNT_QDMAMGR_TRTCM_MODE;

///@brief QDMA rate mode
typedef enum
{
    ECNT_QDMAMGR_RATECTL_MODE_BYTE = 0,   /**< byte mode */
    ECNT_QDMAMGR_RATECTL_MODE_PACKET,     /**< packet mode */
    ECNT_QDMAMGR_RATECTL_MODE_MAX,
}ECNT_QDMAMGR_RATECTL_MODE;

///@brief QDMA rate tick type
typedef enum
{
    ECNT_QDMAMGR_TICK_TYPE_FAST = 0,  /**< fast tick */
    ECNT_QDMAMGR_TICK_TYPE_SLOW,      /**< slow tick */
    ECNT_QDMAMGR_TICK_TYPE_MAX,
}ECNT_QDMAMGR_TICK_TYPE;

//@brief QDMA egress ratemeter time divisor
typedef enum 
{
    ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_1 = 0,  /**< divisor is 1 */
    ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_2,      /**< divisor is 2 */
    ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_4,      /**< divisor is 4 */
    ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_8,      /**< divisor is 8 */
    ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_MAX
}ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR;

//@brief QDMA egress ratelimit bucket scale factor
typedef enum 
{
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_1BYTE = 0, /**< 1byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_2BYTE,     /**< 2byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_4BYTE,     /**< 4byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_8BYTE,     /**< 8byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_16BYTE,    /**< 16byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_32BYTE,    /**< 32byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_64BYTE,    /**< 64byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_128BYTE,   /**< 128byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_256BYTE,   /**< 256byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_512BYTE,   /**< 512byte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_1KBYTE,    /**< 1kbyte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_2KBYTE,    /**< 2kbyte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_4KBYTE,    /**< 4kbyte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_8KBYTE,    /**< 8kbyte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_16KBYTE,   /**< 16kbyte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_32KBYTE,   /**< 32kbyte */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_MAX,
} ECNT_QDMAMGR_BUCKET_SCALE_FACTOR ;

///@brief QDMA virtual channel mode
typedef enum 
{
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE_16QUEUE = 0,  /**< 16 queue mode */
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE_32QUEUE,      /**< 32 queue mode */
}ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE;

///@brief QDMA virtual channel QoS mode
typedef enum
{
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_WRR = 0,  /**< WRR */
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_SP,       /**< SP */
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_SPWRR3,   /**< SP WRR3 */
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_SPWRR2,   /**< SP WRR2 */
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_MAX,
}ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE;

///@brief QDMA DEI threshold scale
typedef enum
{
    ECNT_QDMAMGR_DEI_THR_SCALE_2 = 0,   /**< 1/2 nomal packet */
    ECNT_QDMAMGR_DEI_THR_SCALE_4,       /**< 1/4 nomal packet */
    ECNT_QDMAMGR_DEI_THR_SCALE_8,       /**< 1/8 nomal packet */
    ECNT_QDMAMGR_DEI_THR_SCALE_16,      /**< 1/16 nomal packet */
    ECNT_QDMAMGR_DEI_THR_SCALE_MAX
}ECNT_QDMAMGR_DEI_THR_SCALE;

///@brief QDMA TX congestion mode
typedef enum
{
    ECNT_QDMAMGR_TXCNGST_MODE_NORMAL = 0,       /**< normal mode */
    ECNT_QDMAMGR_TXCNGST_MODE_PEAKRATE_MARGIN,  /**< peak rate margin */
    ECNT_QDMAMGR_TXCNGST_MODE_STATIC,           /**< static mode */
    ECNT_QDMAMGR_TXCNGST_MODE_MAX,
}ECNT_QDMAMGR_TXCNGST_MODE;

///@brief QDMA TX congestion peak rate margin threshold
typedef enum
{
    ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_0 = 0, /**< dynamic threshold 0% margin */
    ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_25,    /**< dynamic threshold 25% margin */
    ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_50,    /**< dynamic threshold 50% margin */
    ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_100,   /**< dynamic threshold 100% margin */
    ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_MAX,
}ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN;

///@brief QDMA debug type
typedef enum
{
    ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_TXCPU = 0,  /**< debug tx cpu */
    ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_TXFWD1,     /**< debug tx fwd1 */
    ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_TXFWD2,     /**< debug tx fwd2 */
    ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_QUEUE,      /**< debug queue */
    ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_RING,       /**< debug ring */
    ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_MAX,
}ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE;

///@brief QDMA aging method type
typedef enum{
	ECNT_QDMAMGR_QOS_AGING_THRESHOLD=0, /**< when used dscp num greater than total_max_threshold */
	ECNT_QDMAMGR_QOS_AGING_ALWAYS=1,    /**< always */
}ECNT_QDMAMGR_AGING_METHOD_TYPE;

///@brief QDMA flow cnt clear type
typedef enum{
	ECNT_QDMAMGR_FLOWCNT_CLEAR_BYTE=0,
	ECNT_QDMAMGR_FLOWCNT_CLEAR_PKT,
	ECNT_QDMAMGR_FLOWCNT_CLEAR_ALL,
}ECNT_QDMAMGR_FLOW_CNT_CLEAR_TYPE;

///@brief QDMA wred packet type
typedef enum{
	ECNT_QDMAMGR_NORMAL_PACKET=0,
	ECNT_QDMAMGR_DEI_PACKET,
	ECNT_QDMAMGR_PACKET_TYPE_ITEMS
}ECNT_QDMAMGR_WRED_PACKET_Type;

///@brief QDMA debug level
typedef enum {
	ECNT_QDMAMGR_DBG_ERR ,
	ECNT_QDMAMGR_DBG_ST ,
	ECNT_QDMAMGR_DBG_WARN ,
	ECNT_QDMAMGR_DBG_MSG ,
	ECNT_QDMAMGR_DBG_LEVEL_MAX
} ECNT_QDMAMGR_DebugLevel_t ;

/************************************************************************
*                            M A C R O S
*************************************************************************
*/
///@brief QDMA channel queue number
#define ECNT_QDMAMGR_CHANNEL_QUEUE_NUM 8

///@brief QDMA per channel tick sel num
#define ECNT_QDMAMGR_PER_CHANNEL_TICK_SEL_NUM 2

///@brief QDMA virtual channel queue number
#define ECNT_QDMAMGR_VIRTUAL_CHANNEL_QUEUE_NUM 8

///@brief QDMA maximum RX ring num
#if defined(TCSUPPORT_CPU_EN7581)
#define ECNT_QDMAMGR_MAX_RX_RING_NUM 32
#else
#define ECNT_QDMAMGR_MAX_RX_RING_NUM 16
#endif

///@brief QDMA maximum TX WRED threshold num
#define ECNT_QDMAMGR_TX_WRED_THR_NUM				(5)

///@brief QDMA maximum TX WRED probability num
#define ECNT_QDMAMGR_TX_WRED_PROBABILITY_NUM		(4)

///@brief QDMA maximum busy timer
#define ECNT_QDMAMGR_BUSY_TIMER_MAX 60 /* 60s */

///@brief QDMA maximum register polling
#define ECNT_QDMAMGR_REG_POLLING_MAX 60 /* 60s */

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
#ifndef unchar
typedef unsigned char unchar;
#endif

#ifndef ushort
typedef unsigned short ushort;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef ulong
typedef unsigned long ulong;
#endif

#ifndef u8
typedef unsigned char u8;
#endif
        
#ifndef u16
typedef unsigned short u16;
#endif
    
#ifndef u32
typedef unsigned int u32;
#endif

///@brief QDMA channel QoS mode and queue weight
typedef struct
{
    ECNT_QDMAMGR_QOS_MODE qos_mode;  /**< QoS mode */
    u8 queue_weight[ECNT_QDMAMGR_CHANNEL_QUEUE_NUM];    /**< queue weight for WRR mode */
}ECNT_QDMAMGR_CHANNEL_QOS ;

///@brief QDMA ratectl configuration
typedef struct 
{
    ECNT_QDMAMGR_RATECTL_MODULE module;    /**< set ingress, sla, egress or global */
    u8 enable;  /**< enable trtcm function or not */
    ECNT_QDMAMGR_TRTCM_MODE mode;   /**< work as trtcm mode or ratelimit mode, invalid for  global ratectl mode */
    u32 slow_tick_ratio;    /**< slow_tick = slow_tickratio X fast_tick */
    u32 fast_tick;  /**< unit: us */
}ECNT_QDMAMGR_RATECTL_CFG;

///@brief QDMA ratectl ratelimit mode configuration
typedef struct 
{
    ECNT_QDMAMGR_RATECTL_MODULE module;    /**< set ingress, egress or global */
    u16 index;   /**< egress: means channel ; ingress: means ring or meter */
    u8 enable;  /**< meter enable */
    ECNT_QDMAMGR_RATECTL_MODE rate_mode; /**< packet mode or byte mode */
    ECNT_QDMAMGR_TICK_TYPE tick_type; /**< slow tick or fast tick */
}ECNT_QDMAMGR_RATELIMIT_MODE_CFG;

///@brief QDMA ratectl ratelimit configuration
typedef struct
{
    ECNT_QDMAMGR_RATECTL_MODULE module;    /**< set ingress, egress or global */
    u16 index;   /**< egress: means channel ; ingress: means ring or meter */
    u32 rate;   /**< rate value */
    u32 bucket_size;    /**< bucket size */
}ECNT_QDMAMGR_RATELIMIT_MODE_VALUE;

///@brief QDMA ratectl TRTCM mode configuration
typedef struct
{
    ECNT_QDMAMGR_RATECTL_MODULE module;    /**< set ingress, egress or global */
    u16 index;   /**< egress: means channel ; ingress: means ring or meter */
    u8 cbs_meter_en;    /**< enable meter for CBS */
    u8 pbs_meter_en;    /**< enable meter for PBS */
    ECNT_QDMAMGR_RATECTL_MODE cbs_rate_mode; /**< CBS packet mode or byte mode */
    ECNT_QDMAMGR_RATECTL_MODE pbs_rate_mode; /**< PBS packet mode or byte mode */
    ECNT_QDMAMGR_TICK_TYPE cbs_tick_type; /**< CBS slow tick or fast tick */
    ECNT_QDMAMGR_TICK_TYPE pbs_tick_type; /**< PBS slow tick or fast tick */
}ECNT_QDMAMGR_TRTCM_MODE_CFG;

///@brief QDMA ratectl TRTCM configuration
typedef struct
{
    ECNT_QDMAMGR_RATECTL_MODULE module;    /**< set ingress, egress or global */
    u16 index;   /**< egress: means channel ; ingress: means ring or meter */
    u32 cbs;    /**< CBS */
    u32 pbs;    /**< PBS */
    u32 cbs_bucket_size;    /**< CBS bucket size */
    u32 pbs_bucket_size;    /**< PBS bucket size */
}ECNT_QDMAMGR_TRTCM_MODE_VALUE;

///@brief QDMA egress ratelimit configuration
typedef struct 
{
    u16 unit[ECNT_QDMAMGR_PER_CHANNEL_TICK_SEL_NUM]; /**< unit is kbps, range[(1~65] */
    ECNT_QDMAMGR_BUCKET_SCALE_FACTOR scale; /**< Scaled factor for ratelimit bucket */
}ECNT_QDMAMGR_EGS_RATELIMIT_CFG;

///@brief QDMA rx ratelimit configuration
typedef struct 
{
    u8 enable;  /**< rx ratelimit enable */
    ECNT_QDMAMGR_RATECTL_MODE pkt_mode; /**< ratelimit packet mode */
    //u16 unit; /**< unit is kbps, scope is (1~65), or pps, scope is (16~1000) */
    //ECNT_QDMAMGR_BUCKET_SCALE_FACTOR bucket_scale;  /**< bucket scale */
    //u16 unit0;   /**< unit0 */
}ECNT_QDMAMGR_RX_RATELIMIT_CFG;

///@brief QDMA virtual channel QoS mode and queue weight
typedef struct
{
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE qos_mode;  /**< QoS mode */
    u8 queue_weight[ECNT_QDMAMGR_VIRTUAL_CHANNEL_QUEUE_NUM];    /**< queue weight for WRR mode */
}ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS;

///@brief QDMA RX ring low threshold
typedef struct 
{
    u32 threshold[ECNT_QDMAMGR_MAX_RX_RING_NUM];   /**< RX ring low threshold */
}ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD;

///@brief QDMA DBA report
typedef struct
{
    u32 cir;    /**< committed information rate */
    u32 cbs;    /**< committed burst size */
    u32 pir;    /**< peak information rate */
    u32 pbs;    /**< peak burst size */
}ECNT_QDMAMGR_DBA_REPORT;

///@brief QDMA TX congestion configuration
typedef struct
{
    ECNT_QDMAMGR_TXCNGST_MODE mode; /**< tx congestion mode */
    ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN peak_rate_margin;  /**< margin for peak rate mode */
    u32 peak_rate_duration; /**< peak rate duration */
}ECNT_QDMAMGR_TXCNGST;

///@brief QDMA TX congestion peak rate configuration
typedef struct
{
    u8 enable;  /**< enabe/disable TX congestion peak margin mode */
    ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN peak_rate_margin;  /**< margin for peak rate mode */
    u32 peak_rate_duration; /**< peak rate duration */
}ECNT_QDMAMGR_TXCNGST_PEAK_RATE_CFG;

//@brief QDMA TX congestion static queue normal threshold
typedef struct
{
    u32 threshold[ECNT_QDMAMGR_CHANNEL_QUEUE_NUM];  /**< static queue normal threshold */
}ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_NORMAL_THRESHOLD;

//@brief QDMA TX congestion static queue normal threshold
typedef struct
{
    u32 threshold[ECNT_QDMAMGR_CHANNEL_QUEUE_NUM];  /**< static queue dei threshold */
}ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_DEI_THRESHOLD;

//@brief QDMA aging cfg
typedef struct {  
    ECNT_QDMAMGR_ENABLE_MODE_TYPE	agingEn;			  	/**< aging enable */
    ECNT_QDMAMGR_AGING_METHOD_TYPE	agingMethod;			/**< aging methold */
    ECNT_QDMAMGR_ENABLE_MODE_TYPE	agingFastReplaceEn;		/**< aging fast replace mode enable */
    uint							agingTime;				/**< aging time, unit: us */
}ECNT_QDMAMGR_QOS_AGING_CFG;

//@brief QDMA flow cnt info
typedef struct {
    unchar  					grpIdx;
    unchar  					cntIdx;
    uint                        byteLow;
    uint                        byteHigh;
    uint                        pktCnt;
	uint                        pktHigh;
}ECNT_QDMAMGR_FLOW_CNT;

//@brief QDMA tx wred cfg
typedef struct {
    ECNT_QDMAMGR_WRED_PACKET_Type	txWredPacketType;
    unchar							txWredThrProbability[ECNT_QDMAMGR_TX_WRED_PROBABILITY_NUM];
    unchar							txWredThrValue[ECNT_QDMAMGR_TX_WRED_THR_NUM];
}ECNT_QDMAMGR_TX_WRED_CFG;

//@brief QDMA static enable
typedef struct {  
    unchar			chnlIdx; 
    unchar			staticThrEn;
}ECNT_QDMAMGR_STATIC_CFG ;

//@brief QDMA static ratio
typedef struct {  
    unchar				queueThrRatio[ECNT_QDMAMGR_CHANNEL_QUEUE_NUM] ;
}ECNT_QDMAMGR_STATIC_RATIO ;

//@brief QDMA dynamic total threshold
typedef struct {
	uint				totalMaxThrh ;
	uint				totalMinThrh ;
}ECNT_QDMAMGR_TOTAL_THR ;

//@brief QDMA lmgr status
typedef struct {
	uint freeLmgr;
	uint usedLmgr;
	uint usedBuf;
}ECNT_QDMAMGR_LMGR_STATUS;

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/**
@brief Set QDMA WRR configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] base WRR scheduling by packet or byte
@param[in] scale WRR scheduling scale
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_wrr_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRR_WEIGHT_BASE base, ECNT_QDMAMGR_WRR_WEIGHT_SCALE scale);

/**
@brief Get QDMA WRR configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_base WRR scheduling by packet or byte
@param[in] p_scale WRR scheduling scale
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_wrr_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRR_WEIGHT_BASE *p_base, ECNT_QDMAMGR_WRR_WEIGHT_SCALE *p_scale);

/**
@brief Set QDMA channel qos
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[in] p_channel_qos channel qos configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_CHANNEL_QOS *p_channel_qos);

/**
@brief Get QDMA channel qos
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[out] p_channel_qos channel qos configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_CHANNEL_QOS *p_channel_qos);

/**
@brief Set QDMA ratectl global configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_cfg ratectl global configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATECTL_CFG *p_cfg);

/**
@brief Get QDMA ratectl global configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_cfg ratectl global configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATECTL_CFG *p_cfg);

/**
@brief Set QDMA ratectl ratelimit mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_ratelimit_cfg ratectl ratelimit mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_CFG *p_ratelimit_cfg);

/**
@brief Get QDMA ratectl ratelimit mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_ratelimit_cfg ratectl ratelimit mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_ratelimit_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_CFG *p_ratelimit_cfg);

/**
@brief Set QDMA ratectl ratelimit mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_ratelimit ratectl ratelimit mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_ratelimit_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_VALUE *p_ratelimit);

/**
@brief Get QDMA ratectl ratelimit mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_ratelimit ratectl ratelimit mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_ratelimit_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_VALUE *p_ratelimit);

/**
@brief Set QDMA ratectl ratelimit mode bucket size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_ratelimit ratectl ratelimit mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_ratelimit_bucket_size(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_VALUE *p_ratelimit);

/**
@brief Set QDMA ratectl trtcm mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_trtcm_cfg ratectl trtcm mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_trtcm_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_CFG *p_trtcm_cfg);

/**
@brief Get QDMA ratectl trtcm mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_trtcm_cfg ratectl trtcm mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_trtcm_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_CFG *p_trtcm_cfg);

/**
@brief Set QDMA ratectl trtcm mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_trtcm ratectl trtcm mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_trtcm_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_VALUE *p_trtcm);

/**
@brief Get QDMA ratectl trtcm mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_trtcm ratectl trtcm mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_trtcm_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_VALUE *p_trtcm);

/**
@brief Set QDMA ratectl trtcm mode bucket size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_trtcm ratectl trtcm mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_trtcm_bucket_size(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_VALUE *p_trtcm);

/**
@brief Set QDMA multicast enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_multicast_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable);

/**
@brief Get QDMA multicast enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_multicast_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable);

/**
@brief Set QDMA flow cnt cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] grpIdx flowcnt group index
@param[in] incDropEn INC drop or not
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_flow_cntr_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 grpIdx, u8 incDropEn);

/**
@brief Get QDMA flow cnt cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] grpIdx flowcnt group index
@param[in] incDropEn INC drop or not
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_flow_cntr_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 grpIdx, u8 *p_incDropEn);

/**
@brief Get QDMA flow cnt value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] cntValue flow cnt info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_flow_cntr_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_FLOW_CNT *p_cntValue);

/**
@brief Set QDMA flow cnt value clear
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] grpIdx group index
@param[in] cntIdx cnt index
@param[in] clearType byte/packet/all
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_clear_flow_cntr_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 grpIdx, u8 cntIdx, ECNT_QDMAMGR_FLOW_CNT_CLEAR_TYPE clearType);

/**
@brief Set QDMA tx wred mode
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable on/off
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_tx_wred_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable);

/**
@brief Get QDMA tx wred mode
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable on/off
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_tx_wred_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable);

/**
@brief Set QDMA tx wred threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_txWred, packet type & threshold info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_tx_wred_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TX_WRED_CFG *p_txWred);

/**
@brief Set QDMA tx wred probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_txWred, packet type & probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_tx_wred_probability(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TX_WRED_CFG *p_txWred);

/**
@brief Get QDMA tx wred threshold & probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_txWred, packet type & threshold & probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_tx_wred_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TX_WRED_CFG *p_txWred);

/**
@brief Set QDMA rx red probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] type, packet type, normal/DEI
@param[in] probability, probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_rx_red_probability(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRED_PACKET_Type type, unchar probability);

/**
@brief Get QDMA rx red probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] type, packet type, normal/DEI
@param[in] probability, probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_rx_red_probability(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRED_PACKET_Type type, unchar *p_probability);

/**
@brief Set QDMA sla channel cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] sla index
@param[in] enable
@param[in] channel index
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_sla_channel_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 slaIdx, u8 enable, u8 chnlIdx);

/**
@brief Get QDMA sla channel cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] sla index
@param[in] enable
@param[in] channel index
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_sla_channel_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 slaIdx, u8 *p_enable, u8 *p_chnlIdx);

/**
@brief Set QDMA qos aging config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_qosAgingCfg aging config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_qos_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_QOS_AGING_CFG *p_qosAgingCfg);

/**
@brief Get QDMA qos aging config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_qosAgingCfg aging config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_qos_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_QOS_AGING_CFG *p_qosAgingCfg);

/**
@brief Set QDMA qos aging per-queue enable/disable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] queueIdx queue index
@param[in] agingEn enable/disable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_per_queue_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 queueIdx, u8 agingEn);

/**
@brief Get QDMA qos aging per-queue enable/disable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] queueIdx queue index
@param[in] p_agingEn enable/disable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_per_queue_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 queueIdx, u8 *p_agingEn);

/**
@brief Set QDMA multicast sptag cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] sptagKeepHiEn keep sp_tag hi-bits or not
@param[in] sptag special tag info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_multicast_sptag_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 sptagKeepHiEn, u16 sptag);

/**
@brief Get QDMA multicast sptag cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] sptagKeepHiEn keep sp_tag hi-bits or not
@param[in] sptag special tag info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_multicast_sptag_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 *p_sptagKeepHiEn, u16 *p_sptag);

/**
@brief Set QDMA multicast force port cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] fport force port
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_multicast_fport_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u16 fport);

/**
@brief Get QDMA multicast force port cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] fport force port
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_multicast_fport_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u16 *p_fport);

/**
@brief Set QDMA static cngst channel enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_channel_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 enable);

/**
@brief Set QDMA static cngst queue ratio value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_static_ratio, per-queue ratio
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_queue_ratio(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_STATIC_RATIO *p_static_ratio);

/**
@brief Get QDMA static cngst queue cfg info
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_total_thr, total max/min threshold
@param[in] p_static_cfg, channel static threshold enable or not
@param[in] p_static_ratio, per-queue ratio
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_static_cngst_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel,
															ECNT_QDMAMGR_TOTAL_THR *p_total_thr, 
															ECNT_QDMAMGR_STATIC_CFG *p_static_cfg,
															ECNT_QDMAMGR_STATIC_RATIO *p_static_ratio);


/**
@brief Set QDMA egress rate meter time
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] div 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR div, u16 time_slice);

/**
@brief Get QDMA egress rate meter time
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] div 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR *p_div, u16 *p_time_slice);

/**
@brief Set QDMA egress rate meter time window size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] windowSize 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 windowSize, u16 timeSlice);

/**
@brief Get QDMA egress rate meter time window size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] windowSize 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_windowSize, u16 *p_timeSlice);

/**
@brief Set QDMA DEI drop enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable enable/disable DEI drop
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_dei_drop_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable);

/**
@brief Get QDMA DEI drop enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable enable/disable DEI drop
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_dei_drop_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable);

/**
@brief Set QDMA congestion total threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] max_threshold maximum threshold
@param[in] min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_total_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 max_threshold, u16 min_threshold);

/**
@brief Get QDMA congestion total threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_max_threshold maximum threshold
@param[out] p_min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_total_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_max_threshold, u16 *p_min_threshold);

/**
@brief Set QDMA congestion channel threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] max_threshold maximum threshold
@param[in] min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_channel_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 max_threshold, u16 min_threshold);

/**
@brief Get QDMA congestion channel threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_max_threshold maximum threshold
@param[out] p_min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_channel_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_max_threshold, u16 *p_min_threshold);

/**
@brief Set QDMA congestion queue threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] max_threshold maximum threshold
@param[in] min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_queue_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 max_threshold, u16 min_threshold);

/**
@brief Get QDMA congestion queue threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_max_threshold maximum threshold
@param[out] p_min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_queue_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_max_threshold, u16 *p_min_threshold);

/**
@brief Dump QDMA Txq congestion Info
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Get QDMA congestion mode
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable congestion mode
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable);

/**
@brief Get QDMA llmgr status
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] lmgrPtr lmgr status
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_lmgr_status(ECNT_QDMAMGR_QDMA_SEL qdma_sel,ECNT_QDMAMGR_LMGR_STATUS *lmgrPtr);

/**
@brief Get QDMA lmgr low threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_threshold lmgr low threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 *p_threshold);

/**
@brief Set QDMA lmgr low threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] threshold lmgr low threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 threshold);

/**
@brief Get QDMA overhead enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable overhead enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_overhead_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable);

/**
@brief Set QDMA overhead enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[int] enable overhead enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_overhead_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable);

/**
@brief Get QDMA overhead value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_cnt overhead value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_overhead_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_cnt);

/**
@brief Set QDMA overhead value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] cnt overhead value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_overhead_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 cnt);

/**
@brief Get QDMA mac resource limit
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_channel_threshold mac resource limit channel thresold
@param[out] p_queue_threshold mac resource limit queue thresold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 *p_channel_threshold, u32 *p_queue_threshold);

/**
@brief Set QDMA mac resource limit
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel_threshold mac resource limit channel thresold
@param[in] queue_threshold mac resource limit queue thresold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 channel_threshold, u32 queue_threshold);

/**
@brief Get QDMA txbuf threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable PSE bufcontrol enable
@param[out] p_channel_threshold channel threshold
@param[out] p_total_threshold total threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7523
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable, u16 *p_channel_threshold, u16 *p_total_threshold);

/**
@brief Set QDMA txbuf threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable PSE bufcontrol enable
@param[in] channel_threshold channel threshold
@param[in] total_threshold total threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7523
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable, u16 channel_threshold, u16 total_threshold);

/**
@brief Get QDMA dba report
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[out] p_dba_report dba report
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_dba_report(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_DBA_REPORT *p_dba_report);

/**
@brief Set QDMA tx DBA report
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[in] p_dba_report dba report
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_dba_report(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_DBA_REPORT *p_dba_report);

/**
@brief Dump QDMA dba report 
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_dbareport(ECNT_QDMAMGR_QDMA_SEL qdma_sel,int channel);

/**
@brief Set QDMA egress ratelimit config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_cfg egress ratelimit config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATELIMIT_CFG *p_cfg);

/**
@brief Get QDMA egress ratelimit config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_cfg egress ratelimit config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATELIMIT_CFG *p_cfg);

/**
@brief Set QDMA txq dynamic congestion enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable enable/disable QDMA txq dynamic congestion
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable);

/**
@brief Set QDMA txq congestion dei scale
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] scale dei scale
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_dei_scale(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_DEI_THR_SCALE scale);

/**
@brief Set QDMA txq congestion autoconfig
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_tx_cngst autoconfig
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_autoconfig(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST *p_tx_cngst);

/**
@brief Set QDMA static congestion dei
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_cfg static congestion dei
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_dei(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_DEI_THRESHOLD *p_cfg);

/**
@brief Set QDMA dbgcntr channel type
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] type channel type
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_dbgcntr_channel(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE type);

/**
@brief Set QDMA txq congestion peekrate
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_cfg peekrate config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_peekrate(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST_PEAK_RATE_CFG *p_cfg);

/**
@brief Set QDMA txq congestion normal config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_cfg normal config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_normal(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_NORMAL_THRESHOLD *p_cfg);

/**
@brief Set QDMA dbgcntr queue
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_dbgcntr_queue(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Clear QDMA dbgcntr queue
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_clear_dbgcntr(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief dump QDMA dbgcntr
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_dbgcntr(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief clear QDMA cpu counter
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_clear_cpu_counter(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Dump QDMA cpu counter
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_cpu_counter(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Dump QDMA register
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_register(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Dump QDMA descriptor
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_descriptor(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Dump QDMA irq
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_irq(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Dump QDMA hwfwd
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_hwfwd(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Dump QDMA all info
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_dump_all(ECNT_QDMAMGR_QDMA_SEL qdma_sel);

/**
@brief Set QDMA debug level
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] level debug level
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_test_dbglevel(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint level);

/**
@brief Set QDMA dma busy
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] busyDuration busyDuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_test_dma_busy(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint busyDuration);

/**
@brief Set QDMA test reg polling
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] regOffset register offset
@param[in] pollingDuration register polling duration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_test_reg_polling(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint regOffset, uint pollingDuration);

/**
@brief Set QDMA test force rx1
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] forceEn force enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_test_force_rx1(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint forceEn);

/**
@brief Set QDMA prefetch
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] pre_fetch_en prefetch enable
@param[in] overdrag_en overdrag enable
@param[in] overdrag_cnt overdrag count
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_prefetch(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 pre_fetch_en, u8 overdrag_en, u32 overdrag_cnt);

/**
@brief Set QDMA DownStream_Qos enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable enable/disable DownStream_Qos
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7529 EN7581 AN7552 AN7583
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_downstream_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable);

/**
@brief Get QDMA DownStream_Qos enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable enable/disable DownStream_Qos
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7529 EN7581 AN7552 AN7583
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_downstream_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable);

// ============== redefine  =================
#define ecnt_qdmamgr_lib_set_wrr_mode_cfg qdmamgr_lib_set_wrr_mode_cfg
#define ecnt_qdmamgr_lib_get_wrr_mode_cfg qdmamgr_lib_get_wrr_mode_cfg
#define ecnt_qdmamgr_lib_set_channel_qos qdmamgr_lib_set_channel_qos
#define ecnt_qdmamgr_lib_get_channel_qos qdmamgr_lib_get_channel_qos
#define ecnt_qdmamgr_lib_set_ratectl_cfg qdmamgr_lib_set_ratectl_cfg
#define ecnt_qdmamgr_lib_get_ratectl_cfg qdmamgr_lib_get_ratectl_cfg
#define ecnt_qdmamgr_lib_set_ratectl_ratelimit_mode_cfg qdmamgr_lib_set_ratectl_ratelimit_mode_cfg
#define ecnt_qdmamgr_lib_get_ratectl_ratelimit_mode_cfg qdmamgr_lib_get_ratectl_ratelimit_mode_cfg
#define ecnt_qdmamgr_lib_set_ratectl_ratelimit_mode_value qdmamgr_lib_set_ratectl_ratelimit_mode_value
#define ecnt_qdmamgr_lib_get_ratectl_ratelimit_mode_value qdmamgr_lib_get_ratectl_ratelimit_mode_value
#define ecnt_qdmamgr_lib_set_ratectl_ratelimit_bucket_size qdmamgr_lib_set_ratectl_ratelimit_bucket_size
#define ecnt_qdmamgr_lib_set_multicast_en qdmamgr_lib_set_multicast_en
#define ecnt_qdmamgr_lib_get_multicast_en qdmamgr_lib_get_multicast_en
#define ecnt_qdmamgr_lib_set_flow_cntr_cfg qdmamgr_lib_set_flow_cntr_cfg
#define ecnt_qdmamgr_lib_get_flow_cntr_cfg qdmamgr_lib_get_flow_cntr_cfg
#define ecnt_qdmamgr_lib_get_flow_cntr_value qdmamgr_lib_get_flow_cntr_value
#define ecnt_qdmamgr_lib_clear_flow_cntr_value qdmamgr_lib_clear_flow_cntr_value
#define ecnt_qdmamgr_lib_set_tx_wred_mode qdmamgr_lib_set_tx_wred_mode
#define ecnt_qdmamgr_lib_get_tx_wred_mode qdmamgr_lib_get_tx_wred_mode
#define ecnt_qdmamgr_lib_set_tx_wred_threshold qdmamgr_lib_set_tx_wred_threshold
#define ecnt_qdmamgr_lib_set_tx_wred_probability qdmamgr_lib_set_tx_wred_probability
#define ecnt_qdmamgr_lib_get_tx_wred_cfg qdmamgr_lib_get_tx_wred_cfg
#define ecnt_qdmamgr_lib_set_rx_red_probability qdmamgr_lib_set_rx_red_probability
#define ecnt_qdmamgr_lib_get_rx_red_probability qdmamgr_lib_get_rx_red_probability
#define ecnt_qdmamgr_lib_set_sla_channel_cfg qdmamgr_lib_set_sla_channel_cfg
#define ecnt_qdmamgr_lib_get_sla_channel_cfg qdmamgr_lib_get_sla_channel_cfg
#define ecnt_qdmamgr_lib_set_qos_aging_cfg qdmamgr_lib_set_qos_aging_cfg
#define ecnt_qdmamgr_lib_get_qos_aging_cfg qdmamgr_lib_get_qos_aging_cfg
#define ecnt_qdmamgr_lib_set_per_queue_aging_cfg qdmamgr_lib_set_per_queue_aging_cfg
#define ecnt_qdmamgr_lib_get_per_queue_aging_cfg qdmamgr_lib_get_per_queue_aging_cfg
#define ecnt_qdmamgr_lib_set_multicast_sptag_cfg qdmamgr_lib_set_multicast_sptag_cfg
#define ecnt_qdmamgr_lib_get_multicast_sptag_cfg qdmamgr_lib_get_multicast_sptag_cfg
#define ecnt_qdmamgr_lib_set_multicast_fport_cfg qdmamgr_lib_set_multicast_fport_cfg
#define ecnt_qdmamgr_lib_get_multicast_fport_cfg qdmamgr_lib_get_multicast_fport_cfg
#define ecnt_qdmamgr_lib_set_static_cngst_channel_en qdmamgr_lib_set_static_cngst_channel_en
#define ecnt_qdmamgr_lib_set_static_cngst_queue_ratio qdmamgr_lib_set_static_cngst_queue_ratio
#define ecnt_qdmamgr_lib_get_static_cngst_cfg qdmamgr_lib_get_static_cngst_cfg
#define ecnt_qdmamgr_lib_set_ratectl_trtcm_mode_cfg qdmamgr_lib_set_ratectl_trtcm_mode_cfg
#define ecnt_qdmamgr_lib_get_ratectl_trtcm_mode_cfg qdmamgr_lib_get_ratectl_trtcm_mode_cfg
#define ecnt_qdmamgr_lib_set_ratectl_trtcm_mode_value qdmamgr_lib_set_ratectl_trtcm_mode_value
#define ecnt_qdmamgr_lib_get_ratectl_trtcm_mode_value qdmamgr_lib_get_ratectl_trtcm_mode_value
#define ecnt_qdmamgr_lib_set_ratectl_trtcm_bucket_size qdmamgr_lib_set_ratectl_trtcm_bucket_size
#define ecnt_qdmamgr_lib_set_egress_ratemeter_time_cfg qdmamgr_lib_set_egress_ratemeter_time_cfg
#define ecnt_qdmamgr_lib_get_egress_ratemeter_time_cfg qdmamgr_lib_get_egress_ratemeter_time_cfg
#define ecnt_qdmamgr_lib_set_egress_ratemeter_window_cfg qdmamgr_lib_set_egress_ratemeter_window_cfg
#define ecnt_qdmamgr_lib_get_egress_ratemeter_window_cfg qdmamgr_lib_get_egress_ratemeter_window_cfg
#define ecnt_qdmamgr_lib_set_dei_drop_en qdmamgr_lib_set_dei_drop_en
#define ecnt_qdmamgr_lib_get_dei_drop_en qdmamgr_lib_get_dei_drop_en
#define ecnt_qdmamgr_lib_set_txq_cngst_total_threshold qdmamgr_lib_set_txq_cngst_total_threshold
#define ecnt_qdmamgr_lib_get_txq_cngst_total_threshold qdmamgr_lib_get_txq_cngst_total_threshold
#define ecnt_qdmamgr_lib_set_txq_cngst_channel_threshold qdmamgr_lib_set_txq_cngst_channel_threshold
#define ecnt_qdmamgr_lib_get_txq_cngst_channel_threshold qdmamgr_lib_get_txq_cngst_channel_threshold
#define ecnt_qdmamgr_lib_set_txq_cngst_queue_threshold qdmamgr_lib_set_txq_cngst_queue_threshold
#define ecnt_qdmamgr_lib_get_txq_cngst_queue_threshold qdmamgr_lib_get_txq_cngst_queue_threshold
#define ecnt_qdmamgr_lib_get_txq_cngst qdmamgr_lib_get_txq_cngst
#define ecnt_qdmamgr_lib_get_txq_cngst_mode  qdmamgr_lib_get_txq_cngst_mode
#define ecnt_qdmamgr_lib_get_lmgr_status qdmamgr_lib_get_lmgr_status
#define ecnt_qdmamgr_lib_get_lmgr_low_threshold qdmamgr_lib_get_lmgr_low_threshold
#define ecnt_qdmamgr_lib_set_lmgr_low_threshold qdmamgr_lib_set_lmgr_low_threshold
#define ecnt_qdmamgr_lib_get_overhead_en qdmamgr_lib_get_overhead_en
#define ecnt_qdmamgr_lib_set_overhead_en qdmamgr_lib_set_overhead_en
#define ecnt_qdmamgr_lib_get_overhead_value qdmamgr_lib_get_overhead_value
#define ecnt_qdmamgr_lib_set_overhead_value qdmamgr_lib_set_overhead_value
#define ecnt_qdmamgr_lib_get_txbuf_threshold qdmamgr_lib_get_txbuf_threshold
#define ecnt_qdmamgr_lib_set_txbuf_threshold  qdmamgr_lib_set_txbuf_threshold
#define ecnt_qdmamgr_lib_dump_dbareport qdmamgr_lib_dump_dbareport
#define ecnt_qdmamgr_lib_set_egress_ratelimit_config qdmamgr_lib_set_egress_ratelimit_config
#define ecnt_qdmamgr_lib_set_txq_cngst_mode qdmamgr_lib_set_txq_cngst_mode
#define ecnt_qdmamgr_lib_set_txq_cngst_dei_scale qdmamgr_lib_set_txq_cngst_dei_scale
#define ecnt_qdmamgr_lib_set_txq_cngst_autoconfig qdmamgr_lib_set_txq_cngst_autoconfig
#define ecnt_qdmamgr_lib_set_static_cngst_dei qdmamgr_lib_set_static_cngst_dei
#define ecnt_qdmamgr_lib_set_txq_cngst_peekrate qdmamgr_lib_set_txq_cngst_peekrate
#define ecnt_qdmamgr_lib_set_static_cngst_normal qdmamgr_lib_set_static_cngst_normal
#define ecnt_qdmamgr_lib_set_dbgcntr_queue qdmamgr_lib_set_dbgcntr_queue
#define ecnt_qdmamgr_lib_clear_dbgcntr qdmamgr_lib_clear_dbgcntr
#define ecnt_qdmamgr_lib_dump_dbgcntr qdmamgr_lib_dump_dbgcntr
#define ecnt_qdmamgr_lib_clear_cpu_counter qdmamgr_lib_clear_cpu_counter
#define ecnt_qdmamgr_lib_dump_cpu_counter qdmamgr_lib_dump_cpu_counter
#define ecnt_qdmamgr_lib_dump_register qdmamgr_lib_dump_register
#define ecnt_qdmamgr_lib_dump_descriptor qdmamgr_lib_dump_descriptor
#define ecnt_qdmamgr_lib_dump_irq  qdmamgr_lib_dump_irq
#define ecnt_qdmamgr_lib_dump_hwfwd  qdmamgr_lib_dump_hwfwd
#define ecnt_qdmamgr_lib_dump_all qdmamgr_lib_dump_all
#define ecnt_qdmamgr_set_test_dbglevel qdmamgr_lib_set_test_dbglevel
#define ecnt_qdmamgr_set_test_dma_busy qdmamgr_lib_set_test_dma_busy
#define ecnt_qdmamgr_set_test_reg_polling qdmamgr_lib_set_test_reg_polling
#define ecnt_qdmamgr_set_test_force_rx1 qdmamgr_lib_set_test_force_rx1
#define ecnt_qdmamgr_get_mac_resource_limit qdmamgr_lib_get_mac_resource_limit
#define ecnt_qdmamgr_set_mac_resource_limit qdmamgr_lib_set_mac_resource_limit
#define ecnt_qdmamgr_set_prefetch qdmamgr_lib_set_prefetch
#define ecnt_qdmamgr_set_egress_ratelimit_config qdmamgr_lib_set_egress_ratelimit_config
#define ecnt_qdmamgr_get_egress_ratelimit_config qdmamgr_lib_get_egress_ratelimit_config
#define ecnt_qdmamgr_set_dba_report qdmamgr_lib_set_dba_report
#define ecnt_qdmamgr_get_dba_report qdmamgr_lib_get_dba_report

#endif /* _API_LIB_QDMAMGR_H */

