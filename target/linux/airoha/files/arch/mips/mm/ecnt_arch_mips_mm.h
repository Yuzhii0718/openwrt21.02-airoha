#ifndef _LINUX_ECNT_ARCH_MIPS_MM_H
#define _LINUX_ECNT_ARCH_MIPS_MM_H

#if defined(TCSUPPORT_CT_JOYME2)
extern int set_panic_log_flag(int panic_flag);
#endif

static inline int ecnt_set_panic_log_flag_hook(int panic_flag)
{
	int ret = -1;
#if defined(TCSUPPORT_CT_JOYME2)
	ret = set_panic_log_flag(panic_flag);
#endif
	return ret;
}


#endif

