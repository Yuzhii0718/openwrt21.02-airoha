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

#ifndef _LINUX_ECNT_NET_IPV6_SDK_H
#define _LINUX_ECNT_NET_IPV6_SDK_H
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <uapi/linux/in6.h>
#include <ecnt_hook/ecnt_hook.h>
#include "../ecnt_net.h"
#include <linux/ecnt_vlan_bind.h>
#include <net/tcp.h>
#include <net/addrconf.h>
#include <linux/version.h>
#include "ecnt_event_global/ecnt_event_global.h"
#include "ecnt_event_global/ecnt_event_system.h"

#if defined(TCSUPPORT_CMCCV2)
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#endif

#include "../../include/net/tcp.h"
#include "../../include/net/xfrm.h"
extern void addrconf_mod_rs_timer(struct inet6_dev *idev, unsigned long when);
extern void tcp_v6_fill_cb(struct sk_buff *skb, const struct ipv6hdr *hdr, const struct tcphdr *th);

#if defined(TCSUPPORT_CT)
#define MAX_PVC_NUM 8
#define isdigit(c)	('0' <= (c) && (c) <= '9')
static inline int skip_atoi(char *s)
{
	int i=0;

	while (isdigit(*s))
		i = i*10 + *(s++) - '0';
	return i;
}
#endif

static inline int ecnt_udp6_lib_rcv_inline_hook
( struct sk_buff *skb,struct udphdr *uh, int proto)
{
#if defined(TCSUPPORT_CT_PORTSLIMIT)
	if ( IPPROTO_UDP == proto )
	{
		if ( NULL != skb->dev &&
			( ('n' == skb->dev->name[0] && 'a' == skb->dev->name[1] && 's' == skb->dev->name[2])
			|| ('p' == skb->dev->name[0] && 'p' == skb->dev->name[1] && 'p' == skb->dev->name[2]))
			)
		{
			if ( 53 == uh->dest
				|| 547 == uh->dest
				|| 1900 == uh->dest )
				return ECNT_RETURN_DROP;
		}
	}
#endif

	return ECNT_CONTINUE;
}


static inline void ecnt_udpv6_sendmsg_inline_hook
(struct flowi6 *p_fl6, struct sock *sk)
{
#if defined(TCSUPPORT_CT)
	struct net_device *dev = NULL;
	char if_buf[10];
	int if_index = 0;
#endif

	if ( !p_fl6 )
		return;

#if defined(TCSUPPORT_CT)
	if ( !p_fl6->flowi6_oif )
	{
		p_fl6->flowi6_oif = sk->sk_bound_dev_if;
		if ( p_fl6->flowi6_oif )
		{
			dev = dev_get_by_index(sock_net(sk), sk->sk_bound_dev_if);
			if ( dev )
			{
				memset(if_buf, 0, sizeof(if_buf));
				
					if ( strlen(dev->name) > 3 )
					strlcpy(if_buf, dev->name + 3, sizeof(if_buf));
					if ( dev->name[0] == 'n' && strlen(if_buf) > 2 )
					{
						if_index = (if_buf[0] - '0') * MAX_PVC_NUM 
							+ (if_buf[2] - '0');
						sk->sk_mark &= (~0x7f0000);
						sk->sk_mark |= (if_index + 1) << 16;
					}
					else if ( dev->name[0] == 'p' )
					{
						if_index = skip_atoi(if_buf);
						sk->sk_mark &= (~0x7f0000);
						sk->sk_mark |= (if_index + 1) << 16;
					}
				
				dev_put(dev);
			}
		}
	}
#endif

	return;
}

static inline int ecnt_udp_v6_push_pending_frames_inline_hook
(struct inet_sock *inet, struct sock *sk, struct sk_buff *skb)
{
	if ( !inet || !sk || !skb )
		return ECNT_CONTINUE;

#if defined(TCSUPPORT_CT_VLAN_BIND)
	ecnt_dns_vlanid_store(inet, sk, skb);
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_tcp_v6_rcv_inline_hook
(struct sock *sk, struct sk_buff *skb,
struct net *net, struct ipv6hdr *hdr, struct tcphdr *th)
{
	if ( !sk )
	{
#if defined(TCSUPPORT_CT)
		if ( nf_conntrack_portscan_enable )
		{
			if (!xfrm6_policy_check(NULL, XFRM_POLICY_IN, skb))
				return ECNT_RETURN_DROP;
			tcp_v6_fill_cb(skb, hdr, th);
			if (skb->len < (th->doff<<2) || tcp_checksum_complete(skb))
			{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
				__TCP_INC_STATS(net, TCP_MIB_CSUMERRORS);
				__TCP_INC_STATS(net, TCP_MIB_INERRS);
#else
			    TCP_INC_STATS_BH(net, TCP_MIB_CSUMERRORS);
				TCP_INC_STATS_BH(net, TCP_MIB_INERRS);
#endif
			}

			return ECNT_RETURN_DROP;
		}
#endif
	}
	else
	{
#if defined(TCSUPPORT_CT)
		if ( NULL != skb->dev && 'b' != skb->dev->name[0] )
		{
			if ( htons(80) == th->dest)
				return ECNT_RETURN_DROP;
		}
#endif
	}

	return ECNT_CONTINUE;
}

static inline int ecnt_icmpv6_send_inline_hook
(struct sk_buff *skb_in, int type, int code, __be32 info)
{

#if defined(TCSUPPORT_CT)
	if ( nf_conntrack_portscan_enable
		&& ICMPV6_DEST_UNREACH == type
		&& ICMPV6_PORT_UNREACH == code )
	{
		return ECNT_RETURN;
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_ndisc_send_skb_inline_hook
(struct sk_buff *skb)
{
#if defined(TCSUPPORT_CT_QOS)
	ecnt_set_qoshigh_hook(skb);
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_get_tcp6_sock_inline_hook(struct seq_file *seq, struct sock *sp, int i)
{
#if defined(TCSUPPORT_CMCCV2)
	const struct in6_addr *dest, *src;
	__u16 destp, srcp;
	int timer_active;
	unsigned long timer_expires;
	const struct inet_sock *inet = inet_sk(sp);
	const struct tcp_sock *tp = tcp_sk(sp);
	const struct inet_connection_sock *icsk = inet_csk(sp);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	struct fastopen_queue *fastopenq = &(icsk->icsk_accept_queue.fastopenq);
#else
	struct fastopen_queue *fastopenq = icsk->icsk_accept_queue.fastopenq;
#endif
	#define GET_DEV_MAX_ADDR 4
	struct in6_addr ifa_src[GET_DEV_MAX_ADDR];
	__be32 addr;
	int ifa_num = 0;
	int index = 0;

	dest  = &sp->sk_v6_daddr;
	src   = &sp->sk_v6_rcv_saddr;

	/*if bind address, show dev max 3 ipv6 address and 1 ipv4 compat address*/
	if(sp->sk_state == TCP_LISTEN && ipv6_addr_type(src) == IPV6_ADDR_ANY && sp->sk_bound_dev_if){

		/*get other information, copy from funtion get_tcp6_sock*/
		destp = ntohs(inet->inet_dport);
		srcp  = ntohs(inet->inet_sport);

		if (icsk->icsk_pending == ICSK_TIME_RETRANS) {
			timer_active	= 1;
			timer_expires	= icsk->icsk_timeout;
		} else if (icsk->icsk_pending == ICSK_TIME_PROBE0) {
			timer_active	= 4;
			timer_expires	= icsk->icsk_timeout;
		} else if (timer_pending(&sp->sk_timer)) {
			timer_active	= 2;
			timer_expires	= sp->sk_timer.expires;
		} else {
			timer_active	= 0;
			timer_expires = jiffies;
		}
		
		/*reserve 1 address for ipv4*/
		ifa_num = ecnt_ipv6_dev_get_saddr_by_index(sock_net(sp), sp->sk_bound_dev_if,
					ifa_src, GET_DEV_MAX_ADDR-1);

		/*get interface ipv4 address*/
		addr = ecnt_ipv4_dev_get_saddr_by_index(sock_net(sp), sp->sk_bound_dev_if);
		if(addr != htonl(INADDR_ANY)){
			/*ipv4 address in ipv6 address compat*/
			ifa_src[ifa_num].s6_addr32[3] = addr;
			ifa_src[ifa_num].s6_addr32[2] = htonl(0x0000FFFF); 
			ifa_src[ifa_num].s6_addr32[1] = htonl(0x00000000);
			ifa_src[ifa_num].s6_addr32[0] = htonl(0x00000000);
			ifa_num++;
		}

		if(!ifa_num)
			return ECNT_CONTINUE;

		for(index=0;index<ifa_num;index++){
			src = &ifa_src[index];
			seq_printf(seq,
				   "%4d: %08X%08X%08X%08X:%04X %08X%08X%08X%08X:%04X "
				   "%02X %08X:%08X %02X:%08lX %08X %5u %8d %lu %d %pK %lu %lu %u %u %d\n",
				   i,
				   src->s6_addr32[0], src->s6_addr32[1],
				   src->s6_addr32[2], src->s6_addr32[3], srcp,
				   dest->s6_addr32[0], dest->s6_addr32[1],
				   dest->s6_addr32[2], dest->s6_addr32[3], destp,
				   sp->sk_state,
				   tp->write_seq-tp->snd_una,
				   (sp->sk_state == TCP_LISTEN) ? sp->sk_ack_backlog : (tp->rcv_nxt - tp->copied_seq),
				   timer_active,
				   jiffies_delta_to_clock_t(timer_expires - jiffies),
				   icsk->icsk_retransmits,
				   from_kuid_munged(seq_user_ns(seq), sock_i_uid(sp)),
				   icsk->icsk_probes_out,
				   sock_i_ino(sp),
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
				   atomic_read(&sp->sk_refcnt.refs), sp,
#else
				   atomic_read(&sp->sk_refcnt), sp,
#endif
				   jiffies_to_clock_t(icsk->icsk_rto),
				   jiffies_to_clock_t(icsk->icsk_ack.ato),
				   (icsk->icsk_ack.quick << 1) | icsk->icsk_ack.pingpong,
				   tp->snd_cwnd,
				   sp->sk_state == TCP_LISTEN ?
					(fastopenq ? fastopenq->max_qlen : 0) :
					(tcp_in_initial_slowstart(tp) ? -1 : tp->snd_ssthresh)
				   );
		}
		/*can skip orign get_tcp6_sock process*/
		return ECNT_RETURN;	
	}
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_addrconf_ppp_ondemand_config_inline_hook(struct net_device *dev, struct inet6_dev *idev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
	if ( NULL == dev || NULL == idev )
		return ECNT_RETURN_DROP;
	
	if ( (dev->type & ARPHRD_PPP) && (dev->ppp_flags & PPP_ONDEMAND_DOWN) )
	{
		if ( idev->if_flags & IF_RS_SENT )
		{
			idev->if_flags &= (~IF_RS_SENT);
			idev->if_flags &= (~IF_RA_RCVD);
		}
		idev->rs_probes = 1;
		addrconf_mod_rs_timer(idev, 
				   (idev->rs_probes == idev->cnf.rtr_solicits) ?
				   idev->cnf.rtr_solicit_delay :
				   idev->cnf.rtr_solicit_interval);
		return ECNT_RETURN_DROP;
	}

	if ( (dev->type & ARPHRD_PPP) && (dev->ppp_flags & PPP_ONDEMAND_UP) )
	{
		if ( IF_RS_SENT != ( idev->if_flags & IF_RS_SENT ) )
			idev->if_flags |= IF_RS_SENT;
	}
#endif
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_addrconf_ppp_ondemand_down_inline_hook(struct net_device *dev, struct inet6_ifaddr *ifp)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
		if ( (dev->type & ARPHRD_PPP) && (dev->ppp_flags & PPP_ONDEMAND_DOWN) )
		{
			write_lock_bh(&ifp->idev->lock);
			spin_lock(&ifp->lock);
			ifp->idev->rs_probes = 1;
			addrconf_mod_rs_timer(ifp->idev, ifp->idev->cnf.rtr_solicit_interval);
			spin_unlock(&ifp->lock);
			write_unlock_bh(&ifp->idev->lock);
			return ECNT_CONTINUE;
		}
#endif
#endif

	return ECNT_RETURN_DROP;
}


static inline void arht_rawv6_recvmsg_inline_hook(struct sock *sk, struct sk_buff *skb)
{
#if defined(TCSUPPORT_LANMGR_NG)
	if ( sk->lPbit & SOCK_TYPE_COPY_MARK )
	{
		sk->sk_mark = (skb->mark & 0xfffffffe);
		if ( 1 == skb->lan_vlan_tci_valid )
			sk->sk_mark |= 0x1;
	}
#endif
}

static inline int ecnt_addrconf_prefix_rcv_prefix_len_inline_hook
(struct prefix_info *pinfo)
{
#if defined(TCSUPPORT_CT_JOYME4)
	if ( pinfo->prefix_len < 64 )
		pinfo->prefix_len = 64;
#endif

	return ECNT_CONTINUE;
}
#endif/*_LINUX_ECNT_NET_IPV6_SDK_H*/