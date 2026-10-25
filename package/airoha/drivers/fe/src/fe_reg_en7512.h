/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _FE_en7512_H
#define _FE_en7512_H



/************************************************************************
*               I N C L U D E S
*************************************************************************
*/

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
extern u32 get_frame_engine_data(u32 reg);
extern void set_frame_engine_data(u32 reg, u32 val);

/*******************************************************
 register access utility
********************************************************/
#ifdef CONFIG_SIMULATION
	#define IO_GREG(reg)							0
	#define IO_SREG(reg, value)					
	#define IO_SBITS(reg, bit)					
	#define IO_CBITS(reg, bit)					
	#define IO_GREG_REP(reg, buf, count)			0
	#define IO_SREG_REP(reg, buf, count)		
#else
#ifdef TCSUPPORT_CPU_ARMV8
	#define IO_GREG(reg)							get_frame_engine_data(reg)
	#define IO_SREG(reg, value) 					set_frame_engine_data(reg, value)
	#define IO_GMASK(reg, mask, shift)				((get_frame_engine_data(reg) & mask) >> shift)
	#define IO_SMASK(reg, mask, shift, value)		{ uint t = get_frame_engine_data(reg); set_frame_engine_data(reg, ((t&~(mask))|((value<<shift)&mask))); }
	#define IO_SBITS(reg, bit)						{ uint t = get_frame_engine_data(reg); set_frame_engine_data(reg, (t|bit)); }
	#define IO_CBITS(reg, bit)						{ uint t = get_frame_engine_data(reg); set_frame_engine_data(reg, (t&~(bit))); }
#else
	#define IO_GREG(reg)							regRead32(reg)
	#define IO_SREG(reg, value)						regWrite32(reg, value)
	#define IO_GMASK(reg, mask, shift)				((regRead32(reg) & mask) >> shift)
	#define IO_SMASK(reg, mask, shift, value)		{ uint t = regRead32(reg); regWrite32(reg, ((t&~(mask))|((value<<shift)&mask))); }
	#define IO_SBITS(reg, bit)						{ uint t = regRead32(reg); regWrite32(reg, (t|bit)); }
	#define IO_CBITS(reg, bit)						{ uint t = regRead32(reg); regWrite32(reg, (t&~(bit))); }
	#define IO_GREG_REP(reg, buf, count)			ioread32_rep((void __iomem *)(reg), buf, count)
	#define IO_SREG_REP(reg, buf, count)			iowrite32_rep((void __iomem *)(reg), buf, count)
#endif
#endif /* CONFIG_SIMULATION */

/********************************
	 * SCU Module Registers *
********************************/
#define SCU_PROBE_MODE_CFG 0xbfb00080
#define FE_RST_CTRL1       0XBFB00834

#define SHARED_UNZIPMENT_SEL 0xbfb00954


/***********************************************************************
		* EN7523 New FE Registers *
***********************************************************************/
#define FE_CDM_OQ_MAP1      	(FE_BASE + 0x54)
#define FE_CDM2_OQ_MAP      	(FE_BASE + 0x58)
#define FE_CDM2_OQ_MAP1     	(FE_BASE + 0x5C)
#define FE_PCE_CFG          	(FE_BASE + 0x70)
#define CDM_HWF_OQ_MASK         (0x7)
#define FE_CDM_HWF_OQ          	(FE_BASE + 0x7C)
#define PSE_QUEUE_CFG_WR		(FE_BASE + 0x80)
#define PSE_CFG_PORT_ID_SHIFT	(24)
#define PSE_CFG_PORT_ID_MASK	(0xF<<PSE_CFG_PORT_ID_SHIFT)
#define PSE_CFG_QUEUE_ID_SHIFT	(16)
#define PSE_CFG_QUEUE_ID_MASK	(0x1F<<PSE_CFG_QUEUE_ID_SHIFT)
#define PSE_CFG_WR_EN_SHIFT		(8)
#define PSE_CFG_WR_EN_MASK		(0x1<<PSE_CFG_WR_EN_SHIFT)
#define PSE_CFG_WR_EN			(0x1<<PSE_CFG_WR_EN_SHIFT)

#if defined(TCSUPPORT_CPU_AN7552)
#define PSE_CFG_LTHD_SEL_SHIFT	(2)
#define PSE_CFG_OQEN_SEL_SHIFT	(3)
#else
#define PSE_CFG_OQEN_SEL_SHIFT	(2)
#define PSE_CFG_LTHD_SEL_SHIFT	(3)
#endif
#define PSE_CFG_OQEN_SEL_MASK	(0x1<<PSE_CFG_OQEN_SEL_SHIFT)
#define PSE_CFG_OQEN_SEL		(0x1<<PSE_CFG_OQEN_SEL_SHIFT)
#define PSE_CFG_LTHD_SEL_MASK	(0x1<<PSE_CFG_LTHD_SEL_SHIFT)
#define PSE_CFG_LTHD_SEL		(0x1<<PSE_CFG_LTHD_SEL_SHIFT)

#define PSE_CFG_IQRSV_SEL_SHIFT	(1)
#define PSE_CFG_IQRSV_SEL_MASK	(0x1<<PSE_CFG_IQRSV_SEL_SHIFT)
#define PSE_CFG_IQRSV_SEL		(0x1<<PSE_CFG_IQRSV_SEL_SHIFT)
#define PSE_CFG_OQRSV_SEL_SHIFT	(0)
#define PSE_CFG_OQRSV_SEL_MASK	(0x1<<PSE_CFG_OQRSV_SEL_SHIFT)
#define PSE_CFG_OQRSV_SEL		(0x1<<PSE_CFG_OQRSV_SEL_SHIFT)
#define PSE_QUEUE_CFG_VAL		(FE_BASE + 0x84)
#if defined(TCSUPPORT_CPU_AN7552)
#define PSE_CFG_OQ_LTHD_SHIFT	(18)
#define PSE_CFG_OQ_EN_SHIFT	    (16)
#define PSE_CFG_OQ_EN_MASK		(0x1<<PSE_CFG_OQ_EN_SHIFT)
#define PSE_CFG_OQ_RSV_SHIFT	(0)
#define PSE_CFG_OQ_RSV_MASK		(0x7FFF<<PSE_CFG_OQ_RSV_SHIFT)
#else
#define PSE_CFG_OQ_LTHD_SHIFT	(30)
#define PSE_CFG_OQ_RSV_SHIFT	(0)
#define PSE_CFG_OQ_RSV_MASK		(0x3FFF<<PSE_CFG_OQ_RSV_SHIFT)
#endif
#define PSE_CFG_OQ_LTHD_MASK	(0x3<<PSE_CFG_OQ_LTHD_SHIFT)
#define PSE_CFG_IQ_RSV_SHIFT	(16)
#define PSE_CFG_IQ_RSV_MASK		(0x3FFF<<PSE_CFG_IQ_RSV_SHIFT)


#define PSE_OQUEUE_EN			(FE_BASE + 0x88)
#define PSE_OQ_TOTAL_RSV		(FE_BASE + 0x90)  /* Total reservation for all OQ and IQ */
#define PSE_FC_CFG          	(FE_BASE + 0x98)  /* PSE_FC_CNT_TYPE, PSE_PORT_SHARE_DIS, PSE_FC_EN */
#define PSE_BUFFER_TYPE         (FE_BASE + 0x9C) 

#define PSE_SHARE_BUF_USED_MAX_SHIFT	(16)
#define PSE_SHARE_BUF_USED_MAX_MASK	(0x7fff)
#define PSE_BUF_USE_REC			(FE_BASE + 0x100) /* 30:16, SHARE_USED_CNT_MAX; 14:0, FQ_CNT_MIN */
#define PSE_SHARE_BUF_USED_CNT_SHIFT	(16)
#define PSE_SHARE_BUF_USED_CNT_MASK	(0x7fff)
#define PSE_SHARE_BUF_FREE_CNT_MASK	(0x7fff)
#define PSE_SHARE_BUF_STA		(FE_BASE + 0x104)
#define PSE_PORT_STA        	(FE_BASE + 0x10C)
#define PSE_OQ_PCNT				(FE_BASE + 0x110)
#define PSE_OQ_PCNT_REC			(FE_BASE + 0x114) /* 29:16, OQ_CNT_MAX; 13:0, OQ_REAL_CNT_MAX */
#define PSE_DROP_CNT_7      	(FE_BASE + 0x13C)
#define PSE_DROP_CNT_8      	(FE_BASE + 0x140)
#define PSE_DROP_CNT_9      	(FE_BASE + 0x144)
#define GDMA_MISC_CFG           (FE_BASE + 0x148)
#define PSE_PORT_Q_USE_STA(i)	(FE_BASE + 0x150 + (i<<2))
#define PSE_PORT_Q_USE_STA0		(FE_BASE + 0x150) /* 30:16, P0_IQ_PCNT; 14:0, P0_OQ_PCNT */
#define PSE_PORT_Q_USE_STA1		(FE_BASE + 0x154)
#define PSE_PORT_Q_USE_STA2		(FE_BASE + 0x158)
#define PSE_PORT_Q_USE_STA3		(FE_BASE + 0x15C)
#define PSE_PORT_Q_USE_STA4		(FE_BASE + 0x160)
#define PSE_PORT_Q_USE_STA5		(FE_BASE + 0x164)
#define PSE_PORT_Q_USE_STA6		(FE_BASE + 0x168)
#define PSE_PORT_Q_USE_STA7		(FE_BASE + 0x16C)
#define PSE_PORT_Q_USE_STA8		(FE_BASE + 0x170)
#define PSE_PORT_Q_USE_STA9		(FE_BASE + 0x174)

#define QDMA1_FC_MAPPING4    	(FE_BASE + 0x1C0)
#define QDMA1_FC_MAPPING5    	(FE_BASE + 0x1C4)
#define QDMA1_FC_MAPPING6    	(FE_BASE + 0x1C8)
#define QDMA1_FC_MAPPING7    	(FE_BASE + 0x1CC)

#define LIST_TYPE_BIT		0
#define ACTION_BIT			1
#define MAC_SELECT_BIT		2
#define PKT_TYPE_OC_BIT		4
#define PKT_TYPE_MC_BIT		5
#define PKT_TYPE_BC_BIT		6
#define PKT_TYPE_UC_BIT		7
#define GDM2_RX_MAC_FILTER_CFG    (FE_BASE + 0x16b0)
#define GDM2_RX_MAC_FILTER_EN     (FE_BASE + 0x16b4)
#define GDM2_RX_MAC_FILTER_PATN_H(n)     (FE_BASE + 0x16c0 + n*8)
#define GDM2_RX_MAC_FILTER_PATN_L(n)     (FE_BASE + 0x16c4 + n*8)
#define GDM3_RX_MAC_FILTER_CFG    (FE_BASE + 0x16b0 + 0x400)
#define GDM3_RX_MAC_FILTER_EN     (FE_BASE + 0x16b4 + 0x400)
#define GDM3_RX_MAC_FILTER_PATN_H(n)     (FE_BASE + 0x16c0 + 0x400 + n*8)
#define GDM3_RX_MAC_FILTER_PATN_L(n)     (FE_BASE + 0x16c4 + 0x400 + n*8)
#define GDMA2_RLS_MODE_BIT           (1<<1)
#define GDMA2_CHN_VLD_MODE_BIT       (1<<5)
#define GDMA2_RDM_ACK_WAIT_PREF_BIT  (1<<9)

#define GDM2_AEWANFWD_FQ    (FE_BASE+0x14FC)
#define FE_AEWANIFC_CFG     (FE_BASE+0x0200)

/*****************************************************************************
 		* EN7523 different from EN7580  FE Registers *
 *****************************************************************************/
#if defined(TCSUPPORT_CPU_EN7523)
#define PSE_FQ_CFG			(FE_BASE + 0x8C)
#define PSE_SHARE_USED_MTHD_OFFSET 16
#define PSE_SHARE_USED_MTHD_MASK (0xffff<<PSE_SHARE_USED_MTHD_OFFSET)
#define PSE_SHARE_USED_THD	(FE_BASE + 0x94)
#define PSE_FQ_STA			(FE_BASE + 0x108)
#define PSE_FQFC_CFG_STA(i)	(FE_BASE + 0x118 + (i<<2))
#define PSE_FQFC_CFG_STA0	(FE_BASE + 0x118)
#define PSE_FQFC_CFG_STA1	(FE_BASE + 0x11C)
#define PSE_DROP_CNT(i)		(FE_BASE + 0x120 + (i<<2))
#define PSE_DROP_CNT_0      (FE_BASE + 0x120)
#define PSE_DROP_CNT_1      (FE_BASE + 0x124)
#define PSE_DROP_CNT_2      (FE_BASE + 0x128)
#define PSE_DROP_CNT_3      (FE_BASE + 0x12C)
#define PSE_DROP_CNT_4      (FE_BASE + 0x130)
#define PSE_DROP_CNT_5      (FE_BASE + 0x134)
#define PSE_DROP_CNT_6      (FE_BASE + 0x138)
#else
#define PSE_FQ_CFG			(FE_BASE + 0x14C)
#define PSE_SHARE_USED_THD  (FE_BASE + 0xD0)
#define PSE_FQ_STA          (FE_BASE + 0x150)
#define PSE_FQFC_CFG_STA(i) (FE_BASE + 0x10C + (i<<2))
#define PSE_FQFC_CFG_STA0   (FE_BASE + 0x10C)
#define PSE_FQFC_CFG_STA1   (FE_BASE + 0x110)
#define PSE_DROP_CNT(i)     (FE_BASE + 0x178 + (i<<2))
#define PSE_DROP_CNT_0      (FE_BASE + 0x178)
#define PSE_DROP_CNT_1      (FE_BASE + 0x17C)
#define PSE_DROP_CNT_2      (FE_BASE + 0x180)
#define PSE_DROP_CNT_3      (FE_BASE + 0x184)
#define PSE_DROP_CNT_4      (FE_BASE + 0x188)
#define PSE_DROP_CNT_5      (FE_BASE + 0x18C)
#define PSE_DROP_CNT_6      (FE_BASE + 0x190)
#endif

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
#define FE_VIP_PORT_EN		0x1fb501f0
#define FE_IFC_PORT_EN		0x1fb501f4
#endif
/*****************************************************************************
 		* EN7580 FE Registers *
 *****************************************************************************/
#define FE_BASE     		0xBFB50000
#define FE_DMA_GLO_CFG      (FE_BASE + 0x00)
#define FE_RLS_CNT_TYPE_BIT	(1 << 2)
#define FE_RST_GLO          (FE_BASE + 0x04)
#define FE_INT_STATUS       (FE_BASE + 0x08)
#define FE_INT_ENABLE       (FE_BASE + 0x0C)
#define FE_FOE_TS           (FE_BASE + 0x10)
#define FE_IPV6_EXT         (FE_BASE + 0x14)
#define FE_CNT_CFG          (FE_BASE + 0x18)
#define FE_TAG_CFG          (FE_BASE + 0x1C)
#define FE_GENTIME          (FE_BASE + 0x20)

#define FE_WAN_PORT         (FE_BASE + 0x24)
#define FE_WAN_MAC_H        (FE_BASE + 0x30)
#define FE_WAN_MAC_LMIN     (FE_BASE + 0x34)
#define FE_WAN_MAC_LMAX     (FE_BASE + 0x38)
#define FE_LAN_MAC_H        (FE_BASE + 0x40)
#define FE_LAN_MAC_LMIN     (FE_BASE + 0x44)
#define FE_LAN_MAC_LMAX     (FE_BASE + 0x48)
#define FE_CDM_OQ_MAP       (FE_BASE + 0x50)

#define PSE_OQ_RSV(i)       (FE_BASE + 0x80 + (i<<2))
#define PSE_OQ_RSV0         (FE_BASE + 0x80)
#define PSE_OQ_RSV1         (FE_BASE + 0x84)
#define PSE_OQ_RSV2         (FE_BASE + 0x88)
#define PSE_OQ_RSV3         (FE_BASE + 0x8C)
#define PSE_OQ_RSV4         (FE_BASE + 0x90)
#define PSE_OQ_RSV5         (FE_BASE + 0x94)
#define PSE_OQ_RSV6         (FE_BASE + 0x98)
#define PSE_OQ_RSV7         (FE_BASE + 0x9C)

#define PSE_OQ_THD          (FE_BASE + 0xA0)

#define PSE_IQ_RSV(i)       (FE_BASE + 0xC0 + (i<<2))
#define PSE_IQ_RSV0         (FE_BASE + 0xC0)
#define PSE_IQ_RSV1         (FE_BASE + 0xC4)
#define PSE_IQ_RSV2         (FE_BASE + 0xC8)
#define PSE_IQ_RSV3         (FE_BASE + 0xCC)

#define PSE_PORT_OQ_EN(i)   (FE_BASE + 0xD4 + (i<<2))
#define PSE_PORT0_OQ_EN     (FE_BASE + 0xD4)
#define PSE_PORT1_OQ_EN     (FE_BASE + 0xD8)
#define PSE_PORT2_OQ_EN     (FE_BASE + 0xDC)
#define PSE_PORT3_OQ_EN     (FE_BASE + 0xE0)
#define PSE_PORT4_OQ_EN     (FE_BASE + 0xE4)
#define PSE_PORT5_OQ_EN     (FE_BASE + 0xE8)
#define PSE_PORT6_OQ_EN     (FE_BASE + 0xEC)

#define PSE_FQFC_CFG_STA2   (FE_BASE + 0x114)
#define PSE_FQFC_CFG_STA3   (FE_BASE + 0x118)
#define PSE_FQFC_CFG_STA4   (FE_BASE + 0x11C)
#define PSE_FQFC_CFG_STA5   (FE_BASE + 0x120)

#define PSE_PORT_OQ_STA(i)  (FE_BASE + 0x124 + (i<<2))
#define PSE_PORT_OQ_STA0    (FE_BASE + 0x124)
#define PSE_PORT_OQ_STA1    (FE_BASE + 0x128)
#define PSE_PORT_OQ_STA2    (FE_BASE + 0x12C)
#define PSE_PORT_OQ_STA3    (FE_BASE + 0x130)
#define PSE_PORT_OQ_STA4    (FE_BASE + 0x134)
#define PSE_PORT_OQ_STA5    (FE_BASE + 0x138)

#define PSE_PORT_IQ_STA(i)  (FE_BASE + 0x13C + (i<<2))
#define PSE_PORT_IQ_STA0    (FE_BASE + 0x13C)
#define PSE_PORT_IQ_STA1    (FE_BASE + 0x140)
#define PSE_PORT_IQ_STA2    (FE_BASE + 0x144)
#define PSE_PORT_IQ_STA3    (FE_BASE + 0x148)

#define PSE_FQ_REC          (FE_BASE + 0x154)
#define PSE_OQ_CNT_REC      (FE_BASE + 0x158)
#define PSE_OQ_REAL_CNT_REC (FE_BASE + 0x15C)
#define PSE_SHARE_CNT_REC   (FE_BASE + 0x160)
#define PSE_MISC_CFG_1      (FE_BASE + 0x174)
#define PSE_MISC_CFG_2      (FE_BASE + 0x194)
#define PSE_PROBE_L         (FE_BASE + 0x1A0)
#define PSE_PROBE_H         (FE_BASE + 0x1A4)

#define QDMA1_FC_MAPPING(i)  (FE_BASE + 0x1B0 + (i<<2))
#define QDMA1_FC_MAPPING0    (FE_BASE + 0x1B0)
#define QDMA1_FC_MAPPING1    (FE_BASE + 0x1B4)
#define QDMA1_FC_MAPPING2    (FE_BASE + 0x1B8)
#define QDMA1_FC_MAPPING3    (FE_BASE + 0x1BC)

#define QDMA2_FC_MAPPING5    (FE_BASE + 0x21C4)

/********************************
 *        PSE  Register         *
 ********************************/

#define PSE_BASE     		(FE_BASE + 0x0100)
#define PSE_FQFC_CFG        (PSE_BASE + 0x00)
#define PSE_FQFC_CFG_PPELTH_OFFSET	0
#define PSE_FQFC_CFG_PPEIQ_OFFSET		8
#define PSE_FQFC_CFG_FQMAX_OFFSET		16
#define PSE_FQFC_CFG_FQMAX_LENGTH		8
#define PSE_FQFC_CFG_FQCURR_OFFSET	24
#define PSE_FQFC_CFG_FQMAX_MASK     (0xFF << PSE_FQFC_CFG_FQMAX_OFFSET)
#ifdef TCSUPPORT_CPU_EN7528
#define PSE_OAM_QSEL_OFFSET		12
#else
#define PSE_OAM_QSEL_OFFSET		28
#endif
#define PSE_OAM_QSEL_MASK     (0xF << PSE_OAM_QSEL_OFFSET)
#define PSE_IQ_REV_VIP      (PSE_BASE + 0x04)
#define PSE_IQ_VIP_P1_OFFSET		0
#define PSE_IQ_VIP_P2_OFFSET		8
#define PSE_Q_PORT_LENGTH			8
#define PSE_IQ_REV1         (PSE_BASE + 0x08)
#define PSE_IQ_RES1_P0_OFFSET	0
#define PSE_IQ_RES1_P1_OFFSET	8
#define PSE_IQ_RES1_P2_OFFSET	16
#define PSE_IQ_REV2         (PSE_BASE + 0x0c)
#define PSE_IQ_RES2_P4_OFFSET	0
#define PSE_IQ_RES2_P5_OFFSET	8
#define PSE_IQ_RES2_P6_OFFSET	16
#define PSE_IQ_STA1        	(PSE_BASE + 0x10)
#define PSE_IQ_P0_OFFSET	0
#define PSE_IQ_P1_OFFSET	8
#define PSE_IQ_P2_OFFSET	16

#define PSE_IQ_STA2        	(PSE_BASE + 0x14)
#define PSE_IQ_P4_OFFSET	0
#define PSE_IQ_P5_OFFSET	8
#define PSE_IQ_P6_OFFSET	16

#define PSE_OQ_STA1         (PSE_BASE + 0x18)
#define PSE_OQ_P0_OFFSET	0
#define PSE_OQ_P1_OFFSET	8
#define PSE_OQ_P2_OFFSET	16

#define PSE_OQ_STA2        	(PSE_BASE + 0x1c)
#define PSE_OQ_P4_OFFSET	0
#define PSE_OQ_P5_OFFSET	8
#define PSE_OQ_P6_OFFSET	16
#if defined(TCSUPPORT_CPU_EN7523)
#define PSE_MISC_CFG        (PSE_BASE + 0x9c)
#define FE_MISC_CFG         (PSE_BASE + 0x194)
#elif defined(TCSUPPORT_CPU_EN7580)
#define PSE_MISC_CFG        (PSE_BASE + 0x174)
#define FE_MISC_CFG         (PSE_BASE + 0x194)
#else
#define PSE_MISC_CFG        (PSE_BASE + 0x20)
#define FE_MISC_CFG         (PSE_BASE + 0x24)
#endif

#define PSE_PORT0_OQ_NUM		(6)
#define PSE_PORT1_OQ_NUM		(6)
#define PSE_PORT2_OQ_NUM		(32)
#define PSE_PORT3_OQ_NUM		(6)
#define PSE_PORT4_OQ_NUM		(2)
#define PSE_PORT5_OQ_NUM		(6)
#define PSE_PORT6_OQ_NUM		(8)
#define PSE_PORT7_OQ_NUM		(10)
#define PSE_PORT8_OQ_NUM		(1)
#define PSE_PORT9_OQ_NUM		(2)
#define PSE_PORT10_OQ_NUM		(2)

#define PPE_SRAM_EN_BIT			(1 << 0)
#define PSE_ALL_PD_BIT			(1 << 9)
#define PSE_PKT_SRAM_PD_SHIFT	(0)
#define PSE_PKT_SRAM_PD_MASK	(0xffff << PSE_PKT_SRAM_PD_SHIFT)
#define PSE_PKT_SRAM_PD_HALF_SHIFT	(8)
#define PSE_PKT_SRAM_PD_HALF_MASK	(0xff << PSE_PKT_SRAM_PD_HALF_SHIFT)
#define PSE_ALL_RSV_SHIFT	(0)
#define PSE_ALL_RSV_MASK	(0x7fff << PSE_ALL_RSV_SHIFT)

#define PPE_HASH_CFG	(FE_BASE + 0xe50)
#define PSE_SHARE_USED_LTHD_OFFSET 16
#define PSE_SHARE_USED_LTHD_MASK (0xffff<<PSE_SHARE_USED_LTHD_OFFSET)
#define PSE_BUFF_SET	(FE_BASE + 0x90)
#define PSE_ALL_PD_CFG	(FE_BASE + 0x194)
#define FE_SRAM_PD0		(FE_BASE + 0x2160)
#define FE_SRAM_PD1		(FE_BASE + 0x2164)
#define FE_SRAM_PD2		(FE_BASE + 0x2168)

#define FE_PROBE_L          (PSE_BASE + 0x30)
#define FE_PROBE_H          (PSE_BASE + 0x34)
#define PSE_FQ_MIN          (PSE_BASE + 0x40)
#define PSE_FQ_MIN_CUR_OFFSET	16
#define PSE_FQ_MIN_CUR_LENGTH	9
#define PSE_FQ_MIN_MIN_OFFSET	0
#define PSE_FQ_MIN_MIN_LENGTH	9
#define PSE_IQ_MAX1         (PSE_BASE + 0x50)
#define PSE_IQ_MAX1_P0_OFFSET	0
#define PSE_IQ_MAX1_P1_OFFSET	8
#define PSE_IQ_MAX1_P2_OFFSET	16
#define PSE_IQ_MAX2         (PSE_BASE + 0x54)
#define PSE_IQ_MAX2_P4_OFFSET	0
#define PSE_IQ_MAX2_P5_OFFSET	8
#define PSE_IQ_MAX2_P6_OFFSET	16
#define PSE_DROP_COUNT_0    (PSE_BASE + 0x80)
#define PSE_DROP_COUNT_1    (PSE_BASE + 0x84)
#define PSE_DROP_COUNT_2    (PSE_BASE + 0x88)
#define PSE_DROP_COUNT_4    (PSE_BASE + 0x90)
#define PSE_DROP_COUNT_5    (PSE_BASE + 0x94)

#define PSE_L2LU_KEY0(x)		(FE_BASE + 0x0200 + ((x) << 3))
#define L2LU_KEY0_CVID_OFFSET			0
#define L2LU_KEY0_CVID_LENGTH			12
#define L2LU_KEY0_CPCP_OFFSET			13
#define L2LU_KEY0_CPCP_LENGTH			3
#define L2LU_KEY0_SVID_OFFSET			16
#define L2LU_KEY0_SVID_LENGTH			12
#define L2LU_KEY0_SPCP_OFFSET			29
#define L2LU_KEY0_SPCP_LENGTH			3

#define PSE_L2LU_KEY1(x)		(FE_BASE + 0x0204 + ((x) << 3))
#define L2LU_KEY1_DSCP_MASK_OFFSET	0
#define L2LU_KEY1_DSCP_MASK_LENGTH	8
#define L2LU_KEY1_CVID_EN_OFFSET		8
#define L2LU_KEY1_CVID_EN_LENGTH		1
#define L2LU_KEY1_CPCP_EN_OFFSET		9
#define L2LU_KEY1_CPCP_EN_LENGTH		1
#define L2LU_KEY1_SVID_EN_OFFSET		12
#define L2LU_KEY1_SVID_EN_LENGTH		1
#define L2LU_KEY1_SPCP_EN_OFFSET		13
#define L2LU_KEY1_SPCP_EN_LENGTH		1
#define L2LU_KEY1_DSCP_OFFSET			16
#define L2LU_KEY1_DSCP_LENGTH			8

#define L2BR_CFG			(FE_BASE + 0x280)
#define L2BR_ETYPE_EN			(FE_BASE + 0x284)
#define L2BR_ETYPE_N(x)			(FE_BASE + 0x290 + 4*(x/2))

#ifdef TCSUPPORT_CPU_EN7528
#define PPE_AC_BASE			(FE_BASE + 0x2400)
#define PPE_MTR_BASE		(FE_BASE + 0x2000)
#define PPE_MTR_CNT(x)			(PPE_MTR_BASE + ((x) << 2))
#else
#define PPE_AC_BASE			(FE_BASE + 0x2000)
#define PPE_MTR_CNT(x)			(PPE_AC_BASE + 0x0c + ((x) << 4))
#endif
#define PPE_AC_BCNT_L(x)		(PPE_AC_BASE + ((x) << 4))
#define PPE_AC_BCNT_H(x)		(PPE_AC_BASE + 0x04 + ((x) << 4))
#define PPE_AC_PCNT(x)			(PPE_AC_BASE + 0x08 + ((x) << 4))

#if defined(TCSUPPORT_CPU_EN7581)
#define GET_PSE_SHARED_USED_LTHD()    IO_GMASK(PSE_BUFF_SET,PSE_SHARE_USED_LTHD_MASK,PSE_SHARE_USED_LTHD_OFFSET)
#define SET_PSE_SHARED_USED_LTHD(val)   IO_SMASK(PSE_BUFF_SET,PSE_SHARE_USED_LTHD_MASK,PSE_SHARE_USED_LTHD_OFFSET,val)
#define GET_PSE_ALL_RSV()    IO_GMASK(PSE_BUFF_SET,0x7fff,0)
#define SET_PSE_ALL_RSV(val)    IO_SMASK(PSE_BUFF_SET,0x7fff,0,val)
#define GET_PSE_SHARED_USED_MTHD()    IO_GMASK(PSE_SHARE_USED_THD,PSE_SHARE_USED_MTHD_MASK,PSE_SHARE_USED_MTHD_OFFSET)
#define SET_PSE_SHARED_USED_MTHD(val)   IO_SMASK(PSE_SHARE_USED_THD,PSE_SHARE_USED_MTHD_MASK,PSE_SHARE_USED_MTHD_OFFSET,val)
#define GET_PSE_SHARED_USED_HTHD()    IO_GMASK(PSE_SHARE_USED_THD,0xffff,0)
#define SET_PSE_SHARED_USED_HTHD(val)   IO_SMASK(PSE_SHARE_USED_THD,0xffff,0,val)
#define GET_PSE_FQ_LITMI() IO_GMASK(PSE_FQ_CFG,0x7fff,0)

#define PSE_RSV_PAGE_DEFAULT    0x80/*hw default rsv page*/
#endif
/********************************
 *        IFC  Register         *
 ********************************/
#if defined(TCSUPPORT_CPU_EN7580)
#define FE_CSR_IFC_BASE			(FE_BASE + 0x200)
#else
#define FE_CSR_IFC_BASE			(FE_BASE + 0x1a0)
#endif
#define FE_CSR_IFC_CFG			(FE_CSR_IFC_BASE + 0x0000)

/********************************
 *        VIP  Register         *
 ********************************/
#define FE_VIP_EN(x)			(FE_BASE + 0x300 + ((x) << 3))
#define FE_VIP_PATN(x)			(FE_BASE + 0x304 + ((x) << 3))

#define VIP_EN_ENABLE_OFFSET		0
#define VIP_EN_TYPE_OFFSET		1
#define VIP_EN_TYPE_LENGTH		3
#define VIP_EN_SPEN_OFFSET		4
#define VIP_EN_DPEN_OFFSET		5
#define VIP_EN_SWEN_OFFSET		6
#define VIP_EN_CPU_OFFSET			7
#define VIP_PATN_SP_OFFSET		0
#define VIP_PATN_DP_OFFSET		16
#define VIP_PATN_PORT_LENGTH		16

#if defined(TCSUPPORT_CPU_EN7580)
#define WAN_PORT_WAN1_EN_OFFSET	16
#define WAN_PORT_WAN1_PORT_OFFSET	8
#define WAN_PORT_WAN0_PORT_OFFSET	0
#else
#define WAN_PORT_WAN1_EN_OFFSET	8
#define WAN_PORT_WAN1_PORT_OFFSET	4
#define WAN_PORT_WAN0_PORT_OFFSET	0
#endif
/********************************
 *        CDM1 Register         *
 ********************************/
#define CDMA1_BASE     		(FE_BASE + 0x400)
#define CDMA1_VLAN_CTRL      (CDMA1_BASE + 0x00)
#define CDMA_VLAN_CTRL_SP_OFFSET			0
#define CDMA_VLAN_CTRL_UNTAG_OFFSET		1
#define CDMA_VLAN_CTRL_TPID_OFFSET		16
#define CDMA1_PPP_GEN        (CDMA1_BASE + 0x04)
#define CDMA_PPP_GEN_INS		16
#define CDMA_PPP_GEN_ID		0

#define CDMA1_FWD_CFG        (CDMA1_BASE + 0x08)
#define CDM_FWD_CFG_Q0_OFFSET		30
#define CDM_FWD_CFG_Q1L_OFFSET		28
#define CDM_FWD_CFG_Q1H_OFFSET		26
#define CDM_FWD_CFG_Q_LENGTH			2
#define CDM_FWD_CFG_Q_MASK  		0x3
#define CDM_FWD_CFG_RXMSG_OFFSET	24
#define CDM_FWD_CFG_RXMSG_LENGTH	2
#define CDM_FWD_CFG_ARB_OFFSET		20
#define CDM_FWD_CFG_ARB_LENGTH		2
#define CDM_FWD_CFG_FP_OFFSET		16
#define CDM_FWD_CFG_FP_LENGTH		1
#define CDM_FWD_CFG_PAD_OFFSET		0
#define CDM_FWD_CFG_PAD_LENGTH		1

#define CDMA1_HWF_CHN_EN     (CDMA1_BASE + 0x0c)
#define CMD1_RXQ_RED_CFG     (CDMA1_BASE + 0x20)

/****************  EN7527 & EN7516 & EN7580 Start  ********************/
/*regIdx 0~3*/
#define CDMA1_CRSN_QSEL(regIdx)					(CDMA1_BASE + 0x10 + ((regIdx)<<2))
#define CDMA2_CRSN_QSEL(regIdx)					(CDMA2_BASE + 0x10 + ((regIdx)<<2))

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define CDMA_CRSN_QSEL_CPU_REASON_QUEUE_MASK    (0x1F)
#define CDMA_CRSN_CPU_REASON_SHIFT(crsn)        (((crsn)%4)<<3)
#else
#define CDMA_CRSN_QSEL_CPU_REASON_QUEUE_MASK    (0xF)
#define CDMA_CRSN_CPU_REASON_SHIFT(crsn)        (((crsn)%8)<<2)
#endif
#define CDMA_CRSN_QSEL_CPU_REASON_MASK(crsn)    (CDMA_CRSN_QSEL_CPU_REASON_QUEUE_MASK<<CDMA_CRSN_CPU_REASON_SHIFT(crsn))
#define CDMA_CRSN_QSEL_Q1						(1)
#define CDMA_CRSN_QSEL_Q5						(5)
#define CDMA_CRSN_QSEL_Q6						(6)
#define CDMA_CRSN_QSEL_Q15						(15)

#define CRSN_08									(0x8) 
#define CRSN_21									(0x15) /*KA*/
#define CRSN_22									(0x16) /*hit bind and force route to CPU*/
#define CRSN_24									(0x18)
#define CRSN_25									(0x19)

#if defined(TCSUPPORT_CPU_EN7581)  || defined(TCSUPPORT_CPU_AN7552)
#define cdm1SetSpecCrsnToCpuRing(crsn,rxIdx)	IO_SMASK(CDMA1_CRSN_QSEL(crsn>>2), CDMA_CRSN_QSEL_CPU_REASON_MASK(crsn), CDMA_CRSN_CPU_REASON_SHIFT(crsn), (rxIdx))
#define cdm2SetSpecCrsnToCpuRing(crsn,rxIdx)	IO_SMASK(CDMA2_CRSN_QSEL(crsn>>2), CDMA_CRSN_QSEL_CPU_REASON_MASK(crsn), CDMA_CRSN_CPU_REASON_SHIFT(crsn), (rxIdx))
#else
#define cdm1SetSpecCrsnToCpuRing(crsn,rxIdx)	IO_SMASK(CDMA1_CRSN_QSEL(crsn>>3), CDMA_CRSN_QSEL_CPU_REASON_MASK(crsn), CDMA_CRSN_CPU_REASON_SHIFT(crsn), (rxIdx))
#define cdm2SetSpecCrsnToCpuRing(crsn,rxIdx)	IO_SMASK(CDMA2_CRSN_QSEL(crsn>>3), CDMA_CRSN_QSEL_CPU_REASON_MASK(crsn), CDMA_CRSN_CPU_REASON_SHIFT(crsn), (rxIdx))
#endif

#if defined(TCSUPPORT_CPU_EN7581)  || defined(TCSUPPORT_CPU_AN7552)
#define OAM_QSEL_OFFSET     27
#define OAM_QSEL_MASK       0x1F
#else
#define OAM_QSEL_OFFSET     28
#define OAM_QSEL_MASK       0xF
#endif

#if defined(TCSUPPORT_CPU_EN7581)  || defined(TCSUPPORT_CPU_AN7552)
#define VIP_QSEL_SHIFT		(20)
#define VIP_QSEL_MASK		(0x1f << VIP_QSEL_SHIFT)
#else
#define VIP_QSEL_SHIFT		(24)
#define VIP_QSEL_MASK		(0xf << VIP_QSEL_SHIFT)
#endif
/****************  EN7527 & EN7516 & EN7580 End  ********************/

/****************  MT7510/7520 & EN7512 Start  ********************/
#define CDMA1_CRSN_QSEL0      (CDMA1_BASE + 0x10)
#define CDMA1_CRSN_QSEL1      (CDMA1_BASE + 0x14)

#define CDMA1_CRSN_QSEL_CPU_REASON_16_OFFSET 0
#define CDMA1_CRSN_QSEL_CPU_REASON_QUEUE_MASK 3
#define CDMA1_CRSN_QSEL_CPU_REASON_16_MASK (CDMA1_CRSN_QSEL_CPU_REASON_QUEUE_MASK<<CDMA1_CRSN_QSEL_CPU_REASON_16_OFFSET)
#define CDMA1_CRSN_QSEL_Q1L 2
#define CDMA1_CRSN_QSEL_Q1H 3
#define CDMA1_CRSN_QSEL_CPU_REASON_22_OFFSET 12
#define CDMA1_CRSN_QSEL_CPU_REASON_22_MASK (CDMA1_CRSN_QSEL_CPU_REASON_QUEUE_MASK<<CDMA1_CRSN_QSEL_CPU_REASON_22_OFFSET)
#define CRSN_QSEL_CPU_REASON_15_OFFSET 30
#define CRSN_QSEL_CPU_REASON_15_QUEUE_MASK (CDMA1_CRSN_QSEL_CPU_REASON_QUEUE_MASK<<CRSN_QSEL_CPU_REASON_15_OFFSET)
/****************  MT7510/7520 & EN7512 End  ********************/


/********************************
 *        GDM1 Register         *
 ********************************/
#define GDMA1_BASE     		(FE_BASE + 0x0500)
#define GDMA1_FWD_FQ        (FE_BASE + 0x04fc)

#define GDMA1_FWD_CFG       (GDMA1_BASE + 0x00)
#define GDMA1_FWD_CFG_UCDP_OFFSET		0
#define GDMA1_FWD_CFG_MCDP_OFFSET		4
#define GDMA1_FWD_CFG_BCDP_OFFSET		8
#define GDMA1_FWD_CFG_MYDP_OFFSET		12
#define GDMA1_FWD_CFG_DP_LENGTH			4
#define GDMA1_FWD_CFG_STRIP_OFFSET	16
#define GDMA1_FWD_CFG_RUT_OFFSET		17
#define GDMA1_FWD_CFG_BRG_OFFSET		18
#define GDMA1_FWD_CFG_UDP_OFFSET		20
#define GDMA1_FWD_CFG_TCP_OFFSET		21
#define GDMA1_FWD_CFG_IP_OFFSET			22
#define GDMA1_FWD_CFG_ETH_OFFSET		23
#define GDMA1_FWD_CFG_SHORT_OFFSET	24
#define GDMA1_FWD_CFG_LONG_OFFSET		25
#define GDMA1_FWD_CFG_RETRY_OFFSET	27
#define GDMA1_FWD_CFG_STAG_OFFSET		28
#define GDMA1_FWD_CFG_CTAG_OFFSET		29
#define GDMA1_FWD_CFG_DSCP_OFFSET		30
#define GDMA1_FWD_CFG_VIP_OFFSET		31
#define GDMA1_SHRP_CFG      (GDMA1_BASE + 0x04)
#define GDMA_SHRP_CFG_EN_OFFSET		31
#define GDMA_SHRP_CFG_TICK_OFFSET	29
#define GDMA_SHRP_CFG_RATE_OFFSET	8
#define GDMA_SHRP_CFG_RATE_LENGTH	21
#define GDMA_SHRP_CFG_BUCK_OFFSET	1
#define GDMA1_MAC_ADRL      (GDMA1_BASE + 0x08)
#define GDMA1_MAC_ADRH      (GDMA1_BASE + 0x0c)
#define GDMA1_VLAN_CHECK    (GDMA1_BASE + 0x10)
#define GDMA_VLAN_CHECK_SP_OFFSET	0
#define GDMA1_LEN_CFG       (GDMA1_BASE + 0x14)
#define GDMA_LEN_CFG_RUNT_OFFSET	0
#define GDMA_LEN_CFG_LONG_OFFSET	16
#define GDMA_LEN_CFG_LENGTH			14
#define GDMA1_LAN_PCP       (GDMA1_BASE + 0x18)
#define GDMA_PCP_CDM_TX_8P0D_OFFSET  0
#define GDMA_PCP_CDM_TX_7P1D_OFFSET  1
#define GDMA_PCP_CDM_TX_6P2D_OFFSET  2
#define GDMA_PCP_CDM_TX_5P3D_OFFSET  3
#define GDMA_PCP_CDM_RX_8P0D_OFFSET  4
#define GDMA_PCP_CDM_RX_7P1D_OFFSET  5
#define GDMA_PCP_CDM_RX_6P2D_OFFSET  6
#define GDMA_PCP_CDM_RX_5P3D_OFFSET  7
#define GDMA_PCP_GDM_TX_8P0D_OFFSET  8
#define GDMA_PCP_GDM_TX_7P1D_OFFSET  9
#define GDMA_PCP_GDM_TX_6P2D_OFFSET  10
#define GDMA_PCP_GDM_TX_5P3D_OFFSET  11
#define GDMA_PCP_GDM_RX_8P0D_OFFSET  12
#define GDMA_PCP_GDM_RX_7P1D_OFFSET  13
#define GDMA_PCP_GDM_RX_6P2D_OFFSET  14
#define GDMA_PCP_GDM_RX_5P3D_OFFSET  15
#define GDMA1_LPBK_CFG      (GDMA1_BASE + 0x1c)
#define GDMA1_CHN_RLS       (GDMA1_BASE + 0x20)

#define GDMA1_TXCHN_EN      (GDMA1_BASE + 0x24)
#define GDMA1_RXCHN_EN      (GDMA1_BASE + 0x28)
#define GDMA1_RXCHN_FP      (GDMA1_BASE + 0x2c)
#define FE_CPORT_CFG        (GDMA1_BASE + 0x40)
#define FE_CPORT_CHN_MAP    (GDMA1_BASE + 0x44)
#define FE_CPORT_SHRP_CFG   (GDMA1_BASE + 0x48)
#define GDMA1_MIB_CLER			(GDMA1_BASE + 0xf0)	

#define GDMA1_TX_CHN_VLD     (GDMA1_BASE + 0x70)
#define GDMA1_RX_CHN_VLD     (GDMA1_BASE + 0x74)

#define CDMA1_TX_OK_CNT             (GDMA1_BASE + 0x80)
#define CDMA1_RXCPU_OK_CNT          (GDMA1_BASE + 0x90)
#define CDMA1_RXHWF_OK_CNT          (GDMA1_BASE + 0x94)
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
#define CDMA1_RXCPU_KA_CNT          (GDMA1_BASE + 0x8c)
#define CDMA1_RXHWF_FAST_ALL_CNT    (GDMA1_BASE + 0x98)
#define CDMA1_RXOQ5_OK_CNT			(GDMA1_BASE + 0x9c) /* default for TSO */
#define CDMA1_RXCPU_DROP_CNT        (GDMA1_BASE + 0xa0)
#define CDMA1_RXHWF_DROP_CNT        (GDMA1_BASE + 0xa4)
#define CDMA1_RXHWF_FAST_DROP_CNT   (GDMA1_BASE + 0xa8)
#define CDMA1_RXOQ5_DROP_CNT		(GDMA1_BASE + 0xac) /* default for TSO */
#define CDMA1_RXCPU0_OK_CNT         (GDMA1_BASE + 0xb0)
#define CDMA1_RXCPU1_OK_CNT         (GDMA1_BASE + 0xb4)
#define CDMA1_RXCPU2_OK_CNT         (GDMA1_BASE + 0xb8)
#define CDMA1_RXCPU3_OK_CNT         (GDMA1_BASE + 0xbc)
#define CDMA1_RXCPU0_DROP_CNT       (GDMA1_BASE + 0xd0)
#define CDMA1_RXCPU1_DROP_CNT       (GDMA1_BASE + 0xd4)
#define CDMA1_RXCPU2_DROP_CNT       (GDMA1_BASE + 0xd8)
#define CDMA1_RXCPU3_DROP_CNT       (GDMA1_BASE + 0xdc)
#else
#define CDMA1_RXCPU_KA_CNT          (GDMA1_BASE + 0x98)
#define CDMA1_RXCPU_DROP_CNT        (GDMA1_BASE + 0xa0)
#define CDMA1_RXHWF_DROP_CNT        (GDMA1_BASE + 0xa4)
#define CDMA1_RXCPU0_OK_CNT         (GDMA1_BASE + 0xa8)
#define CDMA1_RXCPU1_OK_CNT         (GDMA1_BASE + 0xac)
#define CDMA1_RXHWF_FAST_ALL_CNT    (GDMA1_BASE + 0xb0)
#define CDMA1_RXCPU0_DROP_CNT       (GDMA1_BASE + 0xb4)
#define CDMA1_RXCPU1_DROP_CNT       (GDMA1_BASE + 0xb8)
#define CDMA1_RXHWF_FAST_DROP_CNT   (GDMA1_BASE + 0xbc)
#endif

//*gdm sptag setting*//


#define GDMA1_SPTAG_SET0	 (GDMA1_BASE + 0x240)
#define GDMA1_SPTAG_SET1	 (GDMA1_BASE + 0x244)
#define GDMA1_SPTAG_SET2	 (GDMA1_BASE + 0x248)
#define GDMA1_SPTAG_SET3	 (GDMA1_BASE + 0x24c)


#define GDMA1_CHN0_SPTAG_SET0	 (GDMA1_BASE + 0x240)
#define GDMA1_CHN0_SPTAG_SET1	 (GDMA1_BASE + 0x244)
#define GDMA1_CHN0_SPTAG_SET2	 (GDMA1_BASE + 0x248)
#define GDMA1_CHN0_SPTAG_SET3	 (GDMA1_BASE + 0x24c)
#define GDMA1_CHN1_SPTAG_SET0	 (GDMA1_BASE + 0x250)
#define GDMA1_CHN1_SPTAG_SET1	 (GDMA1_BASE + 0x254)
#define GDMA1_CHN1_SPTAG_SET2	 (GDMA1_BASE + 0x258)
#define GDMA1_CHN1_SPTAG_SET3	 (GDMA1_BASE + 0x25c)
#define GDMA1_CHN2_SPTAG_SET0	 (GDMA1_BASE + 0x260)
#define GDMA1_CHN2_SPTAG_SET1	 (GDMA1_BASE + 0x264)
#define GDMA1_CHN2_SPTAG_SET2	 (GDMA1_BASE + 0x268)
#define GDMA1_CHN2_SPTAG_SET3	 (GDMA1_BASE + 0x26c)
#define GDMA1_SRC_PORT_SET	 	 (GDMA1_BASE + 0x23c)


#define GDMA2_CHN0_SPTAG_SET0	 (GDMA2_BASE + 0x240)
#define GDMA2_CHN0_SPTAG_SET1	 (GDMA2_BASE + 0x244)
#define GDMA2_CHN0_SPTAG_SET2	 (GDMA2_BASE + 0x248)
#define GDMA2_CHN0_SPTAG_SET3	 (GDMA2_BASE + 0x24c)
#define GDMA2_CHN1_SPTAG_SET0	 (GDMA2_BASE + 0x250)
#define GDMA2_CHN1_SPTAG_SET1	 (GDMA2_BASE + 0x254)
#define GDMA2_CHN1_SPTAG_SET2	 (GDMA2_BASE + 0x258)
#define GDMA2_CHN1_SPTAG_SET3	 (GDMA2_BASE + 0x25c)
#define GDMA2_CHN2_SPTAG_SET0	 (GDMA2_BASE + 0x260)
#define GDMA2_CHN2_SPTAG_SET1	 (GDMA2_BASE + 0x264)
#define GDMA2_CHN2_SPTAG_SET2	 (GDMA2_BASE + 0x268)
#define GDMA2_CHN2_SPTAG_SET3	 (GDMA2_BASE + 0x26c)
#define GDMA2_SRC_PORT_SET	 	 (GDMA2_BASE + 0x23c)


#define GDMA3_CHN0_SPTAG_SET0	 (GDMA3_BASE + 0x240)
#define GDMA3_CHN0_SPTAG_SET1	 (GDMA3_BASE + 0x244)
#define GDMA3_CHN0_SPTAG_SET2	 (GDMA3_BASE + 0x248)
#define GDMA3_CHN0_SPTAG_SET3	 (GDMA3_BASE + 0x24c)
#define GDMA3_CHN1_SPTAG_SET0	 (GDMA3_BASE + 0x250)
#define GDMA3_CHN1_SPTAG_SET1	 (GDMA3_BASE + 0x254)
#define GDMA3_CHN1_SPTAG_SET2	 (GDMA3_BASE + 0x258)
#define GDMA3_CHN1_SPTAG_SET3	 (GDMA3_BASE + 0x25c)
#define GDMA3_CHN2_SPTAG_SET0	 (GDMA3_BASE + 0x260)
#define GDMA3_CHN2_SPTAG_SET1	 (GDMA3_BASE + 0x264)
#define GDMA3_CHN2_SPTAG_SET2	 (GDMA3_BASE + 0x268)
#define GDMA3_CHN2_SPTAG_SET3	 (GDMA3_BASE + 0x26c)
#define GDMA3_SRC_PORT_SET	 	 (GDMA3_BASE + 0x23c)
/* GDMA1 count define */
#define GDMA1_COUNT_BASE 	    (FE_BASE + 0x600)
#define GDMA1_TX_OK_CNT_H		(GDMA1_COUNT_BASE + 0x180)
#define GDMA1_TX_GET_CNT        (GDMA1_COUNT_BASE + 0x00)
#define GDMA1_TX_OK_CNT         (GDMA1_COUNT_BASE + 0x04)
#define GDMA1_TX_DROP_CNT 	    (GDMA1_COUNT_BASE + 0x08)
#define GDMA1_TX_OK_BYTE_CNT    (GDMA1_COUNT_BASE + 0x0c)

#define GDMA1_RX_OK_CNT         (GDMA1_COUNT_BASE + 0x48)
#define GDMA1_RX_FC_DROP_CNT    (GDMA1_COUNT_BASE + 0x4c)
#define GDMA1_RX_RC_DROP_CNT    (GDMA1_COUNT_BASE + 0x50)
#define GDMA1_RX_OVER_DROP_CNT  (GDMA1_COUNT_BASE + 0x54)
#define GDMA1_RX_ERROR_DROP_CNT (GDMA1_COUNT_BASE + 0x58)
#define GDMA1_RX_BYTECNT        (GDMA1_COUNT_BASE + 0x5c)

#define GDMA1_TX_RC_CFG  (GDMA1_BASE + 0x04)
#define GDMA1_RX_RC_CFG  (GDMA1_BASE + 0xc0)
#define GDMA1_RC_CFG  	 (GDMA1_BASE + 0xc4)
#define GDMA1_RC_DATA_L  (GDMA1_BASE + 0xc8)
#define GDMA1_RC_DATA_H  (GDMA1_BASE + 0xcc)
#define GDMA1_RX_MAC_FILTER_SHP  (GDMA1_BASE + 0x1B8)

/********************************
 *        CDM2 Register         *
 ********************************/
#define CDMA2_BASE     		 (FE_BASE + 0x1400)
#define CDMA2_VLAN_CTRL      (CDMA2_BASE + 0x00)
#define CDMA2_PPP_GEN        (CDMA2_BASE + 0x04)
#define CDMA2_FWD_CFG        (CDMA2_BASE + 0x08)
#define CDMA2_HWF_CHN_EN     (CDMA2_BASE + 0x0c)
#define CDMA2_CRSN_QSEL0     (CDMA2_BASE + 0x10)
#define CDMA2_CRSN_QSEL1     (CDMA2_BASE + 0x14)

#define CMD2_RXQ_RED_CFG     (CDMA2_BASE + 0x20)

/********************************
 *        GDM2 Register         *
 ********************************/
#define GDMA2_BASE     		(FE_BASE + 0x1500)
#define GDMA2_FWD_FQ        (FE_BASE + 0x14fc)
#define GDMA2_FWD_CFG       (GDMA2_BASE + 0x00)
#define GDMA2_TX_FAVOR_OAM_OFFSET		19
#define GDMA2_SHRP_CFG      (GDMA2_BASE + 0x04)
#define GDMA2_MAC_ADRL      (GDMA2_BASE + 0x08)
#define GDMA2_MAC_ADRH      (GDMA2_BASE + 0x0c)
#define GDMA2_VLAN_CHECK    (GDMA2_BASE + 0x10)
#define GDMA2_LEN_CFG    	(GDMA2_BASE + 0x14)
#define GDMA2_WAN_PCP     	(GDMA2_BASE + 0x18)
#define GDMA2_LPBP_CFG      (GDMA2_BASE + 0x1c)
#define GDMA2_CHN_RLS       (GDMA2_BASE + 0x20)
#define GDMA2_TXCHN_EN       (GDMA2_BASE + 0x24)
#define GDMA2_RXCHN_EN       (GDMA2_BASE + 0x28)
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
#define GDMA2_RXCHN_FP       (GDMA2_BASE + 0x4c)
#else
#define GDMA2_RXCHN_FP       (GDMA2_BASE + 0x2c)
#endif

#define GDMA2_RXUC_SHPR_CFG  (GDMA2_BASE + 0x30)
#define GDMA2_RXBC_SHPR_CFG  (GDMA2_BASE + 0x34)
#define GDMA2_RXMC_SHPR_CFG  (GDMA2_BASE + 0x38)
#define GDMA2_RXOC_SHPR_CFG  (GDMA2_BASE + 0x3c)
#define GDMA2_TX_CHN_BUF     (GDMA2_BASE + 0x6c)
#define GDMA2_MIB_CLER			(GDMA2_BASE + 0xf0)	
#define GDMA2_TX_CHN_VLD     (GDMA2_BASE + 0x70)
#define GDMA2_RX_CHN_VLD     (GDMA2_BASE + 0x74)

#define GDMA2_TX_RC_CFG  (GDMA2_BASE + 0x04)
#define GDMA2_RX_RC_CFG  (GDMA2_BASE + 0xc0)
#define GDMA2_RC_CFG  	 (GDMA2_BASE + 0xc4)
#define GDMA2_RC_DATA_L  (GDMA2_BASE + 0xc8)
#define GDMA2_RC_DATA_H  (GDMA2_BASE + 0xcc)
#define GDMA2_RX_MAC_FILTER_SHP  (GDMA2_BASE + 0x1B8)

#define CDMA2_TX_OK_CNT             (GDMA2_BASE + 0x80)
#define CDMA2_RXCPU_OK_CNT          (GDMA2_BASE + 0x90)
#define CDMA2_RXHWF_OK_CNT          (GDMA2_BASE + 0x94)
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
#define CDMA2_RXCPU_KA_CNT          (GDMA2_BASE + 0x8c)
#define CDMA2_RXHWF_FAST_ALL_CNT    (GDMA2_BASE + 0x98)
#define CDMA2_RXOQ5_OK_CNT			(GDMA2_BASE + 0x9c) /* default for TSO */
#define CDMA2_RXCPU_DROP_CNT        (GDMA2_BASE + 0xa0)
#define CDMA2_RXHWF_DROP_CNT        (GDMA2_BASE + 0xa4)
#define CDMA2_RXHWF_FAST_DROP_CNT   (GDMA2_BASE + 0xa8)
#define CDMA2_RXOQ5_DROP_CNT		(GDMA2_BASE + 0xac) /* default for TSO */
#define CDMA2_RXCPU0_OK_CNT         (GDMA2_BASE + 0xb0)
#define CDMA2_RXCPU1_OK_CNT         (GDMA2_BASE + 0xb4)
#define CDMA2_RXCPU2_OK_CNT         (GDMA2_BASE + 0xb8)
#define CDMA2_RXCPU3_OK_CNT         (GDMA2_BASE + 0xbc)
#define CDMA2_RXCPU0_DROP_CNT       (GDMA2_BASE + 0xd0)
#define CDMA2_RXCPU1_DROP_CNT       (GDMA2_BASE + 0xd4)
#define CDMA2_RXCPU2_DROP_CNT       (GDMA2_BASE + 0xd8)
#define CDMA2_RXCPU3_DROP_CNT       (GDMA2_BASE + 0xdc)
#else
#define CDMA2_RXCPU_KA_CNT          (GDMA2_BASE + 0x98)
#define CDMA2_RXCPU_DROP_CNT        (GDMA2_BASE + 0xa0)
#define CDMA2_RXHWF_DROP_CNT        (GDMA2_BASE + 0xa4)
#define CDMA2_RXCPU0_OK_CNT         (GDMA2_BASE + 0xa8)
#define CDMA2_RXCPU1_OK_CNT         (GDMA2_BASE + 0xac)
#define CDMA2_RXHWF_FAST_ALL_CNT    (GDMA2_BASE + 0xb0)
#define CDMA2_RXCPU0_DROP_CNT       (GDMA2_BASE + 0xb4)
#define CDMA2_RXCPU1_DROP_CNT       (GDMA2_BASE + 0xb8)
#define CDMA2_RXHWF_FAST_DROP_CNT   (GDMA2_BASE + 0xbc)	
#endif




#define GDMA2_COUNT_BASE 	(FE_BASE + 0x1600)
#define GDMA2_TX_OKCNT_H	(GDMA2_COUNT_BASE + 0x180)
#define GDMA2_TX_GETCNT     (GDMA2_COUNT_BASE + 0x00)
#define GDMA2_TX_OKCNT     	(GDMA2_COUNT_BASE + 0x4)

#define GDMA2_TX_DROPCNT   	(GDMA2_COUNT_BASE + 0x8)
#define GDMA2_TX_OKBYTE_CNT   	(GDMA2_COUNT_BASE + 0xc)

#define GDMA2_TX_ETHCNT   	(GDMA2_COUNT_BASE + 0x10)
#define GDMA2_TX_ETHLENCNT   	(GDMA2_COUNT_BASE + 0x14)
#define GDMA2_TX_ETHDROPCNT   	(GDMA2_COUNT_BASE + 0x18)
#define GDMA2_TX_ETHBCDCNT   	(GDMA2_COUNT_BASE + 0x1C)
#define GDMA2_TX_ETHMULTICASTCNT   	(GDMA2_COUNT_BASE + 0x20)
#define GDMA2_TX_ETH_LESS64_CNT   	(GDMA2_COUNT_BASE + 0x24)
#define GDMA2_TX_ETH_MORE1518_CNT   	(GDMA2_COUNT_BASE + 0x28)
#define GDMA2_TX_ETH_64_CNT   			(GDMA2_COUNT_BASE + 0x2C)
#define GDMA2_TX_ETH_65_TO_127_CNT   	(GDMA2_COUNT_BASE + 0x30)
#define GDMA2_TX_ETH_128_TO_255_CNT   	(GDMA2_COUNT_BASE + 0x34)
#define GDMA2_TX_ETH_256_TO_511_CNT   	(GDMA2_COUNT_BASE + 0x38)
#define GDMA2_TX_ETH_512_TO_1023_CNT   	(GDMA2_COUNT_BASE + 0x3C)
#define GDMA2_TX_ETH_1024_TO_1518_CNT   	(GDMA2_COUNT_BASE + 0x40)

#define GDMA2_RX_OKCNT     	(GDMA2_COUNT_BASE + 0x48)
#define GDMA2_RX_FCDROPCNT     	(GDMA2_COUNT_BASE + 0x4c)
#define GDMA2_RX_RCDROPCNT     	(GDMA2_COUNT_BASE + 0x50)

#define GDMA2_RX_OVDROPCNT    (GDMA2_COUNT_BASE + 0x54)
#define GDMA2_RX_ERRDROPCNT    (GDMA2_COUNT_BASE + 0x58)
#define GDMA2_RX_OKBYTECNT    (GDMA2_COUNT_BASE + 0x5c)

#define GDMA2_RX_ETHERPCNT  (GDMA2_COUNT_BASE + 0x60)
#define GDMA2_RX_ETHERPLEN  (GDMA2_COUNT_BASE + 0x64)
#define GDMA2_RX_ETHDROPCNT (GDMA2_COUNT_BASE + 0x68)
#define GDMA2_RX_ETHBCCNT   (GDMA2_COUNT_BASE + 0x6c)
#define GDMA2_RX_ETHMCCNT   (GDMA2_COUNT_BASE + 0x70)
#define GDMA2_RX_ETHCRCCNT  (GDMA2_COUNT_BASE + 0x74)
#define GDMA2_RX_ETHFRACCNT (GDMA2_COUNT_BASE + 0x78)
#define GDMA2_RX_ETHJABCNT  (GDMA2_COUNT_BASE + 0x7c)
#define GDMA2_RX_ETHRUNTCNT (GDMA2_COUNT_BASE + 0x80)
#define GDMA2_RX_ETHLONGCNT (GDMA2_COUNT_BASE + 0x84)
#define GDMA2_RX_ETH_64_CNT (GDMA2_COUNT_BASE + 0x88)
#define GDMA2_RX_ETH_65_TO_127_CNT (GDMA2_COUNT_BASE + 0x8C)
#define GDMA2_RX_ETH_128_TO_255_CNT (GDMA2_COUNT_BASE + 0x90)
#define GDMA2_RX_ETH_256_TO_511_CNT (GDMA2_COUNT_BASE + 0x94)
#define GDMA2_RX_ETH_512_TO_1023_CNT (GDMA2_COUNT_BASE + 0x98)
#define GDMA2_RX_ETH_1024_TO_1518_CNT (GDMA2_COUNT_BASE + 0x9C)

#define GDMA2_TX_ETHCNT_H	(GDMA2_COUNT_BASE + 0x188)
#define GDMA2_TX_ETHLENCNT_H	(GDMA2_COUNT_BASE + 0x18c)
#define GDMA2_RX_OKCNT_H	(GDMA2_COUNT_BASE + 0x190)
#define GDMA2_RX_OKBYTECNT_H	(GDMA2_COUNT_BASE + 0x194)
#define GDMA2_RX_ETHERPCNT_H	(GDMA2_COUNT_BASE + 0x198)
#define GDMA2_RX_ETHERPLEN_H	(GDMA2_COUNT_BASE + 0x19c)
#define GDMA2_TX_ETH_64_CNT_H		(GDMA2_COUNT_BASE + 0x1b8)
#define GDMA2_TX_ETH_65_TO_127_CNT_H	(GDMA2_COUNT_BASE + 0x1bc)
#define GDMA2_TX_ETH_128_TO_255_CNT_H	(GDMA2_COUNT_BASE + 0x1c0)
#define GDMA2_TX_ETH_256_TO_511_CNT_H	(GDMA2_COUNT_BASE + 0x1c4)
#define GDMA2_TX_ETH_512_TO_1023_CNT_H	(GDMA2_COUNT_BASE + 0x1c8)
#define GDMA2_TX_ETH_1024_TO_1518_CNT_H	(GDMA2_COUNT_BASE + 0x1cc)
#define GDMA2_RX_ETH_64_CNT_H			(GDMA2_COUNT_BASE + 0x1e8)
#define GDMA2_RX_ETH_65_TO_127_CNT_H	(GDMA2_COUNT_BASE + 0x1ec)
#define GDMA2_RX_ETH_128_TO_255_CNT_H	(GDMA2_COUNT_BASE + 0x1f0)
#define GDMA2_RX_ETH_256_TO_511_CNT_H	(GDMA2_COUNT_BASE + 0x1f4)
#define GDMA2_RX_ETH_512_TO_1023_CNT_H	(GDMA2_COUNT_BASE + 0x1f8)
#define GDMA2_RX_ETH_1024_TO_1518_CNT_H	(GDMA2_COUNT_BASE + 0x1fc)

#define CDM3_TMBI_FRAG	(FE_BASE + 0x2038)
#define CDM3_RMBI_FRAG	(FE_BASE + 0x203C)

/********************************
 *        GDM3 Register         *
 ********************************/
#define GDMA3_BASE     		(FE_BASE + 0x1100)
#define GDMA3_FWD_FQ          (FE_BASE + 0x10fc)
#define GDMA3_FWD_CFG       (GDMA3_BASE + 0x00)

#define STAG_EN_OFFSET	0
#define GDMA3_INGRESS_CFG	(GDMA3_BASE + 0x10)
#define	GDMA3DisableSTAG()	IO_CBITS(GDMA3_INGRESS_CFG, (1 << STAG_EN_OFFSET))

#define GDMA3_SRC_PORT_SET	(GDMA3_BASE + 0x23c)

#define GDMA3_COUNT_BASE     (GDMA3_BASE + 0x100)
#define GDMA3_TX_GETCNT      (GDMA3_COUNT_BASE + 0x00)
#define GDMA3_TX_OKCNT_L     (GDMA3_COUNT_BASE + 0x4)
#define GDMA3_TX_OKCNT_H     (GDMA3_COUNT_BASE + 0x180)
#define GDMA3_TX_DROPCNT     (GDMA3_COUNT_BASE + 0x8)
#define GDMA3_TX_OKBYTE_CNT  (GDMA3_COUNT_BASE + 0xc)
    
#define GDMA3_RX_OKCNT       (GDMA3_COUNT_BASE + 0x48)
#define GDMA3_RX_FCDROPCNT   (GDMA3_COUNT_BASE + 0x4c)
#define GDMA3_RX_RCDROPCNT   (GDMA3_COUNT_BASE + 0x50)
    
#define GDMA3_RX_OVDROPCNT   (GDMA3_COUNT_BASE + 0x54)
#define GDMA3_RX_ERRDROPCNT  (GDMA3_COUNT_BASE + 0x58)

#define GDMA3_LEN_CFG        (GDMA3_BASE + 0x14)  
#define GDMA3_WAN_PCP        (GDMA3_BASE + 0x18)

#define GDMA3_CHN_RLS	(GDMA3_BASE + 0x20)
#define GDMA3_TXCHN_EN	(GDMA3_BASE + 0x24)
#define GDMA3_RXCHN_EN	(GDMA3_BASE + 0x28)
#define GDMA3_TX_CHN_VLD (GDMA3_BASE + 0x70)
#define GDMA3_RX_CHN_VLD (GDMA3_BASE + 0x74)

#define GDMA3_TX_RC_CFG  (GDMA3_BASE + 0x04)
#define GDMA3_RX_RC_CFG  (GDMA3_BASE + 0xc0)
#define GDMA3_RC_CFG  	 (GDMA3_BASE + 0xc4)
#define GDMA3_RC_DATA_L  (GDMA3_BASE + 0xc8)
#define GDMA3_RC_DATA_H  (GDMA3_BASE + 0xcc)
#define GDMA3_RX_MAC_FILTER_SHP  (GDMA3_BASE + 0x1B8)

#define GDMA3_MIB_CFG 		 (GDMA3_BASE + 0xf4)

#define GDM3_TX_MIB_SPLIT_EN_OFFSET		17
#define GDM3_RX_MIB_SPLIT_EN_OFFSET		16
#define GDM3_TX_MIB_ID_OFFSET		8
#define GDM3_RX_MIB_ID_OFFSET		0


#define GDMA3_TX_GET_CNT_L 	     	 (GDMA3_COUNT_BASE + 0x000)
#define GDMA3_TX_OK_CNT_L            (GDMA3_COUNT_BASE + 0x004)
#define GDMA3_TX_DROP_CNT            (GDMA3_COUNT_BASE + 0x008)
#define GDMA3_TX_OK_BYTE_CNT         (GDMA3_COUNT_BASE + 0x00c)
#define GDMA3_TX_ETH_PKT_CNT         (GDMA3_COUNT_BASE + 0x010)
#define GDMA3_TX_ETH_BYTE_CNT        (GDMA3_COUNT_BASE + 0x014)
#define GDMA3_TX_ETH_DROP_CNT        (GDMA3_COUNT_BASE + 0x018)
#define GDMA3_TX_ETH_BC_CNT          (GDMA3_COUNT_BASE + 0x01c)
#define GDMA3_TX_ETH_MC_CNT          (GDMA3_COUNT_BASE + 0x020)
#define GDMA3_TX_ETH_RUNT_CNT        (GDMA3_COUNT_BASE + 0x024)
#define GDMA3_TX_ETH_LONG_CNT        (GDMA3_COUNT_BASE + 0x028)
#define GDMA3_TX_ETH_E64_CNT         (GDMA3_COUNT_BASE + 0x02c)
#define GDMA3_TX_ETH_L64_CNT         (GDMA3_COUNT_BASE + 0x030)
#define GDMA3_TX_ETH_L127_CNT        (GDMA3_COUNT_BASE + 0x034)
#define GDMA3_TX_ETH_L255_CNT        (GDMA3_COUNT_BASE + 0x038)
#define GDMA3_TX_ETH_L511_CNT        (GDMA3_COUNT_BASE + 0x03c)
#define GDMA3_TX_ETH_L1023_CNT       (GDMA3_COUNT_BASE + 0x040)
#define GDMA3_RX_MAC_DROP_CNT        (GDMA3_COUNT_BASE + 0x044)
#define GDMA3_RX_OK_CNT              (GDMA3_COUNT_BASE + 0x048)
#define GDMA3_RX_FC_DROP_CNT         (GDMA3_COUNT_BASE + 0x04c)
#define GDMA3_RX_RC_DROP_CNT         (GDMA3_COUNT_BASE + 0x050)
#define GDMA3_RX_OVER_DROP_CNT       (GDMA3_COUNT_BASE + 0x054)
#define GDMA3_RX_ERROR_DROP_CNT      (GDMA3_COUNT_BASE + 0x058)
#define GDMA3_RX_OK_BYTE_CNT         (GDMA3_COUNT_BASE + 0x05c)
#define GDMA3_RX_ETH_PKT_CNT         (GDMA3_COUNT_BASE + 0x060)
#define GDMA3_RX_ETH_BYTE_CNT        (GDMA3_COUNT_BASE + 0x064)
#define GDMA3_RX_ETH_DROP_CNT        (GDMA3_COUNT_BASE + 0x068)
#define GDMA3_RX_ETH_BC_CNT          (GDMA3_COUNT_BASE + 0x06c)
#define GDMA3_RX_ETH_MC_CNT          (GDMA3_COUNT_BASE + 0x070)
#define GDMA3_RX_ETH_CRCE_CNT        (GDMA3_COUNT_BASE + 0x074)
#define GDMA3_RX_ETH_FRAG_CNT        (GDMA3_COUNT_BASE + 0x078)
#define GDMA3_RX_ETH_JABBER_CNT      (GDMA3_COUNT_BASE + 0x07c)
#define GDMA3_RX_ETH_RUNT_CNT        (GDMA3_COUNT_BASE + 0x080)
#define GDMA3_RX_ETH_LONG_CNT        (GDMA3_COUNT_BASE + 0x084)
#define GDMA3_RX_ETH_E64_CNT         (GDMA3_COUNT_BASE + 0x088)
#define GDMA3_RX_ETH_L64_CNT         (GDMA3_COUNT_BASE + 0x08c)
#define GDMA3_RX_ETH_L127_CNT        (GDMA3_COUNT_BASE + 0x090)
#define GDMA3_RX_ETH_L255_CNT        (GDMA3_COUNT_BASE + 0x094)
#define GDMA3_RX_ETH_L511_CNT        (GDMA3_COUNT_BASE + 0x098)
#define GDMA3_RX_ETH_L1023_CNT       (GDMA3_COUNT_BASE + 0x09c)
#define GDMA3_RX_ETH_OK_CNT          (GDMA3_COUNT_BASE + 0x0A0)
#define GDMA3_RX_OAM_CNT             (GDMA3_COUNT_BASE + 0x0A4)


#define GDMA3_RX_MAC_FILTER_CFG     (GDMA3_COUNT_BASE + 0x0B0)
#define GDMA3_RX_MAC_FILTER_EN       (GDMA3_COUNT_BASE + 0x0b4)
#define GDMA3_MAC_FILTER_PATN_H      (GDMA3_COUNT_BASE + 0x0c0)
#define GDMA3_MAC_FILTER_PATN_L      (GDMA3_COUNT_BASE + 0x0c4)
#define GDMA3_SPTAG_SET_0            (GDMA3_COUNT_BASE + 0x140)
#define GDMA3_SPTAG_SET_1            (GDMA3_COUNT_BASE + 0x144)
#define GDMA3_SPTAG_SET_2            (GDMA3_COUNT_BASE + 0x148)
#define GDMA3_SPTAG_SET_3	     	 (GDMA3_COUNT_BASE + 0x14C)
//#define GDMA3_SRC_PORT_SET           (GDMA3_COUNT_BASE + 0x150)


#define GDMA3_TX_OK_CNT_H            (GDMA3_COUNT_BASE + 0x180)
#define GDMA3_TX_OK_BYTE_CNT_H       (GDMA3_COUNT_BASE + 0x184)
#define GDMA3_TX_ETH_PKT_CNT_H       (GDMA3_COUNT_BASE + 0x188)
#define GDMA3_TX_ETH_BYTE_CNT_H      (GDMA3_COUNT_BASE + 0x18C)
#define GDMA3_RX_OK_CNT_H            (GDMA3_COUNT_BASE + 0x190)
#define GDMA3_RX_OK_BYTE_CNT_H       (GDMA3_COUNT_BASE + 0x194)
#define GDMA3_RX_ETH_PKT_CNT_H       (GDMA3_COUNT_BASE + 0x198)
#define GDMA3_RX_ETH_BYTE_CNT_H      (GDMA3_COUNT_BASE + 0x19C)
#define GDMA3_TX_OK_BC_BYTE_CNT_L    (GDMA3_COUNT_BASE + 0x1A0)
#define GDMA3_TX_OK_BC_BYTE_CNT_H    (GDMA3_COUNT_BASE + 0x1A4)
#define GDMA3_TX_OK_MC_BYTE_CNT_L    (GDMA3_COUNT_BASE + 0x1A8)
#define GDMA3_TX_OK_MC_BYTE_CNT_H    (GDMA3_COUNT_BASE + 0x1AC)
#define GDMA3_TX_BC_PKT_CNT_H        (GDMA3_COUNT_BASE + 0x1B0)
#define GDMA3_TX_MC_PKT_CNT_H        (GDMA3_COUNT_BASE + 0x1B4)
#define GDMA3_TX_ETH_E64_CNT_H       (GDMA3_COUNT_BASE + 0x1B8)
#define GDMA3_TX_ETH_L64_CNT_H       (GDMA3_COUNT_BASE + 0x1BC)
#define GDMA3_TX_ETH_L127_CNT_H      (GDMA3_COUNT_BASE + 0x1C0)
#define GDMA3_TX_ETH_L255_CNT_H      (GDMA3_COUNT_BASE + 0x1C4)
#define GDMA3_TX_ETH_L511_CNT_H      (GDMA3_COUNT_BASE + 0x1C8)
#define GDMA3_TX_ETH_L1023_CNT_H     (GDMA3_COUNT_BASE + 0x1CC)
#define GDMA3_RX_OK_BC_BYTE_CNT_L    (GDMA3_COUNT_BASE + 0x1D0)
#define GDMA3_RX_OK_BC_BYTE_CNT_H    (GDMA3_COUNT_BASE + 0x1D4)
#define GDMA3_RX_OK_MC_BYTE_CNT_L    (GDMA3_COUNT_BASE + 0x1D8)
#define GDMA3_RX_OK_MC_BYTE_CNT_H    (GDMA3_COUNT_BASE + 0x1DC)
#define GDMA3_RX_BC_PKT_CNT_H        (GDMA3_COUNT_BASE + 0x1E0)
#define GDMA3_RX_MC_PKT_CNT_H        (GDMA3_COUNT_BASE + 0x1E4)
#define GDMA3_RX_ETH_E64_CNT_H       (GDMA3_COUNT_BASE + 0x1E8)
#define GDMA3_RX_ETH_L64_CNT_H       (GDMA3_COUNT_BASE + 0x1EC)
#define GDMA3_RX_ETH_L127_CNT_H      (GDMA3_COUNT_BASE + 0x1F0)
#define GDMA3_RX_ETH_L255_CNT_H      (GDMA3_COUNT_BASE + 0x1F4)
#define GDMA3_RX_ETH_L511_CNT_H      (GDMA3_COUNT_BASE + 0x1F8)
#define GDMA3_RX_ETH_L1023_CNT_H     (GDMA3_COUNT_BASE + 0x1FC)


/********************************
 *        GDM4 Register         *
 ********************************/
#define GDMA4_BASE           (FE_BASE + 0x2400)
#define GDMA4_FWD_CFG        (GDMA4_BASE + 0x100)
#define GDMA4_LEN_CFG        (GDMA4_BASE + 0x114)
#define GDMA4_LAN_PCP        (GDMA4_BASE + 0x118)


#define GDMA4_TX_RC_CFG      (GDMA4_BASE + 0x104)
#define GDMA4_RC_CFG         (GDMA4_BASE + 0x1c4)
#define GDMA4_RC_DATA_L      (GDMA4_BASE + 0x1c8)
#define GDMA4_RC_DATA_H      (GDMA4_BASE + 0x1cc)

#define GDMA4_CHN_RLS      (GDMA4_BASE + 0x120)
#define MBI_RX_BUSY          (27)
#define MBI_TX_BUSY          (19)
#define GDMA4_TXCHN_EN	(GDMA3_BASE + 0x124)
#define GDMA4_RXCHN_EN	(GDMA3_BASE + 0x128)
#define GDMA4_TX_CHN_VLD (GDMA3_BASE + 0x170)
#define GDMA4_RX_CHN_VLD (GDMA3_BASE + 0x174)

#define GDMA4_COUNT_BASE     (GDMA4_BASE + 0x200)
#define GDMA4_TX_GETCNT      (GDMA4_COUNT_BASE + 0x0)
#define GDMA4_TX_OKCNT_L     (GDMA4_COUNT_BASE + 0x4)
#define GDMA4_TX_DROPCNT     (GDMA4_COUNT_BASE + 0x8)
#define GDMA4_TX_OKBYTE_CNT  (GDMA4_COUNT_BASE + 0xc)
#define GDMA4_RX_OKCNT_L     (GDMA4_COUNT_BASE + 0x48)
#define GDMA4_RX_FCDROPCNT   (GDMA4_COUNT_BASE + 0x4c)
#define GDMA4_RX_RCDROPCNT   (GDMA4_COUNT_BASE + 0x50)
#define GDMA4_RX_OVDROPCNT   (GDMA4_COUNT_BASE + 0x54)
#define GDMA4_RX_ERRDROPCNT  (GDMA4_COUNT_BASE + 0x58)
#define GDMA4_TX_OKCNT_H     (GDMA4_COUNT_BASE + 0x180)
#define GDMA4_RX_OKCNT_H     (GDMA4_COUNT_BASE + 0x190)

#define SPORT_OFFSET0_SHIFT		(8)
#define SPORT_OFFSET0_MASK		(0xf << SPORT_OFFSET0_SHIFT)
#define SPORT_OFFSET1_SHIFT		(12)
#define SPORT_OFFSET1_MASK		(0xf << SPORT_OFFSET1_SHIFT)
#define SPORT_OFFSET2_SHIFT		(16)
#define SPORT_OFFSET2_MASK		(0xf << SPORT_OFFSET2_SHIFT)

#define GDMA4_SRC_PORT_SET      (GDMA4_BASE + 0x33c)
#define GDMA4_CHN0_SPTAG_SET0	 (GDMA4_BASE + 0x340)
#define GDMA4_CHN0_SPTAG_SET1	 (GDMA4_BASE + 0x344)
#define GDMA4_CHN0_SPTAG_SET2	 (GDMA4_BASE + 0x348)
#define GDMA4_CHN0_SPTAG_SET3	 (GDMA4_BASE + 0x34c)
#define GDMA4_CHN1_SPTAG_SET0	 (GDMA4_BASE + 0x350)
#define GDMA4_CHN1_SPTAG_SET1	 (GDMA4_BASE + 0x354)
#define GDMA4_CHN1_SPTAG_SET2	 (GDMA4_BASE + 0x358)
#define GDMA4_CHN1_SPTAG_SET3	 (GDMA4_BASE + 0x35c)
#define GDMA4_CHN2_SPTAG_SET0	 (GDMA4_BASE + 0x360)
#define GDMA4_CHN2_SPTAG_SET1	 (GDMA4_BASE + 0x364)
#define GDMA4_CHN2_SPTAG_SET2	 (GDMA4_BASE + 0x368)
#define GDMA4_CHN2_SPTAG_SET3	 (GDMA4_BASE + 0x36c)

#define  GDMA4_MIB_CFG 		 (GDMA4_BASE + 0x1f4)

#define GDM4_TX_MIB_SPLIT_EN_OFFSET		17
#define GDM4_RX_MIB_SPLIT_EN_OFFSET		16
#define GDM4_TX_MIB_ID_OFFSET		8
#define GDM4_RX_MIB_ID_OFFSET		0

#define GDMA4_TX_GET_CNT_L 	     	 (GDMA4_COUNT_BASE + 0x000)
#define GDMA4_TX_OK_CNT_L            (GDMA4_COUNT_BASE + 0x004)
#define GDMA4_TX_DROP_CNT            (GDMA4_COUNT_BASE + 0x008)
#define GDMA4_TX_OK_BYTE_CNT         (GDMA4_COUNT_BASE + 0x00c)
#define GDMA4_TX_ETH_PKT_CNT         (GDMA4_COUNT_BASE + 0x010)
#define GDMA4_TX_ETH_BYTE_CNT        (GDMA4_COUNT_BASE + 0x014)
#define GDMA4_TX_ETH_DROP_CNT        (GDMA4_COUNT_BASE + 0x018)
#define GDMA4_TX_ETH_BC_CNT          (GDMA4_COUNT_BASE + 0x01c)
#define GDMA4_TX_ETH_MC_CNT          (GDMA4_COUNT_BASE + 0x020)
#define GDMA4_TX_ETH_RUNT_CNT        (GDMA4_COUNT_BASE + 0x024)
#define GDMA4_TX_ETH_LONG_CNT        (GDMA4_COUNT_BASE + 0x028)
#define GDMA4_TX_ETH_E64_CNT         (GDMA4_COUNT_BASE + 0x02c)
#define GDMA4_TX_ETH_L64_CNT         (GDMA4_COUNT_BASE + 0x030)
#define GDMA4_TX_ETH_L127_CNT        (GDMA4_COUNT_BASE + 0x034)
#define GDMA4_TX_ETH_L255_CNT        (GDMA4_COUNT_BASE + 0x038)
#define GDMA4_TX_ETH_L511_CNT        (GDMA4_COUNT_BASE + 0x03c)
#define GDMA4_TX_ETH_L1023_CNT       (GDMA4_COUNT_BASE + 0x040)
#define GDMA4_RX_MAC_DROP_CNT        (GDMA4_COUNT_BASE + 0x044)
#define GDMA4_RX_OK_CNT              (GDMA4_COUNT_BASE + 0x048)
#define GDMA4_RX_FC_DROP_CNT         (GDMA4_COUNT_BASE + 0x04c)
#define GDMA4_RX_RC_DROP_CNT         (GDMA4_COUNT_BASE + 0x050)
#define GDMA4_RX_OVER_DROP_CNT       (GDMA4_COUNT_BASE + 0x054)
#define GDMA4_RX_ERROR_DROP_CNT      (GDMA4_COUNT_BASE + 0x058)
#define GDMA4_RX_OK_BYTE_CNT         (GDMA4_COUNT_BASE + 0x05c)
#define GDMA4_RX_ETH_PKT_CNT         (GDMA4_COUNT_BASE + 0x060)
#define GDMA4_RX_ETH_BYTE_CNT        (GDMA4_COUNT_BASE + 0x064)
#define GDMA4_RX_ETH_DROP_CNT        (GDMA4_COUNT_BASE + 0x068)
#define GDMA4_RX_ETH_BC_CNT          (GDMA4_COUNT_BASE + 0x06c)
#define GDMA4_RX_ETH_MC_CNT          (GDMA4_COUNT_BASE + 0x070)
#define GDMA4_RX_ETH_CRCE_CNT        (GDMA4_COUNT_BASE + 0x074)
#define GDMA4_RX_ETH_FRAG_CNT        (GDMA4_COUNT_BASE + 0x078)
#define GDMA4_RX_ETH_JABBER_CNT      (GDMA4_COUNT_BASE + 0x07c)
#define GDMA4_RX_ETH_RUNT_CNT        (GDMA4_COUNT_BASE + 0x080)
#define GDMA4_RX_ETH_LONG_CNT        (GDMA4_COUNT_BASE + 0x084)
#define GDMA4_RX_ETH_E64_CNT         (GDMA4_COUNT_BASE + 0x088)
#define GDMA4_RX_ETH_L64_CNT         (GDMA4_COUNT_BASE + 0x08c)
#define GDMA4_RX_ETH_L127_CNT        (GDMA4_COUNT_BASE + 0x090)
#define GDMA4_RX_ETH_L255_CNT        (GDMA4_COUNT_BASE + 0x094)
#define GDMA4_RX_ETH_L511_CNT        (GDMA4_COUNT_BASE + 0x098)
#define GDMA4_RX_ETH_L1023_CNT       (GDMA4_COUNT_BASE + 0x09c)
#define GDMA4_RX_ETH_OK_CNT          (GDMA4_COUNT_BASE + 0x0A0)
#define GDMA4_RX_OAM_CNT             (GDMA4_COUNT_BASE + 0x0A4)

#define GDMA4_TX_OK_CNT_H            (GDMA4_COUNT_BASE + 0x180)
#define GDMA4_TX_OK_BYTE_CNT_H       (GDMA4_COUNT_BASE + 0x184)
#define GDMA4_TX_ETH_PKT_CNT_H       (GDMA4_COUNT_BASE + 0x188)
#define GDMA4_TX_ETH_BYTE_CNT_H      (GDMA4_COUNT_BASE + 0x18C)
#define GDMA4_RX_OK_CNT_H            (GDMA4_COUNT_BASE + 0x190)
#define GDMA4_RX_OK_BYTE_CNT_H       (GDMA4_COUNT_BASE + 0x194)
#define GDMA4_RX_ETH_PKT_CNT_H       (GDMA4_COUNT_BASE + 0x198)
#define GDMA4_RX_ETH_BYTE_CNT_H      (GDMA4_COUNT_BASE + 0x19C)
#define GDMA4_TX_OK_BC_BYTE_CNT_L    (GDMA4_COUNT_BASE + 0x1A0)
#define GDMA4_TX_OK_BC_BYTE_CNT_H    (GDMA4_COUNT_BASE + 0x1A4)
#define GDMA4_TX_OK_MC_BYTE_CNT_L    (GDMA4_COUNT_BASE + 0x1A8)
#define GDMA4_TX_OK_MC_BYTE_CNT_H    (GDMA4_COUNT_BASE + 0x1AC)
#define GDMA4_TX_BC_PKT_CNT_H        (GDMA4_COUNT_BASE + 0x1B0)
#define GDMA4_TX_MC_PKT_CNT_H        (GDMA4_COUNT_BASE + 0x1B4)
#define GDMA4_TX_ETH_E64_CNT_H       (GDMA4_COUNT_BASE + 0x1B8)
#define GDMA4_TX_ETH_L64_CNT_H       (GDMA4_COUNT_BASE + 0x1BC)
#define GDMA4_TX_ETH_L127_CNT_H      (GDMA4_COUNT_BASE + 0x1C0)
#define GDMA4_TX_ETH_L255_CNT_H      (GDMA4_COUNT_BASE + 0x1C4)
#define GDMA4_TX_ETH_L511_CNT_H      (GDMA4_COUNT_BASE + 0x1C8)
#define GDMA4_TX_ETH_L1023_CNT_H     (GDMA4_COUNT_BASE + 0x1CC)
#define GDMA4_RX_OK_BC_BYTE_CNT_L    (GDMA4_COUNT_BASE + 0x1D0)
#define GDMA4_RX_OK_BC_BYTE_CNT_H    (GDMA4_COUNT_BASE + 0x1D4)
#define GDMA4_RX_OK_MC_BYTE_CNT_L    (GDMA4_COUNT_BASE + 0x1D8)
#define GDMA4_RX_OK_MC_BYTE_CNT_H    (GDMA4_COUNT_BASE + 0x1DC)
#define GDMA4_RX_BC_PKT_CNT_H        (GDMA4_COUNT_BASE + 0x1E0)
#define GDMA4_RX_MC_PKT_CNT_H        (GDMA4_COUNT_BASE + 0x1E4)
#define GDMA4_RX_ETH_E64_CNT_H       (GDMA4_COUNT_BASE + 0x1E8)
#define GDMA4_RX_ETH_L64_CNT_H       (GDMA4_COUNT_BASE + 0x1EC)
#define GDMA4_RX_ETH_L127_CNT_H      (GDMA4_COUNT_BASE + 0x1F0)
#define GDMA4_RX_ETH_L255_CNT_H      (GDMA4_COUNT_BASE + 0x1F4)
#define GDMA4_RX_ETH_L511_CNT_H      (GDMA4_COUNT_BASE + 0x1F8)
#define GDMA4_RX_ETH_L1023_CNT_H     (GDMA4_COUNT_BASE + 0x1FC)

#define GDMA_PAD_EN_BIT			(1 << 28)

/*******************************************************************
 *        FE ingress ratelimit Register         *
 ******************************************************************/
#define FE_RX_RL_PORT_EN	(FE_BASE + 0x2240)
#define FE_RX_RL_GLB_CFG	(FE_BASE + 0x2244)
#define FE_RX_RL_TIK_CFG	(FE_BASE + 0x2248)
#define FE_RX_RL_CFG		(FE_BASE + 0x224c)
#define FE_RX_RL_WDATA_L	(FE_BASE + 0x2250)
#define FE_RX_RL_WDATA_H	(FE_BASE + 0x2254)
#define FE_RX_RL_RDATA_L	(FE_BASE + 0x2258)
#define FE_RX_RL_RDATA_H	(FE_BASE + 0x225c)

#define FE_PORT_RATE_UPDATE		(1<<31)
#define FE_PORT_ID_SHIFT		(0)
#define FE_PORT_ID_MASK			(0x1f << FE_PORT_ID_SHIFT)
#define FE_RATE_ID_WDATA_SHIFT	(8)
#define FE_RATE_ID_WDATA_MASK	(0x7f << FE_RATE_ID_WDATA_SHIFT)
#define FE_RATE_ID_RDATA_SHIFT	(16)
#define FE_RATE_ID_RDATA_MASK	(0x7f << FE_RATE_ID_RDATA_SHIFT)
#define FE_SKIP_RATE_SHIFT		(28)
#define FE_SKIP_RATE_MASK		(0x7 << FE_SKIP_RATE_SHIFT)
#define FE_RATE_CFG_ID_SHIFT	(16)
#define FE_RATE_CFG_ID_MASK		(0x7f << FE_RATE_CFG_ID_SHIFT)

#define FeRxEnableRL(val)	IO_SBITS(FE_RX_RL_PORT_EN, (1 << val))
#define FeRxDisableRL(val)	IO_CBITS(FE_RX_RL_PORT_EN, (1 << val))

/*******************************************************************
 *        FE HWF QDMA SEL         *
 ******************************************************************/
 #define ETH_OFT 5/*for 7583 GDM3*/
 #define PCIE_OFT 6/*for 7583 GDM4 OQ0*/
 #define USB_OFT 7/*for 7583 GDM4 OQ1*/
 #define FE_HWF_QDMA_SEL (FE_BASE+0x20f4)

 #define FeGetHwfQdmaSelGdm3() IO_GMASK(FE_HWF_QDMA_SEL,1<<ETH_OFT,ETH_OFT)
 #define FeGetHwfQdmaSelGdm4() (IO_GMASK(FE_HWF_QDMA_SEL,1<<PCIE_OFT,PCIE_OFT)||IO_GMASK(FE_HWF_QDMA_SEL,1<<USB_OFT,USB_OFT))

/********************************
 * Giga Switch Module Registers *
 ********************************/

#define GSW_BASE     		0xBFB58000
#define GSW_ARL_BASE     	(GSW_BASE + 0x0000)
#define GSW_BMU_BASE     	(GSW_BASE + 0x1000)
#define GSW_PORT_BASE     	(GSW_BASE + 0x2000)
#define GSW_MAC_BASE     	(GSW_BASE + 0x3000)
#define GSW_MIB_BASE     	(GSW_BASE + 0x4000)
#define GSW_CFG_BASE     	(GSW_BASE + 0x7000)

#define GSW_MFC     		(GSW_ARL_BASE + 0x10)

#define GSW_IMC     		(GSW_ARL_BASE + 0x1c)
#define IMC_IGMP_RPT_FW_SHIFT			(12)

#define GSW_PSC(n)     		(GSW_PORT_BASE + (n)*0x100 + 0x0C)

#define GSW_PMCR(n)     	(GSW_MAC_BASE + (n)*0x100)
#define GSW_PMSR(n)     	(GSW_MAC_BASE + (n)*0x100 + 0x08)
#define GSW_PINT_EN(n)     	(GSW_MAC_BASE + (n)*0x100 + 0x10)
#define GSW_SMACCR0     	(GSW_MAC_BASE + 0xe4)
#define GSW_SMACCR1     	(GSW_MAC_BASE + 0xe8)
#define GSW_CKGCR		(GSW_MAC_BASE + 0xf0)

#define GSW_TX_DROC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x00)
#define GSW_TX_CRC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x04)
#define GSW_TX_UNIC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x08)
#define GSW_TX_MULC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x0c)
#define GSW_TX_BROC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x10)
#define GSW_TX_COLC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x14)
#define GSW_TX_SCOLC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x18)
#define GSW_TX_MCOLC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x1c)
#define GSW_TX_DEFC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x20)
#define GSW_TX_LCOLC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x24)
#define GSW_TX_ECOLC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x28)
#define GSW_TX_PAUC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x2c)
#define GSW_TX_OCL(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x48)
#define GSW_TX_OCH(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x4c)

#define GSW_RX_DROC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x60)
#define GSW_RX_FILC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x64)
#define GSW_RX_UNIC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x68)
#define GSW_RX_MULC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x6c)
#define GSW_RX_BROC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x70)
#define GSW_RX_ALIGE(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x74)
#define GSW_RX_CRC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x78)
#define GSW_RX_RUNT(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x7c)
#define GSW_RX_FRGE(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x80)
#define GSW_RX_LONG(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x84)
#define GSW_RX_JABE(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x88)
#define GSW_RX_PAUC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0x8c)
#define GSW_RX_OCL(n)    		(GSW_MIB_BASE + (n)*0x100 + 0xa8)
#define GSW_RX_OCH(n)    		(GSW_MIB_BASE + (n)*0x100 + 0xac)
#define GSW_RX_INGC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0xb4)
#define GSW_RX_ARLC(n)    		(GSW_MIB_BASE + (n)*0x100 + 0xb8)

#define EXT_GSW_TX_DROC(n)    		(0x4000 + (n)*0x100)
#define EXT_GSW_TX_CRC(n)    		(0x4004 + (n)*0x100)
#define EXT_GSW_TX_UNIC(n)    		(0x4008 + (n)*0x100)
#define EXT_GSW_TX_MULC(n)    		(0x400c + (n)*0x100)
#define EXT_GSW_TX_BROC(n)    		(0x4010 + (n)*0x100)
#define EXT_GSW_TX_COLC(n)    		(0x4014 + (n)*0x100)
#define EXT_GSW_TX_SCOLC(n)    		(0x4018 + (n)*0x100)
#define EXT_GSW_TX_MCOLC(n)    		(0x401c + (n)*0x100)
#define EXT_GSW_TX_DEFC(n)    		(0x4020 + (n)*0x100)
#define EXT_GSW_TX_LCOLC(n)    		(0x4024 + (n)*0x100)
#define EXT_GSW_TX_ECOLC(n)    		(0x4028 + (n)*0x100)
#define EXT_GSW_TX_PAUC(n)    		(0x402c + (n)*0x100)
#define EXT_GSW_TX_OCL(n)    		(0x4048 + (n)*0x100)
#define EXT_GSW_TX_OCH(n)    		(0x404c + (n)*0x100)

#define EXT_GSW_RX_DROC(n)    		(0x4060 + (n)*0x100)
#define EXT_GSW_RX_FILC(n)    		(0x4064 + (n)*0x100)
#define EXT_GSW_RX_UNIC(n)    		(0x4068 + (n)*0x100)
#define EXT_GSW_RX_MULC(n)    		(0x406c + (n)*0x100)
#define EXT_GSW_RX_BROC(n)    		(0x4070 + (n)*0x100)
#define EXT_GSW_RX_ALIGE(n)    		(0x4074 + (n)*0x100)
#define EXT_GSW_RX_CRC(n)    		(0x4078 + (n)*0x100)
#define EXT_GSW_RX_RUNT(n)    		(0x407c + (n)*0x100)
#define EXT_GSW_RX_FRGE(n)    		(0x4080 + (n)*0x100)
#define EXT_GSW_RX_LONG(n)    		(0x4084 + (n)*0x100)
#define EXT_GSW_RX_JABE(n)    		(0x4088 + (n)*0x100)
#define EXT_GSW_RX_PAUC(n)    		(0x408c + (n)*0x100)
#define EXT_GSW_RX_OCL(n)    		(0x40a8 + (n)*0x100)
#define EXT_GSW_RX_OCH(n)    		(0x40ac + (n)*0x100)
#define EXT_GSW_RX_INGC(n)    		(0x40b4 + (n)*0x100)
#define EXT_GSW_RX_ARLC(n)    		(0x40b8 + (n)*0x100)

#define GSW_PVC(n)     	(0x2010 + (n)*0x100)
#define DEFAULT_TPID  	(0x8100)

#define GSW_CFG_PPSC     	(GSW_CFG_BASE + 0x18)
#define GSW_CFG_PIAC     	(GSW_CFG_BASE + 0x1c)
#define GSW_CFG_GPC     	(GSW_CFG_BASE + 0x14)


#define GSW_VLAN_REG		(GSW_BASE+0x94)
#define GSW_ATA1_REG		(GSW_BASE+0x74)
#define GSW_ATC_REG			(GSW_BASE+0x80)
/***************************************
 * Ethernet Module Register Definition *
 ***************************************/

/* RSTCTRL2 */
#define QDMA1_RST    				(1<<1)
#define QDMA2_RST    				(1<<2)
#define FE_RST    					(1<<21)
#define ESW_RST    					(1<<23)


/* FE_VLAN_ID */
#define VLAN_ID1_SHIFT   				(16)
#define VLAN_IDl       					(0xfff<<VLAN_ID1_SHIFT)
#define VLAN_ID0_SHIFT   				(0)
#define VLAN_ID0       					(0xfff<<VLAN_ID0_SHIFT)


/* GDMA1_FWD_CFG or GDMA2_FWD_CFG */
#define GDM_JMB_LEN_SHIFT				(28)
#define GDM_JMB_LEN						(0xf<<GDM_JMB_LEN_SHIFT)
#define GDM_20US_TICK_SLT				(1<<25)

#define GDM_INSV_EN						(1<<26)
#define GDM_UNTAG_EN					(1<<25)
#define GDM_STAG_EN						(1<<24)
#define GDM_ICS_EN						(1<<22)
#define GDM_TCS_EN						(1<<21)
#define GDM_UCS_EN						(1<<20)
#define GDM_DROP_256B					(1<<19)
#define GDM_DISPAD						(1<<18)
#define GDM_DISCRC						(1<<17)
#define GDM_STRPCRC						(1<<16)
#define GDM_UFRC_P_SHIFT				(12)
#define GDM_UFRC_P						(0xf<<GDM_UFRC_P_SHIFT)
#define GDM_BFRC_P_SHIFT				(8)
#define GDM_BFRC_P						(0xf<<GDM_BFRC_P_SHIFT)
#define GDM_MFRC_P_SHIFT				(4)
#define GDM_MFRC_P						(0xf<<GDM_MFRC_P_SHIFT)
#define GDM_OFRC_P_SHIFT				(0)
#define GDM_OFRC_P						(0xf<<GDM_OFRC_P_SHIFT)

#define GDM2_UNDERRUN_RETRY				(1<<27)
#define GDM2_DROP_256B					(1<<26)
#define GDM2_DROP_LONG					(1<<25)
#define GDM2_DROP_RUNT					(1<<24)
#define GDM2_DROP_CRC_ERR				(1<<23)
#define GDM2_JMB_EN						(1<<19)


/* define GDMA port */
#define GDM_P_PDMA						(0x0)
#define GDM_P_GDMA1						(0x1)
#define GDM_P_GDMA2						(0x2)
#define GDM_P_PPE						(0x4)
#define GDM_P_QDMA						(0x5)
#define GDM_P_DISCARD					(0x7)
#define GDM_P_CPU						GDM_P_PDMA


/* GDMA1_SCH_CFG or GDMA2_SCH_CFG */
#define GDM_SCH_MOD_SHIFT				(24)
#define GDM_SCH_MOD						(0x3<<GDM_SCH_MOD_SHIFT)
#define GDM_WT_Q3_SHIFT					(12)
#define GDM_WT_Q3						(0x7<<GDM_WT_Q3_SHIFT)
#define GDM_WT_Q2_SHIFT					(8)
#define GDM_WT_Q2						(0x7<<GDM_WT_Q2_SHIFT)
#define GDM_WT_Q1_SHIFT					(4)
#define GDM_WT_Q1						(0x7<<GDM_WT_Q1_SHIFT)
#define GDM_WT_Q0_SHIFT					(0)
#define GDM_WT_Q0						(0x7<<GDM_WT_Q0_SHIFT)

#define GDM_SCH_MOD_WRR					(0)
#define GDM_SCH_MOD_SP					(1)

#define GDM_WT(n)						((n>=8) ? 7 : ((n)-1)&0x7)

/* CDMA_CSG_CFG */
#define INS_VLAN_SHIFT					(16)
#define INS_VLAN						(0xffff<<INS_VLAN_SHIFT)
#define CDM_STAG_EN							(1<<0)

/* GSW_MFC */
#define MFC_BC_FFP_SHIFT				(24)
#define MFC_BC_FFP						(0xff<<MFC_BC_FFP_SHIFT)
#define MFC_UNM_FFP_SHIFT				(16)
#define MFC_UNM_FFP						(0xff<<MFC_UNM_FFP_SHIFT)
#define MFC_UNU_FFP_SHIFT				(8)
#define MFC_UNU_FFP						(0xff<<MFC_UNU_FFP_SHIFT)
#define MFC_CPU_EN						(1<<7)
#define MFC_CPU_PORT_SHIFT				(4)
#define MFC_CPU_PORT					(0x7<<MFC_CPU_PORT_SHIFT)
#define MFC_MIRROR_EN					(1<<3)
#define MFC_MIRROR_PORT_SHIFT			(0)
#define MFC_MIRROT_PORT					(0x7<<MFC_MIRROR_PORT_SHIFT)

/* GSW_PMCR */
#define IPG_CFG_PN_SHIFT				(18)
#define IPG_CFG_PN						(0x3<<IPG_CFG_PN_SHIFT)
#define EXT_PHY_PN						(1<<17)
#define MAC_MODE_PN						(1<<16)
#define FORCE_MODE_PN					(1<<15)
#define MAC_TX_EN_PN					(1<<14)
#define MAC_RX_EN_PN					(1<<13)
#define RGMII_MODE_PN					(1<<12)
#define BKOFF_EN_PN						(1<<9)
#define BACKPR_EN_PN					(1<<8)
#define ENABLE_EEE1G_PN					(1<<7)
#define ENABLE_EEE100_PN				(1<<6)
#define ENABLE_RX_FC_PN					(1<<5)
#define ENABLE_TX_FC_PN					(1<<4)
#define FORCE_SPD_PN_SHIFT				(2)
#define FORCE_SPD_PN					(0x3<<FORCE_SPD_PN_SHIFT)
#define FORCE_DPX_PN					(1<<1)
#define FORCE_LNK_PN					(1<<0)

#define IPG_CFG_NORMAL					(0)
#define IPG_CFG_SHORT					(1)
#define IPG_CFG_64BITS                                  (0x2)

#define PN_SPEED_10M					(0)
#define PN_SPEED_100M					(1)
#define PN_SPEED_1000M					(2)

/* GSW_PMSR */
#define EEE1G_STS						(1<<7)
#define EEE100_STS						(1<<6)
#define RX_FC_STS						(1<<5)
#define TX_FC_STS						(1<<4)
#define MAC_SPD_STS_SHIFT				(2)
#define MAC_SPD_STS						(0x3<<MAC_SPD_STS_SHIFT)
#define MAC_DPX_STS						(1<<1)
#define MAC_LINK_STS					(1<<0)


/* GSW_CFG_PPSC */
#define PHY_AP_EN_SHIFT					(1<<24)
#define PHY_AP_EN						(0x7f<<PHY_END_ADDR_SHIFT)

#define PHY_EEE_EN_SHIFT				(1<<16)
#define PHY_EEE_EN						(0x7f<<PHY_END_ADDR_SHIFT)

#define PHY_PRE_EN						(1<<15)
#define PHY_END_ADDR_SHIFT				(8)
#define PHY_END_ADDR					(0x1f<<PHY_END_ADDR_SHIFT)
#define PHY_MDC_CFG_SHIFT				(1<<6)
#define PHY_MDC_CFG						(0x3<<PHY_MDC_CFG_SHIFT)
#define PHY_ST_ADDR_SHIFT				(0)
#define PHY_ST_ADDR						(0x1f<<PHY_ST_ADDR_SHIFT)

/* GSW_CFG_PIAC */
#define PHY_ACS_ST						(1<<31)
#define MDIO_REG_ADDR_SHIFT				(25)
#define MDIO_REG_ADDR					(0x1f<<MDIO_REG_ADDR_SHIFT)
#define MDIO_PHY_ADDR_SHIFT				(20)
#define MDIO_PHY_ADDR					(0x1f<<MDIO_PHY_ADDR_SHIFT)
#define MDIO_CMD_SHIFT					(18)
#define MDIO_CMD						(0x3<<MDIO_CMD_SHIFT)
#define MDIO_ST_SHIFT					(16)
#define MDIO_ST							(0x3<<MDIO_ST_SHIFT)
#define MDIO_RW_DATA_SHIFT				(0)
#define MDIO_RW_DATA					(0xffff<<MDIO_RW_DATA_SHIFT)

#define PHY_ACS_ST_START				(1)
#define MDIO_CMD_WRITE					(1)
#define MDIO_CMD_READ					(2)
#define MDIO_ST_START					(1)

#define MDIO_CL45_CMD_ADDR				(0)
#define MDIO_CL45_CMD_WRITE				(1)
#define MDIO_CL45_CMD_READ				(3)
#define MDIO_CL45_CMD_POSTREAD_INCADDR			(2)
#define MDIO_CL45_ST_START				(0)

#define NORMAL_READ					(1<<0)
#define POST_READ					(1<<1)

/* GSW_CFG_GPC */
#define RX_CLK_MODE						(1<<2)

/* FPORT DEFINE */
#define DPORT_PDMA				0
#define DPORT_GDMA1				1
#define DPORT_GDMA2				2 /* means wan , like SAR/PTM/xPON */
#define DPORT_GDMA3				3

#define DPORT_PPE				4
#define DPORT_QDMA				5/* send to CPU via QDMA */
#define DPORT_QDMA_HW			6/* send to CPU via QDMA HW */
#define DPORT_DISCARD			7
#define DPORT_CPU				DPORT_PDMA

/* PKT_INFO define */
#define IPV6_H						(1<<5)
#define IPV4_H						(1<<4)
#define IPV4_H_INV					(1<<3)
#define TU_H						(1<<1)
#define TU_H_C_INV					(1<<0)

/***************************************
 * MC VLAN 1:N Register Definition *
 ***************************************/
#define MC_VLAN_EN     			(FE_BASE + 0x2100)
#define MC_VLAN_CFG     		(FE_BASE + 0x2104)
#define MC_VLAN_DATA     		(FE_BASE + 0x2108)

/* FE_CSR_MC_VLAN_EN */
#define MC_VLAN_ENABLE								(1)
#define MC_VLAN_EN_SHIFT							(0)
#define MC_VLAN_EN_MASK								(0x1<<MC_VLAN_EN_SHIFT)

/* FE_CSR_MC_VLAN_CFG */
#define MC_VLAN_CFG_RWCMD_DONE						(1<<31)
#define MC_VLAN_CFG_DST_TABLE_ID_SHIFT				(16)
#define MC_VLAN_CFG_DST_TABLE_ID_MASK				(0x3F)
#define MC_VLAN_CFG_DST_PORT_SHIFT					(8)
#define MC_VLAN_CFG_DST_PORT_MASK					(0xF)
#define MC_VLAN_CFG_TABLE_SEL						(0x1<<4) /* 0, vlan table; 1, lut table */
#define MC_VLAN_CFG_RWCMD							(0x1<<0) /* 0, read; 1, write */

/* FE_CSR_MC_VLAN_DATA */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define MC_VLAN_DATA_ENABLE_SHIFT					(16)
#else
#define MC_VLAN_DATA_ENABLE_SHIFT					(15)
#endif
#define MC_VLAN_DATA_ENABLE_MASK					(0x1)
#define MC_VLAN_DATA_PPE_ENTRY_SHIFT				(0)
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define MC_VLAN_DATA_PPE_ENTRY_MASK					(0xFFFF)
#else
#define MC_VLAN_DATA_PPE_ENTRY_MASK					(0x7FFF)
#endif

#define MC_VLAN_DATA_OP_SHIFT						(18)
#define MC_VLAN_DATA_OP_MASK						(0x3)
#define MC_VLAN_DATA_VPM_SHIFT						(16)
#define MC_VLAN_DATA_VPM_MASK						(0x3)
#define MC_VLAN_DATA_VLAN_SHIFT						(0)
#define MC_VLAN_DATA_VLAN_MASK						(0xFFFF)



/***************************************
 * TUnnel table regisiter *
 ***************************************/

#define IP_ASSEM_FRAG_FP_CFG            (FE_BASE + 0x2010)
#define IP_FRAG_PORT_SHIFT              (5)
#define IP_FRAG_NBQ_SHIFT               (0)
#define IP_FRAG_PORT_MASK               (0xF<<IP_FRAG_PORT_SHIFT)
#define IP_FRAG_NBQ_MASK                (0x1F<<IP_FRAG_NBQ_SHIFT)
#define IP_ASSEM_PORT_SHIFT             (21)
#define IP_ASSEM_NBQ_SHIFT              (16)
#define IP_ASSEM_PORT_MASK              (0xF<<IP_ASSEM_PORT_SHIFT)
#define IP_ASSEM_NBQ_MASK               (0x1F<<IP_ASSEM_NBQ_SHIFT)

#define FE_GDM4_TMBI_FRAG     		(FE_BASE + 0x2028)
#define FE_GDM4_RMBI_FRAG     		(FE_BASE + 0x202c)

#define GDM4_SGMII1_WEIGHT_SHIFT       (26)
#define GDM4_SGMII1_FRAG_SIZE_SHIFT    (16)
#define GDM4_SGMII1_FRAG_SIZE_MASK     (0x3ff0000)
#define GDM4_SGMII0_WEIGHT_SHIFT       (10)
#define GDM4_SGMII0_FRAG_SIZE_SHIFT    (0)
#define GDM4_SGMII0_FRAG_SIZE_MASK     (0x3ff)


#define WAN_IP6_CFG     		(FE_BASE + 0x2040)
#define WAN_IP6_WDATA     		(FE_BASE + 0x2044)
#define WAN_IP6_RDATA     		(FE_BASE + 0x2048)

#define WAN_IP6_CFG_IP_ID_SHIFT				(8)
#define WAN_IP6_CFG_IP_ID_MASK				(0x7)
#define WAN_IP6_CFG_IP_OFST_SHIFT			(4)
#define WAN_IP6_CFG_IP_OFST_MASK			(0x3)
#define WAN_IP6_CFG_WR_REQ					(0x1<<0) /* 0, sw access invalid; 1,  sw access valid */



#define TUN_TBL_CFG     		(FE_BASE + 0x207c)
#define TUN_TBL_DATA0     		(FE_BASE + 0x2080)
#define TUN_TBL_DATA1     		(FE_BASE + 0x2084)
#define TUN_TBL_DATA2     		(FE_BASE + 0x2088)
#define TUN_TBL_DATA3     		(FE_BASE + 0x208c)

#define TUN_TBL_CFG_RWCMD_DONE					(1<<31)
#define TUN_TBL_CFG_OFFSET_ID_SHIFT				(16)
#define TUN_TBL_CFG_OFFSET_ID_MASK				(0x7)
#define TUN_TBL_CFG_TABLE_ID_SHIFT				(8)
#define TUN_TBL_CFG_TABLE_ID_MASK				(0x3f)
#define TUN_TBL_CFG_RWCMD_SHIFT					(4) /* 0, read; 1, write */
#define TUN_TBL_CFG_REQ							(0x1<<0) /* 0, sw access invalid; 1,  sw access valid */

#define FE_TUN_CMD_READ					(0)
#define FE_TUN_CMD_WRITE				(1)

#define FE_TUN_SEL_BANK0				(0)		/*layer 2 header*/
#define FE_TUN_SEL_BANK1				(1)		/*ipv4 header*/
#define FE_TUN_SEL_BANK2				(2)		/*ipv6 header*/
#define FE_TUN_SEL_BANK3				(3)		/*l2tp gre vxlan udp header*/
#define FE_TUN_SEL_CFG					(4)


/************************************************************************
*               M A C R O S
*************************************************************************
*/
#ifdef TCSUPPORT_CPU_ARMV8
#define IO_GREG(reg)                    get_frame_engine_data(reg)
#define IO_SREG(reg, value)             set_frame_engine_data(reg, value)
#else
#define IO_GREG(reg)                    regRead32(reg)
#define IO_SREG(reg, value)             regWrite32(reg, value)
#endif

#define L2_SPACE_OFFSET                 (4)
#define L2_SPACE_MASK                   (0xF<<L2_SPACE_OFFSET)
#define PSE_PAGE_SIZE                   (1<<3)
#define PKT_INFO_SPACE_OFFSET           (0)
#define PKT_INFO_SPACE_MASK             (0x7<<PKT_INFO_SPACE_OFFSET)
#define FE_CORE_RESET_OFFSET            (0)
#define FE_GDM3_MBI_ARB_RST_OFFSET      (1<<2)
#define FE_GDM4_MBI_ARB_RST_OFFSET      (1<<3)
#define FE_GDM3_MBI_ARB_RX0_RST_OFFSET  (1<<4)
#define FE_GDM3_MBI_ARB_RX1_RST_OFFSET  (1<<5)
#define FE_GDM3_MBI_ARB_TX0_RST_OFFSET  (1<<6)
#define FE_GDM3_MBI_ARB_TX1_RST_OFFSET  (1<<7)
#define FE_GDM4_MBI_ARB_RX0_RST_OFFSET  (1<<8)
#define FE_GDM4_MBI_ARB_RX1_RST_OFFSET  (1<<9)
#define FE_GDM4_MBI_ARB_TX0_RST_OFFSET  (1<<10)
#define FE_GDM4_MBI_ARB_TX1_RST_OFFSET  (1<<11)
#define FE_GDM3_MBI_ARB_RST_MASK        (0xF0)
#define FE_GDM4_MBI_ARB_RST_MASK        (0xF00)


#define RATE_BYTE_OFFSET				(8)
#define RATE_BYTE_MASK					(0x7F<<RATE_BYTE_OFFSET)


#define INT_GDM2_RX_2_GEMPORT_ISSUE     (1<<23)
#define INT_GDM2_RX_FIFO_OVERFLOW       (1<<22)
#define INT_GDM2_RX_MBI_BY_AUTOAGING    (1<<21)
#define INT_GDM2_RX_MBI_BY_CSR          (1<<20)
#define INT_GDM2_TX_MBI_BY_AUTOAGING    (1<<18)
#define INT_GDM2_TX_MBI_BY_CSR          (1<<17)
#define INT_GDM2_TX_MBI_BY_MAC          (1<<16)

#define INT_STATUS_PSE_FC_DROP          (1<<9)
#define INT_STATUS_PSE_FQ_EMPTY         (1<<8)
#define INT_STATUS_RX_MBI               (0x00300000)
#define INT_STATUS_TX_MBI               (0x00070000)
#define INT_STATUS_TSO                  (0x0000F000)
#define INT_STATUS_PSE_FC               (0x0000003F)
#define INT_STATUS_PSE_EXCEPTION        (0x10000000)

#define FeSetIntStatus(val)    IO_SREG(FE_INT_STATUS, val)
#define FeGetIntStatus()       IO_GREG(FE_INT_STATUS)
#define FeSetIntMask(val)      IO_SREG(FE_INT_ENABLE, val)
#define FeGetIntMask()         IO_GREG(FE_INT_ENABLE)


#define MBI_RX_BUSY_OFFSET          27
#define MBI_RX_AGE_SEL_OFFSET       25
#define MBI_RX_AGE_SEL_MASK         (0x3<<MBI_RX_AGE_SEL_OFFSET)
#define MBI_RX_TERMINATE_OFFSET     24
#define MBI_TX_BUSY_OFFSET          19
#define MBI_TX_AGE_SEL_OFFSET       17
#define MBI_TX_AGE_SEL_MASK         (0x3<<MBI_TX_AGE_SEL_OFFSET)
#define MBI_TX_TERMINATE_OFFSET     16
#define GDMA_CHN_RLS_CHN_OFFSET     4
#define GDMA_CHN_RLS_STAT_OFFSET    1
#define GDMA_CHN_RLS_EN_OFFSET      0 
#define GDMA_CHN_RLS_TIMEOUT        (10)

/* FE_CSR_IFC_CFG */
#define FE_IFC_EN					(1<<0)
#define FE_IFC_MASK					(1)
#define FE_IFC_SHIFT				(0)
#define IFCDisable					IO_CBITS(FE_CSR_IFC_CFG, FE_IFC_EN)

#define GDM_RC_CFG_PARA_MISC		(0x0)
#define GDM_RC_CFG_PARA_TOKEN_RATE	(0x1)
#define GDM_RC_CFG_PARA_BUCK_SHIFT	(0x2)
#define GDM_RC_CFG_PARA_BUCK_CNT	(0x3)
#define GDM_RC_CFG_EN				(1<<31)
#define GDM_RC_CFG_PARA_TYPE_SHIFT	(28)
#define GDM_RC_CFG_PARA_TYPE_MASK	(0x3<<GDM_RC_CFG_PARA_TYPE_SHIFT)
#define GDM_RC_CFG_ID_SHIFT			(16)
#define GDM_RC_CFG_ID_MASK			(0x7<<GDM_RC_CFG_ID_SHIFT)
#define GDM_RC_CFG_PARA_RWCMD		(1<<0)

#define GDM_RC_CFG_METER_DISABLE	(0x0)
#define GDM_RC_CFG_METER_ENABLE		(0x1)
#define GDM_RC_CFG_BYTE_MODE		(0x0)
#define GDM_RC_CFG_PKT_MODE			(0x1)
#define GDM_RC_CFG_FAST_TICK		(0x0)
#define GDM_RC_CFG_SLOW_TICK		(0x1)

#define GDM_RC_CFG_PARA_METER_EN	(1<<2)
#define GDM_RC_CFG_PARA_PPS_MODE	(1<<1)
#define GDM_RC_CFG_PARA_TICK_SEL	(1<<0)

#define RC_TOKEN_RATE_INTEGER_SHIFT		(6)
#define RC_TOKEN_RATE_INTEGER_MASK		(0x3FFFF<<RC_TOKEN_RATE_INTEGER_SHIFT)
#define RC_TOKEN_RATE_FRACTION_MASK		(0x3F)

#define RC_PKT_MODE_BUCKET_SHIFT		(0)
#define RC_BYTE_MODE_BUCKET_SHIFT		(10)

/* FE_CSR_MC_VLAN_EN */
#define feIsMcVlanEnable				IO_GMASK(MC_VLAN_EN, MC_VLAN_EN_MASK, MC_VLAN_EN_SHIFT)
#define feEnableMcVlan					IO_SBITS(MC_VLAN_EN, (MC_VLAN_ENABLE<<MC_VLAN_EN_SHIFT))
#define feDisableMcVlan					IO_CBITS(MC_VLAN_EN, (MC_VLAN_ENABLE<<MC_VLAN_EN_SHIFT))

#define PSE_PORT_NUM_MAX            (16)
#define PSE_PORT_QUEUE_MAX          (32)
/* -----------------PSE Port Number Info ----------------- */
#if defined(TCSUPPORT_CPU_AN7583)
#define PSE_PORT_NUM                (10) /* Port-0 ~ Port-9 for normal use */
#define PSE_PORT0_QUEUE_NUM         (6)
#define PSE_PORT1_QUEUE_NUM         (6)
#define PSE_PORT2_QUEUE_NUM         (32)
#define PSE_PORT3_QUEUE_NUM         (2)/*GDM3*/
#define PSE_PORT4_QUEUE_NUM         (2)
#define PSE_PORT5_QUEUE_NUM         (6)
#define PSE_PORT6_QUEUE_NUM         (6)/*CDM3*/
#define PSE_PORT7_QUEUE_NUM         (12)/*CDM4*/
#define PSE_PORT8_QUEUE_NUM         (0)
#define PSE_PORT9_QUEUE_NUM         (2)
#define PSE_PORT10_QUEUE_NUM        (0)
#define PSE_PORT11_QUEUE_NUM        (0)
#define PSE_PORT12_QUEUE_NUM        (0)
#define PSE_PORT13_QUEUE_NUM        (0)
#define PSE_PORT14_QUEUE_NUM        (0)
#define PSE_PORT15_QUEUE_NUM        (0)
#elif defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define PSE_PORT_NUM                (11) /* Port-0 ~ Port-9 for normal use , Port-15 for free */
#define PSE_PORT0_QUEUE_NUM         (6)
#define PSE_PORT1_QUEUE_NUM         (6)
#define PSE_PORT2_QUEUE_NUM         (32)
#define PSE_PORT3_QUEUE_NUM         (6)
#define PSE_PORT4_QUEUE_NUM         (4)
#define PSE_PORT5_QUEUE_NUM         (6)
#if defined(TCSUPPORT_CPU_AN7552)
#define PSE_PORT6_QUEUE_NUM         (2)
#define PSE_PORT7_QUEUE_NUM         (12)
#else
#define PSE_PORT6_QUEUE_NUM         (8)
#define PSE_PORT7_QUEUE_NUM         (10)
#endif
#define PSE_PORT8_QUEUE_NUM         (4)
#define PSE_PORT9_QUEUE_NUM         (2)
#define PSE_PORT10_QUEUE_NUM        (2)
#define PSE_PORT11_QUEUE_NUM        (0)
#define PSE_PORT12_QUEUE_NUM        (0)
#define PSE_PORT13_QUEUE_NUM        (0)
#define PSE_PORT14_QUEUE_NUM        (0)
#define PSE_PORT15_QUEUE_NUM        (0)
#elif defined(TCSUPPORT_CPU_EN7523)
#define PSE_PORT_NUM                (10) /* Port-0 ~ Port-9 for normal use , Port-15 for free */
#define PSE_PORT0_QUEUE_NUM         (6)
#define PSE_PORT1_QUEUE_NUM         (6)
#define PSE_PORT2_QUEUE_NUM         (32)
#define PSE_PORT3_QUEUE_NUM         (7)
#define PSE_PORT4_QUEUE_NUM         (4)
#define PSE_PORT5_QUEUE_NUM         (6)
#define PSE_PORT6_QUEUE_NUM         (6)
#define PSE_PORT7_QUEUE_NUM         (4)
#define PSE_PORT8_QUEUE_NUM         (1)
#define PSE_PORT9_QUEUE_NUM         (1)
#define PSE_PORT10_QUEUE_NUM        (0)
#define PSE_PORT11_QUEUE_NUM        (0)
#define PSE_PORT12_QUEUE_NUM        (0)
#define PSE_PORT13_QUEUE_NUM        (0)
#define PSE_PORT14_QUEUE_NUM        (0)
#define PSE_PORT15_QUEUE_NUM        (1)
#else
#define PSE_PORT_NUM                (8) /* Port-0 ~ Port-5 for normal use , Port-7 for free */
#define PSE_PORT0_QUEUE_NUM         (3)
#define PSE_PORT1_QUEUE_NUM         (6)
#define PSE_PORT2_QUEUE_NUM         (32)
#define PSE_PORT3_QUEUE_NUM         (5)
#define PSE_PORT4_QUEUE_NUM         (4)
#define PSE_PORT5_QUEUE_NUM         (3)
#define PSE_PORT6_QUEUE_NUM         (0) /* not use */
#define PSE_PORT7_QUEUE_NUM         (1)
#define PSE_PORT8_QUEUE_NUM         (0)
#define PSE_PORT9_QUEUE_NUM         (0)
#define PSE_PORT10_QUEUE_NUM        (0)
#define PSE_PORT11_QUEUE_NUM        (0)
#define PSE_PORT12_QUEUE_NUM        (0)
#define PSE_PORT13_QUEUE_NUM        (0)
#define PSE_PORT14_QUEUE_NUM        (0)
#define PSE_PORT15_QUEUE_NUM        (0)
#endif

#define PSE_PORT_QUEUE_NUM_ARRAY          {\
										PSE_PORT0_QUEUE_NUM,PSE_PORT1_QUEUE_NUM , PSE_PORT2_QUEUE_NUM, PSE_PORT3_QUEUE_NUM ,\
										PSE_PORT4_QUEUE_NUM , PSE_PORT5_QUEUE_NUM , PSE_PORT6_QUEUE_NUM , PSE_PORT7_QUEUE_NUM ,\
										PSE_PORT8_QUEUE_NUM , PSE_PORT9_QUEUE_NUM , PSE_PORT10_QUEUE_NUM , PSE_PORT11_QUEUE_NUM, \
										PSE_PORT12_QUEUE_NUM , PSE_PORT13_QUEUE_NUM , PSE_PORT14_QUEUE_NUM , PSE_PORT15_QUEUE_NUM\
										}

#define XFI_MAC_PCIE0_RST (1<<16)
#define XFI_MAC_PCIE1_RST (1<<17)
#define XFI_MAC_USB_RST   (1<<18)
#define XFI_MAC_ETH_RST   (1<<7)

/*frame engine IPv6 extension header*/
#define FE_IPV6_EXT_MASK      	(0XFF)
#define IP6_EXT0_SHIFT			(0)
#define IP6_EXT1_SHIFT			(8)
#define IP6_EXT2_SHIFT			(16)
#define IP6_EXT2_SHIFT			(24)

/* FAQ Function add for AN7583 */
#define CDM1_FAQ_CFG				(CDMA1_BASE+0xB0)
#define CDM1_FAQTHR_CFG				(CDMA1_BASE+0xB4)
#define CDM1_FAQDBG_CFG				(CDMA1_BASE+0xB8)
#define CDM1_FAQ_ENQCNT				(CDMA1_BASE+0xBC)
#define CDM1_FAQ_DEQCNT				(CDMA1_BASE+0xC0)

#define CDM2_FAQ_CFG				(CDMA2_BASE+0xB0)
#define CDM2_FAQTHR_CFG				(CDMA2_BASE+0xB4)
#define CDM2_FAQDBG_CFG				(CDMA2_BASE+0xB8)
#define CDM2_FAQ_ENQCNT				(CDMA2_BASE+0xBC)
#define CDM2_FAQ_DEQCNT				(CDMA2_BASE+0xC0)

#define FAQ_DEQ_TMO_SHIFT			(8)
#define FAQ_DEQ_TMO_MASK			(0xFF<<FAQ_DEQ_TMO_SHIFT)
#define FAQ_FLOW_MODE_SHIFT			(4)
#define FAQ_FLOW_MODE_MASK			(0x3<<FAQ_FLOW_MODE_SHIFT)
#define FAQ_FLOW_SEL_SHIFT			(1)
#define FAQ_FLOW_SEL_MASK			(0x1<<FAQ_FLOW_SEL_SHIFT)
#define FAQ_EN_SHIFT				(0)
#define FAQDBG_CNTSEL_SHIFT			(16)
#define FAQDBG_CNTSEL_MASK			(0xFF<<FAQDBG_CNTSEL_SHIFT)

#define FAQ_DEQ_LTHR_SHIFT			(0)
#define FAQ_DEQ_LTHR_MASK			(0Xfff<<FAQ_DEQ_LTHR_SHIFT)

#define cdm1SetFaqEnable		(IO_SBITS(CDM1_FAQ_CFG,1<<FAQ_EN_SHIFT))
#define cdm1SetFaqDisable		(IO_CBITS(CDM1_FAQ_CFG,1<<FAQ_EN_SHIFT))
#define cdm2SetFaqEnable		(IO_SBITS(CDM2_FAQ_CFG,1<<FAQ_EN_SHIFT))
#define cdm2SetFaqDisable		(IO_CBITS(CDM2_FAQ_CFG,1<<FAQ_EN_SHIFT))

#define cdm1SetFlowSel(mode)	(IO_SMASK(CDM1_FAQ_CFG,FAQ_FLOW_SEL_MASK,FAQ_FLOW_SEL_SHIFT,mode))		
#define cdm2SetFlowSel(mode)	(IO_SMASK(CDM2_FAQ_CFG,FAQ_FLOW_SEL_MASK,FAQ_FLOW_SEL_SHIFT,mode))		

#define cdm1SetTimeout(num)		(IO_SMASK(CDM1_FAQ_CFG,FAQ_DEQ_TMO_MASK,FAQ_DEQ_TMO_SHIFT,num))	
#define cdm2SetTimeout(num)		(IO_SMASK(CDM2_FAQ_CFG,FAQ_DEQ_TMO_MASK,FAQ_DEQ_TMO_SHIFT,num))	

#define cdm1SetFlowMode(mode)	(IO_SMASK(CDM1_FAQ_CFG,FAQ_FLOW_MODE_MASK,FAQ_FLOW_MODE_SHIFT,mode))		
#define cdm2SetFlowMode(mode)	(IO_SMASK(CDM2_FAQ_CFG,FAQ_FLOW_MODE_MASK,FAQ_FLOW_MODE_SHIFT,mode))		

#define cdm1SetLthr(num)		(IO_SMASK(CDM1_FAQ_CFG,FAQ_DEQ_LTHR_MASK,FAQ_DEQ_LTHR_SHIFT,num))	
#define cdm2SetLthr(num)		(IO_SMASK(CDM2_FAQ_CFG,FAQ_DEQ_LTHR_MASK,FAQ_DEQ_LTHR_SHIFT,num))	

#define cdm1GetEnqCnt			(IO_GREG(CDM1_FAQ_ENQCNT))
#define cdm1GetDeqCnt			(IO_GREG(CDM1_FAQ_DEQCNT))

#define cdm2GetEnqCnt			(IO_GREG(CDM2_FAQ_ENQCNT))
#define cdm2GetDeqCnt			(IO_GREG(CDM2_FAQ_DEQCNT))

/*7583 add force to slow*/
#define FORCE_SLOW_CFG						(FE_BASE + 0x2008)
#define FORCE_SLOW_THLD						(FE_BASE + 0x200c)

#define FORCE_SLOW_EN						(1<<31)
#define FIX_DUTY							(1<<30)
#define REFER_OQ_BUF						(1<<29)

#define FAST_DUTY_SHIFT						24
#define FAST_DUTY_MASK						(0xf<<FAST_DUTY_SHIFT)
#define SLOW_DUTY_SHIFT						16
#define SLOW_DUTY_MASK						(0xff<<SLOW_DUTY_SHIFT)
#define DUTY_MASK							(0xfff<<SLOW_DUTY_SHIFT)
#define BUFFER_CHECK_GAP_SHIFT				0
#define BUFFER_CHECK_GAP_MASK				(0xffff<<BUFFER_CHECK_GAP_SHIFT)

#define SHARE_USED_HTHD_SHIFT				16
#define SHARE_USED_HTHD_MASK				(0xffff<<SHARE_USED_HTHD_SHIFT)
#define SHARE_USED_LTHD_SHIFT				0
#define SHARE_USED_LTHD_MASK				(0xffff<<SHARE_USED_LTHD_SHIFT)

#define FeForceSlowEnable()					IO_SBITS(FORCE_SLOW_CFG, FORCE_SLOW_EN)
#define FeForceSlowDisable()				IO_CBITS(FORCE_SLOW_CFG, FORCE_SLOW_EN)
#define FeForceSlowFixDutyEnable()			IO_SBITS(FORCE_SLOW_CFG, FIX_DUTY)
#define FeForceSlowFixDutyDisable()			IO_CBITS(FORCE_SLOW_CFG, FIX_DUTY)
#define FeReferOqBufEnable()				IO_SBITS(FORCE_SLOW_CFG, REFER_OQ_BUF)
#define FeReferOqBufDisable()				IO_CBITS(FORCE_SLOW_CFG, REFER_OQ_BUF)

#define FeSetFastDuty(val)					IO_SMASK(FORCE_SLOW_CFG, FAST_DUTY_MASK, FAST_DUTY_SHIFT, val)
#define FeSetSlowDuty(val)					IO_SMASK(FORCE_SLOW_CFG, SLOW_DUTY_MASK, SLOW_DUTY_SHIFT, val)
#define FeSetForceSlowDuty(val)				IO_SMASK(FORCE_SLOW_CFG, DUTY_MASK, SLOW_DUTY_SHIFT, val)
#define FeSetBufferCheckGap(val)			IO_SMASK(FORCE_SLOW_CFG, BUFFER_CHECK_GAP_MASK, BUFFER_CHECK_GAP_SHIFT, val)

#define FeSetShareUsedHthd(val)				IO_SMASK(FORCE_SLOW_THLD, SHARE_USED_HTHD_MASK, SHARE_USED_HTHD_SHIFT, val)
#define FeSetShareUsedLthd(val)				IO_SMASK(FORCE_SLOW_THLD, SHARE_USED_LTHD_MASK, SHARE_USED_LTHD_SHIFT, val)

#if defined(TCSUPPORT_CPU_AN7583)||defined(TCSUPPORT_CPU_AN7552)
#define WOE_PORT 7
#define WOE_OQ1 10
#define WOE_OQ2 11
#else
#define WOE_PORT 3
#define WOE_OQ1 0
#define WOE_OQ2 1
#endif

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/



// call hook function by qdma api, delete these define later.   by geoffrey
#define QDMA1_CHN_EN_BASE     (0xbfb540a0)
#define QDMA2_CHN_EN_BASE     (0xbfb560a0)
#define QDMA1_CHN_VLD_BASE    (0xbfb55280)
#define QDMA2_CHN_VLD_BASE    (0xbfb57280)

#define RATE_OR_FULL_DROP   0x0
#define RATE_OR_THRL_DROP   0x55555555
#define RATE_AND_FULL_DROP  0xaaaaaaaa
#define RATE_AND_THRL_DROP  0xffffffff

//#define cdm1SetRxqRedCfg(ringIdx, mode) IO_SMASK(CMD1_RXQ_RED_CFG, 3<<(ringIdx<<1), (ringIdx<<1), mode)
//#define cdm2SetRxqRedCfg(ringIdx, mode) IO_SMASK(CMD2_RXQ_RED_CFG, 3<<(ringIdx<<1), (ringIdx<<1), mode)
#define cdm1SetRxqRedCfg(mode)          IO_SREG(CMD1_RXQ_RED_CFG, mode)
#define cdm2SetRxqRedCfg(mode)          IO_SREG(CMD2_RXQ_RED_CFG, mode)

//#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
static inline unsigned int pse_get_iq_rsv(unsigned int port, unsigned int queue)
{
	unsigned int tmp_val;

	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|(queue<<PSE_CFG_QUEUE_ID_SHIFT));
	tmp_val = IO_GMASK(PSE_QUEUE_CFG_VAL, PSE_CFG_IQ_RSV_MASK, PSE_CFG_IQ_RSV_SHIFT);

	return tmp_val;
}
	
static inline unsigned int pse_get_oq_rsv(unsigned int port, unsigned int queue)
{
	unsigned int tmp_val; 

	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|(queue<<PSE_CFG_QUEUE_ID_SHIFT));
	tmp_val = IO_GMASK(PSE_QUEUE_CFG_VAL, PSE_CFG_OQ_RSV_MASK, PSE_CFG_OQ_RSV_SHIFT);

	return tmp_val;
}

static inline unsigned int pse_get_oq_lthd(unsigned int port, unsigned int queue)
{
	unsigned int tmp_val; 

	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|(queue<<PSE_CFG_QUEUE_ID_SHIFT));
	tmp_val = IO_GMASK(PSE_QUEUE_CFG_VAL, PSE_CFG_OQ_LTHD_MASK, PSE_CFG_OQ_LTHD_SHIFT);

	return tmp_val;
}

static inline unsigned int pse_get_oq_en_by_port(unsigned int port)
{
	unsigned int tmp_val; 

	IO_SREG(PSE_QUEUE_CFG_WR, port<<PSE_CFG_PORT_ID_SHIFT);
	tmp_val = IO_GREG(PSE_OQUEUE_EN);

	return tmp_val;
}

static inline unsigned int pse_get_oq_en_by_queue(unsigned int port, unsigned int queue)
{
	unsigned int tmp_val; 

	IO_SREG(PSE_QUEUE_CFG_WR, port<<PSE_CFG_PORT_ID_SHIFT);
	tmp_val = IO_GMASK(PSE_OQUEUE_EN, 0x1, queue);

	return tmp_val;
}

static inline unsigned int pse_set_iq_rsv(unsigned int port, unsigned int queue, unsigned int val)
{
	IO_SMASK(PSE_QUEUE_CFG_VAL, PSE_CFG_IQ_RSV_MASK, PSE_CFG_IQ_RSV_SHIFT, val);
	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|(queue<<PSE_CFG_QUEUE_ID_SHIFT)|PSE_CFG_WR_EN|PSE_CFG_IQRSV_SEL);

	return 0;
}
	
static inline unsigned int pse_set_oq_rsv(unsigned int port, unsigned int queue, unsigned int val)
{
	IO_SMASK(PSE_QUEUE_CFG_VAL, PSE_CFG_OQ_RSV_MASK, PSE_CFG_OQ_RSV_SHIFT, val);
	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|(queue<<PSE_CFG_QUEUE_ID_SHIFT)|PSE_CFG_WR_EN|PSE_CFG_OQRSV_SEL);

	return 0;
}

static inline unsigned int pse_set_oq_lthd(unsigned int port, unsigned int queue, unsigned int val)
{
	IO_SMASK(PSE_QUEUE_CFG_VAL, PSE_CFG_OQ_LTHD_MASK, PSE_CFG_OQ_LTHD_SHIFT, val);
	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|(queue<<PSE_CFG_QUEUE_ID_SHIFT)|PSE_CFG_WR_EN|PSE_CFG_LTHD_SEL);

	return 0;
}

static inline unsigned int pse_set_oq_en_by_port(unsigned int port, unsigned int val)
{
	IO_SREG(PSE_OQUEUE_EN, val);
	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|PSE_CFG_WR_EN|PSE_CFG_OQEN_SEL);

	return 0;
}

/*
    only 7523, 7552 see pse_cfg_oq_en_set
*/
static inline unsigned int pse_set_oq_en_by_queue(unsigned int port, unsigned int queue, unsigned int val)
{
    unsigned int tmp_val; 

    tmp_val = pse_get_oq_en_by_port(port);

    if(val)
        tmp_val |= (1<<queue);
    else
        tmp_val &= (~(1<<queue));

    IO_SREG(PSE_OQUEUE_EN, tmp_val);
	IO_SREG(PSE_QUEUE_CFG_WR, (port<<PSE_CFG_PORT_ID_SHIFT)|PSE_CFG_WR_EN|PSE_CFG_OQEN_SEL);

	return 0;
}

//#endif


#endif /* _FEMAC_H */

