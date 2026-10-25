/***************************************************************
Copyright Statement:

This software/firmware and related documentation (??EcoNet Software??) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (??EcoNet??) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (??ECONET SOFTWARE??) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ??AS IS?? 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER??S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER??S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#else
#include <asm/uaccess.h>
#endif


struct mutex signal_mutex;
struct task_struct *mount_task_p;


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int send_signal_to_app(int signal);
int init_auto_mount(void);
extern int send_sig(int, struct task_struct *, int);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/*---------------------------------------------------------------------
 ** function mame: automount_pid_write_proc
 *
 ** description:
 *      1. receives AutoMount's PID from user space and turns it into
 *         a task_struct pointer which will be used to send signals to
 *         AutoMount later. 
 *
 ** parameters:
 *      1. buffer: the pointer pointing to the string received from user
 *      2. count: the length of the string in buffer
 *
 ** global:
 *      1. mount_task_p: the pointer pointing to the task_struct structure
 *          that is going to be used to send signals to AutoMount
 *
 ** return: int
 *
 ** call: none
 *
 ** revision:
 *      1. Trey 2010/08/16
 *---------------------------------------------------------------------*/
static int automount_pid_write_proc(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
    char str_pid[16];
    pid_t mount_pid = 0;
    char *cp;
    if (copy_from_user(str_pid, buffer, count)){
        printk("automount_pid_write_proc failed\n");
        return -EFAULT;
    }
    str_pid[count] = '\0';
     
     /* atoi func */
    for (cp = str_pid; *cp != '\0'; cp++) {
        switch (*cp) {
        case '0' ... '9':
            mount_pid = 10*mount_pid+(*cp-'0');
            break;
        default:
            break;
        }
    }
    mount_task_p = find_task_by_vpid(mount_pid);

    return 0;
}

int send_signal_to_app(int signal)
{
     if(mount_task_p != NULL)
    {
    	printk("FUNCTION ==> %s locked\n", __FUNCTION__);
        mutex_lock(&signal_mutex);
        send_sig(signal, mount_task_p, 0);
        mutex_unlock(&signal_mutex);
    }
    return 0;
}

int init_auto_mount(void)
{
    struct proc_dir_entry *automount_proc;

/* 1. mount_task_p: a pointer which will (later) point to the task_struct 
 *      structure that stands for AutoMount's PID. The pointer will be used
 *      by send_sig() to send SIGUSR1 or SIGUSR2 to AutoMount.
 * 2. signal_mutex: a mutex used to prevent more than one signals are sent
 *      at the same time.
 * 3. automount_proc: used when AutoMount write its PID to
 *      "tc3162/automount_pid", automount_pid_write_proc() will be
 *      triggered to transform the PID to the task_struct structure which
 *      is pointed by mount_task_p. Then, mount_task_p can be used to send
 *      signals to AutoMount.  --Trey 2010/08/17
 */
    mount_task_p = NULL;
    mutex_init(&signal_mutex);
    automount_proc = create_proc_entry("tc3162/automount_pid", 0, NULL);
    automount_proc->write_proc = automount_pid_write_proc;
	return 0;
}

