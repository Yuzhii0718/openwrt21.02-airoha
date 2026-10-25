/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°Airoha Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to Airoha Limited (¡°Airoha¡±) and/or 
its licensors. Without the prior written permission of Airoha and/or its licensors, 
any reproduction, modification, use or disclosure of Airoha Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

Airoha Limited. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°AIROHA SOFTWARE¡±) RECEIVED FROM AIROHA 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES AIROHA PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE AIROHA SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN AIROHA SOFTWARE.

AIROHA SHALL NOT BE RESPONSIBLE FOR ANY AIROHA SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
AIROHA'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AIROHA 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT AIROHA'S SOLE OPTION, TO 
REVISE OR REPLACE THE AIROHA SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO AIROHA FOR SUCH 
AIROHA SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include <asm/io.h>
#include "./serdes_cmn_phy_7552.h"
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
#if 0
struct an7552_serdes_common_phy {
	struct device *dev;
	void __iomem *G3_ana2L_phy_rg_base; /* PCIEG3_PHY_PMA_PHYA physical address */
	void __iomem *G3_pma0_phy_rg_base; /* PCIEG3_PHY_PMA_PHYD_0 physical address */
	void __iomem *G3_pma1_phy_rg_base; /* PCIEG3_PHY_PMA_PHYD_1 physical address */
	void __iomem *xfi_ana_pxp_phy_rg_base; /* xfi_ana_pxp physical address */
	void __iomem *xfi_pma_phy_rg_base; /* xfi_pma physical address */
	void __iomem *pon_ana_pxp_phy_rg_base; /* pon_ana_pxp physical address */
	void __iomem *pon_pma_phy_rg_base; /* pon_pma physical address */
};
#endif
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
struct an7552_serdes_common_phy *serdes_common_phy = NULL;

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static const struct of_device_id an7552_serdes_common_phy_of_ids[] = {
	{ .compatible = "airoha,serdes_common_phy"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, an7552_serdes_common_phy_of_ids);

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/


/********************************************************************/

/* APIs */
/***********************************************/


struct an7552_serdes_common_phy* Get_Struct(void)
{
	return serdes_common_phy;
}
EXPORT_SYMBOL(Get_Struct);

void __iomem* Get_Base(u32 base)
{
	printk("\ncommon_phy Get_Base:0x%x\n",base);
	switch (base)
	{
		case 0x1faf3000:	
			return serdes_common_phy->Ana_csr_base;		
			break;
			
		case 0x1faf4000:			
			return serdes_common_phy->Dig_csr_base;			
			break;

		case 0x1fa84000:			
			return serdes_common_phy->Multi_sgmii_base;			
			break;
		default :
			printk("base addr not Serdes Common PHY addr!ex: 0x1faf3000, 0x1faf4000, 0x1fa84000\n");
			return NULL;
			break;
	};
}
EXPORT_SYMBOL(Get_Base);


int an7552_serdes_common_phy_probe(void)
{
	struct resource *res = NULL;
	struct device_node *node=NULL;
	struct platform_device *pdev=NULL;


	printk("\nan7552_serdes_common_phy_probe\n");
	
	node = of_find_node_by_path("/serdes_common_phy@1faf3000");
	if (node==NULL) {
	    printk("\nERROR(%s) node==NULL\n", __func__);
	    return -1;
	}

	pdev = of_find_device_by_node(node);
	if (pdev==NULL) {
	    printk("\nERROR(%s) pdev==NULL\n", __func__);
	    return -1;
	}

	serdes_common_phy = devm_kzalloc(&pdev->dev, sizeof(struct an7552_serdes_common_phy), GFP_KERNEL);
	if (!serdes_common_phy)
		return -ENOMEM;
	platform_set_drvdata(pdev, serdes_common_phy);

	/* PON_HSGMII_PHYA */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	serdes_common_phy->Ana_csr_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(serdes_common_phy->Ana_csr_base)) {
	    printk("\nERROR(%s) Ana_csr_base\n", __func__);
		return -1;
	}
    
	/* PON_HSGMII_PHYD  */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	serdes_common_phy->Dig_csr_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(serdes_common_phy->Dig_csr_base)) {
	    printk("\nERROR(%s) Dig_csr_base\n", __func__);
		return -1;
	}

	/* get Multi SGMII_1fa84000 */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	serdes_common_phy->Multi_sgmii_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(serdes_common_phy->Multi_sgmii_base)){
		printk("\nERROR(%s) Multi_sgmii_base\n", __func__);
		return -1;
	}
    
	serdes_common_phy->dev = &pdev->dev;

	printk("serdes_common_phy->Ana_csr_base= %lx\n", (unsigned long)serdes_common_phy->Ana_csr_base);
	printk("serdes_common_phy->Dig_csr_base= %lx\n", (unsigned long)serdes_common_phy->Dig_csr_base);
	printk("serdes_common_phy->Multi_sgmii_base= %lx\n", (unsigned long)serdes_common_phy->Multi_sgmii_base);
	return 0;
}

/* init SCU registers' base address (a.s.a.p.) before any kernel module might access it. 
 * For example, usb_init() calls "isFPGA" which will access NP SCU register. 
 * If SCU base address has not initialized before that, cpu will crash. 
 * usb_init() uses subsys_initcall to init. Although ECNT_SCU_DRV_PROBE also uses
 * the smae subsys_initcall, it's executed before usb_init(), so it's ok 
 * Note: you can check linux-4.4.115/System.map to see which initcall function will be executed first*/
subsys_initcall(an7552_serdes_common_phy_probe);

