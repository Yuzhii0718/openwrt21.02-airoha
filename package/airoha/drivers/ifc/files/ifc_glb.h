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
#ifndef _IFC_GLB_H_
#define _IFC_GLB_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <asm/tc3162/tc3162.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/	
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
#endif
	#define IO_GREG_REP(reg, buf, count)			ioread32_rep((void __iomem *)(reg), buf, count)
	#define IO_SREG_REP(reg, buf, count)			iowrite32_rep((void __iomem *)(reg), buf, count)
#endif /* CONFIG_SIMULATION */

#if defined(TCSUPPORT_CPU_EN7523)
#define IFC_LUT0_TOTAL_ENTRY			(8)
#define IFC_LUT1_SUBTABLE_NUM			(2)
#else
#define IFC_LUT0_TOTAL_ENTRY			(16)
#define IFC_LUT1_SUBTABLE_NUM			(7)
#endif
#define IFC_LUT1_ENTRY_PER_SUBTABLE		(16)
#define IFC_LUT1_TOTAL_ENTRY			(IFC_LUT1_SUBTABLE_NUM*IFC_LUT1_ENTRY_PER_SUBTABLE)

#define IFC_LUT0_FCPU_RING_NUM			(2) //for 7516/27

#define IFC_LUT1_THRESHOLD_MAX			(8)
#define IFC_ACNT_ID_MAX					(16)

/*
 * DEFINITIONS AND MACROS
 */
#ifndef NEXTHDR_IPIP
#define NEXTHDR_IPIP 4
#endif
#define RALINK_FRAME_ENGINE_BASE        (0xbfb50000)

#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7528)
#define FE_IFC_BASE				 RALINK_FRAME_ENGINE_BASE + 0x1a0
#define FE_IFC_FCPU				(FE_IFC_BASE+0x0020)
#define FE_IFC_RXQ0				(FE_IFC_BASE+0x0024)
#define FE_IFC_RXQ1				(FE_IFC_BASE+0x0028)
#elif defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
#define FE_IFC_BASE				(RALINK_FRAME_ENGINE_BASE + 0x200)
#endif
#define FE_IFC_UDF(i)			(FE_IFC_BASE+0x0020+(i<<2))

#define FE_IFC_CFG				(FE_IFC_BASE+0x0000)
#define FE_IFC_CTRL				(FE_IFC_BASE+0x0004)
#define FE_IFC_DATA0			(FE_IFC_BASE+0x0010)
#define FE_IFC_DATA1			(FE_IFC_BASE+0x0014)
#define FE_IFC_DATA2			(FE_IFC_BASE+0x0018)
#define FE_IFC_DATA3			(FE_IFC_BASE+0x001C)

#define	ATK_IP_KEY0		(0)
#define	ATK_IP_KEY1		(1)
#define	ATK_IP_KEY2		(2)
#define	ATK_IP_IDX_MAX		(3)
#define	IFC_ATK_IP_KEY(i)	(RALINK_FRAME_ENGINE_BASE + 0x2050 + (i<<3))
#define	IFC_ATK_IP_MSK(i)	(RALINK_FRAME_ENGINE_BASE + 0x2054 + (i<<3))

#define FE_IFC_CNT_ID_N(idx)	(FE_IFC_BASE + 0x0040 + ((idx/8)<<2))

/* FE_IFC_CFG */
#define FE_IFC_EN							(1<<0)
#define FE_P0_IFC_EN                        (1<<1)
#define FE_IFC_OTHERS_EN                    (1<<23)
#define FE_IFC_MASK							(1)
#define FE_IFC_SHIFT						(0)

#define IFC_DPI_EN_BIT					(13)

/* FE_IFC_CTRL */
#define FE_IFC_ACK							(1<<31)
#define FE_IFC_ID_SHIFT						(16)
#define FE_IFC_ID_MASK						(0xFF<<FE_IFC_ID_SHIFT)
#define FE_IFC_LUT0_OFST_SHIFT				(8)
#define FE_IFC_LUT0_OFST_MASK				(0x3F<<FE_IFC_LUT0_OFST_SHIFT)
#define FE_IFC_SEL_LUT0					(0)
#define FE_IFC_SEL_LUT1					(1)
#define FE_IFC_SEL_ACT					(2)
#define FE_IFC_SEL_ACNT					(3)
#define FE_IFC_SEL_SHIFT					(4)
#define FE_IFC_SEL_MASK						(0x3<<FE_IFC_SEL_SHIFT)
#define FE_IFC_CMD_READ					(0)
#define FE_IFC_CMD_WRITE				(1)
#define FE_IFC_CMD_CLEAR_ALL_LUT		(2)
#define FE_IFC_CMD_SHIFT					(1)
#define FE_IFC_CMD_MASK						(0x3<<FE_IFC_CMD_SHIFT)
#define FE_IFC_REQ							(1<<0)

/* FE_IFC_UDF */
#define FE_IFC_UDF_L2_BASE				(0)
#define FE_IFC_UDF_L3_BASE				(1)
#define FE_IFC_UDF_L4_BASE				(2)
#define FE_IFC_UDF_BASE_SHIFT(idx)			((idx&0x1)<<4)
#define FE_IFC_UDF_BASE_MASK(idx)			(0x3<<FE_IFC_UDF_BASE_SHIFT(idx))
#define FE_IFC_UDF_OFFSET_SHIFT(idx)		(2+((idx&0x1)<<4))
#define FE_IFC_UDF_OFFSET_MASK(idx)			(0x3FFF<<FE_IFC_UDF_OFFSET_SHIFT(idx))


/* FE_IFC_CFG */
#define FE_IFC_PORT_EN	(RALINK_FRAME_ENGINE_BASE + 0x1F4)
#define IfcPortEnable(val)				IO_SBITS(FE_IFC_PORT_EN, (1 << val))
#define IfcPortDisable(val)				IO_CBITS(FE_IFC_PORT_EN, (1 << val))

#define IfcEnable						IO_SBITS(FE_IFC_CFG, FE_IFC_EN)
#define IfcDisable						IO_CBITS(FE_IFC_CFG, FE_IFC_EN)
#define IfcGetEnable					IO_GMASK(FE_IFC_CFG, FE_IFC_MASK, FE_IFC_SHIFT)

/* FE_IFC_CTRL */
#define IfcSetRuleId(val)				IO_SMASK(FE_IFC_CTRL, FE_IFC_ID_MASK, FE_IFC_ID_SHIFT, val)
#define IfcGetRuleId					IO_GMASK(FE_IFC_CTRL, FE_IFC_ID_MASK, FE_IFC_ID_SHIFT)
#define IfcSetSelect(val)				IO_SMASK(FE_IFC_CTRL, FE_IFC_SEL_MASK, FE_IFC_SEL_SHIFT, val)
#define IfcGetSelect					IO_GMASK(FE_IFC_CTRL, FE_IFC_SEL_MASK, FE_IFC_SEL_SHIFT)
#define IfcSetCmd(val)					IO_SMASK(FE_IFC_CTRL, FE_IFC_CMD_MASK, FE_IFC_CMD_SHIFT, val)
#define IfcGetCmd						IO_GMASK(FE_IFC_CTRL, FE_IFC_CMD_MASK, FE_IFC_CMD_SHIFT)
#define IfcRequestEn					IO_SBITS(FE_IFC_CTRL, FE_IFC_REQ)

/* FE_IFC_UDF */
#define IfcSetUdfBase(idx, val)			IO_SMASK(FE_IFC_UDF((idx>>1)), FE_IFC_UDF_BASE_MASK(idx), FE_IFC_UDF_BASE_SHIFT(idx), val)
#define IfcGetUdfBase(idx)				IO_GMASK(FE_IFC_UDF((idx>>1)), FE_IFC_UDF_BASE_MASK(idx), FE_IFC_UDF_BASE_SHIFT(idx))
#define IfcSetUdfOffset(idx, val)		IO_SMASK(FE_IFC_UDF((idx>>1)), FE_IFC_UDF_OFFSET_MASK(idx), FE_IFC_UDF_OFFSET_SHIFT(idx), val)
#define IfcGetUdfOffset(idx)			IO_GMASK(FE_IFC_UDF((idx>>1)), FE_IFC_UDF_OFFSET_MASK(idx), FE_IFC_UDF_OFFSET_SHIFT(idx))

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#ifdef TCSUPPORT_CPU_ARMV8
#define PHYS_TO_K1(physaddr) phys_to_virt(physaddr)
#define RegRead(reg) 		get_frame_engine_data(reg)
#else
#define PHYS_TO_K1(physaddr) KSEG1ADDR(physaddr)	
#define RegRead(reg)		(uint32_t)regRead32(PHYS_TO_K1(reg))
#endif

#ifdef CONFIG_DEBUG
	#define IFC_MSG(level, F, B...)			{ \
												if(gpIfcPriv->dbgLevel >= level)	\
													printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B) ; \
											}
	#define IFC_MSG_STRIP(level, F, B...)	{ \
												if(gpIfcPriv->dbgLevel >= level) 	\
													printk(F, ##B) ; \
											}
	#define IFC_ERR(F, B...)				printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B)
	#define IFC_LOG(F, B...)				printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B)
#else
	#define IFC_MSG(level, F, B...)	
	#define IFC_ERR(F,B...)			printk(F, ##B)
	#define IFC_LOG(F,B...)			printk(F, ##B)
#endif

#define TCSUPPORT_IFC_P0_DISABLE	(isEN7580 || isEN7523)	/* IFC P0 conflict with NPU @7580/7523 */

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
	
typedef enum {
	IFC_LUT0 = 0 ,
	IFC_LUT1
} IFC_TYPE_t ;

typedef enum {
	Type_TCP = 0 ,
	Type_UDP,
	Type_None
} L4_Type ;

typedef enum {
	Type_IPv4 = 0 ,
	Type_IPv6,
	Type_6RD,
	Type_DSLite
} L3_Type ;

typedef struct {
	unchar			dbgLevel ;
	int				channel ;
	int				queue ;
	int				isPktChkErrFlag ;
	int				isRecvOKFlag ;
	uint			Lut0RecvIndexMap ;
	uint			Lut1RecvIndexMap[2] ;
	spinlock_t		ifcLock ;	/* spin lock for IFC Rule Config */
} IFC_Private_T ;

typedef struct{
	unchar type_changed;
	unchar field_type;
	uint field_num;
	uint ifc_idx;
	uint field;
	uint command;
	uint key;
	uint mask;
	uint key_high;
	uint mask_high;
}ifc_field_param;

#if 1
typedef	union {
	struct {
#ifdef __BIG_ENDIAN
		uint valid:1 ;
		uint enable:31 ;
#else
		uint enable:31 ;
		uint valid:1 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint :2 ;
		uint C31_17:30 ;
#else
		uint C31_17:30 ;
		uint :2 ;
#endif /* __BIG_ENDIAN */

		uint C16_1:32 ;

#ifdef __BIG_ENDIAN
		uint :14 ;
		uint M31:2 ;
		uint M30:16 ;
#else
		uint M30:16 ;
		uint M31:2 ;
		uint :14 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint M29:16 ;
		uint M28:16 ;
#else
		uint M28:16 ;
		uint M29:16 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint M27:8 ;
		uint M26:8 ;
		uint M25:8 ;
		uint M24:8 ;
#else
		uint M24:8 ;
		uint M25:8 ;
		uint M26:8 ;
		uint M27:8 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint M23:16 ;
		uint M22:16 ;
#else
		uint M22:16 ;
		uint M23:16 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint M21:4 ;
		uint M20:12 ;
		uint M19:4 ;
		uint M18:12 ;
#else
		uint M18:12 ;
		uint M19:4 ;
		uint M20:12 ;
		uint M21:4 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint M17:2 ;
		uint M16:2 ;
		uint M15:3 ;
		uint M14:5 ;
		uint M13:20 ;
#else
		uint M13:20 ;
		uint M14:5 ;
		uint M15:3 ;
		uint M16:2 ;
		uint M17:2 ;
#endif /* __BIG_ENDIAN */

		uint M12:32 ;
		uint M11:32 ;
		uint M10:32 ;
		uint M9:32 ;
		uint M8:32 ;
		uint M7:32 ;
		uint M6:32 ;
		uint M5:32 ;
		uint M4:32 ;
		uint M3:32 ;
		
		uint M2_H:32 ;
#ifdef __BIG_ENDIAN
		uint M2_L:16 ;
		uint M1_H:16 ;
#else
		uint M1_H:16 ;
		uint M2_L:16 ;
#endif /* __BIG_ENDIAN */
		uint M1_L:32 ;

#ifdef __BIG_ENDIAN
		uint :14 ;
		uint K31:2 ;
		uint K30:16 ;
#else
		uint K30:16 ;
		uint K31:2 ;
		uint :14 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint K29:16 ;
		uint K28:16 ;
#else
		uint K28:16 ;
		uint K29:16 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint K27:8 ;
		uint K26:8 ;
		uint K25:8 ;
		uint K24:8 ;
#else
		uint K24:8 ;
		uint K25:8 ;
		uint K26:8 ;
		uint K27:8 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint K23:16 ;
		uint K22:16 ;
#else
		uint K22:16 ;
		uint K23:16 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint K21:4 ;
		uint K20:12 ;
		uint K19:4 ;
		uint K18:12 ;
#else
		uint K18:12 ;
		uint K19:4 ;
		uint K20:12 ;
		uint K21:4 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint K17:2 ;
		uint K16:2 ;
		uint K15:3 ;
		uint K14:5 ;
		uint K13:20 ;
#else
		uint K13:20 ;
		uint K14:5 ;
		uint K15:3 ;
		uint K16:2 ;
		uint K17:2 ;
#endif /* __BIG_ENDIAN */

		uint K12:32 ;
		uint K11:32 ;
		uint K10:32 ;
		uint K9:32 ;
		uint K8:32 ;
		uint K7:32 ;
		uint K6:32 ;
		uint K5:32 ;
		uint K4:32 ;
		uint K3:32 ;
		
		uint K2_H:32 ;
#ifdef __BIG_ENDIAN
		uint K2_L:16 ;
		uint K1_H:16 ;
#else
		uint K1_H:16 ;
		uint K2_L:16 ;
#endif /* __BIG_ENDIAN */
		uint K1_L:32 ;
	};
	uint msg[41] ;
} IFC_LUT0_Format ;

typedef union {
	/* Type-0: for general use */
	union {
		struct{
#ifdef __BIG_ENDIAN
			uint :16 ;
			uint valid:1 ;
			uint end:1 ;
			uint Type:1 ;
			uint :3 ;
			uint S0:2 ;
			uint C0:2 ;
			uint F0:6 ;
#else
			uint F0:6 ;
			uint C0:2 ;
			uint S0:2 ;
			uint :3 ;
			uint Type:1 ;
			uint end:1 ;
			uint valid:1 ;
			uint :16 ;
#endif /* __BIG_ENDIAN */
			
#ifdef __BIG_ENDIAN
			uint M0:16 ;
			uint K0:16 ;
#else
			uint K0:16 ;
			uint M0:16 ;
#endif /* __BIG_ENDIAN */
		};
		uint msg[2] ;
	} IFC_LUT1_Format0;
	
	/* Type-1: used for MAC/IP with mask */
	union {
		struct{
#ifdef __BIG_ENDIAN
			uint :16 ;
			uint valid:1 ;
			uint end:1 ;
			uint Type:1 ;
			uint M0:5 ;
			uint D0:1 ;
			uint C0:1 ;
			uint F0:6 ;
#else
			uint F0:6 ;
			uint C0:1 ;
			uint D0:1 ;
			uint M0:5 ;
			uint Type:1 ;
			uint end:1 ;
			uint valid:1 ;
			uint :16 ;
#endif /* __BIG_ENDIAN */

			uint K0 ;
		};
		uint msg[2] ;
	} IFC_LUT1_Format1;
}IFC_LUT1_Format;

typedef	union {
	struct{
#ifdef __BIG_ENDIAN
		#ifdef TCSUPPORT_CPU_EN7523
		uint nbq_low:5 ;
		uint cpu:1 ;
		uint enable:14 ;
		uint owf:1 ;
		uint dpi:1 ;
		#else
		uint nbq_low:4 ;
		uint cpu:1 ;
		uint enable:15 ;
		uint :1 ;
		uint owf:1 ;
		#endif
		uint kpcp:1 ;
		uint mc:1 ;
		uint fast:1 ;
		uint fqos:1 ;
		uint vpm0:2 ;
		uint vo1:2 ;
		uint vo0:2 ;
#else
		uint vo0:2 ;
		uint vo1:2 ;
		uint vpm0:2 ;
		uint fqos:1 ;
		uint fast:1 ;
		uint mc:1 ;
		uint kpcp:1 ;
		#ifdef TCSUPPORT_CPU_EN7523
		uint dpi:1 ;
		uint owf:1 ;
		uint enable:14 ;
		uint cpu:1 ;
		uint nbq_low:5 ;
		#else
		uint owf:1 ;
		uint :1 ;
		uint enable:15 ;
		uint cpu:1 ;
		uint nbq_low:4 ;
		#endif
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint pcp1:4 ;
		uint vid1:12 ;
		uint pcp0:4 ;
		uint vid0:12 ;
#else
		uint vid0:12 ;
		uint pcp0:4 ;
		uint vid1:12 ;
		uint pcp1:4 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint ipv6_traffic_class:8 ;
		uint ipv4_dscp:8 ;
		uint meter_group_id:8 ;
		#ifdef TCSUPPORT_CPU_EN7523
		uint :1 ;
		uint cnt1:5 ;
		uint cnt0_hi:2 ;
		#else
		uint cnt1:6 ;
		uint cnt0_hi:2 ;
		#endif
#else
		#ifdef TCSUPPORT_CPU_EN7523
		uint cnt0_hi:2 ;
		uint cnt1:5 ;
		uint :1 ;
		#else
		uint cnt0_hi:2 ;
		uint cnt1:6 ;
		#endif
		uint meter_group_id:8 ;
		uint ipv4_dscp:8 ;
		uint ipv6_traffic_class:8 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint cnt0_low:4 ;
		uint sp_tag:16 ;
		#ifdef TCSUPPORT_CPU_EN7523
		uint fport:4 ;
		#else
		uint fport:3 ;
		uint nbq_high:1 ;
		#endif
		uint channel:5 ;
		uint queue:3 ;
#else
		uint queue:3 ;
		uint channel:5 ;
		#ifdef TCSUPPORT_CPU_EN7523
		uint fport:4 ;
		#else
		uint nbq_high:1 ;
		uint fport:3 ;
		#endif
		uint sp_tag:16 ;
		uint cnt0_low:4 ;
#endif /* __BIG_ENDIAN */

	};
	uint msg[4] ;
} IFC_Action_Format ;

#else
typedef union {
	/* Type-0: for general use */
	struct {
		union{
			struct{
#ifdef __BIG_ENDIAN
				uint :12 ;
				uint valid:1 ;
				uint end:1 ;
				uint Type:2 ;
				uint C1:2 ;
				uint C0:2 ;
				uint F1:6 ;
				uint F0:6 ;
#else
				uint F0:6 ;
				uint F1:6 ;
				uint C0:2 ;
				uint C1:2 ;
				uint Type:2 ;
				uint end:1 ;
				uint valid:1 ;
				uint :12 ;
#endif /* __BIG_ENDIAN */
				
#ifdef __BIG_ENDIAN
				uint M1:16 ;
				uint M0:16 ;
#else
				uint M0:16 ;
				uint M1:16 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
				uint K1:16 ;
				uint K0:16 ;
#else
				uint K0:16 ;
				uint K1:16 ;
#endif /* __BIG_ENDIAN */
			};
			uint msg[3] ;
		};
	}IFC_Format0;
	
	/* Type-1: used for MAC/IP with mask */
	struct {
		union{
			struct{
#ifdef __BIG_ENDIAN
				uint :12 ;
				uint valid:1 ;
				uint end:1 ;
				uint Type:2 ;
				uint D0:2 ;
				uint C0:2 ;
				uint M0:6 ;
				uint F0:6 ;
#else
				uint F0:6 ;
				uint M0:6 ;
				uint C0:2 ;
				uint D0:2 ;
				uint Type:2 ;
				uint end:1 ;
				uint valid:1 ;
				uint :12 ;
#endif /* __BIG_ENDIAN */
				
#ifdef __BIG_ENDIAN
				uint R0:16 ;
				uint K0_high:16 ;
#else
				uint K0_high:16 ;
				uint R0:16 ;
#endif /* __BIG_ENDIAN */
				
				uint K0_low ;
			};
			uint msg[3] ;
		};
	}IFC_Format1;
	
	/* Type-2: used for MAC/IP without mask */
	struct {
		union{
			struct{
#ifdef __BIG_ENDIAN
				uint :12 ;
				uint valid:1 ;
				uint end:1 ;
				uint Type:2 ;
				uint C1:2 ;
				uint C0:2 ;
				uint F1:6 ;
				uint F0:6 ;
#else
				uint F0:6 ;
				uint F1:6 ;
				uint C0:2 ;
				uint C1:2 ;
				uint Type:2 ;
				uint end:1 ;
				uint valid:1 ;
				uint :12 ;
#endif /* __BIG_ENDIAN */
				
#ifdef __BIG_ENDIAN
				uint K1:16 ;
				uint K0_high:16 ;
#else
				uint K0_high:16 ;
				uint K1:16 ;
#endif /* __BIG_ENDIAN */

				uint K0_low ;
			};
			uint msg[3] ;
		};
	}IFC_Format2;
}IFC_Rule_Format;
#endif


/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

#endif /* _IFC_GLB_H_ */
