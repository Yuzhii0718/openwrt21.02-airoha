#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/sched/debug.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/ftrace.h>

#if defined(TCSUPPORT_NEW_SPIFLASH)
#define SF_MUX_LOCK_MAX		(10)

extern noinline void __up(struct semaphore *sem);
extern noinline int __down_interruptible(struct semaphore *sem);

int down_Manual_interruptible(struct semaphore *sem)
{
	unsigned long flags = 0;
	int result = 0;

    spin_lock_irqsave(&sem->lock, flags);
	if (likely(sem->count == 0)) {
		sem->count += (2 * SF_MUX_LOCK_MAX);
	}
	else{
		result = __down_interruptible(sem);
		if(result == 0)
			sem->count += (2 * SF_MUX_LOCK_MAX);
	}
	spin_unlock_irqrestore(&sem->lock, flags);

	return result;
}
EXPORT_SYMBOL(down_Manual_interruptible);

int down_Auto_interruptible(struct semaphore *sem)
{
	unsigned long flags = 0;
	int result = 0;

	spin_lock_irqsave(&sem->lock, flags);
	if (likely(sem->count < (2 * SF_MUX_LOCK_MAX))) {
		sem->count += 1;
	}
	else{
		result = __down_interruptible(sem);
		if(result == 0)
			sem->count += 1;	
	}
	spin_unlock_irqrestore(&sem->lock, flags);

	return result;
}
EXPORT_SYMBOL(down_Auto_interruptible);
#if 0
int down_Unzip_interruptible(struct semaphore *sem)
{
	unsigned long flags = 0;
	int result = 0;

	spin_lock_irqsave(&sem->lock, flags);
	if (likely(sem->count <= SF_MUX_LOCK_MAX)) {
		sem->count += SF_MUX_LOCK_MAX;
	}
	else{
		result = __down_interruptible(sem);
		if(result == 0)
			sem->count += (SF_MUX_LOCK_MAX);	
	}
	spin_unlock_irqrestore(&sem->lock, flags);

	return result;
}
EXPORT_SYMBOL(down_Unzip_interruptible);
#endif
void up_Manual(struct semaphore *sem)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&sem->lock, flags);
	sem->count -= (2 * SF_MUX_LOCK_MAX);
	if (!(list_empty(&sem->wait_list)))
		__up(sem);
	spin_unlock_irqrestore(&sem->lock, flags);
}
EXPORT_SYMBOL(up_Manual);

void up_Auto(struct semaphore *sem)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&sem->lock, flags);
	sem->count -= 1;
	if (!(list_empty(&sem->wait_list)))
		__up(sem);
	spin_unlock_irqrestore(&sem->lock, flags);
}
EXPORT_SYMBOL(up_Auto);
#if 0
void up_Unzip(struct semaphore *sem)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&sem->lock, flags);
	sem->count -= (SF_MUX_LOCK_MAX);
	if (!(list_empty(&sem->wait_list)))
		__up(sem);
	spin_unlock_irqrestore(&sem->lock, flags);
}
EXPORT_SYMBOL(up_Unzip);
#endif
int down_Normal_interruptible(struct semaphore *sem)
{
	return 0;
}
EXPORT_SYMBOL(down_Normal_interruptible);

void up_Normal(struct semaphore *sem)
{
}
EXPORT_SYMBOL(up_Normal);
#endif

