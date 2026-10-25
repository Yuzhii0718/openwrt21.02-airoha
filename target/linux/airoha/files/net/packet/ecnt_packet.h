#ifndef _LINUX_ECNT_PACKET_H
#define _LINUX_ECNT_PACKET_H
/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************/
#include <linux/foe_hook.h>
#include <ecnt_hook/ecnt_hook.h>
#include <linux/ecnt_in.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************/


/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A   T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************/
/****************************************************************************
**function name
	 __vlan_proto
**description:
	get protocol via skb
**return
	eth_type
**parameter:
	skb: the packet information
****************************************************************************/
static inline __be16 __vlan_proto(const struct sk_buff *skb)
{
	return vlan_eth_hdr(skb)->h_vlan_encapsulated_proto;
}

/*TCSUPPORT_MULTICAST_SPEED start*/
#ifdef TCSUPPORT_RA_HWNAT
	extern int (*hwnat_set_recover_info_hook)(struct sk_buff* skb,	struct sock* sk,int flag);
#else
	static int (*hwnat_set_recover_info_hook)(struct sk_buff* skb,	struct sock* sk,int flag) = NULL;
#endif
/*TCSUPPORT_MULTICAST_SPEED end*/


#ifdef  TCSUPPORT_PON_VLAN
extern int (*pon_check_tpid_hook)(__u16 * buf);
#endif
#if defined(TCSUPPORT_CT_PON)
extern unsigned int hw_igmp_flood_enable;
extern unsigned int g_snooping_enable;
#endif

#define FOEINFOSTORE 0
#define FOEINFORECOVER 1
#define RECEIVE_OR_SEND 0
#define SET_OR_GET_SOCKOPT 1

static void foe_info_op(struct sock *sk,void *ptr,int opmode,int direction)
{
	struct sk_buff * skb;
	struct SkbFoeInfo *skbfoeinfo;

	switch(opmode)
	{
		case RECEIVE_OR_SEND:
			skb = (struct sk_buff *)ptr;
			if(hwnat_set_recover_info_hook)
				hwnat_set_recover_info_hook(skb,sk,direction);
			break;

		case SET_OR_GET_SOCKOPT:
			skbfoeinfo = (struct SkbFoeInfo *)ptr;
			if(FOEINFORECOVER == direction)
			{
				skbfoeinfo->ppe_ai = sk->sk_foe_info.ppe_ai;
				skbfoeinfo->ppe_foe_entry = sk->sk_foe_info.ppe_foe_entry;
				skbfoeinfo->ppe_magic = sk->sk_foe_info.ppe_magic;
				skbfoeinfo->wan_type= sk->sk_foe_info.wan_type;
				skbfoeinfo->wan_index = sk->sk_foe_info.wan_index;
			}
			else
			{
				sk->sk_foe_info.ppe_ai = skbfoeinfo->ppe_ai;
				sk->sk_foe_info.ppe_foe_entry = skbfoeinfo->ppe_foe_entry;
				sk->sk_foe_info.ppe_magic = skbfoeinfo->ppe_magic;
				sk->sk_foe_info.wan_type = skbfoeinfo->wan_type;
				sk->sk_foe_info.wan_index = skbfoeinfo->wan_index;
			}	
			break;	
			
		default:
			break;
	}
}

static inline void ecnt_packet_snd_inline_hook(struct sock *sk,void *ptr,int opmode,int direction)
{
#ifdef TCSUPPORT_MULTICAST_SPEED
#if defined(TCSUPPORT_CT_PON)
	char devName[IFNAMSIZ] = {0};
	int wan_name_get = 0;
	struct net_device *dev = NULL;
	struct sk_buff *skb = NULL;
#endif


#if defined(TCSUPPORT_CT_PON)
	if ( FOEINFORECOVER == direction
		&& sk && !g_snooping_enable && hw_igmp_flood_enable )
	{
		/* ppp interface */
		if ( 1 == sk->sk_foe_info.wan_type )
		{
			snprintf(devName, sizeof(devName) - 1, "ppp%d"
					, sk->sk_foe_info.wan_index);
			wan_name_get = 1;
		}
		/* nas interface */
		else if ( 2 == sk->sk_foe_info.wan_type )
		{
			snprintf(devName, sizeof(devName) - 1, "nas%d_%d"
							, sk->sk_foe_info.wan_index / MAX_SMUX_NUM
							, sk->sk_foe_info.wan_index % MAX_SMUX_NUM);
			wan_name_get = 1;
		}

		if ( wan_name_get )
		{
			skb = (struct sk_buff *)ptr;
			dev = dev_get_by_name(&init_net, devName);

			if ( dev && skb )
			{
#ifdef TCSUPPORT_PORTBIND
				skb->portbind_mark |= MASK_ORIGIN_DEV;
				skb->orig_dev = dev;
#endif
				dev_put(dev);
			}
		}
	}
#endif
	if(sk)
		foe_info_op(sk , ptr , opmode , direction);
#endif

	return ;
}

static inline void ecnt_packet_recvmsg_inline_hook(struct sock *sk,void *ptr,int opmode,int direction)
{
	struct sk_buff * skb = (struct sk_buff *)ptr;

#ifdef TCSUPPORT_MULTICAST_SPEED
	foe_info_op(sk , ptr , opmode , direction);
#endif
#if defined(TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_LANMGR_NG)
	if ( sk->lPbit & SOCK_TYPE_COPY_MARK )
	{
		sk->sk_mark = (skb->mark & 0xfffffffe);
		if ( 1 == skb->lan_vlan_tci_valid )
			sk->sk_mark |= 0x1;
	}
#endif

	return ;
}

void ecnt_packet_rcv_inline_hook(struct sock *sk, struct sk_buff *skb, struct net_device *orig_dev)
{

#if defined(TCSUPPORT_PON_VLAN) 
	u16 *proto = NULL;
	if(orig_dev->name[0] == 'n')
	{
		proto = (u16*)(skb->data + 12);
		while(pon_check_tpid_hook && (pon_check_tpid_hook(proto) == 1))
		{
			memmove(skb->data + VLAN_HLEN, skb->data, 12);
			skb_pull(skb, VLAN_HLEN);
			proto = (u16*)(skb->data + 12);
		}
	}
#endif
#if defined (TCSUPPORT_CT_JOYME4) || defined(TCSUPPORT_LANMGR_NG)
	if ( sk->lPbit & SOCK_TYPE_COPY_MARK )
	{
		sk->sk_mark = (skb->mark & 0xfffffffe);
		if ( 1 == skb->lan_vlan_tci_valid )
			sk->sk_mark |= 0x1;
	}
#endif

	return;
}

void arht_packet_rcv_spkt_rcv_inline_hook(struct sock *sk, struct sk_buff *skb)
{
	return;
}

static inline int ecnt_packet_setsockopt_inline_hook(struct sock *sk,int optname, 
	char __user *optval,unsigned int optlen )
{
	switch (optname)
	{
#ifdef TCSUPPORT_MULTICAST_SPEED	
		case PACKET_SKB_FOE_INFO:
		{	
			struct SkbFoeInfo skbfoeinfo;
		
			if (optlen < sizeof(struct SkbFoeInfo))
				return ECNT_HOOK_ERROR;
		
			if (copy_from_user(&skbfoeinfo, optval, sizeof(struct SkbFoeInfo)))
				return ECNT_HOOK_ERROR;

			foe_info_op(sk,&skbfoeinfo,SET_OR_GET_SOCKOPT,FOEINFOSTORE);
			return ECNT_RETURN;
		}	
#endif
		default:
			return ECNT_CONTINUE ;
	}
	
	return ECNT_CONTINUE ;

}

static inline int ecnt_packet_getsockopt_inline_hook(struct sock *sk,int optname, 
	char __user *optval,int __user *optlen,int len, int lv,void *data )
{
	struct SkbFoeInfo skbfoeinfo;

	switch (optname)
	{
#ifdef TCSUPPORT_MULTICAST_SPEED	
		case PACKET_SKB_FOE_INFO:
		{
			memset(&skbfoeinfo, 0, sizeof(struct SkbFoeInfo));
			if (len > sizeof(struct SkbFoeInfo))
				len = sizeof(struct SkbFoeInfo);
		
			foe_info_op(sk,&skbfoeinfo,SET_OR_GET_SOCKOPT,FOEINFORECOVER);
			data = &skbfoeinfo;
			if (put_user(len, optlen))
				return ECNT_HOOK_ERROR;
			if (copy_to_user(optval, data, len))
				return ECNT_HOOK_ERROR;
			return ECNT_RETURN;
		}	
#endif
		default:
			return ECNT_CONTINUE ;
	}
		
	return ECNT_CONTINUE ;

}

#ifndef AIROHA_BSP
#include "ecnt_packet_sdk.h"
#endif/*AIROHA_BSP*/

#endif

