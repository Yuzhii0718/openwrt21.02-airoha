/***************************************************************************************
 *      Copyright(c) 2014 ECONET Incorporation All rights reserved.
 *
 *      This is unpublished proprietary source code of ECONET Networks Incorporation
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 ***************************************************************************************
 */

/*
 *  global_inc/lib/ecnt_utility.h
 */


#ifndef _ECNT_UTILITY_H_
#define _ECNT_UTILITY_H_

/**
 * memcpy4 - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * You should not use this function to access IO space, use memcpy_toio()
 * or memcpy_fromio() instead.
 */
extern void *memcpy4(void *dest, const void *src, size_t count);
extern ssize_t ecnt_kernel_fs_read(struct file *osfd, char __user *pDataPtr, size_t readLen, loff_t *fpos);
extern ssize_t ecnt_kernel_fs_write(struct file *osfd, const char __user *pDataPtr, size_t writeLen, loff_t *fpos);
extern int ecnt_kernel_fs_read_check(struct file *osfd);
extern int ecnt_kernel_fs_write_check(struct file *osfd);

#if  defined(CONFIG_MIPS) &&  LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
static inline void arch_spin_lock_ecnt(arch_spinlock_t *lock)
{
	int my_ticket;
	int tmp;
	int inc = 0x10000;

	if (R10000_LLSC_WAR) {
		__asm__ __volatile__ (
		"	.set push		# arch_spin_lock	\n"
		"	.set noreorder					\n"
		"							\n"
		"1:	ll	%[ticket], %[ticket_ptr]		\n"
		"	addu	%[my_ticket], %[ticket], %[inc]		\n"
		"	sc	%[my_ticket], %[ticket_ptr]		\n"
		"	beqzl	%[my_ticket], 1b			\n"
		"	 nop						\n"
		"	srl	%[my_ticket], %[ticket], 16		\n"
		"	andi	%[ticket], %[ticket], 0xffff		\n"
		"	bne	%[ticket], %[my_ticket], 4f		\n"
		"	 subu	%[ticket], %[my_ticket], %[ticket]	\n"
		"2:							\n"
		"	.subsection 2					\n"
		"4:	andi	%[ticket], %[ticket], 0xffff		\n"
		"	sll	%[ticket], 5				\n"
		"							\n"
		"6:	bnez	%[ticket], 6b				\n"
		"	 subu	%[ticket], 1				\n"
		"							\n"
		"	lhu	%[ticket], %[serving_now_ptr]		\n"
		"	beq	%[ticket], %[my_ticket], 2b		\n"
		"	 subu	%[ticket], %[my_ticket], %[ticket]	\n"
		"	b	4b					\n"
		"	 subu	%[ticket], %[ticket], 1			\n"
		"	.previous					\n"
		"	.set pop					\n"
		: [ticket_ptr] "+m" (lock->lock),
		  [serving_now_ptr] "+m" (lock->h.serving_now),
		  [ticket] "=&r" (tmp),
		  [my_ticket] "=&r" (my_ticket)
		: [inc] "r" (inc));
	} else {
		__asm__ __volatile__ (
		"	.set push		# arch_spin_lock	\n"
		"	.set noreorder					\n"
		"							\n"
		"1:	ll	%[ticket], %[ticket_ptr]		\n"
		"	addu	%[my_ticket], %[ticket], %[inc]		\n"
		"	sc	%[my_ticket], %[ticket_ptr]		\n"
		"	beqz	%[my_ticket], 1b			\n"
		"	 srl	%[my_ticket], %[ticket], 16		\n"
		"	andi	%[ticket], %[ticket], 0xffff		\n"
		"	bne	%[ticket], %[my_ticket], 4f		\n"
		"	 subu	%[ticket], %[my_ticket], %[ticket]	\n"
		"2:							\n"
		"	.subsection 2					\n"
		"4:	andi	%[ticket], %[ticket], 0x1fff		\n"
		"	sll	%[ticket], 5				\n"
		"							\n"
		"6:	bnez	%[ticket], 6b				\n"
		"	 subu	%[ticket], 1				\n"
		"							\n"
		"	lhu	%[ticket], %[serving_now_ptr]		\n"
		"	beq	%[ticket], %[my_ticket], 2b		\n"
		"	 subu	%[ticket], %[my_ticket], %[ticket]	\n"
		"	b	4b					\n"
		"	 subu	%[ticket], %[ticket], 1			\n"
		"	.previous					\n"
		"	.set pop					\n"
		: [ticket_ptr] "+m" (lock->lock),
		  [serving_now_ptr] "+m" (lock->h.serving_now),
		  [ticket] "=&r" (tmp),
		  [my_ticket] "=&r" (my_ticket)
		: [inc] "r" (inc));
	}

	__asm__ __volatile__("sync 0x10" : : :"memory");

}

static inline void spin_lock_bh_ecnt(spinlock_t *lock)
{
	raw_spinlock_t *raw_lock = &lock->rlock;
	
	__local_bh_disable_ip(_RET_IP_, SOFTIRQ_LOCK_OFFSET);
	spin_acquire(&raw_lock->dep_map, 0, 0, _RET_IP_);

	__acquire(raw_lock);
	arch_spin_lock_ecnt(&raw_lock->raw_lock);
}

static inline void spin_unlock_bh_ecnt(spinlock_t *lock)
{
	raw_spinlock_t *raw_lock;
	arch_spinlock_t *arch_lock;
	unsigned int serving_now;
	
	raw_lock = &lock->rlock;
	arch_lock = &raw_lock->raw_lock;

	spin_release(&raw_lock->dep_map, 1, _RET_IP_);
		
	serving_now = arch_lock->h.serving_now + 1;
	__asm__ __volatile__("sync 0x10" : : :"memory");
	arch_lock->h.serving_now = (u16)serving_now;
	__asm__ __volatile__("sync 0x10" : : :"memory");
	__release(raw_lock);
	
	__local_bh_enable_ip(_RET_IP_, SOFTIRQ_LOCK_OFFSET);
}
#else
static inline void spin_lock_bh_ecnt(spinlock_t *lock)
{
	spin_lock_bh(lock);
}

static inline void spin_unlock_bh_ecnt(spinlock_t *lock)
{
	spin_unlock_bh(lock);
}
#endif
#endif // #ifndef _ECNT_UTILITY_H_

