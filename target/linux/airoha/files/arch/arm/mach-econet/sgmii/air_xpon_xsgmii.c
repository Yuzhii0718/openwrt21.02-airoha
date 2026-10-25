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
#include "H/air_xpon_xsgmii_config.h"
#include "H/air_eth_xsgmii_EOscan.h"
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <asm/delay.h>

#include "uapi/ecnt_event_global/ecnt_event_serdes.h"
#include <ecnt_hook/ecnt_hook_hsgmii_mac.h>


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
static volatile xsgmii_base * _pxSGMII = NULL;//YT_20220622_0
static u8 xsgmii_bk = 0xff;

static u8 is_sgmii_100M = 0; 
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
extern u32 GET_PDIDR(void);

extern int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel);
extern struct en7581_serdes_common_phy* Get_Struct(void);
static void MAC_Linkup_Wrapper(void){
	if(dbg_print) printk("MAC_Linkup_Wrapper\n");
	//extern MAC function  

}
static void MAC_Linkdn_Wrapper(void){
	if(dbg_print) printk("MAC_Linkdn_Wrapper\n");
	//extern MAC function  
	XSI_MAC_LOGIC_RESET(XPON_INDEX);

}
static void MAC_SigDet_Wrapper(void){
	if(dbg_print) printk("MAC_SigDet_Wrapper\n");
	//extern MAC function  

}
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void XPON_xSGMII_Ver(void){
	printk("XPON XSGMII VER = %s\n",VER);
}
EXPORT_SYMBOL(XPON_xSGMII_Ver);
static void RG_W(REG_t* rg){	
#ifdef FPGA_PLATFORM
	if ((rg->base == _pxSGMII->phya_base) || (rg->base == _pxSGMII->pma_base)){
		printk("data %x\n",rg->data.value);
		printk("base %ps, addr %x, data %x\n",rg->base,rg->addr,rg->data.value);}
	else		
	writel(rg->data.value, rg->base + (rg->addr - rg->base_start));
#else
	writel(rg->data.value, rg->base + (rg->addr - rg->base_start));
	if(dbg_print){
                if(MAC_Polling_skip_rg_dump) return;
		rg->data.value = readl(rg->base + (rg->addr - rg->base_start));
		if (rg->base == _pxSGMII->pcs1_base)printk("memwl %lx %x\n",(0x1fa85000+(unsigned long)rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->pcs2_base)printk("memwl %lx %x\n",(0x1fa80000+(unsigned long)rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->an_base)printk("memwl %lx %x\n",(0x1fa80000+(unsigned long)rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->ra_base)printk("memwl %lx %x\n",(0x1fa86000+(unsigned long)rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->multi_sgmii_base)printk("memwl %lx %x\n",(0x1fa84000+(unsigned long)rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->phya_base)printk("memwl %lx %x\n",(0x1fa8a000+(unsigned long)rg->addr),rg->data.value);
		if (rg->base == _pxSGMII->pma_base)printk("memwl %lx %x\n",(0x1fa80000+(unsigned long)rg->addr),rg->data.value);	
	}
#endif
}

static u32 RG_R(REG_t* rg){
#ifdef FPGA_PLATFORM
	if ((rg->base != _pxSGMII->phya_base) && (rg->base != _pxSGMII->pma_base))
		return readl(rg->base + (rg->addr - rg->base_start));
	else
		return 0;
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
		wan_cfg &=(u8)~(0xFF);				
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
static void RX_FEGainPeaking(u8 force,u8 FEGain,u8 FE_Peaking){
	rg_type_t(HAL_rg_force_da_pxp_rx_fe_gain_ctrl) rg_force_da_pxp_rx_fe_gain_ctrl;
	rg_type_t(HAL_rg_da_pxp_jcpll_sdm_scan) rg_da_pxp_jcpll_sdm_scan;
	if(dbg_print) printk("RX_FE,force %x, Gain %x, Peaking %x\n",force,FEGain,FE_Peaking);

	if(!force){
		FEGain = 0;
		FE_Peaking = 0;
	}
	
	rg_force_da_pxp_rx_fe_gain_ctrl.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_gain_ctrl);
    rg_force_da_pxp_rx_fe_gain_ctrl.hal.rg_force_sel_da_pxp_rx_fe_gain_ctrl = force;
	rg_force_da_pxp_rx_fe_gain_ctrl.hal.rg_force_da_pxp_rx_fe_gain_ctrl = FEGain;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_gain_ctrl,rg_force_da_pxp_rx_fe_gain_ctrl.dat.value);

	rg_da_pxp_jcpll_sdm_scan.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_da_pxp_jcpll_sdm_scan);
    rg_da_pxp_jcpll_sdm_scan.hal.rg_force_sel_da_pxp_rx_fe_peaking_ctrl = force;
	rg_da_pxp_jcpll_sdm_scan.hal.rg_force_da_pxp_rx_fe_peaking_ctrl = FE_Peaking;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_da_pxp_jcpll_sdm_scan,rg_da_pxp_jcpll_sdm_scan.dat.value); 
}

static void  RX_Force_FEGain_FEPeaking(u8 xsgmii,u8 force){
	switch(xsgmii){
		case USXGMII:			
			RX_FEGainPeaking(force,USXGMII_RX_FEGain,USXGMII_RX_FEPeaking);
			break;
		case HSGMII:
			RX_FEGainPeaking(force,HSGMII_RX_FEGain,HSGMII_RX_FEPeaking);
			break;
		case SGMII:
			RX_FEGainPeaking(force,SGMII_RX_FEGain,SGMII_RX_FEPeaking);
			break;
	}
}

static void RX_FE_VOS(u8 xsgmii, u8 sel){
	rg_type_t(HAL_rg_force_da_pxp_rx_fe_vos) rg_force_da_pxp_rx_fe_vos;	
	if(dbg_print) printk("RX_FE_VOS\n");
	if(xsgmii == USXGMII) return;
	rg_force_da_pxp_rx_fe_vos.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_vos);
	rg_force_da_pxp_rx_fe_vos.hal.rg_force_sel_da_pxp_rx_fe_vos = sel;
	if(sel) rg_force_da_pxp_rx_fe_vos.hal.rg_force_da_pxp_rx_fe_vos = 0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_vos,rg_force_da_pxp_rx_fe_vos.dat.value);
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

static void PAD_PN_Swap(u8 sel, u8 swap){		
	
	RGDATA_t XPON_SETTING_0={0};	
	RGDATA_t ADD_DIG_RESERVE_0={0};	
	if(dbg_print) printk("TXRX_PN_Swap %x,%x\n",sel,swap);
	XPON_SETTING_0.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_SETTING_0);
	ADD_DIG_RESERVE_0.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_DIG_RESERVE_0);
	switch(sel){
		case 0:
			XPON_SETTING_0.bit.b9 = swap;
			break;
		case 1:
			ADD_DIG_RESERVE_0.bit.b0 = swap;
			break;
		case 2:
			XPON_SETTING_0.bit.b9 = swap;
			ADD_DIG_RESERVE_0.bit.b0 = swap;
			break;	
			
	}
	if(sel < 3)	{
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_SETTING_0,XPON_SETTING_0.value);
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_DIG_RESERVE_0,ADD_DIG_RESERVE_0.value);
	}
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

/*
SER rate control
00: 8 bit  (DA_XPON_RX_DATA[7:0]) /  8 bit  (DA_XFI_RX_DATA[7:0]) 
01: 10 bit  (DA_XPON_RX_DATA[9:0]) /  10 bit  (DA_XFI_RX_DATA[9:0])
10: 16 bit  (DA_XPON_RX_DATA[15:0]) / 16 bit  (DA_XFI_RX_DATA[15:0])
*/    
static void rx_rate_ctrl(u8 xsgmii){
    rg_type_t(HAL_RG_XPON_RX_RESERVED_1) RG_XPON_RX_RESERVED_1;
	rg_type_t(HAL_SS_RX_FLL_2) RG_SS_RX_FLL_2;
	if(dbg_print) printk("rx_rate_ctrl\n");
	RG_XPON_RX_RESERVED_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RG_XPON_RX_RESERVED_1);
    if (xsgmii == 0) RG_XPON_RX_RESERVED_1.hal.rg_xpon_rx_rate_ctrl = 0b10;
    else if (xsgmii == 1) {
	  if (RX_RATE_CTRL_FLL)
	  {    
            if(dbg_print) printk("RX_rate_ctrl FLL\n");
			RG_SS_RX_FLL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_2);
			RG_SS_RX_FLL_2.hal.rg_ck_rate =1 ;
	    	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_2,RG_SS_RX_FLL_2.dat.value);	
	  }
	  RG_XPON_RX_RESERVED_1.hal.rg_xpon_rx_rate_ctrl = 0b00;
	}
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

static void RX_IMP(u8 sel){
	rg_type_t(HAL_RG_PXP_RX_SIGDET_NOVTH) RG_PXP_RX_SIGDET_NOVTH;
	if(dbg_print) printk("RX_IMP %x\n",sel);
	RG_PXP_RX_SIGDET_NOVTH.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_SIGDET_NOVTH);
	RG_PXP_RX_SIGDET_NOVTH.hal.rg_pxp_rx_fe_50ohms_sel = sel;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_RX_SIGDET_NOVTH,RG_PXP_RX_SIGDET_NOVTH.dat.value);
}

static u8 RX_PRCal(u8 xsgmii)
{
	rg_type_t(HAL_RG_PXP_CDR_PR_INJ_MODE) RG_PXP_CDR_PR_INJ_MODE;
	rg_type_t(HAL_rg_force_da_pxp_cdr_pr_lpf_c_en) rg_force_da_pxp_cdr_pr_lpf_c_en;
	rg_type_t(HAL_rg_force_da_pxp_cdr_pr_idac) rg_force_da_pxp_cdr_pr_idac;
	rg_type_t(HAL_rg_force_da_pxp_cdr_pr_pieye_pwdb) rg_force_da_pxp_cdr_pr_pieye_pwdb;
	rg_type_t(HAL_SS_RX_FLL_b) SS_RX_FLL_b;	
	rg_type_t(HAL_SS_RX_FLL_1) SS_RX_FLL_1;
	rg_type_t(HAL_RO_RX_FREQDET) RO_RX_FREQDET;	
	rg_type_t(HAL_SS_RX_FREQ_DET_2) SS_RX_FREQ_DET_2;
	rg_type_t(HAL_SS_RX_FREQ_DET_1) SS_RX_FREQ_DET_1;
	rg_type_t(HAL_SS_RX_FREQ_DET_4) SS_RX_FREQ_DET_4;	
	rg_type_t(HAL_SS_RX_FREQ_DET_3) SS_RX_FREQ_DET_3;
	rg_type_t(HAL_SW_RST_SET) SW_RST_SET; 	

  	uint PrCal_Serach = 0 , RO_FL_Out = 0 , FL_Out_target = 0x9EDF ;
  	uint pr_idac = 0  , RO_pr_idac = 0;
  	int  cdr_pr_idac_tmp = 0, RO_state_freqdet = 0, turn_pr_idac_bit_position = 0;
	uint RO_FL_Out_diff = 0, RO_FL_Out_diff_tmp = 0xffff; 
	u16 lock_cyclecnt=0,unlock_cyclecnt=0;

	if(dbg_print) printk("RX_PRCal %x\n",xsgmii);
	SW_RST_SET.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SW_RST_SET);
	SW_RST_SET.hal.rg_sw_ref_rst_n = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SW_RST_SET,SW_RST_SET.dat.value);

	switch(xsgmii)
  	{
		case USXGMII:   //DS_10.3125G  /  US_1.25G
	   		FL_Out_target = 0x9EDF;
			lock_cyclecnt = 32767;
			unlock_cyclecnt = 32767;
			break;
	
		case HSGMII: //DS_9.95328G  /  US_9.95328G 
	   		FL_Out_target = 0xA000;
			lock_cyclecnt = 20000;
			unlock_cyclecnt = 20000;
			break;  

		case SGMII: //DS_1.25G      /  US_1.25G 
	   		FL_Out_target = 0xA3D6;
			lock_cyclecnt = 32767;
			unlock_cyclecnt = 32767;
			break;  	
	
		default:
			break;	
  	}
	
	SS_RX_FREQ_DET_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_2);
	SS_RX_FREQ_DET_2.hal.rg_lock_target_beg = FL_Out_target - 100;
	SS_RX_FREQ_DET_2.hal.rg_lock_target_end = FL_Out_target + 100;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_2,SS_RX_FREQ_DET_2.dat.value);

	SS_RX_FREQ_DET_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_1);
	SS_RX_FREQ_DET_1.hal.rg_lock_cyclecnt = lock_cyclecnt;
	SS_RX_FREQ_DET_1.hal.rg_unlock_cyclecnt = unlock_cyclecnt;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_1,SS_RX_FREQ_DET_1.dat.value);
		
	SS_RX_FREQ_DET_4.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4);
	SS_RX_FREQ_DET_4.hal.rg_lock_lockth = 3;	
	SS_RX_FREQ_DET_4.hal.rg_unlockth = 3;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4,SS_RX_FREQ_DET_4.dat.value);

	SS_RX_FREQ_DET_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_3);
	SS_RX_FREQ_DET_3.hal.rg_unlock_target_beg = FL_Out_target - 100;
	SS_RX_FREQ_DET_3.hal.rg_unlock_target_end = FL_Out_target + 100;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_3,SS_RX_FREQ_DET_3.dat.value);

	RG_PXP_CDR_PR_INJ_MODE.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_INJ_MODE);
	RG_PXP_CDR_PR_INJ_MODE.hal.rg_pxp_cdr_pr_inj_force_off = 1;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_INJ_MODE,RG_PXP_CDR_PR_INJ_MODE.dat.value);


	rg_force_da_pxp_cdr_pr_lpf_c_en.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_lpf_c_en);
	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_da_pxp_cdr_pr_lpf_c_en = 0;	  
	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_sel_da_pxp_cdr_pr_lpf_c_en = 1;
	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_da_pxp_cdr_pr_lpf_r_en = 1;
	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_sel_da_pxp_cdr_pr_lpf_r_en = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_lpf_c_en,rg_force_da_pxp_cdr_pr_lpf_c_en.dat.value);

	rg_force_da_pxp_cdr_pr_idac.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac);
	rg_force_da_pxp_cdr_pr_idac.hal.rg_force_sel_da_pxp_cdr_pr_idac = 1;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac,rg_force_da_pxp_cdr_pr_idac.dat.value);


	rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb);
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_pwdb = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pwdb = 0;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pwdb = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);

	for (PrCal_Serach = 1; PrCal_Serach < 8 ; PrCal_Serach++)
  	{		
		rg_force_da_pxp_cdr_pr_idac.hal.rg_force_da_pxp_cdr_pr_idac = PrCal_Serach<<8;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac,rg_force_da_pxp_cdr_pr_idac.dat.value);

	 	SS_RX_FREQ_DET_4.hal.rg_freqlock_det_en = 0x00;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4,SS_RX_FREQ_DET_4.dat.value);
		
	 	SS_RX_FREQ_DET_4.hal.rg_freqlock_det_en = 0x03;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4,SS_RX_FREQ_DET_4.dat.value);

	 	udelay(5000);
	 	RO_RX_FREQDET.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RO_RX_FREQDET);		
		RO_FL_Out = RO_RX_FREQDET.hal.ro_fl_out;
		rg_force_da_pxp_cdr_pr_idac.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac);
		RO_pr_idac = rg_force_da_pxp_cdr_pr_idac.hal.rg_force_da_pxp_cdr_pr_idac;
	 	if(dbg_print) printk("pr_idac = 0x%x ,RO_FL_Out = 0x%x\n" ,RO_pr_idac , RO_FL_Out);
		//abs
		/*		
				if (RO_FL_Out > FL_Out_target)
				{ 
				  RO_FL_Out_diff = RO_FL_Out - FL_Out_target;
				}
				else if (RO_FL_Out < FL_Out_target)
				{
				  RO_FL_Out_diff = FL_Out_target - RO_FL_Out;
				}
				else RO_FL_Out_diff = 0;
		*/			
		RO_FL_Out_diff =abs(FL_Out_target -RO_FL_Out);
	
	 	if(RO_FL_Out > FL_Out_target)
	 	{        
			RO_FL_Out_diff_tmp = RO_FL_Out_diff;
			cdr_pr_idac_tmp = (PrCal_Serach<<8);
		 	if(dbg_print) printk("cdr_pr_idac_tmp = 0x%x\n",cdr_pr_idac_tmp);		 
	 	}
  	}

  	for (turn_pr_idac_bit_position = 7; turn_pr_idac_bit_position > -1 ; turn_pr_idac_bit_position--)
  	{
		pr_idac = cdr_pr_idac_tmp |(0x1<<turn_pr_idac_bit_position); 
		rg_force_da_pxp_cdr_pr_idac.hal.rg_force_da_pxp_cdr_pr_idac = pr_idac;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac,rg_force_da_pxp_cdr_pr_idac.dat.value);

		SS_RX_FREQ_DET_4.hal.rg_freqlock_det_en = 0x0;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4,SS_RX_FREQ_DET_4.dat.value);

		SS_RX_FREQ_DET_4.hal.rg_freqlock_det_en = 0x03;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4,SS_RX_FREQ_DET_4.dat.value);
		
		udelay(5000);	  
	    
		RO_RX_FREQDET.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RO_RX_FREQDET);	  
		RO_FL_Out = RO_RX_FREQDET.hal.ro_fl_out;

		if(dbg_print) printk("pr_idac = 0x%x ,RO_FL_Out = 0x%x\n",pr_idac,RO_FL_Out);

		if(RO_FL_Out < FL_Out_target)
		{
	        pr_idac &= ~(0x1<<turn_pr_idac_bit_position);
			cdr_pr_idac_tmp = pr_idac;
			if(dbg_print) printk("cdr_pr_idac_tmp = 0x%x\n",cdr_pr_idac_tmp);
		}
		else
		{
			cdr_pr_idac_tmp = pr_idac;
			if(dbg_print)printk("cdr_pr_idac_tmp = 0x%x\n",cdr_pr_idac_tmp);
		}   
	  
  	}
	
	rg_force_da_pxp_cdr_pr_idac.dat.value = cdr_pr_idac_tmp;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac,rg_force_da_pxp_cdr_pr_idac.dat.value);
	if(dbg_print) printk("sel_cdr_pr_idac = 0x%x\n",cdr_pr_idac_tmp);


	SS_RX_FREQ_DET_4.hal.rg_freqlock_det_en = 0x00;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4,SS_RX_FREQ_DET_4.dat.value);

	SS_RX_FREQ_DET_4.hal.rg_freqlock_det_en = 0x03;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FREQ_DET_4,SS_RX_FREQ_DET_4.dat.value);

  	udelay(5000);
	RO_RX_FREQDET.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RO_RX_FREQDET);	  
  	RO_state_freqdet = RO_RX_FREQDET.hal.ro_fbck_lock; 

	if(dbg_print) printk("RO_state_freqdet = 0x%x\n",RO_state_freqdet);


   //Load_Band
	RG_PXP_CDR_PR_INJ_MODE.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_INJ_MODE);
	rg_force_da_pxp_cdr_pr_lpf_c_en.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_lpf_c_en);
	rg_force_da_pxp_cdr_pr_idac.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac);
	SS_RX_FLL_b.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_b);
	SS_RX_FLL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_1);
	rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb);

   	RG_PXP_CDR_PR_INJ_MODE.hal.rg_pxp_cdr_pr_inj_force_off = 0;
	RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_PR_INJ_MODE,RG_PXP_CDR_PR_INJ_MODE.dat.value);

	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_da_pxp_cdr_pr_lpf_c_en = 0;	
	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_sel_da_pxp_cdr_pr_lpf_c_en = 0;
	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_da_pxp_cdr_pr_lpf_r_en = 1;
	rg_force_da_pxp_cdr_pr_lpf_c_en.hal.rg_force_sel_da_pxp_cdr_pr_lpf_r_en = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_lpf_c_en,rg_force_da_pxp_cdr_pr_lpf_c_en.dat.value);
	
	rg_force_da_pxp_cdr_pr_idac.hal.rg_force_sel_da_pxp_cdr_pr_idac = 0;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_idac,rg_force_da_pxp_cdr_pr_idac.dat.value);

	SS_RX_FLL_b.hal.rg_load_en = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_b,SS_RX_FLL_b.dat.value);
	SS_RX_FLL_1.hal.rg_ipath_idac = cdr_pr_idac_tmp;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_1,SS_RX_FLL_1.dat.value);
	

	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pwdb = 0;	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pwdb = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);	
	rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_pwdb = 0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);

	SW_RST_SET.hal.rg_sw_ref_rst_n = RX_PRCal_REF_RESETB_HI_EN;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SW_RST_SET,SW_RST_SET.dat.value);
	return RO_state_freqdet;
}

static u8 RX_CDR_LFP_L2D_sta(void){
	rg_type_t(HAL_rg_force_da_pxp_cdr_lpf_lck2data) rg_force_da_pxp_cdr_lpf_lck2data;
	u8 sta; 
	rg_force_da_pxp_cdr_lpf_lck2data.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_lpf_lck2data);
	sta = rg_force_da_pxp_cdr_lpf_lck2data.hal.rg_force_sel_da_pxp_cdr_lpf_lck2data ;	
	if(dbg_print) printk("RX_CDR_LFP_L2D_sta %x\n",sta);
	return sta;
}
static void RX_CDR_LFP_L2D(u8 mod,u8 sel){
	rg_type_t(HAL_rg_force_da_pxp_cdr_lpf_lck2data) rg_force_da_pxp_cdr_lpf_lck2data;	
	if(dbg_print) printk("RX_CDR_LFP_L2D mode %x, sel %x\n",mod,sel);
	rg_force_da_pxp_cdr_lpf_lck2data.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_lpf_lck2data);

	//rg_force_da_pxp_cdr_lpf_lck2data.hal.rg_force_sel_da_pxp_cdr_lpf_lck2data = 1;	
	rg_force_da_pxp_cdr_lpf_lck2data.hal.rg_force_sel_da_pxp_cdr_lpf_lck2data = mod;
	rg_force_da_pxp_cdr_lpf_lck2data.hal.rg_force_da_pxp_cdr_lpf_lck2data = sel;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_lpf_lck2data,rg_force_da_pxp_cdr_lpf_lck2data.dat.value);
}


static void RX_CDR_LPF_RSTB(u8 mod,u8 sel){
	rg_type_t(HAL_rg_force_da_pxp_cdr_lpf_lck2data) rg_force_da_pxp_cdr_lpf_lck2data;	
	if(dbg_print) printk("RX_CDR_LPF_RSTB mode %x, sel%x\n",mod,sel);
	rg_force_da_pxp_cdr_lpf_lck2data.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_lpf_lck2data);

	//rg_force_da_pxp_cdr_lpf_lck2data.hal.rg_force_sel_da_pxp_cdr_lpf_rstb = 1;
	rg_force_da_pxp_cdr_lpf_lck2data.hal.rg_force_sel_da_pxp_cdr_lpf_rstb = mod;
	rg_force_da_pxp_cdr_lpf_lck2data.hal.rg_force_da_pxp_cdr_lpf_rstb = sel;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_lpf_lck2data,rg_force_da_pxp_cdr_lpf_lck2data.dat.value);
}

static u8 RX_RDY_Sta(void){
	rg_type_t(HAL_RX_CTRL_SEQUENCE_DISB_CTRL_1) RX_CTRL_SEQUENCE_DISB_CTRL_1;	
	RX_CTRL_SEQUENCE_DISB_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_DISB_CTRL_1);
	if(dbg_print) printk("RX_RDY_Sta %x \n",RX_CTRL_SEQUENCE_DISB_CTRL_1.hal.rg_disb_rx_rdy);
	return RX_CTRL_SEQUENCE_DISB_CTRL_1.hal.rg_disb_rx_rdy;
}

static void RX_RDY(u8 mod,u8 sel){
	rg_type_t(HAL_RX_CTRL_SEQUENCE_DISB_CTRL_1) RX_CTRL_SEQUENCE_DISB_CTRL_1;	
	rg_type_t(HAL_RX_CTRL_SEQUENCE_FORCE_CTRL_1) RX_CTRL_SEQUENCE_FORCE_CTRL_1;
	if(dbg_print) printk("RX_RDY %x, sel %x\n",mod,sel);
	RX_CTRL_SEQUENCE_DISB_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_DISB_CTRL_1);
	RX_CTRL_SEQUENCE_DISB_CTRL_1.hal.rg_disb_rx_rdy = mod;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_DISB_CTRL_1,RX_CTRL_SEQUENCE_DISB_CTRL_1.dat.value);


	RX_CTRL_SEQUENCE_FORCE_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_FORCE_CTRL_1);
	RX_CTRL_SEQUENCE_FORCE_CTRL_1.hal.rg_force_rx_rdy = sel;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_FORCE_CTRL_1,RX_CTRL_SEQUENCE_FORCE_CTRL_1.dat.value);
}

static void RX_CDR_RST(void){
	if(dbg_print) printk("RX_CDR_RST\n");
	RX_CDR_LFP_L2D(1,0);
	RX_CDR_LPF_RSTB(1,0);
	udelay(700);
	RX_CDR_LPF_RSTB(1,1);
	udelay(100);
	RX_CDR_LFP_L2D(1,1);

	//switch to auto
	RX_CDR_LPF_RSTB(0,1);
	RX_CDR_LFP_L2D(0,1);
}

static void RX_PR_CAL_SEQ(u8 xsgmii){//new
	if(dbg_print) printk("RX_PR_CAL_SEQ %x\n",xsgmii);
	if (!RX_PRCal_EN) return ;
	RX_CDR_LFP_L2D(1,0);
	RX_PRCal(xsgmii);
	RX_CDR_LPF_RSTB(1,0);
	udelay(700);
	RX_CDR_LPF_RSTB(1,1);
	udelay(100);
	RX_CDR_LFP_L2D(1,1);

	
	//switch to auto
	//udelay(700);
	RX_CDR_LPF_RSTB(0,1);
	//udelay(100);
	RX_CDR_LFP_L2D(0,1);
}

static void RX_INIT(u8 xsgmii){
	if(dbg_print) printk("RX_INIT\n");
	rx_rate_ctrl(xsgmii); //10.0.2
	RX_Path_Init();
	RX_Force_FEGain_FEPeaking(xsgmii,RX_Force_FEGain_FEPeaking_EN);
	RX_FE_VOS(xsgmii,RX_Force_FEOS_EN);
	if(!RX_EF_IMP_EN) RX_IMP(0b01);
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
	if(dbg_print) printk("RSTB sel%x, val %x\n",sel,val);
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
	if(!TX_EF_IMP_EN)
	{
		rg_force_da_pxp_tx_term_sel.hal.rg_force_sel_da_pxp_tx_term_sel = 1;
		rg_force_da_pxp_tx_term_sel.hal.rg_force_da_pxp_tx_term_sel = 0b01;
	}
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

static void TX_FIR_Load_Para(u8 xsgmii,u8 swing, u8 len, u8 en){
	u8 cn1=0,c0b=0,c1=0;
	u8 (*p)[6][3];
	p = (u8(*)[6][3])TX_FIR_SW500mV;//init
	if(en){
		switch(swing){
			case TX_SW500mV :
				p = (u8(*)[6][3])TX_FIR_SW500mV;
				break;	
			case TX_SW600mV :
				p = (u8(*)[6][3])TX_FIR_SW600mV;
				break;	
			case TX_SW660mV :
				p = (u8(*)[6][3])TX_FIR_SW660mV;
				break;
			case TX_SW800mV :
				p = (u8(*)[6][3])TX_FIR_SW800mV;
				break;	
		}
		
		cn1 = (*(p+xsgmii))[len][TX_FIR_CN1];
		c0b = (*(p+xsgmii))[len][TX_FIR_C0B];
		c1	= (*(p+xsgmii))[len][TX_FIR_C1];
	}
	else{
		switch(xsgmii){
			case USXGMII:
				cn1=0;
				c0b=0;
				c1=0;
				break;
			case HSGMII:
				cn1=0;
				c0b=0;
				c1=0;
				break;
			case SGMII:
				cn1=0;
				c0b=10;
				c1=0;
				break;
		}
	}
	
	if(dbg_print) printk("TX_FIR_Load_Para swing %x, len %x, cn1 %x, c0b %x, c1 %x, en %x\n",swing,len,cn1,c0b,c1,en);
	TX_FIR(cn1,c0b,c1,0);	
}

static void TX_BringUp(u8 xsgmii){
	if(dbg_print) printk("TX BringUp %x\n",xsgmii);
	tx_rate_ctrl(xsgmii);
	switch (xsgmii){
		case USXGMII:
			TX_CONFIG(1,0b101,0b10);
			//TX_FIR(0,0,0,0);
			break;
		case HSGMII:
			TX_CONFIG(1,0b100,0b01);
			//TX_FIR(0,0,0,0);
			break;
		case SGMII:
			TX_CONFIG(1,0b010,0b01);
			//TX_FIR(0,10,0,0);
			break;
		default:
			printk("warring!!! no such selection %x\n",xsgmii);
			printk("please input 0 or 1 or 2\n");
			TX_CONFIG(1,0b101,0b10);
			//TX_FIR(0,0,0,0);	            
			break;			
	}	
        TX_FIR_Load_Para(xsgmii,TX_SWING,TX_TRACE_LEN,XSGMII_TX_FIR_Load_Para_EN);
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
	if(!RX_PR_SEQ_NEW_EN)RX_PR_CAL_SEQ(xsgmii);
	RSTB(eM1,B_ON);
	udelay(100);//10.0.9	
	RSTB(eM1,B_OFF);	
	udelay(100);
	if(RX_PR_SEQ_NEW_EN)RX_PR_CAL_SEQ(xsgmii);
	if(RX_CDR_RST_NEW_EN)RX_CDR_RST();

}
static void xSGMII_AN_AutoSetting(u8 xsgmii,u8 mode,u8 duplx,u8 rate){
	rg_type_t(HAL_sgmii_reg_an_13) sgmii_reg_an_13;
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_6) rg_hsgmii_pcs_ctrol_6;
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;	

#ifdef FPGA_PLATFORM
	RGDATA_t rg={0};
	if(dbg_print) printk("xSGMII_AN_AutoSetting( %x,%x,%x,%x)\n",xsgmii, mode, duplx, rate);
	if(xsgmii == USXGMII){
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL); 
		rg.bit.b24 = 1; //bitfile0714
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)FPGA_STS_CTRL,rg.value); 			
	}	
#endif 
	if(dbg_print) printk("xSGMII_AN_AutoSetting\n");
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);	
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0;			
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg_rate_adapt_ctrl_0.dat.value); 
	
	if (xsgmii == SGMII){
		sgmii_reg_an_13.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13);
		sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x03;
		RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,sgmii_reg_an_13.dat.value);

		rg_hsgmii_pcs_ctrol_6.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_6);	
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_tx_enable = 1;
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_force_rateadapt = 1;
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_mode2_en = 1;
		rg_hsgmii_pcs_ctrol_6.hal.rg_sgmii_mac_mode = mode;
		RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_hsgmii_pcs_ctrol_6,rg_hsgmii_pcs_ctrol_6.dat.value);	
			
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
static void usxgmii_pcs_an_ctrl7(u8 an,u8 rate){ //update
	RGDATA_t rg={0};
	if(dbg_print) printk("usxgmii_pcs_an_ctrl7\n");
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_7);
	if (!an){
		rg.bit.b12 = 1;
		rg.value &= ~(0b111 << 8);
		rg.value |= rate << 8;
	}
	else{ //default value for AN disable & force mode
		rg.bit.b12 = 0;
		rg.value &= ~(0b111 << 8);
	}
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_7,rg.value);
}

static u8 USXGMII_AN_Sta(void){
	RGDATA_t rg={0}; 
	if(dbg_print) printk("USXGMII_AN_Sta\n");
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6);
	rg.bit.b0 = 1;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6,rg.value);
	rg.bit.b0 = 0;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6,rg.value);
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_ro_usxgmii_an_stats_2);	
	if(dbg_print) printk("USXGMII_AN_Sta %x\n",rg.bit.b24);
	return rg.bit.b24;
}

#if 0 //not use
static u8 USXGMII_AN_Timer_Sta(void){	
	RGDATA_t rg={0}; 
	if(dbg_print) printk("USXGMII_AN_Sta\n");
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6);
	rg.bit.b0 = 1;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6,rg.value);
	rg.bit.b0 = 0;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_6,rg.value);

	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_ro_usxgmii_an_stats_2);	
	rg.value &= 0x1fffff;
	if(dbg_print) printk("USXGMII_AN_Sta %x\n",rg.value);
	return rg.value;
}
#endif

static u32 USXGMII_AN_ReStart_Timer(u16 timeout){	
	RGDATA_t rg={0}; 
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_2,timeout & 0x7ff);
	
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_2);	
	if(dbg_print) printk("USXGMII_AN_ReStart_Timer %x\n",rg.value);
	return rg.value;
}

static u32 USXGMII_AN_Complete_Timer(u16 timeout){	
	RGDATA_t rg={0}; 
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_3,timeout & 0x7ff);
	
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_3);	
	if(dbg_print) printk("USXGMII_AN_Complete_Timer %x\n",rg.value);
	return rg.value;
}

static u32 USXGMII_AN_Idle_Timer(u16 timeout){	
	RGDATA_t rg={0}; 
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_4,timeout & 0x7ff);
	
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_4);	
	if(dbg_print) printk("USXGMII_AN_Idle_Timer %x\n",rg.value);
	return rg.value;
}


static void usxgmii_an(u8 enable)
{	
	RGDATA_t rg={0}; 
	if(dbg_print) printk("usxgmii_an\n");
	if(USX_ANTMR){
		USXGMII_AN_ReStart_Timer(0x7b*10);
		USXGMII_AN_Complete_Timer(0x7b*10);//1.6ms * 10
		USXGMII_AN_Idle_Timer(0x7b);
	}
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_0);

	//if ((rg.bit.b0 != enable))
	{
		rg.bit.b0 = enable;
//move to an restart
		//rg.bit.b8 = 1;
		//RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_usxgmii_an_control_0,rg.value);	
		//udelay(3);
		//rg.bit.b8 = 0;
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

#ifdef FPGA_PLATFORM
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

static void usxgmii_init(void){
	RGDATA_t rg;
    rg_type_t(HAL_rg_pcs_ctrol_1) rg_pcs_ctrol_1;
	if(dbg_print) printk("usxgmii_init %x\n",GET_PDIDR());
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x100); 
	rg.bit.b28 = 1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x100,rg.value);	
	if(dbg_print) printk("PCS E0/E1/E3 SWR solution\n");
	if (HIBER_OFF_EN){			
	   if(dbg_print) printk("HIBER_OFF\n");
	   rg_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_pcs_ctrol_1); 
	   rg_pcs_ctrol_1.hal.rg_speed_sel_h =0;
	   RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_pcs_ctrol_1,rg_pcs_ctrol_1.dat.value);
	}
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_vendor_define116,0x00); //avoid pcs sending garbage to mac,  only for E0	
}

static void hsgmii_init(void){

	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_1) rg_hsgmii_pcs_ctrol_1;	
	RGDATA_t rg;	
	if(dbg_print) printk("usxgmii_init\n");
	//PCS2
	rg_hsgmii_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1);
	rg_hsgmii_pcs_ctrol_1.hal.rg_tbi_10b_mode = 1;
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1,rg_hsgmii_pcs_ctrol_1.dat.value);

	//Multi Sgmii
	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
	rg.bit.b2 = 1;
	rg.bit.b4 = 1;
	rg.bit.b5 = 1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		
	if(dbg_print) printk("hsgmii_init\n");
}
static void sgmii_init(void){
	rg_type_t(HAL_rg_hsgmii_pcs_ctrol_1) rg_hsgmii_pcs_ctrol_1;
	rg_type_t(HAL_rg_rate_adapt_ctrl_6) rg_rate_adapt_ctrl_6;
	rg_type_t(HAL_rg_rate_adapt_ctrl_8) rg_rate_adapt_ctrl_8;
	RGDATA_t rg;	 
	
	rg_hsgmii_pcs_ctrol_1.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1);
	rg_hsgmii_pcs_ctrol_1.hal.rg_tbi_10b_mode = 1;
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_ctrol_1,rg_hsgmii_pcs_ctrol_1.dat.value);

	rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
	rg.bit.b2 = 1;
	rg.bit.b4 = 0;
	rg.bit.b5 = 1;
	RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		
	if(dbg_print) printk("sgmii_init\n");

	rg_rate_adapt_ctrl_6.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_6);
	rg_rate_adapt_ctrl_6.dat.value = 0x07070707;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_6,rg_rate_adapt_ctrl_6.dat.value);


	rg_rate_adapt_ctrl_8.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_8);
	rg_rate_adapt_ctrl_8.hal.rg_rate_adapt_rx_afifo_dout_c = 0xff;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_8,rg_rate_adapt_ctrl_8.dat.value);


}
static void xsgmii_init(u8 xsgmii){
	if(dbg_print) printk("xsgmii_init\n");
	switch (xsgmii){
		case 0:
			usxgmii_init();
			break;
		case 1:
			hsgmii_init();
			break;
		case 2:
			sgmii_init();
			break;
		default:
			usxgmii_init();
			break;
	}
}


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

static void HGMII_2p5G(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	if(dbg_print) printk("HGMII_2.5G\n");

	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 1;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg_rate_adapt_ctrl_0.dat.value);
	if(dbg_print) printk("HSGMII_2.5 exit\n");
}

static void SGMII_1G(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	RGDATA_t rg;	
	if(dbg_print) printk("SGMII_1G\n");
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 1;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg_rate_adapt_ctrl_0.dat.value);
			
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	//if(!sgmii_reg_an0.hal.sgmii_an_enable) //v10.0.11
	{
		//Multi Sgmii
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
		rg.bit.b2 = 1;
	rg.bit.b4 = 0;	
		rg.bit.b5 = 1;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		
	}	
	if(dbg_print) printk("SGMII_1G exit\n");
}
static void SGMII_100M(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	RGDATA_t rg;	
	if(dbg_print) printk("SGMII_100M\n");
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0; //v10.0.11
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0; //v10.0.11
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg_rate_adapt_ctrl_0.dat.value);
			
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	//if(!sgmii_reg_an0.hal.sgmii_an_enable)//v10.0.11
	{
		if (PCS_SGMII_force_mode_patch_EN == 1){
			rg.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)0xa24); 
			rg.bit.b0 = 1;
			rg.bit.b4 = 1;	
			rg.bit.b5 = 0;
			RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)0xa24,rg.value);		
		}
		//Multi Sgmii
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
		rg.bit.b2 = 1;
	rg.bit.b4 = 1;	
		rg.bit.b5 = 0;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		
	}	
	if(dbg_print) printk("SGMII_100M exit\n");
}
static void SGMII_10M(void){
	rg_type_t(HAL_rg_rate_adapt_ctrl_0) rg_rate_adapt_ctrl_0;
	rg_type_t(HAL_sgmii_reg_an0)sgmii_reg_an0;	
	RGDATA_t rg;	
	if(dbg_print) printk("SGMII_10M\n");
	rg_rate_adapt_ctrl_0.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0);
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_en = 1;
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_tx_bypass = 0;//v10.0.11
	rg_rate_adapt_ctrl_0.hal.rg_rate_adapt_rx_bypass = 0;//v10.0.11
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_0,rg_rate_adapt_ctrl_0.dat.value);
			
	sgmii_reg_an0.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an0);
	//if(!sgmii_reg_an0.hal.sgmii_an_enable)//v10.0.11
	{
		if (PCS_SGMII_force_mode_patch_EN == 1){
			rg.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)0xa24); 
			rg.bit.b0 = 1;
			rg.bit.b4 = 0;	
			rg.bit.b5 = 1;
			RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)0xa24,rg.value);		
		}		
		//Multi Sgmii
		rg.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18); 
		rg.bit.b2 = 1;
		rg.bit.b4 = 0;
		rg.bit.b5 = 0;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr)0x18,rg.value);		
	}	
	if(dbg_print) printk("SGMII_10M exit\n");
}


static void SGMII_RA_XGMII(u8 RA_XGMII){	
	RGDATA_t rg;
	rg.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)0x100);
	switch(RA_XGMII){
		case 1:
			rg.value = 0;
			break;
		case 2:
			rg.bit.b28 = 1;
			rg.bit.b31 = 1;
			break;
		default:
			break;
	}	
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)0x100,rg.value);
}
static void Solution0(u8 xsgmii,u8 mod,u8 an, u8 rate){ //sgmii force mode
	rg_type_t(HAL_sgmii_reg_an_13) sgmii_reg_an_13;
	rg_type_t(HAL_rg_an_sgmii_mode_force) rg_an_sgmii_mode_force;
	if((xsgmii == SGMII)&&(mod == 0)){
		if(dbg_print) printk("Solution0\n");
		sgmii_reg_an_13.dat.value = RG_R_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13);
		rg_an_sgmii_mode_force.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force);
		rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode_sel = 1;
		switch (rate){
			case ENUM_SGMII_1000M :
				sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x29;
				rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0b00;
			break;
		case ENUM_SGMII_100M :
				sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x25;				
				rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0b01;
			break;
		case ENUM_SGMII_10M :
				sgmii_reg_an_13.hal.sgmii_if_mode_5_0 = 0x21;				
				rg_an_sgmii_mode_force.hal.rg_force_cur_sgmii_mode = 0b10;
			break;	
		}
		RG_W_PL(_pxSGMII->an_base,SGMII_AN_BASE_OFFSET,(RgAddr)_sgmii_reg_an_13,sgmii_reg_an_13.dat.value);		
		RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr)_rg_an_sgmii_mode_force,rg_an_sgmii_mode_force.dat.value);
	}		
}

static void Solution1(u8 xsgmii,u8 mod,u8 an, u8 rate){ //reset rx fifo with thr
	rg_type_t(HAL_rg_rate_adapt_ctrl_1) rg_rate_adapt_ctrl_1;
	u32 tmp = 0;
	if((xsgmii == SGMII)&&(mod == 0)&&(an == 1)){
		if(dbg_print) printk("Solution1\n");
		rg_rate_adapt_ctrl_1.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_1);
		tmp = rg_rate_adapt_ctrl_1.dat.value;
		rg_rate_adapt_ctrl_1.hal.rg_rate_adapt_rx_afifo_wr_thr = 0;
		rg_rate_adapt_ctrl_1.hal.rg_rate_adapt_rx_afifo_rd_thr = 0;	
		RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_1,rg_rate_adapt_ctrl_1.dat.value);		
		udelay(5);
		RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)_rg_rate_adapt_ctrl_1,tmp);		
	}		
}

static void xsgmii_force_data(u8 xsgmii,u8 mod,u8 an, u8 rate){
	rg_type_t(HAL_rg_user_define_sel) rg_user_define_sel;	
	rg_type_t(HAL_rg_user_define_xgmii_control) rg_user_define_xgmii_control;
	rg_type_t(HAL_rg_user_define_xgmii_data_lsb) rg_user_define_xgmii_data_lsb;
	if(dbg_print)printk("USXGMII_force_data xsgmii %x, force %x\n",xsgmii,an);
	switch(xsgmii){
		case USXGMII:
			switch(rate){
				case 0:
					rg_user_define_xgmii_data_lsb.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_xgmii_data_lsb);			
					if(rg_user_define_xgmii_data_lsb.hal.rg_user_define_rxd_lsb != USR_DATA){
						rg_user_define_xgmii_data_lsb.hal.rg_user_define_rxd_lsb = USR_DATA;
						RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_xgmii_data_lsb,rg_user_define_xgmii_data_lsb.dat.value);
					}
					
						rg_user_define_xgmii_control.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_xgmii_control);
					if(rg_user_define_xgmii_control.hal.rg_user_define_rxc != 0xff){
						rg_user_define_xgmii_control.hal.rg_user_define_rxc = 0xff;				
						RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_xgmii_control,rg_user_define_xgmii_control.dat.value);				
					}
					
					rg_user_define_sel.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_sel);
					rg_user_define_sel.hal.rg_user_define_sel = an;
					RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_sel,rg_user_define_sel.dat.value);
					break;
				case 1:					
					rg_user_define_sel.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_sel);
					rg_user_define_sel.hal.rg_user_define_sel = an;
					RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr)_rg_user_define_sel,rg_user_define_sel.dat.value);
					break;
			}
			break;
		case HSGMII:	
		case SGMII:
			break;
	}
}


static void xSGMII_Solution(u8 number,u8 xsgmii,u8 mod,u8 an, u8 rate){	
	if(dbg_print) printk("xSGMII_Solution : %x\n",number);
	switch(number){
		case 0:
			Solution0(xsgmii,mod,an,rate);
			break;
		case 1:
			Solution1(xsgmii,mod,an,rate);
			break;	
	}
}



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
	
static void sgmii_rate_api(u8 rate){
	if(dbg_print) printk("sgmii_rate_api rate %x\n",rate);
	switch (rate){
		case ENUM_SGMII_1000M :
			SGMII_1G();
			break;
		case ENUM_SGMII_100M :
			SGMII_100M();
			break;
		case ENUM_SGMII_10M :
			SGMII_10M();
			break;	
		default :			
			SGMII_1G();
			break;			
	}
}	

static void hsgmii_rate_api(void){
	if(dbg_print) printk("hsgmii\n");
	HGMII_2p5G();
}
static void AN_ReStart(u8 xsgmii){
	//rg_type_t(HAL_ro_usxgmii_an_stats_0) ro_usxgmii_an_stats_0;	
	rg_type_t(HAL_rg_usxgmii_an_control_6) rg_usxgmii_an_control_6;
	RGDATA_t rg;
	//u16 loop;	
	if(dbg_print) printk("AN_ReStart\n");
	switch (xsgmii){
		case USXGMII :
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

#ifdef	FPGA_PLATFORM	 			
			ro_usxgmii_an_stats_0.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_usxgmii_an_stats_0);
			if(dbg_print) printk("AN_ReStart3 %x\n",ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode);
			if((ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode == 0x04) ||(ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode == 0x02))fpga_clk_for_usxgmii_rate(ENUM_USXGII_2p5G);
			else fpga_clk_for_usxgmii_rate(ENUM_USXGII_10G);	
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

static void xSGMII_Rate_API(u8 xsgmii,u8 rate){
	is_sgmii_100M = 0;
	switch (xsgmii){
		case USXGMII :
			usxgmii_rate_api(rate);
			break;
		case HSGMII:
			hsgmii_rate_api();
			break;
		case SGMII:
			sgmii_rate_api(rate);
			if (rate > 1)//100M & 10M
			  is_sgmii_100M =1;
			break;
		default :			
			USXGMII_10G();
			break;			
	}
}

static void xSGMII_AN_API(u8 xsgmii,u8 an){
	if(dbg_print) printk("xSGMII_AN_API %x, enable : %x\n",xsgmii,an);
	switch (xsgmii){
		case USXGMII :
		{
			usxgmii_an(an);
			break;
		}
		case HSGMII :
		{
			if(an) SGMII_RA_XGMII(1);
			hsgmii_an(an); //v10.0.11
			break;
		}
		case SGMII :
		{
			if(an) SGMII_RA_XGMII(1);
			sgmii_an(an);
			break;
		}
		default :			
		{
			usxgmii_an(0);
			sgmii_an(0);
			break;			
		}			
	}	
}
static void xSGMII_PD(u8 PD){
//	RGDATA_t rg = {0x00};
	if(dbg_print)printk("xSGMII_PD\n");
	RSTB(eM1,PD);
}	

static u32 usxgmii_link_sts(void){
	RGDATA_t rg = {0x00};
	rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_usxgmii_an_control_6);
	rg.bit.b0 = 1;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_usxgmii_an_control_6,rg.value);
	rg.bit.b0 = 0;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_usxgmii_an_control_6,rg.value);
	
	return RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_usxgmii_an_stats_0);
}


static u32 sgmii_link_sts(RgAddr multi_sgmii){
//	RGDATA_t rg = {0x00};	
	return RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) multi_sgmii);
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
	if(dbg_print) printk("link sts %x\n",sts.value);
	return sts.value;
}	

void XPON_Test(u8 mod,u8 an, u8 rate){
	#if 0
	switch (mod){
	case 0:
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
	#endif
}
EXPORT_SYMBOL(XPON_Test);

void XPON_RG_RW_API(u8 R,RgAddr rg,u32 data){
	if(R)printk("XPON_RG_RW : R addr %p data%x\n",rg, RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET, rg));
}
EXPORT_SYMBOL(XPON_RG_RW_API);

static u8 RX_SigDet_Flag(void){
	RGDATA_t rg;	
	u8 i,cnt = 0;		
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_DIG_RESERVE_0,0x30000);	
	for (i=0;i<=5;i++){		
		rg.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_DIG_RO_RESERVE_2);
		cnt += rg.bit.b8;
	}
	if(dbg_print)printk("RX_SigDet_Flag, cnt %x\n",cnt);
	
	return cnt >= 4? 1:0;  
}

#ifdef Enable_IRQ
static u8 RX_SigDet_Flag_D(void){
	rg_type_t(HAL_XPON_INT_EN_3) XPON_INT_EN_3;		
	rg_type_t(HAL_SS_RX_SIGDET_1) SS_RX_SIGDET_1;		
	rg_type_t(HAL_XPON_INT_STA_3) XPON_INT_STA_3;
	rg_type_t(HAL_RX_RESET_1) RX_RESET_1;	

	XPON_INT_EN_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_EN_3);	
	XPON_INT_EN_3.hal.rg_rx_sigdet_int_en = 0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_EN_3,XPON_INT_EN_3.dat.value);	

	SS_RX_SIGDET_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_SIGDET_1);	
	SS_RX_SIGDET_1.hal.rg_sigdet_en = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_SIGDET_1,SS_RX_SIGDET_1.dat.value);

	RX_RESET_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_1);	
	RX_RESET_1.hal.rg_sigdet_rst_b = 0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_1,RX_RESET_1.dat.value);
	
	RX_RESET_1.hal.rg_sigdet_rst_b = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_1,RX_RESET_1.dat.value);

	XPON_INT_STA_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3);	
	XPON_INT_STA_3.hal.rx_sigdet_int = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3,XPON_INT_STA_3.dat.value);	
	udelay(50);	
	XPON_INT_STA_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3);	

	SS_RX_SIGDET_1.hal.rg_sigdet_en = 0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_SIGDET_1,SS_RX_SIGDET_1.dat.value);
	
	if(dbg_print)printk("RX_SigDet_Flag_D %x\n",XPON_INT_STA_3.hal.rx_sigdet_int);
	return XPON_INT_STA_3.hal.rx_sigdet_int;  
}

static void SigDet_Int_Init(u8 en){
	rg_type_t(HAL_XPON_INT_EN_3) XPON_INT_EN_3;		
	rg_type_t(HAL_SS_RX_SIGDET_1) SS_RX_SIGDET_1;		
	rg_type_t(HAL_XPON_INT_STA_3) XPON_INT_STA_3;	
	rg_type_t(HAL_RX_RESET_1) RX_RESET_1;	

	if(dbg_print)printk("SigDet_Int_Init, en %x\n",en);	

	XPON_INT_STA_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3);	
	XPON_INT_STA_3.hal.rx_sigdet_int = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3,XPON_INT_STA_3.dat.value);	
	
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3,0x0);	
	XPON_INT_EN_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_EN_3);	
	XPON_INT_EN_3.hal.rg_rx_sigdet_int_en = en;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_EN_3,XPON_INT_EN_3.dat.value);	
	
	SS_RX_SIGDET_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_SIGDET_1);	
	SS_RX_SIGDET_1.hal.rg_sigdet_en = en;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_SIGDET_1,SS_RX_SIGDET_1.dat.value);	

	RX_RESET_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_1);	
	RX_RESET_1.hal.rg_sigdet_rst_b = 0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_1,RX_RESET_1.dat.value);
	
	RX_RESET_1.hal.rg_sigdet_rst_b = 1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_1,RX_RESET_1.dat.value);
}
static u8 SigDet_IntEn_sta(void){
	rg_type_t(HAL_XPON_INT_EN_3) XPON_INT_EN_3;	
	XPON_INT_EN_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_EN_3);
	return (u8) XPON_INT_EN_3.hal.rg_rx_sigdet_int_en;
}


static void sgmii_isr(void){
	rg_type_t(HAL_rg_hsgmii_mode_interrupt) rg_hsgmii_mode_interrupt;
	rg_type_t(HAL_rg_hsgmii_pcs_int_state) rg_hsgmii_pcs_int_state;
	rg_type_t(HAL_rg_rate_adapt_ctrl_1) rg_rate_adapt_ctrl_1;
	
	if(dbg_print)printk("xpon xsgmii: sgmii_isr\n");

	rg_hsgmii_pcs_int_state.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_pcs_int_state);	
	rg_hsgmii_mode_interrupt.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt);

	if (rg_hsgmii_pcs_int_state.hal.ro_hsgmii_mode2_an_done) 
	{
		if(dbg_print)printk("PON SGMII AN DONE INT\n");
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int				  =0x1;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int			  =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int		  =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int 				  =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int_clear			  =0x1;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int_clear 	  =0x1;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x1;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int_clear			  =0x1;
		RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt,rg_hsgmii_mode_interrupt.dat.value);

				
		rg_rate_adapt_ctrl_1.dat.value = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_1);
		rg_rate_adapt_ctrl_1.hal.rg_rate_adapt_rx_afifo_wr_thr = 0;
		rg_rate_adapt_ctrl_1.hal.rg_rate_adapt_rx_afifo_rd_thr = 0;
		RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_1,rg_rate_adapt_ctrl_1.dat.value);		
		if(!is_sgmii_100M)  udelay(1);
		rg_rate_adapt_ctrl_1.hal.rg_rate_adapt_rx_afifo_wr_thr = 0xf;
		rg_rate_adapt_ctrl_1.hal.rg_rate_adapt_rx_afifo_rd_thr = 0x5;
		RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_1,rg_rate_adapt_ctrl_1.dat.value);
	}	
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int				  =0x1;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int			  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int		  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int 				  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int_clear			  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int_clear 	  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x0;
	rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int_clear			  =0x0;
	RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt,rg_hsgmii_mode_interrupt.dat.value);
	
}

static void usxgmii_isr(void){	
	rg_type_t(HAL_xfi_pcs_int_sta_2) rg_xfi_pcs_int_sta_2; 	 
	rg_type_t(HAL_xfi_pcs_int_sta_3) rg_xfi_pcs_int_sta_3; 	 
	rg_type_t(HAL_xfi_pcs_int_sta_4) rg_xfi_pcs_int_sta_4; 	

	//rg_type_t(HAL_rg_xfi_pcs_int_ctrl_2) rg_xfi_pcs_int_ctrl_2;    
	//rg_type_t(HAL_rg_xfi_pcs_int_ctrl_3) rg_xfi_pcs_int_ctrl_3;	
	//rg_type_t(HAL_rg_xfi_pcs_int_ctrl_4) rg_xfi_pcs_int_ctrl_4;

	//rg_type_t(HAL_XPON_INT_EN_3) XPON_INT_EN_3;	
	rg_type_t(HAL_XPON_INT_STA_3) XPON_INT_STA_3;		

	u8 signal = XSGMII_SigDet_A_EN? RX_SigDet_Flag(): RX_SigDet_Flag_D();
	u16 sync = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_base_r_10gb_t_pcs_stus1);
	//u8 signal = RX_SigDet_Flag();
	u8 sd_int = 0;
	
	if(dbg_print)printk("xpon xsgmii: usxgmii_isr signal %x, sync %x\n",signal,sync);
//SD int
	if(SigDet_IntEn_sta()){
		XPON_INT_STA_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3);	
		if(dbg_print)printk("rx_sigdet_int %x\n",XPON_INT_STA_3.hal.rx_sigdet_int);

		if(XPON_INT_STA_3.hal.rx_sigdet_int)
		{			
			sd_int = 1;
			if(dbg_print)printk("---------------rx_sigdet_int isr *** ---------------\n");
			if(RX_CDR_LFP_L2D_sta()== 1) RX_CDR_RST();
			if(RX_RDY_Sta()== 0) RX_RDY(1,0);
			SigDet_Int_Init(0);
			if(XSGMII_SigDet_Wrapper_EN) MAC_SigDet_Wrapper();
		}
		
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3,XPON_INT_STA_3.dat.value);	
		if(dbg_print){
			XPON_INT_STA_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _XPON_INT_STA_3);	
			printk("_XPON_INT_STA_3 %x\n",XPON_INT_STA_3.dat.value);
		}
		if(dbg_print)printk("---------------rx_sigdet_int isr &&&---------------\n");
		
		if(sd_int){
			sd_int = 0;
			return;
		}
	}	

//Linkup int	
	rg_xfi_pcs_int_sta_2.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_2);	 
	rg_xfi_pcs_int_sta_3.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_3);	 
	rg_xfi_pcs_int_sta_4.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_4);	
	if(dbg_print)printk("Interrupt sta rg_xfi_pcs_int_sta_2= %x,rg_xfi_pcs_int_sta_3= %x,rg_xfi_pcs_int_sta_4= %x\n",rg_xfi_pcs_int_sta_2.dat.value,rg_xfi_pcs_int_sta_3.dat.value,rg_xfi_pcs_int_sta_4.dat.value);

	if(dbg_print)printk("link_up_st_int %x\n",rg_xfi_pcs_int_sta_3.hal.link_up_st_int);
	if(rg_xfi_pcs_int_sta_3.hal.link_up_st_int)	{
		if(dbg_print)printk("---------------link_up_st_int isr *** ---------------\n");
		if(signal) sync = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _ro_base_r_10gb_t_pcs_stus1);
		if(dbg_print)printk("link_up_st_int sync = %x\n",sync);
		if((0x100d == sync) && signal) {
			if(XSGMII_Linkup_Wrapper_EN)MAC_Linkup_Wrapper();
			linkup_sta = 1;
			if(USX_FORCE_USR_DATA) xsgmii_force_data(0,8,0,0);
		}	
		else linkup_sta = 0;
		if(dbg_print)printk("---------------link_up_st_int isr &&& ---------------\n");
	}
	else linkup_sta = 0;

//Linkdn int		
	if(dbg_print)printk("link_down_st_int %x\n",rg_xfi_pcs_int_sta_4.hal.link_down_st_int);
	if(rg_xfi_pcs_int_sta_4.hal.link_down_st_int){
		if(dbg_print)printk("---------------link_down_st_int isr *** ---------------\n");				
	    if(((0x100d != sync) && !signal )||((0x100d != sync) && XSGMII_Linkdn_Ignone_SD_EN)||(!linkup_sta && !signal && XSGMII_Linkdn_after_linkup_EN)){
			linkdn_sta = 1;
			if(USX_FORCE_USR_DATA)xsgmii_force_data(0,8,1,0);
			RX_RDY(0,0);
			RX_CDR_LFP_L2D(1,0);
			#if 0
				TMR_INI();
			#else
				SigDet_Int_Init(1);
			#endif
			if(XSGMII_Linkdn_Wrapper_EN)MAC_Linkdn_Wrapper();
		}
		else linkdn_sta = 0;
		if(dbg_print)printk("---------------link_down_st_int isr &&& ---------------\n");
	}
	else linkdn_sta = 0;
		
	if(dbg_print)printk("usxgmii linkup_sta=%x,linkdn_sta=%x\n",linkup_sta,linkdn_sta);
	
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_2,0x1010101);
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_3,0x1010101);
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_4,0x1);
	
	if(dbg_print){
		rg_xfi_pcs_int_sta_2.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_2);	 
		rg_xfi_pcs_int_sta_3.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_3);	 
		rg_xfi_pcs_int_sta_4.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_4);	
		printk("After clear, rg_xfi_pcs_int_sta_2= %x,rg_xfi_pcs_int_sta_3= %x,rg_xfi_pcs_int_sta_4= %x\n",rg_xfi_pcs_int_sta_2.dat.value,rg_xfi_pcs_int_sta_3.dat.value,rg_xfi_pcs_int_sta_4.dat.value);
	}
	
}

static irqreturn_t xsgmii_interrupt(int irq, void *dev_id){
	if(dbg_print) printk("---------------xpon xsgmii_isr:%x, irq=%d---------------\n",xsgmii_bk,irq);
	switch(xsgmii_bk){
		case USXGMII:
			usxgmii_isr();
			break;
		case SGMII :
			sgmii_isr();
			break;	
	}	
	return IRQ_HANDLED;
}

static void usxgmii_pcs_int_init(u8 en){
	
	rg_type_t(HAL_rg_xfi_pcs_int_ctrl_2) rg_xfi_pcs_int_ctrl_2;    
	rg_type_t(HAL_rg_xfi_pcs_int_ctrl_3) rg_xfi_pcs_int_ctrl_3;	
	rg_type_t(HAL_rg_xfi_pcs_int_ctrl_4) rg_xfi_pcs_int_ctrl_4;

	if(dbg_print)printk("usxgmii_pcs_int en %x\n",en);
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_0,0x00); 				
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_1,0x00); 				
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_2,0x00); 				
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_3,0x00); 	
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_4,0x00);

	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_2,0x1010101);
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_3,0x1010101);
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _xfi_pcs_int_sta_4,0x1);
	

			rg_xfi_pcs_int_ctrl_2.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_2);	
			rg_xfi_pcs_int_ctrl_3.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_3);	
			rg_xfi_pcs_int_ctrl_4.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_4);				

	rg_xfi_pcs_int_ctrl_2.hal.rg_r_type_e_int_en =0;			
	rg_xfi_pcs_int_ctrl_2.hal.rg_rxpcs_fsm_dec_err_int_en =0;	
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_2,rg_xfi_pcs_int_ctrl_2.dat.value);					
	rg_xfi_pcs_int_ctrl_3.hal.rg_hi_ber_st_int_en=0;	
	rg_xfi_pcs_int_ctrl_3.hal.rg_link_up_st_int_en=en;		
	rg_xfi_pcs_int_ctrl_3.hal.rg_rx_block_lock_st_int_en=0;
	rg_xfi_pcs_int_ctrl_3.hal.rg_fail_sync_xor_st_int_en=0;
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_3,rg_xfi_pcs_int_ctrl_3.dat.value);		
	rg_xfi_pcs_int_ctrl_4.hal.rg_link_down_st_int_en=en; 
	RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) _rg_xfi_pcs_int_ctrl_4,rg_xfi_pcs_int_ctrl_4.dat.value);
}

static void xsgmii_interrupt_init(u8 xsgmii){
	rg_type_t(HAL_rg_hsgmii_mode_interrupt) rg_hsgmii_mode_interrupt;		
	rg_type_t(HAL_INTERRUPT_EN_0) rg_INTERRUPT_EN_0;
	rg_type_t(HAL_multi_sgmii_reg_interrupt_sel) rg_multi_sgmii_reg_interrupt_sel;
	u8 en = 0;

	switch (xsgmii){
		case USXGMII:
			en = XSGMII_USXGMII_INT_EN;
			usxgmii_pcs_int_init(en);
			break;
		case HSGMII:
			en = 0;
			break;
		case SGMII:
			if(dbg_print)printk("SGMII_Interrupt_init\n");
			en = XSGMII_SGMII_INT_EN;
	rg_INTERRUPT_EN_0.dat.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) INTERRUPT_EN_0);
			rg_INTERRUPT_EN_0.hal.rg_pcs_int_en_0 = en;		
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) INTERRUPT_EN_0, rg_INTERRUPT_EN_0.dat.value);
			break;
	}
	
	if((!en)||(_pxSGMII->irq < 0)){
		if(! en)printk("xsgmii %x int enable 0\n",xsgmii);
		if(_pxSGMII->irq < 0)printk("_pxSGMII->irq NG\n");
		return;
	}
	else if(dbg_print)printk("xsgmii %x,int enable %x\n",xsgmii,en);

	
	if((XSGMII_REQ_IRQ == 0)){		
		XSGMII_REQ_IRQ = 1;
		if(request_irq(_pxSGMII->irq, xsgmii_interrupt, 0,xsgmii==0? "xpon_usxgmii":"xpon_sgmii", _pxSGMII->dev)){
			printk("\n request_irq() (irq number: %d) NG\n", _pxSGMII->irq);
			XSGMII_REQ_IRQ = 0;
			return;// _pxSGMII->irq;
		}
		else if(dbg_print) printk("\n request_irq() (irq number: %d) OK \n", _pxSGMII->irq);
	}
	
	switch(xsgmii){
		case USXGMII:
			if(dbg_print)printk("usxgmii int enable\n");
			break;
		case SGMII:
			rg_hsgmii_mode_interrupt.dat.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt);
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int				   =0x1;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int			   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int 	   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int			   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_done_int_clear 		   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_rx_sync_done_int_clear	   =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x0;
		rg_hsgmii_mode_interrupt.hal.rg_hsgmii_mode2_an_mis_int_clear	   =0x0;
		RG_W_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) _rg_hsgmii_mode_interrupt,rg_hsgmii_mode_interrupt.dat.value);

			#if 1
			rg_multi_sgmii_reg_interrupt_sel.dat.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) multi_sgmii_reg_interrupt_sel);	
		rg_multi_sgmii_reg_interrupt_sel.hal.rg_hsgmii_pcs_int = 1;
		//rg_multi_sgmii_reg_interrupt_sel.hal.rg_hsgmii_phy_int = 1;
		RG_W_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) multi_sgmii_reg_interrupt_sel, rg_multi_sgmii_reg_interrupt_sel.dat.value);
			#endif
	rg_INTERRUPT_EN_0.dat.value = 0;
	rg_INTERRUPT_EN_0.dat.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) INTERRUPT_EN_0);
	if(dbg_print) {
		printk("rg_INTERRUPT_EN_0 %x\n",rg_INTERRUPT_EN_0.dat.value);
		printk("xsgmii: sync_int exit %x\n",XSGMII_REQ_IRQ);
	}	
			break;
	}

	
}
#endif


static void XFI_eye_setting(uint bit_set)
{
    rg_type_t(HAL_RG_PXP_CDR_LPF_RATIO) RG_PXP_CDR_LPF_RATIO;
	rg_type_t(HAL_RX_EYE_TOP_EYECNT_CTRL_0) RX_EYE_TOP_EYECNT_CTRL_0;		
	rg_type_t(HAL_RX_EYE_TOP_EYEINDEX_CTRL_0) RX_EYE_TOP_EYEINDEX_CTRL_0;	
	rg_type_t(HAL_RX_EYE_TOP_EYECNT_CTRL_2) RX_EYE_TOP_EYECNT_CTRL_2;
	rg_type_t(HAL_RX_EYE_TOP_EYEINDEX_CTRL_1) RX_EYE_TOP_EYEINDEX_CTRL_1;
	rg_type_t(HAL_RX_EYE_TOP_EYEINDEX_CTRL_2) RX_EYE_TOP_EYEINDEX_CTRL_2;
	rg_type_t(HAL_RX_EYE_TOP_EYEINDEX_CTRL_3) RX_EYE_TOP_EYEINDEX_CTRL_3;
	rg_type_t(HAL_RX_EYE_TOP_EYEOPENING_CTRL_0) RX_EYE_TOP_EYEOPENING_CTRL_0;
	rg_type_t(HAL_RX_EYE_TOP_EYEOPENING_CTRL_1) RX_EYE_TOP_EYEOPENING_CTRL_1;
	rg_type_t(HAL_PHY_EQ_CTRL_1) PHY_EQ_CTRL_1;
	rg_type_t(HAL_PHY_EQ_CTRL_2) PHY_EQ_CTRL_2;

	RG_PXP_CDR_LPF_RATIO.dat.value = RG_R_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_LPF_RATIO);
	RX_EYE_TOP_EYECNT_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_0);
	RX_EYE_TOP_EYEINDEX_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_0);
	
	if (bit_set == EO_Async_XGPON_1 || bit_set == EO_Sync_XGSPON_2 || bit_set == EO_Sync_XGSPON_1 || bit_set == EO_Sync_XGSPON_2)
    {                	 
	    RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_ratio = 0x0;
	    RX_EYE_TOP_EYECNT_CTRL_0.hal.rg_eye_mask = 0xFF;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_min = 0x1c0;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_max = 0x240;	 
    }
    else if (bit_set == EO_Sync_GPON || bit_set == EO_Async_GPON)
    {                
	    RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_ratio = 0x2;
	    RX_EYE_TOP_EYECNT_CTRL_0.hal.rg_eye_mask = 0x55;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_min = 0x100;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_max = 0x300;
    }
    else if (bit_set == EO_Sync_EPON_1 || bit_set == EO_Sync_EPON_2 )
    {                
	    RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_ratio = 0x3;
	    RX_EYE_TOP_EYECNT_CTRL_0.hal.rg_eye_mask = 0x22;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_min = 0x0;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_max = 0x400;
    }
    // default choose 10G setting
    else
    {                	 
	    RG_PXP_CDR_LPF_RATIO.hal.rg_pxp_cdr_lpf_ratio = 0x0;
	    RX_EYE_TOP_EYECNT_CTRL_0.hal.rg_eye_mask = 0xFF;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_min = 0x1C0;
	    RX_EYE_TOP_EYEINDEX_CTRL_0.hal.rg_x_max = 0x234;
    }
        RG_W_PL(_pxSGMII->phya_base,PXP_BASE_OFFSET,(RgAddr) _RG_PXP_CDR_LPF_RATIO,RG_PXP_CDR_LPF_RATIO.dat.value);
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_0,RX_EYE_TOP_EYECNT_CTRL_0.dat.value);
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_0,RX_EYE_TOP_EYEINDEX_CTRL_0.dat.value);	 
	 

	    RX_EYE_TOP_EYECNT_CTRL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_2);
	    RX_EYE_TOP_EYECNT_CTRL_0.hal.rg_cntlen = 0xF8;
	    RX_EYE_TOP_EYECNT_CTRL_0.hal.rg_cntforever = 0x0;
	    RX_EYE_TOP_EYECNT_CTRL_2.hal.rg_data_shift = 0x0;
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_0,RX_EYE_TOP_EYECNT_CTRL_0.dat.value);
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_2,RX_EYE_TOP_EYECNT_CTRL_2.dat.value);
	 
	    RX_EYE_TOP_EYEINDEX_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_1);
        RX_EYE_TOP_EYEINDEX_CTRL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_2);	 
	    RX_EYE_TOP_EYEINDEX_CTRL_1.hal.rg_index_mode = 0x0;
	    RX_EYE_TOP_EYEINDEX_CTRL_2.hal.rg_eyedur = 0xFFF8;
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_1,RX_EYE_TOP_EYEINDEX_CTRL_1.dat.value);
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_2,RX_EYE_TOP_EYEINDEX_CTRL_2.dat.value);
	 
	    RX_EYE_TOP_EYEINDEX_CTRL_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_3);	
	    RX_EYE_TOP_EYEINDEX_CTRL_3.hal.rg_eye_nextpts_sel = 0x0;
	    RX_EYE_TOP_EYEINDEX_CTRL_3.hal.rg_eye_nextpts_toggle = 0x0;
	    RX_EYE_TOP_EYEINDEX_CTRL_3.hal.rg_eye_nextpts = 0x1;
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_3,RX_EYE_TOP_EYEINDEX_CTRL_3.dat.value);
	 
	    RX_EYE_TOP_EYEOPENING_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEOPENING_CTRL_0);	
	    RX_EYE_TOP_EYEOPENING_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEOPENING_CTRL_1);	
	    RX_EYE_TOP_EYEOPENING_CTRL_0.hal.rg_eyecnt_hth = 0x4;
	    RX_EYE_TOP_EYEOPENING_CTRL_0.hal.rg_eyecnt_vth = 0x4;
        RX_EYE_TOP_EYEOPENING_CTRL_1.hal.rg_eo_hth = 0x4;
	    RX_EYE_TOP_EYEOPENING_CTRL_1.hal.rg_eo_vth = 0x4;
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEOPENING_CTRL_0,RX_EYE_TOP_EYEOPENING_CTRL_0.dat.value);
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEOPENING_CTRL_1,RX_EYE_TOP_EYEOPENING_CTRL_1.dat.value);   

	    PHY_EQ_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_1);	
	    PHY_EQ_CTRL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_2);
	    PHY_EQ_CTRL_1.hal.rg_heo_emphasis = 0x0;
	    PHY_EQ_CTRL_1.hal.rg_a_lgain = 0x0;
        PHY_EQ_CTRL_1.hal.rg_a_mgain = 0x0;
	    PHY_EQ_CTRL_2.hal.rg_a_sel = 0x1;
	    PHY_EQ_CTRL_1.hal.rg_b_zero_sel = 0x0;
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_1,PHY_EQ_CTRL_1.dat.value);
	    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_2,PHY_EQ_CTRL_2.dat.value);	 
}

static void XFI_eye_Cal(void)
{
    rg_type_t(HAL_rg_force_da_pxp_tx_rate_ctrl) rg_force_da_pxp_tx_rate_ctrl;
	rg_type_t(HAL_rg_force_da_pxp_cdr_pr_fll_cor) rg_force_da_pxp_cdr_pr_fll_cor;
	rg_type_t(HAL_PHY_EQ_CTRL_0) PHY_EQ_CTRL_0;
	rg_type_t(HAL_SS_RX_PI_CAL) SS_RX_PI_CAL;
	rg_type_t(HAL_RX_RESET_0) RX_RESET_0;
	rg_type_t(HAL_RX_DISB_MODE_6) RX_DISB_MODE_6;
	rg_type_t(HAL_RX_FORCE_MODE_7) RX_FORCE_MODE_7;
	rg_type_t(HAL_RX_DISB_MODE_5) RX_DISB_MODE_5;
	rg_type_t(HAL_RX_FORCE_MODE_6) RX_FORCE_MODE_6;	
	rg_type_t(HAL_RX_DISB_MODE_3) RX_DISB_MODE_3;
	rg_type_t(HAL_RX_FORCE_MODE_3) RX_FORCE_MODE_3;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_DISB_CTRL_0) RX_CTRL_SEQUENCE_DISB_CTRL_0;
	rg_type_t(HAL_RX_CTRL_SEQUENCE_FORCE_CTRL_0) RX_CTRL_SEQUENCE_FORCE_CTRL_0;

	
	rg_force_da_pxp_tx_rate_ctrl.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_rate_ctrl);
	rg_force_da_pxp_tx_rate_ctrl.hal.rg_force_da_pxp_cdr_pr_pieye = 0x0;
	rg_force_da_pxp_tx_rate_ctrl.hal.rg_force_sel_da_pxp_cdr_pr_pieye = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_tx_rate_ctrl,rg_force_da_pxp_tx_rate_ctrl.dat.value);
	 
	rg_force_da_pxp_cdr_pr_fll_cor.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_fll_cor);
	rg_force_da_pxp_cdr_pr_fll_cor.hal.rg_force_da_pxp_rx_dac_eye = 0x0;
	rg_force_da_pxp_cdr_pr_fll_cor.hal.rg_force_sel_da_pxp_rx_dac_eye = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_fll_cor,rg_force_da_pxp_cdr_pr_fll_cor.dat.value);
	 
	//pical redo
    //reset block	 
	PHY_EQ_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_0);
	SS_RX_PI_CAL.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_PI_CAL);
	RX_RESET_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_0);
	PHY_EQ_CTRL_0.hal.rg_eq_en_delay = 0x80;
	SS_RX_PI_CAL.hal.rg_kpgain = 0x1;
	RX_RESET_0.hal.rg_eq_pi_cal_rst_b = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_0,PHY_EQ_CTRL_0.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_PI_CAL,SS_RX_PI_CAL.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_0,RX_RESET_0.dat.value);
	 
	RX_DISB_MODE_6.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_6);
	RX_FORCE_MODE_7.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_7);
	RX_DISB_MODE_6.hal.rg_disb_rx_and_pical_rstb = 0x0;
	RX_FORCE_MODE_7.hal.rg_force_rx_and_pical_rstb = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_6,RX_DISB_MODE_6.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_7,RX_FORCE_MODE_7.dat.value);	 
	 
	RX_DISB_MODE_6.hal.rg_disb_ref_and_pical_rstb = 0x0;
	RX_FORCE_MODE_7.hal.rg_force_ref_and_pical_rstb = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_6,RX_DISB_MODE_6.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_7,RX_FORCE_MODE_7.dat.value);	 
		
    //enable			 
	RX_DISB_MODE_5.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_5);
	RX_FORCE_MODE_6.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_6);
	RX_DISB_MODE_5.hal.rg_disb_rx_or_pical_en = 0x0;
	RX_FORCE_MODE_6.hal.rg_force_rx_or_pical_en = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_5,RX_DISB_MODE_5.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_6,RX_FORCE_MODE_6.dat.value);
	 
	RX_CTRL_SEQUENCE_DISB_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_DISB_CTRL_0);
	RX_CTRL_SEQUENCE_FORCE_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_FORCE_CTRL_0);
	RX_CTRL_SEQUENCE_DISB_CTRL_0.hal.rg_disb_rx_pical_en = 0x0;
	RX_CTRL_SEQUENCE_FORCE_CTRL_0.hal.rg_force_rx_pical_en = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_DISB_CTRL_0,RX_CTRL_SEQUENCE_DISB_CTRL_0.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_CTRL_SEQUENCE_FORCE_CTRL_0,RX_CTRL_SEQUENCE_FORCE_CTRL_0.dat.value);
		
    //release reset
	RX_RESET_0.hal.rg_eq_pi_cal_rst_b = 0x1;
	RX_FORCE_MODE_7.hal.rg_force_rx_and_pical_rstb = 0x1;
	RX_FORCE_MODE_7.hal.rg_force_ref_and_pical_rstb = 0x1;
	RX_FORCE_MODE_6.hal.rg_force_rx_or_pical_en = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_RESET_0,RX_RESET_0.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_7,RX_FORCE_MODE_7.dat.value);	 
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_6,RX_FORCE_MODE_6.dat.value);
		
    udelay(1000); //delay for 1ms 
	 
	RX_DISB_MODE_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_3);
	RX_FORCE_MODE_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_3);
	RX_FORCE_MODE_6.hal.rg_force_rx_or_pical_en = 0x0;
	RX_DISB_MODE_3.hal.rg_disb_eq_pi_cal_rdy = 0x0;
	RX_FORCE_MODE_3.hal.rg_force_eq_pi_cal_rdy = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_6,RX_FORCE_MODE_6.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_3,RX_DISB_MODE_3.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_3,RX_FORCE_MODE_3.dat.value);
}

static int XFI_readout_EO(int sweep_direction)
{
    volatile unsigned int eye_er, eye_el, eye_eu, eye_eb, i = 0;
    volatile unsigned int dac_d0, dac_d1, dac_e0, dac_e1, dac_eye, feos;
    volatile	unsigned int eye_x_done, eye_y_done, heo_rdy, veo_rdy;
    //volatile unsigned int veo;   
    volatile int fom_num,heo,heo_tmp , eye_er_tmp , eye_el_tmp;
    volatile unsigned int pical_data_out;   
    volatile unsigned int EO_Buf[10]= {0};   
    rg_type_t(HAL_SS_RX_FLL_6) SS_RX_FLL_6;
    rg_type_t(HAL_RX_DEBUG_0) RX_DEBUG_0;	
	rg_type_t(HAL_RX_TORGS_DEBUG_2) RX_TORGS_DEBUG_2;
	rg_type_t(HAL_RX_TORGS_DEBUG_9) RX_TORGS_DEBUG_9;
	rg_type_t(HAL_RX_TORGS_DEBUG_5) RX_TORGS_DEBUG_5;	
	rg_type_t(HAL_RX_TORGS_DEBUG_10) RX_TORGS_DEBUG_10;
	rg_type_t(HAL_RX_TORGS_DEBUG_11) RX_TORGS_DEBUG_11;
	rg_type_t(HAL_ADD_RO_RX2ANA_1) ADD_RO_RX2ANA_1;
	rg_type_t(HAL_ADD_RO_RX2ANA_2) ADD_RO_RX2ANA_2;
	rg_type_t(HAL_RX_DISB_MODE_0) RX_DISB_MODE_0;
	rg_type_t(HAL_RX_FORCE_MODE_0) RX_FORCE_MODE_0;	

    SS_RX_FLL_6.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_6);
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_1_latch_en = 0x01;
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_2_latch_en = 0x01;
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_3_latch_en = 0x01;
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_4_latch_en = 0x01;
    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_6,SS_RX_FLL_6.dat.value);
   
    udelay(50);
   
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_1_latch_en = 0x00;
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_2_latch_en = 0x00;
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_3_latch_en = 0x00;
    SS_RX_FLL_6.hal.ro_lnx_sw_fll_ro_4_latch_en = 0x00;
    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_FLL_6,SS_RX_FLL_6.dat.value);
 	
	RX_DEBUG_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DEBUG_0);
    RX_DEBUG_0.hal.rg_ro_toggle = 0x00;
    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DEBUG_0,RX_DEBUG_0.dat.value);
   
	udelay(100);
	
    RX_DEBUG_0.hal.rg_ro_toggle = 0x01;
    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DEBUG_0,RX_DEBUG_0.dat.value);

	RX_TORGS_DEBUG_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_TORGS_DEBUG_2);
	RX_TORGS_DEBUG_9.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_TORGS_DEBUG_9);
	RX_TORGS_DEBUG_5.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_TORGS_DEBUG_5);

	pical_data_out = RX_TORGS_DEBUG_2.hal.ro_pi_cal_data_out;
	eye_x_done = RX_TORGS_DEBUG_9.hal.eo_x_done;
	eye_y_done = RX_TORGS_DEBUG_9.hal.eo_y_done;
	heo_rdy = RX_TORGS_DEBUG_5.hal.heo_rdy;
	veo_rdy = RX_TORGS_DEBUG_5.hal.veo_rdy;

	if ((eye_x_done == 1) && (eye_y_done == 1))
    {	   
		if(dbg_print) printk("\n X&Y index scan done\n");
		if ((heo_rdy == 1) && (veo_rdy == 1)) {if(dbg_print) printk("\n heo&veo result ready\n");}
	}
	else if(dbg_print) printk("X&Y index scan NOT done\n");
	 
	RX_TORGS_DEBUG_10.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_TORGS_DEBUG_10);
	RX_TORGS_DEBUG_11.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_TORGS_DEBUG_11);

	eye_el = RX_TORGS_DEBUG_10.hal.eye_el;
	eye_er = RX_TORGS_DEBUG_10.hal.eye_er;
	eye_eu = RX_TORGS_DEBUG_11.hal.eye_eu;
	eye_eb = RX_TORGS_DEBUG_11.hal.eye_eb; 

	//ABS_Funt._0724 Morris
	eye_er_tmp = (int) eye_er;  //uint to int 
	eye_el_tmp = (int) eye_el;
		   
    heo_tmp= eye_er_tmp - eye_el_tmp;
	 
    if (heo_tmp>0)heo = heo_tmp;
    else heo = heo_tmp * -1;
	 
	ADD_RO_RX2ANA_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_RO_RX2ANA_1);
	ADD_RO_RX2ANA_2.dat.value= RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _ADD_RO_RX2ANA_2);

	dac_d0 = ADD_RO_RX2ANA_1.hal.ro_rx_dac_d0;
	dac_d1 = ADD_RO_RX2ANA_1.hal.ro_rx_dac_d1;
	dac_e0 = ADD_RO_RX2ANA_1.hal.ro_rx_dac_e0;
	dac_e1 = ADD_RO_RX2ANA_2.hal.ro_rx_dac_e1;
	dac_eye = ADD_RO_RX2ANA_1.hal.ro_rx_dac_eye;
	feos = ADD_RO_RX2ANA_2.hal.ro_rx_feos_out;
	   
	fom_num = heo;   
    
	//print out these results to textbox
     if (sweep_direction == 0) { if(dbg_print)printk("Y index sweep direction: bottom-up\n");}
     else if(dbg_print)printk("Y index sweep direction: upside-down\n");

	 EO_Buf[0] = eye_eu;
	 EO_Buf[1] = eye_eb;
	 EO_Buf[2] = dac_d0;
	 EO_Buf[3] = dac_d1;
	 EO_Buf[4] = dac_e0;
	 EO_Buf[5] = dac_e1;
	 EO_Buf[6] = dac_eye;
	 EO_Buf[7] = feos;

	for (i = 0; i < 8; i++)
	{
	    if ((EO_Buf[i] == feos) && (EO_Buf[i] >= 32))
		{
			EO_Buf[i] = EO_Buf[i] - 64;
		}
	    else if (EO_Buf[i] >= 64)
		{
		    EO_Buf[i] = EO_Buf[i] - 128;
		}
	}

	if(dbg_print) 
	{
		printk("fom_num => %d\n",fom_num); 
	    printk("pical_data_out => %d\n",pical_data_out);		   
	    printk("heo => %d\n",heo);		   
	    //printk("veo => %d\n",veo);		   
	    printk("X_done=> %d  Y_done= %d\n", eye_x_done, eye_y_done);	   
        printk("heo_rdy=> %d\n", heo_rdy);	   
	    printk("veo_rdy=> %d\n", veo_rdy);		  
	    printk("eye_er=> %d\n", eye_er);		   
	    printk("eye_el=> %d\n", eye_el);		  
	    printk("eye_eu=> %d\n", EO_Buf[0]);		   
	    printk("eye_eb=> %d\n", EO_Buf[1]);		   
	    printk("\n");
		   
	    printk("dac_d0=> %d\n", EO_Buf[2]);	
	    printk("dac_d1=> %d\n", EO_Buf[3]);	
	    printk("dac_e0=> %d\n", EO_Buf[4]);	
	    printk("dac_e1=> %d\n", EO_Buf[5]);	
	    printk("dac_eye=> %d\n", EO_Buf[6]);
	    printk("ro_feos=> %d\n", EO_Buf[7]);	
	    printk("\n");
	}	   

    if (heo_rdy != 1)
	{	
		if(dbg_print)printk("EO result isn't valid.\n");
	    RX_DISB_MODE_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_0);
	    RX_FORCE_MODE_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_0);
        RX_DISB_MODE_0.hal.rg_disb_da_xpon_cdr_lpf_rstb = 0x00;
        RX_FORCE_MODE_0.hal.rg_force_da_xpon_cdr_lpf_rstb = 0x00;
        RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_0,RX_DISB_MODE_0.dat.value);
        RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_0,RX_FORCE_MODE_0.dat.value);
	 
	    udelay(500); //delay for 500us 	  
     
        RX_FORCE_MODE_0.hal.rg_force_da_xpon_cdr_lpf_rstb = 0x01;
        RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_0,RX_FORCE_MODE_0.dat.value);
	 
	    udelay(500); //delay for 500us 
	 }
   return fom_num;
}	  


static int XFI_eye_EO(uint bit_rate)
{
    volatile unsigned int fom_num,i=0;
	 
	rg_type_t(HAL_RX_EYE_TOP_EYECNT_CTRL_0) RX_EYE_TOP_EYECNT_CTRL_0;
	rg_type_t(HAL_RX_EYE_TOP_EYEINDEX_CTRL_2) RX_EYE_TOP_EYEINDEX_CTRL_2;
	rg_type_t(HAL_RX_DISB_MODE_8) RX_DISB_MODE_8;
	rg_type_t(HAL_RX_FORCE_MODE_9) RX_FORCE_MODE_9;
	rg_type_t(HAL_RX_DISB_MODE_2) RX_DISB_MODE_2;
	rg_type_t(HAL_RX_EYE_TOP_EYECNT_CTRL_1) RX_EYE_TOP_EYECNT_CTRL_1;
	rg_type_t(HAL_RX_DISB_MODE_7) RX_DISB_MODE_7;
	rg_type_t(HAL_RX_DISB_MODE_3) RX_DISB_MODE_3;
	rg_type_t(HAL_RX_FORCE_MODE_3) RX_FORCE_MODE_3;

	
	RX_EYE_TOP_EYECNT_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_0);
	RX_EYE_TOP_EYEINDEX_CTRL_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_2);
	RX_EYE_TOP_EYECNT_CTRL_0.hal.rg_cntlen = 0xA;
	RX_EYE_TOP_EYEINDEX_CTRL_2.hal.rg_eyedur = 0x44C;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_0,RX_EYE_TOP_EYECNT_CTRL_0.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYEINDEX_CTRL_2,RX_EYE_TOP_EYEINDEX_CTRL_2.dat.value);

	//reset eye_top               
	RX_DISB_MODE_8.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_8);
	RX_FORCE_MODE_9.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9);
	RX_DISB_MODE_8.hal.rg_disb_eye_reset_plu_o = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_8,RX_DISB_MODE_8.dat.value);
	RX_FORCE_MODE_9.hal.rg_force_eye_reset_plu_o = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9,RX_FORCE_MODE_9.dat.value);
	RX_FORCE_MODE_9.hal.rg_force_eye_reset_plu_o = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9,RX_FORCE_MODE_9.dat.value);  
	 
	RX_DISB_MODE_8.hal.rg_disb_eye_top_en = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_8,RX_DISB_MODE_8.dat.value);
	RX_FORCE_MODE_9.hal.rg_force_eye_top_en = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9,RX_FORCE_MODE_9.dat.value);
	RX_FORCE_MODE_9.hal.rg_force_eye_top_en = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9,RX_FORCE_MODE_9.dat.value);
 
    if (bit_rate == EO_Async_XEPON || bit_rate == EO_Sync_XEPON || bit_rate == EO_Async_XGPON_1 || bit_rate == EO_Async_XGPON_2 || bit_rate == EO_Sync_XGSPON_1 || bit_rate == EO_Sync_XGSPON_2)
    {
        udelay(5500); //delay for 5.5ms  
    }
	else
	{
	    for (i=0;i<10;i++)
		{
		    udelay(5500); //delay for 55ms  Compile Error for udelay(55000)
		}
	}
	 
	RX_DISB_MODE_2.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_2);
	RX_EYE_TOP_EYECNT_CTRL_1.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_1);
	RX_DISB_MODE_7.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_7);
	 
	RX_DISB_MODE_2.hal.rg_disb_da_xpon_rx_dac_eye = 0x1;
	RX_DISB_MODE_2.hal.rg_disb_da_xpon_cdr_pr_pieye = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_2,RX_DISB_MODE_2.dat.value);
	RX_EYE_TOP_EYECNT_CTRL_1.hal.rg_disb_eyedur_init_b = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_1,RX_EYE_TOP_EYECNT_CTRL_1.dat.value);	 
	RX_DISB_MODE_7.hal.rg_disb_eyecnt_rx_rst_b = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_7,RX_DISB_MODE_7.dat.value);
	RX_EYE_TOP_EYECNT_CTRL_1.hal.rg_disb_eyedur_en = 0x1;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_EYE_TOP_EYECNT_CTRL_1,RX_EYE_TOP_EYECNT_CTRL_1.dat.value);

    //read out EO results
    fom_num = XFI_readout_EO(0); 

	//clear EXE_X_SW offset value
	RX_FORCE_MODE_9.hal.rg_force_eye_reset_plu_o = 0x1;
	RX_DISB_MODE_8.hal.rg_disb_eye_top_en = 0x0;
	RX_FORCE_MODE_9.hal.rg_force_eye_top_en = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_8,RX_DISB_MODE_8.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_9,RX_FORCE_MODE_9.dat.value);	 

    //reset pi_cal_rdy	 
	RX_DISB_MODE_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_3);
	RX_FORCE_MODE_3.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_3);
	RX_DISB_MODE_3.hal.rg_disb_eq_pi_cal_rdy = 0x0;
	RX_FORCE_MODE_3.hal.rg_force_eq_pi_cal_rdy = 0x0;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_DISB_MODE_3,RX_DISB_MODE_3.dat.value);
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _RX_FORCE_MODE_3,RX_FORCE_MODE_3.dat.value);

    return fom_num;
}

	  

static void EO_Scan(uint bit_sel, uint EO_start, uint EO_end)
{
    volatile int j = 0;
    volatile unsigned int leq_gain = tFE_Gain;            
    volatile unsigned int leq_peaking = 0;
    volatile unsigned int sweep_start = EO_start;
    volatile unsigned int sweep_end = EO_end ;            
    volatile unsigned int fom_num = 0;
    volatile unsigned int best_fom = 0;   
    volatile unsigned int best_gain = leq_gain;
    volatile unsigned int best_peaking = 0; 	
	volatile unsigned int fom_num_list[8] = {}; 	
	rg_type_t(HAL_PHY_EQ_CTRL_0) PHY_EQ_CTRL_0;
	rg_type_t(HAL_SS_RX_PI_CAL) SS_RX_PI_CAL;
    rg_type_t(HAL_rg_da_pxp_jcpll_sdm_scan) rg_da_pxp_jcpll_sdm_scan;
	rg_type_t(HAL_rg_force_da_pxp_cdr_pr_pieye_pwdb) rg_force_da_pxp_cdr_pr_pieye_pwdb;
	rg_type_t(HAL_rg_force_da_pxp_rx_fe_gain_ctrl) rg_force_da_pxp_rx_fe_gain_ctrl;


	if(dbg_print) printk("bit_sel => %x \n" ,bit_sel);

	//if(bit_sel == Async_XEPON || bit_sel == Sync_XEPON || bit_sel == Async_XGPON_1 || bit_sel == Async_XGPON_2 || bit_sel == Sync_XGSPON_1 || bit_sel == Sync_XGSPON_2)	 
	{
    	rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb);
		rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_sel_da_pxp_cdr_pr_pieye_pwdb = 0x1;
		rg_force_da_pxp_cdr_pr_pieye_pwdb.hal.rg_force_da_pxp_cdr_pr_pieye_pwdb = 0x1;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_cdr_pr_pieye_pwdb,rg_force_da_pxp_cdr_pr_pieye_pwdb.dat.value);

    	XFI_eye_setting(bit_sel);

    	//EYE setting, EYE OPEN related  
		PHY_EQ_CTRL_0.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_0);
		SS_RX_PI_CAL.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_PI_CAL);
		PHY_EQ_CTRL_0.hal.rg_eq_en_delay = 0x80;
		SS_RX_PI_CAL.hal.rg_kpgain = 0x04;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _PHY_EQ_CTRL_0,PHY_EQ_CTRL_0.dat.value);
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _SS_RX_PI_CAL,SS_RX_PI_CAL.dat.value);
		
		rg_force_da_pxp_rx_fe_gain_ctrl.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_gain_ctrl);
		rg_force_da_pxp_rx_fe_gain_ctrl.hal.rg_force_sel_da_pxp_rx_fe_gain_ctrl = 0x1;
		rg_force_da_pxp_rx_fe_gain_ctrl.hal.rg_force_da_pxp_rx_fe_gain_ctrl = leq_gain;
		RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_gain_ctrl,rg_force_da_pxp_rx_fe_gain_ctrl.dat.value);
	

	    for (j = sweep_start; j < sweep_end + 1; j++)
	    {
		    rg_da_pxp_jcpll_sdm_scan.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_da_pxp_jcpll_sdm_scan);
	        rg_da_pxp_jcpll_sdm_scan.hal.rg_force_sel_da_pxp_rx_fe_peaking_ctrl = 0x1;
		    rg_da_pxp_jcpll_sdm_scan.hal.rg_force_da_pxp_rx_fe_peaking_ctrl = leq_peaking;
		    RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_da_pxp_jcpll_sdm_scan,rg_da_pxp_jcpll_sdm_scan.dat.value);
			
		    if(dbg_print)
	//#if Pon_Eye_Scan_Result
	            printk("leq_gain => %x , leq_peaking => %x\n" ,leq_gain , leq_peaking);
	//#endif  
		 
	        XFI_eye_Cal();

	        fom_num = XFI_eye_EO(bit_sel);   
		    fom_num_list[j] = fom_num;

	        if (fom_num > best_fom)
	        {
	            best_fom = fom_num;                        
	            best_peaking = leq_peaking;
	            leq_peaking++;
	        }
	        else
	        {
	            leq_peaking++;
	        } 
		}
	
	if(dbg_print) 
	{
	    printk("================================ EO_Scan Result ================================\n");
		printk("rate = %d, ## 7 for Sync XEPON, 6 for Async XEPON\n",bit_sel);
		printk("best gain = 1 (fixed)\n");
		printk("leq_peaking_list = [ 0,  1,  2,  3,  4,  5,  6,  7 ]\n");
		printk("    fom_num_list = [ ");
		for(j=0;j<7;j++)
			printk("%d, ",fom_num_list[j]);
		printk("%d ] (eye width)\n",fom_num_list[7]);
        printk("XFI_best_leq_gain => %x , XFI_best_leq_peaking => %x\n" ,best_gain , best_peaking);
		printk("================================ EO_Scan Result ================================\n");
    }
				
    //set back the best leq gain&peaking setting
    rg_force_da_pxp_rx_fe_gain_ctrl.hal.rg_force_sel_da_pxp_rx_fe_gain_ctrl = 0x1;
	rg_force_da_pxp_rx_fe_gain_ctrl.hal.rg_force_da_pxp_rx_fe_gain_ctrl = best_gain;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_force_da_pxp_rx_fe_gain_ctrl,rg_force_da_pxp_rx_fe_gain_ctrl.dat.value);

	rg_da_pxp_jcpll_sdm_scan.dat.value = RG_R_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_da_pxp_jcpll_sdm_scan);
    rg_da_pxp_jcpll_sdm_scan.hal.rg_force_sel_da_pxp_rx_fe_peaking_ctrl = 0x1;
	rg_da_pxp_jcpll_sdm_scan.hal.rg_force_da_pxp_rx_fe_peaking_ctrl = best_peaking;
	RG_W_PL(_pxSGMII->pma_base,PMA_BASE_OFFSET,(RgAddr) _rg_da_pxp_jcpll_sdm_scan,rg_da_pxp_jcpll_sdm_scan.dat.value); 
  }
}

static void xSGMII_Dbg_Status(u8 xsgmii){
	rg_type_t(HAL_msg_rx_sts_15) _msg_rx_sts_15;
	rg_type_t(HAL_ro_usxgmii_an_stats_0)ro_usxgmii_an_stats_0;
	RGDATA_t rg;
	switch (xsgmii){
		case USXGMII:			
			rg.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) 0x930);
			printk("sysnc status %x\n",rg.value);
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) 0xc1c,0x01);
			RG_W_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) 0xc1c,0x00);
			ro_usxgmii_an_stats_0.dat.value = RG_R_PL(_pxSGMII->pcs1_base,USXGMII_PCS1_BASE_OFFSET,(RgAddr) 0xc10);
			printk("cur_usxgmii_mode %x,partner ability %x\n",ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode,ro_usxgmii_an_stats_0.hal.ro_cur_usxgmii_mode);
			break;
		case HSGMII:	
		case SGMII:
			rg.value = RG_R_PL(_pxSGMII->pcs2_base,USXGMII_PCS2_BASE_OFFSET,(RgAddr) 0xb04);
			printk("sysnc status %x\n",rg.value);
			_msg_rx_sts_15.dat.value = RG_R_PL(_pxSGMII->multi_sgmii_base,USXGMII_MULTI_SGMII_BASE_OFFSET,(RgAddr) msg_rx_sts_15);
			printk("link status %x,link spd %x\n",_msg_rx_sts_15.hal.ro_lnk_sts_p0,_msg_rx_sts_15.hal.ro_speed_sts_p0);
			break;			
	}
}
static void xSGMII_Dbg_RA_Cnt(void){
	u32 data;
	u8 i;
	RgAddr rg;	
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_11,0x1);
    udelay(10);
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_11,0x0);
	rg =0;
	for (i = 0x34;i<=0x50; i+=4){
		data = RG_R_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr)(rg+ i));
		printk("phy cnt %x %d\n",i,data);
	}
}
static void xSGMII_Dbg_RA_Cnt_Clr(void){
	//rg_type_t(HAL_rg_rate_adapt_ctrl_2) rg_rate_adapt_ctrl_2;
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_2,0xffffffff);	
	udelay(10);
	RG_W_PL(_pxSGMII->ra_base,USXGMII_RA_BASE_OFFSET,(RgAddr) _rg_rate_adapt_ctrl_2,0x0);
}

static void xSGMII_Dbg(u8 xsgmii,u8 mod,u8 an, u8 rate){
	switch(an){
		case 0:
			RX_CDR_RST();
			break;
		case 1:
			RX_PR_CAL_SEQ(xsgmii);
			break;
		case 2:
			RX_BringUp(xsgmii);
			udelay(100);//10.0.9	
			RX_PR_CAL_SEQ(xsgmii);	
			RSTB(eM1,B_ON);
			udelay(100);//10.0.9	
			RSTB(eM1,B_OFF);	
			udelay(100);
			break;
		case 3:
			xSGMII_Dbg_Status(xsgmii);
			xSGMII_Dbg_RA_Cnt();
			xSGMII_Dbg_RA_Cnt_Clr();			
			break;
		case 4:
			printk("RX_SigDet_Flag = %x\n",RX_SigDet_Flag());
			break;
		case 5:
#ifdef Enable_IRQ		
			printk("RX_SigDet_Flag = %x\n",RX_SigDet_Flag_D());
#endif			
			break;
		case 6:
			//TMR_div = rate;
			//printk("TMR_div %x\n",TMR_div);
			break;
		case 7:
			AN_ReStart(xsgmii);
			break;
		case 8:
			USXGMII_AN_Sta();			
			break;
		case 9:
			EO_Scan(rate,0,7);			
			break;
		case 10:
			tFE_Gain = rate;			
			break;
			
		case 255://no use
			xSGMII_Solution(0,xsgmii,mod,an,rate);//10.0.12
			break;
	}
}
	
u32 XPON_xSGMII_API(u8 xsgmii,u8 mod,u8 an, u8 rate){	
	u32 data = 0x00;
	if(MAC_Polling_Print_EN) printk("XPON_xSGMII_API xsgmii=%x,mod = %x,an=%x,rate=%x\n",xsgmii,mod,an,rate);
	
	if (mod < 2)
	{
		xsgmii_chg(xsgmii);
		CMN_EN(1);
		PLL_BringUp(xsgmii);
		Phya_BringUp(xsgmii);
		xSGMII_Solution(0,xsgmii,mod,an,rate);//10.0.12

		xsgmii_init(xsgmii);
			#ifdef Enable_IRQ
		printk("Int init\n");
		xsgmii_interrupt_init(xsgmii);
			#endif
	}	
	
	switch(mod){
		case 0: 
			if(dbg_print) printk("force mode\n");
			xSGMII_AN_API(xsgmii,an);
			xSGMII_Rate_API(xsgmii,rate);
			if(xsgmii == USXGMII)usxgmii_pcs_an_ctrl7(an,rate); //update
			if(an)AN_ReStart(xsgmii);
			xSGMII_Solution(1,xsgmii,mod,an,rate);//10.0.12
			break;
		case 1:
			if(dbg_print) printk("AN mode\n");
			xSGMII_AN_API(xsgmii,1);			
			xSGMII_AN_AutoSetting(xsgmii,0,1,rate); // MAC mode
			break;
		case 2:
			data = xSGMII_Link_STS(xsgmii);
			if(MAC_Polling_Print_EN) printk("xsgmii link sts %x\n",data);
			break;
		case 3 ://update
			if(MAC_Polling_Print_EN) printk("usxgmii status\n");
			MAC_Polling_skip_rg_dump = 1;
			data = usxgmii_link_sts();
			switch (an){
				case 0:
					break;
				case 1:
					data = (data >> 28) & 0x07 ; //c mode
					break;	
				case 2:
					data &= 0x00ffff ; //p ability
					break;
				default :
					data = 0xffffffff;
					break;					
			}	
			MAC_Polling_skip_rg_dump = 0;
			if(MAC_Polling_Print_EN)printk("sgmii_link_sts cur mod %x,value %x\n",an,data);
			break;
		case 4 ://update
			if(MAC_Polling_Print_EN) printk("sgmii status\n");
			MAC_Polling_skip_rg_dump = 1;
			switch (an){
				case 0:
					data = sgmii_link_sts((RgAddr)msg_rx_sts_15);
					break;
				case 1:
					data = sgmii_link_sts((RgAddr)msg_rx_sts_16);
					break;	
				case 2:
					data = sgmii_link_sts((RgAddr)msg_rx_sts_17);
					break;
				case 3:
					data = sgmii_link_sts((RgAddr)msg_rx_sts_18);
					break;
				case 4:
					data = sgmii_link_sts((RgAddr)msg_rx_sts_19);
					break;
				default :
					data = 0xffffffff;
					break;					
			}
			MAC_Polling_skip_rg_dump = 0;
			if(MAC_Polling_Print_EN)printk("sgmii_link_sts sel %x,value %x\n",an,data);
			break;
		case 5: //update
			xSGMII_PD(an);
			break;
		case 6: //update
			RSTB(an,rate);
			break;
		case 7:
			if(rate) aux.value |= 1<<an;
			else aux.value &= ~(1<<an);
			if(dbg_print) printk("xpon aux = %x\n",aux.value);
			break;
		case 8:
			xsgmii_force_data(xsgmii,mod,an,rate);
			break;
		case 9:			
			TX_FIR(0,an,rate,0);
			break;
		case 10:	
			TX_FIR(1,an,rate,0);
			break;
		case 11:	
			TX_FIR_Load_Para(xsgmii,an,rate,XSGMII_TX_FIR_Load_Para_EN);
			break;		
		case 12:	
			RX_FEGainPeaking(1,an,rate);
			break;
		case 13:	
			PAD_PN_Swap(an, rate);
			break;
		case 255:
			if(dbg_print) printk("xSGMII_Dbg %x,%x,%x,%x\n",xsgmii, mod, an, rate);
			xSGMII_Dbg( xsgmii, mod, an, rate);
			break;
		default:
			printk("xsgmii mod error!!!\n");
			data = 0xff;
			break;
	}
	return data;
}
EXPORT_SYMBOL(XPON_xSGMII_API);


/* APIs */
static void xsgmii_drv_init(void)
{
	printk("xsgmii_init: start\n");
	//xsgmii_int_init();
	printk("xpon_usxgmii_fpga\n");
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
		if(dbg_print) printk("xfi_pcs_int_sta_3 %x\n",xfi_pcs_int_sta_3.dat.value);
	}
	return sts;
}	
#endif

#ifdef RESOUCE_FROM_COMMON_MODULE
static void xsgmii_get_base(xsgmii_base * _pxSGMII_Tmp){
	struct en7581_serdes_common_phy* common_phy = Get_Struct();
	printk("xsgmii_get_base \n");
	_pxSGMII = _pxSGMII_Tmp;
	_pxSGMII->irq = common_phy->irq;
	
	_pxSGMII->phya_base = Get_Base(0x1fa8a000);
	_pxSGMII->pma_base = Get_Base(0x1fa8b000);
	_pxSGMII->multi_sgmii_base = Get_Base(0x1fa84000);

	if (_pxSGMII->phya_base == 0) _pxSGMII->phya_base = NULL;
	if (_pxSGMII->pma_base == 0) _pxSGMII->pma_base = NULL;
	if (_pxSGMII->multi_sgmii_base == 0) _pxSGMII->multi_sgmii_base = NULL;	
}
#endif

static int xsgmii_drv_probe(struct platform_device *pdev)
{
    const char *int_name;
    struct resource *res = NULL;
    int ret,int_id,region = 0;
	xsgmii_base * _pxSGMII_Tmp = NULL;

	printk("XPON xsgmii_drv_probe\n");
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No xpon_usxgmii DT node found");
        return -EINVAL;
    }

	//int_id
	ret = of_property_read_u32(pdev->dev.of_node, "int_id", &int_id);
	if(ret<0) printk("sgmii get propert error!\n");
	else printk("sgmii:int_id: %d\n",int_id);

    _pxSGMII_Tmp = devm_kzalloc(&pdev->dev, sizeof(xsgmii_base), GFP_KERNEL);
    if (!_pxSGMII_Tmp)
        return -ENOMEM;

    platform_set_drvdata(pdev, _pxSGMII_Tmp);

	xsgmii_get_base(_pxSGMII_Tmp);

	/* get pcs1 base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
    _pxSGMII->pcs1_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(_pxSGMII->pcs1_base)){
	    printk("pcs1_base ng\n");	
        return PTR_ERR(_pxSGMII->pcs1_base);
    }

	/* get pcs2 base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
   	_pxSGMII->pcs2_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(_pxSGMII->pcs2_base)){
	   printk("pcs2_base ng\n");	
	   return PTR_ERR(_pxSGMII->pcs2_base);
   	}
	
	/* get AN base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
   	_pxSGMII->an_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(_pxSGMII->an_base))
	   return PTR_ERR(_pxSGMII->an_base);

	/* get RA base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
   	_pxSGMII->ra_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(_pxSGMII->ra_base))
	   return PTR_ERR(_pxSGMII->ra_base);

	/* get phya base address */
#ifndef RESOUCE_FROM_COMMON_MODULE
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
	_pxSGMII->phya_base = devm_ioremap_resource(&pdev->dev, res);
#endif
	if (IS_ERR(_pxSGMII->phya_base))
		return PTR_ERR(_pxSGMII->phya_base);

	/* Multi SGMII */
#ifndef RESOUCE_FROM_COMMON_MODULE	
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
	_pxSGMII->multi_sgmii_base = devm_ioremap_resource(&pdev->dev, res);
#endif	
	if (IS_ERR(_pxSGMII->multi_sgmii_base))
		return PTR_ERR(_pxSGMII->multi_sgmii_base);

	/* PMA */
#ifndef RESOUCE_FROM_COMMON_MODULE
	res = platform_get_resource(pdev, IORESOURCE_MEM, region++);
	_pxSGMII->pma_base = devm_ioremap_resource(&pdev->dev, res);
#endif
	if (IS_ERR(_pxSGMII->pma_base))
		return PTR_ERR(_pxSGMII->pma_base);

	_pxSGMII->dev = &pdev->dev;
	
	//int_name
	ret = of_property_read_string(pdev->dev.of_node, "int_name", &int_name);
	if(ret < 0)	printk("sgmii get propert error!");
	else printk("sgmii:int_name: %s\n",int_name);

#ifdef Enable_IRQ
	/* get irq num */
	#ifndef RESOUCE_FROM_COMMON_MODULE
	_pxSGMII->irq = platform_get_irq(pdev, 0);
	if(_pxSGMII->irq <= 0) {
		printk("\n get xpon_xsgmii irq number failed\n");
		return _pxSGMII->irq;
	}else printk("irq = %x\n",_pxSGMII->irq);
	#endif

	#if 0//move to API
	/* request irq */
	ret = request_irq(_pxSGMII->irq, xsgmii_interrupt, 0, int_name, &pdev->dev);
	if(ret) {
		printk("\n request_irq() (irq number: %d) failed (ret: %d)\n", _pxSGMII->irq, ret);
		return (ret);
	}else printk("\n request_irq() (irq number: %d) OK (ret: %d)\n", _pxSGMII->irq, ret);
	#endif
#endif

	#if 1
	printk("XOPN_xsgmii region= %x\n", region);
	printk("XOPN_xsgmii->pcs1_base= %pS\n", _pxSGMII->pcs1_base);
	printk("XOPN_xsgmii->pcs2_base= %pS\n", _pxSGMII->pcs2_base);
	printk("XOPN_xsgmii->an_base=   %pS\n", _pxSGMII->an_base);
	printk("XOPN_xsgmii->ra_base=   %pS\n", _pxSGMII->ra_base);
	printk("XOPN_xsgmii->multi_sgmii_base=   %pS\n", _pxSGMII->multi_sgmii_base);
	printk("XOPN_xsgmii->phya_base = %pS\n", _pxSGMII->phya_base);
	printk("XOPN_xsgmii->pma_base = %pS\n", _pxSGMII->pma_base);
	#endif

	if(_pxSGMII == NULL)printk("XPON xsgmii drv probe ng\n");
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

