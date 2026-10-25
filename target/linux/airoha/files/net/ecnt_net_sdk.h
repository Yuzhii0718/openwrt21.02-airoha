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

#ifndef _LINUX_ECNT_NET_SDK_H
#define _LINUX_ECNT_NET_SDK_H
#include <linux/version.h>
#include <uapi/linux/in.h>
#include <uapi/linux/ecnt_in.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <net/sock.h>
#include <net/inet_sock.h>
#include <ecnt_hook/ecnt_hook.h>

#if defined(TCSUPPORT_CT_VLAN_BIND)
static inline int ecnt_dns_vlanid_store
(struct inet_sock *inet, struct sock *sk, struct sk_buff *skb)
{
	if ( !inet || !sk || !skb )
		return ECNT_CONTINUE;


	if( ( VBIND_DNSPORT == inet->inet_sport)
		&& (sk->lVlanId < VBIND_INVALID_VLANID) )
	{
		skb->vlan_tags[0] = sk->lVlanId;
		skb->vlan_tag_flag |= VLAN_TAG_FOR_DNS;
	}

	return ECNT_CONTINUE;
}
#endif

#if defined(TCSUPPORT_CT_QOS)
static inline int ecnt_set_qoshigh_hook
(struct sk_buff *skb)
{
	/* set arp packet to first queue */
	skb->mark &= (~QOS_FILTER_MARK);
	skb->mark |= QOS_HH_PRIORITY;
	/* printk("v4 arp packet to first queue.skb->mark is 0x%x\n", skb->mark); */

	return ECNT_CONTINUE;
}
#endif

#endif/*_LINUX_ECNT_NET_SDK_H*/

