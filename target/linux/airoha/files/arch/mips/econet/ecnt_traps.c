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
#include <linux/version.h>
#include <linux/bug.h>
#include <linux/compiler.h>
#include <linux/context_tracking.h>
#include <linux/cpu_pm.h>
#include <linux/kexec.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <linux/bootmem.h>
#endif
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/kgdb.h>
#include <linux/kdebug.h>
#include <linux/kprobes.h>
#include <linux/notifier.h>
#include <linux/kdb.h>
#include <linux/irq.h>
#include <linux/perf_event.h>
	
#include <asm/bootinfo.h>
#include <asm/branch.h>
#include <asm/break.h>
#include <asm/cop2.h>
#include <asm/cpu.h>
#include <asm/cpu-type.h>
#include <asm/dsp.h>
#include <asm/fpu.h>
#include <asm/fpu_emulator.h>
#include <asm/idle.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/module.h>
#include <asm/msa.h>
#include <asm/pgtable.h>
#include <asm/ptrace.h>
#include <asm/sections.h>
#include <asm/tlbdebug.h>
#include <asm/traps.h>
#include <asm/uaccess.h>
#include <asm/watch.h>
#include <asm/mmu_context.h>
#include <asm/types.h>
#include <asm/stacktrace.h>
#include <asm/uasm.h>
#include <asm/irq.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define NMI_STACK_LEN	80
#define NMI_STACK_MAGIC_NUM 	0x5abc2312

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
extern int regs_to_trapnr(struct pt_regs *regs);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
extern void mips_mt_regdump_nmi(unsigned long mvpctl);
#endif
#ifdef TCSUPPORT_NEW_WDOG
extern void watchDogReset(void);
#endif

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
int watchFlag=0;
EXPORT_SYMBOL(watchFlag);


/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
int regs_to_trapnr(struct pt_regs *regs)
{
	return (regs->cp0_cause >> 2) & 0x1f;
}
#endif

void show_raw_backtrace_nmi(unsigned long sp_start, unsigned long stack_len)
{
	unsigned long *sp = (unsigned long *)(sp_start & ~3);
	unsigned long addr, i = 0;

	printk("Call Trace NMI:");
#ifdef CONFIG_KALLSYMS
	printk("\n");
#endif
	while (i < stack_len) {
		unsigned long __user *p =
			(unsigned long __user *)(unsigned long)sp++;
		if (__get_user(addr, p)) {
			printk(" (Bad stack address)");
			break;
		}
		if (__kernel_text_address(addr))
			print_ip_sym(addr);

		i++;
	}
	printk("\n");
}

void show_stack_nmi(void)
{
#ifdef CONFIG_TC3162_DMEM
	unsigned int dspram_addr = dspram_base_addr();
	int i, dspram_data_len=NMI_STACK_LEN;
	unsigned int *p = (unsigned int *)dspram_addr;

	printk("dspram_addr=0x%x\n", dspram_addr);

	if(*p != NMI_STACK_MAGIC_NUM){
		printk("No NMI Happen!\n");
		return;
	}
	
	p++;
	printk("epc   : %08lx %pS\n", *p,
		   (void *) (*p));
	p++;
	printk("ra	  : %08lx %pS\n",*p,
		   (void *) (*p));

	p++;
	printk("Status: %08x	", (uint32_t) (*p));
	p++;
	printk("Cause : %08x\n", (*p));

	p++;

	while(dspram_data_len){
		if(dspram_data_len % 8 == 0)
			printk("\n		 ");
		printk(" %08lx", *p);

		p++;
		dspram_data_len--;
	}
	printk("\n		 ");

	show_raw_backtrace_nmi(dspram_base_addr(),NMI_STACK_LEN);
#endif
	
}

static void show_stacktrace_nmi(struct task_struct *task,
	const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	long stackdata;
	int i;
	unsigned long __user *sp = (unsigned long __user *)regs->regs[29];

	printk("Stack :");
	i = 0;
	while ((unsigned long) sp & (PAGE_SIZE - 1)) {
		if (i && ((i % (64 / field)) == 0))
			printk("\n		 ");
		if (i > NMI_STACK_LEN-1) {
			printk(" ...");
			break;
		}

		if (__get_user(stackdata, sp++)) {
			printk(" (Bad stack address)");
			break;
		}

		printk(" %0*lx", field, stackdata);
		i++;
	}
	printk("\n");
	//show_backtrace(task, regs);
}

static void __show_regs_nmi(const struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);
	unsigned int cause = regs->cp0_cause;
	int i;

	printk("Cpu %d\n", smp_processor_id());

	/*
	 * Saved main processor registers
	 */
	for (i = 0; i < 32; ) {
		if ((i % 4) == 0)
			printk("$%2d   :", i);
		if (i == 0)
			printk(" %0*lx", field, 0UL);
		else if (i == 26 || i == 27)
			printk(" %*s", field, "");
		else
			printk(" %0*lx", field, regs->regs[i]);

		i++;
		if ((i % 4) == 0)
			printk("\n");
	}

#ifdef CONFIG_CPU_HAS_SMARTMIPS
	printk("Acx    : %0*lx\n", field, regs->acx);
#endif
	printk("Hi	  : %0*lx\n", field, regs->hi);
	printk("Lo	  : %0*lx\n", field, regs->lo);

	/*
	 * Saved cp0 registers
	 */
	//printk("epc	: %0*lx %pS\n", field, regs->cp0_epc,
	//		 (void *) regs->cp0_epc);
	printk("epc   : %0*lx\n", field, regs->cp0_epc);
	printk("	%s\n", print_tainted());
	//printk("ra	: %0*lx %pS\n", field, regs->regs[31],
	//		 (void *) regs->regs[31]);
	printk("ra	  : %0*lx\n", field, regs->regs[31]);

	printk("Status: %08x	", (uint32_t) regs->cp0_status);

	if (current_cpu_data.isa_level == MIPS_CPU_ISA_II) {
		if (regs->cp0_status & ST0_KUO)
			printk("KUo ");
		if (regs->cp0_status & ST0_IEO)
			printk("IEo ");
		if (regs->cp0_status & ST0_KUP)
			printk("KUp ");
		if (regs->cp0_status & ST0_IEP)
			printk("IEp ");
		if (regs->cp0_status & ST0_KUC)
			printk("KUc ");
		if (regs->cp0_status & ST0_IEC)
			printk("IEc ");
	} else {
		if (regs->cp0_status & ST0_KX)
			printk("KX ");
		if (regs->cp0_status & ST0_SX)
			printk("SX ");
		if (regs->cp0_status & ST0_UX)
			printk("UX ");
		switch (regs->cp0_status & ST0_KSU) {
		case KSU_USER:
			printk("USER ");
			break;
		case KSU_SUPERVISOR:
			printk("SUPERVISOR ");
			break;
		case KSU_KERNEL:
			printk("KERNEL ");
			break;
		default:
			printk("BAD_MODE ");
			break;
		}
		if (regs->cp0_status & ST0_ERL)
			printk("ERL ");
		if (regs->cp0_status & ST0_EXL)
			printk("EXL ");
		if (regs->cp0_status & ST0_IE)
			printk("IE ");
	}
	printk("\n");

	printk("Cause : %08x\n", cause);

	cause = (cause & CAUSEF_EXCCODE) >> CAUSEB_EXCCODE;
	if (1 <= cause && cause <= 5)
		printk("BadVA : %0*lx\n", field, regs->cp0_badvaddr);

	printk("PrId  : %08x (%s)\n", read_c0_prid(),
		   cpu_name_string());
}

void show_registers_nmi(struct pt_regs *regs)
{
	const int field = 2 * sizeof(unsigned long);

	__show_regs_nmi(regs);
	//print_modules();
	printk("Process %s (pid: %d, threadinfo=%p, task=%p, tls=%0*lx)\n",
		   current->comm, current->pid, current_thread_info(), current,
		  field, current_thread_info()->tp_value);
	if (cpu_has_userlocal) {
		unsigned long tls;

		tls = read_c0_userlocal();
		if (tls != current_thread_info()->tp_value)
			printk("*HwTLS: %0*lx\n", field, tls);
	}

	show_stacktrace_nmi(current, regs);
	//show_code((unsigned int __user *) regs->cp0_epc);
	printk("\n");
}

void nmi_info_store( struct pt_regs *regs)
{
#ifdef CONFIG_TC3162_DMEM
	const int field = 2 * sizeof(unsigned long);
	unsigned int cause = regs->cp0_cause;
	long stackdata;
	int i;
	unsigned long __user *sp = (unsigned long __user *)regs->regs[29];
	
	/*Store Magic Number*/
	write_to_dspram(NMI_STACK_MAGIC_NUM);

	/*Store register value*/
	write_to_dspram(regs->cp0_epc);
	write_to_dspram(regs->regs[31]); //ra
	write_to_dspram((uint32_t) regs->cp0_status);
	write_to_dspram(cause);
	
	/*Store stack data*/
	i = 0;
	while ((unsigned long) sp & (PAGE_SIZE - 1)) {
		if (i > NMI_STACK_LEN-1) {
			break;
		}

		if (__get_user(stackdata, sp++)) {
			//printk(" (Bad stack address)");
			break;
		}

		//printk(" %0*lx", field, stackdata);
		write_to_dspram(stackdata);
		i++;
	}
#endif
}
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
/*
 * Dump new MIPS MT state for the core. Does not leave TCs halted.
 * Takes an argument which taken to be a pre-call MVPControl value.
 */

void mips_mt_regdump_nmi(unsigned long mvpctl)
{
	unsigned long flags;
	unsigned long vpflags;
	unsigned long mvpconf0;
	int nvpe;
	int ntc;
	int i;
	int tc;
	unsigned long haltval;
	unsigned long tcstatval;
#ifdef CONFIG_MIPS_MT_SMTC
	void smtc_soft_dump(void);
#endif /* CONFIG_MIPT_MT_SMTC */

	local_irq_save(flags);
	vpflags = dvpe();
	printk("=== MIPS MT State Dump ===\n");
	printk("-- Global State --\n");
	printk("   MVPControl Passed: %08lx\n", mvpctl);
	printk("   MVPControl Read: %08lx\n", vpflags);
	printk("   MVPConf0 : %08lx\n", (mvpconf0 = read_c0_mvpconf0()));
	nvpe = ((mvpconf0 & MVPCONF0_PVPE) >> MVPCONF0_PVPE_SHIFT) + 1;
	ntc = ((mvpconf0 & MVPCONF0_PTC) >> MVPCONF0_PTC_SHIFT) + 1;
	printk("-- per-VPE State --\n");
	for (i = 0; i < nvpe; i++) {
		for (tc = 0; tc < ntc; tc++) {
			settc(tc);
			if ((read_tc_c0_tcbind() & TCBIND_CURVPE) == i) {
				printk("  VPE %d\n", i);
				printk("   VPEControl : %08lx\n",
				       read_vpe_c0_vpecontrol());
				printk("   VPEConf0 : %08lx\n",
				       read_vpe_c0_vpeconf0());
				printk("   VPE%d.Status : %08lx\n",
				       i, read_vpe_c0_status());
				//printk("   VPE%d.EPC : %08lx %pS\n",
				//       i, read_vpe_c0_epc(),
				//       (void *) read_vpe_c0_epc());
				printk("   VPE%d.EPC : %08lx\n",
				       i, read_vpe_c0_epc());
				printk("   VPE%d.Cause : %08lx\n",
				       i, read_vpe_c0_cause());
				printk("   VPE%d.Config7 : %08lx\n",
				       i, read_vpe_c0_config7());
				break; /* Next VPE */
			}
		}
	}
	printk("-- per-TC State --\n");
	for (tc = 0; tc < ntc; tc++) {
		settc(tc);
		if (read_tc_c0_tcbind() == read_c0_tcbind()) {
			/* Are we dumping ourself?  */
			haltval = 0; /* Then we're not halted, and mustn't be */
			tcstatval = flags; /* And pre-dump TCStatus is flags */
			printk("  TC %d (current TC with VPE EPC above)\n", tc);
		} else {
			haltval = read_tc_c0_tchalt();
			write_tc_c0_tchalt(1);
			tcstatval = read_tc_c0_tcstatus();
			printk("  TC %d\n", tc);
		}
		printk("   TCStatus : %08lx\n", tcstatval);
		printk("   TCBind : %08lx\n", read_tc_c0_tcbind());
		//printk("   TCRestart : %08lx %pS\n",
		//       read_tc_c0_tcrestart(), (void *) read_tc_c0_tcrestart());
		printk("   TCRestart : %08lx\n",
		       read_tc_c0_tcrestart());
		printk("   TCHalt : %08lx\n", haltval);
		printk("   TCContext : %08lx\n", read_tc_c0_tccontext());
		if (!haltval)
			write_tc_c0_tchalt(0);
	}
#ifdef CONFIG_MIPS_MT_SMTC
	smtc_soft_dump();
#endif /* CONFIG_MIPT_MT_SMTC */
	printk("===========================\n");
	evpe(vpflags);
	local_irq_restore(flags);
}
#endif

void __noreturn die_nmi(const char *str, struct pt_regs *regs, spinlock_t *lock)
{
	static int die_counter;
	int sig = SIGSEGV;
	unsigned long dvpret = dvpe();

	notify_die(DIE_OOPS, str, regs, 0, regs_to_trapnr(regs), SIGSEGV);

	console_verbose();
	spin_lock_irq(lock);
	bust_spinlocks(1);
	mips_mt_regdump_nmi(dvpret);
	

	if (notify_die(DIE_OOPS, str, regs, 0, regs_to_trapnr(regs), SIGSEGV) == NOTIFY_STOP)
		sig = 0;

	printk("%s[#%d]:\n", str, ++die_counter);
	show_registers_nmi(regs);
    #ifdef TCSUPPORT_NEW_WDOG
    watchDogReset();
    #endif
	while(1); //waiting for watchdog reboot
#if 0
	add_taint(TAINT_DIE);
	spin_unlock_irq(&die_lock);

	if (in_interrupt())
		panic("Fatal exception in interrupt");

	if (panic_on_oops) {
		printk(KERN_EMERG "Fatal exception: panic in 5 seconds\n");
		ssleep(5);
		panic("Fatal exception");
	}

	do_exit(sig);
#endif
}

