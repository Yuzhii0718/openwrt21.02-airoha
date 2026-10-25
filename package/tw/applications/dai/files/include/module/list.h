/*-
 * Copyright (c) 2011 Felix Fietkau <nbd@openwrt.org>
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _LINUX_LIST_H_
#define _LINUX_LIST_H_

#include <stddef.h>
#include <stdbool.h>

#define	prefetch(x)

#ifndef container_of
#define container_of(ptr, type, member)					\
	({								\
		const typeof(((type *) NULL)->member) *__mptr = (ptr);	\
		(type *) ((char *) __mptr - offsetof(type, member));	\
	})
#endif

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#undef LIST_HEAD
#define LIST_HEAD(name)	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *obj,
			      struct list_head *prev,
			      struct list_head *next)
{
	prev->next = obj;
	obj->prev = prev;
	obj->next = next;
	next->prev = obj;
}
static inline void
list_add(struct list_head *_new, struct list_head *head)
{
  __list_add(_new, head, head->next);
}

static inline void list_add_tail(struct list_head *obj, struct list_head *head)
{
	__list_add(obj, head->prev, head);
}

static inline void list_add_head(struct list_head *obj, struct list_head *head)
{
	__list_add(obj, head, head->next);
}

static inline void list_del(struct list_head *obj)
{
	obj->next->prev = obj->prev;
	obj->prev->next = obj->next;
}

static inline bool list_empty(struct list_head *head)
{
	return head->next == head;
}

#define list_entry(ptr, type, member)	container_of(ptr, type, member)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &(pos)->member != (head);					\
	     (pos) = list_entry((pos)->member.next, typeof(*(pos)), member))

#endif /* _LINUX_LIST_H_ */
