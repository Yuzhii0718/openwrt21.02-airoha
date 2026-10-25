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
#include <asm/tc3162/tc3162.h>

#include <ecnt_event_global/ecnt_event_xdsl.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/

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
MODULE_LICENSE("GPL");

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/
static int send_xdsl_event_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64] = {0};
	char test_string[64] = {0};
	unsigned long type = 0;
	struct event1_data mypara1;
	struct event2_data mypara2;
	struct event3_data mypara3;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	sscanf(val_string, "%x %s", &type, test_string);

#define TIME_C
#ifdef TIME_C
	int start_time = 0, end_time = 0, usetime = 0;	
	VPint(0xBFBF0114) = 0xFFFFFFFF;
    VPint(0xBFBF0100) = 0x406;
	start_time = VPint(0xBFBF0118);
#endif

/*************************************************************************************/
	if((type & 0xff00) == 0x0 || (type & 0xff00) == 0x300){
		mypara1.type = 0x11;
		mypara1.sub_type = 0x22;
		strcpy(mypara1.message, test_string);
		ecnt_send_event(type, &mypara1, sizeof(mypara1));
	}else if((type & 0xff00) == 0x100 || (type & 0xff00) == 0x400){
		mypara2.status = 0x88;
		strcpy(mypara2.message, test_string);
		ecnt_send_event(type, &mypara2, sizeof(mypara2));
	}else if((type & 0xff00) == 0x200 || (type & 0xff00) == 0x500){
		strcpy(mypara3.message, test_string);	
		ecnt_send_event(type, &mypara3, sizeof(mypara3));
	}
/*************************************************************************************/

#ifdef TIME_C
	end_time = VPint(0xBFBF0118);
	usetime = start_time - end_time;
	usetime = usetime*2 /(SYS_HCLK);
	printk("====>  usetime is (0x%x -> 0x%x) = %dus %d\n", start_time, end_time, usetime, SYS_HCLK);
#endif	
	return count;
}

static int __init ecnt_event_xdsl_init(void)
{
	struct proc_dir_entry *test_proc;	
	test_proc = create_proc_entry("tc3162/send_xdsl_event", 0, NULL);
	if (!test_proc) 
		return 0;
	
	test_proc->read_proc = NULL;
	test_proc->write_proc = send_xdsl_event_write_proc;
	return 0;
}

static void __exit ecnt_event_xdsl_exit(void)
{
	remove_proc_entry("tc3162/send_xdsl_event", 0);
	printk(KERN_INFO "Goodbye\r\n");
}

module_init(ecnt_event_xdsl_init);
module_exit(ecnt_event_xdsl_exit);

MODULE_DESCRIPTION("Ecnt Event Xdsl Test Send Driver");

