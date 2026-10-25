#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif
#if defined(TCSUPPORT_CT_VOIP_QOS)

#define RTP_BIND_WAN_INDEX		"tc3162/RtpBindWanIdx"
#define RTP_PRIORITY		"tc3162/RtpPriority"

int gRtpPriority = 0;
int gRtpBindWanidx = -1;

int getRtpBindWANidx(void);
int setRtpBindWANidx(int value);
int getRtpPriority(void);
int setRtpPriority(int value);
static int vdspProcReadRtpBindWANidx(char *page, char **start, off_t off, int count, int *eof, void *data);
static int vdspProcWriteRtpBindWANidx(struct file *file, const char *buffer, unsigned long count, void *data);
static int vdspProcReadRtpPriority(char *page, char **start, off_t off, int count, int *eof, void *data);
static int vdspProcWriteRtpPriority(struct file *file, const char *buffer, unsigned long count, void *data);

int getRtpBindWANidx(void){
	return gRtpBindWanidx;
}

int setRtpBindWANidx(int value){
	gRtpBindWanidx = value;	
	printk("gRtpBindWanidx= %d\n",gRtpBindWanidx);
	return 0;
}

int getRtpPriority(void){
	return gRtpPriority;
}

int setRtpPriority(int value){
	gRtpPriority = value;	
	return 0;
}

EXPORT_SYMBOL(getRtpBindWANidx);
EXPORT_SYMBOL(getRtpPriority);

static int vdspProcReadRtpBindWANidx(char *page, char **start, off_t off, 
	int count, int *eof, void *data)
{
	int len=0;
	len += sprintf(page+len, "%d\r\n", getRtpBindWANidx());
	
	return len;
}

static int vdspProcWriteRtpBindWANidx(struct file *file, const char *buffer, 
	unsigned long count, void *data)
{	
	char wanIdx[32];
	int index = -1;

	if (count > sizeof(wanIdx) - 1)
		return -EINVAL;

	if (copy_from_user(wanIdx, buffer, count))
		return -EFAULT;

	wanIdx[count] =  '\0';	
	sscanf(wanIdx, "%d",&index);
	setRtpBindWANidx(index);
	return count;
}

static int vdspProcReadRtpPriority(char *page, char **start, off_t off, 
	int count, int *eof, void *data)
{
	int len=0;
	len += sprintf(page+len, "%d\r\n", getRtpPriority());
	
	return len;
}


static int vdspProcWriteRtpPriority(struct file *file, const char *buffer, 
	unsigned long count, void *data)
{	
	char priority[32];
	int iPriority = 0;

	if (count > sizeof(priority) - 1)
		return -EINVAL;

	if (copy_from_user(priority, buffer, count))
		return -EFAULT;

	priority[count] = '\0';
	sscanf(priority, "%d",&iPriority);
	setRtpPriority(iPriority);
	return count;
}


void ecnt_voip_qos_proc_init(void)
{

	struct proc_dir_entry *voip_proc = NULL;

	voip_proc = create_proc_entry(RTP_BIND_WAN_INDEX, 0, NULL);
	voip_proc->read_proc = vdspProcReadRtpBindWANidx;
	voip_proc->write_proc = vdspProcWriteRtpBindWANidx;

	voip_proc = create_proc_entry(RTP_PRIORITY, 0, NULL);
	voip_proc->read_proc = vdspProcReadRtpPriority;
	voip_proc->write_proc = vdspProcWriteRtpPriority;

	return;
}

void ecnt_voip_qos_proc_dest(void)
{

	remove_proc_entry(RTP_BIND_WAN_INDEX, NULL);	
	remove_proc_entry(RTP_PRIORITY, NULL);	

}
#endif
