// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
#include <warp.h>
#include <linux/foe_hook.h>


const struct of_device_id warp_of_ids[] = {
	{	.compatible = "en751221,wed", },
	{	.compatible = "en751221,wed2", },
	{	.compatible = "en751221,wed_test", },
	{ },
};


/*
 *
 */
int
wifi_tx_tuple_add(struct wifi_entry *wifi, u8 idx, u8 *tx_info, u32 wdma_rx_port)
{
	struct wlan_tx_info t, *info =  &t;
	struct port_info wifi_info;

	memset(&wifi_info, 0, sizeof(struct port_info));
	memset(info, 0, sizeof(*info));
	wifi->ops->txinfo_wrapper(tx_info, info);

	if (ra_sw_nat_hook_tx) {
		struct sk_buff *skb = (struct sk_buff *)info->pkt;

		if (!info->pkt) {
			warp_dbg(WARP_DBG_INF, "%s():info->pkt is NULL!!!!\n", __func__);
			return -EFAULT;
		}
		if (!skb->head) {
			warp_dbg(WARP_DBG_INF, "%s():skb->head is NULL!!!!\n", __func__);
			return -EFAULT;
		}

		wifi_info.magic = FOE_MAGIC_WLAN_7615;
#if defined(TCSUPPORT_CPU_AN7552)
		wifi_info.nbq = 10 +  (info->ringidx & 0x1);
		wifi_info.channel = 6 + (info->ringidx & 0x1);
#else
#if defined(TCSUPPORT_CPU_EN7580)
		wifi_info.tsid = 0x7f;
#if defined(TCSUPPORT_CPU_EN7523)
		wifi_info.nbq = 0 + ((idx & 0x1) << 1) | (info->ringidx&0x1);
		wifi_info.channel = 6 + (((idx & 0x1) << 1) | (info->ringidx & 0x1));
#else
		wifi_info.nbq = ((idx & 0x1) << 1) | (info->ringidx & 0x1);
		wifi_info.channel = 8 + (((idx & 0x1) << 1) | (info->ringidx & 0x1));
#endif
#endif
#endif
		wifi_info.stag = ((info->wcid & 0xFF) << 6) | (info->bssidx & 0x3F);
		if (ra_sw_nat_hook_tx) {
			if (ra_sw_nat_hook_tx(skb, &wifi_info, wifi_info.magic) == 0)
				wifi->ops->txinfo_set_drop(tx_info);
		}
	} else {
		warp_dbg(WARP_DBG_INF, "%s(): ra_sw_nat_hook_tx is null!\n", __func__);
	}
	return 0;
}

/*
 *
 */
void
wifi_tx_tuple_reset(struct wifi_entry *wifi)
{

}

