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

#ifndef _LINUX_ECNT_NET_IPV4_SDK_H
#define _LINUX_ECNT_NET_IPV4_SDK_H

static inline int ecnt_udp_send_skb_inline_hook
(struct inet_sock *inet, struct sock *sk, struct sk_buff *skb)
{
	if ( !inet || !sk || !skb )
		return ECNT_CONTINUE;

#if defined(TCSUPPORT_CT_VLAN_BIND)
	ecnt_dns_vlanid_store(inet, sk, skb);
#endif

	return ECNT_CONTINUE;
}


static inline int ecnt_icmp_send_inline_hook
(struct sk_buff *skb_in, int type, int code, __be32 info)
{

#if defined(TCSUPPORT_CT)
	if ( nf_conntrack_portscan_enable
		&& ICMP_DEST_UNREACH == type
		&& ICMP_PORT_UNREACH == code )
	{
		return ECNT_RETURN;
	}
#endif

	return ECNT_CONTINUE;
}


static inline int ecnt_arp_xmit_inline_hook
(struct sk_buff *skb)
{
#if defined(TCSUPPORT_CT_QOS)
	ecnt_set_qoshigh_hook(skb);
#endif

	return ECNT_CONTINUE;
}


static inline int ecnt_get_tcp4_sock_inline_hook(struct sock *sk, __be32 * src)
{
#if defined(TCSUPPORT_CMCCV2)
	/*if bind interface, show interface address*/
	if(sk->sk_state == TCP_LISTEN && *src == htonl(INADDR_ANY) && sk->sk_bound_dev_if){
		*src = ecnt_ipv4_dev_get_saddr_by_index(sock_net(sk), 
			sk->sk_bound_dev_if);
	}
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_raw_recvmsg_inline_hook(struct sock *sk, struct sk_buff *skb)
{
#if defined (TCSUPPORT_CT_JOYME4)
	if ( sk && skb )
	{
		if ( sk->lPbit & SOCK_TYPE_COPY_MARK )
		{
			sk->sk_mark = (skb->mark & 0xfffffffe);
			if ( 1 == skb->lan_vlan_tci_valid )
				sk->sk_mark |= 0x1;
		}
	}
#endif

	return ECNT_CONTINUE;
}


static inline void arht_ip_route_me_harder_inline_hook(struct net *net, struct sk_buff *skb, struct flowi4 *fl4)
{
#if defined(TCSUPPORT_CT_L2TP_VPN) && (defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CFG_NG_UNION))
	int ppp_idx = 0;
	struct net_device *dev = NULL;
#endif

	if ( NULL == net || NULL == skb || NULL == fl4 )
		return;

#if defined(TCSUPPORT_CT_L2TP_VPN) && (defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_CFG_NG_UNION))
	if ( skb->sk )
	{
		dev = dev_get_by_index_rcu(net, skb->sk->sk_bound_dev_if);
		if ( dev )
		{
			sscanf(dev->name,"ppp%3d", &ppp_idx);
			if ( ppp_idx >= 100 && ppp_idx < 116 )
			{
				fl4->flowi4_oif = 0;
			}
		}
	}
#endif

	return;
}

#endif/*_LINUX_ECNT_NET_IPV4_SDK_H*/

