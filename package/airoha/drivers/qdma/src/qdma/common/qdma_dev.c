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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/igmp.h>
#include "qdma_dev.h"
#include "qdma_bmgr.h"
#include "qdma_dvt.h"
#include "qdma_ic_dis.h"
#include <linux/random.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define GDM2_CHNL_RELEASE_CMD						(1<<0)
#define GDM2_CHNL_RELEASE_DONE						(1<<1)
#define GDM2_CHNK_RELEASE_CHN_SHIFT					(4)
#define GDM2_CHNK_RELEASE_CHN_MASK					(0xF<<GDM2_CHNK_RELEASE_CHN_SHIFT)

#define CDM_TX_ENCODING_SHIFT					(0)
#define CDM_TX_ENCODING_MASK					(0xF<<CDM_TX_ENCODING_SHIFT)
#define CDM_RX_DECODING_SHIFT					(4)
#define CDM_RX_DECODING_MASK					(0xF<<CDM_RX_DECODING_SHIFT)
#define GDM_RX_DECODING_SHIFT					(8)
#define GDM_RX_DECODING_MASK					(0xF<<GDM_RX_DECODING_SHIFT)

#define DEF_PRIORITY_PKT_CHK_LEN     150

#define RATELIMIT_BYTE_MODE_BUCKET_SHIFT		(10)
#define RATELIMIT_PKT_MODE_BUCKET_SHIFT			(0)

#define MULTICAST_MAX_CHANNEL					(16)

#ifdef INCLUDE_ITTIM
#define QOS_ROUTER_VOIP							(0x61)
#define QOS_ROUTER_MASK							(0xFF)
#endif /* INCLUDE_ITTIM */

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define METER_1K	(1000)
#define METER_1M	(METER_1K<<10)

DEFINE_SPINLOCK(qdma_config_lock);
/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
#ifdef TCSUPPORT_QOS
extern int qos_flag;
extern int gQueueMask;
#endif
extern int qos_wrr_info[5];
extern atomic_t chnlLimit[CONFIG_QDMA_CHANNEL];
extern atomic_t queueLimit[CONFIG_QDMA_CHANNEL][CONFIG_QDMA_QUEUE];
extern atomic_t txRing1FreeCnt;
extern int txring1ReserveThreshold;

extern qdma_api_op_t qdma_operation[];

extern unsigned char NATv6_USE_HIGH_PRIORITY;

extern unchar qdmaLanHqosMode;	/* 0:disable, 1:enable */
extern unchar qdmaWanHqosMode;
extern unchar qdmaDscpMode;

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
struct timer_list dma_busy_timer ;
int dma_busy_expires = 10; /* 10ms */
int dma_busy_round_cnt = 0;
int tx_dma_busy_enable_cnt=0;
int rx_dma_busy_enable_cnt=0;

struct timer_list trigger_timer ;
int trigger_expires = 1; /* 1ms */
int trigger_timer_going = 0;

struct timer_list reg_polling_timer ;
int reg_polling_expires = 10; /* 10ms */
int reg_polling_round_cnt = 0;
int reg_polling_offset = 0;
uint qdmaRegValBuff[100];

uint channel_limit_threshold = 0 ;
uint queue_limit_threshold = 0 ;

QDMA_Dev_T *gpQdmaDev = NULL ;

unchar qdma_vip_num = 0 ;/*vip rule num*/
QDMA_VIP_INFO_T qdma_vip_info[QDMA_VIP_PACKET_NUM] ; /*sync with fe info,just for port compare*/

uint trtcmCfgBase[TRTCM_MODE_MAX] ;
uint trtcmBucketByteUnit[TRTCM_MODE_MAX] ;
uint trtcmBucketPacketUnit[TRTCM_MODE_MAX] ;

int BUFFER_THRESHOLD[6] = {0};


#if SUPPORT_QUEUE_DISABLE
/* For EN7580,EN7528: record TXQ_DIS_CFG_CHN register value , the register only can write, can not read */
uint TXQ_DIS_CFG_VALUE[TXQ_DIS_CFG_REG_NUM] = {0, 0, 0, 0, 0, 0, 0, 0};
#endif

#if SUPPORT_QDMA_MULTICAST
/***********************Multicast force port information******************************
*            channel-0 => PSE port-1 nbq-0 => sp_tag-0x0001 => keep_sptag_hifield-1
*            channel-1 => PSE port-1 nbq-1 => sp_tag-0x0002 => keep_sptag_hifield-1
*            channel-2 => PSE port-1 nbq-2 => sp_tag-0x0004 => keep_sptag_hifield-1
*            channel-3 => PSE port-1 nbq-3 => sp_tag-0x0008 => keep_sptag_hifield-1
*            channel-4 => PSE port-1 nbq-4 => sp_tag-0x0010 => keep_sptag_hifield-1
*            channel-5 => PSE port-1 nbq-5 => sp_tag-0x0020 => keep_sptag_hifield-1
*            channel-6 => PSE port-0 nbq-0 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-7 => PSE port-4 nbq-0 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-8 => PSE port-3 nbq-0 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-9 => PSE port-3 nbq-1 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-10 => PSE port-3 nbq-2 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-11 => PSE port-3 nbq-3 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-12 => PSE port-3 nbq-4 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-13 => PSE port-3 nbq-5 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-14 => PSE port-3 nbq-6 => sp_tag-0x0000 => keep_sptag_hifield-0
*            channel-15 => PSE port-6 nbq-0 => sp_tag-0x0000 => keep_sptag_hifield-0
*********************************************************************************
*/
#ifdef QDMA_LAN
uint MULTICAST_FORCE_PORT[MULTICAST_MAX_CHANNEL] = { ((1<<5)|0), ((1<<5)|1), ((1<<5)|2), ((1<<5)|3), 
													 ((1<<5)|4), ((1<<5)|5), ((0<<5)|0), ((4<<5)|0), 
													 ((3<<5)|0), ((3<<5)|1), ((3<<5)|2), ((3<<5)|3),
													 ((3<<5)|4), ((3<<5)|5), ((2<<5)|0), ((6<<5)|0),
													};
#else
uint MULTICAST_FORCE_PORT[MULTICAST_MAX_CHANNEL] = { ((2<<5)|0), ((2<<5)|1), ((2<<5)|2), ((2<<5)|3), 
													 ((2<<5)|4), ((2<<5)|5), ((2<<5)|6), ((2<<5)|7), 
													 ((2<<5)|8), ((2<<5)|9), ((2<<5)|10), ((2<<5)|11),
													 ((2<<5)|12), ((2<<5)|13), ((2<<5)|14), ((2<<5)|15),
													};
#endif
uint MULTICAST_SPECIAL_TAG[MULTICAST_MAX_CHANNEL] = { 0x0001, 0x0002, 0x0004, 0x0008, 
													  0x0010, 0x0020, 0x0000, 0x0000, 
													  0x0000, 0x0000, 0x0000, 0x0000,
													  0x0000, 0x0000, 0x0000, 0x0000,
													};
uint MULTICAST_KEEP_SPTAG_HIFIELD[MULTICAST_MAX_CHANNEL] = { 1, 1, 1, 1, 1, 1, 0, 0, 
															 0, 0, 0, 0, 0, 0, 0, 0,
															};
#endif

#if SUPPORT_INX_COLOR_RESOLUTION
uint default_mtr_color_cfg =0;
uint default_mtr_color_cfg1 =0;
#endif
/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
int priPktChkLen = DEF_PRIORITY_PKT_CHK_LEN;
module_param(priPktChkLen, int, 0);

#ifdef PURE_BRIDGE
int priPktChk = 0;
#else
int priPktChk = 1;
#endif
module_param(priPktChk, int, 0);


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
static void set_timer_expires(struct timer_list * arg, int time)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
		arg->data = time;
#else
		arg->expires = time;
#endif
}

static int __inline__ qdmaChecConfigDone(uint reg, uint doneBit)
{
	int RETRY = 3 ;
	volatile uint regValue = 0 ;
	
	while(RETRY--) {
		regValue = IO_GREG(reg) ;
		
		if(regValue & doneBit) {
			break ;
		}
		mdelay(1) ;
	}
	if(RETRY < 0) {
		return -ETIME ;
	}

	return 0;
}

/******************************************************************************
******************************************************************************/
int qdmaSetTxQosScheduler(unchar channel, unchar mode, ushort weight[8])
{
	int i = 0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint wrrCfg = 0 ;
	
	for(i=0 ; i<8 ; i++) {
#if !SUPPORT_WRR_WEIGHT_255
		if(weight[i] == 255)
            continue;
#endif
		wrrCfg = (TXWRR_RWCMD | 
				  ((weight[i]<<TXWRR_WRR_VALUE_SHIFT)&TXWRR_WRR_VALUE_MASK) |
				  ((channel<<TXWRR_CHNL_IDX_SHIFT)&TXWRR_CHNL_IDX_MASK) |
				  ((i<<TXWRR_QUEUE_IDX_SHIFT)&TXWRR_QUEUE_IDX_MASK)) ;
		IO_SREG(QDMA_CSR_TXWRR_WEIGHT_CFG(base), wrrCfg) ;
	
		if(qdmaChecConfigDone(QDMA_CSR_TXWRR_WEIGHT_CFG(base), TXWRR_RWCMD_DONE) < 0) {
			QDMA_ERR("Timeout for setting WRR configuration, channel:%d, queue:%d.\n", channel, i) ;
			return -ETIME ;
		}
	}

	qdmaSetPerChnlQosMode(base, channel, mode) ;
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
int qdmaGetTxQosScheduler(unchar channel, unchar *pMode, ushort weight[8])
{
	int i = 0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint wrrCfg = 0 ;
	
	*pMode = qdmaGetPerChnlQosMode(base, channel) ;

	for(i=0 ; i<8 ; i++) {
		wrrCfg = (((channel<<TXWRR_CHNL_IDX_SHIFT)&TXWRR_CHNL_IDX_MASK) |
				  ((i<<TXWRR_QUEUE_IDX_SHIFT)&TXWRR_QUEUE_IDX_MASK)) ;
		IO_SREG(QDMA_CSR_TXWRR_WEIGHT_CFG(base), wrrCfg) ;
	
		if(qdmaChecConfigDone(QDMA_CSR_TXWRR_WEIGHT_CFG(base), TXWRR_RWCMD_DONE) < 0) {
			QDMA_ERR("Timeout for getting WRR configuration, channel:%d, queue:%d.\n", channel, i) ;
			return -ETIME ;
		}
		wrrCfg = IO_GREG(QDMA_CSR_TXWRR_WEIGHT_CFG(base)) ;
		weight[i] =  ((wrrCfg&TXWRR_WRR_VALUE_MASK)>>TXWRR_WRR_VALUE_SHIFT) ;
	}
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int __inline__ generalChecConfigDone(uint reg, uint doneBit)
{
	int RETRY = 3 ;
	volatile uint regValue = 0 ;
	
	while(RETRY--) {
		regValue = IO_GREG(reg) ;
		
		if(regValue & doneBit) {
			break ;
		}
		mdelay(1) ;
	}
	if(RETRY < 0) {
		return -ETIME ;
	}

	return 0;
}

/*return the max bucketSize shift close to value*/
int biSearchGetBucketSizeShift(uint value, uint lo, uint hi, uint unit){

	int mid = 0;
	
	if(lo > hi )
		return -EINVAL;

	if((value > 0) && (value < unit))
		return 0;

	mid = (lo + hi) / 2;

	if((unit<<mid) == value){
		return mid;
	}else if((unit<<lo) == value){
		return lo;
	}else if((unit<<hi) == value){
		return hi;
	}else if((unit<<mid) > value){
		if((mid - lo) <= 1)
			return mid;
		
		return biSearchGetBucketSizeShift(value, lo, mid, unit);
	}else{
		if((hi - mid) <= 1)
			return hi;
		
		return biSearchGetBucketSizeShift(value, mid, hi, unit);
	}
	
}

/*set TrTcm Module Total enable/disable*/
int generalSetTrtcmRateEnable(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_Trtcm_T trtcmEn){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;

	trtcmBase = trtcmCfgBase[trtcmModuleType]; 

	if( trtcmEn == TRTCM_ENABLE ){
		qdmaEnableGeneralTrtcm(trtcmBase);
	}else if( trtcmEn == TRTCM_DISABLE ){
		qdmaDisableGeneralTrtcm(trtcmBase);
	}else{
		return -EINVAL;
	}
	
	return 0;
}

/*Get ratelimit enable/disable*/
int generalGetTrtcmRateEnable(GENERAL_TrtcmModuleType_T trtcmModuleType){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType];

	return qdmaIsGeneralTrtcmEnable(trtcmBase);
}

/*set trtcm mode: TRTCM mode or Ratelimit Mode*/
int generalSetTrtcmMode(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmMode_T trtcmMode){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType];
	
	if( (trtcmMode < TRTCM_RATELIMIT_MODE) || (trtcmMode > TRTCM_MODE) )
		return -EINVAL;
	
	qdmaSetGeneralTrtcmMode(trtcmBase,trtcmMode);

	return 0;
}

/*get trtcm enable/disable*/
int generalGetTrtcmMode(GENERAL_TrtcmModuleType_T trtcmModuleType){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType]; 

	return qdmaGetGeneralTrtcmMode(trtcmBase);	
}

/*set trtcm  ratelimit fasttick*/
int generalSetTrtcmFastTick(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort fastTick){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType]; 
	
	if(fastTick < 1 || fastTick > 0xFFFF){
		printk("Trtcm Fast Tick value should between 1 and 0xFFFF.\n");
		return -EINVAL;
	
	}

	qdmaSetGeneralFastTick(trtcmBase,fastTick) ;
	
	return 0;
}

/*get trtcm  ratelimit fasttick*/
int generalGetTrtcmFastTick(GENERAL_TrtcmModuleType_T trtcmModuleType){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType]; 

	return qdmaGetGeneralFastTick(trtcmBase) ;
}

/*set trtcm  ratelimit slowtick Ratio*/
int generalSetTrtcmSlowTickRatio(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort slowTickRatio){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType]; 
	
	if(slowTickRatio < 1 || slowTickRatio > 0x3FFF){
		printk("Trtcm slow tick ratio value should between 1 and 0x3FFF.\n");
		return -EINVAL;
	}

	qdmaSetGeneralSlowRatio(trtcmBase,slowTickRatio) ;
	
	return 0;
}

/*get trtcm  ratelimit slow tick ratio*/
int generalGetTrtcmSlowTickRatio(GENERAL_TrtcmModuleType_T trtcmModuleType){
	uint trtcmBase =0;

	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType];

	return qdmaGetGeneralSlowRatio(trtcmBase) ;
}

int generalSetTrtcmSlowTick(GENERAL_TrtcmModuleType_T trtcmModuleType, uint slowTick){
	ushort fastTick = 0;
	ushort slowTickRatio = 0;
	fastTick = generalGetTrtcmFastTick(trtcmModuleType);

	if(slowTick < fastTick){
		printk("slow Tick should be larger than fastTick.\n");
		return  -EINVAL;
	}
	
	slowTickRatio = slowTick / fastTick ;

	return  generalSetTrtcmSlowTickRatio(trtcmModuleType, slowTickRatio);
}

uint generalGetTrtcmSlowTick(GENERAL_TrtcmModuleType_T trtcmModuleType){
	ushort fastTick = 0;
	ushort slowTickRatio = 0;
	uint slowTick = 0;
	fastTick = generalGetTrtcmFastTick(trtcmModuleType);
	slowTickRatio = generalGetTrtcmSlowTickRatio(trtcmModuleType);

	slowTick = fastTick * slowTickRatio ;
	return  slowTick;
}

uint generalGetTrtcmTick(GENERAL_TrtcmModuleType_T trtcmModuleType,GENERAL_TrtcmTickSel_T tickSelIdx){
	if( (tickSelIdx < TRTCM_FAST_TICK) || (tickSelIdx > TRTCM_SLOW_TICK) ){
		printk("Trtcm TickSel Index should be 0 or 1.\n");
		return -EINVAL;
	}

	if(tickSelIdx == TRTCM_FAST_TICK){
		return generalGetTrtcmFastTick(trtcmModuleType) ;
	}else{
		return generalGetTrtcmSlowTick(trtcmModuleType);
	}	
}

int generalGetRatelimitParaConfig(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmParaType_T paraType, ushort index, uint *valueLo,uint *valueHi) {
	uint trtcmParaCfg = 0 ;
	uint trtcmBase =0;

	/*get trtcm config addr*/
	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType];

	trtcmParaCfg = (((paraType<<RATELIMIT_PARA_TYPE_SHIFT)&RATELIMIT_PARA_TYPE_MASK) |
				((GET_METER_IDX(index)<<RATELIMIT_PARA_IDX_INDEX_SHIFT)&RATELIMIT_PARA_IDX_INDEX_MASK)) ;

    #if SUPPORT_INGRESS_3X_RATELIMIT
    trtcmParaCfg |= (GET_METER_GRP(index)<<TRTCM_PARA_METER_GROUP_SHIFT);
    #endif

	IO_SREG(TRTCM_PARAM_CFG(trtcmBase), trtcmParaCfg);
	if(generalChecConfigDone(TRTCM_PARAM_CFG(trtcmBase), TRTCM_PARA_RWCMD_DONE) != 0){
		printk("Timeout for Get TRTCM configuration.\n") ;
		return -ETIME ;
	}

	*valueLo = IO_GREG(TRTCM_DATA_LO(trtcmBase));
	*valueHi = IO_GREG(TRTCM_DATA_HI(trtcmBase));
	
	return 0;
}

int generalSetRatelimitParaConfig(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmParaType_T paraType, ushort index, uint valueLo) {
	uint trtcmParaCfg = 0 ;
	uint trtcmBase =0;
	ulong flags=0 ;
	uint valueLo_tmp = 0;
	uint valueHi_tmp = 0;

	/*get trtcm config addr*/
	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType >= TRTCM_MODE_MAX) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType];

	trtcmParaCfg = (RATELIMIT_PARA_RWCMD |
				((paraType<<RATELIMIT_PARA_TYPE_SHIFT)&RATELIMIT_PARA_TYPE_MASK) |
				((GET_METER_IDX(index)<<RATELIMIT_PARA_IDX_INDEX_SHIFT)&RATELIMIT_PARA_IDX_INDEX_MASK)) ;

    #if SUPPORT_INGRESS_3X_RATELIMIT
    trtcmParaCfg |= (GET_METER_GRP(index)<<TRTCM_PARA_METER_GROUP_SHIFT);
    #endif
	
	do{	
		spin_lock_irqsave(&qdma_config_lock, flags) ;
		IO_SREG(TRTCM_DATA_LO(trtcmBase), valueLo);
#ifdef TCSUPPORT_CPU_ARMV8
		wmb();
#endif
		IO_SREG(TRTCM_PARAM_CFG(trtcmBase), trtcmParaCfg) ;
		spin_unlock_irqrestore(&qdma_config_lock, flags) ;
	
		generalGetRatelimitParaConfig(trtcmModuleType, paraType, index, &valueLo_tmp, &valueHi_tmp);
	}while(valueLo != valueLo_tmp);
	
	if( generalChecConfigDone(TRTCM_PARAM_CFG(trtcmBase), TRTCM_PARA_RWCMD_DONE) != 0) {
		printk("Timeout for set TRTCM configuration.\n") ;
		return -ETIME ;
	}

	return 0;
}

/*set chnl/ring/flow/etc trtcm enable/disable*/
int generalSetRatelimitMeterMode(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmMeter_T meterMode, ushort index){
	uint valueLo = 0, valueHi = 0;

	if( (meterMode < GENERAL_METER_DISABLE) || (meterMode > GENERAL_METER_ENABLE)){
		printk("Trtcm Meter Mode should be 0 or 1.\n");
		return -EINVAL;
	}

	if(generalGetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, &valueLo, &valueHi) < 0){
		printk("Get TRTCM Para Config Failed.\n");
		return -EFAULT;
	}

	valueLo = (meterMode == GENERAL_METER_ENABLE) ? (valueLo|TRTCM_METER_MODE):(valueLo &(~TRTCM_METER_MODE)) ;
	
	if(generalSetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, valueLo) < 0){
		printk("Set TRTCM Para Config : Ratelimit Meter Enable Failed.\n");
		return -EFAULT;	
	}

	return 0;
}

/*get chnl/ring/flow/etc ratelimit enable/disable*/
int generalGetRatelimitMeterMode(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort index){

	uint valueLo = 0, valueHi = 0;

	generalGetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, &valueLo, &valueHi);

	return  ((valueLo & TRTCM_METER_MODE) >> 2);
}

int generalSetRatelimitPktMode(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmPktMode_T pktMode, ushort index){
	uint valueLo = 0, valueHi = 0;

	if( (pktMode < TRTCM_BYTE_MODE) || (pktMode > TRTCM_PACKET_MODE) ){
		printk("Trtcm Packet Mode should be 0 or 1.\n");
		return -EINVAL;
	}

	if(generalGetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, &valueLo, &valueHi) < 0){
		printk("Get TRTCM Para Config Failed.\n");
		return -EFAULT;
	}

	valueLo = (pktMode == TRTCM_PACKET_MODE) ? (valueLo|TRTCM_PKT_MODE):(valueLo &(~TRTCM_PKT_MODE)) ;
	
	if(generalSetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, valueLo) < 0){
		printk("Set TRTCM Para Config : Ratelimit Packet Enable Failed.\n");
		return -EFAULT;	
	}
		
	return 0;
}

int generalGetRatelimitPktMode(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort index){

	uint valueLo = 0, valueHi = 0;

	generalGetRatelimitParaConfig(trtcmModuleType,TRTCM_MISC, index, &valueLo, &valueHi);

	return  ((valueLo & TRTCM_PKT_MODE) >> 1);
}

int generalSetRatelimitTickSel(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmTickSel_T tickSel, ushort index){
	uint valueLo = 0, valueHi = 0;

	if( (tickSel < TRTCM_FAST_TICK) || (tickSel > TRTCM_SLOW_TICK) ){
		printk("Trtcm TickSel Index should be 0 or 1.\n");
		return -EINVAL;
	}

	if(generalGetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, &valueLo, &valueHi) < 0){
		printk("Get TRTCM Para Config Failed.\n");
		return -EFAULT;
	}

	valueLo = (tickSel == TRTCM_SLOW_TICK) ? (valueLo|TRTCM_TICK_SEL):(valueLo &(~TRTCM_TICK_SEL)) ;
			
	if(generalSetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, valueLo) < 0){
		printk("Set TRTCM Para Config : Ratelimit Tick Sel Failed.\n");
		return -EFAULT;	
	}

	return 0;
}

int generalGetRatelimitTickSel(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort index){

	uint valueLo = 0, valueHi = 0;

	generalGetRatelimitParaConfig(trtcmModuleType, TRTCM_MISC, index, &valueLo, &valueHi);

	return  (valueLo & TRTCM_TICK_SEL);
}

int generalGetBucketSizeByRate(uint rateLimitValue)
{
    int trtcm_rate_shift = 0;
	int bucksize = 0;

	if(rateLimitValue <= (METER_1K<<2))
	{
		bucksize = (64<<10);
	}
	else if(rateLimitValue <= (METER_1K<<3))
	{
		bucksize = (512<<10);
	}
	else if(rateLimitValue <= (METER_1K<<4))
	{
		bucksize = (METER_1M);
	}
	else if(rateLimitValue <= (METER_1K<<7))
	{
		bucksize = (METER_1M * 8);
	}
	else if(rateLimitValue <= (METER_1K<<8))
	{
		bucksize = (METER_1M * 24);
	}
	else
	{
		bucksize = (METER_1M * 64);
	}

	return bucksize;
}

/*
 tokenRate_integer = ratelimitvalue /unit 
 tokenRate_fraction =( ratelimtvalue % unit )
so should make sure the 2 params valid.
*/
int generalSetRatelimitTokenRate(GENERAL_TrtcmModuleType_T trtcmModuleType, uint rateLimitValue, ushort index){
	int bucketSize = 0;
	uint tokenRate = 0, tokenRate_integer = 0;
	ushort tokenRate_fraction = 0;
	int curTicksel = 0;
	ushort rateLimitUnit = 0 ;

	/*1. get tick sel*/
	if( TRTCM_FAST_TICK == generalGetRatelimitTickSel(trtcmModuleType, index) )/*fast tick mode*/
		curTicksel = generalGetTrtcmFastTick(trtcmModuleType) ;
	else/*slow tick mode*/
		curTicksel = generalGetTrtcmSlowTick(trtcmModuleType) ;

	/*2. get ratelimit mode, calculate Unit*/
	if(0 == curTicksel)
	{
		printk("tick = 0 , set error.\n");
		return -EINVAL;
	}
	else
	{
		if( TRTCM_BYTE_MODE == generalGetRatelimitPktMode(trtcmModuleType, index) )
			rateLimitUnit = 8000 / curTicksel ;  	/** 8bits X 1000 / (curTicksel X 10e-6 s)  kbps **/
		else
			rateLimitUnit = 1000000 / curTicksel ;  /** 1 / (curTicksel X 10e-6 s) pps **/
	}

	if( 0 == rateLimitUnit )
	{
		printk("rateLimitUnit = 0 , set error.\n");
		return -EINVAL;
	}

	/*3. calculate tokenRate*/
	tokenRate_integer = rateLimitValue / rateLimitUnit ;
	tokenRate_fraction =( rateLimitValue % rateLimitUnit) * 64 / rateLimitUnit ;

	if( (tokenRate_integer > 0x3FFFF) || (tokenRate_fraction > 0x3F) ){
		printk("tokenRate overflow.\n");
		return -EINVAL;
	}

	tokenRate = (tokenRate_integer << TRTCM_TOKEN_RATE_INTEGER_SHIFT) | tokenRate_fraction;
	
	if(generalSetRatelimitParaConfig(trtcmModuleType, TRTCM_TOKEN_RATE, index, tokenRate) < 0){
		printk("Set TRTCM Token Rate Failed.\n");
		return -EFAULT;
	}

	/*4. calculate bucketSize*/
	/*Egress & Global: bucketsize should be small to prevent burst*/
	/*Ingress: bucketsize should be bigger for TCP flow*/
	if(trtcmModuleType == INGRESS_TRTCM){
		if( TRTCM_BYTE_MODE == generalGetRatelimitPktMode(trtcmModuleType, index) )
			bucketSize = generalGetBucketSizeByRate(rateLimitValue);
		else
			bucketSize = rateLimitValue<<RATELIMIT_PKT_MODE_BUCKET_SHIFT;
	}else{
		bucketSize = tokenRate_integer+1;
		if(bucketSize < 4096)/* if bucketsize is lower than 4096, 1518Byte flow will lead to bucket overflow */
			bucketSize = 4096;
	}
    
	return bucketSize;
}

uint generalGetRatelimitTokenRate(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort index){
	uint tokenRate_integer = 0 , tokenRate_fraction = 0;
	uint valueLo = 0 , valueHi = 0;
	int curTicksel = 0;
	ushort rateLimitUnit = 0 ;

	/*1. get tick sel*/
	if( TRTCM_FAST_TICK == generalGetRatelimitTickSel(trtcmModuleType, index) )/*fast tick mode*/
		curTicksel = generalGetTrtcmFastTick(trtcmModuleType) ;
	else/*slow tick mode*/
		curTicksel = generalGetTrtcmSlowTick(trtcmModuleType) ;

	/*2. get ratelimit mode, calculate Unit*/
	if(0 == curTicksel)
	{
		printk("tick = 0 , get error.\n");
		return -EINVAL;
	}
	else
	{
		if( TRTCM_BYTE_MODE == generalGetRatelimitPktMode(trtcmModuleType, index) )
			rateLimitUnit = 8000 / curTicksel ; 	/** 8bits X 1000 / (curTicksel X 10e-6 s)  kbps **/
		else
			rateLimitUnit = 1000000 / curTicksel ;	/** 1 / (curTicksel X 10e-6 s) pps **/
	}

	if( 0 == rateLimitUnit )
	{
		printk("rateLimitUnit = 0 , get error.\n");
		return -EINVAL;
	}

	/*3.calculate ratelimit value*/
	generalGetRatelimitParaConfig(trtcmModuleType, TRTCM_TOKEN_RATE, index, &valueLo, &valueHi);
	tokenRate_integer = (valueLo &  TRTCM_TOKEN_RATE_INTEGER_MASK ) >> TRTCM_TOKEN_RATE_INTEGER_SHIFT;
	tokenRate_fraction = (valueLo & TRTCM_TOKEN_RATE_FRACTION_MASK );

	return	( (tokenRate_integer*rateLimitUnit) + (tokenRate_fraction*rateLimitUnit/64) );
}

int generalSetRatelimitBucketSize(GENERAL_TrtcmModuleType_T trtcmModuleType, uint bucketSize, ushort index){
	int bucketSize_shift = 0;
    uint bucketUnit = 0;

    /*max: 128Mbyte*/
	if(bucketSize > 0x8000000){
		return -EINVAL;	
	}

	if (TRTCM_BYTE_MODE == generalGetRatelimitPktMode(trtcmModuleType, index)){
		bucketUnit = trtcmBucketByteUnit[trtcmModuleType];
	}else{
		bucketUnit = trtcmBucketPacketUnit[trtcmModuleType];
	}

#if SUPPORT_BUCKETSIZE_BITWIDTH_17
	bucketSize_shift = biSearchGetBucketSizeShift(bucketSize, 0, 17, bucketUnit);
#else
	bucketSize_shift = biSearchGetBucketSizeShift(bucketSize, 0, 15, bucketUnit);
#endif
	
	if(generalSetRatelimitParaConfig(trtcmModuleType, TRTCM_BUCKETSIZE_SHIFT, index, bucketSize_shift) < 0){
		printk("Set TRTCM Bucket Size Failed.\n");
		return -EFAULT;
	}
	
	return 0;
}

uint generalGetRatelimitBucketSize(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort index){
	uint valueLo = 0, valueHi = 0;
	unchar bucketSize_shift = 0;
	uint bucketSize = 0;
    uint bucketUnit = 0;

    if (TRTCM_BYTE_MODE == generalGetRatelimitPktMode(trtcmModuleType, index)){
		bucketUnit = trtcmBucketByteUnit[trtcmModuleType];
	}else{
		bucketUnit = trtcmBucketPacketUnit[trtcmModuleType];
	}
    
	generalGetRatelimitParaConfig(trtcmModuleType, TRTCM_BUCKETSIZE_SHIFT, index, &valueLo, &valueHi);

	bucketSize_shift = valueLo & TRTCM_BUCKET_SIZE_SHIFT_MASK ;
	bucketSize = bucketUnit << bucketSize_shift ;
	
	return bucketSize;
}

int generalGetRatelimitBucketCntr(GENERAL_TrtcmModuleType_T trtcmModuleType, ushort index){

	uint valueLo = 0, valueHi = 0;
	uint bucketCntr = 0;
	uint bucketCntr_integer = 0;
	unchar bucketCntr_fraction = 0 ;
	short bucketCntr_signBit = 0;
	
	generalGetRatelimitParaConfig(trtcmModuleType,TRTCM_BUCKET_COUNTER, index, &valueLo, &valueHi);

	bucketCntr_fraction = valueLo & TRTCM_BUCKET_COUNTER_FRACTION_MASK ;
	bucketCntr_integer =((valueLo & TRTCM_BUCKET_COUNTER_LO_MASK) >> TRTCM_BUCKET_COUNTER_INTEGER_SHIFT) |
							((valueHi & TRTCM_BUCKET_COUNTER_HI_MASK) << TRTCM_BUCKET_COUNTER_INTEGER_FIN_SHIFT) ;

	bucketCntr_signBit = (valueHi & TRTCM_BUCKET_COUNTER_SIGN_BIT_HI_MASK) ? 1: 0 ;

	/*need to add , for use bucker_counter*/
	return 0;
}

int generalGetTrtcmParaConfig(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmParaType_T paraType, GENERAL_TrtcmRateType_T rateType, ushort index, uint *valueLo,uint *valueHi) {
	uint trtcmParaCfg = 0 ;
	uint trtcmBase =0;

	/*get trtcm config addr*/
	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType > EGRESS_TRTCM) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType];

	trtcmParaCfg = (((paraType<<TRTCM_PARA_TYPE_SHIFT)&TRTCM_PARA_TYPE_MASK) |
				((GET_METER_IDX(index)<<TRTCM_PARA_IDX_INDEX_SHIFT)&TRTCM_PARA_IDX_INDEX_MASK) |
				((rateType<<TRTCM_PARA_IDX_RATE_TYPE_SHIFT)&TRTCM_PARA_IDX_RATE_TYPE_MASK)) ;

    #if SUPPORT_INGRESS_3X_RATELIMIT
    trtcmParaCfg |= (GET_METER_GRP(index)<<TRTCM_PARA_METER_GROUP_SHIFT);
    #endif

	IO_SREG(TRTCM_PARAM_CFG(trtcmBase), trtcmParaCfg);
	if(generalChecConfigDone(TRTCM_PARAM_CFG(trtcmBase), TRTCM_PARA_RWCMD_DONE) != 0){
		printk("Timeout for Get TRTCM configuration.\n") ;
		return -ETIME ;
	}

	*valueLo = IO_GREG(TRTCM_DATA_LO(trtcmBase));
	*valueHi = IO_GREG(TRTCM_DATA_HI(trtcmBase));
	
	return 0;
}

int generalSetTrtcmParaConfig(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmParaType_T paraType, GENERAL_TrtcmRateType_T rateType, ushort index, uint valueLo) {
	uint trtcmParaCfg = 0 ;
	uint trtcmBase =0;
	ulong flags=0 ;
	uint valueLo_tmp = 0;
	uint valueHi_tmp = 0;
	/*get trtcm config addr*/
	if( (trtcmModuleType < INGRESS_TRTCM) || (trtcmModuleType > EGRESS_TRTCM) )
		return -EINVAL;
	trtcmBase = trtcmCfgBase[trtcmModuleType];

	trtcmParaCfg = (TRTCM_PARA_RWCMD |
				((paraType<<TRTCM_PARA_TYPE_SHIFT)&TRTCM_PARA_TYPE_MASK) |
				((GET_METER_IDX(index)<<TRTCM_PARA_IDX_INDEX_SHIFT)&TRTCM_PARA_IDX_INDEX_MASK) |
				((rateType<<TRTCM_PARA_IDX_RATE_TYPE_SHIFT)&TRTCM_PARA_IDX_RATE_TYPE_MASK)) ;
    
    #if SUPPORT_INGRESS_3X_RATELIMIT
    trtcmParaCfg |= (GET_METER_GRP(index)<<TRTCM_PARA_METER_GROUP_SHIFT);
    #endif
	
	do{	
		spin_lock_irqsave(&qdma_config_lock, flags) ;
		IO_SREG(TRTCM_DATA_LO(trtcmBase), valueLo);
#ifdef TCSUPPORT_CPU_ARMV8
		wmb();
#endif
		IO_SREG(TRTCM_PARAM_CFG(trtcmBase), trtcmParaCfg) ;
		spin_unlock_irqrestore(&qdma_config_lock, flags) ;
		generalGetTrtcmParaConfig(trtcmModuleType, paraType, rateType, index, &valueLo_tmp, &valueHi_tmp);
	}while(valueLo != valueLo_tmp);
	
	if( generalChecConfigDone(TRTCM_PARAM_CFG(trtcmBase), TRTCM_PARA_RWCMD_DONE) != 0) {
		printk("Timeout for set TRTCM configuration.\n") ;
		return -ETIME ;
	}

	return 0;
}

/*set chnl/ring/flow/etc trtcm enable/disable*/
int generalSetTrtcmMeterMode(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmMeter_T meterMode, GENERAL_TrtcmRateType_T rateType, ushort index){
	uint valueLo = 0, valueHi = 0;

	if( (meterMode < GENERAL_METER_DISABLE) || (meterMode > GENERAL_METER_ENABLE)){
		printk("Trtcm Meter Mode should be 0 or 1.\n");
		return -EINVAL;
	}

	if(generalGetTrtcmParaConfig(trtcmModuleType, TRTCM_MISC, rateType, index, &valueLo,&valueHi) < 0){
		printk("Get TRTCM Para Config Failed.\n");
		return -EFAULT;
	}

	valueLo = (meterMode == GENERAL_METER_ENABLE) ? (valueLo|TRTCM_METER_MODE):(valueLo &(~TRTCM_METER_MODE)) ;
	
	if(generalSetTrtcmParaConfig(trtcmModuleType, TRTCM_MISC, rateType, index, valueLo) < 0){
		printk("Set TRTCM Para Config : %s Meter Enable Failed.\n",rateType ? "PIR" : "CIR");
		return -EFAULT;	
	}

	return 0;
}

/*get chnl/ring/flow/etc ratelimit enable/disable*/
int generalGetTrtcmMeterMode(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmRateType_T rateType, ushort index){

	uint valueLo = 0, valueHi = 0;

	generalGetTrtcmParaConfig(trtcmModuleType,TRTCM_MISC, rateType, index, &valueLo, &valueHi);

	return  ((valueLo & TRTCM_METER_MODE) >> 2);
}

int generalSetTrtcmPktMode(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmPktMode_T pktMode, GENERAL_TrtcmRateType_T rateType, ushort index){
	uint valueLo = 0, valueHi = 0;

	if( (pktMode < TRTCM_BYTE_MODE) || (pktMode > TRTCM_PACKET_MODE) ){
		printk("Trtcm Packet Mode should be 0 or 1.\n");
		return -EINVAL;
	}

	if(generalGetTrtcmParaConfig(trtcmModuleType, TRTCM_MISC, rateType, index, &valueLo,&valueHi) < 0){
		printk("Get TRTCM Para Config Failed.\n");
		return -EFAULT;
	}

	valueLo = (pktMode == TRTCM_PACKET_MODE) ? (valueLo|TRTCM_PKT_MODE):(valueLo &(~TRTCM_PKT_MODE)) ;
	
	if(generalSetTrtcmParaConfig(trtcmModuleType, TRTCM_MISC, rateType, index, valueLo) < 0){
		printk("Set TRTCM Para Config : %s Packet Enable Failed.\n",rateType ? "PIR" : "CIR");
		return -EFAULT;	
	}
		
	return 0;
}

int generalGetTrtcmPktMode(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmRateType_T rateType, ushort index){

	uint valueLo = 0, valueHi = 0;

	generalGetTrtcmParaConfig(trtcmModuleType,TRTCM_MISC, rateType, index, &valueLo, &valueHi);

	return  ((valueLo & TRTCM_PKT_MODE) >> 1);
}

int generalSetTrtcmTickSel(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmTickSel_T tickSel, GENERAL_TrtcmRateType_T rateType, ushort index){
	uint valueLo = 0, valueHi = 0;

	if( (tickSel < TRTCM_FAST_TICK) || (tickSel > TRTCM_SLOW_TICK) ){
		printk("Trtcm TickSel Index should be 0 or 1.\n");
		return -EINVAL;
	}

	if(generalGetTrtcmParaConfig(trtcmModuleType, TRTCM_MISC, rateType, index, &valueLo,&valueHi) < 0){
		printk("Get TRTCM Para Config Failed.\n");
		return -EFAULT;
	}

	valueLo = (tickSel == TRTCM_SLOW_TICK) ? (valueLo|TRTCM_TICK_SEL):(valueLo &(~TRTCM_TICK_SEL)) ;
			
	if(generalSetTrtcmParaConfig(trtcmModuleType, TRTCM_MISC, rateType, index, valueLo) < 0){
		printk("Set TRTCM Para Config : %s Tick Sel Failed.\n",rateType ? "PIR" : "CIR");
		return -EFAULT;	
	}

	return 0;

}

int generalGetTrtcmTickSel(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmRateType_T rateType, ushort index){

	uint valueLo = 0, valueHi = 0;

	generalGetTrtcmParaConfig(trtcmModuleType,TRTCM_MISC, rateType, index, &valueLo, &valueHi);

	return  (valueLo & TRTCM_TICK_SEL);
}

/*
 tokenRate_integer = ratelimitvalue /unit 
 tokenRate_fraction =( ratelimtvalue % unit )
so should make sure the 2 params valid.
*/
int generalSetTrtcmTokenRate(GENERAL_TrtcmModuleType_T trtcmModuleType, uint rateLimitValue, GENERAL_TrtcmRateType_T rateType, ushort index){
	int bucketSize = 0;
	uint tokenRate = 0, tokenRate_integer = 0;
	ushort tokenRate_fraction = 0;
	int curTicksel = 0;
	ushort rateLimitUnit = 0 ;

	/*1. get tick sel*/
	if( TRTCM_FAST_TICK == generalGetTrtcmTickSel(trtcmModuleType, rateType, index) )/*fast tick mode*/
		curTicksel = generalGetTrtcmFastTick(trtcmModuleType) ;
	else/*slow tick mode*/
		curTicksel = generalGetTrtcmSlowTick(trtcmModuleType) ;

	/*2. get ratelimit mode, calculate Unit*/
	if(0 == curTicksel)
	{
		printk("tick = 0 , set error.\n");
		return -EINVAL;
	}
	else
	{
		if( TRTCM_BYTE_MODE == generalGetTrtcmPktMode(trtcmModuleType, rateType, index) )
			rateLimitUnit = 8000 / curTicksel ;  	/** 8bits X 1000 / (curTicksel X 10e-6 s)  kbps **/
		else
			rateLimitUnit = 1000000 / curTicksel ;  /** 1 / (curTicksel X 10e-6 s) pps **/
	}

	if( 0 == rateLimitUnit )
	{
		printk("rateLimitUnit = 0 , set error.\n");
		return -EINVAL;
	}

	/*3. calculate tokenRate*/
	tokenRate_integer = rateLimitValue / rateLimitUnit ;
	tokenRate_fraction =( rateLimitValue % rateLimitUnit) * 64 / rateLimitUnit ;

	if( (tokenRate_integer > 0x3FFFF) || (tokenRate_fraction > 0x3F) ){
		printk("tokenRate overflow.\n");
		return -EINVAL;
	}

	tokenRate = (tokenRate_integer << TRTCM_TOKEN_RATE_INTEGER_SHIFT) | tokenRate_fraction;

	if(generalSetTrtcmParaConfig(trtcmModuleType, TRTCM_TOKEN_RATE, rateType, index, tokenRate) < 0){
		printk("Set TRTCM Token Rate Failed.\n");
		return -EFAULT;
	}

	/*4. calculate bucketSize*/
	/*Egress & Global & SLA: bucketsize should be small to prevent burst*/
	/*Ingress: bucketsize should be bigger for TCP flow*/
	if(trtcmModuleType == INGRESS_TRTCM){
		if( TRTCM_BYTE_MODE == generalGetTrtcmPktMode(trtcmModuleType, rateType, index) )
			bucketSize = generalGetBucketSizeByRate(rateLimitValue);
		else
			bucketSize = rateLimitValue<<RATELIMIT_PKT_MODE_BUCKET_SHIFT;
	}else{
		bucketSize = tokenRate_integer+1;
		if(bucketSize < 4096)/* if bucketsize is lower than 4096, 1518Byte flow will lead to bucket overflow */
			bucketSize = 4096;
	}
    
	return bucketSize;

}

uint generalGetTrtcmTokenRate(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmRateType_T rateType, ushort index){
	uint tokenRate_integer = 0 , tokenRate_fraction = 0;
	uint valueLo = 0 , valueHi = 0;
	int curTicksel = 0;
	ushort rateLimitUnit = 0 ;

	/*1. get tick sel*/
	if( TRTCM_FAST_TICK == generalGetTrtcmTickSel(trtcmModuleType, rateType, index) )/*fast tick mode*/
		curTicksel = generalGetTrtcmFastTick(trtcmModuleType) ;
	else/*slow tick mode*/
		curTicksel = generalGetTrtcmSlowTick(trtcmModuleType) ;

	/*2. get ratelimit mode, calculate Unit*/
	if(0 == curTicksel)
	{
		printk("tick = 0 , get error.\n");
		return -EINVAL;
	}
	else
	{
		if( TRTCM_BYTE_MODE == generalGetTrtcmPktMode(trtcmModuleType, rateType, index) )
			rateLimitUnit = 8000 / curTicksel ; 	/** 8bits X 1000 / (curTicksel X 10e-6 s)  kbps **/
		else
			rateLimitUnit = 1000000 / curTicksel ;	/** 1 / (curTicksel X 10e-6 s) pps **/
	}

	if( 0 == rateLimitUnit )
	{
		printk("rateLimitUnit = 0 , get error.\n");
		return -EINVAL;
	}

	/*3.calculate ratelimit value*/
	generalGetTrtcmParaConfig(trtcmModuleType, TRTCM_TOKEN_RATE, rateType, index, &valueLo, &valueHi);
	tokenRate_integer = (valueLo &  TRTCM_TOKEN_RATE_INTEGER_MASK ) >> TRTCM_TOKEN_RATE_INTEGER_SHIFT;
	tokenRate_fraction = (valueLo & TRTCM_TOKEN_RATE_FRACTION_MASK );

	return	( (tokenRate_integer*rateLimitUnit) + (tokenRate_fraction*rateLimitUnit/64) );

}

int generalSetTrtcmBucketSize(GENERAL_TrtcmModuleType_T trtcmModuleType, uint bucketSize, GENERAL_TrtcmRateType_T rateType, ushort index){
	int bucketSize_shift = 0;
	uint bucketUnit = 0;

	/*max: 128Mbyte*/
	if(bucketSize > 0x8000000){
		return -EINVAL;	
	}

	if (TRTCM_BYTE_MODE == generalGetTrtcmPktMode(trtcmModuleType, rateType, index)){
		bucketUnit = trtcmBucketByteUnit[trtcmModuleType];
	}else{
		bucketUnit = trtcmBucketPacketUnit[trtcmModuleType];
	}

#if SUPPORT_BUCKETSIZE_BITWIDTH_17
	bucketSize_shift = biSearchGetBucketSizeShift(bucketSize, 0, 17, bucketUnit);
#else
	bucketSize_shift = biSearchGetBucketSizeShift(bucketSize, 0, 15, bucketUnit);
#endif
	
	if(generalSetTrtcmParaConfig(trtcmModuleType, TRTCM_BUCKETSIZE_SHIFT, rateType, index, bucketSize_shift) < 0){
		printk("Set TRTCM Bucket Size Failed.\n");
		return -EFAULT;
	}
	
	return 0;
}

uint generalGetTrtcmBucketSize(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmRateType_T rateType, ushort index){
	uint valueLo = 0, valueHi = 0;
	unchar bucketSize_shift = 0;
	uint bucketSize = 0;
    uint bucketUnit = 0;

    if (TRTCM_BYTE_MODE == generalGetTrtcmPktMode(trtcmModuleType, rateType, index)){
		bucketUnit = trtcmBucketByteUnit[trtcmModuleType];
	}else{
		bucketUnit = trtcmBucketPacketUnit[trtcmModuleType];
	}
    
	generalGetTrtcmParaConfig(trtcmModuleType, TRTCM_BUCKETSIZE_SHIFT, rateType, index, &valueLo, &valueHi);

	bucketSize_shift = valueLo & TRTCM_BUCKET_SIZE_SHIFT_MASK ;
	bucketSize = bucketUnit << bucketSize_shift ;
    
	return bucketSize;
}

int generalGetTrtcmBucketCntr(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmRateType_T rateType, ushort index){

	uint valueLo = 0, valueHi = 0;
	uint bucketCntr = 0;
	uint bucketCntr_integer = 0;
	unchar bucketCntr_fraction = 0 ;
	short bucketCntr_signBit = 0;
	
	generalGetTrtcmParaConfig(trtcmModuleType,TRTCM_BUCKET_COUNTER, rateType, index, &valueLo, &valueHi);

	bucketCntr_fraction = valueLo & TRTCM_BUCKET_COUNTER_FRACTION_MASK ;
	bucketCntr_integer =((valueLo & TRTCM_BUCKET_COUNTER_LO_MASK) >> TRTCM_BUCKET_COUNTER_INTEGER_SHIFT) |
							((valueHi & TRTCM_BUCKET_COUNTER_HI_MASK) << TRTCM_BUCKET_COUNTER_INTEGER_FIN_SHIFT) ;

	bucketCntr_signBit = (valueHi & TRTCM_BUCKET_COUNTER_SIGN_BIT_HI_MASK) ? 1: 0 ;

	/*need to add , for use bucker_counter*/
	return 0;
}

#if SUPPORT_MTR_ACNT_IN_QDMA
/*old APIs*/
int qdmaSetRxRateLimitConfig(unchar ringIdx, unchar type, ushort value){
	return 0 ;
}
int qdmaGetRxRateLimitConfig(unchar ringIdx, unchar type){
	return 0 ;
}
int qdmaSetTxRateLimitConfig(unchar chnlIdx, unchar type, ushort value){
	return 0 ;
}
int qdmaGetTxRateLimitConfig(unchar chnlIdx, unchar type){
	return 0 ;
}
int qdmaGetLimitRateMax(int ticksel_idx){
	return 0 ;
}
int qdmaGetLimitRateMaxChnl(int ticksel_idx, int channel){
	return 0 ;
}
int qdmaUpdateAllTxRateLimitValue(ushort curUnit, ushort newUnit, int ticksel_idx){
	return 0 ;
}
int qdmaUpdateAllRxRateLimitValue(ushort curUnit, ushort newUnit, unchar ticker){
	return 0 ;
}

/*EN7580: reconsitution the function*/
void qdmaSetDynCngstDeiThrhScale(uint base, QDMA_TxDynCngstDeiThrhScale_t val){
    struct ECNT_QDMA_Data qdma_data;
    QDMA_TxWredCfg_T txWredCfg;
    unchar thrValue = 0;
    unchar i = 0;

    qdma_data.qdma_private.txWredCfgPtr = &txWredCfg ;

    txWredCfg.txWredPacketType = DEI_PACKET;
    if(val == QDMA_DYNCNGST_DEI_THRH_SCALE_1_2){
        thrValue = 4; /*4/8*/
    }else if(val == QDMA_DYNCNGST_DEI_THRH_SCALE_1_4){
        thrValue = 2; /*2/8*/
    }else if(val == QDMA_DYNCNGST_DEI_THRH_SCALE_1_8 || val == QDMA_DYNCNGST_DEI_THRH_SCALE_1_16){
        thrValue = 1; /*1/8*/
    }else{
        QDMA_ERR("Fault: wrong dei threshold scale !\n");
        return;
    }
    for(i=0; i < TX_WRED_THR_NUM ; i++){
        txWredCfg.txWredThrValue[i] = thrValue;
    }

    qdma_set_tx_wred_threshold(&qdma_data);
}
QDMA_TxDynCngstDeiThrhScale_t qdmaGetDynCngstDeiThrhScale(uint base){
    struct ECNT_QDMA_Data qdma_data;
    QDMA_TxWredCfg_T txWredCfg;
    qdma_data.qdma_private.txWredCfgPtr = &txWredCfg ;

    txWredCfg.txWredPacketType = DEI_PACKET;
    qdma_get_tx_wred_threshold(&qdma_data);

    if(txWredCfg.txWredThrValue[0] == 1)
        return QDMA_DYNCNGST_DEI_THRH_SCALE_1_8;
    else if(txWredCfg.txWredThrValue[0] == 2)
        return QDMA_DYNCNGST_DEI_THRH_SCALE_1_4;
    else if(txWredCfg.txWredThrValue[0] == 4)
        return QDMA_DYNCNGST_DEI_THRH_SCALE_1_2;
    else
        return QDMA_DYNCNGST_DEI_THRH_SCALE_ITEMS;
}
void qdmaSetTxPeekRateMargin(uint base, QDMA_PeekRateMargin_t val){
    struct ECNT_QDMA_Data qdma_data;
    QDMA_TxWredCfg_T txWredCfg;
    unchar thrValue = 0;
    unchar i = 0;

    qdma_data.qdma_private.txWredCfgPtr = &txWredCfg ;

    txWredCfg.txWredPacketType = NORMAL_PACKET;
    if(val == QDMA_TXCNGST_PEEKRATE_MARGIN_0){
        thrValue = 8; /*8/8*/
    }else if(val == QDMA_TXCNGST_PEEKRATE_MARGIN_25){
        thrValue = 10; /*10/8*/
    }else if(val == QDMA_TXCNGST_PEEKRATE_MARGIN_50){
        thrValue = 12; /*12/8*/
    }else if(val == QDMA_TXCNGST_PEEKRATE_MARGIN_100){
        thrValue = 16; /*16/8*/
    }else{
        QDMA_ERR("Fault: wrong dei threshold scale !\n");
        return;
    }
    for(i=0; i < TX_WRED_THR_NUM ; i++){
        txWredCfg.txWredThrValue[i] = thrValue;
    }

    qdma_set_tx_wred_threshold(&qdma_data);
}
QDMA_PeekRateMargin_t qdmaGetTxPeekRateMargin(uint base){
    struct ECNT_QDMA_Data qdma_data;
    QDMA_TxWredCfg_T txWredCfg;
    qdma_data.qdma_private.txWredCfgPtr = &txWredCfg ;

    txWredCfg.txWredPacketType = NORMAL_PACKET;
    qdma_get_tx_wred_threshold(&qdma_data);

    if(txWredCfg.txWredThrValue[0] == 8)
        return QDMA_TXCNGST_PEEKRATE_MARGIN_0;
    else if(txWredCfg.txWredThrValue[0] == 10)
        return QDMA_TXCNGST_PEEKRATE_MARGIN_25;
    else if(txWredCfg.txWredThrValue[0] == 12)
        return QDMA_TXCNGST_PEEKRATE_MARGIN_50;
    else if(txWredCfg.txWredThrValue[0] == 16)
        return QDMA_TXCNGST_PEEKRATE_MARGIN_100;
    else
        return QDMA_TXCNGST_PEEKRATE_MARGIN_0;
}
#else
int qdmaSetRxRateLimitConfig(unchar ringIdx, unchar type, ushort value) 
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint rxRateLimitCfg = 0 ;
	
	rxRateLimitCfg = (RX_RATELIMIT_RWCMD | 
					((value<<RX_RATELIMIT_VALUE_SHIFT)&RX_RATELIMIT_VALUE_MASK) |
					RX_RATELIMIT_RING_IDX(ringIdx) |
					(type<<RX_RATELIMIT_TYPE_SHIFT)) ;
	QDMA_MSG(DBG_MSG, "rxRateLimitCfg is 0x%08x.\n", rxRateLimitCfg) ;
	IO_SREG(QDMA_CSR_RX_RATELIMIT_CFG(base), rxRateLimitCfg) ;

	if(qdmaChecConfigDone(QDMA_CSR_RX_RATELIMIT_CFG(base), RX_RATELIMIT_RWCMD_DONE) < 0) {
		QDMA_ERR("Timeout for set Rx Rate Limit configuration.\n") ;
		return -ETIME ;
	}
	
	return 0 ;
}

int qdmaGetRxRateLimitConfig(unchar ringIdx, unchar type)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint rxRateLimitCfg = 0 ;
	
	rxRateLimitCfg = (RX_RATELIMIT_RING_IDX(ringIdx) |
					(type<<RX_RATELIMIT_TYPE_SHIFT)) ;
	IO_SREG(QDMA_CSR_RX_RATELIMIT_CFG(base), rxRateLimitCfg) ;
	
	if(qdmaChecConfigDone(QDMA_CSR_RX_RATELIMIT_CFG(base), RX_RATELIMIT_RWCMD_DONE) < 0) {
		QDMA_ERR("Timeout for get Rx Rate Limit configuration.\n") ;
		return -ETIME ;
	}
	
	rxRateLimitCfg = IO_GREG(QDMA_CSR_RX_RATELIMIT_CFG(base)) ;	
	return (rxRateLimitCfg&RX_RATELIMIT_VALUE_MASK)>>RX_RATELIMIT_VALUE_SHIFT ;
}

/******************************************************************************
******************************************************************************/
int qdmaSetTxRateLimitConfig(unchar chnlIdx, unchar type, ushort value) 
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint txRateLimitCfg = 0 ;
	
	txRateLimitCfg = (EGRESS_RATELIMIT_PARA_RWCMD | 
					((value<<EGRESS_RATELIMIT_PARA_VALUE_SHIFT)&EGRESS_RATELIMIT_PARA_VALUE_MASK) |
					((chnlIdx<<EGRESS_RATELIMIT_PARA_IDX_SHIFT)&EGRESS_RATELIMIT_PARA_IDX_MASK) |
					((type<<EGRESS_RATELIMIT_PARA_TYPE_SHIFT)&EGRESS_RATELIMIT_PARA_TYPE_MASK)) ;
	//QDMA_MSG(DBG_MSG, "txRateLimitCfg is 0x%08x.\n", txRateLimitCfg) ;
	IO_SREG(QDMA_CSR_RATELIMIT_PARAMETER_CFG(base), txRateLimitCfg) ;

	if(qdmaChecConfigDone(QDMA_CSR_RATELIMIT_PARAMETER_CFG(base), EGRESS_RATELIMIT_PARA_RWCMD_DONE) < 0) {
		QDMA_ERR("Timeout for set Tx Rate Limit configuration.\n") ;
		return -ETIME ;
	}
	
	return 0 ;
}

int qdmaGetTxRateLimitConfig(unchar chnlIdx, unchar type)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint txRateLimitCfg = 0 ;
	
	txRateLimitCfg = (((chnlIdx<<EGRESS_RATELIMIT_PARA_IDX_SHIFT)&EGRESS_RATELIMIT_PARA_IDX_MASK) |
					((type<<EGRESS_RATELIMIT_PARA_TYPE_SHIFT)&EGRESS_RATELIMIT_PARA_TYPE_MASK)) ;
	IO_SREG(QDMA_CSR_RATELIMIT_PARAMETER_CFG(base), txRateLimitCfg) ;
	
	if(qdmaChecConfigDone(QDMA_CSR_RATELIMIT_PARAMETER_CFG(base), EGRESS_RATELIMIT_PARA_RWCMD_DONE) < 0) {
		QDMA_ERR("Timeout for get Tx Rate Limit configuration.\n") ;
		return -ETIME ;
	}
	
	txRateLimitCfg = IO_GREG(QDMA_CSR_RATELIMIT_PARAMETER_CFG(base)) ;	
	return ((txRateLimitCfg&EGRESS_RATELIMIT_PARA_VALUE_MASK)>>EGRESS_RATELIMIT_PARA_VALUE_SHIFT) ;
}

int qdmaGetLimitRateMax(int ticksel_idx)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index=0, curRate=0, maxLimitRate=0;
	int chnlTickId=0;

	for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
		chnlTickId=qdmaIsTxRateLimitChnlTickSel1(base,index)?1:0;
		if(chnlTickId == ticksel_idx){
			if(qdmaIsChnlRateLimitEnable(base, index)) {
				curRate = qdmaGetTxRateLimitConfig(index, QDMA_Tx_Rate_Limit_PIR) ;
				if(curRate < 0)
					return curRate ;
				if(curRate > maxLimitRate)
					maxLimitRate = curRate ;
			}
		}
	}
	
	return maxLimitRate ;
}

/*channel 32 means get maxpir channel of all , otherwise means get maxpir channel except the input channel*/
int qdmaGetLimitRateMaxChnl(int ticksel_idx, int channel)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index=0, ret=0;
	int chnlTickId=0, maxChnl=CONFIG_QDMA_CHANNEL;
	int MaxTxLimitRate=0;
	
	if(channel < 0 || channel > CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL) ; 
		return -EINVAL ;
	}
	
	for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
		if(channel != index){
			chnlTickId=qdmaIsTxRateLimitChnlTickSel1(base,index)?1:0;
			if(chnlTickId == ticksel_idx){
				if(qdmaIsChnlRateLimitEnable(base, index)) {
					ret = qdmaGetTxRateLimitConfig(index, QDMA_Tx_Rate_Limit_PIR) ;
					if(ret < 0)
						return ret ;
					if(ret > MaxTxLimitRate){
						MaxTxLimitRate = ret ;	
						maxChnl = index;							
					}
				}
			}
		}
	}
	
	return maxChnl ;
}

int qdmaUpdateAllTxRateLimitValue(ushort curUnit, ushort newUnit, int ticksel_idx)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int ret=0, index=0, curPirValue=0, newPirValue=0;
	int chnlTickId=0, curPbsValue=0, newPbsValue=0;
	if(newUnit == 0)
		return -1;
	
	for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
		chnlTickId=qdmaIsTxRateLimitChnlTickSel1(base,index)?1:0;
		
		if((chnlTickId == ticksel_idx) && qdmaIsChnlRateLimitEnable(base, index)){
			curPirValue = qdmaGetTxRateLimitConfig(index, QDMA_Tx_Rate_Limit_PIR) ;
			curPbsValue = qdmaGetTxRateLimitConfig(index, QDMA_Tx_Rate_Limit_PBS);
			
			if(curPirValue < 0)
				return curPirValue ;			
			if(curPbsValue < 0)
				return curPbsValue ;
			
			newPirValue = curPirValue * curUnit / newUnit;
			newPbsValue = curPbsValue * curUnit / newUnit;
			
			if((ret = qdmaSetTxRateLimitConfig(index, QDMA_Tx_Rate_Limit_PIR, newPirValue)) < 0) {
				return ret ;
			}
			if((ret = qdmaSetTxRateLimitConfig(index, QDMA_Tx_Rate_Limit_PBS, newPbsValue)) < 0) {
				return ret ;
			}
		}
	}
	
	return 0 ;
}

int qdmaUpdateAllRxRateLimitValue(ushort curUnit, ushort newUnit, unchar ticker)
{
	int ret=0, index=0, curPirValue=0, newPirValue=0;
	int curPbsValue=0, newPbsValue=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	
	if(newUnit == 0)
		return -1;
	
	for(index=0; index<RX_RING_NUM; index++) {
		if(qdmaGetRxRatelimitSelTick(base,index) != ticker)
			continue;
		
		curPirValue = qdmaGetRxRateLimitConfig(index, QDMA_Rx_Rate_Limit_PIR) ;
		curPbsValue = qdmaGetRxRateLimitConfig(index, QDMA_Rx_Rate_Limit_PBS) ;
		if(curPirValue < 0)
			return curPirValue ;
		if(curPbsValue < 0)
		newPirValue = curPirValue * curUnit / newUnit;
		newPbsValue = curPbsValue * curUnit / newUnit;
		
		if((ret = qdmaSetRxRateLimitConfig(index, QDMA_Rx_Rate_Limit_PIR, newPirValue)) < 0) {
			return ret ;
		}
		if((ret = qdmaSetRxRateLimitConfig(index, QDMA_Rx_Rate_Limit_PBS, newPirValue)) < 0) {
			return ret ;
		}
	}
	
	return 0 ;
}
#endif

#if SUPPORT_QUEUE_DISABLE
int qdmaSetQueueClose_sw(uint base, unchar channel, unchar queue){
	unchar chnl_offset = (channel>>2);
	uint txq_dis_cfg = 0;

	if(chnl_offset >= TXQ_DIS_CFG_REG_NUM){
		QDMA_ERR("wrong channel index.\n") ;
		return -1;
	}

	txq_dis_cfg = TXQ_DIS_CFG_VALUE[chnl_offset];
	TXQ_DIS_CFG_VALUE[chnl_offset] = (txq_dis_cfg | TXQ_DIS_CHANNEL_QUEUE_OFFSET(channel,queue));
	IO_SREG(QDMA_CSR_QUEUE_CLOSE_CFG(base,channel), TXQ_DIS_CFG_VALUE[chnl_offset]);

	return 0;
}

int qdmaSetQueueOpen_sw(uint base, unchar channel, unchar queue){
	unchar chnl_offset = (channel>>2);
	uint txq_dis_cfg = 0;

	if(chnl_offset >= TXQ_DIS_CFG_REG_NUM){
		QDMA_ERR("wrong channel index.\n") ;
		return -1;
	}

	txq_dis_cfg = TXQ_DIS_CFG_VALUE[chnl_offset];
	TXQ_DIS_CFG_VALUE[chnl_offset] = (txq_dis_cfg & (~(TXQ_DIS_CHANNEL_QUEUE_OFFSET(channel,queue))));
	IO_SREG(QDMA_CSR_QUEUE_CLOSE_CFG(base,channel), TXQ_DIS_CFG_VALUE[chnl_offset]);

	return 0;
}

uint qdmaIsQueueClosed_sw(uint base, unchar channel, unchar queue){
	unchar chnl_offset = (channel>>2);
	uint txq_dis_cfg = 0;
	uint ret = 0;

	if(chnl_offset >= TXQ_DIS_CFG_REG_NUM){
		QDMA_ERR("wrong channel index.\n") ;
		return 0;
	}

	txq_dis_cfg = TXQ_DIS_CFG_VALUE[chnl_offset];
	ret = (txq_dis_cfg & TXQ_DIS_CHANNEL_QUEUE_OFFSET(channel, queue));

	return ret;
}

int qdmaSetChannelCfg_sw(uint base, unchar channel, uint value){
	unchar chnl_offset = (channel>>2);
	uint txq_dis_cfg = 0;

	if(chnl_offset >= TXQ_DIS_CFG_REG_NUM){
		QDMA_ERR("wrong channel index.\n") ;
		return -1;
	}

	txq_dis_cfg = TXQ_DIS_CFG_VALUE[chnl_offset];
	TXQ_DIS_CFG_VALUE[chnl_offset] = ((txq_dis_cfg&~(TXQ_DIS_CHANNEL_MASK(channel)))
									|((value<<TXQ_DIS_CHANNEL_SHIFT(channel))&TXQ_DIS_CHANNEL_MASK(channel)));
	IO_SREG(QDMA_CSR_QUEUE_CLOSE_CFG(base,channel), TXQ_DIS_CFG_VALUE[chnl_offset]);

	return 0;
}

uint qdmaIsChannelClosed_sw(uint base, unchar channel){
	unchar chnl_offset = (channel>>2);
	uint txq_dis_cfg = 0;
	uint ret = 0;

	if(chnl_offset >= TXQ_DIS_CFG_REG_NUM){
		QDMA_ERR("wrong channel index.\n") ;
		return 0;
	}

	txq_dis_cfg = TXQ_DIS_CFG_VALUE[chnl_offset];
	ret = ((txq_dis_cfg & TXQ_DIS_CHANNEL_MASK(channel))>>TXQ_DIS_CHANNEL_SHIFT(channel));

	return ret;
}
#endif
/******************************************************************************
******************************************************************************/
int qdmaSetVirtualChannelQos(unchar phyChnl, unchar virChnlMax, unchar mode, unchar weight[4])
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint wrrCfg = 0 ;
	int virChnl = 0 ;
		
	IO_SMASK(QDMA_CSR_VCHNL_QOS_MODE(base), PHYSICAL_CHNL_QOS_MODE_MASK(phyChnl), PHYSICAL_CHNL_QOS_MODE_SHIFT(phyChnl), mode) ;
	
	for(virChnl=0 ; virChnl<virChnlMax ; virChnl++) {
		if(weight[virChnl] != 255) {
			wrrCfg = (VCHNL_WEIGHT_RWCMD | 
					  ((weight[virChnl]<<VCHNL_WRR_VALUE_SHIFT)&VCHNL_WRR_VALUE_MASK) |
					  ((phyChnl<<PHYSICAL_CHNL_IDX_SHIFT)&PHYSICAL_CHNL_IDX_MASK) |
					  ((virChnl<<VIRTUAL_CHNL_IDX_SHIFT)&VIRTUAL_CHNL_IDX_MASK)) ;
			IO_SREG(QDMA_CSR_VCHNL_WEIGHT_CFG(base), wrrCfg) ;
			
			if(qdmaChecConfigDone(QDMA_CSR_VCHNL_WEIGHT_CFG(base), VCHNL_WEIGHT_DONE) < 0) {
				QDMA_ERR("Timeout for setting virtual channel configuration, physical channel:%d, virtual channel:%d.\n", phyChnl, virChnl) ;
				return -ETIME ;
			}
		}
	}
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
int qdmaGetVirtualChannelQos(unchar phyChnl, unchar virChnlMax, unchar *pMode, unchar weight[4])
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint wrrCfg = 0 ;
	int virChnl = 0;
	
	*pMode = IO_GMASK(QDMA_CSR_VCHNL_QOS_MODE(base), PHYSICAL_CHNL_QOS_MODE_MASK(phyChnl), PHYSICAL_CHNL_QOS_MODE_SHIFT(phyChnl)) ;

	for(virChnl=0 ; virChnl<virChnlMax ; virChnl++) {
		wrrCfg = (((phyChnl<<PHYSICAL_CHNL_IDX_SHIFT)&PHYSICAL_CHNL_IDX_MASK) |
				  ((virChnl<<VIRTUAL_CHNL_IDX_SHIFT)&VIRTUAL_CHNL_IDX_MASK)) ;
		IO_SREG(QDMA_CSR_VCHNL_WEIGHT_CFG(base), wrrCfg) ;
	
		if(qdmaChecConfigDone(QDMA_CSR_VCHNL_WEIGHT_CFG(base), VCHNL_WEIGHT_DONE) < 0) {
			QDMA_ERR("Timeout for getting virtual channel configuration, physical channel:%d, virtual channel:%d.\n", phyChnl, virChnl) ;
			return -ETIME ;
		}
		wrrCfg = IO_GREG(QDMA_CSR_VCHNL_WEIGHT_CFG(base)) ;
		weight[virChnl] =  ((wrrCfg&VCHNL_WRR_VALUE_MASK)>>VCHNL_WRR_VALUE_SHIFT) ;
	}
	return 0 ;
}

/******************************************************************************
******************************************************************************/
#if SUPPORT_SDN_CNTR
/*cnt grp0 index: 0~62 ; cnt grp1 index: 0~30 ; cnt grp2 index: 0~126*/
int qdmaGetFlowCntByteLow(uint base, unchar grp, unchar idx)
{
	uint sdnCntrParaCfg = 0;
	
	sdnCntrParaCfg = (((SDN_CNTR_PARAM_CMD_TYPE(grp)<<SDN_CNTR_PARAM_CMD_TYPE_SHIFT)&SDN_CNTR_PARAM_CMD_TYPE_MASK) |
				((SDN_CNTR_PARAM_SEL_BYTE<<SDN_CNTR_PARAM_PKTCNTSEL_SHIFT)&SDN_CNTR_PARAM_PKTCNTSEL_MASK) |
				((idx<<SDN_CNTR_PARAM_TABLE_IDX_SHIFT)&SDN_CNTR_PARAM_TABLE_IDX_MASK));

	IO_SREG(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), sdnCntrParaCfg);
	if( qdmaChecConfigDone(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), SDN_CNTR_PARAM_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set sdn cntr configuration.\n");
		return -ETIME;
	}

	return IO_GREG(QDMA_CSR_SDN_CNTR_DATA_LO(base));
}

int qdmaGetFlowCntByteHigh(uint base, unchar grp, unchar idx)
{
	uint sdnCntrParaCfg = 0;
	
	sdnCntrParaCfg = (((SDN_CNTR_PARAM_CMD_TYPE(grp)<<SDN_CNTR_PARAM_CMD_TYPE_SHIFT)&SDN_CNTR_PARAM_CMD_TYPE_MASK) |
				((SDN_CNTR_PARAM_SEL_BYTE<<SDN_CNTR_PARAM_PKTCNTSEL_SHIFT)&SDN_CNTR_PARAM_PKTCNTSEL_MASK) |
				((idx<<SDN_CNTR_PARAM_TABLE_IDX_SHIFT)&SDN_CNTR_PARAM_TABLE_IDX_MASK));

	IO_SREG(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), sdnCntrParaCfg);
	if( qdmaChecConfigDone(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), SDN_CNTR_PARAM_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set sdn cntr configuration.\n");
		return -ETIME;
	}

	return IO_GREG(QDMA_CSR_SDN_CNTR_DATA_HI(base));
}

int qdmaClearFlowCntByte(uint base, unchar grp, unchar idx)
{
	uint sdnCntrParaCfg = 0;
	ulong flags=0 ;

	sdnCntrParaCfg = (SDN_CNTR_PARAM_CFG_RWCMD |
				((SDN_CNTR_PARAM_CMD_TYPE(grp)<<SDN_CNTR_PARAM_CMD_TYPE_SHIFT)&SDN_CNTR_PARAM_CMD_TYPE_MASK) |
				((SDN_CNTR_PARAM_SEL_BYTE<<SDN_CNTR_PARAM_PKTCNTSEL_SHIFT)&SDN_CNTR_PARAM_PKTCNTSEL_MASK) |
				((idx<<SDN_CNTR_PARAM_TABLE_IDX_SHIFT)&SDN_CNTR_PARAM_TABLE_IDX_MASK));
	spin_lock_irqsave(&qdma_config_lock, flags) ;
	IO_SREG(QDMA_CSR_SDN_CNTR_DATA_LO(base), 0);
#ifdef TCSUPPORT_CPU_ARMV8
	wmb();
#endif
	IO_SREG(QDMA_CSR_SDN_CNTR_DATA_HI(base), 0);
#ifdef TCSUPPORT_CPU_ARMV8
	wmb();
#endif
	IO_SREG(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), sdnCntrParaCfg);
	spin_unlock_irqrestore(&qdma_config_lock, flags) ;
	
	if( qdmaChecConfigDone(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), SDN_CNTR_PARAM_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set sdn cntr configuration.\n");
		return -ETIME;
	}

	return 0;
}

int qdmaGetFlowCntPkt(uint base, unchar grp, unchar idx)/* qdmaGetFlowCntPktLow */
{
	uint sdnCntrParaCfg = 0;
	
	sdnCntrParaCfg = (((SDN_CNTR_PARAM_CMD_TYPE(grp)<<SDN_CNTR_PARAM_CMD_TYPE_SHIFT)&SDN_CNTR_PARAM_CMD_TYPE_MASK) |
				((SDN_CNTR_PARAM_SEL_PKT<<SDN_CNTR_PARAM_PKTCNTSEL_SHIFT)&SDN_CNTR_PARAM_PKTCNTSEL_MASK) |
				((idx<<SDN_CNTR_PARAM_TABLE_IDX_SHIFT)&SDN_CNTR_PARAM_TABLE_IDX_MASK));

	IO_SREG(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), sdnCntrParaCfg);
	if( qdmaChecConfigDone(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), SDN_CNTR_PARAM_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set sdn cntr configuration.\n");
		return -ETIME;
	}

	return IO_GREG(QDMA_CSR_SDN_CNTR_DATA_LO(base));
}

int qdmaGetFlowCntPktHigh(uint base, unchar grp, unchar idx)
{
	uint sdnCntrParaCfg = 0;
	
	sdnCntrParaCfg = (((SDN_CNTR_PARAM_CMD_TYPE(grp)<<SDN_CNTR_PARAM_CMD_TYPE_SHIFT)&SDN_CNTR_PARAM_CMD_TYPE_MASK) |
				((SDN_CNTR_PARAM_SEL_PKT<<SDN_CNTR_PARAM_PKTCNTSEL_SHIFT)&SDN_CNTR_PARAM_PKTCNTSEL_MASK) |
				((idx<<SDN_CNTR_PARAM_TABLE_IDX_SHIFT)&SDN_CNTR_PARAM_TABLE_IDX_MASK));

	IO_SREG(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), sdnCntrParaCfg);
	if( qdmaChecConfigDone(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), SDN_CNTR_PARAM_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set sdn cntr configuration.\n");
		return -ETIME;
	}

	return IO_GREG(QDMA_CSR_SDN_CNTR_DATA_HI(base));
}

int qdmaClearFlowCntPkt(uint base, unchar grp, unchar idx)
{
	uint sdnCntrParaCfg = 0;
	ulong flags=0 ;
	sdnCntrParaCfg = (SDN_CNTR_PARAM_CFG_RWCMD |
				((SDN_CNTR_PARAM_CMD_TYPE(grp)<<SDN_CNTR_PARAM_CMD_TYPE_SHIFT)&SDN_CNTR_PARAM_CMD_TYPE_MASK) |
				((SDN_CNTR_PARAM_SEL_PKT<<SDN_CNTR_PARAM_PKTCNTSEL_SHIFT)&SDN_CNTR_PARAM_PKTCNTSEL_MASK) |
				((idx<<SDN_CNTR_PARAM_TABLE_IDX_SHIFT)&SDN_CNTR_PARAM_TABLE_IDX_MASK));

	spin_lock_irqsave(&qdma_config_lock, flags) ;
	IO_SREG(QDMA_CSR_SDN_CNTR_DATA_LO(base), 0);
#ifdef TCSUPPORT_CPU_ARMV8
	wmb();
#endif
	IO_SREG(QDMA_CSR_SDN_CNTR_DATA_HI(base), 0);
#ifdef TCSUPPORT_CPU_ARMV8
	wmb();
#endif
	IO_SREG(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), sdnCntrParaCfg);

	spin_unlock_irqrestore(&qdma_config_lock, flags) ;
	
	if( qdmaChecConfigDone(QDMA_CSR_SDN_CNTR_PARAM_CFG(base), SDN_CNTR_PARAM_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set sdn cntr configuration.\n");
		return -ETIME;
	}

	return 0;
}
#endif

#if SUPPORT_DBG_CNTMEM
int qdma_get_dbg_cntmem_cntr(unchar channel, QDMA_DbgCntrMemGroup_t cntrMemType)
{
	uint base = gpQdmaPriv->csrBaseAddr;
	uint cntmemParaCfg = 0;
	
	cntmemParaCfg = (((cntrMemType<<DBG_CNTRMEM_TYPE_SHIFT)&DBG_CNTRMEM_TYPE_MASK) |
				((channel<<DBG_CNTRMEM_CHNL_IDX_SHIFT)&DBG_CNTRMEM_CHNL_IDX_MASK));

	IO_SREG(QDMA_CSR_DBG_CNTMEM_CFG(base), cntmemParaCfg);
	if( qdmaChecConfigDone(QDMA_CSR_DBG_CNTMEM_CFG(base), DBG_CNTRMEM_PARA_RWCMD_DONE) != 0) {
		printk("Timeout for set dbg cntmem configuration.\n");
		return -ETIME;
	}

	return IO_GREG(QDMA_CSR_DBG_CNTMEM_VAR(base));
}

int qdma_set_dbg_cntmem_clear(unchar channel, QDMA_DbgCntrMemGroup_t cntrMemType)
{
	uint base = gpQdmaPriv->csrBaseAddr;
	uint cntmemParaCfg = 0;
	ulong flags=0 ;


	cntmemParaCfg = (DBG_CNTRMEM_PARA_RWCMD |
				((cntrMemType<<DBG_CNTRMEM_TYPE_SHIFT)&DBG_CNTRMEM_TYPE_MASK) |
				((channel<<DBG_CNTRMEM_CHNL_IDX_SHIFT)&DBG_CNTRMEM_CHNL_IDX_MASK));
	
	spin_lock_irqsave(&qdma_config_lock, flags) ;
	IO_SREG(QDMA_CSR_DBG_CNTMEM_VAR(base), 0);
#ifdef TCSUPPORT_CPU_ARMV8
	wmb();
#endif
	IO_SREG(QDMA_CSR_DBG_CNTMEM_CFG(base), cntmemParaCfg);
	spin_unlock_irqrestore(&qdma_config_lock, flags) ;
	
	if( qdmaChecConfigDone(QDMA_CSR_DBG_CNTMEM_CFG(base), DBG_CNTRMEM_PARA_RWCMD_DONE) != 0) {
		printk("Timeout for set dbg cntmem configuration.\n");
		return -ETIME;
	}

	return 0;
}
#endif

/******************************************************************************
 Descriptor:	It's used to configure dbg counter, which has 40 groups for EN7516, 64 for EN7580.
 Input Args:		- cntrIdx:
 				- cntrEn:
 				- cntrSrc:
 				- chnlIdx:
 				- queueIdx:
 				- dscpRingIdx:
 				- isChnlAll:
 				- isQueueAll:
 				- isDscpRingAll:
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_clear_and_set_dbg_cntr_info(QDMA_DBG_CNTR_T *dbgCntrPtr)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	unchar idx = dbgCntrPtr->cntrIdx;
	
	if(dbgCntrPtr->cntrIdx>=CONFIG_MAX_CNTR_NUM) {
		QDMA_ERR("Fault: dbg Cntr counter index shoule between 0 and %d\n", CONFIG_MAX_CNTR_NUM-1) ; 
		return -EINVAL ;
	}
	if(dbgCntrPtr->cntrSrc<0 || dbgCntrPtr->cntrSrc>=QDMA_DBG_CNTR_SRC_ITEMS) {
		QDMA_ERR("Fault: dbg Cntr source shoule between 0 and %d\n", QDMA_DBG_CNTR_SRC_ITEMS-1) ; 
		return -EINVAL ;
	}
	if((dbgCntrPtr->isChnlAll == QDMA_DISABLE) && (dbgCntrPtr->chnlIdx>=CONFIG_QDMA_CHANNEL)) {
		QDMA_ERR("Fault: Channel index shoule between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}
	if((dbgCntrPtr->isQueueAll == QDMA_DISABLE) && ( dbgCntrPtr->queueIdx>=CONFIG_QDMA_QUEUE)) {
		QDMA_ERR("Fault: Queue index shoule between 0 and %d\n", CONFIG_QDMA_QUEUE-1) ; 
		return -EINVAL ;
	}
	if((dbgCntrPtr->isDscpRingAll == QDMA_DISABLE) && (dbgCntrPtr->dscpRingIdx>=RX_RING_NUM)) {
		QDMA_ERR("Fault: ringIdx should be 0 ~ %d\n", (RX_RING_NUM-1)) ; 
		return -EINVAL ;
	}

	qdmaSetCntrSrc(base, idx, dbgCntrPtr->cntrSrc);
	qdmaSetCntrChnlIdx(base, idx, dbgCntrPtr->chnlIdx);
	qdmaSetCntrQueueIdx(base, idx, dbgCntrPtr->queueIdx);
	qdmaSetCntrDscpIdx(base, idx, dbgCntrPtr->dscpRingIdx);
	
	if(dbgCntrPtr->isChnlAll == QDMA_DISABLE) {
		qdmaSetCntrChnlSingle(base,idx);
	} else {
		qdmaSetCntrChnlAll(base,idx);
	}
	
	if(dbgCntrPtr->isQueueAll == QDMA_DISABLE) {
		qdmaSetCntrQueueSingle(base,idx);
	} else {
		qdmaSetCntrQueueAll(base,idx);
	}
	
	if(dbgCntrPtr->isDscpRingAll == QDMA_DISABLE) {
		qdmaSetCntrDscpSingle(base,idx);
	} else {
		qdmaSetCntrDscpAll(base,idx);
	}
	
	qdmaClearCntrCounter(base, idx);
	if(dbgCntrPtr->cntrEn == QDMA_DISABLE) {
		qdmaDisableCntrCfg(base, idx);
	} else {
		qdmaEnableCntrCfg(base, idx);
	}
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to configure dbg counter, which has 32 groups to set.
 Input Args:		- cntrIdx:
 Output Args:	- cntrEn:
 				- cntrSrc:
 				- chnlIdx:
 				- queueIdx:
 				- dscpRingIdx:
 				- isChnlAll:
 				- isQueueAll:
 				- isDscpRingAll:
 				- cntrVal:
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_dbg_cntr_info(QDMA_DBG_CNTR_T *dbgCntrPtr)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	unchar idx = dbgCntrPtr->cntrIdx ;

	if(dbgCntrPtr->cntrIdx>=CONFIG_MAX_CNTR_NUM) {
		QDMA_ERR("Fault: dbg Cntr counter index shoule between 0 and %d\n", CONFIG_MAX_CNTR_NUM-1) ; 
		return -EINVAL ;
	}

	dbgCntrPtr->cntrEn = qdmaIsCntrEnable(base, idx);
	dbgCntrPtr->cntrSrc = qdmaGetCntrSrc(base,idx);
	dbgCntrPtr->chnlIdx = qdmaGetCntrChnlIdx(base,idx);
	dbgCntrPtr->queueIdx = qdmaGetCntrQueueIdx(base,idx);
	dbgCntrPtr->dscpRingIdx = qdmaGetCntrDscpIdx(base,idx);
	
	dbgCntrPtr->isChnlAll = qdmaIsCntrChnlAll(base,idx);
	dbgCntrPtr->isQueueAll = qdmaIsCntrQueueAll(base,idx);
	dbgCntrPtr->isDscpRingAll = qdmaIsCntrDscpRingAll(base,idx);
	dbgCntrPtr->cntrVal = qdmaGetCntrCounter(base, idx);

	return 0;
}

int qdma_set_dbg_cntr_default_config(void)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index=0 ;
    QDMA_DBG_CNTR_T dbgCntr ;
	dbgCntr.cntrEn = DBG_CNTR_ENABLE;
	dbgCntr.isChnlAll = 1;
	dbgCntr.isQueueAll = 1;
	dbgCntr.isDscpRingAll = 1;
	dbgCntr.queueIdx = 0 ;
	dbgCntr.chnlIdx = 0 ;
	dbgCntr.dscpRingIdx = 0;

#if SUPPORT_DBG_CNTMEM
	qdmaEnableCntrMemCounter(base);
	return 0;
#endif

	for(index = 0; index<DBG_CNTR_DEFAULT_CONFIG_NUM; index++) {
		switch(index) {
			case 0:		// tx cpu-path total count
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;			
				break;
				
			case 1:		// tx total count
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;	
				break;
				
			case 2:		// rx cpu-path total count
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_RX;			
				break;
				
			case 3:		// rx fwd-path total count
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_RX;	
				break;
#if SUPPORT_DBGCNTR_2RING_MODE
			case 4:		// tx cpu-path Ring0 count
			    dbgCntr.isDscpRingAll = 0;
			    dbgCntr.dscpRingIdx = 0;
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;			
				break;
				
			case 5:		// tx cpu-path Ring1 count
			    dbgCntr.isDscpRingAll = 0;
			    dbgCntr.dscpRingIdx = 1;
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;	
				break;
				
			case 6:		// rx cpu-path Ring0 count
			    dbgCntr.isDscpRingAll = 0;
			    dbgCntr.dscpRingIdx = 0;
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_RX;			
				break;
				
			case 7:		// rx cpu-path Ring1 count
			    dbgCntr.isDscpRingAll = 0;
			    dbgCntr.dscpRingIdx = 1;
        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_RX;	
				break;
#else
			default:
				if( (index-4) < TX_RING_NUM )// tx cpu-path Ring count
				{
					dbgCntr.isDscpRingAll = 0;
				    dbgCntr.dscpRingIdx = (index-4);
	        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;	
				}
				else if( (index-4-TX_RING_NUM) < RX_RING_NUM )// rx cpu-path Ring count
				{
					dbgCntr.isDscpRingAll = 0;
				    dbgCntr.dscpRingIdx = (index-4-TX_RING_NUM);
	        		dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_RX;	
				}
#ifdef QDMA_LAN
#if SUPPORT_QDMA_MULTICAST
        		else if((index-4-TX_RING_NUM-RX_RING_NUM) < MULTICAST_PORT_NUM ) //lan multicast
        		{
	        		dbgCntr.isChnlAll = 0;
	        		dbgCntr.chnlIdx = (index-4-TX_RING_NUM-RX_RING_NUM);
	        		dbgCntr.cntrSrc = DBG_CNTR_MULTICAST_DROP;	
                    
        		}
        		else
        		{
	        		dbgCntr.isChnlAll = 1;
	        		dbgCntr.chnlIdx = 0;
	        		dbgCntr.cntrSrc = DBG_CNTR_MULTICAST_ALL_DROP;	
        		}
#endif
#endif 
				break;
#endif
		}
		dbgCntr.cntrIdx = index+DBG_CNTR_DEFAULT_CONFIG_OFFSET ;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);		
	}
	return 0;
}

int qdmaSetRxRatelimitDefaultConfig(void){
    uint base = gpQdmaPriv->csrBaseAddr ;
    struct ECNT_QDMA_Data qdma_data;
    char ringIdx = 0;
	
							

#if defined(TCSUPPORT_SWITCH_8851)
	
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    int tickerSel[RX_RING_NUM] = {TRTCM_SLOW_TICK, TRTCM_FAST_TICK, TRTCM_SLOW_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_SLOW_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK};
    /* ring2 for DLF/icmp flow*/
    int rateVal[RX_RING_NUM] = {1500, 1000000, 1500, 1500, 1500, 1000000, 1000000, 200,
                            200, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000}; /* array length need adjust according to demand*/
							
							

#elif SUPPORT_IFC_TCAM
	
	GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    int tickerSel[RX_RING_NUM] = {TRTCM_SLOW_TICK, TRTCM_FAST_TICK, TRTCM_SLOW_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_SLOW_TICK, TRTCM_SLOW_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK};
    /* ring2 for DLF/icmp flow*/
    int rateVal[RX_RING_NUM] = {4000, 1000000, 4000, 4000, 4000, 1000000, 1000000, 4000,
                            200, 200, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000,
                            1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000,
                            1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000}; /* array length need adjust according to demand*/
							

#elif defined(TCSUPPORT_CPU_EN7580)
	
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    int tickerSel[RX_RING_NUM] = {TRTCM_SLOW_TICK, TRTCM_FAST_TICK, TRTCM_SLOW_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_SLOW_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK
                                , TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK, TRTCM_FAST_TICK};
    /* ring2 for DLF/icmp flow*/
    int rateVal[RX_RING_NUM] = {4000, 1000000, 4000, 4000, 4000, 1000000, 1000000, 200,
                            200, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000}; /* array length need adjust according to demand*/
							
							
						
#else
	
    QDMA_RxRateLimitSet_T rxRateLimit;
    int rateVal[RX_RING_NUM] ={4000, 1000000, 4000, 1000000, 1000000, 1000000, 1000000, 1000000,
                                     200, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000, 1000000}; /*pps*/
#endif

    memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data)) ;
    if(isFPGA)
        rateVal[0]= 1800; /*1800pps for 7528 fpga*/
    
#if SUPPORT_MTR_ACNT_IN_QDMA
    /*cfg glb setting*/
    rxRateLimitCfg.MeterEn = GENERAL_METER_ENABLE;
    rxRateLimitCfg.PktMode = TRTCM_PACKET_MODE;
    
    rxRateLimitCfg.trtcmModule = INGRESS_TRTCM;
    
    /*ratelimit glb setting*/
    rxRateLimitSet.trtcmModule = INGRESS_TRTCM;

    for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++){
        /*cfg each ring*/
        rxRateLimitCfg.TickSel = tickerSel[ringIdx];
        rxRateLimitCfg.Index = ringIdx; 
        qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRateLimitCfg;
        qdma_general_set_ratelimit_mode_cfg(&qdma_data);

        /*ratelimit each ring*/
        rxRateLimitSet.Index = ringIdx; 
        rxRateLimitSet.RateLimitValue = rateVal[ringIdx];
        qdma_data.qdma_private.generalTrtcmRatelimitSetPtr = &rxRateLimitSet;
        qdma_general_set_ratelimit_mode_value(&qdma_data);
    }
    generalSetTrtcmMode(INGRESS_TRTCM, TRTCM_RATELIMIT_MODE);
#else
    /* rx ratelimit default setting*/
    qdmaEnableRxRateLimit(base) ;   /* global rate limit default enable */
    qdmaSetRxRateLimitBucketScale(base, QDMA_TRTCM_SCALE_2BYTE) ;
    qdmaSetRxRateLimitByPacket(base);
    qdmaSetRxRateLimitTickVal(base, 0, 62500);       
    qdmaSetRxRateLimitTickVal(base, 1, 62500);       /* unit is 16 pps */
    qdmaSetRxBindToTick1Specified(base, 0xFFFF);     /*tmp bind all rx ring to ticker1*/

    for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++){
        rxRateLimit.rateLimitValue = rateVal[ringIdx];
        rxRateLimit.ringIdx = ringIdx;
        qdma_data.qdma_private.rxRateLimitSetPtr = &rxRateLimit;
        if(qdma_set_rx_ratelimit(&qdma_data)){
            printk("Error:set Rx%d ratelimit failed\n", ringIdx);
        }
    }
#endif
    return 0;

}

/******************************************************************************
******************************************************************************/
int qdmaSetDbgMemXsConfig(QDMA_DbgMemXsMemSel_t memSel, unchar byteSel, ushort addr, uint valueLow, uint valueHigh) 
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint dbgMemXsCfg = 0 ;
	ulong flags = 0;

	dbgMemXsCfg = (DBG_MEM_XS_RWCMD | 
					((memSel<<DBG_MEM_XS_MEMSEL_SHIFT)&DBG_MEM_XS_MEMSEL_MASK) |
					((byteSel<<DBG_MEM_XS_BYTESEL_SHIFT)&DBG_MEM_XS_BYTESEL_MASK) |
					((addr<<DBG_MEM_XS_ADDR_SHIFT)&DBG_MEM_XS_ADDR_MASK)) ;
	printk("dbgMemXsCfg is 0x%08x, valueLow:0x%08x, valueHigh:0x%08x.\n", dbgMemXsCfg, valueLow, valueHigh) ;
	spin_lock_irqsave(&qdma_config_lock, flags);
	/*  prepare data for DATA_LO and DATA_HI register */
	IO_SREG(QDMA_CSR_DBG_MEM_XS_DATA_LO(base), valueLow) ;
#ifdef TCSUPPORT_CPU_ARMV8
	wmb();
#endif
	IO_SREG(QDMA_CSR_DBG_MEM_XS_DATA_HI(base), valueHigh) ;
#ifdef TCSUPPORT_CPU_ARMV8
	wmb();
#endif
	/* write to dbg mem xs config */
	IO_SREG(QDMA_CSR_DBG_MEM_XS_CFG(base), dbgMemXsCfg) ;
	spin_unlock_irqrestore(&qdma_config_lock, flags);
	
	if(qdmaChecConfigDone(QDMA_CSR_DBG_MEM_XS_CFG(base), DBG_MEM_XS_RWCMD_DONE) < 0) {
		QDMA_ERR("Timeout for set dbg mem xs data.\n") ;
		return -ETIME ;
	}
	
	return 0 ;
}

int qdmaGetDbgMemXsConfig(QDMA_DbgMemXsMemSel_t memSel, unchar byteSel, ushort addr, uint *value)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint dbgMemXsCfg = 0 ;
	
	dbgMemXsCfg = (((memSel<<DBG_MEM_XS_MEMSEL_SHIFT)&DBG_MEM_XS_MEMSEL_MASK) |
					((byteSel<<DBG_MEM_XS_BYTESEL_SHIFT)&DBG_MEM_XS_BYTESEL_MASK) |
					((addr<<DBG_MEM_XS_ADDR_SHIFT)&DBG_MEM_XS_ADDR_MASK)) ;
	
	IO_SREG(QDMA_CSR_DBG_MEM_XS_CFG(base), dbgMemXsCfg) ;
	
	if(qdmaChecConfigDone(QDMA_CSR_DBG_MEM_XS_CFG(base), DBG_MEM_XS_RWCMD_DONE) < 0) {
		QDMA_ERR("Timeout for get dbg mem xs value.\n") ;
		return -ETIME ;
	}
	
	*value = IO_GREG(QDMA_CSR_DBG_MEM_XS_DATA_LO(base));
	return 0 ;
}
#ifdef TCSUPPORT_QOS
int isDefaultQueue(struct sk_buff *skb)
{
	int queue=0;
	queue = skb->mark & QOS_FILTER_MARK;
	if(queue == 0){
		return 1;
	}
	
	if(gQueueMask & (1 << ((queue >> 4) -1 ))){ 	/* default queue */
		return 0;
	}else{
		return 1;
	}
}

int isWeigt0(struct sk_buff *skb)
{
	int queue=0;

#if SUPPORT_WRR_WEIGHT_0
	return 0;
#else

	queue = (skb->mark & QOS_FILTER_MARK) >> 4;

#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	if(QOS_HW_WRR_8_QUEUE == qos_flag)
	{
		if(queue < 1 || queue >8){	/* wrr queue is from 1 to 8*/
			return 0;
		}
	}
	else
	{
#endif
	if(queue < 1 || queue >4){	/* wrr queue is from 1 to 4 */
		return 0;
	}
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	}
#endif

	if(qos_wrr_info[queue]){
		return 0;
	}else{				/* weight is 0 */
		return 1;
	}	
#endif    
}

#define ICMPV6_ROUTE_SOL 133
#define ICMPV6_ROUTE_ADV 134
#define ICMPV6_NEIGH_SOL 135
#define ICMPV6_NEIGH_ADV 136
#if defined(QDMA_LAN)
static char isPriorityPkt(struct sk_buff *skb, unchar *cp, struct port_info *pMacInfo)
{
	ushort etherType=0;
	unchar ipVerLen=0;
	unchar ipProtocol=0;
	unchar tcpFlags=0;
	ushort pppProtocol=0;
	unchar ipv6_protocol=0, ipv6_type=0;
	ushort dport=0, sport=0;
	struct igmphdr * ih = NULL;
	int i=0;

	if (cp == NULL)
	{
		QDMA_ERR("ERROR: cp is NULL at isPriorityPkt\n");
		return 0;
	}

    /*atm router mode, no Mac header and vlan*/
    if((gpQdmaPriv->devCfg.macType == MAC_TYPE_ATM) && ((pMacInfo->atm_pppoa == 1) || (pMacInfo->atm_ipoa == 1))) {
		goto ipv4_header;
    }
    
	/* skip DA and SA mac address */
	cp += 12;
	/* get ether type */
	etherType = *(ushort *) cp;
	/* skip ether type */
	cp += 2;

	/* EtherWan remove special tag */
	if(pMacInfo->eth_macSTagEn == 1)
	{
		cp += 2;
		etherType = *(ushort *) cp;
		cp += 2;
	}

	/*parse if vlan exists*/
	if (etherType == htons(0x8100)) {
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}
    	/*parse if vlan exists*/
	if (etherType == htons(0x8100)) {
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	/*check whether PPP packets*/
	if (etherType == htons(0x8864)) {
		/* skip pppoe head */
		cp += 6; 					/* 6: PPPoE header 2: PPP protocol */
		/* get ppp protocol */
		pppProtocol = *(ushort *) cp;
		/* check if LCP protocol and ipcpv6 protocol */
		if ((pppProtocol == htons(0xc021)) || (pppProtocol == htons(0x8021)) || (pppProtocol == htons(0x8057)) 
			|| (pppProtocol == htons(0xc223)) || (pppProtocol == htons(0xc057))) {
			return 1;
		/* check if IPv6 protocol */
		} else if (pppProtocol == htons(0x0057)) {
			cp += 2;
			cp += 6;
			/* get ip protocol */
			ipProtocol = *(unchar*)cp;
			ipVerLen = 0;
			cp += 34;
         
			if (ipProtocol == 0x3a) {
    				ipv6_type = *(unchar*)cp;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCC)  
                		if(ipv6_type == 130){ 
                    			return -1;// lan/wifi to wan mld query will drop
                		} 
#endif/*TCSUPPORT_COMPILE*/

			}

			goto ipv6_header;
		/* check if IP protocol */
		} else if (pppProtocol != htons(0x0021)) {
			return 0;
		}
		/* skip ppp protocol */
		cp += 2; 					/* 6: PPPoE header 2: PPP protocol */
	} else if (etherType == htons(0x8863)) {
		return 1;
	/*check whether arp packet*/	
	} else if (etherType == htons(0x0806)) {
		return 1;	
	} 
	else if (etherType == htons(0x86dd)) {
		cp += 6;
         
		ipv6_protocol = *(unchar*)cp;
		cp += 34;
		if (ipv6_protocol == 0x3a) {
			ipv6_type = *(unchar*)cp;
 #if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCC)
			if(ipv6_type == 130){ 
				return -1;// lan/wifi to wan mld query will drop
			}
 #endif/*TCSUPPORT_COMPILE*/
		}
		/* get ip protocol */
		ipProtocol = ipv6_protocol;
		if(ipProtocol == 0x11) /* udp + dns */
		{
			if(  *(ushort *)(cp + 2) == htons(0x0035) )
				return 1;
		}
		ipVerLen = 0;
		goto ipv6_header;
	}
	else {
		/* check if ip packet */
		if (etherType != htons(0x0800)) {
			return 0;
		}
	}

ipv4_header:

	/*  frank add 20121220 for pppoa mode */
	pppProtocol = *(ushort *) cp;
	/* check if LCP protocol, for pppoa control packet */
	if (pppProtocol == htons(0xc021)) {
		return 1;
	} else if(pppProtocol == htons(0x0021)) {
		cp += 2; 					/* 6: PPPoE header 2: PPP protocol */
	}

	/* check if it is a ipv4 packet */
	ipVerLen = *cp;
	if ((ipVerLen & 0xf0) != 0x40) {
		return 0;
	}

	/* get ip protocol */
	ipProtocol = *(cp + 9);

	if(ipProtocol == 0x11) /* udp + dns */
	{
		if( *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) + 2) == htons(0x0035) )
			return 1;
	}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCC)
    /*check if IGMP protocol */
    if(ipProtocol == 2)
    {
        ih = (struct igmphdr *)(cp+20);
        if(ih->type == IGMP_HOST_MEMBERSHIP_QUERY)
            return -1;   // lan/wifi to wan igmp query will drop
        else
            return 1;
    }
	/* check if ICMP*/
	if (ipProtocol == 1) {
        return 1;
	}
#else/*TCSUPPORT_COMPILE*/
    if((ipProtocol == 2) || (ipProtocol == 1))
        return 1;
#endif/*TCSUPPORT_COMPILE*/
ipv6_header:


	/* check if TCP protocol */
	if (ipProtocol != 6) {
		return 0;
	}

	/* align to TCP header */
	cp += (ipVerLen & 0x0f) << 2;
	/* get TCP flags */
	tcpFlags = *(cp + 13);
	
	/* check if TCP ack */
	if ((tcpFlags & 0x10) == 0x10) {
		return 1;
	}
    
	return 0;
}

#else

static char isPriorityPkt(struct sk_buff *skb, unchar *cp, struct port_info *pMacInfo)
{
	ushort etherType=0;
	unchar ipVerLen=0;
	unchar ipProtocol=0;
	unchar tcpFlags=0;
	ushort pppProtocol=0;
	unchar ipv6_protocol=0, ipv6_type=0;
	ushort dport=0, sport=0;
	struct igmphdr * ih = NULL;
	int i=0;

	if (cp == NULL)
	{
		QDMA_ERR("ERROR: cp is NULL at isPriorityPkt\n");
		return 0;
	}

    /*atm router mode, no Mac header and vlan*/
    if((gpQdmaPriv->devCfg.macType == MAC_TYPE_ATM) && ((pMacInfo->atm_pppoa == 1) || (pMacInfo->atm_ipoa == 1))) {
		goto ipv4_header;
    }
    
	/* skip DA and SA mac address */
	cp += 12;
	/* get ether type */
	etherType = *(ushort *) cp;
	/* skip ether type */
	cp += 2;

	/*parse if vlan exists*/
	if (etherType == htons(0x8100)) {
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}
    	/*parse if vlan exists*/
	if (etherType == htons(0x8100)) {
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	/*check whether PPP packets*/
	if (etherType == htons(0x8864)) {
		/* skip pppoe head */
		cp += 6; 					/* 6: PPPoE header 2: PPP protocol */
		/* get ppp protocol */
		pppProtocol = *(ushort *) cp;
		/* check if LCP protocol and ipcpv6 protocol */
		if ((pppProtocol == htons(0xc021)) || (pppProtocol == htons(0x8021)) || (pppProtocol == htons(0x8057)) 
			|| (pppProtocol == htons(0xc223)) || (pppProtocol == htons(0xc057))) {
			return 1;
		/* check if IPv6 protocol */
		} else if (pppProtocol == htons(0x0057)) {
			cp += 2;
			cp += 6;
			/* get ip protocol */
			ipProtocol = *(unchar*)cp;
			ipVerLen = 0;
			cp += 34;
         
			if (ipProtocol == 0x3a) {
    				ipv6_type = *(unchar*)cp;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCC)  
                		if(ipv6_type == 130){ 
                    			return -1;// lan/wifi to wan mld query will drop
                		} 
#endif/*TCSUPPORT_COMPILE*/

			}

			goto ipv6_header;
		/* check if IP protocol */
		} else if (pppProtocol != htons(0x0021)) {
			return 0;
		}
		/* skip ppp protocol */
		cp += 2; 					/* 6: PPPoE header 2: PPP protocol */
	} else if (etherType == htons(0x8863)) {
		return 1;
	/*check whether arp packet*/	
	} else if (etherType == htons(0x0806)) {
		return 1;	
	} 
	else if (etherType == htons(0x86dd)) {
		cp += 6;
         
		ipv6_protocol = *(unchar*)cp;
		cp += 34;
		if (ipv6_protocol == 0x3a) {
			ipv6_type = *(unchar*)cp;
 #if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCC)
			if(ipv6_type == 130){ 
				return -1;// lan/wifi to wan mld query will drop
			}
 #endif/*TCSUPPORT_COMPILE*/
		}
		/* get ip protocol */
		ipProtocol = ipv6_protocol;
		if(ipProtocol == 0x11) /* udp + dns */
		{
			if(  *(ushort *)(cp + 2) == htons(0x0035) )
				return 1;
		}
		ipVerLen = 0;
		goto ipv6_header;
	}
	else {
		/* check if ip packet */
		if (etherType != htons(0x0800)) {
			return 0;
		}
	}

ipv4_header:

	/*  frank add 20121220 for pppoa mode */
	pppProtocol = *(ushort *) cp;
	/* check if LCP protocol, for pppoa control packet */
	if (pppProtocol == htons(0xc021)) {
		return 1;
	} else if(pppProtocol == htons(0x0021)) {
		cp += 2; 					/* 6: PPPoE header 2: PPP protocol */
	}

	/* check if it is a ipv4 packet */
	ipVerLen = *cp;
	if ((ipVerLen & 0xf0) != 0x40) {
		return 0;
	}

	/* get ip protocol */
	ipProtocol = *(cp + 9);

	if(ipProtocol == 0x11) /* udp + dns */
	{
		if( *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) + 2) == htons(0x0035) )
			return 1;
	}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCC)
    /*check if IGMP protocol */
    if(ipProtocol == 2)
    {
        ih = (struct igmphdr *)(cp+20);
        if(ih->type == IGMP_HOST_MEMBERSHIP_QUERY)
            return -1;   // lan/wifi to wan igmp query will drop
        else
            return 1;
    }
	/* check if ICMP*/
	if (ipProtocol == 1) {
        return 1;
	}
#else/*TCSUPPORT_COMPILE*/
    if((ipProtocol == 2) || (ipProtocol == 1))
        return 1;
#endif/*TCSUPPORT_COMPILE*/
ipv6_header:

	if (ipv6_type ==ICMPV6_ROUTE_SOL || 
		ipv6_type ==ICMPV6_ROUTE_ADV ||
		ipv6_type ==ICMPV6_NEIGH_SOL ||
		ipv6_type ==ICMPV6_NEIGH_ADV) {
		return 1;
	}

#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT	
	/*vip packet , go ring1 */
	if( (ipProtocol == 6) ||  (ipProtocol == 17) )//tcp or udp
	{
		/*get source port and dest port*/
		sport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) );
		dport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) + 2);
		for( i = 0 ; i < qdma_vip_num ; i++ )
		{
			if( ipProtocol != qdma_vip_info[i].ip_type )/*compare ip protocol*/
				continue ;
			if( qdma_vip_info[i].sport_en && qdma_vip_info[i].dport_en )
			{
				if( qdma_vip_info[i].swap_en )
				{
					if( ((sport == qdma_vip_info[i].sport) && (dport == qdma_vip_info[i].dport))    /*swap compare*/
						|| ((sport == qdma_vip_info[i].dport) && (dport == qdma_vip_info[i].sport)) )
						return 1 ;
				}
				else
				{
					if( (sport == qdma_vip_info[i].sport) && (dport == qdma_vip_info[i].dport) ) /*compare source and dest port*/
						return 1 ;
				}
			}
			else if( qdma_vip_info[i].sport_en && (sport == qdma_vip_info[i].sport) )/*only compare source port*/
			{
				return 1 ;
			}
			else if( qdma_vip_info[i].dport_en && (dport == qdma_vip_info[i].dport) )/*only compare dest port*/
			{
				return 1 ;
			}
		}
	}
#endif
	if( ipProtocol == 17 )//udp
	{
		/*get source port and dest port*/
		sport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) );
		dport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) + 2);
		
		if(sport ==  htons(0x0223) && dport== htons(0x0222) ){
			//printk("lalala dhcpv6 adver or reply to first queue.\n"); 
			return 1;		
		}
		if(sport ==  htons(0x0222) && dport ==  htons(0x0223) ){
			//printk("lalala dhcpv6 request or soclit to first queue.\n"); 
			return 1;
		}
	}


	/* check if TCP protocol */
	if (ipProtocol != 6) {
		return 0;
	}

	/* align to TCP header */
	cp += (ipVerLen & 0x0f) << 2;
	/* get TCP flags */
	tcpFlags = *(cp + 13);
	
#ifdef CONFIG_TP_IMAGE
	/* check if TCP fin/syn/reset/ack */
	if (((tcpFlags & 0x01) == 0x01) || ((tcpFlags & 0x02) == 0x02) || ((tcpFlags & 0x04) == 0x04) || ((tcpFlags & 0x10) == 0x10)) {
#else /* CONFIG_TP_IMAGE */
	/* check if TCP fin/syn/reset */
	if (((tcpFlags & 0x01) == 0x01) || ((tcpFlags & 0x02) == 0x02) || ((tcpFlags & 0x04) == 0x04)) {
#endif /* CONFIG_TP_IMAGE */
		return 1;
	}


	//upstream cloudvr for queue-5, upstream ack for queue-6
#if defined(TCSUPPORT_CT_JOYME4)
	if( NATv6_USE_HIGH_PRIORITY && ((tcpFlags & 0x10) == 0x10) ) {
		if(((skb->mark & QOS_FILTER_MARK) >> 4) > 2){
			skb->mark = (skb->mark & (~QOS_FILTER_MARK)) | 0x20;
		}
	}
#endif

    /* check if TCP ack */
	if((qos_flag == NULLQOS) && ((tcpFlags & 0x10) == 0x10)) {
		return 2;
	}
	return 0;
}
#endif

int qdma_get_ringIdx(struct sk_buff *skb, struct port_info *pMacInfo)
{
    int ringIdx = RING_IDX_0;
 
#ifndef TCSUPPORT_FE_VIP_ENHANCEMENT
    if (priPktChk && (skb->len < priPktChkLen)) {   /*add "len" to check tcp packet*/
#endif
        ringIdx = isPriorityPkt(skb, skb->data, pMacInfo);
        if((ringIdx == 1) && (atomic_read(&txRing1FreeCnt) >= txring1ReserveThreshold)){
            QDMA_MSG(DBG_MSG, "cat a VIP packet which will send to TX1 and queue7.\n") ;
            ringIdx = RING_IDX_1;
        } else {
            ringIdx = RING_IDX_0;
        }
#ifndef TCSUPPORT_FE_VIP_ENHANCEMENT
    }
#endif
    return ringIdx;
}

/******************************************************************************
******************************************************************************/
int qdma_qos_mark(struct sk_buff *skb)
{
	int priority = 0;
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	unchar queue = 0;
#endif
    
	switch (qos_flag) {
		case QOS_SW_PQ:
			/* PQ mode */
			if ((skb->mark & QOS_FILTER_MARK) == QOS_HH_PRIORITY) {
				priority = 2;
			}
			else if ((skb->mark & QOS_FILTER_MARK) == QOS_H_PRIORITY) {
				priority = 1;
			}
			break;
		case QOS_SW_CAR:
			/* SW CAR mode */
			if(isDefaultQueue(skb)){		//default queue
				priority = 0;
			}else{
				priority = (skb->mark & QOS_FILTER_MARK)>> 4;
			}
			break;
		case QOS_HW_WRR:
			/* HW WRR mode */			
	#if defined(INCLUDE_ITTIM)
			if (QOS_ROUTER_VOIP == (skb->mark & QOS_ROUTER_MASK))
			{
				/* voip skb->mark is 0x61, highest priority */
				return 3;
			}
	#endif /* INCLUDE_ITTIM */

			if(isDefaultQueue(skb)){ 	/* default queue */
				priority = 0;
			}else if(isWeigt0(skb)){
				return -EFAULT;
			}else
			if ((skb->mark & QOS_FILTER_MARK) == QOS_HH_PRIORITY) {
				/* printk("lalala to first queue.\n"); */
				priority = 3;
			}
			else if ((skb->mark & QOS_FILTER_MARK) == QOS_H_PRIORITY) {
				priority = 2;
			}
			else if ((skb->mark & QOS_FILTER_MARK) == QOS_M_PRIORITY) {
				priority = 1;
			}
			else {
				priority = 0;
			}
			break;
		case QOS_HW_PQ:
			/* HW PQ mode */
	#if defined(INCLUDE_ITTIM)
			if (QOS_ROUTER_VOIP == (skb->mark & QOS_ROUTER_MASK))
			{
				/* voip skb->mark is 0x61, highest priority */
				return 3;
			}
	#endif /* INCLUDE_ITTIM */

			if(isDefaultQueue(skb)){ 	/* default queue */
				priority = 0;
			}else
			if ((skb->mark & QOS_FILTER_MARK) == QOS_HH_PRIORITY) {
				priority = 3;
			}
			else if ((skb->mark & QOS_FILTER_MARK) == QOS_H_PRIORITY) {
				priority = 2;
			}
			else if ((skb->mark & QOS_FILTER_MARK) == QOS_M_PRIORITY) {
				priority = 1;
			}
			break;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS_CA) 
		case QOS_HW_CAR:			
			if(isDefaultQueue(skb)){		//default queue
				priority = 0;
			}else{
				priority = (skb->mark & QOS_FILTER_MARK)>> 4;
			}
			break;
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
		case QOS_HW_WRR_8_QUEUE:
	#if defined(INCLUDE_ITTIM)
			if (QOS_ROUTER_VOIP == (skb->mark & QOS_ROUTER_MASK))
			{
				/* voip skb->mark is 0x61, highest priority */
				return 7;
			}
	#endif /* INCLUDE_ITTIM */

			if(isWeigt0(skb)){
				return -EFAULT;
			}
		case QOS_HW_PQ_8_QUEUE:
	#if defined(INCLUDE_ITTIM)
			if (QOS_ROUTER_VOIP == (skb->mark & QOS_ROUTER_MASK))
			{
				/* voip skb->mark is 0x61, highest priority */
				return 7;
			}
	#endif /* INCLUDE_ITTIM */

			if(isDefaultQueue(skb)){ 	/* default queue */
				return priority;
			}
			queue = (skb->mark & QOS_FILTER_MARK)>>4;
			if((0<queue)&&(queue<=8))
				priority = 8 - queue;
			
			break;
		case QOS_HW_PQ_CIRPIR_8_QUEUE:
			if(isDefaultQueue(skb)){ 	/* default queue */
				return 0;
			}
			queue = (skb->mark & QOS_FILTER_MARK)>>4;
			if((0<queue)&&(queue<=8))
				priority = queue-1;
			
			break;
#endif
		case NULLQOS:/*It's for putting rtp packets to HH priority when qos_flag not be selected as WRR or PQ*/
			if ((skb->mark & QOS_FILTER_MARK) == QOS_HH_PRIORITY) {
				priority = 3;
			}
			break;

		default:
			break;
	}
    
    return priority;
}


#else
int qdma_qos_remark(struct sk_buff *skb)
{
	int priority = -1 ;
    
    if((skb->mark & QOS_REMARKING_FLAG)){
		priority = (int)((skb->mark & QOS_REMARKING_MASK) >> 1);
	}
        
    return priority;
}
#endif

/******************************************************************************
******************************************************************************/
__IMEM int macResourceLimit(struct sk_buff *skb, uint channel, uint queue)
{
    if((atomic_read(&chnlLimit[channel]) >= channel_limit_threshold) || (atomic_read(&queueLimit[channel][queue]) >= queue_limit_threshold)) {
        gpQdmaPriv->counters.txMacLimitDropCnt++;
        return -EFAULT;
    }
    return 0;
}

__IMEM int qdmaEnableInt(uint base, uint bit, QDMA_InterruptNo_t intIdx,QDMA_IntEnable_t enableIdx)
{
	ulong flags=0 ;

#if SUPPORT_INT_ENABLE_NUM_2 /*intNo 1-4, enNo 1-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<1 || enableIdx>QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:1~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#else /*intNo 1-4, enNo 0-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<0 || enableIdx>=QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:0~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#endif
	
	spin_lock_irqsave(&gpQdmaPriv->irqLock, flags) ;
	IO_SBITS(QDMA_CSR_INT_EN(base, intIdx, enableIdx), bit);
	spin_unlock_irqrestore(&gpQdmaPriv->irqLock, flags) ;
	
	return 0 ;
}

__IMEM int qdmaDisableInt(uint base, uint bit, QDMA_InterruptNo_t intIdx,QDMA_IntEnable_t enableIdx)
{
	ulong flags=0 ;

#if SUPPORT_INT_ENABLE_NUM_2 /*intNo 1-4, enNo 1-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<1 || enableIdx>QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:1~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#else /*intNo 1-4, enNo 0-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<0 || enableIdx>=QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:0~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#endif

	
	spin_lock_irqsave(&gpQdmaPriv->irqLock, flags) ;
	IO_CBITS(QDMA_CSR_INT_EN(base, intIdx, enableIdx),bit);
	spin_unlock_irqrestore(&gpQdmaPriv->irqLock, flags) ;
	
	return 0 ;
}
int qdmaSetIntMask(uint base, QDMA_InterruptNo_t intIdx, QDMA_IntEnable_t enableIdx, uint value)
{
	ulong flags=0 ;

#if SUPPORT_INT_ENABLE_NUM_2 /*intNo 1-4, enNo 1-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<1 || enableIdx>QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:1~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#else /*intNo 1-4, enNo 0-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<0 || enableIdx>=QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:0~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#endif


	spin_lock_irqsave(&gpQdmaPriv->irqLock, flags) ;
	IO_SREG(QDMA_CSR_INT_EN(base, intIdx, enableIdx), value) ;
	spin_unlock_irqrestore(&gpQdmaPriv->irqLock, flags) ;

	return 0 ;
}

/******************************************************************************
******************************************************************************/
__IMEM int qdmaGetIntMask(uint base,  QDMA_InterruptNo_t intIdx, QDMA_IntEnable_t enableIdx)
{
	ulong flags=0, value=0 ;

#if SUPPORT_INT_ENABLE_NUM_2 /*intNo 1-4, enNo 1-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<1 || enableIdx>QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:1~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#else /*intNo 1-4, enNo 0-2*/
	if(intIdx<=0 || intIdx>=QDMA_INT_NUM || enableIdx<0 || enableIdx>=QDMA_INT_ENABLE_NUM){
		printk("qdmaEnableInt Error: ParaMeter should intIdx 1~%d, enableIdx:0~%d\n", QDMA_INT_NUM, QDMA_INT_ENABLE_NUM);
		return -EINVAL;
	}
#endif

		
	spin_lock_irqsave(&gpQdmaPriv->irqLock, flags) ;
	value = IO_GREG(QDMA_CSR_INT_EN(base, intIdx, enableIdx)) ;
	spin_unlock_irqrestore(&gpQdmaPriv->irqLock, flags) ;
	
	return value ;
}

/******************************************************************************
******************************************************************************/
int qdmaSetIntBind(uint base, QDMA_InterruptNo_t intIdx)
{
	uint intEnable=0,intEnable1=0,intEnable2=0,intEnable3=0,intEnable4=0,intEnable5=0 ;

    switch(intIdx){
        case QDMA_INT1 :/*set INT-1*/
#if defined(TCSUPPORT_CPU_EN7581)
			intEnable1 = ( INT1_MASK_RX_COHERENT_LOW_16_RING | INT1_MASK_TX_COHERENT_LOW_8_RING |
							INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT) ;
			intEnable2 = ( INT1_MASK_NO_RX_CPU_DSCP_LOW_16_RING | INT1_MASK_RX_DONE_LOW_16_RING );
			intEnable3 = ( INT1_MASK_NO_RX_CPU_DSCP_HIGH_16_RING | INT1_MASK_RX_DONE_HIGH_16_RING );
			intEnable4 = ( INT1_MASK_RX_COHERENT_HIGH_16_RING );
			intEnable5 = ( INT1_MASK_TX_COHERENT_HIGH_24_RING );

        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, intEnable2) ;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3, intEnable3) ;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4, intEnable4) ;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5, intEnable5) ;
#elif defined(TCSUPPORT_CPU_EN7580)
#if defined(TCSUPPORT_CPU_EN7523)
#if 0 /* IRQ2 will be handled by NPU */
			intEnable1 = ( INT1_MASK_RX_COHERENT | INT1_MASK_TX_COHERENT |
							INT_MASK_IRQ2_FULL | INT_MASK_IRQ2_INT | INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT) ;
#else
			intEnable1 = ( INT1_MASK_RX_COHERENT | INT1_MASK_TX_COHERENT |
							INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT) ;
#endif
#else
        	intEnable1 = ( INT1_MASK_RX_COHERENT | INT1_MASK_TX_COHERENT |
        	                INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT) ;
#endif
        	intEnable2 = ( INT1_MASK_NO_RX_CPU_DSCP | INT1_MASK_RX_DONE );
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, intEnable2) ;
#else
            intEnable = (   INT1_MASK_RX_DSCP_LOW_0_1 |INT1_MASK_RX_COHERENT_0_1|
        					INT_MASK_TX0_COHERENT | INT_MASK_TX1_COHERENT | 
                            INT_MASK_RX_PKT_OVERFLOW | INT_MASK_IRQ_FULL | 
        	                INT_MASK_TX1_DONE |INT_MASK_TX0_DONE|
        	                INT1_MASK_NO_RX_CPU_DSCP_0_1 | INT1_MASK_RX_DONE_0_1 ) ;
        	intEnable1 = INT1_MASK_NO_RX_CPU_DSCP_2_15 | INT1_MASK_RX_DONE_2_15; 
        	intEnable2 = INT1_MASK_RX_DSCP_LOW_2_15 | INT1_MASK_RX_COHERENT_2_15;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE0, intEnable) ;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, intEnable2) ;
#endif
            break;

        case QDMA_INT2 :/*set INT-2*/			
#if defined(TCSUPPORT_CPU_EN7581)
			intEnable1 = ( INT2_MASK_RX_COHERENT_LOW_16_RING ) ;
			intEnable2 = ( INT2_MASK_NO_RX_CPU_DSCP_LOW_16_RING | INT2_MASK_RX_DONE_LOW_16_RING );
			intEnable3 = ( INT2_MASK_NO_RX_CPU_DSCP_HIGH_16_RING | INT2_MASK_RX_DONE_HIGH_16_RING );
			intEnable4 = ( INT2_MASK_RX_COHERENT_HIGH_16_RING );

			qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, intEnable1) ;
			qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE2, intEnable2) ;
			qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE3, intEnable3) ;
			qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE4, intEnable4) ;
			qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE5, intEnable5) ;

#elif defined(TCSUPPORT_CPU_EN7580)
            intEnable1 = ( INT2_MASK_RX_COHERENT ) ;
        	intEnable2 = ( INT2_MASK_NO_RX_CPU_DSCP | INT2_MASK_RX_DONE );
        	qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE2, intEnable2) ;    
#else
            intEnable = (   INT2_MASK_RX_DSCP_LOW_0_1 |INT2_MASK_RX_COHERENT_0_1|
	                        INT2_MASK_NO_RX_CPU_DSCP_0_1 | INT2_MASK_RX_DONE_0_1 ) ;
        	intEnable1 = INT2_MASK_NO_RX_CPU_DSCP_2_15 | INT2_MASK_RX_DONE_2_15; 
        	intEnable2 = INT2_MASK_RX_DSCP_LOW_2_15 | INT2_MASK_RX_COHERENT_2_15;
        	qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE0, intEnable) ;
        	qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE2, intEnable2) ;
#endif
            break;

        case QDMA_INT3 :/*set INT-3*/
#if defined(TCSUPPORT_CPU_EN7581)
			intEnable1 = ( INT3_MASK_RX_COHERENT_LOW_16_RING ) ;
			intEnable2 = ( INT3_MASK_NO_RX_CPU_DSCP_LOW_16_RING | INT3_MASK_RX_DONE_LOW_16_RING );
			intEnable3 = ( INT3_MASK_NO_RX_CPU_DSCP_HIGH_16_RING | INT3_MASK_RX_DONE_HIGH_16_RING );
			intEnable4 = ( INT3_MASK_RX_COHERENT_HIGH_16_RING );

			qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, intEnable1) ;
			qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2, intEnable2) ;
			qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE3, intEnable3) ;
			qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE4, intEnable4) ;
			qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE5, intEnable5) ;

#elif defined(TCSUPPORT_CPU_EN7580)
            intEnable1 = ( INT3_MASK_RX_COHERENT ) ;
        	intEnable2 = ( INT3_MASK_NO_RX_CPU_DSCP | INT3_MASK_RX_DONE );
        	qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2, intEnable2) ;
#else
            intEnable = (   INT3_MASK_RX_DSCP_LOW_0_1 |INT3_MASK_RX_COHERENT_0_1|
	                        INT3_MASK_NO_RX_CPU_DSCP_0_1 | INT3_MASK_RX_DONE_0_1 ) ;
        	intEnable1 = INT3_MASK_NO_RX_CPU_DSCP_2_15 | INT3_MASK_RX_DONE_2_15; 
        	intEnable2 = INT3_MASK_RX_DSCP_LOW_2_15 | INT3_MASK_RX_COHERENT_2_15;
        	qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE0, intEnable) ;
        	qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2, intEnable2) ;
#endif
            break;

        case QDMA_INT4 :/*set INT-4*/
#if defined(TCSUPPORT_CPU_EN7581)
			intEnable1 = ( INT4_MASK_RX_COHERENT_LOW_16_RING ) ;
			intEnable2 = ( INT4_MASK_NO_RX_CPU_DSCP_LOW_16_RING | INT4_MASK_RX_DONE_LOW_16_RING );
			intEnable3 = ( INT4_MASK_NO_RX_CPU_DSCP_HIGH_16_RING | INT4_MASK_RX_DONE_HIGH_16_RING );
			intEnable4 = ( INT4_MASK_RX_COHERENT_HIGH_16_RING );

			qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, intEnable1) ;
			qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2, intEnable2) ;
			qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE3, intEnable3) ;
			qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE4, intEnable4) ;
			qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE5, intEnable5) ;

#elif defined(TCSUPPORT_CPU_EN7580)
            intEnable1 = ( INT4_MASK_RX_COHERENT ) ;
        	intEnable2 = ( INT4_MASK_NO_RX_CPU_DSCP | INT4_MASK_RX_DONE );
        	qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2, intEnable2) ;
#else
            intEnable = (   INT4_MASK_RX_DSCP_LOW_0_1 |INT4_MASK_RX_COHERENT_0_1|
	                        INT4_MASK_NO_RX_CPU_DSCP_0_1 | INT4_MASK_RX_DONE_0_1 ) ;
        	intEnable1 = INT4_MASK_NO_RX_CPU_DSCP_2_15 | INT4_MASK_RX_DONE_2_15; 
        	intEnable2 = INT4_MASK_RX_DSCP_LOW_2_15 | INT4_MASK_RX_COHERENT_2_15;
        	qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE0, intEnable) ;
        	qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, intEnable1) ;
        	qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2, intEnable2) ;
#endif
            break;

        default:
            QDMA_ERR("error INT index! \r\n");
            break;
    }
    
    return 0 ;
}

/******************************************************************************
******************************************************************************/
int qdmaSetIrqBind(uint base, QDMA_IrqNo_t intIdx)
{
	uint ring_idx = 0 ;

    switch(intIdx){
        case QDMA_IRQ1 :/*set IRQ-1*/
#if SUPPORT_IRQ2
			for(ring_idx=0 ; ring_idx<TX_RING_NUM ; ring_idx++){
        		if( (irq_tx_ring & (1<<ring_idx)) == 0 ){
					qdmaSetCpuTxBindIRQ1(base, ring_idx) ;
        		}
    		}
#endif
            break;

        case QDMA_IRQ2 :/*set IRQ-2*/
#if SUPPORT_IRQ2
			for(ring_idx=0 ; ring_idx<TX_RING_NUM ; ring_idx++){
				if( (irq_tx_ring & (1<<ring_idx)) != 0 ){
					qdmaSetCpuTxBindIRQ2(base, ring_idx) ;
				}
			}
#endif
			break;

        default:
            QDMA_ERR("error IRQ index! \r\n");
            break;
    }
    
    return 0 ;
}

/******************************************************************************
******************************************************************************/
void qdma_dma_busy_timer(TIMER_FUN_PAAM arg)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint qdmaGlbCfgVal = qdmaGetGlbCfg(base) ;
	
	printk("%04x: dma busy register is 0x%08x\r\n", dma_busy_round_cnt, qdmaGlbCfgVal);

	set_timer_expires(&dma_busy_timer, dma_busy_expires);
	dma_busy_timer.function = qdma_dma_busy_timer;

	if(qdmaGlbCfgVal & GLB_CFG_TX_DMA_BUSY)
		tx_dma_busy_enable_cnt++ ;
	if(qdmaGlbCfgVal & GLB_CFG_RX_DMA_BUSY)
		rx_dma_busy_enable_cnt++ ;	
	
	if(dma_busy_round_cnt--) {
		QDMA_START_TIMER(dma_busy_timer) ;
	} else {
		printk("\r\ntx_busy enable cnt is %d, rx_busy enable cnt is %d\r\n", tx_dma_busy_enable_cnt, rx_dma_busy_enable_cnt);		
	}
}

/******************************************************************************
******************************************************************************/
void qdma_trigger_timer(TIMER_FUN_PAAM arg)
{
    struct ECNT_QDMA_Data qdma_data;
    QDMA_TxQosScheduler_T txQos ;
    int q_idx = 0 ;

    if(trigger_timer_going == 0)
        return ;
                
    txQos.channel = 0 ;
    txQos.qosType = QDMA_TXQOS_TYPE_WRR ;
    for(q_idx=0 ; q_idx<CONFIG_QDMA_QUEUE ; q_idx++) {
        txQos.queue[q_idx].weight = (random32() % 99)+1 ;
    }
    qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
    qdma_set_tx_qos(&qdma_data) ;

	set_timer_expires(&trigger_timer, trigger_expires);
	trigger_timer.function = qdma_trigger_timer;
    QDMA_START_TIMER(trigger_timer) ;

}

/******************************************************************************
******************************************************************************/
void qdma_reg_polling_timer(TIMER_FUN_PAAM arg)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint qdmaRegVal = qdmaGetOffsetVal(base, reg_polling_offset) ;
	static int i=0;
	set_timer_expires(&reg_polling_timer, reg_polling_expires);
	reg_polling_timer.function = qdma_reg_polling_timer;
	
	qdmaRegValBuff[i++] = qdmaRegVal;
	if(--reg_polling_round_cnt) {
		if(i==100) {
			while(i>0) {
				i--;
				printk("\r\n%04x: qdma_reg_value[%x]: 0x%08x", reg_polling_round_cnt+i, QDMA_CSR_OFFSET_ADDR(base, reg_polling_offset), qdmaRegValBuff[i]);
			}
		}
		QDMA_START_TIMER(reg_polling_timer) ;
	} else {
		while(i>0) {
			i--;
			printk("\r\n%04x: qdma_reg_value[%x]: 0x%08x", reg_polling_round_cnt+i, QDMA_CSR_OFFSET_ADDR(base, reg_polling_offset), qdmaRegValBuff[i]);
		}
	}
}

#ifdef TCSUPPORT_DRIVER_API
/***************************************************************
***************************************************************/
#ifdef TCSUPPORT_CPU_ARMV8
static long qdma_ioctl(struct file *filp, uint cmd, void *arg) 
{
	int ret = 0 ;
	struct ECNT_QDMA_Data qdma_data;
	QDMA_TxQosScheduler_T pTxQos;
	GENERAL_TrtcmCfg_T generalTrtcmCfg;
	GENERAL_TrtcmRatelimitCfg_T generalTrtcmRatelimitCfg;
	GENERAL_TrtcmRatelimitSet_T generalTrtcmRatelimitSet;
	GENERAL_TrtcmCbsPbsCfg_T generalTrtcmCbsPbsCfg;
	GENERAL_TrtcmCbsPbsSet_T generalTrtcmCbsPbsSet;
	QDMA_FlowCntIncDropCfg_T flowCntIncDropCfg;
	QDMA_FlowCntValueGet_T flowCntValueGet;
	QDMA_FlowCntValueClear_T flowCntValueClear;
	QDMA_TxWredCfg_T txWredCfg;
	QDMA_RxRedCfg_T rxRedCfg;
	QDMA_SlaChnlCfg_T slaChnlCfg;
	QDMA_QoSAgingCfg_T qosAgingCfg;
	QDMA_TxQAgingCfg_T txqAgingCfg;
	QDMA_MulticastSptagCfg_T multicastSptagCfg;
	QDMA_MulticastFportCfg_T multicastFportCfg;
	QDMA_TxQStaticCfg_T staticThrhCfg;
	QDMA_TxQStaticThrRatio_T staticThrhRatio;
	QDMA_RxLowThreshold_T rxLowThreshold;
	QDMA_TxRateMeter_T txRateMeter;
	QDMA_TxRateLimitCfg_T txRateLimitCfg;
	QDMA_TxRateLimitSet_T txRateLimit;
	QDMA_TxDbaReport_T txDbaReport;
	QDMA_RxRateLimitGet_T rxRateLimit;
	QDMA_RxRateLimitCfg_T rxRateLimitCfg;
	QDMA_VirtualChannelMode_T virChnlMode;
	QDMA_VirtualChannelQoS_T virChnlQoS;
	QDMA_MacLimitThrh_T qdma_mac_limit_thrh;
	QDMA_TxBufCtrl_T pTxBufCtrl;
	QDMA_PrefetchMode_T qdma_prefetch;
	QDMA_LmgrStatus_T lmgr;
	QDMA_txCngstCfg_t pTxCngst;
	QDMA_TxQDynCngstTotalThrh_T totalThrh;
	QDMA_TxQDynCngstChnlThrh_T chnlThrh;
	QDMA_TxQDynCngstQueueThrh_T queueThrh;
	QDMA_PeekRateCfg_t peekrateCfg;
	QDMA_TxQStaticNormalCfg_T normThrh;
	QDMA_TxQStaticDeiCfg_T deiThrh;
	QDMA_MacQoSCfg_T macQosCfg;
	
	if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
    {
		return -EFAULT;
	}

	switch(qdma_data.function_id){
		case QDMA_FUNCTION_SET_TX_QOS:
		case QDMA_FUNCTION_GET_TX_QOS:
			if( copy_from_user(&pTxQos, qdma_data.qdma_private.qdma_tx_qos.pTxQos, sizeof(QDMA_TxQosScheduler_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_tx_qos.pTxQos = &pTxQos;
			break;
		case QDMA_FUNCTION_SET_MAC_LIMIT_THRESHOLD:
		case QDMA_FUNCTION_GET_MAC_LIMIT_THRESHOLD:
			if( copy_from_user(&qdma_mac_limit_thrh, qdma_data.qdma_private.qdma_mac_limit_thrh, sizeof(QDMA_MacLimitThrh_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_mac_limit_thrh = &qdma_mac_limit_thrh;
			break;
		case QDMA_FUNCTION_SET_TXBUF_THRESHOLD:
		case QDMA_FUNCTION_GET_TXBUF_THRESHOLD:
			if( copy_from_user(&pTxBufCtrl, qdma_data.qdma_private.pQdmaTxBufCtrl, sizeof(QDMA_TxBufCtrl_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.pQdmaTxBufCtrl = &pTxBufCtrl;
			break;
		case QDMA_FUNCTION_SET_PREFETCH_MODE:
			if( copy_from_user(&qdma_prefetch, qdma_data.qdma_private.qdma_prefetch, sizeof(QDMA_PrefetchMode_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_prefetch = &qdma_prefetch;
			break;
		case QDMA_FUNCTION_SET_TX_RATEMETER:
		case QDMA_FUNCTION_GET_TX_RATEMETER:
			if( copy_from_user(&txRateMeter, qdma_data.qdma_private.txRateMeterPtr, sizeof(QDMA_TxRateMeter_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.txRateMeterPtr = &txRateMeter;
			break;
		case QDMA_FUNCTION_SET_TX_RATELIMIT_CFG:
		case QDMA_FUNCTION_GET_TX_RATELIMIT_CFG:
			if( copy_from_user(&txRateLimitCfg, qdma_data.qdma_private.txRateLimitCfgPtr, sizeof(QDMA_TxRateLimitCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.txRateLimitCfgPtr = &txRateLimitCfg;
			break;
		case QDMA_FUNCTION_SET_TX_RATELIMIT:
		case QDMA_FUNCTION_GET_TX_RATELIMIT:
			if( copy_from_user(&txRateLimit, qdma_data.qdma_private.txRateLimitSetPtr, sizeof(QDMA_TxRateLimitSet_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.txRateLimitSetPtr = &txRateLimit;
			break;
		case QDMA_FUNCTION_SET_TX_DBAREPORT:
		case QDMA_FUNCTION_GET_TX_DBAREPORT:
			if( copy_from_user(&txDbaReport, qdma_data.qdma_private.txDbaReportPtr, sizeof(QDMA_TxDbaReport_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.txDbaReportPtr = &txDbaReport;
			break;
		case QDMA_FUNCTION_SET_RX_LOW_THRESHOLD:
		case QDMA_FUNCTION_GET_RX_LOW_THRESHOLD:
			if( copy_from_user(&rxLowThreshold, qdma_data.qdma_private.rxLowThresholdPtr, sizeof(QDMA_RxLowThreshold_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.rxLowThresholdPtr = &rxLowThreshold;
			break;
		case QDMA_FUNCTION_GET_LMGR_STATUS:
			if( copy_from_user(&lmgr, qdma_data.qdma_private.qdma_lmgr_status, sizeof(QDMA_LmgrStatus_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_lmgr_status = &lmgr;
			break;	
		case QDMA_FUNCTION_GET_RX_RATELIMIT_CFG:
			if( copy_from_user(&rxRateLimitCfg, qdma_data.qdma_private.rxRateLimitCfgPtr, sizeof(QDMA_RxRateLimitCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.rxRateLimitCfgPtr = &rxRateLimitCfg;
			break;
		case QDMA_FUNCTION_SET_RX_RATELIMIT:
		case QDMA_FUNCTION_GET_RX_RATELIMIT:
			if( copy_from_user(&rxRateLimit, qdma_data.qdma_private.rxRateLimitGetPtr, sizeof(QDMA_RxRateLimitGet_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.rxRateLimitGetPtr = &rxRateLimit;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_AUTO_CONFIG:
			if( copy_from_user(&pTxCngst, qdma_data.qdma_private.pTxCngstCfg, sizeof(QDMA_txCngstCfg_t)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.pTxCngstCfg = &pTxCngst;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_TOTAL_THRESHOLD:
			if( copy_from_user(&totalThrh, qdma_data.qdma_private.totalThrhPtr, sizeof(QDMA_TxQDynCngstTotalThrh_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.totalThrhPtr = &totalThrh;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_CHANNEL_THRESHOLD:
			if( copy_from_user(&chnlThrh, qdma_data.qdma_private.chnlThrhPtr, sizeof(QDMA_TxQDynCngstChnlThrh_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.chnlThrhPtr = &chnlThrh;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_QUEUE_THRESHOLD:
			if( copy_from_user(&queueThrh, qdma_data.qdma_private.queueThrhPtr, sizeof(QDMA_TxQDynCngstQueueThrh_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.queueThrhPtr = &queueThrh;
			break;
		case QDMA_FUNCTION_SET_PEEKRATE_PARAMS:
			if( copy_from_user(&peekrateCfg, qdma_data.qdma_private.peekrateCfgPtr, sizeof(QDMA_PeekRateCfg_t)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.peekrateCfgPtr = &peekrateCfg;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_NORMAL_THRESHOLD:
			if( copy_from_user(&normThrh, qdma_data.qdma_private.normThrhPtr, sizeof(QDMA_TxQStaticNormalCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.normThrhPtr = &normThrh;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_DEI_THRESHOLD:
			if( copy_from_user(&deiThrh, qdma_data.qdma_private.deiThrhPtr, sizeof(QDMA_TxQStaticDeiCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.deiThrhPtr = &deiThrh;
			break;
		case QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_MODE:
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_MODE:
			if( copy_from_user(&virChnlMode, qdma_data.qdma_private.virChnlModePtr, sizeof(QDMA_VirtualChannelMode_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.virChnlModePtr = &virChnlMode;
			break;
		case QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_QOS:
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_QOS:
			if( copy_from_user(&virChnlQoS, qdma_data.qdma_private.virChnlQoSPtr, sizeof(QDMA_VirtualChannelQoS_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.virChnlQoSPtr = &virChnlQoS;
			break;
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_CFG:
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_CFG:
			if( copy_from_user(&generalTrtcmCfg, qdma_data.qdma_private.generalTrtcmCfgPtr, sizeof(GENERAL_TrtcmCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmCfgPtr = &generalTrtcmCfg;
			break;
		case QDMA_FUNCTION_GENERAL_SET_RATELIMIT_MODE_CFG:
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_CFG:
			if( copy_from_user(&generalTrtcmRatelimitCfg, qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr = &generalTrtcmRatelimitCfg;
			break;
		case QDMA_FUNCTION_GENERAL_SET_RATELIMIT_MODE_VALUE:
		case QDMA_FUNCTION_GENERAL_SET_RATELIMIT_BUCKET_SIZE:
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_VALUE:
			if( copy_from_user(&generalTrtcmRatelimitSet, qdma_data.qdma_private.generalTrtcmRatelimitSetPtr, sizeof(GENERAL_TrtcmRatelimitSet_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmRatelimitSetPtr = &generalTrtcmRatelimitSet;
			break;
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_MODE_CFG:
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_CFG:
			if( copy_from_user(&generalTrtcmCbsPbsCfg, qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr, sizeof(GENERAL_TrtcmCbsPbsCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr = &generalTrtcmCbsPbsCfg;
			break;
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_MODE_VALUE:
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_BUCKET_SIZE:
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_VALUE:
			if( copy_from_user(&generalTrtcmCbsPbsSet, qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr, sizeof(GENERAL_TrtcmCbsPbsSet_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr = &generalTrtcmCbsPbsSet;
			break;
		case QDMA_FUNCTION_SET_FLOW_CNTR_CFG:
		case QDMA_FUNCTION_GET_FLOW_CNTR_CFG:
			if( copy_from_user(&flowCntIncDropCfg, qdma_data.qdma_private.flowCntIncDropCfg, sizeof(QDMA_FlowCntIncDropCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.flowCntIncDropCfg = &flowCntIncDropCfg;
			break;
		case QDMA_FUNCTION_GET_FLOW_CNTR_VALUE:
			if( copy_from_user(&flowCntValueGet, qdma_data.qdma_private.flowCntValueGet, sizeof(QDMA_FlowCntValueGet_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.flowCntValueGet = &flowCntValueGet;
			break;
		case QDMA_FUNCTION_CLEAR_FLOW_CNTR_VALUE:
			if( copy_from_user(&flowCntValueClear, qdma_data.qdma_private.flowCntValueClear, sizeof(QDMA_FlowCntValueClear_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.flowCntValueClear = &flowCntValueClear;
			break;
		case QDMA_FUNCTION_SET_TX_WRED_MODE:
		case QDMA_FUNCTION_GET_TX_WRED_MODE:
		case QDMA_FUNCTION_SET_TX_WRED_THRESHOLD:
		case QDMA_FUNCTION_SET_TX_WRED_PROBABILITY:
		case QDMA_FUNCTION_GET_TX_WRED_CFG:
			if( copy_from_user(&txWredCfg, qdma_data.qdma_private.txWredCfgPtr, sizeof(QDMA_TxWredCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.txWredCfgPtr = &txWredCfg;
			break;

		case QDMA_FUNCTION_SET_CPU_RX_RED_PROBABILITY:
		case QDMA_FUNCTION_GET_CPU_RX_RED_PROBABILITY:
			if( copy_from_user(&rxRedCfg, qdma_data.qdma_private.rxRedCfgPtr, sizeof(QDMA_RxRedCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.rxRedCfgPtr = &rxRedCfg;
			break;
		case QDMA_FUNCTION_SET_SLA_CHANNEL_CFG:
		case QDMA_FUNCTION_GET_SLA_CHANNEL_CFG:
			if( copy_from_user(&slaChnlCfg, qdma_data.qdma_private.slaChnlCfgPtr, sizeof(QDMA_SlaChnlCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.slaChnlCfgPtr = &slaChnlCfg;
			break;
		case QDMA_FUNCTION_SET_QOS_AGING_CFG:
		case QDMA_FUNCTION_GET_QOS_AGING_CFG:
			if( copy_from_user(&qosAgingCfg, qdma_data.qdma_private.qosAgingCfgPtr, sizeof(QDMA_QoSAgingCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.qosAgingCfgPtr = &qosAgingCfg;
			break;
		case QDMA_FUNCTION_SET_PER_QUEUE_AGING_CFG:
		case QDMA_FUNCTION_GET_PER_QUEUE_AGING_CFG:
			if( copy_from_user(&txqAgingCfg, qdma_data.qdma_private.txqAgingCfgPtr, sizeof(QDMA_TxQAgingCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.txqAgingCfgPtr = &txqAgingCfg;
			break;
		case QDMA_FUNCTION_SET_MULTICAST_SPTAG_CFG:
		case QDMA_FUNCTION_GET_MULTICAST_SPTAG_CFG:
			if( copy_from_user(&multicastSptagCfg, qdma_data.qdma_private.multicastSptagCfgPtr, sizeof(QDMA_MulticastSptagCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.multicastSptagCfgPtr = &multicastSptagCfg;
			break;
		case QDMA_FUNCTION_SET_MULTICAST_FPORT_CFG:
		case QDMA_FUNCTION_GET_MULTICAST_FPORT_CFG:
			if( copy_from_user(&multicastFportCfg, qdma_data.qdma_private.multicastFportCfgPtr, sizeof(QDMA_MulticastFportCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.multicastFportCfgPtr = &multicastFportCfg;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_CHANNEL_EN:
			if( copy_from_user(&staticThrhCfg, qdma_data.qdma_private.staticThrhCfgPtr, sizeof(QDMA_TxQStaticCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.staticThrhCfgPtr = &staticThrhCfg;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_RATIO:
			if( copy_from_user(&staticThrhRatio, qdma_data.qdma_private.staticThrhRatioPtr, sizeof(QDMA_TxQStaticThrRatio_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.staticThrhRatioPtr = &staticThrhRatio;
			break;
		case QDMA_FUNCTION_SET_MAC_QOS_CONFIG:
			if( copy_from_user(&macQosCfg, qdma_data.qdma_private.macQosCfg, sizeof(QDMA_MacQoSCfg_T)) ) 
		    {
				return -EFAULT;
			}
			qdma_data.qdma_private.macQosCfg = &macQosCfg;
			break;
			
		default:
			break;
	}

	if(qdma_operation[qdma_data.function_id] == NULL){
		QDMA_MSG(DBG_ST, "qdma_ioctl: NULL, function_id=0x%x\n", qdma_data.function_id);
		return -EFAULT;
	}else{
    	ret = qdma_operation[qdma_data.function_id](&qdma_data);
	}

	switch(qdma_data.function_id){
		case QDMA_FUNCTION_GET_TX_QOS:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.qdma_tx_qos.pTxQos, &pTxQos, sizeof(QDMA_TxQosScheduler_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_SET_MAC_LIMIT_THRESHOLD:
		case QDMA_FUNCTION_GET_MAC_LIMIT_THRESHOLD:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.qdma_mac_limit_thrh, &qdma_mac_limit_thrh, sizeof(QDMA_MacLimitThrh_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_SET_TXBUF_THRESHOLD:
		case QDMA_FUNCTION_GET_TXBUF_THRESHOLD:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.pQdmaTxBufCtrl, &pTxBufCtrl, sizeof(QDMA_TxBufCtrl_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_RATEMETER:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.txRateMeterPtr, &txRateMeter, sizeof(QDMA_TxRateMeter_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_RATELIMIT_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.txRateLimitCfgPtr, &txRateLimitCfg, sizeof(QDMA_TxRateLimitCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_RATELIMIT:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.txRateLimitSetPtr, &txRateLimit, sizeof(QDMA_TxRateLimitSet_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_DBAREPORT:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.txDbaReportPtr, &txDbaReport, sizeof(QDMA_TxDbaReport_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_RX_LOW_THRESHOLD:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.rxLowThresholdPtr, &rxLowThreshold, sizeof(QDMA_RxLowThreshold_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_LMGR_STATUS:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.qdma_lmgr_status, &lmgr, sizeof(QDMA_LmgrStatus_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_RX_RATELIMIT_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.rxRateLimitCfgPtr, &rxRateLimitCfg, sizeof(QDMA_RxRateLimitCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_RX_RATELIMIT:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.rxRateLimitGetPtr, &rxRateLimit, sizeof(QDMA_RxRateLimitGet_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_MODE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.virChnlModePtr, &virChnlMode, sizeof(QDMA_VirtualChannelMode_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_QOS:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.virChnlQoSPtr, &virChnlQoS, sizeof(QDMA_VirtualChannelQoS_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.generalTrtcmCfgPtr, &generalTrtcmCfg, sizeof(GENERAL_TrtcmCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr, &generalTrtcmRatelimitCfg, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_VALUE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.generalTrtcmRatelimitSetPtr, &generalTrtcmRatelimitSet, sizeof(GENERAL_TrtcmRatelimitSet_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr, &generalTrtcmCbsPbsCfg, sizeof(GENERAL_TrtcmCbsPbsCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_VALUE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr, &generalTrtcmCbsPbsSet, sizeof(GENERAL_TrtcmCbsPbsSet_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_FLOW_CNTR_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.flowCntIncDropCfg, &flowCntIncDropCfg, sizeof(QDMA_FlowCntIncDropCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_FLOW_CNTR_VALUE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.flowCntValueGet, &flowCntValueGet, sizeof(QDMA_FlowCntValueGet_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_WRED_MODE:
		case QDMA_FUNCTION_GET_TX_WRED_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.txWredCfgPtr, &txWredCfg, sizeof(QDMA_TxWredCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_CPU_RX_RED_PROBABILITY:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.rxRedCfgPtr, &rxRedCfg, sizeof(QDMA_RxRedCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_SLA_CHANNEL_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.slaChnlCfgPtr, &slaChnlCfg, sizeof(QDMA_SlaChnlCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_QOS_AGING_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.qosAgingCfgPtr, &qosAgingCfg, sizeof(QDMA_QoSAgingCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_PER_QUEUE_AGING_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.txqAgingCfgPtr, &txqAgingCfg, sizeof(QDMA_TxQAgingCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_MULTICAST_SPTAG_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.multicastSptagCfgPtr, &multicastSptagCfg, sizeof(QDMA_MulticastSptagCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_MULTICAST_FPORT_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			if( copy_to_user(qdma_data.qdma_private.multicastFportCfgPtr, &multicastFportCfg, sizeof(QDMA_MulticastFportCfg_T)) ) 
		    {
				return -EFAULT;
			}
			break;

		default:
			if( copy_to_user((struct ECNT_QDMA_Data*)arg, &qdma_data, sizeof(struct ECNT_QDMA_Data)) ) 
		    {
				return -EFAULT;
			}
			break;
	}
	
	return ret;
}
#else
static long qdma_ioctl(struct file *filp, uint cmd, void *arg) 
{
	int ret = 0 ;
	struct ECNT_QDMA_Data qdma_data;
	
	if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)arg, sizeof(struct ECNT_QDMA_Data)) ) 
    {
		return -EFAULT;
	}

	if(qdma_operation[qdma_data.function_id] == NULL){
		QDMA_MSG(DBG_ST, "qdma_ioctl: NULL, function_id=0x%x\n", qdma_data.function_id);
		return -EFAULT;
	}else{
    	ret = qdma_operation[qdma_data.function_id](&qdma_data);
	}
	
	if( copy_to_user((struct ECNT_QDMA_Data*)arg, &qdma_data, sizeof(struct ECNT_QDMA_Data)) ) 
    {
		return -EFAULT;
	}
	return ret;
}
#endif

#ifdef CONFIG_COMPAT
static long qdma_compat_ioctl(struct file *filp, uint cmd, void *arg) 
{
	int ret = 0 ;
	struct ECNT_QDMA_Data qdma_data;
	QDMA_TxQosScheduler_T pTxQos;
	GENERAL_TrtcmCfg_T generalTrtcmCfg;
	GENERAL_TrtcmRatelimitCfg_T generalTrtcmRatelimitCfg;
	GENERAL_TrtcmRatelimitSet_T generalTrtcmRatelimitSet;
	GENERAL_TrtcmCbsPbsCfg_T generalTrtcmCbsPbsCfg;
	GENERAL_TrtcmCbsPbsSet_T generalTrtcmCbsPbsSet;
	QDMA_FlowCntIncDropCfg_T flowCntIncDropCfg;
	QDMA_FlowCntValueGet_T flowCntValueGet;
	QDMA_FlowCntValueClear_T flowCntValueClear;
	QDMA_TxWredCfg_T txWredCfg;
	QDMA_RxRedCfg_T rxRedCfg;
	QDMA_SlaChnlCfg_T slaChnlCfg;
	QDMA_QoSAgingCfg_T qosAgingCfg;
	QDMA_TxQAgingCfg_T txqAgingCfg;
	QDMA_MulticastSptagCfg_T multicastSptagCfg;
	QDMA_MulticastFportCfg_T multicastFportCfg;
	QDMA_TxQStaticCfg_T staticThrhCfg;
	QDMA_TxQStaticThrRatio_T staticThrhRatio;
	QDMA_RxLowThreshold_T rxLowThreshold;
	QDMA_TxRateMeter_T txRateMeter;
	QDMA_TxRateLimitCfg_T txRateLimitCfg;
	QDMA_TxRateLimitSet_T txRateLimit;
	QDMA_TxDbaReport_T txDbaReport;
	QDMA_RxRateLimitGet_T rxRateLimit;
	QDMA_RxRateLimitCfg_T rxRateLimitCfg;
	QDMA_VirtualChannelMode_T virChnlMode;
	QDMA_VirtualChannelQoS_T virChnlQoS;
	QDMA_MacLimitThrh_T qdma_mac_limit_thrh;
	QDMA_TxBufCtrl_T pTxBufCtrl;
	QDMA_PrefetchMode_T qdma_prefetch;
	QDMA_LmgrStatus_T lmgr;
	QDMA_txCngstCfg_t pTxCngst;
	QDMA_TxQDynCngstTotalThrh_T totalThrh;
	QDMA_TxQDynCngstChnlThrh_T chnlThrh;
	QDMA_TxQDynCngstQueueThrh_T queueThrh;
	QDMA_PeekRateCfg_t peekrateCfg;
	QDMA_TxQStaticNormalCfg_T normThrh;
	QDMA_TxQStaticDeiCfg_T deiThrh;
	
	if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
	{
		return -EFAULT;
	}

	switch(qdma_data.function_id){
		case QDMA_FUNCTION_SET_TX_QOS:
		case QDMA_FUNCTION_GET_TX_QOS:
			if( copy_from_user(&pTxQos, (QDMA_TxQosScheduler_T *)compat_ptr(qdma_data.qdma_private.qdma_tx_qos.pTxQos), sizeof(QDMA_TxQosScheduler_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_tx_qos.pTxQos = &pTxQos;
			break;
		case QDMA_FUNCTION_SET_MAC_LIMIT_THRESHOLD:
		case QDMA_FUNCTION_GET_MAC_LIMIT_THRESHOLD:
			if( copy_from_user(&qdma_mac_limit_thrh, (QDMA_MacLimitThrh_T *)compat_ptr(qdma_data.qdma_private.qdma_mac_limit_thrh), sizeof(QDMA_MacLimitThrh_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_mac_limit_thrh = &qdma_mac_limit_thrh;
			break;
		case QDMA_FUNCTION_SET_TXBUF_THRESHOLD:
		case QDMA_FUNCTION_GET_TXBUF_THRESHOLD:
			if( copy_from_user(&pTxBufCtrl, (QDMA_TxBufCtrl_T *)compat_ptr(qdma_data.qdma_private.pQdmaTxBufCtrl), sizeof(QDMA_TxBufCtrl_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.pQdmaTxBufCtrl = &pTxBufCtrl;
			break;
		case QDMA_FUNCTION_SET_PREFETCH_MODE:
			if( copy_from_user(&qdma_prefetch, (QDMA_PrefetchMode_T *)compat_ptr(qdma_data.qdma_private.qdma_prefetch), sizeof(QDMA_PrefetchMode_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_prefetch = &qdma_prefetch;
			break;
		case QDMA_FUNCTION_SET_TX_RATEMETER:
		case QDMA_FUNCTION_GET_TX_RATEMETER:
			if( copy_from_user(&txRateMeter, (QDMA_TxRateMeter_T *)compat_ptr(qdma_data.qdma_private.txRateMeterPtr), sizeof(QDMA_TxRateMeter_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.txRateMeterPtr = &txRateMeter;
			break;
		case QDMA_FUNCTION_SET_TX_RATELIMIT_CFG:
		case QDMA_FUNCTION_GET_TX_RATELIMIT_CFG:
			if( copy_from_user(&txRateLimitCfg, (QDMA_TxRateLimitCfg_T *)compat_ptr(qdma_data.qdma_private.txRateLimitCfgPtr), sizeof(QDMA_TxRateLimitCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.txRateLimitCfgPtr = &txRateLimitCfg;
			break;
		case QDMA_FUNCTION_SET_TX_RATELIMIT:
		case QDMA_FUNCTION_GET_TX_RATELIMIT:
			if( copy_from_user(&txRateLimit, (QDMA_TxRateLimitSet_T *)compat_ptr(qdma_data.qdma_private.txRateLimitSetPtr), sizeof(QDMA_TxRateLimitSet_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.txRateLimitSetPtr = &txRateLimit;
			break;
		case QDMA_FUNCTION_SET_TX_DBAREPORT:
		case QDMA_FUNCTION_GET_TX_DBAREPORT:
			if( copy_from_user(&txDbaReport, (QDMA_TxDbaReport_T *)compat_ptr(qdma_data.qdma_private.txDbaReportPtr), sizeof(QDMA_TxDbaReport_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.txDbaReportPtr = &txDbaReport;
			break;
		case QDMA_FUNCTION_SET_RX_LOW_THRESHOLD:
		case QDMA_FUNCTION_GET_RX_LOW_THRESHOLD:
			if( copy_from_user(&rxLowThreshold, (QDMA_RxLowThreshold_T *)compat_ptr(qdma_data.qdma_private.rxLowThresholdPtr), sizeof(QDMA_RxLowThreshold_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.rxLowThresholdPtr = &rxLowThreshold;
			break;
		case QDMA_FUNCTION_GET_LMGR_STATUS:
			if( copy_from_user(&lmgr, (QDMA_LmgrStatus_T *)compat_ptr(qdma_data.qdma_private.qdma_lmgr_status), sizeof(QDMA_LmgrStatus_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.qdma_lmgr_status = &lmgr;
			break;	
		case QDMA_FUNCTION_GET_RX_RATELIMIT_CFG:
			if( copy_from_user(&rxRateLimitCfg, (QDMA_RxRateLimitCfg_T *)compat_ptr(qdma_data.qdma_private.rxRateLimitCfgPtr), sizeof(QDMA_RxRateLimitCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.rxRateLimitCfgPtr = &rxRateLimitCfg;
			break;
		case QDMA_FUNCTION_SET_RX_RATELIMIT:
		case QDMA_FUNCTION_GET_RX_RATELIMIT:
			if( copy_from_user(&rxRateLimit, (QDMA_RxRateLimitGet_T *)compat_ptr(qdma_data.qdma_private.rxRateLimitGetPtr), sizeof(QDMA_RxRateLimitGet_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.rxRateLimitGetPtr = &rxRateLimit;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_AUTO_CONFIG:
			if( copy_from_user(&pTxCngst, (QDMA_txCngstCfg_t *)compat_ptr(qdma_data.qdma_private.pTxCngstCfg), sizeof(QDMA_txCngstCfg_t)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.pTxCngstCfg = &pTxCngst;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_TOTAL_THRESHOLD:
			if( copy_from_user(&totalThrh, (QDMA_TxQDynCngstTotalThrh_T *)compat_ptr(qdma_data.qdma_private.totalThrhPtr), sizeof(QDMA_TxQDynCngstTotalThrh_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.totalThrhPtr = &totalThrh;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_CHANNEL_THRESHOLD:
			if( copy_from_user(&chnlThrh, (QDMA_TxQDynCngstChnlThrh_T *)compat_ptr(qdma_data.qdma_private.chnlThrhPtr), sizeof(QDMA_TxQDynCngstChnlThrh_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.chnlThrhPtr = &chnlThrh;
			break;
		case QDMA_FUNCTION_SET_TXQ_CNGST_QUEUE_THRESHOLD:
			if( copy_from_user(&queueThrh, (QDMA_TxQDynCngstQueueThrh_T *)compat_ptr(qdma_data.qdma_private.queueThrhPtr), sizeof(QDMA_TxQDynCngstQueueThrh_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.queueThrhPtr = &queueThrh;
			break;
		case QDMA_FUNCTION_SET_PEEKRATE_PARAMS:
			if( copy_from_user(&peekrateCfg, (QDMA_PeekRateCfg_t *)compat_ptr(qdma_data.qdma_private.peekrateCfgPtr), sizeof(QDMA_PeekRateCfg_t)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.peekrateCfgPtr = &peekrateCfg;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_NORMAL_THRESHOLD:
			if( copy_from_user(&normThrh, (QDMA_TxQStaticNormalCfg_T *)compat_ptr(qdma_data.qdma_private.normThrhPtr), sizeof(QDMA_TxQStaticNormalCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.normThrhPtr = &normThrh;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_DEI_THRESHOLD:
			if( copy_from_user(&deiThrh, (QDMA_TxQStaticDeiCfg_T *)compat_ptr(qdma_data.qdma_private.deiThrhPtr), sizeof(QDMA_TxQStaticDeiCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.deiThrhPtr = &deiThrh;
			break;
		case QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_MODE:
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_MODE:
			if( copy_from_user(&virChnlMode, (QDMA_VirtualChannelMode_T *)compat_ptr(qdma_data.qdma_private.virChnlModePtr), sizeof(QDMA_VirtualChannelMode_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.virChnlModePtr = &virChnlMode;
			break;
		case QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_QOS:
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_QOS:
			if( copy_from_user(&virChnlQoS, (QDMA_VirtualChannelQoS_T *)compat_ptr(qdma_data.qdma_private.virChnlQoSPtr), sizeof(QDMA_VirtualChannelQoS_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.virChnlQoSPtr = &virChnlQoS;
			break;
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_CFG:
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_CFG:
			if( copy_from_user(&generalTrtcmCfg, (GENERAL_TrtcmCfg_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmCfgPtr), sizeof(GENERAL_TrtcmCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmCfgPtr = &generalTrtcmCfg;
			break;
		case QDMA_FUNCTION_GENERAL_SET_RATELIMIT_MODE_CFG:
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_CFG:
			if( copy_from_user(&generalTrtcmRatelimitCfg, (GENERAL_TrtcmRatelimitCfg_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr), sizeof(GENERAL_TrtcmRatelimitCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr = &generalTrtcmRatelimitCfg;
			break;
		case QDMA_FUNCTION_GENERAL_SET_RATELIMIT_MODE_VALUE:
		case QDMA_FUNCTION_GENERAL_SET_RATELIMIT_BUCKET_SIZE:
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_VALUE:
			if( copy_from_user(&generalTrtcmRatelimitSet, (GENERAL_TrtcmRatelimitSet_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmRatelimitSetPtr), sizeof(GENERAL_TrtcmRatelimitSet_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmRatelimitSetPtr = &generalTrtcmRatelimitSet;
			break;
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_MODE_CFG:
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_CFG:
			if( copy_from_user(&generalTrtcmCbsPbsCfg, (GENERAL_TrtcmCbsPbsCfg_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr), sizeof(GENERAL_TrtcmCbsPbsCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr = &generalTrtcmCbsPbsCfg;
			break;
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_MODE_VALUE:
		case QDMA_FUNCTION_GENERAL_SET_TRTCM_BUCKET_SIZE:
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_VALUE:
			if( copy_from_user(&generalTrtcmCbsPbsSet, (GENERAL_TrtcmCbsPbsSet_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr), sizeof(GENERAL_TrtcmCbsPbsSet_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr = &generalTrtcmCbsPbsSet;
			break;
		case QDMA_FUNCTION_SET_FLOW_CNTR_CFG:
		case QDMA_FUNCTION_GET_FLOW_CNTR_CFG:
			if( copy_from_user(&flowCntIncDropCfg, (QDMA_FlowCntIncDropCfg_T *)compat_ptr(qdma_data.qdma_private.flowCntIncDropCfg), sizeof(QDMA_FlowCntIncDropCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.flowCntIncDropCfg = &flowCntIncDropCfg;
			break;
		case QDMA_FUNCTION_GET_FLOW_CNTR_VALUE:
			if( copy_from_user(&flowCntValueGet, (QDMA_FlowCntValueGet_T *)compat_ptr(qdma_data.qdma_private.flowCntValueGet), sizeof(QDMA_FlowCntValueGet_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.flowCntValueGet = &flowCntValueGet;
			break;
		case QDMA_FUNCTION_CLEAR_FLOW_CNTR_VALUE:
			if( copy_from_user(&flowCntValueClear, (QDMA_FlowCntValueClear_T *)compat_ptr(qdma_data.qdma_private.flowCntValueClear), sizeof(QDMA_FlowCntValueClear_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.flowCntValueClear = &flowCntValueClear;
			break;
		case QDMA_FUNCTION_SET_TX_WRED_MODE:
		case QDMA_FUNCTION_GET_TX_WRED_MODE:
		case QDMA_FUNCTION_SET_TX_WRED_THRESHOLD:
		case QDMA_FUNCTION_SET_TX_WRED_PROBABILITY:
		case QDMA_FUNCTION_GET_TX_WRED_CFG:
			if( copy_from_user(&txWredCfg, (QDMA_TxWredCfg_T *)compat_ptr(qdma_data.qdma_private.txWredCfgPtr), sizeof(QDMA_TxWredCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.txWredCfgPtr = &txWredCfg;
			break;

		case QDMA_FUNCTION_SET_CPU_RX_RED_PROBABILITY:
		case QDMA_FUNCTION_GET_CPU_RX_RED_PROBABILITY:
			if( copy_from_user(&rxRedCfg, (QDMA_RxRedCfg_T *)compat_ptr(qdma_data.qdma_private.rxRedCfgPtr), sizeof(QDMA_RxRedCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.rxRedCfgPtr = &rxRedCfg;
			break;
		case QDMA_FUNCTION_SET_SLA_CHANNEL_CFG:
		case QDMA_FUNCTION_GET_SLA_CHANNEL_CFG:
			if( copy_from_user(&slaChnlCfg, (QDMA_SlaChnlCfg_T *)compat_ptr(qdma_data.qdma_private.slaChnlCfgPtr), sizeof(QDMA_SlaChnlCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.slaChnlCfgPtr = &slaChnlCfg;
			break;
		case QDMA_FUNCTION_SET_QOS_AGING_CFG:
		case QDMA_FUNCTION_GET_QOS_AGING_CFG:
			if( copy_from_user(&qosAgingCfg, (QDMA_QoSAgingCfg_T *)compat_ptr(qdma_data.qdma_private.qosAgingCfgPtr), sizeof(QDMA_QoSAgingCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.qosAgingCfgPtr = &qosAgingCfg;
			break;
		case QDMA_FUNCTION_SET_PER_QUEUE_AGING_CFG:
		case QDMA_FUNCTION_GET_PER_QUEUE_AGING_CFG:
			if( copy_from_user(&txqAgingCfg, (QDMA_TxQAgingCfg_T *)compat_ptr(qdma_data.qdma_private.txqAgingCfgPtr), sizeof(QDMA_TxQAgingCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.txqAgingCfgPtr = &txqAgingCfg;
			break;
		case QDMA_FUNCTION_SET_MULTICAST_SPTAG_CFG:
		case QDMA_FUNCTION_GET_MULTICAST_SPTAG_CFG:
			if( copy_from_user(&multicastSptagCfg, (QDMA_MulticastSptagCfg_T *)compat_ptr(qdma_data.qdma_private.multicastSptagCfgPtr), sizeof(QDMA_MulticastSptagCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.multicastSptagCfgPtr = &multicastSptagCfg;
			break;
		case QDMA_FUNCTION_SET_MULTICAST_FPORT_CFG:
		case QDMA_FUNCTION_GET_MULTICAST_FPORT_CFG:
			if( copy_from_user(&multicastFportCfg, (QDMA_MulticastFportCfg_T *)compat_ptr(qdma_data.qdma_private.multicastFportCfgPtr), sizeof(QDMA_MulticastFportCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.multicastFportCfgPtr = &multicastFportCfg;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_CHANNEL_EN:
			if( copy_from_user(&staticThrhCfg, (QDMA_TxQStaticCfg_T *)compat_ptr(qdma_data.qdma_private.staticThrhCfgPtr), sizeof(QDMA_TxQStaticCfg_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.staticThrhCfgPtr = &staticThrhCfg;
			break;
		case QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_RATIO:
			if( copy_from_user(&staticThrhRatio, (QDMA_TxQStaticThrRatio_T *)compat_ptr(qdma_data.qdma_private.staticThrhRatioPtr), sizeof(QDMA_TxQStaticThrRatio_T)) ) 
			{
				return -EFAULT;
			}
			qdma_data.qdma_private.staticThrhRatioPtr = &staticThrhRatio;
			break;
			
		default:
			break;
	}

	if(qdma_operation[qdma_data.function_id] == NULL){
		QDMA_MSG(DBG_ST, "qdma_ioctl: NULL, function_id=0x%x\n", qdma_data.function_id);
		return -EFAULT;
	}else{
		ret = qdma_operation[qdma_data.function_id](&qdma_data);
	}

	switch(qdma_data.function_id){
		case QDMA_FUNCTION_GET_TX_QOS:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxQosScheduler_T *)compat_ptr(qdma_data.qdma_private.qdma_tx_qos.pTxQos), &pTxQos, sizeof(QDMA_TxQosScheduler_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_SET_MAC_LIMIT_THRESHOLD:
		case QDMA_FUNCTION_GET_MAC_LIMIT_THRESHOLD:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_MacLimitThrh_T *)compat_ptr(qdma_data.qdma_private.qdma_mac_limit_thrh), &qdma_mac_limit_thrh, sizeof(QDMA_MacLimitThrh_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_SET_TXBUF_THRESHOLD:
		case QDMA_FUNCTION_GET_TXBUF_THRESHOLD:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxBufCtrl_T *)compat_ptr(qdma_data.qdma_private.pQdmaTxBufCtrl), &pTxBufCtrl, sizeof(QDMA_TxBufCtrl_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_RATEMETER:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxRateMeter_T *)compat_ptr(qdma_data.qdma_private.txRateMeterPtr), &txRateMeter, sizeof(QDMA_TxRateMeter_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_RATELIMIT_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxRateLimitCfg_T *)compat_ptr(qdma_data.qdma_private.txRateLimitCfgPtr), &txRateLimitCfg, sizeof(QDMA_TxRateLimitCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_RATELIMIT:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxRateLimitSet_T *)compat_ptr(qdma_data.qdma_private.txRateLimitSetPtr), &txRateLimit, sizeof(QDMA_TxRateLimitSet_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_DBAREPORT:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxDbaReport_T *)compat_ptr(qdma_data.qdma_private.txDbaReportPtr), &txDbaReport, sizeof(QDMA_TxDbaReport_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_RX_LOW_THRESHOLD:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_RxLowThreshold_T *)compat_ptr(qdma_data.qdma_private.rxLowThresholdPtr), &rxLowThreshold, sizeof(QDMA_RxLowThreshold_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_LMGR_STATUS:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_LmgrStatus_T *)compat_ptr(qdma_data.qdma_private.qdma_lmgr_status), &lmgr, sizeof(QDMA_LmgrStatus_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_RX_RATELIMIT_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_RxRateLimitCfg_T *)compat_ptr(qdma_data.qdma_private.rxRateLimitCfgPtr), &rxRateLimitCfg, sizeof(QDMA_RxRateLimitCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_RX_RATELIMIT:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_RxRateLimitGet_T *)compat_ptr(qdma_data.qdma_private.rxRateLimitGetPtr), &rxRateLimit, sizeof(QDMA_RxRateLimitGet_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_MODE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_VirtualChannelMode_T *)compat_ptr(qdma_data.qdma_private.virChnlModePtr), &virChnlMode, sizeof(QDMA_VirtualChannelMode_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_QOS:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_VirtualChannelQoS_T *)compat_ptr(qdma_data.qdma_private.virChnlQoSPtr), &virChnlQoS, sizeof(QDMA_VirtualChannelQoS_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((GENERAL_TrtcmCfg_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmCfgPtr), &generalTrtcmCfg, sizeof(GENERAL_TrtcmCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((GENERAL_TrtcmRatelimitCfg_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr), &generalTrtcmRatelimitCfg, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_VALUE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((GENERAL_TrtcmRatelimitSet_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmRatelimitSetPtr), &generalTrtcmRatelimitSet, sizeof(GENERAL_TrtcmRatelimitSet_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((GENERAL_TrtcmCbsPbsCfg_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr), &generalTrtcmCbsPbsCfg, sizeof(GENERAL_TrtcmCbsPbsCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_VALUE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((GENERAL_TrtcmCbsPbsSet_T *)compat_ptr(qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr), &generalTrtcmCbsPbsSet, sizeof(GENERAL_TrtcmCbsPbsSet_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_FLOW_CNTR_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_FlowCntIncDropCfg_T *)compat_ptr(qdma_data.qdma_private.flowCntIncDropCfg), &flowCntIncDropCfg, sizeof(QDMA_FlowCntIncDropCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_FLOW_CNTR_VALUE:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_FlowCntValueGet_T *)compat_ptr(qdma_data.qdma_private.flowCntValueGet), &flowCntValueGet, sizeof(QDMA_FlowCntValueGet_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_TX_WRED_MODE:
		case QDMA_FUNCTION_GET_TX_WRED_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxWredCfg_T *)compat_ptr(qdma_data.qdma_private.txWredCfgPtr), &txWredCfg, sizeof(QDMA_TxWredCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_CPU_RX_RED_PROBABILITY:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_RxRedCfg_T *)compat_ptr(qdma_data.qdma_private.rxRedCfgPtr), &rxRedCfg, sizeof(QDMA_RxRedCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_SLA_CHANNEL_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_SlaChnlCfg_T *)compat_ptr(qdma_data.qdma_private.slaChnlCfgPtr), &slaChnlCfg, sizeof(QDMA_SlaChnlCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_QOS_AGING_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_QoSAgingCfg_T *)compat_ptr(qdma_data.qdma_private.qosAgingCfgPtr), &qosAgingCfg, sizeof(QDMA_QoSAgingCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_PER_QUEUE_AGING_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_TxQAgingCfg_T *)compat_ptr(qdma_data.qdma_private.txqAgingCfgPtr), &txqAgingCfg, sizeof(QDMA_TxQAgingCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_MULTICAST_SPTAG_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_MulticastSptagCfg_T *)compat_ptr(qdma_data.qdma_private.multicastSptagCfgPtr), &multicastSptagCfg, sizeof(QDMA_MulticastSptagCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;
		case QDMA_FUNCTION_GET_MULTICAST_FPORT_CFG:
			if( copy_from_user(&qdma_data, (struct ECNT_QDMA_Data*)compat_ptr(arg), sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			if( copy_to_user((QDMA_MulticastFportCfg_T *)compat_ptr(qdma_data.qdma_private.multicastFportCfgPtr), &multicastFportCfg, sizeof(QDMA_MulticastFportCfg_T)) ) 
			{
				return -EFAULT;
			}
			break;

		default:
			if( copy_to_user((struct ECNT_QDMA_Data*)compat_ptr(arg), &qdma_data, sizeof(struct ECNT_QDMA_Data)) ) 
			{
				return -EFAULT;
			}
			break;
	}
	
	return ret;
}
#endif

/***************************************************************
***************************************************************/
struct file_operations qdma_fops = {
	.owner 			= THIS_MODULE,
	.unlocked_ioctl	= qdma_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = qdma_compat_ioctl,
#endif
} ;


/***************************************************************
***************************************************************/
void qdma_dev_destroy(void) 
{   
	dev_t devNo ;
		
	if(gpQdmaDev->pQdmaDev != NULL) {
		cdev_del(gpQdmaDev->pQdmaDev) ;
		gpQdmaDev->pQdmaDev = NULL ;
	}
	
	devNo = MKDEV(COSNT_QDMA_MAJOR_NUN, 0) ;
	unregister_chrdev_region(devNo, 1) ;
	
	QDMA_MSG(DBG_ST, "QDMA Device destroy successful\n") ;
}
#endif

int qdma_dev_trtcm_cfg_init(uint base)
{
	memset(trtcmCfgBase, 0, sizeof(uint) * TRTCM_MODE_MAX);
	memset(trtcmBucketByteUnit, 0, sizeof(uint) * TRTCM_MODE_MAX);
	memset(trtcmBucketPacketUnit, 0, sizeof(uint) * TRTCM_MODE_MAX);

    qdma_init_trtcm(base,trtcmCfgBase,trtcmBucketByteUnit,trtcmBucketPacketUnit);
	
	return 0;
}

int qdma_dev_ratelimit_init(uint base)
{
#if SUPPORT_MTR_ACNT_IN_QDMA
	generalSetTrtcmRateEnable(GLB_RATECTL, TRTCM_ENABLE);
	generalSetTrtcmFastTick(GLB_RATECTL, 25) ;		 //fasttick 25us
	generalSetTrtcmSlowTickRatio(GLB_RATECTL, 40);	 //slowtick 1000us 

	generalSetTrtcmRateEnable(EGRESS_TRTCM, TRTCM_ENABLE);
	generalSetTrtcmFastTick(EGRESS_TRTCM, 25) ; 	//fasttick 25us
	generalSetTrtcmSlowTickRatio(EGRESS_TRTCM, 40); //slowtick 1000us 

	generalSetTrtcmRateEnable(INGRESS_TRTCM, TRTCM_ENABLE);
	generalSetTrtcmFastTick(INGRESS_TRTCM, 125) ;	//fasttick 125us
	generalSetTrtcmSlowTickRatio(INGRESS_TRTCM, 8); //slowtick 1000us 

#if SUPPORT_SLA
	generalSetTrtcmRateEnable(SLA_TRTCM, TRTCM_ENABLE);
	generalSetTrtcmFastTick(SLA_TRTCM, 25) ;	//fasttick 25us
	generalSetTrtcmSlowTickRatio(SLA_TRTCM, 40);	//slowtick 1000us 
#endif
	/* INGRESS works as trtcm mode */
	generalSetTrtcmMode(INGRESS_TRTCM, TRTCM_RATELIMIT_MODE);

#else
	qdmaEnableTxRateLimit(base) ;	/* global rate limit default enable */
	qdmaSetTxRateLimitBucketScale(base, QDMA_TRTCM_SCALE_2BYTE) ;

	qdmaSetTxRateLimitTicksel(base, 1, 8000/TX_RATELIMIT_TICKER1_ACCURACY) ;	// ticer 500s ,unit is 16kbps,ticker1 can be adjust dynamiclly!!!!
	qdmaSetTxRateLimitTicksel(base, 0, 8000) ;	// unit is 1kbps,should not be modify !!!!
#endif

	/* rx ring ratelimit default setting */
    qdmaSetRxRatelimitDefaultConfig();

	return 0;
}

int qdma_dev_aging_init(uint base)
{
    #if SUPPORT_QUEUE_AGING
	/* default: enable aging */
	qdmaEnableQosAgingEn(base);
	qdmaSetQosAgingMethod(base, QDMA_QOS_AGING_THRESHOLD);
	qdmaEnableQosAgingFastReplaceEn(base);
    #endif

	return 0;
}

int qdma_dev_sdn_cntr_init(uint base)
{
    #if SUPPORT_SDN_CNTR
	/* enable flowcnt & init value */
	struct ECNT_QDMA_Data qdma_data;
	QDMA_FlowCntValueClear_T flowCntValueClear;

	memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data)) ;
	memset(&flowCntValueClear, 0, sizeof(QDMA_FlowCntValueClear_T)) ;

	qdmaEnableSdnCntrEnable(base);
	qdmaEnableSdnCntrCpuRxEnable(base);
	flowCntValueClear.grpIdx = 0;
	flowCntValueClear.cntIdx = CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM;
	flowCntValueClear.clearMode = FLOWCNT_CLEAR_ALL;
	qdma_data.qdma_private.flowCntValueClear = &flowCntValueClear;
	qdma_clear_flow_cntr_value(&qdma_data);
	flowCntValueClear.grpIdx = 1;
	flowCntValueClear.cntIdx = CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM;
	flowCntValueClear.clearMode = FLOWCNT_CLEAR_ALL;
	qdma_data.qdma_private.flowCntValueClear = &flowCntValueClear;
	qdma_clear_flow_cntr_value(&qdma_data);
	flowCntValueClear.grpIdx = 2;
	flowCntValueClear.cntIdx = CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM;
	flowCntValueClear.clearMode = FLOWCNT_CLEAR_ALL;
	qdma_data.qdma_private.flowCntValueClear = &flowCntValueClear;
	qdma_clear_flow_cntr_value(&qdma_data);
    #endif

	return 0;
}

int qdma_dev_multicast_init(uint base)
{
    #if SUPPORT_QDMA_MULTICAST
	int chnl_idx = 0;
	
	for( chnl_idx = 0; chnl_idx < MULTICAST_MAX_CHANNEL; chnl_idx++ ){
		/* init multicast fport info */
		qdmaSetMulticastFport(base, chnl_idx, MULTICAST_FORCE_PORT[chnl_idx]);

		/* init multicast sp_tag info */
		qdmaSetMulticastSptag(base, chnl_idx, MULTICAST_SPECIAL_TAG[chnl_idx]);

		/* init multicast sp_tag hi-field keep or not */
		if( MULTICAST_KEEP_SPTAG_HIFIELD[chnl_idx] > 0 ){
			qdmaEnableMulticastSptagKeepHi(base, chnl_idx);
		}else{
			qdmaDisableMulticastSptagKeepHi(base, chnl_idx);
		}
	}
    #endif

	return 0;
}

int qdma_dev_hqos_init(uint base)
{
#if SUPPORT_HQOS
#ifdef QDMA_LAN
	if(qdmaLanHqosMode == 1){
		qdmaEnableHqosMode(base);
	}else{
		qdmaDisableHqosMode(base);
	}
#else
	if(qdmaWanHqosMode == 1){
		qdmaEnableHqosMode(base);
		/*for epon ,when open hqos.oam favor should be closed*/
		qdmaDisableOamModifyFpEn(base);
	}else{
		qdmaDisableHqosMode(base);
	}
#endif
#endif
	return 0;
}

int qdma_dev_qos_init(uint base)
{
#if SUPPORT_BUFFCFG_DIFFERENT_BETWEEN_FAST_SLOW_PATH
	if(qdmaDscpMode) { /* fast path*/		
		BUFFER_THRESHOLD[0] = BUFF_TOTAL_MAX_THRH_SRAM;
		BUFFER_THRESHOLD[1] = BUFF_TOTAL_MIN_THRH_SRAM;
		BUFFER_THRESHOLD[2] = BUFF_CHN_MAX_THRH_SRAM;
		BUFFER_THRESHOLD[3] = BUFF_CHN_MIN_THRH_SRAM;
		BUFFER_THRESHOLD[4] = BUFF_QUE_MAX_THRH_SRAM;
		BUFFER_THRESHOLD[5] = BUFF_QUE_MIN_THRH_SRAM;
	} else { /* slow path*/
		BUFFER_THRESHOLD[0] = BUFF_TOTAL_MAX_THRH;
		BUFFER_THRESHOLD[1] = BUFF_TOTAL_MIN_THRH;
		BUFFER_THRESHOLD[2] = BUFF_CHN_MAX_THRH;
		BUFFER_THRESHOLD[3] = BUFF_CHN_MIN_THRH;
		BUFFER_THRESHOLD[4] = BUFF_QUE_MAX_THRH;
		BUFFER_THRESHOLD[5] = BUFF_QUE_MIN_THRH;
	}
#else
		BUFFER_THRESHOLD[0] = BUFF_TOTAL_MAX_THRH;
		BUFFER_THRESHOLD[1] = BUFF_TOTAL_MIN_THRH;
		BUFFER_THRESHOLD[2] = BUFF_CHN_MAX_THRH;
		BUFFER_THRESHOLD[3] = BUFF_CHN_MIN_THRH;
		BUFFER_THRESHOLD[4] = BUFF_QUE_MAX_THRH;
		BUFFER_THRESHOLD[5] = BUFF_QUE_MIN_THRH;
#endif
	return 0;
}


/******************************************************************************
******************************************************************************/
int qdma_dev_init(void) 
{
	uint base = gpQdmaPriv->csrBaseAddr ;
 	uint glbCfg=0 ;
	uint channel=0, queue=0, ringIdx=0 ;
	int ret = 0 ;
	dev_t devNo ;
	struct ECNT_QDMA_Data qdma_data;
	QDMA_txCngstCfg_t txCngstCfg;
	
	/********************************************
	* init the public data *
	*********************************************/
	qdma_dev_trtcm_cfg_init(base);

	memset((char *)qdma_vip_info, 0, sizeof(QDMA_VIP_INFO_T)*QDMA_VIP_PACKET_NUM);
#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
	qdmaClearIntStatus(base, 0xFFFFFFFF) ;
#endif	
	qdmaClearIntStatus1(base,0xFFFFFFFF);
	qdmaClearIntStatus2(base,0xFFFFFFFF);
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaClearIntStatus3(base, 0xFFFFFFFF);
	qdmaClearIntStatus4(base, 0xFFFFFFFF);
	qdmaClearIntStatus5(base, 0xFFFFFFFF);
#endif


	/********************************************
	* enable/disable the qdma interrupt         *
	*********************************************/
	qdmaSetIntBind(base,QDMA_INT1);
	qdmaSetIntBind(base,QDMA_INT2);
	qdmaSetIntBind(base,QDMA_INT3);
	qdmaSetIntBind(base,QDMA_INT4);

	/********************************************
	* set the qdma irq bind        *
	*********************************************/
	qdmaSetIrqBind(base,QDMA_IRQ1);
	qdmaSetIrqBind(base,QDMA_IRQ2);

	/********************************************
	* Setting the global register               *
	*********************************************/
    glbCfg = GLB_CFG_VALUE;
	
	if(gpQdmaPriv->irqDepth) {
		glbCfg |= GLB_CFG_IRQ_EN ;
	}
#if SUPPORT_IRQ2
	if(gpQdmaPriv->irq2Depth) {
		glbCfg |= GLB_CFG_IRQ2_EN ;
	}
#endif
	
#ifdef CONFIG_RX_2B_OFFSET
	glbCfg |= GLB_CFG_RX_2B_OFFSET ;
#endif /* CONFIG_RX_2B_OFFSET */

#ifdef CONFIG_TX_WB_DONE
	glbCfg |= GLB_CFG_TX_WB_DONE ;
#endif /* GLB_CFG_TX_WB_DONE */

#if OFFLOAD_NEED_IMMEDIATE_DONE
#if defined(QDMA_LAN)
	/* WIFI/VPN offload packets' path in PPE learning stage is QDMA1->PPE->QDM1, which doesn't go through GDM1.
	 * In this case, QDMA1 can't get Done signal from GDM1, so won't do Tx recycle for those packets. Therefore,
	 * GLB_CFG_TX_IMMEDIATE_DONE needs to be enabled, so that QDMA1 can do Tx recycle for those packets without
	 * waiting for GDM1's Done signal.*/
	glbCfg |= GLB_CFG_TX_IMMEDIATE_DONE;
#endif
#endif

#if SUPPORT_QDMA_MULTICAST
#if defined(QDMA_LAN)
	glbCfg |= GLB_CFG_MULTICAST_MODIFY_FP | GLB_CFG_MULTICAST_EN;
#endif
#endif
	
	qdmaSetGlbCfg(base, glbCfg) ;

	/********************************************
	* Misc Setting                               *
	*********************************************/
	/* set tx qos weight_scale as 16Byte, and weight_base by byte */
    /*EN7580 and EN7528: set tx WRR weight_scale as 1Byte*/
#if SUPPORT_WRR_WEIGHT_SCALE_1Byte
	qdmaSetTxQosWeightScale1(base) ;
#else
	qdmaSetTxQosWeightScale16(base) ;
#endif
	qdmaSetTxQosWeightByByte(base) ;

    qdma_set_txbuf_estimate(base);

	/* force enable tx rate meter */
	qdmaEnableTxRateMeter(base) ;
#if SUPPORT_METER_WINDOW_SIZE_TIME_SLIDE
#ifdef QDMA_LAN
	if (!SUPPORT_TX_PEAK_MODE) 
		qdmaEnableTxRateMeterEqualRate(base) ;
	qdmaSetTxRateMeterWindowSize(base, 0x1F) ; /* 2047us X 31 = 63.457ms */
	qdmaSetTxRateMeterTimeSlice(base, 0x7FF) ;
#else
	qdmaSetTxRateMeterWindowSize(base, 20) ; /* 200us X 20 = 4ms */
	qdmaSetTxRateMeterTimeSlice(base, 200) ;
#endif
#else
	qdmaSetTxRateMeterTimeDivisor(base, QDMA_TX_RATE_METER_TIME_DIVISOR_4) ;
	qdmaSetTxRateMeterTimeSlice(base, 4000) ;
#endif

	/* all rate limit default setting */
	qdma_dev_ratelimit_init(base);

	/* tx ring drop default setting */
	qdmaEnableCngstCpuTxDropEn(base, RING_IDX_0);

	/* tx ring min/max block default setting */
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		qdmaDisableCngstMaxThrhEn(base, ringIdx) ;
		qdmaDisableCngstMinThrhEn(base, ringIdx) ;
	}

	/* init qdma buffer threshold array */
	qdma_dev_qos_init(base);

	/* set txq dynamic threshold to default value */
	txCngstCfg.txCngstMode = QDMA_TXCNGST_DYNAMIC_NORMAL ;
	qdma_data.qdma_private.pTxCngstCfg = &txCngstCfg ;
	qdma_set_txq_cngst_auto_config(&qdma_data) ;

	/* enable 3 trigger mode, and set time trigger time as 250us */
	qdmaEnableCngstModeConfigTrig(base) ;
	qdmaEnableCngstModePacketTrig(base) ;
	qdmaEnableCngstModeTimeTrig(base) ;
	qdmaSetTxqDynCngstTicksel(base, 250) ;
	
	/* set dei packet threshold equal 1/2 normal packet */
	qdmaSetDynCngstDeiThrhScale(base, QDMA_DYNCNGST_DEI_THRH_SCALE_1_2) ;

	/* dbg cntr default setting on index 32~39 */
	qdma_set_dbg_cntr_default_config();

	/* aging & cntr & multicast default setting */
	qdma_dev_aging_init(base);
	qdma_dev_sdn_cntr_init(base);
	qdma_dev_multicast_init(base);
	qdma_dev_hqos_init(base);	
#if !defined(TCSUPPORT_CPU_EN7552) && !defined(TCSUPPORT_CPU_EN7581)
#ifndef QDMA_LAN
	qdmaEnablePseBufEstimate(base);
#endif
#endif

	/* set the default value of queue 7 as normal queue */
	qdmaDisableTxqCngstQueueNonblocking(base, 7);

#if SUPPORT_MIN_DSCP_THRH_CFG
#ifdef QDMA_LAN
	qdmaSetMinDscpThrCfg(base, 0x8<<CHNL_RSV_DSCP_NUM_SHIFT);
#endif
#endif

#if SUPPORT_INX_COLOR_RESOLUTION
	qdmaSetInxColorResCfg1(base,0x424a4201);
	default_mtr_color_cfg=qdmaGetInxColorResCfg(base);
	default_mtr_color_cfg1=qdmaGetInxColorResCfg1(base);
#endif

	/* mac resource limit init */
	channel_limit_threshold=0x10000;
#ifndef QDMA_LAN
	queue_limit_threshold=0x200;
#else
	queue_limit_threshold=0x80;
#endif

	for(channel=0; channel<CONFIG_QDMA_CHANNEL; channel++) {
		atomic_set(&chnlLimit[channel], 0);
 		for(queue=0; queue<CONFIG_QDMA_QUEUE; queue++) {
 			atomic_set(&queueLimit[channel][queue], 0);
		}
	}
	atomic_set(&txRing1FreeCnt, CONFIG_TX1_DSCP_NUM-1);
	
	/* disable qdma rx delay interrupt */
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
		qdmaSetRxDelayIntCfg(base, ringIdx, 0);
	}
	
			
#ifdef TCSUPPORT_DRIVER_API
	devNo = MKDEV(COSNT_QDMA_MAJOR_NUN, 0) ;
	ret = register_chrdev_region(devNo, 1, CONST_QDMA_DEV_NAME) ;
	if(ret < 0) {
		QDMA_ERR("Get the QDMA device number (register_chrdev_region()) failed\n") ;
		return ret ;
	}
	
	gpQdmaDev = &gpQdmaPriv->qdmaDev;
	gpQdmaDev->pQdmaDev = cdev_alloc() ;
	if(gpQdmaDev->pQdmaDev == NULL) {
		QDMA_ERR("Alloc character device (cdev_alloc()) failed\n") ;
		ret = -ENOMEM ;
		goto out ;
	}
	
	cdev_init(gpQdmaDev->pQdmaDev, &qdma_fops) ;
	gpQdmaDev->pQdmaDev->owner = THIS_MODULE ;
	gpQdmaDev->pQdmaDev->ops = &qdma_fops ;
	ret = cdev_add(gpQdmaDev->pQdmaDev, devNo, 1) ;
	if(ret < 0) {
		QDMA_MSG(DBG_ST, "QDMA driver register failed\n") ;
		goto out ;
	}
	
	QDMA_MSG(DBG_ST, "QDMA initialization successful\n") ;    
	return 0 ; 

out:
	qdma_dev_destroy() ;
	return ret ;
	
#else
	return 0 ; 
#endif
}

