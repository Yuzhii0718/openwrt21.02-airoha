#ifndef __ASM_ARM_IRQ_H
#define __ASM_ARM_IRQ_H

#include <linux/version.h>
#define NR_IRQS_LEGACY	64

#ifndef CONFIG_SPARSE_IRQ
#include <mach/irqs.h>
#else
#define NR_IRQS NR_IRQS_LEGACY
#endif

#ifndef irq_canonicalize
#define irq_canonicalize(i)	(i)
#endif

/*
 * Use this value to indicate lack of interrupt
 * capability
 */
#ifndef NO_IRQ
#define NO_IRQ	((unsigned int)(-1))
#endif

#ifndef __ASSEMBLY__
struct irqaction;
struct pt_regs;
extern void migrate_irqs(void);

extern void asm_do_IRQ(unsigned int, struct pt_regs *);
void handle_IRQ(unsigned int, struct pt_regs *);
void init_IRQ(void);

#ifdef CONFIG_MULTI_IRQ_HANDLER
extern void (*handle_arch_irq)(struct pt_regs *);
extern void set_handle_irq(void (*handle_irq)(struct pt_regs *));
#endif

#ifdef CONFIG_SMP
extern void arch_trigger_all_cpu_backtrace(bool);
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#define arch_trigger_all_cpu_backtrace(x) arch_trigger_all_cpu_backtrace(x)
#else
#if !defined(TCSUPPORT_CPU_ARMV8_64)
#define arch_trigger_cpumask_backtrace arch_trigger_cpumask_backtrace
#endif
#endif
#endif

static inline int nr_legacy_irqs(void)
{
	return NR_IRQS_LEGACY;
}

#endif

#endif

