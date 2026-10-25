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

#include "qdma_dev.h"
#include "qdma_bmgr.h"
#include "qdma_api.h"
#include <linux/foe_hook.h>
#include <linux/libcompileoption.h>
#include <ecnt_hook/ecnt_hook_traffic_classify.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_tso.h>
#include <ecnt_hook/ecnt_hook_bbf247.h>

#include <linux/if_vlan.h>

#include "qdma_ic_dis.h"
#include "qdma_pri.h"
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define HIGHSET_PRIORITY    7

#define SWITCH_BUFFER_THRESHOLD 40 /* unit is 40kByte */
#define CHANNEL_RATELIMIT_THRESHOLD 300000 /* unit is 1kbps */
#define EXT_SWITCH_CHANNEL_BASE 6

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define MAX(a, b) ((a>b)?(a):(b))

#define ERROR_METER_INDEX -1
#define METER_STATUS_DONE 0
#define BITSPERWORD 32
#define METER_MASK 0x1F

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/

#ifdef TCSUPPORT_QOS
#ifndef QDMA_LAN
    extern int qos_flag;
#endif
    extern int gQueueMask;
#endif

#if defined(QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
    extern int qos_wrr_info[5];
#endif

#ifdef CONFIG_SUPPORT_SELF_TEST
extern uint selfTestFlag;
#endif
#ifdef TCSUPPORT_IFC_EN
extern uint ifcTestFlag;
#endif
extern struct timer_list dma_busy_timer ;
extern int dma_busy_expires;
extern int dma_busy_round_cnt;
extern int tx_dma_busy_enable_cnt;
extern int rx_dma_busy_enable_cnt;
extern struct timer_list trigger_timer ;
extern int trigger_expires ;
extern struct timer_list reg_polling_timer ;
extern int reg_polling_expires;
extern int reg_polling_round_cnt;
extern int reg_polling_offset;
extern uint qdmaRegValBuff[100];

extern uint channel_limit_threshold;
extern uint queue_limit_threshold;

extern unchar qdmaLanHqosMode;	/* 0:disable, 1:enable */
extern unchar qdmaWanHqosMode;

extern int g_wan_mode;

extern const uint32 CONFIG_TX_DSCP_NUM[];
extern const uint32 CONFIG_RX_DSCP_NUM[];

extern unchar qdma_vip_num ;/*vip rule num*/
extern QDMA_VIP_INFO_T qdma_vip_info[QDMA_VIP_PACKET_NUM] ; 
extern atomic_t qdma_stop_flag ;

extern unchar qdmaPayloadMode; /*0x0:2K, 0x1:1K, 0x2:512, 0x3:256*/
extern unchar qdmaDscpMode; /*0x0:DSCP in DRAM, 0x1:DSCP in SRAM*/
#if SUPPORT_HWDSCP_PAYLOAD_CFG
extern const uint32 CONFIG_PAYLOAD_SIZE[4];
#endif
#if SUPPORT_QUEUE_DISABLE
extern uint TXQ_DIS_CFG_VALUE[TXQ_DIS_CFG_REG_NUM];
#endif
extern int dropBroadcastMulticastFlag;
extern int (*soft_cirpir_rate_check_hook) (struct sk_buff * skb,unsigned int queue_idx,int *ret_queue);

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
extern int SmbdTxSpeedOn;
#endif

#if defined(TCSUPPORT_CT_SWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
extern int (*sw_isSWQosActive) (void);
extern int (*sw_isHwnatOffloadEnable) (void);
#endif

extern int BUFFER_THRESHOLD[6];
extern u8 ext_swicVendor;
extern int ext_switch_sel;

extern int eth_force_dstq;

#if defined(TCSUPPORT_ACCOUNT_METER_V2)	
extern int (*fe_resource_mark_llid_hook)( struct sk_buff *skb, u8 llid, u8 dir);
#endif

int dnstream_qos_enable = 1;
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
	
int isForceRX1=0;

atomic_t chnlLimit[CONFIG_QDMA_CHANNEL];
atomic_t queueLimit[CONFIG_QDMA_CHANNEL][CONFIG_QDMA_QUEUE];
atomic_t txRing1FreeCnt;

#if defined(TCSUPPORT_CPU_EN7581)
const int RX_DSCP_BUDGET[RX_RING_NUM] = {5 ,5, 5, 5, 5, 5, 5, 5,
                                            5 ,5, 5, 5, 5, 5, 5, 5,
                                            5 ,5, 5, 5, 5, 5, 5, 5,
                                            5 ,5, 5, 5, 5, 5, 5, 5};
#else
const int RX_DSCP_BUDGET[RX_RING_NUM] = {5 ,5, 5, 5, 5, 5, 5, 5,
                                            5 ,5, 5, 5, 5, 5, 5, 5};
#endif


/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static unchar qosTypeStr[QDMA_TXQOS_TYPE_NUMS][16] = {
	"WRR",
	"SP",
	"SPWRR7", 
	"SPWRR6", 
	"SPWRR5", 
	"SPWRR4", 
	"SPWRR3",
	"SPWRR2",
};

static unchar vChnlQosTypeStr[QDMA_VCHNL_TXQOS_TYPE_NUMS][16] = {
	"WRR",
	"SP",
	"SPWRR3",
	"SPWRR2",
};

#if defined(TCSUPPORT_QDMA_VERIFY)
static unchar *dramTestDataP = NULL;
static int dramTestDataLen = 0;
static uint dramTestRingIdx = 0; 
static uint dramTestDscpIdx = 0;

extern struct sk_buff *skb_rx_dram_test;
extern unchar dramTestRxDone;
#endif

struct timer_list dbgcntr_dump_timer ;

typedef void (*qdma_txmsg_op_t)(txMsgWord_t* txMsg,uint value);

qdma_txmsg_op_t
qdma_txmsg_operation[]=
{
    qdma_init_txmsg,
    qdma_set_txmsg_whole_txmsg0,
    qdma_set_txmsg_whole_txmsg1,
    qdma_set_txmsg_whole_txmsg2,
    qdma_set_txmsg_mic_idx,
    qdma_set_txmsg_sptag,
    qdma_set_txmsg_gemport,
    qdma_set_txmsg_ico,
    qdma_set_txmsg_uco,
    qdma_set_txmsg_tco,
    qdma_set_txmsg_tso,
    qdma_set_txmsg_fast,
    qdma_set_txmsg_oam,
    qdma_set_txmsg_chn,
    qdma_set_txmsg_qid,
    qdma_set_txmsg_nodrop,
    qdma_set_txmsg_mtr0,
    qdma_set_txmsg_fport,
    qdma_set_txmsg_nbq,
    qdma_set_txmsg_hwf,
    qdma_set_txmsg_hop,
    qdma_set_txmsg_ptp,
    qdma_set_txmsg_acnt0,
    qdma_set_txmsg_acnt1,
    qdma_set_txmsg_sw_udf,
    qdma_set_txmsg_mtr1,
    qdma_set_txmsg_mtr2,
    qdma_set_txmsg_acnt2,
};

typedef void (*qdma_rxmsg_op_t)(rxMsgWord_t* pRxMsg,uint* pValue);

qdma_rxmsg_op_t
qdma_rxmsg_operation[]=
{
    qdma_get_rxmsg_whole_rxmsg0,
    qdma_get_rxmsg_whole_rxmsg1,
    qdma_get_rxmsg_whole_rxmsg2,
    qdma_get_rxmsg_whole_rxmsg3,
    qdma_get_rxmsg_sptag,
    qdma_get_rxmsg_foe_index,
    qdma_get_rxmsg_crsn,
    qdma_get_rxmsg_sport,
    qdma_get_rxmsg_ip4f,
    qdma_get_rxmsg_l4f,
    qdma_get_rxmsg_l4vld,
    qdma_get_rxmsg_ip4,
    qdma_get_rxmsg_ip6,
    qdma_get_rxmsg_l2_len,
    qdma_get_rxmsg_ifc_hit,
    qdma_get_rxmsg_ifc_id,
};
/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/******************************************************************************
 Descriptor:	It's used to init the QDMA software driver and hardware device.
 				This function must be called if the upper layer application wanna
 				use the QDMA to send/receive packets.
 Input Args:	The pointer of the QDMA_InitCfg_t
 Ret Value:		0: init successful otherwise failed.
******************************************************************************/
int qdma_init(struct ECNT_QDMA_Data *qdma_data)
{
	QDMA_InitCfg_t *pInitCfg = qdma_data->qdma_private.pInitCfg ;

    gpQdmaPriv->devCfg.macType = pInitCfg->macType;
	if(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE)
   		gpQdmaPriv->devCfg.txRecycleThreshold = pInitCfg->txRecycleThrh ;
	
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RX_PACKET;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbRecvPkts;
	qdma_register_callback_function(qdma_data) ; 
#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RX_PACKET_PTM_LAN;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbRecvPktsPtmLan;
	qdma_register_callback_function(qdma_data) ;
#endif
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EVENT_HANDLER;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbEventHandler;
	qdma_register_callback_function(qdma_data) ;
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RECYCLE_PACKET;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbRecyclePkts;
	qdma_register_callback_function(qdma_data) ; 
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_GPON_MAC_HANDLER;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbGponMacHandler;
	qdma_register_callback_function(qdma_data) ;
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EPON_MAC_HANDLER;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbEponMacHandler;
	qdma_register_callback_function(qdma_data) ;
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_XPON_PHY_HANDLER;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbXponPhyHandler;
	qdma_register_callback_function(qdma_data) ;

    /*multi-INTs extend*/
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT2_RX_PACKET;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbInt2RecvPkts;
	qdma_register_callback_function(qdma_data) ;
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT2_EVENT_HANDLER;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbInt2EventHandler;
	qdma_register_callback_function(qdma_data) ;
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT3_RX_PACKET;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbInt3RecvPkts;
	qdma_register_callback_function(qdma_data) ;
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT3_EVENT_HANDLER;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbInt3EventHandler;
	qdma_register_callback_function(qdma_data) ;
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT4_RX_PACKET;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbInt4RecvPkts;
	qdma_register_callback_function(qdma_data) ;
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT4_EVENT_HANDLER;
	qdma_data->qdma_private.qdma_reg_cb.pCbFun = pInitCfg->cbInt4EventHandler;
	qdma_register_callback_function(qdma_data) ;

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to deinit the QDMA software driver and hardware device.
 				This function must be called if the upper layer application wanna
 				transfer to another application.
 Input Args:	None
 Ret Value:		0: init successful otherwise failed.
******************************************************************************/
int qdma_deinit(struct ECNT_QDMA_Data *qdma_data)
{
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RX_PACKET;
	qdma_unregister_callback_function(qdma_data) ; 
#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RX_PACKET_PTM_LAN;
	qdma_unregister_callback_function(qdma_data) ;
#endif
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EVENT_HANDLER;
	qdma_unregister_callback_function(qdma_data) ;
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RECYCLE_PACKET;
	qdma_unregister_callback_function(qdma_data) ; 
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_GPON_MAC_HANDLER;
	qdma_unregister_callback_function(qdma_data) ;
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EPON_MAC_HANDLER;
	qdma_unregister_callback_function(qdma_data) ;
	qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_XPON_PHY_HANDLER;
	qdma_unregister_callback_function(qdma_data) ;

    /*multi-INTs extend*/
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT2_RX_PACKET;
	qdma_unregister_callback_function(qdma_data) ; 
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT2_EVENT_HANDLER;
	qdma_unregister_callback_function(qdma_data) ;
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT3_RX_PACKET;
	qdma_unregister_callback_function(qdma_data) ; 
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT3_EVENT_HANDLER;
	qdma_unregister_callback_function(qdma_data) ;
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT4_RX_PACKET;
	qdma_unregister_callback_function(qdma_data) ; 
    qdma_data->qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT4_EVENT_HANDLER;
	qdma_unregister_callback_function(qdma_data) ;

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable TXDMA mode
 Input Args:	arg1: TX DMA mode (QDMA_ENABLE/QDMA_DISABLE)
 Ret Value: 	No
******************************************************************************/
int qdma_tx_dma_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_Mode_t txMode = qdma_data->qdma_private.mode ;

	if(txMode == QDMA_ENABLE) {
		qdmaEnableTxDma(base) ;
	} else {
		qdmaDisableTxDma(base) ;
	}
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable the RXDMA mode
 Input Args:	arg1: RX DMA mode (QDMA_ENABLE/QDMA_DISABLE)
 Ret Value: 	No
******************************************************************************/
int qdma_rx_dma_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_Mode_t rxMode = qdma_data->qdma_private.mode ;
	
	if(rxMode == QDMA_ENABLE) {
		qdmaEnableRxDma(base) ;
	} else {
		qdmaDisableRxDma(base) ;
	}
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable the QDMA loopback mode
 Input Args:	arg1: loopback mode (QDMA_LOOPBACK_DISABLE/QDMA_LOOPBACK_QDMA/QDMA_LOOPBACK_UMAC)
 Ret Value: 	No
******************************************************************************/
int qdma_loopback_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_LoopbackMode_t lbMode = qdma_data->qdma_private.lbMode ;

	if(lbMode == QDMA_LOOPBACK_DISABLE) {
#if SUPPORT_QDMA_UMAC_LOOPBACK
		qdmaDisableUmacLoopback(base) ;
#endif
		qdmaDisableQdmaLoopback(base) ;
	} else if(lbMode == QDMA_LOOPBACK_QDMA) {
#if SUPPORT_QDMA_UMAC_LOOPBACK
		qdmaDisableUmacLoopback(base) ;
#endif
		qdmaEnableQdmaLoopback(base) ;
	}
#if SUPPORT_QDMA_UMAC_LOOPBACK
	else if(lbMode == QDMA_LOOPBACK_UMAC) {
		qdmaDisableQdmaLoopback(base) ;
		qdmaEnableUmacLoopback(base) ;
	}
#endif
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to register the QDMA callback functions. The QDMA 
 				driver support several callback function type that is define 
 				in QDMA_CbType_t enum.
 Input Args:	arg1: callback function type that is define in QDMA_CbType_t enum.
 				arg2: the pointer of the callback function.
 Ret Value:		0: register successful otherwise failed.
******************************************************************************/
int qdma_register_callback_function(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_CbType_t type = qdma_data->qdma_private.qdma_reg_cb.type ;
	void *pCbFun = qdma_data->qdma_private.qdma_reg_cb.pCbFun ;

	if(!pCbFun) {
		return -EINVAL ;	
	}
	
	switch(type) {
		case QDMA_CALLBACK_RX_PACKET:
			gpQdmaPriv->devCfg.bmRecvCallbackFunction = (qdma_callback_recv_packet_t)pCbFun ;	
			break ;
#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
        case QDMA_CALLBACK_RX_PACKET_PTM_LAN:
            gpQdmaPriv->devCfg.bmRecvCallbackPtmLanFunction = (qdma_callback_recv_packet_t)pCbFun ;   
            break ;
#endif
			
		case QDMA_CALLBACK_EVENT_HANDLER:
			gpQdmaPriv->devCfg.bmEventCallbackFunction = (qdma_callback_event_handler_t)pCbFun ;	
			break ;
			
		case QDMA_CALLBACK_RECYCLE_PACKET:
			gpQdmaPriv->devCfg.bmRecyPktCallbackFunction = (qdma_callback_recycle_packet_t)pCbFun ;	
			break ;
			
#if !SUPPORT_INT_ENABLE_NUM_2
		case QDMA_CALLBACK_GPON_MAC_HANDLER :
			gpQdmaPriv->devCfg.bmGponMacIntHandler = (qdma_callback_int_handler_t)pCbFun ;
			qdmaEnableInt(base, INT_MASK_GPON_MAC, QDMA_INT1, QDMA_INT_ENABLE0) ;

			break ;
			
		case QDMA_CALLBACK_EPON_MAC_HANDLER :
			gpQdmaPriv->devCfg.bmEponMacIntHandler = (qdma_callback_int_handler_t)pCbFun ;
			qdmaEnableInt(base, INT_MASK_EPON_MAC, QDMA_INT1, QDMA_INT_ENABLE0) ;

			break ;
			
		case QDMA_CALLBACK_XPON_PHY_HANDLER :
			gpQdmaPriv->devCfg.bmXponPhyIntHandler = (qdma_callback_int_handler_t)pCbFun ;

			qdmaEnableInt(base, INT_MASK_XPON_PHY, QDMA_INT1, QDMA_INT_ENABLE0) ;

			break ;
#endif
        /* multi-INTs extend*/
        case QDMA_CALLBACK_INT2_RX_PACKET:
            gpQdmaPriv->devCfg.bmInt2RecvCallbackFunction = (qdma_callback_recv_packet_with_info_t)pCbFun ;
			break ;

        case QDMA_CALLBACK_INT2_EVENT_HANDLER:
			gpQdmaPriv->devCfg.bmInt2EventCallbackFunction = (qdma_callback_event_handler_t)pCbFun ;	
			break ;

        case QDMA_CALLBACK_INT3_RX_PACKET:
            gpQdmaPriv->devCfg.bmInt3RecvCallbackFunction = (qdma_callback_recv_packet_with_info_t)pCbFun ;	
			break ;

        case QDMA_CALLBACK_INT3_EVENT_HANDLER:
			gpQdmaPriv->devCfg.bmInt3EventCallbackFunction = (qdma_callback_event_handler_t)pCbFun ;	
			break ;

        case QDMA_CALLBACK_INT4_RX_PACKET:
            gpQdmaPriv->devCfg.bmInt4RecvCallbackFunction = (qdma_callback_recv_packet_with_info_t)pCbFun ;	
			break ;

        case QDMA_CALLBACK_INT4_EVENT_HANDLER:
			gpQdmaPriv->devCfg.bmInt4EventCallbackFunction = (qdma_callback_event_handler_t)pCbFun ;	
			break ;

		default:
			return -EFAULT ;	
	}
	return 0 ;
}

/******************************************************************************
 Description:	It's used to unregister the QDMA callback functions.
 Input Args:	arg1: callback function type that is define in QDMA_CbType_t enum.
 Ret Value:		0: unregister successful otherwise failed.
******************************************************************************/
int qdma_unregister_callback_function(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_CbType_t type = qdma_data->qdma_private.qdma_reg_cb.type ;

	switch(type) {
		case QDMA_CALLBACK_RX_PACKET:
			gpQdmaPriv->devCfg.bmRecvCallbackFunction = NULL ;	
			break ;
	#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
        case QDMA_CALLBACK_RX_PACKET_PTM_LAN:
            gpQdmaPriv->devCfg.bmRecvCallbackPtmLanFunction = NULL ;  
            break ;
    #endif
		case QDMA_CALLBACK_EVENT_HANDLER:
			gpQdmaPriv->devCfg.bmEventCallbackFunction = NULL ;	
			break ;
			
		case QDMA_CALLBACK_RECYCLE_PACKET:
			gpQdmaPriv->devCfg.bmRecyPktCallbackFunction = NULL ;	
			break ;
#if !SUPPORT_INT_ENABLE_NUM_2	
		case QDMA_CALLBACK_GPON_MAC_HANDLER :
			qdmaDisableInt(base, INT_MASK_GPON_MAC, QDMA_INT1, QDMA_INT_ENABLE0) ;
			gpQdmaPriv->devCfg.bmGponMacIntHandler = NULL ;
			break ;
			
		case QDMA_CALLBACK_EPON_MAC_HANDLER :
			qdmaDisableInt(base, INT_MASK_EPON_MAC, QDMA_INT1, QDMA_INT_ENABLE0) ;
			gpQdmaPriv->devCfg.bmEponMacIntHandler = NULL ;
			break ;
			
		case QDMA_CALLBACK_XPON_PHY_HANDLER :
			qdmaDisableInt(base, INT_MASK_XPON_PHY, QDMA_INT1, QDMA_INT_ENABLE0) ;
			gpQdmaPriv->devCfg.bmXponPhyIntHandler = NULL ;
			break ;
#endif
        /* multi-INTs extend*/
       case QDMA_CALLBACK_INT2_RX_PACKET:
           gpQdmaPriv->devCfg.bmInt2RecvCallbackFunction = NULL ;   
           break ;

       case QDMA_CALLBACK_INT2_EVENT_HANDLER:
           gpQdmaPriv->devCfg.bmInt2EventCallbackFunction = NULL ;    
           break ;
           
       case QDMA_CALLBACK_INT3_RX_PACKET:
           gpQdmaPriv->devCfg.bmInt3RecvCallbackFunction = NULL ;   
           break ;

       case QDMA_CALLBACK_INT3_EVENT_HANDLER:
           gpQdmaPriv->devCfg.bmInt3EventCallbackFunction = NULL ;    
           break ;

       case QDMA_CALLBACK_INT4_RX_PACKET:
           gpQdmaPriv->devCfg.bmInt4RecvCallbackFunction = NULL ;   
           break ;

       case QDMA_CALLBACK_INT4_EVENT_HANDLER:
           gpQdmaPriv->devCfg.bmInt4EventCallbackFunction = NULL ;    
           break ;
			
		default:
			return -EFAULT ;	
	}
	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
int qdma_enable_rxpkt_int(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaEnableInt(base, INT1_MASK_RX_DONE_LOW_16_RING, QDMA_INT1, QDMA_INT_ENABLE2) ;
	qdmaEnableInt(base, INT1_MASK_RX_DONE_HIGH_16_RING, QDMA_INT1, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
	qdmaEnableInt(base, INT1_MASK_RX_DONE , QDMA_INT1, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
	qdmaEnableInt(base, INT1_MASK_RX_DONE_0_1, QDMA_INT1, QDMA_INT_ENABLE0) ;
	qdmaEnableInt(base, INT1_MASK_RX_DONE_2_15, QDMA_INT1, QDMA_INT_ENABLE1) ;
#endif
	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_disable_rxpkt_int(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;	
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaDisableInt(base, INT1_MASK_RX_DONE_LOW_16_RING, QDMA_INT1, QDMA_INT_ENABLE2) ;
	qdmaDisableInt(base, INT1_MASK_RX_DONE_HIGH_16_RING, QDMA_INT1, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
	qdmaDisableInt(base, INT1_MASK_RX_DONE, QDMA_INT1, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
	qdmaDisableInt(base, INT1_MASK_RX_DONE_0_1, QDMA_INT1, QDMA_INT_ENABLE0) ;
	qdmaDisableInt(base, INT1_MASK_RX_DONE_2_15, QDMA_INT1, QDMA_INT_ENABLE1) ;
#endif
	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
int qdma_enable_rxpkt_int2(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaEnableInt(base, INT2_MASK_RX_DONE_LOW_16_RING, QDMA_INT2, QDMA_INT_ENABLE2) ;
	qdmaEnableInt(base, INT2_MASK_RX_DONE_HIGH_16_RING, QDMA_INT2, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
    qdmaEnableInt(base, INT2_MASK_RX_DONE , QDMA_INT2, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
    qdmaEnableInt(base, INT2_MASK_RX_DONE_0_1, QDMA_INT2, QDMA_INT_ENABLE0) ;
    qdmaEnableInt(base, INT2_MASK_RX_DONE_2_15, QDMA_INT2, QDMA_INT_ENABLE1) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_disable_rxpkt_int2(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;	
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaDisableInt(base, INT2_MASK_RX_DONE_LOW_16_RING, QDMA_INT2, QDMA_INT_ENABLE2) ;
	qdmaDisableInt(base, INT2_MASK_RX_DONE_HIGH_16_RING, QDMA_INT2, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
    qdmaDisableInt(base, INT2_MASK_RX_DONE, QDMA_INT2, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
    qdmaDisableInt(base, INT2_MASK_RX_DONE_0_1, QDMA_INT2, QDMA_INT_ENABLE0) ;
    qdmaDisableInt(base, INT2_MASK_RX_DONE_2_15, QDMA_INT2, QDMA_INT_ENABLE1) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
int qdma_enable_rxpkt_int3(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaEnableInt(base, INT3_MASK_RX_DONE_LOW_16_RING, QDMA_INT3, QDMA_INT_ENABLE2) ;
	qdmaEnableInt(base, INT3_MASK_RX_DONE_HIGH_16_RING, QDMA_INT3, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
    qdmaEnableInt(base, INT3_MASK_RX_DONE , QDMA_INT3, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
    qdmaEnableInt(base, INT3_MASK_RX_DONE_0_1, QDMA_INT3, QDMA_INT_ENABLE0) ;
    qdmaEnableInt(base, INT3_MASK_RX_DONE_2_15, QDMA_INT3, QDMA_INT_ENABLE1) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_disable_rxpkt_int3(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;	
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaDisableInt(base, INT3_MASK_RX_DONE_LOW_16_RING, QDMA_INT3, QDMA_INT_ENABLE2) ;
	qdmaDisableInt(base, INT3_MASK_RX_DONE_HIGH_16_RING, QDMA_INT3, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
    qdmaDisableInt(base, INT3_MASK_RX_DONE, QDMA_INT3, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
    qdmaDisableInt(base, INT3_MASK_RX_DONE_0_1, QDMA_INT3, QDMA_INT_ENABLE0) ;
    qdmaDisableInt(base, INT3_MASK_RX_DONE_2_15, QDMA_INT3, QDMA_INT_ENABLE1) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
int qdma_enable_rxpkt_int4(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaEnableInt(base, INT4_MASK_RX_DONE_LOW_16_RING, QDMA_INT4, QDMA_INT_ENABLE2) ;
	qdmaEnableInt(base, INT4_MASK_RX_DONE_HIGH_16_RING, QDMA_INT4, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
    qdmaEnableInt(base, INT4_MASK_RX_DONE , QDMA_INT4, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
    qdmaEnableInt(base, INT4_MASK_RX_DONE_0_1, QDMA_INT4, QDMA_INT_ENABLE0) ;
    qdmaEnableInt(base, INT4_MASK_RX_DONE_2_15, QDMA_INT4, QDMA_INT_ENABLE1) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_disable_rxpkt_int4(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;	
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaDisableInt(base, INT4_MASK_RX_DONE_LOW_16_RING, QDMA_INT4, QDMA_INT_ENABLE2) ;
	qdmaDisableInt(base, INT4_MASK_RX_DONE_HIGH_16_RING, QDMA_INT4, QDMA_INT_ENABLE3) ;
#elif defined(TCSUPPORT_CPU_EN7580)
    qdmaDisableInt(base, INT4_MASK_RX_DONE, QDMA_INT4, QDMA_INT_ENABLE2) ;
#else /*defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)*/
    qdmaDisableInt(base, INT4_MASK_RX_DONE_0_1, QDMA_INT4, QDMA_INT_ENABLE0) ;
    qdmaDisableInt(base, INT4_MASK_RX_DONE_2_15, QDMA_INT4, QDMA_INT_ENABLE1) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_receive_packets(struct ECNT_QDMA_Data *qdma_data)
{
	int nums=0, curNum=0, nPkts=0, rxRingIdx=0 ;
	int ring_empty_flag[RX_RING_NUM], emptyFlag=0 ;
	int maxPkts = qdma_data->qdma_private.cnt ;

	if(maxPkts == 0)
	{
#ifdef CONFIG_SUPPORT_SELF_TEST
		maxPkts = gpQdmaPriv->hwFwdDscpNum;
#else
		for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++)
		{
			if( int1_rx_ring & (0x1 << rxRingIdx) )
				maxPkts += CONFIG_RX_DSCP_NUM[rxRingIdx] ;
		}
#endif
	}

	do{ 
		for(rxRingIdx=RX_RING_NUM-1; rxRingIdx>=0; rxRingIdx--) 
		{
			ring_empty_flag[rxRingIdx] = 0 ;
			/*EN7516/80: should only receive rx-rings binded to INT-1*/
			if( 0 == (int1_rx_ring & (0x1 << rxRingIdx)) )
			{
				ring_empty_flag[rxRingIdx] = 1 ;
				continue ;
			}

			nPkts = min(maxPkts-nums, RX_DSCP_BUDGET[rxRingIdx]);
			if(nPkts) 
			{	
				/*start to receive packet*/
				curNum = qdma_bm_receive_packets(nPkts, rxRingIdx) ;
				if(curNum<nPkts)
					ring_empty_flag[rxRingIdx] = 1;
				if(curNum>nPkts)
					QDMA_ERR("Error-Ring%d: curNum:%d, nPkts:%d, maxPkts:%d, nums:%d.\n", rxRingIdx, curNum, nPkts, maxPkts, nums) ;
				nums += curNum;
			}
		}
		
		emptyFlag = 1;
		for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) {
			/* if one rx-ring is not empty, then the total emptyFlag is 0 */
			if(ring_empty_flag[rxRingIdx] == 0)
				emptyFlag = 0;
		}
		/* if all rx ring is empty, then exit */
		if(emptyFlag == 1)
			break;
	} while(nums<maxPkts) ;
	
	if(nums>maxPkts)
		QDMA_ERR("Error: nums:%d, maxPkts:%d.\n", nums, maxPkts) ; 
	
	return nums;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_receive_packets_int2(struct ECNT_QDMA_Data *qdma_data)
{
	int nums=0, curNum=0, nPkts=0, rxRingIdx=0 ;
	int ring_empty_flag[RX_RING_NUM], emptyFlag=0 ;
	int maxPkts = qdma_data->qdma_private.cnt ;

	if(maxPkts == 0)
	{
#ifdef CONFIG_SUPPORT_SELF_TEST
		maxPkts = gpQdmaPriv->hwFwdDscpNum;
#else
		for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++)
		{
			if( int2_rx_ring & (0x1 << rxRingIdx) )
				maxPkts += CONFIG_RX_DSCP_NUM[rxRingIdx] ;
		}
#endif
	}

	do{   
		for(rxRingIdx=RX_RING_NUM-1; rxRingIdx>=0; rxRingIdx--) 
		{
			ring_empty_flag[rxRingIdx] = 0 ;
			/*EN7516/80: should only receive rx-rings binded to INT-2*/
			if( 0 == (int2_rx_ring & (0x1 << rxRingIdx)) )
			{
				ring_empty_flag[rxRingIdx] = 1 ;
				continue ;
			}

			nPkts = min(maxPkts-nums, RX_DSCP_BUDGET[rxRingIdx]);
			if(nPkts) 
			{	
				/*start to receive packet*/
				curNum = qdma_bm_receive_packets_int2(nPkts, rxRingIdx) ;
				if(curNum<nPkts)
					ring_empty_flag[rxRingIdx] = 1;
				if(curNum>nPkts)
					QDMA_ERR("Error-Ring%d: curNum:%d, nPkts:%d, maxPkts:%d, nums:%d.\n", rxRingIdx, curNum, nPkts, maxPkts, nums) ;
				nums += curNum;
			}
		}
		
		emptyFlag = 1;
		for(rxRingIdx=RX_RING_NUM-1; rxRingIdx>=0; rxRingIdx--) 
		{
			/* if one rx-ring is not empty, then the total emptyFlag is 0 */
			if(ring_empty_flag[rxRingIdx] == 0)
				emptyFlag = 0;
		}
		/* if all rx ring is empty, then exit */
		if(emptyFlag == 1)
			break;
	} while(nums<maxPkts) ;
	
	if(nums>maxPkts)
		QDMA_ERR("Error: nums:%d, maxPkts:%d.\n", nums, maxPkts) ; 
	
	return nums;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_receive_packets_int3(struct ECNT_QDMA_Data *qdma_data)
{
	int nums=0, curNum=0, nPkts=0, rxRingIdx=0 ;
	int ring_empty_flag[RX_RING_NUM], emptyFlag=0 ;
	int maxPkts = qdma_data->qdma_private.cnt ;

	if(maxPkts == 0)
	{
#ifdef CONFIG_SUPPORT_SELF_TEST
		maxPkts = gpQdmaPriv->hwFwdDscpNum;
#else
		for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++)
		{
			if( int3_rx_ring & (0x1 << rxRingIdx) )
				maxPkts += CONFIG_RX_DSCP_NUM[rxRingIdx] ;
		}
#endif
	}

	do{ 
		for(rxRingIdx=RX_RING_NUM-1; rxRingIdx>=0; rxRingIdx--) 
		{
			ring_empty_flag[rxRingIdx] = 0 ;
			/*EN7516/80: should only receive rx-rings binded to INT-3*/
			if( 0 == (int3_rx_ring & (0x1 << rxRingIdx)) )
			{
				ring_empty_flag[rxRingIdx] = 1 ;
				continue ;
			}

			nPkts = min(maxPkts-nums, RX_DSCP_BUDGET[rxRingIdx]);
			if(nPkts) 
			{	
				/*start to receive packet*/
				curNum = qdma_bm_receive_packets_int3(nPkts, rxRingIdx) ;
				if(curNum<nPkts)
					ring_empty_flag[rxRingIdx] = 1;
				if(curNum>nPkts)
					QDMA_ERR("Error-Ring%d: curNum:%d, nPkts:%d, maxPkts:%d, nums:%d.\n", rxRingIdx, curNum, nPkts, maxPkts, nums) ;
				nums += curNum;
			}
		}
		
		emptyFlag = 1;
		for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) {
			/* if one rx-ring is not empty, then the total emptyFlag is 0 */
			if(ring_empty_flag[rxRingIdx] == 0)
				emptyFlag = 0;
		}
		/* if all rx ring is empty, then exit */
		if(emptyFlag == 1)
			break;
	} while(nums<maxPkts) ;
	
	if(nums>maxPkts)
		QDMA_ERR("Error: nums:%d, maxPkts:%d.\n", nums, maxPkts) ; 
	
	return nums;
}

/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_receive_packets_int4(struct ECNT_QDMA_Data *qdma_data)
{
	int nums=0, curNum=0, nPkts=0, rxRingIdx=0 ;
	int ring_empty_flag[RX_RING_NUM], emptyFlag=0 ;
	int maxPkts = qdma_data->qdma_private.cnt ;

	if(maxPkts == 0)
	{
#ifdef CONFIG_SUPPORT_SELF_TEST
		maxPkts = gpQdmaPriv->hwFwdDscpNum;
#else
		for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++)
		{
			if( int4_rx_ring & (0x1 << rxRingIdx) )
				maxPkts += CONFIG_RX_DSCP_NUM[rxRingIdx] ;
		}
#endif
	}

	do{ 
		for(rxRingIdx=RX_RING_NUM-1; rxRingIdx>=0; rxRingIdx--) 
		{
			ring_empty_flag[rxRingIdx] = 0 ;
			/*EN7516/80: should only receive rx-rings binded to INT-4*/
			if( 0 == (int4_rx_ring & (0x1 << rxRingIdx)) )
			{
				ring_empty_flag[rxRingIdx] = 1 ;
				continue ;
			}

			nPkts = min(maxPkts-nums, RX_DSCP_BUDGET[rxRingIdx]);
			if(nPkts) 
			{	
				/*start to receive packet*/
				curNum = qdma_bm_receive_packets_int4(nPkts, rxRingIdx) ;
				if(curNum<nPkts)
					ring_empty_flag[rxRingIdx] = 1;
				if(curNum>nPkts)
					QDMA_ERR("Error-Ring%d: curNum:%d, nPkts:%d, maxPkts:%d, nums:%d.\n", rxRingIdx, curNum, nPkts, maxPkts, nums) ;
				nums += curNum;
			}
		}
		
		emptyFlag = 1;
		for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) {
			/* if one rx-ring is not empty, then the total emptyFlag is 0 */
			if(ring_empty_flag[rxRingIdx] == 0)
				emptyFlag = 0;
		}
		/* if all rx ring is empty, then exit */
		if(emptyFlag == 1)
			break;
	} while(nums<maxPkts) ;
	
	if(nums>maxPkts)
		QDMA_ERR("Error: nums:%d, maxPkts:%d.\n", nums, maxPkts) ; 
	
	return nums;
}

static inline unsigned char getQueueInfoBySkb(struct sk_buff *skb)
{	
	char *vlan_p = NULL, *ether_type_ptr = NULL;
	unsigned char txq=0;

	if( skb->mark & (1 << QUQUE_REMARK_EXIST_OFFSET) )
	{
		/*if queue is remark, get txq according to queue filed in skb->mark*/
		txq = (skb->mark >> QUEUE_REMARK_OFFSET) & QUQUE_REMARK_MASK;
	}
	else
	{
		/*if queue is not remark, get txq according to pbit*/	
		ether_type_ptr = skb->data + 2 * ETH_ALEN ;/*skip mac header*/
		if( *(unsigned short *)ether_type_ptr == htons(ETH_P_8021Q) )/*if has vlan layer*/
		{
			vlan_p = ether_type_ptr + 2 ; /*pointer to priority*/
			txq = ( (*(unsigned short*)vlan_p) >> VLAN_PRIO_SHIFT ) & PRIO_REMARK_MASK ;/*use 3 bits priority as Q*/
		}
	}
	
	//printk("getQueueInfoBySkb:---skb->mark = 0x%x, get bind txq = %d\r\n", skb->mark, txq);
	return txq;
}

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
int qdma_set_accout_meter_info_to_txmsg(struct sk_buff *skb, txMsgWord_t *pTxMsg)
{
	int dir = 2;
    int temp_id = 0x7f;

#ifdef QDMA_LAN
	dir = 0;
#else
	dir = 1;
#endif
	FE_API_GET_ACNT0_IDX(skb, dir, &temp_id);
	pTxMsg->pTxMsgW1->acnt_g0 = temp_id;
	FE_API_GET_ACNT1_IDX(skb, dir, &temp_id);
	pTxMsg->pTxMsgW1->acnt_g1 = temp_id;
	FE_API_GET_ACNT2_IDX(skb, dir, &temp_id);
	pTxMsg->pTxMsgW2->acnt_g2 = temp_id;
	FE_API_GET_METER_IDX(skb, dir, &temp_id, 0);
	pTxMsg->pTxMsgW1->mtr_g = temp_id;
	FE_API_GET_METER_IDX(skb, dir, &temp_id, 1);
	pTxMsg->pTxMsgW2->mtr_g1 = temp_id;
	FE_API_GET_METER_IDX(skb, dir, &temp_id, 2);
	pTxMsg->pTxMsgW2->mtr_g2 = temp_id;

	return 0;
}
#endif


/******************************************************************************
 Descriptor:
 Input Args:
 Ret Value:	
******************************************************************************/
__IMEM int qdma_transmit_packet(struct ECNT_QDMA_Data *qdma_data)
{
	struct sk_buff *skb = qdma_data->qdma_private.qdma_transmit.skb ;
 
	uint msg0 = qdma_data->qdma_private.qdma_transmit.pTxMsg->txmsg0;
	uint msg1 = qdma_data->qdma_private.qdma_transmit.pTxMsg->txmsg1;
    uint msg2 = qdma_data->qdma_private.qdma_transmit.pTxMsg->txmsg2;

	struct port_info *pMacInfo = qdma_data->qdma_private.qdma_transmit.pMacInfo ;
    int ringIdx = 0 ;
	int priority = 0;
	uint base = gpQdmaPriv->csrBaseAddr ;
    txMsgWord_t txMsg, *pTxMsg = &txMsg ;
    uint channel = 0, queue = 0 ;
	unsigned int qos_car_queue = 0;
	int ret = 0;

	QDMA_TxQosScheduler_T txQos = {0};
	struct ECNT_QDMA_Data qdma_data_t;

    pTxMsg->pTxMsgW0 = (txMsgWord0_t *)&msg0 ;
    pTxMsg->pTxMsgW1 = (txMsgWord1_t *)&msg1 ;
    pTxMsg->pTxMsgW2 = (txMsgWord2_t *)&msg2 ;

    if(!skb)
	{
		QDMA_ERR("The input arguments are wrong, skb is NULL") ; 
		return -EFAULT ;
	}
    
#if defined(CONFIG_SUPPORT_SELF_TEST)
	if((selfTestFlag == 1) && (skb->dropcount != 0xdeadbeef)) 
	{
		printk("catch a packet from normal path, selfTestFlag:%d, dropcount:0x%08x, loopcnt:%d\n", selfTestFlag, skb->dropcount, msg0>>24);
		return -EFAULT ;
	}
	skb->dropcount = 0;
#endif
#if defined(TCSUPPORT_IFC_EN)
	if((ifcTestFlag == 1) && (skb->dropcount != 0xdeadbeef)) 
	{
		printk("catch a packet from normal path, ifcTestFlag:%d, dropcount:0x%08x\n", ifcTestFlag, skb->dropcount);
		return -EFAULT ;
	}
	skb->dropcount = 0;
#endif

#ifndef QDMA_LAN
	if(atomic_read(&qdma_stop_flag) == 1)
	{
		QDMA_MSG(DBG_WARN, "qdma tx stop now, skb:%.8x, skbLen:%d.\n", (uint)skb, skb->len) ; 
		return -EFAULT ;
	}
#endif
	
#ifdef CONFIG_SUPPORT_SELF_TEST
#if defined(TCSUPPORT_CPU_EN7580)
	ringIdx = skb->data[28];    /*tx ring idx stored in data[28]*/
	if( 0x00 == skb->data[24] ) /*recover the data[28] info*/
		skb->data[28] = skb->data[25] ;
#else
    if(pTxMsg->pTxMsgW0->oam) {
        ringIdx = RING_IDX_1 ;
    } 
#endif
#else
    if(pMacInfo == NULL) {
        pTxMsg->pTxMsgW0->queue = 0;
    }else if(pMacInfo->txq_is_valid && !pMacInfo->eth_is_wan) {
        pTxMsg->pTxMsgW0->queue = pMacInfo->txq;	/*if txq which is set by pon driver is valid , txq(HW) will overlap the txmsg(SW)*/
    } else {
#if defined(QDMA_LAN)
#ifdef SUPPORT_IFC
		priority = getPktPriority(skb, skb->data, pMacInfo);
		if(priority > 0)
		{
			ringIdx = RING_IDX_1 ;
			pTxMsg->pTxMsgW0->queue = priority ;
			pMacInfo->txq_is_valid = 1;
			pMacInfo->txq = pTxMsg->pTxMsgW0->queue ;
		}
#else
#if defined(TCSUPPORT_CT_JOYME4)
        ringIdx = qdma_get_ringIdx(skb, pMacInfo) ;
#else
        ringIdx = RING_IDX_0;
#endif
        if(ringIdx == RING_IDX_1) {
            priority = HIGHSET_PRIORITY ;
            pTxMsg->pTxMsgW0->queue = priority ;
            pMacInfo->txq_is_valid = 1;
            pMacInfo->txq = pTxMsg->pTxMsgW0->queue ;
        }
#endif
        else {
#if defined(TCSUPPORT_TSO_VERIFY)
            if(pTxMsg->pTxMsgW0->oam) {
                ringIdx = RING_IDX_1 ;
                pTxMsg->pTxMsgW0->oam = 0;
            } else {
                ringIdx = RING_IDX_0 ;
            }
#else
#if defined(TCSUPPORT_TSO_V2_ENABLE) /*TSO V2 Support*/
            if(skb->tso_mark & TSO_ENABLE_MARK) { /*match tso rule*/
                /*if channel value is odd num,set ringIdx as 1;if channel value is even num,set ringIdx as 0*/
                if(pTxMsg->pTxMsgW0->channel & 1 ) {
                    ringIdx = RING_IDX_1 ;
                } else {
                    ringIdx = RING_IDX_0 ;
                }
            } else {
                ringIdx = RING_IDX_0 ;
            }	
#else
            ringIdx = RING_IDX_0 ;
#endif
            if(TCSUPPORT_FWC_QOS_VAL || TCSUPPORT_DZS_VAL) {
                priority = getQueueInfoBySkb(skb);
                pTxMsg->pTxMsgW0->queue = priority ;
            } else {
                pTxMsg->pTxMsgW0->queue = 0 ;
            }
			if(dnstream_qos_enable == 1)
			{
				priority = qdma_qos_mark(skb);
				pTxMsg->pTxMsgW0->queue = priority ;
			}
#endif
        }
#else
    if(pTxMsg->pTxMsgW0->oam) {    /* oam==1 equal to pMacInfo==NULL */
        ringIdx = RING_IDX_1 ;
        priority = HIGHSET_PRIORITY ;
    } else {
        #ifdef TCSUPPORT_QOS
#ifdef SUPPORT_IFC
        priority = getPktPriority(skb, skb->data, pMacInfo);
        if(priority > 0)
        {
            ringIdx = RING_IDX_1;
            pTxMsg->pTxMsgW0->queue = priority ;
            pMacInfo->txq_is_valid = 1;
            pMacInfo->txq = pTxMsg->pTxMsgW0->queue ;
        }
#else
        ringIdx = qdma_get_ringIdx(skb, pMacInfo) ;
        if(ringIdx == RING_IDX_1) {
            priority = HIGHSET_PRIORITY ;
        } else if(ringIdx == RING_IDX_2) {
        	ringIdx = RING_IDX_1;
            priority = 6 ;
            pTxMsg->pTxMsgW0->queue = priority ;
            pMacInfo->txq_is_valid = 1;
            pMacInfo->txq = pTxMsg->pTxMsgW0->queue ;
        }
#endif
		else {    
			if(TCSUPPORT_FWC_QOS_VAL || TCSUPPORT_DZS_VAL)
			{
				priority = getQueueInfoBySkb(skb);
			}
			else
			{
				priority = qdma_qos_mark(skb);
				if(soft_cirpir_rate_check_hook)
				{
					qos_car_queue = (skb->mark&0xf0)>>4;					
					if((0<qos_car_queue)&&(qos_car_queue<=8)){
					
						ret = soft_cirpir_rate_check_hook(skb,qos_car_queue-1,&priority);
						
						if(ret == -1){  // drop
							gpQdmaPriv->counters.car_queue_drop[qos_car_queue-1]++ ;
							return -EFAULT ;
						}	
						else if(ret == 1)
						{
							gpQdmaPriv->counters.car_queue[priority]++ ;
						}
					}else{
						gpQdmaPriv->counters.car_queue[priority]++ ;
					}
				}            
			}
			
            if(priority == -EFAULT) {
        		QDMA_ERR("priority is wrong: the value of priority is %d.\n", priority) ; 
                return -EFAULT;
            }
        }
        #else
        priority = qdma_qos_remark(skb);
        if(priority == -1) {
            priority = 0 ;
        }
        #endif
    }
    if(ringIdx == -1)
    {
        return -EFAULT ;  /* packet parser and drop*/
    }
    pTxMsg->pTxMsgW0->queue = priority ;
#endif
    }
#endif

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	qdma_set_accout_meter_info_to_txmsg(skb, pTxMsg);
#endif

	/* recycle TX DSCP when send packets in tx polling mode */
	if(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE) {
		if(qdmaGetIrqEntryLen(base) >= gpQdmaPriv->devCfg.txRecycleThreshold) {
			qdma_bm_transmit_done(gpQdmaPriv->devCfg.txRecycleThreshold<<2) ;
		}
	}
    
    channel = pTxMsg->pTxMsgW0->channel ;
    queue = pTxMsg->pTxMsgW0->queue ;    
#ifdef QDMA_LAN 			
	//if enable 247, for 6.2.x queue would be overwriten
	ECNT_API_BBF247_SET_DS_TXQ(skb,&queue,&txQos);  //cann't &pTxMsg->pTxMsgW0->queue
	pTxMsg->pTxMsgW0->queue = queue;
	qdma_data_t.qdma_private.qdma_tx_qos.pTxQos = &txQos;
	qdma_set_tx_qos(&qdma_data_t);
	QDMA_MSG(DBG_ST, "bbf247 ds queue = %u\n",queue);
#else		 //qdma_wan 	
	//if enable 247, for 6.2.X queue would be overwriten
	ECNT_API_BBF247_SET_US_TXQ(skb,&queue);  //cann't &pTxMsg->pTxMsgW0->queue
	pTxMsg->pTxMsgW0->queue = queue;
	QDMA_MSG(DBG_ST, "bbf247 us queue = %u\n",queue);
#endif

#if !defined(TCSUPPORT_TSO_VERIFY)&&!defined(TCSUPPORT_AUTOBENCH)
    if(macResourceLimit(skb, channel, queue) != 0) {
		QDMA_MSG(DBG_ST, "resource limit: chnlLimit[%d]:%d, queueLimit[%d][%d]:%d, skb->len:%d, msg0:0x%08x.\n", 
            channel, atomic_read(&chnlLimit[channel]), channel, queue, atomic_read(&queueLimit[channel][queue]), skb->len, msg0) ;
        return -EFAULT;
    }
#endif
    
#if defined(TCSUPPORT_QDMA_VERIFY)
    if((dropBroadcastMulticastFlag & 0x1) != 0) {   // drop broadcast pkts
        if((skb->data[0] == 0xFF) && (skb->data[1] == 0xFF) && (skb->data[2] == 0xFF) 
        && (skb->data[3] == 0xFF) && (skb->data[4] == 0xFF) && (skb->data[5] == 0xFF)) {
            return -EFAULT ;
        }
    }
    if((dropBroadcastMulticastFlag & 0x2) != 0) {   // drop multicast pkts
        if(((skb->data[0]&0x1) == 1) || ((skb->data[0] == 0x33) && (skb->data[1] == 0x33))) {
            return -EFAULT ;
        }
    }
#endif

    if(pMacInfo && ra_sw_nat_hook_tx) {
        pMacInfo->txq = pTxMsg->pTxMsgW0->queue ;

#if !defined(QDMA_LAN)
#if defined(TCSUPPORT_CT_SWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)	
    if((!sw_isSWQosActive)
        ||(sw_isSWQosActive && !sw_isSWQosActive())
        ||(sw_isSWQosActive && sw_isSWQosActive() && sw_isHwnatOffloadEnable && sw_isHwnatOffloadEnable()))
#endif
#endif
		{
	        /* move foe hook function to here */
			if (ra_sw_nat_hook_tx(skb, pMacInfo, pMacInfo->magic) == 0) {
				QDMA_MSG(DBG_ST, "this is duplicate packet in keepalive new header mode, just drop it \n") ;
				return -EBADR;
			}
		}
    }

#if !defined(QDMA_LAN)
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER) && SUPPORT_ETHERWAN_USE_QDMAWAN_FPORT_TO_GDM1
	if(g_wan_mode == USE_QDMA_WAN_ETHER)
	{
		pTxMsg->pTxMsgW1->fport = 1;
	}
#endif
#endif

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
	if(SmbdTxSpeedOn && skb->smbd_on_speed){/* enable ICO */
        qdma_set_txmsg_ico(pTxMsg,1);
	}
#endif


#if defined(QDMA_LAN) && defined(TCSUPPORT_ACCOUNT_METER_V2)	
/*upstream*/
	if(fe_resource_mark_llid_hook){
		fe_resource_mark_llid_hook(skb, pTxMsg->pTxMsgW0->channel, 1); 
		QDMA_MSG(DBG_ST, "upstream :: llid = %d \n", pTxMsg->pTxMsgW0->channel) ;
	}
/*downstream mark in epon*/	
#endif

#if defined(TCSUPPORT_TSO_ENABLE) && !defined(TCSUPPORT_QDMA_VERIFY)
#if defined(TCSUPPORT_TSO_V2_ENABLE) /*TSO V2 Support*/
    if((skb->tso_mark & TSO_ENABLE_MARK)||(skb_shinfo(skb)->nr_frags || skb_shinfo(skb)->gso_size)){ /*match tso rule*/
#if defined(QDMA_LAN)
        return TSO_TRANSMIT_PACKET(1,skb,msg0,msg1,ringIdx);
#else   
        return TSO_TRANSMIT_PACKET(0,skb,msg0,msg1,ringIdx);
#endif  
    }else{
        return qdma_bm_transmit_packet(skb, ringIdx, pTxMsg) ;
    }
#else /*TSO V1 Support*/
	return qdma_bm_transmit_packet_tso(skb, ringIdx, msg0, msg1) ;
#endif
#else
	return qdma_bm_transmit_packet(skb, ringIdx, pTxMsg) ;
#endif
}

/******************************************************************************
 Descriptor:	It's used to configure the TXQOS weight type and scale.
 Input Args:	arg1: setting the WRR weighting value is base on packet or byte
 					  (QDMA_TXQOS_WEIGHT_BY_PACKET/QDMA_TXQOS_WEIGHT_BY_BYTE)
 				arg2: setting the byte weighting scale(QDMA_TXQOS_WEIGHT_SCALE_64B
 				      /QDMA_TXQOS_WEIGHT_SCALE_16B). when the weigthing value 
 				      is base on packet, these argument is don't care.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_tx_qos_weight(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQosWeightType_t weightBase = qdma_data->qdma_private.qdma_tx_qos.weightBase ;
	QDMA_TxQosWeightScale_t weightScale = qdma_data->qdma_private.qdma_tx_qos.weightScale ;

	if(weightBase == QDMA_TXQOS_WEIGHT_BY_PACKET) {
		qdmaSetTxQosWeightByPacket(base) ;
	} else if(weightBase == QDMA_TXQOS_WEIGHT_BY_BYTE) {
		qdmaSetTxQosWeightByByte(base) ;
#if SUPPORT_WRR_WEIGHT_SCALE_1Byte
		if(weightScale == QDMA_TXQOS_WEIGHT_SCALE_1B) {
			qdmaSetTxQosWeightScale1(base) ;
		} else if(weightScale == QDMA_TXQOS_WEIGHT_SCALE_16B) {
			qdmaSetTxQosWeightScale16(base) ;
		} else {
			return -EINVAL ;
		}
#else
		if(weightScale == QDMA_TXQOS_WEIGHT_SCALE_64B) {
			qdmaSetTxQosWeightScale64(base) ;
		} else if(weightScale == QDMA_TXQOS_WEIGHT_SCALE_16B) {
			qdmaSetTxQosWeightScale16(base) ;
		} else {
			return -EINVAL ;
		}
#endif
	} else {
		return -EINVAL ;
	}
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get the TXQOS weight type and scale.
 Input Args:	arg1: the pointer of the weight base value 
 				      (QDMA_TXQOS_WEIGHT_BY_PACKET/QDMA_TXQOS_WEIGHT_BY_BYTE)
 				arg2: the pointer of the weight scale value
 					  (QDMA_TXQOS_WEIGHT_SCALE_64B/QDMA_TXQOS_WEIGHT_SCALE_16B)
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_tx_qos_weight(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(qdmaIsTxQosWeightByByte(base)) {
		qdma_data->qdma_private.qdma_tx_qos.weightBase = QDMA_TXQOS_WEIGHT_BY_BYTE ;
	} else {
		qdma_data->qdma_private.qdma_tx_qos.weightBase = QDMA_TXQOS_WEIGHT_BY_PACKET ;
	}
	
	if(qdmaIsTxQosWeightScale16(base)) {
		qdma_data->qdma_private.qdma_tx_qos.weightScale = QDMA_TXQOS_WEIGHT_SCALE_16B ;
	} else {
#if SUPPORT_WRR_WEIGHT_SCALE_1Byte
		qdma_data->qdma_private.qdma_tx_qos.weightScale = QDMA_TXQOS_WEIGHT_SCALE_1B ;
#else
		qdma_data->qdma_private.qdma_tx_qos.weightScale = QDMA_TXQOS_WEIGHT_SCALE_64B ;
#endif
	}
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to configure the tx queue scheduler and queue weigth
 				for specific channel.
 Input Args:	The pointer of the tx qos scheduler struct. It includes:
 				- channel: specific the channel ID (0~15)
 				- qosType: The QoS type is define in QDMA_TxQosType_t enum.
 				- weight: The unit of WRR weight is packets (0~100, 255 means don't care).
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_tx_qos(struct ECNT_QDMA_Data *qdma_data) 
{
    uint base = gpQdmaPriv->csrBaseAddr ;
	int ret = 0, i = 0, j = 0;
	unchar qosType = 0 ;
	ushort weight[CONFIG_QDMA_QUEUE];
    unchar qosStr[128], qosStrTmp[16] ;
	int weightNum[QDMA_TXQOS_TYPE_NUMS]={8, 0, 7, 6, 5, 4, 3, 2};
	QDMA_TxQosScheduler_T *pTxQos = qdma_data->qdma_private.qdma_tx_qos.pTxQos ;
	
	if(pTxQos->channel >= CONFIG_QDMA_CHANNEL) {
		return -EINVAL ;
	}
	
	if(pTxQos->qosType >= QDMA_TXQOS_TYPE_NUMS) {
		return -EINVAL ;
	}
	qosType = (unchar)pTxQos->qosType;

#if SUPPORT_QUEUE_DISABLE
    for(i=0 ; i<CONFIG_QDMA_QUEUE ; i++) {
        if( i >= weightNum[qosType] ){/*SP : open the queue*/
            qdmaSetQueueOpen_sw(base,pTxQos->channel,i) ;
        }else{/*WRR: check weight */
            if(pTxQos->queue[i].weight == 0){
                qdmaSetQueueClose_sw(base,pTxQos->channel,i) ;
            }else{
                qdmaSetQueueOpen_sw(base,pTxQos->channel,i) ;
            }
        }
    }

#elif 0
    for(i=0 ; i<CONFIG_QDMA_QUEUE ; i++) {
        if( i >= weightNum[qosType] ){/*SP : open the queue*/
            qdmaSetQueueOpen(base,pTxQos->channel,i) ;
        }else{/*WRR: check weight */
            if(pTxQos->queue[i].weight == 0){
                qdmaSetQueueClose(base,pTxQos->channel,i) ;
            }else{
                qdmaSetQueueOpen(base,pTxQos->channel,i) ;
            }
        }
    }
#else
	for(i=0 ; i<weightNum[pTxQos->qosType] ; i++) {
		if(pTxQos->queue[i].weight == 0) {	
            /* if fixed weight has value 0 in new qosType, then config weight, but use old qosType  */
			ret = qdmaGetTxQosScheduler(pTxQos->channel, &qosType, weight) ;
			if(ret < 0) {
				QDMA_ERR("qdmaGetTxQosScheduler error\n");
				return -EFAULT ;
			}
			for(j=0 ; j<weightNum[qosType] ; j++) {
				if(pTxQos->queue[j].weight == 0) {	/* if fixed weight has value 0 in old qosType, then exit  */
					QDMA_ERR("channel:%d, old qosTYpe:%d, qosWeight[%d] is %d\n", pTxQos->channel, qosType, j, pTxQos->queue[j].weight);
					return -EFAULT ;
				}
			}
			sprintf(qosStr, "channel is %d, old qosType is %d, new qosType is %d, Q0:Q1:...:Q7=", pTxQos->channel, qosType, pTxQos->qosType);
			for(j=0 ; j<CONFIG_QDMA_QUEUE ; j++) {
				sprintf(qosStrTmp, "%d ", pTxQos->queue[j].weight);
				strcat(qosStr, qosStrTmp);
			}
			strcat(qosStr, "\n");
			QDMA_MSG(DBG_ST, "%s", qosStr);
		}
	}
#endif

	for(i=0 ; i<CONFIG_QDMA_QUEUE ; i++) {
#if SUPPORT_QUEUE_DISABLE
        /*in case ,the packet queued in QDMA*/
        /*if wrr=0 , set wrr=1 and close the queue*/
        /*if DE add ageout function, need to be deleted*/
        if(pTxQos->queue[i].weight == 0)
            pTxQos->queue[i].weight = 1 ;
#else
		if((pTxQos->queue[i].weight>100) && (pTxQos->queue[i].weight!=255)) {
			return -EINVAL ;
		}
#endif
		weight[i] = pTxQos->queue[i].weight ;
	}
	
	return qdmaSetTxQosScheduler(pTxQos->channel, qosType, weight) ;
}

/******************************************************************************
 Descriptor:	It's used to set the tx queue scheduler and queue weigth
 				for specific channel.
 Input Args:	The pointer of the tx qos scheduler struct. It includes:
 				- channel: specific the channel ID (0~15)
 				- qosType: The QoS type is define in QDMA_TxQosType_t enum.
 				- weight: The unit of WRR weight is packets.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_tx_qos(struct ECNT_QDMA_Data *qdma_data)
{
    uint base = gpQdmaPriv->csrBaseAddr ;
	int ret = 0, i = 0 ;
	unchar qosType = 0 ;
	ushort weight[CONFIG_QDMA_QUEUE] ;
	QDMA_TxQosScheduler_T *pTxQos = qdma_data->qdma_private.qdma_tx_qos.pTxQos ;
	
	if(pTxQos->channel >= CONFIG_QDMA_CHANNEL) {
		return -EINVAL ;
	}

	ret = qdmaGetTxQosScheduler(pTxQos->channel, &qosType, weight) ;
	if(ret < 0) {
		return -EFAULT ;
	}
	
	pTxQos->qosType = qosType ;
	
	for(i=0 ; i<CONFIG_QDMA_QUEUE ; i++) {
    //if DE add ageout function, need to be deleted
#if SUPPORT_QUEUE_DISABLE/*if wrr=0 , set wrr=1 and close the queue*/
        if((weight[i]==1) && (qdmaIsQueueClosed_sw(base,pTxQos->channel,i)>0))
            weight[i] = 0 ;
#endif
		pTxQos->queue[i].weight = weight[i] ;
	}

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to set channel/queue limit threshold for mac driver.
 Input Args:	arg1: channel limit threshold
                     arg2: queue limit threshold
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_mac_limit_threshold(struct ECNT_QDMA_Data *qdma_data)
{
    channel_limit_threshold = qdma_data->qdma_private.qdma_mac_limit_thrh->chnlThrh ;
    queue_limit_threshold = qdma_data->qdma_private.qdma_mac_limit_thrh->queueThrh ;    
    return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get channel/queue limit threshold for mac driver.
 Input Args:	arg1: channel limit threshold
                     arg2: queue limit threshold
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_get_mac_limit_threshold(struct ECNT_QDMA_Data *qdma_data)
{
    qdma_data->qdma_private.qdma_mac_limit_thrh->chnlThrh = channel_limit_threshold ;
    qdma_data->qdma_private.qdma_mac_limit_thrh->queueThrh = queue_limit_threshold ;    
    return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to configure the tx buffer threshold. For the buffer 
 				management, the total available on-chip buffer is 64Kbyte (256 
 				Blocks, 256 bytes per block). It is shared among WAN and LAN 
 				Tx/Rx interface. If the buffer usage exceeds the threshold, the
 				Tx DMA will stop retrieving packets.
 Input Args:	The pointer of the tx buffer control struct. It includes:
 				- mode: Eanble/Disable tx buffer usage control
 				- chnThreshold: Per tx per channel block usage threshold.
 				- totalThreshold: Total tx block usage threshold.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txbuf_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxBufCtrl_T *pTxBufCtrl = qdma_data->qdma_private.pQdmaTxBufCtrl ;

    qdma_set_txbuf_threshold_inline(base,pTxBufCtrl->mode,pTxBufCtrl->chnThreshold,pTxBufCtrl->totalThreshold);

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get the tx buffer threshold.
 Input Args:	The pointer of the tx buffer control struct. It includes:
 				- mode: Eanble/Disable tx buffer usage control
 				- chnThreshold: Per tx per channel block usage threshold.
 				- totalThreshold: Total tx block usage threshold.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_txbuf_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxBufCtrl_T *pTxBufCtrl = qdma_data->qdma_private.pQdmaTxBufCtrl ;
    QDMA_Mode_t	mode;
	unsigned short  chnThreshold;
	unsigned short  totalThreshold;
    
    qdma_get_txbuf_threshold_inline(base,&mode,&chnThreshold,&totalThreshold);
    
	pTxBufCtrl->mode = mode;
	pTxBufCtrl->chnThreshold = chnThreshold;
	pTxBufCtrl->totalThreshold = totalThreshold;

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable QDMA pre-fetch function. Since the
                on-chip buffer resource is limited, to prevent from Head-Of-Line
                blocking issue, 'when' to retrieving packet from External DRAM 
                to on-chip buffer will be application dependent.
 Input Args:	arg1: Enable/disable TXDMA pre-fetch function (QDMA_ENABLE/QDMA_DISABLE)
 			arg2: Enable/disable TXDMA pre-fetch overdrag function (QDMA_ENABLE/QDMA_DISABLE)
 			arg3: set TXDMA pre-fetch overdrag count value
 Ret Value:	No return value
******************************************************************************/
int qdma_set_prefetch_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_PrefetchMode_T *qdma_prefetch = qdma_data->qdma_private.qdma_prefetch;

    return qdma_set_prefetch_mode_inline(base,qdma_prefetch->prefecthMode,qdma_prefetch->overDragCnt);
}

/******************************************************************************
 Descriptor:	It's used to enable or disable pktsize overhead function.
 Input Args:	arg1: mode means enable or disable.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_pktsize_overhead_en(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_Mode_t mode = qdma_data->qdma_private.mode ;

	if(mode == QDMA_DISABLE) {
		qdmaDisablePktSizeOverHead(base) ;
	} else {
		qdmaEnablePktSizeOverHead(base) ;
	}

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get pktsize overhead function is enable or disable.
 Input Args:	None
 Ret Value:	0: Disable, 1: Enable.
******************************************************************************/
int qdma_get_pktsize_overhead_en(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(qdmaIsPktSizeOverHeadEnable(base)) {
		qdma_data->qdma_private.mode = QDMA_ENABLE ;
	} else {
		qdma_data->qdma_private.mode = QDMA_DISABLE ;
	}

	return qdma_data->qdma_private.mode ;
}

/******************************************************************************
 Descriptor:	It's used to set the value of pktsize overhead.
 Input Args:	arg1: pktsize overhead value
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_pktsize_overhead_value(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint cnt = qdma_data->qdma_private.cnt ;
	qdmaSetPktSizeOverHeadVal(base, cnt);
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get the value of pktsize overhead.
 Input Args:	None
 Ret Value:	pktsize overhead value.
******************************************************************************/
int qdma_get_pktsize_overhead_value(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	qdma_data->qdma_private.cnt = qdmaGetPktSizeOverHeadVal(base) ;
	return qdma_data->qdma_private.cnt ;
}

/******************************************************************************
 Descriptor:	It's used to set the value of lmgr low threshold.
 Input Args:	arg1: lmgr low threshold value
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_lmgr_low_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint lowThrh = qdma_data->qdma_private.threshold ;
	qdmaSetHwLowThrshld(base, lowThrh);
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get the value of lmgr low threshold.
 Input Args:	None
 Ret Value:	lmgr low threshold value.
******************************************************************************/
int qdma_get_lmgr_low_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	qdma_data->qdma_private.threshold = qdmaGetHwLowThrshld(base) ;
	return qdma_data->qdma_private.threshold ;
}

/******************************************************************************
 Descriptor:	It's used to get lmgr status: free lmgr dscp, used lmgr dscp, 
 			and used total buffer usage.
 Input Args:	arg1: to get free lmgr dscp counter
 			arg2: to get used lmgr dscp counter
 			arg3: to get used total buffer usage
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_get_lmgr_status(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint freeLmgr = qdmaLmgrFreeCount(base) ;

	qdma_data->qdma_private.qdma_lmgr_status->freeLmgr = freeLmgr;
	qdma_data->qdma_private.qdma_lmgr_status->usedLmgr = gpQdmaPriv->hwFwdDscpNum - freeLmgr;
	qdma_data->qdma_private.qdma_lmgr_status->usedBuf = qdmaLmgrTotalBufUsage(base);

	return 0 ;
}


/******************************************************************************
 Descriptor:	It's used to clear all the qdma cpu counters.
 Input Args:	None
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_clear_cpu_counters(struct ECNT_QDMA_Data *qdma_data)
{
	memset(&gpQdmaPriv->counters, 0, sizeof(BM_Counters_T)) ;
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to dump all the qdma cpu counters.
 Input Args:	None
 Ret Value:	No return value
******************************************************************************/
int qdma_dump_cpu_counters(struct ECNT_QDMA_Data *qdma_data)
{
	int ringIdx=0 ;

	printk("IRQ Interrupt:                     %u\n", gpQdmaPriv->counters.intIrqcnt) ;
	printk("IRQ Queue Asynchronous:            %u\n", gpQdmaPriv->counters.IrqQueueAsynchronous) ;
	printk("IRQ Queue Index Errors:            %u\n", gpQdmaPriv->counters.txIrqQueueIdxErrs) ;
	printk("IRQ Queue Full Interrupt:          %u\n", gpQdmaPriv->counters.intIrqFull) ;
	printk("IRQ Queue NULL Number:             %u\n", gpQdmaPriv->counters.IrqErr) ;
#if SUPPORT_IRQ2
	printk("IRQ2 Interrupt:                    %u\n", gpQdmaPriv->counters.intIrq2cnt) ;
	printk("IRQ2 Queue Asynchronous	    	   %u\n", gpQdmaPriv->counters.Irq2QueueAsynchronous) ;
	printk("IRQ2 Queue Index Errors:           %u\n", gpQdmaPriv->counters.txIrq2QueueIdxErrs) ;
	printk("IRQ2 Queue Full Interrupt:         %u\n", gpQdmaPriv->counters.intIrq2Full) ;
	printk("IRQ2 Queue NULL Number:            %u\n", gpQdmaPriv->counters.Irq2Err) ;
#endif
	printk("RX Allocate Fail Errors:           %u\n", gpQdmaPriv->counters.rxAllocFailErrs) ;
	printk("No Link DSCP Interrupt:            %u\n", gpQdmaPriv->counters.intNoLinkDscp) ;
	printk("Low Link DSCP Interrupt:           %u\n", gpQdmaPriv->counters.intLowLinkDscp) ;
	printk("Tx Packets Free Counts:            %u\n", gpQdmaPriv->counters.txPktsFreeCounts) ;
	printk("Tx Packets Free Errs:              %u\n", gpQdmaPriv->counters.txPktsFreeErrs) ;
	printk("Tx Mac Resource Drops:             %u\n", gpQdmaPriv->counters.txMacLimitDropCnt);
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
	printk("Tx Force Ring0 Counts:             %u\n", gpQdmaPriv->counters.txForceRing0Cnt);
#endif/*TCSUPPORT_COMPILE*/	
	printk("API Index Errors:				   %u\n", gpQdmaPriv->counters.apiIdxErrs);
	printk("--------------------------------------------\n") ;

	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		printk("Tx%d DSCP Counts:                    %u\n", ringIdx, gpQdmaPriv->counters.txCounts[ringIdx]) ;
		printk("Tx%d DSCP Recycle Counts:            %u\n", ringIdx, gpQdmaPriv->counters.txRecycleCounts[ringIdx]) ;
		printk("Tx%d DSCP Content Incorrect:         %u\n", ringIdx, gpQdmaPriv->counters.txDscpIncorrect[ringIdx]) ;
		printk("Tx%d Done Bit Errors:                %u\n", ringIdx, gpQdmaPriv->counters.txDscpDoneErrs[ringIdx]) ;
#if SUPPORT_TX_DROP_BIT
		printk("Tx%d Drop Bit Errors:                %u\n", ringIdx, gpQdmaPriv->counters.txDscpDropErrs[ringIdx]) ;
#endif
		printk("No Tx%d DSCP Errors:                 %u\n", ringIdx, gpQdmaPriv->counters.noTxDscps[ringIdx]) ;
		printk("Tx%d Chorent Interrupt:              %u\n", ringIdx, gpQdmaPriv->counters.intTxCoherent[ringIdx]) ;
		printk("No Tx%d Buffer Interrupt:            %u\n", ringIdx, gpQdmaPriv->counters.intNoTxBuff[ringIdx]) ;
		printk("No Tx%d DSCP Interrupt:              %u\n", ringIdx, gpQdmaPriv->counters.intNoTxDscp[ringIdx]) ;
		printk("Tx%d Done Interrupt:                 %u\n", ringIdx, gpQdmaPriv->counters.intTxDone[ringIdx]) ;
		printk("--------------------------------------------\n") ;
	}

	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
		printk("Rx%d DSCP Counts:                    %u\n", ringIdx, gpQdmaPriv->counters.rxCounts[ringIdx]) ;
		printk("Rx%d DSCP Content Incorrect:         %u\n", ringIdx, gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]) ;
		printk("Rx%d Packet Errors:                  %u\n", ringIdx, gpQdmaPriv->counters.rxPktErrs[ringIdx]) ;
		printk("No Rx%d Callback Errors:             %u\n", ringIdx, gpQdmaPriv->counters.noRxCbErrs[ringIdx]) ;
		printk("No Rx%d DSCP Errors:                 %u\n", ringIdx, gpQdmaPriv->counters.noRxDscps[ringIdx]) ;
		printk("Rx%d Allocate Fail Drop Errors:      %u\n", ringIdx, gpQdmaPriv->counters.rxAllocFailDropErrs[ringIdx]) ;
		printk("Rx%d Chorent Interrupt:              %u\n", ringIdx, gpQdmaPriv->counters.intRxCoherent[ringIdx]) ;
		printk("No Rx%d Buffer Interrupt:            %u\n", ringIdx, gpQdmaPriv->counters.intNoRxBuff[ringIdx]) ;
		printk("No Rx%d DSCP Interrupt:              %u\n", ringIdx, gpQdmaPriv->counters.intNoRxDscp[ringIdx]) ;
		printk("Rx%d Broadcast Interrupt:            %u\n", ringIdx, gpQdmaPriv->counters.rxBroadcastCounts[ringIdx]) ;
		printk("Rx%d Unknown Unicast Interrupt:      %u\n", ringIdx, gpQdmaPriv->counters.rxUnknownUnicastCounts[ringIdx]) ;
		printk("Rx%d Multicast Interrupt:            %u\n", ringIdx, gpQdmaPriv->counters.rxMulticastCounts[ringIdx]) ;
		printk("Rx%d Done Interrupt:                 %u\n", ringIdx, gpQdmaPriv->counters.intRxDone[ringIdx]) ;
		printk("--------------------------------------------\n") ;
	}
	
	for(ringIdx=0; ringIdx<CONFIG_QDMA_QUEUE; ringIdx++) {
		printk("CAR Queue %d tx Counts:      %u,  tx drop Counts:      %u\n", 
			ringIdx, gpQdmaPriv->counters.car_queue[ringIdx],
			gpQdmaPriv->counters.car_queue_drop[ringIdx]) ;
	
	}
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to dump all qdma related register values.
 Input Args:	None
 Ret Value:	No return value
******************************************************************************/
int qdma_dump_register_value(struct ECNT_QDMA_Data *qdma_data)
{
	int i=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	printk("Tx Mode %s \n",gpQdmaPriv->devCfg.flags.isTxPolling?"Polling":"Interrupt");
	printk("Rx Mode %s \n",gpQdmaPriv->devCfg.flags.isRxPolling?"Polling":
		gpQdmaPriv->devCfg.flags.isRxNapi?"Napi":"Interrupt");

#if SUPPORT_HWDSCP_IN_SRAM
	printk("Hwf Dscp Ram Mode:   %s\n", (qdmaDscpMode == QDMA_SRAM_MODE)?"SRAM":"DRAM");
#endif
#if SUPPORT_HWDSCP_PAYLOAD_CFG
	printk("Hwf Payload Size:    %d Byte\n", CONFIG_PAYLOAD_SIZE[qdmaPayloadMode]);
#endif
	printk("gpQdmaPriv->csrBaseAddr:					 0x%08x\n", gpQdmaPriv->csrBaseAddr) ;
	printk("gpQdmaPriv->hwFwdDscpNum:					 %u\n", gpQdmaPriv->hwFwdDscpNum) ;
	printk("gpQdmaPriv->hwFwdDscpMaxNum:					 %u\n", gpQdmaPriv->hwFwdDscpMaxNum) ;
	printk("gpQdmaPriv->irqDepth:						 %u\n", gpQdmaPriv->irqDepth) ;
	printk("gpQdmaPriv->hwPktSize:						 %u\n", gpQdmaPriv->hwPktSize) ;
	printk("gpQdmaPriv->dscpInfoAddr:					 0x%08x\n", gpQdmaPriv->dscpInfoAddr) ;
	printk("gpQdmaPriv->irqQueueAddr:					 0x%08x\n", gpQdmaPriv->irqQueueAddr) ;
	printk("gpQdmaPriv->hwFwdBaseAddr:					 0x%08x\n", gpQdmaPriv->hwFwdBaseAddr) ;
	printk("gpQdmaPriv->hwFwdBuffAddr:					 0x%08x\n", gpQdmaPriv->hwFwdBuffAddr) ;
	printk("gpQdmaPriv->hwFwdPayloadSize:					 %u\n\n", gpQdmaPriv->hwFwdPayloadSize) ;
	
	for(i=0; i<TX_RING_NUM; i++) {
		printk("gpQdmaPriv->txDscpNum[%d]:					  %u\n", i, gpQdmaPriv->txDscpNum[i]) ;
		printk("gpQdmaPriv->txBaseAddr[%d]: 				  0x%08x\n", i, gpQdmaPriv->txBaseAddr[i]) ;
		printk("gpQdmaPriv->txHeadPtr[%d]->dscpIdx: 		  %d\n", i, (gpQdmaPriv->txHeadPtr[i]?(gpQdmaPriv->txHeadPtr[i]->dscpIdx):(-1))) ;
		printk("gpQdmaPriv->txTailPtr[%d]->dscpIdx: 		  %d\n", i, (gpQdmaPriv->txTailPtr[i]?(gpQdmaPriv->txTailPtr[i]->dscpIdx):(-1))) ;
		printk("gpQdmaPriv->txUsingPtr[%d]->dscpIdx:		  %d\n\n", i, (gpQdmaPriv->txUsingPtr[i]?(gpQdmaPriv->txUsingPtr[i]->dscpIdx):(-1))) ;
	}
	for(i=0; i<RX_RING_NUM; i++) {
		printk("gpQdmaPriv->rxDscpNum[%d]:					  %u\n", i, gpQdmaPriv->rxDscpNum[i]) ;
		printk("gpQdmaPriv->rxBaseAddr[%d]: 				  0x%08x\n", i, gpQdmaPriv->rxBaseAddr[i]) ;
		printk("gpQdmaPriv->rxStartPtr[%d]->dscpIdx:		  %d\n", i, (gpQdmaPriv->rxStartPtr[i]?(gpQdmaPriv->rxStartPtr[i]->dscpIdx):(-1))) ;
		printk("gpQdmaPriv->rxEndPtr[%d]->dscpIdx:			  %d\n", i, (gpQdmaPriv->rxEndPtr[i]?(gpQdmaPriv->rxEndPtr[i]->dscpIdx):(-1))) ;
		printk("gpQdmaPriv->rxUsingPtr[%d]->dscpIdx:		  %d\n\n", i, (gpQdmaPriv->rxUsingPtr[i]?(gpQdmaPriv->rxUsingPtr[i]->dscpIdx):(-1))) ;
	}

	printk("0x%08x: QDMA_CSR_INFO                           :%.8x\n", QDMA_CSR_INFO(base), IO_GREG(QDMA_CSR_INFO(base))) ;
	printk("0x%08x: QDMA_CSR_GLB_CFG                        :%.8x\n", QDMA_CSR_GLB_CFG(base), IO_GREG(QDMA_CSR_GLB_CFG(base))) ;

#if SUPPORT_8_TX_RING
	for(i=0; i<TX_RING_NUM; i++) {
		printk("0x%08X: QDMA_CSR_TX_DSCP_BASE[%d]                :%.8x\n", QDMA_CSR_TX_DSCP_BASE(base, i), i, IO_GREG(QDMA_CSR_TX_DSCP_BASE(base, i))) ;
		printk("0x%08X: QDMA_CSR_TX_BLOCKING[%d]                 :%.8x\n", QDMA_CSR_TX_BLOCKING(base, i), i, IO_GREG(QDMA_CSR_TX_BLOCKING(base, i))) ;
		printk("0x%08X: QDMA_CSR_TX_CPU_IDX[%d]                  :%.8x\n", QDMA_CSR_TX_CPU_IDX(base, i), i, IO_GREG(QDMA_CSR_TX_CPU_IDX(base, i))) ;
		printk("0x%08X: QDMA_CSR_TX_DMA_IDX[%d]                  :%.8x\n", QDMA_CSR_TX_DMA_IDX(base, i), i, IO_GREG(QDMA_CSR_TX_DMA_IDX(base, i))) ;
	}
	for(i=0; i<RX_RING_NUM; i++) {
		printk("0x%08X: QDMA_CSR_RX_DSCP_BASE[%d]                :%.8x\n", QDMA_CSR_RX_DSCP_BASE(base, i), i, IO_GREG(QDMA_CSR_RX_DSCP_BASE(base, i))) ;
		printk("0x%08X: QDMA_CSR_RX_RING_SIZE[%d]                :%.8x\n", QDMA_CSR_RX_RING_SIZE(base, i), i, IO_GREG(QDMA_CSR_RX_RING_SIZE(base, i))) ;
		printk("0x%08X: QDMA_CSR_RX_CPU_IDX[%d]                  :%.8x\n", QDMA_CSR_RX_CPU_IDX(base, i), i, IO_GREG(QDMA_CSR_RX_CPU_IDX(base, i))) ;
		printk("0x%08X: QDMA_CSR_RX_DMA_IDX[%d]                  :%.8x\n", QDMA_CSR_RX_DMA_IDX(base, i), i, IO_GREG(QDMA_CSR_RX_DMA_IDX(base, i))) ;
		printk("0x%08X: QDMA_CSR_RX_DELAY_INT_CFG[%d]            :%.8x\n", QDMA_CSR_RX_DELAY_INT_CFG(base, i), i, IO_GREG(QDMA_CSR_RX_DELAY_INT_CFG(base, i))) ;
	}
	printk("0x%08X: QDMA_CSR_HWFWD_DSCP_BASE                :%.8x\n", QDMA_CSR_HWFWD_DSCP_BASE(base), IO_GREG(QDMA_CSR_HWFWD_DSCP_BASE(base))) ;
	printk("0x%08X: QDMA_CSR_HWFWD_BUFF_BASE                :%.8x\n", QDMA_CSR_HWFWD_BUFF_BASE(base), IO_GREG(QDMA_CSR_HWFWD_BUFF_BASE(base))) ;
	printk("0x%08X: QDMA_CSR_HWFWD_DSCP_CFG                 :%.8x\n", QDMA_CSR_HWFWD_DSCP_CFG(base), IO_GREG(QDMA_CSR_HWFWD_DSCP_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_LMGR_INIT_CFG                  :%.8x\n", QDMA_CSR_LMGR_INIT_CFG(base), IO_GREG(QDMA_CSR_LMGR_INIT_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_CRX_PKT_OVH_CFG                :%.8x\n", QDMA_CSR_CRX_PKT_OVH_CFG(base), IO_GREG(QDMA_CSR_CRX_PKT_OVH_CFG(base))) ;

	printk("0x%08X: QDMA_CSR_INT_STATUS1                     :%.8x\n", QDMA_CSR_INT_STATUS1(base), IO_GREG(QDMA_CSR_INT_STATUS1(base))) ;
	printk("0x%08X: QDMA_CSR_INT_STATUS2                     :%.8x\n", QDMA_CSR_INT_STATUS2(base), IO_GREG(QDMA_CSR_INT_STATUS2(base))) ;
	for(i=1; i<5; i++)/*INT1~INT4*/
	{
		printk("0x%08X: QDMA_CSR_INT%d_ENABLE1                    :%.8x\n", QDMA_CSR_INT_EN(base,i,QDMA_INT_ENABLE1), i, IO_GREG(QDMA_CSR_INT_EN(base,i,QDMA_INT_ENABLE1))) ;
		printk("0x%08X: QDMA_CSR_INT%d_ENABLE2                    :%.8x\n", QDMA_CSR_INT_EN(base,i,QDMA_INT_ENABLE2), i, IO_GREG(QDMA_CSR_INT_EN(base,i,QDMA_INT_ENABLE2))) ;
	}

	printk("0x%08X: QDMA_CSR_IRQ_BASE                       :%.8x\n", QDMA_CSR_IRQ_BASE(base), IO_GREG(QDMA_CSR_IRQ_BASE(base))) ;
	printk("0x%08X: QDMA_CSR_IRQ_CFG                        :%.8x\n", QDMA_CSR_IRQ_CFG(base), IO_GREG(QDMA_CSR_IRQ_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_IRQ_CLEAR_LEN                  :%.8x\n", QDMA_CSR_IRQ_CLEAR_LEN(base), IO_GREG(QDMA_CSR_IRQ_CLEAR_LEN(base))) ;
	printk("0x%08X: QDMA_CSR_IRQ_STATUS                     :%.8x\n", QDMA_CSR_IRQ_STATUS(base), IO_GREG(QDMA_CSR_IRQ_STATUS(base))) ;
	printk("0x%08X: QDMA_CSR_IRQ_PTIME                      :%.8x\n", QDMA_CSR_IRQ_PTIME(base), IO_GREG(QDMA_CSR_IRQ_PTIME(base))) ;
	for(i=0; i<TXQ_DIS_CFG_REG_NUM; i++)
	{
		printk("0x%08X: QDMA_CSR_TXQ_DIS_CFG_CHN%d_%d                    :%.8x\n", (base+0x00a0+i*4), i*4+3, i*4, TXQ_DIS_CFG_VALUE[i]) ;
	}
	printk("0x%08X: QDMA_CSR_TXWRR_MODE_CFG                 :%.8x\n", QDMA_CSR_TXWRR_MODE_CFG(base), IO_GREG(QDMA_CSR_TXWRR_MODE_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_TXWRR_WEIGHT_CFG               :%.8x\n", QDMA_CSR_TXWRR_WEIGHT_CFG(base), IO_GREG(QDMA_CSR_TXWRR_WEIGHT_CFG(base))) ;

	printk("0x%08X: QDMA_CSR_EGRESS_RATEMETER_CFG           :%.8x\n", QDMA_CSR_EGRESS_RATEMETER_CFG(base), IO_GREG(QDMA_CSR_EGRESS_RATEMETER_CFG(base))) ;
	
	printk("0x%08X: QDMA_CSR_MIN_DSCPTHR_CFG                :%.8x\n", QDMA_CSR_TXQ_MIN_DSCP_THRH(base), IO_GREG(QDMA_CSR_TXQ_MIN_DSCP_THRH(base))) ;
	printk("0x%08X: QDMA_CSR_TXQ_CNGST_CFG                  :%.8x\n", QDMA_CSR_TXQ_CNGST_CFG(base), IO_GREG(QDMA_CSR_TXQ_CNGST_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_TXQ_DYN_TOTALTHR               :%.8x\n", QDMA_CSR_TXQ_DYN_TOTALTHR(base), IO_GREG(QDMA_CSR_TXQ_DYN_TOTALTHR(base))) ;
	printk("0x%08X: QDMA_CSR_TXQ_DYN_CHNLTHR_CFG            :%.8x\n", QDMA_CSR_TXQ_DYN_CHNLTHR_CFG(base), IO_GREG(QDMA_CSR_TXQ_DYN_CHNLTHR_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_TXQ_DYN_QUEUETHR_CFG           :%.8x\n", QDMA_CSR_TXQ_DYN_QUEUETHR_CFG(base), IO_GREG(QDMA_CSR_TXQ_DYN_QUEUETHR_CFG(base))) ;

#if SUPPORT_HQOS
	printk("0x%08X: QDMA_CSR_HQOS_MODE_CFG                  :%.8x\n", QDMA_CSR_HQOS_MODE_CFG(base), IO_GREG(QDMA_CSR_HQOS_MODE_CFG(base))) ;

	printk("0x%08X: QDMA_CSR_TXQ_STATIC_THR_CFG             :%.8x\n", QDMA_CSR_STATIC_THR_CFG(base), IO_GREG(QDMA_CSR_STATIC_THR_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_STATIC_QUEUE_RATIO             :%.8x\n", QDMA_CSR_STATIC_QUEUE_RATIO(base), IO_GREG(QDMA_CSR_STATIC_QUEUE_RATIO(base))) ;
	printk("0x%08X: QDMA_CSR_TXQ_DEI_TOTALTHR               :%.8x\n", QDMA_CSR_TXQ_DEI_TOTALTHR(base), IO_GREG(QDMA_CSR_TXQ_DEI_TOTALTHR(base))) ;
	printk("0x%08X: QDMA_CSR_TXQ_DEI_CHNLTHR_CFG            :%.8x\n", QDMA_CSR_TXQ_DEI_CHNLTHR_CFG(base), IO_GREG(QDMA_CSR_TXQ_DEI_CHNLTHR_CFG(base))) ;
	printk("0x%08X: QDMA_CSR_TXQ_DEI_QUEUETHR_CFG           :%.8x\n", QDMA_CSR_TXQ_DEI_QUEUETHR_CFG(base), IO_GREG(QDMA_CSR_TXQ_DEI_QUEUETHR_CFG(base))) ;

	printk("0x%08X: QDMA_CSR_QOS_AGING_CFG           :%.8x\n", QDMA_CSR_QOS_AGING_CFG(base), IO_GREG(QDMA_CSR_QOS_AGING_CFG(base))) ;
	for(i=0; i<8; i++)
		printk("0x%08X: QDMA_CSR_QUEUE_AGING_EN_CFG_%d             :%.8x\n", QDMA_CSR_QUEUE_AGING_EN_CFG(base, i), i, IO_GREG(QDMA_CSR_QUEUE_AGING_EN_CFG(base, i))) ;
#else
	for(i=0; i<8; i++)
		printk("0x0%02X: QDMA_CSR_STATIC_QUEUE_THR_%d             :%.8x\n", 0xB0+(i<<2), i, IO_GREG(QDMA_CSR_STATIC_QUEUE_THR(base, i))) ;
#endif
	printk("0x0F0: QDMA_CSR_DBG_LMGR_STATUS                :%.8x\n", IO_GREG(QDMA_CSR_DBG_LMGR_STATUS(base))) ;
	printk("0x0F4: QDMA_CSR_DBG_LMGR_USAGE                 :%.8x\n", IO_GREG(QDMA_CSR_DBG_LMGR_USAGE(base))) ;
	printk("0x0F8: QDMA_CSR_DBG_QDMA_PROBE_LO              :%.8x\n", IO_GREG(QDMA_CSR_DBG_QDMA_PROBE_LO(base))) ;
	printk("0x0FC: QDMA_CSR_DBG_QDMA_PROBE_HI              :%.8x\n", IO_GREG(QDMA_CSR_DBG_QDMA_PROBE_HI(base))) ;

	for(i=0; i<(CONFIG_QDMA_CHANNEL>>3); i++)
		printk("0x%03x: QDMA_CSR_PERCHNL_QOS_MODE_%02d_%02d         :%.8x\n", 0x040+(i<<2), (i<<3)+7, i<<3, IO_GREG(QDMA_CSR_PERCHNL_QOS_MODE(base, i))) ;
	for(i=0; i<(CONFIG_QDMA_CHANNEL>>1); i++)
		printk("0x%03x: QDMA_CSR_PERCHNL_DATARATE_%02d_%02d         :%.8x\n", 0x200+(i<<2), (i<<1)+1, i<<1, IO_GREG(QDMA_CSR_PERCHNL_DATARATE(base, i))) ;

#else  /*TCSUPPORT_CPU_EN7527 || TCSUPPORT_CPU_EN7516*/
	for(i=0; i<TX_RING_NUM; i++) {
		printk("0x%08x: QDMA_CSR_TX_DSCP_BASE[%d]                :%.8x\n", QDMA_CSR_TX_DSCP_BASE(base, i), i, IO_GREG(QDMA_CSR_TX_DSCP_BASE(base, i))) ;
		printk("0x%08x: QDMA_CSR_TX_CPU_IDX[%d]                  :%.8x\n", QDMA_CSR_TX_CPU_IDX(base, i), i, IO_GREG(QDMA_CSR_TX_CPU_IDX(base, i))) ;
		printk("0x%08x: QDMA_CSR_TX_DMA_IDX[%d]                  :%.8x\n", QDMA_CSR_TX_DMA_IDX(base, i), i, IO_GREG(QDMA_CSR_TX_DMA_IDX(base, i))) ;
	}
	for(i=0; i<RX_RING_NUM; i++) {
		printk("0x%08x: QDMA_CSR_RX_DSCP_BASE[%d]                :%.8x\n", QDMA_CSR_RX_DSCP_BASE(base, i), i, IO_GREG(QDMA_CSR_RX_DSCP_BASE(base, i))) ;
		printk("0x%08x: QDMA_CSR_RX_CPU_IDX[%d]                  :%.8x\n", QDMA_CSR_RX_CPU_IDX(base, i), i, IO_GREG(QDMA_CSR_RX_CPU_IDX(base, i))) ;
		printk("0x%08x: QDMA_CSR_RX_DMA_IDX[%d]                  :%.8x\n", QDMA_CSR_RX_DMA_IDX(base, i), i, IO_GREG(QDMA_CSR_RX_DMA_IDX(base, i))) ;
	}
	printk("0x%08x: QDMA_CSR_HWFWD_DSCP_BASE                :%.8x\n", QDMA_CSR_HWFWD_DSCP_BASE(base), IO_GREG(QDMA_CSR_HWFWD_DSCP_BASE(base))) ;
	printk("0x%08x: QDMA_CSR_HWFWD_BUFF_BASE                :%.8x\n", QDMA_CSR_HWFWD_BUFF_BASE(base), IO_GREG(QDMA_CSR_HWFWD_BUFF_BASE(base))) ;
	printk("0x%08x: QDMA_CSR_HWFWD_DSCP_CFG                 :%.8x\n", QDMA_CSR_HWFWD_DSCP_CFG(base), IO_GREG(QDMA_CSR_HWFWD_DSCP_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_LMGR_INIT_CFG                  :%.8x\n", QDMA_CSR_LMGR_INIT_CFG(base), IO_GREG(QDMA_CSR_LMGR_INIT_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_LMGR_CHNL_RETIRE               :%.8x\n", QDMA_CSR_LMGR_CHNL_RETIRE(base), IO_GREG(QDMA_CSR_LMGR_CHNL_RETIRE(base))) ;
	printk("0x%08x: QDMA_CSR_INT_STATUS                     :%.8x\n", QDMA_CSR_INT_STATUS(base), IO_GREG(QDMA_CSR_INT_STATUS(base))) ;
	printk("0x%08x: QDMA_CSR_INT_ENABLE                     :%.8x\n", QDMA_CSR_INT_ENABLE(base), IO_GREG(QDMA_CSR_INT_ENABLE(base))) ;
	printk("0x%08x: QDMA_CSR_TX_DELAY_INT_CFG               :%.8x\n", QDMA_CSR_TX_DELAY_INT_CFG(base), IO_GREG(QDMA_CSR_TX_DELAY_INT_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_RX_DELAY_INT_CFG               :%.8x\n", QDMA_CSR_RX_DELAY_INT_CFG(base,0), IO_GREG(QDMA_CSR_RX_DELAY_INT_CFG(base,0))) ;
	for(i=2; i<RX_RING_NUM; i++) {
		printk("0x%08x: QDMA_CSR_RX%d_DELAY_INT_CFG				:%.8x\n",QDMA_CSR_RX_DELAY_INT_CFG(base,i), i, IO_GREG(QDMA_CSR_RX_DELAY_INT_CFG(base,i))) ;
	}
	printk("0x%08x: QDMA_CSR_IRQ_BASE                       :%.8x\n", QDMA_CSR_IRQ_BASE(base), IO_GREG(QDMA_CSR_IRQ_BASE(base))) ;
	printk("0x%08x: QDMA_CSR_IRQ_CFG                        :%.8x\n", QDMA_CSR_IRQ_CFG(base), IO_GREG(QDMA_CSR_IRQ_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_IRQ_CLEAR_LEN                  :%.8x\n", QDMA_CSR_IRQ_CLEAR_LEN(base), IO_GREG(QDMA_CSR_IRQ_CLEAR_LEN(base))) ;
	printk("0x%08x: QDMA_CSR_IRQ_STATUS                     :%.8x\n", QDMA_CSR_IRQ_STATUS(base), IO_GREG(QDMA_CSR_IRQ_STATUS(base))) ;
	printk("0x%08x: QDMA_CSR_IRQ_PTIME                      :%.8x\n", QDMA_CSR_IRQ_PTIME(base), IO_GREG(QDMA_CSR_IRQ_PTIME(base))) ;
#if SUPPORT_QUEUE_DISABLE    
    for(i=0; i<TXQ_DIS_CFG_REG_NUM; i++)
	{
		printk("0x%08X: QDMA_CSR_TXQ_DIS_CFG_CHN%d_%d                    :%.8x\n", (base+0x00a0+i*4), i*4+3, i*4, TXQ_DIS_CFG_VALUE[i]) ;
	}
#endif
	printk("0x%08x: QDMA_CSR_TXWRR_MODE_CFG                 :%.8x\n", QDMA_CSR_TXWRR_MODE_CFG(base), IO_GREG(QDMA_CSR_TXWRR_MODE_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_TXWRR_WEIGHT_CFG               :%.8x\n", QDMA_CSR_TXWRR_WEIGHT_CFG(base), IO_GREG(QDMA_CSR_TXWRR_WEIGHT_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_PSE_BUF_USAGE_CFG              :%.8x\n", QDMA_CSR_PSE_BUF_USAGE_CFG(base), IO_GREG(QDMA_CSR_PSE_BUF_USAGE_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_EGRESS_RATEMETER_CFG           :%.8x\n", QDMA_CSR_EGRESS_RATEMETER_CFG(base), IO_GREG(QDMA_CSR_EGRESS_RATEMETER_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_EGRESS_RATELIMIT_CFG           :%.8x\n", QDMA_CSR_EGRESS_RATELIMIT_CFG(base), IO_GREG(QDMA_CSR_EGRESS_RATELIMIT_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_RATELIMIT_PARAMETER_CFG        :%.8x\n", QDMA_CSR_RATELIMIT_PARAMETER_CFG(base), IO_GREG(QDMA_CSR_RATELIMIT_PARAMETER_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_TXQ_CNGST_CFG                  :%.8x\n", QDMA_CSR_TXQ_CNGST_CFG(base), IO_GREG(QDMA_CSR_TXQ_CNGST_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_TXQ_DYN_TOTALTHR               :%.8x\n", QDMA_CSR_TXQ_DYN_TOTALTHR(base), IO_GREG(QDMA_CSR_TXQ_DYN_TOTALTHR(base))) ;
	printk("0x%08x: QDMA_CSR_TXQ_DYN_CHNLTHR_CFG            :%.8x\n", QDMA_CSR_TXQ_DYN_CHNLTHR_CFG(base), IO_GREG(QDMA_CSR_TXQ_DYN_CHNLTHR_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_TXQ_DYN_QUEUETHR_CFG           :%.8x\n", QDMA_CSR_TXQ_DYN_QUEUETHR_CFG(base), IO_GREG(QDMA_CSR_TXQ_DYN_QUEUETHR_CFG(base))) ;
	for(i=0; i<8; i++)
		printk("0x%08x: QDMA_CSR_STATIC_QUEUE_THR_%d             :%.8x\n", QDMA_CSR_STATIC_QUEUE_THR(base, i), i, IO_GREG(QDMA_CSR_STATIC_QUEUE_THR(base, i))) ;
	printk("0x%08x: QDMA_CSR_DBG_LMGR_STATUS                :%.8x\n", QDMA_CSR_DBG_LMGR_STATUS(base), IO_GREG(QDMA_CSR_DBG_LMGR_STATUS(base))) ;
	printk("0x%08x: QDMA_CSR_DBG_LMGR_USAGE                 :%.8x\n", QDMA_CSR_DBG_LMGR_USAGE(base), IO_GREG(QDMA_CSR_DBG_LMGR_USAGE(base))) ;
	printk("0x%08x: QDMA_CSR_DBG_QDMA_PROBE_LO              :%.8x\n", QDMA_CSR_DBG_QDMA_PROBE_LO(base), IO_GREG(QDMA_CSR_DBG_QDMA_PROBE_LO(base))) ;
	printk("0x%08x: QDMA_CSR_DBG_QDMA_PROBE_HI              :%.8x\n", QDMA_CSR_DBG_QDMA_PROBE_HI(base), IO_GREG(QDMA_CSR_DBG_QDMA_PROBE_HI(base))) ;
	printk("0x%08x: QDMA_CSR_RX_RING_CFG                    :%.8x\n", QDMA_CSR_RX_RING_CFG(base), IO_GREG(QDMA_CSR_RX_RING_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_RX_RING_THR                    :%.8x\n", QDMA_CSR_RX_RING_THR(base), IO_GREG(QDMA_CSR_RX_RING_THR(base))) ;
	printk("0x%08x: QDMA_CSR_RX_PROTECT_CFG                 :%.8x\n", QDMA_CSR_RX_PROTECT_CFG(base), IO_GREG(QDMA_CSR_RX_PROTECT_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_RX_RATELIMIT_CFG               :%.8x\n", QDMA_CSR_RX_RATELIMIT_CFG(base), IO_GREG(QDMA_CSR_RX_RATELIMIT_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_RX_TICK_CFG                    :%.8x\n", QDMA_CSR_RX_TICK_CFG(base), IO_GREG(QDMA_CSR_RX_TICK_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_RX_PKTOVH_CFG					:%.8x\n", QDMA_CSR_RX_PKTOVH_CFG(base), IO_GREG(QDMA_CSR_RX_PKTOVH_CFG(base))) ;
	for(i=0; i<(CONFIG_QDMA_CHANNEL>>3); i++)
		printk("0x%08x: QDMA_CSR_PERCHNL_QOS_MODE_%02d_%02d         :%.8x\n", QDMA_CSR_PERCHNL_QOS_MODE(base, i), (i<<3)+7, i<<3, IO_GREG(QDMA_CSR_PERCHNL_QOS_MODE(base, i))) ;
	for(i=0; i<(CONFIG_QDMA_CHANNEL>>1); i++)
		printk("0x%08x: QDMA_CSR_PERCHNL_DATARATE_%02d_%02d         :%.8x\n", QDMA_CSR_PERCHNL_DATARATE(base, i), (i<<1)+1, i<<1, IO_GREG(QDMA_CSR_PERCHNL_DATARATE(base, i))) ;
	for(i=0; i<(CONFIG_QDMA_CHANNEL>>2); i++)
		printk("0x%08x: QDMA_CSR_PERCHNL_QUEUE_NORM_DROP_%02d_%02d  :%.8x\n", QDMA_CSR_PERCHNL_QUEUE_NORM_DROP(base, i), (i<<2)+3, i<<2, IO_GREG(QDMA_CSR_PERCHNL_QUEUE_NORM_DROP(base, i))) ;
	for(i=0; i<(CONFIG_QDMA_CHANNEL>>2); i++)
		printk("0x%08x: QDMA_CSR_PERCHNL_QUEUE_DEI_DROP_%02d_%02d   :%.8x\n", QDMA_CSR_PERCHNL_QUEUE_DEI_DROP(base, i), (i<<2)+3, i<<2, IO_GREG(QDMA_CSR_PERCHNL_QUEUE_DEI_DROP(base, i))) ;
	printk("0x%08x: QDMA_CSR_RATELIMIT_PER_CHNL_TICKSEL				:%.8x\n", QDMA_CSR_RATELIMIT_PER_CHNL_TICKSEL(base), IO_GREG(QDMA_CSR_RATELIMIT_PER_CHNL_TICKSEL(base))) ;
	printk("0x%08x: QDMA_CSR_EGRESS_RATELIMIT_CFG2					:%.8x\n", QDMA_CSR_EGRESS_RATELIMIT_CFG2(base), IO_GREG(QDMA_CSR_EGRESS_RATELIMIT_CFG2(base))) ;
	printk("0x%08x: QDMA_CSR_DBA_CHEAT_CFG							:%.8x\n", QDMA_CSR_DBA_CHEAT_CFG(base), IO_GREG(QDMA_CSR_DBA_CHEAT_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_TXQ_CNGST_QUEUE_NONBLOCKING_CFG		:%.8x\n", QDMA_CSR_TXQ_CNGST_QUEUE_NONBLOCKING_CFG(base), IO_GREG(QDMA_CSR_TXQ_CNGST_QUEUE_NONBLOCKING_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_TXQ_CNGST_CHANNEL_NONBLOCKING_CFG		:%.8x\n", QDMA_CSR_TXQ_CNGST_CHANNEL_NONBLOCKING_CFG(base), IO_GREG(QDMA_CSR_TXQ_CNGST_CHANNEL_NONBLOCKING_CFG(base))) ;
	printk("0x%08x: QDMA_CSR_INT_STATUS1							:%.8x\n", QDMA_CSR_INT_STATUS1(base), IO_GREG(QDMA_CSR_INT_STATUS1(base))) ;
	printk("0x%08x: QDMA_CSR_INT_STATUS2							:%.8x\n", QDMA_CSR_INT_STATUS2(base), IO_GREG(QDMA_CSR_INT_STATUS2(base))) ;
	/*to be continued*/
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to dump all cpu tx/rx dscp information.
 Input Args:	None
 Ret Value:	No return value
******************************************************************************/
int qdma_dump_descriptor_info(struct ECNT_QDMA_Data *qdma_data)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	int i=0, idx=1, ringIdx=0;
    QDMA_DscpDumpInfo_T *dscpDumpInfoPtr=NULL;
    
    if(qdma_data != NULL){
        dscpDumpInfoPtr = qdma_data->qdma_private.dscpDumpInfoPtr;
        if(dscpDumpInfoPtr->dscpDumpMode == QDMA_DUMP_TX_DSCP){
            if(dscpDumpInfoPtr->ringIdx > (TX_RING_NUM-1)){
                QDMA_ERR("Fault: input tx ringIdx should between 0 and %d\n", (TX_RING_NUM-1));
                return -EINVAL ;
            }
        }else if(dscpDumpInfoPtr->dscpDumpMode == QDMA_DUMP_RX_DSCP){
            if(dscpDumpInfoPtr->ringIdx > (RX_RING_NUM-1)){
                QDMA_ERR("Fault: input rx ringIdx should between 0 and %d\n", (RX_RING_NUM-1));
                return -EINVAL ;
            }
        }else{
            QDMA_ERR("Fault: input dump mode error\n");
            return -EINVAL ;
        }
    }

	for(ringIdx=0; ringIdx<TX_RING_NUM ; ringIdx++) {
        if(qdma_data != NULL){
            if( !((dscpDumpInfoPtr->dscpDumpMode == QDMA_DUMP_TX_DSCP)&&(ringIdx == dscpDumpInfoPtr->ringIdx)) )
                continue;
        }
		idx = 1 ;
		diPtr = gpQdmaPriv->txHeadPtr[ringIdx] ;
		printk("Unused Tx%d DSCP Link List:\n", ringIdx) ;
		
		while(diPtr) {
			printk("%d: DSCP Idx:%d, DSCP Ptr:%.8x, Done Bit:%d\n", idx, diPtr->dscpIdx, 
																	(uint)diPtr->dscpPtr, 
																	diPtr->dscpPtr->ctrl.done) ;
#if SUPPORT_TX_DROP_BIT
            printk("drop_bit:%d, nls:%d, pkt_len:%d, pkt_addr:%.8X, next_idx:%d, msg0:%.8X, msg1:%.8X\n",
            diPtr->dscpPtr->ctrl.drop_pkt, diPtr->dscpPtr->ctrl.nls,diPtr->dscpPtr->ctrl.pkt_len,
            diPtr->dscpPtr->pkt_addr,diPtr->dscpPtr->next_idx,diPtr->dscpPtr->msg[0],diPtr->dscpPtr->msg[1]);
#endif

            diPtr = diPtr->next ;
			idx++ ;
		} 
		
		idx = 1 ;
		printk("\nUsing Tx%d DSCP Set:\n", ringIdx) ;
		for(i=0 ; i<gpQdmaPriv->txDscpNum[ringIdx] ; i++) {
			diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + i ;
			if(diPtr->next == NULL && diPtr!=gpQdmaPriv->txTailPtr[ringIdx]) {
				printk("%d: DSCP Idx:%d, DSCP Ptr:%.8x, Done Bit:%d, PktLen:%d, PktAddr:%.8x(%.8x), Next Idx:%d\n", 
																	idx, diPtr->dscpIdx, 
																	(uint)diPtr->dscpPtr, 
																	diPtr->dscpPtr->ctrl.done,
																	diPtr->dscpPtr->ctrl.pkt_len, 
																	(uint)diPtr->skb, 
																	(uint)diPtr->dscpPtr->pkt_addr,
																	diPtr->dscpPtr->next_idx) ;
#if SUPPORT_TX_DROP_BIT
                printk("drop_bit:%d, nls:%d, pkt_len:%d, pkt_addr:%.8X, next_idx:%d, msg0:%.8X, msg1:%.8X\n",
                diPtr->dscpPtr->ctrl.drop_pkt, diPtr->dscpPtr->ctrl.nls,diPtr->dscpPtr->ctrl.pkt_len,
                diPtr->dscpPtr->pkt_addr,diPtr->dscpPtr->next_idx,diPtr->dscpPtr->msg[0],diPtr->dscpPtr->msg[1]);
#endif
				idx++ ;	
			}
			
			if((i&0xFF) == 0xFF) {
				msleep(0) ;
			}
		}
	}
	
	for(ringIdx=0; ringIdx<RX_RING_NUM ; ringIdx++) {
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
		if(is_left_to_right_rx_ring(ringIdx))
			continue;
#endif/*TCSUPPORT_COMPILE*/
        if(qdma_data != NULL){
            if( !((dscpDumpInfoPtr->dscpDumpMode == QDMA_DUMP_RX_DSCP)&&(ringIdx == dscpDumpInfoPtr->ringIdx)) )
                continue;
        }
		idx = 1 ;
		diPtr = gpQdmaPriv->rxStartPtr[ringIdx] ;
		printk("\nRx%d DSCP Ring: RxStartIdx:%d, RxEndIdx:%d\n", ringIdx, gpQdmaPriv->rxStartPtr[ringIdx]->dscpIdx, gpQdmaPriv->rxEndPtr[ringIdx]->dscpIdx) ;
		do {
			if(diPtr) {
				printk("%d: DSCP Idx:%d, DSCP Ptr:%.8x, Done Bit:%d, PktLen:%d, PktAddr:%.8x(%.8x), Next Idx:%d\n", 
																	idx, diPtr->dscpIdx, 
																	(uint)diPtr->dscpPtr, 
																	diPtr->dscpPtr->ctrl.done,
																	diPtr->dscpPtr->ctrl.pkt_len, 
																	(uint)diPtr->skb, 
																	(uint)diPtr->dscpPtr->pkt_addr,
																	diPtr->dscpPtr->next_idx) ;
				diPtr = diPtr->next ;
				idx++ ;
			}
		} while(diPtr!=NULL && diPtr!=gpQdmaPriv->rxStartPtr[ringIdx]) ;
	}
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to dump all cpu tx/rx dscp cnt info.
 Input Args:	None
 Ret Value:	No return value
******************************************************************************/
int qdma_dump_descriptor_cnt(struct ECNT_QDMA_Data *qdma_data)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	int i=0, idx=1, ringIdx=0;
	uint RxCpuIdx = 0 , RxDmaIdx = 0 ;

	for(ringIdx=0 ; ringIdx<TX_RING_NUM ; ringIdx++) {
		printk("Tx%d Total DSCP Num:%d\t",ringIdx,gpQdmaPriv->txDscpNum[ringIdx]);
		
		idx = 0 ;
		for(i=0 ; i<gpQdmaPriv->txDscpNum[ringIdx] ; i++) {
			diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + i ;
			if(diPtr->next == NULL && diPtr!=gpQdmaPriv->txTailPtr[ringIdx]) {
				idx++ ;	
			}
		}
		printk("Using DSCP Num:%d\t", idx) ;

		idx = 0 ;
		diPtr = gpQdmaPriv->txHeadPtr[ringIdx] ;
		while(diPtr) {
			diPtr = diPtr->next ;
			idx++ ;
		}
		printk("Unused DSCP Num:%d\n",idx) ;
		
	}

	printk("\n");
	
	for(ringIdx=0; ringIdx<RX_RING_NUM ; ringIdx++) {
		printk("Rx%d Total DSCP Num:%d\t",ringIdx,gpQdmaPriv->rxDscpNum[ringIdx]);
		RxCpuIdx = qdmaGetRxCpuIdx(gpQdmaPriv->csrBaseAddr, ringIdx); 
		RxDmaIdx = qdmaGetRxDmaIdx(gpQdmaPriv->csrBaseAddr, ringIdx); 

		if( RxDmaIdx > RxCpuIdx )
			printk("Using DSCP Num:%d\n",(RxDmaIdx-RxCpuIdx));
		else
			printk("Using DSCP Num:%d\n",(RxDmaIdx+gpQdmaPriv->rxDscpNum[ringIdx]-RxCpuIdx));
	}
	
	return 0 ;
}


/******************************************************************************
 Descriptor:	It's used to dump all IRQ information.
 Input Args:	None
 Ret Value:	No return value
******************************************************************************/
int qdma_dump_irq_info(struct ECNT_QDMA_Data *qdma_data)
{
	int i=0 ;
	uint *irqPtr=NULL, irqValue=0 ;
	
	for(i=0 ; i<gpQdmaPriv->irqDepth ; i++) {
		irqPtr = (uint *)gpQdmaPriv->irqQueueAddr + i ;
		irqValue = *irqPtr ;
		printk("IRQ Queue:%.4x,     Content:%.8x\n", i, irqValue) ;
		
		if((i&0xFF) == 0xFF) {
			msleep(0) ;
		}
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to dump all IRQ2 information.
 Input Args:	None
 Ret Value:	No return value
******************************************************************************/
int qdma_dump_irq2_info(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_IRQ2
	int i=0 ;
	uint *irq2Ptr=NULL, irq2Value=0 ;
	
	for(i=0 ; i<gpQdmaPriv->irq2Depth ; i++) {
		irq2Ptr = (uint *)gpQdmaPriv->irq2QueueAddr + i ;
		irq2Value = *irq2Ptr ;
		printk("IRQ2 Queue:%.4x,     Content:%.8x\n", i, irq2Value) ;
		
		if((i&0xFF) == 0xFF) {
			msleep(0) ;
		}
	}
#endif
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to dump all hwfwd dscp information.
 Input Args:	None
 Ret Value:	No return value
******************************************************************************/
int qdma_dump_hwfwd_info(struct ECNT_QDMA_Data *qdma_data)
{
	uint i=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_HWFWD_DMA_DSCP_T *pHwDscp=NULL ;
	uint *pHwMsg=NULL, *pHwPkt=NULL ;
	
	printk("Hardware Forwarding DSCP Link List:\n") ;
	for(i=0 ; i<gpQdmaPriv->hwFwdDscpNum ; i++) {
		pHwDscp = (QDMA_HWFWD_DMA_DSCP_T *)(gpQdmaPriv->hwFwdBaseAddr) + i ;
		pHwMsg = pHwDscp->msg ;
		pHwPkt = (uint *)(gpQdmaPriv->hwFwdBuffAddr) + (gpQdmaPriv->hwFwdPayloadSize/sizeof(uint))*i ;
		
		printk("%d: DSCP:%.8x(%.8x), ctx:%s, ctx_idx:%d, MsgAddr:%.8x, PktLen:%d, PktAddr:%.8x(%.8x)\n", 
															i, (uint)pHwDscp, 
															qdmaGetHwDscpBase(base)+i*sizeof(QDMA_HWFWD_DMA_DSCP_T),
															pHwDscp->ctrl.ctx?("CPU"):("HWFWD"),
															pHwDscp->ctrl.ctx_idx,
															(uint)pHwMsg,
															pHwDscp->ctrl.pkt_len,
															(uint)pHwPkt,
															(uint)pHwDscp->pkt_addr) ;
#if SUPPORT_QDMA_MULTICAST
		printk("      mul_first_fdscpidx:%d, mul_remain_pktnum:%d\n" ,(uint)pHwDscp->ctrl.mul_first_idx ,(uint)pHwDscp->ctrl.mul_remain_pktnum);
#endif
		printk("      Msg Content:%.8x, %.8x\n", (uint)(*pHwMsg), (uint)(*(pHwMsg+1))) ;

		if((i&0xFF) == 0xFF) {
			msleep(0) ;
		}
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set dbg level for qdma debug.
 Input Args:	arg1: dbgLevel: 0 for ERROR; 1 for ST; 2 for WARN; 3 for MSG
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_dbg_level(struct ECNT_QDMA_Data *qdma_data)
{
#ifdef CONFIG_DEBUG
	uint dbgLevel = qdma_data->qdma_private.qdma_old_proc.dbgLevel ;

	if(dbgLevel < DBG_LEVEL_MAX) {
		gpQdmaPriv->devCfg.dbgLevel = dbgLevel ;
		printk("Debug Level: %d\n", gpQdmaPriv->devCfg.dbgLevel) ;
		return 0;
	} else {
		printk("Debug Level should less than %d\n", DBG_LEVEL_MAX) ;
		return -EINVAL;
	}
#else
	return 0;
#endif
}

/******************************************************************************
 Descriptor:	It's used to dump tx/rx dma busy bit in several seconds.
 Input Args:	arg1: busyDuration means the seconds of dma busy bit print out. 
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_dump_dma_busy(struct ECNT_QDMA_Data *qdma_data)
{
	uint busyDuration = qdma_data->qdma_private.qdma_old_proc.busyDuration ;
	
	if(busyDuration > QDMA_BUSY_TIMER_MAX) {
		printk("dma busy duration force setting to %d seconds.", QDMA_BUSY_TIMER_MAX);
		busyDuration = QDMA_BUSY_TIMER_MAX;
	}
	dma_busy_round_cnt = busyDuration*100 ;
	tx_dma_busy_enable_cnt = 0 ;
	rx_dma_busy_enable_cnt = 0 ;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 	
	init_timer(&dma_busy_timer) ;
	dma_busy_timer.data = dma_busy_expires ; /* 10 ms */
	dma_busy_timer.function = qdma_dma_busy_timer ;
	QDMA_START_TIMER(dma_busy_timer) ;
#else
	timer_setup(&dma_busy_timer, qdma_dma_busy_timer, 0);
	dma_busy_timer.expires = dma_busy_expires;
	QDMA_START_TIMER(dma_busy_timer) ;
#endif
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to update dynamic threshold per ms.
 Input Args:	NULL. 
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_config_trigger_test(struct ECNT_QDMA_Data *qdma_data)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	init_timer(&trigger_timer) ;
	trigger_timer.data = trigger_expires ; /* 1 ms */
	trigger_timer.function = qdma_trigger_timer ;
	QDMA_START_TIMER(trigger_timer) ;
#else
	timer_setup(&trigger_timer, qdma_trigger_timer, 0);
	trigger_timer.expires = trigger_expires;
	QDMA_START_TIMER(trigger_timer) ;
#endif
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to dump the value of specific qdma register in several seconds.
 Input Args:	arg1: regOffset means the offset of specific qdma register. 
			arg2: pollingDuration means the seconds of polling time. 
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_dump_reg_polling(struct ECNT_QDMA_Data *qdma_data)
{
	uint regOffset = qdma_data->qdma_private.qdma_old_proc.regOffset ;
	uint pollingDuration = qdma_data->qdma_private.qdma_old_proc.pollingDuration ;
	
	if((regOffset & 0x3) != 0) {
		printk("register offset is %u, which should be an integral multiple of 4.", regOffset);
		return -EINVAL;
	}
	
	if(pollingDuration > QDMA_REG_POLLING_MAX) {
		printk("register polling duration force setting to %d seconds.", QDMA_REG_POLLING_MAX);
		pollingDuration = QDMA_REG_POLLING_MAX;
	}	
	reg_polling_round_cnt = pollingDuration*100 ;
	reg_polling_offset = regOffset ;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	init_timer(&reg_polling_timer) ;
	reg_polling_timer.data = reg_polling_expires ; /* 10 ms */
	reg_polling_timer.function = qdma_reg_polling_timer ;
	QDMA_START_TIMER(reg_polling_timer) ;
#else
	timer_setup(&reg_polling_timer, qdma_reg_polling_timer, 0);
	reg_polling_timer.expires = reg_polling_expires;
	QDMA_START_TIMER(reg_polling_timer) ;
#endif
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set receive rx ring1 packets only: rx1:rx0 from 5:2 to 5:0.
 Input Args:	arg1: forceEn means enable/disable force receive rx1 function.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_force_receive_rx_ring1(struct ECNT_QDMA_Data *qdma_data)
{
	uint forceEn = qdma_data->qdma_private.qdma_old_proc.forceEn ;
	
	isForceRX1 = ((forceEn==QDMA_DISABLE)?(0):(1));
	printk("isForceRX1: %d\n", isForceRX1) ;
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to enable or disable TX_DROP_EN.
 Input Args:	arg1: mode means enable or disable.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_tx_drop_en(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_Mode_t txDropMode = qdma_data->qdma_private.mode;

	if(txDropMode == QDMA_DISABLE) {
		qdmaDisableTxqCngstDrop(base) ;
	} else {
		qdmaEnableTxqCngstDrop(base) ;
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of TX_DROP_EN.
 Ret Value:		0: Disable, 1: Enable.
******************************************************************************/
int qdma_get_tx_drop_en(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(qdmaIsTxqCngstDropEnable(base)) {
		qdma_data->qdma_private.mode = QDMA_ENABLE ;
	} else {
		qdma_data->qdma_private.mode = QDMA_DISABLE ;
	}
	
	return qdma_data->qdma_private.mode ;
}

/******************************************************************************
 Descriptor:	It's used to set the value of tx ratemeter config. It includes
 			timeDivisor and timeSlice, while tx_rateMeter_En always enable.
 Input Args:	arg1: The pointer of the  Tx RateMeter Config struct.
                     tx ratemeter default enable.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_tx_ratemeter(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxRateMeter_T *txRateMeterPtr = qdma_data->qdma_private.txRateMeterPtr;

#if SUPPORT_METER_WINDOW_SIZE_TIME_SLIDE
	if((uint)qdmaIsTxRateMeterEnable(base) > 0){
		qdmaDisableTxRateMeter(base) ;
		qdmaSetTxRateMeterWindowSize(base, txRateMeterPtr->txRateMeterWindowSize) ;
		qdmaSetTxRateMeterTimeSlice(base, txRateMeterPtr->txRateMeterTimeSlice) ;
		qdmaEnableTxRateMeter(base)	;
	}else {
		qdmaSetTxRateMeterWindowSize(base, txRateMeterPtr->txRateMeterWindowSize) ;
		qdmaSetTxRateMeterTimeSlice(base, txRateMeterPtr->txRateMeterTimeSlice) ;
	}
#else
	if(txRateMeterPtr->txRateMeterTimeDivisor<0 || txRateMeterPtr->txRateMeterTimeDivisor>=QDMA_TX_RATE_METER_TIME_DIVISOR_ITEMS) {
		QDMA_ERR("Fault: TX Rate Meter Time Divisor value shoule between 0 and %d\n", QDMA_TX_RATE_METER_TIME_DIVISOR_ITEMS-1) ; 
		return -EINVAL ;
	}

	qdmaSetTxRateMeterTimeDivisor(base, txRateMeterPtr->txRateMeterTimeDivisor) ;
	qdmaSetTxRateMeterTimeSlice(base, txRateMeterPtr->txRateMeterTimeSlice) ;
#endif

	return 0 ;
}


/******************************************************************************
 Descriptor:	It's used to get the value of tx ratemeter config. It includes
 			timeDivisor and timeSlice, while tx_rateMeter_En always enable.
 Input Args:	arg1: The pointer of the  Tx RateMeter Config struct. It includes
 				- timeDivisor: the divisor of timeSlice, which will affect datarate.
 				- timeSlice: ratemeter calculate duration.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_get_tx_ratemeter(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxRateMeter_T *txRateMeterPtr = qdma_data->qdma_private.txRateMeterPtr;

#if SUPPORT_METER_WINDOW_SIZE_TIME_SLIDE
	txRateMeterPtr->txRateMeterWindowSize = qdmaGetTxRateMeterWindowSize(base) ;
#else
	txRateMeterPtr->txRateMeterTimeDivisor = qdmaGetTxRateMeterTimeDivisor(base) ;
#endif

	txRateMeterPtr->txRateMeterTimeSlice = qdmaGetTxRateMeterTimeSlice(base) ;
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get tx channel datarate.
 Input Args:	arg1: channel index
 Ret Value:		return tx datarate by channel, unit is bps
******************************************************************************/
int qdma_get_tx_channel_datarate(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxRateMeter_T txRateMeter;
	long dataRate = 0;
	int chnlIdx = qdma_data->qdma_private.channel;

	memset(&txRateMeter, 0, sizeof(QDMA_TxRateMeter_T));
	dataRate = qdmaGetTxPerChnlDataRate(base, chnlIdx);
	qdma_data->qdma_private.txRateMeterPtr = &txRateMeter;
	qdma_get_tx_ratemeter(qdma_data);
#if SUPPORT_METER_WINDOW_SIZE_TIME_SLIDE
	if(txRateMeter.txRateMeterWindowSize && txRateMeter.txRateMeterTimeSlice)
		dataRate = ((dataRate*CONFIG_PAYLOAD_SIZE[qdmaPayloadMode])<<3) * 1000000 
							/ (txRateMeter.txRateMeterWindowSize * txRateMeter.txRateMeterTimeSlice) ;
	else 
		dataRate = -1;
#else
	if(txRateMeter.txRateMeterTimeSlice)
		dataRate = ((dataRate<<3)<<4) * 1000000 * (1<<txRateMeter.txRateMeterTimeDivisor) / txRateMeter.txRateMeterTimeSlice ;
	else 
		dataRate = -1;
#endif
	
	return (int)dataRate ;
}

/******************************************************************************
 Descriptor:	It's used to read VIP info from fe. 
 Input Args:	None
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_read_vip_info(void)
{
	unsigned int en_val;
	unsigned int patn_val;
	int i;
	/*clear info*/
	qdma_vip_num = 0 ;
	memset((char *)qdma_vip_info, 0, sizeof(QDMA_VIP_INFO_T)*QDMA_VIP_PACKET_NUM);
	
	for( i = 0 ; i < QDMA_VIP_PACKET_NUM ; i++ )
	{
		en_val = read_reg_word(FE_VIP_EN(i));
		if ((en_val & 1) == 0)/*disable ,not care */
			continue;
		
		/*0: ETH_TYPE ; 1:PPPOE protocol ; 2:IP protocol; 3:TCP ports; 4:UDP ports*/
		if ( ((en_val >> VIP_EN_TYPE_OFFSET) & 0x07) < 3 )/*only care TCP ports & UDP ports*/
			continue;	

		qdma_vip_info[qdma_vip_num].ip_type = (((en_val >> VIP_EN_TYPE_OFFSET) & 0x07) == 3)? 6:17 ;/*tcp or udp*/
		/*swap the mode ,for qdma->sw and sw->qdma,are opposite*/
		qdma_vip_info[qdma_vip_num].sport_en = (en_val >> VIP_EN_DPEN_OFFSET ) & 0x01 ;
		qdma_vip_info[qdma_vip_num].dport_en = (en_val >> VIP_EN_SPEN_OFFSET ) & 0x01 ;
		qdma_vip_info[qdma_vip_num].swap_en = (en_val >> VIP_EN_SWEN_OFFSET ) & 0x01 ;

		patn_val = read_reg_word(FE_VIP_PATN(i));

		/*swap the sport and dport ,for qdma->sw and sw->qdma,are opposite*/
		qdma_vip_info[qdma_vip_num].sport = (patn_val >> 16) & 0xffff ;
		qdma_vip_info[qdma_vip_num].dport =  patn_val & 0xffff ;

		qdma_vip_num ++ ;
	}

#if 0
	/*show all info for debug*/
	printk("\n\t------------qdma vip packet info show------------\n");
	printk("\tsport \tdport \tsp_en \tdp_en \tsw_en \tip_type\n");
	for( i = 0 ; i < qdma_vip_num ; i++ )
	{
		printk("\t%d \t%d \t%d \t%d \t%d \t%d\n",qdma_vip_info[i].sport,qdma_vip_info[i].dport,
			qdma_vip_info[i].sport_en, qdma_vip_info[i].dport_en, qdma_vip_info[i].swap_en,qdma_vip_info[i].ip_type);
	}
#endif

	return 0 ;
}



/*--------------EN7580 TrTcm and Ratelimit New APIs start-------------------*/
static inline int generalGetRatelimitUnit(GENERAL_TrtcmModuleType_T trtcmModuleType, GENERAL_TrtcmPktMode_T PktMode, GENERAL_TrtcmTickSel_T tickSel)
{
	int unitBase=0, curTicksel=0 , curUnit=0 ;

	if(PktMode==TRTCM_PACKET_MODE)
		unitBase = 1000000;
	else
		unitBase = 8000;

	curTicksel = (tickSel== TRTCM_FAST_TICK) ? 
					generalGetTrtcmFastTick(trtcmModuleType) : generalGetTrtcmSlowTick(trtcmModuleType);
	if(curTicksel == 0)
		curUnit = 0 ;
	else
		curUnit = unitBase / curTicksel ;

	return curUnit;
}

////////////////////////////////////////////////////////////////////////
int qdma_general_set_trtcm_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0;
	GENERAL_TrtcmCfg_T *generalTrtcmCfgPtr = qdma_data->qdma_private.generalTrtcmCfgPtr;

	if((ret = generalSetTrtcmRateEnable( generalTrtcmCfgPtr->trtcmModule, generalTrtcmCfgPtr->trtcmEn )) < 0){
		QDMA_ERR("Fault:Set general trtcm cfg error.\n") ; 
		return ret;
	}

	if((ret = generalSetTrtcmMode( generalTrtcmCfgPtr->trtcmModule, generalTrtcmCfgPtr->trtcmMode )) < 0){
		QDMA_ERR("Fault:Set general trtcm cfg error.\n") ; 
		return ret;
	}

	if((ret = generalSetTrtcmFastTick( generalTrtcmCfgPtr->trtcmModule, generalTrtcmCfgPtr->trtcmFastTick )) < 0){
		QDMA_ERR("Fault:Set general trtcm cfg error.\n") ; 
		return ret;
	}

	if((ret = generalSetTrtcmSlowTickRatio( generalTrtcmCfgPtr->trtcmModule, generalTrtcmCfgPtr->trtcmSlowTickRatio )) < 0){
		QDMA_ERR("Fault:Set general trtcm cfg error.\n") ; 
		return ret;
	}
	
	return 0 ;
}

int qdma_general_get_trtcm_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	GENERAL_TrtcmCfg_T *generalTrtcmCfgPtr = qdma_data->qdma_private.generalTrtcmCfgPtr;

	generalTrtcmCfgPtr->trtcmEn = generalGetTrtcmRateEnable(generalTrtcmCfgPtr->trtcmModule) ;
	generalTrtcmCfgPtr->trtcmMode = generalGetTrtcmMode(generalTrtcmCfgPtr->trtcmModule) ;
	generalTrtcmCfgPtr->trtcmFastTick = generalGetTrtcmFastTick(generalTrtcmCfgPtr->trtcmModule) ;
	generalTrtcmCfgPtr->trtcmSlowTickRatio = generalGetTrtcmSlowTickRatio(generalTrtcmCfgPtr->trtcmModule) ;

	return 0 ;
}

int qdma_general_check_index_valid(GENERAL_TrtcmModuleType_T trtcmModule, GENERAL_TrtcmMode_T trtcmMode , ushort index)
{
    unchar meterIdx = GET_METER_IDX(index);
    unchar meterGrp = GET_METER_GRP(index);
    unchar maxMeterIdx[3] = {GENERAL_INGRESS_INDEX_MAX,GENERAL_INGRESS_INDEX_MAX_GRP1,GENERAL_INGRESS_INDEX_MAX_GRP2};
    
    /*Global ratelimit: no index check*/
    if(trtcmModule == GLB_RATECTL)
        return 0 ;
    
	/*check index range*/
	if( (trtcmModule == INGRESS_TRTCM) && (trtcmMode == TRTCM_RATELIMIT_MODE) ){
		if( meterIdx >= maxMeterIdx[meterGrp] ){ /*0~127*/
			QDMA_MSG(DBG_WARN,"Fault: index should between 0 and %d\n", (maxMeterIdx[meterGrp]-1) ) ; 
			return -EINVAL ;
		}
	}else if( (trtcmModule == INGRESS_TRTCM) && (trtcmMode == TRTCM_MODE) ){
		if( meterIdx >= (maxMeterIdx[meterGrp]>>1) ){/*0~63*/
			QDMA_MSG(DBG_WARN,"Fault: index should between 0 and %d\n", ((maxMeterIdx[meterGrp]>>1)-1) ) ; 
			return -EINVAL ;
		}
	}else if( trtcmModule == SLA_TRTCM ){
		if( meterIdx > GENERAL_SLA_INDEX_MAX ){/*SLA index 0~3 + queue 0~7 => max value is 0x1F*/
			QDMA_MSG(DBG_WARN,"Fault: index should between 0 and %d\n", GENERAL_SLA_INDEX_MAX ) ; 
			return -EINVAL ;
		}
	}else if( trtcmModule == EGRESS_QUEUE_RATELIMIT ){
		if( meterIdx > QUEUE_ALL_NUM ){/*0~255*/
			QDMA_MSG(DBG_WARN,"Fault: index should between 0 and %d\n", QUEUE_ALL_NUM ) ; 
			return -EINVAL ;
		}
	}else{/*Egress : the hardware fix TRTCM_MODE*/
		if( meterIdx > (CONFIG_QDMA_CHANNEL-1) ){/*LAN: 0~12 ; WAN: 0~31*/
			QDMA_MSG(DBG_WARN,"Fault: index should between 0 and %d\n", (CONFIG_QDMA_CHANNEL-1) ) ; 
			return -EINVAL ;
		}
	}

	return 0 ;
}

int qdma_general_set_ratelimit_mode_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0;
	GENERAL_TrtcmRateType_T rateType ;
	GENERAL_TrtcmRatelimitCfg_T *generalTrtcmRatelimitCfgPtr = qdma_data->qdma_private.generalTrtcmRatelimitCfgPtr;
	GENERAL_TrtcmMode_T trtcmMode = generalGetTrtcmMode(generalTrtcmRatelimitCfgPtr->trtcmModule) ;

    /*no trtcm mode for mtr_grp1&mtr_grp2*/
    if(GET_METER_GRP(generalTrtcmRatelimitCfgPtr->Index) > 0)
        trtcmMode = TRTCM_RATELIMIT_MODE;

	/*1. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmRatelimitCfgPtr->trtcmModule, trtcmMode, generalTrtcmRatelimitCfgPtr->Index)) < 0 )
		return ret ;

	/*2. set cfg*/
	if( trtcmMode == TRTCM_RATELIMIT_MODE ){/*ratelimit mode, set just one index*/
		if((ret = generalSetRatelimitMeterMode( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->MeterEn, generalTrtcmRatelimitCfgPtr->Index )) < 0){
			QDMA_ERR("Fault:Set ratelimit mode cfg error.\n") ; 
			return ret;
		}
		if((ret = generalSetRatelimitPktMode( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->PktMode, generalTrtcmRatelimitCfgPtr->Index )) < 0){
			QDMA_ERR("Fault:Set ratelimit mode cfg error.\n") ; 
			return ret;
		}
		if((ret = generalSetRatelimitTickSel( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->TickSel, generalTrtcmRatelimitCfgPtr->Index )) < 0){
			QDMA_ERR("Fault:Set ratelimit mode cfg error.\n") ; 
			return ret;
		}
	}else{/*trtcm mode, set CIR & PIR the cfg*/
		for( rateType = TRTCM_COMMIT_MODE ; rateType < TRTCM_RATE_TYPE_MAX ; rateType++ ){
			if((ret = generalSetTrtcmMeterMode( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->MeterEn, rateType , generalTrtcmRatelimitCfgPtr->Index )) < 0){
				QDMA_ERR("Fault:Set ratelimit mode cfg error.\n") ; 
				return ret;
			}
			if((ret = generalSetTrtcmPktMode( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->PktMode, rateType ,generalTrtcmRatelimitCfgPtr->Index )) < 0){
				QDMA_ERR("Fault:Set ratelimit mode cfg error.\n") ; 
				return ret;
			}
			if((ret = generalSetTrtcmTickSel( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->TickSel, rateType ,generalTrtcmRatelimitCfgPtr->Index )) < 0){
				QDMA_ERR("Fault:Set ratelimit mode cfg error.\n") ; 
				return ret;
			}
		}
	}
	
	return 0 ;
}

int qdma_general_get_ratelimit_mode_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0;
	GENERAL_TrtcmRatelimitCfg_T *generalTrtcmRatelimitCfgPtr = qdma_data->qdma_private.generalTrtcmRatelimitCfgPtr;
	GENERAL_TrtcmMode_T trtcmMode = generalGetTrtcmMode(generalTrtcmRatelimitCfgPtr->trtcmModule) ;

    /*no trtcm mode for mtr_grp1&mtr_grp2*/
    if(GET_METER_GRP(generalTrtcmRatelimitCfgPtr->Index) > 0)
        trtcmMode = TRTCM_RATELIMIT_MODE;

	/*1. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmRatelimitCfgPtr->trtcmModule, trtcmMode, generalTrtcmRatelimitCfgPtr->Index)) < 0 )
		return ret ;

	/*2. get cfg*/
	if( trtcmMode == TRTCM_RATELIMIT_MODE ){
		generalTrtcmRatelimitCfgPtr->MeterEn = generalGetRatelimitMeterMode( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->Index ) ;
		generalTrtcmRatelimitCfgPtr->PktMode = generalGetRatelimitPktMode( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->Index ) ;
		generalTrtcmRatelimitCfgPtr->TickSel = generalGetRatelimitTickSel( generalTrtcmRatelimitCfgPtr->trtcmModule, generalTrtcmRatelimitCfgPtr->Index ) ;
	}else{/*trtcm mode: just read PIR */
		generalTrtcmRatelimitCfgPtr->MeterEn = generalGetTrtcmMeterMode( generalTrtcmRatelimitCfgPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmRatelimitCfgPtr->Index ) ;
		generalTrtcmRatelimitCfgPtr->PktMode = generalGetTrtcmPktMode( generalTrtcmRatelimitCfgPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmRatelimitCfgPtr->Index ) ;
		generalTrtcmRatelimitCfgPtr->TickSel = generalGetTrtcmTickSel( generalTrtcmRatelimitCfgPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmRatelimitCfgPtr->Index ) ;
	}

	return 0 ;
}

int qdma_general_set_ratelimit_mode_value(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0;
	GENERAL_TrtcmRateType_T rateType ;
	GENERAL_TrtcmRatelimitSet_T *generalTrtcmRatelimitSetPtr = qdma_data->qdma_private.generalTrtcmRatelimitSetPtr;
	GENERAL_TrtcmMode_T trtcmMode = generalGetTrtcmMode(generalTrtcmRatelimitSetPtr->trtcmModule) ;
    int bucketSize=0;

    /*no trtcm mode for mtr_grp1&mtr_grp2*/
    if(GET_METER_GRP(generalTrtcmRatelimitSetPtr->Index) > 0)
        trtcmMode = TRTCM_RATELIMIT_MODE;

	/*1. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmRatelimitSetPtr->trtcmModule, trtcmMode, generalTrtcmRatelimitSetPtr->Index)) < 0 )
		return ret ;

	/*2. set value*/
	if( trtcmMode == TRTCM_RATELIMIT_MODE ){/*ratelimit mode, set just one index*/
		if((bucketSize = generalSetRatelimitTokenRate(generalTrtcmRatelimitSetPtr->trtcmModule, generalTrtcmRatelimitSetPtr->RateLimitValue, generalTrtcmRatelimitSetPtr->Index)) < 0) {
			QDMA_ERR("Fault: set ratelimit mode value error.\n");
			return bucketSize ;
		}
        /*set BucketSize*/
		if((ret = generalSetRatelimitBucketSize(generalTrtcmRatelimitSetPtr->trtcmModule, bucketSize, generalTrtcmRatelimitSetPtr->Index))< 0) {
			QDMA_ERR("Fault: set ratelimit mode value error.\n");
			return ret ;
		}
	}else{/*trtcm mode, set CIR & PIR the cfg*/
		for( rateType = TRTCM_COMMIT_MODE ; rateType < TRTCM_RATE_TYPE_MAX ; rateType++ ){
			if((bucketSize = generalSetTrtcmTokenRate(generalTrtcmRatelimitSetPtr->trtcmModule, generalTrtcmRatelimitSetPtr->RateLimitValue, rateType, generalTrtcmRatelimitSetPtr->Index)) < 0) {
				QDMA_ERR("Fault: set ratelimit mode value error.\n");
				return bucketSize ;
			}
			if((ret = generalSetTrtcmBucketSize(generalTrtcmRatelimitSetPtr->trtcmModule, bucketSize, rateType, generalTrtcmRatelimitSetPtr->Index))< 0) {
				QDMA_ERR("Fault: set ratelimit mode value error.\n");
				return ret ;
			}
		}
	}
	
	return 0 ;
}

int qdma_general_get_ratelimit_mode_value(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0;
	GENERAL_TrtcmRatelimitSet_T *generalTrtcmRatelimitSetPtr = qdma_data->qdma_private.generalTrtcmRatelimitSetPtr;
	GENERAL_TrtcmMode_T trtcmMode = generalGetTrtcmMode(generalTrtcmRatelimitSetPtr->trtcmModule) ;

    /*no trtcm mode for mtr_grp1&mtr_grp2*/
    if(GET_METER_GRP(generalTrtcmRatelimitSetPtr->Index) > 0)
        trtcmMode = TRTCM_RATELIMIT_MODE;

	/*1. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmRatelimitSetPtr->trtcmModule, trtcmMode, generalTrtcmRatelimitSetPtr->Index)) < 0 )
		return ret ;

	/*2. get value*/
	if( trtcmMode == TRTCM_RATELIMIT_MODE ){/*ratelimit mode, set just one index*/
		generalTrtcmRatelimitSetPtr->RateLimitValue = generalGetRatelimitTokenRate(generalTrtcmRatelimitSetPtr->trtcmModule, generalTrtcmRatelimitSetPtr->Index);
		generalTrtcmRatelimitSetPtr->BucketSize = generalGetRatelimitBucketSize(generalTrtcmRatelimitSetPtr->trtcmModule, generalTrtcmRatelimitSetPtr->Index);
	}else{/*trtcm mode: just read PIR */
		generalTrtcmRatelimitSetPtr->RateLimitValue = generalGetTrtcmTokenRate(generalTrtcmRatelimitSetPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmRatelimitSetPtr->Index);
		generalTrtcmRatelimitSetPtr->BucketSize = generalGetTrtcmBucketSize(generalTrtcmRatelimitSetPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmRatelimitSetPtr->Index);
	}

	return 0 ;
}

int qdma_general_set_ratelimit_bucket_size(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0;
	GENERAL_TrtcmRateType_T rateType ;
	GENERAL_TrtcmRatelimitSet_T *generalTrtcmRatelimitSetPtr = qdma_data->qdma_private.generalTrtcmRatelimitSetPtr;
	GENERAL_TrtcmMode_T trtcmMode = generalGetTrtcmMode(generalTrtcmRatelimitSetPtr->trtcmModule) ;
    int bucketSize = generalTrtcmRatelimitSetPtr->BucketSize;

    /*no trtcm mode for mtr_grp1&mtr_grp2*/
    if(GET_METER_GRP(generalTrtcmRatelimitSetPtr->Index) > 0)
        trtcmMode = TRTCM_RATELIMIT_MODE;

	/*1. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmRatelimitSetPtr->trtcmModule, trtcmMode, generalTrtcmRatelimitSetPtr->Index)) < 0 )
		return ret ;

	/*2. set Bucket Size*/
	if( trtcmMode == TRTCM_RATELIMIT_MODE ){/*ratelimit mode, set just one index*/
		if((ret = generalSetRatelimitBucketSize(generalTrtcmRatelimitSetPtr->trtcmModule, bucketSize, generalTrtcmRatelimitSetPtr->Index))< 0) {
			QDMA_ERR("Fault: set ratelimit mode value error.\n");
			return ret ;
		}
	}else{/*trtcm mode, set CIR & PIR Bucket Size*/
		for( rateType = TRTCM_COMMIT_MODE ; rateType < TRTCM_RATE_TYPE_MAX ; rateType++ ){
			if((ret = generalSetTrtcmBucketSize(generalTrtcmRatelimitSetPtr->trtcmModule, bucketSize, rateType, generalTrtcmRatelimitSetPtr->Index))< 0) {
				QDMA_ERR("Fault: set ratelimit mode value error.\n");
				return ret ;
			}
		}
	}
	if(generalTrtcmRatelimitSetPtr->trtcmModule == EGRESS_TRTCM && generalTrtcmRatelimitSetPtr->Index == 13 && generalTrtcmRatelimitSetPtr->RateLimitValue != 0)
		eth_force_dstq = 2;	
	return 0 ;
}

int qdma_general_set_trtcm_mode_cfg(struct ECNT_QDMA_Data *qdma_data)
{
    #if SUPPORT_MTR_ACNT_IN_QDMA
	int ret = 0;
	GENERAL_TrtcmCbsPbsCfg_T *generalTrtcmCbsPbsCfgPtr = qdma_data->qdma_private.generalTrtcmCbsPbsCfgPtr;

	/*1. check if the model works as trtcm mode*/
	if( TRTCM_RATELIMIT_MODE == generalGetTrtcmMode(generalTrtcmCbsPbsCfgPtr->trtcmModule) )
	{
		QDMA_ERR("Fault: TRTCM mode is not enable.\n");
		return -EINVAL ;
	}

	/*2. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_MODE, generalTrtcmCbsPbsCfgPtr->Index)) < 0 )
		return ret ;

	/*3. set CIR cfg*/
	if((ret = generalSetTrtcmMeterMode( generalTrtcmCbsPbsCfgPtr->trtcmModule, generalTrtcmCbsPbsCfgPtr->CbsMeterEn, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsCfgPtr->Index )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmPktMode( generalTrtcmCbsPbsCfgPtr->trtcmModule, generalTrtcmCbsPbsCfgPtr->CbsPktMode, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsCfgPtr->Index )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmTickSel( generalTrtcmCbsPbsCfgPtr->trtcmModule, generalTrtcmCbsPbsCfgPtr->CbsTickSel, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsCfgPtr->Index )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}

	/*4. set PIR cfg*/
	if((ret = generalSetTrtcmMeterMode( generalTrtcmCbsPbsCfgPtr->trtcmModule, generalTrtcmCbsPbsCfgPtr->PbsMeterEn, TRTCM_PEAK_MODE, generalTrtcmCbsPbsCfgPtr->Index )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmPktMode( generalTrtcmCbsPbsCfgPtr->trtcmModule, generalTrtcmCbsPbsCfgPtr->PbsPktMode, TRTCM_PEAK_MODE, generalTrtcmCbsPbsCfgPtr->Index )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmTickSel( generalTrtcmCbsPbsCfgPtr->trtcmModule, generalTrtcmCbsPbsCfgPtr->PbsTickSel, TRTCM_PEAK_MODE, generalTrtcmCbsPbsCfgPtr->Index )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
    #endif

	return 0 ;
}

int qdma_general_get_trtcm_mode_cfg(struct ECNT_QDMA_Data *qdma_data)
{
    #if SUPPORT_MTR_ACNT_IN_QDMA
	int ret = 0;
	GENERAL_TrtcmCbsPbsCfg_T *generalTrtcmCbsPbsCfgPtr = qdma_data->qdma_private.generalTrtcmCbsPbsCfgPtr;

	/*1. check if the model works as trtcm mode*/
	if( TRTCM_RATELIMIT_MODE == generalGetTrtcmMode(generalTrtcmCbsPbsCfgPtr->trtcmModule) )
	{
		QDMA_ERR("Fault: TRTCM mode is not enable.\n");
		return -EINVAL ;
	}

	/*2. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_MODE, generalTrtcmCbsPbsCfgPtr->Index)) < 0 )
		return ret ;

	/*3. get CIR cfg*/
	generalTrtcmCbsPbsCfgPtr->CbsMeterEn = generalGetTrtcmMeterMode(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsCfgPtr->Index);
	generalTrtcmCbsPbsCfgPtr->CbsPktMode = generalGetTrtcmPktMode(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsCfgPtr->Index);
	generalTrtcmCbsPbsCfgPtr->CbsTickSel = generalGetTrtcmTickSel(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsCfgPtr->Index);

	/*4. get PIR cfg*/
	generalTrtcmCbsPbsCfgPtr->PbsMeterEn = generalGetTrtcmMeterMode(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmCbsPbsCfgPtr->Index);
	generalTrtcmCbsPbsCfgPtr->PbsPktMode = generalGetTrtcmPktMode(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmCbsPbsCfgPtr->Index);
	generalTrtcmCbsPbsCfgPtr->PbsTickSel = generalGetTrtcmTickSel(generalTrtcmCbsPbsCfgPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmCbsPbsCfgPtr->Index);
    #endif
    
	return 0 ;

}

int qdma_general_set_trtcm_mode_value(struct ECNT_QDMA_Data *qdma_data)
{
    #if SUPPORT_MTR_ACNT_IN_QDMA
    int bucketSize = 0;
	int ret = 0;
	GENERAL_TrtcmCbsPbsSet_T *generalTrtcmCbsPbsSetPtr = qdma_data->qdma_private.generalTrtcmCbsPbsSetPtr;

	/*1. check if the model works as trtcm mode*/
	if( TRTCM_RATELIMIT_MODE == generalGetTrtcmMode(generalTrtcmCbsPbsSetPtr->trtcmModule) )
	{
		QDMA_ERR("Fault: TRTCM mode is not enable.\n");
		return -EINVAL ;
	}

	/*2. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmCbsPbsSetPtr->trtcmModule, TRTCM_MODE, generalTrtcmCbsPbsSetPtr->Index)) < 0 )
		return ret ;

	/*3. set CIR value*/
	if((bucketSize = generalSetTrtcmTokenRate(generalTrtcmCbsPbsSetPtr->trtcmModule, generalTrtcmCbsPbsSetPtr->CbsParamValue, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsSetPtr->Index)) < 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return bucketSize ;
	}
	if((ret = generalSetTrtcmBucketSize(generalTrtcmCbsPbsSetPtr->trtcmModule, bucketSize, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsSetPtr->Index))< 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}

	/*4. set PIR value*/
	if((bucketSize = generalSetTrtcmTokenRate(generalTrtcmCbsPbsSetPtr->trtcmModule, generalTrtcmCbsPbsSetPtr->PbsParamValue, TRTCM_PEAK_MODE, generalTrtcmCbsPbsSetPtr->Index)) < 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return bucketSize ;
	}
	if((ret = generalSetTrtcmBucketSize(generalTrtcmCbsPbsSetPtr->trtcmModule, bucketSize, TRTCM_PEAK_MODE, generalTrtcmCbsPbsSetPtr->Index))< 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}
    #endif

	return 0 ;

}

int qdma_general_get_trtcm_mode_value(struct ECNT_QDMA_Data *qdma_data)
{
    #if SUPPORT_MTR_ACNT_IN_QDMA
	int ret = 0;
	GENERAL_TrtcmCbsPbsSet_T *generalTrtcmCbsPbsSetPtr = qdma_data->qdma_private.generalTrtcmCbsPbsSetPtr;

	/*1. check if the model works as trtcm mode*/
	if( TRTCM_RATELIMIT_MODE == generalGetTrtcmMode(generalTrtcmCbsPbsSetPtr->trtcmModule) )
	{
		QDMA_ERR("Fault: TRTCM mode is not enable.\n");
		return -EINVAL ;
	}

	/*2. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmCbsPbsSetPtr->trtcmModule, TRTCM_MODE, generalTrtcmCbsPbsSetPtr->Index)) < 0 )
		return ret ;

	/*3. get CIR value*/
	generalTrtcmCbsPbsSetPtr->CbsParamValue = generalGetTrtcmTokenRate(generalTrtcmCbsPbsSetPtr->trtcmModule, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsSetPtr->Index);
	generalTrtcmCbsPbsSetPtr->CbsBucketSize = generalGetTrtcmBucketSize(generalTrtcmCbsPbsSetPtr->trtcmModule, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsSetPtr->Index);

	/*4. get PIR value*/
	generalTrtcmCbsPbsSetPtr->PbsParamValue = generalGetTrtcmTokenRate(generalTrtcmCbsPbsSetPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmCbsPbsSetPtr->Index);
	generalTrtcmCbsPbsSetPtr->PbsBucketSize = generalGetTrtcmBucketSize(generalTrtcmCbsPbsSetPtr->trtcmModule, TRTCM_PEAK_MODE, generalTrtcmCbsPbsSetPtr->Index);
    #endif
    
	return 0 ;

}

int qdma_general_set_trtcm_bucket_size(struct ECNT_QDMA_Data *qdma_data)
{
    #if SUPPORT_MTR_ACNT_IN_QDMA
	int ret = 0;
	GENERAL_TrtcmCbsPbsSet_T *generalTrtcmCbsPbsSetPtr = qdma_data->qdma_private.generalTrtcmCbsPbsSetPtr;
	int cbsBucketSize = generalTrtcmCbsPbsSetPtr->CbsBucketSize;
	int pbsBucketSize = generalTrtcmCbsPbsSetPtr->PbsBucketSize;

	/*1. check if the model works as trtcm mode*/
	if( TRTCM_RATELIMIT_MODE == generalGetTrtcmMode(generalTrtcmCbsPbsSetPtr->trtcmModule) )
	{
		QDMA_ERR("Fault: TRTCM mode is not enable.\n");
		return -EINVAL ;
	}

	/*2. check index range*/
	if( (ret = qdma_general_check_index_valid(generalTrtcmCbsPbsSetPtr->trtcmModule, TRTCM_MODE, generalTrtcmCbsPbsSetPtr->Index)) < 0 )
		return ret ;

	/*3. set CIR bucket size*/
	if((ret = generalSetTrtcmBucketSize(generalTrtcmCbsPbsSetPtr->trtcmModule, cbsBucketSize, TRTCM_COMMIT_MODE, generalTrtcmCbsPbsSetPtr->Index))< 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}

	/*4. set PIR bucket size*/
	if((ret = generalSetTrtcmBucketSize(generalTrtcmCbsPbsSetPtr->trtcmModule, pbsBucketSize, TRTCM_PEAK_MODE, generalTrtcmCbsPbsSetPtr->Index))< 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}
    #endif

	return 0 ;
}

int qdma_set_tx_wred_mode(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_TxWredCfg_T *txWredCfgPtr = qdma_data->qdma_private.txWredCfgPtr;
    
    if(txWredCfgPtr->txWredEn == QDMA_ENABLE ){
        qdmaEnableTxWred(base);
    }else if(txWredCfgPtr->txWredEn == QDMA_DISABLE ){
        qdmaDisableTxWred(base);
    }
    #endif

    return 0;
}

int qdma_get_tx_wred_mode(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_TxWredCfg_T *txWredCfgPtr = qdma_data->qdma_private.txWredCfgPtr;

    if(qdmaIsTxWredEnable(base)){
        txWredCfgPtr->txWredEn = QDMA_ENABLE;
    }else{
        txWredCfgPtr->txWredEn = QDMA_DISABLE;
    }
    #endif
    
    return 0;
}

int qdma_set_tx_wred_threshold(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxWredCfg_T *txWredCfgPtr = qdma_data->qdma_private.txWredCfgPtr;
    uint i=0, regVal=0;
    unchar wredThrShift=0, wredThrCfg=0;

    /*shift-0: 0/8, 1/8, 2/8, 3/8, 4/8, 5/8, 6/8, 7/8*/
    /*shift-1: 8/8, 10/8, 12/8, 14/8 ==> 4/4, 5/4, 6/4, 7/4*/
    /*shift-2: 16/8, 20/8, 24/8, 28/8 ==> 4/2, 5/2, 6/2, 7/2*/
    /*shift-3: 32/8, 40/8, 48/8, 56/8 ==> 4/1, 5/1, 6/1, 7/1*/
    for(i=0;i<TX_WRED_THR_NUM;i++){
        if(txWredCfgPtr->txWredThrValue[i]<=56){
            if(txWredCfgPtr->txWredThrValue[i]>=32){
                wredThrShift = 3;
                wredThrCfg = txWredCfgPtr->txWredThrValue[i]/8;
            }else if((txWredCfgPtr->txWredThrValue[i]>=16) && (txWredCfgPtr->txWredThrValue[i]<32)){
                wredThrShift = 2;
                wredThrCfg = txWredCfgPtr->txWredThrValue[i]/4;
            }else if((txWredCfgPtr->txWredThrValue[i]>=8) && (txWredCfgPtr->txWredThrValue[i]<16)){
                wredThrShift=1;
                wredThrCfg = txWredCfgPtr->txWredThrValue[i]/2;
            }else{
                wredThrShift=0;
                wredThrCfg = txWredCfgPtr->txWredThrValue[i]; 
            }
            regVal |= ((wredThrShift<<WRED_THR_SHIFT_SHIFT(i))|(wredThrCfg<<WRED_THR_CFG_SHIFT(i)));
        }else{
            QDMA_ERR("Fault: Tx Wred threshold range: 0 ~ 56 !\n") ; 
            return 0;
        }
    }
    if(txWredCfgPtr->txWredPacketType>=0 && txWredCfgPtr->txWredPacketType<PACKET_TYPE_ITEMS){
        if(txWredCfgPtr->txWredPacketType == NORMAL_PACKET){
            qdmaSetWredNormThreshold(base, regVal);
        }else{ /*DEI_PACKET*/
            qdmaSetWredDeiThreshold(base, regVal);
        }
    }
    #endif

    return 0;
}

int qdma_get_tx_wred_threshold(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxWredCfg_T *txWredCfgPtr = qdma_data->qdma_private.txWredCfgPtr;
    uint i=0, regVal=0;
    unchar wredThrShift=0, wredThrCfg=0;
    unchar txWredThr[TX_WRED_THR_NUM]={0};
    
    if(txWredCfgPtr->txWredPacketType>=0 && txWredCfgPtr->txWredPacketType<PACKET_TYPE_ITEMS){
        if(txWredCfgPtr->txWredPacketType == NORMAL_PACKET){
            regVal = qdmaGetWredNormThreshold(base);
        }else{ /*DEI_PACKET*/
            regVal = qdmaGetWredDeiThreshold(base);
        }
    }
    for(i=0;i<TX_WRED_THR_NUM;i++){
        txWredCfgPtr->txWredThrValue[i] =((regVal&WRED_THR_CFG_MASK(i))>>WRED_THR_CFG_SHIFT(i))<<((regVal& WRED_THR_SHIFT_MASK(i))>>WRED_THR_SHIFT_SHIFT(i)) ;
    }
    #endif

    return 0;
}

int qdma_set_tx_wred_probability(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxWredCfg_T *txWredCfgPtr = qdma_data->qdma_private.txWredCfgPtr;
    uint i=0, regVal=0;
    unchar txWredThrP[TX_WRED_PROBABILITY_NUM]={0};
    
    for(i=0;i<TX_WRED_PROBABILITY_NUM;i++){
        if(txWredCfgPtr->txWredThrProbability[i]<=100)
            txWredThrP[i]= txWredCfgPtr->txWredThrProbability[i]*255/100;
        regVal |=(txWredThrP[i]<< WRED_PROBABILITY_SHIFT(i)) ;
    }
    if(txWredCfgPtr->txWredPacketType>=0 && txWredCfgPtr->txWredPacketType<PACKET_TYPE_ITEMS){
        if(txWredCfgPtr->txWredPacketType == NORMAL_PACKET){
            qdmaSetWredNormProbability(base, regVal);
        }else{ /*DEI_PACKET*/
            qdmaSetWredDeiProbability(base, regVal);
        }
    }
    #endif
    
    return 0;
}

int qdma_get_tx_wred_probability(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxWredCfg_T *txWredCfgPtr = qdma_data->qdma_private.txWredCfgPtr;
    uint i=0, regVal=0;
    unchar txWredThrP[TX_WRED_PROBABILITY_NUM]={0};

   if(txWredCfgPtr->txWredPacketType>=0 && txWredCfgPtr->txWredPacketType<PACKET_TYPE_ITEMS){
        if(txWredCfgPtr->txWredPacketType == NORMAL_PACKET){
           regVal = qdmaGetWredNormProbability(base);
        }else{ /*DEI_PACKET*/
           regVal= qdmaGetWredDeiProbability(base);
        }
    }

    for(i=0;i<TX_WRED_PROBABILITY_NUM;i++){
        txWredThrP[i] = (regVal& WRED_PROBABILITY_MASK(i)) >> WRED_PROBABILITY_SHIFT(i);
        txWredCfgPtr->txWredThrProbability[i] = txWredThrP[i]*100/255;
    }
    #endif

    return 0;
}

int qdma_get_tx_wred_cfg(struct ECNT_QDMA_Data *qdma_data){

    #if SUPPORT_TXQ_WRED
    qdma_get_tx_wred_probability(qdma_data);
    qdma_get_tx_wred_threshold(qdma_data);
    #endif
    
    return 0;
}

int qdma_set_cpu_rx_red_probability(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_RxRedCfg_T *rxRedCfgPtr = qdma_data->qdma_private.rxRedCfgPtr;
    uint redProbability=0;
    if(rxRedCfgPtr->rxRedThrProbability<=100){
        redProbability = rxRedCfgPtr->rxRedThrProbability * 255 /100;
        if(rxRedCfgPtr->rxRedPacketType>=0 && rxRedCfgPtr->rxRedPacketType<PACKET_TYPE_ITEMS){
            if(rxRedCfgPtr->rxRedPacketType == NORMAL_PACKET){              
                qdmaSetCpuRxRedNormProbability(base, redProbability);
            }else{
                qdmaSetCpuRxRedDeiProbability(base, redProbability);
            }
        }
    }
    #endif
    
    return 0;
}

int qdma_get_cpu_rx_red_probability(struct ECNT_QDMA_Data *qdma_data){
    #if SUPPORT_TXQ_WRED
    uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_RxRedCfg_T *rxRedCfgPtr = qdma_data->qdma_private.rxRedCfgPtr;
    uint redProbability=0;
    
    if(rxRedCfgPtr->rxRedPacketType>=0 && rxRedCfgPtr->rxRedPacketType<PACKET_TYPE_ITEMS){
        if(rxRedCfgPtr->rxRedPacketType == NORMAL_PACKET){              
            redProbability = qdmaGetCpuRxRedNormProbability(base);
        }else{
            redProbability = qdmaGetCpuRxRedDeiProbability(base);
        }

        rxRedCfgPtr->rxRedThrProbability = redProbability * 100 /255;
    }
    #endif
    
    return 0;
}

/*--------------EN7580 TrTcm and Ratelimit New APIs end-------------------*/

#if SUPPORT_MTR_ACNT_IN_QDMA
/*--------------old ratelimit APIs set , simplified set for EN7580-------------------*/
int qdma_enable_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int ret = 0 ;

	if((ret = generalSetTrtcmRateEnable( EGRESS_TRTCM, TRTCM_ENABLE )) < 0){
		QDMA_ERR("Fault:Set general trtcm cfg error.\n") ; 
		return ret;
	}

	return 0 ;
}

int qdma_set_rx_ratelimit_en(struct ECNT_QDMA_Data *qdma_data)
{
	int ringIdx=0;
	QDMA_Mode_t rxRateLimitEn = qdma_data->qdma_private.mode;

	GENERAL_TrtcmMode_T trtcmMode = generalGetTrtcmMode(INGRESS_TRTCM) ;

	for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ ) 
	{
		if( trtcmMode == TRTCM_RATELIMIT_MODE )
		{
			generalSetRatelimitMeterMode(INGRESS_TRTCM,rxRateLimitEn,ringIdx);
		}
		else
		{
			generalSetTrtcmMeterMode(INGRESS_TRTCM,rxRateLimitEn,TRTCM_COMMIT_MODE,ringIdx);
			generalSetTrtcmMeterMode(INGRESS_TRTCM,rxRateLimitEn,TRTCM_PEAK_MODE,ringIdx);
		}		
	}

	return 0;
}

/*no need use*/
int qdma_set_tx_ratelimit_cfg(struct ECNT_QDMA_Data *qdma_data){
	return 0 ;
}
int qdma_get_tx_ratelimit_cfg(struct ECNT_QDMA_Data *qdma_data){
	return 0 ;
}
/************/

int qdma_set_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0 ;
	QDMA_TxRateLimitSet_T *txRateLimitPtr = qdma_data->qdma_private.txRateLimitSetPtr;
	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmRatelimitCfg_T txRatelimitCfg;
	GENERAL_TrtcmRatelimitSet_T txRatelimitSet;

	qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &txRatelimitCfg;
	txRatelimitCfg.trtcmModule = EGRESS_TRTCM;
	txRatelimitCfg.Index = txRateLimitPtr->chnlIdx;
	qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);
    
	txRatelimitCfg.MeterEn = txRateLimitPtr->chnlRateLimitEn;
	if( (ret=qdma_general_set_ratelimit_mode_cfg(&qdma_data_tmp)) < 0 )
		return ret;

	qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &txRatelimitSet;
	txRatelimitSet.trtcmModule = EGRESS_TRTCM;
	txRatelimitSet.Index = txRateLimitPtr->chnlIdx ;
	txRatelimitSet.RateLimitValue = txRateLimitPtr->rateLimitValue;	
	if( (ret=qdma_general_set_ratelimit_mode_value(&qdma_data_tmp)) < 0 )
		return ret;

	return 0 ;
}

int qdma_get_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	QDMA_TxRateLimitGet_T *txRateLimitPtr = qdma_data->qdma_private.txRateLimitGetPtr;

	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmRatelimitCfg_T txRatelimitCfg;
	GENERAL_TrtcmRatelimitSet_T txRatelimitSet;

	qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &txRatelimitCfg;
	txRatelimitCfg.trtcmModule = EGRESS_TRTCM;
	txRatelimitCfg.Index = txRateLimitPtr->chnlIdx;
	qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);

	qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &txRatelimitSet;
	txRatelimitSet.trtcmModule = EGRESS_TRTCM;
	txRatelimitSet.Index = txRateLimitPtr->chnlIdx ;
	qdma_general_get_ratelimit_mode_value(&qdma_data_tmp);

	txRateLimitPtr->chnlRateLimitEn = txRatelimitCfg.MeterEn;
	txRateLimitPtr->rateLimitValue = txRatelimitSet.RateLimitValue;
	txRateLimitPtr->pbsParamValue = txRatelimitSet.BucketSize;
	
	return 0 ;
}

int qdma_set_rx_ratelimit_pkt_mode(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0 ;
	uint ring_index;
	QDMA_RxPktMode_t pktMode = qdma_data->qdma_private.pktMode;

	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmRatelimitCfg_T rxRatelimitCfg;
	qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRatelimitCfg;

	for( ring_index = 0 ; ring_index < RX_RING_NUM ; ring_index++ )
	{
		rxRatelimitCfg.trtcmModule = INGRESS_TRTCM;
		rxRatelimitCfg.Index = ring_index;
		qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);
		
		if(pktMode == QDMA_RX_RATE_LIMIT_BY_PACKET)
			rxRatelimitCfg.PktMode = TRTCM_PACKET_MODE;
		else
			rxRatelimitCfg.PktMode = TRTCM_BYTE_MODE;
		if( (ret=qdma_general_set_ratelimit_mode_cfg(&qdma_data_tmp)) < 0 )
			return ret;
	}

	return 0 ;
}

int qdma_get_rx_ratelimit_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	QDMA_RxRateLimitCfg_T *rxRateLimitCfgPtr = qdma_data->qdma_private.rxRateLimitCfgPtr;

	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmRatelimitCfg_T rxRatelimitCfg;
	
	qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRatelimitCfg;
	rxRatelimitCfg.trtcmModule = INGRESS_TRTCM;
	rxRatelimitCfg.Index = RING_IDX_0;	/*only get rx ring-0 info , if using the old APIs, rx rings should be set the same config*/
	qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);

	rxRateLimitCfgPtr->rxRateLimitEn = rxRatelimitCfg.MeterEn;

	if(rxRatelimitCfg.PktMode==TRTCM_PACKET_MODE)
		rxRateLimitCfgPtr->rxRateLimitPktMode = QDMA_RX_RATE_LIMIT_BY_PACKET ;
	else
		rxRateLimitCfgPtr->rxRateLimitPktMode = QDMA_RX_RATE_LIMIT_BY_BYTE ;

	rxRateLimitCfgPtr->rxRateLimitUnit = generalGetRatelimitUnit(INGRESS_TRTCM,rxRatelimitCfg.PktMode,rxRatelimitCfg.TickSel);

	rxRateLimitCfgPtr->rxRateLimitBucketScale = QDMA_TRTCM_SCALE_8KBYTE;
	
	return 0 ;
}

int qdma_set_rx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
    int ret=0 ;
	QDMA_RxRateLimitSet_T *rxRateLimitPtr = qdma_data->qdma_private.rxRateLimitSetPtr;
	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmRatelimitSet_T rxRatelimitSet;
	GENERAL_TrtcmRatelimitCfg_T rxRatelimitCfg;

	qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRatelimitCfg;
	rxRatelimitCfg.trtcmModule = INGRESS_TRTCM;
	rxRatelimitCfg.Index = rxRateLimitPtr->ringIdx;
	qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);

    /*if the value is small, should use slowtick, make sure has tocken per ticksel*/
    if( rxRatelimitCfg.PktMode == TRTCM_BYTE_MODE )
    {
        if( rxRateLimitPtr->rateLimitValue < 100000 )/*100Mbps*/
            rxRatelimitCfg.TickSel = TRTCM_SLOW_TICK;
        else
            rxRatelimitCfg.TickSel = TRTCM_FAST_TICK;
    }
    else
    {
        if( rxRateLimitPtr->rateLimitValue < 80000 )/*80000pps*/
            rxRatelimitCfg.TickSel = TRTCM_SLOW_TICK;
        else
            rxRatelimitCfg.TickSel = TRTCM_FAST_TICK;
    }
    
	if( (ret=qdma_general_set_ratelimit_mode_cfg(&qdma_data_tmp)) < 0 )
		return ret;

	qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &rxRatelimitSet;
	rxRatelimitSet.trtcmModule = INGRESS_TRTCM;
	rxRatelimitSet.Index = rxRateLimitPtr->ringIdx;
	rxRatelimitSet.RateLimitValue = rxRateLimitPtr->rateLimitValue;
	qdma_general_set_ratelimit_mode_value(&qdma_data_tmp);

	return 0 ;
}

int qdma_get_rx_ratelimit(struct ECNT_QDMA_Data *qdma_data){
	QDMA_RxRateLimitGet_T *rxRateLimitPtr = qdma_data->qdma_private.rxRateLimitGetPtr;
	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmRatelimitSet_T rxRatelimitSet;

	qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &rxRatelimitSet;
	rxRatelimitSet.trtcmModule = INGRESS_TRTCM;
	rxRatelimitSet.Index = rxRateLimitPtr->ringIdx;
	qdma_general_get_ratelimit_mode_value(&qdma_data_tmp);

	rxRateLimitPtr->rateLimitValue = rxRatelimitSet.RateLimitValue;
	rxRateLimitPtr->pbsParamValue = rxRatelimitSet.BucketSize;
	return 0 ;
}
/*--------------old ratelimit APIs set NULL end-------------------*/

#else

/******************************************************************************
 Descriptor:	It's used to enable tx global ratelimit function. 
 Input Args:	No input arguments
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_enable_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	
	qdmaEnableTxRateLimit(base) ;
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to set the value of rx rate limit config. It includes
 			rx_rateLimit_En, bucketScale and tickSel.
 Input Args:	arg1: The pointer of the RX Rate Limit config struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_rx_ratelimit_en(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel=0, ringIdx=0;
	uint rxRateLimitPir[RX_RING_NUM], rxRateLimitPbs[RX_RING_NUM] ;
	QDMA_Mode_t rxRateLimitEn = qdma_data->qdma_private.mode;

	if(rxRateLimitEn == QDMA_DISABLE) {
		qdmaDisableRxRateLimit(base) ;
	} else {
		qdmaEnableRxRateLimit(base) ;
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set the value of tx rate limit config. It includes bucketScale and ticksel.
 Input Args:	arg1: The pointer of the  Tx Rate Limit Config struct.It includes
 			- txRateLimitUnit: unit[0](new reg) is 1kbps,we should not modify this unit,unit[1] can be modified.
 			scope is (1kbps~65kbps). 0 means not change.
 			- txRateLimitBucketScale: ratemeter calculate duration.
 			- txRateLimitEn: default enable, ratelimit enable or not decide by chnlRateLimitEn.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_tx_ratelimit_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int maxLimitRate=0, curUnit1=0;
	int curTicksel1=0, newLimitRate=0;
	int maxLimitRateChnl=CONFIG_QDMA_CHANNEL;
	QDMA_TxRateLimitCfg_T *txRateLimitCfgPtr = qdma_data->qdma_private.txRateLimitCfgPtr;
	
	if(txRateLimitCfgPtr->txRateLimitBucketScale<0 || txRateLimitCfgPtr->txRateLimitBucketScale>=QDMA_TRTCM_SCALE_MAX_ITEMS) {
		QDMA_ERR("Fault: TX Rate Limit Bucket Scale should between 0 and %d\n", QDMA_TRTCM_SCALE_MAX_ITEMS-1) ; 
		return -EINVAL ;
	}
	if(txRateLimitCfgPtr->txRateLimitUnit[1]<1 || txRateLimitCfgPtr->txRateLimitUnit[1]>65 ) {
		QDMA_ERR("Fault: TX Rate Limit unit should between 1kbps and 65kbps\n") ; 
		return -EINVAL ;
	}
	
	/* get current rate limit unit1 */
	curTicksel1 = qdmaGetTxRateLimitTicksel(base,1);
	if(curTicksel1 == 0) 
		curUnit1 = 0 ;
	else
		curUnit1 = 8000/curTicksel1 ;
	
	/* if setting ratelimit unit is not equal with current unit, should update all channel's rate limit value */
	if(curUnit1 != txRateLimitCfgPtr->txRateLimitUnit[1]) {
		/*get max rate channel */
		maxLimitRateChnl = qdmaGetLimitRateMaxChnl(1,CONFIG_QDMA_CHANNEL);
		if(maxLimitRateChnl != CONFIG_QDMA_CHANNEL)
			maxLimitRate = qdmaGetTxRateLimitConfig(maxLimitRateChnl, QDMA_Tx_Rate_Limit_PIR);
		if(maxLimitRate < 0) {
				QDMA_ERR("Fault: maxLimitRate is not correct in Tick1 mode\n") ; 
			return -EINVAL ;
		}
			
		/* if setting unit is too small, so that pir would larger than 0xFFFF, then exit*/
		newLimitRate = maxLimitRate*curUnit1/txRateLimitCfgPtr->txRateLimitUnit[1];
		if(newLimitRate > 0xFFFF) {
				QDMA_ERR("Fault: new pir would be 0x%X, should not larger than 0xFFFF in Tick1 mode\n", newLimitRate) ; 
			return -EINVAL ;
		}
	
		/* update all channel's rate limit value */
		qdmaUpdateAllTxRateLimitValue(curUnit1, txRateLimitCfgPtr->txRateLimitUnit[1],1) ;
	}
	
	if(txRateLimitCfgPtr->txRateLimitUnit[1]) {
		qdmaSetTxRateLimitTicksel(base, 1, 8000/txRateLimitCfgPtr->txRateLimitUnit[1]) ;
	}
	
	qdmaSetTxRateLimitBucketScale(base, txRateLimitCfgPtr->txRateLimitBucketScale) ;
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get the value of tx rate limit config. It includes
 			tx_rateLimit_En, bucketScale and ticksel.
 Input Args:	arg1: The pointer of the  Tx Rate Limit Config struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_tx_ratelimit_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel[PER_CHNL_TICKSEL_NUM]={0};
	int i = 0;
	QDMA_TxRateLimitCfg_T *txRateLimitCfgPtr = qdma_data->qdma_private.txRateLimitCfgPtr;

	for(i = 0; i < PER_CHNL_TICKSEL_NUM; i++){
		curTicksel[i] = qdmaGetTxRateLimitTicksel(base,i);
		if(curTicksel[i] == 0)
			txRateLimitCfgPtr->txRateLimitUnit[i] = 0 ;
		else
			txRateLimitCfgPtr->txRateLimitUnit[i] = 8000/curTicksel[i] ;
	}
	
	txRateLimitCfgPtr->txRateLimitBucketScale = qdmaGetTxRateLimitBucketScale(base) ;
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set the value of tx rate limit parameter. It includes chnlRateLimitEn and PIR.
 Input Args:	arg1: The pointer of the TX RateLimit Parameter struct.It includes
 			- chnlRateLimitEn: means current channel ratelimit enable or disable.
 			- PIR: unit is kbps, scope is [1kbps~1Gbps]
 			ticker0 as 1Kbps,we should not change ticker0 unit, only ticker1 can be changed.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0 , enMode=0, revertToTicker0=0;
	int curUnit=0, newUnit=0, switchLeastUnit=0, unit1=0;
	int curTicksel=0, curTicksel_idx=0;
	int maxPirChnl=0, tmpMaxPirValue=0, tmpMaxTxLimitRate=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxRateLimitSet_T *txRateLimitPtr = qdma_data->qdma_private.txRateLimitSetPtr;
	int port = 0;
	int switch_port =0;

	if(txRateLimitPtr->chnlIdx >= CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}	

	if(TCSUPPORT_FWC_ENV_VAL)
	{
#ifdef QDMA_LAN
		if(0 != txRateLimitPtr->chnlIdx)
		{
			port = txRateLimitPtr->chnlIdx - 1;   
			if(MT7530LanPortMap2Switch_hook)
			{
				switch_port = MT7530LanPortMap2Switch_hook(port);
			}
			txRateLimitPtr->chnlIdx = switch_port;
		}  
#endif
	} 

	if(txRateLimitPtr->chnlRateLimitEn == QDMA_DISABLE) {
		qdmaDisableChnlRateLimit(base, txRateLimitPtr->chnlIdx) ;
		if(qdmaIsTxRateLimitChnlTickSel1(base,txRateLimitPtr->chnlIdx)){
			curTicksel_idx = 1;
			txRateLimitPtr->rateLimitValue = 0;
			goto t1;
		}
		return 0;
	} else {
		enMode = qdmaIsChnlRateLimitEnable(base, txRateLimitPtr->chnlIdx) ;		
		qdmaEnableChnlRateLimit(base, txRateLimitPtr->chnlIdx) ;
		if(!enMode) {	/* change enMode from disable to enable */
			if(!qdmaIsTxRateLimitChnlTickSel1(base,txRateLimitPtr->chnlIdx)){
				qdmaSetTxRateLimitPerChnlTickSelIdx(base, txRateLimitPtr->chnlIdx, 1);
			}
			curTicksel = qdmaGetTxRateLimitTicksel(base,1) ;
			qdmaSetTxRateLimitTicksel(base, 1, 1) ;
			ret = qdmaSetTxRateLimitConfig(txRateLimitPtr->chnlIdx, QDMA_Tx_Rate_Limit_PIR, 0xFFFF) ;
			mdelay(100) ;
			qdmaSetTxRateLimitTicksel(base, 1, curTicksel) ;
		}		
	}
	
	/*ticker0 default as 1kbps, you'd better not change ticker0*/
	if(txRateLimitPtr->rateLimitValue <= 1000){	 /*1kbps ~1000kbps */
 		curTicksel_idx = 0;
		if(qdmaIsTxRateLimitChnlTickSel1(base,txRateLimitPtr->chnlIdx)){
			qdmaSetTxRateLimitPerChnlTickSelIdx(base, txRateLimitPtr->chnlIdx, 0);
			revertToTicker0 = 1;
		}
	}else{//1Mbps ~4Gbps
		newUnit = (txRateLimitPtr->rateLimitValue - 1)/0xFFFF + 1;	/*round up to nearest int num*/
		if(newUnit <= TX_RATELIMIT_TICKER1_ACCURACY)
			newUnit = TX_RATELIMIT_TICKER1_ACCURACY;
		curTicksel_idx = 1;
		if(!qdmaIsTxRateLimitChnlTickSel1(base,txRateLimitPtr->chnlIdx))
			qdmaSetTxRateLimitPerChnlTickSelIdx(base, txRateLimitPtr->chnlIdx, 1);
	}
	
t1:
	/* get current unit */
	curTicksel = qdmaGetTxRateLimitTicksel(base, curTicksel_idx) ;
	if(curTicksel == 0)
		curUnit = 0 ;
	else
		curUnit = 8000 / curTicksel ;  /**8bits *1000/(curTicksel*10e-6 s) kbps **/
	if(1 == revertToTicker0 || 1 == curTicksel_idx){ /*we only update channel bind to ticker1 */
		if(curTicksel_idx == 1)
			unit1 = curUnit;
		else /*revert from ticker1 to Ticker0*/
			unit1 = 8000 / qdmaGetTxRateLimitTicksel(base, 1) ;
		maxPirChnl = qdmaGetLimitRateMaxChnl(1, txRateLimitPtr->chnlIdx);
		if(maxPirChnl != CONFIG_QDMA_CHANNEL){
			tmpMaxTxLimitRate = qdmaGetTxRateLimitConfig(maxPirChnl, QDMA_Tx_Rate_Limit_PIR);
		}else{                 //tmpMaxTxLimitRate =0
            if(txRateLimitPtr->chnlRateLimitEn == QDMA_DISABLE)
			    newUnit = TX_RATELIMIT_TICKER1_ACCURACY;       //t1 -> disable
		}

		tmpMaxPirValue = tmpMaxTxLimitRate * unit1;
		if(tmpMaxPirValue > txRateLimitPtr->rateLimitValue)
			newUnit = (tmpMaxPirValue - 1)/0xFFFF + 1;  /*round up to nearest int num*/;
		if(newUnit <= TX_RATELIMIT_TICKER1_ACCURACY)
        	newUnit = TX_RATELIMIT_TICKER1_ACCURACY;
	}

#ifdef QDMA_LAN
	/* to make sure qdma_lan burst size smaller than 40Kbyte */
	switchLeastUnit = txRateLimitPtr->rateLimitValue / (SWITCH_BUFFER_THRESHOLD*1000) ;
	newUnit = MAX(newUnit, switchLeastUnit) ;
#endif
	
	/* update ratelimit unit1 and all channel's pir value */		
	if(newUnit != unit1)
	{
		curTicksel = 8000 / newUnit;
		qdmaSetTxRateLimitTicksel(base, 1, curTicksel) ;
		qdmaUpdateAllTxRateLimitValue(unit1, newUnit,1) ;
		if(curTicksel_idx == 1)
			curUnit = newUnit;
	}
    if( curUnit == 0) {
		QDMA_ERR("Fault:qdma tx ratelimit Unit is zero\n") ; 
		return -EINVAL ;
	}
	if((ret = qdmaSetTxRateLimitConfig(txRateLimitPtr->chnlIdx, QDMA_Tx_Rate_Limit_PIR, txRateLimitPtr->rateLimitValue / curUnit)) < 0) {
		return ret ;
	}
	/* pbs = 2*0.625*pir */
	if((ret = qdmaSetTxRateLimitConfig(txRateLimitPtr->chnlIdx, QDMA_Tx_Rate_Limit_PBS, ((txRateLimitPtr->rateLimitValue>>1) + (txRateLimitPtr->rateLimitValue>>3)) / curUnit)) < 0) {
		return ret ;
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of tx rate limit parameter. It includes PIR and PBS.
 Input Args:	arg1: The pointer of the  TX RateLimit Parameter struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data) 
{
	int ret = 0, ticksel_idx = 0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel=0, curUnit=0 ;
	QDMA_TxRateLimitGet_T *txRateLimitPtr = qdma_data->qdma_private.txRateLimitGetPtr;
	int port = 0;
	int switch_port =0;
	
	if(txRateLimitPtr->chnlIdx>=CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}

	if(TCSUPPORT_FWC_ENV_VAL)
	{
#ifdef QDMA_LAN
		if(0 != txRateLimitPtr->chnlIdx)
		{
			port = txRateLimitPtr->chnlIdx - 1;   
			if(MT7530LanPortMap2Switch_hook)
			{
				switch_port = MT7530LanPortMap2Switch_hook(port);
			}
			txRateLimitPtr->chnlIdx = switch_port;
		}  
#endif
	}  
	ticksel_idx = qdmaIsTxRateLimitChnlTickSel1(base,txRateLimitPtr->chnlIdx)?1:0;
	curTicksel = qdmaGetTxRateLimitTicksel(base,ticksel_idx);
	if(curTicksel == 0) 
		curUnit = 0 ;
	else
		curUnit = 8000 / curTicksel ;
	
	if(qdmaIsChnlRateLimitEnable(base, txRateLimitPtr->chnlIdx)) {
		txRateLimitPtr->chnlRateLimitEn = QDMA_ENABLE ;
	} else {
		txRateLimitPtr->chnlRateLimitEn = QDMA_DISABLE ;
	}
	
	ret = qdmaGetTxRateLimitConfig(txRateLimitPtr->chnlIdx, QDMA_Tx_Rate_Limit_PIR) ;
	if(ret < 0)
		return ret ;
	txRateLimitPtr->rateLimitValue = ret * curUnit ;
	
	ret = qdmaGetTxRateLimitConfig(txRateLimitPtr->chnlIdx, QDMA_Tx_Rate_Limit_PBS) ;
	if(ret < 0)
		return ret ;
	txRateLimitPtr->pbsParamValue = ret * (1<<qdmaGetTxRateLimitBucketScale(base)) ;
		
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to set the value of rx rate limit config. It includes
 			rx_rateLimit_En, bucketScale and tickSel.
 Input Args:	arg1: The pointer of the RX Rate Limit config struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_rx_ratelimit_pkt_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_RxPktMode_t pktMode = qdma_data->qdma_private.pktMode;

	if(pktMode == QDMA_RX_RATE_LIMIT_BY_PACKET) {
		qdmaSetRxRateLimitByPacket(base) ;
	} else {
		qdmaSetRxRateLimitByByte(base) ;
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of rx rate limit config. It includes
 			rx_rateLimit_En, bucketScale and tickSel.
 Input Args:	arg1: The pointer of the RX Rate Limit config struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_rx_ratelimit_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel=0, unitBase=0;	//unitBase means the unit when ticksel is 1us.
	QDMA_RxRateLimitCfg_T *rxRateLimitCfgPtr = qdma_data->qdma_private.rxRateLimitCfgPtr;

	if(qdmaIsRxRateLimitEnable(base)) {
		rxRateLimitCfgPtr->rxRateLimitEn = QDMA_ENABLE ;
	} else {
		rxRateLimitCfgPtr->rxRateLimitEn = QDMA_DISABLE ;
	}
	
	if(qdmaIsRxRateLimitByPacket(base)) {
		rxRateLimitCfgPtr->rxRateLimitPktMode = QDMA_RX_RATE_LIMIT_BY_PACKET ;
		unitBase = 1000000 ;
	} else {
		rxRateLimitCfgPtr->rxRateLimitPktMode = QDMA_RX_RATE_LIMIT_BY_BYTE ;
		unitBase = 8000 ;
	}
	
	curTicksel = qdmaGetRxRateLimitTicksel(base, 1);
	if(curTicksel == 0)
		rxRateLimitCfgPtr->rxRateLimitUnit = 0 ;
	else
		rxRateLimitCfgPtr->rxRateLimitUnit = unitBase/curTicksel ;	/*default ticekr(ticker1)*/
	
	curTicksel = qdmaGetRxRateLimitTicksel(base, 0);
	if(curTicksel == 0)
		rxRateLimitCfgPtr->rxRateLimitUnit0 = 0 ;
	else
		rxRateLimitCfgPtr->rxRateLimitUnit0 = unitBase/curTicksel ; /*ticekr0*/

	rxRateLimitCfgPtr->rxRateLimitBucketScale = qdmaGetRxRateLimitBucketScale(base) ;
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set the value of rx rate limit parameter. It includes PIR and PBS.
 Input Args:	arg1: The pointer of the RX RateLimit Parameter struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_rx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int ret = 0, unitBase=0;	//unitBase means the unit when ticksel is 1us.
	int curTicksel=0, curUnit=0, newUnit=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_RxRateLimitSet_T *rxRateLimitPtr = qdma_data->qdma_private.rxRateLimitSetPtr;
	unchar ticker=0;
	if(rxRateLimitPtr->ringIdx>=RX_RING_NUM) {
		QDMA_ERR("Fault: ringIdx shoule be 0 ~ %d\n",(RX_RING_NUM-1)) ; 
		return -EINVAL ;
	}
	
	if(qdmaIsRxRateLimitByPacket(base)) {
		unitBase = 1000000 ;
	} else {
		unitBase = 8000 ;
	}
	
	/* get new unit and current unit */
	newUnit = 1 + rxRateLimitPtr->rateLimitValue / 0xFFFF;
	ticker = qdmaGetRxRatelimitSelTick(base, rxRateLimitPtr->ringIdx);	/*ticker0 or ticker1*/
	curTicksel = qdmaGetRxRateLimitTicksel(base, ticker);

	if(curTicksel == 0) 
		curUnit = 0 ;
	else
		curUnit = unitBase / curTicksel ;
	
	/* update ratelimit unit and all channel's pir value */		
	if(newUnit > curUnit) {
		curTicksel = unitBase / newUnit;

		qdmaSetRxRateLimitTickVal(base, ticker, curTicksel) ;
		qdmaUpdateAllRxRateLimitValue(curUnit, newUnit, ticker) ;

		curUnit = newUnit;
	}
	
	if((ret = qdmaSetRxRateLimitConfig(rxRateLimitPtr->ringIdx, QDMA_Rx_Rate_Limit_PIR, rxRateLimitPtr->rateLimitValue / curUnit)) < 0) {
		return ret ;
	}
	if((ret = qdmaSetRxRateLimitConfig(rxRateLimitPtr->ringIdx, QDMA_Rx_Rate_Limit_PBS, rxRateLimitPtr->rateLimitValue / curUnit)) < 0) {
		return ret ;
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of rx rate limit parameter. It includes PIR and PBS.
 Input Args:	arg1: The pointer of the  RX RateLimit Parameter struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_rx_ratelimit(struct ECNT_QDMA_Data *qdma_data) 
{
	int ret = 0 , unitBase=0;	/* unitBase means the unit when ticksel is 1us. */
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel=0, curUnit=0 ;
	QDMA_RxRateLimitGet_T *rxRateLimitPtr = qdma_data->qdma_private.rxRateLimitGetPtr;
	unchar ticker=0;

	if(rxRateLimitPtr->ringIdx>=RX_RING_NUM) {
		QDMA_ERR("Fault: ringIdx shoule be 0 ~ %d\n",(RX_RING_NUM-1)) ; 
		return -EINVAL ;
	}
	
	if(qdmaIsRxRateLimitByPacket(base)) {
		unitBase = 1000000 ;
	} else {
		unitBase = 8000 ;
	}
		
	ticker = qdmaGetRxRatelimitSelTick(base, rxRateLimitPtr->ringIdx);	/*ticker0 or ticker1*/
	curTicksel = qdmaGetRxRateLimitTicksel(base, ticker);
	rxRateLimitPtr->rxBindTicker = ticker;

	if(curTicksel == 0) 
		curUnit = 0 ;
	else
		curUnit = unitBase / curTicksel ;
	
	ret = qdmaGetRxRateLimitConfig(rxRateLimitPtr->ringIdx, QDMA_Rx_Rate_Limit_PIR) ;
	if(ret < 0)
		return ret ;
	rxRateLimitPtr->rateLimitValue = ret * curUnit ;
	
	ret = qdmaGetRxRateLimitConfig(rxRateLimitPtr->ringIdx, QDMA_Rx_Rate_Limit_PBS) ;
	if(ret < 0)
		return ret ;
	rxRateLimitPtr->pbsParamValue = ret * (1<<qdmaGetRxRateLimitBucketScale(base));
		
	return 0 ;
}
#endif

/******************************************************************************
 Descriptor:	It's used to set the value of tx dba report parameter. It includes CIR, CBS, PIR and PBS.
 Input Args:	arg1: The pointer of the TX RateLimit Parameter struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_tx_dba_report(struct ECNT_QDMA_Data *qdma_data)
{
	int ret = 0 ;
	uint base = gpQdmaPriv->csrBaseAddr, maxPirValue=0;
	int curTicksel=0, curUnit=0, newUnit=0 ;
	int ticksel_idx=0;
	QDMA_TxDbaReport_T *txDbaReportPtr = qdma_data->qdma_private.txDbaReportPtr;
	
	if(txDbaReportPtr->channel>=CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index shoule between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}

#if SUPPORT_MTR_ACNT_IN_QDMA
	/*1. set CIR cfg & value*/
	if((ret = generalSetTrtcmMeterMode( EGRESS_TRTCM, GENERAL_METER_ENABLE, TRTCM_COMMIT_MODE, txDbaReportPtr->channel )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmPktMode( EGRESS_TRTCM, TRTCM_BYTE_MODE, TRTCM_COMMIT_MODE, txDbaReportPtr->channel )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmTickSel( EGRESS_TRTCM, TRTCM_FAST_TICK, TRTCM_COMMIT_MODE, txDbaReportPtr->channel )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmTokenRate(EGRESS_TRTCM, txDbaReportPtr->cirParamValue, TRTCM_COMMIT_MODE, txDbaReportPtr->channel)) < 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}
	if((ret = generalSetTrtcmBucketSize(EGRESS_TRTCM, txDbaReportPtr->cbsParamValue, TRTCM_COMMIT_MODE, txDbaReportPtr->channel))< 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}

	/*2. set PIR cfg & value*/
	if((ret = generalSetTrtcmMeterMode( EGRESS_TRTCM, GENERAL_METER_ENABLE, TRTCM_PEAK_MODE, txDbaReportPtr->channel )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmPktMode( EGRESS_TRTCM, TRTCM_BYTE_MODE, TRTCM_PEAK_MODE, txDbaReportPtr->channel )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmTickSel( EGRESS_TRTCM, TRTCM_FAST_TICK, TRTCM_PEAK_MODE, txDbaReportPtr->channel )) < 0){
		QDMA_ERR("Fault:Set trtcm mode cfg error.\n") ; 
		return ret;
	}
	if((ret = generalSetTrtcmTokenRate(EGRESS_TRTCM, txDbaReportPtr->pirParamValue, TRTCM_PEAK_MODE, txDbaReportPtr->channel)) < 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}
	if((ret = generalSetTrtcmBucketSize(EGRESS_TRTCM, txDbaReportPtr->pbsParamValue, TRTCM_PEAK_MODE, txDbaReportPtr->channel))< 0) {
		QDMA_ERR("Fault: set trtcm mode value error.\n");
		return ret ;
	}
#else
	if(txDbaReportPtr->pirParamValue > txDbaReportPtr->cirParamValue)
		maxPirValue = txDbaReportPtr->pirParamValue ;
	else
		maxPirValue = txDbaReportPtr->cirParamValue ;
	

	/*ticker0 default as 1kbps, you'd better not change ticker0*/
	if(maxPirValue <= TX_RATELIMIT_TICKER0_ACCURACY*0xFFFF){    /* 0kbps ~64Mbps */
		newUnit = TX_RATELIMIT_TICKER0_ACCURACY;
		ticksel_idx= 0;
		if(qdmaIsTxRateLimitChnlTickSel1(base,txDbaReportPtr->channel))
			qdmaSetTxRateLimitPerChnlTickSelIdx(base, txDbaReportPtr->channel,0);
	}else{     /* 64Mbps ~4Gbps */
		newUnit = (maxPirValue - 1)/0xFFFF + 1;  /*round up to nearest int num*/
		ticksel_idx= 1;
		if(!qdmaIsTxRateLimitChnlTickSel1(base,txDbaReportPtr->channel))
			qdmaSetTxRateLimitPerChnlTickSelIdx(base, txDbaReportPtr->channel,1);
	}

	curTicksel = qdmaGetTxRateLimitTicksel(base, ticksel_idx);
	if(curTicksel == 0)
		curUnit = 0 ;
	else
		curUnit = 8000 / curTicksel ;
	
	/* update ratelimit unit and all channel's pir value */		
	if(newUnit > curUnit) {
		curTicksel = 8000 / newUnit;
		qdmaSetTxRateLimitTicksel(base,ticksel_idx, curTicksel) ;	
		qdmaUpdateAllTxRateLimitValue(curUnit, newUnit, ticksel_idx) ;	
		curUnit = newUnit;
	}
	
	if((ret = qdmaSetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_CIR, txDbaReportPtr->cirParamValue / curUnit)) < 0) {
		return ret ;
	}
	if((ret = qdmaSetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_CBS, txDbaReportPtr->cbsParamValue / (1<<qdmaGetTxRateLimitBucketScale(base)))) < 0) {
		return ret ;
	}
	if((ret = qdmaSetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_PIR, txDbaReportPtr->pirParamValue / curUnit)) < 0) {
		return ret ;
	}
	if((ret = qdmaSetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_PBS, txDbaReportPtr->pbsParamValue) / (1<<qdmaGetTxRateLimitBucketScale(base))) < 0) {
		return ret ;
	}
#endif
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of tx rate limit parameter. It includes PIR and PBS.
 Input Args:	arg1: The pointer of the  TX RateLimit Parameter struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_tx_dba_report(struct ECNT_QDMA_Data *qdma_data) 
{
	int ret = 0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel=0, curUnit=0 ;
	int ticksel_idx=0;
	QDMA_TxDbaReport_T *txDbaReportPtr = qdma_data->qdma_private.txDbaReportPtr;
	
	if( txDbaReportPtr->channel>=CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index shoule between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}

#if SUPPORT_MTR_ACNT_IN_QDMA	
	/*1. get CIR value*/
	txDbaReportPtr->cirParamValue = generalGetTrtcmTokenRate(EGRESS_TRTCM, TRTCM_COMMIT_MODE, txDbaReportPtr->channel);
	txDbaReportPtr->cbsParamValue = generalGetTrtcmBucketSize(EGRESS_TRTCM, TRTCM_COMMIT_MODE, txDbaReportPtr->channel);

	/*2. get PIR value*/
	txDbaReportPtr->pirParamValue = generalGetTrtcmTokenRate(EGRESS_TRTCM, TRTCM_PEAK_MODE, txDbaReportPtr->channel);
	txDbaReportPtr->pbsParamValue = generalGetTrtcmBucketSize(EGRESS_TRTCM, TRTCM_PEAK_MODE, txDbaReportPtr->channel);
#else

	ticksel_idx = qdmaIsTxRateLimitChnlTickSel1(base, txDbaReportPtr->channel)?1:0;
	curTicksel = qdmaGetTxRateLimitTicksel(base, ticksel_idx);
	if(curTicksel == 0) 
		curUnit = 0 ;
	else
		curUnit = 8000 / curTicksel ;
			
	ret = qdmaGetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_CIR) ;
	if(ret < 0)
		return ret ;
	txDbaReportPtr->cirParamValue = ret * curUnit ;
	
	ret = qdmaGetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_CBS) ;
	if(ret < 0)
		return ret ;
	txDbaReportPtr->cbsParamValue = ret * (1<<qdmaGetTxRateLimitBucketScale(base)) ;
	
	ret = qdmaGetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_PIR) ;
	if(ret < 0)
		return ret ;
	txDbaReportPtr->pirParamValue = ret * curUnit ;
	
	ret = qdmaGetTxRateLimitConfig(txDbaReportPtr->channel, QDMA_Tx_Rate_Limit_PBS) ;
	if(ret < 0)
		return ret ;
	txDbaReportPtr->pbsParamValue = ret * (1<<qdmaGetTxRateLimitBucketScale(base)) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable rx protect mode. 
 Input Args:	arg1: Enable/disable rx protect mode (QDMA_ENABLE/QDMA_DISABLE)
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_rx_protect_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_RX_PROTECT_MODE
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_Mode_t rxDropMode = qdma_data->qdma_private.mode;
	
	if(rxDropMode == QDMA_ENABLE) {
		qdmaEnableRxDrop(base) ;
	} else {
		qdmaDisableRxDrop(base) ;
	}
#endif
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get rx protect mode:enable/disable.
 Input Args:	No input arguments
 Ret Value:		return rx protect mode (QDMA_ENABLE/QDMA_DISABLE)
******************************************************************************/
int qdma_get_rx_protect_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_RX_PROTECT_MODE
	uint base = gpQdmaPriv->csrBaseAddr ;
	if(qdmaIsRxDropEnable(base)) {
		qdma_data->qdma_private.mode = QDMA_ENABLE ;
	} else {
		qdma_data->qdma_private.mode = QDMA_DISABLE ;
	}
	return qdma_data->qdma_private.mode ;
#else
	return 0 ;
#endif
}

/******************************************************************************
 Descriptor:	It's used to set the value of rx low threshold. 
 Input Args:	arg1: The pointer of the RX Low Threshold struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_rx_low_threshold(struct ECNT_QDMA_Data *qdma_data)
{

	uint base = gpQdmaPriv->csrBaseAddr ;
	int ringIdx=0;
	QDMA_RxLowThreshold_T *rxLowThresholdPtr = qdma_data->qdma_private.rxLowThresholdPtr;

	for(ringIdx=0;ringIdx<RX_RING_NUM;ringIdx++){
		if(rxLowThresholdPtr->rxRingLowThrh[ringIdx] > qdmaGetRxRingSize(base, ringIdx))
		{
			QDMA_ERR("Fault: rxRingLowThrh_%d is %d, should less than %d\n"
				,ringIdx, rxLowThresholdPtr->rxRingLowThrh[ringIdx], qdmaGetRxRingSize(base, ringIdx)) ; 
			return -EINVAL ;
		}
		qdmaSetRxRingThrh(base, ringIdx, rxLowThresholdPtr->rxRingLowThrh[ringIdx]) ;
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of rx low threshold. 
 Input Args:	arg1: The pointer of the RX Low Threshold struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_rx_low_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_RxLowThreshold_T *rxLowThresholdPtr = qdma_data->qdma_private.rxLowThresholdPtr;
	int ringIdx=0;
	for(ringIdx=0;ringIdx<RX_RING_NUM;ringIdx++)
		rxLowThresholdPtr->rxRingLowThrh[ringIdx] = qdmaGetRxRingThrh(base, ringIdx) ;

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable tx queue DEI dropped function. 
 Input Args:	arg1: Enable/disable tx queue DEI function (QDMA_ENABLE/QDMA_DISABLE)
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_dei_drop_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_Mode_t deiDropMode = qdma_data->qdma_private.mode ;

	if(deiDropMode == QDMA_ENABLE) {
		qdmaEnableTxqCngstDeiDrop(base) ;
	} else {
		qdmaDisableTxqCngstDeiDrop(base) ;
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get tx queue DEI dropped mode. 
 Input Args:	No input arguments
 Ret Value:		return tx queue DEI mode (QDMA_ENABLE/QDMA_DISABLE)
******************************************************************************/
int qdma_get_txq_dei_drop_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(qdmaIsTxqCngstDeiDropEnable(base)) {
		qdma_data->qdma_private.mode = QDMA_ENABLE ;
	} else {
		qdma_data->qdma_private.mode = QDMA_DISABLE ;
	}

	return qdma_data->qdma_private.mode ;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable tx queue dynamic cngst mode. 
 Input Args:	arg1: Enable/disable tx queue dynamic cngst (QDMA_ENABLE/QDMA_DISABLE)
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_Mode_t dynCngstEn = qdma_data->qdma_private.mode ;

	if(dynCngstEn == QDMA_ENABLE) {
		qdmaEnableTxqDynCngstEn(base) ;
#if SUPPORT_SET_TX_QUEUE_MIN_THRH
		qdmaSetTxqMinDscpThrshld(base, 0x4 * (qdmaGetHwPayloadSize(base)+1)) ;
#endif
	} else {
		qdmaDisableTxqDynCngstEn(base) ;
#if SUPPORT_SET_TX_QUEUE_MIN_THRH
		qdmaSetTxqMinDscpThrshld(base, 0) ;
#endif
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get tx queue dynamic cngst mode. 
 Input Args:	No input arguments
 Ret Value:		return tx queue dynamic cngst mode.  (QDMA_ENABLE/QDMA_DISABLE)
******************************************************************************/
int qdma_get_txq_cngst_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(qdmaIsTxqDynCngstEnable(base)) {
		qdma_data->qdma_private.mode = QDMA_ENABLE ;
	} else {
		qdma_data->qdma_private.mode = QDMA_DISABLE ;
	}
	
	return qdma_data->qdma_private.mode ;
}

/******************************************************************************
 Descriptor:	It's used to set tx queue dei threshold scale, scope is (1/2 ~ 1/16) 
 Input Args:	arg1: set tx queue dei threshold scale  (1/2 ~ 1/16)
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_dei_threshold_scale(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint threshold = qdma_data->qdma_private.threshold;
	qdmaSetDynCngstDeiThrhScale(base, threshold);
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get tx queue dei threshold scale, scope is (1/2 ~ 1/16) 
 Input Args:	No input arguments
 Ret Value:	return tx queue dei threshold scale  (1/2 ~ 1/16)
******************************************************************************/
int qdma_get_txq_dei_threshold_scale(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	qdma_data->qdma_private.threshold = qdmaGetDynCngstDeiThrhScale(base) ;
	return qdma_data->qdma_private.threshold ;
}

/* need modify later */
int qdma_set_txq_cngst_auto_config(struct ECNT_QDMA_Data *qdma_data)
{
    uint ringIdx=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstThrh_T txqDynCngstThrh;
	QDMA_txCngstCfg_t *pTxCngstCfg = qdma_data->qdma_private.pTxCngstCfg;

	/********************************************
	* Setting tx dynamic cngst register               *
	*********************************************/
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		qdmaDisableCngstMaxThrhEn(base, ringIdx) ;
		qdmaDisableCngstMinThrhEn(base, ringIdx) ;
	}

#if NEED_ENABLE_TX_RING_BLOCKING
#if defined(QDMA_LAN)
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		qdmaEnableCngstMinThrhEn(base, ringIdx) ;
	}
#endif
#endif
	qdmaEnableCngstModeConfigTrig(base);
	qdmaEnableCngstModePacketTrig(base);
	qdmaEnableCngstModeTimeTrig(base);

	qdmaEnableTxRateMeter(base);

	if(gpQdmaPriv->hwFwdDscpNum <= 256) {
		txqDynCngstThrh.dynCngstTotalMinThrh = 48;
		txqDynCngstThrh.dynCngstChnlMinThrh = 2;
		txqDynCngstThrh.dynCngstQueueMinThrh = 2;
		txqDynCngstThrh.dynCngstTotalMaxThrh = BUFFER_THRESHOLD[0];
		txqDynCngstThrh.dynCngstChnlMaxThrh = BUFFER_THRESHOLD[2];
		txqDynCngstThrh.dynCngstQueueMaxThrh = BUFFER_THRESHOLD[4];
	} else if(gpQdmaPriv->hwFwdDscpNum <= gpQdmaPriv->hwFwdDscpMaxNum){ 
		txqDynCngstThrh.dynCngstTotalMinThrh = BUFFER_THRESHOLD[1];
		txqDynCngstThrh.dynCngstChnlMinThrh = BUFFER_THRESHOLD[3];
        txqDynCngstThrh.dynCngstQueueMinThrh = BUFFER_THRESHOLD[5];
		txqDynCngstThrh.dynCngstTotalMaxThrh = BUFFER_THRESHOLD[0];
		txqDynCngstThrh.dynCngstChnlMaxThrh = BUFFER_THRESHOLD[2];
		txqDynCngstThrh.dynCngstQueueMaxThrh = BUFFER_THRESHOLD[4];
	} else {
		printk("Error: CONFIG_HWFWD_DSCP_NUM should less than %d\n",gpQdmaPriv->hwFwdDscpMaxNum);
		return -1;
	}

    #if SUPPORT_TXQ_CNGST_MIN_THRH
        txqDynCngstThrh.dynCngstMinDscpThrh = TXQ_CNGST_MIN_THRH;
    #endif
	
	if(pTxCngstCfg->txCngstMode == QDMA_TXCNGST_STATIC) {
		qdmaDisableTxqDynCngstEn(base);
		qdmaDisableTxRateMeterPeakRate(base);
		/*  open api for mac setting */
	} else if(pTxCngstCfg->txCngstMode == QDMA_TXCNGST_DYNAMIC_NORMAL) {
		qdmaEnableTxqDynCngstEn(base);
#if SUPPORT_TXQ_CNGS_DYN_MODE_ALWAYS_ON
		qdmaEnableTxqCngstDrop(base);
#endif
		
#if (!SUPPORT_TX_PEAK_MODE) || (!defined(QDMA_LAN))
		qdmaDisableTxRateMeterPeakRate(base);
#endif
		qdma_data->qdma_private.dynCngstThrhPtr = &txqDynCngstThrh;
		qdma_set_txq_cngst_dynamic_threshold(qdma_data);
	} else if(pTxCngstCfg->txCngstMode == QDMA_TXCNGST_DYNAMIC_PEAKRATE_MARGIN){
		qdmaEnableTxqDynCngstEn(base);
		qdmaEnableTxRateMeterPeakRate(base);
		qdmaSetTxPeekRateMargin(base, pTxCngstCfg->peekRateMargin);
		qdmaSetTxPeekRateDuration(base, pTxCngstCfg->peekRateDuration>>4);
		qdma_data->qdma_private.dynCngstThrhPtr = &txqDynCngstThrh;
		qdma_set_txq_cngst_dynamic_threshold(qdma_data);	/*  notice that peekrate mode should set different threshold */
	}

#if SUPPORT_TXQ_DEI_DROP
	/* DEI buffer threshold init, half of normal threshold */
	qdmaSetTxqDEITotalMaxThrh(base, (txqDynCngstThrh.dynCngstTotalMaxThrh >> 1));
	qdmaSetTxqDEITotalMinThrh(base, (txqDynCngstThrh.dynCngstTotalMinThrh >> 1));
	qdmaSetTxqDEIChnlMaxThrh(base, (txqDynCngstThrh.dynCngstChnlMaxThrh >> 1));
	qdmaSetTxqDEIChnlMinThrh(base, (txqDynCngstThrh.dynCngstChnlMinThrh >> 1));
	qdmaSetTxqDEIQueueMaxThrh(base, (txqDynCngstThrh.dynCngstQueueMaxThrh >> 1));
	qdmaSetTxqDEIQueueMinThrh(base, (txqDynCngstThrh.dynCngstQueueMinThrh >> 1));
#endif

#if	SUPPORT_FAST_THR
	qdmaSetTxqDEITotalMaxThrh(base, BUFF_FAST_TOTAL_MAX_THRH);
	qdmaSetTxqDEITotalMinThrh(base, BUFF_FAST_TOTAL_MIN_THRH);
#endif
	return 0;
}

int qdma_get_txq_cngst_auto_config(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_txCngstCfg_t *pTxCngstCfg = qdma_data->qdma_private.pTxCngstCfg;
	
	if(qdmaIsTxqDynCngstEnable(base)) {
		if(qdmaIsTxRateMeterPeakRateEnable(base)) {
			pTxCngstCfg->txCngstMode = QDMA_TXCNGST_DYNAMIC_PEAKRATE_MARGIN;
			pTxCngstCfg->peekRateDuration = qdmaGetTxPeekRateDuration(base);
			pTxCngstCfg->peekRateMargin = qdmaGetTxPeekRateMargin(base);
		} else {
			pTxCngstCfg->txCngstMode = QDMA_TXCNGST_DYNAMIC_NORMAL;
		}
	} else {
		pTxCngstCfg->txCngstMode = QDMA_TXCNGST_STATIC;
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set the value of tx dynamic threshold. It includes 
 			total max threshold, total min Threshold, channel max threshold, 
 			channel min threshold, queue max threshold, queue min threshold.
 Input Args:	arg1: The pointer of the TX dynamic threshold struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_dynamic_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstThrh_T *txqCngstThrhPtr = qdma_data->qdma_private.dynCngstThrhPtr;
	
	if(txqCngstThrhPtr->dynCngstTotalMinThrh > txqCngstThrhPtr->dynCngstTotalMaxThrh) {
		QDMA_ERR("Fault: Dynamic Total Min Threshold should less than Dynamic Total Max Threshold.\n") ; 
		return -EINVAL ;
	}
	
	if(txqCngstThrhPtr->dynCngstChnlMinThrh > txqCngstThrhPtr->dynCngstChnlMaxThrh) {
		QDMA_ERR("Fault: Dynamic Channel Min Threshold should less than Dynamic Channel Max Threshold.\n") ; 
		return -EINVAL ;
	}
	
	if(txqCngstThrhPtr->dynCngstQueueMinThrh > txqCngstThrhPtr->dynCngstQueueMaxThrh) {
		QDMA_ERR("Fault: Dynamic Queue Min Threshold should less than Dynamic Queue Max Threshold.\n") ; 
		return -EINVAL ;
	}

	qdmaSetTxqDynTotalMaxThrh(base, txqCngstThrhPtr->dynCngstTotalMaxThrh) ;
	qdmaSetTxqDynTotalMinThrh(base, txqCngstThrhPtr->dynCngstTotalMinThrh) ;
	
	qdmaSetTxqDynChnlMaxThrh(base, txqCngstThrhPtr->dynCngstChnlMaxThrh) ;
	qdmaSetTxqDynChnlMinThrh(base, txqCngstThrhPtr->dynCngstChnlMinThrh) ;
	
	qdmaSetTxqDynQueueMaxThrh(base, txqCngstThrhPtr->dynCngstQueueMaxThrh) ;
	qdmaSetTxqDynQueueMinThrh(base, txqCngstThrhPtr->dynCngstQueueMinThrh) ;

#if SUPPORT_TXQ_CNGST_MIN_THRH
	qdmaSetTxqMinDscpThrshld(base, txqCngstThrhPtr->dynCngstMinDscpThrh) ;
#endif

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of tx dynamic threshold. It includes 
 			total max threshold, total min Threshold, channel max threshold, 
 			channel min threshold, queue max threshold, queue min threshold.
 Input Args:	arg1: The pointer of the TX dynamic threshold struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_txq_cngst_dynamic_threshold(struct ECNT_QDMA_Data *qdma_data) 
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstThrh_T *txqCngstThrhPtr = qdma_data->qdma_private.dynCngstThrhPtr;
	
	txqCngstThrhPtr->dynCngstTotalMaxThrh = qdmaGetTxqDynTotalMaxThrh(base) ;
	txqCngstThrhPtr->dynCngstTotalMinThrh = qdmaGetTxqDynTotalMinThrh(base) ;
	
	txqCngstThrhPtr->dynCngstChnlMaxThrh = qdmaGetTxqDynChnlMaxThrh(base) ;
	txqCngstThrhPtr->dynCngstChnlMinThrh = qdmaGetTxqDynChnlMinThrh(base) ;
	
	txqCngstThrhPtr->dynCngstQueueMaxThrh = qdmaGetTxqDynQueueMaxThrh(base) ;
	txqCngstThrhPtr->dynCngstQueueMinThrh = qdmaGetTxqDynQueueMinThrh(base) ;

#if SUPPORT_TXQ_CNGST_MIN_THRH
	txqCngstThrhPtr->dynCngstMinDscpThrh = qdmaGetTxqMinDscpThrshld(base) ;
#endif

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to set txQ total max/min threshold, unit is kByte.
 Input Args:	arg1: set txQ total max/min threshold,
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_total_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr = qdma_data->qdma_private.totalThrhPtr;
	
	if(totalThrhPtr->dynCngstTotalMinThrh > totalThrhPtr->dynCngstTotalMaxThrh) {
		QDMA_ERR("Fault: Dynamic Total Min Threshold should less than Dynamic Total Max Threshold.\n") ; 
		return -EINVAL ;
	}
	
	qdmaSetTxqDynTotalMaxThrh(base, totalThrhPtr->dynCngstTotalMaxThrh<<2) ;
	qdmaSetTxqDynTotalMinThrh(base, totalThrhPtr->dynCngstTotalMinThrh<<2) ;
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ total max/min threshold, unit is kByte.
 Input Args:	No input arguments
 Ret Value:	return txQ total max/min threshold,
******************************************************************************/
int qdma_get_txq_cngst_total_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr = qdma_data->qdma_private.totalThrhPtr;
	
	totalThrhPtr->dynCngstTotalMaxThrh = qdmaGetTxqDynTotalMaxThrh(base)>>2 ;
	totalThrhPtr->dynCngstTotalMinThrh = qdmaGetTxqDynTotalMinThrh(base)>>2 ;

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ channel  max/min threshold, unit is kByte.
 Input Args:	arg1: set txQ channel max/min threshold,
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_channel_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstChnlThrh_T *chnlThrhPtr = qdma_data->qdma_private.chnlThrhPtr;
	
	if(chnlThrhPtr->dynCngstChnlMinThrh > chnlThrhPtr->dynCngstChnlMaxThrh) {
		QDMA_ERR("Fault: Dynamic Channel Min Threshold should less than Dynamic Channel Max Threshold.\n") ; 
		return -EINVAL ;
	}	

	qdmaSetTxqDynChnlMaxThrh(base, chnlThrhPtr->dynCngstChnlMaxThrh<<2) ;
	qdmaSetTxqDynChnlMinThrh(base, chnlThrhPtr->dynCngstChnlMinThrh<<2) ;
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ channel  max/min threshold, unit is kByte.
 Input Args:	No input arguments
 Ret Value:	return txQ channel max/min threshold,
******************************************************************************/
int qdma_get_txq_cngst_channel_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstChnlThrh_T *chnlThrhPtr = qdma_data->qdma_private.chnlThrhPtr;
	
	chnlThrhPtr->dynCngstChnlMaxThrh = qdmaGetTxqDynChnlMaxThrh(base)>>2 ;
	chnlThrhPtr->dynCngstChnlMinThrh = qdmaGetTxqDynChnlMinThrh(base)>>2 ;

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ queue max/min threshold, unit is kByte.
 Input Args:	arg1: set txQ queue max/min threshold,
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_queue_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstQueueThrh_T *queueThrhPtr = qdma_data->qdma_private.queueThrhPtr;
	
	if(queueThrhPtr->dynCngstQueueMinThrh > queueThrhPtr->dynCngstQueueMaxThrh) {
		QDMA_ERR("Fault: Dynamic Queue Min Threshold should less than Dynamic Queue Max Threshold.\n") ; 
		return -EINVAL ;
	}	

	qdmaSetTxqDynQueueMaxThrh(base, queueThrhPtr->dynCngstQueueMaxThrh<<2) ;
	qdmaSetTxqDynQueueMinThrh(base, queueThrhPtr->dynCngstQueueMinThrh<<2) ;
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ queue max/min threshold, unit is kByte.
 Input Args:	No input arguments
 Ret Value:	return txQ queue max/min threshold,
******************************************************************************/
int qdma_get_txq_cngst_queue_threshold(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstQueueThrh_T *queueThrhPtr = qdma_data->qdma_private.queueThrhPtr;
	
	queueThrhPtr->dynCngstQueueMaxThrh = qdmaGetTxqDynQueueMaxThrh(base)>>2 ;
	queueThrhPtr->dynCngstQueueMinThrh = qdmaGetTxqDynQueueMinThrh(base)>>2 ;

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ queue nonblocking or not.
 Input Args:	arg1: The pointer of the TXQ Cngst Queue Cfg struct.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_queue_nonblocking(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQCngstQueueCfg_T *txqCngstQueueCfgPtr = qdma_data->qdma_private.txqCngstQueueCfgPtr;
	
	if (txqCngstQueueCfgPtr->queueMode == QDMA_QUEUE_NONBLOCKING) {
		qdmaEnableTxqCngstQueueNonblocking(base, txqCngstQueueCfgPtr->queue);
	}
	else {
		qdmaDisableTxqCngstQueueNonblocking(base, txqCngstQueueCfgPtr->queue);
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ queue nonblocking or not.
 Input Args:	No input arguments
 Ret Value:	return txQ queue nonblocking or not.
******************************************************************************/
int qdma_get_txq_cngst_queue_nonblocking(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQCngstQueueCfg_T *txqCngstQueueCfgPtr = qdma_data->qdma_private.txqCngstQueueCfgPtr;

	if(qdmaIsTxqCngstQueueNonblockingEnable(base, txqCngstQueueCfgPtr->queue)) {
		txqCngstQueueCfgPtr->queueMode = QDMA_QUEUE_NORMAL ;
	} else {
		txqCngstQueueCfgPtr->queueMode = QDMA_QUEUE_NONBLOCKING ;
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ channel nonblocking or not.
 Input Args:	arg1: The pointer of the TXQ Cngst Channel Cfg struct.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_channel_nonblocking(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQCngstChannelCfg_T *txqCngstChannelCfgPtr = qdma_data->qdma_private.txqCngstChannelCfgPtr;
	
	if (txqCngstChannelCfgPtr->channelMode == QDMA_CHANNEL_NONBLOCKING) {
		qdmaEnableTxqCngstChannelNonblocking(base, txqCngstChannelCfgPtr->channel);
	}
	else {
		qdmaDisableTxqCngstChannelNonblocking(base, txqCngstChannelCfgPtr->channel);
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ channel nonblocking or not.
 Input Args:	No input arguments
 Ret Value:	return txQ channel nonblocking or not.
******************************************************************************/
int qdma_get_txq_cngst_channel_nonblocking(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQCngstChannelCfg_T *txqCngstChannelCfgPtr = qdma_data->qdma_private.txqCngstChannelCfgPtr;

	if(qdmaIsTxqCngstChannelNonblockingEnable(base, txqCngstChannelCfgPtr->channel)) {
		txqCngstChannelCfgPtr->channelMode = QDMA_CHANNEL_NORMAL ;
	} else {
		txqCngstChannelCfgPtr->channelMode = QDMA_CHANNEL_NONBLOCKING ;
	}
	
	return 0;
}


/******************************************************************************
 Descriptor:	It's used to set txQ peekrate parameters.
 Input Args:	arg1: set txQ peekrate parameters, it includes
 			- peekRateEn: peekrate enable or disable.
 			- peekRateMargin: scope is [0~3], means (0, 25%, 50%, 100%)
 			- peekRateDuration: unit is ms
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_peekrate_params(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_PeekRateCfg_t *peekrateCfgPtr = qdma_data->qdma_private.peekrateCfgPtr;
	
	if(peekrateCfgPtr->peekRateEn == QDMA_ENABLE) {
		qdmaEnableTxRateMeterPeakRate(base) ;
	} else {
		qdmaDisableTxRateMeterPeakRate(base) ;
	}
	qdmaSetTxPeekRateMargin(base, peekrateCfgPtr->peekRateMargin) ;
	qdmaSetTxPeekRateDuration(base, peekrateCfgPtr->peekRateDuration>>4) ;
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ peekrate parameters.
 Input Args:	No input arguments
 Ret Value:	return txQ peekrate parameters.
******************************************************************************/
int qdma_get_txq_peekrate_params(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_PeekRateCfg_t *peekrateCfgPtr = qdma_data->qdma_private.peekrateCfgPtr;

	if(qdmaIsTxRateMeterPeakRateEnable(base))
		peekrateCfgPtr->peekRateEn = QDMA_ENABLE ;
	else
		peekrateCfgPtr->peekRateEn = QDMA_DISABLE ;

	peekrateCfgPtr->peekRateMargin = qdmaGetTxPeekRateMargin(base) ;
	peekrateCfgPtr->peekRateDuration = qdmaGetTxPeekRateDuration(base)<<4 ;

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ static queue normal threshold.
 Input Args:	arg1: set txQ static queue normal threshold, it includes
 			- normalThrh[8]: unit is byte, scope is 256Byte~16MByte.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_static_queue_normal_threshold(struct ECNT_QDMA_Data *qdma_data)
{
#if !SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
    uint base = gpQdmaPriv->csrBaseAddr ;
    int index = 0 ;
    QDMA_TxQStaticNormalCfg_T *normThrhPtr = qdma_data->qdma_private.normThrhPtr;

    for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
        qdmaSetTxqStaticQueueNormalThrh(base, index, normThrhPtr->normalThrh[index]<<2) ;
    }
#endif

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ static queue dei threshold.
 Input Args:	arg1: set txQ static queue dei threshold, it includes
 			- deiThrh[8]: unit is byte, scope is 256Byte~16MByte.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_static_queue_dei_threshold(struct ECNT_QDMA_Data *qdma_data)
{
#if !SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0 ;
	QDMA_TxQStaticDeiCfg_T *deiThrhPtr = qdma_data->qdma_private.deiThrhPtr;

	for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
		 qdmaSetTxqStaticQueueDeiThrh(base, index, deiThrhPtr->deiThrh[index]<<2) ;
	}
#endif

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ static cngst infomation.
 Input Args:	No input arguments
 Ret Value:	return txQ static cngst infomation.
******************************************************************************/
int qdma_get_txq_cngst_static_info(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
	uint base = gpQdmaPriv->csrBaseAddr;
	int index = 0;
	QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr = qdma_data->qdma_private.staticCfgPtr->totalThrhPtr;
	QDMA_TxQStaticCfg_T *staticThrhCfgPtr = qdma_data->qdma_private.staticCfgPtr->staticThrhCfgPtr;
	QDMA_TxQStaticThrRatio_T *staticThrhRatioPtr = qdma_data->qdma_private.staticCfgPtr->staticThrhRatioPtr;
	
	totalThrhPtr->dynCngstTotalMaxThrh = qdmaGetTxqDynTotalMaxThrh(base);
	totalThrhPtr->dynCngstTotalMinThrh = qdmaGetTxqDynTotalMinThrh(base);

	staticThrhCfgPtr->staticThrEn = qdmaIsChannelStaticThrEn(base, staticThrhCfgPtr->chnlIdx);

	for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
		staticThrhRatioPtr->queueThrRatio[index] = qdmaGetTxqStaticQueueThrhRatio(base, index);
	}
#else
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0 ;
	QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr = qdma_data->qdma_private.staticCfgPtr->totalThrhPtr;
	QDMA_TxQStaticDeiCfg_T *deiThrhPtr = qdma_data->qdma_private.staticCfgPtr->deiThrhPtr;
	QDMA_TxQStaticNormalCfg_T *normThrhPtr = qdma_data->qdma_private.staticCfgPtr->normThrhPtr;
	
	totalThrhPtr->dynCngstTotalMaxThrh = qdmaGetTxqDynTotalMaxThrh(base)>>2 ;
	totalThrhPtr->dynCngstTotalMinThrh = qdmaGetTxqDynTotalMinThrh(base)>>2 ;

	for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
		deiThrhPtr->deiThrh[index] = qdmaGetTxqStaticQueueDeiThrh(base, index)>>2 ;
	}
	for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
		normThrhPtr->normalThrh[index] = qdmaGetTxqStaticQueueNormalThrh(base, index)>>2 ;
	}
#endif

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ static channel enable/disable.
 Input Args:	arg1: channel index.
                    arg2: enable/disable.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_static_channel_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQStaticCfg_T *staticThrhCfgPtr = qdma_data->qdma_private.staticThrhCfgPtr;

	if(staticThrhCfgPtr->chnlIdx >= CONFIG_QDMA_CHANNEL){
		QDMA_ERR("Fault: channel index shoule between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}

	if(staticThrhCfgPtr->staticThrEn == QDMA_ENABLE){
		qdmaEnableChannelStaticThrEn(base, staticThrhCfgPtr->chnlIdx);
	}else{
		qdmaDisableChannelStaticThrEn(base, staticThrhCfgPtr->chnlIdx);
	}
#endif

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set txQ static queue ratio.
 Input Args:	arg1: per queue ratio.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_static_queue_ratio(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0 ;
	QDMA_TxQStaticThrRatio_T *staticThrhRatioPtr = qdma_data->qdma_private.staticThrhRatioPtr;

	for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
		qdmaSetTxqStaticQueueThrhRatio(base, index, staticThrhRatioPtr->queueThrRatio[index]);
	}
#endif

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get txQ dynamic cngst infomation.
 Input Args:	No input arguments
 Ret Value:	return txQ dynamic cngst infomation.
******************************************************************************/
int qdma_get_txq_cngst_dynamic_info(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr = qdma_data->qdma_private.dynCfgPtr->totalThrhPtr;
	QDMA_TxQDynCngstChnlThrh_T *chnlThrhPtr = qdma_data->qdma_private.dynCfgPtr->chnlThrhPtr;
	QDMA_TxQDynCngstQueueThrh_T *queueThrhPtr = qdma_data->qdma_private.dynCfgPtr->queueThrhPtr;
	QDMA_PeekRateCfg_t *peekrateCfgPtr = qdma_data->qdma_private.dynCfgPtr->peekrateCfgPtr;
	
	totalThrhPtr->dynCngstTotalMaxThrh = qdmaGetTxqDynTotalMaxThrh(base)>>2 ;
	totalThrhPtr->dynCngstTotalMinThrh = qdmaGetTxqDynTotalMinThrh(base)>>2 ;
	
	chnlThrhPtr->dynCngstChnlMaxThrh = qdmaGetTxqDynChnlMaxThrh(base)>>2 ;
	chnlThrhPtr->dynCngstChnlMinThrh = qdmaGetTxqDynChnlMinThrh(base)>>2 ;
	
	queueThrhPtr->dynCngstQueueMaxThrh = qdmaGetTxqDynQueueMaxThrh(base)>>2 ;
	queueThrhPtr->dynCngstQueueMinThrh = qdmaGetTxqDynQueueMinThrh(base)>>2 ;
	
	if(qdmaIsTxRateMeterPeakRateEnable(base))
		peekrateCfgPtr->peekRateEn = QDMA_ENABLE ;
	else
		peekrateCfgPtr->peekRateEn = QDMA_DISABLE ;

	peekrateCfgPtr->peekRateMargin = qdmaGetTxPeekRateMargin(base) ;
	peekrateCfgPtr->peekRateDuration = qdmaGetTxPeekRateDuration(base)<<4 ;
	
	return 0;
}


/******************************************************************************
 Descriptor:	It's used to set the value of TXQ configuration. It includes
 			txqDropEn, txqDeiDropEn, dynCngstEn, MaxThrhTx1En, MinThrhTx1En, 
 			MaxThrhTx0En, MinThrhTx0En, modeConfigTriggerEn, modePacketTriggerEn, 
 			modeTimeTriggerEn, deiThrhScale, dynCngstTicksel
 Input Args:	arg1: The pointer of the  TXQ configuration struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_cngst_cfg(QDMA_TxQCngstCfg_T *txqCngstCfgPtr)
{
	int txRingIdx = 0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(txqCngstCfgPtr->dynCngstDeiThrhScale<0 || txqCngstCfgPtr->dynCngstDeiThrhScale>=QDMA_DYNCNGST_DEI_THRH_SCALE_ITEMS) {
		QDMA_ERR("Fault: Tx Dynamic Congestion DEI Threshold Scale shoule between 0 and %d\n", QDMA_DYNCNGST_DEI_THRH_SCALE_ITEMS-1) ; 
		return -EINVAL ;
	}

	if(txqCngstCfgPtr->cngstDropEn == QDMA_DISABLE) {
		qdmaDisableTxqCngstDrop(base) ;
	} else {
		qdmaEnableTxqCngstDrop(base) ;
	}	
	if(txqCngstCfgPtr->cngstDeiDropEn == QDMA_DISABLE) {
		qdmaDisableTxqCngstDeiDrop(base) ;
	} else {
		qdmaEnableTxqCngstDeiDrop(base) ;
	}	
	if(txqCngstCfgPtr->dynCngstEn == QDMA_DISABLE) {
		qdmaDisableTxqDynCngstEn(base) ;
	} else {
		qdmaEnableTxqDynCngstEn(base) ;
	}
#if SUPPORT_TXQ_WRED
	if(txqCngstCfgPtr->cngstWredEn == QDMA_DISABLE) {
		qdmaDisableTxqCngstWredEn(base) ;
	} else {
		qdmaEnableTxqCngstWredEn(base) ;
	}
#endif

	for( txRingIdx = 0 ; txRingIdx < TX_RING_NUM ; txRingIdx++)
	{
		if(txqCngstCfgPtr->dynCngstMaxThrhTxEn[txRingIdx] == QDMA_DISABLE) {
			qdmaDisableCngstMaxThrhEn(base,txRingIdx) ;
		}else{
			qdmaEnableCngstMaxThrhEn(base,txRingIdx) ;
		}
		if(txqCngstCfgPtr->dynCngstMinThrhTxEn[txRingIdx] == QDMA_DISABLE) {
			qdmaDisableCngstMinThrhEn(base,txRingIdx) ;
		}else{
			qdmaEnableCngstMinThrhEn(base,txRingIdx) ;
		}
	}

	if(txqCngstCfgPtr->dynCngstModeConfigTrigEn == QDMA_DISABLE) {
		qdmaDisableCngstModeConfigTrig(base) ;
	} else {
		qdmaEnableCngstModeConfigTrig(base) ;
	}
	if(txqCngstCfgPtr->dynCngstModePacketTrigEn == QDMA_DISABLE) {
		qdmaDisableCngstModePacketTrig(base) ;
	} else {
		qdmaEnableCngstModePacketTrig(base) ;
	}
	if(txqCngstCfgPtr->dynCngstModeTimeTrigEn == QDMA_DISABLE) {
		qdmaDisableCngstModeTimeTrig(base) ;
	} else {
		qdmaEnableCngstModeTimeTrig(base) ;
	}

	qdmaSetDynCngstDeiThrhScale(base, txqCngstCfgPtr->dynCngstDeiThrhScale) ;
	qdmaSetTxqDynCngstTicksel(base, txqCngstCfgPtr->dynCngstTicksel) ;
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get the value of TXQ configuration. It includes
 			txqDropEn, txqDeiDropEn, dynCngstEn, MaxThrhTx1En, MinThrhTx1En, 
 			MaxThrhTx0En, MinThrhTx0En, modeConfigTriggerEn, modePacketTriggerEn, 
 			modeTimeTriggerEn, deiThrhScale, dynCngstTicksel
 Input Args:	arg1: The pointer of the  TXQ configuration struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_txq_cngst_cfg(QDMA_TxQCngstCfg_T *txqCngstCfgPtr)
{
	int txRingIdx = 0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(qdmaIsTxqCngstDropEnable(base)) {
		txqCngstCfgPtr->cngstDropEn = QDMA_ENABLE ;
	} else {
		txqCngstCfgPtr->cngstDropEn = QDMA_DISABLE ;
	}
	if(qdmaIsTxqCngstDeiDropEnable(base)) {
		txqCngstCfgPtr->cngstDeiDropEn = QDMA_ENABLE ;
	} else {
		txqCngstCfgPtr->cngstDeiDropEn = QDMA_DISABLE ;
	}
	if(qdmaIsTxqDynCngstEnable(base)) {
		txqCngstCfgPtr->dynCngstEn = QDMA_ENABLE ;
	} else {
		txqCngstCfgPtr->dynCngstEn = QDMA_DISABLE ;
	}
#if SUPPORT_TXQ_WRED
	if(qdmaIsTxqCngstWredEnable(base)) {
		txqCngstCfgPtr->cngstWredEn = QDMA_ENABLE ;
	} else {
		txqCngstCfgPtr->cngstWredEn = QDMA_DISABLE ;
	}
#endif

	for( txRingIdx = 0 ; txRingIdx < TX_RING_NUM ; txRingIdx++)
	{
		if(qdmaIsCngstMaxThrhEnable(base,txRingIdx)) {
			txqCngstCfgPtr->dynCngstMaxThrhTxEn[txRingIdx] = QDMA_ENABLE ;
		} else {
			txqCngstCfgPtr->dynCngstMaxThrhTxEn[txRingIdx] = QDMA_DISABLE ;
		}
		if(qdmaIsCngstMinThrhEnable(base,txRingIdx)) {
			txqCngstCfgPtr->dynCngstMinThrhTxEn[txRingIdx] = QDMA_ENABLE ;
		} else {
			txqCngstCfgPtr->dynCngstMinThrhTxEn[txRingIdx] = QDMA_DISABLE ;
		}
	}

	if(qdmaIsCngstModeConfigTrigEnable(base)) {
		txqCngstCfgPtr->dynCngstModeConfigTrigEn = QDMA_ENABLE ;
	} else {
		txqCngstCfgPtr->dynCngstModeConfigTrigEn = QDMA_DISABLE ;
	}
	if(qdmaIsCngstModePacketgTrigEnable(base)) {
		txqCngstCfgPtr->dynCngstModePacketTrigEn = QDMA_ENABLE ;
	} else {
		txqCngstCfgPtr->dynCngstModePacketTrigEn = QDMA_DISABLE ;
	}
	if(qdmaIsCngstModeTimeTrigEnable(base)) {
		txqCngstCfgPtr->dynCngstModeTimeTrigEn = QDMA_ENABLE ;
	} else {
		txqCngstCfgPtr->dynCngstModeTimeTrigEn = QDMA_DISABLE ;
	}

    #if SUPPORT_TXQ_CNGST_DYN_DEI_THRH_SCALE
	txqCngstCfgPtr->dynCngstDeiThrhScale = qdmaGetDynCngstDeiThrhScale(base) ;
    #endif
    
	txqCngstCfgPtr->dynCngstTicksel = qdmaGetTxqDynCngstTicksel(base) ;
	
	return 0;
}


#if 0
/******************************************************************************
 Descriptor:	It's used to set the value of tx static threshold. It includes
 			dei packet threshold and Normal packet threshold.
 Input Args:	arg1: The pointer of the TX static threshold struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_txq_static_queue_threshold(QDMA_TxQStaticCngstThrh_T *txqCngstThrhPtr)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	
	if(txqCngstThrhPtr->queueIdx<0 || txqCngstThrhPtr->queueIdx>=CONFIG_QDMA_QUEUE) {
		QDMA_ERR("Fault: queue index shoule between 0 and %d\n", CONFIG_QDMA_QUEUE-1) ; 
		return -EINVAL ;
	}
	
	if(txqCngstThrhPtr->staticDeiThrh > txqCngstThrhPtr->staticNormalThrh) {
		QDMA_ERR("Fault: static DEI packet threshold should less than static Normal packet threshold.\n") ; 
		return -EINVAL ;
	}
	
	qdmaSetTxqStaticQueueDeiThrh(base, txqCngstThrhPtr->queueIdx, txqCngstThrhPtr->staticDeiThrh) ;
	qdmaSetTxqStaticQueueNormalThrh(base, txqCngstThrhPtr->queueIdx, txqCngstThrhPtr->staticNormalThrh) ;
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get the value of tx static threshold. It includes
 			dei packet threshold and Normal packet threshold.
 Input Args:	arg1: The pointer of the TX static threshold struct.
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_get_txq_static_queue_threshold(QDMA_TxQStaticCngstThrh_T *txqCngstThrhPtr) 
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	
	if(txqCngstThrhPtr->queueIdx<0 || txqCngstThrhPtr->queueIdx>=CONFIG_QDMA_QUEUE) {
		QDMA_ERR("Fault: queue index shoule between 0 and %d\n", CONFIG_QDMA_QUEUE-1) ; 
		return -EINVAL ;
	}
	
	txqCngstThrhPtr->staticDeiThrh = qdmaGetTxqStaticQueueDeiThrh(base, txqCngstThrhPtr->queueIdx) ;
	txqCngstThrhPtr->staticNormalThrh = qdmaGetTxqStaticQueueNormalThrh(base, txqCngstThrhPtr->queueIdx) ;
	
	return 0 ;
}
#endif


int qdma_set_tx_cngst_mode(struct ECNT_QDMA_Data *qdma_data)
{
    uint ringIdx=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQDynCngstThrh_T txqDynCngstThrh;
	QDMA_txCngstCfg_t *pTxCngstCfg = qdma_data->qdma_private.pTxCngstCfg;

	/********************************************
	* Setting tx dynamic cngst register               *
	*********************************************/
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		qdmaDisableCngstMaxThrhEn(base, ringIdx) ;
		qdmaDisableCngstMinThrhEn(base, ringIdx) ;
	}

	qdmaEnableCngstModeConfigTrig(base);
	qdmaEnableCngstModePacketTrig(base);
	qdmaEnableCngstModeTimeTrig(base);

	qdmaEnableTxRateMeter(base);

#if 0/*set threshold in qdma_set_txq_cngst_auto_config()*/
	if(gpQdmaPriv->hwFwdDscpNum <= 256) {
		txqDynCngstThrh.dynCngstTotalMinThrh = 48;
		txqDynCngstThrh.dynCngstChnlMinThrh = 2;
		txqDynCngstThrh.dynCngstQueueMinThrh = 2;
	} else if( gpQdmaPriv->hwFwdDscpNum <= gpQdmaPriv->hwFwdDscpMaxNum ){
#if defined(TCSUPPORT_CPU_EN7580)
#if defined(QDMA_LAN)
        txqDynCngstThrh.dynCngstTotalMinThrh = 0x0100;
		txqDynCngstThrh.dynCngstChnlMinThrh = 0x0200;
#else
		txqDynCngstThrh.dynCngstTotalMinThrh = 0x0333;
		txqDynCngstThrh.dynCngstChnlMinThrh = 0x0124;
#endif
		txqDynCngstThrh.dynCngstQueueMinThrh = 0x0018;
#else
		txqDynCngstThrh.dynCngstTotalMinThrh = gpQdmaPriv->hwFwdDscpNum/10; 
		txqDynCngstThrh.dynCngstChnlMinThrh = gpQdmaPriv->hwFwdDscpNum/28; 
		txqDynCngstThrh.dynCngstQueueMinThrh = gpQdmaPriv->hwFwdDscpNum/340; 
#endif
	} else {
		printk("Error: CONFIG_HWFWD_DSCP_NUM should less than %d\n",gpQdmaPriv->hwFwdDscpMaxNum);
		return -1;
	}

#if defined(TCSUPPORT_CPU_EN7580)
	txqDynCngstThrh.dynCngstTotalMaxThrh = 0x3c00;
	txqDynCngstThrh.dynCngstChnlMaxThrh = 0x3c00;
	txqDynCngstThrh.dynCngstQueueMaxThrh = 0x0333;
	txqDynCngstThrh.dynCngstMinDscpThrh = 0x4 * (qdmaGetHwPayloadSize(base)+1);
#else
	txqDynCngstThrh.dynCngstTotalMaxThrh = 0x3c00;//(physical_size - (physical_size>>4) )>>8;	/* (physical_size*15/16)/256Byte */
	txqDynCngstThrh.dynCngstChnlMaxThrh = 0x3c00;
	txqDynCngstThrh.dynCngstQueueMaxThrh = 0x0333;
#endif
#endif

	if(pTxCngstCfg->txCngstMode == QDMA_TXCNGST_STATIC) {
		qdmaDisableTxqDynCngstEn(base);
		qdmaDisableTxRateMeterPeakRate(base);
#if SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
		qdmaDisableTxRateMeter(base);
#endif
		/*  open api for mac setting */
	} else if(pTxCngstCfg->txCngstMode == QDMA_TXCNGST_DYNAMIC_NORMAL) {
		qdmaEnableTxqDynCngstEn(base);
		qdmaDisableTxRateMeterPeakRate(base);
		qdma_data->qdma_private.dynCngstThrhPtr = &txqDynCngstThrh;
		qdma_set_txq_cngst_dynamic_threshold(qdma_data);
	} else if(pTxCngstCfg->txCngstMode == QDMA_TXCNGST_DYNAMIC_PEAKRATE_MARGIN){
		qdmaEnableTxqDynCngstEn(base);
		qdmaEnableTxRateMeterPeakRate(base);
		qdmaSetTxPeekRateDuration(base, pTxCngstCfg->peekRateDuration>>4);
		qdmaSetTxPeekRateMargin(base, pTxCngstCfg->peekRateMargin);
		qdma_data->qdma_private.dynCngstThrhPtr = &txqDynCngstThrh;
		qdma_set_txq_cngst_dynamic_threshold(qdma_data);	/*  notice that peekrate mode should set different threshold */
	}

	return 0;
}

int qdma_get_tx_cngst_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_txCngstCfg_t *pTxCngstCfg = qdma_data->qdma_private.pTxCngstCfg;
	
	if(qdmaIsTxqDynCngstEnable(base)) {
		if(qdmaIsTxRateMeterPeakRateEnable(base)) {
			pTxCngstCfg->txCngstMode = QDMA_TXCNGST_DYNAMIC_PEAKRATE_MARGIN;
			pTxCngstCfg->peekRateDuration = qdmaGetTxPeekRateDuration(base);
			pTxCngstCfg->peekRateMargin = qdmaGetTxPeekRateMargin(base);
		} else {
			pTxCngstCfg->txCngstMode = QDMA_TXCNGST_DYNAMIC_NORMAL;
		}
	} else {
		pTxCngstCfg->txCngstMode = QDMA_TXCNGST_STATIC;
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set virtual channel mode.
 Input Args:	arg1: set virtual channel mode, it includes
 			- virChnlEn: virtual channel mapping mode enable or disable.
 			- virChnlMode: 16 Queue or 32 Queue.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_virtual_channel_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_VirtualChannelMode_T *virChnlModePtr = qdma_data->qdma_private.virChnlModePtr;

	if(virChnlModePtr->virChnlEn == QDMA_ENABLE) {
		qdmaEnableVChnlMapEn(base) ;
	} else {
		qdmaDisableVChnlMapEn(base) ;
	}

	if(virChnlModePtr->virChnlMode == QDMA_VIRTUAL_CHANNEL_32Queue) {
		qdmaSetVChnlMap32Queue(base) ;
	} else {
		qdmaSetVChnlMap16Queue(base) ;
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to get virtual channel mode.
 Input Args:	No input arguments
 Ret Value:	return virtual channel mode.
******************************************************************************/
int qdma_get_virtual_channel_mode(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_VirtualChannelMode_T *virChnlModePtr = qdma_data->qdma_private.virChnlModePtr;

	if(qdmaIsVChnlMapEnable(base)) {
		virChnlModePtr->virChnlEn = QDMA_ENABLE ;
	} else { 
		virChnlModePtr->virChnlEn = QDMA_DISABLE ;
	}

	if(qdmaIsVChnlMap32Queue(base)) {
		virChnlModePtr->virChnlMode = QDMA_VIRTUAL_CHANNEL_32Queue ;
	} else { 
		virChnlModePtr->virChnlMode = QDMA_VIRTUAL_CHANNEL_16Queue ;
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set single physical channel qos value.
 Input Args:	arg1: set single physical channel qos value, it includes
 			- phyChnl: specific physical channel index, scope is (0~7) or (0~15).
 			- qosType: means SP / WRR / SPWRR3 / SPWRR2.
 			- queue[4]: means weight of 2 or 4 queue.
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_set_virtual_channel_qos(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	unchar weight[CONFIG_QDMA_QUEUE] ;
	int virChnl = 0, phyChnlMax=0, virChnlMax=0;
	QDMA_VirtualChannelQoS_T *virChnlQoSPtr = qdma_data->qdma_private.virChnlQoSPtr;

	if(qdmaIsVChnlMapEnable(base) == QDMA_DISABLE) {
		QDMA_ERR("virtual channel mapping mode is disable now.\n") ;
		return -EINVAL;
	}
	
	/* get 16queue or 32queue */
	if(qdmaIsVChnlMap32Queue(base))
		virChnlMax = 4 ;
	else 
		virChnlMax = 2 ;
	phyChnlMax = CONFIG_QDMA_CHANNEL / virChnlMax ;

	if((virChnlQoSPtr->phyChnl >= phyChnlMax) || (virChnlQoSPtr->qosType >= virChnlMax)) {		
		QDMA_ERR("physical channel:%d, should less than %d; mode:%d, should less than %d.\n", virChnlQoSPtr->phyChnl, phyChnlMax, virChnlQoSPtr->qosType, virChnlMax) ;
		return -EINVAL;
	}
	
	for(virChnl=0 ; virChnl<virChnlMax ; virChnl++) {
		if((virChnlQoSPtr->queue[virChnl].weight>100) && (virChnlQoSPtr->queue[virChnl].weight!=255)) {
			return -EINVAL ;
		}	
		weight[virChnl] = virChnlQoSPtr->queue[virChnl].weight ;
	}
	
	return qdmaSetVirtualChannelQos(virChnlQoSPtr->phyChnl, virChnlMax, (unchar)virChnlQoSPtr->qosType, weight) ;
}

/******************************************************************************
 Descriptor:	It's used to get single physical channel qos value.
 Input Args:	No input arguments
 Ret Value:	return single physical channel qos value.
******************************************************************************/
int qdma_get_virtual_channel_qos(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_VirtualChannelQoS_T *virChnlQoSPtr = qdma_data->qdma_private.virChnlQoSPtr ;
	int ret = 0, virChnl = 0, phyChnlMax = 0, virChnlMax = 0 ;
	unchar qosType = 0, weight[CONFIG_QDMA_QUEUE] ;	

	if(qdmaIsVChnlMapEnable(base) == QDMA_DISABLE) {
		QDMA_ERR("virtual channel mapping mode is disable now.\n") ;
		return -EINVAL;
	}
	
	/* get 16queue or 32queue */
	if(qdmaIsVChnlMap32Queue(base))
		virChnlMax = 4 ;
	else 
		virChnlMax = 2 ;
	phyChnlMax = CONFIG_QDMA_CHANNEL / virChnlMax ;
	
	if(virChnlQoSPtr->phyChnl >= phyChnlMax) {		
		QDMA_ERR("physical channel:%d, should less than %d.\n", virChnlQoSPtr->phyChnl, phyChnlMax) ;
		return -EINVAL;
	}

	ret = qdmaGetVirtualChannelQos(virChnlQoSPtr->phyChnl, virChnlMax, &qosType, weight) ;
	if(ret < 0) {
		return -EFAULT ;
	}
	
	virChnlQoSPtr->qosType = qosType ;
	
	for(virChnl=0 ; virChnl<virChnlMax ; virChnl++) {
		virChnlQoSPtr->queue[virChnl].weight = weight[virChnl] ;
	}

	return 0 ;
}

int qdma_set_flow_cntr_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_MTR_ACNT_IN_QDMA
	uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_FlowCntIncDropCfg_T *flowCntIncDropCfg = qdma_data->qdma_private.flowCntIncDropCfg ;

    if( flowCntIncDropCfg->grpIdx > (CONFIG_FLOWCNT_MAX_DROUP_NUM-1) )
    {
        QDMA_ERR("Fault: group index should between 0 and %d\n", CONFIG_FLOWCNT_MAX_DROUP_NUM-1 ) ; 
        return -EINVAL ;
    }
	
	if( flowCntIncDropCfg->enable > 0 )
    {   
        qdmaEnableFlowCntIncDrop(base, flowCntIncDropCfg->grpIdx);
    }
    else
    {
        qdmaDisableFlowCntIncDrop(base, flowCntIncDropCfg->grpIdx);
    }
#endif
	
	return 0;
}

int qdma_get_flow_cntr_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_MTR_ACNT_IN_QDMA
	uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_FlowCntIncDropCfg_T *flowCntIncDropCfg = qdma_data->qdma_private.flowCntIncDropCfg ;

    if( flowCntIncDropCfg->grpIdx > (CONFIG_FLOWCNT_MAX_DROUP_NUM-1) )
    {
        QDMA_ERR("Fault: group index should between 0 and %d\n", CONFIG_FLOWCNT_MAX_DROUP_NUM-1 ) ; 
        return -EINVAL ;
    }
	
	if( qdmaIsFlowCntIncDropEnable(base, flowCntIncDropCfg->grpIdx) > 0 )
        flowCntIncDropCfg->enable = QDMA_ENABLE ;
    else
        flowCntIncDropCfg->enable = QDMA_DISABLE ;
#endif
	
	return 0;
}

int qdma_get_flow_cntr_value(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_MTR_ACNT_IN_QDMA
	uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_FlowCntValueGet_T *flowCntValueGet = qdma_data->qdma_private.flowCntValueGet ;

    if( flowCntValueGet->grpIdx == 0 ){
        if( flowCntValueGet->cntIdx > (CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM-1) ){
            QDMA_MSG(DBG_WARN, "Fault: group_0 cnt_index should between 0 and %d\n", CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM-1 ) ; 
            return -EINVAL ;
        }
    }else if( flowCntValueGet->grpIdx == 1 ){
        if( flowCntValueGet->cntIdx > (CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM-1) ){
            QDMA_MSG(DBG_WARN, "Fault: group_1 cnt_index should between 0 and %d\n", CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM-1 ) ; 
            return -EINVAL ;
        }
    }
#if SUPPORT_ACNT_GRP2
    else if( flowCntValueGet->grpIdx == 2 ){
        if( flowCntValueGet->cntIdx > (CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM-1) ){
            QDMA_MSG(DBG_WARN, "Fault: group_2 cnt_index should between 0 and %d\n", CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM-1 ) ; 
            return -EINVAL ;
        }
    }
#endif
    else{
        QDMA_ERR("Fault: group index should between 0 and %d\n", CONFIG_FLOWCNT_MAX_DROUP_NUM-1 ) ; 
        return -EINVAL ;
    }
	
	flowCntValueGet->byteLow = qdmaGetFlowCntByteLow(base, flowCntValueGet->grpIdx, flowCntValueGet->cntIdx);
    flowCntValueGet->byteHigh = qdmaGetFlowCntByteHigh(base, flowCntValueGet->grpIdx, flowCntValueGet->cntIdx);
    flowCntValueGet->pktCnt = qdmaGetFlowCntPkt(base, flowCntValueGet->grpIdx, flowCntValueGet->cntIdx);
#if SUPPORT_FLOWCNT_PKT_HI
    flowCntValueGet->pktHigh = qdmaGetFlowCntPktHigh(base, flowCntValueGet->grpIdx, flowCntValueGet->cntIdx);
#endif
#endif
	
	return 0;
}

int qdma_clear_flow_cntr_value(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_MTR_ACNT_IN_QDMA
	uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_FlowCntValueClear_T *flowCntValueClear = qdma_data->qdma_private.flowCntValueClear ;
    unchar cntIdx=0 ;
    unchar cntIdxMax=0 ;

    if( flowCntValueClear->grpIdx == 0 ){
        if( flowCntValueClear->cntIdx > CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM ){
            QDMA_ERR("Fault: group_0 cnt_index should between 0 and %d\n", CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM) ; 
            return -EINVAL ;
        }
        cntIdxMax = CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM;
    }else if( flowCntValueClear->grpIdx == 1 ){
        if( flowCntValueClear->cntIdx > CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM ){
            QDMA_ERR("Fault: group_1 cnt_index should between 0 and %d\n", CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM) ; 
            return -EINVAL ;
        }
        cntIdxMax = CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM;
    }
#if SUPPORT_ACNT_GRP2
    else if( flowCntValueClear->grpIdx == 2 ){
        if( flowCntValueClear->cntIdx > CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM ){
            QDMA_ERR("Fault: group_2 cnt_index should between 0 and %d\n", CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM ) ; 
            return -EINVAL ;
        }
		cntIdxMax = CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM;
    }
#endif
    else{
        QDMA_ERR("Fault: group index should between 0 and %d\n", CONFIG_FLOWCNT_MAX_DROUP_NUM-1 ) ; 
        return -EINVAL ;
    }

    for(cntIdx=0; cntIdx<cntIdxMax; cntIdx++)
    {
        if((cntIdx!=flowCntValueClear->cntIdx) && (cntIdxMax!=flowCntValueClear->cntIdx))
            continue ;
        
        switch(flowCntValueClear->clearMode){
            case FLOWCNT_CLEAR_BYTE:
                qdmaClearFlowCntByte(base, flowCntValueClear->grpIdx, cntIdx);
                break ;
            case FLOWCNT_CLEAR_PKT:
                qdmaClearFlowCntPkt(base, flowCntValueClear->grpIdx, cntIdx);
                break ;
            case FLOWCNT_CLEAR_ALL:
                qdmaClearFlowCntByte(base, flowCntValueClear->grpIdx, cntIdx);
                qdmaClearFlowCntPkt(base, flowCntValueClear->grpIdx, cntIdx);
                break ;
            default:
                QDMA_ERR("Fault: clear mode invalid\n") ; 
                return -EINVAL ;
        }
    }
#endif
	
	return 0;
}

int qdma_set_channel_close_status(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_CHANNEL_CLOSE_STATUS
    uint chnl_idx= 0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_ChannelStatus_T *chnlCloseStatusSet = qdma_data->qdma_private.chnlCloseStatusSet ;

    if( chnlCloseStatusSet->chnlIdx < CONFIG_QDMA_CHANNEL ){
        qdmaSetChannelCfg_sw(base,chnlCloseStatusSet->chnlIdx,chnlCloseStatusSet->chnlStatus);
    }else if(chnlCloseStatusSet->chnlIdx == CONFIG_QDMA_CHANNEL){
        for(chnl_idx=0; chnl_idx<CONFIG_QDMA_CHANNEL; chnl_idx++){
            qdmaSetChannelCfg_sw(base,chnl_idx,chnlCloseStatusSet->chnlStatus);
        }
    }else{
        QDMA_ERR("Fault: input channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL ) ; 
        return -EINVAL ;
    }
#endif
    
    return 0 ;
}

int qdma_get_channel_close_status(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_CHANNEL_CLOSE_STATUS
	uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_ChannelStatus_T *chnlCloseStatusGet = qdma_data->qdma_private.chnlCloseStatusGet ;

    if( chnlCloseStatusGet->chnlIdx > (CONFIG_QDMA_CHANNEL-1) ){
        QDMA_ERR("Fault: input channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL-1 ) ; 
        return -EINVAL ;
    }

    chnlCloseStatusGet->chnlStatus = qdmaIsChannelClosed_sw(base,chnlCloseStatusGet->chnlIdx);
#endif

    return 0 ;
}

int qdma_get_channel_empty_status(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_CHANNEL_CLOSE_STATUS
	uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_ChannelStatus_T *chnlEmptyStatusGet = qdma_data->qdma_private.chnlEmptyStatusGet ;

    if( chnlEmptyStatusGet->chnlIdx > (CONFIG_QDMA_CHANNEL-1) ){
        QDMA_ERR("Fault: input channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL-1 ) ; 
        return -EINVAL ;
    }

    chnlEmptyStatusGet->chnlStatus = qdmaIsChannelEmpty(base,chnlEmptyStatusGet->chnlIdx);
#endif

    return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to enable/disable QDMA_OAM_MODIFY_FP.
 Input Args:	arg1: Enable/disable QDMA_OAM_MODIFY_FP (QDMA_ENABLE/QDMA_DISABLE)
 Ret Value:     0: successful, otherwise failed.
******************************************************************************/
int qdma_set_oam_modify_fp_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_OAM_MODIFY_FP
    uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_Mode_t oamFpMode = qdma_data->qdma_private.mode;
    
    if(oamFpMode == QDMA_ENABLE) {
#ifdef QDMA_LAN
		if(qdmaLanHqosMode == 0){
			qdmaEnableOamModifyFpEn(base) ;
		}
#else
		if(qdmaWanHqosMode == 0){
			qdmaEnableOamModifyFpEn(base) ;
		}
#endif
    } else {
        qdmaDisableOamModifyFpEn(base) ;
    }
#endif

    return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get QDMA_OAM_MODIFY_FP info.
 Input Args:	NULL.
 Ret Value:     1: enable , otherwise failed.
******************************************************************************/
int qdma_get_oam_modify_fp_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_OAM_MODIFY_FP
    uint base = gpQdmaPriv->csrBaseAddr ;

    if(qdmaIsOamModifyFpEn(base)) {
        qdma_data->qdma_private.mode = QDMA_ENABLE ;
    } else {
        qdma_data->qdma_private.mode = QDMA_DISABLE ;
    }
    
    return qdma_data->qdma_private.mode ;
#else
    return 0;
#endif
}

/******************************************************************************
 Descriptor:	It's used to enable/disable MULTICAST.
 Input Args:	arg1: Enable/disable QDMA_MULTICAST_EN (QDMA_ENABLE/QDMA_DISABLE)
 Ret Value:     0: successful, otherwise failed.
******************************************************************************/
int qdma_set_multicast_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QDMA_MULTICAST
    uint base = gpQdmaPriv->csrBaseAddr ;
    QDMA_Mode_t multicastMode = qdma_data->qdma_private.mode;
    
    if(multicastMode == QDMA_ENABLE) {
        qdmaEnableMulticastModifyFpEn(base) ;
		qdmaEnableMulticast(base) ;
    } else {
        qdmaDisableMulticastModifyFpEn(base) ;
		qdmaDisableMulticast(base) ;
    }
#endif

    return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to get QDMA_MULTICAST_EN info.
 Input Args:	NULL.
 Ret Value:     1: enable , otherwise failed.
******************************************************************************/
int qdma_get_multicast_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QDMA_MULTICAST
    uint base = gpQdmaPriv->csrBaseAddr ;

    if(qdmaIsMulticastEn(base)) {
        qdma_data->qdma_private.mode = QDMA_ENABLE ;
    } else {
        qdma_data->qdma_private.mode = QDMA_DISABLE ;
    }
    
    return qdma_data->qdma_private.mode ;
#else
    return 0;
#endif
}

/******************************************************************************
 Descriptor:	It's used to set QDMA_MULTICAST_MODIFY_SPTAG_CFG.
 Input Args:	QDMA_MulticastSptagCfg_T
 Ret Value:     0: successful, otherwise failed.
******************************************************************************/
int qdma_set_multicast_sptag_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QDMA_MULTICAST
    uint base = gpQdmaPriv->csrBaseAddr;
    QDMA_MulticastSptagCfg_T *multicastSptagCfg = qdma_data->qdma_private.multicastSptagCfgPtr;

#if defined(QDMA_LAN)
	if( multicastSptagCfg->chnlIdx > (CONFIG_QDMA_CHANNEL-1) ){
		QDMA_ERR("Fault: index should between 0 and %d\n", (CONFIG_QDMA_CHANNEL-1) ) ; 
		return -EINVAL ;
	}

	if( multicastSptagCfg->sptagKeepHiEn > 0 ){
		qdmaEnableMulticastSptagKeepHi(base, multicastSptagCfg->chnlIdx);
	}else{
		qdmaDisableMulticastSptagKeepHi(base, multicastSptagCfg->chnlIdx);
	}
	
	qdmaSetMulticastSptag(base, multicastSptagCfg->chnlIdx, multicastSptagCfg->sptag);
#endif	
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to get QDMA_MULTICAST_MODIFY_SPTAG_CFG info.
 Input Args:	QDMA_MulticastSptagCfg_T.
 Ret Value:    0: successful, otherwise failed.
******************************************************************************/
int qdma_get_multicast_sptag_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QDMA_MULTICAST
	uint base = gpQdmaPriv->csrBaseAddr;
	QDMA_MulticastSptagCfg_T *multicastSptagCfg = qdma_data->qdma_private.multicastSptagCfgPtr;

#if defined(QDMA_LAN)
	if( multicastSptagCfg->chnlIdx > (CONFIG_QDMA_CHANNEL-1) ){
		QDMA_ERR("Fault: index should between 0 and %d\n", (CONFIG_QDMA_CHANNEL-1) ) ; 
		return -EINVAL ;
	}

	multicastSptagCfg->sptagKeepHiEn = qdmaIsMulticastSptagKeepHiEn(base, multicastSptagCfg->chnlIdx) & 0x1;
	multicastSptagCfg->sptag = qdmaGetMulticastSptag(base, multicastSptagCfg->chnlIdx);
#endif
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA_MULTICAST_MODIFY_FPORT_CFG.
 Input Args:	QDMA_MulticastFportCfg_T
 Ret Value:     0: successful, otherwise failed.
******************************************************************************/
int qdma_set_multicast_fport_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QDMA_MULTICAST
    uint base = gpQdmaPriv->csrBaseAddr;
    QDMA_MulticastFportCfg_T *multicastFportCfg = qdma_data->qdma_private.multicastFportCfgPtr;

	if( multicastFportCfg->chnlIdx > (16-1) ){
		QDMA_ERR("Fault: index should between 0 and %d\n", (16-1) ) ; 
		return -EINVAL ;
	}
	qdmaSetMulticastFport(base, multicastFportCfg->chnlIdx, multicastFportCfg->fport);
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to get QDMA_MULTICAST_MODIFY_FPORT_CFG info.
 Input Args:	QDMA_MulticastFportCfg_T.
 Ret Value:    0: successful, otherwise failed.
******************************************************************************/
int qdma_get_multicast_fport_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QDMA_MULTICAST
    uint base = gpQdmaPriv->csrBaseAddr;
	QDMA_MulticastFportCfg_T *multicastFportCfg = qdma_data->qdma_private.multicastFportCfgPtr;

	if( multicastFportCfg->chnlIdx > (16-1) ){
		QDMA_ERR("Fault: index should between 0 and %d\n", (16-1) ) ; 
		return -EINVAL ;
	}
	multicastFportCfg->fport = qdmaGetMulticastFport(base, multicastFportCfg->chnlIdx);
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA_SLA_CHNL_CFG.
 Input Args:	arg1: SLA CFG INDEX(0~3)
 			arg2: Enable/disable QDMA_SLA_CHNL_CFG (QDMA_ENABLE/QDMA_DISABLE)
 			arg3: CHANNEL INDEX
 Ret Value:     0: successful, otherwise failed.
******************************************************************************/
int qdma_set_sla_chnl_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_SLA
    uint base = gpQdmaPriv->csrBaseAddr;
    QDMA_SlaChnlCfg_T *slaChnlCfg = qdma_data->qdma_private.slaChnlCfgPtr;

	if(slaChnlCfg->slaIdx >= SLA_MAX_NUM) {
		QDMA_ERR("Fault: SLA index should between 0 and %d\n", SLA_MAX_NUM-1) ; 
		return -EINVAL ;
	}
	
	if(slaChnlCfg->chnlIdx >= CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}

    if(slaChnlCfg->slaEn == QDMA_ENABLE) {
        qdmaSetSlaCtlEnable(base, slaChnlCfg->slaIdx);
    } else {
        qdmaSetSlaCtlDisable(base, slaChnlCfg->slaIdx);
    }

	qdmaSetSlaCtlChannelIdx(base, slaChnlCfg->slaIdx, slaChnlCfg->chnlIdx);
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to get QDMA_SLA_CHNL_CFG info.
 Input Args:	NULL.
 Ret Value:    0: successful, otherwise failed.
******************************************************************************/
int qdma_get_sla_chnl_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_SLA
    uint base = gpQdmaPriv->csrBaseAddr;
	QDMA_SlaChnlCfg_T *slaChnlCfg = qdma_data->qdma_private.slaChnlCfgPtr;

	if(slaChnlCfg->slaIdx >= SLA_MAX_NUM) {
		QDMA_ERR("Fault: SLA index should between 0 and %d\n", SLA_MAX_NUM-1) ; 
		return -EINVAL ;
	}

    if(qdmaIsSlaCtlEnable(base, slaChnlCfg->slaIdx)) {
        slaChnlCfg->slaEn = QDMA_ENABLE;
    } else {
        slaChnlCfg->slaEn = QDMA_DISABLE;
    }

	slaChnlCfg->chnlIdx = qdmaGetSlaCtlChannelIdx(base, slaChnlCfg->slaIdx);
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA_QOS_AGING_CFG.
 Input Args:	QDMA_QoSAgingCfg_T
 Ret Value:     0: successful, otherwise failed.
******************************************************************************/
int qdma_set_qos_aging_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QUEUE_AGING
    uint base = gpQdmaPriv->csrBaseAddr;
    QDMA_QoSAgingCfg_T *qosAgingCfg = qdma_data->qdma_private.qosAgingCfgPtr;

    if(qosAgingCfg->agingEn == QDMA_ENABLE) {
        qdmaEnableQosAgingEn(base);
    } else {
        qdmaDisableQosAgingEn(base);
    }

	qdmaSetQosAgingMethod(base, qosAgingCfg->agingMethod);

    if(qosAgingCfg->agingFastReplaceEn == QDMA_ENABLE) {
        qdmaEnableQosAgingFastReplaceEn(base);
    } else {
        qdmaDisableQosAgingFastReplaceEn(base);
    }

	qdmaSetQosAgingTime(base, qosAgingCfg->agingTime/20);
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to get QDMA_QOS_AGING_CFG info.
 Input Args:	NULL.
 Ret Value:    0: successful, otherwise failed.
******************************************************************************/
int qdma_get_qos_aging_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QUEUE_AGING
    uint base = gpQdmaPriv->csrBaseAddr;
	QDMA_QoSAgingCfg_T *qosAgingCfg = qdma_data->qdma_private.qosAgingCfgPtr;
	
	qosAgingCfg->agingEn = qdmaIsQosAgingEn(base);
	qosAgingCfg->agingMethod = qdmaGetQosAgingMethod(base);
	qosAgingCfg->agingFastReplaceEn = qdmaIsQosAgingFastReplaceEn(base);
	qosAgingCfg->agingTime = qdmaGetQosAgingTime(base) * 20;
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA_PER_QUEUE_AGING_EN_CFG.
 Input Args:	QDMA_TxQAgingCfg_T
 Ret Value:     0: successful, otherwise failed.
******************************************************************************/
int qdma_set_per_queue_aging_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QUEUE_AGING
    uint base = gpQdmaPriv->csrBaseAddr;
    QDMA_TxQAgingCfg_T *txqAgingCfg = qdma_data->qdma_private.txqAgingCfgPtr;

	if(txqAgingCfg->chnlIdx >= CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}
	
	if(txqAgingCfg->queueIdx >= CONFIG_QDMA_QUEUE) {
		QDMA_ERR("Fault: queue index should between 0 and %d\n", CONFIG_QDMA_QUEUE-1) ; 
		return -EINVAL ;
	}

    if(txqAgingCfg->queueAgingEn == QDMA_ENABLE) {
        qdmaEnablePerQueueAgingEn(base, txqAgingCfg->chnlIdx, txqAgingCfg->queueIdx);
    } else {
        qdmaDisablePerQueueAgingEn(base, txqAgingCfg->chnlIdx, txqAgingCfg->queueIdx);
    }
#endif

    return 0;
}

/******************************************************************************
 Descriptor:	It's used to get QDMA_PER_QUEUE_AGING_EN_CFG info.
 Input Args:	NULL.
 Ret Value:    0: successful, otherwise failed.
******************************************************************************/
int qdma_get_per_queue_aging_cfg(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_QUEUE_AGING
    uint base = gpQdmaPriv->csrBaseAddr;
    QDMA_TxQAgingCfg_T *txqAgingCfg = qdma_data->qdma_private.txqAgingCfgPtr;

	if(txqAgingCfg->chnlIdx >= CONFIG_QDMA_CHANNEL) {
		QDMA_ERR("Fault: channel index should between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}
	
	if(txqAgingCfg->queueIdx >= CONFIG_QDMA_QUEUE) {
		QDMA_ERR("Fault: queue index should between 0 and %d\n", CONFIG_QDMA_QUEUE-1) ; 
		return -EINVAL ;
	}

	txqAgingCfg->queueAgingEn = qdmaIsPerQueueAgingEn(base, txqAgingCfg->chnlIdx, txqAgingCfg->queueIdx);
#endif

    return 0;
}

int qdma_get_hqos_en(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_HQOS	
#ifdef QDMA_LAN
	if(qdmaLanHqosMode == 1){
		qdma_data->qdma_private.mode = QDMA_ENABLE;
	}else{
		qdma_data->qdma_private.mode = QDMA_DISABLE;
	}
#else
	if(qdmaWanHqosMode == 1){
		qdma_data->qdma_private.mode = QDMA_ENABLE;
	}else{
		qdma_data->qdma_private.mode = QDMA_DISABLE;
	}
#endif

	return qdma_data->qdma_private.mode;
#else
	 return 0;
#endif
}

int qdma_set_multicast_1toN_cfg(struct ECNT_QDMA_Data *qdma_data)
{
    uint base = gpQdmaPriv->csrBaseAddr;
	u32 i=0, port=0, nbq=0, channel=0, sptag=0, switch_port;

	for(i = 0; i < 3; i++)
	{
		/* set hsgmii multicast fport info
		*channel-10 => PSE port-3 nbq-4 => sp_tag-0x0000 => keep_sptag_hifield-0
		*channel-11 => PSE port-3 nbq-5 => sp_tag-0x0000 => keep_sptag_hifield-0
		*channel-12 => PSE port-3 nbq-6 => sp_tag-0x0000 => keep_sptag_hifield-0
		*/
		nbq = i + 4;
		channel = i + 10;
		port = 3;
		qdmaSetMulticastFport(base, channel, ((port<<5)|nbq));
	}

	if(ext_switch_sel == 0){
		nbq = 4;
	}else if(ext_switch_sel == 1){
		nbq = 5;
	}else if (ext_switch_sel == 2){
		nbq = 6;
	}else{
		printk("there not has external switch ext_switch_sel=%d\n", ext_switch_sel);
		return 0;
	}

	for(i = 0; i < 4; i++)
	{
		/* set hsgmii multicast fport & sptag info for external switch
		*channel-6 => PSE port-3 nbq-4/5/6 => sp_tag-0x0002 => keep_sptag_hifield-1
		*channel-7 => PSE port-3 nbq-4/5/6 => sp_tag-0x0004 => keep_sptag_hifield-1
		*channel-8 => PSE port-3 nbq-4/5/6 => sp_tag-0x0008 => keep_sptag_hifield-1
		*channel-9 => PSE port-3 nbq-4/5/6 => sp_tag-0x0010 => keep_sptag_hifield-1
		*/
		channel = i + 6;
		ETHER_API_GET_PORTMAP((i+5), &switch_port);
		sptag = 1 << switch_port;
		port = 3;

		qdmaEnableMulticastSptagKeepHi(base, channel);
		qdmaSetMulticastSptag(base, channel, sptag);
		qdmaSetMulticastFport(base, channel, ((port<<5)|nbq));
	}

	return 0;
}

int qdma_get_channel_cfg(struct ECNT_QDMA_Data *qdma_data)
{
	u32 channel=0;
	/* set hsgmii multicast fport & sptag info for external switch
	*  use channel 6~9
	*/
	if(ext_swicVendor)
	{
		channel = qdma_data->qdma_private.channel + EXT_SWITCH_CHANNEL_BASE;
	}
	/* hsgmii_lan use channel 10~13
	*  defined in hsgmii_lan_mac.c
	*  pcei0 serdes: channel 10
	*  pcei1 serdes: channel 11
	*  usb serdes: channel 12
	*  eth serdes: channel 13
	*/
#if defined(TCSUPPORT_HSGMII_LAN)
	channel = qdma_data->qdma_private.channel;
#endif

	return channel;
}

int qdma_clear_dbg_cntr_value_all(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0 ;
	
	for(index=0; index<CONFIG_MAX_CNTR_NUM; index++) {
		qdmaClearCntrCounter(base, index);
	}
#if SUPPORT_DBG_CNTMEM
	/* clear DBG_CNT_MEM */
	for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
		qdma_set_dbg_cntmem_clear(index, QDMA_DBG_CNTR_MEM_CTX);
		qdma_set_dbg_cntmem_clear(index, QDMA_DBG_CNTR_MEM_FTX);
		qdma_set_dbg_cntmem_clear(index, QDMA_DBG_CNTR_MEM_FRX);
		qdma_set_dbg_cntmem_clear(index, QDMA_DBG_CNTR_MEM_AGEOUT_DROP);
	}
	/* clear TX RING done num */
	for(index=0; index<TX_RING_NUM; index++) {
		qdmaClearCpuTxRingDoneCounter(base, index);
	}
	/* clear RX RING done num */
	for(index=0; index<RX_RING_NUM; index++) {
		gpQdmaPriv->counters.rxCounts[index] = 0;
	}
	/* clear total cntr num */
	qdmaClearAllCtxDoneCounter(base);
	qdmaClearAllFtxDoneCounter(base);
	qdmaClearAllFrxDoneCounter(base);
	qdmaClearAllMulticastDropCounter(base);
	qdmaClearAllAgeOutDropCounter(base);

#if defined(QDMA_LAN)
	/* clear multicast drop num */
	for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
		qdmaClearMulticastDropCounter(base, index);
	}
#endif
#endif
	return 0;
}

int qdma_debug_counter_dump(void)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0 ;
	for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {
		if(index%8 == 0)
			printk("\n");
		
		printk("     index:%d , counter:%d",index,qdmaGetCntrCounter(base, index));
	}
	printk("\n");
		
	for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {
		qdmaClearCntrCounter(base, index);
	}
	return 0 ;
}

int qdma_set_cntr_channel(struct ECNT_QDMA_Data *qdma_data)
{
#ifdef QDMA_LAN
    return 0;
#else
    uint channel_id = qdma_data->qdma_private.channel;
    if(16<=channel_id)
    {
        printk("the channel id_is must between 0 and 15  \n");
	    return 0;
	}
    QDMA_DBG_CNTR_T dbgCntr;
    dbgCntr.cntrEn = DBG_CNTR_ENABLE;
    dbgCntr.isChnlAll = 0;
    dbgCntr.isDscpRingAll = 1;
    dbgCntr.isQueueAll = 1;
    dbgCntr.queueIdx = 0 ;
    dbgCntr.dscpRingIdx = 0;
    dbgCntr.cntrIdx = channel_id;
    dbgCntr.chnlIdx= channel_id;
    dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
    qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
    dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;
    dbgCntr.cntrIdx = channel_id+DBG_CNTR_FWD_CHNL_GROUP_NUM;
    qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
    return 0;
#endif

}

int qdma_dump_cntr_channel(struct ECNT_QDMA_Data *qdma_data)
{
#ifdef QDMA_LAN
	return 0;
#else
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint channel_id = qdma_data->qdma_private.channel;

	if(16<=channel_id)
	{
		printk("the channel id_is must between 0 and 15 \n");
		return 0;
	}
	qdma_data->retValue = qdmaGetCntrCounter(base, channel_id) + qdmaGetCntrCounter(base, channel_id + DBG_CNTR_FWD_CHNL_GROUP_NUM);
	return qdma_data->retValue;
#endif
}

int qdma_clear_and_set_dbg_cntr_channel_group(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0, offset = 0 ;
	QDMA_DbgCntrChnlGroup_t cntrSrc = qdma_data->qdma_private.cntrSrc;
    QDMA_DBG_CNTR_T dbgCntr;
    dbgCntr.cntrEn = DBG_CNTR_ENABLE;
    dbgCntr.isChnlAll = 0;
    dbgCntr.isDscpRingAll = 1;
    dbgCntr.isQueueAll = 1;
    dbgCntr.queueIdx = 0 ;
    dbgCntr.dscpRingIdx = 0;

	if(cntrSrc >= QDMA_DBG_CNTR_QUEUE){
		QDMA_ERR("channel config type is error, should between 0 and 2\n") ;
		return -EINVAL;
	}

#if SUPPORT_DBG_CNTMEM
	qdmaEnableCntrMemCounter(base);
	dbgCntr.isChnlAll = 1;
	dbgCntr.chnlIdx = 0;
	dbgCntr.cntrIdx= 0;
	dbgCntr.cntrSrc = DBG_CNTR_MULTICAST_ALL_DROP;
	qdma_clear_and_set_dbg_cntr_info(&dbgCntr);

	gpQdmaPriv->dbgCntrType = cntrSrc ;

	return 0 ;

#else	
#ifdef QDMA_LAN
	if(CONFIG_QDMA_CHANNEL > 16) {
		QDMA_ERR("CONFIG_QDMA_CHANNEL is larger than 16, must modify this function!\n") ;
		return -EINVAL;
	}
	
	for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {		
        dbgCntr.chnlIdx = index ;
		
        dbgCntr.cntrIdx= index;
        dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
        dbgCntr.cntrIdx= index+CONFIG_QDMA_CHANNEL;
        dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
	}
#else
	if(cntrSrc == QDMA_DBG_CNTR_CHNL_TXCPU) {	
		for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {			
	        dbgCntr.chnlIdx = index ;
	        dbgCntr.cntrIdx = index;
	        dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
	        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
		}
	} else if(cntrSrc == QDMA_DBG_CNTR_CHNL_TXFWD1) {
		offset = 0 ;
	} else if(cntrSrc == QDMA_DBG_CNTR_CHNL_TXFWD2) {
		offset = DBG_CNTR_FWD_CHNL_GROUP_NUM;
	}

	if((cntrSrc == QDMA_DBG_CNTR_CHNL_TXFWD1) || (cntrSrc == QDMA_DBG_CNTR_CHNL_TXFWD2)) {
		for(index=0; index<DBG_CNTR_FWD_CHNL_GROUP_NUM; index++) {			
	        dbgCntr.chnlIdx = index+offset ;
			
	        dbgCntr.cntrIdx = index;
	        dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
	        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
	        dbgCntr.cntrIdx = index+DBG_CNTR_FWD_CHNL_GROUP_NUM;
	        dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;
	        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
		}
	}
#endif
	gpQdmaPriv->dbgCntrType = cntrSrc ;

	return 0 ;
#endif
}

int qdma_clear_and_set_dbg_cntr_queue_group(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0 ;
    QDMA_DBG_CNTR_T dbgCntr;
    dbgCntr.cntrEn = DBG_CNTR_ENABLE;
    dbgCntr.isChnlAll = 1;
    dbgCntr.isDscpRingAll = 1;
    dbgCntr.isQueueAll = 0;
    dbgCntr.chnlIdx = 0 ;
    dbgCntr.dscpRingIdx = 0;
	
	if(CONFIG_QDMA_QUEUE > 16) {
		QDMA_ERR("CONFIG_QDMA_QUEUE is larger than 16, must modify this function!\n") ;
		return -EINVAL;
	}

#if SUPPORT_DBG_CNTMEM
	qdmaEnableCntrMemCounter(base);
#endif

	for(index=0; index<CONFIG_QDMA_QUEUE; index++) {		
        dbgCntr.queueIdx = index ;
		
        dbgCntr.cntrIdx= index;
        dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
        dbgCntr.cntrIdx= index+CONFIG_QDMA_QUEUE;
        dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
#if SUPPORT_DBG_CNTMEM
        dbgCntr.cntrIdx= index+CONFIG_QDMA_QUEUE*2;
        dbgCntr.cntrSrc = DBG_CNTR_AGE_OUT_DROP;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
#ifdef QDMA_LAN
        dbgCntr.cntrIdx= index+CONFIG_QDMA_QUEUE*3;
        dbgCntr.cntrSrc = DBG_CNTR_MULTICAST_ALL_DROP;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
#endif
#endif
	}
	gpQdmaPriv->dbgCntrType = QDMA_DBG_CNTR_QUEUE ;

	return 0 ;
}

int qdma_clear_and_set_dbg_cntr_ring_group(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_DBGCNTR_2RING_MODE
	int index = 0 ;
    QDMA_DBG_CNTR_T dbgCntr;
    dbgCntr.cntrEn = DBG_CNTR_ENABLE;
    dbgCntr.isChnlAll = 1;
    dbgCntr.isDscpRingAll = 0;
    dbgCntr.isQueueAll = 1;
    dbgCntr.chnlIdx = 0 ;
	
	if(RX_RING_NUM > 16) {
		QDMA_ERR("RX_RING_NUM is larger than %d, must modify this function!\n", (RX_RING_NUM-1)) ;
		return -EINVAL;
	}
	
	for(index=2; index<RX_RING_NUM; index++) {		
        dbgCntr.dscpRingIdx = index ;
        dbgCntr.cntrIdx= index-2;
        dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_RX;
        qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
	}
	gpQdmaPriv->dbgCntrType = QDMA_DBG_CNTR_RING ;
#endif

	return 0 ;
}


#if SUPPORT_DBG_CNTMEM
void qdma_dump_dbg_cntr_value_timer(TIMER_FUN_PAAM arg)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	int index = 0, offset = 0;
	QDMA_DBG_CNTR_T dbgCntr;
	uint queueTxCpuVal = 0;
	uint queueTxFwdVal = 0;
	uint queueAgeOutDropVal = 0;
	uint queueMulticastDropVal = 0;
	uint chnlTxCpuVal = 0;
	uint chnlTxFwdVal = 0;
	uint chnlRxFwdVal = 0;
	uint chnlAgeOutDropVal = 0;
	uint chnlMulticastDropVal = 0;
	uint ringTxVal[TX_RING_NUM] = {0};
	uint ringRxVal[RX_RING_NUM] = {0};
	uint allTxCpuVal = 0;
	uint allRxCpuVal = 0;
	uint allTxFwdVal = 0;
	uint allRxFwdVal = 0;

	if(gpQdmaPriv->dbgCntrType >= QDMA_DBG_CNTR_MAX){
		QDMA_ERR("gpQdmaPriv->dbgCntrType is %d, should between 0 and 3\n", gpQdmaPriv->dbgCntrType) ;
		return;
	}
	if(gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_QUEUE) {
		printk("Dump Queue %d~%d TX CPU & FWD Counter:\n", 0, CONFIG_QDMA_QUEUE-1);

		for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
			dbgCntr.cntrIdx = index ;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_CPU_TX) || (dbgCntr.isQueueAll == 1) || (dbgCntr.queueIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isQueueAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isQueueAll) ;
				return;
			}
			queueTxCpuVal = dbgCntr.cntrVal;

			dbgCntr.cntrIdx = index+CONFIG_QDMA_QUEUE;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_FWD_TX) || (dbgCntr.isQueueAll == 1) || (dbgCntr.queueIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isQueueAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isQueueAll) ;
				return;
			}
			queueTxFwdVal = dbgCntr.cntrVal;

			dbgCntr.cntrIdx = index+CONFIG_QDMA_QUEUE*2;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_AGE_OUT_DROP) || (dbgCntr.isQueueAll == 1) || (dbgCntr.queueIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isQueueAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isQueueAll) ;
				return;
			}
			queueAgeOutDropVal = dbgCntr.cntrVal;
	
#ifdef QDMA_LAN
			dbgCntr.cntrIdx = index+CONFIG_QDMA_QUEUE*3;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_MULTICAST_ALL_DROP) || (dbgCntr.isQueueAll == 1) || (dbgCntr.queueIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isQueueAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isQueueAll) ;
				return;
			}
			queueMulticastDropVal = dbgCntr.cntrVal;

			printk("queue_%d: tx cpu cnt:%u, tx fwd cnt:%u, ageout drop cnt:%u, multicast drop cnt:%u\n", index, 
				queueTxCpuVal, (queueTxFwdVal>queueTxCpuVal)?(queueTxFwdVal-queueTxCpuVal):0, queueAgeOutDropVal, queueMulticastDropVal);
#else
			printk("queue_%d: tx cpu cnt:%u, tx fwd cnt:%u, ageout drop cnt:%u\n", index, 
				queueTxCpuVal, (queueTxFwdVal>queueTxCpuVal)?(queueTxFwdVal-queueTxCpuVal):0, queueAgeOutDropVal);
#endif
		}

	}
	else if(gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXCPU || gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD1
		|| gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD2){ 
		printk("Dump Channel %d~%d TX CPU & FWD Counter:\n", 0, CONFIG_QDMA_CHANNEL-1);
		for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
			chnlTxCpuVal = qdma_get_dbg_cntmem_cntr(index, QDMA_DBG_CNTR_MEM_CTX);
			chnlTxFwdVal = qdma_get_dbg_cntmem_cntr(index, QDMA_DBG_CNTR_MEM_FTX);
			chnlRxFwdVal = qdma_get_dbg_cntmem_cntr(index, QDMA_DBG_CNTR_MEM_FRX);
			chnlAgeOutDropVal = qdma_get_dbg_cntmem_cntr(index, QDMA_DBG_CNTR_MEM_AGEOUT_DROP);
#ifdef QDMA_LAN
			if(index < MULTICAST_PORT_NUM){
				chnlMulticastDropVal = qdmaGetMulticastDropCounter(base, index);
				printk("channel_%d: tx cpu cnt:%u, tx fwd cnt:%u, rx fwd cnt:%u, ageout drop cnt:%u, multicast drop cnt:%u\n", index, 
					chnlTxCpuVal, (chnlTxFwdVal>chnlTxCpuVal)?(chnlTxFwdVal-chnlTxCpuVal):0,
					(chnlRxFwdVal>chnlTxCpuVal)?(chnlRxFwdVal-chnlTxCpuVal):0, 
					chnlAgeOutDropVal, chnlMulticastDropVal);
			}else{
				printk("channel_%d: tx cpu cnt:%u, tx fwd cnt:%u, rx fwd cnt:%u, ageout drop cnt:%u\n", index, 
					chnlTxCpuVal, (chnlTxFwdVal>chnlTxCpuVal)?(chnlTxFwdVal-chnlTxCpuVal):0,
					(chnlRxFwdVal>chnlTxCpuVal)?(chnlRxFwdVal-chnlTxCpuVal):0, 
					chnlAgeOutDropVal);
			}
#else
			printk("channel_%d: tx cpu cnt:%u, tx fwd cnt:%u, rx fwd cnt:%u, ageout drop cnt:%u\n", index, 
				chnlTxCpuVal, (chnlTxFwdVal>chnlTxCpuVal)?(chnlTxFwdVal-chnlTxCpuVal):0,
				(chnlRxFwdVal>chnlTxCpuVal)?(chnlRxFwdVal-chnlTxCpuVal):0,
				chnlAgeOutDropVal);
#endif
		}
	}

	printk("All channel ageout drop accumulate counter:%u\n", qdmaGetAllAgeOutDropCounter(base));
#ifdef QDMA_LAN
	printk("All channel multicast drop accumulate counter:%u\n", qdmaGetAllMulticastDropCounter(base));
	if(gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXCPU || gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD1
			|| gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD2){
		dbgCntr.cntrIdx = 0;
		qdma_get_dbg_cntr_info(&dbgCntr);
		if((dbgCntr.cntrSrc != DBG_CNTR_MULTICAST_ALL_DROP) || (dbgCntr.isChnlAll == 0)) {
			QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isChnlAll is %d\n", 0 , dbgCntr.cntrSrc, dbgCntr.isChnlAll) ;
			return;
		}else{
			printk("All channel multicast drop counter:%u\n", dbgCntr.cntrVal);
		}
	}
#endif 

	for(index=0; index<TX_RING_NUM; index++) {
		ringTxVal[index] = qdmaGetCpuTxRingDoneCounter(base, index);
	}
	for(index=0; index<RX_RING_NUM; index++) {
		ringRxVal[index] = gpQdmaPriv->counters.rxCounts[index];
		allRxCpuVal = allRxCpuVal + ringRxVal[index];
	}
	allTxCpuVal = qdmaGetAllCtxDoneCounter(base);
	allTxFwdVal = qdmaGetAllFtxDoneCounter(base);
	allRxFwdVal = qdmaGetAllFrxDoneCounter(base);	

	/* TX_CPU , TX_FWD , RX_CPU , RX_FWD , TX ring0~7 , RX ring0~15 */   
	printk("\nTX count: cpu-path:%u, fwd-path:%u, total:%u\n", allTxCpuVal, allTxFwdVal-allTxCpuVal, allTxFwdVal);
	printk("TX cpu-path count: TxRing0:%u, TxRing1:%u, TxRing2:%u, TxRing3:%u, TxRing4:%u, TxRing5:%u, TxRing6:%u, TxRing7:%u\n", 
		ringTxVal[0], ringTxVal[1], ringTxVal[2], ringTxVal[3], ringTxVal[4], ringTxVal[5], ringTxVal[6], ringTxVal[7]);
#if defined(TCSUPPORT_CPU_EN7581)
	printk("                   TxRing8:%u, TxRing9:%u, TxRing10:%u, TxRing11:%u, TxRing12:%u, TxRing13:%u, TxRing14:%u, TxRing15:%u\n", 
		ringTxVal[8], ringTxVal[9], ringTxVal[10], ringTxVal[11], ringTxVal[12], ringTxVal[13], ringTxVal[14], ringTxVal[15]);
	printk("                   TxRing16:%u, TxRing17:%u, TxRing18:%u, TxRing19:%u, TxRing20:%u, TxRing21:%u, TxRing22:%u, TxRing23:%u\n", 
		ringTxVal[16], ringTxVal[17], ringTxVal[18], ringTxVal[19], ringTxVal[20], ringTxVal[21], ringTxVal[22], ringTxVal[23]);
	printk("                   TxRing24:%u, TxRing25:%u, TxRing26:%u, TxRing27:%u, TxRing28:%u, TxRing29:%u, TxRing30:%u, TxRing31:%u\n", 
		ringTxVal[24], ringTxVal[25], ringTxVal[26], ringTxVal[27], ringTxVal[28], ringTxVal[29], ringTxVal[30], ringTxVal[31]);
#endif
	printk("RX count: cpu-path:%u, fwd-path:%u, total:%u\n", allRxCpuVal, allRxFwdVal-allTxCpuVal, allRxCpuVal+allRxFwdVal-allTxCpuVal);
	printk("RX cpu-path count: RxRing0:%u, RxRing1:%u, RxRing2:%u, RxRing3:%u, RxRing4:%u, RxRing5:%u, RxRing6:%u, RxRing7:%u\n", 
		ringRxVal[0], ringRxVal[1], ringRxVal[2], ringRxVal[3], ringRxVal[4], ringRxVal[5], ringRxVal[6], ringRxVal[7]);
	printk("                   RxRing8:%u, RxRing9:%u, RxRing10:%u, RxRing11:%u, RxRing12:%u, RxRing13:%u, RxRing14:%u, RxRing15:%u\n", 
		ringRxVal[8], ringRxVal[9], ringRxVal[10], ringRxVal[11], ringRxVal[12], ringRxVal[13], ringRxVal[14], ringRxVal[15]);
#if defined(TCSUPPORT_CPU_EN7581)
	printk("                   RxRing16:%u, RxRing17:%u, RxRing18:%u, RxRing19:%u, RxRing20:%u, RxRing21:%u, RxRing22:%u, RxRing23:%u\n", 
		ringRxVal[16], ringRxVal[17], ringRxVal[18], ringRxVal[19], ringRxVal[20], ringRxVal[21], ringRxVal[22], ringRxVal[23]);
	printk("                   RxRing24:%u, RxRing25:%u, RxRing26:%u, RxRing27:%u, RxRing28:%u, RxRing29:%u, RxRing30:%u, RxRing31:%u\n", 
		ringRxVal[24], ringRxVal[25], ringRxVal[26], ringRxVal[27], ringRxVal[28], ringRxVal[29], ringRxVal[30], ringRxVal[31]);
#endif

	return;
}

int qdma_dump_dbg_cntr_value(struct ECNT_QDMA_Data *qdma_data)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	init_timer(&dbgcntr_dump_timer) ;
	dbgcntr_dump_timer.data = 1 ;
	dbgcntr_dump_timer.function = qdma_dump_dbg_cntr_value_timer;
	QDMA_START_TIMER(dbgcntr_dump_timer) ;
#else
	timer_setup(&dbgcntr_dump_timer, qdma_dump_dbg_cntr_value_timer, 0);
	dbgcntr_dump_timer.expires = 1;
	QDMA_START_TIMER(dbgcntr_dump_timer) ;
#endif
	return 0 ;
}
#else
int qdma_dump_dbg_cntr_value(struct ECNT_QDMA_Data *qdma_data)
{
	int index = 0, offset = 0 ;
	uint queueTxCpuVal[CONFIG_QDMA_QUEUE];
	uint chnlTxCpuVal[CONFIG_QDMA_CHANNEL];
#if SUPPORT_DBGCNTR_2RING_MODE
	uint ringRxCpuVal[RX_RING_NUM];
#endif
	uint defCfgVal[DBG_CNTR_DEFAULT_CONFIG_NUM];
    QDMA_DBG_CNTR_T dbgCntr;

	if(gpQdmaPriv->dbgCntrType >= QDMA_DBG_CNTR_MAX){
		QDMA_ERR("gpQdmaPriv->dbgCntrType is %d, should between 0 and 3\n", gpQdmaPriv->dbgCntrType) ;
		return -EINVAL;
	}
#if SUPPORT_DBGCNTR_2RING_MODE
		if(gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_RING) {
			printk("Dump Ring %d~%d CPU Path Counter:\n", 0, RX_RING_NUM-1);
			for(index=2; index<RX_RING_NUM; index++) { /*start from rx2*/
				dbgCntr.cntrIdx = index-2 ;
				qdma_get_dbg_cntr_info(&dbgCntr);
				if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_CPU_RX) || (dbgCntr.isDscpRingAll == 1) || (dbgCntr.dscpRingIdx != index)) {
					QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isDscpRingAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isDscpRingAll) ;
					return -EINVAL;
				}
				ringRxCpuVal[index] = dbgCntr.cntrVal;
				printk("Ring%2d: %u ", index, ringRxCpuVal[index]);
				if(index % 2)
					printk("\n");
			}
		}
#endif
	if(gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_QUEUE) {
		printk("Dump Queue %d~%d TX CPU & FWD Counter:\n", 0, CONFIG_QDMA_QUEUE-1);
		for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
			dbgCntr.cntrIdx = index ;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_CPU_TX) || (dbgCntr.isQueueAll == 1) || (dbgCntr.queueIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isQueueAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isQueueAll) ;
				return -EINVAL;
			}
			queueTxCpuVal[index] = dbgCntr.cntrVal;
		}
		
		for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
			dbgCntr.cntrIdx = index+CONFIG_QDMA_QUEUE;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_FWD_TX) || (dbgCntr.isQueueAll == 1) || (dbgCntr.queueIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isQueueAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isQueueAll) ;
				return -EINVAL;
			}
			printk("queue_%d: tx cpu cnt:%u, tx fwd cnt:%u\n", index, queueTxCpuVal[index], (dbgCntr.cntrVal>queueTxCpuVal[index])?(dbgCntr.cntrVal-queueTxCpuVal[index]):0);
		}
	}
#ifdef QDMA_LAN
	else if(gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXCPU){ /*QDMA_DBG_CNTR_CHNL_TXCPU here(QDMA_LAN) means dump both cpu and fwd cntr!!,which sync with app interface*/
		printk("Dump Channel %d~%d TX CPU & FWD Counter:\n", 0, CONFIG_QDMA_CHANNEL-1);
		for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
			dbgCntr.cntrIdx = index ;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_CPU_TX) || (dbgCntr.isChnlAll == 1) || (dbgCntr.chnlIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isChnlAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isChnlAll) ;
				return -EINVAL;
			}
			chnlTxCpuVal[index] = dbgCntr.cntrVal;
		}
		
		for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
			dbgCntr.cntrIdx = index+CONFIG_QDMA_CHANNEL;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_FWD_TX) || (dbgCntr.isChnlAll == 1) || (dbgCntr.chnlIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isChnlAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isChnlAll) ;
				return -EINVAL;
			}
			printk("channel_%d: tx cpu cnt:%u, tx fwd cnt:%u\n", index, chnlTxCpuVal[index], (dbgCntr.cntrVal>chnlTxCpuVal[index])?(dbgCntr.cntrVal-chnlTxCpuVal[index]):0);
		}
	}
#else
	else if (gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXCPU) {
		printk("Dump Channel %d~%d TX CPU Counter:\n", 0, CONFIG_QDMA_CHANNEL-1);
		for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
			dbgCntr.cntrIdx = index ;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_CPU_TX) || (dbgCntr.isChnlAll == 1) || (dbgCntr.chnlIdx != index)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isChnlAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isChnlAll) ;
				return -EINVAL;
			}
			printk("channel_%d: tx cpu cnt:%u\n", index, dbgCntr.cntrVal);
		}
	} else if (gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD1) {
		offset = 0 ;
	} else if (gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD2) {
		offset = DBG_CNTR_FWD_CHNL_GROUP_NUM ;
	}

	if((gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD1) || (gpQdmaPriv->dbgCntrType == QDMA_DBG_CNTR_CHNL_TXFWD2)) {
		printk("Dump Channel %d~%d TX CPU & FWD Counter:\n", offset, offset+DBG_CNTR_FWD_CHNL_GROUP_NUM-1);
		for(index=0; index<DBG_CNTR_FWD_CHNL_GROUP_NUM; index++) {
			dbgCntr.cntrIdx = index ;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_CPU_TX) || (dbgCntr.isChnlAll == 1) || (dbgCntr.chnlIdx != index+offset)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isChnlAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isChnlAll) ;
				return -EINVAL;
			}
			chnlTxCpuVal[index] = dbgCntr.cntrVal;
		}
		
		for(index=0; index<DBG_CNTR_FWD_CHNL_GROUP_NUM; index++) {
			dbgCntr.cntrIdx = index+DBG_CNTR_FWD_CHNL_GROUP_NUM;
			qdma_get_dbg_cntr_info(&dbgCntr);
			if((dbgCntr.cntrSrc != QDMA_DBG_CNTR_SRC_FWD_TX) || (dbgCntr.isChnlAll == 1) || (dbgCntr.chnlIdx != index+offset)) {
				QDMA_ERR("cntrIdx_%d: cntrSrc is %d, isChnlAll is %d\n", dbgCntr.cntrIdx , dbgCntr.cntrSrc, dbgCntr.isChnlAll) ;
				return -EINVAL;
			}
			printk("channel_%d: tx cpu cnt:%u, tx fwd cnt:%u\n", index+offset, chnlTxCpuVal[index], (dbgCntr.cntrVal>chnlTxCpuVal[index])?(dbgCntr.cntrVal-chnlTxCpuVal[index]):0);
		}
	}
#endif

	for(index=0; index<DBG_CNTR_DEFAULT_CONFIG_NUM; index++) {
		dbgCntr.cntrIdx = index+DBG_CNTR_DEFAULT_CONFIG_OFFSET;
		qdma_get_dbg_cntr_info(&dbgCntr);
		defCfgVal[index] = dbgCntr.cntrVal;
	}
	
	/*EN7580: QDMA support multicast*/
#if SUPPORT_QDMA_MULTICAST && defined(QDMA_LAN)
    printk("Multicast Drop Counter:\n");
    for(index=0;index<MULTICAST_PORT_NUM;index++){
       printk("channel_%d:  %u\n", index, defCfgVal[4+TX_RING_NUM+RX_RING_NUM+index]); 
    }
    printk("All channel multicast drop counter : %u", defCfgVal[4+TX_RING_NUM+RX_RING_NUM+MULTICAST_PORT_NUM]);
#endif 

#if SUPPORT_8_TX_RING
    /*EN7580: TX_CPU , TX_FWD , RX_CPU , RX_FWD , TX ring0~7 , RX ring0~15*/   
    printk("\nTX count: cpu-path:%u, fwd-path:%u, total:%u\n", defCfgVal[0], defCfgVal[1]-defCfgVal[0], defCfgVal[1]);
    printk("TX cpu-path count: TxRing0:%u, TxRing1:%u, TxRing2:%u, TxRing3:%u, TxRing4:%u, TxRing5:%u, TxRing6:%u, TxRing7:%u\n", 
		defCfgVal[4], defCfgVal[5], defCfgVal[6], defCfgVal[7], defCfgVal[8], defCfgVal[9], defCfgVal[10], defCfgVal[11]);
    printk("RX count: cpu-path:%u, fwd-path:%u, total:%u\n", defCfgVal[2], defCfgVal[3]-defCfgVal[0], defCfgVal[2]+defCfgVal[3]-defCfgVal[0]);
    printk("RX cpu-path count: RxRing0:%u, RxRing1:%u, RxRing2:%u, RxRing3:%u, RxRing4:%u, RxRing5:%u, RxRing6:%u, RxRing7:%u\n", 
		defCfgVal[12], defCfgVal[13], defCfgVal[14], defCfgVal[15], defCfgVal[16], defCfgVal[17], defCfgVal[18], defCfgVal[19]);
    printk("                   RxRing8:%u, RxRing9:%u, RxRing10:%u, RxRing11:%u, RxRing12:%u, RxRing13:%u, RxRing14:%u, RxRing15:%u\n", 
		defCfgVal[20], defCfgVal[21], defCfgVal[22], defCfgVal[23], defCfgVal[24], defCfgVal[25], defCfgVal[26], defCfgVal[27]);
#else
    printk("\nTX count: cpu-path:%u, fwd-path:%u, total:%u\n", defCfgVal[0], defCfgVal[1]-defCfgVal[0], defCfgVal[1]);
    printk("TX cpu-path count: TxRing0:%u, TxRing1:%u\n", defCfgVal[4], defCfgVal[5]);
    printk("RX count: cpu-path:%u, fwd-path:%u, total:%u\n", defCfgVal[2], defCfgVal[3], defCfgVal[2]+defCfgVal[3]);
    printk("RX cpu-path count: RxRing0:%u, RxRing1:%u\n", defCfgVal[6], defCfgVal[7]);
#endif

	return 0;
}
#endif

int qdma_get_dbg_cntr_rx_ring(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_DBG_CNTMEM
	int index = 0 ;
	QDMA_RX_Ring_Cnt_T *rxRingPtr = qdma_data->qdma_private.rxRingCntPtr;

	for(index=0; index<RX_RING_NUM; index++) 
	{
		rxRingPtr->rx_ring[index] = gpQdmaPriv->counters.rxCounts[index];
	}
#endif

	return 0 ;
}

int qdma_get_dbg_cntr_all_queue_value(struct ECNT_QDMA_Data *qdma_data)
{
	int index = 0 ;
	QDMA_TxQCngstAllQueueValue_T *txqCngstAllQueueValuePtr = qdma_data->qdma_private.txqCngstAllQueueValuePtr;
    QDMA_DBG_CNTR_T dbgCntr;

	memset(&dbgCntr,0,sizeof(dbgCntr));
	for(index=0; index<CONFIG_QDMA_QUEUE; index++) {
		dbgCntr.cntrIdx = index+CONFIG_QDMA_QUEUE;
		qdma_get_dbg_cntr_info(&dbgCntr);
		txqCngstAllQueueValuePtr->queueAllCount[index] = dbgCntr.cntrVal;
	}

	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to dump the tx qos type and queue weigth for all channel.
 Input Args:		chnlIdx: specific the channel ID (0~31) and chnl
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_dump_tx_qos(struct ECNT_QDMA_Data *qdma_data)
{
	int ret = 0 ;
	unchar chnlIdx=0, qosType = 0 ;
	ushort weight[CONFIG_QDMA_QUEUE] ;
	QDMA_TxQosWeightType_t weightBase=0;
	QDMA_TxQosWeightScale_t weightScale=0;	
	int channel = qdma_data->qdma_private.channel ;
	
	ret = qdma_get_tx_qos_weight(qdma_data) ;
	weightBase = qdma_data->qdma_private.qdma_tx_qos.weightBase ;
	weightScale = qdma_data->qdma_private.qdma_tx_qos.weightScale ;	
#if SUPPORT_WRR_WEIGHT_SCALE_1Byte
	printk("weightBase is %s, weightScale is %s\n"
		, ((weightBase==QDMA_TXQOS_WEIGHT_BY_PACKET)?("packet mode"):("byte mode"))
		, ((weightScale==QDMA_TXQOS_WEIGHT_SCALE_1B)?("1Byte"):("16Byte")));
#else
	printk("weightBase is %s, weightScale is %s\n"
		, ((weightBase==QDMA_TXQOS_WEIGHT_BY_PACKET)?("packet mode"):("byte mode"))
		, ((weightScale==QDMA_TXQOS_WEIGHT_SCALE_64B)?("64Byte"):("16Byte")));
#endif

	for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
		if((channel!=CONFIG_QDMA_CHANNEL) && (channel!=chnlIdx))
			continue;
		
		ret = qdmaGetTxQosScheduler(chnlIdx, &qosType, weight) ;
		if((ret < 0) || (qosType>QDMA_TXQOS_TYPE_NUMS)) {
			QDMA_ERR("qdma_dump_tx_qos Fail at channel-%d: qosType is %d\r\n", chnlIdx, qosType);
			return -EFAULT ;
		}
		printk("channel-%d: qosType is %s", chnlIdx, qosTypeStr[qosType]);
		switch(qosType) {
			case QDMA_TXQOS_TYPE_WRR:
				printk(", q7:q6:q5:q4:q3:q2:q1:q0=%d:%d:%d:%d:%d:%d:%d:%d\r\n", weight[7], weight[6], weight[5], weight[4], weight[3], weight[2], weight[1], weight[0]);
				break;
			case QDMA_TXQOS_TYPE_SP:
				printk("\r\n");
				break;
			case QDMA_TXQOS_TYPE_SPWRR7:
				printk(", q6:q5:q4:q3:q2:q1:q0=%d:%d:%d:%d:%d:%d:%d\r\n", weight[6], weight[5], weight[4], weight[3], weight[2], weight[1], weight[0]);
				break;
			case QDMA_TXQOS_TYPE_SPWRR6:
				printk(", q5:q4:q3:q2:q1:q0=%d:%d:%d:%d:%d:%d\r\n", weight[5], weight[4], weight[3], weight[2], weight[1], weight[0]);
				break;
			case QDMA_TXQOS_TYPE_SPWRR5:
				printk(", q4:q3:q2:q1:q0=%d:%d:%d:%d:%d\r\n", weight[4], weight[3], weight[2], weight[1], weight[0]);
				break;
			case QDMA_TXQOS_TYPE_SPWRR4:
				printk(", q3:q2:q1:q0=%d:%d:%d:%d\r\n", weight[3], weight[2], weight[1], weight[0]);
				break;
			case QDMA_TXQOS_TYPE_SPWRR3:
				printk(", q2:q1:q0=%d:%d:%d\r\n", weight[2], weight[1], weight[0]);
				break;
			case QDMA_TXQOS_TYPE_SPWRR2:
				printk(", q1:q0=%d:%d\r\n", weight[1], weight[0]);
				break;
			default:
				QDMA_ERR("\r\nqosType is not correct at channel-%d\r\n", chnlIdx);
				return -EFAULT ;
		}
	}
	
	return 0 ;
}

int qdma_dump_virtual_channel_qos(struct ECNT_QDMA_Data *qdma_data) 
{	
	uint base = gpQdmaPriv->csrBaseAddr ;
	int ret = 0 ;
	unchar qosType = 0 ;
	unchar weight[CONFIG_QDMA_QUEUE] ;	
	int phyChnl=0, phyChnlMax=0, virChnlMax=0;
	int channel = qdma_data->qdma_private.channel ;

	if(qdmaIsVChnlMapEnable(base) == QDMA_DISABLE) {
		printk("virtual channel mapping mode is disable.\n") ;
		return 0;
	}
	
	/* get 16queue or 32queue */
	if(qdmaIsVChnlMap32Queue(base))
		virChnlMax = 4 ;
	else 
		virChnlMax = 2 ;
	phyChnlMax = CONFIG_QDMA_CHANNEL / virChnlMax ;

	if(channel>phyChnlMax) {
		QDMA_ERR("physical channel:%d, should less than %d.\n", channel, phyChnlMax) ;
	}
	
	printk("virtual channel mapping mode is enable, virChnlMode is %s.\n", ((virChnlMax==4)?("32Queue"):("16Queue")));
	
	for(phyChnl=0 ; phyChnl<phyChnlMax ; phyChnl++) {
		if((channel!=CONFIG_QDMA_CHANNEL) && (channel!=phyChnl))
			continue;
		
		ret = qdmaGetVirtualChannelQos(phyChnl, virChnlMax, &qosType, weight) ;
		if((ret < 0) || (qosType > virChnlMax)) {
			QDMA_ERR("qdma_dump_virtual_channel_qos Fail at physical channel-%d: qosType is %d\r\n", phyChnl, qosType);
			return -EFAULT ;
		}
		
		printk("physical channel-%d: qosType:%s", phyChnl, vChnlQosTypeStr[qosType]);
		switch(qosType) {
			case QDMA_VCHNL_TXQOS_TYPE_WRR:
				if(virChnlMax == 4)
					printk(", vChnl3:vChnl2:vChnl1:vChnl0=%d:%d:%d:%d\r\n", weight[3], weight[2], weight[1], weight[0]);
				else
					printk(", vChnl1:vChnl0=%d:%d\r\n", weight[1], weight[0]);
				break;
			case QDMA_VCHNL_TXQOS_TYPE_SP:
				printk("\r\n");
				break;
			case QDMA_VCHNL_TXQOS_TYPE_SPWRR3:
				printk(", vChnl2:vChnl1:vChnl0=%d:%d:%d\r\n", weight[2], weight[1], weight[0]);
				break;
			case QDMA_VCHNL_TXQOS_TYPE_SPWRR2:
				printk(", vChnl1:vChnl0=%d:%d\r\n", weight[1], weight[0]);
				break;
			default:
				QDMA_ERR("\r\nqosType is not correct at physical channel-%d\r\n", phyChnl);
				return -EFAULT ;
		}
	}
	
	return 0;
}

#if SUPPORT_MTR_ACNT_IN_QDMA
int qdma_dump_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int chnlIdx=0;
	unsigned int curTicksel=0, unitBase=0, curUnit=0;
	int channel = qdma_data->qdma_private.channel ; 

	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmCfg_T txTrtcmInit;
	GENERAL_TrtcmRatelimitCfg_T txRateLimitCfg;
	GENERAL_TrtcmRatelimitSet_T txRateLimitSet;

	txTrtcmInit.trtcmModule = EGRESS_TRTCM;
	qdma_data_tmp.qdma_private.generalTrtcmCfgPtr = &txTrtcmInit;
	qdma_general_get_trtcm_cfg(&qdma_data_tmp);
	
	for(chnlIdx = 0 ; chnlIdx < CONFIG_QDMA_CHANNEL ; chnlIdx++)
	{
		if((channel!=CONFIG_QDMA_CHANNEL) && (channel!=chnlIdx))
			continue;
		
		txRateLimitCfg.trtcmModule = EGRESS_TRTCM ;
		txRateLimitCfg.Index = chnlIdx ;
		qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &txRateLimitCfg;
		qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);
		
		txRateLimitSet.trtcmModule = EGRESS_TRTCM ;
		txRateLimitSet.Index = chnlIdx;
		qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &txRateLimitSet;
		qdma_general_get_ratelimit_mode_value(&qdma_data_tmp);
		
		/*cal the unit*/
		if(txRateLimitCfg.PktMode == TRTCM_BYTE_MODE)
			unitBase = 8000;     /**8bits *1000/(curTicksel*10e-6 ) kbps **/
		else
			unitBase = 1000000;   /**1/(curTicksel*10e-6) pps **/
	 	curTicksel = (txRateLimitCfg.TickSel == TRTCM_FAST_TICK) ? 
						txTrtcmInit.trtcmFastTick : (txTrtcmInit.trtcmSlowTickRatio*txTrtcmInit.trtcmFastTick);
		if(curTicksel == 0)
			curUnit = 0 ;
		else
			curUnit = unitBase / curTicksel ;
		/*print info*/
		printk("channel-%d:\n",chnlIdx);
		printk("%s, %s:%dus , Unit: %d%s , %s , ratelimit value = %d%s , PBS = %d%s \n" , 
			(txRateLimitCfg.MeterEn == GENERAL_METER_ENABLE) ? ("ENABLE"):("DISABLE"),
			(txRateLimitCfg.TickSel == TRTCM_FAST_TICK) ? ("FastTick"):("SlowTick"),
			curTicksel,
			curUnit,
			(txRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			(txRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("Packet Mode"):("Byte Mode"),
			txRateLimitSet.RateLimitValue, 
			(txRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			txRateLimitSet.BucketSize,
			(txRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("Packet"):("Byte")
			);
	}

	printk("\r\n");
	return 0 ;
}

int qdma_dump_rx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int ringIdx=0;
	unsigned int curTicksel=0, unitBase=0, curUnit=0;
	int ring = qdma_data->qdma_private.channel ;
	int maxRingIdx=0;

	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmCfg_T rxTrtcmInit;
	GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
	GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;

	rxTrtcmInit.trtcmModule = INGRESS_TRTCM;
	qdma_data_tmp.qdma_private.generalTrtcmCfgPtr = &rxTrtcmInit;
	qdma_general_get_trtcm_cfg(&qdma_data_tmp);

	if(rxTrtcmInit.trtcmMode == TRTCM_MODE)
		maxRingIdx = (GENERAL_INGRESS_INDEX_MAX >> 1);
	else
		maxRingIdx = GENERAL_INGRESS_INDEX_MAX;

	for(ringIdx = 0 ; ringIdx < maxRingIdx ; ringIdx++)
	{
		if((ring!=maxRingIdx) && (ring!=ringIdx))
			continue;
		
		rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;
		rxRateLimitCfg.Index = ringIdx ;
		qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRateLimitCfg;
		qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);
		
		rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
		rxRateLimitSet.Index = ringIdx;
		qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &rxRateLimitSet;
		qdma_general_get_ratelimit_mode_value(&qdma_data_tmp);
		/*cal the unit*/
		if(rxRateLimitCfg.PktMode == TRTCM_BYTE_MODE)
			unitBase = 8000;      /**8bits *1000/(curTicksel*10e-6 ) kbps **/
		else
			unitBase = 1000000;   /**1/(curTicksel*10e-6) pps **/
	 	curTicksel = (rxRateLimitCfg.TickSel == TRTCM_FAST_TICK) ? 
						rxTrtcmInit.trtcmFastTick : (rxTrtcmInit.trtcmSlowTickRatio*rxTrtcmInit.trtcmFastTick);
		if(curTicksel == 0)
			curUnit = 0 ;
		else
			curUnit = unitBase / curTicksel ;
		
		/*print info*/
		if( ringIdx < RX_RING_NUM )
			printk("meter-%d(ring%d):\n",ringIdx,ringIdx);
		else
			printk("meter-%d:\n",ringIdx);
		
		printk("%s, %s:%dus , Unit: %d%s , %s , ratelimit value = %d%s , PBS = %d%s \n" , 
			(rxRateLimitCfg.MeterEn == GENERAL_METER_ENABLE) ? ("ENABLE"):("DISABLE"),
			(rxRateLimitCfg.TickSel == TRTCM_FAST_TICK) ? ("FastTick"):("SlowTick"),
			curTicksel,
			curUnit,
			(rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			(rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("Packet Mode"):("Byte Mode"),
			rxRateLimitSet.RateLimitValue, 
			(rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			rxRateLimitSet.BucketSize,
			(rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("Packet"):("Byte")
			);
	}

	printk("\r\n");
	return 0 ;
}

int qdma_dump_tx_dba_report(struct ECNT_QDMA_Data *qdma_data)
{
	int chnlIdx=0;
	unsigned int curTicksel=0, unitBase=0, curUnit=0;
	int channel = qdma_data->qdma_private.channel ; 

	struct ECNT_QDMA_Data qdma_data_tmp;
	GENERAL_TrtcmCfg_T txTrtcmInit;
	GENERAL_TrtcmCbsPbsCfg_T txTrtcmCfg;
	GENERAL_TrtcmCbsPbsSet_T txTrtcmSet;

	txTrtcmInit.trtcmModule = EGRESS_TRTCM;
	qdma_data_tmp.qdma_private.generalTrtcmCfgPtr = &txTrtcmInit;
	qdma_general_get_trtcm_cfg(&qdma_data_tmp);

	for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
		if((channel!=CONFIG_QDMA_CHANNEL) && (channel!=chnlIdx))
			continue;
		/*1. get trtcm cfg info*/
		txTrtcmCfg.trtcmModule = EGRESS_TRTCM;
		txTrtcmCfg.Index = chnlIdx;
		qdma_data_tmp.qdma_private.generalTrtcmCbsPbsCfgPtr = &txTrtcmCfg;
		qdma_general_get_trtcm_mode_cfg(&qdma_data_tmp);
		/*2. get trtcm value info*/
		txTrtcmSet.trtcmModule = EGRESS_TRTCM;
		txTrtcmSet.Index = chnlIdx;
		qdma_data_tmp.qdma_private.generalTrtcmCbsPbsSetPtr = &txTrtcmSet;
		qdma_general_get_trtcm_mode_value(&qdma_data_tmp);
		/*3. start to show info*/
		printk("channel-%d:\n",chnlIdx);
		/*CBS*/
		printk("cbs: ");
		/*cal the unit*/
		if(txTrtcmCfg.CbsPktMode == TRTCM_BYTE_MODE)
			unitBase = 8000;	  /**8bits *1000/(curTicksel*10e-6 ) kbps **/
		else
			unitBase = 1000000;   /**1/(curTicksel*10e-6) pps **/
		curTicksel = (txTrtcmCfg.CbsTickSel == TRTCM_FAST_TICK) ? 
						txTrtcmInit.trtcmFastTick : (txTrtcmInit.trtcmSlowTickRatio*txTrtcmInit.trtcmFastTick);
		if(curTicksel == 0)
			curUnit = 0 ;
		else
			curUnit = unitBase / curTicksel ;
		/*print info*/
		printk("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s \n" , 
			(txTrtcmCfg.CbsMeterEn == GENERAL_METER_ENABLE) ? ("ENABLE"):("DISABLE"),
			(txTrtcmCfg.CbsTickSel == TRTCM_FAST_TICK) ? ("FastTick"):("SlowTick"),
			curTicksel,
			curUnit,
			(txTrtcmCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			(txTrtcmCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("Packet Mode"):("Byte Mode"),
			txTrtcmSet.CbsParamValue, 
			(txTrtcmCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			txTrtcmSet.CbsBucketSize,
			(txTrtcmCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("Packet"):("Byte")
			);
		/*PBS*/
		printk("pbs: ");
		/*cal the unit*/
		if(txTrtcmCfg.PbsPktMode == TRTCM_BYTE_MODE)
			unitBase = 8000;	  /**8bits *1000/(curTicksel*10e-6 ) kbps **/
		else
			unitBase = 1000000;   /**1/(curTicksel*10e-6) pps **/
		curTicksel = (txTrtcmCfg.PbsTickSel == TRTCM_FAST_TICK) ? 
						txTrtcmInit.trtcmFastTick : (txTrtcmInit.trtcmSlowTickRatio*txTrtcmInit.trtcmFastTick);
		if(curTicksel == 0)
			curUnit = 0 ;
		else
			curUnit = unitBase / curTicksel ;
		/*print info*/
		printk("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s \n" , 
			(txTrtcmCfg.PbsMeterEn == GENERAL_METER_ENABLE) ? ("ENABLE"):("DISABLE"),
			(txTrtcmCfg.PbsTickSel == TRTCM_FAST_TICK) ? ("FastTick"):("SlowTick"),
			curTicksel,
			curUnit,
			(txTrtcmCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			(txTrtcmCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("Packet Mode"):("Byte Mode"),
			txTrtcmSet.PbsParamValue, 
			(txTrtcmCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
			txTrtcmSet.PbsBucketSize,
			(txTrtcmCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("Packet"):("Byte")
			);
	}

	printk("\r\n");
	return 0 ;
}
#else
int qdma_dump_tx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int ret = 0, chnlIdx=0,enMode=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint rateLimitValue=0, pbsParamValue=0, bucketScale=0 ;
	int channel = qdma_data->qdma_private.channel ;
	int curTicksel[PER_CHNL_TICKSEL_NUM]={0}, curUnit[PER_CHNL_TICKSEL_NUM]={0} ;
	int tickSel_idx=0, i=0 ;
	
	for(i=0; i<PER_CHNL_TICKSEL_NUM; i++){
		curTicksel[i] = qdmaGetTxRateLimitTicksel(base,i) ;
		if(curTicksel[i] == 0) 
			curUnit[i] = 0 ;
		else
			curUnit[i] = 8000 / curTicksel[i] ;
	}
	
	enMode = qdmaIsTxRateLimitEnable(base) ;
	bucketScale = qdmaGetTxRateLimitBucketScale(base) ;
	printk("TX-RateLimit: mode:%s, bucksetScale:%uByte\r\n", (enMode)?("Enable"):("Disable"), 1<<bucketScale);
	printk("tickerSel[1]:%dus, unit[1]:%dkbps ; tickerSel[0]:%dus, unit[0]:%dkbps.\r\n ",curTicksel[1],curUnit[1],curTicksel[0],curUnit[0]);
	if(enMode == QDMA_DISABLE) {
		return 0;
	}
		
	for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
		if((channel!=CONFIG_QDMA_CHANNEL) && (channel!=chnlIdx))
			continue;
		
		enMode = qdmaIsChnlRateLimitEnable(base, chnlIdx) ;
		ret = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PIR) ;
		if(ret < 0)
			return ret ;
		tickSel_idx=qdmaIsTxRateLimitChnlTickSel1(base, chnlIdx)?1:0;
		rateLimitValue = ret * curUnit[tickSel_idx] ;
		ret = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PBS) ;
		if(ret < 0)
			return ret ;
		pbsParamValue = ret << bucketScale ;
		printk("\r\nChannel-%2d: mode:%s", chnlIdx, (enMode)?("Enable"):("Disable"));
		printk(", ratelimit value:%ukbps, PBS:%uByte, tickSel[%d]:%dus, unit[%d]:%dkbps\r\n", rateLimitValue, pbsParamValue, tickSel_idx, curTicksel[tickSel_idx], tickSel_idx, curUnit[tickSel_idx]);
	}
	return 0 ;
}

int qdma_dump_rx_ratelimit(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0, ringIdx=0, unitBase=0;	/* unitBase means the unit when ticksel is 1us. */
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel=0, curUnit=0 ;
	QDMA_Mode_t enMode=0 ;
	QDMA_RxPktMode_t pktMode=0 ;
	uint rateLimitValue=0, pbsParamValue=0, bucketScale=0 ;
	int ringid = qdma_data->qdma_private.channel ;
	int curTicksel0=0, curUnit0=0;
	int rxBindTicker[RX_RING_NUM]={0};
	enMode = qdmaIsRxRateLimitEnable(base) ;
	bucketScale = qdmaGetRxRateLimitBucketScale(base) ;
	pktMode = qdmaIsRxRateLimitByPacket(base) ;
	if(pktMode) {
		unitBase = 1000000 ;
	} else {
		unitBase = 8000 ;
	}

	curTicksel = qdmaGetRxRateLimitTicksel(base, 1);
	if(curTicksel == 0) 
		curUnit = 0 ;
	else
		curUnit = unitBase / curTicksel ;
	
	curTicksel0 = qdmaGetRxRateLimitTicksel(base, 0);
	if(curTicksel0 == 0) 
		curUnit0 = 0 ;
	else
		curUnit0 = unitBase / curTicksel0 ;
	
	printk("RX-RateLimit: enMode:%s, pktMode:%s, tickSel1:%dus, unit1:%d%s, tickSel0:%dus, unit0:%d%s, bucksetScale:%uByte\r\n", ((enMode)?("Enable"):("Disable"))
		, ((pktMode)?("packet"):("byte")), curTicksel, curUnit, ((pktMode)?("pps"):("kbps")), curTicksel0, curUnit0, ((pktMode)?("pps"):("kbps")), 1<<bucketScale);

	if(enMode == QDMA_DISABLE) {
		return 0;
	}
	
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
		if((ringid!=RX_RING_NUM) && (ringid!=ringIdx))
			continue;
		
		ret = qdmaGetRxRateLimitConfig(ringIdx, QDMA_Rx_Rate_Limit_PIR) ;
		if(ret < 0)
			return ret ;

		rxBindTicker[ringIdx] = qdmaGetRxRatelimitSelTick(base,ringIdx);
		if(rxBindTicker[ringIdx] == 0)
			rateLimitValue = ret * curUnit0 ;
		else
			rateLimitValue = ret * curUnit ; /*ticker1*/
		ret = qdmaGetRxRateLimitConfig(ringIdx, QDMA_Rx_Rate_Limit_PBS) ;
		if(ret < 0)
			return ret ;
		pbsParamValue = ret << bucketScale ;

		printk("RingIdx-%d: ratelimit value:%u%s, PBS:%uByte, BindTicker %d\r\n", ringIdx, rateLimitValue, ((pktMode)?("pps"):("kbps")), pbsParamValue, rxBindTicker[ringIdx]);

	}
	return 0 ;
}

int qdma_dump_tx_dba_report(struct ECNT_QDMA_Data *qdma_data)
{
	int ret = 0, chnlIdx=0, i=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	int curTicksel[PER_CHNL_TICKSEL_NUM]={0}, curUnit[PER_CHNL_TICKSEL_NUM]={0};
	uint pirValue=0, pbsValue=0, cirValue=0, cbsValue=0, bucketScale=0 ;
	int channel = qdma_data->qdma_private.channel ;	
	
	for(i=0; i<PER_CHNL_TICKSEL_NUM; i++){
		curTicksel[i] = qdmaGetTxRateLimitTicksel(base,i);
		if(curTicksel[i] == 0) 
			curUnit[i] = 0 ;
		else
			curUnit[i] = 8000 / curTicksel[i] ;

		printk("TX-DBA Report: tickSel[%d]:%dus, unit[%d]:%dkbps\r\n", i, curTicksel[i], i, curUnit[i]);
	}	
	bucketScale = qdmaGetTxRateLimitBucketScale(base) ;
	printk("TX-DBA Report: bucksetScale:%uByte\r\n",1<<bucketScale);
	
	for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
		if((channel!=CONFIG_QDMA_CHANNEL) && (channel!=chnlIdx))
			continue;
		/*get ticker index*/
		i = qdmaIsTxRateLimitChnlTickSel1(base,chnlIdx)?1:0;
		ret = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PIR) ;
		if(ret < 0)
			return ret ;
		pirValue = ret * curUnit[i] ;
		
		ret = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PBS) ;
		if(ret < 0)
			return ret ;
		pbsValue = ret << bucketScale ;
		
		ret = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_CIR) ;
		if(ret < 0)
			return ret ;
		cirValue = ret * curUnit[i] ;
		
		ret = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_CBS) ;
		if(ret < 0)
			return ret ;
		cbsValue = ret << bucketScale ;
		
		printk("Channel-%d: CIR:%ukbps, CBS:%uByte, PIR:%ukbps, PBS:%uByte\r\n", chnlIdx, cirValue, cbsValue, pirValue, pbsValue);
	}
	return 0 ;
}
#endif

/******************************************************************************
 Descriptor:	It's used to dump QDMA txq cngst information.
 Input Args:	No input arguments
 Ret Value:	0: successful, otherwise failed.
******************************************************************************/
int qdma_dump_txq_cngst(struct ECNT_QDMA_Data *qdma_data)
{
#if 1
	int txRingIdx = 0 ;
	int i=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	QDMA_TxQCngstCfg_T txqCngstCfg;
	QDMA_TxQDynCngstTotalThrh_T totalThrh;
	QDMA_TxQDynCngstChnlThrh_T chnlThrh;
	QDMA_TxQDynCngstQueueThrh_T queueThrh;
	QDMA_PeekRateCfg_t peekrateCfg;
	QDMA_TxQDynamicCngstInfo_T dynCngstInfo;
	QDMA_TxQStaticDeiCfg_T deiThrh;
	QDMA_TxQStaticNormalCfg_T normThrh;
	QDMA_TxQStaticCfg_T staticCfg;
	QDMA_TxQStaticThrRatio_T staticThrRatio;
	QDMA_TxQStaticCngstInfo_T staticThrInfo;
	
	qdma_get_txq_cngst_cfg(&txqCngstCfg);
	printk("/*************************************** TXQ CNGST BEGIN ************************************/\n");
	printk("txq cngst drop is %s\n", ((txqCngstCfg.cngstDropEn)?("Enable"):("Disable")));
	printk("txq cngst dei drop is %s\n", ((txqCngstCfg.cngstDeiDropEn)?("Enable"):("Disable")));
	printk("txq dyn cngst is %s\n", ((txqCngstCfg.dynCngstEn)?("Enable"):("Disable")));
#if SUPPORT_TXQ_WRED
	printk("txq cngst wred is %s\n", ((txqCngstCfg.cngstWredEn)?("Enable"):("Disable")));
#endif

	for( txRingIdx = 0 ; txRingIdx < TX_RING_NUM ; txRingIdx++ )
	{
		printk("when total tx buffer usage greater than total_max_threshold, tx ring%d will be blocked:  %s\n"
			, txRingIdx , ((txqCngstCfg.dynCngstMaxThrhTxEn[txRingIdx])?("Enable"):("Disable")));
		printk("when total tx buffer usage greater than total_min_threshold, tx ring%d will be blocked:  %s\n"
			, txRingIdx , ((txqCngstCfg.dynCngstMinThrhTxEn[txRingIdx])?("Enable"):("Disable")));
	}

	if(qdmaIsTxqDynCngstEnable(base)) {
		dynCngstInfo.totalThrhPtr = &totalThrh ;
		dynCngstInfo.chnlThrhPtr = &chnlThrh ;
		dynCngstInfo.queueThrhPtr = &queueThrh ; 
		dynCngstInfo.peekrateCfgPtr = &peekrateCfg ; 
		qdma_data->qdma_private.dynCfgPtr = &dynCngstInfo ;
		qdma_get_txq_cngst_dynamic_info(qdma_data);
		printk("\ntxq cngst mode is dynamic\n");
		printk("total max threshold: %ukByte\n", dynCngstInfo.totalThrhPtr->dynCngstTotalMaxThrh);
		printk("total min threshold: %ukByte\n", dynCngstInfo.totalThrhPtr->dynCngstTotalMinThrh);
		printk("channel max threshold: %ukByte\n", dynCngstInfo.chnlThrhPtr->dynCngstChnlMaxThrh);
		printk("channel min threshold: %ukByte\n", dynCngstInfo.chnlThrhPtr->dynCngstChnlMinThrh);
		printk("queue max threshold: %ukByte\n", dynCngstInfo.queueThrhPtr->dynCngstQueueMaxThrh);
		printk("queue min threshold: %ukByte\n", dynCngstInfo.queueThrhPtr->dynCngstQueueMinThrh);
#if SUPPORT_TXQ_CNGST_DYN_DEI_THRH_SCALE
		printk("DEI packet threshold equal 1/%d normal packet\n", 1<<(txqCngstCfg.dynCngstDeiThrhScale+1));
#endif
		printk("dynamic congetion threshold will be updateed after WRR weight changed:  %s\n"
			, ((txqCngstCfg.dynCngstModeConfigTrigEn)?("Enable"):("Disable")));
		printk("dynamic congetion threshold will be updateed after packet receive or transmit:  %s\n"
			, ((txqCngstCfg.dynCngstModePacketTrigEn)?("Enable"):("Disable")));
		printk("dynamic congetion threshold will be updateed after ticksel %dus arrived:  %s\n"
			, txqCngstCfg.dynCngstTicksel, ((txqCngstCfg.dynCngstModeTimeTrigEn)?("Enable"):("Disable")));
		if(peekrateCfg.peekRateEn == QDMA_ENABLE) {
#if !SUPPORT_PEEK_RATE_MARGIN
			printk("Peek Rate Function is Enable, Duration is %ums\n"
				, peekrateCfg.peekRateDuration);
#else
			printk("Peek Rate Function is Enable, Duration is %ums, Margin is %d\n"
				, peekrateCfg.peekRateDuration, peekrateCfg.peekRateMargin);
#endif
		} else {
			printk("Peek Rate Function is Disable\n");
		}
	} else {
#if !SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
		staticThrInfo.totalThrhPtr =  &totalThrh ;
		staticThrInfo.deiThrhPtr = &deiThrh ; 
		staticThrInfo.normThrhPtr = &normThrh ; 
		qdma_data->qdma_private.staticCfgPtr = &staticThrInfo ;
		qdma_get_txq_cngst_static_info(qdma_data);
		printk("\ntxq cngst mode is static\n");
		printk("total max threshold: %ukByte\n", staticThrInfo.totalThrhPtr->dynCngstTotalMaxThrh);
		printk("total min threshold: %ukByte\n", staticThrInfo.totalThrhPtr->dynCngstTotalMinThrh);
		printk("\tNormal Threshold\tDEI Threshold\n");
		for(i=0; i<CONFIG_QDMA_QUEUE; i++) {
			printk("Q%d\t%ukByte\t\t\t%ukByte\n", i, normThrh.normalThrh[i], deiThrh.deiThrh[i]);
		}
#endif
	}

#if SUPPORT_TXQ_CNGST_STATIC_QUEUE_RATIO
	printk("\ntxq cngst static info:\n");
	for(i=0; i<CONFIG_QDMA_CHANNEL; i++) {
		staticCfg.chnlIdx = i;
		staticThrInfo.totalThrhPtr = &totalThrh;
		staticThrInfo.staticThrhCfgPtr = &staticCfg; 
		staticThrInfo.staticThrhRatioPtr = &staticThrRatio; 
		qdma_data->qdma_private.staticCfgPtr = &staticThrInfo;
		qdma_get_txq_cngst_static_info(qdma_data);
		printk("channel-%d:\t%s  ", i, (staticCfg.staticThrEn == QDMA_ENABLE) ? "Enable":"Disable");
		if(((i+1)%4) == 0)
			printk("\n");
	}
	printk("\n");
	printk("total max threshold: %u DSCPs\n", staticThrInfo.totalThrhPtr->dynCngstTotalMaxThrh);
	printk("total min threshold: %u DSCPs\n", staticThrInfo.totalThrhPtr->dynCngstTotalMinThrh);
	printk("Static Queue Threshold Ratio, Q0:Q1:Q2:Q3:Q4:Q5:Q6:Q7 = ");
	for(i=0; i<CONFIG_QDMA_QUEUE; i++) {
		printk("%d", staticThrRatio.queueThrRatio[i]);
		if( i < (CONFIG_QDMA_QUEUE-1) )
			printk(":");
	}
	printk("\n");
#endif
	printk("/**************************************** TXQ CNGST END *************************************/\n");

#endif

	return 0;
}

int qdma_dump_info_all(struct ECNT_QDMA_Data *qdma_data)
{
	int ret=0;
	
	printk("############ dump qdma all information begin ############\r\n");
	printk("\r\n\r\n\r\n************ dump register value ************\r\n\r\n");
	ret |= qdma_dump_register_value(qdma_data);
	printk("\r\n\r\n\r\n************ dump tx/rx descriptor ************\r\n\r\n");
	ret |= qdma_dump_descriptor_info(qdma_data);
	printk("\r\n\r\n\r\n************ dump hwfwd descriptor ************\r\n\r\n");
	ret |= qdma_dump_hwfwd_info(qdma_data);
	printk("\r\n\r\n\r\n************ dump irq information ************\r\n\r\n");
	ret |= qdma_dump_irq_info(qdma_data);
#if SUPPORT_IRQ2
	printk("\r\n\r\n\r\n************ dump irq2 information ************\r\n\r\n");
	ret |= qdma_dump_irq2_info(qdma_data);
#endif

	printk("\r\n\r\n\r\n************ dump cpu counters ************\r\n\r\n");
	ret |= qdma_dump_cpu_counters(qdma_data);
	printk("\r\n\r\n\r\n************ dump tx qos ************\r\n\r\n");
	qdma_data->qdma_private.channel = CONFIG_QDMA_CHANNEL;
	ret |= qdma_dump_tx_qos(qdma_data);
	printk("\r\n\r\n\r\n************ dump virtual channel qos ************\r\n\r\n");
	qdma_data->qdma_private.channel = CONFIG_QDMA_CHANNEL;
	ret |= qdma_dump_virtual_channel_qos(qdma_data);
	printk("\r\n\r\n\r\n************ dump tx ratelimit ************\r\n\r\n");
	qdma_data->qdma_private.channel = CONFIG_QDMA_CHANNEL;
	ret |= qdma_dump_tx_ratelimit(qdma_data);
	printk("\r\n\r\n\r\n************ dump rx ratelimit ************\r\n\r\n");
	qdma_data->qdma_private.channel = RX_RING_NUM;
	ret |= qdma_dump_rx_ratelimit(qdma_data);
	printk("\r\n\r\n\r\n************ dump tx dba report ************\r\n\r\n");
	qdma_data->qdma_private.channel = CONFIG_QDMA_CHANNEL;
	ret |= qdma_dump_tx_dba_report(qdma_data);
	printk("\r\n\r\n############ dump qdma all information end ############\r\n");
	
	return ret;
}

int qdma_set_mac_qos_config(struct ECNT_QDMA_Data *qdma_data){

    int ret = 0, i = 0, j = 0;  
	QDMA_TxQosScheduler_T txQos;
    struct ECNT_QDMA_Data qdma_data_t;
    QDMA_MacQoSCfg_T *macQosCfgPtr = qdma_data->qdma_private.macQosCfg;
#ifdef TCSUPPORT_QOS
	/*
    if((macQosCfgPtr->queueMask <= 0) || (macQosCfgPtr->queueMask > 15)) {
        ret = -1;
        return ret;
    }
	*/
    gQueueMask = macQosCfgPtr->queueMask;
#ifndef QDMA_LAN
    qos_flag = macQosCfgPtr->qosClsfy;
	if(qos_flag == QOS_HW_PQ){  /*strict priority*/

        /* set QDMA as Strict Priority Mode,
	 	 * so that p7> ... >p0 for all paths */
		for(i=0 ; i<10 ; i++){
			txQos.channel = i ;
			txQos.qosType = QDMA_TXQOS_TYPE_SP ; /*  Strict Priority */

			for(j=0 ; j<8 ; j++){
				txQos.queue[j].weight = 0; /* QDMA has 8 queue */
			}
            
			qdma_data_t.qdma_private.qdma_tx_qos.pTxQos = &txQos;
			qdma_set_tx_qos(&qdma_data_t) ;
		}

	}else if(qos_flag == QOS_HW_WRR){  /*WRR*/

        qos_wrr_info[0]=1;

        for(i=0; i<4; i++){
          qos_wrr_info[i+1] =  macQosCfgPtr->qosWrrInfo[i]; 
        }
        /* EN7512:set QDMA's SP & WRR priority registers, so that p7>p6>p5>p4> p3:p2:p1:p0 for all path && 
		 * p3:p2:p1:p0 == qos_wrr_info[1]:qos_wrr_info[2]:  qos_wrr_info[3]:qos_wrr_info[4] */
		for(i=0 ; i<10 ; i++){
			txQos.channel = i ;            
			txQos.qosType = QDMA_TXQOS_TYPE_SPWRR4 ;
            
			for(j=0 ; j<8 ; j++){
                if(j>=4)
				    txQos.queue[j].weight = 0;
                else
				    txQos.queue[j].weight = qos_wrr_info[4-j];
			}
			
			qdma_data_t.qdma_private.qdma_tx_qos.pTxQos = &txQos;
			qdma_set_tx_qos(&qdma_data_t) ;
		}
	}  
#endif
#endif
    return ret;
}

/******************************************************************************
 Descriptor:	It's used to get first unused meter index.
 Input Args:	No input arguments
 Ret Value:	return index of ifrst unused meter idx
******************************************************************************/
int find_first_unused_meter_index(int meter_type)
{
    int meter_index = 127;
    int word_num = 0;
    int word_offset = 0;

    if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (meter_type == METER2_TYPE)){
        for(word_offset = 0; word_offset < gpQdmaPriv->meter2Mgr.MaxMeterNum; word_offset++){
            if(0 == ((gpQdmaPriv->meter2Mgr.MeterUseState[0]>>word_offset)&1)){
                meter_index = word_offset;
                return meter_index;
            }
        }
    }
    else if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (meter_type == METER1_TYPE)){
        for(word_offset = 0; word_offset < gpQdmaPriv->meter1Mgr.MaxMeterNum; word_offset++){
            if(0 == ((gpQdmaPriv->meter1Mgr.MeterUseState[0]>>word_offset)&1)){
                meter_index = word_offset;
                return meter_index;
            }
        }
    }else {
	    for(word_num = 0; word_num < gpQdmaPriv->meterMgr.MeterGroupNum; word_num++){
	        if(gpQdmaPriv->meterMgr.MeterUseState[word_num] == 0xFFFFFFFF){
	            continue;
	        }else{
	            for(word_offset = 0; word_offset < BITSPERWORD; word_offset++){
	                if(0 == ((gpQdmaPriv->meterMgr.MeterUseState[word_num]>>word_offset)&1)){
	                    meter_index = word_num*BITSPERWORD + word_offset;
	                    return meter_index;
	                }
	            }
	        }
	    }
    }
    return meter_index;
}
/******************************************************************************
 Descriptor:	It's used to set meter use_state from disable to enable.
 Input Args:	meter idx
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int set_meter_status_enable(int index, QDMA_MeterManage_T *meterManage)
{   
	/*index>>5, because MeterUseState is 32 bitmap flag*/
	if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (meterManage->meter_type == METER2_TYPE)){
	    if(index < 0 || index >= gpQdmaPriv->meter2Mgr.MaxMeterNum )
        {
	        printk("correct meter:2 idx range 0 : %d; real meter idx :%d\n", gpQdmaPriv->meter2Mgr.MaxMeterNum-1, index);
	        return ERROR_METER_INDEX;
        }else{
	        gpQdmaPriv->meter2Mgr.MeterUseState[index>>5] |= (1<<index);
	        strlcpy(gpQdmaPriv->meter2Mgr.MeterUseFunc[index], meterManage->bind_func_name, NAME_LENGTH);
	        return METER_STATUS_DONE;
	    }

	}else if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (meterManage->meter_type == METER1_TYPE)){
	    if(index < 0 || index >= gpQdmaPriv->meter1Mgr.MaxMeterNum )
	    {
	        printk("correct meter:1 idx range 0 : %d; real meter idx :%d\n", gpQdmaPriv->meter1Mgr.MaxMeterNum-1, index);
	        return ERROR_METER_INDEX;
	    }else{
	        gpQdmaPriv->meter1Mgr.MeterUseState[index>>5] |= (1<<index);
	        strlcpy(gpQdmaPriv->meter1Mgr.MeterUseFunc[index], meterManage->bind_func_name, NAME_LENGTH);
	        return METER_STATUS_DONE;
	    }
	}
	else
	{
	    if(index < RX_RING_NUM || index >= gpQdmaPriv->meterMgr.MaxMeterNum )
	    {
	        printk("correct meter:0 idx range %d : %d; real meter idx :%d\n", RX_RING_NUM, gpQdmaPriv->meterMgr.MaxMeterNum-1, index);
	        return ERROR_METER_INDEX;
	    }else{
	        gpQdmaPriv->meterMgr.MeterUseState[index>>5] |= (1<<(index%BITSPERWORD));
	        strlcpy(gpQdmaPriv->meterMgr.MeterUseFunc[index], meterManage->bind_func_name, NAME_LENGTH);
	        return METER_STATUS_DONE;
        }
    }
}
/******************************************************************************
 Descriptor:	It's used to set meter use_state from enable to disable.
 Input Args:	meter idx
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int set_meter_status_disable(int index, QDMA_MeterType_T meter_type)
{
	if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (meter_type == METER2_TYPE)){
	    if(index < 0 || index >= gpQdmaPriv->meter2Mgr.MaxMeterNum )
	    {
	        printk("correct meter idx range 0 : %d; real meter idx :%d\n", gpQdmaPriv->meter2Mgr.MaxMeterNum-1, index);
	        return ERROR_METER_INDEX;
	    }else{
	        gpQdmaPriv->meter2Mgr.MeterUseState[index>>5] &= ~(1<<index);
	        strcpy(gpQdmaPriv->meter2Mgr.MeterUseFunc[index], "");
	        return METER_STATUS_DONE;
	    }

	}else if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (meter_type == METER1_TYPE)){
	    if(index < 0 || index >= gpQdmaPriv->meter1Mgr.MaxMeterNum )
	    {
	        printk("correct meter idx range 0 : %d; real meter idx :%d\n", gpQdmaPriv->meter1Mgr.MaxMeterNum-1, index);
	        return ERROR_METER_INDEX;
	    }else{
	        gpQdmaPriv->meter1Mgr.MeterUseState[index>>5] &= ~(1<<index);
	        strcpy(gpQdmaPriv->meter1Mgr.MeterUseFunc[index], "");
	        return METER_STATUS_DONE;
	    }
	}else{
	    if(index < RX_RING_NUM || index >= gpQdmaPriv->meterMgr.MaxMeterNum )
	    {
	        printk("correct meter idx range %d : %d; real meter idx :%d\n", RX_RING_NUM, gpQdmaPriv->meterMgr.MaxMeterNum-1, index);
	        return ERROR_METER_INDEX;
	    }else{
	        gpQdmaPriv->meterMgr.MeterUseState[index>>5] &= ~(1<<(index%BITSPERWORD));
	        strcpy(gpQdmaPriv->meterMgr.MeterUseFunc[index], "");
	        return METER_STATUS_DONE;
	    }
	}
}
/******************************************************************************
 Descriptor:	It's used to allocate one meter resource for use.
 Input Args:    meter bind func name
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int qdma_allocate_meter(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_MTR_ACNT_IN_QDMA
    int meter_type = 0;
    QDMA_MeterManage_T *meterManage = qdma_data->qdma_private.meterManage;
    meter_type = meterManage->meter_type;

    meterManage->meter_id= find_first_unused_meter_index(meter_type);
    if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (meterManage->meter_id == 0x7f))
    {
        if(meter_type == METER0_TYPE)
            printk("%d meter-0 id has been used, there is no left\n", GENERAL_INGRESS_INDEX_MAX);
        else if(meter_type == METER1_TYPE)
            printk("%d meter-1 id has been used, there is no left\n", GENERAL_INGRESS_INDEX_MAX_GRP1);
        else
            printk("%d meter-2 id has been used, there is no left\n", GENERAL_INGRESS_INDEX_MAX_GRP2);
    }

    set_meter_status_enable(meterManage->meter_id, meterManage);
#endif
    return 0 ;
}
/******************************************************************************
 Descriptor:	It's used to free one meter resource in use.
 Input Args:    meter id
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int qdma_free_meter(struct ECNT_QDMA_Data *qdma_data)
{
#if SUPPORT_MTR_ACNT_IN_QDMA
    QDMA_MeterManage_T *meterManage = qdma_data->qdma_private.meterManage;
    
    set_meter_status_disable(meterManage->meter_id, meterManage->meter_type);
#endif
    return 0;
}

/******************************************************************************
 Descriptor:	It's used to get first unused acnt0 or acnt 1index.
 Input Args:	No input arguments
 Ret Value:	return index of ifrst unused acnt0 idx
******************************************************************************/
int find_first_unused_acnt_index(int acnt_type){
#if SUPPORT_MTR_ACNT_IN_QDMA
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	int acnt_idx = 0x3f;
#else
    int acnt_idx = 0x7f;
#endif
    int word_offset = 0;
    
    if(acnt_type == ACNT0_TYPE){
        for(word_offset = 0; word_offset < CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM; word_offset++){
            if(0 == (((gpQdmaPriv->acnt0Mgr.Acnt0UseState)>>word_offset) & 0x1)){
                acnt_idx = word_offset;
                return acnt_idx ;
            }
        }
    }else if(acnt_type == ACNT1_TYPE){
        for(word_offset = 0; word_offset < CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM; word_offset++){
            if(0 == (((gpQdmaPriv->acnt1Mgr.Acnt1UseState) >> word_offset) & 0x1)){
                acnt_idx = word_offset;
                return acnt_idx ;
            }
        }
    }else if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (acnt_type == ACNT2_TYPE)){
        for(word_offset = 0; word_offset < CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM; word_offset++){
            if((word_offset <= 63) && (0 == (((gpQdmaPriv->acnt2Mgr.Acnt2UseState0) >> word_offset) & 0x1))){//for acnt0~63
                acnt_idx = word_offset;
                return acnt_idx ;
            }else if((word_offset > 63) && (0 == (((gpQdmaPriv->acnt2Mgr.Acnt2UseState1) >> (word_offset - 64)) & 0x1))){//for acnt64~127
                acnt_idx = word_offset;
                return acnt_idx ;
            }
        }
    }else
        return acnt_idx ; 

    return acnt_idx ; 
#else
    return 0x1f;
#endif
}

/******************************************************************************
 Descriptor:	It's used to set acnt0 or acnt 1 use_state from disable to enable.
 Input Args:	meter idx
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int set_acnt_status_enable(int index, int acnt_type, QDMA_AcntManage_T  *acntManage){
#if SUPPORT_MTR_ACNT_IN_QDMA
    if(acnt_type == ACNT0_TYPE){
        if((index >= CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt0 idx range 0 : %d; real acnt0 idx :%d\n", CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM, index);
            return 0;
        }else{
            gpQdmaPriv->acnt0Mgr.Acnt0UseState |= (1ULL<<index) ;   
            strlcpy(gpQdmaPriv->acnt0Mgr.Acnt0UseFunc[index], acntManage->bind_func_name, NAME_LENGTH);
        }
    }else if(acnt_type == ACNT1_TYPE){
        if((index >= CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt1 idx range 0 : %d; real acnt1 idx :%d\n", CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM, index);
            return 0;
        }else{
            gpQdmaPriv->acnt1Mgr.Acnt1UseState |= (1<<index) ;
            strlcpy(gpQdmaPriv->acnt1Mgr.Acnt1UseFunc[index], acntManage->bind_func_name, NAME_LENGTH);
        }
    }
	else if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (acnt_type == ACNT2_TYPE)){
        if((index >= CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt2 idx range 0 : %d; real acnt2 idx :%d\n", CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM, index);
            return 0;
        }else{
            if(index <= 63){//for acnt0~63
                gpQdmaPriv->acnt2Mgr.Acnt2UseState0 |= (1ULL<<index);
                strlcpy(gpQdmaPriv->acnt2Mgr.Acnt2UseFunc[index], acntManage->bind_func_name, NAME_LENGTH);
            }else if(index > 63){//for acnt64~127
                gpQdmaPriv->acnt2Mgr.Acnt2UseState1 |= (1ULL<<(index - 64));
                strlcpy(gpQdmaPriv->acnt2Mgr.Acnt2UseFunc[index], acntManage->bind_func_name, NAME_LENGTH);
            }
        }
    }else
        return 0; 
#endif

    return 0;
}
/******************************************************************************
 Descriptor:	It's used to set acnt0 or acnt 1 use_state from enable to disable 
 Input Args:	meter idx
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int set_acnt_status_disable(int index, int acnt_type){
#if SUPPORT_MTR_ACNT_IN_QDMA
    if(acnt_type == ACNT0_TYPE){
        if((index >= CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt0 idx range 0 : %d; real acnt0 idx :%d\n", CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM, index);
            return 0;
        }else{
            gpQdmaPriv->acnt0Mgr.Acnt0UseState &= ~(1ULL<<index) ;    
            strcpy(gpQdmaPriv->acnt0Mgr.Acnt0UseFunc[index], "");
        }                  
    }else if(acnt_type == ACNT1_TYPE){
        if((index >= CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt1 idx range 0 : %d; real acnt1 idx :%d\n", CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM, index);
            return 0;
        }else{
            gpQdmaPriv->acnt1Mgr.Acnt1UseState &= ~(1<<index) ;
            strcpy(gpQdmaPriv->acnt1Mgr.Acnt1UseFunc[index], "");
        }   
    }
    else if((TCSUPPORT_ACCOUNT_METER_V2_VAL) && (acnt_type == ACNT2_TYPE)){
        if((index >= CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt2 idx range 0 : %d; real acnt2 idx :%d\n", CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM, index);
            return 0;
        }else{
            if(index <= 63){//for acnt0~63
            	gpQdmaPriv->acnt2Mgr.Acnt2UseState0 &= ~(1ULL<<index);
            	strcpy(gpQdmaPriv->acnt2Mgr.Acnt2UseFunc[index], "");
            }else{//for acnt64~127
            	gpQdmaPriv->acnt2Mgr.Acnt2UseState1 &= ~(1ULL<<(index - 64));
            	strcpy(gpQdmaPriv->acnt2Mgr.Acnt2UseFunc[index], "");
            }
        }   
    }else
        return 0; 
#endif

    return 0;

}
/******************************************************************************
 Descriptor:	It's used to allocate one acnt0 or acnt 1 resource for use.
 Input Args:    meter bind func name
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int qdma_allocate_acnt(struct ECNT_QDMA_Data *qdma_data){
#if SUPPORT_MTR_ACNT_IN_QDMA
    int acnt_type = 0;
    QDMA_AcntManage_T  *acntManage = qdma_data->qdma_private.acntManage;
    acnt_type = acntManage->acnt_type;
    
    acntManage->acnt_id = find_first_unused_acnt_index(acnt_type);

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		if(acntManage->acnt_id == 0x3f)
		{
			if(acnt_type == ACNT0_TYPE)
				printk("63 acnt0 id has been used, there is no left\n");
			else
				printk("31 acnt1 id has been used, there is no left\n");
			return 0;
		}
#else
    if(acntManage->acnt_id == 0x7f)
    {
        if(acnt_type == ACNT0_TYPE)
            printk("%d acnt0 id has been used, there is no left\n", CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM);
        else if(acnt_type == ACNT1_TYPE)
            printk("%d acnt1 id has been used, there is no left\n", CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM);
        else
            printk("%d acnt2 id has been used, there is no left\n", CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM);
        return 0;
    }
#endif

    set_acnt_status_enable(acntManage->acnt_id, acnt_type, acntManage);
#endif

    return 0;
}
/******************************************************************************
 Descriptor:	It's used to free one acnt0 or acnt 1 resource 
 Input Args:    meter bind func name
 Ret Value:	0: successfully set. others failed
******************************************************************************/
int qdma_free_acnt(struct ECNT_QDMA_Data *qdma_data){
#if SUPPORT_MTR_ACNT_IN_QDMA
    QDMA_AcntManage_T  *acntManage = qdma_data->qdma_private.acntManage;
    int dex = 0;
    int acnt_type = 0;

    dex = acntManage->acnt_id ; 
    acnt_type = acntManage->acnt_type ;
    
    set_acnt_status_disable(dex, acnt_type);
#endif
    return 0;
}
/******************************************************************************
 Descriptor:	It's used to get Qos flag in qdma_wan
 Input Args:    none
 Ret Value:	QoS_flag
******************************************************************************/
int qdma_get_qos_flag(struct ECNT_QDMA_Data *qdma_data)
{   
#if SUPPORT_MTR_ACNT_IN_QDMA
    QDMA_MeterManage_T *meterManage = qdma_data->qdma_private.meterManage;

#ifndef QDMA_LAN
#ifdef TCSUPPORT_QOS
    meterManage->Qos_flag = qos_flag;
#else
	meterManage->Qos_flag = NULLQOS;
#endif
#else
    meterManage->Qos_flag = NULLQOS;/*there is no qos flag in qdma_lan*/
#endif
#endif
    return 0;
}

int qdma_dram_test_dma_config(struct ECNT_QDMA_Data *qdma_data)
{
	QDMA_DramTestCfg_T *dramTestCfg = qdma_data->qdma_private.dramTestCfgPtr;
	struct sk_buff *skb_dram_test = NULL;
	uint8 *tx_data = NULL;
	struct QDMA_DscpInfo_S *pNewDscpInfo = NULL;
	QDMA_DMA_DSCP_T *pTxDscp = NULL;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags = 0 ;
	int channel_id = 0, queue_id = 0;

#if defined(TCSUPPORT_QDMA_VERIFY)
	dramTestDataP = dramTestCfg->dataP;
	dramTestDataLen = dramTestCfg->dataLen;

	skb_dram_test = skbmgr_dev_alloc_skb2k() ;
    if(skb_dram_test !=NULL){
        tx_data = skb_put(skb_dram_test, dramTestDataLen);
        //memcpy(tx_data, dramTestDataP, dramTestDataLen);

		/* start to transmit packet */
		spin_lock_irqsave(&gpQdmaPriv->txLock[dramTestRingIdx], flags);	
		/* Get unused TX DSCP from TX unused DSCP link list */	
		pNewDscpInfo = qdma_bm_pop_tx_dscp(dramTestRingIdx) ;
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[dramTestRingIdx]++ ;
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[dramTestRingIdx], flags);
			dev_kfree_skb_any(skb_dram_test);
			return -ENOSR ;
		}
#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[dramTestRingIdx]->dscpPtr);
#else
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[dramTestRingIdx]->dscpPtr);
#endif

		pTxDscp->msg[0] = (channel_id << 3) | queue_id;
		pTxDscp->msg[1] = 0x7f0007ff;

		pTxDscp->ctrl.nls = 0 ;
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
		pTxDscp->pkt_addr = (unsigned long)dramTestDataP;
		//pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)dramTestDataP);
		//ecnt_dcache_wback_inv(((unsigned long)(dramTestDataP)), dramTestDataLen);
#else
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)dramTestDataP);
		dma_cache_wback_inv((unsigned long)(dramTestDataP), dramTestDataLen);
#endif

		pTxDscp->ctrl.pkt_len = dramTestDataLen ; 
		pTxDscp->ctrl.done = 0 ;
#if SUPPORT_TX_DROP_BIT
		pTxDscp->ctrl.drop_pkt = 0 ;
#endif

#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		gpQdmaPriv->txUsingPtr[dramTestRingIdx]->skb = skb_dram_test ;
		gpQdmaPriv->txUsingPtr[dramTestRingIdx] = pNewDscpInfo ;

#if !defined(TCSUPPORT_CPU_ARMV8)
		SYNC_TYPE4(); 
#endif
		//qdmaSetTxCpuIdx(base, dramTestRingIdx, pNewDscpInfo->dscpIdx) ;
		dramTestDscpIdx = pNewDscpInfo->dscpIdx;
			
		gpQdmaPriv->counters.txCounts[dramTestRingIdx]++ ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[dramTestRingIdx], flags);

    }else{		
		printk("allocate skb fail\n");
    }
#endif

	return 0;
}

int qdma_dram_test_dma_enable(struct ECNT_QDMA_Data *qdma_data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	
#if defined(TCSUPPORT_QDMA_VERIFY)
	qdmaSetTxCpuIdx(base, dramTestRingIdx, dramTestDscpIdx) ;
#endif

	return 0;
}

int qdma_dram_test_is_rx_done(struct ECNT_QDMA_Data *qdma_data)
{
	QDMA_DramTestCfg_T *dramTestCfg = qdma_data->qdma_private.dramTestCfgPtr;

#if defined(TCSUPPORT_QDMA_VERIFY)
	dramTestCfg->rxDone = dramTestRxDone;
	dramTestRxDone = 0;
#endif

	return 0;
}

int qdma_dram_test_dma_skb_get(struct ECNT_QDMA_Data *qdma_data)
{
	QDMA_DramTestCfg_T *dramTestCfg = qdma_data->qdma_private.dramTestCfgPtr;

#if defined(TCSUPPORT_QDMA_VERIFY)
	dramTestCfg->skb = skb_rx_dram_test;
#endif

	return 0;
}

int qdma_api_set_txmsg(struct ECNT_QDMA_Data *qdma_data)
{
	txMsgWord_t txMsgWord;
    txMsgWord.pTxMsgW0 = (txMsgWord0_t *)&(qdma_data->qdma_private.qdmaSetTxMsgCfg.pTxMsg->txmsg0);
    txMsgWord.pTxMsgW1 = (txMsgWord1_t *)&(qdma_data->qdma_private.qdmaSetTxMsgCfg.pTxMsg->txmsg1);
    txMsgWord.pTxMsgW2 = (txMsgWord2_t *)&(qdma_data->qdma_private.qdmaSetTxMsgCfg.pTxMsg->txmsg2);
    QDMA_SET_TXMSG_FIELD_t field = qdma_data->qdma_private.qdmaSetTxMsgCfg.field;
    unsigned int value = qdma_data->qdma_private.qdmaSetTxMsgCfg.value;

    if(field < TXMSG_FIELD_MAX_NUM)
        qdma_txmsg_operation[field](&txMsgWord,value);

	return 0;
}

int qdma_api_get_rxmsg(struct ECNT_QDMA_Data *qdma_data)
{
    rxMsgWord_t* pRxMsg = (rxMsgWord_t*)(qdma_data->qdma_private.qdmaGetRxMsgCfg.pRxMsg);
    QDMA_GET_RXMSG_FIELD_t field = qdma_data->qdma_private.qdmaGetRxMsgCfg.field;
    unsigned int* pValue = qdma_data->qdma_private.qdmaGetRxMsgCfg.pValue;

    if(field < RXMSG_FIELD_MAX_NUM)
        qdma_rxmsg_operation[field](pRxMsg,pValue);

	return 0;
}

int qdma_api_get_rxmsg_eth(struct ECNT_QDMA_Data *qdma_data)
{
    rxMsgWord_t* pRxMsg = (rxMsgWord_t*)(qdma_data->qdma_private.qdmaGetRxMsgCfgEth.pRxMsg);
    QDMA_RxMsg_Ether_T* p_rxmsg_eth = qdma_data->qdma_private.qdmaGetRxMsgCfgEth.p_rxmsg_eth;
    unsigned int value;

    qdma_get_rxmsg_whole_rxmsg0(pRxMsg,&value);
    p_rxmsg_eth->rxmsg0 = value;

    qdma_get_rxmsg_whole_rxmsg1(pRxMsg,&value);
    p_rxmsg_eth->rxmsg1 = value;

    qdma_get_rxmsg_whole_rxmsg2(pRxMsg,&value);
    p_rxmsg_eth->rxmsg2 = value;

    qdma_get_rxmsg_hopflags(pRxMsg,&value);
    p_rxmsg_eth->hopflags = value&0x7;

    qdma_get_rxmsg_ptp(pRxMsg,&value);
    p_rxmsg_eth->ptp = value&0x7;

    qdma_get_rxmsg_sptag(pRxMsg,&value);
    p_rxmsg_eth->sptag = value&0xffff;

    qdma_get_rxmsg_foe_index(pRxMsg,&value);
    p_rxmsg_eth->foe_index = value&0xffff;

    qdma_get_rxmsg_sport(pRxMsg,&value);
    p_rxmsg_eth->sport = value&0xff;

    qdma_get_rxmsg_crsn(pRxMsg,&value);
    p_rxmsg_eth->crsn = value&0xff;

    qdma_get_rxmsg_ip4f(pRxMsg,&value);
    p_rxmsg_eth->ip4f = value&0x1;

    qdma_get_rxmsg_l4f(pRxMsg,&value);
    p_rxmsg_eth->l4f = value&0x1;

    qdma_get_rxmsg_l4vld(pRxMsg,&value);
    p_rxmsg_eth->l4vld = value&0x1;

    qdma_get_rxmsg_ip4(pRxMsg,&value);
    p_rxmsg_eth->ip4 = value&0x1;

    qdma_get_rxmsg_ip6(pRxMsg,&value);
    p_rxmsg_eth->ip6 = value&0x1;

    qdma_get_rxmsg_swudf(pRxMsg,&value);
    p_rxmsg_eth->swudf = value&0xff;

	return 0;
}

int qdma_api_check_dscp_is_free(void)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	int i=0, tx_cnt=0, rx_cnt=0, ringIdx=0;
	uint RxCpuIdx = 0 , RxDmaIdx = 0 ;

	for(ringIdx=0 ; ringIdx<TX_RING_NUM ; ringIdx++) {
		
		tx_cnt = 0 ;
		for(i=0 ; i<gpQdmaPriv->txDscpNum[ringIdx] ; i++) {
			diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + i ;
			if(diPtr->next == NULL && diPtr!=gpQdmaPriv->txTailPtr[ringIdx]) {
				tx_cnt++ ;	
			}
		}
		if(tx_cnt > 1)
			return 1;
	}
	
	for(ringIdx=0; ringIdx<RX_RING_NUM ; ringIdx++) {
		RxCpuIdx = qdmaGetRxCpuIdx(gpQdmaPriv->csrBaseAddr, ringIdx); 
		RxDmaIdx = qdmaGetRxDmaIdx(gpQdmaPriv->csrBaseAddr, ringIdx); 

		if( RxDmaIdx > RxCpuIdx )
			rx_cnt = RxDmaIdx-RxCpuIdx;
		else
			rx_cnt = RxDmaIdx+gpQdmaPriv->rxDscpNum[ringIdx]-RxCpuIdx;

		if(rx_cnt > 3)
			return 1;
	}
	
	return 0 ;
}

int qdma_api_set_downstream_qos_mode(struct ECNT_QDMA_Data *qdma_data)
{
	QDMA_Mode_t dnStreamQosMode = qdma_data->qdma_private.mode ;

	if(dnStreamQosMode == QDMA_ENABLE) {
		dnstream_qos_enable = 1;
	} else {
		dnstream_qos_enable = 0;
	}
	
	return 0;
}

int qdma_api_get_downstream_qos_mode(struct ECNT_QDMA_Data *qdma_data)
{
	qdma_data->qdma_private.mode = dnstream_qos_enable;
	
	return 0;
}

