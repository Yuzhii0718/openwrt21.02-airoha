#ifndef __ECNT_LIB_PRINTF_H__
#define __ECNT_LIB_PRINTF_H__

int ecnt_printf(int module_num, int level, const char *fmt,...);
int	ecnt_raw_printf(int module_num, int level, char *data, int len);


#endif
