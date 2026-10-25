/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��)
are protected under relevant copyright laws. The information contained herein
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or
its licensors. Without the prior written permission of EcoNet and/or its licensors,
any reproduction, modification, use or disclosure of EcoNet Software, and
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS��
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE.
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR
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
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <uapi/ecnt_event_global/ecnt_event_system.h>
#include "H/air_xsgmii.h"  
#include "H/air_usb_xsgmii_config.h"
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <asm/delay.h>



/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
static volatile xsgmii_base * _pxSGMII;
 
static const struct of_device_id xsgmii_table[] = {
    { .compatible = COMPATIBLE},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, xsgmii_table);
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void USB_xSGMII_Ver(void){
	printk("ETH XSGMII VER = %s\n",VER);
}
EXPORT_SYMBOL(USB_xSGMII_Ver);
static void RG_W(REG_t* rg){		
#ifdef FPGA_PLATFORM
	if (rg->base == _pxSGMII->phya_base) return;
	else writel(rg->data.value, rg->base + (rg->addr - rg->base_start));
#else
	writel(rg->data.value, rg->base + (rg->addr - rg->base_start));
#endif
}

static u32 RG_R(REG_t* rg){
#ifdef FPGA_PLATFORM
	if (rg->base == _pxSGMII->phya_base) return 0;
	else return readl(rg->base + (rg->addr - rg->base_start));
#else
	return readl(rg->base + (rg->addr - rg->base_start));
#endif
}

static void RG_W_PL(RgAddr Base,RgAddr Base_start,RgAddr Addr,u32 Data){
	REG_t rg;
	if(Base != NULL)
	{
		rg.base = Base;
		rg.base_start = ((RgAddr) Base_start);
		rg.addr = ((RgAddr) Addr);
		rg.data.value = Data;
		RG_W(&rg);
	}
	else printk("please register usxgmii!!!\n");
}
	
static u32 RG_R_PL(RgAddr Base,RgAddr Base_start,RgAddr Addr){
	REG_t rg;
	if(Base != NULL)
	{
		rg.base = Base;
		rg.base_start = ((RgAddr) Base_start);
		rg.addr = ((RgAddr) Addr);
		return RG_R(&rg);
	}
	else
	{
		printk("please register usxgmii!!!\n");
		return 0;
	}
}

static void xsgmii_chg(u8 xsgmii){
	//u8 chg = OFF;
	SCU_SetModSel(0);
	SCU_SetModRst(SCU_GetModRst() | 0x48000);
	SCU_SetModRst(SCU_GetModRst()& ~(0x48000));
}


static void xSGMII_QPHY_PMA(u8 xsgmii){
	rg_type_t(HAL_sgmii_reg_phya_26) sgmii_reg_phya_26;
	//rg_type_t(HAL_sgmii_reg_phya_29) sgmii_reg_phya_29;//john
	rg_type_t(HAL_sgmii_reg_phya_24) sgmii_reg_phya_24;//john
	rg_type_t(HAL_sgmii_reg_phya_18) sgmii_reg_phya_18;
	rg_type_t(HAL_sgmii_reg_phya_19) sgmii_reg_phya_19;
	rg_type_t(HAL_sgmii_reg_phya_8) sgmii_reg_phya_8;
	rg_type_t(HAL_sgmii_reg_phya_6) sgmii_reg_phya_6;

	if (xsgmii == HSGMII){
		sgmii_reg_phya_8.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_8);	
		sgmii_reg_phya_8.hal.rg_ssusb_cdr_bicltr = 0b1111;
		sgmii_reg_phya_8.hal.rg_ssusb_cdr_bicltd0 = 0b1100;
		sgmii_reg_phya_8.hal.rg_ssusb_cdr_bicltd1 = 0b0011;
		sgmii_reg_phya_8.hal.rg_ssusb_cdr_bicltd1 = 0b0011;
		RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_8,sgmii_reg_phya_8.dat.value);
		
		sgmii_reg_phya_6.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_6);	
		sgmii_reg_phya_6.hal.rg_force_cdr_bic = 0x1;//john1213 0x1 <= 0x00
		RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_6,sgmii_reg_phya_6.dat.value);
	}
	sgmii_reg_phya_26.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_26);	
	sgmii_reg_phya_26.hal.rg_ssusb_ln0_cdr_rst_dly = 0x00;
	RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_26,sgmii_reg_phya_26.dat.value);

	//sgmii_reg_phya_29.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_29);//john
	//sgmii_reg_phya_29.hal.rg_ssusb_ln0_reserve = 0x00;//john
	//RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_29,sgmii_reg_phya_29.dat.value);//john
	sgmii_reg_phya_24.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_24);//john
	sgmii_reg_phya_24.hal.rg_ssusb_ln0_cdr_reserve = 0x00;//john ln0_cdr_reservev  rg[31:24]=0x00
	RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_24,sgmii_reg_phya_24.dat.value);//john
	
	sgmii_reg_phya_18.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_18);	
	sgmii_reg_phya_18.hal.rg_ssusb_bg_div = 0b01;
	RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_18,sgmii_reg_phya_18.dat.value);

	sgmii_reg_phya_19.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_19);	
	//sgmii_reg_phya_19.hal.rg_ssusb_xtal_top_reserve = 1;//john
	sgmii_reg_phya_19.hal.rg_ssusb_xtal_top_reserve = 0x200;//john
	RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_19,sgmii_reg_phya_19.dat.value);
}	

static void xSGMII_AN_AutoSetting(u8 xsgmii,u8 mode,u8 duplx,u8 rate){
	rg_type_t(HAL_sgmii_reg_an_13) sgmii_reg_an_13;
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_6) rg_hsgmii_pcs_ctrol_6;
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;	
#ifdef FPGA_PLATFORM
	RGDATA_t rg={0};
	if(xsgmii == USXGMII){
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL); 
		rg.bit.b24 = 1; //bitfile0714
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL,rg.value); 			
	}	
#endif 

		rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);	
		rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 1;
		rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 1;
		rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0;			
		rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0;
		RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg_rate_adapt_ctrl_0.dat.value); 
	
	if (xsgmii == SGMII){
	    //john add
		RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,((RgAddr)_rg_rate_adapt_ctrl_0+0x100),0);//6100=0
		
		rg_type_t(HAL_sgmii_reg_phya_30) sgmii_reg_phya_30;		
	    sgmii_reg_phya_30.dat.value = RG_R_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_30);//a078=10450
	    sgmii_reg_phya_30.hal.rg_sgmii_an_enable = 1;
	    RG_W_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_30,sgmii_reg_phya_30.dat.value);
		//john add^^^
    	
		sgmii_reg_an_13.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13);
		sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x03;
		RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,sgmii_reg_an_13.dat.value);
		
		//john add---
		//1fa9a010=438447
		//rg_type_t(HAL_sgmii_reg_phya_4) sgmii_reg_phya_4;
		//sgmii_reg_phya_4.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_4);	
	    //sgmii_reg_phya_4.hal.rg_ssusb_bg_div = 0b01;
	    //RG_W_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_4,sgmii_reg_phya_4.dat.value);
		
		rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	    sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	    sgmii_reg_an0.hal.sgmii_an_enable = 1;
		sgmii_reg_an0.hal.sgmii_an_restart = 1;
	    RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,sgmii_reg_an0.dat.value); 
		//john add^^^

		//john rg_hsgmii_pcs_ctrol_6.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_6);	
		//john rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_tx_enable = 1;
		//john rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_force_rateadapt = 1;
		//john rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_mode2_en = 1;
		//john rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_mac_mode = mode;
		//john RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_6,rg_hsgmii_pcs_ctrol_6.dat.value);	
			
		if (mode){
			switch (rate){
			case ENUM_SGMII_1000M :
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x1801:0x801);
				break;
			case ENUM_SGMII_100M :
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x1401:0x401);
				break;
			case ENUM_SGMII_10M :
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x1001:0x001);
				break;	
			default :			
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x1801:0x801);
				break;
			}
		}
	}
}

static void sgmii_an(u8 enable){
	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	printk("sgmii_an %x\n",enable);
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	sgmii_reg_an0.hal.sgmii_an_enable = enable;
	RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,sgmii_reg_an0.dat.value); 
} 

static void hsgmii_an(u8 enable){
	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	printk("sgmii_an %x\n",enable);
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	sgmii_reg_an0.hal.sgmii_an_enable = enable;
	RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,sgmii_reg_an0.dat.value);
}

static void hsgmii_init(void){
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_1) rg_hsgmii_pcs_ctrol_1;	
	rg_type_t(HAL_sgmii_reg_phya_11) sgmii_reg_phya_11;		
	printk("hsgmii_init start\n"); 
	sgmii_reg_phya_11.dat.value = RG_R_PL(_pxSGMII->phya_base,MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_11);	
	sgmii_reg_phya_11.hal.rg_tphy_mode = 0b11;
	sgmii_reg_phya_11.hal.rg_tphy_speed = 0b01;
	//john sgmii_reg_phya_11.hal.rg_ssusb_tx_drv_en = 0x1;
	//john sgmii_reg_phya_11.hal.rg_ssusb_tx_imp_sel=0b01001;
	//john sgmii_reg_phya_11.hal.rg_ssusb_tx_lfps = 0b1;
	RG_W_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_11,sgmii_reg_phya_11.dat.value);

	rg_hsgmii_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_1);
	rg_hsgmii_pcs_ctrol_1.hal.rg_an_enable = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_gmii_txclk_ena = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_rx_clk_ena= 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sgmii_1us_timer = 0x9c;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sd_sig_det = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sgmii_ena = 1;	
	RG_W_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_1,rg_hsgmii_pcs_ctrol_1.dat.value);
	printk("hsgmii_init exit\n"); 
}

static void sgmii_init(void){
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_1) rg_hsgmii_pcs_ctrol_1;
	rg_type_t(HAL_sgmii_reg_phya_11) sgmii_reg_phya_11;		
	printk("sgmii_init start\n");
	sgmii_reg_phya_11.dat.value = RG_R_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_11);	
	sgmii_reg_phya_11.hal.rg_tphy_mode = 0b11;
	sgmii_reg_phya_11.hal.rg_tphy_speed = 0b00;
	//john sgmii_reg_phya_11.hal.rg_ssusb_tx_drv_en = 0x1;
	//john sgmii_reg_phya_11.hal.rg_ssusb_tx_imp_sel=0b01001;
	//john sgmii_reg_phya_11.hal.rg_ssusb_tx_lfps = 0b1;
	RG_W_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_11,sgmii_reg_phya_11.dat.value);

	rg_hsgmii_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_1);
	rg_hsgmii_pcs_ctrol_1.hal.rg_an_enable = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_gmii_txclk_ena = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_rx_clk_ena= 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sgmii_1us_timer = 0x9c;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sd_sig_det = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sgmii_ena = 1;	
	RG_W_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_1,rg_hsgmii_pcs_ctrol_1.dat.value);
	
	printk("sgmii_init exit\n");
}
static void xsgmii_init(u8 xsgmii){
	printk("xsgmii_init\n");
	xSGMII_QPHY_PMA(xsgmii);
	switch (xsgmii){
		case 1:
			hsgmii_init();
			break;
		case 2:
			sgmii_init();
			break;
		default:
			sgmii_init();
			break;
	}
}

static void HGMII_2p5G(void){
#if 0 //move to hsgmii_init	
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_1) rg_hsgmii_pcs_ctrol_1;	
	RGDATA_t rg={0};	
	printk("HGMII_2.5G\n");
	rg_hsgmii_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_1);
	rg_hsgmii_pcs_ctrol_1.hal.rg_an_enable = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_gmii_txclk_ena = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_rx_clk_ena= 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sgmii_1us_timer = 0x9c;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sd_sig_det = 1;
	rg_hsgmii_pcs_ctrol_1.hal.rg_sgmii_ena = 1;	
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_1,rg_hsgmii_pcs_ctrol_1.dat.value);
#endif

	rg_type_t(HAL_rg_rate_adapt_ctrl_11) ra_ctl_11;
	RGDATA_t rg={0};	
	printk("HSGMII_2.5G\n");//john printk("SGMII_100M\n");
			
	rg.value = RG_R_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 0;
	rg.bit.b4 = 0;	
	rg.bit.b26 = 1; 
	rg.bit.b27 = 1; 
	RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
		
	ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 0;
	ra_ctl_11.hal.rg_force_rate_adapt_mode = 0;//john 0 <=7
	RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);
    //john add---phy_reset
	rg_type_t(HAL_sgmii_reg_an0) sgmii_reg_an0;	
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	sgmii_reg_an0.hal.sgmii_reset_phy = 1;
	RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,sgmii_reg_an0.dat.value);
    //john add^^^
	
	//YT Mark, this is for debug
	//rg.value = RG_R_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	//rg.bit.b1 = 0;
	//RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,rg.value);
	printk("HSGMII_2.5 exit\n");
}

static void SGMII_1G(void){
	
	rg_type_t(HAL_rg_rate_adapt_ctrl_11) ra_ctl_11;
	RGDATA_t rg={0};	
	printk("SGMII_1G\n");
			
	rg.value = RG_R_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 0;
	rg.bit.b4 = 0;	
	rg.bit.b26 = 1; 
	rg.bit.b27 = 1; 
	RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
		
	//john ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	//john ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 0;
	//john ra_ctl_11.hal.rg_force_rate_adapt_mode = 7;
	//john RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);

	//john add---
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 1;
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0x0;
	RG_W_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force,rg_an_sgmii_mode_force.dat.value);

	rg_type_t(HAL_sgmii_reg_phya_30) sgmii_reg_phya_30;		
	sgmii_reg_phya_30.dat.value = RG_R_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_30);	
	sgmii_reg_phya_30.hal.rg_sgmii_an_enable = 1;
	RG_W_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_30,sgmii_reg_phya_30.dat.value);
	
	RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,((RgAddr)_rg_rate_adapt_ctrl_0+0x100),0);//6100=0
	
	rg_type_t(HAL_sgmii_reg_an_13) sgmii_reg_an_13;
	sgmii_reg_an_13.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13);
	sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x03;
	RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,sgmii_reg_an_13.dat.value);	//ref RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,0x31120003);
	//john add^^^
}
static void SGMII_100M(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_11) ra_ctl_11;
	RGDATA_t rg={0};	
	printk("SGMII_100M\n");

	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 1;
	rg.bit.b4 = 1;	
	rg.bit.b26 = 0; 
	rg.bit.b27 = 0; 
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
		
	//john ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	//john ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 1;
	//john ra_ctl_11.hal.rg_force_rate_adapt_mode = 7;
	//john RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);
	
	printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
	printk("_rg_rate_adapt_ctrl_0 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0));
	
	//john add---
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 1;
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0x1;
	RG_W_PL(_pxSGMII->pcs2_base,SGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force,rg_an_sgmii_mode_force.dat.value);

	rg_type_t(HAL_sgmii_reg_phya_30) sgmii_reg_phya_30;		
	sgmii_reg_phya_30.dat.value = RG_R_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_30);	
	sgmii_reg_phya_30.hal.rg_sgmii_an_enable = 1;
	RG_W_PL(_pxSGMII->phya_base,SGMII_MULTI_PHY_ANA_BASE_OFFSET,(RgAddr)_sgmii_reg_phya_30,sgmii_reg_phya_30.dat.value);
	
	RG_W_PL(_pxSGMII->ra_base,SGMII_RA_BASE_OFFSET,((RgAddr)_rg_rate_adapt_ctrl_0+0x100),0);//6100=0

	rg_type_t(HAL_sgmii_reg_an_13) sgmii_reg_an_13;
	sgmii_reg_an_13.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13);
	sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x05;
	RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,sgmii_reg_an_13.dat.value);	//ref RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,0x31120003);
	//john add^^^
	
}

#ifdef SUPPORT_10M
static void SGMII_10M(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_11)ra_ctl_11;
	RGDATA_t rg={0};	
	printk("SGMII_10M\n");
			
	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 1;
	rg.bit.b4 = 1;	
	rg.bit.b26 = 0; 
	rg.bit.b27 = 0; 
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
		
	ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 1;
	ra_ctl_11.hal.rg_force_rate_adapt_mode = 10;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);
	
	printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
	printk("_rg_rate_adapt_ctrl_0 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0));
	printk("_rg_rate_adapt_ctrl_11 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11));
	printk("SGMII_10M exit\n");
}
#endif 


static void sgmii_rate_api(u8 rate){
	printk("sgmii_rate_api rate %x\n",rate);
	switch (rate){
		case ENUM_SGMII_1000M :
			SGMII_1G();
			break;
		case ENUM_SGMII_100M :
			SGMII_100M();
			break;
#ifdef SUPPORT_10M			
		case ENUM_SGMII_10M :
			SGMII_10M();
			break;
#endif			
		default :			
			SGMII_1G();
			break;			
	}
}	

static void hsgmii_rate_api(void){
	printk("hsgmii\n");
	HGMII_2p5G();
}

static void xSGMII_Rate_API(u8 xsgmii,u8 rate){
	switch (xsgmii){
		case HSGMII:
			hsgmii_rate_api();
			break;
		case SGMII:
			sgmii_rate_api(rate);
			break;
		default :
			hsgmii_rate_api();	
			break;			
	}
}

static void xSGMII_AN_API(u8 xsgmii,u8 an){
	printk("xSGMII_AN_API %x, enable : %x\n",xsgmii,an);
	switch (xsgmii){		
		case HSGMII :
			hsgmii_an(0);
			break;
		case SGMII :
			sgmii_an(an);
			break;
		default :
			hsgmii_an(0);
                        break;	
	}	
}
static u32 xSGMII_Link_STS(u8 xsgmii){
	RGDATA_t rg = {0x00},sts ={0x00};	
	switch (xsgmii){
		case USXGMII :
			rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_base_r_10gb_t_pcs_stus1);
			sts.bit.b0 = rg.bit.b12;//sync
			rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_usxgmii_an_stats_2);
			sts.bit.b1 = rg.bit.b24;//an
			break;
		case HSGMII :
		case SGMII :
			rg.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_state_2);
			sts.bit.b0 = rg.bit.b5;//sync
			sts.bit.b1 = rg.bit.b0;//an
			break;
		default :			
			printk("cmd error!!!\n");
			break;	
	}
	return sts.value;
}	
#ifdef Enable_IRQ
static irqreturn_t xsgmii_interrupt(int irq, void *dev_id){
	rg_type_t(HAL_rg_hsgmii_mode_interrupt) rg_hsgmii_mode_interrupt;
	rg_type_t(HAL_rg_hsgmii_pcs_int_state) rg_hsgmii_pcs_int_state;
	rg_type_t(HAL_rg_rate_adapt_ctrl_1) rg_rate_adapt_ctrl_1;
	
	if(dbg_print) {
		printk("usb xsgmii: an_int called\n");
		printk("usb xsgmii: irq=%d\n",irq);
	}	

	rg_hsgmii_pcs_int_state.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_int_state);	
	if (rg_hsgmii_pcs_int_state.hal.ro_hsgmii_mode2_an_done) 
	{
		;
	}
	
	rg_hsgmii_mode_interrupt.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt);
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int				  =0x1;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int			  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int		  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int 				  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int_clear			  =0x1;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int_clear 	  =0x1;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x1;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int_clear			  =0x1;
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt,rg_hsgmii_mode_interrupt.dat.value);
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int				  =0x1;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int			  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int		  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int 				  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int_clear			  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int_clear 	  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int_clear			  =0x0;
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt,rg_hsgmii_mode_interrupt.dat.value);
	return IRQ_HANDLED;
}

static void xsgmii_interrupt_init(u8 disable){
	rg_type_t(HAL_rg_hsgmii_mode_interrupt) rg_hsgmii_mode_interrupt;		
	rg_type_t(HAL_INTERRUPT_EN_0) rg_INTERRUPT_EN_0;
	rg_type_t(HAL_multi_sgmii_reg_interrupt_sel) rg_multi_sgmii_reg_interrupt_sel;

	if(dbg_print)printk("xsgmii: int_init called and disable = %x\n",disable);
	if(_pxSGMII->irq < 0){
		printk("ETH IRQ NG\n");
		return _pxSGMII->irq;
	}	
	rg_hsgmii_mode_interrupt.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt);
	rg_INTERRUPT_EN_0.dat.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) INTERRUPT_EN_0);
	rg_multi_sgmii_reg_interrupt_sel.dat.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) multi_sgmii_reg_interrupt_sel);	

	if(disable){
		rg_INTERRUPT_EN_0.hal.rg_pcs_int_en_0 = 0;		
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) INTERRUPT_EN_0, rg_INTERRUPT_EN_0.dat.value);
		return;
	}
	else{
		rg_INTERRUPT_EN_0.hal.rg_pcs_int_en_0 = 1;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) INTERRUPT_EN_0, rg_INTERRUPT_EN_0.dat.value);
	}
	
	if((XSGMII_REQ_IRQ == 0)){		
		XSGMII_REQ_IRQ = 1;
		if(request_irq(_pxSGMII->irq, xsgmii_interrupt, 0, "eth_usxgmii", _pxSGMII->dev)){
			printk("\n request_irq() (irq number: %d) NG\n", _pxSGMII->irq);
			XSGMII_REQ_IRQ = 0;
			return _pxSGMII->irq;
		}
		else if(dbg_print) printk("\n request_irq() (irq number: %d) OK \n", _pxSGMII->irq);
	
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int				   =0x1;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int			   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int 	   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int			   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int_clear 		   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int_clear	   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int_clear	   =0x0;
		RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt,rg_hsgmii_mode_interrupt.dat.value);


		rg_multi_sgmii_reg_interrupt_sel.hal.rg_hsgmii_pcs_int = 1;
		//rg_multi_sgmii_reg_interrupt_sel.hal.rg_hsgmii_phy_int = 1;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) multi_sgmii_reg_interrupt_sel, rg_multi_sgmii_reg_interrupt_sel.dat.value);
	}
	rg_INTERRUPT_EN_0.dat.value = 0;
	rg_INTERRUPT_EN_0.dat.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) INTERRUPT_EN_0);
	if(dbg_print) {
		printk("xsgmii: sync_int exit %x\n",XSGMII_REQ_IRQ);
	}	
}
#endif

u32 USB_xSGMII_API(u8 xsgmii,u8 mod,u8 an, u8 rate){		
	u32 data=0;
	printk("USB_xSGMII_API xsgmii=%x,mod = %x,an=%x,rate=%x\n",xsgmii,mod,an,rate);
	printk("---221213---\n");//john temp add
#ifndef FPGA_PLATFORM
    if ( mod < 2 )    
	{
	   xsgmii_chg(xsgmii);
       printk("---xsgmii_chg---\n");//john temp add
	}	
#endif	
    if ( mod < 2 )    
	{
	   xsgmii_init(xsgmii);
	   xsgmii_interrupt_init(codition_int_dis);
	   printk("---230302 init OK ---\n");//john temp add
	}
	switch(mod){
		case 0: 
			printk("force mode\n");
			xSGMII_AN_API(xsgmii,an);
			xSGMII_Rate_API(xsgmii,rate);
			break;
		case 1:
			printk("AN mode\n");
			xSGMII_AN_API(xsgmii,1);
			xSGMII_AN_AutoSetting(xsgmii,0,1,rate); // MAC mode
			break;
		case 2:
			printk("xsgmii status\n");
			data = xSGMII_Link_STS(xsgmii);
			break;
		default:
			printk("force mode\n");
			data = 0xff;
			xSGMII_AN_API(xsgmii,an);
			xSGMII_Rate_API(xsgmii,rate);
			break;
	}	
	return data;
}
EXPORT_SYMBOL(USB_xSGMII_API);


/* APIs */
static void xsgmii_drv_init(void)
{
#ifdef FPGA_PLATFORM	
		return;
#endif
	printk("xsgmii_init: start\n");
	//xsgmii_int_init();
	printk("eth_usxgmii_fpga\n");
	printk("usxgmii: sgmii_serdes_sel: exit!\n");
}


#ifdef UNUSED_FUN
static u8 usxgmii_link_pattner_ability(void){
	rg_type_t(HAL_ro_usxgmii_an_stats_0)rgHAL_ro_usxgmii_an_stats_0;
	rgHAL_ro_usxgmii_an_stats_0.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6);
	printk("PL speed %x\n",rgHAL_ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode);
	return (u8)rgHAL_ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode;
}

static u8 usxgmii_linkup(void){
	u8 sts = 0;
	rg_type_t(HAL_xfi_pcs_int_sta_3) xfi_pcs_int_sta_3;
	xfi_pcs_int_sta_3.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_3);
	if (xfi_pcs_int_sta_3.hal.link_up_st_int){
		sts = (u8) xfi_pcs_int_sta_3.hal.link_up_st_int;
		xfi_pcs_int_sta_3.hal.link_up_st_int = 1;
		printk("xfi_pcs_int_sta_3 %x\n",xfi_pcs_int_sta_3.dat.value);
	}
	return sts;
}

static u8 usxgmii_an_done(void){
	u8 sts = 0;
	rg_type_t(HAL_xfi_pcs_int_sta_3) xfi_pcs_int_sta_3;
	xfi_pcs_int_sta_3.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_3);
	if (xfi_pcs_int_sta_3.hal.link_up_st_int){
		sts = (u8) xfi_pcs_int_sta_3.hal.link_up_st_int;
		xfi_pcs_int_sta_3.hal.link_up_st_int = 1;
		printk("xfi_pcs_int_sta_3 %x\n",xfi_pcs_int_sta_3.dat.value);
	}
	return sts;
}	
#endif


static int xsgmii_drv_probe(struct platform_device *pdev)
{
    const char *int_name;
    struct resource *res = NULL;
    int ret,int_id;
	xsgmii_base * _pxSGMII_Tmp = NULL;

	
	printk("USB xsgmii_drv_probe\n");
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No eth_usxgmii DT node found");
        return -EINVAL;
    }

    _pxSGMII_Tmp = devm_kzalloc(&pdev->dev, sizeof(xsgmii_base), GFP_KERNEL);
    if (!_pxSGMII_Tmp)
        return -ENOMEM;

    platform_set_drvdata(pdev, _pxSGMII_Tmp);
	_pxSGMII = _pxSGMII_Tmp;
	

	/* get pcs2 base address */	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
   	_pxSGMII->pcs2_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(_pxSGMII->pcs2_base)){
	   printk("pcs2_base ng\n");	
	   return PTR_ERR(_pxSGMII->pcs2_base);
   	}

	/* get AN base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
   	_pxSGMII->an_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(_pxSGMII->an_base))
	   return PTR_ERR(_pxSGMII->an_base);

	/* get RA base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
   	_pxSGMII->ra_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(_pxSGMII->ra_base))
	   return PTR_ERR(_pxSGMII->ra_base);

	/* get phya base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
	_pxSGMII->phya_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(_pxSGMII->phya_base))
		return PTR_ERR(_pxSGMII->phya_base);

	/* Multi SGMII */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 4);
	_pxSGMII->multi_sgmii_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(_pxSGMII->multi_sgmii_base))
		return PTR_ERR(_pxSGMII->multi_sgmii_base);

	//int_name
	ret = of_property_read_string(pdev->dev.of_node, "int_name", &int_name);
	if(ret < 0)	printk("sgmii get propert error!");
	else printk("sgmii:int_name: %s\n",int_name);

	//int_id
	ret = of_property_read_u32(pdev->dev.of_node, "int_id", &int_id);
	if(ret<0) printk("sgmii get propert error!\n");
	else printk("sgmii:int_id: %d\n",int_id);
	

	
#ifdef Enable_IRQ
	/* get irq num */
		_pxSGMII->irq = platform_get_irq(pdev, 0);
		if(_pxSGMII->irq <= 0) {
			printk("\n get eth_xsgmii irq number failed\n");
			return _pxSGMII->irq;
		}else printk("irq = %x\n",_pxSGMII->irq);
		//int_name
#endif


	#if 1
	//printk("usb_xsgmii->pcs1_base= %lx\n", (unsigned long)_pxSGMII->pcs1_base);
	printk("usb_xsgmii->pcs2_base= %pS\n", _pxSGMII->pcs2_base);
	printk("usb_xsgmii->an_base=   %pS\n", _pxSGMII->an_base);
	printk("usb_xsgmii->ra_base=   %pS\n", _pxSGMII->ra_base);
	printk("usb_xsgmii->phya_base=   %pS\n", _pxSGMII->phya_base);
	printk("usb_xsgmii->multi_sgmii_base=   %pS\n", _pxSGMII->multi_sgmii_base);
	#endif


	if(_pxSGMII == NULL)printk("USB xsgmii drv probe ng\n");
	else xsgmii_drv_init();

	return 0;
}
static int xsgmii_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver DRV = {
    .probe = xsgmii_drv_probe,
    .remove = xsgmii_drv_remove,
    .driver = {
	    .name = NAME,
	    .of_match_table = xsgmii_table
    },
};
module_platform_driver(DRV);
MODULE_DESCRIPTION(DESCRIPTION);

