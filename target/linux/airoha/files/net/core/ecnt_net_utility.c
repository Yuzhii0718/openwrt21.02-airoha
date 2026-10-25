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
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
#include <linux/proc_fs.h>
#endif
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/if_pppox.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <linux/module.h>
#include <linux/ctype.h>
#ifdef TCSUPPORT_IPV6
#include <linux/ipv6.h>
#endif
#if defined(TCSUPPORT_CT_VOIP_QOS)
#include <linux/ecnt_voip_proc.h>
#endif
#include <linux/ecnt_vlan_bind.h>
#include <net/addrconf.h>
#include "ecnt_net_core.h"
#include "../ipv6/ecnt_net_ipv6.h"
#if defined(TCSUPPORT_CT_JOYME2)
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
#include <spi/spi_nand_flash.h>
#endif
#include <flash_layout/tc_partition.h>
#endif
#include <ecnt_hook/ecnt_hook_fe.h>

#ifdef TCSUPPORT_CPU_ARMV8
#include <asm-generic/irq_regs.h>
#if defined(TCSUPPORT_CPU_ARMV8_64)
#include <linux/syscalls.h>
#endif
#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#include <modules/wifi_global_def.h>
#endif
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define PPP_IP		0x21	/* Internet Protocol */
#define PPP_IPV6	0x57	/* Internet Protocol Version 6 */

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define MAX_DEV_BANDWIDTH_NUM 64

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#ifndef _LINUX_BITOPS_H
#define BIT(n)                          ((unsigned int) 1 << (n))
#endif /* BIT */
	unsigned int wifi_type_bit = 0;
	EXPORT_SYMBOL(wifi_type_bit);
	unsigned int wifi_type_bit_5g = 0;
	EXPORT_SYMBOL(wifi_type_bit_5g);
	//only for transfer
	unsigned int wifi_type_e2p_2g = 0;
	unsigned int wifi_type_e2p_5g = 0;
	//2g variable 
	unsigned char WLAN_2G_CHIPYPTE[WLAN_2G_MAX_ID] = {0};
	EXPORT_SYMBOL(WLAN_2G_CHIPYPTE);
	//5g variable 
	unsigned char WLAN_5G_CHIPYPTE[WLAN_5G_MAX_ID] = {0};
	EXPORT_SYMBOL(WLAN_5G_CHIPYPTE);
#endif

#ifdef TCSUPPORT_DOWNSTREAM_QOS
/*use for set voip rx port in application, shnwind add 20110215.*/
unsigned short int voip_rx_port[VOIP_RX_PORT_NUM] = {0};
EXPORT_SYMBOL(voip_rx_port);
char downstream_qos_enable = 0;
EXPORT_SYMBOL(downstream_qos_enable);
#endif

int remove_proc_flag = 0;

int *gPpeFoeBase = NULL;
EXPORT_SYMBOL(gPpeFoeBase);
int *g_foe_ext = NULL;
EXPORT_SYMBOL(g_foe_ext);

unsigned int queuemark_enable = 0;
EXPORT_SYMBOL(queuemark_enable);
unsigned int queuemark_shift = 0;
EXPORT_SYMBOL(queuemark_shift);
unsigned int queuemark_width = 0;
EXPORT_SYMBOL(queuemark_width);
int ext_switch_sel = -1;
EXPORT_SYMBOL(ext_switch_sel);
u8 ext_swicVendor=0;
EXPORT_SYMBOL(ext_swicVendor);
u8 dev_ratelimit_use_channel=0;
EXPORT_SYMBOL(dev_ratelimit_use_channel);


typedef struct devBandwidth_s{
	unsigned char mac[6];
	unsigned long long upBytes;
	unsigned long long downBytes;
	unsigned int upRate;
	unsigned int downRate;
	int valid;
}devBandwidth_t;	
typedef struct devBandwidthList_s{	
	int enable;
	int portMultiMacEn;
	struct devBandwidth_s bandwidthList[MAX_DEV_BANDWIDTH_NUM];
}devBandwidthList_t;

char *wlan_24g_name_array[] = {"ra0", "ra1", "ra2", NULL};
char *wlan_5g_name_array[] = {"rax0", "rax1", "rax2", NULL};
char *wlan_apcli24g_name_array[] = {"apcli0", "apcli1", "apcli2", NULL};
char *wlan_apcli5g_name_array[] = {"apclii0", "apclii1", "apclii2", NULL};

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
struct devBandwidthList_s *gHwBandwidthList = NULL;
EXPORT_SYMBOL(gHwBandwidthList);
struct devBandwidthList_s *gDevBandwidthList = NULL;
EXPORT_SYMBOL(gDevBandwidthList);
int wlanledsta = 0;
/* appended: VLAN ids and PON hsgmii interface, used by the newer ae_wan */
u16 lan_vlan = 0;
u16 wan_vlan = 0;
u16 mgr_vlan = 0;
EXPORT_SYMBOL(lan_vlan);
EXPORT_SYMBOL(wan_vlan);
EXPORT_SYMBOL(mgr_vlan);
char *hsgmii_pon_itf = "eth5";
EXPORT_SYMBOL(hsgmii_pon_itf);
int wlan11acledsta = 0;
EXPORT_SYMBOL(wlanledsta);
EXPORT_SYMBOL(wlan11acledsta);

int (*match_multicast_vtag_check)
(struct sk_buff *skb, struct net_device *vdev);
EXPORT_SYMBOL(match_multicast_vtag_check);
#if !defined(TCSUPPORT_CT_VLAN_TAG)
int (*match_multicast_vtag)(struct sk_buff *skb, struct net_device *vdev);
EXPORT_SYMBOL(match_multicast_vtag);
#endif
vlanBind_t vBindArray[MAX_LAN_PORT_NUM][MAX_VLAN_GROUP];
EXPORT_SYMBOL(vBindArray);
int (*vlanbind_check_group_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb);
EXPORT_SYMBOL(vlanbind_check_group_hook);
int (*wifi_eth_fast_tx_hook)(struct sk_buff *skb);
EXPORT_SYMBOL(wifi_eth_fast_tx_hook);
int (*offload_eth_fast_tx_hook)(struct sk_buff *skb, int channel);
EXPORT_SYMBOL(offload_eth_fast_tx_hook);
int (*xsi_fast_tx_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(xsi_fast_tx_hook);
#ifdef TCSUPPORT_TEST_VWTEST
int (*wifi_eth_fast_tx_ecnt_hook)(struct sk_buff *skb)= NULL;
EXPORT_SYMBOL(wifi_eth_fast_tx_ecnt_hook);
#endif
void (*wlan_counter_print_hook)(struct file *fp) = NULL;
EXPORT_SYMBOL(wlan_counter_print_hook);
void (*qdma_err_drop_counters_hook)(unsigned short *qdma_tx_err, unsigned short *qdma_rx_err) = NULL;
EXPORT_SYMBOL(qdma_err_drop_counters_hook);

#if defined(TCSUPPORT_CT_DS_LIMIT)
int (*dslimit_remarkQueue_hook)( struct sk_buff *skb, int up_dw );
EXPORT_SYMBOL(dslimit_remarkQueue_hook);
#endif


int (*fe_resource_mark_meter_hook)( struct sk_buff *skb, int dir );
EXPORT_SYMBOL(fe_resource_mark_meter_hook);

/*
 * Hooks registered by the QDMA driver and read by optional features (PON LLID
 * accounting, Ookla speed test, TR-471 statistics, QoS buffer mode). The newer
 * vendor kernel exports them unconditionally; this kernel predates them.
 *
 * They are exported here as NULL so that a QDMA driver built from newer
 * sources can register into them and every reader keeps its NULL check. None
 * of the modules in this tree other than QDMA sets them, so the features stay
 * disabled, exactly as before.
 */
int (*fe_resource_mark_llid_hook)(struct sk_buff *skb, u8 llid, u8 dir) = NULL;
EXPORT_SYMBOL(fe_resource_mark_llid_hook);
int (*ookla_offload_rcv_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(ookla_offload_rcv_hook);
int (*qdma_get_txring_unused_dscp_cnt_hook)(int ring_idx) = NULL;
EXPORT_SYMBOL(qdma_get_txring_unused_dscp_cnt_hook);
void (*set_qdma_lan_qos_buffer_mode_hook)(int mode) = NULL;
EXPORT_SYMBOL(set_qdma_lan_qos_buffer_mode_hook);
void (*get_tr471_rx_msg_hook)(int rx_ring, unsigned int *rx_byte_cnt_l,
		unsigned int *rx_byte_cnt_h, unsigned int *err_cnt,
		unsigned int *drop_cnt) = NULL;
EXPORT_SYMBOL(get_tr471_rx_msg_hook);

/*
 * Source port identifiers used by the ether driver built from newer sources.
 * 0xffff is the "not assigned yet" value the vendor kernel uses; the driver
 * overwrites them while probing its ports.
 */
unsigned int pcie0_srcport = 0xffff;
EXPORT_SYMBOL(pcie0_srcport);
unsigned int pcie1_srcport = 0xffff;
EXPORT_SYMBOL(pcie1_srcport);
unsigned int usb_srcport = 0xffff;
EXPORT_SYMBOL(usb_srcport);
unsigned int eth_srcport = 0xffff;
EXPORT_SYMBOL(eth_srcport);
int (*fe_resource_mark_acnt_hook)( struct sk_buff *skb, int dir );
EXPORT_SYMBOL(fe_resource_mark_acnt_hook);

int (*fe_resource_mark_wan_idx_hook)( struct sk_buff *skb, u8 wan_index, u8 dir) = NULL;
EXPORT_SYMBOL(fe_resource_mark_wan_idx_hook);

void (*wlan_to_lan_hook)(struct sk_buff *skb, u8 local) = NULL;
EXPORT_SYMBOL(wlan_to_lan_hook);


int (*fe_resource_get_set_wan_idx_hook)( struct sk_buff *skb, u8 dir) = NULL;
EXPORT_SYMBOL(fe_resource_get_set_wan_idx_hook);
/*-------------------sw_rps_for_wifi---------------------------*/
//#ifdef TCSUPPORT_WLAN_SW_RPS
int (*fromWlan5GPktRpsHandle_hook)(void* pRxPacket) = NULL;
EXPORT_SYMBOL(fromWlan5GPktRpsHandle_hook);

int (*toWlan5GPktRpsHandle_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(toWlan5GPktRpsHandle_hook);

void (*rps5GQueueDrop_hook)(unsigned int queue, unsigned int *count) = NULL;
EXPORT_SYMBOL(rps5GQueueDrop_hook);

int (*qdma_to_wifi_fast_tx_hook)(struct sk_buff *skb, int index) = NULL;
EXPORT_SYMBOL(qdma_to_wifi_fast_tx_hook);

int (*qdma_to_wifi2g_fast_tx_hook)(struct sk_buff *skb, int index) = NULL;
EXPORT_SYMBOL(qdma_to_wifi2g_fast_tx_hook);

void (*clear_detect_rx_tx_info_hook)(void) = NULL;
EXPORT_SYMBOL(clear_detect_rx_tx_info_hook);

int (*toWlan2GPktRpsHandle_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(toWlan2GPktRpsHandle_hook);

int (*fromWlan2GPktRpsHandle_hook)(void* pRxPacket) = NULL;
EXPORT_SYMBOL(fromWlan2GPktRpsHandle_hook);
int (*ecnt_wifi_rx_rps_2g_hook)(struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(ecnt_wifi_rx_rps_2g_hook);

unsigned int (*ecnt_7603_get_ampdu_pkt_hook)(void) = NULL;
EXPORT_SYMBOL(ecnt_7603_get_ampdu_pkt_hook);
unsigned int (*ecnt_7603_get_rx_pkt_hook)(void) = NULL;
EXPORT_SYMBOL(ecnt_7603_get_rx_pkt_hook);

int (*ecnt_wifi_rx_rps_hook)(struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(ecnt_wifi_rx_rps_hook);

int (*ecnt_wifi_tx_rps_hook)(struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(ecnt_wifi_tx_rps_hook);

int (*get_WifitolanRps_hook)(void) = NULL;
EXPORT_SYMBOL(get_WifitolanRps_hook);
int (*get_Wifi2GtolanRps_hook)(void) = NULL;
EXPORT_SYMBOL(get_Wifi2GtolanRps_hook);

int (*get_LantoWifiRps_hook)(void) = NULL;
EXPORT_SYMBOL(get_LantoWifiRps_hook);

extern void(*get_sw_rps_2g_hook)(int *RxOn, int *WLanCPU, int *TxOn_2G, int *LanCPU) = NULL;
EXPORT_SYMBOL(get_sw_rps_2g_hook);

extern void(*get_sw_rps_hook)(int *RxOn, int *WLanCPU, int *TxOn_2G, int *LanCPU) = NULL;
EXPORT_SYMBOL(get_sw_rps_hook);

int (*ecnt_set_2Gwifi_rps_hook)(int RxOn, int WLanCPU, int TxOn_2G, int LanCPU) = NULL;
EXPORT_SYMBOL(ecnt_set_2Gwifi_rps_hook);

int (*ecnt_set_wifi_rps_hook)(int RxOn, int WLanCPU, int TxOn, int LanCPU) = NULL;
EXPORT_SYMBOL(ecnt_set_wifi_rps_hook);

int (*traffic_process_hook)(struct sk_buff *skb, struct sock *sk, unsigned int *res);
EXPORT_SYMBOL(traffic_process_hook);
int (*dev_bandwidth_hook_get_cnt) (dev_bandwidth_account_t *dev_account) = NULL;
EXPORT_SYMBOL(dev_bandwidth_hook_get_cnt);
int (*dev_bandwidth_hook_clear_cnt) (void) = NULL;
EXPORT_SYMBOL(dev_bandwidth_hook_clear_cnt);
/* TCSUPPORT_FORWARD_LEFT_TO_RIGHT */
int (*vxlan_left_to_right_handle_hook)(char *data_ptr, unsigned short *data_len, unsigned short foe_index, int *data_offset, unsigned short *VirIfIdx) = NULL;
EXPORT_SYMBOL(vxlan_left_to_right_handle_hook);

int (*vxlan_left_to_right_xmit_hook) (char *data_ptr, unsigned short *data_len, unsigned short foe_index, struct net_device *dev) = NULL;
EXPORT_SYMBOL(vxlan_left_to_right_xmit_hook);
void (*update_vxlan_up_info_hook)(unsigned int tcont, unsigned int gem, unsigned int vlan, unsigned char* smac, unsigned char* dmac) = NULL;
EXPORT_SYMBOL(update_vxlan_up_info_hook);
void (*update_nptv6_info_hook)(unsigned int chn, unsigned int stag, unsigned int *ipv6_ip, int dir) = NULL;
EXPORT_SYMBOL(update_nptv6_info_hook);

/* TCSUPPORT_FORWARD_LEFT_TO_RIGHT end */

//#endif
/*-------------------sw_rps_for_wifi---------------------------*/

#ifdef TCSUPPORT_ECNT_1588v2
/* PTP 1588 packet hook function */
int (*ptp_1588_tx_ptk_check_hook)(struct sk_buff *skb, uint32 *msg0, uint32 *msg1) = NULL;
EXPORT_SYMBOL(ptp_1588_tx_ptk_check_hook);

void (*ptp_1588_tx_tstamp_hook)(uint32 ptpDev) = NULL;
EXPORT_SYMBOL(ptp_1588_tx_tstamp_hook);

int (*ptp_1588_rx_tstamp_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(ptp_1588_rx_tstamp_hook);
#endif

/*-------------------npu_wifi_offload---------------------------*/
#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD

struct sk_buff *(*fromHostadptPktHandle_hook)(unsigned int ringIdx,
	unsigned char* preschedule, unsigned int* vnd_spec) = NULL;
EXPORT_SYMBOL(fromHostadptPktHandle_hook);
int (*toHostadptPktHandle_hook)(unsigned char band,
	struct sk_buff *skb, unsigned char *vnd, unsigned short vnd_len) = NULL;
EXPORT_SYMBOL(toHostadptPktHandle_hook);

void (*hostdapt_enable_int_hook)(unsigned int ringIdx) = NULL;
EXPORT_SYMBOL(hostdapt_enable_int_hook);
void (*hostdapt_disable_int_hook)(unsigned int ringIdx) = NULL;
EXPORT_SYMBOL(hostdapt_disable_int_hook);

void (*hostdapt_registe_wifitask_hook)(unsigned int ringIdx, void *func) = NULL;
EXPORT_SYMBOL(hostdapt_registe_wifitask_hook);
#define BAND0_ON_CPU_PATH "tc3162/npu_band0_on_cpu"
#ifdef TCSUPPORT_CPU_AN7552
#ifdef TCSUPPORT_NPU_TUNNEL_OFFLAD
unsigned char isNpuBand0OnCpu = 1;
#else
unsigned char isNpuBand0OnCpu = 0;
#endif
#else
unsigned char isNpuBand0OnCpu = 0;
#endif
EXPORT_SYMBOL(isNpuBand0OnCpu);
#endif
/*-------------------npu_wifi_offload---------------------------*/

int (*xsi_max_tx_hook)(struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(xsi_max_tx_hook);

/*for ALG switch*/
/*0 means switch off; 1 means switch on; 2 means switch not set*/
int nf_conntrack_ftp_enable  __read_mostly = 1;
EXPORT_SYMBOL_GPL(nf_conntrack_ftp_enable);
int nf_conntrack_sip_enable  __read_mostly = 1;
EXPORT_SYMBOL_GPL(nf_conntrack_sip_enable);
int nf_conntrack_h323_enable  __read_mostly = 1;
EXPORT_SYMBOL_GPL(nf_conntrack_h323_enable);
int nf_conntrack_rtsp_enable  __read_mostly = 1;
EXPORT_SYMBOL_GPL(nf_conntrack_rtsp_enable);
int nf_conntrack_l2tp_enable __read_mostly = 2;
EXPORT_SYMBOL_GPL(nf_conntrack_l2tp_enable);
int nf_conntrack_ipsec_enable __read_mostly = 2;
EXPORT_SYMBOL_GPL(nf_conntrack_ipsec_enable);
int nf_conntrack_pptp_enable __read_mostly = 1;
EXPORT_SYMBOL_GPL(nf_conntrack_pptp_enable);
int nf_conntrack_portscan_enable __read_mostly = 0;
EXPORT_SYMBOL_GPL(nf_conntrack_portscan_enable);
int nf_conntrack_ftp_port __read_mostly = 21;
EXPORT_SYMBOL_GPL(nf_conntrack_ftp_port);
int nf_conntrack_esp_timeout __read_mostly = 30;
EXPORT_SYMBOL_GPL(nf_conntrack_esp_timeout);
int nf_conntrack_rtcp_enable  __read_mostly = 1;
EXPORT_SYMBOL_GPL(nf_conntrack_rtcp_enable);
unsigned int nf_conntrack_rtsp_src_ip4_mask __read_mostly = 0xFFFFFFFF;
EXPORT_SYMBOL_GPL(nf_conntrack_rtsp_src_ip4_mask);
int nf_conntrack_tcp_max_session __read_mostly = -1;
EXPORT_SYMBOL_GPL(nf_conntrack_tcp_max_session);
atomic_t nf_conntrack_tcp_session_num;
EXPORT_SYMBOL_GPL(nf_conntrack_tcp_session_num);

struct net_device* (*portbind_get_outdev_by_indev_ct_hook)(unsigned char* indev_name);
EXPORT_SYMBOL(portbind_get_outdev_by_indev_ct_hook);


int (*soft_ratelimit_enqueue_hook) (struct sk_buff * skb,unsigned int queue_idx) = NULL;
EXPORT_SYMBOL(soft_ratelimit_enqueue_hook);

int (*soft_ratelimit_set_queue_hook) (unsigned int queue,unsigned int rate,int(* func)(struct sk_buff * skb)) = NULL;
EXPORT_SYMBOL(soft_ratelimit_set_queue_hook);

int (*soft_ratelimit_allocatequeue_hook)( unsigned int num) = NULL;
EXPORT_SYMBOL(soft_ratelimit_allocatequeue_hook);

int (*soft_ratelimit_recyclequeue_hook)( unsigned int start,unsigned num) = NULL;
EXPORT_SYMBOL(soft_ratelimit_recyclequeue_hook);

int (*soft_cirpir_queue_check_hook) (struct sk_buff * skb,unsigned int queue_idx) = NULL;
EXPORT_SYMBOL(soft_cirpir_queue_check_hook);

int (*soft_cirpir_rate_check_hook) (struct sk_buff * skb,unsigned int queue_idx,int *ret_queue) = NULL;
EXPORT_SYMBOL(soft_cirpir_rate_check_hook);
int (*soft_ratelimit_cds_qdma_hook) (unsigned int idx, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(soft_ratelimit_cds_qdma_hook);

int (*soft_cirpir_set_cir_queue_hook)( unsigned int queue_idx, unsigned int cirqueue_idx) = NULL;
EXPORT_SYMBOL(soft_cirpir_set_cir_queue_hook);
#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
int SmbdTxSpeedOn = 1;
EXPORT_SYMBOL(SmbdTxSpeedOn);

int SmbdTxSpeedCnt0 = 0;
EXPORT_SYMBOL(SmbdTxSpeedCnt0);

int SmbdTxSpeedCnt1 = 0;
EXPORT_SYMBOL(SmbdTxSpeedCnt1);
#endif
int ShortCutTxSpeedCnt0 = 0;
EXPORT_SYMBOL(ShortCutTxSpeedCnt0);

int ShortCutTxSpeedCnt1 = 0;
EXPORT_SYMBOL(ShortCutTxSpeedCnt1);

#if defined(TCSUPPORT_CT_JOYME4)
int g_capable_user_root_switch = 0;
EXPORT_SYMBOL(g_capable_user_root_switch);
#endif

int (*soft_qdma_ratelimit_enqueue_hook) (struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(soft_qdma_ratelimit_enqueue_hook);

int (*soft_qdma_ratelimit_dequeue_hook) (struct sk_buff * skb) = NULL;
EXPORT_SYMBOL(soft_qdma_ratelimit_dequeue_hook);

u8 def_mac_addr[6] = {0x00, 0x00, 0xaa, 0xbb, 0xcc, 0xff};
EXPORT_SYMBOL(def_mac_addr);

void (*macSend_hook)(u32 chanId, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(macSend_hook);

#ifdef TCSUPPORT_WLAN_INODE
int inode_npu_en = 1;
EXPORT_SYMBOL(inode_npu_en);
#endif

int g_Multi_To_One = 0;
#if defined(TCSUPPORT_TEST_VWTEST) || defined(TCSUPPORT_CMCC_ENTERPRISE)
unsigned int ixia_tx_counter0[3] = {0, 0, 0};
unsigned int ixia_tx_counter1[3] = {0, 0, 0};
//int g_Multi_To_One = 0;

EXPORT_SYMBOL(ixia_tx_counter0);
EXPORT_SYMBOL(ixia_tx_counter1);
EXPORT_SYMBOL(g_Multi_To_One);

int forTx2G = 0;
int forTx5G = 0;
int forRx5G = 0;
int Tx_shortcut_level = 5;
int shortGIEnable = 0;
int trafficSkbLen = 1518;
int RxMapLen_now = 0;
EXPORT_SYMBOL(forTx2G);
EXPORT_SYMBOL(forTx5G);
EXPORT_SYMBOL(forRx5G);
EXPORT_SYMBOL(Tx_shortcut_level);
EXPORT_SYMBOL(shortGIEnable);
EXPORT_SYMBOL(trafficSkbLen);
EXPORT_SYMBOL(RxMapLen_now);
unsigned int ecnt_shortcut_cnt[64];
EXPORT_SYMBOL(ecnt_shortcut_cnt);

unsigned char g_Global_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int g_Global_index = -1;
int g_Global_skbLen = 1500;
EXPORT_SYMBOL(g_Global_MAC);
EXPORT_SYMBOL(g_Global_index);
EXPORT_SYMBOL(g_Global_skbLen);

int qmTxLockEnable = 0;
int qmTxLockTryCounter = 0;
EXPORT_SYMBOL(qmTxLockEnable);
EXPORT_SYMBOL(qmTxLockTryCounter);

int drop_KA_Enable = 0;
EXPORT_SYMBOL(drop_KA_Enable);

unsigned int keep_alive_pkt_cnt = 0;
EXPORT_SYMBOL(keep_alive_pkt_cnt);

unsigned int keep_alive_pkt_cnt1[3] = {0, 0, 0};
EXPORT_SYMBOL(keep_alive_pkt_cnt1);

int ecnt_tx_flag = 0;
EXPORT_SYMBOL(ecnt_tx_flag);

struct net_device *rai0_dev = NULL;
EXPORT_SYMBOL(rai0_dev);

struct net_device *ra0_dev = NULL;
EXPORT_SYMBOL(ra0_dev);

int detectRxTxEnable = 0;
EXPORT_SYMBOL(detectRxTxEnable);
#endif

void* foe_ext_export = NULL;
EXPORT_SYMBOL(foe_ext_export);

/* TCSUPPORT_FORWARD_LEFT_TO_RIGHT */
int aggressive_offload_short_cut_mode = 0;
EXPORT_SYMBOL(aggressive_offload_short_cut_mode);

int left_to_right_down_mode = 0;
EXPORT_SYMBOL(left_to_right_down_mode);
int two_way_left_to_right_flag = 0;
EXPORT_SYMBOL(two_way_left_to_right_flag);
int left_to_right_test_mode = 0;
EXPORT_SYMBOL(left_to_right_test_mode);
unsigned char NATv6_USE_HIGH_PRIORITY = 0;
EXPORT_SYMBOL(NATv6_USE_HIGH_PRIORITY);
/* TCSUPPORT_FORWARD_LEFT_TO_RIGHT */

unsigned short port_vlan_bind_vid = 0;
EXPORT_SYMBOL(port_vlan_bind_vid);

#ifdef TCSUPPORT_IFC_EN
unsigned int ifcSendToPpe = 0;
EXPORT_SYMBOL(ifcSendToPpe);
#endif

unsigned int pcie0_force_dstq = 0;
EXPORT_SYMBOL(pcie0_force_dstq);
unsigned int pcie1_force_dstq = 0;
EXPORT_SYMBOL(pcie1_force_dstq);
unsigned int usb_force_dstq = 0;
EXPORT_SYMBOL(usb_force_dstq);
unsigned int eth_force_dstq = 0;
EXPORT_SYMBOL(eth_force_dstq);

char *hsgmii_pcie0_itf = "eth2";
char *hsgmii_pcie1_itf = "eth3";
char *hsgmii_usb_itf = "eth1";
char *hsgmii_eth_itf = "eth4";
EXPORT_SYMBOL(hsgmii_pcie0_itf);
EXPORT_SYMBOL(hsgmii_pcie1_itf);
EXPORT_SYMBOL(hsgmii_usb_itf);
EXPORT_SYMBOL(hsgmii_eth_itf);

struct net_device *hsgmii_pcie0_dev;
struct net_device *hsgmii_pcie1_dev;
struct net_device *hsgmii_usb_dev;
struct net_device *hsgmii_eth_dev;
EXPORT_SYMBOL(hsgmii_pcie0_dev);
EXPORT_SYMBOL(hsgmii_pcie1_dev);
EXPORT_SYMBOL(hsgmii_usb_dev);
EXPORT_SYMBOL(hsgmii_eth_dev);

int wlan_name_mode = 0;

int (*xsi_linkst_monitor_hook)(void) = NULL;
EXPORT_SYMBOL(xsi_linkst_monitor_hook);

/* PORTBIND & VLAN BIND FUNCTIONS */
#ifdef TCSUPPORT_PORTBIND
int (*arht_pbind_vbind_check_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(arht_pbind_vbind_check_hook);
int (*arht_pbind_vbind_data_process_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff **skb) = NULL;
EXPORT_SYMBOL(arht_pbind_vbind_data_process_hook);
int (*arht_pbind_vbind_bridge_check_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(arht_pbind_vbind_bridge_check_hook);

int (*portbind_sw_hook)(void) = NULL;
void (*portbind_update_hook)(struct net_device *dev, int type) = NULL;
int (*portbind_check_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(portbind_sw_hook);
EXPORT_SYMBOL(portbind_check_hook);
EXPORT_SYMBOL(portbind_update_hook);

int (*vlanbind_active_hook)(void) = NULL;
int (*vlanbind_entry_active_hook)(int i) = NULL;
EXPORT_SYMBOL(vlanbind_active_hook);
EXPORT_SYMBOL(vlanbind_entry_active_hook);

int (*portbind_check_bind_lan2)(int bind_index) = NULL;
EXPORT_SYMBOL(portbind_check_bind_lan2);
int (*portbind_check_bind_wantype)(char *landev, int bind_index) = NULL;
EXPORT_SYMBOL(portbind_check_bind_wantype);

int (*portbind_check_mc_hook)(struct net_device *in_dev,  struct net_device *out_dev) = NULL;
EXPORT_SYMBOL(portbind_check_mc_hook);

void (*arht_netif_recv_portbind_hook)(struct sk_buff *skb,struct net_device *orig_dev) = NULL;
EXPORT_SYMBOL(arht_netif_recv_portbind_hook);

#if !defined(TCSUPPORT_CT) /* reserved for fix compile error.*/
int (*portbind_sw_prior_hook)(struct sk_buff *skb);
struct net_device* (*portbind_get_outdev_by_indev_hook)(unsigned char* indev_name);
EXPORT_SYMBOL(portbind_sw_prior_hook);
EXPORT_SYMBOL(portbind_get_outdev_by_indev_hook);
#endif
#endif

extern int udp_rcv(struct sk_buff *skb);
extern void show_regs(struct pt_regs * regs);
extern int ecnt_kernel_fs_read_check(struct file *osfd);
extern ssize_t ecnt_kernel_fs_read(struct file *osfd, char __user *pDataPtr, size_t readLen, loff_t *fpos);
extern int ecnt_kernel_fs_write_check(struct file *osfd);
extern ssize_t ecnt_kernel_fs_write(struct file *osfd, const char __user *pDataPtr, size_t writeLen, loff_t *fpos);
extern int nandflash_erase(unsigned long offset, unsigned long len);
extern int nandflash_write(unsigned long to, unsigned long len, u32 *retlen, unsigned char *buf);

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/


#if (defined(TCSUPPORT_XPON_MAPPING) || defined(TCSUPPORT_PON_VLAN) || defined(TCSUPPORT_XPON_IGMP)) && defined(TCSUPPORT_PON_IP_HOST)
#define br_port_get_rcu(dev) \
	 ((struct net_bridge_port *) rcu_dereference(dev->rx_handler_data))
#define PON_IP_HOST_WANIF_PATH "tc3162/pon_wanIf"
#define WLANIF_NAME_PATH "tc3162/wlanIfname"

char voip_wanIf[8] = "NULL";
char tr069_wanIf[8] = "nas0_0";

static int pon_wanIf_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	 int len = 0;

	 len = sprintf(buf,"voip %s\n tr069 %s", voip_wanIf, tr069_wanIf );

	 if (len < off + count)
		 *eof = 1;
 
	 len -= off;
	 *start = buf + off;
	 if(len > count)
		 len = count;
	  if(len < 0)
		 len = 0;

	return len;
}
 
static int pon_wanIf_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char wan_type[8] = {0};
	char wan_name[8] = {0};
	char temp[16] = {0};
	
	if (count > 15)
		return -EFAULT;

	if (copy_from_user(temp, buffer, count))
		return -EFAULT;

	sscanf(temp, "%s %s", wan_type, wan_name);
	
	if(memcmp(wan_name, "nas", 3) != 0)
		return -EFAULT;
	
	if(strcmp(wan_type, "voip") == 0)
		strcpy(voip_wanIf, wan_name);
	else if(strcmp(wan_type, "tr069") == 0)
		strcpy(tr069_wanIf, wan_name);
	else
		return -EFAULT;
		
	return count;
}

int isBridgeWan(struct net_device *dev)
{
	 struct net_bridge_port *port;
 
	 if (dev == NULL) 
		 return 0;
 
	 if ((port = br_port_get_rcu(dev)) == NULL)
		 return 0;
	 
	 return 1;
}


/****************************************************************************
**function name
	ecnt_wanIf_proc_init
**description:
	wanIf proc init
**return 
**parameter:
****************************************************************************/
void ecnt_wanIf_proc_init(void)
{
	struct proc_dir_entry *voip_wanIf_proc = NULL;

	voip_wanIf_proc = create_proc_entry(PON_IP_HOST_WANIF_PATH, 0, NULL);
	voip_wanIf_proc->read_proc = pon_wanIf_read_proc;
	voip_wanIf_proc->write_proc = pon_wanIf_write_proc;

	return;
}
/****************************************************************************
**function name
	ecnt_wanIf_proc_deinit
**description:
	wanIf proc destroy
**return 
**parameter:
****************************************************************************/
void ecnt_wanIf_proc_deinit(void)
{
	remove_proc_entry(PON_IP_HOST_WANIF_PATH, NULL);
}
#endif

int wlanIf_name_check(int type, char *name)
{
	int i = 0;

	if(name == NULL)
		return 0;

	if(type == 0)
	{
		while(wlan_24g_name_array[i]) {
			if (strcmp(name, wlan_24g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		return 0;
	} else if(type == 1) {
		while(wlan_5g_name_array[i]) {
			if (strcmp(name, wlan_5g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		return 0;
	} else if(type == 2) {
		while(wlan_24g_name_array[i]) {
			if (strcmp(name, wlan_24g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		while(wlan_5g_name_array[i]) {
			if (strcmp(name, wlan_5g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		return 0;
	} else if(type == 3) {
		while(wlan_apcli24g_name_array[i]) {
			if (strcmp(name, wlan_apcli24g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		return 0;
	} else if(type == 4) {
		while(wlan_apcli5g_name_array[i]) {
			if (strcmp(name, wlan_apcli5g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		return 0;
	} else if(type == 5) {
		while(wlan_apcli24g_name_array[i]) {
			if (strcmp(name, wlan_apcli24g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		while(wlan_apcli5g_name_array[i]) {
			if (strcmp(name, wlan_apcli5g_name_array[i]) == 0)
				return 1;
        	i++;
		}
		return 0;
	} else
		return 0;
}

static int wlanIf_name_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	 int len = 0;
	 if (wlan_name_mode == 0)
		 len = sprintf(buf,"wlan name is default 0\n");
	 else
		 len = sprintf(buf,"wlan name mode is %d\n", wlan_name_mode);

	 if (len < off + count)
		 *eof = 1;

	 len -= off;
	 *start = buf + off;
	 if(len > count)
		 len = count;
	  if(len < 0)
		 len = 0;

	return len;
}

static int wlanIf_name_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char temp[16] = {0};

	if (count > 2)
		return -EFAULT;

	if (copy_from_user(temp, buffer, count))
		return -EFAULT;

	sscanf(temp, "%d", &wlan_name_mode);

	return count;
}

/****************************************************************************
**function name
	ecnt_wlanIf_name_proc_init
**description:
	wlanIf proc init
**return
**parameter:
****************************************************************************/
void ecnt_wlanIf_name_proc_init(void)
{
	struct proc_dir_entry *wlanIf_proc = NULL;

	wlanIf_proc = create_proc_entry(WLANIF_NAME_PATH, 0, NULL);
	wlanIf_proc->read_proc = wlanIf_name_read_proc;
	wlanIf_proc->write_proc = wlanIf_name_write_proc;

	return;
}

/****************************************************************************
**function name
	ecnt_wlanIf_name_proc_deinit
**description:
	wlanIf proc destroy
**return
**parameter:
****************************************************************************/
void ecnt_wlanIf_name_proc_deinit(void)
{
	remove_proc_entry(WLANIF_NAME_PATH, NULL);
}

int isVoipWan(struct net_device *dev){
#if (defined(TCSUPPORT_XPON_MAPPING) || defined(TCSUPPORT_PON_VLAN) || defined(TCSUPPORT_XPON_IGMP)) && defined(TCSUPPORT_PON_IP_HOST)
	if(dev == NULL || isBridgeWan(dev))
		return 0;

#if defined(TCSUPPORT_CT)
	if(dev->name[0] == 'n' && dev->name[3] == voip_wanIf[3] && dev->name[5] == voip_wanIf[5])
#else
	if(dev->name[0] == 'n' && dev->name[3] == voip_wanIf[3])
#endif
		return 1;
#endif	
	return 0;
}

EXPORT_SYMBOL(isVoipWan);

#if defined(TCSUPPORT_CMCC)
int isTR069Wan(struct net_device *dev){
#if (defined(TCSUPPORT_XPON_MAPPING) || defined(TCSUPPORT_PON_VLAN) || defined(TCSUPPORT_XPON_IGMP)) && defined(TCSUPPORT_PON_IP_HOST)
	if(dev == NULL || isBridgeWan(dev))
		return 0;

#if defined(TCSUPPORT_CT)
	if(dev->name[0] == 'n' && dev->name[3] == tr069_wanIf[3] && dev->name[5] == tr069_wanIf[5])
#else
	if(dev->name[0] == 'n' && dev->name[3] == tr069_wanIf[3])
#endif
		return 1;
#endif
	return 0;
}
#endif

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
/****************************************************************************
**function name
	samba_shortcut_read_proc
**description:
	samba_shortcut_read_proc
**return 
**parameter:
****************************************************************************/
static int samba_shortcut_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;

	len += sprintf(page+len,"SmbdTxSpeedOn: %d \n", SmbdTxSpeedOn);
	len += sprintf(page+len,"SmbdTxSpeedCnt0: %d \n", SmbdTxSpeedCnt0);
	len += sprintf(page+len,"SmbdTxSpeedCnt1: %d \n", SmbdTxSpeedCnt1);

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}
/****************************************************************************
**function name
	samba_shortcut_write_proc
**description:
	samba_shortcut_write_proc
**return 
**parameter:
****************************************************************************/
static int samba_shortcut_write_proc(struct file *file
, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] = {0};
	int enable_tmp = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	if (strstr(val_string, "reset")) {
		SmbdTxSpeedCnt0 = 0;
		SmbdTxSpeedCnt1 = 0;
		return count;
	}

	sscanf(val_string, "%d", &enable_tmp);
	if((enable_tmp == 0) || (enable_tmp == 1))
		SmbdTxSpeedOn = enable_tmp;
	else
		printk("\nerr value\n");
	
	return count;
}
/****************************************************************************
**function name
	ecnt_samba_shortcut_proc_init
**description:
	ecnt_samba_shortcut_proc_init
**return 
**parameter:
****************************************************************************/
void ecnt_samba_shortcut_proc_init(void)
{
	struct proc_dir_entry *samba_shortcut_proc = NULL;

	samba_shortcut_proc = create_proc_entry(SAMBA_SHORTCUT_PATH, 0, NULL);
	samba_shortcut_proc->read_proc = samba_shortcut_read_proc;
	samba_shortcut_proc->write_proc = samba_shortcut_write_proc;	

	return;
}
/****************************************************************************
**function name
	ecnt_samba_shortcut_proc_deinit
**description:
	ecnt_samba_shortcut_proc_deinit
**return 
**parameter:
****************************************************************************/
void ecnt_samba_shortcut_proc_deinit(void)
{
	remove_proc_entry(SAMBA_SHORTCUT_PATH, NULL);
}
#endif

char ecnt_shortcut_apps[APP_SHORTCUT_MAX_NUM][32] = {0};

/****************************************************************************
**function name
	ecnt_shortcut_app_list_check
**description:
	ecnt_shortcut_app_list_check
**return
**parameter:
****************************************************************************/

int ecnt_shortcut_app_list_check(char * name)
{
    int i;

    for (i = 0; i < APP_SHORTCUT_MAX_NUM; i++)
    {
        if (!strcmp(ecnt_shortcut_apps[i], name))
            break;
    }

    if (i < APP_SHORTCUT_MAX_NUM)
    {
        return 1;
    }

    return 0;
}

/****************************************************************************
**function name
	ecnt_shortcut_app_list_add
**description:
	ecnt_shortcut_app_list_add
**return
**parameter:
****************************************************************************/

int ecnt_shortcut_app_list_add(char * name)
{
    int i, pos = APP_SHORTCUT_MAX_NUM;

    for (i = 0; i < APP_SHORTCUT_MAX_NUM; i++)
    {
        if (!strcmp(ecnt_shortcut_apps[i], name))
        {
            return 1;
        }
		if ((ecnt_shortcut_apps[i][0] == '\0') && (pos == APP_SHORTCUT_MAX_NUM))
        {
            pos = i;
			break;
        }
    }

    if (pos < APP_SHORTCUT_MAX_NUM)
    {
        strncpy(ecnt_shortcut_apps[pos], name, 31);
        ecnt_shortcut_apps[pos][31] = '\0';
        return 1;
    }

    return 0;
}

/****************************************************************************
**function name
	ecnt_shortcut_app_list_del
**description:
	ecnt_shortcut_app_list_del
**return
**parameter:
****************************************************************************/

int ecnt_shortcut_app_list_del(char * name)
{
    int i;

    for (i = 0; i < APP_SHORTCUT_MAX_NUM; i++)
    {
        if (!strcmp(ecnt_shortcut_apps[i], name))
            break;
    }

    if (i < APP_SHORTCUT_MAX_NUM)
    {
        ecnt_shortcut_apps[i][0] = '\0';
        return 1;
    }

    return 0;
}

/****************************************************************************
**function name
	ecnt_app_shortcut_read_proc
**description:
	ecnt_app_shortcut_read_proc
**return
**parameter:
****************************************************************************/

static int ecnt_app_shortcut_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0, i;

	len += sprintf(page+len,"ShortCutApps: ");
    for (i = 0; i < APP_SHORTCUT_MAX_NUM; i++)
    {
        len += sprintf(page+len,"%s ", ecnt_shortcut_apps[i]);
    }
	len += sprintf(page+len,"\n");
	len += sprintf(page+len,"ShortCutTxSpeedCnt0: %d \n", ShortCutTxSpeedCnt0);
	len += sprintf(page+len,"ShortCutTxSpeedCnt1: %d \n", ShortCutTxSpeedCnt1);

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}

/****************************************************************************
**function name
	ecnt_app_shortcut_write_proc
**description:
	ecnt_app_shortcut_write_proc
**return
**parameter:
****************************************************************************/

static int ecnt_app_shortcut_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32] = {0};
	char name[32] = {0};
	int flag = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	if (strstr(val_string, "reset"))
    {
		ShortCutTxSpeedCnt0 = 0;
		ShortCutTxSpeedCnt1 = 0;
		return count;
	}

	sscanf(val_string, "%d %s", &flag, name);
    if (flag)
    {
        ecnt_shortcut_app_list_add(name);
    }
    else
    {
        ecnt_shortcut_app_list_del(name);
    }
	return count;
}

/****************************************************************************
**function name
	ecnt_app_shortcut_proc_init
**description:
	ecnt_app_shortcut_proc_init
**return
**parameter:
****************************************************************************/

void ecnt_app_shortcut_proc_init(void)
{
	struct proc_dir_entry *app_shortcut_proc = NULL;

	app_shortcut_proc = create_proc_entry(APP_SHORTCUT_PATH, 0, NULL);
	app_shortcut_proc->read_proc = ecnt_app_shortcut_read_proc;
	app_shortcut_proc->write_proc = ecnt_app_shortcut_write_proc;

	return;
}

/****************************************************************************
**function name
	ecnt_app_shortcut_proc_deinit
**description:
	ecnt_app_shortcut_proc_deinit
**return
**parameter:
****************************************************************************/

void ecnt_app_shortcut_proc_deinit(void)
{
	remove_proc_entry(APP_SHORTCUT_PATH, NULL);
}

#if defined(TCSUPPORT_CT_JOYME4)
/****************************************************************************
**function name
	capable_user_root_switch_read_proc
**description:
	capable_user_root_switch_read_proc
**return
**parameter:
****************************************************************************/
static int capable_user_root_switch_read_proc(char *buf, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;

	len += sprintf(buf+len, "%d\n", g_capable_user_root_switch);

	len -= off;
	*start = buf + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}
/****************************************************************************
**function name
	capable_user_root_switch_write_proc
**description:
	capable_user_root_switch_write_proc
**return
**parameter:
****************************************************************************/
static int capable_user_root_switch_write_proc(struct file *file
, const char *buffer, unsigned long count, void *data)
{
	int len;
	char get_buf[32] = {0};

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 31)
		len = 31;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';
	
	sscanf(get_buf, "%d", &g_capable_user_root_switch);
	
	return len;

}
/****************************************************************************
**function name
	ecnt_capable_user_root_switch_proc_init
**description:
	ecnt_capable_user_root_switch_proc_init
**return
**parameter:
****************************************************************************/
void ecnt_capable_user_root_switch_proc_init(void)
{
	struct proc_dir_entry *capable_user_root_switch_proc = NULL;

	capable_user_root_switch_proc = create_proc_entry(CAPABLE_USER_ROOT_SWITCH_PATH, 0, NULL);
	capable_user_root_switch_proc->read_proc = capable_user_root_switch_read_proc;
	capable_user_root_switch_proc->write_proc = capable_user_root_switch_write_proc;

	return;
}
/****************************************************************************
**function name
	ecnt_capable_user_root_switch_proc_deinit
**description:
	ecnt_capable_user_root_switch_proc_deinit
**return
**parameter:
****************************************************************************/
void ecnt_capable_user_root_switch_proc_deinit(void)
{
	remove_proc_entry(CAPABLE_USER_ROOT_SWITCH_PATH, NULL);
}
#endif

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
/****************************************************************************
**function name
	wifi_multi_chip_read_proc
**description:
	wifi_multi_chip_read_proc
**return
**parameter:
****************************************************************************/
static int wifi_multi_chip_read_proc(char *page, char **start, off_t offset,
			int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "%x %x\n", wifi_type_bit,wifi_type_bit_5g);

	len = out - page;
	len -= offset;
	if (len < count) {
		*eof = 1;
		if (len <= 0)
			return 0;
	} else
		len = count;

	*start = page + offset;
	return len;
}

/****************************************************************************
**function name
	wifi_multi_chip_write_proc
**description:
	wifi__multi_chip_write_proc
**return
**parameter:
****************************************************************************/
static int wifi_multi_chip_write_proc(struct file *file
, const char *buffer, unsigned long count, void *data)
{
	int len;
	char get_buf[32] = {0};
	int i;
	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 31)
		len = 31;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';
	
	sscanf(get_buf, "%x %x", &wifi_type_bit,&wifi_type_bit_5g);

	memset(WLAN_2G_CHIPYPTE,0,WLAN_2G_MAX_ID);
	memset(WLAN_5G_CHIPYPTE,0,WLAN_5G_MAX_ID);
	for(i = WLAN_2G_NOCHIP_ID;i <= WLAN_2G_MAX_ID;i++)
	{
		if(wifi_type_bit & BIT(i))
		{
			WLAN_2G_CHIPYPTE[i] = 1;
			break;
		}		
	}
		
	if(i == WLAN_2G_MAX_ID)
		WLAN_2G_CHIPYPTE[WLAN_2G_NOCHIP_ID] = 1;
	
	for(i = WLAN_5G_NOCHIP_ID;i <= WLAN_5G_MAX_ID;i++)
	{
		if(wifi_type_bit_5g & BIT(i))
		{
			WLAN_5G_CHIPYPTE[i] = 1;
			break;
		}		
	}

	if(i == WLAN_5G_MAX_ID)
		WLAN_5G_CHIPYPTE[WLAN_5G_NOCHIP_ID] = 1;
#if 0
	printk("\r\nwifi__multi_chip_write_proc");
	printk("\r\n---WLAN_2G_7592 = %d------------",WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID]);
	printk("\r\n---WLAN_2G_7615N = %d------------",WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID]);
	printk("\r\n---WLAN_2G_7615D = %d------------",WLAN_2G_CHIPYPTE[WLAN_2G_7615D_ID]);
	printk("\r\n---WLAN_2G_7915N = %d------------",WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID]);
	printk("\r\n---WLAN_2G_7915D = %d------------",WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID]);
	printk("\r\n---WLAN_2G_7916D = %d------------",WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID]);

	printk("\r\n---WLAN_5G_7613E = %d------------",WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID]);
	printk("\r\n---WLAN_5G_7615E = %d------------",WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID]);
	printk("\r\n---WLAN_5G_7615D = %d------------",WLAN_5G_CHIPYPTE[WLAN_5G_7615D_ID]);
	printk("\r\n---WLAN_5G_7915E = %d------------",WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID]);
	printk("\r\n---WLAN_5G_7915D = %d------------",WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID]);
	printk("\r\n---WLAN_5G_7916D = %d------------",WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID]);
#endif
	return len;

}

/****************************************************************************
**function name
	wifi_multi_chip_5g_read_proc
**description:
	wifi_multi_chip_5g_read_proc
**return
**parameter:
****************************************************************************/
static int wifi_multi_chip_5g_read_proc(char *buf, char **start, off_t off,
		int count, int *eof, void *data)
{
	int len = 0;
	int type = WLAN_5G_NOCHIP_ID;
	int i = 0;
	for(i = WLAN_5G_7613E_ID;i <= WLAN_5G_MAX_ID;i++)
	{
		if(WLAN_5G_CHIPYPTE[i])
		{
			type = i;
			break;
		}		
	}
	
	len += sprintf(buf+len, "%d\n", type);
	
	len -= off;
	*start = buf + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}

/****************************************************************************
**function name
	wifi_multi_chip_2g_read_proc
**description:
	wifi_multi_chip_2g_read_proc
**return
**parameter:
****************************************************************************/
static int wifi_multi_chip_2g_read_proc(char *buf, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;
	int type = WLAN_2G_NOCHIP_ID;
	int i = 0;
	for(i = WLAN_2G_7592_ID;i <= WLAN_2G_MAX_ID;i++)
	{
		if(WLAN_2G_CHIPYPTE[i])
		{
			type = i;
			break;
		}		
	}
	len += sprintf(buf+len, "%d\n", type);

	len -= off;
	*start = buf + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}

/****************************************************************************
**function name
	ecnt_wifi_multi_chip_proc_int
**description:
	ecnt_wifi_multi_chip_proc_int
**return 
**parameter:
****************************************************************************/
void ecnt_wifi_multi_chip_proc_int(void)
{
	struct proc_dir_entry *multi_chip_proc = NULL;
	
	multi_chip_proc = create_proc_entry(WIFI_CHIP_PATH, 0, NULL);
	multi_chip_proc->read_proc = wifi_multi_chip_read_proc;
	multi_chip_proc->write_proc = wifi_multi_chip_write_proc;	

	//this proc used for .sh
	create_proc_read_entry(WIFI_CHIP_PATH_BAND0, 0, NULL, wifi_multi_chip_2g_read_proc, NULL);
	create_proc_read_entry(WIFI_CHIP_PATH_BAND1, 0, NULL, wifi_multi_chip_5g_read_proc, NULL);
	
	return;
}

/****************************************************************************
**function name
	ecnt_wifi_multi_chip_proc_deinit
**description:
	ecnt_wifi_multi_chip_proc_deinit
**return 
**parameter:
****************************************************************************/
void ecnt_wifi_multi_chip_proc_deinit(void)
{
	remove_proc_entry(WIFI_CHIP_PATH, NULL);
	remove_proc_entry(WIFI_CHIP_PATH_BAND0, NULL);
	remove_proc_entry(WIFI_CHIP_PATH_BAND1, NULL);
}
#endif

#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD

/****************************************************************************
**function name
	npu_band0_on_cpu_read_proc
**description:
	npu_band0_on_cpu_read_proc
**return
**parameter:
****************************************************************************/
static int npu_band0_on_cpu_read_proc(char *page, char **start, off_t offset,
			int count, int *eof, void *data)
{
	char *out = page;
	int len;

	out += sprintf(out, "NPU Band0 on CPU is %s\n", isNpuBand0OnCpu?"on":"off");

	len = out - page;
	len -= offset;
	if (len < count) {
		*eof = 1;
		if (len <= 0)
			return 0;
	} else
		len = count;

	*start = page + offset;
	return len;

}
/****************************************************************************
**function name
	npu_band0_on_cpu_write_proc
**description:
	npu_band0_on_cpu_write_proc
**return
**parameter:
****************************************************************************/
static int npu_band0_on_cpu_write_proc(struct file *file
, const char *buffer, unsigned long count, void *data)
{
	int len;
	char get_buf[32] = {0};
	int i;
	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 31)
		len = 31;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';
	
	sscanf(get_buf, "%d", &isNpuBand0OnCpu);
	printk("set NPU wifi offload Band0 on CPU: %s\n", (isNpuBand0OnCpu)?"on":"off");
	
	return len;

}
/****************************************************************************
**function name
	ecnt_npu_band0_on_cpu_proc_int
**description:
	ecnt_npu_band0_on_cpu_proc_int
**return 
**parameter:
****************************************************************************/
void ecnt_npu_band0_on_cpu_proc_int(void)
{
	struct proc_dir_entry *band0_on_cpu_proc = NULL;
	
	band0_on_cpu_proc = create_proc_entry(BAND0_ON_CPU_PATH, 0, NULL);
	band0_on_cpu_proc->read_proc = npu_band0_on_cpu_read_proc;
	band0_on_cpu_proc->write_proc = npu_band0_on_cpu_write_proc;	
	
	return;
}

/****************************************************************************
**function name
	ecnt_npu_band0_on_cpu_proc_deinit
**description:
	ecnt_npu_band0_on_cpu_proc_deinit
**return 
**parameter:
****************************************************************************/
void ecnt_npu_band0_on_cpu_proc_deinit(void)
{
	remove_proc_entry(BAND0_ON_CPU_PATH, NULL);
}
#endif
/****************************************************************************
**function name
	dbg_msg_read_proc
**description:
	dbg_msg_read_proc
**return 
**parameter:
****************************************************************************/
static int dbg_msg_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	return 0;
}
/****************************************************************************
**function name
	dbg_msg_write_proc
**description:
	dbg_msg_write_proc
**return 
**parameter:
****************************************************************************/
static int dbg_msg_write_proc(struct file *file
, const char *buffer, unsigned long count, void *data)
{
	char val_string[1024] = {0};

	if (count > sizeof(val_string) - 1) 
	{
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count)) 
	{
		return -EFAULT;
	}

	val_string[count] = '\0';
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,10,17)	
	printk(KERN_CONT "%s", val_string);
#else
	printk(val_string);
#endif


	return count;
}
/****************************************************************************
**function name
	ecnt_dbg_msg_proc_init
**description:
	ecnt_dbg_msg_proc_init
**return 
**parameter:
****************************************************************************/
void ecnt_dbg_msg_proc_init(void)
{
	struct proc_dir_entry *dbg_msg_proc = NULL;

	dbg_msg_proc = create_proc_entry(DBG_MSG_PATH, 0, NULL);
	dbg_msg_proc->read_proc = dbg_msg_read_proc;
	dbg_msg_proc->write_proc = dbg_msg_write_proc;	

	return;
}
/****************************************************************************
**function name
	ecnt_dbg_msg_proc_deinit
**description:
	ecnt_dbg_msg_proc_deinit
**return 
**parameter:
****************************************************************************/
void ecnt_dbg_msg_proc_deinit(void)
{
	remove_proc_entry(DBG_MSG_PATH, NULL);
}

#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) 
#define AEWAN_IDX_PATH "tc3162/aewan_idx"
int aewan_idx = -1;
int get_aewan_idx(void)
{
	return aewan_idx;
}
EXPORT_SYMBOL(aewan_idx);
EXPORT_SYMBOL(get_aewan_idx);

void ecnt_aewan_idx_proc_deinit(void)
{
	remove_proc_entry(AEWAN_IDX_PATH, NULL);
}

static int aewan_idx_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	 int len = 0;
	 if (aewan_idx == -1)
		 len = sprintf(buf,"Aewan used as Wan");
	 else
		 len = sprintf(buf,"Aewan used as Lan, portmask idx = %d", aewan_idx);

	 if (len < off + count)
		 *eof = 1;
 
	 len -= off;
	 *start = buf + off;
	 if(len > count)
		 len = count;
	  if(len < 0)
		 len = 0;

	return len;
}
 
static int aewan_idx_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char temp[16] = {0};
	
	if (count > 2)
		return -EFAULT;

	if (copy_from_user(temp, buffer, count))
		return -EFAULT;

	sscanf(temp, "%d", &aewan_idx);
		
	return count;
}

void ecnt_aewan_idx_proc_init(void)
{
		struct proc_dir_entry *aewan_idx_proc = NULL;
	
		aewan_idx_proc = create_proc_entry(AEWAN_IDX_PATH, 0, NULL);
		aewan_idx_proc->read_proc = aewan_idx_read_proc;
		aewan_idx_proc->write_proc = aewan_idx_write_proc;
	
		return;
}
#else
int aewan_idx = -1;
int get_aewan_idx(void)
{
	return -1;
}
EXPORT_SYMBOL(get_aewan_idx);

#endif
void ecnt_netdev_init_hook(void)
{
	if (remove_proc_flag == 1)
		return;
#if defined(TCSUPPORT_CT_VOIP_QOS)
	ecnt_voip_qos_proc_init();
#endif

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
	ecnt_samba_shortcut_proc_init();
#endif
	ecnt_app_shortcut_proc_init();

#if defined(TCSUPPORT_CT_JOYME4)
	ecnt_capable_user_root_switch_proc_init();
#endif

#if (defined(TCSUPPORT_XPON_MAPPING) || defined(TCSUPPORT_PON_VLAN) || defined(TCSUPPORT_XPON_IGMP)) && defined(TCSUPPORT_PON_IP_HOST)
	ecnt_wanIf_proc_init();
#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	ecnt_wifi_multi_chip_proc_int();
#endif
#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
	ecnt_npu_band0_on_cpu_proc_int();
#endif
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) 
	ecnt_aewan_idx_proc_init();
#endif
	ecnt_dbg_msg_proc_init();

	ecnt_wlanIf_name_proc_init();

	remove_proc_flag = 1;
}


void ecnt_netdev_dest_hook(void)
{
	/* only remove one time */
	if (remove_proc_flag == 0)
		return;
#if defined(TCSUPPORT_CT_VOIP_QOS)
	ecnt_voip_qos_proc_dest();
#endif

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
	ecnt_samba_shortcut_proc_deinit();
#endif
	ecnt_app_shortcut_proc_deinit();

#if defined(TCSUPPORT_CT_JOYME4)
	ecnt_capable_user_root_switch_proc_deinit();
#endif

#if (defined(TCSUPPORT_XPON_MAPPING) || defined(TCSUPPORT_PON_VLAN) || defined(TCSUPPORT_XPON_IGMP)) && defined(TCSUPPORT_PON_IP_HOST)
	ecnt_wanIf_proc_deinit();
#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	ecnt_wifi_multi_chip_proc_deinit();
#endif
#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
	ecnt_npu_band0_on_cpu_proc_deinit();
#endif
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) 
	ecnt_aewan_idx_proc_deinit();
#endif
	ecnt_dbg_msg_proc_deinit();

	ecnt_wlanIf_name_proc_deinit();

	remove_proc_flag = 0;
}


int localInShortCut(struct sk_buff *skb, int protocol)
{
	struct iphdr *iph = NULL;
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
	bool refcounted;
#endif

    skb->pkt_type = PACKET_HOST;
	skb->ip_summed = CHECKSUM_UNNECESSARY ;

	skb_reset_network_header(skb);
	skb->mac_len = skb->network_header - skb->mac_header;
	iph = ip_hdr(skb);
	pskb_trim_rcsum(skb, ntohs(iph->tot_len));
	
	if ( !skb->skb_iif && skb->dev)
		skb->skb_iif = skb->dev->ifindex;

	/* remove ip header */
	skb_pull(skb, ip_hdrlen(skb));
	skb_reset_transport_header(skb);

	if (NULL == skb_dst(skb) && skb->dev)
	{
		ip_route_input_noref(skb, iph->daddr, iph->saddr,
			iph->tos, skb->dev);
	}

    if (protocol)
    {
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
        if (!skb_steal_sock(skb, &refcounted) && !skb_rtable(skb))
#else
        if (!skb_steal_sock(skb) && !skb_rtable(skb))
#endif
        {
            kfree_skb(skb);
            return 0;
        }

        udp_rcv(skb);
    }
    else
    {   
        tcp_v4_rcv(skb);
    }

	return 0;
}
EXPORT_SYMBOL(localInShortCut);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
void smp_call_function_single_ext(int cpuid, struct call_single_data *data)
#else
void smp_call_function_single_ext(int cpuid, call_single_data_t *data)
#endif
{
    smp_call_function_single_async(cpuid, data);

    return;
}
EXPORT_SYMBOL(smp_call_function_single_ext);

#if defined(TCSUPPORT_CT_JOYME2)
#define PANIC_BUF_LEN	(12*1024)
int panic_log_on = 0;
int panic_len = 0;
char panic_buf[PANIC_BUF_LEN];

int get_panic_log_flag(void)
{
	return panic_log_on;
}

int set_panic_log_flag(int panic_flag)
{
	panic_log_on = panic_flag;
	return 0;
}

int get_panic_log_buffer_len(void)
{
	return panic_len;
}

int set_panic_log_buffer_len(int len)
{
	panic_len = len;
	return 0;
}

int config_reboot_info_exception(void)
{
	struct file *fp_info, *fp_src, *fp_tz;
	int ret = -1, len = 0;
	mm_segment_t fs;
	struct timeval nowtime;
    struct rtc_time tm;
	char format_time[20] = {0};
	char buf[256] = {0};
	char *p = NULL;
	char tmp1[8] = {0}, tmp2[8] = {0};
	int tz_hour = 0, tz_min = 0, minus = 1;

	const char* json_format = "{\"Time\":\"%s\","
							  "\"Source\":\"%s\"}";

	fs = get_fs();
	set_fs(KERNEL_DS);
	
	/* get local TZ */
	fp_tz = filp_open("/etc/TZ", O_RDONLY, 0);
	if (IS_ERR(fp_tz)) {
		printk("open TZ fail\n");
		goto out;
	}
	else if ( !ecnt_kernel_fs_read_check(fp_tz) )
	{
		filp_close(fp_tz, NULL);
		printk("cann't read TZ file\n");
		goto out;
	}
	else {
		ret = ecnt_kernel_fs_read(fp_tz, buf, sizeof(buf) - 1, &fp_tz->f_pos);
		printk("TZ:%s\n", buf);
		if ( (p = strstr(buf, "+")) || (p = strstr(buf, "-")) ) {
			memset(tmp1, 0, sizeof(tmp1));
			strncpy(tmp1, p, 3);
			if ('-' == tmp1[0])
				minus = -1;
			memset(tmp2, 0, sizeof(tmp2));
			strncpy(tmp2, &tmp1[1], 2);
			sscanf(tmp2, "%d", &tz_hour);
		}
		else {
			tz_hour = 8;
			minus = -1;
		}
		filp_close(fp_tz, NULL);
	}
	
	fp_info = filp_open("/opt/upt/apps/info/reboot_info", O_CREAT | O_WRONLY, 0666);
	if (IS_ERR(fp_info)) {
		printk("failed open file\n");
		goto out;
	}
	if( !ecnt_kernel_fs_write_check(fp_info) )
	{
		printk("cann't write\n");
		filp_close(fp_info, NULL);
		goto out;
	}

	/* get nowtime */
    #if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
	do_gettimeofday(&nowtime);
    #endif
    /* parse to local time with current TZ */
    nowtime.tv_sec -= minus * tz_hour * 60 * 60;
    /* parse to tm */
    rtc_time_to_tm(nowtime.tv_sec,&tm);
	snprintf(format_time
			, sizeof(format_time)
			, "%04d-%02d-%02d %02d:%02d:%02d"
			, tm.tm_year + 1900
			, tm.tm_mon + 1
			, tm.tm_mday
			, tm.tm_hour
			, tm.tm_min
			, tm.tm_sec);

	len = snprintf(buf, sizeof(buf), json_format, format_time, "Exception");

	ret = ecnt_kernel_fs_write(fp_info, buf, len, &fp_info->f_pos);
	filp_close(fp_info, NULL);

	/* create reboot_source file */
	fp_src = filp_open("/opt/upt/apps/info/reboot_source", O_CREAT | O_WRONLY, 0666);
	if (IS_ERR(fp_src)) {
		printk("failed open file\n");
		goto out;
	}
	
	if( !ecnt_kernel_fs_write_check(fp_src) )
	{
		printk("cann't write\n");
		filp_close(fp_src, NULL);
		goto out;
	}
	ret = ecnt_kernel_fs_write(fp_src, buf, len, &fp_src->f_pos);
	filp_close(fp_src, NULL);

out:
	set_fs(fs);
	return 0;
}

extern int nand_flash_avalable_size;
#ifndef TCSUPPORT_CPU_ARMV8
extern int iswatchDogReset;
#endif
#ifdef TCSUPPORT_WLAN_AUTO_DEBUG_TOOL
extern int iswatchDogReset;
#define MTK_WADT_DRV_FILE			"/var/wadt_log/wifi.txt"  
#define MTK_WADT_DRV_PERIDOIC_FILE  "/var/wadt_log/periodic.txt"
#define MTK_WADT_LAST_TAR 			"/var/wadt_log/wadt_last100_log.tar.gz"

#define MTK_WADT_CRASH_PERIDOIC_FILE 	"/usr/config/WLan/MAP/wadt_crash_periodic.txt"
#define MTK_WADT_CRASH_WIFI_FILE 		"/usr/config/WLan/MAP/wadt_crash_wifi.txt"
#define MTK_WADT_CRASH_TAR 				"/usr/config/WLan/MAP/wadt_crash_log.tar.gz"
#define WADT_BUF_LEN		(20*1024)
#define WADT_TAR_BUF_LEN	(40*1024)
char wadt_periodic_buf[WADT_BUF_LEN]={0};
char wadt_wifi_buf[WADT_BUF_LEN]={0};
char wadt_tar_buf[WADT_TAR_BUF_LEN]={0};
int read_file_bytesto_buf(struct file *fp, char* buf, int size){
	int ret = 0;
	ret = fp->f_op->read(fp, buf, size - 1, &fp->f_pos);
	printk("read_file_bytesto_buf:ret=%d\n",ret);
	if(ret < 0)
		return -1;

	return 0;
}

int write_wadt_log_to_buf(char * file_path, char * buf, int write_from_tail,int buf_len){
	struct file *fp = NULL;
	int tail=buf_len*-1 , ret = 0;
	fp = filp_open(file_path, O_RDONLY, 0666);
	if (IS_ERR(fp)) {
		printk("open periodic fail\n");
		return -1;
	}else if (!fp->f_op || !fp->f_op->read) {
		filp_close(fp, NULL);
		printk("cann't read file\n");
		return -1;
	}

	if(write_from_tail==1){
		if(!fp->f_op->llseek){
			filp_close(fp, NULL);
			printk("cann't read file\n");
			return -1;
		}else{
			ret = fp->f_op->llseek(fp, tail, SEEK_END);
		}
	}

	if(ret<0){
		filp_close(fp, NULL);
		printk("cann't llseek file\n");
		return -1;
	}else{
		read_file_bytesto_buf(fp,buf,buf_len);
		filp_close(fp, NULL);
	}
	return 0;
}

int write_buf_to_flash(char* file_path, char * buf, int len){
	struct file *fp = NULL;
	mm_segment_t fs;
	int ret =0;

	fp = filp_open(file_path, O_CREAT | O_WRONLY, 0666);
	if (NULL == fp) {
		printk("failed open file %s\n",file_path);
		return -1;
	}
	if (IS_ERR(fp)) {
		printk("failed open file %s\n",file_path);
		return -1;
	}
	if (!fp->f_op || !fp->f_op->write) {
		printk("cann't write\n");
		goto out;
	}
	
	fs = get_fs();
	set_fs(KERNEL_DS);
	ret = fp->f_op->write(fp, buf, len, &fp->f_pos);
	set_fs(fs);
out:
	filp_close(fp, NULL);
	return ret;
}

int get_wadt_log_file_size(char * file_path){
	struct file *fp = NULL;
	struct kstat * stat;
	int err=0,size=0;
	mm_segment_t fs;

	stat = (struct kstat *) kmalloc(sizeof(struct kstat),GFP_KERNEL);
	if(NULL == stat){
		return -1;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);
	err = vfs_stat(file_path,stat);
	size = stat->size;
	set_fs(fs);
	kfree(stat);
	return size;
}

void collect_wadt_log(char * file, char* buf , char * save_log_path,int buf_len){
	int log_len = 0, write_from_tail = 0;
	int ret = 0;
	log_len = get_wadt_log_file_size(file);
 	if( log_len >= buf_len){
 		log_len = buf_len -1;
 		write_from_tail = 1;
 	}
 	if(log_len<=0){
 		printk("%s is empty\n",file);
 		return;
 	}

 	ret = write_wadt_log_to_buf(file, buf, write_from_tail,buf_len);
 	if(ret<0){
 		return;
 	}

 	ret = write_buf_to_flash(save_log_path, buf, log_len);
 	if(ret<0){
 		return;
 	}
 	return ;
 	
}
#endif
int panic_write(void){	
	struct file *fp = NULL;
	int ret = -1, len = 0;
	mm_segment_t fs;
	int info_size = 0;
	unsigned int defaultromfile_flag_addr = 0;
#ifdef TCSUPPORT_WLAN_AUTO_DEBUG_TOOL	
	/*collest WADT wifi*/
 	collect_wadt_log(MTK_WADT_DRV_FILE, wadt_wifi_buf, MTK_WADT_CRASH_WIFI_FILE,WADT_BUF_LEN);
 	/*collest WADT periodic*/
 	collect_wadt_log(MTK_WADT_DRV_PERIDOIC_FILE, wadt_periodic_buf, MTK_WADT_CRASH_PERIDOIC_FILE,WADT_BUF_LEN);
 	/*collest WADT tar*/
 	collect_wadt_log(MTK_WADT_LAST_TAR,wadt_tar_buf,MTK_WADT_CRASH_TAR,WADT_TAR_BUF_LEN);
 	iswatchDogReset = 1;
#endif
	if ((len = get_panic_log_buffer_len()) <= 0)
		goto out;

/*------------------------------------------------------------------------
	Use the defaultromfile sector to save crash_info, because the reservearea is 
	last sector and the reservearea's size is 0x1c0000, so the defaultromfile_flag_addr
	= nand_flash_avalable_size - 0x1c0000 + 0x40000
	|sector 	name				cover area				note
	|1			backupromfile			0~0x3ffff					256k
	|2			defaultromfile			0x40000~0x7ffff 		256k
	#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*7
	#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
------------------------------------------------------------------------*/
	defaultromfile_flag_addr = nand_flash_avalable_size - RESERVEAREA_TOTAL_SIZE + DEFAULTROMFILE_RA_OFFSET;
	/*erase defaultromfile*/
	nandflash_erase(defaultromfile_flag_addr, 0x10000);
	/*write crash_info into defaultromfile sector*/
	nandflash_write(defaultromfile_flag_addr, len + 4, &info_size, panic_buf);

#ifndef TCSUPPORT_CPU_ARMV8
	iswatchDogReset = 1;
#endif
	fp = filp_open("/opt/upt/apps/info/crash_info", O_CREAT | O_WRONLY, 0666);
	if (IS_ERR(fp)) {
		printk("failed open file\n");
		return ret;
	}
	
	if( !ecnt_kernel_fs_write_check(fp) )
	{
		printk("cann't write\n");
		goto out;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);
	ret = ecnt_kernel_fs_write(fp, panic_buf, len + 4, &fp->f_pos);
	set_fs(fs);

out:
	if (NULL != fp)
	{	
		filp_close(fp, NULL);
	}
	config_reboot_info_exception();
    #if defined(TCSUPPORT_CPU_ARMV8_64)
    ksys_sync();
    #else
	sys_sync();
    #endif
	return ret;
}

int panic_write2(const char *buf)
{
	struct pt_regs *regs;
	int ret = 0, panic_flag = 0;

	panic_flag = get_panic_log_flag();
	if (panic_flag) 
		return 0;
	set_panic_log_flag(1);
	printk("%s\n", buf);
 	regs = get_irq_regs();
	if (regs) 
		show_regs(regs);
	dump_stack();
	ret = panic_write();

	return ret;
}

void set_panic_log_buffer(va_list *args, const char* fmt){
	int panic_flag = 0, len = 0;

	panic_flag = get_panic_log_flag();
	len = get_panic_log_buffer_len();
		
	if (panic_flag && len < PANIC_BUF_LEN) {
		if (len == 0)
		{
			panic_buf[0] = 'e';
			panic_buf[1] = 'c';
			panic_buf[2] = 'o';
			panic_buf[3] = '\n';
			len += vsnprintf(&panic_buf[len + 4], PANIC_BUF_LEN - 4 - len, fmt, *args);
		}
		else
			len += vsnprintf(&panic_buf[len], PANIC_BUF_LEN - 4 - len, fmt, *args);
	}
	set_panic_log_buffer_len(len);
}
#endif

int hsgmii_lan = 0;
EXPORT_SYMBOL(hsgmii_lan);

int itf_start_idx = 1;
char *lan_itf  = "eth0";
char *sep_itf  = "eth0.";
char *ae_wan_itf  = "ae_wan";
EXPORT_SYMBOL(itf_start_idx);
EXPORT_SYMBOL(lan_itf);
EXPORT_SYMBOL(sep_itf);
EXPORT_SYMBOL(ae_wan_itf);

int isLANInterface(struct net_device *dev)
{
#if defined(TCSUPPORT_SEPERATED_ETH_ITF)
	return ((dev != NULL) && (strncmp(dev->name, sep_itf, strlen(sep_itf)) == 0));
#else
	return ((dev != NULL) && (strncmp(dev->name, lan_itf, strlen(lan_itf)) == 0));
#endif
}
EXPORT_SYMBOL(isLANInterface);

int is24GWiFiInterface(struct net_device *dev)
{
	if(wlan_name_mode == 1)
		return wlanIf_name_check(0, dev->name);

#if defined(TCSUPPORT_WLAN_MT76_MAC80211) || defined (TCSUPPORT_WLAN_INODE)
    return ((dev != NULL) && (strlen(dev->name) == 7) && \
        (strncmp(dev->name,"wlan0", 5) == 0));
#elif defined(TCSUPPORT_SEPERATED_ETH_ITF)
    return ((dev != NULL) && \
        (dev->name[0] == 'r') && (dev->name[1] == 'a') && (dev->name[2] >= '0' && dev->name[2] <= '9'));
#else
    return ((dev != NULL) && (strlen(dev->name) >= 3) && \
        (dev->name[0] == 'r') && (dev->name[1] == 'a') && (dev->name[2] != 'i'));
#endif
}
EXPORT_SYMBOL(is24GWiFiInterface);

int is24GWDSInterface(struct net_device *dev)
{
    return ((dev != NULL) && (strlen(dev->name) == 4)&& (dev->name[0] == 'w') && \
        (dev->name[1] == 'd') && (dev->name[2] == 's'));
}
EXPORT_SYMBOL(is24GWDSInterface);

int is5GWDSInterface(struct net_device *dev)
{
    return ((dev != NULL) && (strlen(dev->name) == 5)&& (dev->name[0] == 'w') && \
        (dev->name[1] == 'd') && (dev->name[2] == 's') && (dev->name[3] == 'i'));
}
EXPORT_SYMBOL(is5GWDSInterface);

int is24GAPCLIInterface(struct net_device *dev)
{
	if(wlan_name_mode == 1)
		return wlanIf_name_check(3, dev->name);
      return ((dev != NULL) && (strlen(dev->name) == 6)&& (dev->name[0] == 'a') && \
        (dev->name[1] == 'p') && (dev->name[2] == 'c') && (dev->name[3] == 'l') && (dev->name[4] == 'i'));
}
EXPORT_SYMBOL(is24GAPCLIInterface);

int is5GAPCLIInterface(struct net_device *dev)
{
	if(wlan_name_mode == 1)
		return wlanIf_name_check(4, dev->name);
    return ((dev != NULL) && (strlen(dev->name) == 7)&& (dev->name[0] == 'a') && \
        (dev->name[1] == 'p') && (dev->name[2] == 'c') && (dev->name[3] == 'l') && \
        (dev->name[4] == 'i') && (dev->name[5] == 'i'));
}
EXPORT_SYMBOL(is5GAPCLIInterface);

int is6GAPCLIInterface(struct net_device *dev)
{
    return ((dev != NULL) && (strlen(dev->name) == 7)&& (dev->name[0] == 'a') && \
        (dev->name[1] == 'p') && (dev->name[2] == 'c') && (dev->name[3] == 'l') && \
        (dev->name[4] == 'i') && (dev->name[5] == 'x'));
}
EXPORT_SYMBOL(is6GAPCLIInterface);

int isAPCLIInterface(struct net_device *dev)
{
	if(wlan_name_mode == 1)
		return wlanIf_name_check(5, dev->name);
	return ((dev != NULL) && (dev->name[0] == 'a') && \
		(dev->name[1] == 'p') && (dev->name[2] == 'c') && (dev->name[3] == 'l') && (dev->name[4] == 'i'));
}
EXPORT_SYMBOL(isAPCLIInterface);

int is5GWiFiInterface(struct net_device *dev)
{
	if(wlan_name_mode == 1)
		return wlanIf_name_check(1, dev->name);

#if defined(TCSUPPORT_WLAN_MT76_MAC80211) || defined (TCSUPPORT_WLAN_INODE)
    return ((dev != NULL) && (strlen(dev->name) == 7) && \
        ((strncmp(dev->name,"wlan1", 5) == 0)||(strncmp(dev->name,"wlan2", 5) == 0)));
#elif defined(TCSUPPORT_SEPERATED_ETH_ITF)
    return ((dev != NULL) && (strlen(dev->name) == 4) && \
        (dev->name[0] == 'r') && (dev->name[1] == 'a') && (dev->name[2] == 'i'));
#else
    return ((dev != NULL) && (strlen(dev->name) >= 4) && \
        (dev->name[0] == 'r') && (dev->name[1] == 'a') && (dev->name[2] == 'i'));
#endif
}
EXPORT_SYMBOL(is5GWiFiInterface);

int is6GWiFiInterface(struct net_device *dev)
{
//#if defined(TCSUPPORT_WLAN_MT76_MAC80211) || defined (TCSUPPORT_WLAN_INODE)
//    return ((dev != NULL) && (strlen(dev->name) == 7) && \
//        ((strncmp(dev->name,"wlan1", 5) == 0)||(strncmp(dev->name,"wlan2", 5) == 0)));
//#else
    return ((dev != NULL) && (strlen(dev->name) == 4) && \
        (dev->name[0] == 'r') && (dev->name[1] == 'a') && (dev->name[2] == 'x'));
//#endif
}
EXPORT_SYMBOL(is6GWiFiInterface);

int isWiFiInterface(struct net_device *dev)
{
	if(wlan_name_mode == 1)
		return wlanIf_name_check(3, dev->name);
#if defined(TCSUPPORT_WLAN_MT76_MAC80211) || defined (TCSUPPORT_WLAN_INODE)
    return ((dev != NULL) && \
        (strncmp(dev->name,"wlan", 4) == 0));
#else
    return ((dev != NULL) && (dev->name[0] == 'r') && \
        (dev->name[1] == 'a'));
#endif
}
EXPORT_SYMBOL(isWiFiInterface);

#ifdef TCSUPPORT_WLAN_INODE
int is5GLowInterface(struct net_device *dev)
{
	return ((dev != NULL) && (strlen(dev->name) == 7) && \
        (strncmp(dev->name,"wlan1", 5) == 0));
}

int is5GHighInterface(struct net_device *dev)
{
	return ((dev != NULL) && (strlen(dev->name) == 7) && \
        (strncmp(dev->name,"wlan2", 5) == 0));	
}
EXPORT_SYMBOL(is5GLowInterface);
EXPORT_SYMBOL(is5GHighInterface);
#endif

int isUSBInterface(struct net_device *dev)
{
    return ((dev != NULL) && (dev->name[0] == 'u') && \
        (dev->name[1] == 's') && (dev->name[2] == 'b'));
}
EXPORT_SYMBOL(isUSBInterface);

int isXSIInterface(struct net_device *dev)
{
 	if((dev != NULL) && (dev->name[0] == 'e') && (dev->name[1] == 't') && \
		(dev->name[2] == 'h') && ((dev->name[3] == '1') || (dev->name[5] >= '5') && (dev->name[5] <= '8')))
	{		
		return 1;	
	}else{		
		return 0;	
	}
}
EXPORT_SYMBOL(isXSIInterface);

int isMAPInterface(struct net_device *dev)
{
    return ((dev != NULL) && (dev->name[0] == 'm') && \
        (dev->name[1] == 'a') && (dev->name[2] == 'p') && (dev->name[3] == '-'));
}
EXPORT_SYMBOL(isMAPInterface);

int isPONInterface(struct net_device *dev)
{
    return ((dev != NULL) && (dev->name[0] == 'p') && \
        (dev->name[1] == 'o') && (dev->name[2] == 'n'));
}
EXPORT_SYMBOL(isPONInterface);

int isWANInterface(struct net_device *dev)
{
    return ((dev != NULL) && (dev->name[0] == 'n') && \
        (dev->name[1] == 'a')&& (dev->name[2] == 's'));
}
EXPORT_SYMBOL(isWANInterface);

int isBridgeInterface(struct net_device *dev)
{
    return ((dev != NULL) && (dev->name[0] == 'b') && 
        (dev->name[1] == 'r'));
}
EXPORT_SYMBOL(isBridgeInterface);

int isPPPInterface(struct net_device *dev)
{
	return ((dev != NULL) && (dev->name[0] == 'p') && \
	(dev->name[1] == 'p') && (dev->name[2] == 'p'));
}
EXPORT_SYMBOL(isPPPInterface);

int isPPTPInterface(struct net_device *dev)
{
	return ((dev != NULL) && (dev->name[0] == 'p') && \
	(dev->name[1] == 'p') && (dev->name[2] == 't') && \
	(dev->name[3] == 'p') && (dev->name[4] == '-'));
}
EXPORT_SYMBOL(isPPTPInterface);

int isL2TPInterface(struct net_device *dev)
{
	return ((dev != NULL) && (dev->name[0] == 'l') && \
	(dev->name[1] == '2') && (dev->name[2] == 't') && \
	(dev->name[3] == 'p') && (dev->name[4] == '-'));
}
EXPORT_SYMBOL(isL2TPInterface);

int getSwitchLANIndex(struct net_device *dev)
{
	unsigned int eth_intf = 0;
	char format[10];

	if (memcmp(dev->name, sep_itf, strlen(sep_itf)) == 0){
		snprintf(format, sizeof(format), "%s%%d", sep_itf);
		sscanf(dev->name, format, &eth_intf);
		return eth_intf - itf_start_idx;
	}else{
		return eth_intf;
	}
}
EXPORT_SYMBOL(getSwitchLANIndex);

int getSwitchLANIndexByName(char *pname)
{
	unsigned int eth_intf = 0;
	char format[10];

	if (memcmp(pname, sep_itf, strlen(sep_itf)) == 0){
		snprintf(format, sizeof(format), "%s%%d", sep_itf);
		sscanf(pname, format, &eth_intf);
		return eth_intf - itf_start_idx;
	}else{
		return eth_intf;
	}
}
EXPORT_SYMBOL(getSwitchLANIndexByName);

int getLogicLANIndex(struct net_device *dev)
{
	unsigned int eth_intf = 0;
	char format[10];

	if (memcmp(dev->name, sep_itf, strlen(sep_itf)) == 0){
		snprintf(format, sizeof(format), "%s%%d", sep_itf);
		sscanf(dev->name, format, &eth_intf);
		return eth_intf - itf_start_idx;
	}else{
		return eth_intf;
	}
}
EXPORT_SYMBOL(getLogicLANIndex);

int getLogicLANIndexByName(char *pname)
{
	unsigned int eth_intf = 0;
	char format[10];

	if (memcmp(pname, sep_itf, strlen(sep_itf)) == 0){
		snprintf(format, sizeof(format), "%s%%d", sep_itf);
		sscanf(pname, format, &eth_intf);
		return eth_intf - itf_start_idx;
	}else{
		return eth_intf;
	}
}
EXPORT_SYMBOL(getLogicLANIndexByName);

char lanPortNamePre[] = "eth0.";
char *lanNamePre(void)
{
    return lanPortNamePre;
}
EXPORT_SYMBOL(lanNamePre);

int get24GWDSIndex(struct net_device *dev)
{
    return (dev->name[3] - '0');
}
EXPORT_SYMBOL(get24GWDSIndex);

int get5GWDSIndex(struct net_device *dev)
{
    return (dev->name[4] - '0');
}
EXPORT_SYMBOL(get5GWDSIndex);

int get24GWifiIndex(struct net_device *dev)
{
	long index = 0;
	char *str = dev->name;
    while (*str && !isdigit(*str)) {
        str++;
    }
    if (*str) {
        if (kstrtol(str, 10, &index) == 0)
        	return index;
    }

	return 0;
}
EXPORT_SYMBOL(get24GWifiIndex);

int get5GWifiIndex(struct net_device *dev)
{
	long index = 0;
	char *str = dev->name;
	while (*str && !isdigit(*str)) {
		str++;
	}
	if (*str) {
		if (kstrtol(str, 10, &index) == 0)
			return index;
	}

	return 0;
}
EXPORT_SYMBOL(get5GWifiIndex);

int get6GWifiIndex(struct net_device *dev)
{
	long index = 0;
	char *str = dev->name;
	while (*str && !isdigit(*str)) {
		str++;
	}
	if (*str) {
		if (kstrtol(str, 10, &index) == 0)
			return index;
	}

	return 0;
}
EXPORT_SYMBOL(get6GWifiIndex);

int get24GLogicWifiIndex(struct net_device *dev)
{
	long index = 0;
	char *str = dev->name;
	while (*str && !isdigit(*str)) {
		str++;
	}
	if (*str) {
		if (kstrtol(str, 10, &index) == 0)
			return index;
	}

	return 0;
}
EXPORT_SYMBOL(get24GLogicWifiIndex);

int get5GLogicWifiIndex(struct net_device *dev)
{
	long index = 0;
	char *str = dev->name;
	while (*str && !isdigit(*str)) {
		str++;
	}
	if (*str) {
		if (kstrtol(str, 10, &index) == 0)
			return index;
	}

	return 0;
}
EXPORT_SYMBOL(get5GLogicWifiIndex);

int get6GLogicWifiIndex(struct net_device *dev)
{
	long index = 0;
	char *str = dev->name;
	while (*str && !isdigit(*str)) {
		str++;
	}
	if (*str) {
		if (kstrtol(str, 10, &index) == 0)
			return index;
	}

	return 0;
}
EXPORT_SYMBOL(get6GLogicWifiIndex);

int getUSBIndex(struct net_device *dev)
{
    return (dev->name[3] - '0');
}
EXPORT_SYMBOL(getUSBIndex);

/*After calling this interface to get net_dev, we need to call dev_put to release it*/
struct net_device *get24GWifiName(int index)
{
	struct net_device dev;

	if(wlan_name_mode == 1)
	{
		int i = 0;
		long idx = 0;
		char *str = NULL;

		while(str = wlan_24g_name_array[i]) {
			while (!isdigit(*str))
				str++;

			if (*str) {
				if ((kstrtol(str, 10, &idx) == 0) && (idx == index))
					return dev_get_by_name(&init_net, wlan_24g_name_array[i]);
			}
        	i++;
		}
		return NULL;
	}
#if defined(TCSUPPORT_WLAN_MT76_MAC80211) || defined (TCSUPPORT_WLAN_INODE)
	dev.name[0] = 'w';
	dev.name[1] = 'l';
	dev.name[3] = 'a';
	dev.name[4] = 'n';
	dev.name[5] = '0';
	dev.name[6] = '-';
	dev.name[7] = '0'+index;
	dev.name[8] = '\0';
#else
	dev.name[0] = 'r';
	dev.name[1] = 'a';
	dev.name[2] = '0'+index;
	dev.name[3] = '\0';
#endif
    return dev_get_by_name(&init_net, dev.name);
}
EXPORT_SYMBOL(get24GWifiName);

int global_vlan_enable;
int ecnt_global_vlan_enable_set(int enable)
{
	global_vlan_enable = enable;

	return 0;
}
EXPORT_SYMBOL(ecnt_global_vlan_enable_set);

int ecnt_global_vlan_allowed_egress(const struct net_device *dst_dev,
						const const struct net_device *src_dev)
{
	if((global_vlan_enable == 0) || strncmp(src_dev->name, dst_dev->name, 5) || (src_dev->name[6] - '0') >= 0 || (dst_dev->name[6] - '0') >= 0)
		return 1;
	else
		return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
struct class	*hnat_class1;
struct device *ecnt_device_create(int result)
{
	hnat_class1 = class_create(THIS_MODULE, "hwnat0");
	
	return device_create(hnat_class1,NULL,MKDEV(result,0),NULL,"hwnat0");
}
EXPORT_SYMBOL(ecnt_device_create);

void ecnt_device_delete(int result)
{
	device_destroy(hnat_class1,MKDEV(result,0));
	class_destroy(hnat_class1);
}
EXPORT_SYMBOL(ecnt_device_delete);
#endif

#ifndef AIROHA_BSP
#include "ecnt_net_utility_sdk.c"
#endif/*AIROHA_BSP*/
