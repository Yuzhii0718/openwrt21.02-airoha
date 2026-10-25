/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attempt
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    
    hook.c

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2006-10-06      Initial version
*/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/foe_hook.h>
#include <net/sock.h>
#include <linux/in6.h>

int (*ra_sw_nat_hook_rx_set_l2lu)(struct sk_buff * skb, unsigned int direction, int PpeIndex) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_rx_set_l2lu);
int (*ra_sw_nat_hook_rx) (struct sk_buff * skb) = NULL;
int (*ra_sw_nat_mcst_offload) (struct sk_buff * skb, int *dp) = NULL;
int (*ra_sw_nat_ds_offload) (struct sk_buff * skb, int *dp) = NULL;
int (*ra_nat_cirpir_get_start_idx)(void) = NULL;
int (*ra_sw_nat_hook_update_dp)(int index, int dp) = NULL;
int (*ra_sw_nat_hook_update_vlan)(int index,int outer_vlan,int inner_vlan) = NULL;
int (*ra_sw_nat_local_in_tx) (struct sk_buff * skb,unsigned short port) = NULL;

int (*ra_sw_nat_hook_save_rxinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_restore_rxinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_save_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_restore_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_save_pptp_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_restore_pptp_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_save_gre_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_restore_gre_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_is_gre_offload_pkt)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_save_vxlan_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_restore_vxlan_txinfo)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_is_vxlan_offload_pkt)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_is_hwnat_pkt)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_sendto_ppe)(struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_set_l2tp_dev)(struct net_device *dev) = NULL;
struct net_device* (*ra_sw_nat_hook_read_l2tp_dev)(void) = NULL;
int (*ra_sw_nat_hook_set_pptp_dev)(struct net_device *dev) = NULL;
struct net_device* (*ra_sw_nat_hook_read_pptp_dev)(void) = NULL;
int (*ra_sw_nat_rtsp_offload_restore) (struct sk_buff * skb, int calc_sum) = NULL;
int (*ra_sw_nat_rtsp_data_handle) (struct sk_buff * skb, char *rb_ptr, unsigned int datalen) = NULL;
int (*ra_sw_nat_rtspv6_npt_data_handle) (struct sk_buff * skb) = NULL;
void (*ra_sw_nat_set_wan_acntid_hook) (struct sk_buff *skb, unsigned char wan_index, unsigned char dir) = NULL;
void (*ra_sw_nat_clear_wan_acntid_hook) (unsigned char wan_index) = NULL;
void (*ra_sw_nat_get_wan_acntid_counter_hook)(unsigned char wan_index, struct net_device_stats *storage) = NULL; 
void (*ra_sw_nat_set_mac_hook) (struct sk_buff *skb, unsigned char *mac) = NULL;
void (*ra_sw_nat_set_wan_meterid_hook) (struct sk_buff *skb, unsigned char wan_index) = NULL;
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
void (*ra_sw_nat_set_mul_br_vid_hook)(struct sk_buff *skb, unsigned short vid) = NULL;
unsigned short (*ra_sw_nat_get_mul_br_vid_hook)(struct sk_buff *skb) = NULL;
#endif
#ifdef TCSUPPORT_MT7510_FE
int (*ra_sw_nat_hook_tx) (struct sk_buff * skb, struct port_info * pinfo, int magic);
#else
int (*ra_sw_nat_hook_tx) (struct sk_buff * skb, int gmac_no) = NULL;
#endif
int (*ra_hit_bind_force_to_cpu) (struct sk_buff *skb) = NULL;
int (*ra_sw_nat_hook_free) (struct sk_buff * skb) = NULL;
int (*ra_sw_nat_hook_rxinfo) (struct sk_buff * skb, int magic, char *data, int data_length) = NULL;
int (*ra_sw_nat_hook_txq) (struct sk_buff * skb, int txq) = NULL;
int (*ra_sw_nat_hook_magic) (struct sk_buff * skb, int magic) = NULL;
int (*ra_sw_nat_hook_set_magic) (struct sk_buff * skb, int magic) = NULL;
int (*ra_sw_nat_hook_xfer) (struct sk_buff *skb, const struct sk_buff *prev_p) = NULL;
int (*ra_sw_nat_hook_is_alive_pkt)(unsigned int crsn) = NULL;
int (*ra_sw_nat_hook_drop_packet) (struct sk_buff * skb) = NULL;
int (*ra_sw_nat_hook_clean_table) (void) = NULL;
int (*ra_sw_nat_hook_clean_multicast_entry) (void) = NULL;
int (*ra_sw_nat_hook_cpu_meter)(struct sk_buff* skb,FETxMsg_T* txMsg,struct port_info* pinfo,unsigned char dir,unsigned short mtrIndex) = NULL;
int (*ra_sw_nat_hook_ipv6_nf_conntrack_hook) (struct nf_conntrack* nf) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_ipv6_nf_conntrack_hook);
int (*ra_sw_nat_hook_tls_vtag_handle_hook)(struct sk_buff** pskb) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_tls_vtag_handle_hook);
int (*ra_sw_nat_cds_all_ratelimit_hook) (struct sk_buff* skb) = NULL;
EXPORT_SYMBOL(ra_sw_nat_cds_all_ratelimit_hook);

int (*ra_sw_nat_hook_tunnel_down_acc_hook) (struct sk_buff * skb, unsigned int rx_len, unsigned short sptag, unsigned short udf, int magic, char *data, int data_length) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_tunnel_down_acc_hook);


int is_hwnat_dont_clean = 0;
EXPORT_SYMBOL(is_hwnat_dont_clean);

int (*ra_sw_nat_hook_entry_type)(int index) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_entry_type);

int (*ra_sw_nat_hook_clean_entry_by_dst_mac)(struct net_data_s *net_data) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_clean_entry_by_dst_mac);

int (*ra_sw_nat_hook_clean_entry_by_channel)(int channelIdx) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_clean_entry_by_channel);

int (*ra_sw_nat_hook_clean_entry_by_dev)(struct net_device *dev) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_clean_entry_by_dev);

int (*ra_sw_nat_hook_clean_entry_by_mac)(const unsigned char* mac) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_clean_entry_by_mac);

int (*ra_sw_nat_hook_foeentry) (void * inputvalue,int operation) = NULL;
int (*ra_sw_nat_hook_is_entry_valid) (int foe_entry_idx, int ring_idx) = NULL;

int (*ra_sw_nat_natv6_fast_handler) (struct sk_buff * skb) = NULL;
int (*ra_sw_nat_vxlan_fast_handler) (struct sk_buff * skb) = NULL;

#ifdef TCSUPPORT_MT7510_FE
#ifdef TCSUPPORT_HWNAT_LED
int is_hwnat_led_enable = 1;
#else
int is_hwnat_led_enable = 0;
#endif // end of TCSUPPORT_HWNAT_LED
EXPORT_SYMBOL(is_hwnat_led_enable);
#endif // end of TCSUPPORT_MT7510_FE

int g_wan_mode = 0;
EXPORT_SYMBOL(g_wan_mode);

SMUX_Bridge_Info_Data pppoe_info_for_app;
EXPORT_SYMBOL(pppoe_info_for_app);

wan_virtualdev_ifname wan_virtualdev_ifname_info[16];
EXPORT_SYMBOL(wan_virtualdev_ifname_info);
int wan_virtualdev_if_num = 0;
EXPORT_SYMBOL(wan_virtualdev_if_num);
//#if defined(TCSUPPORT_XPON_IGMP) || defined(TCSUPPORT_MULTICAST_SPEED)
int (*hwnat_clean_entry_by_dst_mac_hook)(const unsigned char* mac) =NULL;
int (*hwnat_is_alive_pkt_hook)(struct sk_buff* skb) = NULL;
int (*hwnat_skb_to_foe_hook)(struct sk_buff* skb) = NULL;
int (*hwnat_set_special_tag_hook)(int index, int tag) = NULL;
int (*hwnat_multicast_set_hwnat_info_hook)(int index, unsigned long long mask, short int bindvid) = NULL;
int (*hwnat_delete_foe_entry_hook)(int index) = NULL; 
int (*hwnat_delete_foe_entry_hook_unlock)(int index) = NULL;
int (*hwnat_is_multicast_entry_hook)(int index ,unsigned char* grp_addr,unsigned char* src_addr,int type) = NULL;
int (*hwnat_is_drop_entry_hook)(int index ,unsigned char* grp_addr,unsigned char* src_addr,int type) = NULL;
int (*hwnat_set_multicast_vlan_hook)(int index, int vid, int vpm) = NULL;
IGMP_HWNATEntry_t*  (*multicast_speed_find_entry_hook)(int index) = NULL;
int  (*multicast_speed_learn_flow_hook)(struct sk_buff* skb) = NULL;
int  (*hwnat_set_rule_according_to_state_hook)(int index, int state,unsigned long long mask) = NULL;
int  (*hwnat_set_recover_info_hook)(struct sk_buff* skb,struct sock *sk,int flag) = NULL;
int  (*xpon_igmp_learn_flow_hook)(struct sk_buff* skb) = NULL;
int  (*multicast_hwnat_drop_entry_hook)(struct sk_buff* skb) = NULL;
int  (*xpon_igmp_exist)(void) = NULL;
int  (*xpon_igmp_clear_flows)(void) = NULL;
int (*hwnat_set_wlan_multicast_hook)(int index ,int flag) = NULL;
int (*wan_multicast_drop_hook)(struct sk_buff* skb) = NULL;
int (*wan_multicast_undrop_hook)(void) = NULL;
int (*wan_multicast_undrop_by_grpip_hook)(unsigned char is_ipv6,unsigned char* grp_ip) = NULL;
int (*wan_mvlan_change_hook)(void) = NULL;
multicast_flood_hwentry_t*  (*multicast_flood_find_entry_hook)(int index) = NULL;
int  (*hwnat_set_multicast_speed_enable_hook)(int enable) = NULL;
int  (*multicast_flood_is_bind_hook)(int index) = NULL;
int  (*multicast_xmit_packet_hook)(struct sk_buff* skb, unsigned short vid, 
                                   struct net_device *skip_eth_dev, unsigned short skip_eth_vid) = NULL;
int  (*multicast_get_stb_src_ip4)(unsigned int ip4) = NULL;
int  (*multicast_get_stb_src_ip6)(struct in6_addr *ip6) = NULL;
int (*multicast_subcribe_group_hook)(unsigned char* grp_addr, unsigned int proto, unsigned int update_mode) = NULL;
int (*multicast_get_local_hook)(unsigned char* grp_addr) = NULL;
int (*hwnat_skb_cpu_reason_hook)(struct sk_buff* skb) = NULL;
EXPORT_SYMBOL(hwnat_skb_cpu_reason_hook);

int (*MT7530LanPortMap2Switch_hook) (int port) = NULL;
EXPORT_SYMBOL(MT7530LanPortMap2Switch_hook);

int (*hwnat_is_bind_and_L3_pkt_hook)(struct sk_buff* skb) = NULL;
EXPORT_SYMBOL(hwnat_is_bind_and_L3_pkt_hook);

int (*hwnat_is_from_force_cpu_pkt_hook)(struct sk_buff* skb) = NULL;
EXPORT_SYMBOL(hwnat_is_from_force_cpu_pkt_hook);

/*
 * WAN description used by the ether driver built from newer sources.
 * The newer vendor kernel owns these here; this kernel predates them.
 * wan_conn_type keeps the vendor default 4, which makes the only reader in
 * eth_lan.c skip wan_itf_name altogether, so the empty name is harmless.
 */
unsigned int wan_conn_type = 4;
EXPORT_SYMBOL(wan_conn_type);
char wan_itf_name[16];
EXPORT_SYMBOL(wan_itf_name);

/* TR-471 statistics hook, NULL by default like the other optional hooks. */
int (*wan_tr471_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(wan_tr471_hook);

unsigned int (*hwnat_set_bind_threshold_hook)(unsigned int threshold) = NULL;
EXPORT_SYMBOL(hwnat_set_bind_threshold_hook);

int (*ra_sw_nat_hook_wifi_tx) (struct sk_buff * skb, unsigned int rx_len) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_wifi_tx);

int (*ra_sw_nat_to_wifi_fast_tx) (struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(ra_sw_nat_to_wifi_fast_tx);

int (*ra_sw_nat_get_wifi_dev_name) (struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(ra_sw_nat_get_wifi_dev_name);

int (*ra_sw_nat_to_xsi_fast_tx) (struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(ra_sw_nat_to_xsi_fast_tx);

void (*ecnt_7603_set_shortcut_flag_hook)(int flag)=NULL;
EXPORT_SYMBOL(ecnt_7603_set_shortcut_flag_hook);
int (*ecnt_7603_get_shortcut_flag_hook)(void)=NULL;
EXPORT_SYMBOL(ecnt_7603_get_shortcut_flag_hook);

void (*restore_offload_info_hook)(struct sk_buff *skb, struct port_info *pinfo, int magic) = NULL;
EXPORT_SYMBOL(restore_offload_info_hook);

int (*ecnt_7603_shortcut_hook)(struct sk_buff *skb, int index)=NULL;
EXPORT_SYMBOL(ecnt_7603_shortcut_hook);

int (*ecnt_7612_shortcut_hook)(struct sk_buff *skb, int index)=NULL;
EXPORT_SYMBOL(ecnt_7612_shortcut_hook);
int wifi_tx_shortcut_ver = 0;
EXPORT_SYMBOL(wifi_tx_shortcut_ver);

int (*hwnat_gre_fast_down_hook)(struct sk_buff* skb) = NULL;
EXPORT_SYMBOL(hwnat_gre_fast_down_hook);

int (*hwnat_gre_fast_up_hook)(struct sk_buff* skb) = NULL;
EXPORT_SYMBOL(hwnat_gre_fast_up_hook);

unsigned int sysctl_lan_tx_off = 1;
EXPORT_SYMBOL(sysctl_lan_tx_off);

int TxShortFlag = 0;
EXPORT_SYMBOL(TxShortFlag);

unsigned int is5GTest = 0;
EXPORT_SYMBOL(is5GTest);

#ifdef TCSUPPORT_WLAN_LED_BY_SW
void (*WLan_Led_Flash_Op_hook)(unsigned int opmode) = NULL;
EXPORT_SYMBOL(WLan_Led_Flash_Op_hook);
#endif

unsigned int sync_type10 = 0;
EXPORT_SYMBOL(sync_type10);

int (*ra_sw_nat_npu_offload_vxlan_enable)(void) = NULL;
EXPORT_SYMBOL(ra_sw_nat_npu_offload_vxlan_enable);

int (*ra_sw_nat_npu_store_vxlan_hdr)(struct sk_buff * skb,unsigned int type) = NULL;
EXPORT_SYMBOL(ra_sw_nat_npu_store_vxlan_hdr);

int (*ra_sw_nat_npu_check_vxlan_hdr_list_hook)(void) = NULL;
EXPORT_SYMBOL(ra_sw_nat_npu_check_vxlan_hdr_list_hook);

int (*ra_sw_nat_restore_npu_pingpong_info)(struct sk_buff * skb,unsigned short cpu_info) = NULL;
EXPORT_SYMBOL(ra_sw_nat_restore_npu_pingpong_info);

int (*ra_sw_nat_add_nptv6_prefix)(unsigned char* ifname,unsigned short* s_prefix,unsigned int s_prefix_len,unsigned short* d_prefix,unsigned int d_prefix_len) = NULL;
EXPORT_SYMBOL(ra_sw_nat_add_nptv6_prefix);

EXPORT_SYMBOL(hwnat_clean_entry_by_dst_mac_hook);
EXPORT_SYMBOL(ra_hit_bind_force_to_cpu);
EXPORT_SYMBOL(multicast_flood_is_bind_hook);
EXPORT_SYMBOL(multicast_flood_find_entry_hook);
EXPORT_SYMBOL(hwnat_set_multicast_speed_enable_hook);
EXPORT_SYMBOL(hwnat_is_alive_pkt_hook);
EXPORT_SYMBOL(hwnat_skb_to_foe_hook);
EXPORT_SYMBOL(hwnat_set_special_tag_hook);
EXPORT_SYMBOL(hwnat_multicast_set_hwnat_info_hook);
EXPORT_SYMBOL(hwnat_delete_foe_entry_hook);
EXPORT_SYMBOL(hwnat_delete_foe_entry_hook_unlock);
EXPORT_SYMBOL(hwnat_is_multicast_entry_hook);
EXPORT_SYMBOL(hwnat_is_drop_entry_hook);
EXPORT_SYMBOL(hwnat_set_multicast_vlan_hook);
EXPORT_SYMBOL(multicast_speed_find_entry_hook);
EXPORT_SYMBOL(multicast_speed_learn_flow_hook);
EXPORT_SYMBOL(hwnat_set_rule_according_to_state_hook);
EXPORT_SYMBOL(hwnat_set_recover_info_hook);
EXPORT_SYMBOL(xpon_igmp_learn_flow_hook);
EXPORT_SYMBOL(multicast_hwnat_drop_entry_hook);
EXPORT_SYMBOL(xpon_igmp_exist);
EXPORT_SYMBOL(xpon_igmp_clear_flows);
EXPORT_SYMBOL(hwnat_set_wlan_multicast_hook);
EXPORT_SYMBOL(wan_multicast_drop_hook);
EXPORT_SYMBOL(wan_multicast_undrop_hook);
EXPORT_SYMBOL(wan_multicast_undrop_by_grpip_hook);
EXPORT_SYMBOL(wan_mvlan_change_hook);
EXPORT_SYMBOL(multicast_xmit_packet_hook);
//#endif

EXPORT_SYMBOL(ra_sw_nat_hook_rx);
EXPORT_SYMBOL(ra_sw_nat_mcst_offload);
EXPORT_SYMBOL(ra_sw_nat_ds_offload);
EXPORT_SYMBOL(ra_nat_cirpir_get_start_idx);
EXPORT_SYMBOL(ra_sw_nat_hook_update_dp);
EXPORT_SYMBOL(ra_sw_nat_hook_update_vlan);
EXPORT_SYMBOL(ra_sw_nat_local_in_tx);

EXPORT_SYMBOL(ra_sw_nat_hook_save_rxinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_restore_rxinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_save_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_restore_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_save_pptp_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_restore_pptp_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_save_gre_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_restore_gre_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_is_gre_offload_pkt);
EXPORT_SYMBOL(ra_sw_nat_hook_save_vxlan_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_restore_vxlan_txinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_is_vxlan_offload_pkt);
EXPORT_SYMBOL(ra_sw_nat_hook_is_hwnat_pkt);
EXPORT_SYMBOL(ra_sw_nat_hook_sendto_ppe);
EXPORT_SYMBOL(ra_sw_nat_hook_set_l2tp_dev);
EXPORT_SYMBOL(ra_sw_nat_hook_read_l2tp_dev);
EXPORT_SYMBOL(ra_sw_nat_hook_set_pptp_dev);
EXPORT_SYMBOL(ra_sw_nat_hook_read_pptp_dev);
EXPORT_SYMBOL(ra_sw_nat_rtsp_offload_restore);
EXPORT_SYMBOL(ra_sw_nat_rtsp_data_handle);
EXPORT_SYMBOL(ra_sw_nat_set_wan_acntid_hook);
EXPORT_SYMBOL(ra_sw_nat_clear_wan_acntid_hook);
EXPORT_SYMBOL(ra_sw_nat_get_wan_acntid_counter_hook);
EXPORT_SYMBOL(ra_sw_nat_set_mac_hook);
EXPORT_SYMBOL(ra_sw_nat_set_wan_meterid_hook);
EXPORT_SYMBOL(ra_sw_nat_rtspv6_npt_data_handle);

EXPORT_SYMBOL(ra_sw_nat_hook_tx);
EXPORT_SYMBOL(ra_sw_nat_hook_free);
EXPORT_SYMBOL(ra_sw_nat_hook_rxinfo);
EXPORT_SYMBOL(ra_sw_nat_hook_txq);
EXPORT_SYMBOL(ra_sw_nat_hook_magic);
EXPORT_SYMBOL(ra_sw_nat_hook_set_magic);
EXPORT_SYMBOL(ra_sw_nat_hook_xfer);
EXPORT_SYMBOL(ra_sw_nat_hook_is_alive_pkt);
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
EXPORT_SYMBOL(ra_sw_nat_set_mul_br_vid_hook);
EXPORT_SYMBOL(ra_sw_nat_get_mul_br_vid_hook);
#endif
EXPORT_SYMBOL(ra_sw_nat_hook_drop_packet);
EXPORT_SYMBOL(ra_sw_nat_hook_clean_table);
EXPORT_SYMBOL(ra_sw_nat_hook_clean_multicast_entry);
EXPORT_SYMBOL(ra_sw_nat_hook_foeentry);
EXPORT_SYMBOL(ra_sw_nat_hook_cpu_meter);
EXPORT_SYMBOL(ra_sw_nat_hook_is_entry_valid);
EXPORT_SYMBOL(multicast_get_stb_src_ip4);
EXPORT_SYMBOL(multicast_get_stb_src_ip6);
EXPORT_SYMBOL(ra_sw_nat_natv6_fast_handler);
EXPORT_SYMBOL(ra_sw_nat_vxlan_fast_handler);
EXPORT_SYMBOL(multicast_subcribe_group_hook);
EXPORT_SYMBOL(multicast_get_local_hook);

int (*hwnat_clean_lan_hook)(unsigned int) = NULL;
int (*hwnat_clean_wan_hook)(unsigned int) = NULL;
EXPORT_SYMBOL(hwnat_clean_lan_hook);
EXPORT_SYMBOL(hwnat_clean_wan_hook);

int hwnat_clean_lan(unsigned int port)
{
	if (hwnat_clean_lan_hook)
		return hwnat_clean_lan_hook(port);
	return 0;
}

int hwnat_clean_wan(unsigned int vid)
{
	if (hwnat_clean_wan_hook)
		return hwnat_clean_wan_hook(vid);
	return 0;
}
EXPORT_SYMBOL(hwnat_clean_lan);
EXPORT_SYMBOL(hwnat_clean_wan);

int (*sw_upstream_nat_rx_hook) (struct sk_buff * skb, int foe_tbl_index) = NULL;
int (*sw_upstream_nat_tx_hook)(struct sk_buff * skb, uint msg0, uint msg1, struct port_info* qdma_info)= NULL;
int (*sw_downstream_nat_rx_hook) (struct sk_buff * skb) = NULL;
int (*sw_downstream_nat_tx_hook)(struct sk_buff * skb, uint msg0, uint msg1, struct port_info * qdma_info)= NULL;
EXPORT_SYMBOL(sw_upstream_nat_rx_hook);
EXPORT_SYMBOL(sw_upstream_nat_tx_hook);
EXPORT_SYMBOL(sw_downstream_nat_rx_hook);
EXPORT_SYMBOL(sw_downstream_nat_tx_hook);

int (*qdma_wan_bm_transmit_packet_hook)(struct sk_buff *skb, int ringIdx, uint msg0, uint msg1) = NULL;
int (*qdma_lan_bm_transmit_packet_hook)(struct sk_buff *skb, int ringIdx, uint msg0, uint msg1) = NULL;
EXPORT_SYMBOL(qdma_wan_bm_transmit_packet_hook);
EXPORT_SYMBOL(qdma_lan_bm_transmit_packet_hook);

unsigned char (*get_swnat_clean_flag_hook)(void) = NULL;
void (*set_swnat_clean_flag_hook)(unsigned char val) = NULL;
EXPORT_SYMBOL(get_swnat_clean_flag_hook);
EXPORT_SYMBOL(set_swnat_clean_flag_hook);
int (*sw_threshold_get_hook)(int *min, int *max)= NULL;
EXPORT_SYMBOL(sw_threshold_get_hook);

#ifndef TCSUPPORT_IPSEC_PASSTHROUGH
#include <net/mtk_esp.h>
//when not open vpn passthrough function,below function do nothing
void
ipsec_esp_ouput_finish_pt(ipsec_finishpara_t* inputParams)
{
	return;
}

void ipsec_esp_input_finish_pt(ipsec_finishpara_t* inputParams)
{
	return;
}
int ipsec_esp_output_pt(ipsec_para_t* ipsecparams)
{
	return -1;
}

int ipsec_esp_input_pt(ipsec_para_t* ipsecparams)
{
	return -1;
}

EXPORT_SYMBOL(ipsec_esp_input_pt);
EXPORT_SYMBOL(ipsec_esp_input_finish_pt);
EXPORT_SYMBOL(ipsec_esp_output_pt);
EXPORT_SYMBOL(ipsec_esp_ouput_finish_pt);
#endif
int (*wan_speed_test_hook)(struct sk_buff*) = NULL;
EXPORT_SYMBOL(wan_speed_test_hook);
int (*wan_speed_test_pinpong_handle_hook)(struct sk_buff*) = NULL;
EXPORT_SYMBOL(wan_speed_test_pinpong_handle_hook);
int (*wan_speed_test_tso_hook)(struct sk_buff*) = NULL;
EXPORT_SYMBOL(wan_speed_test_tso_hook);

int (*ra_sw_wifi_hook_is_wifi_down) (struct sk_buff * skb, int band) = NULL;
EXPORT_SYMBOL(ra_sw_wifi_hook_is_wifi_down);
int (*ra_sw_wifi_hook_wifi_down_handle) (struct net_device *dev) = NULL;
EXPORT_SYMBOL(ra_sw_wifi_hook_wifi_down_handle);
int (*ra_sw_wifi_hook_wifi_up_handle) (struct net_device *dev) = NULL;
EXPORT_SYMBOL(ra_sw_wifi_hook_wifi_up_handle);

int (*ra_nat_delete_mc_drop_by_grp)(unsigned int grp_addr)= NULL;
EXPORT_SYMBOL(ra_nat_delete_mc_drop_by_grp);
int (*send_to_hsgmii_hook)(struct sk_buff* skb, int hsgmii_mask) = NULL;
EXPORT_SYMBOL(send_to_hsgmii_hook);


int (*ra_sw_nat_hit_ifc_fast_handler) (struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hit_ifc_fast_handler);

int (*ra_sw_nat_hook_ifc_hit_info) (struct sk_buff* skb, unsigned char hit, unsigned int ifc_id) = NULL;
EXPORT_SYMBOL(ra_sw_nat_hook_ifc_hit_info);


