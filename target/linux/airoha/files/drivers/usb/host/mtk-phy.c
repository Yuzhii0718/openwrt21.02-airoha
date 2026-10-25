#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/tc3162/tc3162.h>
#define U3_PHY_LIB
#include "mtk-phy.h"
#ifdef CONFIG_C60802_SUPPORT
#include "mtk-phy-c60802.h"
#endif
#ifdef CONFIG_D60802_SUPPORT
#include "mtk-phy-d60802.h"
#endif
#ifdef CONFIG_PROJECT_7662
#include "mtk-phy-7662.h"
#endif
#ifdef CONFIG_PROJECT_5399
#include "mtk-phy-5399.h"
#endif
#ifdef CONFIG_PROJECT_7512
#include "mtk-phy-7512.h"
#endif
#ifdef CONFIG_PROJECT_7628
#include "mtk-phy-7628.h"
#endif
#ifdef CONFIG_C60802_SUPPORT
static const struct u3phy_operator c60802_operators = {
	.init = phy_init_c60802,
	.change_pipe_phase = phy_change_pipe_phase_c60802,
	.eyescan_init = eyescan_init_c60802,
	.eyescan = phy_eyescan_c60802,
	.u2_save_current_entry = u2_save_cur_en_c60802,
	.u2_save_current_recovery = u2_save_cur_re_c60802,
	.u2_slew_rate_calibration = u2_slew_rate_calibration_c60802,
};
#endif
#ifdef CONFIG_D60802_SUPPORT
static const struct u3phy_operator d60802_operators = {
	.init = phy_init_d60802,
	.change_pipe_phase = phy_change_pipe_phase_d60802,
	.eyescan_init = eyescan_init_d60802,
	.eyescan = phy_eyescan_d60802,
	//.u2_save_current_entry = u2_save_cur_en_d60802,
	//.u2_save_current_recovery = u2_save_cur_re_d60802,	
	.u2_slew_rate_calibration = u2_slew_rate_calibration_d60802,
};
#endif
#ifdef CONFIG_PROJECT_PHY
static struct u3phy_operator project_operators = {
	.init = phy_init,
	.change_pipe_phase = phy_change_pipe_phase,
	.eyescan_init = eyescan_init,
	.eyescan = phy_eyescan,
	.u2_slew_rate_calibration = u2_slew_rate_calibration,
};
#endif

void static setup_25M_PLL(void)
{
    	
	U3PhyWriteReg8(0xbfa80c1c, 0x18);
	U3PhyWriteReg8(0xbfa80c1d, 0x18);
	U3PhyWriteReg8(0xbfa80c1f, 0x18);
	U3PhyWriteReg32(0xbfa80c24, 0x18000000);
	U3PhyWriteReg32(0xbfa80c28, 0x18000000);
	U3PhyWriteReg32(0xbfa80c30, 0x18000000);
	U3PhyWriteReg32(0xbfa80c38, 0x004a004a);
	U3PhyWriteReg8(0xbfa80c3e, 0x4a);
	U3PhyWriteReg8(0xbfa80c3f, 0x0);
	U3PhyWriteReg8(0xbfa80c42, 0x48);
	U3PhyWriteReg8(0xbfa80c43, 0x0);
	U3PhyWriteReg8(0xbfa80c44, 0x48);
	U3PhyWriteReg8(0xbfa80c45, 0x0);
	U3PhyWriteReg8(0xbfa80c48, 0x48);
	U3PhyWriteReg8(0xbfa80c49, 0x0);

	U3PhyWriteReg8(0xbfa80b24, 0x90);
	U3PhyWriteReg8(0xbfa80b25, 0x1);
	U3PhyWriteReg32(0xbfa80b10, 0x1c000000);
	U3PhyWriteReg8(0xbfa80b0b, 0xe);
	return;
	
}
PHY_INT32 u3phy_config_751221(void)
{
	if(  (isEN751221 && (readl(0xbfb0008c)&0x01))  || isEN7526c )//Biker_0906_7522_always use 25M input clk
	{
		setup_25M_PLL(); //Biker_20160516
	}


	// 7512
	//enable port 0: SSUSB -isEN7526c (7522), 7526D, 7526G, 7513, 7513G, 7521G, 7586
	//enable port 1: USB2    -7526F, 7512,           7526D, 7526G, 7513, 7513G, 7521G, 7586

	//7522 (7526c)
	//enable port 0: 7526F
	//7521S, 7521F NO USB

	if(isEN7526c){		//Biker_20160906, Add setting for 7522 
	
		if(isEN7526F ||(isEN7521F&&EFUSE_IS_DDR3)) //Biker_20170807
		{	
			writel(0xC0240008, 0xBFA8081C); /* enable port0 */
			printk(KERN_ERR "USB 2 PHY config, enable port0");
			return 1;
		}else		
			return 0; //No port enabled, following driver will not run
		
		
		
	}// 7512 case
	else if(isEN7526D || isEN7526G || isEN7513 || isEN7513G || isEN7521G || isEN7586){
		writel(0xC0240008, 0xBFA8081C); /* enable port0 */
		writel(0xC0240000, 0xBFA8101C); /* enable port1 */
		printk(KERN_ERR "751221 USB PHY config, enable port0 port1");

		//Patch TxDetRx Timing for 7512 E1, from DR 20160421, Biker_20160516
		U3PhyWriteReg32(0xbfa80a28,  ((U3PhyReadReg32(0xbfa80a28) &(~(0x1ff<<9)) )|(0x10<<9) ));//rg_ssusb_rxdet_stb2_set[8:0]
		U3PhyWriteReg32(0xbfa80a2c,  ((U3PhyReadReg32(0xbfa80a2c) &(~0x1ff) )|0x10 ));//rg_ssusb_rxdet_stb2_set_p3[8:0]

	//Patch LFPS Filter Threshold for E1, from DR 20160421, Biker_20160516
		U3PhyWriteReg32 (0xbfa8090c,((U3PhyReadReg32(0xbfa8090c) &(~(0x3f<<16)) )|(0x34<<16) ));//rg_ssusb_fwake_th[5:0]
		
		return 1;
		
	}else if(isEN7526F || isEN7512){
		writel(0xC0241580, 0xBFA8081C);//disable port 0
		writel(0xC0240000, 0xBFA8101C);//enable port 1
		printk(KERN_ERR "7512/7526F USB PHY config, enable port1");
		return 1;
		
	}else 
		return 0;
}

PHY_INT32 u3phy_config_751627(void)
{
	if(  (readl(0xbfb0008c)&0x40000) == 0) 
	{
		setup_25M_PLL(); 
		printk(KERN_ERR "USB PLL 25MHz setting\n");
	}
	
	writel(0xC0240008, 0xBFA8081C); /* enable port0 */
	writel(0xC0240000, 0xBFA8101C); /* enable port1 */
	printk(KERN_ERR "7516/7528 USB PHY config, enable port0 port1\n");		
	
	return 1;
}

PHY_INT32 u3phy_config_fpga(void){

	/* TODO */
	return 1;
}

PHY_INT32 U2_Slew_Rate_Calibration(void){
	PHY_INT32 i=0;
	PHY_INT32 j=0;
	PHY_INT32 fgRet = 0;
	PHY_INT32 u4FmOut = 0;	
	PHY_INT32 u4Tmp = 0;
	PHY_INT32 U2_PHYA_CR0[U2_port_num]={0xbfa80810, 0xbfa81010};

	for(j=0;j<U2_port_num;j++)
	{
		printk(KERN_ERR "port %d u2_slew_rate_calibration\n",j);
		// => RG_USB20_HSTX_SRCAL_EN = 1
		// enable HS TX SR calibration
		U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (U3PhyReadReg32((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCAL_EN)|((0x1)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		DRV_MSLEEP(1);	
		//printk("%x\n",U2_PHYA_CR0[j]);
		//printk("%x\n",U3PhyReadReg32((void *)(U2_PHYA_CR0[j])));
		// => RG_FRCK_EN = 1    
		// Enable free run clock
		U3PhyWriteReg32(0xbfa80110, (U3PhyReadReg32(0xbfa80110)&(~RG_FRCK_EN)|((0x1)<<RG_FRCK_EN_OFST)));
		//printk("%x\n",U3PhyReadReg32(0xbfa80110));
		// => RG_MONCLK_SEL = 0x0/0x1 for port0/port1
		// Setting MONCLK_SEL
		U3PhyWriteReg32(0xbfa80100, (U3PhyReadReg32(0xbfa80100)&(~RG_MONCLK_SEL)|((j)<<RG_MONCLK_SEL_OFST)));
		// => RG_CYCLECNT = 0x400
		// Setting cyclecnt = 0x400
		U3PhyWriteReg32(0xbfa80100, (U3PhyReadReg32(0xbfa80100)&(~RG_CYCLECNT)|((0x400)<<RG_CYCLECNT_OFST)));
		// => RG_FREQDET_EN = 1
		// Enable frequency meter
		U3PhyWriteReg32(0xbfa80100, (U3PhyReadReg32(0xbfa80100)&(~RG_FREQDET_EN)|((0x1)<<RG_FREQDET_EN_OFST)));
		//printk("%x\n",U3PhyReadReg32(0xbfa80100));
		// wait for FM detection done, set 10ms timeout
		for(i=0; i<10; i++){
			u4FmOut = U3PhyReadReg32(0xbfa8010c);
			// check if FM detection done 
			if (u4FmOut != 0)
			{
				// => u4FmOut = USB_FM_OUT
				// read FM_OUT
				printk(KERN_ERR "FM_OUT value = %d(0x%08X)\n", u4FmOut, u4FmOut);
				fgRet = 0;
				//printk(KERN_ERR "FM detection done! loop = %d\n", i);
				break;
			}

			fgRet = 1;
			DRV_MSLEEP(1);
		}
		// => RG_FREQDET_EN = 0
		// disable frequency meter
		U3PhyWriteReg32(0xbfa80100, (U3PhyReadReg32(0xbfa80100)&(~RG_FREQDET_EN)|((0x0)<<RG_FREQDET_EN_OFST)));

		// => RG_FRCK_EN = 0
		// disable free run clock
		U3PhyWriteReg32(0xbfa80110, (U3PhyReadReg32(0xbfa80110)&(~RG_FRCK_EN)|((0x0)<<RG_FRCK_EN_OFST)));

		// => RG_USB20_HSTX_SRCAL_EN = 0
		// disable HS TX SR calibration
		U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (U3PhyReadReg32((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCAL_EN)|((0x0)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		DRV_MSLEEP(1);

		if(u4FmOut == 0){
			U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (U3PhyReadReg32((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCTRL)|((0x4)<<RG_USB20_HSTX_SRCTRL_OFST)));
			fgRet = 1;
		}
		else{
			// set reg = (1024/FM_OUT) * REF_CK * U2_SR_COEF (round to the nearest digits)
			u4Tmp = (((1024 * REF_CK * U2_SR_COEF) / u4FmOut) + 500) / 1000; 
			printk(KERN_ERR "SR calibration value = %d\n", (PHY_UINT8)u4Tmp);
			U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (U3PhyReadReg32((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCTRL)|((u4Tmp&0x7)<<RG_USB20_HSTX_SRCTRL_OFST)));
		}
	}
	return fgRet;
}

int ecnt_u3h_phy_init(void)
{
	int ret = 0;
	
	if(isFPGA){
		ret = u3phy_config_fpga();
	}else if(isEN751221){
		ret = u3phy_config_751221();		
		printk(KERN_ERR "USB driver version: 751221.kernel3.4.20190524\n");
		U2_Slew_Rate_Calibration();
	}else if(isEN751627){
		ret = u3phy_config_751627();
		printk(KERN_ERR "USB driver version: 751627/7528.kernel3.1.20170822\n");	
		U2_Slew_Rate_Calibration();
	}else{
		printk(KERN_ERR "**Unknown chip ID for USB driver**\n");
	}
	
	
	return ret;
}

PHY_INT32 u3phy_init(){
#ifndef CONFIG_PROJECT_PHY
	PHY_INT32 u3phy_version;
#endif
	
	if (u3phy != NULL)
		return PHY_TRUE;

	u3phy = kmalloc(sizeof(struct u3phy_info), GFP_NOIO);
	if (u3phy == NULL)
		return PHY_FALSE;

#if defined (CONFIG_USB_EN7512_XHCI_HCD)
	u3phy_p1 = kmalloc(sizeof(struct u3phy_info), GFP_NOIO);
	if (u3phy_p1 == NULL)
		return PHY_FALSE;
#endif
#ifdef CONFIG_U3_PHY_GPIO_SUPPORT
	u3phy->phyd_version_addr = 0x2000e4;
#if defined (CONFIG_USB_EN7512_XHCI_HCD)
	u3phy_p1->phyd_version_addr = 0x2000e4;
#endif
#else
#if defined (CONFIG_RALINK_MT7628)
	u3phy->phyd_version_addr = U2_PHY_BASE + 0xf0;
	printk("******MT7628 mtk phy\n");
#else
	u3phy->phyd_version_addr = U3_PHYD_B2_BASE + 0xe4;
#if defined (CONFIG_USB_EN7512_XHCI_HCD)
	u3phy_p1->phyd_version_addr = U3_PHYD_B2_BASE_P1 + 0xe4;
#endif
#endif
#endif

#ifdef CONFIG_PROJECT_PHY
	printk("*****run project phy.\n");
	u3phy->u2phy_regs = (struct u2phy_reg *)U2_PHY_BASE;
#if !defined (CONFIG_RALINK_MT7628)
	u3phy->u3phyd_regs = (struct u3phyd_reg *)U3_PHYD_BASE;
	u3phy->u3phyd_bank2_regs = (struct u3phyd_bank2_reg *)U3_PHYD_B2_BASE;
	u3phy->u3phya_regs = (struct u3phya_reg *)U3_PHYA_BASE;
	u3phy->u3phya_da_regs = (struct u3phya_da_reg *)U3_PHYA_DA_BASE;
	u3phy->sifslv_chip_regs = (struct sifslv_chip_reg *)SIFSLV_CHIP_BASE;		
#endif
	u3phy->sifslv_fm_regs = (struct sifslv_fm_feg *)SIFSLV_FM_FEG_BASE;	
	u3phy_ops = &project_operators;

#if defined (CONFIG_USB_EN7512_XHCI_HCD)
	u3phy_p1->u2phy_regs = (struct u2phy_reg *)U2_PHY_BASE_P1;
	u3phy_p1->u3phyd_regs = (struct u3phyd_reg *)U3_PHYD_BASE_P1;
	u3phy_p1->u3phyd_bank2_regs = (struct u3phyd_bank2_reg *)U3_PHYD_B2_BASE_P1;
	u3phy_p1->u3phya_regs = (struct u3phya_reg *)U3_PHYA_BASE_P1;
	u3phy_p1->u3phya_da_regs = (struct u3phya_da_reg *)U3_PHYA_DA_BASE_P1;
	u3phy_p1->sifslv_chip_regs = (struct sifslv_chip_reg *)SIFSLV_CHIP_BASE;
	u3phy_p1->sifslv_fm_regs = (struct sifslv_fm_feg *)SIFSLV_FM_FEG_BASE;
#endif
#else
	
	//parse phy version
	u3phy_version = U3PhyReadReg32(u3phy->phyd_version_addr);
	printk(KERN_ERR "phy version: %x\n", u3phy_version);
	u3phy->phy_version = u3phy_version;

	if(u3phy_version == 0xc60802a){
	#ifdef CONFIG_C60802_SUPPORT	
	#ifdef CONFIG_U3_PHY_GPIO_SUPPORT
		u3phy->u2phy_regs_c = 0x0;
		u3phy->u3phyd_regs_c = 0x100000;
		u3phy->u3phyd_bank2_regs_c = 0x200000;
		u3phy->u3phya_regs_c = 0x300000;
		u3phy->u3phya_da_regs_c = 0x400000;
		u3phy->sifslv_chip_regs_c = 0x500000;
		u3phy->sifslv_fm_regs_c = 0xf00000;
	#else
		u3phy->u2phy_regs_c = U2_PHY_BASE;
		u3phy->u3phyd_regs_c = U3_PHYD_BASE;
		u3phy->u3phyd_bank2_regs_c = U3_PHYD_B2_BASE;
		u3phy->u3phya_regs_c = U3_PHYA_BASE;
		u3phy->u3phya_da_regs_c = U3_PHYA_DA_BASE;
		u3phy->sifslv_chip_regs_c = SIFSLV_CHIP_BASE;		
		u3phy->sifslv_fm_regs_c = SIFSLV_FM_FEG_BASE;		
	#endif
		u3phy_ops = &c60802_operators;
	#endif
	}
	else if(u3phy_version == 0xd60802a){
	#ifdef CONFIG_D60802_SUPPORT
	#ifdef CONFIG_U3_PHY_GPIO_SUPPORT
		u3phy->u2phy_regs_d = 0x0;
		u3phy->u3phyd_regs_d = 0x100000;
		u3phy->u3phyd_bank2_regs_d = 0x200000;
		u3phy->u3phya_regs_d = 0x300000;
		u3phy->u3phya_da_regs_d = 0x400000;
		u3phy->sifslv_chip_regs_d = 0x500000;
		u3phy->sifslv_fm_regs_d = 0xf00000;		
	#else
		u3phy->u2phy_regs_d = U2_PHY_BASE;
		u3phy->u3phyd_regs_d = U3_PHYD_BASE;
		u3phy->u3phyd_bank2_regs_d = U3_PHYD_B2_BASE;
		u3phy->u3phya_regs_d = U3_PHYA_BASE;
		u3phy->u3phya_da_regs_d = U3_PHYA_DA_BASE;
		u3phy->sifslv_chip_regs_d = SIFSLV_CHIP_BASE;		
		u3phy->sifslv_fm_regs_d = SIFSLV_FM_FEG_BASE;	
	#endif
		u3phy_ops = &d60802_operators;
	#endif
	}
	else if(u3phy_version == 0xe60802a){
	#ifdef CONFIG_E60802_SUPPORT
	#endif
	}
	else{
		printk(KERN_ERR "No match phy version\n");
		return PHY_FALSE;
	}
	
#endif

	return PHY_TRUE;
}

PHY_INT32 U3PhyWriteField8(PHY_INT32 addr, PHY_INT32 offset, PHY_INT32 mask, PHY_INT32 value){
	PHY_INT8 cur_value;
	PHY_INT8 new_value;

	cur_value = U3PhyReadReg8(addr);
	new_value = (cur_value & (~mask)) | (value << offset);
	//udelay(i2cdelayus);
	U3PhyWriteReg8(addr, new_value);
	return PHY_TRUE;
}

PHY_INT32 U3PhyWriteField32(PHY_INT32 addr, PHY_INT32 offset, PHY_INT32 mask, PHY_INT32 value){
	PHY_INT32 cur_value;
	PHY_INT32 new_value;

	cur_value = U3PhyReadReg32(addr);
	new_value = (cur_value & (~mask)) | ((value << offset) & mask);
	U3PhyWriteReg32(addr, new_value);
	//DRV_MDELAY(100);

	return PHY_TRUE;
}

PHY_INT32 U3PhyReadField8(PHY_INT32 addr,PHY_INT32 offset,PHY_INT32 mask){
	
	return ((U3PhyReadReg8(addr) & mask) >> offset);
}

PHY_INT32 U3PhyReadField32(PHY_INT32 addr, PHY_INT32 offset, PHY_INT32 mask){

	return ((U3PhyReadReg32(addr) & mask) >> offset);
}

