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

#ifndef _LINUX_ECNT_NETFILTER_IPV6_SDK_H
#define _LINUX_ECNT_NETFILTER_IPV6_SDK_H
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <ecnt_hook/ecnt_hook.h>
#include "../ecnt_net_ipv6.h"
#include <linux/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <net/netfilter/nf_nat_core.h>
#endif

static inline int ecnt_ip6t_mangle_out_inline_hook
(struct sk_buff *skb, const struct net_device *out)
{
#if defined(TCSUPPORT_CT)
	char if_buf[10];
	int if_index = 0;
#endif

	if ( !skb || !out )
		return ECNT_CONTINUE;

#if defined(TCSUPPORT_CT)
	memset(if_buf, 0, sizeof(if_buf));
	
		if ( strlen(out->name) > 3 )
		strlcpy(if_buf, out->name + 3,sizeof(if_buf));

		if ( out->name[0] == 'n' && strlen(if_buf) > 2 )
		{
			if_index = (if_buf[0] - '0') * MAX_PVC_NUM + (if_buf[2] - '0');
			skb->mark |= (if_index + 1) << 16;
		}
		else if (out->name[0] == 'p')
		{
			if_index = skip_atoi(if_buf);
			skb->mark |= (if_index + 1) << 16;
		}
	
#endif

	return ECNT_CONTINUE;
}

#endif/*_LINUX_ECNT_NET_IPV4_SDK_H*/

