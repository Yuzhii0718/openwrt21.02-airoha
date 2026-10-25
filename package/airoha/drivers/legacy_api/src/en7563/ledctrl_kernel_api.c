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
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include "ledctrl_kernel_api.h"
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include <cpu/pkgid.h>
#include <linux/of.h>
#include <linux/property.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif

#define LED_MAX_NO			107
#define puts(s)				printk("%s", s)

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

/*--------------- Added for led.conf support --------------------*/
#define	LED_CONFIG_PATH			"/userfs/led.conf"

//LED Modes
#define LED_MODE_NOT_USED			0
#define LED_MODE_ONOFF				1
#define LED_MODE_BLINK				2
#define LED_MODE_NOACT				3
#define LED_MODE_PULSE				4
#define LED_MODE_INPUT				5

//LED Speed Modes 
#define LED_SPEED_FAST			0
#define LED_SPEED_MEDIUM		1
#define LED_SPEED_SLOW			2
#define LED_SPEED_VERY_SLOW		3

//LED Speeds in ms
#define LED_SPEED_FAST_BNO			200
#define LED_SPEED_MEDIUM_BNO		500
#define LED_SPEED_SLOW_BNO			1000
#define LED_SPEED_VERY_SLOW_BNO		2000

#define LED_MODE_MASK			0xf
#define LED_MODE(x)			((x) & LED_MODE_MASK)


static int led_ctrl_debug_flag = 0;
static int led_test_mode_flag = 0;

typedef struct {
	uint8 led_no;
	uint8 gpio;
	uint8 mode;
	uint16 speed;
	uint8 onoff;
}ledctrl_t;

static ledctrl_t ledDefCtrl[LED_MAX_NO] = {
	/* empty configuration */
{0,       0,       0,       0},//0
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       0,       0,       0},
{0,       1,       0,       0},//31
{0,       0,       0,       0},//32
{0,       0,       0,       0},//33
{0,       0,       0,       0},//34
{0,       0,       0,       0},//35
{0,       0,       0,       0},//36
{0,       0,       0,       0},//37
{0,       0,       0,       0},//38
{0,       0,       0,       0},//39
{0,       0,       0,       0},//40
{0,       0,       0,       0},//41
{25,	  LED_MODE_ONOFF,		0,		 1},//42 Tx_disable
{0,       0,       0,       0},//43
{0,       0,       0,       0},//44
{0,       0,       0,       0},//45
{0,       0,       0,       0},//46
{0,       0,       0,       0},//47
{0,       0,       0,       0},//48
{0,       0,       0,       0},//49
{0,       0,       0,       0},//50
{0,       0,       0,       0},//51
{0,       0,       0,       0},//52
{0,       0,       0,       0},//53
{0,       0,       0,       0},//54
{0,       0,       0,       0},//55
{0,       0,       0,       0},//56 SYS Reset
{0,       0,       0,       0},//57
{0,       0,       0,       0},//58
{0,       0,       0,       0},//59
{0,       0,       0,       0},//60
{0,       0,       0,       0},//61
{0,       0,       0,       0},//62
{0,       0,       0,       0},//63
{0,       0,       0,       0},//64
{0, 	  0,	   0,       0},//65
{0,       0,       0,       0},//66
{0,       0,       0,       0},//67
{0,       0,       0,       0},//68
{0,       0,       0,       0},//69
{0,       0,       0,       0},//70
{0, 	  0,	   0,       0},//71
{0,       0,       0,       0},//72
{0, 	  0,	   0,       0},//73
{0, 	  0,	   0,       0},//74
{0, 	  0,	   0,       0},//75
{0,       0,       0,       0},//76
{0, 	  0,	   0,       0},//77
{0,       0,       0,       0},//78
{0,       0,       0,       0},//79
{0,       0,       0,       0},//80
{0,       0,       0,       0},//81
{0,       0,       0,       0},//82
{0, 	  0,	   0,       0},//83
{0,       0,       0,       0},//84
{0, 	  0,	   0,       0},//85
{0, 	  0,	   0,       0},//86
{0, 	  0,	   0,       0},//87
{0,       0,       0,       0},//88
{0, 	  0,	   0,       0},//89
{0,       0,       0,       0},//90
{0,       0,       0,       0},//91
{0,       0,       0,       0},//92
{0,       0,       0,       0},//93
{0,       0,       0,       0},//94
{0, 	  0,	   0,       0},//95
{0,       0,       0,       0},//96
{0, 	  0,	   0,       0},//97
{0, 	  0,	   0,       0},//98
{0, 	  0,	   0,       0},//99
{0, 	  0,	   0,       0},//100
{0, 	  0,	   0,       0},//101
{0, 	  0,	   0,       0},//102
{0, 	  0,	   0,       0},//103
{0, 	  0,	   0,       0},//104
{0, 	  0,	   0,       0},//105
{0, 	  0,	   0,       0},//106
};

/*----------------------------------------*/

extern struct list_head leds_list;

struct led_name_t {
	char *name;
	uint8 led_no;
};

struct led_info_map{
	uint8 led_no;
	struct led_classdev *led_cdev;
	struct led_ctrl_set_data *led_data;
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
};

#define isDEFAULT         0x0
typedef struct led_path
{
    uint8 chipid ;
    const char *path;
}led_path_m;

struct led_info_map led_cdev_map[LED_MAX_NO];

struct led_ctrl_set_data{
	struct led_classdev *led_cdev;
	int mode;  /*0: on/off; 1: blink*/
	int on_off;
	unsigned long delay_on;
	unsigned long delay_off;
	struct work_struct work;
};

static void led_ctrl_set_wq(struct work_struct *work){
	struct led_ctrl_set_data *led_data = container_of(work, struct led_ctrl_set_data, work);
	
	/*turn off*/
	if (led_data->on_off == 0) {
		if(led_ctrl_debug_flag)
			printk("==%s turn off==\n", __func__);
		if (test_bit(LED_BLINK_SW, &(led_data->led_cdev->work_flags)))
			led_stop_software_blink(led_data->led_cdev);	
		led_set_brightness(led_data->led_cdev, 0);
	}
	/*blink mode*/
	else if(led_data->mode == 1 && ((led_data->delay_on)!= 0 || (led_data->delay_off) != 0)){
		if(led_ctrl_debug_flag)
			printk("==%s blink==\n", __func__);
		led_blink_set(led_data->led_cdev, &(led_data->delay_on), &(led_data->delay_off));
	}
	/*onoff mode and turn on*/
	else if(led_data->mode == 0){
		if(led_ctrl_debug_flag)
			printk("==%s turn on==\n", __func__);
		if(test_bit(LED_BLINK_SW, &(led_data->led_cdev->work_flags)))
			led_stop_software_blink(led_data->led_cdev);
		led_set_brightness(led_data->led_cdev, 1);
	}
}

/*_____________________________________________________________________________
**      function name: led_test_mode_write_proc
**		descriptions:
**            When echo "1" > /proc/legacy_api/led_test_mode, this function will
**            					enable the test mode so that ledTurnOn, ledTurnOff, 
**								ledBlinkOn and ledBlinkOn2 will be by passed for 
**								testing drivers like gpio, led using sysfs/chardev 
**								interface.								   
**            When echo "0" > /proc/legacy_api/led_test_mode, this function will
**            					disable the test mode.
**____________________________________________________________________________
*/
static int led_test_mode_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32];
	char test_status[32] = {0};
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	val_string[count] = '\0';
	
	if (sscanf(val_string, "%s", test_status) != 1) {
		printk("usage: <1/0>\n");
		return count;
	}
	
	if(strcasecmp("1", test_status) == 0) {
		led_test_mode_flag = 1;
		printk("Led test mode enabled, value = %d \n", led_test_mode_flag);
	} 
	else if(strcasecmp("0", test_status) == 0) {
		led_test_mode_flag = 0;
		printk("Led test mode disabled, value = %d \n", led_test_mode_flag);
	}
	else {
		printk("error input, echo <1/0>  > /proc/legacy_api/led_test_mode \n");
	}
			
	return count;
}

/*_____________________________________________________________________________
**      function name: blink_test_write_proc
**		speed : 0 -> Fast, 1-> MEDIUM, 2-> SLOW, 4-> VERY_SLOW
**      descriptions:
**            When echo "10 speed 1" > /proc/legacy_api/blink_test, this function will
**            enable blinking on the led_no 10 with MEDIUM speed.
**            When echo "10 on 200 off 200" > /proc/legacy_api/blink_test, this function will
**            enable blinking on the led_no 10 with delay_on speed = 200 and delay_off speed = 200.
**____________________________________________________________________________
*/
static int blink_test_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{

	char val_string[32];
	char led_status1[32] = {0};
	char led_status2[32] = {0};
	int led_no = -1;
	int speed = -1;
	int delay_on;
	int delay_off;
	int num_args = 0;
	int i;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	
	val_string[count] = '\0';
	
	for(i=0; i<count; i++){
		if(val_string[i] == ' '){
			num_args++;
		}
	}
	
	num_args++;
	
	if(num_args == 3){
		if (sscanf(val_string, "%d %s %d", &led_no, led_status1, &speed) != 3) {
			printk("usage: <led_no> speed <0/1/2/3>\n");
			return count;
		}
	}
	else if(num_args==5){
		if (sscanf(val_string, "%d %s %d %s %d", &led_no, led_status1, &delay_on, led_status2, &delay_off) != 5) {
			printk("usage: <led_no> on <delay_no> off <delay_off>\n");
			return count;
		}
	}

	printk("LED NO = %d \n", led_no);

	if( led_no < 0 || led_no >= LED_MAX_NO) {
		printk("led_no exceeds %d.\n", LED_MAX_NO - 1);
		return -EINVAL;
	}	
	else if(strcasecmp("speed", led_status1) == 0) {	
		ledBlinkOn(led_no, speed);
		printk("blink on LED %d.\n", led_no);
	} 
	else if((strcasecmp("on", led_status1) == 0) && (strcasecmp("off", led_status2) == 0)) {
		ledBlinkOn2(led_no, delay_on, delay_off);
		printk("blink on LED %d.\n", led_no);
	}
	else {
		printk("error input\n");
	}

	printk("BLINK test finish.\n");
			
	return count;
}


/*_____________________________________________________________________________
**      function name: led_test_write_proc
**      descriptions:
**            When echo "10 on" > /proc/legacy_api/led_test, this function will
**            turn on the led_no 10 with work_queue.
**            When echo "10 off" > /proc/legacy_api/led_test, this function will
**            turn off the led_no 10 with work_queue.
**            When echo "10 onRT" > /proc/legacy_api/led_test, this function will
**            turn on the led_no 10 without work_queue.
**            When echo "10 offRT" > /proc/legacy_api/led_test, this function will
**            turn off the led_no 10 without work_queue.
**____________________________________________________________________________
*/

static int led_test_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32];
	char led_status[32];
	int led_no;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	
	val_string[count] = '\0';
	
	if (sscanf(val_string, "%d %s", &led_no, led_status) != 2) {
		printk("usage: <led_no> <on/off/onRT/offRT>\n");
		return count;
	}

	printk("LED NO = %d", led_no);

	if(led_no < 0 ||led_no >= LED_MAX_NO) {
		printk("led_no exceeds %d.\n", LED_MAX_NO - 1);
		return -EINVAL;
	}	
	else if(strcasecmp("on", led_status) == 0) {	
		ledTurnOn(led_no);
		printk("turn on LED %d.\n", led_no);
	} 
	else if(strcasecmp("off", led_status) == 0) {
		ledTurnOff(led_no);
		printk("turn off LED %d.\n", led_no);
	}
	else if(strcasecmp("onRT", led_status) == 0) {	
		ledTurnOnRT(led_no);
		printk("turn onRT LED %d.\n", led_no);
	}
	else if(strcasecmp("offRT", led_status) == 0) {	
		ledTurnOffRT(led_no);
		printk("turn offRT LED %d.\n", led_no);
	}
	else {
		printk("error input, echo <led_no> <on/off/onRT/offRT>  > /proc/legacy_api/led_test \n");
	}

	printk("LED test finish.\n");
			
	return count;
	
}

/*______________________________________________________________________________
**	ledBlinkOn2
**
**	descriptions: Make the led with led_no blink with delay on and delay off time in ms
**	parameters: led_no, delay_on, dealy_off
**	called by: Various other modules
**	call: led_blink_set 
**____________________________________________________________________________*/

void ledBlinkOn2(uint8 led_no, unsigned long delay_on, unsigned long delay_off){
	if(1 == led_test_mode_flag)
		return;
		
	struct led_classdev *led_cdev = led_cdev_map[led_no].led_cdev;
	struct led_ctrl_set_data *led_data = led_cdev_map[led_no].led_data;
	
	if(led_cdev == NULL){
		printk("Error, led_no = %d : No such led device in device tree\n", led_no);
		return;
	}
	
	if(led_ctrl_debug_flag)
		printk("=========ledBlinkOn2 led_no = %d========\n", led_no);
	
	led_data->led_cdev = led_cdev;
	led_data->mode = 1;
	led_data->on_off = 1;
	led_data->delay_on = delay_on;
	led_data->delay_off = delay_off;

	if (schedule_work(&led_data->work) == 0){
		if(led_ctrl_debug_flag)
			printk("schedule work for led [%d] in ledBlinkOn2 failed\n", led_no);
	}
}

/*______________________________________________________________________________
**	ledBlinkOn
**
**	descriptions: Make the led with led_no blink according to speed modes
**                 0 -> Fast, 1-> MEDIUM, 2-> SLOW, 4-> VERY_SLOW
**	parameters: led_no, speed <0/1/2/3>
**	called by: Various other modules
**	call: led_blink_set 
**____________________________________________________________________________*/

void ledBlinkOn(uint8 led_no, int speed){
	if(1 == led_test_mode_flag)
		return;
	
	struct led_classdev *led_cdev = led_cdev_map[led_no].led_cdev;
	struct led_ctrl_set_data *led_data = led_cdev_map[led_no].led_data;

	if(led_cdev == NULL){
		printk("Error, led_no = %d : No such led device in device tree\n", led_no);
		return;
	}
	
	if(led_ctrl_debug_flag)
		printk("=========ledBlinkOn led_no = %d========\n", led_no);
	
	led_data->led_cdev = led_cdev;
	led_data->on_off = 1;
	led_data->mode = 1;
	
	if(speed == LED_SPEED_FAST){
		led_data->delay_on = LED_SPEED_FAST_BNO;
		led_data->delay_off = LED_SPEED_FAST_BNO;
	}
	else if(speed == LED_SPEED_MEDIUM){
		led_data->delay_on = LED_SPEED_MEDIUM_BNO;
		led_data->delay_off = LED_SPEED_MEDIUM_BNO;
	}
	else if(speed == LED_SPEED_SLOW){
		led_data->delay_on = LED_SPEED_SLOW_BNO;
		led_data->delay_off = LED_SPEED_SLOW_BNO;
	}
	else if(speed == LED_SPEED_VERY_SLOW){
		led_data->delay_on = LED_SPEED_VERY_SLOW_BNO;
		led_data->delay_off = LED_SPEED_VERY_SLOW_BNO;
	}
	else{
		printk("Error : Speed mode is invalid \n");
	}
	
	if (schedule_work(&led_data->work) == 0){
		if(led_ctrl_debug_flag)
			printk("schedule work for led [%d] in ledBlinkOn failed\n", led_no);
	}
	
}

EXPORT_SYMBOL(ledBlinkOn);
EXPORT_SYMBOL(ledBlinkOn2);

/*______________________________________________________________________________
**	ledTurnOff
**
**	descriptions: Turns off the led corrosponding to led_no with workqueue
**	parameters: led_no
**	called by: Various other modules
**	call: led_set_brightness 
**____________________________________________________________________________*/

void ledTurnOff(uint8 led_no)
{		
	if(1 == led_test_mode_flag)
		return;
	
	struct led_classdev *led_cdev = led_cdev_map[led_no].led_cdev;
	struct led_ctrl_set_data *led_data = led_cdev_map[led_no].led_data;
	
	if(led_cdev == NULL || led_data == NULL){
		//printk("%s: led_no = %d Error : No such led device in device tree\n", __func__, led_no);
	}
	else{
		if(led_no != 1 && led_no != 42 && led_no != 33){
			if(led_ctrl_debug_flag)
				printk("=========ledTurnOff led_no = %d========\n", led_no);
		}

		led_data->mode = 0;
		led_data->on_off= 0;
		led_data->led_cdev = led_cdev;

		if (schedule_work(&led_data->work) == 0){
			if(led_ctrl_debug_flag)
				printk("schedule work for led [%d] in ledTurnOff failed\n", led_no);
		}
	}
}

/*______________________________________________________________________________
**	ledTurnOn
**
**	descriptions: Turns on the led corrosponding to led_no with workqueue
**	parameters: led_no
**	called by: Various other modules
**	call: led_set_brightness, led_blink_set 
**____________________________________________________________________________*/

__IMEM void ledTurnOn(uint8 led_no)
{
	if(1 == led_test_mode_flag)
		return;
	
	//if(led_no != 1 && led_no != 42 && led_no != 33 && led_no != 66 && led_no != 67){
		//printk("========================================led_no = %d===========================\n", led_no);
	//}
	
	struct led_classdev *led_cdev = led_cdev_map[led_no].led_cdev;
	ledctrl_t *pledctl = &ledDefCtrl[led_no];
	struct led_ctrl_set_data *led_data = led_cdev_map[led_no].led_data;
	
	if((LED_MODE_NOT_USED == pledctl->mode) || (LED_MODE_INPUT== pledctl->mode)){
		//printk("%s: led_no = %d Error : Mode = not used or input\n", __func__, led_no);
		return;
	}
	
	if(led_cdev == NULL || led_data == NULL){
		//printk("%s: led_no = %d Error : No such led device in device tree\n", __func__, led_no);
		return;
	}
	
	led_data->led_cdev = led_cdev;
	
	//Blink mode
	if(pledctl->mode == LED_MODE_BLINK){
		if(led_no != 1 && led_no != 42 && led_no != 33){
			if(led_ctrl_debug_flag)
				printk("=========ledTurnOn -> Blink_mode, led_no = %d========\n", led_no);
		}

		led_data->mode = 1;
		led_data->on_off = 1;		
		
		if(pledctl->speed == LED_SPEED_FAST){
			led_data->delay_on = LED_SPEED_FAST_BNO;
			led_data->delay_off = LED_SPEED_FAST_BNO;
		}
		else if(pledctl->speed == LED_SPEED_MEDIUM){
			led_data->delay_on = LED_SPEED_MEDIUM_BNO;
			led_data->delay_off = LED_SPEED_MEDIUM_BNO;
		}
		else if(pledctl->speed == LED_SPEED_SLOW){
			led_data->delay_on = LED_SPEED_SLOW_BNO;
			led_data->delay_off = LED_SPEED_SLOW_BNO;
		}
		else if(pledctl->speed == LED_SPEED_VERY_SLOW){
			led_data->delay_on = LED_SPEED_VERY_SLOW_BNO;
			led_data->delay_off = LED_SPEED_VERY_SLOW_BNO;
		}
		else{
			printk("%s: led_no = %d Error : Speed mode is invalid \n", __func__, led_no);
			return;
		}

		if (schedule_work(&led_data->work) == 0){
			if(led_ctrl_debug_flag)
				printk("schedule work for led [%d] in ledTurnOn blink failed\n", led_no);
		}
	}
	//Turn_on_off mode
	else if(pledctl->mode == LED_MODE_ONOFF){
		if(led_no != 1 && led_no != 42 && led_no != 33){
			if(led_ctrl_debug_flag)
				printk("=========ledTurnOn -> on_off_mode led_no = %d========\n", led_no);
		}

		led_data->mode = 0; 
		led_data->on_off = 1;
		if (schedule_work(&led_data->work) == 0){
			if(led_ctrl_debug_flag)
				printk("schedule work for led [%d] in ledTurnOn_on_off failed\n", led_no);
		}
	}	
	else{
		printk("%s: led_no = %d Error : No such mode\n", __func__, led_no);
		return;
	}
}

EXPORT_SYMBOL(ledTurnOn);
EXPORT_SYMBOL(ledTurnOff);


void ledTurnOffRT(uint8 led_no)
{	
	
	if(1 == led_test_mode_flag)
		return;
	
	struct led_classdev *led_cdev = led_cdev_map[led_no].led_cdev;
	
	if(led_cdev == NULL){
		//printk("%s: led_no = %d Error : No such led device in device tree\n", __func__, led_no);
	}
	else{
		if(led_ctrl_debug_flag)
			printk("=========ledTurnOffRT led_no = %d========\n", led_no);

		led_set_brightness(led_cdev, 0);
	}
}

/*______________________________________________________________________________
**	ledTurnOnRT
**
**	descriptions: Turns on the led corrosponding to led_no without workqueue
**	parameters: led_no
**	called by: Various other modules
**	call: led_set_brightness, led_blink_set 
**____________________________________________________________________________*/


__IMEM void ledTurnOnRT(uint8 led_no)
{
	if(1 == led_test_mode_flag)
		return;
	
	struct led_classdev *led_cdev = led_cdev_map[led_no].led_cdev;
	ledctrl_t *pledctl = &ledDefCtrl[led_no];
	
	if((LED_MODE_NOT_USED == pledctl->mode) || (LED_MODE_INPUT== pledctl->mode)){
		//printk("%s: led_no = %d Error : Mode = not used or input\n", __func__, led_no);
		return;
	}
	
	if(led_cdev == NULL){
		//printk("%s: led_no = %d Error : No such led device in device tree\n", __func__, led_no);
		return;
	}
	
	if(led_ctrl_debug_flag)
		printk("=========ledTurnOnRT led_no = %d========\n", led_no);

	
	//Blink mode
	if(pledctl->mode == LED_MODE_BLINK){
		printk ("do not use ledTurnOnRT to set blink\n");
	}
	//Turn_on_off mode
	else if(pledctl->mode == LED_MODE_ONOFF){
		led_set_brightness(led_cdev, 1);
	}	
	else{
		printk("%s: led_no = %d Error : No such mode\n", __func__, led_no);
		return;
	}
}


EXPORT_SYMBOL(ledTurnOnRT);
EXPORT_SYMBOL(ledTurnOffRT);


uint8 ledGetMode(uint8 led_no)
{
	return LED_MODE(ledDefCtrl[led_no].mode);
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

EXPORT_SYMBOL(ledSetMode);
EXPORT_SYMBOL(ledGetGpio);


static char* led_conf_path_find(void)
{
    uint8 i;
    char *p_path = NULL;
    uint8 arr_len = 0;
    struct file *srcf;
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
           {isDEFAULT,"/userfs/led.conf"}/*must last element*/
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
						ledDefCtrl[set_info[0]].gpio  = set_info[1];
						ledDefCtrl[set_info[0]].mode  = set_info[2];
						ledDefCtrl[set_info[0]].speed = set_info[3];
						ledDefCtrl[set_info[0]].onoff = set_info[4];
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
	
int led_init(void)
{
	struct proc_dir_entry *led_test_mode_proc;
	struct proc_dir_entry *led_test_proc;
	struct proc_dir_entry *blink_test_proc;

	struct led_classdev *temp_cdev;
	const char *label = NULL;
	int i = 0;
	int array_size = (sizeof(led_name_map) / sizeof(led_name_map[0]));
	
	//Reading led.conf
	led_read_config();
	
	
	//Initializing led_cdev_map
	for(i=0; i<LED_MAX_NO; i++){
		led_cdev_map[i].led_no = i;
		led_cdev_map[i].led_cdev = NULL;
		led_cdev_map[i].led_data = NULL;
	}
	
	list_for_each_entry(temp_cdev, &leds_list, node){
		fwnode_property_read_string(temp_cdev->dev->fwnode, "label", &label);
		for(i=0; i<array_size; i++){
			if(strcmp(label, led_name_map[i].name) == 0){
				led_cdev_map[led_name_map[i].led_no].led_cdev = temp_cdev;
				led_cdev_map[led_name_map[i].led_no].led_data = (struct led_ctrl_set_data*)kmalloc(sizeof(*(led_cdev_map[led_name_map[i].led_no].led_data)), GFP_KERNEL);
				INIT_WORK(&(led_cdev_map[led_name_map[i].led_no].led_data->work), led_ctrl_set_wq);
				if(led_name_map[i].led_no != LED_PHY_TX_POWER_DISABLE)
					ledTurnOff(led_name_map[i].led_no);
			}
		}	
	}
	
	led_test_mode_proc = create_proc_entry("legacy_api/led_test_mode", 0, NULL);
	if(led_test_mode_proc) {
		led_test_mode_proc->write_proc = led_test_mode_write_proc;
	}
	
	led_test_proc = create_proc_entry("legacy_api/led_test", 0, NULL);
	if(led_test_proc) {
		led_test_proc->write_proc = led_test_write_proc;
	}
	
	blink_test_proc = create_proc_entry("legacy_api/blink_test", 0, NULL);
	if(blink_test_proc) {
		blink_test_proc->write_proc = blink_test_write_proc;
	}
	
	ledTurnOn(LED_TR68_PWR_BOOTED);
	ledTurnOnRT(LED_PHY_TX_POWER_DISABLE);
	led_init_others();

	return 0;
}

void led_exit(void)
{
	int i;
	
	for(i=0; i<LED_MAX_NO; i++){
		if(led_cdev_map[i].led_data != NULL){
			kfree(led_cdev_map[i].led_data);
			led_cdev_map[i].led_data = NULL;
		}
	}
	
	remove_proc_entry("legacy_api/led_test_mode", NULL);
	remove_proc_entry("legacy_api/led_test", NULL);
	remove_proc_entry("legacy_api/blink_test", NULL);
	
	led_exit_others();
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Legacy LED driver");