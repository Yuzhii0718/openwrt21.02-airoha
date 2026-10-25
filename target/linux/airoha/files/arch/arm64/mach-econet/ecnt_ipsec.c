/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
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

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define IPSEC_BASE_OFFSET 0x0000

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_ipsec_str {
	struct device *dev;
	void __iomem *ipsec_base;
	unsigned int ipsec_irq;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_ipsec_str *ecnt_ipsec = NULL;

static const struct of_device_id ecnt_ipsec_of_id[] = {
    { .compatible = "econet,ecnt-ipsec"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_ipsec_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/*****************************************************************
 ****  N P    S C U    a c c e s s ********************************
 ******************************************************************/
unsigned int get_ipsec_data(unsigned int reg)
{
	reg = reg - IPSEC_BASE_OFFSET;
	return readl(ecnt_ipsec->ipsec_base + reg);
}
EXPORT_SYMBOL(get_ipsec_data);

void set_ipsec_data(unsigned int reg, unsigned int val)
{
	reg = reg - IPSEC_BASE_OFFSET;
    writel(val, ecnt_ipsec->ipsec_base + reg); 
}
EXPORT_SYMBOL(set_ipsec_data);

int get_ipsec_irq(void)
{
	return ecnt_ipsec->ipsec_irq;
}
EXPORT_SYMBOL(get_ipsec_irq);

struct device* get_ipsec_dev(void)
{
    if ((ecnt_ipsec) && (ecnt_ipsec->dev))
        return ecnt_ipsec->dev;
    else
        return NULL;
}
EXPORT_SYMBOL(get_ipsec_dev);

static int ecnt_ipsec_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
	int irq_idx = 0;
	int irq = -1;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No ipsec DT node found");
        return -EINVAL;
    }

    ecnt_ipsec = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_ipsec_str), GFP_KERNEL);
    if (!ecnt_ipsec)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_ipsec);

    /* get IPSec base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_ipsec->ipsec_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_ipsec->ipsec_base))
        return PTR_ERR(ecnt_ipsec->ipsec_base);
	
    ecnt_ipsec->dev = &pdev->dev;

	/* get irq num */
	irq = platform_get_irq(pdev, irq_idx);
	if (irq <= 0)
		return irq;
	
	ecnt_ipsec->ipsec_irq = irq;
	printk("irq-%d: %d ", irq_idx, irq);

    return 0;
}

static int ecnt_ipsec_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_ipsec_driver = {
    .probe = ecnt_ipsec_drv_probe,
    .remove = ecnt_ipsec_drv_remove,
    .driver = {
	    .name = "ecnt-ipsec",
	    .of_match_table = ecnt_ipsec_of_id
    },
};
module_platform_driver(ecnt_ipsec_driver);


MODULE_DESCRIPTION("EcoNet IPSec Driver");

