/*****************************************************************************
 * Airoha (HK) Limited  Airoha. ALL RIGHTS RESERVED.
 * 
 * BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
 * ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
 * DOCUMENTATIONS (��Airoha SOFTWARE��) RECEIVED FROM Airoha 
 * AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
 * BASIS ONLY. Airoha EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
 * WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
 * OR NON-INFRINGEMENT. NOR DOES Airoha PROVIDE ANY WARRANTY 
 * WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
 * MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE Airoha SOFTWARE. 
 * RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
 * WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
 * THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
 * PARTY ALL PROPER LICENSES CONTAINED IN Airoha SOFTWARE.
 * 
 * Airoha SHALL NOT BE RESPONSIBLE FOR ANY Airoha SOFTWARE RELEASES 
 * MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
 * Airoha'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE Airoha 
 * SOFTWARE RELEASED HEREUNDER SHALL BE, AT Airoha'S SOLE OPTION, TO 
 * REVISE OR REPLACE THE Airoha SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
 * LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO Airoha FOR SUCH 
 * Airoha SOFTWARE.
 *
 *****************************************************************************/

#ifndef _LINUX_ECNT_BR_SDK_H
#define _LINUX_ECNT_BR_SDK_H
#include <linux/kernel.h>
#include <linux/kmemcheck.h>
#include <linux/compiler.h>
#include <linux/time.h>
#include <linux/bug.h>
#include <linux/cache.h>
#include <linux/if_pppox.h>
#include <linux/if_bridge.h>
#include <uapi/linux/ppp_defs.h>
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
#include <linux/proc_fs.h>
#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <linux/igmp.h>
#if IS_ENABLED(CONFIG_IPV6)
#include <net/ipv6.h>
#include <net/mld.h>
#endif
#include <linux/ecnt_in.h>
#include <linux/spinlock.h>
#include "br_private.h"
#include <lan_port/lan_port_info.h>

extern int (*portbind_is_other_bridge_wan_hook)(struct net_device *dev);
extern struct net_device * (*portbind_find_bind_other_bridge_wan_hook)(struct net_device *dev);

static inline int ecnt_br_fdb_other_wan_bridge_inline_hook(struct net_device *dev)
{
	if(portbind_is_other_bridge_wan_hook)
	{
		if(dev->name[0] != 'n')
			return ECNT_CONTINUE;
		
		if(portbind_is_other_bridge_wan_hook(dev))
			return ECNT_RETURN;
	}

	return ECNT_CONTINUE;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
static inline int ecnt_br_other_bridge_portbind_inline_hook(struct sk_buff *skb, struct net_device *dev, bool local_rcv)
#else
static inline int ecnt_br_other_bridge_portbind_inline_hook(struct sk_buff *skb, struct net_device *dev, struct sk_buff *skb2)
#endif
{
	struct net_device *other_bridge_wan_dev = NULL;
	struct net_bridge_port *p = NULL;
	
	if(portbind_find_bind_other_bridge_wan_hook)
	{
		if((skb->dev->name[0] != 'e') && (skb->dev->name[0] != 'r'))
			return ECNT_CONTINUE;

		if(!dev || dev->name[0] != 'n')
			return ECNT_CONTINUE;
		
		other_bridge_wan_dev = portbind_find_bind_other_bridge_wan_hook(skb->dev);
		if(other_bridge_wan_dev == NULL)
			return ECNT_CONTINUE;

		p = br_port_get_rcu(other_bridge_wan_dev);
		if(NULL == p)
			return ECNT_CONTINUE;
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
		br_forward(p, skb, local_rcv, false);
#else
		br_forward(p, skb, skb2);
#endif
		return ECNT_RETURN;
	}

	return ECNT_CONTINUE;
}

static inline int ecnt_maybe_deliver_inline_hook(struct net_bridge_port *prev, struct sk_buff *skb)
{
	struct net_device *indev = NULL;
	struct net_device *outdev = NULL;

	#ifdef TCSUPPORT_PORT_ISOLATION
	#if defined(TCSUPPORT_ROUTEPOLICY_PRIOR_PORTBIND)
		if (portbind_sw_prior_hook && (portbind_sw_prior_hook(skb) == 1)) 
	#else
		if (portbind_sw_hook && (portbind_sw_hook() == 1)) 
	#endif		
		{
			if (skb->dev) 
				indev = skb->dev;
			if (prev) 
				outdev = prev->dev;

			if ( (indev == NULL) || 
				(outdev == NULL) ||
				(portbind_check_hook == NULL) ||
				((portbind_check_hook) && 
				portbind_check_hook(indev->name, outdev->name)) ) 
			{
				if(!checkPacketsDeliver(prev, skb, 1))
					return -1;
			}
			else
			{
				if(!checkPacketsDeliver(prev, skb, 0))
					return -1;
			}
		}
		else
		{
			if(!checkPacketsDeliver(prev,skb,1))
				return -1;
		}
	#endif

	return 0;
}

#ifndef TCSUPPORT_ECNT_MAP
static inline int is_p1905_discovery(struct sk_buff *skb, struct net_device *indev, struct net_device *todev) 
{
	unsigned short msg_type;
	unsigned char *data = NULL;
	if (skb->protocol == htons(0x893A)) {
		data = skb->data;
		msg_type = *(unsigned short*)(data + 2);
		if (msg_type == 0) {
			return 1;
		}
	}

	return 0;
}
#endif


static inline int ecnt_br_drop_1905_discovery_hook(struct net_device *indev, struct sk_buff *skb)
{
#ifndef TCSUPPORT_ECNT_MAP
		/* drop 1905 discovery packet from lan to lan */
		if ((indev!=NULL)&&(indev->name[0] == 'e' && skb->dev->name[0] == 'e')) {
			if (is_p1905_discovery(skb, indev, skb->dev)) {
				kfree_skb(skb);
				return 1;
			}
		}
#endif
	return 0;
}
#if defined(TCSUPPORT_CT)
static int ecnt_br_static_mac_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int pos = 0, idx = 0;
	unsigned char *addr = NULL;
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
	struct hlist_node *n = NULL;
	struct net_bridge_fdb_entry *f = NULL;
	ndev = dev_get_by_name(&init_net, "br0");
	if ( ndev )
		br = netdev_priv(ndev);
	if ( br )
	{
		spin_lock_bh(&br->hash_lock);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)		
		for ( idx = 0; idx < BR_HASH_SIZE; idx ++ )
#endif			
		{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
			hlist_for_each_entry_safe(f, n, &br->fdb_list, fdb_node)
#else		
			hlist_for_each_entry_safe(f, n, &br->hash[idx], hlist)
#endif			
			{
				if ( f->dst || !f->is_static || !f->is_local )
					continue;
				if ( pos >= (count - 18) )
					break;
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
				addr = f->key.addr.addr;
#else
				addr = f->addr.addr;
#endif				
				pos += sprintf(buf + pos, "%02x:%02x:%02x:%02x:%02x:%02x\n"
					, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
			}
		}
		spin_unlock_bh(&br->hash_lock);
	}
	if ( ndev )
		dev_put(ndev);
	if ( pos <= off + count )
		*eof = 1;
	*start = buf + off;
	pos -= off;
	if ( pos > count )
		pos = count;
	if ( pos < 0 )
		pos = 0;
	return pos;
}
static int ecnt_br_static_mac_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char cmd_buf[64] = {0}, action[12] = {0}, mac_buf[24] = {0};
	int ret = 0, ret2 = 0, i_action  = 0, idx = 0, tmp[6] = {0};
	unsigned char macaddr[6] = {0};
	unsigned char *addr = NULL;
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
	memset(cmd_buf, 0, sizeof(cmd_buf));
	memset(action, 0, sizeof(action));
	memset(mac_buf, 0, sizeof(mac_buf));
	memset(macaddr, 0, sizeof(macaddr));
	memset(tmp, 0, sizeof(tmp));
	if ( count > sizeof(cmd_buf) - 1 )
		return -EINVAL;
	if ( copy_from_user(cmd_buf, buffer, count))
		return -EFAULT;
	ret = sscanf(cmd_buf, "%10s %20s", action, mac_buf);
	if ( 2 == ret )
		ret = sscanf(mac_buf, "%02x:%02x:%02x:%02x:%02x:%02x",
					&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
	if ( 'A' == action[0] || 'a' == action[0] )
		i_action = 1;
	else if ( 'D' == action[0] || 'd' == action[0] )
		i_action = 2;
	if ( i_action )
	{
		ndev = dev_get_by_name(&init_net, "br0");
		if ( ndev )
			br = netdev_priv(ndev);
		if ( br )
		{
			for ( idx = 0; idx < sizeof(macaddr); idx ++ )
					macaddr[idx] = tmp[idx];
			if ( 1 == i_action ) /* add */
			{
				rtnl_lock();
				ret2 = br_fdb_insert(br, NULL, macaddr, 0);
				rtnl_unlock();
			}
			else if ( 2 == i_action ) /* delete */
			{
				rtnl_lock();
				ret2 = fdb_delete_by_addr(br, macaddr, 0);
				rtnl_unlock();
			}
		}
		if ( ndev )
			dev_put(ndev);
	}
	return count;
}
#endif

#endif/*_LINUX_ECNT_BR_SDK_H*/

