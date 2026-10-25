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

#ifndef _LINUX_ECNT_NETFILTER_SDK_H
#define _LINUX_ECNT_NETFILTER_SDK_H
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <net/netfilter/nf_conntrack.h>
#include <ecnt_hook/ecnt_hook.h>
#include <linux/ecnt_vlan_bind.h>
#include "../ecnt_net.h"
#include <net/netfilter/nf_conntrack_extend.h>
#include <net/netfilter/nf_conntrack_seqadj.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/ipv6.h>

static inline void ecnt_nf_conntrack_init_start_inline_hook(int cpu)
{
#if defined(TCSUPPORT_CT_VLAN_BIND)
	/* Set up fake conntrack: to never be deleted, not in any hashes */
	for_each_possible_cpu(cpu) {
		struct nf_conn *ct = &per_cpu(nf_conntrack_untracked, cpu);
		ct->ct_general.lVlanId = VBIND_INVALID_VLANID;
#if defined(TCSUPPORT_CT_APPROUTE)
		ct->ct_general.dscp_up		= NF_DSCP_DEF;
		ct->ct_general.dscp_dw		= NF_DSCP_DEF;
		ct->ct_general.up_mark		= 0;
		ct->ct_general.match_flag	= 0;
#endif
	}
#endif
	return;
}

static inline int ecnt_generic_packet_inline_hook
(struct nf_conn *ct,
const struct sk_buff *skb,
unsigned int dataoff,
enum ip_conntrack_info ctinfo,
u_int8_t pf,
unsigned int hooknum,
unsigned int *timeout)
{
#if defined(TCSUPPORT_CT)
	if ( (0x32 == nf_ct_protonum(ct)) /* esp */
		&& (!(test_bit(IPS_SEEN_REPLY_BIT, &ct->status))) /* UNREPLIED */
		&& (nf_conntrack_esp_timeout > 0) )
	{
		nf_ct_refresh_acct(ct, ctinfo, skb, nf_conntrack_esp_timeout*HZ);
		return ECNT_RETURN;
	}
#endif

	return ECNT_CONTINUE;
}

static int ecnt_nf_conntrack_ipv6_route_hook(struct nf_conn* ct, struct sk_buff *skb)
{
#if defined(TCSUPPORT_CT_VLAN_BIND)
	if(0 == skb->vlan_tags[0])
	{
		if(ct->ct_general.lVlanId != VBIND_INVALID_VLANID)
		{
			ct->ct_general.lVlanId = VBIND_INVALID_VLANID;
			if(ra_sw_nat_hook_ipv6_nf_conntrack_hook)
				ra_sw_nat_hook_ipv6_nf_conntrack_hook(&ct->ct_general);
		}
	}
	else
	{
		if(ct->ct_general.lVlanId != VTAG_GET_VID(skb->vlan_tags[0]))
		{
			ct->ct_general.lVlanId = VTAG_GET_VID(skb->vlan_tags[0]);
			if(ra_sw_nat_hook_ipv6_nf_conntrack_hook)
				ra_sw_nat_hook_ipv6_nf_conntrack_hook(&ct->ct_general);
		}
	}
#endif
	return 0;
}


#endif/*_LINUX_ECNT_NETFILTER_SDK_H*/

