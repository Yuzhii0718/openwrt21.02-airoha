#ifndef _AIR_ETH_XSGMII_CONFIG_
#define _AIR_ETH_XSGMII_CONFIG_
/***Basic config***/
#include "xfi_pcs_20210924_reg.h"
#include "rate_adaption_csr_XGMII_reg.h"
#include "rate_adaption_csr_XGMII_hal_reg.h"
#include "xfi_pcs_20210924_hal_reg.h"
#include "hsgmii_csr_pcs_reg.h"
#include "hsgmii_csr_pcs_hal_reg.h"
#include "hsgmii_an_csr_reg.h"
#include "hsgmii_an_csr_hal_reg.h"
#include "xfi_ana_pxp_reg.h"
#include "xfi_ana_pxp_hal_reg.h"
#include "../sgmii_globaldef.h"
#include "air_xsgmii.h"
#include "en7581_xfi_pma_reg.h"
#include "en7581_xfi_pma_hal_reg.h"
#include "multi_sgmii_csr_RX_reg.h"
#include "multi_sgmii_csr_RX_hal_reg.h"
#include "multi_phy_ana_hal_reg.h"
#include "multi_phy_ana_reg.h"
#include "multi_sgmii_csr_RX_RO_reg.h"
#include "multi_sgmii_csr_RX_RO_hal_reg.h"
#include "multi_sgmii_csr_TX_reg.h"
#include "multi_sgmii_csr_TX_hal_reg.h"
#include "air_eth_xsgmii_tx_fir.h"


#define ETH_INDEX            			        0x4

#define USXGMII_INT_EN              			0x5f0
#define USXGMII_INT_STA_CLR         			0x5f4
#define USXGMII_INT_STA             			0x5f8
	
#define USXGMII_PCS1_BASE_OFFSET    			(RgAddr) 0x0900
#define USXGMII_PCS2_BASE_OFFSET    			(RgAddr) 0x0A00
#define SGMII_AN_BASE_OFFSET      				(RgAddr) 0x0000
#define MULTI_PHY_ANA_BASE_OFFSET   			(RgAddr) 0x0000
#define PXP_BASE_OFFSET   						(RgAddr) 0x0000
#define PMA_BASE_OFFSET   						(RgAddr) 0xB000

#define USXGMII_RA_BASE_OFFSET      		 	(RgAddr) 0x0000
#define USXGMII_MULTI_SGMII_BASE_OFFSET      	(RgAddr) 0x0000

#define xsgmii_table air_eth_xsgmii_of_id
#define COMPATIBLE "airoha,air-eth_usxgmii"
#define XSGMII_DRV air_eth_xsgmii_driver
#define DESCRIPTION "eth xsgmii driver"
#define NAME "air_eth_xsgmii"
#define VER "AN7581.ETH.2.2.2R"
#define DRV ETH_XSGMII_DRV



/***Additional feature***/
#define Enable_IRQ

//Support FPGA
#ifndef CONFIG_USE_MT7520_ASIC
	#define FPGA_PLATFORM
#endif	
//Support common phy
#define RESOUCE_FROM_COMMON_MODULE
#ifdef RESOUCE_FROM_COMMON_MODULE
	#include "../../combo_phy/en7581/ecnt_serdes_common_phy.h"
#endif

//Support scu mod & rst operation
#define SCU_MOD_RST_OP
#ifdef SCU_MOD_RST_OP
	extern u32 GET_NP_SCU_SERDES_ETHERNET_SEL(void);
	extern void SET_NP_SCU_SERDES_ETHERNET_SEL(u32 val);
	extern u32 GET_SCU_RST_RG(void);
	extern void SET_SCU_RST_RG(u32 val);
//	static u32 (*fptr_SCU_GetModSel)(void)= GET_NP_SCU_SERDES_ETHERNET_SEL;
	static void (*fptr_SCU_SetModSel)(u32)= SET_NP_SCU_SERDES_ETHERNET_SEL;
	static u32 (*fptr_SCU_GetModRst)(void)= GET_SCU_RST_RG;
	static void (*fptr_SCU_SetModRst)(u32)= SET_SCU_RST_RG;
//	#define SCU_GetModSel (*fptr_SCU_GetModSel)
	#define SCU_SetModSel (*fptr_SCU_SetModSel)
	#define SCU_GetModRst (*fptr_SCU_GetModRst)
	#define SCU_SetModRst (*fptr_SCU_SetModRst)
	#define SCU_PHY_RST_POS 8	
	#define SCU_MAC_RST_POS 7
	#define SCU_RST_BIT 0b11
#endif
//Debug print
extern u8 dbg_print;
extern RGDATA_t api_aux;
#define MAC_Polling_Print_EN api_aux.bit.b0
#define MAC_Polling_skip_rg_dump api_aux.bit.b1
//---------------------------aux---------------------------
static RGDATA_t aux = {(0b11111111<<24)|(0b11111110<<16)|(0b10101110<<8)|(0b01001101<<0)};
//PMA
#define RX_PRCal_EN aux.bit.b9
//PCS
#define PCS_SGMII_force_mode_patch_EN aux.bit.b0
#define USX_FORCE_USR_DATA aux.bit.b3
#define USX_ANTMR aux.bit.b4
#define USR_EOF 0xfd
#define USR_IDLE 0x07070707
#define USR_DATA  USR_IDLE 
//MISC
#define XSGMII_USXGMII_INT_EN aux.bit.b1
#define XSGMII_SGMII_INT_EN aux.bit.b2
#define XSGMII_Linkup_Wrapper_EN aux.bit.b5
#define XSGMII_Linkdn_Wrapper_EN aux.bit.b6
#define XSGMII_SigDet_Wrapper_EN aux.bit.b7
#define XSGMII_Linkdn_Ignone_SD_EN aux.bit.b8
#define XSGMII_SigDet_A_EN aux.bit.b10

#define XSGMII_TX_FIR_Load_Para_EN aux.bit.b11
#define TX_SWING TX_SW660mV
#define TX_TRACE_LEN TX_5_inch

#define RX_Force_FEGain_FEPeaking_EN aux.bit.b12
#define USXGMII_RX_FEGain 1
#define USXGMII_RX_FEPeaking 2
#define HSGMII_RX_FEGain 1
#define HSGMII_RX_FEPeaking 1
#define SGMII_RX_FEGain 1
#define SGMII_RX_FEPeaking 1

#define RX_Force_FEOS_EN aux.bit.b13
#define RX_PR_SEQ_NEW_EN aux.bit.b14
#define RX_CDR_RST_NEW_EN aux.bit.b15
#define RX_PRCal_REF_RESETB_HI_EN aux.bit.b16
#define XSGMII_Linkdn_after_linkup_EN aux.bit.b17
#define RX_RATE_CTRL_FLL aux.bit.b18

#define TX_EF_IMP_EN aux.bit.b19
#define RX_EF_IMP_EN aux.bit.b20
#define HIBER_OFF_EN aux.bit.b21

//---------------------------flag---------------------------
static RGDATA_t xsgmii_flag = {0};
//PMA
//PCS
#define linkup_sta xsgmii_flag.bit.b8
#define linkdn_sta xsgmii_flag.bit.b9
//MISC
#define XSGMII_REQ_IRQ xsgmii_flag.bit.b24

//---------------------------dbg---------------------------
// static RGDATA_t dbg ={(0b11111111<<24)|(0b11111111<<16)|(0b11111111<<8)|(0b11111111<<0)};
static u8 tFE_Gain = 1;
#endif
