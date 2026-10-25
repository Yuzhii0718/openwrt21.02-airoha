#ifndef __DATAINF_H__
#define __DATAINF_H__

#include "module/wifi.h"
#include "module/network.h"
#include "module/system.h"
#include "module/rae.h"

#if 0
#define LOGFILE "/dev/console"
#define DBG_MSG(fmt, arg...)                                                             \
    do                                                                                   \
    {                                                                                    \
        FILE *log_fp = fopen(LOGFILE, "w");                                              \
        fprintf(log_fp, "%s:%s:%d:"fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
        fclose(log_fp);                                                                  \
    } while (0)
#endif

#endif
