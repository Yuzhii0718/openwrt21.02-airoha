/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Setting up the clock on the MIPS boards.
 */
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timex.h>

#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/hardirq.h>
#include <asm/irq.h>
#include <asm/div64.h>
#include <asm/cpu.h>
#include <asm/time.h>
#include <asm/setup.h>

#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/TCIfSetQuery_os.h>
#ifdef TCSUPPORT_MIPS_1004K
#include <asm/gic.h>
#endif
#ifdef TCSUPPORT_NEW_WDOG
extern void wdog_msg_print(void);
#endif

unsigned long cpu_khz;

static int mips_cpu_timer_irq;
extern int cp0_perfcount_irq;
extern void smtc_timer_broadcast(int);
static unsigned long cycles_per_jiffy __read_mostly;
unsigned int cpu_timer_loss[4];
#ifdef TCSUPPORT_MIPS_1004K
static unsigned int expirelo[4];
static const unsigned int cputmr_cnt[] = {CR_CPUTMR_CNT0, CR_CPUTMR_CNT1, CR_CPUTMR_CNT2, CR_CPUTMR_CNT3};
static const unsigned int cputmr_cmr[] = {CR_CPUTMR_CMR0, CR_CPUTMR_CMR1, CR_CPUTMR_CMR2, CR_CPUTMR_CMR3};

extern int timers_intSrcNum[NR_CPUS];
#else
static unsigned int expirelo[2];
static const unsigned int cputmr_cnt[] = {CR_CPUTMR_CNT0, CR_CPUTMR_CNT1};
static const unsigned int cputmr_cmr[] = {CR_CPUTMR_CMR0, CR_CPUTMR_CMR1};
#endif

static int vpe1_timer_installed = 0;
#ifdef TCSUPPORT_BONDING
unsigned long int bondaddr;
EXPORT_SYMBOL(bondaddr);
static char tag;
#endif
#ifdef CONFIG_PCI
extern int pcieRegInitConfig(void);
extern void pcieReset(void);
extern void setahbstat(int val);
#endif
#ifdef TCSUPPORT_CPU_EN7528
#define RBUS_TIMEOUT_STS0   0xbfa000d0
#define RBUS_TIMEOUT_CFG0   0xbfa000d8
#define RBUS_TIMEOUT_CFG1   0xbfa000dc
#define RBUS_TIMEOUT_CFG2   0xbfa000e0
#endif


uint32 timerCnt(void)
{

	volatile uint32 cnt = regRead32(CR_TIMER1_VLR);
	return cnt;
}
uint32  timerCntAdjust(uint32 lastTimerCnt, uint32 currentTimerCnt)
{
	volatile uint32 freeTimerMaxCnt = regRead32(CR_TIMER1_LDV);
	if (currentTimerCnt < lastTimerCnt)
		return lastTimerCnt - currentTimerCnt;
	else
		return currentTimerCnt = freeTimerMaxCnt - currentTimerCnt + lastTimerCnt;
}
uint32 getOneTickUnit(void)
{

	return SYS_HCLK * 500;
}

EXPORT_SYMBOL(timerCnt);
EXPORT_SYMBOL(timerCntAdjust);
EXPORT_SYMBOL(getOneTickUnit);


static void delay1ms(int ms)
{
	volatile uint32 timer_now, timer_last;
	volatile uint32 tick_acc;
	uint32 one_tick_unit = SYS_HCLK * 500;//1 * SYS_HCLK * 1000 / 2
	volatile uint32 tick_wait = ms * one_tick_unit; 
	volatile uint32 timer1_ldv = regRead32(CR_TIMER1_LDV);

	tick_acc = 0;
	timer_last = regRead32(CR_TIMER1_VLR);
	do {
		timer_now = regRead32(CR_TIMER1_VLR);
	  	if (timer_last >= timer_now) 
	  		tick_acc += timer_last - timer_now;
		else
			tick_acc += timer1_ldv - timer_now + timer_last;
		timer_last = timer_now;
	} while (tick_acc < tick_wait);
}

void delay1us(int period, int number)
{
	volatile unsigned int timer_now, timer_last;
	volatile unsigned int  tick_acc;
	unsigned int  one_tick_unit = SYS_HCLK * 500; // 500/100 = 1ms /100 = 10us
	volatile unsigned int  tick_wait = number * one_tick_unit / 10; //caculate 10 us delay wait
	volatile unsigned int  timer1_ldv = regRead32(CR_TIMER1_LDV);
	int same_count = 0;
	tick_acc = 0;
 	timer_last = regRead32(CR_TIMER1_VLR);
	do {
   		timer_now = regRead32(CR_TIMER1_VLR);
		if(timer_last == timer_now)
		{
			same_count++;
		}
		if(same_count >= period)
		{
			printk("delay1us: dead loop, break;\r\n");
			return;
		}
       	if (timer_last >= timer_now)
       		tick_acc += timer_last - timer_now;
      	else
       		tick_acc += timer1_ldv - timer_now + timer_last;
     	timer_last = timer_now;
	} while (tick_acc < tick_wait);
}

EXPORT_SYMBOL(delay1us);


void
timer_Configure(
	uint8  timer_no, 
	uint8 timer_enable, 
	uint8 timer_mode, 
	uint8 timer_halt
)
{
	uint32 word,word1;

	word = regRead32(CR_TIMER_CTL);
	word1 = (timer_enable << timer_no)|(timer_mode << (timer_no + 8))|(timer_halt << (timer_no + 26));
	word |= word1;
	regWrite32(CR_TIMER_CTL, word);
} 

void 
timerSet(
	uint32 timer_no,
	uint32 timerTime, 
	uint32 enable,
	uint32 mode, 
	uint32 halt
)
{   
    uint32 word;

	/* when SYS_HCLK is large, it will cause overflow. The calculation will be wrong */
    /* word = (timerTime * SYS_HCLK) * 1000 / 2; */
    word = (timerTime * SYS_HCLK) * 500; 
    timerLdvSet(timer_no,word);
    timerCtlSet(timer_no,enable,mode,halt);
}

void
timer_WatchDogConfigure (
	uint8 tick_enable, 
	uint8 watchdog_enable
)
{
	uint32 word;

	word = regRead32(CR_TIMER_CTL);
	word &= 0xfdffffdf;
	word |= ( tick_enable << 5)|(watchdog_enable<<25);
	regWrite32(CR_TIMER_CTL, word);
}

int
is_nmi_enable(void)
{
	uint32 word = regRead32(CR_AHB_NMI_CONF);

	if(word & 0x3)
		return 1;
	else
		return 0;
	
}

void
set_nmi_enable(uint8 nmi_enable){
	uint32 word;
	/*Config NMI0*/
	word = regRead32(CR_INTC_NMI0IMR0);	
	if(nmi_enable)
		word |= 0x200;
	else
		word &= ~0x200;
	regWrite32(CR_INTC_NMI0IMR0, word);	

	#if 0
	/*Config NMI1*/
	word = regRead32(CR_INTC_NMI1IMR0);
	if(nmi_enable)
		word |= 0x200;
	else
		word &= ~0x200;
	regWrite32(CR_INTC_NMI1IMR0, word);
	#endif

}

#if defined(TCSUPPORT_DYING_GASP)
EXPORT_SYMBOL(timerSet);
EXPORT_SYMBOL(timer_WatchDogConfigure);
#endif
#define get_current_vpe()   \
	((read_c0_tcbind() >> TCBIND_CURVPE_SHIFT) & TCBIND_CURVPE)

extern void tc3162_enable_irq(unsigned int irq);

static void mips_timer_dispatch(void)
{
	//pr_info("\nmips_timer_dispatch, Status= %08x", read_c0_status());
	do_IRQ(SI_TIMER_INT);
}

static void mips_perf_dispatch(void)
{
	do_IRQ(cp0_perfcount_irq);
}

extern int (*perf_irq)(void);

/*
 * Estimate CPU frequency.  Sets mips_hpt_frequency as a side-effect
 */
static unsigned int __init estimate_cpu_frequency(void)
{
	unsigned int count;
	unsigned long flags;
	unsigned int start;

	local_irq_save(flags);

	/* Start r4k counter. */
	start = read_c0_count();

	/* delay 1 second */
	delay1ms(100);

	count = read_c0_count() - start;

	/* restore interrupts */
	local_irq_restore(flags);

	count*=10;
	count += 5000;    /* round */
	count -= count%10000;

	mips_hpt_frequency = count;

	/* on 34K, 2 cycles per count */
	count *= 2;

	return count;
}

irqreturn_t mips_perf_interrupt(int irq, void *dev_id)
{
	return perf_irq();
}

static struct irqaction perf_irqaction = {
	.handler = mips_perf_interrupt,

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	.flags =  IRQF_PERCPU,
#else
	.flags = IRQF_DISABLED | IRQF_PERCPU,
#endif	
	.name = "performance",
};
extern struct clocksource clocksource_mips;

#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
extern void xpon_phy_tx_disable(void);
#endif

#if defined(TCSUPPORT_DYING_GASP) && (defined(CONFIG_MIPS_RT63365) && !(defined(TCSUPPORT_CPU_MT7510)||defined(TCSUPPORT_CPU_MT7520) || defined(TCSUPPORT_CPU_MT7505)))
irqreturn_t real_watchdog_timer_interrupt(int irq, void *dev_id)
#else
irqreturn_t watchdog_timer_interrupt(int irq, void *dev_id)
#endif
{
	uint32 word;

	word = regRead32(CR_TIMER_CTL); 
	word &= 0xffc0ffff;
	word |= 0x00200000;
	regWrite32(CR_TIMER_CTL, word);

	/* The KERN_ALERT will stop printk ring buffer mode. 
	 * This is used for flush ring buffer message to console.
	 */
	printk(KERN_ALERT "watchdog timer interrupt to CPU%d\n", smp_processor_id());

#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
	xpon_phy_tx_disable();
#endif

#ifdef CONFIG_TC3162_ADSL
    /* stop adsl */
	if (adsl_dev_ops)
	    adsl_dev_ops->set(ADSL_SET_DMT_CLOSE, NULL, NULL); 
#endif

#if defined(CONFIG_MIPS_TC3262) && defined(TCSUPPORT_POWERSAVE_ENABLE)
	if(isRT63365){
		word = regRead32(CR_AHB_CLK);
		word |= 0x57e1;//restore ahb clk to default value
		regWrite32(CR_AHB_CLK, word);
	}
#endif
	dump_stack();
    #ifdef TCSUPPORT_NEW_WDOG
    wdog_msg_print();
    #endif

#if defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_MT7510)
	word = regRead32(CR_DRAMC_CONF);
	word &= ~(0x1<<2);
	regWrite32(CR_DRAMC_CONF, word);
#endif

	return IRQ_HANDLED;
}
//only 63365 need another watchdog function in IMEM
#if defined(TCSUPPORT_DYING_GASP) && (defined(CONFIG_MIPS_RT63365) && !(defined(TCSUPPORT_CPU_MT7510)||defined(TCSUPPORT_CPU_MT7520)||defined(TCSUPPORT_CPU_MT7505)))
__IMEM
irqreturn_t watchdog_timer_interrupt(int irq, void *dev_id){
	
	unsigned int word;
	word = regRead32(0xbfb00834);
        word &= ~(1<<18);//enable spi
        regWrite32(0xbfb00834, word);
	word = regRead32(0xbfb00040);
        word &= ~(1<<0); //enable ddr device
        regWrite32(0xbfb00040, word);

	return real_watchdog_timer_interrupt(irq, dev_id);
}
#endif
static struct irqaction watchdog_timer_irqaction = {
	.handler = watchdog_timer_interrupt,
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)		
	.flags = IRQF_DISABLED ,
#endif	
	.name = "watchdog",
};

static void watchdog_timer_dispatch(void)
{
	do_IRQ(TIMER5_INT);
}

/************************************************************************
*                   B U S  T I M E O U T  I N T E R R U P T  
*************************************************************************
*/

irqreturn_t bus_timeout_interrupt(int irq, void *dev_id)
{
	uint32 reg;
	uint32 addr;
	uint32 isBusTout=0;
	
	/* read to clear interrupt */
	if(isMT751020 || isMT7505 || isEN751221 || isEN751627||isEN7580)
	{
		if(isMT7505 || isEN751221 || isEN751627||isEN7580){
			isBusTout = (regRead32(CR_PRATIR)&0x1);
			regWrite32(CR_PRATIR, 1);
		}
		else
		regWrite32(CR_PRATIR, 0);
		addr =  regRead32(CR_ERR_ADDR);
                addr &= ~((1 << 30) | (1 << 31));
                if (isBusTout)
			printk("pbus timeout interrupt ERR ADDR=%08lx\n", addr);
                else
			printk("unknown bus timeout interrupt ERR ADDR=%08lx\n", addr);
        
		dump_stack();	

#if 0//def CONFIG_PCI
		if(addr >= 0x1fb80000 && addr <= 0x1fb80064)
		{
			pcieReset();
			pcieRegInitConfig();
			setahbstat(1);
		}
#endif
	}
	else
	{
	reg = regRead32(CR_PRATIR);
	reg &= ~((1 << 30) | (1 << 31));
	printk("bus timeout interrupt ERR ADDR=%08lx\n", reg);
	dump_stack();	
	
#ifdef CONFIG_PCI
	pcieReset();
	pcieRegInitConfig();
	setahbstat(1);
#endif
	}
	
	return IRQ_HANDLED;
}

#ifdef TCSUPPORT_CPU_EN7528
/* no chance to run this ISR because rbus timeout interrupt will be set as NMI */
irqreturn_t rbus_timeout_interrupt(int irq, void *dev_id)
{	
	return IRQ_HANDLED;
}
#endif

static struct irqaction bus_timeout_irqaction = {
	.handler = bus_timeout_interrupt,
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)		
	.flags = IRQF_DISABLED ,
#endif	
	.name = "bus timeout",
};

#ifdef TCSUPPORT_CPU_EN7528
static struct irqaction rbus_timeout_irqaction = {
	.handler = rbus_timeout_interrupt,
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)	
	.flags = IRQF_DISABLED ,
#endif
	.name = "rbus timeout",
};
#endif

static void bus_timeout_dispatch(void)
{
	do_IRQ(BUS_TOUT_INT);
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)	
unsigned int  get_c0_compare_int(void)
#else
unsigned int __cpuinit get_c0_compare_int(void)
#endif	
{
#ifdef TCSUPPORT_MIPS_1004K
    /* 1. this function will return back to r4k_clockevent_init in arch/mips/kernel/cevt-r4k.c,
     *    then setup_irq will be called
     * 2. during setup_irq, ecnt_gic_unmask_irq in arch/mips/econet/irq.c will be called to 
     *    enable GIC mask for four extermal cpu timers 
     * 3. only cpu0 will use setup_irq to register the timer interupt with irqNum as 
     *    SI_TIMER_INT and all cpus shared the irqNum (and ISR) */
    mips_cpu_timer_irq = SI_TIMER_INT;

    /* enable interrupt masks for external cpu timer 1/2/3. 
     * external cpu timer 0's interrupt mask will be enabled later in setup_irq */
     if (smp_processor_id() > 0) {
        GIC_SET_INTR_MASK(timers_intSrcNum[smp_processor_id()]);
     }
#else
	if ((get_current_vpe()) && !vpe1_timer_installed) {
		tc3162_enable_irq(SI_TIMER1_INT);
		vpe1_timer_installed++;
	}

	if (vpe1_timer_installed == 0) {
		if (cpu_has_veic) 
			set_vi_handler(SI_TIMER_INT, mips_timer_dispatch);	
	}
	mips_cpu_timer_irq = SI_TIMER_INT;
#endif
	
	return mips_cpu_timer_irq;
}
static cycle_t cputmr_hpt_read(void)
{
	return regRead32(cputmr_cnt[0]);
}
static void __init cputmr_hpt_timer_init(void)
{
    unsigned int tmp, i, j=2;

#ifdef TCSUPPORT_MIPS_1004K
    j=4;
#endif

    for (i=0; i<j; i++)
        regWrite32(cputmr_cnt[i], 0x0);

	expirelo[0] = cycles_per_jiffy;
    for (i=1; i<j; i++)
	    expirelo[i] = expirelo[0];

    for (i=0; i<j; i++)
	    regWrite32(cputmr_cmr[i], expirelo[i]);

	tmp = regRead32(CR_CPUTMR_CTL);
	tmp |= (1<<1)|(1<<0);
	regWrite32(CR_CPUTMR_CTL, tmp);	
#ifdef TCSUPPORT_MIPS_1004K
    /* after enable external CPU timers 3 & 2, intSrc 36,37 serve for them */
    tmp = regRead32(CR_CPUTMR_23_CTL);
    tmp |= (1<<1)|(1<<0);
    regWrite32(CR_CPUTMR_23_CTL, tmp);    
#endif
}
static void cputmr_timer_ack(void)
{
	int cpu=0;
	int vpe=0;
	
	/*As kernel3.18.21,there is a one - to - one relationship between cpu and vpe,and cpu_data[cpu].vpe_id is not correct,so don't use it*/	
#ifdef CONFIG_MIPS_MT_SMP
	cpu = smp_processor_id();
	vpe = cpu;
#endif

#ifndef TCSUPPORT_MIPS_1004K
#if defined(MIPS_CPS) || defined(MIPS_CMP)
	cpu = smp_processor_id();
	vpe = cpu_data[cpu].vpe_id;
#endif
#endif
	/* Ack this timer interrupt and set the next one.  */
	expirelo[vpe] += cycles_per_jiffy;

	/* Check to see if we have missed any timer interrupts.  */
	while (unlikely((regRead32(cputmr_cnt[vpe]) - expirelo[vpe]) < 0x7fffffff)) {
		/* missed_timer_count++; */
		expirelo[vpe] += cycles_per_jiffy;
		cpu_timer_loss[cpu]++;
	}
	/* update CR_CPUTMR_CMR */
	regWrite32(cputmr_cmr[vpe], expirelo[vpe]);
#ifdef TCSUPPORT_BONDING
	if (isMT751020) {
		if(bondaddr != 0) {
            if(tag==0) {
                tag = 1;
                regWrite32(bondaddr, 1);
            }else {
                tag = 0;
                regWrite32(bondaddr, 0);
            }
		}
	}
#endif
}

void (*mips_timer_ack)(void) = NULL;

void ecnt_mips_time_ack(int cpu){
	
	if (isRT63165 || isRT63365 || isMT751020 || isEN751221 || isEN751627||isEN7580) 
	{				
		mips_timer_ack();
	}
}
void __init tc3162_time_init(void)
{

	pr_info("\r\ntc3162_time_init: Init bus timeout and watchdog\r\n");
	timerSet(1, TIMERTICKS_10MS, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);

	if (isRT63165 || isRT63365 || isMT751020 || isMT7505 || isEN751221 || isEN751627||isEN7580) {
		/* watchdog timer */
		/* set count down 3 seconds to issue interrupt */
		regWrite32(CR_WDOG_THSLD, ((3 * TIMERTICKS_1S * SYS_HCLK) * 500)); // (3 * TIMERTICKS_1S * SYS_HCLK) * 1000 / 2
		if (cpu_has_vint)
			set_vi_handler(TIMER5_INT, watchdog_timer_dispatch);
#ifdef CONFIG_MIPS_MT_SMTC
		setup_irq_smtc(TIMER5_INT, &watchdog_timer_irqaction, 0x0);
#else
		setup_irq(TIMER5_INT, &watchdog_timer_irqaction);
#endif
    #if defined(TCSUPPORT_NEW_WDOG) && defined(TCSUPPORT_MIPS_1004K)
        /* bind watchdog Intr to CPU1.
         * when CPU0 hangs at ISR, all wdogs will stop. 
         * Then hw wdog will be triggered and wdog ISR will be sent to CPU1
          to dump CPU0's hang point. */
        GIC_SH_MAP_TO_VPE_SMASK(gicVecPlus1_to_intSrc(TIMER5_INT), 1);
    #endif

		/* setup bus timeout interrupt */
   		//VPint(CR_MON_TMR) |= ((1<<30) | (0xff));
		if(isMT751020 || isMT7505 || isEN751221 || isEN751627||isEN7580)
		{
			if (isEN7528) { /* setup bus timeout timer as about 1 sec */
                		if (isFPGA) { /* FPGA bus_clk: 64MHz */
					regWrite32(CR_MON_TMR, 0x4000000); /* unit: one bus clock */
					regWrite32(CR_MON_TMR, 0x44000000); /* enable it */
				}
				else { /* ASIC bus_clk: 235MHz */
					regWrite32(CR_MON_TMR, 0x10000000); 
					regWrite32(CR_MON_TMR, 0x50000000);
				}
			}
			else {
				regWrite32(CR_MON_TMR, 0xcfffffff);
				if(isMT7505 || isEN751221 || isEN751627||isEN7580)
					regWrite32(CR_BUSTIMEOUT_SWITCH, 0xffffffff);
				else
					regWrite32(CR_BUSTIMEOUT_SWITCH, 0xfdbfffff);//switch off usb phy(bit22/bit25) control because hw issue
			}
		}

		if (cpu_has_vint)
			set_vi_handler(BUS_TOUT_INT, bus_timeout_dispatch);
#ifdef CONFIG_MIPS_MT_SMTC
		setup_irq_smtc(BUS_TOUT_INT, &bus_timeout_irqaction, 0x0);
#else
		setup_irq(BUS_TOUT_INT, &bus_timeout_irqaction);
#endif
        #ifdef TCSUPPORT_CPU_EN7528
        setup_irq(RBUS_TOUT_INTR, &rbus_timeout_irqaction);
        /* set rbus timeout intr as NMI */
        GICWRITE(GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(gicVecPlus1_to_intSrc(RBUS_TOUT_INTR))), GIC_MAP_TO_NMI_MSK);
        /* enable rbus timeout mechanism */
        regWrite32(RBUS_TIMEOUT_CFG0, 0x3ffffff);
        regWrite32(RBUS_TIMEOUT_CFG1, 0x3ffffff);
        regWrite32(RBUS_TIMEOUT_CFG2, 0x3ffffff);
        regWrite32(RBUS_TIMEOUT_STS0, 0x80000000);
        printk("set rbus timeout as NMI then enable it\n");
        #endif
	} 

}
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)	
extern void (*board_time_init)(void);
#endif
void __init plat_time_init(void)
{
	unsigned int est_freq = 0;


	if(board_time_init)
		board_time_init();
	timerSet(1, TIMERTICKS_10MS, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);

	est_freq = estimate_cpu_frequency ();

	printk("CPU frequency %d.%02d MHz\n", est_freq/1000000,
	       (est_freq%1000000)*100/1000000);

    	cpu_khz = est_freq / 1000;
		
	if (isRT63165 || isRT63365 || isMT751020 || isMT7505 ||isEN751221 || isEN751627||isEN7580) {		
		
		/* enable CPU external timer */
		clocksource_mips.read = cputmr_hpt_read;
		mips_hpt_frequency = CPUTMR_CLK;

		mips_timer_ack = cputmr_timer_ack;

		printk("plat_time_init: Entered, mips_timer_ack ptr is [%p]\r\n", mips_timer_ack);

		/* Calculate cache parameters.  */
		cycles_per_jiffy =
			(mips_hpt_frequency + HZ / 2) / HZ;

		cputmr_hpt_timer_init();
		
		printk(" Using %u.%03u MHz high precision timer.\n",
		   ((mips_hpt_frequency + 500) / 1000) / 1000,
		   ((mips_hpt_frequency + 500) / 1000) % 1000);
	}
}

