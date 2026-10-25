#ifndef __ASM_MACH_MIPS_IRQ_H
#define __ASM_MACH_MIPS_IRQ_H


#ifdef CONFIG_MIPS_TC3262
#define NR_IRQS 64
#else
#define NR_IRQS 32
#endif
#ifdef L2CACHE_LOCK_CODE
#undef MAX_L2C_LOCK_SIZE
#ifdef TCSUPPORT_CPU_EN7580
#define MAX_L2C_LOCK_SIZE	262144	/* 256K for IA whose L2 cache is 512K */
#else
#define MAX_L2C_LOCK_SIZE	131072	/* 128K for 1004K whose L2 cache is 256K */
#endif
#endif
#define MIPS_CPU_IRQ_BASE 0

#include_next <irq.h>

#endif /* __ASM_MACH_MIPS_IRQ_H */
