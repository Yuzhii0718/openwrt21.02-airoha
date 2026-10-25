#ifndef _LINUX_ECNT_NET_H
#define _LINUX_ECNT_NET_H
#include <linux/version.h>
#include <uapi/linux/in.h>
#include <uapi/linux/ecnt_in.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <net/sock.h>
#include <net/inet_sock.h>
#include <ecnt_hook/ecnt_hook.h>
#include <lan_port/lan_port_info.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,9,263)	
#define IP_CMSG_CUSTOM_DEFINE_START      8
#elif LINUX_VERSION_CODE > KERNEL_VERSION(3,19,8)	
#define IP_CMSG_CUSTOM_DEFINE_START      7
#else
#define IP_CMSG_CUSTOM_DEFINE_START      6
#endif

#define IP_CMSG_SKB_MARK    BIT(IP_CMSG_CUSTOM_DEFINE_START+1)
#define IP_CMSG_VLAN_ID		BIT(IP_CMSG_CUSTOM_DEFINE_START+2)

/*for alg switch*/
extern int nf_conntrack_ftp_enable;
extern int nf_conntrack_sip_enable;
extern int nf_conntrack_h323_enable;
extern int nf_conntrack_rtsp_enable;
extern int nf_conntrack_l2tp_enable;
extern int nf_conntrack_ipsec_enable;
extern int nf_conntrack_pptp_enable;
extern int nf_conntrack_portscan_enable;
extern int nf_conntrack_ftp_port;
extern int nf_conntrack_esp_timeout;
extern int nf_conntrack_rtcp_enable;
extern unsigned int nf_conntrack_rtsp_src_ip4_mask;
extern int ecnt_shortcut_app_list_check(char * name);

static inline int ecnt_skbmark_to_sockmark_copy
(struct sock *sk, struct sk_buff *skb)
{
	if ( !sk || !skb )
		return ECNT_CONTINUE;

	sk->sk_mark &= (~(LANIF_MASK));
	/* not change QoS info stored into sk->sk_mark */
	sk->sk_mark |= (skb->mark & (~QOS_FILTER_MARK));

#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_LANMGR_NG)
	if ( sk->lPbit & SOCK_TYPE_RCV_MAC )
	{
		memcpy(sk->hwaddr, eth_hdr(skb)->h_source, 6);
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_sock_create_inline_hook(struct socket *sock)
{
#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
	if(strcmp(current->comm, "smbd") == 0){
		sock->sk->sk_smbd_info.smbd_sk = 1;
	}else{
		sock->sk->sk_smbd_info.smbd_sk = 0;
	}
#endif

	return ECNT_CONTINUE;
}


static inline int ecnt_sock_accept4_inline_hook(struct socket *sock)
{
    if (ecnt_shortcut_app_list_check(current->comm))
    {
        sock->sk->sk_shortcut_info.shortcut_sk = 1;
        sock->sk->sk_shortcut_info.shortcut_speed = 0;
        sock->sk->sk_shortcut_info.out_dev = NULL;		
    }
    else
    {
        sock->sk->sk_shortcut_info.shortcut_sk = 0;
        sock->sk->sk_shortcut_info.shortcut_speed = 0;
        sock->sk->sk_shortcut_info.out_dev = NULL;	
    }
	return ECNT_CONTINUE;
}
#ifndef AIROHA_BSP
#include "ecnt_net_sdk.h"
#endif/*AIROHA_BSP*/
#endif

