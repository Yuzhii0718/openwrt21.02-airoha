#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/tc3162/tc3162.h>
#include <linux/sched.h>
#include <asm/setup.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

#include <ecnt_hook/ecnt_hook_cpu_interrupt_type.h>


#define CPU_INTERRUPT_MAJOR 224
typedef int (*cpu_interrupt_api_op_t)(struct ecnt_cpu_interrupt_data * data);
int cpu_interrupt_api_get_irqnum(struct ecnt_cpu_interrupt_data *data);
int cpu_interrupt_api_show_interrupts(struct ecnt_cpu_interrupt_data *data);
int cpu_interrupt_api_check_intrName(struct ecnt_cpu_interrupt_data *data);

static long cpu_interrupt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

struct file_operations cpu_interrupt_fops = {
	.owner 			= THIS_MODULE,
	.unlocked_ioctl	= cpu_interrupt_ioctl,
};

static cpu_interrupt_api_op_t cpu_interrupt_operation[] = {
	cpu_interrupt_api_get_irqnum,
    cpu_interrupt_api_show_interrupts,
    cpu_interrupt_api_check_intrName,
};

int get_irqNum_by_name(int *irqNumP, char *irqName) 
{
	unsigned int irq;
	struct irq_desc *desc;
//    printk("This is get_irqNum_by_name, input name=%s\r\n", irqName);
	irq_lock_sparse();	
	for_each_irq_desc(irq, desc){
		if(!desc)
			continue;
		else{
			if(!(desc->action))
				continue;			
//			printk("irq_desc %s %d\r\n", desc->action->name, irq);
			if(strcmp(irqName, desc->action->name)==0){
//				printk("match %s %d\r\n", desc->action->name, irq);				
				*(irqNumP) = irq;
				irq_unlock_sparse();
				return 0;
			}
		}			
	}
	irq_unlock_sparse();
    return -1;
}

int cpu_interrupt_api_get_irqnum(struct ecnt_cpu_interrupt_data *data)
{
    if (get_irqNum_by_name(&(data->irqNum), data->irqString)==0)
        data->retValue = 0;
    else
        data->retValue = -1;

    return data->retValue;
}

int cpu_interrupt_api_show_interrupts(struct ecnt_cpu_interrupt_data *data){
	printk("Function does not implement yet!!\r\n");
    data->retValue = 0;
    return data->retValue;
}

int cpu_interrupt_api_check_intrName(struct ecnt_cpu_interrupt_data *data){
	return cpu_interrupt_api_get_irqnum(data);	
}

void ecnt_register_cpu_interrupt_fops(void)
{
    if (register_chrdev(CPU_INTERRUPT_MAJOR, "/dev/cpu_interrupt", &cpu_interrupt_fops) < 0) {
        printk(KERN_WARNING "cpu_interrupt: can't get major %d\n", CPU_INTERRUPT_MAJOR);
    }
    return;
}

static long cpu_interrupt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) 
{
	int ret = 0 ;
	struct ecnt_cpu_interrupt_data data;
	struct ecnt_cpu_interrupt_data* puser = (struct ecnt_cpu_interrupt_data*)arg;

	if (cmd >= CPU_INTERRUPT_FUNCTION_MAX_NUM) {
        printk("\nError: cmd:%d is too large. Max is %d\n", cmd, CPU_INTERRUPT_FUNCTION_MAX_NUM);
		return -1;
    }
	
	memset(&data,0,sizeof(struct ecnt_cpu_interrupt_data));
	if(copy_from_user(&data, puser ,sizeof(struct ecnt_cpu_interrupt_data)) != 0 ){
		printk("copy_from_user fail!!\r\n");
		return -1;
	}
	ret = cpu_interrupt_operation[data.function_id](&data);
	if( copy_to_user(puser,&data,sizeof(struct ecnt_cpu_interrupt_data)) != 0 ){
		printk("copy_to_user fail!!\r\n");
		return -1;		
	}
	return ret;
}


void ecnt_register_cpu_interrupts(void) {
	printk("ecnt_register_cpu_interrupts");
    ecnt_register_cpu_interrupt_fops();
    return;
}
EXPORT_SYMBOL(ecnt_register_cpu_interrupts);


void ecnt_unregister_cpu_interrupt_fops(void)
{
    unregister_chrdev(CPU_INTERRUPT_MAJOR, "/dev/cpu_interrupt");
    return;
}

void ecnt_unregister_cpu_interrupts(void) {
	printk("ecnt_register_cpu_interrupts");
    ecnt_unregister_cpu_interrupt_fops();
    return;
}
EXPORT_SYMBOL(ecnt_unregister_cpu_interrupts);
