#ifndef _LINUX_ECNT_VLAN_H
#define _LINUX_ECNT_VLAN_H
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/if_vlan.h>
#include <linux/netpoll.h>
#include <linux/export.h>
#include <linux/version.h>
#include <ecnt_hook/ecnt_hook.h>
/* for vlan_find_dev(), used by ecnt_vlan_find_dev() below */
#include "vlan.h"

#if defined(TCSUPPORT_PON_VLAN) 
extern int (*pon_vlan_get_mode_hook)(void);
#endif

#ifdef TCSUPPORT_PON_SFU_HGU_HYBRID
extern int (*pon_hybrid_sfu_wan_check_hook)(struct sk_buff **pskb);
#endif

#if defined(TCSUPPORT_CT_VLAN_TAG)
extern unsigned int vtag_free_rx_cnt;
extern int (*check_vtag_ct_hook)(void);
extern int (*store_vtag_ct_hook)(struct sk_buff *skb, struct net_device *dev);
#endif



extern struct sk_buff *skb_reorder_vlan_header(struct sk_buff *skb);

static inline int isBridgeWan(struct net_device *dev)
{
	 if (dev == NULL) 
		 return 0;
 
	 if (rcu_dereference(dev->rx_handler_data) == NULL)
		 return 0;
	 
	 return 1;
}

static inline int ecnt_vlan_receive_inline_hook(struct sk_buff **skbp, __be16 vlan_proto)
{
	struct sk_buff *skb = *skbp;
	unsigned int offset = skb->data - skb_mac_header(skb);
	__u16			vlan_tci;
	
#if defined(TCSUPPORT_PON_VLAN)
	int vlan_mode = MODE_HGU;
	if(pon_vlan_get_mode_hook)
		vlan_mode = pon_vlan_get_mode_hook();
#endif

	
	/*
	 * vlan_insert_tag expect skb->data pointing to mac header.
	 * So change skb->data before calling it and change back to
	 * original position later
	 */
	skb_push(skb, offset);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,115))
	vlan_tci =  skb_vlan_tag_get(skb);
#else
	vlan_tci =  vlan_tx_tag_get(skb);
#endif
	if(skb->vlan_tag_flag&VLAN_TAG_FOR_CFI)
	{
		vlan_tci |= VLAN_CFI_MASK;
	}
	skb = *skbp = vlan_insert_tag(skb, skb->vlan_proto,
					  vlan_tci);

	if (!skb)
		return ECNT_CONTINUE;
	skb->vlan_tci = 0;
	skb->protocol = vlan_proto;
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
	__vlan_hwaccel_clear_tag(skb);
#endif
	skb_pull(skb, offset);
		
#if defined(TCSUPPORT_CT_VLAN_TAG)
	/*remove vlan tag from packet, store vlan tag to skb structure*/
	struct net_device * orig_dev = skb->dev;
	if((orig_dev->name[0] == 'b') || ((orig_dev->name[0] == 'n') && !isBridgeWan(orig_dev))){
		if (check_vtag_ct_hook && (check_vtag_ct_hook() == 1)
			&& store_vtag_ct_hook){
			if( store_vtag_ct_hook(skb, orig_dev)== 0){
				/* Take off the VLAN header (4 bytes currently) */
				skb_pull_rcsum(skb, VLAN_HLEN);
				skb = *skbp = skb_reorder_vlan_header(skb);
				if (unlikely(!skb))
					return ECNT_CONTINUE;
				skb->dev = orig_dev;				
				return ECNT_RETURN; /*return true*/
			}else{
				/* must free skb !! */
				vtag_free_rx_cnt++;
				return ECNT_CONTINUE;
			}
		}
	}
#endif

#ifdef TCSUPPORT_PON_VLAN
	if(vlan_mode == MODE_HGU)
#endif
	{
	}
	return ECNT_CONTINUE;
}

/*
 * Exported for the out-of-tree hw_nat module (reference SDK layout).
 * This header is only included by net/8021q/vlan_core.c, so the inline
 * instantiation and the export land in that translation unit.
 */
static inline struct net_device *ecnt_vlan_find_dev(struct net_device *real_dev, __be16 vlan_proto, u16 vlan_id)
{
	struct net_device *dev = vlan_find_dev(real_dev, vlan_proto, vlan_id);
	return dev;
}
EXPORT_SYMBOL(ecnt_vlan_find_dev);

#ifndef AIROHA_BSP
#include "ecnt_vlan_sdk.h"
#endif/*AIROHA_BSP*/
#endif

