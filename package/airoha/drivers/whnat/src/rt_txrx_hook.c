#include "rt_linux_txrx_hook.h"
#include "link_list.h"
struct mt_wlan_hook_ctrl wlan_hook;
VOID os_zero_mem(
	PVOID ptr,
	ULONG length)
{
	memset(ptr, 0, length);
}


/*
*
*/
void mt_wlan_hook_init(void)
{
	os_zero_mem(&wlan_hook, sizeof(struct mt_wlan_hook_ctrl));
	DlListInit(&wlan_hook.hook_head);
}
EXPORT_SYMBOL(mt_wlan_hook_init);

/*
*
*/
int mt_wlan_hook_call(unsigned short hook, void *ad, void *priv)
{
	struct mt_wlan_hook_ops *cur;
	cur = DlListEntry((&wlan_hook.hook_head)->Next, struct mt_wlan_hook_ops, list);
	if (cur) {
		DlListForEach(cur, &wlan_hook.hook_head, struct mt_wlan_hook_ops, list)
		if (cur->hooks & 1 << hook)
			cur->fun(hook, ad, priv);
		
	}
	return 0;
}
EXPORT_SYMBOL(mt_wlan_hook_call);

/*
*
*/
int mt_wlan_hook_register(struct mt_wlan_hook_ops *ops)
{
	struct mt_wlan_hook_ops *next = NULL;
	struct mt_wlan_hook_ops *cur = NULL;
	printk("%s:%d !!!!!\n",__func__,__LINE__);
	
	if (!ops)
		return -1;
	printk("%s:%d !!!!!\n",__func__,__LINE__);
	cur = DlListEntry((&wlan_hook.hook_head)->Next, struct mt_wlan_hook_ops, list);
	if (cur) {
		printk("%s:%d ops_priority=%d cur_priority=%d !!!!!\n",__func__,__LINE__,ops->priority,cur->priority);
		DlListForEachSafe(cur, next, &wlan_hook.hook_head,
						  struct mt_wlan_hook_ops, list)

		if (cur && ops->priority > cur->priority) {
			(ops->list).Next = &next->list;
			cur->list.Next = &ops->list;
			return 0;
		}

		/*add first entry*/
		DlListAdd(&wlan_hook.hook_head, &ops->list);
	}
	printk("%s:%d !!!!!\n",__func__,__LINE__);
	return 0;
}
EXPORT_SYMBOL(mt_wlan_hook_register);


/*
*
*/
int mt_wlan_hook_unregister(struct mt_wlan_hook_ops *ops)
{
	struct mt_wlan_hook_ops *next, *cur;

	if (!ops)
		return -1;

	DlListForEachSafe(cur, next, &wlan_hook.hook_head,
					  struct mt_wlan_hook_ops, list)

	if (ops == cur) {
		DlListDel(&ops->list);
		return 0;
	}

	return -1;
}
EXPORT_SYMBOL(mt_wlan_hook_unregister);

