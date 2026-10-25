/*
 * Copyright (C) 2007, 2011 MIPS Technologies, Inc.
 *	All rights reserved.

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
 * Arbitrary Monitor interface
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/smp.h>

#include <asm/addrspace.h>
#ifdef TCSUPPORT_MIPS_1004K
#include <cpu/launch.h>
#else
#include <asm/mips-boards/launch.h>
#endif
#include <asm/mipsmtregs.h>

int amon_cpu_avail(int cpu)
{
#ifdef TCSUPPORT_MIPS_1004K
    struct cpulaunch *launch = (struct cpulaunch *)CPU_LAUNCH_BASE;
#else
	struct cpulaunch *launch = (struct cpulaunch *)CKSEG0ADDR(CPULAUNCH);
#endif
	if (cpu < 0 || cpu >= NCPULAUNCH) {
		printk("avail: cpu%d is out of range\n", cpu);
		return 0;
	}

	launch += cpu;
	if (!(launch->flags & LAUNCH_FREADY)) {
		printk("avail: cpu%d is not ready\n", cpu);
		return 0;
	}
	if (launch->flags & (LAUNCH_FGO|LAUNCH_FGONE)) {
		printk("avail: too late.. cpu%d is already gone\n", cpu);
		return 0;
	}

	return 1;
}

void amon_cpu_start(int cpu,
		    unsigned long pc, unsigned long sp,
		    unsigned long gp, unsigned long a0)
{
	volatile struct cpulaunch *launch =
   #ifdef TCSUPPORT_MIPS_1004K
        (struct cpulaunch  *)CPU_LAUNCH_BASE;
   #else
		(struct cpulaunch  *)CKSEG0ADDR(CPULAUNCH);
   #endif

	if (!amon_cpu_avail(cpu)) {
		return;
	}
	if (cpu == smp_processor_id()) {
		printk("launch: I am cpu%d!\n", cpu);
		return;
	}
	launch += cpu;

	printk("launch: starting cpu%d\n", cpu);

	launch->pc = pc;
	launch->gp = gp;
	launch->sp = sp;
	launch->a0 = a0;

	smp_wmb();		/* Target must see parameters before go */
	launch->flags |= LAUNCH_FGO;
	smp_wmb();		/* Target must see go before we poll  */
	while ((launch->flags & LAUNCH_FGONE) == 0)
		;
	smp_rmb();		/* Target will be updating flags soon */
	printk("launch: cpu%d gone!\n", cpu);
}

