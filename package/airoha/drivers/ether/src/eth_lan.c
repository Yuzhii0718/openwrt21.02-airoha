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
#include "eth_lan.h"
#if defined(TCSUPPORT_WAN_ETHER)
#include "eth_wan.h"
#endif


#if defined(TCSUPPORT_SEPERATED_ETH_ITF) 
#include "eth_virtual_if.h"
#endif

#ifdef TCSUPPORT_AUTOBENCH
#include "../../auto_bench/autobench.h"
#endif

#include "./tcphy/tcconsole.h"
    
#include "eth_proc.h"
#include "fe_verify.h"
#ifdef TCSUPPORT_MT7530_SWITCH_API
#include "./tcphy/mtkswitch_api.h"
#include <modules/eth_global_def.h>
/*extern int macMT7530gswAPIDispatch(struct ifreq *ifr);*/
#endif

#ifdef LOOPBACK_SUPPORT
#include "fe_verify.h"
#endif
    
#include "./tcphy/phy_api.h"
#include "eth_api.h"
#include <ecnt_hook/ecnt_hook_traffic_classify.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_multicast.h>
#include <ecnt_hook/ecnt_hook_multicast_general.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_l2tp.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_ephy.h>
#include <ecnt_hook/ecnt_hook_bbf247.h>
#ifdef TCSUPPORT_OLT
#include <ecnt_hook/ecnt_hook_olt_mac.h>
#endif

#ifdef TCSUPPORT_SOE
#include <ecnt_hook/ecnt_hook_ipsec.h>
#endif

#ifdef TCSUPPORT_XSI_ENABLE
#include <ecnt_hook/ecnt_hook_xsi_mac.h>
#endif

#ifdef TCSUPPORT_HSGMII_LAN
#include <ecnt_hook/ecnt_hook_hsgmii_mac.h>
#endif

#ifdef TCSUPPORT_ACTIVE_ETHERNET_WAN
#include <ecnt_hook/ecnt_hook_ae_mac.h>
#endif

#include <ecnt_event_global/ecnt_event_global.h>
#include <ecnt_event_global/ecnt_event_eth.h>
#include <linux/libcompileoption.h>
#ifdef TCSUPPORT_DS_HWNAT_OFFLOAD
#include "./soft_qdma/soft_qdma.h"
#endif

#include "ecnt_hook/ecnt_hook_vlan.h"
#include <ecnt_hook/ecnt_hook_asym.h>
#include "modules/ptp/ptp.h"
#include <linux/foe_hook.h>


#include "eth_ext_switch.h"

#ifdef INCLUDE_ETH_SWITCH_RTL8372
#include "eth_rtl8372n.h"
#endif /* INCLUDE_ETH_SWITCH_RTL8372 */

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef __BIG_ENDIAN
#define FE_BYTE_SWAP
#endif
    
#ifndef TCSUPPORT_CPU_MT7520
#define RAETH_CHECKSUM_OFFLOAD
#endif
    
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#if defined(WAN2LAN) || defined(CONFIG_TC3162_ADSL)
#define VLAN_TAG_USED 0
#else
#define VLAN_TAG_USED 1
#endif
#else
#define VLAN_TAG_USED 0
#endif
    
#define GEN_1588_PKT_7530_VERIFY	/*MTK20120829_MT7530_1588pkt_generation*/

/* ADMTEK6996M register */
#define ADM_PORT0_BASIC		0x01
#define ADM_PORT1_BASIC		0x03
#define ADM_PORT2_BASIC		0x05
#define ADM_PORT3_BASIC		0x07
#define ADM_PORT4_BASIC		0x08
#define ADM_PORT5_BASIC		0x09

#define ADM_CHIP_ID0		0xa0
#define ADM_CHIP_ID1		0xa1


extern u32 macMT7530SwitchMap2LanPort(u32 switchPortId);


#ifdef CONFIG_TP_IMAGE
  /* fix bug 856251, wan mark is conflic with 8021.p remark, cause pbit incorrect and packet is dropped by olt */
  #ifdef CONFIG_8021P_REMARK
  #undef CONFIG_8021P_REMARK
  #endif
#else
#define CONFIG_8021P_REMARK 1
#endif /* CONFIG_TP_IMAGE */

#if defined(TCSUPPORT_WAN_ETHER) && defined(CONFIG_8021P_REMARK)
#define QOS_8021p_MARK			0x0F00 	/* 8~11 bits used for 802.1p */
#define QOS_8021P_0_MARK		0x08	/* default mark is zero */
#define VLAN_HLEN				4
#define VLAN_ETH_ALEN			6
#endif

#define     VLAN_REMARKING      0x0FFF00
#define     PRI_STORE     0xE


#define INITIALIZED 1
#define UNINIT 0

#define GE_PORT		4			/*for 7521F,switch GE-port num*/

#define MAC_SYNC_TIMEUP		12	/*one step for 250ms, 12 means 3s*/

#define DOWN 0
#define UP 1

#define QOS_IN_SWITCH   0
#define QOS_IN_QDMA     1

#define EXT_SWITCH_FLAG 0x80
#define SWITCH_PORT_MASK 0x7f

#define SPEED_TEST_SUCCESS	0

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

#ifdef TCSUPPORT_CPU_EN7580
/*
    switch port : 0x10 ~ 0x15
*/
#define NO_SPECIAL_TAG(skb, _rxmsg) (!((_rxmsg.sport >= 0x10) && (_rxmsg.sport <= 0x15)))
#else
#define NO_SPECIAL_TAG(skb, _rxmsg) (((*(uint16 *)(skb->data+12)) & htons(0xfc78)) > 0) 
#endif

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
static int ephy_7581_GSW_polling_switch_on=1;
extern int eth_fast_mode_flag;
extern int wan_port_id;
extern uint32 macLoopback;
extern uint32 macRxLoopback;
extern int macTest_channel;
extern int macTest_queue;
extern int macTest_vid;
extern int macTest_insert_vid;

/* frankliao added 20101215 */
extern unsigned long flash_base;
extern unsigned int (*ranand_read_byte)(unsigned long long);
/*MTK20120829_MT7530_1588pkt_generation*/
extern void getnstimeofday(struct timespec *tv);    
extern void tcephydbgcmd(void);
extern int macSetUpPhy(macAdapter_t *mac_p);
extern int mt7530_switch_init(void);
extern int32 macPhyLookUp(macAdapter_t *mac_p, uint32 companyId);
extern void macSetGSW(macAdapter_t *mac_p);

extern int checkQueueForSwitchBySkbMark(struct sk_buff* skb);
extern void (*wlan_to_lan_hook)(struct sk_buff *skb, u8 local);


extern void external_switch_init(void);

#if defined(TCSUPPORT_WAN_ETHER)
extern int gmac_wan_init(void);
extern int gmac_wan_exit(void);
extern struct net_device *wan_dev;
extern macAdapter_t *mac_wan_p;
extern int (*wan_speed_test_hook)(struct sk_buff*);
extern int (*wan_tr471_hook)(struct sk_buff*);
#endif                                        

extern uint8 use_ext_switch;

extern unsigned int queuemark_enable;

#if defined(TCSUPPORT_WAN2LAN_EXT)
extern uint32 (*gswPbusRead_hook)(uint32 pbus_addr);
extern int (*gswPbusWrite_hook)(uint32 pbus_addr, uint32 pbus_data);
extern int (*lanPortmap_hook)(int lanPort);
extern int (*lanPortmap_rev_hook)(int switchPort);
extern int masko_ext;
#endif


#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
extern eth_port_vlan_t eth_port_vlan[];
extern char extend_switch_port;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
extern int (*hook_lan_led_action)(u8 mode);
#endif
extern int (*soft_ratelimit_enqueue_hook) (struct sk_buff * skb,unsigned int queue_idx);


extern char lan_port_map[ ];
extern char switch_port_map[ ];

#if defined(TCSUPPORT_SWNAT)
extern int (*sw_downstream_nat_tx_hook)(struct sk_buff * skb, uint msg0, uint msg1, struct port_info * qdma_info);
extern int (*sw_upstream_nat_rx_hook) (struct sk_buff * skb, int foe_tbl_index);
#endif

extern int checkSkbFromSocSwitchPort(struct sk_buff* skb);

extern int virtual_itf_link_aggr_config(int enable);
extern int (*hwnat_clean_lan_hook)(unsigned int);

#ifdef TCSUPPORT_ECNT_1588v2
extern int (*ptp_1588_tx_ptk_check_hook)(struct sk_buff *skb, uint32 *msg0, uint32 *msg1);
extern void (*ptp_1588_tx_tstamp_hook)(uint32 ptpDev);
extern int (*ptp_1588_rx_tstamp_hook)(struct sk_buff *skb);
#endif

extern struct net_device *lan_virtual_dev[MAX_ETH_ITF_NUM];

extern void (*set_qdma_lan_qos_buffer_mode_hook)(int qdma_mode);

extern unsigned int wan_conn_type;
extern char wan_itf_name[];

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/
/* netdev ops eth0*/
static int eth_lan_mac_start(struct net_device *dev);
static int eth_lan_mac_open(struct net_device *dev);
static int eth_lan_mac_close(struct net_device *dev);
static int eth_timer_init(void);
static struct net_device_stats *eth_lan_mac_stats(struct net_device *dev);
static struct rtnl_link_stats64 * eth_lan_mac_stats_64(struct net_device* dev, struct rtnl_link_stats64* stats);
int eth_lan_mac_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
int my_eth_change_mtu(struct net_device *dev, int new_mtu);
static int eth_lan_mac_set_macaddr(struct net_device *dev, void *p);

ecnt_ret_val ecnt_ether_hook(struct ecnt_data *indata);

#if 1
ecnt_ret_val fh_eth_rx_hook(struct ecnt_data *indata);
ecnt_ret_val fh_eth_tx_hook(struct ecnt_data *indata);
#endif

void gswLanLinkSt_monitor(void);
void perChannelRateLimitSwitch(void);
int eth_get_force_dstq(struct ECNT_ETHER_Data *eth_data);

extern int (*xsi_linkst_monitor_hook)(void);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************/


#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_LOOPDETECT)
    int (*check_rcv_hook)(struct sk_buff *skb) = NULL;
     EXPORT_SYMBOL(check_rcv_hook);
#endif/*TCSUPPORT_COMPILE*/

mt7530_switch_brtable *pBrtable = NULL ;

ratelimit_global_mac_t ratelimit_global_mac_p;
int macqueue_start_idx = -1;
EXPORT_SYMBOL(macqueue_start_idx);

uint16 lan_port_tpid[MAX_ETH_ITF_NUM];
uint16 ether_wan_tpid = DEFAULT_TPID;
uint16 cpu_port_tpid = DEFAULT_TPID;

/* for EN7516 SOC test 20170714 */
int g_dump_pkt_cnt = 0;
#ifdef CONFIG_SUPPORT_SELF_TEST
int g_eth_overwrite_tx_msg = 0;
int g_eth_remove_sp_tag = 0;
#endif

/* 0:disable, 1: force to gdma, 2; force to qdma */
int g_force_dstq;
EXPORT_SYMBOL(g_force_dstq);
extern int pcie0_force_dstq;
extern int pcie1_force_dstq;
extern int usb_force_dstq;
extern int eth_force_dstq;
extern int pcie0_srcport;
extern int pcie1_srcport;
extern int usb_srcport;
extern int eth_srcport;

uint32 dsoffload_presetting = 0; 
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
extern int g_wan_mode;
#endif
EXPORT_SYMBOL(dsoffload_presetting);
portInfo lanPortState[LIMIT_PORT_NUM];
EXPORT_SYMBOL(lanPortState);
portInfo lanPortStateExt[LIMIT_PORT_NUM];

uint burst_monitor_enable = 1;

uint g_cdma1_rxhwf_drop_cnt_old = 0;
#ifdef TCSUPPORT_ETHER_DOWNVLAN
    void (*eth_down_vlan_hook)(struct sk_buff *skb) = NULL;
    EXPORT_SYMBOL(eth_down_vlan_hook);
#endif
#if defined(TCSUPPORT_TSO_ENABLE)
    static int max_pkt_len = 65535;
#else
    static int max_pkt_len = 2000;
#endif

/*0:switch, 1:qdma_lan*/
int ds_qos;
EXPORT_SYMBOL(ds_qos);

int link_aggr_en = 0;

#ifdef TCSUPPORT_AUTOBENCH
int eth_mac_tx_autobench(struct sk_buff *skb, struct net_device *dev)
{
	if (skb != NULL){
		dev_kfree_skb_any(skb);
	}
	return NETDEV_TX_OK;
}
#endif

#if KERNEL_2_6_36
static const struct net_device_ops gmac_netdev_ops = {
    .ndo_init               = eth_lan_mac_start,
    .ndo_open               = eth_lan_mac_open,
    .ndo_stop               = eth_lan_mac_close,
    .ndo_start_xmit         = eth_lan_mac_tx,
    .ndo_get_stats          = eth_lan_mac_stats,
    .ndo_get_stats64        = eth_lan_mac_stats_64,
/*    .ndo_set_multicast_list = tc3262_gmac_set_multicast_list,*/
    .ndo_do_ioctl           = eth_lan_mac_ioctl,
#ifdef TCSUPPORT_MAX_PACKET_2000
    .ndo_change_mtu         = my_eth_change_mtu,
#else
    .ndo_change_mtu         = eth_change_mtu,
#endif
    .ndo_set_mac_address    = eth_lan_mac_set_macaddr,
    .ndo_validate_addr      = eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
    .ndo_poll_controller    = tc3262_gmac_poll_controller,
#endif
};

#endif /*KERNEL_2_6_36*/

#if defined(TCSUPPORT_CPU_MT7520)
int mac_receive_num = 0; //0 means receive packet and no drop
int mac_receive_threshold = MAC_RXDESCP_NO; //MAC_RXDESCP_NO_DEFAULT; //0 means receive packet and no drop
#else
int mac_receive_num = MAC_RECV_THLD; //0 means receive packet and no drop
int mac_receive_threshold = MAC_RXDESCP_NO_DEFAULT; //0 means receive packet and no drop
#endif

struct ecnt_hook_ops ecnt_driver_ether_op = {
    .name = "driver_ether_hook",
    .hookfn = ecnt_ether_hook,
    .maintype = ECNT_ETHER_SWITCH,
    .is_execute = 1,
    .subtype = ECNT_DRIVER_API,
    .priority = 1
};

#if 1
struct ecnt_hook_ops fh_eth_rx_op = {
    .name = "fh_eth_rx_hook",
    .hookfn = fh_eth_rx_hook,
    .maintype = ECNT_ETHER_SWITCH,
    .is_execute = 1,
    .subtype = ECNT_ETH_RX,
    .priority = 1
};

struct ecnt_hook_ops fh_eth_tx_op = {
    .name = "fh_eth_tx_hook",
    .hookfn = fh_eth_tx_hook,
    .maintype = ECNT_ETHER_SWITCH,
    .is_execute = 1,
    .subtype = ECNT_ETH_TX,
    .priority = 1
};
#endif

extern wan_virtualdev_ifname wan_virtualdev_ifname_info[16];
uint8 interface_map[TRAFFIC_TYPE_MAX] = {0};
/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
/* Device data */
struct net_device *lan_dev;
macAdapter_t *mac_p = NULL;

/* Timer */
static struct timer_list eth_timer;
static struct timer_list eth_poll_timer;

/* phy lock */
spinlock_t phy_lock;

#ifdef TCSUPPORT_CPU_AN7583
spinlock_t phy_lock_ext0;
spinlock_t phy_lock_ext1;
#endif

static macPhyLinkProfile_t enetPhyLinkProfile;
uint8 macInitialized = 0;    
extern uint8 def_mac_addr[6];
uint8 swicVendor = 0;
static DEFINE_SPINLOCK(eth_napilock);
#ifdef WAN2LAN
/* ------ xyzhu_091105:special tag relation data start ---------- */
extern uint8 macSTagFlag;
/* ------ xyzhu_091105:special tag relation data end ----------- */
extern int masko_on_off;
extern int wlan_masko_on_off;
#endif
int ethmac_qdmaCfg=UNINIT;

/*for macTable sync to Kernel , bugfix for L2B DMAC+VLAN mode*/
unsigned char mac_sync_en = 0 ;
unsigned char mac_sync_timer_count = 0 ;
unsigned char mac_sync_flag = 0 ;

/* ------------------ module param ------------------ */
char *wan_itf  = "nas10";
char *lan_itf  = "eth0";
char *sep_itf  = "eth0.";

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
int itf_num =8;
#else
int itf_num =4;
#endif

/* ------------------ ECNT_Hook ------------------ */
typedef int (*ether_api_op_t)(struct ECNT_ETHER_Data *ephy_data);

/* Warning: same sequence with enum 'ETHER_HookFunction_t' */
ether_api_op_t
ether_operation[]=
{
    eth_mdio_read,
    eth_mdio_write,
    eth_mac_send,
    eth_gsw_pbus_read,	/* ETHER_FUNCTION_GSW_PBUS_READ */
    eth_gsw_pbus_write,	/* ETHER_FUNCTION_GSW_PBUS_WRITE */
    eth_ext_gsw_pbus_read,
    eth_ext_gsw_pbus_write,
    eth_set_ratelimit_switch,
    eth_set_mactable_sync_en,
    rgmii_traffic_set,
    rgmii_mode,
    eth_set_port_matrix,
    eth_get_port_map,
    eth_set_port_linkstate,
    eth_set_per_vlan_action,
    eth_set_per_port_vlan_action,
    eth_mac_autobench_loopback,
    eth_clean_mactable,
    eth_use_qdma_wan,
    eth_rx_for_lro,
    eth_get_force_dstq,
    eth_ctag_white_list_mode,
    eth_ctag_white_list_per_port_mode,
    eth_ctag_white_list_add,
    eth_ctag_white_list_del,
    eth_get_phy_addr,
    eth_add_arl_diptbl,
    eth_add_arl_siptbl,
    eth_add_arl_iptbl_multi,
    eth_get_per_port_mib_counter,
    eth_get_drop_crc_counter,
    eth_set_port_mirror,
    eth_set_flow_control,
    eth_get_flow_control
};

spinlock_t hookFuncLock[ETHER_FUNCTION_MAX_NUM];

uint8 hal_api_ext=0;
/*zdh: soft 64 bit flag reserve*/
unsigned long long Expand_64_Tx[GSW_MAX_TX_TYPE][GSW_MAX_PORT] = {0};
unsigned long long Expand_64_Rx[GSW_MAX_RX_TYPE][GSW_MAX_PORT] = {0};
/*kaidong: soft switch for lan 64 bit counter*/
int expand_64bit_monitor_enable = 0;

unsigned int g_Tx_Last[GSW_MAX_TX_TYPE][GSW_MAX_PORT] = {0};
unsigned int g_Rx_Last[GSW_MAX_RX_TYPE][GSW_MAX_PORT] = {0};
/*TCSUPPORT_FWC_ENV start*/
uint8 ratelimit_En = 0 ;/*1: downstream total bandwidth ratelimit works; 0: not work*/
uint8 ge_portlimit_En = 0 ;/*GE port, set per-port limit, 1:enable ; 0 :disable*/
/*TCSUPPORT_FWC_ENV endt*/
/* ----------------------------------------------- */

/* ------------------ ECNT_Hook for Kernel ------------------ */
ecnt_ret_val ecnt_eth_updateKernelMacTable(struct ecnt_data *in_data)
{
	if( ENABLE == mac_sync_en )
	{
		mac_sync_flag = ENABLE ;
		return ECNT_CONTINUE;
	}

	return ECNT_CONTINUE;
}

struct ecnt_hook_ops ecnt_eth_updateKernelMacTable_op = {
	.name = "ecnt_delDevBandWidthMac",
	.is_execute = 1,
	.hookfn = ecnt_eth_updateKernelMacTable,
	.maintype = ECNT_NET_BR_FDB,
	.subtype = ECNT_BR_FDB_GET,
	.priority = 1
};
/* ----------------------------------------------- */

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
ecnt_ret_val ecnt_ether_hook(struct ecnt_data *indata)
{
	struct ECNT_ETHER_Data *ether_data = (struct ECNT_ETHER_Data *)indata ;
	ulong flags=0 ;	
	
	if(ether_data->function_id >= ETHER_FUNCTION_MAX_NUM) {
		printk("ether_data->function_id is %d, exceed max number: %d", 
		    ether_data->function_id, ETHER_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}
	ether_data->retValue = ether_operation[ether_data->function_id](ether_data) ;
	
	return ECNT_CONTINUE;
}

#if 1
/* *********************************************************************************
*   the following two function is only for FHT simulation test, and FHT need to accomplish the hook
***********************************************************************************/
ecnt_ret_val fh_eth_rx_hook(struct ecnt_data *indata)
{

	return ECNT_CONTINUE;
}

ecnt_ret_val fh_eth_tx_hook(struct ecnt_data *indata)
{

	return ECNT_CONTINUE;
}
#endif
/************************************************************************
    Funtcion:       dump_skb
    Description:    
    Calls:
    Called by:      for debug
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void dump_skb(struct sk_buff *skb)
{
	char tmp[80];
	char *p = skb->data;
	char *t = tmp;
	int i, n = 0;

	printk("ERR skb=%08lx skb->head=%08lx \n data=%08lx len=%d mark=%08lX\n cb=%08lx \n", 
	    (uint32) skb, (uint32) skb->head, (uint32) skb->data, skb->len, skb->mark, 
	    *(unsigned long int *)(skb->cb));
    printk("skb dev %s, orig dev %s\n", skb->dev->name, skb->original_dev->name);
	for (i = 0; i < skb->len; i++) {
		t += sprintf(t, "%02x ", *p++ & 0xff);
		if ((i & 0x0f) == 0x0f) {
			printk("%04x: %s\n", n, tmp);
			n += 16;
			t = tmp;
		}
	}
	if (i & 0x0f)
		printk("%04x: %s\n", n, tmp);
}
EXPORT_SYMBOL(dump_skb);

int eth_get_force_dstq(struct ECNT_ETHER_Data *eth_data)
{
    eth_data->ether_private.force_dstq = g_force_dstq;
    return 0;
}

/************************************************************************
    Funtcion:       merge_switchtable2kerneltable
    Description:    merge switch mactable to kernel mac table
    Calls:          
    Called by:      eth_monitor
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void merge_switchtable2kerneltable(void)
{
	int index = 0 ;
	unsigned int portID = 0 ;
	unsigned int lanID = 0 ;
	struct net_device *dev = NULL ;
	struct net_bridge_port *p = NULL ;
	struct net_bridge *br = NULL ;

	memset(pBrtable, 0, sizeof(mt7530_switch_brtable));
	/*read the switch mac table*/
	macMT7530GetAllBRTable(pBrtable);

	/*merge the switch mac table to Kernel mac table*/
	for( index = 0 ; index < pBrtable->count ; index++ )
	{
		portID = pBrtable->macinfo[index].port ;
		lanID = macMT7530SwitchMap2LanPort( portID ) ;
		
		if(lanPortState[portID].linkState == DOWN)
			continue;

#if defined(TCSUPPORT_SEPERATED_ETH_ITF) 
		/******** virtual interface eth0.1~4 assign to dev ********/
		dev = find_virtual_itf_by_index(lanID) ;
#endif
		if(dev)
		{
			p = br_port_get_rcu(dev) ;
			if( (!p) || (p->state == BR_STATE_DISABLED) )
			{
				continue ;
			}
			br = p->br ;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19)	
			if( p->flags & BR_LEARNING )
				br_fdb_update(br, p , pBrtable->macinfo[index].mac , 0 , false);
#else
			br_fdb_update(br, p , pBrtable->macinfo[index].mac , NULL);
#endif
#endif

		}
	}

	return ;
}

/************************************************************************
    Funtcion:       burst_monitor
    Description:   monitor downstream burst that QDMA can not hold, enable cpu buffer
    Calls:          
    Called by:     eth_monitor
    Input:           N/A
    Output:         N/A
    Return:         N/A
    Others:         N/A
************************************************************************/
void burst_monitor(void)
{
    uint32 cdma1_rxhwf_drop_cnt = 0;

    /* get counter of pkts from WAN dropped at GDMA2 due to HWFWD DSCP running out */
    cdma1_rxhwf_drop_cnt = read_reg_word(CDMA1_RXHWF_DROP_CNT);
    if (unlikely(cdma1_rxhwf_drop_cnt != g_cdma1_rxhwf_drop_cnt_old))
    {
        /* enable half speed up, DP of FOE entry will be cpu port afterwards */
        dsoffload_presetting |= 0x1; /* DS_MULTICAST enable multicast by default */

        burst_monitor_enable = 0;

        /* clear all FOE entries, the entries will be re-learned afterwards */
#if defined(TCSUPPORT_RA_HWNAT_ENHANCE_HOOK)
        if(ra_sw_nat_hook_clean_multicast_entry)
        {
            ra_sw_nat_hook_clean_multicast_entry();
        }
#endif
    }
}

void  expand_64bit_monitor(void)
{
	int tmp_1, tmp_2;

	for(tmp_1=0; tmp_1<GSW_MAX_TX_TYPE - 2 ; ++tmp_1)
	{
		for(tmp_2=0; tmp_2<GSW_MAX_PORT ; ++tmp_2)
		{
			unsigned int tx_tmp_0 = calcnt(GSW_MIB_BASE + tmp_1*0x04);
			unsigned int tx_tmp = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + tmp_1*0x04);
			if(tmp_2 == 0)
			{
				if(tx_tmp_0 < **(g_Tx_Last+tmp_1))
				{
					**(Expand_64_Tx+tmp_1) += 1;
					
				}
				**(g_Tx_Last+tmp_1) = tx_tmp_0;
			}
			else
			{
				if(tx_tmp< *(*(g_Tx_Last+tmp_1)+tmp_2))
				{
					*(*(Expand_64_Tx+tmp_1)+tmp_2) += 1;					
				}
				*(*(g_Tx_Last+tmp_1)+tmp_2) = tx_tmp;
			}			
		}
	}
	for(tmp_2=0; tmp_2<GSW_MAX_PORT ; ++tmp_2)
	{
		unsigned int tx_tmp_ocl_0 = calcnt(GSW_MIB_BASE +  0x48);
		unsigned int tx_tmp_och_0 = calcnt(GSW_MIB_BASE +  0x4c);
		unsigned int tx_tmp_ocl = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + 0x48);
		unsigned int tx_tmp_och = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + 0x4c);

		if(0 == tmp_2)
		{
			if (tx_tmp_ocl_0<*(*(g_Tx_Last+12)))
				*(*(Expand_64_Tx+12)) += 1;
			*(*(g_Tx_Last+12)) = tx_tmp_ocl_0;
			if (tx_tmp_och_0<*(*(g_Tx_Last+13)))
				*(*(Expand_64_Tx+13)) += 1;
			*(*(g_Tx_Last+13)) = tx_tmp_och_0;
		}
		else
		{		
			if (tx_tmp_ocl<*(*(g_Tx_Last+12)+tmp_2))
				*(*(Expand_64_Tx+12)+tmp_2) += 1;
			*(*(g_Tx_Last+12)+tmp_2) = tx_tmp_ocl;
			if (tx_tmp_och<*(*(g_Tx_Last+13)+tmp_2))
				*(*(Expand_64_Tx+13)+tmp_2) += 1;
			*(*(g_Tx_Last+13)+tmp_2) = tx_tmp_och;
		}		
	}

	for(tmp_1=0; tmp_1<GSW_MAX_RX_TYPE - 4; ++tmp_1)
	{
		for(tmp_2=0; tmp_2<GSW_MAX_PORT ; ++tmp_2)
		{	
			unsigned int rx_tmp_0 = calcnt(GSW_MIB_BASE + tmp_1*0x04+0x60);
			unsigned int rx_tmp = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + tmp_1*0x04+0x60);
			if(tmp_2 == 0)
			{
				if(rx_tmp_0 < **(g_Rx_Last+tmp_1))
				{
					**(Expand_64_Rx+tmp_1) += 1;
					
				}
				**(g_Rx_Last+tmp_1) = rx_tmp_0;
			}
			else
			{
				if(rx_tmp< *(*(g_Rx_Last+tmp_1)+tmp_2))
				{
					*(*(Expand_64_Rx+tmp_1)+tmp_2) += 1;					
				}
				*(*(g_Rx_Last+tmp_1)+tmp_2) = rx_tmp;
			}		
		}
	}
	for(tmp_2=0; tmp_2<GSW_MAX_PORT; ++tmp_2)
	{
		unsigned int rx_tmp_ocl_0 = calcnt(GSW_MIB_BASE +  0xa8);
		unsigned int rx_tmp_och_0 = calcnt(GSW_MIB_BASE +  0xac);
		unsigned int rx_tmp_ingc_0 = calcnt(GSW_MIB_BASE +  0xb4);
		unsigned int rx_tmp_argc_0 = calcnt(GSW_MIB_BASE +  0xb8);

		unsigned int rx_tmp_ocl = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + 0xa8);
		unsigned int rx_tmp_och = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + 0xac);
		unsigned int rx_tmp_ingc = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + 0xb4);
		unsigned int rx_tmp_argc = switch_reg_read(GSW_MIB_BASE + tmp_2*0x100 + 0xb8);


		if(tmp_2 ==0)
		{
			if (rx_tmp_ocl_0<*(*(g_Rx_Last+12)))
			{
				*(*(Expand_64_Rx+12)) += 1;
			}
			*(*(g_Rx_Last+12)) = rx_tmp_ocl_0;
			if (rx_tmp_och_0<*(*(g_Rx_Last+13)))
			{
				*(*(Expand_64_Rx+13)) += 1;
			}
			*(*(g_Rx_Last+13)) = rx_tmp_och_0;
			if (rx_tmp_ingc_0<*(*(g_Rx_Last+14)))
			{
				*(*(Expand_64_Rx+14)) += 1;
			}
			*(*(g_Rx_Last+14)) = rx_tmp_ingc_0;
			if (rx_tmp_argc_0<*(*(g_Rx_Last+15)))
			{
				*(*(Expand_64_Rx+15)) += 1;
			}
			*(*(g_Rx_Last+15)) = rx_tmp_argc_0; 
		}
		else
		{
			if (rx_tmp_ocl<*(*(g_Rx_Last+12)+tmp_2))
			{
				*(*(Expand_64_Rx+12)+tmp_2) += 1;
			}
			*(*(g_Rx_Last+12)+tmp_2) = rx_tmp_ocl;
			if (rx_tmp_och<*(*(g_Rx_Last+13)+tmp_2))
			{
				*(*(Expand_64_Rx+13)+tmp_2) += 1;
			}
			*(*(g_Rx_Last+13)+tmp_2) = rx_tmp_och;
			if (rx_tmp_ingc<*(*(g_Rx_Last+14)+tmp_2))
			{
				*(*(Expand_64_Rx+14)+tmp_2) += 1;
			}
			*(*(g_Rx_Last+14)+tmp_2) = rx_tmp_ingc;
			if (rx_tmp_argc<*(*(g_Rx_Last+15)+tmp_2))
			{
				*(*(Expand_64_Rx+15)+tmp_2) += 1;
			}
			*(*(g_Rx_Last+15)+tmp_2) = rx_tmp_argc; 
		}
	}
}

int dyn_qdma1_txq_cngst_en = 0;
int dyn_qdma1_txq_cngst_total_min = 0x1300;    /* the low 16 bit of 0xbfb540a4*/
int dyn_qdma1_txq_cngst_timeout = 180;     /* unit: second*/
int dyn_qdma1_txq_cngst_stream_rate = 90;  /* unit Mbps */
int dyn_qdma1_txq_cngst_other_queue_rate = 50;     /* unit: pps */

static inline int is_downstream_over_threshold(void)
{
    static unsigned int gsw_p6_rx_byte_cnt_old = 0;
    
    unsigned int gsw_p6_rx_byte_dif = 0;
    unsigned int gsw_p6_rx_byte_cnt_new = read_reg_word(GSW_RX_OCL(6)); 

    if(gsw_p6_rx_byte_cnt_new < gsw_p6_rx_byte_cnt_old)
        gsw_p6_rx_byte_dif = (((unsigned int) -1) - gsw_p6_rx_byte_cnt_old + gsw_p6_rx_byte_cnt_new);
    else
        gsw_p6_rx_byte_dif = gsw_p6_rx_byte_cnt_new - gsw_p6_rx_byte_cnt_old;

    gsw_p6_rx_byte_cnt_old = gsw_p6_rx_byte_cnt_new;
    
    if(gsw_p6_rx_byte_dif && /* byte to Mbps */
        ((gsw_p6_rx_byte_dif>>17) > dyn_qdma1_txq_cngst_stream_rate))
        return 1;

    return 0;
}

static inline int is_qdma1_performance_test(void)
{
	static unsigned int qdma1_queue_cnt_last[CONFIG_QDMA_QUEUE] = {0};

	int index = 0;
	QDMA_TxQCngstAllQueueValue_T txqCngstAllQueueValue;
	unsigned int qdma1_queue_cnt[CONFIG_QDMA_QUEUE] = {0};
	unsigned int qdma1_queue_cnt_per_sec[CONFIG_QDMA_QUEUE] = {0};
	unsigned int qdma1_queue_rate_highest_index=0;	 /* which queue is highest rate */
	unsigned int qdma1_queue_rate_highest_per_sec=0; 

	memset(&txqCngstAllQueueValue, 0, sizeof(txqCngstAllQueueValue));
	QDMA_API_GET_DBG_CNTR_ALL_QUEUE_COUNTER(ECNT_QDMA_LAN, &txqCngstAllQueueValue);

	for(index=0; index<CONFIG_QDMA_QUEUE; index++){
		qdma1_queue_cnt[index] = txqCngstAllQueueValue.queueAllCount[index];
		if(qdma1_queue_cnt[index] >= qdma1_queue_cnt_last[index]){
			qdma1_queue_cnt_per_sec[index] = (qdma1_queue_cnt[index] - qdma1_queue_cnt_last[index]);
		}
		else{
			qdma1_queue_cnt_per_sec[index] = (((unsigned int) -1) - qdma1_queue_cnt_last[index] + qdma1_queue_cnt[index]);
		}
		qdma1_queue_cnt_last[index] = qdma1_queue_cnt[index];

		if(qdma1_queue_rate_highest_per_sec < qdma1_queue_cnt_per_sec[index]) {
			qdma1_queue_rate_highest_index = index;
			qdma1_queue_rate_highest_per_sec = qdma1_queue_cnt_per_sec[index];
		}
	}

    if(qdma1_queue_rate_highest_per_sec < ((dyn_qdma1_txq_cngst_stream_rate<<17)/2000))/* lowest packets */
        return 0;
        
	for(index=0; index<CONFIG_QDMA_QUEUE; index++){
		if(index == qdma1_queue_rate_highest_index) 
            continue;
        
		if(qdma1_queue_cnt_per_sec[index] > dyn_qdma1_txq_cngst_other_queue_rate){
			return 0;
		}
	}
	
	return 1;
}

static inline void dyn_set_qdma_lan_txq_cngst_totalthr(void)
{
    static unsigned int dyn_sel_mode = 0;  /* 0: qos mode, 1: performance mode*/
    static unsigned int dyna_set_count = 0;

    int qdma1_performance_flag = 1;  
    int downstream_over_threshold_flag = 0;
    QDMA_TxQDynCngstTotalThrh_T total_threshold;

    downstream_over_threshold_flag = is_downstream_over_threshold();
    qdma1_performance_flag = is_qdma1_performance_test();

    memset(&total_threshold, 0, sizeof(total_threshold));

    if(0 == dyn_sel_mode 
        && (downstream_over_threshold_flag && qdma1_performance_flag)) {
        QDMA_API_GET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_LAN, &total_threshold);
        total_threshold.dynCngstTotalMinThrh = (dyn_qdma1_txq_cngst_total_min>>2);
        QDMA_API_SET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_LAN, &total_threshold);
        dyna_set_count = 0;
        dyn_sel_mode = 1;
        printk("%s [%d]: qdma_lan dynCngstTotalMinThrh set to 0x%x.\n", __FUNCTION__, __LINE__, dyn_qdma1_txq_cngst_total_min);
    }
 
    if(1 == dyn_sel_mode && 
        dyna_set_count >= dyn_qdma1_txq_cngst_timeout){
        QDMA_API_GET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_LAN, &total_threshold);
        total_threshold.dynCngstTotalMinThrh = 0x33;
        QDMA_API_SET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_LAN, &total_threshold);
        dyna_set_count = 0;
        dyn_sel_mode = 0;
        printk("%s [%d]: qdma_lan dynCngstTotalMinThrh set to default.\n", __FUNCTION__, __LINE__);
    }      
    dyna_set_count ++;
}

static inline void qdma1_txq_cngst_monitor(void)
{
    static int check_times = 0;

    if(dyn_qdma1_txq_cngst_en == 0)
        return;
    
    check_times++;
    if(check_times == 4)
    {
        check_times = 0;
        dyn_set_qdma_lan_txq_cngst_totalthr();
    }

    return;
}

/************************************************************************
    Funtcion:       eth_monitor
    Description:    auto polling eth phy link status
    Calls:          
    Called by:      eth_timer_init
    Input:
    Output:
    Return:
    Others:     ifconfig down
************************************************************************/

/* Monitor the status of link, re-do link initialization if necessary. */
static void eth_monitor(TIMER_FUN_PAAM data)
{
	uint32 bmsr;
	uint32 bmcr;
	uint32 loopback = 0;
	uint32 advert, lpa;
	uint32 advert2, lpa2;
	uint32 enetMode;
#if defined(TCSUPPORT_NEW_SPIFLASH_DEBUG)
	static uint32 cnt_threshold=0;
	extern uint32 spi_task2_en;
#endif

	if (mac_p == NULL)
		return;
	
#if defined(TCSUPPORT_NEW_SPIFLASH_DEBUG)
	if(spi_task2_en==1) {
		cnt_threshold++;
		if(cnt_threshold >= 100) {
			sf_task2_test();
			cnt_threshold = 100;
		}
	}
#endif

#ifdef TCPHY_SUPPORT
#ifndef MT7505_PQA_SWITCH_TEST
	periodChk();
#endif	
#endif	

#ifdef  TC_CONSOLE_ENABLE
    tcconsole_chk();
#endif

	/* save the original enet mode */
	enetMode = mac_p->macPhyLinkProfile_p->enetMode;

	if (swicVendor) {
		mac_p->macPhyLinkProfile_p->enetMode =  LAN_ST_100MB | LAN_ST_FULL_DUPLEX | LAN_ST_LINK_UP;
		mac_p->macPhyLinkProfile_p->enetMode &= ~LAN_ST_100MB;
		mac_p->macPhyLinkProfile_p->enetMode |= LAN_ST_1000MB;
	} else {
        // phy_api_loopback_get     phy_api_7512
        // phy_api_link_mode        phy_api_7512
        // phy_api_lpa              phy_api_7512
	    // phy_api_link_status      phy_api_7512
		mac_p->macPhyLinkProfile_p->enetMode = 0;
	}
    EPHY_API_GET_MONITOR();
	if( ephy_7581_GSW_polling_switch_on == 1)
{
	gswLanLinkSt_monitor();
}
	if(xsi_linkst_monitor_hook)
		xsi_linkst_monitor_hook();

	/*------merge switch mac table to kernel mac table-----*/
	if( mac_sync_timer_count < MAC_SYNC_TIMEUP )
	{
		mac_sync_timer_count++ ;
	}
	else if( ENABLE == mac_sync_flag )
	{
		merge_switchtable2kerneltable();
		mac_sync_flag = DISABLE ;
		mac_sync_timer_count = 0 ;
	}

	qdma1_txq_cngst_monitor();
	/*-----------------------------------------------*/

    /* for en7512 soft qdma, monitor downstream burst that QDMA can not buffer */
    if (TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && burst_monitor_enable)
    {
        burst_monitor();
    }

	/*For 64bit statistical expansion, update 32bit software expand high bit*/	
	if (expand_64bit_monitor_enable)
	{
		expand_64bit_monitor();
	}
	
	/* Schedule for the next time */
	eth_timer.expires = jiffies + msecs_to_jiffies(250);
  	add_timer(&eth_timer);
}

#if defined(TCSUPPORT_WAN2LAN_EXT)
void wan2lan_ext_tx(struct sk_buff *skb)
{
    if(masko_ext & 0x20){
        if(!(skb->mark & SKBUF_COPYTOLAN)){
            dump_skb(skb);
        }
    }
}
#endif

inline struct sk_buff * insert_special_tag(ethTxMsg_t *TxMsg, struct sk_buff *skb)
{
#ifdef CONFIG_SUPPORT_SELF_TEST
    /* for EN7516 SOC test 20170714 */
    if (g_eth_remove_sp_tag)
    {
        return skb;
    }
#endif

    if(unlikely(macSTagFlag == DISABLE)){
        return skb;
    }

#ifdef TCSUPPORT_ETHER_DOWNVLAN
    if(eth_down_vlan_hook)
        eth_down_vlan_hook(skb);
#endif

	return INSERT_STAG(TxMsg, skb);

}

extern int (*offload_eth_fast_tx_hook)(struct sk_buff *skb, int channel);
#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
extern int SmbdTxSpeedOn;
extern int SmbdTxSpeedCnt1;
inline int samba_short_cut_handler(struct sk_buff *skb, ethTxMsg_t *TxMsg)
{
	if(SmbdTxSpeedOn && skb->sk && skb->sk->sk_smbd_info.smbd_sk){	
		if(skb->smbd_on_speed){ /* this packet has speed up */
			SmbdTxSpeedCnt1++;
			TxMsg->raw.ico = 1;
			return NETDEV_TX_OK;
		}else{
			skb->sk->sk_smbd_info.smbd_outDev = skb->dev;
			memcpy(skb->sk->sk_smbd_info.smbd_mac_header, skb->data, ETH_HLEN);
			memcpy(skb->sk->sk_smbd_info.smbd_ip_header, skb->data+ETH_HLEN, sizeof(struct iphdr));
			skb->sk->sk_smbd_info.smbd_speed = 1;
		}
	}

	return -1;
}
#endif

extern int ShortCutTxSpeedCnt1;
inline int ecnt_app_shortcut_handler(struct sk_buff *skb, ethTxMsg_t *TxMsg)
{
    if(skb->sk && skb->sk->sk_shortcut_info.shortcut_sk)
    {   
        if(skb->shortcut_on_speed)
        {
            ShortCutTxSpeedCnt1++;
            return NETDEV_TX_OK;
        }
        else
        {
            skb->sk->sk_shortcut_info.out_dev = skb->dev;
            memcpy(skb->sk->sk_shortcut_info.mac_header, skb->data, ETH_HLEN);
            memcpy(skb->sk->sk_shortcut_info.ip_header, skb->data + ETH_HLEN, sizeof(struct iphdr));
            skb->sk->sk_shortcut_info.shortcut_speed = 1;
        }
    }
    return -1;
}

/************************************************************************
    Funtcion:
    Description:    update sw mib both for  wan and lan
    Calls:
    Called by:      eth_rx, eth_mac_tx, gmac_wan_tx 
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
__IMEM void update_sw_mib(macAdapter_t *mib_type,struct sk_buff *skb, int direction)
{
    if (mib_type->statisticOn) {
        switch(direction){
            case RX_MIB:
                mib_type->macStat.MIB_II.inOctets += (skb->len>64?skb->len:64);
                if (skb->data[0] == 0x01)
                    mib_type->macStat.MIB_II.inMulticastPkts++;
                else if(skb->data[0] == 0xFF)
                    mib_type->macStat.MIB_II.inBroadcastPkts++;
                else
                    mib_type->macStat.MIB_II.inUnicastPkts++;				
                break;
            case TX_MIB:
                mib_type->macStat.MIB_II.outOctets += (skb->len>64?skb->len:64);
            	if (skb->data[0] == 0x01)
            		mib_type->macStat.MIB_II.outMulticastPkts++;
                else if(skb->data[0] == 0xFF)
            		mib_type->macStat.MIB_II.outBroadcastPkts++;
            	else
            		mib_type->macStat.MIB_II.outUnicastPkts++;
                break;
        }
    }
}

#ifdef TCSUPPORT_AUTOBENCH
__IMEM int eth_mac_tx(struct sk_buff *skb, struct net_device *dev)
#else
int tc3262_gmac_tx_autobench(struct sk_buff *skb, struct net_device *dev)
#endif
{
	if (skb != NULL){
		dev_kfree_skb_any(skb);
	}
	return NETDEV_TX_OK;
}

int check_ratelimit_mac(struct sk_buff *skb, bool updown)
{
	int i,j,index;
	if(UP==updown)
		index = 6;
	else
		index = 0;
	for(i=0;i<MAC_NUM;i++)
	{
		if(ratelimit_global_mac_p.mac_q[i].updown==updown && ratelimit_global_mac_p.mac_q[i].active)
		{
			for(j=0;j<6;j++)
			if( *(skb->data+index+j) != ratelimit_global_mac_p.mac_q[i].mac[j])
				break;
			if(j==6)
			{
				skb->macEnque = i;
				skb->macEnque |= 1<<6;
				return 0;
			}
		}
	}
	return 0;
}

inline uint16 getStagBySkb(struct sk_buff *skb)
{	
	uint8 *cp=NULL;
	uint16 stag=0;

	cp = skb->data;
#ifdef TCSUPPORT_CPU_EN7580
	stag = skb->dlf_resv0;
#else
	stag = ntohs(*(uint16 *)(cp+12));
#endif
	
	return stag;
}

inline int getChannelByPortmask(unsigned long int port_mask)
{
	int i, channel=1 ;
	for(i=0;i<LIMIT_PORT_NUM;i++)
	{
		if( ((1<<i) & port_mask ) > 0 )
		{
			channel = i;
			break;
		}
	}
	
#if 0
	if( port_mask & ~(1<<channel))
	{
		channel = 5;
	}
#endif

	return channel ;
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CDS)
static void downstream_qos_cds(struct sk_buff *skb, struct port_info *eth_info)
{
	unsigned long int port_mask;

	if( ENABLE == ratelimit_En )
	{
		eth_info->channel = 0;
		eth_info->ds_to_qdma = 1;
		if(skb->pon_mark & DS_TRTCM_ENABLE_MARK)
		{
			eth_info->txq_is_valid = 1;
			eth_info->txq = skb->pon_mark&DS_TRTCM_ID_MARK;
		}
		if(skb->pon_mark & DS_QUEUE_ENABLE_MARK)
		{
		    eth_info->txq_is_valid = 1;
			eth_info->txq = skb->pon_mark&DS_QUEUE_ID_MARK;
		}
	}
	else
	{
		port_mask = getPortMask(skb);
		eth_info->channel = getChannelByPortmask(port_mask);
	}

	return;
}
#endif/*TCSUPPORT_COMPILE*/

/************************************************************************
    Funtcion:
    Description:    eth packet tx
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:     ifconfig down
************************************************************************/
#ifdef TCSUPPORT_AUTOBENCH
int tc3262_gmac_tx_autobench(struct sk_buff *skb, struct net_device *dev)
#else
__IMEM int eth_mac_tx(struct sk_buff *skb, struct net_device *dev)
#endif
{
    ethTxMsg_t ethTxMsg;
    int error,ringIdx;
	unsigned char  txq = 0, offload_packet = 0;
	struct port_info eth_info;
	struct port_info *qdma_info;
	unsigned long int port_mask;
    int tsid = 0x7f;
    int acnt1 = 0x1f;
    int acnt0 = 0x1f;
	int ret = -1;
#ifdef TCSUPPORT_ECNT_1588v2
	int ptp_pkt = 0;
#endif

if(macqueue_start_idx != -1  && soft_ratelimit_enqueue_hook)
{
	check_ratelimit_mac(skb,DOWN);
}

#ifdef TCSUPPORT_RA_HWNAT
	if (ra_sw_nat_hook_magic) 
	{
		if (ra_sw_nat_hook_magic(skb, FOE_MAGIC_PPE)) 
		{
			/* PPE offload*/
			offload_packet = 1;
		}
	}
#endif

	if(offload_packet == 1)
	{
		/***************** Count the MIB-II *****************/
		update_sw_mib(mac_p, skb, TX_MIB);
		/**************** prepare Tx Msg *****************/
		memset(&ethTxMsg, 0, sizeof(ethTxMsg_t));
		ethTxMsg.raw.fPort = DPORT_PPE;
		ethTxMsg.raw.channel = WIFI_OFFLOAD_CHANNEL;
		
		qdma_info = NULL;
	}
	else
	{
#if defined(TCSUPPORT_WAN2LAN_EXT)
		wan2lan_ext_tx(skb);
#endif
	/***************** virtual interface *****************/
#if defined(TCSUPPORT_SEPERATED_ETH_ITF)    
#ifdef TCSUPPORT_QTN_ISOLATION
		if(unlikely(virtual_itf_tx_hook(skb,dev)== NULL))
		{
			mac_p->macStat.MIB_II.outDiscards++;
			return NETDEV_TX_OK;
		}
#else
		virtual_itf_tx_hook(skb,dev);
#endif
#endif

#ifdef CONFIG_TC3162_ADSL
		isTCConsolePkt(skb);
#endif

		memset(&ethTxMsg, 0, sizeof(ethTxMsg_t));
#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
		samba_short_cut_handler(skb, &ethTxMsg);
#endif
		if(isEN751221)
			ecnt_app_shortcut_handler(skb, &ethTxMsg);
			
		skb = insert_special_tag(&ethTxMsg, skb);
		if(unlikely(skb == NULL))
		{
			mac_p->macStat.MIB_II.outDiscards++;
			return NETDEV_TX_OK;
		}

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
		if(isEN751221)
		{
			if(SmbdTxSpeedOn && skb->smbd_on_speed && offload_eth_fast_tx_hook){
				port_mask = getPortMask(skb);
				offload_eth_fast_tx_hook(skb, getChannelByPortmask(port_mask));
				mac_p->macStat.inSilicon.txEnQueueNum++;
				return NETDEV_TX_OK;
			}
		}
#endif

		if(isEN751221)
		{
		 	if(skb->shortcut_on_speed && offload_eth_fast_tx_hook)
		 	{
		 		port_mask = getPortMask(skb);
		 		offload_eth_fast_tx_hook(skb, getChannelByPortmask(port_mask));
		 		mac_p->macStat.inSilicon.txEnQueueNum++;
		 		return NETDEV_TX_OK;
		 	}
		}

#ifndef LOOPBACK_SUPPORT_JUMBO
		if (skb->len > max_pkt_len)
		{
			printk("skb length more than the PSE port 0 buffer usage \
		    		skb->len:%d, max_pkt_len:%d\n", 
            		skb->len, max_pkt_len);
			dev_kfree_skb_any(skb);
			mac_p->macStat.MIB_II.outDiscards++;
			return NETDEV_TX_OK;
		}
#endif

		/***************** Count the MIB-II *****************/	
		update_sw_mib(mac_p, skb, TX_MIB);
        
		/**************** prepare Tx Msg *****************/ 

		ethTxMsg.raw.fPort = DPORT_GDMA1;  

#ifdef LOOPBACK_SUPPORT
		eth_loopback_tx_hook(&ethTxMsg,skb);
#endif

		/**************** prepare port_info *****************/ 
		memset(&eth_info, 0, sizeof(struct port_info));

#if defined(TCSUPPORT_CPU_EN7580)
		/* in EN7580, tsid=0x7F means no sharping, tsid=0~126 measn sharping */
        if ((wan_conn_type > 0 && wan_conn_type < 4) && strncmp(dev->name, wan_itf_name, strlen(dev->name)) == 0)
		{
			FE_API_GET_METER_IDX(skb, UP_STREAM, &tsid, 0);
			FE_API_GET_ACNT0_IDX(skb, UP_STREAM, &acnt0);
		}else{
        FE_API_GET_METER_IDX(skb, DOWN_STREAM, &tsid, 0);
			FE_API_GET_ACNT0_IDX(skb, DOWN_STREAM, &acnt0);
		}
        eth_info.tsid = tsid;
        ethTxMsg.raw.mtr_g = tsid;
        ethTxMsg.raw.acnt_g0 = acnt0;
        
        FE_API_GET_ACNT1_IDX(skb, DOWN_STREAM, &acnt1);
        ethTxMsg.raw.acnt_g1 = acnt1;
#endif
		eth_info.txq = (txq & 0xf);
		/*eth_info.eth_is_wan = 0;*//*no need*/
		
		/*add for eth_wan*/
		int i = 0;

		for (i = 0; i < 16; i++)
		{
			if (wan_virtualdev_ifname_info[i].valid == 1)
			{
				if (strncmp(dev->name, wan_virtualdev_ifname_info[i].dev_name, strlen(dev->name)) == 0)
				{
					eth_info.eth_is_wan = 1;				
					break;
				}
			}			
		}	
		/*add for eth_wan*/
		
		if(!(SPTAG_INSERT_SUPPORT)) {
			eth_info.eth_macSTagEn = macSTagFlag&0x1;
		}
		eth_info.magic = FOE_MAGIC_GE;
		eth_info.ds_to_qdma = 0;
		if(TCSUPPORT_FWC_QOS_VAL)
		{
			/*copy stag from skb to eth_info*/
			eth_info.stag = getStagBySkb(skb);
			/*if channel 0 ratelimit is enable , default sent to lan qdma channel 0*/
			/*else, port0~4 sent to lan qdma channel 0~4*/
			if( ENABLE == ratelimit_En )
			{
				eth_info.channel = 0;
				eth_info.ds_to_qdma = 1;
			}
			else
			{
				/* Assign Channel by port mask */
				port_mask = getPortMask(skb);
				eth_info.channel = getChannelByPortmask(port_mask);
				/*GE-port, and has been set per-port ratelimit,then go qdma*/
				if( (GE_PORT == eth_info.channel) && (ENABLE == ge_portlimit_En) )
				{
					eth_info.ds_to_qdma = 1;
				}

			}
		}
		else
		{
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CDS)
			downstream_qos_cds(skb, &eth_info);
#else/*TCSUPPORT_COMPILE*/
			/* Assign Channel by port mask */
			port_mask = getPortMask(skb);
			eth_info.channel = getChannelByPortmask(port_mask);
#endif/*TCSUPPORT_COMPILE*/
		}	

		/* decide path by link rate */
		if(eth_info.channel<LIMIT_PORT_NUM)
		{
			if( (lanPortState[eth_info.channel].linkSpeed == 0) || (lanPortState[eth_info.channel].linkSpeed == 1) )
			{ 
				/* link rate 100/10 mbps to Qdma */
				eth_info.ds_to_qdma = 1;
				eth_info.ds_need_offload = 1;
			}
		}
		if(g_force_dstq == 1 || g_force_dstq == 4) 		/* To Gdma */
			eth_info.ds_to_qdma = 0;
		else if(g_force_dstq == 2)  /* To Qdma */
			eth_info.ds_to_qdma = 1;
        else if(g_force_dstq == 3){ /* For TCT stress test: wifi->lan to Qdma, wan->lan to Gdma*/
            if(skb->original_dev){
                if(isWANInterface(skb->original_dev))   //come from wan
                    eth_info.ds_to_qdma = 0;
                else if(isWiFiInterface(skb->original_dev))  //come from wifi
                    eth_info.ds_to_qdma = 1;
            }
        }
		if(checkQueueForSwitchBySkbMark(skb))
			eth_info.ds_to_qdma = 0;

		ethTxMsg.raw.channel = eth_info.channel;
		if(SPTAG_INSERT_SUPPORT) {
			eth_info.stag = ethTxMsg.raw.sp_tag;
			eth_info.nbq = eth_info.channel;
#if defined(TCSUPPORT_CPU_EN7580)		
			ethTxMsg.raw.nboq = eth_info.nbq;
#endif
		}

		qdma_info = &eth_info;
	}/*else for offload Packet */

#ifdef CONFIG_SUPPORT_SELF_TEST
    /* for EN7516 SOC test 20170714 */
    if (g_eth_overwrite_tx_msg)
    {
        ethTxMsg.msg[0] = (ethTxMsg.msg[0] | (0x0102 << 12)); /* sp_tag 0102*/
        ethTxMsg.msg[1] = (ethTxMsg.msg[1] | 0x40064);        /* vlan_tag 100, insert  1*/
    }
#endif

    if(ds_qos && queuemark_enable && qdma_info)
    {
        qdma_info->txq_is_valid = 1;
        qdma_info->txq = getQueueForSwitchbySkbMark(skb);
    }

    if (eth_fast_mode_flag) {
        qdma_info->fast = 1;
    } else {
        qdma_info->fast = 0;
    }

#if defined(TCSUPPORT_SWNAT)
    if(sw_downstream_nat_tx_hook){
        sw_downstream_nat_tx_hook(skb, ethTxMsg.msg[0], ethTxMsg.msg[1], qdma_info);
    }
#endif

#ifdef TCSUPPORT_ECNT_1588v2
	if (unlikely((skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP) || (skb_shinfo(skb)->tx_flags & SKBTX_SW_TSTAMP))) {
		if (ptp_1588_tx_ptk_check_hook) {
			if (!ptp_1588_tx_ptk_check_hook(skb, &ethTxMsg.msg[0], &ethTxMsg.msg[1])) {
				ptp_pkt = 1;
			}
		}
	}
#endif

	#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CF_UNIFIED_PLATFORM)
	ret = ECNT_ETH_SEND_HOOK(&skb);
	if (ECNT_RETURN_DROP == ret)
	{
	    dev_kfree_skb_any(skb);
	    return NETDEV_TX_OK;
	}
	#endif/*TCSUPPORT_COMPILE*/
	
	error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN, skb,
        		ethTxMsg.msg[0], ethTxMsg.msg[1], qdma_info);
	if(unlikely(error))
	{
		dev_kfree_skb_any(skb);
		mac_p->macStat.MIB_II.outDiscards++;
		return NETDEV_TX_OK;
	}

#ifdef TCSUPPORT_ECNT_1588v2
	/* 1588 software timestamp */
	if (ptp_pkt == 1) {
		if (ptp_1588_tx_tstamp_hook) {
			ptp_1588_tx_tstamp_hook(PDEV_GSW_PTP_ID);
		}
	}
#endif

	mac_p->macStat.inSilicon.txEnQueueNum++;

	return NETDEV_TX_OK;
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_ENV)
extern int fhdrv_kdrv_get_cpu_mac(unsigned char *mac, unsigned int len);

/*ge_portlimit_En == 1: means ge-port has been set ratelimit*/
int eth_set_ge_portlimit_switch(unsigned char enable)
{
	ge_portlimit_En = enable ;

	return 0;
}
EXPORT_SYMBOL(eth_set_ge_portlimit_switch);

#endif/*TCSUPPORT_COMPILE*/


/************************************************************************
    Funtcion:       set_mac_addr_from_flash
    Description:    read mac addr from flash and set to switch and FE
    Calls:
    Called by:      eth_lan_mac_start
    Input:
    Output:
    Return:
    Others:     
************************************************************************/

void set_mac_addr_from_flash(struct net_device *dev)
{
    int i;
    struct sockaddr addr;
	uint8 MacAddr[] = {0x00, 0x00, 0xaa, 0xbb, 0xcc, 0xff};

#if	defined TCSUPPORT_CPU_ARMV8 
	if(get_ethaddr(MacAddr, sizeof(MacAddr)) != 0){
		printk("func:%s get ethaddr error!!!\n",__func__);
	}

	for (i=0; i<6; i++) {
        addr.sa_data[i] = MacAddr[i];
    }
#else
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_ENV)
	fhdrv_kdrv_get_cpu_mac((unsigned char *)(addr.sa_data), 6);
#else/*TCSUPPORT_COMPILE*/
    for (i=0; i<6; i++) {
        addr.sa_data[i] = READ_FLASH_BYTE(flash_base + 0xff48 + i);
    }
#endif/*TCSUPPORT_COMPILE*/
#endif
    if(eth_lan_mac_set_macaddr(dev,&addr)){
        printk(KERN_INFO "The MAC address in flash is null!\n");
    }
    printk(KERN_INFO
           "%s: FE MAC Ethernet address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           dev->name, 
           dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
           dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
}


/************************************************************************
    Funtcion:
    Description:    Starting up the ethernet device 
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:     ifconfig add
************************************************************************/
static int eth_lan_mac_start(struct net_device *dev)
{
    set_mac_addr_from_flash(dev);

    mac_p = netdev_priv(dev);

    spin_lock_init(&mac_p->lock);
    spin_lock_init(&phy_lock);

#ifdef TCSUPPORT_CPU_AN7583
	spin_lock_init(&phy_lock_ext0); 
	spin_lock_init(&phy_lock_ext1);
#endif


//#if KERNEL_2_6_36  always 1
#if 1 
    mac_p->dev = dev;
#else
    /* Hook up with handlers */
    dev->get_stats          = tc3262_gmac_stats;
    dev->open               = tc3262_gmac_open;
    dev->stop               = tc3262_gmac_close;
    dev->set_multicast_list = tc3262_gmac_set_multicast_list;
    dev->do_ioctl           = tc3262_gmac_ioctl;
    dev->set_mac_address    = tc3262_gmac_set_macaddr;
    /*dev->weight           = MAC_RXDESCP_NO>>1;*/
    dev->weight             = MAC_NAPI_WEIGHT;  
#if VLAN_TAG_USED
    dev->features |= NETIF_F_HW_VLAN_TX;
#endif

#ifdef RAETH_CHECKSUM_OFFLOAD
    dev->features |= NETIF_F_IP_CSUM;
#endif

#endif /*KERNEL_2_6_36*/
    return 0;
}

/************************************************************************
    Funtcion:       macReset
    Description:    
    Calls:
    Called by:      macInit
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void macReset(void)
{
    unsigned int regVal;
    unsigned char time=0;
#ifdef TCSUPPORT_CPU_ARMV8
	regVal = GET_SCU_RST_CTRL_1();
	regVal |= (ESW_RST);
	SET_SCU_RST_CTRL_1(regVal);
	mdelay(1);	
	regVal &= ~(ESW_RST);
	SET_SCU_RST_CTRL_1(regVal);
#else
    regVal = regRead32(CR_RSTCTRL2);
    regVal |= (ESW_RST);
    regWrite32(CR_RSTCTRL2, regVal);
    mdelay(1);  
    regVal &= ~(ESW_RST);
    regWrite32(CR_RSTCTRL2, regVal);
#endif
    if(use_ext_switch){
	    gswPbusWrite(EXT_GSW_CFG, 0x3);
    }

    /* wait until vlan table init is done */
    do{
        mdelay(1);
        time++;
        regVal = switch_reg_read(GSW_AGC);
    }while((!(regVal & (1<<17))) && time<5);

}

void macSetVip(void)
{
    uint32 reg;

    reg = (0x40<<MFC_BC_FFP_SHIFT) | (0x40<<MFC_UNM_FFP_SHIFT) | (0x40<<MFC_UNU_FFP_SHIFT) |
                MFC_CPU_EN  | (6<<MFC_CPU_PORT_SHIFT);
	switch_reg_write(GSW_MFC, reg);
    
    reg = switch_reg_read(GSW_IMC);
    reg &= ~((1 << MLD_MANG_FR_SHIFT) | (1 << IGMP_MANG_FR_SHIFT));
    reg &= ~((0x7 << IMC_IGMP_QUE_FW_SHIFT) | (0x7 << IMC_IGMP_RPT_FW_SHIFT) | 
        (0x7 << IMC_MLD_QUE_FW_SHIFT) | (0x7 << IMC_MLD_RPT_FW_SHIFT));
    reg |= ((0x6 << IMC_IGMP_QUE_FW_SHIFT) | (0x6 << IMC_IGMP_RPT_FW_SHIFT) |
        (0x6 << IMC_MLD_QUE_FW_SHIFT) | (0x6 << IMC_MLD_RPT_FW_SHIFT));
    switch_reg_write(GSW_IMC, reg);

    reg = switch_reg_read(GSW_APC);
    reg &= ~((1 << PPP_MANG_FR_SHIFT) | (1 << ARP_MANG_FR_SHIFT));
    reg &= ~((0x7 << APC_ARP_PORT_FW_SHIFT) | (0x7 << APC_PPP_PORT_FW_SHIFT));
    reg |= ((0x6 << APC_ARP_PORT_FW_SHIFT) | (0x6 << APC_PPP_PORT_FW_SHIFT));
    switch_reg_write(GSW_APC, reg);

    reg = switch_reg_read(GSW_BPC);
    reg &= ~((1 << PAE_MANG_FR_SHIFT) | (1 << BPDU_MANG_FR_SHIFT));
    reg &= ~((0x7 << BPC_PAE_PORT_FW_SHIFT) | (0x7 << BPC_BPDU_PORT_FW_SHIFT));    
    reg |= ((0x06 << BPC_PAE_PORT_FW_SHIFT) | (0x06 << BPC_BPDU_PORT_FW_SHIFT));
    switch_reg_write(GSW_BPC, reg);

    reg = switch_reg_read(GSW_DPC);
    reg &= ~((1 << DHCP_6_MANG_FR_SHIFT) | (1 << DHCP_4_MANG_FR_SHIFT));
    reg &= ~((0x7 << DPC_DHCP_6_PORT_FW_SHIFT) | (0x7 << DPC_DHCP_4_PORT_FW_SHIFT));    
    reg |= ((0x06 << DPC_DHCP_6_PORT_FW_SHIFT) | (0x06 << DPC_DHCP_4_PORT_FW_SHIFT));
    switch_reg_write(GSW_DPC, reg);
}

/************************************************************************
    Funtcion:       setup_gdma1_len
    Description:    
    Calls:
    Called by:      macInit
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void setup_gdma1_len(void)
{
	unsigned short int long_packet_len, short_packet_len;

	long_packet_len = GDM1_LONG_LEN_VALUE;
	short_packet_len = 60;

#ifdef TCSUPPORT_FTTDP_V2
/* no 4-byte STAG in LAN side when FTTdp mode */
    short_packet_len -= 4;
#endif
	FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA1,long_packet_len,short_packet_len);

	return;
	
}


/************************************************************************
    Funtcion:       macGetAdapterByChanID
    Description:    
    Calls:
    Called by:      macInit
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
macAdapter_t * macGetAdapterByChanID(void)
{
	return mac_p;
}

/************************************************************************
    Funtcion:
    Description:    macInit
    Calls:
    Called by:      eth_lan_mac_open
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void macSetMACCR(macAdapter_t *mac_p)
{
	uint32 reg;

#ifdef RAETH_CHECKSUM_OFFLOAD
	FE_API_SET_DROP_TCP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA1, FE_ENABLE);
	FE_API_SET_DROP_TCP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA1, FE_ENABLE);
	FE_API_SET_DROP_TCP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA1, FE_ENABLE);
#endif
#ifdef TCSUPPORT_FTTDP_V2
/* in FTTdp, packets from PTM_LAN have CRC, but packets from GSW don't have. 
 * We have let (in fe.ko) FE CPORT add CRC to packets from GSW, 
 * so htat packets from PTM_LAN and GSW can both have CRC. 
 * Then, strip thrie CRC in GDM1. */
	FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA1, FE_ENABLE);
#endif
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA1, FE_TYPE_OC, FE_DP_QDMA1_CPU);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA1, FE_TYPE_MC, FE_DP_QDMA1_CPU);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA1, FE_TYPE_BC, FE_DP_QDMA1_CPU);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA1, FE_TYPE_UC, FE_DP_QDMA1_CPU);

	FE_API_SET_PADDING(FE_ENABLE);
	FE_API_SET_INS_VLAN_TPID(FE_CDM_SEL_CDMA1, 0x8100);

	setup_gdma1_len();
	/* check if FPGA */
	if (isFPGA) {
		#ifdef TCSUPPORT_CPU_ARMV8
		reg = GET_SCU_CLK_CFG();
		reg &= ~(0x3f000000);
		reg |= (0x31<<24);
		SET_SCU_CLK_CFG(reg);
		#else
		/* set 1us clock for FPGA */
		reg = read_reg_word(CR_CLK_CFG);
		reg &= ~(0x3f000000);
		reg |= (0x31<<24);
		write_reg_word(CR_CLK_CFG, reg);
		#endif
	}

}

/************************************************************************
    Funtcion:       powerdown_unused_phy
    Description:    
    Calls:
    Called by:      eth_lan_mac_open
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
int powerdown_unused_phy(void)
{
    uint32 phy_mask = 0;
    int i = 0;
    if(isEN7521S)
        phy_mask = 0xf00;   /* fe phy (add 8~11) power down*/
    else if(isEN7526F || isEN7526D || isEN7521F)
        phy_mask = (itf_num==5)? 0:0x100;   /* ge phy (add 8) power down*  when itf_num!=5*/
    else if(isEN7526G)
        phy_mask = (itf_num==5)? 0x1f00:0x1f01;  /* phy (add 0,8~12) power down, if itf_num=5 ,0 up*/
    else if(isEN7513G)
        phy_mask = 0x1f00;  /* phy (add 8~12) power down*/
    else if(isAN7581ST)
	phy_mask = 0x1c00;  /* phy (add 10~12) power down*/
    else
        phy_mask = 0;

        
    if(phy_mask !=0){
        for(i = 0; i < 32 ; i++){
            if( phy_mask & (1<<i) ){
                EPHY_API_POWER_DOWN(i);
            }
        }
    }
    return 0;
}

/************************************************************************
    Funtcion:       macGetMacAddr
    Description:    
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:
************************************************************************/
void macGetMacAddr(macAdapter_t *mac_p)
{
	uint32 i;

	for ( i = 0; i < 6; i++ )
		mac_p->macAddr[i] = def_mac_addr[i];
}

void rgmii_traffic_setting(void){
    interface_map[WAN_TO_LAN] = TRGMII;
    interface_map[WIFI0_RA_TO_LAN] = RGMII;
    interface_map[WIFI1_RAIX_TO_LAN] = RGMII;
}
/************************************************************************
    Funtcion:
    Description:    macInit
    Calls:
    Called by:      eth_lan_mac_open
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
int macInit(void)
{

	if (macInitialized)
		return 0;

    detect_switch();
    rgmii_traffic_setting();
    init_ethernet_port_map();

    /* disable management frames and set forward port */
    macSetVip();
        
	/* ----- Get Mac Adapter from dummy data or NDIS control block ----- */
	mac_p = macGetAdapterByChanID();

	/* ----- Assign reserved data pointer ----- */
	mac_p->macPhyLinkProfile_p = &enetPhyLinkProfile;
	mac_p->statisticOn = MAC_STATISTIC_ON;
	/* ----- Get the Mac address ----- */
	macGetMacAddr(mac_p);

	/* ----- Initialize Frame Engine Registers ----- */
    macSetMACCR(mac_p);

	/* ----- Initialize Switch Registers ----- */
    macSetGSW(mac_p);

    mt7530_switch_init();
	
	if(support_external_switch())
	{
		external_switch_init();
	}

	#if (defined(TCSUPPORT_CPU_EN7523) && defined(TCSUPPORT_AUTOBENCH))
	/*disable mac rx/tx sptag function*/
	write_reg_word(GDMA1_VLAN_CHECK, 1);
	#endif
    
	macInitialized = 1;
	
	return 0;
}

/************************************************************************
    Funtcion:       eth_timer_init
    Description:    init timer for eth polling task
    Calls:
    Called by:      eth_lan_mac_open
    Input:
    Output:
    Return:
    Others:
************************************************************************/
static void adsl_poll_func(TIMER_FUN_PAAM data)
{
#ifdef CONFIG_TC3162_ADSL
	uint8 modemst;

	if (adsl_dev_ops == NULL)
		goto down_proc;

	adsl_dev_ops->query(ADSL_QUERY_STATUS, &modemst, NULL);		  
	switch (modemst) {
		case ADSL_MODEM_STATE_DOWN:
down_proc:
			mod_timer(&eth_poll_timer, jiffies + msecs_to_jiffies(500));
			break;
		case ADSL_MODEM_STATE_WAIT_INIT:
			mod_timer(&eth_poll_timer, jiffies + msecs_to_jiffies(100));
			break;
		case ADSL_MODEM_STATE_INIT:
			mod_timer(&eth_poll_timer, jiffies + msecs_to_jiffies(100));
			break;
		case ADSL_MODEM_STATE_UP:
			mod_timer(&eth_poll_timer, jiffies + msecs_to_jiffies(500));
			break;
        }
#endif		
}

/************************************************************************
    Funtcion:       eth_timer_init
    Description:    init timer for eth polling task
    Calls:
    Called by:      eth_lan_mac_open
    Input:
    Output:
    Return:
    Others:
************************************************************************/
static int eth_timer_init(void)
{
#ifndef MT7505_PQA_SWITCH_TEST
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    /* Schedule timer for monitoring link status */
    init_timer(&eth_timer);
    eth_timer.expires = jiffies + msecs_to_jiffies(250);
  	eth_timer.function = eth_monitor;
    eth_timer.data = 0;
#else
	timer_setup(&eth_timer,eth_monitor,0);
	eth_timer.expires = jiffies + msecs_to_jiffies(250);
#endif
    add_timer(&eth_timer);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    /* Schedule timer for monitoring link status */
    init_timer(&eth_poll_timer);
    eth_poll_timer.expires = jiffies + msecs_to_jiffies(500);
  	eth_poll_timer.function = adsl_poll_func;
    eth_poll_timer.data = 0;
#else
	timer_setup(&eth_poll_timer,adsl_poll_func,0);
	eth_poll_timer.expires = jiffies + msecs_to_jiffies(500);
#endif
    add_timer(&eth_poll_timer);
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    eth_led_init();
#endif
    return 0;
}

/************************************************************************
    Funtcion:       eth_timer_init
    Description:    delete timer for eth polling task
    Calls:
    Called by:      eth_lan_mac_open
    Input:
    Output:
    Return:
    Others:
************************************************************************/

static int eth_timer_deinit(void)
{
#ifndef MT7505_PQA_SWITCH_TEST
     /* Kill timer */
    del_timer_sync(&eth_timer);
    del_timer_sync(&eth_poll_timer);
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    eth_led_deinit();
#endif
    return 0;
}


/************************************************************************
    Funtcion:
    Description:    Starting up the ethernet device 
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:     ifconfig eth up
************************************************************************/
static int eth_lan_mac_open(struct net_device *dev)
{
    int err;
    uint32 value = 0;
    printk(KERN_INFO "%s: starting interface.\n", dev->name);
    /* MII setup */
    mac_p->mii_if.phy_id = mac_p->enetPhyAddr;
    mac_p->mii_if.full_duplex = 1;
    mac_p->mii_if.phy_id_mask = 0x1f;
    mac_p->mii_if.reg_num_mask = 0x1f;
    mac_p->mii_if.dev = dev;
    mac_p->mii_if.mdio_read = mdio_read;
    mac_p->mii_if.mdio_write = mdio_write;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    mac_p->mii_if.supports_gmii = mii_check_gmii_support(&mac_p->mii_if);
#endif

    napi_enable(&mac_p->napi);
    netif_start_queue(dev);
    value = read_reg_word(GSW_PMCR(6));
    value |= LINK_UP;
    write_reg_word(GSW_PMCR(6), value);
    printk("netif_start_queue\n");
#if defined(TCSUPPORT_AUTOBENCH)
#else
    powerdown_unused_phy();
#endif
    eth_timer_init();

	//enable gdm3(XSI_PCIE0_IDX=0), gdm3(XSI_PCIE1_IDX=1), gdm4(XSI_USB_IDX=2), gdm4(XSI_ETH_IDX=4) befote open napi.
	FE_API_SET_GDMA_ENABLE(0);
	FE_API_SET_GDMA_ENABLE(1);
	FE_API_SET_GDMA_ENABLE(2);
	FE_API_SET_GDMA_ENABLE(4);
	FE_API_SET_GDMA_ENABLE(5);
	
    return 0;
}

/************************************************************************
    Funtcion:
    Description:    Stopping the ethernet device
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:     ifconfig down
************************************************************************/
static int eth_lan_mac_close(struct net_device *dev)
{
    
    uint32 value = 0, i = 0, ret = 0;

    value = read_reg_word(GSW_PMCR(6));
    value &= LINK_DOWN;
    write_reg_word(GSW_PMCR(6), value);
    netif_stop_queue(dev);

	//close gdm3(XSI_PCIE0_IDX=0), gdm3(XSI_PCIE1_IDX=1), gdm4(XSI_USB_IDX=2), gdm4(XSI_ETH_IDX=4) after stop napi.
	FE_API_SET_GDMA_DISABLE(0);
	FE_API_SET_GDMA_DISABLE(1);
	FE_API_SET_GDMA_DISABLE(2);
	FE_API_SET_GDMA_DISABLE(4);
	FE_API_SET_GDMA_DISABLE(5);

	while (i++ < 5000) {
		mdelay(1);
		ret = QDMA_API_CHECK_DSCP_IS_FREE(ECNT_QDMA_LAN);
		if(!ret)
			break;
	}
	
    napi_disable(&mac_p->napi);
    printk("netif_stop_queue\n");
    eth_timer_deinit();
    return 0;
}


/************************************************************************
    Funtcion:
    Description:    Get the stats information
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:
************************************************************************/
static struct net_device_stats *eth_lan_mac_stats(struct net_device *dev)
{
	struct net_device_stats *stats;

	stats = &mac_p->stats;
	if(isEN7526c){
		return stats;
	}
		
	stats->rx_packets = calcnt(GSW_RX_UNIC(0)) + calcnt(GSW_RX_MULC(0));
	stats->tx_packets = calcnt(GSW_TX_UNIC(0)) + calcnt(GSW_TX_MULC(0));
	stats->rx_bytes = calcnt(GSW_RX_OCL(0));
	stats->tx_bytes = calcnt(GSW_TX_OCL(0));
	stats->rx_dropped = calcnt(GSW_RX_DROC(0));
	stats->tx_dropped = calcnt(GSW_TX_DROC(0));
	stats->multicast = calcnt(GSW_RX_MULC(0));
	stats->rx_errors = calcnt(GSW_RX_ALIGE(0)) + calcnt(GSW_RX_CRC(0)) + 
                        calcnt(GSW_RX_RUNT(0)) + calcnt(GSW_RX_FRGE(0)) + 
                        calcnt(GSW_RX_LONG(0));
	stats->tx_errors = calcnt(GSW_TX_COLC(0));
	stats->collisions = calcnt(GSW_TX_COLC(0));
	return stats;
}




static struct rtnl_link_stats64 * eth_lan_mac_stats_64(struct net_device* dev, struct rtnl_link_stats64* stats)
{


	stats->rx_packets = (**(Expand_64_Rx + 2)<<32) +calcnt(GSW_RX_UNIC(0)) + (**(Expand_64_Rx + 3)<<32) +calcnt(GSW_RX_MULC(0));
	stats->tx_packets = (**(Expand_64_Tx + 2)<<32 )+ calcnt(GSW_TX_UNIC(0)) + (*(*(Expand_64_Rx + 3) + 6)<<32) + read_reg_word(GSW_RX_MULC(6));
	stats->rx_bytes = (**(Expand_64_Rx + 12)<<32) + calcnt(GSW_RX_OCL(0));
	stats->tx_bytes = (**(Expand_64_Tx + 12)<<32) + calcnt(GSW_TX_OCL(0));
	stats->rx_dropped = (**Expand_64_Rx << 32) + calcnt(GSW_RX_DROC(0));
	stats->tx_dropped = (**Expand_64_Tx <<32) + calcnt(GSW_TX_DROC(0));
	stats->multicast = (**(Expand_64_Rx + 3)<<32 )+ calcnt(GSW_RX_MULC(0));
	stats->rx_errors = (**(Expand_64_Rx + 5)<<32) + calcnt(GSW_RX_ALIGE(0)) + (**(Expand_64_Rx + 6)<<32) +  calcnt(GSW_RX_CRC(0)) + 
                        (**(Expand_64_Rx + 7)<<32) + calcnt(GSW_RX_RUNT(0)) + (**(Expand_64_Rx + 8)<<32 )+ calcnt(GSW_RX_FRGE(0)) + 
                        (**(Expand_64_Rx + 9)<<32) + calcnt(GSW_RX_LONG(0));
	stats->tx_errors = (**(Expand_64_Tx + 5)<<32) + calcnt(GSW_TX_COLC(0));
	stats->collisions = (**(Expand_64_Tx + 5)<<32 )+ calcnt(GSW_TX_COLC(0));
	
	return stats;
}

int eth_dstq_write(int value)
{
	if( (value<0) || (value>4)){
		printk("Input Value [0/1/2/3/4]\n");
	}else{
		if(g_force_dstq != value){
			g_force_dstq = value;
			perChannelRateLimitSwitch();
		}
	}
	return 0;
}

int xsi_dstq_write(int xsi_serdes, int mode)
{
	if( (mode<0) || (mode>2)){
		printk("Input Value [0/1/2]\n");
	}
	else{
		if(xsi_serdes == 3)
		{
			if(pcie0_force_dstq != mode)
				pcie0_force_dstq = mode;
		}
		if(xsi_serdes == 4)
		{
			if(pcie1_force_dstq != mode)
				pcie1_force_dstq = mode;
		}
		if(xsi_serdes == 5)
		{
			if(usb_force_dstq != mode)
				usb_force_dstq = mode;
		}
		if(xsi_serdes == 6)
		{
			if(eth_force_dstq != mode)
				eth_force_dstq = mode;
		}
	}
	return 0;
}

int eth_link_aggregation_enable(int enable)
{
    link_aggr_en = enable;

    virtual_itf_link_aggr_config(enable);

    if(ra_sw_nat_hook_clean_table)
        ra_sw_nat_hook_clean_table();
    
    return 0;
}

int eth_gsw_fc_write(int value)
{
	uint32 reg;

	if( (value<0) || (value>1)){
		printk("Input Value [0/1]\n");
	}else{
		if(use_ext_switch){
			reg = read_reg_word(GSW_PMCR(5));
			if( value==0){
				/* fc disable */
				reg &= (~ (ENABLE_RX_FC_PN | ENABLE_TX_FC_PN));
			}
			else{
				/* fc enable */
				reg = reg | ENABLE_RX_FC_PN | ENABLE_TX_FC_PN;
			}
			/* internal switch port 5 */
			write_reg_word(GSW_PMCR(5), reg);
			/* external switch port 6 */
			gswPbusWrite(0x3600, reg);
		}else{
			reg = read_reg_word(GSW_PMCR(6));
			if( value==0){
				/* fc disable */
				reg &= (~ (ENABLE_RX_FC_PN | ENABLE_TX_FC_PN));
			}
			else{
				/* fc enable */
				reg = reg | ENABLE_RX_FC_PN | ENABLE_TX_FC_PN;
			}
			/* internal switch port 6 */
			write_reg_word(GSW_PMCR(6), reg);
		}
	}
	return 0;
}

int eth_ds_qos_write(int value)
{
    if( (value<0) || (value>2)){
		printk("Input Value [0/1]\n");
	}else{
		ds_qos = QOS_IN_SWITCH;
		if(set_qdma_lan_qos_buffer_mode_hook)
			set_qdma_lan_qos_buffer_mode_hook(value);
	}
	return 0;
}

int check_reg_valid(uint32 reg)
{
	uint32 reg_offset = 0;
	uint32 ret_val = 0;

	reg_offset = reg % 4;
	if(reg_offset != 0){
		ret_val = 0;
	}else{
		ret_val = 1;
	}
	#ifdef TCSUPPORT_CPU_ARMV8
	//0xbfb000f8 and  0xbfb00064 are useless in arm cpu
	if(reg == 0xbfb000f8 || reg == 0xbfb00064)
		ret_val = 0; 
	#endif

	
	return ret_val;
}

/************************************************************************
    Funtcion:
    Description:    Handling ioctl call
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:
************************************************************************/
int eth_lan_mac_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	int rc = 0;
	gsw_reg reg;
	struct mii_ioctl_data mii;
#if defined(TCSUPPORT_SEPERATED_ETH_ITF)	
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	int portmap[8] ={-1};	
#else
	int portmap[4] ={-1};
#endif

	uint8 port;
	uint8 value;
    uint32 switch_phy_addr=0;
	int i;
#endif /* TCSUPPORT_SEPERATED_ETH_ITF */
	if (macInitialized) {
		switch (cmd) {
			case RAETH_REG_READ:
				copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
				if(check_reg_valid(reg.off)){
					#ifdef TCSUPPORT_CPU_ARMV8
					if(reg.off == 0xbfb00834)
						reg.val = GET_SCU_RST_CTRL_1();
					else
						reg.val = read_reg_word(reg.off);
					#else
						reg.val = read_reg_word(reg.off);
					#endif
					copy_to_user(ifr->ifr_data, &reg, sizeof(reg));
				}
				break;

			case RAETH_REG_WRITE:
				copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
				if(check_reg_valid(reg.off)){
					#ifdef TCSUPPORT_CPU_ARMV8
					if(reg.off == 0xbfb00834)
						SET_SCU_RST_CTRL_1(reg.val);
					else
						write_reg_word(reg.off, reg.val);
					#else
						write_reg_word(reg.off, reg.val);
					#endif
				}
				break;

			case RAETH_GSW_REG_READ:
				copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
				if(check_reg_valid(reg.off)){
					reg.val = read_reg_word(GSW_BASE + reg.off);
					copy_to_user(ifr->ifr_data, &reg, sizeof(reg));
				}
				break;
			case RAETH_GSW_REG_WRITE:
				copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
				if(check_reg_valid(reg.off)){
					write_reg_word(GSW_BASE + reg.off, reg.val);
				}
				break;

			case RAETH_GSWEXT_REG_READ:  /*MTK120625 ///YM*/
				copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
                if(reg.switch_id == 0)
                    reg.val = read_reg_word(GSW_BASE + reg.off);
                else{
#if defined (TCSUPPORT_MULTI_SWITCH_EXT)
                    switch(reg.switch_id){
                        case 1:
                            switch_phy_addr = GSW_PHY_OFFSET_31;
                            break;
                        case 2:
                            switch_phy_addr = GSW_PHY_OFFSET_23;
                            break;
                        case 3:
                            switch_phy_addr = GSW_PHY_OFFSET_15;
                            break;                            
                    }
                    reg.val = gswPbusRead_extend(reg.off, switch_phy_addr);
#else
                    reg.val = gswPbusRead(reg.off);
#endif
                }
				copy_to_user(ifr->ifr_data, &reg, sizeof(reg));
				break;

			case RAETH_GSWEXT_REG_WRITE:  /*MTK120625 ///YM*/
				copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
                if(reg.switch_id == 0)
                    write_reg_word(GSW_BASE + reg.off, reg.val);
                else{
#if defined (TCSUPPORT_MULTI_SWITCH_EXT)
                    switch(reg.switch_id){
                        case 1:
                            switch_phy_addr = GSW_PHY_OFFSET_31;
                            break;
                        case 2:
                            switch_phy_addr = GSW_PHY_OFFSET_23;
                            break;
                        case 3:
                            switch_phy_addr = GSW_PHY_OFFSET_15;
                            break;                            
                            
                    }
                    gswPbusWrite_extend(reg.off, reg.val, switch_phy_addr);
#else
                    gswPbusWrite(reg.off, reg.val);
#endif
                }

				break;

            case RAETH_GSWEXT2_REG_READ:
                copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
                reg.val = gswPbusRead_extend(reg.off, GSW_PHY_OFFSET_23);
                copy_to_user(ifr->ifr_data, &reg, sizeof(reg));
                break;

            case RAETH_GSWEXT2_REG_WRITE:
                copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
                gswPbusWrite_extend(reg.off, reg.val, GSW_PHY_OFFSET_23);
                break;

			case RAETH_GSW_PHY_READ:
				copy_from_user(&mii, ifr->ifr_data, sizeof(mii));
				mii.val_out = tcMiiStationRead(mii.phy_id, mii.reg_num);
				copy_to_user(ifr->ifr_data, &mii, sizeof(mii));
				break;

			case RAETH_GSW_PHY_WRITE:
				copy_from_user(&mii, ifr->ifr_data, sizeof(mii));
				tcMiiStationWrite(mii.phy_id, mii.reg_num, mii.val_in);
				break;

			case RAETH_GSWEXT_PHY_READ:  /*MTK120625 ///YM*/
				copy_from_user(&mii, ifr->ifr_data, sizeof(mii));
				mii.val_out = gswPmiRead(mii.phy_id, mii.reg_num);
				copy_to_user(ifr->ifr_data, &mii, sizeof(mii));
				break;

			case RAETH_GSWEXT_PHY_WRITE:  /*MTK120625 ///YM*/
				copy_from_user(&mii, ifr->ifr_data, sizeof(mii));
				gswPmiWrite(mii.phy_id, mii.reg_num, mii.val_in);
				break;
		#ifdef TCSUPPORT_MT7530_SWITCH_API
			case RAETH_GSW_CTLAPI:
                /* MT7530 switch*/
				if (swicVendor == SWIC_MT7530)	
				{
					macMT7530gswAPIDispatch(ifr);
				}
				break;
		#endif
#if defined(TCSUPPORT_SEPERATED_ETH_ITF)		
		/* Set port map dynamically  for FHT*/
			case RAETH_GSW_SETPORTMAP:
#if defined(TCSUPPORT_CF_ENV) && defined(TCSUPPORT_CPU_EN7521) && defined(TCSUPPORT_WAN_ETHER)
				if ( -1 != wan_port_id )
					break;
#endif
				copy_from_user(portmap, ifr->ifr_data, sizeof(portmap));
				memset(lan_port_map, -1, MAX_ETH_ITF_NUM*sizeof(char));
				memset(switch_port_map, -1, RT63365_SWIC_PORTNUM*sizeof(char));
				lan_port_map[0]= portmap[0];
				lan_port_map[1]= portmap[1];
				lan_port_map[2]= portmap[2];
				lan_port_map[3]= portmap[3];
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
				lan_port_map[4]= portmap[4];
				lan_port_map[5]= portmap[5];
				lan_port_map[6]= portmap[6];
				lan_port_map[7]= portmap[7];
#endif
				for (port=0; port<MAX_ETH_ITF_NUM; port++)

				{
					value = lan_port_map[port];
					if (value < RT63365_SWIC_PORTNUM)
					{
						switch_port_map[value] = port;
					}
				}
				break;
#endif /* TCSUPPORT_SEPERATED_ETH_ITF */	
			case RAETH_DSTQ:
				copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
				if(reg.off == 0){
					eth_dstq_write(reg.val);
				}else if(reg.off == 1){
					eth_gsw_fc_write(reg.val);
				}else if(reg.off == 2){
					eth_ds_qos_write(reg.val);
				}else if((reg.off >= 3) && (reg.off <= 6)){
					xsi_dstq_write(reg.off, reg.val);
				}else{
					printk("error dstq type:%x\n", reg.off);
				}
				break; 
                        case RAETH_LINK_AGGREGATION:             
                            copy_from_user(&reg, ifr->ifr_data, sizeof(reg));
                            if(reg.off)
                            {
                                eth_link_aggregation_enable(reg.val);
                                printk("%s link aggregation\n",reg.val?"Enable":"Disable");
                            }
                            else
                            {
                                printk("link aggregation is %s\n",link_aggr_en?"Enable":"Disable");
                            }
                            break;
			default:
				copy_from_user(&mii, ifr->ifr_data, sizeof(mii));
				rc = generic_mii_ioctl(&mac_p->mii_if, &mii, cmd, NULL);
				copy_to_user(ifr->ifr_data, &mii, sizeof(mii));
				break;
		}
	} else {
		rc = -EOPNOTSUPP;
	}
	return rc;
}

/************************************************************************
    Funtcion:   my_eth_change_mtu
    Description:    
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:
************************************************************************/
#ifdef TCSUPPORT_MAX_PACKET_2000
int my_eth_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu < 68 || new_mtu > 2000)
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0;
}
#endif

void mac_limit_clear(int portId)
{
    uint32 value, extsw_flag = 0;

    if(portId & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	portId = portId & SWITCH_PORT_MASK;

    value = gswRegRead_extend(GSW_PSC(portId), extsw_flag);
    gswRegWrite_extend(GSW_PSC(portId),0xfff00, extsw_flag);
    value &= ~(0xfff << SA_LRN_CNT);
    gswRegWrite_extend(GSW_PSC(portId),value, extsw_flag);
}
static void eth_dev_check_linkstate(int port_id, int port_linkstate)
{

	struct net_device *dev ;
	int lan_port;
#if defined(TCSUPPORT_WAN_ETHER)
    if(wan_port_id == port_id)
    {
		lan_port = port_id;
		if(wan_dev) {
			if(port_linkstate){
				if(!netif_carrier_ok(wan_dev))
					netif_carrier_on(wan_dev);
			} else {
				if(netif_carrier_ok(wan_dev))
					netif_carrier_off(wan_dev);
			}
		}
    }
    else
#endif
    {
	lan_port = switch_port_map[port_id];
    }
	if(0 <= lan_port && lan_port < MAX_ETH_ITF_NUM )	
	{	 
		dev=lan_virtual_dev[lan_port];
		if(dev) {
			if(port_linkstate){
				if(!netif_carrier_ok(dev))
					netif_carrier_on(dev);
			} else{
				if(netif_carrier_ok(dev))
					netif_carrier_off(dev);
			}
		}
			
	}

	return;
}

static void eth_send_event_down(int port_id)
{
	unsigned long type = 0;
	struct eth_event_data event_para;
	struct net_device *dev ;

    memset(&event_para, 0x0, sizeof(event_para));
#if defined(TCSUPPORT_WAN_ETHER)
	if(wan_port_id == port_id)
	{
		type = ((ECNT_EVENT_ETH<<8) | ECNT_EVENT_ETH_WAN_DOWN);
		event_para.lan_port = port_id;
	}
    else
#endif
    {
	type = ((ECNT_EVENT_ETH<<8) | ECNT_EVENT_ETH_DOWN);
	event_para.lan_port = switch_port_map[port_id];
    }
	ecnt_send_event(type, &event_para, sizeof(event_para));
#if !defined (CONFIG_TP_IMAGE)
	if(0 <= event_para.lan_port && event_para.lan_port < MAX_ETH_ITF_NUM ) 
	{	  
		dev=lan_virtual_dev[event_para.lan_port];
		if(!dev)
		return ;
		dev->flags &= ~IFF_UP;

	}
#endif /* !defined (CONFIG_TP_IMAGE) */
	if(hwnat_clean_lan_hook)
		hwnat_clean_lan_hook(event_para.lan_port);
		
	return;
}

static void eth_send_event_up(int port_id, int speed, int duplex)
{
	unsigned long type = 0;
	struct eth_event_data event_para;
	struct net_device *dev ;

    memset(&event_para, 0x0, sizeof(event_para));
#if defined(TCSUPPORT_WAN_ETHER)
    if(wan_port_id == port_id)
    {
        type = ((ECNT_EVENT_ETH<<8) | ECNT_EVENT_ETH_WAN_UP);
        event_para.lan_port = port_id;
    }
    else
#endif
    {
	type = ((ECNT_EVENT_ETH<<8) | ECNT_EVENT_ETH_UP);
	event_para.lan_port = switch_port_map[port_id];
    }

	event_para.link_speed = speed;
	event_para.link_duplex = duplex?1:0;
	ecnt_send_event(type, &event_para, sizeof(event_para));
#if !defined (CONFIG_TP_IMAGE)
	if(0 <= event_para.lan_port && event_para.lan_port < MAX_ETH_ITF_NUM )	
	{	 
		dev=lan_virtual_dev[event_para.lan_port];
		if(!dev)
		return ;
		dev->flags |= IFF_UP; 
		
	}
#endif /* !defined (CONFIG_TP_IMAGE) */

	return;
}

/************************************************************************
    Funtcion:		eth_force_linkup
    Description:    Force port link up according to port link status
    Called by:      gswLanLinkSt_monitor
    Others:
************************************************************************/
static inline int eth_force_linkup(int portId, int is_extend_gsw, uint32 value1)
{
	uint32 value = 0, value2 = 0;
	
	value2 = gswRegRead_extend(GSW_MAC_PAUSE_CAP(portId), is_extend_gsw);
	value2 = (value2 & (0x3<<(portId*2)))>>(portId*2);

	value1 = (value1&0x1) | ((value1&(0x3<<1))<<1) | ((value1&(0x1<<3))>>2) | ((value1&(0x3<<5))<<1);
	value = gswRegRead_extend(GSW_PMCR(portId), is_extend_gsw) & (~0xFF) | value1 | (value2<<4);
	gswRegWrite_extend(GSW_PMCR(portId), value, is_extend_gsw);

	return 0;
}

/************************************************************************
    Funtcion:		eth_force_linkdown
    Description:    If phy turns down :
    				1. disable mac tx;
    				2. release force mode;
    				3. enable mac tx				
    Called by:      gswLanLinkSt_monitor
    Return:			0: up->up	1: up->down
    Others:
************************************************************************/
static inline int eth_force_linkdown(int portId, int is_extend_gsw)
{
	uint32 value = 0;

	value = gswRegRead_extend(GSW_PMCR(portId), is_extend_gsw) & ~(0x1<<14);
	gswRegWrite_extend(GSW_PMCR(portId), value, is_extend_gsw);

	mdelay(2);

	value = gswRegRead_extend(GSW_PMCR(portId), is_extend_gsw) & ~(0x1) | (0x1<<14);
	gswRegWrite_extend(GSW_PMCR(portId), value, is_extend_gsw);

	mdelay(2);

	return 0;
}

static inline void eth_check_phy_state(int portId, int is_extend_gsw)
{
	uint32 value = 0;
	int offset = (portId % 4) * 8;
	static uint32 up_flag = 0;
	int up_flag_port_mask = 1<<(is_extend_gsw?(portId+8):portId);
	
	value = gswRegRead_extend(GSW_PHY_STAUS(portId), is_extend_gsw);
	value = (value & (0xFF<<offset))>>offset;

	if((value & PHY_LINKUP) && !(up_flag & up_flag_port_mask))
	{
		eth_force_linkup(portId, is_extend_gsw, value);
		up_flag |= up_flag_port_mask;
	}
	else if(!(value & PHY_LINKUP) && (up_flag & up_flag_port_mask))
	{
		eth_force_linkdown(portId, is_extend_gsw);
		up_flag &= ~up_flag_port_mask;
	}

	return;
}

static void eth_lan_port_init(void)
{
	int portId;
	uint32 value = 0;

	for(portId=1; portId<5; portId++)
	{
		value = gswRegRead_extend(GSW_PMCR(portId), 0) | (0x3<<14);
		value &= ~0x1;
		gswRegWrite_extend(GSW_PMCR(portId), value, 0);
	}

	for(portId=0; portId<4; portId++)
	{
		value = gswRegRead_extend(GSW_PMCR(portId), 1) | (0x3<<14);
		value &= ~0x1;
		gswRegWrite_extend(GSW_PMCR(portId), value, 1);
	}

	return;
}

void gswLanLinkSt_monitor(void)
{
	uint32 reg;
	int portId;
	int speed;
	int rateValue[4] = {10, 100, 1000, 10000};	/* unit: mbps */
	int orgLinkState = 0, newLinkState=0;
	int duplex = 0;
#if defined(CONFIG_TP_IMAGE) && defined(TCSUPPORT_SEPERATED_ETH_ITF)
	unsigned int lanID = 0;
	struct net_device *dev = NULL ;
	char ifname[IFNAMSIZ] = {0};
#endif /* CONFIG_TP_IMAGE && TCSUPPORT_SEPERATED_ETH_ITF */

	QDMA_TxRateLimitSet_T txRateLimitSet;
	QDMA_TxRateLimitGet_T txRateLimitGet;

	if (!macInitialized) {
		return;
	}

	rateValue[1]+=LIMIT_BUFFER_FOR_100MB;
	rateValue[2]+=LIMIT_BUFFER_FOR_1GB;

	for (portId = 0; portId < LIMIT_PORT_NUM; portId++) {
		if(!MAC_AUTO_POLLING_BY_PHY)
		{
			if(portId > 0 && portId < 5)
			{
				eth_check_phy_state(portId, 0);
			}
		}
		reg = switch_reg_read(GSW_PMSR(portId));
		if(lanPortState[portId].linkState ^ (reg & MAC_LINK_STS))
			macMT7530CleanMactblByPort(portId);
		orgLinkState = lanPortState[portId].linkState;
		lanPortState[portId].linkState = (reg & MAC_LINK_STS);
		eth_dev_check_linkstate(portId,lanPortState[portId].linkState);
#if defined(CONFIG_TP_IMAGE) && defined(TCSUPPORT_SEPERATED_ETH_ITF)
		lanID = 0xff;
		if (0 <= portId && portId < RT63365_SWIC_PORTNUM - 1)
		{
			lanID = (unsigned int)switch_port_map[portId];
		}
		/******** virtual interface eth0.1~4 assign to dev ********/
		snprintf(ifname, sizeof(ifname), "%s%d", sep_itf, lan_port_map[lanID]);
		dev = dev_get_by_name(&init_net, ifname);
#endif /* CONFIG_TP_IMAGE && TCSUPPORT_SEPERATED_ETH_ITF */
		if (!(reg & MAC_LINK_STS)) {
			/* Down */
			if(UP == orgLinkState){
				mac_limit_clear(portId);        //clear learned source address number
#if defined(CONFIG_TP_IMAGE) && defined(TCSUPPORT_SEPERATED_ETH_ITF)
				if (dev != NULL)
				{
					netif_carrier_off(dev);
				}
#endif /* CONFIG_TP_IMAGE && TCSUPPORT_SEPERATED_ETH_ITF */
				eth_send_event_down(portId);
            }
			continue;
		}
		speed = (reg & MAC_SPD_STS) >> MAC_SPD_STS_SHIFT;

		if( DOWN == orgLinkState){
			duplex = (reg & MAC_DPX_STS);
#if defined(CONFIG_TP_IMAGE) && defined(TCSUPPORT_SEPERATED_ETH_ITF)
			if (dev != NULL)
			{
				netif_carrier_on(dev);
			}
#endif /* CONFIG_TP_IMAGE && TCSUPPORT_SEPERATED_ETH_ITF */
			eth_send_event_up(portId, speed, duplex);
		}
		
		if(lanPortState[portId].linkSpeed != speed){
			/* chnlRateLimitEn keeps original setting  */
			txRateLimitGet.chnlIdx = portId;
			QDMA_API_GET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitGet);
			txRateLimitSet.chnlRateLimitEn = txRateLimitGet.chnlRateLimitEn;

			txRateLimitSet.chnlIdx = portId;
			txRateLimitSet.rateLimitValue = (rateValue[speed] * 1000 ); 	/* unit: kbps */
			QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitSet);

			lanPortState[portId].linkSpeed = speed;
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
			if((USE_QDMA_WAN_ETHER == g_wan_mode) && (portId == wan_port_id)){
				txRateLimitSet.chnlRateLimitEn = 1;
				if(SUPPORT_GDM_SPTAG_ACTION){
					txRateLimitSet.chnlIdx = 0;
				}else{
					txRateLimitSet.chnlIdx = portId;
				}
				txRateLimitSet.rateLimitValue = (rateValue[speed] * 1000 ); 	/* unit: kbps */
				QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_WAN, &txRateLimitSet);
			}
#endif
		}
	}

#if defined(CONFIG_TP_IMAGE) && defined(TCSUPPORT_SEPERATED_ETH_ITF)
	if (dev)
	{
		dev_put(dev);
	}
#endif /* CONFIG_TP_IMAGE && TCSUPPORT_SEPERATED_ETH_ITF */

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        for (portId = 0; portId < LIMIT_PORT_NUM; portId++) {
			if(portId >= 0 && portId < 4)
				eth_check_phy_state(portId, 1);
			
            reg = gswRegRead_extend(GSW_PMSR(portId), 1);
            newLinkState = reg & MAC_LINK_STS;
            orgLinkState = lanPortStateExt[portId].linkState;
            
            speed = (reg & MAC_SPD_STS) >> MAC_SPD_STS_SHIFT;
            lanPortStateExt[portId].linkSpeed = speed;
            
            /* if link state changes*/
            if(orgLinkState ^ newLinkState){
                macMT7530CleanMactblByPort(portId | EXT_SWITCH_FLAG);
                lanPortStateExt[portId].linkState = newLinkState;
            }
            /* clear learned source address number when up to down*/
            if ((newLinkState == DOWN) && (orgLinkState == UP) ) {
                mac_limit_clear(portId | EXT_SWITCH_FLAG);        
                continue;
            }
        }
    }
#endif

	return;
}

/************************************************************************
    Funtcion:
    Description:    Setting customized mac address
    Calls:
    Called by:      gmac_netdev_ops
    Input:
    Output:
    Return:
    Others:
************************************************************************/
static int eth_lan_mac_set_macaddr(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;
        unsigned int mac_h = 0,mac_lmin = 0,mac_lmax = 0;

	/* Check if given address is valid ethernet MAC address */
  	if (!is_valid_ether_addr(addr->sa_data))
    	return(-EIO);

	/* Save the customize mac address */
  	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
  	memcpy(def_mac_addr, addr->sa_data, dev->addr_len);

  	if(isEN751627later){
  	  	mac_h = (def_mac_addr[0]<<16) | (def_mac_addr[1]<<8) | def_mac_addr[2];
  	  	mac_lmin = (def_mac_addr[3]<<16) | (def_mac_addr[4]<<8) | def_mac_addr[5];
  	  	mac_lmax = mac_lmin;
   	  	FE_API_SET_MAC_ADDR_7516(FE_MAC_SET_LAN,mac_h,mac_lmin,mac_lmax);
  	}else{
  	  	FE_API_SET_MAC_ADDR(FE_GDM_SEL_GDMA1, def_mac_addr, 0xf8);
  	}
    return 0; /* Do nothing */
}

/************************************************************************
    Funtcion:       GetMacAddr
    Description:    
    Calls:
    Called by:      hwnat.ko
    Input:
    Output:
    Return:
    Others:
************************************************************************/
uint8 *GetMacAddr(void)
{
	static uint8 macAddr[7];
  
  	memcpy(macAddr, def_mac_addr, 6);
	macAddr[6] = 0x0;
	return macAddr;
}
EXPORT_SYMBOL(GetMacAddr);


/************************************************************************
    Funtcion:
    Description:    
    Calls:
    Called by:      ethmac_qdmaCfg_init
    Input:
    Output:
    Return:
    Others:
************************************************************************/
static int eth_recycle_tx(void *txMsg)
{   
    /* get tx msg and increase counters*/
    return 0;
}



/************************************************************************
    Funtcion:
    Description:
    Calls:
    Called by:      ethmac_qdmaCfg_init
    Input:
    Output:
    Return:
    Others:
************************************************************************/
static int qdma_reg_init(void)
{
	int i;
	QDMA_TxQStaticCngstThrh_T txqThreshold;
	QDMA_TxQueueCongestScale_T txqScale;
	QDMA_Mode_t qdmaMode = QDMA_ENABLE;

#if defined (QOS_REMARKING) || defined (TCSUPPORT_HW_QOS)
	/* do QDMA congestion configuration, otherwise low
	 * priority packets may use all tx descriptors. 
	 * We suppose each queue (for all channels) can 
	 * use 16*5==80 dscps at most! */
	memset(&txqScale, 0, sizeof(QDMA_TxQueueCongestScale_T));
	txqScale.maxScale = QDMA_TXQUEUE_SCALE_16_DSCP;
/*	qdma_set_congestion_scale(&txqScale);*/
	
	memset(&txqThreshold, 0, sizeof(QDMA_TxQStaticCngstThrh_T));
	for (i = 0; i < TX_QUEUE_NUM; i++)
	{
		txqThreshold.queueIdx = i;
/*		txqThreshold.grnMaxThreshold = 5;*/
/*		qdma_set_congest_threshold(&txqThreshold);*/
	}
/*	qdma_set_txqueue_threshold_mode(qdmaMode);*/
#endif
	QDMA_TxBufCtrl_T pTxBufCtrl;
	pTxBufCtrl.chnThreshold = 6;
	pTxBufCtrl.totalThreshold = 24;
	if(QDMA_TXBUFCNT_INIT_DISABLE)
		pTxBufCtrl.mode = QDMA_DISABLE;
	else
		pTxBufCtrl.mode = QDMA_ENABLE;

	QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_LAN,&pTxBufCtrl);
    return 0;
}

	
static inline int remove_special_tag(QDMA_RxMsg_Ether_T *rxmsg, struct sk_buff *skb)
{
    if(macSTagFlag == DISABLE )
    {
        return SUCCESS;
    }
    /*multicast data copy to cpu,we don't do anything here(hw nat driver will handle it)*/
    if(REMOVE_STAG(rxmsg, skb) == -1)
    {
        return FAIL;
    }
    return SUCCESS;
        
}


#define LOOPBACK_PKT_LEN 1500
#ifdef TCSUPPORT_AUTOBENCH
int autobench_mac_lpbk_flag = 0;
int autobench_mac_lpbk_cnt[6][3] = {0};
int autobench_mac_lpbk_cnt_port = 0;
int autobench_mac_hsgmii_lpbk_flag = 0;
int autobench_mac_hsgmii_lpbk_cnt = 0;
int autobench_mac_hsgmii_usb_lpbk_cnt = 0;
int autobench_mac_hsgmii_ether_lpbk_cnt = 0;
int autobench_mac_hsgmii_pcie1_lpbk_cnt = 0;
int autobench_mac_hsgmii_pkt_send = 2000;
int autobench_mac_ae_hsgmii_lpbk_flag = 0;
int autobench_mac_ae_hsgmii_lpbk_cnt = 0;
int autobench_mac_ae_hsgmii_pkt_send = 2000;
int autobench_mac_speed[6] = {0};
int autobench_mac_lpbk1_flag = 0;
int autobench_mac_step[3]= {20, 10, 200};
int autobench_mac_pkt_send[3] = {500, 800, 2000};
int autobench_mode = 0;
unsigned char LoopbackDataAllZero[LOOPBACK_PKT_LEN] = {
	0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x01,0x82,0x00,0x00,0x00, 
	0x08,0x06,0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,
	0xc0,0xa8,0x63,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,0x63,0x63,0x20,0x20,
	0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xb0,0xd2,0x60,0xb3
};

unsigned char LoopbackDataAllOne[LOOPBACK_PKT_LEN] = {
	0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x01,0x82,0x00,0x00,0x00, 
	0x08,0x06,0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,
	0xc0,0xa8,0x63,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,0x63,0x63,0x20,0x20,
	0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xb0,0xd2,0x60,0xb3
};


int check_data_fail = 1;

static int autobench_get_port(int port)
{
    int i = 0;

    for(i=0;i<6;i++)
    {
        if(port&(1<<i))
            return i;
    }

    return 0;
}

static int autobench_rx_hook(struct sk_buff *skb)
{
	uint32 pattern;
    int number = 0, i, port, rate;
    if (autobench_mac_lpbk_flag){
        // check payload is increment, special tag will remove, so only check to 1498 //

        port = skb->data[13] & 0xff;
        #if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
        port = autobench_get_port(port);
        #endif
        rate = autobench_mac_speed[port];
        for(i=64; i<LOOPBACK_PKT_LEN; i++){
            // odd port use all ones
            if( port %2 == 1 ){
                if(skb->data[i]  != 0xFF )
                    break;
            }else{
            // even port use all zero
                if(skb->data[i]  != 0 )
                    break;
            }
        }
        if( i == LOOPBACK_PKT_LEN ){
            autobench_mac_lpbk_cnt[port][rate]++;
            autobench_mac_lpbk_cnt_port++;
        } else {
            if (check_data_fail){
                printk("The errer data at %d bytes, error data: %02x, correct data: %02x\n", 
                    i, skb->data[i], 0);
                check_data_fail = 0;
            }   
        }
    }
    return DROP;

}



static int autobench_hsgmii_rx_hook(struct sk_buff *skb, unsigned int src_port)
{
	uint32 pattern;
    int number = 0, i, port, rate;
    if (autobench_mac_hsgmii_lpbk_flag){
        for(i=0; i<LOOPBACK_PKT_LEN; i++){
			if(i < 64 ){
				if(skb->data[i]  != LoopbackDataAllZero[i] )
					break;
			}else{
				if(skb->data[i]  != 0xFF )
					break;
			}
			
        }
        if( i == LOOPBACK_PKT_LEN ){
			if(AUTOBENCH_7523_SUPPORT_HSGMII_LAN)
				autobench_mac_hsgmii_lpbk_cnt ++;
			if(AUTOBENCH_7581_SUPPORT_USB0_HSGMII && src_port == usb_srcport )
				autobench_mac_hsgmii_usb_lpbk_cnt ++;
			if(AUTOBENCH_7581_SUPPORT_PCIE1_USXGMII10G &&  src_port == pcie1_srcport)
				autobench_mac_hsgmii_pcie1_lpbk_cnt ++;
			if((AUTOBENCH_7581_SUPPORT_ETHER_USXGMII10G || AUTOBENCH_7581_SUPPORT_ETHER_HSGMII2P5G)&&  src_port == eth_srcport)
				autobench_mac_hsgmii_ether_lpbk_cnt ++;
        } else {
                printk("The errer data at %d bytes, error data: %02x, correct data: %02x\n", 
                    i, skb->data[i], 0);
        }
    }
    return DROP;

}

static int autobench_ae_hsgmii_rx_hook(struct sk_buff *skb)
{
	uint32 pattern;
    int number = 0, i, port, rate;
    if (autobench_mac_ae_hsgmii_lpbk_flag){
        for(i=0; i<LOOPBACK_PKT_LEN; i++){
			if(i < 64 ){
				if(skb->data[i]  != LoopbackDataAllZero[i] )
					break;
			}else{
				if(skb->data[i]  != 0xFF )
					break;
			}
			
        }
        if( i == LOOPBACK_PKT_LEN ){
			autobench_mac_ae_hsgmii_lpbk_cnt ++;

        } else {
                printk("The errer data at %d bytes, error data: %02x, correct data: %02x\n", 
                    i, skb->data[i], 0);
        }
    }
    return DROP;

}



#endif

void wan_to_lan(struct sk_buff *skb, uint8 local)
{
    struct sk_buff *skb2 = NULL;    // for wan2lan 

#ifdef  TCSUPPORT_HOST_TCCONSOLE
	int tc_pro_idx=0;
	int vlan_level=0;
	tc_pro_idx=tcconsole_pro_idx(skb, &vlan_level);
	if (skb->data[tc_pro_idx] == 0xaa) {
		if(skb_headroom(skb) < TX_STAG_LEN){
			skb2 = skb_copy_expand(skb, TX_STAG_LEN, skb_tailroom(skb) , GFP_ATOMIC);
		}else{
			skb2 = skb_copy(skb,GFP_ATOMIC);
		}

		macSend(WAN2LAN_CH_ID,skb2);
		return;
	}
#endif

    if(masko_on_off
#if defined(TCSUPPORT_WAN2LAN_EXT)
        || (masko_ext & 0x10)
#endif
        ){
        if(skb_headroom(skb) < TX_STAG_LEN){
            skb2 = skb_copy_expand(skb, TX_STAG_LEN, skb_tailroom(skb) , GFP_ATOMIC);
        }else{
            skb2 = skb_copy(skb,GFP_ATOMIC);
        }

        if(skb2 == NULL){
            printk("wan2lan failure in rx direction for skb3 allocate failure.\n");
        }else{
#if defined(TCSUPPORT_RA_HWNAT)
            if (ra_sw_nat_hook_xfer){
                ra_sw_nat_hook_xfer(skb, skb2);
            }
#endif
            if(local == 1){
                skb_push(skb2,14);
            }
            skb2->mark |= SKBUF_COPYTOLAN;
#if defined(TCSUPPORT_RA_HWNAT)
			if (ra_sw_nat_hook_free)
				ra_sw_nat_hook_free(skb2);
#endif
            macSend(WAN2LAN_CH_ID,skb2);
        }
    }
}
void wlan_to_lan(struct sk_buff *skb, u8 local){
    struct sk_buff *skb2 = NULL;    // for wlan2lan 
    if(wlan_masko_on_off){
        if(skb_headroom(skb) < TX_STAG_LEN){
            skb2 = skb_copy_expand(skb, TX_STAG_LEN, skb_tailroom(skb) , GFP_ATOMIC);
        }
        else
            skb2 = skb_copy(skb,GFP_ATOMIC);

        if(skb2 == NULL){
            printk("wlan2lan failure in rx direction for skb2 allocate failure.\n");
        }else{
#if defined(TCSUPPORT_RA_HWNAT)
            if (ra_sw_nat_hook_xfer){
                ra_sw_nat_hook_xfer(skb, skb2);
            }
#endif
            if(local == 1){
               skb_push(skb2,14);
            }
            skb2->mark |= SKBUF_COPYTOLAN;
#if defined(TCSUPPORT_RA_HWNAT)
            if (ra_sw_nat_hook_free)
                ra_sw_nat_hook_free(skb2);
#endif
            macSend(WAN2LAN_CH_ID,skb2);
        }
    }
}        

/*****************************************
1.save outer vlan tag in skb->mark bit[19:8]
2.clean skb-<mark bit[23:21]
*****************************************/
inline void skb_vlan_parser(struct sk_buff *skb)
{
    uint8 *cp=NULL;
    uint16 tag;
    uint16 tpid;

    cp = skb->data;
    /* ingore stag vlan*/
    tpid = *(uint16 *)(cp+12);
    tag  = *(uint16 *)(cp+14);
    if(tpid == htons(ETH_P_8021Q)){
        if(hal_api_ext){
            skb->mark &= ~(VLAN_REMARKING | PRI_STORE);
            skb->mark |= (tag & 0xFFF)<<8;
            skb->mark |= (tag & 0xE000)>>12;/*>>13 <<1*/
        }
    }
}

/************************************************************************
    Funtcion:       soft_qdma_eth_tx
    Description:   for downstream half hwnat packets, directly sent to switch
    Calls:            N/A
    Called by:      eth_rx
    Input:            struct sk_buff *skb
    Output:          N/A
    Return:          NETDEV_TX_OK
    Others:          N/A
************************************************************************/
int soft_qdma_eth_tx(struct sk_buff *skb)
{
    ethTxMsg_t ethTxMsg;
    struct port_info eth_info;
    unsigned long int port_mask = 0;
    int error;

    memset(&ethTxMsg, 0, sizeof(ethTxMsg_t));
    ethTxMsg.raw.fPort = DPORT_GDMA1;

    memset(&eth_info, 0, sizeof(struct port_info));
    
    if(TCSUPPORT_FWC_QOS_VAL && (ENABLE == ratelimit_En))
    {
        /* if channel 0 ratelimit is enable , default sent to lan qdma channel 0 */
        /* else, port0~4 sent to lan qdma channel 0~4 */
        eth_info.channel = 0;
    }
    else
    {
        /* Assign Channel by port mask */
        port_mask = getStagBySkb(skb) & 0x1F;
        eth_info.channel = getChannelByPortmask(port_mask);
    }

    eth_info.force_high_priority_ring = 1;
    ethTxMsg.raw.channel = eth_info.channel;
#ifdef TCSUPPORT_CPU_EN7580
	ethTxMsg.raw.sp_tag = skb->dlf_resv0 & 0x1F ;
#endif
    error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN, skb,
                ethTxMsg.msg[0], ethTxMsg.msg[1], &eth_info);
    if(unlikely(error))
    {
        dev_kfree_skb_any(skb);
        mac_p->macStat.MIB_II.outDiscards++;
        return NETDEV_TX_OK;
    }
    mac_p->macStat.inSilicon.txEnQueueNum++;

    return NETDEV_TX_OK;
}

#ifdef TCSUPPORT_BBF_247
extern int (*pon_insert_tag_hook)(struct sk_buff **pskb);
int eth_bbf247_fwd_packet(struct sk_buff *skb)
{
    struct net_device *dev_p = NULL;
	uint8* mymac = GetMacAddr();
    uint8 ret = 0;
	int itf_index = 0;
	skb_push(skb,ETH_HLEN);

    dev_p = dev_get_by_name(&init_net,"pon");
    ret = (dev_p == NULL) || (skb->data[0]& 1) || (memcmp(skb->data,mymac,6) == 0);
    //printk("eth_rx packet dev queue xmit, pkt mac[0]:%x & 0x01 = %d, is my mac %d ret %d\n",skb->data[0],(skb->data[0]& 1),(memcmp(skb->data,mymac,6) != 0),ret);
    //if((0 == skb->data[0]& 1) && (memcmp(skb->data,mymac,6) != 0) && (dev_p != NULL))
    if(!ret)
    {
    	/*printk("eth_rx packet dev queue xmit, pkgMac %x:%x:%x:%x:%x:%x, mymac %x:%x:%x:%x:%x:%x\n",
            skb->data[0],skb->data[1],skb->data[2],skb->data[3],skb->data[4],skb->data[5],
            mymac[0],mymac[1],mymac[2],mymac[3],mymac[4],mymac[5]);*/
	if (isLANInterface(skb->dev))
	{
			itf_index = getLogicLANIndex(skb->dev) + 1 ;
		skb->mark |= itf_index << DEV_OFFSET;
	}

        skb->original_dev = skb->dev;
    	skb->dev = dev_p;
        skb->pon_vlan_flag |= PON_PKT_FROM_LAN;
	    dev_queue_xmit(skb);
    }
    else
    {
    	//printk("eth_rx packet go to netif receive skb\n");
    	skb_pull(skb,ETH_HLEN);
    	netif_receive_skb(skb);
    }
	/* dev_get_by_name ʱ?1Ŀǰݲ֪Ƿput˴˴dev_putݲ */
    //dev_put(dev_p);
    return 0;
}
#endif
/************************************************************************
    Funtcion:
    Description:
    Calls:
    Called by:      ethmac_qdmaCfg_init
    Input:
    Output:
    Return:
    Others:
************************************************************************/
__IMEM int eth_rx(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len)
{

    struct sk_buff *newskb;
	int npackets = 0;
	unsigned char offload_packet = 0;
	unsigned char traffic_classify_flag = E_ECNT_FORWARD;
    uint32 foe_ds_offload = 0;
	uint32 foe_mcst_offload = 0;
	int dp = 0;
	int ret = -1;
    uint32 value = 0;
    QDMA_RxMsg_Ether_T rxmsg;
    
    npackets++;
    if(unlikely(skb == NULL)){
        printk("Error Skb NULL, fail to receive!\n");
    }
	
    /**************** check rx msg *****************/ 
    QDMA_API_GET_RXMSG_ETHER(ECNT_QDMA_LAN,msg_p,&rxmsg);	

#ifdef TCSUPPORT_SOE
	if(rxmsg.hopflags >= HOP_FLAG_SOE_ENC && rxmsg.hopflags <= HOP_FLAG_SOE_PADDING_ERR){
		IPSEC_API_RCV_PACKET_FROM_SOE(skb, rx_len, rxmsg.swudf, rxmsg.hopflags);
		return 0;
	}
#endif

#if defined(TCSUPPORT_OLT)
    if(OLT_XPON_LAN_SRCPORT == rxmsg.sport)
    {
        OLT_MAC_ETH_RX_HOOK_FUN(msg_p,msg_len,skb,rx_len,rxmsg.sptag);
        return 0;
    }
#endif

#ifdef TCSUPPORT_XSI_ENABLE
    if(SPORT_GDMA3 == rxmsg.sport)
    {
        if(TCSUPPORT_WAN_XSI_VAL)
            XSI_WAN_MAC_IFC_RCV_PKT(msg_p,msg_len,skb,rx_len);
        else
            XSI_MAC_RECEIVE_PACKET(msg_p,msg_len,skb,rx_len);
        return 0;
    }
#endif

#ifdef TCSUPPORT_AUTOBENCH
#if defined(TCSUPPORT_HSGMII_LAN)	
	if(pcie0_srcport == rxmsg.sport || pcie1_srcport == rxmsg.sport || usb_srcport == rxmsg.sport || eth_srcport == rxmsg.sport)		
		{

			if(autobench_hsgmii_rx_hook(skb, rxmsg.sport)== FAIL){
				dev_kfree_skb_any(skb);
				return 1;
			}

		}
#endif

#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)	
		if(SPORT_GDMA2 == rxmsg.sport )
		{

			if(autobench_ae_hsgmii_rx_hook(skb)== FAIL){
				dev_kfree_skb_any(skb);
				return 1;
			}

		}
#endif


#else
#if defined(TCSUPPORT_HSGMII_LAN)
	if(pcie0_srcport == rxmsg.sport || pcie1_srcport == rxmsg.sport || usb_srcport == rxmsg.sport || eth_srcport == rxmsg.sport)
	{
		if((rxmsg.crsn != HIT_BIND_FORCE_TO_CPU) && (rxmsg.crsn != HIT_BIND_MUL_CPU)){
			HSGMII_LAN_MAC_RCV_PKT(msg_p,msg_len,skb,rx_len);
			return 0;
		}
	}

#endif


#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
		if(pcie0_srcport == rxmsg.sport || pcie1_srcport == rxmsg.sport || usb_srcport == rxmsg.sport || eth_srcport == rxmsg.sport)
		{
			if((rxmsg.crsn != HIT_BIND_FORCE_TO_CPU) && (rxmsg.crsn != HIT_BIND_MUL_CPU)){
				AE_WAN_MAC_RCV_PKT(msg_p,msg_len,skb,rx_len);
				return 0;
			}
		}
	
#endif
#endif /* TCSUPPORT_AUTOBENCH */


    /**************** check packet len *****************/ 
#ifdef TCSUPPORT_LRO_ENABLE
    if (unlikely(rx_len > SKBMGR_LRO_RX_BUF_LEN))
#else
    if (unlikely(rx_len > 2000))
#endif
    {
        mac_p->macStat.inSilicon.rxEtherFrameLengthErr++;
        dev_kfree_skb_any(skb);
        return 1;
    }
    skb_put(skb, rx_len);  

    /* dump rx packet*/
    if (g_dump_pkt_cnt)
    {
        g_dump_pkt_cnt--;
        printk("RX_MSG: W0=%x, W1=%x, W2=%x, rxmsg.sport = 0x%x, NO_SPECIAL_TAG = %d\n",
            rxmsg.rxmsg0, rxmsg.rxmsg1,rxmsg.rxmsg2, rxmsg.sport, (NO_SPECIAL_TAG(skb, rxmsg)));
        dump_skb(skb);
    }

#ifdef TCSUPPORT_ECNT_1588v2
	/* ECNT Hook Function for TIME STAMP Feature */
	if (PTP_1588_PKT_RX_CHECK(rxmsg)) {
		if (ptp_1588_rx_tstamp_hook) {
			ptp_1588_rx_tstamp_hook(skb);
		}
	}
#endif

	if (likely(ra_sw_nat_hook_rxinfo))
	{
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GE, (char*)&(rxmsg.rxmsg1), sizeof(rxmsg.rxmsg1));
	}

    #ifdef TCSUPPORT_NPU
    if((SPORT_NPU == rxmsg.sport)&&ra_sw_nat_restore_npu_pingpong_info)
        ra_sw_nat_restore_npu_pingpong_info(skb,rxmsg.sptag);
    #endif

    if(ra_sw_nat_natv6_fast_handler)
    {
        if(0 == ra_sw_nat_natv6_fast_handler(skb))
            return 0;
    }
    if(ra_sw_nat_vxlan_fast_handler)
    {
        if(0 == ra_sw_nat_vxlan_fast_handler(skb))
            return 0;
    }
#if defined(TCSUPPORT_DS_HWNAT_OFFLOAD)
    if (likely(ra_sw_nat_ds_offload))
    {
        foe_ds_offload = ra_sw_nat_ds_offload(skb, &dp);
    }
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_MCST) || defined(TCSUPPORT_XPON_HAL_API_MCST)
    if (likely(ra_sw_nat_mcst_offload))
    {
        foe_mcst_offload = ra_sw_nat_mcst_offload(skb, &dp);
    }
	/* whether the pkt sent by hwnat */
	if (1 == foe_mcst_offload)
	{
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_MCST)
		ecnt_mc_api_vlan_handle(skb);
#else/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_MCST)
		ECNT_HOOK_MC_API_VLAN_HANDLE(skb);
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
		return npackets;
	}
#endif/*TCSUPPORT_COMPILE*/

#ifdef TCSUPPORT_RA_HWNAT
	/*MultiCast Offload to WIFI or WIFI Offload*/  
	if ((0x18 == rxmsg.crsn) 
         || ((0x16 == rxmsg.crsn)
            &&(NO_SPECIAL_TAG(skb, rxmsg))
            #ifdef TCSUPPORT_DS_HWNAT_OFFLOAD
            &&(!foe_ds_offload)
            #endif
            ))
    {    
		offload_packet = 1;
	}
#endif
    
	if(offload_packet == 1){
		update_sw_mib(mac_p, skb, RX_MIB);
                skb->dev = lan_dev;

		ecnt_mc_api_recv(skb,rxmsg.crsn);
	}else{
		/**************** check rx msg *****************/ 
		if( unlikely(rxmsg.ip4f)){
			mac_p->macStat.inSilicon.rxIpCrcErr++;
 		}
		if( unlikely(rxmsg.l4f)){
 			mac_p->macStat.inSilicon.rxUdpTcpCrcErr++;
		}
		
		if(macqueue_start_idx != -1  && soft_ratelimit_enqueue_hook)
		{
			check_ratelimit_mac(skb,UP);
		}

/*TCSUPPORT_DLF_CTRL start*/
#ifdef TCSUPPORT_CPU_EN7580
        skb->dlf_resv0 = (uint16)(rxmsg.sptag&0xffff);
#else
        skb->dlf_resv0 = *(uint16 *)(skb->data+12) ;/*store special-tag , skip mac addr*/
#endif
/*TCSUPPORT_DLF_CTRL end*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
#ifdef TCSUPPORT_DS_HWNAT_OFFLOAD
        if (dsoffload_presetting && foe_ds_offload)
        {
            soft_qdma_pkt_proc(skb, lan_dev, foe_ds_offload);
            return 1;
        }
#endif
#endif

#ifdef TCSUPPORT_AUTOBENCH
#else
		/**************** remove stag, mark if wan packet *****************/
		if(unlikely(remove_special_tag(&rxmsg, skb) == FAIL)){
 			dev_kfree_skb_any(skb);
 			return 1;
		}
#endif

#if defined(TCSUPPORT_HWNAT_L2VID)
#if/*TCSUPPORT_COMPILE*/ !defined(TCSUPPORT_XPON_HAL_API_NG)
		if(ra_sw_nat_hook_rx_set_l2lu)
 			ra_sw_nat_hook_rx_set_l2lu(skb, 1, rxmsg.foe_index);
#endif/*TCSUPPORT_COMPILE*/
#endif
    
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_LOOPDETECT)			
		if(check_rcv_hook && check_rcv_hook(skb)){
			return 1;
		}
#endif/*TCSUPPORT_COMPILE*/

#ifdef TCSUPPORT_AUTOBENCH
		if(autobench_rx_hook(skb)== DROP){
			dev_kfree_skb_any(skb);
			return 1;
		}
#endif /* TCSUPPORT_AUTOBENCH */

#ifdef LOOPBACK_SUPPORT
		if(eth_loopback_rx_hook(msg_p, skb) == SUCCESS){
			return 1;
		}
#endif
#ifdef  TCSUPPORT_HOST_TCCONSOLE
		if(isComeFromWan(skb) != 1){
#endif
			if(unlikely(tcconsole_proc(skb)==1)){
				return 1;
			}
#ifdef  TCSUPPORT_HOST_TCCONSOLE
		}
#endif

		if ((rxmsg.sport != SPORT_QDMA_LAN && rxmsg.sport != SPORT_QDMA_WAN) && 
			(rxmsg.ip4 || rxmsg.ip6) && (rxmsg.l4vld) && (!rxmsg.ip4f && !rxmsg.l4f)){
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		}else{
			skb->ip_summed = CHECKSUM_NONE;
		}
    
		/**************** Count the MIB-II *****************/
#if defined(TCSUPPORT_WAN_ETHER)
		if(wan_dev && (isComeFromWan(skb) == 1)){
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CF_ENV)
		if(FAIL == FH_DOUBLE_VLAN_RX_PROC(skb)){
			dev_kfree_skb_any(skb);
			return 1;
		}
#endif/*TCSUPPORT_COMPILE*/
			update_sw_mib(mac_wan_p, skb, RX_MIB);
			wan_to_lan(skb,0);
			skb->dev = wan_dev;
		}else
#endif
		{
			update_sw_mib(mac_p, skb, RX_MIB);
			skb->dev = lan_dev;
		}
#if defined(TCSUPPORT_SEPERATED_ETH_ITF) 
		/**************** virtual interface eth0.1~4 assign to skb->dev********/
		virtual_itf_rx_hook(skb);
#endif
		FH_VLAN_PARSER(skb);
	}/*else for offload packet*/

    /**************** Layer 2 packet handle *****************/
    ret = ECNT_ETH_RECV_HOOK(&skb);
    if (ECNT_RETURN_DROP == ret)
    {
	    dev_kfree_skb_any(skb);
	    return 1;
    }
	
    skb->protocol = eth_type_trans(skb, skb->dev);
    skb->dev->last_rx = jiffies;  
#if defined(TCSUPPORT_WAN_ETHER)
	if(wan_speed_test_hook)
	{
		if(wan_speed_test_hook(skb)==SPEED_TEST_SUCCESS)
				return SPEED_TEST_SUCCESS;	
	}
	/* hook for tr471 */
	if(wan_tr471_hook)
	{
		if(wan_tr471_hook(skb)==SPEED_TEST_SUCCESS)
				return SPEED_TEST_SUCCESS;	
	}
#endif	
#ifdef TCSUPPORT_RA_HWNAT
    if (ecnt_l2tp_api_rx_handle(skb) > 0)
    {
	   return 0;
    }
    
    if (ra_sw_nat_hook_rx) {
        if (ra_sw_nat_hook_rx(skb) == 0) {
            return 0;
        }
    }
#endif

	#ifdef TCSUPPORT_ASYM_VOD
	if(isLANInterface(skb->dev))
	{
		if(checkSkbFromSocSwitchPort(skb))
		{
			if(1 == ECNT_ASYM_API_PKT_HANDLER(&skb,ECNT_ASYM_PKT_PHASE_SW_D))
			{
				dev_kfree_skb_any(skb);
				return 1;
			}
		}
		else
			ECNT_ASYM_API_PKT_HANDLER(&skb,ECNT_ASYM_PKT_PHASE_SW_U);
	}
	#endif

#if defined(TCSUPPORT_CF_ENV) && defined(TCSUPPORT_CPU_EN7521) && defined(TCSUPPORT_WAN_ETHER)
	if ( wan_dev == skb->dev )
	{
		if ( FH_EWAN_VLAN_HANDLE(skb, FHEWAN_RX_HOOK) )
		{
			dev_kfree_skb_any(skb);
			return 1;
		}
	}
#endif

#ifdef TCSUPPORT_TLS
	if(!FE_API_TLS_FORWARD(skb,FE_TLS_FORWARD_UPSTREAM))
		return 0;
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT) || defined(TCSUPPORT_XPON_HAL_API)
    PPE_API_TRAFFIC_CLASSIFY(skb) ;
#endif/*TCSUPPORT_COMPILE*/

#if defined(TCSUPPORT_WAN2LAN_EXT)
    if(masko_ext & 0x10)
        wan_to_lan(skb,1);
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_TRAFFIC_CLASSIFY) || defined(TCSUPPORT_FWC_ENV) || defined(TCSUPPORT_CMCCV2)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2)
	skb->mark2 &= (~(QUEUE_MARK_MASK));
	skb->mark2 |= QUEUE_FROM_LAN;
#else/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_ENV)
	if (0 == strncmp(skb->dev->name, "eth", strlen("eth")))
	{
		skb->mark |= (skb->dev->name[3] - '0') << 1;
	}
#else/*TCSUPPORT_COMPILE*/
	/*mark packet from dev*/
	if (isLANInterface(skb->dev))
	{
		skb->mark |= getSwitchLANIndex(skb->dev) << 1;
	}
#endif/*TCSUPPORT_COMPILE*/
#endif/*TCSUPPORT_COMPILE*/
	ECNT_TRAFFIC_CLASSIFY_HOOK(skb, &traffic_classify_flag);
	if(E_ECNT_DROP == traffic_classify_flag)
	{
#if defined(TCSUPPORT_RA_HWNAT) && defined(TCSUPPORT_RA_HWNAT_ENHANCE_HOOK)
		if(ra_sw_nat_hook_drop_packet)
		{
			ra_sw_nat_hook_drop_packet(skb);
		}
#endif
		dev_kfree_skb_any(skb);
		return 1;
	}
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_SWNAT)
	if(sw_upstream_nat_rx_hook){
		if(sw_upstream_nat_rx_hook(skb, 0)){
			return 0;
		}
	}
#endif

	if(0 == ECNT_HOOK_MULTICAST_DATA_HANLDE(skb))
	{
		dev_kfree_skb_any(skb);
		return 1;
	}

	if(ECNT_CONTINUE == ECNT_API_BBF247_PKT_US_HANDLE(skb)){
		//pkt has been forward by bbf247
	}
	else{
    	netif_receive_skb(skb);
	}

	return 0;
}

#if defined(TCSUPPORT_WAN2LAN_EXT)
int lanPortmap(int lanPort)
{
	return lan_port_map[lanPort - 1];
}

int lanPortmap_rev(int switchPort)
{
	int eth_intf = 0;

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	if(switchPort >= 0 && switchPort <= 5)
	{
		return (switch_port_map[switchPort] + 1);
	}
#else
	if(switchPort > 0 && switchPort <= 4)
	{
		for(;eth_intf <= 4; eth_intf++)
		{
			if(lan_port_map[eth_intf] == switchPort)
			{
				return (eth_intf+1);
			}
		}
	}
#endif
	return 0;
}

#endif

/************************************************************************
    Funtcion:
    Description:
    Calls:
    Called by:      ethmac_qdmaCfg_init
    Input:
    Output:
    Return:
    Others:
************************************************************************/
__IMEM static int eth_qdmaEventHandler(QDMA_EventType_t qdmaEventType)
{   
    unsigned long flags;
    if ((qdmaEventType == QDMA_EVENT_RECV_PKTS) ||
        (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER))
	{
        if (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER)
            printk("\nQDMA_EVENT_NO_RX_BUFFER\n");
    
		spin_lock_irqsave(&eth_napilock, flags);

		/* eth_lan and eth_wan use the same rx data path*/
		if (napi_schedule_prep(&mac_p->napi))
		{
			QDMA_API_DISABLE_RXPKT_INT(ECNT_QDMA_LAN); /*disable RX interrupt.*/
			__napi_schedule(&mac_p->napi);
		}
		spin_unlock_irqrestore(&eth_napilock, flags);
	}
	else if (qdmaEventType == QDMA_EVENT_TX_CROWDED)
	{
		printk("\nQDMA_EVENT_TX_CROWDED\n");
	}
	else
		printk("\nWrong QDMA Event Type: %d\n", qdmaEventType);

	return 0;
}

/************************************************************************
    Funtcion:
    Description:
    Calls:
    Called by:      ethmac_qdmaCfg_init
    Input:
    Output:
    Return:
    Others:
************************************************************************/
__IMEM static int eth_napiPoll(struct napi_struct *napi, int budget)
{
	int n;
	unsigned long flags;

	/* call eth_rx to receive a packet 
	 * until all packets are received */
	n = QDMA_API_RECEIVE_PACKETS(ECNT_QDMA_LAN, budget);

	if (n < budget)
	{
		spin_lock_irqsave(&eth_napilock, flags);
	
		__napi_complete(napi);
		QDMA_API_ENABLE_RXPKT_INT(ECNT_QDMA_LAN);

		spin_unlock_irqrestore(&eth_napilock, flags);
	}
	
	return n;

}

/************************************************************************
    Funtcion:
    Description:
    Calls:
    Called by:
    Input:
    Output:
    Return:
    Others:
************************************************************************/
static int ethmac_qdmaCfg_init(void)
{
    QDMA_InitCfg_t qdmaInitCfg;
    if(ethmac_qdmaCfg != INITIALIZED){
        QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN, QDMA_DISABLE);
        QDMA_API_RX_DMA_MODE(ECNT_QDMA_LAN, QDMA_DISABLE);
        /* HW QDMA QoS init */
    	qdma_reg_init();
        /* register QDMA callback functions */
        memset(&qdmaInitCfg, 0, sizeof(QDMA_InitCfg_t));
        qdmaInitCfg.macType = MAC_TYPE_ETHER;
    	qdmaInitCfg.cbRecvPkts = eth_rx;
    	qdmaInitCfg.cbEventHandler = eth_qdmaEventHandler;
        QDMA_API_INIT(ECNT_QDMA_LAN, &qdmaInitCfg);
        
	    QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN, QDMA_ENABLE);
        QDMA_API_RX_DMA_MODE(ECNT_QDMA_LAN, QDMA_ENABLE);
        ethmac_qdmaCfg = INITIALIZED;
    }
	return 0;
}

static int ethmac_qdmaCfg_deinit(void)
{
    if(ethmac_qdmaCfg == INITIALIZED){
        QDMA_API_RX_DMA_MODE(ECNT_QDMA_LAN, QDMA_DISABLE);
        QDMA_API_DEINIT(ECNT_QDMA_LAN);
        QDMA_API_TX_DMA_MODE(ECNT_QDMA_LAN, QDMA_DISABLE);
        ethmac_qdmaCfg = UNINIT;
    }
	return 0;
}


/************************************************************************
    Funtcion:       eth_lan_dev_register
    Description:    regiester eth lan dev eth0,eth0.1~0.4
    Calls:
    Called by:      eth_mac_init
    Input:
    Output:
    Return:
    Others:
************************************************************************/

static int eth_lan_dev_register(void)
{
    struct net_device *dev;
/* register netdev to kernel */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19)
    dev = alloc_netdev(sizeof(macAdapter_t), lan_itf,NET_NAME_USER, ether_setup);
#else
	dev = alloc_netdev(sizeof(macAdapter_t), lan_itf, ether_setup);
#endif
    if (!dev)
        return -ENOMEM;
        
    lan_dev = dev;
	/*dev->irq = MAC_INT;*/
    mac_p = netdev_priv(dev);

#if KERNEL_2_6_36
    /* Hook up with handlers */
    dev->netdev_ops = &gmac_netdev_ops;

    mac_p->napi.weight = MAC_NAPI_WEIGHT;

    netif_napi_add(dev, &mac_p->napi, eth_napiPoll, MAC_NAPI_WEIGHT);
#else
    dev->init = tc3262_gmac_start;
#endif
    return register_netdev(dev);
}


static int eth_hook_init(void)
{
	if(ecnt_register_hook(&ecnt_driver_ether_op)) {
		printk("ecnt_driver_ether_op register fail\n");
		return -ENODEV ;
    }
#if defined(TCSUPPORT_WAN2LAN_EXT)
	rcu_assign_pointer(gswPbusRead_hook, gswPbusRead);
	rcu_assign_pointer(gswPbusWrite_hook, gswPbusWrite);
	rcu_assign_pointer(lanPortmap_hook, lanPortmap);
	rcu_assign_pointer(lanPortmap_rev_hook, lanPortmap_rev);
#endif
#ifdef WAN2LAN
    rcu_assign_pointer(wlan_to_lan_hook, wlan_to_lan);
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_DS_LIMIT)
	rcu_assign_pointer(dslimit_SetDsTrtcmEnable,
                        dslimit_macMT7530SetDsTrtcmEnable);
	rcu_assign_pointer(dslimit_SetDsTrtcm,
                        dslimit_macMT7530SetDsTrtcm);
#endif/*TCSUPPORT_COMPILE*/

#ifdef TCSUPPORT_MT7530_SWITCH_API
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    rcu_assign_pointer(hook_lan_led_action, macMT7530LanLedControl);
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
#if defined(TCSUPPORT_DS_HWNAT_OFFLOAD)
    rcu_assign_pointer(ra_sw_nat_rtsp_data_handle, soft_qdma_rtsp_data_handle);
#endif
#endif

    return 0;
}

static int eth_hook_exit(void)
{
	ecnt_unregister_hook(&ecnt_driver_ether_op);
#if defined(TCSUPPORT_WAN2LAN_EXT)
	rcu_assign_pointer(gswPbusRead_hook, NULL);
	rcu_assign_pointer(gswPbusWrite_hook, NULL);
	rcu_assign_pointer(lanPortmap_hook, NULL);
	rcu_assign_pointer(lanPortmap_rev_hook,NULL);
#endif
#ifdef WAN2LAN
    rcu_assign_pointer(wlan_to_lan_hook, NULL);
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_DS_LIMIT)
	rcu_assign_pointer(dslimit_SetDsTrtcmEnable, NULL);
	rcu_assign_pointer(dslimit_SetDsTrtcm, NULL);
#endif/*TCSUPPORT_COMPILE*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	rcu_assign_pointer(hook_lan_led_action, NULL);
#endif

#if defined(TCSUPPORT_DS_HWNAT_OFFLOAD)
    rcu_assign_pointer(ra_sw_nat_rtsp_data_handle, NULL);
#endif
	return 0;
}

void downstermToQdmaTxRateLimitInit(void)
{
	int portId = 0;
	int limitValue = 0;
	QDMA_TxRateLimitSet_T txRateLimitSet;
	QDMA_TxRateLimitGet_T txRateLimitGet;
	
	for(portId=0;portId<LIMIT_PORT_NUM; portId++){
		lanPortState[portId].linkSpeed = -1;
		lanPortState[portId].linkState = 0;

		/* initial rate limit 1000 mbps */
		txRateLimitSet.chnlIdx=portId;
		txRateLimitSet.chnlRateLimitEn = 1;
		txRateLimitSet.rateLimitValue = (1000 * 1000);	/* unit: kbps */
		QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitSet);
	}
	/* initial rate limit 1000 mbps */
	/* channel 7 for wifi offload, no need ratelimt in newest IC like 7528/7523/7580/7581/... */
	txRateLimitSet.chnlIdx = WIFI_OFFLOAD_CHANNEL;
	limitValue = 1200;
	txRateLimitSet.chnlRateLimitEn = 0;
	if (isEN751627 && !isEN7528)
	{
		limitValue = 1200;
		#ifdef TCSUPPORT_WLAN_MT7915D
		txRateLimitSet.chnlRateLimitEn = 0;
		#else
		txRateLimitSet.chnlRateLimitEn = 1;
		#endif
	}
	else if (isEN751221)
	{
		limitValue = 1000;
		txRateLimitSet.chnlRateLimitEn = 1;
	}
	txRateLimitSet.rateLimitValue = (limitValue * 1000);	/* unit: kbps */
	QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitSet);
	
	return;
}

/* set chnlRateLimitEn */
void perChannelRateLimitSwitch(void)
{
	uint32 regVal=0;
	int isRatelimitEn=0;
	int portId = 0;
	QDMA_TxRateLimitSet_T txRateLimitSet;
    int rateValue[4] = {10, 100, 1000, 10000};  /* unit: mbps */

	/*enable QDMA LAN channel 0~4 rate limit*/
	if(g_force_dstq==0){			/* ds force disable */
		if(use_ext_switch){		/* disable rate limit */
			/* do nothing */
		}else{						/* enable rate limit */
			isRatelimitEn=1;
		}
	}else if(g_force_dstq==1){ /* ds force to gdma, disable rate limit */
		/* do nothing */
	}else if(g_force_dstq==2 || g_force_dstq==3 || g_force_dstq==4){ 
		isRatelimitEn=1;
	}

	for(portId=0;portId<LIMIT_PORT_NUM; portId++){
		/* update chnlRateLimitEn of channel  */
		txRateLimitSet.chnlIdx = portId;
        if (isRatelimitEn){
            if ((lanPortState[portId].linkSpeed >=0) && (lanPortState[portId].linkSpeed <=3)){
    		    txRateLimitSet.rateLimitValue = rateValue[lanPortState[portId].linkSpeed] * 1000;
            }else{
    		    txRateLimitSet.rateLimitValue = 1000 * 1000; /* Set to 1000Mbps by default */
            }
        }else{
		    txRateLimitSet.rateLimitValue = 0;
        }

#if 0//defined(TCSUPPORT_WAN_ETHER)
		if(portId == wan_port_id){
			/* Disable QDMA limit if the port is used for ETHERWAN */
			txRateLimitSet.chnlRateLimitEn = 0;
		}else{
			txRateLimitSet.chnlRateLimitEn = isRatelimitEn;
		}
#else
		txRateLimitSet.chnlRateLimitEn = isRatelimitEn;
#endif
		QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitSet);
	}

	return;
}

#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
static QDMA_TxRateLimitGet_T txRateLimitGet = {0};
static unsigned int p6_fc_status = 0;

static void use_qdma_wan_fe_set(int channel, int wan_mode)
{
	int i = 0;
	unsigned int wan_conf = 0;

	if(USE_QDMA_WAN_ETHER == wan_mode){
		/* Gdma2 FWD to PSE Port 1(GDM1), disable CRC strip */
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_OC, FE_DP_GDM1);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_MC, FE_DP_GDM1);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_BC, FE_DP_GDM1);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_UC, FE_DP_GDM1);
		FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA2, FE_DISABLE);
        FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA2, 0x7d0, 0x20);

		/* enable Gdma2 Loopback */
		FE_API_SET_LOOPBACK_ENABLE(FE_GDM_SEL_GDMA2, channel, FE_ENABLE);
		FE_API_SET_LOOPBACK_MODE(FE_GDM_SEL_GDMA2, FIX_MODE, FIX_MODE, FIX_MODE);
		QDMA_API_LOOPBACK_MODE(ECNT_QDMA_WAN, QDMA_LOOPBACK_UMAC);

		/* Enable CDM2/GDM2 HWF/TX/RX CHN */
		FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_TX, channel, FE_ENABLE);
		for(i = 0; i < 16; i++) {
			FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_RX, i, FE_ENABLE);
		}
		FE_API_SET_HWFWD_CHANNEL(FE_CDM_SEL_CDMA2, channel, FE_ENABLE);

		FE_API_SET_GLO_RATE_BYTE(ADD_MODE, 0x14);
		if(isEN7528)
		{
			wan_conf = read_reg_word(0xbfb00070);
			wan_conf = (wan_conf & ~0x7) | 0x1;     /* 7528 GDMA2 Rx  to GDMA1 Tx channel should not be 0 (for wdma) */
			write_reg_word(0xbfb00070, wan_conf);        /* wan conf should be epon mode so that GDMA1 Tx channel will be the same as GDMA2 Rx channel */
		}
		if(SUPPORT_GDM_SPTAG_ACTION)
		{
			FE_API_SET_GMD2_SPTAG_FOR_LOOPBACK(FE_ENABLE);
		}
	}
	else{
#if 0
		for(i = 0; i < 16; i++) {
			/* Disable CDM2/GDM2 HWF/TX/RX CHN */
			FE_API_SET_HWFWD_CHANNEL(FE_CDM_SEL_CDMA2, i, FE_DISABLE);
			FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_RX, i, FE_DISABLE);
			FE_API_SET_CHANNEL_ENABLE(FE_GDM_SEL_GDMA2, FE_GDM_SEL_TX, i, FE_DISABLE);
		}
#endif
		/* disable Gdma2 Loopback */
		FE_API_SET_LOOPBACK_ENABLE(FE_GDM_SEL_GDMA2, 0, FE_ENABLE);
		FE_API_SET_LOOPBACK_MODE(FE_GDM_SEL_GDMA2, RANDOM_MODE, RANDOM_MODE, FIX_MODE);
		QDMA_API_LOOPBACK_MODE(ECNT_QDMA_WAN, QDMA_LOOPBACK_DISABLE);

		/* Gdma2 FWD to PSE Port 4(PPE) */
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_OC, FE_DP_PPE);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_MC, FE_DP_PPE);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_BC, FE_DP_PPE);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_UC, FE_DP_PPE);
		FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA2, FE_ENABLE);
        FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA2, 0x7d0, 0x3c);

		FE_API_SET_GLO_RATE_BYTE(ADD_MODE, 0x18);
		if(SUPPORT_GDM_SPTAG_ACTION)
		{
			FE_API_SET_GMD2_SPTAG_FOR_LOOPBACK(FE_DISABLE);
		}
	}
	
	return;
}

static void use_qdma_wan_p6_fc_set(int wan_mode)
{
	unsigned int value = 0;

	if(USE_QDMA_WAN_ETHER == wan_mode){
		value = read_reg_word(GSW_BASE+0x3600);
		p6_fc_status = value;

		value &= ~0x30;
		write_reg_word(GSW_BASE+0x3600, value);
	}
	else{
		if(p6_fc_status)
		{
			write_reg_word(GSW_BASE+0x3600, p6_fc_status);
			p6_fc_status = 0;
		}
	}

	return;
}

static void ues_qdma_wan_chnl_ratelimit(int wan_port, int wan_mode)
{
	QDMA_TxRateLimitSet_T txRateLimitSet = {0};
	QDMA_TxBufCtrl_T txBufUsage;
	QDMA_txCngstCfg_t qdmaTxCngstCfg;

	if(USE_QDMA_WAN_ETHER == wan_mode){
		g_wan_mode = USE_QDMA_WAN_ETHER;
		if((wan_port<0) || (wan_port>=LIMIT_PORT_NUM))
			return;
		
		txRateLimitGet.chnlIdx = wan_port;
		QDMA_API_GET_TX_RATELIMIT(ECNT_QDMA_WAN, &txRateLimitGet);
		QDMA_API_SET_PKTSIZE_OVERHEAD_VALUE(ECNT_QDMA_WAN, 0x14);

		if(SUPPORT_GDM_SPTAG_ACTION){
			txBufUsage.mode = QDMA_DISABLE;
			QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_WAN, &txBufUsage);

			qdmaTxCngstCfg.txCngstMode = QDMA_TXCNGST_DYNAMIC_NORMAL;
			QDMA_API_SET_TXQ_CNGST_AUTO_CONFIG(ECNT_QDMA_WAN, &qdmaTxCngstCfg);

			QDMA_API_SET_MAC_LIMIT_THRESHOLD(ECNT_QDMA_WAN, 200, 50);
			QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);
			QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);
		}
	
		lanPortState[wan_port].linkSpeed = -1;
	}
	else{	
		g_wan_mode = USE_QDMA_WAN_PON_XDSL;
		txRateLimitSet.chnlRateLimitEn = txRateLimitGet.chnlRateLimitEn;
		txRateLimitSet.chnlIdx = txRateLimitGet.chnlIdx;
		txRateLimitSet.rateLimitValue = txRateLimitGet.rateLimitValue; 
		QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_WAN, &txRateLimitSet);
		QDMA_API_SET_PKTSIZE_OVERHEAD_VALUE(ECNT_QDMA_WAN, 0x18);
	}

	return;
}

void ether_wan_use_qdma_wan_set(int wan_port, int set_wan_mode)
{
	if(g_wan_mode == set_wan_mode)
		return;
	if(SUPPORT_GDM_SPTAG_ACTION){
		wan_port = 0;
	}
	printk("ether_wan_use_qdma_wan_set wan_port %d set_wan_mode %d\n", wan_port, set_wan_mode);
	use_qdma_wan_fe_set(wan_port, set_wan_mode);

	use_qdma_wan_p6_fc_set(set_wan_mode);

	ues_qdma_wan_chnl_ratelimit(wan_port, set_wan_mode);

	return;
}

void ether_wan_use_qdma_wan(int set_wan_mode)
{
    printk("ether_wan_use_qdma_wan wan_port %d set_wan_mode %d\n", wan_port_id, set_wan_mode);

	ether_wan_use_qdma_wan_set(wan_port_id, set_wan_mode);

	return;
}
#endif  /* endif for TCSUPPORT_QDMA_WAN_FOR_ETHER */

static int __init eth_mac_init(void)
{
    int err = 0;

    ds_qos = QOS_IN_SWITCH;
    g_force_dstq=2;
    downstermToQdmaTxRateLimitInit();

    printk(KERN_INFO "%s", version);

	/*init the space*/
	pBrtable = (mt7530_switch_brtable *)kmalloc(sizeof(mt7530_switch_brtable), GFP_ATOMIC);
	if ( !pBrtable ){
		return -ENOMEM;
	}

    macReset(); 
    mdelay(5);	

    err = eth_lan_dev_register();
    if(err)
        return err;

#if defined(TCSUPPORT_SEPERATED_ETH_ITF) 
    err = init_virtual_interface();
    if(err)
        return err;
#endif

#if defined(TCSUPPORT_WAN_ETHER)
    eth_wan_init();
#endif

#ifdef TCSUPPORT_HEC_6906
    amc_verify_init();
#endif


#ifdef TC_CONSOLE_ENABLE 
    tcconsole_init();
#endif
    eth_proc_init();

    err = eth_hook_init();
    if(err)
        return err;

    macInit();

    /* add wan_port_id to GDM2 wan_port */
#if defined(TCSUPPORT_WAN_ETHER)
    if(SUPPORT_SET_DOUBLE_WAN_PORT){
        FE_API_SET_WAN_PORT_7516(FE_ENABLE, FE_WANPORT_GSW_P0+wan_port_id, FE_WANPORT_PSE_P2);
    }
#endif

    /* Etherwan port map is used, set chnlRateLimitEn after macInit() */
    perChannelRateLimitSwitch();

	memset(Expand_64_Tx, 0 ,sizeof(Expand_64_Tx));
	memset(Expand_64_Rx,0 ,sizeof(Expand_64_Rx));

	memset(g_Tx_Last,0 , sizeof(g_Tx_Last)); 
	memset(g_Rx_Last,0, sizeof(g_Rx_Last));	
    
    ecnt_register_hook(&ecnt_eth_updateKernelMacTable_op);

    /* register callback function to qdma */
    ethmac_qdmaCfg_init();

	if(!MAC_AUTO_POLLING_BY_PHY)
    	eth_lan_port_init();
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER) && defined(TCSUPPORT_CPU_EN7512) 
    g_wan_mode = NOT_ETHER_USE_QDMA_WAN;
#endif    

	if(BURST_MONITOR_INIT_ENABLE)
		burst_monitor_enable = 0;
	else
		burst_monitor_enable = 1;

#ifdef INCLUDE_ETH_SWITCH_RTL8372
	rtl8372n_switch_init();
#endif /* INCLUDE_ETH_SWITCH_RTL8372 */

    return 0;
}


static void __exit eth_mac_exit(void)
{
    ethmac_qdmaCfg_deinit();

#if defined(TCSUPPORT_SEPERATED_ETH_ITF) 
    exit_virtual_interface();
#endif
    macReset();	

	unregister_netdev(lan_dev);

	free_netdev(lan_dev);

    kfree(pBrtable);

#if defined(TCSUPPORT_WAN_ETHER)	
	eth_wan_exit();
#endif

#ifdef TC_CONSOLE_ENABLE 
	tcconsole_exit();
#endif
    eth_proc_exit();
    eth_hook_exit();

    ecnt_unregister_hook(&ecnt_eth_updateKernelMacTable_op);
}
 
#if defined(TCSUPPORT_CF_ENV) && defined(TCSUPPORT_CPU_EN7521) && defined(TCSUPPORT_WAN_ETHER)
int eth1_active_op(int active)
{
	uint8 port = 0, value = 0;

	/* only support open eth1, cannot close it. */
	if ( 1 == active )
	{
		wan_port_id = 4;
		lan_port_map[0] = 3;
		lan_port_map[1] = 2;
		lan_port_map[2] = 1;
		lan_port_map[3] = -1;

		for ( port=0; port<RT63365_SWIC_PORTNUM; port++ )
		{
			value = lan_port_map[port];
			if ( 0<=value && value <RT63365_SWIC_PORTNUM )
			{
				switch_port_map[value] = port;
			}
		}

	    /* Etherwan port map is used, set chnlRateLimitEn after macInit() */
	    perChannelRateLimitSwitch();
	}

	return 0;
}
#endif




#ifdef TCSUPPORT_AUTOBENCH
#if defined(TCSUPPORT_HSGMII_LAN)	

int hsgmii_lan_mac_tx_autobench(struct sk_buff *skb, int hsgmii_index)
{
	ethTxMsg_t xsiTxMsg = {0};
	int error = 0,  force_port = 0;
	int tsid = 0x7f;
	int acnt1 = 0x1f;
	int acnt0 = 0x1f;
	struct port_info xsi_info = {0};
	struct port_info *qdma_info = NULL;

	unchar nbq = 0, channel = 0;

	if(AUTOBENCH_7523_HSGMII_MODE){
		
		if(hsgmii_index == 0 ){
			nbq = 4;
			channel = 10;
			force_port =DPORT_GDMA3; 
		}else if(hsgmii_index == 1 ){
			
			nbq = 5;
			channel = 11;
			force_port =DPORT_GDMA3;
		}else if ( hsgmii_index == 2 ){
			
			nbq = 6;
			channel = 12;
			force_port =DPORT_GDMA3;
		}else if ( hsgmii_index == 3 ){
			
			nbq = 0;
			channel = 0;
			force_port =DPORT_GDMA2;
		}else{
			
			printk("dev name is not in hsgmii range \n");
			dev_kfree_skb_any(skb);
			return NETDEV_TX_OK;
		}
	}else if(AUTOBENCH_7581_HSGMII_MODE){
		if(hsgmii_index == 0 ){
			nbq = 4;
			channel = 10;
			force_port =DPORT_GDMA3; 
			//pcie0 serdes
		}else if(hsgmii_index == 1 ){
			
			nbq = 5;
			channel = 11;
			force_port =DPORT_GDMA3;
			//pcie1 serdes
		}else if ( hsgmii_index == 2 ){
			
			nbq = 1;
			channel = 12;
			force_port =DPORT_GDMA4;
			//usb serdes
		}else if ( hsgmii_index == 3 ){
			
			nbq = 0;
			channel = 13;
			force_port =DPORT_GDMA4;
			//ethernet serdes
		}else{
			
			printk("dev name is not in hsgmii range \n");
			dev_kfree_skb_any(skb);
			return NETDEV_TX_OK;
		}


	}else {
		printk("only support 7528 7581 hsgmii mode\n");
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;

	}
		

	

    
	memset(&xsi_info, 0, sizeof(struct port_info));

	xsi_info.tsid = tsid;   
	xsi_info.txq = 0;
    xsi_info.nbq = nbq;
	xsi_info.magic = FOE_MAGIC_XSI;
	xsi_info.channel = channel;/*qdma lan chnl 10-12,for hsgmii pcie0/1 usb*/
	xsi_info.ds_to_qdma = 0;

	memset(&xsiTxMsg, 0, sizeof(ethTxMsg_t));
	xsiTxMsg.raw.mtr_g = tsid;
	xsiTxMsg.raw.fPort = force_port;
	xsiTxMsg.raw.channel = xsi_info.channel;

	xsiTxMsg.raw.acnt_g1 = acnt1;
	xsiTxMsg.raw.acnt_g0 = acnt0;

    xsiTxMsg.raw.nboq = nbq;

	qdma_info = &xsi_info;

	//XSI_MSG(XSI_DBG_MSG,"xsiTxMsg.raw.channel=%d fport=%d sp_tag=%x\n",xsiTxMsg.raw.channel,xsiTxMsg.raw.fPort,xsiTxMsg.raw.sp_tag);
	//XSI_MSG(XSI_DBG_MSG,"xsi_info.txq=%d eth_macSTagEn=%d magic=%d ds_to_qdma=%d channel=%d\n",xsi_info.txq,xsi_info.eth_macSTagEn,xsi_info.magic,xsi_info.ds_to_qdma,xsi_info.channel);

	error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN,skb,xsiTxMsg.msg[0], xsiTxMsg.msg[1], qdma_info);
	if(unlikely(error)){
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}
	
	return NETDEV_TX_OK;

}


int eth_mac_hsgmii_loopback_gen_and_check(int hsgmiiIdx, int pktnum){
	struct sk_buff* skb=NULL;
	int i=0,rx_port1=0,rx_port2=0,pkt_loss=0, step = 0;
    long long delay;
	char* LoopbackData;
	
    autobench_mac_hsgmii_lpbk_flag = 1;
	
    step =100;
    LoopbackData = LoopbackDataAllOne;

    
	for(i=0; i<pktnum; i++){
	
		skb=skbmgr_dev_alloc_skb2k();

		if(skb){
			memcpy(skb->data, LoopbackData, LOOPBACK_PKT_LEN);
			skb_put(skb, LOOPBACK_PKT_LEN);
            skb->len = LOOPBACK_PKT_LEN;
			hsgmii_lan_mac_tx_autobench(skb, hsgmiiIdx);
		}
		else{
			printk("Allocate skb failure!\r\n");
		}  

		if( i%step == step-1 ){
            delay = jiffies + 2;
            while(time_before(jiffies, delay)){
                schedule();
            }
        }

	}

	return pkt_loss;
	
}

#define GDM3_LPBK_CFG 0xBFB5111c
#define GDM3_FWG_CFG 0xBFB51100
#define GDM4_FWG_CFG 0xBFB52500
#define GDM4_LPBK_CFG 0xBFB5251c



/*in 7523step 1 , only need test pcie1 2.5G  hsgmii*/
/*in 7581 test usb,ether,pcie1 */
int eth_mac_hsgmii_autobench_loopback(void)
{
	int retValue=0,i=0,j=0,pkt_loss=0;
	long long delay;

	/*hsgmii_index : 0 pcie0 as hsgmii,1 pcie1 as hsgmii, 2 usb as hsgmii,3 active ethernet*/
	//int hsgmii_index = eth_data->ether_private.hsgmii_index;


	for(i=64; i<LOOPBACK_PKT_LEN; i++){
		  LoopbackDataAllZero[i] = 0;
		  LoopbackDataAllOne[i] = 0xFF;
	}
#if defined(TCSUPPORT_CPU_EN7581)||defined(TCSUPPORT_CPU_AN7552)
	autobench_mac_hsgmii_usb_lpbk_cnt = 0;
	autobench_mac_hsgmii_ether_lpbk_cnt = 0;
	autobench_mac_hsgmii_pcie1_lpbk_cnt = 0;

	regWrite32(GDM3_FWG_CFG, 0x7f10000);
	regWrite32(GDM4_FWG_CFG, 0x7f10000);

	//regWrite32(GDM3_FWG_CFG, 0x7f00000);
	//regWrite32(GDM4_FWG_CFG, 0x7f00000);
	//regWrite32(GDM3_LPBK_CFG, 0xfffc03);
	//regWrite32(GDM4_LPBK_CFG, 0xfffc03);


	if(AUTOBENCH_7581_SUPPORT_USB0_HSGMII){
		xsgmii_api(5,1,0,0,0);
		delay = jiffies + 3 * HZ;
		// wait hsgmii phy loopback mode link up
		while(time_before(jiffies, delay)){
		   schedule();
		}
	}
	if(AUTOBENCH_7581_SUPPORT_PCIE1_USXGMII10G){
          	xsgmii_api(3,0,0,0,0);/*must link up pcie0 before link up pcie1*/
		xsgmii_api(4,0,0,0,0);
		delay = jiffies + 3 * HZ;
		// wait hsgmii phy loopback mode link up
		while(time_before(jiffies, delay)){
		   schedule();
		}
	}
	if(AUTOBENCH_7581_SUPPORT_ETHER_USXGMII10G){
		xsgmii_api(1,0,0,0,0);
		delay = jiffies + 3 * HZ;
		// wait hsgmii phy loopback mode link up
		while(time_before(jiffies, delay)){
		   schedule();
		}
	}
	if(AUTOBENCH_7581_SUPPORT_ETHER_HSGMII2P5G){
		xsgmii_api(1,1,0,0,0);
		delay = jiffies + 3 * HZ;
		// wait hsgmii phy loopback mode link up
		while(time_before(jiffies, delay)){
		   schedule();
		}
	}

	

	if(AUTOBENCH_7581_SUPPORT_USB0_HSGMII){
		eth_mac_hsgmii_loopback_gen_and_check(2,autobench_mac_hsgmii_pkt_send);
		delay = jiffies + 3 * HZ;
		while(time_before(jiffies, delay)){
		   schedule();
		}
	}
	if(AUTOBENCH_7581_SUPPORT_PCIE1_USXGMII10G){
		eth_mac_hsgmii_loopback_gen_and_check(1,autobench_mac_hsgmii_pkt_send);
		
		delay = jiffies + 3 * HZ;
		while(time_before(jiffies, delay)){
		   schedule();
		}
	}
	if(AUTOBENCH_7581_SUPPORT_ETHER_USXGMII10G || AUTOBENCH_7581_SUPPORT_ETHER_HSGMII2P5G){
		eth_mac_hsgmii_loopback_gen_and_check(3,autobench_mac_hsgmii_pkt_send);
		delay = jiffies + 3 * HZ;
		while(time_before(jiffies, delay)){
		   schedule();
		}
	}
#else
	autobench_mac_hsgmii_lpbk_cnt = 0;

	/*set gdm3 loopback for debug test*/
	//regWrite32(GDM3_LPBK_CFG, 0xfffc03);
	//regWrite32(GDM3_FWG_CFG, 0x7f00000);
	/*set pcie phy 2.5G for test */
	
	sgmii_api_force_mode(1,0);

	delay = jiffies + 3 * HZ;
	// wait hsgmii pcie1 phy loopback mode link up
	while(time_before(jiffies, delay)){
	   schedule();
	}

	regWrite32(GDM3_FWG_CFG, 0x7f10000);
	
	eth_mac_hsgmii_loopback_gen_and_check(1,autobench_mac_hsgmii_pkt_send);
#endif
	return 0;

}

#endif

#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)	

int hsgmii_ae_mac_tx_autobench(struct sk_buff *skb)
{
	ethTxMsg_t xsiTxMsg = {0};
	int error = 0,  force_port = 0;
	int tsid = 0x7f;
	int acnt1 = 0x1f;
	int acnt0 = 0x1f;
	struct port_info xsi_info = {0};
	struct port_info *qdma_info = NULL;

	unchar nbq = 0, channel = 0;

	force_port =DPORT_GDMA2;
	
	memset(&xsi_info, 0, sizeof(struct port_info));

	xsi_info.tsid = tsid;   
	xsi_info.txq = 0;
    xsi_info.nbq = nbq;
	xsi_info.magic = FOE_MAGIC_XSI;
	xsi_info.channel = channel;/*qdma lan chnl 10-12,for hsgmii pcie0/1 usb*/
	xsi_info.ds_to_qdma = 0;

	memset(&xsiTxMsg, 0, sizeof(ethTxMsg_t));
	xsiTxMsg.raw.mtr_g = tsid;
	xsiTxMsg.raw.fPort = force_port;
	xsiTxMsg.raw.channel = xsi_info.channel;

	xsiTxMsg.raw.acnt_g1 = acnt1;
	xsiTxMsg.raw.acnt_g0 = acnt0;

    xsiTxMsg.raw.nboq = nbq;

	qdma_info = &xsi_info;

	//XSI_MSG(XSI_DBG_MSG,"xsiTxMsg.raw.channel=%d fport=%d sp_tag=%x\n",xsiTxMsg.raw.channel,xsiTxMsg.raw.fPort,xsiTxMsg.raw.sp_tag);
	//XSI_MSG(XSI_DBG_MSG,"xsi_info.txq=%d eth_macSTagEn=%d magic=%d ds_to_qdma=%d channel=%d\n",xsi_info.txq,xsi_info.eth_macSTagEn,xsi_info.magic,xsi_info.ds_to_qdma,xsi_info.channel);

	error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN,skb,xsiTxMsg.msg[0], xsiTxMsg.msg[1], qdma_info);
	if(unlikely(error)){
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}
	
	return NETDEV_TX_OK;

}


int eth_mac_hsgmii_ae_loopback_gen_and_check( int pktnum){
	struct sk_buff* skb=NULL;
	int i=0,rx_port1=0,rx_port2=0,pkt_loss=0, step = 0;
    long long delay;
	char* LoopbackData;
	
    autobench_mac_ae_hsgmii_lpbk_flag = 1;
	
    step =100;
    LoopbackData = LoopbackDataAllOne;

    
	for(i=0; i<pktnum; i++){
	
		skb=skbmgr_dev_alloc_skb2k();

		if(skb){
			memcpy(skb->data, LoopbackData, LOOPBACK_PKT_LEN);
			skb_put(skb, LOOPBACK_PKT_LEN);
            skb->len = LOOPBACK_PKT_LEN;
			hsgmii_ae_mac_tx_autobench(skb);
		}
		else{
			printk("Allocate skb failure!\r\n");
		}  

		if( i%step == step-1 ){
            delay = jiffies + 2;
            while(time_before(jiffies, delay)){
                schedule();
            }
        }

	}

	return pkt_loss;
	
}

#define GDM2_LPBK_CFG 0xBFB5151c
#define GDM2_TXCHN_EN 0xBFB51524
#define GDM2_RXCHN_EN 0xBFB51528
#define GDM2_FWG_CFG 0xBFB51500

/*step 1 , only need test pcie1 2.5G  hsgmii*/
int eth_mac_hsgmii_ae_autobench_loopback(void)
{
	int retValue=0,i=0,j=0,pkt_loss=0;
	long long delay;

	/*hsgmii_index : 0 pcie0 as hsgmii,1 pcie1 as hsgmii, 2 usb as hsgmii,3 active ethernet*/
	//int hsgmii_index = eth_data->ether_private.hsgmii_index;


	for(i=64; i<LOOPBACK_PKT_LEN; i++){
		  LoopbackDataAllZero[i] = 0;
		  LoopbackDataAllOne[i] = 0xFF;
	}
	autobench_mac_ae_hsgmii_lpbk_cnt = 0;

	/*set gdm3 loopback for debug test*/
	//regWrite32(GDM2_LPBK_CFG, 0xfffc03);
	//regWrite32(GDM2_FWG_CFG, 0x7f00000);

#if defined(TCSUPPORT_CPU_EN7581)
	//set pon usxgmii mode 10G
	xsgmii_api(2,0,0,0,0);

	//regWrite32(GDM2_LPBK_CFG, 0xfffc03);
	//regWrite32(GDM2_FWG_CFG, 0x7f00000);

#elif defined(TCSUPPORT_CPU_AN7552)
    xsgmii_api(2,1,0,0,0);

    regWrite32(0x1fb501f0, 0);
    regWrite32(0x1fb501f4, 0);
#else
	/*set pon hsgmii phy 2.5G for test */

	sgmii_api_force_mode(3,0);
#endif

	delay = jiffies + 3 * HZ;
	// wait hsgmii pcie1 phy loopback mode link up
	while(time_before(jiffies, delay)){
	   schedule();
	}

	regWrite32(GDM2_FWG_CFG, 0x7f00000);
	regWrite32(GDM2_TXCHN_EN, 0xffffffff);
	regWrite32(GDM2_RXCHN_EN, 0xffff);
	
    regWrite32(0x1fb501b0,0x00210040);
	
	eth_mac_hsgmii_ae_loopback_gen_and_check(autobench_mac_hsgmii_pkt_send);
	
	return 0;

}

#endif




void delay1ms_autobench(int ms)
{
#ifdef TCSUPPORT_CPU_ARMV8  //for arm porting

	delay1ms(ms);
#else

	volatile uint32 timer_now, timer_last;
	volatile uint32 tick_acc;
	uint32 one_tick_unit = 1 * SYS_HCLK * 1000 / 2;
	volatile uint32 tick_wait = ms * one_tick_unit; 
	volatile uint32 timer1_ldv = VPint(CR_TIMER1_LDV);

	tick_acc = 0;
	timer_last = VPint(CR_TIMER1_VLR);
	do {
		timer_now = VPint(CR_TIMER1_VLR);
	  	if (timer_last >= timer_now) 
	  		tick_acc += timer_last - timer_now;
		else
			tick_acc += timer1_ldv - timer_now + timer_last;
		timer_last = timer_now;
	} while (tick_acc < tick_wait);
#endif
}

int eth_mac_loopback_gen_and_check(int portIdx, int pktnum){
	struct sk_buff* skb=NULL;
	int i=0,rx_port1=0,rx_port2=0,pkt_loss=0, step = 0;
    long long delay;
	char* LoopbackData;
    autobench_mac_lpbk_flag = 1;
    step = autobench_mac_step[ autobench_mac_speed[portIdx] ];

    // odd port use all ones packet, even port use all zeros packet
    if( portIdx % 2 == 1 )
        LoopbackData = LoopbackDataAllOne;
    else
        LoopbackData = LoopbackDataAllZero;
    
	for(i=0; i<pktnum; i++){
	
		skb=skbmgr_dev_alloc_skb2k();

		if(skb){
			memcpy(skb->data, LoopbackData, LOOPBACK_PKT_LEN);
			skb_put(skb, LOOPBACK_PKT_LEN);
            skb->len = LOOPBACK_PKT_LEN;
			skb->data[15] = ((0x01<<portIdx)&0xff);//tx to switch port i
			tc3262_gmac_tx_autobench(skb, lan_dev);
		}
		else{
			printk("Allocate skb failure!\r\n");
		}  
		if( i%step == step-1 ){
            delay = jiffies + 2;
            while(time_before(jiffies, delay)){
                schedule();
            }
        }
	}
	pkt_loss = pktnum - autobench_mac_lpbk_cnt_port;
    autobench_mac_lpbk_cnt_port = 0;
	return pkt_loss;
	
}

int eth_mac_port_rgmii_loopback(uint16 switch_port, uint16 phy_address)
{
	int pkt_loss=0;
	
	tc_outl(GSW_PMCR(switch_port), 0x5e33b);//port force link up
	tcMiiStationWrite(phy_address, 0, 0x4140);//enable phy loopback
	delay1ms_autobench(10);
	pkt_loss = eth_mac_loopback_gen_and_check(switch_port,2000);
	printk("RGMII mode switch port %d loopback pkt_loss = %d\r\n",switch_port,pkt_loss);
	tc_outl(GSW_PMCR(switch_port), 0x56330);
	tcMiiStationWrite(phy_address, 0, 0x3100);
	delay1ms_autobench(10);	
	return pkt_loss;	
}

void set_phy_loopback(int port_idx, uint mode)
{
	uint reg_value=0;

	if(mode == ENABLE)
	{
		reg_value = tcMiiStationRead(port_idx+8, 0);
		reg_value |= 0x4000; 
		tcMiiStationWrite(port_idx+8, 0, reg_value);//enable phy loopback @ phyaddress 4
	}
	else
	{
		reg_value = tcMiiStationRead(port_idx+8, 0);
		reg_value &= ~0x4000; 
		tcMiiStationWrite(port_idx+8, 0, reg_value);
	}
	return;
}

int gsw_slt_fail_print_gsw_p6_status(void)
{
        printk("Internal switch P6 \n");
        printk("Rx Unicase Pkts        = 0x%08lx, ", read_reg_word(GSW_RX_UNIC(6)));
        printk("Rx Multicast Pkts      = 0x%08lx\n", read_reg_word(GSW_RX_MULC(6)));
        printk("Rx Broadcast Pkts      = 0x%08lx, ", read_reg_word(GSW_RX_BROC(6)));
        printk("Rx Align Error         = 0x%08lx\n", read_reg_word(GSW_RX_ALIGE(6)));
        printk("Rx CRC Error           = 0x%08lx, ", read_reg_word(GSW_RX_CRC(6)));
        printk("Rx Under Size Pkts     = 0x%08lx\n", read_reg_word(GSW_RX_RUNT(6)));
        printk("Rx Fragment Error      = 0x%08lx, ", read_reg_word(GSW_RX_FRGE(6)));
        printk("Rx Over Size PPkts     = 0x%08lx\n", read_reg_word(GSW_RX_LONG(6)));
        printk("Rx Jabber Error        = 0x%08lx, ", read_reg_word(GSW_RX_JABE(6)));
        printk("Rx Pause Pkts          = 0x%08lx\n", read_reg_word(GSW_RX_PAUC(6)));
        printk("Rx Drop Pkts           = 0x%08lx, ", read_reg_word(GSW_RX_DROC(6)));
        printk("Rx ING Drop Pkts       = 0x%08lx\n", read_reg_word(GSW_RX_INGC(6)));
        printk("Rx ARL Drop Pkts       = 0x%08lx, ", read_reg_word(GSW_RX_ARLC(6)));
        printk("Rx FILTER Drop Pkts    = 0x%08lx\n", read_reg_word(GSW_RX_FILC(6)));
        printk("Tx Unicase Pkts        = 0x%08lx, ", read_reg_word(GSW_TX_UNIC(6)));
        printk("Tx Multicast Pkts      = 0x%08lx\n", read_reg_word(GSW_TX_MULC(6)));
        printk("Tx Broadcast Pkts      = 0x%08lx, ", read_reg_word(GSW_TX_BROC(6)));
        printk("Tx Collision           = 0x%08lx\n", read_reg_word(GSW_TX_COLC(6)));
        printk("Tx Single Collision    = 0x%08lx, ", read_reg_word(GSW_TX_SCOLC(6)));
        printk("Tx Multiple Collision  = 0x%08lx\n", read_reg_word(GSW_TX_MCOLC(6)));
        printk("Tx Deffer              = 0x%08lx, ", read_reg_word(GSW_TX_DEFC(6)));
        printk("Tx Late Collision      = 0x%08lx\n", read_reg_word(GSW_TX_LCOLC(6)));
        printk("Tx eXcessive Collision = 0x%08lx, ", read_reg_word(GSW_TX_ECOLC(6)));
        printk("Tx Pause Pkts          = 0x%08lx\n", read_reg_word(GSW_TX_PAUC(6)));
        printk("Tx Drop Pkts           = 0x%08lx\n", read_reg_word(GSW_TX_DROC(6)));
	    printk("------\n");

        return 0;
}

int gsw_slt_fail_print_fe_reg_status(void)
{
#if defined(TCSUPPORT_CPU_EN7581)||defined(TCSUPPORT_CPU_AN7552)
    printk("\nCDMA1_TX_OK_CNT           (0x%08x) = 0x%08lx\n", 0x1fb50580, read_reg_word(0x1fb50580));
	printk("CDMA1_RXCPU_OK_CNT        (0x%08x) = 0x%08lx\n", 0x1fb50590, read_reg_word(0x1fb50590));
	printk("CDMA1_RXCPU_DROP_CNT      (0x%08x) = 0x%08lx\n", 0x1fb505a0, read_reg_word(0x1fb505a0));

	printk("\nGDMA1_TX_GET_CNT          (0x%08x) = 0x%08lx\n", 0x1fb50600 , read_reg_word(0x1fb50600));
	printk("GDMA1_TX_OK_CNT_L         (0x%08x) = 0x%08lx\n", 0x1fb50604 , read_reg_word(0x1fb50604));
	printk("GDMA1_TX_OK_CNT_H         (0x%08x) = 0x%08lx\n", 0x1fb50780, read_reg_word(0x1fb50780));

	printk("GDMA1_TX_DROP_CNT         (0x%08x) = 0x%08lx\n", 0x1fb50608, read_reg_word(0x1fb50608));
	printk("GDMA1_RX_OK_CNT           (0x%08x) = 0x%08lx\n", 0x1fb50648 , read_reg_word(0x1fb50648));
	printk("GDMA1_RX_FC_DROP_CNT      (0x%08x) = 0x%08lx\n", 0x1fb5064c, read_reg_word(0x1fb5064c));
	printk("GDMA1_RX_RC_DROP_CNT      (0x%08x) = 0x%08lx\n", 0x1fb50650, read_reg_word(0x1fb50650));
	printk("GDMA1_RX_OVER_DROP_CNT    (0x%08x) = 0x%08lx\n", 0x1fb50654, read_reg_word(0x1fb50654));
	printk("GDMA1_RX_ERROR_DROP_CNT   (0x%08x) = 0x%08lx\n", 0x1fb50658, read_reg_word(0x1fb50658));

    printk("\nCDMA2_TX_OK_CNT           (0x%08x) = 0x%08lx\n", 0x1fb51580, read_reg_word(0x1fb51580));
	printk("CDMA2_RXCPU_OK_CNT        (0x%08x) = 0x%08lx\n", 0x1fb51590, read_reg_word(0x1fb51590));
	printk("CDMA2_RXCPU_DROP_CNT      (0x%08x) = 0x%08lx\n", 0x1fb515a0, read_reg_word(0x1fb515a0));

    printk("\nGDMA2_TX_GET_CNT          (0x%08x) = 0x%08lx\n", 0x1fb51600 , read_reg_word(0x1fb51600));
	printk("GDMA2_TX_OK_CNT_L         (0x%08x) = 0x%08lx\n", 0x1fb51604 , read_reg_word(0x1fb51604));
	printk("GDMA2_TX_OK_CNT_H         (0x%08x) = 0x%08lx\n", 0x1fb51780, read_reg_word(0x1fb51780));
    
    printk("GDMA2_TX_DROP_CNT         (0x%08x) = 0x%08lx\n", 0x1fb51608, read_reg_word(0x1fb51608));
	printk("GDMA2_RX_OK_CNT           (0x%08x) = 0x%08lx\n", 0x1fb51648 , read_reg_word(0x1fb51648));
	printk("GDMA2_RX_FC_DROP_CNT      (0x%08x) = 0x%08lx\n", 0x1fb5164c, read_reg_word(0x1fb5164c));
	printk("GDMA2_RX_RC_DROP_CNT      (0x%08x) = 0x%08lx\n", 0x1fb51650, read_reg_word(0x1fb51650));
	printk("GDMA2_RX_OVER_DROP_CNT    (0x%08x) = 0x%08lx\n", 0x1fb51654, read_reg_word(0x1fb51654));
	printk("GDMA2_RX_ERROR_DROP_CNT   (0x%08x) = 0x%08lx\n", 0x1fb51658, read_reg_word(0x1fb51658));

#if defined(TCSUPPORT_CPU_EN7581)
	printk("\nGDMA4_TX_GETCNT           (0x%08x) = 0x%08lx\n", 0x1fb52600, read_reg_word(0x1fb52600));
	printk("GDMA4_TX_OKCNT_L          (0x%08x) = 0x%08lx\n", 0x1fb52604, read_reg_word(0x1fb52604));
	printk("GDMA4_TX_OKCNT_H          (0x%08x) = 0x%08lx\n", 0x1fb52780, read_reg_word(0x1fb52780));
	printk("GDMA4_TX_DROPCNT          (0x%08x) = 0x%08lx\n", 0x1fb52608, read_reg_word(0x1fb52608));
	printk("GDMA4_RX_OKCNT_L          (0x%08x) = 0x%08lx\n", 0x1fb52648, read_reg_word(0x1fb52648));
	printk("GDMA4_RX_OKCNT_H          (0x%08x) = 0x%08lx\n", 0x1fb52790, read_reg_word(0x1fb52790));
	printk("GDMA4_RX_FCDROPCNT        (0x%08x) = 0x%08lx\n", 0x1fb5264c, read_reg_word(0x1fb5264c));
	printk("GDMA4_RX_RCDROPCNT        (0x%08x) = 0x%08lx\n", 0x1fb52650, read_reg_word(0x1fb52650));
	printk("GDMA4_RX_OVDROPCNT        (0x%08x) = 0x%08lx\n", 0x1fb52654, read_reg_word(0x1fb52654));
	printk("GDMA4_RX_ERRDROPCNT       (0x%08x) = 0x%08lx\n", 0x1fb52658, read_reg_word(0x1fb52658));
#endif
#endif

    return 0;
}
int eth_mac_autobench_loopback(struct ECNT_ETHER_Data *eth_data)
{
	int retValue=0,i=0,j=0,pkt_loss=0,slt_debug_loop,slt_start_phy=9,slt_end_phy=12,slt_temp,dev_1e_reg_9b,dev_1e_reg_9c,slt_debug_enable=1;
    long long delay;
	uint16 speed_mode[3]={10,100,1000};
	uint32 value_9 = 0,value_10 = 0,value_11 = 0,value_12 = 0;
	uint32 link_9 = 0,link_10 = 0,link_11 = 0,link_12 = 0;	
	uint32 slt_counter= 0,slt_counter_out = 5000;
	uint32 value = 0;
	uint start_port=0,end_port=4;

	struct rgmii_port_info rgmii_port[] = {
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527)
		{0,4},
		{5,4}
#elif defined(TCSUPPORT_CPU_EN7580)
		{5,5}
#endif
	};

    // generate loopback increment data //
    for(i=64; i<LOOPBACK_PKT_LEN; i++){
        LoopbackDataAllZero[i] = 0;
        LoopbackDataAllOne[i] = 0xFF;
    }

    memset(autobench_mac_lpbk_cnt, 0, sizeof(autobench_mac_lpbk_cnt));

	int num_of_rgmii_port = sizeof(rgmii_port)/sizeof(struct rgmii_port_info);
	
	if(SWITCH_INIT_FROM_PORT1)
		start_port=1;


	if(AUTOBENCH_7581_TEST_ONLYPORT1)
		end_port = 1;



	for(i=0; i<7; i++)
		write_reg_word(GSW_BSR(i),0);//disable broadcast ratelimit

    /*disable ifc&vip*/
    write_reg_word(0x1fb501f0,0);
    write_reg_word(0x1fb501f4,0);
	
	tc_outl(GSW_AGC, 0x71809);//disable length check

	if(!AUTOBENCH_NOT_SUPPORT_RGMII){
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527)
		tc_outl(EN7516_PORT0_MODE_SELECT_REG, EN7516_PORT0_RGMII_MODE);
#endif

		for(i=0; i<num_of_rgmii_port; i++)
			retValue += eth_mac_port_rgmii_loopback(rgmii_port[i].switch_port, rgmii_port[i].phy_address);
			
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527)
		tc_outl(EN7516_PORT0_MODE_SELECT_REG, EN7516_PORT0_GPHY_MODE);
#endif
	}
	        ephy_7581_GSW_polling_switch_on=0;
            //enable auto MAC polling phy speed
			//regWrite32(0x1fb5b100,0x56330);
			//regWrite32(0x1fb5b200,0x56330);
			//regWrite32(0x1fb5b300,0x56330);
			//regWrite32(0x1fb5b400,0x56330);
			
			//enable gsw p6 fc            
			//value = regRead32(0x1fb5b600);            
			//value |= (3<<4);            
			//regWrite32(0x1fb5b600,value);
			regWrite32(0x1fb5b600,0x205e33b);
			//regWrite32(0x1fb5b500,0x5e33b);

	for(i=0;i<3;i++){
		
        if(i==0)
        {
			regWrite32(0x1fb5b100,0x5e331);
			regWrite32(0x1fb5b200,0x5e331);
			regWrite32(0x1fb5b300,0x5e331);
			regWrite32(0x1fb5b400,0x5e331);
		}
        if(i==1)
        {
			regWrite32(0x1fb5b100,0x5e335);
			regWrite32(0x1fb5b200,0x5e335);
			regWrite32(0x1fb5b300,0x5e335);
			regWrite32(0x1fb5b400,0x5e335);
		}
        if(i==2)
        {
			regWrite32(0x1fb5b100,0x5e33d);
			regWrite32(0x1fb5b200,0x5e33d);
			regWrite32(0x1fb5b300,0x5e33d);
			regWrite32(0x1fb5b400,0x5e33d);
		}
		
	//i = autobench_mode;  // for_debug only
		for(j=start_port; j<=end_port; j++)
		{
			if(AUTOBENCH_7523_PORT1_2_TEST_10M_100M){
				if(i == 2 && (j ==1 || j ==2)){
					continue;
				}
			}
			EPHY_API_SLT_MODE(8+j, speed_mode[i]);
			//value = regRead32(0x1fb5b000+100*j);
			//value |=((1<<15)|1|(i<<2));
			//printk("port %d speed %d set \n",j,i,0x1fb5b000+0x100*j,value);
			//regWrite32(0x1fb5b000+0x100*j,value);
            autobench_mac_speed[j] = i;    // autobench_rx will fill cnt in diff rate
		    	
		}
		//enable auto MAC polling phy speed
		    //regWrite32(0x1fb5f018,0x7f7f8c08);
			//regWrite32(0x1fb5b100,0x56330);
			//regWrite32(0x1fb5b200,0x56330);
			//regWrite32(0x1fb5b300,0x56330);
			//regWrite32(0x1fb5b400,0x56330);
			//regWrite32(0x1fb5b100,0x56330);


        if(i==2)
		{
            //delay = jiffies + 3 * HZ;
            delay = jiffies + 6 * HZ;
			//printk("link delay %d 1xx \n\r",speed_mode[i]);
		}
        else
		{
            //delay = jiffies + 1 * HZ;
			delay = jiffies + 6 * HZ;
        // wait phy loopback mode link up
		}
		while(time_before(jiffies, delay)){
            schedule();
        }
		/*
		
			printk("0x1fb5f018=%x\n\r",regRead32(0x1fb5f018));
			printk("0x1fb5fc10=%x\n\r",regRead32(0x1fb5fc10));
			printk("0x1fb5b100=%x\n\r",regRead32(0x1fb5b100));
			printk("0x1fb5b108=%x\n\r",regRead32(0x1fb5b108));			
			printk("0x1fb5b110=%x\n\r",regRead32(0x1fb5b110));			
			printk("0x1fb5b200=%x\n\r",regRead32(0x1fb5b200));
			printk("0x1fb5b208=%x\n\r",regRead32(0x1fb5b208));			
			printk("0x1fb5b210=%x\n\r",regRead32(0x1fb5b210));				
			printk("0x1fb5b300=%x\n\r",regRead32(0x1fb5b300));
			printk("0x1fb5b308=%x\n\r",regRead32(0x1fb5b308));			
			printk("0x1fb5b310=%x\n\r",regRead32(0x1fb5b310));				
			printk("0x1fb5b400=%x\n\r",regRead32(0x1fb5b400));
			printk("0x1fb5b408=%x\n\r",regRead32(0x1fb5b408));			
			printk("0x1fb5b410=%x\n\r",regRead32(0x1fb5b410));				
			printk("0x1fb5b600=%x\n\r",regRead32(0x1fb5b600));
			printk("0x1fb5b608=%x\n\r",regRead32(0x1fb5b608));			
			printk("0x1fb5b610=%x\n\r",regRead32(0x1fb5b610));				
*/			
		for(j=start_port; j<=end_port; j++)
		{
			
            eth_mac_loopback_gen_and_check(j,autobench_mac_pkt_send[i]);
		}

        if(i==2)
		{
            //delay = jiffies +  HZ;
            delay = jiffies + 4 * HZ;
			//printk("packet_delay %d  \n\r",speed_mode[i]);
		}
        else
		{
            //delay = jiffies +  HZ/2;
            delay = jiffies + 3 * HZ;
		}
        // wait last pakcet receive
        while(time_before(jiffies, delay)){
            schedule();
        }
	}
ephy_7581_GSW_polling_switch_on=1;
#if defined(TCSUPPORT_HSGMII_LAN)	
	if(AUTOBENCH_7523_SUPPORT_HSGMII_LAN || AUTOBENCH_7581_SUPPORT_HSGMII_LAN){
		eth_mac_hsgmii_autobench_loopback();
		delay = jiffies +  HZ/2;
	    // wait last pakcet receive
	    while(time_before(jiffies, delay)){
	        schedule();
	    }
	}
#endif

#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)	
		if(AUTOBENCH_7523_SUPPORT_AE_WAM || AUTOBENCH_7581_SUPPORT_AE_WAM || AUTOBENCH_7552_SUPPORT_AE_WAM){
			eth_mac_hsgmii_ae_autobench_loopback();
			delay = jiffies +  HZ/2;
			// wait last pakcet receive
			while(time_before(jiffies, delay)){
				schedule();
			}
		}
#endif




    for(j=start_port; j<=end_port; j++)
    {
        for(i=0;i<3;i++){
			if(AUTOBENCH_7523_PORT1_2_TEST_10M_100M){
				if(i == 2 && (j ==1 || j ==2)){
					continue;
				}
			}
			
            printk("Port %d rate %dMbps, receive pkt %d \n", j, speed_mode[i], autobench_mac_lpbk_cnt[j][i]);
            if( autobench_mac_lpbk_cnt[j][i] < autobench_mac_pkt_send[i] )
                retValue += 1;
        }
    }

#if defined(TCSUPPORT_HSGMII_LAN)	
	if(AUTOBENCH_7523_SUPPORT_HSGMII_LAN){
		printk("Hsgmii receive pkt %d \n", autobench_mac_hsgmii_lpbk_cnt);
		if(autobench_mac_hsgmii_lpbk_cnt < autobench_mac_hsgmii_pkt_send)
			retValue += 1;
	}

	if(AUTOBENCH_7581_SUPPORT_USB0_HSGMII){
		printk("Hsgmii usb receive pkt %d \n", autobench_mac_hsgmii_usb_lpbk_cnt);
		if(autobench_mac_hsgmii_usb_lpbk_cnt < autobench_mac_hsgmii_pkt_send)
			retValue += 1;
	}

	if(AUTOBENCH_7581_SUPPORT_PCIE1_USXGMII10G){
		printk("usxgmii pcie1 receive pkt %d \n", autobench_mac_hsgmii_pcie1_lpbk_cnt);
		if(autobench_mac_hsgmii_pcie1_lpbk_cnt < autobench_mac_hsgmii_pkt_send)
			retValue += 1;
	}

	if(AUTOBENCH_7581_SUPPORT_ETHER_USXGMII10G || AUTOBENCH_7581_SUPPORT_ETHER_HSGMII2P5G){
		printk("ether serdes receive pkt %d \n", autobench_mac_hsgmii_ether_lpbk_cnt);
		if(autobench_mac_hsgmii_ether_lpbk_cnt < autobench_mac_hsgmii_pkt_send)
			retValue += 1;
	}
		
#endif
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)	
		if(AUTOBENCH_7523_SUPPORT_AE_WAM || AUTOBENCH_7581_SUPPORT_AE_WAM || AUTOBENCH_7552_SUPPORT_AE_WAM){
			printk("pon hsgmii receive pkt %d \n", autobench_mac_ae_hsgmii_lpbk_cnt);
			if(autobench_mac_ae_hsgmii_lpbk_cnt < autobench_mac_ae_hsgmii_pkt_send)
				retValue += 1;
		}
#endif


	tc_outl(GSW_AGC, 0x71819);//enable length check

	if(retValue)
	{
        gsw_slt_fail_print_gsw_p6_status();
        printk("dump fe debug:\n");
        gsw_slt_fail_print_fe_reg_status();
        if(AUTOBENCH_7552_SUPPORT_AE_WAM)
            printk("hsgmii mac tx:0x%x,rx:0x%x\n",get_xsi_data(0x1fa08108),get_xsi_data(0x1fa08188));
		return retValue;
    }
	else
		return 0;
}
#else
int eth_mac_autobench_loopback(struct ECNT_ETHER_Data *eth_data)
{
    return 0;
}
#endif


/* Register startup/shutdown routines */
module_init(eth_mac_init);
module_exit(eth_mac_exit);
module_param(lan_itf, charp, S_IRUGO);
module_param(sep_itf, charp, S_IRUGO);
module_param(wan_itf, charp, S_IRUGO);
module_param(itf_start_idx, int, S_IRUGO);
module_param(itf_num, int, S_IRUGO);


MODULE_LICENSE("Proprietary");

