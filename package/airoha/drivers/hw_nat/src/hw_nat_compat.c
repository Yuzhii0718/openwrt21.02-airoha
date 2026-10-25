/*
 * Compatibility shims for hw_nat symbols that this product does not get from
 * a vendor module.
 *
 * 1) Optional features whose providers (soft-ratelimit, TR471 speedtest) are
 *    not shipped here; the old closed blob did not reference them either.
 *      wifiqueue_start_idx                -1    -> no per-SSID wifi queue
 *      soft_ratelimit_allocatequeue_hook  NULL  -> allocation skipped
 *      wan_tr471_pinpong_handle_hook      NULL  -> handler skipped
 * 2) FOE helpers that only the newer (aarch64) vendor objects define; the
 *    ARMv7 objects for this SoC predate them:
 *      ppe_clean_table_flag/timer 0 -> ppe_clean_table_timer_check() no-ops
 *      set_foe_SMA_mode()          no-op
 *      FoeGetEntries()             reports zero entries (debug dump only)
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/skbuff.h>

#include "hwnat_ioctl.h"

int wifiqueue_start_idx = -1;

int (*soft_ratelimit_allocatequeue_hook)(unsigned int num) = NULL;

/*
 * soft_rate_limit.ko (shipped as package/airoha/drivers/soft_ratelimit) and
 * npu's hostadpt both reference this pointer, and in the vendor BSP it is
 * hw_nat that owns it. Export it so the prebuilt module loads; it stays NULL
 * until soft_ratelimit registers its queue check, so nothing changes when that
 * module is not loaded.
 */
int (*soft_ratelimit_check_queue_hook)(unsigned int queue_idx) = NULL;
EXPORT_SYMBOL(soft_ratelimit_check_queue_hook);

int (*wan_tr471_pinpong_handle_hook)(struct sk_buff *skb) = NULL;

unsigned char ppe_clean_table_flag = 0;

unsigned int ppe_clean_table_timer = 0;

/*
 * Hook pointers that the newer vendor objects define and hw_nat assigns at
 * init. Defined here as NULL so the assignments link; the consumers are
 * modules this product does not ship (wifi queue / XSI / SFU / OOKLA / sw_nat).
 */
struct net_data_s;

int (*ra_sw_nat_hook_clean_entry_by_src_mac)(struct net_data_s *net_data) = NULL;
int (*hwnat_clean_xsi_lan_hook)(unsigned int hsgmii_index, unsigned int channel) = NULL;
int (*hwnat_multicast_set_info_for_sfu_hook)(int index, int tag) = NULL;
int (*getWifiQueueIndex_hook)(struct net_device *dev, unsigned char direction) = NULL;
int (*ookla_offload_rcv_hook)(struct sk_buff *skb) = NULL;
int (*ookla_offload_pingpong_hook)(struct sk_buff *skb) = NULL;

void set_foe_SMA_mode(unsigned int mode)
{
	/* only reached when ppe_clean_table_flag == 1, which is never set */
}

int FoeGetEntries(struct hwnat_entries_args *opt)
{
	/* the FOE entry table lives in the newer vendor objects */
	if (opt)
		opt->num_of_entries = 0;

	return 0;
}
