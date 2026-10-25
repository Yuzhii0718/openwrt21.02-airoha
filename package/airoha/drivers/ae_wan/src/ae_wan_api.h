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
#ifndef _AE_WAN_H_
#define _AE_WAN_H_

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include "ae_wan_ic_dis.h"
#include <asm/tc3162/tc3162.h>
#include <ecnt_hook/ecnt_hook_ae_mac_type.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef TCSUPPORT_CPU_ARMV8

#define regRead32(reg)				get_xsi_data(reg)
#define regWrite32(reg, wdata)		set_xsi_data(reg, wdata)


#define regRead32_for_fe(reg)				get_frame_engine_data(reg)
#define regWrite32_for_fe(reg, wdata)		set_frame_engine_data(reg, wdata)

#endif


#define IO_GREG(reg)							regRead32(reg)
#define IO_SREG(reg, value)						regWrite32(reg, value)
#define IO_GMASK(reg, mask, shift)				((regRead32(reg) & mask) >> shift)
#define IO_SMASK(reg, mask, shift, value)		{ uint32 t = regRead32(reg); regWrite32(reg, ((t&~(mask))|((value<<shift)&mask))); }
#define IO_SBITS(reg, bit)						{ uint32 t = regRead32(reg); regWrite32(reg, (t|bit)); }
#define IO_CBITS(reg, bit)						{ uint32 t = regRead32(reg); regWrite32(reg, (t&~(bit))); }

#define FE_SMASK(reg, mask, shift, value)		{ uint32 t = regRead32_for_fe(reg); regWrite32_for_fe(reg, ((t&~(mask<<shift))|((value&mask)<<shift))); }



#define xsiGetTxmbiStopSts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_IF_STS) & XSI_IF_STS_F_TXMBI_STOP_STS)
#define xsiSetTxmbiDisable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TXMBI_STOP))
#define xsiSetTxmbiEnable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TXMBI_STOP))

#define xsiGetTxmpiStopSts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_IF_STS) & XSI_IF_STS_F_TXMPI_STOP_STS)
#define xsiSetTxmpiDisable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TXMPI_STOP))
#define xsiSetTxmpiEnable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TXMPI_STOP))

#define xsiGetRxmbiStopSts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_IF_STS) & XSI_IF_STS_F_RXMBI_STOP_STS)
#define xsiSetRxmbiDisable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RXMBI_STOP))
#define xsiSetRxmbiEnable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RXMBI_STOP))

#define xsiGetRxmpiStopSts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_IF_STS) & XSI_IF_STS_F_RXMPI_STOP_STS)
#define xsiSetRxmpiDisable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RXMPI_STOP))
#define xsiSetRxmpiEnable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RXMPI_STOP))

#define xsiIsTxmpiIdle(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_IF_STS) & XSI_IF_STS_F_TXMPI_MASK_STS)
#define xsiSetTxmpiIdle(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TXMPI_MASK))
#define xsiSetTxmpiNormal(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TXMPI_MASK))

#define xsiIsRxFcOn(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_GLB_CFG) & XSI_GLB_CFG_F_RX_FC_EN)
#define xsiSetRxFcEnable(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RX_FC_EN))
#define xsiSetRxFcDisable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RX_FC_EN))

#define xsiIsTxFcOn(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_GLB_CFG) & XSI_GLB_CFG_F_TX_FC_EN)
#define xsiSetTxFcEnable(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TX_FC_EN))
#define xsiSetTxFcDisable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TX_FC_EN))

#define xsiIsRxPauseLenChkOn(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_GLB_CFG) & XSI_GLB_CFG_F_RX_PAUSE_LENCHK)
#define xsiSetRxPauseLenChkEnable(index)     (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RX_PAUSE_LENCHK))
#define xsiSetRxPauseLenChkDisable(index)        (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_RX_PAUSE_LENCHK))

#define xsiIsTxUseRxFaultRealTimeOn(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_GLB_CFG) & XSI_GLB_CFG_F_TX_USE_RXFAULT_REALTIME)
#define xsiSetTxUseRxFaultRealTimeEnable(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TX_USE_RXFAULT_REALTIME))
#define xsiSetTxUseRxFaultRealTimeDisable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_TX_USE_RXFAULT_REALTIME))

#define xsiIsFcsErrFwd(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_GLB_CFG) & XSI_GLB_CFG_F_FCS_ERR_FWD)
#define xsiSetFcsErrFwd(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_FCS_ERR_FWD))
#define xsiSetFcsErrDrop(index)      (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_FCS_ERR_FWD))


#define xsiSetIpgNum(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_GLB_CFG, XSI_GLB_CFG_F_IPG_NUM_MASK, XSI_GLB_CFG_F_IPG_NUM_SHIFT, value))
#define xsiGetIpgNum(index)      		(IO_GMASK(HSGMII_BASE_REG[index]+XSI_GLB_CFG, XSI_GLB_CFG_F_IPG_NUM_MASK, XSI_GLB_CFG_F_IPG_NUM_SHIFT))


#define xsiIsLpbkEn(index)        	 (IO_GREG(HSGMII_BASE_REG[index]+XSI_GLB_CFG) & XSI_GLB_CFG_F_XGMII_LPBK_EN_WHEN_TDC)
#define xsiSetLpbkEnable(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_XGMII_LPBK_EN_WHEN_TDC))
#define xsiSetLpbkDisable(index)      (IO_CBITS(HSGMII_BASE_REG[index]+XSI_GLB_CFG,XSI_GLB_CFG_F_XGMII_LPBK_EN_WHEN_TDC))


#define xsiSetTxFragLen(index,value)        (IO_SMASK(HSGMII_BASE_REG[index]+XSI_GLB_CFG, XSI_GLB_CFG_F_TX_FRAG_LEN_MASK, XSI_GLB_CFG_F_TX_FRAG_LEN_SHIFT, value))
#define xsiGetTxFragLen(index)      		(IO_GMASK(HSGMII_BASE_REG[index]+XSI_GLB_CFG, XSI_GLB_CFG_F_TX_FRAG_LEN_MASK, XSI_GLB_CFG_F_TX_FRAG_LEN_SHIFT))


#define xsiSetRxFragLen(index,value)        (IO_SMASK(HSGMII_BASE_REG[index]+XSI_GLB_CFG, XSI_GLB_CFG_F_RX_FRAG_LEN_MASK, XSI_GLB_CFG_F_RX_FRAG_LEN_SHIFT, value))
#define xsiGetRxFragLen(index)      		(IO_GMASK(HSGMII_BASE_REG[index]+XSI_GLB_CFG, XSI_GLB_CFG_F_RX_FRAG_LEN_MASK, XSI_GLB_CFG_F_RX_FRAG_LEN_SHIFT))



#define xsiClearAllInt(index)        (IO_SREG(HSGMII_BASE_REG[index]+XSI_INT_STS,0xFFFFFFFF))

#define xsiGetTxCrcInvusdIntSts(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_TX_DATAFIFO_OVRUN_INT)
#define xsiClrTxCrcInvusdIntSts(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_TX_DATAFIFO_OVRUN_INT))
#define xsiSetTxCrcInvusdIntEnable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_TX_DATAFIFO_OVRUN_EN))
#define xsiSetTxCrcInvusdIntDisable(index)       (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_TX_DATAFIFO_OVRUN_EN))


#define xsiGetTxFifoUnderRunIntSts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_TX_DATAFIFO_UNDRUN_INT)
#define xsiClrTxFifoUnderRunIntSts(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_TX_DATAFIFO_UNDRUN_INT))
#define xsiSetTxFifoUnderRunIntEnable(index)     (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_TX_DATAFIFO_UNDRUN_EN))
#define xsiSetTxFifoUnderRunIntDisable(index)        (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_TX_DATAFIFO_UNDRUN_EN))


#define xsiGetRxFifoOverRunIntSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_RX_DATAFIFO_OVRUN_INT)
#define xsiClrRxFifoOverRunIntSts(index)     (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_RX_DATAFIFO_OVRUN_INT))
#define xsiSetRxFifoOverRunIntEnable(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_DATAFIFO_OVRUN_EN))
#define xsiSetRxFifoOverRunIntDisable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_DATAFIFO_OVRUN_EN))


#define xsiGetTxCrcInvupdIntSts(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_RX_DATAFIFO_UNDRUN_INT)
#define xsiClrTxCrcInvupdIntSts(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_RX_DATAFIFO_UNDRUN_INT))
#define xsiSetTxCrcInvupdIntEnable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_DATAFIFO_UNDRUN_EN))
#define xsiSetTxCrcInvupdIntDisable(index)       (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_DATAFIFO_UNDRUN_EN))


#define xsiGetLocalFaultIntSts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_LOCAL_FAULT_INT)
#define xsiClrLocalFaultIntSts(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_LOCAL_FAULT_INT))
#define xsiSetLocalFaultIntEnable(index)     (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_LOCAL_FAULT_EN))
#define xsiSetLocalFaultIntDisable(index)        (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_LOCAL_FAULT_EN))


#define xsiGetRemoteFaultIntSts(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_REMOTE_FAULT_INT)
#define xsiClrRemoteFaultIntSts(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_REMOTE_FAULT_INT))
#define xsiSetRemoteFaultIntEnable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_REMOTE_FAULT_EN))
#define xsiSetRemoteFaultIntDisable(index)       (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_REMOTE_FAULT_EN))


#define xsiGetRxPauseFrameIntSts(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_RX_PAUSE_FRAME_INT)
#define xsiClrRxPauseFrameIntSts(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_RX_PAUSE_FRAME_INT))
#define xsiSetRxPauseFrameIntEnable(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_PAUSE_FRAME_EN))
#define xsiSetRxPauseFrameIntDisable(index)      (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_PAUSE_FRAME_EN))


#define xsiGetRxCrcErrIntSts(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_RX_CRC_ERROR_INT)
#define xsiClrRxCrcErrIntSts(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_RX_CRC_ERROR_INT))
#define xsiSetRxCrcErrIntEnable(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_CRC_ERROR_EN))
#define xsiSetRxCrcErrIntDisable(index)      (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_CRC_ERROR_EN))


#define xsiGetRxFragErrIntSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_RX_FRAG_ERROR_INT)
#define xsiClrRxFragErrIntSts(index)     (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_RX_FRAG_ERROR_INT))
#define xsiSetRxFragErrIntEnable(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_FRAG_ERROR_EN))
#define xsiSetRxFragErrIntDisable(index)     (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_FRAG_ERROR_EN))


#define xsiGetRxLenErrIntSts(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_RX_LENG_ERROR_INT)
#define xsiClrRxLenErrIntSts(index)      (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_RX_LENG_ERROR_INT))
#define xsiSetRxLenErrIntEnable(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_LENG_ERROR_EN))
#define xsiSetRxLenErrIntDisable(index)      (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_LENG_ERROR_EN))


#define xsiGetRxCodingErrIntSts(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_INT_STS) & XSI_INT_STS_F_RX_CODING_ERROR_INT)
#define xsiClrRxCodingErrIntSts(index)       (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_STS,XSI_INT_STS_F_RX_CODING_ERROR_INT))
#define xsiSetRxCodingErrIntEnable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_CODING_ERROR_EN))
#define xsiSetRxCodingErrIntDisable(index)       (IO_CBITS(HSGMII_BASE_REG[index]+XSI_INT_EN,XSI_INT_EN_F_RX_CODING_ERROR_EN))


#define xsimaclogicrst(index)		IO_SREG(HSGMII_BASE_REG[index]+XSI_MAC_LOGIC_RST,0)	
#define xsimaclogicrstenable(index)		IO_SREG(HSGMII_BASE_REG[index]+XSI_MAC_LOGIC_RST,1)	


#define xsiGetRxFcSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_FC_STS) & XSI_FC_STS_F_RX_FC_STS)
#define xsiGetTxFcSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_FC_STS) & XSI_FC_STS_F_TX_FC_STS)
#define xsiGetRxPauseSts(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_FC_STS) & XSI_FC_STS_F_RX_PAUSE_STS)
#define xsiGetFeQFullSts(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_FC_STS) & XSI_FC_STS_F_FE_Q_FULL_STS)
#define xsiGetTxPausePktType(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_FC_STS) & XSI_FC_STS_F_TX_PAUSE_PKTTYPE)


#define xsiGetTxPauseTimePkt(index)      (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TX_PAUSE_TIMER, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_PKT_MASK, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_PKT_SHIFT))
#define xsiSetTxPauseTimePkt(index,value)     (IO_SMASK(HSGMII_BASE_REG[index]+XSI_TX_PAUSE_TIMER, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_PKT_MASK, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_PKT_SHIFT, value))
#define xsiGetTxPauseTimeCal(index)      (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TX_PAUSE_TIMER, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_CAL_MASK, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_CAL_SHIFT))
#define xsiSetTxPauseTimeCal(index,value)     (IO_SMASK(HSGMII_BASE_REG[index]+XSI_TX_PAUSE_TIMER, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_CAL_MASK, XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_CAL_SHIFT, value))


#define xsiGetTxPauseQuantaCyc(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_QUANTA_CYCLE, XSI_QUANTA_CYCLE_F_PAUSE_QUANTA_CYC_MASK, XSI_QUANTA_CYCLE_F_PAUSE_QUANTA_CYC_SHIFT))
#define xsiSetTxPauseQuantaCyc(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_QUANTA_CYCLE, XSI_QUANTA_CYCLE_F_PAUSE_QUANTA_CYC_MASK, XSI_QUANTA_CYCLE_F_PAUSE_QUANTA_CYC_SHIFT, value))


#define xsiGetTxdFifoAEmptyThr(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXDFIFO_THR, XSI_TXDFIFO_THR_F_TXDFIFO_AEMPTY_THR_MASK, XSI_TXDFIFO_THR_F_TXDFIFO_AEMPTY_THR_SHIFT))
#define xsiSetTxdFifoAEmptyThr(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_TXDFIFO_THR, XSI_TXDFIFO_THR_F_TXDFIFO_AEMPTY_THR_MASK, XSI_TXDFIFO_THR_F_TXDFIFO_AEMPTY_THR_SHIFT, value))
#define xsiGetTxdFifoAFullThr(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXDFIFO_THR, XSI_TXDFIFO_THR_F_TXDFIFO_AFULL_THR_MASK, XSI_TXDFIFO_THR_F_TXDFIFO_AFULL_THR_SHIFT))
#define xsiSetTxdFifoAFullThr(index,value)        (IO_SMASK(HSGMII_BASE_REG[index]+XSI_TXDFIFO_THR, XSI_TXDFIFO_THR_F_TXDFIFO_AFULL_THR_MASK, XSI_TXDFIFO_THR_F_TXDFIFO_AFULL_THR_SHIFT, value))


#define xsiGetRxdFifoAEmptyThr(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RXDFIFO_THR, XSI_RXDFIFO_THR_F_RXDFIFO_AEMPTY_THR_MASK, XSI_RXDFIFO_THR_F_RXDFIFO_AEMPTY_THR_SHIFT))
#define xsiSetRxdFifoAEmptyThr(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_RXDFIFO_THR, XSI_RXDFIFO_THR_F_RXDFIFO_AEMPTY_THR_MASK, XSI_RXDFIFO_THR_F_RXDFIFO_AEMPTY_THR_SHIFT, value))


#define xsiGetTxhFifoAEmptyThr(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR, XSI_TXHFIFO_THR_F_TXHFIFO_AEMPTY_THR_MASK, XSI_TXHFIFO_THR_F_TXHFIFO_AEMPTY_THR_SHIFT))
#define xsiSetTxhFifoAEmptyThr(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR, XSI_TXHFIFO_THR_F_TXHFIFO_AEMPTY_THR_MASK, XSI_TXHFIFO_THR_F_TXHFIFO_AEMPTY_THR_SHIFT, value))
#define xsiGetTxhFifoAFullThr(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR, XSI_TXHFIFO_THR_F_TXHFIFO_AFULL_THR_MASK, XSI_TXHFIFO_THR_F_TXHFIFO_AFULL_THR_SHIFT))
#define xsiSetTxhFifoAFullThr(index,value)        (IO_SMASK(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR, XSI_TXHFIFO_THR_F_TXHFIFO_AFULL_THR_MASK, XSI_TXHFIFO_THR_F_TXHFIFO_AFULL_THR_SHIFT, value))
#define xsiGetRealTxhFifoAEmptyThr(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR, XSI_TXHFIFO_THR_F_CF_TXHFIFO_AEMPTY_THR_MASK, XSI_TXHFIFO_THR_F_CF_TXHFIFO_AEMPTY_THR_SHIFT))
#define xsiGetRealTxhFifoAFullThr(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR, XSI_TXHFIFO_THR_F_CF_TXHFIFO_AFULL_THR_MASK, XSI_TXHFIFO_THR_F_CF_TXHFIFO_AFULL_THR_SHIFT))
#define xsiGetTxhFifoThrTrigSts(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR) & XSI_TXHFIFO_THR_F_TXHFIFO_THR_TRIG)
#define xsiSetTxhFifoThrTrigEnable(index)        (IO_SBITS(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR,XSI_TXHFIFO_THR_F_TXHFIFO_THR_TRIG))
#define xsiSetTxhFifoThrTrigDisable(index)       (IO_CBITS(HSGMII_BASE_REG[index]+XSI_TXHFIFO_THR,XSI_TXHFIFO_THR_F_TXHFIFO_THR_TRIG))

#define xsiGetRxdFifoWfullSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_RXDFIFO_WFULL)
#define xsiGetRxdFifoRemptySts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_RXDFIFO_REMPTY)
#define xsiGetTxdFifoWfullSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_TXDFIFO_WFULL)
#define xsiGetTxdFifoRemptySts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_TXDFIFO_REMPTY)
#define xsiGetTxhFifoWfullSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_TXHFIFO_WFULL)
#define xsiGetTxhFifoWAlmostFullSts(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_TXHFIFO_WALMOST_FULL)
#define xsiGetTxhFifoRemptySts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_TXHFIFO_REMPTY)
#define xsiGetTxhFifoRAlmostEmptySts(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_FIFO_STS) & XSI_FIFO_STS_F_TXHFIFO_RALMOST_EMPTY)

#define xsiGetMacAddrH(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_MACADDRH, XSI_MACADDRH_F_XSI_MACADDRH_MASK, XSI_MACADDRH_F_XSI_MACADDRH_SHIFT))
#define xsiSetMacAddrH(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_MACADDRH, XSI_MACADDRH_F_XSI_MACADDRH_MASK, XSI_MACADDRH_F_XSI_MACADDRH_SHIFT, value))
#define xsiGetMacAddrL(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_MACADDRL))
#define xsiSetMacAddrL(index,value)       (IO_SREG(HSGMII_BASE_REG[index]+XSI_MACADDRL, value))

#define xsiGetRxLocalFaultSts(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_FAULT_STS) & XSI_FAULT_STS_F_RX_LOCAL_FAULT_STS)
#define xsiGetRxRemoteFaultSts(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_FAULT_STS) & XSI_FAULT_STS_F_RX_REMOTE_FAULT_STS)

#define xsiGetProbeSel(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_PRB_SEL, XSI_PRB_SEL_F_PROBE_SEL_MASK, XSI_PRB_SEL_F_PROBE_SEL_SHIFT))
#define xsiSetProbeSel(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_PRB_SEL, XSI_PRB_SEL_F_PROBE_SEL_MASK, XSI_PRB_SEL_F_PROBE_SEL_SHIFT, value))
#define xsiGetProbeBit0Sel(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_PRB_SEL, XSI_PRB_SEL_F_PROBE_BIT0_SEL_MASK, XSI_PRB_SEL_F_PROBE_BIT0_SEL_SHIFT))
#define xsiSetProbeBit0Sel(index,value)       (IO_SMASK(HSGMII_BASE_REG[index]+XSI_PRB_SEL, XSI_PRB_SEL_F_PROBE_BIT0_SEL_MASK, XSI_PRB_SEL_F_PROBE_BIT0_SEL_SHIFT, value))
#define xsiGetProbeDtgrpSel(index)       (IO_GMASK(HSGMII_BASE_REG[index]+XSI_PRB_SEL, XSI_PRB_SEL_F_PROBE_DTGRP_SEL_MASK, XSI_PRB_SEL_F_PROBE_DTGRP_SEL_SHIFT))
#define xsiSetProbeDtgrpSel(index,value)      (IO_SMASK(HSGMII_BASE_REG[index]+XSI_PRB_SEL, XSI_PRB_SEL_F_PROBE_DTGRP_SEL_MASK, XSI_PRB_SEL_F_PROBE_DTGRP_SEL_SHIFT, value))

#define xsiGetProbeOutH(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_PRB_OUTH))
#define xsiGetProbeOutL(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_PRB_OUTL))

#define xsiClearAllCnt(index)     (IO_SBITS(HSGMII_BASE_REG[index]+XSI_CNT_CLR,XSI_CNT_CLR_F_GLB_CNTCLR))

#define xsiGetTxBytes(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_TX_OCTETS_CNT))
#define xsiGetTxPktCnt(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_TX_PKT_CNT))
#define xsiGetTxMbiEthCnt(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_TXMBI_ETH_CNT))
#define xsiGetTxMbiUcEthCnt(index)       (IO_GREG(HSGMII_BASE_REG[index]+XSI_TXMBI_UCETH_CNT))
#define xsiGetTxMbiBcEthCnt(index)       (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXMBI_MCETH_CNT, XSI_TXMBI_MCETH_CNT_F_TXMBI_BCETH_CNT_MASK, XSI_TXMBI_MCETH_CNT_F_TXMBI_BCETH_CNT_SHIFT))
#define xsiGetTxMbiMcEthCnt(index)       (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXMBI_MCETH_CNT, XSI_TXMBI_MCETH_CNT_F_TXMBI_MCETH_CNT_MASK, XSI_TXMBI_MCETH_CNT_F_TXMBI_MCETH_CNT_SHIFT))
#define xsiGetTxMbiPauseOnCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXMBI_PAUSE_CNT, XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEON_CNT_MASK, XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEON_CNT_SHIFT))
#define xsiGetTxMbiPauseOffCnt(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_TXMBI_PAUSE_CNT, XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEOFF_CNT_MASK, XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEOFF_CNT_SHIFT))


#define xsiGetTxSofCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XGMII_TX_SOF_EOF_CNT, XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_SOF_CNT_MASK, XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_SOF_CNT_SHIFT))
#define xsiGetTxEofCnt(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XGMII_TX_SOF_EOF_CNT, XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_EOF_CNT_MASK, XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_EOF_CNT_SHIFT))

#define xsiGetTxByteCnt(index)      	(IO_GREG(HSGMII_BASE_REG[index]+XGMII_TX_BYTES_CNT))
#define xsiGetTxNormalByteCnt(index)      (IO_GREG(HSGMII_BASE_REG[index]+TX_NORMAL_PKT_BYTES_CNT))

#define xsiGetTxDeqCheckCnt1(index)      	(IO_GREG(HSGMII_BASE_REG[index]+TX_DEQ_CHECK_CNT1))
#define xsiGetTxDeqCheckCnt2(index)      	(IO_GREG(HSGMII_BASE_REG[index]+TX_DEQ_CHECK_CNT2))



#define xsiGetRxFrameCnt(index)      (IO_GREG(HSGMII_BASE_REG[index]+XSI_RX_FRAME_CNT))
#define xsiGetRxBytes(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_RX_OCTETS_CNT))
#define xsiGetRxPktCnt(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_RX_PKT_CNT))
#define xsiGetRxEthCnt(index)        (IO_GREG(HSGMII_BASE_REG[index]+XSI_RX_ETH_CNT))
#define xsiGetRxPauseOnCnt(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RX_PAUSE_CNT, XSI_RX_PAUSE_CNT_F_RX_PAUSEON_CNT_MASK, XSI_RX_PAUSE_CNT_F_RX_PAUSEON_CNT_SHIFT))
#define xsiGetRxPauseOffCnt(index)       (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RX_PAUSE_CNT, XSI_RX_PAUSE_CNT_F_RX_PAUSEOFF_CNT_MASK, XSI_RX_PAUSE_CNT_F_RX_PAUSEOFF_CNT_SHIFT))
#define xsiGetRxLenErrCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RX_LENERR_CNT, XSI_RX_LENERR_CNT_F_RX_LENERR_CNT_MASK, XSI_RX_LENERR_CNT_F_RX_LENERR_CNT_SHIFT))
#define xsiGetRxFragErrCnt(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RX_LENERR_CNT, XSI_RX_LENERR_CNT_F_RX_FRAGERR_CNT_MASK, XSI_RX_LENERR_CNT_F_RX_FRAGERR_CNT_SHIFT))
#define xsiGetRxCrcErrCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RX_CRCERR_CNT, XSI_RX_CRCERR_CNT_F_RX_CRCERR_CNT_MASK, XSI_RX_CRCERR_CNT_F_RX_CRCERR_CNT_SHIFT))
#define xsiGetRxCodingErrCnt(index)      (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RX_CRCERR_CNT, XSI_RX_CRCERR_CNT_F_RX_CODINGERR_CNT_MASK, XSI_RX_CRCERR_CNT_F_RX_CODINGERR_CNT_SHIFT))
#define xsiGetRxMbiPktCnt(index)     (IO_GREG(HSGMII_BASE_REG[index]+XSI_RXMBI_PKT_CNT))
#define xsiGetRxMbiErrDropCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RXMBI_DROP_CNT, XSI_RXMBI_DROP_CNT_F_RXMBI_ERRDROP_CNT_MASK, XSI_RXMBI_DROP_CNT_F_RXMBI_ERRDROP_CNT_SHIFT))
#define xsiGetRxMbiSofDropCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XSI_RXMBI_DROP_CNT, XSI_RXMBI_DROP_CNT_F_RXMBI_SOFDROP_CNT_MASK, XSI_RXMBI_DROP_CNT_F_RXMBI_SOFDROP_CNT_SHIFT))




#define xsiGetRxSofCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+XGMII_RX_SOF_EOF_CNT, XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_SOF_CNT_MASK, XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_SOF_CNT_SHIFT))
#define xsiGetRxEofCnt(index)        (IO_GMASK(HSGMII_BASE_REG[index]+XGMII_RX_SOF_EOF_CNT, XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_EOF_CNT_MASK, XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_EOF_CNT_SHIFT))

#define xsiGetRxMpiSopCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+RX_MPI_SOP_EOP_CNT, RX_MPI_SOP_EOP_CNT_F_RX_MPI_SOP_CNT_MASK, RX_MPI_SOP_EOP_CNT_F_RX_MPI_SOP_CNT_SHIFT))
#define xsiGetRxMpiEopCnt(index)        (IO_GMASK(HSGMII_BASE_REG[index]+RX_MPI_SOP_EOP_CNT, RX_MPI_SOP_EOP_CNT_F_RX_MPI_EOP_CNT_MASK, RX_MPI_SOP_EOP_CNT_F_RX_MPI_EOP_CNT_SHIFT))


#define xsiGetRxNormalByteCnt(index)      	(IO_GREG(HSGMII_BASE_REG[index]+RX_NORMAL_PKT_BYTES_CNT))

#define xsiGetRxEnqCheckCnt1(index)      	(IO_GREG(HSGMII_BASE_REG[index]+RX_ENQ_CHECK_CNT1))
#define xsiGetRxEnqCheckCnt2(index)      	(IO_GREG(HSGMII_BASE_REG[index]+RX_ENQ_CHECK_CNT2))

#define xsiGetRxMbiSopCnt(index)     (IO_GMASK(HSGMII_BASE_REG[index]+RX_MBI_SOP_EOP_CNT, RX_MBI_SOP_EOP_CNT_F_RX_MBI_SOP_CNT_MASK, RX_MBI_SOP_EOP_CNT_F_RX_MBI_SOP_CNT_SHIFT))
#define xsiGetRxMbiEopCnt(index)        (IO_GMASK(HSGMII_BASE_REG[index]+RX_MBI_SOP_EOP_CNT, RX_MBI_SOP_EOP_CNT_F_RX_MBI_EOP_CNT_MASK, RX_MBI_SOP_EOP_CNT_F_RX_MBI_EOP_CNT_SHIFT))

#define xsiGetRxUcDropCnt(index)      	(IO_GREG(HSGMII_BASE_REG[index]+RX_UC_DROP_CNT))
#define xsiGetRxBcDropCnt(index)      	(IO_GREG(HSGMII_BASE_REG[index]+RX_BC_DROP_CNT))
#define xsiGetRxMcDropCnt(index)      	(IO_GREG(HSGMII_BASE_REG[index]+RX_MC_DROP_CNT))
#define xsiGetRxTotalDropCnt(index)      	(IO_GREG(HSGMII_BASE_REG[index]+RX_TOTAL_DROP_CNT))

#define xsiSetRxParserID(index,value)				(IO_SREG(HSGMII_BASE_REG[index]+RX_PARSER_ID, value))
#define xsiSetRxParserRXCCoding(index,value)		(IO_SREG(HSGMII_BASE_REG[index]+RX_PARSER_RXC_CODING, value))
#define xsiSetRxParserRXDCodingL(index,value)		(IO_SREG(HSGMII_BASE_REG[index]+RX_PARSER_RXD_CODING_L, value))
#define xsiSetRxParserRXDCodingH(index,value)		(IO_SREG(HSGMII_BASE_REG[index]+RX_PARSER_RXD_CODING_H, value))
#define xsiSetRxParserEn(index,value)				(IO_SREG(HSGMII_BASE_REG[index]+RX_PARSER_EN, value))
#define xsiSetRxLpiChkEn(index,value)				(IO_SREG(HSGMII_BASE_REG[index]+RX_LPI_CHK_EN, value))
#define xsiGetRxLpiOn(index)				(IO_GREG(HSGMII_BASE_REG[index]+RX_LPI_ON))
#define xsiGetRxLpiEventCnt(index)				(IO_GREG(HSGMII_BASE_REG[index]+RX_LPI_EVENT_CNT))
#define xsiGetRxLpiDurationCnt(index)				(IO_GREG(HSGMII_BASE_REG[index]+RX_LPI_DURATION_CNT))

#define xsiSetTxLpiGen(index,value)      		(IO_SREG(HSGMII_BASE_REG[index]+TX_LPI_GEN_EN, value))
#define xsiSetTxLpiTimeoutThr(index,value) 			(IO_SREG(HSGMII_BASE_REG[index]+TX_LPI_TIMEOUT_CNT_THR, value))
#define xsiGetTxLpiOn(index)				(IO_GREG(HSGMII_BASE_REG[index]+TX_LPI_ON))
#define xsiGetTxLpiEventCnt(index)				(IO_GREG(HSGMII_BASE_REG[index]+TX_LPI_EVENT_CNT))
#define xsiGetTxLpiDurationCnt(index)				(IO_GREG(HSGMII_BASE_REG[index]+TX_LPI_DURATION_CNT))

int xsi_mac_set_lpi(uint hsgmii_index, uint enable, uint direction);
int xsi_mac_get_lpi(uint hsgmii_index,  uint direction);


#define xsiWanSetGDM2FWDFQ(index,value)     IO_SREG(GDM2_FWD_FQ,value)
#define xsiWanSetGDM2IFCDisable(index)     (IO_CBITS(FE_IFC_CFG,P2_IFC_EN))


#define CSR_SP_DFT_CPORT(sp)	(SP_DFT_CPORT_0+(sp>>3)*4)
#define SP_DFT_CPORT_MASK		(0xF)
#define SP_DFT_CPORT_SHIFT(sp)	((sp&0x7)*4)
#define XSI_SET_SP_DFT_CPORT(sp, value)	(FE_SMASK(CSR_SP_DFT_CPORT(sp), SP_DFT_CPORT_MASK, SP_DFT_CPORT_SHIFT(sp), value))


int xsi_api_set_mac(uint hsgmii_index,uint8 * mac_add) ;
void xsi_mac_itf_reset(uint hsgmii_index);
void xsi_mac_itf_close(uint hsgmii_index);


#define HSGMII_RC_CFG_METER_DISABLE	(0x0)
#define HSGMII_RC_CFG_METER_ENABLE	(0x1)
#define HSGMII_RC_CFG_BYTE_MODE		(0x0)
#define HSGMII_RC_CFG_PKT_MODE		(0x1)
#define HSGMII_RC_CFG_FAST_TICK		(0x0)
#define HSGMII_RC_CFG_SLOW_TICK		(0x1)

#define HSGMII_RC_CFG_PARA_MISC		(0x0)
#define HSGMII_RC_CFG_PARA_TOKEN_RATE	(0x1)
#define HSGMII_RC_CFG_PARA_BUCK_SHIFT	(0x2)
#define HSGMII_RC_CFG_PARA_BUCK_CNT	(0x3)
#define HSGMII_RC_CFG_EN				(1<<31)
#define HSGMII_RC_CFG_PARA_TYPE_SHIFT	(28)
#define HSGMII_RC_CFG_PARA_TYPE_MASK	(0x3<<HSGMII_RC_CFG_PARA_TYPE_SHIFT)
#define HSGMII_RC_CFG_ID_SHIFT			(16)
#define HSGMII_RC_CFG_ID_MASK			(0x7<<HSGMII_RC_CFG_ID_SHIFT)
#define HSGMII_RC_CFG_PARA_RWCMD		(1<<0)


#define HSGMII_RC_CFG_PARA_METER_EN	(1<<2)
#define HSGMII_RC_CFG_PARA_PPS_MODE	(1<<1)
#define HSGMII_RC_CFG_PARA_TICK_SEL	(1<<0)

#define RC_TOKEN_RATE_INTEGER_SHIFT		(6)
#define RC_TOKEN_RATE_INTEGER_MASK		(0x3FFFF<<RC_TOKEN_RATE_INTEGER_SHIFT)
#define RC_TOKEN_RATE_FRACTION_MASK		(0x3F)

#define RC_PKT_MODE_BUCKET_SHIFT		(0)
#define RC_BYTE_MODE_BUCKET_SHIFT		(10)

#define HSGMII_RX_UC_RATE 	0
#define HSGMII_RX_BC_RATE 	1
#define HSGMII_RX_MC_RATE 	2
#define HSGMII_RX_TOTAL_RATE 3
#define HSGMII_TX_TOTAL_RATE 4

//int xsi_mac_api_get_phy_link_status(ECNT_XSI_Data_s *xsi_data);
//int xsi_mac_api_get_phy_mode(ECNT_XSI_Data_s *xsi_data);
//int xsi_mac_api_set_phy_mode(ECNT_XSI_Data_s *xsi_data);


int xsi_mac_api_set_rxuc_rate(ECNT_AE_WAN_Data_s *xsi_data);
int xsi_mac_api_set_rxbc_rate(ECNT_AE_WAN_Data_s *xsi_data);
int xsi_mac_api_set_rxmc_rate(ECNT_AE_WAN_Data_s *xsi_data);
int xsi_mac_api_set_rxtotal_rate(ECNT_AE_WAN_Data_s *xsi_data);


#if SUPPORT_AE_WAN_NEW_ARCH
int xsi_mac_api_set_xsgmii_phy_speed(uint hsgmii_index , uint xsgmii_speed, uint mod ,uint an );
int xsi_set_xsgmii_serdes_speed_forcemode_andisable(uint hsgmii_index , uint xsgmii_speed );
int xsi_set_xsgmii_serdes_speed_forcemode_anauto(uint hsgmii_index , uint xsgmii_speed );
int xsi_set_xsgmii_serdes_enable_interrupt(uint hsgmii_index);
int xsi_set_mdio(void);

//bit 0 an done
//bit1 sync up
unsigned int xsi_mac_api_set_xsgmii_phy_linkstats(uint hsgmii_index, uint linkstats);
unsigned int xsi_mac_api_get_xsgmii_phy_linkstats(uint hsgmii_index);
int xsi_mac_api_reset(int hsgmii_index);
int xsi_mac_api_logic_reset(int hsgmii_index);
int xsi_mac_api_set_tx_frag(uint hsgmii_index);
int xsi_mac_api_set_rx_frag(uint hsgmii_index);
int xsi_mac_api_set_pcie_tx_frag(uint hsgmii_index, int frag_size);
int xsi_mac_api_set_pcie_rx_frag(uint hsgmii_index, int frag_size);

#endif

#endif

