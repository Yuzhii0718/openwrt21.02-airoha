/***************************************************************/
#ifndef _PPE2_REG_H
#define _PPE2_REG_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include "frame_engine.h"
#include "util.h"

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define PPE2_SUPPORTED    (isEN7581?1:0)

#define PPE2_SRAM_HASH_IDX_BASE  0x2000
#define PPE2_DRAM_HASH_IDX_BASE  0x8000
    
#define RALINK_PPE2_BASE				0xBFB51c00

#define PPE2_FQFC_CFG	    RALINK_PPE2_BASE + 0x00
#define PPE2_IQ_CFG	    RALINK_PPE2_BASE + 0x04
#define PPE2_QUE_STA	    RALINK_PPE2_BASE + 0x08

#define PPE2_GLO_CFG	    	RALINK_PPE2_BASE + 0x200
#define PPE2_FLOW_CFG	    RALINK_PPE2_BASE + 0x204

#define PPE2_FLOW_SET	    PPE2_FLOW_CFG
#define PPE2_IP_PROT_CHK	    RALINK_PPE2_BASE + 0x208
#define PPE2_IP_PROT_0	    RALINK_PPE2_BASE + 0x20C
#define PPE2_IP_PROT_1	    RALINK_PPE2_BASE + 0x210
#define PPE2_IP_PROT_2	    RALINK_PPE2_BASE + 0x214
#define PPE2_IP_PROT_3	    RALINK_PPE2_BASE + 0x218
#define PPE2_TB_CFG	    	RALINK_PPE2_BASE + 0x21C
#define PPE2_FOE_CFG	    	PPE2_TB_CFG
#define PPE2_TB_BASE	    	RALINK_PPE2_BASE + 0x220
#define PPE2_FOE_BASE	    PPE2_TB_BASE
#define PPE2_TB_USED	    	RALINK_PPE2_BASE + 0x224
#define PPE2_BNDR	    	RALINK_PPE2_BASE + 0x228
#define PPE2_FOE_BNDR	    PPE2_BNDR
#define PPE2_BIND_LMT_0	    RALINK_PPE2_BASE + 0x22C
#define PPE2_FOE_LMT1	    PPE2_BIND_LMT_0
#define PPE2_BIND_LMT_1	    RALINK_PPE2_BASE + 0x230
#define PPE2_FOE_LMT2	    PPE2_BIND_LMT_1
#define PPE2_KA		    	RALINK_PPE2_BASE + 0x234
#define PPE2_FOE_KA	    	PPE2_KA
#define PPE2_UNB_AGE	    	RALINK_PPE2_BASE + 0x238
#define PPE2_FOE_UNB_AGE	    PPE2_UNB_AGE
#define PPE2_BND_AGE_0	    RALINK_PPE2_BASE + 0x23C
#define PPE2_FOE_BND_AGE0    PPE2_BND_AGE_0
#define PPE2_BND_AGE_1	    RALINK_PPE2_BASE + 0x240
#define PPE2_FOE_BND_AGE1    PPE2_BND_AGE_1
#define PPE2_HASH_SEED	    RALINK_PPE2_BASE + 0x244
#define PPE2_DFP_CPORT	    RALINK_PPE2_BASE + 0x248
#define PPE2_DFP_CPORT1	    RALINK_PPE2_BASE + 0x24c
#define PPE2_TB_HASH_CFG	    RALINK_PPE2_BASE + 0X250
#define PPE2_FCNT_CTRL	    RALINK_PPE2_BASE + 0X250
#define PPE2_FCNT_PCNT	    RALINK_PPE2_BASE + 0X254
#define PPE2_MIRROR			RALINK_PPE2_BASE + 0X254
#define PPE2_FCNT_BCNT_L	    RALINK_PPE2_BASE + 0X258
#define PPE2_FCNT_BCNT_H	    RALINK_PPE2_BASE + 0X25C
#define PPE2_MCAST_PPSE	    RALINK_PPE2_BASE + 0x284

#define PPE2_L2B_CFG         RALINK_PPE2_BASE + 0x288
#define PPE2_TLS_CFG0        RALINK_PPE2_BASE + 0x290
#define PPE2_TLS_CFG1        RALINK_PPE2_BASE + 0x294
#define PPE2_TLS_CFG2        RALINK_PPE2_BASE + 0x298
#define PPE2_TLS_CFG3        RALINK_PPE2_BASE + 0x29c
#define PPE2_TLS_CFG4        RALINK_PPE2_BASE + 0x2a0
#define PPE2_TLS_CFG5        RALINK_PPE2_BASE + 0x2a4
#define PPE2_TLS_VID0        RALINK_PPE2_BASE + 0x2b0
#define PPE2_TLS_VID1        RALINK_PPE2_BASE + 0x2b4
#define PPE2_TLS_VID2        RALINK_PPE2_BASE + 0x2b8
#define PPE2_TLS_VID3        RALINK_PPE2_BASE + 0x2bc
#define PPE2_TLS_VID4        RALINK_PPE2_BASE + 0x2c0
#define PPE2_TLS_VID5        RALINK_PPE2_BASE + 0x2c4
#define PPE2_TLS_VID6        RALINK_PPE2_BASE + 0x2c8
#define PPE2_TLS_VID7        RALINK_PPE2_BASE + 0x2cc

#define PPE2_MTR_BBS0        RALINK_PPE2_BASE + 0x2F0
#define PPE2_MTR_BBS1        RALINK_PPE2_BASE + 0x2F4
#define PPE2_MTR_PBS0        RALINK_PPE2_BASE + 0x2F8
#define PPE2_MTR_PBS1        RALINK_PPE2_BASE + 0x2FC
#define PPE2_VLAN_TPID	    RALINK_PPE2_BASE + 0x318

#define PPE2_MTU_DROP	    RALINK_PPE2_BASE + 0x300
#define PPE2_MTU_VLYR0	    RALINK_PPE2_BASE + 0x304
#define PPE2_MTU_VLYR1	    RALINK_PPE2_BASE + 0x308
#define PPE2_MTU_VLYR2	    RALINK_PPE2_BASE + 0x30c

#define PPE2_MTU_DRP	    	RALINK_PPE2_BASE + 0x308
#define PPE2_MTU_VLYR_0	    RALINK_PPE2_BASE + 0x30C
#define PPE2_MTU_VLYR_1	    RALINK_PPE2_BASE + 0x310
#define PPE2_MTU_VLYR_2	    RALINK_PPE2_BASE + 0x314

#define PPE2_MTU_VLYR_3	    RALINK_PPE2_BASE + 0x310


#define PPE2_TPID	    RALINK_PPE2_BASE + 0x318

/****************  EN7580 Start  ********************/
#define PPE2_L2B_ETYPE_EN        (RALINK_PPE2_BASE + 0x28C)
#define PPE2_L2B_ETYPE_N(idx)    (RALINK_PPE2_BASE + 0x2D0 + ((idx/2)<<2))

#define PPE2_SRAM_CTRL           (RALINK_PPE2_BASE + 0x31C)
#define PPE2_SRAM_ENTRY_BASE     (RALINK_PPE2_BASE + 0x320)

#if 0/*0x370/0x374 access only by PPE1*/
#define PPE2_UPDMEM_CTRL     (RALINK_PPE2_BASE + 0x370)
#define PPE2_UPDMEM_DATA     (RALINK_PPE2_BASE + 0x374)
#endif
/****************  EN7580 End  ********************/

#define PPE2_NPTV6_PREFIX_CTRL	(RALINK_PPE2_BASE+0x378)
#define PPE2_NPTV6_PREFIX_DATA	(RALINK_PPE2_BASE+0x37c)

#define ppe2FidIntoHashKeyEnable()				IO_SBITS(PPE2_GLO_CFG, FID_HASH_EN)
#define ppe2FidIntoHashKeyDisable()				IO_CBITS(PPE2_GLO_CFG, FID_HASH_EN)
#define ppe2IsFidIntoHashKeyEnable()			(IO_GREG(PPE2_GLO_CFG) & FID_HASH_EN)
#define ppe2SetL2tpParserEnable()			IO_SBITS(PPE2_GLO_CFG, L2TP_PARSER_EN)
#define ppe2SetL2tpParserDisable()			IO_CBITS(PPE2_GLO_CFG, L2TP_PARSER_EN)
#define ppe2GetNptV6Mode()					((IO_GREG(PPE2_GLO_CFG) & NPTV6_MODE)>>NPTV6_MODE_SHIFT)
#define ppe2SetNptV6Mode(val)				IO_SMASK(PPE2_GLO_CFG, NPTV6_MODE_MASK, NPTV6_MODE_SHIFT, val)


#define ppe2SramFlowTableEnable()				IO_SBITS(PPE2_TB_HASH_CFG, SRAM_TB_EN)
#define ppe2SramFlowTableDisable()				IO_CBITS(PPE2_TB_HASH_CFG, SRAM_TB_EN)
#define ppe2IsSramFlowTableEnable()				(IO_GREG(PPE2_TB_HASH_CFG) & SRAM_TB_EN)
#define ppe2DramFlowTableEnable()				IO_SBITS(PPE2_TB_HASH_CFG, DRAM_TB_EN)
#define ppe2DramFlowTableDisable()				IO_CBITS(PPE2_TB_HASH_CFG, DRAM_TB_EN)
#define ppe2IsDramFlowTableEnable()				(IO_GREG(PPE2_TB_HASH_CFG) & DRAM_TB_EN)
#define ppe2SramHash1ModeEnable()				IO_SBITS(PPE2_TB_HASH_CFG, SRAM_HASH1_EN)
#define ppe2SramHash1ModeDisable()				IO_CBITS(PPE2_TB_HASH_CFG, SRAM_HASH1_EN)
#define ppe2IsSramHash1ModeEnable()				(IO_GREG(PPE2_TB_HASH_CFG) & SRAM_HASH1_EN)
#define ppe2DramHash1ModeEnable()				IO_SBITS(PPE2_TB_HASH_CFG, DRAM_HASH1_EN)
#define ppe2DramHash1ModeDisable()				IO_CBITS(PPE2_TB_HASH_CFG, DRAM_HASH1_EN)
#define ppe2IsDramHash1ModeEnable()				(IO_GREG(PPE2_TB_HASH_CFG) & DRAM_HASH1_EN)

#define ppe2SetVlanOperationMode()			IO_SBITS(PPE2_TB_CFG, VLAN_OP_MODE)
#define ppe2SetVlanLayerMode()				IO_CBITS(PPE2_TB_CFG, VLAN_OP_MODE)
#define ppe2SetFINFastRp()					IO_SBITS(PPE2_TB_CFG, FIN_FAST_RP)
#define ppe2SetSramTableEntryNum(val)		IO_SMASK(PPE2_TB_CFG, SRAM_TB_ETRY_NUM_MASK, SRAM_TB_ETRY_NUM_SHIFT, val)
#define ppe2GetSramTableEntryNum()			IO_GMASK(PPE2_TB_CFG, SRAM_TB_ETRY_NUM_MASK, SRAM_TB_ETRY_NUM_SHIFT)
#define ppe2SetDramTableEntryNum(val)		IO_SMASK(PPE2_TB_CFG, DRAM_TB_ETRY_NUM_MASK, DRAM_TB_ETRY_NUM_SHIFT, val)
#define ppe2GetDramTableEntryNum()			IO_GMASK(PPE2_TB_CFG, DRAM_TB_ETRY_NUM_MASK, DRAM_TB_ETRY_NUM_SHIFT)
#define ppe2SetSramHash1Mode(val)			IO_SMASK(PPE2_TB_HASH_CFG, SRAM_HASH1_MODE_MASK, SRAM_HASH1_MODE_SHIFT, val)
#define ppe2GetSramHash1Mode()				IO_GMASK(PPE2_TB_HASH_CFG, SRAM_HASH1_MODE_MASK, SRAM_HASH1_MODE_SHIFT)
#define ppe2SetSramHash0Mode(val)			IO_SMASK(PPE2_TB_HASH_CFG, SRAM_HASH0_MODE_MASK, SRAM_HASH0_MODE_SHIFT, val)
#define ppe2GetSramHash0Mode()				IO_GMASK(PPE2_TB_HASH_CFG, SRAM_HASH0_MODE_MASK, SRAM_HASH0_MODE_SHIFT)
#define ppe2SetDramHashMode(val)				IO_SMASK(PPE2_TB_HASH_CFG, DRAM_HASH_MODE_MASK, DRAM_HASH_MODE_SHIFT, val)
#define ppe2GetDramHashMode()				IO_GMASK(PPE2_TB_HASH_CFG, DRAM_HASH_MODE_MASK, DRAM_HASH_MODE_SHIFT)
#define ppe2SetDramHash1Mode(val)			IO_SMASK(PPE2_TB_HASH_CFG, DRAM_HASH1_MODE_MASK, DRAM_HASH1_MODE_SHIFT, val)
#define ppe2GetDramHash1Mode()				IO_GMASK(PPE2_TB_HASH_CFG, DRAM_HASH1_MODE_MASK, DRAM_HASH1_MODE_SHIFT)

#define ppe2SetHashShiftVector(val)			IO_SMASK(PPE2_TB_CFG, HASH_SHIFT_VECTOR_MASK, HASH_SHIFT_VECTOR_SHIFT, val)
#define ppe2GetHashShiftVector()				IO_GMASK(PPE2_TB_CFG, HASH_SHIFT_VECTOR_MASK, HASH_SHIFT_VECTOR_SHIFT)
#define ppe2SetKeepAliveCfg(val)				IO_SMASK(PPE2_TB_CFG, KA_CFG_MASK, KA_CFG_SHIFT, val)
#define ppe2GetKeepAliveCfg()				IO_GMASK(PPE2_TB_CFG, KA_CFG_MASK, KA_CFG_SHIFT)
#define ppe2SetSearchMissAction(val)			IO_SMASK(PPE2_TB_CFG, SEARCH_MISS_ACTION_MASK, SEARCH_MISS_ACTION_SHIFT, val)
#define ppe2GetSearchMissAction()			IO_GMASK(PPE2_TB_CFG, SEARCH_MISS_ACTION_MASK, SEARCH_MISS_ACTION_SHIFT)
#define ppe2SetTableEntrySize80Byte()		IO_SBITS(PPE2_TB_CFG, TB_ENTRY_SIZE)
#define ppe2SetTableEntrySize64Byte()		IO_CBITS(PPE2_TB_CFG, TB_ENTRY_SIZE)

#define ppe2L2bNoIPEnable()						IO_SBITS(PPE2_L2B_CFG, L2B_NO_IP_EN)
#define ppe2L2bNoIPDisable()					IO_CBITS(PPE2_L2B_CFG, L2B_NO_IP_EN)
#define ppe2L2bBroadcastEnable()				IO_SBITS(PPE2_L2B_CFG, L2B_BC_EN)
#define ppe2L2bBroadcastDisable()				IO_CBITS(PPE2_L2B_CFG, L2B_BC_EN)
#define ppe2L2bMulticastEnable()				IO_SBITS(PPE2_L2B_CFG, L2B_MC_EN)
#define ppe2L2bMulticastDisable()				IO_CBITS(PPE2_L2B_CFG, L2B_MC_EN)
#define ppe2IsL2bMulticastEnable()				(IO_GREG(PPE2_L2B_CFG) & L2B_MC_EN)
#define ppe2L2bSetEtypeBlackList()				IO_SBITS(PPE2_L2B_CFG, L2B_ETYPE_BLIST)
#define ppe2L2bSetEtypeWhiteList()				IO_CBITS(PPE2_L2B_CFG, L2B_ETYPE_BLIST)
#define ppe2IsL2bEtypeBlackList()				(IO_GREG(PPE2_L2B_CFG) & L2B_ETYPE_BLIST)
#define ppe2L2bSmacEnable()						IO_SBITS(PPE2_L2B_CFG, L2B_SMAC_EN)
#define ppe2L2bSmacDisable()					IO_CBITS(PPE2_L2B_CFG, L2B_SMAC_EN)
#define ppe2IsL2bSmacEnable()					(IO_GREG(PPE2_L2B_CFG) & L2B_SMAC_EN)

#define ppe2SramSetCtrlEntryVal(val)			IO_SMASK(PPE2_SRAM_CTRL, SRAM_CTRL_ENTRY_MASK, SRAM_CTRL_ENTRY_SHIFT, val)
#define ppe2SramGetCtrlEntryVal()				IO_GMASK(PPE2_SRAM_CTRL, SRAM_CTRL_ENTRY_MASK, SRAM_CTRL_ENTRY_SHIFT)

#define ppe2Enable()                            IO_SBITS(PPE2_GLO_CFG, PPE_EN)
#define ppe2Disable()                           IO_CBITS(PPE2_GLO_CFG, PPE_EN)
#define ppe2IsEnabled()                         (IO_GREG(PPE2_GLO_CFG) & PPE_EN)   

/************************************************************************
*               M A C R O S
*************************************************************************
*/
#define PPE2_IS_IPV6_FLAB_EBL()	((RegRead(PPE2_FLOW_SET) & BIT_IPV6_HASH_FLAB) ? 1 : 0)
#define PPE2_IS_FLAB_EBL_HIGH_PRIO()	((RegRead(PPE2_FLOW_SET) & BIT_IPV6_MC_HPRI_EN) ? 0 : 1)


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

#endif

