#ifndef _LINUX_ECNT_NET_CORE_H
#define _LINUX_ECNT_NET_CORE_H
#include <linux/version.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <linux/kmemcheck.h>
#endif
#include <linux/compiler.h>
#include <linux/time.h>
#include <linux/bug.h>
#include <linux/cache.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
#include <linux/proc_fs.h>
#endif
#include <linux/atomic.h>
#include <asm/types.h>
#include <linux/spinlock.h>
#include <linux/net.h>
#include <linux/textsearch.h>
#include <net/checksum.h>
#include <linux/rcupdate.h>
#include <linux/hrtimer.h>
#include <linux/dma-mapping.h>
#include <linux/netdev_features.h>
#include <linux/sched.h>
#include <linux/socket.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
#include <net/flow_keys.h>
#endif
#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <uapi/linux/ecnt_in.h>
#include <linux/ecnt_vlan_bind.h>

#ifdef TCSUPPORT_IPV6_ENHANCEMENT
#include <net/neighbour.h>
#include <linux/sysctl.h>
#endif
#include <linux/qos_type.h>
#include <asm/tc3162/tc3162.h>
#include <linux/igmp.h>


extern int (*smux_pkt_recv_hook)(struct sk_buff *skb, 
                  struct net_device *dev,
                  struct net_device *rdev);

extern void ecnt_netdev_dest_hook(void);
extern void ecnt_netdev_init_hook(void); 
extern void ecnt_skb_init(void);

#ifdef TCSUPPORT_PORTBIND
extern int (*arht_pbind_vbind_check_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb);
extern int (*arht_pbind_vbind_data_process_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff **skb);
extern void (*arht_netif_recv_portbind_hook)(struct sk_buff *skb,struct net_device *orig_dev);

#define ADD_GROUP		1
#define DEL_GROUP		2
extern void (*portbind_update_hook)(struct net_device *dev, int type);
extern int (*portbind_check_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb);
extern int (*vlanbind_active_hook)(void);
extern int (*vlanbind_entry_active_hook)(int i);
#endif

#if defined(TCSUPPORT_CT_VLAN_BIND) || defined(TCSUPPORT_CMCC) || defined(TCSUPPORT_PORTBIND)
extern struct net_bridge_fdb_entry *get_fdb_by_skb(struct sk_buff *skb);
#endif


#if defined(TCSUPPORT_CFG_NG_UNION)
#if defined(TCSUPPORT_CT_VLAN_TAG)
extern int (*check_vtag_ct_hook)(void);
extern int (*check_vtag_restore_ct_hook)(void);
extern int (*restore_vtag_ct_hook)(struct sk_buff **pskb);
#endif
#endif

#if defined(TCSUPPORT_CT_DS_LIMIT)
extern int (*dslimit_remarkQueue_hook)( struct sk_buff *skb, int up_dw );
#endif

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
extern int (*fe_resource_mark_meter_hook)(struct sk_buff *skb, int dir) ;   
extern int (*fe_resource_mark_acnt_hook)(struct sk_buff *skb, int dir) ;    
#endif
#if defined(TCSUPPORT_CT_APPROUTE)
extern char vpn_if_prefix[32];
#endif

extern int isVoipWan(struct net_device *dev);
extern void (*get_net_device_interface_hw_stats_hook)(struct net_device *dev, struct rtnl_link_stats64 *storage);

extern int (*fe_resource_get_set_wan_idx_hook)( struct sk_buff *skb, u8 dir);
static inline int ecnt_vlan_untag_inline_hook(struct sk_buff *skb, __be16 vlan_tci)
{
	if(vlan_tci&VLAN_CFI_MASK)
		skb->vlan_tag_flag |= VLAN_TAG_FOR_CFI;
	return 0;
}


#ifndef TCSUPPORT_PON_VLAN
static inline int ecnt_skb_original_dev_inline_hook(struct sk_buff **skbp)
{
	struct sk_buff *skb = *skbp;
	if(skb->dev->name[0] != 'b')
		skb->original_dev = skb->dev;
		
	return ECNT_CONTINUE;
}
#endif
#if defined(TCSUPPORT_PON_VLAN) 
extern int (*pon_insert_tag_hook)(struct sk_buff **pskb);
extern int (*pon_check_vlan_hook)(struct net_device *dev, struct sk_buff *skb);
extern int (*pon_vlan_get_mode_hook)(void);

#ifdef TCSUPPORT_PON_SFU_HGU_HYBRID
extern int (*pon_hybrid_sfu_lan_check_hook)(struct sk_buff **pskb);
extern int (*pon_hybrid_sfu_wan_check_hook)(struct sk_buff **pskb);
#endif


static inline int ecnt_ponvlan_rx_inline_hook(struct sk_buff **skbp,struct net_device *orig_dev)
{	
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
		int vlan_mode = MODE_HGU;
		struct sk_buff *skb = *skbp;
		if(pon_vlan_get_mode_hook)
			vlan_mode = pon_vlan_get_mode_hook();

		int retval = ECNT_CONTINUE;
		if(orig_dev->name[0] == 'r' || orig_dev->name[0] == 'u' || orig_dev->name[0] == 'e')
		{
			if(vlan_mode == MODE_SFU)
							skb->pon_vlan_flag |= PON_PKT_FROM_LAN;

#ifdef TCSUPPORT_PON_SFU_HGU_HYBRID
			else if(vlan_mode == MODE_HGU){
				if(pon_hybrid_sfu_lan_check_hook && pon_hybrid_sfu_lan_check_hook(skbp))
					skb->pon_vlan_flag |= PON_PKT_FROM_HYBRID_PPTP;
				else
					skb->pon_vlan_flag |= PON_PKT_FROM_HYBRID_VEIP;
			}
#endif
		}
		else if(strncmp(orig_dev->name,"pon", 3) == 0 && vlan_mode == MODE_SFU)
		{
			skb->pon_vlan_flag |= PON_PKT_FROM_WAN;
		}
#ifdef TCSUPPORT_PON_SFU_HGU_HYBRID
		else if((strncmp(orig_dev->name,"pon", 3) || strncmp(orig_dev->name,"nas", 3))== 0 && 
                        (pon_hybrid_sfu_wan_check_hook && pon_hybrid_sfu_wan_check_hook(skbp)))
		{
			skb->pon_vlan_flag |= PON_PKT_FROM_HYBRID_SFU_WAN; //hybrid mode, sfu port downstream
		}
#endif

		if(skb->dev->name[0] != 'b')
		skb->original_dev = skb->dev;
		if(pon_insert_tag_hook && vlan_mode == MODE_HGU)
		{
			if(skb->pon_vlan_flag & PON_PKT_FROM_HYBRID_PPTP)
				return ECNT_CONTINUE;
			
			if(strcmp(orig_dev->name,"pon") == 0)
				skb->pon_vlan_flag |= PON_VLAN_RX_CALL_HOOK;

#if defined(TCSUPPORT_CMCC)
			if(skb->pon_vlan_flag & PON_PKT_DROP_FLAG){
				if(!isTR069Wan(orig_dev)){
					kfree_skb(skb);
					return ECNT_RETURN_DROP;
				}
			}
#endif
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
			if(pon_insert_tag_hook)
			{
				retval = pon_insert_tag_hook(skbp);
				if(retval == -1)
				{
					kfree_skb(skb);
					return ECNT_RETURN_DROP;
				}
				if(retval == -2)
				{
					return ECNT_RETURN;
				}
				skb = *skbp;
			}

		}
#ifdef TCSUPPORT_PON_IP_HOST
		else if(pon_insert_tag_hook && vlan_mode == MODE_SFU){
			if(isVoipWan(orig_dev)){
				skb->pon_vlan_flag |= PON_PKT_VOIP_RX;
				if(pon_insert_tag_hook(&skb) == -1)
				{
					kfree_skb(skb);
					return ECNT_RETURN_DROP;
				}
			}
		}
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_ponvlan_xmit_inline_hook(struct sk_buff **skbp, struct net_device *dev)
{
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
	struct sk_buff *skb = *skbp;
	int vlan_mode = MODE_HGU;
	int retval = ECNT_CONTINUE;
	if(pon_vlan_get_mode_hook)
		vlan_mode = pon_vlan_get_mode_hook();

    if(vlan_mode == MODE_SFU)
	{
		/* packet from cpe */
        if (skb->original_dev == NULL) 
		{
			skb->pon_vlan_flag |= PON_PKT_FROM_CPE;
        }
	}
	else
	{
		if((strcmp(skb->dev->name,"pon") == 0)){
			//Marked for supporting OpenWRT HGU mode. 20230501 Toby
			/*skb->pon_vlan_flag |= PON_VLAN_TX_CALL_HOOK;*/ 
			#ifdef TCSUPPORT_PON_SFU_HGU_HYBRID
			if ((skb->pon_vlan_flag & PON_PKT_FROM_HYBRID_PPTP) == 0){
				skb->pon_vlan_flag |= PON_PKT_FROM_HYBRID_VEIP;
			}
			#endif
		}
	}
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
	if(pon_insert_tag_hook)
	{	
		retval = pon_insert_tag_hook(skbp);
		if(retval == -1)
		{
			kfree_skb(*skbp);
			return ECNT_RETURN_DROP;
        }
		if(retval == -2)
			return ECNT_RETURN;
		skb = *skbp;
	}
    if(pon_check_vlan_hook)
	{
		retval = pon_check_vlan_hook(dev,skb);
		if(retval != 1)
			return ECNT_RETURN;
	}
	return ECNT_CONTINUE;
}
#endif


#if defined(TCSUPPORT_VLAN_TAG)
static inline int ecnt_insert_vlan_tag_inline_hook(struct sk_buff **skbp, struct net_device *dev)
{
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
	struct sk_buff *skb = *skbp;
#if defined(TCSUPPORT_PON_VLAN)
	int vlan_mode = MODE_HGU;
#endif

#ifdef TCSUPPORT_PON_VLAN
	if(pon_vlan_get_mode_hook)
		vlan_mode = pon_vlan_get_mode_hook();
	
	if(vlan_mode == MODE_HGU)
#endif	
	{
	if (check_vtag_hook && (check_vtag_hook()) == 1)
	{
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
		if (insert_vtag_hook && (-1 == insert_vtag_hook(skbp)))
		{
			return ECNT_RETURN_DROP;
		}
		skb = *skbp;
	}
	}
	return ECNT_CONTINUE; 

}

static inline int ecnt_remark_vlan_pbit_inline_hook(struct sk_buff * skb)
{
	if (NULL != skb->sk && sk_fullsock(skb->sk) && skb->sk->lPbit <= 7 )
	{
		skb->vlan_tag_flag |= VLAN_TAG_PBIT_REMARK_ENABLE;
		skb->vlan_tag_flag &= ~(0x7<<16);
		skb->vlan_tag_flag |= (skb->sk->lPbit << 16);
	}

	return ECNT_CONTINUE;
}
#endif

#if defined(TCSUPPORT_PON_MAC_FILTER)
extern int (*pon_check_mac_hook)(struct sk_buff *skb);
extern int (*pon_mac_filter_get_mode_hook)(void);

static inline int ecnt_pon_macfilter_rx_inline_hook(struct sk_buff **skbp,struct net_device *orig_dev)
{	
	int mac_filter_mode = MODE_HGU;
	struct sk_buff *skb = *skbp;
	int retval = ECNT_CONTINUE;
	
	if(pon_mac_filter_get_mode_hook)
		mac_filter_mode = pon_mac_filter_get_mode_hook();

	if(pon_check_mac_hook){
		if(mac_filter_mode == MODE_HGU)
		{
			if(strcmp(orig_dev->name,"pon") == 0)
				skb->pon_mac_filter_flag |= PON_MAC_FILTER_RX_CALL_HOOK;

			retval = pon_check_mac_hook(*skbp);
			if(retval == -1)
			{				
				kfree_skb(*skbp);
				return ECNT_RETURN_DROP;
			}
		} 
#ifdef TCSUPPORT_CHS
		else if(mac_filter_mode == MODE_SFU)
		{
			if (orig_dev->name[0] == 'e'|| orig_dev->name[0] == 'r'|| orig_dev->name[0] == 'u')
				skb->pon_mac_filter_flag |= PON_MAC_FILTER_RX_CALL_HOOK;

			retval = pon_check_mac_hook(*skbp);
			if(retval == -1)
			{				
				kfree_skb(*skbp);
				return ECNT_RETURN_DROP;
			}
		}
#endif
	}
		
	return ECNT_CONTINUE; 
}


static inline int ecnt_pon_macfilter_xmit_inline_hook(struct sk_buff **skbp)
{
	struct sk_buff *skb = *skbp;	
	int mac_filter_mode = MODE_HGU;
	int retval = ECNT_CONTINUE;

	if(pon_mac_filter_get_mode_hook)
		mac_filter_mode = pon_mac_filter_get_mode_hook();


	if(pon_check_mac_hook)
	{
		if(mac_filter_mode == MODE_SFU 
			&& (skb->dev->name[0] == 'e' || skb->dev->name[0] == 'r' || skb->dev->name[0] == 'u'))
			skb->pon_mac_filter_flag |= PON_MAC_FILTER_TX_CALL_HOOK;

		retval = pon_check_mac_hook(*skbp);
		if(retval == -1)
		{		
			//printk("[%s:%d]pon_check_mac_hook fail:kfree,orig_dev->name=%s\n",__FUNCTION__,__LINE__, skb->dev->name);
			kfree_skb(*skbp);
			return ECNT_RETURN_DROP;
		}
	}

	return ECNT_CONTINUE;
}
#endif

#if defined(TCSUPPORT_GPON_DOWNSTREAM_MAPPING)
extern int (*gpon_downstream_mapping_hook)(struct sk_buff *skb);
static inline int ecnt_downstream_mapping_inline_hook(struct sk_buff *skb)
{
	if(gpon_downstream_mapping_hook && (-1 == gpon_downstream_mapping_hook(skb)))
		return ECNT_RETURN_DROP;

	return ECNT_CONTINUE;
}
#endif
static inline int ecnt_netif_recv_inline_hook(struct sk_buff **pskb)
{
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
	struct sk_buff *skb = *pskb;
	int ret;
	struct net_device *orig_dev;
	orig_dev = skb->dev;
#if defined(TCSUPPORT_PON_VLAN)
	int vlan_mode = MODE_HGU;
#endif

	ECNT_CORE_DEV_HOOK(ECNT_NETIF_RCV_SKB, skb);
#ifndef TCSUPPORT_PON_VLAN
	ecnt_skb_original_dev_inline_hook(pskb);
	skb = *pskb;
#endif
#if defined(TCSUPPORT_PON_VLAN) 
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
	ret = ecnt_ponvlan_rx_inline_hook(pskb, orig_dev);
	if(ret != ECNT_CONTINUE)
		return ECNT_RETURN;
	skb = *pskb;
#endif

#ifdef TCSUPPORT_PON_VLAN
	if(pon_vlan_get_mode_hook)
		vlan_mode = pon_vlan_get_mode_hook();

	if(vlan_mode == MODE_HGU)
#endif
	{
#ifdef TCSUPPORT_VLAN_TAG
		  if (check_vtag_hook && (check_vtag_hook() == 1))
		  {
			  if (get_vtag_hook)
				  if (-1 == get_vtag_hook(orig_dev, skb)) {
					  kfree_skb(skb);
					  return ECNT_RETURN_DROP;
				  }
		  }
#endif	
	}

#if defined(TCSUPPORT_PON_MAC_FILTER) 
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
	ret = ecnt_pon_macfilter_rx_inline_hook(pskb, orig_dev);
	if(ret != ECNT_CONTINUE){
		return ECNT_RETURN;
	}
	skb = *pskb;
#endif

#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK) 
	if (orig_dev && 
		(!strncmp(orig_dev->name, "nas", 3) || !strncmp(orig_dev->name, "ppp", 3)))
	{
		skb->wan_dev = orig_dev;
	}
#endif

#if defined(TCSUPPORT_PORTBIND)
	if ( arht_netif_recv_portbind_hook )
	{
		arht_netif_recv_portbind_hook(skb, orig_dev);
	}
#endif

#ifdef CONFIG_SMUX
	if((orig_dev->priv_flags & IFF_RSMUX) && smux_pkt_recv_hook
#ifdef TCSUPPORT_XPON_HAL_API_EXT
		&&!(skb->mark&DOWNSTREAM_SIMULATION_MASK)
#endif
	){
		ret = smux_pkt_recv_hook(skb, skb->dev, orig_dev);		  
		return ECNT_RETURN;
	}
#endif
	/*ecnt_dp_hook 0 for downstream*/
	if(fe_resource_get_set_wan_idx_hook && smux_pkt_recv_hook==NULL ){
		fe_resource_get_set_wan_idx_hook(skb,DOWN_STREAM);
	}
	return ECNT_CONTINUE;
}

#if defined(TCSUPPORT_CT)
static inline struct nf_conntrack *__skb_nfct(const struct sk_buff *skb)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,10,17)
	return skb_nfct(skb);
#else
	return skb->nfct;
#endif
}

#if defined(TCSUPPORT_CT_APPROUTE) || defined(TCSUPPORT_CT_MAXNET_DPI)
static inline int setDscpRemark(struct sk_buff **pskb, uint8_t u_dscp)
{
	struct sk_buff *skb = NULL;
	struct iphdr *iph = NULL;
	struct ipv6hdr *ip6h = NULL;

	if ( NULL == pskb || NULL == *pskb )
		return ECNT_CONTINUE;

	skb = skb_unshare(*pskb, GFP_ATOMIC);
	*pskb = skb;
	if ( NULL == skb )
	{
		return ECNT_CONTINUE;
	}

	if ( htons(ETH_P_IP) == skb->protocol )
	{
		iph = ip_hdr(skb);
		if ( iph )
		{
			iph->tos &= 0x03;
			iph->tos |= (u_dscp<<2);
			iph->check = 0;
			iph->check = ip_fast_csum((u8 *)iph, iph->ihl);
		}
	}
	else if ( htons(ETH_P_IPV6) == skb->protocol )
	{
		ip6h = ipv6_hdr(skb);
		if ( ip6h )
		{
			(*(u16 *) ip6h) &= htons(0xf03f);
			(*(u16 *) ip6h) |= htons(u_dscp<<6);
		}
	}

	return ECNT_CONTINUE;
}
static inline int approute_dscp_hook(struct sk_buff **pskb)
{
	struct sk_buff *skb = NULL;
	uint8_t u_dscp = 0;

	if ( NULL == pskb )
		return ECNT_CONTINUE;

	skb = *pskb;
	if ( NULL == skb || NULL== skb->dev || NULL == skb->orig_dev
		|| NULL == __skb_nfct(skb) )
		return ECNT_CONTINUE;

	/* 2. upstream */
	if ( ('e' == skb->orig_dev->name[0] || 'r' == skb->orig_dev->name[0])
		&& ('n' == skb->dev->name[0] || 'p' == skb->dev->name[0]
#if defined(TCSUPPORT_CT_APPROUTE)
		|| (0 != vpn_if_prefix[0] && vpn_if_prefix[0] == skb->dev->name[0]
			&& vpn_if_prefix[1] == skb->dev->name[1])
#endif
		) )
	{
		if ( NF_DSCP_DEF != __skb_nfct(skb)->dscp_up )
		{
			u_dscp = __skb_nfct(skb)->dscp_up & 0x3f;
			/* there is no MAC layer in skb data when vpn interface for upstream. */
			setDscpRemark(pskb, u_dscp);
		}
	}
	/* 3. down stream */
	else if ( ('n' == skb->orig_dev->name[0] || 'p' == skb->orig_dev->name[0]
#if defined(TCSUPPORT_CT_APPROUTE)
		|| (0 != vpn_if_prefix[0] && vpn_if_prefix[0] == skb->orig_dev->name[0]
			&& vpn_if_prefix[1] == skb->orig_dev->name[1])
#endif
		)
		&& ('e' == skb->dev->name[0] || 'r' == skb->dev->name[0]) )
	{
		if ( NF_DSCP_DEF != __skb_nfct(skb)->dscp_dw )
		{
			u_dscp = __skb_nfct(skb)->dscp_dw & 0x3f;
			setDscpRemark(pskb, u_dscp);
		}
	}

	return ECNT_CONTINUE;
}
#endif

#endif

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
extern int SmbdTxSpeedOn;
#endif

static inline int ecnt_dev_queue_xmit_inline_hook(struct sk_buff **pskb)
{
/*!!!If pskb is used as the input parameter of a function, the skb must be reassigned after the function call is finished*/
	struct sk_buff *skb = *pskb;

    if(skb->igmp_leave_drop_flag == 1) {
        kfree_skb(*pskb);
        return ECNT_RETURN_DROP ;
    }

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
	if(SmbdTxSpeedOn && skb->smbd_on_speed){
		return ECNT_CONTINUE;
	}
#endif
    if(skb->shortcut_on_speed)
    {
        return ECNT_CONTINUE;
    }
	if(!isEN751221)
	{
		/*only sk_fullsock sock check shortcut_sk*/
	    if(skb->sk && sk_fullsock(skb->sk) && skb->sk->sk_shortcut_info.shortcut_sk == 1
	        && skb->dev && ((!strncmp(skb->dev->name, "eth0.", 5)) || (!strncmp(skb->dev->name, "ra", 2))))
	    {
	        skb->sk->sk_shortcut_info.out_dev = skb->dev;
	        memcpy(skb->sk->sk_shortcut_info.mac_header, skb->data, ETH_HLEN);
	        memcpy(skb->sk->sk_shortcut_info.ip_header, skb->data + ETH_HLEN, sizeof(struct iphdr));
	        skb->sk->sk_shortcut_info.shortcut_speed = 1;			
	    }
	}

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
	if (fe_resource_mark_meter_hook )
	{
		fe_resource_mark_meter_hook(skb, DOWN_STREAM);
	}
	if (fe_resource_mark_acnt_hook )
	{
		fe_resource_mark_acnt_hook(skb, DOWN_STREAM);
	}      
	if(fe_resource_get_set_wan_idx_hook ){
		fe_resource_get_set_wan_idx_hook(skb,UP_STREAM);
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_register_netdevice_inline_hook(struct net_device *dev)
{	
#if defined(TCSUPPORT_CT)
#ifdef TCSUPPORT_PORTBIND
	if (portbind_update_hook){
		portbind_update_hook(dev, ADD_GROUP);
	}
#endif
#endif
	
#if defined(TCSUPPORT_TSO_ENABLE)
	if(!strncmp(dev->name, "eth", 3) ){
		dev->features |= NETIF_F_SG;
		dev->features |= NETIF_F_GSO;
		dev->hw_features |= NETIF_F_SG;
		dev->hw_features |= NETIF_F_GSO;
		dev->wanted_features |= NETIF_F_SG;
		dev->wanted_features |= NETIF_F_GSO;
	}
	if(!strncmp(dev->name, "nas", 3) || !strncmp(dev->name, "pon", 3) || !strncmp(dev->name, "ppp", 3)){
		dev->hw_features |= NETIF_F_SG;
		dev->hw_features |= NETIF_F_GSO;
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_neighbour_sysctl_register_inline_hook(struct ctl_table *neigh_var, struct neigh_parms *p, struct net_device *dev)
{
#if defined(TCSUPPORT_IPV6_ENHANCEMENT) || defined(TCSUPPORT_IPV6_CEROUTER)
	struct ctl_table *neigh_var_tmp =NULL;
#endif	
#ifdef TCSUPPORT_IPV6_ENHANCEMENT
	neigh_var_tmp = neigh_var+NEIGH_VAR_DEFAULT_ROUTE;
	neigh_var_tmp->data = &p->dlf_route[0];	
	neigh_var_tmp->extra1 = dev;
	neigh_var_tmp->extra2 = p;
#endif
#if defined(TCSUPPORT_IPV6_CEROUTER)
	neigh_var_tmp = neigh_var+NEIGH_VAR_RA_FLAGS;
	neigh_var_tmp->data = &p->ra_flags;	
	neigh_var_tmp->extra1 = dev;
	neigh_var_tmp->extra2 = p;
#endif
        return ECNT_CONTINUE;
}


static inline int ecnt_netif_hw_stats_inline_hook(struct net_device *dev, struct rtnl_link_stats64 *storage)
{
	if(get_net_device_interface_hw_stats_hook)
	{
		get_net_device_interface_hw_stats_hook(dev, storage);
	}

	return ECNT_CONTINUE;
}

static inline int ecnt_sock_setsockopt_inline_hook
(struct sock *sk, int optname, int val, int valbool, int *ret)
{
	switch ( optname )
	{
		case SO_TYPE_TRAFFIC:
			if (!capable(CAP_NET_ADMIN))
				*ret = -EPERM;
			else
			{
				if ( val )
					sk->lPbit |= ( (val & 0x7) << 8 );
				else
					sk->lPbit &= (~TRAFFIC_PROCESS_MASK);
				*ret = 0;
			}
			break;
		case SO_TYPE_COPY_SKB_MARK:
			if (!capable(CAP_NET_ADMIN))
				*ret = -EPERM;
			else
			{
				if ( val )
					sk->lPbit |= ( SOCK_TYPE_COPY_MARK );
				else
					sk->lPbit &= ( ~SOCK_TYPE_COPY_MARK );
				*ret = 0;
			}

			break;
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_LANMGR_NG)
		case SO_TYPE_COPY_SRC_MAC:
			if ( !capable(CAP_NET_ADMIN) )
				*ret = -EPERM;
			else
			{
				if ( val )
					sk->lPbit |= ( SOCK_TYPE_RCV_MAC );
				else
					sk->lPbit &= ( ~SOCK_TYPE_RCV_MAC );
				*ret = 0;
			}	
			break;
#endif
		default:
			break;
	}

	return ECNT_CONTINUE;
}

static inline int arht_sock_getsockopt_inline_hook
(struct sock *sk, int optname, char __user *optval, int __user *optlen, int *ret)
{
	int val = 0, len = 0, ret_v = *ret;

	switch ( optname )
	{
		case SO_TYPE_SRC_MAC:
			break;
		default:
			return *ret;
	}

	if (get_user(len, optlen))
		return -EINVAL;
	if (len < 0)
		return -EINVAL;

	switch ( optname )
	{
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_LANMGR_NG)
		case SO_TYPE_SRC_MAC:
			if ( len >= 6 )
			{
				len = 6;
				if ( put_user(len, optlen) )
					return -EINVAL;
				if ( copy_to_user(optval, sk->hwaddr, len) )
					return -EINVAL;

				*ret = 0;
				break;
			}
			else
				return -EINVAL;

#endif
		default:
			break;
	}

	return *ret;
}

#ifndef AIROHA_BSP
#include "ecnt_net_core_sdk.h"
#endif/*AIROHA_BSP*/

#endif

