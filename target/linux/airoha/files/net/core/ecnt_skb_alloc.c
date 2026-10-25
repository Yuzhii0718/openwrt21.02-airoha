#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <linux/kmemcheck.h>
#endif
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/slab.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netdevice.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
#include <linux/proc_fs.h>
#endif
#include <linux/string.h>
#include <linux/skbuff.h>
#include <linux/splice.h>
#include <linux/cache.h>
#include <linux/rtnetlink.h>
#include <linux/init.h>
#include <linux/scatterlist.h>
#include <linux/errqueue.h>
#include <linux/prefetch.h>
#include <linux/if_vlan.h>
#include <linux/ecnt_utility.h>

#if defined(TCSUPPORT_RA_HWNAT)
#include <linux/foe_hook.h>
#endif

atomic_t g_used_skb_num;
#if defined(MT7612E) || defined(MT7613E) || defined(TCSUPPORT_WLAN_MULTI_CHIP)
int g_max_skb_num = 4096;
#else
int g_max_skb_num = 1280;
#endif
EXPORT_SYMBOL(g_used_skb_num);

int peak_skb_num = 0;

#if defined(CONFIG_CPU_TC3162) || defined(CONFIG_MIPS_TC3262) || defined(TCSUPPORT_ECNT_SKBMGR)
atomic_t skbmgr_alloc_no;
atomic_t skbmgr_4k_alloc_no;
#endif

#if defined(CONFIG_CPU_TC3162) || defined(CONFIG_MIPS_TC3262) || defined(TCSUPPORT_ECNT_SKBMGR)

#define SKBMGR_DEF_HOT_LIST_LEN			512

extern void skb_release_data(struct sk_buff *skb);
extern void kfree_skbmem(struct sk_buff *skb);

int skbmgr_limit = 1280;
int skbmgr_max_alloc_no = 0;
int skbmgr_alloc_fail = 0;


int skbmgr_alloc_normal = 0;

int skbmgr_hot_list_len = SKBMGR_DEF_HOT_LIST_LEN;
int skbmgr_max_list_len = 0;

int skbmgr_2k_truesize = 0;
int skbmgr_4k_truesize = 0;
int skbmgr_lro_truesize = 0;

union {
	struct sk_buff_head     list;
	char                    pad[SMP_CACHE_BYTES];
} skbmgr_pool[SKBMGR_MAX_QUEUE];

__IMEM struct sk_buff *skbmgr_alloc_skb2k(void)
{
	struct sk_buff_head *list;
	struct sk_buff *skb;
	int alloc_no;
	
	list = &skbmgr_pool[SKBMGR_QUEUE_ID].list;

	if (skb_queue_len(list)) {
		unsigned int size;
		struct skb_shared_info *shinfo;
		u8 *data;

		skb = skb_dequeue(list);

		if (unlikely(skb == NULL))
			goto try_normal;

#ifdef TCSUPPORT_TEST_VWTEST
		size = skb->truesize-SKB_DATA_ALIGN(sizeof(struct sk_buff))-SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		data = skb->head;

		/*
		 * Only clear those fields we need to clear, not those that we will
		 * actually initialise below. Hence, don't put any more fields after
		 * the tail pointer in struct sk_buff!
		 */
		#ifdef TCSUPPORT_WLAN_SW_RPS 
		memset(skb, 0, offsetof(struct sk_buff, pAd));
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 4, 0)
		skb->fe_resource_mark = 0;
#endif
		#else
		memset(skb, 0, offsetof(struct sk_buff, tail));
		#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
		atomic_set(&skb->users, 1);
#else
		refcount_set(&skb->users, 1);
#endif			
		skb->head = data;
		skb->data = data;
		skb_reset_tail_pointer(skb);
		skb->end = skb->tail + size;
		skb->mac_header = (typeof(skb->mac_header))~0U;
		skb->transport_header = (typeof(skb->transport_header))~0U;

		/* make sure we initialize shinfo sequentially */
		shinfo = skb_shinfo(skb);
		memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
		atomic_set(&shinfo->dataref, 1);

		skb->skb_recycling_callback = skbmgr_recycling_callback;
		skb->skb_recycling_ind = SKBMGR_INDICATION;
#endif

		atomic_inc(&skbmgr_alloc_no);
		alloc_no = atomic_read(&skbmgr_alloc_no);
		if (alloc_no > skbmgr_max_alloc_no)
			skbmgr_max_alloc_no = alloc_no;

		return skb;
	}

try_normal:
	if ((skbmgr_limit == 0) || (atomic_read(&skbmgr_alloc_no) < skbmgr_limit)) {
		skbmgr_alloc_normal++;
		skb = alloc_skb(SKBMGR_RX_BUF_LEN, GFP_ATOMIC|__GFP_NOWARN);
		if (likely(skb)) {
			skb->skb_recycling_callback = skbmgr_recycling_callback;
			skb->skb_recycling_ind = SKBMGR_INDICATION;
			if(0 == skbmgr_2k_truesize)
			{
				skbmgr_2k_truesize = skb->truesize;
			}
			atomic_inc(&skbmgr_alloc_no);
			alloc_no = atomic_read(&skbmgr_alloc_no);
			if (alloc_no > skbmgr_max_alloc_no)
				skbmgr_max_alloc_no = alloc_no;
		} else {
			skbmgr_alloc_fail++;
		}
	} else {
		skb = NULL;
		skbmgr_alloc_fail++;
	}
	return skb;
}

EXPORT_SYMBOL(skbmgr_alloc_skb2k);

#if defined(TCSUPPORT_TEST_VWTEST)
int syncSkbFlag = 0;
EXPORT_SYMBOL(syncSkbFlag);
__IMEM void skb_queue_head_ecnt(struct sk_buff_head *list, struct sk_buff *newsk)
{
	unsigned long flags;

	spin_lock_bh_ecnt(&list->lock);
	__skb_queue_head(list, newsk);
	spin_unlock_bh_ecnt(&list->lock);
}
EXPORT_SYMBOL(skb_queue_head_ecnt);
#endif

__IMEM int skbmgr_recycling_callback(struct sk_buff *skb)
{
	struct sk_buff_head *list;

	list = &skbmgr_pool[SKBMGR_QUEUE_ID].list;	

	if (skb_queue_len(list) < skbmgr_hot_list_len) {
		if ((skbmgr_2k_truesize != skb->truesize) ||
			skb_is_nonlinear(skb) ||
			(skb->fclone != SKB_FCLONE_UNAVAILABLE) ||
			skb_cloned(skb) ||
			(skb_shinfo(skb)->nr_frags) || 
			skb_has_frag_list(skb)) {
			return 0;
		}
#if defined(TCSUPPORT_HWNAT)		
		pktflow_free(skb);		
#endif
#if defined(TCSUPPORT_RA_HWNAT)
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
#endif
		if (skb_queue_len(list) > skbmgr_max_list_len)
			skbmgr_max_list_len = skb_queue_len(list) + 1;

#ifndef TCSUPPORT_TEST_VWTEST
		unsigned int size;
		struct skb_shared_info *shinfo;
		u8 *data;
		
		size = skb->truesize-SKB_DATA_ALIGN(sizeof(struct sk_buff))-SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		data = skb->head;

		/*
		 * Only clear those fields we need to clear, not those that we will
		 * actually initialise below. Hence, don't put any more fields after
		 * the tail pointer in struct sk_buff!
		 */
		#ifdef TCSUPPORT_WLAN_SW_RPS 
		memset(skb, 0, offsetof(struct sk_buff, pAd));
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 4, 0)
		skb->fe_resource_mark = 0;	
#endif
		#else
		memset(skb, 0, offsetof(struct sk_buff, tail));
		#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
		atomic_set(&skb->users, 1);
#else
		refcount_set(&skb->users, 1); 
#endif	

		skb->head = data;
		skb->data = data;
		skb_reset_tail_pointer(skb);
		skb->end = skb->tail + size;
		skb->mac_header = (typeof(skb->mac_header))~0U;
		skb->transport_header = (typeof(skb->transport_header))~0U;

		/* make sure we initialize shinfo sequentially */
		shinfo = skb_shinfo(skb);
		memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
		atomic_set(&shinfo->dataref, 1);

		skb->skb_recycling_callback = skbmgr_recycling_callback;
		skb->skb_recycling_ind = SKBMGR_INDICATION;
#endif
#if defined(TCSUPPORT_TEST_VWTEST)
		if(syncSkbFlag)
			skb_queue_head_ecnt(list, skb);
		else
#endif
			skb_queue_head(list, skb);

		return 1;
	}

	return 0;
}

EXPORT_SYMBOL(skbmgr_recycling_callback);

#if defined(TCSUPPORT_MAX_PACKET_2000) || defined(MT7612E) || defined(MT7613E)|| defined(TCSUPPORT_WLAN_MULTI_CHIP)
#ifdef TCSUPPORT_CPU_EN7521
#define SKBMGR_4K_LIMIT					4096
#else
#define SKBMGR_4K_LIMIT					2048
#endif
#else
#if defined(MT7592)
#define SKBMGR_4K_LIMIT					1024
#else
#define SKBMGR_4K_LIMIT					512
#endif
#endif

#if defined(TCSUPPORT_MAX_PACKET_2000) || defined(MT7612E) || defined(MT7615E) || defined(MT7613E)|| defined(TCSUPPORT_WLAN_MULTI_CHIP)
#ifdef TCSUPPORT_WLAN_MT7615_V34
#define SKBMGR_4K_DEF_HOT_LIST_LEN              1024
#else
#define SKBMGR_4K_DEF_HOT_LIST_LEN		512
#endif
#else
#define SKBMGR_4K_DEF_HOT_LIST_LEN		128
#endif

int skbmgr_4k_max_alloc_no = 0;
int skbmgr_4k_alloc_fail = 0;
int skbmgr_4k_alloc_normal = 0;

//int skbmgr_limit = SKBMGR_4K_LIMIT; 
int skbmgr_4k_hot_list_len = SKBMGR_4K_DEF_HOT_LIST_LEN;
int skbmgr_4k_max_list_len = 0;
__DMEM int skbmgr_4k_limit = SKBMGR_4K_LIMIT;


union {
	struct sk_buff_head     list;
	char                    pad[SMP_CACHE_BYTES];
} skbmgr_4k_pool[SKBMGR_MAX_QUEUE];
#ifdef TCSUPPORT_TEST_VWTEST
void dump_skbinfo(void)
{
	int i;
	struct sk_buff_head *list;

	printk("skbmgr_limit = %d %d\n", skbmgr_limit, SKBMGR_RX_BUF_LEN);
	printk("skbmgr_max_alloc_no = %d\n", skbmgr_max_alloc_no);
	printk("skbmgr_alloc_fail = %d\n", skbmgr_alloc_fail);
	printk("skbmgr_alloc_no = %d\n", atomic_read(&skbmgr_alloc_no));
	printk("skbmgr_max_list_len = %d\n", skbmgr_max_list_len);
	printk("skbmgr_alloc_normal = %d\n", skbmgr_alloc_normal);
	printk("\nskbmgr_4k_limit = %d\n", skbmgr_4k_limit);
	printk("skbmgr_4k_max_alloc_no = %d\n", skbmgr_4k_max_alloc_no);
	printk("skbmgr_4k_alloc_fail = %d\n", skbmgr_4k_alloc_fail);
	printk("skbmgr_4k_alloc_no = %d\n", atomic_read(&skbmgr_4k_alloc_no));
	printk("skbmgr_4k_max_list_len = %d\n", skbmgr_4k_max_list_len);
	printk("skbmgr_4k_alloc_normal = %d\n", skbmgr_4k_alloc_normal);
	printk("skbmgr_2k_truesize = %d\n",  skbmgr_2k_truesize);
	printk("skbmgr_4k_truesize = %d\n",  skbmgr_4k_truesize);
	printk("skbmgr_lro_truesize = %d\n", skbmgr_lro_truesize);
	
	printk("skbmgr_hot_list_len %d\n", skbmgr_hot_list_len);
	printk("skbmgr_4k_hot_list_len %d\n", skbmgr_4k_hot_list_len);
	printk("offsetof pAd&dataref %d %d\n", offsetof(struct sk_buff, pAd), offsetof(struct skb_shared_info, dataref));

	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_pool[i].list;
		printk("skbmgr_queue_len CPU%d = %d\n", i, skb_queue_len(list));
	}	

	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_4k_pool[i].list;
		printk("skbmgr_4k_queue_len CPU%d = %d\n", i, skb_queue_len(list));
	}	
	
	printk("\n");

}
EXPORT_SYMBOL(dump_skbinfo);

void clear_skbinfo(void)
{
	skbmgr_max_alloc_no = 0;
	skbmgr_alloc_fail = 0;
	skbmgr_alloc_normal = 0;
	skbmgr_4k_max_alloc_no = 0;
	skbmgr_4k_alloc_fail = 0;
	skbmgr_4k_alloc_normal = 0;
}
EXPORT_SYMBOL(clear_skbinfo);
#endif
__IMEM struct sk_buff *skbmgr_alloc_skb4k(void)
{
	struct sk_buff_head *list;
	struct sk_buff *skb;
	int alloc_no;
	
	list = &skbmgr_4k_pool[SKBMGR_QUEUE_ID].list;
	
	if (skb_queue_len(list)) {
		skb = skb_dequeue(list);

		if (unlikely(skb == NULL))
			goto try_normal;

#if !defined(TCSUPPORT_TEST_VWTEST) && (!defined(TCSUPPORT_WLAN_MT7615_V34) || !defined(TCSUPPORT_CPU_EN7516))
		unsigned int size;
		struct skb_shared_info *shinfo;
		u8 *data;

		size = skb->truesize-SKB_DATA_ALIGN(sizeof(struct sk_buff))-SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		data = skb->head;

		/*
		 * Only clear those fields we need to clear, not those that we will
		 * actually initialise below. Hence, don't put any more fields after
		 * the tail pointer in struct sk_buff!
		 */
		#ifdef TCSUPPORT_WLAN_SW_RPS 
		memset(skb, 0, offsetof(struct sk_buff, pAd));
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 4, 0)
		skb->fe_resource_mark = 0;
#endif
		#else
		memset(skb, 0, offsetof(struct sk_buff, tail));
		#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		atomic_set(&skb->users, 1);
#else
		refcount_set(&skb->users, 1);
#endif	
		skb->head = data;
		skb->data = data;
		skb_reset_tail_pointer(skb);
		skb->end = skb->tail + size;
		skb->mac_header = (typeof(skb->mac_header))~0U;
		skb->transport_header = (typeof(skb->transport_header))~0U;

		/* make sure we initialize shinfo sequentially */
		shinfo = skb_shinfo(skb);
		memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
		atomic_set(&shinfo->dataref, 1);

		skb->skb_recycling_callback = skbmgr_4k_recycling_callback;
		skb->skb_recycling_ind = SKBMGR_4K_INDICATION;
#endif
		atomic_inc(&skbmgr_4k_alloc_no);		
		alloc_no = atomic_read(&skbmgr_4k_alloc_no);

		if (alloc_no > skbmgr_4k_max_alloc_no)
			skbmgr_4k_max_alloc_no = alloc_no;

		return skb;
	}

try_normal:
	if ((atomic_read(&skbmgr_4k_alloc_no) < skbmgr_4k_limit)){
		skbmgr_4k_alloc_normal++;
		skb = alloc_skb(SKBMGR_4K_RX_BUF_LEN , GFP_ATOMIC|__GFP_NOWARN);
		if (likely(skb)) {
			skb->skb_recycling_callback = skbmgr_4k_recycling_callback;
			skb->skb_recycling_ind = SKBMGR_4K_INDICATION;
			if(0 == skbmgr_4k_truesize)
			{
				skbmgr_4k_truesize = skb->truesize;
			}

			atomic_inc(&skbmgr_4k_alloc_no);	
			alloc_no = atomic_read(&skbmgr_4k_alloc_no);

			if (alloc_no > skbmgr_4k_max_alloc_no)
				skbmgr_4k_max_alloc_no = alloc_no;
		} else {
			skbmgr_4k_alloc_fail++;
		}
	} else {
		skb = NULL;
		skbmgr_4k_alloc_fail++;
	}
	return skb;
}

EXPORT_SYMBOL(skbmgr_alloc_skb4k);

__IMEM int skbmgr_4k_recycling_callback(struct sk_buff *skb)
{
	struct sk_buff_head *list;

	list = &skbmgr_4k_pool[SKBMGR_QUEUE_ID].list;	

	if (skb_queue_len(list) < skbmgr_4k_hot_list_len) {
		if ((skbmgr_4k_truesize != skb->truesize) ||
			(skb->fclone != SKB_FCLONE_UNAVAILABLE) ||
			skb_cloned(skb) ||
			(skb_shinfo(skb)->nr_frags) || 
			skb_has_frag_list(skb)) {
			return 0;
		}

#if defined(TCSUPPORT_HWNAT)		
		pktflow_free(skb);		
#endif
#if defined(TCSUPPORT_RA_HWNAT)
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
#endif
		if (skb_queue_len(list) > skbmgr_4k_max_list_len)
			skbmgr_4k_max_list_len = skb_queue_len(list) + 1;

#if defined(TCSUPPORT_TEST_VWTEST) || (defined(TCSUPPORT_WLAN_MT7615_V34) && defined(TCSUPPORT_CPU_EN7516))

		unsigned int size;
		struct skb_shared_info *shinfo;
		u8 *data;
		size = skb->truesize-SKB_DATA_ALIGN(sizeof(struct sk_buff))-SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		data = skb->head;

		/*
		 * Only clear those fields we need to clear, not those that we will
		 * actually initialise below. Hence, don't put any more fields after
		 * the tail pointer in struct sk_buff!
		 */
#ifdef TCSUPPORT_WLAN_SW_RPS 
		memset(skb, 0, offsetof(struct sk_buff, pAd));
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 4, 0)
		skb->fe_resource_mark = 0;
#endif
#else
		memset(skb, 0, offsetof(struct sk_buff, tail));
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		atomic_set(&skb->users, 1);
#else
		refcount_set(&skb->users, 1);
#endif
		skb->head = data;
		skb->data = data;
		skb_reset_tail_pointer(skb);
		skb->end = skb->tail + size;
		skb->mac_header = (typeof(skb->mac_header))~0U;
		skb->transport_header = (typeof(skb->transport_header))~0U;

		/* make sure we initialize shinfo sequentially */
		shinfo = skb_shinfo(skb);
		memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
		atomic_set(&shinfo->dataref, 1);

		skb->skb_recycling_callback = skbmgr_4k_recycling_callback;
		skb->skb_recycling_ind = SKBMGR_4K_INDICATION;
#endif


		skb_queue_head(list, skb);

		return 1;
	}

	return 0;
}

EXPORT_SYMBOL(skbmgr_4k_recycling_callback);

#if defined(TCSUPPORT_LRO_ENABLE)

#define SKBMGR_LRO_LIMIT			1024
#define SKBMGR_LRO_DEF_HOT_LIST_LEN		128


int skbmgr_lro_max_alloc_no = 0;
int skbmgr_lro_alloc_fail = 0;
int skbmgr_lro_alloc_normal = 0;

atomic_t skbmgr_lro_alloc_no;

int skbmgr_lro_hot_list_len = SKBMGR_LRO_DEF_HOT_LIST_LEN;
int skbmgr_lro_max_list_len = 0;
__DMEM int skbmgr_lro_limit = SKBMGR_LRO_LIMIT;
union {
	struct sk_buff_head	list;
	char			pad[SMP_CACHE_BYTES];
} skbmgr_lro_pool[SKBMGR_MAX_QUEUE];

__IMEM struct sk_buff *skbmgr_alloc_skb_lro(void)
{
	struct sk_buff_head *list;
	struct sk_buff *skb;
	int alloc_no;
	
	list = &skbmgr_lro_pool[SKBMGR_QUEUE_ID].list;
	
	if (skb_queue_len(list)) {
		unsigned int size;
		struct skb_shared_info *shinfo;
		u8 *data;

		skb = skb_dequeue(list);

		if (unlikely(skb == NULL))
			goto try_normal;
		
		size = skb->truesize-SKB_DATA_ALIGN(sizeof(struct sk_buff))-SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		data = skb->head;

		/*
		 * Only clear those fields we need to clear, not those that we will
		 * actually initialise below. Hence, don't put any more fields after
		 * the tail pointer in struct sk_buff!
		 */
		memset(skb, 0, offsetof(struct sk_buff, tail));
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		atomic_set(&skb->users, 1);
#else
		refcount_set(&skb->users, 1);
#endif	
		skb->head = data;
		skb->data = data;
		skb_reset_tail_pointer(skb);
		skb->end = skb->tail + size;
		skb->mac_header = (typeof(skb->mac_header))~0U;
		skb->transport_header = (typeof(skb->transport_header))~0U;

		/* make sure we initialize shinfo sequentially */
		shinfo = skb_shinfo(skb);
		memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
		atomic_set(&shinfo->dataref, 1);

		skb->skb_recycling_callback = skbmgr_lro_recycling_callback;
		skb->skb_recycling_ind = SKBMGR_LRO_INDICATION;

		atomic_inc(&skbmgr_lro_alloc_no);
		alloc_no = atomic_read(&skbmgr_lro_alloc_no);
		if (alloc_no > skbmgr_lro_max_alloc_no)
			skbmgr_lro_max_alloc_no = alloc_no;

		return skb;
	}

try_normal:
	if ((atomic_read(&skbmgr_lro_alloc_no) < skbmgr_lro_limit)) {
		skbmgr_lro_alloc_normal++;
		skb = alloc_skb(SKBMGR_LRO_RX_BUF_LEN , GFP_ATOMIC|__GFP_NOWARN);
		if (likely(skb)) {
			skb->skb_recycling_callback = skbmgr_lro_recycling_callback;
			skb->skb_recycling_ind = SKBMGR_LRO_INDICATION;
			if(0 == skbmgr_lro_truesize)
			{
				skbmgr_lro_truesize = skb->truesize;
			}
			atomic_inc(&skbmgr_lro_alloc_no);
			alloc_no = atomic_read(&skbmgr_lro_alloc_no);
			if (alloc_no > skbmgr_lro_max_alloc_no)
				skbmgr_lro_max_alloc_no = alloc_no;
		} else {
			skbmgr_lro_alloc_fail++;
		}
	} else {
		skb = NULL;
		skbmgr_lro_alloc_fail++;
	}
	return skb;
}

EXPORT_SYMBOL(skbmgr_alloc_skb_lro);

__IMEM int skbmgr_lro_recycling_callback(struct sk_buff *skb)
{
	struct sk_buff_head *list;

	list = &skbmgr_lro_pool[SKBMGR_QUEUE_ID].list;	

	if (skb_queue_len(list) < skbmgr_lro_hot_list_len) {
		if ((skbmgr_lro_truesize != skb->truesize) ||
			(skb->fclone != SKB_FCLONE_UNAVAILABLE) ||
			skb_cloned(skb) ||
			(skb_shinfo(skb)->nr_frags) || 
			skb_has_frag_list(skb)) {
			return 0;
		}

#if defined(TCSUPPORT_RA_HWNAT)
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
#endif
		if (skb_queue_len(list) > skbmgr_lro_max_list_len)
			skbmgr_lro_max_list_len = skb_queue_len(list) + 1;

		skb_queue_head(list, skb);

		return 1;
	}

	return 0;
}

EXPORT_SYMBOL(skbmgr_lro_recycling_callback);
#endif

#endif

#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)

#define SKBMGR_SG_RX_BUF_LEN 			SKB_DATA_ALIGN(128+NET_SKB_PAD)

__DMEM int skbmgr_sg_max_list_len = 0;

__DMEM union {
	struct sk_buff_head     list;
	char                    pad[SMP_CACHE_BYTES];
} skbmgr_sg_pool[NR_CPUS];

struct sk_buff *skbmgr_alloc_skb128(void)
{
	struct sk_buff_head *list = &skbmgr_sg_pool[smp_processor_id()].list;
	struct sk_buff *skb;

	if (skb_queue_len(list)) {
		unsigned long flags;
		unsigned int size;
		struct skb_shared_info *shinfo;
		u8 *data;

 		local_irq_save(flags);
		skb = __skb_dequeue(list);
		local_irq_restore(flags);

		if (unlikely(skb == NULL))
			goto try_normal;
		
		size = skb->truesize-SKB_DATA_ALIGN(sizeof(struct sk_buff))-SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
		data = skb->head;

		/*
		 * Only clear those fields we need to clear, not those that we will
		 * actually initialise below. Hence, don't put any more fields after
		 * the tail pointer in struct sk_buff!
		 */
		memset(skb, 0, offsetof(struct sk_buff, tail));
		atomic_set(&skb->users, 1);
		skb->head = data;
		skb->data = data;
		skb_reset_tail_pointer(skb);
		skb->end = skb->tail + size;
		skb->mac_header = (typeof(skb->mac_header))~0U;
		skb->transport_header = (typeof(skb->transport_header))~0U;

		/* make sure we initialize shinfo sequentially */
		shinfo = skb_shinfo(skb);
		memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
		atomic_set(&shinfo->dataref, 1);

		skb->skb_recycling_callback = skbmgr_sg_recycling_callback;

		return skb;
	}

try_normal:
	skb = alloc_skb(SKBMGR_SG_RX_BUF_LEN, GFP_ATOMIC|__GFP_NOWARN);
	if (likely(skb)) 
		skb->skb_recycling_callback = skbmgr_sg_recycling_callback;
	return skb;
}

EXPORT_SYMBOL(skbmgr_alloc_skb128);
int skbmgr_sg_recycling_callback(struct sk_buff *skb)
{
	struct sk_buff_head *list = &skbmgr_sg_pool[smp_processor_id()].list;

	if (skb_queue_len(list) < skbmgr_hot_list_len) {
		unsigned long flags;

		if (/*(skb->truesize - sizeof(struct sk_buff) != SKBMGR_SG_RX_BUF_LEN) ||*/
			(skb_shinfo(skb)->nr_frags) ||
			(skb_shinfo(skb)->frag_list)) {
			return 0;
		}
		
#if defined(TCSUPPORT_HWNAT)		
		pktflow_free(skb);		
#endif
#ifdef TCSUPPORT_RA_HWNAT
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
#endif

		if (skb_queue_len(list) > skbmgr_sg_max_list_len)
			skbmgr_sg_max_list_len = skb_queue_len(list) + 1;

		local_irq_save(flags);
		__skb_queue_head(list, skb);
		local_irq_restore(flags);

		return 1;
	}

	return 0;
}

EXPORT_SYMBOL(skbmgr_sg_recycling_callback);
#endif //xflu

void skbmgr_free_all_skbs(void)
{
	struct sk_buff_head *list;
	struct sk_buff *skb;
	int i;

	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_pool[i].list;
		while ((skb = skb_dequeue(list)) != NULL) {
			skb->skb_recycling_callback = NULL;
			if (skb->skb_recycling_ind == SKBMGR_INDICATION)
				atomic_dec(&skbmgr_alloc_no);
			skb->skb_recycling_ind = 0;
			skb_release_data(skb);
			kfree_skbmem(skb);
		}
	}
#if 0
	//do not need free 4k poll here. shnwind.
	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_4k_pool[i].list;
		while ((skb = skb_dequeue(list)) != NULL) {
			skb->skb_recycling_callback = NULL;
			if (skb->skb_recycling_ind  == SKBMGR_4K_INDICATION)
				atomic_dec(&skbmgr_4k_alloc_no);
			skb->skb_recycling_ind = 0;
			skb_release_data(skb);
			kfree_skbmem(skb);
		}
	}
#endif

#ifdef SKBMGR_SINGLE_QUEUE
	list = &skbmgr_4k_pool[0].list;
	while ((skb = skb_dequeue(list)) != NULL) {
		skb->skb_recycling_callback = NULL;
		if (skb->skb_recycling_ind == SKBMGR_4K_INDICATION)
			atomic_dec(&skbmgr_4k_alloc_no);
		skb->skb_recycling_ind = 0;
		kfree_skbmem(skb);
	}
#else
	for (i=0; i<NR_CPUS; i++) {
		list = &skbmgr_4k_pool[i].list;
		while ((skb = skb_dequeue(list)) != NULL) {
			skb->skb_recycling_callback = NULL;
			if (skb->skb_recycling_ind  == SKBMGR_4K_INDICATION)
				atomic_dec(&skbmgr_4k_alloc_no);
			skb->skb_recycling_ind = 0;
			kfree_skbmem(skb);
		}
	}
#endif

#if defined(TCSUPPORT_LRO_ENABLE)
#ifdef SKBMGR_SINGLE_QUEUE
	list = &skbmgr_lro_pool[0].list;
	while ((skb = skb_dequeue(list)) != NULL) {
		skb->skb_recycling_callback = NULL;
		if (skb->skb_recycling_ind == SKBMGR_LRO_INDICATION)
			atomic_dec(&skbmgr_lro_alloc_no);
		skb->skb_recycling_ind = 0;
		kfree_skbmem(skb);
	}
#else
	for (i=0; i<NR_CPUS; i++) {
		list = &skbmgr_lro_pool[i].list;
		while ((skb = skb_dequeue(list)) != NULL) {
			skb->skb_recycling_callback = NULL;
			if (skb->skb_recycling_ind  == SKBMGR_LRO_INDICATION)
				atomic_dec(&skbmgr_lro_alloc_no);
			skb->skb_recycling_ind = 0;
			kfree_skbmem(skb);
		}
	}
#endif
#endif

#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)
	for (i=0; i<NR_CPUS; i++) {
		list = &skbmgr_sg_pool[i].list;
		while ((skb = skb_dequeue(list)) != NULL) {
			skb->skb_recycling_callback = NULL;
			kfree_skbmem(skb);
		}
	}
#endif
}

static int hot_list_len_read(char *page, char **start, off_t offset,
			    int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "skbmgr_hot_list_len %d skbmgr_4k_hot_list_len %d\n", skbmgr_hot_list_len,skbmgr_4k_hot_list_len);

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

static int hot_list_len_write(struct file *file, const char __user * buffer,
			     unsigned long count, void *data)
{
	char buf[64];
	int val;
	int val2=0;
	int ret=0;

	if (count >= 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	buf[count]='\0';
	
	ret=sscanf(buf,"%d %d", &val,&val2);
	if(ret==1){
		skbmgr_hot_list_len = val;
	}else if(ret==2){
		skbmgr_hot_list_len = val;
		if(val2!=0)
			skbmgr_4k_hot_list_len = val2;
	}
	
	if (skbmgr_hot_list_len == 0) {
		skbmgr_free_all_skbs();
		skbmgr_max_list_len = 0;
#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)
		skbmgr_sg_max_list_len = 0;
#endif
	}

	return count;
}

/*add proc function,user can change max_skb_num value */
static int driver_max_skb_read(char *page, char **start, off_t offset,
		int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "%d (%d,%d)\n", g_max_skb_num, atomic_read(&g_used_skb_num), peak_skb_num);

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

static int driver_max_skb_write(struct file *file, const char __user * buffer,
			unsigned long count, void *data)
{
	char buf[64];
	int val;

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	val = simple_strtoul(buf, NULL, 10);

	g_max_skb_num = val;

	return count;
}

static int limit_read(char *page, char **start, off_t offset,
			int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "%d\n", skbmgr_limit);

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

static int limit_write(struct file *file, const char __user * buffer,
			     unsigned long count, void *data)
{
	char buf[64];
	int val;
#ifdef TCSUPPORT_TEST_VWTEST
	struct sk_buff_head *list;
	struct sk_buff *skb;
	int i;
#endif

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	val = simple_strtoul(buf, NULL, 10);

	skbmgr_limit = val;
	if (skbmgr_hot_list_len == 0) {
		skbmgr_free_all_skbs();
		skbmgr_max_list_len = 0;
#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)
		skbmgr_sg_max_list_len = 0;
#endif
	}
#ifdef TCSUPPORT_TEST_VWTEST
	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_pool[i].list;
		while ((skb = skb_dequeue(list)) != NULL) {
			skb_release_data(skb);
			kfree_skbmem(skb);
		}
	}	
#endif
	return count;
}



static int limit_4k_read(char *page, char **start, off_t offset,
			int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "%d\n", skbmgr_4k_limit);

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

static int limit_4k_write(struct file *file, const char __user * buffer,
			     unsigned long count, void *data)
{
	char buf[64];
	int val;
	struct sk_buff_head *list;
	int i;
	struct sk_buff *skb;

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	val = simple_strtoul(buf, NULL, 10);

	skbmgr_4k_limit = val;
	if (skbmgr_hot_list_len == 0) {
		skbmgr_free_all_skbs();
		skbmgr_max_list_len = 0;
#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)
		skbmgr_sg_max_list_len = 0;
#endif
	}
#ifdef TCSUPPORT_TEST_VWTEST
#ifdef SKBMGR_SINGLE_QUEUE
	list = &skbmgr_4k_pool[0].list;
	while ((skb = skb_dequeue(list)) != NULL) {
		skb_release_data(skb);
		kfree_skbmem(skb);
	}
#endif	
#endif	
	return count;
}
#ifdef TCSUPPORT_TEST_VWTEST
static int skbmgr_info_write(struct file *file, const char __user * buffer,
				 unsigned long count, void *data)
{
	char buf[64];

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	skbmgr_max_alloc_no = 0;
	skbmgr_alloc_fail = 0;
	skbmgr_alloc_normal = 0;
	
	skbmgr_4k_max_alloc_no = 0;
	skbmgr_4k_alloc_fail = 0;
	skbmgr_4k_alloc_normal = 0;
	
	return count;
}
#endif
static int skbmgr_info_read(char *page, char **start, off_t offset,
			    int count, int *eof, void *data)
{
	char *out = page;
	int len;
	struct sk_buff_head *list;
	int i;

	out += sprintf(out, "skbmgr_limit = %d\n", skbmgr_limit);
	out += sprintf(out, "skbmgr_max_alloc_no = %d\n", skbmgr_max_alloc_no);
	out += sprintf(out, "skbmgr_alloc_fail = %d\n", skbmgr_alloc_fail);
	out += sprintf(out, "skbmgr_alloc_no = %d\n", atomic_read(&skbmgr_alloc_no));
	out += sprintf(out, "skbmgr_max_list_len = %d\n", skbmgr_max_list_len);
	out += sprintf(out, "skbmgr_alloc_normal = %d\n", skbmgr_alloc_normal);
	out += sprintf(out, "skbmgr_4k_limit = %d\n", skbmgr_4k_limit);
	out += sprintf(out, "skbmgr_4k_max_alloc_no = %d\n", skbmgr_4k_max_alloc_no);
	out += sprintf(out, "skbmgr_4k_alloc_fail = %d\n", skbmgr_4k_alloc_fail);
	out += sprintf(out, "skbmgr_4k_alloc_no = %d\n", atomic_read(&skbmgr_4k_alloc_no));
	out += sprintf(out, "skbmgr_4k_max_list_len = %d\n", skbmgr_4k_max_list_len);
	out += sprintf(out, "skbmgr_4k_alloc_normal = %d\n", skbmgr_4k_alloc_normal);
	out += sprintf(out, "skbmgr_2k_truesize = %d\n",  skbmgr_2k_truesize);
	out += sprintf(out, "skbmgr_4k_truesize = %d\n",  skbmgr_4k_truesize);
	out += sprintf(out, "skbmgr_lro_truesize = %d\n", skbmgr_lro_truesize);
	
#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)
	out += sprintf(out, "skbmgr_sg_max_list_len = %d\n", skbmgr_sg_max_list_len);
#endif
#if defined(TCSUPPORT_LRO_ENABLE)
	out += sprintf(out, "skbmgr_lro_limit = %d\n", skbmgr_lro_limit);
	out += sprintf(out, "skbmgr_lro_max_alloc_no = %d\n", skbmgr_lro_max_alloc_no);
	out += sprintf(out, "skbmgr_lro_alloc_fail = %d\n", skbmgr_lro_alloc_fail);
	out += sprintf(out, "skbmgr_lro_alloc_no = %d\n", atomic_read(&skbmgr_lro_alloc_no));
	out += sprintf(out, "skbmgr_lro_max_list_len = %d\n", skbmgr_lro_max_list_len);
	out += sprintf(out, "skbmgr_lro_alloc_normal = %d\n", skbmgr_lro_alloc_normal);
#endif

	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_pool[i].list;
		out += sprintf(out, "skbmgr_queue_len CPU%d = %d\n", i, skb_queue_len(list));
	}	

	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_4k_pool[i].list;
		out += sprintf(out, "skbmgr_4k_queue_len CPU%d = %d\n", i, skb_queue_len(list));
	}	
#if defined(TCSUPPORT_LRO_ENABLE)
	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_lro_pool[i].list;
		out += sprintf(out, "skbmgr_lro_queue_len CPU%d = %d\n", i, skb_queue_len(list));
	}	
#endif
#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)
	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		list = &skbmgr_sg_pool[i].list;
		out += sprintf(out, "skbmgr_sg_queue_len CPU%d = %d\n", i, skb_queue_len(list));
	}	
#endif

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

#if defined(TCSUPPORT_MEMORY_CONTROL) || defined(TCSUPPORT_CT)
int auto_clear_cache_flag = 0;
int auto_kill_process_flag = 0;
static int auto_clear_cache_read(char *page, char **start, off_t offset,
			    int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "%d\n", auto_clear_cache_flag);

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

static int auto_clear_cache_write(struct file *file, const char __user * buffer,
			     unsigned long count, void *data)
{
	char buf[64];
	int val;

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	val = simple_strtoul(buf, NULL, 10);

	auto_clear_cache_flag = val;

	return count;
}

static int auto_kill_process_read(char *page, char **start, off_t offset,
			    int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "%d\n", auto_kill_process_flag);

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

static int auto_kill_process_write(struct file *file, const char __user * buffer,
			     unsigned long count, void *data)
{
	char buf[64];
	int val;

	if (count > 64)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	val = simple_strtoul(buf, NULL, 10);

	auto_kill_process_flag = val;

	return count;
}
#endif
int register_proc_skbmgr(void)
{
	struct proc_dir_entry *p;

	p = create_proc_entry("skbmgr_hot_list_len", 0644, init_net.proc_net);
	if (!p) 
		return 0;

	p->read_proc = hot_list_len_read;
	p->write_proc = hot_list_len_write;

#ifdef TCSUPPORT_TEST_VWTEST
	p = create_proc_entry("skbmgr_info", 0644, init_net.proc_net);
	if (!p) 
		return 0;
	p->read_proc = skbmgr_info_read;
	p->write_proc = skbmgr_info_write;
#else
	p = create_proc_read_entry("skbmgr_info", 0, init_net.proc_net, skbmgr_info_read, NULL);
	if (!p) 
		return 0;
#endif
	p = create_proc_entry("skbmgr_limit", 0644, init_net.proc_net);
	if (!p) 
		return 0;

	p->read_proc = limit_read;
	p->write_proc = limit_write;

	p = create_proc_entry("skbmgr_4k_limit", 0644, init_net.proc_net);
	if (!p) 
		return 0;
	p->read_proc = limit_4k_read;
	p->write_proc = limit_4k_write;


	p = create_proc_entry("skbmgr_driver_max_skb", 0644, init_net.proc_net);
	if (!p) 
		return 0;

	p->read_proc = driver_max_skb_read;
	p->write_proc = driver_max_skb_write;

#if defined(TCSUPPORT_MEMORY_CONTROL) || defined(TCSUPPORT_CT)
	p = create_proc_entry("auto_clear_cache", 0644, init_net.proc_net);
	if (!p) 
		return 0;

	p->read_proc = auto_clear_cache_read;
	p->write_proc = auto_clear_cache_write;

	p = create_proc_entry("auto_kill_process", 0644, init_net.proc_net);
	if (!p) 
		return 0;

	p->read_proc = auto_kill_process_read;
	p->write_proc = auto_kill_process_write;
#endif
	return 1;
}

static void unregister_proc_skbmgr(void)
{
	remove_proc_entry("skbmgr_hot_list_len", init_net.proc_net);
	remove_proc_entry("skbmgr_info", init_net.proc_net);
	remove_proc_entry("skbmgr_limit", init_net.proc_net);
	remove_proc_entry("skbmgr_driver_max_skb", init_net.proc_net);
#if defined(TCSUPPORT_MEMORY_CONTROL) || defined(TCSUPPORT_CT)
	remove_proc_entry("auto_clear_cache", init_net.proc_net);
	remove_proc_entry("auto_kill_process", init_net.proc_net);
#endif
}

#if defined(TCSUPPORT_LRO_ENABLE)
void skbmgr_lro_pool_init(void){
	int i;
	
	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		skb_queue_head_init(&skbmgr_lro_pool[i].list);
	}	
}
#endif

void skbmgr_4k_pool_init(void){
	int i;
	
	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
		skb_queue_head_init(&skbmgr_4k_pool[i].list);
	}	
}

void skbmgr_pool_init(void){
	int i;
	for (i=0; i<SKBMGR_MAX_QUEUE; i++) {
			skb_queue_head_init(&skbmgr_pool[i].list);
	}	
}

void ecnt_skb_init(void){
	int i;
	
	atomic_set(&skbmgr_alloc_no, 0);
	atomic_set(&g_used_skb_num, 0);

	skbmgr_pool_init();
	skbmgr_4k_pool_init();
#if defined(TCSUPPORT_LRO_ENABLE)
	skbmgr_lro_pool_init();
#endif
	
#if defined(TC3262_GMAC_SG_MODE) || defined(TC3262_PTM_SG_MODE)
	for (i=0; i<NR_CPUS; i++) {
		skb_queue_head_init(&skbmgr_sg_pool[i].list);
	}
#endif

	register_proc_skbmgr();
}
EXPORT_SYMBOL(ecnt_skb_init);
