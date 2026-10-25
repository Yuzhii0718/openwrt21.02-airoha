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

#include <linux/of_address.h>
#include <linux/fs.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ecnt_timer.h>
#include <linux/interrupt.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define APB_TIMER_DBG			(0)

#define CR_TIMER_BASE  			(0x000000)	/*phys:0xBFBF0100*/
#define CR_TIMER_CTL    		(CR_TIMER_BASE + 0x000)
#define CR_TIMER0_LDV   		(CR_TIMER_BASE + 0x04)
#define CR_TIMER0_VLR			(CR_TIMER_BASE + 0x08)
#define CR_TIMER1_LDV			(CR_TIMER_BASE + 0x0C)
#define CR_TIMER1_VLR			(CR_TIMER_BASE + 0x10)
#define CR_TIMER2_LDV			(CR_TIMER_BASE + 0x14)
#define CR_TIMER2_VLR			(CR_TIMER_BASE + 0x18)
//#define CR_TIMER3_LDV			(CR_TIMER_BASE + 0x1C)
//#define CR_TIMER3_VLR			(CR_TIMER_BASE + 0x20)
//#define CR_TIMER4_LDV			(CR_TIMER_BASE + 0x24)
//#define CR_TIMER4_VLR			(CR_TIMER_BASE + 0x28)
//#define CR_TIMER5_LDV			(CR_TIMER_BASE + 0x2C)
#define CR_TIMER3_LDV			(CR_TIMER_BASE + 0x2C)
#define CR_TIMER3_VLR			(CR_TIMER_BASE + 0x30)
#define CR_WDOG_THSLD           (CR_TIMER_BASE + 0x34)
#define CR_WDOG_RLD         	(CR_TIMER_BASE + 0x38)

#define TIMERTICKS_1MS       (1)  
#define TIMERTICKS_10MS      (10)  // set timer ticks as 10 ms
#define TIMERTICKS_100MS     (100)
#define TIMERTICKS_1S        (1000) 
#define TIMERTICKS_10S       (10000)

#define ENABLE          (1)
#define DISABLE         (0)

#define TIMER_LOOP_DETECT_THRESHOLD		(10)
#define DELAY_US_MODE	(0)
#define DELAY_MS_MODE	(1)
#define DELAY_MAX_MODE	(2)

#define TIMER_NO_FOR_DELAY_FUNC	(2)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/



/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_timer {
	struct device *dev;
	void __iomem *base;
	u32 irq;
	u32 irq_soft_ratelimit;
};



/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_timer *ecnt_timer =NULL;

static const struct of_device_id ecnt_timer_of_ids[] = {
		{ .compatible = "econet,ecnt-timer"},
	    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_timer_of_ids);

static DEFINE_RAW_SPINLOCK(timer_reg_lock);
static DEFINE_RAW_SPINLOCK(timer_data_lock);
void (*wdog_irq_callback)(void) = NULL;

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
static u32 get_timer_data(u32 reg)
{
    return readl(ecnt_timer->base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_timer_data(u32 reg, u32 val)
{
    writel(val, ecnt_timer->base + reg); 
}

int get_soft_ratelimit_timer_irq(void)
{
	return ecnt_timer->irq_soft_ratelimit;
}
EXPORT_SYMBOL(get_soft_ratelimit_timer_irq);

struct device* get_ecnt_timer_dev(void)
{
	if( (ecnt_timer) && (ecnt_timer->dev) )
		return ecnt_timer->dev;
	else
		return NULL;
}
EXPORT_SYMBOL(get_ecnt_timer_dev);

void set_wdogTimer_threshold(u32 val)
{
	unsigned long flags;
	
    raw_spin_lock_irqsave(&timer_reg_lock, flags);

    writel(val, ecnt_timer->base + CR_WDOG_THSLD); 

	raw_spin_unlock_irqrestore(&timer_reg_lock, flags);	
}
EXPORT_SYMBOL(set_wdogTimer_threshold);

static int apb_timer_delay(uint32 mode, uint32 time)
{
	volatile uint32 timer_now, timer_last;
	volatile uint32 tick_acc;
	uint32 tick_per_unit;
	volatile uint32 tick_wait; 
	volatile uint32 timer2_ldv = get_timer_data(CR_TIMER2_LDV);
	uint32 same_count = 0;

	if(mode >= DELAY_MAX_MODE) {
		printk("%s: Delay mode error.\r\n", __func__);
		return -1;
	}

	if((get_timer_data(CR_TIMER_CTL) & 0x4) == 0) {
		printk("%s: Error, APB Timer2 does not be enabled.\r\n", __func__);
		return -1;
	}

	if(mode == DELAY_US_MODE) {
		tick_per_unit = SYS_HCLK >> 1;//SYS_HCLK *10^6 / 10^6 / 2
	} else {
		tick_per_unit = SYS_HCLK * 500;//SYS_HCLK *10^6 / 10^3 / 2
	}
	tick_wait = time * tick_per_unit;
	
	tick_acc = 0;
	timer_last = get_timer_data(CR_TIMER2_VLR);
	do {
		timer_now = get_timer_data(CR_TIMER2_VLR);
		if(timer_last == timer_now) {
			same_count++;
		}
		if(same_count >= TIMER_LOOP_DETECT_THRESHOLD) {
			printk("%s: dead loop, break;\r\n", __func__);
			return -1;
		}
	  	if (timer_last >= timer_now) {
	  		tick_acc += timer_last - timer_now;
		} else {
			tick_acc += timer2_ldv - timer_now + timer_last;
		}
		timer_last = timer_now;
	} while (tick_acc < tick_wait);

	return 0;
}

void delay1us(int us)
{
	if(apb_timer_delay(DELAY_US_MODE, us) != 0) {
		printk("%s: error;\r\n", __func__);
	}
}
EXPORT_SYMBOL(delay1us);

void delay1ms(int ms)
{
	if(apb_timer_delay(DELAY_MS_MODE, ms) != 0) {
		printk("%s: error;\r\n", __func__);
	}
}
EXPORT_SYMBOL(delay1ms);

u32 get_vlr(u32 timer_no)
{
	
	if(timer_no >= 3) {
		printk("Error, timer_no only 0 1 or 2 available.\n");
		return 0;
	}
	
	return get_timer_data(CR_TIMER0_VLR + timer_no * 0x8);
}
EXPORT_SYMBOL(get_vlr);

u32 get_ldv(u32 timer_no)
{
	
	if(timer_no >= 3) {
		printk("Error, timer_no only 0 1 or 2 available.\n");
		return 0;
	}
	
	return get_timer_data(CR_TIMER0_LDV + timer_no * 0x8);
}
EXPORT_SYMBOL(get_ldv);

uint32 timerCnt(void)
{	
	//volatile uint32 cnt = regRead32(CR_TIMER1_VLR);	
	volatile uint32 cnt = get_vlr(1);	
	return cnt;
}
uint32  timerCntAdjust(uint32 lastTimerCnt, uint32 currentTimerCnt)
{	//volatile uint32 freeTimerMaxCnt = regRead32(CR_TIMER1_LDV);
	volatile uint32 freeTimerMaxCnt = get_ldv(1);

	if (currentTimerCnt < lastTimerCnt)		
		return lastTimerCnt - currentTimerCnt;	
	else		
		return currentTimerCnt = freeTimerMaxCnt - currentTimerCnt + lastTimerCnt;
}

uint32 getOneMsTick(void)
{	
	return SYS_HCLK * 500;
}
EXPORT_SYMBOL(getOneMsTick);

uint32 getOneUsTick(void)
{	
	return SYS_HCLK >>1;
}
EXPORT_SYMBOL(getOneUsTick);


uint32 getOneTickUnit(void)
{	
	return getOneMsTick();
}
EXPORT_SYMBOL(timerCnt);
EXPORT_SYMBOL(timerCntAdjust);
EXPORT_SYMBOL(getOneTickUnit);


static void timer_Configure(uint8 timer_no, uint8 timer_enable, uint8 timer_halt)
{
	uint32 word, offset;

    offset = timer_no;

	word = get_timer_data(CR_TIMER_CTL);
	/* Set enable */
	if(timer_enable) {
		word |= (1 << offset);
	} else {
		word &= ~(1 << offset);
	}
	/* Set interrupt */
	if(timer_halt) {
		word |= (1 << (offset + 16));
	} else {
		word &= ~(1 << (offset + 16));
	}

	set_timer_data(CR_TIMER_CTL, word);
	
#if APB_TIMER_DBG
	printk("\rtimer_configure: set_timer_data = %x\n", word);
#endif
} 

static void timerLdvSet(uint8 timer_no, u32 val)
{
	uint32_t offset;	
    offset = timer_no;
	set_timer_data(CR_TIMER0_LDV+(offset*0x08),val);
	
#if APB_TIMER_DBG
	printk("\timerLdvSet: set_timer_data = %x\n", val);
#endif
}

static void timerCtlSet(uint8 timer_no, uint8 timer_enable, uint8 timer_halt)
{
	timer_Configure(timer_no, timer_enable, timer_halt);	
}

void timer_WatchDogConfigure(uint8 tick_enable, uint8 watchdog_enable)
{
	uint32 word;

	word = get_timer_data(CR_TIMER_CTL);
	word &= 0xfdffffdf;
	word |= ( tick_enable << 5)|(watchdog_enable<<25);
	set_timer_data(CR_TIMER_CTL, word);
}
EXPORT_SYMBOL(timer_WatchDogConfigure);

void wdog_kick(void)
{
	set_timer_data(CR_WDOG_RLD, 0x1);
}
EXPORT_SYMBOL(wdog_kick);


/* 
   Block out the wdog_kick_api function 
   since the fucntion is already moved to tcwdog.c file
   Date: 2020/10/08  
   Editor: Keng-Chih 
*/
#if 0
/* source==1 for WIFI->LAN/WAN offload
 * source==2 for Mcast WAN->WIFI offload */
void wdog_kick_api(int source)
{
#if 0 // should be moved to tcwdog.c once the tcwdog is ready
    if (source==1) { /* WIFI->LAN/WAN offload */
        wifi_rx_cnt--;
        if (wifi_rx_cnt<=0) {
            wifi_rx_cnt=wifi_rx_cnt_load_value;
            wdog_kick();
        }
    }
    else if (source==2) { /* Mcast WAN->WIFI offload */
        mcast_offload_cnt--;
        if (mcast_offload_cnt<=0) {
            mcast_offload_cnt=mcast_offload_cnt_load_value;
            wdog_kick();
        }
    }
#endif
	wdog_kick();

    return;
}
EXPORT_SYMBOL(wdog_kick_api);
#endif
extern int iswatchDogReset;
void timerSet(uint32 timer_no, uint32 timerTime, uint32 enable, uint8 timer_halt)
{   
    uint32_t word, offset;	


	if(timer_no == 2 || timer_no > 3) {		
        printk("Error, timer_no:%u is not allowed to use.\n", timer_no);		
        return;	
    }
	
	/* timer number 3 is mapping to offset 5, 
	   since timer3_enable_disable is located in bit5 position,
	   instead of bit3 position, relative to timer0~2 */
	if(timer_no == 3) {
        offset = 5;	
    } else {
        offset = timer_no;	
    }
    
    if((get_timer_data(CR_TIMER_CTL) & (1 << offset)) == (1 << offset)) {
        if(timer_no != 3){
            printk("%s: Error, APB Timer%d has been enabled.\r\n", __func__, timer_no); 	
            return;
        }
    }	

	/* when SYS_HCLK is large, it will cause overflow. The calculation will be wrong */
    /* word = (timerTime * SYS_HCLK) * 1000 / 2; */
    word = (timerTime * SYS_HCLK) * 500; 
    /* set timer3 countdown value */	
    timerLdvSet(offset, word);	
    /* enable or disable timer3 and its interrupt */	
    timerCtlSet(offset, enable, timer_halt);

}
EXPORT_SYMBOL(timerSet);

/*aip for softratelimit module, EN7523 timer_no is timer 0*/
void softRatelimitTimerCtlIntSet(uint8 timer_no, uint8 timer_halt)
{  
	uint32 word, offset;

	offset = timer_no;

	word = get_timer_data(CR_TIMER_CTL);

	/* Set interrupt */
	if(timer_halt) {
		word |= (1 << (offset + 16));
	} else {
		word &= ~(1 << (offset + 16));
	}

	set_timer_data(CR_TIMER_CTL, word);
}
EXPORT_SYMBOL(softRatelimitTimerCtlIntSet);

/*aip for softratelimit module, EN7523 timer_no is timer 0*/
void softRatelimittimerCtlEnableSet(uint8 timer_no, uint8 timer_enable)
{  
	uint32 word, offset;

    offset = timer_no;

	word = get_timer_data(CR_TIMER_CTL);
	/* Set enable */
	if(timer_enable) {
		word |= (1 << offset);
	} else {
		word &= ~(1 << offset);
	}

	set_timer_data(CR_TIMER_CTL, word);
}
EXPORT_SYMBOL(softRatelimittimerCtlEnableSet);

/*aip for softratelimit module, EN7523 timer_no is timer 0*/
void softRatelimittimerLdvSet(uint32 timer_no, uint32 timerTime)
{   	    
    timerLdvSet(timer_no, timerTime);	
}
EXPORT_SYMBOL(softRatelimittimerLdvSet);

static void delay_func_timerSet(uint32 timerTime, uint32 enable, uint8 timer_halt)
{   
    uint32 word;

	/* when SYS_HCLK is large, it will cause overflow. The calculation will be wrong */
    /* word = (timerTime * SYS_HCLK) * 1000 / 2; */
    word = (timerTime * SYS_HCLK) * 500; 
    timerLdvSet(TIMER_NO_FOR_DELAY_FUNC,word);
    timerCtlSet(TIMER_NO_FOR_DELAY_FUNC,enable,timer_halt);
}

static irqreturn_t watchdog_timer_interrupt(int irq, void *dev_id)
{
    u32 word;

	word = get_timer_data(CR_TIMER_CTL); 
	word &= 0xffc0ffff;
	word |= 0x00200000;
	set_timer_data(word, CR_TIMER_CTL);
	
	if(!wdog_irq_callback){
		printk("[Warning] wdog_irq_callback function pointer is NULL!\n");
	}else{
        wdog_irq_callback();
    }

	return IRQ_HANDLED;	
}

int ecnt_timer_register(void (*callback)(void), void *data)
{
	unsigned long flags;
	int ret;

	ret = 0;
	raw_spin_lock_irqsave(&timer_data_lock, flags);
	if (wdog_irq_callback) {
		printk("[Warning] wdog_irq_callback function pointer is not NULL!\n");
		ret = -EBUSY;
		goto out;
	}

	wdog_irq_callback = callback;

out:
	raw_spin_unlock_irqrestore(&timer_data_lock, flags);	
	return ret;
}

EXPORT_SYMBOL(ecnt_timer_register);

static int ecnt_timer_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
    int ret;

	printk("[apb_timer] ecnt_timer_drv_probe\n");

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No timer DT node found\n");
        return -EINVAL;
    }

    ecnt_timer = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_timer), GFP_KERNEL);
    if (!ecnt_timer) {
		printk("[apb_timer] devm_kzalloc error.\n");
        return -ENOMEM;
    }

    platform_set_drvdata(pdev, ecnt_timer);

	/* get irq num */
	ecnt_timer->irq = platform_get_irq(pdev, 3);
    printk("[timer] ecnt_timer->irq: %d\n", ecnt_timer->irq);		

	ecnt_timer->irq_soft_ratelimit = platform_get_irq(pdev, 0);
	ret = devm_request_irq(&pdev->dev, ecnt_timer->irq, watchdog_timer_interrupt, 
	                           0, dev_name(&pdev->dev), ecnt_timer);
	if(ret){
		dev_err(&(pdev->dev), "devm_request_irq failed with err %d\n", ret);
		return -EINVAL;
	}

    /* get timer base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_timer->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_timer->base)) {
		printk("[apb_timer] devm_ioremap_resource error.\n");
        return PTR_ERR(ecnt_timer->base);
    }

    ecnt_timer->dev = &pdev->dev;

	/* Enable timer2 for delay1us and delay1ms function. */
	delay_func_timerSet(TIMERTICKS_10MS, ENABLE, DISABLE);
	
	printk("[timer] res->name:%s\n", res->name);
#if APB_TIMER_DBG
	printk("[timer] res->start:0x%llx ===\n", res->start);
	printk("[timer] res->end:0x%llx ===\n", res->end);
	printk("[timer] ecnt_timer->base:0x%lx\n", (unsigned long)ecnt_timer->base);
#endif
    return 0;
}

static int ecnt_timer_drv_remove(struct platform_device *pdev)
{
    return 0;
}


/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_timer_driver = {
	.probe = ecnt_timer_drv_probe,
	.remove = ecnt_timer_drv_remove,
	.driver = {
		.name = "ecnt-timer",
		.of_match_table = ecnt_timer_of_ids,
	},
};
module_platform_driver(ecnt_timer_driver);


MODULE_DESCRIPTION("EcoNet timer");


