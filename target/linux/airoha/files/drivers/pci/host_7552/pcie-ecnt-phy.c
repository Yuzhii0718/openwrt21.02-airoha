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

#ifdef TCSUPPORT_CPU_ARMV8
/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <asm/io.h>
//#include <boot/packageInfo.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define rg_ssusb_tx_impsel 0x210
#define rg_ssusb_rx_impsel 0x214
#define rg_ssusb_iext_intr_ctrl 0x400
//#define rg_SCU_Xtal_SEL	0x254

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define get_efuse_data(x) 0

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_pcie_phy {
	struct device *dev;
	void __iomem *pc0_base;
	void __iomem *pc1_base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_pcie_phy *pcie_phy = NULL;

static const struct of_device_id ecnt_pcie_phy_of_id[] = {
    { .compatible = "econet,ecnt-pcie_phy"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_pcie_phy_of_id);
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
u32 get_pc0_reg(u32 reg)
{
	return readl(pcie_phy->pc0_base + (reg));
}

void set_pc0_reg(u32 reg, u32 val)
{
	writel(val, (pcie_phy->pc0_base + (reg))); 
}

u32 get_pc1_reg(u32 reg)
{
    return readl(pcie_phy->pc1_base + (reg));
}

void set_pc1_reg(u32 reg, u32 val)
{
	writel(val, (pcie_phy->pc1_base + (reg))); 
}


/* APIs */
#if 0
static void pcie_phy_load_efuse_7523(void)
{
#if 1
	if (get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_PCIE_P0) != 0)
	{
		set_pc0_reg(rg_ssusb_tx_impsel, get_pc0_reg(rg_ssusb_tx_impsel)&(~(0x1f<<24) )|((get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_PCIE_P0))<<24));
		set_pc0_reg(rg_ssusb_tx_impsel, get_pc0_reg(rg_ssusb_tx_impsel)|0x80000000);	//force_tx_impsel
		mdelay(1);
	}
	
	if (get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_PCIE_P0) != 0)
	{
		set_pc0_reg(rg_ssusb_rx_impsel, get_pc0_reg(rg_ssusb_rx_impsel)&(~(0x1f<<24) )|((get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_PCIE_P0))<<24));
		set_pc0_reg(rg_ssusb_rx_impsel, get_pc0_reg(rg_ssusb_rx_impsel)|0x80000000);	//force_rx_impsel
		mdelay(1);
	}
	
	if (get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_PCIE_P0) > 23)
	{	
		set_pc0_reg(rg_ssusb_iext_intr_ctrl, get_pc0_reg(rg_ssusb_iext_intr_ctrl)&(~(0x3f<<10) )|((get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_PCIE_P0))<<10));
		mdelay(1);
	}
	
	if (get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_PCIE_P1) != 0)
	{
		set_pc1_reg(rg_ssusb_tx_impsel, get_pc1_reg(rg_ssusb_tx_impsel)&(~(0x1f<<24) )|((get_efuse_data(DA_SSUSB_LN0_TX_IMP_SEL_PCIE_P1))<<24));
		set_pc1_reg(rg_ssusb_tx_impsel, get_pc1_reg(rg_ssusb_tx_impsel)|0x80000000);	//force_tx_impsel
		mdelay(1);
	}
	if (get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_PCIE_P1) != 0)
	{
		set_pc1_reg(rg_ssusb_rx_impsel, get_pc1_reg(rg_ssusb_rx_impsel)&(~(0x1f<<24) )|((get_efuse_data(DA_SSUSB_LN0_RX_IMP_SEL_PCIE_P1))<<24));
		set_pc1_reg(rg_ssusb_rx_impsel, get_pc1_reg(rg_ssusb_rx_impsel)|0x80000000);	//force_rx_impsel
		mdelay(1);
	}
	
	if (get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_PCIE_P1) > 23)
	{
		set_pc1_reg(rg_ssusb_iext_intr_ctrl, get_pc1_reg(rg_ssusb_iext_intr_ctrl)&(~(0x3f<<10) )|((get_efuse_data(RG_SSUB_IEXT_INTR_CTRL_PCIE_P1))<<10));
		mdelay(1);
	}
	
	printk("Read PCIe impedance setting \n");
	printk("TX_IMP_SEL_PCIE_P0 0x910 [28:24]:0x%x \n", (get_pc0_reg(rg_ssusb_tx_impsel)>>24) &0x1f);	
	printk("RX_IMP_SEL_PCIE_P0 0x914 [28:24]:0x%x \n", (get_pc0_reg(rg_ssusb_rx_impsel)>>24)&0x1f);
	printk("INTR_CTRL_PCIE_P0 0xb00 [15:10]:0x%x \n",  (get_pc0_reg(rg_ssusb_iext_intr_ctrl)>>10)&0x3f);
	printk("TX_IMP_SEL_PCIE_P1 0x910 [28:24]:0x%x \n", (get_pc1_reg(rg_ssusb_tx_impsel)>>24)&0x1f);
	printk("RX_IMP_SEL_PCIE_P1 0x914 [28:24]:0x%x \n", (get_pc1_reg(rg_ssusb_rx_impsel)>>24)&0x1f);
	printk("INTR_CTRL_PCIE_P1 0xb00 [15:10]:0x%x \n", (get_pc1_reg(rg_ssusb_iext_intr_ctrl)>>10)&0x3f);
#endif
}
#endif

static void pcieClk_init(void)
{
	printk("PCIe ref clk init.\n");
	if(get_xtal_sel() == 0)
	{
		printk("PCIe PLL init for 20MHz clk source\n");
		//port 0
		//0x1fa93b0c[31:24] = 0x7d
		set_pc0_reg(0x40c, 0x7d000001);
		//0x1fa93b10[24] = 0x1
		set_pc0_reg(0x410, 0xf9e00008);
		//0x1fa93b14[7:0] = 0x40
		set_pc0_reg(0x414, 0x140);
		//0x1fa93c38[23:16] = 0x38
		set_pc0_reg(0x538, 0x380047);
		//0x1fa93c3c[7:0] = 0x38
		//0x1fa93c3c[23:16] = 0x38
		set_pc0_reg(0x53c, 0x380038);
		//0x1fa93c40[7:0] = 0x38
		set_pc0_reg(0x540, 0x430038);
		//0x1fa93c44[7:0] = 0x36
		//0x1fa93c44[23:16] = 0x36
		set_pc0_reg(0x544, 0x360036);
		//0x1FA93C48[7:0] = 0x36
		//0x1FA93C48[23:16] = 0x36
		set_pc0_reg(0x548, 0x360036);
		udelay(10);
		//0x1FA93810[24] = 0x1
		//0x1FA93810[25] = 0x1 // rg_swrst_u3_phyd_force_en
		set_pc0_reg(0x110, 0x43000000);
		//0x1FA9380C[29] = 0x1
		//0x1FA9380C[31] = 0x1 // force_ssusb_ip_sw_rst
		set_pc0_reg(0x10c, 0xe0000000);
		udelay(1);
		//0x1FA93810[24] = 0x0
		//0x1FA93810[25] = 0x0 // rg_swrst_u3_phyd_force_en
		set_pc0_reg(0x110, 0x40000000);
		//0x1FA9380C[29] = 0x0
		//0x1FA9380C[31] = 0x0 // force_ssusb_ip_sw_rst
		set_pc0_reg(0x10c, 0x40000000);
		//port 1
		//0x1fa95b0c[31:24] = 0x7d
		set_pc1_reg(0x40c, 0x7d000001);
		//0x1fa95b10[24] = 0x1
		set_pc1_reg(0x410, 0xf9e00008);
		//0x1fa95b14[7:0] = 0x40
		set_pc1_reg(0x414, 0x140);
		//0x1fa95c38[23:16] = 0x38
		set_pc1_reg(0x538, 0x380047);
		//0x1fa95c3c[7:0] = 0x38
		//0x1fa95c3c[23:16] = 0x38
		set_pc1_reg(0x53c, 0x380038);
		//0x1fa95c40[7:0] = 0x38
		set_pc1_reg(0x540, 0x430038);
		//0x1fa95c44[7:0] = 0x36
		//0x1fa95c44[23:16] = 0x36
		set_pc1_reg(0x544, 0x360036);
		//0x1fa95C48[7:0] = 0x36
		//0x1fa95C48[23:16] = 0x36
		set_pc1_reg(0x548, 0x360036);
		udelay(10);
		//0x1fa95810[24] = 0x1
		//0x1fa95810[25] = 0x1 // rg_swrst_u3_phyd_force_en
		set_pc1_reg(0x110, 0x43000000);
		//0x1fa9580C[29] = 0x1
		//0x1fa9580C[31] = 0x1 // force_ssusb_ip_sw_rst
		set_pc1_reg(0x10c, 0xe0000000);
		udelay(1);
		//0x1fa95810[24] = 0x0
		//0x1fa95810[25] = 0x0 // rg_swrst_u3_phyd_force_en
		set_pc1_reg(0x110, 0x40000000);
		//0x1fa9580C[29] = 0x0
		//0x1fa9580C[31] = 0x0 // force_ssusb_ip_sw_rst
		set_pc1_reg(0x10c, 0x40000000);
	}
}

static void pcie_PowerSaving(void)
{
	u32 val;
	//reduce TX driving to save power
	//pcie 0
	//1fa93a04[14]=1, [13:8]=0x24
	val = get_pc0_reg(0x304) & 0xffff80ff | 0x6400;
	set_pc0_reg(0x304, val);
	//1fa93b28[2:0]=0x1
	val = get_pc0_reg(0x428) & 0xfffffff8 | 0x1;
	set_pc0_reg(0x428, val);

	//pcie 1
	//1fa95a04[14]=1, [13:8]=0x24
	val = get_pc1_reg(0x304) & 0xffff80ff | 0x6400;
	set_pc1_reg(0x304, val);
	//1fa95b28[2:0]=0x1
	val = get_pc1_reg(0x428) & 0xfffffff8 | 0x1;
	set_pc1_reg(0x428, val);	
}

static void pcie_initSequence(void)
{
	printk("PCIe init.\n");
	printk("Iven check\n");
	//port 0
	// 1fa93b18[31:24] 0x0C
	set_pc0_reg(0x418, 0x0C010000);
	// 1fa93b04[25:10] 0x0200
	set_pc0_reg(0x404, 0x60080007);	
	// 1fa93a18[28:24] 0x4 rg_ssusb_g2_cdr_bir_ltd1[4:0]
	set_pc0_reg(0x318, 0x4100803); 	 //20230911 - improve PCIE RX JTOL performance 
		
	//port 1
	// 1fa95b18[31:24] 0x0C
	set_pc1_reg(0x418, 0x0C010000);
	// 1fa95b04[25:10] 0x0200
	set_pc1_reg(0x404, 0x60080007);
	// 1fa95a18[28:24] 0x4 rg_ssusb_g2_cdr_bir_ltd1[4:0]
	set_pc1_reg(0x318, 0x4100803);  // 20230911 - improve PCIE RX JTOL performance 	
}

static int pcie_phy_drv_probe(struct platform_device *pdev)
{
  struct resource *res = NULL;
	int ret = 0;

	printk("PCIe driver version: 7552.1.20230912\n");
	
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No pcie_phy DT node found");
        return -EINVAL;
    }

    pcie_phy = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_pcie_phy), GFP_KERNEL);
    if (!pcie_phy)
        return -ENOMEM;

    platform_set_drvdata(pdev, pcie_phy);

    /* get pc0 base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    pcie_phy->pc0_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(pcie_phy->pc0_base))
        return PTR_ERR(pcie_phy->pc0_base);
	
    /* get pc1 base address */	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
   	pcie_phy->pc1_base = devm_ioremap_resource(&pdev->dev, res);
   	if (IS_ERR(pcie_phy->pc1_base))
	   return PTR_ERR(pcie_phy->pc1_base);

    pcie_phy->dev = &pdev->dev;	

//	pcieClk_init();
//	pcie_PowerSaving();

	pcie_initSequence();
		
    return 0;
}
static int pcie_phy_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver pcie_phy_driver = {
    .probe = pcie_phy_drv_probe,
    .remove = pcie_phy_drv_remove,
    .driver = {
	    .name = "en7552-pcie_phy",
	    .of_match_table = ecnt_pcie_phy_of_id
    },
};
builtin_platform_driver(pcie_phy_driver);


//MODULE_DESCRIPTION("EcoNet pcie phy Driver");
#endif
