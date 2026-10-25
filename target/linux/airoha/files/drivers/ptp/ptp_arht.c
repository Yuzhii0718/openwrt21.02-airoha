/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/ptp_clock_kernel.h>
#include <linux/ptp_classify.h>
#include <modules/ptp/ptp.h>
#include <asm/tc3162/tc3162.h>

#include "ptp_private.h"
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
struct arht_ptp_s {
	struct ptp_clock *ptp_clock;
	struct ptp_clock_info ptp_caps;
	spinlock_t ptp_lock;
};


/************************************************************************
*                       M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
static struct arht_ptp_s arht_ptp_clock;


/************************************************************************
*       E X T E R N A L	 F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
extern int tod_current_time(u64 *sec, u32 *nsec);
extern void set_freq_via_tod(uint32_t *tod_freq);
extern int set_timestamp_to_tod(u32 sec, u32 nsec);
extern void set_tod_add_unit(uint32_t *tod_add_unit);
extern void tod_init_time(void);
extern void tod_module_init(void);

static int arht_ptp_clock_adjfreq(struct ptp_clock_info *ptp, s32 delta)
{
	uint32_t tod_adj_freq[2];
	unsigned long flags;

	spin_lock_irqsave(&arht_ptp_clock.ptp_lock, flags);
	/* "-mgeral-regs-only" is not compatible with the use of floating-point type.
	The original "1e9"&"2e9" is floating-point type, so it is changrd to integer type. */
	tod_adj_freq[0] = 1000000000 + delta;
	tod_adj_freq[1] = 2000000000;
	set_freq_via_tod(tod_adj_freq);
	spin_unlock_irqrestore(&arht_ptp_clock.ptp_lock, flags);
	return 0;
}


static int arht_ptp_clock_adjtime(struct ptp_clock_info *ptp, s64 delta)
{
	struct timespec64 now, new, deltatime;
	u64 sec = 0;
	u32 nsec = 0;
	s64 update_time = 0;
	unsigned long flags;

	memset(&now, 0, sizeof(struct timespec64));
	memset(&new, 0, sizeof(struct timespec64));
	memset(&deltatime, 0, sizeof(struct timespec64));

	spin_lock_irqsave(&arht_ptp_clock.ptp_lock, flags);
	tod_current_time(&sec, &nsec);
	now.tv_sec = sec;
	now.tv_nsec = nsec;

	update_time = (timespec64_to_ns(&now) + delta);

	new = ns_to_timespec64(update_time);

	set_timestamp_to_tod(new.tv_sec, new.tv_nsec);

	tod_current_time(&sec, &nsec);
	spin_unlock_irqrestore(&arht_ptp_clock.ptp_lock, flags);

	return 0;
}

static int arht_ptp_clock_adjfine(struct ptp_clock_info *ptp, long scaled_ppm)
{
	int negative = 0;
	long ppb = 0;
	long offset = 0;
	uint32_t tod_adj_freq[2];
	unsigned long flags;

	if (!scaled_ppm) {
		return 0;
	} else if (scaled_ppm < 0) {
		negative = 1;
		scaled_ppm = labs(-scaled_ppm);
	}
	ppb = ((scaled_ppm*1000)>>16);

	if (negative)
		ppb = -ppb;

	spin_lock_irqsave(&arht_ptp_clock.ptp_lock, flags);
	offset = ppb;
	/* "-mgeral-regs-only" is not compatible with the use of floating-point type.
	The original "1e9"&"2e9" is floating-point type, so it is changrd to integer type. */
	tod_adj_freq[0] = 1000000000 + offset;
	tod_adj_freq[1] = 2000000000;
	set_freq_via_tod(tod_adj_freq);
	spin_unlock_irqrestore(&arht_ptp_clock.ptp_lock, flags);

	return 0;
}

static int arht_ptp_clock_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
	int ret = 0;
	u64 sec = 0;
	u32 nsec = 0;
	unsigned long flags;

	spin_lock_irqsave(&arht_ptp_clock.ptp_lock, flags);
	tod_current_time(&sec, &nsec);
	ts->tv_sec = sec;
	ts->tv_nsec = nsec;
	spin_unlock_irqrestore(&arht_ptp_clock.ptp_lock, flags);

	return ret;
}

static int arht_ptp_clock_settime(struct ptp_clock_info *ptp, const struct timespec64 *ts)
{
	return 0;
}

static int arht_ptp_clock_enable(struct ptp_clock_info *ptp, struct ptp_clock_request *request, int on)
{
	return 0;
}

static const struct ptp_clock_info arht_ptp_clock_caps = {
	.owner		= THIS_MODULE,
	.name		= "ARHT timer",
	.max_adj	= 200000000,
	.n_ext_ts	= 0,
	.n_pins		= 0,
	.pps		= 0,
	.adjfine	= arht_ptp_clock_adjfine,
	.adjfreq	= arht_ptp_clock_adjfreq,
	.adjtime	= arht_ptp_clock_adjtime,
	.gettime64	= arht_ptp_clock_gettime,
	.settime64	= arht_ptp_clock_settime,
	.enable		= arht_ptp_clock_enable,
};

/* module operations */
static void __exit ptp_arht_exit(void)
{
	ptp_clock_unregister(arht_ptp_clock.ptp_clock);
}

dev_t devt;

static int __init ptp_arht_init(void)
{
	int err;
	struct posix_clock *clk = NULL;
	struct ptp_clock *ptp;

	arht_ptp_clock.ptp_caps = arht_ptp_clock_caps;

	arht_ptp_clock.ptp_clock = ptp_clock_register(&arht_ptp_clock.ptp_caps, NULL);

	ptp = arht_ptp_clock.ptp_clock;

	clk = &ptp->clock;
	cdev_device_del(&clk->cdev, clk->dev);

	devt = MKDEV(251, 0);
	err = register_chrdev_region(devt, MINORMASK + 1, "ptp");
	if (err <0) {
		pr_err("%s().%d ptp: failed to allocate device region\n", __func__, __LINE__);
	}
	ptp->devid = devt;
	ptp->dev.devt = ptp->devid;

	err = posix_clock_register(&ptp->clock, &ptp->dev);
	if (err <0) {
		pr_err("%s().%d ptp: failed to allocate device region\n", __func__, __LINE__);
	}

	if (IS_ERR(arht_ptp_clock.ptp_clock))
		return PTR_ERR(arht_ptp_clock.ptp_clock);

	tod_module_init();

	return 0;
}

module_init(ptp_arht_init);
module_exit(ptp_arht_exit);

MODULE_DESCRIPTION("ARHT PTP clock");
MODULE_LICENSE("GPL");
