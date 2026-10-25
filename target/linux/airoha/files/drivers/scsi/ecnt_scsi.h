#ifndef _ECNT_SCSI_H_
#define _ECNT_SCSI_H_

#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#else
#include <asm/uaccess.h>
#endif


 
extern int send_signal_to_app(int signal);
extern int init_auto_mount(void);




#endif