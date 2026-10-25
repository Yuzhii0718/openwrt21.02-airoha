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
 *  kernel_ext/lib/ecnt_utility.c
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <linux/fs.h>

extern void *memcpy(void *dest, const void *src, size_t count);

/**
 * memcpy4 - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * You should not use this function to access IO space, use memcpy_toio()
 * or memcpy_fromio() instead.
 */
void *memcpy4(void *dest, const void *src, size_t count)
{
	unsigned int *d4 = (unsigned int *)dest;
	unsigned int *s4 = (unsigned int *)src;

	if((((unsigned int)s4 & 0x3) != 0) ||
	   (((unsigned int)d4 & 0x3) != 0)) {
		return memcpy(dest, src, count);
	}

	while(count > 0) {
		if(count >> 2) {
			*d4++ = *s4++;
			count -= 4;
		} else {
			memcpy(d4, s4, count);
			count = 0;
		}
	}

	return dest;
}
EXPORT_SYMBOL(memcpy4);

int ecnt_irq_set_affinity(unsigned int irq, unsigned int mask)
{
	cpumask_var_t new_value;

	if (!alloc_cpumask_var(&new_value, GFP_KERNEL))
		return -ENOMEM;

	*(unsigned long *)new_value = mask;
	
	irq_set_affinity(irq, new_value);
	
free_cpumask:
	free_cpumask_var(new_value);
	return 0;
}
EXPORT_SYMBOL(ecnt_irq_set_affinity);

int ecnt_kernel_fs_read_check(struct file *osfd)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
	if (osfd->f_op && osfd->f_op->read)
#else
	if (osfd->f_op)
#endif
		return 1;
	else
		return 0;
}

EXPORT_SYMBOL(ecnt_kernel_fs_read_check);


int ecnt_kernel_fs_write_check(struct file *osfd)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
	if (osfd->f_op && osfd->f_op->write)
#else
	if (osfd->f_op)
#endif
		return 1;
	else
		return 0;
}

EXPORT_SYMBOL(ecnt_kernel_fs_write_check);

ssize_t ecnt_kernel_fs_read(struct file *osfd, char __user *pDataPtr, size_t readLen, loff_t *fpos)
{
	/* The object must have a read method */
#if (KERNEL_VERSION(4,4,90) > LINUX_VERSION_CODE)
	if (osfd->f_op && osfd->f_op->read) 
		return osfd->f_op->read(osfd, pDataPtr, readLen, fpos);
#elif (KERNEL_VERSION(4, 19, 0) <= LINUX_VERSION_CODE)
	if (osfd->f_mode & FMODE_CAN_READ) 
		return kernel_read(osfd, pDataPtr, readLen, fpos);
#else
	if (osfd->f_mode & FMODE_CAN_READ) 
		return __vfs_read(osfd, pDataPtr, readLen, fpos);
#endif
	 else 
		return -1;
}

EXPORT_SYMBOL(ecnt_kernel_fs_read);


ssize_t ecnt_kernel_fs_write(struct file *osfd, const char __user *pDataPtr, size_t writeLen, loff_t *fpos)
{
#if (KERNEL_VERSION(4,4,90) > LINUX_VERSION_CODE)
	if(osfd->f_op->write)
		return osfd->f_op->write(osfd, pDataPtr, (size_t) writeLen, fpos);
	else
		return -1;
#elif (KERNEL_VERSION(4, 19, 0) <= LINUX_VERSION_CODE)
	return kernel_write(osfd, pDataPtr, (size_t) writeLen, fpos);
#else
	return __vfs_write(osfd, pDataPtr, (size_t) writeLen, fpos);
#endif
}
EXPORT_SYMBOL(ecnt_kernel_fs_write);


/* End of [ecnt_utility.c] */
