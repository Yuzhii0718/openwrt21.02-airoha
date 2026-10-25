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
#include <asm/barrier.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef TCSUPPORT_CPU_ARMV8
#define GDMA_BASE                   (0x00000000)
#else
#define GDMA_BASE                   (0xBFB30000)
#endif
    
#define CR_GDMA_SA0                 (GDMA_BASE+0x000)
#define CR_GDMA_DA0                 (GDMA_BASE+0x004)
#define CR_GDMA_CT00                (GDMA_BASE+0x008)
#define CR_GDMA_CT10                (GDMA_BASE+0x00C)
#define CR_GDMA_DONE                (GDMA_BASE+0x204)

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_gdma {
	struct device *dev;
	void __iomem *base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_gdma *ecnt_gdma = NULL;


static const struct of_device_id ecnt_gdma_of_id[] = {
    { .compatible = "econet,ecnt-gdma"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_gdma_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_gdma_data(u32 reg)
{
    return readl(ecnt_gdma->base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_gdma_data(u32 reg, u32 val)
{
    writel(val, ecnt_gdma->base + reg); 
}

struct device* get_gdma_dev(void)
{
    if ((ecnt_gdma) && (ecnt_gdma->dev))
        return ecnt_gdma->dev;
    else
        return NULL;
}

void __iomem * get_gdma_base(void)
{
    return ecnt_gdma->base;
}

u32 GET_GDMA_SA(u32 channel)
{
    return get_gdma_data(CR_GDMA_SA0+(channel<<4));
}
EXPORT_SYMBOL(GET_GDMA_SA);

u32 GET_GDMA_DA(u32 channel)
{
    return get_gdma_data(CR_GDMA_DA0+(channel<<4));
}
EXPORT_SYMBOL(GET_GDMA_DA);

u32 GET_GDMA_CT0(u32 channel)
{
    return get_gdma_data(CR_GDMA_CT00+(channel<<4));
}
EXPORT_SYMBOL(GET_GDMA_CT0);

u32 GET_GDMA_CT1(u32 channel)
{
    return get_gdma_data(CR_GDMA_CT10+(channel<<4));
}
EXPORT_SYMBOL(GET_GDMA_CT1);

u32 IS_GDMA_DONE(u32 channel)
{
    if((get_gdma_data(CR_GDMA_DONE) & (1<<channel))== 0)
        return 0;
    else
        return 1;
}
EXPORT_SYMBOL(IS_GDMA_DONE);

void CLEAR_GDMA_DONE(u32 channel)
{
    set_gdma_data(CR_GDMA_DONE, (1<<channel));
    return;
}
EXPORT_SYMBOL(CLEAR_GDMA_DONE);

void WAIT_GDMA_DONE(u32 channel)
{
    while( (get_gdma_data(CR_GDMA_CT00+(channel<<4)) &0x2) != 0);
    return;
}
EXPORT_SYMBOL(WAIT_GDMA_DONE);

void SET_GDMA_CONFIG(u32 channel, u32 sa, u32 da, u32 ct0, u32 ct1)
{
    set_gdma_data(CR_GDMA_SA0+(channel<<4), sa);
    set_gdma_data(CR_GDMA_DA0+(channel<<4), da);
    set_gdma_data(CR_GDMA_CT10+(channel<<4), ct1);
    set_gdma_data(CR_GDMA_CT00+(channel<<4), ct0);
    return;
}
EXPORT_SYMBOL(SET_GDMA_CONFIG);

void set_GDMA_enable_bit(u32 channel)
{
    u32 val;
    
    val = get_gdma_data(CR_GDMA_CT00+(channel<<4));
    val |= (1<<1);
    set_gdma_data(CR_GDMA_CT00+(channel<<4), val);
    return;
}
EXPORT_SYMBOL(set_GDMA_enable_bit);


void SET_GDMA_CT0(u32 channel, u32 ct0)
{
    set_gdma_data(CR_GDMA_CT00+(channel<<4), ct0);
    return;
}
EXPORT_SYMBOL(SET_GDMA_CT0);


static int ecnt_gdma_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No gdma DT node found");
        return -EINVAL;
    }

    ecnt_gdma = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_gdma), GFP_KERNEL);
    if (!ecnt_gdma)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_gdma);

    /* get GDMA base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_gdma->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gdma->base))
        return PTR_ERR(ecnt_gdma->base);

    ecnt_gdma->dev = &pdev->dev;

#if 0
	printk("[gdma] res->name:%s\n", res->name);
	printk("[gdma] res->start:0x%llx ===\n", res->start);
	printk("[gdma] res->end:0x%llx ===\n", res->end);
	printk("[gdma] ecnt_gdma->base:0x%lx\n", (unsigned long)ecnt_gdma->base);
#endif

    return 0;
}

static int ecnt_gdma_drv_remove(struct platform_device *pdev)
{
    return 0;
}


/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_gdma_driver = {
    .probe = ecnt_gdma_drv_probe,
    .remove = ecnt_gdma_drv_remove,
    .driver = {
        .name = "ecnt-gdma",
        .of_match_table = ecnt_gdma_of_id
    },
};
module_platform_driver(ecnt_gdma_driver);


MODULE_DESCRIPTION("EcoNet GDMA Driver");


