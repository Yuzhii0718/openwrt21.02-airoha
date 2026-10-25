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
#ifndef _AE_WAN_MAC_H_
#define _AE_WAN_MAC_H_

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/

#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/rt_flash.h>
#include <asm/tc3162/tc3162.h>
#include <ecnt_hook/ecnt_hook_ae_mac_type.h>




#include <linux/foe_hook.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define driver_ae_wan_hook_name    "driver_ae_wan_hook"

#define XSI_TX 0
#define XSI_RX 1

#define DOWN 0
#define UP 1

//7523 phy speed
typedef enum{
	SGMII_SPEED_2500M,
	SGMII_SPEED_1000M,
	SGMII_SPEED_100M,
	SGMII_SPEED_10M,
	SGMII_SPEED_UNKNOW
}sgmii_speed_type;

//7581 support speed
typedef enum{
	USXGII_10G,
	USXGII_5G,
	USXGII_2p5G,
	USXGII_1G,
	USXGII_100M,
	HSGMII_2p5G,
	SGMII_1000M,
	SGMII_100M,
	SGMII_10M,
	SPEED_UNKNOW,
}xsgmii_speed_type;

typedef enum{
	ENUM_USXGII_10G,
	ENUM_USXGII_5G,
	ENUM_USXGII_2p5G,
	ENUM_USXGII_1G,
	ENUM_USXGII_100M,
	ENUM_USXGII_UNKNOW,
}USXGMII_RATE_t;


typedef enum{
	USXGMII,
	HSGMII,
	SGMII,
}xSGMII_t;

typedef enum{
	ENUM_SGMII_1000M,
	ENUM_SGMII_100M,
	ENUM_SGMII_10M,
	ENUM_SGMII_UNKNOW,
}SGMII_RATE_t;	


typedef enum{
	ENUM_VERSION,
	ENUM_ETH_SERDES,
	ENUM_XPON_SERDES,
	ENUM_PCIE0_SERDES,
	ENUM_PCIE1_SERDES,
	ENUM_USB_SERDES,
	ENUM_UNKNOW_SERDES,
}xsgmii_serdes_inphy;	


//0: pon serdrs as 2.5G wan; 1: usb as 2.5G WAN; 2:pcie0 as 2.5G wan; 3:pcie1 as 2.5G WAN 4:for 7581 eth as wan


typedef enum{
	ENUM_PON_SERDES_ASWAN,
	ENUM_USB_SERDES_ASWAN,
	ENUM_PCIE0_SERDES_ASWAN,
	ENUM_PCIE1_SERDES_ASWAN,
	ENUM_ETH_SERDES_ASWAN,
	ENUM_UNKNOW_SERDES_ASWAN,
}serdes_as_wan;	




//SPORT DEFINE
#define SPORT_QDMA_LAN          0
#define SPORT_GDMA1				1
#define SPORT_GDMA2				2
#define SPORT_GDMA3				3
#define SPORT_PPE				4
#define SPORT_QDMA_WAN          5//send to CPU via QDMA
#define SPORT_QDMA_HW			6//send to CPU via QDMA HW
#define SPORT_DISCARD			7
#define SPORT_CPU				DPORT_PDMA
    
//FPORT DEFINE
#define DPORT_PDMA				0
#define DPORT_GDMA1				1
#define DPORT_GDMA2				2
#define DPORT_GDMA3				3
#define DPORT_GDMA4				9
#define DPORT_PPE				4
#define DPORT_QDMA				5//send to CPU via QDMA
#define DPORT_QDMA_HW			6//send to CPU via QDMA HW
#define DPORT_DISCARD			7
#define DPORT_CPU				DPORT_PDMA

typedef struct xsiStat_s {
    uint32 inOctets;            /* Receive Octets */
    uint32 inUnicastPkts;       /* Receive Unicast Packets */
    uint32 inMulticastPkts;     /* Receive Multicast Packets */
    uint32 inBroadcastPkts;     /* Receive Broadcast Packets */
    uint32 inErrors;            /* Receive Error Packets */
	uint32 inDrops;
    uint32 outOctets;           /* Transmit Octets */
    uint32 outUnicastPkts;      /* Transmit Unicast Packets */
    uint32 outMulticastPkts;    /* Transmit Multicast Packets */
    uint32 outBroadcastPkts;    /* Transmit Broadcast Packets */
    uint32 outErrors;           /* Transmit Error Packets */
	uint32 outDrops;
} xsiStat_t;


typedef struct xsiWanAdapter_s {
    uint8  macAddr[6];
    xsiStat_t	xsiStat;
    uint8 statisticOn;
    struct net_device_stats stats;
    struct net_device   *dev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    struct napi_struct          napi ;
#endif 
} xsiWanAdapter_t;

#define XSI_MAC_STATISTIC_ON		1
#define XSI_MAC_STATISTIC_OFF		0

typedef enum {
	XSI_DBG_ERR,
	XSI_DBG_ST,
	XSI_DBG_WARN,
	XSI_DBG_MSG
} XSI_DebugLevel_t ;

#ifdef XSI_DEBUG
	#define XSI_MSG(level, F, B...)	{ \
											if(xsi_dbg_level >= level) 	\
												printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B) ; \
										}
	#define XSI_ERR(F, B...)			printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B)
	#define XSI_LOG(F, B...)			printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B)
#else
	#define XSI_MSG(level, F, B...)			
	#define XSI_ERR(F,B...)			printk(F, ##B) ; 
	#define XSI_LOG(F,B...)			printk(F, ##B) ; 
#endif

#define read_reg_word(reg) 		regRead32(reg)
#define write_reg_word(reg, wdata) 	regWrite32(reg, wdata)

#endif

#if defined(TCSUPPORT_CPU_EN7523)
#if defined(TCSUPPORT_SWITCH_8851)
#define SUPPORT_EN8851 1
#else
#define SUPPORT_EN8851 0
#endif
#endif