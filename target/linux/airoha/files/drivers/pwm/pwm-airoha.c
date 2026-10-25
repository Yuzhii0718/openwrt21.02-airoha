// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2021-2023 Airoha Inc.

/*
* PWM implementation based on Airoha SDK
* 
* Author: Dishant Dhillon <dishant.dhillon@airoha.com>
*
*/

#include<linux/err.h> /* ENOMEM */
#include<linux/kernel.h>
#include<linux/module.h>
#include <linux/io.h>
#include<linux/of.h>
#include<linux/pwm.h> /* pwm_chip */
#include<linux/slab.h> /* kzalloc */
#include<linux/of_platform.h>
#include <linux/proc_fs.h>

#include "../gpio/gpiolib.h" /* gpio_desc */


/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define MAX_GPOUP_MEMBER	5
#define MAX_GEN_WAVE_NUM	8
#define REG_SECTIONS	3
#define MAX_GPIO_PINS	52
#define MAX_PERIOD	1000000000
#define MIN_PERIOD	4000000
#define GPIO_PIN_NOT_SUPPORTED1	32
#define GPIO_PIN_NOT_SUPPORTED2	33
#define GPIO_PIN_NOT_SUPPORTED3	34
#define GPIO_PIN_NOT_SUPPORTED4	35
#define BANK_SIZE	32
#define PWM_PIN_GROUPS	16

static enum reg_addr_section{
	SECTION1,
	SECTION2,
	SECTION3
};

struct pwm_airoha_chip{
	struct pwm_chip chip;
	void __iomem *base_addr[REG_SECTIONS];
	uint8_t num;
	uint8_t pin_mode;
	uint8_t work_mode;
	uint8_t duration_mode;
	uint8_t waveGenNum;
	uint8_t frequency_div;
	uint8_t pwm_mode;
	uint16_t time_interval;
	uint8_t variation;
	uint16_t manual_duty;
};


static inline struct pwm_airoha_chip *
to_pwm_airoha_chip(struct pwm_chip *chip)
{
	return container_of(chip, struct pwm_airoha_chip, chip);
}

/************************************************************************
*                G P I O L I B     A P I
*************************************************************************
*/
extern struct gpio_desc *gpio_to_desc(unsigned gpio);
extern int gpiod_direction_output(struct gpio_desc *desc, int value);
extern int gpiod_direction_input(struct gpio_desc *desc);
extern int gpiod_get_direction(struct gpio_desc *desc);


/************************************************************************
*            R E G I S T E R      A d d r e s s e s 
*************************************************************************
*/
#define GPIO_FLASH_MODE_CFG 0x00   /*0x1fbf0234*/
#define GPIO_RSP_MODE_CFG 0x04     /*0x1fbf0238*/
#define GPIO_FLASH_PRD_SET0 0x08   /*0x1fbf023C*/
#define GPIO_FLASH_PRD_SET1 0x0C   /*0x1fbf0240*/
#define GPIO_FLASH_PRD_SET2 0x10   /*0x1fbf0244*/
#define GPIO_FLASH_PRD_SET3 0x14   /*0x1fbf0248*/
#define GPIO_FLASH_MAP_CFG0 0x18   /*0x1fbf024C*/
#define GPIO_FLASH_MAP_CFG1 0x1C   /*0x1fbf0250*/
#define GPIO_FLASH_MODE_CFG_EXT 0x00   /*0x1fbf0268*/
#define CYCLE_CFG_VALUE0 0x00   /*0x1fbf0298*/
#define CYCLE_CFG_VALUE1 0x04   /*0x1fbf029C*/
#define AUTO_FLASH_STEP1_0 0x08   /*0x1fbf02A0*/
#define AUTO_FLASH_STEP3_2 0x0C   /*0x1fbf02A4*/
#define AUTO_FLASH_STEP5_4 0x10   /*0x1fbf02A8*/
#define AUTO_FLASH_STEP7_6 0x14   /*0x1fbf02AC*/
#define AUTO_FLASH_AMP3_0 0x18   /*0x1fbf02B0*/
#define AUTO_FLASH_AMP7_4 0x1C   /*0x1fbf02B4*/
#define AUTO_FLASH_EN 0x20   /*0x1fbf02B8*/


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
/* PWM use constants */
static int gen_wave_state[MAX_GEN_WAVE_NUM] = {0};
static int gpio_OE_state[2*BANK_SIZE];
static int map_group_member[PWM_PIN_GROUPS][MAX_GPOUP_MEMBER] = {
	{0, -1, -1, -1, -1},
	{1, -1, -1, -1, -1},
	{2, -1, -1, -1, -1},
	{3, -1, -1, -1, -1},
	{4, -1, -1, -1, -1},
	{5, -1, -1, -1, -1},
	{6, -1, -1, -1, -1},
	{7, -1, -1, -1, -1},
	{8, 16, 24, 36, 44},
	{9, 17, 25, 37, 45},
	{10, 18, 26, 38, 46},
	{11, 19, 27, 39, 47},
	{12, 20, 28, 40, 48},
	{13, 21, 29, 41, 49},
	{14, 22, 30, 42, 50},
	{15, 23, 31, 43, 51}
};

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
static u32 get_pwm_data(void __iomem *reg)
{	
    return readl(reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_pwm_data(void __iomem *reg, u32 val)
{
    writel(val, reg);
}

/* gpio_map_pwmgpio only for RSP_mode, flash_enable, mapping_gen_wave
 * you might get in trouble if you use for other function
 */
 
static int8_t gpio_map_pwmgpio(u8 gpio_pin)
{
	int8_t ret;
	switch (gpio_pin){
		case 0 ... 15 :
			ret = gpio_pin;
			break;
		case 16 ... 23 :
			ret = (gpio_pin & 15) + 8;
			break;
		case 24 ... 31 :
			ret = (gpio_pin & 15);
			break;
		case 36 ... 43 :
			ret = gpio_pin - 28;
			break;
		case 44 ... 51 :
			ret = gpio_pin - 36;
			break;
		default :
			printk("[pwm]WARNING : this gpio %u cannot map\n", gpio_pin);
			ret = -1;
			break;
	}
	return ret;
}

/* Below functions are used to get current value of different registers 
 * and set their value according to data given in parameter 
 */

static u32 gpiopwm_get_gpio_flash_mode_cfg(u8 gpioPinNum, struct pwm_airoha_chip *pc)
{
    return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MODE_CFG);
}

static void gpiopwm_set_gpio_flash_mode_cfg(u8 gpioPinNum, u32 data, struct pwm_airoha_chip *pc)
{
    set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MODE_CFG, data);
}

static u32 gpiopwm_get_gpio_rsp_mode_cfg(u8 gpioPinNum, struct pwm_airoha_chip *pc)
{
    return get_pwm_data(pc->base_addr[SECTION1] + GPIO_RSP_MODE_CFG);
}

static void gpiopwm_set_gpio_rsp_mode_cfg(u8 gpioPinNum, u32 data, struct pwm_airoha_chip *pc)
{
    set_pwm_data(pc->base_addr[SECTION1] + GPIO_RSP_MODE_CFG, data);
}

static u32 gpiopwm_get_gpio_flash_map_cfg(u8 gpioPinNum, struct pwm_airoha_chip *pc)
{
	u8 mapped_gpio = gpio_map_pwmgpio(gpioPinNum);
    switch(mapped_gpio){
        case 0 ... 7:
            return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG0);
        case 8 ... 15:
            return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG1);
    }
	printk("[pwm]Warning:unknow gpioPinNum:%d\n", gpioPinNum);
	return 0;
}

static void gpiopwm_set_gpio_flash_map_cfg(u8 gpioPinNum, u32 data, struct pwm_airoha_chip *pc)
{
	u8 mapped_gpio = gpio_map_pwmgpio(gpioPinNum);
    switch(mapped_gpio){
        case 0 ... 7:
            set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG0, data);
            break;
        case 8 ... 15:
            set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG1, data);
            break;
    }
}

static u32 gpiopwm_get_cycle_cfg_value(u8 waveGenNum, struct pwm_airoha_chip *pc)
{
    switch(waveGenNum){
        case 0 ... 3:
            return get_pwm_data(pc->base_addr[SECTION3] + CYCLE_CFG_VALUE0);
        case 4 ... 7:
            return get_pwm_data(pc->base_addr[SECTION3] + CYCLE_CFG_VALUE1);
    }
	printk("[pwm]Warning:unknow waveGenNum:%d\n", waveGenNum);
	return 0;
}

static void gpiopwm_set_cycle_cfg_value(u8 waveGenNum, u32 data, struct pwm_airoha_chip *pc)
{
    switch(waveGenNum){
        case 0 ... 3:
            set_pwm_data(pc->base_addr[SECTION3] + CYCLE_CFG_VALUE0, data);
            break;
        case 4 ... 7:
            set_pwm_data(pc->base_addr[SECTION3] + CYCLE_CFG_VALUE1, data);
            break;
    }
}

static u32 gpiopwm_get_auto_flash_step(u8 waveGenNum, struct pwm_airoha_chip *pc){

    switch(waveGenNum){
        case 0 ... 1:
            return get_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP1_0);
        case 2 ... 3:
            return get_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP3_2);
        case 4 ... 5:
            return get_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP5_4);
        case 6 ... 7:
            return get_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP7_6);		
    }
	printk("[pwm]Warning:unknow waveGenNum:%d\n", waveGenNum);
	return 0;
}

static void gpiopwm_set_auto_flash_step(u8 waveGenNum, u32 data, struct pwm_airoha_chip *pc){

    switch(waveGenNum){
        case 0 ... 1:
            set_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP1_0, data);
            break;
        case 2 ... 3:
            set_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP3_2, data);
            break;
        case 4 ... 5:
            set_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP5_4, data);
            break;
        case 6 ... 7:
            set_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_STEP7_6, data);
            break;
    }
}

static u32 gpiopwm_get_auto_flash_amp(u8 waveGenNum, struct pwm_airoha_chip *pc)
{
    switch(waveGenNum){
        case 0 ... 3:
            return get_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_AMP3_0);
        case 4 ... 7:
            return get_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_AMP7_4);
    }
	printk("[pwm]Warning:unknow waveGenNum:%d\n", waveGenNum);
	return 0;
}

static void gpiopwm_set_auto_flash_amp(u8 waveGenNum, u32 data, struct pwm_airoha_chip *pc)
{
    switch(waveGenNum){
        case 0 ... 3:
            set_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_AMP3_0, data);
            break;
        case 4 ... 7:
            set_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_AMP7_4, data);
            break;
    }
}

/* get the value of AUTO_FLASH register */

static u32 gpiopwm_get_auto_flash_en(struct pwm_airoha_chip *pc)
{
    return get_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_EN);
}

/* set AUTO_FLASH register */

static void gpiopwm_set_auto_flash_en(u8 waveGenNum, u32 data, struct pwm_airoha_chip *pc)
{
    set_pwm_data(pc->base_addr[SECTION3] + AUTO_FLASH_EN, data);
}

/* get the current state of FLASH_PRD registers */

static u32 gpiopwm_get_hiloprd(u8 waveGenNum, struct pwm_airoha_chip *pc)
{
    switch(waveGenNum){
        case 0 ... 1:
            return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET0);		
        case 2 ... 3:
            return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET1);	
        case 4 ... 5:
            return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET2);	
        case 6 ... 7:
            return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET3);	
    }
	printk("[pwm]Warning:unknown waveGenNum:%d\n", waveGenNum);
	return 0;
}

/* set FLASH_PRD registers */

static void gpiopwm_set_hiloprd(u8 waveGenNum, u32 data, struct pwm_airoha_chip *pc)
{
	switch(waveGenNum){
		case 0 ... 1:
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET0, data);
			break;
		case 2 ... 3:
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET1, data);
			break;
		case 4 ... 5:
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET2, data);
			break;
		case 6 ... 7:
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_PRD_SET3, data);
			break;
		default:
			printk("[pwm]Warning:unknown waveGenNum:%d\n", waveGenNum);
			break;
	}
}

static void gpio_reg_config(struct pwm_airoha_chip *pc, u32 (*get_func_p)(u8, struct pwm_airoha_chip *), void (*set_func_p)(u8, u32, struct pwm_airoha_chip *), 
    u16 data, u8 num_per_reg){
    
    uint32_t value_tmp = 0;	
    uint16_t bitMask = 0;
    uint8_t num = 0;
    
    if(get_func_p == &gpiopwm_get_gpio_flash_map_cfg || 
        get_func_p == &gpiopwm_get_gpio_rsp_mode_cfg ||
        get_func_p == &gpiopwm_get_gpio_flash_mode_cfg){
        num = pc->num;
    }else{
        num = pc->waveGenNum;
    }
    
    switch(num_per_reg){
        case 2:
            bitMask = 0xffff;
            break;
        case 4:
            bitMask = 0xff;
            break;
        case 8:
            bitMask = 0xf;
            break;
        case 32:
            bitMask = 0x1;
            break;
        default:
            printk("no action for num_per_reg %d\n", num_per_reg);
            break;
    }
    if(get_func_p == &gpiopwm_get_gpio_flash_map_cfg)
	    bitMask = 7;
    value_tmp = (*get_func_p)(num, pc);
    value_tmp = (value_tmp & (~(bitMask << ((BANK_SIZE/num_per_reg)*(num % num_per_reg)))));	
    (*set_func_p)(num, value_tmp | (data << ((BANK_SIZE/num_per_reg)*(num % num_per_reg))), pc);
}

/* make the output stae of OE register to Input mode */

static void pwm_OE_disable(u8 gpio_pin, struct gpio_desc *descriptor)
{
	int ret = gpiod_direction_input(descriptor);
	if(ret){
		printk("[pwm]Error\n");
	}
}

/* Enable the FLASH_MODE registers */

static void pwm_flash_mode_enable(u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	switch (gpio_pin){
		case 0 ... 15 :
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MODE_CFG,
					get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MODE_CFG) | 
					(1 << gpio_pin));
			break;
		case 16 ... 31 :
			set_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT,
					get_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT)
				       	| (1 << (gpio_pin & 15)));
			break;
		case 36 ... 51 :
			set_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT,
					get_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT)
					| (1 << (gpio_pin - 20)));
			break;
		default :
			break;
	}
}

/* Disable the FLASH_MODE registers */

static void pwm_flash_mode_disable(u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	switch (gpio_pin){
		case 0 ... 15 :
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MODE_CFG,
					get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MODE_CFG) &
					(~(1 << gpio_pin)));
			break;
		case 16 ... 31 :
			set_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT,
					get_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT)
					& (~(1 << (gpio_pin & 15))));
			break;
		case 36 ... 51 :
			set_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT,
					get_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT)
					& (~(1 << (gpio_pin - 20))));
			break;
		default :
			break;
	}
}

/* Enable the FLASH_MAP registers */

static void pwm_flash_map_enable(u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	int8_t mapped_gpio = gpio_map_pwmgpio(gpio_pin);
	switch (mapped_gpio){
		case 0 ... 7 :
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG0, 
					get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG0)
					| ((uint8_t)8 << (mapped_gpio << 2)));
			break;
		case 8 ... 15 :
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG1,
					get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG1)
					| (8 << (((uint8_t)mapped_gpio - 8)
						       	<< 2)));
			break;
		default :
			break;
	}
}

/* get the current state of FLASH_MODE register */

static int8_t pwm_flash_mode_state(u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	switch (gpio_pin){
		case 0 ... 15 :
			return (get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MODE_CFG) 
					>> gpio_pin) & 1;
		case 16 ... 31 :
			return (get_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT)
			       	>> (gpio_pin-16)) & 1;
		case 36 ... 51 :
			return (get_pwm_data(pc->base_addr[SECTION2] + GPIO_FLASH_MODE_CFG_EXT)
			       	>> (gpio_pin - 20)) & 1;
		default :
			break;
	}
	return -1;
}

/* Unset the value of FLASH_MAP registers */

static void pwm_flash_map_disable(u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	int8_t mapped_gpio = gpio_map_pwmgpio(gpio_pin);
	switch (mapped_gpio){
		case 0 ... 7 :
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG0,
					get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG0)
					& ~(8 << ((uint8_t)mapped_gpio 
							<< 2)));
			break;
		case 8 ... 15 :
			set_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG1,
					get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG1)
					& ~(8 << (((uint8_t)mapped_gpio -8)
							<< 2)));
			break;
		default :
			break;
	}
}

/* get the current direction of Output Enable register */

static int8_t pwm_get_OE_state(struct gpio_desc *descriptor)
{
	return gpiod_get_direction(descriptor); 
}

/* get the current value of FLASH_MAP registers */

static int8_t pwm_get_flash_map_cfg(u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	int8_t mapped_gpio = gpio_map_pwmgpio(gpio_pin);
	switch (mapped_gpio) {
		case 0 ... 7 :
			return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG0) 
				>> ((uint8_t)mapped_gpio << 2) & 7;
		case 8 ... 15 :
			return get_pwm_data(pc->base_addr[SECTION1] + GPIO_FLASH_MAP_CFG1)
				>> (((uint8_t)mapped_gpio & 7) << 2) & 7;
		default :
			break;
	}
	return -1;
}

/* set rsp_mode register to 1 */

static void pwm_set_rsp_mode(u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	int8_t mapped_gpio = gpio_map_pwmgpio(gpio_pin);
	if (mapped_gpio >= 0)
	{
		set_pwm_data(pc->base_addr[SECTION1] + GPIO_RSP_MODE_CFG, get_pwm_data(pc->base_addr[SECTION1] + GPIO_RSP_MODE_CFG) | 
			(1 << (uint8_t)mapped_gpio));
	}
}

static void original_OE_state(u8 gpio_pin, struct gpio_desc *descriptor)
{
	gpio_OE_state[gpio_pin] = pwm_get_OE_state(descriptor);
}

/* check if flah mode state is already mapped for particular group */
static bool check_member_flash (u8 gpio_pin, struct pwm_airoha_chip *pc)
{
	int key, i = 0;
	key = (int)gpio_map_pwmgpio(gpio_pin);
	if (key < 0)
	{
		return false;
	}

	for(i = 0 ; i < MAX_GPOUP_MEMBER ; i++){
		if((map_group_member[key][i] != gpio_pin)
				&& (map_group_member[key][i] != -1) &&
				(pwm_flash_mode_state(map_group_member[key][i], pc)
				))
			return true;
	}
	return false;
}

/* compare period and duty_cycle with input gen_num's
 * if compared return true
 * else return false
 * */
static bool same_pwm_config(int wave_num ,uint32_t period, uint8_t low, struct pwm_airoha_chip *pc)
{
	uint8_t tmp_low;
	uint32_t tmp_period;
	tmp_period = gpiopwm_get_cycle_cfg_value(wave_num, pc);
	tmp_period = (tmp_period >> ((wave_num % 4)<<3)) & 0xff;
	tmp_low = ((gpiopwm_get_hiloprd(wave_num, pc) >>
				((wave_num % 2)<<4))) >> 8;
	if((tmp_period == period) && (tmp_low == low)){
		 return true;
	}
	return false;
}

/* select available gen_wave for pwm_airoha_config in pwm-airoha.c
 * if do not find return -1
 * */
static int8_t select_gen_wave(struct pwm_airoha_chip *pc, u8 gpio_pin, uint32_t period, uint8_t low)
{
	int wave_num = -1, i, mapped_gpio;

	mapped_gpio = gpio_map_pwmgpio(gpio_pin);
	/* searching for member in the same group that is using pwm 
	 * if found and identical configuration return wave_num
	 * if found but different configuration return -1
	 * */
	if(check_member_flash(gpio_pin, pc)){
		wave_num = (int)pwm_get_flash_map_cfg(gpio_pin, pc);
		if(same_pwm_config(wave_num, period, low, pc)){
			gen_wave_state[wave_num] += 1;
			return wave_num;
		}else{
			printk("[pwm]ERROR, OTHER GPIO IN THIS GROUP IS USING PWM\n");
			return -1;
		}
	}
	/* searching for same configuration pair
	 * if found, use same gen_wave*/
	for(i = 0 ; i < MAX_GEN_WAVE_NUM ; i++){
		if((gen_wave_state[i] > 0) && same_pwm_config(i, period, low, pc)){
			wave_num = i;
			gen_wave_state[wave_num] += 1;	
			return wave_num;
		}
	}

	/* searching for idle gen_wave */
	if(wave_num == -1){
		for(i = 0 ; i < MAX_GEN_WAVE_NUM ; i++){
			if(gen_wave_state[i] <= 0){
				wave_num = i;
				gen_wave_state[wave_num] += 1;
				return wave_num;
			}
		}
	}

	/*if none of the gen_wave can be configured, return -1 */
	if(wave_num < 0){
		printk("[pwm]ERROR, NO AVAILABLE GEN_WAVE\n");
		return -1;
	}
}

static void pwm_airoha_manual_config(struct pwm_airoha_chip *pc)
{
	gpio_reg_config(pc, &gpiopwm_get_gpio_flash_map_cfg,
			&gpiopwm_set_gpio_flash_map_cfg,
		       	pc->waveGenNum, 8);
	gpio_reg_config(pc, &gpiopwm_get_cycle_cfg_value,
			&gpiopwm_set_cycle_cfg_value,
		       	pc->frequency_div, 4);
	gpio_reg_config(pc, &gpiopwm_get_hiloprd, &gpiopwm_set_hiloprd,
		       	pc->manual_duty, 2);
}

static void airoha_gpio_config_pwm(struct pwm_airoha_chip *pc, uint8_t num, uint8_t waveGenNum,
	       	uint8_t frequency_div, uint16_t manual_duty)
{
	pc->num = num;
	pc->waveGenNum = waveGenNum;
	pc->frequency_div = frequency_div;
	pc->manual_duty = manual_duty;
	pwm_airoha_manual_config(pc);
}

/* Disable a particular gpio pin */

static void pwm_airoha_disable(int gpio_pin, struct gpio_desc *descriptor, struct pwm_airoha_chip *pc)
{
	int index = 0;

	pwm_flash_mode_disable(gpio_pin, pc);
	if(gpio_OE_state[gpio_pin] == 0){
		pwm_OE_disable(gpio_pin, descriptor);
	}
	if(!check_member_flash(gpio_pin, pc)){
		pwm_flash_map_disable(gpio_pin, pc);
		index = pwm_get_flash_map_cfg(gpio_pin, pc);
		if(index >= 0)
		{
			gen_wave_state[index] -= 1;
		}
	}
}

/*  Configure the frquency and duty cycle and selects generation wave number for them */

static int pwm_airoha_config(struct pwm_device *pwm, struct pwm_airoha_chip *pc, u64 duty_ns, u64 cycle_ns)
{

	struct gpio_desc *descriptor = gpio_to_desc(pwm->hwpwm);
	int wave_num = -1;
	int8_t ret;
	uint8_t  high, low;
	uint64_t period;

	/* frequency_div */
	if(cycle_ns > MAX_PERIOD || cycle_ns < MIN_PERIOD){
		printk("[pwm]ERROR, WE ONLY SUPPORT PERIOD BETWEEN 4000000 AND 1000000000ns\n");
		return -1;
	}
	period = cycle_ns * 250; /* frequency range is 1Hz to 250Hz */
	period = DIV_ROUND_CLOSEST_ULL(period, 1000000000);

	/* manual_duty */
	high = (unsigned char) ( (255 * duty_ns) / cycle_ns);
	low = 255 - high;

	/* waveGenNum */
	ret = select_gen_wave(pc, pwm->hwpwm, period, low);
	if(ret == -1)
	{
		return -1;
	}else{
		if(pwm->state.enabled)
			pwm_airoha_disable(pwm->hwpwm, descriptor, pc);
		wave_num = ret;
	}

	airoha_gpio_config_pwm(pc, pwm->hwpwm, wave_num, period, low<<8 | high);

	return 0;
}

/* Enable a Particular gpio pin according to its period and duty cycle */

static void pwm_airoha_enable(int gpio_pin, struct gpio_desc *descriptor, struct pwm_airoha_chip *pc)
{
	gpiod_direction_output(descriptor, 1);
	pwm_flash_mode_enable(gpio_pin, pc);
	pwm_flash_map_enable(gpio_pin, pc);
	pwm_set_rsp_mode(gpio_pin, pc);
}

/* pwm_airoha_apply : This function is used to change the period
 * , duty cycle and enable and disable the gpio pin 
 */

static int pwm_airoha_apply(struct pwm_chip *chip, struct pwm_device *pwm,
	       	const struct pwm_state *state)
{
	int ret = -1;

	if((pwm->hwpwm == GPIO_PIN_NOT_SUPPORTED1) 
			|| (pwm->hwpwm == GPIO_PIN_NOT_SUPPORTED2) 
			|| (pwm->hwpwm == GPIO_PIN_NOT_SUPPORTED3) 
			|| (pwm->hwpwm == GPIO_PIN_NOT_SUPPORTED4)){
		printk("[pwm]we do not support PWM for gpio pin %d\n", pwm->hwpwm);
		return -1;
	}

	/* convert pin to its descriptor */
	struct gpio_desc *descriptor = gpio_to_desc(pwm->hwpwm);

	/* containor of pwm chip */
	struct pwm_airoha_chip *pc = to_pwm_airoha_chip(chip);

	if(state->polarity != PWM_POLARITY_NORMAL)
		return -EINVAL;

	if(!state->enabled){
		if(pwm->state.enabled)
			pwm_airoha_disable(pwm->hwpwm, descriptor, pc);
		return 0;
	}
	/* get the current Output Enable state */
	original_OE_state(pwm->hwpwm, descriptor);
	ret = pwm_airoha_config(pwm, pc, state->duty_cycle, state->period);	
	if(ret == 0){
		pwm_airoha_enable(pwm->hwpwm, descriptor, pc);
	}
	return ret;
}
static const struct pwm_ops pwm_airoha_ops = {
	.apply = pwm_airoha_apply,
	.owner = THIS_MODULE,
};

/* when the driver is attached to kernel by default 
 * this function will be called 
 */

static int pwm_airoha_probe(struct platform_device *pdev)
{
	int ret = 0, i = 0;
	struct pwm_airoha_chip *pc;
	pc = devm_kzalloc(&pdev->dev, sizeof(*pc), GFP_KERNEL);
	if(!pc){
		return -ENOMEM;
	}
	
	for(i=0; i<REG_SECTIONS; i++){
		pc->base_addr[i] = devm_platform_ioremap_resource(pdev, i);
		if(IS_ERR(pc->base_addr[i]))
			return PTR_ERR(pc->base_addr[i]);
	}

	pc->chip.dev = &pdev->dev;
	pc->chip.ops = &pwm_airoha_ops;
	pc->chip.npwm = MAX_GPIO_PINS;
	ret = pwmchip_add(&pc->chip);
	if(ret<0){
		printk("[pwm]pwmchip_add failed");
		return ret;
	}
	printk(KERN_INFO "[pwm]chip added \n");
	return ret;
}

static const struct of_device_id pwm_airoha_of_match[] = {
	{ .compatible = "airoha,airoha-pwm"},
	{},
};
MODULE_DEVICE_TABLE(of, pwm_airoha_of_match);

static struct platform_driver pwm_airoha_driver = {
	.driver = {
		.name = "airoha-pwm",
		.of_match_table = pwm_airoha_of_match,
	},
	.probe = pwm_airoha_probe,
};
module_platform_driver(pwm_airoha_driver);

MODULE_LICENSE("GPL v2");
