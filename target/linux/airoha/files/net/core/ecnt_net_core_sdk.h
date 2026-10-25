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

#ifndef _LINUX_ECNT_NET_CORE_SDK_H
#define _LINUX_ECNT_NET_CORE_SDK_H
#include <linux/version.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <linux/kmemcheck.h>
#endif
#include <linux/compiler.h>
#include <linux/time.h>
#include <linux/bug.h>
#include <linux/cache.h>

static inline int ecnt_unregister_netdevice_queue_inline_hook(struct net_device *dev)
{
#if defined(TCSUPPORT_CT)
#ifdef TCSUPPORT_PORTBIND
	if (portbind_update_hook){
		portbind_update_hook(dev, DEL_GROUP);
	}
#endif
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_sock_init_data_inline_hook(struct socket *sock, struct sock *sk)
{
#if defined(TCSUPPORT_CT_VLAN_TAG)
#ifdef TR143
			sk->lPbit = 8;
#endif
#endif
return ECNT_CONTINUE;
}

static inline int ecnt_skb_scrub_packet_inline_hook(struct sk_buff *skb)
{
#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
	if(skb->dev && (0 == strncmp(skb->dev->name, "veth", 4))){
		return ECNT_RETURN;
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_ppp_flags_init_inline_hook(struct net_device *dev)
{
#if defined(TCSUPPORT_CT)	
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	if ( NULL != dev )
		dev->ppp_flags = 0;
#endif
#endif

	return ECNT_CONTINUE;
}

#endif/*_LINUX_ECNT_NET_CORE_SDK_H*/

