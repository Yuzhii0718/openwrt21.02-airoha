#ifndef _LINUX_ECNT_KERNEL_PRINTK_H
#define _LINUX_ECNT_KERNEL_PRINTK_H

#if defined(TCSUPPORT_CT_JOYME2)
extern void set_panic_log_buffer(va_list *args, const char *fmt);
#endif

static inline int ecnt_set_panic_log_buffer_hook(va_list *args, const char* fmt)
{
#if defined(TCSUPPORT_CT_JOYME2)
	set_panic_log_buffer(args, fmt);
#endif
	return 0;
}
#endif

