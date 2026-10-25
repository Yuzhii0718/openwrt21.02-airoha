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

#include <linux/io.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include <linux/gpio/consumer.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
/* GPIO */
#define GPIO_BASE 0x00000000
#define GPIO_CTRL (GPIO_BASE + 0x00)
#define GPIO_DATA (GPIO_BASE + 0x04)
#define GPIO_OE (GPIO_BASE + 0x14)
#define GPIO_LEDCTRL (GPIO_BASE + 0x1C)
#define GPIO_CTRL1 (GPIO_BASE + 0x20)
#define GPIO_FLASH_MODE_CFG (GPIO_BASE + 0x34)
#define GPIO_RSP_MODE_CFG (GPIO_BASE + 0x38)
#define GPIO_FLASH_PRD_SET0 (GPIO_BASE + 0x3C)
#define GPIO_FLASH_PRD_SET1 (GPIO_BASE + 0x40)
#define GPIO_FLASH_PRD_SET2 (GPIO_BASE + 0x44)
#define GPIO_FLASH_PRD_SET3 (GPIO_BASE + 0x48)
#define GPIO_FLASH_MAP_CFG0 (GPIO_BASE + 0x4C)
#define GPIO_FLASH_MAP_CFG1 (GPIO_BASE + 0x50)
#define GPIO_CTRL2 (GPIO_BASE + 0x60)
#define GPIO_CTRL3 (GPIO_BASE + 0x64)
#define GPIO_FLASH_MODE_CFG_EXT (GPIO_BASE + 0x68)
#define GPIO_OE1 (GPIO_BASE + 0x78)
#define CYCLE_CFG_VALUE0 (GPIO_BASE + 0x98)
#define CYCLE_CFG_VALUE1 (GPIO_BASE + 0x9C)
#define AUTO_FLASH_STEP1_0 (GPIO_BASE + 0xA0)
#define AUTO_FLASH_STEP3_2 (GPIO_BASE + 0xA4)
#define AUTO_FLASH_STEP5_4 (GPIO_BASE + 0xA8)
#define AUTO_FLASH_STEP7_6 (GPIO_BASE + 0xAC)
#define AUTO_FLASH_AMP3_0 (GPIO_BASE + 0xB0)
#define AUTO_FLASH_AMP7_4 (GPIO_BASE + 0xB4)
#define AUTO_FLASH_EN (GPIO_BASE + 0xB8)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/* used in ledcetl.h */
unsigned long g_gpio_base = 0;
EXPORT_SYMBOL(g_gpio_base);

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/

struct airoha_gpio_ctrl {
	struct gpio_chip gc;
	void __iomem *data;
	void __iomem *dir[2];
	void __iomem *output;
};

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

static unsigned long get_gpio_base(void)
{
	unsigned int gpio = 0;
	struct airoha_gpio_ctrl *ctrl = NULL;
	void __iomem *base = 0;
	
	for(; gpio<32; gpio++) {
		struct gpio_desc *desc = gpio_to_desc(gpio);
		struct gpio_chip *chip = gpiod_to_chip(desc);
		if (chip != NULL) {
			ctrl = gpiochip_get_data(chip);
			if (ctrl != NULL) {
				base = ctrl->dir[0];
				printk("%s: gpio_pin: %d, gpio_chip : %px, ctrl: %px, gpio_desc: %px, base: %px\n", 
	                  __FUNCTION__, gpio, chip, ctrl, desc, base);
			}
			break;
		}

		else
			continue;
	}
	
	if (gpio == 32) {
		printk(KERN_INFO "no gpio is defined in device tree!");
		return 0;

	}

	return base;
	
}

/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_gpio_data(u32 reg)
{
	if (g_gpio_base == 0) {
		g_gpio_base = get_gpio_base();

		if (g_gpio_base == 0)
			return 0;
	}
	
    return readl(g_gpio_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_gpio_data(u32 reg, u32 val)
{
	if (g_gpio_base == 0) {
		g_gpio_base = get_gpio_base();

		if (g_gpio_base == 0)
			return;
	}

    writel(val, g_gpio_base + reg); 
}

u32 GET_GPIO_CTRL(void)
{
    return get_gpio_data(GPIO_CTRL);
}
EXPORT_SYMBOL(GET_GPIO_CTRL);

void SET_GPIO_CTRL(u32 data)
{
    set_gpio_data(GPIO_CTRL, data);
}
EXPORT_SYMBOL(SET_GPIO_CTRL);

u32 GET_GPIO_DATA(void)
{
    return get_gpio_data(GPIO_DATA);
}
EXPORT_SYMBOL(GET_GPIO_DATA);

void SET_GPIO_DATA(u32 data)
{
    set_gpio_data(GPIO_DATA, data);
}
EXPORT_SYMBOL(SET_GPIO_DATA);

u32 GET_GPIO_OE(void)
{
    return get_gpio_data(GPIO_OE);
}
EXPORT_SYMBOL(GET_GPIO_OE);

void SET_GPIO_OE(u32 data)
{
    set_gpio_data(GPIO_OE, data);
}
EXPORT_SYMBOL(SET_GPIO_OE);

u32 GET_GPIO_CTRL1(void)
{
    return get_gpio_data(GPIO_CTRL1);
}
EXPORT_SYMBOL(GET_GPIO_CTRL1);

void SET_GPIO_CTRL1(u32 data)
{
    set_gpio_data(GPIO_CTRL1, data);
}
EXPORT_SYMBOL(SET_GPIO_CTRL1);

u32 GET_GPIO_LEDCTRL(void)
{
    return get_gpio_data(GPIO_LEDCTRL);
}
EXPORT_SYMBOL(GET_GPIO_LEDCTRL);

void SET_GPIO_LEDCTRL(u32 data)
{
    set_gpio_data(GPIO_LEDCTRL, data);
}
EXPORT_SYMBOL(SET_GPIO_LEDCTRL);

u32 GPIOPWM_GET_GPIO_FLASH_MODE_CFG(u8 gpioPinNum)
{
    return get_gpio_data(GPIO_FLASH_MODE_CFG);
}
EXPORT_SYMBOL(GPIOPWM_GET_GPIO_FLASH_MODE_CFG);

static void GPIOPWM_SET_GPIO_FLASH_MODE_CFG(u8 gpioPinNum, u32 data)
{
    set_gpio_data(GPIO_FLASH_MODE_CFG, data);
}
EXPORT_SYMBOL(GPIOPWM_SET_GPIO_FLASH_MODE_CFG);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Legacy GPIO driver");