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
*************************************************************************
*/
#include <linux/module.h>
#include <asm/tc3162/tc3162.h>
#include <linux/netdevice.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_ifc.h>
#include "fe_reg_en7512.h"
#include "fe.h"
#include "fe_api.h"
#include "fe_ic_dis.h"
#if defined(TCSUPPORT_CPU_EN7580)
#include "fe_resource_manage.h"
#endif
#include <ecnt_hook/ecnt_hook_pon_mac.h>
#include <ecnt_hook/ecnt_hook_ether.h>

#include <ecnt_hook/ecnt_hook_hsgmii_mac.h>
#if SUPPORT_EVENT_SYSTEM
#include "ecnt_event_global/ecnt_event_system.h"
#endif

#include <linux/libcompileoption.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
uint32 fe_gdma1_sel = 0xffffffff;
uint32 fe_gdma2_sel = 0xffffffff;
unsigned int flowByteCnt_HW[8] = {0};
extern int channel_retire;
atomic_t qdma_stop_flag = ATOMIC_INIT(0);
EXPORT_SYMBOL(qdma_stop_flag);

extern unsigned long long gdm2_rx_len_high;
extern unsigned long long gdm2_tx_len_high;
extern unsigned long long gdm2_rx_drop_high;
extern unsigned long long gdm2_tx_drop_high;
extern SMUX_Bridge_Info_Data pppoe_info_for_app;

extern unsigned int g_fe_iq_reset_cnt;
extern unsigned int g_fe_all_reset_cnt;
extern unsigned int g_fe_qdma_reset_cnt;
extern unsigned int g_fe_last_qdma_dscp;
extern unsigned int g_fe_qdma_lan_reset_cnt;
extern unsigned int g_fe_last_qdma_lan_dscp;

extern unsigned int *tx_ok_cnt[ANI_UNI_PORT_MAX];
extern unsigned int *tx_ok_byte_cnt[ANI_UNI_PORT_MAX];
extern unsigned int *rx_ok_cnt[ANI_UNI_PORT_MAX];
extern unsigned int *rx_ok_byte_cnt[ANI_UNI_PORT_MAX];
extern unsigned int *rx_discard_cnt[ANI_UNI_PORT_MAX];
extern unsigned int *tx_discard_cnt[ANI_UNI_PORT_MAX];
extern unsigned int *rx_error_cnt[ANI_UNI_PORT_MAX];
extern unsigned int *tx_error_cnt[ANI_UNI_PORT_MAX];

extern int fe_cnt_cur_idx;
extern int g_wan_mode;
#ifdef TCSUPPORT_IFC_EN
extern unsigned int ifcSendToPpe;
#endif
/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define FE_VIP_TYPE_ETHER		0
#define FE_VIP_TYPE_PPPOE 		1
#define FE_VIP_TYPE_IP 			2
#define FE_VIP_TYPE_TCP 		3
#define FE_VIP_TYPE_UDP 		4

#define FE_VIP_PACKET_NUM  		32
#define FE_L2LU_KEY_NUM 		15

#define	UP_QUEUE_CNT_START		16
#define FLOW_NUMBER			8
#define MC_PKT_ACNT_ID				56

#define METER_SCALE_K	(1000)
#define METER_SCALE_M	(METER_SCALE_K<<10)
#define METER_SCALE_G	(METER_SCALE_M<<10)

#define METER_TOKEN_RATE_BYTE_MAX	0x3fff

/* EN7523 PSE buffer = 128K + 32k GDMP */
#define PSE_BUFFER_SIZE		0x500
#define GDM_RX_MAC_FILTER_MAX_NUM	16
/*uint of GDM_TX_MAX_RATE is kbps*/
#define GDM_TX_MAX_RATE 16777215
/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

typedef struct fe_vip_packet_s{
	unsigned short en: 1;
	unsigned short type: 3;
	unsigned short sp_en: 1;
	unsigned short dp_en: 1;
	unsigned short sw_en: 1;
	unsigned short res: 9;
	union{	
		unsigned short sport;
		unsigned short etype;
		unsigned short ppp;
		unsigned short iptype;
	};
	unsigned short dport;	
} fe_vip_packet;
	
typedef struct fe_l2lu_key_s{
	unsigned int dscp_mask: 8;
	unsigned int cvid_en: 1;
	unsigned int cpcp_en: 1;
	unsigned int res0: 2;
	unsigned int svid_en: 1;
	unsigned int spcp_en: 1;
	unsigned int res1: 2;	 
	unsigned int dscp: 16;
	unsigned int cvid: 12;
	unsigned int res2: 1;	
	unsigned int cpcp: 3;
	unsigned int svid: 12;
	unsigned int res3: 1;	 
	unsigned int spcp: 3;	
} fe_l2lu_key;

enum {
    METER_TICK_BYTE_125US = 0,
    METER_TICK_BYTE_1MS
};

enum {
    METER_BUCKET_64KB = 0,
    METER_BUCKET_32KB,
    METER_BUCKET_16KB,
    METER_BUCKET_4KB
};

enum {
    MAC_FILTER_DISABLE = 0,
    MAC_FILTER_ENABLE,
    MAC_FILTER_ADD,
    MAC_FILTER_DEL,
    MAC_FILTER_MAX_NUM
};

typedef enum {
	XSI_PCIE0_IDX = 0,
	XSI_PCIE1_IDX = 1,
	XSI_USB_IDX   = 2, 
	XSI_AE_IDX    = 3,
	XSI_ETH_IDX   = 4,
	GSW_ETH_IDX   = 5,
	XSIS_SEL_MAX_NUM
}xsi_sel_type;

static unsigned int g_Ppe_Ac_Pcnt1 = 0;
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int fe_ioctl_set_tls_cfg(unsigned char cmd,unsigned char tls_port,unsigned short tls_vlan);
extern int fe_api_tls_forwad(struct ecnt_fe_data *fe_data);
extern int fe_ioctl_set_trunk_mode_tls_cfg(unsigned char cmd,FE_Tls_info_t tls_info);

extern int fe_api_init_resource_manage(struct ecnt_fe_data *fe_data);
extern int fe_api_deinit_resource_manage(struct ecnt_fe_data *fe_data);
extern int fe_api_set_meter_ratelimit(struct ecnt_fe_data *fe_data);
extern int fe_api_get_meter_ratelimit(struct ecnt_fe_data *fe_data);
extern int fe_api_get_meter_idx(struct ecnt_fe_data *fe_data);
extern int fe_api_get_acnt2_idx(struct ecnt_fe_data *fe_data);
extern int fe_api_get_acnt1_idx(struct ecnt_fe_data *fe_data);
extern int fe_api_get_acnt0_idx(struct ecnt_fe_data *fe_data);
extern int fe_api_get_rx_ratelimit_mode(struct ecnt_fe_data *fe_data);
extern int fe_api_set_rx_ratelimit_rule(struct ecnt_fe_data *fe_data);
extern int fe_api_set_rx_ratelimit_mode(struct ecnt_fe_data *fe_data);
extern int fe_api_set_meter_ctl_by_olt(struct ecnt_fe_data *fe_data);
extern int fe_api_get_flow_cnt(struct ecnt_fe_data *fe_data);
extern int fe_api_get_acnt0_mode(struct ecnt_fe_data *fe_data);
extern int fe_api_get_acnt1_mode(struct ecnt_fe_data *fe_data);
extern int fe_api_set_acnt0_mode(struct ecnt_fe_data *fe_data);
extern int fe_api_set_acnt1_mode(struct ecnt_fe_data *fe_data);
extern int fe_api_set_acnt2_mode(struct ecnt_fe_data *fe_data);
extern int fe_api_get_meter_enable(struct ecnt_fe_data *fe_data);
extern int fe_api_clear_flow_cnt(struct ecnt_fe_data *fe_data);
extern int fe_api_get_dev_mac_index(struct ecnt_fe_data *fe_data);
extern int fe_api_set_pse_oq_threshold(struct ecnt_fe_data *fe_data);
extern int fe_api_get_wan_itf_index(struct ecnt_fe_data *fe_data);
extern int fe_api_set_ratelimit_for_pkt_formate(struct ecnt_fe_data *fe_data);
extern void fe_set_per_oq_rsv(uint port,uint oq,uint val);

extern spinlock_t fe_pse_reset_lock;

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int fe_api_set_pkt_length(struct ecnt_fe_data *fe_data);
int fe_api_set_channel_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_mac_addr(struct ecnt_fe_data *fe_data);
int fe_api_set_mac_addr_7516(struct ecnt_fe_data *fe_data);
int fe_api_set_wan_port_7516(struct ecnt_fe_data *fe_data);
int fe_get_hwfwd_channel(struct ecnt_fe_data *fe_data);
int fe_set_hwfwd_channel(struct ecnt_fe_data *fe_data);
int fe_api_set_channel_retire(struct ecnt_fe_data *fe_data);
int fe_api_set_crc_strip(struct ecnt_fe_data *fe_data);
int fe_api_set_padding(struct ecnt_fe_data *fe_data);
int fe_api_get_ext_tpid(struct ecnt_fe_data *fe_data);
int fe_api_set_ext_tpid(struct ecnt_fe_data *fe_data);
int fe_api_get_fw_cfg(struct ecnt_fe_data *fe_data);
int fe_api_set_fw_cfg(struct ecnt_fe_data *fe_data);
int fe_api_set_drop_udp_chksum_err_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_drop_tcp_chksum_err_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_drop_ip_chksum_err_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_drop_runt_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_drop_long_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_drop_crc_err_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_vlan_check(struct ecnt_fe_data *fe_data);
int fe_api_get_ok_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_err_crc_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_drop_fifo_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_drop_err_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_ok_byte_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_tx_get_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_tx_drop_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_time_stamp(struct ecnt_fe_data *fe_data);
int fe_api_set_time_stamp(struct ecnt_fe_data *fe_data);
int fe_api_set_ins_vlan_tpid(struct ecnt_fe_data *fe_data);
int fe_api_set_vlan_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_black_list(struct ecnt_fe_data *fe_data);
int fe_api_set_ether_type(struct ecnt_fe_data *fe_data);
int fe_api_set_L2U_key(struct ecnt_fe_data *fe_data);
int fe_api_get_ac_group_pkt_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_ac_group_byte_cnt(struct ecnt_fe_data *fe_data);
int fe_api_clear_ac_group_pkt_cnt(struct ecnt_fe_data *fe_data);
int fe_api_clear_ac_group_byte_cnt(struct ecnt_fe_data *fe_data);
int fe_api_set_meter_group(struct ecnt_fe_data *fe_data);
int fe_api_get_meter_group(struct ecnt_fe_data *fe_data);
int fe_api_set_gdm_pcp_coding(struct ecnt_fe_data *fe_data);
int fe_api_set_cdm_pcp_coding(struct ecnt_fe_data *fe_data);
int fe_api_set_vip_enable(struct ecnt_fe_data *fe_data);
int fe_api_get_eth_rx_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_eth_tx_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_eth_frame_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_eth_err_cnt(struct ecnt_fe_data *fe_data);
int fe_api_set_clear_mib(struct ecnt_fe_data *fe_data);
int fe_api_set_cdm_rx_red_drop_mode(struct ecnt_fe_data *fe_data);
int fe_api_get_cdm_rx_red_drop_mode(struct ecnt_fe_data *fe_data);
int fe_api_set_channel_retire_all(struct ecnt_fe_data *fe_data);
int fe_api_set_channel_retire_one(struct ecnt_fe_data *fe_data);
int fe_api_set_tx_rate(struct ecnt_fe_data *fe_data);
int fe_api_set_rx_rate(struct ecnt_fe_data *fe_data);
int fe_api_set_rxuc_rate(struct ecnt_fe_data *fe_data);
int fe_api_set_rxbc_rate(struct ecnt_fe_data *fe_data);
int fe_api_set_rxmc_rate(struct ecnt_fe_data *fe_data);
int fe_api_set_rxoc_rate(struct ecnt_fe_data *fe_data);
int fe_api_set_rx_mac_filter_rate(struct ecnt_fe_data *fe_data);
int fe_api_add_vip_ether(struct ecnt_fe_data *fe_data);
int fe_api_add_vip_ppp(struct ecnt_fe_data *fe_data);
int fe_api_add_vip_ip(struct ecnt_fe_data *fe_data);
int fe_api_add_vip_tcp(struct ecnt_fe_data *fe_data);
int fe_api_add_vip_udp(struct ecnt_fe_data *fe_data);
int fe_api_del_vip_ether(struct ecnt_fe_data *fe_data);
int fe_api_del_vip_ppp(struct ecnt_fe_data *fe_data);
int fe_api_del_vip_ip(struct ecnt_fe_data *fe_data);
int fe_api_del_vip_tcp(struct ecnt_fe_data *fe_data);
int fe_api_del_vip_udp(struct ecnt_fe_data *fe_data);
int fe_api_add_l2lu_vlan_dscp(struct ecnt_fe_data *fe_data);
int fe_api_add_l2lu_vlan_trfc(struct ecnt_fe_data *fe_data);
int fe_api_del_l2lu_vlan_dscp(struct ecnt_fe_data *fe_data);
int fe_api_del_l2lu_vlan_trfc(struct ecnt_fe_data *fe_data);
int fe_api_add_traffic_class(struct ecnt_fe_data * fe_data);
int fe_api_del_traffic_class(struct ecnt_fe_data * fe_data);
int fe_api_set_tx_favor_oam_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_tls_cfg(struct ecnt_fe_data *fe_data);
int fe_api_do_fe_reset(struct ecnt_fe_data *fe_data);
int fe_api_set_loopback_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_loopback_mode(struct ecnt_fe_data *fe_data);
int fe_api_get_unknown_mul_pkt(struct ecnt_fe_data *fe_data);
int fe_api_set_glo_rate_byte(struct ecnt_fe_data *fe_data);
int fe_api_get_pppoe_info(struct ecnt_fe_data *fe_data);
int fe_api_set_pppoe_info_clean(struct ecnt_fe_data *fe_data);
int fe_api_get_tx_traffic(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_traffic(struct ecnt_fe_data *fe_data);
int fe_api_get_tx_rate(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_rate(struct ecnt_fe_data *fe_data);
int fe_api_get_tx_octets(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_octets(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_discard_counter(struct ecnt_fe_data *fe_data);
int fe_api_get_tx_discard_counter(struct ecnt_fe_data *fe_data);
int fe_api_get_rx_error_counter(struct ecnt_fe_data *fe_data);
int fe_api_get_tx_error_counter(struct ecnt_fe_data *fe_data);
int fe_api_add_dev_to_total_account(struct ecnt_fe_data *fe_data);
int fe_api_add_stb_src_ip(struct ecnt_fe_data *fe_data);
int fe_api_del_stb_src_ip(struct ecnt_fe_data *fe_data);

int fe_api_set_rx_mac_filter(struct ecnt_fe_data *fe_data);

int fe_api_set_mc_vlan_global(struct ecnt_fe_data *fe_data);
int fe_api_get_mc_vlan_global(struct ecnt_fe_data *fe_data);
int fe_api_set_mc_vlan_table_cfg(struct ecnt_fe_data *fe_data);
int fe_api_get_mc_vlan_table_cfg(struct ecnt_fe_data *fe_data);
int fe_api_set_mc_vlan_action_cfg(struct ecnt_fe_data *fe_data);
int fe_api_get_mc_vlan_action_cfg(struct ecnt_fe_data *fe_data);
int fe_api_set_mc_vlan_clear_all(struct ecnt_fe_data *fe_data);
int fe_api_xfi_link_change(struct ecnt_fe_data *fe_data);
int fe_api_set_gdma_misc_config(struct ecnt_fe_data *fe_data);
int fe_api_get_hsgmii_rx_cnt(struct ecnt_fe_data *fe_data);
int fe_api_get_hsgmii_tx_cnt(struct ecnt_fe_data *fe_data);
int fe_api_set_aewan_fwdfq(struct ecnt_fe_data *fe_data);
int fe_api_set_aewan_ifcdisable(struct ecnt_fe_data *fe_data);
int fe_api_set_gdm2_sptag_for_loopback(struct ecnt_fe_data *fe_data);
int fe_api_set_tunnel_cfg(struct ecnt_fe_data *fe_data);
int fe_api_set_gdm_sptag_for_extswitch(struct ecnt_fe_data *fe_data);
int fe_api_pse_oq_rsv_en(struct ecnt_fe_data *fe_data);
int fe_api_set_hsgmii_rx_port_ratelimit(struct ecnt_fe_data *fe_data);
int fe_api_set_mbi_arb_rst(struct ecnt_fe_data *fe_data);
int fe_api_set_rmbi_frag(struct ecnt_fe_data *fe_data);
int fe_api_get_chn_rls(struct ecnt_fe_data *fe_data);
int fe_api_set_tmbi_frag(struct ecnt_fe_data *fe_data);
int fe_api_set_gdma_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_gdma_disable(struct ecnt_fe_data *fe_data);
int fe_api_set_chn_retire_action(struct ecnt_fe_data *fe_data);
int fe_api_set_chn_retire_done(struct ecnt_fe_data *fe_data);
int fe_api_set_qbi_fttr_chn_disable(struct ecnt_fe_data *fe_data);
int fe_api_set_force_slow_enable(struct ecnt_fe_data *fe_data);
int fe_api_set_force_slow_duty(struct ecnt_fe_data *fe_data);
int fe_api_set_vip_rxq_selection(struct ecnt_fe_data *fe_data);
int fe_api_set_vip_for_tcp_speedtest(struct ecnt_fe_data *fe_data);
int fe_api_set_dev_stat_ratelimit_mode(struct ecnt_fe_data *fe_data);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
int fe_disable_mac_filter(FE_RxMacFilter_t macfilter);
int fe_enable_mac_filter(FE_RxMacFilter_t macfilter);
int fe_add_mac_filter(FE_RxMacFilter_t macfilter);
int fe_del_mac_filter(FE_RxMacFilter_t macfilter);

/*
  *spinlock_t hookFuncLock[FE_FUNCTION_MAX_NUM] ;
 */

typedef int (*fe_api_op_t)(struct ecnt_fe_data *fe_data);

static unsigned int ac_byte_cnt_lo[FE_AC_MTR_GROUP_MAX]={0};
static unsigned int ac_byte_cnt_hi[FE_AC_MTR_GROUP_MAX]={0};
static unsigned int ac_packet_cnt[FE_AC_MTR_GROUP_MAX]={0};

/* Warning: same sequence with enum 'FE_HookFunctionID_t' in ecnt_hook_fe.h */
fe_api_op_t
fe_operation[]=
{
	fe_api_set_pkt_length,
	fe_api_set_channel_enable,
	fe_api_set_mac_addr,
	fe_set_hwfwd_channel,
	fe_api_set_channel_retire,
	fe_api_set_crc_strip,
	fe_api_set_padding,
	fe_api_get_ext_tpid,
	fe_api_set_ext_tpid,
	fe_api_get_fw_cfg,
	fe_api_set_fw_cfg,
	fe_api_set_drop_udp_chksum_err_enable,
	fe_api_set_drop_tcp_chksum_err_enable,
	fe_api_set_drop_ip_chksum_err_enable,
	fe_api_set_drop_crc_err_enable,
	fe_api_set_drop_runt_enable,
	fe_api_set_drop_long_enable,
	fe_api_set_vlan_check,
	fe_api_get_ok_cnt,
	fe_api_get_rx_err_crc_cnt,
	fe_api_get_rx_drop_fifo_cnt,
	fe_api_get_rx_drop_err_cnt,
	fe_api_get_ok_byte_cnt,
	fe_api_get_tx_get_cnt,
	fe_api_get_tx_drop_cnt,
	fe_api_get_time_stamp,
	fe_api_set_time_stamp,
	fe_api_set_ins_vlan_tpid,
	fe_api_set_vlan_enable,
	fe_api_set_black_list,
	fe_api_set_ether_type,
	fe_api_set_L2U_key,
	fe_api_get_ac_group_pkt_cnt,
	fe_api_get_ac_group_byte_cnt,
	fe_api_clear_ac_group_pkt_cnt,
	fe_api_clear_ac_group_byte_cnt,
	fe_api_set_meter_group,
	fe_api_get_meter_group,
	fe_api_set_gdm_pcp_coding,
	fe_api_set_cdm_pcp_coding,
	fe_api_set_vip_enable,
	fe_api_get_eth_rx_cnt,
	fe_api_get_eth_tx_cnt,
	fe_api_get_eth_frame_cnt,
	fe_api_get_eth_err_cnt,
	fe_api_set_clear_mib,
	fe_api_set_cdm_rx_red_drop_mode,
	fe_api_get_cdm_rx_red_drop_mode,
	fe_api_set_channel_retire_all,
	fe_api_set_channel_retire_one,
	fe_api_set_tx_rate,
	fe_api_set_rxuc_rate,
	fe_api_set_rxbc_rate,
	fe_api_set_rxmc_rate,
	fe_api_set_rxoc_rate,
	fe_api_add_vip_ether,
	fe_api_add_vip_ppp,
	fe_api_add_vip_ip,
	fe_api_add_vip_tcp,
	fe_api_add_vip_udp,
	fe_api_del_vip_ether,
	fe_api_del_vip_ppp,
	fe_api_del_vip_ip,
	fe_api_del_vip_tcp,
	fe_api_del_vip_udp,	
	fe_api_add_l2lu_vlan_dscp,
	fe_api_add_l2lu_vlan_trfc,
	fe_api_del_l2lu_vlan_dscp,
	fe_api_del_l2lu_vlan_trfc,
	fe_api_add_traffic_class,
    fe_api_del_traffic_class,
	fe_api_set_tx_favor_oam_enable,
	fe_api_set_tls_cfg,
	fe_api_tls_forwad,
	fe_api_do_fe_reset,
	fe_api_set_mac_addr_7516,
	fe_api_set_wan_port_7516,
	fe_api_set_loopback_enable,
	fe_api_set_loopback_mode,
	fe_api_get_unknown_mul_pkt,
	fe_api_set_meter_ratelimit,
	fe_api_get_meter_ratelimit,
	fe_api_get_meter_idx,
	fe_api_get_acnt1_idx,
	fe_api_get_acnt0_idx,
	fe_api_init_resource_manage,
	fe_api_deinit_resource_manage,
	fe_api_set_rx_ratelimit_rule,
	fe_api_set_rx_ratelimit_mode,
	fe_api_set_meter_ctl_by_olt,
	fe_api_get_flow_cnt,
	fe_api_clear_flow_cnt,
	fe_api_get_acnt0_mode,
	fe_api_get_acnt1_mode,
	fe_api_set_acnt0_mode,
	fe_api_set_acnt1_mode,
	fe_api_get_meter_enable,
	fe_api_get_dev_mac_index,
	fe_api_set_pse_oq_threshold,
	fe_api_get_acnt2_idx,
	fe_api_set_acnt2_mode,
	fe_api_get_wan_itf_index,
	fe_api_set_glo_rate_byte,
	fe_api_get_pppoe_info,
	fe_api_set_pppoe_info_clean,
	fe_api_get_tx_traffic,  /* kbps */
	fe_api_get_rx_traffic,
	fe_api_get_tx_rate,     /* pps */
	fe_api_get_rx_rate,
	fe_api_get_tx_octets,  /* byte cnt in 15 minutes */
	fe_api_get_rx_octets,
	fe_api_get_rx_discard_counter, /*cnt in 15 minutes*/
	fe_api_get_tx_discard_counter,
	fe_api_get_rx_error_counter,
	fe_api_get_tx_error_counter,
	fe_api_add_dev_to_total_account,
	fe_api_add_stb_src_ip,
	fe_api_del_stb_src_ip,
	fe_api_set_ratelimit_for_pkt_formate,
	fe_api_set_mc_vlan_global,
	fe_api_get_mc_vlan_global,
	fe_api_set_mc_vlan_table_cfg,
	fe_api_get_mc_vlan_table_cfg,
	fe_api_set_mc_vlan_action_cfg,
	fe_api_get_mc_vlan_action_cfg,
	fe_api_set_mc_vlan_clear_all,
	fe_api_set_rx_mac_filter,
	fe_api_set_rx_mac_filter_rate,
	fe_api_xfi_link_change,
	fe_api_set_gdma_misc_config,
	fe_api_get_rx_ratelimit_mode,
	fe_api_get_hsgmii_rx_cnt,
	fe_api_get_hsgmii_tx_cnt,
	fe_api_set_aewan_fwdfq,
	fe_api_set_aewan_ifcdisable,
	fe_api_set_gdm2_sptag_for_loopback,
	fe_api_set_rx_rate,
	fe_api_set_tunnel_cfg,
	fe_api_set_gdm_sptag_for_extswitch,
	fe_api_pse_oq_rsv_en,
	fe_api_set_hsgmii_rx_port_ratelimit,
	fe_api_set_mbi_arb_rst,
	fe_api_set_rmbi_frag,
	fe_api_get_chn_rls,
	fe_api_set_tmbi_frag,
	fe_api_set_gdma_enable,
	fe_api_set_gdma_disable,
    fe_api_set_chn_retire_action,
    fe_api_set_chn_retire_done,
    fe_api_set_qbi_fttr_chn_disable,
    fe_api_set_force_slow_enable,
    fe_api_set_force_slow_duty,
    fe_api_set_vip_rxq_selection,
    fe_api_set_vip_for_tcp_speedtest,
	fe_api_set_dev_stat_ratelimit_mode
};

typedef int (*fe_mac_filter)(FE_RxMacFilter_t macfilter);
fe_mac_filter 
mac_filter_operation[]= 
{
	fe_disable_mac_filter,
	fe_enable_mac_filter,
	fe_add_mac_filter,
	fe_del_mac_filter
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int  feChecConfigDone(uint reg, uint doneBit)
{
	int RETRY = 3 ;
	volatile uint regValue = 0 ;
	
	while(RETRY--) {
		regValue = IO_GREG(reg) ;
		
		if(regValue & doneBit) {
			break ;
		}
		mdelay(1) ;
	}
	if(RETRY < 0) {
		return -ETIME ;
	}

	return 0;
}

unsigned int fe_reg_read(unsigned int reg_offset){
	return read_reg_word(FE_BASE + reg_offset);

}

void fe_reg_write(unsigned int reg_offset, unsigned int value){
	write_reg_word((FE_BASE + reg_offset), value);
	return;
}

void fe_reg_modify_bits(unsigned int reg_offset, unsigned int Data, unsigned int Offset, unsigned int Len)
{
	unsigned int Mask = 0;
	unsigned int Value;
	unsigned int i;

	for (i = 0; i < Len; i++) {
		Mask |= 1 << (Offset + i);
	}

	Value = fe_reg_read(reg_offset);
	Value &= ~Mask;
	Value |= (Data << Offset) & Mask;;

	fe_reg_write(reg_offset, Value);
}

ecnt_ret_val ecnt_fe_api_hook(struct ecnt_data *in_data)
{
	struct ecnt_fe_data *fe_data = (struct ecnt_fe_data *)in_data ;
	/* ulong flags = 0 ; */
	
	if(fe_data->function_id >= FE_FUNCTION_MAX_NUM) {
		printk("fe_data->function_id is %d, exceed max number: %d", fe_data->function_id, FE_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}

	/* spin_lock_irqsave(&hookFuncLock[fe_data->function_id], flags) ; */
	fe_data->retValue = fe_operation[fe_data->function_id](fe_data) ;
	/* spin_unlock_irqrestore(&hookFuncLock[fe_data->function_id], flags) ; */
	
	return ECNT_CONTINUE;
}

int fe_api_set_pkt_length(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	uint length_long = fe_data->api_data.pkt_len.length_long;
	uint length_short = fe_data->api_data.pkt_len.length_short;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_LEN_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
	    base_addr = GDMA2_LEN_CFG;
    }else if(gdm_sel == FE_GDM_SEL_GDMA3){
		base_addr = GDMA3_LEN_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA4){
		base_addr = GDMA4_LEN_CFG;
	}else{
		printk("input gdm_sel error gdm_sel=%d\n",gdm_sel);
		return 0;
	}
	
	val = read_reg_word(base_addr);
	if(length_long != 0)
		val &= ~(0xffff<<16);
	if(length_short != 0)
		val &= ~(0xffff);
	
	val |= ((length_long << 16) | length_short);
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_channel_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_TXRX_Sel_t txrx_sel = fe_data->txrx_sel;
	FE_Enable_t enable = fe_data->api_data.enable;
	uint channel = fe_data->channel;
    uint32 flags = 0;

    if (isEN751221)
    {
	    spin_lock_irqsave(&fe_pse_reset_lock, flags);
    }

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		if(txrx_sel == FE_GDM_SEL_TX)
			base_addr = GDMA1_TXCHN_EN;
		else
			base_addr = GDMA1_RXCHN_EN;
	}else{
		if(txrx_sel == FE_GDM_SEL_TX)
			base_addr = GDMA2_TXCHN_EN;
		else
			base_addr = GDMA2_RXCHN_EN;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<channel);
	}else{
		val |= (1<<channel);
	}
	write_reg_word(base_addr, val);

    if (isEN751221)
    {
	    spin_unlock_irqrestore(&fe_pse_reset_lock, flags);
    }
    
	return 0;
}

int fe_api_set_mac_addr_7526(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr_lsb, base_addr_msb, val;
	unchar *mac = fe_data->api_data.mac_addr.mac;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int mask = fe_data->api_data.mac_addr.mask;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr_lsb = GDMA1_MAC_ADRL;
		base_addr_msb = GDMA1_MAC_ADRH;
	}else{
		base_addr_lsb = GDMA2_MAC_ADRL;
		base_addr_msb = GDMA2_MAC_ADRH;
	}

	val = (mac[2]<<24) | (mac[3]<<16) | (mac[4]<<8) | (mac[5]<<0);
	write_reg_word(base_addr_lsb, val);
	val = (mask<<16) | (mac[0]<<8) | (mac[1]<<0);
	write_reg_word(base_addr_msb, val);
	return 0;
}


int fe_api_set_mac_addr_7516(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr_mac_h, base_addr_mac_lmin,base_addr_mac_lmax;
	unsigned int mac_h = fe_data->api_data.mac_addr_7516.mac_h;
	unsigned int mac_lmin = fe_data->api_data.mac_addr_7516.mac_lmin;
	unsigned int mac_lmax = fe_data->api_data.mac_addr_7516.mac_lmax;
	FE_MacSet_Sel_t macSet_sel = fe_data->macSet_sel;
	
	if(macSet_sel == FE_MAC_SET_LAN){
		base_addr_mac_h = FE_LAN_MAC_H;
		base_addr_mac_lmin = FE_LAN_MAC_LMIN;
		base_addr_mac_lmax = FE_LAN_MAC_LMAX;
	}else{
		base_addr_mac_h = FE_WAN_MAC_H;
		base_addr_mac_lmin = FE_WAN_MAC_LMIN;
		base_addr_mac_lmax = FE_WAN_MAC_LMAX;
	}
	
	write_reg_word(base_addr_mac_h, mac_h);
	write_reg_word(base_addr_mac_lmin, mac_lmin);
	write_reg_word(base_addr_mac_lmax, mac_lmax);
	return 0;
}

int fe_api_set_mac_addr(struct ecnt_fe_data *fe_data)
{
	struct ecnt_fe_data fe_data_7527;
	if((isEN751627) || (FE_MAC_RANGE_SUPPORT)){
		unsigned int range;
		unchar *mac = fe_data->api_data.mac_addr.mac;
		unsigned int mac_h = mac[0]<<16 | mac[1]<<8 | mac[2];
		unsigned int mac_lmin = mac[3]<<16 | mac[4]<<8 | mac[5];
		range = fe_data->api_data.mac_addr.mask; // EN7527 not support my mac mask
		unsigned int mac_lmax = mac_lmin + range;
		
		fe_data_7527.api_data.mac_addr_7516.mac_h = mac_h;
		fe_data_7527.api_data.mac_addr_7516.mac_lmax = mac_lmax;
		fe_data_7527.api_data.mac_addr_7516.mac_lmin = mac_lmin;
		fe_data_7527.macSet_sel = fe_data->gdm_sel;
		
		fe_api_set_mac_addr_7516(&fe_data_7527);
		
	}else{
		fe_api_set_mac_addr_7526(fe_data);
	}
	
	return 0;
}

int fe_api_set_wan_port_7516(struct ecnt_fe_data *fe_data)
{
	unchar wan1_en = fe_data->api_data.wan_port_7516.wan1_en;
	unchar wan1_port = fe_data->api_data.wan_port_7516.wan1_port;
	unchar wan0_port = fe_data->api_data.wan_port_7516.wan0_port;
	unsigned long int val;

	val = (wan1_en<<WAN_PORT_WAN1_EN_OFFSET)|(wan1_port<<WAN_PORT_WAN1_PORT_OFFSET)|wan0_port;

	write_reg_word(FE_WAN_PORT, val);
	
	return 0;
}

int fe_get_hwfwd_channel(struct ecnt_fe_data *fe_data)
{
	unsigned int	reg, value ;
	FE_Cdma_Sel_t	cdm_sel = fe_data->cdm_sel;
	uint			channel = fe_data->channel;

	if (cdm_sel == FE_CDM_SEL_CDMA1){
		reg = CDMA1_HWF_CHN_EN;
	}else if(cdm_sel == FE_CDM_SEL_CDMA2){
		reg = CDMA2_HWF_CHN_EN;
	}else{
		return -1;
	}

	value = read_reg_word(reg);

	value &= (1 << channel);

	fe_data->api_data.enable = (value >> channel);

	return 0;
}

int fe_set_hwfwd_channel(struct ecnt_fe_data *fe_data)
{
	FE_Cdma_Sel_t	cdm_sel = fe_data->cdm_sel;
	uint			channel = fe_data->channel;
	FE_Enable_t		enable = fe_data->api_data.enable;
	unsigned int	reg, value ;

	if (cdm_sel == FE_CDM_SEL_CDMA1){
		reg = CDMA1_HWF_CHN_EN;
	}else if(cdm_sel == FE_CDM_SEL_CDMA2){
		reg = CDMA2_HWF_CHN_EN;
	}else{
		return -1;
	}

	value = read_reg_word(reg);

	value &= ~(1 << channel);

	value |= (enable << channel);
	
	write_reg_word(reg, value);

	return 0;
}

#ifdef TCSUPPORT_CPU_EN7580
int mbi_hang_unlock_by_aging(FE_Gdma_Sel_t idx, uint aging_sel)
{
    unsigned int reg=0, value=0;
    
    if (idx == FE_GDM_SEL_GDMA1) {
        reg = GDMA1_CHN_RLS;
    } else if(idx == FE_GDM_SEL_GDMA2) {
        reg = GDMA2_CHN_RLS;
    } else {
        return -1;
    }

    value = read_reg_word(reg);
    value &= ~(MBI_TX_AGE_SEL_MASK | MBI_RX_AGE_SEL_MASK);
    value |= (aging_sel << MBI_TX_AGE_SEL_OFFSET) & MBI_TX_AGE_SEL_MASK;
    value |= (aging_sel << MBI_RX_AGE_SEL_OFFSET) & MBI_RX_AGE_SEL_MASK;
    write_reg_word(reg, value);

    return 0;
}

int mbi_hang_unlock_by_terminate(FE_Gdma_Sel_t idx)
{
    unsigned int reg=0, value=0, cnt=0, mbi_ok_flag=0;
    
    if (idx == FE_GDM_SEL_GDMA1) {
        reg = GDMA1_CHN_RLS;
    } else if(idx == FE_GDM_SEL_GDMA2) {
        reg = GDMA2_CHN_RLS;
    } else {
        return -1;
    }

    cnt = mbi_ok_flag = 0;
    do {
        if((read_reg_word(reg) & (1 << MBI_TX_BUSY_OFFSET)) == 0) {
            mbi_ok_flag = 1;
            break;
        }
    } while((cnt++)<=10);
    if(mbi_ok_flag == 0) {
        value = read_reg_word(reg);
        write_reg_word(reg, value | (1<<MBI_TX_TERMINATE_OFFSET) );
        cnt = 0;
        do {
            if((read_reg_word(reg) & (1 << MBI_TX_BUSY_OFFSET)) == 0)
                break;
        } while((cnt++)<=10);
        if(cnt>10) {
            printk("Error: MBI TX Hang issue Terminate Fail!\n");
            return -1;
        }
        write_reg_word(reg, value & (~(1<<MBI_TX_TERMINATE_OFFSET)) );
    }
    
    cnt = mbi_ok_flag = 0;
    do {
        if((read_reg_word(reg) & (1 << MBI_RX_BUSY_OFFSET)) == 0) {
            mbi_ok_flag = 1;
            break;
        }
    } while((cnt++)<=10);
    if(mbi_ok_flag == 0) {
        value = read_reg_word(reg);
        write_reg_word(reg, value | (1<<MBI_RX_TERMINATE_OFFSET) );
        cnt = 0;
        do {
            if((read_reg_word(reg) & (1 << MBI_RX_BUSY_OFFSET)) == 0)
                break;
        } while((cnt++)<=10);
        if(cnt>10) {
            printk("Error: MBI RX Hang issue Terminate Fail!\n");
            return -1;
        }
        write_reg_word(reg, value & (~(1<<MBI_RX_TERMINATE_OFFSET)) );
    }
    
    return 0;
}
#endif

static int fe_channel_retire_one(FE_Gdma_Sel_t idx,uint chn)
{
	unsigned int txreg=0, rxreg=0, txchn=0, rxchn=0;
	unsigned int reg=0, reg2=0, reg3=0, value=0;
	unsigned int rlsCnt=0, probe=0;
	int ret = 0;

#ifdef TCSUPPORT_CPU_EN7580
	if (idx == FE_GDM_SEL_GDMA1){
		txreg = GDMA1_TXCHN_EN;
		rxreg = GDMA1_RXCHN_EN;
		reg = GDMA1_CHN_RLS;
		reg2 = GDMA1_TX_CHN_VLD;
		reg3 = QDMA1_CHN_VLD_BASE;
	}else if(idx == FE_GDM_SEL_GDMA2){
		txreg = GDMA2_TXCHN_EN;
		rxreg = GDMA2_RXCHN_EN;
		reg = GDMA2_CHN_RLS;
		reg2 = GDMA2_TX_CHN_VLD;
		reg3 = QDMA2_CHN_VLD_BASE;
	}else if(idx == FE_GDM_SEL_GDMA3){
        txreg = GDMA3_TXCHN_EN;
        rxreg = GDMA3_RXCHN_EN;
        reg = GDMA3_CHN_RLS;
        reg2= GDMA3_RX_CHN_VLD;
		if(HWF_QDMA_SEL_SUPPORT&&FeGetHwfQdmaSelGdm3())
			reg3 = QDMA2_CHN_VLD_BASE;
		else
			reg3 = QDMA1_CHN_VLD_BASE;
	}else if(idx == FE_GDM_SEL_GDMA4){
        txreg = GDMA4_TXCHN_EN;
        rxreg = GDMA4_RXCHN_EN;
        reg = GDMA4_CHN_RLS;
        reg2= GDMA4_RX_CHN_VLD;
		if(HWF_QDMA_SEL_SUPPORT&&FeGetHwfQdmaSelGdm4())
			reg3 = QDMA2_CHN_VLD_BASE;
		else
			reg3 = QDMA1_CHN_VLD_BASE;
    }else{
		return -1;
	}
	reg3 += (chn>>2)<<2;
	txchn = read_reg_word(txreg);
	rxchn = read_reg_word(rxreg);
	write_reg_word(rxreg, 1<<chn);
	write_reg_word(txreg, 1<<chn);
        
	chn &= 0x1f;
	value = (chn << GDMA_CHN_RLS_CHN_OFFSET) | (1 << GDMA_CHN_RLS_EN_OFFSET);
	write_reg_word(reg, value);
	
retry:
	
	mdelay(1);
    
	rlsCnt++;

	if (rlsCnt < GDMA_CHN_RLS_TIMEOUT)
	{
		for(value = 0; value < 2; value++)
		{
			if ( ((read_reg_word(reg) & (1 << GDMA_CHN_RLS_STAT_OFFSET)) == 0)
				|| ((read_reg_word(reg2) & (1 << chn)) != 0)
				|| ((read_reg_word(reg3) & (0xFF<<((chn&0x3)<<3))) != 0) ) {
				goto retry;
			}
		}
	} else {
		ret = -1;
	}
    
	write_reg_word(reg, 0);
	write_reg_word(txreg, txchn);
	write_reg_word(rxreg, rxchn); 
#else
	if (idx == FE_GDM_SEL_GDMA1){
		reg = GDMA1_CHN_RLS;
		reg2 = GDMA1_TX_CHN_VLD;
	}else if(idx == FE_GDM_SEL_GDMA2){
		reg = GDMA2_CHN_RLS;
		reg2= GDMA2_TX_CHN_VLD;
	}else{
		return -1;
	}	

	chn &= 0x1f;
	
	value = (chn << GDMA_CHN_RLS_CHN_OFFSET) | (1 << GDMA_CHN_RLS_EN_OFFSET);
	
	write_reg_word(reg,value);
	
retry:
	
	mdelay(1);
	
	rlsCnt++;

	if (rlsCnt < GDMA_CHN_RLS_TIMEOUT)
	{
		for(value = 0; value < 10; value++)
		{
			if ( (read_reg_word(reg) & (1 << GDMA_CHN_RLS_STAT_OFFSET)) == 0
				||(read_reg_word(reg2) & (1 << chn)) != 0 ) {
				goto retry;
			}
		}
	}else{
		ret = -1;			
	}

	write_reg_word(reg,0);
#endif

	return ret;
}

static int fe_channel_retire(struct ecnt_fe_data *fe_data)
{
	unsigned int txreg=0, rxreg=0, txchn=0, rxchn=0;
	unsigned int hwreg=0, hw=0, i=0, j=0;
	QDMA_TxBufCtrl_T oldtxbuff,newTxbuff;
	unsigned int reg=0, reg2=0, qdmaChnlEn[8];
	int ret=0;

	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	uint channel = fe_data->channel;

#ifdef TCSUPPORT_CPU_EN7580
    if (gdm_sel == FE_GDM_SEL_GDMA1){
        hwreg = QDMA1_CHN_EN_BASE;
        reg = GDMA1_CHN_RLS;
        reg2 = GDMA1_RX_CHN_VLD;
    }else if(gdm_sel == FE_GDM_SEL_GDMA2){
        hwreg = QDMA2_CHN_EN_BASE;
        reg = GDMA2_CHN_RLS;
        reg2= GDMA2_RX_CHN_VLD;
	}else if(gdm_sel == FE_GDM_SEL_GDMA3){
		if(HWF_QDMA_SEL_SUPPORT&&FeGetHwfQdmaSelGdm3())
			hwreg = QDMA2_CHN_EN_BASE;
		else
			hwreg = QDMA1_CHN_EN_BASE;
        reg = GDMA3_CHN_RLS;
        reg2= GDMA3_RX_CHN_VLD;
	}else if(gdm_sel == FE_GDM_SEL_GDMA4){
		if(HWF_QDMA_SEL_SUPPORT&&FeGetHwfQdmaSelGdm4())
			hwreg = QDMA2_CHN_EN_BASE;
		else
			hwreg = QDMA1_CHN_EN_BASE;
        reg = GDMA4_CHN_RLS;
        reg2= GDMA4_RX_CHN_VLD;
    }else{
        return -1;
    }
    
    for(i=0; i<8; i++) {
        qdmaChnlEn[i] = read_reg_word(hwreg+(i<<2));
        write_reg_word(hwreg+(i<<2), 0);
    }
    
    mbi_hang_unlock_by_terminate(gdm_sel);

    mdelay(1);

    for(i=0; i<channel; i++) {
        ret = fe_channel_retire_one(gdm_sel,i);
        if(ret == -1) {
            printk("fe_channel_retire_one(%d/%d): timeout (%08lx) ,(%08lx) \n", i, gdm_sel, read_reg_word(reg), read_reg_word(reg2));
        }
    }
    
    for(j=0; j<10; j++) {
        if (read_reg_word(reg2) == 0) {
            break;
        }
    }
    if(j==10) {
        printk("rx channel vld is error: rx_chnl_vld:%08lx\n", read_reg_word(reg2));
    }
    
    for(i=0; i<8; i++) {
        write_reg_word(hwreg+(i<<2), qdmaChnlEn[i]);
    }
#else
	if (gdm_sel == FE_GDM_SEL_GDMA1){
		txreg = GDMA1_TXCHN_EN;
		rxreg = GDMA1_RXCHN_EN;
		hwreg = CDMA1_HWF_CHN_EN;
		reg = GDMA1_CHN_RLS;
		reg2 = GDMA1_TX_CHN_VLD;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
		txreg = GDMA2_TXCHN_EN;
		rxreg = GDMA2_RXCHN_EN;	
		hwreg = CDMA2_HWF_CHN_EN;	
		reg = GDMA2_CHN_RLS;
		reg2= GDMA2_TX_CHN_VLD;
	}else{
		return -1;
	}	

	txchn = read_reg_word(txreg);
	
	rxchn = read_reg_word(rxreg);
	
	hw = read_reg_word(hwreg);
	
	QDMA_API_GET_TXBUF_THRESHOLD(ECNT_QDMA_WAN,&oldtxbuff);

	write_reg_word(hwreg,0);

	newTxbuff.mode = QDMA_ENABLE;
	newTxbuff.chnThreshold = 1;
	newTxbuff.totalThreshold = 0x40;
	QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_WAN,&newTxbuff);

	write_reg_word(rxreg,0);
	
	write_reg_word(txreg,0);
	
	mdelay(1);

	j = 0;
	while(j < 2)
	{
		for(i = 0; i < channel; i++)
		{
			ret = fe_channel_retire_one(gdm_sel,i);
			if(ret == -1 && j == 1) {
				printk("fe_channel_retire_one(%d/%d): timeout (%08x) ,(%08x) ,(%08x) \n",i,gdm_sel,read_reg_word(reg),read_reg_word(reg2),read_reg_word(FE_PROBE_H));
			}
		}
		j++;
	}
	
	write_reg_word(txreg,txchn);
	
	write_reg_word(rxreg,rxchn);
	
	QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_WAN,&oldtxbuff);

	write_reg_word(hwreg,hw);
#endif

	return 0;
}

static int fe_channel_drop(void)
{
	unsigned int txreg,rxreg,txchn,rxchn;
	unsigned int hwreg,qdmareg,hw,qdma;
	unsigned int gdma_lpbk_cfg_reg, gdma_fwd_cfg_reg ,qdma_glb_cfg_reg,gdm2_tx_cha_vld_reg;
	unsigned int gdma_lpbk_cfg,gdma_fwd_cfg,qdma_glb_cfg;
	QDMA_TxBufCtrl_T oldtxbuff,newTxbuff;
	unsigned int reg, reg2;
	int ret;
	unsigned int value = 0;
	FE_Gdma_Sel_t gdm_sel = FE_GDM_SEL_GDMA2;
		
	gdma_lpbk_cfg_reg	= GDMA2_LPBP_CFG;
	gdma_fwd_cfg_reg	= GDMA2_FWD_CFG;
	qdma_glb_cfg_reg	= 0xbfb55004;
	gdm2_tx_cha_vld_reg = GDMA2_TX_CHN_VLD;
	
	txreg = GDMA2_TXCHN_EN;
	rxreg = GDMA2_RXCHN_EN; 
	hwreg = CDMA2_HWF_CHN_EN;	
	reg = GDMA2_CHN_RLS;
	reg2= GDMA2_TX_CHN_VLD;
	
	txchn = read_reg_word(txreg);
	rxchn = read_reg_word(rxreg);
	hw = read_reg_word(hwreg);	

	#ifdef TCSUPPORT_CPU_EN7580
	mbi_hang_unlock_by_terminate(gdm_sel);
	#endif
	
	/*************diable Cdm2/Gdm2 rx and enable Gdm2 tx*********/	  
	write_reg_word(txreg,0xffff);
	write_reg_word(rxreg,0);
	write_reg_word(hwreg,0);

	/****************set gdm2 loop back*****************/
	gdma_lpbk_cfg = read_reg_word(gdma_lpbk_cfg_reg);
	gdma_fwd_cfg = read_reg_word(gdma_fwd_cfg_reg);
	qdma_glb_cfg = read_reg_word(qdma_glb_cfg_reg);
	
	write_reg_word(gdma_lpbk_cfg_reg,0x4007d003);
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	write_reg_word(gdma_fwd_cfg_reg,0x03f1ffff);
#else
    write_reg_word(gdma_fwd_cfg_reg,0x03f17777);
#endif
	write_reg_word(qdma_glb_cfg_reg,qdma_glb_cfg | 1<<17);
		
	
	while((read_reg_word(gdm2_tx_cha_vld_reg) != 0) && (value++<300)){
		mdelay(1);
	}
			
	write_reg_word(gdma_lpbk_cfg_reg,gdma_lpbk_cfg);
	write_reg_word(gdma_fwd_cfg_reg,gdma_fwd_cfg);	
	write_reg_word(qdma_glb_cfg_reg,qdma_glb_cfg);

	write_reg_word(txreg,txchn);
	write_reg_word(rxreg,rxchn);
	write_reg_word(hwreg,hw);
	
	return 0;

}

/* Check occurrence of FE HW bug, in case of which channel retire can not be performed */
static int fe_api_pse_iq_abnormal(void)
{
    uint32 pse_iq_cnt = 0, pse_iq_stat1 = 0, pse_iq_stat2 = 0;
   
    pse_iq_stat1 = read_reg_word(0xbfb50110);
    pse_iq_stat2 = read_reg_word(0xbfb50114);
    pse_iq_cnt = (pse_iq_stat1 & 0xff) + ((pse_iq_stat1 >> 8) & 0xff) + ((pse_iq_stat1 >> 16) & 0xff) + ((pse_iq_stat2 >> 8) & 0xff);

    if (pse_iq_cnt > 0xA0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
    used when wan link is down,and called by Wan MAC Driver. 
    if channel_retire is 1, this function will retire each channel twice by two loops. First loop retire the  channels queued in PSE buffer,
    these channels will be retired done at first loop. Second loop will given the chance to those channels not queued in PSE buffer but
    queued in QDMA TX queue. All Channels will be retired done at second loop;
    esle, this function will do channel drop. the packet queued in qdma will be sent to gdma2, and set the force port as drop.
    the default set channel_retire is 0, and this function will call channel drop
*/
int fe_api_set_channel_retire_all(struct ecnt_fe_data *fe_data)
{
    if (isEN751221 && fe_api_pse_iq_abnormal())
    {
        printk("Info: fe_api_set_channel_retire_all ingored due to PSE IQ resource abnormal\n");
        return 0;
    }

	atomic_set(&qdma_stop_flag, 1);
	if(channel_retire != CHANNEL_RETIRE)
		fe_channel_drop();
	else 
		fe_channel_retire(fe_data);
	atomic_set(&qdma_stop_flag, 0);

	return 0;
}

int fe_api_set_channel_retire_one(struct ecnt_fe_data *fe_data)
{
    unsigned int hwreg=0, hw=0;
	QDMA_TxBufCtrl_T oldtxbuff,newTxbuff;
	
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	uint channel = fe_data->channel;

    if (isEN751221 && fe_api_pse_iq_abnormal())
    {
        printk("Info: fe_api_set_channel_retire_one ingored due to PSE IQ resource abnormal\n");
        return 0;
    }

#ifdef TCSUPPORT_CPU_EN7580
    if (gdm_sel == FE_GDM_SEL_GDMA1){
        hwreg = QDMA1_CHN_EN_BASE;
    }else if(gdm_sel == FE_GDM_SEL_GDMA2){
        hwreg = QDMA2_CHN_EN_BASE;		
	}else if(gdm_sel == FE_GDM_SEL_GDMA3){
		if(HWF_QDMA_SEL_SUPPORT&&FeGetHwfQdmaSelGdm3())
			hwreg = QDMA2_CHN_EN_BASE;
		else
			hwreg = QDMA1_CHN_EN_BASE;
	}else if(gdm_sel == FE_GDM_SEL_GDMA4){
		if(HWF_QDMA_SEL_SUPPORT&&FeGetHwfQdmaSelGdm4())
			hwreg = QDMA2_CHN_EN_BASE;
		else
			hwreg = QDMA1_CHN_EN_BASE;
    }else{
        return -1;
    }

    mbi_hang_unlock_by_terminate(gdm_sel);
#if 0	
    // disable qdma queue enable for single channel
    hwreg += (channel>>2)<<2;
	hw = read_reg_word(hwreg);
	write_reg_word(hwreg, hw & (~(0xFF<<((channel&0x3)<<3))) );
    
    mdelay(1);
#endif    
    fe_channel_retire_one(gdm_sel, channel);
#if 0    
    write_reg_word(hwreg, hw);
#endif
#else
	if (gdm_sel == FE_GDM_SEL_GDMA1){
		hwreg = CDMA1_HWF_CHN_EN;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
		hwreg = CDMA2_HWF_CHN_EN;		
	}else{
		return -1;
	}
	
	hw = read_reg_word(hwreg);
	
	QDMA_API_GET_TXBUF_THRESHOLD(ECNT_QDMA_WAN,&oldtxbuff);	
	write_reg_word(hwreg,hw & (~(1<<channel)) );
	
	newTxbuff.mode = QDMA_ENABLE;
	newTxbuff.chnThreshold = 1;
	newTxbuff.totalThreshold = 0x40;
	QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_WAN,&newTxbuff);

	mdelay(1);	

	fe_channel_retire_one(gdm_sel,channel);
	
	QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_WAN,&oldtxbuff);

	write_reg_word(hwreg,hw);
#endif

	return 0;
}

int fe_api_set_channel_retire(struct ecnt_fe_data *fe_data)
{
	unsigned int reg,reg2,value,mask,ret = 0;
	unsigned int txreg,rxreg,txchn,rxchn;
	FE_Enable_t rxHwfwd;
	struct ecnt_fe_data cdm_fe_data;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	uint channel = fe_data->channel;
	uint mode = fe_data->reg_val;

    if (isEN751221 && fe_api_pse_iq_abnormal())
    {
        printk("Info: fe_api_set_channel_retire ingored due to PSE IQ resource abnormal\n");
        return 0;
    }

	if (gdm_sel == FE_GDM_SEL_GDMA1){
		reg = GDMA1_CHN_RLS;
		reg2 = GDMA1_TX_CHN_VLD;
		txreg = GDMA1_TXCHN_EN;
		rxreg = GDMA1_RXCHN_EN;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
		reg = GDMA2_CHN_RLS;
		reg2= GDMA2_TX_CHN_VLD;
		txreg = GDMA2_TXCHN_EN;
		rxreg = GDMA2_RXCHN_EN;		
	}else{
		return -1;
	}	
	
	mask = read_reg_word(reg2);

	if ( ((1<< channel)& mask) == 0)
		return 0;
	
	if (gdm_sel == FE_GDM_SEL_GDMA1) {
		cdm_fe_data.cdm_sel = FE_CDM_SEL_CDMA1;
		cdm_fe_data.channel = channel;
		
		fe_get_hwfwd_channel(&cdm_fe_data);
		rxHwfwd = cdm_fe_data.api_data.enable;

		cdm_fe_data.api_data.enable = FE_DISABLE;
		fe_set_hwfwd_channel(&cdm_fe_data);
	}
	else {
		cdm_fe_data.cdm_sel = FE_CDM_SEL_CDMA2;
		cdm_fe_data.channel = channel;
		
		fe_get_hwfwd_channel(&cdm_fe_data);
		rxHwfwd = cdm_fe_data.api_data.enable;
		
		cdm_fe_data.api_data.enable = FE_DISABLE;
		fe_set_hwfwd_channel(&cdm_fe_data);
	}

	if (mode == FE_LINKDOWN){
		txchn = read_reg_word(txreg);
		rxchn = read_reg_word(rxreg);
		write_reg_word(txreg,0);
		write_reg_word(rxreg,0);
	}

	value = (channel << GDMA_CHN_RLS_CHN_OFFSET) | (1 << GDMA_CHN_RLS_EN_OFFSET);
	
	write_reg_word(reg,value);

	mdelay(1);
	
	value = 0;
		
	while( ( (read_reg_word(reg) & (1 << GDMA_CHN_RLS_STAT_OFFSET)) == 0 
				||  (read_reg_word(reg2) & (1 << channel)) != 0 )
				&& (value++ < GDMA_CHN_RLS_TIMEOUT)){
		mdelay(1);
	}
	
	if (value >= GDMA_CHN_RLS_TIMEOUT){
		printk("fe_api_set_channel_retire: timeout \n");
		ret = -1;
	}

	write_reg_word(reg,0);
	
	if (mode == FE_LINKDOWN){
		write_reg_word(txreg,txchn);
		write_reg_word(rxreg,rxchn);
	}
	
	if (gdm_sel == FE_GDM_SEL_GDMA1) {
		cdm_fe_data.cdm_sel = FE_CDM_SEL_CDMA1;
		cdm_fe_data.channel = channel;
		cdm_fe_data.api_data.enable = rxHwfwd;
		fe_set_hwfwd_channel(&cdm_fe_data);
	} else {
		cdm_fe_data.cdm_sel = FE_CDM_SEL_CDMA2;
		cdm_fe_data.channel = channel;
		cdm_fe_data.api_data.enable = rxHwfwd;
		fe_set_hwfwd_channel(&cdm_fe_data);
	}
	
	return ret;

}

int fe_api_set_crc_strip(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_Enable_t enable = fe_data->api_data.enable;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
		base_addr = GDMA2_FWD_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA3){
		base_addr = GDMA3_FWD_CFG;
	}else{
		base_addr = GDMA4_FWD_CFG;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<16);
	}else{
		val |= (1<<16);
	}
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_padding(struct ecnt_fe_data *fe_data)
{
    unsigned long int addr, val;
    FE_Enable_t enable = fe_data->api_data.enable;
    
    addr = FE_CPORT_CFG;
    val = read_reg_word(addr);
    if(enable == FE_DISABLE){
        val &= ~(1<<26);
    }else{
        val |= (1<<26);
    }
    write_reg_word(addr, val);
    return 0;
}

int fe_api_get_ext_tpid(struct ecnt_fe_data *fe_data)
{
    fe_data->reg_val = (read_reg_word(FE_DMA_GLO_CFG)>>16) & 0xffff;
    return 0;
}

int fe_api_set_ext_tpid(struct ecnt_fe_data *fe_data)
{
	unsigned long int addr, val;
	uint tpid = fe_data->reg_val;
	
    addr = FE_DMA_GLO_CFG;
    val = read_reg_word(addr);
    
    val &= ~(0xffff<<16);
    val |= (tpid<<16);
    write_reg_word(addr, val);
    return 0;
}

int fe_api_get_fw_cfg(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;

	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
		base_addr = GDMA2_FWD_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA3){
		base_addr = GDMA3_FWD_CFG;
	}else{
		base_addr = GDMA4_FWD_CFG;
	}

	fe_data->reg_val = read_reg_word(base_addr);
	return 0;
}

int fe_api_set_fw_cfg(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr=0, offset=0, mask=0, val=0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_Frame_type_t dp_sel = fe_data->api_data.fw_cfg.dp_sel;
	FE_Frame_dp_t dp_type = fe_data->api_data.fw_cfg.dp_val;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
		base_addr = GDMA2_FWD_CFG;
	}else if(gdm_sel == FE_GDM_SEL_GDMA3){
		base_addr = GDMA3_FWD_CFG;
	}else{
		base_addr = GDMA4_FWD_CFG;
	}
	if(dp_sel == FE_TYPE_OC){
		offset = GDM_OFRC_P_SHIFT;
		mask = GDM_OFRC_P;
	}else if(dp_sel == FE_TYPE_MC){
		offset = GDM_MFRC_P_SHIFT;
		mask = GDM_MFRC_P;
	}else if(dp_sel == FE_TYPE_BC){
		offset = GDM_BFRC_P_SHIFT;
		mask = GDM_BFRC_P;
	}else if(dp_sel == FE_TYPE_UC){
		offset = GDM_UFRC_P_SHIFT;
		mask = GDM_UFRC_P;
	}
	val = read_reg_word(base_addr);
	val &= ~(mask);
	val |= (dp_type << offset);

	write_reg_word(base_addr, val);
	
	return 0;
}

int fe_api_set_drop_udp_chksum_err_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;	
	FE_Enable_t enable = fe_data->api_data.enable;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else{
		base_addr = GDMA2_FWD_CFG;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<20);
	}else{
		val |= (1<<20);
	}
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_drop_tcp_chksum_err_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;	
	FE_Enable_t enable = fe_data->api_data.enable;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else{
		base_addr = GDMA2_FWD_CFG;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<21);
	}else{
		val |= (1<<21);
	}
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_drop_ip_chksum_err_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;	
	FE_Enable_t enable = fe_data->api_data.enable;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else{
		base_addr = GDMA2_FWD_CFG;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<22);
	}else{
		val |= (1<<22);
	}
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_drop_crc_err_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_Enable_t enable = fe_data->api_data.enable;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else{
		base_addr = GDMA2_FWD_CFG;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<23);
	}else{
		val |= (1<<23);
	}
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_drop_runt_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_Enable_t enable = fe_data->api_data.enable;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else{
		base_addr = GDMA2_FWD_CFG;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<24);
	}else{
		val |= (1<<24);
	}
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_drop_long_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_Enable_t enable = fe_data->api_data.enable;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_FWD_CFG;
	}else{
		base_addr = GDMA2_FWD_CFG;
	}

	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
		val &= ~(1<<25);
	}else{
		val |= (1<<25);
	}
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_vlan_check(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr=0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_Enable_t enable = fe_data->api_data.enable;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_VLAN_CHECK;
	}else{
		base_addr = GDMA2_VLAN_CHECK;
	}
	
	write_reg_word(base_addr, enable);

	return 0;
}

int fe_api_get_ok_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr=0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_TXRX_Sel_t txrx_sel = fe_data->txrx_sel;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
        if (txrx_sel == FE_GDM_SEL_TX)
		    base_addr = GDMA1_TX_OK_CNT;
        else
            base_addr = GDMA1_RX_OK_CNT;
	}else{
        if (txrx_sel == FE_GDM_SEL_TX)
		    base_addr = GDMA2_TX_OKCNT;
        else
            base_addr = GDMA2_RX_OKCNT;
	}

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_get_rx_err_crc_cnt(struct ecnt_fe_data *fe_data)
{
	fe_data->cnt = read_reg_word(GDMA2_RX_ETHCRCCNT);
	return 0;
}

int fe_api_get_rx_drop_fifo_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_RX_OVER_DROP_CNT;
	}else{
		base_addr = GDMA2_RX_OVDROPCNT;
	}

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_get_rx_drop_err_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_RX_ERROR_DROP_CNT;
	}else{
		base_addr = GDMA2_RX_ERRDROPCNT;
	}

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_get_ok_byte_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr=0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_TXRX_Sel_t txrx_sel = fe_data->txrx_sel;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
        if (txrx_sel == FE_GDM_SEL_TX)
		    base_addr = GDMA1_TX_OK_BYTE_CNT;
        else
            base_addr = GDMA1_RX_BYTECNT;
	}else{
        if (txrx_sel == FE_GDM_SEL_TX)
		    base_addr = GDMA2_TX_OKBYTE_CNT;
        else
            base_addr = GDMA2_RX_OKBYTECNT;
	}

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_get_tx_get_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;

	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_TX_GET_CNT;
	}else{
		base_addr = GDMA2_TX_GETCNT;
	}

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_get_tx_drop_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_TX_DROP_CNT;
	}else{
		base_addr = GDMA2_TX_DROPCNT;
	}

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_get_time_stamp(struct ecnt_fe_data *fe_data)
{
	fe_data->cnt = read_reg_word(FE_FOE_TS) & 0xFFFF;
	return 0;
}

int fe_api_set_time_stamp(struct ecnt_fe_data *fe_data)
{
	unsigned long int val;

	val = read_reg_word(FE_FOE_TS);
	val &= ~(0xFFFF);
	val |= (fe_data->timeStamp&0xFFFF);
	write_reg_word(FE_FOE_TS, val);
	
	return 0;
}
int fe_api_set_ins_vlan_tpid(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Cdma_Sel_t cdm_sel = fe_data->cdm_sel;
	uint tpid = fe_data->reg_val;
	
	if(cdm_sel == FE_CDM_SEL_CDMA1){
		base_addr = CDMA1_VLAN_CTRL;
	}else{
		base_addr = CDMA2_VLAN_CTRL;
	}

	val = read_reg_word(base_addr);
    val &= ~(INS_VLAN);
    val |= (tpid<<INS_VLAN_SHIFT);
    write_reg_word(base_addr, val);
   
	return 0;
}

int fe_api_set_vlan_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val;
	FE_Cdma_Sel_t cdm_sel = fe_data->cdm_sel;
	FE_Enable_t enable = fe_data->api_data.enable;
	
	if(cdm_sel == FE_CDM_SEL_CDMA1){
		base_addr = CDMA1_VLAN_CTRL;
	}else{
		base_addr = CDMA2_VLAN_CTRL;
	}
	
	val = read_reg_word(base_addr);
	if(enable == FE_DISABLE){
        val &= ~(1<<1);
    }else{
        val |= (1<<1);
    }
	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_set_black_list(struct ecnt_fe_data *fe_data)
{
	FE_Enable_t enable = fe_data->api_data.enable;
	write_reg_word(L2BR_CFG, enable);
	return 0;
}

int fe_api_set_ether_type(struct ecnt_fe_data *fe_data)
{
	unsigned long int val, type;
	uint index = fe_data->index;
	FE_Enable_t enable = fe_data->api_data.eth_cfg.enable;
	FE_PPPOE_t is_pppoe = fe_data->api_data.eth_cfg.is_pppoe;
	uint value = fe_data->api_data.eth_cfg.value;
	
	if(index > 15){
		printk("Error index %d, should be 0~15!\n", index);
		return 1;
	}
	val = read_reg_word(L2BR_ETYPE_EN);
	if(enable == FE_DISABLE){
		if(is_pppoe)	
			val &= ~(1<<(index+16));
		
			val &= ~(1<<index);
	}else{
		if(is_pppoe)
			val |= (1<<(index+16));

			val |= (1<<index);
	}

	write_reg_word(L2BR_ETYPE_EN, val);
	
	type = read_reg_word(L2BR_ETYPE_N(index));
	type &= ~(0xffff << (16*(index%2)));
	type |= value << (16*(index%2));
	write_reg_word(L2BR_ETYPE_N(index), type);
	return 0;
}

int fe_api_set_L2U_key(struct ecnt_fe_data *fe_data)
{
	/* TO DO */
	return 0;
}

int fe_api_get_ac_group_pkt_cnt(struct ecnt_fe_data *fe_data)
{
	uint index = fe_data->index;
	uint flowid = index - UP_QUEUE_CNT_START;
	uint softPkts = 0, softByte_L=0, softByte_H=0;
    
	ac_packet_cnt[index] += read_reg_word(PPE_AC_PCNT(index));
	fe_data->cnt = ac_packet_cnt[index];
    
	if(index < UP_DOWN_WAN_ITF_NUM) {
		fe_get_wan_itf_cnt(index/2, index%2, &softByte_L, &softByte_H, &softPkts);
		fe_data->cnt += softPkts;
	} else if((index >= UP_QUEUE_CNT_START) && (flowid < UP_DOWN_QUEUE_NUM)){
		PPE_API_GET_SOFT_PKTS(flowid, &softPkts);
		fe_data->cnt += softPkts;
	} 
    
	return 0;
}

int fe_api_get_ac_group_byte_cnt(struct ecnt_fe_data *fe_data)
{
	uint index = fe_data->index;
	uint flowid = index - UP_QUEUE_CNT_START;
	uint softCnt = 0;
	uint softPkts = 0, softByte_L=0, softByte_H=0;
	uint pre_byte_cnt_lo = 0;
	
	pre_byte_cnt_lo = ac_byte_cnt_lo[index];
	ac_byte_cnt_lo[index] += read_reg_word(PPE_AC_BCNT_L(index));
	ac_byte_cnt_hi[index] += read_reg_word(PPE_AC_BCNT_H(index));
	if(ac_byte_cnt_lo[index] < pre_byte_cnt_lo)
		ac_byte_cnt_hi[index] += 1;
    
	fe_data->cnt = ac_byte_cnt_lo[index];
	fe_data->cnt_hi = ac_byte_cnt_hi[index];

	if(flowid < FLOW_NUMBER){
		flowByteCnt_HW[flowid] += fe_data->cnt;
		PPE_API_GET_SOFT_CNT(flowid, &softCnt);
		fe_data->api_data.byteCnt.byteCnt_L = flowByteCnt_HW[flowid] + softCnt;
	}

	if(index < UP_DOWN_WAN_ITF_NUM) {
		fe_get_wan_itf_cnt(index/2, index%2, &softByte_L, &softByte_H, &softPkts);
		fe_data->cnt += softByte_L;
		if(fe_data->cnt < softByte_L)
			fe_data->cnt_hi++;
		fe_data->cnt_hi += softByte_H;
	} else if((index >= UP_QUEUE_CNT_START) && (flowid < UP_DOWN_QUEUE_NUM)){
		PPE_API_GET_SOFT_CNT(flowid, &softCnt);
		fe_data->cnt += softCnt;
		if(fe_data->cnt < softCnt)
			fe_data->cnt_hi++;
	}

	return 0;
}

int fe_api_clear_ac_group_pkt_cnt(struct ecnt_fe_data *fe_data)
{
	uint index = fe_data->index;
	int i=0;
	if(index == FE_AC_MTR_GROUP_MAX) {
		for(i=0; i<FE_AC_MTR_GROUP_MAX; i++) {
			ac_packet_cnt[i] = 0;
		}
		for(i=0; i<UP_DOWN_WAN_ITF_NUM; i++) {
			fe_clear_wan_itf_cnt(i/2, i%2);
		}
	} else {
		ac_packet_cnt[index] = 0;
	}
    
	return 0;
}

int fe_api_clear_ac_group_byte_cnt(struct ecnt_fe_data *fe_data)
{
	uint index = fe_data->index;
	int i=0;
    
	if(index == FE_AC_MTR_GROUP_MAX) {
		for(i=0; i<FE_AC_MTR_GROUP_MAX; i++) {
			ac_byte_cnt_lo[i] = 0;
			ac_byte_cnt_hi[i] = 0;
		}
		for(i=0; i<UP_DOWN_WAN_ITF_NUM; i++) {
			fe_clear_wan_itf_cnt(i/2, i%2);
		}
	} else {
		ac_byte_cnt_lo[index] = 0;
		ac_byte_cnt_hi[index] = 0;
	}
    
	return 0;
}

int fe_api_set_meter_group(struct ecnt_fe_data *fe_data)
{
	/* keep the forwards and backwards compatibility for FH API fe_set_flow_ratelimit, so restore the implementation */
	uint tick,maxBkSize;
	uint index = fe_data->index;
	uint Rate = fe_data->api_data.meter_rate;
	
	if (Rate <= METER_SCALE_M) /* 0 ~ 1Mbps */
	{
		tick = METER_TICK_BYTE_1MS;
		Rate /= (METER_SCALE_K<<4); /* unit: 16kbps */
		maxBkSize = METER_BUCKET_4KB;
	}
	else 
	{
		tick = METER_TICK_BYTE_125US;
		Rate /= (METER_SCALE_K<<7); /* unit: 128kbps */

		if (METER_SCALE_M < Rate && Rate <= (METER_SCALE_M<<7)) /* 1M ~ 128M bps */
		{
			maxBkSize = METER_BUCKET_4KB;
		}
		else if ((METER_SCALE_M<<7) < Rate && Rate <= (METER_SCALE_M<<9)) /* 128M ~ 512M bps */
		{
			maxBkSize = METER_BUCKET_16KB;
		}
		else if ((METER_SCALE_M<<9) < Rate && Rate <= METER_SCALE_G) /* 512M ~ 1G bps */
		{
			maxBkSize = METER_BUCKET_32KB;
		}
		else
		{
			maxBkSize = METER_BUCKET_64KB;
		}
	}

	if (Rate > METER_TOKEN_RATE_BYTE_MAX)
	{
		Rate = METER_TOKEN_RATE_BYTE_MAX;
	}
	
	#ifdef TCSUPPORT_XPON_HAL_API_EXT
	return PPE_API_SET_METER_GROUP(index,maxBkSize,Rate,tick,0,PPE_METER_BYTE_MODE);
	#else
	return 0;
	#endif
}

int fe_api_get_meter_group(struct ecnt_fe_data *fe_data)
{
	/* keep the forwards and backwards compatibility for FH API fe_get_flow_ratelimit, so restore the implementation */
	uint index = fe_data->index;
	uint tick,trtcmEn,maxBkSize,mode;
	
	#ifdef TCSUPPORT_XPON_HAL_API_EXT
	return PPE_API_GET_METER_GROUP(index,&maxBkSize,&fe_data->api_data.meter_rate,&tick,&trtcmEn,&mode);
	#else
	return 0;
	#endif
}

int fe_api_set_gdm_pcp_coding(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, offset, mask, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_TXRX_Sel_t txrx_sel = fe_data->txrx_sel;
	FE_PcpMode_t mode = fe_data->api_data.coding_mode;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1)
		base_addr = GDMA1_LAN_PCP;
	else if(gdm_sel == FE_GDM_SEL_GDMA2)
		base_addr = GDMA2_WAN_PCP;
	else if(gdm_sel == FE_GDM_SEL_GDMA3)
		base_addr = GDMA3_WAN_PCP;
	else if((gdm_sel == FE_GDM_SEL_GDMA4) && GDM4_SUPPORT)
		base_addr = GDMA4_LAN_PCP;
	else
		return 0;
		
	if(txrx_sel == FE_GDM_SEL_TX){
		offset = GDM_TX_ENCODING_SHIFT;
		mask = GDM_TX_ENCODING_MASK;
	}else{
		offset = GDM_RX_DECODING_SHIFT;
		mask = GDM_RX_DECODING_MASK;
	}
	
	val = read_reg_word(base_addr);
	val &= ~(mask);
	val |= (mode << offset);
	
	write_reg_word(base_addr, val);
	return 0;
}

int fe_api_set_cdm_pcp_coding(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, offset, mask, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_TXRX_Sel_t txrx_sel = fe_data->txrx_sel;
	FE_PcpMode_t mode = fe_data->api_data.coding_mode;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1)
		base_addr = GDMA1_LAN_PCP;
	else
		base_addr = GDMA2_WAN_PCP;
		
	if(txrx_sel == FE_GDM_SEL_TX){
		offset = CDM_TX_ENCODING_SHIFT;
		mask = CDM_TX_ENCODING_MASK;
	}else{
		offset = CDM_RX_DECODING_SHIFT;
		mask = CDM_RX_DECODING_MASK;
	}
	val = read_reg_word(base_addr);
	val &= ~(mask);
	val |= (mode << offset);
	
	write_reg_word(base_addr, val);
	return 0;
}


int fe_api_set_vip_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int val, type;
	uint index = fe_data->index;
	FE_Enable_t enable = fe_data->api_data.vip_cfg.enable;
	FE_Patn_type patten_type = fe_data->api_data.vip_cfg.patten_type;
	uint patten = fe_data->api_data.vip_cfg.patten;
	
	if(index > 31){
		printk("Error index %d, should be 0~31!\n", index);
		return 1;
	}
	
	type = 0;
	val = 0;
	
	if(enable == FE_ENABLE){
		type |= ((0x01<<VIP_EN_CPU_OFFSET) | (0x01<<VIP_EN_ENABLE_OFFSET) | patten_type<<VIP_EN_TYPE_OFFSET);
		val = patten;
	}
	
	write_reg_word(FE_VIP_EN(index), type);
	write_reg_word(FE_VIP_PATN(index), val);

	return 0;
}

int fe_api_get_eth_rx_cnt(struct ecnt_fe_data *fe_data)
{
	if (GDM2_RX_ETH_MIB_SUPPORT)
	{
		unsigned long long gdm2_rx_high = 0;
		gdm2_rx_high = read_reg_word(GDMA2_RX_OKCNT_H);
		fe_data->api_data.FE_RxCnt.rxOKPktCnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_OKCNT));
		fe_data->api_data.FE_RxCnt.rxFCDropCnt = read_reg_word(GDMA2_RX_FCDROPCNT);
		fe_data->api_data.FE_RxCnt.rxRCDropCnt = read_reg_word(GDMA2_RX_RCDROPCNT);
		fe_data->api_data.FE_RxCnt.rxOVDropCnt = read_reg_word(GDMA2_RX_OVDROPCNT);
		fe_data->api_data.FE_RxCnt.rxERRDropCnt= read_reg_word(GDMA2_RX_ERRDROPCNT);

		gdm2_rx_high = read_reg_word(GDMA2_RX_OKBYTECNT_H);
		fe_data->api_data.FE_RxCnt.rxOKByteCnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_OKBYTECNT));
		fe_data->api_data.FE_RxCnt.rxOversizeCnt = read_reg_word(GDMA2_RX_ETHLONGCNT);
		fe_data->api_data.FE_RxCnt.rxUnderSizeCnt = read_reg_word(GDMA2_RX_ETHRUNTCNT);

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETHERPCNT_H);
		fe_data->api_data.FE_RxCnt.rxFrameCnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETHERPCNT));

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETHERPLEN_H);
		fe_data->api_data.FE_RxCnt.rxFrameLen = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETHERPLEN));
		fe_data->api_data.FE_RxCnt.rxDropCnt = (gdm2_rx_drop_high << 32) + read_reg_word(GDMA2_RX_ETHDROPCNT);
		fe_data->api_data.FE_RxCnt.rxBroadcastCnt = read_reg_word(GDMA2_RX_ETHBCCNT);
		fe_data->api_data.FE_RxCnt.rxMulticastCnt = read_reg_word(GDMA2_RX_ETHMCCNT);
		fe_data->api_data.FE_RxCnt.rxCrcCnt = read_reg_word(GDMA2_RX_ETHCRCCNT);
		fe_data->api_data.FE_RxCnt.rxFragFameCnt = read_reg_word(GDMA2_RX_ETHFRACCNT);
		fe_data->api_data.FE_RxCnt.rxJabberFameCnt = read_reg_word(GDMA2_RX_ETHJABCNT);
		fe_data->api_data.FE_RxCnt.rxLess64Cnt = read_reg_word(GDMA2_RX_ETHRUNTCNT);
		fe_data->api_data.FE_RxCnt.rxMore1518Cnt = read_reg_word(GDMA2_RX_ETHLONGCNT);

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETH_64_CNT_H);
		fe_data->api_data.FE_RxCnt.rxEq64Cnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETH_64_CNT));

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETH_65_TO_127_CNT_H);
		fe_data->api_data.FE_RxCnt.rxFrom65To127Cnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETH_65_TO_127_CNT));

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETH_128_TO_255_CNT_H);
		fe_data->api_data.FE_RxCnt.rxFrom128To255Cnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETH_128_TO_255_CNT));

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETH_256_TO_511_CNT_H);
		fe_data->api_data.FE_RxCnt.rxFrom256To511Cnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETH_256_TO_511_CNT));

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETH_512_TO_1023_CNT_H);
		fe_data->api_data.FE_RxCnt.rxFrom512To1023Cnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETH_512_TO_1023_CNT));

		gdm2_rx_high = read_reg_word(GDMA2_RX_ETH_1024_TO_1518_CNT_H);
		fe_data->api_data.FE_RxCnt.rxFrom1024To1518Cnt = ((gdm2_rx_high << 32) + read_reg_word(GDMA2_RX_ETH_1024_TO_1518_CNT));
	}
	else
	{
		fe_data->api_data.FE_RxCnt.rxOKPktCnt = read_reg_word(GDMA2_RX_OKCNT);
		fe_data->api_data.FE_RxCnt.rxFCDropCnt = read_reg_word(GDMA2_RX_FCDROPCNT);
		fe_data->api_data.FE_RxCnt.rxRCDropCnt = read_reg_word(GDMA2_RX_RCDROPCNT);
		fe_data->api_data.FE_RxCnt.rxOVDropCnt = read_reg_word(GDMA2_RX_OVDROPCNT);
		fe_data->api_data.FE_RxCnt.rxERRDropCnt= read_reg_word(GDMA2_RX_ERRDROPCNT);
		fe_data->api_data.FE_RxCnt.rxOKByteCnt = read_reg_word(GDMA2_RX_OKBYTECNT);
		fe_data->api_data.FE_RxCnt.rxOversizeCnt = read_reg_word(GDMA2_RX_ETHLONGCNT);
		fe_data->api_data.FE_RxCnt.rxUnderSizeCnt = read_reg_word(GDMA2_RX_ETHRUNTCNT);
		fe_data->api_data.FE_RxCnt.rxFrameCnt = read_reg_word(GDMA2_RX_ETHERPCNT);
		fe_data->api_data.FE_RxCnt.rxFrameLen = (gdm2_rx_len_high<<32) + read_reg_word(GDMA2_RX_ETHERPLEN);
		fe_data->api_data.FE_RxCnt.rxDropCnt = read_reg_word(GDMA2_RX_ETHDROPCNT);
		fe_data->api_data.FE_RxCnt.rxBroadcastCnt = read_reg_word(GDMA2_RX_ETHBCCNT);
		fe_data->api_data.FE_RxCnt.rxMulticastCnt = read_reg_word(GDMA2_RX_ETHMCCNT);
		fe_data->api_data.FE_RxCnt.rxCrcCnt = read_reg_word(GDMA2_RX_ETHCRCCNT);
		fe_data->api_data.FE_RxCnt.rxFragFameCnt = read_reg_word(GDMA2_RX_ETHFRACCNT);
		fe_data->api_data.FE_RxCnt.rxJabberFameCnt = read_reg_word(GDMA2_RX_ETHJABCNT);
		fe_data->api_data.FE_RxCnt.rxLess64Cnt = read_reg_word(GDMA2_RX_ETHRUNTCNT);
		fe_data->api_data.FE_RxCnt.rxMore1518Cnt = read_reg_word(GDMA2_RX_ETHLONGCNT);
		fe_data->api_data.FE_RxCnt.rxEq64Cnt = read_reg_word(GDMA2_RX_ETH_64_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom65To127Cnt = read_reg_word(GDMA2_RX_ETH_65_TO_127_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom128To255Cnt = read_reg_word(GDMA2_RX_ETH_128_TO_255_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom256To511Cnt = read_reg_word(GDMA2_RX_ETH_256_TO_511_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom512To1023Cnt = read_reg_word(GDMA2_RX_ETH_512_TO_1023_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom1024To1518Cnt = read_reg_word(GDMA2_RX_ETH_1024_TO_1518_CNT);
	}

	return 0;
}

int fe_api_get_eth_tx_cnt(struct ecnt_fe_data *fe_data)
{
	if (GDM2_TX_ETH_MIB_SUPPORT)
	{
		unsigned long long gdm2_tx_high = 0;
		gdm2_tx_high = read_reg_word(GDMA2_TX_ETHCNT_H);
		fe_data->api_data.FE_TxCnt.txFrameCnt = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETHCNT));

		gdm2_tx_high = read_reg_word(GDMA2_TX_ETHLENCNT_H);
		fe_data->api_data.FE_TxCnt.txFrameLen = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETHLENCNT));
		fe_data->api_data.FE_TxCnt.txDropCnt =  (gdm2_tx_drop_high << 32) + read_reg_word(GDMA2_TX_ETHDROPCNT);
		fe_data->api_data.FE_TxCnt.txBroadcastCnt = read_reg_word(GDMA2_TX_ETHBCDCNT);
		fe_data->api_data.FE_TxCnt.txMulticastCnt = read_reg_word(GDMA2_TX_ETHMULTICASTCNT);
		fe_data->api_data.FE_TxCnt.txLess64Cnt = read_reg_word(GDMA2_TX_ETH_LESS64_CNT);
		fe_data->api_data.FE_TxCnt.txMore1518Cnt = read_reg_word(GDMA2_TX_ETH_MORE1518_CNT);

		gdm2_tx_high = read_reg_word(GDMA2_TX_ETH_64_CNT_H);
		fe_data->api_data.FE_TxCnt.txEq64Cnt = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETH_64_CNT));

		gdm2_tx_high = read_reg_word(GDMA2_TX_ETH_65_TO_127_CNT_H);
		fe_data->api_data.FE_TxCnt.txFrom65To127Cnt = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETH_65_TO_127_CNT));

		gdm2_tx_high = read_reg_word(GDMA2_TX_ETH_128_TO_255_CNT_H);
		fe_data->api_data.FE_TxCnt.txFrom128To255Cnt = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETH_128_TO_255_CNT));

		gdm2_tx_high = read_reg_word(GDMA2_TX_ETH_256_TO_511_CNT_H);
		fe_data->api_data.FE_TxCnt.txFrom256To511Cnt = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETH_256_TO_511_CNT));

		gdm2_tx_high = read_reg_word(GDMA2_TX_ETH_512_TO_1023_CNT_H);
		fe_data->api_data.FE_TxCnt.txFrom512To1023Cnt = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETH_512_TO_1023_CNT));

		gdm2_tx_high = read_reg_word(GDMA2_TX_ETH_1024_TO_1518_CNT_H);
		fe_data->api_data.FE_TxCnt.txFrom1024To1518Cnt = ((gdm2_tx_high << 32) + read_reg_word(GDMA2_TX_ETH_1024_TO_1518_CNT));
	}
	else
	{
		fe_data->api_data.FE_TxCnt.txFrameCnt = read_reg_word(GDMA2_TX_ETHCNT);
		fe_data->api_data.FE_TxCnt.txFrameLen = (gdm2_tx_len_high<<32) + read_reg_word(GDMA2_TX_ETHLENCNT);
		fe_data->api_data.FE_TxCnt.txDropCnt = read_reg_word(GDMA2_TX_ETHDROPCNT);
		fe_data->api_data.FE_TxCnt.txBroadcastCnt = read_reg_word(GDMA2_TX_ETHBCDCNT);
		fe_data->api_data.FE_TxCnt.txMulticastCnt = read_reg_word(GDMA2_TX_ETHMULTICASTCNT);
		fe_data->api_data.FE_TxCnt.txLess64Cnt = read_reg_word(GDMA2_TX_ETH_LESS64_CNT);
		fe_data->api_data.FE_TxCnt.txMore1518Cnt = read_reg_word(GDMA2_TX_ETH_MORE1518_CNT);
		fe_data->api_data.FE_TxCnt.txEq64Cnt = read_reg_word(GDMA2_TX_ETH_64_CNT);
		fe_data->api_data.FE_TxCnt.txFrom65To127Cnt = read_reg_word(GDMA2_TX_ETH_65_TO_127_CNT);
		fe_data->api_data.FE_TxCnt.txFrom128To255Cnt = read_reg_word(GDMA2_TX_ETH_128_TO_255_CNT);
		fe_data->api_data.FE_TxCnt.txFrom256To511Cnt = read_reg_word(GDMA2_TX_ETH_256_TO_511_CNT);
		fe_data->api_data.FE_TxCnt.txFrom512To1023Cnt = read_reg_word(GDMA2_TX_ETH_512_TO_1023_CNT);
		fe_data->api_data.FE_TxCnt.txFrom1024To1518Cnt = read_reg_word(GDMA2_TX_ETH_1024_TO_1518_CNT);
	}

	return 0;
}


int fe_api_get_hsgmii_rx_cnt(struct ecnt_fe_data *fe_data)
{
	int hsgmii_index = fe_data->index; //0:pcie 0 hsgmii, 1 pcie 1 hsgmii, 2:usb hsgmii in 7523
	//0:pcie 0 hsgmii, 1 pcie 1 hsgmii, 2:usb hsgmii ,3:eth hsgmii, pcie0/pcie1 in gdm3,usb eth in gdm4 in 7581
	unsigned int reg=0;
	unsigned long long hsgmii_rx_high = 0;
	unsigned int hsgmii_rx_low = 0;
	if (isEN7523)
	{
		if(hsgmii_index == 0) {
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (4 << GDM3_TX_MIB_ID_OFFSET) + (0 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );
		}else if(hsgmii_index == 1){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (5 << GDM3_TX_MIB_ID_OFFSET) + (1 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}else if(hsgmii_index == 2){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (6 << GDM3_TX_MIB_ID_OFFSET) + (2 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}
 

		hsgmii_rx_high = read_reg_word(GDMA3_RX_OK_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_OK_CNT);
		fe_data->api_data.FE_RxCnt.rxOKPktCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
		fe_data->api_data.FE_RxCnt.rxFCDropCnt = read_reg_word(GDMA3_RX_FC_DROP_CNT);
		fe_data->api_data.FE_RxCnt.rxRCDropCnt = read_reg_word(GDMA3_RX_RC_DROP_CNT);
		fe_data->api_data.FE_RxCnt.rxOVDropCnt = read_reg_word(GDMA3_RX_OVER_DROP_CNT);
		fe_data->api_data.FE_RxCnt.rxERRDropCnt= read_reg_word(GDMA3_RX_ERROR_DROP_CNT);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_OK_BYTE_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_PKT_CNT);
		fe_data->api_data.FE_RxCnt.rxOKByteCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
		fe_data->api_data.FE_RxCnt.rxOversizeCnt = read_reg_word(GDMA3_RX_ETH_LONG_CNT);
		fe_data->api_data.FE_RxCnt.rxUnderSizeCnt = read_reg_word(GDMA3_RX_ETH_RUNT_CNT);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_PKT_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_PKT_CNT);
		fe_data->api_data.FE_RxCnt.rxFrameCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_BYTE_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_BYTE_CNT);
		fe_data->api_data.FE_RxCnt.rxFrameLen = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
		fe_data->api_data.FE_RxCnt.rxDropCnt = read_reg_word(GDMA3_RX_ETH_DROP_CNT);
		hsgmii_rx_high = read_reg_word(GDMA3_RX_BC_PKT_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_BC_CNT);
		fe_data->api_data.FE_RxCnt.rxBroadcastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
		hsgmii_rx_high = read_reg_word(GDMA3_RX_MC_PKT_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_MC_CNT);
		fe_data->api_data.FE_RxCnt.rxMulticastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
		
		fe_data->api_data.FE_RxCnt.rxCrcCnt = read_reg_word(GDMA3_RX_ETH_CRCE_CNT);
		fe_data->api_data.FE_RxCnt.rxFragFameCnt = read_reg_word(GDMA3_RX_ETH_FRAG_CNT);
		fe_data->api_data.FE_RxCnt.rxJabberFameCnt = read_reg_word(GDMA3_RX_ETH_JABBER_CNT);
		fe_data->api_data.FE_RxCnt.rxLess64Cnt = read_reg_word(GDMA3_RX_ETH_RUNT_CNT);
		fe_data->api_data.FE_RxCnt.rxMore1518Cnt = read_reg_word(GDMA3_RX_ETH_LONG_CNT);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_E64_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_E64_CNT);
		fe_data->api_data.FE_RxCnt.rxEq64Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L64_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L64_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom65To127Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L127_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L127_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom128To255Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L255_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L255_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom256To511Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L511_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L511_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom512To1023Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

		hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L1023_CNT_H);
		hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L1023_CNT);
		fe_data->api_data.FE_RxCnt.rxFrom1024To1518Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
	}else if (isEN7581){


		if(hsgmii_index == 0) {
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (4 << GDM3_TX_MIB_ID_OFFSET) + (0 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );
		}else if(hsgmii_index == 1){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (5 << GDM3_TX_MIB_ID_OFFSET) + (1 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}else if(hsgmii_index == 2){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_ID_OFFSET) + (1 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}else if(hsgmii_index == 3){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM4_TX_MIB_ID_OFFSET) + (0 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}
		if(hsgmii_index == 0 || hsgmii_index == 1){
			hsgmii_rx_high = read_reg_word(GDMA3_RX_OK_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_OK_CNT);
			fe_data->api_data.FE_RxCnt.rxOKPktCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxFCDropCnt = read_reg_word(GDMA3_RX_FC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxRCDropCnt = read_reg_word(GDMA3_RX_RC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxOVDropCnt = read_reg_word(GDMA3_RX_OVER_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxERRDropCnt= read_reg_word(GDMA3_RX_ERROR_DROP_CNT);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_OK_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxOKByteCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxOversizeCnt = read_reg_word(GDMA3_RX_ETH_LONG_CNT);
			fe_data->api_data.FE_RxCnt.rxUnderSizeCnt = read_reg_word(GDMA3_RX_ETH_RUNT_CNT);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_BYTE_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameLen = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxDropCnt = read_reg_word(GDMA3_RX_ETH_DROP_CNT);
			hsgmii_rx_high = read_reg_word(GDMA3_RX_BC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_BC_CNT);
			fe_data->api_data.FE_RxCnt.rxBroadcastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			hsgmii_rx_high = read_reg_word(GDMA3_RX_MC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_MC_CNT);
			fe_data->api_data.FE_RxCnt.rxMulticastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			
			fe_data->api_data.FE_RxCnt.rxCrcCnt = read_reg_word(GDMA3_RX_ETH_CRCE_CNT);
			fe_data->api_data.FE_RxCnt.rxFragFameCnt = read_reg_word(GDMA3_RX_ETH_FRAG_CNT);
			fe_data->api_data.FE_RxCnt.rxJabberFameCnt = read_reg_word(GDMA3_RX_ETH_JABBER_CNT);
			fe_data->api_data.FE_RxCnt.rxLess64Cnt = read_reg_word(GDMA3_RX_ETH_RUNT_CNT);
			fe_data->api_data.FE_RxCnt.rxMore1518Cnt = read_reg_word(GDMA3_RX_ETH_LONG_CNT);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_E64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_E64_CNT);
			fe_data->api_data.FE_RxCnt.rxEq64Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L64_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom65To127Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L127_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L127_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom128To255Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L255_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L255_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom256To511Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L511_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L511_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom512To1023Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L1023_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L1023_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom1024To1518Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
		}else if(hsgmii_index == 2 || hsgmii_index == 3){
			hsgmii_rx_high = read_reg_word(GDMA4_RX_OK_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_OK_CNT);
			fe_data->api_data.FE_RxCnt.rxOKPktCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxFCDropCnt = read_reg_word(GDMA4_RX_FC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxRCDropCnt = read_reg_word(GDMA4_RX_RC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxOVDropCnt = read_reg_word(GDMA4_RX_OVER_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxERRDropCnt= read_reg_word(GDMA4_RX_ERROR_DROP_CNT);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_OK_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxOKByteCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxOversizeCnt = read_reg_word(GDMA4_RX_ETH_LONG_CNT);
			fe_data->api_data.FE_RxCnt.rxUnderSizeCnt = read_reg_word(GDMA4_RX_ETH_RUNT_CNT);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_BYTE_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameLen = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxDropCnt = read_reg_word(GDMA4_RX_ETH_DROP_CNT);
			hsgmii_rx_high = read_reg_word(GDMA4_RX_BC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_BC_CNT);
			fe_data->api_data.FE_RxCnt.rxBroadcastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			hsgmii_rx_high = read_reg_word(GDMA4_RX_MC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_MC_CNT);
			fe_data->api_data.FE_RxCnt.rxMulticastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			
			fe_data->api_data.FE_RxCnt.rxCrcCnt = read_reg_word(GDMA4_RX_ETH_CRCE_CNT);
			fe_data->api_data.FE_RxCnt.rxFragFameCnt = read_reg_word(GDMA4_RX_ETH_FRAG_CNT);
			fe_data->api_data.FE_RxCnt.rxJabberFameCnt = read_reg_word(GDMA4_RX_ETH_JABBER_CNT);
			fe_data->api_data.FE_RxCnt.rxLess64Cnt = read_reg_word(GDMA4_RX_ETH_RUNT_CNT);
			fe_data->api_data.FE_RxCnt.rxMore1518Cnt = read_reg_word(GDMA4_RX_ETH_LONG_CNT);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_E64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_E64_CNT);
			fe_data->api_data.FE_RxCnt.rxEq64Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L64_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom65To127Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L127_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L127_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom128To255Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L255_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L255_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom256To511Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L511_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L511_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom512To1023Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L1023_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L1023_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom1024To1518Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

		}


	}else if (isAN7583){


		if(hsgmii_index == 0) {
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM4_TX_MIB_ID_OFFSET) + (0 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );
		}else if(hsgmii_index == 1){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM4_TX_MIB_ID_OFFSET) + (0 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}else if(hsgmii_index == 2){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_ID_OFFSET) + (1 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}else if(hsgmii_index == 3){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM3_TX_MIB_ID_OFFSET) + (0 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}
		if(hsgmii_index == 0 || hsgmii_index == 1 || hsgmii_index == 2){
			hsgmii_rx_high = read_reg_word(GDMA4_RX_OK_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_OK_CNT);
			fe_data->api_data.FE_RxCnt.rxOKPktCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxFCDropCnt = read_reg_word(GDMA4_RX_FC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxRCDropCnt = read_reg_word(GDMA4_RX_RC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxOVDropCnt = read_reg_word(GDMA4_RX_OVER_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxERRDropCnt= read_reg_word(GDMA4_RX_ERROR_DROP_CNT);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_OK_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxOKByteCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxOversizeCnt = read_reg_word(GDMA4_RX_ETH_LONG_CNT);
			fe_data->api_data.FE_RxCnt.rxUnderSizeCnt = read_reg_word(GDMA4_RX_ETH_RUNT_CNT);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_BYTE_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameLen = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxDropCnt = read_reg_word(GDMA4_RX_ETH_DROP_CNT);
			hsgmii_rx_high = read_reg_word(GDMA4_RX_BC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_BC_CNT);
			fe_data->api_data.FE_RxCnt.rxBroadcastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			hsgmii_rx_high = read_reg_word(GDMA4_RX_MC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_MC_CNT);
			fe_data->api_data.FE_RxCnt.rxMulticastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			
			fe_data->api_data.FE_RxCnt.rxCrcCnt = read_reg_word(GDMA4_RX_ETH_CRCE_CNT);
			fe_data->api_data.FE_RxCnt.rxFragFameCnt = read_reg_word(GDMA4_RX_ETH_FRAG_CNT);
			fe_data->api_data.FE_RxCnt.rxJabberFameCnt = read_reg_word(GDMA4_RX_ETH_JABBER_CNT);
			fe_data->api_data.FE_RxCnt.rxLess64Cnt = read_reg_word(GDMA4_RX_ETH_RUNT_CNT);
			fe_data->api_data.FE_RxCnt.rxMore1518Cnt = read_reg_word(GDMA4_RX_ETH_LONG_CNT);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_E64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_E64_CNT);
			fe_data->api_data.FE_RxCnt.rxEq64Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L64_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom65To127Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L127_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L127_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom128To255Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L255_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L255_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom256To511Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L511_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L511_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom512To1023Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA4_RX_ETH_L1023_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA4_RX_ETH_L1023_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom1024To1518Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
		}else if(hsgmii_index == 3){
			hsgmii_rx_high = read_reg_word(GDMA3_RX_OK_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_OK_CNT);
			fe_data->api_data.FE_RxCnt.rxOKPktCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxFCDropCnt = read_reg_word(GDMA3_RX_FC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxRCDropCnt = read_reg_word(GDMA3_RX_RC_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxOVDropCnt = read_reg_word(GDMA3_RX_OVER_DROP_CNT);
			fe_data->api_data.FE_RxCnt.rxERRDropCnt= read_reg_word(GDMA3_RX_ERROR_DROP_CNT);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_OK_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxOKByteCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxOversizeCnt = read_reg_word(GDMA3_RX_ETH_LONG_CNT);
			fe_data->api_data.FE_RxCnt.rxUnderSizeCnt = read_reg_word(GDMA3_RX_ETH_RUNT_CNT);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_PKT_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_BYTE_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_BYTE_CNT);
			fe_data->api_data.FE_RxCnt.rxFrameLen = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			fe_data->api_data.FE_RxCnt.rxDropCnt = read_reg_word(GDMA3_RX_ETH_DROP_CNT);
			hsgmii_rx_high = read_reg_word(GDMA3_RX_BC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_BC_CNT);
			fe_data->api_data.FE_RxCnt.rxBroadcastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			hsgmii_rx_high = read_reg_word(GDMA3_RX_MC_PKT_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_MC_CNT);
			fe_data->api_data.FE_RxCnt.rxMulticastCnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			
			fe_data->api_data.FE_RxCnt.rxCrcCnt = read_reg_word(GDMA3_RX_ETH_CRCE_CNT);
			fe_data->api_data.FE_RxCnt.rxFragFameCnt = read_reg_word(GDMA3_RX_ETH_FRAG_CNT);
			fe_data->api_data.FE_RxCnt.rxJabberFameCnt = read_reg_word(GDMA3_RX_ETH_JABBER_CNT);
			fe_data->api_data.FE_RxCnt.rxLess64Cnt = read_reg_word(GDMA3_RX_ETH_RUNT_CNT);
			fe_data->api_data.FE_RxCnt.rxMore1518Cnt = read_reg_word(GDMA3_RX_ETH_LONG_CNT);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_E64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_E64_CNT);
			fe_data->api_data.FE_RxCnt.rxEq64Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L64_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L64_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom65To127Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L127_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L127_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom128To255Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L255_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L255_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom256To511Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L511_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L511_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom512To1023Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);

			hsgmii_rx_high = read_reg_word(GDMA3_RX_ETH_L1023_CNT_H);
			hsgmii_rx_low = read_reg_word(GDMA3_RX_ETH_L1023_CNT);
			fe_data->api_data.FE_RxCnt.rxFrom1024To1518Cnt = ((hsgmii_rx_high << 32) + hsgmii_rx_low);
			

		}

	}


	return 0;
}

int fe_api_get_hsgmii_tx_cnt(struct ecnt_fe_data *fe_data)
{
	int hsgmii_index = fe_data->index; //0:pcie 0 hsgmii, 1 pcie 1 hsgmii, 2:usb hsgmii in 7523
	//0:pcie 0 hsgmii, 1 pcie 1 hsgmii, 2:usb hsgmii ,3:eth hsgmii, pcie0/pcie1 in gdm3,usb eth in gdm4 in 7581
	unsigned int reg=0;
	unsigned long long hsgmii_tx_high = 0;
	unsigned int hsgmii_tx_low = 0;

	if (isEN7523)
	{
		if(hsgmii_index == 0) {
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (4 << GDM3_TX_MIB_ID_OFFSET) + (0 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );
		}else if(hsgmii_index == 1){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (5 << GDM3_TX_MIB_ID_OFFSET) + (1 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}else if(hsgmii_index == 2){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (6 << GDM3_TX_MIB_ID_OFFSET) + (2 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_PKT_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_PKT_CNT);
		fe_data->api_data.FE_TxCnt.txFrameCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_BYTE_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_BYTE_CNT);
		fe_data->api_data.FE_TxCnt.txFrameLen = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
		fe_data->api_data.FE_TxCnt.txDropCnt = read_reg_word(GDMA3_TX_ETH_DROP_CNT);
		hsgmii_tx_high = read_reg_word(GDMA3_TX_BC_PKT_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_BC_CNT);
		fe_data->api_data.FE_TxCnt.txBroadcastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
		hsgmii_tx_high = read_reg_word(GDMA3_TX_MC_PKT_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_MC_CNT);
		fe_data->api_data.FE_TxCnt.txMulticastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
		
		fe_data->api_data.FE_TxCnt.txLess64Cnt = read_reg_word(GDMA3_TX_ETH_RUNT_CNT);
		fe_data->api_data.FE_TxCnt.txMore1518Cnt = read_reg_word(GDMA3_TX_ETH_LONG_CNT);

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_E64_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_E64_CNT);
		fe_data->api_data.FE_TxCnt.txEq64Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L64_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L64_CNT);
		fe_data->api_data.FE_TxCnt.txFrom65To127Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L127_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L127_CNT);
		fe_data->api_data.FE_TxCnt.txFrom128To255Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L255_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L255_CNT);
		fe_data->api_data.FE_TxCnt.txFrom256To511Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L511_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L511_CNT);
		fe_data->api_data.FE_TxCnt.txFrom512To1023Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L1023_CNT_H);
		hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L1023_CNT);
		fe_data->api_data.FE_TxCnt.txFrom1024To1518Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
	}else if (isEN7581){


		if(hsgmii_index == 0) {
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (4 << GDM3_TX_MIB_ID_OFFSET) + (0 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );
		}else if(hsgmii_index == 1){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (5 << GDM3_TX_MIB_ID_OFFSET) + (1 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}else if(hsgmii_index == 2){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_ID_OFFSET) + (1 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}else if(hsgmii_index == 3){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM4_TX_MIB_ID_OFFSET) + (0 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}
		if(hsgmii_index == 0 || hsgmii_index == 1){
			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_PKT_CNT);
			fe_data->api_data.FE_TxCnt.txFrameCnt = ((hsgmii_tx_high << 32) +hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_BYTE_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_BYTE_CNT);
			fe_data->api_data.FE_TxCnt.txFrameLen = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			fe_data->api_data.FE_TxCnt.txDropCnt = read_reg_word(GDMA3_TX_ETH_DROP_CNT);
			hsgmii_tx_high = read_reg_word(GDMA3_TX_BC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_BC_CNT);
			fe_data->api_data.FE_TxCnt.txBroadcastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			hsgmii_tx_high = read_reg_word(GDMA3_TX_MC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_MC_CNT);
			fe_data->api_data.FE_TxCnt.txMulticastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			
			fe_data->api_data.FE_TxCnt.txLess64Cnt = read_reg_word(GDMA3_TX_ETH_RUNT_CNT);
			fe_data->api_data.FE_TxCnt.txMore1518Cnt = read_reg_word(GDMA3_TX_ETH_LONG_CNT);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_E64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_E64_CNT);
			fe_data->api_data.FE_TxCnt.txEq64Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L64_CNT);
			fe_data->api_data.FE_TxCnt.txFrom65To127Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L127_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L127_CNT);
			fe_data->api_data.FE_TxCnt.txFrom128To255Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L255_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L255_CNT);
			fe_data->api_data.FE_TxCnt.txFrom256To511Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L511_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L511_CNT);
			fe_data->api_data.FE_TxCnt.txFrom512To1023Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L1023_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L1023_CNT);
			fe_data->api_data.FE_TxCnt.txFrom1024To1518Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		}else if(hsgmii_index == 2 || hsgmii_index == 3){
			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_PKT_CNT);
			fe_data->api_data.FE_TxCnt.txFrameCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_BYTE_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_BYTE_CNT);
			fe_data->api_data.FE_TxCnt.txFrameLen = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			fe_data->api_data.FE_TxCnt.txDropCnt = read_reg_word(GDMA4_TX_ETH_DROP_CNT);
			hsgmii_tx_high = read_reg_word(GDMA4_TX_BC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_BC_CNT);
			fe_data->api_data.FE_TxCnt.txBroadcastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			hsgmii_tx_high = read_reg_word(GDMA4_TX_MC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_MC_CNT);
			fe_data->api_data.FE_TxCnt.txMulticastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			
			fe_data->api_data.FE_TxCnt.txLess64Cnt = read_reg_word(GDMA4_TX_ETH_RUNT_CNT);
			fe_data->api_data.FE_TxCnt.txMore1518Cnt = read_reg_word(GDMA4_TX_ETH_LONG_CNT);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_E64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_E64_CNT);
			fe_data->api_data.FE_TxCnt.txEq64Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L64_CNT);
			fe_data->api_data.FE_TxCnt.txFrom65To127Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L127_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L127_CNT);
			fe_data->api_data.FE_TxCnt.txFrom128To255Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L255_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L255_CNT);
			fe_data->api_data.FE_TxCnt.txFrom256To511Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L511_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L511_CNT);
			fe_data->api_data.FE_TxCnt.txFrom512To1023Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L1023_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L1023_CNT);
			fe_data->api_data.FE_TxCnt.txFrom1024To1518Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		}



	}else if (isAN7583){


		if(hsgmii_index == 0) {
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM4_TX_MIB_ID_OFFSET) + (0 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );
		}else if(hsgmii_index == 1){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM4_TX_MIB_ID_OFFSET) + (0 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}else if(hsgmii_index == 2){
			reg = (1 << GDM4_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_SPLIT_EN_OFFSET) + (1 << GDM4_TX_MIB_ID_OFFSET) + (1 << GDM4_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA4_MIB_CFG, reg );

		}else if(hsgmii_index == 3){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (0 << GDM3_TX_MIB_ID_OFFSET) + (0 << GDM3_RX_MIB_ID_OFFSET); 
			write_reg_word(GDMA3_MIB_CFG, reg );

		}
		if(hsgmii_index == 0 || hsgmii_index == 1 || hsgmii_index == 2){
			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_PKT_CNT);
			fe_data->api_data.FE_TxCnt.txFrameCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_BYTE_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_BYTE_CNT);
			fe_data->api_data.FE_TxCnt.txFrameLen = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			fe_data->api_data.FE_TxCnt.txDropCnt = read_reg_word(GDMA4_TX_ETH_DROP_CNT);
			hsgmii_tx_high = read_reg_word(GDMA4_TX_BC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_BC_CNT);
			fe_data->api_data.FE_TxCnt.txBroadcastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			hsgmii_tx_high = read_reg_word(GDMA4_TX_MC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_MC_CNT);
			fe_data->api_data.FE_TxCnt.txMulticastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			
			fe_data->api_data.FE_TxCnt.txLess64Cnt = read_reg_word(GDMA4_TX_ETH_RUNT_CNT);
			fe_data->api_data.FE_TxCnt.txMore1518Cnt = read_reg_word(GDMA4_TX_ETH_LONG_CNT);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_E64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_E64_CNT);
			fe_data->api_data.FE_TxCnt.txEq64Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L64_CNT);
			fe_data->api_data.FE_TxCnt.txFrom65To127Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L127_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L127_CNT);
			fe_data->api_data.FE_TxCnt.txFrom128To255Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L255_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L255_CNT);
			fe_data->api_data.FE_TxCnt.txFrom256To511Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L511_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L511_CNT);
			fe_data->api_data.FE_TxCnt.txFrom512To1023Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA4_TX_ETH_L1023_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA4_TX_ETH_L1023_CNT);
			fe_data->api_data.FE_TxCnt.txFrom1024To1518Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		}else if(hsgmii_index == 3){
			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_PKT_CNT);
			fe_data->api_data.FE_TxCnt.txFrameCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_BYTE_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_BYTE_CNT);
			fe_data->api_data.FE_TxCnt.txFrameLen = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			fe_data->api_data.FE_TxCnt.txDropCnt = read_reg_word(GDMA3_TX_ETH_DROP_CNT);
			hsgmii_tx_high = read_reg_word(GDMA3_TX_BC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_BC_CNT);
			fe_data->api_data.FE_TxCnt.txBroadcastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			hsgmii_tx_high = read_reg_word(GDMA3_TX_MC_PKT_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_MC_CNT);
			fe_data->api_data.FE_TxCnt.txMulticastCnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);
			
			fe_data->api_data.FE_TxCnt.txLess64Cnt = read_reg_word(GDMA3_TX_ETH_RUNT_CNT);
			fe_data->api_data.FE_TxCnt.txMore1518Cnt = read_reg_word(GDMA3_TX_ETH_LONG_CNT);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_E64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_E64_CNT);
			fe_data->api_data.FE_TxCnt.txEq64Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L64_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L64_CNT);
			fe_data->api_data.FE_TxCnt.txFrom65To127Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L127_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L127_CNT);
			fe_data->api_data.FE_TxCnt.txFrom128To255Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L255_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L255_CNT);
			fe_data->api_data.FE_TxCnt.txFrom256To511Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L511_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L511_CNT);
			fe_data->api_data.FE_TxCnt.txFrom512To1023Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

			hsgmii_tx_high = read_reg_word(GDMA3_TX_ETH_L1023_CNT_H);
			hsgmii_tx_low = read_reg_word(GDMA3_TX_ETH_L1023_CNT);
			fe_data->api_data.FE_TxCnt.txFrom1024To1518Cnt = ((hsgmii_tx_high << 32) + hsgmii_tx_low);

		}

	}
	

	return 0;
}


int fe_api_get_eth_frame_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr;
	FE_TXRX_Sel_t txrx_sel = fe_data->txrx_sel;
	if(txrx_sel == FE_GDM_SEL_TX)
		base_addr = GDMA2_TX_ETHCNT;
	else
		base_addr = GDMA2_RX_ETHERPCNT;

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_get_eth_err_cnt(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr=0;
	FE_Err_type_t err_type = fe_data->err_type;
	if(err_type == FE_ERR_CRC)
		base_addr = GDMA2_RX_ETHCRCCNT;
	else if(err_type == FE_ERR_LONG)
		base_addr = GDMA2_RX_ETHLONGCNT;
	else if(err_type == FE_ERR_RUNT)
		base_addr = GDMA2_RX_ETHRUNTCNT;

	fe_data->cnt = read_reg_word(base_addr);
	return 0;
}

int fe_api_set_clear_mib(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, val, offset;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_TXRX_Sel_t txrx_sel = fe_data->txrx_sel;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_MIB_CLER;
	}else{
		base_addr = GDMA2_MIB_CLER;
	}
	offset = txrx_sel;
	
	val = read_reg_word(base_addr);
	val |= (1<<offset);
	
	write_reg_word(base_addr, val);
	
	return 0;
}

int  fe_get_ok_cnt(int  gdm_sel, int txrx_sel)
{
	/* NULL fun for hw_nat */
	return 0;
}

int qdma_set_channel_retire(uint channel)
{
	/* NULL fun for hw_nat */
	return 0;
}

void qdma_dma_mode(uint  txMode, uint rxMode, uint hwFwdMode)
{
	/* NULL fun for hw_nat */
	return;
}

int fe_api_set_cdm_rx_red_drop_mode(struct ecnt_fe_data *fe_data)
{
	FE_Cdma_Sel_t cdm_sel = fe_data->cdm_sel;
    FE_RedDropQ_Sel_t dropQ_sel = fe_data->dropQ_sel;
    FE_RedDropMode_Sel_t dropMode_sel = fe_data->dropMode_sel;
    unsigned long int reg, val, offset, mask;
	
	if(cdm_sel == FE_CDM_SEL_CDMA1)
		reg = CDMA1_FWD_CFG;
	else
		reg = CDMA2_FWD_CFG;

    mask = CDM_FWD_CFG_Q_MASK;
		
	if(dropQ_sel == FE_RED_DROP_Q0)
		offset = CDM_FWD_CFG_Q0_OFFSET;
    else if(dropQ_sel == FE_RED_DROP_Q1L)
        offset = CDM_FWD_CFG_Q1L_OFFSET;
	else
		offset = CDM_FWD_CFG_Q1H_OFFSET;
	
	val = read_reg_word(reg);
	val &= ~(mask << offset);
	val |= (dropMode_sel << offset);
	
	write_reg_word(reg, val);
	return 0;
}

int fe_api_get_cdm_rx_red_drop_mode(struct ecnt_fe_data *fe_data)
{
	FE_Cdma_Sel_t cdm_sel = fe_data->cdm_sel;
    FE_RedDropQ_Sel_t dropQ_sel = fe_data->dropQ_sel;
    unsigned long int reg, val, offset, mask;
	
	if(cdm_sel == FE_CDM_SEL_CDMA1)
		reg = CDMA1_FWD_CFG;
	else
		reg = CDMA2_FWD_CFG;

    mask = CDM_FWD_CFG_Q_MASK;
		
	if(dropQ_sel == FE_RED_DROP_Q0)
		offset = CDM_FWD_CFG_Q0_OFFSET;
    else if(dropQ_sel == FE_RED_DROP_Q1L)
        offset = CDM_FWD_CFG_Q1L_OFFSET;
	else
		offset = CDM_FWD_CFG_Q1H_OFFSET;
	
	val = read_reg_word(reg);
    fe_data->dropMode_sel = (val >> offset) & mask;
        
	return 0;
}

int get_bucketsize_shift(uint value, uint lo, uint hi, uint unit)
{
	int mid = 0;
	
	if(lo > hi )
		return -EINVAL;

	if((value > 0) && (value < unit))
		return 0;

	mid = (lo + hi) / 2;

	if((unit<<mid) == value){
		return mid;
		
	}else if((unit<<mid) > value){
		if((mid - lo) <= 1)
			return mid;
		
		return get_bucketsize_shift(value, lo, mid, unit);
	}else{
		if((hi - mid) <= 1)
			return hi;
		
		return get_bucketsize_shift(value, mid, hi, unit);
	}
}

int fe_set_ratelimit_param(uint gdm_sel,uint paraType,uint cfg_id,uint valueLo)
{
	uint paraCfg = 0;
	uint rc_cfg_reg = 0;
	uint rc_data_L_reg = 0;

	/* check FE GDM1/2/3/4 */
	if(FE_GDM_SEL_GDMA1 == gdm_sel)
	{
		rc_cfg_reg = GDMA1_RC_CFG;
		rc_data_L_reg = GDMA1_RC_DATA_L;
	}
	else if(FE_GDM_SEL_GDMA3 == gdm_sel)
	{
		rc_cfg_reg = GDMA3_RC_CFG;
		rc_data_L_reg = GDMA3_RC_DATA_L;
	}
	else if(FE_GDM_SEL_GDMA4 == gdm_sel)
	{
		rc_cfg_reg = GDMA4_RC_CFG;
		rc_data_L_reg = GDMA4_RC_DATA_L;
	}
	else
	{
		/* default cfg is GDMA2 */
		rc_cfg_reg = GDMA2_RC_CFG;
		rc_data_L_reg = GDMA2_RC_DATA_L;
	}
	
	write_reg_word(rc_data_L_reg, valueLo);
	
	paraCfg = (GDM_RC_CFG_EN | GDM_RC_CFG_PARA_RWCMD | ((paraType<<GDM_RC_CFG_PARA_TYPE_SHIFT)&GDM_RC_CFG_PARA_TYPE_MASK) | 
		((cfg_id<<GDM_RC_CFG_ID_SHIFT)&GDM_RC_CFG_ID_MASK));

	write_reg_word(rc_cfg_reg, paraCfg);
	
	return 0;
}

int fe_get_ratelimit_param(uint gdm_sel,uint paraType,uint cfg_id,uint *valueLo,uint *valueHi)
{
	uint paraCfg = 0;
	uint rc_cfg_reg = 0;
	uint rc_data_L_reg = 0;
	uint rc_data_H_reg = 0;

	/* check FE GDM1/2/3/4 */
	if(FE_GDM_SEL_GDMA1 == gdm_sel)
	{
		rc_cfg_reg = GDMA1_RC_CFG;
		rc_data_L_reg = GDMA1_RC_DATA_L;
		rc_data_H_reg = GDMA1_RC_DATA_H;
	}
	else if(FE_GDM_SEL_GDMA3 == gdm_sel)
	{
		rc_cfg_reg = GDMA3_RC_CFG;
		rc_data_L_reg = GDMA3_RC_DATA_L;
		rc_data_H_reg = GDMA3_RC_DATA_H;
	}
	else if(FE_GDM_SEL_GDMA4 == gdm_sel)
	{
		rc_cfg_reg = GDMA4_RC_CFG;
		rc_data_L_reg = GDMA4_RC_DATA_L;
		rc_data_H_reg = GDMA4_RC_DATA_H;
	}
	else
	{
		/* default cfg is GDMA2 */
		rc_cfg_reg = GDMA2_RC_CFG;
		rc_data_L_reg = GDMA2_RC_DATA_L;
		rc_data_H_reg = GDMA2_RC_DATA_H;
	}
	
	paraCfg = (((paraType<<GDM_RC_CFG_PARA_TYPE_SHIFT)&GDM_RC_CFG_PARA_TYPE_MASK) | 
		((cfg_id<<GDM_RC_CFG_ID_SHIFT)&GDM_RC_CFG_ID_MASK));

	write_reg_word(rc_cfg_reg, paraCfg);
	
	*valueLo = read_reg_word(rc_data_L_reg);
	*valueHi = read_reg_word(rc_data_H_reg);
	
	return 0;
}

#ifdef TCSUPPORT_CPU_EN7523
static int fe_set_tx_rate(unsigned int gdm_sel, unsigned int rate,unsigned int mode,unsigned maxBkSize,unsigned tick)
{
	unsigned int val = 0;
	unsigned int meter_en = 0;
	unsigned int tickSel = 0;
	ushort rateLimitUnit = 0;
	int bucketSize_shift = 0;
	int curTicksel = 0,bucketSize=0;
	uint tokenRate = 0;
	unsigned int tx_rc_cfg_reg = 0;
	unsigned int rc_data_h_reg = 0;
	uint tokenRate_integer = 0;
	ushort tokenRate_fraction = 0;
	uint valueLo = 0, valueHi = 0;

	if(GDM_RC_CFG_BYTE_MODE == mode){
		if((rate << 3) > GDM_TX_MAX_RATE){
			printk("rate exceeds the Max_tx_value allowed\n");
			return -EINVAL;
		}
	}	
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER) && defined(TCSUPPORT_CPU_EN7523)
	QDMA_TxRateLimitSet_T txRateLimitSet;

	if(USE_QDMA_WAN_ETHER == g_wan_mode){
		if(FE_GDM_SEL_GDMA2 != gdm_sel)
			return 0;

		if(0 == rate)
			txRateLimitSet.chnlRateLimitEn = 0;
		else
			txRateLimitSet.chnlRateLimitEn = 1;
		
		txRateLimitSet.chnlIdx = 0;
		
		txRateLimitSet.rateLimitValue = (rate << 3) ; 	/* unit: kbps */
		QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_WAN, &txRateLimitSet);
	}
#endif

	/* check FE GDM1/2/3 */
	if(FE_GDM_SEL_GDMA1 == gdm_sel)
	{
		tx_rc_cfg_reg = GDMA1_TX_RC_CFG;
		rc_data_h_reg = GDMA1_RC_DATA_H;
	}
	else if(FE_GDM_SEL_GDMA3 == gdm_sel)
	{
		tx_rc_cfg_reg = GDMA3_TX_RC_CFG;
		rc_data_h_reg = GDMA3_RC_DATA_H;
	}
	else if(FE_GDM_SEL_GDMA4 == gdm_sel)
	{
		tx_rc_cfg_reg = GDMA4_TX_RC_CFG;
		rc_data_h_reg = GDMA4_RC_DATA_H;
	}
	else
	{
		/* default cfg is GDMA2 */
		tx_rc_cfg_reg = GDMA2_TX_RC_CFG;
		rc_data_h_reg = GDMA2_RC_DATA_H;
	}
	
	/* check rate value */
	if(0 == rate)
	{
		meter_en = GDM_RC_CFG_METER_DISABLE;
	}
	else
	{
		meter_en = GDM_RC_CFG_METER_ENABLE;
	}
	
	/* default tick 125us */
	curTicksel = 125;
	tickSel = GDM_RC_CFG_FAST_TICK;
	
	/* enable tx glb ratelimit and set default tick = 125us */
	write_reg_word(tx_rc_cfg_reg,0x8001007d);
	write_reg_word(rc_data_h_reg,0x0);

	/* set basic parameters */
	fe_get_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_MISC, FE_WAN_TX_RATE_LIMIT, &valueLo, &valueHi);
	valueLo = (meter_en == GDM_RC_CFG_METER_ENABLE) ? (valueLo|GDM_RC_CFG_PARA_METER_EN):(valueLo &(~GDM_RC_CFG_PARA_METER_EN));
	valueLo = (mode == GDM_RC_CFG_PKT_MODE) ? (valueLo|GDM_RC_CFG_PARA_PPS_MODE):(valueLo &(~GDM_RC_CFG_PARA_PPS_MODE));
	valueLo = valueLo & (~GDM_RC_CFG_PARA_TICK_SEL);
	printk("basicParameter = 0x%x\n", valueLo);
	fe_set_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_MISC, FE_WAN_TX_RATE_LIMIT, valueLo);

	/* set token unit */
	if(GDM_RC_CFG_BYTE_MODE == mode)
	{
		/* restore the rate value for bps unit */
		rate = (rate << 3);
		
		/* 8bits X 1000 / (curTicksel X 10e-6 s)  kbps */
		rateLimitUnit = (8000 / curTicksel);
	}
	else
	{
		/* 1 / (curTicksel X 10e-6 s) pps */
		rateLimitUnit = (1000000 / curTicksel);
	}

	
	/* calculate tokenRate */
	tokenRate_integer = (rate / rateLimitUnit);
	tokenRate_fraction = ((rate % rateLimitUnit) * 64 / rateLimitUnit);
	if((tokenRate_integer > 0x3FFFF) || (tokenRate_fraction > 0x3F))
	{
		printk("tokenRate overflow.\n");
		return -EINVAL;
	}
	
	/* set token rate */
	tokenRate = ((tokenRate_integer << RC_TOKEN_RATE_INTEGER_SHIFT) | tokenRate_fraction);
	printk("tokenRate = 0x%x\n", tokenRate);
	fe_set_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_TOKEN_RATE, FE_WAN_TX_RATE_LIMIT, tokenRate);

	/* get bucket size  */
	/* refer to 7580 tx bucksize */
	bucketSize = 0x400000;


	/* set bucketsize_shift */
	bucketSize_shift = get_bucketsize_shift(bucketSize, 0, 15, 1024);
	printk("bucketSize_shift = 0x%x\n", bucketSize_shift);
	fe_set_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_BUCK_SHIFT, FE_WAN_TX_RATE_LIMIT, bucketSize_shift);
	
	return 0;
}

static int fe_set_rx_rate(unsigned int gdm_sel,unsigned int type,unsigned int rate,unsigned int mode)
{
	unsigned int val = 0;
	unsigned int meter_en = 0;
	unsigned int tickSel = 0;
	unsigned int rateLimitUnit = 0;
	int bucketSize_shift = 0;
	int curTicksel = 0,bucketSize=0;
	uint tokenRate = 0;
	unsigned int rx_rc_cfg_reg = 0;
	unsigned int rc_data_h_reg = 0;
	uint tokenRate_integer = 0;
	unsigned int tokenRate_fraction = 0;
	uint valueLo = 0, valueHi = 0;

	/* check FE GDM1/2/3 */
	if(FE_GDM_SEL_GDMA1 == gdm_sel)
	{
		rx_rc_cfg_reg = GDMA1_RX_RC_CFG;
		rc_data_h_reg = GDMA1_RC_DATA_H;
		/* GDM RX MAC Filter shaping */
		if (FE_WAN_RX_MAC_FILTER_RATE_LIMIT == type)
		{
			rx_rc_cfg_reg = GDMA1_RX_MAC_FILTER_SHP;
		}
	}
	else if(FE_GDM_SEL_GDMA3 == gdm_sel)
	{
		rx_rc_cfg_reg = GDMA3_RX_RC_CFG;
		rc_data_h_reg = GDMA3_RC_DATA_H;
		/* GDM RX MAC Filter shaping */
		if (FE_WAN_RX_MAC_FILTER_RATE_LIMIT == type)
		{
			rx_rc_cfg_reg = GDMA3_RX_MAC_FILTER_SHP;
		}
	}
	else
	{
		/* default cfg is GDMA2 */
		rx_rc_cfg_reg = GDMA2_RX_RC_CFG;
		rc_data_h_reg = GDMA2_RC_DATA_H;
		/* GDM RX MAC Filter shaping */
		if (FE_WAN_RX_MAC_FILTER_RATE_LIMIT == type)
		{
			rx_rc_cfg_reg = GDMA2_RX_MAC_FILTER_SHP;
		}
	}
	
	/* check rate value */
	if(0 == rate)
	{
		meter_en = GDM_RC_CFG_METER_DISABLE;
	}
	else
	{
		meter_en = GDM_RC_CFG_METER_ENABLE;
	}
	
	/* default tick 125us */
	curTicksel = 125;
	tickSel = GDM_RC_CFG_FAST_TICK;
	
	/* enable Rx glb ratelimit and set tick = 125us */
	write_reg_word(rx_rc_cfg_reg,0x8001007d);
	write_reg_word(rc_data_h_reg,0x0);

	/* set basic parameters */
	fe_get_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_MISC, type, &valueLo, &valueHi);
	valueLo = (meter_en == GDM_RC_CFG_METER_ENABLE) ? (valueLo|GDM_RC_CFG_PARA_METER_EN):(valueLo &(~GDM_RC_CFG_PARA_METER_EN));
	valueLo = (mode == GDM_RC_CFG_PKT_MODE) ? (valueLo|GDM_RC_CFG_PARA_PPS_MODE):(valueLo &(~GDM_RC_CFG_PARA_PPS_MODE));
	valueLo = valueLo & (~GDM_RC_CFG_PARA_TICK_SEL);
	printk("basicParameter = 0x%x\n", valueLo);
	fe_set_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_MISC, type, valueLo);
	
	/* set token unit  */
	if(GDM_RC_CFG_BYTE_MODE == mode)
	{
		/* 8bits X 1000 / (curTicksel X 10e-6 s)  kbps */
		rateLimitUnit = (8000 / curTicksel);
	}
	else
	{
		/* 1 / (curTicksel X 10e-6 s) pps */
		rateLimitUnit = (1000000 / curTicksel);
	}

	
	/* calculate tokenRate */
	tokenRate_integer = (rate / rateLimitUnit);
	tokenRate_fraction = ((rate % rateLimitUnit) * 64 / rateLimitUnit);
	if((tokenRate_integer > 0x3FFFF) || (tokenRate_fraction > 0x3F))
	{
		printk("tokenRate overflow.\n");
		return -EINVAL;
	}
	
	/* set token rate */
	tokenRate = ((tokenRate_integer << RC_TOKEN_RATE_INTEGER_SHIFT) | tokenRate_fraction);
	printk("tokenRate = 0x%x\n", tokenRate);
	fe_set_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_TOKEN_RATE, type, tokenRate);
	
	
	/* get bucket size  */
	if(GDM_RC_CFG_BYTE_MODE == mode)
	{
		bucketSize = (rate<<RC_BYTE_MODE_BUCKET_SHIFT);
	}
	else
	{
		bucketSize = (rate<<RC_PKT_MODE_BUCKET_SHIFT);
	}

	/*max: 32M bucket size */
	if(bucketSize > 0x2000000)
	{
		bucketSize = 0x2000000;
	}
	
	/* set bucketsize_shift */
	bucketSize_shift = get_bucketsize_shift(bucketSize, 0, 15, 1024);
	printk("bucketSize_shift = 0x%x\n", bucketSize_shift);
	fe_set_ratelimit_param(gdm_sel,GDM_RC_CFG_PARA_BUCK_SHIFT, type, bucketSize_shift);
	
	return 0;
}
#else
static int fe_set_tx_rate(unsigned int rate,unsigned int mode,unsigned maxBkSize,unsigned tick)
{

	unsigned int value = 0, bk_size = 0x7f;

	if (mode > 0)
		mode = 1;

    if((rate < 4096) && (mode == 0)){
        bk_size = rate >> 5;
        bk_size = bk_size > 0 ? bk_size : 1;        
    }

    if(isEN7580){
        tick = 2; /*for 7580, 0:20us, 1:125us, 2:1ms; 3:100ms*/
    }
    
	if(rate == 0){
		write_reg_word(GDMA2_SHRP_CFG,0);
	}else{
		value = (1 << GDMA_SHRP_CFG_EN_OFFSET) | (tick << GDMA_SHRP_CFG_TICK_OFFSET)
					|(rate << GDMA_SHRP_CFG_RATE_OFFSET) | (bk_size << GDMA_SHRP_CFG_BUCK_OFFSET) | mode;  

		write_reg_word(GDMA2_SHRP_CFG,value);
	}
	return 0;
}

static int fe_set_rx_rate(unsigned int type,unsigned int rate,unsigned int mode)
{
	unsigned int reg = 0,value = 0,tick = 0, bk_size = 0x7f;

	if (type == FE_WAN_RX_UC_RATE)
		reg = GDMA2_RXUC_SHPR_CFG;
	else if (type == FE_WAN_RX_BC_RATE)
		reg = GDMA2_RXBC_SHPR_CFG;
	else if (type == FE_WAN_RX_MC_RATE)
		reg = GDMA2_RXMC_SHPR_CFG;
	else if  (type == FE_WAN_RX_OC_RATE)
		reg = GDMA2_RXOC_SHPR_CFG;
	else
		return -1;


	if (mode > 0)
		mode = 1;
    
	if(isEN7580){
        tick = 2; /*for 7580, 0:20us, 1:125us, 2:1ms; 3:100ms*/
        if (mode == 1)
        {
            rate = rate / 1000;
        }
    }
    if((rate < 4096) && (mode == 0)){
        bk_size = rate >> 5;
        bk_size = bk_size > 0 ? bk_size : 1;    
    }

    if (!isEN7580 && (mode == 1))
    {
        if (rate < 10)
        {
            tick = 3;
        }
        else if (rate < 100)
        {
            tick = 2;
            rate = rate / 10;
        }
        else if (rate < 10000)
        {
            tick = 1;
            rate = rate /100;
        }
        else
        {
            tick = 0;
            rate = rate / 1000;
        }
    }
    
	rate &= 0x1fffff;
	
	if(rate == 0){
		write_reg_word(reg,0);
	}else{
		value = (1 << GDMA_SHRP_CFG_EN_OFFSET) | (tick << GDMA_SHRP_CFG_TICK_OFFSET)
					|(rate << GDMA_SHRP_CFG_RATE_OFFSET) | (bk_size << GDMA_SHRP_CFG_BUCK_OFFSET) | mode;  

		write_reg_word(reg,value);
	}
	return 0;
}
#endif

int fe_get_ratelimit_config(uint paraType,uint cfg_id,uint *valueLo,uint *valueHi)
{
	uint paraCfg = 0;
	
	paraCfg = (((paraType<<GDM_RC_CFG_PARA_TYPE_SHIFT)&GDM_RC_CFG_PARA_TYPE_MASK) | 
		((cfg_id<<FE_RATE_CFG_ID_SHIFT) & FE_RATE_CFG_ID_MASK));

	write_reg_word(FE_RX_RL_CFG, paraCfg);
	
	*valueLo = read_reg_word(FE_RX_RL_RDATA_L);
	*valueHi = read_reg_word(FE_RX_RL_RDATA_H);
	
	return 0;
}

int fe_set_ratelimit_config(uint paraType,uint cfg_id,uint *valueLo,uint *valueHi)
{
	uint paraCfg = 0;
	
	write_reg_word(FE_RX_RL_WDATA_L, valueLo);
	write_reg_word(FE_RX_RL_WDATA_H, valueHi);
	
	paraCfg = (GDM_RC_CFG_EN | GDM_RC_CFG_PARA_RWCMD | ((paraType<<GDM_RC_CFG_PARA_TYPE_SHIFT)&GDM_RC_CFG_PARA_TYPE_MASK) | 
		((cfg_id<<FE_RATE_CFG_ID_SHIFT) & FE_RATE_CFG_ID_MASK));

	printk("rate_cfg: value = 0x%x\n", paraCfg);
	write_reg_word(FE_RX_RL_CFG, paraCfg);
	
	return 0;
}

int fe_set_ratelimit_glb_config(uint port_id, uint meter_id, uint skip_rate)
{
	uint paraCfg = 0;
	
	paraCfg = (((port_id << FE_PORT_ID_SHIFT) & FE_PORT_ID_MASK) 
		| ((meter_id << FE_RATE_ID_WDATA_SHIFT) & FE_RATE_ID_WDATA_MASK) 
		| ((skip_rate << FE_SKIP_RATE_SHIFT) & FE_SKIP_RATE_MASK)
		| (FE_PORT_RATE_UPDATE));

	printk("glb_cfg: value = 0x%x\n", paraCfg);
	write_reg_word(FE_RX_RL_GLB_CFG, paraCfg);
	
	return 0;
}

int fe_set_rx_glb_rate(unsigned int port_id,unsigned int rate,unsigned int mode,unsigned int meter_id)
{
	unsigned int val = 0;
	unsigned int meter_en = 0;
	unsigned int tickSel = 0;
	unsigned int rateLimitUnit = 0;
	int bucketSize_shift = 0;
	int curTicksel = 0,bucketSize=0;
	uint tokenRate = 0;
	uint tokenRate_integer = 0;
	unsigned int tokenRate_fraction = 0;
	uint valueLo = 0, valueHi = 0;
	
	/* check rate value */
	if(0 == rate)
	{
		meter_en = FE_DISABLE;
	}
	else
	{
		meter_en = FE_ENABLE;
	}
	
	/* default tick=125us */
	curTicksel = 125;
	tickSel = FAST_TICK;
	
	/* enable rx ratelimit and set tick = 125us */
	write_reg_word(FE_RX_RL_TIK_CFG, 0x8001007d);

	/* clean the value */
	write_reg_word(FE_RX_RL_WDATA_L, 0x0);
	write_reg_word(FE_RX_RL_WDATA_H, 0x0);

	/* set basic parameters */
	valueLo = (meter_en == GDM_RC_CFG_METER_ENABLE) ? (valueLo|GDM_RC_CFG_PARA_METER_EN):(valueLo &(~GDM_RC_CFG_PARA_METER_EN));
	valueLo = (mode == GDM_RC_CFG_PKT_MODE) ? (valueLo|GDM_RC_CFG_PARA_PPS_MODE):(valueLo &(~GDM_RC_CFG_PARA_PPS_MODE));
	valueLo = (valueLo & (~GDM_RC_CFG_PARA_TICK_SEL));
	printk("basicParameter = 0x%x\n", valueLo);
	fe_set_ratelimit_config(GDM_RC_CFG_PARA_MISC, meter_id, valueLo, valueHi);
	
	/* calculate token unit */
	if(GDM_RC_CFG_BYTE_MODE == mode)
	{
		/* 8bits X 1000 / (curTicksel X 10e-6 s)  kbps */
		rateLimitUnit = (8000 / curTicksel);
	}
	else
	{
		/* 1 / (curTicksel X 10e-6 s) pps */
		rateLimitUnit = (1000000 / curTicksel);
	}
	
	/* calculate tokenRate */
	tokenRate_integer = (rate / rateLimitUnit);
	tokenRate_fraction = ((rate % rateLimitUnit) * 64 / rateLimitUnit);
	if((tokenRate_integer > 0x3FFFF) || (tokenRate_fraction > 0x3F))
	{
		printk("tokenRate overflow.\n");
		return -EINVAL;
	}
	
	/* set tokenRate */
	tokenRate = ((tokenRate_integer << RC_TOKEN_RATE_INTEGER_SHIFT) | tokenRate_fraction);
	printk("tokenRate = 0x%x\n", tokenRate);
	fe_set_ratelimit_config(GDM_RC_CFG_PARA_TOKEN_RATE, meter_id, tokenRate, valueHi);
	
	
	/* get bucket size  */
	if(GDM_RC_CFG_BYTE_MODE == mode)
	{
		bucketSize = (rate<<RC_BYTE_MODE_BUCKET_SHIFT);
	}
	else
	{
		bucketSize = (rate<<RC_PKT_MODE_BUCKET_SHIFT);
	}

	/*max: 32M bucket size */
	if(bucketSize > 0x2000000)
	{
		bucketSize = 0x2000000;
	}
	
	/* set bucketsize_shift */
	bucketSize_shift = get_bucketsize_shift(bucketSize, 0, 15, 1024);
	printk("bucketSize_shift = 0x%x\n", bucketSize_shift);
	fe_set_ratelimit_config(GDM_RC_CFG_PARA_BUCK_SHIFT, meter_id, bucketSize_shift, valueHi);

	/* set the port_id bind to the meter_id */
	printk("port_id = %d, meter_id = %d\n", port_id, meter_id);
	if(0 == port_id)
	{
		printk("only set meter\n");
		return 0;
	}
	
	if(0 == rate)
	{
		FeRxDisableRL(port_id);
	}
	else
	{
		FeRxEnableRL(port_id);
	}
	
	fe_set_ratelimit_glb_config(port_id, meter_id, 0x7);
	
	return 0;
}

static int fe_set_rxuc_rate(unsigned int gdm_sel,unsigned int rate,unsigned int mode)
{
	#ifdef TCSUPPORT_CPU_EN7523
	return fe_set_rx_rate(gdm_sel,FE_WAN_RX_UC_RATE,rate,mode);
	#else
	return fe_set_rx_rate(FE_WAN_RX_UC_RATE,rate,mode);
	#endif
}

static int fe_set_rxbc_rate(unsigned int gdm_sel,unsigned int rate,unsigned int mode)
{
	#ifdef TCSUPPORT_CPU_EN7523
	return fe_set_rx_rate(gdm_sel,FE_WAN_RX_BC_RATE,rate,mode);
	#else
	return fe_set_rx_rate(FE_WAN_RX_BC_RATE,rate,mode);
	#endif
}

static  int fe_set_rxmc_rate(unsigned int gdm_sel,unsigned int rate,unsigned int mode)
{
	#ifdef TCSUPPORT_CPU_EN7523
	return fe_set_rx_rate(gdm_sel,FE_WAN_RX_MC_RATE,rate,mode);
	#else
	return fe_set_rx_rate(FE_WAN_RX_MC_RATE,rate,mode);
	#endif
}

static int fe_set_rxoc_rate(unsigned int gdm_sel,unsigned int rate,unsigned int mode)
{
	#ifdef TCSUPPORT_CPU_EN7523
	return fe_set_rx_rate(gdm_sel,FE_WAN_RX_OC_RATE,rate,mode);
	#else
	return fe_set_rx_rate(FE_WAN_RX_OC_RATE,rate,mode);
	#endif
}

static int fe_set_rx_mac_filter_rate(unsigned int gdm_sel,unsigned int rate,unsigned int mode)
{
	#ifdef TCSUPPORT_CPU_EN7523
	return fe_set_rx_rate(gdm_sel,FE_WAN_RX_MAC_FILTER_RATE_LIMIT,rate,mode);
	#else
	return 0;
	#endif
}

static int  fe_set_vip_type(int idx,fe_vip_packet* vip){

	unsigned int value;

	if (idx >= FE_VIP_PACKET_NUM)
		return -1;

	write_reg_word(FE_VIP_EN(idx),0);
	write_reg_word(FE_VIP_PATN(idx),0);

	value = vip->sport;
	if (vip->type == FE_VIP_TYPE_TCP || vip->type == FE_VIP_TYPE_UDP)
		value |= (vip->dport << VIP_PATN_DP_OFFSET);
	
	write_reg_word(FE_VIP_PATN(idx),value);

	value = (1 << VIP_EN_CPU_OFFSET) | (vip->sw_en << VIP_EN_SWEN_OFFSET) 
			  | (vip->dp_en << VIP_EN_DPEN_OFFSET) | (vip->sp_en << VIP_EN_SPEN_OFFSET) 
			  | (vip->type << VIP_EN_TYPE_OFFSET) | (1 << VIP_EN_ENABLE_OFFSET);
	
	write_reg_word(FE_VIP_EN(idx),value);
	
	return idx;
}


static int fe_find_vip_type(fe_vip_packet* vip){

	unsigned int en_val;
	unsigned int patn_val;
	int i;

	for(i = 0; i< FE_VIP_PACKET_NUM;i++){
		en_val = read_reg_word(FE_VIP_EN(i));
		if ((en_val & 1) == 0)
			continue;
		
		if (((en_val >> VIP_EN_TYPE_OFFSET) & 0x07) !=  vip->type)
			continue;	

		patn_val = read_reg_word(FE_VIP_PATN(i));

		if( vip->type < 3 )/*0: ETH_TYPE ; 1:PPPOE protocol ; 2:IP protocol*/
		{
			if( (patn_val & 0xffff) != vip->etype )
				continue ;
		}
		else/*3:TCP ports; 4:UDP ports*/
		{
			if( (((en_val >> VIP_EN_DPEN_OFFSET ) & 0x01) ==  vip->dp_en) && (((en_val >> VIP_EN_SPEN_OFFSET ) & 0x01) ==  vip->sp_en) 
				&& (((en_val >> VIP_EN_SWEN_OFFSET ) & 0x01) ==  vip->sw_en) )/*compare port-type*/
			{
				if( (vip->sp_en == 1) && ((patn_val & 0xffff) != vip->sport) )
					continue;	
				if( ( vip->dp_en ==1 ) && (((patn_val >> 16) & 0xffff) != vip->dport) )
					continue ;
			}
			else
			{
			continue;
			}
		}

		return i;
	}

	return -1;
}

static int fe_get_vip_empty(void){

	unsigned int value;
	int i;

	for(i = 0; i< FE_VIP_PACKET_NUM;i++){
		value = read_reg_word(FE_VIP_EN(i));
		if ((value & 1) == 0)
			break;
	}

	return (i < FE_VIP_PACKET_NUM) ? i : (-1);
}

static int fe_del_vip_type(int idx){
	
	if (idx >= FE_VIP_PACKET_NUM)
		return -1;

	write_reg_word(FE_VIP_EN(idx),0);
	write_reg_word(FE_VIP_PATN(idx),0);
	return 0;
}

static int fe_add_vip_common(fe_vip_packet* vip)
{
	int idx;

	if (fe_find_vip_type(vip) >= 0)
		return -1;

	if ((idx = fe_get_vip_empty()) < 0)
		return -1;

	return fe_set_vip_type(idx,vip);
}

static int fe_del_vip_common(fe_vip_packet* vip)
{
	int idx;
	
	if ((idx = fe_find_vip_type(vip)) < 0)
		return -1;	

	return fe_del_vip_type(idx);
}


static int fe_add_vip_ether(uint16 type)
{
	fe_vip_packet	vip;

	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 0;
	vip.etype = type;

	return fe_add_vip_common(&vip);
}

static int fe_del_vip_ether(uint16 type)
{
	fe_vip_packet	vip;
	
	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 0;
	vip.etype = type;

	return fe_del_vip_common(&vip);
}

static int fe_add_vip_ppp(uint16 type)
{
	fe_vip_packet	vip;
	
	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 1;
	vip.ppp = type;

	return fe_add_vip_common(&vip);
}

static int fe_del_vip_ppp(uint16 type)
{
	fe_vip_packet	vip;
	
	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 1;
	vip.ppp = type;

	return fe_del_vip_common(&vip);
}

static int fe_add_vip_ip(uint16 type)
{
	fe_vip_packet	vip;

	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 2;
	vip.iptype= type;

	return fe_add_vip_common(&vip);
}

static int fe_del_vip_ip(uint16 type)
{
	fe_vip_packet	vip;
	
	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 2;
	vip.iptype= type;

	return fe_del_vip_common(&vip);
}

static int fe_add_vip_tcp(uint16 src, uint16 dst,unchar mode)
{
	fe_vip_packet	vip;

	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 3;
	vip.sport= src;
	vip.dport = dst;
	vip.sp_en = mode & 0x01 ;
	vip.dp_en = (mode & 0x02)>>1 ;
	vip.sw_en = (mode & 0x04)>>2 ;
	
	return fe_add_vip_common(&vip);
}

static int fe_del_vip_tcp(uint16 src, uint16 dst,unchar mode)
{
	fe_vip_packet	vip;

	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type =3;
	vip.sport= src;
	vip.dport = dst;
	vip.sp_en = mode & 0x01 ;
	vip.dp_en = (mode & 0x02)>>1 ;
	vip.sw_en = (mode & 0x04)>>2 ;

	return fe_del_vip_common(&vip);
}

static int fe_add_vip_udp(uint16 src, uint16 dst,unchar mode)
{
	fe_vip_packet	vip;

	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 4;
	vip.sport= src;
	vip.dport = dst;
	vip.sp_en = mode & 0x01 ;
	vip.dp_en = (mode & 0x02)>>1 ;
	vip.sw_en = (mode & 0x04)>>2 ;
	
	return fe_add_vip_common(&vip);
}

static int fe_del_vip_udp(uint16 src, uint16 dst,unchar mode)
{
	fe_vip_packet	vip;
	
	memset(&vip,0,sizeof(vip));
	
	vip.en = 1;
	vip.type = 4;
	vip.sport= src;
	vip.dport = dst;
	vip.sp_en = mode & 0x01 ;
	vip.dp_en = (mode & 0x02)>>1 ;
	vip.sw_en = (mode & 0x04)>>2 ;

	return fe_del_vip_common(&vip);
}

static int fe_set_l2lu_key(int idx ,fe_l2lu_key* key){

	unsigned int value;

	if (idx >= FE_L2LU_KEY_NUM) 
		return -1;
	
	write_reg_word(PSE_L2LU_KEY0(idx),0);
	write_reg_word(PSE_L2LU_KEY1(idx),0);

	value = (key->spcp << L2LU_KEY0_SPCP_OFFSET) | (key->svid << L2LU_KEY0_SVID_OFFSET) 
			| (key->cpcp << L2LU_KEY0_CPCP_OFFSET) | key->cvid;

	write_reg_word(PSE_L2LU_KEY1(idx),value);

	value = (key->dscp << L2LU_KEY1_DSCP_OFFSET) | (key->spcp_en << L2LU_KEY1_SPCP_EN_OFFSET) 
			| (key->svid_en << L2LU_KEY1_SVID_EN_OFFSET) | (key->cpcp_en << L2LU_KEY1_CPCP_EN_OFFSET)
			|  (key->cvid_en << L2LU_KEY1_CVID_EN_OFFSET) | key->dscp_mask;

	write_reg_word(PSE_L2LU_KEY0(idx),value);

	return idx;
}

static int fe_find_l2lu_key(fe_l2lu_key* key)
{
	unsigned int val;
	int i;


	for(i = 0; i< FE_VIP_PACKET_NUM;i++){
		val = read_reg_word(PSE_L2LU_KEY0(i));
		if ((val & 0xff) != key->dscp_mask)
			continue;
		if (((val >> 8) & 0x1) != key->cvid_en)
			continue;
		if (((val >> 9) & 0x1) != key->cpcp_en)
			continue;
		if (((val >> 12) & 0x1) != key->svid_en)
			continue;
		if (((val >> 13) & 0x1) != key->spcp_en)
			continue;
		if (((val >> 16) & 0xffff) != key->dscp)
			continue;
		
		val = read_reg_word(PSE_L2LU_KEY1(i));
		if ((val & 0xfff) != key->cvid)
			continue;
		if (((val >> 13) & 0x7) != key->cpcp)
			continue;		
		if (((val >> 16) & 0xfff) != key->svid)
			continue;	
		if (((val >> 29) & 0x7) != key->spcp)
			continue;
		
		return i;
	}

	return -1;

}

static int fe_get_l2lu_empty(void)
{
	unsigned int val,i;

	for(i = 0; i< FE_L2LU_KEY_NUM;i++){
		val = read_reg_word(PSE_L2LU_KEY0(i));
		if ((val & 0xffff) == 0)
			break;
	}
	
	return  (i < FE_L2LU_KEY_NUM) ? i: (-1);
}

static int fe_del_l2lu_key(int idx)
{
	if (idx >= FE_L2LU_KEY_NUM)
		return -1;
	
	write_reg_word(PSE_L2LU_KEY0(idx),0);
	write_reg_word(PSE_L2LU_KEY1(idx),0);
	return 0;
}

static int fe_add_l2lu_common(fe_l2lu_key* key)
{
	int idx ;

	if (fe_find_l2lu_key(key) >= 0)
		return -1;

	if ((idx = fe_get_l2lu_empty()) < 0)
		return -1;

	return fe_set_l2lu_key(idx,key);
}

static int fe_del_l2lu_common(fe_l2lu_key* key)
{
	int idx;

	if ((idx = fe_find_l2lu_key(key)) < 0)
		return -1;

	return fe_del_l2lu_key(idx);
}


static int fe_add_l2lu_vlan_dscp(ushort svlan,ushort cvlan,unchar dscp, unchar mask)
{
	fe_l2lu_key key;

	memset(&key,0,sizeof(key));
	
	key.svid_en = mask & 1;
	key.spcp_en = (mask >> 1) & 1;
	key.cvid_en = (mask >> 2) & 1;
	key.cpcp_en = (mask >> 3) & 1;
	key.dscp_mask= ((mask >> 4) & 1) ? 0x1f : 0;

	if (key.svid_en)
		key.svid = svlan & 0xfff;
	
	if (key.spcp_en)
		key.spcp = (svlan >> 13) & 0x7;

	if (key.cvid_en)
		key.cvid = cvlan & 0xfff;

	if (key.cpcp_en)
		key.cpcp = (cvlan >> 13) & 0x7;

	if (key.dscp_mask)
		key.dscp = dscp;
	
	return fe_add_l2lu_common(&key);
}

static int fe_del_l2lu_vlan_dscp(ushort svlan,ushort cvlan,unchar dscp, unchar mask)
{
	fe_l2lu_key key;

	memset(&key,0,sizeof(key));
	
	key.svid_en = mask & 1;
	key.spcp_en = (mask >> 1) & 1;
	key.cvid_en = (mask >> 2) & 1;
	key.cpcp_en = (mask >> 3) & 1;
	key.dscp_mask= ((mask >> 4) & 1) ? 0x1f : 0;

	if (key.svid_en)
		key.svid = svlan & 0xfff;
	
	if (key.spcp_en)
		key.spcp = (svlan >> 13) & 0x7;

	if (key.cvid_en)
		key.cvid = cvlan & 0xfff;

	if (key.cpcp_en)
		key.cpcp = (cvlan >> 13) & 0x7;

	if (key.dscp_mask)
		key.dscp = dscp;

	return fe_del_l2lu_common(&key);
}

static int fe_add_l2lu_vlan_trfc(ushort svlan,ushort cvlan,unchar trfc, unchar mask)
{
	fe_l2lu_key key;

	memset(&key,0,sizeof(key));
	
	key.svid_en = mask & 1;
	key.spcp_en = (mask >> 1) & 1;
	key.cvid_en = (mask >> 2) & 1;
	key.cpcp_en = (mask >> 3) & 1;
	key.dscp_mask= ((mask >> 4) & 1) ? 0xff : 0;

	if (key.svid_en)
		key.svid = svlan & 0xfff;
	
	if (key.spcp_en)
		key.spcp = (svlan >> 13) & 0x7;

	if (key.cvid_en)
		key.cvid = cvlan & 0xfff;

	if (key.cpcp_en)
		key.cpcp = (cvlan >> 13) & 0x7;

	if (key.dscp_mask)
		key.dscp = trfc;

	return fe_add_l2lu_common(&key);
}

static int fe_del_l2lu_vlan_trfc(ushort svlan,ushort cvlan,unchar trfc, unchar mask)
{
	fe_l2lu_key key;

	memset(&key,0,sizeof(key));
	
	key.svid_en = mask & 1;
	key.spcp_en = (mask >> 1) & 1;
	key.cvid_en = (mask >> 2) & 1;
	key.cpcp_en = (mask >> 3) & 1;
	key.dscp_mask= ((mask >> 4) & 1) ? 0xff : 0;

	if (key.svid_en)
		key.svid = svlan & 0xfff;
	
	if (key.spcp_en)
		key.spcp = (svlan >> 13) & 0x7;

	if (key.cvid_en)
		key.cvid = cvlan & 0xfff;

	if (key.cpcp_en)
		key.cpcp = (cvlan >> 13) & 0x7;

	if (key.dscp_mask)
		key.dscp = trfc;

	return fe_del_l2lu_common(&key);
}

int fe_set_tx_favor_oam_enable(unchar enable)
{
	unsigned int val;
	
	if((0 != enable)&&(1 != enable))
		return -1;
		
	if(!isEN7526c && !isEN751627)
		return -1;

	val = read_reg_word(GDMA2_FWD_CFG);

	val &= ~(1 << GDMA2_TX_FAVOR_OAM_OFFSET);
	val |= enable<<GDMA2_TX_FAVOR_OAM_OFFSET;

	write_reg_word(GDMA2_FWD_CFG,val);

	return 0;
}

int fe_do_scu_reset(void)
{
	unsigned char wan_mac_mode = 0;
	unsigned int val;
	unsigned int pse_iq_s1=0,pse_iq_s2=0,pse_oq_s1=0,pse_oq_s2=0;
	unsigned int check_times = 0;
	
	/*step 1:disable qdma tx*/
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN,QDMA_DISABLE);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN,QDMA_DISABLE);

	/*step 2:disable wan mac rx,only pon support in EN7626c*/
	ECNT_API_XPON_MODE_GET(&wan_mac_mode);
	
	if((ECNT_XPON_MAC_MODE_GPON == wan_mac_mode)||(ECNT_XPON_MAC_MODE_EPON == wan_mac_mode))
		ECNT_API_XPON_MAC_RX_DIS_SET(ECNT_XPON_MAC_RX_DISABLE);

	
	/*step 3:link down GSW P6*/
	ETHER_API_SET_PORT_LINKSTATE(6,PORT_LINK_DOWN);


	/*step 4:disable KA and aging*/
	PPE_API_SET_KA_CFG(KA_DISABLE);
	PPE_API_SET_AGING_CFG(0);


	/*step 5:wait until there is no pkt in PSE input queue&out queue*/
	while(check_times < 5)
	{
		mdelay(1);
		pse_iq_s1 = read_reg_word(PSE_IQ_STA1);
		pse_iq_s2 = read_reg_word(PSE_IQ_STA1);
		pse_oq_s1 = read_reg_word(PSE_OQ_STA1);
		pse_oq_s2 = read_reg_word(PSE_OQ_STA2);

		if((0 == pse_iq_s1)&&(0 == pse_iq_s2)&&(0 == pse_oq_s1)&&(0 == pse_oq_s2))
			break;

		check_times++;
	}

	if(5 == check_times)
		printk("WARNING!do scu reset when there is pkt in PSE!\n");

	/*step 6:write reset ctrl register*/
	val = read_reg_word(FE_RST_CTRL1);
	val |= 1<<21;
	write_reg_word(FE_RST_CTRL1,val);
	
	val &= ~(1<<21);
	write_reg_word(FE_RST_CTRL1,val);

	mdelay(1);

	/*step 7:enable qdma tx*/
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN,QDMA_ENABLE);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN,QDMA_ENABLE);

	/*step 8:enable wan mac rx*/
	if((ECNT_XPON_MAC_MODE_GPON == wan_mac_mode)||(ECNT_XPON_MAC_MODE_EPON == wan_mac_mode))
		ECNT_API_XPON_MAC_RX_DIS_SET(ECNT_XPON_MAC_RX_ENABLE);
	
	/*step 9:link up GSW p6*/
	ETHER_API_SET_PORT_LINKSTATE(6,PORT_LINK_UP);
	
	return 0;
}

int fe_do_core_reset(int qdma_reset_flag)
{
	unsigned char wan_mac_mode = 0;
	unsigned int aging_cfg = 0;
	unsigned int val;
	unsigned int pse_iq_s1=0,pse_iq_s2=0,pse_oq_s1=0,pse_oq_s2=0;
	unsigned int qdma_lan_free_dscp = 0, qdma_lan_free_dscp1 = 0;
	unsigned int qdma_wan_free_dscp = 0, qdma_wan_free_dscp1 = 0;
	unsigned int check_times = 0;
	
	/*step 1:disable qdma tx*/
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN,QDMA_DISABLE);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN,QDMA_DISABLE);

	/*step 2:disable wan mac rx,only pon support in EN7626c*/
	ECNT_API_XPON_MODE_GET(&wan_mac_mode);
	if((ECNT_XPON_MAC_MODE_GPON == wan_mac_mode)||(ECNT_XPON_MAC_MODE_EPON == wan_mac_mode))
		ECNT_API_XPON_MAC_RX_DIS_SET(ECNT_XPON_MAC_RX_DISABLE);

	/*step 3:link down GSW P6*/
	ETHER_API_SET_PORT_LINKSTATE(6,PORT_LINK_DOWN);

	/*step 4:disable KA and aging*/
	PPE_API_SET_KA_CFG(KA_DISABLE);
	PPE_API_GET_AGING_CFG(&aging_cfg);
	PPE_API_SET_AGING_CFG(0);

	/*step 5:wait until there is no pkt in PSE input queue&out queue*/
	while(check_times < 5)
	{
		mdelay(1);
		pse_iq_s1 = read_reg_word(PSE_IQ_STA1);
		pse_iq_s2 = read_reg_word(PSE_IQ_STA1);
		pse_oq_s1 = read_reg_word(PSE_OQ_STA1);
		pse_oq_s2 = read_reg_word(PSE_OQ_STA2);

		if((0 == pse_iq_s1)&&(0 == pse_iq_s2)&&(0 == pse_oq_s1)&&(0 == pse_oq_s2))
			break;

		check_times++;
	}

	if(5 == check_times)
		printk("WARNING!do scu reset when there is pkt in PSE!\n");

	/*step6:disable qdma rx*/
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN,QDMA_DISABLE);
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_LAN,QDMA_DISABLE);

	mdelay(1);
	/*step 7:write reset ctrl register*/
	val = read_reg_word(FE_RST_GLO);
	val |= 1<<1;	// FE_CORE_RESET
	write_reg_word(FE_RST_GLO,val);
	g_fe_iq_reset_cnt++;

	val = read_reg_word(FE_RST_GLO);
	val |= 1;	//  PSE_RESET
	write_reg_word(FE_RST_GLO,val);
	g_fe_all_reset_cnt++;
	
	mdelay(1);

	if(qdma_reset_flag == 1)
	{
		/* need to reset qdma wan in case of dscp leaking */
		val = read_reg_word(0xbfb550f0);
		qdma_wan_free_dscp = val & 0xFFFF;
		//if ((qdma_wan_free_dscp != 0x1000) || g_fe_pse_reset_test)
		{
			QDMA_API_RESET(ECNT_QDMA_WAN);
			g_fe_qdma_reset_cnt++;
			g_fe_last_qdma_dscp = qdma_wan_free_dscp;
		}

		val = read_reg_word(0xbfb540f0);
		qdma_lan_free_dscp = val & 0xFFFF;
		//if ((qdma_lan_free_dscp != 0x400) || g_fe_pse_reset_test)
		{
			QDMA_API_RESET(ECNT_QDMA_LAN);
			g_fe_qdma_lan_reset_cnt++;
			g_fe_last_qdma_lan_dscp = qdma_lan_free_dscp;
		}
    
		mdelay(1);
	}

	/*step 8:enable qdma tx*/
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN,QDMA_ENABLE);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN,QDMA_ENABLE);

	/*step 9:enable qdma rx*/
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN,QDMA_ENABLE);
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_LAN,QDMA_ENABLE);

	/*step 10:enable wan mac rx*/
	if((ECNT_XPON_MAC_MODE_GPON == wan_mac_mode)||(ECNT_XPON_MAC_MODE_EPON == wan_mac_mode))
		ECNT_API_XPON_MAC_RX_DIS_SET(ECNT_XPON_MAC_RX_ENABLE);
	
	/*step 9:link up GSW p6*/
	ETHER_API_SET_PORT_LINKSTATE(6,PORT_LINK_UP);

	/*step 11:enable KA and aging*/
	PPE_API_SET_KA_CFG(KA_DUPLICATE);
	PPE_API_SET_AGING_CFG(aging_cfg);
	
	return 0;
}

int fe_do_core_reset_with_qdma(void)
{
    fe_do_core_reset(1);
    return 0;
}

int fe_do_core_reset_without_qdma(void)
{
    fe_do_core_reset(0);
    return 0;
}

int fe_core_soft_reset(void){
	/* reset fe */
	write_reg_word(FE_RST_GLO,0x1);
	return 0;
}
void pse_set_total_page(void)
{
	unsigned char wan_mac_mode = 0;
	unsigned int aging_cfg = 0;
	unsigned int val;
	unsigned int pse_iq_s1=0,pse_iq_s2=0,pse_oq_s1=0,pse_oq_s2=0;
	unsigned int check_times = 0;

	/* select GDMP for FE usage */
	gdump_mode_sel(0x3);
	
	/*step 1:disable qdma tx*/
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN,QDMA_DISABLE);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN,QDMA_DISABLE);

	/*step 2:disable wan mac rx */
	ECNT_API_XPON_MODE_GET(&wan_mac_mode);
	if((ECNT_XPON_MAC_MODE_GPON == wan_mac_mode)||(ECNT_XPON_MAC_MODE_EPON == wan_mac_mode))
		ECNT_API_XPON_MAC_RX_DIS_SET(ECNT_XPON_MAC_RX_DISABLE);

	/*step 3:link down GSW P6*/
	ETHER_API_SET_PORT_LINKSTATE(6,PORT_LINK_DOWN);

	/*step 4:disable KA and aging*/
	PPE_API_SET_KA_CFG(KA_DISABLE);
	PPE_API_GET_AGING_CFG(&aging_cfg);
	PPE_API_SET_AGING_CFG(0);
	
	/*step6:disable qdma rx*/
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN,QDMA_DISABLE);
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_LAN,QDMA_DISABLE);

	mdelay(1);

	/* set PSE buffer to 0x500 = 0x400(pse itself) + 0x100(GDMP buffer) */
	write_reg_word(PSE_FQ_CFG, PSE_BUFFER_SIZE);
	
	QDMA_API_RESET(ECNT_QDMA_WAN);
	mdelay(1);
	QDMA_API_RESET(ECNT_QDMA_LAN);
	mdelay(1);
	
	/* reset fe */
	write_reg_word(FE_RST_GLO,0x1);
	
	mdelay(1);
	
	/*step 8:enable qdma tx*/
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN,QDMA_ENABLE);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN,QDMA_ENABLE);

	/*step 9:enable qdma rx*/
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN,QDMA_ENABLE);
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_LAN,QDMA_ENABLE);

	/*step 10:enable wan mac rx*/
	if((ECNT_XPON_MAC_MODE_GPON == wan_mac_mode)||(ECNT_XPON_MAC_MODE_EPON == wan_mac_mode))
		ECNT_API_XPON_MAC_RX_DIS_SET(ECNT_XPON_MAC_RX_ENABLE);
	
	/*step 9:link up GSW p6*/
	ETHER_API_SET_PORT_LINKSTATE(6,PORT_LINK_UP);

	/*step 11:enable KA and aging*/
	PPE_API_SET_KA_CFG(KA_DUPLICATE);
	PPE_API_SET_AGING_CFG(aging_cfg);
	
	return 0;
}

int fe_api_set_tx_rate(struct ecnt_fe_data *fe_data)
{
	 uint rate = fe_data->api_data.rate_cfg.rate;
	 uint mode = fe_data->api_data.rate_cfg.mode;
	 uint maxBkSize = fe_data->api_data.rate_cfg.maxBkSize;
	 uint tick = fe_data->api_data.rate_cfg.tick;

	 #ifdef TCSUPPORT_CPU_EN7523
	 uint gdm_sel = fe_data->gdm_sel;
	 fe_data->retValue = fe_set_tx_rate(gdm_sel,rate,mode,maxBkSize,tick);
	 #else
	 fe_data->retValue = fe_set_tx_rate(rate,mode,maxBkSize,tick);
	 #endif

	 return 0;
}

int fe_api_set_rx_rate(struct ecnt_fe_data *fe_data)
{
	uint port_id = fe_data->index;
	uint rate = fe_data->api_data.meter_cfg.rate;
	uint mode = fe_data->api_data.meter_cfg.pkt_mode;
	uint meter_id = fe_data->api_data.meter_cfg.meter_id;
	
	fe_data->retValue = fe_set_rx_glb_rate(port_id,rate,mode,meter_id);
	
	return 0;
}

int fe_api_set_rxuc_rate(struct ecnt_fe_data *fe_data)
{
	uint rate = fe_data->api_data.rate_cfg.rate;
	uint mode = fe_data->api_data.rate_cfg.mode;
	uint gdm_sel = fe_data->gdm_sel;
	fe_data->retValue = fe_set_rxuc_rate(gdm_sel,rate,mode);
	return 0;
}

int fe_api_set_rxbc_rate(struct ecnt_fe_data *fe_data)
{
	uint rate = fe_data->api_data.rate_cfg.rate;
	uint mode = fe_data->api_data.rate_cfg.mode;
	uint gdm_sel = fe_data->gdm_sel;
	fe_data->retValue = fe_set_rxbc_rate(gdm_sel,rate,mode);
	return 0;
}

int fe_api_set_rxmc_rate(struct ecnt_fe_data *fe_data)
{
	uint rate = fe_data->api_data.rate_cfg.rate;
	uint mode = fe_data->api_data.rate_cfg.mode;
	uint gdm_sel = fe_data->gdm_sel;
	fe_data->retValue = fe_set_rxmc_rate(gdm_sel,rate,mode);
	return 0;
}

int fe_api_set_rxoc_rate(struct ecnt_fe_data *fe_data)
{
	uint rate = fe_data->api_data.rate_cfg.rate;
	uint mode = fe_data->api_data.rate_cfg.mode;
	uint gdm_sel = fe_data->gdm_sel;
	fe_data->retValue = fe_set_rxoc_rate(gdm_sel,rate,mode);
	return 0;
}

int fe_api_set_rx_mac_filter_rate(struct ecnt_fe_data *fe_data)
{
	uint rate = fe_data->api_data.rate_cfg.rate;
	uint mode = fe_data->api_data.rate_cfg.mode;
	uint gdm_sel = fe_data->gdm_sel;
	fe_data->retValue = fe_set_rx_mac_filter_rate(gdm_sel,rate,mode);
	return 0;
}

int fe_api_add_vip_ether(struct ecnt_fe_data *fe_data)
{
	ushort type = fe_data->api_data.vip.type;
	fe_data->retValue = fe_add_vip_ether(type);
	return 0;
}

int fe_api_add_vip_ppp(struct ecnt_fe_data *fe_data)
{
	ushort type = fe_data->api_data.vip.type;
	fe_data->retValue = fe_add_vip_ppp(type);
	return 0;
}

int fe_api_add_vip_ip(struct ecnt_fe_data *fe_data)
{
	ushort type = fe_data->api_data.vip.type;
	fe_data->retValue = fe_add_vip_ip(type);
	return 0;
}

int fe_api_add_vip_tcp(struct ecnt_fe_data *fe_data)
{
	ushort src = fe_data->api_data.vip.src;
	ushort dst = fe_data->api_data.vip.dst;
	unchar mode = fe_data->api_data.vip.mode;
	fe_data->retValue = fe_add_vip_tcp(src,dst,mode);
#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
	QDMA_API_READ_VIP_INFO(ECNT_QDMA_WAN);
#endif
	return 0;
}

int fe_api_add_vip_udp(struct ecnt_fe_data *fe_data)
{
	ushort src = fe_data->api_data.vip.src;
	ushort dst = fe_data->api_data.vip.dst;
	unchar mode = fe_data->api_data.vip.mode;
	fe_data->retValue = fe_add_vip_udp(src,dst,mode);
#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
	QDMA_API_READ_VIP_INFO(ECNT_QDMA_WAN);
#endif
	return 0;
}

int fe_api_del_vip_ether(struct ecnt_fe_data *fe_data)
{
	ushort type = fe_data->api_data.vip.type;
	fe_data->retValue = fe_del_vip_ether(type);
	return 0;
}

int fe_api_del_vip_ppp(struct ecnt_fe_data *fe_data)
{
	ushort type = fe_data->api_data.vip.type;
	fe_data->retValue = fe_del_vip_ppp(type);
	return 0;
}

int fe_api_del_vip_ip(struct ecnt_fe_data *fe_data)
{
	ushort type = fe_data->api_data.vip.type;
	fe_data->retValue = fe_del_vip_ip(type);
	return 0;

}

int fe_api_del_vip_tcp(struct ecnt_fe_data *fe_data)
{
	ushort src = fe_data->api_data.vip.src;
	ushort dst = fe_data->api_data.vip.dst;
	unchar mode = fe_data->api_data.vip.mode;
	fe_data->retValue = fe_del_vip_tcp(src,dst,mode);
#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
	QDMA_API_READ_VIP_INFO(ECNT_QDMA_WAN);
#endif
	return 0;
}

int fe_api_del_vip_udp(struct ecnt_fe_data *fe_data)
{
	ushort src = fe_data->api_data.vip.src;
	ushort dst = fe_data->api_data.vip.dst;
	unchar mode = fe_data->api_data.vip.mode;
	fe_data->retValue = fe_del_vip_udp(src,dst,mode);
#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
	QDMA_API_READ_VIP_INFO(ECNT_QDMA_WAN);
#endif
	return 0;
}

int fe_api_add_l2lu_vlan_dscp(struct ecnt_fe_data *fe_data)
{
	unchar mask = fe_data->api_data.l2lu.mask;
	unchar dscp = fe_data->api_data.l2lu.dscp;
	ushort svlan = fe_data->api_data.l2lu.svlan;
	ushort cvlan = fe_data->api_data.l2lu.cvlan;
	fe_data->retValue = fe_add_l2lu_vlan_dscp(svlan,cvlan,dscp,mask);	
	return 0;
}

int fe_api_add_l2lu_vlan_trfc(struct ecnt_fe_data *fe_data)
{
	unchar mask = fe_data->api_data.l2lu.mask;
	unchar trfc = fe_data->api_data.l2lu.dscp;
	ushort svlan = fe_data->api_data.l2lu.svlan;
	ushort cvlan = fe_data->api_data.l2lu.cvlan;
	fe_data->retValue = fe_add_l2lu_vlan_trfc(svlan,cvlan,trfc,mask);		
	return 0;	
}

 int fe_api_del_l2lu_vlan_dscp(struct ecnt_fe_data *fe_data)
{
	unchar mask = fe_data->api_data.l2lu.mask;
	unchar dscp = fe_data->api_data.l2lu.dscp;
	ushort svlan = fe_data->api_data.l2lu.svlan;
	ushort cvlan = fe_data->api_data.l2lu.cvlan;
	fe_data->retValue = fe_del_l2lu_vlan_dscp(svlan,cvlan,dscp,mask);
	return 0;
}

 int fe_api_del_l2lu_vlan_trfc(struct ecnt_fe_data *fe_data)
{
	unchar mask = fe_data->api_data.l2lu.mask;
	unchar trfc = fe_data->api_data.l2lu.dscp;
	ushort svlan = fe_data->api_data.l2lu.svlan;
	ushort cvlan = fe_data->api_data.l2lu.cvlan;
	fe_data->retValue = fe_del_l2lu_vlan_trfc(svlan,cvlan,trfc,mask);	
	return 0;
}

int fe_add_l2lu_vlan_trfc_dscp(struct ecnt_fe_data *fe_data)
{
	unchar tc_min = fe_data->api_data.fe_trfc_cfg.tc_min;
	unchar tc_max = fe_data->api_data.fe_trfc_cfg.tc_max;
	unchar tos_min = fe_data->api_data.fe_trfc_cfg.tos_min;
	unchar tos_max = fe_data->api_data.fe_trfc_cfg.tos_max;
	ushort svid_min = fe_data->api_data.fe_trfc_cfg.svid_min;
	ushort svid_max = fe_data->api_data.fe_trfc_cfg.svid_max;
	unchar spbit_min = fe_data->api_data.fe_trfc_cfg.spbit_min;
	unchar spbit_max = fe_data->api_data.fe_trfc_cfg.spbit_max;
	ushort cvid_min = fe_data->api_data.fe_trfc_cfg.cvid_min;
	ushort cvid_max = fe_data->api_data.fe_trfc_cfg.cvid_max;
	unchar cpbit_min = fe_data->api_data.fe_trfc_cfg.cpbit_min;
	unchar cpbit_max = fe_data->api_data.fe_trfc_cfg.cpbit_max;
	uint mask = fe_data->api_data.fe_trfc_cfg.mask;
	unchar field_num = fe_data->api_data.fe_trfc_cfg.field_num;
 
	ushort svlan = 0;
	ushort cvlan = 0;
	ushort l2lu_mask = 0;
	uint range_idx = 0;

	if(field_num > 1){
		svlan = (((spbit_min & 0x7) << 13) | (svid_min & 0xfff));
		cvlan = (((cpbit_min & 0x7) << 13) | (cvid_min & 0xfff));
		if(mask & FE_TRFC_TC){
			/* TC + VLAN */
			l2lu_mask = ((mask & FE_TRFC_VLAN) | ((mask & FE_TRFC_TC) >> 1));
			fe_data->retValue = fe_add_l2lu_vlan_trfc(svlan, cvlan, tc_min, l2lu_mask);
		}else if(mask & FE_TRFC_TOS){
			/* TOS + VLAN */
			l2lu_mask = (mask & (FE_TRFC_VLAN | FE_TRFC_TOS));
			fe_data->retValue = fe_add_l2lu_vlan_dscp(svlan, cvlan, tos_min, l2lu_mask);
		}else if(mask & FE_TRFC_VLAN){
			/* VLAN */
			l2lu_mask = (mask & FE_TRFC_VLAN);
			fe_data->retValue = fe_add_l2lu_vlan_dscp(svlan, cvlan, 0, l2lu_mask);
		}else{
			goto add_l2lu_vlan_trfc_dscp_err;
		}
	}else if(field_num == 1){
		if(mask & FE_TRFC_TC){
			l2lu_mask = (FE_TRFC_TC >> 1);
			for(range_idx = tc_min; range_idx <= tc_max; range_idx++){
				fe_data->retValue = fe_add_l2lu_vlan_trfc(0, 0, range_idx, l2lu_mask);
			}
		}else if(mask & FE_TRFC_TOS){
			l2lu_mask = FE_TRFC_TOS;
			for(range_idx = tos_min; range_idx <= tos_max; range_idx++){
				fe_data->retValue = fe_add_l2lu_vlan_dscp(0, 0, range_idx, l2lu_mask);
			}
		}else if(mask & FE_TRFC_CPBIT){
			l2lu_mask = FE_TRFC_CPBIT;
			for(range_idx = cpbit_min; range_idx <= cpbit_max; range_idx++){
				cvlan = ((range_idx & 0x7) << 13);
				fe_data->retValue = fe_add_l2lu_vlan_dscp(0, cvlan, 0, l2lu_mask);
			}
		}else if(mask & FE_TRFC_CVID){
			l2lu_mask = FE_TRFC_CVID;
			for(range_idx = cvid_min; range_idx <= cvid_max; range_idx++){
				cvlan = (range_idx & 0xfff);
				fe_data->retValue = fe_add_l2lu_vlan_dscp(0, cvlan, 0, l2lu_mask);
			}
		}else if(mask & FE_TRFC_SPBIT){
			l2lu_mask = FE_TRFC_SPBIT;
			for(range_idx = spbit_min; range_idx <= spbit_max; range_idx++){
				svlan = ((range_idx & 0x7) << 13);
				fe_data->retValue = fe_add_l2lu_vlan_dscp(svlan, 0, 0, l2lu_mask);
			}
		}else if(mask & FE_TRFC_SVID){
			l2lu_mask = FE_TRFC_SVID;
			for(range_idx = svid_min; range_idx <= svid_max; range_idx++){
				svlan = (range_idx & 0xfff);
				fe_data->retValue = fe_add_l2lu_vlan_dscp(svlan, 0, 0, l2lu_mask);
			}
		}else{
			goto add_l2lu_vlan_trfc_dscp_err;
		}
	}else{
		goto add_l2lu_vlan_trfc_dscp_err;
	}

	if(fe_data->retValue < 0)
		printk("fe_add_l2lu_vlan_trfc_dscp: Config error! field_num = %d, mask=0x%08X\n", field_num, mask);

	return 0;

add_l2lu_vlan_trfc_dscp_err:
	printk("fe_add_l2lu_vlan_trfc_dscp: WARNING! Not support! field_num = %d, mask=0x%08X\n", field_num, mask);
	return 0;
}

int fe_del_l2lu_vlan_trfc_dscp(struct ecnt_fe_data *fe_data)
{
	unchar tc_min = fe_data->api_data.fe_trfc_cfg.tc_min;
	unchar tc_max = fe_data->api_data.fe_trfc_cfg.tc_max;
	unchar tos_min = fe_data->api_data.fe_trfc_cfg.tos_min;
	unchar tos_max = fe_data->api_data.fe_trfc_cfg.tos_max;
	ushort svid_min = fe_data->api_data.fe_trfc_cfg.svid_min;
	ushort svid_max = fe_data->api_data.fe_trfc_cfg.svid_max;
	unchar spbit_min = fe_data->api_data.fe_trfc_cfg.spbit_min;
	unchar spbit_max = fe_data->api_data.fe_trfc_cfg.spbit_max;
	ushort cvid_min = fe_data->api_data.fe_trfc_cfg.cvid_min;
	ushort cvid_max = fe_data->api_data.fe_trfc_cfg.cvid_max;
	unchar cpbit_min = fe_data->api_data.fe_trfc_cfg.cpbit_min;
	unchar cpbit_max = fe_data->api_data.fe_trfc_cfg.cpbit_max;
	uint mask = fe_data->api_data.fe_trfc_cfg.mask;
	unchar field_num = fe_data->api_data.fe_trfc_cfg.field_num;
 
	ushort svlan = 0;
	ushort cvlan = 0;
	ushort l2lu_mask = 0;
	uint range_idx = 0;

	if(field_num > 1){
		svlan = (((spbit_min & 0x7) << 13) | (svid_min & 0xfff));
		cvlan = (((cpbit_min & 0x7) << 13) | (cvid_min & 0xfff));
		if(mask & FE_TRFC_TC){
			/* TC + VLAN */
			l2lu_mask = ((mask & FE_TRFC_VLAN) | ((mask & FE_TRFC_TC) >> 1));
			fe_data->retValue = fe_del_l2lu_vlan_trfc(svlan, cvlan, tc_min, l2lu_mask);
		}else if(mask & FE_TRFC_TOS){
			/* TOS + VLAN */
			l2lu_mask = (mask & (FE_TRFC_VLAN | FE_TRFC_TOS));
			fe_data->retValue = fe_del_l2lu_vlan_dscp(svlan, cvlan, tos_min, l2lu_mask);
		}else if(mask & FE_TRFC_VLAN){
			/* VLAN */
			l2lu_mask = (mask & FE_TRFC_VLAN);
			fe_data->retValue = fe_del_l2lu_vlan_dscp(svlan, cvlan, 0, l2lu_mask);
		}else{
			goto del_l2lu_vlan_trfc_dscp_err;
		}
	}else if(field_num == 1){
		if(mask & FE_TRFC_TC){
			l2lu_mask = (FE_TRFC_TC >> 1);
			for(range_idx = tc_min; range_idx <= tc_max; range_idx++){
				fe_data->retValue = fe_del_l2lu_vlan_trfc(0, 0, range_idx, l2lu_mask);
			}
		}else if(mask & FE_TRFC_TOS){
			l2lu_mask = FE_TRFC_TOS;
			for(range_idx = tos_min; range_idx <= tos_max; range_idx++){
				fe_data->retValue = fe_del_l2lu_vlan_dscp(0, 0, range_idx, l2lu_mask);
			}
		}else if(mask & FE_TRFC_CPBIT){
			l2lu_mask = FE_TRFC_CPBIT;
			for(range_idx = cpbit_min; range_idx <= cpbit_max; range_idx++){
				cvlan = ((range_idx & 0x7) << 13);
				fe_data->retValue = fe_del_l2lu_vlan_dscp(0, cvlan, 0, l2lu_mask);
			}
		}else if(mask & FE_TRFC_CVID){
			l2lu_mask = FE_TRFC_CVID;
			for(range_idx = cvid_min; range_idx <= cvid_max; range_idx++){
				cvlan = (range_idx & 0xfff);
				fe_data->retValue = fe_del_l2lu_vlan_dscp(0, cvlan, 0, l2lu_mask);
			}
		}else if(mask & FE_TRFC_SPBIT){
			l2lu_mask = FE_TRFC_SPBIT;
			for(range_idx = spbit_min; range_idx <= spbit_max; range_idx++){
				svlan = ((range_idx & 0x7) << 13);
				fe_data->retValue = fe_del_l2lu_vlan_dscp(svlan, 0, 0, l2lu_mask);
			}
		}else if(mask & FE_TRFC_SVID){
			l2lu_mask = FE_TRFC_SVID;
			for(range_idx = svid_min; range_idx <= svid_max; range_idx++){
				svlan = (range_idx & 0xfff);
				fe_data->retValue = fe_del_l2lu_vlan_dscp(svlan, 0, 0, l2lu_mask);
			}
		}else{
			goto del_l2lu_vlan_trfc_dscp_err;
		}
	}else{
		goto del_l2lu_vlan_trfc_dscp_err;
	}

	if(fe_data->retValue < 0)
		printk("fe_del_l2lu_vlan_trfc_dscp: Config error! field_num = %d, mask=0x%08X\n", field_num, mask);

	return 0;

del_l2lu_vlan_trfc_dscp_err:
	printk("fe_del_l2lu_vlan_trfc_dscp: WARNING! Not support! field_num = %d, mask=0x%08X\n", field_num, mask);
	return 0;
}

int fe_ifc_vlan_trfc_dscp_parser(struct ecnt_fe_data *fe_data, struct ecnt_ifc_param *ifc_param)
{
	unchar ifc_field_idx = 0;
	unchar tc_min = fe_data->api_data.fe_trfc_cfg.tc_min;
	unchar tc_max = fe_data->api_data.fe_trfc_cfg.tc_max;
	unchar tos_min = fe_data->api_data.fe_trfc_cfg.tos_min;
	unchar tos_max = fe_data->api_data.fe_trfc_cfg.tos_max;
	ushort svid_min = fe_data->api_data.fe_trfc_cfg.svid_min;
	ushort svid_max = fe_data->api_data.fe_trfc_cfg.svid_max;
	unchar spbit_min = fe_data->api_data.fe_trfc_cfg.spbit_min;
	unchar spbit_max = fe_data->api_data.fe_trfc_cfg.spbit_max;
	ushort cvid_min = fe_data->api_data.fe_trfc_cfg.cvid_min;
	ushort cvid_max = fe_data->api_data.fe_trfc_cfg.cvid_max;
	unchar cpbit_min = fe_data->api_data.fe_trfc_cfg.cpbit_min;
	unchar cpbit_max = fe_data->api_data.fe_trfc_cfg.cpbit_max;
	uint mask = fe_data->api_data.fe_trfc_cfg.mask;
	unchar field_num = fe_data->api_data.fe_trfc_cfg.field_num;

	/* 1. set traffic class / DSCP field */
	if(mask & FE_TRFC_TC){
		ifc_param->field[ifc_field_idx] = IPv6_TRAFFIC_CLASS;
		ifc_param->key[ifc_field_idx] = tc_min;
		ifc_param->mask[ifc_field_idx] = tc_max;
		ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
		ifc_field_idx++;
	}else if(mask & FE_TRFC_TOS){
		ifc_param->field[ifc_field_idx] = IPv4_DSCP;
		ifc_param->key[ifc_field_idx] = (tos_min<<2)&0xfc; /* skip ECN bits */
		ifc_param->mask[ifc_field_idx] = ((tos_max<<2)&0xfc) | 0x3;
		ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
		ifc_field_idx++;
	}
	/* 2. set VLAN field */
	if(((mask & FE_TRFC_SVLAN) == 0) && ((mask & FE_TRFC_CVLAN) != 0)){
		/* only CVLAN ==> VLAN0 */
		if(mask & FE_TRFC_CPBIT){
			ifc_param->field[ifc_field_idx] = PBIT0;
			ifc_param->key[ifc_field_idx] = (cpbit_min<<1) & 0xe; /* skip CFI bit */
			ifc_param->mask[ifc_field_idx] = ((cpbit_max<<1) & 0xe) | 0x1;
			ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
			ifc_field_idx++;
		}
		if(mask & FE_TRFC_CVID){
			ifc_param->field[ifc_field_idx] = VID0;
			ifc_param->key[ifc_field_idx] = cvid_min;
			ifc_param->mask[ifc_field_idx] = cvid_max;
			ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
			ifc_field_idx++;
		}
	}else if(mask & FE_TRFC_VLAN){
		/* SVLAN==>VLAN0,  CVLAN ==> VLAN1 */
		if(mask & FE_TRFC_CPBIT){
			ifc_param->field[ifc_field_idx] = PBIT1;
			ifc_param->key[ifc_field_idx] = (cpbit_min<<1) & 0xe; /* skip CFI bit */
			ifc_param->mask[ifc_field_idx] = ((cpbit_max<<1) & 0xe) | 0x1;
			ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
			ifc_field_idx++;
		}
		if(mask & FE_TRFC_CVID){
			ifc_param->field[ifc_field_idx] = VID1;
			ifc_param->key[ifc_field_idx] = cvid_min;
			ifc_param->mask[ifc_field_idx] = cvid_max;
			ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
			ifc_field_idx++;
		}
		if(mask & FE_TRFC_SPBIT){
			ifc_param->field[ifc_field_idx] = PBIT0;
			ifc_param->key[ifc_field_idx] = (spbit_min<<1) & 0xe; /* skip CFI bit */
			ifc_param->mask[ifc_field_idx] = ((spbit_max<<1) & 0xe) | 0x1;
			ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
			ifc_field_idx++;
		}
		if(mask & FE_TRFC_SVID){
			ifc_param->field[ifc_field_idx] = VID0;
			ifc_param->key[ifc_field_idx] = svid_min;
			ifc_param->mask[ifc_field_idx] = svid_max;
			ifc_param->command[ifc_field_idx] = IFC_COMMAND_IN_RANGE;
			ifc_field_idx++;
		}
	}
	
	return 0;
}

int fe_add_ifc_vlan_trfc_dscp(struct ecnt_fe_data *fe_data)
{
	struct ecnt_ifc_param ifc_param;

#ifdef TCSUPPORT_IFC_EN
	if(ifcSendToPpe == 0)
	{
		printk("fe_add_ifc_vlan_trfc_dscp: WARNING! Not support! ifcSendToPpe is disable\n");
		return 0;
	}
#endif
	
	memset(&ifc_param, 0, sizeof(struct ecnt_ifc_param));
	fe_ifc_vlan_trfc_dscp_parser(fe_data, &ifc_param);

	if(0 > IFC_API_SET_LUT_RULE_AUTO(&ifc_param)){
		printk("fe_add_ifc_vlan_trfc_dscp: Fail!\n");
	}

	return 0;
}

int fe_del_ifc_vlan_trfc_dscp(struct ecnt_fe_data *fe_data)
{
	struct ecnt_ifc_param ifc_param;

	memset(&ifc_param, 0, sizeof(struct ecnt_ifc_param));
	fe_ifc_vlan_trfc_dscp_parser(fe_data, &ifc_param);

	if(0 > IFC_API_DEL_LUT_RULE_AUTO(&ifc_param)){
		printk("fe_del_ifc_vlan_trfc_dscp: Fail!\n");
	}

	return 0;
}

int fe_api_add_traffic_class(struct ecnt_fe_data *fe_data)
{
	int ifc_enable = 0;
	
	if(isEN751221){
		fe_add_l2lu_vlan_trfc_dscp(fe_data);
		
	}else if(isEN751627 || isEN7528){
		ifc_enable = IFC_API_GET_GLOBAL_ENABLE();

		if(0 == ifc_enable)/* if IFC disable, set L2LU rules */
			fe_add_l2lu_vlan_trfc_dscp(fe_data);
		else if(1 == ifc_enable)/* if IFC enable, set IFC rules */
			fe_add_ifc_vlan_trfc_dscp(fe_data);
		else
			printk("fe_api_add_traffic_class: add fail\n");
		
	}else if(IFC_TC_SUPPORT){
		fe_add_ifc_vlan_trfc_dscp(fe_data);
		
	}else{
		printk("fe_api_add_traffic_class: WARNING! Not support!\n");
	}
	
	return 0;
}

int fe_api_del_traffic_class(struct ecnt_fe_data *fe_data)
{	
	int ifc_enable = 0;
	
	if(isEN751221){
		fe_del_l2lu_vlan_trfc_dscp(fe_data);
		
	}else if(isEN751627 || isEN7528){
		ifc_enable = IFC_API_GET_GLOBAL_ENABLE();

		if(0 == ifc_enable)/* if IFC disable, set L2LU rules */
			fe_del_l2lu_vlan_trfc_dscp(fe_data);
		else if(1 == ifc_enable)/* if IFC enable, set IFC rules */
			fe_del_ifc_vlan_trfc_dscp(fe_data);
		else
			printk("fe_api_add_traffic_class: del fail\n");
		
	}else if(IFC_TC_SUPPORT){
		fe_del_ifc_vlan_trfc_dscp(fe_data);
		
	}else{
		printk("fe_api_del_traffic_class: WARNING! Not support!\n");
	}
	
	return 0;
}

 int fe_api_set_tx_favor_oam_enable(struct ecnt_fe_data *fe_data)
{
	unchar enable = fe_data->api_data.enable;
	fe_data->retValue = fe_set_tx_favor_oam_enable(enable);	
	return 0;
}

 int fe_api_set_tls_cfg(struct ecnt_fe_data *fe_data)
{
	unchar cmd = fe_data->api_data.tls_cfg.cmd;
	#ifdef TCSUPPORT_CSC_EEUROPE	
	FE_Tls_info_t tls_info;

	tls_info.uni_port_id = fe_data->api_data.tls_cfg.tls_info.uni_port_id;
	tls_info.uni_vlan = fe_data->api_data.tls_cfg.tls_info.uni_vlan;
	tls_info.gem_port = fe_data->api_data.tls_cfg.tls_info.gem_port;
	tls_info.ani_vlan = fe_data->api_data.tls_cfg.tls_info.ani_vlan;
	
	fe_data->retValue = fe_ioctl_set_trunk_mode_tls_cfg(cmd,tls_info);	
	#else
	unchar tls_port = fe_data->api_data.tls_cfg.tls_port;
	ushort tls_vlan = fe_data->api_data.tls_cfg.tls_vlan;

	fe_data->retValue = fe_ioctl_set_tls_cfg(cmd,tls_port,tls_vlan);	
	#endif
	
	return 0;
}

 int fe_api_do_fe_reset(struct ecnt_fe_data *fe_data)
{
	uint reset_mode = fe_data->api_data.reset_mode;
	
	if(!isEN7526c && !isEN751627)
		return -1;

	if(FE_SCU_RESET == reset_mode)
		fe_do_scu_reset();
	else if(FE_CORE_RESET == reset_mode)
		fe_do_core_reset(1);
	else
		return -1;

	return 0;
}

int fe_api_add_dev_to_total_account(struct ecnt_fe_data *fe_data)
{
	fe_add_dev_to_total_account(fe_data->api_data.dev_cfg.mac);

	return 0;
}

int fe_api_add_stb_src_ip(struct ecnt_fe_data *fe_data)
{
	fe_add_stb_src_ip(fe_data->api_data.stb_ip_cfg.ip_type, fe_data->api_data.stb_ip_cfg.ip4, &fe_data->api_data.stb_ip_cfg.ip6);

	return 0;
}

int fe_api_del_stb_src_ip(struct ecnt_fe_data *fe_data)
{
	fe_del_stb_src_ip(fe_data->api_data.stb_ip_cfg.ip_type, fe_data->api_data.stb_ip_cfg.ip4, &fe_data->api_data.stb_ip_cfg.ip6);

	return 0;
}

int fe_api_set_loopback_enable(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, offset, mask, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	uint channel = fe_data->channel;
	FE_Enable_t enable = fe_data->api_data.enable;

	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_LPBK_CFG;
	}else{
		base_addr = GDMA2_LPBP_CFG;
	}

	mask = 0xf1;
	val = read_reg_word(base_addr);
	val &= ~(mask);
	val |= (((0xf&channel)<<4) + enable);

	write_reg_word(base_addr, val);
	
	return 0;
}

int fe_api_set_loopback_mode(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, offset, mask, val;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	FE_Random_mode_t channel_mode = fe_data->api_data.lpbp_mode.channel_mode;
	FE_Random_mode_t length_mode = fe_data->api_data.lpbp_mode.length_mode;
	FE_Random_mode_t gap_mode = fe_data->api_data.lpbp_mode.gap_mode;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1){
		base_addr = GDMA1_LPBK_CFG;
	}else{
		base_addr = GDMA2_LPBP_CFG;
	}

	mask = 0x0e;
	val = read_reg_word(base_addr);
	val &= ~(mask);
	val |= ((channel_mode<<1) + (length_mode<<2) + (gap_mode<<3));

	write_reg_word(base_addr, val);
	
	return 0;
}

int fe_api_get_unknown_mul_pkt(struct ecnt_fe_data *fe_data)
{
	unsigned int gdm2_rx_eth_mc_cnt=read_reg_word(GDMA2_RX_ETHMCCNT);
	unsigned int ppe_ac_pcnt_tmp = read_reg_word(PPE_AC_PCNT(MC_PKT_ACNT_ID));
	g_Ppe_Ac_Pcnt1 += ppe_ac_pcnt_tmp;
	fe_data->api_data.unknown_mul_pkt = gdm2_rx_eth_mc_cnt - g_Ppe_Ac_Pcnt1;	
	return 0;
}

int fe_api_set_glo_rate_byte(struct ecnt_fe_data *fe_data)
{
	unsigned long int base_addr, offset, mask, val;
	FE_Rate_minus_t rate_minus = fe_data->api_data.glo_rate_byte.rate_minus;
	unchar rate_byte = fe_data->api_data.glo_rate_byte.rate_byte;

	base_addr = FE_DMA_GLO_CFG;

	mask = 0xff<<8;
	val = read_reg_word(base_addr);
	val &= ~(mask);
	val |= ((rate_minus<<15) + ((rate_byte&0x7f)<<8));

	write_reg_word(base_addr, val);

	return 0;
}

int fe_api_get_pppoe_info(struct ecnt_fe_data *fe_data)
{
	fe_data->api_data.pppoe_info.valid = pppoe_info_for_app.valid;
	fe_data->api_data.pppoe_info.sessionID= pppoe_info_for_app.sessionID;
	fe_data->api_data.pppoe_info.src_ip = pppoe_info_for_app.src_ip;
	fe_data->api_data.pppoe_info.dst_ip = pppoe_info_for_app.dst_ip;

	memcpy(fe_data->api_data.pppoe_info.smac_add, pppoe_info_for_app.smac_add, 6);
	memcpy(fe_data->api_data.pppoe_info.dmac_add, pppoe_info_for_app.dmac_add, 6);
	memcpy(fe_data->api_data.pppoe_info.ifname, pppoe_info_for_app.ifname, 16);

	fe_data->api_data.pppoe_info.dns_valid = pppoe_info_for_app.dns_valid;
	fe_data->api_data.pppoe_info.pri_dns = pppoe_info_for_app.pri_dns;
	fe_data->api_data.pppoe_info.snd_dns = pppoe_info_for_app.snd_dns;
	
	return 0;
}

int fe_api_set_pppoe_info_clean(struct ecnt_fe_data *fe_data)
{
	pppoe_info_for_app.valid = 0;

	memset(&pppoe_info_for_app, 0, sizeof(SMUX_Bridge_Info_Data));
	
	return 0;
}

int fe_api_get_tx_traffic(struct ecnt_fe_data *fe_data)
{
    unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
    int pre_index = (fe_cnt_cur_idx+FE_CNT_SECONDS-1)%FE_CNT_SECONDS;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

    if(tx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] >= tx_ok_byte_cnt[port_sel][pre_index])
        fe_data->api_data.fe_cnt_str.rate = ((tx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] - tx_ok_byte_cnt[port_sel][pre_index])/1000)*8;
    else
        fe_data->api_data.fe_cnt_str.rate = ((0xFFFFFFFF - tx_ok_byte_cnt[port_sel][pre_index]+ tx_ok_byte_cnt[port_sel][fe_cnt_cur_idx])/1000)*8;
    
	return 0;
}

int fe_api_get_rx_traffic(struct ecnt_fe_data *fe_data)
{
    unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
    int pre_index = (fe_cnt_cur_idx+FE_CNT_SECONDS-1)%FE_CNT_SECONDS;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

    if(rx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] >= rx_ok_byte_cnt[port_sel][pre_index])
        fe_data->api_data.fe_cnt_str.rate = ((rx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] - rx_ok_byte_cnt[port_sel][pre_index])/1000)*8;
    else
        fe_data->api_data.fe_cnt_str.rate = ((0xFFFFFFFF - rx_ok_byte_cnt[port_sel][pre_index]+ rx_ok_byte_cnt[port_sel][fe_cnt_cur_idx])/1000)*8;
    
	return 0;
}

int fe_api_get_tx_rate(struct ecnt_fe_data *fe_data)
{
    unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
    int pre_index = (fe_cnt_cur_idx+FE_CNT_SECONDS-1)%FE_CNT_SECONDS;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

    if(tx_ok_cnt[port_sel][fe_cnt_cur_idx] >= tx_ok_cnt[port_sel][pre_index])
        fe_data->api_data.fe_cnt_str.rate = (tx_ok_cnt[port_sel][fe_cnt_cur_idx] - tx_ok_cnt[port_sel][pre_index]);
    else
        fe_data->api_data.fe_cnt_str.rate = (0xFFFFFFFF - tx_ok_cnt[port_sel][pre_index]+ tx_ok_cnt[port_sel][fe_cnt_cur_idx]);
    
	return 0;
}

int fe_api_get_rx_rate(struct ecnt_fe_data *fe_data)
{
    unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
    int pre_index = (fe_cnt_cur_idx+FE_CNT_SECONDS-1)%FE_CNT_SECONDS;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

    if(rx_ok_cnt[port_sel][fe_cnt_cur_idx] >= rx_ok_cnt[port_sel][pre_index])
        fe_data->api_data.fe_cnt_str.rate= (rx_ok_cnt[port_sel][fe_cnt_cur_idx] - rx_ok_cnt[port_sel][pre_index]);
    else
        fe_data->api_data.fe_cnt_str.rate = (0xFFFFFFFF - rx_ok_cnt[port_sel][pre_index]+ rx_ok_cnt[port_sel][fe_cnt_cur_idx]);
    
	return 0;
}

int fe_api_get_tx_octets(struct ecnt_fe_data *fe_data)
{
    unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
    int pre_15m_index = (fe_cnt_cur_idx+1)%FE_CNT_SECONDS;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

    if(tx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] >= tx_ok_byte_cnt[port_sel][pre_15m_index])
        fe_data->api_data.fe_cnt_str.frameByteCnt = tx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] - tx_ok_byte_cnt[port_sel][pre_15m_index];
    else
        fe_data->api_data.fe_cnt_str.frameByteCnt = 0xFFFFFFFF - tx_ok_byte_cnt[port_sel][pre_15m_index] + tx_ok_byte_cnt[port_sel][fe_cnt_cur_idx];
    
	return 0;
}

int fe_api_get_rx_octets(struct ecnt_fe_data *fe_data)
{
    unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
    int pre_15m_index = (fe_cnt_cur_idx+1)%FE_CNT_SECONDS;
	
	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

    if(rx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] >= rx_ok_byte_cnt[port_sel][pre_15m_index]){
        fe_data->api_data.fe_cnt_str.frameByteCnt = rx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] - rx_ok_byte_cnt[port_sel][pre_15m_index];
    	}
    else{
        fe_data->api_data.fe_cnt_str.frameByteCnt = 0xFFFFFFFF - rx_ok_byte_cnt[port_sel][pre_15m_index] + rx_ok_byte_cnt[port_sel][fe_cnt_cur_idx] ;
    	}
    
	return 0;
}

int fe_api_get_rx_discard_counter(struct ecnt_fe_data *fe_data)
{
	unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
	int pre_15m_index = (fe_cnt_cur_idx+1)%FE_CNT_SECONDS;

	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

	if(rx_discard_cnt[port_sel][fe_cnt_cur_idx] >= rx_discard_cnt[port_sel][pre_15m_index])
        fe_data->api_data.fe_cnt_str.frameByteCnt = rx_discard_cnt[port_sel][fe_cnt_cur_idx] - rx_discard_cnt[port_sel][pre_15m_index];
    else
        fe_data->api_data.fe_cnt_str.frameByteCnt = 0xFFFFFFFF - rx_discard_cnt[port_sel][pre_15m_index] + rx_discard_cnt[port_sel][fe_cnt_cur_idx] ;
  
	return 0;
}

int fe_api_get_tx_discard_counter(struct ecnt_fe_data *fe_data)
{
	unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
	int pre_15m_index = (fe_cnt_cur_idx+1)%FE_CNT_SECONDS;

	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

	if(tx_discard_cnt[port_sel][fe_cnt_cur_idx] >= tx_discard_cnt[port_sel][pre_15m_index])
        fe_data->api_data.fe_cnt_str.frameByteCnt = tx_discard_cnt[port_sel][fe_cnt_cur_idx] - tx_discard_cnt[port_sel][pre_15m_index];
    else
        fe_data->api_data.fe_cnt_str.frameByteCnt = 0xFFFFFFFF - tx_discard_cnt[port_sel][pre_15m_index] + tx_discard_cnt[port_sel][fe_cnt_cur_idx] ;
    
	return 0;
}

int fe_api_get_rx_error_counter(struct ecnt_fe_data *fe_data)
{
	unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
	int pre_15m_index = (fe_cnt_cur_idx+1)%FE_CNT_SECONDS;

	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

	if(rx_error_cnt[port_sel][fe_cnt_cur_idx] >= rx_error_cnt[port_sel][pre_15m_index])
        fe_data->api_data.fe_cnt_str.frameByteCnt = rx_error_cnt[port_sel][fe_cnt_cur_idx] - rx_error_cnt[port_sel][pre_15m_index];
    else
        fe_data->api_data.fe_cnt_str.frameByteCnt = 0xFFFFFFFF - rx_error_cnt[port_sel][pre_15m_index] + rx_error_cnt[port_sel][fe_cnt_cur_idx] ;
    
	return 0;
}

int fe_api_get_tx_error_counter(struct ecnt_fe_data *fe_data)
{
	unsigned int port_sel = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int lan_port = fe_data->api_data.fe_cnt_str.lan_port;
	int pre_15m_index = (fe_cnt_cur_idx+1)%FE_CNT_SECONDS;

	if(gdm_sel == FE_GDM_SEL_GDMA1) {
		port_sel = lan_port;
	} else {
		port_sel = ANI_UNI_PORT_MAX-1;
	}

	if(tx_error_cnt[port_sel][fe_cnt_cur_idx] >= tx_error_cnt[port_sel][pre_15m_index])
        fe_data->api_data.fe_cnt_str.frameByteCnt = tx_error_cnt[port_sel][fe_cnt_cur_idx] - tx_error_cnt[port_sel][pre_15m_index];
    else
        fe_data->api_data.fe_cnt_str.frameByteCnt = 0xFFFFFFFF - tx_error_cnt[port_sel][pre_15m_index] + tx_error_cnt[port_sel][fe_cnt_cur_idx] ;
    
	return 0;
}

int fe_api_set_mc_vlan_global(struct ecnt_fe_data *fe_data)
{
	FE_Enable_t enable = fe_data->api_data.enable;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if( enable == FE_ENABLE ){
		feEnableMcVlan;
	}else{
		feDisableMcVlan;
	}

	return 0;
#else
	return -1;
#endif
}

int fe_api_get_mc_vlan_global(struct ecnt_fe_data *fe_data)
{
	FE_Enable_t enable = feIsMcVlanEnable;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if( enable == FE_ENABLE ){
		fe_data->api_data.enable = FE_ENABLE;
	}else{
		fe_data->api_data.enable = FE_DISABLE;
	}

	return 0;
#else
	return -1;
#endif

}

int fe_api_set_mc_vlan_table_cfg(struct ecnt_fe_data *fe_data)
{
	uint mcVlanData = 0;
	uint mcVlanCfg = 0;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if(fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id >= FE_MC_VLAN_TABLE_MAX){
		printk("Error mc_table_id = %d. Should be 0~%d\n", fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id, FE_MC_VLAN_TABLE_MAX-1);
		return -1;
	}
	
	mcVlanData = ( ((fe_data->api_data.fe_mcvlan_table_cfg.enable & MC_VLAN_DATA_ENABLE_MASK) << MC_VLAN_DATA_ENABLE_SHIFT) |
		((fe_data->api_data.fe_mcvlan_table_cfg.ppe_entry_id & MC_VLAN_DATA_PPE_ENTRY_MASK) << MC_VLAN_DATA_PPE_ENTRY_SHIFT) );


	IO_SREG(MC_VLAN_DATA, mcVlanData);
		
	mcVlanCfg = ( MC_VLAN_CFG_RWCMD | MC_VLAN_CFG_TABLE_SEL |
				((fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id & MC_VLAN_CFG_DST_TABLE_ID_MASK) << MC_VLAN_CFG_DST_TABLE_ID_SHIFT) );
	
	IO_SREG(MC_VLAN_CFG, mcVlanCfg);

	if( feChecConfigDone(MC_VLAN_CFG, MC_VLAN_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set mc vlan configuration.\n");
		return -ETIME;
	}

	return 0;
#else
	return -1;
#endif

}

int fe_api_get_mc_vlan_table_cfg(struct ecnt_fe_data *fe_data)
{
	uint mcVlanData = 0;
	uint mcVlanCfg = 0;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if(fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id >= FE_MC_VLAN_TABLE_MAX){
		printk("Error mc_table_id = %d. Should be 0~%d\n", fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id, FE_MC_VLAN_TABLE_MAX-1);
		return -1;
	}

	mcVlanCfg = ( MC_VLAN_CFG_TABLE_SEL |
				((fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id & MC_VLAN_CFG_DST_TABLE_ID_MASK) << MC_VLAN_CFG_DST_TABLE_ID_SHIFT) );
	
	IO_SREG(MC_VLAN_CFG, mcVlanCfg);

	if( feChecConfigDone(MC_VLAN_CFG, MC_VLAN_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set mc vlan configuration.\n");
		return -ETIME;
	}

	mcVlanData = IO_GREG(MC_VLAN_DATA);

	fe_data->api_data.fe_mcvlan_table_cfg.enable = ((mcVlanData >> MC_VLAN_DATA_ENABLE_SHIFT) & MC_VLAN_DATA_ENABLE_MASK);
	fe_data->api_data.fe_mcvlan_table_cfg.ppe_entry_id = ((mcVlanData >> MC_VLAN_DATA_PPE_ENTRY_SHIFT) & MC_VLAN_DATA_PPE_ENTRY_MASK);

	return 0;
#else
	return -1;
#endif

}

int fe_api_set_mc_vlan_action_cfg(struct ecnt_fe_data *fe_data)
{
	uint mcVlanData = 0;
	uint mcVlanCfg = 0;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if(fe_data->api_data.fe_mcvlan_action_cfg.mc_table_id >= FE_MC_VLAN_TABLE_MAX){
		printk("Error mc_table_id = %d. Should be 0~%d\n", fe_data->api_data.fe_mcvlan_action_cfg.mc_table_id, FE_MC_VLAN_TABLE_MAX-1);
		return -1;
	}
	if(fe_data->api_data.fe_mcvlan_action_cfg.mc_chnl_id >= FE_MC_VLAN_PORT_MAX){
		printk("Error mc_chnl_id = %d. Should be 0~%d\n", fe_data->api_data.fe_mcvlan_action_cfg.mc_chnl_id, FE_MC_VLAN_PORT_MAX-1);
		return -1;
	}

	mcVlanData = ( ((fe_data->api_data.fe_mcvlan_action_cfg.mc_vlan_op & MC_VLAN_DATA_OP_MASK) << MC_VLAN_DATA_OP_SHIFT) |
		((fe_data->api_data.fe_mcvlan_action_cfg.mc_vlan_vpm & MC_VLAN_DATA_VPM_MASK) << MC_VLAN_DATA_VPM_SHIFT) |
		((fe_data->api_data.fe_mcvlan_action_cfg.mc_vlan & MC_VLAN_DATA_VLAN_MASK) << MC_VLAN_DATA_VLAN_SHIFT) );

	IO_SREG(MC_VLAN_DATA, mcVlanData);
		
	mcVlanCfg = ( MC_VLAN_CFG_RWCMD |
				((fe_data->api_data.fe_mcvlan_action_cfg.mc_table_id & MC_VLAN_CFG_DST_TABLE_ID_MASK) << MC_VLAN_CFG_DST_TABLE_ID_SHIFT) |
				((fe_data->api_data.fe_mcvlan_action_cfg.mc_chnl_id & MC_VLAN_CFG_DST_PORT_MASK) << MC_VLAN_CFG_DST_PORT_SHIFT) );
	
	IO_SREG(MC_VLAN_CFG, mcVlanCfg);

	if( feChecConfigDone(MC_VLAN_CFG, MC_VLAN_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set mc vlan configuration.\n");
		return -ETIME;
	}

	return 0;
#else
	return -1;
#endif
}

int fe_api_get_mc_vlan_action_cfg(struct ecnt_fe_data *fe_data)
{
	uint mcVlanData = 0;
	uint mcVlanCfg = 0;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if(fe_data->api_data.fe_mcvlan_action_cfg.mc_table_id >= FE_MC_VLAN_TABLE_MAX){
		printk("Error mc_table_id = %d. Should be 0~%d\n", fe_data->api_data.fe_mcvlan_action_cfg.mc_table_id, FE_MC_VLAN_TABLE_MAX-1);
		return -1;
	}
	if(fe_data->api_data.fe_mcvlan_action_cfg.mc_chnl_id >= FE_MC_VLAN_PORT_MAX){
		printk("Error mc_chnl_id = %d. Should be 0~%d\n", fe_data->api_data.fe_mcvlan_action_cfg.mc_chnl_id, FE_MC_VLAN_PORT_MAX-1);
		return -1;
	}

	mcVlanCfg = ( ((fe_data->api_data.fe_mcvlan_action_cfg.mc_table_id & MC_VLAN_CFG_DST_TABLE_ID_MASK) << MC_VLAN_CFG_DST_TABLE_ID_SHIFT) |
				((fe_data->api_data.fe_mcvlan_action_cfg.mc_chnl_id & MC_VLAN_CFG_DST_PORT_MASK) << MC_VLAN_CFG_DST_PORT_SHIFT) );
	
	IO_SREG(MC_VLAN_CFG, mcVlanCfg);

	if( feChecConfigDone(MC_VLAN_CFG, MC_VLAN_CFG_RWCMD_DONE) != 0) {
		printk("Timeout for set mc vlan configuration.\n");
		return -ETIME;
	}

	mcVlanData = IO_GREG(MC_VLAN_DATA);

	fe_data->api_data.fe_mcvlan_action_cfg.mc_vlan_op = ((mcVlanData >> MC_VLAN_DATA_OP_SHIFT) & MC_VLAN_DATA_OP_MASK);
	fe_data->api_data.fe_mcvlan_action_cfg.mc_vlan_vpm = ((mcVlanData >> MC_VLAN_DATA_VPM_SHIFT) & MC_VLAN_DATA_VPM_MASK);
	fe_data->api_data.fe_mcvlan_action_cfg.mc_vlan = ((mcVlanData >> MC_VLAN_DATA_VLAN_SHIFT) & MC_VLAN_DATA_VLAN_MASK);
	
	return 0;
#else
	return -1;
#endif

}

int fe_api_set_mc_vlan_clear_all(struct ecnt_fe_data *fe_data)
{
	unsigned char mc_table_id = 0;
	unsigned char mc_chnl_id = 0;
	uint mcVlanData = 0;
	uint mcVlanCfg = 0;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	for( mc_table_id = 0; mc_table_id < FE_MC_VLAN_TABLE_MAX; mc_table_id++ ){
		/* if input is ECNT_FEMGR_MC_VLAN_TABLE_MAX, clear all tables */
		if( fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id <  FE_MC_VLAN_TABLE_MAX ){
			if( fe_data->api_data.fe_mcvlan_table_cfg.mc_table_id != mc_table_id )
				continue ;
		}
		
		/* clear vlan table config */
		mcVlanData = ( ((FE_DISABLE & MC_VLAN_DATA_ENABLE_MASK) << MC_VLAN_DATA_ENABLE_SHIFT) |
				((0x0 & MC_VLAN_DATA_PPE_ENTRY_MASK) << MC_VLAN_DATA_PPE_ENTRY_SHIFT) );
		
		IO_SREG(MC_VLAN_DATA, mcVlanData);
			
		mcVlanCfg = ( MC_VLAN_CFG_RWCMD | MC_VLAN_CFG_TABLE_SEL |
					((mc_table_id & MC_VLAN_CFG_DST_TABLE_ID_MASK) << MC_VLAN_CFG_DST_TABLE_ID_SHIFT) );
		
		IO_SREG(MC_VLAN_CFG, mcVlanCfg);
	
		if( feChecConfigDone(MC_VLAN_CFG, MC_VLAN_CFG_RWCMD_DONE) != 0) {
			printk("Timeout for set mc vlan clear all.\n");
			return -ETIME;
		}
		
		/* clear vlan action config */
		for( mc_chnl_id = 0; mc_chnl_id < FE_MC_VLAN_PORT_MAX; mc_chnl_id++ ){
			mcVlanData = ( ((FE_MC_VLAN_KEEP & MC_VLAN_DATA_OP_MASK) << MC_VLAN_DATA_OP_SHIFT) |
				((FE_MC_VPM_KEEP & MC_VLAN_DATA_VPM_MASK) << MC_VLAN_DATA_VPM_SHIFT) |
				((0x0000 & MC_VLAN_DATA_VLAN_MASK) << MC_VLAN_DATA_VLAN_SHIFT) );

			IO_SREG(MC_VLAN_DATA, mcVlanData);
				
			mcVlanCfg = ( MC_VLAN_CFG_RWCMD |
						((mc_table_id & MC_VLAN_CFG_DST_TABLE_ID_MASK) << MC_VLAN_CFG_DST_TABLE_ID_SHIFT) |
						((mc_chnl_id & MC_VLAN_CFG_DST_PORT_MASK) << MC_VLAN_CFG_DST_PORT_SHIFT) );
			
			IO_SREG(MC_VLAN_CFG, mcVlanCfg);

			if( feChecConfigDone(MC_VLAN_CFG, MC_VLAN_CFG_RWCMD_DONE) != 0) {
				printk("Timeout for set mc vlan clear all.\n");
				return -ETIME;
			}
		}
	}

	return 0;
#else
	return -1;
#endif

}

void ModifyRegBits(uint32_t Addr, uint32_t Data, uint32_t Offset, uint32_t Len)
{
	uint32_t Mask = 0;
	uint32_t Value;
	uint32_t i;

	for (i = 0; i < Len; i++) 
	{
		Mask |= (1 << (Offset + i));
	}

	Value = read_reg_word(Addr);
	Value &= (~Mask);
	Value |= ((Data << Offset) & Mask);

	write_reg_word(Addr, Value);
}

int fe_find_mac_patn(FE_RxMacFilter_t macfilter_cfg)
{
	int i, n = 0;
	unsigned char mac[6] = {0};
	unsigned int mac_h, mac_l;
	uint32_t mac_h_reg,mac_l_reg;
	uint32_t mac_h_base_reg,mac_l_base_reg;

	/* get reg base */
	if (FE_GDM_SEL_GDMA2 == macfilter_cfg.gdm_sel)
	{
		mac_h_reg = GDM2_RX_MAC_FILTER_PATN_H(n);
		mac_l_reg = GDM2_RX_MAC_FILTER_PATN_L(n);
	}
	else if (FE_GDM_SEL_GDMA3 == macfilter_cfg.gdm_sel)
	{
		mac_h_reg = GDM3_RX_MAC_FILTER_PATN_H(n);
		mac_l_reg = GDM3_RX_MAC_FILTER_PATN_L(n);
	}
	else
	{
		printk("%s line %d parameter error\n", __FUNCTION__,__LINE__);
		return 0;
	}

	mac_h_base_reg = mac_h_reg;
	mac_l_base_reg = mac_l_reg;
	
	for (i=0; i<GDM_RX_MAC_FILTER_MAX_NUM; i++)
	{
		mac_h_reg = (mac_h_base_reg + (i*8));
		mac_l_reg = (mac_l_base_reg + (i*8));
		mac_h = read_reg_word(mac_h_reg);
		mac_l = read_reg_word(mac_l_reg);

		mac[0] = ((mac_h >> 24) & 0xff);
		mac[1] = ((mac_h >> 16) & 0xff);
		mac[2] = ((mac_h >> 8) & 0xff);
		mac[3] = (mac_h & 0xff);
		mac[4] = ((mac_l >> 8) & 0xff);
		mac[5] = (mac_l & 0xff);

		/* find mac */
		if (0 == memcmp(macfilter_cfg.mac, mac, 6))
		{
			return i;
		}
	}

	return -1;
}

int fe_set_mac_patn(int index, FE_RxMacFilter_t macfilter_cfg)
{
	int i, n = 0;
	unsigned int mac_h, mac_l;
	uint32_t mac_h_reg,mac_l_reg;
	uint32_t mac_h_base_reg,mac_l_base_reg;

	/* set all mac or one mac */
	if (GDM_RX_MAC_FILTER_MAX_NUM == index)
	{
		n = 0;
	}
	else
	{
		n = index;
	}
	
	/* get reg base */
	if (FE_GDM_SEL_GDMA2 == macfilter_cfg.gdm_sel)
	{
		mac_h_reg = GDM2_RX_MAC_FILTER_PATN_H(n);
		mac_l_reg = GDM2_RX_MAC_FILTER_PATN_L(n);
	}
	else if (FE_GDM_SEL_GDMA3 == macfilter_cfg.gdm_sel)
	{
		mac_h_reg = GDM3_RX_MAC_FILTER_PATN_H(n);
		mac_l_reg = GDM3_RX_MAC_FILTER_PATN_L(n);
	}
	else
	{
		printk("%s line %d: parameter error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* get mac addr */
	if ((MAC_FILTER_DEL == macfilter_cfg.cmd_id) || (MAC_FILTER_DISABLE == macfilter_cfg.cmd_id))
	{
		mac_h = 0;
		mac_l = 0;
	}
	else
	{
		mac_h = ((macfilter_cfg.mac[0]<<24) | (macfilter_cfg.mac[1]<<16) | (macfilter_cfg.mac[2]<<8) | (macfilter_cfg.mac[3]));
		mac_l = ((macfilter_cfg.mac[4]<<8) | (macfilter_cfg.mac[5]));
	}

	/* set mac patn */
	if (GDM_RX_MAC_FILTER_MAX_NUM == index)
	{
		mac_h_base_reg = mac_h_reg;
		mac_l_base_reg = mac_l_reg;
		
		for (i=0; i<GDM_RX_MAC_FILTER_MAX_NUM; i++)
		{
			mac_h_reg = (mac_h_base_reg + (i*8));
			mac_l_reg = (mac_l_base_reg + (i*8));
			write_reg_word(mac_h_reg, mac_h);
			write_reg_word(mac_l_reg, mac_l);
		}
	}
	else
	{
		write_reg_word(mac_h_reg, mac_h);
		write_reg_word(mac_l_reg, mac_l);
	}

	return 0;
}

int fe_set_mac_filter(int index, FE_RxMacFilter_t macfilter_cfg)
{
	uint32_t filter_en_reg;
	uint32_t data;
	unsigned char offset,len;

	/* get reg base */
	if (FE_GDM_SEL_GDMA2 == macfilter_cfg.gdm_sel)
	{
		filter_en_reg = GDM2_RX_MAC_FILTER_EN;
	}
	else if (FE_GDM_SEL_GDMA3 == macfilter_cfg.gdm_sel)
	{
		filter_en_reg = GDM3_RX_MAC_FILTER_EN;
	}
	else
	{
		printk("%s line %d: parameter error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* disable mac filter */
	if (MAC_FILTER_DISABLE == macfilter_cfg.cmd_id)
	{
		data = 0;
		offset = 0;
		len = GDM_RX_MAC_FILTER_MAX_NUM;
	}
	else if (MAC_FILTER_DEL == macfilter_cfg.cmd_id)
	{
		data = 0;
		offset = index;
		len = 1;
	}
	else
	{
		data = 1;
		offset = index;
		len = 1;
	}

	ModifyRegBits(filter_en_reg, data, offset, len);
	/* set mac patn */
	fe_set_mac_patn(index, macfilter_cfg);
		
	return 0;
}

int fe_set_mac_filter_para(FE_RxMacFilter_t macfilter_cfg)
{
	unsigned char pkt_type_bit = 0;
	uint32_t filter_cfg_reg,filter_en_reg;

	if (FE_GDM_SEL_GDMA2 == macfilter_cfg.gdm_sel)
	{
		filter_cfg_reg = GDM2_RX_MAC_FILTER_CFG;
	}
	else if (FE_GDM_SEL_GDMA3 == macfilter_cfg.gdm_sel)
	{
		filter_cfg_reg = GDM3_RX_MAC_FILTER_CFG;
	}
	else
	{
		printk("%s line %d: parameter error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* config  rx mac filter parameters */
	ModifyRegBits(filter_cfg_reg, macfilter_cfg.list_type, LIST_TYPE_BIT, 1);
	ModifyRegBits(filter_cfg_reg, macfilter_cfg.action, ACTION_BIT, 1);
	ModifyRegBits(filter_cfg_reg, macfilter_cfg.mac_sel, MAC_SELECT_BIT, 1);

	/* filter disable */
	if (MAC_FILTER_DISABLE == macfilter_cfg.cmd_id)
	{
		ModifyRegBits(filter_cfg_reg, macfilter_cfg.cmd_id, PKT_TYPE_OC_BIT, 4);
		return 0;
	}

	/* check packet type */
	if (FE_TYPE_OC == macfilter_cfg.pkt_type)
	{
		pkt_type_bit = PKT_TYPE_OC_BIT;
	}
	else if (FE_TYPE_MC == macfilter_cfg.pkt_type)
	{
		pkt_type_bit = PKT_TYPE_MC_BIT;
	}
	else if (FE_TYPE_BC == macfilter_cfg.pkt_type)
	{
		pkt_type_bit = PKT_TYPE_BC_BIT;
	}
	else if (FE_TYPE_UC == macfilter_cfg.pkt_type)
	{
		pkt_type_bit = PKT_TYPE_UC_BIT;
	}
	else
	{
		printk("%s line %d: parameter error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ModifyRegBits(filter_cfg_reg, macfilter_cfg.cmd_id, pkt_type_bit, 1);

	return 0;
}

int fe_get_mac_filter_idx(FE_RxMacFilter_t macfilter_cfg)
{
	int i = 0;
	uint32_t reg_val = 0;
	uint32_t filter_en_reg;

	if (FE_GDM_SEL_GDMA2 == macfilter_cfg.gdm_sel)
	{
		filter_en_reg = GDM2_RX_MAC_FILTER_EN;
	}
	else if (FE_GDM_SEL_GDMA3 == macfilter_cfg.gdm_sel)
	{
		filter_en_reg = GDM3_RX_MAC_FILTER_EN;
	}
	else
	{
		printk("%s line %d: parameter error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* get un-used index */
	reg_val = read_reg_word(filter_en_reg);
	for (i = 0; i<GDM_RX_MAC_FILTER_MAX_NUM; i++)
	{
		if (0 == (reg_val & (1 << i)))
		{
			return i;
		}
	}

	return -1;
}

int fe_check_mac_filter_empty(FE_RxMacFilter_t macfilter_cfg)
{
	uint32_t reg_val = 0;
	uint32_t filter_en_reg;

	if (FE_GDM_SEL_GDMA2 == macfilter_cfg.gdm_sel)
	{
		filter_en_reg = GDM2_RX_MAC_FILTER_EN;
	}
	else if (FE_GDM_SEL_GDMA3 == macfilter_cfg.gdm_sel)
	{
		filter_en_reg = GDM3_RX_MAC_FILTER_EN;
	}
	else
	{
		printk("%s line %d: parameter error\n", __FUNCTION__, __LINE__);
		return -1;
	}

	reg_val = read_reg_word(filter_en_reg);

	return reg_val;
}

int fe_del_mac_filter(FE_RxMacFilter_t macfilter)
{
	uint32_t filter_en = 0;
	int mac_exist_idx = -1;

	/* check the mac exist or not */
	mac_exist_idx = fe_find_mac_patn(macfilter);
	if (mac_exist_idx < 0)
	{
		printk("the mac not exist\n");
		return -1;
	}

	fe_set_mac_filter(mac_exist_idx, macfilter);

	/* check empty or not */
	filter_en = fe_check_mac_filter_empty(macfilter);
	if (0 == filter_en)
	{
		macfilter.cmd_id = MAC_FILTER_DISABLE;
		macfilter.list_type = 0;
		macfilter.action = 0;
		macfilter.mac_sel = 0;
		fe_set_mac_filter_para(macfilter);
	}

	return 0;
}

int fe_add_mac_filter(FE_RxMacFilter_t macfilter)
{
	int filter_en_idx = 0;
	int mac_exist_idx = -1;

	/* check the mac exist or not */
	mac_exist_idx = fe_find_mac_patn(macfilter);
	if (mac_exist_idx >= 0)
	{
		printk("the mac exist\n");
		return -1;
	}

	/* get per-mac enable index */
	filter_en_idx = fe_get_mac_filter_idx(macfilter);
	if (filter_en_idx < 0)
	{
		printk("there is no filter index\n");
		return -1;
	}

	fe_set_mac_filter(filter_en_idx, macfilter);

	return 0;
}

int fe_enable_mac_filter(FE_RxMacFilter_t macfilter)
{
	fe_set_mac_filter_para(macfilter);

	return 0;
}

int fe_disable_mac_filter(FE_RxMacFilter_t macfilter)
{
	fe_set_mac_filter_para(macfilter);
	fe_set_mac_filter(GDM_RX_MAC_FILTER_MAX_NUM, macfilter);

	return 0;
}

int fe_api_set_rx_mac_filter(struct ecnt_fe_data *fe_data)
{
	int ret = 0;
	FE_RxMacFilter_t macfilter_cfg;

	if (!isEN7523)
	{
		return -1;
	}

	memset(&macfilter_cfg,0,sizeof(macfilter_cfg));
	memcpy(&macfilter_cfg, &(fe_data->api_data.macfilter_cfg), sizeof(fe_data->api_data.macfilter_cfg));

	if ((macfilter_cfg.cmd_id) >= MAC_FILTER_MAX_NUM)
	{
		printk("Error: cmd_id %d, exceed max number: %d", macfilter_cfg.cmd_id, MAC_FILTER_MAX_NUM);
		return -1;
	}

	ret = mac_filter_operation[macfilter_cfg.cmd_id](macfilter_cfg);

	return ret;
}


int fe_api_xfi_link_change(struct ecnt_fe_data *fe_data)
{
    unsigned int reg, value;
    static int disable_flag = 0;
    static unsigned int gdm3_fwd_cfg = 0;
    static unsigned int gdm3_lpbk_cfg = 0;
    static unsigned int qdma_chn_dis_cfg = 0;
    int max_try_cnt = 1000;
    XFI_LINK_STATUS_t status = fe_data->api_data.status;
    int i;
    
    if(XSI_PHY_LINK_DOWN == status)
    {
        if(disable_flag)
            return 0;

        // txmpi mask enable
        value = IO_GREG(0xbfa04000);
        value |= (1<<9);
        IO_SREG(0xbfa04000, value);
       
        
        // disable channel 12 for xfi tx
        value = IO_GREG(0xbfb540ac);
        qdma_chn_dis_cfg = value;
        value &= (~0xFF);
        value |= 0xFF;
        IO_SREG(0xbfb540ac, value);

        do{
            mdelay(1);
            max_try_cnt--;   
            if(0 == (max_try_cnt)){
                printk("check over 1 second\n");
                break;
            }
            IO_SREG(PSE_PORT_OQ_STA4, 0x20000);
            value = (IO_GREG(PSE_PORT_OQ_STA5) & 0x3FFF); 
            
        }while(value);
        
        // txmpi mask disable after OQ3 buffer is released
        value = IO_GREG(0xbfa04000);
        value &= ~(1<<9);
        IO_SREG(0xbfa04000, value);
        disable_flag = 1;
    }
    else if(XSI_PHY_LINK_UP == status)
    {
        if(!disable_flag)
            return 0;
        // recover channel 12 for xfi 
        IO_SREG(0xbfb540ac, qdma_chn_dis_cfg);

        // txmpi mask disable after OQ3 buffer is released when xfi phy up for double confirm.
        value = IO_GREG(0xbfa04000);
        value &= ~(1<<9);
        IO_SREG(0xbfa04000, value);
        disable_flag = 0; 
    }
    else
        return -1;

    return 0;
}

int fe_api_set_gdma_misc_config(struct ecnt_fe_data *fe_data)
{
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	uint mode = fe_data->api_data.fe_misc_cfg;

    if(isEN7523){
		if (FE_GDM_SEL_GDMA2 == gdm_sel){
			if(FE_MISC_CONFIG_GPON == mode){
				IO_SBITS(GDMA_MISC_CFG,GDMA2_RLS_MODE_BIT);
			}else{
				IO_CBITS(GDMA_MISC_CFG,GDMA2_RLS_MODE_BIT);
			}
		}
	}
	else{
		return 0;
	}
	
	return 0;
}

int fe_api_set_aewan_fwdfq(struct ecnt_fe_data *fe_data)
{
	if(SUPPORT_AEWAN_FWDFQ){
		FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
		uint value = fe_data->reg_val;

		if(FE_GDM_SEL_GDMA2 == gdm_sel)
			IO_SREG(GDM2_AEWANFWD_FQ,value);
	}
	return 0;
}

int fe_api_set_aewan_ifcdisable(struct ecnt_fe_data *fe_data)
{
	if(isEN7523){
		uint value = fe_data->reg_val;
		IO_CBITS(FE_AEWANIFC_CFG,value);	
	}
	return 0;
}

int fe_api_set_gdm2_sptag_for_loopback(struct ecnt_fe_data *fe_data)
{
#ifdef TCSUPPORT_CPU_EN7523
	FE_Enable_t enable = fe_data->api_data.enable;

	if(enable == FE_ENABLE){
		fe_set_gdm2_sptag_for_loopback();
	}else{
		fe_set_gdm2_sptag_for_loopback_recover();
	}
#endif

	return 0;
}

int fe_api_set_tunnel_cfg(struct ecnt_fe_data *fe_data)
{
	unsigned int tableIndex = fe_data->api_data.fe_tunnel_cfg.tableIndex;
	unsigned int offset = fe_data->api_data.fe_tunnel_cfg.offset;
	unsigned int value0 = fe_data->api_data.fe_tunnel_cfg.value0;
	unsigned int value1 = fe_data->api_data.fe_tunnel_cfg.value1;
	unsigned int value2 = fe_data->api_data.fe_tunnel_cfg.value2;
	unsigned int value3 = fe_data->api_data.fe_tunnel_cfg.value3;
	
	if(SUPPORT_TUNNEL_OFFLOAD)
	{
		fe_set_tunnel_info(tableIndex, offset, value0, value1, value2, value3);
	}
	
	return 0;
}

int fe_api_set_gdm_sptag_for_extswitch(struct ecnt_fe_data *fe_data)
{
#ifdef TCSUPPORT_CPU_EN7523
	int gdm_idx;
	FE_Enable_t enable = fe_data->api_data.enable;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int channel = (int)(fe_data->channel);

	if(gdm_sel == FE_GDM_SEL_GDMA1){
        gdm_idx = 1;
	}else if(gdm_sel == FE_GDM_SEL_GDMA2){
        gdm_idx = 2;
	}else{
        gdm_idx = 3;
	}

	if(enable == FE_ENABLE){
		fe_set_gdm_sptag_for_extswitch(gdm_idx, channel);
	}else{
		fe_set_gdm_sptag_for_extswitch_recover(gdm_idx, channel);
	}
#endif

	return 0;
}

int fe_api_pse_oq_rsv_en(struct ecnt_fe_data *fe_data)
{
#ifdef TCSUPPORT_CPU_EN7581
	unsigned int port = fe_data->api_data.fe_oq_rsv_en.port;
    unsigned int channel = fe_data->api_data.fe_oq_rsv_en.channel;
    unsigned int enable = fe_data->api_data.fe_oq_rsv_en.enable;

    /*keep P2 oq0~oq5 rsv*/
    if((FE_DP_GDM2 == port)&&(channel >= 0)&&(channel <= 5))
            return 0;
    
    if(enable)
        fe_set_per_oq_rsv(port,channel,PSE_RSV_PAGE_DEFAULT);
    else
        fe_set_per_oq_rsv(port,channel,0); 
    
#endif

	return 0;
}

int fe_api_set_hsgmii_rx_port_ratelimit(struct ecnt_fe_data *fe_data)
{
	uint hsgmii_index = 0;
	uint rate = 0;
	uint mode = 0;

	hsgmii_index = fe_data->index;
	rate = fe_data->api_data.rate_cfg.rate;
	mode = fe_data->api_data.rate_cfg.mode;

	HSGMII_LAN_INGRESS_PORT_RATELIMIT(hsgmii_index, rate, mode);

	return 0;
}

int fe_api_set_mbi_arb_rst(struct ecnt_fe_data *fe_data)
{
	uint32 val = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;

	if(ARBITER_SUPPORT)
	{
		if(gdm_sel == FE_GDM_SEL_GDMA3)
		{	
			//reset gdm3 arbiter
	    	write_reg_word(FE_RST_GLO,FE_GDM3_MBI_ARB_RST_OFFSET);
		}
		else if(gdm_sel == FE_GDM_SEL_GDMA4)
		{
			//reset gdm4 arbiter
	    	write_reg_word(FE_RST_GLO,FE_GDM4_MBI_ARB_RST_OFFSET);
		}
		else
		{
			printk("gdm_sel must be 2/3, gdm_sel = %d\n",gdm_sel);
			return 0;
		}
	}

	return 0;
}

int fe_api_set_rmbi_frag(struct ecnt_fe_data *fe_data)
{
	uint32 val = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int channel = (int)(fe_data->channel);
	uint frag_size = fe_data->reg_val;

	if(SERDES_EOF)
	{
		if((gdm_sel == FE_GDM_SEL_GDMA4) && (channel == 0))
		{
			val = read_reg_word(FE_GDM4_RMBI_FRAG);
			val = val & (~GDM4_SGMII0_FRAG_SIZE_MASK) | (frag_size<<GDM4_SGMII0_FRAG_SIZE_SHIFT);
	    	write_reg_word(FE_GDM4_RMBI_FRAG, val);
		}
		else if((gdm_sel == FE_GDM_SEL_GDMA4) && (channel == 1))
		{
			val = read_reg_word(FE_GDM4_RMBI_FRAG);
			val = val & (~GDM4_SGMII1_FRAG_SIZE_MASK) | (frag_size<<GDM4_SGMII1_FRAG_SIZE_SHIFT);
	    	write_reg_word(FE_GDM4_RMBI_FRAG, val);
		}
		else
		{
			printk("gdm_sel must be 3, channel must be 0/1, gdm_sel = %d, channel=%d\n",gdm_sel,channel);
			return 0;
		}
	}

	return 0;
}

int fe_api_set_tmbi_frag(struct ecnt_fe_data *fe_data)
{
	uint32 val = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	int channel = (int)(fe_data->channel);
	uint frag_size = fe_data->reg_val;

	if(SERDES_EOF)
	{
		if((gdm_sel == FE_GDM_SEL_GDMA4) && (channel == 0))
		{
			val = read_reg_word(FE_GDM4_TMBI_FRAG);
			val = val & (~GDM4_SGMII0_FRAG_SIZE_MASK) | (frag_size<<GDM4_SGMII0_FRAG_SIZE_SHIFT);
	    	write_reg_word(FE_GDM4_TMBI_FRAG, val);
		}
		else if((gdm_sel == FE_GDM_SEL_GDMA4) && (channel == 1))
		{
			val = read_reg_word(FE_GDM4_TMBI_FRAG);
			val = val & (~GDM4_SGMII1_FRAG_SIZE_MASK) | (frag_size<<GDM4_SGMII1_FRAG_SIZE_SHIFT);
	    	write_reg_word(FE_GDM4_TMBI_FRAG, val);
		}
		else
		{
			printk("gdm_sel must be 3, channel must be 0/1, gdm_sel = %d, channel=%d\n",gdm_sel,channel);
			return 0;
		}
	}

	return 0;
}
static inline uint8 fe_api_judge_gdma_para(uint32 gdm_sel, uint32 *fe_gdm_sel_gdma, uint32 *gdma_fwd_reg, uint32 *vip_port, uint32 *ifc_port)
{
	uint32 gdma_chanel_id = 0, gdma_sport_offset = 0;

	//pcie0 and pcie1 use gdm3, eth and usb use gdm4. 
	switch((xsi_sel_type)gdm_sel){
		case XSI_PCIE0_IDX:
			*fe_gdm_sel_gdma = FE_GDM_SEL_GDMA3;
			*gdma_fwd_reg = GDMA3_FWD_CFG;
			gdma_chanel_id = 0; //pcie0_serdes
			gdma_sport_offset = 6;
			break;
		case XSI_PCIE1_IDX:
			*fe_gdm_sel_gdma = FE_GDM_SEL_GDMA3;
			*gdma_fwd_reg = GDMA3_FWD_CFG;
			gdma_chanel_id = 1; //pcie1_serdes
			gdma_sport_offset = 6;
			break;
		case XSI_USB_IDX:
			*fe_gdm_sel_gdma = FE_GDM_SEL_GDMA4;
			*gdma_fwd_reg = GDMA4_FWD_CFG;
			gdma_chanel_id = 1; 	//usb_serdes
			gdma_sport_offset = 8;
			break;
		case XSI_AE_IDX:
			*fe_gdm_sel_gdma = FE_GDM_SEL_GDMA2;
			*gdma_fwd_reg = GDMA2_FWD_CFG;
			gdma_chanel_id = 0; 	//pon_serdes
			gdma_sport_offset = 0;
			*vip_port = 1<<2;
			*ifc_port = *vip_port ;
			return 1;
		case XSI_ETH_IDX:
			*fe_gdm_sel_gdma = FE_GDM_SEL_GDMA4;
			*gdma_fwd_reg = GDMA4_FWD_CFG;
			gdma_chanel_id = 0;     //eth_serdes
			gdma_sport_offset = 8;
			break;
		case GSW_ETH_IDX:
			*fe_gdm_sel_gdma = FE_GDM_SEL_GDMA1;
			*gdma_fwd_reg = GDMA1_FWD_CFG;
			gdma_chanel_id = 0;     //switch_serdes
			gdma_sport_offset = 1;
			break;
		default:
			printk("func[%s] Error:serdes index(%d) not between [0,1,2,4,5], not 3(ae_wan)!\n",__func__,gdm_sel);
			return 0;
	}
	*vip_port = (1<<(gdma_chanel_id + gdma_sport_offset + 0x10));
	*ifc_port = *vip_port ;

	return 1;
}
int fe_api_set_gdma_enable(struct ecnt_fe_data *fe_data)
{
	uint32 reg_val = 0, fe_gdm_sel_gdma = 0, gdma_fwd_reg = 0, vip_port = 0, ifc_port = 0;
 	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	uint32 fe_gdma_sel = 0xffffffff;

	if(0 == fe_api_judge_gdma_para(gdm_sel,&fe_gdm_sel_gdma,&gdma_fwd_reg, &vip_port, &ifc_port)){
		return 0;
	}

#if SUPPORT_VIP_IFC_EN
		//enable gdm of vip
		reg_val = read_reg_word(FE_VIP_PORT_EN);
		reg_val |= vip_port;
		write_reg_word(FE_VIP_PORT_EN,reg_val);
		
		//enable gdm of ifc
		reg_val = read_reg_word(FE_IFC_PORT_EN);
		reg_val |=  ifc_port;
		write_reg_word(FE_IFC_PORT_EN,reg_val);

		if(fe_gdm_sel_gdma == FE_GDM_SEL_GDMA1)
			fe_gdma_sel = fe_gdma1_sel;
		else if(fe_gdm_sel_gdma == FE_GDM_SEL_GDMA2)
			fe_gdma_sel = fe_gdma2_sel;
		else
			;
		
		//set gdm to ppe or ppe2
		if(SUPPORT_PPE2 && WAN_VALID)
		{
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_UC, fe_gdma_sel==0xffffffff ? FE_DP_PPE2 : (fe_gdma_sel>>12)&0xf);
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_BC, fe_gdma_sel==0xffffffff ? FE_DP_PPE2 : (fe_gdma_sel>>8)&0xf);
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_MC, fe_gdma_sel==0xffffffff ? FE_DP_PPE2 : (fe_gdma_sel>>4)&0xf);
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_OC, fe_gdma_sel==0xffffffff ? FE_DP_PPE2 : fe_gdma_sel&0xf);
		}
		else
		{
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_UC, fe_gdma_sel==0xffffffff ? FE_DP_PPE : (fe_gdma_sel>>12)&0xf);
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_BC, fe_gdma_sel==0xffffffff ? FE_DP_PPE : (fe_gdma_sel>>8)&0xf);
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_MC, fe_gdma_sel==0xffffffff ? FE_DP_PPE : (fe_gdma_sel>>4)&0xf);
			FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_OC, fe_gdma_sel==0xffffffff ? FE_DP_PPE : fe_gdma_sel&0xf);
		}
#endif
	return 0;
}
int fe_api_set_gdma_disable(struct ecnt_fe_data *fe_data)
{
	uint32 reg_val = 0, fe_gdm_sel_gdma = 0, gdma_fwd_reg = 0, vip_port = 0, ifc_port = 0;
 	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;

	if(0 == fe_api_judge_gdma_para(gdm_sel,&fe_gdm_sel_gdma,&gdma_fwd_reg, &vip_port, &ifc_port)){
		return 0;
	}

#if SUPPORT_VIP_IFC_EN
		//disable gdm of vip
		reg_val = read_reg_word(FE_VIP_PORT_EN);
		reg_val &= (~vip_port) ;
		write_reg_word(FE_VIP_PORT_EN,reg_val);
		
		//disable gdm of ifc
		reg_val = read_reg_word(FE_IFC_PORT_EN);
		reg_val &=  (~ifc_port)  ;
		write_reg_word(FE_IFC_PORT_EN,reg_val);

		if(fe_gdm_sel_gdma == FE_GDM_SEL_GDMA1)
			FE_API_GET_FW_CFG(FE_GDM_SEL_GDMA1, &fe_gdma1_sel);
		else if(fe_gdm_sel_gdma == FE_GDM_SEL_GDMA2)
			FE_API_GET_FW_CFG(FE_GDM_SEL_GDMA2, &fe_gdma2_sel);
		else
			;
		
		//set gdm to drop
		FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_UC, FE_DP_DROP);
		FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_BC, FE_DP_DROP);
		FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_MC, FE_DP_DROP);
		FE_API_SET_FW_CFG(fe_gdm_sel_gdma, FE_TYPE_OC, FE_DP_DROP);
#endif
	return 0;
}


int fe_api_get_chn_rls(struct ecnt_fe_data *fe_data)
{
	uint32 val = 0;
	FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
	
	if(SERDES_EOF)
	{
		if(gdm_sel == FE_GDM_SEL_GDMA4)
		{
			val = read_reg_word(GDMA4_CHN_RLS);
			fe_data->reg_val = val & (1 << MBI_RX_BUSY);
		}
		else if(gdm_sel == FE_GDM_SEL_GDMA3)
		{
			val = read_reg_word(GDMA3_CHN_RLS);
			fe_data->reg_val = val & (1 << MBI_RX_BUSY);
		}
		else
		{
			printk("%s:%d gdm_sel must be 2/3, gdm_sel = %d\n",__FUNCTION__,__LINE__,gdm_sel);
			return 0;
		}
	}

	return 0;
}


int fe_api_set_chn_retire_action(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580)
    unsigned int      reg = 0;
    unsigned int    value = 0;
    unsigned int      chn = fe_data->channel;
    FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    if (gdm_sel == FE_GDM_SEL_GDMA1)
    {
        reg = GDMA1_CHN_RLS;
    }
    else if(gdm_sel == FE_GDM_SEL_GDMA2)
    {
        reg = GDMA2_CHN_RLS;
    }
    else
    {
        return -1;
    }
    chn &= 0x1f;
    value = (chn << GDMA_CHN_RLS_CHN_OFFSET) | (1 << GDMA_CHN_RLS_EN_OFFSET);
    write_reg_word(reg, value);
#endif

    return 0;
}

int fe_api_set_chn_retire_done(struct ecnt_fe_data *fe_data)
{
#if defined(TCSUPPORT_CPU_EN7580)
    unsigned int   reg = 0;
    unsigned int value = 0;
    uint      chn_done = 0;
    uint           chn = fe_data->channel;
    FE_Gdma_Sel_t gdm_sel = fe_data->gdm_sel;
    if (gdm_sel == FE_GDM_SEL_GDMA1)
    {
        reg = GDMA1_CHN_RLS;
    }
    else if(gdm_sel == FE_GDM_SEL_GDMA2)
    {
        reg = GDMA2_CHN_RLS;
    }
    else
    {
        return -1;
    }
    chn &= 0x1f;
    value = read_reg_word(reg);
    chn_done = (value >> GDMA_CHN_RLS_CHN_OFFSET) & 0x1f;
    if(chn_done == chn)
    {
        write_reg_word(reg, 0);
    }
    else
    {
        return -1;
    }
#endif

    return 0;
}

int fe_api_set_qbi_fttr_chn_disable(struct ecnt_fe_data *fe_data)
{
    uint qbi_fttr_chn_reg = 0x1fb52068;
    write_reg_word(qbi_fttr_chn_reg,0);
    return 0;
}

int fe_api_set_force_slow_enable(struct ecnt_fe_data *fe_data)
{
#if SUPPORT_FORCE_TO_SLOW
	FE_Enable_t enable = fe_data->api_data.enable;
	unsigned int val_Hthd = 0, val_Lthd = 0, val_cfg;
	QDMA_TxQDynCngstTotalThrh_T total_threshold;
	
	memset(&total_threshold, 0, sizeof(total_threshold));
	QDMA_API_GET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_LAN, &total_threshold);
	
	if(enable == FE_ENABLE)
	{
		val_cfg = FORCE_SLOW_EN | REFER_OQ_BUF | FIX_DUTY | (0xf << FAST_DUTY_SHIFT) 
				 | (0x4 << SLOW_DUTY_SHIFT) | (0x20 << BUFFER_CHECK_GAP_SHIFT);
		write_reg_word(FORCE_SLOW_CFG,val_cfg);		

		val_Lthd = (read_reg_word(PSE_SHARE_BUF_STA) & 0x7fff)/3;
		val_Hthd = (read_reg_word(PSE_SHARE_BUF_STA) & 0x7fff)/3*2;
		FeSetShareUsedLthd(val_Lthd);
		FeSetShareUsedHthd(val_Hthd);
		cdm1SetFaqDisable;
		
	}
	else
	{
		FeForceSlowDisable();
		cdm1SetFaqEnable;

	}
#endif	
    return 0;
}

int fe_api_set_force_slow_duty(struct ecnt_fe_data *fe_data)
{
#if SUPPORT_FORCE_TO_SLOW
	uint fix_duty  = fe_data->api_data.force_slow_cfg.fix_duty_enable;
	uint fast_duty = fe_data->api_data.force_slow_cfg.fast_duty;
	uint slow_duty = fe_data->api_data.force_slow_cfg.slow_duty;
	uint duty = (fast_duty << 8) + slow_duty;

	if((fast_duty > 0xf) || (slow_duty > 0xff))
	{
		printk("fast_duty should smaller than 15, slow_duty should smaller than 255\n");
		return 0;
	}
	
	if(fix_duty == FE_ENABLE)
	{
		FeForceSlowFixDutyEnable();
		FeSetForceSlowDuty(duty);
	}
	else
	{
		FeForceSlowFixDutyDisable();
		FeSetForceSlowDuty(duty);
	}
#endif
    return 0;
}

int fe_api_set_vip_rxq_selection(struct ecnt_fe_data *fe_data)
{
    unsigned char ring_idx = 0;
    
    ring_idx = fe_data->channel & 0xf;
	if (SUPPORT_VIP)
	{
        IO_SMASK(CDMA1_FWD_CFG, VIP_QSEL_MASK , VIP_QSEL_SHIFT , ring_idx);
        IO_SMASK(CDMA2_FWD_CFG, VIP_QSEL_MASK , VIP_QSEL_SHIFT , ring_idx);
	}

	return 0;
}

int fe_api_set_vip_for_tcp_speedtest(struct ecnt_fe_data *fe_data)
{
	ushort operation = fe_data->api_data.vip.type;
	ushort src = fe_data->api_data.vip.src;
	ushort dst = fe_data->api_data.vip.dst;
    unchar mode = 0;

	if(!SUPPORT_IFC_NUM_NOT_ENOUGH)
        return 0;
    
    if(TCSUPPORT_QDMA_WAN_FOR_ETHER_VAL) 
        mode = 3;
    else 
        mode = 7;

    if(operation == 1)
        fe_add_vip_tcp(src, dst, mode);
    else if(operation == 0)
        fe_del_vip_tcp(src, dst, mode);
	
    return 1;
}

long fe_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) 
{
	int ret = 0 ;
	struct ecnt_fe_data data;
	struct ecnt_pcie_data* puser = (struct ecnt_fe_data*)arg;

	if (cmd >= FE_FUNCTION_MAX_NUM)
		return -1;
	
	memset(&data,0,sizeof(struct ecnt_fe_data));
	copy_from_user(&data, puser ,sizeof(struct ecnt_fe_data));
	if(data.function_id >= FE_FUNCTION_MAX_NUM) {
		printk("fe_data->function_id is %d, exceed max number: %d", data.function_id, FE_FUNCTION_MAX_NUM);
		return -1;
	}

	ret = fe_operation[data.function_id](&data);
	copy_to_user(puser,&data,sizeof(struct ecnt_fe_data));
	
	return ret;
}

EXPORT_SYMBOL(fe_get_ok_cnt);
EXPORT_SYMBOL(fe_reg_read);
EXPORT_SYMBOL(fe_reg_write);
EXPORT_SYMBOL(fe_reg_modify_bits);
EXPORT_SYMBOL(qdma_set_channel_retire);
EXPORT_SYMBOL(qdma_dma_mode);

