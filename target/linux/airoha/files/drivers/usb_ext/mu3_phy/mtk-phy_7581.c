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
#ifdef TCSUPPORT_CPU_EN7581
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
#include <linux/device.h>

#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/tc3162/tc3162.h>
#include <linux/delay.h>

#include "ecnt_event_global/ecnt_event_serdes.h"

#include "mtk-phy_7581.h"
#include "mtk-phy_hook.h"

//#define USB_DEBUG

static int u2_port_num = 2;
static int u3_port_num = 2;
static int init_done_flag = 0;

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
int mtk_usb_phy_init(void);
int mtk_usb_PowerDown(u32 portnum);
int mtk_usb_PowerUp(u32 portnum);

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#ifdef USB_DEBUG
#define DEBUGk printk
#define USB_HOOK_TEST(x) ecnt_u3h_phy_PowerDown(x)

#else
#define DEBUGk(...)
#define USB_HOOK_TEST(x)
#endif



/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_usb {
	struct device *dev;
	void __iomem *uphy_base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/

struct ecnt_usb *ecnt_usb1 = NULL;
struct ecnt_usb *ecnt_ssusb1 = NULL;
struct ecnt_usb *ecnt_usb2 = NULL;


static const struct of_device_id ecnt_usb_phy_of_id[] = {
    { .compatible = "econet,ecnt-usb_phy"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_usb_phy_of_id);

static const struct mtk_usb_phy_ops USB_ops = {
	.init			= mtk_usb_phy_init,
	.PowerDown		= mtk_usb_PowerDown,
	.PowerUp		= mtk_usb_PowerUp,
};

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
extern u32 regRead_ssusb2_phy(u32 reg);
extern void regWrite_ssusb2_phy(u32 reg, u32 val);
extern u32 GET_PDIDR(void);
/*****************************************************************
 ****  Reg   a c c e s s ********************************
 ******************************************************************/

u32 get_USB1_phy_reg(u32 reg)
{
	DEBUGk("Elijah:get u1 rg on %px\n",ecnt_usb1->uphy_base + reg);
	return readl(ecnt_usb1->uphy_base + (reg));
}
u32 get_SSUSB1_phy_reg(u32 reg)
{
	DEBUGk("Elijah:set ssu1 rg  on %px\n",ecnt_ssusb1->uphy_base + reg);
	return readl(ecnt_ssusb1->uphy_base + (reg));
}
u32 get_USB2_phy_reg(u32 reg)
{
	return readl(ecnt_usb2->uphy_base + (reg));	
}

void set_USB1_phy_reg(u32 reg, u32 val)
{
	writel(val, (ecnt_usb1->uphy_base + (reg))); 
}
void set_SSUSB1_phy_reg(u32 reg, u32 val)
{
	writel(val, (ecnt_ssusb1->uphy_base + (reg))); 
}
void set_USB2_phy_reg(u32 reg, u32 val)
{
	writel(val, (ecnt_usb2->uphy_base + (reg))); 
}
//EXPORT_SYMBOL(get_USB1_phy_reg);

u32 get_uphy_reg(u32 port,u16 reg) // lower 2 bytes of absolute address 
{
	if(reg >= 0x0700 && reg <= 0x0c64)
	{
		if(port==2)
		{
			DEBUGk("Elijah:get u2 rg on %x\n",reg + 0x1fae0000);
			return regRead_ssusb2_phy(reg + 0x1fae0000);
		}
		else
			return get_SSUSB1_phy_reg(reg - 0x0700);
	}
	else if(reg >= 0x0000 && reg <= 0x03FC)
	{
		if(port==2)
			return get_USB2_phy_reg(reg);
		else
			return get_USB1_phy_reg(reg);
	}
	return 0xdeadbeef;
}

void set_uphy_reg(u32 port,u16 reg, u32 val) //lower 2 bytes of absolute address 
{
	if(reg >= 0x0700 && reg <= 0x0c64)
	{
		if(port==2)
			regWrite_ssusb2_phy(reg + 0x1fae0000, val);
		else
			set_SSUSB1_phy_reg(reg - 0x0700, val);
	}
	else if(reg >= 0x0000 && reg <= 0x03FC)
	{
		if(port==2)
			set_USB2_phy_reg(reg, val);
		else
			set_USB1_phy_reg(reg, val);
	}
}

void rw_uphy_reg(u32 port,u16 reg, u32 val, u32 mask, u32 offset)
{
	u32	data = get_uphy_reg(port,reg);
	if (data==0xdeadbeef)
		return;
	//data &=(~mask);
	//data |=(val<<offset)&mask;
	data &=~(mask<<offset);
	data |=((val&mask)<<offset);
	set_uphy_reg(port,reg,data);
}

EXPORT_SYMBOL(get_uphy_reg);
EXPORT_SYMBOL(set_uphy_reg);
EXPORT_SYMBOL(rw_uphy_reg);


int ECNT_USB1_DRV_PROBE(void)
{
	struct resource *res = NULL;
    struct device_node *node=NULL;
    struct platform_device *pdev=NULL;

	int ret = 0;

    node = of_find_node_by_path("/usb1_phy@1fac0000");
	DEBUGk("ELijah test probe u1\n");
    if (node==NULL) {
        printk("\nERROR(%s) node==NULL\n", __func__);
        return -1;
    }

    pdev = of_find_device_by_node(node);
    if (pdev==NULL) {
        printk("\nERROR(%s) pdev==NULL\n", __func__);
        return -1;
    }

	ecnt_usb1 = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_usb), GFP_KERNEL);
	if (!ecnt_usb1)
		return -ENOMEM;
	platform_set_drvdata(pdev, ecnt_usb1);

	/* Get NP SCU address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ecnt_usb1->uphy_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ecnt_usb1->uphy_base)) {
        printk("\nERROR(%s) ecnt_usb_base\n", __func__);
		return -1;
    }
	ecnt_usb1->dev = &pdev->dev;
	mtk_usb_phy_ops_init(&USB_ops);
	return ret;
}

int ECNT_SSUSB1_DRV_PROBE(void)
{
	struct resource *res = NULL;
    struct device_node *node=NULL;
    struct platform_device *pdev=NULL;

	int ret = 0;

    node = of_find_node_by_path("/ssusb1_phy@1fac0700");
	DEBUGk("ELijah test probe ssu1\n");
    if (node==NULL) {
        printk("\nERROR(%s) node==NULL\n", __func__);
        return -1;
    }

    pdev = of_find_device_by_node(node);
    if (pdev==NULL) {
        printk("\nERROR(%s) pdev==NULL\n", __func__);
        return -1;
    }

	ecnt_ssusb1 = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_usb), GFP_KERNEL);
	if (!ecnt_ssusb1)
		return -ENOMEM;
	platform_set_drvdata(pdev, ecnt_ssusb1);

	/* Get NP SCU address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ecnt_ssusb1->uphy_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ecnt_ssusb1->uphy_base)) {
        printk("\nERROR(%s) ecnt_usb_base\n", __func__);
		return -1;
    }
	ecnt_ssusb1->dev = &pdev->dev;
	return ret;
}

int ECNT_USB2_DRV_PROBE(void)
{
	struct resource *res = NULL;
    struct device_node *node=NULL;
    struct platform_device *pdev=NULL;

	int ret = 0;

    node = of_find_node_by_path("/usb2_phy@1fae0000");
	DEBUGk("ELijah test probe u2\n");
    if (node==NULL) {
        printk("\nERROR(%s) node==NULL\n", __func__);
        return -1;
    }

    pdev = of_find_device_by_node(node);
    if (pdev==NULL) {
        printk("\nERROR(%s) pdev==NULL\n", __func__);
        return -1;
    }

	ecnt_usb2 = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_usb), GFP_KERNEL);
	if (!ecnt_usb2)
		return -ENOMEM;
	platform_set_drvdata(pdev, ecnt_usb2);

	/* Get NP SCU address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ecnt_usb2->uphy_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ecnt_usb2->uphy_base)) {
        printk("\nERROR(%s) ecnt_usb_base\n", __func__);
		return -1;
    }
	ecnt_usb2->dev = &pdev->dev;
	USB_HOOK_TEST(2);
	return ret;
}



/* init SCU registers' base address (a.s.a.p.) before any kernel module might access it. 
 * For example, usb_init() calls "isFPGA" which will access NP SCU register. 
 * If SCU base address has not initialized before that, cpu will crash. 
 * usb_init() uses subsys_initcall to init. Although ECNT_SCU_DRV_PROBE also uses
 * the smae subsys_initcall, it's executed before usb_init(), so it's ok 
 * Note: you can check linux-4.4.115/System.map to see which initcall function will be executed first*/
subsys_initcall(ECNT_USB1_DRV_PROBE);
subsys_initcall(ECNT_SSUSB1_DRV_PROBE);
subsys_initcall(ECNT_USB2_DRV_PROBE);

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static int u3phy_config(u32 port){
	
	//U3 CDR Setting
    DEBUGk("reg read test : CDR Setting %x\n",get_uphy_reg(port,RG_SSUSB_CDR_RST_RESERVE_ADDR));
	
	rw_uphy_reg(port,RG_SSUSB_CDR_RST_DLY_ADDR,0x0,RG_SSUSB_CDR_RST_DLY_MASK,RG_SSUSB_CDR_RST_DLY_OFST);
    rw_uphy_reg(port,RG_SSUSB_CDR_RST_RESERVE_ADDR,0x0E,RG_SSUSB_CDR_RST_RESERVE_MASK,RG_SSUSB_CDR_RST_RESERVE_OFST);
    rw_uphy_reg(port,RG_SSUSB_BG_DIV_ADDR,0x1,RG_SSUSB_BG_DIV_MASK,RG_SSUSB_BG_DIV_OFST);
	rw_uphy_reg(port,RG_SSUSB_XTAL_TOP_RESERVE_ADDR,0x600,0x600,RG_SSUSB_XTAL_TOP_RESERVE_OFST);
			
	DEBUGk("reg read test : CDR Setting %x\n",get_uphy_reg(port,RG_SSUSB_CDR_RST_RESERVE_ADDR));
	DEBUGk("Read USB impedance setting\n");
	DEBUGk("TX_IMP_SEL_SSUSB_P0 0x1fad0910 [28:24]:0x%x \n",  (get_uphy_reg(port,0x910)>>24) &0x1f);
	DEBUGk("RX_IMP_SEL_SSUSB_P0 0x1fad0914 [28:24]:0x%x \n", (get_uphy_reg(port,0x914)>>24)&0x1f);
	DEBUGk("INTR_CTRL_SSUSB_P0 0x1fad0b00 [15:10]:0x%x \n",  (get_uphy_reg(port,0xb00)>>10)&0x3f);
	return 0;
}
int u2phy_config(void){
    
    int port=1;
	
    for (port=1;port<=u2_port_num;port++)
    {
		//U2 init
        rw_uphy_reg(port,RG_USB20_BC11_SW_EN_ADDR,0,RG_USB20_BC11_SW_MASK,RG_USB20_BC11_SW_EN_OFST);
        mdelay(1);    
        DEBUGk("USB20_INTR_CTRL_P%d 0x1fad0304 [23:19]:0x%x \n",port,  (get_uphy_reg(port,0x0304)>>19)&0x1f);
        rw_uphy_reg(port,RG_USBPHYACR4_ADDR,6,RG_USB20_FS_CR_MASK,RG_USB20_FS_CR_OFST);
        rw_uphy_reg(port,RG_USBPHYACR4_ADDR,2,RG_USB20_FS_SR_MASK,RG_USB20_FS_SR_OFST);
		//
		if (GET_PDIDR()>=2){
			rw_uphy_reg(port,RG_USBPHYACR6_ADDR,0x9,RG_USB20_SQTH_MASK,RG_USB20_SQTH_OFST);
		}
		rw_uphy_reg(port,RG_USBPHYACR6_ADDR,0xA,RG_USB20_DISCTH_MASK,RG_USB20_DISCTH_OFST);
    }
    return 0;
}

int u2_slew_rate_cal(void)
{
	int i=0;
	int j=0;
	int fgRet = 0;
	int u4FmOut = 0;	
	int u4Tmp = 0;

	for(j=1;j<=u2_port_num;j++)
	{
		printk(KERN_ERR "port %d u2_slew_rate_cal_7581\n",j);
		// => RG_USB20_HSTX_SRCAL_EN = 1
		// enable HS TX SR calibration
		rw_uphy_reg(j,RG_USB20_HSTX_SRCAL_EN_ADDR,0x1,RG_USB20_HSTX_SRCAL_EN,RG_USB20_HSTX_SRCAL_EN_OFST);
		mdelay(1);	
		DEBUGk("RG_USB20_HSTX_SRCAL_EN(%x): %x\n",RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j ,get_uphy_reg(j,RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j));
		
		// => RG_FRCK_EN = 1    
		// Enable free run clock
		rw_uphy_reg(j,RG_FRCK_EN_ADDR,0x1,RG_FRCK_EN,RG_FRCK_EN_OFST);
		DEBUGk("RG_FRCK_EN(%x): %x\n",RG_FRCK_EN_ADDR ,get_uphy_reg(j,RG_FRCK_EN_ADDR));

		// => RG_MONCLK_SEL = 0x0/0x1 for port0/port1
		// Setting MONCLK_SEL
		rw_uphy_reg(j,RG_MONCLK_SEL_ADDR,j,RG_MONCLK_SEL,RG_MONCLK_SEL_OFST);
		DEBUGk("RG_MONCLK_SEL_ADDR(%x): %x\n",RG_MONCLK_SEL_ADDR ,get_uphy_reg(j,RG_MONCLK_SEL_ADDR));

		// => RG_CYCLECNT = 0x400
		// Setting cyclecnt = 0x400
		rw_uphy_reg(j,RG_CYCLECNT_ADDR,0x400,RG_CYCLECNT,RG_CYCLECNT_OFST);
		DEBUGk("RG_CYCLECNT(%x): %x\n",RG_CYCLECNT_ADDR ,get_uphy_reg(j,RG_CYCLECNT_ADDR));
		
		// => RG_FREQDET_EN = 1
		// Enable frequency meter
		rw_uphy_reg(j,RG_FREQDET_EN_ADDR,0x1,RG_FREQDET_EN,RG_FREQDET_EN_OFST);
		DEBUGk("RG_FREQDET_EN(%x): %x\n",RG_FREQDET_EN_ADDR ,get_uphy_reg(j,RG_FREQDET_EN_ADDR));
		// wait for FM detection done, set 10ms timeout
		for(i=0; i<10; i++){
			u4FmOut = get_uphy_reg(j,FM_OUT_ADDR);
			// check if FM detection done 
			if (u4FmOut != 0)
			{
				// => u4FmOut = USB_FM_OUT
				// read FM_OUT
				printk(KERN_ERR "FM_OUT value = %d(0x%08X)\n", u4FmOut, u4FmOut);
				fgRet = 0;
				DEBUGk(KERN_ERR "FM detection done! loop = %d\n", i);
				break;
			}

			fgRet = 1;
			mdelay(1);
		}
		// => RG_FREQDET_EN = 0
		// disable frequency meter
		rw_uphy_reg(j,RG_FREQDET_EN_ADDR,0x0,RG_FREQDET_EN,RG_FREQDET_EN_OFST);
		DEBUGk("RG_CYCLECNT(%x): %x\n", RG_FREQDET_EN_ADDR, get_uphy_reg(j,RG_FREQDET_EN_ADDR));
		// => RG_FRCK_EN = 0
		// disable free run clock
		rw_uphy_reg(j,RG_FRCK_EN_ADDR,0x0,RG_FRCK_EN,RG_FRCK_EN_OFST);
		DEBUGk("RG_FRCK_EN_ADDR(%x): %x\n", RG_FRCK_EN_ADDR, get_uphy_reg(j,RG_FRCK_EN_ADDR));

		// => RG_USB20_HSTX_SRCAL_EN = 0
		// disable HS TX SR calibration
		rw_uphy_reg(j,RG_USB20_HSTX_SRCAL_EN_ADDR,0x0,RG_USB20_HSTX_SRCAL_EN,RG_USB20_HSTX_SRCAL_EN_OFST);
		mdelay(1);
		DEBUGk("RG_USB20_HSTX_SRCAL_EN_ADDR(%x): %x\n", RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j, get_uphy_reg(j,RG_USB20_HSTX_SRCAL_EN_ADDR+0x1000*j));

		// In case not K success, use measured value
		if(u4FmOut == 0){
			rw_uphy_reg(j,RG_USB20_HSTX_SRCTRL_ADDR,HSTX_SRCTRL_MEASURE,RG_USB20_HSTX_SRCTRL,RG_USB20_HSTX_SRCTRL_OFST);
			fgRet = 1;
			printk(KERN_ERR "Use default SR calibration value \n");
		}
		else{
			// set reg = (1024/FM_OUT) * REF_CK * U2_SR_COEF (round to the nearest digits)
			u4Tmp = (((1024 * REF_CK * U2_SR_COEF) / u4FmOut) + 500) / 1000; 
			printk(KERN_ERR "SR calibration value = %d\n", (u8)u4Tmp);
			rw_uphy_reg(j,RG_USB20_HSTX_SRCTRL_ADDR,(u4Tmp&0x7),RG_USB20_HSTX_SRCTRL,RG_USB20_HSTX_SRCTRL_OFST);
		}
	}
	return fgRet;
}

int mtk_usb_phy_init(void)
{
	int ret = -1;
	u32 serdes_sel=-1;
	DEBUGk("USB init\n");
	if(init_done_flag){
		return 1;
	}
	if(isEN7581){
		printk(KERN_ERR "USB driver version: 7581.2.20230918\n");
        u2_port_num = 2;
        serdes_sel=get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_USB1);
		DEBUGk("SSUSB1 sel : %d\n",serdes_sel);
        if (serdes_sel==ECNT_EVENT_SERDES_USB1_USB30)
        {
            ret = u3phy_config(1);
        }
        serdes_sel=get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_USB2);
		DEBUGk("SSUSB2 sel : %d\n",serdes_sel);
        if (serdes_sel==ECNT_EVENT_SERDES_USB2_USB30)
        {
            ret = u3phy_config(2);
        }
        u2phy_config();
		u2_slew_rate_cal();
		mdelay(1);
	}else{
		printk(KERN_ERR "**Unknown chip ID for USB driver**\n");
		ret = -1;
	}	

	init_done_flag = 1;
	
	return ret;	
}

int mtk_usb_PowerDown(u32 portnum)
{
	/*portnum :0 for all port,  1 for port1U3, 2 for port2U3, 3 for port1U2, 4 for port2U2, */
	u32 port=1;
    //printk("Performing mtk_usb_PowerDown(%d).\n",portnum);
    switch(portnum)
    {
        case 0:
            //U3
            for (port=1;port<=u3_port_num;port++)
            {
                rw_uphy_reg(port,RG_c60802_gpio_ctlc_ADDR,0xA0000000,RG_c60802_gpio_ctlc_MASK,RG_c60802_gpio_ctlc_OFST);
                mdelay(1);
            }
            //U2
            for (port=1;port<=u2_port_num;port++)
            {
                rw_uphy_reg(port,RG_USB20_BC11_SW_EN_ADDR,0,RG_USB20_BC11_SW_MASK,RG_USB20_BC11_SW_EN_OFST);
                mdelay(1);
            }
            break;
        case 1:
            rw_uphy_reg(1,RG_c60802_gpio_ctlc_ADDR,0xA0000000,RG_c60802_gpio_ctlc_MASK,RG_c60802_gpio_ctlc_OFST);
            break;
        case 2:
            rw_uphy_reg(2,RG_c60802_gpio_ctlc_ADDR,0xA0000000,RG_c60802_gpio_ctlc_MASK,RG_c60802_gpio_ctlc_OFST);
            break;
        case 3:
            rw_uphy_reg(1,RG_USB20_BC11_SW_EN_ADDR,1,RG_USB20_BC11_SW_MASK,RG_USB20_BC11_SW_EN_OFST);
            break;
        case 4:
            rw_uphy_reg(2,RG_USB20_BC11_SW_EN_ADDR,1,RG_USB20_BC11_SW_MASK,RG_USB20_BC11_SW_EN_OFST);
            break;
        default :
            printk("mtk_usb_PowerDown(%d). arg error\n",portnum);
            return -1;
            break;
    }
	/*do something*/
    mdelay(1);
	return 0;
}
int mtk_usb_PowerUp(u32 portnum)
{
	/*portnum :0 for all port,  1 for port1U3, 2 for port2U3, 3 for port1U2, 4 for port2U2, */
    u32 port=1;
	printk("Performing mtk_usb_PowerUp(%d).\n",portnum);
	switch(portnum)
    {
        case 0:
            //U3
            for (port=1;port<=u3_port_num;port++)
            {
                rw_uphy_reg(port,RG_c60802_gpio_ctlc_ADDR,0x0,RG_c60802_gpio_ctlc_MASK,RG_c60802_gpio_ctlc_OFST);
                mdelay(1);
            }
            //U2
            u2phy_config();
            break;
        case 1:
            rw_uphy_reg(1,RG_c60802_gpio_ctlc_ADDR,0x0,RG_c60802_gpio_ctlc_MASK,RG_c60802_gpio_ctlc_OFST);
            break;
        case 2:
            rw_uphy_reg(2,RG_c60802_gpio_ctlc_ADDR,0x0,RG_c60802_gpio_ctlc_MASK,RG_c60802_gpio_ctlc_OFST);
            break;
        case 3:
            rw_uphy_reg(1,RG_USB20_BC11_SW_EN_ADDR,0x0,RG_USB20_BC11_SW_MASK,RG_USB20_BC11_SW_EN_OFST);
            break;
        case 4:
            rw_uphy_reg(2,RG_USB20_BC11_SW_EN_ADDR,0x0,RG_USB20_BC11_SW_MASK,RG_USB20_BC11_SW_EN_OFST);
            break;
        default :
            printk("mtk_usb_PowerUp(%d). arg error\n",portnum);
            return -1;
            break;
    }
    mdelay(1);
	/*do something*/
	return 0;
}

#endif
