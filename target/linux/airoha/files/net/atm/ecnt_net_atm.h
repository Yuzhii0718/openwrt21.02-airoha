#ifndef _LINUX_ECNT_NET_CORE_H
#define _LINUX_ECNT_NET_CORE_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <linux/ip.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <net/arp.h>
#include <linux/atm.h>
#include <linux/atmdev.h>
#include <linux/capability.h>
#include <linux/seq_file.h>

#include <linux/atmbr2684.h>

#include <ecnt_hook/ecnt_hook.h>
#include "common.h"


#if !defined(TCSUPPORT_CPU_MT7510) && !defined(TCSUPPORT_CPU_MT7505) && !defined(TCSUPPORT_CPU_EN7512)
#ifdef TCSUPPORT_RA_HWNAT
#include <linux/foe_hook.h>
#endif
#endif

#define MIN_PKT_SIZE     60
extern int napi_en;
extern void (*br2684_config_hook)(int linkMode, int linkType);
extern int (*br2684_init_hook)(struct atm_vcc *atmvcc, int encaps);
extern int (*br2684_push_hook)(struct atm_vcc *atmvcc, struct sk_buff *skb);
extern int (*br2684_xmit_hook)(struct sk_buff *skb, struct net_device *dev, int encaps);
extern void (*pppoatm_config_hook)(int linkMode, int linkType);
extern int (*pppoatm_init_hook)(struct atm_vcc *atmvcc, int encaps);
extern int (*pppoatm_push_hook)(struct atm_vcc *atmvcc, struct sk_buff *skb);

#if defined(TCSUPPORT_CT_SWQOS)
extern int (*sw_pktqosEnqueue) (struct sk_buff * bp);
#endif

#if defined(TCSUPPORT_CT)
extern int (*do_mulif_interface_unregister_hook)(char* dev_name);
#else
extern int (*check_smuxIf_exist_hook)(struct net_device *dev);
#endif

#if defined(TCSUPPORT_CT)
#ifdef TCSUPPORT_BRIDGE_FASTPATH

int (*hook_bridge_shortcut_process)(struct net_device *net_dev, struct sk_buff *skb);
EXPORT_SYMBOL(hook_bridge_shortcut_process);
#endif
#endif

static inline int ecnt_br2684_xmit_vcc_if_inline_hook(struct sk_buff *skb, struct net_device *dev, int encaps, struct atm_vcc *atmvcc)
{
	int err = 0;
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
	if (br2684_xmit_hook){
		err = br2684_xmit_hook(skb, dev, encaps);
		if (err){
			return ECNT_RETURN;
		}
		else 
			return ECNT_CONTINUE;
	}
	else 
		return ECNT_RETURN_DROP;
#endif
	if(atmvcc->send == NULL)
	{
		printk("\r\n[br2684_xmit_vcc]++++atmvcc->send == NULL++++");
		dev_kfree_skb(skb);
		return ECNT_RETURN;
	}

	return ECNT_RETURN_DROP;
}

static inline int ecnt_br2684_start_xmit_padpkt_inline_hook(struct sk_buff **skb, int *copies_failed, rwlock_t *devs_lock, struct net_device *dev){
	struct sk_buff *skb2;
	/*if the packet length < 60, pad upto 60 bytes. shnwind 2008.4.17*/
	
#if defined(TCSUPPORT_CT_SWQOS)
	if(sw_pktqosEnqueue){
		(*skb)->dev = dev;
		if(sw_pktqosEnqueue(*skb) == 1)
			return ECNT_RETURN_DROP;
	}
#endif

#if !defined(TCSUPPORT_HOST_SERVICE_MANAGER)
	if ((*skb)->len < MIN_PKT_SIZE)
	{
		skb2=skb_copy_expand(*skb, 0, MIN_PKT_SIZE - (*skb)->len, GFP_ATOMIC);
		dev_kfree_skb(*skb);
		if (skb2 == NULL) {
			*copies_failed=*copies_failed+1;
			read_unlock(devs_lock);
			return ECNT_RETURN_DROP;
			
		}
		*skb = skb2;		
		memset((*skb)->tail, 0, MIN_PKT_SIZE - (*skb)->len);		
		skb_put((*skb), MIN_PKT_SIZE - (*skb)->len);
	}
#endif
	return ECNT_CONTINUE;
}

static inline void ecnt_br2684_push_destroy_inline_hook(struct atm_vcc *atmvcc, struct net_device *net_dev)
{
#if defined(CONFIG_CPU_TC3162) || defined(CONFIG_MIPS_TC3262)
#ifdef CONFIG_SMUX
#if !defined(TCSUPPORT_CT)
	unsigned char ifNum = 0;
#endif
#endif

#ifdef CONFIG_SMUX
#if defined(TCSUPPORT_CT)
	if(do_mulif_interface_unregister_hook != NULL) {
		
		do_mulif_interface_unregister_hook(net_dev->name);
		printk("\n==> Unregister all interfaces belong to PVC %s\n", net_dev->name);
	}
#else
	if(check_smuxIf_exist_hook != NULL) {
		if((ifNum = check_smuxIf_exist_hook(net_dev)) > 0) {
			printk("\n==> Exist %d smux interfaces, just return and do not close PVC\n", ifNum);
			return;//If smux interface exist, just return and do not close PVC
		}
	}
#endif
#endif
#endif	
}

static inline int ecnt_br2684_push_skb_trim_inline_hook(struct sk_buff *skb){
	if (skb->data[7] == 0x01)
		__skb_trim(skb, skb->len - 4);
	return ECNT_CONTINUE;
}


static inline int ecnt_br2684_setup_routed_flags_inline_hook(struct net_device *netdev)
{
	/*release note TLM7.3.24.0-9
	Symptom: When Wan PVC is IPoA mode, the RIP function is failed.
	Reason: IPoA mode PVC is created with Point-to-Point Flag.	
	Solution:Remove Point-to-Point Flag in br2684_setup_routed().
	*/
	netdev->flags = IFF_NOARP | IFF_MULTICAST;
	return ECNT_CONTINUE;
}

static inline int ecnt_br2684_create_config_inline_hook(int payload){
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
		if (br2684_config_hook){
			br2684_config_hook(payload, 0);
		} else {
			printk("br2684_config_hook function: (NULL)\n");
		}
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_pppoatm_devppp_ioctl_config_inline_hook(void)
{
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
	if (pppoatm_config_hook){
		// choose router mode & pppoa type
		pppoatm_config_hook(0, 1);
	}
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_br2684_push_hwnat_inline_hook(struct sk_buff *skb){
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
    if (napi_en)
    {
        netif_receive_skb(skb);
		return ECNT_CONTINUE;
    } 
#endif
	return ECNT_RETURN_DROP;

}

static inline int ecnt_br2684_regvcc_init_inline_hook(struct atm_vcc *atmvcc, int encaps, int *err)
{
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
	if (br2684_init_hook){
		*err = br2684_init_hook(atmvcc, encaps);
		if (*err){
			printk("br2684_init_hook: error detected\n");
			return ECNT_RETURN;
			//return err;
		} else {
			printk("br2684_init_hook: success\n");
		}
	} else {
		printk("br2684_init_hook function: (NULL)\n");
	}
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_br2684_push_hook_inline_hook(struct atm_vcc *atmvcc, struct sk_buff *skb){	
	int err;
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
		// hardware handle mpoa header
		if (br2684_push_hook){
			err = br2684_push_hook(atmvcc, skb);
			if (err){
				//goto error;
				return ECNT_RETURN;
			}	
			else
				return ECNT_CONTINUE;
		} 
		// soft handle mpoa header
		
#endif
	return ECNT_RETURN_DROP;
}

static inline int ecnt_pppoatm_push_hook_inline_hook(struct atm_vcc *atmvcc, struct sk_buff *skb)
{
 	int ret = 0;
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
	if (pppoatm_push_hook){
		ret = pppoatm_push_hook(atmvcc, skb);
		if (ret == -1){
			return ECNT_HOOK_ERROR;
			//goto error;
		} else if (ret == -2){
			return ECNT_RETURN;
			//return;
		}
		return ECNT_CONTINUE;
	}
	else 
			return ECNT_RETURN_DROP;
#endif
	return ECNT_RETURN_DROP;
}

static inline int ecnt_pppoatm_assign_vcc_init_inline_hook(struct atm_vcc *atmvcc, int encaps, int *err){
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_HOST_SERVICE_MANAGER)
		if (pppoatm_init_hook){
			*err = pppoatm_init_hook(atmvcc, (encaps-1));
			if (*err){
				printk("pppoatm_init_hook: error detected\n");
				return ECNT_RETURN;
				//return err;
			} else {
				printk("pppoatm_init_hook: success\n");
			}
		} else {
			printk("pppoatm_init_hook function: (NULL)\n");
		}
#endif
	return ECNT_CONTINUE;
}


#endif

static inline int ecnt_pppoatm_may_send_pppoa_inline_hook(void){
#if !defined(TCSUPPORT_PPPOA_ENHANCE)		
	return ECNT_CONTINUE;
#else
	return ECNT_RETURN_DROP;
#endif
}

