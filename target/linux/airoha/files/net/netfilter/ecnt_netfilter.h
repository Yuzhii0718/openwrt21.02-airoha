#ifndef _LINUX_ECNT_NETFILTER_H
#define _LINUX_ECNT_NETFILTER_H
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

#if defined(TCSUPPORT_CT_MAXNET_DPI)
extern int (*dpi_core_nf_event_hook)(enum ip_conntrack_events event, struct nf_conn *ct);
#endif
#if defined(TCSUPPORT_CT_APPROUTE)
extern int (*approute_core_nf_event_hook)(enum ip_conntrack_events event, struct nf_conn *ct, struct sk_buff *skb);
#endif

#ifdef TCSUPPORT_DS_HWNAT_OFFLOAD
extern int rtsp_hwnat_offload;
#endif

extern int (*ra_sw_nat_hook_ipv6_nf_conntrack_hook)(struct nf_conntrack* nf);
#define ECNT_NF_CT_SYSCTL_TABLE_HOOK \	
{\
	.procname	= "nf_conntrack_ftp_enable",\
	.data		= &nf_conntrack_ftp_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for ftp alg switch */ \
{\
	.procname	= "nf_conntrack_sip_enable",\
	.data		= &nf_conntrack_sip_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for sip alg switch */ \
{\
	.procname	= "nf_conntrack_h323_enable",\
	.data		= &nf_conntrack_h323_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for h323 alg switch */ \
{\
	.procname	= "nf_conntrack_rtsp_enable",\
	.data		= &nf_conntrack_rtsp_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for rtsp alg switch */ \
{\
	.procname	= "nf_conntrack_l2tp_enable",\
	.data		= &nf_conntrack_l2tp_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for l2tp alg switch */ \
{\
	.procname	= "nf_conntrack_ipsec_enable",\
	.data		= &nf_conntrack_ipsec_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for ipsec alg switch */ \
{\
	.procname	= "nf_conntrack_pptp_enable",\
	.data		= &nf_conntrack_pptp_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for pptp alg switch */ \
{\
	.procname	= "nf_conntrack_portscan_enable",\
	.data		= &nf_conntrack_portscan_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for ftp port */ \
{\
	.procname	= "nf_conntrack_ftp_port",\
	.data		= &nf_conntrack_ftp_port,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for portscan switch */ \
{\
	.procname	= "nf_conntrack_esp_timeout",\
	.data		= &nf_conntrack_esp_timeout,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for esp unreply ct timeout */ \
{\
	.procname	= "nf_conntrack_rtcp_enable",\
	.data		= &nf_conntrack_rtcp_enable,\
	.maxlen 	= sizeof(int),\
	.mode		= 0644,\
	.proc_handler	= &proc_dointvec,\
},	/* for rtcp alg switch */ \
{\
	.procname	= "nf_conntrack_rtsp_src_ip4_mask",\
	.data		= &nf_conntrack_rtsp_src_ip4_mask,\
	.maxlen 	= sizeof(unsigned int),\
	.mode		= 0644,\
	.proc_handler	= &proc_doulongvec_minmax,\
},	/* for rtsp src ip mask */




static inline void ecnt_init_conntrack_inline_hook
(struct net *net, struct sk_buff *skb, struct nf_conn *ct)
{
#if defined(TCSUPPORT_CT_VLAN_BIND)
	ct->ct_general.lVlanId = VBIND_INVALID_VLANID;
	ct->ct_general.foe_index = 0xFFFF;
	if ( ROUTING_MODE_VLAN_PACKET 
		== (skb->vlan_tag_flag & ROUTING_MODE_VLAN_PACKET)
		&& (skb->vlan_tags[0] < VBIND_INVALID_VLANID))
	{
		if(htons(ETH_P_IPV6) == skb->protocol && (skb->vlan_tags[0] == 0))
			ct->ct_general.lVlanId = VBIND_INVALID_VLANID;
		ct->ct_general.lVlanId = VTAG_GET_VID(skb->vlan_tags[0]);
	}
#endif
#if defined(TCSUPPORT_CT_MAXNET_DPI)
	if ( dpi_core_nf_event_hook )
		dpi_core_nf_event_hook(master_ct(ct) ? IPCT_RELATED : IPCT_NEW, ct);
#endif
#if defined(TCSUPPORT_CT_APPROUTE)
	ct->ct_general.dscp_up		= NF_DSCP_DEF;
	ct->ct_general.dscp_dw		= NF_DSCP_DEF;
	ct->ct_general.up_mark		= 0;
	ct->ct_general.match_flag	= 0;
	if ( approute_core_nf_event_hook )
		approute_core_nf_event_hook(master_ct(ct) ? IPCT_RELATED : IPCT_NEW, ct, skb);
#endif

	ct->loport = 0;
	ct->hiport = 0;
	return;
}

static inline int ecnt_nf_conntrack_ftp_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	/*for FTP ALG switch*/
	if ( !nf_conntrack_ftp_enable )
		return ECNT_RETURN; /*ftp switch is off, just accept packet and do not do ALG */

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_sip_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	/*for SIP ALG switch*/
	if ( !nf_conntrack_sip_enable )
		return ECNT_RETURN; /* sip switch is off, just accept packet and do not do ALG */

	if(!((ct->status) & IPS_NAT_MASK))
		return ECNT_RETURN;
        
	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_sip_help_tcp_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	int ret = 0;

	ret = ecnt_nf_conntrack_sip_help_inline_hook(skb, ct);
	if ( ECNT_CONTINUE !=  ret)
		return ret;

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_sip_help_udp_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	int ret = 0;

	ret = ecnt_nf_conntrack_sip_help_inline_hook(skb, ct);
	if ( ECNT_CONTINUE !=  ret)
		return ret;

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_h323_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	/* for H.323 ALG switch */
	if ( !nf_conntrack_h323_enable )
		return ECNT_RETURN; /*h323 switch is off, just accept packet and do not do ALG  */

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_h245_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	int ret = 0;

	ret = ecnt_nf_conntrack_h323_help_inline_hook(skb, ct);
	if ( ECNT_CONTINUE !=  ret)
		return ret;

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_q31_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	int ret = 0;

	ret = ecnt_nf_conntrack_h323_help_inline_hook(skb, ct);
	if ( ECNT_CONTINUE !=  ret)
		return ret;

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_ras_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	int ret = 0;

	ret = ecnt_nf_conntrack_h323_help_inline_hook(skb, ct);
	if ( ECNT_CONTINUE !=  ret)
		return ret;

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_rtsp_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	/* for RTSP ALG switch */
	if ( !nf_conntrack_rtsp_enable )
		return ECNT_RETURN; /* rtsp switch is off, just accept packet and do not do ALG */

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_rtcp_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	/* for RTCP ALG switch */
	if ( !nf_conntrack_rtcp_enable )
		return ECNT_RETURN; /* rtcp switch is off, just accept packet and do not do ALG */

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_pptp_help_inline_hook
(struct sk_buff *skb, struct nf_conn *ct)
{
	/* for PPTP ALG switch */
	if ( !nf_conntrack_pptp_enable )
		return ECNT_RETURN; /* pptp switch is off, just accept packet and do not do ALG */

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_alloc_inline_hook
(struct net *net, u16 zone,
 const struct nf_conntrack_tuple *orig,
 const struct nf_conntrack_tuple *repl,
 gfp_t gfp, u32 hash, struct nf_conn *ct)
{
#if defined(TCSUPPORT_CT_MAXNET_DPI)
	if ( ct )
	{
        ct->layer7_id = 0;
        ct->dpi_context = NULL;
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_nf_conntrack_free_inline_hook
(struct nf_conn *ct)
{
#if defined(TCSUPPORT_CT_MAXNET_DPI)
	if ( dpi_core_nf_event_hook )
		dpi_core_nf_event_hook(IPCT_DESTROY, ct);
#endif

	return ECNT_CONTINUE;
}

static inline void ecnt_resolve_normal_ct_inline_hook
(struct nf_conn_help *help, struct sk_buff *skb, struct nf_conntrack_tuple_hash *h)
{
	struct nf_conn *ct;
	
	ct = nf_ct_tuplehash_to_ctrack(h);

	if(!strncmp(help->helper->name, "rtspv6",6)&& (NF_CT_DIRECTION(h) == IP_CT_DIR_REPLY)&&(ct->status&IPS_NAT_MASK))
	{
		if(ra_sw_nat_rtspv6_npt_data_handle)
			ra_sw_nat_rtspv6_npt_data_handle(skb);
		return;
	}
	
#ifdef TCSUPPORT_DS_HWNAT_OFFLOAD
	if(!(rtsp_hwnat_offload
        &&(strcmp(help->helper->name, "rtsp") == 0)
        && (NF_CT_DIRECTION(h) == IP_CT_DIR_REPLY))){
#endif
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
#ifdef TCSUPPORT_DS_HWNAT_OFFLOAD
	}
#endif

	return;
}

static inline void ecnt_nf_conntrack_rtsp_help_reply_inline_hook
(struct sk_buff * skb, char *rb_ptr, unsigned int datalen)
{
#ifdef TCSUPPORT_DS_HWNAT_OFFLOAD
	if(ra_sw_nat_rtsp_data_handle)
		ra_sw_nat_rtsp_data_handle(skb, rb_ptr, datalen);
#endif

	return;
}

/*
* function nf_nat_mangle_tcp_packet() can only adjust sequence once for one 
* skb data, add ecnt_nf_ct_seqadj_set_inline_hook for change offset after 
nf_nat_mangle_tcp_packet
*/
static inline int
ecnt_nf_ct_seqadj_set_inline_hook
(struct nf_conn *ct, enum ip_conntrack_info ctinfo,
__be32 seq, s32 off)
{
	struct nf_conn_seqadj *seqadj = nfct_seqadj(ct);
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);
	struct nf_ct_seqadj *this_way = NULL;

	if (unlikely(!seqadj)) {
		WARN_ONCE(1, "Missing nfct_seqadj_ext_add() setup call\n");
		return 0;
	}

	if ( 0 == off )
		return 0;

	set_bit(IPS_SEQ_ADJUST_BIT, &ct->status);

	spin_lock_bh(&ct->lock);

	/* only HOOK TX, so use thisway direction. */
	this_way  = &seqadj->seq[dir];
	/* increase mode */
	if ( this_way->correction_pos == ntohl(seq) )
	{
		this_way->offset_after += off;
	}

	spin_unlock_bh(&ct->lock);
	return 0;
}

static inline void ecnt_nf_conntrack_rtsp_help_out_inline_hook
(struct nf_conntrack_expect *exp, struct nf_conn *ct)
{
    if(nf_conntrack_rtsp_src_ip4_mask != 0xFFFFFFFF)
    {
        if(AF_INET == nf_ct_l3num(ct))
            memcpy(&exp->mask.src.u3, &nf_conntrack_rtsp_src_ip4_mask, 4);
    }
    
	return;
}

extern int ipv6_skip_exthdr(const struct sk_buff *skb, int start, u8 *nexthdrp,__be16 *frag_offp);
static inline void ecnt_get_skb_tcpdata_inline_hook(struct sk_buff* skb, char** pptcpdata, uint* ptcpdatalen)
{
	struct ipv6hdr* ip6h = NULL;
	struct tcphdr*  tcph = NULL;
	int offset;
	unsigned char nexthdr;
	unsigned short frag_off;
	
	if(skb->protocol != htons(ETH_P_IPV6))
		return;

	ip6h = ipv6_hdr(skb);
	if(IPPROTO_HOPOPTS == ip6h->nexthdr)
	{
		nexthdr = ip6h->nexthdr;
		offset = ipv6_skip_exthdr(skb, sizeof(struct ipv6hdr), &nexthdr, &frag_off);
		tcph = (struct tcphdr*)((unsigned char *)ip6h + offset);
	}
	else
		tcph = (struct tcphdr*)((unsigned char *)ip6h + sizeof(struct ipv6hdr));

	*pptcpdata = (char*)tcph +  tcph->doff*4;
	*ptcpdatalen = ((char*)skb_transport_header(skb) + skb->len) - *pptcpdata;

	return;
}

static inline int ecnt_tcp_in_window_inline_hook(void)
{
#ifdef TCSUPPORT_RA_HWNAT
	return ECNT_RETURN;
#else
	return ECNT_CONTINUE;
#endif
}

#ifndef AIROHA_BSP
#include "ecnt_netfilter_sdk.h"
#endif/*AIROHA_BSP*/

#endif

