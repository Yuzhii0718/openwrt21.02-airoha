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

#include <asm/io.h>

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define CR_HSGMII_AE_PHY_BASE		(0x1fa08000)
#define CR_HSGMII_AE_PHY_RANGE		(0x300)
#define CR_HSGMII_AE_PHY_END		(CR_HSGMII_AE_PHY_BASE + CR_HSGMII_AE_PHY_RANGE)

#define CR_HSGMII_USB_PHY_BASE		(0x1fa07000)
#define CR_HSGMII_USB_PHY_RANGE		(0x300)
#define CR_HSGMII_USB_PHY_END		(CR_HSGMII_USB_PHY_BASE + CR_HSGMII_USB_PHY_RANGE)

#define CR_HSGMII_PCIE0_PHY_BASE		(0x1fa04000)
#define CR_HSGMII_PCIE0_PHY_RANGE		(0x300)
#define CR_HSGMII_PCIE0_PHY_END		(CR_HSGMII_PCIE0_PHY_BASE + CR_HSGMII_PCIE0_PHY_RANGE)

#define CR_HSGMII_PCIE1_PHY_BASE		(0x1fa05000)
#define CR_HSGMII_PCIE1_PHY_RANGE		(0x300)
#define CR_HSGMII_PCIE1_PHY_END			(CR_HSGMII_PCIE1_PHY_BASE + CR_HSGMII_PCIE1_PHY_RANGE)


#define CR_HSGMII_ETH_PHY_BASE		(0x1fa09000)
#define CR_HSGMII_ETH_PHY_RANGE		(0x300)
#define CR_HSGMII_ETH_PHY_END		(CR_HSGMII_ETH_PHY_BASE + CR_HSGMII_ETH_PHY_RANGE)



/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_xsi_str {
	struct device *dev;
	void __iomem *hsgmii_ae_base;
	void __iomem *hsgmii_pcie0_base;
	void __iomem *hsgmii_pcie1_base;
	void __iomem *hsgmii_usb_base;
	void __iomem *hsgmii_eth_base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_xsi_str *ecnt_xsi = NULL;


static const struct of_device_id ecnt_xsi_of_id[] = {
    { .compatible = "econet,ecnt-xsi"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_xsi_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_ae_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_ae_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_ae_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_ae_base + reg); 
}


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_pcie0_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_pcie0_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_pcie0_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_pcie0_base + reg); 
}


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_pcie1_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_pcie1_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_pcie1_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_pcie1_base + reg); 
}


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_usb_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_usb_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_usb_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_usb_base + reg); 
}

/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_eth_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_eth_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_eth_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_eth_base + reg); 
}


/* APIs */
u32 get_xsi_data(u32 reg)
{
	u32 reg_phy = 0;
	u32 reg_offset = 0;

	/* translate addr to physical addr */
	if( reg > 0xa0000000)
		reg_phy = (reg & 0x1fffffff);
	else
		reg_phy = reg;
	
	reg_offset = reg_phy % 4;
	if(reg_offset != 0){
		printk("\nDatapath(%s) get reg error, error reg=0x%08X\n", __func__, reg);
		return 0;
	}
	
	if( (CR_HSGMII_AE_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_AE_PHY_END) )
		return get_hsgmii_ae_data(reg_phy - CR_HSGMII_AE_PHY_BASE);
	else if( (CR_HSGMII_PCIE0_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE0_PHY_END) )
		return get_hsgmii_pcie0_data(reg_phy - CR_HSGMII_PCIE0_PHY_BASE);
	else if( (CR_HSGMII_PCIE1_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE1_PHY_END) )
		return get_hsgmii_pcie1_data(reg_phy - CR_HSGMII_PCIE1_PHY_BASE);
	else if( (CR_HSGMII_USB_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_USB_PHY_END) )
		return get_hsgmii_usb_data(reg_phy - CR_HSGMII_USB_PHY_BASE);
	else if( (CR_HSGMII_ETH_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_ETH_PHY_END) )
		return get_hsgmii_eth_data(reg_phy - CR_HSGMII_ETH_PHY_BASE);
	else
		printk("\nDatapath(%s) get reg error, reg=0x%08X\n", __func__, reg);

	return 0;
}
EXPORT_SYMBOL(get_xsi_data);

void set_xsi_data(u32 reg, u32 val)
{
	u32 reg_phy = 0;
	u32 reg_offset = 0;

	/* translate addr to physical addr */
	if( reg > 0xa0000000)
		reg_phy = (reg & 0x1fffffff);
	else
		reg_phy = reg;

	reg_offset = reg_phy % 4;
	if(reg_offset != 0){
		printk("\nDatapath(%s) set reg error, error reg=0x%08X\n", __func__, reg);
		return 0;
	}

	if( (CR_HSGMII_AE_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_AE_PHY_END) )
	 	set_hsgmii_ae_data(reg_phy - CR_HSGMII_AE_PHY_BASE, val);
	else if( (CR_HSGMII_PCIE0_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE0_PHY_END) )
		set_hsgmii_pcie0_data(reg_phy - CR_HSGMII_PCIE0_PHY_BASE, val);
	else if( (CR_HSGMII_PCIE1_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE1_PHY_END) )
		set_hsgmii_pcie1_data(reg_phy - CR_HSGMII_PCIE1_PHY_BASE, val);
	else if( (CR_HSGMII_USB_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_USB_PHY_END) )
		set_hsgmii_usb_data(reg_phy - CR_HSGMII_USB_PHY_BASE, val);
	else if( (CR_HSGMII_ETH_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_ETH_PHY_END) )
		set_hsgmii_eth_data(reg_phy - CR_HSGMII_ETH_PHY_BASE, val);
	else	
		printk("\nDatapath(%s) set reg error, reg=0x%08X\n", __func__, reg);

}
EXPORT_SYMBOL(set_xsi_data);

struct device* get_xsi_dev(void)
{
	if( (ecnt_xsi) && (ecnt_xsi->dev) )
		return ecnt_xsi->dev;
	else
		return NULL;
}
EXPORT_SYMBOL(get_xsi_dev);



static int ecnt_xsi_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
	int irq = 0;
	int irq_idx = 0;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No frame engine DT node found");
        return -EINVAL;
    }

    ecnt_xsi = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_xsi_str), GFP_KERNEL);
    if (!ecnt_xsi)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_xsi);

	/* get hsgmii ae base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_xsi->hsgmii_ae_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_ae_base))
        return PTR_ERR(ecnt_xsi->hsgmii_ae_base);

	/* get hsgmii pcie0 base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    ecnt_xsi->hsgmii_pcie0_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_pcie0_base))
        return PTR_ERR(ecnt_xsi->hsgmii_pcie0_base);

	/* get hsgmii pcie1 base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
    ecnt_xsi->hsgmii_pcie1_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_pcie1_base))
        return PTR_ERR(ecnt_xsi->hsgmii_pcie1_base);

	/* get hsgmii usb base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
    ecnt_xsi->hsgmii_usb_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_usb_base))
        return PTR_ERR(ecnt_xsi->hsgmii_usb_base);

	
	/* get hsgmii eth base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 4);
    ecnt_xsi->hsgmii_eth_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_eth_base))
        return PTR_ERR(ecnt_xsi->hsgmii_eth_base);

	
    ecnt_xsi->dev = &pdev->dev;

    return 0;
}

static int ecnt_xsi_remove(struct platform_device *pdev)
{
    return 0;
}


/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_xsi_driver = {
    .probe = ecnt_xsi_probe,
    .remove = ecnt_xsi_remove,
    .driver = {
        .name = "ecnt-xsi",
        .of_match_table = ecnt_xsi_of_id
    },
};
module_platform_driver(ecnt_xsi_driver);

MODULE_DESCRIPTION("EcoNet XSI Driver");


#else

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define CR_HSGMII_AE_PHY_BASE		(0x1fa60000)
#define CR_HSGMII_AE_PHY_RANGE		(0x300)
#define CR_HSGMII_AE_PHY_END		(CR_HSGMII_AE_PHY_BASE + CR_HSGMII_AE_PHY_RANGE)

#define CR_HSGMII_USB_PHY_BASE		(0x1fa80000)
#define CR_HSGMII_USB_PHY_RANGE		(0x300)
#define CR_HSGMII_USB_PHY_END		(CR_HSGMII_USB_PHY_BASE + CR_HSGMII_USB_PHY_RANGE)

#define CR_HSGMII_PCIE0_PHY_BASE		(0x1fa70000)
#define CR_HSGMII_PCIE0_PHY_RANGE		(0x300)
#define CR_HSGMII_PCIE0_PHY_END		(CR_HSGMII_PCIE0_PHY_BASE + CR_HSGMII_PCIE0_PHY_RANGE)

#define CR_HSGMII_PCIE1_PHY_BASE		(0x1fa71000)
#define CR_HSGMII_PCIE1_PHY_RANGE		(0x300)
#define CR_HSGMII_PCIE1_PHY_END			(CR_HSGMII_PCIE1_PHY_BASE + CR_HSGMII_PCIE1_PHY_RANGE)


/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_xsi_str {
	struct device *dev;
	void __iomem *hsgmii_ae_base;
	void __iomem *hsgmii_pcie0_base;
	void __iomem *hsgmii_pcie1_base;
	void __iomem *hsgmii_usb_base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_xsi_str *ecnt_xsi = NULL;


static const struct of_device_id ecnt_xsi_of_id[] = {
    { .compatible = "econet,ecnt-xsi"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_xsi_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_ae_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_ae_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_ae_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_ae_base + reg); 
}


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_pcie0_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_pcie0_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_pcie0_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_pcie0_base + reg); 
}


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_pcie1_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_pcie1_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_pcie1_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_pcie1_base + reg); 
}


/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_hsgmii_usb_data(u32 reg)
{
    return readl(ecnt_xsi->hsgmii_usb_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_hsgmii_usb_data(u32 reg, u32 val)
{
    writel(val, ecnt_xsi->hsgmii_usb_base + reg); 
}

/* APIs */
u32 get_xsi_data(u32 reg)
{
	u32 reg_phy = 0;
	u32 reg_offset = 0;

	/* translate addr to physical addr */
	if( reg > 0xa0000000)
		reg_phy = (reg & 0x1fffffff);
	else
		reg_phy = reg;
	
	reg_offset = reg_phy % 4;
	if(reg_offset != 0){
		printk("\nDatapath(%s) get reg error, reg=0x%08X\n", __func__, reg);
		return 0;
	}
	
	if( (CR_HSGMII_AE_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_AE_PHY_END) )
		return get_hsgmii_ae_data(reg_phy - CR_HSGMII_AE_PHY_BASE);
	else if( (CR_HSGMII_PCIE0_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE0_PHY_END) )
		return get_hsgmii_pcie0_data(reg_phy - CR_HSGMII_PCIE0_PHY_BASE);
	else if( (CR_HSGMII_PCIE1_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE1_PHY_END) )
		return get_hsgmii_pcie1_data(reg_phy - CR_HSGMII_PCIE1_PHY_BASE);
	else if( (CR_HSGMII_USB_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_USB_PHY_END) )
		return get_hsgmii_usb_data(reg_phy - CR_HSGMII_USB_PHY_BASE);
	else
		printk("\nDatapath(%s) get reg error, reg=0x%08X\n", __func__, reg);

	return 0;
}
EXPORT_SYMBOL(get_xsi_data);

void set_xsi_data(u32 reg, u32 val)
{
	u32 reg_phy = 0;
	u32 reg_offset = 0;

	/* translate addr to physical addr */
	if( reg > 0xa0000000)
		reg_phy = (reg & 0x1fffffff);
	else
		reg_phy = reg;

	reg_offset = reg_phy % 4;
	if(reg_offset != 0){
		printk("\nDatapath(%s) set reg error, reg=0x%08X\n", __func__, reg);
		return 0;
	}

	if( (CR_HSGMII_AE_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_AE_PHY_END) )
	 	set_hsgmii_ae_data(reg_phy - CR_HSGMII_AE_PHY_BASE, val);
	else if( (CR_HSGMII_PCIE0_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE0_PHY_END) )
		set_hsgmii_pcie0_data(reg_phy - CR_HSGMII_PCIE0_PHY_BASE, val);
	else if( (CR_HSGMII_PCIE1_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_PCIE1_PHY_END) )
		set_hsgmii_pcie1_data(reg_phy - CR_HSGMII_PCIE1_PHY_BASE, val);
	else if( (CR_HSGMII_USB_PHY_BASE <= reg_phy) && (reg_phy < CR_HSGMII_USB_PHY_END) )
		set_hsgmii_usb_data(reg_phy - CR_HSGMII_USB_PHY_BASE, val);
	else	
		printk("\nDatapath(%s) set reg error, reg=0x%08X\n", __func__, reg);

}
EXPORT_SYMBOL(set_xsi_data);

struct device* get_xsi_dev(void)
{
	if( (ecnt_xsi) && (ecnt_xsi->dev) )
		return ecnt_xsi->dev;
	else
		return NULL;
}
EXPORT_SYMBOL(get_xsi_dev);



static int ecnt_xsi_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
	int irq = 0;
	int irq_idx = 0;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No frame engine DT node found");
        return -EINVAL;
    }

    ecnt_xsi = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_xsi_str), GFP_KERNEL);
    if (!ecnt_xsi)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_xsi);

	/* get hsgmii ae base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_xsi->hsgmii_ae_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_ae_base))
        return PTR_ERR(ecnt_xsi->hsgmii_ae_base);

	/* get hsgmii pcie0 base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    ecnt_xsi->hsgmii_pcie0_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_pcie0_base))
        return PTR_ERR(ecnt_xsi->hsgmii_pcie0_base);

	/* get hsgmii pcie1 base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
    ecnt_xsi->hsgmii_pcie1_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_pcie1_base))
        return PTR_ERR(ecnt_xsi->hsgmii_pcie1_base);

	/* get hsgmii usb base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
    ecnt_xsi->hsgmii_usb_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_xsi->hsgmii_usb_base))
        return PTR_ERR(ecnt_xsi->hsgmii_usb_base);

	
    ecnt_xsi->dev = &pdev->dev;

    return 0;
}

static int ecnt_xsi_remove(struct platform_device *pdev)
{
    return 0;
}


/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_xsi_driver = {
    .probe = ecnt_xsi_probe,
    .remove = ecnt_xsi_remove,
    .driver = {
        .name = "ecnt-xsi",
        .of_match_table = ecnt_xsi_of_id
    },
};
module_platform_driver(ecnt_xsi_driver);

MODULE_DESCRIPTION("EcoNet XSI Driver");
#endif


/*
 * Appended: the newer kernel returns the per-port MAC EEE interrupt number
 * here. The EEE interrupt handling it belongs to only exists on AN7583, so on
 * this SoC the vendor implementation returns 0 as well.
 */
int get_xsi_mac_eee_irq(int index)
{
	(void)index;
	return 0;
}
EXPORT_SYMBOL(get_xsi_mac_eee_irq);
