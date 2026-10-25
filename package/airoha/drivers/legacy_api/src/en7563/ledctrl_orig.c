#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/leds.h>
#include "linux/init.h"
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/fcntl.h>
#include <linux/kmod.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include "tcversion.h"
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include "ledctrl_kernel_api.h"
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif

#ifndef INT32
#define INT32
typedef int32_t int32;    		/* 32-bit signed integer        */
#endif

#ifndef UINT32
#define UINT32
typedef uint32_t uint32; 		/* 32-bit unsigned integer      */
#endif

#ifndef UINT16
#define UINT16
typedef uint16_t uint16;          /* 16-bit unsigned integer      */
#endif

#ifndef UINT8
#define UINT8
typedef uint8_t uint8;            /* 8-bit unsigned integer       */
#endif

#ifndef INT16
#define INT16
typedef signed short int int16;         /* 16-bit signed integer        */
#endif


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


#define REG_SETBITS(reg, bitsmask, bits) (regWrite32(reg, (regRead32(reg) & ~(bitsmask)) | (bits)))
#define REG_CLRBITS(reg, bits) (regWrite32(reg, regRead32(reg) & ~(bits)))

#define LED_OEN(x)		do { 		\
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
													} while(0)
									/* input enable */
#define LED_IEN(x)		do { 		\
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
													} while(0)





#ifdef TR068_LED
int internet_led_on = 0;	/* 1:IP Connected; 0:No IP Connected*/
int internet_trying_led_on = 0;  /* 1:adsl up and trying to get WAN IP; 0:adsl not up or can't found defaule router*/

EXPORT_SYMBOL(internet_led_on);
EXPORT_SYMBOL(internet_trying_led_on);
#endif

#ifdef TCSUPPORT_USB_HOST_LED
int usb_dev_status = 0 ;
unsigned int usb_phyport_status = 0 ;

extern void (*Usb_Led_Flash_Op_hook)(unsigned int opmode ,unsigned int phyport);
#if KERNEL_3_18_21
extern int pre_usb_state[2];
#endif
#endif

#if defined(TCSUPPORT_XPON_LED)
static int led_xpon_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data);
static int led_xpon_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data);
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

void led_oen(uint8 led_no){
	LED_OEN(led_no);
}

void led_ien(uint8 led_no){
	LED_IEN(led_no);
}

EXPORT_SYMBOL(led_oen);
EXPORT_SYMBOL(led_ien);


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
	if(val > 3)
#else
	if(val > 2)
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


int led_init_others(void)
{
	struct proc_dir_entry *xpon_proc;
	
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


#if defined(TCSUPPORT_XPON_LED)
	/* xpon led */
	xpon_proc = create_proc_entry("tc3162/led_xpon", 0, NULL);
	xpon_proc->read_proc = led_xpon_read_proc;
	xpon_proc->write_proc = led_xpon_write_proc;
#endif	

	return 0;
}


void  led_exit_others(void)
{
#if defined(TCSUPPORT_XPON_LED)
	remove_proc_entry("tc3162/led_xpon", NULL);
#endif

#ifdef TCSUPPORT_USB_HOST_LED
	rcu_assign_pointer(Usb_Led_Flash_Op_hook, NULL);
#endif

}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Legacy LED driver");