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
#include <asm/uaccess.h>

#include <asm/io.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


#define GDUMP_DATA_SEL			(0x74)
#define GDUMP_DATA_SEL_MASK		(0x3)

#if defined(TCSUPPORT_CPU_AN7552)
#define PCIE_HSGMII1_SEL_BIT	(1<<30)
#endif

#define GDMP_REG_BASE	0x00000000
#define GDMP_GLO_CFG	(GDMP_REG_BASE + 0x00000000)
#define GDMP_CAP_RLT	(GDMP_REG_BASE + 0x00000004)
#define GDMP_TRG_RLT	(GDMP_REG_BASE + 0x00000008)
#define GDMP_INT_STS	(GDMP_REG_BASE + 0x00000010)
#define GDMP_INT_MSK	(GDMP_REG_BASE + 0x00000014)
#define GDMP_PRB_SEL	(GDMP_REG_BASE + 0x00000020)
#define	GDMP_TRG_PATN0_L		(GDMP_REG_BASE + 0x00000030)
#define	GDMP_TRG_PATN0_MSK_L	(GDMP_REG_BASE + 0x00000034)
#define	GDMP_TRG_PATN0_H		(GDMP_REG_BASE + 0x00000038)
#define	GDMP_TRG_PATN0_MSK_H	(GDMP_REG_BASE + 0x0000003C)
#define	GDMP_TRG_PATN1_L		(GDMP_REG_BASE + 0x00000040)
#define	GDMP_TRG_PATN1_MSK_L	(GDMP_REG_BASE + 0x00000044)
#define	GDMP_TRG_PATN1_H		(GDMP_REG_BASE + 0x00000048)
#define	GDMP_TRG_PATN1_MSK_H	(GDMP_REG_BASE + 0x0000004C)
#define	GDMP_TRG_MODE			(GDMP_REG_BASE + 0x00000050)
#define	GDMP_CAP_CFG			(GDMP_REG_BASE + 0x00000060)
#define	GDMP_CLK_CFG			(GDMP_REG_BASE + 0x00000070)
#define GDMP_MASK_CFG1          (GDMP_REG_BASE + 0x00000080)

#define GDMP_LMEM_PD		(1<<8)
#define GDMP_DMEM_PD		(1<<9)
#define GDMPDEBUGP(f, a...)	if (gdumpdebug) printk(f, ## a )

enum gdump_sel
{
	PBUS_ACCESS_DATA,
	PBUS_ACCESS_LENGTH,
	GDUMP_ACCESS_DATA,
	FE_ACCESS_DATA,
	MAX_GDUMP_SEL_NUM
};

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_gdump {
	struct device *gdump_dev;
	void __iomem *gdump_base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_gdump *ecnt_gdump = NULL;

static int gdumpdebug = 0 ;

static const struct of_device_id ecnt_gdump_of_id[] = {
    { .compatible = "econet,ecnt-gdump"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_gdump_of_id);


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
static u32 get_gdump_data(u32 reg)
{
    return readl(ecnt_gdump->gdump_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_gdump_data(u32 reg, u32 val)
{
    writel(val, ecnt_gdump->gdump_base + reg); 
}

int gdump_regs_test(u32 cnt)
{
    set_gdump_data(GDMP_TRG_PATN0_L, cnt);
    set_gdump_data(GDMP_MASK_CFG1, cnt+1);
    if ((get_gdump_data(GDMP_TRG_PATN0_L)!=cnt) || (get_gdump_data(GDMP_MASK_CFG1)!=(cnt+1))) {
        printk("Error: (GDMP_TRG_PATN0_L:0x%x != 0x%x) or (GDMP_MASK_CFG1:0x%x != 0x%x)\n",
                get_gdump_data(GDMP_TRG_PATN0_L), cnt, get_gdump_data(GDMP_MASK_CFG1), cnt+1);
        return -1;
    }
    return 0;
}
EXPORT_SYMBOL(gdump_regs_test);

u32 gdump_get_mode_sel(void)
{
	return get_gdump_data(GDUMP_DATA_SEL);
}
EXPORT_SYMBOL(gdump_get_mode_sel);

void gdump_mode_sel(u32 val)
{
	u32 reg = 0;
	if (val >= MAX_GDUMP_SEL_NUM)
	{
		printk("(%s): ERROR: invalid usage!!\r\n",__func__);
	}
	else
	{
		reg = gdump_get_mode_sel();
		reg &= ~(GDUMP_DATA_SEL_MASK);
		reg |= val;
		set_gdump_data(GDUMP_DATA_SEL, reg);
		//printk("(%s): GDUMP sel has switch to %x\r\n",__func__,val);
#if defined(TCSUPPORT_CPU_AN7552)
		/* Need to modify clock setting when FE use gdump SRAM*/
		reg = GET_SSR3();
		if (val == FE_ACCESS_DATA)
		{
			reg &= ~(PCIE_HSGMII1_SEL_BIT);
		}
		else
		{
			reg |= (PCIE_HSGMII1_SEL_BIT);
		}
		SET_SSR3 (reg);
#endif
	}
}
EXPORT_SYMBOL(gdump_mode_sel);

int gdump_cfg_dump(unsigned char *buf,int debug)
{
	int len = 0;

	gdumpdebug = debug;

	GDMPDEBUGP("GDMP_GLO_CFG\t%08x\r\n",get_gdump_data(GDMP_GLO_CFG));	
	len += sprintf(buf + len,"GDMP_GLO_CFG\t%08x\r\n",get_gdump_data(GDMP_GLO_CFG));
	
	GDMPDEBUGP("GDMP_CAP_RLT\t%08x\r\n",get_gdump_data(GDMP_CAP_RLT));
	len += sprintf(buf + len,"GDMP_CAP_RLT\t%08x\r\n",get_gdump_data(GDMP_CAP_RLT));
	
	GDMPDEBUGP("GDMP_TRG_RLT\t%08x\r\n",get_gdump_data(GDMP_TRG_RLT));	
	len += sprintf(buf + len,"GDMP_TRG_RLT\t%08x\r\n",get_gdump_data(GDMP_TRG_RLT));
	
	GDMPDEBUGP("GDMP_INT_STS\t%08x\r\n",get_gdump_data(GDMP_INT_STS));
	len += sprintf(buf + len,"GDMP_INT_STS\t%08x\r\n",get_gdump_data(GDMP_INT_STS));
	
	GDMPDEBUGP("GDMP_INT_MSK\t%08x\r\n",get_gdump_data(GDMP_INT_MSK));	
	len += sprintf(buf + len,"GDMP_INT_MSK\t%08x\r\n",get_gdump_data(GDMP_INT_MSK));
	
	GDMPDEBUGP("GDMP_PRB_SEL\t%08x\r\n",get_gdump_data(GDMP_PRB_SEL));
	len += sprintf(buf + len,"GDMP_PRB_SEL\t%08x\r\n",get_gdump_data(GDMP_PRB_SEL));

	GDMPDEBUGP("GDMP_TRG_PATN0_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_L));	
	len += sprintf(buf + len,"GDMP_TRG_PATN0_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_L));
	
	GDMPDEBUGP("GDMP_TRG_PATN0_MSK_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_MSK_L));
	len +=sprintf(buf + len,"GDMP_TRG_PATN0_MSK_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_MSK_L));

	GDMPDEBUGP("GDMP_TRG_PATN0_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_H));	
	len +=sprintf(buf + len,"GDMP_TRG_PATN0_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_H));
	
	GDMPDEBUGP("GDMP_TRG_PATN0_MSK_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_MSK_H));
	len +=sprintf(buf + len,"GDMP_TRG_PATN0_MSK_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN0_MSK_H));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_L));	
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_L));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_MSK_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_MSK_L));
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_MSK_L\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_MSK_L));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_H));	
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_H));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_MSK_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_MSK_H));
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_MSK_H\t%08x\r\n",get_gdump_data(GDMP_TRG_PATN1_MSK_H));
		
	GDMPDEBUGP("GDMP_TRG_MODE\t%08x\r\n",get_gdump_data(GDMP_TRG_MODE));	
	len +=sprintf(buf + len,"GDMP_TRG_MODE\t%08x\r\n",get_gdump_data(GDMP_TRG_MODE));
	
	GDMPDEBUGP("GDMP_CAP_CFG\t%08x\r\n",get_gdump_data(GDMP_CAP_CFG));
	len +=sprintf(buf + len,"GDMP_CAP_CFG\t%08x\r\n",get_gdump_data(GDMP_CAP_CFG));
	
	GDMPDEBUGP("GDMP_CLK_CFG\t%08x\r\n",get_gdump_data(GDMP_CLK_CFG));
	len +=sprintf(buf + len,"GDMP_CLK_CFG\t%08x\r\n",get_gdump_data(GDMP_CLK_CFG)); 
	
	return len;
}
EXPORT_SYMBOL(gdump_cfg_dump);

static int ecnt_gdump_drv_shutdown(struct platform_device *pdev)
{
	unsigned int data = 0 ;
	/* power down GDUMP SRAM*/
	data = get_gdump_data(GDMP_GLO_CFG);
	data |= (GDMP_LMEM_PD | GDMP_DMEM_PD);
	set_gdump_data(GDMP_GLO_CFG, data);	

    return 0;
}

static int ecnt_gdump_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No gdump DT node found");
        return -EINVAL;
    }

    ecnt_gdump = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_gdump), GFP_KERNEL);
    if (!ecnt_gdump)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_gdump);

    /* get GDUMP base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_gdump->gdump_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gdump->gdump_base))
        return PTR_ERR(ecnt_gdump->gdump_base);

    ecnt_gdump->gdump_dev = &pdev->dev;
	
#ifdef TCSUPPORT_DYING_GASP
	dying_gasp_handler_register("gdump", (void *)ecnt_gdump_drv_shutdown, 0);
#endif

#if 0
	printk("[gdump] res->name:%s\n", res->name);
	printk("[gdump] res->start:0x%llx ===\n", res->start);
	printk("[gdump] res->end:0x%llx ===\n", res->end);
	printk("[sram] ecnt_gdump->gdump_base:0x%lx\n", (unsigned long)ecnt_gdump->gdump_base);
#endif

    return 0;
}

static int ecnt_gdump_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_gdump_driver = {
    .probe = ecnt_gdump_drv_probe,
    .remove = ecnt_gdump_drv_remove,
    .driver = {
        .name = "ecnt-gdump",
        .of_match_table = ecnt_gdump_of_id
    },
};
module_platform_driver(ecnt_gdump_driver);


MODULE_DESCRIPTION("EcoNet GDUMP Driver");

