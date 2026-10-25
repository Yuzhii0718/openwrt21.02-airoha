#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <asm/delay.h>
#include "H/air_eth_xsgmii.h"
#include "H/air_xsgmii_api.h"

#define BUFSIZE 255

static struct proc_dir_entry *xsgmii_proc_ent;
static ssize_t xsgmii_cmd_w(struct file *file,const char __user *ubuf, size_t count, loff_t *ppos){
	int num,c,xsgmii,an,rate,op,mod;
	char buf[BUFSIZE];
	if(*ppos > 0|| count > BUFSIZE) {printk("NG1\n"); return -EFAULT;}
	if(copy_from_user(buf, ubuf, count)) {printk("NG2\n"); return -EFAULT;}
	num = sscanf(buf,"%d %d %d %d %d",&op,&xsgmii,&mod,&an,&rate);
	if(num != 5) {printk("NG3\n"); return -EFAULT;}
	c = strlen(buf);
	*ppos = c;	
	printk("op = %x,xsgmii %x,mod %x,an %x,rate %x,buf %x,num %x,count %lx\n",op,xsgmii,mod,an,rate,*buf,num,count);
	xsgmii_api((u8) op,(u8) xsgmii,(u8) mod,(u8) rate,(u8) an);
	
	return count;
}

static struct file_operations xsgmii_ops={
	.owner = THIS_MODULE,
	.write = xsgmii_cmd_w,
};

static void xsgmii_proc_init(void){
	xsgmii_proc_ent = proc_create("xsgmii",0660,NULL,&xsgmii_ops);
	printk("xsgmii_proc_init\n");
	//return 0;
}

static void xsgmii_proc_remove(void){
	proc_remove(xsgmii_proc_ent);
	printk("xsgmii_proc_remove\n");
}

module_init(xsgmii_proc_init);
module_exit(xsgmii_proc_remove);

