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
#include <uapi/ecnt_event_global/ecnt_event_serdes.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <asm/delay.h>

#include "H/air_xsgmii.h"  
#include "H/air_xpon_xsgmii_config.h"

#define RX_EYE_SCAN	0
#define PMA_BASE _pxSGMII->pma_base
#define PHYA_BASE _pxSGMII->phya_base
#define PCS2_BASE _pxSGMII->pcs2_base
#define AN_BASE _pxSGMII->an_base
#define MULTI_SGMII_BASE _pxSGMII->multi_sgmii_base
#define RA_BASE _pxSGMII->ra_base

/************************************************************************
*                  S T A T I C   V A R I A B L E   D E C L A R A T I O N S
*************************************************************************
*/

static volatile xsgmii_base * _pxSGMII = NULL;
static u8 xsgmii_bk = 0xff;
 
static const struct of_device_id xsgmii_table[] = {
    { .compatible = COMPATIBLE},
    { /* sentinel */}
};

MODULE_DEVICE_TABLE(of, xsgmii_table);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern 	void __iomem* Get_Base(u32 base);

extern int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel);

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

void XPON_xSGMII_Ver(void){
	printk("XPON XSGMII Version = %s\n",VER);
}
EXPORT_SYMBOL(XPON_xSGMII_Ver);

static void RG_W(REG_t* rg){	
	writel(rg->data.value, rg->base + (rg->addr - rg->base_start));
	if(dbg_print){
		rg->data.value = readl(rg->base + (rg->addr - rg->base_start));
		if (rg->base == _pxSGMII->pcs1_base)		printk("\tmemwl %8x %x\n",(0x1fa85000+rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->pcs2_base)		printk("\tmemwl %8x %x\n",(0x1fa80a00-0xa00+rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->an_base)			printk("\tmemwl %8x %x\n",(0x1fa80000+rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->ra_base)			printk("\tmemwl %8x %x\n",(0x1fa86000+rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->multi_sgmii_base)	printk("\tmemwl %8x %x\n",(0x1fa84000+rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->phya_base)		printk("\tmemwl %8x %x\n",(0x1faf3000-0x3000+rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->pma_base)			printk("\tmemwl %8x %x\n",(0x1faf4000-0x4000+rg->addr),rg->data.value);	
	}
}

static u32 RG_R(REG_t* rg){
	u32 value;
	value = readl(rg->base + (rg->addr - rg->base_start));
	if(dbg_print){
		rg->data.value = readl(rg->base + (rg->addr - rg->base_start));
		if (rg->base == _pxSGMII->pcs1_base)		printk("\tmemrl %8x [readback: 0x%x]\n",(0x1fa85000+rg->addr),value);
		if (rg->base == _pxSGMII->pcs2_base)		printk("\tmemrl %8x [readback: 0x%x]\n",(0x1fa80a00-0xa00+rg->addr),value);
		if (rg->base == _pxSGMII->an_base)			printk("\tmemrl %8x [readback: 0x%x]\n",(0x1fa80000+rg->addr),value);
		if (rg->base == _pxSGMII->ra_base)			printk("\tmemrl %8x [readback: 0x%x]\n",(0x1fa86000+rg->addr),value);
		if (rg->base == _pxSGMII->multi_sgmii_base)	printk("\tmemrl %8x [readback: 0x%x]\n",(0x1fa84000+rg->addr),value);
		if (rg->base == _pxSGMII->phya_base)		printk("\tmemrl %8x [readback: 0x%x]\n",(0x1faf3000-0x3000+rg->addr),value);
		if (rg->base == _pxSGMII->pma_base)			printk("\tmemrl %8x [readback: 0x%x]\n",(0x1faf4000-0x4000+rg->addr),value);	
	}
	return value;

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
	else printk("\nERROR(%s) please register xgmii!!!\n",__func__);
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
		printk("\nERROR(%s) please register xgmii!!!\n",__func__);
		return 0;
	}
}  
static u8 xsgmii_chg(u8 xsgmii){
	u8 chg = OFF,wan_cfg = GET_WAN_CONF();
	
#ifdef FPGA_PLATFORM
	if((xsgmii_bk = 0xff)||(xsgmii != xsgmii_bk)){
		chg = ON;		
		xsgmii_bk = xsgmii;
	}
#else
	//if((xsgmii_bk = 0xff)||(xsgmii != xsgmii_bk))
	{
		chg = ON;		
		xsgmii_bk = xsgmii;
		wan_cfg &=~(0xFF);				
		switch (xsgmii){
			case USXGMII :
				SCU_SetModSel(1);
				SET_WAN_CONF(wan_cfg|0x12);
				break;
			case HSGMII:
				SET_WAN_CONF(wan_cfg|0x11);
				SCU_SetModSel(2);
				break;
			case SGMII:
				SET_WAN_CONF(wan_cfg|0x10);
				SCU_SetModSel(2);
				break;
		}
		SCU_SetModRst(SCU_GetModRst() | (SCU_RST_BIT << SCU_PHY_RST_POS));		
		SCU_SetModRst(SCU_GetModRst()& ~(SCU_RST_BIT << SCU_PHY_RST_POS));
		
		SCU_SetModRst(SCU_GetModRst1() | (SCU_RST_BIT << SCU_MAC_RST_POS));		
		SCU_SetModRst(SCU_GetModRst1()& ~(SCU_RST_BIT << SCU_MAC_RST_POS));
	}
	
#endif

	if(dbg_print) printk("xsgmii_chg %x\n",chg);
	
	return chg;
}

/** NOT USED ***************************************************************************************************************** 
static void CMN_EN(u8 en){
	rg_type_t(HAL_RG_PXP_CMN_EN) RG_PXP_CMN_EN;

	RG_PXP_CMN_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CMN_EN);
	RG_PXP_CMN_EN.hal.rg_pxp_cmn_en = en;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CMN_EN,RG_PXP_CMN_EN.dat.value);
}	

static void JCPLL_LDO(u8 enable){
	rg_type_t(HAL_RG_PXP_JCPLL_SPARE_H) RG_PXP_JCPLL_SPARE_H;
	if(dbg_print) printk("JCPLL_LDO\n");
	udelay(200);
	RG_PXP_JCPLL_SPARE_H.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SPARE_H);
	RG_PXP_JCPLL_SPARE_H.hal.rg_pxp_jcpll_spare_l &= ~0x20; //turn off LDO for bg settle, enable LDO for clk working
	RG_PXP_JCPLL_SPARE_H.hal.rg_pxp_jcpll_spare_l |= (enable << 5); //turn off LDO for bg settle, enable LDO for clk working
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SPARE_H,RG_PXP_JCPLL_SPARE_H.dat.value);
}

static void JCPLL_SDM(u8 di_en,u8 ifm,u8 hren){
	rg_type_t(HAL_RG_PXP_JCPLL_RST_DLY) RG_PXP_JCPLL_RST_DLY;	
	rg_type_t(HAL_RG_PXP_JCPLL_SDM_IFM) RG_PXP_JCPLL_SDM_IFM;
	rg_type_t(HAL_RG_PXP_JCPLL_SDM_HREN) RG_PXP_JCPLL_SDM_HREN;
	if(dbg_print) printk("JCPLL_SDM\n");

	RG_PXP_JCPLL_RST_DLY.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY);
	RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_sdm_di_en = di_en;	
	RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_sdm_di_ls = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);

	RG_PXP_JCPLL_SDM_IFM.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SDM_IFM);
	RG_PXP_JCPLL_SDM_IFM.hal.rg_pxp_jcpll_sdm_ifm = ifm;	
	RG_PXP_JCPLL_SDM_IFM.hal.rg_pxp_jcpll_sdm_mode = 0x0;
	RG_PXP_JCPLL_SDM_IFM.hal.rg_pxp_jcpll_sdm_ord = 0x3;	
	RG_PXP_JCPLL_SDM_IFM.hal.rg_pxp_jcpll_sdm_out = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SDM_IFM,RG_PXP_JCPLL_SDM_IFM.dat.value);

	RG_PXP_JCPLL_SDM_HREN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SDM_HREN);
	RG_PXP_JCPLL_SDM_HREN.hal.rg_pxp_jcpll_sdm_hren = hren;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SDM_HREN,RG_PXP_JCPLL_SDM_HREN.dat.value);	
}

static void JCPLL_SSC(u8 phase_ini,u16 delta1,u16 delta,u16 period,u8 ssc_en){
	rg_type_t(HAL_RG_PXP_JCPLL_SSC_DELTA) RG_PXP_JCPLL_SSC_DELTA;	
	rg_type_t(HAL_RG_PXP_JCPLL_SSC_TRI_EN) RG_PXP_JCPLL_SSC_TRI_EN;	
	rg_type_t(HAL_RG_PXP_JCPLL_VCO_TCLVAR) RG_PXP_JCPLL_VCO_TCLVAR;
	if(dbg_print) printk("JCPLL_SSC\n");

	RG_PXP_JCPLL_SSC_DELTA.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SSC_DELTA);
	RG_PXP_JCPLL_SSC_DELTA.hal.rg_pxp_jcpll_ssc_delta = delta;
	RG_PXP_JCPLL_SSC_DELTA.hal.rg_pxp_jcpll_ssc_period = period;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SSC_DELTA,RG_PXP_JCPLL_SSC_DELTA.dat.value);

	
	RG_PXP_JCPLL_SSC_TRI_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SSC_TRI_EN);
	RG_PXP_JCPLL_SSC_TRI_EN.hal.rg_pxp_jcpll_ssc_tri_en = 0x0;	
	RG_PXP_JCPLL_SSC_TRI_EN.hal.rg_pxp_jcpll_ssc_delta1 = delta1;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SSC_TRI_EN,RG_PXP_JCPLL_SSC_TRI_EN.dat.value);

	RG_PXP_JCPLL_VCO_TCLVAR.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCO_TCLVAR);
	RG_PXP_JCPLL_VCO_TCLVAR.hal.rg_pxp_jcpll_ssc_phase_ini = phase_ini;	
	RG_PXP_JCPLL_VCO_TCLVAR.hal.rg_pxp_jcpll_vco_vcovar_bias_l = 0x0;
	RG_PXP_JCPLL_VCO_TCLVAR.hal.rg_pxp_jcpll_vco_tclvar = 0x0;	
	RG_PXP_JCPLL_VCO_TCLVAR.hal.rg_pxp_jcpll_ssc_en = ssc_en;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCO_TCLVAR,RG_PXP_JCPLL_VCO_TCLVAR.dat.value);
}


static void JCPLL_LPF(u8 ibias,u8 br,u8 bp,u8 bwr,u8 bwc){
	rg_type_t(HAL_RG_PXP_JCPLL_IB_EXT_EN) RG_PXP_JCPLL_IB_EXT_EN;	
	rg_type_t(HAL_RG_PXP_JCPLL_LPF_BR) RG_PXP_JCPLL_LPF_BR;	
	rg_type_t(HAL_RG_PXP_JCPLL_LPF_BWC) RG_PXP_JCPLL_LPF_BWC;
	if(dbg_print) printk("JCPLL_LPF\n");
	RG_PXP_JCPLL_IB_EXT_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_IB_EXT_EN);
	RG_PXP_JCPLL_IB_EXT_EN.hal.rg_pxp_jcpll_chp_ibias = ibias;
	RG_PXP_JCPLL_IB_EXT_EN.hal.rg_pxp_jcpll_chp_iofst = 0x00;
	RG_PXP_JCPLL_IB_EXT_EN.hal.rg_pxp_jcpll_lpf_shck_en = 0x00;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_IB_EXT_EN,RG_PXP_JCPLL_IB_EXT_EN.dat.value);

	
	RG_PXP_JCPLL_LPF_BR.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_LPF_BR);
	RG_PXP_JCPLL_LPF_BR.hal.rg_pxp_jcpll_lpf_br = br;	
	RG_PXP_JCPLL_LPF_BR.hal.rg_pxp_jcpll_lpf_bc = 0x1F;
	RG_PXP_JCPLL_LPF_BR.hal.rg_pxp_jcpll_lpf_bp = bp;
	RG_PXP_JCPLL_LPF_BR.hal.rg_pxp_jcpll_lpf_bwr = bwr;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_LPF_BR,RG_PXP_JCPLL_LPF_BR.dat.value);

	RG_PXP_JCPLL_LPF_BWC.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_LPF_BWC);
	RG_PXP_JCPLL_LPF_BWC.hal.rg_pxp_jcpll_lpf_bwc = bwc;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_LPF_BWC,RG_PXP_JCPLL_LPF_BWC.dat.value);
}

static void JCPLL_VCO(void){
	rg_type_t(HAL_RG_PXP_JCPLL_VCODIV) RG_PXP_JCPLL_VCODIV;	
	rg_type_t(HAL_RG_PXP_JCPLL_VCO_TCLVAR) RG_PXP_JCPLL_VCO_TCLVAR;	
	if(dbg_print) printk("JCPLL_VCO\n");
	RG_PXP_JCPLL_VCODIV.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCODIV);
	RG_PXP_JCPLL_VCODIV.hal.rg_pxp_jcpll_vco_cfix = 0x1;
	RG_PXP_JCPLL_VCODIV.hal.rg_pxp_jcpll_vco_scapwr = 0x04;
	RG_PXP_JCPLL_VCODIV.hal.rg_pxp_jcpll_vco_halflsb_en = 0x01;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCODIV,RG_PXP_JCPLL_VCODIV.dat.value);

	
	RG_PXP_JCPLL_VCO_TCLVAR.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCO_TCLVAR);
	RG_PXP_JCPLL_VCO_TCLVAR.hal.rg_pxp_jcpll_vco_vcovar_bias_h = 0x3;	
	RG_PXP_JCPLL_VCO_TCLVAR.hal.rg_pxp_jcpll_vco_vcovar_bias_l = 0x0;
	RG_PXP_JCPLL_VCO_TCLVAR.hal.rg_pxp_jcpll_vco_tclvar = 0x5;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCO_TCLVAR,RG_PXP_JCPLL_VCO_TCLVAR.dat.value);
}

static void JCPLL_PCW(u32 pcw){	
	rg_type_t(HAL_rg_force_da_pxp_jcpll_sdm_pcw) rg_force_da_pxp_jcpll_sdm_pcw;		
	rg_type_t(HAL_rg_force_da_pxp_rx_fe_vos) rg_force_da_pxp_rx_fe_vos;	
	if(dbg_print) printk("JCPLL_PCW\n");
	rg_force_da_pxp_jcpll_sdm_pcw.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_jcpll_sdm_pcw);
	rg_force_da_pxp_jcpll_sdm_pcw.hal.rg_force_da_pxp_jcpll_sdm_pcw = pcw;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_jcpll_sdm_pcw,rg_force_da_pxp_jcpll_sdm_pcw.dat.value);

	rg_force_da_pxp_rx_fe_vos.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_vos);
	rg_force_da_pxp_rx_fe_vos.hal.rg_force_sel_da_pxp_jcpll_sdm_pcw = 0x01;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_vos,rg_force_da_pxp_rx_fe_vos.dat.value);
}

static void JCPLL_RSTB(u8 action){
	rg_type_t(HAL_RG_PXP_JCPLL_RST_DLY) RG_PXP_JCPLL_RST_DLY;	
	if(dbg_print) printk("JCPLL_RSTB\n");
	RG_PXP_JCPLL_RST_DLY.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY);
	RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_rst_dly = 0x04; //YT change 4
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);
	switch(action){
		case 0: //toggle			
			RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_pll_rstb = 0x0; //aways high
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);
			//YT add
			udelay(200);
			RG_PXP_JCPLL_RST_DLY.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY);
			RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_pll_rstb = 0x1; //aways high
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);
			break;
		case 1: //once
			if(!RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_pll_rstb){
				RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_pll_rstb = 0x1; //aways high
				RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);
			}	
			
			break;
		default:
			RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_pll_rstb = 0x0; //aways high
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);
			
			//YT add
			udelay(200);
			RG_PXP_JCPLL_RST_DLY.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY);
			RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_pll_rstb = 0x1; //aways high
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);
			break;
	}	
}


static void JCPLL_DIV(u8 prediv_mode,u8 vcodiv,u8 postdiv_d5){
	rg_type_t(HAL_RG_PXP_JCPLL_MMD_PREDIV_MODE) RG_PXP_JCPLL_MMD_PREDIV_MODE;
	rg_type_t(HAL_RG_PXP_JCPLL_VCODIV) RG_PXP_JCPLL_VCODIV;
	if(dbg_print) printk("JCPLL_DIV\n");
	RG_PXP_JCPLL_MMD_PREDIV_MODE.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_MMD_PREDIV_MODE);
	RG_PXP_JCPLL_MMD_PREDIV_MODE.hal.rg_pxp_jcpll_mmd_prediv_mode = prediv_mode;	
	//YT add default value 0
	RG_PXP_JCPLL_MMD_PREDIV_MODE.hal.rg_pxp_jcpll_postdiv_d5 = postdiv_d5;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_MMD_PREDIV_MODE,RG_PXP_JCPLL_MMD_PREDIV_MODE.dat.value);

	RG_PXP_JCPLL_VCODIV.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCODIV);
	RG_PXP_JCPLL_VCODIV.hal.rg_pxp_jcpll_vcodiv = vcodiv;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCODIV,RG_PXP_JCPLL_VCODIV.dat.value);
}

static void JCPLL_KBand(void){
	//rg_type_t(HAL_RG_PXP_JCPLL_RST_DLY) RG_PXP_JCPLL_RST_DLY;
	rg_type_t(HAL_RG_PXP_JCPLL_KBAND_KFC) RG_PXP_JCPLL_KBAND_KFC;
	rg_type_t(HAL_RG_PXP_JCPLL_LPF_BWC) RG_PXP_JCPLL_LPF_BWC;
	//rg_type_t(HAL_RG_PXP_JCPLL_MMD_PREDIV_MODE) RG_PXP_JCPLL_MMD_PREDIV_MODE;
	//rg_type_t(HAL_RG_PXP_JCPLL_VCODIV) RG_PXP_JCPLL_VCODIV;
	if(dbg_print) printk("JCPLL_KBand\n");
	//move to JCPLL_RST
	//RG_PXP_JCPLL_RST_DLY.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY);
	//RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_pll_rstb = 0x1; //aways high
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);

	RG_PXP_JCPLL_KBAND_KFC.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_KBAND_KFC);
	RG_PXP_JCPLL_KBAND_KFC.hal.rg_pxp_jcpll_kband_kfc = 0x0;
	RG_PXP_JCPLL_KBAND_KFC.hal.rg_pxp_jcpll_kband_kf = 0x3;
	RG_PXP_JCPLL_KBAND_KFC.hal.rg_pxp_jcpll_kband_ks = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_KBAND_KFC,RG_PXP_JCPLL_KBAND_KFC.dat.value);

	RG_PXP_JCPLL_LPF_BWC.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_LPF_BWC);
	RG_PXP_JCPLL_LPF_BWC.hal.rg_pxp_jcpll_kband_div = 0x2;	
	RG_PXP_JCPLL_LPF_BWC.hal.rg_pxp_jcpll_kband_code = 0xE4; //it's randon value from DE 
	RG_PXP_JCPLL_LPF_BWC.hal.rg_pxp_jcpll_kband_option = 0x0; 
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_LPF_BWC,RG_PXP_JCPLL_LPF_BWC.dat.value);

	//move to JCPLL_DIV
	//RG_PXP_JCPLL_MMD_PREDIV_MODE.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_MMD_PREDIV_MODE);
	//RG_PXP_JCPLL_MMD_PREDIV_MODE.hal.rg_pxp_jcpll_mmd_prediv_mode = 0x0;	
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_MMD_PREDIV_MODE,RG_PXP_JCPLL_MMD_PREDIV_MODE.dat.value);

	//move to JCPLL_DIV
	//RG_PXP_JCPLL_VCODIV.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCODIV);
	//RG_PXP_JCPLL_VCODIV.hal.rg_pxp_jcpll_vcodiv = 0x0;
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_VCODIV,RG_PXP_JCPLL_VCODIV.dat.value);
}

static void JCPLL_TCL(u8 kband_vref,u8 amp_gain,u8 amp_vref,u8 lpf_bw,u8 amp_en,u8 lpf_en){
	rg_type_t(HAL_RG_PXP_JCPLL_SPARE_H) RG_PXP_JCPLL_SPARE_H;
	rg_type_t(HAL_RG_PXP_JCPLL_SDM_HREN) RG_PXP_JCPLL_SDM_HREN;
	rg_type_t(HAL_RG_PXP_JCPLL_TCL_CMP_EN) RG_PXP_JCPLL_TCL_CMP_EN;
	if(dbg_print) printk("JCPLL_TCL\n");
	RG_PXP_JCPLL_SPARE_H.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SPARE_H);
	RG_PXP_JCPLL_SPARE_H.hal.rg_pxp_jcpll_tcl_kband_vref = kband_vref;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SPARE_H,RG_PXP_JCPLL_SPARE_H.dat.value);

	RG_PXP_JCPLL_SDM_HREN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SDM_HREN);
	RG_PXP_JCPLL_SDM_HREN.hal.rg_pxp_jcpll_tcl_amp_gain = amp_gain;
	RG_PXP_JCPLL_SDM_HREN.hal.rg_pxp_jcpll_tcl_amp_vref = amp_vref;
	RG_PXP_JCPLL_SDM_HREN.hal.rg_pxp_jcpll_tcl_amp_en = amp_en;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_SDM_HREN,RG_PXP_JCPLL_SDM_HREN.dat.value);

	//comp_en disable, keep default value
	RG_PXP_JCPLL_TCL_CMP_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_TCL_CMP_EN);
	RG_PXP_JCPLL_TCL_CMP_EN.hal.rg_pxp_jcpll_tcl_lpf_bw = lpf_bw;	
	RG_PXP_JCPLL_TCL_CMP_EN.hal.rg_pxp_jcpll_tcl_lpf_en = lpf_en;
	//RG_PXP_JCPLL_TCL_CMP_EN.hal.rg_pxp_jcpll_tcl_cmp_vth = 0x0;
	//RG_PXP_JCPLL_TCL_CMP_EN.hal.rg_pxp_jcpll_tcl_cmp_en = cmp_en;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_TCL_CMP_EN,RG_PXP_JCPLL_TCL_CMP_EN.dat.value);
}
//static void JCPLL_KBand_EN(void){ // no use
	//rg_type_t(HAL_scan_mode) scan_mode;
	//scan_mode.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _scan_mode);
	//scan_mode.hal.rg_force_sel_da_pxp_jcpll_kband_load_en = 0x1;
	//scan_mode.hal.rg_force_da_pxp_jcpll_kband_load_en = 0x0;
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _scan_mode,scan_mode.dat.value);
//}

static void JCPLL_EN(u8 enable){
	rg_type_t(HAL_rg_force_da_pxp_jcpll_ckout_en) rg_force_da_pxp_jcpll_ckout_en;
	if(dbg_print) printk("JCPLL_EN\n");
	rg_force_da_pxp_jcpll_ckout_en.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_jcpll_ckout_en);
	rg_force_da_pxp_jcpll_ckout_en.hal.rg_force_sel_da_pxp_jcpll_en = 1;
	rg_force_da_pxp_jcpll_ckout_en.hal.rg_force_da_pxp_jcpll_en = enable;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_jcpll_ckout_en,rg_force_da_pxp_jcpll_ckout_en.dat.value);
}
static void JCPLL_Out(u8 enable){
	rg_type_t(HAL_rg_force_da_pxp_jcpll_ckout_en) rg_force_da_pxp_jcpll_ckout_en;
	
	if(dbg_print) printk("JCPLL_Out\n");
	//rg_type_t(HAL_RG_PXP_JCPLL_FREQ_MEAS_EN) RG_PXP_JCPLL_FREQ_MEAS_EN;
	//rg_type_t(HAL_RG_PXP_JCPLL_RST_DLY) RG_PXP_JCPLL_RST_DLY;

	rg_force_da_pxp_jcpll_ckout_en.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_jcpll_ckout_en);
	//move to JCPLL_EN()
	//rg_force_da_pxp_jcpll_ckout_en.hal.rg_force_sel_da_pxp_jcpll_en = 1;
	//rg_force_da_pxp_jcpll_ckout_en.hal.rg_force_da_pxp_jcpll_en = 1;
	rg_force_da_pxp_jcpll_ckout_en.hal.rg_force_sel_da_pxp_jcpll_ckout_en = 1;
	rg_force_da_pxp_jcpll_ckout_en.hal.rg_force_da_pxp_jcpll_ckout_en = enable;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_jcpll_ckout_en,rg_force_da_pxp_jcpll_ckout_en.dat.value);

	//no need
	//RG_PXP_JCPLL_FREQ_MEAS_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_FREQ_MEAS_EN);
	//RG_PXP_JCPLL_FREQ_MEAS_EN.hal.rg_pxp_jcpll_vco_kband_meas_en = 0x0;//don't setS
	//RG_PXP_JCPLL_FREQ_MEAS_EN.hal.rg_pxp_jcpll_freq_meas_en = 0x1;//pll lock ok if meas pll freq 
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_FREQ_MEAS_EN,RG_PXP_JCPLL_FREQ_MEAS_EN.dat.value);

	//config in JCPLL_RST()
	//RG_PXP_JCPLL_RST_DLY.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY);
	//RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_sdm_di_ls = 0x0;
	//RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_sdm_di_en = 0x0;
	//RG_PXP_JCPLL_RST_DLY.hal.rg_pxp_jcpll_rst_dly = 0x2;
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_RST_DLY,RG_PXP_JCPLL_RST_DLY.dat.value);
}
static void JCPLL_BringUp(u8 xsgmii){
	if(dbg_print) printk("JCPLL BringUp %x\n",xsgmii);
	JCPLL_LDO(1); //LDO follow pllen 
	JCPLL_RSTB(1); //just set high & always keep high
	JCPLL_EN(OFF);
	switch(xsgmii){
		case USXGMII:			
			JCPLL_SDM(0,0,0);
			JCPLL_SSC(0,0,0,0,0);
			JCPLL_LPF(0x18,0b01010,0x10,0x00,0x00);
			JCPLL_VCO();
			JCPLL_PCW(0x25800000);
			JCPLL_DIV(0,0,0);
			JCPLL_KBand();
			JCPLL_TCL(0xf,0x01,0b0101,0,1,1);

			JCPLL_EN(1);
			JCPLL_Out(1);
			break;
		case HSGMII:
			JCPLL_SDM(0,0,0);
			JCPLL_SSC(0,0,0,0,0);
			JCPLL_LPF(0x18,0b01010,0x10,0x00,0x00);
			JCPLL_VCO();
			JCPLL_PCW(0x25800000);
			JCPLL_DIV(0,0,0);
			JCPLL_KBand();
			JCPLL_TCL(0x10,0x01,0b0101,0,1,1);

			JCPLL_EN(1);
			JCPLL_Out(1);
			break;
		case SGMII://10.0.9
			JCPLL_SDM(0,0,0);
			JCPLL_SSC(0,0,0,0,0);
			JCPLL_LPF(0x18,0b01010,0x10,0x00,0x00);
			JCPLL_VCO();
			JCPLL_PCW(0x25800000);
			JCPLL_DIV(0,0,0);
			JCPLL_KBand();
			JCPLL_TCL(0x10,0x01,0b0101,0,1,1);
			
			JCPLL_EN(1);
			JCPLL_Out(1);
			break;
		default: //USXGMII
			JCPLL_SDM(0,0,0);
			JCPLL_SSC(0,0,0,0,0);
			JCPLL_LPF(0x18,0b01010,0x10,0x00,0x00);
			JCPLL_VCO();
			JCPLL_PCW(0x25800000);
			JCPLL_DIV(0,0,0);
			JCPLL_KBand();
			JCPLL_TCL(0xf,0x01,0b0101,0,1,1);

			JCPLL_EN(1);
			JCPLL_Out(1);
			printk("Waring!!! JCPLL xSGMII SEL NG\n");
			break;
	}
	
}
static void TXPLL_SDM(u8 di_en,u8 ifm,u8 hren){
	rg_type_t(HAL_RG_PXP_TXPLL_SDM_DI_EN) RG_PXP_TXPLL_SDM_DI_EN;
	rg_type_t(HAL_RG_PXP_TXPLL_SDM_ORD) RG_PXP_TXPLL_SDM_ORD;
	if(dbg_print) printk("TXPLL_SDM\n");
	RG_PXP_TXPLL_SDM_DI_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SDM_DI_EN);
	RG_PXP_TXPLL_SDM_DI_EN.hal.rg_pxp_txpll_sdm_di_en = di_en;	
	RG_PXP_TXPLL_SDM_DI_EN.hal.rg_pxp_txpll_sdm_di_ls = 0x0;	
	RG_PXP_TXPLL_SDM_DI_EN.hal.rg_pxp_txpll_sdm_ifm = ifm;	
	RG_PXP_TXPLL_SDM_DI_EN.hal.rg_pxp_txpll_sdm_mode = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SDM_DI_EN,RG_PXP_TXPLL_SDM_DI_EN.dat.value);

	RG_PXP_TXPLL_SDM_ORD.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SDM_ORD);
	RG_PXP_TXPLL_SDM_ORD.hal.rg_pxp_txpll_sdm_ord = 0x3;	
	RG_PXP_TXPLL_SDM_ORD.hal.rg_pxp_txpll_sdm_hren = hren;	
	RG_PXP_TXPLL_SDM_ORD.hal.rg_pxp_txpll_sdm_out = 0x0;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SDM_ORD,RG_PXP_TXPLL_SDM_ORD.dat.value);
}

static void TXPLL_SSC(void){
	rg_type_t(HAL_RG_PXP_TXPLL_SSC_DELTA1) RG_PXP_TXPLL_SSC_DELTA1;
	rg_type_t(HAL_RG_PXP_TXPLL_SSC_EN) RG_PXP_TXPLL_SSC_EN;
	rg_type_t(HAL_RG_PXP_TXPLL_SSC_PERIOD) RG_PXP_TXPLL_SSC_PERIOD;
	if(dbg_print) printk("TXPLL_SSC\n");

	RG_PXP_TXPLL_SSC_DELTA1.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_DELTA1);
	RG_PXP_TXPLL_SSC_DELTA1.hal.rg_pxp_txpll_ssc_delta = 0x0;	
	RG_PXP_TXPLL_SSC_DELTA1.hal.rg_pxp_txpll_ssc_delta1 = 0x0;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_DELTA1,RG_PXP_TXPLL_SSC_DELTA1.dat.value);

	RG_PXP_TXPLL_SSC_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_EN);
	RG_PXP_TXPLL_SSC_EN.hal.rg_pxp_txpll_ssc_tri_en = 0x0;	
	RG_PXP_TXPLL_SSC_EN.hal.rg_pxp_txpll_ssc_phase_ini = 0x0;	
	RG_PXP_TXPLL_SSC_EN.hal.rg_pxp_txpll_ssc_en = 0x0;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_EN,RG_PXP_TXPLL_SSC_EN.dat.value);

	
	RG_PXP_TXPLL_SSC_PERIOD.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_PERIOD);
	RG_PXP_TXPLL_SSC_PERIOD.hal.rg_pxp_txpll_ssc_period = 0x0;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_PERIOD,RG_PXP_TXPLL_SSC_PERIOD.dat.value);
}

static void TXPLL_LPF(u8 chp_ibias,u8 br,u8 bc,u8 bp,u8 bwr,u8 bwc){
	rg_type_t(HAL_RG_PXP_TXPLL_CHP_IBIAS) RG_PXP_TXPLL_CHP_IBIAS;
	rg_type_t(HAL_RG_PXP_TXPLL_LPF_BP) RG_PXP_TXPLL_LPF_BP;
	//rg_type_t(HAL_RG_PXP_JCPLL_FREQ_MEAS_EN) RG_PXP_JCPLL_FREQ_MEAS_EN;
	if(dbg_print) printk("TXPLL_LPF\n");
	RG_PXP_TXPLL_CHP_IBIAS.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_CHP_IBIAS);
	RG_PXP_TXPLL_CHP_IBIAS.hal.rg_pxp_txpll_chp_ibias = chp_ibias;	
	RG_PXP_TXPLL_CHP_IBIAS.hal.rg_pxp_txpll_chp_iofst = 0x0;	
	RG_PXP_TXPLL_CHP_IBIAS.hal.rg_pxp_txpll_lpf_br = br;	
	RG_PXP_TXPLL_CHP_IBIAS.hal.rg_pxp_txpll_lpf_bc = bc;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_CHP_IBIAS,RG_PXP_TXPLL_CHP_IBIAS.dat.value);

	RG_PXP_TXPLL_LPF_BP.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_LPF_BP);
	RG_PXP_TXPLL_LPF_BP.hal.rg_pxp_txpll_lpf_bp = bp;	
	RG_PXP_TXPLL_LPF_BP.hal.rg_pxp_txpll_lpf_bwr = bwr;	
	RG_PXP_TXPLL_LPF_BP.hal.rg_pxp_txpll_lpf_bwc = bwc;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_LPF_BP,RG_PXP_TXPLL_LPF_BP.dat.value);

	//check CODA!!! YT mark for default setting
	//RG_PXP_JCPLL_FREQ_MEAS_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_FREQ_MEAS_EN);
	//RG_PXP_JCPLL_FREQ_MEAS_EN.hal.rg_pxp_txpll_lpf_shck_en = 0x0;	
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_JCPLL_FREQ_MEAS_EN,RG_PXP_JCPLL_FREQ_MEAS_EN.dat.value);
}

static void TXPLL_VCO(u8 vco_cfix){
	rg_type_t(HAL_RG_PXP_TXPLL_TCL_LPF_EN) RG_PXP_TXPLL_TCL_LPF_EN;
	rg_type_t(HAL_RG_PXP_TXPLL_VCO_HALFLSB_EN) RG_PXP_TXPLL_VCO_HALFLSB_EN;
	if(dbg_print) printk("TXPLL_VCO\n");
	RG_PXP_TXPLL_TCL_LPF_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN);
	RG_PXP_TXPLL_TCL_LPF_EN.hal.rg_pxp_txpll_vco_cfix = vco_cfix;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN,RG_PXP_TXPLL_TCL_LPF_EN.dat.value);
	
	RG_PXP_TXPLL_VCO_HALFLSB_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_VCO_HALFLSB_EN);
	RG_PXP_TXPLL_VCO_HALFLSB_EN.hal.rg_pxp_txpll_vco_halflsb_en = 1;
	RG_PXP_TXPLL_VCO_HALFLSB_EN.hal.rg_pxp_txpll_vco_scapwr = 0x7;

	RG_PXP_TXPLL_VCO_HALFLSB_EN.hal.rg_pxp_txpll_vco_vcovar_bias_h = 0x4;	
	RG_PXP_TXPLL_VCO_HALFLSB_EN.hal.rg_pxp_txpll_vco_vcovar_bias_l = 0x0;	
	RG_PXP_TXPLL_VCO_HALFLSB_EN.hal.rg_pxp_txpll_vco_tclvar = 0x4;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_VCO_HALFLSB_EN,RG_PXP_TXPLL_VCO_HALFLSB_EN.dat.value);
}

static void TXPLL_PCW(u32 pcw){
	
	rg_type_t(HAL_rg_force_da_pxp_txpll_sdm_pcw) rg_force_da_pxp_txpll_sdm_pcw;
	rg_type_t(HAL_rg_force_da_pxp_cdr_pr_idac) rg_force_da_pxp_cdr_pr_idac;
	if(dbg_print) printk("TXPLL_PCW\n");
	rg_force_da_pxp_txpll_sdm_pcw.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_txpll_sdm_pcw);
	rg_force_da_pxp_txpll_sdm_pcw.hal.rg_force_da_pxp_txpll_sdm_pcw = pcw;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_txpll_sdm_pcw,rg_force_da_pxp_txpll_sdm_pcw.dat.value);

	
	rg_force_da_pxp_cdr_pr_idac.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac);
	rg_force_da_pxp_cdr_pr_idac.hal.rg_force_sel_da_pxp_txpll_sdm_pcw = 0x1;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac,rg_force_da_pxp_cdr_pr_idac.dat.value);
}
static void TXPLL_RSTB(u8 action){	
	rg_type_t(HAL_RG_PXP_TXPLL_REFIN_INTERNAL) RG_PXP_TXPLL_REFIN_INTERNAL;
	if(dbg_print) printk("TXPLL_RSTB\n");
	RG_PXP_TXPLL_REFIN_INTERNAL.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL);
	RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_rst_dly = 0x4; 
	RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_refin_internal = 1; 
	RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_refin_div = 0; //10.0.9
	switch(action) {
		case 0:			
			RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_pll_rstb = 0x0;	
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL,RG_PXP_TXPLL_REFIN_INTERNAL.dat.value);

			udelay(200);//10.0.9
			RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_pll_rstb = 0x1;	
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL,RG_PXP_TXPLL_REFIN_INTERNAL.dat.value);
			break;
		case 1:
			//if(!RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_pll_rstb)//10.0.9
			{
				RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_pll_rstb = 0x1;	
				RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL,RG_PXP_TXPLL_REFIN_INTERNAL.dat.value);
			}	
			break;
		default:
			RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_pll_rstb = 0x0;	
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL,RG_PXP_TXPLL_REFIN_INTERNAL.dat.value);

			udelay(200);//10.0.9
			RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_pll_rstb = 0x1;	
			RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL,RG_PXP_TXPLL_REFIN_INTERNAL.dat.value);
			break;
	}		
}

//Coda default value = 0x01
static void TXPLL_VCOLDO_Out(void){	
	rg_type_t(HAL_RG_PXP_TXPLL_SSC_PERIOD) RG_PXP_TXPLL_SSC_PERIOD;
	if(dbg_print) printk("TXPLL_VCOLDO_Out\n");
	RG_PXP_TXPLL_SSC_PERIOD.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_PERIOD);
	RG_PXP_TXPLL_SSC_PERIOD.hal.rg_pxp_txpll_ldo_vco_out = 0x01; //0x00 > 0x01 check from kaiwen	
	RG_PXP_TXPLL_SSC_PERIOD.hal.rg_pxp_txpll_ldo_out = 0x01;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SSC_PERIOD,RG_PXP_TXPLL_SSC_PERIOD.dat.value);
}
static void TXPLL_DIV(u8 prediv_mode,u8 vcodiv, u8 postdiv_en){
	rg_type_t(HAL_RG_PXP_TXPLL_KBAND_KS) RG_PXP_TXPLL_KBAND_KS;	
	rg_type_t(HAL_RG_PXP_TXPLL_TCL_LPF_EN)  RG_PXP_TXPLL_TCL_LPF_EN;
	if(dbg_print) printk("TXPLL_DIV\n");
	RG_PXP_TXPLL_KBAND_KS.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_KBAND_KS);
	RG_PXP_TXPLL_KBAND_KS.hal.rg_pxp_txpll_mmd_prediv_mode = prediv_mode;	
	//YT add default value 1, set to 0
	//rg_pxp_txpll_postdiv_d256_en //10.09 no need
	RG_PXP_TXPLL_KBAND_KS.hal.rg_pxp_txpll_postdiv_en = postdiv_en;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_KBAND_KS,RG_PXP_TXPLL_KBAND_KS.dat.value);

	RG_PXP_TXPLL_TCL_LPF_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN);
	RG_PXP_TXPLL_TCL_LPF_EN.hal.rg_pxp_txpll_vcodiv = vcodiv;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN,RG_PXP_TXPLL_TCL_LPF_EN.dat.value);
}

static void TXPLL_KBand(u8 kband_div){
	//rg_type_t(HAL_RG_PXP_TXPLL_REFIN_INTERNAL) RG_PXP_TXPLL_REFIN_INTERNAL;
	rg_type_t(HAL_RG_PXP_TXPLL_KBAND_CODE) RG_PXP_TXPLL_KBAND_CODE;
	rg_type_t(HAL_RG_PXP_TXPLL_KBAND_KS) RG_PXP_TXPLL_KBAND_KS;		
	//rg_type_t(HAL_RG_PXP_TXPLL_TCL_LPF_EN)  RG_PXP_TXPLL_TCL_LPF_EN;
	rg_type_t(HAL_RG_PXP_TXPLL_LPF_BP) RG_PXP_TXPLL_LPF_BP;
	if(dbg_print) printk("TXPLL_KBand\n");
	//move to TXPLL_RST
	//RG_PXP_TXPLL_REFIN_INTERNAL.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL);
	//RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_pll_rstb = 0x1;	
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL,RG_PXP_TXPLL_REFIN_INTERNAL.dat.value);

	RG_PXP_TXPLL_KBAND_CODE.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_KBAND_CODE);
	RG_PXP_TXPLL_KBAND_CODE.hal.rg_pxp_txpll_kband_kfc = 0x0;	
	RG_PXP_TXPLL_KBAND_CODE.hal.rg_pxp_txpll_kband_kf = 0x3;	
	RG_PXP_TXPLL_KBAND_CODE.hal.rg_pxp_txpll_kband_div = kband_div;
	//kris randon value change to default value
	RG_PXP_TXPLL_KBAND_CODE.hal.rg_pxp_txpll_kband_code = 0xE4; 
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_KBAND_CODE,RG_PXP_TXPLL_KBAND_CODE.dat.value);

	RG_PXP_TXPLL_KBAND_KS.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_KBAND_KS);
	RG_PXP_TXPLL_KBAND_KS.hal.rg_pxp_txpll_kband_ks = 0x1;	
	//move to TXPLL_DIV
	//RG_PXP_TXPLL_KBAND_KS.hal.rg_pxp_txpll_mmd_prediv_mode = 0x0;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_KBAND_KS,RG_PXP_TXPLL_KBAND_KS.dat.value);

	//move to TXPLL_DIV
	//RG_PXP_TXPLL_TCL_LPF_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN);
	//RG_PXP_TXPLL_TCL_LPF_EN.hal.rg_pxp_txpll_vcodiv = 0;	
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN,RG_PXP_TXPLL_TCL_LPF_EN.dat.value);

	RG_PXP_TXPLL_LPF_BP.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_LPF_BP);
	RG_PXP_TXPLL_LPF_BP.hal.rg_pxp_txpll_kband_option = 0x0;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_LPF_BP,RG_PXP_TXPLL_LPF_BP.dat.value);

}

static void TXPLL_TCL(u8 kband_vref,u8 amp_gain,u8 amp_vref,u8 lpf_bw,u8 lpf_en,u8 amp_en){
	rg_type_t(HAL_RG_PXP_TXPLL_TCL_KBAND_VREF) RG_PXP_TXPLL_TCL_KBAND_VREF;
	rg_type_t(HAL_RG_PXP_TXPLL_TCL_AMP_GAIN) RG_PXP_TXPLL_TCL_AMP_GAIN;
	rg_type_t(HAL_RG_PXP_TXPLL_TCL_LPF_EN) RG_PXP_TXPLL_TCL_LPF_EN;		
	rg_type_t(HAL_RG_PXP_TXPLL_SDM_ORD)  RG_PXP_TXPLL_SDM_ORD;

	if(dbg_print) printk("TXPLL_TCL\n");
	RG_PXP_TXPLL_TCL_KBAND_VREF.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_KBAND_VREF);
	RG_PXP_TXPLL_TCL_KBAND_VREF.hal.rg_pxp_txpll_tcl_kband_vref = kband_vref;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_KBAND_VREF,RG_PXP_TXPLL_TCL_KBAND_VREF.dat.value);

	RG_PXP_TXPLL_TCL_AMP_GAIN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_AMP_GAIN);
	RG_PXP_TXPLL_TCL_AMP_GAIN.hal.rg_pxp_txpll_tcl_amp_gain = amp_gain;	
	RG_PXP_TXPLL_TCL_AMP_GAIN.hal.rg_pxp_txpll_tcl_amp_vref = amp_vref;	
	//no use, keep default value
	//RG_PXP_TXPLL_TCL_AMP_GAIN.hal.rg_pxp_txpll_tcl_cmp_vth = 0x00;	
	//RG_PXP_TXPLL_TCL_AMP_GAIN.hal.rg_pxp_txpll_tcl_cmp_en = cmp_en;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_AMP_GAIN,RG_PXP_TXPLL_TCL_AMP_GAIN.dat.value);

	RG_PXP_TXPLL_TCL_LPF_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN);
	RG_PXP_TXPLL_TCL_LPF_EN.hal.rg_pxp_txpll_tcl_lpf_bw = lpf_bw;
	RG_PXP_TXPLL_TCL_LPF_EN.hal.rg_pxp_txpll_tcl_lpf_en = lpf_en;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_TCL_LPF_EN,RG_PXP_TXPLL_TCL_LPF_EN.dat.value);

	RG_PXP_TXPLL_SDM_ORD.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SDM_ORD);
	RG_PXP_TXPLL_SDM_ORD.hal.rg_pxp_txpll_tcl_amp_en = amp_en;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_SDM_ORD,RG_PXP_TXPLL_SDM_ORD.dat.value);
	
}

//monitor setting
#if 0
static void TXPLL_VTP(void){	
	rg_type_t(HAL_RG_PXP_TXPLL_VTP_EN) RG_PXP_TXPLL_VTP_EN;
	rg_type_t(HAL_RG_PXP_TXPLL_PHY_CK1_EN) RG_PXP_TXPLL_PHY_CK1_EN;

	RG_PXP_TXPLL_VTP_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_VTP_EN);
	RG_PXP_TXPLL_VTP_EN.hal.rg_pxp_txpll_ldo_vco_vtp = 0x0;
	RG_PXP_TXPLL_VTP_EN.hal.rg_pxp_txpll_ldo_vco_vtp_en = 0x0;	
	RG_PXP_TXPLL_VTP_EN.hal.rg_pxp_txpll_vtp = 0x0;	
	RG_PXP_TXPLL_VTP_EN.hal.rg_pxp_txpll_vtp_en = 0x1;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_VTP_EN,RG_PXP_TXPLL_VTP_EN.dat.value);

	RG_PXP_TXPLL_PHY_CK1_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_PHY_CK1_EN);
	RG_PXP_TXPLL_PHY_CK1_EN.hal.rg_pxp_txpll_phy_ck2_en = 0x0;	
	RG_PXP_TXPLL_PHY_CK1_EN.hal.rg_pxp_txpll_phy_ck1_en = 0x1;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_PHY_CK1_EN,RG_PXP_TXPLL_PHY_CK1_EN.dat.value);

}
#endif
static void TXPLL_EN(u8 enable){
	rg_type_t(HAL_rg_force_da_pxp_txpll_ckout_en) rg_force_da_pxp_txpll_ckout_en;
	if(dbg_print) printk("TXPLL_EN\n");
	rg_force_da_pxp_txpll_ckout_en.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_txpll_ckout_en);
	rg_force_da_pxp_txpll_ckout_en.hal.rg_force_sel_da_pxp_txpll_en = 1;	
	rg_force_da_pxp_txpll_ckout_en.hal.rg_force_da_pxp_txpll_en = enable;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_txpll_ckout_en,rg_force_da_pxp_txpll_ckout_en.dat.value);
}

static void TXPLL_Out(u8 enable){
	rg_type_t(HAL_rg_force_da_pxp_txpll_ckout_en) rg_force_da_pxp_txpll_ckout_en;
	//rg_type_t(HAL_RG_PXP_TXPLL_REFIN_INTERNAL) RG_PXP_TXPLL_REFIN_INTERNAL;
	if(dbg_print) printk("TXPLL_Out\n");
	rg_force_da_pxp_txpll_ckout_en.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_txpll_ckout_en);
	rg_force_da_pxp_txpll_ckout_en.hal.rg_force_sel_da_pxp_txpll_ckout_en = 1;	
	rg_force_da_pxp_txpll_ckout_en.hal.rg_force_da_pxp_txpll_ckout_en = enable;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_txpll_ckout_en,rg_force_da_pxp_txpll_ckout_en.dat.value);


	//move to TXPLL_RST()
	//RG_PXP_TXPLL_REFIN_INTERNAL.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL);
	//RG_PXP_TXPLL_REFIN_INTERNAL.hal.rg_pxp_txpll_rst_dly = 0x2;	
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TXPLL_REFIN_INTERNAL,RG_PXP_TXPLL_REFIN_INTERNAL.dat.value);
	
}
static void TXPLL_BringUp(u8 xSGMII){
	if(dbg_print) printk("TXPLL BringUp\n");
	TXPLL_VCOLDO_Out();
	TXPLL_RSTB(1);	
	TXPLL_EN(0);
	switch (xSGMII){
		case USXGMII: //10.315G		
			TXPLL_SDM(0,0,1);
			TXPLL_SSC();
			TXPLL_LPF(0x0F,0x05,0x1f,0b10,0b1011,0b11000);
			TXPLL_VCO(0b01);
			TXPLL_PCW(0x08400000);
			TXPLL_KBand(0x4);
			TXPLL_DIV(0,0,0);//10.0.9
			TXPLL_TCL(0xf,0b11,0b01011,0,1,1);
			
			TXPLL_EN(1);
			TXPLL_Out(1);
			break;
		case HSGMII: //12.5G/4
			TXPLL_SDM(0,0,0);
			TXPLL_SSC();
			TXPLL_LPF(0x0A,0x05,0x1f,0b101,0,0) ;
			TXPLL_VCO(0b00);
			TXPLL_PCW(0x0A000000);
			TXPLL_KBand(0x4);
			TXPLL_DIV(0,1,0);//10.0.9
			TXPLL_TCL(0xf,0b11,0b01101,0,1,1);
			
			TXPLL_EN(1);
			TXPLL_Out(1);
			break;
		
		case SGMII:  //10G/8
			TXPLL_SDM(0,0,0);
			TXPLL_SSC();
			TXPLL_LPF(0x0F,0x05,0x1f,0b10,0b1011,0b11000);
			TXPLL_VCO(0b11);
			TXPLL_PCW(0x08000000);
			TXPLL_KBand(0x4);
			TXPLL_DIV(0,0,0); //10.0.9
			TXPLL_TCL(0xf,0b11,0b01011,0,1,1);
			
			TXPLL_EN(1);
			TXPLL_Out(1);
			break;
		default:
			TXPLL_SDM(0,0,1);
			TXPLL_SSC();
			TXPLL_LPF(0x0F,0x05,0x1f,0b10,0b1011,0b11000);
			TXPLL_VCO(0b01);
			TXPLL_PCW(0x08400000);
			TXPLL_KBand(0x4);
			TXPLL_DIV(0,0,0);
			TXPLL_TCL(0xf,0b11,0b01011,0,1,1);
			
			TXPLL_EN(1);
			TXPLL_Out(1);
			printk("waring!!! no such selection %x\n",xSGMII);
			break;
	}
}

static void RX_Path_Init(void){
	rg_type_t(HAL_SS_LCPLL_PWCTL_SETTING_1) SS_LCPLL_PWCTL_SETTING_1;	
	rg_type_t(HAL_RX_EXTRAL_CTRL) RX_EXTRAL_CTRL;	
	rg_type_t(HAL_RG_PXP_CDR_PD_PICAL_CKD8_INV) RG_PXP_CDR_PD_PICAL_CKD8_INV;
	//rg_type_t(HAL_rg_force_da_pxp_cdr_pr_pieye_pwdb) rg_force_da_pxp_cdr_pr_pieye_pwdb;
	//rg_type_t(HAL_rg_force_da_pxp_cdr_pd_pwdb) rg_force_da_pxp_cdr_pd_pwdb;
	//rg_type_t(HAL_rg_force_da_pxp_rx_fe_pwdb) rg_force_da_pxp_rx_fe_pwdb;
	//rg_type_t(HAL_rg_force_da_pxp_rx_scan_rst_b) rg_force_da_pxp_rx_scan_rst_b;
	//rg_type_t(HAL_SS_DA_XPON_PWDB_0) SS_DA_XPON_PWDB_0;
	rg_type_t(HAL_rg_force_da_pxp_aeq_bypass) rg_force_da_pxp_aeq_bypass;
	//rg_type_t(HAL_ADD_RX_SYS_EN_SEL_0) ADD_RX_SYS_EN_SEL_0;
	rg_type_t(HAL_rg_force_da_pxp_aeq_rstb) rg_force_da_pxp_aeq_rstb;
	//rg_type_t(HAL_rg_force_da_pxp_aeq_speed) rg_force_da_pxp_aeq_speed;
	rg_type_t(HAL_RG_PXP_CDR_PR_MONPR_EN) RG_PXP_CDR_PR_MONPR_EN;
	//rg_type_t(HAL_RG_PXP_CDR_PR_BETA_DAC) RG_PXP_CDR_PR_BETA_DAC;
	//rg_type_t(HAL_RG_PXP_CDR_PR_VREG_IBAND_VAL) RG_PXP_CDR_PR_VREG_IBAND_VAL;
	//rg_type_t(HAL_RG_PXP_CDR_LPF_RATIO) RG_PXP_CDR_LPF_RATIO;
	//rg_type_t(HAL_RG_PXP_RX_PHYCK_DIV) RG_PXP_RX_PHYCK_DIV;//10.0.3 mark

//Kris sim to add
	rg_type_t(HAL_RG_PXP_AEQ_CFORCE) RG_PXP_AEQ_CFORCE;
	rg_type_t(HAL_RG_PXP_RX_OSCAL_WATCH_WNDW) RG_PXP_RX_OSCAL_WATCH_WNDW;
	rg_type_t(HAL_RX_DISB_MODE_4) RX_DISB_MODE_4;
	rg_type_t(HAL_SS_RX_FLL_5) SS_RX_FLL_5;
	rg_type_t(HAL_RG_PXP_RX_DAC_D1_BYPASS_AEQ) RG_PXP_RX_DAC_D1_BYPASS_AEQ;
	rg_type_t(HAL_RG_PXP_RX_FE_PEAKING_CTRL_MSB) RG_PXP_RX_FE_PEAKING_CTRL_MSB;

	rg_type_t(HAL_RG_PXP_RX_FE_VCM_GEN_PWDB) RG_PXP_RX_FE_VCM_GEN_PWDB;
	if(dbg_print) printk("RX_Path_Init\n");

	//V10***
	SS_RX_FLL_5.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_5);
	SS_RX_FLL_5.hal.rg_fll_idac_max = 0x3FF;
	SS_RX_FLL_5.hal.rg_fll_idac_min = 0x400;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_5,SS_RX_FLL_5.dat.value);

	RG_PXP_RX_DAC_D1_BYPASS_AEQ.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_DAC_D1_BYPASS_AEQ);
	RG_PXP_RX_DAC_D1_BYPASS_AEQ.hal.rg_pxp_rx_dac_eye_bypass_aeq = 1;
	RG_PXP_RX_DAC_D1_BYPASS_AEQ.hal.rg_pxp_rx_dac_e1_bypass_aeq = 1;
	RG_PXP_RX_DAC_D1_BYPASS_AEQ.hal.rg_pxp_rx_dac_e0_bypass_aeq = 1;	
	RG_PXP_RX_DAC_D1_BYPASS_AEQ.hal.rg_pxp_rx_dac_d1_bypass_aeq = 1;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_DAC_D1_BYPASS_AEQ,RG_PXP_RX_DAC_D1_BYPASS_AEQ.dat.value);

	RG_PXP_RX_FE_PEAKING_CTRL_MSB.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_PEAKING_CTRL_MSB);
	RG_PXP_RX_FE_PEAKING_CTRL_MSB.hal.rg_pxp_rx_dac_d0_bypass_aeq = 1;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_PEAKING_CTRL_MSB,RG_PXP_RX_FE_PEAKING_CTRL_MSB.dat.value);

	RG_PXP_RX_FE_VCM_GEN_PWDB.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_VCM_GEN_PWDB);
	RG_PXP_RX_FE_VCM_GEN_PWDB.hal.rg_pxp_rx_fe_vcm_gen_pwdb = 1;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_VCM_GEN_PWDB,RG_PXP_RX_FE_VCM_GEN_PWDB.dat.value);
	//V10&&&


	SS_LCPLL_PWCTL_SETTING_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_LCPLL_PWCTL_SETTING_1);
	SS_LCPLL_PWCTL_SETTING_1.hal.rg_lcpll_man_pwdb = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_LCPLL_PWCTL_SETTING_1,SS_LCPLL_PWCTL_SETTING_1.dat.value);
	
	RG_PXP_AEQ_CFORCE.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_AEQ_CFORCE);
	RG_PXP_AEQ_CFORCE.hal.rg_pxp_aeq_oforce = 1<<9;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_AEQ_CFORCE,RG_PXP_AEQ_CFORCE.dat.value);
	
	RG_PXP_RX_OSCAL_WATCH_WNDW.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_OSCAL_WATCH_WNDW);
	RG_PXP_RX_OSCAL_WATCH_WNDW.hal.rg_pxp_rx_oscal_force = 0x3FF;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_OSCAL_WATCH_WNDW,RG_PXP_RX_OSCAL_WATCH_WNDW.dat.value);

	RX_DISB_MODE_4.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_4);
	RX_DISB_MODE_4.hal.rg_disb_blwc_offset = 0;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_4,RX_DISB_MODE_4.dat.value);

//&&&
	RX_EXTRAL_CTRL.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EXTRAL_CTRL);
	RX_EXTRAL_CTRL.hal.rg_disb_leq = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EXTRAL_CTRL,RX_EXTRAL_CTRL.dat.value);

	RG_PXP_CDR_PD_PICAL_CKD8_INV.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PD_PICAL_CKD8_INV);
	RG_PXP_CDR_PD_PICAL_CKD8_INV.hal.rg_pxp_cdr_pd_edge_dis = 0x0;	
	RG_PXP_CDR_PD_PICAL_CKD8_INV.hal.rg_pxp_cdr_pd_pical_ckd8_inv = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PD_PICAL_CKD8_INV,RG_PXP_CDR_PD_PICAL_CKD8_INV.dat.value);
#if 0 //move to pwdb
	rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb);
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_pwdb = 0x1;	
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pwdb = 0x1;
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_pieye_pwdb = 0x1;
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pieye_pwdb = 0x1;
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);

	rg_force_da_pxp_cdr_pd_pwdb.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pd_pwdb);
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_kband_rstb = 0x0;	
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_da_pxp_cdr_pr_kband_rstb = 0x0;	
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_sel_da_pxp_cdr_pd_pwdb = 0x1;	
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_da_pxp_cdr_pd_pwdb = 0x1;
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pd_pwdb,rg_force_da_pxp_cdr_pd_pwdb.dat.value);

	rg_force_da_pxp_rx_fe_pwdb.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_pwdb);
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_sel_da_pxp_rx_pdoscal_en = 0x0;
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_da_pxp_rx_pdoscal_en = 0x0;
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_sel_da_pxp_rx_fe_pwdb = 0x1;
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_da_pxp_rx_fe_pwdb = 0x1;	
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_pwdb,rg_force_da_pxp_rx_fe_pwdb.dat.value);

	rg_force_da_pxp_rx_scan_rst_b.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_scan_rst_b);
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_sel_da_pxp_rx_sigdet_pwdb = 0x1;
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_da_pxp_rx_sigdet_pwdb = 0x1;
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_sel_da_pxp_rx_scan_rst_b = 0x0;
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_da_pxp_rx_scan_rst_b = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_scan_rst_b,rg_force_da_pxp_rx_scan_rst_b.dat.value);

	SS_DA_XPON_PWDB_0.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_0);
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_cdr_pd_pwdb = 0x1;
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_cdr_pr_pieye_pwdb = 0x1;
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_cdr_pr_pwdb = 0x1;
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_rx_fe_pwdb = 0x1;	
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_0,SS_DA_XPON_PWDB_0.dat.value);
#endif	    
	rg_force_da_pxp_aeq_bypass.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_bypass);
	rg_force_da_pxp_aeq_bypass.hal.rg_force_sel_da_pxp_aeq_ckon = 0x1;
	rg_force_da_pxp_aeq_bypass.hal.rg_force_da_pxp_aeq_ckon = 0x0; //10.0.4
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_bypass,rg_force_da_pxp_aeq_bypass.dat.value);
#if 0
	ADD_RX_SYS_EN_SEL_0.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _ADD_RX_SYS_EN_SEL_0);
	ADD_RX_SYS_EN_SEL_0.hal.rg_da_rx_sys_en_sel = 0x1;
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _ADD_RX_SYS_EN_SEL_0,ADD_RX_SYS_EN_SEL_0.dat.value);
#endif
	rg_force_da_pxp_aeq_rstb.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_rstb);
	rg_force_da_pxp_aeq_rstb.hal.rg_force_sel_da_pxp_cdr_injck_sel = 0x1;	
	rg_force_da_pxp_aeq_rstb.hal.rg_force_da_pxp_cdr_injck_sel = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_rstb,rg_force_da_pxp_aeq_rstb.dat.value);
//---------------------------------
#if 0
	rg_force_da_pxp_aeq_speed.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_speed);
	rg_force_da_pxp_aeq_speed.hal.rg_force_sel_da_pxp_rx_osr_sel = 0x1;	
	rg_force_da_pxp_aeq_speed.hal.rg_force_da_pxp_rx_osr_sel = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_speed,rg_force_da_pxp_aeq_speed.dat.value);
#endif	
	RG_PXP_CDR_PR_MONPR_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_MONPR_EN);
	RG_PXP_CDR_PR_MONPR_EN.hal.rg_pxp_rx_dac_mon = 0x0;
	RG_PXP_CDR_PR_MONPR_EN.hal.rg_pxp_cdr_pr_xfick_en = 0x1;
	RG_PXP_CDR_PR_MONPR_EN.hal.rg_pxp_cdr_pr_monpi_en = 0x0;
	RG_PXP_CDR_PR_MONPR_EN.hal.rg_pxp_cdr_pr_monpr_en = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_MONPR_EN,RG_PXP_CDR_PR_MONPR_EN.dat.value);
#if 0	
	RG_PXP_CDR_PR_BETA_DAC.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_BETA_DAC);
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_kband_div = 0x4;
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_beta_sel = 0x1;
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_vcoadc_os = 0x8;
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_beta_dac = 0x1C;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_BETA_DAC,RG_PXP_CDR_PR_BETA_DAC.dat.value);

	RG_PXP_CDR_PR_VREG_IBAND_VAL.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_VREG_IBAND_VAL);
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_fbksel = 0x0;
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_dac_band = 0x1C;
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_vreg_ckbuf_val = 0x0;
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_vreg_iband_val = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_VREG_IBAND_VAL,RG_PXP_CDR_PR_VREG_IBAND_VAL.dat.value);

	RG_PXP_CDR_LPF_RATIO.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_LPF_RATIO);
	RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_top_lim = 0x20000;	
	RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_ratio = 0x0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_LPF_RATIO,RG_PXP_CDR_LPF_RATIO.dat.value);
#endif	

	//10.0.3 mark
	//RG_PXP_RX_PHYCK_DIV.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_PHYCK_DIV);
	//RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_tdc_ck_sel = 0x1;
	//RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_phyck_sel = 0x1;
	//RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_phyck_div = 0x42;
	//RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_phyck_rstb = 0x0;
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_PHYCK_DIV,RG_PXP_RX_PHYCK_DIV.dat.value);

}

static void RX_OSR(u8 OSR){
	rg_type_t(HAL_rg_force_da_pxp_aeq_speed) rg_force_da_pxp_aeq_speed;
	rg_type_t(HAL_RG_PXP_CDR_PD_PICAL_CKD8_INV) RG_PXP_CDR_PD_PICAL_CKD8_INV;
	if(dbg_print) printk("RX_OSR\n");
	rg_force_da_pxp_aeq_speed.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_speed);
	rg_force_da_pxp_aeq_speed.hal.rg_force_sel_da_pxp_rx_osr_sel = 0x1;
	rg_force_da_pxp_aeq_speed.hal.rg_force_da_pxp_rx_osr_sel = OSR;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_aeq_speed,rg_force_da_pxp_aeq_speed.dat.value);

	
	RG_PXP_CDR_PD_PICAL_CKD8_INV.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PD_PICAL_CKD8_INV);
	RG_PXP_CDR_PD_PICAL_CKD8_INV.hal.rg_pxp_cdr_pd_edge_dis = OSR > 0? 1:0; //dis = 1 when osr /2~8	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PD_PICAL_CKD8_INV,RG_PXP_CDR_PD_PICAL_CKD8_INV.dat.value);
}
static void RX_CDR_xxx_Pwdb(u8 SW){	
	rg_type_t(HAL_rg_force_da_pxp_cdr_pr_pieye_pwdb) rg_force_da_pxp_cdr_pr_pieye_pwdb;
	rg_type_t(HAL_rg_force_da_pxp_cdr_pd_pwdb) rg_force_da_pxp_cdr_pd_pwdb;
	rg_type_t(HAL_rg_force_da_pxp_rx_fe_pwdb) rg_force_da_pxp_rx_fe_pwdb;
	rg_type_t(HAL_rg_force_da_pxp_rx_scan_rst_b) rg_force_da_pxp_rx_scan_rst_b;
	rg_type_t(HAL_SS_DA_XPON_PWDB_0) SS_DA_XPON_PWDB_0;	
	if(dbg_print) printk("RX_CDR_xxx_Pwdb\n");
//L0 start
	rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb);
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_pwdb = 1;	
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pwdb = SW;
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_pieye_pwdb = 1;
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pieye_pwdb = SW;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);

	rg_force_da_pxp_cdr_pd_pwdb.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pd_pwdb);
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_kband_rstb = 0x0;	
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_da_pxp_cdr_pr_kband_rstb = 0x0;	
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_sel_da_pxp_cdr_pd_pwdb = 1;	
	rg_force_da_pxp_cdr_pd_pwdb.hal.rg_force_da_pxp_cdr_pd_pwdb = SW;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pd_pwdb,rg_force_da_pxp_cdr_pd_pwdb.dat.value);

	rg_force_da_pxp_rx_fe_pwdb.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_pwdb);
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_sel_da_pxp_rx_pdoscal_en = 0x0; //default value ok
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_da_pxp_rx_pdoscal_en = 0x0;
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_sel_da_pxp_rx_fe_pwdb = 1;
	rg_force_da_pxp_rx_fe_pwdb.hal.rg_force_da_pxp_rx_fe_pwdb = SW;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_pwdb,rg_force_da_pxp_rx_fe_pwdb.dat.value);

	rg_force_da_pxp_rx_scan_rst_b.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_scan_rst_b);
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_sel_da_pxp_rx_sigdet_pwdb = 1;
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_da_pxp_rx_sigdet_pwdb = SW;
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_sel_da_pxp_rx_scan_rst_b = 0x0; //debug mode, default value ok 
	rg_force_da_pxp_rx_scan_rst_b.hal.rg_force_da_pxp_rx_scan_rst_b = 0x0; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_scan_rst_b,rg_force_da_pxp_rx_scan_rst_b.dat.value);

	SS_DA_XPON_PWDB_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_0);
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_cdr_pd_pwdb = SW;
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_cdr_pr_pieye_pwdb = SW;
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_cdr_pr_pwdb = SW;
	SS_DA_XPON_PWDB_0.hal.rg_da_xpon_rx_fe_pwdb = SW;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_0,SS_DA_XPON_PWDB_0.dat.value);
}
static void RX_SYS_En(u8 on){	
	rg_type_t(HAL_ADD_RX_SYS_EN_SEL_0) ADD_RX_SYS_EN_SEL_0;
	rg_type_t(HAL_SS_LCPLL_PWCTL_SETTING_0) SS_LCPLL_PWCTL_SETTING_0;
	
	if(dbg_print) printk("RX_SYS_En\n");
	//depend on lcpllen enable
	ADD_RX_SYS_EN_SEL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_RX_SYS_EN_SEL_0);
	ADD_RX_SYS_EN_SEL_0.hal.rg_da_rx_sys_en_sel = on;//0x01
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_RX_SYS_EN_SEL_0,ADD_RX_SYS_EN_SEL_0.dat.value);
	
	SS_LCPLL_PWCTL_SETTING_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_LCPLL_PWCTL_SETTING_0);
	SS_LCPLL_PWCTL_SETTING_0.hal.rg_sw_lcpll_en = 1; //old structure, no use
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_LCPLL_PWCTL_SETTING_0,SS_LCPLL_PWCTL_SETTING_0.dat.value);
}

static void CDR_LPF_RATIO(u8 ratio){	
	rg_type_t(HAL_RG_PXP_CDR_LPF_RATIO) RG_PXP_CDR_LPF_RATIO;
	if(dbg_print) printk("CDR_LPF_RATIO\n");
	RG_PXP_CDR_LPF_RATIO.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_LPF_RATIO);
	RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_top_lim = 0x20000;	
	RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_ratio = ratio;//10.0.9
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_LPF_RATIO,RG_PXP_CDR_LPF_RATIO.dat.value);
}

static void RX_Cal_En(u8 en){
	//Use default value
}

static void CDR_PR(u8 dac,u8 band){	
	rg_type_t(HAL_RG_PXP_CDR_PR_VREG_IBAND_VAL) RG_PXP_CDR_PR_VREG_IBAND_VAL;
	rg_type_t(HAL_RG_PXP_CDR_PR_BETA_DAC) RG_PXP_CDR_PR_BETA_DAC;
	if(dbg_print) printk("CDR_PR\n");
	RG_PXP_CDR_PR_BETA_DAC.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_BETA_DAC);
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_kband_div = 0x4;
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_beta_sel = 0x1;
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_vcoadc_os = 0x8;
	RG_PXP_CDR_PR_BETA_DAC.hal.rg_pxp_cdr_pr_beta_dac = dac;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_BETA_DAC,RG_PXP_CDR_PR_BETA_DAC.dat.value);
//V10	
	RG_PXP_CDR_PR_VREG_IBAND_VAL.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_VREG_IBAND_VAL);
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_fbksel = 0x0;
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_dac_band = band;
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_vreg_ckbuf_val = 0x6;
	RG_PXP_CDR_PR_VREG_IBAND_VAL.hal.rg_pxp_cdr_pr_vreg_iband_val = 0x6;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_VREG_IBAND_VAL,RG_PXP_CDR_PR_VREG_IBAND_VAL.dat.value);
}
static void RX_BySerdes(u8 xsgmii){	
	if(dbg_print) printk("RX_BySerdes\n");
	switch(xsgmii){
		case USXGMII:
			RX_OSR(0b00);
			CDR_LPF_RATIO(0b00);
			CDR_PR(0b0001000,0b01000);
			break;
		case HSGMII:			
			RX_OSR(0b01);
			CDR_LPF_RATIO(0b01);
			CDR_PR(0b0000110,0b00110);
			break;
		case SGMII:			
			RX_OSR(0b11);
			CDR_LPF_RATIO(0b11);
			CDR_PR(0b0001000,0b01000);
			break;
		default:			
			RX_OSR(0b00);
			CDR_LPF_RATIO(0b00);
			CDR_PR(0b0001000,0b01000);
			break;
	}
}

static void RX_EYE_Mon(void){
#if 0	
	rg_type_t(HAL_RG_PXP_RX_FE_EQ_HZEN) RG_PXP_RX_FE_EQ_HZEN;
#endif
#if 0
	rg_type_t(HAL_SS_DA_XPON_PWDB_1) SS_DA_XPON_PWDB_1;
#endif
	rg_type_t(HAL_PHY_EQ_CTRL_2) PHY_EQ_CTRL_2;
	rg_type_t(HAL_RX_EYE_TOP_EYECNT_CTRL_2) RX_EYE_TOP_EYECNT_CTRL_2;
	if(dbg_print) printk("RX_EYE_Mon\n");
#if 0	
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_0) RX_CTRL_SEQUENCE_CTRL_0;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_6) RX_CTRL_SEQUENCE_CTRL_6;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_1) RX_CTRL_SEQUENCE_CTRL_1;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_4) RX_CTRL_SEQUENCE_CTRL_4;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_2) RX_CTRL_SEQUENCE_CTRL_2;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_3) RX_CTRL_SEQUENCE_CTRL_3;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_5) RX_CTRL_SEQUENCE_CTRL_5;
	rg_type_t(HAL_SS_LCPLL_PWCTL_SETTING_0) SS_LCPLL_PWCTL_SETTING_0;
	//rg_type_t(HAL_SW_RST_SET) SW_RST_SET;
#endif
#if 0	
	RG_PXP_RX_FE_EQ_HZEN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_EQ_HZEN);
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_vb_eq3_en = 0x1;
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_vb_eq2_en = 0x1;	
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_vb_eq1_en = 0x1; //CML EQ1~3	
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_eq_hzen = 0x0; //old thing, don't care	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_EQ_HZEN,RG_PXP_RX_FE_EQ_HZEN.dat.value);
#endif
#if 0	
	SS_DA_XPON_PWDB_1.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_1);
	SS_DA_XPON_PWDB_1.hal.rg_da_xpon_rx_sigdet_pwdb = 0x1;
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_1,SS_DA_XPON_PWDB_1.dat.value);
#endif
	PHY_EQ_CTRL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_2);
	PHY_EQ_CTRL_2.hal.rg_eq_debug_sel = 0x0;
	PHY_EQ_CTRL_2.hal.rg_fom_num_order = 0x1; 
	PHY_EQ_CTRL_2.hal.rg_a_sel = 0x3; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_2,PHY_EQ_CTRL_2.dat.value);
	
	RX_EYE_TOP_EYECNT_CTRL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_2);
	RX_EYE_TOP_EYECNT_CTRL_2.hal.rg_data_shift = 0x0;	
	RX_EYE_TOP_EYECNT_CTRL_2.hal.rg_eyecnt_fast = 0x1; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_2,RX_EYE_TOP_EYECNT_CTRL_2.dat.value);
#if 0
	RX_CTRL_SEQUENCE_CTRL_0.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_0);
	RX_CTRL_SEQUENCE_CTRL_0.hal.rg_rx_os_start = 0x1;
	RX_CTRL_SEQUENCE_CTRL_0.hal.rg_osc_speed_opt = 0x1; 
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_0,RX_CTRL_SEQUENCE_CTRL_0.dat.value);

	RX_CTRL_SEQUENCE_CTRL_6.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_6);
	RX_CTRL_SEQUENCE_CTRL_6.hal.rg_rx_os_end = 0x2; 
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_6,RX_CTRL_SEQUENCE_CTRL_6.dat.value);

	RX_CTRL_SEQUENCE_CTRL_1.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_1);
	RX_CTRL_SEQUENCE_CTRL_1.hal.rg_rx_pical_end = 0x32;	
	RX_CTRL_SEQUENCE_CTRL_1.hal.rg_rx_pical_start = 0x2; 
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_1,RX_CTRL_SEQUENCE_CTRL_1.dat.value);

	RX_CTRL_SEQUENCE_CTRL_4.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_4);
	RX_CTRL_SEQUENCE_CTRL_4.hal.rg_rx_sdcal_end = 0x32;	
	RX_CTRL_SEQUENCE_CTRL_4.hal.rg_rx_sdcal_start = 0x2; 
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_4,RX_CTRL_SEQUENCE_CTRL_4.dat.value);

	RX_CTRL_SEQUENCE_CTRL_2.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_2);
	RX_CTRL_SEQUENCE_CTRL_2.hal.rg_rx_pdos_end = 0x32;	
	RX_CTRL_SEQUENCE_CTRL_2.hal.rg_rx_pdos_start = 0x2;
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_2,RX_CTRL_SEQUENCE_CTRL_2.dat.value);

	RX_CTRL_SEQUENCE_CTRL_3.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_3);
	RX_CTRL_SEQUENCE_CTRL_3.hal.rg_rx_feos_end = 0x32;	
	RX_CTRL_SEQUENCE_CTRL_3.hal.rg_rx_feos_start = 0x2; 
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_3,RX_CTRL_SEQUENCE_CTRL_3.dat.value);

	RX_CTRL_SEQUENCE_CTRL_5.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_5);
	RX_CTRL_SEQUENCE_CTRL_5.hal.rg_rx_rdy = 0xA;	
	RX_CTRL_SEQUENCE_CTRL_5.hal.rg_rx_blwc_rdy_en = 0x5; 
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_5,RX_CTRL_SEQUENCE_CTRL_5.dat.value);

	SS_LCPLL_PWCTL_SETTING_0.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_LCPLL_PWCTL_SETTING_0);
	SS_LCPLL_PWCTL_SETTING_0.hal.rg_sw_lcpll_en = 0x0; //no use
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_LCPLL_PWCTL_SETTING_0,SS_LCPLL_PWCTL_SETTING_0.dat.value);
#endif
#if 0
	SW_RST_SET.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SW_RST_SET);
	SW_RST_SET.hal.rg_sw_xfi_rxpcs_rst_n = 0x1;	
	SW_RST_SET.hal.rg_sw_tx_fifo_rst_n = 0x1; 
	SW_RST_SET.hal.rg_sw_xfi_txpcs_rst_n = 0x1; 
	SW_RST_SET.hal.rg_sw_ref_rst_n = 0x1; 
	SW_RST_SET.hal.rg_sw_allpcs_rst_n = 0x1; 
	SW_RST_SET.hal.rg_sw_pma_rst_n = 0x1; 
	SW_RST_SET.hal.rg_sw_tx_rst_n = 0x1;	
	SW_RST_SET.hal.rg_sw_rx_rst_n = 0x1; 
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SW_RST_SET,SW_RST_SET.dat.value);
#endif	
}
static void RX_CMLEQ_EN(u8 en){	
	rg_type_t(HAL_RG_PXP_RX_FE_EQ_HZEN) RG_PXP_RX_FE_EQ_HZEN;
	if(dbg_print) printk("RX_CMLEQ_EN\n");
	RG_PXP_RX_FE_EQ_HZEN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_EQ_HZEN);
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_vb_eq3_en = en;
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_vb_eq2_en = en;	
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_vb_eq1_en = en; //CML EQ1~3	
	RG_PXP_RX_FE_EQ_HZEN.hal.rg_pxp_rx_fe_eq_hzen = 0x0; //old thing, don't care	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_FE_EQ_HZEN,RG_PXP_RX_FE_EQ_HZEN.dat.value);
} 
*******************************************************************************************************************/	

/*
SER rate control
00: 8 bit  (DA_XPON_RX_DATA[7:0]) /  8 bit  (DA_XFI_RX_DATA[7:0]) 
01: 10 bit  (DA_XPON_RX_DATA[9:0]) /  10 bit  (DA_XFI_RX_DATA[9:0])
10: 16 bit  (DA_XPON_RX_DATA[15:0]) / 16 bit  (DA_XFI_RX_DATA[15:0])
*/

/** NOT USED *****************************************************************************************************************
static void rx_rate_ctrl(u8 xsgmii){
    rg_type_t(HAL_RG_XPON_RX_RESERVED_1) RG_XPON_RX_RESERVED_1;
	if(dbg_print) printk("rx_rate_ctrl\n");
	RG_XPON_RX_RESERVED_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RG_XPON_RX_RESERVED_1);
    if (xsgmii == 0) RG_XPON_RX_RESERVED_1.hal.rg_xpon_rx_rate_ctrl = 0b10;
    else if (xsgmii == 1) RG_XPON_RX_RESERVED_1.hal.rg_xpon_rx_rate_ctrl = 0b00;
    else if (xsgmii == 2) RG_XPON_RX_RESERVED_1.hal.rg_xpon_rx_rate_ctrl = 0b00;
	else RG_XPON_RX_RESERVED_1.hal.rg_xpon_rx_rate_ctrl = 0b10;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RG_XPON_RX_RESERVED_1,RG_XPON_RX_RESERVED_1.dat.value);
}

static void RX_REV(void){
    rg_type_t(HAL_RG_PXP_RX_REV_0) RG_PXP_RX_REV_0;
	if(dbg_print) printk("RX_REV\n");
	
	RG_PXP_RX_REV_0.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_REV_0);

    RG_PXP_RX_REV_0.hal.rg_pxp_rx_rev_1 &= ~(0x03<<2);
    RG_PXP_RX_REV_0.hal.rg_pxp_rx_rev_1 |= 0b00<<2;
    RG_PXP_RX_REV_0.hal.rg_pxp_rx_rev_1 &= ~(0x07<<4);
    RG_PXP_RX_REV_0.hal.rg_pxp_rx_rev_1 |= 0b100<<4;
    RG_PXP_RX_REV_0.hal.rg_pxp_rx_rev_1 &= ~(0x07<<8);
    RG_PXP_RX_REV_0.hal.rg_pxp_rx_rev_1 |= 0b100<<4;
    
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_REV_0,RG_PXP_RX_REV_0.dat.value);
}
static void RX_SigDet(void){
    rg_type_t(HAL_RG_PXP_RX_SIGDET_NOVTH) RG_PXP_RX_SIGDET_NOVTH;
	rg_type_t(HAL_RG_PXP_RX_DAC_RANGE) RG_PXP_RX_DAC_RANGE;
	if(dbg_print) printk("RX_SigDet\n");
	RG_PXP_RX_SIGDET_NOVTH.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_SIGDET_NOVTH);
	RG_PXP_RX_DAC_RANGE.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_DAC_RANGE);


    RG_PXP_RX_SIGDET_NOVTH.hal.rg_pxp_rx_sigdet_peak = 0b10;
    RG_PXP_RX_SIGDET_NOVTH.hal.rg_pxp_rx_sigdet_vth_sel = 0b00010;
    
    RG_PXP_RX_DAC_RANGE.hal.rg_pxp_rx_sigdet_lpf_ctrl = 0b11;
	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_SIGDET_NOVTH,RG_PXP_RX_SIGDET_NOVTH.dat.value);
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_DAC_RANGE,RG_PXP_RX_DAC_RANGE.dat.value); 
}    
    
static void RX_CDR_PR(u8 xsgmii){
    rg_type_t(HAL_RG_PXP_CDR_PR_MONPR_EN) RG_PXP_CDR_PR_MONPR_EN;
	u8 cap_en,buf_in_sr;
	if(dbg_print) printk("RX_CDR_PR\n");
	
	RG_PXP_CDR_PR_MONPR_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_MONPR_EN);

    if (xsgmii == 0){
        cap_en = 0b0;
        buf_in_sr = 0b111;
	}	
    else if (xsgmii == 1){
        cap_en = 0b1;
        buf_in_sr = 0b110;
	}	
    else if (xsgmii == 2){
        cap_en = 0b0;
        buf_in_sr = 0b111;
	}	
    else{    
        cap_en = 0b0;
        buf_in_sr = 0b111;
	}	
        
    RG_PXP_CDR_PR_MONPR_EN.hal.rg_pxp_cdr_pr_cap_en = cap_en;
    RG_PXP_CDR_PR_MONPR_EN.hal.rg_pxp_cdr_pr_buf_in_sr = buf_in_sr;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_MONPR_EN,RG_PXP_CDR_PR_MONPR_EN.dat.value); 
}
    
static void PXP_RX_PHYCK(u8 xsgmii){ //10.0.3
    rg_type_t(HAL_RG_PXP_RX_PHYCK_DIV) RG_PXP_RX_PHYCK_DIV;
	rg_type_t(HAL_RG_PXP_RX_BUSBIT_SEL) RG_PXP_RX_BUSBIT_SEL;
	u8 phyck_div,phyck_sel;
	if(dbg_print) printk("PXP_RX_PHYCK\n");
	RG_PXP_RX_PHYCK_DIV.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_PHYCK_DIV);
	RG_PXP_RX_BUSBIT_SEL.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_BUSBIT_SEL);

    
    if (xsgmii == 0){
        phyck_div = 0b01000010;
        phyck_sel = 0b10;
	}	
    else if (xsgmii == 1){
        phyck_div = 0b00001011;
        phyck_sel = 0b01;
	}	
    else if (xsgmii == 2){
        phyck_div = 0b00101001;
        phyck_sel = 0b01;
	}	
    else{    
        phyck_div = 0b01000010;
        phyck_sel = 0b10;
    }
	
    RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_tdc_ck_sel = 0x0;
    RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_phyck_sel = phyck_sel;
    RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_phyck_div = phyck_div;
    RG_PXP_RX_PHYCK_DIV.hal.rg_pxp_rx_phyck_rstb = 0x1;
    
    RG_PXP_RX_BUSBIT_SEL.hal.rg_pxp_rx_phy_ck_sel_force = 1;
    RG_PXP_RX_BUSBIT_SEL.hal.rg_pxp_rx_phy_ck_sel = 0; 
    
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_PHYCK_DIV,RG_PXP_RX_PHYCK_DIV.dat.value);
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_BUSBIT_SEL,RG_PXP_RX_BUSBIT_SEL.dat.value);
}    

static void RX_FLL_PR_FMeter(u8 bypass){ //10.0.9
    rg_type_t(HAL_RX_DISB_MODE_8) RX_DISB_MODE_8;
	rg_type_t(HAL_RX_FORCE_MODE_9) RX_FORCE_MODE_9;
	rg_type_t(HAL_PLL_TDC_FREQDET_0) PLL_TDC_FREQDET_0;
	rg_type_t(HAL_PLL_TDC_FREQDET_1) PLL_TDC_FREQDET_1;
	rg_type_t(HAL_PLL_TDC_FREQDET_3) PLL_TDC_FREQDET_3;
	if(dbg_print) printk("RX_FLL_PR_FMeter\n");

    if (bypass == 1){
		
		RX_DISB_MODE_8.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_8);
		RX_FORCE_MODE_9.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9);

        RX_DISB_MODE_8.hal.rg_disb_fbck_lock = 0 ;//dis meter
        RX_FORCE_MODE_9.hal.rg_force_fbck_lock = 1 ;//rdy
        
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_8,RX_DISB_MODE_8.dat.value);
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9,RX_FORCE_MODE_9.dat.value);	
	}	
    else{
		
		PLL_TDC_FREQDET_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PLL_TDC_FREQDET_0);
		PLL_TDC_FREQDET_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PLL_TDC_FREQDET_1);
		PLL_TDC_FREQDET_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PLL_TDC_FREQDET_3);
		
        PLL_TDC_FREQDET_0.hal.rg_pll_lock_cyclecnt = 1;
        PLL_TDC_FREQDET_1.hal.rg_pll_lock_target_beg = 0;
        PLL_TDC_FREQDET_1.hal.rg_pll_lock_target_end = 0xFFFF;
        PLL_TDC_FREQDET_3.hal.rg_pll_lock_lockth = 1;

		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PLL_TDC_FREQDET_0,PLL_TDC_FREQDET_0.dat.value);
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PLL_TDC_FREQDET_1,PLL_TDC_FREQDET_1.dat.value);
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PLL_TDC_FREQDET_3,PLL_TDC_FREQDET_3.dat.value);	
	}
        
       
}
	
static void RX_SigDet_Pwdb(u8 b_on){	
	rg_type_t(HAL_SS_DA_XPON_PWDB_1) SS_DA_XPON_PWDB_1;
	if(dbg_print) printk("RX_SigDet_Pwdb\n");
#if 1 //move to fun rx_sigdet_pwdb
	SS_DA_XPON_PWDB_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_1);
	SS_DA_XPON_PWDB_1.hal.rg_da_xpon_rx_sigdet_pwdb = b_on;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_DA_XPON_PWDB_1,SS_DA_XPON_PWDB_1.dat.value);
#endif
}

static void RX_Rdy_TimeOut(void){
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_5) RX_CTRL_SEQUENCE_CTRL_5;
	if(dbg_print) printk("RX_Rdy_TimeOut\n");
	RX_CTRL_SEQUENCE_CTRL_5.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_5);
	RX_CTRL_SEQUENCE_CTRL_5.hal.rg_rx_rdy = 0xA;	
	RX_CTRL_SEQUENCE_CTRL_5.hal.rg_rx_blwc_rdy_en = 0x5; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_5,RX_CTRL_SEQUENCE_CTRL_5.dat.value);
}

static void RX_CalBoundry_Init(void){
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_0) RX_CTRL_SEQUENCE_CTRL_0;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_6) RX_CTRL_SEQUENCE_CTRL_6;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_1) RX_CTRL_SEQUENCE_CTRL_1;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_4) RX_CTRL_SEQUENCE_CTRL_4;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_2) RX_CTRL_SEQUENCE_CTRL_2;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_CTRL_3) RX_CTRL_SEQUENCE_CTRL_3;
	
	if(dbg_print) printk("RX_CalBoundry_Init\n");
	
#if 1 //use default  value for cal. start and end move to RX calbration init
	RX_CTRL_SEQUENCE_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_0);
	RX_CTRL_SEQUENCE_CTRL_0.hal.rg_rx_os_start = 0x1;
	RX_CTRL_SEQUENCE_CTRL_0.hal.rg_osc_speed_opt = 0x1; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_0,RX_CTRL_SEQUENCE_CTRL_0.dat.value);

	RX_CTRL_SEQUENCE_CTRL_6.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_6);
	RX_CTRL_SEQUENCE_CTRL_6.hal.rg_rx_os_end = 0x2; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_6,RX_CTRL_SEQUENCE_CTRL_6.dat.value);
	RX_CTRL_SEQUENCE_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_1);
	RX_CTRL_SEQUENCE_CTRL_1.hal.rg_rx_pical_end = 0x32; 
	RX_CTRL_SEQUENCE_CTRL_1.hal.rg_rx_pical_start = 0x2; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_1,RX_CTRL_SEQUENCE_CTRL_1.dat.value);
	
	RX_CTRL_SEQUENCE_CTRL_4.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_4);
	RX_CTRL_SEQUENCE_CTRL_4.hal.rg_rx_sdcal_end = 0x32; 
	RX_CTRL_SEQUENCE_CTRL_4.hal.rg_rx_sdcal_start = 0x2; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_4,RX_CTRL_SEQUENCE_CTRL_4.dat.value);
	
	RX_CTRL_SEQUENCE_CTRL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_2);
	RX_CTRL_SEQUENCE_CTRL_2.hal.rg_rx_pdos_end = 0x32;	
	RX_CTRL_SEQUENCE_CTRL_2.hal.rg_rx_pdos_start = 0x2;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_2,RX_CTRL_SEQUENCE_CTRL_2.dat.value);
	
	RX_CTRL_SEQUENCE_CTRL_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_3);
	RX_CTRL_SEQUENCE_CTRL_3.hal.rg_rx_feos_end = 0x32;	
	RX_CTRL_SEQUENCE_CTRL_3.hal.rg_rx_feos_start = 0x2; 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_CTRL_3,RX_CTRL_SEQUENCE_CTRL_3.dat.value);
#endif
}	

static void RX_INIT(u8 xsgmii){
	if(dbg_print) printk("RX_INIT\n");
	rx_rate_ctrl(xsgmii); //10.0.2
	RX_Path_Init();	
	RX_FLL_PR_FMeter(0);//10.0.6
	RX_REV();//10.0.4
	RX_Rdy_TimeOut();
	RX_CalBoundry_Init();	
	RX_BySerdes(xsgmii);
	RX_EYE_Mon();	
	
	//calibration start
	RX_SYS_En(ON);	
	udelay(500);
	
	RX_FLL_PR_FMeter(1);//10.0.6	
	RX_Cal_En(ON);
	RX_CMLEQ_EN(ON);
	RX_CDR_PR(xsgmii); //10.0.2
	RX_CDR_xxx_Pwdb(B_ON);
	RX_SigDet();
	RX_SigDet_Pwdb(B_ON);
	//udelay(100);
	
	PXP_RX_PHYCK(xsgmii);
	udelay(100);
}

static void RX_BringUp(u8 xsgmii){		
	if(dbg_print) printk("RX BringUp %x\n",xsgmii);
	RX_INIT(xsgmii);
	RX_CDR_xxx_Pwdb(B_OFF);
	RX_SigDet_Pwdb(B_OFF);
}	

static void RSTB(u8 sel,u8 val){
	rg_type_t(HAL_SW_RST_SET) SW_RST_SET; 	
	SW_RST_SET.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SW_RST_SET);

		switch(sel){
			case eALLPCS:
				SW_RST_SET.hal.rg_sw_allpcs_rst_n = val; 
				break;
			case eRXPCS:
				SW_RST_SET.hal.rg_sw_xfi_rxpcs_rst_n = val;	
				break;
			case eTXPCS:			
				SW_RST_SET.hal.rg_sw_xfi_txpcs_rst_n = val; 
				break;
			case eTXFIFO:
				SW_RST_SET.hal.rg_sw_tx_fifo_rst_n = val; 
				break;
			case eREF:			
				SW_RST_SET.hal.rg_sw_ref_rst_n = val; 
				break;
			case ePMA:			
				SW_RST_SET.hal.rg_sw_pma_rst_n = val; 
				break;	
			case eRX:			
				SW_RST_SET.hal.rg_sw_rx_rst_n = val; 			
				break;	
			case eTX:			
				SW_RST_SET.hal.rg_sw_tx_rst_n = val;	
				break;
			case eM1:
				SW_RST_SET.hal.rg_sw_xfi_rxpcs_rst_n = val;	
				SW_RST_SET.hal.rg_sw_rx_fifo_rst_n = val;	 //10.0.8			
				SW_RST_SET.hal.rg_sw_tx_fifo_rst_n = val; 
				SW_RST_SET.hal.rg_sw_xfi_txpcs_rst_n = val; 
				SW_RST_SET.hal.rg_sw_ref_rst_n = 1; 
				SW_RST_SET.hal.rg_sw_allpcs_rst_n = val; 
				SW_RST_SET.hal.rg_sw_pma_rst_n = val; 
				SW_RST_SET.hal.rg_sw_tx_rst_n = val;	
				SW_RST_SET.hal.rg_sw_rx_rst_n = val; 			

				SW_RST_SET.hal.rg_sw_xfi_rxpcs_bist_rst_n = val;
		 		SW_RST_SET.hal.rg_sw_hsg_txpcs_rst_n = val;
 				SW_RST_SET.hal.rg_sw_hsg_rxpcs_rst_n = val;
				break;	
			default:
				SW_RST_SET.hal.rg_sw_xfi_rxpcs_rst_n = val;	
				SW_RST_SET.hal.rg_sw_rx_fifo_rst_n = val;	 //10.0.8
				SW_RST_SET.hal.rg_sw_tx_fifo_rst_n = val; 
				SW_RST_SET.hal.rg_sw_xfi_txpcs_rst_n = val; 
				SW_RST_SET.hal.rg_sw_ref_rst_n = 1; 
				SW_RST_SET.hal.rg_sw_allpcs_rst_n = val; 
				SW_RST_SET.hal.rg_sw_pma_rst_n = val; 
				SW_RST_SET.hal.rg_sw_tx_rst_n = val;	
				SW_RST_SET.hal.rg_sw_rx_rst_n = val; 			

				SW_RST_SET.hal.rg_sw_xfi_rxpcs_bist_rst_n = val;
		 		SW_RST_SET.hal.rg_sw_hsg_txpcs_rst_n = val;
 				SW_RST_SET.hal.rg_sw_hsg_rxpcs_rst_n = val;			
				break;
		}
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SW_RST_SET,SW_RST_SET.dat.value);	
}	

static void TX_CONFIG(u8 ckin_sel,u8 ckin_divisor,u8 rate_ctrl){	
	rg_type_t(HAL_rg_force_da_pxp_tx_acjtag_en) rg_force_da_pxp_tx_acjtag_en;
	rg_type_t(HAL_rg_force_da_pxp_tx_term_sel) rg_force_da_pxp_tx_term_sel;
	rg_type_t(HAL_rg_force_da_pxp_tx_rate_ctrl) rg_force_da_pxp_tx_rate_ctrl;
	//rg_type_t(HAL_RG_PXP_CMN_EN) RG_PXP_CMN_EN;
	rg_type_t(HAL_RG_PXP_TX_CKLDO_EN) RG_PXP_TX_CKLDO_EN;
	if(dbg_print) printk("TX_CONFIG\n");
	//rg_type_t(HAL_SS_TX_RST_B) SS_TX_RST_B;
//V11
	RG_PXP_TX_CKLDO_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TX_CKLDO_EN);
	RG_PXP_TX_CKLDO_EN.hal.rg_pxp_tx_dmedgegen_en = 0x1;		
	RG_PXP_TX_CKLDO_EN.hal.rg_pxp_tx_ckldo_en = 0x1;	
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_TX_CKLDO_EN,RG_PXP_TX_CKLDO_EN.dat.value);

//Kris//V10	
	//RG_PXP_CMN_EN.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CMN_EN);
	//RG_PXP_CMN_EN.hal.rg_pxp_cmn_en = 0x1;	
	//RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CMN_EN,RG_PXP_CMN_EN.dat.value);
	udelay(1);
		
	rg_force_da_pxp_tx_acjtag_en.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_acjtag_en);
	rg_force_da_pxp_tx_acjtag_en.hal.rg_force_sel_da_pxp_tx_ckin_sel = 0x1;
	rg_force_da_pxp_tx_acjtag_en.hal.rg_force_da_pxp_tx_ckin_sel = ckin_sel;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_acjtag_en,rg_force_da_pxp_tx_acjtag_en.dat.value);

	rg_force_da_pxp_tx_term_sel.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_term_sel);
	rg_force_da_pxp_tx_term_sel.hal.rg_force_sel_da_pxp_tx_term_sel = 1;
    rg_force_da_pxp_tx_term_sel.hal.rg_force_da_pxp_tx_term_sel = 0b01;
	rg_force_da_pxp_tx_term_sel.hal.rg_force_sel_da_pxp_tx_ckin_divisor = 0x1;
	rg_force_da_pxp_tx_term_sel.hal.rg_force_da_pxp_tx_ckin_divisor = ckin_divisor;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_term_sel,rg_force_da_pxp_tx_term_sel.dat.value);

	rg_force_da_pxp_tx_rate_ctrl.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_rate_ctrl);
	rg_force_da_pxp_tx_rate_ctrl.hal.rg_force_sel_da_pxp_tx_rate_ctrl = 0x1;
	rg_force_da_pxp_tx_rate_ctrl.hal.rg_force_da_pxp_tx_rate_ctrl = rate_ctrl;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_rate_ctrl,rg_force_da_pxp_tx_rate_ctrl.dat.value);
#if 0 //move to void static TX_RSTB(u8 b_on){
 	SS_TX_RST_B.dat.value = RG_R_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_TX_RST_B);
	SS_TX_RST_B.hal.txcalib_rst_b = 0x1;
	SS_TX_RST_B.hal.tx_top_rst_b = 0x1;	
	RG_W_PL(_pxSGMII->phya_base,PMA_BASE_OFFSET,(RgAddr) _SS_TX_RST_B,SS_TX_RST_B.dat.value);
#endif
}
void static TX_FIR(u8 cn1,u8 c0b,u8 c1,u8 c2){
	rg_type_t(HAL_rg_force_da_pxp_tx_fir_c0b) rg_force_da_pxp_tx_fir_c0b;	
	rg_type_t(HAL_rg_force_da_pxp_tx_fir_c1) rg_force_da_pxp_tx_fir_c1;
	if(dbg_print) printk("TX_FIR\n");
	rg_force_da_pxp_tx_fir_c0b.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_fir_c0b);
	rg_force_da_pxp_tx_fir_c0b.hal.rg_force_sel_da_pxp_tx_fir_cn1 = 1;
	rg_force_da_pxp_tx_fir_c0b.hal.rg_force_sel_da_pxp_tx_fir_c0b = 1;	
	rg_force_da_pxp_tx_fir_c0b.hal.rg_force_da_pxp_tx_fir_cn1 = cn1;
	rg_force_da_pxp_tx_fir_c0b.hal.rg_force_da_pxp_tx_fir_c0b = c0b;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_fir_c0b,rg_force_da_pxp_tx_fir_c0b.dat.value);

	
	rg_force_da_pxp_tx_fir_c1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_fir_c1);
	rg_force_da_pxp_tx_fir_c1.hal.rg_force_sel_da_pxp_tx_fir_c1 = 1;
	rg_force_da_pxp_tx_fir_c1.hal.rg_force_sel_da_pxp_tx_fir_c2 = 0;	
	rg_force_da_pxp_tx_fir_c1.hal.rg_force_da_pxp_tx_fir_c1 = c1;
	rg_force_da_pxp_tx_fir_c1.hal.rg_force_da_pxp_tx_fir_c2 = c2;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_fir_c1,rg_force_da_pxp_tx_fir_c1.dat.value);
}
void static TX_RSTB(u8 b_on){
	
	rg_type_t(HAL_SS_TX_RST_B) SS_TX_RST_B;
	if(dbg_print) printk("TX_RSTB\n");
 	SS_TX_RST_B.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_TX_RST_B);
	SS_TX_RST_B.hal.txcalib_rst_b = b_on;
	SS_TX_RST_B.hal.tx_top_rst_b = b_on;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_TX_RST_B,SS_TX_RST_B.dat.value);
}

static void tx_rate_ctrl(u8 xsgmii){
    rg_type_t(HAL_xpon_tx_rate_ctrl) xpon_tx_rate_ctrl;
	if(dbg_print) printk("tx_rate_ctrl %x\n",xsgmii);
    xpon_tx_rate_ctrl.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _xpon_tx_rate_ctrl);

    if (xsgmii == 0) xpon_tx_rate_ctrl.hal.rg_pon_tx_rate_ctrl = 0b10;
    else if (xsgmii == 1) xpon_tx_rate_ctrl.hal.rg_pon_tx_rate_ctrl = 0b01;
    else if (xsgmii == 2) xpon_tx_rate_ctrl.hal.rg_pon_tx_rate_ctrl = 0b01;
    else xpon_tx_rate_ctrl.hal.rg_pon_tx_rate_ctrl = 0b10;
    
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _xpon_tx_rate_ctrl,xpon_tx_rate_ctrl.dat.value);
}


static void TX_BringUp(u8 xsgmii){
	if(dbg_print) printk("TX BringUp %x\n",xsgmii);
	tx_rate_ctrl(xsgmii);
	switch (xsgmii){
		case USXGMII:
			TX_CONFIG(1,0b101,0b10);
			TX_FIR(0,0,0,0);
			break;
		case HSGMII:
			TX_CONFIG(1,0b100,0b01);
			TX_FIR(0,0,0,0);
			break;
		case SGMII:
			TX_CONFIG(1,0b010,0b01);
			TX_FIR(0,10,0,0);
			break;
		default:
			printk("warring!!! no such selection %x\n",xsgmii);
			printk("please input 0 or 1 or 2\n");
			TX_CONFIG(1,0b101,0b10);
			TX_FIR(0,0,0,0);
			break;			
	}	
	TX_RSTB(B_OFF);
}

static void PLL_BringUp(u8 xsgmii){
	JCPLL_BringUp(xsgmii);
	//JCPLL_RSTB(0);
	udelay(200); //10.0.9
	TXPLL_BringUp(xsgmii);
	//TXPLL_RSTB(0);
	udelay(200); //10.0.9
}
static void Phya_BringUp(u8 xsgmii){	
	TX_BringUp(xsgmii);	
	RX_BringUp(xsgmii); 
	udelay(100);//10.0.9	
	RSTB(eM1,B_ON);
	udelay(100);//10.0.9	
	RSTB(eM1,B_OFF);	
	udelay(100);
}
*******************************************************************************************************************/


static void xSGMII_AN_AutoSetting(u8 xsgmii,u8 mode,u8 duplx,u8 rate){


	rg_type_t(HAL_sgmii_reg_an_force_cl37)sgmii_reg_an_force_cl37;
	rg_type_t(HAL_sgmii_reg_an_13) sgmii_reg_an_13;
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_6) rg_hsgmii_pcs_ctrol_6;
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;	

	#ifdef FPGA_PLATFORM
	/*
		RGDATA_t rg={0};
		if(dbg_print) printk("xSGMII_AN_AutoSetting( %x,%x,%x,%x)\n",xsgmii, mode, duplx, rate);
		if(xsgmii == USXGMII){
			rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL); 
			rg.bit.b24 = 1; //bitfile0714
			RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL,rg.value); 			
		}
	*/	
	#endif 
	if(dbg_print) printk("xSGMII_AN_AutoSetting\n");


	if (xsgmii == SGMII){		
					
		if (mode){

			/* 0x1FA80060 0x1 */
			sgmii_reg_an_force_cl37.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_force_cl37);
			sgmii_reg_an_force_cl37.hal.rg_force_an_done = 1;
			RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an_force_cl37,sgmii_reg_an_force_cl37.dat.value);


			switch (rate){
			case ENUM_SGMII_1000M :
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x9801:0x8801);
				break;
			case ENUM_SGMII_100M :
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x9401:0x8401);
				break;
			case ENUM_SGMII_10M :
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x9001:0x8001);
				break;	
			default :			
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_4,duplx? 0x9801:0x8801);
				break;
			}
			
		}

		/* 0x1FA80034 0x3112000B */
		sgmii_reg_an_13.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13);
		sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x0b;
		RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,sgmii_reg_an_13.dat.value);

		rg_hsgmii_pcs_ctrol_6.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_6);	
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_tx_enable = 1;
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_force_rateadapt = 1;
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_mode2_en = 1;
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_mac_mode = mode; 
		RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_6,rg_hsgmii_pcs_ctrol_6.dat.value);	

	}
}

/** NOT USED *****************************************************************************************************************
static void usxgmii_an(u8 enable)
{	
	RGDATA_t rg={0}; 
	if(dbg_print) printk("usxgmii_an\n");
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_0);

	//if ((rg.bit.b0 != enable))
	{
		rg.bit.b0 = enable;
//move to an restart
//		rg.bit.b8 = 1;
//		RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_0,rg.value);	
//		udelay(3);
//		rg.bit.b8 = 0;
		RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_0,rg.value);	

		//dev ability //move to  10G~100M function
#if 0
		rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_1) & ~(6<<8); 
		rg.value |= 6<<8;
		RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_1,rg.value);
#endif

		//move to 10G~100M
#if 0		
		rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
		rg.bit.b8 = 0; //10G =1
		RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,rg.value);
#endif
	}
	if(dbg_print) printk("_rg_usxgmii_an_control_0 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_0));
	if(dbg_print) printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
}

#ifdef	FPGA_PLATFORM	
static u8 fpga_clk_for_usxgmii_rate(u8 rate){
	RGDATA_t rg={0};
	if(dbg_print) printk("fpga_clk_for_usxgmii_rate %x\n",rate);
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL); 

	#if 0 //chk LP rate
		rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6);
		rg.bit.b0 = 1;
		RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6,rg.value);		
		rg.bit.b0 = 0;
		RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6,rg.value);
	#endif	
	
	switch (rate){
		case ENUM_USXGII_2p5G :
		case ENUM_USXGII_100M :
			rg.bit.b28 = 1;
			break;
		default :
			rg.bit.b28 = 0;
			break;
	}
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL,rg.value);				
	return(u8)rg.bit.b28;
}
#endif
*******************************************************************************************************************/


static void sgmii_an(u8 enable){

	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	if(dbg_print) printk("sgmii_an %x\n",enable);
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	sgmii_reg_an0.hal.sgmii_an_enable = enable;
	RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,sgmii_reg_an0.dat.value); 

	
} 

static void hsgmii_an(u8 enable){
	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	if(dbg_print) printk("sgmii_an %x\n",enable);
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	sgmii_reg_an0.hal.sgmii_an_enable = enable;
	RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,sgmii_reg_an0.dat.value);
}

/** NOT USED *****************************************************************************************************************
static void usxgmii_init(void){
	RGDATA_t rg;	
	if(dbg_print) printk("usxgmii_init\n");
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x100); 
	rg.bit.b28 = 1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x100,rg.value);		
	if(dbg_print) printk("1fa84100 %x\n",RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET, (RgAddr)0x100));
}
*******************************************************************************************************************/

static void hsgmii_init(void){
	if(dbg_print) printk("hsgmii_init\n");

	UINT32 tmp;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0 fw_PON_RXFEDIG_CTRL_0;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_1 fw_CDR_CTRL_1;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_0 fw_CDR_CTRL_0;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_3 fw_CDR_CTRL_3;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_5 fw_CDR_CTRL_5;
	 REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS;
	 REG_RG_SSUSB_LN0_CDR_RESERVE fw_RG_SSUSB_LN0_CDR_RESERVE;
	 REG_RG_SSUSB_PLL_KBAND_KFC fw_RG_SSUSB_PLL_KBAND_KFC;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0 fw_PON_DA_CTRL_0;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19 fw_PON_DA_CTRL_19;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1 fw_PON_DA_CTRL_1;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4 fw_PON_DA_CTRL_4;
	 pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10 fw_PON_SERDES_CTRL_10;
	 pon_hsgmii_dig_csr_REG_FRQ_CTRL_2 fw_FRQ_CTRL_2;
	 pon_hsgmii_dig_csr_REG_FRQ_CTRL_4 fw_FRQ_CTRL_4;
	 pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0 fw_PON_SYS_CTRL_0;
	 pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0 fw_HG_RST_CTRL_0;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2 fw_PON_DA_CTRL_2;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12 fw_PON_RXFEDIG_CTRL_12;
	 pon_hsgmii_dig_csr_REG_PON_OSR_SEL_CTRL fw_PON_OSR_SEL_CTRL;
	 pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0 fw_HG_MODE_CTRL_0;
	 pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2 fw_SS_LCPLL_TDC_FLT_2;
	 pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5 fw_SS_LCPLL_TDC_FLT_5;
	 pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3 fw_SS_LCPLL_TDC_FLT_3;
	 REG_RG_SSUSB_PLL_SDM_HREN fw_RG_SSUSB_PLL_SDM_HREN;	 
	 pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0 fw_PON_SERDES_CTRL_0;
	 REG_RG_PCIE_CLKDRV_OFFSET fw_RG_PCIE_CLKDRV_OFFSET;



	 fw_PON_RXFEDIG_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0);
	 fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_RX500M_CK_SEL = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2004409

	 fw_CDR_CTRL_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_3);
	 fw_CDR_CTRL_3.Bits.rg_bc_ltd0_mode_0 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_3, fw_CDR_CTRL_3.Raw); //write addr: 0x1FAF4330 = 0x1771C1E

	 fw_CDR_CTRL_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_5);
	 fw_CDR_CTRL_5.Bits.rg_bc_ltd1_mode_0 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_5, fw_CDR_CTRL_5.Raw); //write addr: 0x1FAF4338 = 0x1771C1E

	 fw_CDR_CTRL_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_1);
	 fw_CDR_CTRL_1.Bits.rg_bc_ltr_mode_0 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_1, fw_CDR_CTRL_1.Raw); // write addr: 0x1FAF4328 = 0x1771C1E

	 fw_CDR_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_0);
	 fw_CDR_CTRL_0.Bits.rg_bic_ltr_mode_1 = 0xF; 
	 fw_CDR_CTRL_0.Bits.rg_bir_ltr_mode_0 = 0x10; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_0, fw_CDR_CTRL_0.Raw); // write addr: 0x1FAF4324 = 0xF60710

	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS);
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_CDR_EPEN = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS, fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw); // write addr: 0x1FAF3024 = 0x18000702

	 fw_RG_SSUSB_LN0_CDR_RESERVE.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_RESERVE);
	 fw_RG_SSUSB_LN0_CDR_RESERVE.Bits.RG_SSUSB_LN0_CDR_RESERVE = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_RESERVE, fw_RG_SSUSB_LN0_CDR_RESERVE.Raw); // write addr: 0x1FAF3018 = 0x100

	 fw_RG_SSUSB_PLL_KBAND_KFC.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_KBAND_KFC);
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOPFD_VSEL = 0x0; 
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOCHP_VSEL = 0x0; 
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOLPF_VSEL = 0x1; 
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOVCO_VSEL = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_KBAND_KFC, fw_RG_SSUSB_PLL_KBAND_KFC.Raw); // write addr: 0x1FAF3010 = 0x1F910001

	 fw_PON_DA_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0);
	 fw_PON_DA_CTRL_0.Bits.RG_SSUSB_PLL_IR = 0x9; 
	 fw_PON_DA_CTRL_0.Bits.RG_SSUSB_PLL_BR = 0x7; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0, fw_PON_DA_CTRL_0.Raw); // write addr: 0x1FAF4600 = 0x779E1012

	 fw_PON_DA_CTRL_19.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19);
	 fw_PON_DA_CTRL_19.Bits.RG_SSUSB_PLL_BPA = 0x3; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19, fw_PON_DA_CTRL_19.Raw); // write addr: 0x1FAF464C = 0x13

	 fw_PON_DA_CTRL_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1);
	 fw_PON_DA_CTRL_1.Bits.RG_SSUSB_PLL_BC = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1, fw_PON_DA_CTRL_1.Raw); // write addr: 0x1FAF4604 = 0x30004010

	 fw_PON_DA_CTRL_4.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4);
	 fw_PON_DA_CTRL_4.Bits.RG_SSUSB_PLL_FBKDIV = 0x3E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4, fw_PON_DA_CTRL_4.Raw); // write addr: 0x1FAF4610 = 0x3E

	 fw_PON_SERDES_CTRL_10.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10);
	 fw_PON_SERDES_CTRL_10.Bits.rg_tx_bit_polarity = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10, fw_PON_SERDES_CTRL_10.Raw); // write addr: 0x1FAF4228 = 0x3



//*******  //freq meter
	 fw_FRQ_CTRL_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_2);
	 fw_FRQ_CTRL_2.Bits.rg_hg_tx_freq_det_mux = 0x0; 
	 fw_FRQ_CTRL_2.Bits.rg_hg_rx_freq_det_mux = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_2, fw_FRQ_CTRL_2.Raw); // write addr: 0x1FAF4364 = 0x3003

	 fw_FRQ_CTRL_4.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_4);
	 fw_FRQ_CTRL_4.Bits.rg_hg_tx_freq_cnt = 0x618; 
	 fw_FRQ_CTRL_4.Bits.rg_hg_rx_freq_cnt = 0x618; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_4, fw_FRQ_CTRL_4.Raw); // write addr: 0x1FAF436C = 0x6180618

	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS);
#if RX_EYE_SCAN
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_CDR_EPEN = 0x1; 
#else
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_CDR_EPEN = 0x0; 
#endif
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS, fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw); //write addr: 0x1FAF3024 = 0x18000702



//*******    //CK EN
	 fw_PON_SYS_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0);
	 fw_PON_SYS_CTRL_0.Bits.rg_pon_tx250m_ck_en = 0x0; 
	 fw_PON_SYS_CTRL_0.Bits.rg_pon_rx500m_ck_en = 0x0; 
	 fw_PON_SYS_CTRL_0.Bits.rg_hg_tx250m_ck_en = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_hg_rx500m_ck_en = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_baser_txck_en = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_baser_rxck_en = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_hg_rx_pma_clk_div_sel = 0x0; 
	 fw_PON_SYS_CTRL_0.Bits.rg_rx_pma_clk_sel = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_hg_tx_pma_clk_div_sel = 0x0; 
	 fw_PON_SYS_CTRL_0.Bits.rg_tx_pma_clk_sel = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_pon_rx_pma_clk_div_sel = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0, fw_PON_SYS_CTRL_0.Raw); // write addr: 0x1FAF4300 = 0x2C0003C0



//*******    //RST EN
	 fw_HG_RST_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0);
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pma_tx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pma_rx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pcs_tx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pcs_rx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_mac_tx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_mac_rx_reset_n = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0, fw_HG_RST_CTRL_0.Raw); // write addr: 0x1FAF4310 = 0x3703F

	 fw_PON_DA_CTRL_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2);
#if RX_EYE_SCAN
	 fw_PON_DA_CTRL_2.Bits.RG_SSUSB_CDR_PI_PWD = 0x0; 
#else
	 fw_PON_DA_CTRL_2.Bits.RG_SSUSB_CDR_PI_PWD = 0x1; 
#endif
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2, fw_PON_DA_CTRL_2.Raw); //write addr: 0x1FAF4608 = 0x56101801

	 fw_PON_RXFEDIG_CTRL_12.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12);
	 tmp = fw_PON_RXFEDIG_CTRL_12.Bits.RG_SSUSB_EQ_REV;
	 tmp = (0x0<<6) | (tmp & (~(0x1<<6)));
	 fw_PON_RXFEDIG_CTRL_12.Bits.RG_SSUSB_EQ_REV = tmp;
	 fw_PON_RXFEDIG_CTRL_12.Bits.RG_SSUSB_EQ_REV = 0x38; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12, fw_PON_RXFEDIG_CTRL_12.Raw); // write addr: 0x1FAF4530 = 0x380013

	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS);
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_RX_DAC_PWD = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS, fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw); //write addr: 0x1FAF3024 = 0x10000702

	 fw_RG_PCIE_CLKDRV_OFFSET.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_PCIE_CLKDRV_OFFSET);
	 tmp = fw_RG_PCIE_CLKDRV_OFFSET.Bits.RG_SSUSB_XTAL_TOP_RESERVE;
	 tmp = (0x1<<6) | (tmp & (~(0x1<<6)));
	 fw_RG_PCIE_CLKDRV_OFFSET.Bits.RG_SSUSB_XTAL_TOP_RESERVE = tmp;
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_PCIE_CLKDRV_OFFSET, fw_RG_PCIE_CLKDRV_OFFSET.Raw); //write addr: 0x1FAF3008 = 0x2400542

	 fw_PON_OSR_SEL_CTRL.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_OSR_SEL_CTRL);
	 fw_PON_OSR_SEL_CTRL.Bits.rg_tx_osr_sel = 0x1; 
	 fw_PON_OSR_SEL_CTRL.Bits.rg_rx_osr_sel = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_OSR_SEL_CTRL, fw_PON_OSR_SEL_CTRL.Raw); // write addr: 0x1FAF4320 = 0x11

	 fw_HG_MODE_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0);
	 fw_HG_MODE_CTRL_0.Bits.rg_serdes_mode = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0, fw_HG_MODE_CTRL_0.Raw); // write addr: 0x1FAF4314 = 0x1



//*******    //NCPO
	 fw_PON_DA_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0);
	 fw_PON_DA_CTRL_0.Bits.RG_SSUSB_PLL_FBKSEL = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0, fw_PON_DA_CTRL_0.Raw); // write addr: 0x1FAF4600 = 0x779E0012

	 fw_SS_LCPLL_TDC_FLT_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2);
	 fw_SS_LCPLL_TDC_FLT_2.Bits.rg_lcpll_ncpo_value = 0x7D000000; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2, fw_SS_LCPLL_TDC_FLT_2.Raw); // write addr: 0x1FAF4130 = 0x7D000000

	 fw_SS_LCPLL_TDC_FLT_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5);
	 fw_SS_LCPLL_TDC_FLT_5.Bits.rg_lcpll_ncpo_chg = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5, fw_SS_LCPLL_TDC_FLT_5.Raw); // write addr: 0x1FAF413C = 0x10100

	 fw_SS_LCPLL_TDC_FLT_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3);
	 fw_SS_LCPLL_TDC_FLT_3.Bits.rg_lcpll_ncpo_load = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3, fw_SS_LCPLL_TDC_FLT_3.Raw); // write addr: 0x1FAF4134 = 0x20000100

	 fw_SS_LCPLL_TDC_FLT_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5);
	 fw_SS_LCPLL_TDC_FLT_5.Bits.rg_lcpll_ncpo_chg = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5, fw_SS_LCPLL_TDC_FLT_5.Raw); // write addr: 0x1FAF413C = 0x1010100

	 fw_RG_SSUSB_PLL_SDM_HREN.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_SDM_HREN);
	 tmp = fw_RG_SSUSB_PLL_SDM_HREN.Bits.RG_SSUSB_PLL_RESERVE;
	 tmp = (0x1<<1) | (tmp & (~(0x1<<1)));
	 fw_RG_SSUSB_PLL_SDM_HREN.Bits.RG_SSUSB_PLL_RESERVE = tmp;
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_SDM_HREN, fw_RG_SSUSB_PLL_SDM_HREN.Raw); // write addr: 0x1FAF3014 = 0x1000C87
 
	 fw_PON_SERDES_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0);
	 fw_PON_SERDES_CTRL_0.Bits.rg_phya_auto_init = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0, fw_PON_SERDES_CTRL_0.Raw); // write addr: 0x1FAF4200 = 0x100381

		
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_1) rg_hsgmii_pcs_ctrol_1;	
	RGDATA_t rg;	
	

	/* PCS Setting 0x0A00  0x0c9cc000, bit[30]=0 PCS TX run as 20bit mode (deafult) */
	rg_hsgmii_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1);
	rg_hsgmii_pcs_ctrol_1.hal.rg_tbi_10b_mode = 0;
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1,rg_hsgmii_pcs_ctrol_1.dat.value);

	/*
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
	rg.bit.b2 = 1;
	rg.bit.b4 = 1;
	rg.bit.b5 = 1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);
	*/
	
	if(dbg_print) printk("hsgmii_init\n");
}



static void sgmii_init(void){
	if(dbg_print) printk("sgmii_init\n");

	UINT32 tmp;

	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0 fw_PON_RXFEDIG_CTRL_0;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_5 fw_CDR_CTRL_5;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_4 fw_CDR_CTRL_4;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_0 fw_CDR_CTRL_0;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_1 fw_CDR_CTRL_1;
	 pon_hsgmii_dig_csr_REG_CDR_CTRL_3 fw_CDR_CTRL_3;	 
	 REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS;
	 REG_RG_SSUSB_LN0_CDR_RESERVE fw_RG_SSUSB_LN0_CDR_RESERVE;
	 REG_RG_SSUSB_PLL_KBAND_KFC fw_RG_SSUSB_PLL_KBAND_KFC;	 
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0 fw_PON_DA_CTRL_0;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19 fw_PON_DA_CTRL_19;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1 fw_PON_DA_CTRL_1;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4 fw_PON_DA_CTRL_4;
	 pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10 fw_PON_SERDES_CTRL_10;
	 pon_hsgmii_dig_csr_REG_FRQ_CTRL_2 fw_FRQ_CTRL_2;
	 pon_hsgmii_dig_csr_REG_FRQ_CTRL_4 fw_FRQ_CTRL_4;
	 pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0 fw_PON_SYS_CTRL_0;
	 pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0 fw_HG_RST_CTRL_0;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2 fw_PON_DA_CTRL_2;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12 fw_PON_RXFEDIG_CTRL_12;
	 REG_RG_PCIE_CLKDRV_OFFSET fw_RG_PCIE_CLKDRV_OFFSET;

	 pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0 fw_HG_MODE_CTRL_0;
	 pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2 fw_SS_LCPLL_TDC_FLT_2;
	 pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5 fw_SS_LCPLL_TDC_FLT_5;
	 pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3 fw_SS_LCPLL_TDC_FLT_3;
	 REG_RG_SSUSB_PLL_SDM_HREN fw_RG_SSUSB_PLL_SDM_HREN;	 
	 pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0 fw_PON_SERDES_CTRL_0;

	 fw_PON_RXFEDIG_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0);
	 fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_RX500M_CK_SEL = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2004409

	 fw_CDR_CTRL_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_5);
	 fw_CDR_CTRL_5.Bits.rg_bc_ltd1_mode_1 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_5, fw_CDR_CTRL_5.Raw); // write addr: 0x1FAF4338 = 0x1771E1A

	 fw_CDR_CTRL_4.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_4);
	 fw_CDR_CTRL_4.Bits.rg_bic_ltd1_mode_0 = 0x2; 
	 fw_CDR_CTRL_4.Bits.rg_bir_ltd1_mode_0 = 0x4; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_4, fw_CDR_CTRL_4.Raw); // write addr: 0x1FAF4334 = 0x320204

	 fw_CDR_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_0);
	 fw_CDR_CTRL_0.Bits.rg_bir_ltr_mode_1 = 0x10; 
	 fw_CDR_CTRL_0.Bits.rg_bic_ltr_mode_0 = 0xF; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_0, fw_CDR_CTRL_0.Raw); // write addr: 0x1FAF4324 = 0xCF1005

	 fw_CDR_CTRL_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_1);
	 fw_CDR_CTRL_1.Bits.rg_bc_ltr_mode_1 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_1, fw_CDR_CTRL_1.Raw); // write addr: 0x1FAF4328 = 0x1771E1A

	 fw_CDR_CTRL_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_3);
	 fw_CDR_CTRL_3.Bits.rg_bc_ltd0_mode_0 = 0x1E; 
	 fw_CDR_CTRL_3.Bits.rg_bc_ltd0_mode_1 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_3, fw_CDR_CTRL_3.Raw); // write addr: 0x1FAF4330 = 0x1771E1E

	 fw_CDR_CTRL_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_5);
	 fw_CDR_CTRL_5.Bits.rg_bc_ltd1_mode_0 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_5, fw_CDR_CTRL_5.Raw); // write addr: 0x1FAF4338 = 0x1771E1E

	 fw_CDR_CTRL_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_1);
	 fw_CDR_CTRL_1.Bits.rg_bc_ltr_mode_0 = 0x1E; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_1, fw_CDR_CTRL_1.Raw); // write addr: 0x1FAF4328 = 0x1771E1E

	 fw_CDR_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_0);
	 fw_CDR_CTRL_0.Bits.rg_bic_ltr_mode_1 = 0xF; 
	 fw_CDR_CTRL_0.Bits.rg_bir_ltr_mode_0 = 0xC; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_CDR_CTRL_0, fw_CDR_CTRL_0.Raw); // write addr: 0x1FAF4324 = 0xFF100C

	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS);
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_CDR_EPEN = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS, fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw); // write addr: 0x1FAF3024 = 0x18000702

	 fw_RG_SSUSB_LN0_CDR_RESERVE.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_RESERVE);
	 fw_RG_SSUSB_LN0_CDR_RESERVE.Bits.RG_SSUSB_LN0_CDR_RESERVE = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_RESERVE, fw_RG_SSUSB_LN0_CDR_RESERVE.Raw); // write addr: 0x1FAF3018 = 0x100

	 fw_RG_SSUSB_PLL_KBAND_KFC.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_KBAND_KFC);
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOPFD_VSEL = 0x0; 
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOCHP_VSEL = 0x0; 
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOLPF_VSEL = 0x1; 
	 fw_RG_SSUSB_PLL_KBAND_KFC.Bits.RG_SSUSB_PLL_LDOVCO_VSEL = 0x0; 
 	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_KBAND_KFC, fw_RG_SSUSB_PLL_KBAND_KFC.Raw); // write addr: 0x1FAF3010 = 0x1F910001

	 fw_PON_DA_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0);
	 fw_PON_DA_CTRL_0.Bits.RG_SSUSB_PLL_IR = 0x5; 
	 fw_PON_DA_CTRL_0.Bits.RG_SSUSB_PLL_BR = 0x7; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0, fw_PON_DA_CTRL_0.Raw); // write addr: 0x1FAF4600 = 0x775E1012

	 fw_PON_DA_CTRL_19.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19);
	 fw_PON_DA_CTRL_19.Bits.RG_SSUSB_PLL_BPA = 0x4; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19, fw_PON_DA_CTRL_19.Raw); // write addr: 0x1FAF464C = 0x14

	 fw_PON_DA_CTRL_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1);
	 fw_PON_DA_CTRL_1.Bits.RG_SSUSB_PLL_BC = 0x2; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1, fw_PON_DA_CTRL_1.Raw); // write addr: 0x1FAF4604 = 0x30004020

	 fw_PON_DA_CTRL_4.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4);
	 fw_PON_DA_CTRL_4.Bits.RG_SSUSB_PLL_FBKDIV = 0x32; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4, fw_PON_DA_CTRL_4.Raw); // write addr: 0x1FAF4610 = 0x32

	 fw_PON_SERDES_CTRL_10.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10);
	 fw_PON_SERDES_CTRL_10.Bits.rg_tx_bit_polarity = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10, fw_PON_SERDES_CTRL_10.Raw); // write addr: 0x1FAF4228 = 0x3



//*******  //freq meter
	 fw_FRQ_CTRL_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_2);
	 fw_FRQ_CTRL_2.Bits.rg_hg_tx_freq_det_mux = 0x0; 
	 fw_FRQ_CTRL_2.Bits.rg_hg_rx_freq_det_mux = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_2, fw_FRQ_CTRL_2.Raw); // write addr: 0x1FAF4364 = 0x3003

	 fw_FRQ_CTRL_4.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_4);
	 fw_FRQ_CTRL_4.Bits.rg_hg_tx_freq_cnt = 0x271; 
	 fw_FRQ_CTRL_4.Bits.rg_hg_rx_freq_cnt = 0x271; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_FRQ_CTRL_4, fw_FRQ_CTRL_4.Raw); // write addr: 0x1FAF436C = 0x2710271

	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS);
#if RX_EYE_SCAN
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_CDR_EPEN = 0x1; 
#else
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_CDR_EPEN = 0x0; 
#endif
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS, fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw); //write addr: 0x1FAF3024 = 0x18000702



//*******    //CK EN
	 fw_PON_SYS_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0);
	 fw_PON_SYS_CTRL_0.Bits.rg_pon_tx250m_ck_en = 0x0; 
	 fw_PON_SYS_CTRL_0.Bits.rg_pon_rx500m_ck_en = 0x0; 
	 fw_PON_SYS_CTRL_0.Bits.rg_hg_tx250m_ck_en = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_hg_rx500m_ck_en = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_baser_txck_en = 0x1; 
	 fw_PON_SYS_CTRL_0.Bits.rg_baser_rxck_en = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0, fw_PON_SYS_CTRL_0.Raw); // write addr: 0x1FAF4300 = 0x1B0003C0

	 fw_PON_SYS_CTRL_0.Bits.rg_rx_pma_clk_sel = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0, fw_PON_SYS_CTRL_0.Raw); // write addr: 0x1FAF4300 = 0x1F0003C0

	 fw_PON_SYS_CTRL_0.Bits.rg_tx_pma_clk_sel = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0, fw_PON_SYS_CTRL_0.Raw); // write addr: 0x1FAF4300 = 0x3F0003C0



//*******    //RST EN
	 fw_HG_RST_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0);
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pma_tx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pma_rx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pcs_tx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_pcs_rx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_mac_tx_reset_n = 0x1; 
	 fw_HG_RST_CTRL_0.Bits.rg_hg_mac_rx_reset_n = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0, fw_HG_RST_CTRL_0.Raw); // write addr: 0x1FAF4310 = 0x3703F

	 fw_PON_DA_CTRL_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2);
#if RX_EYE_SCAN
	 fw_PON_DA_CTRL_2.Bits.RG_SSUSB_CDR_PI_PWD = 0x0; 
#else
	 fw_PON_DA_CTRL_2.Bits.RG_SSUSB_CDR_PI_PWD = 0x1; 
#endif
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2, fw_PON_DA_CTRL_2.Raw); //write addr: 0x1FAF4608 = 0x56101801

	 fw_PON_RXFEDIG_CTRL_12.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12);
	 tmp = fw_PON_RXFEDIG_CTRL_12.Bits.RG_SSUSB_EQ_REV;
	 tmp = (0x0<<6) | (tmp & (~(0x1<<6)));
	 fw_PON_RXFEDIG_CTRL_12.Bits.RG_SSUSB_EQ_REV = tmp;
	 fw_PON_RXFEDIG_CTRL_12.Bits.RG_SSUSB_EQ_REV = 0x38; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12, fw_PON_RXFEDIG_CTRL_12.Raw); // write addr: 0x1FAF4530 = 0x380013

	 fw_RG_SSUSB_LN0_CDR_RESERVE.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_RESERVE);
	 fw_RG_SSUSB_LN0_CDR_RESERVE.Bits.RG_SSUSB_LN0_RXAFE_RESERVE = 0x11; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_RESERVE, fw_RG_SSUSB_LN0_CDR_RESERVE.Raw); // write addr: 0x1FAF3018 = 0x1100

	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS);
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_RX_DAC_PWD = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS, fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw); //write addr: 0x1FAF3024 = 0x10000702

	 fw_RG_PCIE_CLKDRV_OFFSET.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_PCIE_CLKDRV_OFFSET);
	 tmp = fw_RG_PCIE_CLKDRV_OFFSET.Bits.RG_SSUSB_XTAL_TOP_RESERVE;
	 tmp = (0x1<<6) | (tmp & (~(0x1<<6)));
	 fw_RG_PCIE_CLKDRV_OFFSET.Bits.RG_SSUSB_XTAL_TOP_RESERVE = tmp;
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_PCIE_CLKDRV_OFFSET, fw_RG_PCIE_CLKDRV_OFFSET.Raw); // write addr: 0x1FAF3008 = 0x2400542

	 fw_HG_MODE_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0);
	 fw_HG_MODE_CTRL_0.Bits.rg_serdes_mode = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0, fw_HG_MODE_CTRL_0.Raw); // write addr: 0x1FAF4314 = 0x1



//*******    //NCPO
	 fw_PON_DA_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0);
	 fw_PON_DA_CTRL_0.Bits.RG_SSUSB_PLL_FBKSEL = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0, fw_PON_DA_CTRL_0.Raw); // write addr: 0x1FAF4600 = 0x775E0012

	 fw_SS_LCPLL_TDC_FLT_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2);
	 fw_SS_LCPLL_TDC_FLT_2.Bits.rg_lcpll_ncpo_value = 0x64000000; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2, fw_SS_LCPLL_TDC_FLT_2.Raw); // write addr: 0x1FAF4130 = 0x64000000
	 
	 fw_SS_LCPLL_TDC_FLT_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5);
	 fw_SS_LCPLL_TDC_FLT_5.Bits.rg_lcpll_ncpo_chg = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5, fw_SS_LCPLL_TDC_FLT_5.Raw); // write addr: 0x1FAF413C = 0x10100

	 fw_SS_LCPLL_TDC_FLT_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3);
	 fw_SS_LCPLL_TDC_FLT_3.Bits.rg_lcpll_ncpo_load = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3, fw_SS_LCPLL_TDC_FLT_3.Raw); // write addr: 0x1FAF4134 = 0x20000100

	 fw_SS_LCPLL_TDC_FLT_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5);
	 fw_SS_LCPLL_TDC_FLT_5.Bits.rg_lcpll_ncpo_chg = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5, fw_SS_LCPLL_TDC_FLT_5.Raw); // write addr: 0x1FAF413C = 0x1010100

	 fw_RG_SSUSB_PLL_SDM_HREN.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_SDM_HREN);
	 tmp = fw_RG_SSUSB_PLL_SDM_HREN.Bits.RG_SSUSB_PLL_RESERVE;
	 tmp = (0x1<<1) | (tmp & (~(0x1<<1)));
	 fw_RG_SSUSB_PLL_SDM_HREN.Bits.RG_SSUSB_PLL_RESERVE = tmp;
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_PLL_SDM_HREN, fw_RG_SSUSB_PLL_SDM_HREN.Raw); // write addr: 0x1FAF3014 = 0x1000C87
 
	 fw_PON_SERDES_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0);
	 fw_PON_SERDES_CTRL_0.Bits.rg_phya_auto_init = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0, fw_PON_SERDES_CTRL_0.Raw); // write addr: 0x1FAF4200 = 0x100381

	
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_1) rg_hsgmii_pcs_ctrol_1;
	RGDATA_t rg;	

	/* PCS Setting 0x0A00  0x0c9cc000, bit[30] PCS TX run as 20bit mode (deafult) */
	rg_hsgmii_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1);
	rg_hsgmii_pcs_ctrol_1.hal.rg_tbi_10b_mode = 0;
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1,rg_hsgmii_pcs_ctrol_1.dat.value);

	/*
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
	rg.bit.b2 = 1;
	rg.bit.b4 = 0;
	rg.bit.b5 = 1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);
	*/
	
	if(dbg_print) printk("sgmii_init\n");
}
static void xsgmii_init(u8 xsgmii){
	if(dbg_print) printk(">>xsgmii_init\n");
	
	switch (xsgmii){
		case USXGMII:
			//usxgmii_init();
			break;
		case HSGMII:
			hsgmii_init();
			break;
		case SGMII:
			sgmii_init();
			break;
		default:
			hsgmii_init();
			break;
	}
}

/** NOT USED *****************************************************************************************************************
static void USXGMII_10G(void)
{	
	rg_type_t(HAL_rg_rate_adapt_ctrl_11)ra_ctl_11;	
	rg_type_t(HAL_rg_usxgmii_an_control_1)rg_usxgmii_an_control_1;	
	RGDATA_t rg={0};	
	if(dbg_print) printk("USXGMII_10G\n");
#if 0 //move to usxgmii init	
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x100); 
	rg.bit.b28 = 1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x100,rg.value);	
	if(dbg_print) printk("read USXGMII_RG_USXGMII_AN_CONTROL_1 %x\n",RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET, (RgAddr)0x100));
#endif	
	rg_usxgmii_an_control_1.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_1) & ~(6<<8); 
	rg_usxgmii_an_control_1.hal.rg_an_ability &= 0xf0ff;
	rg_usxgmii_an_control_1.hal.rg_an_ability |= 6<8;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_1,rg_usxgmii_an_control_1.dat.value);

	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 1;
	rg.bit.b4 = 1;
	rg.bit.b26 = 1;	
	rg.bit.b27 = 1;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
	
	ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 1;
	ra_ctl_11.hal.rg_force_rate_adapt_mode = 0;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);

	if(dbg_print) printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_0 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_11 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11));
	if(dbg_print) printk("USXGMII_10G exit\n");
	
}

static void USXGMII_5G(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_11)ra_ctl_11;
	RGDATA_t rg={0};	
	if(dbg_print) printk("USXGMII_5G\n");
		
	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 1;
	rg.bit.b4 = 1;	
	rg.bit.b26 = 0; 
	rg.bit.b27 = 0;	
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
	
	ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 1;
	ra_ctl_11.hal.rg_force_rate_adapt_mode = 1;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);

	if(dbg_print) printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_0 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_11 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11));
	if(dbg_print) printk("USXGMII_5G exit\n");
}

static void USXGMII_2p5G(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_11)ra_ctl_11;
	RGDATA_t rg={0};	
	if(dbg_print) printk("USXGMII_2.5G\n");
			
	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 1;
	rg.bit.b4 = 1;	
	rg.bit.b26 = 0; 
	rg.bit.b27 = 0; 
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
		
	ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 1;
	ra_ctl_11.hal.rg_force_rate_adapt_mode = 2;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);

	if(dbg_print) printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_0 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_11 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11));
	if(dbg_print) printk("USXGMII_2.5G exit\n");

}

static void USXGMII_1G(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_11)ra_ctl_11;
	RGDATA_t rg={0};	
	if(dbg_print) printk("USXGMII_1G\n");
			
	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 1;
	rg.bit.b4 = 1;	
	rg.bit.b26 = 0; 
	rg.bit.b27 = 0; 
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
		
	ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 1;
	ra_ctl_11.hal.rg_force_rate_adapt_mode = 4;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);
	
	if(dbg_print) printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_0 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_11 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11));
	if(dbg_print) printk("USXGMII_1G exit\n");
}

static void USXGMII_100M(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_11)ra_ctl_11;
	RGDATA_t rg={0};	
	if(dbg_print) printk("USXGMII_100M\n");
			
	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg.bit.b0 = 1;
	rg.bit.b4 = 1;	
	rg.bit.b26 = 0; 
	rg.bit.b27 = 0; 
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg.value);
		
	ra_ctl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	ra_ctl_11.hal.rg_force_rate_adapt_mode_en = 1;
	ra_ctl_11.hal.rg_force_rate_adapt_mode = 6;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11,ra_ctl_11.dat.value);
	
	if(dbg_print) printk("_rg_usxgmii_an_control_1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, (RgAddr)_rg_usxgmii_an_control_1));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_0 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0));
	if(dbg_print) printk("_rg_rate_adapt_ctrl_11 %x\n",RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11));
	if(dbg_print) printk("USXGMII_100M exit\n");
}
*******************************************************************************************************************/


static void HSGMII_2p5G(void){
	if(dbg_print) printk("HSGMII_2.5G\n");



	
	/* Rata Adaption Setting 0x6100 0, bit[31] mill mode disable */
	rg_type_t(HAL_RATE_ADP_P0_CTRL_0) rg_rate_adapt_p0_ctrl_0;
	rg_rate_adapt_p0_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET, (RgAddr)_RATE_ADP_P0_CTRL_0);
	rg_rate_adapt_p0_ctrl_0.hal.rg_p0_dis_mii_mode=0;		//bit31
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_RATE_ADP_P0_CTRL_0, rg_rate_adapt_p0_ctrl_0.dat.value); 
	
	
	/* Rata Adaption Setting 0x6000 0xc000c11 */
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 0x1;		//bit0
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 0x1;		//bit4
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0x1;	//bit26
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0x1;	//bit27
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0, rg_rate_adapt_ctrl_0.dat.value);

	
	if(dbg_print) printk("HSGMII_2.5 exit\n");
}

static void SGMII_1G(void){
	if(dbg_print) printk("SGMII_1G\n");

	

	/* PCS Setting 0x1FA80A24 0x11 */
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 0x1;		//bit0
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0x0;		//bit5-4
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force, rg_an_sgmii_mode_force.dat.value);
	
	
	/* Rata Adaption Setting 0x6100 0, bit[31] mill mode disable */
	rg_type_t(HAL_RATE_ADP_P0_CTRL_0) rg_rate_adapt_p0_ctrl_0;
	rg_rate_adapt_p0_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET, (RgAddr)_RATE_ADP_P0_CTRL_0);
	rg_rate_adapt_p0_ctrl_0.hal.rg_p0_dis_mii_mode = 0x0;	//bit31
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_RATE_ADP_P0_CTRL_0, rg_rate_adapt_p0_ctrl_0.dat.value); 

	
	/* Rata Adaption Setting 0x6000 0xc000c11 */
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 0x1;		//bit0
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 0x1;		//bit4
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0x1;	//bit26
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0x1;	//bit27
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0, rg_rate_adapt_ctrl_0.dat.value);

	/* Rata Adaption Setting 0x602c 0x0004 (default) 
	rg_type_t(HAL_rg_rate_adapt_ctrl_11) rg_rate_adapt_ctrl_11;
	rg_rate_adapt_ctrl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode_en = 0x0;	//bit8
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode = 0x0;		//bit15-12
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11, rg_rate_adapt_ctrl_11.dat.value);
	*/
	
	
	if(dbg_print) printk("SGMII_1G exit\n");
}
static void SGMII_100M(void){
	if(dbg_print) printk("SGMII_100M\n");
	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	RGDATA_t rg;	
	
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	if(!sgmii_reg_an0.hal.sgmii_an_enable){
		//Multi Sgmii
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
		rg.bit.b2 = 1;
		rg.bit.b4 = 1;	
		rg.bit.b5 = 0;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		
	}

	/* PCS Setting 0x1FA80A24 0x11 */
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 0x1;		//bit0
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0x1;		//bit5-4
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force, rg_an_sgmii_mode_force.dat.value);
	
	/* Rata Adaption Setting 0x6100 0, bit[31] mill mode disable */
	rg_type_t(HAL_RATE_ADP_P0_CTRL_0) rg_rate_adapt_p0_ctrl_0;
	rg_rate_adapt_p0_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET, (RgAddr)_RATE_ADP_P0_CTRL_0);
	rg_rate_adapt_p0_ctrl_0.hal.rg_p0_dis_mii_mode = 0x0;	//bit31
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_RATE_ADP_P0_CTRL_0, rg_rate_adapt_p0_ctrl_0.dat.value);

	/* Rata Adaption Setting 0x6000 0xc11 */
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 0x1;		//bit0
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 0x1;		//bit4
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0x0;	//bit26
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0x0;	//bit27
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0, rg_rate_adapt_ctrl_0.dat.value);

	/* Rata Adaption Setting 0x602c 0x7104 
	rg_type_t(HAL_rg_rate_adapt_ctrl_11) rg_rate_adapt_ctrl_11;
	rg_rate_adapt_ctrl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode_en = 0x1;	//bit8
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode = 0x7;		//bit15-12
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11, rg_rate_adapt_ctrl_11.dat.value);
	*/
	
	#ifdef FPGA_PLATFORM_AN7552
	/* Rata Adaption Reset 0x4024 0x40*/
		//RGDATA_t rg;
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x24);
		rg.bit.b6 = 1;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x24,rg.value);
		udelay(3);
		rg.bit.b6 = 0;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x24,rg.value);
	#endif

	
	

	if(dbg_print) printk("SGMII_100M exit\n");
}
static void SGMII_10M(void){
	if(dbg_print) printk("SGMII_10M\n");

	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	RGDATA_t rg;	
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	if(!sgmii_reg_an0.hal.sgmii_an_enable){
		//Multi Sgmii
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
		rg.bit.b2 = 1;
		rg.bit.b4 = 0;
		rg.bit.b5 = 0;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		
	}

	/* PCS Setting 0x1FA80A24 0x11 */
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 0x1;		//bit0
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0x2;		//bit5-4
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force, rg_an_sgmii_mode_force.dat.value);

	/* Rata Adaption Setting 0x6100 0, bit[31] mill mode disable */
	rg_type_t(HAL_RATE_ADP_P0_CTRL_0) rg_rate_adapt_p0_ctrl_0;
	rg_rate_adapt_p0_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET, (RgAddr)_RATE_ADP_P0_CTRL_0);
	rg_rate_adapt_p0_ctrl_0.hal.rg_p0_dis_mii_mode = 0x0;	//bit31
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_RATE_ADP_P0_CTRL_0, rg_rate_adapt_p0_ctrl_0.dat.value);

	/* Rata Adaption Setting 0x6000 0xc11 */
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 0x1;		//bit0
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 0x1;		//bit4
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0x0;	//bit26
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0x0;	//bit27
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0, rg_rate_adapt_ctrl_0.dat.value);

	/* Rata Adaption Setting 0x602c 0xa104 
	rg_type_t(HAL_rg_rate_adapt_ctrl_11) rg_rate_adapt_ctrl_11;
	rg_rate_adapt_ctrl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode_en = 0x1;	//bit8
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode = 0xa;		//bit15-12
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11, rg_rate_adapt_ctrl_11.dat.value);
	*/
	
	#ifdef FPGA_PLATFORM_AN7552
	/* Rata Adaption Reset 0x4024 0x40*/
		//RGDATA_t rg;
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x24);
		rg.bit.b6 = 1;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x24,rg.value);
		udelay(3);
		rg.bit.b6 = 0;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x24,rg.value);
	#endif
	
	
	
	if(dbg_print) printk("SGMII_10M exit\n");
}

/** NOT USED *****************************************************************************************************************
static void usxgmii_rate_api(u8 rate){
	if(dbg_print) printk("usxgmii_rate_api rate %x : ",rate);
	
	#ifdef	FPGA_PLATFORM	
	fpga_clk_for_usxgmii_rate(rate);
	#endif

	switch (rate){
		case ENUM_USXGII_10G :
			if(dbg_print) printk("10G"); 
			USXGMII_10G();
			break;
		case ENUM_USXGII_5G :
			if(dbg_print) printk("5G"); 
			USXGMII_5G();
			break;
		case ENUM_USXGII_2p5G :
			if(dbg_print) printk("2.5G"); 
			USXGMII_2p5G();
			break;
		case ENUM_USXGII_1G :
			if(dbg_print) printk("1G"); 
			USXGMII_1G();
			break;
		case ENUM_USXGII_100M :
			if(dbg_print) printk("100M"); 
			USXGMII_100M();
			break;
			
		default :
			if(dbg_print) printk("10G"); 
			USXGMII_10G();
			break;			
	}
	if(dbg_print) printk("\n");
}	
*******************************************************************************************************************/	


static void hsgmii_force_mode_setting(void)
{
	if(dbg_print) printk("hsgmii_force_mode_setting: \n");


	/* PCS Setting 0x1FA80A24 0x0 */
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 0x0;		//bit0
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0x0;		//bit5-4
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force, rg_an_sgmii_mode_force.dat.value);

	
	RGDATA_t rg;
	//Multi Sgmii
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
	rg.bit.b2 = 1;
	rg.bit.b4 = 3 & 0x1;	
	rg.bit.b5 = (3 & 0x2)>>1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		


	/* Rata Adaption Setting 0x602c 0x0004 (default) */
	rg_type_t(HAL_rg_rate_adapt_ctrl_11) rg_rate_adapt_ctrl_11;
	rg_rate_adapt_ctrl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode_en = 0x0;	//bit8
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode = 0x0;		//bit15-12
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11, rg_rate_adapt_ctrl_11.dat.value);
	

	
}	

static void sgmii_force_mode_setting(u8 rate){
	if(dbg_print) printk("sgmii_force_mode_setting: rate=%x \n", rate);

	u8 cur_sgmii_mode;
	u8 link_mode;
	u8 rate_adapt_mode;
	u8 rate_adapt_mode_en;
	
	switch (rate){
		case ENUM_SGMII_1000M :
			cur_sgmii_mode = 0;
	 		link_mode = 2;
			rate_adapt_mode = 0;
			rate_adapt_mode_en = 0;
			break;
		case ENUM_SGMII_100M :
			cur_sgmii_mode = 1;
	 		link_mode = 1;
			rate_adapt_mode = 0x7;
			rate_adapt_mode_en = 1;
			break;
		case ENUM_SGMII_10M :
			cur_sgmii_mode = 2;
	 		link_mode = 0;
			rate_adapt_mode = 0xa;
			rate_adapt_mode_en = 1;
			break;	
		default :			
			cur_sgmii_mode = 0;
	 		link_mode = 2;
			rate_adapt_mode = 0;
			rate_adapt_mode_en = 0;
			break;			
	}
	
	/* PCS Setting 0x1FA80A24 0x0 */
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 0x1;		//bit0
	rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = cur_sgmii_mode;		//bit5-4
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force, rg_an_sgmii_mode_force.dat.value);


	RGDATA_t rg;
	//Multi Sgmii
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
	rg.bit.b2 = 0x1;
	rg.bit.b4 = link_mode & 0x1;	
	rg.bit.b5 = (link_mode & 0x2)>>1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		

	/* Rata Adaption Setting 0x602c 0x7104 */
	//this setting not in sv file
	rg_type_t(HAL_rg_rate_adapt_ctrl_11) rg_rate_adapt_ctrl_11;
	rg_rate_adapt_ctrl_11.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11);
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode_en = rate_adapt_mode_en;	//bit8
	rg_rate_adapt_ctrl_11.hal.rg_force_rate_adapt_mode = rate_adapt_mode;		//bit15-12
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_11, rg_rate_adapt_ctrl_11.dat.value);

}	

static void sgmii_rate_adaption(u8 rate){
	if(dbg_print) printk("sgmii_rate_api rate %x\n",rate);

	u8 adapt_tx_bypass = 1;
	u8 adapt_rx_bypass = 1;
	
	switch (rate){
		case ENUM_SGMII_1000M :
			//SGMII_1G();
			adapt_tx_bypass = 1;
			adapt_rx_bypass = 1;
			break;
		case ENUM_SGMII_100M :
			//SGMII_100M();
			adapt_tx_bypass = 0;
			adapt_rx_bypass = 0;
			break;
		case ENUM_SGMII_10M :
			//SGMII_10M();
			adapt_tx_bypass = 0;
			adapt_rx_bypass = 0;
			break;	
		default :			
			//SGMII_1G();
			adapt_tx_bypass = 1;
			adapt_rx_bypass = 1;
			break;			
	}

	/* Rata Adaption Setting 0x6100 0, bit[31] mill mode disable */
	rg_type_t(HAL_RATE_ADP_P0_CTRL_0) rg_rate_adapt_p0_ctrl_0;
	rg_rate_adapt_p0_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET, (RgAddr)_RATE_ADP_P0_CTRL_0);
	rg_rate_adapt_p0_ctrl_0.hal.rg_p0_dis_mii_mode = 0x0;	//bit31
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_RATE_ADP_P0_CTRL_0, rg_rate_adapt_p0_ctrl_0.dat.value);

	/* Rata Adaption Setting 0x6000 0xc11 */
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 0x1;		//bit0
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 0x1;		//bit4
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = adapt_tx_bypass;	//bit26
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = adapt_rx_bypass;	//bit27
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0, rg_rate_adapt_ctrl_0.dat.value);
	
	
}	

static void hsgmii_rate_api(void){
	if(dbg_print) printk("hsgmii\n");
	HSGMII_2p5G();
}
static void AN_ReStart(u8 xsgmii){
	RGDATA_t rg;
	
	//u16 loop;	
	if(dbg_print) printk("AN_ReStart\n");
	
	switch (xsgmii){
		case USXGMII :
		/*
			rg_type_t(HAL_ro_usxgmii_an_stats_0) ro_usxgmii_an_stats_0;	
			rg_type_t(HAL_rg_usxgmii_an_control_6) rg_usxgmii_an_control_6;
			rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_usxgmii_an_control_0);
			if(dbg_print) printk("AN_ReStart1 %x\n",rg.value);

			if(!rg.bit.b0)break; //AN disable
			
			rg_usxgmii_an_control_6.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_usxgmii_an_control_6);
			rg_usxgmii_an_control_6.hal.rg_tog_pcs_autoneg_sts = 1;
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_usxgmii_an_control_6,rg_usxgmii_an_control_6.dat.value);
			if(dbg_print) printk("AN_ReStart1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) 0xc1c));
			udelay(3);
			rg_usxgmii_an_control_6.hal.rg_tog_pcs_autoneg_sts = 0;
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_usxgmii_an_control_6,rg_usxgmii_an_control_6.dat.value);
			if(dbg_print) printk("AN_ReStart1 %x\n",RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) 0xc1c));
		*/
			#ifdef	FPGA_PLATFORM
		/*
			ro_usxgmii_an_stats_0.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_usxgmii_an_stats_0);
			if(dbg_print) printk("AN_ReStart3 %x\n",ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode);
			if((ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode == 0x04) ||(ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode == 0x02))fpga_clk_for_usxgmii_rate(ENUM_USXGII_2p5G);
			else fpga_clk_for_usxgmii_rate(ENUM_USXGII_10G);	
		*/
			#endif		
			break;
			
		case HSGMII:
			
		case SGMII:
			rg.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0);
			if(!rg.bit.b12) break;
			else{				
				rg.bit.b9 = 1;			
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,rg.value);
				udelay(3);
				rg.bit.b9 = 0;			
				RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an0,rg.value);			
			}
			break;
			
		default :			
			printk("AN parameter error!!!\n");
			break;			
	}
	

}	

static void xSGMII_Force_Mode_Setting(u8 xsgmii,u8 rate){
	if(dbg_print) printk("xSGMII_force_mode_setting %x, rate : %x\n",xsgmii,rate);
	switch (xsgmii){
		case USXGMII :
			
			break;
		case HSGMII:
			hsgmii_force_mode_setting();
			break;
		case SGMII:
			sgmii_force_mode_setting(rate);
			break;
		default :			
			hsgmii_force_mode_setting();
			break;			
	}
}


static void xSGMII_Rate_API(u8 xsgmii,u8 rate){
	if(dbg_print) printk("xSGMII_Rate_API %x, rate : %x\n",xsgmii,rate);
	switch (xsgmii){
		case USXGMII :
			//usxgmii_rate_api(rate);
			break;
		case HSGMII:
			hsgmii_rate_api();
			break;
		case SGMII:
			sgmii_rate_adaption(rate);
			break;
		default :			
			hsgmii_rate_api();
			break;			
	}
}

static void xSGMII_AN_API(u8 xsgmii,u8 an){
	if(dbg_print) printk("xSGMII_AN_API %x, enable : %x\n",xsgmii,an);
	switch (xsgmii){
		case USXGMII :
			//usxgmii_an(an);
			break;
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
		/*
			rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_base_r_10gb_t_pcs_stus1);
			sts.bit.b0 = rg.bit.b12;//sync
			rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_usxgmii_an_stats_2);
			sts.bit.b1 = rg.bit.b24;//an
		*/
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
	if(dbg_print) printk("link status:\nrx_sync: %x\nan_done: %x\n", sts.bit.b0, sts.bit.b1);
	return sts.value;
}	

static void xSGMII_PHY_Counter(void){
	rg_type_t(HAL_rg_hsgmii_gpii_state_1) rg_hsgmii_gpii_state_1;
	rg_type_t(HAL_rg_hsgmii_gpii_state_2) rg_hsgmii_gpii_state_2;
	rg_type_t(HAL_rg_hsgmii_gpii_state_3) rg_hsgmii_gpii_state_3;
	rg_type_t(HAL_rg_hsgmii_gpii_state_6) rg_hsgmii_gpii_state_6;
	rg_type_t(HAL_rg_hsgmii_gpii_state_7) rg_hsgmii_gpii_state_7;

	rg_type_t(HAL_rg_hsgmii_gpii_state_8) rg_hsgmii_gpii_state_8;
	rg_type_t(HAL_rg_hsgmii_gpii_state_9) rg_hsgmii_gpii_state_9;
	rg_type_t(HAL_rg_hsgmii_gpii_state_10) rg_hsgmii_gpii_state_10;


	rg_type_t(HAL_ro_rate_adapt_sts_1) ro_rate_adapt_sts_1;
	rg_type_t(HAL_ro_rate_adapt_sts_2) ro_rate_adapt_sts_2;
	rg_type_t(HAL_ro_rate_adapt_sts_3) ro_rate_adapt_sts_3;
	rg_type_t(HAL_ro_rate_adapt_sts_4) ro_rate_adapt_sts_4;
	rg_type_t(HAL_ro_rate_adapt_sts_5) ro_rate_adapt_sts_5;
	rg_type_t(HAL_ro_rate_adapt_sts_6) ro_rate_adapt_sts_6;
	rg_type_t(HAL_ro_rate_adapt_sts_7) ro_rate_adapt_sts_7;
	rg_type_t(HAL_ro_rate_adapt_sts_8) ro_rate_adapt_sts_8;
	
	rg_type_t(HAL_ro_rate_adapt_sts_9) ro_rate_adapt_sts_9;
	rg_type_t(HAL_ro_rate_adapt_sts_10) ro_rate_adapt_sts_10;
	rg_type_t(HAL_ro_rate_adapt_sts_11) ro_rate_adapt_sts_11;

	rg_type_t(HAL_rg_rate_adapt_ctrl_11) rg_rate_adapt_ctrl_11;

	u32 val;
		
	 pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0 fw_PON_DEBUG_REGISTER_0;
	 reg_type_t(REG_rg_hsgmii_pcs_state_2) fw_rg_hsgmii_pcs_state_2;
	 reg_type_t(REG_rg_hsgmii_pcs_state_4) fw_rg_hsgmii_pcs_state_4;
	 reg_type_t(REG_sgmii_reg_an_21) fw_sgmii_reg_an_21;
	 reg_type_t(REG_sgmii_reg_an_12) fw_sgmii_reg_an_12;
	 reg_type_t(multi_sgmii_csr_REG_msg_rx_sts_15) fw_msg_rx_sts_15;
	 reg_type_t(multi_sgmii_csr_REG_msg_rx_sts_19) fw_msg_rx_sts_19;

	 fw_PON_DEBUG_REGISTER_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0);
	 val = fw_PON_DEBUG_REGISTER_0.Bits.ro_lck2ref; //excel 4 read addr: 0x1FAF4700 [00:00]
	 printk("ro_lck2ref : %x\n", val);

	 fw_rg_hsgmii_pcs_state_2.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_2);
	 val = fw_rg_hsgmii_pcs_state_2.Bits.ro_rx_sync; //excel 5 read addr: 0x1FA80B04 [05:05]
	 printk("ro_rx_sync : %x\n", val);

	 fw_rg_hsgmii_pcs_state_2.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_2);
	 val = fw_rg_hsgmii_pcs_state_2.Bits.ro_dec_sync; //excel 6 read addr: 0x1FA80B04 [04:04]
	 printk("ro_dec_sync : %x\n", val);

	 fw_rg_hsgmii_pcs_state_4.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_4);
	 val = fw_rg_hsgmii_pcs_state_4.Bits.ro_rx_invalid; //excel 7 read addr: 0x1FA80B0C [01:01]
	 printk("ro_rx_invalid : %x\n", val);

	 fw_sgmii_reg_an_21.Raw.value = RG_R_PL(AN_BASE, AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an_21);
	 fw_sgmii_reg_an_21.Bits.SGMII_PCS_DEC_ERROR_CNT_READ_LATCH = 0x1; 
	 RG_W_PL(AN_BASE, AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an_21, fw_sgmii_reg_an_21.Raw.value); //excel 8, write addr: 0x1FA80054 = 0x40000000

	 fw_sgmii_reg_an_12.Raw.value = RG_R_PL(AN_BASE, AN_BASE_OFFSET, (RgAddr) _sgmii_reg_an_12);
	 val = fw_sgmii_reg_an_12.Bits.SGMII_DEC_ERROR_CNT; //excel 9 read addr: 0x1FA80030 [15:00]
	 printk("SGMII_DEC_ERROR_CNT : %x\n", val);

	 fw_rg_hsgmii_pcs_state_2.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_2);
	 val = fw_rg_hsgmii_pcs_state_2.Bits.ro_an_done; //excel 10 read addr: 0x1FA80B04 [00:00]
	 printk("ro_an_done : %x\n", val);

	 fw_msg_rx_sts_15.Raw.value = RG_R_PL(MULTI_SGMII_BASE, MULTI_SGMII_BASE_OFFSET, (RgAddr) _multi_sgmii_csr_msg_rx_sts_15);
	 val = fw_msg_rx_sts_15.Bits.ro_speed_sts_p0; //excel 11 read addr: 0x1FA8443C [02:00]
	 printk("ro_speed_sts_p0 : %x\n", val);

	 fw_msg_rx_sts_15.Raw.value = RG_R_PL(MULTI_SGMII_BASE, MULTI_SGMII_BASE_OFFSET, (RgAddr) _multi_sgmii_csr_msg_rx_sts_15);
	 val = fw_msg_rx_sts_15.Bits.ro_lnk_sts_p0; //excel 12 read addr: 0x1FA8443C [03:03]
	 printk("ro_lnk_sts_p0 : %x\n", val);

	 fw_msg_rx_sts_19.Raw.value = RG_R_PL(MULTI_SGMII_BASE, MULTI_SGMII_BASE_OFFSET, (RgAddr) _multi_sgmii_csr_msg_rx_sts_19);
	 val = fw_msg_rx_sts_19.Bits.p0_lp_ability; //excel 13 read addr: 0x1FA8444C [15:0]
	 printk("p0_lp_ability : %x\n", val);


	
	/* Toggle read RA-XGMII counter 0x602c 3 ->0 */
	rg_rate_adapt_ctrl_11.dat.value	= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_11);	
	rg_rate_adapt_ctrl_11.hal.rg_rate_adapt_rd_toggle = 0x1;
	rg_rate_adapt_ctrl_11.hal.rg_xgmii_pkt_rd_toggle = 0x1;
	RG_W_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_11, rg_rate_adapt_ctrl_11.dat.value);
	rg_rate_adapt_ctrl_11.hal.rg_rate_adapt_rd_toggle = 0x0;
	rg_rate_adapt_ctrl_11.hal.rg_xgmii_pkt_rd_toggle = 0x0;
	RG_W_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_11, rg_rate_adapt_ctrl_11.dat.value);

	
	/* Read Counter(PCS/RA) */
	rg_hsgmii_gpii_state_1.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_1);
	rg_hsgmii_gpii_state_2.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_2);
	rg_hsgmii_gpii_state_3.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_3);
	rg_hsgmii_gpii_state_6.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_6);
	rg_hsgmii_gpii_state_7.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_7);

	rg_hsgmii_gpii_state_8.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_8);
	rg_hsgmii_gpii_state_9.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_9);
	rg_hsgmii_gpii_state_10.dat.value	= RG_R_PL(_pxSGMII->pcs2_base, USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_gpii_state_10);


	ro_rate_adapt_sts_1.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_1);
	ro_rate_adapt_sts_2.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_2);
	ro_rate_adapt_sts_3.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_3);
	ro_rate_adapt_sts_4.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_4);
	ro_rate_adapt_sts_5.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_5);
	ro_rate_adapt_sts_6.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_6);
	ro_rate_adapt_sts_7.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_7);
	ro_rate_adapt_sts_8.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_8);
	
	ro_rate_adapt_sts_9.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_9);
	ro_rate_adapt_sts_10.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_10);
	ro_rate_adapt_sts_11.dat.value		= RG_R_PL(_pxSGMII->ra_base, USXGMII_RA_BASE_OFFSET,(RgAddr) _ro_rate_adapt_sts_11);	

	printk("\n[PCS RX Counter]\n");
	printk(" RO_FB_IN_CNT:		\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_1),	rg_hsgmii_gpii_state_1.hal.ro_fb_in_cnt,	rg_hsgmii_gpii_state_1.hal.ro_fb_in_cnt);
	printk(" RO_FD_IN_CNT:		\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_2),	rg_hsgmii_gpii_state_2.hal.ro_fd_in_cnt,	rg_hsgmii_gpii_state_2.hal.ro_fd_in_cnt);
	printk(" RO_FBO_IN_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_6),	rg_hsgmii_gpii_state_6.hal.ro_fbo_in_cnt,	rg_hsgmii_gpii_state_6.hal.ro_fbo_in_cnt);
	printk(" RO_FDO_IN_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_7),	rg_hsgmii_gpii_state_7.hal.ro_fdo_in_cnt,	rg_hsgmii_gpii_state_7.hal.ro_fdo_in_cnt);
	printk(" RO_FE_IN_CNT:		\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_3),	rg_hsgmii_gpii_state_3.hal.ro_fe_in_cnt,	rg_hsgmii_gpii_state_3.hal.ro_fe_in_cnt);

	printk("\n[PCS TX Counter]\n");
	printk(" RO_FB_OUT_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_8),	rg_hsgmii_gpii_state_8.hal.ro_fb_out_cnt,	rg_hsgmii_gpii_state_8.hal.ro_fb_out_cnt);
	printk(" RO_FD_OUT_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_9),	rg_hsgmii_gpii_state_9.hal.ro_fd_out_cnt,	rg_hsgmii_gpii_state_9.hal.ro_fd_out_cnt);
	printk(" RO_FE_OUT_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa80000+_rg_hsgmii_gpii_state_10),	rg_hsgmii_gpii_state_10.hal.ro_fe_out_cnt,	rg_hsgmii_gpii_state_10.hal.ro_fe_out_cnt);


	printk("\n[RA-XGMII RX Counter]\n");
	printk(" RO_RX_XFI_SOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_5),	ro_rate_adapt_sts_5.hal.ro_rate_adapt_rx_xfi_sop_cnt,		ro_rate_adapt_sts_5.hal.ro_rate_adapt_rx_xfi_sop_cnt);
	printk(" RO_RX_XFI_EOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_6),	ro_rate_adapt_sts_6.hal.ro_rate_adapt_rx_xfi_eop_cnt,		ro_rate_adapt_sts_6.hal.ro_rate_adapt_rx_xfi_eop_cnt);
	printk(" RO_RX_MAC_SOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_7),	ro_rate_adapt_sts_7.hal.ro_rate_adapt_rx_mac_sop_cnt,		ro_rate_adapt_sts_7.hal.ro_rate_adapt_rx_mac_sop_cnt);
	printk(" RO_RX_MAC_EOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_8),	ro_rate_adapt_sts_8.hal.ro_rate_adapt_rx_mac_eop_cnt,		ro_rate_adapt_sts_8.hal.ro_rate_adapt_rx_mac_eop_cnt);
	printk(" RO_RX_XFI_FE_CNT:  	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_10),	ro_rate_adapt_sts_10.hal.ro_rate_adapt_rx_xfi_fe_cnt,		ro_rate_adapt_sts_10.hal.ro_rate_adapt_rx_xfi_fe_cnt);

	printk("\n[RA-XGMII TX Counter]\n");
	printk(" RO_TX_XFI_SOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_1),	ro_rate_adapt_sts_1.hal.ro_rate_adapt_tx_xfi_sop_cnt,		ro_rate_adapt_sts_1.hal.ro_rate_adapt_tx_xfi_sop_cnt);
	printk(" RO_TX_XFI_EOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_2),	ro_rate_adapt_sts_2.hal.ro_rate_adapt_tx_xfi_eop_cnt,		ro_rate_adapt_sts_2.hal.ro_rate_adapt_tx_xfi_eop_cnt);
	printk(" RO_TX_MAC_SOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_3),	ro_rate_adapt_sts_3.hal.ro_rate_adapt_tx_mac_sop_cnt,		ro_rate_adapt_sts_3.hal.ro_rate_adapt_tx_mac_sop_cnt);
	printk(" RO_TX_MAC_EOP_CNT: 	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_4),	ro_rate_adapt_sts_4.hal.ro_rate_adapt_tx_mac_eop_cnt,		ro_rate_adapt_sts_4.hal.ro_rate_adapt_tx_mac_eop_cnt);
	printk(" RO_TX_MAC_FE_CNT:  	\t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_9),	ro_rate_adapt_sts_9.hal.ro_rate_adapt_tx_mac_fe_cnt,		ro_rate_adapt_sts_9.hal.ro_rate_adapt_tx_mac_fe_cnt);
	printk(" RO_TX_XFI_IPG_ERR_CNT: \t(%x) = %010d\t(HEX:%x)\n",	(0x1fa86000+_ro_rate_adapt_sts_11),	ro_rate_adapt_sts_11.hal.ro_rate_adapt_tx_xfi_ipg_err_cnt,	ro_rate_adapt_sts_11.hal.ro_rate_adapt_tx_xfi_ipg_err_cnt);
}


#if 0
static void Rx_Tx_Path_Cnt(void)
{
	u32 val;
		
	 pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0 fw_PON_DEBUG_REGISTER_0;
	 reg_type_t(REG_rg_hsgmii_pcs_state_2) fw_rg_hsgmii_pcs_state_2;
	 reg_type_t(REG_rg_hsgmii_pcs_state_4) fw_rg_hsgmii_pcs_state_4;
	 reg_type_t(REG_sgmii_reg_an_21) fw_sgmii_reg_an_21;
	 reg_type_t(REG_sgmii_reg_an_12) fw_sgmii_reg_an_12;
	 reg_type_t(multi_sgmii_csr_REG_msg_rx_sts_15) fw_msg_rx_sts_15;
	 reg_type_t(multi_sgmii_csr_REG_msg_rx_sts_19) fw_msg_rx_sts_19;
	 reg_type_t(multi_sgmii_csr_REG_Debug_REGISTER_1) fw_Debug_REGISTER_1;
	 reg_type_t(REG_rg_hsgmii_gpii_state_1) fw_rg_hsgmii_gpii_state_1;
	 reg_type_t(REG_rg_hsgmii_gpii_state_2) fw_rg_hsgmii_gpii_state_2;
	 reg_type_t(REG_rg_hsgmii_gpii_state_3) fw_rg_hsgmii_gpii_state_3;
	 reg_type_t(REG_rg_hsgmii_gpii_state_6) fw_rg_hsgmii_gpii_state_6;
	 reg_type_t(REG_rg_hsgmii_gpii_state_7) fw_rg_hsgmii_gpii_state_7;
	 reg_type_t(multi_phy_ra_csr_REG_rg_rate_adapt_ctrl_11) fw_rg_rate_adapt_ctrl_11;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_5) fw_ro_rate_adapt_sts_5;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_6) fw_ro_rate_adapt_sts_6;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_10) fw_ro_rate_adapt_sts_10;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_7) fw_ro_rate_adapt_sts_7;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_8) fw_ro_rate_adapt_sts_8;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_3) fw_ro_rate_adapt_sts_3;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_4) fw_ro_rate_adapt_sts_4;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_9) fw_ro_rate_adapt_sts_9;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_1) fw_ro_rate_adapt_sts_1;
	 reg_type_t(multi_phy_ra_csr_REG_ro_rate_adapt_sts_2) fw_ro_rate_adapt_sts_2;
	 reg_type_t(REG_rg_hsgmii_gpii_state_8) fw_rg_hsgmii_gpii_state_8;
	 reg_type_t(REG_rg_hsgmii_gpii_state_9) fw_rg_hsgmii_gpii_state_9;
	 reg_type_t(REG_rg_hsgmii_gpii_state_10) fw_rg_hsgmii_gpii_state_10;

	 
 fw_PON_DEBUG_REGISTER_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0);
 val = fw_PON_DEBUG_REGISTER_0.Bits.ro_lck2ref; //excel 4 read addr: 0x1FAF4700 [00:00]
 printk("ro_lck2ref : %x\n", val);

 fw_rg_hsgmii_pcs_state_2.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_2);
 val = fw_rg_hsgmii_pcs_state_2.Bits.ro_rx_sync; //excel 5 read addr: 0x1FA80B04 [05:05]
 printk("ro_rx_sync : %x\n", val);

 fw_rg_hsgmii_pcs_state_2.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_2);
 val = fw_rg_hsgmii_pcs_state_2.Bits.ro_dec_sync; //excel 6 read addr: 0x1FA80B04 [04:04]
 printk("ro_dec_sync : %x\n", val);

 fw_rg_hsgmii_pcs_state_4.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_4);
 val = fw_rg_hsgmii_pcs_state_4.Bits.ro_rx_invalid; //excel 7 read addr: 0x1FA80B0C [01:01]
 printk("ro_rx_invalid : %x\n", val);

 fw_sgmii_reg_an_21.Raw.value = RG_R_PL(AN_BASE, AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an_21);
 fw_sgmii_reg_an_21.Bits.SGMII_PCS_DEC_ERROR_CNT_READ_LATCH = 0x1; 
 RG_W_PL(AN_BASE, AN_BASE_OFFSET,(RgAddr) _sgmii_reg_an_21, fw_sgmii_reg_an_21.Raw.value); //excel 8, write addr: 0x1FA80054 = 0x40000000

 fw_sgmii_reg_an_12.Raw.value = RG_R_PL(AN_BASE, AN_BASE_OFFSET, (RgAddr) _sgmii_reg_an_12);
 val = fw_sgmii_reg_an_12.Bits.SGMII_DEC_ERROR_CNT; //excel 9 read addr: 0x1FA80030 [15:00]
 printk("SGMII_DEC_ERROR_CNT : %x\n", val);

 fw_rg_hsgmii_pcs_state_2.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_pcs_state_2);
 val = fw_rg_hsgmii_pcs_state_2.Bits.ro_an_done; //excel 10 read addr: 0x1FA80B04 [00:00]
 printk("ro_an_done : %x\n", val);

 fw_msg_rx_sts_15.Raw.value = RG_R_PL(MULTI_SGMII_BASE, MULTI_SGMII_BASE_OFFSET, (RgAddr) _multi_sgmii_csr_msg_rx_sts_15);
 val = fw_msg_rx_sts_15.Bits.ro_speed_sts_p0; //excel 11 read addr: 0x1FA8443C [02:00]
 printk("ro_speed_sts_p0 : %x\n", val);

 fw_msg_rx_sts_15.Raw.value = RG_R_PL(MULTI_SGMII_BASE, MULTI_SGMII_BASE_OFFSET, (RgAddr) _multi_sgmii_csr_msg_rx_sts_15);
 val = fw_msg_rx_sts_15.Bits.ro_lnk_sts_p0; //excel 12 read addr: 0x1FA8443C [03:03]
 printk("ro_lnk_sts_p0 : %x\n", val);

 fw_msg_rx_sts_19.Raw.value = RG_R_PL(MULTI_SGMII_BASE, MULTI_SGMII_BASE_OFFSET, (RgAddr) _multi_sgmii_csr_msg_rx_sts_19);
 val = fw_msg_rx_sts_19.Bits.p0_lp_ability; //excel 13 read addr: 0x1FA8444C [15:0]
 printk("p0_lp_ability : %x\n", val);



//*******  check point 2: Rx path cnt


//*******  [PHYD] PCS Rx cnt
 fw_Debug_REGISTER_1.Raw.value = RG_R_PL(MULTI_SGMII_BASE, MULTI_SGMII_BASE_OFFSET, (RgAddr) _multi_sgmii_csr_Debug_REGISTER_1);
 val = fw_Debug_REGISTER_1.Bits.ro_dump_001; //excel 16 read addr: 0x1FA84604 [31:00]
 printk("ro_dump_001 : %x\n", val);

 fw_rg_hsgmii_gpii_state_1.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_1);
 val = fw_rg_hsgmii_gpii_state_1.Bits.ro_fb_in_cnt; //excel 17 read addr: 0x1FA80B20 [31:00]
 printk("ro_fb_in_cnt : %x\n", val);

 fw_rg_hsgmii_gpii_state_2.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_2);
 val = fw_rg_hsgmii_gpii_state_2.Bits.ro_fd_in_cnt; //excel 18 read addr: 0x1FA80B28 [31:00]
 printk("ro_fd_in_cnt : %x\n", val);

 fw_rg_hsgmii_gpii_state_3.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_3);
 val = fw_rg_hsgmii_gpii_state_3.Bits.ro_fe_in_cnt; //excel 19 read addr: 0x1FA80B2C [31:00]
 printk("ro_fe_in_cnt : %x\n", val);

 fw_rg_hsgmii_gpii_state_6.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_6);
 val = fw_rg_hsgmii_gpii_state_6.Bits.ro_fbo_in_cnt; //excel 20 read addr: 0x1FA80B38 [31:00]
 printk("ro_fbo_in_cnt : %x\n", val);

 fw_rg_hsgmii_gpii_state_7.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_7);
 val = fw_rg_hsgmii_gpii_state_7.Bits.ro_fdo_in_cnt; //excel 21 read addr: 0x1FA80B3C [31:00]
 printk("ro_fdo_in_cnt : %x\n", val);



//*******  [PHYD] Rate adaption Rx cnt
 fw_rg_rate_adapt_ctrl_11.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET,(RgAddr) _multi_phy_ra_csr_rg_rate_adapt_ctrl_11);
 fw_rg_rate_adapt_ctrl_11.Bits.rg_rate_adapt_rd_toggle = 0x1; 
 RG_W_PL(RA_BASE, RA_BASE_OFFSET,(RgAddr) _multi_phy_ra_csr_rg_rate_adapt_ctrl_11, fw_rg_rate_adapt_ctrl_11.Raw.value); //excel 35, write addr: 0x1FA8602C = 0x5

 fw_rg_rate_adapt_ctrl_11.Bits.rg_rate_adapt_rd_toggle = 0x0; 
 RG_W_PL(RA_BASE, RA_BASE_OFFSET,(RgAddr) _multi_phy_ra_csr_rg_rate_adapt_ctrl_11, fw_rg_rate_adapt_ctrl_11.Raw.value); //excel 36, write addr: 0x1FA8602C = 0x4

 fw_ro_rate_adapt_sts_5.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_5);
 val = fw_ro_rate_adapt_sts_5.Bits.ro_rate_adapt_rx_xfi_sop_cnt; //excel 37 read addr: 0x1FA86044 [31:00]
 printk("ro_rate_adapt_rx_xfi_sop_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_6.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_6);
 val = fw_ro_rate_adapt_sts_6.Bits.ro_rate_adapt_rx_xfi_eop_cnt; //excel 38 read addr: 0x1FA86048 [31:00]
 printk("ro_rate_adapt_rx_xfi_eop_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_10.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_10);
 val = fw_ro_rate_adapt_sts_10.Bits.ro_rate_adapt_rx_xfi_fe_cnt; //excel 39 read addr: 0x1FA86058 [31:00]
 printk("ro_rate_adapt_rx_xfi_fe_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_7.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_7);
 val = fw_ro_rate_adapt_sts_7.Bits.ro_rate_adapt_rx_mac_sop_cnt; //excel 40 read addr: 0x1FA8604C [31:00]
 printk("ro_rate_adapt_rx_mac_sop_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_8.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_8);
 val = fw_ro_rate_adapt_sts_8.Bits.ro_rate_adapt_rx_mac_eop_cnt; //excel 41 read addr: 0x1FA86050 [31:00]
 printk("ro_rate_adapt_rx_mac_eop_cnt : %x\n", val);

#if 0

//*******  [MAC] Rx cnt
 fw_xfi_rx_frame_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rx_frame_cnt);
 val = fw_xfi_rx_frame_cnt.Bits.rx_frame_cnt; //excel 44 read addr: 0x1FA08180 [31:00]
 printk("rx_frame_cnt : %x\n", val);

 fw_xfi_rx_octets_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rx_octets_cnt);
 val = fw_xfi_rx_octets_cnt.Bits.rx_octets_cnt; //excel 45 read addr: 0x1FA08184 [31:00]
 printk("rx_octets_cnt : %x\n", val);

 fw_xfi_rx_pkt_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rx_pkt_cnt);
 val = fw_xfi_rx_pkt_cnt.Bits.rx_packet_cnt; //excel 46 read addr: 0x1FA08188 [31:00]
 printk("rx_packet_cnt : %x\n", val);

 fw_xfi_rx_crcerr_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rx_crcerr_cnt);
 val = fw_xfi_rx_crcerr_cnt.Bits.rx_crcerr_cnt; //excel 47 read addr: 0x1FA08198 [15:00]
 printk("rx_crcerr_cnt : %x\n", val);

 fw_xfi_rx_crcerr_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rx_crcerr_cnt);
 val = fw_xfi_rx_crcerr_cnt.Bits.rx_codingerr_cnt; //excel 48 read addr: 0x1FA08198 [31:16]
 printk("rx_codingerr_cnt : %x\n", val);

 fw_xfi_rxmbi_pkt_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rxmbi_pkt_cnt);
 val = fw_xfi_rxmbi_pkt_cnt.Bits.rxmbi_eth_cnt; //excel 49 read addr: 0x1FA0819C [31:00]
 printk("rxmbi_eth_cnt : %x\n", val);

 fw_xfi_rxmbi_drop_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rxmbi_drop_cnt);
 val = fw_xfi_rxmbi_drop_cnt.Bits.rxmbi_errdrop_cnt; //excel 50 read addr: 0x1FA081A0 [15:00]
 printk("rxmbi_errdrop_cnt : %x\n", val);

 fw_xfi_rxmbi_drop_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_rxmbi_drop_cnt);
 val = fw_xfi_rxmbi_drop_cnt.Bits.rxmbi_sofdrop_cnt; //excel 51 read addr: 0x1FA081A0 [31:16]
 printk("rxmbi_sofdrop_cnt : %x\n", val);



//*******  [FE] Rx cnt use command 'cat /proc/tc3162/fe_debug_reg'
 fw_GDM_RX_OK_CNT.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_RX_OK_CNT);
 val = fw_GDM_RX_OK_CNT.Bits.GDM_RX_OK_CNT; //excel 53 read addr: 0x1FB51648 [31:00]
 printk("GDM_RX_OK_CNT : %x\n", val);

 fw_GDM_RX_FC_DROP_CNT.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_RX_FC_DROP_CNT);
 val = fw_GDM_RX_FC_DROP_CNT.Bits.GDM_RX_FC_DROP_CNT; //excel 54 read addr: 0x1FB5164C [31:00]
 printk("GDM_RX_FC_DROP_CNT : %x\n", val);

 fw_GDM_RX_RC_DROP_CNT.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_RX_RC_DROP_CNT);
 val = fw_GDM_RX_RC_DROP_CNT.Bits.GDM_RX_RC_DROP_CNT; //excel 55 read addr: 0x1FB51650 [31:00]
 printk("GDM_RX_RC_DROP_CNT : %x\n", val);

 fw_GDM_RX_OVER_DROP_CNT.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_RX_OVER_DROP_CNT);
 val = fw_GDM_RX_OVER_DROP_CNT.Bits.GDM_RX_OVER_DROP_CNT; //excel 56 read addr: 0x1FB51654 [31:00]
 printk("GDM_RX_OVER_DROP_CNT : %x\n", val);

 fw_GDM_RX_ERROR_DROP_CNT.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_RX_ERROR_DROP_CNT);
 val = fw_GDM_RX_ERROR_DROP_CNT.Bits.GDM_RX_ERROR_DROP_CNT; //excel 57 read addr: 0x1FB51658 [31:00]
 printk("GDM_RX_ERROR_DROP_CNT : %x\n", val);



//*******  check point 3: Tx path cnt


//*******  [FE] Tx cnt
 fw_GDM_TX_GET_CNT_L.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_TX_GET_CNT_L);
 val = fw_GDM_TX_GET_CNT_L.Bits.GDM_TX_GET_CNT_L; //excel 60 read addr: 0x1FB51600 [31:00]
 printk("GDM_TX_GET_CNT_L : %x\n", val);

 fw_GDM_TX_OK_CNT_L.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_TX_OK_CNT_L);
 val = fw_GDM_TX_OK_CNT_L.Bits.GDM_TX_OK_CNT_L; //excel 61 read addr: 0x1FB51604 [31:00]
 printk("GDM_TX_OK_CNT_L : %x\n", val);

 fw_GDM_TX_OK_CNT_H.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_TX_OK_CNT_H);
 val = fw_GDM_TX_OK_CNT_H.Bits.GDM_TX_OK_CNT_H; //excel 62 read addr: 0x1FB51780 [31:00]
 printk("GDM_TX_OK_CNT_H : %x\n", val);

 fw_GDM_TX_DROP_CNT.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _GDM_TX_DROP_CNT);
 val = fw_GDM_TX_DROP_CNT.Bits.GDM_TX_DROP_CNT; //excel 63 read addr: 0x1FB51608 [31:00]
 printk("GDM_TX_DROP_CNT : %x\n", val);



//*******  [MAC] Tx
 fw_xfi_tx_octets_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_tx_octets_cnt);
 val = fw_xfi_tx_octets_cnt.Bits.tx_octets_cnt; //excel 65 read addr: 0x1FA08104 [31:00]
 printk("tx_octets_cnt : %x\n", val);

 fw_xfi_tx_pkt_cnt.Raw.value = RG_R_PL(, _OFFSET, (RgAddr) _xfi_tx_pkt_cnt);
 val = fw_xfi_tx_pkt_cnt.Bits.tx_packet_cnt; //excel 66 read addr: 0x1FA08108 [31:00]
 printk("tx_packet_cnt : %x\n", val);

#endif

//*******  [PHYD] Rate adaption Tx cnt
 fw_rg_rate_adapt_ctrl_11.Bits.rg_rate_adapt_rd_toggle = 0x1; 
 RG_W_PL(RA_BASE, RA_BASE_OFFSET,(RgAddr) _multi_phy_ra_csr_rg_rate_adapt_ctrl_11, fw_rg_rate_adapt_ctrl_11.Raw.value); //excel 81, write addr: 0x1FA8602C = 0x5

 fw_rg_rate_adapt_ctrl_11.Bits.rg_rate_adapt_rd_toggle = 0x0; 
 RG_W_PL(RA_BASE, RA_BASE_OFFSET,(RgAddr) _multi_phy_ra_csr_rg_rate_adapt_ctrl_11, fw_rg_rate_adapt_ctrl_11.Raw.value); //excel 82, write addr: 0x1FA8602C = 0x4

 fw_ro_rate_adapt_sts_3.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_3);
 val = fw_ro_rate_adapt_sts_3.Bits.ro_rate_adapt_tx_mac_sop_cnt; //excel 83 read addr: 0x1FA8603C [31:00]
 printk("ro_rate_adapt_tx_mac_sop_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_4.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_4);
 val = fw_ro_rate_adapt_sts_4.Bits.ro_rate_adapt_tx_mac_eop_cnt; //excel 84 read addr: 0x1FA86040 [31:00]
 printk("ro_rate_adapt_tx_mac_eop_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_9.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_9);
 val = fw_ro_rate_adapt_sts_9.Bits.ro_rate_adapt_tx_mac_fe_cnt; //excel 85 read addr: 0x1FA86054 [31:00]
 printk("ro_rate_adapt_tx_mac_fe_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_1.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_1);
 val = fw_ro_rate_adapt_sts_1.Bits.ro_rate_adapt_tx_xfi_sop_cnt; //excel 86 read addr: 0x1FA86034 [31:00]
 printk("ro_rate_adapt_tx_xfi_sop_cnt : %x\n", val);

 fw_ro_rate_adapt_sts_2.Raw.value = RG_R_PL(RA_BASE, RA_BASE_OFFSET, (RgAddr) _multi_phy_ra_csr_ro_rate_adapt_sts_2);
 val = fw_ro_rate_adapt_sts_2.Bits.ro_rate_adapt_tx_xfi_eop_cnt; //excel 87 read addr: 0x1FA86038 [31:00]
 printk("ro_rate_adapt_tx_xfi_eop_cnt : %x\n", val);



//*******  [PHYD] PCS Tx cnt
 fw_rg_hsgmii_gpii_state_8.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_8);
 val = fw_rg_hsgmii_gpii_state_8.Bits.ro_fb_out_cnt; //excel 90 read addr: 0x1FA80B40 [31:00]
 printk("ro_fb_out_cnt : %x\n", val);

 fw_rg_hsgmii_gpii_state_9.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_9);
 val = fw_rg_hsgmii_gpii_state_9.Bits.ro_fd_out_cnt; //excel 91 read addr: 0x1FA80B44 [31:00]
 printk("ro_fd_out_cnt : %x\n", val);

 fw_rg_hsgmii_gpii_state_10.Raw.value = RG_R_PL(PCS2_BASE, PCS2_BASE_OFFSET, (RgAddr) _rg_hsgmii_gpii_state_10);
 val = fw_rg_hsgmii_gpii_state_10.Bits.ro_fe_out_cnt; //excel 92 read addr: 0x1FA80B48 [31:00]
 printk("ro_fe_out_cnt : %x\n", val);

	
}

#endif

static void XFI_Rx_Eye_Scan(u8 xsgmii, bool quick_flag)
{
	u32 val, zero_cnt, odd_even_cnt;
	int x, y, y_cur, x_step;

	odd_even_cnt = 0;
	zero_cnt = 0;
	x_step = (xsgmii == HSGMII) ? 1 : 2;

	 REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8 fw_PON_DA_CTRL_8;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5 fw_PON_DA_CTRL_5;
	 pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2 fw_PON_DA_CTRL_2;
	 REG_RG_SSUSB_LN0_RXAFE_DCMON_SEL fw_RG_SSUSB_LN0_RXAFE_DCMON_SEL;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3 fw_PON_RXFEDIG_STS_3;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1 fw_PON_RXFEDIG_STS_1;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2 fw_PON_RXFEDIG_STS_2;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_7 fw_PON_RXFEDIG_STS_7;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8 fw_PON_RXFEDIG_CTRL_8;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6 fw_PON_RXFEDIG_CTRL_6;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_7 fw_PON_RXFEDIG_CTRL_7;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0 fw_PON_RXFEDIG_CTRL_0;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_0 fw_PON_RXFEDIG_STS_0;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_4 fw_PON_RXFEDIG_STS_4;
	 pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_5 fw_PON_RXFEDIG_STS_5;
	 
	//*******  PI_EN
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS);
	 fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Bits.RG_SSUSB_LN0_CDR_EPEN = 0x1; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS, fw_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS.Raw); // write addr: 0x1FAF3024 = 0x18001702

	 fw_PON_DA_CTRL_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5);
	 fw_PON_DA_CTRL_5.Bits.RG_FORCE_CDR_PWD = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5, fw_PON_DA_CTRL_5.Raw); // write addr: 0x1FAF4614 = 0x10000000

	 fw_PON_DA_CTRL_8.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8);
	 fw_PON_DA_CTRL_8.Bits.RG_SSUSB_CDR_PWD = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8, fw_PON_DA_CTRL_8.Raw); // write addr: 0x1FAF4620 = 0x130D023

	 fw_PON_DA_CTRL_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2);
	 fw_PON_DA_CTRL_2.Bits.RG_SSUSB_CDR_PI_PWD = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2, fw_PON_DA_CTRL_2.Raw); // write addr: 0x1FAF4608 = 0x54101801



	//*******  Reset CDR PD
	 fw_RG_SSUSB_LN0_RXAFE_DCMON_SEL.Raw = RG_R_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_RXAFE_DCMON_SEL);
	 fw_RG_SSUSB_LN0_RXAFE_DCMON_SEL.Bits.RG_SSUSB_LN0_RX_PD_RST = 0x1; 
	 fw_RG_SSUSB_LN0_RXAFE_DCMON_SEL.Bits.RG_SSUSB_LN0_RX_PD_RST_PASS = 0x1; 
	 fw_RG_SSUSB_LN0_RXAFE_DCMON_SEL.Bits.RG_SSUSB_LN0_RX_PD_RST = 0x0; 
	 RG_W_PL(PHYA_BASE, PHYA_BASE_OFFSET,(RgAddr) _REG_RG_SSUSB_LN0_RXAFE_DCMON_SEL, fw_RG_SSUSB_LN0_RXAFE_DCMON_SEL.Raw); // write addr: 0x1FAF302C = 0x4050



	//*******  RX status readout
	 fw_PON_RXFEDIG_STS_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3);
	 val = fw_PON_RXFEDIG_STS_3.Bits.RGS_SSUSB_EQ_DCLEQ; // read addr: 0x1FAF4548 [11:08]
	 printk("RGS_SSUSB_EQ_DCLEQ : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1);
	 val = fw_PON_RXFEDIG_STS_1.Bits.RGS_SSUSB_EQ_DCD0H; // read addr: 0x1FAF4540 [06:00]
	 printk("RGS_SSUSB_EQ_DCD0H : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1);
	 val = fw_PON_RXFEDIG_STS_1.Bits.RGS_SSUSB_EQ_DCD0L; // read addr: 0x1FAF4540 [14:08]
	 printk("RGS_SSUSB_EQ_DCD0L : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1);
	 val = fw_PON_RXFEDIG_STS_1.Bits.RGS_SSUSB_EQ_DCD1H; // read addr: 0x1FAF4540 [22:16]
	 printk("RGS_SSUSB_EQ_DCD1H : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_1.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1);
	 val = fw_PON_RXFEDIG_STS_1.Bits.RGS_SSUSB_EQ_DCD1L; // read addr: 0x1FAF4540 [30:24]
	 printk("RGS_SSUSB_EQ_DCD1L : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2);
	 val = fw_PON_RXFEDIG_STS_2.Bits.RGS_SSUSB_EQ_DCE0; // read addr: 0x1FAF4544 [06:00]
	 printk("RGS_SSUSB_EQ_DCE0 : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2);
	 val = fw_PON_RXFEDIG_STS_2.Bits.RGS_SSUSB_EQ_DCE1; // read addr: 0x1FAF4544 [14:08]
	 printk("RGS_SSUSB_EQ_DCE1 : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3);
	 val = fw_PON_RXFEDIG_STS_3.Bits.RGS_SSUSB_EQ_DCHHL; // read addr: 0x1FAF4548 [06:00]
	 printk("RGS_SSUSB_EQ_DCHHL : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3);
	 val = fw_PON_RXFEDIG_STS_3.Bits.RGS_SSUSB_EQ_DCLHL; // read addr: 0x1FAF4548 [30:24]
	 printk("RGS_SSUSB_EQ_DCLHL : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2);
	 val = fw_PON_RXFEDIG_STS_2.Bits.RGS_SSUSB_EQ_DCEYE0; // read addr: 0x1FAF4544 [22:16]
	 printk("RGS_SSUSB_EQ_DCEYE0 : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_2.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2);
	 val = fw_PON_RXFEDIG_STS_2.Bits.RGS_SSUSB_EQ_DCEYE1; // read addr: 0x1FAF4544 [30:24]
	 printk("RGS_SSUSB_EQ_DCEYE1 : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_3.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3);
	 val = fw_PON_RXFEDIG_STS_3.Bits.RGS_SSUSB_EQ_DCLEQOS; // read addr: 0x1FAF4548 [20:16]
	 printk("RGS_SSUSB_EQ_DCLEQOS : 0x%x\n", val);

	 fw_PON_RXFEDIG_STS_7.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_7);
	 val = fw_PON_RXFEDIG_STS_7.Bits.RGS_SSUSB_EQ_PILPO; // read addr: 0x1FAF4554 [06:00]
	 printk("RGS_SSUSB_EQ_PILPO : 0x%x\n", val);



	//*******  Eye Scan init
	 fw_PON_DA_CTRL_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5);
	 fw_PON_DA_CTRL_5.Bits.RG_FORCE_RX_PI_CAL_EN = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5, fw_PON_DA_CTRL_5.Raw); // write addr: 0x1FAF4614 = 0x10020000

	 fw_PON_DA_CTRL_8.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8);
	 fw_PON_DA_CTRL_8.Bits.RG_SSUSB_RX_PI_CAL_EN = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8, fw_PON_DA_CTRL_8.Raw); // write addr: 0x1FAF4620 = 0x130D063

	 fw_PON_DA_CTRL_8.Bits.RG_SSUSB_RX_PI_CAL_EN = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8, fw_PON_DA_CTRL_8.Raw); // write addr: 0x1FAF4620 = 0x130D023

	 fw_PON_DA_CTRL_5.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5);
	 fw_PON_DA_CTRL_5.Bits.RG_FORCE_RX_PI_CAL_EN = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5, fw_PON_DA_CTRL_5.Raw); // write addr: 0x1FAF4614 = 0x10000000

	 fw_PON_RXFEDIG_CTRL_8.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8);
	 fw_PON_RXFEDIG_CTRL_8.Bits.RG_SSUSB_EQ_EYE_MASK = 0x3FF; 
	 fw_PON_RXFEDIG_CTRL_8.Bits.RG_SSUSB_EQ_EYE_XOFFSET = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8, fw_PON_RXFEDIG_CTRL_8.Raw); // write addr: 0x1FAF4520 = 0xF00003FF

	 fw_PON_RXFEDIG_CTRL_6.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6);
	 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE0_Y = 0x0; 
	 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE1_Y = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6, fw_PON_RXFEDIG_CTRL_6.Raw); // write addr: 0x1FAF4518 = 0x0

	 fw_PON_RXFEDIG_STS_7.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_7);
	 val = fw_PON_RXFEDIG_STS_7.Bits.RGS_SSUSB_EQ_PILPO; // read addr: 0x1FAF4554 [06:00]
	 printk("RGS_SSUSB_EQ_PILPO : 0x%x\n", val);

	 fw_PON_RXFEDIG_CTRL_7.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_7);
	 fw_PON_RXFEDIG_CTRL_7.Bits.RG_SSUSB_EQ_EYE_CNT = 0xC8; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_7, fw_PON_RXFEDIG_CTRL_7.Raw); // write addr: 0x1FAF451C = 0xC8

	 fw_PON_RXFEDIG_CTRL_0.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0);
	 fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_MON_EN = 0x0; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005409

	 fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_MON_EN = 0x1; 
	 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005609

	//*******  Eye Scan manual

	printk("\ndata start\n");	

	
	for(x=0;x>-64*x_step;x--)
	{	
		
		 fw_PON_RXFEDIG_CTRL_8.Bits.RG_SSUSB_EQ_EYE_XOFFSET = x&0x7f; 
		 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8, fw_PON_RXFEDIG_CTRL_8.Raw); // write addr: 0x1FAF4520 = 0xF00003FF


		fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x0; 
		RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005609

		fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x1; 
		RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005709
	
	}

	if (quick_flag != 1)
		for(y=0;y>-64;y--)
		{

			 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE0_Y = y&0x7f; 
			 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE1_Y = y&0x7f; 
			 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6, fw_PON_RXFEDIG_CTRL_6.Raw); // write addr: 0x1FAF4518 = 0x0

			fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x0; 
			RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005609

			fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x1; 
			RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005709			
		}

	for(x=-64*x_step; x<64*x_step; x++)
	{

		if (quick_flag == 1)
		{
			fw_PON_RXFEDIG_CTRL_8.Bits.RG_SSUSB_EQ_EYE_XOFFSET = x&0x7f; 
			RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8, fw_PON_RXFEDIG_CTRL_8.Raw); // write addr: 0x1FAF4520 = 0xF00003FF


			fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x0; 
			RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005609

			fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x1; 
			RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005709


			
			if(x%x_step==0)
			{

				udelay(100);
				//msleep(100);

				fw_PON_RXFEDIG_STS_4.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_4);
				val = fw_PON_RXFEDIG_STS_4.Bits.RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0; // read addr: 0x1FAF454C [19:00]			
				
				printk("%03d ",val);

				if (val == 0)
					zero_cnt++;				
			}
		}
		else
		{	
			 fw_PON_RXFEDIG_CTRL_8.Bits.RG_SSUSB_EQ_EYE_XOFFSET = x&0x7f; 
			 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8, fw_PON_RXFEDIG_CTRL_8.Raw); // write addr: 0x1FAF4520 = 0xF00003FF

			if(x % x_step == 0)
			{
								
				for(y=-64;y<64;y++)
				{
					if((odd_even_cnt%2) ==0)
						y_cur = y;
					else
						y_cur = -y-1;			

					 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE0_Y = y_cur&0x7f; 
					 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE1_Y = y_cur&0x7f; 
					 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6, fw_PON_RXFEDIG_CTRL_6.Raw); // write addr: 0x1FAF4518 = 0x0

					fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x0; 
					RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005609

					fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x1; 
					RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005709			
					 
					udelay(100);
					//msleep(100);
					
					fw_PON_RXFEDIG_STS_4.Raw = RG_R_PL(PMA_BASE, PMA_BASE_OFFSET, (RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_4);
					val = fw_PON_RXFEDIG_STS_4.Bits.RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0; // read addr: 0x1FAF454C [19:00]			
					printk("%03d ",val );
					
					if(((y+1)%32)==0)
						printk("z\n");
				}
				printk("x=%d\n",x );	
				odd_even_cnt++;
			}
		}
	}


	for(x=64*x_step; x>0; x--)
	{
		
		 fw_PON_RXFEDIG_CTRL_8.Bits.RG_SSUSB_EQ_EYE_XOFFSET = x&0x7f; 
		 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8, fw_PON_RXFEDIG_CTRL_8.Raw); // write addr: 0x1FAF4520 = 0xF00003FF

		fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x0; 
		RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005609

		fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x1; 
		RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005709

	}

	if (quick_flag == 1)
	{
		printk("\neye width at y=0 : %d \n",zero_cnt);
	}else
	{
		for(y=64; y>0; y--)
		{

			if((odd_even_cnt%2) ==0)
				y_cur = y;
			else
				y_cur = -y-1;			

			 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE0_Y = y_cur&0x7f; 
			 fw_PON_RXFEDIG_CTRL_6.Bits.RG_SSUSB_EQ_EYE1_Y = y_cur&0x7f; 
			 RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6, fw_PON_RXFEDIG_CTRL_6.Raw); // write addr: 0x1FAF4518 = 0x0

			fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x0; 
			RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005609

			fw_PON_RXFEDIG_CTRL_0.Bits.RG_SSUSB_EQ_EYE_CNT_EN = 0x1; 
			RG_W_PL(PMA_BASE, PMA_BASE_OFFSET,(RgAddr) _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, fw_PON_RXFEDIG_CTRL_0.Raw); // write addr: 0x1FAF4500 = 0x2005709			

		}
		
	}
}


#if 0 

static int do_eye_scan(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	void *buf, *buf2;
	unsigned long tmp = 0;
	ulong addr;

	unsigned long sum_y = 0;
	unsigned int zero_cnt = 0;
	bool quick_flag;
	int x,y,y_cur, x_input=0, left_x=0;	
	unsigned char x_step = 1;
	unsigned int odd_even_cnt = 0;
	unsigned int array[128];

	quick_flag = (simple_strtoul(argv[1], NULL, 10) ==1 )? 1: 0;

	if (argc > 2)
		x_step = simple_strtoul(argv[2], NULL, 10) ;

	if (argc > 3)
		x_input = simple_strtoul(argv[3], NULL, 10) ;
	

	printf("try ver 0629: quick y \n");
	
	buf = map_sysmem(0x1FAF3024, 4);
	*((u32 *)buf) = (u32) 0x18001702;
	udelay(100);	

	buf = map_sysmem(0x1FAF4614, 4);
	*((u32 *)buf) = (u32) 0x10000000;
	udelay(100);	

	buf = map_sysmem(0x1FAF4620, 4);
	*((u32 *)buf) = (u32) 0x130D023;
	udelay(100);	

	buf = map_sysmem(0x1FAF4608, 4);
	*((u32 *)buf) = (u32) 0x54101801;
	udelay(100);	

	buf = map_sysmem(0x1FAF302C, 4);
	*((u32 *)buf) = (u32) 0x4050;
	udelay(100);	


	buf = map_sysmem(0x1FAF4614, 4);
	*((u32 *)buf) = (u32) 0x10020000;
	udelay(100);	

	buf = map_sysmem(0x1FAF4620, 4);
	*((u32 *)buf) = (u32) 0x130D063;
	udelay(100);	

	buf = map_sysmem(0x1FAF4620, 4);
	*((u32 *)buf) = (u32) 0x130D023;
	udelay(100);	

	buf = map_sysmem(0x1FAF4614, 4);
	*((u32 *)buf) = (u32) 0x10000000;
	udelay(100);	

	buf = map_sysmem(0x1FAF4520, 4);
	*((u32 *)buf) = (u32) (0xF00003FF );
	udelay(100);	

	buf = map_sysmem(0x1FAF4518, 4);
	*((u32 *)buf) = (u32) 0;
	udelay(100);	

	buf = map_sysmem(0x1FAF451C, 4);
	*((u32 *)buf) = (u32) 0xC8;
	udelay(100);	


	buf = map_sysmem(0x1FAF4554, 4);
	if(buf)
		printf("EQ_PILPO: 0x%x \n",(*((u32 *)buf)));


	printf("\ndata start\n");
	
	for(x=0;x>-64*x_step;x--)
	{
		
		buf = map_sysmem(0x1FAF4520, 4);
		*((u32 *)buf) = (u32) (0xF00003FF |((x&0x7f)<<16));
		udelay(100);	

		buf = map_sysmem(0x1FAF4500, 4);
		*((u32 *)buf) = (u32) 0x2005e09;

		udelay(100);

		buf = map_sysmem(0x1FAF4500, 4);
		*((u32 *)buf) = (u32) 0x2005f09;

	}

	if(quick_flag != 1)
		for(y=0;y>-64;y--)
		{
			
			buf = map_sysmem(0x1FAF4518, 4);
			*((u32 *)buf) = (u32) ((y&0x7f) | ((y&0x7f)<<8));
			udelay(100);

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005e09;

			udelay(100);

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005f09;

		}


#if 0
	for(x=0;x>-65;x--)
	{
		//x_cur = x + 64;
		//if (quick_flag == 1)
		//{
			buf = map_sysmem(0x1FAF4520, 4);
			*((u32 *)buf) = (u32) (0xF00003FF |((x&0x7f)<<16));
			udelay(1000);

			//buf = map_sysmem(0x1FAF4518, 4);
			//*((u32 *)buf) = (u32) ((y&0x7f) | ((y&0x7f)<<8));
			//udelay(1000);

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005e09;

			udelay(1000);

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005f09;
			 
			udelay(5000);			
			
			buf = map_sysmem(0x1FAF454C, 4);
			//buf2 = map_sysmem(0x1FAF453C, 4);
			if(buf)
				printf("%03d ",(*((u32 *)buf)));
			if((*((u32 *)buf)) >0)
			{
				left_x = x;
				printf("left_x = %d \n",left_x);
				break;
			}
		//}
	}
#endif

/*
	if (quick_flag == 1)
		for(x=-64; x<left_x+1; x++)
			printf("%03d ",511);

*/
	
	//for(x=left_x+1; x<64; x++)
	for(x=-64*x_step; x<64*x_step; x++)
	{
		//x_cur = x + 64;
		if (quick_flag == 1)
		{
			buf = map_sysmem(0x1FAF4520, 4);
			*((u32 *)buf) = (u32) (0xF00003FF |((x&0x7f)<<16));
			udelay(100);

			buf = map_sysmem(0x1FAF4518, 4);
			*((u32 *)buf) = (u32) ((y&0x7f) | ((y&0x7f)<<8));
			udelay(100);

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005e09;

			udelay(100);

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005f09;
			 
			udelay(1000);			
			
			buf = map_sysmem(0x1FAF454C, 4);
			
			if((buf) && (x%x_step==0))
			{
				if (*((u32 *)buf) == 0)
					zero_cnt++;
				
				printf("%03d ",(*((u32 *)buf)));
			}


			//if(x == x_input * x_step)
			if(zero_cnt == x_input)
			{
				left_x = x;
				for(y=0;y>-64;y--)
				{
					
					buf = map_sysmem(0x1FAF4518, 4);
					*((u32 *)buf) = (u32) ((y&0x7f) | ((y&0x7f)<<8));
					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005e09;

					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005f09;

				}


				for(y=-64;y<64;y++)
				{	

					buf = map_sysmem(0x1FAF4518, 4);
					*((u32 *)buf) = (u32) ((y&0x7f) | ((y&0x7f)<<8));
					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005e09;

					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005f09;
					 
					udelay(1000);			
					
					buf = map_sysmem(0x1FAF454C, 4);
					//buf2 = map_sysmem(0x1FAF453C, 4);
					if(buf)
						array[y+64] = *((u32 *)buf);
				}


				for(y=63;y>0;y--)
				{
					
					buf = map_sysmem(0x1FAF4518, 4);
					*((u32 *)buf) = (u32) ((y&0x7f) | ((y&0x7f)<<8));
					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005e09;

					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005f09;

				}
				
			}
		}
		else
		{	
			buf = map_sysmem(0x1FAF4520, 4);
			*((u32 *)buf) = (u32) (0xF00003FF |((x&0x7f)<<16));
			udelay(100);

			if(x % x_step == 0)
			{
								
				for(y=-64;y<64;y++)
				{
					if((odd_even_cnt%2) ==0)
						y_cur = y;
					else
						y_cur = -y-1;			

					buf = map_sysmem(0x1FAF4518, 4);
					*((u32 *)buf) = (u32) ((y_cur&0x7f) | ((y_cur&0x7f)<<8));
					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005e09;

					udelay(100);

					buf = map_sysmem(0x1FAF4500, 4);
					*((u32 *)buf) = (u32) 0x2005f09;
					 
					udelay(1000);			
					
					buf = map_sysmem(0x1FAF454C, 4);
					//buf2 = map_sysmem(0x1FAF453C, 4);
					if(buf)
						printf("%03d ",(*((u32 *)buf)));
					
					if(((y+1)%32)==0)
						printf("z\n");
				}
				printf("x=%d\n",x );	
				odd_even_cnt++;
			}
		}
	}

	for(x=64*x_step; x>0; x--)
	{
		
		buf = map_sysmem(0x1FAF4520, 4);
		*((u32 *)buf) = (u32) (0xF00003FF |((x&0x7f)<<16));
		udelay(100);	

		buf = map_sysmem(0x1FAF4500, 4);
		*((u32 *)buf) = (u32) 0x2005e09;

		udelay(100);

		buf = map_sysmem(0x1FAF4500, 4);
		*((u32 *)buf) = (u32) 0x2005f09;

	}


	if (quick_flag == 1)
	{
		printf("\neye width at y=0 : %d \n",zero_cnt);

		printf("\neye scan at x=%d :\n",left_x);

		for(y=0; y<64; y++)
			printf("%03d ",array[y]);
		
	}else
		for(y=64; y>0; y--)
		{

			if((odd_even_cnt%2) ==0)
				y_cur = y;
			else
				y_cur = -y-1;			

			buf = map_sysmem(0x1FAF4518, 4);
			*((u32 *)buf) = (u32) ((y_cur&0x7f) | ((y_cur&0x7f)<<8));
			udelay(100);		

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005e09;

			udelay(100);

			buf = map_sysmem(0x1FAF4500, 4);
			*((u32 *)buf) = (u32) 0x2005f09;

		}

	printf("\n");

}


U_BOOT_CMD(
	eye,	4,	0,	do_eye_scan,
	"eye scan",
	"[none]"
);

#endif

void XPON_Test(u8 mod,u8 an, u8 rate){
/** NOT USED *****************************************************************************************************************
	switch (mod){
	case 0:
			switch(an){
				case 0 :				
					if(dbg_print) printk("pxp base %ps\n",_pxSGMII->phya_base);
					if(dbg_print) printk("pma base %ps\n",_pxSGMII->pma_base);	
					break;
				case 1 ://jcpll	
					switch(rate){
						case 0:
							JCPLL_LDO(1); //LDO follow pllen 
							break;
						case 1:
							JCPLL_RSTB(1); //just set high & always keep high
							break;
						case 2:
							JCPLL_EN(OFF);
							break;
						case 3:
							JCPLL_SDM(0,0,0);
							break;
						case 4:
							JCPLL_SSC(0,0,0,0,0);
							break;
						case 5:					
							JCPLL_LPF(0x18,0b01010,0x10,0x00,0x00);
							break;
						case 6:
							JCPLL_VCO();
							break;
						case 7:	
							JCPLL_PCW(0x25800000);
							break;
						case 8:
							JCPLL_DIV(0,0,0);
							break;
						case 9:
							JCPLL_KBand();
							break;
						case 10:					
							JCPLL_TCL(0xf,0x01,0b0101,0,1,1);
							break;
						case 11:
							JCPLL_EN(1);
							break;
						case 12:
							JCPLL_Out(1);
							break;
					}
				case 2:
					//add case
					break;
			}	
		break;
	case 1:
		xsgmii_init(0);
		break;
	case 2:
		xSGMII_AN_API(0,1);
		break;
	case 3:		
		AN_ReStart(0);
		break;
	case 4:
		break;
	}
***********************************************************************************************************************/
}
EXPORT_SYMBOL(XPON_Test);

/* NOT USED */
void XPON_RG_RW_API(u8 R,RgAddr rg,u32 data){
	if(R)printk("XPON_RG_RW : R addr %p data%x\n",rg, RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, rg));
}
EXPORT_SYMBOL(XPON_RG_RW_API);


/************************************************************************
*                  A P I s
*************************************************************************
*/
u32 XPON_xSGMII_API(u8 xsgmii,u8 mod,u8 an, u8 rate){	
	u32 data = 0x00;		
	if(dbg_print) printk("XPON_xSGMII_API xsgmii=%x,mod = %x,an=%x,rate=%x\n",xsgmii,mod,an,rate);

	if (xsgmii == USXGMII){
		printk("USXGMII Not supported.\n");
		data = 0xff;
		return data;
	}
		
	if (mod < 2)
	{
		/*MAC config*/
		xsgmii_chg(xsgmii);

		/* *** Not used ***
		CMN_EN(1);
		PLL_BringUp(xsgmii);
		Phya_BringUp(xsgmii);
		*/

		/*PHY init config*/
		xsgmii_init(xsgmii);
	}
	
	switch(mod){
		case 0: 
			if(dbg_print) printk(">>Force mode\n");
			xSGMII_Force_Mode_Setting(xsgmii, rate); //Biker fit Jay sv file
			
			xSGMII_AN_API(xsgmii,an);						
			xSGMII_Rate_API(xsgmii,rate);			
			if(an)AN_ReStart(xsgmii);
			break;
			
		case 1:
			if(dbg_print) printk(">>AN mode\n");
			
			xSGMII_AN_API(xsgmii,1);			
			xSGMII_AN_AutoSetting(xsgmii,0,1,rate); // 7522 is MAC, so work in MAC mode, equal bit mac_mode=0, a little confuse
			
			xSGMII_Rate_API(xsgmii,rate); //Biker fit Jay sv file
			break;
			
		case 2:
			if(dbg_print) printk(">>xSGMII Link Status\n");
			data = xSGMII_Link_STS(xsgmii);
			break;

		case 3:
			if(dbg_print) printk(">>xSGMII PHY Counter\n");
			xSGMII_PHY_Counter();
			break;

		/* FPGA_AN7552 Verify */
		case 4: 
			if(dbg_print) printk(">>For FPAGA_AN7552 verify\n");			
			/* PHY config (same as force mode)*/
			xSGMII_Rate_API(xsgmii,rate);
			xSGMII_AN_API(xsgmii,an);
			if(an)AN_ReStart(xsgmii);
			break;
			
		case 5: 
			if(dbg_print) printk(">>full rx eye scan\n");						
			XFI_Rx_Eye_Scan(xsgmii, 0); //full eye scan
			break;

		case 6: 
			if(dbg_print) printk(">>quick rx eye scan\n");						
			XFI_Rx_Eye_Scan(xsgmii, 1); //quick eye scan
			break;
			
		default:
			printk("xsgmii mod error!!!\n");
			data = 0xff;
			break;
			
	}	
	return data;
}
EXPORT_SYMBOL(XPON_xSGMII_API);


/** NOT USED *****************************************************************************************************************
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
		if(dbg_print) printk("xfi_pcs_int_sta_3 %x\n",xfi_pcs_int_sta_3.dat.value);
	}
	return sts;
}	
#endif
*******************************************************************************************************************/



#ifdef Enable_IRQ
static irqreturn_t xsgmii_interrupt(int irq, void *dev_id)
{
	printk("XPON IRQ\n");
	return IRQ_HANDLED;
}
#endif

static void xsgmii_get_base(xsgmii_base * _pxSGMII_Tmp){
	printk("xsgmii_get_base \n");
	_pxSGMII = _pxSGMII_Tmp;
		
	#ifdef RESOUCE_FROM_COMMON_MODULE
		_pxSGMII->phya_base = Get_Base(0x1faf3000);
		_pxSGMII->pma_base = Get_Base(0x1faf4000);
		_pxSGMII->multi_sgmii_base = Get_Base(0x1fa84000);
		
		
		if (_pxSGMII->phya_base == 0) _pxSGMII->phya_base = NULL;
		if (_pxSGMII->pma_base == 0) _pxSGMII->pma_base = NULL;
		if (_pxSGMII->multi_sgmii_base == 0) _pxSGMII->pma_base = NULL;
	#endif
	
}

static int xsgmii_drv_probe(struct platform_device *pdev)
{
	const char *int_name;
	int ret, int_id, region = 0;
	struct resource *res = NULL;
	xsgmii_base *_pxSGMII_Tmp = NULL;

	printk("\nXPON xsgmii_drv_probe\n");
	printk("\nXPON_XSGMII driver version: 7552.4.20230915\n improve jit tol setting \n"); 
	
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "\nNo xsgmii DT node found\n");
        return -EINVAL;
    }
	
	/* get int_id */
	ret = of_property_read_u32(pdev->dev.of_node, "int_id", &int_id);
	if(ret<0) printk("sgmii get propert error!\n");
	else printk("sgmii:int_id: %d\n",int_id);

	
	/* xsgmii allocate memory */
    _pxSGMII_Tmp = devm_kzalloc(&pdev->dev, sizeof(xsgmii_base), GFP_KERNEL);
    if (!_pxSGMII_Tmp)
        return -ENOMEM;

    platform_set_drvdata(pdev, _pxSGMII_Tmp);

	
	xsgmii_get_base(_pxSGMII_Tmp);
	

	/* get pcs2 base address_1fa80a00 */
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
	_pxSGMII->pcs2_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(_pxSGMII->pcs2_base)){
		printk("\nERROR(%s) pcs2_base\n", __func__);
		return PTR_ERR(_pxSGMII->pcs2_base);
	}
	

	/* get AN base address_1fa80000 */
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
	_pxSGMII->an_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(_pxSGMII->an_base)){
		printk("\nERROR(%s) an_base\n", __func__);
		return PTR_ERR(_pxSGMII->an_base);
   	}

	/* get RA base address_1fa86000 */
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
   	_pxSGMII->ra_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(_pxSGMII->ra_base)){
		printk("\nERROR(%s) ra_base\n", __func__);
		return PTR_ERR(_pxSGMII->ra_base);
   	}

	/* get Multi SGMII_1fa84000 */
	#if 0
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
	_pxSGMII->multi_sgmii_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(_pxSGMII->multi_sgmii_base)){
		printk("\nERROR(%s) multi_sgmii_base\n", __func__);
		return PTR_ERR(_pxSGMII->multi_sgmii_base);
	}
	#endif
	

	_pxSGMII->dev = &pdev->dev;


	/* get int_name */
	ret = of_property_read_string(pdev->dev.of_node, "int_name", &int_name);
	if(ret < 0)	printk("sgmii get propert error!");
	else printk("sgmii:int_name: %s\n",int_name);

	/* get IRQ */
	#ifdef Enable_IRQ
	
		/* get irq num */
		_pxSGMII->irq = platform_get_irq(pdev, 0);
		if(_pxSGMII->irq <= 0) {
			printk("\n get eth_xsgmii irq number failed\n");
			return _pxSGMII->irq;
		}
		else printk("irq = %x\n",_pxSGMII->irq);

		/* request irq */
		ret = request_irq(_pxSGMII->irq, xsgmii_interrupt, 0, int_name, &pdev->dev);
		if(ret) {
			printk("\n request_irq() (irq number: %d) failed (ret: %d)\n", _pxSGMII->irq, ret);
			eturn (ret);
		}
		else printk("\n request_irq() (irq number: %d) OK (ret: %d)\n", _pxSGMII->irq, ret);

	#endif
	
	
	#if 1
	
	printk("XOPN_xsgmii region= %x\n", region);
	printk("XOPN_xsgmii->pcs2_base			= %pS\n", _pxSGMII->pcs2_base);
	printk("XOPN_xsgmii->an_base			= %pS\n", _pxSGMII->an_base);
	printk("XOPN_xsgmii->ra_base			= %pS\n", _pxSGMII->ra_base);
	printk("XOPN_xsgmii->phya_base			= %pS\n", _pxSGMII->phya_base);
	printk("XOPN_xsgmii->multi_sgmii_base	= %pS\n", _pxSGMII->multi_sgmii_base);
	printk("XOPN_xsgmii->pma_base			= %pS\n", _pxSGMII->pma_base);

	#endif

	if(_pxSGMII == NULL)printk("XPON xsgmii drv probe ng\n");
	
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

