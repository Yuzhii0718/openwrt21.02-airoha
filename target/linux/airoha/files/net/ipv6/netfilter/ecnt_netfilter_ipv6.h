#ifndef _LINUX_ECNT_NETFILTER_IPV6_H
#define _LINUX_ECNT_NETFILTER_IPV6_H
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

#if defined(TCSUPPORT_IPV6_CELOGO)
extern struct frag_queue *fq_find(struct net *net, __be32 id,
					 u32 user, struct in6_addr *src,
					 struct in6_addr *dst, u8 ecn);
#endif

static inline int enct_ip6t_snpt_tg_inline_hook(struct sk_buff* skb)
{
	struct nf_nat_range range = {0};
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct;

	ct = nf_ct_get(skb, &ctinfo);
	if((ct && ctinfo == IP_CT_NEW) 
        &&(!nf_nat_initialized(ct, NF_NAT_MANIP_SRC)))
	{
		memcpy(&range.min_addr, &ipv6_hdr(skb)->saddr, sizeof(struct in6_addr));
		memcpy(&range.max_addr, &ipv6_hdr(skb)->saddr, sizeof(struct in6_addr));
		range.flags = NF_NAT_RANGE_MAP_IPS;

		nf_nat_setup_info(ct, &range, NF_NAT_MANIP_SRC);
	}

	return 0;
}

static inline int ecnt_nf_ct_frag6_queue_inline_hook
	(struct frag_queue *fq, struct sk_buff *skb, int offset, const struct frag_hdr *fhdr)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	struct sk_buff *head_skb;
	struct frag_hdr *head_fhdr;
	struct sk_buff *skb2;
	struct icmp6hdr *hdr;

	if(offset == 0 && fhdr->nexthdr == NEXTHDR_ICMP)
	{
		hdr = (struct icmp6hdr *)((unsigned char *)fhdr + sizeof(struct frag_hdr));
		if(!(hdr->icmp6_type & ICMPV6_INFOMSG_MASK))
		{
			hdr->icmp6_type |= ICMPV6_INFOMSG_MASK;
			skb2 = skb_copy(skb, GFP_ATOMIC);
			if(skb2){
				icmpv6_param_prob(skb2, 3, 0);
				return ECNT_RETURN;
			}	
		}
	}

	if(offset && fhdr->nexthdr == NEXTHDR_ICMP)
	{
		if(!fq->q.fragments)
			return ECNT_CONTINUE;

		head_skb = fq->q.fragments;
		head_fhdr = (struct frag_hdr *)skb_transport_header(head_skb);
		if(head_fhdr->nexthdr == NEXTHDR_ICMP)	
			return ECNT_CONTINUE;

		hdr = (struct icmp6hdr *)((unsigned char *)head_fhdr + sizeof(struct frag_hdr));
		hdr->icmp6_type |= ICMPV6_INFOMSG_MASK;
		skb2 = skb_copy(skb, GFP_ATOMIC);
		if(skb2){
			icmpv6_param_prob(skb2, 3, 0);
			return ECNT_RETURN;
		}	
	}
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_overlap_preceding_fragment_inline_hook(struct frag_queue *fq)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	fq->discard_flag = 1;
	return ECNT_RETURN;
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_overlap_succeeding_segment_inline_hook(struct frag_queue *fq)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	fq->discard_flag = 1;
	return ECNT_RETURN;
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_nf_ct_frag6_gather_inline_hook
	(struct frag_queue **fq_p, struct frag_hdr *fhdr, struct net *net, 
	u32 user, struct ipv6hdr *hdr, struct inet_frags *nf_frags)
{
#if defined(TCSUPPORT_IPV6_CELOGO)
	struct frag_queue *fq = *fq_p;
	if((fq->q.flags & INET_FRAG_FIRST_IN) &&
		(ntohs(fhdr->frag_off) & ~0x7) == 0)
	{
		inet_frag_kill(&fq->q, nf_frags);
		fq = fq_find(net, fhdr->identification, user, &hdr->saddr, &hdr->daddr,
			 (1 << (ipv6_get_dsfield(hdr) & INET_ECN_MASK)));
		if (fq == NULL) {
			pr_debug("Can't find and can't create new queue\n");
			return ECNT_RETURN;
		}
		*fq_p = fq;
	}
#endif
	return ECNT_CONTINUE;
}
#ifndef AIROHA_BSP
#include "ecnt_netfilter_ipv6_sdk.h"
#endif/*AIROHA_BSP*/
#endif

