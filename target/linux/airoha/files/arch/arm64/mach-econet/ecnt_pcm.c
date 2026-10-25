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
#if defined(TCSUPPORT_CPU_ARMV8_64)
#include <linux/syscalls.h>
#endif
#include <asm/io.h>

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

struct ecnt_pcm_s {
    struct device *dev;
    void __iomem *base;
    int irq;
};


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_pcm_s *ecnt_pcm = NULL;


static const struct of_device_id ecnt_pcm_of_id[] = {
    { .compatible = "econet,ecnt-pcm"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_pcm_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
u32 GET_PCM_REG(u32 reg)
{
    return readl(ecnt_pcm->base + reg);
}
EXPORT_SYMBOL(GET_PCM_REG);
void SET_PCM_REG(u32 reg, u32 val)
{
    writel(val, ecnt_pcm->base + reg); 
}
EXPORT_SYMBOL(SET_PCM_REG);


int get_pcm_irq(void)
{
    return ecnt_pcm->irq; 
}
EXPORT_SYMBOL(get_pcm_irq);

struct device * get_pcm_dev(void)
{
    return ecnt_pcm->dev; 
}
EXPORT_SYMBOL(get_pcm_dev);



static int ecnt_pcm_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
    int irq = -1;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No pcm DT node found");
        return -EINVAL;
    }

    ecnt_pcm = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_pcm_s), GFP_KERNEL);
    if (!ecnt_pcm)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_pcm);

    /* get pcm base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_pcm->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_pcm->base))
        return PTR_ERR(ecnt_pcm->base);

    ecnt_pcm->dev = &pdev->dev;

    irq = platform_get_irq(pdev, 0);
    if (irq <= 0)
        return irq;
    ecnt_pcm->irq = irq;

#if 0
    printk("[pcm] res->name:%s\n", res->name);
    printk("[pcm] res->start:0x%llx ===\n", res->start);
    printk("[pcm] res->end:0x%llx ===\n", res->end);
    printk("[pcm] ecnt_pcm->base:0x%lx\n", (unsigned long)ecnt_pcm->base);
#endif

    return 0;
}

static int ecnt_pcm_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_pcm_driver = {
    .probe = ecnt_pcm_drv_probe,
    .remove = ecnt_pcm_drv_remove,
    .driver = {
        .name = "ecnt-pcm",
        .of_match_table = ecnt_pcm_of_id
    },
};
module_platform_driver(ecnt_pcm_driver);

int ecnt_kernel_delete_file(char * path){
    #if defined(TCSUPPORT_CPU_ARMV8_64)
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
    init_unlink(path);
    #else
    ksys_unlink(path);
    #endif
    #else
    sys_unlink(path);
    #endif
    return 0;
}
    
EXPORT_SYMBOL(ecnt_kernel_delete_file);

MODULE_DESCRIPTION("EcoNet pcm Driver");


