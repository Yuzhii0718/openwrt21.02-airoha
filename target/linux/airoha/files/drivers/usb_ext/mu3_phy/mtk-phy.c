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

#ifdef TCSUPPORT_CPU_ARMV8
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
//#include <linux/delay.h>
#include <asm/io.h>
#include <linux/device.h>
#endif

#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/tc3162/tc3162.h>
#include "mtk-phy.h"
#include <linux/delay.h>

#if defined(TCSUPPORT_CPU_EN7580)
#include <boot/packageInfo.h>
#endif

//#define USB_DEBUG 1
#define writelmsk(addr, data, msk) \
                { writel(addr, ((readl(addr) & ~(msk)) | ((data) & (msk)))); \
        }

static int u2_port_num = 2;
static int init_done_flag = 0;

#ifdef TCSUPPORT_CPU_ARMV8


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
/*
struct ecnt_usb_phy {
	struct device *dev;
	void __iomem *uphy_base;
};
*/
struct ecnt_usb {
	struct device *dev;
	void __iomem *uphy_base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
//struct ecnt_usb_phy *usb_phy = NULL;
struct ecnt_usb *ecnt_usb = NULL;


static const struct of_device_id ecnt_usb_phy_of_id[] = {
    { .compatible = "econet,ecnt-usb_phy"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_usb_phy_of_id);
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/*****************************************************************
 ****  Reg   a c c e s s ********************************
 ******************************************************************/
// u32 get_efuse_data(u32 addr)
//{
//	return 0;
//}

u32 get_uphy_reg(u32 reg)
{
	return readl(ecnt_usb->uphy_base + (reg));
}
EXPORT_SYMBOL(get_uphy_reg);
void set_uphy_reg(u32 reg, u32 val)
{
	writel(val, (ecnt_usb->uphy_base + (reg))); 
}
EXPORT_SYMBOL(set_uphy_reg);



/* APIs */
/*
static int usb_phy_drv_probe(struct platform_device *pdev)
{
	struct resource *res = NULL;
	int ret = 0;

	printk("debug 1\n");
	//printk("usb driver version: 7523.0.20200910\n");

	if (!pdev->dev.of_node) {
	        dev_err(&pdev->dev, "No usb_phy DT node found");
	        return -EINVAL;
	    }
	usb_phy = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_usb_phy), GFP_KERNEL);
   	if (!usb_phy)
		return -ENOMEM;
printk("debug 2\n");
	platform_set_drvdata(pdev, usb_phy);
printk("debug 3\n");
    // get uphy base address 
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	usb_phy->uphy_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(usb_phy->uphy_base))
	    return PTR_ERR(usb_phy->uphy_base);	
printk("debug 4\n");
	usb_phy->dev = &pdev->dev;

	return 0;
}
*/
int ECNT_USB_DRV_PROBE(void)
{
	struct resource *res = NULL;
    struct device_node *node=NULL;
    struct platform_device *pdev=NULL;

	int ret = 0;

    node = of_find_node_by_path("/usb_phy@1fad0000");
    if (node==NULL) {
        printk("\nERROR(%s) node==NULL\n", __func__);
        return -1;
    }

    pdev = of_find_device_by_node(node);
    if (pdev==NULL) {
        printk("\nERROR(%s) pdev==NULL\n", __func__);
        return -1;
    }

	ecnt_usb = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_usb), GFP_KERNEL);
	if (!ecnt_usb)
		return -ENOMEM;
	platform_set_drvdata(pdev, ecnt_usb);

	/* Get NP SCU address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ecnt_usb->uphy_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ecnt_usb->uphy_base)) {
        printk("\nERROR(%s) ecnt_usb_base\n", __func__);
		return -1;
    }
    
	
    
	ecnt_usb->dev = &pdev->dev;


	return 0;
}

/* init SCU registers' base address (a.s.a.p.) before any kernel module might access it. 
 * For example, usb_init() calls "isFPGA" which will access NP SCU register. 
 * If SCU base address has not initialized before that, cpu will crash. 
 * usb_init() uses subsys_initcall to init. Although ECNT_SCU_DRV_PROBE also uses
 * the smae subsys_initcall, it's executed before usb_init(), so it's ok 
 * Note: you can check linux-4.4.115/System.map to see which initcall function will be executed first*/
subsys_initcall(ECNT_USB_DRV_PROBE);
/*
static int usb_phy_drv_remove(struct platform_device *pdev)
{
	
	if(usb_phy)  {
		devm_free(usb_phy);
	}
	
	return 0;
}
*/
/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

/*
static struct platform_driver usb_phy_driver = {
    .probe = usb_phy_drv_probe,
    .remove = usb_phy_drv_remove,
    .driver = {
	    .name = "ecnt-usb_phy",
	    .of_match_table = ecnt_usb_phy_of_id
    },
};
builtin_platform_driver(usb_phy_driver);
*/
//MODULE_DESCRIPTION("EcoNet usb phy Driver");

PHY_INT32 u3phy_config_en7523(void){
	
	PHY_INT32 i=0;
	PHY_INT32 tmp=0;

	//20MHz according to DC's Doc
	if(get_xtal_sel() == 0)
	{
		printk("U3 PHY 20MHz setting \n");

		set_uphy_reg(0xb0c, (get_uphy_reg(0xb0c)&(~(0xff<<24))|((0x7d)<<24)));
		set_uphy_reg(0xb10, (get_uphy_reg(0xb10)&(~(0xff<<24))|((0xf9)<<24)));
		set_uphy_reg(0xb14, (get_uphy_reg(0xb14)&(~0xff)|((0x40)<<0)));
		set_uphy_reg(0xb38, (get_uphy_reg(0xb38)&(~0xff)|((0x38)<<0)));
		set_uphy_reg(0xb40, (get_uphy_reg(0xb40)&(~(0xff<<16))|((0x36)<<16)));
	}
	

	
#ifdef USB_DEBUG
	//printk("u3phy_config_en7523 test, reg read test : 1fad3e00 %x\n",get_uphy_reg(0x3e00));
	printk("reg read test : 1fad0c1c %x\n",get_uphy_reg(0x0c1c));
#endif
	
	for(i=0;i<u2_port_num;i++)
	{
	//U2 init
		set_uphy_reg(RG_USB20_BC11_SW_EN_ADDR+0x1000*i, get_uphy_reg(RG_USB20_BC11_SW_EN_ADDR+0x1000*i)&(~RG_USB20_BC11_SW_EN));
		mdelay(1);
	//U2 eye tuning
				
		
	}		
	//U2 Cross point voltage  adjust to 0x5
	set_uphy_reg(0x1310, (get_uphy_reg(0x1310)&(~(0x7<<8))|((0x5)<<8)));

	
#if 0 //reading efuse function move to bootloader.
// Read Efuse then write to tx_impsel, rx_impsel
	//RG_SSUSB_IEXT_INTR_CTRL[5:0]  p0
	if (get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_SSUSB_P0) > 23)
	{
		set_uphy_reg(RG_SSUSB_IEXT_INTR_CTRL_ADDR, get_uphy_reg(RG_SSUSB_IEXT_INTR_CTRL_ADDR)&(~RG_SSUSB_IEXT_INTR_CTRL)|((get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_SSUSB_P0))<<RG_SSUSB_IEXT_INTR_CTRL_OFST));
		mdelay(1);
	}

	//RG_SSUSB_TX_IMP_SEL_SSUSB[4:0]  p0
	if (get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_SSUSB_P0) != 0)
	{
		set_uphy_reg(RG_SSUSB_LN0_TX_IMP_SEL_ADDR, get_uphy_reg(RG_SSUSB_LN0_TX_IMP_SEL_ADDR)&(~RG_SSUSB_LN0_TX_IMP_SEL)|((get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_SSUSB_P0))<<RG_SSUSB_LN0_TX_IMP_SEL_OFST));
		mdelay(1);
	}
	//RG_SSUSB_RX_IMP_SEL_SSUSB[5:0]  p0
	if (get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_SSUSB_P0) != 0)
	{	
		set_uphy_reg(RG_SSUSB_LN0_RX_IMP_SEL_ADDR, get_uphy_reg(RG_SSUSB_LN0_RX_IMP_SEL_ADDR)&(~RG_SSUSB_LN0_RX_IMP_SEL)|((get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_SSUSB_P0))<<RG_SSUSB_LN0_RX_IMP_SEL_OFST));
		mdelay(1);
	}



	//U2
	//RG_USB20_INTR_CTRL[4:0]  p0, plus 3 for bigger eye diagram
	if (get_efuse_data(RG_USB20_INTR_CTRL_P0) != 0)
	{
		set_uphy_reg(RG_USB20_INTR_CAL_ADDR, get_uphy_reg(RG_USB20_INTR_CAL_ADDR)&(~RG_USB20_INTR_CAL)|(((get_efuse_data(RG_USB20_INTR_CTRL_P0))+3)<<RG_USB20_INTR_CAL_OFST));
		mdelay(1);
	}

	//RG_USB20_INTR_CTRL[4:0]  p1
	if (get_efuse_data(RG_USB20_INTR_CTRL_P1) != 0)
	{
		set_uphy_reg(RG_USB20_INTR_CAL_ADDR+0x1000, get_uphy_reg(RG_USB20_INTR_CAL_ADDR+0x1000)&(~RG_USB20_INTR_CAL)|(((get_efuse_data(RG_USB20_INTR_CTRL_P1))+3)<<RG_USB20_INTR_CAL_OFST));
		mdelay(1);
	}

#endif
#ifdef USB_DEBUG
	printk("Read USB impedance setting\n");
	printk("TX_IMP_SEL_SSUSB_P0 0x1fad0910 [28:24]:0x%x \n",  (get_uphy_reg(0x910)>>24) &0x1f);
	printk("RX_IMP_SEL_SSUSB_P0 0x1fad0914 [28:24]:0x%x \n", (get_uphy_reg(0x914)>>24)&0x1f);
	printk("INTR_CTRL_SSUSB_P0 0x1fad0b00 [15:10]:0x%x \n",  (get_uphy_reg(0xb00)>>10)&0x3f);
	printk("USB20_INTR_CTRL_P0 0x1fad0304 [23:19]:0x%x \n",  (get_uphy_reg(0x0304)>>19)&0x1f);
	printk("USB20_INTR_CTRL_P1 0x1fad1304 [23:19]:0x%x \n",  (get_uphy_reg(0x1304)>>19)&0x1f);
#endif
		
	//return 1 will run USB software setting 
	if( isEN7523SU) 
	{	
		//printk("USB ecnt_u3h_phy_init return 0\n");
		return 0;//No port enabled, following driver will not run
	}else
	{
		return 1; 
	}

	
}


PHY_INT32 u2_slew_rate_cal_en7523(void)
{
	PHY_INT32 i=0;
	PHY_INT32 j=0;
	PHY_INT32 fgRet = 0;
	PHY_INT32 u4FmOut = 0;	
	PHY_INT32 u4Tmp = 0;

	for(j=0;j<u2_port_num;j++)
	{
		printk(KERN_ERR "port %d u2_slew_rate_cal_7523\n",j);
		// => RG_USB20_HSTX_SRCAL_EN = 1
		// enable HS TX SR calibration
		set_uphy_reg(RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j, (get_uphy_reg(RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j)&(~RG_USB20_HSTX_SRCAL_EN)|((0x1)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		mdelay(1);	
		#ifdef USB_DEBUG
		printk("RG_USB20_HSTX_SRCAL_EN(%x): %x\n",RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j ,get_uphy_reg(RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j));
		#endif

		// => RG_FRCK_EN = 1    
		// Enable free run clock
		set_uphy_reg(RG_FRCK_EN_ADDR, (get_uphy_reg(RG_FRCK_EN_ADDR)&(~RG_FRCK_EN)|((0x1)<<RG_FRCK_EN_OFST)));
		#ifdef USB_DEBUG
		printk("RG_FRCK_EN(%x): %x\n",RG_FRCK_EN_ADDR ,get_uphy_reg(RG_FRCK_EN_ADDR));
		#endif

		// => RG_MONCLK_SEL = 0x0/0x1 for port0/port1
		// Setting MONCLK_SEL
		set_uphy_reg(RG_MONCLK_SEL_ADDR, (get_uphy_reg(RG_MONCLK_SEL_ADDR)&(~RG_MONCLK_SEL)|((j)<<RG_MONCLK_SEL_OFST)));
		#ifdef USB_DEBUG
		printk("RG_MONCLK_SEL_ADDR(%x): %x\n",RG_MONCLK_SEL_ADDR ,get_uphy_reg(RG_MONCLK_SEL_ADDR));
		#endif

		// => RG_CYCLECNT = 0x400
		// Setting cyclecnt = 0x400
		set_uphy_reg(RG_CYCLECNT_ADDR, (get_uphy_reg(RG_CYCLECNT_ADDR)&(~RG_CYCLECNT)|((0x400)<<RG_CYCLECNT_OFST)));
		#ifdef USB_DEBUG
		printk("RG_CYCLECNT(%x): %x\n",RG_CYCLECNT_ADDR ,get_uphy_reg(RG_CYCLECNT_ADDR));
		#endif

		// => RG_FREQDET_EN = 1
		// Enable frequency meter
		set_uphy_reg(RG_FREQDET_EN_ADDR, (get_uphy_reg(RG_FREQDET_EN_ADDR)&(~RG_FREQDET_EN)|((0x1)<<RG_FREQDET_EN_OFST)));
		#ifdef USB_DEBUG
		printk("RG_FREQDET_EN(%x): %x\n",RG_FREQDET_EN_ADDR ,get_uphy_reg(RG_FREQDET_EN_ADDR));
		#endif
		// wait for FM detection done, set 10ms timeout
		for(i=0; i<10; i++){
			u4FmOut = get_uphy_reg(FM_OUT_ADDR);
			// check if FM detection done 
			if (u4FmOut != 0)
			{
				// => u4FmOut = USB_FM_OUT
				// read FM_OUT
				printk(KERN_ERR "FM_OUT value = %d(0x%08X)\n", u4FmOut, u4FmOut);
				fgRet = 0;
				#ifdef USB_DEBUG
				printk(KERN_ERR "FM detection done! loop = %d\n", i);
				#endif
				break;
			}

			fgRet = 1;
			mdelay(1);
		}
		// => RG_FREQDET_EN = 0
		// disable frequency meter
		set_uphy_reg(RG_FREQDET_EN_ADDR, (get_uphy_reg(RG_FREQDET_EN_ADDR)&(~RG_FREQDET_EN)|((0x0)<<RG_FREQDET_EN_OFST)));
		#ifdef USB_DEBUG
		printk("RG_CYCLECNT(%x): %x\n", RG_FREQDET_EN_ADDR, get_uphy_reg(RG_FREQDET_EN_ADDR));
		#endif
		// => RG_FRCK_EN = 0
		// disable free run clock
		set_uphy_reg(RG_FRCK_EN_ADDR, (get_uphy_reg(RG_FRCK_EN_ADDR)&(~RG_FRCK_EN)|((0x0)<<RG_FRCK_EN_OFST)));
		#ifdef USB_DEBUG
		printk("RG_FRCK_EN_ADDR(%x): %x\n", RG_FRCK_EN_ADDR, get_uphy_reg(RG_FRCK_EN_ADDR));
		#endif

		// => RG_USB20_HSTX_SRCAL_EN = 0
		// disable HS TX SR calibration
		set_uphy_reg(RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j, (get_uphy_reg(RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j)&(~RG_USB20_HSTX_SRCAL_EN)|((0x0)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		mdelay(1);
		#ifdef USB_DEBUG
		printk("RG_USB20_HSTX_SRCAL_EN_ADDR(%x): %x\n", RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j, get_uphy_reg(RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j));
		#endif


		// In case not K success, use measured value
		if(u4FmOut == 0){
			set_uphy_reg(RG_USB20_HSTX_SRCTRL_ADDR+0x1000*j, (get_uphy_reg(RG_USB20_HSTX_SRCTRL_ADDR+0x1000*j)&(~RG_USB20_HSTX_SRCTRL)|(HSTX_SRCTRL_MEASURE<<RG_USB20_HSTX_SRCTRL_OFST)));
			fgRet = 1;
			printk(KERN_ERR "Use default SR calibration value \n");
		}
		else{
			// set reg = (1024/FM_OUT) * REF_CK * U2_SR_COEF (round to the nearest digits)
			u4Tmp = (((1024 * REF_CK * U2_SR_COEF) / u4FmOut) + 500) / 1000; 
			printk(KERN_ERR "SR calibration value = %d\n", (PHY_UINT8)u4Tmp);
			set_uphy_reg(RG_USB20_HSTX_SRCTRL_ADDR+0x1000*j, (get_uphy_reg(RG_USB20_HSTX_SRCTRL_ADDR+0x1000*j)&(~RG_USB20_HSTX_SRCTRL)|((u4Tmp&0x7)<<RG_USB20_HSTX_SRCTRL_OFST)));
		}
	}
	return fgRet;
}



#else

PHY_INT32 u3phy_config_fpga(void){

        /* TODO */
        return 1;
}

PHY_INT32 u3phy_config_en7528(void)
{
//	if(  (readl(0xbfb0008c)&0x40000) == 0) 
//	{
//		setup_25M_PLL(); 
//		printk(KERN_ERR "USB PLL 25MHz setting\n");
//	}

	//reference to 7528 FT flow
	writel(0xC0240000, 0xBFA8031C); /* Disable BC 1.1 port0 */
	writel(0xC0240000, 0xBFA8131C); /* Disable BC 1.1 port1 */
	printk(KERN_ERR "7528 USB PHY config, enable port0 port1\n");		

	//combo phy Rx R FT mean value too high, tune target R -5 ohm
	regWrite32(0xbfa80b2c,  ((regRead32(0xbfa80b2c) &(~(0x3<<12)) )|(0x1<<12) ));
	
	return 1; //OSBNB00092836 Coverity issue, no return value
}

PHY_INT32 u3phy_config_en7580(void){
	
	PHY_INT32 i=0;
	PHY_INT32 tmp=0;
	
	for(i=0;i<u2_port_num;i++)
	{
	//U2 init
		U3PhyWriteReg32(RG_USB20_BC11_SW_EN_ADDR+0x10000*i, U3PhyReadReg32(RG_USB20_BC11_SW_EN_ADDR+0x10000*i)&(~RG_USB20_BC11_SW_EN));
		mdelay(1);
	//U2 eye tuning
		//RG_USB20_INTR_EN = 1 (default)
		//U3PhyWriteReg32(RG_USB20_INTR_EN_ADDR+0x10000*i, readl(RG_USB20_INTR_EN_ADDR+0x10000*i)&(~RG_USB20_INTR_EN)|((0x1)<<RG_USB20_INTR_EN_OFST));
		//mdelay(1);

		//RG_SSUSB_INTR_EN = 0 , Not apply
		//U3PhyWriteReg32(RG_SSUSB_INTR_EN_ADDR+0x10000*i, readl(RG_SSUSB_INTR_EN_ADDR+0x10000*i)&(~RG_SSUSB_INTR_EN)|((0x0)<<RG_SSUSB_INTR_EN_OFST));
		//mdelay(1);
				
		//RG_USB20_HS_100U_U3_EN = 0 
		U3PhyWriteReg32(RG_USB20_HS_100U_U3_EN_ADDR+0x10000*i, readl(RG_USB20_HS_100U_U3_EN_ADDR+0x10000*i)&(~RG_USB20_HS_100U_U3_EN)|((0x0)<<RG_USB20_HS_100U_U3_EN_OFST));
		mdelay(1);

		//RG_USB20_INTR_CAL[4:0] = 5'b 10111
		U3PhyWriteReg32(RG_USB20_INTR_CAL_ADDR+0x10000*i, readl(RG_USB20_INTR_CAL_ADDR+0x10000*i)&(~RG_USB20_INTR_CAL)|((0x17)<<RG_USB20_INTR_CAL_OFST));
		mdelay(1);
		
	}		

	//deemphasis 3.5 db ratio out SPEC :909.7dppm, tune setting from 17=>18
	U3PhyWriteReg32(RG_SSUSB_IDEM_3p5db_ADDR, readl(RG_SSUSB_IDEM_3p5db_ADDR)&(~(RG_SSUSB_IDEM_3p5db|RG_SSUSB_FORCE_IDEM_3p5db ))|(0x52<<RG_SSUSB_IDEM_3p5db_OFST));
	mdelay(1);
	U3PhyWriteReg32(RG_SSUSB_IDEM_3p5db_ADDR+0x10000, readl(RG_SSUSB_IDEM_3p5db_ADDR+0x10000)&(~(RG_SSUSB_IDEM_3p5db|RG_SSUSB_FORCE_IDEM_3p5db ))|(0x52<<RG_SSUSB_IDEM_3p5db_OFST));
	mdelay(1);


// Read Efuse then write to tx_impsel, rx_impsel
#if defined(TCSUPPORT_CPU_EN7580)
	//RG_SSUSB_IEXT_INTR_CTRL[5:0]  p0
	if (get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_SSUSB_P0) > 23)
	{
		U3PhyWriteReg32(RG_SSUSB_IEXT_INTR_CTRL_ADDR, readl(RG_SSUSB_IEXT_INTR_CTRL_ADDR)&(~RG_SSUSB_IEXT_INTR_CTRL)|((get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_SSUSB_P0))<<RG_SSUSB_IEXT_INTR_CTRL_OFST));
		mdelay(1);
	}

	//RG_SSUSB_TX_IMP_SEL_SSUSB[4:0]  p0
	if (get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_SSUSB_P0) != 0)
	{
		U3PhyWriteReg32(RG_SSUSB_LN0_TX_IMP_SEL_ADDR, readl(RG_SSUSB_LN0_TX_IMP_SEL_ADDR)&(~RG_SSUSB_LN0_TX_IMP_SEL)|((get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_SSUSB_P0))<<RG_SSUSB_LN0_TX_IMP_SEL_OFST));
		mdelay(1);
	}
	//RG_SSUSB_RX_IMP_SEL_SSUSB[5:0]  p0
	if (get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_SSUSB_P0) != 0)
	{	
		U3PhyWriteReg32(RG_SSUSB_LN0_RX_IMP_SEL_ADDR, readl(RG_SSUSB_LN0_RX_IMP_SEL_ADDR)&(~RG_SSUSB_LN0_RX_IMP_SEL)|((get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_SSUSB_P0))<<RG_SSUSB_LN0_RX_IMP_SEL_OFST));
		mdelay(1);
	}

	//RG_SSUSB_IEXT_INTR_CTRL[5:0]  p1
		if (get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_SSUSB_P1) > 23)
	{
		U3PhyWriteReg32(RG_SSUSB_IEXT_INTR_CTRL_ADDR+0x10000, readl(RG_SSUSB_IEXT_INTR_CTRL_ADDR+0x10000)&(~RG_SSUSB_IEXT_INTR_CTRL)|((get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_SSUSB_P1))<<RG_SSUSB_IEXT_INTR_CTRL_OFST));
		mdelay(1);
	}

	//RG_SSUSB_TX_IMP_SEL_SSUSB[4:0]  p1
	if (get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_SSUSB_P1) != 0)
	{
		U3PhyWriteReg32(RG_SSUSB_LN0_TX_IMP_SEL_ADDR+0x10000, readl(RG_SSUSB_LN0_TX_IMP_SEL_ADDR+0x10000)&(~RG_SSUSB_LN0_TX_IMP_SEL)|((get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_SSUSB_P1))<<RG_SSUSB_LN0_TX_IMP_SEL_OFST));
		mdelay(1);
	}

	//RG_SSUSB_RX_IMP_SEL_SSUSB[5:0]  p1
	if (get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_SSUSB_P1) != 0)
	{
		U3PhyWriteReg32(RG_SSUSB_LN0_RX_IMP_SEL_ADDR+0x10000, readl(RG_SSUSB_LN0_RX_IMP_SEL_ADDR+0x10000)&(~RG_SSUSB_LN0_RX_IMP_SEL)|((get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_SSUSB_P1))<<RG_SSUSB_LN0_RX_IMP_SEL_OFST));
		mdelay(1);
	}


	//U2
	//RG_USB20_INTR_CTRL[4:0]  p0, plus 3 for bigger eye diagram
	if (get_efuse_data(RG_USB20_INTR_CTRL_P0) != 0)
	{
		U3PhyWriteReg32(RG_USB20_INTR_CAL_ADDR, readl(RG_USB20_INTR_CAL_ADDR)&(~RG_USB20_INTR_CAL)|(((get_efuse_data(RG_USB20_INTR_CTRL_P0))+3)<<RG_USB20_INTR_CAL_OFST));
		mdelay(1);
	}

	//RG_USB20_INTR_CTRL[4:0]  p1
	if (get_efuse_data(RG_USB20_INTR_CTRL_P1) != 0)
	{
		U3PhyWriteReg32(RG_USB20_INTR_CAL_ADDR+0x10000, readl(RG_USB20_INTR_CAL_ADDR+0x10000)&(~RG_USB20_INTR_CAL)|(((get_efuse_data(RG_USB20_INTR_CTRL_P1))+3)<<RG_USB20_INTR_CAL_OFST));
		mdelay(1);
	}


printk("Read USB impedance setting\n");
printk("TX_IMP_SEL_SSUSB_P0 0xbfa80910 [28:24]:0x%x \n",  (regRead32(0xbfa80910)>>24) &0x1f);
printk("RX_IMP_SEL_SSUSB_P0 0xbfa80914 [28:24]:0x%x \n", (regRead32(0xbfa80914)>>24)&0x1f);
printk("INTR_CTRL_SSUSB_P0 0xbfa80b00 [15:10]:0x%x \n",  (regRead32(0xbfa80b00)>>10)&0x3f);
printk("TX_IMP_SEL_SSUSB_P1 0xbfa90910 [28:24]:0x%x \n",  (regRead32(0xbfa90910)>>24)&0x1f);
printk("RX_IMP_SEL_SSUSB_P1 0xbfa90914 [28:24]:0x%x \n", (regRead32(0xbfa90914)>>24)&0x1f);
printk("INTR_CTRL_SSUSB_P1 0xbfa90b00 [15:10]:0x%x \n", (regRead32(0xbfa90b00)>>10)&0x3f);
printk("USB20_INTR_CTRL_P0 0xbfa80304 [23:19]:0x%x \n",  (regRead32(0xbfa80304)>>19)&0x1f);
printk("USB20_INTR_CTRL_P1 0xbfa90304 [23:19]:0x%x \n",  (regRead32(0xbfa90304)>>19)&0x1f);


#endif

	
        return 1;
}

PHY_INT32 u2_slew_rate_cal_en7528(void){
	PHY_INT32 i=0;
	PHY_INT32 j=0;
	PHY_INT32 fgRet = 0;
	PHY_INT32 u4FmOut = 0;	
	PHY_INT32 u4Tmp = 0;
	PHY_INT32 U2_PHYA_CR0[2]={0xbfa80310, 0xbfa81310};

	for(j=0;j<u2_port_num;j++)
	{
		printk(KERN_ERR "port %d u2_slew_rate_cal_7528\n",j);
		// => RG_USB20_HSTX_SRCAL_EN = 1
		// enable HS TX SR calibration
		U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (readl((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCAL_EN)|((0x1)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		mdelay(1);	
		//printk("%x\n",U2_PHYA_CR0[j]);
		//printk("%x\n",readl((void *)(U2_PHYA_CR0[j])));
		// => RG_FRCK_EN = 1    
		// Enable free run clock
		U3PhyWriteReg32(0xbfa80110, (readl(0xbfa80110)&(~RG_FRCK_EN)|((0x1)<<RG_FRCK_EN_OFST)));
		//printk("%x\n",readl(0xbfa80110));
		// => RG_MONCLK_SEL = 0x0/0x1 for port0/port1
		// Setting MONCLK_SEL
		U3PhyWriteReg32(0xbfa80100, (readl(0xbfa80100)&(~RG_MONCLK_SEL)|((j)<<RG_MONCLK_SEL_OFST)));
		// => RG_CYCLECNT = 0x400
		// Setting cyclecnt = 0x400
		U3PhyWriteReg32(0xbfa80100, (readl(0xbfa80100)&(~RG_CYCLECNT)|((0x400)<<RG_CYCLECNT_OFST)));
		// => RG_FREQDET_EN = 1
		// Enable frequency meter
		U3PhyWriteReg32(0xbfa80100, (readl(0xbfa80100)&(~RG_FREQDET_EN)|((0x1)<<RG_FREQDET_EN_OFST)));
		//printk("%x\n",readl(0xbfa80100));
		// wait for FM detection done, set 10ms timeout
		for(i=0; i<10; i++){
			u4FmOut = readl(0xbfa8010c);
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
			mdelay(1);
		}
		// => RG_FREQDET_EN = 0
		// disable frequency meter
		U3PhyWriteReg32(0xbfa80100, (readl(0xbfa80100)&(~RG_FREQDET_EN)|((0x0)<<RG_FREQDET_EN_OFST)));

		// => RG_FRCK_EN = 0
		// disable free run clock
		U3PhyWriteReg32(0xbfa80110, (readl(0xbfa80110)&(~RG_FRCK_EN)|((0x0)<<RG_FRCK_EN_OFST)));

		// => RG_USB20_HSTX_SRCAL_EN = 0
		// disable HS TX SR calibration
		U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (readl((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCAL_EN)|((0x0)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		mdelay(1);

		if(u4FmOut == 0){
			U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (readl((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCTRL)|((0x4)<<RG_USB20_HSTX_SRCTRL_OFST)));
			fgRet = 1;
		}
		else{
			// set reg = (1024/FM_OUT) * REF_CK * U2_SR_COEF (round to the nearest digits)
			u4Tmp = (((1024 * REF_CK * U2_SR_COEF) / u4FmOut) + 500) / 1000; 
			printk("SR calibration value = %d\n", (PHY_UINT8)u4Tmp);
			U3PhyWriteReg32((void *)(U2_PHYA_CR0[j]), (readl((void *)(U2_PHYA_CR0[j]))&(~RG_USB20_HSTX_SRCTRL)|((u4Tmp&0x7)<<RG_USB20_HSTX_SRCTRL_OFST)));
		}
	}
	return fgRet;
}

PHY_INT32 u2_slew_rate_cal_en7580(void){
	PHY_INT32 i=0;
	PHY_INT32 j=0;
	PHY_INT32 fgRet = 0;
	PHY_INT32 u4FmOut = 0;	
	PHY_INT32 u4Tmp = 0;

	for(j=0;j<u2_port_num;j++)
	{
		printk(KERN_ERR "port %d u2_slew_rate_cal_7580\n",j);
		// => RG_USB20_HSTX_SRCAL_EN = 1
		// enable HS TX SR calibration
		U3PhyWriteReg32(RG_USB20_HSTX_SRCAL_EN_ADDR+0x10000*j, (readl(RG_USB20_HSTX_SRCAL_EN_ADDR+0x10000*j)&(~RG_USB20_HSTX_SRCAL_EN)|((0x1)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		mdelay(1);	
		#ifdef USB_DEBUG
		printk("RG_USB20_HSTX_SRCAL_EN: %x\n",readl(RG_USB20_HSTX_SRCAL_EN_ADDR+0x10000*j));
		#endif

		// => RG_FRCK_EN = 1    
		// Enable free run clock
		U3PhyWriteReg32(RG_FRCK_EN_ADDR+0x10000*j, (readl(RG_FRCK_EN_ADDR+0x10000*j)&(~RG_FRCK_EN)|((0x1)<<RG_FRCK_EN_OFST)));
		#ifdef USB_DEBUG
		printk("RG_FRCK_EN: %x\n",readl(RG_FRCK_EN_ADDR+0x10000*j));
		#endif

		// => RG_MONCLK_SEL = 0x0/0x1 for port0/port1
		// Setting MONCLK_SEL
		//7580 not use RG_MONCLK_SEL to select port.
		//U3PhyWriteReg32(0xbfa80100, (readl(0xbfa80100)&(~RG_MONCLK_SEL)|((j)<<RG_MONCLK_SEL_OFST)));

		// => RG_CYCLECNT = 0x400
		// Setting cyclecnt = 0x400
		U3PhyWriteReg32(RG_CYCLECNT_ADDR+0x10000*j, (readl(RG_CYCLECNT_ADDR+0x10000*j)&(~RG_CYCLECNT)|((0x400)<<RG_CYCLECNT_OFST)));

		// => RG_FREQDET_EN = 1
		// Enable frequency meter
		U3PhyWriteReg32(RG_FREQDET_EN_ADDR+0x10000*j, (readl(RG_FREQDET_EN_ADDR+0x10000*j)&(~RG_FREQDET_EN)|((0x1)<<RG_FREQDET_EN_OFST)));
		#ifdef USB_DEBUG
		printk("RG_FREQDET_EN: %x\n",readl(RG_FREQDET_EN_ADDR+0x10000*j));
		#endif
		// wait for FM detection done, set 10ms timeout
		for(i=0; i<10; i++){
			u4FmOut = readl(FM_OUT_ADDR+0x10000*j);
			// check if FM detection done 
			if (u4FmOut != 0)
			{
				// => u4FmOut = USB_FM_OUT
				// read FM_OUT
				printk(KERN_ERR "FM_OUT value = %d(0x%08X)\n", u4FmOut, u4FmOut);
				fgRet = 0;
				#ifdef USB_debug
				printk(KERN_ERR "FM detection done! loop = %d\n", i);
				#endif
				break;
			}

			fgRet = 1;
			mdelay(1);
		}
		// => RG_FREQDET_EN = 0
		// disable frequency meter
		U3PhyWriteReg32(RG_FREQDET_EN_ADDR+0x10000*j, (readl(RG_FREQDET_EN_ADDR+0x10000*j)&(~RG_FREQDET_EN)|((0x0)<<RG_FREQDET_EN_OFST)));

		// => RG_FRCK_EN = 0
		// disable free run clock
		U3PhyWriteReg32(RG_FRCK_EN_ADDR+0x10000*j, (readl(RG_FRCK_EN_ADDR+0x10000*j)&(~RG_FRCK_EN)|((0x0)<<RG_FRCK_EN_OFST)));

		// => RG_USB20_HSTX_SRCAL_EN = 0
		// disable HS TX SR calibration
		U3PhyWriteReg32(RG_USB20_HSTX_SRCAL_EN_ADDR+0x10000*j, (readl(RG_USB20_HSTX_SRCAL_EN_ADDR+0x10000*j)&(~RG_USB20_HSTX_SRCAL_EN)|((0x0)<<RG_USB20_HSTX_SRCAL_EN_OFST)));
		mdelay(1);

		// In case not K success, use measured value
		if(u4FmOut == 0){
			U3PhyWriteReg32(RG_USB20_HSTX_SRCTRL_ADDR+0x10000*j, (readl(RG_USB20_HSTX_SRCTRL_ADDR+0x10000*j)&(~RG_USB20_HSTX_SRCTRL)|(HSTX_SRCTRL_MEASURE<<RG_USB20_HSTX_SRCTRL_OFST)));
			fgRet = 1;
			printk(KERN_ERR "Use default SR calibration value \n");
		}
		else{
			// set reg = (1024/FM_OUT) * REF_CK * U2_SR_COEF (round to the nearest digits)
			u4Tmp = (((1024 * REF_CK * U2_SR_COEF) / u4FmOut) + 500) / 1000; 
			printk(KERN_ERR "SR calibration value = %d\n", (PHY_UINT8)u4Tmp);
			U3PhyWriteReg32(RG_USB20_HSTX_SRCTRL_ADDR+0x10000*j, (readl(RG_USB20_HSTX_SRCTRL_ADDR+0x10000*j)&(~RG_USB20_HSTX_SRCTRL)|((u4Tmp&0x7)<<RG_USB20_HSTX_SRCTRL_OFST)));
		}
	}
	return fgRet;
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

PHY_INT32 U3PhyWriteReg32(PHY_UINT32 addr, PHY_UINT32 data)
{
        writel(data, addr);

        return 0;
}

PHY_INT32 U3PhyReadReg32(PHY_UINT32 addr)
{
        return readl(addr);
}

PHY_INT32 U3PhyWriteReg8(PHY_UINT32 addr, PHY_UINT8 data)
{
        writelmsk(addr&0xfffffffc, data<<((addr%4)*8), 0xff<<((addr%4)*8));
        return 0;
}

PHY_INT8 U3PhyReadReg8(PHY_UINT32 addr)
{
        return ((readl(addr)>>((addr%4)*8))&0xff);
}

#endif

int ecnt_u3h_phy_init(void)
{
	int ret = -1;


	if(init_done_flag){
		return 1;
	}
		
#ifndef TCSUPPORT_CPU_ARMV8
	
	if(isFPGA){
                ret = u3phy_config_fpga();
        }else if(isEN7528){
		printk(KERN_ERR "USB driver version: 7528.3.20190226\n");
		u2_port_num = 2;
		
		ret = u3phy_config_en7528();
		u2_slew_rate_cal_en7528();
		
		mdelay(1);
		
	}else if(isEN7580){
		printk(KERN_ERR "USB driver version: 7580.2.20181030\n");
		u2_port_num = 2;
		
		ret = u3phy_config_en7580();
		u2_slew_rate_cal_en7580();
		
		mdelay(1);
				
	}else{
		printk(KERN_ERR "**Unknown chip ID for USB driver**\n");
		ret = -1;
	}	

#else
	if(isEN7523){
		printk(KERN_ERR "USB driver version: 7523.4.20210825\n");

		//U2 only: 7523DU, 7523GU, 
		//NO USB: 7523SU,
		/*
		if(isEN7523SU)
		{
			u2_port_num = 0;
		}else if(isEN7523DU ||isEN7523GU)
		{
			u2_port_num = 1;
			ret = u3phy_config_en7523();
		}else
		{
			u2_port_num = 2;		
			ret = u3phy_config_en7523();
		}
		u2_slew_rate_cal_en7523();
		
		mdelay(1);
		*/

		u2_port_num = 2;
		
		ret = u3phy_config_en7523();
		u2_slew_rate_cal_en7523();
		
		mdelay(1);
				
	}else{
		printk(KERN_ERR "**Unknown chip ID for USB driver**\n");
		ret = -1;
	}	


#endif
	init_done_flag = 1;

	
	return ret;	
}

#ifdef TCSUPPORT_CPU_ARMV8
EXPORT_SYMBOL(ecnt_u3h_phy_init);


#endif
