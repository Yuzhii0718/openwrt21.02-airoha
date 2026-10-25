#ifndef _LINUX_ECNT_NET_IPV6_H
#define _LINUX_ECNT_NET_IPV6_H
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
#if defined(TCSUPPORT_IPV6_CEROUTER)
#include <net/ip6_route.h>
#endif
#ifdef TCSUPPORT_IPV6_ENHANCEMENT
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
static void addrconf_add_linklocal(struct inet6_dev *idev, const struct in6_addr *addr, u32 flags);
#else
static void addrconf_add_linklocal(struct inet6_dev *idev, const struct in6_addr *addr);
#endif
static int ipv6_generate_eui64(u8 *eui, struct net_device *dev);
#endif
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
#define PREFIX_ORIGN_DFT	0
#define PREFIX_ORIGN_SLLA	1
#define PREFIX_ORIGN_DHCP	2
#define PREFIX_ORIGN_STATIC	3
#define PREFIX_ORIGN_NONE	4

#define INET6_S16_ADDR_BYTES	8
#define MAX_PD_PREFIX 	64
#endif

#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
extern int generate_prefix(struct sk_buff *skb ,struct prefix_info * pinfo_ptr);
#endif
extern int ecnt_send_event(unsigned short type,	struct ecnt_event_data *event_data, int event_data_len);

static inline void ecnt_udpv6_recvmsg_inline_hook
(struct msghdr *msg, struct sock *sk, struct sk_buff *skb, struct inet_sock *inet, int is_udp4)
{
	if ( !is_udp4 && skb->protocol == htons(ETH_P_IPV6) )
	{
		if ( inet->cmsg_flags & IP_CMSG_SKB_MARK )
		{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
			ip_cmsg_recv_offset(msg, sk, skb, 0, 0);
#else
			ip_cmsg_recv(msg, skb); 
#endif
		}
	}

	return;
}

static inline int ecnt_do_ipv6_setsockopt_inline_hook
(struct sock *sk, int level, struct net *net, int usrval,
int optname, char __user *optval, unsigned int optlen, int *err)
{
	int val = usrval;

	if ( optlen < sizeof(int) )
	{
		if (optlen >= sizeof(__u16))
		{
			__u16 u16val;
			if (get_user(u16val, (__u16 __user *) optval))
				return ECNT_HOOK_ERROR;
			val = (int) u16val;
		}
		else if ( optlen >= sizeof(char) )
		{
			unsigned char ucval;
		
			if ( get_user(ucval, (unsigned char __user *) optval) )
			{
				*err = -EFAULT;
				return ECNT_HOOK_ERROR;
			}
			val = (int) ucval;
		}
	}

	switch ( optname )
	{
		case IPV6_SKB_MARK:
			*err = -EINVAL;
			if ( optlen < sizeof(int) )
				break;
			sk->sk_mark = val;
			*err = 0;
			break;
		case IPV6_SKB_VLAN_ID:
			*err = -EINVAL;
			if ( optlen < sizeof(__be16) )
				break;
			sk->lVlanId = val;
			*err = 0;
			break;
		default:
			break;
	}

	return ECNT_CONTINUE;
}

static inline int ecnt_do_ipv6_getsockopt_inline_hook
(struct sock *sk, int level, int optname, int *val,
char __user *optval, int __user *optlen, unsigned int flags)
{
	int len = 0;

	switch ( optname )
	{
		case IPV6_SKB_VLAN_ID:
			*val = sk->lVlanId;;
			break;
		default:
			return ECNT_CONTINUE;
	}

	return ECNT_RETURN;
}


#if defined(TCSUPPORT_IPV6_CELOGO)
static int last_reply_icmp_id = 0;
#endif
static inline int ecnt_icmpv6_push_pending_frames_inline_hook(struct icmp6hdr *thdr, struct icmp6hdr *icmp6h)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	if(thdr->icmp6_type = ICMPV6_ECHO_REPLY){
		last_reply_icmp_id = icmp6h->icmp6_identifier;
	}
#endif
	return ECNT_CONTINUE;  
}

static inline int ecnt_packet_too_big_validate_inline_hook(struct icmp6hdr *hdr)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	struct icmp6hdr *tmp_icmp6hdr = NULL;
	struct ipv6hdr *tmp_ip6hdr = NULL;

	tmp_ip6hdr = (char *)hdr + 8;
	if(NEXTHDR_ICMP != tmp_ip6hdr->nexthdr)
		return ECNT_CONTINUE;

	tmp_icmp6hdr = (char *)tmp_ip6hdr + sizeof(struct ipv6hdr);

	if(ICMPV6_ECHO_REPLY != tmp_icmp6hdr->icmp6_type)
		return ECNT_CONTINUE;

	if(last_reply_icmp_id != tmp_icmp6hdr->icmp6_identifier){
		return ECNT_RETURN;
	}

#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_ipv6_rcv_inline_hook(struct sk_buff *skb)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	int offset;
	__u8 nexthdr = ipv6_hdr(skb)->nexthdr;
	__be16 frag_off;
	struct icmp6hdr *icmphdr;

	if(nexthdr != IPPROTO_FRAGMENT)
		return ECNT_CONTINUE;
	
	offset = ipv6_skip_exthdr(skb, sizeof(struct ipv6hdr),
								  &nexthdr, &frag_off);
	if (offset < 0)
		return ECNT_CONTINUE;

	if (nexthdr != IPPROTO_ICMPV6) 
		return ECNT_CONTINUE;

	icmphdr = (struct icmp6hdr *)(skb_network_header(skb) + offset);
	if(icmphdr->icmp6_type < 133 || icmphdr->icmp6_type > 137)
		return ECNT_CONTINUE;
	
	return ECNT_RETURN;
#else
	return ECNT_CONTINUE;
#endif
}

static inline int ecnt_ip6_expire_frag_queue_inline_hook(struct frag_queue *fq)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	if(fq->discard_flag)
		return ECNT_RETURN;
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_ip6_rt_update_pmtu_inline_hook(u32 mtu)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	if (mtu < IPV6_MIN_MTU)
		return ECNT_RETURN;
#endif
	return ECNT_CONTINUE;
}


#ifdef TCSUPPORT_IPV6_ENHANCEMENT
#ifdef TCSUPPORT_FH_UNIFIED_PLATFORM

extern wan_virtualdev_ifname wan_virtualdev_ifname_info[16];

static inline  int isEtherWanVirInterface(char *name)
{
	int i = 0;

	for(i = 0; i < 16; i++){
		if(wan_virtualdev_ifname_info[i].valid == 0)
			continue;
		if(strncmp(name, wan_virtualdev_ifname_info[i].virtual_name, 16) == 0)
			return 1;
	}

	return 0;
}
#endif
static inline int is_wan_dev(struct net_device *dev)
{
	return (strstr(dev->name, "nas") != NULL 
#if defined(TCSUPPORT_FH_ENV) || defined(TCSUPPORT_IS_FH_PON)
		|| strstr(dev->name, "pon0.") != NULL
#if defined(TCSUPPORT_FH_UNIFIED_PLATFORM)	
		|| (isEtherWanVirInterface(dev->name)) 
#endif		
#endif
#if defined(TCSUPPORT_OPENWRT) || defined(RDKB_BUILD)
			|| strstr(dev->name, "pon") != NULL 
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
			|| strstr(dev->name, "ae_wan") != NULL 
#endif
#endif
		|| strstr(dev->name, "ppp") != NULL);
}
#endif


static inline int ecnt_ndisc_router_discovery_if_inline_hook(struct inet6_dev *in6_dev)
{
#ifdef TCSUPPORT_IPV6_ENHANCEMENT	
		/*Enable WAN interface to receive RA for SLAAC mode*/
	if (!is_wan_dev(in6_dev->dev) || !in6_dev->cnf.accept_ra) 
		return ECNT_CONTINUE;
	else 
		return ECNT_RETURN_DROP;
#else
	if (!ipv6_accept_ra(in6_dev)) 
		return ECNT_CONTINUE;
	else 
		return ECNT_RETURN_DROP;
#endif
	
}


static inline int ecnt_ndisc_router_discovery_inline_hook
(struct sk_buff *skb,struct inet6_dev *in6_dev,struct ndisc_options ndopts, struct neighbour *neigh)
{
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
	struct prefix_info *pinfo_ptr = NULL;
	u16 pinfo_size = 0;
		
	/*using TR069 IPv6Address.ChildPrefixBits defined to generate a new prefix*/
	if(in6_dev->cnf.accept_ra_pinfo && !ndopts.nd_opts_pi)
	{
		if((in6_dev->cnf.child_prefix_orign == PREFIX_ORIGN_SLLA)
			&& (in6_dev->cnf.parent_pd_prefix[0] != '\0')
			&& (in6_dev->cnf.child_prefix[0] != '\0')){
			pinfo_size = sizeof(struct prefix_info);
			pinfo_ptr = kmalloc( pinfo_size, GFP_ATOMIC);
			if(pinfo_ptr == NULL){
				printk("[%s]---kmalloc fail!,[%d] \n",__FUNCTION__, __LINE__);
				goto childprefix_end;
	
			}
			pinfo_ptr->type = ND_OPT_PREFIX_INFO;
			pinfo_ptr->length = pinfo_size; //no using
			pinfo_ptr->onlink = 1;
			pinfo_ptr->autoconf = 1;
			pinfo_ptr->reserved = 0;
			pinfo_ptr->valid = 172800; // 2 day
			pinfo_ptr->prefered = 86400; // 1 day
			pinfo_ptr->reserved2 = 0;
				
			if(generate_prefix(skb, pinfo_ptr) == 0){
				addrconf_prefix_rcv(skb->dev, (u8*)pinfo_ptr, pinfo_size,
					ndopts.nd_opts_src_lladdr != NULL);
			}
			if(pinfo_ptr != NULL){
				kfree(pinfo_ptr);
			}
		}
	}
	childprefix_end:
				//nothing
#endif

	
#if defined(TCSUPPORT_IPV6_ENHANCEMENT) || defined(TCSUPPORT_IPV6_CEROUTER)
	if(neigh){
#if defined(TCSUPPORT_IPV6_CEROUTER)
			struct ra_msg *ra_msg = (struct ra_msg *)skb_transport_header(skb);
			if ( 0 == ntohs(ra_msg->icmph.icmp6_rt_lifetime) )
			{
				neigh->parms->ra_flags |= RA_FLAGS_ZEROLIFE;
			}
			else
			{
				neigh->parms->ra_flags &= (~RA_FLAGS_ZEROLIFE);
			}
#endif

			/*Add for outputing default gateway by RA*/
	sprintf(neigh->parms->dlf_route, NIP6_FMT, NIP6(ipv6_hdr(skb)->saddr));
	}

#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_addrconf_prefix_rcv_inline_hook
(struct inet6_dev *in6_dev,struct prefix_info *pinfo,struct in6_addr addr)
{
#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
	/*record the ra prefix info */
	sprintf(in6_dev->cnf.slaac_prefix, NIP6_FMT"/%d", NIP6(pinfo->prefix), pinfo->prefix_len);
#endif

#ifdef TCSUPPORT_IPV6_ENHANCEMENT
	/*Add for outputing slaac address by RA*/
	sprintf(in6_dev->cnf.slaac_addr, NIP6_FMT" %d", NIP6(addr), pinfo->prefix_len);
#endif
	return ECNT_CONTINUE;
}
static inline int ecnt_addrconf_prefix_rcv_inline_hook1
(struct inet6_dev *in6_dev,struct prefix_info *pinfo)
{
#if defined(TCSUPPORT_IPV6_CEROUTER) 
	/*record the ra prefix info */
	sprintf(in6_dev->cnf.slaac_prefix, NIP6_FMT"/%d", NIP6(pinfo->prefix), pinfo->prefix_len);
	if(pinfo->onlink)
		in6_dev->cnf.slaac_lflag = 1;
	else
		in6_dev->cnf.slaac_lflag = 0;
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_addrconf_dad_complete_if_inline_hook(struct inet6_ifaddr *ifp)
{
#ifdef TCSUPPORT_IPV6_ENHANCEMENT
		/*Enable WAN interface to send RS for SLAAC mode*/
		if(is_wan_dev(ifp->idev->dev) )
			return ECNT_CONTINUE;
		else
			return ECNT_RETURN_DROP;
#else
		if(ipv6_accept_ra(ifp->idev))
			return ECNT_CONTINUE;
		else
			return ECNT_RETURN_DROP;
#endif

}
static inline int ecnt_addrconf_dad_work_inline_hook(struct inet6_ifaddr *ifp)
{
#ifdef TCSUPPORT_IPV6_CEROUTER
	ifp->dad_nonce = 0;
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_addrconf_rs_timer_if_inline_hook(struct inet6_dev *idev)
{
#ifdef TCSUPPORT_IPV6_ENHANCEMENT
		/*Enable WAN interface to send RS for SLAAC mode*/
		if(!is_wan_dev(idev->dev))
			return ECNT_CONTINUE;
		else 
			return ECNT_RETURN_DROP;
#else
		if (!ipv6_accept_ra(idev))
			return ECNT_CONTINUE;
		else
			return ECNT_RETURN_DROP;
#endif

}

static inline int ecnt_addrconf_dev_config_if_inline_hook(struct net_device *dev, struct inet6_dev *idev)
{
#ifdef TCSUPPORT_IPV6_ENHANCEMENT
		struct in6_addr addr;
		//Disable lan device add linklocal address,except br0
			if(is_wan_dev(dev) || (strstr(dev->name, "br") != NULL)){
				memset(&addr, 0, sizeof(struct in6_addr));
				addr.s6_addr32[0] = htonl(0xFE800000);
				if (ipv6_generate_eui64(addr.s6_addr + 8, dev) == 0)
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
					addrconf_add_linklocal(idev, &addr, 0);
#else
					addrconf_add_linklocal(idev, &addr);
#endif
			}
			return ECNT_RETURN_DROP;
#else
	return ECNT_CONTINUE;
#endif

}

static inline int ecnt_addrconf_verify_rtnl_inline_hook(struct inet6_ifaddr *ifp, unsigned long age)
{
	struct ecnt_ipv6_addrinfo_data data;
	int evt_code = ((ECNT_EVENT_SYSTEM << 8) | ECNT_EVENT_V6_ADDR_TIMEOUT);

	memset(&data, 0, sizeof(data));
	if ( ifp->valid_lft != 0xFFFFFFFF && age >= ifp->valid_lft )
	{
		/* addr life time end. */
		data.st_code = EVT_ADDR_TIMEOUT;

		snprintf(data.dev_name, sizeof(data.dev_name), "%s", ifp->idev->dev->name);
		memcpy(&data.in6_u_val, &ifp->addr, sizeof(data.in6_u_val));
		ecnt_send_event(evt_code, &data, sizeof(data));
	}

	return ECNT_CONTINUE;
}

static inline void ecnt_rawv6_sendmsg_inline_hook(struct flowi6 *fl6, int addr_len, int sin6_scope_id, int type)
{
#ifdef TCSUPPORT_IPV6_CELOGO
	if (addr_len >= sizeof(struct sockaddr_in6) 
		&& sin6_scope_id 
		&& ((type & IPV6_ADDR_LINKLOCAL) || (type & IPV6_ADDR_MULTICAST)))
		fl6->flowi6_oif = sin6_scope_id;
#endif
}


static inline int ecnt_ip6_pol_route_inline_hook(int *strict)
{
#if defined(TCSUPPORT_IPV6_CEROUTER)
	*strict |= RT6_LOOKUP_F_REACHABLE;
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_ndisc_recv_rs_inline_hook(struct inet6_dev *idev)
{
#if defined(TCSUPPORT_IPV6_CEROUTER)
  	if(is_wan_dev(idev->dev))
		return ECNT_RETURN_DROP;
#endif
	return ECNT_CONTINUE;
}
static inline int ecnt___ip6_rt_update_pmtu_inline_hook(int allfrag,struct rt6_info *rt6)
{
#if defined(TCSUPPORT_IPV6_CEROUTER)
	if(allfrag == 1)
	{
		unsigned int flag =dst_metric_raw(&rt6->dst,RTAX_FEATURES);
		dst_metric_set(&rt6->dst, RTAX_FEATURES, flag | RTAX_FEATURE_ALLFRAG);
	}
#endif
	return ECNT_CONTINUE;
}
static inline int ecnt_icmp6_send_inline_hook(struct flowi6 *fl6,u8 code)
{
#if defined(TCSUPPORT_IPV6_CEROUTER)
	if(code ==ICMPV6_POLICY_FAIL && fl6 != NULL)
	{
		fl6->flowi6_mark |= POLICY_FAIL_MASK;
	}
#endif
	return ECNT_CONTINUE;
}

#ifndef AIROHA_BSP
#include "ecnt_net_ipv6_sdk.h"
#endif/*AIROHA_BSP*/

#endif/*_LINUX_ECNT_NET_IPV6_H*/