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
#ifndef _XSI_REG_7523_H_
#define _XSI_REG_7523_H_

#define XSI_AE_BASE		0xbfa60000
#define	XSI_PCIE0_BASE	0xbfa70000
#define	XSI_PCIE1_BASE	0xbfa71000
#define	XSI_USB_BASE	0xbfa80000

#define XSI_AE_IDX		3
#define	XSI_PCIE0_IDX	0
#define	XSI_PCIE1_IDX	1
#define	XSI_USB_IDX		2

#define PON_SEL_GDMA 		FE_GDM_SEL_GDMA2
#define ETH_SEL_GDMA		FE_GDM_SEL_GDMA3
#define PCIE0_SEL_GDMA		FE_GDM_SEL_GDMA3
#define PCIE1_SEL_GDMA		FE_GDM_SEL_GDMA3
#define USB_SEL_GDMA		FE_GDM_SEL_GDMA3

#define XSI_IDX_MAX		XSI_AE_IDX

#define HSGMII_LAN_PCIE0_SRCPORT	0x16
#define HSGMII_LAN_PCIE1_SRCPORT	0x17
#define HSGMII_LAN_USB_SRCPORT		0x18
#define HSGMII_LAN_ETH_SRCPORT		0xffff

extern unsigned int HSGMII_BASE_REG[4];

//============================================================================
#define XSI_GLB_CFG (0x0)
#define XSI_GLB_CFG_F_TXMBI_STOP (1<<0)
#define XSI_GLB_CFG_F_TXMPI_STOP (1<<1)
#define XSI_GLB_CFG_F_RXMBI_STOP (1<<2)
#define XSI_GLB_CFG_F_RXMPI_STOP (1<<3)
#define XSI_GLB_CFG_F_RX_FC_EN (1<<4)
#define XSI_GLB_CFG_F_TX_FC_EN (1<<5)
#define XSI_GLB_CFG_F_RX_PAUSE_LENCHK (1<<6)
#define XSI_GLB_CFG_F_TX_USE_RXFAULT_REALTIME (1<<7)
#define XSI_GLB_CFG_F_FCS_ERR_FWD (1<<8)
#define XSI_GLB_CFG_F_TXMPI_MASK (1<<9)
#define XSI_GLB_CFG_F_IPG_NUM_SHIFT (10)
#define XSI_GLB_CFG_F_IPG_NUM_MASK (0xFC00)
#define XSI_GLB_CFG_F_XGMII_LPBK_EN_WHEN_TDC (1<<16)
#define XSI_GLB_CFG_F_TX_FRAG_LEN_SHIFT (17)
#define XSI_GLB_CFG_F_TX_FRAG_LEN_MASK (0x3E0000)
#define XSI_GLB_CFG_F_RX_FRAG_LEN_SHIFT (22)
#define XSI_GLB_CFG_F_RX_FRAG_LEN_MASK (0x7C00000)
#define XSI_GLB_CFG_F_MEM_POWER_DOWM (1<<27)

//============================================================================
#define XSI_INT_STS (0x4)
#define XSI_INT_STS_F_TX_DATAFIFO_OVRUN_INT (1<<0)
#define XSI_INT_STS_F_TX_DATAFIFO_UNDRUN_INT (1<<1)
#define XSI_INT_STS_F_RX_DATAFIFO_OVRUN_INT (1<<2)
#define XSI_INT_STS_F_RX_DATAFIFO_UNDRUN_INT (1<<3)
#define XSI_INT_STS_F_LOCAL_FAULT_INT (1<<4)
#define XSI_INT_STS_F_REMOTE_FAULT_INT (1<<5)
#define XSI_INT_STS_F_RX_PAUSE_FRAME_INT (1<<6)
#define XSI_INT_STS_F_RX_CRC_ERROR_INT (1<<7)
#define XSI_INT_STS_F_RX_FRAG_ERROR_INT (1<<8)
#define XSI_INT_STS_F_RX_LENG_ERROR_INT (1<<9)
#define XSI_INT_STS_F_RX_CODING_ERROR_INT (1<<10)

//============================================================================
#define XSI_INT_EN (0x8)
#define XSI_INT_EN_F_TX_DATAFIFO_OVRUN_EN (1<<0)
#define XSI_INT_EN_F_TX_DATAFIFO_UNDRUN_EN (1<<1)
#define XSI_INT_EN_F_RX_DATAFIFO_OVRUN_EN (1<<2)
#define XSI_INT_EN_F_RX_DATAFIFO_UNDRUN_EN (1<<3)
#define XSI_INT_EN_F_LOCAL_FAULT_EN (1<<4)
#define XSI_INT_EN_F_REMOTE_FAULT_EN (1<<5)
#define XSI_INT_EN_F_RX_PAUSE_FRAME_EN (1<<6)
#define XSI_INT_EN_F_RX_CRC_ERROR_EN (1<<7)
#define XSI_INT_EN_F_RX_FRAG_ERROR_EN (1<<8)
#define XSI_INT_EN_F_RX_LENG_ERROR_EN (1<<9)
#define XSI_INT_EN_F_RX_CODING_ERROR_EN (1<<10)

//============================================================================
#define XSI_MAC_LOGIC_RST (0x10)
#define XSI_MAC_LOGIC_RST_F_XSI_MAC_LOGIC_RST (1<<0)

//============================================================================
#define XSI_FC_STS (0x20)
#define XSI_FC_STS_F_RX_FC_STS (1<<0)
#define XSI_FC_STS_F_TX_FC_STS (1<<1)
#define XSI_FC_STS_F_RX_PAUSE_STS (1<<2)
#define XSI_FC_STS_F_FE_Q_FULL_STS (1<<3)
#define XSI_FC_STS_F_TX_PAUSE_PKTTYPE (1<<4)

//============================================================================
#define XSI_TX_PAUSE_TIMER (0x24)
#define XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_PKT_SHIFT (0)
#define XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_PKT_MASK (0xFFFF)
#define XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_CAL_SHIFT (16)
#define XSI_TX_PAUSE_TIMER_F_TX_PAUSE_TIME_CAL_MASK (0xFFFF0000)

//============================================================================
#define XSI_QUANTA_CYCLE (0x28)
#define XSI_QUANTA_CYCLE_F_PAUSE_QUANTA_CYC_SHIFT (0)
#define XSI_QUANTA_CYCLE_F_PAUSE_QUANTA_CYC_MASK (0x3F)

//============================================================================
#define XSI_TXDFIFO_THR (0x40)
#define XSI_TXDFIFO_THR_F_TXDFIFO_AEMPTY_THR_SHIFT (0)
#define XSI_TXDFIFO_THR_F_TXDFIFO_AEMPTY_THR_MASK (0xFF)
#define XSI_TXDFIFO_THR_F_TXDFIFO_AFULL_THR_SHIFT (16)
#define XSI_TXDFIFO_THR_F_TXDFIFO_AFULL_THR_MASK (0x7FF0000)

//============================================================================
#define XSI_RXDFIFO_THR (0x44)
#define XSI_RXDFIFO_THR_F_RXDFIFO_AEMPTY_THR_SHIFT (0)
#define XSI_RXDFIFO_THR_F_RXDFIFO_AEMPTY_THR_MASK (0xFF)

//============================================================================
#define XSI_TXHFIFO_THR (0x48)
#define XSI_TXHFIFO_THR_F_TXHFIFO_AFULL_THR_SHIFT (0)
#define XSI_TXHFIFO_THR_F_TXHFIFO_AFULL_THR_MASK (0x1F)
#define XSI_TXHFIFO_THR_F_TXHFIFO_AEMPTY_THR_SHIFT (8)
#define XSI_TXHFIFO_THR_F_TXHFIFO_AEMPTY_THR_MASK (0x1F00)
#define XSI_TXHFIFO_THR_F_CF_TXHFIFO_AFULL_THR_SHIFT (16)
#define XSI_TXHFIFO_THR_F_CF_TXHFIFO_AFULL_THR_MASK (0x1F0000)
#define XSI_TXHFIFO_THR_F_CF_TXHFIFO_AEMPTY_THR_SHIFT (24)
#define XSI_TXHFIFO_THR_F_CF_TXHFIFO_AEMPTY_THR_MASK (0x1F000000)
#define XSI_TXHFIFO_THR_F_TXHFIFO_THR_TRIG (1<<31)

//============================================================================
#define XSI_FIFO_STS (0x4C)
#define XSI_FIFO_STS_F_RXDFIFO_WFULL (1<<0)
#define XSI_FIFO_STS_F_RXDFIFO_REMPTY (1<<1)
#define XSI_FIFO_STS_F_TXDFIFO_WFULL (1<<2)
#define XSI_FIFO_STS_F_TXDFIFO_REMPTY (1<<3)
#define XSI_FIFO_STS_F_TXHFIFO_WFULL (1<<4)
#define XSI_FIFO_STS_F_TXHFIFO_WALMOST_FULL (1<<5)
#define XSI_FIFO_STS_F_TXHFIFO_REMPTY (1<<6)
#define XSI_FIFO_STS_F_TXHFIFO_RALMOST_EMPTY (1<<7)

//============================================================================
#define XSI_MACADDRH (0x60)
#define XSI_MACADDRH_F_XSI_MACADDRH_SHIFT (0)
#define XSI_MACADDRH_F_XSI_MACADDRH_MASK (0xFFFF)

//============================================================================
#define XSI_MACADDRL (0x64)


//============================================================================
#define XSI_FAULT_STS (0x80)
#define XSI_FAULT_STS_F_RX_LOCAL_FAULT_STS (1<<0)
#define XSI_FAULT_STS_F_RX_REMOTE_FAULT_STS (1<<1)

//============================================================================
#define RX_RC_CFG (0xA0)
#define RX_RC_CFG_F_RX_RC_EN (1<<31)
#define RX_RC_CFG_F_RX_RATELIMIT_L2_EN (1<<30)
#define RX_RC_CFG_F_RX_SLOW_TICK_RATIO_SHIFT (16)
#define RX_RC_CFG_F_RX_SLOW_TICK_RATIO_MASK (0x3FFF0000)
#define RX_RC_CFG_F_RX_FAST_TICK_SHIFT (0)
#define RX_RC_CFG_F_RX_FAST_TICK_MASK (0xFFFF)

//============================================================================
#define RX_CFG (0xA4)
#define RX_CFG_F_RC_CFG_EN (1<<31)
#define RX_CFG_F_RC_CFG_PARAM_SHIFT (28)
#define RX_CFG_F_RC_CFG_PARAM_MASK (0x30000000)
#define RX_CFG_F_RC_CFG_ID_SHIFT (16)
#define RX_CFG_F_RC_CFG_ID_MASK (0x30000)
#define RX_CFG_F_RX_CFG_WR (1<<0)

//============================================================================
#define RC_DATA_L (0xA8)


//============================================================================
#define RC_DATA_H (0xAC)

//============================================================================
#define RC_RD_DATA_L RC_DATA_L

//============================================================================
#define RC_RD_DATA_H RC_DATA_H

//============================================================================
#define RC_WR_DATA_L RC_DATA_L

//============================================================================
#define RC_WR_DATA_H RC_DATA_H

//============================================================================
#define TOTAL_RX_RC_CFG (0xB0)
#define TOTAL_RX_RC_CFG_F_TOTAL_RX_RC_EN (1<<31)
#define TOTAL_RX_RC_CFG_F_TOTAL_RX_RATELIMIT_L2_EN (1<<30)
#define TOTAL_RX_RC_CFG_F_TOTAL_RX_SLOW_TICK_RATIO_SHIFT (16)
#define TOTAL_RX_RC_CFG_F_TOTAL_RX_SLOW_TICK_RATIO_MASK (0x3FFF0000)
#define TOTAL_RX_RC_CFG_F_TOTAL_RX_FAST_TICK_SHIFT (0)
#define TOTAL_RX_RC_CFG_F_TOTAL_RX_FAST_TICK_MASK (0xFFFF)

//============================================================================
#define TOTAL_RX_CFG (0xB4)
#define TOTAL_RX_CFG_F_TOTAL_RC_CFG_EN (1<<31)
#define TOTAL_RX_CFG_F_TOTAL_RC_CFG_PARAM_SHIFT (28)
#define TOTAL_RX_CFG_F_TOTAL_RC_CFG_PARAM_MASK (0x30000000)
#define TOTAL_RX_CFG_F_TOTAL_RC_CFG_ID (1<<16)
#define TOTAL_RX_CFG_F_TOTAL_RX_CFG_WR (1<<0)

//============================================================================
#define TOTAL_RC_DATA_L (0xB8)


//============================================================================
#define TOTAL_RC_DATA_H (0xBC)

//============================================================================
#define TOTAL_RC_RD_DATA_L TOTAL_RC_DATA_L

//============================================================================
#define TOTAL_RC_RD_DATA_H TOTAL_RC_DATA_H

//============================================================================
#define TOTAL_RC_WR_DATA_L TOTAL_RC_DATA_L

//============================================================================
#define TOTAL_RC_WR_DATA_H TOTAL_RC_DATA_H

#define TOTAL_TX_RC_CFG RX_RC_CFG

#define TOTAL_TX_CFG RX_CFG

//============================================================================
#define XSI_PRB_SEL (0xC0)
#define XSI_PRB_SEL_F_PROBE_SEL_SHIFT (0)
#define XSI_PRB_SEL_F_PROBE_SEL_MASK (0x1F)
#define XSI_PRB_SEL_F_PROBE_BIT0_SEL_SHIFT (8)
#define XSI_PRB_SEL_F_PROBE_BIT0_SEL_MASK (0x1F00)
#define XSI_PRB_SEL_F_PROBE_DTGRP_SEL_SHIFT (16)
#define XSI_PRB_SEL_F_PROBE_DTGRP_SEL_MASK (0xFF0000)

//============================================================================
#define XSI_PRB_OUTH (0xC4)


//============================================================================
#define XSI_PRB_OUTL (0xC8)


//============================================================================
#define XSI_IF_STS (0xCC)
#define XSI_IF_STS_F_TXMBI_STOP_STS (1<<0)
#define XSI_IF_STS_F_TXMPI_STOP_STS (1<<1)
#define XSI_IF_STS_F_RXMBI_STOP_STS (1<<2)
#define XSI_IF_STS_F_RXMPI_STOP_STS (1<<3)
#define XSI_IF_STS_F_TXMPI_MASK_STS (1<<4)

//============================================================================
#define XSI_CNT_CLR (0x100)
#define XSI_CNT_CLR_F_GLB_CNTCLR (1<<0)

//============================================================================
#define XSI_TX_OCTETS_CNT (0x104)


//============================================================================
#define XSI_TX_PKT_CNT (0x108)


//============================================================================
#define XSI_TXMBI_ETH_CNT (0x114)


//============================================================================
#define XSI_TXMBI_UCETH_CNT (0x118)


//============================================================================
#define XSI_TXMBI_MCETH_CNT (0x11C)
#define XSI_TXMBI_MCETH_CNT_F_TXMBI_MCETH_CNT_SHIFT (0)
#define XSI_TXMBI_MCETH_CNT_F_TXMBI_MCETH_CNT_MASK (0xFFFF)
#define XSI_TXMBI_MCETH_CNT_F_TXMBI_BCETH_CNT_SHIFT (16)
#define XSI_TXMBI_MCETH_CNT_F_TXMBI_BCETH_CNT_MASK (0xFFFF0000)

//============================================================================
#define XSI_TXMBI_PAUSE_CNT (0x120)
#define XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEON_CNT_SHIFT (0)
#define XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEON_CNT_MASK (0xFFFF)
#define XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEOFF_CNT_SHIFT (16)
#define XSI_TXMBI_PAUSE_CNT_F_TXMBI_PAUSEOFF_CNT_MASK (0xFFFF0000)

//============================================================================
#define XGMII_TX_SOF_EOF_CNT (0x130)
#define XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_SOF_CNT_SHIFT (16)
#define XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_SOF_CNT_MASK (0xFFFF0000)
#define XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_EOF_CNT_SHIFT (0)
#define XGMII_TX_SOF_EOF_CNT_F_XGMII_TX_EOF_CNT_MASK (0xFFFF)

//============================================================================
#define XGMII_TX_BYTES_CNT (0x134)


//============================================================================
#define TX_NORMAL_PKT_BYTES_CNT (0x138)


//============================================================================
#define TX_DEQ_CHECK_CNT1 (0x13C)


//============================================================================
#define TX_DEQ_CHECK_CNT2 (0x140)


//============================================================================
#define XSI_RX_FRAME_CNT (0x180)


//============================================================================
#define XSI_RX_OCTETS_CNT (0x184)


//============================================================================
#define XSI_RX_PKT_CNT (0x188)


//============================================================================
#define XSI_RX_ETH_CNT (0x18C)


//============================================================================
#define XSI_RX_PAUSE_CNT (0x190)
#define XSI_RX_PAUSE_CNT_F_RX_PAUSEON_CNT_SHIFT (0)
#define XSI_RX_PAUSE_CNT_F_RX_PAUSEON_CNT_MASK (0xFFFF)
#define XSI_RX_PAUSE_CNT_F_RX_PAUSEOFF_CNT_SHIFT (16)
#define XSI_RX_PAUSE_CNT_F_RX_PAUSEOFF_CNT_MASK (0xFFFF0000)

//============================================================================
#define XSI_RX_LENERR_CNT (0x194)
#define XSI_RX_LENERR_CNT_F_RX_LENERR_CNT_SHIFT (0)
#define XSI_RX_LENERR_CNT_F_RX_LENERR_CNT_MASK (0xFFFF)
#define XSI_RX_LENERR_CNT_F_RX_FRAGERR_CNT_SHIFT (16)
#define XSI_RX_LENERR_CNT_F_RX_FRAGERR_CNT_MASK (0xFFFF0000)

//============================================================================
#define XSI_RX_CRCERR_CNT (0x198)
#define XSI_RX_CRCERR_CNT_F_RX_CRCERR_CNT_SHIFT (0)
#define XSI_RX_CRCERR_CNT_F_RX_CRCERR_CNT_MASK (0xFFFF)
#define XSI_RX_CRCERR_CNT_F_RX_CODINGERR_CNT_SHIFT (16)
#define XSI_RX_CRCERR_CNT_F_RX_CODINGERR_CNT_MASK (0xFFFF0000)

//============================================================================
#define XSI_RXMBI_PKT_CNT (0x19C)


//============================================================================
#define XSI_RXMBI_DROP_CNT (0x1A0)
#define XSI_RXMBI_DROP_CNT_F_RXMBI_ERRDROP_CNT_SHIFT (0)
#define XSI_RXMBI_DROP_CNT_F_RXMBI_ERRDROP_CNT_MASK (0xFFFF)
#define XSI_RXMBI_DROP_CNT_F_RXMBI_SOFDROP_CNT_SHIFT (16)
#define XSI_RXMBI_DROP_CNT_F_RXMBI_SOFDROP_CNT_MASK (0xFFFF0000)

//============================================================================
#define XGMII_RX_SOF_EOF_CNT (0x1A4)
#define XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_SOF_CNT_SHIFT (16)
#define XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_SOF_CNT_MASK (0xFFFF0000)
#define XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_EOF_CNT_SHIFT (0)
#define XGMII_RX_SOF_EOF_CNT_F_XGMII_RX_EOF_CNT_MASK (0xFFFF)

//============================================================================
#define RX_MPI_SOP_EOP_CNT (0x1A8)
#define RX_MPI_SOP_EOP_CNT_F_RX_MPI_SOP_CNT_SHIFT (16)
#define RX_MPI_SOP_EOP_CNT_F_RX_MPI_SOP_CNT_MASK (0xFFFF0000)
#define RX_MPI_SOP_EOP_CNT_F_RX_MPI_EOP_CNT_SHIFT (0)
#define RX_MPI_SOP_EOP_CNT_F_RX_MPI_EOP_CNT_MASK (0xFFFF)

//============================================================================
#define RX_NORMAL_PKT_BYTES_CNT (0x1AC)


//============================================================================
#define RX_ENQ_CHECK_CNT1 (0x1B0)


//============================================================================
#define RX_ENQ_CHECK_CNT2 (0x1B4)


//============================================================================
#define RX_MBI_SOP_EOP_CNT (0x1B8)
#define RX_MBI_SOP_EOP_CNT_F_RX_MBI_SOP_CNT_SHIFT (16)
#define RX_MBI_SOP_EOP_CNT_F_RX_MBI_SOP_CNT_MASK (0xFFFF0000)
#define RX_MBI_SOP_EOP_CNT_F_RX_MBI_EOP_CNT_SHIFT (0)
#define RX_MBI_SOP_EOP_CNT_F_RX_MBI_EOP_CNT_MASK (0xFFFF)

//============================================================================
#define RX_UC_DROP_CNT (0x200)


//============================================================================
#define RX_BC_DROP_CNT (0x204)


//============================================================================
#define RX_MC_DROP_CNT (0x208)


//============================================================================
#define RX_TOTAL_DROP_CNT (0x20C)

#define P2_IFC_EN          (1<<3)


#define FE_VIP_PORT_EN		0xbfb501f0
#define FE_IFC_PORT_EN		0xbfb501f4
#define GDM2_TXCHN_EN       0Xbfb51524
#define GDM2_RXCHN_EN       0Xbfb51528


#define PPE_DFP_CPORT	    0xbfb50e48 
#define PPE_DFP_CPORT1	    0xbfb50e4c 
#define PPE2_DFT_CPORT0   0xbfb51e48 
#define PPE2_DFT_CPORT1    0xbfb51e4c 

#define SP_DFT_CPORT_0		0Xbfb520e0

//============================================================================
#define XFI_MAC_PCIE0_RST (1<<16)
#define XFI_MAC_PCIE1_RST (1<<17)
#define XFI_MAC_USB_RST   (1<<18)
#define XFI_MAC_ETH_RST   (1<<7)
#define XFI_MAC_AE_RST    (1<<28)

#define XFI_MAC_PCIE_RXARB_FRAG          (0x1fa04f0c)
#define XFI_MAC_PCIE1_RXARB_FRAG_SHIFT   (16)
#define XFI_MAC_PCIE1_RXARB_FRAG_MASK    (0x3ff0000)
#define XFI_MAC_PCIE0_RXARB_FRAG_SHIFT   (0)
#define XFI_MAC_PCIE0_RXARB_FRAG_MASK    (0x3ff)

#define XFI_MAC_PCIE_TXARB_FRAG          (0x1fa04f08)
#define XFI_MAC_PCIE1_TXARB_FRAG_SHIFT   (16)
#define XFI_MAC_PCIE1_TXARB_FRAG_MASK    (0x3ff0000)
#define XFI_MAC_PCIE0_TXARB_FRAG_SHIFT   (0)
#define XFI_MAC_PCIE0_TXARB_FRAG_MASK    (0x3ff)


//============================================================================
#define RX_PARSER_ID 					(0x690)
#define RX_PARSER_EN 					(0x694)
#define RX_PARSER_RXC_CODING			(0x698)
#define RX_PARSER_RXD_CODING_L			(0x69C)
#define RX_PARSER_RXD_CODING_H			(0x6A0)

#define RX_LPI_CHK_EN 					(0x6A4)
#define RX_LPI_CNT_THR					(0x6A8)
#define RX_LPI_ON 						(0x6B4)
#define RX_LPI_EVENT_CNT 				(0x6AC)
#define RX_LPI_DURATION_CNT 			(0x6B0)

#define TX_LPI_GEN_EN 					(0x6B8)
#define TX_LPI_TIMEOUT_CNT_THR			(0x6CC)
#define TX_LPI_ON 						(0x6D0)
#define TX_LPI_EVENT_CNT 				(0x6D4)
#define TX_LPI_DURATION_CNT 			(0x6D8)

#endif
