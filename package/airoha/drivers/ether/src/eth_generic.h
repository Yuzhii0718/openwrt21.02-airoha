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
#ifndef _ETH_GEN_H
#define _ETH_GEN_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/dma-mapping.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/rt_flash.h>
#include <linux/version.h> 
#include <asm/io.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#ifndef TCSUPPORT_CPU_ARMV8
#include <asm/tc3162/TCIfSetQuery_os.h>
#endif
#include <linux/if_link.h>
#ifdef TCPHY_SUPPORT
#include <asm/tc3162/cmdparse.h>
#include "./tcphy/tcswitch.h"
#endif


#ifdef TCSUPPORT_RA_HWNAT
#include <linux/foe_hook.h>
#endif
#define KERNEL_2_6_36 		(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,31))

#include "eth_en7512.h"
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_ephy.h>
#include <ecnt_hook/ecnt_hook_qdma.h>

//#define LOOPBACK_SUPPORT

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifndef TCSUPPORT_QOS
#define QOS_REMARKING  1  
#endif
#define TCSUPPORT_HW_QOS

#ifdef QOS_REMARKING  
#define QOS_REMARKING_MASK    0x00000007
#define QOS_REMARKING_FLAG    0x00000001
/*#define QOS_DMAWRR_USERDEFINE  0x01*/
#endif
#if defined (QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
#define QOS_DMAWRR_USERDEFINE 0x1
#define PTQCR_WRR_EN			(1<<4)
#define PTQCR_WRR_SELECT 	 	(1<<6)
#endif


#ifdef TCSUPPORT_QOS
#ifndef QOS_FILTER_MARK
#define		QOS_FILTER_MARK		0xf0
#endif
#ifndef QOS_HH_PRIORITY
#define 	QOS_HH_PRIORITY		0x10
#endif
#define 	QOS_H_PRIORITY		0x20
#define 	QOS_M_PRIORITY		0x30
#define		QOS_L_PRIORITY		0x40
	
#define		NULLQOS				-1
#define 	QOS_SW_PQ			0	//will use hw at the same time
#define		QOS_SW_WRR			1
#define		QOS_SW_CAR			2
#define 	QOS_HW_WRR			3
#define		QOS_HW_PQ			4
#define		QOS_HW_PQ_WRR		5
	
#endif
	
#define LAN_ST_100MB        0x01
#define LAN_ST_FULL_DUPLEX	0x02
#define LAN_ST_LINK_UP      0x04
#define LAN_ST_1000MB       0x08

#define SUCCESS 0
#define FAIL 1

#define RX_MIB 1
#define TX_MIB 0

#define DROP 1

#define CB_MBGIC_ETH_NONE	0x0
#define CB_MBGIC_ETH_0	0x32620000
#define CB_MBGIC_ETH_1	0x32620010
#define CB_MBGIC_ETH_2	0x32620020
#define CB_MBGIC_ETH_3	0x32620030
#define CB_MBGIC_ETH_4	0x32620040
#define CB_MBGIC_ETH_5	0x32620050
#define CB_MBGIC_ETH_6	0x32620060
#define CB_MBGIC_ETH_7	0x32620070
#define CB_MBGIC_ETH_8	0x32620080
#define CB_MBGIC_ETH_9	0x32620090
#define CB_MBGIC_ETH_10	0x326200a0
#define SEP_ETH_ITF_0	(0)
#define SEP_ETH_ITF_1	(1)
#define SEP_ETH_ITF_2	(2)
#define SEP_ETH_ITF_3	(3)
#define SEP_ETH_ITF_4	(4)
#define SEP_ETH_ITF_5	(5)
#define SEP_ETH_ITF_6	(6)
#define SEP_ETH_ITF_7	(7)
#define SEP_ETH_ITF_8	(8)
#define SEP_ETH_ITF_9	(9)
#define SEP_ETH_ITF_10	(10)
#define SEP_ETH_ITF_UNKNOWN	(-1)

#define EXTEND_SWITCH 1
#define PURE_SWITCH 0

#define GSW_PHY_OFFSET_7   (7)
#define GSW_PHY_OFFSET_15   (15)
#define GSW_PHY_OFFSET_23   (23)
#define GSW_PHY_OFFSET_31   (31)


#define GSW_1 GSW_PHY_OFFSET_23
#define GSW_2 0 //GSW_PHY_OFFSET_15
#define GSW_3 0 //GSW_PHY_OFFSET_31


/* GSW1 , eth0.4~eth0.7 */
#define SOC_SWITCH_TO_GSW_1 5
#define CPU_PORT_OF_GSW_1   5

/* GSW2*/
#define SOC_SWITCH_TO_GSW_2 3
#define CPU_PORT_OF_GSW_2   6

/* GSW3 */
#define SOC_SWITCH_TO_GSW_3 4
#define CPU_PORT_OF_GSW_3   6


#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
/* extend one switch, add 5 port */
#define MAX_ETH_ITF_NUM	(11)
#else
#define MAX_ETH_ITF_NUM	(RT63365_SWIC_PORTNUM - 1)
#endif



#define EXTERNAL 1
#define INTERNAL 0

#define SOC_TO_EXT 0
#define EXT_TO_SOC 1

#define FHEWAN_RX_HOOK 0
#define FHEWAN_TX_HOOK 1
#define STAG_REPLACE_MODE 0
#define STAG_INSERT_MODE 1

/*zdh :for 64bit expand length come from Eth_en7512.h defined*/
#define GSW_MAX_PORT  7
#define GSW_MAX_TX_TYPE  14
#define GSW_MAX_RX_TYPE  16

/************************************************************************
*               M A C R O S
*************************************************************************
*/
#define CHK_BUF() pos = begin + index; if (pos < off) { index = 0; begin = pos; }; if (pos > off + count) goto done;

#ifdef TCSUPPORT_CPU_ARMV8
#define read_reg_word(reg) 			get_frame_engine_data(reg)
#define write_reg_word(reg, wdata) 	set_frame_engine_data(reg, wdata)
#define regRead32(reg)				get_frame_engine_data(reg)
#define regWrite32(reg, wdata)		set_frame_engine_data(reg, wdata)
//#define tc_outl(offset,val)    		(*(volatile unsigned long *)(offset) = val)
#define tc_outl(offset,val)			set_frame_engine_data(offset, val)

#else
#define read_reg_word(reg) 		regRead32(reg)
#define write_reg_word(reg, wdata) 	regWrite32(reg, wdata)
#endif
	
#define RX_BUF_LEN 			(2048 - NET_SKB_PAD - 64 - (sizeof(struct skb_shared_info)))

#if defined(TCSUPPORT_FTTDP_V2)
#define INSERT_STAG(TxMsg, skb) skb
#define REMOVE_STAG(sptag, skb) 0
#else
#define INSERT_STAG(TxMsg, skb) macEN7512STagInsert(TxMsg, skb)
#define REMOVE_STAG(rxmsg, skb) macEN7512STagRemove(rxmsg, skb)
#endif

#ifdef TCSUPPORT_AUTOBENCH
#define eth_tx_autobench    eth_mac_tx
#define eth_lan_mac_tx      eth_mac_tx_autobench
#else
#define eth_lan_mac_tx      eth_mac_tx
#endif

#define pause(x)        mdelay(x)
#define atoi(x)         simple_strtoul(x, NULL,10)


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
typedef struct _gsw_reg {
    __u32 off;
    __u32 val;
	u32 switch_id;
} gsw_reg;

/*********************
 * Mac Control Block *
 *********************/

/* ----- MIB-II ----- */
typedef struct macMIB_II_s {
    uint32 inOctets;            /* Receive Octets */
    uint32 inUnicastPkts;       /* Receive Unicast Packets */
    uint32 inMulticastPkts;     /* Receive Multicast Packets */
    uint32 inBroadcastPkts;     /* Receive Broadcast Packets */
    uint32 inDiscards;          /* Receive Discard Packets */
    uint32 inErrors;            /* Receive Error Packets */
    uint32 inUnknownProtocols;  /* Receive Unknown Prototol Packets */
    uint32 outOctets;           /* Transmit Octets */
    uint32 outUnicastPkts;      /* Transmit Unicast Packets */
    uint32 outMulticastPkts;    /* Transmit Multicast Packets */
    uint32 outBroadcastPkts;    /* Transmit Broadcast Packets */
    uint32 outDiscards;         /* Transmit Discard Packets */
    uint32 outErrors;           /* Transmit Error Packets */
} macMIB_II_t;

/* ----- Adapter Statistics ----- */
typedef struct inSiliconStat_s {
    uint32 txJabberTimeCnt;
    uint32 txLossOfCarrierCnt;
    uint32 txNoCarrierCnt;
    uint32 txLateCollisionCnt;
    uint32 txExCollisionCnt;
    uint32 txHeartbeatFailCnt;
    uint32 txCollisionCnt;
    uint32 txExDeferralCnt;
    uint32 txUnderRunCnt;

    uint32 rxAlignErr;
    uint32 rxSymbolErr;
    uint32 rxMiiErr;
    uint32 rxCrcErr;
    uint32 rxEtherFrameLengthErr; /* 60 > size(Packet) or 1518 < size(Packet) */
    uint32 rx802p3FrameLengthErr; /* value of length field of 802.3 packet is
                                     larger than real packet payload */
    uint32 rxDribblingErr;
    uint32 rxRuntErr;
    uint32 rxLongErr;
    uint32 rxCollisionErr;

    uint32 rxPktIPChkSumErr;

    uint32 rxEnQueueNum;          /* Number of packets enqueued in macRxRingproc() */
    uint32 rxDeQueueNum;          /* Number of packets dequeued in macRxToUpperTask() */
    uint32 txEnQueueNum;          /* Number of packets enqueued in macSend() */
    uint32 txDeQueueNum;          /* Number of packets dequeued in macTxRingproc() */

    uint32 rxIpCrcErr;
    uint32 rxUdpTcpCrcErr;
} inSiliconStat_t;


/* ----- Ethernet Link Profile ----- */
typedef struct macPhyLinkProfile_s {
    uint32 linkSpeed;                           /* 10Mbps or 100Mbps */
    uint32 duplexMode;                          /* Half/Full Duplex Mode */
    uint32 enetMode;
    uint32 ANCompFlag;                          /* auto_negotiation complete Flag */
    uint32 PollCount;                           /* auto_negotiation polling check count */
} macPhyLinkProfile_t;


/* ----- Statistics for GMAC ----- */
typedef struct macStat_s {
    macMIB_II_t MIB_II; /* MIB-II */
    inSiliconStat_t inSilicon;
} macStat_t;

/* ----- Adapter Card Table ------ */
typedef struct macAdapter_s {
    uint8  macAddr[6];  /* MAC-Address */
    macPhyLinkProfile_t *macPhyLinkProfile_p;
    macStat_t           macStat;
    uint8 statisticOn;  /* Flag to record statistics or not */

    uint32 resetNum;    /* Number of Reset the LAN Cont. */
    uint32 enetPhyAddr;
    uint8  enetPhyId;

    struct mii_if_info mii_if;

    spinlock_t lock;
    struct net_device_stats stats;
#if KERNEL_2_6_36
    struct net_device   *dev;
    struct napi_struct  napi;
#endif
} macAdapter_t;

typedef struct phyDeviceList_s {
    uint16 companyId;
    char vendorName[30];
} phyDeviceList_t;

#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527)
#define EN7516_PORT0_RGMII_MODE 0
#define EN7516_PORT0_GPHY_MODE 1
#define EN7516_PORT0_MODE_SELECT_REG 0xbfb5f80c
#endif
struct rgmii_port_info{
	uint16 switch_port;
	uint16 phy_address;
};

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

int eth_mac_tx(struct sk_buff *skb, struct net_device *dev);
void update_sw_mib(macAdapter_t *mib_type,struct sk_buff *skb, int direction);
uint32 calcnt(uint32 basereg);

uint32 switch_reg_read(uint32 reg);
uint32 switch_reg_write(uint32 reg, uint32 wdata);
uint32 gswPbusRead(uint32 pbus_addr);
int gswPbusWrite(uint32 pbus_addr, uint32 pbus_data);

uint32 gswPbusRead2(uint32 pbus_addr);
int gswPbusWrite2(uint32 pbus_addr, uint32 pbus_data);


uint32 gswPbusRead_extend(uint32 pbus_addr, uint32 phy_addr);
int gswPbusWrite_extend(uint32 pbus_addr, uint32 pbus_data, uint32 phy_addr);

uint32 gswRegRead_extend(uint32 reg, uint32 is_extend_gsw);
uint32 gswRegWrite_extend(uint32 reg, uint32 wdata, uint32 is_extend_gsw);

#define LIMIT_PORT_NUM 	6
#define LIMIT_BUFFER_FOR_100MB 	0
#ifdef TCSUPPORT_CPU_EN7580
#define LIMIT_BUFFER_FOR_1GB 	1
#else
#define LIMIT_BUFFER_FOR_1GB 	0
#endif
#ifdef CONFIG_TP_IMAGE
/* 发现在线速转发小包时，多1Mbps的ratelimit仍会丢包，这里增加到20Mbps */
#undef LIMIT_BUFFER_FOR_1GB
#define LIMIT_BUFFER_FOR_1GB 	20
#endif
#define WIFI_OFFLOAD_CHANNEL	7

#define DSTQ_BIT_MASK_SIZE (1<<5)

typedef struct{
	int linkState;	/* 0: down, 1: up */
	int linkSpeed;	/* 0: 10m, 1: 100m , 2: 1g */
}portInfo;

#if (__GNUC__ <= 4 && __GNUC_MINOR__ <= 3)
#ifndef TCSUPPORT_CPU_ARMV8
inline unsigned long int getPortMask(struct sk_buff *skb);
inline int setPortMask(struct sk_buff *skb, uint8 port_mask);
#else
unsigned long int getPortMask(struct sk_buff *skb);
int setPortMask(struct sk_buff *skb, uint8 port_mask);
#endif
#endif



#if defined (TCSUPPORT_CPU_EN7523)
#define MAC_TABLE_NUM_MAX	1024	
#define MAC_TABLE_MASK		0x3ff	
#else
#define MAC_TABLE_NUM_MAX	2048	
#define MAC_TABLE_MASK		0x7ff	
#endif


u32 macMT7530SwitchMap2LanPort(u32 switchPortId);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void eth_led_tx_hook(uint8 port_mask);
void eth_led_rx_hook(uint8 pvid);
#endif
unsigned long int getPortMask(struct sk_buff *skb);


//for 7580 later chip, mac tx always insert sptag
#define SPTAG_INSERT_SUPPORT (isEN7580||isEN7523||isEN7581||isAN7552||isAN7583) 

//for 751627 7528 and later chip
#define isEN751627later (isEN751627 || isEN7580 || isEN7523 ||isEN7581||isAN7552||isAN7583)


#define SWITCH_NOTSUPPORT_TRTCM 0
//switch port start from port 1
#define SWITCH_INIT_FROM_PORT1 (isEN7526c || isEN7580 || isEN7528 || isEN7523 ||isEN7581 || isAN7552||isAN7583)

#define SWITCH_SUPPORT_4PORTS_INFPGA ( isEN7580 || isEN7523 ||isEN7581 ||isAN7552||isAN7583)

//Eth mac link up auto polling by phy
#define MAC_AUTO_POLLING_BY_PHY (isEN7581||isAN7552||isAN7583)

//gdm1/2/3 support insert/replace ..mode
#define GDM_SPTAG_SUPPORT	(isEN7523 ||isEN7581||isAN7583)

#define SUPPORT_VLAN_TABLE_PBIT_REMARK	(isEN7523 ||isEN7581||isAN7552||isAN7583)

#define BURST_MONITOR_INIT_ENABLE	(isEN7523 ||isEN7581 || isAN7552||isAN7583)

#define WOE_TXRING_RATELIMIT_DISABLE (isEN7528 || isEN7580 ||isEN7523 ||isEN7581||isAN7552||isAN7583)

#define SUPPORT_GDM_SPTAG_ACTION (isEN7523 ||isEN7581||isAN7552||isAN7583)

#define SUPPORT_SET_DOUBLE_WAN_PORT (isEN751627 || isEN7580 || isEN7523||isEN7581||isAN7552||isAN7583)

//tsid port info is as meter 
#define SUPPORT_TSID_AS_METER	(isEN7580||isEN7523 ||isEN7581||isAN7552||isAN7583)

#define QDMA_TXBUFCNT_INIT_DISABLE	(isEN7523 ||isEN7581||isAN7552||isAN7583)

//from 7581 ,cport cfg in switch , early chip define in fe
#define CPORT_CFG_DEFINE_IN_SWITCH	(isEN7581||isAN7552||isAN7583)

#define CPORT_CFG_SPTAG_ENABLE	(isEN7581||isAN7552||isAN7583)

//form 7523. per port support rx/tx crc  action
#define SUPPORT_SWITCH_PERPORT_RX_ACTION	(isEN7523 ||isEN7581||isAN7552||isAN7583)

//gdm fp drop is 0xf after 7523
#define FE_DP_DROP          (isEN7523 || isEN7581 || isAN7552 || isAN7583)?(0xf):(0x7)

#define AUTOBENCH_7581_TEST_ONLYPORT1		isAN7581ST
#define AUTOBENCH_NOT_SUPPORT_RGMII			(isEN7528 || isEN7523 || isEN7581 || isAN7552)
#define AUTOBENCH_7523_SUPPORT_HSGMII_LAN	(isEN7523&&(!(isEN7523DU || isEN7523GU || isEN7523SU)))
#define AUTOBENCH_7523_SUPPORT_AE_WAM		(isEN7523&&((isEN7562DU || isEN7562DT || isEN7562CU || isEN7562GTH || isEN7562GTS || isEN7562CT)))
#define AUTOBENCH_7523_PORT1_2_TEST_10M_100M	isEN7523DU
#define AUTOBENCH_7581_SUPPORT_AE_WAM		(isEN7581 && isAN7566PT)

#define AUTOBENCH_7581_SUPPORT_USB0_HSGMII	(isEN7581 && isAN7581DT)
#define AUTOBENCH_7581_SUPPORT_PCIE1_USXGMII10G		(isEN7581 && (isAN7581ST||isAN7581FD))

#define AUTOBENCH_7581_SUPPORT_ETHER_USXGMII10G		(isEN7581 && (isAN7581GT || isAN7581PT || isAN7581IT || isAN7581ST || isAN7566PT||isAN7581FG||isAN7581FP||isAN7581FD))
#define AUTOBENCH_7581_SUPPORT_ETHER_HSGMII2P5G		(isEN7581 && (isAN7581CT || isAN7581DT || isAN7551PT))

#define AUTOBENCH_7523_HSGMII_MODE	isEN7523
#define AUTOBENCH_7581_HSGMII_MODE	isEN7581

#define AUTOBENCH_7581_SUPPORT_HSGMII_LAN	(AUTOBENCH_7581_SUPPORT_USB0_HSGMII || AUTOBENCH_7581_SUPPORT_PCIE1_USXGMII10G || AUTOBENCH_7581_SUPPORT_ETHER_USXGMII10G || AUTOBENCH_7581_SUPPORT_ETHER_HSGMII2P5G)

#define AUTOBENCH_7552_SUPPORT_AE_WAM		(isAN7552&&(isAN7563CT || isAN7563PT))

#endif /*_XXX_H*/
