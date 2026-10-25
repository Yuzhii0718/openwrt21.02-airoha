/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>

#include <ecnt_hook/ecnt_hook_event.h>

#include "ecnt_event.h"
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/
MODULE_LICENSE("GPL");

static struct sock *nl_sk = NULL;

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/



/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/
__IMEM struct event_work *work_dequeue(struct event_work_head *list)
{
	unsigned long flags;
	struct event_work *result;

	spin_lock_irqsave(&list->lock, flags);
	result = __work_dequeue(list);
	spin_unlock_irqrestore(&list->lock, flags);
	return result;
}

__IMEM void work_queue_head(struct event_work_head *list, struct event_work *newsk)
{
	unsigned long flags;

	spin_lock_irqsave(&list->lock, flags);
	__work_queue_head(list, newsk);
	spin_unlock_irqrestore(&list->lock, flags);
}

__IMEM int workmgr_free_callback(struct event_work *work)
{
	struct event_work_head *list;

	list = &workmgr_pool[WORKMGR_QUEUE_ID].list;	
	if (work_queue_len(list) > workmgr_max_list_len){
		workmgr_max_list_len = work_queue_len(list) + 1;
	}
	work_queue_head(list, work);
	atomic_dec(&workmgr_alloc_no);
	return 1;
}

__IMEM struct event_work *workmgr_alloc(void)
{
	struct event_work_head *list;
	struct event_work *work;
	int alloc_no;
	struct ecnt_event_data *event_data;
	
	list = &workmgr_pool[WORKMGR_QUEUE_ID].list;

	if (work_queue_len(list)) {
		work = work_dequeue(list);

		if (unlikely(work == NULL))
			goto try_normal;
 
		atomic_inc(&workmgr_alloc_no);
		alloc_no = atomic_read(&workmgr_alloc_no);
		if (alloc_no > workmgr_max_alloc_no)
			workmgr_max_alloc_no = alloc_no;

		return work;
	}
	
try_normal:
	if(printk_ratelimit())
		printk(KERN_ERR "event work memory need alloc More!\n");
	if ((atomic_read(&workmgr_alloc_no) < workmgr_limit_max)) {
		work = kzalloc(sizeof(struct event_work), GFP_ATOMIC|__GFP_NOWARN);
		if (likely(work)) {
			work->work_free_callback = workmgr_free_callback;
			event_data = kzalloc(MAX_MSGSIZE, GFP_ATOMIC|__GFP_NOWARN);
			work->event_data = event_data;
			atomic_inc(&workmgr_alloc_no);
			alloc_no = atomic_read(&workmgr_alloc_no);
			if (alloc_no > workmgr_max_alloc_no)
				workmgr_max_alloc_no = alloc_no;
		} else {
			workmgr_alloc_fail++;
		}
	} else {
		work = NULL;
		if(printk_ratelimit())
			printk(KERN_ERR "event work memory alloc fail try normal!\n");
		workmgr_alloc_fail++;
	}
	return work;
}

static void send_event_msg(struct work_struct *workp)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;
    int len = NLMSG_SPACE(MAX_MSGSIZE);
    
    struct event_work *p_event_work = container_of(workp, struct event_work, mywork);

    skb_1 = alloc_skb(len, GFP_KERNEL);
    if(!skb_1){
    	event_send_err++;
        printk(KERN_ERR "send_event_msg:alloc_skb error\n");
        goto finish;
    }

    nlh = nlmsg_put(skb_1, 0, 0, p_event_work->type, MAX_MSGSIZE, 0);

#if  LINUX_VERSION_CODE < KERNEL_VERSION(3,18,19)
	NETLINK_CB(skb_1).pid = 0; 
#else
    NETLINK_CB(skb_1).portid = 0;
#endif
    NETLINK_CB(skb_1).dst_group = 1;

	memcpy(NLMSG_DATA(nlh), p_event_work->event_data, p_event_work->event_data_len);

	ECNT_EVENT_BROADCAST((struct ecnt_data *)skb_1);

	if(nl_sk){
   		netlink_broadcast(nl_sk, skb_1, 0, 1, GFP_KERNEL);
   	}else{
    	printk(KERN_ERR "Should first init netlink sock!\n");
    	consume_skb(skb_1);
    }
finish:
	if (p_event_work->work_free_callback) {
		(*p_event_work->work_free_callback)(p_event_work);
	}
}

static int work_limit_read(char *page, char **start, off_t offset,
			int count, int *eof, void *data)
{
	char *out = page;
	int len;
	int i;
	struct event_work_head *list;

	out += sprintf(out, "workmgr_limit = %d\n", workmgr_limit);
	out += sprintf(out, "workmgr_limit_max = %d\n", workmgr_limit_max);
	out += sprintf(out, "workmgr_alloc_no = %d\n", atomic_read(&workmgr_alloc_no));
	out += sprintf(out, "workmgr_alloc_fail = %d\n", workmgr_alloc_fail);
	out += sprintf(out, "workmgr_max_alloc_no = %d\n", workmgr_max_alloc_no);
	out += sprintf(out, "workmgr_max_list_len = %d\n", workmgr_max_list_len);
	out += sprintf(out, "event_send_total = %d\n", event_send_total);
	out += sprintf(out, "event_send_err = %d\n", event_send_err);

	for (i=0; i<WORKMGR_MAX_QUEUE; i++) {
		list = &workmgr_pool[i].list;
		out += sprintf(out, "event_work_queue_len CPU%d = %d\n", i, work_queue_len(list));
	}	
	
	len = out - page;
	len -= offset;
	if (len < count) {
		*eof = 1;
		if (len <= 0)
			return 0;
	} else
		len = count;

	*start = page + offset;
	return len;
}

static int work_limit_write(struct file *file, const char __user * buffer,
			     unsigned long count, void *data)
{
	char buf[64];
	int val;

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	val = simple_strtoul(buf, NULL, 10);

	workmgr_limit_max = val;

	return count;
}

static void workmgr_mem_init(void)
{
	struct event_work_head *list;
	struct event_work *work;
	struct ecnt_event_data *event_data;
	int alloc_no;
	int i;

	list = &workmgr_pool[WORKMGR_QUEUE_ID].list;
	for(i=0; i<workmgr_limit; i++) {
		work = kzalloc(sizeof(struct event_work), GFP_ATOMIC|__GFP_NOWARN);
		if (likely(work)) {
			work->work_free_callback = workmgr_free_callback;
			event_data = kzalloc(MAX_MSGSIZE, GFP_ATOMIC|__GFP_NOWARN);
			work->event_data = event_data;

			work_queue_head(list, work);
		} else {
			workmgr_alloc_fail++;
			printk(KERN_ERR "event work memory init fail Once!\n");
		}
	}
}

static void user_to_kernel (struct sk_buff *skb)
{
	/* TODO userspace -> kenrel event */
}

static void netlink_sock_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19)
	struct netlink_kernel_cfg cfg = {
		.input		= user_to_kernel,
		.flags		= NL_CFG_F_NONROOT_RECV,
		.groups		= 1,
	};	
	nl_sk = netlink_kernel_create(&init_net, NETLINK_ECNT_EVENT, &cfg);
#else
	nl_sk = netlink_kernel_create(&init_net, NETLINK_ECNT_EVENT, 0, user_to_kernel, NULL, THIS_MODULE);
#endif
	printk(KERN_INFO "Initializing Econet Event Netlink Socket nl_sk = %p\r\n", nl_sk);
}

int ecnt_send_event(unsigned short type, 
	struct ecnt_event_data *event_data, int event_data_len)
{
	struct event_work *event_workp = workmgr_alloc();

	event_send_total++;
	
	if(event_workp == NULL){
		event_send_err++;
		if(printk_ratelimit())
			printk(KERN_ERR "workmgr alloc fail, maybe too many events(0x%x)!!\n", type);
		return 0;
	}

	event_workp->type = type;
	
	if(event_data_len < MAX_MSGSIZE)
		event_workp->event_data_len = event_data_len;
	else
		event_workp->event_data_len = MAX_MSGSIZE;

	memcpy(event_workp->event_data, event_data, event_workp->event_data_len);
	
	INIT_WORK(&(event_workp->mywork), send_event_msg);

	if(schedule_work(&(event_workp->mywork)) == 0)
	{
		event_send_err++;
		printk(KERN_ERR "event(0x%x) work schedule fail!!\n", type);
		if (event_workp->work_free_callback) {
			(*event_workp->work_free_callback)(event_workp);
		}
		return 0;
	}
	
	return 1;
}

EXPORT_SYMBOL(ecnt_send_event);

int ecnt_event_execute(struct ecnt_data *in_data, struct ecnt_event_handle *event_handle)
{
	int i = 0;
	struct nlmsghdr *nlh;
	struct sk_buff *skb = (struct sk_buff *)in_data;
	struct ecnt_event_data *event_data;
	unsigned short event_type;
    
	nlh = (struct nlmsghdr *)skb->data;
	
	event_data = (struct ecnt_event_data *)(NLMSG_DATA(nlh));

	while(event_handle[i].handle){
		event_type = (((event_handle[i].maintype)<<8) | (event_handle[i].subtype));
		if(nlh->nlmsg_type == event_type){
			event_handle[i].handle(event_data);
		}
		i++;
	}
	
	return 1;
}

EXPORT_SYMBOL(ecnt_event_execute);

struct ecnt_event_source* ecnt_event_register(char *name, 
	int (*hook)(struct ecnt_data *in_data))
{
	struct ecnt_hook_ops *ecnt_event_opt;
	
	ecnt_event_opt = kzalloc(sizeof(struct ecnt_hook_ops), GFP_KERNEL);	
	if (ecnt_event_opt == NULL)
		return NULL;

	ecnt_event_opt->is_execute = 1;
	ecnt_event_opt->hookfn = hook;
	ecnt_event_opt->maintype = ECNT_HOOK_EVENT;
	ecnt_event_opt->subtype = ECNT_HOOK_EVENT_SUB;
	ecnt_event_opt->priority = 1;
	ecnt_event_opt->name = name;
	
	if(ecnt_register_hook(ecnt_event_opt)) {
		printk("ecnt_event_opt register fail\n");
		kfree(ecnt_event_opt);
		return NULL;
	}
	
	printk("ecnt_event_opt register......%s\n", ecnt_event_opt->name);
	return ecnt_event_opt;
}

EXPORT_SYMBOL(ecnt_event_register);

int ecnt_event_unregister(struct ecnt_event_source *source){

	/* Free Source */
	ecnt_unregister_hook(source);
	kfree(source);
	return 0;
}

EXPORT_SYMBOL(ecnt_event_unregister);

static int __init event_notify_init(void)
{
	struct proc_dir_entry *test_proc;
	int i;

	for (i=0; i<WORKMGR_MAX_QUEUE; i++) {
		work_queue_head_init(&workmgr_pool[i].list);
	}	
	workmgr_mem_init();

	test_proc = create_proc_entry("tc3162/workmgr_limit", 0, NULL);
	if (!test_proc) 
		return 0;

	test_proc->read_proc = work_limit_read;
	test_proc->write_proc = work_limit_write;
	
	printk(KERN_INFO "Initializing Ecnt Event Netlink Socket\r\n");
	netlink_sock_init();
	
	return 0;
}

static void __exit event_notify_exit(void)
{
	sock_release(nl_sk->sk_socket);
	remove_proc_entry("tc3162/workmgr_limit", 0);
	printk(KERN_INFO "Goodbye\r\n");
	
}

subsys_initcall(event_notify_init);
module_exit(event_notify_exit);

MODULE_DESCRIPTION("Ecnt Event Notify Driver");

