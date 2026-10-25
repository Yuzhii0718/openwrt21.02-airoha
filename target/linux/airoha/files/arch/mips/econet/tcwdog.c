/*
** $Id: tcwdog.c,v 1.2 2010/12/09 13:18:35 xmdai_nj Exp $
*/
/************************************************************************
 *
 *	Copyright (C) 2006 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/
/*
** $Log: tcwdog.c,v $
** Revision 1.5  2011/09/23 02:04:50  shnwind
** Add rt63365 support
**
** Revision 1.4  2011/07/07 07:55:51  shnwind
** RT63260 & RT63260 auto_bench support
**
** Revision 1.3  2011/06/03 02:04:23  lino
** add RT65168 support
**
** Revision 1.2  2010/12/09 13:18:35  xmdai_nj
** #7955:When doing upgrade firmware in web page, it can not reboot.
**
** Revision 1.1.1.1  2010/04/09 09:39:13  feiyan
** New TC Linux Make Flow Trunk
**
** Revision 1.1.1.1  2009/12/17 01:43:39  josephxu
** 20091217, from Hinchu ,with VoIP
**
** Revision 1.1.1.1  2007/04/12 09:42:02  ian
** TCLinuxTurnkey2007
**
** Revision 1.2  2006/07/06 07:24:23  lino
** update copyright year
**
** Revision 1.1.1.1  2005/11/02 05:45:19  lino
** no message
**
*/
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
//#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/tc3162/tc3162.h>

#include <asm/tc3162/TCIfSetQuery_os.h>
#include <linux/delay.h>
#include <linux/watchdog.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif
#ifdef TCSUPPORT_NEW_WDOG
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/cpumask.h>
#include <asm/mipsmtregs.h>
#include <asm/stacktrace.h>

#define CPU_NUM             4
#define WDOG_TIME_UP        40
#define WDOG_RELOAD_VAL     50
#define WDOG_TIMETICK_1S    (100 * TIMERTICKS_10MS)

#define MAX_WDOG_TIMEOUT	30
#define MIN_WDOG_TIMEOUT	5

/* if TCSUPPORT_NEW_WDOG */
#define WDOG0_MINOR		126	/* Wdog0 */
#define WDOG1_MINOR		127	/* Wdog1 */
#define WDOG2_MINOR		128	/* Wdog2 */
#define WDOG3_MINOR		129	/* Wdog3 */
/* endif TCSUPPORT_NEW_WDOG */

#endif

#if defined(TCSUPPORT_CPU_EN7580)
enum {
	NORMAL_REBOOT = 0,
	ABNORMAL_WATCHDOG_REBOOT,
	COLD_REBOOT,
	EXCEPTION_RESULT,
};
#endif

#ifdef CONFIG_WATCHDOG_NOWAYOUT
static int nowayout = 1;
#else
static int nowayout = 0;
#endif

#if 0
MODULE_PARM(nowayout,"i");
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default=CONFIG_WATCHDOG_NOWAYOUT)");
#endif
#ifdef L2CACHE_LOCK_CODE
#define GDMP_SRAM_BOOT2_BASE    0x9fa41000
#define CACHE_LINE_SIZE         (32)
extern int l2c_lock_range(unsigned long start, unsigned long end);
extern int l2c_release_all(void);
extern void l2c_dump_range (unsigned long start, unsigned long end);
extern void l2c_dump_all (void);
extern void l2c_dump_info (void);
unsigned int boot2_lock_size=0;
#endif
extern unsigned int cpu_timer_loss[4];

static int watchdog_enabled = 0;
static int wdog_kernel_kick_enabled = 1;

#if defined(TCSUPPORT_CPU_EN7580)
static int reboot_status = EXCEPTION_RESULT;
#endif

extern void timer_Configure(uint8  timer_no, uint8 timer_enable, uint8 timer_mode, uint8 timer_halt);
extern void timerSet(uint32 timer_no, uint32 timerTime, uint32 enable, uint32 mode, uint32 halt);

extern void timer_WatchDogConfigure(uint8 tick_enable, uint8 watchdog_enable);

#if defined(TCSUPPORT_CT_JOYME2)
extern int panic_write2(const char* buf);
#endif

static spinlock_t  protect_lock;
#define PPE_DFP_CPORT	  0xbfb50e48
#define PPE_TB_CFG	      0xbfb50e1c

#ifdef TCSUPPORT_NEW_WDOG
typedef struct wdogCounters_s {
    unsigned int wdog_cnt;                 /*record cnt for itself*/
    unsigned int wdog_prev_cnt[CPU_NUM];   /*record other cpus' prev cnt*/
    unsigned int wdog_debt[CPU_NUM];            /*record other cpus' status*/
    unsigned int wdog_enabled;                  /*enable flag for itself*/
    unsigned int wdog_kick_cnt;            /*record kick cnt for itself*/
    unsigned int wdog_kick_prev_cnt[CPU_NUM];/*record other cpus' prev kick cnt*/
    
} wdogCounters_t;

static int cpu_nr;
static unsigned int cpu_counterpart[CPU_NUM]={1,0,3,2};
static wdogCounters_t wdogCnts[CPU_NUM];
static unsigned int tcwdog_cnt;
static unsigned int wdog_kick_cnt_limit=50;
static spinlock_t  wdog_print_lock;
static int msg_print_done=0;
/* mcast_offload_cnt_load_value has smaller value than wifi_rx_cnt_load_value.
 * This means that Mcast WAN->WIFI path has larger burden than WIFI->LAN path */
static int wifi_rx_cnt_load_value=2000;
static int wifi_rx_cnt=2000;
static int mcast_offload_cnt_load_value=1200;
static int mcast_offload_cnt=1200;

static int wdog_timeout = WDOG_RELOAD_VAL;

extern void mips_mt_regdump(unsigned long mvpctl);
#endif
#ifdef TCSUPPORT_MIPS_1004K
extern void gic_edge_nmi_send (int cpu);
extern void interrupt_nmi_set (unsigned int intSrc);
#endif


void 
attack_protect_set(int active, int mode){
	unsigned int value=0;
	unsigned int val_1=0;
	unsigned int flags=0;

	spin_lock_irqsave(&protect_lock, flags);

	/*active : o , off, 1 on. mode: 1 wan protect, 2, lan protect, 0, not set*/
	if(active == 1){
		if(mode == 1){
			/*disable keep alive*/
			value = regRead32(PPE_TB_CFG);
			value &= ~(0x3000);
			regWrite32(PPE_TB_CFG, value);
			/*default port 2 to drop*/
			value = regRead32(PPE_DFP_CPORT);
			value &= ~(0x700);
			value |= 0x700;
			regWrite32(PPE_DFP_CPORT, value);
		}
		else if(mode == 2){
			/*disable keep alive*/
			value = regRead32(PPE_TB_CFG);
			value &= ~(0x3000);
			regWrite32(PPE_TB_CFG, value);
			
			/*default port 1 to drop*/
			value = regRead32(PPE_DFP_CPORT);
			value &= ~(0x070);
			value |= 0x070;
			regWrite32(PPE_DFP_CPORT, value);
		}
		else{
			;/*not settings*/
		}
	}
	else{
		/*restore normal settings*/
		if(mode == 1){
			val_1 = regRead32(PPE_DFP_CPORT);
			if((val_1 & 0x70)== 0x0){ /*make sure the keep alive is setting by qdma driver*/
				/*restore the keep alive*/
				value = regRead32(PPE_TB_CFG);
				value &= ~(0x3000);
				value |= 0x3000;
				regWrite32(PPE_TB_CFG, value);
			}
			/*restore p2 to qdma port*/
			value = val_1;
			value &= ~(0x700);
			value |= 0x500;
			regWrite32(PPE_DFP_CPORT, value);
		}
		else if(mode == 2){ 
			val_1 = regRead32(PPE_DFP_CPORT);
			if((val_1 & 0x700) == 0x500){ /*make sure the keep alive is setting by femac driver*/
				/*restore the keep alive*/
				value = regRead32(PPE_TB_CFG);
				value &= ~(0x3000);
				value |= 0x3000;
				regWrite32(PPE_TB_CFG, value);
			}
			/*restore p1 to cpu port*/
			value = val_1;
			value &= ~(0x70);
			regWrite32(PPE_DFP_CPORT, value);
		}
		else{
			;/*not settings*/
		}
	}
	spin_unlock_irqrestore(&protect_lock, flags);

}
EXPORT_SYMBOL(attack_protect_set);

#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
extern void xpon_phy_tx_disable(void);
#endif
int iswatchDogReset = 0;
int isResetByProc = 0;

void watchDogReset(void)
{
	static char buf[1024];
	iswatchDogReset = 1;
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
	 xpon_phy_tx_disable();
#endif

#if defined(TCSUPPORT_CT_JOYME2)
	if(isResetByProc != 1)
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
	    panic_write2(buf);
#endif
	}
#endif

#ifdef CONFIG_TC3162_ADSL
    /* stop adsl */
	if (adsl_dev_ops)
	    adsl_dev_ops->set(ADSL_SET_DMT_CLOSE, NULL, NULL); 
#endif

#if defined(CONFIG_MIPS_TC3262) && defined(TCSUPPORT_POWERSAVE_ENABLE)
	if(isRT63365)
		VPint(CR_AHB_CLK) |= 0x57e1;//restore ahb clk to default value
#endif

#if defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_MT7510)
    disable_all_interrupts();

	VPint(CR_TIMER_CTL) = 0x0;
#endif

#if defined(TCSUPPORT_WLAN_MT7592_PCIE) && defined(TCSUPPORT_CPU_MT7520)
	printk("0xbfb00834=0xeff88ce0 \n");
	printk("0xbfb00380 reset phy \n");
    disable_all_interrupts();

	VPint(CR_TIMER_CTL) = 0x0;
#endif
    #ifdef TCSUPPORT_NEW_WDOG
    /*prevent tc3162wdog_kick from being called to reload tcwdog in other places, such as in net_rx_action */
    watchdog_enabled=0;
    #endif
/*watchdog reset*/
	timerSet(5, 10 * TIMERTICKS_10MS, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);
	timer_WatchDogConfigure(ENABLE, ENABLE);

#if defined(TCSUPPORT_WLAN_MT7592_PCIE) && defined(TCSUPPORT_CPU_MT7520)
	VPint(0xbfbd0040) = 0x0;
	mdelay(10);
	VPint(0xbfb00834) = 0xeff88ce0;
	VPint(0xbfb00380) &= ~(1<<25);
	udelay(1) ;
	VPint(0xbfb00380) |= (1<<25);
#endif

#if defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_MT7510)
	VPint(CR_DRAMC_CONF) &= ~(0x1<<2);
#endif

	while(1);
//	_machine_restart();
}

void tc3162wdog_kick(void)
{
    #ifdef TCSUPPORT_NEW_WDOG
    if ((watchdog_enabled==0) || (wdog_kernel_kick_enabled==0))
        return;
    #endif

	if(iswatchDogReset){
		return;
	}
      /* Because kernel 3.18 don't support the type before 63365*/
	regWrite32(CR_WDOG_RLD, 0x1); 
    #ifdef TCSUPPORT_NEW_WDOG
    /* tc3162wdog_kick is called in many places (such as in net_rx_action) 
     * to prevent hw wdog from rebooting when CPU is busy at handling packets.
     * Therefore, wdog_kick_cnt can be used to distinguish if certain CPU
     * is busy at handling packets */
    wdogCnts[smp_processor_id()].wdog_kick_cnt++;
    #endif
}
EXPORT_SYMBOL(tc3162wdog_kick);

void tc3162wdog_kick2(void)
{
    #ifdef TCSUPPORT_NEW_WDOG
    if (watchdog_enabled==0)
        return;
    #endif

	if(iswatchDogReset){
		return;
	}
      /* Because kernel 3.18 don't support the type before 63365*/
	regWrite32(CR_WDOG_RLD, 0x1);
    #ifdef TCSUPPORT_NEW_WDOG
    /* tc3162wdog_kick is called in many places (such as in net_rx_action)
     * to prevent hw wdog from rebooting when CPU is busy at handling packets.
     * Therefore, wdog_kick_cnt can be used to distinguish if certain CPU
     * is busy at handling packets */
    wdogCnts[smp_processor_id()].wdog_kick_cnt++;
    #endif
}
EXPORT_SYMBOL(tc3162wdog_kick2);

/* handle open device */

static int tc3162wdog_open(struct inode *inode, struct file *file)
{
	/*	Allow only one person to hold it open */
	if(watchdog_enabled)
		return -EBUSY;
	if (nowayout) {
		printk("Watchdog cannot be stopped once started. \n");
	}
  
    #ifdef TCSUPPORT_NEW_WDOG
    /* originally, hw wdog' timeout is 20 secs. 
     * So, set new wdogs' timeout as 20 secs, and extend hw wdog' timeout to 30 secs */
    timerSet(5, WDOG_RELOAD_VAL * WDOG_TIMETICK_1S, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);
    tcwdog_cnt=0;
    #else
	timerSet(5, 2000 * TIMERTICKS_10MS, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);
    #endif
	timer_WatchDogConfigure(ENABLE, ENABLE);

	watchdog_enabled=1;
	printk("TC3162 hardware watchdog initialized\n");
	return 0;
}

static long tc3162wdog_ioctl (struct file *file, unsigned int cmd, unsigned long arg)
{
	int time, ret = -ENOTTY;

	switch (cmd)
	{
#ifdef TCSUPPORT_NEW_WDOG
		case WDIOC_SETTIMEOUT:
			ret = get_user(time, (int *)arg);
			if (ret)
				break;

			if (time < MIN_WDOG_TIMEOUT || time > MAX_WDOG_TIMEOUT)
			{
				ret = -EINVAL;
				break;
			}

			wdog_timeout = time;

			timerSet(5, (wdog_timeout * WDOG_TIMETICK_1S), ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);

			/* Fall through */
		case WDIOC_GETTIMEOUT:
			ret = put_user(wdog_timeout, (int *)arg);
			break;
#endif

		default:
			printk ("Unsupport event: %x\n", cmd);
			break;
	}

	return ret;
}

static int tc3162wdog_release(struct inode *inode, struct file *file)
{
	/*
	 *	Shut off the watchdog
	 * 	Lock it in if it's a module and we set nowayout
	 */
	if (nowayout) {
		printk(KERN_CRIT "Watchdog cannot be stopped once started! \n");
	} else {
		/* Stop watchdog timer */
		timer_WatchDogConfigure(DISABLE, DISABLE);

		watchdog_enabled = 0;
		printk("TC3162 hardware watchdog stopped\n");
	}
	return 0;
}

static ssize_t tc3162wdog_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
    #ifdef TCSUPPORT_NEW_WDOG
	if (len && watchdog_enabled) {
		tc3162wdog_kick2();

        /* add counter for tcwdog, so we can check its status via proc */
        tcwdog_cnt++;
	}
    #else
	if (len)
		tc3162wdog_kick();
    #endif
	return len;
}

#ifdef TCSUPPORT_NEW_WDOG
static int wdog_open(struct inode *inode, struct file *file)
{
    unsigned int cpu = smp_processor_id();
    int i;

    wdogCnts[cpu].wdog_cnt = 0;
    wdogCnts[cpu].wdog_kick_cnt = 0;

    for (i=0; i<cpu_nr; i++) {
        wdogCnts[cpu].wdog_prev_cnt[i] = 0;
        wdogCnts[cpu].wdog_debt[i] = 0;
        wdogCnts[cpu].wdog_kick_prev_cnt[i] = 0;
    }  


    wdogCnts[cpu].wdog_enabled = 1;

	return 0;
}

static int wdog_release(struct inode *inode, struct file *file)
{
    unsigned int i;

    /* when keyin "kill `pidof tcwdog`" in console, CPU3 will kill tcwdog app and
     * all of its child pthreads (wdog apps). wdog_release will triggered 4 times
     * but all by CPU3, so use "for" loop to disable all wdogs */
    for (i=0; i<cpu_nr; i++) {
        wdogCnts[i].wdog_enabled = 0;
    }

	return 0;
}


void wdog_msg_print(void)
{
    int i;
    unsigned int cpu = smp_processor_id();
    unsigned long flags;

    spin_lock_irqsave((spinlock_t*)&wdog_print_lock, flags);


    for (i=0; i<cpu_nr; i++)
        printk("wdogCnts[%d].wdog_debt[%d] == %d\n", cpu, i, wdogCnts[cpu].wdog_debt[i]);

    for (i=0; i<cpu_nr; i++)
        printk("wdogCnts[%d].wdog_cnt == 0x%x\n", i, wdogCnts[i].wdog_cnt);
    
    printk("tcwdog_cnt  == 0x%x\n", tcwdog_cnt);

    for (i=0; i<cpu_nr; i++)
        printk("wdogCnts[%d].wdog_prev_cnt[%d] == 0x%x\n", cpu, i, wdogCnts[cpu].wdog_prev_cnt[i]);

    for (i=0; i<cpu_nr; i++)
        printk("wdogCnts[%d].wdog_kick_cnt == 0x%x\n", i, wdogCnts[i].wdog_kick_cnt);


    mips_mt_regdump(0);

    spin_unlock_irqrestore((spinlock_t*)&wdog_print_lock, flags);

    return;
}

/* check if certian cpu is busy at handling packets via its wdog_kick_cnt counter.
 * Note1: "cur_cpu" is the supervisor CPU and "cpu" is monitored CPU 
 * Note2: since different supervisor CPUs have different view of monitored CPUs,
 *        each supervisor CPU should record wdog_kick_prev_cnt counters for its own use */
static int isCpuBusy(unsigned int cpu)
{
    unsigned int cur_cpu = smp_processor_id();
    unsigned long int wdog_kick_diff_cnt;
    
    if (wdogCnts[cpu].wdog_kick_cnt > wdogCnts[cur_cpu].wdog_kick_prev_cnt[cpu])
        wdog_kick_diff_cnt = wdogCnts[cpu].wdog_kick_cnt - wdogCnts[cur_cpu].wdog_kick_prev_cnt[cpu];
    else
        wdog_kick_diff_cnt = wdogCnts[cur_cpu].wdog_kick_prev_cnt[cpu] - wdogCnts[cpu].wdog_kick_cnt;

    wdogCnts[cur_cpu].wdog_kick_prev_cnt[cpu] = wdogCnts[cpu].wdog_kick_cnt;

    if (wdog_kick_diff_cnt > wdog_kick_cnt_limit)
        return 1;
    else
        return 0;
}

static ssize_t wdog_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
    unsigned int cpu = smp_processor_id(); /*the supervisor CPU*/
    unsigned int i;
    int dead_cpu = -1;
#ifdef TCSUPPORT_MIPS_1004K
    unsigned int counterpart;
#endif

    if (wdogCnts[cpu].wdog_enabled==0)
        return len;

    wdogCnts[cpu].wdog_cnt++;

    /* check other CPUs' status */
    for (i=0; i<cpu_nr; i++) {
        
        if (i==cpu) continue;

        
        if ((wdogCnts[cpu].wdog_prev_cnt[i] == wdogCnts[i].wdog_cnt) && 
            (isCpuBusy(i)==0) /* if the monitored CPU is busy (due to traffic), don't count "wdog_debt" for it */)
           wdogCnts[cpu].wdog_debt[i]++;
        else {
            wdogCnts[cpu].wdog_prev_cnt[i] = wdogCnts[i].wdog_cnt;
            wdogCnts[cpu].wdog_debt[i] = 0; /* reset to 0 */
        }

        /* a cpu is sentanced to death only if its wdog_cnt hasn't been updated in 20 consecutive secs
         * and traffic is not busy during the 20 consecutive secs */
        if (wdogCnts[cpu].wdog_debt[i]==WDOG_TIME_UP) {
            dead_cpu = i;
            break;
        }
    }

    if (dead_cpu==-1) {
        return len;
    }
#ifdef TCSUPPORT_MIPS_1004K
    else { 
        /* only dead CPU's counterpart can dump dead CPU's status because they are in the same CPU Core */
        counterpart = cpu_counterpart[dead_cpu];
        /* if the supervisor CPU is not the dead CPU's counterpart, check if the counterpart is in debt:
         * if not, just return, so that the counterpart has the chance to become supervisor CPU,
         * if yes, the supervisor CPU sends NMI to the dead CPU's counterpart to dump dead CPU's status */
        if (cpu!=counterpart) { 
            if (wdogCnts[cpu].wdog_debt[counterpart] > 0) {
                if (msg_print_done==0) {
                    msg_print_done=1;
                    printk("dead CPU%d's counterpart CPU%d is in debt also, so CPU%d sends MNI to counterpart to dump dead CPU \n", 
                            dead_cpu, counterpart, cpu);
                    gic_edge_nmi_send(counterpart);
                }
            }
            return len;
        }            
    }
#endif

    if (msg_print_done==0) {
        msg_print_done=1;
        printk("\nCPU%d is dead, notified by CPU%d\n", dead_cpu, cpu);
        wdog_msg_print();
        watchDogReset();
    }

	return len;
}

static struct file_operations wdog_fops = {
	owner:		THIS_MODULE,
	write:		wdog_write,
	open:		wdog_open,
	release:	wdog_release,
};

static struct miscdevice wdog0_miscdev = {
	minor:		WDOG0_MINOR,
	name:		"wdog0",
	fops:		&wdog_fops,
};

static struct miscdevice wdog1_miscdev = {
	minor:		WDOG1_MINOR,
	name:		"wdog1",
	fops:		&wdog_fops,
};

static struct miscdevice wdog2_miscdev = {
	minor:		WDOG2_MINOR,
	name:		"wdog2",
	fops:		&wdog_fops,
};

static struct miscdevice wdog3_miscdev = {
	minor:		WDOG3_MINOR,
	name:		"wdog3",
	fops:		&wdog_fops,
};

static int wdog_counters_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len, i;

	len = 0;

	for(i = 0; i < cpu_nr; i++) {
		len += sprintf(page + len, "wdogCnts[%d].wdog_cnt: 0x%x\n", i, wdogCnts[i].wdog_cnt);
	}

    len += sprintf(page + len, "tcwdog_cnt : 0x%x\n", tcwdog_cnt);

	for(i = 0; i < cpu_nr; i++) {
		len += sprintf(page + len, "wdogCnts[%d].wdog_kick_cnt: 0x%x\n", i, wdogCnts[i].wdog_kick_cnt);
	}

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

static int tcwdog_enable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[8];
	uint8 enable;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	enable = simple_strtol(val_string, NULL, 10);

    if (enable) {
        timerSet(5, WDOG_RELOAD_VAL * WDOG_TIMETICK_1S, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);
        timer_WatchDogConfigure(ENABLE, ENABLE);

        tcwdog_cnt=0;
        watchdog_enabled=1;
        printk("hardware watchdog & tcwdog are enabled\n");
    }
    else {
        timer_WatchDogConfigure(DISABLE, DISABLE);

        watchdog_enabled = 0;
        printk("hardware watchdog & tcwdog are disabled\n");
    }

	return count;
}

static int tcwdog_set_wdog_timeout_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[8];
	uint8 timeout;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	timeout = simple_strtol(val_string, NULL, 10);

	if (watchdog_enabled) {
		if ((timeout >= MIN_WDOG_TIMEOUT) && (timeout <= MAX_WDOG_TIMEOUT)) {
			wdog_timeout = timeout;

			timerSet(5, (wdog_timeout * WDOG_TIMETICK_1S), ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);

			printk ("Set wdog timeout: %d\n", wdog_timeout);
		}
		else {
			printk("Timeout %d is invalid. wdog timeout interval from %d to %d\n", timeout, MIN_WDOG_TIMEOUT, MAX_WDOG_TIMEOUT);
		}
	}
	else {
		printk ("Disable wdog now.\n");
	}

	return count;
}

static int tcwdog_kernel_kick_enable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[8];
	uint8 enable;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	enable = simple_strtol(val_string, NULL, 10);

    if (enable) {
        wdog_kernel_kick_enabled = 1;
        printk("Enable kernel kick wdog\n");
    }
    else {
        wdog_kernel_kick_enabled = 0;
        printk("Disable kernel kick wdog\n");
    }

	return count;
}

static int wdogs_enable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[16];
    int i;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

    if (cpu_nr==4)
        sscanf(val_string, "%d %d %d %d", &wdogCnts[0].wdog_enabled, &wdogCnts[1].wdog_enabled, &wdogCnts[2].wdog_enabled, &wdogCnts[3].wdog_enabled) ;
    else {
        printk("cpu_nr==%d is unexpected\n", cpu_nr);
        return count;
    }

    for (i=0; i<cpu_nr; i++)
        printk("wdogCnts[%d].wdog_enabled: %d\n", i, wdogCnts[i].wdog_enabled);

	return count;
}

static int wdog_kick_cnt_limit_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[16];
    unsigned int old_value = wdog_kick_cnt_limit;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

    sscanf(val_string, "%d", &wdog_kick_cnt_limit) ;

    printk("wdog_kick_cnt_limit:%d (old_value:%d)\n", wdog_kick_cnt_limit, old_value);

	return count;
}

/* source==1 for WIFI->LAN/WAN offload
 * source==2 for Mcast WAN->WIFI offload */
void wdog_kick_api(int source)
{
    if (source==1) { /* WIFI->LAN/WAN offload */
        wifi_rx_cnt--;
        if (wifi_rx_cnt<=0) {
            wifi_rx_cnt=wifi_rx_cnt_load_value;
            tc3162wdog_kick();
        }
    }
    else if (source==2) { /* Mcast WAN->WIFI offload */
        mcast_offload_cnt--;
        if (mcast_offload_cnt<=0) {
            mcast_offload_cnt=mcast_offload_cnt_load_value;
            tc3162wdog_kick();
        }
    }

    return;
}
EXPORT_SYMBOL(wdog_kick_api);

static int wdog_wifi_rx_cnt_load_value_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[8];

	if (count > sizeof(get_buf) - 1)
		return -EINVAL;

    memset(get_buf, 0, sizeof(get_buf));

	if (copy_from_user(get_buf, buffer, count))
		return -EFAULT;

    get_buf[count] = '\0';

    sscanf(get_buf, "%d", &wifi_rx_cnt_load_value);

    printk("wifi_rx_cnt_load_value:%d\n", wifi_rx_cnt_load_value);
    
    return count;
}

static int wdog_mcast_offload_cnt_load_value_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[8];

	if (count > sizeof(get_buf) - 1)
		return -EINVAL;

    memset(get_buf, 0, sizeof(get_buf));

	if (copy_from_user(get_buf, buffer, count))
		return -EFAULT;

    get_buf[count] = '\0';

    sscanf(get_buf, "%d", &mcast_offload_cnt_load_value);

    printk("mcast_offload_cnt_load_value:%d\n", mcast_offload_cnt_load_value);
    
    return count;
}
#else

void wdog_kick_api(int source)
{
    return;
}
EXPORT_SYMBOL(wdog_kick_api);
#endif

#ifdef L2CACHE_LOCK_CODE
static int l2cache_dump_range_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[32];
    unsigned int start, len;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

    sscanf(val_string, "%08x %08x", &start, &len) ;

    if (((start&0x1f)!=0) || ((len&0x1f)!=0)) {
        printk("Error: start:0x%08x or len:0x%08x is not 32B alignment\n", start, len);
        return count;
    }

    l2c_dump_range(start, start+len);

	return count;
}

static int l2cache_dump_all_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	l2c_dump_all();

	return 0;
}

static int l2cache_dump_info_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	l2c_dump_info();

	return 0;
}

static int l2cache_release_all_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[32];
    int isRelease=0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

    sscanf(val_string, "%d", &isRelease) ;

    if (isRelease)
        l2c_release_all();

	return count;
}
#endif

static struct file_operations tc3162wdog_fops = {
	owner:				THIS_MODULE,
	write:				tc3162wdog_write,
	open:				tc3162wdog_open,
	release:			tc3162wdog_release,
	unlocked_ioctl:		tc3162wdog_ioctl,
};

static struct miscdevice tc3162wdog_miscdev = {
	minor:		WATCHDOG_MINOR,
	name:		"watchdog",
	fops:		&tc3162wdog_fops,
};

#if defined(TCSUPPORT_CPU_EN7580)
static int reboot_status_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	int len = 0;

	len += sprintf(page + len, "%d\n", reboot_status);

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
#endif

static int watchdog_reset_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data){

	return 0;
}
static int watchdog_reset_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	isResetByProc = 1;
	watchDogReset();	
	return 0;
}

extern int is_nmi_enable(void);
extern void set_nmi_enable(uint8 nmi_enable);
extern void show_stack_nmi(void);

#ifdef TCSUPPORT_MIPS_1004K
static int gic_edge_nmi_send_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[16];
	int cpu;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	cpu = simple_strtol(val_string, NULL, 10);

    gic_edge_nmi_send(cpu);
        
	return count;
}

static int interrupt_nmi_set_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[16];
	unsigned int intSrc;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	intSrc = simple_strtol(val_string, NULL, 10);

    interrupt_nmi_set(intSrc);
        
	return count;
}

#else

static int nmi_enable_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data){

	int len;

	len = sprintf(page, is_nmi_enable() ? "1\n" : "0\n");
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
static int nmi_enable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	char val_string[3];
	uint8 enable;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	if(val_string[0] == 'd'){
		show_stack_nmi();
	}
	else{

		enable = simple_strtol(val_string, NULL, 10);
		set_nmi_enable(enable);
	}

	return count;
}
#endif

static int timer_interrupt_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len, i;

	len = 0;
	for(i = 0; i < 4; i++) {
		len += sprintf(page + len, "CPU%d: timer lose d'%08u\n", i, cpu_timer_loss[i]);
	}

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

static int timer_interrupt_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{	
	char val_string[64], cmd[64] ,subcmd[64];
	uint action;
	int i;

	memset(val_string, 0, (sizeof(val_string)));
	memset(cmd, 0, (sizeof(cmd)));
	memset(subcmd, 0, (sizeof(subcmd)));
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %s %x", cmd, subcmd, &action) ;
	
	if(!strcmp(cmd, "reset")) {
		for(i = 0; i < 4; i++) {
			cpu_timer_loss[i] = 0;
		}
	} else if(!strcmp(cmd, "Usage")) {
		printk("reset\n");
    }

	return count ;
}

struct proc_dir_entry *timer_procdir=NULL;

static int __init tc3162_watchdog_init(void)
{
	struct proc_dir_entry *watchdog_proc=NULL;
	struct proc_dir_entry *timer_proc=NULL;

#ifdef TCSUPPORT_NEW_WDOG
    cpu_nr = num_online_cpus();
    if ((cpu_nr!=CPU_NUM)) { /* for now, new_wdog only supports 4 cpus*/
        printk("\n%s Fail due to cpu_nr==%d\n", __func__, cpu_nr);
        return 0;
    }
#ifdef L2CACHE_LOCK_CODE
    /* lock the begining 3 cache line size of boot2 for NMI handling */
    if (isEN7528) {
        boot2_lock_size = (CACHE_LINE_SIZE*3);
        if (l2c_lock_range((unsigned long)GDMP_SRAM_BOOT2_BASE, (unsigned long)(GDMP_SRAM_BOOT2_BASE+boot2_lock_size))==0)
            printk("l2c_lock_range for 7528 boot2: OK\n");
    }
#endif    
#endif

#if defined(TCSUPPORT_CPU_EN7580)
	if (regRead32(0xBFB00240) & (1<<2))
	{
		VPint(0xBFB00240) &= ~(1<<2);

		if (regRead32(0xBFB00080) & (1<<31))
		{
			reboot_status = NORMAL_REBOOT;
		}
		else
		{
			/* 0xBFB00240[2]==1 && 0xBFB00080[31] == 0 is abnormal status. */
			reboot_status = EXCEPTION_RESULT;

			printk("reboot_status error. 0xBFB00080[31] should not be 0.\n");
		}
	}
	else if (regRead32(0xBFB00080) & (1<<31))
	{
		reboot_status = ABNORMAL_WATCHDOG_REBOOT;
	}
	else
	{
		reboot_status = COLD_REBOOT;
	}

	watchdog_proc = create_proc_entry("reboot_status", 0, NULL);
	watchdog_proc->read_proc = reboot_status_read_proc;
#endif

	watchdog_proc = create_proc_entry("watchdog_reset", 0, NULL);
	watchdog_proc->read_proc = watchdog_reset_read_proc;
	watchdog_proc->write_proc = watchdog_reset_write_proc;
#ifdef TCSUPPORT_MIPS_1004K
    watchdog_proc = create_proc_entry("gic_edge_nmi_send", 0, NULL);
    watchdog_proc->write_proc = gic_edge_nmi_send_write_proc;
    watchdog_proc = create_proc_entry("interrupt_nmi_set", 0, NULL);
    watchdog_proc->write_proc = interrupt_nmi_set_write_proc;
#else 
	if (isMT751020 || isMT7505 || isEN751221) {
		watchdog_proc = create_proc_entry("nmi_enable", 0, NULL);
		watchdog_proc->read_proc = nmi_enable_read_proc;
		watchdog_proc->write_proc = nmi_enable_write_proc;
	}
#endif

#ifdef TCSUPPORT_NEW_WDOG
	watchdog_proc = create_proc_entry("wdog_counters", 0, NULL);
	watchdog_proc->read_proc = wdog_counters_read_proc;
	watchdog_proc = create_proc_entry("tcwdog_enable", 0, NULL);
	watchdog_proc->write_proc = tcwdog_enable_write_proc;
	watchdog_proc = create_proc_entry("wdog_set_timeout", 0, NULL);
	watchdog_proc->write_proc = tcwdog_set_wdog_timeout_write_proc;
	watchdog_proc = create_proc_entry("wdog_kernel_kick_enable", 0, NULL);
	watchdog_proc->write_proc = tcwdog_kernel_kick_enable_write_proc;
	watchdog_proc = create_proc_entry("wdogs_enable", 0, NULL);
	watchdog_proc->write_proc = wdogs_enable_write_proc;
	watchdog_proc = create_proc_entry("wdog_kick_cnt_limit", 0, NULL);
	watchdog_proc->write_proc = wdog_kick_cnt_limit_write_proc;
    watchdog_proc = create_proc_entry("wdog_wifi_rx_cnt_load_value", 0, NULL);
    watchdog_proc->write_proc = wdog_wifi_rx_cnt_load_value_write_proc;
    watchdog_proc = create_proc_entry("wdog_mcast_offload_cnt_load_value", 0, NULL);
    watchdog_proc->write_proc = wdog_mcast_offload_cnt_load_value_write_proc;
#endif
#ifdef L2CACHE_LOCK_CODE
	watchdog_proc = create_proc_entry("l2cache_dump_range", 0, NULL);
	watchdog_proc->write_proc = l2cache_dump_range_proc;
	watchdog_proc = create_proc_entry("l2cache_dump_all", 0, NULL);
	watchdog_proc->read_proc = l2cache_dump_all_proc;
	watchdog_proc = create_proc_entry("l2cache_dump_info", 0, NULL);
	watchdog_proc->read_proc = l2cache_dump_info_proc;
	watchdog_proc = create_proc_entry("l2cache_release_all", 0, NULL);
    watchdog_proc->write_proc = l2cache_release_all_proc;
#endif
	spin_lock_init(&protect_lock);
	
	misc_register(&tc3162wdog_miscdev);
#ifdef TCSUPPORT_NEW_WDOG
    spin_lock_init(&wdog_print_lock);
    misc_register(&wdog0_miscdev);
    misc_register(&wdog1_miscdev);
    misc_register(&wdog2_miscdev);
    misc_register(&wdog3_miscdev);

    if (isFPGA)
        wdog_kick_cnt_limit = 2;
    else if (isEN751627)
        wdog_kick_cnt_limit = 20;
    else
        wdog_kick_cnt_limit = 50;
#endif
    
	if(timer_procdir == NULL) {
		timer_procdir = proc_mkdir("timer", NULL);
		if(timer_procdir != NULL) {
			timer_proc = create_proc_entry("interrupt", 0, timer_procdir);
			if(timer_proc != NULL) {
				timer_proc->read_proc = timer_interrupt_read_proc;
				timer_proc->write_proc = timer_interrupt_write_proc;
			} else {
				printk("\ncreate timer_interrupt proc fail.\n");
			}
		} else {
			printk("\ncreate timer_interrupt proc fail.\n");
		}
	}
	printk("TC3162 hardware watchdog module loaded.\n");
	return 0;
}
static void __exit tc3162_watchdog_exit(void)
{
	misc_deregister(&tc3162wdog_miscdev);
	remove_proc_entry("interrupt",timer_procdir);
	remove_proc_entry("timer",NULL);
    #ifdef TCSUPPORT_NEW_WDOG
    misc_deregister(&wdog0_miscdev);
    misc_deregister(&wdog1_miscdev);
    misc_deregister(&wdog2_miscdev);
    misc_deregister(&wdog3_miscdev);

    remove_proc_entry("wdog_counters", NULL);
    remove_proc_entry("tcwdog_enable", NULL);
    remove_proc_entry("wdog_set_timeout", NULL);
    remove_proc_entry("wdog_kernel_kick_enable", NULL);
    remove_proc_entry("wdogs_enable", NULL);
    remove_proc_entry("wdog_kick_cnt_limit", NULL);
    remove_proc_entry("wdog_wifi_rx_cnt_load_value", NULL);
	remove_proc_entry("wdog_mcast_offload_cnt_load_value", NULL);
    #endif
    #if defined(TCSUPPORT_CPU_EN7580)
    remove_proc_entry("reboot_status", NULL);
    #endif
    #ifdef TCSUPPORT_MIPS_1004K
    remove_proc_entry("gic_edge_nmi_send", NULL);
    remove_proc_entry("interrupt_nmi_set", NULL);
    #endif
    #ifdef L2CACHE_LOCK_CODE
	remove_proc_entry("l2cache_dump_range", NULL);
    remove_proc_entry("l2cache_dump_all", NULL);
    remove_proc_entry("l2cache_dump_info", NULL);
    remove_proc_entry("l2cache_release_all", NULL);
    #endif
}

module_init(tc3162_watchdog_init);
module_exit(tc3162_watchdog_exit);

