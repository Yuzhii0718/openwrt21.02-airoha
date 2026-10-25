#ifndef _AIR_ETH_XSGMII_CONFIG_
#define _AIR_ETH_XSGMII_CONFIG_

#include "../../sgmii_globaldef.h"
#include "air_xsgmii.h"

/* CODA Reg mapping header */
#include "rate_adaption_csr_XGMII_reg.h"
#include "rate_adaption_csr_XGMII_hal_reg.h"
#include "rate_adaption_csr_MII_RATE_ADP_reg.h"
#include "rate_adaption_csr_MII_RATE_ADP_hal_reg.h"
#include "hsgmii_csr_pcs_reg.h"
#include "hsgmii_csr_pcs_hal_reg.h"
#include "hsgmii_an_csr_reg.h"
#include "hsgmii_an_csr_hal_reg.h"
#include "pon_hsgmii_ana_csr_c_header.h"
#include "pon_hsgmii_dig_csr_c_header.h"


#include "hsgmii_an_csr_c_header.h"
//#include "multi_phy_ra_csr_c_header.h"
#include "multi_sgmii_csr_c_header.h"
#include "hsgmii_csr_pcs_c_header.h"



#define USXGMII_INT_EN              			0x5f0
#define USXGMII_INT_STA_CLR         			0x5f4
#define USXGMII_INT_STA             			0x5f8
	
#define USXGMII_PCS1_BASE_OFFSET    			(RgAddr) 0x0900
#define USXGMII_PCS2_BASE_OFFSET    			(RgAddr) 0x0A00
#define SGMII_AN_BASE_OFFSET      				(RgAddr) 0x0000
#define MULTI_PHY_ANA_BASE_OFFSET   			(RgAddr) 0x0000
#define PXP_BASE_OFFSET   						(RgAddr) 0x0000
#define PMA_BASE_OFFSET   						(RgAddr) 0x4000 //7552
#define PHYA_BASE_OFFSET   						(RgAddr) 0x3000 //7552
#define PCS2_BASE_OFFSET 					(RgAddr) 0xA00 //7552
#define AN_BASE_OFFSET						(RgAddr) 0x0 //7552
#define MULTI_SGMII_BASE_OFFSET				(RgAddr) 0x0 //7552
#define RA_BASE_OFFSET						(RgAddr) 0x0 //7552


#define USXGMII_RA_BASE_OFFSET      		 	(RgAddr) 0x0000
#define USXGMII_MULTI_SGMII_BASE_OFFSET      	(RgAddr) 0x0000


#define xsgmii_table air_xpon_xsgmii_of_id
#define COMPATIBLE "airoha,air-xpon_xsgmii"
#define XSGMII_DRV air_xpon_xsgmii_driver
#define DESCRIPTION "xpon xsgmii driver"
#define NAME "air_xpon_xsgmii"
#define VER "7552.1.20230324"
#define DRV XPON_XSGMII_DRV
//#define Enable_IRQ

/* Additional feature */
//Support FPGA
#define FPGA_PLATFORM_AN7552
#ifndef CONFIG_USE_MT7520_ASIC
	#define FPGA_PLATFORM
#endif	

/* Support common phy */
#define RESOUCE_FROM_COMMON_MODULE
#ifdef RESOUCE_FROM_COMMON_MODULE
	#include "../../../combo_phy/an7552/serdes_cmn_phy_7552.h"
#endif

/* Support scu mod & rst operation */
#define SCU_MOD_RST_OP
#ifdef SCU_MOD_RST_OP
	extern u32 GET_NP_SCU_SERDES_PON_SEL(void);
	extern void SET_NP_SCU_SERDES_PON_SEL(u32 val);
	extern u32 GET_SCU_RST_RG(void);
	u32 GET_SCU_RSTCTRL1(void);
	extern void SET_SCU_RST_RG(u32 val);
	extern void SET_SCU_RSTCTRL1(u32 val);
	extern u32 GET_WAN_CONF(void);
	extern void SET_WAN_CONF(u32 val);
		
	static u32 (*fptr_SCU_GetModSel)(void)= GET_NP_SCU_SERDES_PON_SEL;
	static void (*fptr_SCU_SetModSel)(u32)= SET_NP_SCU_SERDES_PON_SEL;
	static u32 (*fptr_SCU_GetModRst)(void)= GET_SCU_RST_RG;	
	static u32 (*fptr_SCU_GetModRst1)(void)= GET_SCU_RSTCTRL1;
	static void (*fptr_SCU_SetModRst)(u32)= SET_SCU_RST_RG;
	static void (*fptr_SCU_SetModRst1)(u32)= SET_SCU_RSTCTRL1;
	#define SCU_GetModSel (*fptr_SCU_GetModSel)
	#define SCU_SetModSel (*fptr_SCU_SetModSel)
	#define SCU_GetModRst (*fptr_SCU_GetModRst)	
	#define SCU_GetModRst1 (*fptr_SCU_GetModRst1)
	#define SCU_SetModRst (*fptr_SCU_SetModRst)
	#define SCU_SetModRst1 (*fptr_SCU_SetModRst1)
	#define SCU_PHY_RST_POS 0
	#define SCU_RST_BIT 0b1
	#define SCU_MAC_RST_POS 31
#endif

/* Debug print */
extern u8 dbg_print;

#endif

