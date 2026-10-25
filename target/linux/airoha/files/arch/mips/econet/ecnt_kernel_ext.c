/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/export.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/TCIfSetQuery_os.h>
#if defined(TCSUPPORT_CT_APPROUTE)
#include <linux/netfilter/nf_conntrack_common.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_acct.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,20)
#include <net/netfilter/nf_conntrack_l3proto.h>
#endif
#include <net/netfilter/nf_conntrack_l4proto.h>
#include <net/netfilter/nf_conntrack_zones.h>
#endif

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define NETIF_F_HW_VLAN_TX	128	/* Transmit VLAN hw acceleration, defined in 
								linux2.6.36/include/linux/netdevice.h*/
#define GDUMP_DATA_SEL_MASK		(0x3)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

enum gdump_sel
{
	PBUS_ACCESS_DATA,
	PBUS_ACCESS_LENGTH,
	GDUMP_ACCESS_DATA,
	FE_ACCESS_DATA,
	MAX_GDUMP_SEL_NUM
};


/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* add for fix compile error */
int g_port_reverse_kernel = 0;
EXPORT_SYMBOL(g_port_reverse_kernel);

int rtsp_hwnat_offload = 0;
EXPORT_SYMBOL(rtsp_hwnat_offload);

EXPORT_SYMBOL(find_task_by_vpid);

int wifi_led_stop = 0;
EXPORT_SYMBOL(wifi_led_stop);


char npu_stat = 0;  // there is no npu for mips.
EXPORT_SYMBOL(npu_stat);

#if defined(TCSUPPORT_CT_SWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
int (*sw_pktqosEnqueue) (struct sk_buff * bp) = NULL;
void (*sw_PKTQOS_SET_STOP) (void) = NULL;
void (*sw_PKTQOS_CLEAR_STOP) (void) = NULL;
int (*sw_isSWQosActive) (void) = NULL;
int (*sw_isHwnatOffloadEnable) (void) = NULL;
int (*sw_qosCreatPolicer) (int id, int bandwidth) = NULL;
int (*sw_qosDeletePolicer) (int id) = NULL;
int (*sw_qosOverallRatelimit) (int bandwidth) = NULL;
EXPORT_SYMBOL(sw_pktqosEnqueue);
EXPORT_SYMBOL(sw_PKTQOS_SET_STOP);
EXPORT_SYMBOL(sw_PKTQOS_CLEAR_STOP);
EXPORT_SYMBOL(sw_isSWQosActive);
EXPORT_SYMBOL(sw_isHwnatOffloadEnable);
EXPORT_SYMBOL(sw_qosCreatPolicer);
EXPORT_SYMBOL(sw_qosDeletePolicer);
EXPORT_SYMBOL(sw_qosOverallRatelimit);
#endif
#ifdef TCSUPPORT_USB_HOST_LED
void (*Usb_Led_Flash_Op_hook)(unsigned int opmode ,unsigned int phyport);
EXPORT_SYMBOL(Usb_Led_Flash_Op_hook);
int pre_usb_state[2] = {0, 0};
EXPORT_SYMBOL(pre_usb_state);
#endif

#if defined(TCSUPPORT_CT_ACCESSLIMIT)
struct sk_buff * (*al_main_check_hook)(struct sk_buff *skb, unsigned char ipVer);
EXPORT_SYMBOL(al_main_check_hook);
void (*al_handle_arp_reply_hook)(struct sk_buff *skb);
EXPORT_SYMBOL(al_handle_arp_reply_hook);
struct sk_buff * (*al_main_check_hook_ip6)(struct sk_buff *skb,unsigned char ipVer);
EXPORT_SYMBOL(al_main_check_hook_ip6);
#endif

#if defined(TCSUPPORT_CT_URL_FILTER)
int (*url_filter_check_hook)(struct sk_buff *skb);
EXPORT_SYMBOL(url_filter_check_hook);
#endif


#if defined(TCSUPPORT_CT_VLAN_TAG)
u32 	vtag_free_rx_cnt = 0;
u32 	vtag_free_tx_cnt = 0;

int (*check_vtag_ct_hook)(void);
int (*check_vtag_restore_ct_hook)(void);
int (*store_vtag_ct_hook)(struct sk_buff *skb, struct net_device *dev);
int (*restore_vtag_ct_hook)(struct sk_buff **pskb);
int (*handle_vtag_tx_ct_hook)(struct sk_buff **pskb);
int (*handle_vtag_rx_ct_hook)(struct sk_buff **pskb);

EXPORT_SYMBOL(check_vtag_ct_hook);
EXPORT_SYMBOL(check_vtag_restore_ct_hook);
EXPORT_SYMBOL(store_vtag_ct_hook);
EXPORT_SYMBOL(restore_vtag_ct_hook);
EXPORT_SYMBOL(handle_vtag_tx_ct_hook);
EXPORT_SYMBOL(handle_vtag_rx_ct_hook);
EXPORT_SYMBOL(vtag_free_rx_cnt);
EXPORT_SYMBOL(vtag_free_tx_cnt);
#endif

#ifdef TCSUPPORT_VLAN_TAG
int (*remove_vtag_hook)(struct sk_buff *skb, struct net_device *dev);
int (*insert_vtag_hook)(struct sk_buff **pskb);
//#if !defined(TCSUPPORT_FTP_THROUGHPUT)
int (*check_vtag_hook)(void);
//#endif
int (*get_vtag_hook)(struct net_device *dev, struct sk_buff *skb);
/*TCSUPPORT_ROUTEPOLICY_PRIOR_PORTBIND*/
struct net_device* (*tcvlan_get_outdev_hook)(struct sk_buff* skb);
/*END TCSUPPORT_ROUTEPOLICY_PRIOR_PORTBIND*/
/*TCSUPPORT_CPU_PERFORMANCE_TEST---start---*/
int (*get_pvid_by_dev_name_hook)(char *devName);
int (*get_wan_index_by_vlan_hook)(unsigned char vlanLayer, unsigned short vlanId);
/*TCSUPPORT_CPU_PERFORMANCE_TEST---end---*/
EXPORT_SYMBOL(remove_vtag_hook);
EXPORT_SYMBOL(insert_vtag_hook);
//#if !defined(TCSUPPORT_FTP_THROUGHPUT)
EXPORT_SYMBOL(check_vtag_hook);
//#endif
EXPORT_SYMBOL(get_vtag_hook);
/*TCSUPPORT_ROUTEPOLICY_PRIOR_PORTBIND*/
EXPORT_SYMBOL(tcvlan_get_outdev_hook);
/*END TCSUPPORT_ROUTEPOLICY_PRIOR_PORTBIND*/
/*TCSUPPORT_CPU_PERFORMANCE_TEST---start---*/
EXPORT_SYMBOL(get_pvid_by_dev_name_hook);
EXPORT_SYMBOL(get_wan_index_by_vlan_hook);
/*TCSUPPORT_CPU_PERFORMANCE_TEST---end---*/
#endif
/*TCSUPPORT_VLAN_PASSTHROUGH*/
int (*check_vlan_range_hook)(struct net_device *dev, struct sk_buff *skb);
EXPORT_SYMBOL(check_vlan_range_hook);
/*END TCSUPPORT_VLAN_PASSTHROUGH*/

int (*xpon_store_upstream_igmp_vlan_tci_hook)(struct sk_buff *skb) = NULL;
int (*xpon_down_multicast_vlan_handle_hook)(struct sk_buff **skb) = NULL;
int (*xpon_upstream_vlan_recovery_by_dynlist_hook)(struct sk_buff* skb) = NULL;
int (*xpon_hgu_down_multicast_access_control_hook)(struct sk_buff* skb) = NULL;
int (*xpon_hgu_set_multicast_max_groups_hook)(int maxGroups) = NULL;
int (*xpon_hgu_set_multicast_max_rate_hook)(unsigned int maxRate) = NULL;  /*unit packets/s*/
int (*xpon_hgu_down_multicast_vlan_tci_hook)(struct sk_buff *skb) = NULL;


EXPORT_SYMBOL(xpon_store_upstream_igmp_vlan_tci_hook);
EXPORT_SYMBOL(xpon_down_multicast_vlan_handle_hook);
EXPORT_SYMBOL(xpon_upstream_vlan_recovery_by_dynlist_hook);
EXPORT_SYMBOL(xpon_hgu_down_multicast_access_control_hook);
EXPORT_SYMBOL(xpon_hgu_set_multicast_max_groups_hook);
EXPORT_SYMBOL(xpon_hgu_set_multicast_max_rate_hook);
EXPORT_SYMBOL(xpon_hgu_down_multicast_vlan_tci_hook);

//#ifdef CONFIG_SMUX
#if defined(TCSUPPORT_CT)
int (*do_mulif_interface_unregister_hook)(char* dev_name);
EXPORT_SYMBOL(do_mulif_interface_unregister_hook);
#else
int (*check_smuxIf_exist_hook)(struct net_device *dev);
EXPORT_SYMBOL(check_smuxIf_exist_hook);
#endif
int (*smux_pkt_recv_hook)(struct sk_buff *skb, 
                  struct net_device *dev,
                  struct net_device *rdev);
EXPORT_SYMBOL(smux_pkt_recv_hook);
void (*get_wan_index_info_hook)(char* wan_index);
EXPORT_SYMBOL(get_wan_index_info_hook);
int (*check_mulif_tci_hook)(u16 tci,u16 multicast_tci);
EXPORT_SYMBOL(check_mulif_tci_hook);

//#endif



int (*I2CWriterPtr)(unsigned char DevAddr, unsigned char WordAddr, unsigned char* data_value, unsigned char data_len);
int (*I2CReaderPtr)(unsigned char DevAddr, unsigned char WordAddr, unsigned char* data_value, unsigned char data_len);
EXPORT_SYMBOL(I2CWriterPtr);
EXPORT_SYMBOL(I2CReaderPtr);

#if defined(TCSUPPORT_XPON_IGMP)
int (*xpon_sfu_up_send_multicast_frame_hook)(struct sk_buff *skb, int clone) = NULL;

int (*xpon_sfu_up_multicast_incoming_hook)(struct sk_buff *skb, int clone) = NULL;

int (*xpon_sfu_down_multicast_incoming_hook)(struct sk_buff *skb, int clone) = NULL;

int (*xpon_hgu_down_multicast_incoming_hook)(struct sk_buff *skb, int clone) = NULL;

int (*xpon_hybrid_down_multicast_incoming_hook)(struct sk_buff *skb, int clone) = NULL;

int (*xpon_sfu_up_multicast_vlan_hook)(struct sk_buff *skb, int clone) = NULL;

int (*xpon_sfu_multicast_protocol_hook)(struct sk_buff *skb) = NULL;

int (*xpon_up_igmp_uni_vlan_filter_hook)(struct sk_buff *skb) = NULL;

int (*xpon_up_igmp_ani_vlan_filter_hook)(struct sk_buff *skb) = NULL;

int (*isVlanOperationInMulticastModule_hook)(struct sk_buff *skb) = NULL;


EXPORT_SYMBOL(xpon_sfu_up_send_multicast_frame_hook);

EXPORT_SYMBOL(xpon_sfu_up_multicast_incoming_hook);

EXPORT_SYMBOL(xpon_sfu_down_multicast_incoming_hook);

EXPORT_SYMBOL(xpon_hgu_down_multicast_incoming_hook);

EXPORT_SYMBOL(xpon_hybrid_down_multicast_incoming_hook);

EXPORT_SYMBOL(xpon_sfu_up_multicast_vlan_hook);

EXPORT_SYMBOL(xpon_sfu_multicast_protocol_hook);

EXPORT_SYMBOL(xpon_up_igmp_uni_vlan_filter_hook);

EXPORT_SYMBOL(xpon_up_igmp_ani_vlan_filter_hook);

EXPORT_SYMBOL(isVlanOperationInMulticastModule_hook);
#endif

#ifdef TCSUPPORT_PON_MAC_FILTER
int (*pon_check_mac_hook)(struct sk_buff *skb);
int (*pon_mac_filter_get_mode_hook)(void);

EXPORT_SYMBOL(pon_check_mac_hook);
EXPORT_SYMBOL(pon_mac_filter_get_mode_hook);
#endif

#ifdef TCSUPPORT_GPON_MAPPING
int (*gpon_mapping_hook)(struct sk_buff *pskb);
int (*xpon_mode_get_hook)(void);

EXPORT_SYMBOL(gpon_mapping_hook);
EXPORT_SYMBOL(xpon_mode_get_hook);

#if defined(TCSUPPORT_GPON_DOWNSTREAM_MAPPING)
int (*gpon_downstream_mapping_hook)(struct sk_buff *skb);
int (*gpon_downstream_mapping_stag_hook)(struct sk_buff **skb);
EXPORT_SYMBOL(gpon_downstream_mapping_hook);
EXPORT_SYMBOL(gpon_downstream_mapping_stag_hook);
#endif
#endif

#ifdef TCSUPPORT_EPON_MAPPING
int (*epon_sfu_clsfy_hook)(struct sk_buff *skb, int port);
int (*epon_mapping_hook)(struct sk_buff *skb);
EXPORT_SYMBOL(epon_sfu_clsfy_hook);
EXPORT_SYMBOL(epon_mapping_hook);
#endif

int (*ecnt_br_map_link_local_packet_handle_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(ecnt_br_map_link_local_packet_handle_hook);

#ifdef TCSUPPORT_PON_VLAN
int (*pon_insert_tag_hook)(struct sk_buff **pskb);
int (*pon_vlan_get_mode_hook)(void);
int (*pon_store_tag_hook)(struct sk_buff *skb, struct net_device *dev);
int (*pon_check_vlan_hook)(struct net_device *dev, struct sk_buff *skb);
int (*pon_check_tpid_hook)(__u16 * buf);
int (*pon_check_user_group_hook)(struct sk_buff *skb);
int (*pon_PCP_decode_hook)(struct sk_buff **pskb);
int (*pon_hybrid_sfu_lan_check_hook)(struct sk_buff **pskb) = NULL;
int (*pon_hybrid_sfu_wan_check_hook)(struct sk_buff **pskb) = NULL;
int (*pon_vlan_is_ds_1_to_N_hook)(struct sk_buff **pskb, int *count)=NULL;
int (*pon_vlan_ds_1_to_N_handler_hook)(struct sk_buff **pskb, int index)=NULL;

EXPORT_SYMBOL(pon_insert_tag_hook);
EXPORT_SYMBOL(pon_vlan_get_mode_hook);
EXPORT_SYMBOL(pon_store_tag_hook);
EXPORT_SYMBOL(pon_check_vlan_hook);
EXPORT_SYMBOL(pon_check_tpid_hook);
EXPORT_SYMBOL(pon_check_user_group_hook);
EXPORT_SYMBOL(pon_PCP_decode_hook);
EXPORT_SYMBOL(pon_hybrid_sfu_lan_check_hook);
EXPORT_SYMBOL(pon_hybrid_sfu_wan_check_hook);
EXPORT_SYMBOL(pon_vlan_is_ds_1_to_N_hook);
EXPORT_SYMBOL(pon_vlan_ds_1_to_N_handler_hook);

#endif

#if defined(TCSUPPORT_XPON_IGMP)
int (*xpon_igmp_acl_filter_hook)(struct sk_buff* skb) = NULL;
EXPORT_SYMBOL(xpon_igmp_acl_filter_hook);
#endif

#if defined(TCSUPPORT_XPON_IGMP)
int (*xpon_igmp_ioctl_hook)(unsigned long subcmd,unsigned long argv1,unsigned long argv2);
EXPORT_SYMBOL(xpon_igmp_ioctl_hook);
#endif

/*#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)*/
int (*xpon_hgu_multicast_data_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(xpon_hgu_multicast_data_hook);
/*#endif*/
int (*hwnat_multicast_data_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(hwnat_multicast_data_hook);

#if defined(TCSUPPORT_XPON_IGMP)
int (*xpon_add_delete_port_hook)(struct net_device* dev, int op);
EXPORT_SYMBOL(xpon_add_delete_port_hook);
#endif

#if defined(TCSUPPORT_CT_MAXNET_DPI)
int (*dpi_core_nf_event_hook)(enum ip_conntrack_events event, struct nf_conn *ct);
EXPORT_SYMBOL(dpi_core_nf_event_hook);
#endif
#if defined(TCSUPPORT_CT_APPROUTE)
int (*approute_core_nf_event_hook)(enum ip_conntrack_events event, struct nf_conn *ct, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(approute_core_nf_event_hook);
char vpn_if_prefix[32] = {0};
EXPORT_SYMBOL(vpn_if_prefix);
#endif
#if defined(TCSUPPORT_CT_APPROUTE) || defined(TCSUPPORT_CT_APPFILTER)
int (*approute_ctrl_log_hook)(char *buf_addr, struct timeval *tv) = NULL;
EXPORT_SYMBOL(approute_ctrl_log_hook);

uint32_t g_Lan_IP = htonl(0xc0a80101);
EXPORT_SYMBOL(g_Lan_IP);
uint32_t g_Lan_IP_Mask = htonl(0xffffff00);
EXPORT_SYMBOL(g_Lan_IP_Mask);
uint8_t g_Lan_IP6[16] = {0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
EXPORT_SYMBOL(g_Lan_IP6);
int32_t g_TimeZone = 0;
EXPORT_SYMBOL(g_TimeZone);
#endif

/*for atm ptm*/
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512)
int napi_en = 0;
EXPORT_SYMBOL(napi_en);
void (*br2684_config_hook)(int linkMode, int linkType);
EXPORT_SYMBOL(br2684_config_hook);
int (*br2684_init_hook)(struct atm_vcc *atmvcc, int encaps) = NULL;
EXPORT_SYMBOL(br2684_init_hook);
int (*br2684_push_hook)(struct atm_vcc *atmvcc, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(br2684_push_hook);
int (*br2684_xmit_hook)(struct sk_buff *skb, struct net_device *dev, enum br2684_encaps encaps) = NULL;
EXPORT_SYMBOL(br2684_xmit_hook);
#endif

#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512)
extern netdev_tx_t br2684_start_xmit(struct sk_buff *skb, struct net_device *dev);

EXPORT_SYMBOL(br2684_start_xmit);
#endif

#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) 
extern int br2684_init(void);
extern void br2684_exit(void);

EXPORT_SYMBOL(br2684_init);
EXPORT_SYMBOL(br2684_exit);
#endif

#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505)
void (*pppoatm_config_hook)(int linkMode, int linkType) = NULL;
EXPORT_SYMBOL(pppoatm_config_hook);

int (*pppoatm_init_hook)(struct atm_vcc *atmvcc, int encaps) = NULL;
EXPORT_SYMBOL(pppoatm_init_hook);

int (*pppoatm_push_hook)(struct atm_vcc *atmvcc, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(pppoatm_push_hook);
#endif

#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7505)
extern int pppoatm_init(void);
extern void pppoatm_exit(void);

EXPORT_SYMBOL(pppoatm_init);
EXPORT_SYMBOL(pppoatm_exit);
#endif
/*atm ptm end*/
#ifdef MTK_CRYPTO_DRIVER
int ip_output(struct sock *sk, struct sk_buff *skb);
int xfrm_parse_spi(struct sk_buff *skb, u8 nexthdr, __be32 *spi, __be32 *seq);
EXPORT_SYMBOL(ip_output);
EXPORT_SYMBOL(xfrm_parse_spi);
#endif

void (*get_net_device_interface_hw_stats_hook)(struct net_device *dev, struct rtnl_link_stats64 *storage) = NULL;
EXPORT_SYMBOL(get_net_device_interface_hw_stats_hook);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
/**
 *	random32 - pseudo random number generator
 *
 *	A 32 bit pseudo-random number is generated using a fast
 *	algorithm suitable for simulation. This algorithm is NOT
 *	considered safe for cryptographic use.
 */
u32 random32(void)
{
	return prandom_u32();
}
EXPORT_SYMBOL(random32);

void gdump_mode_sel(u32 val)
{
	u32 reg = 0;
	if (val >= MAX_GDUMP_SEL_NUM)
	{
		printk("(%s): ERROR: invalid usage!!\r\n",__func__);
	}
	else
	{
		reg = VPint(0xbfb00954);
		reg &= ~(GDUMP_DATA_SEL_MASK);
		reg |= val;
		VPint(0xbfb00954) = reg;
		printk("(%s): GDUMP sel has switch to %x\r\n",__func__,val);
	}
}
EXPORT_SYMBOL(gdump_mode_sel);


#if 1
struct sk_buff *__vlan_put_tag(struct sk_buff *skb, u16 vlan_tci)
{
	skb->protocol = htons(ETH_P_8021Q);
	return vlan_insert_tag(skb, skb->protocol, vlan_tci);
}
#else
struct sk_buff *__vlan_put_tag(struct sk_buff *skb, u16 vlan_tci)
{
	struct vlan_ethhdr *veth;

	if (skb_cow_head(skb, VLAN_HLEN) < 0) {
		kfree_skb(skb);
		return NULL;
	}
	veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	memmove(skb->data, skb->data + VLAN_HLEN, 2 * VLAN_ETH_ALEN);
	skb->mac_header -= VLAN_HLEN;

	/* first, the ethernet type */
	veth->h_vlan_proto = htons(ETH_P_8021Q);

	/* now, the TCI */
	veth->h_vlan_TCI = htons(vlan_tci);

	skb->protocol = htons(ETH_P_8021Q);

	return skb;
}
#endif
EXPORT_SYMBOL(__vlan_put_tag);

/**
 * vlan_put_tag - inserts VLAN tag according to device features
 * @skb: skbuff to tag
 * @vlan_tci: VLAN TCI to insert
 *
 * Assumes skb->dev is the target that will xmit this frame.
 * Returns a VLAN tagged skb.
 */
struct sk_buff *vlan_put_tag(struct sk_buff *skb, u16 vlan_tci)
{
	if (skb->dev->features & NETIF_F_HW_VLAN_TX) {
		__vlan_hwaccel_put_tag(skb, ETH_P_8021Q, vlan_tci);
		return skb;
	} 
	
	return __vlan_put_tag(skb, vlan_tci);
}
EXPORT_SYMBOL(vlan_put_tag);

#ifdef TCSUPPORT_PON_VLAN
static DEFINE_SPINLOCK(ptype_lock);
static struct list_head ptype_base[PTYPE_HASH_SIZE] __read_mostly;
static struct list_head ptype_all __read_mostly;	/* Taps */

//check if type already in the list.
//return: 0:yes 1:no
int pon_check_pack(__u16 type)
{
	struct list_head *head;
	struct packet_type *pt1;

	spin_lock_bh(&ptype_lock);
	if (type == htons(ETH_P_ALL))
		head = &ptype_all;
	else
		head = &ptype_base[ntohs(type) & 15];

	list_for_each_entry(pt1, head, list) 
	{
		if (type == pt1->type)
		{
			spin_unlock_bh(&ptype_lock);
			return 0;
		}
	}
	spin_unlock_bh(&ptype_lock);
	return 1;
}
EXPORT_SYMBOL(pon_check_pack);
#endif

#if defined(TCSUPPORT_PON_VLAN) || defined(TCSUPPORT_VLAN_TPID)
/*
Uesd for xPon insert tag.Support set TPID
*/
struct sk_buff *__pon_vlan_put_tag(struct sk_buff *skb, u16 tpid,unsigned short vlan_tci)
{
	struct vlan_ethhdr *veth;

	if (skb_cow_head(skb, VLAN_HLEN) < 0) {
		kfree_skb(skb);
		return NULL;
	}
	veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	memmove(skb->data, skb->data + VLAN_HLEN, 2 * ETH_ALEN);
	skb->mac_header -= VLAN_HLEN;

	/* first, the ethernet type */
	veth->h_vlan_proto = htons(tpid);

	/* now, the TCI */
	veth->h_vlan_TCI = htons(vlan_tci);

	skb->protocol = htons(tpid);

	return skb;
}
EXPORT_SYMBOL(__pon_vlan_put_tag);
#endif

struct net *__get_net_ns_by_pid(pid_t pid) {
	struct net *netns = NULL;
	
	netns = get_net_ns_by_pid(pid);
	if(netns == ERR_PTR(-ESRCH)) 
		netns = NULL;
	
	return netns;
}
EXPORT_SYMBOL(__get_net_ns_by_pid);
/**************************************************
Function: Clear all the SW and HW maintained
          multicast flow
Input: 
	N/A
Return: 
	0: ok
**************************************************/
#if 0
int igmp_hwnat_clear_flows(void)
{
	IGMP_HWNATEntry_t* entry = NULL,*tmp = NULL;
	struct list_head* hwnat_flow = igmp_hwnat_get_list();
	
	IGMP_HWNAT_DEBUG("enter");
	
	spin_lock(&hwnat_lock);
	#ifdef TCSUPPORT_MULTICAST_SPEED
	list_for_each_entry_rcu(entry,hwnat_flow,list)
	#else
	list_for_each_entry_safe(entry,tmp,hwnat_flow,list)
	#endif
	{
		igmp_hwnat_delete_flow(entry);
	}
	spin_unlock(&hwnat_lock);
	return 0;
}
EXPORT_SYMBOL(igmp_hwnat_clear_flows);
#endif

#if defined(TCSUPPORT_WLAN_GPIO) || (defined(TCSUPPORT_LED_SWITCH_BUTTON) && defined(TCSUPPORT_WLAN))
int (*hook_wlan_led_action)(int action, int gpio);
EXPORT_SYMBOL(hook_wlan_led_action);
#endif

#if defined(TCSUPPORT_LED_SWITCH_BUTTON) && defined(TCSUPPORT_WLAN)
#if defined(TCSUPPORT_WLAN_AC)
int (*hook_wlan_led_action_5g)(int action, int gpio);
EXPORT_SYMBOL(hook_wlan_led_action_5g);
#endif
#endif

#if !defined(TCSUPPORT_FH_JOYMEV2_PON)
void os_TCIfQuery(unsigned short query_id, void *result1, void *result2)
{
	if (adsl_dev_ops == NULL)
		return;
  
	adsl_dev_ops->query(query_id, result1, result2);
}
EXPORT_SYMBOL(os_TCIfQuery);
#endif

#include <asm/cache.h>
#include <asm/cacheops.h>
#include <asm/r4kcache.h>
#include <asm/dma-coherence.h>

inline void r4k_dma_cache_wback_inv_ecnt(unsigned long addr, unsigned long size)
{
	blast_dcache_range(addr, addr + size);
	blast_scache_range(addr, addr + size);
}
EXPORT_SYMBOL(r4k_dma_cache_wback_inv_ecnt);

inline void r4k_dma_cache_inv_ecnt(unsigned long addr, unsigned long size)
{
	blast_inv_dcache_range(addr, addr + size);
	cache_op(Hit_Writeback_Inv_SD, addr & 0xffffffe0);
	cache_op(Hit_Writeback_Inv_SD, (addr + size - 1) & 0xffffffe0);
	blast_inv_scache_range(addr, addr + size);
}
EXPORT_SYMBOL(r4k_dma_cache_inv_ecnt);

int wscStatus_24g = 0;
EXPORT_SYMBOL(wscStatus_24g);
int wscStatus_5g = 0;
EXPORT_SYMBOL(wscStatus_5g);
int wscTimerRunning_24g = 0;
EXPORT_SYMBOL(wscTimerRunning_24g);
int wscTimerRunning_5g = 0;
EXPORT_SYMBOL(wscTimerRunning_5g);


/*gpio lock for multiple CPU cores*/
DEFINE_SPINLOCK(gpioLock);
unsigned int gpio_spinlock_flags[NR_CPUS] = {0};
void gpio_spin_lock(void) { spin_lock_irqsave(&gpioLock, gpio_spinlock_flags[smp_processor_id()]);}
void gpio_spin_unlock(void) {spin_unlock_irqrestore(&gpioLock, gpio_spinlock_flags[smp_processor_id()]);}
EXPORT_SYMBOL(gpio_spin_lock);
EXPORT_SYMBOL(gpio_spin_unlock);
/*gpio lock end*/

#if defined(TCSUPPORT_CT_APPROUTE)
struct nf_conn *ecnt_get_nfct(const struct sk_buff *skb, struct net *net, u_int8_t pf)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,20)
	struct nf_conntrack_l3proto *l3proto = NULL;
#endif
	struct nf_conntrack_l4proto *l4proto = NULL;
	unsigned int dataoff = 0;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,0,21)
	u_int8_t protonum = 0;
	int ret = 0;
#endif
	struct nf_conntrack_tuple tuple = {0};
	struct nf_conntrack_tuple_hash *h = NULL;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,0,21)
	/* rcu_read_lock()ed by nf_hook_slow */
	l3proto = __nf_ct_l3proto_find(pf);
	ret = l3proto->get_l4proto(skb, skb_network_offset(skb),
				   &dataoff, &protonum);
	if ( ret <= 0 )
		return NULL;

	l4proto = __nf_ct_l4proto_find(pf, protonum);
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,3,6)
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,0,21)
	if ( !nf_ct_get_tuplepr(skb, skb_network_offset(skb),
						   pf, net, &tuple) )
		return NULL;
#else
	if ( !nf_ct_get_tuple(skb, skb_network_offset(skb),
				 dataoff, pf, protonum, net, &tuple, l3proto,
				 l4proto) )
	{
		/*printk("%s(): Can't get tuple\n", __FUNCTION__);*/
		return NULL;
	}
#endif

	h = nf_conntrack_find_get(net, &nf_ct_zone_dflt, &tuple);
#else
	if ( !nf_ct_get_tuple(skb, skb_network_offset(skb),
				 dataoff, pf, protonum, &tuple, l3proto,
				 l4proto) )
	{
		/*printk("%s(): Can't get tuple\n", __FUNCTION__);*/
		return NULL;
	}

	h = nf_conntrack_find_get(net, NF_CT_DEFAULT_ZONE, &tuple);
#endif

	if ( h )
		return nf_ct_tuplehash_to_ctrack(h);
	return NULL;
}
EXPORT_SYMBOL(ecnt_get_nfct);

void ecnt_nfct_delete(struct nf_conn *ct)
{
	if ( NULL == ct )
		return;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,8,17)
	if ( del_timer(&ct->timeout) )
#endif
		nf_ct_delete(ct, 0, 0);
}
EXPORT_SYMBOL(ecnt_nfct_delete);
#endif
//NPU does not support MIPS, but hw_nat will include ecnt_hook_fe.h that uses npu api.
void npu_wifi_offload_set_force_to_cpu_flag(char isForceToCpu)
{
	printk("NPU does not support MIPS!\n", __func__);
	return;
	
}
EXPORT_SYMBOL(npu_wifi_offload_set_force_to_cpu_flag);
struct device* get_gdmpSram_dev(void)
{
        return NULL;
}
EXPORT_SYMBOL(get_gdmpSram_dev);
int host_notify_npuMbox(void *mboxInfo)
{
	return 0;
}
EXPORT_SYMBOL(host_notify_npuMbox);

