
#ifndef _ECNT_PROC_FS_H
#define _ECNT_PROC_FS_H

#ifdef CONFIG_PROC_FS

typedef int (*proc_write_t)(struct file *, char *, size_t);
typedef int (*proc_write_t)(struct file *, char *, size_t);
typedef	int (read_proc_t)(char *page, char **start, off_t off, int count, int *eof, void *data);
typedef	int (write_proc_t)(struct file *file, const char __user *buffer,unsigned long count, void *data);

struct proc_dir_entry {
	/*
	 * number of callers into module in progress;
	 * negative -> it's going away RSN
	 */
	atomic_t in_use;
	refcount_t refcnt;
	struct list_head pde_openers;	/* who did ->open, but not ->release */
	/* protects ->pde_openers and all struct pde_opener instances */
	spinlock_t pde_unload_lock;
	struct completion *pde_unload_completion;
	const struct inode_operations *proc_iops;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	const struct file_operations *proc_fops;
#else
	union {
		const struct proc_ops *proc_ops;
		const struct file_operations *proc_dir_ops;
	};
#endif

	const struct dentry_operations *proc_dops;
	union {
		const struct seq_operations *seq_ops;
		int (*single_show)(struct seq_file *, void *);
	};
	proc_write_t write;
	void *data;    
	read_proc_t *read_proc;
	write_proc_t *write_proc;
	int pde_users;	
	unsigned int state_size;
	unsigned int low_ino;
	nlink_t nlink;
	kuid_t uid;
	kgid_t gid;
	loff_t size;
	struct proc_dir_entry *parent;
	struct rb_root subdir;
	struct rb_node subdir_node;
	char *name;
	umode_t mode;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	u8 flags;
#endif
	u8 namelen;
	char inline_name[];
} __randomize_layout;

union proc_op {
	int (*proc_get_link)(struct dentry *, struct path *);
	int (*proc_show)(struct seq_file *m,
		struct pid_namespace *ns, struct pid *pid,
		struct task_struct *task);
	const char *lsm;
};

struct proc_inode {
	struct pid *pid;
	unsigned int fd;
	union proc_op op;
	struct proc_dir_entry *pde;
	struct ctl_table_header *sysctl;
	struct ctl_table *sysctl_entry;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	struct hlist_node sysctl_inodes;
#else
	struct hlist_node sibling_inodes;
#endif
	const struct proc_ns_operations *ns_ops;
	struct inode vfs_inode;
} __randomize_layout;

extern struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode, struct proc_dir_entry *parent);
static inline struct proc_dir_entry *create_proc_read_entry(const char *name,mode_t mode, struct proc_dir_entry *base, read_proc_t *read_proc, void * data)
{
	struct proc_dir_entry *res=create_proc_entry(name,mode,base);
	if (res) {
		res->read_proc=read_proc;
		res->data=data;
	}
	return res;
}
#endif
#endif
