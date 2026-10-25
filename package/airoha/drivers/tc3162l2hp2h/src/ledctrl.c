/*********************************************************************************
 * LEDs and button driver for GPIOs
 *
 * Copyright (C) 2006 Trendchip Technologies, Corp.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *********************************************************************************/
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/fcntl.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif
#include <linux/platform_device.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include <linux/input.h>
#include <linux/of_gpio.h>
#include <linux/gpio_keys.h>
#include <linux/gpio/driver.h>
#include "linux/init.h"
#include "linux/gpio.h"
#include "tcledctrl.h"

/*********************************************************************************
 * micro definition setction
 *
 *********************************************************************************/
#ifdef TCSUPPORT_CPU_ARMV8
static inline uint32 regRead32(unsigned long reg)		\
{						  	\
	return readl(reg);			  	\
}		

static inline void regWrite32(unsigned long reg, uint32 vlaue)	\
{                           \
	writel(vlaue, reg);                      	\
}
#endif


#define NO_USE_UPSTREAM 0

#define LED_MAX_NO			107
#define BUTTON_MAX_NO			10
#define LED_NAME_LEN			32
#define LED_MODE_MASK			0xf

#define LED_MODE(x)			((x) & LED_MODE_MASK)
#define LED_BICOLOR(x)			((x) & LED_MODE_BICOLOR)
#define LED_GPIO1(x)			(((x) & 0xff00) >>8)
#define LED_GPIO2(x)			((x) & 0x00ff)

#define LED_GPIO1_NEW(x)	(((x) & 0xff00) >> 8)
#define LED_GPIO2_NEW(x)	((x) & 0xff)
#define LED_GET_GPIO_CTRL(x) 	((x>31)?(regRead32(CR_GPIO_CTRL1)&(1<<(x-32))):(regRead32(CR_GPIO_CTRL)&(1<<x)))
#define LED_GET_GPIO_DATA(x) 	((x>31)?(regRead32(CR_GPIO_DATA1)&(1<<(x-32))):(regRead32(CR_GPIO_DATA)&(1<<x)))
#define REG_SETBITS(reg, bitsmask, bits) (regWrite32(reg, (regRead32(reg) & ~(bitsmask)) | (bits)))
#define REG_CLRBITS(reg, bits) (regWrite32(reg, regRead32(reg) & ~(bits)))

#define GPIO_LED_OFF			0
#define GPIO_LED_ON			1
#define GPIO_LED_BLINK			2

#define	DEVICENAME			"econet:led"
#define	LED_CONFIG_PATH			"/userfs/led.conf"
#define CDEV_TO_LED(led_cdev)	container_of(led_cdev,struct econet_led,cdev)


#define ECNT_KEYS_POLL_INTERVAL	20	/* msecs */
#define ECNT_KEYS_DEBOUNCE_INTERVAL	(3 * ECNT_KEYS_POLL_INTERVAL)

#define DO_LED_OFF(x, b) do {								\
	if(!(led_wifi(WLAN_LED_OFF, x, b))){				\
		if (b) {						\
			if(LED_GPIO_MODE(x)){	\
				uint32 value1 = regRead32(CR_GPIO_DATA);	\
				uint32 value2 = regRead32(CR_GPIO_DATA1);	\
				uint32 flag = 0;	\
				if(LED_GPIO1_NEW(x) > 31){\
					value2 |= (1<<(LED_GPIO1_NEW(x)-32));	\
				}else{	\
					value1 |= (1<<(LED_GPIO1_NEW(x)));	\
				}	\
				if(LED_GPIO2_NEW(x) > 31){\
					value2 |= (1<<(LED_GPIO2_NEW(x)-32));	\
					flag = 1;\
				}else{	\
					value1 |= (1<<(LED_GPIO2_NEW(x)));	\
				}	\
				if(flag){	\
					regWrite32(CR_GPIO_DATA1,value2);				\
					regWrite32(CR_GPIO_DATA,value1);				\
				}else{	\
					regWrite32(CR_GPIO_DATA,value1);				\
					regWrite32(CR_GPIO_DATA1,value2);				\
				}	\
			}else{	\
				regWrite32(CR_GPIO_DATA,regRead32(CR_GPIO_DATA)|(1<<(LED_GPIO1(x))));	\
				regWrite32(CR_GPIO_DATA,regRead32(CR_GPIO_DATA)|(1<<(LED_GPIO2(x))));	\
			}	\
		} else {						\
			if(x > 31){					\
				regWrite32(CR_GPIO_DATA1,regRead32(CR_GPIO_DATA1)|(1<<(x-32)));	\
			}else{						\
				regWrite32(CR_GPIO_DATA,regRead32(CR_GPIO_DATA)|(1<<x));	\
			}						\
		}							\
	}								\
} while (0)

#define DO_LED_ON(x, b) do {								\
	if(!(led_wifi(WLAN_LED_ON, x, b))){	\
		if (b) {						\
			if(LED_GPIO_MODE(x)){	\
				uint32 value1 = regRead32(CR_GPIO_DATA);	\
				uint32 value2 = regRead32(CR_GPIO_DATA1);	\
				int flag = 0;\
				if(LED_GPIO1_NEW(x) > 31){\
					value2 &= ~ (1<<(LED_GPIO1_NEW(x)-32));	\
				}else{	\
					value1 &= ~ (1<<(LED_GPIO1_NEW(x)));	\
				}	\
				if(LED_GPIO2_NEW(x) > 31){\
					value2 |= (1<<(LED_GPIO2_NEW(x)-32));	\
					flag = 1;\
				}else{	\
					value1 |= (1<<(LED_GPIO2_NEW(x)));	\
				}	\
				if(flag){	\
					regWrite32(CR_GPIO_DATA1,value2);				\
					regWrite32(CR_GPIO_DATA,value1);				\
				}else{\
					regWrite32(CR_GPIO_DATA,value1);				\
					regWrite32(CR_GPIO_DATA1,value2);				\
				}\
			}else{						\
				regWrite32(CR_GPIO_DATA,regRead32(CR_GPIO_DATA)& ~(1<<(LED_GPIO1(x))));	\
				regWrite32(CR_GPIO_DATA,regRead32(CR_GPIO_DATA)|(1<<(LED_GPIO2(x))));	\
			}	\
		} else {						\
			if(x > 31){					\
				regWrite32(CR_GPIO_DATA1,regRead32(CR_GPIO_DATA1)& ~(1<<(x-32)));	\
			}else{						\
				regWrite32(CR_GPIO_DATA,regRead32(CR_GPIO_DATA)& ~(1<<x));	\
			}						\
		} 							\
	}								\
} while (0)

#define LED_OEN(x)		do { 	if(!(led_wifi(WLAN_LED_OEN, x, 0))){	\
																		if(x > 31){ 						\
																			if(x > 47){ 					\
																				REG_SETBITS(CR_GPIO_CTRL3, 0x3<<((x-48)*2), 1<<((x-48)*2)); \
																			}else{							\
																				REG_SETBITS(CR_GPIO_CTRL2, 0x3<<((x-32)*2), 1<<((x-32)*2)); \
																			}								\
																			REG_SETBITS(CR_GPIO_ODRAIN1, 0x1<<(x-32), 1<<(x-32)); \
																		}									\
																		else{								\
																			if(x > 15){ 					\
																				REG_SETBITS(CR_GPIO_CTRL1,0x3<<((x-16)*2), 1<<((x-16)*2)); \
																			}else{							\
																				REG_SETBITS(CR_GPIO_CTRL,0x3<<((x)*2), 1<<((x)*2)); \
																			}								\
																			REG_SETBITS(CR_GPIO_ODRAIN, 0x1<<(x), 1<<(x)); \
																		}									\
																	}\
													} while(0)
									/* input enable */
#define LED_IEN(x)		do { 	if(!(led_wifi(WLAN_LED_IEN, x, 0))){	\
																		if(x > 31){ 						\
																			if(x > 47)						\
																				REG_CLRBITS(CR_GPIO_CTRL3, (0x00000003 << ((x-48)* 2)));	\
																			else							\
																				REG_CLRBITS(CR_GPIO_CTRL2, (0x00000003 << ((x-32)* 2))); \
																			REG_CLRBITS(CR_GPIO_ODRAIN1, (0x00000001 << (x-32))); \
																		}									\
																		else{								\
																			if(x > 15)						\
																				REG_CLRBITS(CR_GPIO_CTRL1,(0x00000003 << ((x-16)* 2))); \
																			else							\
																				REG_CLRBITS(CR_GPIO_CTRL,(0x00000003 << (x* 2)));	\
																			REG_CLRBITS(CR_GPIO_ODRAIN,(0x00000001 << (x))); \
																		}									\
																	}\
													} while(0)
/*********************************************************************************
 * type definition setction
 *
 *********************************************************************************/
typedef struct {
	uint8 led_no;
	uint8 gpio;
	uint8 mode;
	uint16 speed;
	uint8 onoff;
}ledctrl_t;

struct led_name_t {
	char *name;
	uint8 led_no;
};
#if NO_USE_UPSTREAM
struct btn_info_t {
	uint8 led_no;
	char *name;
	int code;
	uint8 gpio;
	uint8 active;
};

struct econet_led_priv {
	int num_leds;
	struct econet_led leds[];
};
#endif
#define isDEFAULT         0x0
typedef struct led_path
{
    uint8 chipid ;
    const char *path;
}led_path_m;

#if NO_USE_UPSTREAM
#ifdef TCSUPPORT_USB_HOST_LED
int usb_dev_status = 0 ;
unsigned int usb_phyport_status = 0 ;

extern void (*Usb_Led_Flash_Op_hook)(unsigned int opmode ,unsigned int phyport);
#if KERNEL_3_18_21
extern int pre_usb_state[2];
#endif
#endif
#endif

#ifdef TR068_LED
int internet_led_on = 0;	/* 1:IP Connected; 0:No IP Connected*/
int internet_trying_led_on = 0;  /* 1:adsl up and trying to get WAN IP; 0:adsl not up or can't found defaule router*/

EXPORT_SYMBOL(internet_led_on);
EXPORT_SYMBOL(internet_trying_led_on);
#endif


#if defined(TCSUPPORT_WLAN_GPIO)
#if 0
int (*hook_wlan_led_action)(int action, int gpio);
EXPORT_SYMBOL(hook_wlan_led_action);
#else
extern int (*hook_wlan_led_action)(int action, int gpio);
#endif
#endif

unsigned int (*hook_wlan_led_get_data)(int gpio);
EXPORT_SYMBOL(hook_wlan_led_get_data);

#if defined(TCSUPPORT_WLAN_GPIO) || (defined(TCSUPPORT_LED_SWITCH_BUTTON)&&defined(TCSUPPORT_WLAN))
void set_wlan_gpio_mode(void);
EXPORT_SYMBOL(set_wlan_gpio_mode);
#endif

int xpon_los_status = 0;              /* indicate if there is optical signal */
EXPORT_SYMBOL(xpon_los_status);

#if defined(TCSUPPORT_XPON_LED)
static int led_xpon_status = 0;
EXPORT_SYMBOL(led_xpon_status);
uint8 led_ether_up_status[4] = {0};
EXPORT_SYMBOL(led_ether_up_status);
#endif

int (*hook_lan_led_action)(u8 mode);
EXPORT_SYMBOL(hook_lan_led_action);

#define MAX_GPIO_WIFI 17
int gpio_current[MAX_GPIO_WIFI]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
EXPORT_SYMBOL(gpio_current);
unsigned short wsc_done = 0;
EXPORT_SYMBOL(wsc_done);
int wscStatus=0;
EXPORT_SYMBOL(wscStatus);
int wscTimerRunning=0;
EXPORT_SYMBOL(wscTimerRunning);


#if defined(TCSUPPORT_CPU_MT7510) ||  defined(TCSUPPORT_CPU_MT7520) || defined(TCSUPPORT_CPU_MT7505)
int internet_hwnat_pktnum = 8;
int internet_hwnat_timer_switch = 1;

EXPORT_SYMBOL(internet_hwnat_pktnum);
EXPORT_SYMBOL(internet_hwnat_timer_switch);
#endif
#if NO_USE_UPSTREAM
static int get_led_data(int num);

#if defined(TCSUPPORT_WLAN_GPIO)
#if defined(RT5392) || defined(RT5390) || defined(RT5390HL)|| defined(MT7592)
int chkWlanGpioMode(uint8 led_gpio);
EXPORT_SYMBOL(chkWlanGpioMode);

uint8 getDslLedGpio(void);
EXPORT_SYMBOL(getDslLedGpio);
#endif
#endif


/*********************************************************************************
 * function prototype setction
 *
 *********************************************************************************/
extern int init_led_notifier_list(void);
#endif
static int led_wifi(int action, int num, int b);

#if defined(TCSUPPORT_XPON_LED)
static int led_xpon_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data);
static int led_xpon_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data);
#endif

/*********************************************************************************
 * global definition setction
 *
 *********************************************************************************/
static struct gpio_keys_button *keys = NULL;
static struct platform_device *plat_dev = NULL;

static ledctrl_t ledDefCtrl[LED_MAX_NO] = {
/* empty configuration */
{-1,       0,       0,       0},//0
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       0,       0,       0},
{-1,       1,       0,       0},//31
{-1,       0,       0,       0},//32
{-1,       0,       0,       0},//33
{-1,       0,       0,       0},//34
{-1,       0,       0,       0},//35
{-1,       0,       0,       0},//36
{-1,       0,       0,       0},//37
{-1,       0,       0,       0},//38
{-1,       0,       0,       0},//39
{-1,       0,       0,       0},//40
{-1,       0,       0,       0},//41
{25,      LED_MODE_ONOFF,       0,       1},//42 Tx_disable
{-1,       0,       0,       0},//43
{-1,       0,       0,       0},//44
{-1,       0,       0,       0},//45
{-1,       0,       0,       0},//46
{-1,       0,       0,       0},//47
{-1,       0,       0,       0},//48
{-1,       0,       0,       0},//49
{-1,       0,       0,       0},//50
{-1,       0,       0,       0},//51
{-1,       0,       0,       0},//52
{-1,       0,       0,       0},//53
{-1,       0,       0,       0},//54
{-1,       0,       0,       0},//55
{-1,       0,       0,       0},//56 SYS Reset
{-1,       0,       0,       0},//57
{-1,       0,       0,       0},//58
{-1,       0,       0,       0},//59
{-1,       0,       0,       0},//60
{-1,       0,       0,       0},//61
{-1,       0,       0,       0},//62
{-1,       0,       0,       0},//63
{-1,       0,       0,       0},//64
{-1,       0,       0,       0},//65
{-1,       0,       0,       0},//66
{-1,       0,       0,       0},//67
{-1,       0,       0,       0},//68
{-1,       0,       0,       0},//69
{-1,       0,       0,       0},//70
{-1,       0,       0,       0},//71
{-1,       0,       0,       0},//72
{-1,       0,       0,       0},//73
{-1,       0,       0,       0},//74
{-1,       0,       0,       0},//75
{-1,       0,       0,       0},//76
{-1,       0,       0,       0},//77
{-1,       0,       0,       0},//78
{-1,       0,       0,       0},//79
{-1,       0,       0,       0},//80
{-1,       0,       0,       0},//81
{-1,       0,       0,       0},//82
{-1,       0,       0,       0},//83
{-1,       0,       0,       0},//84
{-1,       0,       0,       0},//85
{-1,       0,       0,       0},//86
{-1,       0,       0,       0},//87
{-1,       0,       0,       0},//88
{-1,       0,       0,       0},//89
{-1,       0,       0,       0},//90
{-1,       0,       0,       0},//91
{-1,       0,       0,       0},//92
{-1,       0,       0,       0},//93
{-1,       0,       0,       0},//94
{-1,       0,       0,       0},//95
{-1,       0,       0,       0},//96
{-1,       0,       0,       0},//97
{-1,       0,       0,       0},//98
{-1,       0,       0,       0},//99
{-1,       0,       0,       0},//100
{-1,       0,       0,       0},//101
{-1,       0,       0,       0},//102
{-1,       0,       0,       0},//103
{-1,       0,       0,       0},//104
{-1,       0,       0,       0},//105
{-1,       0,       0,       0},//106
};

struct led_name_t led_name_map[] = {
	{"dsl",  0},
	{"dsl",  1},
	{"dsl",  2},
	{"dsl",  3},
	{"dsl",	 4},
	{"dsl",	 5},
	{"dsl",	 6},
	{"ppp",  7},
	{"ppp",  8},
	{"ppp",  9},
	{"usb_0",  10},
	{"usb_1",  11},
	{"usb_2",  12},
	{"wlan", 13},
	{"wlan", 14},
	{"wlan", 15},
	{"ethernet", 16},
	{"ethernet", 17},
	{"ethernet", 18},
	{"flash", 19},
	{"system", 20},
	{"system", 21},
	{"system", 22},
	{"eth_10M", 23},
	{"eth_10M", 24},
	{"eth_10M", 25},
	{"eth_100M", 26},
	{"eth_100M", 27},
	{"eth_100M", 28},
	{"sim_cfg", 29},
	{"sim_cfg", 30},
	{"lan_reset", 31},
	{"internet", 33},
	{"internet", 34},
	{"internet", 35},
	{"internet_try", 38},
	{"power", 40},
	{"power", 41},
	{"phy_tx_power_disable", 42},
	{"wps", 43},
	{"wps", 44},
	{"wps", 45},
	{"voip_reset", 46},
	{"voip_reset2", 47},
	{"dsl_alarm", 48},
	{"voip_hook", 49},
	{"voip_hook2", 50},
	{"sim", 51},
	{"sim", 52},
	{"sim", 53},
	{"usb2", 54},
	{"usb2", 55},
	{"sysReset", 56},
	{"wlanRadio", 57},
	{"wlanWps", 58},
	{"pwrFlash", 59},
	{"pwrUSB", 61},
	{"switch", 62},
	{"LAN1_reset", 63},
	{"xponStatus", 64},
	{"xponStatus", 65},
	{"xponLos", 66},
	{"xponLos", 67},
	{"eth1", 68},
	{"eth1", 69},
	{"eth2", 70},
	{"eth2", 71},
	{"eth3", 72},
	{"eth3", 73},
	{"eth4", 74},
	{"eth4", 75},
	{"voip_reg", 101},
	{"phyVccDis", 102},
	{"wps_5g", 104},
	{"radio_5g", 105},
	{"phy_tx_ben", 106},
};
#if NO_USE_UPSTREAM

/*notice: ledno 42 for phy tx disable*/
struct btn_info_t btn_info[] = {
	{56, "BTN_0",	BTN_0,		0xff, 0},
	{57, "BTN_1",	BTN_0+1,	0xff, 0},
	{58, "BTN_2",	BTN_0+2,	0xff, 0},
	{-1, "failsafe",-1,			0xff, 0},
	{-1, "power",	-1,			0xff, 0},
	{-1, "reset",	-1,			0xff, 0},
	{-1, "rfkill",  -1,			0xff, 0},
};

/*trigger name*/
static char dsl_trig_name[] = {"dsldev"};

static void led_platdev_release(struct device *dev)
{
	return;
}

static struct platform_device led_plat_device = {
	.name = DEVICENAME,
	.id = -1,
	.dev = {
		.release = led_platdev_release,
	}
};
#endif

#if defined(TCSUPPORT_WLAN_GPIO)
static int iswifigpio(int num)
{
	int ret;

	if((num >= (WLAN_GPIO_START + WLAN_GPIO_MAP_OFFSET)) && (num <= (WLAN_GPIO_END + WLAN_GPIO_MAP_OFFSET)))
	{
		ret = (num - WLAN_GPIO_MAP_OFFSET);
	}
	else
	{
		ret = -1;
	}

	return ret;
}
#endif

static inline int led_wifi(int action, int num, int biclor)
{
#if defined(TCSUPPORT_WLAN_GPIO)
	int wifi_gpio = 0;

	if (biclor) 
	{
		return 0;
	}

	wifi_gpio = iswifigpio(num);
	//printk("Led num %d maps to wifi gpio %d\n",num,wifi_gpio);
	if (wifi_gpio != -1)
	{
		/* for blink map */
		if (LED_FULL == action)
		{
			action = WLAN_LED_ON;
		}

		if (hook_wlan_led_action != NULL)
		{
			hook_wlan_led_action(action,wifi_gpio);
		}
		return 1;
	}
#endif

	return 0;
}
#if NO_USE_UPSTREAM

#ifdef TCSUPPORT_USB_HOST_LED
void Usb_Led_Flash_Op(unsigned int opmode ,unsigned int phyport)
{
	int temp = 0;
	int phyport_sw = phyport;

	if (isEN7526c)
		phyport_sw = USBPHYPORT2;

	switch (opmode)
	{
		case USB_DISCONNECT:
		if(USBPHYPORT1 == phyport_sw)
		{
			temp = usb_dev_status & 0x0000ffff;
			temp = --temp < 0? 0:temp;
			usb_dev_status &= 0xffff0000;
			usb_dev_status |= temp & 0xffff;
			if(!temp)
			{
				usb_phyport_status &= ~(1<<phyport_sw);
				//ledTurnOff(LED_USB_ACT_STATUS);
				ledTurnOff(LED_USB_STATUS);
			}
		}
		else if(USBPHYPORT2 == phyport_sw)
		{
			temp = (usb_dev_status & 0xffff0000) >> 16;
			temp = --temp < 0? 0:temp;
			usb_dev_status &= 0x0000ffff;
			usb_dev_status |= (temp << 16) & 0xffff0000;
			if(!temp)
			{
				usb_phyport_status &= ~(1<<phyport_sw);
				//ledTurnOff(LED_USB2_ACT_STATUS);
				ledTurnOff(LED_USB2_STATUS);
			}
		}

		break;

		case USB_CONNECT:
		if(USBPHYPORT1 == phyport_sw)
		{
			temp = usb_dev_status & 0xffff;
			//temp++;
			usb_dev_status &= 0xffff0000;
			usb_dev_status |= ++temp & 0xffff;
			//ledTurnOff(LED_USB_ACT_STATUS);
			ledTurnOn(LED_USB_STATUS);
		}
		else if(USBPHYPORT2 == phyport_sw)
		{
			temp = (usb_dev_status & 0xffff0000) >> 16;
			//temp++;
			usb_dev_status &= 0xffff;
			usb_dev_status |= (++temp << 16) & 0xffff0000;
			//ledTurnOff(LED_USB2_ACT_STATUS);
			ledTurnOn(LED_USB2_STATUS);
		}
		usb_phyport_status |= 1<<phyport_sw;

		break;

		case USB_BLINK:
		if(USBPHYPORT1 == phyport_sw)
			ledTurnOn(LED_USB_ACT_STATUS);

		if(USBPHYPORT2 == phyport_sw)
			ledTurnOn(LED_USB2_ACT_STATUS);

		break;

		case USB_DEFAULT://Fall through
		default:
		//for timer use
		if(usb_dev_status){
			if(usb_phyport_status & (1<<USBPHYPORT1))
			{
				ledTurnOn(LED_USB_STATUS);
			}
			else
			{
				ledTurnOff(LED_USB_STATUS);
			}

			if(usb_phyport_status & (1<<USBPHYPORT2))
			{
				ledTurnOn(LED_USB2_STATUS);
			}
			else
			{
				ledTurnOff(LED_USB2_STATUS);
			}
		}
		else{
			ledTurnOff(LED_USB_STATUS);
			ledTurnOff(LED_USB_ACT_STATUS);
			ledTurnOff(LED_USB2_STATUS);
			ledTurnOff(LED_USB2_ACT_STATUS);
		}
		break;
	}

	return;
}
#endif

static inline void led_on_off(int action,int num, int active_low,bool bicolor)
{
	uint32 word;

	if (led_wifi(action, num, bicolor))
	{
		return;
	}

	if (num <= GPIO_REG_DATA_NUM)
	{
		//printk("set DATA1 GPIO %d (on:off %d)\n",num,action);
		if ((!active_low && (action == GPIO_LED_OFF))
			|| (active_low && (action)))
		{
			if (bicolor)
			{
				word = regRead32(GPIO_REG_DATA);
				word &= ~(1 << (LED_GPIO1(num)));
				word &= ~(1 << (LED_GPIO2(num)));
				regWrite32(GPIO_REG_DATA,word);
			}
			else
			{
				word = regRead32(GPIO_REG_DATA);
				word &= ~(1<<(num));
				regWrite32(GPIO_REG_DATA,word);
			}
		}
		else if ((!active_low && (action))
			|| (active_low && (action == GPIO_LED_OFF)))
		{
			if (bicolor)
			{
				word = regRead32(GPIO_REG_DATA);
				word |= (1 << (LED_GPIO1(num)));
				word |= (1 << (LED_GPIO2(num)));
				regWrite32(GPIO_REG_DATA,word);
			}
			else
			{
				word = regRead32(GPIO_REG_DATA);
				word |= (1 << (num));
				regWrite32(GPIO_REG_DATA,word);
			}
		}
	}
	else if ((num > GPIO_REG_DATA_NUM) && (num <= GPIO_REG_DATA1_NUM))
	{
		//printk("set DATA2 GPIO %d (on:off %d)\n",num,action);
		if ((!active_low && (GPIO_LED_OFF == action))
			|| (active_low && (action)))
		{
			if (bicolor)
			{
				word = regRead32(GPIO_REG_DATA1);
				word &= ~(1 << (LED_GPIO1(num-32)));
				word &= ~(1 << (LED_GPIO2(num-32)));
				regWrite32(GPIO_REG_DATA1,word);
			}
			else
			{
				word = regRead32(GPIO_REG_DATA1);
				word &= ~(1<<(num-32));
				regWrite32(GPIO_REG_DATA1,word);
			}
		}
		else if ((!active_low && (action))
			|| (active_low && (GPIO_LED_OFF == action)))
		{
			if (bicolor)
			{
				word = regRead32(GPIO_REG_DATA1);
				word |= (1 << (LED_GPIO1(num-32)));
				word |= (1 << (LED_GPIO2(num-32)));
				regWrite32(GPIO_REG_DATA1,word);
			}
			else
			{
				word = regRead32(GPIO_REG_DATA1);
				word |= (1 << (num-32));
				regWrite32(GPIO_REG_DATA1,word);
			}
		}
	}
}

static inline void gpio_input_enable(int num)
{
	uint32 word;

	if (led_wifi(WLAN_LED_IEN,num,0))
	{
		return;
	}

	/* set input mode and enable */
	if (num <= GPIO_REG_CTRL_NUM)
	{
		/*GPIO control reg */
		word = regRead32(GPIO_REG_CTRL);
		word &= ~(0x00000003 << ((num)*2));
		regWrite32(GPIO_REG_CTRL,word);

		/*clear GPIO output enable reg bit */
		word = regRead32(GPIO_REG_OEN);
		word &= ~(1 << (num));
		regWrite32(GPIO_REG_OEN,word);
	}
	else if ((num > GPIO_REG_CTRL_NUM) && (num <= GPIO_REG_CTRL1_NUM))
	{
		/*GPIO control reg1 */
		word = regRead32(GPIO_REG_CTRL1);
		word &= ~(0x00000003 << ((num-16)*2));
		regWrite32(GPIO_REG_CTRL1,word);

		/*clear GPIO output enable reg bit*/
		word = regRead32(GPIO_REG_OEN);
		word &= ~ (1 << (num));
		regWrite32(GPIO_REG_OEN,word);
	}
	else if ((num > GPIO_REG_CTRL1_NUM) && (num <= GPIO_REG_CTRL2_NUM))
	{
		/*GPIO control reg2 */
		word = regRead32(GPIO_REG_CTRL2);
		word &= ~(0x00000003 << ((num-32)*2));
		regWrite32(GPIO_REG_CTRL2,word);

		/*clear GPIO output enable reg1 bit*/
		word = regRead32(GPIO_REG_OEN1);
		word &= ~ (1 << (num - 32));
		regWrite32(GPIO_REG_OEN1,word);
	}
	else if ((num > GPIO_REG_CTRL2_NUM) && (num <= GPIO_REG_CTRL3_NUM))
	{
		/*GPIO control reg3 */
		word = regRead32(GPIO_REG_CTRL3);
		word &= ~(0x00000003 << ((num-48)*2));
		regWrite32(GPIO_REG_CTRL3,word);

		/*clear GPIO output enable reg1 bit*/
		word = regRead32(GPIO_REG_OEN1);
		word &= ~ (1 << (num - 32));
		regWrite32(GPIO_REG_OEN1,word);
	}

}

static inline void gpio_output_enable(int num)
{
	uint32 word;

	if (led_wifi(WLAN_LED_OEN,num,0))
	{
		return;
	}

	/* set output mode and enable */
	if (num <= GPIO_REG_CTRL_NUM)
	{
		/*GPIO control reg */
		word = regRead32(GPIO_REG_CTRL);
		word |= (1 << ((num)*2));
		regWrite32(GPIO_REG_CTRL,word);

		/*GPIO output enable reg */
		word = regRead32(GPIO_REG_OEN);
		word |= (1 << (num));
		regWrite32(GPIO_REG_OEN,word);
	}
	else if ((num > GPIO_REG_CTRL_NUM) && (num <= GPIO_REG_CTRL1_NUM))
	{
		/*GPIO control reg1 */
		word = regRead32(GPIO_REG_CTRL1);
		word |= (1 << ((num-16)*2));
		regWrite32(GPIO_REG_CTRL1,word);

		/*GPIO output enable reg */
		word = regRead32(GPIO_REG_OEN);
		word |= (1 << (num));
		regWrite32(GPIO_REG_OEN,word);
	}
	else if ((num > GPIO_REG_CTRL1_NUM) && (num <= GPIO_REG_CTRL2_NUM))
	{
		/*GPIO control reg2 */
		word = regRead32(GPIO_REG_CTRL2);
		word |= (1 << ((num-32)*2));
		regWrite32(GPIO_REG_CTRL2,word);

		/*GPIO output enable reg1 */
		word = regRead32(GPIO_REG_OEN1);
		word |= (1 << (num-32));
		regWrite32(GPIO_REG_OEN1,word);
	}
	else if ((num > GPIO_REG_CTRL2_NUM) && (num <= GPIO_REG_CTRL3_NUM))
	{
		/*GPIO control reg3 */
		word = regRead32(GPIO_REG_CTRL3);
		word |= (1 << ((num-48)*2));
		regWrite32(GPIO_REG_CTRL3,word);

		/*GPIO output enable reg1 */
		word = regRead32(GPIO_REG_OEN1);
		word |= (1 << (num-32));
		regWrite32(GPIO_REG_OEN1,word);
	}
}

static inline int gpio_get_data(int num)
{
	int ret = 0;
#if defined(TCSUPPORT_WLAN_GPIO)
	int wifi_gpio = 0;

	wifi_gpio = iswifigpio(num);
	if (wifi_gpio != -1)
	{

		if (hook_wlan_led_get_data != NULL)
		{
			ret = hook_wlan_led_get_data(wifi_gpio);
		}
		else
		{
			//0 means press.
			ret = (1 << wifi_gpio);
		}
	}
	else
#endif
	{
		if (num < GPIO_REG_DATA_NUM)
		{
			ret = regRead32(GPIO_REG_DATA) & (1<<num);
		}
		else if (num < GPIO_REG_DATA1_NUM)
		{
			ret = regRead32(GPIO_REG_DATA1) & (1<<(num-32));
		}
	}

	return ret;
}

static inline int sizeof_econet_led_priv(int num_leds)
{
	return sizeof(struct econet_led_priv) + (sizeof(struct econet_led) * num_leds);
}
#endif
static char* led_conf_path_find(void)
{
    uint8 i;
    char *p_path = NULL;
    uint8 arr_len = 0;
	struct file	*srcf;
    led_path_m path_table[]=
    {
    	{isFPGA, "/userfs/led_fpga.conf"},
        #ifdef TCSUPPORT_CPU_EN7523
        {isEN7529DU || isEN7529DT || isEN7529CU || isEN7562DU ||
		 isEN7562DT || isEN7562CU || isEN7529GTH || isEN7562GTH ||
		 isEN7529GTS || isEN7562GTS || isEN7529IT ||  isEN7529CT ||
		 isEN7562CT || isEN7523DT || isEN7529DTM || isEN7562DTM || 
		 isEN7529ITM ||	isEN7529CTM || isEN7562CTM || isEN7523DTM, "/userfs/7529_62led.conf"},
        {isEN7523GU, "/userfs/7523guled.conf"},
        {isEN7523DU, "/userfs/7523duled.conf"},
        {isEN7523SU, "/userfs/7523suled.conf"},
	#ifdef TCSUPPORT_CPU_EN7581
	{(isAN7581GT || isAN7581FG), "/userfs/7581gtled.conf"},
	{(isAN7581PT || isAN7581CT || isAN7581DT || isAN7581ST ||
			isAN7551PT || isAN7566PT || isAN7581IT ||
			isAN7581FP || isAN7581FD), "/userfs/7581ptled.conf"},
	#endif
        #else
        {isEN7526FT, "/userfs/7526ftled.conf"},
        {isEN7526FT_C, "/userfs/7526ftled.conf"},
        {isEN7526F, "/userfs/7526fled.conf"},
        {isEN7526D, "/userfs/7526dled.conf"},
        {isEN7526G, "/userfs/7526gled.conf"},
        {isEN7512, "/userfs/7512led.conf"},
        {isEN7513, "/userfs/7513led.conf"},
        {isEN7513G, "/userfs/7513gled.conf"},
        {isEN7586, "/userfs/7586led.conf"},
        {isEN7521F, "/userfs/7521fled.conf"},
        {isEN7521G, "/userfs/7521gled.conf"},
        {isEN7521S, "/userfs/7521sled.conf"},
        {isEN7516G, "/userfs/7516gled.conf"},
        {isEN7527G, "/userfs/7527gled.conf"},
        {isEN7527H, "/userfs/7527hled.conf"},
        #ifdef TCSUPPORT_CPU_EN7580
        {(isEN7580 && GET_IS_DDR4), "/userfs/7580led_ddr4.conf"},
        #endif
        #endif
        {isDEFAULT,LED_CONFIG_PATH}/*must last element*/
    };

    arr_len = sizeof(path_table)/sizeof(path_table[0]);

    /*find led.conf*/
    for(i = 0; i < arr_len; i++)
    {
        if(path_table[i].chipid)
        {
            p_path = path_table[i].path;
            break;
        }
    }

    /*check whether led.conf file exist,otherwise use default*/
    if(p_path && *p_path)
    {
        srcf = filp_open(p_path, O_RDONLY, 0);
        if (IS_ERR(srcf))
        {
            p_path = path_table[arr_len-1].path;
        }
        else
        {
            filp_close(srcf,NULL);
        }
    }
    else
    {
        printk("find led.conf error\r\n");
    }

    /*use default led.conf*/
    if(i == arr_len)
        p_path = path_table[arr_len-1].path;

    /*if TCSUPPORT_XPON_HAL_API_EXT,rewrite path*/
#if defined(TCSUPPORT_XPON_HAL_API_EXT)
    p_path = "/etc/led.conf";
#endif

    return p_path;
}

void led_read_config(void)
{
	struct file *srcf = NULL;
	uint32 tmp_fpos = 0;
	char *src = NULL;
	mm_segment_t orgfs;
	char *buffer = NULL;
	char *tmpbuf = NULL;
	int set_info[5] = {0};
	const int bufSize = 128;

	buffer = kmalloc(bufSize, GFP_ATOMIC);
	if(NULL == buffer)
	{
		printk("--> kmalloc failed \n");
		return;
	}

	src = led_conf_path_find();
	printk("led path:%s\r\n",src);

	/*Save uid and gid used for filesystem access.
	Set user and group to 0 (root) */
	orgfs = get_fs();
	set_fs(KERNEL_DS);

	if (src && *src)
	{
		srcf = filp_open(src, O_RDONLY, 0);
		if (IS_ERR(srcf))
		{
			printk("--> Error opening %s\n",src);
		}
		else
		{
			memset(buffer,0,bufSize);
			while(ecnt_kernel_fs_read(srcf, buffer, bufSize, &srcf->f_pos)>0)
			{
				tmpbuf = buffer;
				/* filter comment line */
				if ((*tmpbuf) != ';')
				{
					sscanf(tmpbuf,"%d %d %d %d %d",set_info,(set_info+1),(set_info+2),(set_info+3),(set_info+4));
					if (set_info[2] > 10)
					{
						sscanf(tmpbuf,"%d %x %x %d %d",set_info,(set_info+1),(set_info+2),(set_info+3),(set_info+4));
					}
					if(set_info[0] < LED_MAX_NO)
					{
						ledDefCtrl[set_info[0]].led_no = set_info[0];
						ledDefCtrl[set_info[0]].mode   = set_info[2];
						ledDefCtrl[set_info[0]].gpio   = (set_info[2] == LED_MODE_NOT_USED)?(-1):set_info[1];
						ledDefCtrl[set_info[0]].speed  = set_info[3];
						ledDefCtrl[set_info[0]].onoff  = set_info[4];
					}
				}
				/*If read the end of line, read the next*/
				while ((*tmpbuf) != '\r' && (*tmpbuf) != '\n')
				{
					tmp_fpos++;
					tmpbuf++;
				}

				/*indicate the next head of line*/
				tmp_fpos++;
				srcf->f_pos = tmp_fpos;
				memset(buffer, 0, bufSize);
			}
			filp_close(srcf,NULL);
		}
	}

	set_fs(orgfs);
	kfree(buffer);
}
int led_get_gpio(uint8 *gpio,char led_name[])
{
	int i = 0;
	uint8 led_no = 0;
	int array_size = (sizeof(led_name_map) / sizeof(led_name_map[0]));

	for (i = 0; i < array_size; i++)
	{
		if (strncmp(led_name,led_name_map[i].name,LED_NAME_LEN)== 0)
		{
			led_no=led_name_map[i].led_no;
			*gpio = ledDefCtrl[led_no].gpio;
			return 1;
		}
	}

	return 0;
}
EXPORT_SYMBOL(led_get_gpio);

#if NO_USE_UPSTREAM
static inline int GpioIndexExist(int idx,int *pidx,int size)
{
	uint8 i = 0;

	if (NULL == pidx)
	{
		return 0;
	}

	for (i = 0; i < size; i++)
	{
		if (idx == pidx[i])
		{
			return 1;
		}
	}

	return 0;
}

int led_get_name(uint8 led_no,char led_name[])
{
	int i = 0;
	int array_size = (sizeof(led_name_map) / sizeof(led_name_map[0]));

	for (i = 0; i < array_size; i++)
	{
		if (led_no == led_name_map[i].led_no)
		{
			strncpy(led_name,led_name_map[i].name,LED_NAME_LEN);

			return 1;
		}
	}

	return 0;
}

int led_get_info(ledctrl_t led_gpio[])
{
	uint8 j,k,n = 0;
	int m = 0;

	int key_idx[LED_MAX_NO];

	for (j = 0; j < LED_MAX_NO; j++)
	{
		key_idx[j] = -1;
	}

	for (j = 0; j < LED_MAX_NO; j++)
	{
		/* ignore the not used LED and same gpio */
		if ((LED_MODE_NOT_USED == ledDefCtrl[j].mode)
			|| (LED_MODE_INPUT== ledDefCtrl[j].mode)
			|| (GpioIndexExist(j,key_idx,ARRAY_SIZE(key_idx))))
		{
			continue;
		}

		/* look for the same gpio */
		for (k = j + 1; k < LED_MAX_NO; k++)
		{
			if (ledDefCtrl[j].gpio == ledDefCtrl[k].gpio)
			{
				key_idx[n++] = k;
			}
		}

		/* save the unique gpio info */
		led_gpio[m].led_no= ledDefCtrl[j].led_no;
		led_gpio[m].gpio = ledDefCtrl[j].gpio;
		led_gpio[m].mode = ledDefCtrl[j].mode;
		led_gpio[m].speed = ledDefCtrl[j].speed;
		led_gpio[m++].onoff = ledDefCtrl[j].onoff;
	}

	/* set gpio to output mode */
	for (j = 0; j < m; j++)
	{
		gpio_output_enable(led_gpio[j].gpio);
	}

	return m;
}

void led_config_brightness(struct led_classdev *led_cdev,enum led_brightness value)
{
	struct econet_led *led = CDEV_TO_LED(led_cdev);

	led_on_off(value,led->gpio,led->active_low,led->bicolor);
}

static int led_add(ledctrl_t *led_info, struct econet_led *led, struct device *parent)
{
	int ret,i;

	if ((NULL == led) || (NULL == led_info))
	{
		printk("led is NULL\n");
		return -1;
	}

	led->gpio = led_info->gpio;
	led->bicolor = LED_BICOLOR(led_info->mode);
	led->speed = led_info->speed;
	if (led_info->onoff)
	{
		led->active_low = 0;
	}
	else
	{
		led->active_low = 1;
	}

	/*get led name */
	if (led_get_name(led_info->led_no,led->name))
	{
		led->cdev.name = led->name;
	}
	else
	{
		return 0;
	}

	led->cdev.brightness = 0;
	led->cdev.brightness_set = led_config_brightness;
	led->cdev.blink_delay_on = led->speed;
	led->cdev.blink_delay_off = led->speed;


	if(0 != led->gpio )
	{
		led->cdev.default_trigger = dsl_trig_name;
		led->mode = 2;
	}


	ret = led_classdev_register(parent,&led->cdev);
	if (ret < 0)
	{
		dev_err(parent,"register gpio %d LED failed\n",led->gpio);
		return ret;
	}

	return 0;
}

static void led_delete(struct econet_led *led)
{
	led_classdev_unregister(&led->cdev);
}

static int led_probe(struct platform_device *pdev)
{
	int i,ret = 0;
	int num_leds = 0;
	ledctrl_t led_gpio[LED_MAX_NO];
	struct econet_led_priv *priv = NULL;

	for (i = 0; i < LED_MAX_NO; i++)
	{
		led_gpio[i].led_no = 0;
		led_gpio[i].gpio = -1;
		led_gpio[i].mode = LED_MODE_NOT_USED;
		led_gpio[i].speed = 0;
		led_gpio[i].onoff= 1;
	}

	/* get gpio-led info */
	num_leds = led_get_info(led_gpio);
	if (0 == num_leds)
	{
		dev_err(&pdev->dev, "no gpio leds\n");
		return -ENODEV;
	}

	priv = devm_kzalloc(&pdev->dev,sizeof_econet_led_priv(num_leds),GFP_KERNEL);
	if (NULL == priv)
	{
		dev_err(&pdev->dev, "devie memory alloc error\n");
		return -ENOMEM;
	}

	/* register led device */
	priv->num_leds = num_leds;
	for (i = 0; i < priv->num_leds; i++)
	{
		ret = led_add(&led_gpio[i],&priv->leds[i],&pdev->dev);
		if (ret < 0) 
		{
			for (i = i - 1; i >= 0; i--)
			{
				/* delete device */
				led_delete(&priv->leds[i]);
			}

			devm_kfree(&pdev->dev,priv);
			return ret;
		}
	}

	/* save priv */
	platform_set_drvdata(pdev, priv);

	return 0;
}

static int led_remove(struct platform_device *pdev)
{
	int i;
	struct econet_led_priv *priv = platform_get_drvdata(pdev);

	for (i = 0; i < priv->num_leds; i++)
	{
		led_delete(&priv->leds[i]);
	}

	devm_kfree(&pdev->dev,priv);
	return 0;
}

static struct platform_driver led_plat_driver = {
	.probe = led_probe,
	.remove = led_remove,
	.driver = {
		.name = DEVICENAME,
		.owner = THIS_MODULE,
	},
};


/*********************************************************************************
 * GPIO buttons/keys implementation setction
 *
 *********************************************************************************/
static void __init register_gpio_keys(int id,unsigned poll_interval,
					    unsigned nbuttons,struct gpio_keys_button *buttons)
{
	struct gpio_keys_platform_data pdata;
	struct gpio_keys_button *p = NULL;
	int err;

	p = kmemdup(buttons, nbuttons * sizeof(*p), GFP_KERNEL);
	if (NULL == p)
	{
		return;
	}

	plat_dev = platform_device_alloc("gpio-keys-polled", id);
	if (NULL == plat_dev)
	{
		kfree(p);
		p = NULL;

		return;
	}

	memset(&pdata, 0, sizeof(pdata));
	pdata.poll_interval = poll_interval;
	pdata.nbuttons = nbuttons;
	pdata.buttons = p;

	err = platform_device_add_data(plat_dev, &pdata, sizeof(pdata));
	if (err)
	{
		platform_device_put(plat_dev);
		kfree(p);
		p = NULL;

		return;
	}

	err = platform_device_add(plat_dev);
	if (err)
	{
		platform_device_put(plat_dev);
		kfree(p);
		p = NULL;
	}
}


#ifdef CONFIG_GPIOLIB
static int ecnt_gpio_get_value(struct gpio_chip *chip, unsigned offset)
{
	return gpio_get_data(offset);
}

static int ecnt_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	gpio_input_enable(offset);

	return 0;
}

static int ecnt_gpio_direction_output(struct gpio_chip *chip, unsigned offset,int value)
{
	gpio_output_enable(offset);

	return 0;
}

static struct gpio_chip ecnt_gpio_chip = {

	.label				= "ecnt",
	.ngpio 				= LED_MAX_NO,
	.base				= 0,
	.get				= ecnt_gpio_get_value,
	.direction_input	= ecnt_gpio_direction_input,
	.direction_output	= ecnt_gpio_direction_output,
};
#endif


void __init ecnt_gpio_init(void)
{

	int err = 1;

#ifdef CONFIG_GPIOLIB
	err = gpiochip_add(&ecnt_gpio_chip);
#endif

	if (err)
	{
		panic("cannot add ECNT GPIO chip, error=%d\n", err);
	}
}
int btn_info_update(void)
{
	int k = 0, i = 0, ret = 0;
	int array_size = (sizeof(btn_info) / sizeof(btn_info[0]));
	for (k = 0; k < LED_MAX_NO; k++)
	{
		for (i = 0; i < array_size; i++)
		{
			if ((ledDefCtrl[k].led_no == btn_info[i].led_no)
				&& (-1 !=btn_info[i].code) && (ledDefCtrl[k].mode == 5))
			{
				btn_info[i].gpio = ledDefCtrl[k].gpio;
				btn_info[i].active = ledDefCtrl[k].onoff;
				ret ++;
				continue;
			}
		}
	}

	return ret;
}

void __init ecnt_gpio_keys_init(void)
{

	int i = 0, k = 0;
	int button_num = 0;

	/* get gpio button info */
	button_num = btn_info_update();

	printk("=====ecnt_gpio_keys_init============= button_num is %d \n" ,button_num);

	if (0 == button_num)
	{
		return;
	}

	keys = kmalloc(sizeof(*keys) * button_num, GFP_KERNEL);
	if (NULL == keys)
	{
		return;
	}

	/* init keys button */
	for(i = 0 ; i < sizeof(btn_info) / sizeof(btn_info[0]); i++)
	{
		if( 0xff == btn_info[i].gpio ) continue;
		keys[k].code = btn_info[i].code;
		keys[k].gpio = btn_info[i].gpio;
		if (btn_info[i].active)
		{
			keys[k].active_low = 0;
		}
		else
		{
			keys[k].active_low = 1;
		}
		keys[k].desc = btn_info[i].name;
		keys[k].type = EV_KEY;
		keys[k].wakeup = 0;
		keys[k].debounce_interval = ECNT_KEYS_DEBOUNCE_INTERVAL;
		k++;
	}

	register_gpio_keys(-1,ECNT_KEYS_POLL_INTERVAL,button_num,keys);
}

void ecnt_gpio_exit(void)
{
	platform_device_unregister(plat_dev);

	if (keys)
	{
		kfree(keys);
	}

	#ifdef CONFIG_GPIOLIB
	gpiochip_remove(&ecnt_gpio_chip);
	#endif
}

uint32 get_gpio_ssr(void)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    return GET_GPIO_SSR();
    #else
    return regRead32(CR_GPIO_SHR);
    #endif
}

void set_gpio_ssr(uint32 word)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    SET_GPIO_SSR(word);
    #else
	regWrite32(CR_GPIO_SHR,word);
    #endif
    return;
}

/*______________________________________________________________________________
**	ledInit
**
**	descriptions:
**	parameters:
**	local:
**	global:
**	return:
**	called by:
**	call:
**	revision:
**____________________________________________________________________________*/
void EcntledInit(void)
{
	uint8  i;
	uint32 word;

#ifndef CONFIG_MIPS_TC3262
	/* LED USB, XDSLLINK, XDSLACT, SYS, ROM_WRPT: output pin */
	regWrite32(CR_GPIO_CTRL,(1<<(5*2))|(1<<(3*2))|(1<<(2*2))|(1<<(1*2))|(1<<(0*2)));
	regWrite32(CR_GPIO_ODRAIN, (1<<5)|(1<<3)|(1<<2)|(1<<1)|(1<<0));

	// set gpio6 as input pin; gpio6 is reset button
	word = regRead32(CR_GPIO_CTRL);
	word &= 0xffffcfff;
	regWrite32(CR_GPIO_CTRL,word);

#endif
#ifdef TCSUPPORT_CPU_MT7520
	// set bfb00860 bit14=1, single led mode.
	word = get_gpio_ssr();
	word |= (1<<14);

#ifdef TCSUPPORT_XPON_LED
	// enable software control ether led.
	if (isMT7525 || isMT7520 || isMT7520S
	)
		word &= ~(0x1F<<9);
#endif

	set_gpio_ssr(word);
#endif

}
#endif

static int __init led_init(void)
{
	struct proc_dir_entry *led_proc;

	printk("MT751x LED driver init\n");
	led_read_config();
#if NO_USE_UPSTREAM
	init_led_notifier_list();
	EcntledInit();

	/* gpio button */
	ecnt_gpio_init();
	ecnt_gpio_keys_init();

	/* gpio led */
	platform_device_register(&led_plat_device);
	platform_driver_register(&led_plat_driver);
#endif
	dev_trig_init();

#if NO_USE_UPSTREAM
#ifdef TCSUPPORT_USB_HOST_LED
	rcu_assign_pointer(Usb_Led_Flash_Op_hook, Usb_Led_Flash_Op);
#if KERNEL_3_18_21
	if ( USB_CONNECT == pre_usb_state[0]
		|| USB_BLINK == pre_usb_state[0] )
		Usb_Led_Flash_Op(USB_CONNECT, USBPHYPORT1);
	if ( USB_CONNECT == pre_usb_state[1]
		|| USB_BLINK == pre_usb_state[1] )
		Usb_Led_Flash_Op(USB_CONNECT, USBPHYPORT2);
#endif
#endif

	ledTurnOn(LED_TR68_PWR_BOOTED);
#endif
	ledTurnOn(LED_PHY_TX_POWER_DISABLE);
	ledTurnOn(LED_PON_TX_BEN);

#if defined(TCSUPPORT_XPON_LED)
	/* xpon led */
	led_proc = create_proc_entry("tc3162/led_xpon", 0, NULL);
	led_proc->read_proc = led_xpon_read_proc;
	led_proc->write_proc = led_xpon_write_proc;
#endif
	return 0;
}

static void __exit led_exit(void)
{
	printk("MT751x LED driver exit\n");

#if defined(TCSUPPORT_XPON_LED)
	remove_proc_entry("tc3162/led_xpon", NULL);
#endif

#if NO_USE_UPSTREAM
	/* gpio led */
	platform_device_unregister(&led_plat_device);
	platform_driver_unregister(&led_plat_driver);
#endif
	dev_trig_exit();

#if NO_USE_UPSTREAM
#ifdef TCSUPPORT_USB_HOST_LED
	rcu_assign_pointer(Usb_Led_Flash_Op_hook, NULL);
#endif

	/* gpio button */
	ecnt_gpio_exit();
#endif
}
#if defined(TCSUPPORT_WLAN_GPIO) || (defined(TCSUPPORT_LED_SWITCH_BUTTON)&&defined(TCSUPPORT_WLAN))

void set_wlan_gpio_mode(void){

}
#endif

/*______________________________________________________________________________
**	ledTurnOff
**
**	descriptions:
**	parameters:
**	local:
**	global:
**	return:
**	called by:
**	call:
**	revision:
**____________________________________________________________________________*/
	void ledTurnOff(uint8 led_no)
{
	int ret = 0;
	ledctrl_t *pledctl = &ledDefCtrl[led_no];

	if ((LED_MODE_NOT_USED == pledctl->mode)
			|| (LED_MODE_INPUT== ledDefCtrl->mode))
		return;

	ret=call_led_notifier(pledctl->gpio,DSL_DOWN);

	return;
}

/*______________________________________________________________________________
**	ledTurnOn
**
**	descriptions:
**	parameters:
**	local:
**	global:
**	return:
**	called by:
**	call:
**	revision:
**____________________________________________________________________________*/
__IMEM void ledTurnOn(uint8 led_no)
{
	ledctrl_t *pledctl = &ledDefCtrl[led_no];

	if ((LED_MODE_NOT_USED == pledctl->mode)
			|| (LED_MODE_INPUT== ledDefCtrl->mode))
		return;

	if(pledctl->mode == 1)
		call_led_notifier(pledctl->gpio, DSL_UP);
	else if(pledctl->mode == 2)
		call_led_notifier(pledctl->gpio ,DSL_SYNCING);
	else
		return;

	return;
}

/*______________________________________________________________________________
**	ledGetMode
**
**	descriptions:
**	parameters:
**	local:
**	global:
**	return:
**	called by:
**	call:
**	revision:
**____________________________________________________________________________*/
uint8 ledGetMode(uint8 led_no)
{
	return LED_MODE(ledDefCtrl[led_no].mode);
}

#if defined(TCSUPPORT_WLAN_GPIO)
#if defined(RT5392) || defined(RT5390) || defined(RT5390HL)|| defined(MT7592)

/*_____________________________________________________________________________
**      function name: chkWlanGpioMode
**      descriptions:
**			check whether wlan gpio set correct or not
**
**      parameters:
**            None
**
**      global:
**            None
**
**      return:
**             0: set wrong
**             1: set correct
**
**      call:
**   	 None
**
**      revision:
**      1. shelven.lu 2012/05/08
**____________________________________________________________________________
*/
int chkWlanGpioMode(uint8 led_gpio)
{
	if(get_led_mode_data(led_gpio) || get_led_data(led_gpio)){
		return 0;
	}

	return 1;
}
/*_____________________________________________________________________________
**      function name: getDslLedGpio
**      descriptions:
**			get DSL led GPIO value
**
**      parameters:
**            None
**
**      global:
**             ledCtrl
**
**      return:
**             gpio value
**      call:
**   	 None
**
**      revision:
**      1. shelven.lu 2012/05/08
**____________________________________________________________________________
*/
uint8 getDslLedGpio(void)
{
	return ledDefCtrl[LED_DSL_STATUS].gpio;
}
/*_____________________________________________________________________________
**      function name: get_led_mode_data
**      descriptions:
**			get led mode value
**
**      parameters:
**            None
**
**      global:
**            None
**
**      return:
**             mode value
**      call:
**   	 None
**
**      revision:
**      1. shelven.lu 2012/05/08
**____________________________________________________________________________
*/

int get_led_mode_data(int led_gpio)
{
	int ret;
	int wifi_gpio = 0;
	int modeflag=WLAN_GPIO_MODE_DATE;
	wifi_gpio = iswifigpio(led_gpio);

	if(wifi_gpio != -1){
#ifndef TCSUPPORT_LED_SWITCH_BUTTON
		if(hook_wlan_led_get_data != NULL){
			ret=hook_wlan_led_get_data(wifi_gpio | modeflag);
		}
		else
#endif
		{
			ret = ((1<<wifi_gpio)+8);
		}
	}
	else
	{
		//ret = regRead32(CR_GPIO_CTRL) & (1<<led_gpio);
		ret = LED_GET_GPIO_CTRL(led_gpio);
	}
	return ret;
}

EXPORT_SYMBOL(get_led_mode_data);

#endif
#endif

static int get_led_data(int num){
	int ret;

#if defined(TCSUPPORT_WLAN_GPIO)
	int wifi_gpio = 0;

	wifi_gpio = iswifigpio(num);
// 	printk("wifi_gpio %d %d\n",wifi_gpio,num);
	if(wifi_gpio != -1){
		if(hook_wlan_led_get_data != NULL){
			ret = hook_wlan_led_get_data(wifi_gpio);
		}else
		{
			ret = (1<<wifi_gpio); //0 means press.
		}
	}else
#endif
	{
		//ret = regRead32(CR_GPIO_DATA) & (1<<num);
		ret = LED_GET_GPIO_DATA(num);

	}
	return ret;
}

void led_oen(uint8 led_no){
	LED_OEN(led_no);
}

void led_ien(uint8 led_no){
	LED_IEN(led_no);
}

uint8 ledGetGpio(uint8 led_no)
{
	return ledDefCtrl[led_no].gpio;
}

void ledSetMode(uint8 led_no, uint8 mode)
{
	if(ledGetMode(led_no) != LED_MODE_NOT_USED){
		ledDefCtrl[led_no].mode = mode;
	}
}

/*_____________________________________________________________________________
**      function name: led_xpon_read_proc & led_xpon_write_proc
**      descriptions:
**			xpon led proc func
**
**      parameters:
**            None
**
**      global:
**            None
**
**      return:
**            value
**      call:
**   	 None
**
**      revision:
**      1. da.pang 2021/03/24
**____________________________________________________________________________
*/

#if defined(TCSUPPORT_XPON_LED)
static int led_xpon_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%d\n", led_xpon_status);

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}

static int led_xpon_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[8];
	unsigned long val;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	val = simple_strtoul(val_string, NULL, 10);
#if defined(TCSUPPORT_XPON_LED_UPGRADE)	
	if(val < 0 || val > 3)
#else
	if(val < 0 || val > 2)
#endif		
		led_xpon_status = 0;
	else
		led_xpon_status = val;
	
#if defined(TCSUPPORT_C9_ROST_LED)
	if (led_xpon_status == 2 && xpon_los_status != 0) {
		ledTurnOff(LED_XPON_UNREG_STATUS); 
		ledTurnOff(LED_XPON_TRYING_STATUS); 	
		ledTurnOn(LED_XPON_STATUS);
	} else if(led_xpon_status == 1 && xpon_los_status != 0){
		ledTurnOff(LED_XPON_UNREG_STATUS); 
		ledTurnOff(LED_XPON_STATUS); 	
		ledTurnOn(LED_XPON_TRYING_STATUS);

	}else{
		ledTurnOff(LED_XPON_TRYING_STATUS); 
		ledTurnOff(LED_XPON_STATUS); 	
		ledTurnOn(LED_XPON_UNREG_STATUS);
	}
#else	
	if (led_xpon_status == 2 && xpon_los_status != 0) {
#if defined(TCSUPPORT_XPON_LED_UPGRADE)
		ledTurnOff(LED_XPON_UPGRADE);
#endif	
		ledTurnOff(LED_XPON_TRYING_STATUS);		
		ledTurnOn(LED_XPON_STATUS);
	} else if(led_xpon_status == 1 && xpon_los_status != 0){
#if defined(TCSUPPORT_XPON_LED_UPGRADE)
		ledTurnOff(LED_XPON_UPGRADE);
#endif	
		ledTurnOff(LED_XPON_STATUS);
		ledTurnOn(LED_XPON_TRYING_STATUS);
#if defined(TCSUPPORT_XPON_LED_UPGRADE)
	} else if(led_xpon_status == 3 && xpon_los_status != 0){
		ledTurnOff(LED_XPON_STATUS);
		ledTurnOff(LED_XPON_TRYING_STATUS);
		ledTurnOn(LED_XPON_UPGRADE);
#endif	
	}else{
		ledTurnOff(LED_XPON_STATUS);
		ledTurnOff(LED_XPON_TRYING_STATUS);
#if defined(TCSUPPORT_XPON_LED_UPGRADE)
		ledTurnOff(LED_XPON_UPGRADE);
#endif	

	}
#endif	

	return count;
}
#endif	
EXPORT_SYMBOL(ledTurnOn);
EXPORT_SYMBOL(ledTurnOff);
EXPORT_SYMBOL(ledGetMode);
/*EXPORT_SYMBOL(get_led_mode_data);*/

EXPORT_SYMBOL(ledSetMode);
EXPORT_SYMBOL(ledGetGpio);
EXPORT_SYMBOL(led_oen);
EXPORT_SYMBOL(led_ien);
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("MT751x LED driver");
