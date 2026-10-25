#ifndef _ECNT_SEMAPHORE_H_
#define _ECNT_SEMAPHORE_H_

#include <linux/semaphore.h>


#if defined(TCSUPPORT_NEW_SPIFLASH)
extern int down_Normal_interruptible(struct semaphore *sem);
extern void up_Normal(struct semaphore *sem);
#endif

#endif