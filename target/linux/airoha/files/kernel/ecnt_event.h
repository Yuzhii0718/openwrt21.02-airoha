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
#ifndef ECNT_EVENT_H_
#define	ECNT_EVENT_H_

/************************************************************************
*                  I N C L U D E S
************************************************************************/
#include "ecnt_event_global/ecnt_event_global.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/
#define WORKMGR_QUEUE_ID 0
#define WORKMGR_MAX_QUEUE 1

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
struct event_work{
	/* These two members must be first. */
	struct event_work		*next;
	struct event_work		*prev;

	int type;
	int event_data_len;
	int	(*work_free_callback)(struct event_work *work);
	
	struct ecnt_event_data *event_data;

	struct work_struct mywork;	
};

struct event_work_head {
	/* These two members must be first. */
	struct event_work	*next;
	struct event_work	*prev;

	__u32		qlen;
	spinlock_t	lock;
};

union {
	struct event_work_head  list;
	char                    pad[SMP_CACHE_BYTES];
} workmgr_pool[WORKMGR_MAX_QUEUE];


/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/
atomic_t workmgr_alloc_no;

int workmgr_limit = 12;
int workmgr_limit_max = 30;
int workmgr_max_alloc_no = 0;
int workmgr_alloc_fail = 0;
int workmgr_max_list_len = 0;

int event_send_total = 0;
int event_send_err = 0;

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/
static inline __u32 work_queue_len(const struct event_work_head *list_)
{
	return list_->qlen;
}

static inline struct event_work *work_peek(const struct sk_buff_head *list_)
{
	struct event_work *work = list_->next;

	if (work == (struct event_work *)list_)
		work = NULL;
	return work;
}

static inline void __work_unlink(struct event_work *work, struct event_work_head *list)
{
	struct event_work *next, *prev;

	list->qlen--;
	next	   = work->next;
	prev	   = work->prev;
	work->next  = work->prev = NULL;
	next->prev = prev;
	prev->next = next;
}

static inline struct event_work *__work_dequeue(struct event_work_head *list)
{
	struct event_work *work = work_peek(list);
	if (work)
		__work_unlink(work, list);
	return work;
}

static inline void __work_insert(struct event_work *newsk,
				struct event_work *prev, struct event_work *next,
				struct event_work_head *list)
{
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
	list->qlen++;
}

static inline void __work_queue_after(struct event_work_head *list,
				     struct event_work *prev,
				     struct event_work *newsk)
{
	__work_insert(newsk, prev, prev->next, list);
}

static inline void __work_queue_head(struct event_work_head *list,
				    struct event_work *newsk)
{
	__work_queue_after(list, (struct event_work *)list, newsk);
}

static inline void __work_queue_head_init(struct event_work_head *list)
{
	list->prev = list->next = (struct event_work *)list;
	list->qlen = 0;
}

static inline void work_queue_head_init(struct event_work_head *list)
{
	spin_lock_init(&list->lock);
	__work_queue_head_init(list);
}

#endif/* ECNT_EVENT_H_ */

