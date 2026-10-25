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
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <asm/tc3162/tc3162.h>
#include <linux/version.h>
#include <linux/mtd/rt_flash.h>
#include <linux/libcompileoption.h>

#include "qdma_bmgr.h"
#include "qdma_dev.h"
#include "qdma_dvt.h"
#include "qdma_virtual_if.h"
#include "qdma_frag_reassemble.h"
#include "qdma_verify.h"
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_ifc.h>
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
#include <linux/soft_dma.h>
#endif/*TCSUPPORT_COMPILE*/
#include "qdma_ic_dis.h"
#include "qdma_pri.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define BROADCAST_OFFSET 0x10
#define MULTICAST_OFFSET 0x20
#define UNKNOWN_UNICAST_OFFSET 0x30

/*  the scale limitValue is kbps */
#define RATELIMIT_MAX 0xFFFF
#define UNIT_MAX 64

#define QOS_DMAWRR_USERDEFINE        0x01
#define ATM_VC_MAX				10

#define MIN(a, b) ((a)<(b)?(a):(b))

#define VLAN_HLEN			4
#define ETH_CRC			4
#define RALINK_PPE_BASE		0xBFB50c00
#define PPE_TPID	(RALINK_PPE_BASE + 0x318)

#if defined(TCSUPPORT_CPU_EN7580)
#if defined(TCSUPPORT_CPU_EN7581)
#define CONFIG_METER_GP0_INIT_STATE        (0xFFFFFFFF)
#else
#define CONFIG_METER_GP0_INIT_STATE        (0xFFFF)  /*bit 0-31 mapping meter 0-31, meter 0-15 default bind to ring 0-15 */
#endif
#define CONFIG_METER_GP1_INIT_STATE        (0x0)     /*bit 0-31 mapping meter 32-63,  meter32-37 mean 6 queue in qos car mode  */
#define CONFIG_METER_GP2_INIT_STATE        (0x0)     /*bit 0-31 mapping meter 64-95 */
#define CONFIG_METER_GP3_INIT_STATE        (0x0)     /*bit 0-30mapping meter 96-126, bit31 or meter 127 mean no ratelimit*/

#define CONFIG_ACNT0_INIT_STATE             (0x0)
#define CONFIG_ACNT1_INIT_STATE             (0x0)
#define CONFIG_ACNT2_INIT_STATE             (0x0)

const uint32 CONFIG_METER_INIT_STATE[METER_GROUP_NUM] = {CONFIG_METER_GP0_INIT_STATE , CONFIG_METER_GP1_INIT_STATE,
                                                        CONFIG_METER_GP2_INIT_STATE, CONFIG_METER_GP3_INIT_STATE};
#endif

#ifdef TCSUPPORT_CPU_EN7580 
#ifdef TCSUPPORT_CPU_EN7581
	uint32 CONFIG_TX_DSCP_NUM[TX_RING_NUM] = { CONFIG_TX0_DSCP_NUM , CONFIG_TX1_DSCP_NUM , CONFIG_TX2_DSCP_NUM , CONFIG_TX3_DSCP_NUM , 
												 CONFIG_TX4_DSCP_NUM , CONFIG_TX5_DSCP_NUM , CONFIG_TX6_DSCP_NUM , CONFIG_TX7_DSCP_NUM,
												 CONFIG_TX8_DSCP_NUM , CONFIG_TX9_DSCP_NUM , CONFIG_TX10_DSCP_NUM , CONFIG_TX11_DSCP_NUM , 
												 CONFIG_TX12_DSCP_NUM , CONFIG_TX13_DSCP_NUM , CONFIG_TX14_DSCP_NUM , CONFIG_TX15_DSCP_NUM,
												 CONFIG_TX16_DSCP_NUM , CONFIG_TX17_DSCP_NUM , CONFIG_TX18_DSCP_NUM , CONFIG_TX19_DSCP_NUM , 
												 CONFIG_TX20_DSCP_NUM , CONFIG_TX21_DSCP_NUM , CONFIG_TX22_DSCP_NUM , CONFIG_TX23_DSCP_NUM,
												 CONFIG_TX24_DSCP_NUM , CONFIG_TX25_DSCP_NUM , CONFIG_TX26_DSCP_NUM , CONFIG_TX27_DSCP_NUM , 
												 CONFIG_TX28_DSCP_NUM , CONFIG_TX29_DSCP_NUM , CONFIG_TX30_DSCP_NUM , CONFIG_TX31_DSCP_NUM };

#else
    uint32 CONFIG_TX_DSCP_NUM[TX_RING_NUM] = { CONFIG_TX0_DSCP_NUM , CONFIG_TX1_DSCP_NUM , CONFIG_TX2_DSCP_NUM , CONFIG_TX3_DSCP_NUM , 
													 CONFIG_TX4_DSCP_NUM , CONFIG_TX5_DSCP_NUM , CONFIG_TX6_DSCP_NUM , CONFIG_TX7_DSCP_NUM };
#endif
 	uint32 CONFIG_PAYLOAD_SIZE[4] = { CONFIG_PAYLOAD_2K_BYTE, CONFIG_PAYLOAD_1K_BYTE, CONFIG_PAYLOAD_512_BYTE, CONFIG_PAYLOAD_256_BYTE };
#else /*EN7516/EN7527*/
	uint32 CONFIG_TX_DSCP_NUM[TX_RING_NUM] = { CONFIG_TX0_DSCP_NUM , CONFIG_TX1_DSCP_NUM };
#endif

#if SUPPORT_CONFIG_QDMA_DSCP_BY_MI_CONFIG
    uint32 qdma_lan_rx_dscp_num[QDMA_LAN_DSCP_NUM_MODE_MAX][RX_RING_NUM]=
    {
        QDMA_LAN_DSCP_NUM_DEFAULT,
        QDMA_LAN_DSCP_NUM_TWO_WLAN,
    	QDMA_LAN_DSCP_NUM_ONE_WLAN,	
    	QDMA_LAN_DSCP_NUM_NO_WLAN,
    	QDMA_LAN_DSCP_NUM_MEMORY_SHRINK,
    	QDMA_LAN_DSCP_NUM_ONE_WLAN_SHRINK
    };

    uint32 qdma_wan_rx_dscp_num[QDMA_WAN_DSCP_NUM_MODE_MAX][RX_RING_NUM]=
    {
        QDMA_WAN_DSCP_NUM_DEFAULT
    };

    uint32 CONFIG_RX_DSCP_NUM[RX_RING_NUM];
#else
    uint32 CONFIG_RX_DSCP_NUM[RX_RING_NUM] = {  CONFIG_RX0_DSCP_NUM , CONFIG_RX1_DSCP_NUM , CONFIG_RX2_DSCP_NUM, CONFIG_RX3_DSCP_NUM ,
	 				        CONFIG_RX4_DSCP_NUM , CONFIG_RX5_DSCP_NUM , CONFIG_RX6_DSCP_NUM , CONFIG_RX7_DSCP_NUM ,
						CONFIG_RX8_DSCP_NUM , CONFIG_RX9_DSCP_NUM , CONFIG_RX10_DSCP_NUM , CONFIG_RX11_DSCP_NUM , 
						CONFIG_RX12_DSCP_NUM , CONFIG_RX13_DSCP_NUM , CONFIG_RX14_DSCP_NUM , CONFIG_RX15_DSCP_NUM };
#endif

/************************************************************************
*                  M A C R O S
*************************************************************************
*/	
#define atoi(x)  simple_strtoul(x, NULL,10)
#define QDMA_STOP_TIMER(timer)			{ del_timer_sync(&timer) ; }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
#define QDMA_START_TIMER(timer)			{ mod_timer(&timer, (jiffies + ((timer.data*HZ)/1000))) ; }
#else
#define QDMA_START_TIMER(timer)			{ mod_timer(&timer, (jiffies + ((timer.expires*HZ)/1000))) ; } 
#endif
#define WIFI_OFFLOAD_CHANNEL	7

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
struct device* qdma_dummy_dev_ptr = NULL;
#else
struct device qdma_dummy_dev = {
	.coherent_dma_mask = DMA_BIT_MASK(32),
};
struct device *qdma_dummy_dev_ptr = &qdma_dummy_dev;
#endif

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
#ifdef CONFIG_SUPPORT_SELF_TEST
extern uint loopCnt;
extern QDMA_DbgCounters_T counters[CONFIG_QDMA_CHANNEL][CONFIG_QDMA_QUEUE] ;
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
extern void (*sw_PKTQOS_CLEAR_STOP) (void);
#endif/*TCSUPPORT_COMPILE*/

extern atomic_t chnlLimit[CONFIG_QDMA_CHANNEL];
extern atomic_t queueLimit[CONFIG_QDMA_CHANNEL][CONFIG_QDMA_QUEUE];
extern atomic_t txRing1FreeCnt;


extern uint channel_limit_threshold;
extern uint queue_limit_threshold;

extern int trigger_timer_going ;

extern int dnstream_qos_enable;
#if defined(TCSUPPORT_TSO_ENABLE) && !defined(TCSUPPORT_TSO_V2_ENABLE)
extern struct timer_list tso_qos_timer ;
#endif

#if defined(QDMA_LAN)
extern int (*wifi_eth_fast_tx_hook)(struct sk_buff *skb);
extern int (*offload_eth_fast_tx_hook)(struct sk_buff *skb, int channel);
extern int (*xsi_fast_tx_hook)(struct sk_buff *skb);
extern int (*ra_sw_nat_get_wifi_dev_name) (struct sk_buff * skb);
#ifdef TCSUPPORT_TEST_VWTEST
extern int (*wifi_eth_fast_tx_ecnt_hook)(struct sk_buff *skb);
#endif
extern void (*qdma_err_drop_counters_hook)(unsigned short *qdma_tx_err, unsigned short *qdma_rx_err);
extern int (*qdma_get_txring_unused_dscp_cnt_hook)(int ring_idx);
extern void (*set_qdma_lan_qos_buffer_mode_hook)(int qdma_mode);

#else

extern void (*get_tr471_rx_msg_hook)(int rx_ring,unsigned int * rx_byte_cnt_l,unsigned int * rx_byte_cnt_h,unsigned int * err_cnt,unsigned int * drop_cnt);

#endif

#ifdef TCSUPPORT_CPU_ARMV8
extern struct device* get_frame_engine_dev(void);
extern unsigned int ecnt_qdma_lan_get_buffer_base(void);
extern unsigned int ecnt_qdma_wan_get_buffer_base(void);
#endif

#ifdef TCSUPPORT_NEW_WDOG
extern void wdog_kick_api(int source);
#endif

#if SUPPORT_INX_COLOR_RESOLUTION
extern uint default_mtr_color_cfg;
extern uint default_mtr_color_cfg1;
#endif
QDMA_TxQueueTrtcm_T txRateLimitGetParam[CONFIG_QDMA_CHANNEL] ;
QDMA_RxRateLimitGet_T rxRateLimitGetParam[RX_RING_NUM] ;
QDMA_TxQosScheduler_T txQosParam[CONFIG_QDMA_CHANNEL] ;
QDMA_VirtualChannelQoS_T txVirQosParam[CONFIG_QDMA_CHANNEL] ;

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int qdma_reset(struct ECNT_QDMA_Data *qdma_data);

/* Warning: same sequence with enum 'QDMA_HookFunction_t' */
qdma_api_op_t
qdma_operation[]=
{
	/* init */
	/* 0x00 */
	qdma_init,
	qdma_deinit,
	qdma_tx_dma_mode,
	qdma_rx_dma_mode,
	qdma_loopback_mode,
	qdma_register_callback_function,
	qdma_unregister_callback_function,
	qdma_enable_rxpkt_int,
	qdma_disable_rxpkt_int,
	qdma_receive_packets,
	qdma_transmit_packet,
	qdma_set_tx_qos_weight,
	qdma_get_tx_qos_weight,
	qdma_set_tx_qos,
	qdma_get_tx_qos,
	qdma_set_mac_limit_threshold,
	qdma_get_mac_limit_threshold,
	qdma_bm_transmit_packet_wifi_fast,
	
	/* other */
	/* 0x12 */
	qdma_set_txbuf_threshold,
	qdma_get_txbuf_threshold,
	qdma_set_prefetch_mode,
	qdma_set_pktsize_overhead_en,
	qdma_get_pktsize_overhead_en,
	qdma_set_pktsize_overhead_value,
	qdma_get_pktsize_overhead_value, 
	qdma_set_lmgr_low_threshold,
	qdma_get_lmgr_low_threshold,
	qdma_get_lmgr_status,

	/* test */
	/* 0x1C */
	qdma_set_dbg_level,
	qdma_dump_dma_busy,
	qdma_dump_reg_polling,
	qdma_set_force_receive_rx_ring1,

	/* tx rate limit */
	/* 0x20 */
	qdma_set_tx_drop_en,
	qdma_get_tx_drop_en,
	qdma_set_tx_ratemeter,
	qdma_get_tx_ratemeter,
	qdma_enable_tx_ratelimit,
	qdma_set_tx_ratelimit_cfg,
	qdma_get_tx_ratelimit_cfg,
	qdma_set_tx_ratelimit,
	qdma_get_tx_ratelimit,
	qdma_set_tx_dba_report,
	qdma_get_tx_dba_report,
	
	/* rx rate limit */
	/* 0x2B */
	qdma_set_rx_protect_en,
	qdma_get_rx_protect_en,
	qdma_set_rx_low_threshold,
	qdma_get_rx_low_threshold,
	qdma_set_rx_ratelimit_en,
	qdma_set_rx_ratelimit_pkt_mode,
	qdma_get_rx_ratelimit_cfg,
	qdma_set_rx_ratelimit,
	qdma_get_rx_ratelimit,
	
	/* txq cngst */
	/* 0x34 */
	qdma_set_txq_dei_drop_mode,
	qdma_get_txq_dei_drop_mode,
	qdma_set_txq_cngst_mode,
	qdma_get_txq_cngst_mode,
	qdma_set_txq_dei_threshold_scale,
	qdma_get_txq_dei_threshold_scale,
	qdma_set_txq_cngst_auto_config,
	qdma_get_txq_cngst_auto_config,
	qdma_set_txq_cngst_dynamic_threshold,
	qdma_get_txq_cngst_dynamic_threshold,
	qdma_set_txq_cngst_total_threshold,
	qdma_get_txq_cngst_total_threshold,
	qdma_set_txq_cngst_channel_threshold,
	qdma_get_txq_cngst_channel_threshold,
	qdma_set_txq_cngst_queue_threshold,
	qdma_get_txq_cngst_queue_threshold,
	qdma_set_txq_peekrate_params,
	qdma_get_txq_peekrate_params,
	qdma_set_txq_cngst_static_queue_normal_threshold,
	qdma_set_txq_cngst_static_queue_dei_threshold,
	qdma_get_txq_cngst_dynamic_info,
	qdma_get_txq_cngst_static_info,
	qdma_set_txq_cngst_queue_nonblocking,
	qdma_get_txq_cngst_queue_nonblocking,
	qdma_set_txq_cngst_channel_nonblocking,
	qdma_get_txq_cngst_channel_nonblocking,
		
	/* virtual channel */
	/* 0x4E */
	qdma_set_virtual_channel_mode,
	qdma_get_virtual_channel_mode,
	qdma_set_virtual_channel_qos,
	qdma_get_virtual_channel_qos,

	/* dbg cntr */
	/* 0x52 */
	qdma_clear_and_set_dbg_cntr_channel_group,
	qdma_clear_and_set_dbg_cntr_queue_group,
	qdma_clear_dbg_cntr_value_all,
	qdma_dump_dbg_cntr_value,
	qdma_set_cntr_channel,
	qdma_dump_cntr_channel,
	qdma_get_dbg_cntr_all_queue_value,
	qdma_get_dbg_cntr_rx_ring,
	/* dump */
	/* 0x5A */
	qdma_dump_tx_qos,
	qdma_dump_virtual_channel_qos,
	qdma_dump_tx_ratelimit,
	qdma_dump_rx_ratelimit,
	qdma_dump_tx_dba_report,
	qdma_dump_txq_cngst,
	/* 0x60 */
	qdma_clear_cpu_counters,
	qdma_dump_cpu_counters,
	qdma_dump_register_value,
	qdma_dump_descriptor_info,
	qdma_dump_irq_info,
	qdma_dump_hwfwd_info,
	qdma_dump_info_all,
	/* 0x67 */
	qdma_read_vip_info,
    qdma_set_mac_qos_config,
	/* 0x69 */
	qdma_general_set_trtcm_cfg,
	qdma_general_get_trtcm_cfg,
	qdma_general_set_ratelimit_mode_cfg,
	qdma_general_get_ratelimit_mode_cfg,
	qdma_general_set_ratelimit_mode_value,
	qdma_general_get_ratelimit_mode_value,
	qdma_general_set_ratelimit_bucket_size,
    
	/*EN7527/16: new APIs*/
	/* 0x70 */
	qdma_clear_and_set_dbg_cntr_ring_group,
	/*EN7580: new APIs*/
	/* 0x71 */
	qdma_enable_rxpkt_int2,
	qdma_disable_rxpkt_int2,
	qdma_receive_packets_int2,
	/* 0x74 */
	qdma_general_set_trtcm_mode_cfg,
	qdma_general_get_trtcm_mode_cfg,
	qdma_general_set_trtcm_mode_value,
	qdma_general_get_trtcm_mode_value,
	qdma_general_set_trtcm_bucket_size,
	/* 0x79 */
	qdma_set_flow_cntr_cfg,
	qdma_get_flow_cntr_cfg,
	qdma_get_flow_cntr_value,
	qdma_clear_flow_cntr_value,	
	qdma_set_tx_wred_mode,
	qdma_get_tx_wred_mode,
	qdma_set_tx_wred_threshold,
	qdma_set_tx_wred_probability,
	qdma_get_tx_wred_cfg,
	qdma_set_cpu_rx_red_probability,
	qdma_get_cpu_rx_red_probability,
	/* 0x84 */
	qdma_set_channel_close_status,
	qdma_get_channel_close_status,
	qdma_get_channel_empty_status,
	qdma_set_oam_modify_fp_en,
	qdma_get_oam_modify_fp_en,
	qdma_set_multicast_en,
	qdma_get_multicast_en,
	/* 0x8B */
    qdma_allocate_meter,
    qdma_free_meter,
    qdma_get_qos_flag,
    qdma_allocate_acnt,
    qdma_free_acnt,

	/* 0x90 */
	qdma_set_sla_chnl_cfg,
	qdma_get_sla_chnl_cfg,
	qdma_set_qos_aging_cfg,
	qdma_get_qos_aging_cfg,
	qdma_set_per_queue_aging_cfg,
	qdma_get_per_queue_aging_cfg,
	qdma_set_multicast_sptag_cfg,
	qdma_get_multicast_sptag_cfg,
	qdma_set_multicast_fport_cfg,
	qdma_get_multicast_fport_cfg,
	qdma_set_txq_cngst_static_channel_en,
	qdma_set_txq_cngst_static_queue_ratio,
	qdma_get_hqos_en,

	/* 0x9D */
	qdma_reset,
	/* DRAM TEST APIs */
	qdma_dram_test_dma_config,
	qdma_dram_test_dma_enable,
	qdma_dram_test_is_rx_done,
	qdma_dram_test_dma_skb_get,

	qdma_get_using_tx_dscp_channel,
	qdma_set_multicast_1toN_cfg,
	qdma_get_channel_cfg,
	qdma_api_set_txmsg,
    qdma_api_get_rxmsg,
    qdma_api_get_rxmsg_eth,
	qdma_api_check_dscp_is_free,
	qdma_api_set_downstream_qos_mode,
	qdma_api_get_downstream_qos_mode,
	/* QDMA_FUNCTION_MAX_NUM */
	/* 0xAB */
	NULL ,
};

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
QDMA_Private_T *gpQdmaPriv = NULL ;
int storm_ctrl_shrehold = 120;
int hwfwd_payload_max_size=0;
EXPORT_SYMBOL(storm_ctrl_shrehold);

/*Global Ring Mask for free Setting ,bind to INTs*/
uint int1_rx_ring = INT1_RX_RING_MASK;
uint int2_rx_ring = INT2_RX_RING_MASK;
uint int3_rx_ring = INT3_RX_RING_MASK;
uint int4_rx_ring = INT4_RX_RING_MASK;
/*Global TX Ring Mask for free Setting ,bind to IRQ1/IRQ2*/
#if SUPPORT_IRQ2
uint irq_tx_ring = IRQ_TX_RING_MASK;
#endif

int qdma_fwd_timer = 100; /* 100ms */
EXPORT_SYMBOL(qdma_fwd_timer);

uint macType = 0;
/* for EN7580 dscp&buffer init , start*/
unchar qdmaPayloadMode = 0;	/* 0x0:2K, 0x1:1K, 0x2:512, 0x3:256 */
unchar qdmaDscpMode = 0;	/* 0x0:DSCP in DRAM, 0x1:DSCP in SRAM */
uint qdmaLanBufferSize = 0;	/* QDMA LAN need DRAM buffer size*/
uint qdmaWanBufferSize = 0;	/* QDMA WAN need DRAM buffer size*/
unchar qdmaLanHqosMode = 0;	/* 0:disable, 1:enable */
unchar qdmaWanHqosMode = 0;
unchar qdmaLanDscpNumMode = 0;
unchar qdmaWanDscpNumMode = 0;
unchar qdmaDscpNumFileConfig = 0;
/* for EN7580 dscp&buffer init , end*/

#ifdef TCSUPPORT_QOS
#ifdef TCSUPPORT_OPENWRT
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
int qos_flag = QOS_HW_PQ_8_QUEUE;
#else
int qos_flag = QOS_HW_PQ;
#endif
#else
int qos_flag = NULLQOS;
#endif

#ifdef TCSUPPORT_OPENWRT
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
int gQueueMask = 255;
#else
int gQueueMask = 15;
#endif
#else
int gQueueMask = 0;
#endif
int isNeedHWQoS = 1;
#endif

#if defined(QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
int qos_wrr_info[9] = {0};
#else
int qos_wrr_info[5] = {0};
#endif
int max_prio = 3;
unchar qos_wrr_user = 0x00;
#endif

extern unsigned int sysctl_lan_tx_off;
extern unsigned int is5GTest;
extern int wifi_tx_shortcut_ver;
unsigned int sysctl_lan_tx_counter0= 0;
unsigned int sysctl_lan_tx_counter1 = 0;

atomic_t free_txd_count[TX_RING_NUM]={0};
int txPktDump=0, rxPktDump=0 ;      // dump tx pkt/msg, rx pkt/msg once.
int dropBroadcastMulticastFlag=0 ;      // drop broadcast pkts.

int txring1ReserveThreshold=0 ;

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
#if 0
static int dump_mem(unsigned long addr, unsigned long len)
{
	register int n, m, c, r;
	unsigned char temp[16];

	for( n = len; n > 0; ){
			printk("%.8lx ", addr);
			r = n < 16? n: 16;
			memcpy((void *) temp, (void *) addr, r);
			addr += r;
			for( m = 0; m < r; ++m ){
					printk("%c", (m & 3) == 0 && m > 0? '.': ' ');
					printk("%.2x", temp[m]);
			}
			for(; m < 16; ++m )
					printk("   ");
			printk("  |");
			for( m = 0; m < r; ++m ){
				c = temp[m];
				printk("%c", ' ' <= c && c <= '~'? c: '.');
			}
			n -= r;
			for(; m < 16; ++m )
					printk(" ");
			printk("|\n");
	}

	return 0;
}
#endif

int inline qdma_kfree_skb(struct sk_buff *skb, uint msg0, int ringIdx)
{
	uint channel=0, queue=0, ret=0 ;

	if((skb->ppe_magic == FOE_MAGIC_SPEED_TEST_UPSTREAM) && (skb->tso_mark & TSO_STATIC_SKB)) {
		skb->tso_mark &= (~TSO_SKB_USING);
		return ret;
	}
#if defined(TCSUPPORT_QDMA_VERIFY) && defined(QDMA_LAN)
	if(1 == qdma_driver_packet_test_check(skb)){
		return ret;
	}
#endif
    
    channel = ((txMsgWord0_t)msg0).channel ;
    queue = ((txMsgWord0_t)msg0).queue ;

#if defined(QDMA_LAN)
    /* qdma lan channel 7 for wifi offload to ppe, not do atomic for better performance.*/
    if (channel != WIFI_OFFLOAD_CHANNEL)
    {
        atomic_dec(&chnlLimit[channel]);
        atomic_dec(&queueLimit[channel][queue]);
    }
#else
	atomic_dec(&chnlLimit[channel]);
	atomic_dec(&queueLimit[channel][queue]);
	if(ringIdx == RING_IDX_1)
		atomic_inc(&txRing1FreeCnt);
#endif
	
	gpQdmaPriv->counters.txPktsFreeCounts++ ;
    if(gpQdmaPriv->devCfg.bmRecyPktCallbackFunction) {
        if(gpQdmaPriv->devCfg.bmRecyPktCallbackFunction(skb, msg0) != 0) 
        {
			ret = -1;
        } 
    } else {
        dev_kfree_skb_any(skb) ;
    }
    return ret;
}

int qdmaRateMeterValueCalCulate(int unit, ushort *timeDivisor, ushort *timeSlice)
{
	long closeUnitVal=0x7FFFFFFF;
	long curTimeDivisor=0, curTimeSlice=0, curCloseUnitVal=0 ;
	
	if(unit<2 || unit>UNIT_MAX) {
		printk("Fault: Rate Meter unit is %d, which should between 2 and %d\n", unit, UNIT_MAX) ;
		return -1;
	}

	for(curTimeDivisor=0; curTimeDivisor<QDMA_TX_RATE_METER_TIME_DIVISOR_ITEMS; curTimeDivisor++) {
		curTimeSlice = ((1<<3)<<4)*1000*(1<<curTimeDivisor)/unit ;
		if(curTimeSlice>0 && curTimeSlice<=RATELIMIT_MAX) {
			curCloseUnitVal = ((1<<3)<<4)*1000*(1<<curTimeDivisor)/curTimeSlice;
			if(abs(curCloseUnitVal)<abs(closeUnitVal)) {
				closeUnitVal = curCloseUnitVal ;
				*timeDivisor = (ushort)curTimeDivisor ;
				*timeSlice = (ushort)curTimeSlice ;
				if(closeUnitVal == 0)
					return 1;
			}
		}
	}
	
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to configure the TX interrupt delay parameters.
 Input Args:	arg1: irq queue threshold (0~irqDepth). When valid irq entry 
 				len > irq queue threshold, the hardware will generate an interrupt. 
 				arg2: irq pending time (0~255). This argument is specified max 
 				pending for the irq queue interrupt. The uint of the pending time 
 				is 20us.
 Ret Value:		0: setting successful otherwise failed.
******************************************************************************/
int qdma_set_tx_delay(unchar txIrqThreshold, ushort txIrqPtime)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	
	if(txIrqThreshold<=0 || txIrqThreshold>gpQdmaPriv->irqDepth || txIrqPtime>0xFFFF) {
		return -EINVAL ;
	}
	
	qdmaSetIrqThreshold(base, txIrqThreshold) ;
	qdmaSetIrqPtime(base, txIrqPtime) ;
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	It's used to configure the RX interrupt delay parameters.
 Input Args:	arg1: Specified max number of pending interrupts. When the 
 				number of pending interrupts equal or greater than the arg1 or 
 				interrupt pending time reach the limit (arg2*20us), a final
 				RX_DLY_INT is generated.
 				arg2: Specified max pending time for the internal RX_DONE_INT. 
 				When the pending time equal or greater arg2*20us or the number
 				of RX_DONT_INT equal or greater than arg1, an final RX_DLY_INT 
 				is generated.
 Ret Value:		0: setting successful otherwise failed.
******************************************************************************/
int qdma_set_rx_delay(unchar rxMaxInt, unchar rxMaxPtime, int ringIdx)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(rxMaxInt != 0) {
#if !SUPPORT_RX_DLYINT_EN
		qdmaSetRxDelayIntCfg(base, ringIdx, ((rxMaxInt<<DLY_INT_RXMAX_PINT_SHIFT) | (rxMaxPtime<<DLY_INT_RXMAX_PTIME_SHIFT)));
#else
		qdmaSetRxDelayIntCfg(base, ringIdx, ((DLY_INT_RXDLY_INT_EN(ringIdx)) | (rxMaxInt<<(DLY_INT_RXMAX_PINT_SHIFT(ringIdx))) | (rxMaxPtime<<(DLY_INT_RXMAX_PTIME_SHIFT(ringIdx))))) ;
#endif
	} else {
		qdmaSetRxDelayIntCfg(base, ringIdx, 0) ;
	}

	return 0 ;
}

static inline int qdma_has_free_rxdscp(int ringIdx)
{
	return (gpQdmaPriv->rxEndPtr[ringIdx]->next != gpQdmaPriv->rxStartPtr[ringIdx]) ;
}

int qdma_bm_tx_polling_mode(QDMA_Mode_t txMode, unchar txThreshold)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(txMode == QDMA_ENABLE) {
		gpQdmaPriv->devCfg.txRecycleThreshold = txThreshold ;
#if SUPPORT_INT_ENABLE_NUM_2
		qdmaDisableInt(base, (INT_MASK_IRQ_INT|INT_STATUS_IRQ_FULL), QDMA_INT1, QDMA_INT_ENABLE1) ;
#else
		qdmaDisableInt(base, (INT_MASK_TX0_DONE|INT_MASK_TX1_DONE|INT_STATUS_IRQ_FULL), QDMA_INT1, QDMA_INT_ENABLE0) ;
#endif	
		gpQdmaPriv->devCfg.flags.isTxPolling = QDMA_ENABLE ;
	} else {
		gpQdmaPriv->devCfg.flags.isTxPolling = QDMA_DISABLE ;
#if SUPPORT_INT_ENABLE_NUM_2
		qdmaEnableInt(base, (INT_MASK_IRQ_INT|INT_STATUS_IRQ_FULL), QDMA_INT1, QDMA_INT_ENABLE1) ;
#else
		qdmaEnableInt(base, (INT_MASK_TX0_DONE|INT_MASK_TX1_DONE|INT_STATUS_IRQ_FULL), QDMA_INT1, QDMA_INT_ENABLE0) ;
#endif	

	}
	
	return 0 ;
}

int qdma_receive_packet_mode(QDMA_RecvMode_t rxMode)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	
	if(rxMode == QDMA_RX_POLLING) {
#if defined(TCSUPPORT_CPU_EN7581)
		qdmaDisableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_LOW_16_RING|INT1_MASK_RX_DONE_LOW_16_RING), QDMA_INT1, QDMA_INT_ENABLE2) ;
		qdmaDisableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_HIGH_16_RING|INT1_MASK_RX_DONE_HIGH_16_RING), QDMA_INT1, QDMA_INT_ENABLE3) ;	
#elif defined(TCSUPPORT_CPU_EN7580)
		qdmaDisableInt(base, (INT1_MASK_NO_RX_CPU_DSCP|INT1_MASK_RX_DONE), QDMA_INT1, QDMA_INT_ENABLE2) ;
#else
		qdmaDisableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_0_1|INT1_MASK_RX_DONE_0_1), QDMA_INT1, QDMA_INT_ENABLE0) ;
		qdmaDisableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_2_15|INT1_MASK_RX_DONE_2_15), QDMA_INT1, QDMA_INT_ENABLE1) ;
#endif
		gpQdmaPriv->devCfg.flags.isRxPolling = QDMA_ENABLE ;
		gpQdmaPriv->devCfg.flags.isRxNapi = QDMA_DISABLE ;
	} else {
		gpQdmaPriv->devCfg.flags.isRxPolling = QDMA_DISABLE ;
	
		if(rxMode == QDMA_RX_NAPI) {
			gpQdmaPriv->devCfg.flags.isRxNapi = QDMA_ENABLE ;
		} else {
			gpQdmaPriv->devCfg.flags.isRxNapi = QDMA_DISABLE ;
		}
#if defined(TCSUPPORT_CPU_EN7581)
		qdmaEnableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_LOW_16_RING|INT1_MASK_RX_DONE_LOW_16_RING), QDMA_INT1, QDMA_INT_ENABLE2) ;
		qdmaEnableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_HIGH_16_RING|INT1_MASK_RX_DONE_HIGH_16_RING), QDMA_INT1, QDMA_INT_ENABLE3) ; 
#elif defined(TCSUPPORT_CPU_EN7580)
		qdmaEnableInt(base, (INT1_MASK_NO_RX_CPU_DSCP|INT1_MASK_RX_DONE), QDMA_INT1, QDMA_INT_ENABLE2) ;
#else
		qdmaEnableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_0_1|INT1_MASK_RX_DONE_0_1), QDMA_INT1, QDMA_INT_ENABLE0) ;
		qdmaEnableInt(base, (INT1_MASK_NO_RX_CPU_DSCP_2_15|INT1_MASK_RX_DONE_2_15), QDMA_INT1, QDMA_INT_ENABLE1) ;
#endif
	}
	
	return 0 ;
}

int qdma_recycle_packet_mode(QDMA_RecycleMode_t txMode, unchar txThrh)
{
	int ret = 0;
	
	if(txMode == QDMA_TX_POLLING) {
		if(txThrh > 0) {
			ret = qdma_bm_tx_polling_mode(QDMA_ENABLE, txThrh) ;
		} else {
			ret = qdma_bm_tx_polling_mode(QDMA_ENABLE, 32) ;
		}
	} else {
		ret = qdma_bm_tx_polling_mode(QDMA_DISABLE, 0) ;
	}
	
	return ret ;
}

__IMEM struct sk_buff * allocate_skb_buffer(void)
{
	int offset=0 ;
	struct sk_buff * skb=NULL ;
	
	/* allocate the packet buffer */
	skb = skbmgr_dev_alloc_skb2k() ;
	if(skb == NULL) {
		QDMA_MSG(DBG_WARN, "Allocate socket buffer failed.\n");
		gpQdmaPriv->counters.rxAllocFailErrs++ ;
		goto finish ;
	}
	
	/* Shift to 4 byte alignment */
	offset = ((uint)(skb->tail) & (SKB_ALIGNMENT-1)) ;
	if(offset) {
		skb_reserve(skb, (SKB_ALIGNMENT - offset)) ;
	}
	
	/* alloc big head room */
	skb_reserve(skb, 64) ;

#ifdef CONFIG_RX_2B_OFFSET
	/* Reserve 2 bytes for zero copy */
#if defined(TCSUPPORT_CPU_ARMV8_64)
	skb_reserve(skb, 2) ;
#else
	skb_reserve(skb, NET_IP_ALIGN) ;
#endif
#endif
	
finish:
	return skb;
}

#if defined(TCSUPPORT_LRO_ENABLE)
__IMEM struct sk_buff * allocate_skb_lro_buffer(void)
{
	int offset=0 ;
	struct sk_buff * skb=NULL ;
	
	/* allocate the packet buffer */
	skb = skbmgr_dev_alloc_skb_lro() ;
	if(skb == NULL) {
		QDMA_MSG(DBG_WARN, "Allocate socket buffer failed.\n") ;
		gpQdmaPriv->counters.rxAllocFailErrs++ ;
		goto finish ;
	}
	
	/* Shift to 4 byte alignment */
	offset = ((uint)(skb->tail) & (SKB_ALIGNMENT-1)) ;
	if(offset) {
		skb_reserve(skb, (SKB_ALIGNMENT - offset)) ;
	}
	
#ifdef CONFIG_RX_2B_OFFSET
	/* Reserve 2 bytes for zero copy */
#if defined(TCSUPPORT_CPU_ARMV8_64)
	skb_reserve(skb, 2) ;
#else
	skb_reserve(skb, NET_IP_ALIGN) ;
#endif
#endif
	
finish:
	return skb;
}
#endif

int qdma_prepare_rx_buffer(int ringIdx)
{
	int ret = 0 ;
	struct sk_buff *skb = NULL ;

#if defined(TCSUPPORT_LRO_ENABLE) && !defined(TCSUPPORT_QDMA_VERIFY)
	if( ((INT_STATUS_RX0_DONE<<ringIdx)&INT2_MASK_RX_DONE) 
#if SUPPORT_FRAGMENT_REASSEMBLE
	&& (FRAG_RX_RING_IDX != ringIdx) && (ASSE_RX_RING_IDX != ringIdx)
#endif
	)
		skb = allocate_skb_lro_buffer();
	else
#endif
	skb = allocate_skb_buffer();
	if(skb == NULL) {
		goto err ;
	}
		
	ret = qdma_bm_hook_receive_buffer(skb, ringIdx) ;
	if(ret != 0) {
		ret = -ENODEV ;
		goto err ;
	}
	
	return ret ;
	
err:
	if(skb) 		dev_kfree_skb_any(skb) ;
	return ret ;
}

/******************************************************************************
 Packet Receive
******************************************************************************/
/******************************************************************************
******************************************************************************/
static inline void qdma_bm_add_rx_dscp(struct QDMA_DscpInfo_S *diPtr, int ringIdx) 
{
	if(!gpQdmaPriv->rxStartPtr[ringIdx]) {
		gpQdmaPriv->rxStartPtr[ringIdx] = diPtr ;
		diPtr->next = gpQdmaPriv->rxStartPtr[ringIdx] ;
	} else {
		diPtr->next = gpQdmaPriv->rxStartPtr[ringIdx]->next ;
		gpQdmaPriv->rxStartPtr[ringIdx]->next = diPtr ;
		gpQdmaPriv->rxStartPtr[ringIdx] = diPtr ;
	}
}

/******************************************************************************
******************************************************************************/
static inline struct QDMA_DscpInfo_S *qdma_bm_remove_rx_dscp(int ringIdx)
{
	struct QDMA_DscpInfo_S *diPtr = NULL ;

	if(gpQdmaPriv->rxStartPtr[ringIdx]) {
		diPtr = gpQdmaPriv->rxStartPtr[ringIdx]->next ;
		
		if(gpQdmaPriv->rxStartPtr[ringIdx] == diPtr) {
			gpQdmaPriv->rxStartPtr[ringIdx] = NULL ;
		} else {
			gpQdmaPriv->rxStartPtr[ringIdx]->next = diPtr->next ;
		}
	}

	return diPtr ;
}

/******************************************************************************
******************************************************************************/
static inline struct QDMA_DscpInfo_S *qdma_bm_get_unused_rx_dscp(int ringIdx)
{
	struct QDMA_DscpInfo_S *diPtr = NULL ;
	
	if(gpQdmaPriv->rxStartPtr[ringIdx]) {
		if(!gpQdmaPriv->rxEndPtr[ringIdx]) {
			diPtr = gpQdmaPriv->rxStartPtr[ringIdx] ;
			gpQdmaPriv->rxEndPtr[ringIdx] = diPtr ;
		} else if(gpQdmaPriv->rxEndPtr[ringIdx]->next != gpQdmaPriv->rxStartPtr[ringIdx]) {
			diPtr = gpQdmaPriv->rxEndPtr[ringIdx]->next ;
			gpQdmaPriv->rxEndPtr[ringIdx] = diPtr ; ;
		}
	} 

	return diPtr ;
}

static void __dump_skb(struct sk_buff *skb, uint pktLen) 
{
	unchar n, *p = (unchar *)skb->data ;
	int i ;

	for(i=0 ; i<pktLen ; i++) {
		n = i & 0x0f ;
		
		if(n == 0x00) 		printk(" \n%.4x: ", i) ;
		else if(n == 0x08) 	printk(" ") ;

		printk("%.2x ", *p++) ; 
	}
	printk("\n") ;
}

static void __dump_skb_for_crx_scatter(struct sk_buff *skb, uint pktLen) 
{
	unchar n, *p = (unchar *)skb->data ;
	int i ;

    p = p-2; /*for 2Byte offset*/

	for(i=0 ; i<pktLen ; i++) {
		n = i & 0x0f ;
		
		if(n == 0x00) 		printk(" \n%.4x: ", i) ;
		else if(n == 0x08) 	printk(" ") ;

		printk("%.2x ", *p++) ; 
	}
	printk("\n") ;
}


#if defined(QDMA_LAN)
int isSpecialTag(uint16 eth_type)
{
	/* Please modify this function to speed up the packet with special tag
	 * Ex: 
	 *    Ralink switch = 0x81xx
	 *    Realtek switch = 0x8899 
	 */ 
	if ((eth_type & 0xCC40) == 0x8000){ //New Stag format with these  bit 0, alway force UP 0~3.
		return 1;
	} else {
		return 0;
	}
}

int is8021Q(uint16 eth_type)
{
	if (eth_type == htons(ETH_P_8021Q)) {
		return 1;
	} else {
		return 0;
	}
}

char isTPID(uint16 eth_type){
	uint16 tpid = read_reg_word(PPE_TPID) & 0xffff;

	if((tpid == 0x8100) || (tpid == 0x00)){
		if(eth_type == htons(0x88a8)){
			//PpeParseResult.vlan_tag = eth_type;
			return 1;
		}
	}else{
		if((eth_type == htons(0x88a8)) || (eth_type == htons(tpid))){
			//PpeParseResult.vlan_tag = eth_type;
			return 1;
		}
	}

	return 0;
}

int GetPppoeSid(struct sk_buff * skb, uint32 vlan_gap,
	    uint16 * sid, uint16 * ppp_tag)
{
	struct pppoe_hdr *peh = NULL;

	peh = (struct pppoe_hdr *)(skb->data + ETH_HLEN + vlan_gap);

	*ppp_tag = peh->tag[0].tag_type;
	if (peh->ver != 1 || peh->type != 1){
		return 1;
	}

	*sid = peh->sid;
	return 0;
}

#define QDMA_PARSE_FTP_SYN 1
#define QDMA_PARSE_FTP_FIN 2
int QdmaParseFTPInfo(struct sk_buff * skb, unsigned char* type,unsigned int* dst_ip)
{
	struct vlan_hdr *vh = NULL;
	struct ethhdr *eth = NULL;
	struct iphdr *iph = NULL;
	struct tcphdr *th = NULL;
	int offset = 0;
	int i = 0, count = 0;
	uint16 eth_type = 0;
	uint32 pppoe_gap = 0;
	uint16 ppp_tag = 0;
	uint16 pppoe_sid = 0;
	uint16 vlan1_gap = 0;
	uint16 vlan2_gap = 0;
	uint16 vlan_layer = 0;
    unsigned short ftp_port_low = 0;
    unsigned char *data = NULL;
    uint16 ip_len = 0, tcp_len = 0, ip_total_len = 0;
    
	eth = (struct ethhdr *)skb->data;
	eth_type = eth->h_proto;
    
	if(eth_type == ETH_P_PPP_SES){
		pppoe_gap = 8;
		if (GetPppoeSid(skb, vlan1_gap,
			&pppoe_sid,	&ppp_tag)) {
			return -1;
		}
	}
	else if (is8021Q(eth_type) || isTPID(eth_type))
	{
		vh = (struct vlan_hdr *)(skb->data + ETH_HLEN);
		vlan_layer++;
		vlan1_gap = VLAN_HLEN;

		/* VLAN + PPPoE */
		if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
			pppoe_gap = 8;
			if (GetPppoeSid(skb, vlan1_gap,
					&pppoe_sid, &ppp_tag)) {
				return -1;
			}
			eth_type = vh->h_vlan_encapsulated_proto;
			/* Double VLAN = VLAN + VLAN */
		} else if ( is8021Q(vh->h_vlan_encapsulated_proto) || 
			isSpecialTag(vh->h_vlan_encapsulated_proto)) {
			vh = (struct vlan_hdr *)(skb->data + ETH_HLEN + VLAN_HLEN);
			vlan_layer++;
			vlan2_gap = VLAN_HLEN;

			/* VLAN + VLAN + PPPoE */
			if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
				pppoe_gap = 8;
				if (GetPppoeSid(skb, (vlan1_gap + vlan2_gap),
				     &pppoe_sid, &ppp_tag)) {
					return -1;
				}
				eth_type = vh->h_vlan_encapsulated_proto;
			} else if (is8021Q(vh->h_vlan_encapsulated_proto)) {
				/* VLAN + VLAN + VLAN */
				vh = (struct vlan_hdr *)(skb->data + ETH_HLEN + VLAN_HLEN + VLAN_HLEN);
				vlan_layer++;
				if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
					/* VLAN*3 + PPPOE */
					pppoe_gap = 8;
					if (GetPppoeSid(skb, vlan_layer*VLAN_HLEN,
							&pppoe_sid, &ppp_tag)) {
						return -1;
					}
					eth_type = vh->h_vlan_encapsulated_proto;
				}else if (is8021Q(vh->h_vlan_encapsulated_proto)) {
					/* VLAN*4 */
					vh = (struct vlan_hdr *)(skb->data + ETH_HLEN + VLAN_HLEN + VLAN_HLEN + VLAN_HLEN);
					vlan_layer++;
					if (ntohs(vh->h_vlan_encapsulated_proto) == ETH_P_PPP_SES) {
						/* VLAN*4 + PPPOE */
						pppoe_gap = 8;
						if (GetPppoeSid(skb, vlan_layer*VLAN_HLEN,
							&pppoe_sid, &ppp_tag)) {
							return -1;
						}
						eth_type = vh->h_vlan_encapsulated_proto;
					}else if (is8021Q(vh->h_vlan_encapsulated_proto)) {
						/* VLAN*5 no support */
						return -1;
					}else{
						/* VLAN*4 + IP */
						eth_type = vh->h_vlan_encapsulated_proto;
					}
				}else{
					/* VLAN*3 + IP */
					eth_type = vh->h_vlan_encapsulated_proto;
				}
			} else {
				/* VLAN + VLAN + IP */
				eth_type = vh->h_vlan_encapsulated_proto;
			}
		} else {
			/* VLAN + IP */
			eth_type = vh->h_vlan_encapsulated_proto;
		}
	}

	/* set layer2 start addr */
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
	offset = (unsigned char *)(void *)eth - skb->data;
	skb_set_mac_header(skb,offset);
#else
	LAYER2_HEADER(skb) = (void *)eth;
#endif

	/* set layer3 start addr */
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
	offset = (unsigned char *)((void *)eth + ETH_HLEN + vlan1_gap +
	vlan2_gap + pppoe_gap) - skb->data;
	skb_set_network_header(skb, offset);
#else
	LAYER3_HEADER(skb) =
	    ((void *)eth + ETH_HLEN + vlan1_gap + vlan2_gap + pppoe_gap);
#endif

	if(vlan_layer >= 3) {
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
		skb->network_header += (vlan_layer - 2)*VLAN_HLEN;
#else
		LAYER3_HEADER(skb) += (vlan_layer - 2)*VLAN_HLEN;
#endif
	}

 	if ((eth_type == htons(ETH_P_IP)) || (eth_type == htons(ETH_P_PPP_SES)
	&& ppp_tag == htons(PPP_IP))) 
	{
		iph = (struct iphdr *)LAYER3_HEADER(skb);
        ip_len = iph->ihl<<2;
        ip_total_len = ntohs(iph->tot_len);
		*dst_ip = ntohl(iph->daddr);
		//prepare layer3/layer4 info
		if (iph->protocol == IPPROTO_TCP) {
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
			offset = ((uint8_t *) iph + ip_len) - skb->data;
			skb_set_transport_header(skb,offset);
#else
			LAYER4_HEADER(skb) = ((uint8_t *) iph + ip_len);
#endif
			th = (struct tcphdr *)LAYER4_HEADER(skb);
            tcp_len = th->doff<<2;
			data = (uint8_t *)th + tcp_len;
			if(th->syn)
				*type = QDMA_PARSE_FTP_SYN;
			else if(th->fin)
				*type = QDMA_PARSE_FTP_FIN;
			else
				return -1;
		} else {
			return -1;
		}
	} else {
		return -1;
	}

	return 0;
}
#endif

/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_receive_packets(uint maxPkts, int ringIdx) 
{
	QDMA_DMA_DSCP_T rxDscp ;
	struct QDMA_DscpInfo_S *dscpInfoPtr=NULL ;
	uint cnt = maxPkts ;
	uint pktCount = 0 ;
	int retValue = 0, ret = 0 ;
	unsigned short ftpPort = 0;
	struct sk_buff *newSkb = NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	int (*bmRecvCallbackFun)(void *, uint, struct sk_buff *, uint) = NULL ;
	QDMA_RxInfo_T rxInfo;
#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
    unsigned int *msgPtr=NULL;
#endif
	rxMsgWord_t *curRxMsg = NULL;
	struct sk_buff *skb = NULL;
	unsigned short sptag = 0, sw_udf = 0;
	int magic = 0;
	unchar tcp_type = 0;
	uint32 tcp_dip = 0;
	unsigned int tmp_sptag = 0,tmp_swudf = 0;
	
	do {
		if(!gpQdmaPriv->rxStartPtr[ringIdx] || 
            gpQdmaPriv->rxStartPtr[ringIdx]== gpQdmaPriv->rxEndPtr[ringIdx])
		{
			QDMA_MSG(DBG_WARN, "gpQdmaPriv->rxStartPtr[%d]:%d, gpQdmaPriv->rxEndPtr[%d]:%d, \
                crtl.done:%d, cpuIdx:%d, dmaIdx:%d\n"
				, ringIdx, gpQdmaPriv->rxStartPtr[ringIdx]->dscpIdx
				, ringIdx, gpQdmaPriv->rxEndPtr[ringIdx]->dscpIdx
				, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr->ctrl.done
				, qdmaGetRxCpuIdx(base, ringIdx), qdmaGetRxDmaIdx(base, ringIdx)) ;
			return pktCount ;
		}
		memcpy(&rxDscp, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr, sizeof(QDMA_DMA_DSCP_T)) ;
		if(!rxDscp.ctrl.done)
			return pktCount ;
		dscpInfoPtr = gpQdmaPriv->rxStartPtr[ringIdx];
		
		gpQdmaPriv->rxStartPtr[ringIdx] = gpQdmaPriv->rxStartPtr[ringIdx]->next ;
		
		pktCount++ ;
		
		/* check DSCP cotent: if pkt_addr is NULL or pkt_len is 0, then some big issue happens, must stop running and check why */
		if(!rxDscp.pkt_addr || !rxDscp.ctrl.pkt_len)
		{
			QDMA_ERR("The content of the RX DSCP is incorrect.\n") ;
			gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ; 
			break ;
		}

		/* overflow is not critical issue, drop this packet and go-head */
#if SUPPORT_TX_DROP_BIT
		if(rxDscp.ctrl.drop_pkt)
#else
		if(rxDscp.ctrl.overflow)
#endif	
		{
			QDMA_MSG(DBG_WARN, "The pkt size is overflow: ready pkt_len is %d, received pkt_len is %d.\n", skb_tailroom(dscpInfoPtr->skb), rxDscp.ctrl.pkt_len) ;
			gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ;
			newSkb = dscpInfoPtr->skb;
			goto next;
		}
		
		newSkb = allocate_skb_buffer();
		if(newSkb == NULL) {
			QDMA_MSG(DBG_WARN, " Allocate skb fail, Revert received packet as new skb.\n");
			gpQdmaPriv->counters.rxAllocFailDropErrs[ringIdx]++ ;
			newSkb = dscpInfoPtr->skb;
			goto next;
		}

#ifdef TCSUPPORT_CPU_ARMV8
		ecnt_dcache_inv(((unsigned long)dscpInfoPtr->skb->data), rxDscp.ctrl.pkt_len);
		mb();
#endif
        
#ifdef CONFIG_DEBUG
        if(gpQdmaPriv->devCfg.dbgLevel >= DBG_ST) {
            printk("\n\nrxMsg0:0x%08x, rxMsg1:0x%08x, pkt_len:%d\n", (uint)rxDscp.msg[0],(uint)rxDscp.msg[1], rxDscp.ctrl.pkt_len);
            __dump_skb(dscpInfoPtr->skb, MIN(128, rxDscp.ctrl.pkt_len));
        }
#endif
        if(rxPktDump == 1) {
            printk("rxMsg0:0x%08x, rxMsg1:0x%08x, rxMsg2:0x%08x, rxMsg3:0x%08x, pkt_len:%d\n"
                , (uint)rxDscp.msg[0], (uint)rxDscp.msg[1], (uint)rxDscp.msg[2], (uint)rxDscp.msg[3], rxDscp.ctrl.pkt_len);
            __dump_skb(dscpInfoPtr->skb, rxDscp.ctrl.pkt_len);
            rxPktDump = 0;
        }
		
#if SUPPORT_TUNNEL_HW_ACCELE
            skb = dscpInfoPtr->skb;
			curRxMsg = (rxMsgWord_t*) rxDscp.msg;

			qdma_get_rxmsg_sptag(curRxMsg, &tmp_sptag);
			qdma_get_rxmsg_swudf(curRxMsg, &tmp_swudf);
			sptag = tmp_sptag & 0xffff;
			sw_udf = tmp_swudf & 0xffff;				

#if defined(QDMA_LAN)
			magic = FOE_MAGIC_GE;
#else

			magic = FOE_MAGIC_GPON;
#endif

			if((ra_sw_nat_hook_tunnel_down_acc_hook) && (ra_sw_nat_hook_tunnel_down_acc_hook(skb,rxDscp.ctrl.pkt_len,sptag,sw_udf,magic, (char*)&curRxMsg->rxMsgW1, sizeof(rxMsgWord1_t)))){
				 goto next;
			}
#endif


#if defined(TCSUPPORT_QDMA_VERIFY) && defined(QDMA_LAN)
       if(1 == qdma_driver_receive_and_transmit_test(dscpInfoPtr, &rxDscp, ringIdx)){
            goto next;
       }
#endif

#if defined(QDMA_LAN)
          	//QA have a test case that SYC flood attact ONU WAN interface including FTP port 20 and 21, we need to drop lots of SYC flood packet.
		//But QA have another test case that FTP server on ONU LAN side, we need set IFC rule to make LAN FTP packet go into indicate RX ring.
		if(unlikely(FTP_PASV_PORT_RING == ringIdx))
		{
			if(-1 != QdmaParseFTPInfo(dscpInfoPtr->skb,&tcp_type,&tcp_dip)){
				IFC_API_SET_QDMA_FTP(tcp_type,tcp_dip);				
			}				
		}
#endif

#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
        /* In FTTdp, channel 0 is for ethLan and channel 1~4 are for ptmLan */
        msgPtr = (unsigned int*)rxDscp.msg;
        if (((*msgPtr)>>3) & 0xff)
            bmRecvCallbackFun = gpQdmaPriv->devCfg.bmRecvCallbackPtmLanFunction;
        else
            bmRecvCallbackFun = gpQdmaPriv->devCfg.bmRecvCallbackFunction;
#else
            bmRecvCallbackFun = gpQdmaPriv->devCfg.bmRecvCallbackFunction;
#endif
    
        if(bmRecvCallbackFun) {
			retValue = bmRecvCallbackFun((void *)rxDscp.msg, QDMA_RX_DSCP_MSG_LENS, dscpInfoPtr->skb, rxDscp.ctrl.pkt_len);
			if(retValue< 0)
			{
				QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets: retValue = %d, EFAULT = %d, line = %d\n", 
                    retValue, EFAULT, __LINE__);
				if ((retValue + EFAULT) == -BROADCAST_OFFSET)
				{
					gpQdmaPriv->counters.rxBroadcastCounts[ringIdx]++;
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets:rxBroadcastCounts[%d]=%d, line = %d\n", 
                        ringIdx, gpQdmaPriv->counters.rxBroadcastCounts[ringIdx], __LINE__);
				}
				else if ((retValue + EFAULT) == -MULTICAST_OFFSET)
				{
					gpQdmaPriv->counters.rxMulticastCounts[ringIdx]++;					
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets: rxMulticastCounts[%d]=%d, line = %d\n", 
                        ringIdx, gpQdmaPriv->counters.rxMulticastCounts[ringIdx], __LINE__);
				}
				else if ((retValue + EFAULT) == -UNKNOWN_UNICAST_OFFSET)
				{
					gpQdmaPriv->counters.rxUnknownUnicastCounts[ringIdx]++;					
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets: rxUnknownUnicastCounts[%d]=%d, line = %d\n", 
                        ringIdx, gpQdmaPriv->counters.rxUnknownUnicastCounts[ringIdx], __LINE__);
				}
				gpQdmaPriv->counters.rxPktErrs[ringIdx]++ ;
			} else {
				QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets: retValue = %d, line = %d\n", retValue, __LINE__);
				if (retValue == BROADCAST_OFFSET)
				{
					gpQdmaPriv->counters.rxBroadcastCounts[ringIdx]++;					
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets: rxBroadcastCounts[%d]=%d, line = %d\n", 
                        ringIdx, gpQdmaPriv->counters.rxBroadcastCounts[ringIdx],  __LINE__);
				}
				else if (retValue == MULTICAST_OFFSET)
				{
					gpQdmaPriv->counters.rxMulticastCounts[ringIdx]++;					
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets:rxMulticastCounts[%d]=%d, line = %d\n", 
                        ringIdx, gpQdmaPriv->counters.rxMulticastCounts[ringIdx],__LINE__);
				}
				else if (retValue == UNKNOWN_UNICAST_OFFSET)	
				{
					gpQdmaPriv->counters.rxUnknownUnicastCounts[ringIdx]++;					
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets: rxUnknownUnicastCounts[%d]=%d, line = %d\n", 
                        ringIdx, gpQdmaPriv->counters.rxUnknownUnicastCounts[ringIdx], __LINE__);
				}
				gpQdmaPriv->counters.rxCounts[ringIdx]++ ;
			} 
		} else {
			QDMA_ERR("\nRX Error: rx callback function is NULL, kfree current skb\n");
			gpQdmaPriv->counters.noRxCbErrs[ringIdx]++ ;
			dev_kfree_skb_any(dscpInfoPtr->skb);
		}
next:
		if (qdma_has_free_rxdscp(ringIdx)){
			qdma_bm_hook_receive_buffer(newSkb, ringIdx);
		} else {
			QDMA_ERR("\nRX Error: no available QDMA RX descritor\n");
			gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
			dev_kfree_skb_any(newSkb);
		}
	} while((!maxPkts) || (--cnt)) ;

	return pktCount ;
}

/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_receive_packets_int2(uint maxPkts, int ringIdx) 
{
	QDMA_DMA_DSCP_T rxDscp ;
	struct QDMA_DscpInfo_S *dscpInfoPtr=NULL ;
	uint cnt = maxPkts ;
	uint pktCount = 0 ;
	int retValue = 0 ;
	struct sk_buff *newSkb = NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	int (*bmRecvCallbackFun)(void *, uint, struct sk_buff *, uint, QDMA_RxInfo_T *) = NULL ;
	QDMA_RxInfo_T rxInfo;
#ifdef TCSUPPORT_LRO_ENABLE
	LRO_RX_DSCP_T *pRxD;
	uint *rxd;
	uint agg_cnt;
#endif
	
	do {
		if(!gpQdmaPriv->rxStartPtr[ringIdx] || 
			gpQdmaPriv->rxStartPtr[ringIdx]== gpQdmaPriv->rxEndPtr[ringIdx])
		{
			QDMA_MSG(DBG_WARN, "gpQdmaPriv->rxStartPtr[%d]:%d, gpQdmaPriv->rxEndPtr[%d]:%d, \
				crtl.done:%d, cpuIdx:%d, dmaIdx:%d\n"
				, ringIdx, gpQdmaPriv->rxStartPtr[ringIdx]->dscpIdx
				, ringIdx, gpQdmaPriv->rxEndPtr[ringIdx]->dscpIdx
				, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr->ctrl.done
				, qdmaGetRxCpuIdx(base, ringIdx), qdmaGetRxDmaIdx(base, ringIdx)) ;
			return pktCount ;
		}
		memcpy(&rxDscp, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr, sizeof(QDMA_DMA_DSCP_T)) ;
		if(!rxDscp.ctrl.done)
			return pktCount ;
		dscpInfoPtr = gpQdmaPriv->rxStartPtr[ringIdx];
		
#ifdef TCSUPPORT_LRO_ENABLE
		pRxD = &rxDscp;
		rxInfo.tcp_ts_rply = pRxD->tcp_ts_rply;
		rxInfo.tcp_ack_seq = pRxD->tcp_ack_seq;
		agg_cnt = (rxDscp.msg[2] >> LRO_RXMSG_AGGCOUNT_SHIFT) & LRO_RXMSG_AGGCOUNT_MASK;
		QDMA_MSG(DBG_ST, "tcp_ts_reply:%u, tcp_ack_seq:%u\n", rxInfo.tcp_ts_rply, rxInfo.tcp_ack_seq);
		rxd = &rxDscp;
		QDMA_MSG(DBG_ST, "rxd[0]:%x, rxd[1]:%x, rxd[2]:%x, rxd[3]:%x, pkt_len:%d\n"
			, rxd[0], rxd[1], rxd[2], rxd[3], rxDscp.ctrl.pkt_len);
#endif
		gpQdmaPriv->rxStartPtr[ringIdx] = gpQdmaPriv->rxStartPtr[ringIdx]->next ;
		
		pktCount++ ;
		
		/* check DSCP cotent: if pkt_addr is NULL or pkt_len is 0, then some big issue happens, must stop running and check why */
		if(!rxDscp.pkt_addr || !rxDscp.ctrl.pkt_len)
		{
			QDMA_ERR("The content of the RX DSCP is incorrect.\n") ;
			gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ; 
			break ;
		}
		/* overflow is not critical issue, drop this packet and go-head */
#if SUPPORT_TX_DROP_BIT
		if(rxDscp.ctrl.drop_pkt)
#else
		if(rxDscp.ctrl.overflow)
#endif	
		{
			QDMA_ERR("The pkt size is overflow: ready pkt_len is %d, received pkt_len is %d.\n", skb_tailroom(dscpInfoPtr->skb), rxDscp.ctrl.pkt_len) ;
			gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ;
			newSkb = dscpInfoPtr->skb;
			goto next;
		}	

#if defined(TCSUPPORT_LRO_ENABLE) && !defined(TCSUPPORT_QDMA_VERIFY)
		if( ((INT_STATUS_RX0_DONE<<ringIdx)&INT2_MASK_RX_DONE)
#if SUPPORT_FRAGMENT_REASSEMBLE
		&& (FRAG_RX_RING_IDX != ringIdx) && (ASSE_RX_RING_IDX != ringIdx)
#endif
		)
			newSkb = allocate_skb_lro_buffer();
		else
#endif
		newSkb = allocate_skb_buffer();
		if(newSkb == NULL) {
			QDMA_MSG(DBG_WARN, "Allocate skb fail, Revert received packet as new skb.\n") ;
			gpQdmaPriv->counters.rxAllocFailDropErrs[ringIdx]++ ;
			newSkb = dscpInfoPtr->skb;
			goto next;
		}

#ifdef TCSUPPORT_CPU_ARMV8
		ecnt_dcache_inv(((unsigned long)dscpInfoPtr->skb->data), rxDscp.ctrl.pkt_len);
		mb();
#endif

#ifdef CONFIG_DEBUG
		if(gpQdmaPriv->devCfg.dbgLevel >= DBG_ST) {
            printk("rxMsg0:0x%08x, rxMsg1:0x%08x, rxMsg2:0x%08x, rxMsg3:0x%08x, pkt_len:%d\n"
                , (uint)rxDscp.msg[0], (uint)rxDscp.msg[1], (uint)rxDscp.msg[2], (uint)rxDscp.msg[3], rxDscp.ctrl.pkt_len);
            __dump_skb(dscpInfoPtr->skb, MIN(128, rxDscp.ctrl.pkt_len));
        }
#endif

#if defined(QDMA_LAN) && SUPPORT_FRAGMENT_REASSEMBLE
		if( (FRAG_RX_RING_IDX == ringIdx) || (ASSE_RX_RING_IDX == ringIdx) ) {
			retValue = qdma_fragment_reassemble_handler(dscpInfoPtr->skb, rxDscp.ctrl.pkt_len, (void *)&rxDscp, ringIdx);
			if(retValue < 0) {
				gpQdmaPriv->counters.rxPktErrs[ringIdx]++;
			} else {
				gpQdmaPriv->counters.rxCounts[ringIdx]++;
			}

			goto next;
		}
#endif

#if defined(TCSUPPORT_QDMA_VERIFY) 
       if(1 == qdma_driver_receive_and_transmit_test(dscpInfoPtr, &rxDscp, ringIdx)){
            goto next;
       }
#endif
		
		bmRecvCallbackFun = gpQdmaPriv->devCfg.bmInt2RecvCallbackFunction;

		if(bmRecvCallbackFun) {
			rxInfo.ring_idx = ringIdx;
			retValue = bmRecvCallbackFun((void *)rxDscp.msg, QDMA_RX_DSCP_MSG_LENS, dscpInfoPtr->skb, rxDscp.ctrl.pkt_len, &rxInfo);
			if(retValue< 0)
			{
				QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int2: retValue = %d, EFAULT = %d, line = %d\n", 
					retValue, EFAULT, __LINE__);
#ifdef TCSUPPORT_LRO_ENABLE
				gpQdmaPriv->counters.rxPktErrs[ringIdx] += agg_cnt;
#else
				gpQdmaPriv->counters.rxPktErrs[ringIdx]++;
#endif
			} else {
				QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int2: retValue = %d, line = %d\n", retValue, __LINE__);
#ifdef TCSUPPORT_LRO_ENABLE
				gpQdmaPriv->counters.rxCounts[ringIdx] += agg_cnt;
#else
				gpQdmaPriv->counters.rxCounts[ringIdx]++;
#endif
			} 
		} else {
			QDMA_ERR("\nRX Error: rx callback function is NULL, kfree current skb\n");
			gpQdmaPriv->counters.noRxCbErrs[ringIdx]++ ;
			dev_kfree_skb_any(dscpInfoPtr->skb);
		}
next:
		if (qdma_has_free_rxdscp(ringIdx)){
			qdma_bm_hook_receive_buffer(newSkb, ringIdx);
		} else {
			QDMA_ERR("\nRX Error: no available QDMA RX descritor\n");
			gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
			dev_kfree_skb_any(newSkb);
		}
	} while((!maxPkts) || (--cnt)) ;

	return pktCount ;
}

/******************************************************************************
******************************************************************************/
#if defined(QDMA_LAN)
#if defined(TCSUPPORT_TEST_VWTEST) || defined(TCSUPPORT_CMCC_ENTERPRISE)
#define RxMsgW1			(curRxMsg->rxMsgW1.raw)
extern int (*qdma_to_wifi_fast_tx_hook)(struct sk_buff * skb, int index);
extern struct net_device *rai0_dev;
extern int forTx5G;
extern int forRx5G;
extern unsigned int ixia_tx_counter0[3];
extern unsigned int ixia_tx_counter1[3];

extern int g_Multi_To_One;
extern unsigned char g_Global_MAC[6];
extern int g_Global_index;
extern int g_Global_skbLen;
extern int trafficSkbLen;
extern int forTx2G;
extern int (*qdma_to_wifi2g_fast_tx_hook)(struct sk_buff *skb, int index);
#ifdef MT7613E
extern int qdmaToWifiFastTxEcnt(struct sk_buff * skb, int index);
#endif
extern inline void r4k_dma_cache_inv_ecnt(unsigned long addr, unsigned long size);
extern inline void r4k_dma_cache_wback_inv_ecnt(unsigned long addr, unsigned long size);

__IMEM int qdma_bm_hook_receive_buffer_ecnt(struct sk_buff *skb, int ringIdx)
{
	struct QDMA_DscpInfo_S *pNewDscpInfo;
	QDMA_DMA_DSCP_T *pRxDscp ;
	dma_addr_t dmaPktAddr;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint dscpIdxInt;
	if(unlikely(!skb))
	{
		QDMA_ERR("The input arguments are wrong, skb is NULL.\n") ; 
		return -EFAULT ;
	}
	
	pNewDscpInfo = qdma_bm_get_unused_rx_dscp(ringIdx) ;
	if(unlikely(pNewDscpInfo == NULL)) {
		QDMA_ERR("There is not any free RX DSCP.\n") ; 
		gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
		return -ENOMEM ;
	}
	
#ifdef CONFIG_RX_2B_OFFSET
	QDMA_MSG(DBG_MSG, "Adjust the skb->tail location for net IP alignment\n") ;
#if defined(TCSUPPORT_CPU_ARMV8_64)
	if(((uint)skb->tail & 3) == 0) {
		skb_reserve(skb, 2) ;
	}
	dmaPktAddr = K0_TO_PHYSICAL(((unsigned long)skb->tail-2));
	ecnt_dcache_inv(((unsigned long)skb->tail-2), 32);
#else
	if(((uint)skb->tail & 3) == 0) {
		skb_reserve(skb, NET_IP_ALIGN) ;
	}
#ifdef TCSUPPORT_CPU_ARMV8
	dmaPktAddr = K0_TO_PHYSICAL(((unsigned long)skb->tail-NET_IP_ALIGN));
	ecnt_dcache_inv(((unsigned long)skb->tail-NET_IP_ALIGN), 32);
#else
	dmaPktAddr = K0_TO_PHYSICAL(((uint)skb->tail-NET_IP_ALIGN));
	r4k_dma_cache_inv_ecnt(((uint)skb->tail-NET_IP_ALIGN), 32);
#endif
#endif
#else /* NOT CONFIG_RX_2B_OFFSET */
#ifdef TCSUPPORT_CPU_ARMV8
	dmaPktAddr = K0_TO_PHYSICAL(((unsigned long)skb->tail));
	ecnt_dcache_inv(((unsigned long)skb->tail), skb_tailroom(skb));
#else
	dmaPktAddr = K0_TO_PHYSICAL(((uint)skb->tail));
	dma_cache_inv(((uint)skb->tail), skb_tailroom(skb));
#endif
#endif /* CONFIG_RX_2B_OFFSET */

#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4();
#else
    mb();
#endif

#ifdef TCSUPPORT_CPU_ARMV8
	pRxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->rxUsingPtr[ringIdx]->dscpPtr);
#else
	pRxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->rxUsingPtr[ringIdx]->dscpPtr);
#endif

	pRxDscp->pkt_addr = dmaPktAddr ;
	dscpIdxInt = pNewDscpInfo->dscpIdx;
	pRxDscp->next_idx = dscpIdxInt;

	*(uint32 *) &(pRxDscp->ctrl) = 0x6fe;

	QDMA_MSG(DBG_MSG, "Hook RX DSCP to RXDMA. RX_CPU_IDX:%.8x, RX_NULL_IDX:%.8x\n", 
	    gpQdmaPriv->rxUsingPtr[ringIdx]->dscpIdx, pNewDscpInfo->dscpIdx) ;
	QDMA_MSG(DBG_MSG, "RXDSCP: DONE:%d, PKT:%.8x, PKTLEN:%d, NEXT_IDX:%d\n", 
													(uint)pRxDscp->ctrl.done, 
													(uint)pRxDscp->pkt_addr,
													(uint)pRxDscp->ctrl.pkt_len,
													(uint)pRxDscp->next_idx) ;
	
#if defined(TCSUPPORT_CPU_ARMV8)
	ecnt_dcache_wback_inv(((unsigned long)(pRxDscp)), sizeof(QDMA_DMA_DSCP_T));
	mb();
#elif defined(TCSUPPORT_MIPS_1004K)
	r4k_dma_cache_wback_inv_ecnt(((unsigned long)(pRxDscp)), sizeof(QDMA_DMA_DSCP_T));
	SYNC_TYPE4();
#else
	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pRxDscp)));
#endif
	gpQdmaPriv->rxUsingPtr[ringIdx]->skb = skb ;
	gpQdmaPriv->rxUsingPtr[ringIdx] = pNewDscpInfo ;
	
	/* Setting DMA Rx Descriptor Register */
	qdmaSetRxCpuIdx(base, ringIdx, dscpIdxInt) ;
	
	return 0;
}
#endif
#endif

__IMEM int qdma_bm_receive_packets_int3(uint maxPkts, int ringIdx) 
{
	QDMA_DMA_DSCP_T rxDscp ;
	struct QDMA_DscpInfo_S *dscpInfoPtr=NULL ;
	uint cnt = maxPkts ;
	uint pktCount = 0 ;
	int retValue = 0 ;
	struct sk_buff *newSkb = NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
    int (*bmRecvCallbackFun)(void *, uint, struct sk_buff *, uint, QDMA_RxInfo_T *) = NULL ;
    QDMA_RxInfo_T rxInfo;
	
#if defined(QDMA_LAN)
#if defined(TCSUPPORT_TEST_VWTEST) || defined(TCSUPPORT_CMCC_ENTERPRISE)
	int forTx_internal = 0;
	struct QDMA_DscpInfo_S *dscpInfoPtrEnd;
	uint pkt_len_internal;
	int ret;
	if(forTx5G){
		forTx_internal = 1;
	}
	else if(forTx2G){
		forTx_internal = 2;
	}
#if defined(TCSUPPORT_NP_RAM_SHRINK) || (defined(CONFIG_TP_IMAGE) && defined(INCLUDE_SPEC_XX530v_COMBO))
	if(forTx_internal==2)
#else
	if( forTx_internal==1 || forTx_internal==2)
#endif
	{	
		do{
			dscpInfoPtr = gpQdmaPriv->rxStartPtr[ringIdx];
			dscpInfoPtrEnd = gpQdmaPriv->rxEndPtr[ringIdx];
			
			if(unlikely(!dscpInfoPtr || dscpInfoPtr == dscpInfoPtrEnd))
			{
				QDMA_MSG(DBG_WARN, "gpQdmaPriv->rxStartPtr[%d]:%d, gpQdmaPriv->rxEndPtr[%d]:%d, \
	                		crtl.done:%d, cpuIdx:%d, dmaIdx:%d\n"
					, ringIdx, gpQdmaPriv->rxStartPtr[ringIdx]->dscpIdx
					, ringIdx, gpQdmaPriv->rxEndPtr[ringIdx]->dscpIdx
					, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr->ctrl.done
					, qdmaGetRxCpuIdx(base, ringIdx), qdmaGetRxDmaIdx(base, ringIdx)) ;
				return pktCount ;
			}
			#if 0
			memcpy(&rxDscp, dscpInfoPtr->dscpPtr, sizeof(QDMA_DMA_DSCP_T));
			#else
			rxDscp.ctrl = dscpInfoPtr->dscpPtr->ctrl;
			rxDscp.pkt_addr = dscpInfoPtr->dscpPtr->pkt_addr;
			rxDscp.next_idx = dscpInfoPtr->dscpPtr->next_idx;//0;

			rxDscp.msg[0] = dscpInfoPtr->dscpPtr->msg[0];//0;
			rxDscp.msg[1] = dscpInfoPtr->dscpPtr->msg[1];
			rxDscp.msg[2] = dscpInfoPtr->dscpPtr->msg[2];//0x1c;
			rxDscp.msg[3] = dscpInfoPtr->dscpPtr->msg[3];//0;	
			#endif

			pkt_len_internal = rxDscp.ctrl.pkt_len;
			
			if(!rxDscp.ctrl.done)
				return pktCount ;
			
			gpQdmaPriv->rxStartPtr[ringIdx] = dscpInfoPtr->next ;
	
			pktCount++ ;
			
	        	newSkb = allocate_skb_buffer();
			if(unlikely(newSkb == NULL)) {
				QDMA_MSG(DBG_WARN, "Allocate skb fail, Revert received packet as new skb.\n") ;
				gpQdmaPriv->counters.rxAllocFailDropErrs[ringIdx]++ ;
				newSkb = dscpInfoPtr->skb;
				goto next_shortcut;
			}

#ifdef TCSUPPORT_CPU_ARMV8
			ecnt_dcache_inv(((unsigned long)dscpInfoPtr->skb->data), rxDscp.ctrl.pkt_len);
			mb();
#endif

			rxMsgWord_t *curRxMsg;
			struct sk_buff *skb;
			uint32 rxmsg1;
            uint32 crsn,foe_index;
			
			skb = dscpInfoPtr->skb;
			trafficSkbLen = skb->len  + pkt_len_internal;
			curRxMsg = (rxMsgWord_t*) rxDscp.msg;
			rxmsg1 = curRxMsg->rxMsgW1.word;
            qdma_get_rxmsg_crsn(curRxMsg,&crsn);
            qdma_get_rxmsg_foe_index(curRxMsg,&foe_index);
			/**************** check cpu reason *****************/
			if(0x16 == crsn)
			{
				if(forTx_internal==1){
#ifdef TCSUPPORT_CMCC_ENTERPRISE
						if(ra_sw_nat_get_wifi_dev_name)
							ret = ra_sw_nat_get_wifi_dev_name(skb);//dev_get_by_name(&init_net, "rai0"); 
						if(ret != 2)
							goto normal;
#endif
#ifdef MT7613E
					if(trafficSkbLen < 150){
						skb->tail += pkt_len_internal;
						skb->len  += pkt_len_internal;
						*(uint16 *) &(skb->foe[0]) = FOE_MAGIC_GE;
						*(uint32 *) &(skb->foe[2]) = rxmsg1;//swab32(word_internal);
						qdmaToWifiFastTxEcnt(skb, foe_index);
						goto next_shortcut;
					}
					else
#endif
					{
						skb->tail += pkt_len_internal;
						skb->len  += pkt_len_internal;	
						if((g_Multi_To_One == 1) && (skb->len >= g_Global_skbLen) && (g_Global_MAC[0] != 0xFF))
						{
							memcpy(skb->data, g_Global_MAC, 6);
							RxMsgW1.ppe_entry = g_Global_index;
						}
						*(uint16 *) &(skb->foe[0]) = FOE_MAGIC_GE;
						*(uint32 *) &(skb->foe[2]) = rxmsg1;//swab32(word_internal);
						if (qdma_to_wifi_fast_tx_hook){
							qdma_to_wifi_fast_tx_hook(skb, foe_index); 
							goto next_shortcut;
						}					
					}
				}
				else if(forTx_internal==2){
					skb->tail += pkt_len_internal;
					skb->len  += pkt_len_internal;
#ifdef MT7915N
					if((g_Multi_To_One == 1) && (skb->len >= g_Global_skbLen) && (g_Global_MAC[0] != 0xFF))
					{
						memcpy(skb->data, g_Global_MAC, 6);
						RxMsgW1.ppe_entry = g_Global_index;
					}
#endif
					*(uint16 *) &(skb->foe[0]) = FOE_MAGIC_GE;
					*(uint32 *) &(skb->foe[2]) = rxmsg1;//swab32(word_internal);
					if (qdma_to_wifi2g_fast_tx_hook){
						qdma_to_wifi2g_fast_tx_hook(skb, foe_index);
						goto next_shortcut;
					}
				}
			}
normal:
	       		bmRecvCallbackFun = gpQdmaPriv->devCfg.bmInt3RecvCallbackFunction;
	    
	        	if(bmRecvCallbackFun) {
				rxInfo.ring_idx = ringIdx;
				retValue = bmRecvCallbackFun((void *)rxDscp.msg, QDMA_RX_DSCP_MSG_LENS, dscpInfoPtr->skb, rxDscp.ctrl.pkt_len, &rxInfo);
				if(retValue< 0)
				{
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int3: retValue = %d, EFAULT = %d, line = %d\n", 
	                    			retValue, EFAULT, __LINE__);
					gpQdmaPriv->counters.rxPktErrs[ringIdx]++ ;
				} else {
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int3: retValue = %d, line = %d\n", retValue, __LINE__);
					gpQdmaPriv->counters.rxCounts[ringIdx]++ ;
				} 
			} else {
				QDMA_ERR("\nRX Error: rx callback function is NULL, kfree current skb\n");
				gpQdmaPriv->counters.noRxCbErrs[ringIdx]++ ;
				dev_kfree_skb_any(dscpInfoPtr->skb);
			}
	next_shortcut:
			if (likely(dscpInfoPtrEnd->next != dscpInfoPtr->next)){
				qdma_bm_hook_receive_buffer_ecnt(newSkb, ringIdx);
			} else {
				QDMA_ERR("\nRX Error: no available QDMA RX descritor\n");
				gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
				dev_kfree_skb_any(newSkb);
			}
		}while((!maxPkts) || (--cnt)) ;
	}else
#endif
#endif
	{
		do {
			if(!gpQdmaPriv->rxStartPtr[ringIdx] || 
            		gpQdmaPriv->rxStartPtr[ringIdx]== gpQdmaPriv->rxEndPtr[ringIdx])
			{
				QDMA_MSG(DBG_WARN, "gpQdmaPriv->rxStartPtr[%d]:%d, gpQdmaPriv->rxEndPtr[%d]:%d, \
                		crtl.done:%d, cpuIdx:%d, dmaIdx:%d\n"
				, ringIdx, gpQdmaPriv->rxStartPtr[ringIdx]->dscpIdx
				, ringIdx, gpQdmaPriv->rxEndPtr[ringIdx]->dscpIdx
				, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr->ctrl.done
				, qdmaGetRxCpuIdx(base, ringIdx), qdmaGetRxDmaIdx(base, ringIdx)) ;
				return pktCount ;
			}
			memcpy(&rxDscp, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr, sizeof(QDMA_DMA_DSCP_T)) ;
			if(!rxDscp.ctrl.done)
				return pktCount ;
			dscpInfoPtr = gpQdmaPriv->rxStartPtr[ringIdx];
		
			gpQdmaPriv->rxStartPtr[ringIdx] = gpQdmaPriv->rxStartPtr[ringIdx]->next ;
		
			pktCount++ ;
		
			/* check DSCP cotent: if pkt_addr is NULL or pkt_len is 0, then some big issue happens, must stop running and check why */
			if(!rxDscp.pkt_addr || !rxDscp.ctrl.pkt_len)
			{
				QDMA_ERR("The content of the RX DSCP is incorrect.\n") ;
				gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ; 
				break ;
			}
			/* overflow is not critical issue, drop this packet and go-head */
#if SUPPORT_TX_DROP_BIT
			if(rxDscp.ctrl.drop_pkt)
#else
			if(rxDscp.ctrl.overflow)
#endif	
			{
				QDMA_ERR("The pkt size is overflow: ready pkt_len is %d, received pkt_len is %d.\n", skb_tailroom(dscpInfoPtr->skb), rxDscp.ctrl.pkt_len) ;
				gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ;
				newSkb = dscpInfoPtr->skb;
				goto next;
			}	
		
        		newSkb = allocate_skb_buffer();
			if(newSkb == NULL) {
				QDMA_MSG(DBG_WARN, "Allocate skb fail, Revert received packet as new skb.\n") ;
				gpQdmaPriv->counters.rxAllocFailDropErrs[ringIdx]++ ;
				newSkb = dscpInfoPtr->skb;
				goto next;
			}
			
#ifdef TCSUPPORT_CPU_ARMV8
            ecnt_dcache_inv(((unsigned long)dscpInfoPtr->skb->data), rxDscp.ctrl.pkt_len);
            mb();
#endif

        		bmRecvCallbackFun = gpQdmaPriv->devCfg.bmInt3RecvCallbackFunction;
    
        		if(bmRecvCallbackFun) {
				rxInfo.ring_idx = ringIdx;
				retValue = bmRecvCallbackFun((void *)rxDscp.msg, QDMA_RX_DSCP_MSG_LENS, dscpInfoPtr->skb, rxDscp.ctrl.pkt_len, &rxInfo);
				if(retValue< 0)
				{
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int3: retValue = %d, EFAULT = %d, line = %d\n", 
                   				retValue, EFAULT, __LINE__);
					gpQdmaPriv->counters.rxPktErrs[ringIdx]++ ;
				} else {
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int3: retValue = %d, line = %d\n", retValue, __LINE__);
					gpQdmaPriv->counters.rxCounts[ringIdx]++ ;
				} 	
			} else {
				QDMA_ERR("\nRX Error: rx callback function is NULL, kfree current skb\n");
				gpQdmaPriv->counters.noRxCbErrs[ringIdx]++ ;
				dev_kfree_skb_any(dscpInfoPtr->skb);
			}
next:
			if (qdma_has_free_rxdscp(ringIdx)){
				qdma_bm_hook_receive_buffer(newSkb, ringIdx);
			} else {
				QDMA_ERR("\nRX Error: no available QDMA RX descritor\n");
				gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
				dev_kfree_skb_any(newSkb);
			}
		} while((!maxPkts) || (--cnt)) ;
	}
	return pktCount ;
}

/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_receive_packets_int4(uint maxPkts, int ringIdx) 
{
	QDMA_DMA_DSCP_T rxDscp ;
	struct QDMA_DscpInfo_S *dscpInfoPtr=NULL ;
	uint cnt = maxPkts ;
	uint pktCount = 0 ;
	int retValue = 0 ;
	struct sk_buff *newSkb = NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
    int (*bmRecvCallbackFun)(void *, uint, struct sk_buff *, uint, QDMA_RxInfo_T *) = NULL ;
    QDMA_RxInfo_T rxInfo;
	
#if defined(QDMA_LAN)
#if defined(TCSUPPORT_TEST_VWTEST) || defined(TCSUPPORT_CMCC_ENTERPRISE)
	int forTx_internal = 0;
	struct QDMA_DscpInfo_S *dscpInfoPtrEnd;
	uint pkt_len_internal;
	int ret;
	if(forTx5G){
		forTx_internal = 1;
	}
	else if(forTx2G){
		forTx_internal = 2;
	}
#if defined(TCSUPPORT_NP_RAM_SHRINK) || (defined(CONFIG_TP_IMAGE) && defined(INCLUDE_SPEC_XX530v_COMBO))
	if(forTx_internal == 2)
#else
	if( forTx_internal == 1 || forTx_internal == 2)
#endif
	{	
		do{
			dscpInfoPtr = gpQdmaPriv->rxStartPtr[ringIdx];
			dscpInfoPtrEnd = gpQdmaPriv->rxEndPtr[ringIdx];
			
			if(unlikely(!dscpInfoPtr || dscpInfoPtr == dscpInfoPtrEnd))
			{
				QDMA_MSG(DBG_WARN, "gpQdmaPriv->rxStartPtr[%d]:%d, gpQdmaPriv->rxEndPtr[%d]:%d, \
	                		crtl.done:%d, cpuIdx:%d, dmaIdx:%d\n"
					, ringIdx, gpQdmaPriv->rxStartPtr[ringIdx]->dscpIdx
					, ringIdx, gpQdmaPriv->rxEndPtr[ringIdx]->dscpIdx
					, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr->ctrl.done
					, qdmaGetRxCpuIdx(base, ringIdx), qdmaGetRxDmaIdx(base, ringIdx)) ;
				return pktCount ;
			}
			#if 0
			memcpy(&rxDscp, dscpInfoPtr->dscpPtr, sizeof(QDMA_DMA_DSCP_T));
			#else
			rxDscp.ctrl = dscpInfoPtr->dscpPtr->ctrl;
			rxDscp.pkt_addr = dscpInfoPtr->dscpPtr->pkt_addr;
			rxDscp.next_idx = dscpInfoPtr->dscpPtr->next_idx;//0;

			rxDscp.msg[0] = dscpInfoPtr->dscpPtr->msg[0];//0;
			rxDscp.msg[1] = dscpInfoPtr->dscpPtr->msg[1];
			rxDscp.msg[2] = dscpInfoPtr->dscpPtr->msg[2];//0x1c;
			rxDscp.msg[3] = dscpInfoPtr->dscpPtr->msg[3];//0;	
			#endif
	
			pkt_len_internal = rxDscp.ctrl.pkt_len;
			
			if(!rxDscp.ctrl.done)
				return pktCount ;
			
			gpQdmaPriv->rxStartPtr[ringIdx] = dscpInfoPtr->next ;
			
			pktCount++ ;
			
	       	 	newSkb = allocate_skb_buffer();
			if(unlikely(newSkb == NULL)) {
				QDMA_MSG(DBG_WARN, "Allocate skb fail, Revert received packet as new skb.\n") ;
				gpQdmaPriv->counters.rxAllocFailDropErrs[ringIdx]++ ;
				newSkb = dscpInfoPtr->skb;
				goto next_shortcut;
			}

#ifdef TCSUPPORT_CPU_ARMV8
			ecnt_dcache_inv(((unsigned long)dscpInfoPtr->skb->data), rxDscp.ctrl.pkt_len);
			mb();
#endif
			
			rxMsgWord_t *curRxMsg;
			struct sk_buff *skb;
			uint32 rxmsg1;
            uint32 crsn,foe_index;
				
			skb = dscpInfoPtr->skb;
			trafficSkbLen = skb->len  + pkt_len_internal;
			curRxMsg = (rxMsgWord_t*) rxDscp.msg;			
			rxmsg1 = curRxMsg->rxMsgW1.word;
            qdma_get_rxmsg_crsn(curRxMsg,&crsn);
            qdma_get_rxmsg_foe_index(curRxMsg,&foe_index);
			/**************** check cpu reason *****************/
			if(0x16 == crsn)
			{
				if(forTx_internal==1){
#ifdef TCSUPPORT_CMCC_ENTERPRISE
					if(ra_sw_nat_get_wifi_dev_name)
						ret = ra_sw_nat_get_wifi_dev_name(skb);//dev_get_by_name(&init_net, "rai0"); 
					if(ret != 2)
						goto normal;
#endif
#ifdef MT7613E
					if(trafficSkbLen < 150){
						skb->tail += pkt_len_internal;
						skb->len  += pkt_len_internal;
						*(uint16 *) &(skb->foe[0]) = FOE_MAGIC_GE;
						*(uint32 *) &(skb->foe[2]) = rxmsg1;//swab32(word_internal);
						qdmaToWifiFastTxEcnt(skb, foe_index);
						goto next_shortcut;
					}
					else
#endif
					{
						skb->tail += pkt_len_internal;
						skb->len  += pkt_len_internal; 						
						if((g_Multi_To_One == 1) && (skb->len >= g_Global_skbLen) && (g_Global_MAC[0] != 0xFF))
						{
							memcpy(skb->data, g_Global_MAC, 6);
							RxMsgW1.ppe_entry = g_Global_index;
						}                                         
						*(uint16 *) &(skb->foe[0]) = FOE_MAGIC_GE;
						*(uint32 *) &(skb->foe[2]) = rxmsg1;//swab32(word_internal);
						if (qdma_to_wifi_fast_tx_hook){
							qdma_to_wifi_fast_tx_hook(skb, foe_index); 
							goto next_shortcut;
						}					
					}
				}
				else if(forTx_internal==2){
					skb->tail += pkt_len_internal;
					skb->len  += pkt_len_internal;                                  
					*(uint16 *) &(skb->foe[0]) = FOE_MAGIC_GE;
					*(uint32 *) &(skb->foe[2]) = rxmsg1;//swab32(word_internal);
					if (qdma_to_wifi2g_fast_tx_hook){
						qdma_to_wifi2g_fast_tx_hook(skb, foe_index);
						goto next_shortcut;
					}
				}
			}
normal:
	       	 	bmRecvCallbackFun = gpQdmaPriv->devCfg.bmInt4RecvCallbackFunction;
	    
	        	if(bmRecvCallbackFun) {
				rxInfo.ring_idx = ringIdx;
				retValue = bmRecvCallbackFun((void *)rxDscp.msg, QDMA_RX_DSCP_MSG_LENS, dscpInfoPtr->skb, rxDscp.ctrl.pkt_len, &rxInfo);
				if(retValue< 0)
				{
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int4: retValue = %d, EFAULT = %d, line = %d\n", 
	                    retValue, EFAULT, __LINE__);
					gpQdmaPriv->counters.rxPktErrs[ringIdx]++ ;
				} else {
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int4: retValue = %d, line = %d\n", retValue, __LINE__);
					gpQdmaPriv->counters.rxCounts[ringIdx]++ ;
				} 
			} else {
				QDMA_ERR("\nRX Error: rx callback function is NULL, kfree current skb\n");
				gpQdmaPriv->counters.noRxCbErrs[ringIdx]++ ;
				dev_kfree_skb_any(dscpInfoPtr->skb);
			}
next_shortcut:
			if (likely(dscpInfoPtrEnd->next != dscpInfoPtr->next)){
					qdma_bm_hook_receive_buffer_ecnt(newSkb, ringIdx);
			} else {
				QDMA_ERR("\nRX Error: no available QDMA RX descritor\n");
				gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
				dev_kfree_skb_any(newSkb);
			}
		} while((!maxPkts) || (--cnt)) ;
	}else
#endif
#endif
	{
		do {
			if(!gpQdmaPriv->rxStartPtr[ringIdx] || 
            			gpQdmaPriv->rxStartPtr[ringIdx]== gpQdmaPriv->rxEndPtr[ringIdx])
			{
				QDMA_MSG(DBG_WARN, "gpQdmaPriv->rxStartPtr[%d]:%d, gpQdmaPriv->rxEndPtr[%d]:%d, \
                			crtl.done:%d, cpuIdx:%d, dmaIdx:%d\n"
					, ringIdx, gpQdmaPriv->rxStartPtr[ringIdx]->dscpIdx
					, ringIdx, gpQdmaPriv->rxEndPtr[ringIdx]->dscpIdx
					, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr->ctrl.done
					, qdmaGetRxCpuIdx(base, ringIdx), qdmaGetRxDmaIdx(base, ringIdx)) ;
				return pktCount ;
			}
			memcpy(&rxDscp, gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr, sizeof(QDMA_DMA_DSCP_T)) ;
			if(!rxDscp.ctrl.done)
				return pktCount ;
			dscpInfoPtr = gpQdmaPriv->rxStartPtr[ringIdx];
		
			gpQdmaPriv->rxStartPtr[ringIdx] = gpQdmaPriv->rxStartPtr[ringIdx]->next ;
		
			pktCount++ ;
		
			/* check DSCP cotent: if pkt_addr is NULL or pkt_len is 0, then some big issue happens, must stop running and check why */
			if(!rxDscp.pkt_addr || !rxDscp.ctrl.pkt_len)
			{
				QDMA_ERR("The content of the RX DSCP is incorrect.\n") ;
				gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ; 
				break ;
			}
		/* overflow is not critical issue, drop this packet and go-head */
#if SUPPORT_TX_DROP_BIT
			if(rxDscp.ctrl.drop_pkt)
#else
			if(rxDscp.ctrl.overflow)
#endif	
			{
				QDMA_ERR("The pkt size is overflow: ready pkt_len is %d, received pkt_len is %d.\n", skb_tailroom(dscpInfoPtr->skb), rxDscp.ctrl.pkt_len) ;
				gpQdmaPriv->counters.rxDscpIncorrect[ringIdx]++ ;
				newSkb = dscpInfoPtr->skb;
				goto next;
			}	

        		newSkb = allocate_skb_buffer();
			if(newSkb == NULL) {
				QDMA_MSG(DBG_WARN, "Allocate skb fail, Revert received packet as new skb.\n") ;
				gpQdmaPriv->counters.rxAllocFailDropErrs[ringIdx]++ ;
				newSkb = dscpInfoPtr->skb;
				goto next;
			}

	
#ifdef TCSUPPORT_CPU_ARMV8
				ecnt_dcache_inv(((unsigned long)dscpInfoPtr->skb->data), rxDscp.ctrl.pkt_len);
				mb();
#endif
       			bmRecvCallbackFun = gpQdmaPriv->devCfg.bmInt4RecvCallbackFunction;
    
        		if(bmRecvCallbackFun) {
				rxInfo.ring_idx = ringIdx;
				retValue = bmRecvCallbackFun((void *)rxDscp.msg, QDMA_RX_DSCP_MSG_LENS, dscpInfoPtr->skb, rxDscp.ctrl.pkt_len, &rxInfo);
				if(retValue< 0)
				{
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int4: retValue = %d, EFAULT = %d, line = %d\n", 
                    				retValue, EFAULT, __LINE__);
					gpQdmaPriv->counters.rxPktErrs[ringIdx]++ ;
				} else {
					QDMA_MSG(DBG_WARN, "qdma_bm_receive_packets_int4: retValue = %d, line = %d\n", retValue, __LINE__);
					gpQdmaPriv->counters.rxCounts[ringIdx]++ ;
				} 
			} else {
				QDMA_ERR("\nRX Error: rx callback function is NULL, kfree current skb\n");
				gpQdmaPriv->counters.noRxCbErrs[ringIdx]++ ;
				dev_kfree_skb_any(dscpInfoPtr->skb);
			}
next:
			if (qdma_has_free_rxdscp(ringIdx)){
				qdma_bm_hook_receive_buffer(newSkb, ringIdx);
			} else {
				QDMA_ERR("\nRX Error: no available QDMA RX descritor\n");
				gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
				dev_kfree_skb_any(newSkb);
			}
		} while((!maxPkts) || (--cnt)) ;
	}

	return pktCount ;
}


/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_hook_receive_buffer(struct sk_buff *skb, int ringIdx)
{
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pRxDscp=NULL ;
	dma_addr_t dmaPktAddr=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	unsigned char *skb_tail=NULL;

	if(!skb)
	{
		QDMA_ERR("The input arguments are wrong, skb is NULL.\n") ; 
		return -EFAULT ;
	}

	skb_tail = skb_tail_pointer(skb);
	
	pNewDscpInfo = qdma_bm_get_unused_rx_dscp(ringIdx) ;
	if(pNewDscpInfo == NULL) {
		QDMA_ERR("There is not any free RX DSCP.\n") ; 
		gpQdmaPriv->counters.noRxDscps[ringIdx]++ ;
		return -ENOMEM ;
	}
	
#ifdef CONFIG_RX_2B_OFFSET
	QDMA_MSG(DBG_MSG, "Adjust the skb->tail location for net IP alignment\n") ;
#if defined(TCSUPPORT_CPU_ARMV8_64)
	if(((uint)skb_tail & 3) == 0) {
		skb_reserve(skb, 2) ;
	}
	dmaPktAddr = K0_TO_PHYSICAL(((unsigned long)skb_tail-2));
	ecnt_dcache_inv(((unsigned long)skb_tail-2), skb_tailroom(skb));
#else
	if(((uint)skb_tail & 3) == 0) {
		skb_reserve(skb, NET_IP_ALIGN) ;
	}
#ifdef TCSUPPORT_CPU_ARMV8
	dmaPktAddr = K0_TO_PHYSICAL(((unsigned long)skb_tail-NET_IP_ALIGN));
	ecnt_dcache_inv(((unsigned long)skb_tail-NET_IP_ALIGN), skb_tailroom(skb));
#else
	dmaPktAddr = K0_TO_PHYSICAL(((uint)skb_tail-NET_IP_ALIGN));
	dma_cache_inv(((uint)skb_tail-NET_IP_ALIGN), skb_tailroom(skb));
#endif
#endif
#else /* NOT CONFIG_RX_2B_OFFSET */
#ifdef TCSUPPORT_CPU_ARMV8
	dmaPktAddr = K0_TO_PHYSICAL(((unsigned long)skb_tail));
	ecnt_dcache_inv(((unsigned long)skb_tail), skb_tailroom(skb));
#else
	dmaPktAddr = K0_TO_PHYSICAL(((uint)skb_tail));
	dma_cache_inv(((uint)skb_tail), skb_tailroom(skb));
#endif
#endif /* CONFIG_RX_2B_OFFSET */

#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4();
#else
    mb();
#endif

#ifdef TCSUPPORT_CPU_ARMV8
	pRxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->rxUsingPtr[ringIdx]->dscpPtr);
#else
	pRxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->rxUsingPtr[ringIdx]->dscpPtr);
#endif

	memset (&pRxDscp->msg[0], 0, (QDMA_DSCP_MSG_WORD_LENS<<2));
	pRxDscp->pkt_addr = dmaPktAddr ;
	pRxDscp->next_idx = pNewDscpInfo->dscpIdx ;
	pRxDscp->ctrl.pkt_len = skb_tailroom(skb) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
	pRxDscp->ctrl.done = gpQdmaPriv->devCfg.rxDscpDoneBit ;
#else
	pRxDscp->ctrl.done = 0 ;
#endif /* CONFIG_SUPPORT_SELF_TEST */
#if SUPPORT_TX_DROP_BIT
    pRxDscp->ctrl.drop_pkt = 0 ;
#else
    pRxDscp->ctrl.overflow = 0 ;
#endif	

	QDMA_MSG(DBG_MSG, "Hook RX DSCP to RXDMA. RX_CPU_IDX:%.8x, RX_NULL_IDX:%.8x\n", 
	    gpQdmaPriv->rxUsingPtr[ringIdx]->dscpIdx, pNewDscpInfo->dscpIdx) ;
	QDMA_MSG(DBG_MSG, "RXDSCP: DONE:%d, PKT:%.8x, PKTLEN:%d, NEXT_IDX:%d\n", 
													(uint)pRxDscp->ctrl.done, 
													(uint)pRxDscp->pkt_addr,
													(uint)pRxDscp->ctrl.pkt_len,
													(uint)pRxDscp->next_idx) ;
	
#if defined(TCSUPPORT_CPU_ARMV8)
	ecnt_dcache_wback_inv(((unsigned long)(pRxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
	dma_cache_wback_inv(((unsigned long)(pRxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pRxDscp)));
#endif
	gpQdmaPriv->rxUsingPtr[ringIdx]->skb = skb ;
	gpQdmaPriv->rxUsingPtr[ringIdx] = pNewDscpInfo ;

#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4();
#else
    mb();
#endif
	
	/* Setting DMA Rx Descriptor Register */
	qdmaSetRxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;
	
	return 0;
}

/******************************************************************************
******************************************************************************/
int qdma_bm_recycle_receive_buffer(int ringIdx) 
{
	QDMA_DMA_DSCP_T *pRxDscp=NULL ;
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;

	if(!gpQdmaPriv->rxStartPtr[ringIdx] || !gpQdmaPriv->rxEndPtr[ringIdx]) {
		return -EFAULT ;
	}

	while(gpQdmaPriv->rxStartPtr[ringIdx] != gpQdmaPriv->rxEndPtr[ringIdx]) {
		diPtr = gpQdmaPriv->rxStartPtr[ringIdx] ;
		pRxDscp = gpQdmaPriv->rxStartPtr[ringIdx]->dscpPtr ;

		if(diPtr->skb && pRxDscp->pkt_addr) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			dma_unmap_single(NULL, pRxDscp->pkt_addr, skb_tailroom(diPtr->skb), DMA_FROM_DEVICE) ;	
#else
			dma_unmap_single(qdma_dummy_dev_ptr, pRxDscp->pkt_addr, skb_tailroom(diPtr->skb), DMA_FROM_DEVICE) ;	
#endif
			if(gpQdmaPriv->devCfg.bmRecvCallbackFunction) {
				gpQdmaPriv->devCfg.bmRecvCallbackFunction((void *)pRxDscp->msg, QDMA_RX_DSCP_MSG_LENS, diPtr->skb, pRxDscp->ctrl.pkt_len) ;
			} else {
				QDMA_MSG(DBG_WARN, "\nrx callback function is NULL, kfree current skb\n");
				gpQdmaPriv->counters.noRxCbErrs[ringIdx]++ ;
				dev_kfree_skb_any(diPtr->skb);
			}
		}
		
		gpQdmaPriv->rxStartPtr[ringIdx] = diPtr->next ;		
	}

	diPtr = gpQdmaPriv->rxStartPtr[ringIdx] ;
	gpQdmaPriv->rxUsingPtr[ringIdx] = diPtr ;
	qdmaSetRxCpuIdx(base, ringIdx, diPtr->dscpIdx) ;
	qdmaSetRxDmaIdx(base, ringIdx, diPtr->dscpIdx) ;
	
	
	return 0 ;
}

/******************************************************************************
 Packet Transmit
******************************************************************************/
/******************************************************************************
******************************************************************************/
static inline int qdma_bm_push_tx_dscp(struct QDMA_DscpInfo_S *diPtr, int ringIdx) 
{
	if(diPtr->next != NULL) {
		QDMA_ERR("The TX DSCP is not return from tx used pool\n") ;
		return -1 ;
	}

	diPtr->skb = NULL ;
	if(!gpQdmaPriv->txHeadPtr[ringIdx]) {
		gpQdmaPriv->txHeadPtr[ringIdx] = diPtr ;
		gpQdmaPriv->txTailPtr[ringIdx] = diPtr ;
	} else {
		gpQdmaPriv->txTailPtr[ringIdx]->next = diPtr ;
		gpQdmaPriv->txTailPtr[ringIdx] = gpQdmaPriv->txTailPtr[ringIdx]->next ;
	}

	atomic_inc(&free_txd_count[ringIdx]);

	return 0 ;
}


/******************************************************************************
******************************************************************************/
inline struct QDMA_DscpInfo_S *qdma_bm_pop_tx_dscp(int ringIdx)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	
	diPtr = gpQdmaPriv->txHeadPtr[ringIdx] ;
	if(gpQdmaPriv->txHeadPtr[ringIdx] == gpQdmaPriv->txTailPtr[ringIdx]) {
		gpQdmaPriv->txHeadPtr[ringIdx] = NULL ;
		gpQdmaPriv->txTailPtr[ringIdx] = NULL ;
	} else {
		gpQdmaPriv->txHeadPtr[ringIdx] = gpQdmaPriv->txHeadPtr[ringIdx]->next ;
	}

	if(diPtr) {
		diPtr->next = NULL ;
		
		atomic_dec(&free_txd_count[ringIdx]);
	}
	
	return diPtr ;
}

static inline int channel_is_recorded(unsigned char channel, int num, unsigned char *channelPtr)
{
	int i = 0 ;
	unsigned char * tmpPtr = channelPtr;
	for(i = 0; i < num; i++){
		if(channel == *tmpPtr){
			/*This channel id has been recorded already.*/
			return 0;
		}
		tmpPtr ++;
	}
	return -1;
}

__IMEM int qdma_get_using_tx_dscp_channel(struct ECNT_QDMA_Data *qdma_data)
{
	struct QDMA_DscpInfo_S *diPtr = NULL;
	int ringIdx = 0;
	int i = 0;
	int number = 0;
	unsigned char * tmpPtr = NULL;
	unsigned char channel = 0;

	if(NULL == qdma_data)
	{
		QDMA_ERR("The input arguments are wrong.\n"); 
		return -EFAULT;
	}

	if(NULL != gpQdmaPriv->txHeadPtr[ringIdx]) {
		QDMA_MSG(DBG_MSG,"txHeadPtr is not NULL\n"); 
		return -ENOSR;
	}

	tmpPtr = qdma_data->qdma_private.qdma_dscp_chnl_info->channel;

	for(i = 0; i < gpQdmaPriv->txDscpNum[ringIdx]; i++) {
		diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + i;
		if(diPtr->next == NULL && diPtr!=gpQdmaPriv->txTailPtr[ringIdx] && !diPtr->dscpPtr->ctrl.done) {
			QDMA_MSG(DBG_MSG," msg[0]:%.8x\n", diPtr->dscpPtr->msg[0]);
			channel = (unsigned char)(diPtr->dscpPtr->msg[0] >> 3);
			if(channel_is_recorded(channel, number, qdma_data->qdma_private.qdma_dscp_chnl_info->channel) != 0){
				*tmpPtr = channel;
				number ++;
				if(number > CONFIG_QDMA_CHANNEL){
					QDMA_ERR("Channel number = %d, exceeds CONFIG_QDMA_CHANNEL(%d)\n", number, CONFIG_QDMA_CHANNEL) ;
					return -EFAULT;
				}
				tmpPtr ++;
				QDMA_MSG(DBG_MSG," channel = %d\n", channel) ;
			}
		}
	}
	qdma_data->qdma_private.qdma_dscp_chnl_info->chnlNum = number;
	QDMA_MSG(DBG_MSG," num = %d\n", number) ;

	return 0;
}

/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_transmit_done(int amount) 
{
	QDMA_DMA_DSCP_T *txDscpPtr=NULL ;
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	int i=0, j=0, idx=0, ringIdx=0, ret=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint entryLen=0, headIdx=0, irqValue=0;
	uint RETRY=5, irqStatus=0 ; 
	volatile uint *irqPtr=NULL ;
	ulong flags=0 ;
#ifdef CONFIG_SUPPORT_SELF_TEST
	txMsgWord0_t txMsgW0;
	uint channel=0, queue=0 ;
#endif
	struct ECNT_QDMA_Data qdma_data;

	if(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE)
		spin_lock_irqsave(&gpQdmaPriv->txDoneLock, flags) ;
	irqStatus = qdmaGetIrqStatus(base) ;
	headIdx = (irqStatus & IRQ_STATUS_HEAD_IDX_MASK) >> IRQ_STATUS_HEAD_IDX_SHIFT ;
	entryLen = (irqStatus & IRQ_STATUS_ENTRY_LEN_MASK) >> IRQ_STATUS_ENTRY_LEN_SHIFT ;
	if(entryLen == 0) {
#ifdef CONFIG_SUPPORT_SELF_TEST
		gpQdmaPriv->counters.IrqErr++ ;
#endif
		goto out2 ;
	}

	entryLen = (amount && amount<entryLen) ? amount : entryLen ;
	for(i=0 ; i<entryLen ; i++) {
		irqPtr = (uint *)gpQdmaPriv->irqQueueAddr + ((headIdx+i) % CONFIG_IRQ_DEPTH) ;

#if defined(TCSUPPORT_CPU_EN7580)
		RETRY = 15 ;
#else
		RETRY = 5 ;
#endif
		while(RETRY--) {
			irqValue = *irqPtr ;
			if(irqValue == CONFIG_IRQ_DEF_VALUE) {
				if(RETRY == 0){
					QDMA_ERR("There is no data available in IRQ queue. irq value:%.8x, irq ptr:%.8x TIMEs:%d\n", (uint)irqValue, (uint)irqPtr, RETRY) ;
#if defined(TCSUPPORT_QDMA_VERIFY)
                    printk("irqStatus = %.8X \n",irqStatus);
                    qdma_dump_irq_info(&qdma_data) ;
#endif
                }
				if(RETRY <= 0) {
					gpQdmaPriv->counters.IrqQueueAsynchronous++ ;
					ret = -ENODATA ;
					goto out1 ;
				}
			} else {
				*irqPtr = CONFIG_IRQ_DEF_VALUE ;
				break ;
			}
		}
		
		idx = (irqValue & IRQ_CFG_IDX_MASK) ;
		ringIdx = (irqValue & IRQ_CFG_RINGIDX_MASK) >> IRQ_CFG_RINGIDX_SHIFT;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
#if defined(QDMA_LAN)
		if(aggressive_offload_short_cut_mode)
		{
			if(OFFLOAD_TX_RING == ringIdx              
#ifdef TWO_WAY_LEFT_TO_RIGHT
#ifndef LEFT_TO_RIGHT_QDMA_WAN_TX_2 
				|| (two_way_left_to_right_flag && (OFFLOAD_TX_RING_2 == ringIdx))
#endif
#endif
			)
			{
#ifdef TCSUPPORT_NEW_WDOG
				wdog_kick_api(1);
#endif
				continue;
			}
		}
#else
#ifdef TWO_WAY_LEFT_TO_RIGHT
#ifdef LEFT_TO_RIGHT_QDMA_WAN_TX_2  
		if(aggressive_offload_short_cut_mode)
		{
			if(two_way_left_to_right_flag && (OFFLOAD_TX_RING_2 == ringIdx))
			{
#ifdef TCSUPPORT_NEW_WDOG
				wdog_kick_api(1);
#endif
				continue;
			}
		}
#endif
#endif
#endif
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_CPU_EN7580) /*EN7580 only has IRQ INT, here can know ring index*/
#ifndef CONFIG_SUPPORT_SELF_TEST
		gpQdmaPriv->counters.intTxDone[ringIdx]++ ;
#endif
#endif
		if(idx<0 || idx>=gpQdmaPriv->txDscpNum[ringIdx]) {
			QDMA_ERR("The TX DSCP index %d is invalid.\n", idx) ;
#if defined(TCSUPPORT_QDMA_VERIFY)
            printk("irqStatus = %.8X \n",irqStatus);
            qdma_dump_irq_info(&qdma_data) ;
#endif
			gpQdmaPriv->counters.txIrqQueueIdxErrs++ ;
			ret = -EFAULT ;
			continue ;
		}
		
		diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + idx ;
		if(diPtr->dscpIdx!=idx || diPtr->next!=NULL) {
			QDMA_ERR("The content of the TX DSCP_INFO(%.8x) is incorrect. ENTRY_LEN:%d, HEAD_IDX:%d, IRQ_VALUE:%.8x.\n", (uint)diPtr, entryLen, headIdx+i, irqValue) ;
			gpQdmaPriv->counters.txDscpIncorrect[ringIdx]++ ;
			ret = -EFAULT ;
			continue ;
		}
		
		txDscpPtr = diPtr->dscpPtr;

#ifdef CONFIG_TX_WB_DONE
		/*done or drop for EN7580 , only done for others*/
		if( (!txDscpPtr->ctrl.done)
#if SUPPORT_TX_DROP_BIT
			&& (!txDscpPtr->ctrl.drop_pkt)
#endif
			) 
		{
			QDMA_ERR("The done/drop bit of TX DSCP is incorrect. ADDR:%.8x, IDX:%d.\n", (uint)diPtr->dscpPtr, diPtr->dscpIdx) ;
			gpQdmaPriv->counters.txDscpDoneErrs[ringIdx]++ ;
			ret = -EFAULT ;
			continue ;
		}
#if SUPPORT_TX_DROP_BIT
		if(txDscpPtr->ctrl.drop_pkt)
		{
			gpQdmaPriv->counters.txDscpDropErrs[ringIdx]++ ;
		}
#endif

#endif /* CONFIG_TX_WB_DONE */

#ifdef CONFIG_SUPPORT_SELF_TEST
		txMsgW0.word = txDscpPtr->msg[0] ;
		channel = txMsgW0.channel ;
		queue = txMsgW0.queue ;
		counters[channel][queue].tx_frames++;
#endif
		gpQdmaPriv->counters.txRecycleCounts[ringIdx]++ ;

#if !defined(TCSUPPORT_TSO_ENABLE)
		if(diPtr->skb == NULL)
        {      
			QDMA_ERR("diPtr->skb is NULL.\n") ;	
#if defined(TCSUPPORT_QDMA_VERIFY)
            printk("irqStatus = %.8X \n",irqStatus);
            qdma_dump_irq_info(&qdma_data) ;
#endif
            goto out1 ;
        }
#endif

		/* diPtr->skb == NULL is a valid case, when scatter skb is sent, and shouldn't be NULL when nls is 0. 
			but nls will be set 0,when HW writeback done bit.*/		
		if(diPtr->skb != NULL){
#if defined(TCSUPPORT_TSO_VERIFY)
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
			struct bio_vec *frag;
#else
			struct skb_frag_struct *frag;
#endif
			unsigned int nr_frags;
			int frag_idx;
			nr_frags = skb_shinfo(diPtr->skb)->nr_frags;
			if(nr_frags > 0){
				for(frag_idx=0; frag_idx<nr_frags; frag_idx++){						
					frag = &skb_shinfo(diPtr->skb)->frags[frag_idx];
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
					if(frag->bv_page){
						__free_pages(frag->bv_page ,get_order(frag->bv_len));
#else 
					if(frag->page.p){
						__free_pages(frag->page.p ,get_order(frag->size));
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)	
						diPtr->skb->len			-= frag->bv_len;
						diPtr->skb->data_len	-= frag->bv_len;
						diPtr->skb->truesize	-= frag->bv_len;
						frag->bv_offset = 0;
						frag->bv_len = 0;
#else

						diPtr->skb->len			-= frag->size;
						diPtr->skb->data_len	-= frag->size;
						diPtr->skb->truesize	-= frag->size;
						frag->page_offset = 0;
						frag->size = 0;
#endif		

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)	
						frag->bv_page =  NULL;
#else
						frag->page.p = NULL;
#endif
					}
				}
				skb_shinfo(diPtr->skb)->nr_frags = 0;
			}
#endif
			if(qdma_kfree_skb(diPtr->skb, txDscpPtr->msg[0], ringIdx) != 0) {
				QDMA_ERR("qdma_kfree_skb fail:headIdx:%d, dscpIdx:%d, ringIdx:%d.\n", headIdx+i, idx, ringIdx) ;
				qdma_dump_irq_info(&qdma_data) ;
				ret = -EFAULT ;	
				goto out1 ;		
			}
		}
		txDscpPtr->msg[0] = 0;
		txDscpPtr->msg[1] = 0;
		
		spin_lock(&gpQdmaPriv->txLock[ringIdx]) ;
		qdma_bm_push_tx_dscp(diPtr, ringIdx) ;
		spin_unlock(&gpQdmaPriv->txLock[ringIdx]) ;
	}

out1:
	if(i!=0) {
		for(j=0 ; j<(i>>7) ; j++) {
			qdmaSetIrqClearLen(base, 0x80) ;
		}
		qdmaSetIrqClearLen(base, (i&0x7F)) ;
	}

out2:
	if(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE)
			spin_unlock_irqrestore(&gpQdmaPriv->txDoneLock, flags) ;
	return ret ;
}

/******************************************************************************
******************************************************************************/
#if SUPPORT_IRQ2
__IMEM int qdma_bm_transmit_done_2(int amount) 
{
	QDMA_DMA_DSCP_T *txDscpPtr=NULL ;
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	int i=0, j=0, idx=0, ringIdx=0, ret=0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint entryLen=0, headIdx=0, irq2Value=0;
	uint RETRY=5, irq2Status=0 ; 
	volatile uint *irq2Ptr=NULL ;
	ulong flags=0 ;
#ifdef CONFIG_SUPPORT_SELF_TEST
	txMsgWord0_t txMsgW0;
	uint channel=0, queue=0 ;
#endif
	struct ECNT_QDMA_Data qdma_data;

	if(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE)
		spin_lock_irqsave(&gpQdmaPriv->txDoneLock, flags) ;
	irq2Status = qdmaGetIrq2Status(base) ;
	headIdx = (irq2Status & IRQ_STATUS_HEAD_IDX_MASK) >> IRQ_STATUS_HEAD_IDX_SHIFT ;
	entryLen = (irq2Status & IRQ_STATUS_ENTRY_LEN_MASK) >> IRQ_STATUS_ENTRY_LEN_SHIFT ;
	if(entryLen == 0) {
#ifdef CONFIG_SUPPORT_SELF_TEST
		gpQdmaPriv->counters.Irq2Err++ ;
#endif
		goto out2 ;
	}

	entryLen = (amount && amount<entryLen) ? amount : entryLen ;
	for(i=0 ; i<entryLen ; i++) {
		irq2Ptr = (uint *)gpQdmaPriv->irq2QueueAddr + ((headIdx+i) % CONFIG_IRQ2_DEPTH) ;

#if defined(TCSUPPORT_CPU_EN7580)
		RETRY = 15 ;
#else
		RETRY = 5 ;
#endif
		while(RETRY--) {
			irq2Value = *irq2Ptr ;
			if(irq2Value == CONFIG_IRQ_DEF_VALUE) {
				if(RETRY == 0){
					QDMA_ERR("There is no data available in IRQ2 queue. irq2 value:%.8x, irq2 ptr:%.8x TIMEs:%d\n", 
						(uint)irq2Value, (uint)irq2Ptr, RETRY) ;
#if defined(TCSUPPORT_QDMA_VERIFY)
                    printk("irq2Status = %.8X \n",irq2Status);
                    qdma_dump_irq2_info(&qdma_data) ;
#endif
                }
				if(RETRY <= 0) {
					gpQdmaPriv->counters.Irq2QueueAsynchronous++ ;
					ret = -ENODATA ;
					goto out1 ;
				}
			} else {
				*irq2Ptr = CONFIG_IRQ_DEF_VALUE ;
				break ;
			}
		}
		
		idx = (irq2Value & IRQ_CFG_IDX_MASK) ;
		ringIdx = (irq2Value & IRQ_CFG_RINGIDX_MASK) >> IRQ_CFG_RINGIDX_SHIFT;
#if defined(TCSUPPORT_CPU_EN7580) /*EN7580 only has IRQ INT, here can know ring index*/
#ifndef CONFIG_SUPPORT_SELF_TEST
		gpQdmaPriv->counters.intTxDone[ringIdx]++ ;
#endif
#endif
		if(idx<0 || idx>=gpQdmaPriv->txDscpNum[ringIdx]) {
			QDMA_ERR("The TX DSCP index %d is invalid.\n", idx) ;
#if defined(TCSUPPORT_QDMA_VERIFY)
            printk("irq2Status = %.8X \n",irq2Status);
            qdma_dump_irq2_info(&qdma_data) ;
#endif
			gpQdmaPriv->counters.txIrq2QueueIdxErrs++ ;
			ret = -EFAULT ;
			continue ;
		}
		
		diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + idx ;
		if(diPtr->dscpIdx!=idx || diPtr->next!=NULL) {
			QDMA_ERR("The content of the TX DSCP_INFO(%.8x) is incorrect. ENTRY_LEN:%d, HEAD_IDX:%d, IRQ2_VALUE:%.8x.\n", 
				(uint)diPtr, entryLen, headIdx+i, irq2Value) ;
			gpQdmaPriv->counters.txDscpIncorrect[ringIdx]++ ;
			ret = -EFAULT ;
			continue ;
		}
		
		txDscpPtr = diPtr->dscpPtr;

#ifdef CONFIG_TX_WB_DONE
		/*done or drop for EN7580 , only done for others*/
		if( (!txDscpPtr->ctrl.done)
#if SUPPORT_TX_DROP_BIT
			&& (!txDscpPtr->ctrl.drop_pkt)
#endif
			) 
		{
			QDMA_ERR("The done/drop bit of TX DSCP is incorrect. ADDR:%.8x, IDX:%d.\n", (uint)diPtr->dscpPtr, diPtr->dscpIdx) ;
			gpQdmaPriv->counters.txDscpDoneErrs[ringIdx]++ ;
			ret = -EFAULT ;
			continue ;
		}
#if SUPPORT_TX_DROP_BIT
		if(txDscpPtr->ctrl.drop_pkt)
		{
			gpQdmaPriv->counters.txDscpDropErrs[ringIdx]++ ;
		}
#endif

#endif /* CONFIG_TX_WB_DONE */

#ifdef CONFIG_SUPPORT_SELF_TEST
		txMsgW0.word = txDscpPtr->msg[0] ;
		channel = txMsgW0.channel ;
		queue = txMsgW0.queue ;
		counters[channel][queue].tx_frames++;
#endif
		gpQdmaPriv->counters.txRecycleCounts[ringIdx]++ ;

#if !defined(TCSUPPORT_TSO_ENABLE)
		if(diPtr->skb == NULL)
        {      
			QDMA_ERR("diPtr->skb is NULL.\n") ;	
#if defined(TCSUPPORT_QDMA_VERIFY)
            printk("irq2Status = %.8X \n",irq2Status);
            qdma_dump_irq2_info(&qdma_data) ;
#endif
            goto out1 ;
        }
#endif

		/* diPtr->skb == NULL is a valid case, when scatter skb is sent, and shouldn't be NULL when nls is 0. 
			but nls will be set 0,when HW writeback done bit.*/		
		if(diPtr->skb != NULL){
#if defined(TCSUPPORT_TSO_VERIFY)
			struct skb_frag_struct *frag;
			unsigned int nr_frags;
			int frag_idx;
			nr_frags = skb_shinfo(diPtr->skb)->nr_frags;
			if(nr_frags > 0){
				for(frag_idx=0; frag_idx<nr_frags; frag_idx++){						
					frag = &skb_shinfo(diPtr->skb)->frags[frag_idx];
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
					if(frag->bv_page){
						__free_pages(frag->bv_page ,get_order(frag->bv_len));
#else 
					if(frag->page.p){
						__free_pages(frag->page.p ,get_order(frag->size));
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)	
						diPtr->skb->len 		-= frag->bv_len;
						diPtr->skb->data_len	-= frag->bv_len;
						diPtr->skb->truesize	-= frag->bv_len;
						frag->bv_offset = 0;
						frag->bv_len = 0;
#else

						diPtr->skb->len 		-= frag->size;
						diPtr->skb->data_len	-= frag->size;
						diPtr->skb->truesize	-= frag->size;
						frag->page_offset = 0;
						frag->size = 0;
#endif		

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)	
						frag->bv_page =  NULL;
#else
						frag->page.p = NULL;
#endif

					}
				}
				skb_shinfo(diPtr->skb)->nr_frags = 0;
			}
#endif
			if(qdma_kfree_skb(diPtr->skb, txDscpPtr->msg[0], ringIdx) != 0) {
				QDMA_ERR("qdma_kfree_skb fail:headIdx:%d, dscpIdx:%d, ringIdx:%d.\n", headIdx+i, idx, ringIdx) ;
				qdma_dump_irq2_info(&qdma_data) ;
				ret = -EFAULT ;	
				goto out1 ;		
			}
		}
		txDscpPtr->msg[0] = 0;
		txDscpPtr->msg[1] = 0;
		
		spin_lock(&gpQdmaPriv->txLock[ringIdx]) ;
		qdma_bm_push_tx_dscp(diPtr, ringIdx) ;
		spin_unlock(&gpQdmaPriv->txLock[ringIdx]) ;
	}

out1:
	if(i!=0) {
		for(j=0 ; j<(i>>7) ; j++) {
			qdmaSetIrq2ClearLen(base, 0x80) ;
		}
		qdmaSetIrq2ClearLen(base, (i&0x7F)) ;
	}

out2:
	if(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE)
			spin_unlock_irqrestore(&gpQdmaPriv->txDoneLock, flags) ;
	return ret ;
}
#endif

/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_transmit_packet(struct sk_buff *skb, int ringIdx, txMsgWord_t *pTxMsg)
{
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint channel = 0, queue = 0 ;
	ulong flags = 0 ;
	int ret = 0 ;

#if SUPPORT_NO_DROP_BIT && !defined(TCSUPPORT_QDMA_VERIFY)
    /*oam should be set no_drop bit, and only oam can use this bit*/
    if( (pTxMsg->pTxMsgW0->oam)^(pTxMsg->pTxMsgW1->no_drop) )
    {
        QDMA_ERR("OAM packet should set no_drop_bit, others should not.\n") ; 
		return -EFAULT ;
    }
#endif
    
	if(unlikely(!skb || skb->len<=0 || skb->len>CONFIG_MAX_PKT_LENS)) 
	{
		QDMA_ERR("The input arguments are wrong, skb:%.8x, skbLen:%d.\n", (uint)skb, skb->len) ; 
		return -EFAULT ;
	}

	if(skb->ppe_magic == FOE_MAGIC_SPEED_TEST_UPSTREAM || skb->ppe_magic == FOE_MAGIC_GRE_UP_2){
        qdma_set_txmsg_tco(pTxMsg,1);
	}else if(skb->ppe_magic ==FOE_MAGIC_TR471_TEST_UPSTREAM){
        qdma_set_txmsg_ico(pTxMsg,1);
        qdma_set_txmsg_uco(pTxMsg,1);
        qdma_set_txmsg_tco(pTxMsg,1);
	}
#if SUPPORT_TR471_HW_FUNCTION
	else if(skb->ppe_magic ==FOE_MAGIC_TR471_HW_TEST_UPSTREAM){
		qdma_set_txmsg_pad(pTxMsg,1);
		qdma_set_txmsg_ico(pTxMsg,0);
		qdma_set_txmsg_tco(pTxMsg,0);
		qdma_set_txmsg_tso(pTxMsg,0);
	}
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
#if defined(QDMA_LAN)
    if(aggressive_offload_short_cut_mode){
        if(ringIdx == OFFLOAD_TX_RING){
            gpQdmaPriv->counters.txForceRing0Cnt++;
            ringIdx = 0;
        }
    }
#else
#ifdef LEFT_TO_RIGHT_QDMA_WAN_TX_2
    if(aggressive_offload_short_cut_mode && two_way_left_to_right_flag){
        if(ringIdx == OFFLOAD_TX_RING_2){
            gpQdmaPriv->counters.txForceRing0Cnt++;
            ringIdx = 0;
        }
    }
#endif
#endif
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_TSO_ENABLE) && !defined(TCSUPPORT_QDMA_VERIFY) && !defined(TCSUPPORT_TSO_V2_ENABLE)
	/* Reserve MAX_SKB_FRAGS+1 TxDs for a TSO skb. */
	if(atomic_read(&free_txd_count[ringIdx]) <= MAX_SKB_FRAGS+1){
        gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
		QDMA_MSG(DBG_MSG,"There are not enough TxDs! free_txd_count[%d]:%d\n", ringIdx, atomic_read(&free_txd_count[ringIdx]));
		return -EFAULT ;
	}
#endif

	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);	
	/* Get unused TX DSCP from TX unused DSCP link list */	
	pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
	if(unlikely(pNewDscpInfo == NULL)) {
		gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
		QDMA_MSG(DBG_MSG,"pNewDscpInfo is NULL\n") ; 
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
		return -ENOSR ;
	}
    
	channel = pTxMsg->pTxMsgW0->channel ;
	queue = pTxMsg->pTxMsgW0->queue ;

	#if defined(QDMA_LAN)
	/* qdma lan channel 7 for wifi offload to ppe, not do atomic for better performance.*/
	if (channel != WIFI_OFFLOAD_CHANNEL)
	{
		atomic_inc(&chnlLimit[channel]);
		atomic_inc(&queueLimit[channel][queue]);
	}
	#else
	atomic_inc(&chnlLimit[channel]);
	atomic_inc(&queueLimit[channel][queue]);
	if(ringIdx == RING_IDX_1)
		atomic_dec(&txRing1FreeCnt);
	#endif

#ifdef TCSUPPORT_CPU_ARMV8
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif
	pTxDscp->msg[0] = pTxMsg->pTxMsgW0->word;
	pTxDscp->msg[1] = pTxMsg->pTxMsgW1->word;
    if(pTxMsg->pTxMsgW2)
        pTxDscp->msg[2] = pTxMsg->pTxMsgW2->word;
    else
        pTxDscp->msg[2] = 0xffff;
	pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
	ecnt_dcache_wback_inv(((unsigned long)(skb->data)), skb->len);
#else
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
	dma_cache_wback_inv((unsigned long)(skb->data), skb->len);
#endif
	pTxDscp->ctrl.pkt_len = skb->len ;

#ifdef CONFIG_SUPPORT_SELF_TEST
	pTxDscp->ctrl.done = gpQdmaPriv->devCfg.txDscpDoneBit ;
#else
	pTxDscp->ctrl.done = 0 ;
#endif /* CONFIG_SUPPORT_SELF_TEST */

#if SUPPORT_TX_DROP_BIT
	pTxDscp->ctrl.drop_pkt = 0 ;
#endif
	pTxDscp->ctrl.nls = 0 ;		/* use of this bit need to be changed*/

#if defined(TCSUPPORT_QDMA_VERIFY) && defined(QDMA_LAN)
#if	SUPPORT_NO_DROP_BIT
	if((skb->data[1]) > 0)
		pTxDscp->ctrl.no_drop = 1;
	else
		pTxDscp->ctrl.no_drop = 0;
#endif
#ifdef CONFIG_DEBUG
    if(gpQdmaPriv->devCfg.dbgLevel > 0)
    {
        printk("\n--------QDMA Verify Test TX Start--------\n");
#if 1
        __dump_skb(skb, skb->len);
#else
        __dump_skb_for_crx_scatter(skb, skb->len);
#endif
        printk("TXDSCP: DONE:%d, NLS:%d, PKT:%.8x, PKTLEN:%d\n", (uint)pTxDscp->ctrl.done, 
                        (uint)pTxDscp->ctrl.nls, (uint)pTxDscp->pkt_addr, (uint)pTxDscp->ctrl.pkt_len);
        printk("msg0=%.8X, msg1=%.8X\n", (uint)pTxDscp->msg[0],(uint)pTxDscp->msg[1]);
        printk("--------QDMA Verify Test TX End--------\n");
    }
#endif
#endif

	QDMA_MSG(DBG_MSG, "TX msg0: %.8X, TX msg1: %.8X\n",(uint)pTxDscp->msg[0],(uint)pTxDscp->msg[1]);
	QDMA_MSG(DBG_MSG, "Hook TX DSCP to TXDMA. TX_CPU_IDX:%d, TX_NULL_IDX:%d\n", gpQdmaPriv->txUsingPtr[ringIdx]->dscpIdx, pNewDscpInfo->dscpIdx) ;
	QDMA_MSG(DBG_MSG, "TXDSCP: DONE:%d, PKT:%.8x, PKTLEN:%d, NEXT_IDX:%d, loopcnt:%d\n", 
																(uint)pTxDscp->ctrl.done, 
																(uint)pTxDscp->pkt_addr,
																(uint)pTxDscp->ctrl.pkt_len,
																(uint)pTxDscp->next_idx,
																(uint)pTxDscp->msg[1]>>24) ;
	
#if defined(TCSUPPORT_CPU_ARMV8)
	ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
	dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
	gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
	gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;

#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4();
#endif
	
#ifdef CONFIG_DEBUG 
    if(gpQdmaPriv->devCfg.dbgLevel >= DBG_ST) {
       printk("\n\ntxMsg0:0x%08x, txMsg1:0x%08x", (uint)pTxDscp->msg[0],(uint)pTxDscp->msg[1]);
       __dump_skb(skb, MIN(128, pTxDscp->ctrl.pkt_len));
    }
#endif
#ifdef CONFIG_SUPPORT_SELF_TEST
	gpQdmaPriv->devCfg.countDown-- ;
	if(!gpQdmaPriv->devCfg.countDown) {
		gpQdmaPriv->devCfg.countDownRound++ ;
		qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;
		gpQdmaPriv->devCfg.countDown = (gpQdmaPriv->devCfg.waitTxMaxNums) ? (gpQdmaPriv->devCfg.waitTxMaxNums) : ((random32()%(gpQdmaPriv->txDscpNum[ringIdx]-1))+1) ;
	} 
#else
	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;
#endif /* CONFIG_SUPPORT_SELF_TEST */
	
	gpQdmaPriv->counters.txCounts[ringIdx]++ ;
	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

    if(txPktDump == 1) {
        printk("\n\ntxMsg0:0x%08x, txMsg1:0x%08x\n", (uint)pTxDscp->msg[0], (uint)pTxDscp->msg[1]);
        __dump_skb(skb, pTxDscp->ctrl.pkt_len);
        txPktDump = 0;
    }

#if SUPPORT_IRQ2
	QDMA_MSG(DBG_MSG, "RingIdx:%d, GLG:%.8x, IRQStatus:%.8x, IRQ2Status:%.8x, CSR info: RX_CPU_IDX:%d, RX_DMA_IDX:%d, TX_CPU_IDX:%d, TX_DMA_IDX:%d\n", 
																ringIdx, 
																qdmaGetGlbCfg(base), 
																qdmaGetIrqStatus(base), 
																qdmaGetIrq2Status(base),
																qdmaGetRxCpuIdx(base, ringIdx), 
																qdmaGetRxDmaIdx(base, ringIdx), 
																qdmaGetTxCpuIdx(base, ringIdx), 
																qdmaGetTxDmaIdx(base, ringIdx)) ;
#else
	QDMA_MSG(DBG_MSG, "RingIdx:%d, GLG:%.8x, IRQStatus:%.8x, CSR info: RX_CPU_IDX:%d, RX_DMA_IDX:%d, TX_CPU_IDX:%d, TX_DMA_IDX:%d\n", 
																ringIdx, 
																qdmaGetGlbCfg(base), 
																qdmaGetIrqStatus(base), 
																qdmaGetRxCpuIdx(base, ringIdx), 
																qdmaGetRxDmaIdx(base, ringIdx), 
																qdmaGetTxCpuIdx(base, ringIdx), 
																qdmaGetTxDmaIdx(base, ringIdx)) ;
#endif

    return ret ;
}


__IMEM int qdma_bm_transmit_packet_wifi_fast(struct ECNT_QDMA_Data *qdma_data)
{
	struct sk_buff *skb = qdma_data->qdma_private.qdma_transmit_wifi_fast.skb ;

	return qdma_bm_transmit_packet_wifi_fast_hook(skb);
}
#ifdef TCSUPPORT_TEST_VWTEST
__IMEM int qdma_bm_transmit_packet_wifi_fast_ecnt_hook(struct sk_buff *skb)
{
	int ringIdx = OFFLOAD_FAST_TXRING_IDX;

	struct QDMA_DscpInfo_S *pNewDscpInfo ;
	QDMA_DMA_DSCP_T *pTxDscp ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags;
	int ret = 0 ;
	uint dscpIdxInt;
    txMsgWord_t txMsg;

	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);	
	/* Get unused TX DSCP from TX unused DSCP link list */	
	pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
	if(unlikely(pNewDscpInfo == NULL)) {
		gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
		dev_kfree_skb_any(skb);
		return -ENOSR ;
	}
    
#ifdef TCSUPPORT_CPU_ARMV8
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif
    txMsg.pTxMsgW0 = (txMsgWord0_t *)&(pTxDscp->msg[0]);
    txMsg.pTxMsgW1 = (txMsgWord1_t *)&(pTxDscp->msg[1]);
    txMsg.pTxMsgW2 = (txMsgWord2_t *)&(pTxDscp->msg[2]);

    qdma_init_txmsg(&txMsg,0);
    qdma_set_txmsg_whole_txmsg0(&txMsg,0x38);
    qdma_set_txmsg_fport(&txMsg,4);

	pTxDscp->ctrl.nls = 0 ;		/* use of this bit need to be changed*/
	dscpIdxInt = pNewDscpInfo->dscpIdx ;
	pTxDscp->next_idx = dscpIdxInt;
#if defined(TCSUPPORT_CPU_ARMV8)
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
	ecnt_dcache_wback_inv(((unsigned long)(skb->data)), 32);
#else
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
	r4k_dma_cache_wback_inv_ecnt((unsigned long)(skb->data), 32);
#endif

	pTxDscp->ctrl.pkt_len = skb->len ; 

	pTxDscp->ctrl.done = 0 ;
#if SUPPORT_TX_DROP_BIT
	pTxDscp->ctrl.drop_pkt = 0 ;
#endif

#if defined(TCSUPPORT_CPU_ARMV8)
	ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
	dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T)); 
#else
	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
	gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
	gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;
#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4();
#endif

	qdmaSetTxCpuIdx(base, ringIdx, dscpIdxInt) ;
	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

	return ret ;
}
#endif
__IMEM int qdma_bm_transmit_packet_wifi_fast_hook(struct sk_buff *skb)
{
	int ringIdx = OFFLOAD_FAST_TXRING_IDX;

	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags = 0 ;
	int ret = 0 ;
    txMsgWord_t txMsg;

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
#if defined(QDMA_LAN)
    if(aggressive_offload_short_cut_mode){
        if(ringIdx == OFFLOAD_TX_RING){
            gpQdmaPriv->counters.txForceRing0Cnt++;
            ringIdx = 0;
        }
    }
#else
#ifdef LEFT_TO_RIGHT_QDMA_WAN_TX_2
    if(aggressive_offload_short_cut_mode && two_way_left_to_right_flag){
        if(ringIdx == OFFLOAD_TX_RING_2){
            gpQdmaPriv->counters.txForceRing0Cnt++;
            ringIdx = 0;
        }
    }
#endif
#endif
#endif/*TCSUPPORT_COMPILE*/

	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);	
	/* Get unused TX DSCP from TX unused DSCP link list */	
	pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
	if(unlikely(pNewDscpInfo == NULL)) {
		gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
		dev_kfree_skb_any(skb);
		return -ENOSR ;
	}

#ifdef TCSUPPORT_CPU_ARMV8
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif
    txMsg.pTxMsgW0 = (txMsgWord0_t *)&(pTxDscp->msg[0]);
    txMsg.pTxMsgW1 = (txMsgWord1_t *)&(pTxDscp->msg[1]);
    txMsg.pTxMsgW2 = (txMsgWord2_t *)&(pTxDscp->msg[2]);

    qdma_init_txmsg(&txMsg,0);
    qdma_set_txmsg_whole_txmsg0(&txMsg,0x38);
    qdma_set_txmsg_fport(&txMsg,4);

	pTxDscp->ctrl.nls = 0 ;		/* use of this bit need to be changed*/
	pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
	ecnt_dcache_wback_inv(((unsigned long)(skb->data)), 32);
#else
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
	dma_cache_wback_inv((unsigned long)(skb->data), 32);
#endif

	pTxDscp->ctrl.pkt_len = skb->len ; 

	pTxDscp->ctrl.done = 0 ;
#if SUPPORT_TX_DROP_BIT
	pTxDscp->ctrl.drop_pkt = 0 ;
#endif

#if defined(TCSUPPORT_CPU_ARMV8)
	ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
	dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
	gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
	gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;

#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4(); 
#endif

	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;

	gpQdmaPriv->counters.txCounts[ringIdx]++ ;
	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

	return ret ;
}

__IMEM int qdma_bm_transmit_packet_offload_fast_hook(struct sk_buff *skb, int channel)
{
	int ringIdx = OFFLOAD_FAST_TXRING_IDX;

	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags = 0 ;
	int ret = 0 ;
    txMsgWord_t txMsg;

#if defined(TCSUPPORT_TSO_ENABLE) && !defined(TCSUPPORT_TSO_V2_ENABLE)
    if ((skb->shortcut_on_speed || (skb->ppe_magic == FOE_MAGIC_SPEED_TEST_UPSTREAM && (skb->tso_mark & TSO_ENABLE_MARK)))
        && ((skb_shinfo(skb)->gso_size != 0) || (skb_shinfo(skb)->nr_frags != 0)))
    {
		if(skb->ppe_magic == FOE_MAGIC_SPEED_TEST_UPSTREAM)
			return qdma_bm_transmit_packet_tso_speedtest(skb, channel);
		else
        	return qdma_bm_transmit_packet_tso_offload_fast_hook(skb, channel);
    }
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
#if defined(QDMA_LAN)
    if(aggressive_offload_short_cut_mode){
        if(ringIdx == OFFLOAD_TX_RING){
            gpQdmaPriv->counters.txForceRing0Cnt++;
            ringIdx = 0;
        }
    }
#else
#ifdef LEFT_TO_RIGHT_QDMA_WAN_TX_2
    if(aggressive_offload_short_cut_mode && two_way_left_to_right_flag){
        if(ringIdx == OFFLOAD_TX_RING_2){
            gpQdmaPriv->counters.txForceRing0Cnt++;
            ringIdx = 0;
        }
    }
#endif
#endif
#endif/*TCSUPPORT_COMPILE*/
	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);	
	/* Get unused TX DSCP from TX unused DSCP link list */	
	pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
	if(unlikely(pNewDscpInfo == NULL)) {
		gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
		if((skb->ppe_magic == FOE_MAGIC_SPEED_TEST_UPSTREAM) && (skb->tso_mark & TSO_STATIC_SKB)) {
			skb->tso_mark &= (~TSO_SKB_USING);
		}else{
			dev_kfree_skb_any(skb);
		}
		return -ENOSR ;
	}

#ifdef TCSUPPORT_CPU_ARMV8
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif
    txMsg.pTxMsgW0 = (txMsgWord0_t *)&(pTxDscp->msg[0]);
    txMsg.pTxMsgW1 = (txMsgWord1_t *)&(pTxDscp->msg[1]);
    txMsg.pTxMsgW2 = (txMsgWord2_t *)&(pTxDscp->msg[2]);

    qdma_init_txmsg(&txMsg,0);
    qdma_set_txmsg_chn(&txMsg,channel);
    qdma_set_txmsg_fport(&txMsg,4);

    if ( (skb->ppe_magic == FOE_MAGIC_VXLAN_UP_FRAG)
		|| (skb->ppe_magic == FOE_MAGIC_GRE_UP_2))
    {
        qdma_set_txmsg_ico(&txMsg,1);
    }
    else if ((skb->ppe_magic == FOE_MAGIC_VXLAN_UP_2) || (skb->ppe_magic == FOE_MAGIC_L2TP_VPN_UPSTREAM))
    {
        qdma_set_txmsg_ico(&txMsg,1);
        qdma_set_txmsg_uco(&txMsg,1);
    }

	if(skb->ppe_magic == FOE_MAGIC_SPEED_TEST_UPSTREAM){
        qdma_set_txmsg_tco(&txMsg,1);
	}else if(skb->ppe_magic ==FOE_MAGIC_TR471_TEST_UPSTREAM){
        qdma_set_txmsg_ico(&txMsg,1);
        qdma_set_txmsg_uco(&txMsg,1);
        qdma_set_txmsg_tco(&txMsg,1);
	}
#if SUPPORT_TR471_HW_FUNCTION
	else if(skb->ppe_magic ==FOE_MAGIC_TR471_HW_TEST_UPSTREAM){
		qdma_set_txmsg_pad(&txMsg,1);
		qdma_set_txmsg_ico(&txMsg,0);
		qdma_set_txmsg_tco(&txMsg,0);
		qdma_set_txmsg_tso(&txMsg,0);
		/*debug fport*/
		//qdma_set_txmsg_fport(&txMsg,1);
		//qdma_set_txmsg_sptag(&txMsg,0x4);
	}
#endif

	if (skb->shortcut_on_speed)
    {
        qdma_set_txmsg_ico(&txMsg,1);
        qdma_set_txmsg_tco(&txMsg,1);
	}

	pTxDscp->ctrl.nls = 0 ;		/* use of this bit need to be changed*/
	pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
	ecnt_dcache_wback_inv(((unsigned long)(skb->data)), skb->len);
#else
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
	dma_cache_wback_inv((unsigned long)(skb->data), skb->len);
#endif

	pTxDscp->ctrl.pkt_len = skb->len ; 

	pTxDscp->ctrl.done = 0 ;
#if SUPPORT_TX_DROP_BIT
	pTxDscp->ctrl.drop_pkt = 0 ;
#endif

#if defined(TCSUPPORT_CPU_ARMV8)
	ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
	dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
	gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
	gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;

#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4(); 
#endif

	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;

	gpQdmaPriv->counters.txCounts[ringIdx]++ ;
	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

	return ret ;
}

__IMEM int qdma_bm_transmit_packet_xsi_fast_hook(struct sk_buff *skb)
{
		int ringIdx = OFFLOAD_FAST_TXRING_IDX;

		struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
		QDMA_DMA_DSCP_T *pTxDscp=NULL ;
		uint base = gpQdmaPriv->csrBaseAddr ;
		ulong flags = 0 ;
		int ret = 0 ;
        txMsgWord_t txMsg;
	
		spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags); 
		/* Get unused TX DSCP from TX unused DSCP link list */	
		pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
			dev_kfree_skb_any(skb);
			return -ENOSR ;
		}
#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif
        txMsg.pTxMsgW0 = (txMsgWord0_t *)&(pTxDscp->msg[0]);
        txMsg.pTxMsgW1 = (txMsgWord1_t *)&(pTxDscp->msg[1]);
        txMsg.pTxMsgW2 = (txMsgWord2_t *)&(pTxDscp->msg[2]);

        qdma_init_txmsg(&txMsg,0);
        qdma_set_txmsg_whole_txmsg0(&txMsg,0x60);
        qdma_set_txmsg_fport(&txMsg,3);
        qdma_set_txmsg_nbq(&txMsg,XSI_FAST_HANDLER_NBQ);

		pTxDscp->ctrl.nls = 0 ; 	/* use of this bit need to be changed*/
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);

#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(skb->data)), skb->len);
#else
		dma_cache_wback_inv((unsigned long)(skb->data), skb->len);
#endif
		
		pTxDscp->ctrl.pkt_len = skb->len ; 
	
		pTxDscp->ctrl.done = 0 ;
#if SUPPORT_TX_DROP_BIT
		pTxDscp->ctrl.drop_pkt = 0 ;
#endif
	
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
		gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;
	
#if !defined(TCSUPPORT_CPU_ARMV8)
		SYNC_TYPE4(); 
#endif

		qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;
	
		gpQdmaPriv->counters.txCounts[ringIdx]++ ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
	
		return ret ;
}

/******************************************************************************
******************************************************************************/
int qdma_bm_recycle_transmit_buffer(int ringIdx)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	void *msgPtr=NULL;
	ulong flags=0 ;
	int i=0 ;

	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags) ;

	for(i=0 ; i<gpQdmaPriv->txDscpNum[ringIdx] ; i++) {
		diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + i ;
		if(diPtr->next==NULL && diPtr!=gpQdmaPriv->txTailPtr[ringIdx] && diPtr!=gpQdmaPriv->txUsingPtr[ringIdx]) {
			pTxDscp = diPtr->dscpPtr ;
		
			if(diPtr->skb && pTxDscp->pkt_addr) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
				dma_unmap_single(NULL, pTxDscp->pkt_addr, pTxDscp->ctrl.pkt_len, DMA_TO_DEVICE) ;
#else
				dma_unmap_single(qdma_dummy_dev_ptr, pTxDscp->pkt_addr, pTxDscp->ctrl.pkt_len, DMA_TO_DEVICE) ;
#endif
				qdma_kfree_skb(diPtr->skb, pTxDscp->msg[0], ringIdx);
				msgPtr = (void *)pTxDscp->msg ;
				if(msgPtr)
				    memset((void *)pTxDscp->msg, 0, QDMA_TX_DSCP_MSG_LENS);
			}
		
			qdma_bm_push_tx_dscp(diPtr, ringIdx) ;
		}
	}

	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags) ;

	diPtr = gpQdmaPriv->txUsingPtr[ringIdx] ;
	qdmaSetTxCpuIdx(base, ringIdx, diPtr->dscpIdx) ;
	qdmaSetTxDmaIdx(base, ringIdx, diPtr->dscpIdx) ;
	
	return 0 ;
}

#if 0
/******************************************************************************
 Proc function for QDMA debug
******************************************************************************/
/******************************************************************************
******************************************************************************/

/*32 means echo all queue info */
static int dump_channel_or_queue_Info(uint chnlIdx, uint queIdx, char *cmd)
{
	uint channelIdx=0, queueIdx=0;	
	uint totalcnt=0, channelDscpCnt=0;
	uint qAddr=0, queueInfoMemXs0=0;
	uint queueNum[QUEUE_ALL_NUM]={0}, channelNum[CONFIG_QDMA_CHANNEL]={0};
		
	if(chnlIdx == CONFIG_QDMA_CHANNEL){
		for(channelIdx=0; channelIdx<CONFIG_QDMA_CHANNEL; channelIdx++){
			for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++){
				
				qAddr = (channelIdx<<3) + queueIdx;
				
                qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 0, qAddr, &queueInfoMemXs0);				
				totalcnt = queueInfoMemXs0 & DBG_MEM_XS_QUEUE_INFO_CNT_MASK;			
				queueNum[qAddr] = totalcnt;
				channelDscpCnt += totalcnt;
			}
			
			channelNum[channelIdx]=channelDscpCnt;
			channelDscpCnt = 0;			
		}
		
		if(!strcmp(cmd, "queueinfo")){
			for(channelIdx=0; channelIdx<CONFIG_QDMA_CHANNEL; channelIdx++)
				for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++){
					qAddr = (channelIdx<<3) + queueIdx;
					printk("channel %d queue %d : totalcnt %d\r\n", channelIdx, queueIdx, queueNum[qAddr]);
				}
		}else /*"channelinfo"*/{
			for(channelIdx=0; channelIdx<CONFIG_QDMA_CHANNEL; channelIdx++)
				printk("channel %d :  channel dscp num: %d\r\n", channelIdx, channelNum[channelIdx]);		
		}

	}else{
	/*channelIdx < CONFIG_QDMA_CHANNEL && channelIdx >= 0 &&  queueIdx <= CONFIG_QDMA_QUEUE && queueIdx >= 0*/
		if(!strcmp(cmd, "queueinfo")){
			channelIdx=chnlIdx;
			queueIdx=queIdx;
			qAddr = (channelIdx<<3) + queueIdx;
        	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 0, qAddr, &queueInfoMemXs0);
			totalcnt = queueInfoMemXs0 & DBG_MEM_XS_QUEUE_INFO_CNT_MASK;	 
			printk("channel %d queue %d : totalcnt %d\r\n", channelIdx, queueIdx, totalcnt);
			
		}else{/*channelinfo*/
			for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++){
				qAddr = (channelIdx<<3) + queueIdx;
	            qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 0, qAddr, &queueInfoMemXs0);
			    totalcnt = queueInfoMemXs0 & DBG_MEM_XS_QUEUE_INFO_CNT_MASK;
				channelDscpCnt += totalcnt;
			}
			printk("channel %d :  channel dscp num: %d\r\n", channelIdx, channelDscpCnt);
		
		}
	}

	return 0;		
}

static int dump_queue_dscplist(uint chnlIdx, uint queIdx)
{
	uint queueDscpCnt=0, headDscpIdx=0, tailDscpIdx=0, nextDscpIdx0=0;
	uint channelIdx=chnlIdx, queueIdx=queIdx;
	uint qAddr=0;
	uint queueInfoMemXs0=0, queueInfoMemXs1=0;
	int *queueDscpIdxArray=0;
	int i=0;
	
	/*get queue dscp counter, head*/
	qAddr = (channelIdx << 3)+ queueIdx;
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 0, qAddr, &queueInfoMemXs0);
	queueDscpCnt = (queueInfoMemXs0 >>  DBG_MEM_XS_QUEUE_INFO_CNT_SHIFT  ) & DBG_MEM_XS_QUEUE_INFO_CNT_MASK;
	
	/*QDMA_LAN CONFIG_HWFWD_DSCP_NUM 1024, QDMA_WAN CONFIG_HWFWD_DSCP_NUM 4096*/
	if(queueDscpCnt>=CONFIG_HWFWD_DSCP_NUM || queueDscpCnt < 0){
		QDMA_ERR("invalid Dscp cnt : %d \r\n!", queueDscpCnt);
		return -EFAULT;/*need reslove*/
	}
	
	tailDscpIdx = (queueInfoMemXs0 >> DBG_MEM_XS_QUEUE_INFO_TAIL_SHIFT) & DBG_MEM_XS_QUEUE_INFO_TAIL_MASK;
	
	#if defined(QDMA_LAN)
	headDscpIdx = ((queueInfoMemXs0 >> DBG_MEM_XS_QUEUE_INFO_HEAD_SHIFT) & DBG_MEM_XS_QUEUE_INFO_HEAD_MASK);
	#else
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 1, qAddr, &queueInfoMemXs1);
	headDscpIdx = (((queueInfoMemXs0 >> DBG_MEM_XS_QUEUE_INFO_HEAD_SHIFT) & 0x7F)|((queueInfoMemXs1 & 0x1F) << 7)) & DBG_MEM_XS_QUEUE_INFO_HEAD_MASK;
	#endif
	queueDscpIdxArray=(int*)kzalloc(queueDscpCnt, sizeof(uint));
	if(!queueDscpIdxArray){
		QDMA_ERR("Allocate memory for queue DscpIdx Array failed.\n") ;
		return -ENOMEM ;
	}
	
	queueDscpIdxArray[0]=headDscpIdx;
	for(i=1; i<queueDscpCnt; i++){
		qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_DSCP_INFO, 0, queueDscpIdxArray[i-1], &nextDscpIdx0);
		queueDscpIdxArray[i]=nextDscpIdx0 & DBG_MEM_XS_DSCP_IDX_MASK;	
	}

	printk("channel%d queue%d", channelIdx, queueIdx);
	for(i=0; i<queueDscpCnt; i++){
		if(i%10 == 0){
			printk("\r\n");
			printk("%-4d - %-4d : %-4d ", i, i+9, queueDscpIdxArray[i]);
		}else
			printk("%-4d ",queueDscpIdxArray[i]);		
	}
	kfree(queueDscpIdxArray);
	
	return 0;
}

static int get_lmgr_queue_dscpIdx(uint dscpIdx){
	uint nextDscpIdx = 0;
	if(dscpIdx >= CONFIG_DSCP_NUM_MAX || dscpIdx < 0){
		QDMA_ERR("Invalid dscpIdx.\n");
		return -EINVAL;
	}
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_DSCP_INFO, 0, dscpIdx, &nextDscpIdx);
	return nextDscpIdx;
	
}

static int get_lmgr_queue_info(uint channelIdx, uint queueIdx, QDMA_Lmgr_queueInfo_T queueInfo){
	
	uint queueInfoMemXs0 = 0, queueInfoMemXs1 = 0;
	uint qAddr=0;

	if(channelIdx >= CONFIG_QDMA_CHANNEL || channelIdx < 0 ){
		QDMA_ERR("Invalid Channel Index %d\n", channelIdx);
		return -EINVAL;
	}else if(queueIdx >= CONFIG_QDMA_QUEUE || queueIdx < 0){
		QDMA_ERR("Invalid Queue Index %d\n", queueIdx);
		return -EINVAL;
	}
	
	qAddr = (channelIdx<<3) + queueIdx;
	#if defined(QDMA_LAN)
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 0, qAddr, &queueInfoMemXs0);
	queueInfo.cntr = queueInfoMemXs0 & DBG_MEM_XS_QUEUE_INFO_CNT_MASK;
	queueInfo.tailIdx = (queueInfoMemXs0 >> DBG_MEM_XS_QUEUE_INFO_TAIL_SHIFT) & DBG_MEM_XS_QUEUE_INFO_TAIL_MASK;
	queueInfo.headIdx = (queueInfoMemXs0 >> DBG_MEM_XS_QUEUE_INFO_HEAD_SHIFT) & DBG_MEM_XS_QUEUE_INFO_HEAD_MASK;
	#else
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 0, qAddr, &queueInfoMemXs0);
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO, 1, qAddr, &queueInfoMemXs1);
	queueInfo.cntr = queueInfoMemXs0 & DBG_MEM_XS_QUEUE_INFO_CNT_MASK;
	queueInfo.tailIdx = (queueInfoMemXs0 >> DBG_MEM_XS_QUEUE_INFO_TAIL_SHIFT) & DBG_MEM_XS_QUEUE_INFO_TAIL_MASK;
	queueInfo.headIdx = (((queueInfoMemXs0 >> DBG_MEM_XS_QUEUE_INFO_HEAD_SHIFT) & 0x7F)|((queueInfoMemXs1 & 0x1F) << 7)) & DBG_MEM_XS_QUEUE_INFO_HEAD_MASK;
	#endif

	return 0;		
}

static int get_lmgr_chnl_queue_bufsize(uint channelIdx, uint queueBuf[]){

	uint queueBufMemXs0=0, queueBufMemXs1=0, queueBufMemXs2=0, queueBufMemXs3=0, queueBufMemXs4=0, queueBufMemXs5=0;
	if(queueBuf == NULL){
		QDMA_ERR("NULL ptr.\n");
		return EFAULT;
	}
	if(channelIdx >= CONFIG_QDMA_CHANNEL || channelIdx < 0 ){
		QDMA_ERR("Invalid Channel Index %d\n", channelIdx);
		return -EINVAL;
	}
	/*get queue bufsize from reg*/
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_BUFFER_MONITOR, 0, channelIdx, &queueBufMemXs0);
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_BUFFER_MONITOR, 1, channelIdx, &queueBufMemXs1);
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_BUFFER_MONITOR, 2, channelIdx, &queueBufMemXs2);
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_BUFFER_MONITOR, 3, channelIdx, &queueBufMemXs3);
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_BUFFER_MONITOR, 4, channelIdx, &queueBufMemXs4);
	qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_BUFFER_MONITOR, 5, channelIdx, &queueBufMemXs5);

	queueBuf[0] = queueBufMemXs0 & 0xFFFFFF;
	queueBuf[1] = ((queueBufMemXs0 >> 24)| (queueBufMemXs1 & 0xFFFF)) & 0xFFFFFF;
	queueBuf[2] = ((queueBufMemXs1 >> 16)| (queueBufMemXs2 & 0xFF)) & 0xFFFFFF;
	queueBuf[3] = (queueBufMemXs2 >> 8) & 0xFFFFFF;

	queueBuf[4] = queueBufMemXs3 & 0xFFFFFF;
	queueBuf[5] = ((queueBufMemXs3 >> 24)| (queueBufMemXs4 & 0xFFFF)) & 0xFFFFFF;
	queueBuf[6] = ((queueBufMemXs4 >> 16)| (queueBufMemXs5 & 0xFF)) & 0xFFFFFF;
	queueBuf[7] = (queueBufMemXs5 >> 8) & 0xFFFFFF;

	return 0;
}
#endif

void qdma_err_drop_counters(unsigned short *qdma_tx_err, unsigned short *qdma_rx_err)
{
	int ringIdx=0;
	for(ringIdx=0 ; ringIdx<TX_RING_NUM ; ringIdx++) {
		qdma_tx_err[ringIdx] = gpQdmaPriv->counters.noTxDscps[ringIdx];
		qdma_tx_err[TX_RING_NUM+ringIdx] = gpQdmaPriv->counters.intNoTxDscp[ringIdx];
		gpQdmaPriv->counters.noTxDscps[ringIdx] = 0;
		gpQdmaPriv->counters.intNoTxDscp[ringIdx]= 0;
	}
	qdma_rx_err[0] = gpQdmaPriv->counters.intIrqFull;
	gpQdmaPriv->counters.intIrqFull = 0;

	return ;
}

/******************************************************************************
******************************************************************************/
static int qdma_bm_counters_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	qdma_dump_cpu_counters(NULL);
	return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_bm_counters_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] ;
	uint cmd=0 ;
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%d", &cmd) ;

	if(cmd == 1) {
		qdma_clear_cpu_counters(NULL);
	}
	
	return count ;
}

/******************************************************************************
******************************************************************************/
#ifdef QDMA_LAN
static int lan_tx_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("qdma lan shoutcut %s\n", sysctl_lan_tx_off ? "disable" : "enable");
	printk("qdma lan shoutcut %s\n", is5GTest ? "5G" : "2.4G");
	printk("wifi tx shortcut ver: %d \n",wifi_tx_shortcut_ver );	
	printk("lan_tx_counter0 = %d\n", sysctl_lan_tx_counter0);
	printk("lan_tx_counter1 = %d\n", sysctl_lan_tx_counter1);
	return 0 ;
}
static int lan_tx_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[8];
	int len = 0;
	unsigned int lan_tx_off = 0;
	int wifi_ver = 0;
	
	if (count > (sizeof(get_buf) - 1))
		len = sizeof(get_buf) - 1;
	else 
		len = count;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;

	get_buf[len] = '\0';

	sscanf(get_buf, "%d %d %d", &lan_tx_off, &is5GTest,&wifi_ver);
	
	sysctl_lan_tx_off = lan_tx_off;
	wifi_tx_shortcut_ver =  wifi_ver;
	sysctl_lan_tx_counter0 = 0;
	sysctl_lan_tx_counter1 = 0;

	return len;
}
#endif


/******************************************************************************
******************************************************************************/
static int qdma_bm_debug_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int qdma_bm_debug_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] = {0}, cmd[64] = {0}, subcmd[64] = {0}, dumpMode[64] = {0};
	uint value=0, cnt=0, offset=0, ringIdx=0;
	struct ECNT_QDMA_Data qdma_data;
	QDMA_DscpDumpInfo_T dscpDumpInfo;
	QDMA_DBG_CNTR_T dbgCntr;
	uint index;
	uint debug_temp;
#if SUPPORT_INX_COLOR_RESOLUTION
	uint base = gpQdmaPriv->csrBaseAddr ;
#endif 

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %s %d %x", cmd, subcmd, &cnt, &offset) ;

	if(!strcmp(cmd, "level")) {
#ifdef CONFIG_DEBUG
		value = subcmd[0] - 48 ;
		qdma_data.qdma_private.qdma_old_proc.dbgLevel = value;
		qdma_set_dbg_level(&qdma_data);
#endif /* CONFIG_DEBUG */
	} else if(!strcmp(cmd, "dump")) {
		if(!strcmp(subcmd, "csr")) {
			qdma_dump_register_value(NULL);
		} else if(!strcmp(subcmd, "dscp")) {
			sscanf(val_string, "%s %s %s %d", cmd, subcmd, dumpMode, &ringIdx);
			if(!strcmp(dumpMode, "tx"))
				dscpDumpInfo.dscpDumpMode = QDMA_DUMP_TX_DSCP;
			else if(!strcmp(dumpMode, "rx"))
				dscpDumpInfo.dscpDumpMode = QDMA_DUMP_RX_DSCP;
			else{/*dump all tx&rx ring*/
				qdma_dump_descriptor_info(NULL);
				return count ;
			}
			dscpDumpInfo.ringIdx = ringIdx;
			qdma_data.qdma_private.dscpDumpInfoPtr = &dscpDumpInfo;
			qdma_dump_descriptor_info(&qdma_data);
		} else if(!strcmp(subcmd, "irq")) {
			qdma_dump_irq_info(NULL);
		} else if(!strcmp(subcmd, "hwfwd")) {
			qdma_dump_hwfwd_info(NULL);
		} else if(!strcmp(subcmd, "dscpcnt")) {
			qdma_dump_descriptor_cnt(NULL);
		}
#if SUPPORT_IRQ2
		else if(!strcmp(subcmd, "irq2")) {
			qdma_dump_irq2_info(NULL);
		}
#endif
	} else if(!strcmp(cmd, "dbgcntr")) {
		if(!strcmp(subcmd, "dump")) {
			qdma_dump_dbg_cntr_value(NULL);
		} else if(!strcmp(subcmd, "clear")) {
			qdma_clear_dbg_cntr_value_all(NULL);
		} else if(!strcmp(subcmd, "queue")) {
			qdma_clear_and_set_dbg_cntr_queue_group(NULL);
		} 
#if SUPPORT_DBGCNTR_2RING_MODE
		else if(!strcmp(subcmd, "ring")) {
			qdma_clear_and_set_dbg_cntr_ring_group(NULL);
		}
#endif
#if defined(QDMA_LAN)
		else if(!strcmp(subcmd, "channel")) {
			qdma_data.qdma_private.cntrSrc = QDMA_DBG_CNTR_CHNL_TXCPU;
			qdma_clear_and_set_dbg_cntr_channel_group(&qdma_data);
		} 
#else
		else if(!strcmp(subcmd, "channel_cpu")) {
			qdma_data.qdma_private.cntrSrc = QDMA_DBG_CNTR_CHNL_TXCPU;
			qdma_clear_and_set_dbg_cntr_channel_group(&qdma_data);
		} else if(!strcmp(subcmd, "channel_fwd1")) {
			qdma_data.qdma_private.cntrSrc = QDMA_DBG_CNTR_CHNL_TXFWD1;
			qdma_clear_and_set_dbg_cntr_channel_group(&qdma_data);
		} else if(!strcmp(subcmd, "channel_fwd2")) {
			qdma_data.qdma_private.cntrSrc = QDMA_DBG_CNTR_CHNL_TXFWD2;
			qdma_clear_and_set_dbg_cntr_channel_group(&qdma_data);
		}
#endif
	} else if(!strcmp(cmd, "test")) {
		if(!strcmp(subcmd, "wrr")) {
			int i, j ;
			QDMA_TxQosScheduler_T txQos ;
			
			for(i=0 ; i<CONFIG_QDMA_CHANNEL ; i++) {
				txQos.channel = i ;
				txQos.qosType = i%QDMA_TXQOS_TYPE_NUMS ;
				for(j=0 ; j<CONFIG_QDMA_QUEUE ; j++) {
					txQos.queue[j].weight = i*10+j ;
				}
				qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
				qdma_set_tx_qos(&qdma_data) ;
			}
			
			for(i=0 ; i<CONFIG_QDMA_CHANNEL ; i++) {
				memset(&txQos, 0, sizeof(QDMA_TxQosScheduler_T)) ;
				
				txQos.channel = i ;
				qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
				qdma_get_tx_qos(&qdma_data) ;
				printk("Channel:%d, Type:%d, Q0:%d, Q1:%d, Q2:%d, Q3:%d, Q4:%d, Q5:%d, Q6:%d, Q7:%d\n",
									i, txQos.qosType, 
									txQos.queue[0].weight,
									txQos.queue[1].weight,
									txQos.queue[2].weight,
									txQos.queue[3].weight,
									txQos.queue[4].weight,
									txQos.queue[5].weight,
									txQos.queue[6].weight,
									txQos.queue[7].weight) ;									
			}
		} else if(!strcmp(subcmd, "dma_busy")) {
			qdma_data.qdma_private.qdma_old_proc.busyDuration = cnt;
			qdma_dump_dma_busy(&qdma_data); //the unit of cnt is second.
		} else if(!strcmp(subcmd, "trigger")) {
			if(cnt > 0){
			    trigger_timer_going = 1 ;
			    qdma_config_trigger_test(&qdma_data);
			}else{
			    trigger_timer_going = 0 ;
			}
		} else if(!strcmp(subcmd, "reg_polling")) {
			qdma_data.qdma_private.qdma_old_proc.regOffset = offset;
			qdma_data.qdma_private.qdma_old_proc.pollingDuration = cnt;
			qdma_dump_reg_polling(&qdma_data);	//the unit of cnt is second.
		} else if(!strcmp(subcmd, "forceRX1")) { /* force rx1:rx0=5:0 */
			qdma_data.qdma_private.qdma_old_proc.forceEn = cnt;
			qdma_set_force_receive_rx_ring1(&qdma_data);
		}
	} else if(!strcmp(cmd, "set")) { 
		if(!strcmp(subcmd, "dumpOnce")) { /* only dump tx/rx pkt&msg once */
			if(cnt > 0) {
				txPktDump=rxPktDump=1 ;
			}
		} else if(!strcmp(subcmd, "drop_broadcast_multicast")) { /* drop broadcast pkts */
			/* 0: no drop;   1: only drop broadcast;   2: only drop multicast;   3: drop broadcast & multicast */
			dropBroadcastMulticastFlag=cnt;
		} else if(!strcmp(subcmd, "txring1_threshold")) {
			txring1ReserveThreshold=cnt;
			printk("txring1ReserveThreshold is %d\n", txring1ReserveThreshold);
		}
	}
#if defined(TCSUPPORT_CPU_EN7528) && defined(TCSUPPORT_QDMA_VERIFY)
    else if(!strcmp(cmd, "dbgcnt_test")) {
		dbgCntr.cntrEn = DBG_CNTR_ENABLE;
		if(!strcmp(subcmd, "queue_cpu")) {
			dbgCntr.isChnlAll = 1;
			dbgCntr.isDscpRingAll = 1;
	  	  	dbgCntr.isQueueAll = 0;
			dbgCntr.queueIdx = cnt ;
			dbgCntr.dscpRingIdx = 0;
			dbgCntr.chnlIdx = 0 ;
			for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {					
				dbgCntr.cntrIdx= index;
				dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
				qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
			}
		}
		if(!strcmp(subcmd, "queue_fwd")) { 
			dbgCntr.isChnlAll = 1;
			dbgCntr.isDscpRingAll = 1;
	  	  	dbgCntr.isQueueAll = 0;
			dbgCntr.queueIdx = cnt ;
			dbgCntr.dscpRingIdx = 0;
			dbgCntr.chnlIdx = 0 ;
			for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {					
				dbgCntr.cntrIdx= index;
				dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;
				qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
			}
		}
		if(!strcmp(subcmd, "channel_cpu")) {
			dbgCntr.isChnlAll = 0;
			dbgCntr.isDscpRingAll = 1;
	  	  	dbgCntr.isQueueAll = 1;
			dbgCntr.chnlIdx= cnt ;
			dbgCntr.dscpRingIdx = 0;
			dbgCntr.queueIdx = 0 ;
			for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {					
				dbgCntr.cntrIdx= index;
				dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
				qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
			}

		}
		if(!strcmp(subcmd, "channel_fwd")) {
			dbgCntr.isChnlAll = 0;
			dbgCntr.isDscpRingAll = 1;
	  	  	dbgCntr.isQueueAll = 1;
			dbgCntr.chnlIdx= cnt ;
			dbgCntr.dscpRingIdx = 0;
			dbgCntr.queueIdx = 0 ;
			for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {					
				dbgCntr.cntrIdx= index;
				dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;
				qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
			}
	    }

		if(!strcmp(subcmd, "rx_ring")) { 
			dbgCntr.isChnlAll = 1;
			dbgCntr.isDscpRingAll = 0;
	  	  	dbgCntr.isQueueAll = 1;
			dbgCntr.dscpRingIdx= cnt ;
			dbgCntr.chnlIdx = 0 ;
			dbgCntr.queueIdx = 0 ;
			for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {					
				dbgCntr.cntrIdx= index;
				dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_RX;
				qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
			}

		}
		if(!strcmp(subcmd, "tx_ring")) { 
			dbgCntr.isChnlAll = 1;
			dbgCntr.isDscpRingAll = 0;
	  	  	dbgCntr.isQueueAll = 1;
			dbgCntr.dscpRingIdx= cnt ;
			dbgCntr.chnlIdx = 0 ;
			dbgCntr.queueIdx = 0 ;
			for(index=0; index<DBG_CNTR_INDEX_MAX; index++) {					
				dbgCntr.cntrIdx= index;
				dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
				qdma_clear_and_set_dbg_cntr_info(&dbgCntr);
			}
		}
		if(!strcmp(subcmd, "dump")) { 
				qdma_debug_counter_dump();
		}
    }
#endif
#if SUPPORT_INX_COLOR_RESOLUTION
	else if(!strcmp(cmd, "meter_mode")){
		if(!strcmp(subcmd, "mode0")) { 
			qdmaSetInxColorResCfg(base,default_mtr_color_cfg);
			qdmaSetInxColorResCfg1(base,default_mtr_color_cfg1);
		}else if(!strcmp(subcmd, "mode1")) { 
			qdmaSetInxColorResCfg(base,0x88666666);
			qdmaSetInxColorResCfg1(base,0x40141414);
		}
	}
#endif 
	return count ;
}

#if SUPPORT_MTR_ACNT_IN_QDMA
static int qdma_bm_meter_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int idx = 0, meter_idx = 0;
    GENERAL_TrtcmMode_T trtcmMode;
    struct ECNT_QDMA_Data qdma_data_tmp;
    GENERAL_TrtcmCfg_T rxTrtcmCfg;
    GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;
    GENERAL_TrtcmCbsPbsCfg_T rxTrtcmCbsPbsCfg;
    GENERAL_TrtcmCbsPbsSet_T rxTrtcmCbsPbsSet;
    memset(&qdma_data_tmp, 0, sizeof(struct ECNT_QDMA_Data)) ;
    memset(&rxTrtcmCfg, 0, sizeof(GENERAL_TrtcmCfg_T)) ;
    memset(&rxRateLimitCfg, 0, sizeof(GENERAL_TrtcmRatelimitCfg_T)) ;
    memset(&rxRateLimitSet, 0, sizeof(GENERAL_TrtcmRatelimitSet_T)) ;
    memset(&rxTrtcmCbsPbsCfg, 0, sizeof(GENERAL_TrtcmCbsPbsCfg_T)) ;
    memset(&rxTrtcmCbsPbsSet, 0, sizeof(GENERAL_TrtcmCbsPbsSet_T)) ;
    
    trtcmMode = generalGetTrtcmMode(INGRESS_TRTCM) ;

    for(idx = 0; idx < METER_GROUP_NUM; idx ++)
		printk("Meter group[0].MeterUseState[%d] : %x\n", idx, gpQdmaPriv->meterMgr.MeterUseState[idx]);
    if(TCSUPPORT_ACCOUNT_METER_V2_VAL){
		printk("Meter group[1].MeterUseState[0] : %x\n", gpQdmaPriv->meter1Mgr.MeterUseState[0]);
		printk("Meter group[2].MeterUseState[0] : %x\n", gpQdmaPriv->meter2Mgr.MeterUseState[0]);
    }

    /*printk binding meter */
    printk("QDMA meter_0 map state: \n");
    printk("meter_id\tstatus\tfunc\t\tenable\tratevalue\n");
    for(meter_idx = 0; meter_idx < gpQdmaPriv->meterMgr.MaxMeterNum; meter_idx ++){
        if(((gpQdmaPriv->meterMgr.MeterUseState[meter_idx>>5])>>(meter_idx&0x1F))&1){
            if(trtcmMode == TRTCM_RATELIMIT_MODE) {
                rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;
                rxRateLimitCfg.Index = meter_idx ;
                qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRateLimitCfg;
                qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);

                rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
                rxRateLimitSet.Index = meter_idx;
                qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &rxRateLimitSet;
                qdma_general_get_ratelimit_mode_value(&qdma_data_tmp);
                
                printk("%d\t\t%s\t%s\t%s\t%d%s\n", meter_idx, "Using", gpQdmaPriv->meterMgr.MeterUseFunc[meter_idx],
                (rxRateLimitCfg.MeterEn == GENERAL_METER_ENABLE) ? ("Yes"):("No"),
                rxRateLimitSet.RateLimitValue,
                (rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
                );
            } else {
                rxTrtcmCbsPbsCfg.trtcmModule = INGRESS_TRTCM ;
                rxTrtcmCbsPbsCfg.Index = meter_idx ;
                qdma_data_tmp.qdma_private.generalTrtcmCbsPbsCfgPtr = &rxTrtcmCbsPbsCfg;
                qdma_general_get_trtcm_mode_cfg(&qdma_data_tmp);
                
                rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
                rxTrtcmCbsPbsSet.Index = meter_idx;
                qdma_data_tmp.qdma_private.generalTrtcmCbsPbsSetPtr = &rxTrtcmCbsPbsSet;
                qdma_general_get_trtcm_mode_value(&qdma_data_tmp);
                
                if(rxTrtcmCbsPbsSet.CbsParamValue == rxTrtcmCbsPbsSet.PbsParamValue) {
                    printk("%d\t\t%s\t%s\t%s\t%d%s\n", meter_idx, "Using", gpQdmaPriv->meterMgr.MeterUseFunc[meter_idx],
                    ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
                    rxTrtcmCbsPbsSet.PbsParamValue,
                    (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
                    );
                } else {
                    printk("%d\t\t%s\t%s\t%s\t%d%s~%d%s\n", meter_idx, "Using", gpQdmaPriv->meterMgr.MeterUseFunc[meter_idx],
                    ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
                    rxTrtcmCbsPbsSet.CbsParamValue,
                    (rxTrtcmCbsPbsCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
                    rxTrtcmCbsPbsSet.PbsParamValue,
                    (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
                    );
                }
            }
        }
    }        

	if(TCSUPPORT_ACCOUNT_METER_V2_VAL){
	    printk("QDMA meter_1 map state: \n");
	    printk("meter_id\tstatus\tfunc\t\tenable\tratevalue\n");
	    for(meter_idx = 0; meter_idx < gpQdmaPriv->meter1Mgr.MaxMeterNum; meter_idx ++){
	        if(((gpQdmaPriv->meter1Mgr.MeterUseState[meter_idx>>5])>>(meter_idx&0x1F))&1){
	            if(trtcmMode == TRTCM_RATELIMIT_MODE) {
	                rxRateLimitCfg.trtcmModule = INGRESS_TRTCM;
	                rxRateLimitCfg.Index = ((0x1<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRateLimitCfg;
	                qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);

	                rxRateLimitSet.trtcmModule = INGRESS_TRTCM;
	                rxRateLimitSet.Index = ((0x1<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &rxRateLimitSet;
	                qdma_general_get_ratelimit_mode_value(&qdma_data_tmp);
	                
	                printk("%d\t\t%s\t%s\t%s\t%d%s\n", meter_idx, "Using", gpQdmaPriv->meter1Mgr.MeterUseFunc[meter_idx],
	                (rxRateLimitCfg.MeterEn == GENERAL_METER_ENABLE) ? ("Yes"):("No"),
	                rxRateLimitSet.RateLimitValue,
	                (rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
	                );
	            } else {
	                rxTrtcmCbsPbsCfg.trtcmModule = INGRESS_TRTCM ;
	                rxTrtcmCbsPbsCfg.Index = ((0x1<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmCbsPbsCfgPtr = &rxTrtcmCbsPbsCfg;
	                qdma_general_get_trtcm_mode_cfg(&qdma_data_tmp);
	                
	                rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
	                rxTrtcmCbsPbsSet.Index = ((0x1<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmCbsPbsSetPtr = &rxTrtcmCbsPbsSet;
	                qdma_general_get_trtcm_mode_value(&qdma_data_tmp);
	                
	                if(rxTrtcmCbsPbsSet.CbsParamValue == rxTrtcmCbsPbsSet.PbsParamValue) {
	                    printk("%d\t\t%s\t%s\t%s\t%d%s\n", meter_idx, "Using", gpQdmaPriv->meter1Mgr.MeterUseFunc[meter_idx],
	                    ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
	                    rxTrtcmCbsPbsSet.PbsParamValue,
	                    (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
	                    );
	                } else {
	                    printk("%d\t\t%s\t%s\t%s\t%d%s~%d%s\n", meter_idx, "Using", gpQdmaPriv->meter2Mgr.MeterUseFunc[meter_idx],
	                    ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
	                    rxTrtcmCbsPbsSet.CbsParamValue,
	                    (rxTrtcmCbsPbsCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
	                    rxTrtcmCbsPbsSet.PbsParamValue,
	                    (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
	                    );
	                }
	            }
	        }
	    }   	

	    printk("QDMA meter_2 map state: \n");
	    printk("meter_id\tstatus\tfunc\t\tenable\tratevalue\n");
	    for(meter_idx = 0; meter_idx < gpQdmaPriv->meter2Mgr.MaxMeterNum; meter_idx ++){
	        if(((gpQdmaPriv->meter2Mgr.MeterUseState[meter_idx>>5])>>(meter_idx&0x1F))&1){
	            if(trtcmMode == TRTCM_RATELIMIT_MODE) {
	                rxRateLimitCfg.trtcmModule = INGRESS_TRTCM;
	                rxRateLimitCfg.Index = ((0x2<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmRatelimitCfgPtr = &rxRateLimitCfg;
	                qdma_general_get_ratelimit_mode_cfg(&qdma_data_tmp);

	                rxRateLimitSet.trtcmModule = INGRESS_TRTCM;
	                rxRateLimitSet.Index = ((0x2<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmRatelimitSetPtr = &rxRateLimitSet;
	                qdma_general_get_ratelimit_mode_value(&qdma_data_tmp);
	                
	                printk("%d\t\t%s\t%s\t%s\t%d%s\n", meter_idx, "Using", gpQdmaPriv->meter2Mgr.MeterUseFunc[meter_idx],
	                (rxRateLimitCfg.MeterEn == GENERAL_METER_ENABLE) ? ("Yes"):("No"),
	                rxRateLimitSet.RateLimitValue,
	                (rxRateLimitCfg.PktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
	                );
	            } else {
	                rxTrtcmCbsPbsCfg.trtcmModule = INGRESS_TRTCM ;
	                rxTrtcmCbsPbsCfg.Index = ((0x2<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmCbsPbsCfgPtr = &rxTrtcmCbsPbsCfg;
	                qdma_general_get_trtcm_mode_cfg(&qdma_data_tmp);
	                
	                rxTrtcmCbsPbsSet.trtcmModule = INGRESS_TRTCM ;
	                rxTrtcmCbsPbsSet.Index = ((0x2<<8) | meter_idx);
	                qdma_data_tmp.qdma_private.generalTrtcmCbsPbsSetPtr = &rxTrtcmCbsPbsSet;
	                qdma_general_get_trtcm_mode_value(&qdma_data_tmp);
	                
	                if(rxTrtcmCbsPbsSet.CbsParamValue == rxTrtcmCbsPbsSet.PbsParamValue) {
	                    printk("%d\t\t%s\t%s\t%s\t%d%s\n", meter_idx, "Using", gpQdmaPriv->meter2Mgr.MeterUseFunc[meter_idx],
	                    ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
	                    rxTrtcmCbsPbsSet.PbsParamValue,
	                    (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
	                    );
	                } else {
	                    printk("%d\t\t%s\t%s\t%s\t%d%s~%d%s\n", meter_idx, "Using", gpQdmaPriv->meter2Mgr.MeterUseFunc[meter_idx],
	                    ((rxTrtcmCbsPbsCfg.CbsMeterEn == GENERAL_METER_ENABLE) ||(rxTrtcmCbsPbsCfg.PbsMeterEn == GENERAL_METER_ENABLE)) ? ("Yes"):("No"),
	                    rxTrtcmCbsPbsSet.CbsParamValue,
	                    (rxTrtcmCbsPbsCfg.CbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps"),
	                    rxTrtcmCbsPbsSet.PbsParamValue,
	                    (rxTrtcmCbsPbsCfg.PbsPktMode == TRTCM_PACKET_MODE) ? ("pps"):("kbps")
	                    );
	                }
	            }
	        }
	    }   	
	}
    return 0;
}

static int qdma_bm_acnt_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int idx = 0;
    
    printk("Acnt0 using state\n");
    printk("Acnt0_id\tstatus\tfunc\n");

    for(idx = 0; idx <CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM; idx ++)
    {
        if(((gpQdmaPriv->acnt0Mgr.Acnt0UseState) >> idx)&1)
        {
            printk("%d\t\t%s\t%s\n", idx, "Using", gpQdmaPriv->acnt0Mgr.Acnt0UseFunc[idx]);
        }
    }

    printk("\nAcnt1 using state\n");
    printk("Acnt1_id\tstatus\tfunc\n");

    for(idx = 0; idx <CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM; idx ++)
    {
        if((gpQdmaPriv->acnt1Mgr.Acnt1UseState >> idx) & 1)
        {
            printk("%d\t\t%s\t%s\n", idx, "Using", gpQdmaPriv->acnt1Mgr.Acnt1UseFunc[idx]);
        }
    }
        
    printk("\nAcnt2 using state\n");
    printk("Acnt2_id\tstatus\tfunc\n");
        
    for(idx = 0; idx <CONFIG_FLOWCNT_GRP2_MAX_IDX_NUM; idx ++)
    {
        if((idx <= 63) && ((gpQdmaPriv->acnt2Mgr.Acnt2UseState0 >> idx) & 1ULL))
        {
            printk("%d\t\t%s\t%s\n", idx, "Using", gpQdmaPriv->acnt2Mgr.Acnt2UseFunc[idx]);
	    }else if((gpQdmaPriv->acnt2Mgr.Acnt2UseState1 >> (idx - 64)) & 1ULL){//for acnt64~127
            printk("%d\t\t%s\t%s\n", idx, "Using", gpQdmaPriv->acnt2Mgr.Acnt2UseFunc[idx]);
	    }
    }
    
    return 0;
}
/*need to be filled for EN7580*/
static int qdma_bm_ratelimit_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;

	return index ;
}

static int qdma_bm_ratelimit_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	return count ;
}
#else
static int qdma_bm_ratelimit_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+ index, "echo ratelimit [tx/rx] r > /proc/%s\n", ratelimit_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo ratelimit [tx/rx] w [Enable/Disable] [unit:(1~%dkbps)] > /proc/%s\n", UNIT_MAX, ratelimit_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo ratelimit [tx/rx] w [chanIdx/ringIdx] [pir:(kbps)] [dropEn:Enable/Disable]> /proc/%s\n", ratelimit_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo ratemeter tx r > /proc/%s\n", ratelimit_path) ;
	CHK_BUF() ;
	/*  the unit can not be 1kbps for the minimum rate meter unit is 2. */
	index += sprintf(buf+ index, "echo ratemeter tx w [Enable/Disable] [unit:(2~%dkbps)] > /proc/%s\n", UNIT_MAX, ratelimit_path) ;
	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_bm_ratelimit_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128] = {0}, cmd[128] = {0}, direction[128] = {0}, action[128] = {0}, Enable[128] = {0}, dropEn[128] = {0};
	int index=0, pir=0, unit=0, dataRate=0 ;
	int enFlag=0, dropEnFlag=0;
	int i=0;
	QDMA_TxRateLimitCfg_T txRateLimitCfg ;
	QDMA_RxRateLimitCfg_T rxRateLimitCfg ;
	QDMA_TxRateLimitSet_T txRateLimitSetParam ;
	QDMA_TxRateLimitGet_T txRateLimitGetParam ;
	QDMA_RxRateLimitSet_T rxRateLimitSetParam ;
	QDMA_RxRateLimitGet_T rxRateLimitGetParam ;
	QDMA_TxRateMeter_T txRateMeterCfg ;
	struct ECNT_QDMA_Data qdma_data;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %s %s %s %d %s", cmd, direction, action, Enable, &pir, dropEn) ;

	if(!strcmp(Enable, "Enable"))
		enFlag = QDMA_ENABLE ;
	else if(!strcmp(Enable, "Disable"))
		enFlag = QDMA_DISABLE ;
	else {
		index = atoi(Enable);
		if((index < 0) || (index > CONFIG_QDMA_CHANNEL && (!strcmp(direction, "tx"))) || (index > RX_RING_NUM && (!strcmp(direction, "rx")))) {
			printk("Fault: index is %d, which should between 0 and %d\n", index, ((!strcmp(direction, "tx"))?(CONFIG_QDMA_CHANNEL):(RX_RING_NUM))) ;
			return -EFAULT ;
		}
	}
	
	if(!strcmp(dropEn, "Enable"))
		dropEnFlag = QDMA_ENABLE ;
	else
		dropEnFlag = QDMA_DISABLE ;

	if((!strcmp(action, "w")) && (!strcmp(cmd, "ratelimit")) && ((!strcmp(Enable, "Enable")) || (!strcmp(Enable, "Disable")))) {
		unit = pir;
		if(unit<1 || unit>UNIT_MAX) {
			printk("Fault: Rate Limit unit is %dkbps, which should between 1 and %d\n", unit, UNIT_MAX) ;
			return -EFAULT ;
		}
	}
	if((!strcmp(action, "w")) && (!strcmp(cmd, "ratemeter"))) {
		unit = pir;
		if(unit<2 || unit>UNIT_MAX) {
			printk("Fault: Rate Meter unit is %dkpbs, which should between 2 and %d\n", unit, UNIT_MAX) ;
			return -EFAULT ;
		}
	}
	
	if(!strcmp(cmd, "ratelimit")) {
		if(!strcmp(direction, "tx")) {
			if(!strcmp(action, "r")) {
				qdma_data.qdma_private.txRateLimitCfgPtr = &txRateLimitCfg;
				qdma_get_tx_ratelimit_cfg(&qdma_data) ;
				printk("txRateLimitEn:Enable, txBucketScale:%d, ", 1<<txRateLimitCfg.txRateLimitBucketScale);

				for(i=0; i<PER_CHNL_TICKSEL_NUM; i++)
					printk("unit%d:%dkbps ", i,txRateLimitCfg.txRateLimitUnit[i]);
				printk("\n");
				
				for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
					txRateLimitGetParam.chnlIdx = (unchar)index;
					qdma_data.qdma_private.txRateLimitGetPtr = &txRateLimitGetParam;
					qdma_get_tx_ratelimit(&qdma_data) ;
					if(txRateLimitGetParam.rateLimitValue || txRateLimitGetParam.pbsParamValue) {
						printk("Channel_%d:  pir:%dkbps, pbs:%dByte\n", index, txRateLimitGetParam.rateLimitValue, 
							txRateLimitGetParam.pbsParamValue<<txRateLimitCfg.txRateLimitBucketScale) ;
					}
				}
			} else if(!strcmp(action, "w")) {
				if(unit != 0) {	/*  enable or disable */
					txRateLimitCfg.txRateLimitBucketScale = QDMA_TRTCM_SCALE_2BYTE ;
					txRateLimitCfg.txRateLimitUnit[1] = unit ;  //only can modify tick1 <--> unit1
					qdma_data.qdma_private.txRateLimitCfgPtr = &txRateLimitCfg;
					//qdma_set_tx_ratelimit_cfg(&qdma_data) ;
				} else {		/*  per channel ratelimit config */
					txRateLimitSetParam.chnlIdx = index ;
					//txRateLimitSetParam.pbsParamValue = 10000 ;
					txRateLimitSetParam.rateLimitValue = pir ;
					if(txRateLimitSetParam.rateLimitValue > (4<<20)) {
						printk("Error: ratelimitValue is %dkbps, which shold less than 4gbps!", pir);
						return -EFAULT;
					}
					printk("channel:%d, pirParamValue:%ukbps\n", index, txRateLimitSetParam.rateLimitValue) ;
					qdma_data.qdma_private.txRateLimitSetPtr = &txRateLimitSetParam;
					qdma_set_tx_ratelimit(&qdma_data) ;
				}
			}
		} else if(!strcmp(direction, "rx")) {
			if(!strcmp(action, "r")) {
				qdma_data.qdma_private.rxRateLimitCfgPtr = &rxRateLimitCfg;
				qdma_get_rx_ratelimit_cfg(&qdma_data) ;
				printk("rxDropEn:Enable, rxRateLimitEn:%s, rxBucketScale:%d, rxRateLimitUnit:%dkbps\n", 
					(rxRateLimitCfg.rxRateLimitEn)?("Enable"):("Disable"), 1<<rxRateLimitCfg.rxRateLimitBucketScale, rxRateLimitCfg.rxRateLimitUnit) ;
				if(rxRateLimitCfg.rxRateLimitUnit) {
					for(index=0; index<RX_RING_NUM; index++) {
						rxRateLimitGetParam.ringIdx = index;
						qdma_data.qdma_private.rxRateLimitGetPtr = &rxRateLimitGetParam;
						qdma_get_rx_ratelimit(&qdma_data) ;
						if(rxRateLimitGetParam.rateLimitValue || rxRateLimitGetParam.pbsParamValue) {
							printk("RX%d:  pir:%dkbps, pbs:%dByte\n", index, rxRateLimitGetParam.rateLimitValue, rxRateLimitGetParam.pbsParamValue<<rxRateLimitCfg.rxRateLimitBucketScale) ;
						}
					}
				}
			} else if(!strcmp(action, "w")) {
				if(unit != 0) {
					rxRateLimitCfg.rxRateLimitEn = enFlag ;
					rxRateLimitCfg.rxRateLimitBucketScale = QDMA_TRTCM_SCALE_2BYTE ;
					rxRateLimitCfg.rxRateLimitUnit = unit ;
					qdma_data.qdma_private.mode = enFlag;
					qdma_set_rx_ratelimit_en(&qdma_data) ;
					qdma_data.qdma_private.pktMode = QDMA_RX_RATE_LIMIT_BY_BYTE;
					qdma_set_rx_ratelimit_pkt_mode(&qdma_data) ;
				} else {
					rxRateLimitSetParam.ringIdx = index ;
					//rxRateLimitSetParam.pbsParamValue = 10000 ;
					rxRateLimitSetParam.rateLimitValue = pir ;
					if(rxRateLimitSetParam.rateLimitValue > (4<<20)) {
						printk("Error: ratelimitValue is %dkbps, which shold less than 4gbps!", pir);
						return -EFAULT;
					}
					printk("ringIdx:%d, pirParamValue:%ukbps\n", index, rxRateLimitSetParam.rateLimitValue) ;
					qdma_data.qdma_private.rxRateLimitSetPtr = &rxRateLimitSetParam;
					qdma_set_rx_ratelimit(&qdma_data) ;
				}
			}
		} else {
			printk("Fault: ratelimit only effect on tx/rx Direction\n") ;		
		}
	} else if(!strcmp(cmd, "ratemeter")) {	
		if(!strcmp(direction, "tx")) {
			if(!strcmp(action, "r")) {
				qdma_data.qdma_private.txRateMeterPtr = &txRateMeterCfg;
				qdma_get_tx_ratemeter(&qdma_data) ;
				printk("txRateMeterEn:Enable, txRateMeterDivisor:%d, txRateMeterTimeSlice:%dus\n", 
					1<<txRateMeterCfg.txRateMeterTimeDivisor, txRateMeterCfg.txRateMeterTimeSlice) ;
				for(index=0; index<CONFIG_QDMA_CHANNEL; index++) {
					qdma_data.qdma_private.channel = index;
					dataRate = qdma_get_tx_channel_datarate(&qdma_data);
					if(dataRate != -1) {
						printk("Channel_%d: rateMeter:%dkbps\n", index, dataRate/1000) ;
					}
				}
			} else if(!strcmp(action, "w")) {
				if(qdmaRateMeterValueCalCulate(unit, (ushort *)&txRateMeterCfg.txRateMeterTimeDivisor, (ushort *)&txRateMeterCfg.txRateMeterTimeSlice) != -1) {
					qdma_data.qdma_private.txRateMeterPtr = &txRateMeterCfg;
					qdma_set_tx_ratemeter(&qdma_data) ;
				}
			}
		} else {
			printk("Fault: ratemeter only effect on tx Direction\n") ;		
		}
	}

	return count ;
}
#endif

static int qdma_bm_qos_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+ index, "echo [chnlId:0~31] sp > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo [chnlId:0~31] wrr w0 w1 w2 w3 w4 w5 w6 w7 > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo [chnlId:0~31] wrr2 w0 w1 > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo [chnlId:0~31] wrr3 w0 w1 w2 > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo [chnlId:0~31] wrr4 w0 w1 w2 w3 > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo [chnlId:0~31] wrr5 w0 w1 w2 w3 w4 > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo [chnlId:0~31] wrr6 w0 w1 w2 w3 w4 w5 > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	index += sprintf(buf+ index, "echo [chnlId:0~31] wrr7 w0 w1 w2 w3 w4 w5 w6 > /proc/%s\n", qos_path) ;
	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_bm_qos_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128] = {0}, mode[128] = {0};
	int i=0, chnlID=0, weight[CONFIG_QDMA_QUEUE] ;
	QDMA_TxQosScheduler_T txQos;
	struct ECNT_QDMA_Data qdma_data;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%d %s %d %d %d %d %d %d %d %d", &chnlID, mode, &weight[0], &weight[1], &weight[2], &weight[3], &weight[4], &weight[5], &weight[6], &weight[7]) ;
	
	txQos.channel = chnlID ;
	
	for(i=0 ; i<CONFIG_QDMA_QUEUE ; i++) {
		txQos.queue[i].weight = weight[i];
	}

	if(!strcmp(mode, "sp")) {
		txQos.qosType = 1 ;
	} else if(!strcmp(mode, "wrr")) {
		txQos.qosType = 0 ;
	} else if(!strcmp(mode, "wrr2")) {
		txQos.qosType = 7 ;
	} else if(!strcmp(mode, "wrr3")) {
		txQos.qosType = 6 ;
	} else if(!strcmp(mode, "wrr4")) {
		txQos.qosType = 5 ;
	} else if(!strcmp(mode, "wrr5")) {
		txQos.qosType = 4 ;
	} else if(!strcmp(mode, "wrr6")) {
		txQos.qosType = 3 ;
	} else if(!strcmp(mode, "wrr7")) {
		txQos.qosType = 2 ;
	}
	
	qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
	qdma_set_tx_qos(&qdma_data);

	return count ;
}

#if defined (QOS_REMARKING) || defined (TCSUPPORT_HW_QOS)
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
static int isEightQueueQos(void)
{
	if((qos_flag == QOS_HW_WRR_8_QUEUE) || (qos_flag == QOS_HW_PQ_8_QUEUE)||(qos_flag == QOS_HW_PQ_CIRPIR_8_QUEUE))
		return 1;

	return 0;
}
#endif

static int mac_qos_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data){

#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	if(isEightQueueQos())
	{
		int len = sprintf(page, "%s %d %d %d %d %d %d %d %d\r\n",(*qos_wrr_info)?"WRR":"SP", *(qos_wrr_info + 1), \
			*(qos_wrr_info + 2), *(qos_wrr_info + 3), *(qos_wrr_info + 4), *(qos_wrr_info + 5), \
			*(qos_wrr_info + 6), *(qos_wrr_info + 7), *(qos_wrr_info + 8));
		return len;
	}
	else 
	{
#endif
		int len = sprintf(page, "%d %d %d %d %d\r\n",*qos_wrr_info, *(qos_wrr_info + 1), *(qos_wrr_info + 2), *(qos_wrr_info + 3), *(qos_wrr_info + 4));
		return len;
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	}
#endif
}
static int mac_qos_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	int len;
	char get_buf[32];
	int max_wrr_val = 0, i, j;
	QDMA_TxQosScheduler_T txQos;
	struct ECNT_QDMA_Data qdma_data;
	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > sizeof(get_buf) - 1)
		len = sizeof(get_buf) - 1;
	else
		len = count;
	/* use the copy_from_user function to copy buffer data to our get_buf */
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	/* zero terminate get_buf */
	get_buf[len]='\0';

#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	if(isEightQueueQos())
	{
		if(sscanf(get_buf, "%d %d %d %d %d %d %d %d %d", qos_wrr_info, (qos_wrr_info+1), \
			(qos_wrr_info+2), (qos_wrr_info+3), (qos_wrr_info+4), (qos_wrr_info+5), \
			(qos_wrr_info+6), (qos_wrr_info+7), (qos_wrr_info+8))!= 9)
			return count;
	}
	else
	{
#endif
		if(sscanf(get_buf, "%d %d %d %d %d", qos_wrr_info, (qos_wrr_info+1), (qos_wrr_info+2), (qos_wrr_info+3), (qos_wrr_info+4)) != 5)
			return count;
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	}
#endif
	
	/* find max qos wrr weight */
	for (i = 0; i < 4; i++) {
		if (max_wrr_val < qos_wrr_info[i + 1]) {
			max_wrr_val = qos_wrr_info[i + 1];
			max_prio = i;
		}
	}

	printk("qos_wrr_info: %d %d %d %d %d\n", qos_wrr_info[0], qos_wrr_info[1], qos_wrr_info[2], qos_wrr_info[3], qos_wrr_info[4]);

	qos_wrr_user = QOS_DMAWRR_USERDEFINE;
	if(*qos_wrr_info == 0){  /*strict priority*/
	/*TSARM_GFR &= ~(GFR_DMT_WRR_EN); */

		printk("QDMA QoS: HW SP\n");

        /* set QDMA as Strict Priority Mode,
	 	 * so that p7> ... >p0 for all paths */
		for(i=0 ; i<QOS_CHANNEL_MAX_IDX ; i++){
			txQos.channel = i ;
			txQos.qosType = QDMA_TXQOS_TYPE_SP ;	/*  Strict Priority */

			/* QDMA has 8 queue */
			for(j=0 ; j<8 ; j++){
				txQos.queue[j].weight = 0;
			}
            
			qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
			qdma_set_tx_qos(&qdma_data) ;
		}

		for(i=0 ; i<QOS_CHANNEL_MAX_IDX ; i++){
			memset(&txQos, 0, sizeof(QDMA_TxQosScheduler_T)) ;
			txQos.channel = i ;
			qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
			qdma_get_tx_qos(&qdma_data) ;
		}
	} else{  /*WRR*/

		printk("QDMA QoS: HW WRR\n");

        /* EN7512:set QDMA's SP & WRR priority registers,
	 	 * so that p7>p6>p5>p4> p3:p2:p1:p0 for all path && 
		 * p3:p2:p1:p0 == qos_wrr_info[1]:qos_wrr_info[2]:
		 * 				  qos_wrr_info[3]:qos_wrr_info[4] */
		for(i=0 ; i<QOS_CHANNEL_MAX_IDX ; i++){
			txQos.channel = i ;            
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
			if(isEightQueueQos())
			{
				txQos.qosType = QDMA_TXQOS_TYPE_WRR ;
				for(j=0 ; j<8 ; j++){
					txQos.queue[j].weight = qos_wrr_info[8-j];
				}
			}

			else
			{
#endif
			txQos.qosType = QDMA_TXQOS_TYPE_SPWRR4 ;
            
			/* QDMA has 8 queue */
			for(j=0 ; j<4 ; j++){
				txQos.queue[j].weight = qos_wrr_info[4-j];
			}

			for(j=4 ; j<8 ; j++){
				txQos.queue[j].weight = 0;
			}
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
			}
#endif     
			if(txQos.queue[0].weight == 0)
				txQos.queue[0].weight = 1;
            
			qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
			qdma_set_tx_qos(&qdma_data) ;
		}

		for(i=0 ; i<QOS_CHANNEL_MAX_IDX ; i++){
			memset(&txQos, 0, sizeof(QDMA_TxQosScheduler_T)) ;
			txQos.channel = i ;
			qdma_data.qdma_private.qdma_tx_qos.pTxQos = &txQos;
			qdma_get_tx_qos(&qdma_data) ;
          
            printk("Channel:%d, Type:%d, Q0:%d, Q1:%d, Q2:%d, Q3:%d, Q4:%d, Q5:%d, Q6:%d, Q7:%d\n",
	           i, txQos.qosType,
               txQos.queue[0].weight,
               txQos.queue[1].weight,
               txQos.queue[2].weight,
               txQos.queue[3].weight,
               txQos.queue[4].weight,
               txQos.queue[5].weight,
               txQos.queue[6].weight,
                   txQos.queue[7].weight) ;
		}
	}
	return len;
}
#endif

#ifdef TCSUPPORT_QOS
static int mac_default_queuemask_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data
)
{
	printk("\nqos gQueueMask is 0x%x\n",gQueueMask);		
	return 0;
}


static int mac_default_queuemask_write_proc(
		struct file *file, const char *buffer,
		unsigned long count, void *data
)
{
	char valString[8];
	int val = 0;
	
	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buffer, count))
		return -EINVAL;

	valString[count] = '\0';
	sscanf(valString, "%d", &val);
	if (val)
	{
		gQueueMask = val;
		QDMA_MSG(DBG_ST, "\nqos gQueueMask is 0x%x\n",gQueueMask);
	}

	return count;
}


static int mac_qos_flag_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data){

	if (qos_flag == QOS_SW_PQ) {
		printk("qos discipline is PQ.\n");
	}
	else if (qos_flag == QOS_SW_WRR) {
		printk("qos discipline is WRR.\n");
	}
	else if (qos_flag == QOS_SW_CAR) {
		printk("qos discipline is CAR.\n");
	}
	else if (qos_flag == QOS_HW_WRR) {
		printk("qos discipline is HW WRR.\n");
	}
	else if (qos_flag == QOS_HW_PQ) {
		printk("qos discipline is HW PQ.\n");
	}
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	else if (qos_flag == QOS_HW_WRR_8_QUEUE) {
		printk("qos discipline is HW WRR 8 queue.\n");
	}
	else if (qos_flag == QOS_HW_PQ_8_QUEUE) {
		printk("qos discipline is HW PQ 8 queue.\n");
	}
	else if (qos_flag == QOS_HW_PQ_CIRPIR_8_QUEUE) {
		printk("qos discipline is HW PQ CIRPIR 8 queue.\n");
	}
#endif
	else {
		printk("qos is disabled.\n");
	}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS_CA) 	
	printk("\n isNeedHWQoS =%d.\n\n",isNeedHWQoS);
#endif/*TCSUPPORT_COMPILE*/
	return 0;
}

static int mac_qos_flag_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){

	char qos_disc[10];
	int len;


	if (count > sizeof(qos_disc) - 1) {
		len = sizeof(qos_disc) - 1;
	}
	else {
		len = count;
	}
	memset(qos_disc, 0, sizeof(qos_disc));
	
	if(copy_from_user(qos_disc, buffer, len))
		return -EFAULT;

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS_CA) 
	isNeedHWQoS = 1;
#endif/*TCSUPPORT_COMPILE*/
	qos_disc[len] = '\0';
	if (!strncmp(qos_disc, "PQ", 2)) {
		qos_flag = QOS_SW_PQ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS_CA) 
		isNeedHWQoS = 0;
#endif/*TCSUPPORT_COMPILE*/
	}
	else if (!strncmp(qos_disc, "WRR", 3)) {
		qos_flag = QOS_SW_WRR;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS_CA) 
		isNeedHWQoS = 0;
#endif/*TCSUPPORT_COMPILE*/
	}
	else if (!strncmp(qos_disc, "CAR", 3)) {
		qos_flag = QOS_SW_CAR;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS_CA) 
		isNeedHWQoS = 0;
#endif/*TCSUPPORT_COMPILE*/
	}
	else if (!strncmp(qos_disc, "HWWRR", 5)) {
		qos_flag = QOS_HW_WRR;
	}
	else if (!strncmp(qos_disc, "HWPQ", 4)) {
		qos_flag = QOS_HW_PQ;
	}
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
	else if (!strncmp(qos_disc, "8QWRR", 4)) {
		qos_flag = QOS_HW_WRR_8_QUEUE;
	}
	else if (!strncmp(qos_disc, "8QPQ", 4)) {
		qos_flag = QOS_HW_PQ_8_QUEUE;
	}
	else if (!strncmp(qos_disc, "CP8QPQ", 6)) {
		qos_flag = QOS_HW_PQ_CIRPIR_8_QUEUE;
	}
#endif
	else {
		qos_flag = NULLQOS;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CPU_MT7505)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_HWQOS) || defined(TCSUPPORT_XPON_HAL_API_QOS_CA)
		isNeedHWQoS = 0;
#endif/*TCSUPPORT_COMPILE*/		
#endif/*TCSUPPORT_COMPILE*/		
	}

	return len;
}
#endif

static int mac_resource_limit_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64];
	int ret = 0;
		
	if (count > sizeof(val_string) - 1)
	{
		ret = -EFAULT;
		return ret;
	}
	memset(val_string,0,64);
	if (copy_from_user(val_string, buffer, count))
	{
		ret = -EFAULT;
		return ret;
	}
	sscanf(val_string, "%x %x", &channel_limit_threshold, &queue_limit_threshold) ;
	
	return count;
}

static int mac_resource_limit_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i=0, j=0;

	printk("0x%x, 0x%x\n",channel_limit_threshold,queue_limit_threshold);

	for(i=0; i<CONFIG_QDMA_CHANNEL; i++) {
		printk("chnlLimit[%d]:%d\n queueLimit[%d]:", i, (int)atomic_read(&chnlLimit[i]), i);
		for(j=0; j<CONFIG_QDMA_QUEUE; j++) {
			printk("%d  ", (int)atomic_read(&queueLimit[i][j]));
		}
		printk("\n");
	}
	return 0;
}

static int downstream_qos_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64];
	int ret = 0;
		
	if (count > sizeof(val_string) - 1)
	{
		ret = -EFAULT;
		return ret;
	}
	memset(val_string,0,64);
	if (copy_from_user(val_string, buffer, count))
	{
		ret = -EFAULT;
		return ret;
	}
	sscanf(val_string, "%d", &dnstream_qos_enable) ;
	
	return count;
}

static int downstream_qos_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i=0, j=0;

	printk("dnstream_qos_enable = %d\n",dnstream_qos_enable);

	return 0;
}

static int qdma_ring_bind_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int ring_idx = 0;
	int index = 0;
	off_t pos = 0, begin = 0;

    index += sprintf(buf+index, "INT1_RX_RING_MASK:%.4X\nINT2_RX_RING_MASK:%.4X\nINT3_RX_RING_MASK:%.4X\nINT4_RX_RING_MASK:%.4X\n",
                                int1_rx_ring , int2_rx_ring , int3_rx_ring , int4_rx_ring) ;

    index += sprintf(buf+index, "Ring_Idx    INT-1    INT-2    INT-3    INT-4\n") ;
    for(ring_idx=0 ; ring_idx<RX_RING_NUM ; ring_idx++){
        index += sprintf( buf+index, "   %d\t\t%d\t%d\t%d\t%d\t\n", ring_idx, 
                                                            (int1_rx_ring&(1<<ring_idx))?1:0,
                                                            (int2_rx_ring&(1<<ring_idx))?1:0,
                                                            (int3_rx_ring&(1<<ring_idx))?1:0,
                                                            (int4_rx_ring&(1<<ring_idx))?1:0 );
    }
#if SUPPORT_LRO
    index += sprintf(buf+index, "WARNING: EN7580 INT2 for LRO, do not change ring bind !!!\n") ;
#endif

#if SUPPORT_IRQ2
	index += sprintf(buf+index, "IRQ_TX_RING_MASK:%.4X(0:IRQ1, 1:IRQ2)\n", irq_tx_ring) ;

    index += sprintf(buf+index, "Ring_Idx    IRQ-1    IRQ-2\n") ;
    for(ring_idx=0 ; ring_idx<TX_RING_NUM ; ring_idx++){
        index += sprintf( buf+index, "   %d\t\t%d\t%d\t\n", ring_idx, 
                                                            (irq_tx_ring&(1<<ring_idx))?0:1,
                                                            (irq_tx_ring&(1<<ring_idx))?1:0 );
    }
#endif

	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_ring_bind_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    uint base = gpQdmaPriv->csrBaseAddr ;
	char val_string[128];
    char cmd[128] = {0};
    uint ring_mask = 0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %x", cmd, &ring_mask) ;

    if(!strcmp(cmd, "int1")) {
		#if !SUPPORT_32_RX_RING
        	ring_mask = ring_mask & 0xFFFF;
        #endif
        if(ring_mask & (int2_rx_ring | int3_rx_ring | int4_rx_ring)){
            QDMA_ERR("invalid ring bind mask, the same ring should not be binded to multi-INTs! \r\n");
            return count ;
        }
        int1_rx_ring = ring_mask ;
        qdmaSetIntBind(base,QDMA_INT1);
    }else if(!strcmp(cmd, "int2")) {
        #if !SUPPORT_32_RX_RING
        	ring_mask = ring_mask & 0xFFFF;
        #endif
        if(ring_mask & (int1_rx_ring | int3_rx_ring | int4_rx_ring)){
            QDMA_ERR("invalid ring bind mask, the same ring should not be binded to multi-INTs! \r\n");
            return count ;
        }
        int2_rx_ring = ring_mask ;
        qdmaSetIntBind(base,QDMA_INT2);
    }else if(!strcmp(cmd, "int3")) {
        #if !SUPPORT_32_RX_RING
        	ring_mask = ring_mask & 0xFFFF;
        #endif
        if(ring_mask & (int1_rx_ring | int2_rx_ring | int4_rx_ring)){
            QDMA_ERR("invalid ring bind mask, the same ring should not be binded to multi-INTs! \r\n");
            return count ;
        }
        int3_rx_ring = ring_mask ;
        qdmaSetIntBind(base,QDMA_INT3);
    }else if(!strcmp(cmd, "int4")) {
        #if !SUPPORT_32_RX_RING
        	ring_mask = ring_mask & 0xFFFF;
        #endif
        if(ring_mask & (int1_rx_ring | int2_rx_ring | int3_rx_ring)){
            QDMA_ERR("invalid ring bind mask, the same ring should not be binded to multi-INTs! \r\n");
            return count ;
        }
        int4_rx_ring = ring_mask ;
        qdmaSetIntBind(base,QDMA_INT4);
    }
#if SUPPORT_IRQ2
	else if(!strcmp(cmd, "irq")) {
        #if !SUPPORT_32_RX_RING
        	ring_mask = ring_mask & 0xFF;
        #endif
		irq_tx_ring = ring_mask ;
		qdmaSetIrqBind(base,QDMA_IRQ1);
		qdmaSetIrqBind(base,QDMA_IRQ2);
	}
#endif
	else{
        QDMA_ERR("invalid cmd! \r\n");
    }

	return count ;
}

#if 0
static int qdma_bm_memxs_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return 0;
}


static int qdma_bm_memxs_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64], cmd[32];
	uint channelIdx=0, queueIdx=0;
	
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %d %d", cmd, &channelIdx, &queueIdx) ;
			
	if(!strcmp(cmd, "queueinfo")||!strcmp(cmd, "channelinfo")) {
		if(channelIdx > CONFIG_QDMA_CHANNEL|| channelIdx < 0 || queueIdx>=CONFIG_QDMA_QUEUE|| queueIdx < 0){
			QDMA_ERR("invalid input param: channelIdx %d! , queueIdx: %d \r\n", channelIdx, queueIdx);
			return -EINVAL;
	    }else
		    dump_channel_or_queue_Info(channelIdx, queueIdx, cmd);
	}else if(!strcmp(cmd, "dscplist")){
		if(channelIdx >= CONFIG_QDMA_CHANNEL|| channelIdx < 0 || queueIdx>=CONFIG_QDMA_QUEUE|| queueIdx < 0){
			QDMA_ERR("invalid input  channelIdx: %d, queueIdx : %d!  \r\n", channelIdx, queueIdx);
			return -EINVAL;
		}else
			dump_queue_dscplist(channelIdx, queueIdx);
	}
	return count;
	
		
}
#endif


__IMEM ecnt_ret_val ecnt_qdma_hook(struct ecnt_data *in_data)
{
	struct ECNT_QDMA_Data *qdma_data = (struct ECNT_QDMA_Data *)in_data ;
	//ulong flags=0 ;	
	
	if(qdma_data->function_id >= QDMA_FUNCTION_MAX_NUM) {
		QDMA_ERR("qdma_data->function_id is %d, exceed max number: %d", qdma_data->function_id, QDMA_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}
	if(qdma_operation[qdma_data->function_id] == NULL){
		QDMA_MSG(DBG_ST, "ecnt_qdma_hook: NULL, function_id=0x%x\n", qdma_data->function_id);
		return ECNT_CONTINUE;
	}else{
		qdma_data->retValue = qdma_operation[qdma_data->function_id](qdma_data);
	}
	
	return ECNT_CONTINUE;
}

/*****************************************************************
*****************************************************************/
#if defined(TCSUPPORT_CPU_EN7581)
__IMEM static irqreturn_t qdma_isr_1(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0, intStatus3=0, intStatus4=0, intStatus5=0, ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
#ifdef CONFIG_SUPPORT_SELF_TEST
	struct ECNT_QDMA_Data qdma_data;
	QDMA_DMA_DSCP_T *pTxDscp=NULL, *pRxDscp=NULL ;
	uint idx=0 ;
#endif

	/*status1: bit16~bit31=RX-COHERENT ; bit8~15=TX-COHERENT */
			 /*bit5:IRQ2_FULL ; bit4: IRQ2_INT; bit3:FWD_DSCP_LOW ; bit2:FWD_DSCP_EMPTY; bit1:IRQ_FULL ; bit0: IRQ_INT*/
	/*status2: bit16~bit31=RX-N0DSCP;bit0~15:RX_DONE*/
	/*status3: bit16~bit31=RX16-31-NODSCP, bit0~15: RX16-31-DONE*/
	/*status4: bit16~bit31=RX16-31-COHERENT*/
	/*status5: bit8~bit31=TX8-31-COHERENT*/
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ;
	intStatus3 = qdmaGetIntStatus3(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3) ;
	intStatus4 = qdmaGetIntStatus4(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4) ;
	intStatus5 = qdmaGetIntStatus5(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5) ;

	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT1-1: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT1-2: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT1-3: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus3(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT1-4: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus4(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT1-5: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus5(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5)) ;

	/*Handle RX-DONE or TX-DONE INT*/
	if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus2 & INT1_STATUS_RX_DONE_LOW_16_RING) || (intStatus3 & INT1_STATUS_RX_DONE_HIGH_16_RING)) 
	{
		if((intStatus2 & INT1_STATUS_RX_DONE_LOW_16_RING) || (intStatus3 & INT1_STATUS_RX_DONE_HIGH_16_RING)) 
		{
			qdmaClearIntStatus2(base, (intStatus2 & INT1_STATUS_RX_DONE_LOW_16_RING)) ; 
			qdmaClearIntStatus3(base, (intStatus3 & INT1_STATUS_RX_DONE_HIGH_16_RING)) ;
			
			if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
				if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
					gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
				}
			}

			for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
			{
				if( ((ringIdx<16) && ((intStatus2 & INT1_STATUS_RX_DONE_LOW_16_RING) & (INT_STATUS_RX0_DONE<<ringIdx))) || \
					((ringIdx>=16) && ((intStatus3 & INT1_STATUS_RX_DONE_HIGH_16_RING) & (INT_STATUS_RX16_DONE<<(ringIdx-16)))))
				{
					gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
					QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
				}
			}
		}
		
		if(intStatus1 & INT_STATUS_IRQ_INT)
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}
	}

#if defined(CONFIG_SUPPORT_SELF_TEST)
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif
	
	if((intStatus1 & INT_STATUS_QDMA_FAULT)||(intStatus2 & INT1_STATUS_NO_RX_CPU_DSCP_LOW_16_RING)|| \
		(intStatus3 & INT1_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING)) 
	{
		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{	
#ifdef CONFIG_SUPPORT_SELF_TEST
			QDMA_ERR("IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
#else
			QDMA_MSG(DBG_WARN,"IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_MSG(DBG_WARN,"IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
#endif
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	
		if(intStatus1 & INT_STATUS_HWFWD_DSCP_EMPTY)
		{
			QDMA_ERR("IRQ: NO HW Link manger descriptor available.\n") ;
			qdmaClearIntStatus1(base, INT_STATUS_HWFWD_DSCP_EMPTY) ;
			if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
				gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_TX_CROWDED) ;
			}
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_data.qdma_private.cnt = 0 ;
			printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*	for csr2 test */
#endif
			gpQdmaPriv->counters.intNoLinkDscp++ ;
		}
		
		if(intStatus1 & INT_STATUS_HWFWD_DSCP_LOW) 
		{
			QDMA_ERR("IRQ: Low HW Link manger descriptor available.\n") ;
			qdmaClearIntStatus1(base, INT_STATUS_HWFWD_DSCP_LOW) ;
			if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
				gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_TX_CROWDED) ;
			}
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_data.qdma_private.cnt = 0 ;
			printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*	for csr2 test */
#endif
			gpQdmaPriv->counters.intLowLinkDscp++ ;
		}

		if((intStatus2 & INT1_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) || (intStatus3 & INT1_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING)) 
		{
			QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
			qdmaClearIntStatus2(base, INT1_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) ;
			qdmaClearIntStatus3(base, INT1_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) ;
			
			for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ ) 
			{
				if(((ringIdx<16) && ((intStatus2 & INT1_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx))) || \
					((ringIdx>=16) && ((intStatus3 & INT1_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) & (INT_STATUS_NO_RX16_CPU_DSCP<<(ringIdx-16)))))
				{
#ifdef CONFIG_SUPPORT_SELF_TEST
					qdma_data.qdma_private.cnt = 0 ;
					printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*	for csr1 test */
#else
					if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
					{
						/* need inform to high layer driver */ ;
						if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
							gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
						} 
					}
#endif
					gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
					QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
				}
			}
		}

#if defined(TCSUPPORT_QDMA_VERIFY)
		if(intStatus1 & INT_STATUS_CNT_OVER_FLOW)
		{
			/*debug: need to changed to DBG_MSG, and add handle function*/
			QDMA_MSG(DBG_ERR, "FlowCnt: there is cnt overflow.\n") ;
			{
				uint i,j ;
				struct ECNT_QDMA_Data qdma_data_tmp;
				QDMA_FlowCntValueGet_T flowCntValueGet;
				QDMA_FlowCntValueClear_T flowCntValueClear;
				uint cntMax[CONFIG_FLOWCNT_MAX_DROUP_NUM]={CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM,CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM};

				for( j=0; j<CONFIG_FLOWCNT_MAX_DROUP_NUM; j++ )
				{
					for( i=0; i<cntMax[j]; i++ )
					{
						flowCntValueGet.grpIdx = j;
						flowCntValueGet.cntIdx = i;
						qdma_data_tmp.qdma_private.flowCntValueGet=&flowCntValueGet;
						qdma_get_flow_cntr_value(&qdma_data_tmp);
						if((flowCntValueGet.byteHigh & 0x80000000) >0)
						{
							printk("grp-%d,cnt-%d, byte overflow\n",j,i);
							printk("byte-L = %.8X, byte-H = %.8X\n",flowCntValueGet.byteLow,flowCntValueGet.byteHigh);
							printk("clear byte\n");
							flowCntValueClear.clearMode = FLOWCNT_CLEAR_BYTE;
							flowCntValueClear.grpIdx=j;
							flowCntValueClear.cntIdx=i;
							qdma_data_tmp.qdma_private.flowCntValueClear=&flowCntValueClear;
							qdma_clear_flow_cntr_value(&qdma_data_tmp);
						}
						if((flowCntValueGet.pktCnt & 0x80000000) >0)
						{
							printk("grp-%d,cnt-%d, packet overflow\n",j,i);
							printk("cnt = %.8X\n",flowCntValueGet.pktCnt);
							printk("clear packet\n");
							flowCntValueClear.clearMode = FLOWCNT_CLEAR_PKT;
							flowCntValueClear.grpIdx=j;
							flowCntValueClear.cntIdx=i;
							qdma_data_tmp.qdma_private.flowCntValueClear=&flowCntValueClear;
							qdma_clear_flow_cntr_value(&qdma_data_tmp);
						}
					}
				}
			}
			qdmaClearIntStatus1(base, INT_STATUS_CNT_OVER_FLOW) ;
		}
#endif

	}

	if((intStatus1 & (INT1_STATUS_RX_COHERENT_LOW_16_RING |INT1_STATUS_TX_COHERENT_LOW_8_RING)) || \
		(intStatus4 & INT1_STATUS_RX_COHERENT_HIGH_16_RING) || \
		(intStatus5 & INT1_STATUS_TX_COHERENT_HIGH_24_RING)) 
	{
		/*for RX COHERENT*/
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if(((ringIdx < 16) && ((intStatus1&INT1_STATUS_RX_COHERENT_LOW_16_RING) & (INT_STATUS_RX0_COHERENT<<ringIdx))) || \
				((ringIdx >= 16) && ((intStatus4&INT1_STATUS_RX_COHERENT_HIGH_16_RING) & (INT_STATUS_RX16_COHERENT<<(ringIdx-16))))) 
			{
#ifdef CONFIG_SUPPORT_SELF_TEST
				qdmaDisableRxDma(base) ;
				idx = qdmaGetRxDmaIdx(base, ringIdx) ;
				if(idx>=0 && idx<gpQdmaPriv->rxDscpNum[ringIdx]) {
					pRxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->rxBaseAddr[ringIdx] + idx)->dscpPtr ;
					pRxDscp->ctrl.done = 0 ;
				}
				qdmaEnableRxDma(base) ;
#else
				QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;
#endif
				if(ringIdx < 16)
					qdmaClearIntStatus1(base, (INT_STATUS_RX0_COHERENT<<ringIdx) ) ;
				if(ringIdx >= 16)
					qdmaClearIntStatus4(base, (INT_STATUS_RX16_COHERENT<<(ringIdx-16)) ) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}

		/*for TX COHERENT*/
		for( ringIdx = 0 ; ringIdx < TX_RING_NUM ; ringIdx++ )
		{
			if( ((ringIdx < 8) && ((intStatus1&INT1_STATUS_TX_COHERENT_LOW_8_RING) & (INT_STATUS_TX0_COHERENT<<ringIdx)) ) || \
				((ringIdx >= 8) && ((intStatus5&INT1_STATUS_TX_COHERENT_HIGH_24_RING) & (INT_STATUS_TX8_COHERENT<<(ringIdx-8)))) )
				
			{
				if(ringIdx < 8)
					qdmaClearIntStatus1(base, (INT_STATUS_TX0_COHERENT<<ringIdx)) ;
				if(ringIdx >= 8)
					qdmaClearIntStatus5(base, (INT_STATUS_TX8_COHERENT<<(ringIdx-8))) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
				qdmaDisableTxDma(base) ;
				idx = qdmaGetTxDmaIdx(base, ringIdx) ;
				if(idx>=0 && idx<gpQdmaPriv->txDscpNum[ringIdx]) {
					pTxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + idx)->dscpPtr ;
					pTxDscp->ctrl.done = 0 ;
				}
				qdmaEnableTxDma(base) ;
#else
				QDMA_ERR("IRQ: TX%d_DMA finds data coherent event when checking DONE bit, TX_DMA_IDX:%d.\n", 
					ringIdx , qdmaGetTxDmaIdx(base, ringIdx) ) ;		
#endif
				gpQdmaPriv->counters.intTxCoherent[ringIdx]++ ;
			}
		}
	}

	return IRQ_HANDLED ;
}

/*----Reserve INT2 for LRO_Driver.----*/
__IMEM static irqreturn_t qdma_isr_2(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0 , intStatus3=0, intStatus4=0, intStatus5=0, ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	BM_DevConfig_T *pQdmaDevCfg = &gpQdmaPriv->devCfg;

	QDMA_MSG(DBG_ST,"\nEnter qdma_isr2.\n");

	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE2) ;
	intStatus3 = qdmaGetIntStatus3(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE3) ;
	intStatus4 = qdmaGetIntStatus4(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE4) ;
	intStatus5 = qdmaGetIntStatus5(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE5) ;

	/*Handle RX-DONE INT*/
	if((intStatus2 & INT2_STATUS_RX_DONE_LOW_16_RING) || (intStatus3 & INT2_STATUS_RX_DONE_HIGH_16_RING)) 
	{		
		qdmaClearIntStatus2(base, (intStatus2 & INT2_STATUS_RX_DONE_LOW_16_RING)) ; 
		qdmaClearIntStatus3(base, (intStatus3 & INT2_STATUS_RX_DONE_HIGH_16_RING)) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
#if defined(QDMA_LAN)
		if(0 == offload_short_cut_handle(intStatus2))
			return IRQ_HANDLED;
#endif
#endif/*TCSUPPORT_COMPILE*/
		if(pQdmaDevCfg->flags.isRxNapi == QDMA_ENABLE) {
			if(pQdmaDevCfg->bmInt2EventCallbackFunction) {
				pQdmaDevCfg->bmInt2EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
			}
		}

		//for( ringIdx = INT2_RX_RING_START; ringIdx <= INT2_RX_RING_END ; ringIdx++ )
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{			
			if(((ringIdx<16) && ((intStatus2 & INT2_STATUS_RX_DONE_LOW_16_RING) & (INT_STATUS_RX0_DONE<<ringIdx))) || \
				((ringIdx>=16) && ((intStatus3 & INT2_STATUS_RX_DONE_HIGH_16_RING) & (INT_STATUS_RX16_DONE<<(ringIdx-16)))))
			{
				gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
			}
		}
	}
	else if((intStatus2 & INT2_STATUS_NO_RX_CPU_DSCP_LOW_16_RING)||(intStatus3 & INT2_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING))
	{
		QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;		
		qdmaClearIntStatus2(base, INT2_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) ;
		qdmaClearIntStatus3(base, INT2_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) ;

		//for( ringIdx = INT2_RX_RING_START ; ringIdx < INT2_RX_RING_END ; ringIdx++ )
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{			
			if(((ringIdx<16) && ((intStatus2 & INT2_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx))) || \
				((ringIdx>=16) && ((intStatus3 & INT2_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) & (INT_STATUS_NO_RX16_CPU_DSCP<<(ringIdx-16)))))
			{
				if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
				{
					/* need inform to high layer driver */ ;
					if(pQdmaDevCfg->bmInt2EventCallbackFunction) {
						pQdmaDevCfg->bmInt2EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
					} 
				} 
				gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
				QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
			}
		}
	}
	else if((intStatus1 & INT2_STATUS_RX_COHERENT_LOW_16_RING) || (intStatus4 & INT2_STATUS_RX_COHERENT_HIGH_16_RING)) 
	{
		qdmaClearIntStatus1(base, INT2_STATUS_RX_COHERENT_LOW_16_RING) ;
		qdmaClearIntStatus4(base, INT2_STATUS_RX_COHERENT_HIGH_16_RING) ;
		/*for RX COHERENT*/
		//for( ringIdx = INT2_RX_RING_START; ringIdx <= INT2_RX_RING_END ; ringIdx++ )
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{				
			if(((ringIdx < 16) && ((intStatus1&INT2_STATUS_RX_COHERENT_LOW_16_RING) & (INT_STATUS_RX0_COHERENT<<ringIdx))) || \
				((ringIdx >= 16) && ((intStatus4&INT2_STATUS_RX_COHERENT_HIGH_16_RING) & (INT_STATUS_RX16_COHERENT<<(ringIdx-16))))) 
			{
				QDMA_ERR("IRQ: RX%u_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}
	}
#if defined(CONFIG_SUPPORT_SELF_TEST)
	else if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus1 & INT_STATUS_IRQ_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{
			QDMA_ERR("IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	}
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	else if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif
	else{		
		QDMA_MSG(DBG_ST,"IRQ: Unexpected descriptor interrupt. intStatus1:%x, intStatus2:%x\n" , intStatus1, intStatus2) ;
	}

	return IRQ_HANDLED ;
}

/*for_2.4G_Wifi*/
__IMEM static irqreturn_t qdma_isr_3(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0, intStatus3=0, intStatus4=0, intStatus5=0, ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;

	/*status1: bit16~bit31=RX-COHERENT ; bit8~15=TX-COHERENT */
	/*bit3:FWD_DSCP_LOW ; bit2:FWD_DSCP_EMPTY; bit1:IRQ_FULL ; bit0: IRQ_INT*/
	/*status2: bit16~bit31=RX-N0DSCP;bit0~15:RX_DONE*/
	/*status3: bit16~bit31=RX16-31-NODSCP, bit0~15: RX16-31-DONE*/
	/*status4: bit16~bit31=RX16-31-COHERENT*/
	/*status5: bit8~bit31=TX8-31-COHERENT*/
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2) ;
	intStatus3 = qdmaGetIntStatus3(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE3) ;
	intStatus4 = qdmaGetIntStatus4(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE4) ;
	intStatus5 = qdmaGetIntStatus5(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE5) ;

	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT3-1: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT3-2: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT3-3: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus3(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE3)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT3-4: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus4(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE4)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT3-5: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus5(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE5)) ;

	/*Handle RX-DONE*/
	if((intStatus2 & INT3_STATUS_RX_DONE_LOW_16_RING) || (intStatus3 & INT3_STATUS_RX_DONE_HIGH_16_RING)) 
	{		
		qdmaClearIntStatus2(base, (intStatus2 & INT3_STATUS_RX_DONE_LOW_16_RING)) ; 
		qdmaClearIntStatus3(base, (intStatus3 & INT3_STATUS_RX_DONE_HIGH_16_RING)) ;
		if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
			if(gpQdmaPriv->devCfg.bmInt3EventCallbackFunction) {
				gpQdmaPriv->devCfg.bmInt3EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
			}
		}

		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{			
			if( ((ringIdx<16) && ((intStatus2 & INT3_STATUS_RX_DONE_LOW_16_RING) & (INT_STATUS_RX0_DONE<<ringIdx))) || \
				((ringIdx>=16) && ((intStatus3 & INT3_STATUS_RX_DONE_HIGH_16_RING) & (INT_STATUS_RX16_DONE<<(ringIdx-16)))))
			{
				gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
			}
		}
	}

	if((intStatus2 & INT3_STATUS_NO_RX_CPU_DSCP_LOW_16_RING)||(intStatus3 & INT3_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING)) 
	{
		QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;		
		qdmaClearIntStatus2(base, INT3_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) ;
		qdmaClearIntStatus3(base, INT3_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) ;
		
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{				
			if(((ringIdx<16) && ((intStatus2 & INT3_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx))) || \
				((ringIdx>=16) && ((intStatus3 & INT3_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) & (INT_STATUS_NO_RX16_CPU_DSCP<<(ringIdx-16)))))
			{
				if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
				{
					/* need inform to high layer driver */ ;
					if(gpQdmaPriv->devCfg.bmInt3EventCallbackFunction) {
						gpQdmaPriv->devCfg.bmInt3EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
					} 
				}
				gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
				QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
			}
		}
	}

	if((intStatus1 & INT3_STATUS_RX_COHERENT_LOW_16_RING) || (intStatus4 & INT1_STATUS_RX_COHERENT_HIGH_16_RING)) 
	{
		/*for RX COHERENT*/
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if(((ringIdx < 16) && ((intStatus1&INT3_STATUS_RX_COHERENT_LOW_16_RING) & (INT_STATUS_RX0_COHERENT<<ringIdx))) || \
				((ringIdx >= 16) && ((intStatus4&INT3_STATUS_RX_COHERENT_HIGH_16_RING) & (INT_STATUS_RX16_COHERENT<<(ringIdx-16))))) 
			{
				QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;				
				if(ringIdx < 16)
					qdmaClearIntStatus1(base, (INT_STATUS_RX0_COHERENT<<ringIdx) ) ;
				if(ringIdx >= 16)
					qdmaClearIntStatus4(base, (INT_STATUS_RX16_COHERENT<<(ringIdx-16)) ) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}

	}

#if defined(CONFIG_SUPPORT_SELF_TEST)
	if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus1 & INT_STATUS_IRQ_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{
			QDMA_ERR("IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	}

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif

	return IRQ_HANDLED ;
}

/*for_5G_Wifi*/
__IMEM static irqreturn_t qdma_isr_4(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0, intStatus3=0, intStatus4=0, intStatus5=0, ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;

	/*status1: bit16~bit31=RX-COHERENT ; bit8~15=TX-COHERENT */
	/*bit3:FWD_DSCP_LOW ; bit2:FWD_DSCP_EMPTY; bit1:IRQ_FULL ; bit0: IRQ_INT*/
	/*status2: bit16~bit31=RX-N0DSCP;bit0~15:RX_DONE*/
	/*status3: bit16~bit31=RX16-31-NODSCP, bit0~15: RX16-31-DONE*/
	/*status4: bit16~bit31=RX16-31-COHERENT*/
	/*status5: bit8~bit31=TX8-31-COHERENT*/
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2) ;
	intStatus3 = qdmaGetIntStatus3(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE3) ;
	intStatus4 = qdmaGetIntStatus4(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE4) ;
	intStatus5 = qdmaGetIntStatus5(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE5) ;

	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT4-1: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT4-2: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT4-3: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus3(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE3)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT4-4: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus4(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE4)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT4-5: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus5(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE5)) ;

	/*Handle RX-DONE*/		
	if((intStatus2 & INT4_STATUS_RX_DONE_LOW_16_RING) || (intStatus3 & INT4_STATUS_RX_DONE_HIGH_16_RING)) 
	{		
		qdmaClearIntStatus2(base, (intStatus2 & INT4_STATUS_RX_DONE_LOW_16_RING)) ; 
		qdmaClearIntStatus3(base, (intStatus3 & INT4_STATUS_RX_DONE_HIGH_16_RING)) ;
		if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
			if(gpQdmaPriv->devCfg.bmInt4EventCallbackFunction) {
				gpQdmaPriv->devCfg.bmInt4EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
			}
		}

		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{				
			if( ((ringIdx<16) && ((intStatus2 & INT4_STATUS_RX_DONE_LOW_16_RING) & (INT_STATUS_RX0_DONE<<ringIdx))) || \
				((ringIdx>=16) && ((intStatus3 & INT4_STATUS_RX_DONE_HIGH_16_RING) & (INT_STATUS_RX16_DONE<<(ringIdx-16)))))
			{
				gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
			}
		}
	}
		
	if((intStatus2 & INT4_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) || (intStatus3 & INT4_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING)) 
	{
		QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;		
		qdmaClearIntStatus2(base, INT4_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) ;
		qdmaClearIntStatus3(base, INT4_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) ;
		
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{				
			if(((ringIdx<16) && ((intStatus2 & INT4_STATUS_NO_RX_CPU_DSCP_LOW_16_RING) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx))) || \
				((ringIdx>=16) && ((intStatus3 & INT4_STATUS_NO_RX_CPU_DSCP_HIGH_16_RING) & (INT_STATUS_NO_RX16_CPU_DSCP<<(ringIdx-16)))))
			{
				if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
				{
					/* need inform to high layer driver */ ;
					if(gpQdmaPriv->devCfg.bmInt4EventCallbackFunction) {
						gpQdmaPriv->devCfg.bmInt4EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
					} 
				}
				gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
				QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
			}
		}
	}
		
	if((intStatus1 & INT4_STATUS_RX_COHERENT_LOW_16_RING) || (intStatus4 & INT4_STATUS_RX_COHERENT_HIGH_16_RING)) 
	{
		/*for RX COHERENT*/
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{				
			if(((ringIdx < 16) && ((intStatus1&INT4_STATUS_RX_COHERENT_LOW_16_RING) & (INT_STATUS_RX0_COHERENT<<ringIdx))) || \
				((ringIdx >= 16) && ((intStatus4&INT4_STATUS_RX_COHERENT_HIGH_16_RING) & (INT_STATUS_RX16_COHERENT<<(ringIdx-16))))) 
			{
				QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;				
				if(ringIdx < 16)
					qdmaClearIntStatus1(base, (INT_STATUS_RX0_COHERENT<<ringIdx) ) ;
				if(ringIdx >= 16)
					qdmaClearIntStatus4(base, (INT_STATUS_RX16_COHERENT<<(ringIdx-16)) ) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}

	}

#if defined(CONFIG_SUPPORT_SELF_TEST)
	if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus1 & INT_STATUS_IRQ_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{
			QDMA_ERR("IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	}

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif

	return IRQ_HANDLED ;
}

#elif defined(TCSUPPORT_CPU_EN7580)
__IMEM static irqreturn_t qdma_isr_1(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0, ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
#ifdef CONFIG_SUPPORT_SELF_TEST
	struct ECNT_QDMA_Data qdma_data;
	QDMA_DMA_DSCP_T *pTxDscp=NULL, *pRxDscp=NULL ;
	uint idx=0 ;
#endif

	/*status1: bit16~bit31=RX-COHERENT ; bit8~15=TX-COHERENT */
	/*bit5:IRQ2_FULL ; bit4: IRQ2_INT; bit3:FWD_DSCP_LOW ; bit2:FWD_DSCP_EMPTY; bit1:IRQ_FULL ; bit0: IRQ_INT*/
	/*status2: bit16~bit31=RX-N0DSCP;bit0~15:RX_DONE*/
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ;

	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT1-1: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT1,QDMA_INT_ENABLE1)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT1-2: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2)) ;

	/*Handle RX-DONE or TX-DONE INT*/
	if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus2 & INT1_STATUS_RX_DONE) ) 
	{
		if((intStatus2 & INT1_STATUS_RX_DONE)) 
		{
			qdmaClearIntStatus2(base, (intStatus2 & INT1_STATUS_RX_DONE)) ;
			if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
				if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
					gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
				}
			}

			for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
			{
				if( (intStatus2 & INT1_STATUS_RX_DONE) & (INT_STATUS_RX0_DONE<<ringIdx) )
				{
					gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
					QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
				}
			}
		}
		
		if(intStatus1 & INT_STATUS_IRQ_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}
	}

#if defined(CONFIG_SUPPORT_SELF_TEST)
#if SUPPORT_IRQ2
	if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif
	
	if( (intStatus1 & INT_STATUS_QDMA_FAULT) || (intStatus2 & INT1_STATUS_NO_RX_CPU_DSCP) ) 
	{
		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{
			QDMA_MSG(DBG_WARN,"IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_MSG(DBG_WARN,"IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	
		if(intStatus1 & INT_STATUS_HWFWD_DSCP_EMPTY)
		{
			QDMA_ERR("IRQ: NO HW Link manger descriptor available.\n") ;
			qdmaClearIntStatus1(base, INT_STATUS_HWFWD_DSCP_EMPTY) ;
			if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
				gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_TX_CROWDED) ;
			}
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_data.qdma_private.cnt = 0 ;
			printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*  for csr2 test */
#endif
			gpQdmaPriv->counters.intNoLinkDscp++ ;
		}
		
		if(intStatus1 & INT_STATUS_HWFWD_DSCP_LOW) 
		{
			QDMA_ERR("IRQ: Low HW Link manger descriptor available.\n") ;
			qdmaClearIntStatus1(base, INT_STATUS_HWFWD_DSCP_LOW) ;
			if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
				gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_TX_CROWDED) ;
			}
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_data.qdma_private.cnt = 0 ;
			printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*  for csr2 test */
#endif
			gpQdmaPriv->counters.intLowLinkDscp++ ;
		}

		if(intStatus2 & INT1_STATUS_NO_RX_CPU_DSCP) 
		{
			QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
			qdmaClearIntStatus2(base, INT1_STATUS_NO_RX_CPU_DSCP) ;
			
			for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
			{
				if( (intStatus2 & INT1_STATUS_NO_RX_CPU_DSCP) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx) )
				{
#ifdef CONFIG_SUPPORT_SELF_TEST
                    qdma_data.qdma_private.cnt = 0 ;
                    printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*	for csr1 test */
#else
                    if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
                    {
                        /* need inform to high layer driver */ ;
                        if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
                            gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
                        } 
			        }
#endif
					gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
					QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
				}
			}
		}

#if defined(TCSUPPORT_QDMA_VERIFY)
		if(intStatus1 & INT_STATUS_CNT_OVER_FLOW) 
		{
		    /*debug: need to changed to DBG_MSG, and add handle function*/
			QDMA_MSG(DBG_ERR, "FlowCnt: there is cnt overflow.\n") ;
            {
                uint i,j ;
                struct ECNT_QDMA_Data qdma_data_tmp;
                QDMA_FlowCntValueGet_T flowCntValueGet;
                QDMA_FlowCntValueClear_T flowCntValueClear;
                uint cntMax[CONFIG_FLOWCNT_MAX_DROUP_NUM]={CONFIG_FLOWCNT_GRP0_MAX_IDX_NUM,CONFIG_FLOWCNT_GRP1_MAX_IDX_NUM};

                for( j=0; j<CONFIG_FLOWCNT_MAX_DROUP_NUM; j++ )
                {
                    for( i=0; i<cntMax[j]; i++ )
                    {
                        flowCntValueGet.grpIdx = j;
                        flowCntValueGet.cntIdx = i;
                        qdma_data_tmp.qdma_private.flowCntValueGet=&flowCntValueGet;
                        qdma_get_flow_cntr_value(&qdma_data_tmp);
                        if((flowCntValueGet.byteHigh & 0x80000000) >0)
                        {
                            printk("grp-%d,cnt-%d, byte overflow\n",j,i);
                            printk("byte-L = %.8X, byte-H = %.8X\n",flowCntValueGet.byteLow,flowCntValueGet.byteHigh);
                            printk("clear byte\n");
                            flowCntValueClear.clearMode = FLOWCNT_CLEAR_BYTE;
                            flowCntValueClear.grpIdx=j;
                            flowCntValueClear.cntIdx=i;
                            qdma_data_tmp.qdma_private.flowCntValueClear=&flowCntValueClear;
                            qdma_clear_flow_cntr_value(&qdma_data_tmp);
                        }
                        if((flowCntValueGet.pktCnt & 0x80000000) >0)
                        {
                            printk("grp-%d,cnt-%d, packet overflow\n",j,i);
                            printk("cnt = %.8X\n",flowCntValueGet.pktCnt);
                            printk("clear packet\n");
                            flowCntValueClear.clearMode = FLOWCNT_CLEAR_PKT;
                            flowCntValueClear.grpIdx=j;
                            flowCntValueClear.cntIdx=i;
                            qdma_data_tmp.qdma_private.flowCntValueClear=&flowCntValueClear;
                            qdma_clear_flow_cntr_value(&qdma_data_tmp);
                        }
                    }
                }
            }
            qdmaClearIntStatus1(base, INT_STATUS_CNT_OVER_FLOW) ;
		}
#endif

	}

	if(intStatus1 & (INT1_STATUS_RX_COHERENT |INT1_STATUS_TX_COHERENT)) 
	{
		/*for RX COHERENT*/
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus1&INT1_STATUS_RX_COHERENT) & (INT_STATUS_RX0_COHERENT<<ringIdx) ) 
			{
#ifdef CONFIG_SUPPORT_SELF_TEST
				qdmaDisableRxDma(base) ;
				idx = qdmaGetRxDmaIdx(base, ringIdx) ;
				if(idx>=0 && idx<gpQdmaPriv->rxDscpNum[ringIdx]) {
					pRxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->rxBaseAddr[ringIdx] + idx)->dscpPtr ;
					pRxDscp->ctrl.done = 0 ;
				}
				qdmaEnableRxDma(base) ;
#else
				QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;
#endif
				qdmaClearIntStatus1(base, (INT_STATUS_RX0_COHERENT<<ringIdx) ) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}

		/*for TX COHERENT*/
		for( ringIdx = 0 ; ringIdx < TX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus1&INT1_STATUS_TX_COHERENT) & (INT_STATUS_TX0_COHERENT<<ringIdx) ) 
			{
				qdmaClearIntStatus1(base, (INT_STATUS_TX0_COHERENT<<ringIdx)) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
				qdmaDisableTxDma(base) ;
				idx = qdmaGetTxDmaIdx(base, ringIdx) ;
				if(idx>=0 && idx<gpQdmaPriv->txDscpNum[ringIdx]) {
					pTxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[ringIdx] + idx)->dscpPtr ;
					pTxDscp->ctrl.done = 0 ;
				}
				qdmaEnableTxDma(base) ;
#else
				QDMA_ERR("IRQ: TX%d_DMA finds data coherent event when checking DONE bit, TX_DMA_IDX:%d.\n", 
	                ringIdx , qdmaGetTxDmaIdx(base, ringIdx) ) ;		
#endif
				gpQdmaPriv->counters.intTxCoherent[ringIdx]++ ;
			}
		}
	}

	return IRQ_HANDLED ;
}

/*----Reserve INT2 for LRO_Driver.----*/
__IMEM static irqreturn_t qdma_isr_2(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0 , ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	BM_DevConfig_T *pQdmaDevCfg = &gpQdmaPriv->devCfg;

	QDMA_MSG(DBG_ST,"\nEnter qdma_isr2.\n");
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE2) ;

	/*Handle RX-DONE INT*/
	if((intStatus2 & INT2_STATUS_RX_DONE)) 
	{
		qdmaClearIntStatus2(base, (intStatus2 & INT2_STATUS_RX_DONE)) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)          
        if(0 == soft_dma_forward_handle(intStatus2))
            return IRQ_HANDLED;
#endif/*TCSUPPORT_COMPILE*/
		if(pQdmaDevCfg->flags.isRxNapi == QDMA_ENABLE) {
			if(pQdmaDevCfg->bmInt2EventCallbackFunction) {
				pQdmaDevCfg->bmInt2EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
			}
		}

		for( ringIdx = INT2_RX_RING_START; ringIdx <= INT2_RX_RING_END ; ringIdx++ )
		{
			if( (intStatus2 & INT2_STATUS_RX_DONE) & (INT_STATUS_RX0_DONE<<ringIdx) )
			{
				gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
			}
		}

#if defined(QDMA_LAN) && SUPPORT_FRAGMENT_REASSEMBLE
		if( (intStatus2 & INT2_STATUS_RX_DONE) & (INT_STATUS_RX0_DONE << FRAG_RX_RING_IDX) ) {
			gpQdmaPriv->counters.intRxDone[FRAG_RX_RING_IDX]++ ;
			QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , FRAG_RX_RING_IDX) ;
		}
		if( (intStatus2 & INT2_STATUS_RX_DONE) & (INT_STATUS_RX0_DONE << ASSE_RX_RING_IDX) ) {
			gpQdmaPriv->counters.intRxDone[ASSE_RX_RING_IDX]++ ;
			QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ASSE_RX_RING_IDX) ;
		}
#endif
	}
	else if(intStatus2 & INT2_STATUS_NO_RX_CPU_DSCP) 
	{
		QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
		qdmaClearIntStatus2(base, INT2_STATUS_NO_RX_CPU_DSCP) ;

		for( ringIdx = INT2_RX_RING_START ; ringIdx < INT2_RX_RING_END ; ringIdx++ )
		{
			if( (intStatus2 & INT2_STATUS_NO_RX_CPU_DSCP) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx) )
			{
				if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
				{
					/* need inform to high layer driver */ ;
					if(pQdmaDevCfg->bmInt2EventCallbackFunction) {
						pQdmaDevCfg->bmInt2EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
					} 
				} 
				gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
				QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
			}
		}
#if defined(QDMA_LAN) && SUPPORT_FRAGMENT_REASSEMBLE
		if( (intStatus2 & INT2_STATUS_NO_RX_CPU_DSCP) & (INT_STATUS_NO_RX0_CPU_DSCP << FRAG_RX_RING_IDX) )
		{
			if((gpQdmaPriv->rxStartPtr[FRAG_RX_RING_IDX] == gpQdmaPriv->rxEndPtr[FRAG_RX_RING_IDX]) ) 
			{
				/* need inform to high layer driver */ ;
				if(pQdmaDevCfg->bmInt2EventCallbackFunction) {
					pQdmaDevCfg->bmInt2EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
				} 
			} 
			gpQdmaPriv->counters.intNoRxDscp[FRAG_RX_RING_IDX]++ ;
			QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , FRAG_RX_RING_IDX) ;
		}
		if( (intStatus2 & INT2_STATUS_NO_RX_CPU_DSCP) & (INT_STATUS_NO_RX0_CPU_DSCP << ASSE_RX_RING_IDX) )
		{
			if((gpQdmaPriv->rxStartPtr[ASSE_RX_RING_IDX] == gpQdmaPriv->rxEndPtr[ASSE_RX_RING_IDX]) ) 
			{
				/* need inform to high layer driver */ ;
				if(pQdmaDevCfg->bmInt2EventCallbackFunction) {
					pQdmaDevCfg->bmInt2EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
				} 
			} 
			gpQdmaPriv->counters.intNoRxDscp[ASSE_RX_RING_IDX]++ ;
			QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ASSE_RX_RING_IDX) ;
		}
#endif
	}
	else if(intStatus1 & INT2_STATUS_RX_COHERENT ) 
	{
		qdmaClearIntStatus1(base, INT2_STATUS_RX_COHERENT) ;
		/*for RX COHERENT*/
		for( ringIdx = INT2_RX_RING_START; ringIdx <= INT2_RX_RING_END ; ringIdx++ )
		{
			if( (intStatus1&INT2_STATUS_RX_COHERENT) & (INT_STATUS_RX0_COHERENT<<ringIdx) )
			{
				QDMA_ERR("IRQ: RX%u_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}
#if defined(QDMA_LAN) && SUPPORT_FRAGMENT_REASSEMBLE
		if( (intStatus1&INT2_STATUS_RX_COHERENT) & (INT_STATUS_RX0_COHERENT << FRAG_RX_RING_IDX) )
		{
			QDMA_ERR("IRQ: RX%u_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
				, FRAG_RX_RING_IDX , qdmaGetRxDmaIdx(base, FRAG_RX_RING_IDX)) ;
			gpQdmaPriv->counters.intRxCoherent[FRAG_RX_RING_IDX]++ ;
		}
		if( (intStatus1&INT2_STATUS_RX_COHERENT) & (INT_STATUS_RX0_COHERENT << ASSE_RX_RING_IDX) )
		{
			QDMA_ERR("IRQ: RX%u_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
				, ASSE_RX_RING_IDX , qdmaGetRxDmaIdx(base, ASSE_RX_RING_IDX)) ;
			gpQdmaPriv->counters.intRxCoherent[ASSE_RX_RING_IDX]++ ;
		}
#endif
	}
#if defined(CONFIG_SUPPORT_SELF_TEST)
	else if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus1 & INT_STATUS_IRQ_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{
			QDMA_ERR("IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	}
#if SUPPORT_IRQ2
	else if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif
	else{		
		QDMA_MSG(DBG_ST,"IRQ: Unexpected descriptor interrupt. intStatus1:%x, intStatus2:%x\n" , intStatus1, intStatus2) ;
	}

	return IRQ_HANDLED ;
}

/*for_2.4G_Wifi*/
__IMEM static irqreturn_t qdma_isr_3(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0, ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;

	/*status1: bit16~bit31=RX-COHERENT ; bit8~15=TX-COHERENT */
	/*bit3:FWD_DSCP_LOW ; bit2:FWD_DSCP_EMPTY; bit1:IRQ_FULL ; bit0: IRQ_INT*/
	/*status2: bit16~bit31=RX-N0DSCP;bit0~15:RX_DONE*/
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2) ;

	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT3-1: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT3-2: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2)) ;

	/*Handle RX-DONE*/
	if((intStatus2 & INT3_STATUS_RX_DONE)) 
	{
		qdmaClearIntStatus2(base, (intStatus2 & INT3_STATUS_RX_DONE)) ;

		if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
			if(gpQdmaPriv->devCfg.bmInt3EventCallbackFunction) {
				gpQdmaPriv->devCfg.bmInt3EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
			}
		}

		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus2 & INT3_STATUS_RX_DONE) & (INT_STATUS_RX0_DONE<<ringIdx) )
			{
				gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
			}
		}
	}

	if(intStatus2 & INT3_STATUS_NO_RX_CPU_DSCP) 
	{
		QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
		qdmaClearIntStatus2(base, INT3_STATUS_NO_RX_CPU_DSCP) ;
		
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus2 & INT3_STATUS_NO_RX_CPU_DSCP) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx) )
			{
				if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
				{
					/* need inform to high layer driver */ ;
					if(gpQdmaPriv->devCfg.bmInt3EventCallbackFunction) {
						gpQdmaPriv->devCfg.bmInt3EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
					} 
				}
				gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
				QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
			}
		}
	}

	if(intStatus1 & INT3_STATUS_RX_COHERENT) 
	{
		/*for RX COHERENT*/
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus1&INT3_STATUS_RX_COHERENT) & (INT_STATUS_RX0_COHERENT<<ringIdx) ) 
			{
				QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;
				qdmaClearIntStatus1(base, (INT_STATUS_RX0_COHERENT<<ringIdx) ) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}

	}

#if defined(CONFIG_SUPPORT_SELF_TEST)
	if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus1 & INT_STATUS_IRQ_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{
			QDMA_ERR("IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	}

#if SUPPORT_IRQ2
	if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif

	return IRQ_HANDLED ;
}

/*for_5G_Wifi*/
__IMEM static irqreturn_t qdma_isr_4(int irq, void *dev_id)
{
	uint intStatus1=0, intStatus2=0, ringIdx=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;

	/*status1: bit16~bit31=RX-COHERENT ; bit8~15=TX-COHERENT */
	/*bit3:FWD_DSCP_LOW ; bit2:FWD_DSCP_EMPTY; bit1:IRQ_FULL ; bit0: IRQ_INT*/
	/*status2: bit16~bit31=RX-N0DSCP;bit0~15:RX_DONE*/
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2) ;

	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT4-1: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, INT4-2: Status:%.8x, Mask:%.8x\n", 
						qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2)) ;

	/*Handle RX-DONE*/
	if((intStatus2 & INT4_STATUS_RX_DONE)) 
	{
		qdmaClearIntStatus2(base, (intStatus2 & INT4_STATUS_RX_DONE)) ;
		if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
			if(gpQdmaPriv->devCfg.bmInt4EventCallbackFunction) {
				gpQdmaPriv->devCfg.bmInt4EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
			}
		}

		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus2 & INT4_STATUS_RX_DONE) & (INT_STATUS_RX0_DONE<<ringIdx) )
			{
				gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n" , ringIdx) ;
			}
		}
	}

	if(intStatus2 & INT4_STATUS_NO_RX_CPU_DSCP) 
	{
		QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
		qdmaClearIntStatus2(base, INT4_STATUS_NO_RX_CPU_DSCP) ;
		
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus2 & INT4_STATUS_NO_RX_CPU_DSCP) & (INT_STATUS_NO_RX0_CPU_DSCP<<ringIdx) )
			{
				if((gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]) ) 
				{
					/* need inform to high layer driver */ ;
					if(gpQdmaPriv->devCfg.bmInt4EventCallbackFunction) {
						gpQdmaPriv->devCfg.bmInt4EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
					} 
				}
				gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
				QDMA_MSG(DBG_MSG,"IRQ: NO RX%d descriptor interrupt.\n" , ringIdx) ;
			}
		}
	}

	if(intStatus1 & INT4_STATUS_RX_COHERENT) 
	{
		/*for RX COHERENT*/
		for( ringIdx = 0 ; ringIdx < RX_RING_NUM ; ringIdx++ )
		{
			if( (intStatus1&INT4_STATUS_RX_COHERENT) & (INT_STATUS_RX0_COHERENT<<ringIdx) ) 
			{
				QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n"
					, ringIdx , qdmaGetRxDmaIdx(base, ringIdx)) ;
				qdmaClearIntStatus1(base, (INT_STATUS_RX0_COHERENT<<ringIdx) ) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;
			}
		}

	}

#if defined(CONFIG_SUPPORT_SELF_TEST)
	if( (intStatus1 & INT_STATUS_IRQ_INT) || (intStatus1 & INT_STATUS_IRQ_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_INT) ;
			qdma_bm_transmit_done(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrqcnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ_FULL) 
		{
			QDMA_ERR("IRQ: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
		}
	}

#if SUPPORT_IRQ2
	if( (intStatus1 & INT_STATUS_IRQ2_INT) || (intStatus1 & INT_STATUS_IRQ2_FULL) )
	{
		if(intStatus1 & INT_STATUS_IRQ2_INT) 
		{
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_INT) ;
			qdma_bm_transmit_done_2(0) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_PON)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
			gpQdmaPriv->counters.intIrq2cnt++ ;
			QDMA_MSG(DBG_WARN, "IRQ2: TX DSCP DONE interrupt\n") ;
		}

		if(intStatus1 & INT_STATUS_IRQ2_FULL) 
		{
			QDMA_ERR("IRQ2: INT_STATUS_IRQ_FULL\n") ;
			QDMA_ERR("IRQ2: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrq2EntryLen(base)) ;
			qdmaClearIntStatus1(base, INT_STATUS_IRQ2_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done_2(0) ;
#endif
			gpQdmaPriv->counters.intIrq2Full++ ;
		}
	}
#endif
#endif

	return IRQ_HANDLED ;
}

#else /*TCSUPPORT_CPU_EN7516/27*/
__IMEM static irqreturn_t qdma_isr_1(int irq, void *dev_id)
{
	uint intStatus=0 ;
	uint intStatus1 = 0,intStatus2 = 0;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint ringIdx=0;
	
#ifdef CONFIG_SUPPORT_SELF_TEST
	struct ECNT_QDMA_Data qdma_data;
	QDMA_DMA_DSCP_T *pTxDscp=NULL, *pRxDscp=NULL ;
	uint idx=0 ;
#endif
	intStatus = qdmaGetIntStatus(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE0) ;
	intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status0:%.8x, Mask0:%.8x\n", qdmaGetIntStatus(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE0)) ;
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status1:%.8x, Mask1:%.8x\n", qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1)) ;	
	QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status2:%.8x, Mask2:%.8x\n", qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2)) ;
	if((intStatus & (INT_STATUS_TX_DONE | INT1_STATUS_RX_DONE_0_1)) || (intStatus1 & INT1_STATUS_RX_DONE_2_15) || (intStatus & INT_STATUS_RX_PKT_OVERFLOW) ) {

		if((intStatus & INT1_STATUS_RX_DONE_0_1 )|| (intStatus1 & INT1_STATUS_RX_DONE_2_15) || (intStatus & INT_STATUS_RX_PKT_OVERFLOW)) {
			qdmaClearIntStatus(base, (intStatus & INT1_STATUS_RX_DONE_0_1)) ;
			qdmaClearIntStatus1(base, (intStatus1 & INT1_STATUS_RX_DONE_2_15)) ;

			if(intStatus & INT_STATUS_RX_PKT_OVERFLOW){
			    QDMA_MSG(DBG_WARN, "ISR: INT_STATUS_RX_PKT_OVERFLOW\n") ;
			    qdmaClearIntStatus(base, INT_STATUS_RX_PKT_OVERFLOW) ;
			}
			
			if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
				if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
					gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
				}
			}

			if(intStatus & INT1_STATUS_RX0_DONE){
				gpQdmaPriv->counters.intRxDone[RING_IDX_0]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP DONE interrupt.\n") ;
			}
			if(intStatus & INT1_STATUS_RX1_DONE){
				gpQdmaPriv->counters.intRxDone[RING_IDX_1]++ ;
				QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP DONE interrupt.\n") ;
			}
 			for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
				if(intStatus1 & INT1_STATUS_RX_DONE(ringIdx)){
					gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
					QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n", ringIdx) ;
				}	
 			}
		}

		if(intStatus & INT_STATUS_TX0_DONE) {
			qdmaClearIntStatus(base, INT_STATUS_TX0_DONE) ;
			qdma_bm_transmit_done(0) ;			
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_SWQOS)
			if(sw_PKTQOS_CLEAR_STOP)
				sw_PKTQOS_CLEAR_STOP();
#endif/*TCSUPPORT_COMPILE*/

			gpQdmaPriv->counters.intTxDone[RING_IDX_0]++ ;
			QDMA_MSG(DBG_WARN, "IRQ: TX DSCP DONE interrupt\n") ;
		}
	}

	if(intStatus & INT_STATUS_EXTERNAL) {
		if(intStatus & INT_STATUS_XPON_PHY) {
			QDMA_MSG(DBG_WARN, "IRQ: External XPON PHY device interrupt\n") ;
			qdmaClearIntStatus(base, INT_STATUS_XPON_PHY) ;
			if(gpQdmaPriv->devCfg.bmXponPhyIntHandler)
				gpQdmaPriv->devCfg.bmXponPhyIntHandler() ;
		} 
		if(intStatus & INT_STATUS_EPON_MAC) {
			QDMA_MSG(DBG_WARN, "IRQ: External EPON MAC interrupt\n") ;
			qdmaClearIntStatus(base, INT_STATUS_EPON_MAC) ;
			if(gpQdmaPriv->devCfg.bmEponMacIntHandler)
				gpQdmaPriv->devCfg.bmEponMacIntHandler() ;
		} 
		if(intStatus & INT_STATUS_GPON_MAC) {
			QDMA_MSG(DBG_WARN, "IRQ: External GPON MAC interrupt\n") ;
			qdmaClearIntStatus(base, INT_STATUS_GPON_MAC) ;
			if(gpQdmaPriv->devCfg.bmGponMacIntHandler)
				gpQdmaPriv->devCfg.bmGponMacIntHandler() ;
		}
	}

	if((intStatus & INT1_STATUS_RX_DSCP_LOW_0_1) || (intStatus2 & INT1_STATUS_RX_DSCP_LOW_2_15)){
		
		qdmaClearIntStatus(base, (intStatus & INT1_STATUS_RX_DSCP_LOW_0_1)) ;
		qdmaClearIntStatus2(base, (intStatus2 & INT1_STATUS_RX_DSCP_LOW_2_15)) ;
		
		if(intStatus & INT1_STATUS_RX0_DSCP_LOW){
			QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP LOW interrupt\n") ; 		
			gpQdmaPriv->counters.intRxDscpLow[RING_IDX_0]++;
		}
		if(intStatus & INT1_STATUS_RX1_DSCP_LOW){
			QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP LOW interrupt\n") ; 		
			gpQdmaPriv->counters.intRxDscpLow[RING_IDX_1]++;
		}
		for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
			if(intStatus2 & INT1_STATUS_RX_DSCP_LOW(ringIdx)){
				QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP LOW interrupt\n", ringIdx) ;			
				gpQdmaPriv->counters.intRxDscpLow[ringIdx]++ ;
			}
		}
	}
	
	if((intStatus & INT_STATUS_QDMA_FAULT) || (intStatus & INT1_STATUS_NO_RX_CPU_DSCP_0_1) || (intStatus & INT1_STATUS_RX_COHERENT_0_1) ||
											  (intStatus1 & INT1_STATUS_NO_RX_CPU_DSCP_2_15) || (intStatus2 & INT1_STATUS_RX_COHERENT_2_15)) {

		if(intStatus & INT_STATUS_IRQ_FULL) {
			qdmaClearIntStatus(base, INT_STATUS_IRQ_FULL) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_bm_transmit_done(0) ;
#endif
			gpQdmaPriv->counters.intIrqFull++ ;
			QDMA_MSG(DBG_WARN, "IRQ: IRQ full interrupt, entryLen:%d.\n", qdmaGetIrqEntryLen(base)) ;
		}
		
		if(intStatus & INT_STATUS_RX0_COHERENT) {
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdmaDisableRxDma(base) ;

			idx = qdmaGetRxDmaIdx(base, RING_IDX_0) ;
			if(idx>=0 && idx<gpQdmaPriv->rxDscpNum[RING_IDX_0]) {
				pRxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->rxBaseAddr[RING_IDX_0] + idx)->dscpPtr ;
				pRxDscp->ctrl.done = 0 ;
			}
			qdmaEnableRxDma(base) ;
#else
			QDMA_ERR("IRQ: RX0_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_0)) ;
#endif /* CONFIG_SUPPORT_SELF_TEST */
			qdmaClearIntStatus(base, INT_STATUS_RX0_COHERENT) ;
			gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]++ ;
			QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_0, gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]) ;			

		}
		
		if(intStatus & INT_STATUS_RX1_COHERENT) {
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdmaDisableRxDma(base) ;
			idx = qdmaGetRxDmaIdx(base, RING_IDX_1) ;
			if(idx>=0 && idx<gpQdmaPriv->rxDscpNum[RING_IDX_1]) {
				pRxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->rxBaseAddr[RING_IDX_1] + idx)->dscpPtr ;
				pRxDscp->ctrl.done = 0 ;
			}
			qdmaEnableRxDma(base) ;
#else
			QDMA_ERR("IRQ: RX1_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_1)) ;
#endif /* CONFIG_SUPPORT_SELF_TEST */
			qdmaClearIntStatus(base, INT_STATUS_RX1_COHERENT) ;
			gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]++ ;
			QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_1, gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]) ;			

		}
		for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
			if(intStatus2 & INT1_STATUS_RX_COHERENT(ringIdx)){
#ifdef CONFIG_SUPPORT_SELF_TEST
				qdmaDisableRxDma(base) ;
				idx = qdmaGetRxDmaIdx(base, ringIdx) ;
				if(idx>=0 && idx<gpQdmaPriv->rxDscpNum[ringIdx]) {
					pRxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->rxBaseAddr[ringIdx] + idx)->dscpPtr ;
					pRxDscp->ctrl.done = 0 ;
				}
				qdmaEnableRxDma(base) ;
#else
				QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", ringIdx, qdmaGetRxDmaIdx(base, ringIdx)) ;
#endif /* CONFIG_SUPPORT_SELF_TEST */
				qdmaClearIntStatus2(base, INT1_STATUS_RX_COHERENT(ringIdx)) ;
				gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;				
				QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", ringIdx, gpQdmaPriv->counters.intRxCoherent[ringIdx]) ;
			}
		}
		
		if(intStatus & INT_STATUS_TX0_COHERENT) {
			qdmaClearIntStatus(base, INT_STATUS_TX0_COHERENT) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdmaDisableTxDma(base) ;
			idx = qdmaGetTxDmaIdx(base, RING_IDX_0) ;
			if(idx>=0 && idx<gpQdmaPriv->txDscpNum[RING_IDX_0]) {
				pTxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[RING_IDX_0] + idx)->dscpPtr ;
				pTxDscp->ctrl.done = 0 ;
			}
			qdmaEnableTxDma(base) ;
#else
			QDMA_ERR("IRQ: TX0_DMA finds data coherent event when checking DONE bit, TX_DMA_IDX:%d.\n", qdmaGetTxDmaIdx(base, RING_IDX_0)) ; 
#endif /* CONFIG_SUPPORT_SELF_TEST */
			gpQdmaPriv->counters.intTxCoherent[RING_IDX_0]++ ;
			QDMA_MSG(DBG_ST, "intTxCoherent[%d] : %d ;\n", RING_IDX_0, gpQdmaPriv->counters.intTxCoherent[RING_IDX_0]) ;			

		}
		
		if(intStatus & INT_STATUS_TX1_COHERENT) {
			qdmaClearIntStatus(base, INT_STATUS_TX1_COHERENT) ;
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdmaDisableTxDma(base) ;
			idx = qdmaGetTxDmaIdx(base, RING_IDX_1) ;
			if(idx>=0 && idx<gpQdmaPriv->txDscpNum[RING_IDX_1]) {
				pTxDscp = ((struct QDMA_DscpInfo_S *)gpQdmaPriv->txBaseAddr[RING_IDX_1] + idx)->dscpPtr ;
				pTxDscp->ctrl.done = 0 ;
			}
			qdmaEnableTxDma(base) ;
#else
			QDMA_ERR("IRQ: TX1_DMA finds data coherent event when checking DONE bit, TX_DMA_IDX:%d.\n", qdmaGetTxDmaIdx(base, RING_IDX_1)) ; 
#endif /* CONFIG_SUPPORT_SELF_TEST */
			gpQdmaPriv->counters.intTxCoherent[RING_IDX_1]++ ;
			QDMA_MSG(DBG_ST, "intTxCoherent[%d] : %d ;\n", RING_IDX_1, gpQdmaPriv->counters.intTxCoherent[RING_IDX_1]) ;			
		}
		
		if(intStatus & INT_STATUS_HWFWD_DSCP_EMPTY) {
			QDMA_ERR("IRQ: NO HW Link manger descriptor available.\n") ;
			qdmaClearIntStatus(base, INT_STATUS_HWFWD_DSCP_EMPTY) ;
			if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
				gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_TX_CROWDED) ;
			}
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_data.qdma_private.cnt = 0 ;
			printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*  for csr2 test */
#endif
			gpQdmaPriv->counters.intNoLinkDscp++ ;
		}
		
		if(intStatus & INT_STATUS_HWFWD_DSCP_LOW) {
			QDMA_ERR("IRQ: Low HW Link manger descriptor available.\n") ;
			qdmaClearIntStatus(base, INT_STATUS_HWFWD_DSCP_LOW) ;
			if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
				gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_TX_CROWDED) ;
			}
#ifdef CONFIG_SUPPORT_SELF_TEST
			qdma_data.qdma_private.cnt = 0 ;
			printk("receive %d \n", qdma_receive_packets(&qdma_data)) ; /*  for csr2 test */
#endif /* CONFIG_SUPPORT_SELF_TEST */
			gpQdmaPriv->counters.intLowLinkDscp++ ;
		}

		if(intStatus & INT1_STATUS_NO_RX_CPU_DSCP_0_1) {
			QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
			qdmaClearIntStatus(base, (intStatus & INT1_STATUS_NO_RX_CPU_DSCP_0_1)) ;
			if((gpQdmaPriv->rxStartPtr[0] == gpQdmaPriv->rxEndPtr[0]) || (gpQdmaPriv->rxStartPtr[1] == gpQdmaPriv->rxEndPtr[1])) {
				/* need inform to high layer driver */ ;
				if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
					gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
				} 
			} 
			if(intStatus & INT1_STATUS_NO_RX0_CPU_DSCP)
				gpQdmaPriv->counters.intNoRxDscp[0]++ ;
			if(intStatus & INT1_STATUS_NO_RX1_CPU_DSCP)
				gpQdmaPriv->counters.intNoRxDscp[1]++ ;
		}

		for( ringIdx = 2 ; ringIdx < RX_RING_NUM ; ringIdx++ ){
			if(intStatus1 & INT1_STATUS_NO_RX_CPU_DSCP(ringIdx) ) {
				QDMA_MSG(DBG_WARN, "IRQ: NO RX%d CPU descriptor available interrupt.\n", ringIdx) ;
				qdmaClearIntStatus1(base, INT_STATUS_NO_RX_CPU_DSCP(ringIdx)) ;
				if(gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]){
					if(gpQdmaPriv->devCfg.bmEventCallbackFunction) {
						gpQdmaPriv->devCfg.bmEventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
					} 
				}	
				gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
				
			}
		}
		if((intStatus & INT_STATUS_NO_TX0_CPU_DSCP) || (intStatus & INT_STATUS_NO_TX1_CPU_DSCP)) {
			qdmaClearIntStatus(base, INT_STATUS_NO_TX0_CPU_DSCP|INT_STATUS_NO_TX1_CPU_DSCP) ;
			if(intStatus & INT_STATUS_NO_TX0_CPU_DSCP){
				gpQdmaPriv->counters.intNoTxDscp[0]++ ;
				QDMA_MSG(DBG_MSG,"IRQ: NO TX0 CPU descriptor available interrupt.\n") ;
			}
			if(intStatus & INT_STATUS_NO_TX1_CPU_DSCP){
				QDMA_MSG(DBG_MSG,"IRQ: NO TX1 CPU descriptor available interrupt.\n") ;
				gpQdmaPriv->counters.intNoTxDscp[1]++ ;
			}
		}
	}
	
	return IRQ_HANDLED ;
}

/*-------------------------INT2~4: only handle rx ring INTs------------------------------------*/
__IMEM static irqreturn_t qdma_isr_2(int irq, void *dev_id){
    uint intStatus=0 ;
    uint intStatus1 = 0,intStatus2 = 0;
    uint base = gpQdmaPriv->csrBaseAddr ;
    uint ringIdx=0;
    
    QDMA_MSG(DBG_ST, "Enter QDMA IRQ 2\n");
    intStatus = qdmaGetIntStatus(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE0) ;
    intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1) ;
    intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE2) ;
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status0:%.8x, Mask0:%.8x\n", qdmaGetIntStatus(base), qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE0)) ;
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status1:%.8x, Mask1:%.8x\n", qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1)) ;    
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status2:%.8x, Mask2:%.8x\n", qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE2)) ;

    if((intStatus & INT2_STATUS_RX_DONE_0_1 )|| (intStatus1 & INT2_STATUS_RX_DONE_2_15)) {
        qdmaClearIntStatus(base, (intStatus & INT2_STATUS_RX_DONE_0_1)) ;
        qdmaClearIntStatus1(base, (intStatus1 & INT2_STATUS_RX_DONE_2_15)) ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)          
        if(0 == soft_dma_forward_handle(intStatus1))
            return IRQ_HANDLED;
#endif/*TCSUPPORT_COMPILE*/
        if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
            if(gpQdmaPriv->devCfg.bmInt2EventCallbackFunction) {
                gpQdmaPriv->devCfg.bmInt2EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
            }
        }

        if(intStatus & INT2_STATUS_RX0_DONE){
            gpQdmaPriv->counters.intRxDone[RING_IDX_0]++ ;
            QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP DONE interrupt.\n") ;
        }
        if(intStatus & INT2_STATUS_RX1_DONE){
            gpQdmaPriv->counters.intRxDone[RING_IDX_1]++ ;
            QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP DONE interrupt.\n") ;
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus1 & INT2_STATUS_RX_DONE(ringIdx)){
                gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
                QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n", ringIdx) ;
            }   
        }
    }

    if((intStatus & INT2_STATUS_RX_DSCP_LOW_0_1) || (intStatus2 & INT2_STATUS_RX_DSCP_LOW_2_15)){
        qdmaClearIntStatus(base, (intStatus & INT2_STATUS_RX_DSCP_LOW_0_1)) ;
        qdmaClearIntStatus2(base, (intStatus2 & INT2_STATUS_RX_DSCP_LOW_2_15)) ;
        
        if(intStatus & INT2_STATUS_RX0_DSCP_LOW){
            QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP LOW interrupt\n") ;       
            gpQdmaPriv->counters.intRxDscpLow[RING_IDX_0]++;
        }
        if(intStatus & INT2_STATUS_RX1_DSCP_LOW){
            QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP LOW interrupt\n") ;       
            gpQdmaPriv->counters.intRxDscpLow[RING_IDX_1]++;
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus2 & INT2_STATUS_RX_DSCP_LOW(ringIdx)){
                QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP LOW interrupt\n", ringIdx) ;         
                gpQdmaPriv->counters.intRxDscpLow[ringIdx]++ ;
            }
        }
    }
    
    if((intStatus & INT2_STATUS_NO_RX_CPU_DSCP_0_1) || (intStatus & INT2_STATUS_RX_COHERENT_0_1) ||
        (intStatus1 & INT2_STATUS_NO_RX_CPU_DSCP_2_15) || (intStatus2 & INT2_STATUS_RX_COHERENT_2_15)) {
        if(intStatus & INT2_STATUS_RX0_COHERENT) {
            QDMA_ERR("IRQ: RX0_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_0)) ;
            qdmaClearIntStatus(base, INT2_STATUS_RX0_COHERENT) ;
            gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]++ ;
            QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_0, gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]) ;            
        }
        if(intStatus & INT2_STATUS_RX1_COHERENT) {
            QDMA_ERR("IRQ: RX1_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_1)) ;
            qdmaClearIntStatus(base, INT2_STATUS_RX1_COHERENT) ;
            gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]++ ;
            QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_1, gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]) ;            
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus2 & INT2_STATUS_RX_COHERENT(ringIdx)){
                QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", ringIdx, qdmaGetRxDmaIdx(base, ringIdx)) ;
                qdmaClearIntStatus2(base, INT2_STATUS_RX_COHERENT(ringIdx)) ;
                gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;             
                QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", ringIdx, gpQdmaPriv->counters.intRxCoherent[ringIdx]) ;
            }
        }

        if(intStatus & INT2_STATUS_NO_RX_CPU_DSCP_0_1) {
            QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
            qdmaClearIntStatus(base, (intStatus & INT2_STATUS_NO_RX_CPU_DSCP_0_1)) ;
            if((gpQdmaPriv->rxStartPtr[0] == gpQdmaPriv->rxEndPtr[0]) || (gpQdmaPriv->rxStartPtr[1] == gpQdmaPriv->rxEndPtr[1])) {
                /* need inform to high layer driver */ ;
                if(gpQdmaPriv->devCfg.bmInt2EventCallbackFunction) {
                    gpQdmaPriv->devCfg.bmInt2EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
                } 
            } 
            if(intStatus & INT2_STATUS_NO_RX0_CPU_DSCP)
                gpQdmaPriv->counters.intNoRxDscp[0]++ ;
            if(intStatus & INT2_STATUS_NO_RX1_CPU_DSCP)
                gpQdmaPriv->counters.intNoRxDscp[1]++ ;
        }
        for( ringIdx = 2 ; ringIdx < RX_RING_NUM ; ringIdx++ ){
            if(intStatus1 & INT2_STATUS_NO_RX_CPU_DSCP(ringIdx) ) {
                QDMA_MSG(DBG_WARN, "IRQ: NO RX%d CPU descriptor available interrupt.\n", ringIdx) ;
                qdmaClearIntStatus1(base, INT2_STATUS_NO_RX_CPU_DSCP(ringIdx)) ;
                if(gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]){
                    if(gpQdmaPriv->devCfg.bmInt2EventCallbackFunction) {
                        gpQdmaPriv->devCfg.bmInt2EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
                    } 
                }   
                gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
            }
        }
    }
    
    return IRQ_HANDLED ;
}

/*for_2.4G_Wifi*/
__IMEM static irqreturn_t qdma_isr_3(int irq, void *dev_id){
    uint intStatus=0 ;
    uint intStatus1 = 0,intStatus2 = 0;
    uint base = gpQdmaPriv->csrBaseAddr ;
    uint ringIdx=0;

    QDMA_MSG(DBG_ST, "Enter QDMA IRQ 3\n");
    intStatus = qdmaGetIntStatus(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE0) ;
    intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1) ;
    intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2) ;
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status0:%.8x, Mask0:%.8x\n", qdmaGetIntStatus(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE0)) ;
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status1:%.8x, Mask1:%.8x\n", qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1)) ;    
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status2:%.8x, Mask2:%.8x\n", qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE2)) ;

    if((intStatus & INT3_STATUS_RX_DONE_0_1 )|| (intStatus1 & INT3_STATUS_RX_DONE_2_15)) {
        qdmaClearIntStatus(base, (intStatus & INT3_STATUS_RX_DONE_0_1)) ;
        qdmaClearIntStatus1(base, (intStatus1 & INT3_STATUS_RX_DONE_2_15)) ;
        if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
            if(gpQdmaPriv->devCfg.bmInt3EventCallbackFunction) {
                gpQdmaPriv->devCfg.bmInt3EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
            }
        }

        if(intStatus & INT3_STATUS_RX0_DONE){
            gpQdmaPriv->counters.intRxDone[RING_IDX_0]++ ;
            QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP DONE interrupt.\n") ;
        }
        if(intStatus & INT3_STATUS_RX1_DONE){
            gpQdmaPriv->counters.intRxDone[RING_IDX_1]++ ;
            QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP DONE interrupt.\n") ;
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus1 & INT3_STATUS_RX_DONE(ringIdx)){
                gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
                QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n", ringIdx) ;
            }   
        }
    }

    if((intStatus & INT3_STATUS_RX_DSCP_LOW_0_1) || (intStatus2 & INT3_STATUS_RX_DSCP_LOW_2_15)){
        qdmaClearIntStatus(base, (intStatus & INT3_STATUS_RX_DSCP_LOW_0_1)) ;
        qdmaClearIntStatus2(base, (intStatus2 & INT3_STATUS_RX_DSCP_LOW_2_15)) ;

        if(intStatus & INT3_STATUS_RX0_DSCP_LOW){
            QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP LOW interrupt\n") ;       
            gpQdmaPriv->counters.intRxDscpLow[RING_IDX_0]++;
        }
        if(intStatus & INT3_STATUS_RX1_DSCP_LOW){
            QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP LOW interrupt\n") ;       
            gpQdmaPriv->counters.intRxDscpLow[RING_IDX_1]++;
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus2 & INT3_STATUS_RX_DSCP_LOW(ringIdx)){
                QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP LOW interrupt\n", ringIdx) ;         
                gpQdmaPriv->counters.intRxDscpLow[ringIdx]++ ;
            }
        }
    }

    if((intStatus & INT3_STATUS_NO_RX_CPU_DSCP_0_1) || (intStatus & INT3_STATUS_RX_COHERENT_0_1) ||
        (intStatus1 & INT3_STATUS_NO_RX_CPU_DSCP_2_15) || (intStatus2 & INT3_STATUS_RX_COHERENT_2_15)) {
        if(intStatus & INT3_STATUS_RX0_COHERENT) {
            QDMA_ERR("IRQ: RX0_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_0)) ;
            qdmaClearIntStatus(base, INT3_STATUS_RX0_COHERENT) ;
            gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]++ ;
            QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_0, gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]) ;            
        }
        if(intStatus & INT3_STATUS_RX1_COHERENT) {
            QDMA_ERR("IRQ: RX1_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_1)) ;
            qdmaClearIntStatus(base, INT3_STATUS_RX1_COHERENT) ;
            gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]++ ;
            QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_1, gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]) ;            
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus2 & INT3_STATUS_RX_COHERENT(ringIdx)){
                QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", ringIdx, qdmaGetRxDmaIdx(base, ringIdx)) ;
                qdmaClearIntStatus2(base, INT3_STATUS_RX_COHERENT(ringIdx)) ;
                gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;             
                QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", ringIdx, gpQdmaPriv->counters.intRxCoherent[ringIdx]) ;
            }
        }

        if(intStatus & INT3_STATUS_NO_RX_CPU_DSCP_0_1) {
            QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
            qdmaClearIntStatus(base, (intStatus & INT3_STATUS_NO_RX_CPU_DSCP_0_1)) ;
            if((gpQdmaPriv->rxStartPtr[0] == gpQdmaPriv->rxEndPtr[0]) || (gpQdmaPriv->rxStartPtr[1] == gpQdmaPriv->rxEndPtr[1])) {
                /* need inform to high layer driver */ ;
                if(gpQdmaPriv->devCfg.bmInt3EventCallbackFunction) {
                    gpQdmaPriv->devCfg.bmInt3EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
                } 
            } 
            if(intStatus & INT3_STATUS_NO_RX0_CPU_DSCP)
                gpQdmaPriv->counters.intNoRxDscp[0]++ ;
            if(intStatus & INT3_STATUS_NO_RX1_CPU_DSCP)
                gpQdmaPriv->counters.intNoRxDscp[1]++ ;
        }
        for( ringIdx = 2 ; ringIdx < RX_RING_NUM ; ringIdx++ ){
            if(intStatus1 & INT3_STATUS_NO_RX_CPU_DSCP(ringIdx) ) {
                QDMA_MSG(DBG_WARN, "IRQ: NO RX%d CPU descriptor available interrupt.\n", ringIdx) ;
                qdmaClearIntStatus1(base, INT3_STATUS_NO_RX_CPU_DSCP(ringIdx)) ;
                if(gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]){
                    if(gpQdmaPriv->devCfg.bmInt3EventCallbackFunction) {
                        gpQdmaPriv->devCfg.bmInt3EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
                    } 
                }   
                gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
            }
        }
    }

    return IRQ_HANDLED ;
}

/*for_5G_Wifi*/
__IMEM static irqreturn_t qdma_isr_4(int irq, void *dev_id){
    uint intStatus=0 ;
    uint intStatus1 = 0,intStatus2 = 0;
    uint base = gpQdmaPriv->csrBaseAddr ;
    uint ringIdx=0;

    QDMA_MSG(DBG_ST, "Enter QDMA IRQ 4\n");
    intStatus = qdmaGetIntStatus(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE0) ;
    intStatus1 = qdmaGetIntStatus1(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1) ;
    intStatus2 = qdmaGetIntStatus2(base) & qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2) ;
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status0:%.8x, Mask0:%.8x\n", qdmaGetIntStatus(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE0)) ;
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status1:%.8x, Mask1:%.8x\n", qdmaGetIntStatus1(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1)) ;    
    QDMA_MSG(DBG_WARN, "QDMA interrupt service routine is return, Status2:%.8x, Mask2:%.8x\n", qdmaGetIntStatus2(base), qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE2)) ;

    if((intStatus & INT4_STATUS_RX_DONE_0_1 )|| (intStatus1 & INT4_STATUS_RX_DONE_2_15)) {
        qdmaClearIntStatus(base, (intStatus & INT4_STATUS_RX_DONE_0_1)) ;
        qdmaClearIntStatus1(base, (intStatus1 & INT4_STATUS_RX_DONE_2_15)) ;

        if(gpQdmaPriv->devCfg.flags.isRxNapi == QDMA_ENABLE) {
            if(gpQdmaPriv->devCfg.bmInt4EventCallbackFunction) {
                gpQdmaPriv->devCfg.bmInt4EventCallbackFunction(QDMA_EVENT_RECV_PKTS) ;
            }
        }

        if(intStatus & INT4_STATUS_RX0_DONE){
            gpQdmaPriv->counters.intRxDone[RING_IDX_0]++ ;
            QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP DONE interrupt.\n") ;
        }
        if(intStatus & INT4_STATUS_RX1_DONE){
            gpQdmaPriv->counters.intRxDone[RING_IDX_1]++ ;
            QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP DONE interrupt.\n") ;
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus1 & INT4_STATUS_RX_DONE(ringIdx)){
                gpQdmaPriv->counters.intRxDone[ringIdx]++ ;
                QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP DONE interrupt.\n", ringIdx) ;
            }   
        }
    }

    if((intStatus & INT4_STATUS_RX_DSCP_LOW_0_1) || (intStatus2 & INT4_STATUS_RX_DSCP_LOW_2_15)){
        qdmaClearIntStatus(base, (intStatus & INT4_STATUS_RX_DSCP_LOW_0_1)) ;
        qdmaClearIntStatus2(base, (intStatus2 & INT4_STATUS_RX_DSCP_LOW_2_15)) ;

        if(intStatus & INT4_STATUS_RX0_DSCP_LOW){
            QDMA_MSG(DBG_WARN, "IRQ: RX0 DSCP LOW interrupt\n") ;       
            gpQdmaPriv->counters.intRxDscpLow[RING_IDX_0]++;
        }
        if(intStatus & INT4_STATUS_RX1_DSCP_LOW){
            QDMA_MSG(DBG_WARN, "IRQ: RX1 DSCP LOW interrupt\n") ;       
            gpQdmaPriv->counters.intRxDscpLow[RING_IDX_1]++;
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus2 & INT4_STATUS_RX_DSCP_LOW(ringIdx)){
                QDMA_MSG(DBG_WARN, "IRQ: RX%d DSCP LOW interrupt\n", ringIdx) ;         
                gpQdmaPriv->counters.intRxDscpLow[ringIdx]++ ;
            }
        }
    }

    if((intStatus & INT4_STATUS_NO_RX_CPU_DSCP_0_1) || (intStatus & INT4_STATUS_RX_COHERENT_0_1) ||
        (intStatus1 & INT4_STATUS_NO_RX_CPU_DSCP_2_15) || (intStatus2 & INT4_STATUS_RX_COHERENT_2_15)) {
        if(intStatus & INT4_STATUS_RX0_COHERENT) {
            QDMA_ERR("IRQ: RX0_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_0)) ;
            qdmaClearIntStatus(base, INT3_STATUS_RX0_COHERENT) ;
            gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]++ ;
            QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_0, gpQdmaPriv->counters.intRxCoherent[RING_IDX_0]) ;            
        }
        if(intStatus & INT4_STATUS_RX1_COHERENT) {
            QDMA_ERR("IRQ: RX1_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", qdmaGetRxDmaIdx(base, RING_IDX_1)) ;
            qdmaClearIntStatus(base, INT3_STATUS_RX1_COHERENT) ;
            gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]++ ;
            QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", RING_IDX_1, gpQdmaPriv->counters.intRxCoherent[RING_IDX_1]) ;            
        }
        for(ringIdx = 2; ringIdx < RX_RING_NUM; ringIdx++ ){
            if(intStatus2 & INT4_STATUS_RX_COHERENT(ringIdx)){
                QDMA_ERR("IRQ: RX%d_DMA finds data coherent event when checking DONE bit, RX_DMA_IDX:%d.\n", ringIdx, qdmaGetRxDmaIdx(base, ringIdx)) ;
                qdmaClearIntStatus2(base, INT4_STATUS_RX_COHERENT(ringIdx)) ;
                gpQdmaPriv->counters.intRxCoherent[ringIdx]++ ;             
                QDMA_MSG(DBG_ST, "intRxCoherent[%d] : %d ;\n", ringIdx, gpQdmaPriv->counters.intRxCoherent[ringIdx]) ;
            }
        }

        if(intStatus & INT4_STATUS_NO_RX_CPU_DSCP_0_1) {
            QDMA_MSG(DBG_WARN, "IRQ: NO RX CPU descriptor available interrupt.\n") ;
            qdmaClearIntStatus(base, (intStatus & INT4_STATUS_NO_RX_CPU_DSCP_0_1)) ;
            if((gpQdmaPriv->rxStartPtr[0] == gpQdmaPriv->rxEndPtr[0]) || (gpQdmaPriv->rxStartPtr[1] == gpQdmaPriv->rxEndPtr[1])) {
                /* need inform to high layer driver */ ;
                if(gpQdmaPriv->devCfg.bmInt4EventCallbackFunction) {
                    gpQdmaPriv->devCfg.bmInt4EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
                } 
            } 
            if(intStatus & INT4_STATUS_NO_RX0_CPU_DSCP)
                gpQdmaPriv->counters.intNoRxDscp[0]++ ;
            if(intStatus & INT4_STATUS_NO_RX1_CPU_DSCP)
                gpQdmaPriv->counters.intNoRxDscp[1]++ ;
        }
        for( ringIdx = 2 ; ringIdx < RX_RING_NUM ; ringIdx++ ){
            if(intStatus1 & INT4_STATUS_NO_RX_CPU_DSCP(ringIdx) ) {
                QDMA_MSG(DBG_WARN, "IRQ: NO RX%d CPU descriptor available interrupt.\n", ringIdx) ;
                qdmaClearIntStatus1(base, INT4_STATUS_NO_RX_CPU_DSCP(ringIdx)) ;
                if(gpQdmaPriv->rxStartPtr[ringIdx] == gpQdmaPriv->rxEndPtr[ringIdx]){
                    if(gpQdmaPriv->devCfg.bmInt4EventCallbackFunction) {
                        gpQdmaPriv->devCfg.bmInt4EventCallbackFunction(QDMA_EVENT_NO_RX_BUFFER) ;
                    } 
                }   
                gpQdmaPriv->counters.intNoRxDscp[ringIdx]++ ;
            }
        }
    }

    return IRQ_HANDLED ;
}
#endif

/******************************************************************************
******************************************************************************/
struct ecnt_hook_ops ecnt_driver_qdma_op = {
	.name = driver_qdma_hook_name,
	.hookfn = ecnt_qdma_hook,
	.maintype = ECNT_QDMA,
	.is_execute = 1,
	.subtype = ECNT_DRIVER_API,
	.priority = 1
};

static int qdma_init_param_parse(void)
{
	unsigned int flashQdmaInit = 0;
	unchar qdmaLanPayloadMode = 0, qdmaLanDscpMode = 0;
	unchar qdmaWanPayloadMode = 0, qdmaWanDscpMode = 0;
	/*--16K*2048, 16K*1024, 16K*512, 16K*256--*/
	uint qdmaSramBuffer[4] = {32, 16, 8, 4};
	/*--64K*2048, 64K*1024, 64K*512, 64K*256--*/
	uint qdmaDramBuffer[4] = {128, 64, 32, 16};

	/* EN7523: only dram mode, 16K dscp */
	/*--16K*2048, 16K*1024, 16K*512, 16K*256--*/
	uint qdma7523Buffer[4] = {32, 16, 8, 4};

#ifdef TCSUPPORT_CPU_ARMV8
	flashQdmaInit = get_qdmainit();
#else
	flashQdmaInit = READ_FLASH_BYTE(flash_base + 0xffb4);
#endif
	printk("flashQdmaInit = 0x%x\n", flashQdmaInit);

	/* QDMA init setting */
	/* [1:0], 0x0--qdma_lan payload size 2k, 0x1--qdma_lan 1k, 0x2--qdma_lan 512, 0x3--qdma_lan 256
	** [2], reserve, EN7523 for qdma_lan HQoS
	** [3], 0x0--qdma_lan dscp in DRAM, 0x1--qdma_lan dscp in SRAM, EN7523 fix DRAM + 16K dscp
	** [5:4], 0x0--qdma_wan payload size 2k, 0x1--qdma_wan 1k, 0x2--qdma_wan 512, 0x3--qdma_wan 256
	** [6], reserve, EN7523 for qdma_wan HQoS
	** [7], 0x0--qdma_wan dscp in DRAM, 0x1--qdma_wan dscp in SRAM, EN7523 fix DRAM + 16K dscp */
	qdmaLanPayloadMode = (flashQdmaInit & 0x3);
	qdmaLanHqosMode = (flashQdmaInit >> 2) & 0x1;
	qdmaLanDscpMode = (flashQdmaInit >> 3) & 0x1;
	qdmaWanPayloadMode = (flashQdmaInit >> 4) & 0x3;
	qdmaWanHqosMode = (flashQdmaInit >> 6) & 0x1;
	qdmaWanDscpMode = (flashQdmaInit >> 7) & 0x1;

#if defined(QDMA_LAN)
	qdmaPayloadMode = qdmaLanPayloadMode;
	qdmaDscpMode = qdmaLanDscpMode;
#else
	qdmaPayloadMode = qdmaWanPayloadMode;
	qdmaDscpMode = qdmaWanDscpMode;
#endif
	/* EN7523 fix as DRAM */
	if(isEN7523){
		qdmaDscpMode = 0;
	}

	/*calculate QDMA LAN need buffer size*/
	if(isEN7523){/*DSCP in DRAM: 16K-dscp*/
		qdmaLanBufferSize = qdma7523Buffer[qdmaLanPayloadMode];
	}else if(qdmaLanDscpMode){/*DSCP in SRAM: 16K-dscp*/
		qdmaLanBufferSize = qdmaSramBuffer[qdmaLanPayloadMode];
	}else{/*DSCP in DRAM: 64K-dscp*/
		qdmaLanBufferSize = qdmaDramBuffer[qdmaLanPayloadMode];
	}

	/*calculate QDMA WAN need buffer size*/
	if(isEN7523){/*DSCP in DRAM: 16K-dscp*/
		qdmaWanBufferSize = qdma7523Buffer[qdmaWanPayloadMode];
	}else if(qdmaWanDscpMode){/*DSCP in SRAM: 16K-dscp*/
		qdmaWanBufferSize = qdmaSramBuffer[qdmaWanPayloadMode];
	}else{/*DSCP in DRAM: 64K-dscp*/
		qdmaWanBufferSize = qdmaDramBuffer[qdmaWanPayloadMode];
	}

    #if SUPPORT_CONFIG_QDMA_DSCP_BY_MI_CONFIG
    /*
    * [27:25] qdma_lan_dscp_num_mode
    * [30:28] qdma_wan_dscp_num_mode
    * [31] qdma_dscp_num_file_config
    */
    qdmaLanDscpNumMode = (flashQdmaInit>>25)&0x7;
    qdmaWanDscpNumMode = (flashQdmaInit>>28)&0x7;
    qdmaDscpNumFileConfig = (flashQdmaInit>>31)&0x1;
	
	/* check flashQdmaInit if exist beyond Ranges*/
	if(QDMA_LAN_DSCP_NUM_MODE_MAX <= qdmaLanDscpNumMode){
		printk("Ranges error! check flashQdmaInit. qdmaLanDscpNumMode = %d, QDMA_LAN_DSCP_NUM_MODE_MAX:%d \n",qdmaLanDscpNumMode,QDMA_LAN_DSCP_NUM_MODE_MAX);
		qdmaLanDscpNumMode = 0;
	}
	if(QDMA_WAN_DSCP_NUM_MODE_MAX <= qdmaWanDscpNumMode){
		printk("Ranges error! check flashQdmaInit. qdmaWanDscpNumMode = %d, QDMA_WAN_DSCP_NUM_MODE_MAX:%d \n",qdmaWanDscpNumMode,QDMA_WAN_DSCP_NUM_MODE_MAX);
		qdmaWanDscpNumMode = 0;
	}
#endif
	return 0 ;
}

static dma_addr_t qdma_hwf_buffer_init(uint hwTotalPktSize)
{
	uint memsize = 0;
	dma_addr_t hwFwdBuffAddr = 0;
	unsigned int qdma_res;
#if defined(TCSUPPORT_DSL_PHYMODE) || (defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_EN7528))
	qdma_res = 1;
#else
	qdma_res = 16;
#endif

#ifdef TCSUPPORT_CPU_ARMV8
#if defined(QDMA_LAN)		
	hwFwdBuffAddr = ecnt_qdma_lan_get_buffer_base();
#else		
	hwFwdBuffAddr = ecnt_qdma_wan_get_buffer_base();
#endif	
	gpQdmaPriv->hwFwdBuffAddr = __va(hwFwdBuffAddr);
	printk("gpQdmaPriv->hwFwdBuffAddr = 0x%lx, hwFwdBuffAddr = 0x%lx\n", (unsigned long)gpQdmaPriv->hwFwdBuffAddr, (unsigned long)hwFwdBuffAddr);
#else
#if SUPPORT_QDMA_INIT_PARAM
	/*alloc the skb buffer*/
	memsize = GET_DRAM_SIZE;
	if(memsize >= 512)
		memsize = 448;
	/*calculate hwf buffer size*/
	if( (qdmaLanBufferSize+qdmaWanBufferSize) >= memsize ){
		QDMA_ERR("EN7580: too small DRAM..., memsize = %d, QDMA_LAN nead: %d, QDMA_WAN need: %d\n",
					memsize, qdmaLanBufferSize, qdmaWanBufferSize) ;
		return 0 ;
	}else{
#if defined(QDMA_LAN)
		memsize = memsize - qdmaLanBufferSize;
#else
		memsize = memsize - (qdmaLanBufferSize+qdmaWanBufferSize);
#endif
	}
	hwFwdBuffAddr = memsize << 20;
	gpQdmaPriv->hwFwdBuffAddr = (uint)(ioremap_nocache(hwFwdBuffAddr, hwTotalPktSize));
#else /*other CPUs TCSUPPORT_CPU_EN7516*/
#if defined(QDMA_LAN)/*EN7516: LAN most 2K-->4M*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	gpQdmaPriv->hwFwdBuffAddr = (uint)dma_alloc_coherent(NULL, hwTotalPktSize, &hwFwdBuffAddr, GFP_KERNEL) ;
#else
	gpQdmaPriv->hwFwdBuffAddr = (uint)dma_alloc_coherent(qdma_dummy_dev_ptr, hwTotalPktSize, &hwFwdBuffAddr, GFP_KERNEL) ;
#endif
	if(!gpQdmaPriv->hwFwdBuffAddr) {
		QDMA_ERR("Allocate Buffer for hardware forwarding failed.\n") ;
		return 0 ;
	}
#else	/* QDMA_WAN */
	memsize = GET_DRAM_SIZE;
	if(memsize >= 512){
			memsize = 448-qdma_res; /* always reserve 16M to QDMA HW when memsize if 512MByte */
	}else{
			memsize = memsize-qdma_res; /* we should reserve 16M to QDMA HW */
	}
	hwFwdBuffAddr = memsize << 20;
	gpQdmaPriv->hwFwdBuffAddr = (uint)(ioremap_nocache(hwFwdBuffAddr, hwTotalPktSize));
#endif
#endif
#endif

	return hwFwdBuffAddr;
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
int is_left_to_right_rx_ring(int ringIdx)
{
#if defined(TCSUPPORT_CPU_EN7580)
#if defined(QDMA_LAN)
	if(ringIdx == OFFLOAD_RX_RING)
		return 1;
#else
	if(ringIdx == OFFLOAD_RX_RING_2)
		return 1;
#endif
#endif

	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
static int qdma_bm_dscp_init(QDMA_Init_T qdmaInit)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	dma_addr_t dscpDmaAddr=0, irqDmaAddr=0, irq2DmaAddr=0,hwFwdDmaAddr=0 ;
	dma_addr_t hwFwdBuffAddr=0 ;
#ifdef TCSUPPORT_CPU_ARMV8
	struct device *dev = NULL ;
	unsigned long dscpBaseAddr=0 ;
#else
	uint dscpBaseAddr=0 ;
#endif
	uint hwTotalDscpSize=0, hwTotalPktSize=0 ;
	uint i=0, base=0;
	int ringIdx=0 ;
	uint txDscpNum[TX_RING_NUM], txDscpTotal=0 ;
	uint rxDscpNum[RX_RING_NUM], rxDscpTotal=0 ;
	uint hwDscpNum=0, hwDscpMaxNum=0, curDscpNum=0 ;
	uint irqDepth=0, irq2Depth ;
	uint hwFwdPktLen=0 ;
	int flag=0, cnt=0, rxThr=0;
#if NEED_WRITE_LKMGR_CSR_WHEN_INIT
	uint lmgrInitCfg = 0 ;
#endif

	base = gpQdmaPriv->csrBaseAddr ;
	hwDscpNum = qdmaInit.hwDscpNum ;
	hwDscpMaxNum = qdmaInit.hwDscpMaxNum ;
	irqDepth = qdmaInit.irqDepth ;
	irq2Depth = qdmaInit.irq2Depth ;
	hwFwdPktLen = qdmaInit.hwFwdPktLen ;
	gpQdmaPriv->hwFwdDscpNum = qdmaInit.hwDscpNum ;
	gpQdmaPriv->hwFwdDscpMaxNum = qdmaInit.hwDscpMaxNum ;
	gpQdmaPriv->irqDepth = qdmaInit.irqDepth ;
	gpQdmaPriv->irq2Depth = qdmaInit.irq2Depth ;
	gpQdmaPriv->hwPktSize = qdmaInit.hwFwdPktLen ;
	
	for(txDscpTotal=0, ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		gpQdmaPriv->txDscpNum[ringIdx] = qdmaInit.txDscpNum[ringIdx] ;
		txDscpNum[ringIdx] = qdmaInit.txDscpNum[ringIdx] ;
		txDscpTotal += txDscpNum[ringIdx] ;
		if(txDscpNum[ringIdx] > CONFIG_DSCP_NUM_MAX) {
			QDMA_ERR("Failed: rxDscpNum is larger than %d.\n", CONFIG_DSCP_NUM_MAX) ;
			return -EFAULT ;
		}
	}
	for(rxDscpTotal=0, ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
		gpQdmaPriv->rxDscpNum[ringIdx] = qdmaInit.rxDscpNum[ringIdx] ;
		rxDscpNum[ringIdx] = qdmaInit.rxDscpNum[ringIdx] ;
		rxDscpTotal += rxDscpNum[ringIdx] ;
		if(rxDscpNum[ringIdx] > CONFIG_DSCP_NUM_MAX) {
			QDMA_ERR("Failed: rxDscpNum is larger than %d.\n", CONFIG_DSCP_NUM_MAX) ;
			return -EFAULT ;
		}
	}
	
	if((hwDscpNum>hwDscpMaxNum) || (irqDepth>CONFIG_IRQ_NUM_MAX) || (irq2Depth>CONFIG_IRQ_NUM_MAX) || (hwFwdPktLen>CONFIG_MAX_PKT_LENS) || (hwFwdPktLen<48))
	{
		QDMA_ERR("The initial parameters are invalid.\n") ;
		return -EFAULT ;
	}
	if((hwFwdPktLen & 0x3) != 0) {
		QDMA_ERR("The max packets lens must be a multiple of 4.\n") ;
		return -EFAULT ;
	}
	
	/******************************************
	* Allocate descriptor DMA memory          *
	*******************************************/
#ifdef TCSUPPORT_CPU_ARMV8
	if( (dev=get_frame_engine_dev()) == NULL ) {
		QDMA_ERR("Get device failed.\n") ; 
		return -ENOMEM ;
	}
	dscpBaseAddr = (unsigned long)dma_alloc_coherent(dev, sizeof(QDMA_DMA_DSCP_T)*(txDscpTotal+rxDscpTotal), &dscpDmaAddr, GFP_KERNEL) ;
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	dscpBaseAddr = (uint)dma_alloc_coherent(NULL, sizeof(QDMA_DMA_DSCP_T)*(txDscpTotal+rxDscpTotal), &dscpDmaAddr, GFP_KERNEL) ;
#else
	dscpBaseAddr = (uint)dma_alloc_coherent(qdma_dummy_dev_ptr, sizeof(QDMA_DMA_DSCP_T)*(txDscpTotal+rxDscpTotal), &dscpDmaAddr, GFP_KERNEL) ;
#endif
#endif
	if(!dscpBaseAddr) {
		QDMA_ERR("Allocate memory for TX/RX DSCP failed.\n") ; 
		return -ENOMEM ;
	}

	/* Set the TX_DSCP_BASE and RX_DSCP_BASE address */
	curDscpNum = 0 ;
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		qdmaSetTxDscpBase(base, ringIdx, dscpDmaAddr + sizeof(QDMA_DMA_DSCP_T)*curDscpNum) ;
		curDscpNum += txDscpNum[ringIdx];
	}
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
		if(is_left_to_right_rx_ring(ringIdx)){
			curDscpNum += rxDscpNum[ringIdx];
			continue;
		}
#endif/*TCSUPPORT_COMPILE*/
		qdmaSetRxDscpBase(base, ringIdx, dscpDmaAddr + sizeof(QDMA_DMA_DSCP_T)*curDscpNum) ;
		curDscpNum += rxDscpNum[ringIdx];
		
		qdmaSetRxRingSize(base, ringIdx, rxDscpNum[ringIdx]);
	}
	
	/* Set the RX Ring Threhold  */
#if !defined(CONFIG_SUPPORT_SELF_TEST)
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++){
        rxThr = rxDscpNum[ringIdx] >> 3 ;
        rxThr = (rxThr <= 0) ? 1 : rxThr ;
        rxThr = (rxThr > 32) ? 32 : rxThr ;
		qdmaSetRxRingThrh(base, ringIdx, rxThr);
	}
#endif

#if defined(QDMA_LAN)
#if SUPPORT_STAG_IN_SKB
    qdmaSetPktSizeOverHeadVal(base, 0x14); 
#endif 
#endif

	/******************************************
	* Allocate memory for IRQ queue           *
	******************************************/
	if(irqDepth) {
#ifdef TCSUPPORT_CPU_ARMV8
		gpQdmaPriv->irqQueueAddr = (unsigned long)dma_alloc_coherent(dev, irqDepth<<2, &irqDmaAddr, GFP_KERNEL) ;
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
		gpQdmaPriv->irqQueueAddr = (uint)dma_alloc_coherent(NULL, irqDepth<<2, &irqDmaAddr, GFP_KERNEL) ;
#else
		gpQdmaPriv->irqQueueAddr = (uint)dma_alloc_coherent(qdma_dummy_dev_ptr, irqDepth<<2, &irqDmaAddr, GFP_KERNEL) ;
#endif
#endif
		if(!gpQdmaPriv->irqQueueAddr) {
			QDMA_ERR("Allocate memory for IRQ queue failed.\n") ;
			return -ENOMEM ;
		}
		memset((void *)gpQdmaPriv->irqQueueAddr, CONFIG_IRQ_DEF_VALUE, irqDepth<<2) ;
	
		/* Setting the IRQ queue information to QDMA register */
		qdmaSetIrqBase(base, irqDmaAddr) ;
		qdmaSetIrqDepth(base, irqDepth) ;
		qdmaSetIrqThreshold(base, QDMA_TX_THRESHOLD) ;
	}
#if SUPPORT_IRQ2
	if(irq2Depth) {
#ifdef TCSUPPORT_CPU_ARMV8
		gpQdmaPriv->irq2QueueAddr = (unsigned long)dma_alloc_coherent(dev, irq2Depth<<2, &irq2DmaAddr, GFP_KERNEL) ;
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		gpQdmaPriv->irq2QueueAddr = (uint)dma_alloc_coherent(NULL, irq2Depth<<2, &irq2DmaAddr, GFP_KERNEL) ;
#else
		gpQdmaPriv->irq2QueueAddr = (uint)dma_alloc_coherent(qdma_dummy_dev_ptr, irq2Depth<<2, &irq2DmaAddr, GFP_KERNEL) ;
#endif
#endif
		if(!gpQdmaPriv->irq2QueueAddr) {
			QDMA_ERR("Allocate memory for IRQ2 queue failed.\n") ;
			return -ENOMEM ;
		}
		memset((void *)gpQdmaPriv->irq2QueueAddr, CONFIG_IRQ_DEF_VALUE, irq2Depth<<2) ;

		/* Setting the IRQ queue information to QDMA register */
		qdmaSetIrq2Base(base, irq2DmaAddr) ;
		qdmaSetIrq2Depth(base, irq2Depth) ;
		qdmaSetIrq2Threshold(base, QDMA_TX_THRESHOLD) ;
	}
#endif

	/***************************************************
	* Allocate memory for TX/RX DSCP Information node  *
	****************************************************/
#ifdef TCSUPPORT_CPU_ARMV8
	gpQdmaPriv->dscpInfoAddr = (unsigned long)kzalloc(sizeof(struct QDMA_DscpInfo_S)*(txDscpTotal+rxDscpTotal), GFP_KERNEL) ;
#else
	gpQdmaPriv->dscpInfoAddr = (uint)kzalloc(sizeof(struct QDMA_DscpInfo_S)*(txDscpTotal+rxDscpTotal), GFP_KERNEL) ;
#endif
	if(!gpQdmaPriv->dscpInfoAddr) {
		QDMA_ERR("Alloc memory for TX/RX DSCP information node failed\n") ;
		return -ENOMEM ;
	}
	
	/* Set the TX_DSCP_BASE and RX_DSCP_BASE address */
	curDscpNum = 0 ;
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		gpQdmaPriv->txBaseAddr[ringIdx] = gpQdmaPriv->dscpInfoAddr + sizeof(struct QDMA_DscpInfo_S)*curDscpNum ;
		curDscpNum += txDscpNum[ringIdx];
	}
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
		gpQdmaPriv->rxBaseAddr[ringIdx] = gpQdmaPriv->dscpInfoAddr + sizeof(struct QDMA_DscpInfo_S)*curDscpNum ;
		curDscpNum += rxDscpNum[ringIdx];
	}
	
	/* Create unused tx descriptor link list */
	for(i=0 ; i<txDscpTotal ; i++)
	{
		diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->dscpInfoAddr + i ;
		diPtr->dscpPtr = (QDMA_DMA_DSCP_T *)dscpBaseAddr + i ;
		
		diPtr->next = NULL ;
		diPtr->dscpPtr->ctrl.done = 1 ;
#if SUPPORT_TX_DROP_BIT
		diPtr->dscpPtr->ctrl.drop_pkt = 0 ;
#endif

		for(curDscpNum=0, ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
			if((i-curDscpNum) < txDscpNum[ringIdx]) {
				diPtr->dscpIdx = i-curDscpNum;
				qdma_bm_push_tx_dscp(diPtr, ringIdx) ;				
				break;
			}
			curDscpNum += txDscpNum[ringIdx];
		}
	}

	/* Create using rx descriptor ring */
	for(i=txDscpTotal ; i<(txDscpTotal+rxDscpTotal) ; i++)
	{
		diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->dscpInfoAddr + i ;
		diPtr->dscpPtr = (QDMA_DMA_DSCP_T *)dscpBaseAddr + i ;
		
		for(curDscpNum=0, ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
			if((i-txDscpTotal-curDscpNum) < rxDscpNum[ringIdx]) {
				diPtr->dscpIdx = i-txDscpTotal-curDscpNum;
				qdma_bm_add_rx_dscp(diPtr, ringIdx) ;
				break;
			}
			curDscpNum += rxDscpNum[ringIdx];
		}
	}

	/***************************************************
	* Initialization first DSCP for Tx DMA              *
	****************************************************/
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
		diPtr = qdma_bm_pop_tx_dscp(ringIdx) ;
		if(!diPtr) {
			QDMA_ERR("There is not any free TX%d DSCP.\n", ringIdx) ; 
			return -ENOSR ;
		}
		gpQdmaPriv->txUsingPtr[ringIdx] = diPtr ;
		qdmaSetTxCpuIdx(base, ringIdx, diPtr->dscpIdx) ;
		qdmaSetTxDmaIdx(base, ringIdx, diPtr->dscpIdx) ;
	}
		
	/***************************************************
	* Initialization first DSCP for Rx DMA              *
	****************************************************/
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
		if(is_left_to_right_rx_ring(ringIdx))
			continue;
#endif/*TCSUPPORT_COMPILE*/
		diPtr = qdma_bm_get_unused_rx_dscp(ringIdx) ;
		if(diPtr == NULL) {
			QDMA_ERR("There is not any free RX%d DSCP.\n", ringIdx) ;
			return -ENOSR ;
		} 
		gpQdmaPriv->rxUsingPtr[ringIdx] = diPtr ;
		qdmaSetRxCpuIdx(base, ringIdx, diPtr->dscpIdx) ;
		qdmaSetRxDmaIdx(base, ringIdx, diPtr->dscpIdx) ;
	}
	
	/***************************************************
	* Initialization packets for Rx DMA              *
	****************************************************/
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
		if(is_left_to_right_rx_ring(ringIdx))
			continue;
#endif/*TCSUPPORT_COMPILE*/
		do {
			if(qdma_prepare_rx_buffer(ringIdx) != 0)
			{
				QDMA_ERR("Attention: qdma init rx buffer undone, please double check! \n");
				break ;
			}
		} while(qdma_has_free_rxdscp(ringIdx)) ;
	}
	
	/***************************************************
	* Initialization DSCP for hardware forwarding      *
	****************************************************/
	if(hwDscpNum) { 
		hwTotalDscpSize = sizeof(QDMA_HWFWD_DMA_DSCP_T) * hwDscpNum ;
		gpQdmaPriv->hwFwdPayloadSize = hwfwd_payload_max_size;
		hwTotalPktSize = hwfwd_payload_max_size * hwDscpNum ;		
		/*alloc the dscp buffer*/
#ifdef TCSUPPORT_CPU_ARMV8
		gpQdmaPriv->hwFwdBaseAddr = (unsigned long)dma_alloc_coherent(dev, hwTotalDscpSize, &hwFwdDmaAddr, GFP_KERNEL) ;
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
		gpQdmaPriv->hwFwdBaseAddr = (uint)dma_alloc_coherent(NULL, hwTotalDscpSize, &hwFwdDmaAddr, GFP_KERNEL) ;
#else
		gpQdmaPriv->hwFwdBaseAddr = (uint)dma_alloc_coherent(qdma_dummy_dev_ptr, hwTotalDscpSize, &hwFwdDmaAddr, GFP_KERNEL) ;
#endif
#endif
		if(!gpQdmaPriv->hwFwdBaseAddr) {
			QDMA_ERR("Allocate Descriptor for hardware forwarding failed.\n") ;
			return -ENOMEM ;
		}
		/*alloc the hwf buffer*/
		hwFwdBuffAddr = qdma_hwf_buffer_init(hwTotalPktSize);
		if(0 == hwFwdBuffAddr)
			return -ENOMEM ;
		/* set base addr register*/
		qdmaSetHwDscpBase(base, hwFwdDmaAddr) ;
		qdmaSetHwBuffBase(base, hwFwdBuffAddr) ;

		/* set qdma payload size register*/
#if SUPPORT_HWDSCP_PAYLOAD_CFG
		qdmaSetHwPayloadSize(base, qdmaPayloadMode);
#else
		qdmaSetHwPayloadSize(base, HWFWD_PAYLOAD_SIZE_2K);
#endif
		/* set qdma hwf dscp low threshold register*/
		qdmaSetHwLowThrshld(base, HWFWD_LOW_THRESHOLD);

		/* set qdma hwf lmgr init register*/
#if NEED_WRITE_LKMGR_CSR_WHEN_INIT 
		lmgrInitCfg = qdmaGetHwInitCfg(base) ;
		lmgrInitCfg = (lmgrInitCfg | LMGR_INIT_START | hwDscpNum | (qdmaDscpMode<<LMGR_DSCP_RAM_MODE_SHIFT)) ;
		qdmaSetHwInitCfg(base, lmgrInitCfg) ;
#else
		qdmaSetHwDscpNum(base, hwDscpNum);
		qdmaSetHWInitStart(base);
#endif
		
		flag = 0 ;

		cnt = 25;
		while((cnt--) > 0) {		
			mdelay(1);
			if(qdmaGetHWInitStart(base) == 0) {
				flag=1;
				break;
			}
			
		}

		if(flag == 0) {
			QDMA_ERR("hw_fwd init fail!\n") ;
			return -1;
		}
	}
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int qdma_bm_dscp_deinit(void) 
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	uint totalDscpNum=0, base=0 ;
	int i=0, ringIdx=0 ;

	base = gpQdmaPriv->csrBaseAddr ;
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++)
		totalDscpNum += gpQdmaPriv->txDscpNum[ringIdx] ;
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++)
		totalDscpNum += gpQdmaPriv->rxDscpNum[ringIdx] ;
	
	qdmaDisableTxDma(base) ;
	qdmaDisableRxDma(base) ;

	QDMA_MSG(DBG_MSG, "qdma_transmit_done\n");
	qdma_bm_transmit_done(gpQdmaPriv->irqDepth) ;

	QDMA_MSG(DBG_MSG, "qdma_recycle_packet_mode\n");
	qdma_recycle_packet_mode(QDMA_TX_INTERRUPT, 0);
	
	QDMA_MSG(DBG_MSG, "qdma_recycle_receive_buffer\n");
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++)
		qdma_bm_recycle_receive_buffer(ringIdx);

	QDMA_MSG(DBG_MSG, "qdma_recycle_transmit_buffer\n");
	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++)
		qdma_bm_recycle_transmit_buffer(ringIdx);

	if(gpQdmaPriv->dscpInfoAddr) {
		for(i=0 ; i<totalDscpNum ; i++) {
			diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->dscpInfoAddr + i ;
			
			if(diPtr->skb && diPtr->dscpPtr->pkt_addr) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
				dma_unmap_single(NULL, diPtr->dscpPtr->pkt_addr, (size_t)diPtr->dscpPtr->ctrl.pkt_len, DMA_BIDIRECTIONAL) ;
#else
				dma_unmap_single(qdma_dummy_dev_ptr, diPtr->dscpPtr->pkt_addr, (size_t)diPtr->dscpPtr->ctrl.pkt_len, DMA_BIDIRECTIONAL) ;
#endif
			}
		}

		diPtr = (struct QDMA_DscpInfo_S *)gpQdmaPriv->dscpInfoAddr ;
		if(diPtr->dscpPtr) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			dma_free_coherent(NULL, sizeof(QDMA_DMA_DSCP_T)*totalDscpNum, (void *)diPtr->dscpPtr, qdmaGetTxDscpBase(base, RING_IDX_1)) ;
#else
			dma_free_coherent(qdma_dummy_dev_ptr, sizeof(QDMA_DMA_DSCP_T)*totalDscpNum, (void *)diPtr->dscpPtr, qdmaGetTxDscpBase(base, RING_IDX_1)) ;
#endif
			diPtr->dscpPtr = NULL ;
		}
	
		kfree(diPtr) ;
		gpQdmaPriv->dscpInfoAddr = 0 ;
	}
	
	if(gpQdmaPriv->irqQueueAddr) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		dma_free_coherent(NULL, gpQdmaPriv->irqDepth<<2, (void *)gpQdmaPriv->irqQueueAddr, qdmaGetIrqBase(base)) ;
#else
		dma_free_coherent(qdma_dummy_dev_ptr, gpQdmaPriv->irqDepth<<2, (void *)gpQdmaPriv->irqQueueAddr, qdmaGetIrqBase(base)) ;
#endif
		gpQdmaPriv->irqQueueAddr = 0 ;
	}
#if SUPPORT_IRQ2
	if(gpQdmaPriv->irq2QueueAddr) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		dma_free_coherent(NULL, gpQdmaPriv->irq2Depth<<2, (void *)gpQdmaPriv->irq2QueueAddr, qdmaGetIrq2Base(base)) ;
#else
		dma_free_coherent(qdma_dummy_dev_ptr, gpQdmaPriv->irq2Depth<<2, (void *)gpQdmaPriv->irq2QueueAddr, qdmaGetIrq2Base(base)) ;
#endif
		gpQdmaPriv->irq2QueueAddr = 0 ;
	}
#endif
	
	if(gpQdmaPriv->hwFwdBaseAddr) {
		uint maxSize = sizeof(QDMA_HWFWD_DMA_DSCP_T) * qdmaGetHwDscpNum(base) ;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		dma_free_coherent(NULL, maxSize, (void *)gpQdmaPriv->hwFwdBaseAddr, qdmaGetHwDscpBase(base)) ;
#else
		dma_free_coherent(qdma_dummy_dev_ptr, maxSize, (void *)gpQdmaPriv->hwFwdBaseAddr, qdmaGetHwDscpBase(base)) ;
#endif
		gpQdmaPriv->hwFwdBaseAddr = 0 ;
	}

#if defined(TCSUPPORT_CPU_ARMV8)
	if(gpQdmaPriv->hwFwdBuffAddr) {
		uint maxSize = gpQdmaPriv->hwFwdPayloadSize * qdmaGetHwDscpNum(base) ;
		struct device *dev = NULL;
		if( (dev=get_frame_engine_dev()) == NULL ) {
			QDMA_ERR("Get device failed.\n") ; 
			return 0 ;
		}
		
		dma_free_coherent(dev, maxSize, (void *)gpQdmaPriv->hwFwdBuffAddr, qdmaGetHwBuffBase(base)) ;
		gpQdmaPriv->hwFwdBuffAddr = 0 ;
	}
#else
#if SUPPORT_REV_BUFF_IN_KERNEL
	iounmap((uint *)gpQdmaPriv->hwFwdBuffAddr) ;
	gpQdmaPriv->hwFwdBuffAddr = 0 ;
#else /*other CPUs*/
#if defined(QDMA_LAN)
	if(gpQdmaPriv->hwFwdBuffAddr) {
		uint maxSize = gpQdmaPriv->hwFwdPayloadSize * qdmaGetHwDscpNum(base) ;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		dma_free_coherent(NULL, maxSize, (void *)gpQdmaPriv->hwFwdBuffAddr, qdmaGetHwBuffBase(base)) ;
#else
		dma_free_coherent(qdma_dummy_dev_ptr, maxSize, (void *)gpQdmaPriv->hwFwdBuffAddr, qdmaGetHwBuffBase(base)) ;
#endif
		gpQdmaPriv->hwFwdBuffAddr = 0 ;
	}
#else	/* QDMA_WAN */
	iounmap((uint *)gpQdmaPriv->hwFwdBuffAddr) ;
	gpQdmaPriv->hwFwdBuffAddr = 0 ;
#endif
#endif
#endif

	return 0 ;
}

/******************************************************************************
******************************************************************************/
static void __exit qdma_module_cleanup(void) 
{
#ifdef TCSUPPORT_CPU_ARMV8
	struct device *dev = NULL ;
#endif

	qdma_dvt_deinit() ;
	
    #if defined(QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
	remove_proc_entry(mac_qos_path, NULL);
	#endif
	
	remove_proc_entry(mac_default_mask_path, NULL);

	#ifdef TCSUPPORT_QOS
	remove_proc_entry(mac_qos_flag_path, NULL);
	#endif
	
	remove_proc_entry(mac_resource_limit_path, NULL) ;

	remove_proc_entry(downstream_qos_path, NULL);
	
	remove_proc_entry(counters_path, NULL) ;
	remove_proc_entry(debug_path, NULL) ;
	remove_proc_entry(ratelimit_path, NULL) ;
	remove_proc_entry(qos_path, NULL) ;

#ifdef QDMA_LAN
	remove_proc_entry("tc3162/lan_tx", NULL) ;
#endif

#if SUPPORT_MTR_ACNT_IN_QDMA
    remove_proc_entry(meter_state_path, NULL) ;
    remove_proc_entry(acnt_state_path, NULL) ;
#endif
	qdma_pri_proc_exit();
#if defined(QDMA_LAN)
	qdma_err_drop_counters_hook = NULL;
	set_qdma_lan_qos_buffer_mode_hook = NULL;

#endif

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
#ifndef QDMA_LAN
    FE_API_DEINIT_RESOURCE_MANAGE();
#endif
#endif

#if 0
	remove_proc_entry(memxs_path, NULL) ;
#endif
	remove_proc_entry(ring_bind_path, NULL) ;

	/*QDMA Multi-INT hook deinit */
	qdma_virtual_interface_deinit();
#if defined(QDMA_LAN) && SUPPORT_FRAGMENT_REASSEMBLE
	qdma_frag_reassemble_deinit();
#endif
#if defined(TCSUPPORT_QDMA_VERIFY) && defined(QDMA_LAN)
	qdma_verify_deinit();
#endif

	/*Warn:all remove_proc_entry should be on the top of this description!*/
	remove_proc_entry(qdma_path, NULL) ;
	
	ecnt_unregister_hook(&ecnt_driver_qdma_op);
	
	QDMA_STOP_TIMER(gpQdmaPriv->fwdCfg_timer) ;

#ifdef TCSUPPORT_CPU_ARMV8
	if(gpQdmaPriv->devCfg.flags.isIsrRequest) {
		if( (dev=get_frame_engine_dev()) == NULL ) {
			QDMA_ERR("Get device failed.\n") ; 
			return ;
		}
#if defined(QDMA_LAN)
		free_irq(get_qdma_lan_irq(0), dev) ;
		free_irq(get_qdma_lan_irq(1), dev) ;
		free_irq(get_qdma_lan_irq(2), dev) ;
		free_irq(get_qdma_lan_irq(3), dev) ;
#else
		free_irq(get_qdma_wan_irq(0), dev) ;
		free_irq(get_qdma_wan_irq(1), dev) ;
		free_irq(get_qdma_wan_irq(2), dev) ;
		free_irq(get_qdma_wan_irq(3), dev) ;
#endif
	}
#else
	if(gpQdmaPriv->devCfg.flags.isIsrRequest) {
		free_irq(CONFIG_QDMA_IRQ_1, NULL) ;
		free_irq(CONFIG_QDMA_IRQ_2, NULL) ;
		free_irq(CONFIG_QDMA_IRQ_3, NULL) ;
		free_irq(CONFIG_QDMA_IRQ_4, NULL) ;
	}
#endif
	
#ifdef TCSUPPORT_DRIVER_API
	qdma_dev_destroy();
#endif

	qdma_bm_dscp_deinit() ;

	if(gpQdmaPriv->csrBaseAddr) {
		iounmap((uint *)gpQdmaPriv->csrBaseAddr) ;
		gpQdmaPriv->csrBaseAddr = 0 ;
	}

	kfree(gpQdmaPriv) ;
	gpQdmaPriv = NULL ;
	
#if defined(TCSUPPORT_TSO_ENABLE) && !defined(TCSUPPORT_TSO_V2_ENABLE)
	tso_proc_deinit();
	del_timer(&tso_qos_timer);
#endif

	return;
}
void qdma_fwd_cfg_timer_expires(unsigned long arg)
{
	uint regFoeGlbCfg = 0;
	FE_Gdma_Sel_t gdm_sel;

	#if defined(QDMA_LAN)
	gdm_sel = FE_GDM_SEL_GDMA1;
	#else
	gdm_sel = FE_GDM_SEL_GDMA2;
	#endif
	
	regFoeGlbCfg = IO_GREG(0xBFB50E00);
	
	if (regFoeGlbCfg & 0x01){
		FE_API_SET_FW_CFG(gdm_sel, FE_TYPE_OC, FE_DP_PPE);
		FE_API_SET_FW_CFG(gdm_sel, FE_TYPE_MC, FE_DP_PPE);
		FE_API_SET_FW_CFG(gdm_sel, FE_TYPE_BC, FE_DP_PPE);
	}
	else{
		FE_API_SET_FW_CFG(gdm_sel, FE_TYPE_OC, FE_DP_QDMA2_CPU);
		FE_API_SET_FW_CFG(gdm_sel, FE_TYPE_MC, FE_DP_QDMA2_CPU);
		FE_API_SET_FW_CFG(gdm_sel, FE_TYPE_BC, FE_DP_QDMA2_CPU);
	}
}

/******************************************************************************
******************************************************************************/
#if SUPPORT_MTR_ACNT_IN_QDMA 
int qdma_meter_init(void)
{
    int ret = 1, idx = 0;
    char str_tmp[10] = {0};

    /*once change ingress trtcm mode when dug, shou change this num, could add in API:generalSetTrtcmMode*/
    if(generalGetTrtcmMode(INGRESS_TRTCM) == 0){
        gpQdmaPriv->meterMgr.MaxMeterNum = GENERAL_INGRESS_INDEX_MAX;  
        gpQdmaPriv->meterMgr.MeterGroupNum = 4;
    }else{
        gpQdmaPriv->meterMgr.MaxMeterNum = GENERAL_INGRESS_INDEX_MAX>>1;
        gpQdmaPriv->meterMgr.MeterGroupNum = 2;
    }
    
    for(idx = 0; idx < METER_GROUP_NUM; idx ++)
        gpQdmaPriv->meterMgr.MeterUseState[idx] = CONFIG_METER_INIT_STATE[idx];

    if(TCSUPPORT_ACCOUNT_METER_V2_VAL){
        gpQdmaPriv->meter1Mgr.MaxMeterNum = GENERAL_INGRESS_INDEX_MAX_GRP1;  
        gpQdmaPriv->meter1Mgr.MeterGroupNum = 1;
        gpQdmaPriv->meter2Mgr.MaxMeterNum = GENERAL_INGRESS_INDEX_MAX_GRP2;  
        gpQdmaPriv->meter2Mgr.MeterGroupNum = 1;
        gpQdmaPriv->meter1Mgr.MeterUseState[0] = CONFIG_METER_GP1_INIT_STATE;
        gpQdmaPriv->meter2Mgr.MeterUseState[0] = CONFIG_METER_GP1_INIT_STATE;
    }
	
    /*set meter bind fuc name, Rxring ,IP range ,IF , QoS car*/ 
    for(idx = 0;idx < RX_RING_NUM; idx ++){
        sprintf(str_tmp, "RxRing-%2d", idx);
        strcpy(gpQdmaPriv->meterMgr.MeterUseFunc[idx], str_tmp);
    }
    return ret;
}

int qdma_acnt_init(void)
{
    gpQdmaPriv->acnt0Mgr.Acnt0UseState = CONFIG_ACNT0_INIT_STATE;
    gpQdmaPriv->acnt1Mgr.Acnt1UseState = CONFIG_ACNT1_INIT_STATE;
	if(TCSUPPORT_ACCOUNT_METER_V2_VAL){
        gpQdmaPriv->acnt2Mgr.Acnt2UseState0 = CONFIG_ACNT2_INIT_STATE;
        gpQdmaPriv->acnt2Mgr.Acnt2UseState1 = CONFIG_ACNT2_INIT_STATE;
    }
    return 0;   
}
#endif

/* support change qdma rx ring dscp num dynamically */
#if SUPPORT_CONFIG_QDMA_DSCP_BY_MI_CONFIG
/*
* format:echo l/w t/r [ring_idx] [value] > /tmp/qdmadscp.conf
*    l -- qdma_lan w -- qdma_wan
*    t -- tx dscp r --rx dscp
*    ex(set qdma lan rx ring 10 dscp num 1024): echo l r 10 1024 >/tmp/qdmadscp.conf
*/
#define QDMA_DSCP_NUM_CONF_FILE "/tmp/qdmadscp.conf"
static int qdma_parse_dscp_num_conf(void)
{
    char file_path[100] = {0};

    struct file *fp;
    	loff_t pos;
	ssize_t n;
    char buff[256] = {0};
    unsigned char qdma_type,ring_type;
    unsigned int ring_idx,dscp_num;
    char* p_end = NULL;

    sprintf(file_path, QDMA_DSCP_NUM_CONF_FILE);

    fp = filp_open(file_path, O_RDONLY, 0);    
    if( IS_ERR(fp) ){
        printk("file %s read fail \n",QDMA_DSCP_NUM_CONF_FILE);
        return 1;
    }
    fp->f_pos = 0;

    pos = fp->f_pos;
    n = kernel_read(fp, &buff, sizeof(buff),
			&pos);
	if (n != sizeof(buff)) {
		printk("read fail %d\n", n);
	}

    p_end = buff;

    while(p_end&&((*p_end == 'l')||(*p_end == 'w')))
    {
        sscanf(p_end,"%c %c %d %d",&qdma_type,&ring_type,&ring_idx,&dscp_num); 

        if(qdma_type == 'l')
        {
           #if defined(QDMA_LAN)
           if(ring_type == 'r')
            CONFIG_RX_DSCP_NUM[ring_idx] = dscp_num; 
           else if(ring_type == 't')
            CONFIG_TX_DSCP_NUM[ring_idx] = dscp_num;
           else
            continue;
           #endif
        }
        else
        {
           #ifndef defined(QDMA_LAN)
           if(ring_type == 'r')
            CONFIG_RX_DSCP_NUM[ring_idx] = dscp_num; 
           else if(ring_type == 't')
            CONFIG_TX_DSCP_NUM[ring_idx] = dscp_num;
           else
            continue; 
           #endif
        }
        p_end = strchr(p_end,'\n');
        if(p_end)
            p_end+=1;
    }
    

    filp_close(fp, NULL);    

    return 1;
}
#endif
int qdma_burstlen_init(void)
{
#if (SUPPORT_MAX_BURSTLEN_256B)
    uint base = gpQdmaPriv->csrBaseAddr ;
	/* remove rx burst_len 256 */
    //RegModifyBits(QDMA_CSR_GLB_CFG(base), 1, RX_MAX_BURST_LEN_SHIFT, 1);
    RegModifyBits(QDMA_CSR_GLB_CFG(base), 1, TX_MAX_BURST_LEN_SHIFT, 1);
#endif
	return 0;
}
/******************************************************************************
******************************************************************************/

static int __init qdma_module_init(void)
{
	int idx=0, ret=0, ringIdx=0 ;
	struct proc_dir_entry *qdma_proc=NULL;
	QDMA_Init_T qdmaInit ;
	uint _reg=0;
	struct ECNT_QDMA_Data qdma_data;
#ifdef TCSUPPORT_CPU_ARMV8
	struct device *dev = NULL ;
#endif
	
	printk("macType = %u\n", macType);
	if(macType == MAC_TYPE_ATM) {
		printk("mac type is MAC_TYPE_ATM\n");
                  _reg  = IO_GREG(0xbfb00070); 
                  _reg &= ~(0x7);
                  _reg  |= 0x3;
		IO_SREG(0xbfb00070, _reg);
                  
	} else if(macType == MAC_TYPE_PTM) {
		printk("mac type is MAC_TYPE_PTM\n");
		_reg  = IO_GREG(0xbfb00070); 
                  _reg &= ~(0x7);
                  _reg  |= 0x2;
		IO_SREG(0xbfb00070, _reg);
	}

	/* Get QDMA init parameter */
#if SUPPORT_QDMA_INIT_PARAM
	qdma_init_param_parse();
#endif

	/* Initial device private data */
	gpQdmaPriv = (QDMA_Private_T *)kzalloc(sizeof(QDMA_Private_T), GFP_KERNEL) ;
	if(gpQdmaPriv == NULL) {
		QDMA_ERR("Alloc private data memory failed\n") ;
		return -ENOMEM ;
	} 

	gpQdmaPriv->devCfg.txRecycleThreshold = QDMA_TX_THRESHOLD ;
#ifdef CONFIG_QDMA_CSR_OFFSET
	gpQdmaPriv->devCfg.txQueueTrtcmScale = QDMA_TRTCM_SCALE_128BYTE ;
	gpQdmaPriv->devCfg.gponTrtcmScale = QDMA_TRTCM_SCALE_128BYTE ;
#endif /* CONFIG_QDMA_CSR_OFFSET */
#ifdef CONFIG_DEBUG
	gpQdmaPriv->devCfg.dbgLevel = DBG_ERR ;
#endif /* CONFIG_DEBUG */
#ifdef CONFIG_SUPPORT_SELF_TEST
	gpQdmaPriv->devCfg.waitTxMaxNums = 1 ;
	gpQdmaPriv->devCfg.countDown = 1 ;
	gpQdmaPriv->devCfg.countDownRound = 0 ;
#endif /* CONFIG_SUPPORT_SELF_TEST */
	for(idx=0; idx<TX_RING_NUM; idx++)
		spin_lock_init(&gpQdmaPriv->txLock[idx]) ;

	spin_lock_init(&gpQdmaPriv->irqLock) ;
	spin_lock_init(&gpQdmaPriv->txDoneLock) ;
	
	/* Base Register remap of QDMA */
#ifdef TCSUPPORT_CPU_ARMV8
	/* reg has been mapped in kernel */
	gpQdmaPriv->csrBaseAddr = (CONFIG_QDMA_BASE_ADDR | 0xa0000000) ;
#else
	gpQdmaPriv->csrBaseAddr = (uint)(ioremap_nocache(CONFIG_QDMA_BASE_ADDR, CSR_MAP_LENGTH)) ; 
#endif

	if(!gpQdmaPriv->csrBaseAddr) {
		QDMA_ERR("ioremap the QDMA base address failed.\n") ;
		return -EFAULT ;
	}
	
	/* Initial for design and verification function */
#if SUPPORT_HWDSCP_PAYLOAD_CFG
	hwfwd_payload_max_size = CONFIG_PAYLOAD_SIZE[qdmaPayloadMode];
#else
	hwfwd_payload_max_size = CONFIG_PAYLOAD_2K_BYTE;
#endif

/* support change qdma rx ring dscp num dynamically */
#if SUPPORT_CONFIG_QDMA_DSCP_BY_MI_CONFIG
#if defined(QDMA_LAN)
    for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++)
        CONFIG_RX_DSCP_NUM[ringIdx] = qdma_lan_rx_dscp_num[qdmaLanDscpNumMode][ringIdx];
#else
    for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++)
        CONFIG_RX_DSCP_NUM[ringIdx] = qdma_wan_rx_dscp_num[qdmaWanDscpNumMode][ringIdx];
#endif
    if(qdmaDscpNumFileConfig)
        qdma_parse_dscp_num_conf();
#endif

	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++)
		qdmaInit.txDscpNum[ringIdx] = CONFIG_TX_DSCP_NUM[ringIdx];
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++)
		qdmaInit.rxDscpNum[ringIdx] = CONFIG_RX_DSCP_NUM[ringIdx];

#if SUPPORT_HWDSCP_IN_SRAM && !defined(CONFIG_SUPPORT_SELF_TEST)
	if(qdmaDscpMode == QDMA_DRAM_MODE){
		qdmaInit.hwDscpNum = CONFIG_HWFWD_DRAM_DSCP_NUM;
	}else{
		qdmaInit.hwDscpNum = CONFIG_HWFWD_SRAM_DSCP_NUM;
	}
#else
	qdmaInit.hwDscpNum = CONFIG_HWFWD_DSCP_NUM;
#endif

#if SUPPORT_HWDSCP_IN_SRAM 
	if(qdmaDscpMode == QDMA_DRAM_MODE){
		qdmaInit.hwDscpMaxNum = CONFIG_HWFWD_DRAM_DSCP_NUM_MAX;
	}else{
		qdmaInit.hwDscpMaxNum = CONFIG_HWFWD_SRAM_DSCP_NUM_MAX;
	}
#else
	qdmaInit.hwDscpMaxNum = CONFIG_HWFWD_DSCP_NUM_MAX;
#endif

	qdmaInit.irqDepth = CONFIG_IRQ_DEPTH;
	qdmaInit.irq2Depth = CONFIG_IRQ2_DEPTH;
	qdmaInit.hwFwdPktLen = CONFIG_MAX_PKT_LENS;
	if((ret = qdma_bm_dscp_init(qdmaInit)) != 0) 
	{
		QDMA_ERR("QDMA DSCP initialization failed.\n") ;
		return ret ;
	}
	
	/***************************************************
	* QDMA device initialization                       *
	****************************************************/
	if((ret = qdma_dev_init()) != 0) {
		QDMA_ERR("QDMA hardware device initialization failed.\n") ;
		return ret ;
	}
    /***************************************************
	* QDMA meter manage initialization                       *
	****************************************************/
#if SUPPORT_MTR_ACNT_IN_QDMA
    qdma_meter_init();
    qdma_acnt_init();
#endif 

	/* QDMA burstlen init function */	
	qdma_burstlen_init();
	/* Register QDMA interrupt */
#ifdef TCSUPPORT_CPU_ARMV8
	if( (dev=get_frame_engine_dev()) == NULL ) {
		QDMA_ERR("Get device failed.\n") ; 
		return -ENODEV ;
	}
#if defined(QDMA_LAN)
	if(request_irq(get_qdma_lan_irq(0), qdma_isr_1, 0, isr1_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_lan_irq(0)) ;
		return -ENODEV ;
	}
	if(request_irq(get_qdma_lan_irq(1), qdma_isr_2, 0, isr2_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_lan_irq(1)) ;
		return -ENODEV ;
	}
	if(request_irq(get_qdma_lan_irq(2), qdma_isr_3, 0, isr3_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_lan_irq(2)) ;
		return -ENODEV ;
	}
	#if !defined(TCSUPPORT_NPU_WIFI_OFFLOAD) || defined(TCSUPPORT_TPCF_HAL)
	if(request_irq(get_qdma_lan_irq(3), qdma_isr_4, 0, isr4_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_lan_irq(3)) ;
		return -ENODEV ;
	}
	#endif
#else
	if(request_irq(get_qdma_wan_irq(0), qdma_isr_1, 0, isr1_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_wan_irq(0)) ;
		return -ENODEV ;
	}
	if(request_irq(get_qdma_wan_irq(1), qdma_isr_2, 0, isr2_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_wan_irq(1)) ;
		return -ENODEV ;
	}
	if(request_irq(get_qdma_wan_irq(2), qdma_isr_3, 0, isr3_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_wan_irq(2)) ;
		return -ENODEV ;
	}
	if(request_irq(get_qdma_wan_irq(3), qdma_isr_4, 0, isr4_name, dev) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", get_qdma_wan_irq(3)) ;
		return -ENODEV ;
	}
#endif
#else
	if(request_irq(CONFIG_QDMA_IRQ_1, qdma_isr_1, 0, isr1_name, NULL) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", CONFIG_QDMA_IRQ_1) ;
		return -ENODEV ;
	}
	if(request_irq(CONFIG_QDMA_IRQ_2, qdma_isr_2, 0, isr2_name, NULL) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", CONFIG_QDMA_IRQ_2) ;
		return -ENODEV ;
	}
	if(request_irq(CONFIG_QDMA_IRQ_3, qdma_isr_3, 0, isr3_name, NULL) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", CONFIG_QDMA_IRQ_3) ;
		return -ENODEV ;
	}
	if(request_irq(CONFIG_QDMA_IRQ_4, qdma_isr_4, 0, isr4_name, NULL) != 0) {
		QDMA_ERR("Request the interrupt service routine fail, irq:%d.\n", CONFIG_QDMA_IRQ_4) ;
		return -ENODEV ;
	}
#endif

	gpQdmaPriv->devCfg.flags.isIsrRequest = 1 ;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	init_timer(&gpQdmaPriv->fwdCfg_timer) ;
	gpQdmaPriv->fwdCfg_timer.data = qdma_fwd_timer ; /* 100 ms */
	gpQdmaPriv->fwdCfg_timer.function = qdma_fwd_cfg_timer_expires ;
#else
	timer_setup(&gpQdmaPriv->fwdCfg_timer, qdma_fwd_cfg_timer_expires, 0);
	gpQdmaPriv->fwdCfg_timer.expires = jiffies + ((qdma_fwd_timer * HZ)/1000);
#endif
	/***************************************************
	* Initialization TX Mode and RX Mode              *
	****************************************************/
	qdma_receive_packet_mode(QDMA_RX_MODE);
	qdma_recycle_packet_mode(QDMA_TX_MODE, QDMA_TX_THRESHOLD);

	/*  enable QDMA Rx Drop  */
	qdma_data.qdma_private.mode = QDMA_ENABLE;
	qdma_set_rx_protect_en(&qdma_data);
	
	/*  enable QDMA Tx Drop  */
	qdma_data.qdma_private.mode = QDMA_ENABLE;
	qdma_set_tx_drop_en(&qdma_data);

	/*  enable QDMA Tx DEI Drop  */
	qdma_data.qdma_private.mode = QDMA_ENABLE;
	qdma_set_txq_dei_drop_mode(&qdma_data);

	if(ecnt_register_hook(&ecnt_driver_qdma_op)) {
		QDMA_ERR("ecnt_dev_qdma_op register fail\n");
		return -ENODEV ;
	}
#if defined(QDMA_LAN)
	wifi_eth_fast_tx_hook = qdma_bm_transmit_packet_wifi_fast_hook;
	offload_eth_fast_tx_hook = qdma_bm_transmit_packet_offload_fast_hook;
	xsi_fast_tx_hook = qdma_bm_transmit_packet_xsi_fast_hook;
#ifdef TCSUPPORT_TEST_VWTEST
	wifi_eth_fast_tx_ecnt_hook = qdma_bm_transmit_packet_wifi_fast_ecnt_hook;
#endif
	qdma_err_drop_counters_hook = qdma_err_drop_counters;
	qdma_get_txring_unused_dscp_cnt_hook =qdma_get_txring_unused_dscp_cnt;
	set_qdma_lan_qos_buffer_mode_hook = set_qdma_lan_qos_buffer_mode;
#else
	/*qdma2 will receive the pkt*/
	get_tr471_rx_msg_hook = qdma_get_tr471_rxmsg;
	
#endif

#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
	qdma_read_vip_info();
#endif

	/* Initial proc file node */
	proc_mkdir(qdma_path, NULL);
	qdma_proc = create_proc_entry(counters_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_bm_counters_read_proc ;
		qdma_proc->write_proc = qdma_bm_counters_write_proc ;
	}
	qdma_proc = create_proc_entry(debug_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_bm_debug_read_proc ;
		qdma_proc->write_proc = qdma_bm_debug_write_proc ;
	}
	qdma_proc = create_proc_entry(ratelimit_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_bm_ratelimit_read_proc ;
		qdma_proc->write_proc = qdma_bm_ratelimit_write_proc ;
	}

#ifdef QDMA_LAN
	qdma_proc = create_proc_entry("tc3162/lan_tx", 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = lan_tx_read_proc ;
		qdma_proc->write_proc = lan_tx_write_proc ;
	}
#endif

#if SUPPORT_MTR_ACNT_IN_QDMA
    qdma_proc = create_proc_entry(meter_state_path, 0, NULL) ;
    if(qdma_proc) {
        qdma_proc->read_proc = qdma_bm_meter_state_read_proc;
    }

    qdma_proc = create_proc_entry(acnt_state_path, 0, NULL) ;
    if(qdma_proc) {
        qdma_proc->read_proc = qdma_bm_acnt_state_read_proc;
    }
#endif
	qdma_proc = create_proc_entry(qos_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_bm_qos_read_proc ;
		qdma_proc->write_proc = qdma_bm_qos_write_proc ;
	}

#if 0
	qdma_proc = create_proc_entry(memxs_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_bm_memxs_read_proc ;
		qdma_proc->write_proc = qdma_bm_memxs_write_proc ;
	}
#endif
	
#if defined(QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
	qdma_proc = create_proc_entry(mac_qos_path, 0, NULL);
	if (qdma_proc){
		qdma_proc->read_proc = mac_qos_read_proc;
		qdma_proc->write_proc = mac_qos_write_proc;
	}
#endif

#ifdef TCSUPPORT_QOS
	qdma_proc = create_proc_entry(mac_default_mask_path, 0, NULL);    
	if (qdma_proc){
		qdma_proc->read_proc = mac_default_queuemask_read_proc;
		qdma_proc->write_proc = mac_default_queuemask_write_proc;
	}

	qdma_proc = create_proc_entry(mac_qos_flag_path, 0, NULL);
	if (qdma_proc){
		qdma_proc->read_proc = mac_qos_flag_read_proc;
		qdma_proc->write_proc = mac_qos_flag_write_proc;
	}
#endif

	qdma_proc = create_proc_entry(mac_resource_limit_path, 0, NULL);
	if (qdma_proc)
	{
		qdma_proc->write_proc = mac_resource_limit_write_proc;
		qdma_proc->read_proc = mac_resource_limit_read_proc;
	}

	qdma_proc = create_proc_entry(downstream_qos_path, 0, NULL);
	if (qdma_proc){
		qdma_proc->read_proc = downstream_qos_read_proc;
		qdma_proc->write_proc = downstream_qos_write_proc;
	}
	qdma_proc = create_proc_entry(ring_bind_path, 0, NULL);
	if (qdma_proc)
	{
		qdma_proc->write_proc = qdma_ring_bind_write_proc;
		qdma_proc->read_proc = qdma_ring_bind_read_proc;
	}
	qdma_pri_proc_init();
#if defined(TCSUPPORT_TSO_ENABLE) && !defined(TCSUPPORT_TSO_V2_ENABLE)
	tso_proc_init();
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	init_timer(&tso_qos_timer);
#else
	timer_setup(&tso_qos_timer, tso_qos_weight_timer, 0);
#endif
#endif

	/* QDMA Verify Functions Init*/
#if defined(TCSUPPORT_QDMA_VERIFY) && defined(QDMA_LAN)
	if((ret = qdma_verify_init()) != 0) {
		QDMA_ERR("QDMA software loopback verification test initialization failed.\n") ;
		return ret ;
	}
#endif

	/* Initial for design and verification function */
	if((ret = qdma_dvt_init()) != 0) {
		QDMA_ERR("QDMA hardware loopback verification test initialization failed.\n") ;
		return ret ;
	}
	
	/***************************************************
	* QDMA Multi-INT hook initialization                       *
	****************************************************/
	if((ret = qdma_virtual_interface_init()) != 0) {
		QDMA_ERR("QDMA Multi-INT initialization failed.\n") ;
		return ret ;
	}

	/***************************************************
	* QDMA HOP for fragment & reassemble hook initialization					   *
	****************************************************/
#if defined(QDMA_LAN) && SUPPORT_FRAGMENT_REASSEMBLE
	if((ret = qdma_frag_reassemble_init()) != 0) {
		QDMA_ERR("QDMA frag & reassemble initialization failed.\n") ;
		return ret ;
	}
#endif
	
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FORWARD_LEFT_TO_RIGHT)
#if defined(QDMA_LAN)
    qdma_offload_short_cut_init();
#else

#ifdef TWO_WAY_LEFT_TO_RIGHT
#ifdef LEFT_TO_RIGHT_QDMA_WAN_TX_2
    qdma_offload_short_cut_init();
#endif
#endif
#endif
#endif/*TCSUPPORT_COMPILE*/
	return 0 ;
}

int qdma_indirect_reg_backup(void)
{
    int i=0, chnlIdx=0, queueIdx=0, ringIdx=0;
    int phyChnlMax=0, virChnlMax=0;
    unchar qosType=0;
	ushort weight[CONFIG_QDMA_QUEUE];
    uint base = gpQdmaPriv->csrBaseAddr, reg, offset;
    struct ECNT_QDMA_Data qdma_data;
    
    // save tx ratelimit value.
    for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
        txRateLimitGetParam[chnlIdx].pirParamValue = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PIR) ;
        txRateLimitGetParam[chnlIdx].pbsParamValue = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PBS) ;
        txRateLimitGetParam[chnlIdx].cirParamValue = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_CIR) ;
        txRateLimitGetParam[chnlIdx].cbsParamValue = qdmaGetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_CBS) ;
    }
    
#if !SUPPORT_MTR_ACNT_IN_QDMA
    // save rx ratelimt value.
    for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {        
        rxRateLimitGetParam[ringIdx].rateLimitValue = qdmaGetRxRateLimitConfig(ringIdx, QDMA_Rx_Rate_Limit_PIR) ;
        rxRateLimitGetParam[ringIdx].pbsParamValue = qdmaGetRxRateLimitConfig(ringIdx, QDMA_Rx_Rate_Limit_PBS) ;
    }
#endif
    
    // save tx qos value.
    for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
        qdmaGetTxQosScheduler(chnlIdx, &qosType, weight);
        txQosParam[chnlIdx].qosType = qosType;
        for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++) {
            txQosParam[chnlIdx].queue[queueIdx].weight = weight[queueIdx];
        }
    }
    
    // save virtual qos value.
    if(qdmaIsVChnlMap32Queue(base))
        virChnlMax = 4 ;
    else 
        virChnlMax = 2 ;
    phyChnlMax = CONFIG_QDMA_CHANNEL / virChnlMax ;
    for(chnlIdx=0; chnlIdx<phyChnlMax; chnlIdx++) {
        qdmaGetVirtualChannelQos(chnlIdx, virChnlMax, &qosType, weight);
        txVirQosParam[chnlIdx].qosType = qosType;
        for(queueIdx=0; queueIdx<virChnlMax; queueIdx++) {
            txVirQosParam[chnlIdx].queue[queueIdx].weight = weight[queueIdx];
        }
    }
    
    return 0;
}

int qdma_indirect_reg_restore(void)
{
    int chnlIdx=0, queueIdx=0, ringIdx=0;
    int ret=0, phyChnlMax=0, virChnlMax=0;
    unchar qosType=0;
	ushort weight[CONFIG_QDMA_QUEUE];
    uint base = gpQdmaPriv->csrBaseAddr ;
    
    // restore virtual qos value.
    if(qdmaIsVChnlMap32Queue(base))
        virChnlMax = 4 ;
    else 
        virChnlMax = 2 ;
    phyChnlMax = CONFIG_QDMA_CHANNEL / virChnlMax ;
    for(chnlIdx=0; chnlIdx<phyChnlMax; chnlIdx++) {
        for(queueIdx=0; queueIdx<virChnlMax; queueIdx++) {
            weight[queueIdx] = txVirQosParam[chnlIdx].queue[queueIdx].weight;
        }
        qdmaSetVirtualChannelQos(chnlIdx, virChnlMax, txVirQosParam[chnlIdx].qosType, weight);
    }
    
    // restore tx qos value.
    for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
        for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++) {
            weight[queueIdx] = txQosParam[chnlIdx].queue[queueIdx].weight;
        }
        qdmaSetTxQosScheduler(chnlIdx, txQosParam[chnlIdx].qosType, weight);
    }
    
#if !SUPPORT_MTR_ACNT_IN_QDMA
    // restore rx ratelimt value.
    for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
        if((ret = qdmaSetRxRateLimitConfig(ringIdx, QDMA_Rx_Rate_Limit_PIR, rxRateLimitGetParam[ringIdx].rateLimitValue)) < 0) {
            return ret ;
        }
        if((ret = qdmaSetRxRateLimitConfig(ringIdx, QDMA_Rx_Rate_Limit_PBS, rxRateLimitGetParam[ringIdx].pbsParamValue)) < 0) {
            return ret ;
        }
    }
#endif
    
    // restore tx ratelimit value.
    for(chnlIdx=0; chnlIdx<CONFIG_QDMA_CHANNEL; chnlIdx++) {
        if((ret = qdmaSetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PIR, txRateLimitGetParam[chnlIdx].pirParamValue)) < 0) {
            return ret ;
        }
        if((ret = qdmaSetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_PBS, txRateLimitGetParam[chnlIdx].pbsParamValue)) < 0) {
            return ret ;
        }
        if((ret = qdmaSetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_CIR, txRateLimitGetParam[chnlIdx].cirParamValue)) < 0) {
            return ret ;
        }
        if((ret = qdmaSetTxRateLimitConfig(chnlIdx, QDMA_Tx_Rate_Limit_CBS, txRateLimitGetParam[chnlIdx].cbsParamValue)) < 0) {
            return ret ;
        }
    }
    
    return 0 ;
}
void  qdma_get_tr471_rxmsg(int rx_ring,unsigned int * rx_byte_cnt_l,unsigned int * rx_byte_cnt_h,unsigned int * err_cnt,unsigned int * drop_cnt)
{
#if SUPPORT_TR471_HW_FUNCTION

	QDMA_DMA_DSCP_T rxDscp ;
	TR471_RX_DSCP_T *pRxD;
	memcpy(&rxDscp, gpQdmaPriv->rxEndPtr[rx_ring]->dscpPtr, sizeof(QDMA_DMA_DSCP_T)) ;

	//memcpy(&rxDscp, gpQdmaPriv->rxEndPtr[rx_ring]->dscpPtr, sizeof(QDMA_DMA_DSCP_T)) ;
	pRxD = &rxDscp;

	*rx_byte_cnt_l=pRxD->rx_byte_cnt_l;
    *rx_byte_cnt_h=pRxD->rx_byte_cnt_h;
	*err_cnt=pRxD->seq_err_cnt;
	*drop_cnt=pRxD->seq_drop_cnt;
#endif
	
	return;
}

int qdma_get_txring_unused_dscp_cnt(int ring_idx)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	int cnt=0;

	diPtr = gpQdmaPriv->txHeadPtr[ring_idx] ;
	while(diPtr) {
		diPtr = diPtr->next ;
		cnt++ ;
	}
	return cnt ;
}

void set_qdma_lan_qos_buffer_mode(int qdma_mode){
#if SUPPORT_BUFFCFG_DIFFERENT_BETWEEN_FAST_SLOW_PATH
#if defined(QDMA_LAN)
	uint base = gpQdmaPriv->csrBaseAddr ;
	if(!qdmaDscpMode && qdma_mode == 2){
		printk("Not support for this qdmaDscpMode\n");
		return;
	}
	/* 1:qos mode , 2:buffer mode*/
	if(qdma_mode==1){
		qdmaSetTxqDynTotalMaxThrh(base, BUFF_TOTAL_MAX_THRH_SRAM) ;
		qdmaSetTxqDynTotalMinThrh(base, BUFF_TOTAL_MIN_THRH_SRAM) ;
		qdmaSetTxqDynChnlMaxThrh(base, BUFF_CHN_MAX_THRH_SRAM) ;
	}else if(qdma_mode==2){
		qdmaSetTxqDynTotalMaxThrh(base, BUFF_TOTAL_MAX_THRH_SRAM_BUFFER_MODE) ;
		qdmaSetTxqDynTotalMinThrh(base, BUFF_TOTAL_MIN_THRH_SRAM_BUFFER_MODE) ;
		qdmaSetTxqDynChnlMaxThrh(base, BUFF_CHN_MAX_THRH_SRAM_BUFFER_MODE) ;
	}
#endif
#else
	if(qdma_mode==2)
		printk("Not support for this ic\n");
#endif
	return;

}
void RegModifyBits(uint32_t Addr, uint32_t Data, uint32_t Offset, uint32_t Len)
{
	uint32_t Mask = 0;
	uint32_t Value;
	uint32_t i;

	for (i = 0; i < Len; i++) {
		Mask |= 1 << (Offset + i);
	}

	Value = read_reg_word(Addr);
	Value &= ~Mask;
	Value |= (Data << Offset) & Mask;;

	write_reg_word(Addr, Value);
}

int qdma_reset(struct ECNT_QDMA_Data *qdma_data)
{
    uint base = gpQdmaPriv->csrBaseAddr ;
    int idx=0, ret=0 ;
    int ringIdx=0, i=0, j=0, intIdx=0, intEnIdx=0;
    QDMA_Init_T qdmaInit ;
    struct ECNT_QDMA_Data qdma_data_tmp;
    unsigned int qdma_int_en[QDMA_INT_NUM][QDMA_INT_ENABLE_NUM];
	BM_DevConfig_T function_hook_tmp;

	memset(&qdmaInit, 0, sizeof(QDMA_Init_T));
	
    for(i=0, intIdx=QDMA_INT1; intIdx<QDMA_INT_NUM; i++, intIdx++) {
#if !SUPPORT_INT_ENABLE_NUM_2
        intEnIdx = QDMA_INT_ENABLE0;
#else
        intEnIdx = QDMA_INT_ENABLE1;
#endif
        for(j=0; j<QDMA_INT_ENABLE_NUM; j++, intEnIdx++) {
            qdma_int_en[i][j] = qdmaGetIntMask(base, intIdx, intEnIdx);
            qdmaSetIntMask(base, intIdx, intEnIdx, 0);
        }
    }
    mdelay(1);
                      
    QDMA_STOP_TIMER(gpQdmaPriv->fwdCfg_timer);
    
#ifdef TCSUPPORT_DRIVER_API
    qdma_dev_destroy();
#endif

    qdma_bm_dscp_deinit();
    
    function_hook_tmp.bmRecvCallbackFunction  = gpQdmaPriv->devCfg.bmRecvCallbackFunction;
    function_hook_tmp.bmEventCallbackFunction = gpQdmaPriv->devCfg.bmEventCallbackFunction;
    function_hook_tmp.bmGponMacIntHandler     = gpQdmaPriv->devCfg.bmGponMacIntHandler;
    function_hook_tmp.bmEponMacIntHandler     = gpQdmaPriv->devCfg.bmEponMacIntHandler;
    function_hook_tmp.bmXponPhyIntHandler     = gpQdmaPriv->devCfg.bmXponPhyIntHandler;
    function_hook_tmp.bmInt2RecvCallbackFunction= gpQdmaPriv->devCfg.bmInt2RecvCallbackFunction;
	function_hook_tmp.bmInt2EventCallbackFunction = gpQdmaPriv->devCfg.bmInt2EventCallbackFunction;
	function_hook_tmp.bmInt3RecvCallbackFunction = gpQdmaPriv->devCfg.bmInt3RecvCallbackFunction;
	function_hook_tmp.bmInt3EventCallbackFunction = gpQdmaPriv->devCfg.bmInt3EventCallbackFunction;
	function_hook_tmp.bmInt4RecvCallbackFunction = gpQdmaPriv->devCfg.bmInt4RecvCallbackFunction;
	function_hook_tmp.bmInt4EventCallbackFunction = gpQdmaPriv->devCfg.bmInt4EventCallbackFunction;

    // qdma indirect reg save
    qdma_indirect_reg_backup();
    
    // qdma reset
    RegModifyBits(QDMA_CSR_GLB_CFG(base), 1, 23, 1);
    RegModifyBits(QDMA_CSR_LMGR_INIT_CFG(base), 1, 31, 1);
    mdelay(1);
    
    /* Get QDMA init parameter */
#if SUPPORT_QDMA_INIT_PARAM
    qdma_init_param_parse();
#endif

    if(gpQdmaPriv->csrBaseAddr) 
    {
        iounmap((uint *)gpQdmaPriv->csrBaseAddr);
        gpQdmaPriv->csrBaseAddr = 0;
    }
    kfree(gpQdmaPriv);
    gpQdmaPriv = NULL;
    
    /* Initial device private data */
    gpQdmaPriv = (QDMA_Private_T *)kzalloc(sizeof(QDMA_Private_T), GFP_KERNEL);
    if(gpQdmaPriv == NULL) 
    {
        QDMA_ERR("Alloc private data memory failed\n");
        return -ENOMEM;
    }
    
    gpQdmaPriv->devCfg.txRecycleThreshold = QDMA_TX_THRESHOLD;
#ifdef CONFIG_QDMA_CSR_OFFSET
    gpQdmaPriv->devCfg.txQueueTrtcmScale = QDMA_TRTCM_SCALE_128BYTE;
    gpQdmaPriv->devCfg.gponTrtcmScale = QDMA_TRTCM_SCALE_128BYTE;
#endif /* CONFIG_QDMA_CSR_OFFSET */
#ifdef CONFIG_DEBUG
    gpQdmaPriv->devCfg.dbgLevel = DBG_ERR;
#endif /* CONFIG_DEBUG */
#ifdef CONFIG_SUPPORT_SELF_TEST
    gpQdmaPriv->devCfg.waitTxMaxNums = 1;
    gpQdmaPriv->devCfg.countDown = 1;
    gpQdmaPriv->devCfg.countDownRound = 0;
#endif /* CONFIG_SUPPORT_SELF_TEST */
    for(idx=0; idx<TX_RING_NUM; idx++)
        spin_lock_init(&gpQdmaPriv->txLock[idx]);
    
    spin_lock_init(&gpQdmaPriv->irqLock);
    spin_lock_init(&gpQdmaPriv->txDoneLock);
    
    /* Base Register remap of QDMA */
    gpQdmaPriv->csrBaseAddr = (uint)(ioremap_nocache(CONFIG_QDMA_BASE_ADDR, CSR_MAP_LENGTH)) ; 
    if(!gpQdmaPriv->csrBaseAddr) 
    {
        QDMA_ERR("ioremap the QDMA base address failed.\n");
        return -EFAULT;
    }
    
	/* Initial for design and verification function */
#if SUPPORT_HWDSCP_PAYLOAD_CFG
	hwfwd_payload_max_size = CONFIG_PAYLOAD_SIZE[qdmaPayloadMode];
#else
	hwfwd_payload_max_size = CONFIG_PAYLOAD_2K_BYTE;
#endif

	for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++)
		qdmaInit.txDscpNum[ringIdx] = CONFIG_TX_DSCP_NUM[ringIdx];
	for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++)
		qdmaInit.rxDscpNum[ringIdx] = CONFIG_RX_DSCP_NUM[ringIdx];

#if SUPPORT_HWDSCP_IN_SRAM && !defined(CONFIG_SUPPORT_SELF_TEST)
	if(qdmaDscpMode == QDMA_DRAM_MODE){
		qdmaInit.hwDscpNum = CONFIG_HWFWD_DRAM_DSCP_NUM;
	}else{
		qdmaInit.hwDscpNum = CONFIG_HWFWD_SRAM_DSCP_NUM;
	}
#else
	qdmaInit.hwDscpNum = CONFIG_HWFWD_DSCP_NUM;
#endif

#if SUPPORT_HWDSCP_IN_SRAM
	if(qdmaDscpMode == QDMA_DRAM_MODE){
		qdmaInit.hwDscpMaxNum = CONFIG_HWFWD_DRAM_DSCP_NUM_MAX;
	}else{
		qdmaInit.hwDscpMaxNum = CONFIG_HWFWD_SRAM_DSCP_NUM_MAX;
	}
#else
	qdmaInit.hwDscpMaxNum = CONFIG_HWFWD_DSCP_NUM_MAX;
#endif

	qdmaInit.irqDepth = CONFIG_IRQ_DEPTH;
	qdmaInit.hwFwdPktLen = CONFIG_MAX_PKT_LENS;
    if((ret = qdma_bm_dscp_init(qdmaInit)) != 0) 
    {
        QDMA_ERR("QDMA DSCP initialization failed.\n");
        return EFAULT ;
    }
    
	/***************************************************
	* QDMA device initialization                       *
	****************************************************/
    if((ret = qdma_dev_init()) != 0) {
        QDMA_ERR("QDMA hardware device initialization failed.\n") ;
        return ret ;
    }
    
	/***************************************************
	* QDMA meter manage initialization                       *
	****************************************************/
#if SUPPORT_MTR_ACNT_IN_QDMA
    qdma_meter_init();
    qdma_acnt_init();
#endif 

    gpQdmaPriv->devCfg.flags.isIsrRequest = 1;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    init_timer(&gpQdmaPriv->fwdCfg_timer);
    gpQdmaPriv->fwdCfg_timer.data = qdma_fwd_timer; /* 100 ms */
    gpQdmaPriv->fwdCfg_timer.function = qdma_fwd_cfg_timer_expires;
#else
	timer_setup(&gpQdmaPriv->fwdCfg_timer, qdma_fwd_cfg_timer_expires, 0);
	gpQdmaPriv->fwdCfg_timer.expires = jiffies + ((qdma_fwd_timer * HZ)/1000);
#endif
    
    qdma_receive_packet_mode(QDMA_RX_MODE);
    qdma_recycle_packet_mode(QDMA_TX_MODE, QDMA_TX_THRESHOLD);
    
    /*  enable QDMA Rx Drop  */
    qdma_data_tmp.qdma_private.mode = QDMA_ENABLE;
    qdma_set_rx_protect_en(&qdma_data_tmp);
    
    /*  enable QDMA Tx Drop  */
    qdma_data_tmp.qdma_private.mode = QDMA_ENABLE;
    qdma_set_tx_drop_en(&qdma_data_tmp);
    
    /*  enable QDMA Tx DEI Drop  */
    qdma_data_tmp.qdma_private.mode = QDMA_ENABLE;
    qdma_set_txq_dei_drop_mode(&qdma_data_tmp);
    
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RX_PACKET;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmRecvCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EVENT_HANDLER;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmEventCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);	
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT2_RX_PACKET;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmInt2RecvCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT2_EVENT_HANDLER;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmInt2EventCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT3_RX_PACKET;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmInt3RecvCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT3_EVENT_HANDLER;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmInt3EventCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT4_RX_PACKET;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmInt4RecvCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_INT4_EVENT_HANDLER;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmInt4EventCallbackFunction;
	qdma_register_callback_function(&qdma_data_tmp);	
#if SUPPORT_PON_INT_IN_QDMA
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_GPON_MAC_HANDLER;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmGponMacIntHandler;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EPON_MAC_HANDLER;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmEponMacIntHandler;
	qdma_register_callback_function(&qdma_data_tmp);
	qdma_data_tmp.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_XPON_PHY_HANDLER;
	qdma_data_tmp.qdma_private.qdma_reg_cb.pCbFun = function_hook_tmp.bmXponPhyIntHandler;
	qdma_register_callback_function(&qdma_data_tmp);
#endif

    // qdma indirect reg restore
    qdma_indirect_reg_restore();

    for(i=0, intIdx=QDMA_INT1; intIdx<QDMA_INT_NUM; i++, intIdx++) {
#if !SUPPORT_INT_ENABLE_NUM_2
        intEnIdx = QDMA_INT_ENABLE0;
#else
        intEnIdx = QDMA_INT_ENABLE1;
#endif
        for(j=0; j<QDMA_INT_ENABLE_NUM; j++, intEnIdx++) {
            qdmaSetIntMask(base, intIdx, intEnIdx, qdma_int_en[i][j]);
        }
    }

    return 0 ;
}

module_init(qdma_module_init);
module_exit(qdma_module_cleanup);
module_param(macType, uint, 0);
