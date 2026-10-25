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

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*                  M A C R O S
*************************************************************************
*/

#define CRYPTO_REG_BASE				0x00000000
#define CRYPTO_RAM_PD_CONTROL_REG	(CRYPTO_REG_BASE + 0x00000108)

#define CRYPTO_RAM_PD				(1<<0)

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_crypto_k {
	struct device *dev;
	void __iomem *base;
	int crypto_k_irq;
};

/* used in ledcetl.h */
unsigned long g_crypto_k_base;
EXPORT_SYMBOL(g_crypto_k_base);

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_crypto_k *ecnt_crypto_k = NULL;


static const struct of_device_id ecnt_crypto_k_of_id[] = {
    { .compatible = "econet,ecnt-crypto_k"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_crypto_k_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
#ifdef TCSUPPORT_DYING_GASP
extern void dying_gasp_handler_register(char *name, void *fn, void *context);
#endif

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_crypto_k_data(u32 reg)
{
    return readl(ecnt_crypto_k->base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_crypto_k_data(u32 reg, u32 val)
{
    writel(val, ecnt_crypto_k->base + reg); 
}

struct device* get_crypto_k_dev(void)
{
	if ((ecnt_crypto_k) && (ecnt_crypto_k->dev))
		return ecnt_crypto_k->dev;
	else
		return NULL;
}
EXPORT_SYMBOL(get_crypto_k_dev);

int get_crypto_k_irq(void)
{
	return ecnt_crypto_k->crypto_k_irq;
}
EXPORT_SYMBOL(get_crypto_k_irq);

static int ecnt_crypto_k_drv_shutdown(struct platform_device *pdev)
{
	unsigned int data = 0 ;
	data = get_crypto_k_data(CRYPTO_RAM_PD_CONTROL_REG);
	data |= CRYPTO_RAM_PD;
	set_crypto_k_data(CRYPTO_RAM_PD_CONTROL_REG, data);

    return 0;
}

static int ecnt_crypto_k_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
	int irq = 0;
	
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No crypto DT node found");
        return -EINVAL;
    }

    ecnt_crypto_k = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_crypto_k), GFP_KERNEL);
    if (!ecnt_crypto_k)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_crypto_k);

    /* get crypto_k base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_crypto_k->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_crypto_k->base))
        return PTR_ERR(ecnt_crypto_k->base);
    ecnt_crypto_k->dev = &pdev->dev;
    g_crypto_k_base = (unsigned long)ecnt_crypto_k->base;

	
	/* get irq num */
	irq = platform_get_irq(pdev, 0);
	if(irq <= 0) {
		printk("\n get irq number failed (Keng-Chih)\n");
		return irq;
	}
	ecnt_crypto_k->crypto_k_irq = irq;

#ifdef TCSUPPORT_DYING_GASP
	dying_gasp_handler_register("crypto", (void *)ecnt_crypto_k_drv_shutdown, 0);
#endif	
	
//#if 0
	printk("[crypto_k] res->name:%s\n", res->name);
	printk("[crypto_k] res->start:0x%llx ===\n", (unsigned long long)res->start);
	printk("[crypto_k] res->end:0x%llx ===\n", (unsigned long long)res->end);
	printk("[crypto_k] ecnt_crypto_k->base:0x%lx\n", (unsigned long)ecnt_crypto_k->base);
//#endif

    return 0;
}

static int ecnt_crypto_k_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_crypto_k_driver = {
    .probe = ecnt_crypto_k_drv_probe,
    .remove = ecnt_crypto_k_drv_remove,
    .driver = {
        .name = "ecnt-crypto_k",
        .of_match_table = ecnt_crypto_k_of_id
    },
};

module_platform_driver(ecnt_crypto_k_driver);
MODULE_DESCRIPTION("EcoNet crypto Driver");


