#ifndef __ECNT_TRAPS_H
#define __ECNT_TRAPS_H

extern int watchFlag;
extern void nmi_info_store( struct pt_regs *regs);
extern void __noreturn die_nmi(const char *str, struct pt_regs *regs, spinlock_t *lock);

#endif
