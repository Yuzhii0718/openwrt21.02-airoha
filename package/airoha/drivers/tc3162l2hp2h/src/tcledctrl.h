/*********************************************************************************
 * LEDs and button driver for GPIOs
 *
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 *
 *********************************************************************************/

#ifndef _TCLED_CTRL_H
#define _TCLED_CTRL_H

#include <linux/leds.h>
#define LED_NAME_LEN			32
struct econet_led {
	uint8 gpio;
	uint8 mode;
	uint8 bicolor;
	uint8 active_low;
	char name[LED_NAME_LEN];
	uint16 speed;
	struct led_classdev cdev;
};

#define WLAN_GPIO_START			0
#if defined(MT7592)
#define WLAN_GPIO_END			15
#else
#define WLAN_GPIO_END			14
#endif
#define WLAN_LED_OFF			0
#define WLAN_LED_ON				1
#define WLAN_LED_OEN			2
#define WLAN_LED_IEN			3
#define WLAN_LED_RECOVER		4
#define WLAN_GPIO_MAP_OFFSET	64
#define WLAN_GPIO_MODE_DATE		(1<<30)

#define GPIO_REG_CTRL_NUM		15
#define GPIO_REG_CTRL1_NUM		31
#define GPIO_REG_CTRL2_NUM		47
#define GPIO_REG_CTRL3_NUM		63
#define GPIO_REG_DATA_NUM		31
#define GPIO_REG_DATA1_NUM		63

#ifndef TCSUPPORT_CPU_ARMV8 /* move to arch/arm/include/asm/tc3162/ledcetrl.h for ARM */
#define GPIO_REG_BASE			0xBFBF0200
#define GPIO_REG_CTRL			(GPIO_REG_BASE + 0x00)
#define GPIO_REG_DATA			(GPIO_REG_BASE + 0x04)
#define GPIO_REG_OEN			(GPIO_REG_BASE + 0x14)
#define GPIO_REG_CTRL1			(GPIO_REG_BASE + 0x20)
#define GPIO_REG_CTRL2			(GPIO_REG_BASE + 0x60)
#define GPIO_REG_CTRL3			(GPIO_REG_BASE + 0x64)
#define GPIO_REG_DATA1			(GPIO_REG_BASE + 0x70)
#define GPIO_REG_OEN1			(GPIO_REG_BASE + 0x78)
#endif

#define DSL_DOWN	0
#define DSL_UP		1
#define DSL_SYNCING	2

#endif
