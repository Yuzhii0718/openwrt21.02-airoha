#include <linux/cache.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/mount.h>
#include <linux/init.h>
#include <linux/idr.h>
#include <linux/bitops.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>

#include "internal.h"

extern const struct inode_operations proc_file_inode_operations;
extern struct proc_dir_entry *__proc_create(struct proc_dir_entry **parent,
					  const char *name,
					  umode_t mode,
					  nlink_t nlink);

#define PROC_BLOCK_SIZE	(PAGE_SIZE - 1024)

static ssize_t __proc_file_read(struct file *file, char __user *buf, size_t nbytes,
	       loff_t *ppos)
{
	struct inode * inode = file->f_path.dentry->d_inode;
	char 	*page;
	ssize_t	retval=0;
	int	eof=0;
	ssize_t	n, count;
	char	*start;
	struct proc_dir_entry * dp;
	unsigned long long pos;

	/*
	 * Gaah, please just use "seq_file" instead. The legacy /proc
	 * interfaces cut loff_t down to off_t for reads, and ignore
	 * the offset entirely for writes..
	 */
	pos = *ppos;
	if (pos > MAX_NON_LFS)
		return 0;
	if (nbytes > MAX_NON_LFS - pos)
		nbytes = MAX_NON_LFS - pos;

	dp = PDE(inode);
	if (!(page = (char*) __get_free_page(GFP_KERNEL)))
		return -ENOMEM;

	while ((nbytes > 0) && !eof) {
		count = min_t(size_t, PROC_BLOCK_SIZE, nbytes);

		start = NULL;
		if (dp->read_proc) {
			/*
			 * How to be a proc read function
			 * ------------------------------
			 * Prototype:
			 *    int f(char *buffer, char **start, off_t offset,
			 *          int count, int *peof, void *dat)
			 *
			 * Assume that the buffer is "count" bytes in size.
			 *
			 * If you know you have supplied all the data you
			 * have, set *peof.
			 *
			 * You have three ways to return data:
			 * 0) Leave *start = NULL.  (This is the default.)
			 *    Put the data of the requested offset at that
			 *    offset within the buffer.  Return the number (n)
			 *    of bytes there are from the beginning of the
			 *    buffer up to the last byte of data.  If the
			 *    number of supplied bytes (= n - offset) is 
			 *    greater than zero and you didn't signal eof
			 *    and the reader is prepared to take more data
			 *    you will be called again with the requested
			 *    offset advanced by the number of bytes 
			 *    absorbed.  This interface is useful for files
			 *    no larger than the buffer.
			 * 1) Set *start = an unsigned long value less than
			 *    the buffer address but greater than zero.
			 *    Put the data of the requested offset at the
			 *    beginning of the buffer.  Return the number of
			 *    bytes of data placed there.  If this number is
			 *    greater than zero and you didn't signal eof
			 *    and the reader is prepared to take more data
			 *    you will be called again with the requested
			 *    offset advanced by *start.  This interface is
			 *    useful when you have a large file consisting
			 *    of a series of blocks which you want to count
			 *    and return as wholes.
			 *    (Hack by Paul.Russell@rustcorp.com.au)
			 * 2) Set *start = an address within the buffer.
			 *    Put the data of the requested offset at *start.
			 *    Return the number of bytes of data placed there.
			 *    If this number is greater than zero and you
			 *    didn't signal eof and the reader is prepared to
			 *    take more data you will be called again with the
			 *    requested offset advanced by the number of bytes
			 *    absorbed.
			 */
			n = dp->read_proc(page, &start, *ppos,
					  count, &eof, dp->data);
		} else
			break;

		if (n == 0)   /* end of file */
			break;
		if (n < 0) {  /* error */
			if (retval == 0)
				retval = n;
			break;
		}

		if (start == NULL) {
			if (n > PAGE_SIZE) {
				printk(KERN_ERR
				       "proc_file_read: Apparent buffer overflow!\n");
				n = PAGE_SIZE;
			}
			n -= *ppos;
			if (n <= 0)
				break;
			if (n > count)
				n = count;
			start = page + *ppos;
		} else if (start < page) {
			if (n > PAGE_SIZE) {
				printk(KERN_ERR
				       "proc_file_read: Apparent buffer overflow!\n");
				n = PAGE_SIZE;
			}
			if (n > count) {
				/*
				 * Don't reduce n because doing so might
				 * cut off part of a data block.
				 */
				printk(KERN_WARNING
				       "proc_file_read: Read count exceeded\n");
			}
		} else /* start >= page */ {
			unsigned long startoff = (unsigned long)(start - page);
			if (n > (PAGE_SIZE - startoff)) {
				printk(KERN_ERR
				       "proc_file_read: Apparent buffer overflow!\n");
				n = PAGE_SIZE - startoff;
			}
			if (n > count)
				n = count;
		}
		
 		n -= copy_to_user(buf, start < page ? page : start, n);
		if (n == 0) {
			if (retval == 0)
				retval = -EFAULT;
			break;
		}

		*ppos += start < page ? (unsigned long)start : n;
		nbytes -= n;
		buf += n;
		retval += n;
	}
	free_page((unsigned long) page);
	return retval;
}

static inline int use_pde(struct proc_dir_entry *pde)
{
	return likely(atomic_inc_unless_negative(&pde->in_use));
}
enum {BIAS = -1U<<31};
static void unuse_pde(struct proc_dir_entry *pde)
{
	if (unlikely(atomic_dec_return(&pde->in_use) == BIAS))
		complete(pde->pde_unload_completion);
}


static ssize_t
proc_file_read(struct file *file, char __user *buf, size_t nbytes,
	       loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file_inode(file));
	ssize_t rv = -EIO;

	if (use_pde(pde)) {
		if (pde->read_proc)
			rv = __proc_file_read(file, buf, nbytes, ppos);
		unuse_pde(pde);
	}
	return rv;
}

static ssize_t
proc_file_write(struct file *file, const char __user *buffer,
		size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file_inode(file));
	ssize_t rv = -EIO;

	 if (use_pde(pde)){
		if (pde->write_proc)
			rv = pde->write_proc(file, buffer, count, pde->data);
		unuse_pde(pde);
	}
	return rv;
}


static loff_t
proc_file_lseek(struct file *file, loff_t offset, int orig)
{
	loff_t retval = -EINVAL;
	switch (orig) {
	case 1:
		offset += file->f_pos;
	/* fallthrough */
	case 0:
		if (offset < 0 || offset > MAX_NON_LFS)
			break;
		file->f_pos = retval = offset;
	}
	return retval;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)

static const struct file_operations proc_file_operations = {
	.llseek		= proc_file_lseek,
	.read		= proc_file_read,
	.write		= proc_file_write,
};
#else
static const struct proc_ops proc_file_operations = {
	.proc_read	= proc_file_read,
	.proc_write	= proc_file_write,
	.proc_lseek	= proc_file_lseek,
};
#endif
struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode,
					 struct proc_dir_entry *parent)
{
	struct proc_dir_entry *ent;

	if ((mode & S_IFMT) == 0)
		mode |= S_IFREG;

	if (!S_ISREG(mode)) {
		WARN_ON(1);	/* use proc_mkdir() */
		return NULL;
	}

	if ((mode & S_IALLUGO) == 0)
		mode |= S_IRUGO;

	ent = __proc_create(&parent, name, mode, 1);	
	
	if (ent) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
		if (ent->proc_fops == NULL)
			ent->proc_fops = &proc_file_operations;
#else
		if (ent->proc_ops == NULL)
			ent->proc_ops = &proc_file_operations;
#endif
		if (ent->proc_iops == NULL)
			ent->proc_iops = &proc_file_inode_operations;
		if (proc_register(parent, ent) < 0) {
			printk("\n===>proc_register < 0");
			kfree(ent);
			ent = NULL;
		}
	}
	return ent;
}
EXPORT_SYMBOL(create_proc_entry);

