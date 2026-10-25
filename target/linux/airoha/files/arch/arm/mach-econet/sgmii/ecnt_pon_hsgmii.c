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
#include "ecnt_sgmii.h"
#include "sgmii_reg_pon.h"
#include <linux/delay.h>

#include <asm/io.h>
#include <uapi/ecnt_event_global/ecnt_event_system.h>
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define PON_PCS1_BASE_OFFSET 0x100
#define PON_PCS2_BASE_OFFSET 0xA00
#define PON_AN_BASE_OFFSET 0
#define PON_RA_BASE_OFFSET 0

#define PON_INT_EN 0x5f0
#define PON_INT_STA_CLR 0x5f4
#define PON_INT_STA 0x5f8


/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_hsgmii {
	struct device *dev;
	void __iomem *pcs1_base;
	void __iomem *pcs2_base;
	void __iomem *an_base;
	void __iomem *ra_base;
	int irq;
	int speed;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_hsgmii *ecnt_pon_hsgmii = NULL;

static const struct of_device_id ecnt_pon_hsgmii_of_id[] = {
    { .compatible = "econet,ecnt-pon_hsgmii"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_pon_hsgmii_of_id);
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static void sgmii_init(void);
extern int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel);
extern void SET_SSR3(u32 val);
extern u32 GET_SSR3(void);
extern u32 get_chip_scu_data(u32 reg);
extern void set_chip_scu_data(u32 reg, u32 val);
extern void sgmii_ver(void);


/*****************************************************************
 ****  N P    S C U    a c c e s s ********************************
 ******************************************************************/
 
u32 get_pon_pcs1_data(u32 reg)
{
	return readl(ecnt_pon_hsgmii->pcs1_base + (reg - PON_PCS1_BASE_OFFSET));
}

void set_pon_pcs1_data(u32 reg, u32 val)
{
	writel(val, (ecnt_pon_hsgmii->pcs1_base + (reg - PON_PCS1_BASE_OFFSET))); 
}

u32 get_pon_pcs2_data(u32 reg)
{
    return readl(ecnt_pon_hsgmii->pcs2_base + (reg - PON_PCS2_BASE_OFFSET));
}
EXPORT_SYMBOL(get_pon_pcs2_data);

u32 set_pon_pcs2_data(u32 reg, u32 val)
{
    writel(val, (ecnt_pon_hsgmii->pcs2_base + (reg - PON_PCS2_BASE_OFFSET)));
}
EXPORT_SYMBOL(set_pon_pcs2_data);

void set_pon_an_data(u32 reg, u32 val)
{
	writel(val, ecnt_pon_hsgmii->an_base + (reg - PON_AN_BASE_OFFSET)); 
}
EXPORT_SYMBOL(set_pon_an_data);

u32 get_pon_an_data(u32 reg)
{
	return readl(ecnt_pon_hsgmii->an_base + (reg - PON_AN_BASE_OFFSET));
}
EXPORT_SYMBOL(get_pon_an_data);

u32 get_pon_ra_data(u32 reg)
{
    return readl(ecnt_pon_hsgmii->ra_base + (reg - PON_RA_BASE_OFFSET));
}
EXPORT_SYMBOL(get_pon_ra_data);

u32 set_pon_ra_data(u32 reg, u32 val)
{
    writel(val, ecnt_pon_hsgmii->ra_base + (reg - PON_RA_BASE_OFFSET));
}
EXPORT_SYMBOL(set_pon_ra_data);

/* APIs */
static void sgmii_init(void)
{
	HAL_RG_TOP rg;
	uint32 data_t;
	printk("sgmii: pon_sgmii_init\n");

	int serdes_ret = get_serdes_sel(ECNT_EVENT_PON_HSGMII);
	u32 val_default = 0;

	data_t = get_pon_an_data(SGMII_REG_AN_13);
	rg.an.sgmii_reg_an_13 = &data_t;
	rg.an.sgmii_reg_an_13->sgmii_remote_fault_dis = 1;
	set_pon_an_data(SGMII_REG_AN_13, data_t);

	if (serdes_ret == 1) {
		printk("enable!\n");
		val_default = GET_SSR3();
		SET_SSR3((val_default & 0xFFFF9FFF) | 0x00004000);
		#if 0
		val_default = get_chip_scu_data(0x1e4);
		set_chip_scu_data(0x1e4, val_default | 0x4000);
		val_default = get_chip_scu_data(0x1e8);
		set_chip_scu_data(0x1e8, val_default & 0x20000);
		#endif
	} else if(serdes_ret == 0) {
		printk("close!\n");
		//val_default = GET_SSR3();
		//SET_SSR3(val_default | SSR3_mask_dis);
		
		val_default = get_chip_scu_data(0x1e4);
		set_chip_scu_data(0x1e4, val_default & 0xFFFFBFFF);
		val_default = get_chip_scu_data(0x1e8);
		set_chip_scu_data(0x1e8, val_default & 0xFFFDFFFF);
	} else {
		printk("ret error!\n");
	}

	printk("sgmii: sgmii_serdes_sel: exit!\n");
}


static void pon_hsgmii_int_init(void)
{
	return;
}

static irqreturn_t pon_hsgmii_interrupt(int irq, void *dev_id)
{
	uint32 pcs1_int_sts = 0;

	HAL_RG_TOP rg;
	uint32 data_t;
	uint32 reg_value = 0;
	uint32 wo_rateadapt = 0;
	uint32 wo_ra_bypass = 1;
	uint32 wo_an_en = 1;
	uint32 wo_ra_en = 1;
	uint32 default_val;
	uint32 cur_speed = SGMII_SPEED_UNKNOW;
	uint32 speed = 0;
	uint32 an_page = 0;
	uint32 mac_glb_cfg = 0;

#if DEBUG
	printk("\n%s: start\n", __func__);
#endif
	/* clear interrupt */
	pcs1_int_sts = get_pon_pcs1_data(PON_INT_STA);
	set_pon_pcs1_data(PON_INT_STA_CLR, 0x3ff);

	reg_value = get_pon_an_data(SGMII_REG_AN_13);
	rg.an.sgmii_reg_an_13 = &reg_value;
	rg.an.sgmii_reg_an_13->sgmii_remote_fault_dis = 1;
	set_pon_an_data(SGMII_REG_AN_13, reg_value);
	set_pon_an_data(SGMII_REG_AN_4, 0x00004001);
	set_pon_an_data(0x00,0x9340);

	msleep(30);
#if DEBUG
	printk("\n%s: set page 1\n", __func__);
	reg_value = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_STATE_2);
	printk("pon_sgmii:0xb04 = %x\n", reg_value);
#endif

	reg_value = get_pon_an_data(SGMII_REG_AN_5);
#if DEBUG
	printk("pon_sgmii:0xe14 = %x\n", reg_value);
#endif
	/* get an */
	if (reg_value == 0 || reg_value == 0x4001) {
		/* page all 0, or page is sgmii mac to phy */
		set_pon_an_data(SGMII_REG_AN_4, 0x000001a0);
		set_pon_an_data(0x00,0x9340);
		msleep(30);
#if DEBUG
		printk("\n%s: set page 1a0\n", __func__);
#endif
	}

	reg_value = get_pon_an_data(SGMII_REG_AN_5);
#if DEBUG
	printk("pon_sgmii:0xe14 = %x\n", reg_value);
#endif
	if ((reg_value & 0x1) == 0) {
		/* page for 802.3 1000Base-X */
		speed = 2;
		an_page = 0x000001A0;
		set_pon_an_data(SGMII_REG_AN_4, 0x000001a0);
	} else {
		if (reg_value == 0x4001) {
			/* page for sgmii mac to phy*/
			speed = 2;
			an_page = 0x000001A0;
			set_pon_an_data(SGMII_REG_AN_4, 0x000001a0);
		} else {
			/* page for sgmii phy to mac*/
			speed = (reg_value >> 10) & 0x3;	//get speed bit:11_10
			an_page = 0x00004001;
			set_pon_an_data(SGMII_REG_AN_4, 0x00004001);
		}
	}

#if DEBUG
	printk("pon_sgmii:set_rateadapt\n");
	printk("pon_sgmii: get_an_dump=%x\n", reg_value);
	printk("pon_sgmii:speed=%d\n",speed);
#endif

	switch(speed)
	{
		case 0x0: //10M
			printk("pon_sgmii:10M\n");
			wo_rateadapt = 2;
			wo_ra_bypass = 0;
			wo_an_en     = 1;
			wo_ra_en     = 1;
			cur_speed    = SGMII_SPEED_10M;
			break;
		case 0x1: //100M
			printk("pon_sgmii:100M\n");
			wo_rateadapt = 1;
			wo_ra_bypass = 0;
			wo_an_en     = 1;
			wo_ra_en     = 1;
			cur_speed    = SGMII_SPEED_100M;
			break;
		case 0x2: //1G
			printk("pon_sgmii:1G\n");
			wo_rateadapt = 0;
			wo_ra_bypass = 1;
			wo_an_en     = 1;
			wo_ra_en     = 1;
			cur_speed    = SGMII_SPEED_1000M;
			break;
		default:
			printk("pon_sgmii:int_an=%d, error!",speed);
			break;
	}

	if (ecnt_pon_hsgmii->speed != cur_speed) {
		mac_glb_cfg = get_xsi_data(0x1fa60000);
		default_val = GET_SCU_RST_RG();
		SET_SCU_RST_RG(default_val | 1);
	    SET_SCU_RST_RG(default_val & (~1));
        mdelay(10);
		//****/Walter Add
	    SET_SCU_RST_RG(0x180);
	    SET_SCU_RST_RG(0);

		set_pon_phy_data(SS_LCPLL_TDC_FLT_2_ADDR, 0x64000000 ); //25Mhz
		default_val = GET_SSR3();
		SET_SSR3((default_val & 0xFFFF9FFF) | 0x00004000); //0xe0004820 [14:13]=0
		SET_WAN_CONF(0xe0000010); //0x1fb00070 6 --> 11 , wan_sel as hsgmii 10,SGMII
		set_pon_phy_data(HG_RG_CTRL_0_ADDR, 0x220);
		set_pon_an_data(SGMII_REG_AN_4, an_page);
		set_pon_an_data(0x00, 0x1140); //1140 -> 140 AN DIS
		set_pon_phy_data(PON_SERDES_CTRL_10_ADDR, 0x103 ); //0x3 => E2
		set_pon_phy_data(FRQ_CTRL_2_ADDR, 0x3003 ); //3c03 -> 3003
		set_pon_phy_data(FRQ_CTRL_4_ADDR, 0x2710271 ); //6180618 => E2
		set_pon_phy_data(RG_SSUSB_LN0_CDR_PD_DIV_BYPASS_ADDR, 0x18001722 ); //18000722 -> 18001722
		set_pon_phy_data(HG_RST_CTRL_0_ADDR, 0x3f ); //0 -> 3f
		set_pon_phy_data(PON_DA_CTRL_2_ADDR, 0x54101801 ); //56101801 -> 54101801
		set_pon_phy_data(PON_RXFEDIG_CTRL_12_ADDR, 0x380013 ); //780013 -> 380013
		set_pon_phy_data(HG_MODE_CTRL_0_ADDR, 0x1 ); //0 -> 1
		set_pon_phy_data(SS_LCPLL_TDC_FLT_3_ADDR, 0x20000100 ); //20000000 --> 2000_0100
		set_pon_phy_data(SS_LCPLL_TDC_FLT_5_ADDR, 0x1010100 ); //101_0100 --> 10100
		set_pon_phy_data(PON_SERDES_CTRL_0_ADDR, 0x100381 ); //100380 -> 100381
		set_pon_phy_data(PON_SYS_CTRL_0_ADDR, 0x3f0003c0 );

		default_val = GET_SSR3();
		// maybe no necessary
		SET_SSR3((default_val & 0xF3FFFFFF) | (wo_rateadapt << 26));      // 100M : 0xe4004820 , 10M : 0xe8004820 , 1G : 0xe0004820

		set_pon_phy_data(HG_MODE_CTRL_0_ADDR, 0x1 | (wo_rateadapt << 4)); // 100M : 0x11, 10M : 0x21, 1G : 0x1
		//****/Walter Add

		// maybe no necessary
		reg_value = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_6);
		rg.pcs2.rg_hsgmii_pcs_ctrol_6 = &reg_value;
		rg.pcs2.rg_hsgmii_pcs_ctrol_6->rg_sgmii_force_rateadapt_value= wo_rateadapt;
		set_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_6, reg_value);

		reg_value = get_pon_ra_data(SGMII_RG_RATE_ADAPT_CTRL_0);
		rg.ra.rg_rate_adapt_ctrl_0 = &reg_value;
		rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_tx_bypass= wo_ra_bypass;
		rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_bypass= wo_ra_bypass;
		rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_tx_en= wo_ra_en;
		rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_en= wo_ra_en;
		set_pon_ra_data(SGMII_RG_RATE_ADAPT_CTRL_0, reg_value);

		// maybe no necessary
		reg_value = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_1);
		rg.pcs2.rg_hsgmii_pcs_ctrol_1 = &reg_value;
		rg.pcs2.rg_hsgmii_pcs_ctrol_1->rg_an_enable= wo_an_en;
		set_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_1, reg_value);

		reg_value = get_pon_an_data(SGMII_REG_AN_13);
		rg.an.sgmii_reg_an_13 = &reg_value;
		rg.an.sgmii_reg_an_13->sgmii_remote_fault_dis = 1;
		set_pon_an_data(SGMII_REG_AN_13, reg_value);

		set_pon_an_data(SGMII_REG_AN_4, an_page);
		set_pon_pcs1_data(PON_INT_EN, 0x22);
		msleep(30);
		set_xsi_data(0x1fa60000, mac_glb_cfg);
		/* clear interrupt */
		pcs1_int_sts = get_pon_pcs1_data(PON_INT_STA);
		set_pon_pcs1_data(PON_INT_STA_CLR, 0x3ff);

		ecnt_pon_hsgmii->speed = cur_speed;
	} else {
		printk("pon_sgmii: speed not changed\n");
	}

#if DEBUG
	reg_value = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_STATE_2);
	printk("pon_sgmii:0xb04 = %x\n", reg_value);
	printk("%s: exit\n", __func__);
#endif

	return IRQ_HANDLED;
}


static int ecnt_pon_hsgmii_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
	int ret = 0;
	sgmii_ver();
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No pon_hsgmii DT node found");
        return -EINVAL;
    }

    ecnt_pon_hsgmii = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_hsgmii), GFP_KERNEL);
    if (!ecnt_pon_hsgmii)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_pon_hsgmii);

    /* get pcs1 base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_pon_hsgmii->pcs1_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_pon_hsgmii->pcs1_base))
        return PTR_ERR(ecnt_pon_hsgmii->pcs1_base);
	
    /* get pcs2 base address */	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
   	ecnt_pon_hsgmii->pcs2_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(ecnt_pon_hsgmii->pcs2_base))
	   return PTR_ERR(ecnt_pon_hsgmii->pcs2_base);

	/* get AN base address */	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
   	ecnt_pon_hsgmii->an_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(ecnt_pon_hsgmii->an_base))
	   return PTR_ERR(ecnt_pon_hsgmii->an_base);

	/* get RA base address */	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
   	ecnt_pon_hsgmii->ra_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(ecnt_pon_hsgmii->ra_base))
	   return PTR_ERR(ecnt_pon_hsgmii->ra_base);

    ecnt_pon_hsgmii->dev = &pdev->dev;

	/* get irq num */
	ecnt_pon_hsgmii->irq = platform_get_irq(pdev, 0);
	if(ecnt_pon_hsgmii->irq <= 0) {
		printk("\n get pon hsgmii irq number failed\n");
		return ecnt_pon_hsgmii->irq;
	}

	ecnt_pon_hsgmii->speed = SGMII_SPEED_UNKNOW;
	
	sgmii_init();
	pon_hsgmii_int_init();
	
	/* request irq */
	ret = devm_request_threaded_irq(&pdev->dev, ecnt_pon_hsgmii->irq, NULL, pon_hsgmii_interrupt, IRQF_ONESHOT, "PonHsgmiiInt", ecnt_pon_hsgmii);
	if(ret) {
		printk("\n devm_request_threaded_irq() (irq number: %d) failed (ret: %d)\n", ecnt_pon_hsgmii->irq, ret);
		return (ret);
	}

    return 0;
}
static int ecnt_pon_hsgmii_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_pon_hsgmii_driver = {
    .probe = ecnt_pon_hsgmii_drv_probe,
    .remove = ecnt_pon_hsgmii_drv_remove,
    .driver = {
	    .name = "ecnt-pon_hsgmii",
	    .of_match_table = ecnt_pon_hsgmii_of_id
    },
};
module_platform_driver(ecnt_pon_hsgmii_driver);


MODULE_DESCRIPTION("EcoNet pon HSGMII Driver");

