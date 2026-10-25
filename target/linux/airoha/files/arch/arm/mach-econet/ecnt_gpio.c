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
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include "scu/ecnt_scu.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef TCSUPPORT_OPENWRT
/* the order must match dts setting */
enum {
	RG_REGION_LEDDSP = 0,
	RG_REGION_SGPIO,
	RG_REGION_FLASH_MODE_CONTROL,
	RG_REGION_FLASH_MODE_CONTROL2,
	RG_REGION_GPIO_INT_CONTROL,
	RG_REGION_GPIO_INT_CONTROL2,
	RG_REGION_GPIO_PWM,
};

#define GPIO_PM_ADDR 0x0
//#define GPIO_PM_ADDR 0x1FBF0200
extern void __iomem *arht_gpio_ctrl;
extern void __iomem *arht_gpio_ctrl1;
extern void __iomem *arht_gpio_data;
extern void __iomem *arht_gpio_oe_ctrl;
#endif
/* GPIO */
#define GPIO_BASE       	0x00000000
#define GPIO_CTRL           (GPIO_BASE + 0x00)
#define GPIO_DATA           (GPIO_BASE + 0x04)
#define GPIO_INT			(GPIO_BASE + 0x08)
#define GPIO_OE             (GPIO_BASE + 0x14)
#define GPIO_LEDPRD			(GPIO_BASE + 0x18)
#define GPIO_LEDCTRL		(GPIO_BASE + 0x1C)
#define GPIO_CTRL1          (GPIO_BASE + 0x20)
#define GPIO_SGPIO_LEDDATA	(GPIO_BASE + 0x24)
#define GPIO_SGPIO_CLKDIVR	(GPIO_BASE + 0x28)
#define GPIO_SGPIO_CLKDLY	(GPIO_BASE + 0x2C)
#define GPIO_SGPIO_CFG		(GPIO_BASE + 0x30)
#define GPIO_FLASH_MODE_CFG    (GPIO_BASE + 0x34)
#define GPIO_RSP_MODE_CFG      (GPIO_BASE + 0x38)
#define GPIO_FLASH_PRD_SET0    (GPIO_BASE + 0x3C)
#define GPIO_FLASH_PRD_SET1    (GPIO_BASE + 0x40)
#define GPIO_FLASH_PRD_SET2    (GPIO_BASE + 0x44)
#define GPIO_FLASH_PRD_SET3    (GPIO_BASE + 0x48)
#define GPIO_FLASH_MAP_CFG0    (GPIO_BASE + 0x4C)
#define GPIO_FLASH_MAP_CFG1    (GPIO_BASE + 0x50)
#define GPIO_FLASH_MODE_CFG_EXT    (GPIO_BASE + 0x68)
#define GPIO_INT1              (GPIO_BASE + 0x7C)
#define CYCLE_CFG_VALUE0       (GPIO_BASE + 0x98)
#define CYCLE_CFG_VALUE1       (GPIO_BASE + 0x9C)
#define AUTO_FLASH_STEP1_0     (GPIO_BASE + 0xA0)
#define AUTO_FLASH_STEP3_2     (GPIO_BASE + 0xA4)
#define AUTO_FLASH_STEP5_4     (GPIO_BASE + 0xA8)
#define AUTO_FLASH_STEP7_6     (GPIO_BASE + 0xAC)
#define AUTO_FLASH_AMP3_0      (GPIO_BASE + 0xB0)
#define AUTO_FLASH_AMP7_4      (GPIO_BASE + 0xB4)
#define AUTO_FLASH_EN          (GPIO_BASE + 0xB8)
#define FAN_EN                 (GPIO_BASE + 0xBC)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_gpio {
	struct device *dev;
#ifdef TCSUPPORT_OPENWRT 
	void __iomem *gpio_ledprd_ctrl_base;
	void __iomem *led_dsp_base;
	void __iomem *sgpio_base;
	void __iomem *flash_mode_base[2];
	void __iomem *gpio_int_ctl_base[2];
	void __iomem *pwm_base;
#else
	void __iomem *base;
#endif
};


typedef struct{
	uint8_t num;         
	uint8_t pin_mode;
	uint8_t work_mode;
	uint8_t duration_mode;
	uint8_t waveGenNum;
	uint8_t frequency_div;
	uint8_t pwm_mode;
	uint16_t time_interval;
	uint8_t variation;
}gpioStruct;

/* used in ledcetl.h */
unsigned long g_gpio_base;
EXPORT_SYMBOL(g_gpio_base);

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_gpio *ecnt_gpio = NULL;


static const struct of_device_id ecnt_gpio_of_id[] = {
    { .compatible = "econet,ecnt-gpio"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_gpio_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
#ifdef TCSUPPORT_OPENWRT
#define RG_IS_SGPIO(x) ((x) >= (GPIO_PM_ADDR + GPIO_SGPIO_LEDDATA) && (x) <= (GPIO_PM_ADDR + GPIO_SGPIO_CFG))  
#define RG_IS_PWM(x) ((x) >= (GPIO_PM_ADDR + CYCLE_CFG_VALUE0) && (x) <= (GPIO_PM_ADDR + FAN_EN))
#define RG_IS_FLASH_MODE(x) ((x) >= (GPIO_PM_ADDR + GPIO_FLASH_MODE_CFG) && (x) <= (GPIO_PM_ADDR + GPIO_FLASH_MAP_CFG1))
#define RG_IS_FLASH_MODE2(x) ((x) == (GPIO_PM_ADDR + GPIO_FLASH_MODE_CFG_EXT))
#define RG_IS_INT_SETTING(x) ((x) >= (GPIO_PM_ADDR + GPIO_INT) && (x) < (GPIO_PM_ADDR + GPIO_OE))
#define RG_IS_INT_SETTING2(x) ((x) >= (GPIO_PM_ADDR + GPIO_INT1) && (x) < (GPIO_PM_ADDR + CYCLE_CFG_VALUE0))
#define RG_IS_LEDDSP(x) ((x) == GPIO_LEDPRD || (x) == GPIO_LEDCTRL) 
#define RG_IS_GPIO_CTRL(x) ((x) == GPIO_CTRL)
#define RG_IS_GPIO_CTRL1(x) ((x) == GPIO_CTRL1)
#define RG_IS_GPIO_DATA(x) ((x) == GPIO_DATA)
#define RG_IS_GPIO_OE(x) ((x) == GPIO_OE)

#define GET_SGPIO_RG(x) (ecnt_gpio->sgpio_base + ((x) - GPIO_PM_ADDR - GPIO_SGPIO_LEDDATA))
#define GET_PWM_RG(x) (ecnt_gpio->pwm_base + ((x) - GPIO_PM_ADDR - CYCLE_CFG_VALUE0))
#define GET_FLASH_MODE_RG(x) (ecnt_gpio->flash_mode_base[0] + ((x) - GPIO_PM_ADDR - GPIO_FLASH_MODE_CFG))
#define GET_FLASH_MODE_RG2(x) (ecnt_gpio->flash_mode_base[1] + ((x) - GPIO_PM_ADDR - GPIO_FLASH_MODE_CFG_EXT))
#define GET_INT_RG(x) (ecnt_gpio->gpio_int_ctl_base[0] + ((x) - GPIO_PM_ADDR - GPIO_INT)) 
#define GET_INT_RG2(x) (ecnt_gpio->gpio_int_ctl_base[1] + ((x) - GPIO_PM_ADDR - GPIO_INT1))
#define GET_LEDDSP_RG(x) (ecnt_gpio->led_dsp_base + ((x) - GPIO_LEDPRD))
#define GET_GPIO_CTRL_RG(x) (arht_gpio_ctrl)
#define GET_GPIO_CTRL1_RG(x) (arht_gpio_ctrl1)
#define GET_GPIO_DATA_RG(x) (arht_gpio_data)
#define GET_GPIO_OE_RG(x) (arht_gpio_oe_ctrl)

u32 get_gpio_data(u32 reg)
{
	if (RG_IS_SGPIO(reg)) {
		return readl(GET_SGPIO_RG(reg));
	} else if (RG_IS_PWM(reg)) {
		return readl(GET_PWM_RG(reg));
	} else if (RG_IS_FLASH_MODE(reg)) {
		return readl(GET_FLASH_MODE_RG(reg));
	} else if (RG_IS_FLASH_MODE2(reg)) {
		return readl(GET_FLASH_MODE_RG2(reg));
	} else if (RG_IS_INT_SETTING(reg)) {
		return readl(GET_INT_RG(reg));
	} else if (RG_IS_INT_SETTING2(reg)) {
		return readl(GET_INT_RG2(reg));
	} else if (RG_IS_LEDDSP(reg)) {
		return readl(GET_LEDDSP_RG(reg));
	} else if (RG_IS_GPIO_CTRL(reg)) {
		return readl(GET_GPIO_CTRL_RG(reg));
	} else if (RG_IS_GPIO_CTRL1(reg)) {
		return readl(GET_GPIO_CTRL1_RG(reg));
	} else if (RG_IS_GPIO_DATA(reg)) {
		return readl(GET_GPIO_DATA_RG(reg));
	} else if (RG_IS_GPIO_OE(reg)) {
		return readl(GET_GPIO_OE_RG(reg));
	} else {
		printk("GPIO Access Error: Not supported RG: 0x%x\n", reg);
	}
	return 0;
}

void set_gpio_data(u32 reg, u32 val)
{
	if (RG_IS_SGPIO(reg)) {
		writel(val, GET_SGPIO_RG(reg));
	} else if (RG_IS_PWM(reg)) {
		writel(val, GET_PWM_RG(reg));
	} else if (RG_IS_FLASH_MODE(reg)) {
		writel(val, GET_FLASH_MODE_RG(reg));
	} else if (RG_IS_FLASH_MODE2(reg)) {
		writel(val, GET_FLASH_MODE_RG2(reg));
	} else if (RG_IS_INT_SETTING(reg)) {
		writel(val, GET_INT_RG(reg));
	} else if (RG_IS_INT_SETTING2(reg)) {
		writel(val, GET_INT_RG2(reg));
	} else if (RG_IS_LEDDSP(reg)) {
		writel(val, GET_LEDDSP_RG(reg));
	} else if (RG_IS_GPIO_CTRL(reg)) {
		writel(val, GET_GPIO_CTRL_RG(reg));
	} else if (RG_IS_GPIO_CTRL1(reg)) {
		writel(val, GET_GPIO_CTRL1_RG(reg));
	} else if (RG_IS_GPIO_DATA(reg)) {
		writel(val, GET_GPIO_DATA_RG(reg));
	} else if (RG_IS_GPIO_OE(reg)) {
		writel(val, GET_GPIO_OE_RG(reg));
	} else {
		printk("GPIO Access Error: Not supported RG: 0x%x\n", reg);
	}
}
#else
u32 get_gpio_data(u32 reg)
{
    return readl(ecnt_gpio->base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
void set_gpio_data(u32 reg, u32 val)
{
    writel(val, ecnt_gpio->base + reg); 
}
#endif

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

static u32 GPIOPWM_GET_GPIO_RSP_MODE_CFG(u8 gpioPinNum)
{
    return get_gpio_data(GPIO_RSP_MODE_CFG);
}

static void GPIOPWM_SET_GPIO_RSP_MODE_CFG(u8 gpioPinNum, u32 data)
{
    set_gpio_data(GPIO_RSP_MODE_CFG, data);
}

static u32 GPIOPWM_GET_GPIO_FLASH_MAP_CFG(u8 gpioPinNum)
{
    switch(gpioPinNum){
        case 0 ... 7:
            return get_gpio_data(GPIO_FLASH_MAP_CFG0);
        case 8 ... 15:
            return get_gpio_data(GPIO_FLASH_MAP_CFG1);
    }

	return 0;
}

static void GPIOPWM_SET_GPIO_FLASH_MAP_CFG(u8 gpioPinNum, u32 data)
{
    switch(gpioPinNum){
        case 0 ... 7:
            set_gpio_data(GPIO_FLASH_MAP_CFG0, data);
            break;
        case 8 ... 15:
            set_gpio_data(GPIO_FLASH_MAP_CFG1, data);
            break;
    }
}

static u32 GPIOPWM_GET_CYCLE_CFG_VALUE(u8 waveGenNum)
{
    switch(waveGenNum){
        case 0 ... 3:
            return get_gpio_data(CYCLE_CFG_VALUE0);
        case 4 ... 7:
            return get_gpio_data(CYCLE_CFG_VALUE1);
    }

	return 0;
}

static void GPIOPWM_SET_CYCLE_CFG_VALUE(u8 waveGenNum, u32 data)
{
    switch(waveGenNum){
        case 0 ... 3:
            set_gpio_data(CYCLE_CFG_VALUE0, data);
            break;
        case 4 ... 7:
            set_gpio_data(CYCLE_CFG_VALUE1, data);
            break;
    }
}

static u32 GPIOPWM_GET_AUTO_FLASH_STEP(u8 waveGenNum){

    switch(waveGenNum){
        case 0 ... 1:
            return get_gpio_data(AUTO_FLASH_STEP1_0);
        case 2 ... 3:
            return get_gpio_data(AUTO_FLASH_STEP3_2);
        case 4 ... 5:
            return get_gpio_data(AUTO_FLASH_STEP5_4);
        case 6 ... 7:
            return get_gpio_data(AUTO_FLASH_STEP7_6);		
    }

	return 0;
}

static void GPIOPWM_SET_AUTO_FLASH_STEP(u8 waveGenNum, u32 data){

    switch(waveGenNum){
        case 0 ... 1:
            set_gpio_data(AUTO_FLASH_STEP1_0, data);
            break;
        case 2 ... 3:
            set_gpio_data(AUTO_FLASH_STEP3_2, data);
            break;
        case 4 ... 5:
            set_gpio_data(AUTO_FLASH_STEP5_4, data);
            break;
        case 6 ... 7:
            set_gpio_data(AUTO_FLASH_STEP7_6, data);
            break;
    }
}

static u32 GPIOPWM_GET_AUTO_FLASH_AMP(u8 waveGenNum)
{
    switch(waveGenNum){
        case 0 ... 3:
            return get_gpio_data(AUTO_FLASH_AMP3_0);
        case 4 ... 7:
            return get_gpio_data(AUTO_FLASH_AMP7_4);
    }

	return 0;
}

static void GPIOPWM_SET_AUTO_FLASH_AMP(u8 waveGenNum, u32 data)
{
    switch(waveGenNum){
        case 0 ... 3:
            set_gpio_data(AUTO_FLASH_AMP3_0, data);
            break;
        case 4 ... 7:
            set_gpio_data(AUTO_FLASH_AMP7_4, data);
            break;
    }
}

static u32 GPIOPWM_GET_AUTO_FLASH_EN(void)
{
    return get_gpio_data(AUTO_FLASH_EN);
}

static void GPIOPWM_SET_AUTO_FLASH_EN(u8 waveGenNum, u32 data)
{
    set_gpio_data(AUTO_FLASH_EN, data);
}

u32 GPIOPWM_GET_HILOPRD(u8 waveGenNum)
{
    switch(waveGenNum){
        case 0:
            return get_gpio_data(GPIO_FLASH_PRD_SET0);		
        case 1:
            return get_gpio_data(GPIO_FLASH_PRD_SET1);	
        case 2:
            return get_gpio_data(GPIO_FLASH_PRD_SET2);	
        case 3:
            return get_gpio_data(GPIO_FLASH_PRD_SET3);	
    }

	return 0;
}
EXPORT_SYMBOL(GPIOPWM_GET_HILOPRD);

static void gpio_global_reset(void)
{
    unsigned long reg_value = GET_SCU_RSTCTRL1();
    SET_SCU_RSTCTRL1(reg_value | (0x1 << 13));     /*   set bit13 */
    SET_SCU_RSTCTRL1(reg_value & (~(0x1 << 13)));  /* clear bit13 */
}
EXPORT_SYMBOL(gpio_global_reset);

static void gpioReg_config(gpioStruct *gpioObj_p, u32 (*get_func_p)(u8), void (*set_func_p)(u8, u32), 
    u16 data, u8 num_per_reg){
    
    uint32_t value_tmp = 0;	
    uint16_t bitMask = 0;
    uint8_t num = 0;
    
    if(get_func_p == &GPIOPWM_GET_GPIO_FLASH_MAP_CFG || 
        get_func_p == &GPIOPWM_GET_GPIO_RSP_MODE_CFG ||
        get_func_p == &GPIOPWM_GET_GPIO_FLASH_MODE_CFG){
        num = gpioObj_p->num;
    }else{
        num = gpioObj_p->waveGenNum;
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
    value_tmp = (*get_func_p)(num);
    value_tmp = (value_tmp & (~(bitMask << ((32/num_per_reg)*(num % num_per_reg)))));	
    (*set_func_p)(num, value_tmp | (data << ((32/num_per_reg)*(num % num_per_reg))));
}

void gpioPwm_config(gpioStruct *gpioObj_p)
{
	uint32_t value_tmp;

    /* (1) Configure GPIO pins(0~15) as output mode & output enable mode */
    if(1 == gpioObj_p->pin_mode){  //output mode
        SET_GPIO_CTRL(GET_GPIO_CTRL() | (1 << (2*gpioObj_p->num)));
        SET_GPIO_OE(GET_GPIO_OE() | (1 << gpioObj_p->num));
    }else{
        printk("no action for this pin_mode %d\n", gpioObj_p->pin_mode);
    }

    /* (2) Configure GPIO pins(0~15) as flash mode */
    gpioReg_config(gpioObj_p, &GPIOPWM_GET_GPIO_FLASH_MODE_CFG, 
        &GPIOPWM_SET_GPIO_FLASH_MODE_CFG, gpioObj_p->work_mode, 32);

    /* (3) Configure GPIO pins' duration mode as cycle mode for continuous PWM signal */
    gpioReg_config(gpioObj_p, &GPIOPWM_GET_GPIO_RSP_MODE_CFG, 
        &GPIOPWM_SET_GPIO_RSP_MODE_CFG, gpioObj_p->duration_mode, 32);

    /* (4) Map GPIO pins to waveform generators */
    gpioReg_config(gpioObj_p, &GPIOPWM_GET_GPIO_FLASH_MAP_CFG, 
        &GPIOPWM_SET_GPIO_FLASH_MAP_CFG, (8 | gpioObj_p->waveGenNum), 8);

    /* (5) Determine PWM output freqency. For PWM mode, the freqency should be 
           larger than 60 Hz where human would always see LED stay on, instead of flash */
    gpioReg_config(gpioObj_p, &GPIOPWM_GET_CYCLE_CFG_VALUE, 
        &GPIOPWM_SET_CYCLE_CFG_VALUE, gpioObj_p->frequency_div, 4);

    /* (6) Configure waveform generators as auto PWM mode */
    gpioReg_config(gpioObj_p, &GPIOPWM_GET_AUTO_FLASH_EN, 
        &GPIOPWM_SET_AUTO_FLASH_EN, gpioObj_p->pwm_mode, 32);

    /* (7) Determine the time interval in which the duty cycle changes value */
    gpioReg_config(gpioObj_p, &GPIOPWM_GET_AUTO_FLASH_STEP, 
        &GPIOPWM_SET_AUTO_FLASH_STEP, gpioObj_p->time_interval, 2);

    /* (8) Configure how much pulse width the duty cycle changes each time interval */
    gpioReg_config(gpioObj_p, &GPIOPWM_GET_AUTO_FLASH_AMP, 
        &GPIOPWM_SET_AUTO_FLASH_AMP, gpioObj_p->variation, 4);
}
EXPORT_SYMBOL(gpioPwm_config);

static int ecnt_gpio_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
    struct proc_dir_entry *gpioPwmProc;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No gpio DT node found");
        return -EINVAL;
    }

    ecnt_gpio = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_gpio), GFP_KERNEL);
    if (!ecnt_gpio) {
        return -ENOMEM;
	}

    platform_set_drvdata(pdev, ecnt_gpio);

#ifdef TCSUPPORT_OPENWRT
	/* LED Display */
	res = platform_get_resource(pdev, IORESOURCE_MEM, RG_REGION_LEDDSP);
	ecnt_gpio->led_dsp_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->led_dsp_base)) {
        return PTR_ERR(ecnt_gpio->led_dsp_base);
	}

	/* SGPIO */
	res = platform_get_resource(pdev, IORESOURCE_MEM, RG_REGION_SGPIO);
	ecnt_gpio->sgpio_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->sgpio_base)) {
        return PTR_ERR(ecnt_gpio->sgpio_base);
	}

	/* Flash MODE control */
	res = platform_get_resource(pdev, IORESOURCE_MEM, RG_REGION_FLASH_MODE_CONTROL);
	ecnt_gpio->flash_mode_base[0] = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->flash_mode_base[0])) {
        return PTR_ERR(ecnt_gpio->flash_mode_base[0]);
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, RG_REGION_FLASH_MODE_CONTROL2);
	ecnt_gpio->flash_mode_base[1] = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->flash_mode_base[1])) {
        return PTR_ERR(ecnt_gpio->flash_mode_base[1]);
	}

	/* GPIO INT Contrl */
	res = platform_get_resource(pdev, IORESOURCE_MEM, RG_REGION_GPIO_INT_CONTROL);
	ecnt_gpio->gpio_int_ctl_base[0] = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->gpio_int_ctl_base[0])) {
        return PTR_ERR(ecnt_gpio->gpio_int_ctl_base[0]);
	}
	res = platform_get_resource(pdev, IORESOURCE_MEM, RG_REGION_GPIO_INT_CONTROL2);
	ecnt_gpio->gpio_int_ctl_base[1] = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->gpio_int_ctl_base[1])) {
        return PTR_ERR(ecnt_gpio->gpio_int_ctl_base[1]);
	}

	/* PWM */
	res = platform_get_resource(pdev, IORESOURCE_MEM, RG_REGION_GPIO_PWM);
	ecnt_gpio->pwm_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->pwm_base)) {
        return PTR_ERR(ecnt_gpio->pwm_base);
	}

#else
    /* get GPIO base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_gpio->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_gpio->base))
        return PTR_ERR(ecnt_gpio->base);

    ecnt_gpio->dev = &pdev->dev;
    g_gpio_base = (unsigned long)ecnt_gpio->base;	

#if 0
    printk("[gpio] res->name:%s\n", res->name);
    printk("[gpio] res->start:0x%llx ===\n", res->start);
    printk("[gpio] res->end:0x%llx ===\n", res->end);
    printk("[gpio] ecnt_gpio->base:0x%lx\n", (unsigned long)ecnt_gpio->base);
#endif
#endif

    return 0;
}

static int ecnt_gpio_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_gpio_driver = {
    .probe = ecnt_gpio_drv_probe,
    .remove = ecnt_gpio_drv_remove,
    .driver = {
        .name = "ecnt-gpio",
        .of_match_table = ecnt_gpio_of_id
    },
};
module_platform_driver(ecnt_gpio_driver);
MODULE_DESCRIPTION("EcoNet GPIO Driver");


