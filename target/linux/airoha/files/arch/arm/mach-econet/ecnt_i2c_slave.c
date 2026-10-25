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

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <asm/tc3162/tc3162.h>
#include <linux/irqreturn.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define I2C_SLAVE_BASE					(0x000000)  /*phys:0x1fbe3300*/

#define I2CSLV_CTRL						(I2C_SLAVE_BASE+0x00)
#define I2CSLV_INTST					(I2C_SLAVE_BASE+0x04)
#define I2CSLV_INTEN			    	(I2C_SLAVE_BASE+0x08)
#define I2CSLV_STS						(I2C_SLAVE_BASE+0x0C)

#define I2C_DEV_ADDR_MASK				(0x7F)
#define I2C_DEV_ADDR_OFFSET				(8)

#define I2C_DEV_AMOUNT					(2)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_i2c_slave {
	struct device *dev;
	void __iomem *base;
	u32 irq;

};


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_i2c_slave *ecnt_i2c_slave = NULL;
static u8 dev_addr[I2C_DEV_AMOUNT] = {0};

static const struct of_device_id ecnt_i2c_slave_of_id[] = {
    { .compatible = "econet,ecnt-i2c_slave"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_i2c_slave_of_id);

void (*i2c_slave_isr_func)(u32 intr_stat);


/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
extern void scu_Enable_Module_Clock (u8 module);

/* don't EXPORT this function. Create API for your purpose instead. */
static u32 get_i2c_slave_data(u32 reg)
{
    return readl(ecnt_i2c_slave->base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_i2c_slave_data(u32 reg, u32 val)
{
    writel(val, ecnt_i2c_slave->base + reg); 
}

void set_i2c_slave_isr(void *func)
{
	i2c_slave_isr_func = func;
}
EXPORT_SYMBOL(set_i2c_slave_isr);

u8 get_i2c_slave_device_addr(int device)
{
	u32 reg = 0;
	u8 addr = 0;

	reg = get_i2c_slave_data(I2CSLV_CTRL);
	addr = (reg >> (I2C_DEV_ADDR_OFFSET * device)) & I2C_DEV_ADDR_MASK;
	return addr;
}

EXPORT_SYMBOL(get_i2c_slave_device_addr);

void set_i2c_slave_device_addr(int device, u8 addr)
{
	u32 reg = 0;
	u32 offset = 0;
	u32 mask = 0;
	u32 data = addr;
		
	reg = get_i2c_slave_data(I2CSLV_CTRL);
	offset = (device * I2C_DEV_ADDR_OFFSET);
	mask = (I2C_DEV_ADDR_MASK << offset);
	reg &= ~(mask);
	reg |= (data << offset);

	set_i2c_slave_data(I2CSLV_CTRL, reg);
	
}

EXPORT_SYMBOL(set_i2c_slave_device_addr);

void hw_init_i2c_slave_module(void)
{
	u8 i;

	/* Disable i2c_slave module in boot loader. Need to enable i2c_slave at insert module. */
	scu_Enable_Module_Clock(2);

	for ( i=0 ; i<I2C_DEV_AMOUNT ; i++ )
	{
		set_i2c_slave_device_addr(i, dev_addr[i]);
	}
}

EXPORT_SYMBOL(hw_init_i2c_slave_module);

static irqreturn_t i2c_slave_interrupt(int irq, void *dev_id)
{
	u32 irq_status;
	
	irq_status = get_i2c_slave_data(I2CSLV_INTST);

	if (i2c_slave_isr_func!=NULL)
	{
		i2c_slave_isr_func(irq_status);
	}

	set_i2c_slave_data(I2CSLV_INTST, irq_status);
	
	return IRQ_HANDLED;
}


static int ecnt_i2c_slave_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
	int ret = 0;
	
	u32 dev0_addr = 0;
	u32 dev1_addr = 0;
	
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No i2c_slave DT node found\n");
        return -EINVAL;
    }

    ecnt_i2c_slave = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_i2c_slave), GFP_KERNEL);
    if (!ecnt_i2c_slave)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_i2c_slave);

    /* get i2c_slave base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_i2c_slave->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_i2c_slave->base))
        return PTR_ERR(ecnt_i2c_slave->base);

    ecnt_i2c_slave->dev = &pdev->dev;
	
	res = platform_get_resource(pdev,IORESOURCE_IRQ, 0);
	if (!res)
	{	
	 	dev_err(&(pdev->dev), "No irq resource for %s\n", dev_name(&(pdev->dev)));
	 	return -EINVAL; 
	}

	ecnt_i2c_slave->irq = res->start;
	ret = devm_request_irq(&(pdev->dev), ecnt_i2c_slave->irq, i2c_slave_interrupt, 0, dev_name(&(pdev->dev)), ecnt_i2c_slave);
	
	if (ret)
	{
		dev_err(&(pdev->dev), "request_irq failed with err %d\n", ret);
		return -EINVAL;
	}
	if (pdev->dev.of_node && of_property_read_u32(pdev->dev.of_node, "dev0_addr", &(dev0_addr)))
	{
		dev_info(&(pdev->dev), "Missing dev0_addr property in %s, use 0x60 as its addr\n", dev_name(&(pdev->dev)));
		dev0_addr = 0x60;
	}
	if (pdev->dev.of_node && of_property_read_u32(pdev->dev.of_node, "dev1_addr", &(dev1_addr)))
	{
		dev_info(&(pdev->dev), "Missing dev1_addr property in %s use 0x62 as its addr\n", dev_name(&(pdev->dev)));
		dev1_addr = 0x62;
	}

	/* Modify device addr during the default device addr (0x50) is used by pon phy*/
	dev_addr[0] = dev0_addr;
	dev_addr[1] = dev1_addr;

    printk("\ni2c_slave init(%s) done\n", __func__);   
	

    return 0;
}

static int ecnt_i2c_slave_drv_remove(struct platform_device *pdev)
{
    return 0;
}


/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_i2c_slave_driver = {
    .probe = ecnt_i2c_slave_drv_probe,
    .remove = ecnt_i2c_slave_drv_remove,
    .driver = {
        .name = "ecnt-i2c_slave",
        .of_match_table = ecnt_i2c_slave_of_id
    },
};
module_platform_driver(ecnt_i2c_slave_driver);


MODULE_DESCRIPTION("EcoNet i2c_slave Driver");


