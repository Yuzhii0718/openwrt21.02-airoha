/*
** $Id: $
*/
/************************************************************************
 *
 *	Copyright (C) 2012 Mediatek Inc.
 *	All Rights Reserved.
 *
 * Mediatek Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Mediatek Inc.
 *
 *************************************************************************/
/*
** $Log$
**
 */

#ifdef TCSUPPORT_MT7530_SWITCH_API

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <asm/tc3162/tc3162.h>
#include "tcversion.h"


#ifdef TCPHY_SUPPORT
#include <asm/tc3162/cmdparse.h>

#endif
#include "../eth_generic.h"
#include "tcswitch.h"
#include "phy_api.h"
#include "mtkswitch_api.h"
#include <linux/foe_hook.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <linux/libcompileoption.h>
#include "../eth_ext_switch.h"
#include <ecnt_hook/ecnt_hook_fe.h>


/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/

/* define  */
#define QOS_IN_SWITCH 0
#define QOS_IN_QDMA   1

#define ARL_IPTBL_STATUS_STATIC 0x3
#define ARL_IPTBL_STATUS_STATIC_BY_SIP_PORTMAP 0x2
/************************************************************************
*                            M A C R O S
*************************************************************************
*/

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
struct mii_data {
      u16   phy_id;
      u16   reg_num;
      u16   val_in;
      u16   val_out;
};

enum HGSMII_LAN_PORT
{
	HSGMII_USB = 0x09,
	HSGMII_PCIE0,
	HSGMII_PCIE1,
	HSGMII_ETH,
};

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
u32 macMT7530ReadReg2(u32 gsw_reg, u8 external);
int macMT7530WriteReg2(u32 gsw_reg, u32 gsw_data, u8 external);
	
#define IO_SMASK(reg, ext_switch, mask, shift, value)       { uint t = macMT7530ReadReg2(reg, ext_switch); macMT7530WriteReg2(reg, ((t&~(mask))|((value<<shift)&mask)), ext_switch); }


/************************************************************************
*                        P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                      E X T E R N A L   D A T A
*************************************************************************
*/
extern uint8 hal_api_ext;
extern int wan_port_id;
extern int use_soc_lan;
extern int vport_enable;
extern int ds_qos;
extern unsigned int queuemark_enable;
extern unsigned int queuemark_shift;
extern unsigned int queuemark_width;

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
extern eth_port_vlan_t eth_port_vlan[];
extern char extend_switch_port;
extern char inter_switch_lanport_num;
extern char lan_port_map_ext[];
#endif
#define EXT_SWITCH_FLAG 0x80
#define SWITCH_PORT_MASK 0x7f

extern uint8 interface_map[TRAFFIC_TYPE_MAX];

extern int pausetime_for_test;

#ifdef TCSUPPORT_QOS
extern int qos_flag;
#endif

/*soft 64 bit flag reserve*/
extern unsigned long long Expand_64_Tx[GSW_MAX_TX_TYPE][GSW_MAX_PORT] ;
extern unsigned long long Expand_64_Rx[GSW_MAX_RX_TYPE][GSW_MAX_PORT] ;

extern unsigned int g_Tx_Last[GSW_MAX_TX_TYPE][GSW_MAX_PORT] ;
extern unsigned int g_Rx_Last[GSW_MAX_RX_TYPE][GSW_MAX_PORT] ;

/************************************************************************
*                       P R I V A T E   D A T A
*************************************************************************
*/
static u8 vlanbind_matrix[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
static u8 portbind_matrix[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
static u8 macMT7530_APIDbgDumpEn = 0;

enum portName_t{
    LAN1=0,
    LAN2,
    LAN3,
    LAN4,
    CPU_PORT,
    WAN_PORT,
    SWITCH_OTHER_PORT
};

char *portNameList[MAX_PORT_NAME_NUM] = {
    "Logic Port:0", /* 0x0 */
    "Logic Port:1",
    "Logic Port:2",
    "Logic Port:3",
    "CPU Port",
    "WAN Port",
    "Switch Other Port",
};

// This table Used by macMT7530SetAutoDetection() and macMT7530GetAutoDetection()
/*               *Speed*    *Duplex*
**        0x00 - Auto;      Auto
**        0x01 - 10 only;   Full duplex only
**        0x02 - 100 only;  Full duplex only
**        0x03 - 1000 only; Full duplex only
**        0x04 - Auto;      Full duplex only
**        0x10 - 10 only;   Auto
**        0x11 - 10 only;   Half duplex only
**        0x12 - 100 only;  Half duplex only
**        0x13 - 1000 only; Half duplex only
**        0x14 - Auto;      Half duplex only
**        0x20 - 1000 only; Auto
**        0x30 - 100 only;  Auto
*/
const static mt7530_switch_MIILinkType MIIMediaOption[]={
//	{index_param, MII_BMCR_value(0x00), MII_ADVERTISE_value(0x04), MII_CTRL1000_value(0x09)}
	{0x00, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_10FULL | ADVERTISE_10HALF | ADVERTISE_100FULL | ADVERTISE_100HALF), (ADVERTISE_1000FULL | ADVERTISE_1000HALF)},
//	{0x00, (BMCR_ANENABLE), 0, 0},	// if ADVERTISE_CSMA = 0, means all?
	{0x01, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_10FULL), 0},
	{0x02, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_100FULL), 0},
	{0x03, (BMCR_ANENABLE), (ADVERTISE_CSMA), (ADVERTISE_1000FULL)},
	{0x04, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_10FULL | ADVERTISE_100FULL), (ADVERTISE_1000FULL)},
	{0x10, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_10FULL | ADVERTISE_10HALF), 0},
	{0x11, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_10HALF), 0},
	{0x12, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_100HALF), 0},
	{0x13, (BMCR_ANENABLE), (ADVERTISE_CSMA), (ADVERTISE_1000HALF)},
	{0x14, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_10HALF | ADVERTISE_100HALF), (ADVERTISE_1000HALF)},
	{0x20, (BMCR_ANENABLE), (ADVERTISE_CSMA), (ADVERTISE_1000FULL | ADVERTISE_1000HALF)},
	{0x30, (BMCR_ANENABLE), (ADVERTISE_CSMA | ADVERTISE_100FULL | ADVERTISE_100HALF), 0},
};
#define MIIMediaOption_Entries (sizeof(MIIMediaOption) / sizeof(MIIMediaOption[0]))

#if (0)
// MII_BMCR(0x00) : Basic mode control register.
#define BMCR_SPEED100           0x2000  /* Select 100Mbps              */
#define BMCR_SPEED1000		0x0040  /* MSB of Speed (1000)         */
#define BMCR_FULLDPLX           0x0100  /* Full duplex                 */
#define BMCR_ANENABLE           0x1000  /* Enable auto negotiation     */
#define BMCR_ANRESTART          0x0200  /* Auto negotiation restart    */
#define BMCR_RESET              0x8000  /* Reset the DP83840           */
// MII_ADVERTISE(0x04) : Advertisement control register.
#define ADVERTISE_CSMA          0x0001  /* Only selector supported     */
#define ADVERTISE_10FULL        0x0040  /* Try for 10mbps full-duplex  */
#define ADVERTISE_10HALF        0x0020  /* Try for 10mbps half-duplex  */
#define ADVERTISE_100FULL       0x0100  /* Try for 100mbps full-duplex */
#define ADVERTISE_100HALF       0x0080  /* Try for 100mbps half-duplex */
#define ADVERTISE_1000XFULL     0x0020  /* Try for 1000BASE-X full-duplex */
#define ADVERTISE_1000XHALF     0x0040  /* Try for 1000BASE-X half-duplex */
// MII_CTRL1000(0x09) : 1000BASE-T Control register
#define ADVERTISE_1000FULL      0x0200  /* Advertise 1000BASE-T full duplex */
#define ADVERTISE_1000HALF      0x0100  /* Advertise 1000BASE-T half duplex */
#endif


// This table Used by macMT7530GetConfigurationStatus()
/*
**         0x01 : 10BaseT full duplex
**         0x02 : 100BaseT full duplex
**         0x03 : Gigabit Ethernet full duplex
**         0x11 : 10BaseT half duplex
**         0x12 : 100BaseT half duplex
**         0x13 : Gigabit Ethernet half duplex
*/
#define mt7530MediaStatus_Entries (6)
const static u8 mt7530MediaStatus[mt7530MediaStatus_Entries][2] = {
	//user-defined param, 0x3n08[3:0]
	{0x01, ((0 << 2) + (1 << 1) + 1)},
	{0x02, ((1 << 2) + (1 << 1) + 1)},
	{0x03, ((2 << 2) + (1 << 1) + 1)},
	{0x11, ((0 << 2) + (0 << 1) + 1)},
	{0x12, ((1 << 2) + (0 << 1) + 1)},
	{0x13, ((2 << 2) + (0 << 1) + 1)},
};
//#define mt7530MediaStatus_Entries (sizeof(mt7530MediaStatus) / sizeof(mt7530MediaStatus[0]))

#if (0)
const static u16 MIIMediaStatus[][3] ={
	{0x01, (BMSR_10FULL), 0},
	{0x02, (BMSR_100FULL), 0},
	{0x03, 0, (LPA_1000FULL)},
	{0x11, (BMSR_10HALF), 0},
	{0x12, (BMSR_100HALF), 0},
	{0x13, 0, (LPA_1000HALF)},
};
#define MIIMediaStatus_Entries (sizeof(MIIMediaStatus) / sizeof(MIIMediaStatus[0]))

#if (0)
#define MII_BMSR            0x01        /* Basic mode status register  */
#define MII_STAT1000        0x0a        /* 1000BASE-T status           */
/* MII_BMSR (0x01) : Basic mode status register. */
#define BMSR_10HALF             0x0800  /* Can do 10mbps, half-duplex  */
#define BMSR_10FULL             0x1000  /* Can do 10mbps, full-duplex  */
#define BMSR_100HALF            0x2000  /* Can do 100mbps, half-duplex */
#define BMSR_100FULL            0x4000  /* Can do 100mbps, full-duplex */
/* MII_STAT1000 (0x0a) : 1000BASE-T Status register */
#define LPA_1000FULL            0x0800  /* Link partner 1000BASE-T full duplex */
#define LPA_1000HALF            0x0400  /* Link partner 1000BASE-T half duplex */
#endif
#endif

// This table Used by macMT7530GetConfigurationStatus()
/*
**         0 : DCE or MDI-X.
**         1 : DTE or MDI.
**         2 : Automatic selection. (default)
*/
#define mt7530DTEDCEInd_Entries (4)
const static u8 mt7530DTEDCEInd[mt7530DTEDCEInd_Entries][2] = {
	//user-defined param, PHY register tReg05[4:3]
	{0, (3 << 3)},
	{1, (2 << 3)},
	{2, (0 << 3)},
	{2, (1 << 3)},
};

static u8 fcStatus = 0;
/*ds qos*/
static unsigned int p6_fc_status = 0;
extern int (*ra_sw_nat_hook_clean_table)(void);

int ext_switch_led_control = DRIVER_CTL_LED;
/************************************************************************
*        F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
u8 getPortNameIdxByPortid(int portid);

/*___________________________________________________________________
**      function name:
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/01
**_______________________________________________________________
*/

/*___________________________________________________________________
**      function name:
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/19
**_______________________________________________________________
*/
#if (1)
extern uint8 use_ext_switch;

u32 macMT7530ReadReg(u32 gsw_reg)
{
	return (switch_reg_read(GSW_BASE + gsw_reg));
}

int macMT7530WriteReg(u32 gsw_reg, u32 gsw_data)
{
	return (switch_reg_write((GSW_BASE + gsw_reg), gsw_data));
}

/*
read reg of internal switch or extend switch, use extsw_flag to distinguish

*/
u32 macMultiMT7530ReadReg(u32 gsw_reg, u32 extsw_flag)
{
    if(extsw_flag)
        return gswPbusRead_extend(gsw_reg, GSW_PHY_OFFSET_23);
    else
	    return (switch_reg_read(GSW_BASE + gsw_reg));
}

int macMultiMT7530WriteReg(u32 gsw_reg, u32 gsw_data, u32 extsw_flag)
{
    if(extsw_flag)
        return gswPbusWrite_extend(gsw_reg, gsw_data, GSW_PHY_OFFSET_23);
    else
	    return (switch_reg_write((GSW_BASE + gsw_reg), gsw_data));
}


u32 macMT7530PHYReadReg(u32 phy_addr, u32 phy_reg)
{
#ifdef TCSUPPORT_CPU_ARMV8
	return (tcMiiStationRead(phy_addr, phy_reg));
#else
	#ifdef EXT_SWITCH_PHY_CONNECT_INT_MDIO
		#if defined(JUDGE_SWITCH_SCENARIO_BY_751020_SUBMODEL)
	if (DefaultUseExtMT7530)
	{
		phy_addr += EXT_SWITCH_PHY_ADDR_OFFSET;
	}
		#elif defined(DEFAULT_USE_EXT_SWIC)
	phy_addr += EXT_SWITCH_PHY_ADDR_OFFSET;
		#else
		#endif
	return (tcMiiStationRead(phy_addr, phy_reg));
	#else
		#if defined(JUDGE_SWITCH_SCENARIO_BY_751020_SUBMODEL)
	if (DefaultUseExtMT7530)
	{
		return (tcMiiStationRead(phy_addr, phy_reg));
//		return (gswPmiRead(phy_addr, phy_reg));
	}
	else
	{
		return (tcMiiStationRead(phy_addr, phy_reg));
	}
		#elif defined(DEFAULT_USE_EXT_SWIC)
	return (gswPmiRead(phy_addr, phy_reg));
		#else
	return (tcMiiStationRead(phy_addr, phy_reg));
		#endif
	#endif
#endif

}

u32 macMT7530PHYWriteReg(u32 phy_addr, u32 phy_reg, u32 phy_data)
{
#ifdef TCSUPPORT_CPU_ARMV8
	tcMiiStationWrite(phy_addr, phy_reg, phy_data);
	return 0;
#else	

	#ifdef EXT_SWITCH_PHY_CONNECT_INT_MDIO
		#if defined(JUDGE_SWITCH_SCENARIO_BY_751020_SUBMODEL)
	if (DefaultUseExtMT7530)
	{
		phy_addr += EXT_SWITCH_PHY_ADDR_OFFSET;
	}
		#elif defined(DEFAULT_USE_EXT_SWIC)
	phy_addr += EXT_SWITCH_PHY_ADDR_OFFSET;
		#else
		#endif
	tcMiiStationWrite(phy_addr, phy_reg, phy_data);
	#else
		#if defined(JUDGE_SWITCH_SCENARIO_BY_751020_SUBMODEL)
	if (DefaultUseExtMT7530)
	{
		tcMiiStationWrite(phy_addr, phy_reg, phy_data);
//		return (gswPmiWrite(phy_addr, phy_reg, phy_data));
	}
	else
	{
		tcMiiStationWrite(phy_addr, phy_reg, phy_data);
	}
		#elif defined(DEFAULT_USE_EXT_SWIC)
	return (gswPmiWrite(phy_addr, phy_reg, phy_data));
		#else
	tcMiiStationWrite(phy_addr, phy_reg, phy_data);
		#endif
	#endif
		return (0);
#endif
}
#else
#define macMT7530ReadReg(switch_no, gsw_reg) ((gswPbusRead(gsw_reg)))
#define macMT7530WriteReg(witch_no, gsw_reg, gsw_data) (gswPbusWrite(gsw_reg, gsw_data)))
#define macMT7530PHYReadReg(switch_no, phy_addr, phy_reg) (gswPmiRead(phy_addr, phy_reg)))
#define macMT7530PHYWriteReg(switch_no, phy_addr, phy_reg, phy_data) (gswPmiWrite(phy_addr, phy_reg, phy_data)))
#endif

/*___________________________________________________________________
**      function name: macMT7530GetBrglearningInd
**      descriptions:
**         get port bridge learning status (disable/enable)
**
**      corresponding register:
**         0x2P0C[4] SA_DIS
**
**      parameters:
**         u8 port
**
**      global:
**
**      return:
**         0:disable, 1:enable
**         -1: failure
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/22
**_______________________________________________________________
*/
int macMT7530GetBrglearningInd(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x200C + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", 
	    __FUNCTION__, port, reg, value);
	value = (value >> 4) & 0x01;
	return value;
}

int macMT7530SetBrglearningInd(u8 port, u8 Ind)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x200C + (0x100 * port);
	value = ((macMultiMT7530ReadReg(reg, extsw_flag) & ~(1 << 4))) | ((Ind ? 1 : 0) << 4);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", 
	    __FUNCTION__, port, reg, value);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetPortBrgInd
**      descriptions:
**         get bridging between UNI ports status (disable/enable)
**
**      corresponding register:
**         0x3P00[14] MAC_TX_EN_Pn
**         0x3P00[13] MAC_RX_EN_Pn
**
**      parameters:
**         u8 port
**
**      global:
**
**      return:
**         0:disable, 1:enable
**         -1: failure
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/22
**_______________________________________________________________
*/
int macMT7530GetPortBrgInd(u8 port, u8 *Ind)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x2004 + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", 
	    __FUNCTION__, port, reg, value);

	*Ind = (value >> 16) & 0xff;
	return 0;
}

int macMT7530SetPortBrgInd(u8 port, u8 Ind)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
	if(TCSUPPORT_BBF_247_VAL!=1)
    {/* 247 Զ˿ڸʱOMCI  òȷҪOMCI ɴ */
		if (port >= MT7530_TOTAL_PORTS) return (-1);
		reg = 0x2004 + (0x100 * port);

		value = macMultiMT7530ReadReg(reg, extsw_flag) & 0xff00fffc | (Ind << 16);
		
		macMultiMT7530WriteReg(reg, value, extsw_flag);
		if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", 
		    __FUNCTION__, port, reg, value);
    }
	return (0);
}

#ifdef TCSUPPORT_PON_VLAN
EXPORT_SYMBOL(macMT7530GetPortBrgInd);
EXPORT_SYMBOL(macMT7530SetPortBrgInd);
#endif

/*___________________________________________________________________
**      function name: macMT7530GetDiscardUnknownMacInd
**      descriptions:
**         get discard unknown Mac address status (disable/enable)
**
**      corresponding register:
**         0x0010[31:24] BC_FFP
**         0x0010[23:16] MC_FFP
**         0x0010[15:8]  UC_FFP
**
**      parameters:
**         u8 port
**
**      global:
**
**      return:
**         0:Discard, 1:Not Discard
**         -1: failure
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/25
**_______________________________________________________________
*/
int macMT7530GetDiscardUnknownMacInd(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x0010;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);
	value = (((value >> port) & 0x01010100) == 0x01010100) ? 1 : 0;
	return value;
}

int macMT7530SetDiscardUnknownMacInd(u8 port, u8 Ind)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x0010;
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & ~(0x01010100 << port)) | ((Ind ? 0x01010100 : 0) << port);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetAgeTime
**      descriptions:
**         get address table age time (in seconds)
**
**      corresponding register:
**         0x00A0[19:12] AGE_CNT, default=0x95
**         0x00A0[11:0]  AGE_UNIT, default=1
**         --> (AGE_CNT+1)*(AGE_UNIT+1) seconds
**
**      parameters:
**         u32 *MaxAgeTime
**
**      global:
**
**      return:
**         0: ok
**         -1: failure
**
**         u32 *outptr_MaxAgeTime: MaxAgeTime (in seconds)
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/25
**_______________________________________________________________
*/
int macMT7530GetAgeTime(u32 *outptr_MaxAgeTime)
{
	u32 reg, value, age_cnt, age_unit;

	reg = 0x00A0;
	value = macMT7530ReadReg(reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() (%08X) : %08X\n", __FUNCTION__, reg, value);
	age_cnt = 1 + ((value >> 12) & 0x00FF);
	age_unit = 1 + (value & 0x0FFF);
	(*outptr_MaxAgeTime) = age_cnt * age_unit;
	return value;
}

int macMT7530SetAgeTime(u32 MaxAgeTime)
{
	u32 reg, value, age_cnt, age_unit;
	int i;
	int ret_val=0;

	if ((MaxAgeTime > (1 << 20)) || (MaxAgeTime == 0))
	{
		ret_val = -1;
	}
	else
	{
		reg = 0x00A0;
		value = macMT7530ReadReg(reg);
		age_unit = 1 + (value & 0x0FFF);
		age_cnt = MaxAgeTime / age_unit;
		// try to keep previous age_unit

		if ((age_cnt > (1 << 8)) || (age_cnt < 1))	// if unit too small or too large
		{
			for (i = 20; i > 0; i --)	// find leading first nonzero bit
			{
				if ((MaxAgeTime >> i) & 1) break;
			}
			age_unit = (i > 8) ? (1 << (i - 8)) : 1;
			age_cnt = MaxAgeTime / age_unit;
		}
	}
	if (!ret_val)
	{
		// (AGE_CNT+1)*(AGE_UNIT+1) seconds
		age_cnt = (age_cnt) ? ((age_cnt - 1) & 0x00FF) : 0;
		age_unit = (age_unit) ? ((age_unit - 1) & 0x0FFF) : 0;
		value = (value & ~0x000FFFFF) + (age_cnt << 12) + (age_unit);
		macMT7530WriteReg(reg, value);
        #if defined(TCSUPPORT_MULTI_SWITCH_EXT)
        if (isEN7526G)
        {
            macMultiMT7530WriteReg(reg, value, 1);
        }
        #endif
		if (macMT7530_APIDbgDumpEn) printk("%s() (%08X) : %08X\n", __FUNCTION__, reg, value);
	}

	return ret_val;
}

int getPortByMask(u8 mask, u8 *port);
u8 getPortRevertFWC(u8 in_port);
/*___________________________________________________________________
**      function name: macMT7530GetBRTable
**      descriptions:
**         get switch bridge info
**
**      corresponding register:
**         0x0080, 0x0084, 0x0088, 0x008C
**
**      parameters:
**         switch_brtable *brtable
**	      u8 port
**
**      global:
**
**      return:
**         0: ok
**         -1: failure
**
**         u8 *brinfo
**
**      call:
**
**      revision:
**_______________________________________________________________
*/
int macMT7530GetBRTable(mt7530_switch_brtable *brtable, u8 port)
{
	u32 value = 0, portId = 0;
	int ret_val=0, loop_timeout = 0, ret = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	loop_timeout = 1000000;
	/*  wait until not busy */
	while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)
	{
		if (--loop_timeout == 0)
		{
			ret_val = -1;
			break;
		}
	}

	/*  trigger 1st search , 3'b100 : Start search command (reset to 1st entry)  */
	value = (1 << REG_ATC_BUSY_OFFT) + 
		(0 << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);
	macMultiMT7530WriteReg(REG_ATC_ADDR, value, extsw_flag);
	loop_timeout = 1000000;
	/* wait until not busy */
	while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)
	{
		if (--loop_timeout == 0)
		{
			ret_val = -1;
			break;
		}
	}

	while (1)
	{
		value = macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag);
		if ( ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
			|| (brtable->count >= MAC_TABLE_NUM_MAX) )
		{
			ret_val = 0;

			/* printk("...Address Table end...\n", value); */
			break;
		}

		if(value & REG_AT_SRCH_HIT_MASK)
		{
			mt7530_switch_macinfo *fbr = &(brtable->macinfo[brtable->count]);

			/* read Portmap */
			value = macMultiMT7530ReadReg(REG_ATRD_ADDR, extsw_flag);

			/* PORT */
			ret = getPortByMask((value>>4)&0xFF, &fbr->port);
			if ( 0 != ret )
				break;

			if ( 0 == port /*ALL*/
				|| (0 != port && port == fbr->port) ) /* one port */
			{
				/* TIMER */
				fbr->timer = (value>>24)&0xFF;
				/* MAC */
				value = macMultiMT7530ReadReg(REG_TSRA1_ADDR, extsw_flag);
				fbr->mac[0] = (value >> 24) & 0xFF;
				fbr->mac[1] = (value >> 16) & 0xFF;
				fbr->mac[2] = (value >>  8) & 0xFF;
				fbr->mac[3] = (value      ) & 0xFF;
				value = macMultiMT7530ReadReg(REG_TSRA2_ADDR, extsw_flag);
				fbr->mac[4] = (value >> 24) & 0xFF;
				fbr->mac[5] = (value >> 16) & 0xFF;
				/*Vid*/
				fbr->vid = value & 0xFFF;
				if(fbr->vid == (fbr->port+1))  /* vid is pvid, consider as untag.if the vid == pvid, will be consider as untag*/
				{
					fbr->vid = 0xFFFF;
				}

				brtable->count ++;
				
				//if new mac_info == old mac_info , no need add this mac info
				if( brtable->count > 1 )
				{
					/*pointer to previous one*/
					mt7530_switch_macinfo *fbr_pre = &(brtable->macinfo[brtable->count-2]);
					if( (fbr->port == fbr_pre->port) && (fbr->vid == fbr_pre->vid) && (0 == memcmp(fbr->mac, fbr_pre->mac, 6)) )
						brtable->count --;
				}
			}
		}

		/* if not found, trigger next search, 3'b101 : Next search command (next entry) */
		value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT)
		+ (5 << REG_ATC_AC_CMD_OFFT);
		macMultiMT7530WriteReg(REG_ATC_ADDR, value, extsw_flag);

		loop_timeout = 1000000;
		/* wait until not busy */
		while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)
		{
			if (--loop_timeout == 0)
			{
				ret_val = -1;
				break;
			}
		}
	}

	return ret_val;
}

/*___________________________________________________________________
**      function name: macMT7530GetAllBRTable
**      descriptions:
**         get switch all mac entrys
**
**      corresponding register:
**         0x0080, 0x0084, 0x0088, 0x008C
**
**      parameters:
**		mt7530_switch_brtable *brtable
**
**      global:
**
**      return:
**         0: success
**         -1: failure
**
**      call:
**
**      revision:
**_______________________________________________________________
*/
int macMT7530GetAllBRTable(mt7530_switch_brtable *brtable)
{
	u32 value = 0;
	int ret_val=0, loop_timeout = 0, ret = 0;

	loop_timeout = 1000000;
	/*  wait until not busy */
	while (macMT7530ReadReg(REG_ATC_ADDR) & REG_ATC_BUSY_MASK)
	{
		if (--loop_timeout == 0)
		{
			ret_val = -1;
			break;
		}
	}

	/*  trigger 1st search , 3'b100 : Start search command (reset to 1st entry)  */
	value = (1 << REG_ATC_BUSY_OFFT) + 
		(0 << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);
	macMT7530WriteReg(REG_ATC_ADDR, value);
	loop_timeout = 1000000;
	/* wait until not busy */
	while (macMT7530ReadReg(REG_ATC_ADDR) & REG_ATC_BUSY_MASK)
	{
		if (--loop_timeout == 0)
		{
			ret_val = -1;
			break;
		}
	}

	while (1)
	{
		value = macMT7530ReadReg(REG_ATC_ADDR);
		if ( ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
			|| (brtable->count >= MAC_TABLE_NUM_MAX) )
		{
			ret_val = 0;
			break;
		}

		if(value & REG_AT_SRCH_HIT_MASK)
		{
			mt7530_switch_macinfo *fbr = &(brtable->macinfo[brtable->count]);

			/* read Portmap */
			value = macMT7530ReadReg(REG_ATRD_ADDR);

			/* get PORT info*/
			ret = getPortByMask((value>>4)&0xFF, &fbr->port);
			if ( 0 != ret )
				break;

			/* TIMER */
			fbr->timer = (value>>24)&0xFF;
			/* MAC */
			value = macMT7530ReadReg(REG_TSRA1_ADDR);
			fbr->mac[0] = (value >> 24) & 0xFF;
			fbr->mac[1] = (value >> 16) & 0xFF;
			fbr->mac[2] = (value >>  8) & 0xFF;
			fbr->mac[3] = (value      ) & 0xFF;
			value = macMT7530ReadReg(REG_TSRA2_ADDR);
			fbr->mac[4] = (value >> 24) & 0xFF;
			fbr->mac[5] = (value >> 16) & 0xFF;
			/*Vid*/
			fbr->vid = value & 0xFFF ;
			if(fbr->vid == (fbr->port+1))  /* vid is pvid, consider as untag.if the vid == pvid, will be consider as untag*/
			{
				fbr->vid = 0xFFFF;
			}
			brtable->count ++ ;
				
			//if new mac_info == old mac_info , no need add this mac info
			if( brtable->count > 1 )
			{
				/*pointer to previous one*/
				mt7530_switch_macinfo *fbr_pre = &(brtable->macinfo[brtable->count-2]);
				if( (fbr->port == fbr_pre->port) && (fbr->vid == fbr_pre->vid) && (0 == memcmp(fbr->mac, fbr_pre->mac, 6)) )
					brtable->count --;
			}
		}

		/* if not found, trigger next search, 3'b101 : Next search command (next entry) */
		value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT)
		+ (5 << REG_ATC_AC_CMD_OFFT);
		macMT7530WriteReg(REG_ATC_ADDR, value);

		loop_timeout = 1000000;
		/* wait until not busy */
		while (macMT7530ReadReg(REG_ATC_ADDR) & REG_ATC_BUSY_MASK)
		{
			if (--loop_timeout == 0)
			{
				ret_val = -1;
				break;
			}
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        loop_timeout = 1000000;
        /*  wait until not busy */
        while (macMultiMT7530ReadReg(REG_ATC_ADDR, 1) & REG_ATC_BUSY_MASK)
        {
            if (--loop_timeout == 0)
            {
                ret_val = -1;
                break;
            }
        }
        
        /*  trigger 1st search , 3'b100 : Start search command (reset to 1st entry)  */
        value = (1 << REG_ATC_BUSY_OFFT) + 
            (0 << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);
        macMultiMT7530WriteReg(REG_ATC_ADDR, value, 1);
        loop_timeout = 1000000;
        /* wait until not busy */
        while (macMultiMT7530ReadReg(REG_ATC_ADDR, 1) & REG_ATC_BUSY_MASK)
        {
            if (--loop_timeout == 0)
            {
                ret_val = -1;
                break;
            }
        }
        
        while (1)
        {
            value = macMultiMT7530ReadReg(REG_ATC_ADDR, 1);
            if ( ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
                || (brtable->count >= MAC_TABLE_NUM_MAX) )
            {
                ret_val = 0;
                break;
            }
        
            if(value & REG_AT_SRCH_HIT_MASK)
            {
                mt7530_switch_macinfo *fbr = &(brtable->macinfo[brtable->count]);
        
                /* read Portmap */
                value = macMultiMT7530ReadReg(REG_ATRD_ADDR, 1);
        
                /* get PORT info*/
                ret = getPortByMask((value>>4)&0xFF, &fbr->port);
                if ( 0 != ret )
                    break;
        
                /* TIMER */
                fbr->timer = (value>>24)&0xFF;
                /* MAC */
                value = macMultiMT7530ReadReg(REG_TSRA1_ADDR, 1);
                fbr->mac[0] = (value >> 24) & 0xFF;
                fbr->mac[1] = (value >> 16) & 0xFF;
                fbr->mac[2] = (value >>  8) & 0xFF;
                fbr->mac[3] = (value      ) & 0xFF;
                value = macMultiMT7530ReadReg(REG_TSRA2_ADDR, 1);
                fbr->mac[4] = (value >> 24) & 0xFF;
                fbr->mac[5] = (value >> 16) & 0xFF;
                /*Vid*/
                fbr->vid = value & 0xFFF ;
                if(fbr->vid == (fbr->port+1))  /* vid is pvid, consider as untag.if the vid == pvid, will be consider as untag*/
                {
                    fbr->vid = 0xFFFF;
                }
                brtable->count ++ ;
                    
                //if new mac_info == old mac_info , no need add this mac info
                if( brtable->count > 1 )
                {
                    /*pointer to previous one*/
                    mt7530_switch_macinfo *fbr_pre = &(brtable->macinfo[brtable->count-2]);
                    if( (fbr->port == fbr_pre->port) && (fbr->vid == fbr_pre->vid) && (0 == memcmp(fbr->mac, fbr_pre->mac, 6)) )
                        brtable->count --;
                }
            }
        
            /* if not found, trigger next search, 3'b101 : Next search command (next entry) */
            value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT)
            + (5 << REG_ATC_AC_CMD_OFFT);
            macMultiMT7530WriteReg(REG_ATC_ADDR, value, 1);
        
            loop_timeout = 1000000;
            /* wait until not busy */
            while (macMultiMT7530ReadReg(REG_ATC_ADDR, 1) & REG_ATC_BUSY_MASK)
            {
                if (--loop_timeout == 0)
                {
                    ret_val = -1;
                    break;
                }
            }
        }

    }
#endif

	return ret_val;
}

/*___________________________________________________________________
**      function name: macMT7530GetPortMac
**      descriptions:
**         get port mac address
**
**      corresponding register:
**         0x0080, 0x0084, 0x0088, 0x008C
**
**      parameters:
**         u8 port
**         u8 *PortMac
**
**      global:
**
**      return:
**         0: ok
**         -1: failure
**
**         u8 *PortMac
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/26
**_______________________________________________________________
*/
int macMT7530GetPortMac(u8 port, u8 *outptr_PortMac)	// "u8 portMac[6];
{
	u32 value;
	int ret_val=0, loop_timeout;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	loop_timeout = 1000000;
	while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)	// wait until not busy
	{
		if (--loop_timeout == 0)
		{
			ret_val = -1;
			break;
		}
	}

	// trigger 1st search
	value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);	// 3'b100 : Start search command (reset to 1st entry)
	macMultiMT7530WriteReg(REG_ATC_ADDR, value, extsw_flag);
	loop_timeout = 1000000;
	while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)	// wait until not busy
	{
		if (--loop_timeout == 0)
		{
			ret_val = -1;
			break;
		}
	}

	while (1)
	{
		value = macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag);
		if (!(value & REG_AT_SRCH_HIT_MASK) || (value & REG_AT_SRCH_END_MASK))
		{
			ret_val = -1;

			// printk("...Address Table end...\n", value);
			break;
		}

		// read Portmap
		value = (macMultiMT7530ReadReg(REG_ATRD_ADDR, extsw_flag) & REG_ATWD_PORT_MASK) >> REG_ATWD_PORT_OFFT;

		if (value & (1 << port))
		{
			value = macMultiMT7530ReadReg(REG_TSRA1_ADDR, extsw_flag);
			outptr_PortMac[0] = (value >> 24) & 0xFF;
			outptr_PortMac[1] = (value >> 16) & 0xFF;
			outptr_PortMac[2] = (value >>  8) & 0xFF;
			outptr_PortMac[3] = (value      ) & 0xFF;

			value = macMultiMT7530ReadReg(REG_TSRA2_ADDR, extsw_flag);
			outptr_PortMac[4] = (value >> 24) & 0xFF;
			outptr_PortMac[5] = (value >> 16) & 0xFF;

			break;
		}

		// if not found, trigger next search
		value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT);	// 3'b101 : Next search command (next entry)
		macMultiMT7530WriteReg(REG_ATC_ADDR, value, extsw_flag);

		loop_timeout = 1000000;
		while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)	// wait until not busy
		{
			if (--loop_timeout == 0)
			{
				ret_val = -1;
				break;
			}
		}
	}

	return ret_val;
}

/*___________________________________________________________________
**      function name: macMT7530GetPortStatus
**      descriptions:
**         get port status
**
**      corresponding register:
**         0x2P00[1:0] FID0_PST
**         0x3P08[0] MAC_LNK_STS_Pn
**
**      parameters:
**         u8 port
**
**      global:
**
**      return:
**         0: Disabled
**         1: Listening
**         2: Learning
**         3: Forwarding
**         4: Blocking (same as 1 for MT7530 h/w)
**         5: Linkdown
**         6: (R)Stp_off (N/A)
**         -1: failure
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/26
**_______________________________________________________________
*/
int macMT7530GetPortStatus(u8 port)
{
	u32 reg, value;
	int ret_val=0;
	const u8 stp_state_mapping[4] =
	{
		0,	// Disable/Discarding
		1, 	// Blocking/Listening/Discarding
		2,	// Learning
		3,	// Forwarding
	};
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
	
	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x2000 + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	ret_val = stp_state_mapping[value & 0x0003];
	
	reg = 0x3008  + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	
	if ((value & 0x0001) == 0)	// =0: link down
	{
		ret_val = 5;
	}
	return ret_val;
}

/*___________________________________________________________________
**      function name: macMT7530SetPortStatus
**      descriptions:
**         set port  stp status
**
**      corresponding register:
**         0x2P00[1:0] FID0_PST
**         0x3P08[0] MAC_LNK_STS_Pn
**
**      parameters:
**         u8 port
**         u8 stp_Status
**         0: Disabled
**         1: // Blocking/Listening/Discarding
**         2: Learning
**         3: Forwarding
**      call:
**
**      revision:
**_______________________________________________________________
*/
int macMT7530SetPortStatus(u8 port, u8 stp_Status)
{
	u32 reg, value;
	int ret_val=0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
	
	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x2000 + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	
	value = ((value & (~(0x0003))) | (stp_Status & 0x03));
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	
	if (macMT7530_APIDbgDumpEn) printk("%s() (%08X) : %08X\n", __FUNCTION__, reg, value);

	return ret_val;
}

/*___________________________________________________________________
**      function name: macMT7530GetMIBCounter
**      descriptions:
**         ...
**
**      parameters:
**         mt7530_switch_api_MibCntType MibCntType
**         u8 port
**         u32 *Out_Cnt_ptr
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/01
**_______________________________________________________________
*/
int macMT7530GetMIBCounter(mt7530_switch_api_MibCntType MibCntType, u8 port, unsigned long long* outptr_Cnt)
{
	u32 reg;
	unsigned long long value = 0;
	unsigned int tmp;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
//	reg = 0x4000 + (0x100 * port) + (MibCntType * 4);
	reg = MibCntType + (0x100 * port);
	value = (unsigned long long)(macMultiMT7530ReadReg(reg, extsw_flag));

	/*expend 64bit software workaround*/
	if(MibCntType - (GSW_MIB_BASE-GSW_BASE) <= 0x4c)
	{
		tmp = (MibCntType - (GSW_MIB_BASE-GSW_BASE) )/0x04;
		if (18 == tmp)
			value += (*(*(Expand_64_Tx + 12)+port)<<32);
		else if (19 == tmp)
			value += (*(*(Expand_64_Tx + 13)+port)<<32);
		else if (tmp <= 11)
			value += (*(*(Expand_64_Tx + tmp)+port)<<32);
		
		
	}
	else if(MibCntType - (GSW_MIB_BASE-GSW_BASE) >= 0x60 && MibCntType - (GSW_MIB_BASE-GSW_BASE) <= 0xb8)
	{
		tmp = (MibCntType - (GSW_MIB_BASE-GSW_BASE)-0x60)/0x04;
		if (18 == tmp)
			value += (*(*(Expand_64_Rx + 12)+port)<<32);
		else if (19 == tmp)
			value += (*(*(Expand_64_Rx + 13)+port)<<32);
		else if (21 == tmp)
			value += (*(*(Expand_64_Rx + 14)+port)<<32);
		else if (22 == tmp)
			value += (*(*(Expand_64_Rx + 15)+port)<<32);
		else if (tmp <= 11)
			value += (*(*(Expand_64_Rx + tmp)+port)<<32);	
	}
	
	(*outptr_Cnt) = value;
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %16llX , %llu \n", __FUNCTION__, port, reg, value, *outptr_Cnt);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetQueuePriority
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/10
**_______________________________________________________________
*/
int macMT7530SetQueuePriority(u16 priority, u8 queueId)
{
	u32 reg, value;
	u32 bit_offset;

	if ((priority > 7) || (queueId >= MT7530_QUEUE_NUM)) return (-1);

	reg = 0x0048 + ((priority / 2)*4);
	bit_offset = (priority % 2) ? 24 : 8;
	value = (macMT7530ReadReg(reg) & ~(0x7 << bit_offset)) | (queueId << bit_offset);
	macMT7530WriteReg(reg, value);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, queueId, reg, value);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetMaximumQueueSize
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/10
**_______________________________________________________________
*/
int macMT7530GetMaximumQueueSize(u8 queueId)
{
	// 8 queues share total 256 units
	// every unit = 2 blocks = 2 * 256 bytes
	// every queue max 256 * 2 * 256 / 8 = 16384 bytes (0x4000)
	// but FC_BLK_THD_Q0~Q7 only have 4 bits to select units = 15 * 2 * 256 = 7680 (0x1E00)
	return (15 * MT7530_FC_BLK_UNIT_SIZE);
}

/*___________________________________________________________________
**      function name: macMT7530SetAllocatedQueueSize
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/10
**_______________________________________________________________
*/
int macMT7530SetAllocatedQueueSize(u16 AllocatedQueueSize, u8 queueId)
{
	u32 reg, value;

	if (queueId >= MT7530_QUEUE_NUM) return (-1);
#if (1)
	AllocatedQueueSize = ((u32)AllocatedQueueSize + MT7530_FC_BLK_UNIT_SIZE - 1) / MT7530_FC_BLK_UNIT_SIZE;
	if (AllocatedQueueSize > 15) return (-1);	// every queue threshold max 15 units (every unit = 2 * 256 bytes)

	reg = 0x1FE4;
	value = (macMT7530ReadReg(reg) & ~(0x0F << (queueId * 4))) | (AllocatedQueueSize << (queueId * 4));
	macMT7530WriteReg(reg, value);
#else
	AllocatedQueueSize = ((u32)AllocatedQueueSize + (128 * 2) - 1) / (128 * 2);
	if (AllocatedQueueSize > (256 / 8)) return (-1);	// every queue max 32 units (every unit = 2 * 128 bytes)

	reg = 0x1FF4;
	value = macMT7530ReadReg(reg) & 0xFF;
	value |= (AllocatedQueueSize & 0xFF);
	macMT7530WriteReg(reg, value);
#endif

	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, queueId, reg, value);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetAllocatedQueueSize
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/10
**_______________________________________________________________
*/
int macMT7530GetAllocatedQueueSize(u8 queueId)
{
	u32 reg, value;

	if (queueId >= MT7530_QUEUE_NUM) return (-1);

#if (1)
	reg = 0x1FE4;
	value = macMT7530ReadReg(reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, queueId, reg, value);
	value = ((value >> (queueId * 4)) & 0x0F) * MT7530_FC_BLK_UNIT_SIZE;
#else
	reg = 0x1FF4;
	value = macMT7530ReadReg(reg) & 0x00FF;
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, queueId, reg, value);
	value *= 128 * 2;	// max 255*2*128=65280(0xFF00)
#endif
	return (value);
}

/*___________________________________________________________________
**      function name: macMT7530SetWeight
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/12
**_______________________________________________________________
*/
int macMT7530SetWeight(u8 weight, u8 queueId, u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);
	if ((weight > 16) || (weight == 0)) return (-1);	// weight : 1~16

	reg = 0x1004 + (0x0100 * port) + (0x0008 * queueId);
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & ~(0x0F << 24)) | ((weight - 1) << 24);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d,%2d (%08X) : %08X\n", __FUNCTION__, port, queueId, reg, value);

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetWeight
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/12
**_______________________________________________________________
*/
int macMT7530GetWeight(u8 queueId, u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);

	reg = 0x1004 + (0x0100 * port) + (0x0008 * queueId);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d,%2d (%08X) : %08X\n", __FUNCTION__, port, queueId, reg, value);
	value = 1 + ((value >> 24) & 0x0F);

	return (value);
}

/*___________________________________________________________________
**      function name: macMT7530SetMACForceFC
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/10
**_______________________________________________________________
*/
int macMT7530SetMACForceFC(u8 port,u8 mode)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	reg = 0x3000 + (0x100 * port);
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & (~(0x3<<4)) ) | ( mode ? 0x3<<4 : 0 );
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetBackPressure
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/10
**_______________________________________________________________
*/
int macMT7530SetBackPressure(mt7530_switch_api_backPressure_t *backPressure_p, u8 queueId, u8 port)
{
	u32 reg, value;
	u32 max_threshold, min_threshold;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);

	max_threshold = (backPressure_p->MaxQueueThreshold) / MT7530_FC_BLK_UNIT_SIZE;
	min_threshold = (backPressure_p->MinQueueThreshold) / MT7530_FC_BLK_UNIT_SIZE;
#if (1)
	if ((max_threshold > 255) || (min_threshold > 15)) return (-1);

	reg = 0x1FE0;
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & ~(1 << 31)) | ((backPressure_p->Enable) ? (1 << 31) : 0);
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	reg = 0x3000 + (0x100 * port);
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & ~(1 << 8)) | ((backPressure_p->Enable) ? (1 << 8) : 0);
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	reg = 0x1FE4;
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & ~(0x0F << (queueId * 4))) | (min_threshold << (queueId * 4));
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	reg = 0x1FF4;
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & ~(0xFF)) | max_threshold;
	macMultiMT7530WriteReg(reg, value, extsw_flag);
#else
	if ((max_threshold > 255) || (min_threshold > 255)) return (-1);

	reg = 0x1FE0;
	value = macMT7530ReadReg(reg);
	value &= ~((1 << 31) | (0xFF << 8) | (0xFF));
	value |= (((backPressure_p->Enable) ? 1 : 0) << 31) | (max_threshold << 8) | (min_threshold);
	macMT7530WriteReg(reg, value);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, queueId, reg, value);
#endif
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetBackPressure
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/10
**_______________________________________________________________
*/
int macMT7530GetBackPressure(mt7530_switch_api_backPressure_t *backPressure_p, u8 queueId, u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);

#if (1)
	backPressure_p->Enable = 0;

	reg = 0x1FE0;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (value & (1 << 31))	// FC_EN
	{
		reg = 0x3000 + (0x100 * port);
		value = macMultiMT7530ReadReg(reg, extsw_flag);
		if (value & (1 << 8))	// BACKPR_EN_Pn
		{
			backPressure_p->Enable = 1;
		}
	}

	reg = 0x1FE4;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	backPressure_p->MinQueueThreshold = MT7530_FC_BLK_UNIT_SIZE * ((value >> (4 * queueId)) & 0x0F);

	reg = 0x1FF4;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	backPressure_p->MaxQueueThreshold = MT7530_FC_BLK_UNIT_SIZE * (value & 0xFF);
#else
	reg = 0x1FE0;
	value = macMT7530ReadReg(reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, queueId, reg, value);

	backPressure_p->Enable = (value & (1 << 31)) ? 1 : 0;
	backPressure_p->MinQueueThreshold = value & 0xFF;
	backPressure_p->MaxQueueThreshold = (value >> 8) & 0xFF;
#endif

	reg = 0x3008 + (0x100 * port);
	value = (macMultiMT7530ReadReg(reg, extsw_flag) >> 2) & 0x03;
	switch (value)
	{	// return result in us; back-pressure fixed 64 bytes length + 96 bytes IPG
		case 0 :
			backPressure_p->time = ((64 + 96) * 8) / 10;
			break;
		case 1 :
			backPressure_p->time = ((64 + 96) * 8) / 100;
			break;
		case 2 :
			backPressure_p->time = ((64 + 96) * 8) / 1000;
			break;
		default :
			backPressure_p->time = ((64 + 96) * 8) / 10;
			break;
	}

	return (0);
}

static mt7530_switch_DropPolicy_t DropPoliySettings[MT7530_TOTAL_PORTS][MT7530_QUEUE_NUM];

static mt7530_switch_api_trafficDescriptor_t TrafficDescSettings[MT7530_TOTAL_PORTS][2];

const static int PortQueueACLRuleTblEntry[MT7530_TOTAL_PORTS][MT7530_QUEUE_NUM] =
{
	{ 0,  1,  2,  3,  4,  5,  6,  7},
	{ 8,  9, 10, 11, 12, 13, 14, 15},
	{16, 17, 18, 19, 20, 21, 22, 23},
	{24, 25, 26, 27, 28, 29, 30, 31},
	{32, 33, 34, 35, 36, 37, 38, 39},
	{40, 41, 42, 43, 44, 45, 46, 47},
	{48, 49, 50, 51, 52, 53, 54, 55},
};

const static int PortQueueACLActionTblEntry[MT7530_TOTAL_PORTS][MT7530_QUEUE_NUM] =
{
	{ 0,  1,  2,  3,  4,  5,  6,  7},
	{ 8,  9, 10, 11, 12, 13, 14, 15},
	{16, 17, 18, 19, 20, 21, 22, 23},
	{24, 25, 26, 27, 28, 29, 30, 31},
	{32, 33, 34, 35, 36, 37, 38, 39},
	{40, 41, 42, 43, 44, 45, 46, 47},
	{48, 49, 50, 51, 52, 53, 54, 55},
};

static int macMT7530ACLAllocateEntry(u8 port, u8 queueId)
{
	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);
	if ((PortQueueACLRuleTblEntry[port][queueId] == -1) || (PortQueueACLActionTblEntry[port][queueId] == -1))
	{
		// use fixed table
	}
	if ((PortQueueACLRuleTblEntry[port][queueId] < 0) || (PortQueueACLRuleTblEntry[port][queueId] >= MT7530_ACL_RULE_NUM)) return (-1);
	if ((PortQueueACLActionTblEntry[port][queueId] < 0) || (PortQueueACLActionTblEntry[port][queueId] >= MT7530_ACL_ACTION_NUM)) return (-1);
	return (PortQueueACLRuleTblEntry[port][queueId]);
}

static int macMT7530ACLReleaseEntry(u8 port, u8 queueId)
{
	// use fixed table
	return (0);
}

static int macMT7530ACLEntryWrite(u8 table_type_rw, u8 acl_entry, u32 word_lo, u32 word_hi)
{
	u32 reg, value, timeout;

	if (acl_entry >= MT7530_ACL_RULE_NUM) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	macMT7530WriteReg(0x94, word_lo);
	macMT7530WriteReg(0x98, word_hi);

	value = (1 << 31) | (table_type_rw << 12) | acl_entry;
	macMT7530WriteReg(reg, value);
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	return (0);
}

static int macMT7530ACLPortEnable(u8 port, u8 enable)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x2004 + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value = (enable) ? (value | (1 << 10)) : (value & ~(1 << 10));	// ACL_EN 0x2n04[10]
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	return (0);
}

static int macMT7530QueuePriorityInit(void)
{
	u32 reg, value;

	reg = 0x48;
	value = macMT7530ReadReg(reg);
	value &= ~((7 << 8) | (7 << 11) | (7 << 24) | (7 << 27));
	value |=  ((0 << 8) | (0 << 11) | (1 << 24) | (1 << 27)) ;
	macMT7530WriteReg(reg, value);

	reg = 0x4C;
	value = macMT7530ReadReg(reg);
	value &= ~((7 << 8) | (7 << 11) | (7 << 24) | (7 << 27));
	value |=  ((2 << 8) | (2 << 11) | (3 << 24) | (3 << 27)) ;
	macMT7530WriteReg(reg, value);

	reg = 0x50;
	value = macMT7530ReadReg(reg);
	value &= ~((7 << 8) | (7 << 11) | (7 << 24) | (7 << 27));
	value |=  ((4 << 8) | (4 << 11) | (5 << 24) | (5 << 27)) ;
	macMT7530WriteReg(reg, value);

	reg = 0x54;
	value = macMT7530ReadReg(reg);
	value &= ~((7 << 8) | (7 << 11) | (7 << 24) | (7 << 27));
	value |=  ((6 << 8) | (6 << 11) | (7 << 24) | (7 << 27)) ;
	macMT7530WriteReg(reg, value);

	return (0);
}

static int macMT7530ACLRuleEntryEnable(u8 acl_rule_entry, u8 enable)
{
	u32 reg, value, timeout;

	if (acl_rule_entry >= MT7530_ACL_RULE_NUM) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	value = (1 << 31) | (MT7530_ACL_RULE_TBL_READ << 12) | acl_rule_entry;
	macMT7530WriteReg(reg, value);
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	value = macMT7530ReadReg(0x98);			// ACL Rule Table bit [32+19] : "EN"
	value = (enable) ? (value | (1 << 19)) : (value & ~(1 << 19));
	macMT7530WriteReg(0x98, value);

	value = (1 << 31) | (MT7530_ACL_RULE_TBL_WRITE << 12) | acl_rule_entry;
	macMT7530WriteReg(reg, value);
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	return (0);
}

static int macMT7530ACLActionEntryEnable(u8 acl_action_entry, u8 enable)
//static int macMT7530ACLActionEntryEnable(u8 acl_action_entry, u32 mask_wordlo, u32 mask_wordhi)
{
	u32 reg, value, timeout;

	if (acl_action_entry >= MT7530_ACL_ACTION_NUM) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	if (enable)
	{
		if (acl_action_entry < 32)
		{
			macMT7530WriteReg(0x94, (1 << acl_action_entry));	// [31:0]
			macMT7530WriteReg(0x98, 0x00000000);			// [63:32]
		}
		else
		{
			macMT7530WriteReg(0x94, 0x00000000);
			macMT7530WriteReg(0x98, (1 << (acl_action_entry - 32)));
		}
	}
	else
	{
		macMT7530WriteReg(0x94, 0x00000000);
		macMT7530WriteReg(0x98, 0x00000000);
	}
//	macMT7530WriteReg(0x94, mask_wordlo);
//	macMT7530WriteReg(0x98, mask_wordhi);

	value = (1 << 31) | (MT7530_ACL_MASK_TBL_WRITE << 12) | acl_action_entry;	// 9 : write specific ACL Mask Table entry
	macMT7530WriteReg(reg, value);
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	return (0);
}

static int macMT7530MarkDEIYellow(u8 port)
{
	int i;
	int acl_rule_entry, acl_action_entry;
	u32 word_lo, word_hi;
	u32 rule_mask;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	macMT7530ACLPortEnable(extsw_flag ? (port & EXT_SWITCH_FLAG) : port, 1);

	for (i = 0; i < MT7530_QUEUE_NUM; i ++)
	{
		macMT7530ACLRuleEntryEnable(PortQueueACLRuleTblEntry[port][i], 0);
		macMT7530ACLActionEntryEnable(PortQueueACLActionTblEntry[port][i], 0);
		macMT7530ACLReleaseEntry(port, i);
	}
	//macMT7530ACLAllocateEntry(port, 0); remove for coverity

	acl_rule_entry = PortQueueACLRuleTblEntry[port][0];
	acl_action_entry = PortQueueACLActionTblEntry[port][0];

	if ((acl_rule_entry == -1) || (acl_action_entry == -1)) return (-1);

	word_lo = (0xFFFF << 16) | 0x8100;			// double check 0x8100 tag
	word_hi = (1 << 19) | (0 << 16) | (port << 8) | 0x0C;	// check BP 0x0C
	macMT7530ACLEntryWrite(MT7530_ACL_RULE_TBL_WRITE, (acl_rule_entry + 7), word_lo, word_hi);

	word_lo = (0x1000 << 16) | 0x1000;			// check DEI bit
	word_hi = (1 << 19) | (0 << 16) | (port << 8) | 0x0E;	// check BP 0x0E
	macMT7530ACLEntryWrite(MT7530_ACL_RULE_TBL_WRITE, acl_rule_entry, word_lo, word_hi);

	rule_mask = (1 << 7) | (1 << 0);		// entry (8N+7) && (8N+0)
	if (port < 4)
	{
		word_lo = rule_mask << (port << 3);
		word_hi = 0x00000000;
	}
	else
	{
		word_lo = 0x00000000;
		word_hi = rule_mask << ((port - 4) << 3);
	}
	macMT7530ACLEntryWrite(MT7530_ACL_MASK_TBL_WRITE, acl_action_entry, word_lo, word_hi);

	word_lo = 0x0000;
	word_hi = (2 << 17);	// force yellow
	macMT7530ACLEntryWrite(MT7530_ACL_CTRL_TBL_WRITE, acl_action_entry, word_lo, word_hi);

	return (0);
}

static int macMT7530MarkPriorityYellow(u8 port, u8 Priority)
{
	int acl_rule_entry, acl_action_entry;
	u32 word_lo, word_hi;
	u32 rule_mask;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if ((Priority != 4) && (Priority != 2) && (Priority != 0)) return (-1);

	acl_rule_entry = PortQueueACLRuleTblEntry[port][0];
	acl_action_entry = PortQueueACLActionTblEntry[port][0];

	if ((acl_rule_entry == -1) || (acl_action_entry == -1)) return (-1);

	word_lo = (0xFFFF << 16) | 0x8100;			// double check 0x8100 tag
	word_hi = (1 << 19) | (0 << 16) | (port << 8) | 0x0C;	// check BP 0x0C
	macMT7530ACLEntryWrite(MT7530_ACL_RULE_TBL_WRITE, (acl_rule_entry + 7), word_lo, word_hi);

	acl_rule_entry += Priority;
	acl_action_entry += Priority;

	if (Priority == 4)	// check 4DE
	{
		word_lo = ((0x07 << 13) << 16) | (4 << 13);		// check PCP=4
		word_hi = (1 << 19) | (0 << 16) | (port << 8) | 0x0E;	// check BP 0x0E
		macMT7530ACLEntryWrite(MT7530_ACL_RULE_TBL_WRITE, acl_rule_entry, word_lo, word_hi);
	}
	else if (Priority == 2)	// check 2DE
	{
		word_lo = ((0x07 << 13) << 16) | (2 << 13);		// check PCP=2
		word_hi = (1 << 19) | (0 << 16) | (port << 8) | 0x0E;	// check BP 0x0E
		macMT7530ACLEntryWrite(MT7530_ACL_RULE_TBL_WRITE, acl_rule_entry, word_lo, word_hi);
	}
	else if (Priority == 0)	// check 0DE
	{
		word_lo = ((0x07 << 13) << 16) | (0 << 13);		// check PCP=0
		word_hi = (1 << 19) | (0 << 16) | (port << 8) | 0x0E;	// check BP 0x0E
		macMT7530ACLEntryWrite(MT7530_ACL_RULE_TBL_WRITE, acl_rule_entry, word_lo, word_hi);
	}

	rule_mask = (1 << 7) | (1 << Priority);		// entry (8N+7) && (8N+x)
	if (port < 4)
	{
		word_lo = rule_mask << (port << 3);
		word_hi = 0x00000000;
	}
	else
	{
		word_lo = 0x00000000;
		word_hi = rule_mask << ((port - 4) << 3);
	}
	macMT7530ACLEntryWrite(MT7530_ACL_MASK_TBL_WRITE, acl_action_entry, word_lo, word_hi);

	word_lo = 0x0000;
	word_hi = (2 << 17);	// force yellow
	macMT7530ACLEntryWrite(MT7530_ACL_CTRL_TBL_WRITE, acl_action_entry, word_lo, word_hi);

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetXPYD
**      descriptions:
**         ...
**
**      parameters:
**         DropPrecdenceColourMarking :
**           3:8P0D (DEI as yellow)
**           4:7P1D (DEI + 4DE as yellow)
**           5:6P2D (DEI + 4DE + 2DE as yellow)
**           6:5P3D (DEI + 4DE + 2DE +0DE as yellow)
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/11/27
**_______________________________________________________________
*/
static int macMT7530SetXPYD(u8 port, u8 DropPrecdenceColourMarking)
{
	u8 i;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if ((DropPrecdenceColourMarking < 3) || (DropPrecdenceColourMarking > 6)) return (-1);

	macMT7530QueuePriorityInit();
	macMT7530ACLPortEnable(port, 1);

	for (i = 0; i < MT7530_QUEUE_NUM; i ++)
	{
		macMT7530ACLRuleEntryEnable(PortQueueACLRuleTblEntry[port][i], 0);
		macMT7530ACLActionEntryEnable(PortQueueACLActionTblEntry[port][i], 0);
		macMT7530ACLReleaseEntry(port, i);
	}
	//macMT7530ACLAllocateEntry(port, 0); remove for coverity

	if (DropPrecdenceColourMarking >= 4)			// 7P1D, 6P2D, 5P3D
	{
		macMT7530MarkPriorityYellow(port, 4);		// check 4DE

		if (DropPrecdenceColourMarking >= 5)		// 6P2D, 5P3D
		{
			macMT7530MarkPriorityYellow(port, 2);	// check 2DE

			if (DropPrecdenceColourMarking >= 6)	// 5P3D
			{
				macMT7530MarkPriorityYellow(port, 0);	// check 0DE
			}
		}
	}

	return (0);
}

static int macMT7530SetDropPercent(u8 port, u8 queueId, mt7530_switch_DropPolicy_t *DropPolicy)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);

	reg = 0x180C + (0x100 * port) + (0x04 * queueId);
	value = DropPolicy->yellowPacketDropMaxn_p;	// parameter in 8 bits : 255=100%
	value = value >> 5;				// hw in 3 bits : 7=87.5%
	value |= (100 << 12) | (50 << 0);		// high threshold 100 pages, low threshold 50 pages
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	reg = 0x180C + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value |= (1 << 31);		// enable Pn_DP_en
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetDropPolicy
**      descriptions:
**         ...
**
**      parameters:
**         DropPrecdenceColourMarking :
**           0:No Marking
**           1:from Traffic Descriptor
**           2:DEI Marking
**           3:8P0D, 4:7P1D, 5:6P2D, 6:5P3D
**           7:reserved
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/11/26
**_______________________________________________________________
*/
//int macMT7530SetDropPolicy(u8 port, u8 queueId, u16 greenPacketDropQueueMaxThr, u16 greenPacketDropQueueMinThr, u16 yellowPacketDropQueueMaxThr, u16 yellowPacketDropQueueMinThr, u8 greenPacketDropMax_p, u8 yellowPacketDropMaxn_p, u8 QueueDropW_q, u8 DropPrecdenceColourMarking)
int macMT7530SetDropPolicy(u8 port, u8 queueId, mt7530_switch_DropPolicy_t *DropPolicy)
{
	int value, i, acl_rule_entry, acl_action_entry;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);
	if ((DropPolicy->DropPrecdenceColourMarking == 7) || (DropPolicy->DropPrecdenceColourMarking >= 8)) return (-1);	// reserved or invalid

	value = macMT7530ACLAllocateEntry(port, queueId);
	if (value == -1)
	{
		macMT7530ACLReleaseEntry(port, queueId);
		return (-1);
	}
	acl_rule_entry = PortQueueACLRuleTblEntry[port][queueId];
	acl_action_entry = PortQueueACLActionTblEntry[port][queueId];

	switch (DropPolicy->DropPrecdenceColourMarking)
	{
		case 0 :	// "No Marking"
			macMT7530ACLRuleEntryEnable(acl_rule_entry, 0);
			macMT7530ACLActionEntryEnable(acl_action_entry, 0);
			macMT7530ACLReleaseEntry(port, queueId);
			break;

		case 1 :	// "from Traffic Descriptor"
			// ...
			break;

		case 2 :	// "DEI Marking"
			macMT7530MarkDEIYellow(port);
			break;

		case 3 :	// "8P0D"
			macMT7530SetXPYD(port, DropPolicy->DropPrecdenceColourMarking);
			break;

		case 4 :	// "7P1D"
			macMT7530SetXPYD(port, DropPolicy->DropPrecdenceColourMarking);
			break;

		case 5 :	// "6P2D"
			macMT7530SetXPYD(port, DropPolicy->DropPrecdenceColourMarking);
			break;

		case 6 :	// "5P3D"
			macMT7530SetXPYD(port, DropPolicy->DropPrecdenceColourMarking);
			break;

		default :	// reserved/invalid
			return (-1);
	}

	if ((DropPolicy->DropPrecdenceColourMarking) == 2)	// "DEI Marking"
	{
		for (i = 0; i < MT7530_QUEUE_NUM; i ++)
		{
			macMT7530SetDropPercent(port, i, DropPolicy);
		}
	}
	else
	{
		macMT7530SetDropPercent(port, queueId, DropPolicy);
	}


	// backup parameters for macMT7530GetDropPolicy()
	DropPoliySettings[port][queueId].greenPacketDropQueueMaxThr = DropPolicy->greenPacketDropQueueMaxThr;
	DropPoliySettings[port][queueId].greenPacketDropQueueMinThr = DropPolicy->greenPacketDropQueueMinThr;
	DropPoliySettings[port][queueId].yellowPacketDropQueueMaxThr = DropPolicy->yellowPacketDropQueueMaxThr;
	DropPoliySettings[port][queueId].yellowPacketDropQueueMinThr = DropPolicy->yellowPacketDropQueueMinThr;
	DropPoliySettings[port][queueId].greenPacketDropMax_p = DropPolicy->greenPacketDropMax_p;
	DropPoliySettings[port][queueId].yellowPacketDropMaxn_p = DropPolicy->yellowPacketDropMaxn_p;
	DropPoliySettings[port][queueId].QueueDropW_q = DropPolicy->QueueDropW_q;
	DropPoliySettings[port][queueId].DropPrecdenceColourMarking = DropPolicy->DropPrecdenceColourMarking;

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetDropPolicy
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/11/27
**_______________________________________________________________
*/
//int macMT7530GetDropPolicy(u8 port, u8 queueId, u16 *outptr_greenPacketDropQueueMaxThr, u16 *outptr_greenPacketDropQueueMinThr, u16 *outptr_yellowPacketDropQueueMaxThr, u16 *outptr_yellowPacketDropQueueMinThr, u8 *outptr_greenPacketDropMax_p, u8 *outptr_yellowPacketDropMaxn_p, u8 *outptr_QueueDropW_q, u8 *outptr_DropPrecdenceColourMarking)
int macMT7530GetDropPolicy(u8 port, u8 queueId, mt7530_switch_DropPolicy_t *outptr_DropPolicy)
{
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (queueId >= MT7530_QUEUE_NUM) return (-1);

	(outptr_DropPolicy->greenPacketDropQueueMaxThr) = DropPoliySettings[port][queueId].greenPacketDropQueueMaxThr;
	(outptr_DropPolicy->greenPacketDropQueueMinThr) = DropPoliySettings[port][queueId].greenPacketDropQueueMinThr;
	(outptr_DropPolicy->yellowPacketDropQueueMaxThr) = DropPoliySettings[port][queueId].yellowPacketDropQueueMaxThr;
	(outptr_DropPolicy->yellowPacketDropQueueMinThr) = DropPoliySettings[port][queueId].yellowPacketDropQueueMinThr;
	(outptr_DropPolicy->greenPacketDropMax_p) = DropPoliySettings[port][queueId].greenPacketDropMax_p;
	(outptr_DropPolicy->yellowPacketDropMaxn_p) = DropPoliySettings[port][queueId].yellowPacketDropMaxn_p;
	(outptr_DropPolicy->QueueDropW_q) = DropPoliySettings[port][queueId].QueueDropW_q;
	(outptr_DropPolicy->DropPrecdenceColourMarking) = DropPoliySettings[port][queueId].DropPrecdenceColourMarking;

	return (0);
}

/*___________________________________________________________________
struct trafficDescriptor_s{
u32 CIR;
u32 PIR;
u32 CBS;
u32 PBS;
u8 colourMode;
u8 ingressColourMarking;
u8 engressColourMarking;
u8 meterType;
}
**_______________________________________________________________
*/

/*___________________________________________________________________
**      function name: macMT7530SetTrafficDescriptor
**      descriptions:
**         ...
**
**      parameters:


**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/11/26
**_______________________________________________________________
*/
int macMT7530SetTrafficDescriptor(u8 port, mt7530_switch_api_trafficDescriptor_t *trafficDescriptor_p)
{
	u32 reg, value;
	u32 cir, cbs, cir_bit16;
	u8 direction = trafficDescriptor_p->direction;	// 0:TX, 1:RX

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (direction > 1) return (-1);

	reg = ((direction == 0) ? (0x1040) : (0x1800)) + (0x100 * port);	// 0:TX, 1:RX

	cir = (((trafficDescriptor_p->CIR) * 8) + 0x7FFF) >> 15;	// cir register in unit of 32768(=2^15) bps; but function parameter is given as byte/s
	if (cir > 0x1FFFF) cir = 0x1FFFF;	// cir register max 2^17 units;
	cir_bit16 = cir >> 16;			// cir register bit 16 is separated
	cir &= 0xFFFF;

	if (trafficDescriptor_p->CBS == 0) // ONU factory policy
	{
		cbs = 0xF;
	}
	else
	{
	cbs = (trafficDescriptor_p->CBS + 0x1FF) >> 9;		// cbs register in unit of 512(=2^9) bytes
	if (cbs > 0x7F) cbs = 0x7F;		// cbs register max 2^7 units
	}


	value = macMT7530ReadReg(reg) & 0x0000E080;
	value |= (cir << 16) | (cir_bit16 << 12) | (7 << 8) | (1 << 7) | (cbs << 0);	/* modify rate measurement period to 7:1ms, change ticker source from sw_2x_clk to sw_ticker_gen */
	if (trafficDescriptor_p->CIR == 0)  // ONU factory policy
		value &= ~(1<<15);
	else
	value |= (1 << 15);              // rate limit control enable.
	macMT7530WriteReg(reg, value);
//	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);

	// backup parameters for macMT7530GetTrafficDescriptor()
	TrafficDescSettings[port][direction].CIR = trafficDescriptor_p->CIR;
	TrafficDescSettings[port][direction].PIR = trafficDescriptor_p->PIR;
	TrafficDescSettings[port][direction].CBS = trafficDescriptor_p->CBS;
	TrafficDescSettings[port][direction].PBS = trafficDescriptor_p->PBS;
	TrafficDescSettings[port][direction].colourMode = trafficDescriptor_p->colourMode;
	TrafficDescSettings[port][direction].ingressColourMarking = trafficDescriptor_p->ingressColourMarking;
	TrafficDescSettings[port][direction].engressColourMarking = trafficDescriptor_p->engressColourMarking;
	TrafficDescSettings[port][direction].meterType = trafficDescriptor_p->meterType;
	TrafficDescSettings[port][direction].direction = trafficDescriptor_p->direction;

	return (0);
}

int macMT7530GetTrafficDescriptor(u8 port, mt7530_switch_api_trafficDescriptor_t *outptr_trafficDescriptor_p)
{
	u8 direction = outptr_trafficDescriptor_p->direction;	// 0:TX, 1:RX

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (direction > 1) return (-1);

 	outptr_trafficDescriptor_p->CIR = TrafficDescSettings[port][direction].CIR;
 	outptr_trafficDescriptor_p->PIR = TrafficDescSettings[port][direction].PIR;
 	outptr_trafficDescriptor_p->CBS = TrafficDescSettings[port][direction].CBS;
 	outptr_trafficDescriptor_p->PBS = TrafficDescSettings[port][direction].PBS;
 	outptr_trafficDescriptor_p->colourMode = TrafficDescSettings[port][direction].colourMode;
 	outptr_trafficDescriptor_p->ingressColourMarking = TrafficDescSettings[port][direction].ingressColourMarking;
 	outptr_trafficDescriptor_p->engressColourMarking = TrafficDescSettings[port][direction].engressColourMarking;
 	outptr_trafficDescriptor_p->meterType = TrafficDescSettings[port][direction].meterType;
 	outptr_trafficDescriptor_p->direction = TrafficDescSettings[port][direction].direction;

	return (0);
}

int macMT7530CleanTrafficDescriptor(u8 port, u8 direction)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (direction > 1) return (-1);

	reg = ((direction == 0) ? (0x1040) : (0x1800)) + (0x100 * port);	// 0:TX, 1:RX
	value = macMultiMT7530ReadReg(reg, extsw_flag) & ~(1 << 7);	// disable EGC_TBEN/IGN_TBEN
	value &= ~(1 << 15);           // rate limit control diable.
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	return (0);
}


/*___________________________________________________________________
**      function name: macMT7530SetAutoDetection
**      descriptions:
**         ...
**
**      parameters:
**        AutoDetction :
**        0x00 - Auto;      Auto
**        0x01 - 10 only;   Full duplex only
**        0x02 - 100 only;  Full duplex only
**        0x03 - 1000 only; Full duplex only
**        0x04 - Auto;      Full duplex only
**        0x10 - 10 only;   Auto
**        0x11 - 10 only;   Half duplex only
**        0x12 - 100 only;  Half duplex only
**        0x13 - 1000 only; Half duplex only
**        0x14 - Auto;      Half duplex only
**        0x20 - 1000 only; Auto
**        0x30 - 100 only;  Auto
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/17
**_______________________________________________________________
*/
int macMT7530SetAutoDetection(u8 AutoDetction, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	int i;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	if (isFPGA) return (0);
	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);

	for (i = 0; i < MIIMediaOption_Entries; i ++)
	{
		if (AutoDetction == MIIMediaOption[i].index_param)
		{
			break;
		}

	}
	if (i == MIIMediaOption_Entries) return (-1);	// can not found

	if ((MIIMediaOption[i].MII_BMCR_value) & BMCR_ANENABLE)
	{
		phy_reg = MII_ADVERTISE;
		phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
		phy_value &= ~(ADVERTISE_CSMA | ADVERTISE_10HALF | ADVERTISE_10FULL | ADVERTISE_100HALF | ADVERTISE_100FULL);
		phy_value |= MIIMediaOption[i].MII_ADVERTISE_value;
		macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);

		phy_reg = MII_CTRL1000;
		phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
		phy_value &= ~(ADVERTISE_1000FULL | ADVERTISE_1000HALF);
		phy_value |= MIIMediaOption[i].MII_CTRL1000_value;
		macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);
	}

	phy_reg = MII_BMCR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	phy_value &= ~(BMCR_ANENABLE | BMCR_SPEED100 | BMCR_SPEED1000 | BMCR_FULLDPLX);
	phy_value |= BMCR_ANRESTART + MIIMediaOption[i].MII_BMCR_value;
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);

	return (0);
}

int macMT7530GetAutoDetection(u8 *outptr_AutoDetction, u8 port)
{
	u32 phy_addr, phy_bmcr, phy_advertise, phy_ctrl1000;
	int i;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);

	phy_bmcr = macMT7530PHYReadReg(phy_addr, MII_BMCR) & (BMCR_ANENABLE | BMCR_SPEED100 | BMCR_SPEED1000 | BMCR_FULLDPLX);
	phy_advertise = macMT7530PHYReadReg(phy_addr, MII_ADVERTISE) & (ADVERTISE_CSMA | ADVERTISE_10HALF | ADVERTISE_10FULL | ADVERTISE_100HALF | ADVERTISE_100FULL);
	phy_ctrl1000 = macMT7530PHYReadReg(phy_addr, MII_CTRL1000) & (ADVERTISE_1000FULL | ADVERTISE_1000HALF);

	for (i = 0; i < MIIMediaOption_Entries; i ++)
	{
		if (!(phy_bmcr & BMCR_ANENABLE) && (phy_bmcr != MIIMediaOption[i].MII_BMCR_value))
		{
			continue;
		}
		if ((phy_bmcr & BMCR_ANENABLE) && !(MIIMediaOption[i].MII_BMCR_value & BMCR_ANENABLE))
		{
			continue;
		}
		if ((phy_bmcr & BMCR_ANENABLE) && (phy_advertise != MIIMediaOption[i].MII_ADVERTISE_value))
		{
			continue;
		}
#if (1)	// FPGA platform phy chip not support 1000M AN, related register contain garbage value
		if ((phy_bmcr & BMCR_ANENABLE) && (phy_ctrl1000 != MIIMediaOption[i].MII_CTRL1000_value))
		{
			continue;
		}
#endif
		(*outptr_AutoDetction) = MIIMediaOption[i].index_param;
		return (0);
	}
	return (-1);	// can not found
}

/*___________________________________________________________________
**      function name: macMT7530SetLoopbackConf
**      descriptions:
**         PHY Loopback
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/23
**_______________________________________________________________
*/
int macMT7530SetLoopbackConf(u8 enable, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(BMCR_LOOPBACK)) | (enable ? (BMCR_LOOPBACK) : 0);
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);

	return (0);
}

int macMT7530GetLoopbackConf(u8 *outptr_mode, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);
	(*outptr_mode) = (phy_value & BMCR_LOOPBACK) ? 1 : 0;

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetConfigurationStatus
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0x01 : 10BaseT full duplex
**         0x02 : 100BaseT full duplex
**         0x03 : Gigabit Ethernet full duplex
**         0x11 : 10BaseT half duplex
**         0x12 : 100BaseT half duplex
**         0x13 : Gigabit Ethernet half duplex
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/23
**_______________________________________________________________
*/
int macMT7530GetConfigurationStatus(u8 port)
{
	int i;
#if (1)
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x3008 + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);
	value &= 0x000F;

	for (i = 0; i < mt7530MediaStatus_Entries; i ++)
	{
		if (value != mt7530MediaStatus[i][1])
		{
			continue;
		}
		return (mt7530MediaStatus[i][0]);
	}
	return (-1);	// can not found
#else
	u32 phy_addr, phy_reg, phy_value;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port);
	phy_reg = MII_BMSR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
#endif

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetLoopbackConf
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/24
**_______________________________________________________________
*/
int macMT7530SetMaxFrameSize(u16 MaxFrameSize, u8 port)
{
	u32 reg, value, rx_pkt_len;

	switch (MaxFrameSize)
	{
		case 1518 :	// 1518(untag)/1522(tag)
			rx_pkt_len = 0;
			break;

		case 1536 :
			rx_pkt_len = 1;
			break;

		case 1552 :
			rx_pkt_len = 2;
			break;

		default :
			return (-1);	// Lisa said unsupported size will return fail
			break;
	}

	reg = 0x30E0;
	value = macMT7530ReadReg(reg);
	value = (value & ~(0x03)) | rx_pkt_len;
	macMT7530WriteReg(reg, value);
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif
	if (macMT7530_APIDbgDumpEn) printk("%s() (%08X) : %08X\n", __FUNCTION__, reg, value);

	return (0);
}

int macMT7530GetMaxFrameSize(u16 *outptr_MaxFrameSize, u8 port)
{
	u32 reg, value;

	reg = 0x30E0;
	value = macMT7530ReadReg(reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() (%08X) : %08X\n", __FUNCTION__, reg, value);

	switch (value & 0x03)
	{
		case 0 :	// 1518(untag)/1522(tag)
			(*outptr_MaxFrameSize) = 1518;
			break;

		case 1 :
			(*outptr_MaxFrameSize) = 1536;
			break;

		case 2 :
			(*outptr_MaxFrameSize) = 1552;
			break;

		case 3 :
			(*outptr_MaxFrameSize) = 1024 * ((value >> 2) & 0x0F);
			break;
	}

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetLoopbackConf
**      descriptions:
**         ...
**
**      parameters:
**         0 : DCE or MDI-X.
**         1 : DTE or MDI.
**         2 : Automatic selection. (default)
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/24
**_______________________________________________________________
*/
int macMT7530SetDTEDCEInd(u8 ind, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	int i;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	for (i = 0; i < mt7530DTEDCEInd_Entries; i ++)
	{
		if (ind == mt7530DTEDCEInd[i][0])
		{
			break;
		}

	}
	if (i == mt7530DTEDCEInd_Entries) return (-1);	// invalid parameter

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);

	// need enter PHY chip test mode to change this settings
	macMT7530PHYWriteReg(phy_addr, 0x1F, 0x2A30);

	phy_reg = 0x05;
	phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(0x0018)) | (mt7530DTEDCEInd[i][1]);
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);

	// leave PHY chip test mode
	macMT7530PHYWriteReg(phy_addr, 0x1F, 0x0000);

	return (0);
}

int macMT7530GetDTEDCEInd(u8 *outptr_ind, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	int i;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);

	// need enter PHY chip test mode to change this settings
	macMT7530PHYWriteReg(phy_addr, 0x1F, 0x2A30);

	phy_reg = 0x05;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);
	phy_value &= 0x0018;

	// leave PHY chip test mode
	macMT7530PHYWriteReg(phy_addr, 0x1F, 0x0000);

	for (i = 0; i < mt7530DTEDCEInd_Entries; i ++)
	{
		if (phy_value == mt7530DTEDCEInd[i][1])
		{
			break;
		}

	}
	if (i == mt7530DTEDCEInd_Entries) return (-1);	// can not found

	(*outptr_ind) = mt7530DTEDCEInd[i][0];

	return (0);
}
int macMT7530SetPortFlowControl(u8 port, u8 enable)
{
	u32 reg, val, phy_addr, phy_reg;;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x3000 + (port*0x100);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	
	if(enable)
		val |= 3<<4;
	else
		val &= ~(3<<4);

	macMultiMT7530WriteReg(reg, val, extsw_flag);
	
	return 0;
}

int macMT7530GetPortFlowControl(u8 port)
{
	u32 reg, val;
	u8 ret;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x3000 + (port*0x100);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	ret = (u8)((val&(3<<4))?1:0);

	return ret;
}

/*___________________________________________________________________
**      function name: macMT7530SetPauseTime
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/25
**_______________________________________________________________
*/
int macMT7530SetPauseTime(u16 pauseTime, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
    int chn_index = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_ADVERTISE;
	phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(ADVERTISE_PAUSE_CAP)) | (pauseTime ? (ADVERTISE_PAUSE_CAP) : 0);
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);

    if(pausetime_for_test){
        macMT7530SetPortFlowControl(6, 1);
        if(use_ext_switch){
            phy_reg = GSW_BASE + 0x3500;
    	    phy_value = read_reg_word(phy_reg);
            phy_value |= 3<<4;
            write_reg_word(phy_reg, phy_value);
            
            phy_reg = GSW_BASE + 0x3600;
    	    phy_value = read_reg_word(phy_reg);
            phy_value |= 3<<4;
            write_reg_word(phy_reg, phy_value);
        }
        QDMA_API_SET_TX_DROP_EN(ECNT_QDMA_WAN, QDMA_DISABLE);
        for(chn_index = 0; chn_index < 32; chn_index++) {
    		FE_API_SET_HWFWD_CHANNEL(FE_CDM_SEL_CDMA2, chn_index, FE_ENABLE);
    	}
    }    
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);

	return (0);
}

int macMT7530GetPauseTime(u16 *outptr_pauseTime, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_ADVERTISE;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);
	(*outptr_pauseTime) = (phy_value & ADVERTISE_PAUSE_CAP) ? 1 : 0;

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetPortAdmin
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. Andy 2014/01/01
**_______________________________________________________________
*/
int macMT7530SetPortAdmin(u8 admin, u8 port)
{
    u32 phy_addr, phy_reg, phy_value;
    u32 extsw_flag = 0;
    u8 ori_port = port;
    
    if(port & EXT_SWITCH_FLAG)
        extsw_flag = 1;
    port = port & SWITCH_PORT_MASK;

    if (port >= MT7530_TOTAL_PORTS) return (-1);

    phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
    phy_reg = MII_BMCR;

    if(admin){
        /*power down*/  /* stop MAC fist*/
        macMT7530SetMACTxRx(ori_port, 0, MAC_TX);

        /*power down phy*/
        phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(BMCR_PDOWN)) | (BMCR_PDOWN);
        macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);
    }else{
        
        /*power up phy*/
        phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(BMCR_PDOWN));
        macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);
        
        /*power up*/  /* start MAC second*/
        macMT7530SetMACTxRx(ori_port, 1, MAC_TX);
    
    }


    if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);

    return (0);
}


int macMT7530GetPortAdmin(u8 *outptr_admin, u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);
	(*outptr_admin) = (phy_value & BMCR_PDOWN) ? 1 : 0;

	return (0);
}


int macMT7530GetPortChangedCnt(u8 port, u32 *outptr_Cnt)
{
	return (0);	// Lightman_Wang delete request
}
/*___________________________________________________________________
**      function name: macMT7530GetPortMacLimit
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/26
**_______________________________________________________________
*/
int macMT7530GetPortMacLimit(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	
	reg = 0x200c + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);

	if (value & 0x10) // disable source mac addr learning
		return 0; // disable learning 
	
	if (value & 0x20){ // enable rx sa learning counter
		value = (value & 0xFFF00) >> 8;
		if (value == 0)
			return 0; // disable learning 
		else if (value == 0xFFF)
			return 0xFFFF; // no limit
		return value;
	}else
		return 0xFFFF; // no limit
}

int macMT7530GetPortMacLearnNum(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);
	
	reg = 0x200c + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);

	value = (value & 0xFFF00000) >> 20;
	return value;
}



int macMT7530CleanAllMactbl(void){
	
	u32 reg, value;
	reg = REG_ATC_ADDR;
	while (1){	// wait until not busy
		value = macMT7530ReadReg(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
	// clean all entries
	reg = REG_ATC_ADDR;
	value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);	// 3'b010 : clean command (multiple entries)
	macMT7530WriteReg(reg, value);
	
	while (1){	// wait until not busy
	value = macMT7530ReadReg(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = REG_ATC_ADDR;
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        // clean all entries
        reg = REG_ATC_ADDR;
        value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT); // 3'b010 : clean command (multiple entries)
        macMultiMT7530WriteReg(reg, value, 1);
        
        while (1){  // wait until not busy
        value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif
    return 0;
}

int macMT7530CleanMactblByPort(u8 port){
	
	u32 reg, value;
	int ac_mat = 0xc;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	reg = REG_ATC_ADDR;
	while (1){	// wait until not busy
		value = macMultiMT7530ReadReg(reg, extsw_flag);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
	
	macMultiMT7530WriteReg(REG_ATA1_ADDR, 1<<port, extsw_flag);
	
	// clean all entries
	reg = REG_ATC_ADDR;
	value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);	// 3'b010 : clean command (multiple entries)
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	
	while (1){	// wait until not busy
	value = macMultiMT7530ReadReg(reg, extsw_flag);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
    return 0;
}

int macMT7530CleanMactbl(void){
	u32 reg, value;
    u8 port, mac_limit_enable=0;
    for(port = 0; port<7; port++){
    	reg = 0x200C + (0x0100 * port);
	    value = macMT7530ReadReg(reg);
	    if((value & 0xC)== 0x4)
	        mac_limit_enable = 1; /* SA lock enable*/
    }

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        for(port = 0; port<7; port++){
            reg = 0x200C + (0x0100 * port);
            value = macMultiMT7530ReadReg(reg, 1);
            if((value & 0xC)== 0x4)
                mac_limit_enable = 1; /* SA lock enable*/
        }
    }
#endif

    if(mac_limit_enable){
        for(port = 0; port<7; port++){
            macMT7530SetPortActive(port, DISABLE);
	        mdelay(1);
	        macMT7530CleanMactblByPort(port);
	        reg = 0x200C + (0x0100 * port);
	        value = macMT7530ReadReg(reg);
	        value &= ~0x20;
	        macMT7530WriteReg(reg, value);
	        value |= 0x20;
	        macMT7530WriteReg(reg, value);
	        macMT7530SetPortActive(port, ENABLE);   
        }       

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
        if (isEN7526G)
        {
            for(port = 0; port<7; port++){
                macMT7530SetPortActive((port | EXT_SWITCH_FLAG), DISABLE);
                mdelay(1);
                macMT7530CleanMactblByPort((port | EXT_SWITCH_FLAG));
                reg = 0x200C + (0x0100 * port);
                value = macMultiMT7530ReadReg(reg, 1);
                value &= ~0x20;
                macMultiMT7530WriteReg(reg, value, 1);
                value |= 0x20;
                macMultiMT7530WriteReg(reg, value, 1);
                macMT7530SetPortActive((port | EXT_SWITCH_FLAG), ENABLE);   
            }       
        }
#endif
    }else{
        macMT7530CleanAllMactbl();
    }

    return 0;
}

int macMT7530CleanSocMacTbl(void){
    u32 value;
    u32 reg = GSW_ATC_REG;
    
    while (1){  // wait until not busy
        value = read_reg_word(reg);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }

    // clean all entries
    reg = GSW_ATC_REG;
    value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + 
        (2 << REG_ATC_AC_CMD_OFFT);    // 3'b010 : clean command (multiple entries)
    write_reg_word(reg, value);
    while (1){  // wait until not busy
    value = read_reg_word(reg);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }
    
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = GSW_ATC_REG - GSW_BASE;
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        
        // clean all entries
        reg = GSW_ATC_REG - GSW_BASE;
        value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + 
            (2 << REG_ATC_AC_CMD_OFFT);    // 3'b010 : clean command (multiple entries)
        macMultiMT7530WriteReg(reg, value, 1);
        while (1){  // wait until not busy
        value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif
    return 0;
}

int macMT7530CleanExtMacTbl(void){
    u32 value;

    u32 reg = REG_ATC_ADDR;
    while (1){  // wait until not busy
        value = gswPbusRead(reg);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }

    // clean all entries
    reg = REG_ATC_ADDR;
    value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + 
        (2 << REG_ATC_AC_CMD_OFFT);    // 3'b010 : clean command (multiple entries)
    gswPbusWrite(reg, value);
    while (1){  // wait until not busy
    value = gswPbusRead(reg);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }

    return 0;
}

int macMT7530SetPortMacLimit(u8 port, u32 cnt)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x200c + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= 0x3;
	if (cnt == 0) // disable learing
		value = value |(0xFFF<<8);
	else if (cnt >= 0xFFF)// no limit
		value = value |(0xFFF<<8)|0x20;
	else
		value = value | (cnt<<8)|0x24;
	macMT7530SetPortActive(extsw_flag ? (port | EXT_SWITCH_FLAG) : port, DISABLE);
	mdelay(1);
	macMT7530CleanMactblByPort(extsw_flag ? (port | EXT_SWITCH_FLAG) : port);
	macMultiMT7530WriteReg(reg, 0xfff00, extsw_flag);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	macMT7530SetPortActive(extsw_flag ? (port | EXT_SWITCH_FLAG) : port, ENABLE);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);
	return 0;
}
int macMT7530SetPortSpeedMode(u8 port, u8 mode)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x3000 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);

	value &= 0xFFFFFF01; // clear speed & duplex
	switch (mode){
		case 0x01: // 10M half
		value = value | (0x01<<15) ;
		break;
		case 0x02: // 10M full
		value = value | (0x01<<15) | 0x02;
		break;
		case 0x03: //100M half
		value = value | (0x01<<15) | 0x04;
		break;
		case 0x04: // 100M full
		value = value | (0x01<<15) | 0x06;
		break;
		case 0x05: // 1g
		value = value | (0x01<<15) | 0x0A; // 1000M full
		break;
		default:
			return 0;
	}
	
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);
	return 0;
}

/*___________________________________________________________________
**      function name: macMT7530GetPause
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/26
**_______________________________________________________________
*/
int macMT7530GetPortPause(u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_ADVERTISE;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);

	return (phy_value & ADVERTISE_PAUSE_CAP) ? 1 : 0;
}

int macMT7530SetPortPause(u8 port, u8 enable)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_ADVERTISE;
	phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(ADVERTISE_PAUSE_CAP|0x0800)) | (enable ? (ADVERTISE_PAUSE_CAP) : 0);
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);

	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetPortPolicingEnable
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530GetPortPolicingEnable(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x1800 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);

	return ((value & (1 << 15)) ? 1 : 0);
}

/*___________________________________________________________________
**      function name: macMT7530SetPortPolicingEnable
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530SetPortPolicingEnable(u8 port, u8 enable)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x1800 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value = (enable) ? (value | (1 << 15)) : (value & ~(1 << 15));
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetPortPolicing
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530GetPortPolicing(u8 port, u32 *outptr_cir, u32 *outptr_cbs, u32 *outptr_ebs)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x1800 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);

	if ((value & (1 << 7)) == 0) return (-1);		// TB_EN=0 : not bucket mode

	(*outptr_cir) = ((value >> 16) & 0xFFFF);
	if (value & (1 << 12)) (*outptr_cir) |= (1 << 16);	// cir register bit 16 is separated
	(*outptr_cir) <<= 5;					// cir register in unit of 32768(=2^15) bps; but function parameter is given as kbps

	(*outptr_cbs) = (value & 0x7F) << 9;			// cbs register in unit of 512(=2^9) bytes

	(*outptr_ebs) = 0;					// ebs fixed 0
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetPortPolicing
**      descriptions:
**         ...
**
**      parameters:
**         cir : in kbps
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530SetPortPolicing(u8 port, u32 cir, u32 cbs, u32 ebs)
{
	u32 reg, value;
	u32 cir_bit16;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	cir = (cir + 0x1F) >> 5;		// cir register in unit of 32768(=2^15) bps; but function parameter is given as kbps
	if (cir > 0x1FFFF) cir = 0x1FFFF;	// cir register max 2^17 units;
	cir_bit16 = cir >> 16;			// cir register bit 16 is separated
	cir &= 0xFFFF;

	cbs = (cbs + 0x1FF) >> 9;		// cbs register in unit of 512(=2^9) bytes
	if (cbs > 0x7F) cbs = 0x7F;		// cbs register max 2^7 units

	reg = 0x1800 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag) & 0x0000E080;
	value |= (cir << 16) | (cir_bit16 << 12) | (7 << 8) | (1 << 7) | (cbs << 0);	/* modify rate measurement period to 7:1ms, change ticker source from sw_2x_clk to sw_ticker_gen */
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetPortRatelimitingEnable
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530GetPortRatelimitingEnable(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x1040 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);
	
	return ((value & (1 << 15)) ? 1 : 0);
}

/*___________________________________________________________________
**      function name: macMT7530SetPortRatelimitingEnable
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530SetPortRatelimitingEnable(u8 port, u8 enable)
{
	u32 reg, value;

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
	
	if (port >= MT7530_TOTAL_PORTS) return (-1);
	reg = 0x1040 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value = (enable) ? (value | (1 << 15)) : (value & ~(1 << 15));
	macMultiMT7530WriteReg(reg, value, extsw_flag);
#else

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x1040 + (0x0100 * port);
	value = macMT7530ReadReg(reg);
	value = (enable) ? (value | (1 << 15)) : (value & ~(1 << 15));
	macMT7530WriteReg(reg, value);
#endif
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);
	
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetPortRatelimiting
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530GetPortRatelimiting(u8 port, u32 *outptr_cir, u32 *outptr_pir)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x1040 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);

	if ((value & (1 << 7)) == 0) return (-1);		// TB_EN=0 : not bucket mode

	(*outptr_cir) = ((value >> 16) & 0xFFFF);
	if (value & (1 << 12)) (*outptr_cir) |= (1 << 16);	// cir register bit 16 is separated
	(*outptr_cir) <<= 5;					// cir register in unit of 32768(=2^15) bps; but function parameter is given as kbps

	(*outptr_pir) = (*outptr_cir);				// pir == cir


	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetPortRatelimiting
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/10/30
**_______________________________________________________________
*/
int macMT7530SetPortRatelimiting(u8 port, u32 cir, u32 pir)
{
	u32 reg, value;
	u32 cir_bit16;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	cir = (cir + 0x1F) >> 5;		// cir register in unit of 32768(=2^15) bps; but function parameter is given as kbps
	if (cir >= 0x20000) cir = 0x1FFFF;	// cir register max 2^17 units;
	cir_bit16 = cir >> 16;			// cir register bit 16 is separated
	cir &= 0xFFFF;

	reg = 0x1040 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag) & 0x0000E080;
	value |= (cir << 16) | (cir_bit16 << 12) | (7 << 8) | (1 << 7) | (0x7F << 0);	/* modify rate measurement period to 7:1ms, change ticker source from sw_2x_clk to sw_ticker_gen */
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);
	
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetPortLoopDetect
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/25
**_______________________________________________________________
*/
int macMT7530GetPortLoopDetect(u8 port)
{
#if (0)	// Lightman_Wang delete request
	u32 phy_addr, phy_reg, phy_value;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port);
	phy_reg = MII_BMCR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);
	return ((phy_value & BMCR_LOOPBACK) ? 1 : 0);
#else
	return (-1);
#endif
}

int macMT7530SetPortLoopDetect(u8 port, u8 enable)
{
#if (0)	// Lightman_Wang delete request
	u32 phy_addr, phy_reg, phy_value;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port);
	phy_reg = MII_BMCR;
	phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(BMCR_LOOPBACK)) | (enable ? (BMCR_LOOPBACK) : 0);
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, phy_addr, phy_reg, phy_value);

	return (0);
#else
	return (-1);
#endif
}

int macMT7530SetPortDisableLooped(u8 port, u8 enable)
{
	return (-1);	// Lightman_Wang delete request
}

/*___________________________________________________________________
**      function name: macMT7530GetPortActive
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/25
**_______________________________________________________________
*/
int macMT7530GetPortActive(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x3000 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);

	return ((value & (0x03 << 13)) ? 1 : 0);
}

int macMT7530SetPortActive(u8 port, u8 enable)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x3000 + (0x0100 * port);
	value = (macMultiMT7530ReadReg(reg, extsw_flag) & ~(0x03 << 13)) | (enable ? (0x03 << 13) : 0);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);

	return (0);
}

int macMT7530GetPortAutoNegState(u8 port){
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	return ((phy_value & BMCR_ANENABLE) ? 1 : 0);
}
/*___________________________________________________________________
**      function name: macMT7530PortAutoNegActive
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/25
**_______________________________________________________________
*/
int macMT7530PortAutoNegActive(u8 port, u8 enable)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	phy_value = (enable) ? (phy_value | BMCR_ANENABLE) : (phy_value & ~(BMCR_ANENABLE));
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);

	return (0);
}
EXPORT_SYMBOL(macMT7530PortAutoNegActive);
/*___________________________________________________________________
**      function name: macMT7530PortAutoNegRestartAutoConfig
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/25
**_______________________________________________________________
*/
int macMT7530PortAutoNegRestartAutoConfig(u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);
	phy_value |= (BMCR_ANENABLE | BMCR_ANRESTART);
	macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetEthPortAutoNegFailure
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/
**_______________________________________________________________
*/
int macMT7530GetEthPortAutoNegFailure(u8 port)
{
	u32 phy_addr, phy_reg;
	u16 an_enable, link_up, lp_ability;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	an_enable = macMT7530PHYReadReg(phy_addr, phy_reg) & BMCR_ANENABLE;
	phy_reg = MII_BMSR;
	link_up = macMT7530PHYReadReg(phy_addr, phy_reg) & BMSR_LSTATUS;
	phy_reg = MII_LPA;
	lp_ability = macMT7530PHYReadReg(phy_addr, phy_reg);

	if (an_enable )
	{
		if(!link_up && lp_ability)
			return 1;
		else
			return 0;
	}
	else
	{
		return (0);
	}
}

/*___________________________________________________________________
**      function name: macMT7530GetEthPortLos
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:Signal Not Loss
**         1:Signal Loss
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/
**_______________________________________________________________
*/
int macMT7530GetEthPortLos(u8 port)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = 0x3008 + (0x0100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) printk("%s() %2d (%08X) : %08X\n", __FUNCTION__, port, reg, value);

	return ((value & 0x01) ? 0 : 1);
}

/*___________________________________________________________________
**      function name: macMT7530GetEthPortFailure
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/09
**_______________________________________________________________
*/
int macMT7530GetEthPortFailure(u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMSR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);

	// In current MT/RT GSW PHY chip, BMSR reg msb 7 bits = 0111,100x,xxxx,xxxx
	return (((phy_value & 0xFE00) == 0x7800) ? 0 : 1);
}

/*___________________________________________________________________
**      function name: macMT7530GetEthPortCongestion
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/
**_______________________________________________________________
*/
int macMT7530GetEthPortCongestion(u8 port)
{
	static u32 PrevPauseCnt[MT7530_TOTAL_PORTS*2][2];//{MIB_ID_TX_PAUSE_CNT, MIB_ID_RX_PAUSE_CNT}
	u32 reg, value, tx_pause_cnt, rx_pause_cnt;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	//reg = 0x4000 + (0x100 * port) + (MIB_ID_TX_PAUSE_CNT * 4);
	reg = MIB_ID_TX_PAUSE_CNT + (0x100 * port);
	tx_pause_cnt = macMultiMT7530ReadReg(reg, extsw_flag);

	//reg = 0x4000 + (0x100 * port) + (MIB_ID_RX_PAUSE_CNT * 4);
	reg = MIB_ID_RX_PAUSE_CNT + (0x100 * port);
	rx_pause_cnt = macMultiMT7530ReadReg(reg, extsw_flag);

	value =  ((tx_pause_cnt != PrevPauseCnt[port+MT7530_TOTAL_PORTS*extsw_flag][0]) || (rx_pause_cnt != PrevPauseCnt[port+MT7530_TOTAL_PORTS*extsw_flag][1])) ? 1 : 0;
	PrevPauseCnt[port+MT7530_TOTAL_PORTS*extsw_flag][0] = tx_pause_cnt;
	PrevPauseCnt[port+MT7530_TOTAL_PORTS*extsw_flag][1] = rx_pause_cnt;

	return (value);
}

/*___________________________________________________________________
**      function name: macMT7530GetTxTimestamp
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/01
**_______________________________________________________________
*/
int macMT7530GetTxTimestamp(mt7530_switch_api_timestamp_fifo *outptr_tx_ts)
{
	u32 reg, value;

	reg = 0x1F90;
	value = macMT7530ReadReg(reg);
	if (value & (1 << 6)) return (-1);	//TMSTMP_TFIFO_EMPTY 0x1F90[6]

	reg = 0x1F98;
	value = macMT7530ReadReg(reg);
	outptr_tx_ts->seq_id = (value >> 16) & 0x07;	//TMSTMP_TFIFO_SEQ 0x1F98[18:16]
	outptr_tx_ts->msg_type = value & 0x03;		//TMSTMP_TFIFO_SEQ 0x1F98[1:0]

	reg = 0x1F9C;
	value = macMT7530ReadReg(reg);
	(outptr_tx_ts->tstamp).seconds = (value >> 28) & 0x0F;		//TMSTMP_TFIFO_S_LSB 0x1F9C[31:28]
	(outptr_tx_ts->tstamp).nanoseconds = (value & 0x0FFFFFFF) << 2;	//TMSTMP_TFIFO_NS 0x1F9C[27:0]

	reg = 0x1F90;
	value = 0x0001;
	macMT7530WriteReg(reg, value);		//TMSTMP_TFIFO_LOAD 0x1F90[0]

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetRxTimestamp
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/01
**_______________________________________________________________
*/
int macMT7530GetRxTimestamp(mt7530_switch_api_timestamp_fifo *outptr_rx_ts)
{
	u32 reg, value;

	reg = 0x1FA0;
	value = macMT7530ReadReg(reg);
	if (value & (1 << 6)) return (-1);	//TMSTMP_RFIFO_EMPTY 0x1FA0[6]

	reg = 0x1FA8;
	value = macMT7530ReadReg(reg);
	outptr_rx_ts->seq_id = (value >> 16) & 0x07;	//TMSTMP_RFIFO_SEQ 0x1FA8[18:16]
	outptr_rx_ts->msg_type = value & 0x03;		//TMSTMP_RFIFO_SEQ 0x1FA8[1:0]

	reg = 0x1FAC;
	value = macMT7530ReadReg(reg);
	(outptr_rx_ts->tstamp).seconds = (value >> 28) & 0x0F;		//TMSTMP_RFIFO_S_LSB 0x1FAC[31:28]
	(outptr_rx_ts->tstamp).nanoseconds = (value & 0x0FFFFFFF) << 2;	//TMSTMP_RFIFO_NS 0x1FAC[27:0]

	reg = 0x1FA0;
	value = 0x0001;
	macMT7530WriteReg(reg, value);		//TMSTMP_RFIFO_LOAD 0x1FA0[0]

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetCurrTime
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/01
**_______________________________________________________________
*/
int macMT7530GetCurrTime(mt7530_switch_api_timestamp *outptr_ts)
{
	u32 reg, value;

	reg = 0x1F60;
	value = macMT7530ReadReg(reg);		// read PTP_CTRL to latch TIMESTAMP_LSB
	if ((value & (1 << 31)) == 0) return (-1);	//TIMESTAMP_EN 0x1F60[31]

	reg = 0x1F68;
	value = macMT7530ReadReg(reg);
	outptr_ts->seconds = (value >> 28) & 0x0F;		//TMSTMP_S_LSB 0x1F68[31:28]
	outptr_ts->nanoseconds = (value & 0x0FFFFFFF) << 2;	//TMSTMP_NS 0x1F68[27:0]

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetCurrTime
**      descriptions:
**         ...
**
**      parameters:
**        type: 0(immediate); 1(next 1pps)
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/01
**_______________________________________________________________
*/
int macMT7530SetCurrTime(int type, mt7530_switch_api_timestamp *ts)
{
	u32 reg, value, ts4ns;

	ts4ns = ts->nanoseconds >> 2;
	if ((ts->seconds > 0x0F) || (ts4ns > 0x0FFFFFFF)) return (-1);

	reg = 0x1F70;
//20120927 YM_Chan : spec update
//spec update	value = ((type) ? (1 << 2) : 0) | (0 << 0);		//TMSTMP_OFFSET_MODE 0x1F70[0] = 0 : Absolute mode; TMSTMP_ABS_MODE 0x1F70[2] = (0:next clock; 1:next 1pps)
	value = ((type) ? (1 << 2) : 0) | (1 << 0);		//TMSTMP_OFFSET_MODE 0x1F70[0] = 1 : Absolute mode; TMSTMP_ABS_MODE 0x1F70[2] = (0:next clock; 1:next 1pps)
	macMT7530WriteReg(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif

	reg = 0x1F78;
	value = (ts->seconds << 28) | ts4ns;			//TMSTMP_OFFSET_LSB 0x1F78[31:28], TMSTMP_NS_OFFSET 0x1F78[27:0]
	macMT7530WriteReg(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetOffset
**      descriptions:
**         ...
**
**      parameters:
**         1:add, 0:sub
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/01
**_______________________________________________________________
*/
int macMT7530SetOffset(int opt, mt7530_switch_api_timestamp *ts)
{
	u32 reg, value, ts4ns;

	reg = 0x1F70;
//20120927 YM_Chan : spec update
//spec update	value = (1 << 0);		//TMSTMP_OFFSET_MODE 0x1F70[0] = 1 : Add/Sub mode
	value = (0 << 0);		//TMSTMP_OFFSET_MODE 0x1F70[0] = 0 : Add/Sub mode
	macMT7530WriteReg(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif

	reg = 0x1F78;
	if (opt)	//opt=1:add
	{
		ts4ns = ts->nanoseconds >> 2;
		if (ts4ns > 0x0FFFFFFF) return (-1);
		value = 0 | ts4ns;			//TMSTMP_OFFSET_LSB 0x1F78[31:28] = 0(Add mode), 0xF(Sub mode)
							//TMSTMP_NS_OFFSET 0x1F78[27:0]
	}
	else	//opt=1:sub
	{
		ts4ns = (1000000000L - ts->nanoseconds) >> 2;
		if (ts4ns > 0x0FFFFFFF) return (-1);
		value = (0xF << 28) | ts4ns;		//TMSTMP_OFFSET_LSB 0x1F78[31:28] = 0(Add mode), 0xF(Sub mode)
							//TMSTMP_NS_OFFSET 0x1F78[27:0]
	}
	macMT7530WriteReg(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530GetPTPState
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/03
**_______________________________________________________________
*/
int macMT7530GetPTPState(int *outptr_EnablePort)
{
	u32 reg, value;
	u8 i;

	reg = 0x1F60;
	value = macMT7530ReadReg(reg);
	if (((value & (1 << 31)) == 0) || ((value & 0x7F) == 0))	//TIMESTAMP_EN 0x1F60[31]; PTPn_EN 0x1F60[6:0]
	{
		(*outptr_EnablePort) = -1;
	}
	else
	{
		value &= 0x7F;
		for (i = 0; i < MT7530_TOTAL_PORTS; i ++)
		{
			if (value & 1)
			{
				(*outptr_EnablePort) = i;
				break;
			}
			value >>= 1;
		}
	}

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetPTPState
**      descriptions:
**         ...
**
**      parameters:
**         -1 : disable all port 1588 feature
**         0~6 : select port to enable 1588 feature
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/08/03
**_______________________________________________________________
*/
int macMT7530SetPTPState(int EnablePort)
{
	u32 reg, value;

	if ((EnablePort >= MT7530_TOTAL_PORTS) || (EnablePort < -1)) return (-1);

	reg = 0x1F60;
	value = macMT7530ReadReg(reg);
	value &= ~((1 << 31) | 0x7F);
	if (EnablePort != -1)
	{
		value |= (1 << 31) | (1 << EnablePort);		//TIMESTAMP_EN 0x1F60[31]; PTPn_EN 0x1F60[6:0]
	}
	macMT7530WriteReg(reg, value);

	return (0);
}
static mt7530_switch_api_trtcm_acl_t TrtcmAclSetting;
/*___________________________________________________________________
**      function name: macMT7530SetDsTrtcmEnable
**      descriptions:
**         ...
**
**      parameters:
**         enable : 
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. Andy Yi 2014/03/24
**_______________________________________________________________
*/
int macMT7530SetDsTrtcmEnable(u8 enable)
{
	u32 reg, value = 0;
	int rule, i = 0;

	if ((!isMT7525G) && (!isMT7520G))
		return 0;

	memset(&TrtcmAclSetting, 0xff, sizeof(TrtcmAclSetting));
	TrtcmAclSetting.enable = enable ? 1 : 0;

	if (enable)
	{		
		/* internal switch trtcm enable */
		reg = 0x9c ;
		value = read_reg_word(GSW_BASE+reg);
		value = (value | (1 << 31));
		write_reg_word(GSW_BASE+reg, value);			

		/* internal switch port5 Q0-Q7 control 10(yellow)-11(red)*/
		for (i = 0; i < 8; i++)
		{
			reg = 0x1d0c + i * 4;
			value = 0x20000;
			write_reg_word(GSW_BASE+reg, value);						
		}
		
		reg = 0x1d0c ;
		value = read_reg_word(GSW_BASE+reg);
		value = (value | (1 << 31));
		write_reg_word(GSW_BASE+reg, value);

		for (i = 0; i < 8; i++)
		{
			reg = 0x1d2c + i * 4;
			value = 0;
			write_reg_word(GSW_BASE+reg, value);						
		}
		
		reg = 0x1d2c ;
		value = read_reg_word(GSW_BASE+reg);
		value = (value | (1 << 31));
		write_reg_word(GSW_BASE+reg, value);	

		/* enable internal switch port 6 ACL, and set 32 acl rule */
		reg = 0x2604 ;
		value = read_reg_word(GSW_BASE+reg);
		value = (value | (1 << 10));
		write_reg_word(GSW_BASE+reg, value);

		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			value = 0x7c000000 + (rule << 9);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x8400c;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x80005000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}

		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			value = (1 << rule);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x0;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x80009000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}

		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			if (rule < 8)
				value = 0x18000000 + (rule << 24) + (rule << 4);
			else
				value = 0x10000000 ;
				
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x00010000 + (rule << 18);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x8000b000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}		

		/* trtcm value set to default 0xFFFF */
		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			value = 0xffffffff;
			write_reg_word(GSW_BASE+reg, value);
			
			reg = 0x98;
			value = 0xffffffff;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x80007000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}
	}
	else
	{
		/* internal switch trtcm disable */
		reg = 0x9c ;
		value = read_reg_word(GSW_BASE+reg);
		value &= (~(1 << 31));
		write_reg_word(GSW_BASE+reg, value);	
		
		/*restore internal switch port 5 Q0-Q7 control 10-11 */
		for (i = 0; i < 8; i++)
		{
			reg = 0x1d0c + i * 4;
			value = 0;
			write_reg_word(GSW_BASE+reg, value);						
		}

		for (i = 0; i < 8; i++)
		{
			reg = 0x1d2c + i * 4;
			value = 0;
			write_reg_word(GSW_BASE+reg, value);						
		}
	
		/* undo internal switch ACL rule */
		reg = 0x2604 ;
		value = read_reg_word(GSW_BASE+reg);
		value &= (~(1 << 10));
		write_reg_word(GSW_BASE+reg, value);
	}

	return 0;
}
/*___________________________________________________________________
**      function name: macMT7530GetDsTrtcm
**      descriptions:
**         ...
**
**      parameters:
**         rate : rate <kbps>
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. Andy Yi 2014/03/11
**_______________________________________________________________
*/
int macMT7530GetDsTrtcm(mt7530_switch_api_trtcm_acl_t *trtcm_acl_p)
{
	u32 reg, value = 0;
	int i = 0;

	if ((!isMT7525G) && (!isMT7520G))
		return 0;

	if (trtcm_acl_p == NULL)
		return -1;
	
	trtcm_acl_p->enable = TrtcmAclSetting.enable;
	for (i = 0; i < TRTCM_RULE_NUM; i++)
	{	
		trtcm_acl_p->trtcm_acl_table[i].trtcmId = TrtcmAclSetting.trtcm_acl_table[i].trtcmId;
		trtcm_acl_p->trtcm_acl_table[i].CBS = TrtcmAclSetting.trtcm_acl_table[i].CBS;
		trtcm_acl_p->trtcm_acl_table[i].PBS = TrtcmAclSetting.trtcm_acl_table[i].PBS;
		trtcm_acl_p->trtcm_acl_table[i].CIR = TrtcmAclSetting.trtcm_acl_table[i].CIR;
		trtcm_acl_p->trtcm_acl_table[i].PIR = TrtcmAclSetting.trtcm_acl_table[i].PIR;
	}
	
	return 0;
}

/*___________________________________________________________________
**      function name: macMT7530SetDsTrtcm
**      descriptions:
**         ...
**
**      parameters:
**         rate : rate <kbps>
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. Andy Yi 2014/03/11
**_______________________________________________________________
*/
int macMT7530SetDsTrtcm(mt7530_switch_api_trtcm_t *trtcm_p)
{
	u32 reg, value = 0;

	if ((!isMT7525G) && (!isMT7520G))
		return 0;

	if (trtcm_p == NULL)
		return -1;

	if (trtcm_p->trtcmId >= TRTCM_RULE_NUM)
		return -1;
	
	TrtcmAclSetting.trtcm_acl_table[trtcm_p->trtcmId].trtcmId = trtcm_p->trtcmId;
	TrtcmAclSetting.trtcm_acl_table[trtcm_p->trtcmId].CBS = trtcm_p->CBS;
	TrtcmAclSetting.trtcm_acl_table[trtcm_p->trtcmId].PBS = trtcm_p->PBS;
	TrtcmAclSetting.trtcm_acl_table[trtcm_p->trtcmId].CIR = trtcm_p->CIR;
	TrtcmAclSetting.trtcm_acl_table[trtcm_p->trtcmId].PIR = trtcm_p->PIR;
	
	reg = 0x94;
	value = ((trtcm_p->CBS << 16) | (trtcm_p->PBS));
	write_reg_word(GSW_BASE+reg, value);
		
	reg = 0x98;
	value = ((trtcm_p->CIR << 16) | (trtcm_p->PIR));
	write_reg_word(GSW_BASE+reg, value);
	
	reg = 0x90;
	value = 0x80007000 + trtcm_p->trtcmId;
	write_reg_word(GSW_BASE+reg, value);

	return 0;
}


/*___________________________________________________________________
**      function name: macMT7530SetDsP5RateLimitEnable
**      descriptions:
**         ...
**
**      parameters:
**         enable : 
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. Andy Yi 2014/03/11
**_______________________________________________________________
*/
int macMT7530SetDsP5RateLimitEnable(u8 enable)
{
	u32 reg, value = 0;

	if ((!isMT7525G) && (!isMT7520G))
		return 0;
	
	reg = 0x1540 ;
	value = read_reg_word(GSW_BASE+reg);
	value = (enable) ? (value | (1 << 15)) : (value & ~(1 << 15));
	write_reg_word(GSW_BASE+reg, value);	

	return 0;
}

/*___________________________________________________________________
**      function name: macMT7530SetDsP5RateLimit
**      descriptions:
**         ...
**
**      parameters:
**         rate : rate <kbps>
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. Andy Yi 2014/03/11
**_______________________________________________________________
*/
int macMT7530SetDsP5RateLimit(u32 rate)
{
	u32 reg, value = 0;

	if ((!isMT7525G) && (!isMT7520G))
		return 0;
	
	reg = 0x1540 ;
	value = ((rate/32)<<16) + (1<<15) + (0<<8) + (1<<7) + 0xf;
	write_reg_word(GSW_BASE+reg, value);	

	return 0;
}


/*___________________________________________________________________
**      function name: macMT7530SetDsPort5QosEnable
**      descriptions:
**         ...
**
**      parameters:
**         0 : disable internal switch port5 qos feature
**         1 : enable internal switch port5 qos feature
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. Andy Yi 2014/03/11
**_______________________________________________________________
*/
int macMT7530SetDsP5Qos(u8 enable)
{
	u32 reg, value;
	int queue = 0, rule = 0;

	if ((!isMT7525G) && (!isMT7520G))
		return 0;

	if (enable)
	{
		fcStatus=0x80;
		/* close flow control*/
		reg = 0x1fe0 ;
		value = read_reg_word(GSW_BASE+reg);
		fcStatus |= (value & (1<<31)) ? (1<<0) : 0;
		value &= ~(1<<31);
		write_reg_word(GSW_BASE+reg, value);

		reg = 0x3600 ;
		value = read_reg_word(GSW_BASE+reg);
		fcStatus |= (value & (3<<4)) ? (1<<1) : 0;
		value &= ~(3<<4);
		write_reg_word(GSW_BASE+reg, value);	

		reg = 0x3500 ;
		value = read_reg_word(GSW_BASE+reg);
		fcStatus |= (value & (3<<4)) ? (1<<2) : 0;
		value &= ~(3<<4);
		write_reg_word(GSW_BASE+reg, value);

		reg = 0x1fe0 ;
		value = macMT7530ReadReg(reg);
		fcStatus |= (value & (1<<31)) ? (1<<3) : 0;
		value &= ~(1<<31);
		macMT7530WriteReg(reg, value);

		reg = 0x3600 ;
		value = macMT7530ReadReg(reg);
		fcStatus |= (value & (3<<4)) ? (1<<4) : 0;
		value &= ~(3<<4);
		macMT7530WriteReg(reg, value);		

		/* internal switch priority based acl */	
		reg = 0x44;
		value = read_reg_word(GSW_BASE+reg);
		value &= (~((0x7<<0) | (0x7<<4) | (0x7<<8) | (0x7<<12) | (0x7<<16)));
		value |= ((0x7<<0) | (0x2<<4) | (0x2<<8) | (0x2<<12) | (0x2<<16));
		write_reg_word(GSW_BASE+reg, value);

		/* internal switch port 5 scheduler method is SP */
		for (queue=0; queue<8; queue++)
		{
			reg = 0x1500 + queue*8;
			value = read_reg_word(GSW_BASE+reg);
			value &= (0x0);
			value |= (1<<31);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x1504 + queue*8;
			value = read_reg_word(GSW_BASE+reg);
			value &= (0x0);
			write_reg_word(GSW_BASE+reg, value);			
		}

		/* enable internal switch port 6 ACL, and set eight acl rule */
		reg = 0x2604;
		value = read_reg_word(GSW_BASE+reg);
		value |= (1<<10);
		write_reg_word(GSW_BASE+reg, value);

		for (rule = 0; rule < 8; rule++)
		{
			reg = 0x94;
			value = 0xe0000000 + (rule <<13);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x94000;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x80005000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}

		for (rule = 0; rule < 8; rule++)
		{
			reg = 0x94;
			value = (1 << rule);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x0;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x80009000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}

		for (rule = 0; rule < 8; rule++)
		{
			reg = 0x94;
			value = 0x18000000 + (rule << 24) + (rule << 4);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x0;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x8000b000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}		
	}
	else
	{
		/* restore flow control value */
		if (fcStatus & 0x80)
		{
			fcStatus &= ~(0x80);
			reg = 0x1fe0 ;
			value = read_reg_word(GSW_BASE+reg);
			if (fcStatus & (1<<0))
				value |= (1<<31);
			else
				value &= ~(1<<31);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x3600 ;
			value = read_reg_word(GSW_BASE+reg);
			if (fcStatus & (1<<1))
				value |= (3<<4);
			else
				value &= ~(3<<4);
			write_reg_word(GSW_BASE+reg, value);	

			reg = 0x3500 ;
			value = read_reg_word(GSW_BASE+reg);
			if (fcStatus & (1<<2))
				value |= (3<<4);
			else
				value &= ~(3<<4);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x1fe0 ;
			value = macMT7530ReadReg(reg);
			if (fcStatus & (1<<3))
				value |= (1<<31);
			else
				value &= ~(1<<31);
			macMT7530WriteReg(reg, value);

			reg = 0x3600 ;
			value = macMT7530ReadReg(reg);
			if (fcStatus & (1<<4))
				value |= (3<<4);
			else
				value &= ~(3<<4);
			macMT7530WriteReg(reg, value);		
		}
		
		/* restore internal switch port 5 scheduler method*/
		for (queue=0; queue<8; queue++)
		{
			reg = 0x1500 + queue*8;
			value = read_reg_word(GSW_BASE+reg);
			value &= (0x0);
			write_reg_word(GSW_BASE+reg, value);
		
			reg = 0x1504 + queue*8;
			value = read_reg_word(GSW_BASE+reg);
			value &= (0x0);
			write_reg_word(GSW_BASE+reg, value); 		
		}

		/* disable internal switch port 6 ACL */
		reg = 0x2604;
		value = read_reg_word(GSW_BASE+reg);
		value &= ~(1<<10);
		write_reg_word(GSW_BASE+reg, value);
	}

	return (0);
}


int switch_port_mask=0x7f;
#define MAX_SWITCH_PORT_NUM 7

/*___________________________________________________________________
**      function name: macMT7530SetStormCtrlPort
**      descriptions:
**         ...
**
**      parameters:
**         0x00~0x7f: bit0~bit6 means port0~port6,default value is 0x7f
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. kenny 2014/03/11
**_______________________________________________________________
*/

int macMT7530SetStormCtrlPort(u16 port)
{
	switch_port_mask = port;

	printk("set switch port_mask: 0x%x\r\n", switch_port_mask);
	return (0);
}




/*___________________________________________________________________
**      function name: macMT7530SetStormCtrlUnit
**      descriptions:
**         ...
**
**      parameters:
**         0 : 64 packet per second
**         1 : 256 packets per second
**         2 : 1 k packet per second
**         3 : 4 k packets per second
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. kenny 2014/03/11
**_______________________________________________________________
*/

int macMT7530SetStormCtrlUnit(u8 unit)
{
	u32 reg, value;
	int i=0;
	
	if(unit > 3)
		return -1;
	
	for(i=0; i<MAX_SWITCH_PORT_NUM; i++)
	{
		if(switch_port_mask & (1<<i))
		{
			reg = 0x201c + (i*0x100);
			value = macMultiMT7530ReadReg(reg, switch_port_mask & EXT_SWITCH_FLAG);
			
			/*printk("1 set storm contrl unit: 0x%lx\r\n", value);*/
			value &= ~(3 << 24);
			
			/*printk("2 set storm contrl unit: 0x%lx\r\n", value);*/
			value |= (unit << 24);

			/*printk("3 set storm contrl unit: 0x%lx\r\n", value);*/
			macMultiMT7530WriteReg(reg, value, switch_port_mask & EXT_SWITCH_FLAG);
		}

	}
	printk("set storm contrl unit: 0x%x\r\n", unit);
	return (0);
}


/*___________________________________________________________________
**      function name: macMT7530SetStormCtrlType
**      descriptions:
**         ...
**
**      parameters:
**         4 : broadcast
**         2 : multicast
**         1 : unknow mac unicast
**       
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. kenny 2014/03/11
**_______________________________________________________________
*/

int macMT7530SetStormCtrlType(u8 type)
{
	u32 reg, value;
	int i=0;

	if(type > 7)
		return -1;
	
	for(i=0; i<MAX_SWITCH_PORT_NUM; i++)
	{
		if(switch_port_mask & (1<<i))
		{
			reg = 0x201c + (i*0x100);
			value = macMultiMT7530ReadReg(reg, switch_port_mask & EXT_SWITCH_FLAG);

			value &= ~(7 << 28);
			value |= (type << 28);

			macMultiMT7530WriteReg(reg, value, switch_port_mask & EXT_SWITCH_FLAG);
		}

	}
	printk("set storm contrl type: 0x%x\r\n", type);

	return (0);
}


/*___________________________________________________________________
**      function name: macMT7530SetStormCtrlMode
**      descriptions:
**         ...
**
**      parameters:
**         0 : Packet-base
**         1 : rate-base
**       
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. kenny 2014/03/11
**_______________________________________________________________
*/

int macMT7530SetStormCtrlMode(u8 mode)
{
	u32 reg, value;
	int i=0;

	if(mode > 1)
		return -1;
	
	for(i=0; i<MAX_SWITCH_PORT_NUM; i++)
	{
		if(switch_port_mask & (1<<i))
		{
			reg = 0x201c + (i*0x100);
			value = macMultiMT7530ReadReg(reg, switch_port_mask & EXT_SWITCH_FLAG);

			value &= ~(1 << 31);
			value |= (mode << 31);

			macMultiMT7530WriteReg(reg, value, switch_port_mask & EXT_SWITCH_FLAG);
		}

	}
	printk("set storm contrl mode: 0x%x\r\n", mode);
	return (0);
}


/*___________________________________________________________________
**      function name: macMT7530SetStormCtrlPeriod
**      descriptions:
**         ...
**
**      parameters:
**         0 : 1 second
**         1 : 125 us
**       
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. kenny 2014/03/11
**_______________________________________________________________
*/

int macMT7530SetStormCtrlPeriod(u8 period)
{
	u32 reg, value;
	int i=0;

	if(period > 1)
		return -1;
	
	for(i=0; i<MAX_SWITCH_PORT_NUM; i++)
	{
		if(switch_port_mask & (1<<i))
		{
			reg = 0x201c + (i*0x100);
			value = macMultiMT7530ReadReg(reg, switch_port_mask & EXT_SWITCH_FLAG);

			value &= ~(1 << 26);
			value |= (period << 26);

			macMultiMT7530WriteReg(reg, value, switch_port_mask & EXT_SWITCH_FLAG);
		}

	}
	printk("set storm contrl period: 0x%x\r\n", period);

	return (0);
}

int macMT7530GetPortUpDownState(u8 port)
{
	u32 reg, value;
	int ret_val=0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	reg = 0x3008  + (0x100 * port);
	value = macMultiMT7530ReadReg(reg, extsw_flag);

	if ((value & 0x0001) == 0)	// =0: link down
	{
		ret_val = 0;
	}
	else
	{
	    ret_val = 1;
	}

	return ret_val;

}
EXPORT_SYMBOL(macMT7530GetPortUpDownState);

int macMT753SetPortUpDown(u8 port, u8 enable)
{
    
    u32 phy_addr, phy_reg, phy_value;
    u8  down_flag = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
    if (port >= MT7530_TOTAL_PORTS)
    {
        return (-1);
    }
    if(enable)
    {
        down_flag = 0; 
    }
    else
    {
        down_flag = 1;
    }
    phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
    phy_reg = MII_BMCR;
    phy_value = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(BMCR_PDOWN)) | (down_flag ? (BMCR_PDOWN) : 0);
    macMT7530PHYWriteReg(phy_addr, phy_reg, phy_value);

    if (macMT7530_APIDbgDumpEn){
        printk("%s() %2d (%02X, %02X) : %04X\n", __FUNCTION__, port, 
            phy_addr, phy_reg, phy_value);
    }

    return (0);

}

int macMT7530GetChipType(e_chip_type *chip_type)
{
    if(isMT7520S)
    {
        *chip_type = E_CHIP_MT7520S;
    }
    else if(isMT7520)
    {
        *chip_type = E_CHIP_MT7520F;
    }
    else if(isMT7520G)
    {
        *chip_type = E_CHIP_MT7520G;
    }
    else if(isMT7525)
    {
        *chip_type = E_CHIP_MT7525F;
    }
    else if(isMT7525G)
    {
        *chip_type = E_CHIP_MT7525G;
    }
    else if(isEN7521F)
    {
        *chip_type = E_CHIP_MT7521F;
    }				
    else if(isEN7526F)
    {
        *chip_type = E_CHIP_MT7526F;
    }
    else if(isEN751627)
    {
        *chip_type = E_CHIP_MT751627;
    }
    else if(isEN7528)
    {
        *chip_type = E_CHIP_MT7528;
    }
    else if(isEN7580)
    {
        *chip_type = E_CHIP_MT7580;
    }	
    return (0);
}

int macMT7530GetPortAllMac(u8 port, mt7530_switch_port_mac *outptr_PortMac)
{
    u32 value = 0, addr1 = 0, addr2 = 0;
    int ret_val=0, loop_timeout;
    int i = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

    if (port >= MT7530_TOTAL_PORTS) return (-1);
    loop_timeout = 1000000;
    while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)  // wait until not busy
    {
        if (--loop_timeout == 0)
        {
            ret_val = -1;
            break;
        }
    }

    value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT); // 3'b100 : Start search command (reset to 1st entry)
    macMultiMT7530WriteReg(REG_ATC_ADDR, value, extsw_flag);
    loop_timeout = 1000000;
    while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)  // wait until not busy
    {
        if (--loop_timeout == 0)
        {
            ret_val = -1;
            break;
        }
    }

    while (1)
    {
        value = macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag);
        if ( (value & REG_AT_SRCH_END_MASK) 
            && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
        {
            ret_val = 0;
            break;
        }
        if(value & REG_AT_SRCH_HIT_MASK)
        {
            value = macMultiMT7530ReadReg(REG_ATRD_ADDR, extsw_flag);
            if (((value & REG_ATWD_PORT_MASK) >> REG_ATWD_PORT_OFFT) & (1 << port))
            {
                addr1 = macMultiMT7530ReadReg(REG_TSRA1_ADDR, extsw_flag);
                outptr_PortMac->portmac[i].mac[0] = (addr1 >> 24) & 0xFF;
                outptr_PortMac->portmac[i].mac[1] = (addr1 >> 16) & 0xFF;
                outptr_PortMac->portmac[i].mac[2] = (addr1 >>  8) & 0xFF;
                outptr_PortMac->portmac[i].mac[3] = (addr1      ) & 0xFF;
                addr2 = macMultiMT7530ReadReg(REG_TSRA2_ADDR, extsw_flag);
                outptr_PortMac->portmac[i].mac[4]  = (addr2 >> 24) & 0xFF;
                outptr_PortMac->portmac[i].mac[5]  = (addr2 >> 16) & 0xFF;
                i++;
                outptr_PortMac->vaild_entry_num = i;
                //if new mac_info == old mac_info , no need add this mac info
                
				printk("MAC %08X%04X :", addr1, (addr2 >> 16));
				printk(" TIMER:%d, SA_PORT_FW:%d, SA_MIR_EN:%d, USER_PRI:%d,\n",
				  (value>>24)&0xFF, (value>>20)&7, (value>>19)&1, (value>>16)&7);
				printk("                   EG_TAG:%d, LEAKY_EN:%d, PORT:%X, STATUS:%d, TYPE:%d\n",
				  (value>>13)&7, (value>>12)&1, (value>>4)&0xFF, (value>>2)&3, (value)&3);
                printk("                   IVL:%d, FID:%d,CVID:%d\n",
                  (addr2>>15)&1, (addr2>>12)&7, (addr2)&0xFFF);
                  
                if( (outptr_PortMac->vaild_entry_num > 1) 
                    && (0 == memcmp(outptr_PortMac->portmac[i-2].mac, outptr_PortMac->portmac[i-1].mac, 6)) )
                {
                    i--;
                    outptr_PortMac->vaild_entry_num = i;
                }
                if(i > (MAX_VAILD_PORT_ENTRY - 1))
                {
                    break;
                }
            }
        }
        value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT); // 3'b101 : Next search command (next entry)
        macMultiMT7530WriteReg(REG_ATC_ADDR, value, extsw_flag);
        loop_timeout = 1000000;
        while (macMultiMT7530ReadReg(REG_ATC_ADDR, extsw_flag) & REG_ATC_BUSY_MASK)  // wait until not busy
        {
            if (--loop_timeout == 0)
            {
                ret_val = -1;
                break;
            }
        }
    }
    printk("\nTotal Entry Num:%d\n", outptr_PortMac->vaild_entry_num);
    return ret_val;
}

int macMT7530GetPortByMac(mt7530_switch_macinfo *inptr_PortMac)
{
	u32 value = 0, reg = 0;
	u32 addr1 = 0, addr2 = 0;
    int ret = 0;
    u8 switch_port = 0, portNameIdx = 0;

	reg = REG_ATC_ADDR + GSW_BASE;
	while (1){	// wait until not busy
		value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	// trigger 1st search
	reg = REG_ATC_ADDR + GSW_BASE;
	value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);	// 3'b100 : Start search command (reset to 1st entry)
	switch_reg_write(reg, value);
	while (1){	// wait until not busy
		value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	while (1){
		if ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
		{
			break;
		}
        if(value & REG_AT_SRCH_HIT_MASK)
        {
    		reg = REG_TSRA1_ADDR + GSW_BASE;
    		addr1 = switch_reg_read(reg);

    		reg = REG_TSRA2_ADDR + GSW_BASE;
    		addr2 = switch_reg_read(reg);

    		// other parameters
    		reg = REG_ATRD_ADDR + GSW_BASE;
    		value = switch_reg_read(reg);

            /* get PORT info*/
            ret = getPortByMask((value>>4)&0xFF, &switch_port);
            if ( 0 != ret )
                break;
            portNameIdx = getPortNameIdxByPortid(switch_port);
            if(portNameIdx >= MAX_PORT_NAME_NUM)
                break;

            if((inptr_PortMac->mac[0] == ((addr1 >> 24) & 0xFF)) &&
                (inptr_PortMac->mac[1] == ((addr1 >> 16) & 0xFF)) &&
                (inptr_PortMac->mac[2] == ((addr1 >>  8) & 0xFF)) &&
                (inptr_PortMac->mac[3] == ((addr1      ) & 0xFF)) &&
                (inptr_PortMac->mac[4] == ((addr2 >> 24) & 0xFF)) &&
                (inptr_PortMac->mac[5] == ((addr2 >> 16) & 0xFF))){
                inptr_PortMac->port = portNameIdx;
                break;
            }
        }
		// trigger next
		reg = REG_ATC_ADDR + GSW_BASE;
		value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT);	// 3'b101 : Next search command (next entry)
		switch_reg_write(reg, value);
		while (1){	// wait until not busy
			value = switch_reg_read(reg);
			if ((value & REG_ATC_BUSY_MASK) == 0){
				break;
			}
		}
	}

	return 0;
}
int portVlanBindSetFlag = 0;
#define wifiVPort (10)
extern unsigned short port_vlan_bind_vid;
u8 port_vlan_bind_mask = 0;
int macMT7530AddPortVlanBind(u8 port, u16 bindvlan)
{
	u32 reg, value1 = 0, value2 = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS)
	{
		if(port != wifiVPort)	/* port 10 means wifi */
	    	return (-1);
	}

	if(bindvlan > 4095)
	{
	    return (-1);
	}

	macMultiMT7530WriteReg(0x0090,(0x80000000 + bindvlan), extsw_flag);
	value1 = macMultiMT7530ReadReg(0x0094, extsw_flag);
	value1 &= ~(0x3<<28);
	value1 |= (0x1<<28);
	if(port != wifiVPort)
	{
		value2 = macMultiMT7530ReadReg(0x0098, extsw_flag);
		value2 |= 0x2<<(port*2);
		port_vlan_bind_vid = bindvlan;
		port_vlan_bind_mask |= (1<<port);
	}
	macMultiMT7530WriteReg(0x0094,value1, extsw_flag);
	macMultiMT7530WriteReg(0x0098,value2, extsw_flag);
	macMultiMT7530WriteReg(0x0090,(0x80001000 + bindvlan), extsw_flag);

	if(portVlanBindSetFlag)
		return 0;
	
	reg = 0x2610;
	value1 = macMultiMT7530ReadReg(reg, extsw_flag);
	value1 &= ~(0x7 << 8);
	macMultiMT7530WriteReg(reg, value1, extsw_flag);

	reg = 0x2604;
	value1 = macMultiMT7530ReadReg(reg, extsw_flag);
	value1 &= ~0x3;
	value1 |= 0x3;
	macMultiMT7530WriteReg(reg, value1, extsw_flag);
	
	portVlanBindSetFlag = 1;

	return 0;
}

int macMT7530DelPortVlanBind(u8 port, u16 bindvlan)
{
	u32 reg, value, value2 = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS)
	{
		if(port != wifiVPort)	/* port 10 means wifi */
	    	return (-1);
	}

	if(bindvlan > 4095)
	{
	    return (-1);
	}

	macMultiMT7530WriteReg(0x0090,(0x80000000 + bindvlan), extsw_flag);
	value = macMultiMT7530ReadReg(0x0094, extsw_flag);

	if(port != wifiVPort)
	{
		value2 = macMultiMT7530ReadReg(0x0098, extsw_flag);
		value2 &= ~(0x3<<(port*2));
		port_vlan_bind_mask &= ~(1<<port);
		if(port_vlan_bind_mask == 0)
		{
			portVlanBindSetFlag = 0;
			port_vlan_bind_vid = 0;
			value &= ~(0x3<<28);
			value |= (0x2<<28);
		}
	}

	macMultiMT7530WriteReg(0x0094,value, extsw_flag);
	macMultiMT7530WriteReg(0x0098,value2, extsw_flag);
	macMultiMT7530WriteReg(0x0090,(0x80001000 + bindvlan), extsw_flag);

	return 0;
}

/*___________________________________________________________________
**      function name: macMT7530VLan_Entry_Enable
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/09/17
**_______________________________________________________________
*/
int macMT7530VLan_Entry_Enable(u16 vid, u8 sw)
{
	u32 reg, value, timeout;

	if ((vid >= 4096) || (vid == 0)) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	if (sw)
	{
		value = (1 << 31) | (3 << 12) | vid;	// 3 : validate specific VLAN entry
	}
	else
	{
		value = (1 << 31) | (2 << 12) | vid;	// 2 : invalidate specific VLAN entry
	}
	macMT7530WriteReg(reg, value);
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = 0x90; // VTCR
        timeout = 100000;
        while (-- timeout) {
            value = macMultiMT7530ReadReg(reg, 1);      // VTCR
            if ((value & (1 << 31)) == 0) break;    // table busy finish?
        }
        
        if (sw)
        {
            value = (1 << 31) | (3 << 12) | vid;    // 3 : validate specific VLAN entry
        }
        else
        {
            value = (1 << 31) | (2 << 12) | vid;    // 2 : invalidate specific VLAN entry
        }
        macMultiMT7530WriteReg(reg, value, 1);
        timeout = 100000;
        while (-- timeout) {
            value = macMultiMT7530ReadReg(reg, 1);      // VTCR
            if ((value & (1 << 31)) == 0) break;    // table busy finish?
        }
    }
#endif

	return (0);
}
EXPORT_SYMBOL(macMT7530VLan_Entry_Enable);

/*___________________________________________________________________
**      function name: macMT7530VLan_Entry_SetETag
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/09/17
**_______________________________________________________________
*/
int macMT7530VLan_Entry_SetETag(u16 vid, u8 port, u8 etag)
{
	u32 reg, value, value_vawd1, value_vawd2, timeout;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if ((vid >= 4096) || (vid == 0) || (port >= MT7530_TOTAL_PORTS) || (etag >= 4)) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	value = (1 << 31) | (0 << 12) | vid;	// 0 : read specific VLAN entry
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}
	if (value & (1 << 16)) return (-1);	// means this index is out of valid index

	value_vawd1 = macMultiMT7530ReadReg(0x94, extsw_flag);
	value_vawd2 = macMultiMT7530ReadReg(0x98, extsw_flag);

	value_vawd1 = (value_vawd1 & ~(1 << 29) ) | (1 << 28);	// VTAG_EN(b28)=1, EG_CON(b29)=0
	value_vawd2 = (value_vawd2 & ~(3 << (port * 2))) | (etag << (port * 2));	// set EG_TAG (2 bits per port)

	macMultiMT7530WriteReg(0x94, value_vawd1, extsw_flag);
	macMultiMT7530WriteReg(0x98, value_vawd2, extsw_flag);

	value = (1 << 31) | (1 << 12) | vid;	// 1 : write specific VLAN entry
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	return (0);
}
EXPORT_SYMBOL(macMT7530VLan_Entry_SetETag);

/*___________________________________________________________________
**      function name: macMT7530VLan_Entry_GetETag
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/09/17
**_______________________________________________________________
*/
int macMT7530VLan_Entry_GetETag(u16 vid, u8 port, u8* outptr_etag)
{
	u32 reg, value, value_vawd1, value_vawd2, timeout;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if ((vid >= 4096) || (vid == 0) || (port >= MT7530_TOTAL_PORTS)) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	value = (1 << 31) | (0 << 12) | vid;	// 0 : read specific VLAN entry
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}
	if (value & (1 << 16)) return (-1);	// means this index is out of valid index

	value_vawd1 = macMultiMT7530ReadReg(0x94, extsw_flag);
	value_vawd2 = macMultiMT7530ReadReg(0x98, extsw_flag);

	if (!(value_vawd1 & (1 << 28)) || (value_vawd1 & (1 << 29))) return (-1);	// VTAG_EN(b28)=1, EG_CON(b29)=0

	(*outptr_etag) = (value_vawd2 >> (port * 2)) & 3;	// EG_TAG (2 bits per port)

	return (0);
}
EXPORT_SYMBOL(macMT7530VLan_Entry_GetETag);

/*___________________________________________________________________
**      function name: macMT7530VLan_Entry_SetSTag
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/09/17
**_______________________________________________________________
*/
int macMT7530VLan_Entry_SetSTag(u16 vid, u8 port, u8 stag)
{
	u32 reg, value, value_vawd1, value_vawd2, timeout;
	u8 bit_offset;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if ((vid >= 4096) || (vid == 0) || (port >= MT7530_TOTAL_PORTS) || (stag >= 4)) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	value = (1 << 31) | (0 << 12) | vid;	// 0 : read specific VLAN entry
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}
	if (value & (1 << 16)) return (-1);	// means this index is out of valid index

	value_vawd1 = macMultiMT7530ReadReg(0x94, extsw_flag);
	value_vawd2 = macMultiMT7530ReadReg(0x98, extsw_flag);

	#if (0)	//20120918 Lidong_Hu confirm do not need following checking or setting
	if (~(value_vawd1 & (1 << 31))) return (-1);	// PORT_STAG shall be 1
	#endif
	#if (1)	//20120925 Lidong_Hu again request to set
	value_vawd1 |= (1 << 31);			// PORT_STAG shall be 1
	macMultiMT7530WriteReg(0x94, value_vawd1, extsw_flag);
	#endif

	if (port < 4)	// S_TAG1 for port 0~3
	{
		bit_offset = 4 + (port * 3);
		value_vawd1 = (value_vawd1 & ~(7 << bit_offset)) | (stag << bit_offset);	// set S_TAG1 (3 bits per port)
		macMultiMT7530WriteReg(0x94, value_vawd1, extsw_flag);
	}
	else		// S_TAG2 for port 4~7
	{
		bit_offset = 16 + ((port - 4) * 3);
		value_vawd2 = (value_vawd2 & ~(7 << bit_offset)) | (stag << bit_offset);	// set S_TAG2 (3 bits per port)
		macMultiMT7530WriteReg(0x98, value_vawd2, extsw_flag);
	}

	value = (1 << 31) | (1 << 12) | vid;	// 1 : write specific VLAN entry
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	return (0);
}
EXPORT_SYMBOL(macMT7530VLan_Entry_SetSTag);

/*___________________________________________________________________
**      function name: macMT7530VLan_Entry_GetSTag
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/09/17
**_______________________________________________________________
*/
int macMT7530VLan_Entry_GetSTag(u16 vid, u8 port, u8* outptr_stag)
{
	u32 reg, value, value_vawd1, value_vawd2, timeout;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if ((vid >= 4096) || (vid == 0) || (port >= MT7530_TOTAL_PORTS)) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	value = (1 << 31) | (0 << 12) | vid;	// 0 : read specific VLAN entry
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	timeout = 100000;
	while (-- timeout) {
		value = macMultiMT7530ReadReg(reg, extsw_flag);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}
	if (value & (1 << 16)) return (-1);	// means this index is out of valid index

	value_vawd1 = macMultiMT7530ReadReg(0x94, extsw_flag);
	value_vawd2 = macMultiMT7530ReadReg(0x98, extsw_flag);

	#if (1)
	//20120918 Lidong_Hu confirm do not need following checking or setting
	//20120925 Lidong_Hu again request to check
	if (!(value_vawd1 & (1 << 31))) return (-1);	// PORT_STAG shall be 1
	#endif

	if (port < 4)	// S_TAG1 for port 0~3
	{
		(*outptr_stag) = (value_vawd1 >> (4 + (port * 3))) & 7;		// S_TAG1 (3 bits per port)
	}
	else		// S_TAG2 for port 4~7
	{
		(*outptr_stag) = (value_vawd2 >> (16 + ((port - 4) * 3))) & 7;	// S_TAG2 (3 bits per port)
	}

	return (0);
}
EXPORT_SYMBOL(macMT7530VLan_Entry_GetSTag);

/*___________________________________________________________________
**      function name: macMT7530VLan_Port_AddSTag
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/09/17
**_______________________________________________________________
*/
int macMT7530VLan_Port_AddSTag(u8 port, u8 stag, u16 vid)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if ((vid >= 4096) || (vid == 0) || (port >= MT7530_TOTAL_PORTS) || (stag >= 4)) return (-1);

	reg = 0x2020 + (0x100 * port) + (4 * (stag >> 1));
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (stag % 2)
	{
		value = (value & ~(0xFFF << 12)) | (vid << 12);
	}
	else
	{
		value = (value & ~0xFFF) | vid;
	}
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	return (0);
}
EXPORT_SYMBOL(macMT7530VLan_Port_AddSTag);

/*___________________________________________________________________
**      function name: macMT7530VLan_Entry_ETagMode
**      descriptions:
**         ...
**
**      parameters:
**         0: set vid Entry to EG_CON mode
**         1: set vid Entry to EG_TAG mode
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/09/25
**_______________________________________________________________
*/
int macMT7530VLan_Entry_SetETagMode(u16 vid, u8 etag_mode)
{
	u32 reg, value, value_vawd1, timeout;

	if ((vid >= 4096) || (vid == 0)) return (-1);

	reg = 0x90;	// VTCR
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

	value = (1 << 31) | (0 << 12) | vid;	// 0 : read specific VLAN entry
	macMT7530WriteReg(reg, value);
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}
	if (value & (1 << 16)) return (-1);	// means this index is out of valid index

	value_vawd1 = macMT7530ReadReg(0x94);
	value_vawd1 &= ~(1 << 29);			// bit29 : EG_CON : "when EG_CON is set, EG_TAG field will be ignored"
	value_vawd1 |= ((etag_mode) ? (0 << 29) : (1 << 29));	// etag_mode = inverse of EG_CON
	macMT7530WriteReg(0x94, value_vawd1);

	value = (1 << 31) | (1 << 12) | vid;	// 1 : write specific VLAN entry
	macMT7530WriteReg(reg, value);
	timeout = 100000;
	while (-- timeout) {
		value = macMT7530ReadReg(reg);		// VTCR
		if ((value & (1 << 31)) == 0) break;	// table busy finish?
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = 0x90; // VTCR
        timeout = 100000;
        while (-- timeout) {
            value = macMultiMT7530ReadReg(reg, 1);      // VTCR
            if ((value & (1 << 31)) == 0) break;    // table busy finish?
        }
        
        value = (1 << 31) | (0 << 12) | vid;    // 0 : read specific VLAN entry
        macMultiMT7530WriteReg(reg, value, 1);
        timeout = 100000;
        while (-- timeout) {
            value = macMultiMT7530ReadReg(reg, 1);      // VTCR
            if ((value & (1 << 31)) == 0) break;    // table busy finish?
        }
        if (value & (1 << 16)) return (-1); // means this index is out of valid index
        
        value_vawd1 = macMultiMT7530ReadReg(0x94, 1);
        value_vawd1 &= ~(1 << 29);          // bit29 : EG_CON : "when EG_CON is set, EG_TAG field will be ignored"
        value_vawd1 |= ((etag_mode) ? (0 << 29) : (1 << 29));   // etag_mode = inverse of EG_CON
        macMultiMT7530WriteReg(0x94, value_vawd1, 1);
        
        value = (1 << 31) | (1 << 12) | vid;    // 1 : write specific VLAN entry
        macMultiMT7530WriteReg(reg, value, 1);
        timeout = 100000;
        while (-- timeout) {
            value = macMultiMT7530ReadReg(reg, 1);      // VTCR
            if ((value & (1 << 31)) == 0) break;    // table busy finish?
        }
    }
#endif

	return (0);
}
EXPORT_SYMBOL(macMT7530VLan_Entry_SetETagMode);

/*___________________________________________________________________
**      function name: macMT7530EnableMIBCounter
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/05
**_______________________________________________________________
*/
int macMT7530EnableMIBCounter(u32 enable)
{
	u32 reg, value;

	reg = 0x4FE0;
	value = macMT7530ReadReg(reg);
	value = (value & (~(1 << 31))) | ((enable) ? (1 << 31) : 0);
	macMT7530WriteReg(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif

	if (macMT7530_APIDbgDumpEn) printk("%s() (%08X) : %08X\n", __FUNCTION__, reg, value);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530ClearMIBCounter
**      descriptions:
**         ...
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/05
**_______________________________________________________________
*/
int macMT7530ClearMIBCounter(void)
{
	u32 reg, value;

	reg = 0x4FE0;
	value = macMT7530ReadReg(reg);
	macMT7530WriteReg(reg, (value & (~(1 << 31))));
	macMT7530WriteReg(reg, (value | (1 << 31)));

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        value = macMultiMT7530ReadReg(reg, 1);
        macMultiMT7530WriteReg(reg, (value & (~(1 << 31))), 1);
        macMultiMT7530WriteReg(reg, (value | (1 << 31)), 1);
    }
#endif
	return (0);
}

int macMT7530ClearPortMIBCounter(int port)
{

	u32 reg, value;
	
	reg = 0x4FE0;
	value = macMT7530ReadReg(reg);
	macMT7530WriteReg(reg, (value | (1 << (16+port))));
	macMT7530WriteReg(reg, (value & (~(1 << (16+port)))));

	return (0);
}


/*___________________________________________________________________
**      function name: macMT7530SetRxOctMode
**      descriptions:
**         select which mode will count into MIB_ID_RX_OCT_CNT_H/MIB_ID_RX_OCT_CNT_L
**
**      parameters:
**         mt7530_switch_api_RxTxOctMode mode = 1 (OCTMODE_BAD_ONLY)
**                                              2 (OCTMODE_GOOD_ONLY)
**                                              3 (OCTMODE_BAD_GOOD_BOTH)
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/05
**_______________________________________________________________
*/
int macMT7530SetRxOctMode(mt7530_switch_api_RxTxOctMode mode)
{
	u32 reg, value;

	if (mode > OCTMODE_BAD_GOOD_BOTH) return (-1);

	reg = 0x4FE0;
	value = macMT7530ReadReg(reg);
	value = (value & ~(3 << 6)) | ((u32)mode << 6);
	macMT7530WriteReg(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        value = macMultiMT7530ReadReg(reg, 1);
        value = (value & ~(3 << 6)) | ((u32)mode << 6);
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetTxOctMode
**      descriptions:
**         select which mode will count into MIB_ID_TX_OCT_CNT_H/MIB_ID_TX_OCT_CNT_L
**
**      parameters:
**         mt7530_switch_api_RxTxOctMode mode = 1 (OCTMODE_BAD_ONLY)
**                                              2 (OCTMODE_GOOD_ONLY)
**                                              3 (OCTMODE_BAD_GOOD_BOTH)
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/05
**_______________________________________________________________
*/
int macMT7530SetTxOctMode(mt7530_switch_api_RxTxOctMode mode)
{
	u32 reg, value;

	if (mode > OCTMODE_BAD_GOOD_BOTH) return (-1);

	reg = 0x4FE0;
	value = macMT7530ReadReg(reg);
	value = (value & ~(3 << 4)) | ((u32)mode << 4);
	macMT7530WriteReg(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        value = macMultiMT7530ReadReg(reg, 1);
        value = (value & ~(3 << 4)) | ((u32)mode << 4);
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetBxPktMode
**      descriptions:
**         select which mode will count into BX_PKT_xxx_CNT
**
**      parameters:
**         mt7530_switch_api_BxPktMode mode = 1 (BXMODE_TX_ONLY)
**                                            2 (BXMODE_RX_ONLY)
**                                            3 (BXMODE_TX_RX_BOTH)
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/09
**_______________________________________________________________
*/
int macMT7530SetBxPktMode(mt7530_switch_api_BxPktMode mode)
{
#if (MT7530_MIBCNT_VER == 1)
	u32 reg, value;

	if (mode > BXMODE_TX_RX_BOTH) return (-1);

	reg = 0x4FE0;
	value = macMT7530ReadReg(reg);
	value = (value & (~3)) | (u32)mode;
	macMT7530WriteReg(reg, value);
	return (0);
#endif
#if (MT7530_MIBCNT_VER == 2)
	return (-1);
#endif
}

/*___________________________________________________________________
**      function name: macMT7530GetPortPhyAddr
**      descriptions:
**
**      parameters:
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/07/09
**_______________________________________________________________
*/
int macMT7530GetPortPhyAddr(u8 port, u32 extsw_flag)
{
	if (port >= MT7530_TOTAL_PORTS) return (-1);

    if (extsw_flag)
    {
        return ((port + GSW_PHY_OFFSET_23 + 1) & 0x1f);
    }

	// PHY_POLL 0x7018[4:0] = PHY_ST_ADDR
	if( (!use_ext_switch) && (!isFPGA) ){
		port += 8;
	}

	return port;

}

/*___________________________________________________________________
**      function name: macMT7530APIDbgDumpEn
**      descriptions:
**         ...
**
**      parameters:
**         u8 DbgDump_enable
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/21
**_______________________________________________________________
*/
int macMT7530APIDbgDumpEn(u32 DbgDump_enable)
{
	macMT7530_APIDbgDumpEn = DbgDump_enable;
	printk("%s(%d)\n", __FUNCTION__, DbgDump_enable);
	return (0);
}

/*___________________________________________________________________
**      function name: doP6Cal
**      descriptions:
**         ...
**
**      parameters:
**         u8 DbgDump_enable
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/21
**_______________________________________________________________
*/

#define EXTERNAL 1
#define INTERNAL 0

#define SOC_TO_EXT 0
#define EXT_TO_SOC 1

#define HEAD    1
#define TAIL    2
#define FINISH  3

u32 macMT7530ReadReg2(u32 gsw_reg, u8 external)
{
    if(external){
        return gswPbusRead(gsw_reg);
    }else{
        return read_reg_word(GSW_BASE + gsw_reg);
    }
}

int macMT7530WriteReg2(u32 gsw_reg, u32 gsw_data, u8 external)
{
    if(external){
        return gswPbusWrite(gsw_reg, gsw_data);
    }else{
        write_reg_word((GSW_BASE + gsw_reg), gsw_data);
        return 0;
    }
}

int ErrChk(u32 RegAddr, u8 RxSwitch){
	u32 ErrCnt, RegVal;

	ErrCnt = 0;

	RegVal = macMT7530ReadReg2(RegAddr, RxSwitch);
	
	RegVal = RegVal | 0x40000000;
	macMT7530WriteReg2(RegAddr, RegVal, RxSwitch);

	RegVal = RegVal & 0xBFFFFFFF;
	macMT7530WriteReg2(RegAddr, RegVal, RxSwitch);

	ErrCnt = macMT7530ReadReg2(RegAddr, RxSwitch);
	ErrCnt = (ErrCnt & 0x00000F00) >> 8;

	return ErrCnt;
}

void macMT7530PortReset(u8 port)
{
	u32 value, reg;
	u8 linkstate;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	reg = 0x3008 + (port*0x100);
	value = macMultiMT7530ReadReg(reg, 1);
	
	linkstate = (value & (1<<0))?1:0;
	if(linkstate)
	{
		macMT7530SetPortAdmin(1, extsw_flag ? (port | EXT_SWITCH_FLAG) : port);
		mdelay(100);  /* changed by baiyong & jason.gong*/
		macMT7530SetPortAdmin(0, extsw_flag ? (port | EXT_SWITCH_FLAG) : port);
	}
	return;
}
int macMT7530SetPortMaxBitRate(u8 port, char* speed)
{
	u32 value, value1, phy_addr, phy_reg, reg;
	u8 linkstate;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	if (speed==NULL) return (-1);
		
	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	value1 = macMT7530PHYReadReg(phy_addr, phy_reg);

	if(strcmp(speed, "Auto") == 0)
		value = (BMCR_ANENABLE | BMCR_ANRESTART);
	else if(strcmp(speed, "100") == 0)
		value = (BMCR_SPEED100 | BMCR_ANRESTART);
	else if(strcmp(speed, "10") == 0)
		value = BMCR_ANRESTART;
	else
		value = (BMCR_ANENABLE | BMCR_ANRESTART);

	if( (value1 & ~0xcfbf) == (value& ~0xcfbf))
		return 0;
	
	value1 &= 0xcfbf;
	value |= value1;
	macMT7530PHYWriteReg(phy_addr, phy_reg, value);

	return 0;
}
EXPORT_SYMBOL(macMT7530SetPortMaxBitRate);

int macMT7530GetPortMaxBitRate(u8 port, char* speed)
{
	u32 value, phy_addr, phy_reg;
	u32 reg, val;
	u8 spd, linkstate;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if(NULL == speed) return (-1);
	if (port >= MT7530_TOTAL_PORTS) return (-1);
		
	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	value = macMT7530PHYReadReg(phy_addr, phy_reg);

	if(value&BMCR_ANENABLE){
		reg = 0x3008 + (port*0x100);
		val = macMultiMT7530ReadReg(reg, extsw_flag);
	
		linkstate = (val & (1<<0))?1:0;
	
		if(linkstate){
			spd = (val & (0x3<<2)) >> 2;
			if(2 == spd)
			{
				strcpy(speed, "1000");
			}
			else if(1 == spd)
			{
				strcpy(speed, "100");
			}
			else if(0 == spd)
			{
				strcpy(speed, "10");
			}	
			else
				return -1;
		}
		else{
			strcpy(speed, "Down");
		}
		
	}
	else if(value&BMCR_SPEED100)
		strcpy(speed, "100");
	else
		strcpy(speed, "10");
	
	return 0;
}

int macMT7530SetPortDuplexMode(u8 port, char* Mode)
{
	u32 value, value1, phy_addr, phy_reg, reg;
	u8 linkstate;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	if (Mode==NULL) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	value1 = macMT7530PHYReadReg(phy_addr, phy_reg);

	if(strcmp(Mode, "Auto") == 0)
		value = (BMCR_ANENABLE | BMCR_ANRESTART);
	else if(strcmp(Mode, "Full") == 0)
		value = (BMCR_FULLDPLX | BMCR_ANRESTART);
	else if(strcmp(Mode, "Half") == 0)
		value = BMCR_ANRESTART;
    else
        return -1;

	if( (value1&0x0100) == (value&0x0100))
		return 0;
	
	value1 &= 0xeeff;
	value |= value1;
	macMT7530PHYWriteReg(phy_addr, phy_reg, value);

	macMT7530PortReset(extsw_flag ? (port | EXT_SWITCH_FLAG) : port);

	return 0;
}
EXPORT_SYMBOL(macMT7530SetPortDuplexMode);

int macMT7530GetPortDuplexMode(u8 port, int* Mode)
{
	u32 value, phy_addr, phy_reg;
	u32 reg, val;
	u8 full, linkstate;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	if (Mode==NULL) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	value = macMT7530PHYReadReg(phy_addr, phy_reg);

	if(value&BMCR_ANENABLE){
		reg = 0x3008 + (port*0x100);
		val = macMultiMT7530ReadReg(reg, extsw_flag);
		
		linkstate = (val & (1<<0))?1:0;

		if(linkstate){
			full = (val & (1<<1));
			if(full)
				strcpy(Mode, "Full");
			else
				strcpy(Mode, "Half");
		}
		else{
			strcpy(Mode, "Down");
		}
	}
	else if(value&BMCR_FULLDPLX)
		strcpy(Mode, "Full");
	else
		strcpy(Mode, "Half");


	return 0;
}

int macMT7530GetPortRxByte(u8 port, unsigned int* RxByte_L, unsigned int* RxByte_H)
{
	u32 value, reg;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = REG_PORT_RX_BYTE_LOW_CNT + (0x100 * port);
	*RxByte_L = macMultiMT7530ReadReg(reg, extsw_flag);

	reg = REG_PORT_RX_BYTE_HIGH_CNT + (0x100 * port);
	*RxByte_H = macMultiMT7530ReadReg(reg, extsw_flag);

	return 0;
}

int macMT7530GetPortTxByte(u8 port, unsigned int* TxByte_L, unsigned int* TxByte_H)
{
	u32 value, reg;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	reg = REG_PORT_TX_BYTE_LOW_CNT + (0x100 * port);
	*TxByte_L = macMultiMT7530ReadReg(reg, extsw_flag);

	reg = REG_PORT_TX_BYTE_HIGH_CNT + (0x100 * port);
	*TxByte_H = macMultiMT7530ReadReg(reg, extsw_flag);

	return 0;
}

int macMT7530SetRgmiiRxCtrlDelay(u8 rx_ctrl_delay, u8 ext_switch)
{
    u32 reg, value;

    if(rx_ctrl_delay > 7)
    {
        printk("error:rgmii rx ctrl delay must be 0~7!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_RX_CLK_CTRL, ext_switch, RGMII_RX_CTRL_DELAY_MASK, RGMII_RX_CTRL_DELAY_SHIFT, rx_ctrl_delay);

    return (0);
}  
int macMT7530SetRgmiiRxDataDelay(u8 rx_data_delay, u8 ext_switch)
{
    u32 reg, value;

    if(rx_data_delay > 7)
    {
        printk("error:rgmii rx data delay must be 0~7!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_RX_CLK_CTRL, ext_switch, RGMII_RX_DATA_DELAY_MASK, RGMII_RX_DATA_DELAY_SHIFT, rx_data_delay);

    return (0);
}   
int macMT7530SetRgmiiRxClkDelay(u8 rx_clk_delay, u8 ext_switch)
{
    u32 reg, value;

    if(rx_clk_delay > 7)
    {
        printk("error:rgmii rx clock delay must be 0~7!\n");
        return -1;
    }

    value = macMT7530ReadReg2(EXT_RGMII_RX_CLK_CTRL, ext_switch);

    if((value & RGMII_RX_CLK_ALIGN_MASK) != 0)
    {
        printk("RXC/RXD is central-aligned, RXC does not pass through the delay chain!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_RX_CLK_CTRL, ext_switch, RGMII_RX_CLK_DELAY_MASK, RGMII_RX_CLK_DELAY_SHIFT, rx_clk_delay);

    return (0);
}
int macMT7530SetRgmiiTxEnDelay(u8 tx_en_delay, u8 ext_switch)
{
    u32 reg, value;

    if(tx_en_delay > 7)
    {
        printk("error:rgmii txen must be 0~7!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_TX_CLK_CTRL, ext_switch, RGMII_TX_EN_DELAY_MASK, RGMII_TX_EN_DELAY_SHIFT, tx_en_delay );

    return (0);
}   
int macMT7530SetRgmiiTxDataDelay(u8 tx_data_delay, u8 ext_switch)
{
    u32 reg, value;

    if(tx_data_delay > 7)
    {
        printk("error:rgmii tx data delay must be 0~7!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_TX_CLK_CTRL, ext_switch, RGMII_TX_DATA_DELAY_MASK, RGMII_TX_DATA_DELAY_SHIFT, tx_data_delay);

    return (0);
}    
int macMT7530SetRgmiiTxClkDelay(u8 tx_clk_delay, u8 ext_switch)
{
    u32 reg, value;

    if(tx_clk_delay > 7)
    {
        printk("error:rgmii tx clock delay must be 0~7!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_TX_CLK_CTRL, ext_switch, RGMII_TX_CLK_DELAY_MASK, RGMII_TX_CLK_DELAY_SHIFT, tx_clk_delay);

    return (0);
}    
int macMT7530SetRgmiiRxClkEn(u8 rx_clk_en, u8 ext_switch)
{
    u32 reg, value;

    if(rx_clk_en > 1)
    {
        printk("error:rgmii rxc clock enable must be 0/1!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_CLK_EN_CTRL, ext_switch, RGMII_RX_CLK_EN_MASK, RGMII_RX_CLK_EN_SHIFT, rx_clk_en);

    return (0);
}

int macMT7530SetRgmiiRxClkAlign(u8 rx_clk_align, u8 ext_switch)
{
    u32 reg, value;

    if(rx_clk_align > 1)
    {
        printk("error:rgmii rx clock align must be 0/1!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_RX_CLK_CTRL, ext_switch, RGMII_RX_CLK_ALIGN_MASK, RGMII_RX_CLK_ALIGN_SHIFT, rx_clk_align);

    return (0);
}   


int macMT7530SetRgmiiRxClkInv(u8 rx_clk_inv, u8 ext_switch)
{
    u32 reg, value;

    if(rx_clk_inv > 1)
    {
        printk("error:rgmii rx clock inverted must be 0/1!\n");
        return -1;
    }

    value = macMT7530ReadReg2(EXT_RGMII_RX_CLK_CTRL, ext_switch);

    if((value & RGMII_RX_CLK_ALIGN_MASK) != 0)
    {
        printk("RXC/RXD is central-aligned, RXC does not pass through the delay chain!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_RX_CLK_CTRL, ext_switch, RGMII_RX_CLK_INV_MASk, RGMII_RX_CLK_INV_SHIFT, rx_clk_inv);

    return (0);
}
  
int macMT7530SetRgmiiTxClkEn(u8 tx_clk_en, u8 ext_switch)
{
    u32 reg, value;

    if(tx_clk_en > 1)
    {
        printk("error:rgmii txc clock enable must be 0/1!\n");
        return -1;
    }
    
    IO_SMASK(EXT_RGMII_CLK_EN_CTRL, ext_switch, RGMII_TX_CLK_EN_MASK, RGMII_TX_CLK_EN_SHIFT, tx_clk_en);

    return (0);
}

int macMT7530SetRgmiiTxClkAlign(u8 tx_clk_align, u8 ext_switch)
{
    u32 reg, value;

    if(tx_clk_align > 1)
    {
        printk("error:rgmii tx clock align must be 0/1!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_TX_CLK_CTRL, ext_switch, RGMII_TX_CLK_ALIGN_MASK, RGMII_TX_CLK_ALIGN_SHIFT, tx_clk_align);

    return (0);
}   

int macMT7530SetRgmiiTxClkInv(u8 tx_clk_inv, u8 ext_switch)
{
    u32 reg, value;

    if(tx_clk_inv > 1)
    {
        printk("error:rgmii tx clock inverted must be 0/1!\n");
        return -1;
    }

    IO_SMASK(EXT_RGMII_TX_CLK_CTRL, ext_switch, RGMII_TX_CLK_INV_MASk, RGMII_TX_CLK_INV_SHIFT, tx_clk_inv);

    return (0);
}   

int macMT7530doP6Cal(void)
{
	u32 RxChannel, DAC, index, RxRegAddr, TxRegAddrl, TxRegAddr2, Regvalue, ErrCnt, Val;
    u8 TxSwitch=0,RxSwitch=0,direction=0;
    u8 delay_windows[5][2]={0},state;
    u16 delay_tap=0;
	RxChannel = 0;
	DAC = 0;

    for (direction=0;direction < 2;direction++){
        printk("Direction %s \n",direction?"EXT_TO_SOC":"SOC_TO_EXT");     
        if(direction == SOC_TO_EXT){
            TxSwitch = INTERNAL;
            RxSwitch = EXTERNAL;
        }else{
            TxSwitch = EXTERNAL;
            RxSwitch = INTERNAL;
        }
    	//Tx training mode enable
    	TxRegAddrl = 0x7A40;
    	Regvalue = macMT7530ReadReg2(TxRegAddrl, TxSwitch);
    	Regvalue = Regvalue | 0x80000000;
    	macMT7530WriteReg2(TxRegAddrl, Regvalue, TxSwitch);

    	//Scan
    	for (RxChannel = 0 ; RxChannel <= 4 ; RxChannel++) {
    	    switch(RxChannel){
    	        case 0:
    	            RxRegAddr = 0x7A10;
    				TxRegAddr2 = 0x7A50;
    	            break;
    	        case 1:
    	            RxRegAddr = 0x7A18;
    				TxRegAddr2 = 0x7A58;
    	            break;	            
    	        case 2:
    	            RxRegAddr = 0x7A20;
    				TxRegAddr2 = 0x7A60;
    	            break;
    	        case 3:
    	            RxRegAddr = 0x7A28;
    				TxRegAddr2 = 0x7A68;
    	            break;
    	        default:
    	            RxRegAddr = 0x7A30;
    				TxRegAddr2 = 0x7A70;
    	            break;
    	    }
            state = HEAD;
            for (DAC = 1 ; DAC <= 127 ; DAC++) {
                ErrCnt = 0;
    			Regvalue = macMT7530ReadReg2(TxRegAddr2,TxSwitch);
    			Regvalue = (Regvalue & 0xFFFFFF00) | 0x55;
    			macMT7530WriteReg2(TxRegAddr2, Regvalue, TxSwitch);
    			
    			Regvalue = macMT7530ReadReg2(RxRegAddr, RxSwitch);
    			Regvalue = Regvalue & 0xFFFFFF80;
    			Regvalue = Regvalue | DAC;
    			macMT7530WriteReg2(RxRegAddr, Regvalue, RxSwitch);
    			ErrCnt = ErrChk(RxRegAddr, RxSwitch);
    			Val = (macMT7530ReadReg2(RxRegAddr, RxSwitch) & 0x00FF0000) >> 16;
    			printk("RxChannel=%8x, DAC=%8x, Val=%8x, ErrCnt=%8x \n", 
    			    RxChannel, DAC, Val, ErrCnt);
    			
			    switch(state){
			        case HEAD:
			            if((Val == 0x55) && (ErrCnt == 0)){
			                delay_windows[RxChannel][0]=DAC;
			                state = TAIL;
			            }
			            break;
			        case TAIL:
			            if((Val == 0x55) && (ErrCnt == 0)){
			                delay_windows[RxChannel][1]=DAC;			                
			            }else{
			                state = FINISH;
			            }
			            break;
			        default:
			            break;
                }
    		}
    		Regvalue = macMT7530ReadReg2(RxRegAddr, RxSwitch);
            Regvalue = Regvalue & 0xFFFFFF80;
    		if((state == FINISH) && (delay_windows[RxChannel][0]!=delay_windows[RxChannel][1])){
                delay_tap = (delay_windows[RxChannel][0] + delay_windows[RxChannel][1]) >> 1;
                printk("RxChannel=%8x, delay windows 0x%2X ~ 0x%2X \n",
                    RxChannel, delay_windows[RxChannel][0], delay_windows[RxChannel][1]);
    		}else{
    		    delay_tap = 0;
    		    printk("RxChannel=%8x, delay windows fail, set to default \n",RxChannel);
    		}
            Regvalue = Regvalue | delay_tap;
            macMT7530WriteReg2(RxRegAddr, Regvalue, RxSwitch);
    	}
        
        //Tx training mode disable
    	TxRegAddrl = 0x7A40;
    	Regvalue = macMT7530ReadReg2(TxRegAddrl, TxSwitch);
    	Regvalue = Regvalue & ~0x80000000;
    	macMT7530WriteReg2(TxRegAddrl, Regvalue, TxSwitch);
    }
    
	return 0;
	
}

#include "tcswitch.h"
/* map to switch ports */
extern char lan_port_map[];

/************************************************************************
    Funtcion:	macMT7530LanPortMap2Switch
    Description:	changes Lan port to switch port
    Input:	Lan port(port for soft)
    Return:	switch port(port for hardware)
************************************************************************/
inline u32 macMT7530LanPortMap2Switch(u32 lanPortId){
	int switch_port = 0 ;
#if defined(TCSUPPORT_NP_CMCC)
	if(100 == lanPortId)
		return wan_port_id;
#endif
	if (lanPortId < MAX_ETH_ITF_NUM){
		switch_port = lan_port_map[lanPortId];
		if ((0 <= switch_port) && (switch_port < (RT63365_SWIC_PORTNUM-1))){
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
			if(switch_port == extend_switch_port){
				switch_port = lan_port_map_ext[lanPortId];
				switch_port |= EXT_SWITCH_FLAG;
			}
#endif
			return switch_port;
		}

	}
	return lanPortId;
}
EXPORT_SYMBOL(macMT7530LanPortMap2Switch);

/************************************************************************
    Funtcion:	macMT7530LanPortMap2SwitchForMulti
    Description:	changes Lan port to switch port for multicast
    Input:       lan port(port for soft)
    Return:	switch port(port for hardware)
************************************************************************/
inline u32 macMT7530LanPortMap2SwitchForMulti(u32 lanPortId){
	int switch_port = 0 ;
	if (lanPortId < MAX_ETH_ITF_NUM){
		switch_port = lan_port_map[lanPortId];
		if ((0 <= switch_port) && (switch_port < (RT63365_SWIC_PORTNUM-1))){
			return switch_port;
		}
	}
	return lanPortId;
}
EXPORT_SYMBOL(macMT7530LanPortMap2SwitchForMulti);

extern char switch_port_map[ ];
/************************************************************************
    Funtcion:	macMT7530SwitchMap2LanPort
    Description:	changes switch port to lan port
    Input:	switch port(port for hardware)
    Return:	Lan port(port for soft)
************************************************************************/
inline u32 macMT7530SwitchMap2LanPort(u32 switchPortId){
	int LanPort ;
	if (switchPortId < RT63365_SWIC_PORTNUM-1){
		LanPort = switch_port_map[switchPortId];
		if (0 <= LanPort &&  LanPort < MAX_ETH_ITF_NUM)
			return LanPort;
	}
	return switchPortId;
}
EXPORT_SYMBOL(macMT7530SwitchMap2LanPort);


int getPortByMask(u8 mask, u8 *port)
{
	u8 portIdx = 0, idx = 0;

	if ( !port )
		return -1;

	while ( !(mask & (1 << portIdx)) )
	{
		portIdx ++;
		if ( portIdx >= 8 )
			break;
	}
	if ( portIdx >= 8 )
		return -1;

	*port = portIdx;
	return 0;
}

u8 getPortNameIdxByPortid(int portid){
    u8 lanport = 0xff;

    if(portid == RT63365_SWIC_PORTNUM - 1)
        return CPU_PORT;

    if(portid == wan_port_id)
        return WAN_PORT;
        
    if (0 <= portid && portid < RT63365_SWIC_PORTNUM-1){
        lanport = switch_port_map[portid];
    }

    if (lanport >=  MAX_ETH_ITF_NUM)
        return SWITCH_OTHER_PORT;
    
    return lanport;
}

u8 getPortRevertFWC(u8 in_port)
{
	u8 portIdx = in_port, idx = 0;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_ENV)
	if ( 0xff == in_port )
		return 0;

	for ( idx = 0; idx <  MAX_ETH_ITF_NUM; idx ++ )
	{
		if ( -1 != lan_port_map[idx]
			&& lan_port_map[idx] == in_port )
			portIdx = (++idx);
	}
#endif/*TCSUPPORT_COMPILE*/

	return portIdx;
}


int Eth_General_API(uint32 api_type, uint32 api_value)
{
    switch(api_type){
        case HAL_API_EXT:
            hal_api_ext = api_value;
            break;
    }
    return 0;
}


/*___________________________________________________________________
**      function name: rgmii_setting rgmii_setting_display rgmii_disable
**      descriptions:
**         7513G.7526G chipset, soc & ext switch support both tgmii & rgmii interface
**          for 2.9G + 1G bandwidth
**      parameters:
**        
**
**      global:
**
**      return:
**         0:succuces
**         -1:fail
**
**      call:
**
**      revision:
**      1. Jason.Gong 2016/02/15
**_______________________________________________________________
*/
void rgmii_port_force_link(uint8 mode){
	uint32 reg,reg_val;
    if(mode == ENABLE){
        reg_val = read_reg_word(GSW_PCR(5));
        reg_val = (reg_val & 0xFF00FFFF) | (0xFE << 16);    /*soc port5 !=> port0*/
        write_reg_word(GSW_PCR(5), reg_val);

        reg_val = read_reg_word(GSW_PCR(0));
        reg_val = (reg_val & 0xFF00FFFF) | (0xDF << 16);    /*soc port0 !=> port5*/
        write_reg_word(GSW_PCR(0), reg_val);

        reg_val = gswPbusRead(EXT_GSW_PCR(5));
        reg_val = (reg_val & 0xFF00FFFF) | (0xBF << 16);    /*ext port5 !=> port6*/
        gswPbusWrite(EXT_GSW_PCR(5), reg_val);

        reg_val = gswPbusRead(EXT_GSW_PCR(6));
        reg_val = (reg_val & 0xFF00FFFF) | (0xDF << 16);    /*ext port6 !=> port5*/
        gswPbusWrite(EXT_GSW_PCR(6), reg_val);
        /* TXEB Disable and Link up */
        reg = (IPG_CFG_64BITS<<IPG_CFG_PN_SHIFT) | MAC_MODE_PN | FORCE_MODE_PN | 
            MAC_TX_EN_PN | MAC_RX_EN_PN | BKOFF_EN_PN | BACKPR_EN_PN | 
            (PN_SPEED_1000M<<FORCE_SPD_PN_SHIFT) | FORCE_DPX_PN | FORCE_LNK_PN;
    }else{
        reg = (IPG_CFG_SHORT<<IPG_CFG_PN_SHIFT) | MAC_MODE_PN  | 
            MAC_TX_EN_PN | MAC_RX_EN_PN | BKOFF_EN_PN | BACKPR_EN_PN | 
            ENABLE_RX_FC_PN | ENABLE_TX_FC_PN;
    }
    write_reg_word(GSW_PMCR(0), reg);
    gswPbusWrite(0x3500, reg);


}

int rgmii_setting(uint8 wan_type,uint8 interface)
{
    if(interface_map[wan_type] == interface){
        return 0;
    }
    interface_map[wan_type] = interface;
    PPE_API_CLEAN_FOE_TABLE();
    macMT7530CleanExtMacTbl();
    return 0;
}

int rgmii_setting_display(void)
{
    uint index=0;
    for(;index<TRAFFIC_TYPE_MAX;index++){
        switch(index){
            case 0:
                printk("Wan\t  ");
                break;
            case 1:
                printk("Wifi 2.4G ");
                break;
            case 2:
                printk("Wifi 5G\t  ");
                break;
        }
        printk("traffic : %s \n",interface_map[index]?"RGMII":"TRGMII");
    }
    return 0;
}

static int rgmii_acl_enable(u8 enable)
{
	u32 reg, value;
	reg = GSW_PCR(6);
	value = read_reg_word(reg);
	value = (enable) ? (value | (1 << 10)) : (value & ~(1 << 10));	// ACL_EN 0x2n04[10]
	write_reg_word(reg, value);

	return (0);
}

int rgmii_disable(uint8 mode)
{
    uint index=0;
    u32 value = 0;
    u32 reg = 0;
    reg = GSW_PVC(5);
    value = switch_reg_read(reg);
    if(mode == ENABLE){
        rgmii_acl_enable(ENABLE);
        rgmii_port_force_link(ENABLE);
        PPE_API_CLEAN_FOE_TABLE();
        macMT7530CleanExtMacTbl();
        value |= 1<<PORT_STAG_SHIFT;
        switch_reg_write(reg, value);// user port & enable stag

    }else{
        for(;index<TRAFFIC_TYPE_MAX;index++){
            interface_map[index] = TRGMII;
        }        
        //disable_rgmii_acl();
        rgmii_acl_enable(DISABLE);
        rgmii_port_force_link(DISABLE);
        PPE_API_CLEAN_FOE_TABLE();
        macMT7530CleanExtMacTbl();
        value &=~(1<<PORT_STAG_SHIFT);
        switch_reg_write(reg, value);// user port & disable stag

    }

    return 0;
}

int macMT7530SetPortLoopBackRemote(u8 port, u8 enable)
{
	u32 phy_reg, val, phy_addr;
	const 	u16 page_reg=31;
	u16	page_value = 0x0001;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port > 4) return (-1);

	if (isFPGA) return (0);

	phy_reg = 0x1a;

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	macMT7530PHYWriteReg(phy_addr, page_reg, page_value);
	val = (macMT7530PHYReadReg(phy_addr, phy_reg) & ~(1<<15)) | (enable ? (1<<15) : 0);
	macMT7530PHYWriteReg(phy_addr, page_reg, page_value);
	macMT7530PHYWriteReg(phy_addr, phy_reg, val);

	return 0;
}

int macMT7530GetPortLoopBackRemote(u8 port, u8 *enable)
{
	u32 phy_reg, val, phy_addr;
	const 	u16 page_reg=31;
	u16	page_value = 0x0001;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port > 4) return (-1);

	if (isFPGA) return (0);

	phy_reg = 0x1a;

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	macMT7530PHYWriteReg(phy_addr, page_reg, page_value);
	val = macMT7530PHYReadReg(phy_addr, phy_reg);

	*enable = (val&(1<<15))?1:0;

	return 0;
}

int macMT7530SetPortMode(u8 port, u8 mode)
{
	u32 value, value1, phy_addr, phy_reg, reg;
	u8 linkstate;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	value1 = macMT7530PHYReadReg(phy_addr, phy_reg);
	value1 &= 0xcebf;
	
	if(1 == mode)
		value = (BMCR_ANENABLE | BMCR_ANRESTART);
	else if(2 == mode)
		value = (BMCR_SPEED100 | BMCR_FULLDPLX | BMCR_ANRESTART);
	else if(3 == mode)
		value = (BMCR_SPEED100 | BMCR_ANRESTART);
	else if(4 == mode)
		value = (BMCR_FULLDPLX | BMCR_ANRESTART);
	else
		value = BMCR_ANRESTART;

	if( (value1&value) == value)
		return 0;
	
	value |= value1;
	macMT7530PHYWriteReg(phy_addr, phy_reg, value);

	macMT7530PortReset(extsw_flag ? (port | EXT_SWITCH_FLAG) : port);

	return 0;

}
EXPORT_SYMBOL(macMT7530SetPortMode);

int macMT7530GetPortMode(u8 port, u8 *mode)
{
	u32 value, phy_addr, phy_reg;
	int bmcr;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMCR;
	bmcr = macMT7530PHYReadReg(phy_addr, phy_reg);
	if (bmcr & BMCR_ANENABLE)
	{
		*mode = 1;
		return 0;
	}

	if(bmcr & BMCR_SPEED100)
	{
		if(bmcr & BMCR_FULLDPLX)
			*mode = 2;
		else
			*mode = 3;
	}
	else
	{
		if(bmcr & BMCR_FULLDPLX)
			*mode = 4;
		else
			*mode = 5;
	}	

	return 0;

}
EXPORT_SYMBOL(macMT7530GetPortMode);

int macMT7530SetPortLinkState(u8 port,u8 linkstate)
{
	u32 reg = 0,val = 0;
	u8 force_bit = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) 
		return -1;

	reg = 0x3000 + (port*0x100);
	val = macMultiMT7530ReadReg(reg, extsw_flag);

	force_bit = (val>>15)&1;

	if(!force_bit)
	{
		val |= 1<<15;
		val &= ~1;
		val |= linkstate;
		macMultiMT7530WriteReg(reg,val, extsw_flag);
		val &=~(1<<15);
	}
	else
	{
		val &= ~1;
		val |= linkstate;
	}

	macMultiMT7530WriteReg(reg,val, extsw_flag);
		
	return 0;
}

int macMT7530GetPortLinkState(u8 port, u8 *linkstate, u8 *speedmode)
{
	u32 reg, val;
	u8 speed, full;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	reg = 0x3008 + (port*0x100);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	
	*linkstate = (val & (1<<0))?1:0;

	if(*linkstate){
		full = (val & (1<<1));
		speed = (val & (0x3<<2)) >> 2;
		if(2 == speed)
		{
			if(full)
				*speedmode = 1;	/* 1000M FULL */
			else
				*speedmode = 6;	/* 1000M HALF */
		}
		else if(1 == speed)
		{
			if(full)
				*speedmode = 2;	/* 100M FULL */
			else
				*speedmode = 3;	/* 100M HALF */
		}
		else if(0 == speed)
		{
			if(full)
				*speedmode = 4;	/* 10M FULL */
			else
				*speedmode = 5;	/* 10M HALF */
		}	
		else
			return -1;
	}

	return 0;
}
EXPORT_SYMBOL(macMT7530GetPortLinkState);

int macMT7530SetPortFlowControlGlobal(u8 enable)
{
	u32 reg, val;
	
	reg = 0x1fe0;
	val = macMT7530ReadReg(reg);
	
	if(enable)
		val |= 1<<31;
	else
		val &= ~(1<<31);

	macMT7530WriteReg(reg, val);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        val = macMultiMT7530ReadReg(reg, 1);
        if(enable)
            val |= 1<<31;
        else
            val &= ~(1<<31);
        macMultiMT7530WriteReg(reg, val, 1);
    }
#endif

	return 0;
}

int macMT7530GetPortFlowControlGlobal(void)
{
	u32 reg, val;
	u8 ret;
	
	reg = 0x1fe0;
	val = macMT7530ReadReg(reg);
	ret = (val&(1<<31))?1:0;

	return ret;
}

int macMT7530Setlan2lanClear(void)
{
	u32 reg, val;
	int i;

	reg = 0x10;
	val = macMT7530ReadReg(reg) & (~(1<<3));
	val &= 0xFFFFFFF8;
	macMT7530WriteReg(reg, val);
	
	for(i=0;i<5;i++)
	{
		reg = 0x2004 + (0x100*i);
		val = macMT7530ReadReg(reg);
		val &= ~(3<<8);
		macMT7530WriteReg(reg, val);
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = 0x10;
        val = macMultiMT7530ReadReg(reg, 1) & (~(1<<3));
        val &= 0xFFFFFFF8;
        macMultiMT7530WriteReg(reg, val, 1);
        
        for(i=0;i<5;i++)
        {
            reg = 0x2004 + (0x100*i);
            val = macMultiMT7530ReadReg(reg, 1);
            val &= ~(3<<8);
            macMultiMT7530WriteReg(reg, val, 1);
        }
    }
#endif
	
	return 0;
}

int macMT7530SetVlanActive(uint8 enable)
{
    uint8 i = 0;
    uint32 value = 0;
    uint32 reg = 0;;

    // active
    if(1 == enable)
    {
        //port 0 to 4, forward broadcast/multicast/unicast only to cpu if lookup miss.
        reg = GSW_BASE + 0x0010;
        switch_reg_write(reg, 0x404040e0);
        
        //forward every arp packet to cpu port
        reg = GSW_BASE + 0x0020;
        value = switch_reg_read(reg);
        value &= ~((1<<18) | (1<<17) | (1<<16) | (1<<2) | (1<<1) | (1<<0));
        value |=  ((1<<18) | (1<<17) | (1<<2) | (1<<1));
        switch_reg_write(reg, value);
        
        // vlan mismatch forword to cpu port
        for (i = 0; i <= 6; i++) 
        {
            reg = GSW_BASE + 0x2004 + i*0x100;
            switch_reg_write(reg, 0xff0003);
        }

        // ether wan config
        if (wan_port_id >=0 && wan_port_id < 6)
        {
            // user acl for ether wan
            //use port matrix + ACL to make etherwan packet only sent to CPU port
            switch_reg_write(GSW_PCR(wan_port_id), 0x00f00400);
            
            //disable etherwan port mac learning
            switch_reg_write(GSW_PSC(wan_port_id), 0xfff10);
            
            switch_reg_write(GSW_BASE + 0x94, 0x0);    
            switch_reg_write(GSW_BASE + 0x98, 0x80000 | ((1<<wan_port_id)<<8));
            switch_reg_write(GSW_BASE + 0x90, 0x80005000);
            
            switch_reg_write(GSW_BASE + 0x94, 0x1);    
            switch_reg_write(GSW_BASE + 0x98, 0x0);
            switch_reg_write(GSW_BASE + 0x90, 0x80009000);
            
            switch_reg_write(GSW_BASE + 0x94, 0x4080);    
            switch_reg_write(GSW_BASE + 0x98, 0x0);
            switch_reg_write(GSW_BASE + 0x90, 0x8000B000);

        }
        
        for (i = 0; i <= 5; i++)
        {
            #ifdef TCSUPPORT_CPU_EN7580
                switch_reg_write(GSW_PVC(i), 0x810081C0); // EG_TAG consistent
            #else
                switch_reg_write(GSW_PVC(i), 0x81008100); // EG_TAG consistent
            #endif
        }
        
        reg = GSW_BASE + 0x2510;
        switch_reg_write(reg, 0x81008020);// user port & enable stag
        
        reg = GSW_BASE + 0x2610;
        value = 0x81008020;
        if(use_soc_lan & use_ext_switch)
        {   // only ext switch support soc lan/wan port
            value |= 0x800;// PT option for soc lan or wan port 
        }
        switch_reg_write(reg, value);// user port & enable stag     

        for (i = 0; i <= 6; i++) 
        {   // set vlan group
            value = 0x10000 + (i+1);
            switch_reg_write(GSW_PPBV1(i), value);
        }
        
        if (use_ext_switch == 1)
        {
            //config inner switch
            for (i = 0; i <= 6; i++)
            {
                reg = GSW_BASE + 0x2010 + i*0x100;
                //set all port to transparent mode
                write_reg_word(reg, 0x810081c0);
            }
            
            if(use_soc_lan)
            {
                reg = GSW_BASE + 0x2510;
                write_reg_word(reg, 0x81008120);//port5 user port & enable stag & consistent mode

                reg = GSW_BASE + 0x2010;
                write_reg_word(reg, 0x81008100);//port0 user port
                
                reg = GSW_BASE + 0x2610;
                write_reg_word(reg, 0x81008120);//port6 user port & enable stag & consistent mode

                reg = GSW_BASE + 0x2604;
                write_reg_word(reg, 0xff0400);//port6 ACL_EN
                
                reg = GSW_BASE + 0x2504;
                write_reg_word(reg, 0x400000);//port5 force to cpu port6
                
                reg = GSW_BASE + 0x2004;
                write_reg_word(reg, 0x400000);//port0 force to cpu port6
            }
            
            for (i = 0; i <= 6; i++)
            {
                //disable mac learning
                write_reg_word(GSW_PSC(i), 0xfff10);
            }

            //disable inner switch pkt len check
            reg = GSW_BASE + 0x000c;
            value = read_reg_word(reg);
            value &= ~(1 << 4);
            write_reg_word(reg, value);
        }
        vport_enable = 1;           
    }
    else //deactive
    {
        /* Disable VLAN Function */
        // port 0 to 4, forword broadcast/multicast/unicast to every port.
        reg = GSW_BASE + 0x0010;
        switch_reg_write(reg, 0xffffffe0);
        // forward every arp packet
        reg = GSW_BASE + 0x0020;
        value = switch_reg_read(reg);
        value &= ~((1<<18) | (1<<17) | (1<<16) | (1<<2) | (1<<1) | (1<<0));
        switch_reg_write(reg, value);
        
        // port matrix mode 
        for (i = 0; i <= 6; i++) 
        {
            switch_reg_write(GSW_PCR(i), 0xff0000);
        }

        // set EG_TAG = CONSISTENT
        for (i = 0; i <= 5; i++)
        {
            switch_reg_write(GSW_PVC(i), 0x81008100);
        }
        
        // set CPU Port EG_TAG=CONSISTENT & enable SP_TAG
        reg = GSW_BASE + 0x2610;
        switch_reg_write(reg, 0x81008120);

        // set PVID as 1
        for (i = 0; i <= 6; i++) 
        {
            value = 0x10000 + (1);
            switch_reg_write(GSW_PPBV1(i), value);
        }
        //enable mac learning
        for (i = 0; i <= 5; i++)
        {
            switch_reg_write(GSW_PSC(i), 0xfff00);
        }   
        if (use_ext_switch == 1)
        {
            for (i = 0; i <= 6; i++)
            {
                //set all port to transparent mode
                write_reg_word(GSW_PVC(i), 0x810081c0);
            }
            for (i = 0; i <= 6; i++)
            {
                //disable mac learning
                write_reg_word(GSW_PSC(i), 0xfff10);
            }
        }
        vport_enable = 0;       
    }

    /* switch clear */
    reg = GSW_BASE + 0x80;
    switch_reg_write(reg, 0x8002);
    msleep(5);
    value = switch_reg_read(reg);
    if(use_ext_switch == 1)
    {
        write_reg_word(reg, 0x8002);
        msleep(5);
        value = read_reg_word(reg);
    }

    return 0;
}

int macMT7530Setqosparam(u8 port, mt7530_switch_qosparam param)
{	
    u8 queue = 0;
	u32 value = 0;
	u32 switch_reg = 0; 
	u8 queue_range = 4;
	u8 is_etght_queue = param.eight_queue;

	if(param.eight_queue){
		queue_range = 8;
	}
	else{ 
		queue_range = 4;
		}
    int i = 0;
    int qos_wrr_info[8] = {0};
    u8 wrr_queue_cnt = 0;
    u8 qos_mode = 0;

	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
    qos_mode = param.qosmode;
    wrr_queue_cnt = param.wrr_queue_cnt;

    if(port>6)
    {
        printk("macMT7530Setqosparam(): wrong parameters, portid range 0~6. \r\n");
        return -1;
    }
    
    for(i=0; i<8; i++)
    {
        qos_wrr_info[i] = param.wi[i];
    }

    //SP mode
    if(0 == qos_mode) 
    { 
    
		for( queue = 0; queue < queue_range;queue++)
        {   
        	if((param.eight_queue)){
				switch_reg = 0x1000 + queue*0x8 + port * 0x100;
	    		value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
				value&= 0;
	    		value|= (1<<31);
	    		macMultiMT7530WriteReg(switch_reg, value, extsw_flag);

	    		//enable MAX_SP_WFQ_Qx_Pn with SP mode		
	    		switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
	    		macMultiMT7530WriteReg(switch_reg, 0, extsw_flag);
			}else{
	    		//Port n Queue x min.rate control enable		
	    		switch_reg = 0x1000 + queue*0x8 + port * 0x100;
	    		value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
	    		value|= (1<<15);
	    		value&=(0xfffff080);
	    		macMultiMT7530WriteReg(switch_reg, value, extsw_flag);

	    		//enable MAX_SP_WFQ_Qx_Pn with SP mode		
	    		switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
	    		value=macMultiMT7530ReadReg(switch_reg, extsw_flag);
	    		value|= (1<<31);
	    		macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
			}
		}		
	}
    //WRR mode
	else if(1 == qos_mode) 
    {  
    
		if((param.eight_queue)){
			for (queue=0; queue<queue_range; queue++)
			{
				switch_reg = 0x1000 + queue*0x8 + port * 0x100;
	    		value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
				value&= 0;
	    		value|= (1<<31);
				value|= (1<<15);
	    		macMultiMT7530WriteReg(switch_reg, value, extsw_flag);

	    		//enable MAX_SP_WFQ_Qx_Pn with SP mode		
	    		switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
				value&= 0;

				value|= (((qos_wrr_info[queue_range-1-queue]-1)&0xf)<<24);
	    		macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
			}
		}else{
			for (queue=0; queue<queue_range; queue++)
		    {
				//Port n Queue x min.rate control enable		
				switch_reg = 0x1000 + queue*0x8 + port * 0x100;
				value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
				value &= (0xfffff080);
				value |= (1<<15);
				macMultiMT7530WriteReg(switch_reg, value, extsw_flag);

				//enable MAX_SP_WFQ_Qx_Pn with RR mode		
				switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
				value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
				value &= (~(1<<31));
				macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
			}
			//weight
			for( queue = 0; queue < queue_range;queue++)
		    {
				switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
				value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
				value &= (~(0xf<<24));
				value|= (((qos_wrr_info[queue_range-1-queue]-1)&0xf)<<24);
				macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
			}
		}
	}
    //PQ_WRR mode
    else if(2 == qos_mode) 
        { 
            if(wrr_queue_cnt<1 || wrr_queue_cnt>(queue_range - 1))
            {
                	printk("==>doQoSDiscHWPQ_WRR(): wrong parameters. \r\n"
                		   "		usage: \r\n"
                		   "		qoscmd discpline HWPQ_WRR wrr_queue_cnt w3 w2 w1, wrr queue cnt support 1-%d\r\n", (queue_range - 1));
                    return -1;
            }

			if((param.eight_queue)){
				//1. SP mode
				for(i=0; i<(queue_range-wrr_queue_cnt); i++)
				{
					queue = (queue_range - 1)-i;
					//Port n Queue x min.rate control enable		
					switch_reg = 0x1000 + queue*0x8 + port * 0x100;
					value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
					value&= 0;
	    			value|= (1<<31);
					macMultiMT7530WriteReg(switch_reg, value, extsw_flag);

					//enable MAX_SP_WFQ_Qx_Pn with SP mode		
					switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
					value=macMultiMT7530ReadReg(switch_reg, extsw_flag);
					value&= 0;
					macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
				}	

				//2.1 WRR mode
				for (queue=0; queue<wrr_queue_cnt; queue++)
				{
					//Port n Queue x min.rate control enable		
					switch_reg = 0x1000 + queue*0x8 + port * 0x100;
					value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
					value&= 0;
		    		value|= (1<<31);
					value|= (1<<15);
					macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
				
					//enable MAX_SP_WFQ_Qx_Pn with WRR mode 	 
					switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;		
					value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
					value &= (~(1<<31));
					value &= (~(0xf<<24));
	                value|= (((qos_wrr_info[wrr_queue_cnt-1-queue]-1)&0xf)<<24);
					macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
				}


			}else{
	            //1. SP mode
	            for(i=0; i<(queue_range-wrr_queue_cnt); i++)
	            {
	                queue = (queue_range - 1)-i;
	                //Port n Queue x min.rate control enable		
	                switch_reg = 0x1000 + queue*0x8 + port * 0x100;
	                value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
	                value|= (1<<15);
	                value&=(0xfffff080);
	                macMultiMT7530WriteReg(switch_reg, value, extsw_flag);

	                //enable MAX_SP_WFQ_Qx_Pn with SP mode		
	                switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
	                value=macMultiMT7530ReadReg(switch_reg, extsw_flag);
	                value|= (1<<31);
	                macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
	            }	

	            //2.1 WRR mode
	            for (queue=0; queue<wrr_queue_cnt; queue++)
	            {
	                //Port n Queue x min.rate control enable        
	                switch_reg = 0x1000 + queue*0x8 + port * 0x100;
	                value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
	                value &= (0xfffff080);
	                value |= (1<<15);
	                macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
	            
	                //enable MAX_SP_WFQ_Qx_Pn with WRR mode      
	                switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;        
	                value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
	                value &= (~(1<<31));
	                macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
	            }
	            //2.2 WRR weight
	            for( queue=0; queue<wrr_queue_cnt; queue++)
	            {
	                switch_reg = 0x1000 + queue*0x8 + 0x4+ port * 0x100;
	                value = macMultiMT7530ReadReg(switch_reg, extsw_flag);
	                value &= (~(0xf<<24));
	                value|= (((qos_wrr_info[wrr_queue_cnt-1-queue]-1)&0xf)<<24);
	                macMultiMT7530WriteReg(switch_reg, value, extsw_flag);
	            }    
			}
    }

    return 0;

}


int macMT7530Setlan2lan(mt7530_switch_lan2lan para)
{
	u32 reg, val;
	u8 dstport, srcport;
	u32 extsw_flag_dst = 0, extsw_flag_src = 0;

	dstport = macMT7530LanPortMap2Switch(para.dstport);
	srcport = macMT7530LanPortMap2Switch(para.srcport);

    if(dstport & EXT_SWITCH_FLAG)
    {
        extsw_flag_dst = 1;
        dstport = dstport & SWITCH_PORT_MASK;
    }
    
    if(srcport & EXT_SWITCH_FLAG)
    {
        extsw_flag_src = 1;
        srcport = srcport & SWITCH_PORT_MASK;
    }

    if (extsw_flag_dst != extsw_flag_src)
    {
		printk("dst port and src port belong to different switch!\n");
        return -1;
    }

	if(dstport>=MT7530_TOTAL_PORTS)
	{
		printk("dest port is %d, error\n", dstport);
		return -1;
	}
	
	if(srcport>=MT7530_TOTAL_PORTS){
		printk("src port is %d, not valid\n", srcport);
		return -1;
	}
	if(srcport == dstport){
		printk("src port %d is the same with dest port, not support!\n", srcport);
		return -1;
	}

	reg = 0x2004 + (0x100*srcport);
	val = macMultiMT7530ReadReg(reg, extsw_flag_dst);
	if(para.rx_enable)
		val |= 1<<8;
	if(para.tx_enable)
		val |= 1<<9;
	macMultiMT7530WriteReg(reg, val, extsw_flag_dst);

	reg = 0x10;
	val = macMultiMT7530ReadReg(reg, extsw_flag_dst);
	val |= 1<<3;   /* Enable mirror function */
	val &= 0xFFFFFFF8;
	val |= dstport;
	macMultiMT7530WriteReg(reg, val, extsw_flag_dst);
	
	return 0;
}

int macMT7530Getlan2lan(mt7530_switch_lan2lan_info *para)
{
	u32 reg, val,dstport;
	u8 i;

	memset(para, 0x0, sizeof(mt7530_switch_lan2lan_info));
	reg = 0x10;
	val = macMT7530ReadReg(reg);

	para->enable = (u8)((val & (1<<3))?1:0);
	dstport = (val&0x07);
	para->dstport = macMT7530SwitchMap2LanPort(dstport);

	for(i=0;i<MT7530_TOTAL_PORTS;i++)
	{
		reg = 0x2004 + (0x100*i);
		val = macMT7530ReadReg(reg);
		val &= 3<<8;
		if(val)
		{
			if(para->srcportnum>=7)
				return 0;
			para->srcport[para->srcportnum].port= macMT7530SwitchMap2LanPort(i);
			if(val&(1<<8))
			{
				para->srcport[para->srcportnum].rx_enable = 1;
			}
			if(val&(1<<9))
			{
				para->srcport[para->srcportnum].tx_enable = 1;
			}
			para->srcportnum++;
		}
	}
	
	return 0;
}

int macMT7530Getportcnt(u8 port, mt7530_switch_api_cnt *para)
{
	u32 reg, val;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*tx para*/
	reg = MIB_ID_TX_UCAST_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxUniPktsCnt = val;
	reg = MIB_ID_TX_MCAST_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxMultiPktsCnt = val;
	reg = MIB_ID_TX_BCAST_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxBroadPktsCnt = val;
	para->TxPktsCnt = para->TxUniPktsCnt + para->TxMultiPktsCnt + para->TxBroadPktsCnt;
	para->TxNonUniPktsCnt = para->TxMultiPktsCnt + para->TxBroadPktsCnt ; 
    
	reg = MIB_ID_TX_OCT_CNT_L + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxBytesCnt_Lo = val;
	reg = MIB_ID_TX_OCT_CNT_H+ (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxBytesCnt_Hi = val;

	para->TxUnderSizePktsCnt = 0;/*tx:no undersize 64 packet*/
	reg = MIB_ID_TX_PKT_64_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx64BytePktsCnt = val;
	reg = MIB_ID_TX_PKT_65TO127_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx65_127BytePktsCnt = val;
	reg = MIB_ID_TX_PKT_128TO255_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx128_255BytePktsCnt = val;
	reg = MIB_ID_TX_PKT_256TO511_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx256_511BytePktsCnt = val;
	reg = MIB_ID_TX_PKT_512TO1023_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx512_1023BytePktsCnt = val;
	reg = MIB_ID_TX_PKT_1024TOMAX_CNT + (0x100 * port);/*1024~1518*/
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx1024_MAXBytePktsCnt = val;
	reg = MIB_ID_TX_PKT_1024TO1518_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx1024_1518BytePktsCnt = val;
	reg = MIB_ID_TX_PKT_1519TOMAX_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Tx1519_MAXBytePktsCnt = val;
	para->TxOverSizePktsCnt = 0 ;/*tx:no overrsize 1518 packet*/

	reg = MIB_ID_TX_DROP_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxDropFramesCnt = val;
	reg = MIB_ID_TX_PAUSE_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxPauseFramesCnt = val;
	reg = MIB_ID_TX_CRC_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxCRCFramesCnt = val;
	reg = MIB_ID_TX_SCOL_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxSingleCollsionEvent = val;
	reg = MIB_ID_TX_MCOL_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxMultiCollsionEvent = val;

	reg = MIB_ID_TX_LCOL_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxLateCollisionEvent = val;
	reg = MIB_ID_TX_XCOL_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxExcessCollisionEvent = val;
	reg = MIB_ID_TX_DEFER_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxDeferredEvent = val;
	reg = MIB_ID_TX_DROP_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->TxOverFlowFrameCnt = val;
	para->TxInternalMacErrorCnt = 0;    /* confirmed always return 0*/
	para->TXCarrierSenseErrorCnt = 0;   /* confirmed always return 0 */

	/*rx para*/
	reg = MIB_ID_RX_UCAST_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxUniPktsCnt = val;
	reg = MIB_ID_RX_MCAST_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxMultiPktsCnt = val;
	reg = MIB_ID_RX_BCAST_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxBroadPktsCnt = val;
	para->RxPktsCnt = para->RxUniPktsCnt + para->RxMultiPktsCnt + para->RxBroadPktsCnt;
	para->RxNonUniPktsCnt = para->RxMultiPktsCnt + para->RxBroadPktsCnt;

	reg = MIB_ID_RX_OCT_CNT_L + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxBytesCnt_Lo = val;
	reg = MIB_ID_RX_OCT_CNT_H+ (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxBytesCnt_Hi = val;

	reg = MIB_ID_RX_UNDERSIZE_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxUnderSizePktsCnt = val;
	reg = MIB_ID_RX_PKT_64_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx64BytePktsCnt = val;
	reg = MIB_ID_RX_PKT_65TO127_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx65_127BytePktsCnt = val;
	reg = MIB_ID_RX_PKT_128TO255_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx128_255BytePktsCnt = val;
	reg = MIB_ID_RX_PKT_256TO511_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx256_511BytePktsCnt = val;
	reg = MIB_ID_RX_PKT_512TO1023_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx512_1023BytePktsCnt = val;
	reg = MIB_ID_RX_PKT_1024TOMAX_CNT + (0x100 * port);/*1024~1518*/
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx1024_MAXBytePktsCnt = val;
	reg = MIB_ID_RX_PKT_1024TO1518_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx1024_1518BytePktsCnt = val;
	reg = MIB_ID_RX_PKT_1519TOMAX_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->Rx1519_MAXBytePktsCnt= val;
	reg = MIB_ID_RX_OVERSIZE_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxOverSizePktsCnt = val;

	reg = MIB_ID_RX_DROP_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxDropFramesCnt = val ;
	reg = MIB_ID_RX_PAUSE_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxPauseFramesCnt = val ;
	reg = MIB_ID_RX_FCS_ERR_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxCRCFramesCnt = val ;
		
	reg = MIB_ID_RX_DROP_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxOverFlowFrameCnt = val;
	reg = MIB_ID_RX_FRAG_ERR_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxFragmentErrorCnt = val;
	reg = MIB_ID_RX_JABB_ERR_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxJabberErrorCnt = val;
	reg = MIB_ID_RX_ALIGN_ERR_CNT + (0x100 * port);
	val = macMultiMT7530ReadReg(reg, extsw_flag);
	para->RxAlignmentErrorCnt = val;
    
	return 0;
}

void set_port_matrix(u8 port, u8 matrix)
{
    u32 reg_value, reg_add;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
    reg_add = 0x2004 + 0x100* port;
    reg_value = macMultiMT7530ReadReg(reg_add, extsw_flag);
    reg_value = (reg_value & (~0xFF0000)) | (matrix<<16);
    macMultiMT7530WriteReg(reg_add, reg_value, extsw_flag);
}

int macMT7530SetPortIsolate(u8 enable)
{
    u8 port;
    u32 reg_value, reg_add;

    for(port=0;port<5;port++){
        if(enable){
            set_port_matrix(port, (0x40 | (1<<port)));
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
            if (isEN7526G)
            {
                set_port_matrix(port | EXT_SWITCH_FLAG, (0x40 | (1<<port)));
            }
#endif
        }else{
            set_port_matrix(port, 0xFF);
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
            if (isEN7526G)
            {
                set_port_matrix(port | EXT_SWITCH_FLAG, 0xFF);
            }
#endif
        }
    }
    return 0;
}

/*___________________________________________________________________
**      function name: macMT7530SetPortMatrix
**      descriptions:
**         ...
**
**      parameters:
**         int port_group[6]       
**         int type
**         0:general_group
**         1:vlanbind_group
**         2:portbind_group
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**
**_______________________________________________________________
*/
int macMT7530SetPortMatrix(int group[6], int type)
{
    u8 lan_port, group_id, matrix, switch_port, defaultmt = 0;
    u32 reg_value, reg_add;
#ifdef TCSUPPORT_MULTI_SWITCH_EXT
    #define DEFAULT_INT_SWITCH_MASK 0xdf
	u8 lan_port_max_num = 4;
#else
	#define DEFAULT_INT_SWITCH_MASK 0xff
	u8 lan_port_max_num = 6;
	
    if(support_external_switch())
    {
        lan_port_max_num = 4; 
    }
#endif
	for(lan_port = 0; lan_port < lan_port_max_num; lan_port++)
	{
		if(lan_port_map[lan_port] != -1)
		{
			if(group[lan_port] > 6 || group[lan_port] < 0)
			{
				printk("[%s] port %x, group_id %d is wrong\n", __func__, lan_port, group[lan_port]);
				return -1;
			}
		}
		else
		{
			if(group[lan_port] != -1)
			{
				printk("unused port groupid must be -1!\n");
				return -1;
			}
		}
		if(0 == group[lan_port]){	/*this port(groupid=0) in matrix by default*/
			defaultmt |= (1 << macMT7530LanPortMap2Switch(lan_port));
			if(type == 0)
				set_port_matrix(macMT7530LanPortMap2Switch(lan_port),0xff);
			else if(type == 1)
				vlanbind_matrix[macMT7530LanPortMap2Switch(lan_port)] = 0xff;
			else if(type == 2)
				portbind_matrix[macMT7530LanPortMap2Switch(lan_port)] = 0xff;
		}
	}
	for(group_id=1; group_id<6; group_id++)
	{
		matrix = (0x40 | defaultmt);   /* port 6 and port(groupid=0) in matrix by default*/
		for(lan_port=0; lan_port<lan_port_max_num; lan_port++)
		{
			if((lan_port_map[lan_port] >= 0) && (group[lan_port] == group_id)){
				matrix |= 1<<macMT7530LanPortMap2Switch(lan_port);
			}
		}

		for(switch_port=0; switch_port<lan_port_max_num; switch_port++)
		{
			if(matrix & (~defaultmt) & (1<<switch_port))
			{
				if(type == 0)
				set_port_matrix(switch_port, matrix);
				else if(type == 1)
					vlanbind_matrix[switch_port] = matrix;
				else if(type == 2)
					portbind_matrix[switch_port] = matrix;
			}
		}
	}
	if((type == 1) || (type == 2)){ 	   
		for(switch_port = 0; switch_port < lan_port_max_num; switch_port++){
			set_port_matrix(switch_port,(vlanbind_matrix[switch_port] & portbind_matrix[switch_port] & DEFAULT_INT_SWITCH_MASK));
		}
	}

#ifdef TCSUPPORT_MULTI_SWITCH_EXT	
	set_port_matrix(extend_switch_port, 0x60);
#endif
   
    return 0;
}

int macMT7530SetLan2Lanswitch()
{
	int i;
	unsigned int value;
	for(i = 0; i < 6; ++i)
	{		
		value = switch_reg_read(GSW_BASE + 0x2004 + i * 0x100);
		value &= ~(0x3);
		switch_reg_write(GSW_BASE + 0x2004 + i * 0x100 , value);
	}
	printk("set lan2lan switch mode success!\n");
	return 0;
}

int macMT7530GetPortJabber(u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMSR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);

	return ((phy_value & 0x2) ? 1 : 0);
}

int macMT7530GetPortRemoteFault(u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = MII_BMSR;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);

	return ((phy_value & 0x10) ? 1 : 0);
}

int macMT7530GetPortMDICrossover(u8 port)
{
	u32 phy_addr, phy_reg, phy_value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	phy_addr = macMT7530GetPortPhyAddr(port, extsw_flag);
	phy_reg = 0x1D;
	phy_value = macMT7530PHYReadReg(phy_addr, phy_reg);

	return ((phy_value & (0x1 << 13)) ? 1 : 0);
}

int get_hsgmii_port_count(struct ifreq *ifr, mt7530_switch_api_cnt *portcnt)
{
	int ret_value = 0;
    mt7530_switch_api_params api_param;
	FE_RxCnt_t rxCnt;
	FE_TxCnt_t txCnt;
	u32 RxBytesCnt_Hi = 0;
	u32 RxBytesCnt_Lo = 0;
	u32 TxBytesCnt_Hi = 0;
	u32 TxBytesCnt_Lo = 0;

    if(copy_from_user(&api_param, (mt7530_switch_api_params *)ifr->ifr_data, sizeof(api_param)))
		return -1;

	memset(&rxCnt, 0, sizeof(FE_RxCnt_t));
	memset(&txCnt, 0, sizeof(FE_TxCnt_t));

	ret_value = FE_API_GET_HSGMII_RX_CNT(&rxCnt, 3);//3:eth hsgmii
	portcnt->RxPktsCnt = (u32)rxCnt.rxFrameCnt;
	portcnt->RxBroadPktsCnt = (u32)rxCnt.rxBroadcastCnt;
	portcnt->RxMultiPktsCnt = (u32)rxCnt.rxMulticastCnt;
	portcnt->RxNonUniPktsCnt = (u32)(portcnt->RxBroadPktsCnt + portcnt->RxMultiPktsCnt);
	portcnt->RxUniPktsCnt = (u32)(portcnt->RxPktsCnt - portcnt->RxNonUniPktsCnt);
	RxBytesCnt_Hi = rxCnt.rxFrameLen >> 32;
	RxBytesCnt_Lo = (u32)rxCnt.rxFrameLen;
	portcnt->RxBytesCnt_Hi = RxBytesCnt_Hi;
	portcnt->RxBytesCnt_Lo = RxBytesCnt_Lo;
	portcnt->RxUnderSizePktsCnt = (u32)rxCnt.rxUnderSizeCnt;
	portcnt->Rx64BytePktsCnt = (u32)rxCnt.rxEq64Cnt;
	portcnt->Rx65_127BytePktsCnt = (u32)rxCnt.rxFrom65To127Cnt;
	portcnt->Rx128_255BytePktsCnt = (u32)rxCnt.rxFrom128To255Cnt;
	portcnt->Rx256_511BytePktsCnt = (u32)rxCnt.rxFrom256To511Cnt;
	portcnt->Rx512_1023BytePktsCnt = (u32)rxCnt.rxFrom512To1023Cnt;
	portcnt->Rx1024_1518BytePktsCnt = (u32)rxCnt.rxFrom1024To1518Cnt;
	portcnt->RxOverSizePktsCnt = (u32)rxCnt.rxOversizeCnt;
	portcnt->RxDropFramesCnt = (u32)rxCnt.rxDropCnt;
	portcnt->RxCRCFramesCnt = (u32)rxCnt.rxCrcCnt;
	portcnt->RxFragmentErrorCnt = (u32)rxCnt.rxFragFameCnt;
	portcnt->RxJabberErrorCnt = (u32)rxCnt.rxJabberFameCnt; 
	portcnt->Rx1519_MAXBytePktsCnt = (u32)rxCnt.rxMore1518Cnt;

	ret_value = FE_API_GET_HSGMII_TX_CNT(&txCnt, 3);
	portcnt->TxPktsCnt = (u32)txCnt.txFrameCnt;
	portcnt->TxBroadPktsCnt = (u32)txCnt.txBroadcastCnt;
	portcnt->TxMultiPktsCnt = (u32)txCnt.txMulticastCnt;
	portcnt->TxNonUniPktsCnt = (u32)(portcnt->TxBroadPktsCnt + portcnt->TxMultiPktsCnt);
	portcnt->TxUniPktsCnt = (u32)(portcnt->TxPktsCnt - portcnt->TxNonUniPktsCnt);
	TxBytesCnt_Hi = txCnt.txFrameLen >> 32;
	TxBytesCnt_Lo = (u32)txCnt.txFrameLen;
	portcnt->TxBytesCnt_Hi = TxBytesCnt_Hi;
	portcnt->TxBytesCnt_Lo = TxBytesCnt_Lo;
	portcnt->TxUnderSizePktsCnt = (u32)txCnt.txLess64Cnt;
	portcnt->Tx64BytePktsCnt = (u32)txCnt.txEq64Cnt;
	portcnt->Tx65_127BytePktsCnt = (u32)txCnt.txFrom65To127Cnt;
	portcnt->Tx128_255BytePktsCnt = (u32)txCnt.txFrom128To255Cnt;
	portcnt->Tx256_511BytePktsCnt = (u32)txCnt.txFrom256To511Cnt;
	portcnt->Tx512_1023BytePktsCnt = (u32)txCnt.txFrom512To1023Cnt;
	portcnt->Tx1024_1518BytePktsCnt = (u32)txCnt.txFrom1024To1518Cnt;
	portcnt->TxOverSizePktsCnt = (u32)txCnt.txMore1518Cnt;
	portcnt->TxDropFramesCnt = (u32)txCnt.txDropCnt;
	portcnt->Tx1519_MAXBytePktsCnt = (u32)txCnt.txMore1518Cnt;

	return ret_value;
}

/*___________________________________________________________________
**      function name: macMT7530gswAPIDispatch
**      descriptions:
**         Called by tc3262_gmac_ioctl(cmd == RAETH_GSW_CTLAPI)
**
**      parameters:
**         struct ifreq *ifr: The arguments number.
**
**      global:
**
**      return:
**         0:succuces
**         -1:fail
**
**      call:
**
**      revision:
**      1. WPLin 2012/06/01
**_______________________________________________________________
*/
int macMT7530gswAPIDispatch(struct ifreq *ifr)
{
    int ret_value = 0;
    mt7530_switch_api_params api_param;
    u32 portId = 0;

    copy_from_user(&api_param, (mt7530_switch_api_params *)ifr->ifr_data, sizeof(api_param));
//  if (macMT7530_APIDbgDumpEn) printk(KERN_DEBUG "%s(%04X)\n", __FUNCTION__, api_param.cmdid);
    if (macMT7530_APIDbgDumpEn) printk("%s(%04X)\n", __FUNCTION__, api_param.cmdid);

    portId = api_param.PortQueueId;

    if(support_external_switch())
    {
        ret_value = macExtSwitchgswGlobalAPI(api_param);
        if(portId>4)
        {
            ret_value = macExtSwitchgswPortAPI(portId, api_param);
            goto err_exit;
        }
    }

    portId = macMT7530LanPortMap2Switch(portId);

    switch (api_param.cmdid)
    {
        // user API start from ID 0x0010
        case CMD_GET_BRGLEARNINGIND :
        {
            ret_value = macMT7530GetBrglearningInd(portId);
        }
            break;

        case CMD_SET_BRGLEARNINGIND :
        {
            ret_value = macMT7530SetBrglearningInd(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORTBRGIND :
        {
            u8 ind;
            ret_value = macMT7530GetPortBrgInd(portId, &ind);
            
            if (copy_to_user(api_param.paramext_ptr, &ind, sizeof(ind)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_PORTBRGIND :
        {
            ret_value = macMT7530SetPortBrgInd(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_DISCARDUNKNOWNMACIND :
        {
            ret_value = macMT7530GetDiscardUnknownMacInd(portId);
        }
            break;

        case CMD_SET_DISCARDUNKNOWNMACIND :
        {
            ret_value = macMT7530SetDiscardUnknownMacInd(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_AGETIME :
        {
            u32 MaxAgeTime;
            ret_value = macMT7530GetAgeTime(&MaxAgeTime);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &MaxAgeTime, sizeof(MaxAgeTime)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_AGETIME :
        {
            ret_value = macMT7530SetAgeTime((unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORTMAC :
        {
            u8 portMac[6];
            ret_value = macMT7530GetPortMac(portId, portMac);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, portMac, sizeof(portMac)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_GET_MIBCNT :
        {
            mt7530_switch_GetMibCnt_param GetMibCnt_param;
            if (copy_from_user(&GetMibCnt_param, api_param.paramext_ptr, sizeof(mt7530_switch_GetMibCnt_param)))
            {
                ret_value = -1;
                goto err_exit;
            }
			if (GetMibCnt_param.MibCntType < MIB_ID_TX_DROP_CNT || GetMibCnt_param.MibCntType >= MIB_ID_MAX)
			{
				printk("invalid mib cnt type 0x%x, type range should in 0x%x - 0x%x\n", GetMibCnt_param.MibCntType, MIB_ID_TX_DROP_CNT, MIB_ID_MAX);
				ret_value = -1;
                goto err_exit;
			}
            ret_value = macMT7530GetMIBCounter(GetMibCnt_param.MibCntType, portId, &(GetMibCnt_param.Out__Cnt));
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &GetMibCnt_param, sizeof(mt7530_switch_GetMibCnt_param)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_QUEUE_PRIORITY :
        {
            ret_value = macMT7530SetQueuePriority((unsigned long)(api_param.paramext_ptr), api_param.PortQueueId);
        }
            break;

        case CMD_GET_QUEUE_MAX_SIZE :
        {
            ret_value = macMT7530GetMaximumQueueSize(api_param.PortQueueId);
        }
            break;

        case CMD_SET_ALLOC_QUEUE_SIZE :
        {
            ret_value = macMT7530SetAllocatedQueueSize((u16)(unsigned long)(api_param.paramext_ptr), api_param.PortQueueId);
        }
            break;

        case CMD_GET_ALLOC_QUEUE_SIZE :
        {
            ret_value = macMT7530GetAllocatedQueueSize(api_param.PortQueueId);
        }
            break;

        case CMD_SET_WEIGHT :
        {
            portId = macMT7530LanPortMap2Switch((api_param.PortQueueId >> 16) & 0xFF);
            ret_value = macMT7530SetWeight((u8)(unsigned long)(api_param.paramext_ptr), (u8)(api_param.PortQueueId & 0xFF), (u8)portId);
        }
            break;

        case CMD_GET_WEIGHT :
        {
            portId = macMT7530LanPortMap2Switch((api_param.PortQueueId >> 16) & 0xFF);
            ret_value = macMT7530GetWeight((u8)(api_param.PortQueueId & 0xFF), (u8)portId);
        }
            break;

        case CMD_SET_BACK_PRESSURE :
        {
            mt7530_switch_api_backPressure_t backPressure;
            if (copy_from_user(&backPressure, api_param.paramext_ptr, sizeof(mt7530_switch_api_backPressure_t)))
            {
                ret_value = -1;
                goto err_exit;
            }
            portId = macMT7530LanPortMap2Switch((api_param.PortQueueId >> 16) & 0xFF);
            ret_value = macMT7530SetBackPressure(&backPressure, (u8)(api_param.PortQueueId & 0xFF), (u8)portId);
        }
            break;

        case CMD_GET_BACK_PRESSURE :
        {
            mt7530_switch_api_backPressure_t backPressure;
            portId = macMT7530LanPortMap2Switch((api_param.PortQueueId >> 16) & 0xFF);
            ret_value = macMT7530GetBackPressure(&backPressure, (api_param.PortQueueId & 0xFF), portId);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &backPressure, sizeof(mt7530_switch_api_backPressure_t)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_DROP_POLICY :
        {
            mt7530_switch_DropPolicy_t DropPolicy;
            if (copy_from_user(&DropPolicy, api_param.paramext_ptr, sizeof(mt7530_switch_DropPolicy_t)))
            {
                ret_value = -1;
                goto err_exit;
            }
            portId = macMT7530LanPortMap2Switch((api_param.PortQueueId >> 16) & 0xFF);
            ret_value = macMT7530SetDropPolicy(portId, (api_param.PortQueueId & 0xFF), &DropPolicy);
        }
            break;

        case CMD_GET_DROP_POLICY :
        {
            mt7530_switch_DropPolicy_t DropPolicy;
            portId = macMT7530LanPortMap2Switch((api_param.PortQueueId >> 16) & 0xFF);
            ret_value = macMT7530GetDropPolicy(portId, (api_param.PortQueueId & 0xFF), &DropPolicy);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &DropPolicy, sizeof(mt7530_switch_DropPolicy_t)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_TRAFFIC_DESCRIPTOR :
        {
            mt7530_switch_api_trafficDescriptor_t trafficDescriptor;
            if (copy_from_user(&trafficDescriptor, api_param.paramext_ptr, sizeof(trafficDescriptor)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetTrafficDescriptor(portId, &trafficDescriptor);
        }
            break;

        case CMD_GET_TRAFFIC_DESCRIPTOR :
        {
            mt7530_switch_api_trafficDescriptor_t trafficDescriptor;
            if (copy_from_user(&trafficDescriptor, api_param.paramext_ptr, sizeof(trafficDescriptor)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530GetTrafficDescriptor(portId, &trafficDescriptor);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &trafficDescriptor, sizeof(trafficDescriptor)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_CLR_TRAFFIC_DESCRIPTOR :
        {
            ret_value = macMT7530CleanTrafficDescriptor(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_AUTO_DETECT :
        {
            ret_value = macMT7530SetAutoDetection((u8)(unsigned long)(api_param.paramext_ptr), portId);
        }
            break;

        case CMD_GET_AUTO_DETECT :
        {
            u8 AutoDetction;
            ret_value = macMT7530GetAutoDetection(&AutoDetction, portId);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &AutoDetction, sizeof(AutoDetction)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_LOOPBACK_CONF :
        {
            ret_value = macMT7530SetLoopbackConf((u8)(unsigned long)(api_param.paramext_ptr), portId);
        }
            break;

        case CMD_GET_LOOPBACK_CONF :
        {
            u8 mode;
            ret_value = macMT7530GetLoopbackConf(&mode, portId);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &mode, sizeof(mode)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_GET_CONFIG_STAT :
        {
            ret_value = macMT7530GetConfigurationStatus(portId);
        }
            break;

        case CMD_SET_MAX_FRAME_SIZE :
        {
            ret_value = macMT7530SetMaxFrameSize((u16)(unsigned long)(api_param.paramext_ptr), 0);
        }
            break;

        case CMD_GET_MAX_FRAME_SIZE :
        {
            u16 MaxFrameSize;
            ret_value = macMT7530GetMaxFrameSize(&MaxFrameSize, 0);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &MaxFrameSize, sizeof(MaxFrameSize)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_DTEDCE_Ind :
        {
            ret_value = macMT7530SetDTEDCEInd((u8)(unsigned long)(api_param.paramext_ptr), portId);
        }
            break;

        case CMD_GET_DTEDCE_Ind :
        {
            u8 ind;
            ret_value = macMT7530GetDTEDCEInd(&ind, portId);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &ind, sizeof(ind)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_PAUSE_TIME :
        {
            ret_value = macMT7530SetPauseTime((u16)(unsigned long)(api_param.paramext_ptr), portId);
        }
            break;

        case CMD_GET_PAUSE_TIME :
        {
            u16 pauseTime;
            ret_value = macMT7530GetPauseTime(&pauseTime, portId);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &pauseTime, sizeof(pauseTime)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_PORT_ADMIN :
        {
            ret_value = macMT7530SetPortAdmin((u16)(unsigned long)(api_param.paramext_ptr), portId);
        }
            break;

        case CMD_GET_PORT_ADMIN :
        {
            u8 admin;
            ret_value = macMT7530GetPortAdmin(&admin, portId);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &admin, sizeof(admin)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_GET_PORT_CHANGEDCNT :
        {
            u32 cnt = 0;
            ret_value = macMT7530GetPortChangedCnt(portId, &cnt);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &cnt, sizeof(cnt)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_GET_PORT_MACLMT:
        {
            ret_value = macMT7530GetPortMacLimit(portId);
        }
            break;
			
		case CMD_GET_PORT_MACNUM:
		{
			ret_value = macMT7530GetPortMacLearnNum(portId);
		}
			break;
            
        case CMD_SET_PORT_MACLMT:
        {
            ret_value = macMT7530SetPortMacLimit(portId, (unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_PORT_SPDMD:
        {
            ret_value = macMT7530SetPortSpeedMode(portId, (unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORT_PAUSE :
        {
            ret_value = macMT7530GetPortPause(portId);
        }
            break;

        case CMD_SET_PORT_PAUSE :
        {
            ret_value = macMT7530SetPortPause(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORT_POLEN :
        {
            ret_value = macMT7530GetPortPolicingEnable(portId);
        }
            break;

        case CMD_SET_PORT_POLEN :
        {
            ret_value = macMT7530SetPortPolicingEnable(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORT_POL :
        {
            mt7530_switch_api_paramext4 paramext;
            u32 cir, cbs, ebs;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530GetPortPolicing(portId, &cir, &cbs, &ebs);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user((u32 *)(paramext.p1), &cir, sizeof(cir)))
            {
                ret_value = -1;
//              goto err_exit;
            }
            if (copy_to_user((u32 *)(paramext.p2), &cbs, sizeof(cbs)))
            {
                ret_value = -1;
//              goto err_exit;
            }
            if (copy_to_user((u32 *)(paramext.p3), &ebs, sizeof(ebs)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_PORT_POL :
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetPortPolicing(portId, paramext.p1, paramext.p2, paramext.p3);
        }
            break;

        case CMD_GET_PORT_RATELMTEN :
        {
            ret_value = macMT7530GetPortRatelimitingEnable(portId);
        }
            break;

        case CMD_SET_PORT_RATELMTEN :
        {
            ret_value = macMT7530SetPortRatelimitingEnable(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORT_RATELMT :
        {
            mt7530_switch_api_paramext2 paramext;
            u32 cir, pir;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530GetPortRatelimiting(portId, &cir, &pir);
//          if (ret_value == -1) goto err_exit;
            if (copy_to_user((u32 *)(paramext.p1), &cir, sizeof(cir)))
            {
                ret_value = -1;
//              goto err_exit;
            }
            if (copy_to_user((u32 *)(paramext.p2), &pir, sizeof(pir)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_PORT_RATELMT :
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetPortRatelimiting(portId, paramext.p1, paramext.p2);
        }
            break;

        case CMD_SET_LOOPBACK_REMOTE:
        {
            ret_value = macMT7530SetPortLoopBackRemote(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_GET_LOOPBACK_REMOTE:
        {
            u8 enable;
            ret_value = macMT7530GetPortLoopBackRemote(portId, &enable);
            if (copy_to_user(api_param.paramext_ptr, &enable, sizeof(enable)))
            {
                ret_value = -1;
            }
        }
        break;
        
        case CMD_SET_PORT_SPEEDMODE:
        {
            ret_value = macMT7530SetPortMode(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_GET_PORT_SPEEDMODE:
        {
            u8 mode = 0;
            u32 tmp;
            ret_value = macMT7530GetPortMode(portId, &mode);
            tmp = mode;
            if (copy_to_user(api_param.paramext_ptr, &tmp, sizeof(tmp)))
            {
                ret_value = -1;
            }
        }
        break;

        case CMD_GET_PORT_LINKSTATE:
        {
            mt7530_switch_api_paramext2 paramext;
            u8 linkstate, speed = 0;

            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530GetPortLinkState(portId, &linkstate, &speed);
            paramext.p1 = (u32)linkstate;
            paramext.p2 = (u32)speed;
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }
        }
        break;
        
        case CMD_SET_PORT_FLOWCONTROL_GLOBAL:
        {
            ret_value = macMT7530SetPortFlowControlGlobal((u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_GET_PORT_FLOWCONTROL_GLOBAL:
        {
            u8 enable;
            enable = macMT7530GetPortFlowControlGlobal();
            if (copy_to_user(api_param.paramext_ptr, &enable, sizeof(enable)))
            {
                ret_value = -1;
            }
        }
        break;
        
        case CMD_SET_PORT_FLOWCONTROL:
        {
            if((5==portId)||(6==portId))
                ret_value = macMT7530SetPortFlowControl(portId, (u8)(unsigned long)(api_param.paramext_ptr));
            else{
                ret_value = macMT7530SetPortPause(portId, (u8)(unsigned long)(api_param.paramext_ptr));
                macMT7530PortReset(portId);
            }
        }
        break;

        case CMD_GET_PORT_FLOWCONTROL :
        {
            u8 enable;
            if ((5 == portId ) || ( 6 == portId ))
            {
                ret_value = macMT7530GetPortFlowControl(portId);
            }
            else
            {
                ret_value = macMT7530GetPortPause(portId);
            }
            
            if(ret_value >= 0)
            {
                enable = (u8)ret_value;
                if (copy_to_user(api_param.paramext_ptr, &enable, sizeof(enable)))
                {
                    ret_value = -1;
                }
            }
        }
        break;

        case CMD_GET_PORT_FLOWCONTROL_CFG :
        {
            u8 enable;
            ret_value = macMT7530GetPortFlowControl(portId);
            
            if(ret_value >= 0)
            {
                enable = (u8)ret_value;
                if (copy_to_user(api_param.paramext_ptr, &enable, sizeof(enable)))
                {
                    ret_value = -1;
                }
            }
        }
        break;

        case CMD_SET_LAN2LAN_CLEAR:
        {
            ret_value = macMT7530Setlan2lanClear();            
        }
        break;

        case CMD_SET_LAN2LAN:
        {
            mt7530_switch_lan2lan paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530Setlan2lan(paramext);            
        }
        break;

        case CMD_SET_VLAN_ACTIVE:
        {
            ret_value = macMT7530SetVlanActive((u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_SET_PORT_QOS_PARAM:
        {
            mt7530_switch_qosparam qos_param;
            if (copy_from_user(&qos_param, api_param.paramext_ptr, sizeof(qos_param)))
            {
                ret_value = -1;
                goto err_exit;
            }

            #ifdef TCSUPPORT_QOS
            if (portId == wan_port_id)
            {
                if (0 == qos_param.qosmode)
                {
                    qos_flag = QOS_HW_PQ;
                }
                else if (1 == qos_param.qosmode)
                {
                    qos_flag = QOS_HW_WRR;
                }
                else if (2 == qos_param.qosmode)
                {
                    qos_flag = QOS_HW_PQ_WRR;
                }
                else if (0xff == qos_param.qosmode)
                {
                    qos_flag = NULLQOS;
                }
            }
            #endif
            
            ret_value = macMT7530Setqosparam(portId, qos_param); 

        }
        break;

        case CMD_GET_LAN2LAN:
        {
            mt7530_switch_lan2lan_info lan2lan;
            ret_value = macMT7530Getlan2lan(&lan2lan);
            if (ret_value == -1) 
            {
                goto err_exit;
            }
            if (copy_to_user(api_param.paramext_ptr, &lan2lan, sizeof(lan2lan)))
            {
                ret_value = -1;
            }
        }
        break;

        case CMD_GET_PORT_STATISTICS:
        {
            mt7530_switch_api_cnt portcnt;

			memset(&portcnt, 0, sizeof(mt7530_switch_api_cnt));
			
			if(get_lan_info(api_param.PortQueueId) == HSGMII_ETH){
            	ret_value = get_hsgmii_port_count(ifr, &portcnt);
			}else{		
				ret_value = macMT7530Getportcnt(portId, &portcnt);
			}
			
            if (ret_value == -1) 
            {
                goto err_exit;
            }
            if (copy_to_user(api_param.paramext_ptr, &portcnt, sizeof(portcnt)))
            {
                ret_value = -1;
            }
        }
        break;

        case CMD_SET_PORT_ISOLATE:
        {
            ret_value = macMT7530SetPortIsolate((u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_SET_PORT_MATRIX:
        {
            mt7530_switch_api_matrix_t matrix_entry;
            int i = 0;
            int group[6];
            int type = 0;
            memset(group, -1, sizeof(group));
            if(copy_from_user(&matrix_entry, api_param.paramext_ptr, sizeof(matrix_entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
            
            type = matrix_entry.type;
            for(i = 0; i < 6; i++)
                group[i] = matrix_entry.group[i];
            ret_value = macMT7530SetPortMatrix(group, type);
        }
        break;

		case CMD_SET_LAN2LAN_SWITCH:
		{
			ret_value =macMT7530SetLan2Lanswitch();
		}
		break;
		
        case CMD_CLR_MAC_TABLE:
        {
            ret_value = macMT7530CleanMactbl();
        }
        break;
        
        case CMD_SET_PORT_CLEAN_MAC:
        {
            ret_value = macMT7530CleanMactblByPort(portId);
        }
        break;

        case CMD_GET_BR_TABLE:
        {
            mt7530_switch_brtable *pBrtable
                = (mt7530_switch_brtable *)kmalloc(sizeof(mt7530_switch_brtable), GFP_KERNEL);
            if ( !pBrtable )
            {
                ret_value = -1;
                goto err_exit;
            }
            pBrtable->count = 0;
            ret_value = macMT7530GetBRTable(pBrtable, portId);
            if (ret_value == -1) 
            {
                kfree(pBrtable);
                goto err_exit;
            }
            if ( copy_to_user(api_param.paramext_ptr, pBrtable
                , sizeof(mt7530_switch_brtable)))
                ret_value = -1;
            kfree(pBrtable);
        }
        break;      

        case CMD_SET_PORT_STATISTICS_CLEAR:
        {
             ret_value = macMT7530ClearMIBCounter();
        }
        break;
		
		case CMD_SET_CLEAR_PORT_STATISTICS:
        {
             ret_value = macMT7530ClearPortMIBCounter(portId);
        }
        break;

        case CMD_GET_PORT_LOOPDET :
        {
            ret_value = macMT7530GetPortLoopDetect(portId);
        }
            break;

        case CMD_SET_PORT_LOOPDET :
        {
            ret_value = macMT7530SetPortLoopDetect(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_PORT_DISLOOPED :
        {
            ret_value = macMT7530SetPortDisableLooped(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORT_ACT :
        {
            ret_value = macMT7530GetPortActive(portId);
        }
            break;

        case CMD_SET_PORT_ACT :
        {
            ret_value = macMT7530SetPortActive(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORT_AN :
        {
            ret_value = macMT7530GetPortAutoNegState(portId);
        }
            break;
            
        case CMD_SET_PORT_AN :
        {
            ret_value = macMT7530PortAutoNegActive(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_PORT_AN_RESTART :
        {
            ret_value = macMT7530PortAutoNegRestartAutoConfig(portId);
        }
            break;

        case CMD_GET_PORT_AN_FAIL :
        {
            ret_value = macMT7530GetEthPortAutoNegFailure(portId);
        }
            break;

        case CMD_GET_PORT_LINK_LOSS :
        {
            ret_value = macMT7530GetEthPortLos(portId);
        }
            break;

        case CMD_GET_PORT_FAIL :
        {
            ret_value = macMT7530GetEthPortFailure(portId);
        }
            break;

        case CMD_GET_PORT_CONGESTION :
        {
            ret_value = macMT7530GetEthPortCongestion(portId);
        }
            break;

        case CMD_SET_PORT_MAX_BIT_RATE :
        {
            char speed[5];
			memset(speed, 0, sizeof(speed));
            if (copy_from_user(speed, api_param.paramext_ptr, sizeof(speed) - 1))
            {
                ret_value = -1;
                goto err_exit;
            }
			speed[4] = '\0';
            ret_value = macMT7530SetPortMaxBitRate(portId, speed);
        }
            break;

        case CMD_GET_PORT_MAX_BIT_RATE :
        {
            char speed[5];
            ret_value = macMT7530GetPortMaxBitRate(portId, speed);
            if (copy_to_user(api_param.paramext_ptr, speed, 5))
            {
                ret_value = -1;
            }
        }
            break;

        case CMD_SET_PORT_DUPLEX_MODE :
        {
            char mode[5];
			memset(mode, 0, sizeof(mode));
            if (copy_from_user(mode, api_param.paramext_ptr, sizeof(mode) - 1))
            {
                ret_value = -1;
                goto err_exit;
            }
			mode[4] = '\0';
            ret_value = macMT7530SetPortDuplexMode(portId, mode);
        }
            break;
            
        case CMD_GET_PORT_DUPLEX_MODE :
        {
            char mode[5];
            ret_value = macMT7530GetPortDuplexMode(portId, mode);
            if (copy_to_user(api_param.paramext_ptr, mode, 5))
            {
                ret_value = -1;
            }
        }
            break;

        case CMD_GET_PORT_RX_BYTE :
        {
            mt7530_switch_api_paramext2 paramext;
            unsigned int RxByteCnt_L = 0, RxByteCnt_H = 0;
			mt7530_switch_api_cnt portcnt;

			memset(&portcnt, 0, sizeof(mt7530_switch_api_cnt));
			
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
			
			if(get_lan_info(api_param.PortQueueId) == HSGMII_ETH){
            	ret_value = get_hsgmii_port_count(ifr, &portcnt);
				RxByteCnt_H = portcnt.RxBytesCnt_Hi;
				RxByteCnt_L = portcnt.RxBytesCnt_Lo;
			}else
            	ret_value = macMT7530GetPortRxByte(portId, &RxByteCnt_L, &RxByteCnt_H);
			
            if (ret_value == -1) goto err_exit;

			paramext.p1 = RxByteCnt_H;
			paramext.p2 = RxByteCnt_L;
			if(copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
			{
                ret_value = -1;
            }
        }
            break;

        case CMD_GET_PORT_TX_BYTE :
        {
            mt7530_switch_api_paramext2 paramext;
            unsigned int TxByteCnt_L = 0, TxByteCnt_H = 0;
			mt7530_switch_api_cnt portcnt;

			memset(&portcnt, 0, sizeof(mt7530_switch_api_cnt));
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }

			if(get_lan_info(api_param.PortQueueId) == HSGMII_ETH){
            	ret_value = get_hsgmii_port_count(ifr, &portcnt);
				TxByteCnt_H = portcnt.TxBytesCnt_Hi;
				TxByteCnt_L = portcnt.TxBytesCnt_Lo;
			}else
            	ret_value = macMT7530GetPortTxByte(portId, &TxByteCnt_L, &TxByteCnt_H);
			
            if (ret_value == -1) goto err_exit;

			paramext.p1 = TxByteCnt_H;
			paramext.p2 = TxByteCnt_L;
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }
        }
            break;

        case CMD_SET_RGMII_RX_CTRL_DELAY:
          {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiRxCtrlDelay((u8)(paramext.p1), (u8)(paramext.p2));
        
          }
          break;
          case CMD_SET_RGMII_RX_DATA_DELAY:
          {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiRxDataDelay((u8)(paramext.p1), (u8)(paramext.p2));
          
          }
          break;
        
          case CMD_SET_RGMII_RX_CLK_DELAY:
              {
                   mt7530_switch_api_paramext2 paramext;
                  if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
                  {
                      ret_value = -1;
                      goto err_exit;
                  }
                  ret_value = macMT7530SetRgmiiRxClkDelay((u8)(paramext.p1), (u8)(paramext.p2));
              
              }
              break;
        
          case CMD_SET_RGMII_TX_EN_DELAY:
              {
                   mt7530_switch_api_paramext2 paramext;
                  if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
                  {
                      ret_value = -1;
                      goto err_exit;
                  }
                  ret_value = macMT7530SetRgmiiTxEnDelay((u8)(paramext.p1), (u8)(paramext.p2));
              
              }
              break;
        
          case CMD_SET_RGMII_TX_DATA_DELAY:
              {
                   mt7530_switch_api_paramext2 paramext;
                  if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
                  {
                      ret_value = -1;
                      goto err_exit;
                  }
                  ret_value = macMT7530SetRgmiiTxDataDelay((u8)(paramext.p1), (u8)(paramext.p2));
              
              }
              break;
        
          case CMD_SET_RGMII_TX_CLK_DELAY:
          {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiTxClkDelay((u8)(paramext.p1), (u8)(paramext.p2));
        
          }
          break; 
          case CMD_SET_RGMII_RX_CLK_EN:
              {
                   mt7530_switch_api_paramext2 paramext;
                  if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
                  {
                      ret_value = -1;
                      goto err_exit;
                  }
                  ret_value = macMT7530SetRgmiiRxClkEn((u8)(paramext.p1), (u8)(paramext.p2));
              
              }
              break;
        
          case CMD_SET_RGMII_RX_CLK_ALIGN:
                      {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiRxClkAlign((u8)(paramext.p1), (u8)(paramext.p2));
        
          }
          break;
          case CMD_SET_RGMII_RX_CLK_INV:
                      {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiRxClkInv((u8)(paramext.p1), (u8)(paramext.p2));
        
          }
          break;
          case CMD_SET_RGMII_TX_CLK_EN:
                      {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiTxClkEn((u8)(paramext.p1), (u8)(paramext.p2));
        
          }
          break;
          case CMD_SET_RGMII_TX_CLK_ALIGN:
                      {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiTxClkAlign((u8)(paramext.p1), (u8)(paramext.p2));
        
          }
          break;
          case CMD_SET_RGMII_TX_CLK_INV:
                      {
               mt7530_switch_api_paramext2 paramext;
              if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
              {
                  ret_value = -1;
                  goto err_exit;
              }
              ret_value = macMT7530SetRgmiiTxClkInv((u8)(paramext.p1), (u8)(paramext.p2));
        
          }
          break;

        case CMD_GET_TX_TIMESTAMP :
        {
            mt7530_switch_api_timestamp_fifo tx_ts;
            ret_value = macMT7530GetTxTimestamp(&tx_ts);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &tx_ts, sizeof(mt7530_switch_api_timestamp_fifo)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_GET_RX_TIMESTAMP :
        {
            mt7530_switch_api_timestamp_fifo rx_ts;
            ret_value = macMT7530GetRxTimestamp(&rx_ts);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &rx_ts, sizeof(mt7530_switch_api_timestamp_fifo)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_GET_CURRTIME :
        {
            mt7530_switch_api_timestamp ts;
            ret_value = macMT7530GetCurrTime(&ts);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &ts, sizeof(mt7530_switch_api_timestamp)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_CURRTIME :
        {
            mt7530_switch_api_timestamp ts;
            if (copy_from_user(&ts, api_param.paramext_ptr, sizeof(mt7530_switch_api_timestamp)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetCurrTime((int)(api_param.PortQueueId), &ts);
        }
            break;

        case CMD_SET_OFFSET :
        {
            mt7530_switch_api_timestamp ts;
            if (copy_from_user(&ts, api_param.paramext_ptr, sizeof(mt7530_switch_api_timestamp)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetOffset((int)(api_param.PortQueueId),&ts);
        }
            break;

        case CMD_GET_PTPSTATE :
        {
            int EnablePort;
            ret_value = macMT7530GetPTPState(&EnablePort);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &EnablePort, sizeof(int)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_PTPSTATE :
        {
            ret_value = macMT7530SetPTPState((int)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_DS_TRTCM_ENABLE :
        {
            ret_value = macMT7530SetDsTrtcmEnable((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_JOYME_BANDWIDTH) || defined(TCSUPPORT_CF_JOYMEV2_PON) || defined(TCSUPPORT_XPON_HAL_API_EXT)              
        case CMD_SET_DEV_TRTCM_RULE :
        {
            mt7530_switch_dev_speed_acl_t dev_trtcm_entry;
            if (copy_from_user(&dev_trtcm_entry, api_param.paramext_ptr, sizeof(dev_trtcm_entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
            
            ret_value = macMT7530SetMacMatchACL(&dev_trtcm_entry);
        }
            break;
#endif/*TCSUPPORT_COMPILE*/         
        case CMD_SET_DS_TRTCM :
        {
            mt7530_switch_api_trtcm_t trtcm_entry;
            if (copy_from_user(&trtcm_entry, api_param.paramext_ptr, sizeof(trtcm_entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetDsTrtcm(&trtcm_entry);
        }
            break;

        case CMD_GET_DS_TRTCM :
        {
            mt7530_switch_api_trtcm_acl_t trtcm_acl_entry;
			memset(&trtcm_acl_entry, 0, sizeof(mt7530_switch_api_trtcm_acl_t));
            ret_value = macMT7530GetDsTrtcm(&trtcm_acl_entry);
            if (ret_value == -1) goto err_exit;
            if (copy_to_user(api_param.paramext_ptr, &trtcm_acl_entry, sizeof(trtcm_acl_entry)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_DSP5_RATELIMIT_ENABLE:
        {
            ret_value = macMT7530SetDsP5RateLimitEnable((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_DSP5_RATELIMIT:
        {
            ret_value = macMT7530SetDsP5RateLimit((unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_DSP5_QOS :
        {
            ret_value = macMT7530SetDsP5Qos((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

// add for storm control
        case CMD_SET_STORM_CTRL_PORT :
        {
            ret_value = macMT7530SetStormCtrlPort((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;  

        case CMD_SET_STORM_CTRL_UNIT :
        {
            ret_value = macMT7530SetStormCtrlUnit((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;      
            
        case CMD_SET_STORM_CTRL_TYPE :
        {
            ret_value = macMT7530SetStormCtrlType((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;  
            
        case CMD_SET_STORM_CTRL_MODE :
        {
            ret_value = macMT7530SetStormCtrlMode((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;  
            
        case CMD_SET_STORM_CTRL_PERIOD :
        {
            ret_value = macMT7530SetStormCtrlPeriod((u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;          
        case CMD_GET_PORT_UP_DOWN_STATE :
        {
            ret_value = macMT7530GetPortUpDownState(portId);
        }
        break;

        case CMD_SET_PORT_UP_DOWN :
        {
            ret_value = macMT753SetPortUpDown(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_GET_CHIP_TYPE :
        {
            e_chip_type chip_type = 0;
            ret_value = macMT7530GetChipType(&chip_type);
            if (ret_value == -1)
            {
                goto err_exit;
            }
            if (copy_to_user(api_param.paramext_ptr, &chip_type, sizeof(chip_type)))
            {
                ret_value = -1;
                goto err_exit;
            }
        }
            break;
        case CMD_GET_PORT_ALL_MAC:
        {
            mt7530_switch_port_mac entry;
            memset(&entry, 0, sizeof(entry));

            ret_value = macMT7530GetPortAllMac(portId, &entry);
            if (ret_value == -1) 
            {
                goto err_exit;
            }
            if (copy_to_user(api_param.paramext_ptr, &entry, sizeof(entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
        }
            break;
        case CMD_GET_PORT_BY_MAC:
        {
            mt7530_switch_macinfo entry;
            memset(&entry, 0, sizeof(entry));
            
            if (copy_from_user(&entry, api_param.paramext_ptr, sizeof(entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
            
            ret_value = macMT7530GetPortByMac(&entry);
            
            if (ret_value == -1) 
            {
                goto err_exit;
            }
            if (copy_to_user(api_param.paramext_ptr, &entry, sizeof(entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
        }
            break;
        case CMD_ADD_VLAN_BIND_MODE:
        {
            ret_value = macMT7530AddPortVlanBind(portId, (u16)(unsigned long)(api_param.paramext_ptr));
            if (ret_value == -1) 
            {
                goto err_exit;
            }
        }
            break;
        case CMD_DEL_VLAN_BIND_MODE:
        {
            ret_value = macMT7530DelPortVlanBind(portId, (u16)(unsigned long)(api_param.paramext_ptr));
            if (ret_value == -1) 
            {
                goto err_exit;
            }
        }
            break;
 /*end storm control*/
        case CMD_SET_PORTSTATUS:
        {
            ret_value = macMT7530SetPortStatus(portId, (u8)(unsigned long)(api_param.paramext_ptr));
            break;
        }
        
        case CMD_GET_PORTSTATUS:
        {
            ret_value = macMT7530GetPortStatus(portId);
        }
        break;

        case CMD_SET_ACL_RULE:
        {
            mt7530_switch_acl_info acl_rule_info ;
            if (copy_from_user(&acl_rule_info, api_param.paramext_ptr, sizeof(acl_rule_info)))
            {
                ret_value = -1;
                goto err_exit;
            }
            if( acl_rule_info.mode < ACL_SET_MODE_DMAC )
            {
                ret_value = acl_set_upstream_ratelimit_for_special(acl_rule_info.rate , acl_rule_info.enable ,acl_rule_info.mode);
            }
            else if( acl_rule_info.mode < ACL_SET_MODE_VLAN )
            {
                ret_value = acl_set_upstream_ratelimit_for_mac(acl_rule_info.pattern_info.mac ,acl_rule_info.rate ,acl_rule_info.enable
                                                                , acl_rule_info.index , acl_rule_info.mode);
            }
            else if(ACL_SET_MODE_VLAN == acl_rule_info.mode)
            {
                ret_value = acl_set_upstream_ratelimit_for_vlan(acl_rule_info.pattern_info.vlan , acl_rule_info.rate ,acl_rule_info.enable
                                                                , acl_rule_info.index);
            }   
        }
            break;
        case CMD_SET_VLAN_ENTRY_ENABLE :
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530VLan_Entry_Enable((u16)(paramext.p1), (u8)(paramext.p2));
        }
            break;

        case CMD_SET_VLAN_ENTRY_EGTAG :
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530VLan_Entry_SetETag((u16)(paramext.p1), (u8)(portId), (u8)(paramext.p2));
        }
            break;

        case CMD_GET_VLAN_ENTRY_EGTAG :
        {
            mt7530_switch_api_paramext2 paramext;
            u8 etag;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530VLan_Entry_GetETag((u16)(paramext.p1), (u8)(portId), &etag);
            if (copy_to_user((u8 *)(u32 *)(paramext.p2), &etag, sizeof(etag)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_SET_VLAN_ENTRY_STAG :
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530VLan_Entry_SetSTag((u16)(paramext.p1), (u8)(portId), (u8)(paramext.p2));
        }
            break;

        case CMD_GET_VLAN_ENTRY_STAG :
        {
            mt7530_switch_api_paramext2 paramext;
            u8 stag;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530VLan_Entry_GetSTag((u16)(paramext.p1), (u8)(portId), &stag);
            if (copy_to_user((u8 *)(u32 *)(paramext.p2), &stag, sizeof(stag)))
            {
                ret_value = -1;
//              goto err_exit;
            }
        }
            break;

        case CMD_ADD_VLAN_PORT_STAG :
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530VLan_Port_AddSTag((u8)(portId), (u8)(paramext.p1), (u16)(paramext.p2));
        }
            break;

        case CMD_SET_VLAN_ENTRY_ETAGMODE :
        {
            ret_value = macMT7530VLan_Entry_SetETagMode((u16)(api_param.PortQueueId), (u8)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_GET_PORT_RATE_LIMIT :
        {
            mt7530_switch_api_paramext2 paramext;
            
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530GetPortRateLimit(portId, paramext.p1, &paramext.p2);
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }
        }
            break;

        case CMD_SET_PORT_RATE_LIMIT :
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetPortRateLimit(portId, paramext.p1, paramext.p2);
        }
            break;

#ifdef CONFIG_TP_IMAGE
        case CMD_GET_ETHERWAN_PORT_RATELMTEN :
        {
            if((wan_port_id >= 0) && (wan_port_id<MT7530_SWIC_PORTNUM-1))
            {   
                printk("wan port id is %d \n", wan_port_id);
                ret_value = macMT7530GetPortRatelimitingEnable(wan_port_id);
            }
            else
            {
                ret_value = -1;
            }
        }
            break;

        case CMD_SET_ETHERWAN_PORT_RATELMTEN :
        {
            if(wan_port_id >= 0 && wan_port_id<MT7530_SWIC_PORTNUM-1)
            {
                printk("wan port id is %d \n", wan_port_id);
                ret_value = macMT7530SetPortRatelimitingEnable(wan_port_id, (u8)(unsigned long)(api_param.paramext_ptr));
            }
            else
            {
                ret_value = -1;
            }
        }
            break;

#endif /* CONFIG_TP_IMAGE */

        case CMD_GET_ETHERWAN_PORT_RATE_LIMIT :
		{
			mt7530_switch_api_paramext2 paramext;
			u32 rate=0;
			if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
			{
				ret_value = -1;
			}
	        if((wan_port_id >= 0) && (wan_port_id<MT7530_SWIC_PORTNUM-1))
            {   
                printk("wan port id is %d \n", wan_port_id);
			    ret_value = macMT7530GetPortRateLimit(wan_port_id, paramext.p2, &rate);
            }
			if (copy_to_user((u32 *)(paramext.p1), &rate, sizeof(rate)))
			{
				ret_value = -1;
			}
		}
		
			break;

		case CMD_SET_ETHERWAN_PORT_RATE_LIMIT :
		{
			mt7530_switch_api_paramext2 paramext;
			if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
			{
				ret_value = -1;
			}
            if(wan_port_id >= 0 && wan_port_id<MT7530_SWIC_PORTNUM-1)
            {
                printk("wan port id is %d \n", wan_port_id);
			    ret_value = macMT7530SetPortRateLimit(wan_port_id, paramext.p1, paramext.p2);
            }
		}
			break;

        case CMD_SET_PORT_QOS_ENABLE :
        {

            ret_value = macMT7530SetDsPortQos(portId, (u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;
                
        case CMD_SET_PORT_QOS_MODE :
        {
            mt7530_switch_queue_info paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530Setqueuemark(paramext);
        }
                break;

        case CMD_SET_MAC_TX_RX :
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetMACTxRx(portId, paramext.p1, paramext.p2);
        }
            break;          

        // =========== internal API start from ID 0x1000 ============
        case CMD_SET_MIBCNT_EN :
        {
            ret_value = macMT7530EnableMIBCounter((unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_MIBCNT_CLR :
        {
            ret_value = macMT7530ClearMIBCounter();
        }
            break;

        case CMD_SET_RXOCT_MODE :
        {
            ret_value = macMT7530SetRxOctMode((unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_TXOCT_MODE :
        {
            ret_value = macMT7530SetTxOctMode((mt7530_switch_api_RxTxOctMode)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_BXPKT_MODE :
        {
            ret_value = macMT7530SetBxPktMode((mt7530_switch_api_RxTxOctMode)(api_param.paramext_ptr));
        }
            break;

        // =========== debug API start from ID 0x7F00 =============
        case CMD_APIDBGDUMP_EN :
        {
            ret_value = macMT7530APIDbgDumpEn((unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_DO_P6Cal:
        {
            ret_value = macMT7530doP6Cal();
        }
            break;

        // =========== ether general API start from ID 0x8000 =============
        case CMD_ETH_GENERAL_API_CTL:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = Eth_General_API((u32)(paramext.p1), (u32)(paramext.p2));            
        }
        break;

        /*  7513G,7526G rgmii interface*/
        case CMD_TRAFFIC_SET:
        {
            rgmii_setting_s param;
            if (copy_from_user(&param, api_param.paramext_ptr, sizeof(param)))
            {
                ret_value = -1;
                goto err_exit;
            }
			
			if (TRAFFIC_TYPE_MAX > param.wan_type )
			{
            	ret_value = rgmii_setting(param.wan_type, param.interface);
			}
			else 
			{
				printk("the value of wan_type must less than %d", TRAFFIC_TYPE_MAX);
				ret_value = -1;
				goto err_exit;
			}
        }
            break;

        case CMD_TRAFFIC_DISPLAY:
            ret_value = rgmii_setting_display();
            break;

        case CMD_TRAFFIC_RGMII_MODE:
        {
            rgmii_setting_s param;
            if (copy_from_user(&param, api_param.paramext_ptr, sizeof(param)))
            {
                ret_value = -1;
                goto err_exit;
            }
            if(param.interface == RGMII){
                ret_value = rgmii_disable(param.mode);
            }
            printk("%s \n", param.interface?"RGMII":"TRGMII");
        }
            break;
        case CMD_SET_PER_VLAN_ACTION:
        {
            mt7530_switch_api_vlan_table_t vlantable_entry;

            if (copy_from_user(&vlantable_entry, api_param.paramext_ptr, sizeof(vlantable_entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
			if (7 > vlantable_entry.port_id)
			{
				ret_value = macMT7530SetPerVlanAction(vlantable_entry.port_id, vlantable_entry.o_vid, vlantable_entry.n_vid, vlantable_entry.vlan_mode, vlantable_entry.enable);
			}
			else 
			{
				printk("the value of port_id must less than 7");
				ret_value = -1;
                goto err_exit;
			}
        }
            break;

        case CMD_SET_PER_PORT_VLAN_ACTION:
        {
            mt7530_switch_api_vlan_table_t vlantable_entry;

            if (copy_from_user(&vlantable_entry, api_param.paramext_ptr, sizeof(vlantable_entry)))
            {
                ret_value = -1;
                goto err_exit;
            }
			if (7 > vlantable_entry.port_id)
			{
				ret_value = macMT7530SetPerPortVlanAction(vlantable_entry.port_id, vlantable_entry.n_vid, vlantable_entry.vlan_mode, vlantable_entry.enable);
			}
			else 
			{
				printk("the value of port_id must less than 7");
				ret_value = -1;
                goto err_exit;
			}
        }
            break;
            
        case CMD_SET_ARL_AGING:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetArlAging((u8)(paramext.p1), (u32)(paramext.p2));            
        }
            break;  

        case CMD_SET_ARL_MACTBLADD:
        {
            mt7530_switch_mactbladd_t mactbladd;
            if (copy_from_user(&mactbladd, api_param.paramext_ptr, sizeof(mactbladd)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetArlMacTblAdd(&mactbladd);            
        }
            break;
            
        case CMD_SET_ARL_ADDRTBLDISP:
        {
            ret_value = macMT7530SetArlAddrTblDisp((u8)(unsigned long)(api_param.paramext_ptr));            
        }
            break;  

        case CMD_SET_ARL_ADDRTBLDISP2:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetArlAddrTblDisp2((u32)(paramext.p1), (u8)(paramext.p2));            
        }
            break;  

        case CMD_SET_ARL_ADDRTBLCLR:
        {
            ret_value = macMT7530SetArlAddrTblClr((u8)(unsigned long)(api_param.paramext_ptr));            
        }
            break;  

        case CMD_SET_ARL_ADDRTBLCLR2:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetArlAddrTblClr2((u32)(paramext.p1), (u8)(paramext.p2));            
        }
            break;  
        case CMD_SET_VLAN_PVID :
        {
            portId = api_param.PortQueueId;
            ret_value = macMT7530SetVlanPvid(portId, (u16)(unsigned long)(api_param.paramext_ptr));
        }
            break;

        case CMD_SET_VLAN_VID:
        {
            mt7530_switch_api_vlan_vid_t vid;
            if (copy_from_user(&vid, api_param.paramext_ptr, sizeof(vid)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetVlanVid(&vid);            
        }
            break;  
            
        case CMD_SET_VLAN_DISP:
        {
            ret_value = macMT7530SetVlanDisp();           
        }
            break;

        case CMD_SET_VLAN_PORT_ATTR:
        {
            portId = api_param.PortQueueId;
            ret_value = macMT7530SetVlanPortAttr(portId,(u8)(unsigned long)(api_param.paramext_ptr));           
        }
            break;
            
        case CMD_SET_VLAN_PORT_MODE:
        {
            portId = api_param.PortQueueId;
            ret_value = macMT7530SetVlanPortMode(portId,(u8)(unsigned long)(api_param.paramext_ptr));           
        }
            break;  
        case CMD_SET_VLAN_EGRESS_TAG_PVC:
        {
            portId = api_param.PortQueueId;
            ret_value = macMT7530SetVlanEgressTagPvc(portId,(u8)(unsigned long)(api_param.paramext_ptr));           
        }
            break;
        case CMD_SET_VLAN_EGRESS_TAG_PCR:
        {
            portId = api_param.PortQueueId;
            ret_value = macMT7530SetVlanEgressTagPcr(portId,(u8)(unsigned long)(api_param.paramext_ptr));           
        }
            break;
            
        case CMD_SET_VLAN_ACC_FRM:
        {
            portId = api_param.PortQueueId;
            ret_value = macMT7530SetVlanAccFrm(portId,(u8)(unsigned long)(api_param.paramext_ptr));             
        }
            break;  
            
        case CMD_SET_PORT_MIRROR_ENABLE:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetPortMirrorEnable((u8)(paramext.p1), (u8)(paramext.p2));            
        }
            break;
            
        case CMD_SET_PORT_MIRROR_PORT_BASED:
        {
            mt7530_switch_api_port_mirror_port_based_t portbased;
            portId = api_param.PortQueueId;
            if (copy_from_user(&portbased, api_param.paramext_ptr, sizeof(portbased)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetPortMirrorPortBased(portId, &portbased);            
        }
            break;  
            
        case CMD_SET_DSTQ:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
    		eth_dstq_write((u32)(paramext.p1));
    		eth_gsw_fc_write((u32)(paramext.p2));
            
            ret_value = 0;          
        }
            break;
            
        case CMD_SET_ACL_PORT_ENABLE:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetAclPortEnable((u8)(paramext.p1), (u8)(paramext.p2));           
        }
            break;
            
        case CMD_SET_ACL_MULTI_HIT:
        {
            ret_value = macMT7530SetAclMultiHit((u8)(unsigned long)(api_param.paramext_ptr));           
        }
            break;  
            
        case CMD_SET_ACL_TBL_CLR:
        {
            ret_value = macMT7530SetAclTblClr();              
        }
            break;
            
        case CMD_SET_ACL_TBL_DISP:
        {
            ret_value = macMT7530SetAclTblDisp((u8)(unsigned long)(api_param.paramext_ptr));            
        }
            break;
            
        case CMD_SET_ACL_TBL_ADD:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetAclTblAdd((u8)(paramext.p1), (u32)(paramext.p2), (u32)(paramext.p3));             
        }
            break;
            
        case CMD_SET_ACL_MASK_TBL_ADD:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetAclMaskTblAdd((u8)(paramext.p1), (u32)(paramext.p2), (u32)(paramext.p3));             
        }
            break;

        
        case CMD_SET_ACL_RULE_TBL_ADD:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetAclRuleTblAdd((u8)(paramext.p1), (u32)(paramext.p2), (u32)(paramext.p3));             
        }
            break;

        
        case CMD_SET_ACL_RATE_TBL_ADD:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetAclRateTblAdd((u8)(paramext.p1), (u32)(paramext.p2), (u32)(paramext.p3));             
        }
            break;
        
        case CMD_SET_QOS_BASE:
        {
            ret_value = macMT7530SetQosBase((u8)(unsigned long)(api_param.paramext_ptr));           
        }
            break;
            
        case CMD_SET_QOS_PRIO_QMAP:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetQosPrioQmap((u8)(paramext.p1), (u8)(paramext.p2));         
        }
            break;

        case CMD_SET_QOS_PRIO_TAGMAP:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetQosPrioTagmap((u8)(paramext.p1), (u8)(paramext.p2));           
        }
            break;

        
        case CMD_SET_QOS_PRIO_DSCPMAP:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetQosPrioDscpmap((u8)(paramext.p1), (u8)(paramext.p2));          
        }
            break;

        case CMD_SET_QOS_PORT_PRIO:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetQosPortPrio((u8)(paramext.p1), (u8)(paramext.p2));         
        }
            break;

    
        case CMD_SET_QOS_PORT_WEIGHT:
        {
            mt7530_switch_api_qos_port_weight_t qosweight;
            if (copy_from_user(&qosweight, api_param.paramext_ptr, sizeof(qosweight)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetQosPortWeight(&qosweight);           
        }
            break;

            
        case CMD_SET_QOS_DSCP_PRIO:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetQosDscpPrio((u8)(paramext.p1), (u8)(paramext.p2));         
        }
            break;

        case CMD_GET_GSWR:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            macMT7530GetGSWRead(paramext.p1, &paramext.p2);
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }
        }
        break;
            
        case CMD_SET_GSWW:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetGSWWrite((u32)(paramext.p1), (u32)(paramext.p2));          
        }
        break;
            
        case CMD_GET_GSWR2:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            macMT7530GetGSWRead2(paramext.p1, paramext.p2, &paramext.p3);
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }
        }
        break;

        case CMD_SET_GSWW2:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetGSWWrite2((u32)(paramext.p1), (u32)(paramext.p2), (u32)(paramext.p3));         
        }
        break;

        case CMD_MIIR:
        {
            mt7530_switch_config_t paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530Getmiir(&paramext); 
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }
        }
            break;
            
        case CMD_MIIW:
        {
            mt7530_switch_config_t paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530Setmiiw(&paramext);  
        }
            break;
            
        case CMD_MIIR2:
        {
            mt7530_switch_config_t paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530Getmiir2(&paramext);
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }
        }
            break;
            
        case CMD_MIIW2:
        {
            mt7530_switch_config_t paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530Setmiiw2(&paramext);          
        }
            break;
			
	case CMD_EMIIR:
        {
            mt7530_switch_api_paramext4 paramext;
             if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530GetEmiir(&paramext); 
            if (copy_to_user(api_param.paramext_ptr, &paramext, sizeof(paramext)))
            {
                ret_value = -1;
            }   
        }
            break;
			
	case CMD_EMIIW:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetEmiiw(&paramext);          
        }
            break;
			
        case CMD_SET_VIP_PKT:
        {
    
            mt7530_switch_vip_info vip;
            if (copy_from_user(&vip, api_param.paramext_ptr, sizeof(vip)))
            {
                ret_value = -1;
                goto err_exit;
            }
            macMT7530SetVipPkt(vip);
        }   
            break;
      
        case CMD_SET_GSW_SETETHPORTMAP:
        {
            mt7530_switch_eth_portmap_t eth_portmap;
            if(copy_from_user(&eth_portmap, api_param.paramext_ptr, sizeof(eth_portmap)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetPortMap(eth_portmap.wan_port, eth_portmap.lan_portmap);
        }
        break;
        case CMD_GET_GSW_GETETHPORTMAP:
		{		
			
		   if (copy_to_user(api_param.paramext_ptr, lan_port_map, MAX_ETH_ITF_NUM))
            {
                ret_value = -1;
            }			
			
		}
		break;
        case CMD_WHITE_LIST_MODE:
        {
            ret_value = macMT7530ctagWhiteListMode((u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_WHITE_LIST_PER_PORT_MODE:
        {
            ret_value = macMT7530ctagWhiteListPerPortMode(portId,(u8)(unsigned long)(api_param.paramext_ptr));
        }
        break;
        
        case CMD_WHITE_LIST_ADD:
        {
            ret_value = macMT7530ctagWhiteListAdd(portId,(u16)(unsigned long)(api_param.paramext_ptr));
        }
        break;
        
        case CMD_WHITE_LIST_DEL:
        {
            ret_value = macMT7530ctagWhiteListDel(portId,(u16)(unsigned long)(api_param.paramext_ptr));
        }
        break;

        case CMD_GET_PORT_JABBER :
        {
            ret_value = macMT7530GetPortJabber(portId);
        }
        break;

        case CMD_GET_PORT_REMOTE_FAULT :
        {
            ret_value = macMT7530GetPortRemoteFault(portId);
        }
        break;

        case CMD_GET_PORT_MDI_CROSSOVER :
        {
            ret_value = macMT7530GetPortMDICrossover(portId);
        }
        break;

        case CMD_SET_MUL_IPTBL_ADD:
        {
            mt7530_switch_arl_mul_iptbladd_t mul_iptbladd;
            if (copy_from_user(&mul_iptbladd, api_param.paramext_ptr, sizeof(mul_iptbladd)))
            {
                ret_value = -1;
                goto err_exit;
            }
            
            ret_value = macMT7530SetMulArlIpTblAdd(&mul_iptbladd);
        }
        break;        

        case CMD_SET_ARL_DIPTBL_CLEAN_BY_DIP:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530CleanSpecifiedArlDipTblByDip((u32)(paramext.p1), (u32)(paramext.p2));         
        }
        break;

        case CMD_SET_ARL_SIPTBL_CLEAN_BY_DIP:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530CleanMultipleArlSipTblByDip((u32)(paramext.p1), (u32)(paramext.p2));         
        }
        break;

        case CMD_SET_ARL_SIPTBL_CLEAN_BY_SIP:
        {
            mt7530_switch_api_paramext2 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530CleanMultipleArlSipTblBySip((u32)(paramext.p1), (u32)(paramext.p2));         
        }
        break;

        case CMD_SET_ARL_SIPTBL_CLEAN_BY_DIPSIP:
        {
            mt7530_switch_api_paramext4 paramext;
            if (copy_from_user(&paramext, api_param.paramext_ptr, sizeof(paramext)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530CleanSpecifiedArlSipTblByDipSip((u32)(paramext.p1), (u32)(paramext.p2), (u32)(paramext.p3));         
        }
        break;
        case CMD_GET_WOL_STATUS:
        {
       		mt7530_switch_api_wol_status_t wol_status; 
			ret_value = macMT7530GetWOLStatus(&wol_status);
            if (ret_value == -1) 
            {
                goto err_exit;
            }
            if (copy_to_user(api_param.paramext_ptr, &wol_status, sizeof(wol_status)))
            {
                ret_value = -1;
            }
        }
        break;
        case CMD_SET_WOL_CONFIG:
        {
            mt7530_switch_api_wol_config_t wol_config;
            if (copy_from_user(&wol_config, api_param.paramext_ptr, sizeof(wol_config)))
            {
                ret_value = -1;
                goto err_exit;
            }
            ret_value = macMT7530SetWOL(&wol_config);         
        }
        break;
        default :
            break;
    }

err_exit:
    api_param.ret_value = ret_value;
    copy_to_user(ifr->ifr_data, &api_param, sizeof(mt7530_switch_api_params));

    return ret_value;
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_DS_LIMIT)
/*
	macMT7530SetDsTrtcmEnable for data speeed limit
*/
int dslimit_macMT7530SetDsTrtcmEnable(u8 enable)
{
	u32 reg, value = 0;
	u32 GDMA1=0,GDMA2=0;
	int rule, i = 0;

	if (SWITCH_NOTSUPPORT_TRTCM)
		return 0;

	memset(&TrtcmAclSetting, 0xff, sizeof(TrtcmAclSetting));
	TrtcmAclSetting.enable = enable ? 1 : 0;

	reg = 0xBFB50500;
	value = read_reg_word(reg);
	GDMA1 = value;
	value = value & 0xFFFF0000 | 0x7777;
	write_reg_word(reg,value);

	reg = 0xBFB51500;
	value = read_reg_word(reg);
	GDMA2 = value;
	value = value & 0xFFFF0000 | 0x7777;
	write_reg_word(reg,value);

	if ( enable )
	{		
		/* enable internal switch port 6 ACL, and set 32 acl rule */
		reg = 0x2604 ;
		value = read_reg_word(GSW_BASE+reg);
		value = (value | (1 << 10));
		write_reg_word(GSW_BASE+reg, value);

		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			/* match UPRI */
			value = 0x70000000 + (rule << 12);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x8400c;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x80005000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}

		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			value = (1 << rule);
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x0;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x80009000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}

		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			if (rule < 8)
				value = 0x18000000 + (rule << 24) + (rule << 4);
			else
				value = 0x10000000 ;

			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value=0;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x8000b000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}		

		/* ACL Rate value set to default 0xFFFF */
		for (rule = 0; rule < TRTCM_RULE_NUM; rule++)
		{
			reg = 0x94;
			value = 0x3FFF;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x98;
			value = 0x0;
			write_reg_word(GSW_BASE+reg, value);

			reg = 0x90;
			value = 0x8000d000 + rule;
			write_reg_word(GSW_BASE+reg, value);
		}
	}
	else
	{
		/* undo internal switch ACL rule */
		reg = 0x2604 ;
		value = read_reg_word(GSW_BASE+reg);
		value &= (~(1 << 10));
		write_reg_word(GSW_BASE+reg, value);
	}

	reg = 0xBFB50500;
	write_reg_word(reg,GDMA1);

	reg = 0xBFB51500;
	write_reg_word(reg,GDMA2);

	return 0;
}

/*
   macMT7530SetDsTrtcm for data speeed limit
 */
int dslimit_macMT7530SetDsTrtcm(mt7530_switch_api_trtcm_t *trtcm_p)
{
	u32 reg, value = 0;
	u32 GDMA1=0,GDMA2=0;

	if (SWITCH_NOTSUPPORT_TRTCM)
		return 0;

	if (trtcm_p == NULL)
		return -1;

	if (trtcm_p->trtcmId >= TRTCM_RULE_NUM)
		return -1;

	reg = 0xBFB50500;
	value = read_reg_word(reg);
	GDMA1 = value;
	value = value & 0xFFFF0000 | 0x7777;
	write_reg_word(reg,value);

	reg = 0xBFB51500;
	value = read_reg_word(reg);
	GDMA2 = value;
	value = value & 0xFFFF0000 | 0x7777;
	write_reg_word(reg,value);

	TrtcmAclSetting.trtcm_acl_table[trtcm_p->trtcmId].trtcmId = trtcm_p->trtcmId;
	TrtcmAclSetting.trtcm_acl_table[trtcm_p->trtcmId].CIR = trtcm_p->CIR;

	reg = 0x94;
	if(trtcm_p->CIR == 0x3FFF)
		value = trtcm_p->CIR;
	else
		value = 0x8000 | (trtcm_p->CIR);
	write_reg_word(GSW_BASE+reg, value);

	reg = 0x98;
	value = 0;
	write_reg_word(GSW_BASE+reg, value);

	reg = 0x90;
	value = 0x8000D000 + trtcm_p->trtcmId;
	write_reg_word(GSW_BASE+reg, value);

	reg = 0xBFB50500;
	write_reg_word(reg,GDMA1);

	reg = 0xBFB51500;
	write_reg_word(reg,GDMA2);

	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_JOYME_BANDWIDTH) || defined(TCSUPPORT_CF_JOYMEV2_PON) || defined(TCSUPPORT_XPON_HAL_API_EXT)	
void disableACLOfSwitchPort(void){
	u32 reg, value = 0;
	
	/* undo internal switch ACL rule */
    reg = 0x2004 ;
    value = macMT7530ReadReg(reg);
    value &= (~(1 << 10));
    macMT7530WriteReg(reg, value);

	reg = 0x2104 ;
    value = macMT7530ReadReg(reg);
    value &= (~(1 << 10));
    macMT7530WriteReg(reg, value);
		
	reg = 0x2204 ;
    value = macMT7530ReadReg(reg);
    value &= (~(1 << 10));
    macMT7530WriteReg(reg, value);
	
	reg = 0x2304 ;
    value = macMT7530ReadReg(reg);
    value &= (~(1 << 10));
    macMT7530WriteReg(reg, value);

	reg = 0x2404 ;
    value = macMT7530ReadReg(reg);
    value &= (~(1 << 10));
    macMT7530WriteReg(reg, value);
	
	reg = 0x2504 ;
    value = macMT7530ReadReg(reg);
    value &= (~(1 << 10));
    macMT7530WriteReg(reg, value);
		
	reg = 0x2604 ;
    value = macMT7530ReadReg(reg);
    value &= (~(1 << 10));
    macMT7530WriteReg(reg, value);
}

void enalbeACLOfSwitchPort(int up_down){	
	u32 reg, value = 0;
	
	switch(up_down){
		case ACL_MAC_UP_STREAM:
			/* enable internal switch port 0~4 ACL for 25F*/
			reg = 0x2004 ;
			value = macMT7530ReadReg(reg);
			value = (value | (1 << 10));
			macMT7530WriteReg(reg, value);
	
			reg = 0x2104 ;
			value = macMT7530ReadReg(reg);
			value = (value | (1 << 10));
			macMT7530WriteReg(reg, value);
	
			reg = 0x2204 ;
			value = macMT7530ReadReg(reg);
			value = (value | (1 << 10));
			macMT7530WriteReg(reg, value);
	
			reg = 0x2304 ;
			value = macMT7530ReadReg(reg);
			value = (value | (1 << 10));
			macMT7530WriteReg(reg, value);
	
			reg = 0x2404 ;
			value = macMT7530ReadReg(reg);
			value = (value | (1 << 10));
			macMT7530WriteReg(reg, value);

			/*port 5:for 7525G*/
			reg = 0x2504 ;
			value = macMT7530ReadReg(reg);
			value = (value | (1 << 10));
			macMT7530WriteReg(reg, value);
			break;
		case ACL_MAC_DOWN_STREAM:
			/* enable internal switch port 6 ACL*/
			reg = 0x2604 ;
			value = macMT7530ReadReg(reg);
			value = (value | (1 << 10));
			macMT7530WriteReg(reg, value);
			break;
	}
	
	return;
}

void setACLMacRule(int rule, int up_down, unsigned int* dev_mac, int speed){	
	u32 reg, value = 0;
	int i = 0;
	int ruleIndex = 0;
	int tableIndex = 0;

	if(up_down == ACL_MAC_UP_STREAM){
		ruleIndex = rule*2 + 1; 						/*eg.entry index: 1,3,5,7,9...*/
	}else{
		ruleIndex = rule*2; 							/*eg.entry index: 0,2,4,6,8.*/
	}

	/* 1.set ACL table entry, match dst mac need 3 tables*/
	for (i = 0; i < ACL_MAC_MATCH_TABLE_NUM; i++){
		reg = 0x94;
		value = 0xffff0000 + (dev_mac[i*2]<<8) + dev_mac[i*2+1];			//match mac 
		macMT7530WriteReg(reg, value);

		reg = 0x98;
		if(up_down == ACL_MAC_DOWN_STREAM){
			value = 0x84000+i*2;									//dst mac
		}else{
			value = 0x81f06+i*2;									//src mac
		}
		macMT7530WriteReg(reg, value);

		reg = 0x90;	
		tableIndex = ruleIndex*3 + i; 					
		value = 0x80005000 + tableIndex;	
		macMT7530WriteReg(reg, value);
	}


	
	/* 2.set ACL mask entry,  need match 3 tables*/
	reg = 0x94;
	value = (1 << ruleIndex*3) + (1 << (ruleIndex*3+1)) + (1 << (ruleIndex*3+2));		//match 3 rules	
	macMT7530WriteReg(reg, value);

	reg = 0x98;
	value = 0x0;	
	macMT7530WriteReg(reg, value);

	reg = 0x90;
	value = 0x80009000 + ruleIndex;							//acl mask Entry
	macMT7530WriteReg(reg, value);


	/* 3.set ACL Control entry*/
	reg = 0x94;
	if (ruleIndex < 8)										//only support 8 ACL Event Counter
		value = 0x18000000 + (ruleIndex << 24);			//enable ACL hit count, and set counter group index
	else
		value = 0x10000000 ;	
	macMT7530WriteReg(reg, value);

	reg = 0x98;
	value=0;
	macMT7530WriteReg(reg, value);

	reg = 0x90; 										//ACL rule control entry
	value = 0x8000b000 + ruleIndex;	
	macMT7530WriteReg(reg, value);


	/* 4.set ACL rate entry,  need match 3 tables*/
	reg = 0x94;
	value = 0x8000 | speed;	
	macMT7530WriteReg(reg, value);
	
	reg = 0x98;
	value = 0x0;
	macMT7530WriteReg(reg, value);

	reg = 0x90;
	value = 0x8000d000 + ruleIndex;	
	macMT7530WriteReg(reg, value);


	enalbeACLOfSwitchPort(up_down);
	
}

void clearACLMacRule(void)
{
	u32 reg, value = 0;
	int i = 0;

	for (i = 0; i < (((TRTCM_MATCH_MAC_RULE_NUM-1)*2 + 1)*3 + ACL_MAC_MATCH_TABLE_NUM); i++){
		reg = 0x94;
		value = 0x0;
		macMT7530WriteReg(reg, value);

		reg = 0x98;
		value = 0x0;
		macMT7530WriteReg(reg, value);

		reg = 0x90; 				
		value = 0x80005000 + i;    
		macMT7530WriteReg(reg, value);
	}

	for(i = 0; i < TRTCM_MATCH_MAC_RULE_NUM; i++){
		reg = 0x94;
		value = 0x0;
		macMT7530WriteReg(reg, value);

		reg = 0x98;
		value = 0x0;	
		macMT7530WriteReg(reg, value);

		reg = 0x90;
		value = 0x80009000 + i;
		macMT7530WriteReg(reg, value);

		reg = 0x94;
		value = 0x0;
		macMT7530WriteReg(reg, value);

		reg = 0x98;
		value=0x0;
		macMT7530WriteReg(reg, value);

		reg = 0x90;
		value = 0x8000b000 + i; 
		macMT7530WriteReg(reg, value);

		reg = 0x94;
		value = 0x0; 
		macMT7530WriteReg(reg, value);

		reg = 0x98;
		value = 0x0;
		macMT7530WriteReg(reg, value);

		reg = 0x90;
		value = 0x8000d000 + i; 
		macMT7530WriteReg(reg, value);
	}

	return;
}

int macMT7530SetMacMatchACL(mt7530_switch_dev_speed_acl_t* dev_mac_trtcm_p)
{
	u32 value1 = 0, value2 = 0;
	u32 val1[4]={0}, val2[4]={0};
    u32 GDMA1=0,GDMA2=0;
	int rule, i = 0;

	/*discard all packet before set ACL rule*/
	FE_API_GET_FW_CFG(FE_GDM_SEL_GDMA1, &value1);
	FE_API_GET_FW_CFG(FE_GDM_SEL_GDMA2, &value2);

	for(i = 0; i < 4; i++)
	{
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA1, i, FE_DP_DROP);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, i, FE_DP_DROP);
	}
	
    clearACLMacRule();

	printk("macMT7530SetMacMatchACL:enable=%d\n",dev_mac_trtcm_p->enable);

	if (dev_mac_trtcm_p->enable)
	{		
		for (rule = 0; rule < TRTCM_MATCH_MAC_RULE_NUM; rule++){
			if(dev_mac_trtcm_p->trtcm_acl_mac_table[rule].ds_queue_speed != 0){
				/*need set downstreams ACL rule*/
				setACLMacRule(rule,ACL_MAC_DOWN_STREAM,dev_mac_trtcm_p->trtcm_acl_mac_table[rule].dev_mac,dev_mac_trtcm_p->trtcm_acl_mac_table[rule].ds_queue_speed);
			}

			if(dev_mac_trtcm_p->trtcm_acl_mac_table[rule].us_queue_speed != 0){
				/*need set upstreams ACL rule*/				
				setACLMacRule(rule,ACL_MAC_UP_STREAM,dev_mac_trtcm_p->trtcm_acl_mac_table[rule].dev_mac,dev_mac_trtcm_p->trtcm_acl_mac_table[rule].us_queue_speed);
			}
		}
	}
	else
	{
		disableACLOfSwitchPort();
	}


	/*restoe GDMA1 & GMDA2 register*/
	for(i = 0; i < 4; i++)
	{
		val1[i] = value1 & (0xf << (i*4));
		val1[i] = val1[i] >> (i*4);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA1, i, val1[i]);
		
		val2[i] = value2 & (0xf << (i*4));
		val2[i] = val2[i] >> (i*4);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, i, val2[i]);
	}

	return 0;
}
#endif/*TCSUPPORT_COMPILE*/

/*___________________________________________________________________
**      function name: macMT7530GetPortRateLimit
**      descriptions:
**         get switch port rate limit
**
**      parameters:
**		port: switch port index
**		dir:	0:rx
**			1:tx
**		rate:  rate limit
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**_______________________________________________________________
*/
int macMT7530GetPortRateLimit(u8 port, u32 dir, u32 *rate)
{
	u32 reg, value;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (port >= MT7530_TOTAL_PORTS) return (-1);

	if (dir==1){			// ingress
		reg = 0x1800 + port*0x100;
	} else if (dir==0) {	// exgress
		reg = 0x1040 + port*0x100;
	} else {
		return -1;
	}
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	if (macMT7530_APIDbgDumpEn) 
		printk("%s() %2d (%02X) : %08X\n", __FUNCTION__, port, reg, value);

	if ((value & (1 << 7)) == 0) return (-1);		// TB_EN=0 : not bucket mode

	(*rate) = ((value >> 16) & 0xFFFF);
	if (value & (1 << 12)) (*rate) |= (1 << 16);	// cir register bit 16 is separated
	(*rate) <<= 5;					// cir register in unit of 32768(=2^15) bps; but function parameter is given as kbps
	
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetPortRateLimit
**      descriptions:
**         set switch port rate limit
**
**      parameters:
**		port: switch port index
**		dir:	0:rx
**			1:tx
**		rate:  rate limit
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**_______________________________________________________________
*/
int macMT7530SetPortRateLimit(u8 port, u32 rate, u8 dir)
{
	u32 reg, val, enable, tick;
	u32 extsw_flag = 0;

	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
	if (port >= MT7530_TOTAL_PORTS) return (-1);

	if (dir==1){			// ingress
		reg = 0x1FF0;
		val = macMultiMT7530ReadReg(reg, extsw_flag)& 0xFFFFFF00;
		val |= 0x18;
		macMultiMT7530WriteReg(reg, val, extsw_flag);
		
		reg = 0x1800 + port*0x100;
	} else if (dir==0) {	// exgress
		reg = 0x10E0;
		val = macMultiMT7530ReadReg(reg, extsw_flag)& 0xFFFFFF00;
		val |= 0x18;
		macMultiMT7530WriteReg(reg, val, extsw_flag);
		
		reg = 0x1040 + port*0x100;
	} else {
		return -1;
	}

	if (rate == 0)
	{
		enable = 0;
#if defined(TCSUPPORT_FWC_ENV)
		/*if set GE port ratelimit , set the flag ,so GE will go QDMA not GDM1*/
		if( GE_PORT == port )/*GE-port*/
		{
			eth_set_ge_portlimit_switch(DISABLE);
		}
#endif
	}
	else
	{
		enable = 1;
#if defined(TCSUPPORT_FWC_ENV)
		/*if set GE port ratelimit , set the flag ,so GE will go QDMA not GDM1*/
		if( GE_PORT == port )/*GE-port*/
		{
			eth_set_ge_portlimit_switch(ENABLE);
		}
#endif
	}

    if(rate > 10000)
    {
		tick = 2;
		//  7526FCU use clock 400MHz for tick between than 1/128ms ~ 1/2ms
		// so rate need to recalculated by rate * 500 / 400
		// When tick=2, need cir rate bit[2:0] always 0
		if(isEN7526FC_U2)
		{
			rate = rate*5/4;
			rate = ((rate/32)&0xfff8)<<16;
		}
		else
			rate = (rate/32)<<16;
    }
    else
    {
		tick = 7;
		rate = (rate/32)<<16;
    }

	val = (rate) + (enable<<15) + (tick<<8) + (1<<7) + 0xf; /* modify rate measurement period to 2:1/32ms, change ticker source from sw_2x_clk to sw_ticker_gen */

	macMultiMT7530WriteReg(reg, val, extsw_flag);
	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530SetDsPortQos
**      descriptions:
**         set switch port qos function enable
**
**      parameters:
**         0 : disable internal switch port qos feature
**         1 : enable internal switch port qos feature, default sp mode
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**_______________________________________________________________
*/
int macMT7530SetDsPortQos(u8 port, u8 enable)
{
	u32 reg=0, value=0;
	int queue = 0, rule = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	if (enable)
	{
		/* internal switch port  scheduler method is SP */
		for (queue=0; queue<8; queue++)
		{
			reg = 0x1000 + port*0x100 + queue*8;
			value = macMultiMT7530ReadReg(reg, extsw_flag);
			value &= (0x0);
			value |= (1<<31);
			macMultiMT7530WriteReg(reg, value, extsw_flag);

			reg = 0x1004 + port*0x100 + queue*8;
			value = macMultiMT7530ReadReg(reg, extsw_flag);
			value &= (0x0);
			macMultiMT7530WriteReg(reg, value, extsw_flag);			
		}

		/* set qos base as stag-based */
		reg = 0x44;
		value = macMultiMT7530ReadReg(reg, extsw_flag);
		value &= (~((0x7<<0) | (0x7<<4) | (0x7<<8) | (0x7<<12) | (0x7<<16) | (0x7<<20)));
		value |= ((0x2<<0) | (0x7<<4) | (0x2<<8) | (0x2<<12) | (0x2<<16) | (0x2<<20));
		macMultiMT7530WriteReg(reg, value, extsw_flag);
	}
	else
	{
		/* restore internal switch port scheduler method*/
		for (queue=0; queue<8; queue++)
		{
			reg = 0x1000 + port*0x100 + queue*8;
			value = macMultiMT7530ReadReg(reg, extsw_flag);
			value &= (0x0);
			macMultiMT7530WriteReg(reg, value, extsw_flag);
		
			reg = 0x1004 + port*0x100 + queue*8;
			value = macMultiMT7530ReadReg(reg, extsw_flag);
			
			value &= (0x0);
			macMultiMT7530WriteReg(reg, value, extsw_flag); 		
		}

		/* restore qos base */
		reg = 0x44;
		value = macMultiMT7530ReadReg(reg, extsw_flag);			
		value &= (~((0x7<<0) | (0x7<<4) | (0x7<<8) | (0x7<<12) | (0x7<<16) | (0x7<<20)));
		value |= ((0x7<<0) | (0x6<<4) | (0x5<<8) | (0x4<<12) | (0x3<<16) | (0x2<<20));
		macMultiMT7530WriteReg(reg, value, extsw_flag);		
	}

	return (0);
}

/*___________________________________________________________________
**      function name: macMT7530Setqueuemark
**      descriptions:
**         set switch  queue mark
**
**      parameters:
**         mt7530_switch_queue_info paramex
**         paramex.enable : 1: set switch queue by skb->mark   0: disable this function  
**         paramex.shift : the start bit of skb->mark which express queue
**         paramex.width: the queue mark width 
**
**      global:
**
**      return:
**
**      call:
**
**      revision:
**_______________________________________________________________
*/

int macMT7530Setqueuemark(mt7530_switch_queue_info paramext)
{
		unsigned int value = 0;
		int i = 0;
		
		queuemark_enable = paramext.enable;
		queuemark_shift = paramext.shift;
		queuemark_width = paramext.width;

		if(queuemark_enable)
		{
			value = switch_reg_read(GSW_BASE+0x3600);
			p6_fc_status = value;
	
			value &= ~0x30;
			switch_reg_write(GSW_BASE+0x3600, value);
	
			for(i = 0;i < 4;i++)
				macMT7530SetDsPortQos(i, 1);
		}
		else
		{
			if(p6_fc_status)
			{
				switch_reg_write(GSW_BASE+0x3600, p6_fc_status);
				p6_fc_status = 0;
			}

			  for(i = 0;i < 4;i++)
				macMT7530SetDsPortQos(i, 0);
		}
#if defined(TCSUPPORT_RA_HWNAT_ENHANCE_HOOK)
		if (ra_sw_nat_hook_clean_table)
		{
			ra_sw_nat_hook_clean_table();
		}
#endif
	
		return 0;

}

/*___________________________________________________________________
**      function name: macMT7530SetMACTxRx
**      descriptions:
**         set switch mac tx/rx enable or disable
**
**      parameters:
**         port : 
**         enable : 
**
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**_______________________________________________________________
*/

int macMT7530SetMACTxRx(u8 port, u32 enable, u32 direction)
{
    u32 reg=0, value=0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

    value = macMultiMT7530ReadReg(GSW_PMCR(port)-GSW_BASE, extsw_flag);

    if(enable)
    {
        if(direction == MAC_TX)
        {
            value |= 0x4000;    /* enable mac Tx */
        }
        else
        {
            value |= 0x2000;    /* enable mac Rx */
        }
        
    }
    else
    {
        if(direction == MAC_TX)
        {
            value &= ~0x4000;    /* disable mac Tx*/
        }
        else
        {
            value &= ~0x2000;    /* disable mac Rx */
        }
    }
    macMultiMT7530WriteReg(GSW_PMCR(port)-GSW_BASE, value, extsw_flag);
    return 0;
}


int macMT7530LanLedControl(u8 mode)
{
    u32 reg=0, value=0;
    
    switch(mode)
    {
        /*led on/off*/
        case LAN_LED_OFF:
        case LAN_LED_ON:
            /*set led pin as gpio mode*/
            reg = 0x7D04;
            value = 0x66666;
            macMT7530WriteReg(reg, value);
            #if defined(TCSUPPORT_MULTI_SWITCH_EXT)
            if (isEN7526G)
            {
                macMultiMT7530WriteReg(reg, value, 1);
            }
            #endif
            /*set gpio mode as output mode*/
            reg = 0x7D10;
            value = 0x77777;
            macMT7530WriteReg(reg, value);
            #if defined(TCSUPPORT_MULTI_SWITCH_EXT)
            if (isEN7526G)
            {
                macMultiMT7530WriteReg(reg, value, 1);
            }
            #endif
            /*set output enable*/
            reg = 0x7D14;
            value = 0x77777;
            macMT7530WriteReg(reg, value);
            #if defined(TCSUPPORT_MULTI_SWITCH_EXT)
            if (isEN7526G)
            {
                macMultiMT7530WriteReg(reg, value, 1);
            }
            #endif
            /*turn on/off*/
            reg = 0x7D18;
            if( mode == 1)
                value = 0x66666;
            else
                value = 0x77777;
            macMT7530WriteReg(reg, value);
            #if defined(TCSUPPORT_MULTI_SWITCH_EXT)
            if (isEN7526G)
            {
                macMultiMT7530WriteReg(reg, value, 1);
            }
            #endif
            break;
        /*led recover*/
        case LAN_LED_RECOVER:
            /*set led pin as gpio mode*/
            reg = 0x7D04;
            value = 0x77777;
            macMT7530WriteReg(reg, value);
            #if defined(TCSUPPORT_MULTI_SWITCH_EXT)
            if (isEN7526G)
            {
                macMultiMT7530WriteReg(reg, value, 1);
            }
            #endif
            break;
        default:
            break;
    }
    return 0;
}

int stag_index[7][8] = {0};

int get_stagindex_by_vid(unsigned int vid)
{
    int i = 0, j = 0, flag = 0;
    
    for(i = 0; i < 8; i++)      //index
    {
        for(j = 0; j <7; j++)   //port
        {
            if(stag_index[j][i] == vid)
            {
                return i;
            }
        }
    }

    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 7; j++)
        {
            if(stag_index[j][i] != 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
            return i;
        else
            flag = 0;
    }

    return -1;
    
}

int get_stagindex_by_vid_port(unsigned int vid, unsigned char port_id)
{
    int i = 0;
    for(i = 0; i < 8; i++){
        if(stag_index[port_id][i] == vid)
            return i;
    }
    return -1;
}

int port_all_reset_by_index(int index)
{
    int i = 0;
    for(i = 0; i < 7; i++)
    {
        if(stag_index[i][index] != 0)
            return 0;
    }
    return 1;
}

int macMT7530SetPerVlanAction(unsigned char port_id, unsigned int o_vid, unsigned int n_vid, ECNT_SWITCH_VLAN_MODE vlan_mode, unsigned char enable)
{
    u32 reg=0, value=0, vawd1, vawd2, p_id = 0;
    int index = -1;

    if(enable)
    {
        index = get_stagindex_by_vid(o_vid);

        if(index == -1)
        {
            printk("error:index used up!\n");
            return -1;
        }
        else
        {
            stag_index[port_id][index] = o_vid;
        }

        reg = GSW_PVC(6);
        value = switch_reg_read(reg);
        value = (value & ~EG_TAG_MASK);  // disable egtag of port6 when 1:N
        switch_reg_write(reg, value);
    
        reg = VTCR; 
        value = (0x80000000 + o_vid);
        switch_reg_write(reg, value);

        reg = VTCR;
        while (1) {
            value = switch_reg_read(reg);
            if ((value & 0x80000000) == 0) { //table busy
                break;
            }
        }
        
        reg = VAWD1;
        vawd1 = switch_reg_read(reg);
        
        reg = VAWD2;
        vawd2 = switch_reg_read(reg);
        
        reg = VAWD1;
        vawd1 |= 0x90000001;     //bit31(PORT_STAG), bit28(VLAN_EN), bit0(VALID) as 1;
        vawd1 |= (index << 4);
        vawd1 &= ~(1 << 29);    //bit 29(EG_CON) as 0
        switch_reg_write(reg, vawd1);

        reg = VAWD2;
        vawd2 &= ~(0x3 << (port_id << 1));
        vawd2 |= (vlan_mode << (port_id << 1));
        for(p_id = 0; p_id < 7; p_id ++){
            if(stag_index[p_id][index] == 0)
            {
                vawd2 &= ~(0x3 << (p_id << 1));
                vawd2 |= (0x2 << (p_id << 1));
            }
        }
        switch_reg_write(reg, vawd2);   //egress tag control enable
        
        reg = VTCR;
        value = (0x80001000 + o_vid);
        switch_reg_write(reg, value);
        
        reg = GSW_STAG(port_id, (index >> 1));
        value = switch_reg_read(reg);
        if(index & 0x1){
            value &= ~(0xfff << 12);
            value |= ((n_vid & 0xfff) << 12);
			if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
				value &= ~(0xf << 28);
				value |= ((n_vid & 0xf000) << 16);
			}		
        }
        else{
            value &= ~(0xfff);
            value |= (n_vid & 0xfff);
			if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
				value &= ~(0xf << 24);
				value |= ((n_vid & 0xf000) << 12);
			}
        }
        switch_reg_write(reg, value);        
    }
    else
    {
        index = get_stagindex_by_vid_port(o_vid, port_id);
        if(index >= 0)
        {
            stag_index[port_id][index] = 0;
        }
        else
        {
            return -1;
        }
        
        reg = GSW_PVC(6);
        value = switch_reg_read(reg);
        value = (value & ~EG_TAG_MASK) | EG_TAG_CONSISTENT;  // set egtag of port6 as consistent when 1:N
        switch_reg_write(reg, value);
        
        reg = VTCR;
        value = (0x80000000 + o_vid);
        switch_reg_write(reg, value);

        reg = VTCR;
        while (1) {
                value = switch_reg_read(reg);
                if ((value & 0x80000000) == 0) { //table busy
                    break;
            }
        }
        
        reg = VAWD1; 
        vawd1 = switch_reg_read(reg);

        reg = VAWD2;
        vawd2 = switch_reg_read(reg);
        vawd2 &= ~(0x3 << (port_id << 1));
        vawd2 |= (0x2 << (port_id << 1));
        if(port_all_reset_by_index(index))
        {
            printk("vlan all reset, return to consistent mode!\n");
            reg = VAWD1;
            vawd1 &= 0x6fff000f;     //bit31(PORT_STAG), bit28(VLAN_EN), stag[15:4] as 0;
            vawd1 |= (1 << 29);    //bit 29(EG_CON) as 1
            switch_reg_write(reg, vawd1);

            vawd2 = 0;
        }
        reg = VAWD2;
        switch_reg_write(reg, vawd2);   //egress tag control
        
        reg = VTCR;
        value = (0x80001000 + o_vid);
        switch_reg_write(reg, value);

        reg = GSW_STAG(port_id, (index >> 1));
        value = switch_reg_read(reg);
        if(index & 0x1){
            value &= ~(0xfff << 12);
			if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
				value &= ~(0xf << 28);		
			}
        }
        else{
            value &= ~(0xfff);
			if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
				value &= ~(0xf << 24);
			}
        }
        switch_reg_write(reg, value);
    }
    return 0;
}

int macMT7530SetPerPortVlanAction(unsigned char port_id, unsigned int n_vid, ECNT_SWITCH_VLAN_MODE vlan_mode, unsigned char enable)
{
    u32 reg=0, value=0, vawd1, vawd2, p_id = 0;;
    int i = 0, index = 0;

    if(enable)
    {
        stag_index[port_id][0]= 1;

        for(p_id = 0; p_id < 7; p_id++)
        {
            value = switch_reg_read(GSW_PCR(p_id));
            if(p_id == port_id){
                value &= ~(0x3 << 28);
                value |= (vlan_mode << 28); /*EG_TAG*/
            }
            else if(stag_index[p_id][0] == 0)
            {
                value &= ~(0x3 << 28);
                value |= (0x2 << 28); 
            }
            switch_reg_write(GSW_PCR(p_id), value);
        }
        
        for(i = 1; i <4096; i++)
        {            
            reg = VTCR;
            value = (0x80000000 + i);
            switch_reg_write(reg, value);

            reg = VTCR;
            while (1) {
                value = switch_reg_read(reg);
                if ((value & 0x80000000) == 0) { //table busy
                    break;
                }
            }
            reg = VAWD1;
            vawd1 = switch_reg_read(reg);
            
            reg = VAWD1;
            vawd1 |= 0x80000001;     //bit31(PORT_STAG), bit0(VALID) as 1;
            vawd1 &= ~(0x3 << 28);    //bit28(VLAN_EN), bit 29(EG_CON) as 0
            vawd1 &= ~(0xfff << 4);  //index as 0,
            switch_reg_write(reg, vawd1);
            
            reg = VTCR;
            value = (0x80001000 + i);
            switch_reg_write(reg, value); 
        }

        for(index = 0; index < 8 ; index++){
            reg = GSW_STAG(port_id, (index >> 1));
            value = switch_reg_read(reg);
            if(index & 0x1){
                value &= ~(0xfff << 12);
                value |= (n_vid << 12);
				if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
					value &= ~(0xf << 28);
					value |= ((n_vid & 0xf000) << 16);
				}
            }
            else{
                value &= ~(0xfff);
                value |= n_vid;
				if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
					value &= ~(0xf << 24);
					value |= ((n_vid & 0xf000) << 12);
				}
            }
            switch_reg_write(reg, value); 
        }
    }
    else
    {
        stag_index[port_id][0]= 0;
        value = switch_reg_read(GSW_PCR(port_id));
        value &= ~(0x3 << 28);  /*clear EG_TAG*/
        value |= (0x2 << 28);
        switch_reg_write(GSW_PCR(port_id), value);
        
        if(port_all_reset_by_index(0)){
            printk("port all reset, return to consistent mode.\n");
            for(p_id = 0; p_id < 7; p_id++){
                value = switch_reg_read(GSW_PCR(p_id));
                value &= ~(0x3 << 28);  /*clear EG_TAG*/
                switch_reg_write(GSW_PCR(p_id), value);
            }
            for(i = 1; i <4096; i++)
            {            
                reg = VTCR;
                value = (0x80000000 + i);
                switch_reg_write(reg, value);

                reg = VTCR;
                while (1) {
                    value = switch_reg_read(reg);
                    if ((value & 0x80000000) == 0) { //table busy
                        break;
                    }
                }
                reg = VAWD1;
                vawd1 = switch_reg_read(reg);
                
                reg = VAWD1;
                vawd1 &= 0x6fff000f;     //bit31(PORT_STAG), bit28(VLAN_EN), stag[15:4] as 0;
                vawd1 |= (1 << 29);      //bit 29(EG_CON) as 1
                switch_reg_write(reg, vawd1);
                
                reg = VTCR;
                value = (0x80001000 + i);
                switch_reg_write(reg, value);
            }
        }
        
        for(index = 0; index < 8 ; index++){
            reg = GSW_STAG(port_id, (index >> 1));
            value = switch_reg_read(reg);
            if(index & 0x1){
                value &= ~(0xfff << 12);
				if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
					value &= ~(0xf << 28);		
				}
            }
            else{
                value &= ~(0xfff);
				if(SUPPORT_VLAN_TABLE_PBIT_REMARK){
					value &= ~(0xf << 24);
				}
            }
            switch_reg_write(reg, value); 
        }
    }
    return 0;
}

int macMT7530SetArlAging(u8 aging_en, u32 time)
{
	u32 reg = 0, value = 0, aging_unit = 0, aging_cnt = 0;
	
	reg = 0xa0 + GSW_BASE;
	value = switch_reg_read(reg);
	value &= (~(1<<20));	// enable aging
	if (!aging_en){
		value |= (1<<20);	// disable aging
	}
	switch_reg_write(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = 0xa0;
        value = macMultiMT7530ReadReg(reg, 1);
        value &= (~(1<<20));    // enable aging
        if (!aging_en){
            value |= (1<<20);   // disable aging
        }
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif

	macMT7530SetAgeTime(time);
	return 0;
}

int macMT7530SetArlMacTblAdd(mt7530_switch_mactbladd_t *mactbladd)
{
	u32 value = 0, reg = 0;
		
	
	reg = REG_ATC_ADDR + GSW_BASE;
	while (1){	// wait until not busy
		value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	reg = REG_ATA1_ADDR + GSW_BASE;
	value = (mactbladd->mac[0] << 24) | (mactbladd->mac[1] << 16) | (mactbladd->mac[2] << 8) | mactbladd->mac[3];
	switch_reg_write(reg, value);

	reg = REG_ATA2_ADDR + GSW_BASE;
	value = (mactbladd->mac[4] << 24) | (mactbladd->mac[5] << 16);
	switch_reg_write(reg, value);

	//Check the input parameters is right or not.

	if (mactbladd->destportmap > REG_ATWD_PORT_RELMASK){
		printk("[%s]%d:arl mactbl-add <MacAddr> <DestPortMap>\n", __FUNCTION__, __LINE__);
		return -1;
	}

	// set other parameters
	reg = REG_ATWD_ADDR + GSW_BASE;
	value = 0;
	value |= (mactbladd->destportmap << REG_ATWD_PORT_OFFT);
	value |= (3 << REG_ATWD_STATUS_OFFT);		// 3 : Entry is static and won't aged out or changed by the hardware
	value |= (mactbladd->leaky_en << REG_ATWD_LEAKY_EN_OFFT) + (mactbladd->eg_tag << REG_ATWD_EG_TAG_OFFT) + (mactbladd->usr_pri << REG_ATWD_USR_PRI_OFFT);
	value |= (mactbladd->sa_mir_en << REG_ATWD_SA_MIR_EN_OFFT) + (mactbladd->sa_port_fw << REG_ATWD_SA_PORT_FW_OFFT);
	switch_reg_write(reg, value);

	// trigger
	reg = REG_ATC_ADDR + GSW_BASE;
	value = (1 << REG_ATC_BUSY_OFFT) + (0 << REG_ATC_AC_SAT_OFFT) + (1 << REG_ATC_AC_CMD_OFFT); // 3'b001 : Write command (single entry)
	switch_reg_write(reg, value);
	while (1){	// wait until not busy
		value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
	printk("Add one static MAC Address entry OK.\n");

	return 0;
}

int macMT7530IssearchedArlSipTblByDip(u32 is_extend_gsw, u32 dip){

	u32 value = 0, reg = 0;
	u32 addr1 = 0, addr2 = 0;
    int ret = 0;
    u8 port = 0, portNameIdx = 0;
    u32 ac_mat = 0x8;

    reg = REG_ATA2_ADDR;
    macMultiMT7530WriteReg(reg, dip, is_extend_gsw);
    
	reg = REG_ATC_ADDR;
	while (1){	// wait until not busy
		value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	// trigger 1st search
	reg = REG_ATC_ADDR;
	value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);	// 3'b100 : Start search command (reset to 1st entry)
	macMultiMT7530WriteReg(reg, value, is_extend_gsw);
	while (1){	// wait until not busy
        value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	while (1){
        if(value == 0)
            break;
		if ((value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
		{
			break;
		}

        if(value & REG_AT_SRCH_HIT_MASK)
        {
    		return 1;
        }

		// trigger next
		reg = REG_ATC_ADDR;
		value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT);	// 3'b101 : Next search command (next entry)
        macMultiMT7530WriteReg(reg, value, is_extend_gsw);
		while (1){	// wait until not busy
			value = macMultiMT7530ReadReg(reg, is_extend_gsw);
			if ((value & REG_ATC_BUSY_MASK) == 0){
				break;
			}
		}
	}

	return 0;
}    

int macMT7530CleanSpecifiedArlDipTblByDip(u32 is_extend_gsw, u32 dip){
	u32 reg, value;


	reg = REG_ATC_ADDR;
	while (1){	// wait until not busy
		value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
	
    reg = REG_ATA1_ADDR;
    macMultiMT7530WriteReg(reg, dip, is_extend_gsw);

    reg = REG_ATWD_ADDR;
    macMultiMT7530WriteReg(reg, 0, is_extend_gsw);
	
	// clean spacific dip address entry
	reg = REG_ATC_ADDR;
    value = (1 << REG_ATC_BUSY_OFFT) + (1 << REG_ATC_AC_SAT_OFFT) + (1 << REG_ATC_AC_CMD_OFFT); // 3'b001 : Write command (single entry)
	macMultiMT7530WriteReg(reg, value, is_extend_gsw);
	
	while (1){	// wait until not busy
	value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
    return 0;

}

int macMT7530CleanSpecifiedArlSipTblByDipSip(u32 is_extend_gsw, u32 dip, u32 sip){
    u32 reg, value;
    
	reg = REG_ATC_ADDR;
	while (1){	// wait until not busy
		value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
	
    reg = REG_ATA1_ADDR;
    macMultiMT7530WriteReg(reg, dip, is_extend_gsw);

    reg = REG_ATA2_ADDR;
    macMultiMT7530WriteReg(reg, sip, is_extend_gsw);

    
    reg = REG_ATWD_ADDR;
    macMultiMT7530WriteReg(reg, 0, is_extend_gsw);
    
	// clean spacific sip address entry
	reg = REG_ATC_ADDR;
    value = (1 << REG_ATC_BUSY_OFFT) + (2 << REG_ATC_AC_SAT_OFFT) + (1 << REG_ATC_AC_CMD_OFFT); // 3'b001 : Write command (single entry)
	macMultiMT7530WriteReg(reg, value, is_extend_gsw);

	while (1){	// wait until not busy
	    value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
    return 0;
}

int macMT7530CleanMultipleArlSipTblBySip(u32 is_extend_gsw, u32 sip){
    u32 reg, value;

	reg = REG_ATC_ADDR;
	while (1){	// wait until not busy
		value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
	
    reg = REG_ATA2_ADDR;
    macMultiMT7530WriteReg(reg, sip, is_extend_gsw);

    
	// clean all relative sip address entries based on the specified DIP from ATA2 register
	reg = REG_ATC_ADDR;
	value = (1 << REG_ATC_BUSY_OFFT) + (0x9 << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);	// 3'b010 : clean command (multiple entries)
	macMultiMT7530WriteReg(reg, value, is_extend_gsw);
	
	while (1){	// wait until not busy
	value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
    return 0;
}

int macMT7530CleanMultipleArlSipTblByDip(u32 is_extend_gsw, u32 dip){
    u32 reg, value;

	reg = REG_ATC_ADDR;
	while (1){	// wait until not busy
		value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
	
    reg = REG_ATA2_ADDR;
    macMultiMT7530WriteReg(reg, dip, is_extend_gsw);

    
	// clean all relative sip address entries based on the specified DIP from ATA2 register
	reg = REG_ATC_ADDR;
	value = (1 << REG_ATC_BUSY_OFFT) + (0x8 << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);	// 3'b010 : clean command (multiple entries)
	macMultiMT7530WriteReg(reg, value, is_extend_gsw);
	
	while (1){	// wait until not busy
	value = macMultiMT7530ReadReg(reg, is_extend_gsw);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}
    
    return 0;
}

int macMT7530SetArlDipTblAdd(mt7530_switch_arl_diptbladd_t *diptbladd){

    u32 value, reg;
    u32 is_extend_gsw = diptbladd->is_extend_gsw;

    reg = REG_ATC_ADDR;
    while (1){  // wait until not busy
        value = macMultiMT7530ReadReg(reg, is_extend_gsw);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }

    reg = REG_ATA1_ADDR;
    value = diptbladd->dip;
    macMultiMT7530WriteReg(reg, value, is_extend_gsw);

    //Check the input parameters is right or not.
	if ((diptbladd->destportmap > REG_ATWD_PORT_RELMASK) || (diptbladd->status > REG_ATWD_STATUS_RELMASK)){
		printk("[%s]%d:arl diptbl-add <DIP> <DestPortMap> error!\n", __FUNCTION__, __LINE__);
		return -1;
	}

    // set other parameters
    reg = REG_ATWD_ADDR;
    value = 0;
    value |= (diptbladd->destportmap << REG_ATWD_PORT_OFFT);
    value |= (diptbladd->status << REG_ATWD_STATUS_OFFT);
    value |= (diptbladd->leaky_en << REG_ATWD_LEAKY_EN_OFFT) + (diptbladd->eg_tag << REG_ATWD_EG_TAG_OFFT) + (diptbladd->usr_pri << REG_ATWD_USR_PRI_OFFT);
    macMultiMT7530WriteReg(reg, value, is_extend_gsw);

    // trigger
    reg = REG_ATC_ADDR;
    value = (1 << REG_ATC_BUSY_OFFT) + (1 << REG_ATC_AC_SAT_OFFT) + (1 << REG_ATC_AC_CMD_OFFT); // 3'b001 : Write command (single entry)
    macMultiMT7530WriteReg(reg, value, is_extend_gsw);
    while (1){  // wait until not busy
        value = macMultiMT7530ReadReg(reg, is_extend_gsw);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }
    
    //printk("Add one static DIP Address entry OK.\n");
    
    return 0;
}

int macMT7530SetArlSipTblAdd(mt7530_switch_arl_siptbladd_t *siptbladd){
    u32 value, reg;
    u32 is_extend_gsw = siptbladd->is_extend_gsw;

    reg = REG_ATC_ADDR;
    while (1){	// wait until not busy
        value = macMultiMT7530ReadReg(reg, is_extend_gsw);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }

    reg = REG_ATA1_ADDR;
    value = siptbladd->dip;
    macMultiMT7530WriteReg(reg, value, is_extend_gsw);

    reg = REG_ATA2_ADDR;
    value = siptbladd->sip;
    macMultiMT7530WriteReg(reg, value, is_extend_gsw);

    //Check the input parameters is right or not.
    if (siptbladd->destportmap > REG_ATWD_PORT_RELMASK){
        printk("[%s]%d:arl diptbl-add <DIP> <DestPortMap> error!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    // set other parameters
    reg = REG_ATWD_ADDR;
    value = 0;
    value |= (siptbladd->destportmap << REG_ATWD_PORT_OFFT);
    value |= (3 << REG_ATWD_STATUS_OFFT);   // 3 : Entry is static and won't aged out or changed by the hardware
    macMultiMT7530WriteReg(reg, value, is_extend_gsw);

    // trigger
    reg = REG_ATC_ADDR;
    value = (1 << REG_ATC_BUSY_OFFT) + (2 << REG_ATC_AC_SAT_OFFT) + (1 << REG_ATC_AC_CMD_OFFT);	// 3'b001 : Write command (single entry)
    macMultiMT7530WriteReg(reg, value, is_extend_gsw);
    while (1){	// wait until not busy
        value = macMultiMT7530ReadReg(reg, is_extend_gsw);
        if ((value & REG_ATC_BUSY_MASK) == 0){
            break;
        }
    }
    
    //printk("Add one static SIP Address entry OK.\n");

    return 0;
}

int macMT7530SetMulArlIpTblAdd(mt7530_switch_arl_mul_iptbladd_t *multi_info){

    mt7530_switch_arl_siptbladd_t siptbladd;
    mt7530_switch_arl_diptbladd_t diptbladd;

    int port = 0;  
    int switchport = 0;

    memset(&siptbladd, 0, sizeof(mt7530_switch_arl_siptbladd_t));    
    memset(&diptbladd, 0, sizeof(mt7530_switch_arl_diptbladd_t));

    diptbladd.dip = multi_info->dip;
    diptbladd.status = ARL_IPTBL_STATUS_STATIC_BY_SIP_PORTMAP;
    diptbladd.destportmap = 0;
    diptbladd.is_extend_gsw = multi_info->is_extend_gsw;

    siptbladd.dip = multi_info->dip;
    siptbladd.sip = multi_info->sip;
    siptbladd.is_extend_gsw = multi_info->is_extend_gsw;

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    for(port = 0; port < 8; port++){
        switchport= macMT7530LanPortMap2SwitchForMulti(port);
        if((multi_info->is_extend_gsw)?(switchport == extend_switch_port):(switchport != extend_switch_port)){
            if(multi_info->sw_mask & (1 << port))
                siptbladd.destportmap |= (1 <<macMT7530LanPortMap2Switch(port));
        }
    }
#else
    for(port = 0; port < 4; port++){
        switchport= macMT7530LanPortMap2SwitchForMulti(port);
            if(multi_info->sw_mask & (1 << port))
                siptbladd.destportmap |= (1 <<macMT7530LanPortMap2Switch(port));
    }
#endif

    if(siptbladd.destportmap){
        macMT7530SetArlDipTblAdd(&diptbladd);
        macMT7530SetArlSipTblAdd(&siptbladd);
    }
    else{
        macMT7530CleanSpecifiedArlSipTblByDipSip(siptbladd.is_extend_gsw, siptbladd.dip, siptbladd.sip);
        if(!macMT7530IssearchedArlSipTblByDip(multi_info->is_extend_gsw, multi_info->dip)){
            //printk("sip entry is no searched based on the specified ip(%08X)!.\n",multi_info->dip);
            macMT7530CleanSpecifiedArlDipTblByDip(diptbladd.is_extend_gsw, diptbladd.dip);
        }
    }

    return 0;
}
int macMT7530SetArlAddrTblDisp(int ac_mat)
{
	u32 value = 0, reg = 0;
	u32 addr1 = 0, addr2 = 0;
    int ret = 0;
    u8 port = 0, portNameIdx = 0;

	if (ac_mat > 3){
		return 0;
	}

	reg = REG_ATC_ADDR + GSW_BASE;
	while (1){	// wait until not busy
		value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	// trigger 1st search
	reg = REG_ATC_ADDR + GSW_BASE;
	value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);	// 3'b100 : Start search command (reset to 1st entry)
	switch_reg_write(reg, value);
	while (1){	// wait until not busy
		value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	while (1){
		if ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
		{
			printk("[%s]%d:...Address Table end...\n", __FUNCTION__, __LINE__);
			break;
		}
        if(value & REG_AT_SRCH_HIT_MASK)
        {
    		reg = REG_TSRA1_ADDR + GSW_BASE;
    		addr1 = switch_reg_read(reg);

    		reg = REG_TSRA2_ADDR + GSW_BASE;
    		addr2 = switch_reg_read(reg);

    		// other parameters
    		reg = REG_ATRD_ADDR + GSW_BASE;
    		value = switch_reg_read(reg);

            /* get PORT info*/
            ret = getPortByMask((value>>4)&0xFF, &port);
            if ( 0 != ret )
    				break;
            portNameIdx = getPortNameIdxByPortid(port);
            if(portNameIdx >= MAX_PORT_NAME_NUM)
                break;;
            
    		// ac_mat: 0(mac)/1(dip)/2(sip)
    		if (ac_mat == 0){
    			printk("[%s]%d:MAC %08X%04X :\n", __FUNCTION__, __LINE__, addr1, (addr2 >> 16));
    			printk("[%s]%d:TIMER:%d, SA_PORT_FW:%d, SA_MIR_EN:%d, USER_PRI:%d,\n", __FUNCTION__, __LINE__,
    			  (value>>24)&0xFF, (value>>20)&7, (value>>19)&1, (value>>16)&7);
    			printk("[%s]%d:EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
    			  (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
    		} else if (ac_mat == 1){
    			printk("[%s]%d:DIP %08X :\n", __FUNCTION__, __LINE__, addr1);
    			printk("[%s]%d:tmr:%d, flag:%X, cnt%X\n", __FUNCTION__, __LINE__, (addr2>>24)&0x0F, (addr2>>16)&0x0F, addr2&0xFF);
    			printk("[%s]%d:USER_PRI:%d, EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
    			  (value>>16)&7, (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
    		} else if (ac_mat == 2){
    			printk("[%s]%d:DIP %08X, SIP %08X :\n", __FUNCTION__, __LINE__, addr1, addr2);
    			printk("[%s]%d:%s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
    			  portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
    		}
        }
		// trigger next
		reg = REG_ATC_ADDR + GSW_BASE;
		value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT);	// 3'b101 : Next search command (next entry)
		switch_reg_write(reg, value);
		while (1){	// wait until not busy
			value = switch_reg_read(reg);
			if ((value & REG_ATC_BUSY_MASK) == 0){
				break;
			}
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        printk("[%s]%d:Ext Switch Address Table:\n", __FUNCTION__, __LINE__);
        
        reg = REG_ATC_ADDR;
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        
        // trigger 1st search
        reg = REG_ATC_ADDR;
        value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);    // 3'b100 : Start search command (reset to 1st entry)
        macMultiMT7530WriteReg(reg, value, 1);
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        
        while (1){
            if ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
            {
                printk("[%s]%d:...Address Table end...\n", __FUNCTION__, __LINE__);
                break;
            }
            if(value & REG_AT_SRCH_HIT_MASK)
            {
                reg = REG_TSRA1_ADDR;
                addr1 = macMultiMT7530ReadReg(reg, 1);
        
                reg = REG_TSRA2_ADDR;
                addr2 = macMultiMT7530ReadReg(reg, 1);
        
                // other parameters
                reg = REG_ATRD_ADDR;
                value = macMultiMT7530ReadReg(reg, 1);
        
                /* get PORT info*/
                ret = getPortByMask((value>>4)&0xFF, &port);
                if ( 0 != ret )
                        break;
                portNameIdx = getPortNameIdxByPortid(port);
                if(portNameIdx >= MAX_PORT_NAME_NUM)
                    break;;
                
                // ac_mat: 0(mac)/1(dip)/2(sip)
                if (ac_mat == 0){
                    printk("[%s]%d:MAC %08X%04X :\n", __FUNCTION__, __LINE__, addr1, (addr2 >> 16));
                    printk("[%s]%d:TIMER:%d, SA_PORT_FW:%d, SA_MIR_EN:%d, USER_PRI:%d,\n", __FUNCTION__, __LINE__,
                      (value>>24)&0xFF, (value>>20)&7, (value>>19)&1, (value>>16)&7);
                    printk("[%s]%d:EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
                      (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
                } else if (ac_mat == 1){
                    printk("[%s]%d:DIP %08X :\n", __FUNCTION__, __LINE__, addr1);
                    printk("[%s]%d:tmr:%d, flag:%X, cnt%X\n", __FUNCTION__, __LINE__, (addr2>>24)&0x0F, (addr2>>16)&0x0F, addr2&0xFF);
                    printk("[%s]%d:USER_PRI:%d, EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
                      (value>>16)&7, (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
                } else if (ac_mat == 2){
                    printk("[%s]%d:DIP %08X, SIP %08X :\n", __FUNCTION__, __LINE__, addr1, addr2);
                    printk("[%s]%d:%s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
                      portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
                }
            }
            // trigger next
            reg = REG_ATC_ADDR;
            value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT);    // 3'b101 : Next search command (next entry)
            macMultiMT7530WriteReg(reg, value, 1);
            while (1){  // wait until not busy
                value = macMultiMT7530ReadReg(reg, 1);
                if ((value & REG_ATC_BUSY_MASK) == 0){
                    break;
                }
            }
        }
    }
#endif

	return 0;
}


int macMT7530SetArlAddrTblDisp2(u32 ext_switch, int ac_mat)
{
	u32 value = 0, reg = 0;
	u32 addr1 = 0, addr2 = 0;
    int ret = 0;
    u8 port = 0, portNameIdx = 0;

	if (ac_mat > 3){
		return 0;
	}
	printk("[%s]%d:(ext_switch:%d)\r\n", __FUNCTION__, __LINE__, ext_switch);

	reg = REG_ATC_ADDR + GSW_BASE;
	while (1){	// wait until not busy
		value = switch_reg_read2(ext_switch, reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	// trigger 1st search
	reg = REG_ATC_ADDR + GSW_BASE;
	value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);	// 3'b100 : Start search command (reset to 1st entry)
	switch_reg_write2(ext_switch, reg, value);
	while (1){	// wait until not busy
		value = switch_reg_read2(ext_switch, reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	while (1){
		if ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK ) )
		{
			printk("[%s]%d:...Address Table end...\n", __FUNCTION__, __LINE__);
			break;
		}

        if(value & REG_AT_SRCH_HIT_MASK)
        {
    		reg = REG_TSRA1_ADDR + GSW_BASE;
    		addr1 = switch_reg_read2(ext_switch, reg);

    		reg = REG_TSRA2_ADDR + GSW_BASE;
    		addr2 = switch_reg_read2(ext_switch, reg);

    		// other parameters
    		reg = REG_ATRD_ADDR + GSW_BASE;
    		value = switch_reg_read2(ext_switch, reg);
    		printk("[%s]%d:reg dump 0x%08X, 0x%08X, 0x%04X\n", __FUNCTION__, __LINE__, addr1, addr2, value);

            /* get PORT info*/
            ret = getPortByMask((value>>4)&0xFF, &port);
            if ( 0 != ret )
                break;
            portNameIdx = getPortNameIdxByPortid(port);
            if(portNameIdx >= MAX_PORT_NAME_NUM)
                break;;
            
    		// ac_mat: 0(mac)/1(dip)/2(sip)
    		if (ac_mat == 0){
    			printk("[%s]%d:MAC %08X%04X :\n", __FUNCTION__, __LINE__, addr1, (addr2 >> 16));
    			printk("[%s]%d:TIMER:%d, SA_PORT_FW:%d, SA_MIR_EN:%d, USER_PRI:%d,\n", __FUNCTION__, __LINE__,
    			  (value>>24)&0xFF, (value>>20)&7, (value>>19)&1, (value>>16)&7);
    			printk("[%s]%d:EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
    			  (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
    		} else if (ac_mat == 1){
    			printk("[%s]%d:DIP %08X :\n", __FUNCTION__, __LINE__, addr1);
    			printk("[%s]%d:tmr:%d, flag:%X, cnt%X\n", __FUNCTION__, __LINE__, (addr2>>24)&0x0F, (addr2>>16)&0x0F, addr2&0xFF);
    			printk("[%s]%d:USER_PRI:%d, EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
    			  (value>>16)&7, (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
    		} else if (ac_mat == 2){
    			printk("[%s]%d:DIP %08X, SIP %08X :\n", __FUNCTION__, __LINE__, addr1, addr2);
    			printk("[%s]%d:%s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
    			  portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
    		}
        }

		printk("[%s]%d:reg dump 0x%08X, 0x%08X, 0x%04X\n", __FUNCTION__, __LINE__, addr1, addr2, value);

		// trigger next
		reg = REG_ATC_ADDR + GSW_BASE;
		value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT);	// 3'b101 : Next search command (next entry)
		switch_reg_write2(ext_switch, reg, value);
		while (1){	// wait until not busy
			value = switch_reg_read2(ext_switch, reg);
			if ((value & REG_ATC_BUSY_MASK) == 0){
				break;
			}
		}
	}
    
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        printk("[%s]%d:Ext Switch Address Table:\n", __FUNCTION__, __LINE__);
        
        reg = REG_ATC_ADDR;
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        
        // trigger 1st search
        reg = REG_ATC_ADDR;
        value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (4 << REG_ATC_AC_CMD_OFFT);    // 3'b100 : Start search command (reset to 1st entry)
        macMultiMT7530WriteReg(reg, value, 1);
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        
        while (1){
            if ( (value & REG_AT_SRCH_END_MASK) && ( ((value>> REG_AT_ADDR_OFFT) & REG_AT_ADDR_RELMASK)==MAC_TABLE_MASK) )
            {
                printk("[%s]%d:...Address Table end...\n", __FUNCTION__, __LINE__);
                break;
            }
            if(value & REG_AT_SRCH_HIT_MASK)
            {
                reg = REG_TSRA1_ADDR;
                addr1 = macMultiMT7530ReadReg(reg, 1);
        
                reg = REG_TSRA2_ADDR;
                addr2 = macMultiMT7530ReadReg(reg, 1);
        
                // other parameters
                reg = REG_ATRD_ADDR;
                value = macMultiMT7530ReadReg(reg, 1);
        
                /* get PORT info*/
                ret = getPortByMask((value>>4)&0xFF, &port);
                if ( 0 != ret )
                        break;
                portNameIdx = getPortNameIdxByPortid(port);
                if(portNameIdx >= MAX_PORT_NAME_NUM)
                    break;;
                
                // ac_mat: 0(mac)/1(dip)/2(sip)
                if (ac_mat == 0){
                    printk("[%s]%d:MAC %08X%04X :\n", __FUNCTION__, __LINE__, addr1, (addr2 >> 16));
                    printk("[%s]%d:TIMER:%d, SA_PORT_FW:%d, SA_MIR_EN:%d, USER_PRI:%d,\n", __FUNCTION__, __LINE__,
                      (value>>24)&0xFF, (value>>20)&7, (value>>19)&1, (value>>16)&7);
                    printk("[%s]%d:EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
                      (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
                } else if (ac_mat == 1){
                    printk("[%s]%d:DIP %08X :\n", __FUNCTION__, __LINE__, addr1);
                    printk("[%s]%d:tmr:%d, flag:%X, cnt%X\n", __FUNCTION__, __LINE__, (addr2>>24)&0x0F, (addr2>>16)&0x0F, addr2&0xFF);
                    printk("[%s]%d:USER_PRI:%d, EG_TAG:%d, LEAKY_EN:%d, %s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
                      (value>>16)&7, (value>>13)&7, (value>>12)&1, portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
                } else if (ac_mat == 2){
                    printk("[%s]%d:DIP %08X, SIP %08X :\n", __FUNCTION__, __LINE__, addr1, addr2);
                    printk("[%s]%d:%s  Switch Port:%X, STATUS:%d, TYPE:%d\n", __FUNCTION__, __LINE__,
                      portNameList[portNameIdx], port, (value>>2)&3, (value)&3);
                }
            }
            // trigger next
            reg = REG_ATC_ADDR;
            value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (5 << REG_ATC_AC_CMD_OFFT);    // 3'b101 : Next search command (next entry)
            macMultiMT7530WriteReg(reg, value, 1);
            while (1){  // wait until not busy
                value = macMultiMT7530ReadReg(reg, 1);
                if ((value & REG_ATC_BUSY_MASK) == 0){
                    break;
                }
            }
        }
    }
#endif

	return 0;
}

int macMT7530SetArlAddrTblClr(int ac_mat)
{
	u32 value = 0, reg = 0;

	if (ac_mat > 3){
		return 0;
	}

	reg = REG_ATC_ADDR + GSW_BASE;
	while (1){	// wait until not busy
		value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	// clean all entries
	reg = REG_ATC_ADDR + GSW_BASE;
	value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);	// 3'b010 : clean command (multiple entries)
	switch_reg_write(reg, value);
	while (1){	// wait until not busy
	value = switch_reg_read(reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = REG_ATC_ADDR;
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        
        // clean all entries
        reg = REG_ATC_ADDR;
        value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);    // 3'b010 : clean command (multiple entries)
        macMultiMT7530WriteReg(reg, value, 1);
        while (1){  // wait until not busy
        value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif

	return 0;
}


int macMT7530SetArlAddrTblClr2(u32 ext_switch, int ac_mat)
{
	u32 value = 0, reg = 0;

	if (ac_mat > 3){
		return 0;
	}
	printk("(ext_switch:%d)\r\n", ext_switch);

	reg = REG_ATC_ADDR + GSW_BASE;
	while (1){	// wait until not busy
		value = switch_reg_read2(ext_switch, reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

	// clean all entries
	reg = REG_ATC_ADDR + GSW_BASE;
	value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);	// 3'b010 : clean command (multiple entries)
	switch_reg_write2(ext_switch, reg, value);
	while (1){	// wait until not busy
	value = switch_reg_read2(ext_switch, reg);
		if ((value & REG_ATC_BUSY_MASK) == 0){
			break;
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = REG_ATC_ADDR;
        while (1){  // wait until not busy
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
        
        // clean all entries
        reg = REG_ATC_ADDR;
        value = (1 << REG_ATC_BUSY_OFFT) + (ac_mat << REG_ATC_AC_MAT_OFFT) + (2 << REG_ATC_AC_CMD_OFFT);    // 3'b010 : clean command (multiple entries)
        macMultiMT7530WriteReg(reg, value, 1);
        while (1){  // wait until not busy
        value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_ATC_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif

	return 0;
}

int macMT7530SetVlanPvid(u8 port, u16 pvid)
{
	u32 value = 0;
	u32 reg = 0;

	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
	/*Parameters is error*/
	if (port >= MT7530_TOTAL_PORTS) {
		return -1;
	}

	reg = 0x2014 + (port * 0x100);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= ~0xfff;
	value |= pvid;
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	/* switch clear */

	reg = 0x80;
	macMultiMT7530WriteReg(reg, 0x8002, extsw_flag);
	//usleep(5000);
	value = macMultiMT7530ReadReg(reg, extsw_flag);

	return 0;
}

int macMT7530SetVlanVid(mt7530_switch_api_vlan_vid_t *viddata)
{
	u8 index=0;
	u8 active=0;
	u8 portMap=0;
	u8 tagPortMap=0;
	u16 vid=0;
	u8 ivl_en=0;
	u8 fid=0;
	u16 stag=0;
	u32 value=0;
	u32 value2=0;
	int reg=0, i = 0;
	

	index=viddata->index;
	active=viddata->active;
	vid=viddata->vid;

	/*Check the input parameters is right or not.*/
	if((index >= 16)|| (active > 1))
	{
		return -1;
	}

	/*CPU Port is always the membership*/
	portMap=viddata->portMap;
	tagPortMap=viddata->tagPortMap;
	ivl_en=viddata->ivl_en;
	fid=viddata->fid;
	stag=viddata->stag;

//	ivl_en=1;
	portMap |= (1<<6);

	value = (portMap<<16);
	value |= (stag<<4);
	value |= (ivl_en<<30);
	value |= (fid<<1);
	value |= (active ? 1 : 0);

	// total 7 ports

	for (i = 0; i < 6; i++)	//SWIC_PORTS_NUM
	{
		if (tagPortMap & (1<<i)) {
			value2 |= 0x2<<(i*2);
		}
	}

	if (value2) {
		value |= (1<<28);		// eg_tag
	}

	reg = 0x98 + GSW_BASE; // VAWD2
	switch_reg_write(reg, value2);

	reg = 0x94 + GSW_BASE; // VAWD1
	switch_reg_write(reg, value);

	reg = 0x90 + GSW_BASE; // VTCR
	value = (0x80001000 + vid);
	switch_reg_write(reg, value);

	reg = 0x90 + GSW_BASE; // VTCR
	while (1) {
		value = switch_reg_read(reg);
		if ((value & 0x80000000) == 0) { //table busy
			break;
		}
	}

	/* switch clear */
	reg = 0x80 + GSW_BASE;
	switch_reg_write(reg, 0x8002);
	value = switch_reg_read(reg);
	
	return 0;
}

int macMT7530GetVlanPvid(u8 port){
	u32 value = 0, reg = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*Parameters is error*/
	if (port >= MT7530_TOTAL_PORTS) {
		return -1;
	}

	reg = 0x2014 + (port * 0x100);
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	return (value & 0xfff);
}/*end macRT63365VlanGetPvid*/

int macMT7530GetVlanVtbl(u16 index){
	u16 vid = 0;
	u32 reg = 0, value = 0, vawd1 = 0, vawd2 = 0;

	reg = 0x90 + GSW_BASE; // VTCR
	value = (0x80000000 + index);
	switch_reg_write(reg, value);

	reg = 0x90 + GSW_BASE; // VTCR
	while (1) {
		value = switch_reg_read(reg);
		if ((value & 0x80000000) == 0) { //table busy
			break;
		}
	}

	reg = 0x94 + GSW_BASE; // VAWD1
	vawd1 = switch_reg_read( reg);

	reg = 0x98 + GSW_BASE; // VAWD2
	vawd2 = switch_reg_read(reg);

	if (vawd1&0x1){
		//printk("[%s]%d:%02d.%03s vid:%02d fid:%02d portMap:0x%02x tagMap:0x%04x stag:0x%02x ivl_en:0x%02x\r\n", __FUNCTION__, __LINE__,
		//index,(vawd1&0x1)?"on":"off",index,((vawd1&0xe)>>1),(vawd1&0xff0000)>>16,vawd2,(vawd1&0xfff0)>>0x4,(vawd1>>30)&0x1);
	}
	return 0;
}

int macMT7530SetVlanDisp(void){
	int i=0;
	u32 reg = 0, value = 0;

	reg = 0x2604 + GSW_BASE;
	value = switch_reg_read(reg);
	value &= 0x30000000;

	printk("[%s]%d:VLAN function is %s\n", __FUNCTION__, __LINE__, value ? "enable":"disable");
	printk("[%s]%d:PVID e0:%02d e1:%02d e2:%02d e3:%02d e4:%02d e5:%02d e6:%02d\n", __FUNCTION__, __LINE__,
		macMT7530GetVlanPvid(0), macMT7530GetVlanPvid(1), macMT7530GetVlanPvid(2),
		macMT7530GetVlanPvid(3), macMT7530GetVlanPvid(4), macMT7530GetVlanPvid(5), macMT7530GetVlanPvid(6));

	for(i=0; i<4096; i++){
		macMT7530GetVlanVtbl(i);
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = 0x2604;
        value = macMultiMT7530ReadReg(reg, 1);
        value &= 0x30000000;
        
        printk("[%s]%d:Ext Switch VLAN function is %s\n", __FUNCTION__, __LINE__, value ? "enable":"disable");
        printk("[%s]%d:PVID e0:%02d e1:%02d e2:%02d e3:%02d e4:%02d e5:%02d e6:%02d\n", __FUNCTION__, __LINE__,
            macMT7530GetVlanPvid(0 | EXT_SWITCH_FLAG), macMT7530GetVlanPvid(1 | EXT_SWITCH_FLAG), macMT7530GetVlanPvid(2 | EXT_SWITCH_FLAG),
            macMT7530GetVlanPvid(3 | EXT_SWITCH_FLAG), macMT7530GetVlanPvid(4 | EXT_SWITCH_FLAG), macMT7530GetVlanPvid(5 | EXT_SWITCH_FLAG), macMT7530GetVlanPvid(6 | EXT_SWITCH_FLAG));
    }
#endif

	return 0;
}/*end macMT7530VlanDisp*/

int macMT7530SetVlanPortAttr(u8 port, u8 attr)
{
	u32 value = 0, reg = 0; 
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*Check the input parameters is right or not.*/
	if(attr > 3){
		return -1;
	}

	reg = 0x2010 + port*0x100;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= (0xffffff3f);
	value |= (attr<<6);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	return 0;
}

int macMT7530SetVlanPortMode(u8 port, u8 mode)
{
	u32 value = 0, reg = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*Check the input parameters is right or not.*/
	if(mode>3){
		return -1;
	}

	reg = 0x2004 + port*0x100;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= (~((1<<0) | (1<<1)));
	value |= (mode & 0x3);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	return 0;
}

int macMT7530SetVlanEgressTagPvc(u8 port, u8 eg_tag)
{
	u32 value = 0, reg = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*Check the input parameters is right or not.*/
	if (eg_tag > REG_PVC_EG_TAG_RELMASK){
		return -1;
	}

	reg = REG_PVC_P0_ADDR + port*0x100;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= (~REG_PVC_EG_TAG_MASK);
	value |= ((u32)eg_tag << REG_PVC_EG_TAG_OFFT);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	return 0;
}

int macMT7530SetVlanEgressTagPcr(u8 port, u8 eg_tag)
{
	u32 value = 0, reg = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*Check the input parameters is right or not.*/
	if (eg_tag > REG_PCR_EG_TAG_RELMASK)
	{
		return -1;
	}

	reg = REG_PCR_P0_ADDR + port*0x100;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= (~REG_PCR_EG_TAG_MASK);
	value |= ((u32)eg_tag << REG_PCR_EG_TAG_OFFT);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	return 0;
}

int macMT7530SetVlanAccFrm(u8 port, u8 type)
{
	u32 value = 0, reg = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*Check the input parameters is right or not.*/
	if (type > REG_PVC_ACC_FRM_RELMASK)
	{
		return -1;
	}

	reg = REG_PVC_P0_ADDR + port*0x100;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= (~REG_PVC_ACC_FRM_MASK);
	value |= ((u32)type << REG_PVC_ACC_FRM_OFFT);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	return 0;
}

int macMT7530SetPortMirrorEnable(u8 mirror_en, u8 mirror_port)
{
	u32 value = 0, reg = 0;
	u32 extsw_flag = 0;
	if(mirror_port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	mirror_port = mirror_port & SWITCH_PORT_MASK;

	/*Check the input parameters is right or not.*/
	if ((mirror_en > 1) || (mirror_port > REG_MFC_MIRROR_PORT_RELMASK))
	{
		return -1;
	}

	reg = REG_MFC_ADDR;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= (~REG_MFC_MIRROR_EN_MASK);
	value |= (mirror_en << REG_MFC_MIRROR_EN_OFFT);
	value &= (~REG_MFC_MIRROR_PORT_MASK);
	value |= (mirror_port << REG_MFC_MIRROR_PORT_OFFT);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	return 0;
}

int macMT7530SetPortMirrorPortBased(u8 port, mt7530_switch_api_port_mirror_port_based_t *portbased)
{
	u32 value= 0, reg = 0;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	/*Check the input parameters is right or not.*/
	if((port >= 7) || (portbased->port_tx_mir > 1) || (portbased->port_rx_mir > 1) 
		|| (portbased->acl_mir > 1) || (portbased->vlan_mis > 1))// also allow CPU port (port6)
	{	
		return -1;
	}

	reg = REG_PCR_P0_ADDR + port*0x100;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= ~(REG_PORT_TX_MIR_MASK | REG_PORT_RX_MIR_MASK | REG_PCR_ACL_MIR_MASK | REG_PCR_VLAN_MIS_MASK);
	value |= (portbased->port_tx_mir<<REG_PORT_TX_MIR_OFFT) + (portbased->port_rx_mir<<REG_PORT_RX_MIR_OFFT);
	value |= (portbased->acl_mir<<REG_PCR_ACL_MIR_OFFT) + (portbased->vlan_mis<<REG_PCR_VLAN_MIS_OFFT);
	macMultiMT7530WriteReg(reg, value, extsw_flag);

	reg = REG_PIC_P0_ADDR + port*0x100;
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= ~(REG_PIC_IGMP_MIR_MASK);
	value |= (portbased->igmp_mir<<REG_PIC_IGMP_MIR_OFFT);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	return 0;
}

int macMT7530SetDstq(u32 offset, u32 value)
{
	if(offset == 0){
		eth_dstq_write(value);
	}else if(offset == 1){
		eth_gsw_fc_write(value);
	}else{
		if (macMT7530_APIDbgDumpEn)printk("error dstq type:%x\n", offset);
	}
	return 0;
}

int macMT7530SetAclPortEnable(u8 acl_port, u8 acl_en)
{
	u32 reg = 0, value = 0;
	u32 extsw_flag = 0;
	if(acl_port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	acl_port = acl_port & SWITCH_PORT_MASK;

	printk("[%s]%d:acl_port = %d, acl_en = %d\n", __FUNCTION__, __LINE__, acl_port, acl_en);
	reg = 0x2004 + (0x100 * acl_port);	/*0x2004[10]*/
	value = macMultiMT7530ReadReg(reg, extsw_flag);
	value &= (~REG_PORT_ACL_EN_MASK);
	value |= (acl_en << REG_PORT_ACL_EN_OFFT);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
	macMultiMT7530WriteReg(reg, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, macMultiMT7530ReadReg(reg, extsw_flag));
	return 0;
}

int macMT7530SetAclMultiHit(u8 acl_multi)
{
	u32 reg = 0, value = 0;
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:acl_multi = %d\n", __FUNCTION__, __LINE__, acl_multi);
	reg = 0x000C + GSW_BASE;
	value = switch_reg_read(reg);
	value &= (~(1 << 5));
	value |= (acl_multi << 5);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
	switch_reg_write(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        reg = 0x000C;
        value = macMultiMT7530ReadReg(reg, 1);
        value &= (~(1 << 5));
        value |= (acl_multi << 5);
        macMultiMT7530WriteReg(reg, value, 1);
    }
#endif

	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(reg));
	return 0;
}

int macMT7530SetAclTblClr(void)
{
	int i = 0, j = 0;	
	u32 reg = 0, value = 0;
	const u32 tbl_field[5][2] =
	{
		{(REG_VTCR_BUSY_MASK | (0x05 << 12)), 64},	/* ACL table entry 0~63*/
		{(REG_VTCR_BUSY_MASK | (0x07 << 12)), 32},	/* trTCM entry 0~31*/
		{(REG_VTCR_BUSY_MASK | (0x09 << 12)), 32},	/* ACL Mask entry 0~31*/
		{(REG_VTCR_BUSY_MASK | (0x0B << 12)), 32},	/* ACL Rule Control entry 0~31*/
		{(REG_VTCR_BUSY_MASK | (0x0D << 12)), 32},	/* ACL Rate Control entry 0~31*/
	};

	switch_reg_write((0x0094 + GSW_BASE), 0x00000000);
	switch_reg_write((0x0098 + GSW_BASE), 0x00000000);

	reg = 0x0090 + GSW_BASE;
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}
	/* Clear different kinds of table entries*/
	for (j = 0; j < 5; j ++){
		for (i = 0; i < tbl_field[j][1]; i ++){
			value = tbl_field[j][0] | i;
			switch_reg_write(reg, value);
			if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
			while (1){	/* wait until not busy*/
				value = switch_reg_read(reg);
				if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
				if ((value & REG_VTCR_BUSY_MASK) == 0){
					break;
				}
			}
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(0x0094, 0x00000000, 1);
        macMultiMT7530WriteReg(0x0098, 0x00000000, 1);
        
        reg = 0x0090;
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
        /* Clear different kinds of table entries*/
        for (j = 0; j < 5; j ++){
            for (i = 0; i < tbl_field[j][1]; i ++){
                value = tbl_field[j][0] | i;
                macMultiMT7530WriteReg(reg, value, 1);
                if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
                while (1){  /* wait until not busy*/
                    value = macMultiMT7530ReadReg(reg, 1);
                    if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
                    if ((value & REG_VTCR_BUSY_MASK) == 0){
                        break;
                    }
                }
            }
        }
    }
#endif
	return 0;
}

int macMT7530SetAclTblDisp(u8 ext_switch)
{
	u32 reg = 0, value = 0;
	u32 vawd1, vawd2, rule_vawd1, rule_vawd2, rate_vawd1, rate_vawd2;
	int i;
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:ext_switch = %d\n", __FUNCTION__, __LINE__, ext_switch);
	while (1){	/* wait until not busy*/
		value = switch_reg_read2(ext_switch, (REG_VTCR_ADDR + GSW_BASE));
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}

	/* Display ACL table entry 0~63*/
	if (macMT7530_APIDbgDumpEn)printk("Found following ACL table entries (ext_switch :%d):\n", ext_switch);
	for (i = 0; i < 64; i ++){
		value = REG_VTCR_BUSY_MASK | (0x04 << REG_VTCR_FUNC_OFFT) | i;	/* read ACL table (0x04)*/
		switch_reg_write2(ext_switch, (REG_VTCR_ADDR + GSW_BASE), value);
		while (1){	/* wait until not busy*/
			value = switch_reg_read2(ext_switch, (REG_VTCR_ADDR + GSW_BASE));
			if ((value & REG_VTCR_BUSY_MASK) == 0){
				break;
			}
		}
		vawd1 = switch_reg_read2(ext_switch, (REG_VAWD1_ADDR + GSW_BASE));
		vawd2 = switch_reg_read2(ext_switch, (REG_VAWD2_ADDR + GSW_BASE));

		if (vawd2 & (1 << 19)){ /* EN==1*/
			if (macMT7530_APIDbgDumpEn)printk("[#%2d] CMP_SEL:%d, SP_map:%02X, OFST_TP:%d, WORD_OFST:%3d, BIT_MASK:%04X, CMP_PAT:%04X\n",
				i, (vawd2 & 1), ((vawd2>>8) & 0xFF), ((vawd2>>16) & 0x07), ((vawd2>>1) & 0x3F), ((vawd1>>16) & 0xFFFF), (vawd1 & 0xFFFF));
		}
	}

	/* Display Mask, Rule Ctrl, and Rate Ctrl table entry 0~31*/
	if (macMT7530_APIDbgDumpEn)printk("Found following Mask, Rule ctrl, Rate ctrl table entries : (in vawd1,vawd2)\n");
	for (i = 0; i < 32; i ++){
		value = REG_VTCR_BUSY_MASK | (0x08 << REG_VTCR_FUNC_OFFT) | i;	/* read Mask Table (0x08)*/
		switch_reg_write2(ext_switch, (REG_VTCR_ADDR + GSW_BASE), value);
		while (1){	/* wait until not busy*/
			value = switch_reg_read2(ext_switch, (REG_VTCR_ADDR + GSW_BASE));
			if ((value & REG_VTCR_BUSY_MASK) == 0){
				break;
			}
		}
		vawd1 = switch_reg_read2(ext_switch, (REG_VAWD1_ADDR + GSW_BASE));
		vawd2 = switch_reg_read2(ext_switch, (REG_VAWD2_ADDR + GSW_BASE));

		if (vawd1 || vawd2){	/* ACL_MASK[63:0] any bit != 0*/
			value = REG_VTCR_BUSY_MASK | (0x0A << REG_VTCR_FUNC_OFFT) | i;	/* read Rule Ctrl Table (0x0A)*/
			switch_reg_write2(ext_switch, (REG_VTCR_ADDR + GSW_BASE), value);
			while (1){	/* wait until not busy*/
				value = switch_reg_read2(ext_switch, (REG_VTCR_ADDR + GSW_BASE));
				if ((value & REG_VTCR_BUSY_MASK) == 0){
					break;
				}
			}
			rule_vawd1 = switch_reg_read2(ext_switch, (REG_VAWD1_ADDR + GSW_BASE));
			rule_vawd2 = switch_reg_read2(ext_switch, (REG_VAWD2_ADDR + GSW_BASE));

			value = REG_VTCR_BUSY_MASK | (0x0C << REG_VTCR_FUNC_OFFT) | i;	/* read Rate Ctrl Table (0x0C)*/
			switch_reg_write2(ext_switch, (REG_VTCR_ADDR + GSW_BASE), value);
			while (1){	/* wait until not busy*/
				value = switch_reg_read2(ext_switch, (REG_VTCR_ADDR + GSW_BASE));
				if ((value & REG_VTCR_BUSY_MASK) == 0){
					break;
				}
			}
			rate_vawd1 = switch_reg_read2(ext_switch, (REG_VAWD1_ADDR + GSW_BASE));
			rate_vawd2 = switch_reg_read2(ext_switch, (REG_VAWD2_ADDR + GSW_BASE));

			if (macMT7530_APIDbgDumpEn)printk("[#%2d] Mask:%08X,%08X; Rule:%08X,%08X; Rate:%08X,%08X\n",
				i, vawd1, vawd2, rule_vawd1, rule_vawd2, rate_vawd1, rate_vawd2);
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:ext_switch\n", __FUNCTION__, __LINE__);
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(REG_VTCR_ADDR, 1);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
        
        /* Display ACL table entry 0~63*/
        if (macMT7530_APIDbgDumpEn)printk("Found following ACL table entries (ext_switch):\n");
        for (i = 0; i < 64; i ++){
            value = REG_VTCR_BUSY_MASK | (0x04 << REG_VTCR_FUNC_OFFT) | i;  /* read ACL table (0x04)*/
            macMultiMT7530WriteReg(REG_VTCR_ADDR, value, 1);
            while (1){  /* wait until not busy*/
                value = macMultiMT7530ReadReg(REG_VTCR_ADDR, 1);
                if ((value & REG_VTCR_BUSY_MASK) == 0){
                    break;
                }
            }
            vawd1 = macMultiMT7530ReadReg(REG_VAWD1_ADDR, 1);
            vawd2 = macMultiMT7530ReadReg(REG_VAWD2_ADDR, 1);
        
            if (vawd2 & (1 << 19)){ /* EN==1*/
                if (macMT7530_APIDbgDumpEn)printk("[#%2d] CMP_SEL:%d, SP_map:%02X, OFST_TP:%d, WORD_OFST:%3d, BIT_MASK:%04X, CMP_PAT:%04X\n",
                    i, (vawd2 & 1), ((vawd2>>8) & 0xFF), ((vawd2>>16) & 0x07), ((vawd2>>1) & 0x3F), ((vawd1>>16) & 0xFFFF), (vawd1 & 0xFFFF));
            }
        }
        
        /* Display Mask, Rule Ctrl, and Rate Ctrl table entry 0~31*/
        if (macMT7530_APIDbgDumpEn)printk("Found following Mask, Rule ctrl, Rate ctrl table entries : (in vawd1,vawd2)\n");
        for (i = 0; i < 32; i ++){
            value = REG_VTCR_BUSY_MASK | (0x08 << REG_VTCR_FUNC_OFFT) | i;  /* read Mask Table (0x08)*/
            macMultiMT7530WriteReg(REG_VTCR_ADDR, value, 1);
            while (1){  /* wait until not busy*/
                value = macMultiMT7530ReadReg(REG_VTCR_ADDR, 1);
                if ((value & REG_VTCR_BUSY_MASK) == 0){
                    break;
                }
            }
            vawd1 = macMultiMT7530ReadReg(REG_VAWD1_ADDR, 1);
            vawd2 = macMultiMT7530ReadReg(REG_VAWD2_ADDR, 1);
        
            if (vawd1 || vawd2){    /* ACL_MASK[63:0] any bit != 0*/
                value = REG_VTCR_BUSY_MASK | (0x0A << REG_VTCR_FUNC_OFFT) | i;  /* read Rule Ctrl Table (0x0A)*/
                macMultiMT7530WriteReg(REG_VTCR_ADDR, value, 1);
                while (1){  /* wait until not busy*/
                    value = macMultiMT7530ReadReg(REG_VTCR_ADDR, 1);
                    if ((value & REG_VTCR_BUSY_MASK) == 0){
                        break;
                    }
                }
                rule_vawd1 = macMultiMT7530ReadReg(REG_VAWD1_ADDR, 1);
                rule_vawd2 = macMultiMT7530ReadReg(REG_VAWD2_ADDR, 1);
        
                value = REG_VTCR_BUSY_MASK | (0x0C << REG_VTCR_FUNC_OFFT) | i;  /* read Rate Ctrl Table (0x0C)*/
                macMultiMT7530WriteReg(REG_VTCR_ADDR, value, 1);
                while (1){  /* wait until not busy*/
                    value = macMultiMT7530ReadReg(REG_VTCR_ADDR, 1);
                    if ((value & REG_VTCR_BUSY_MASK) == 0){
                        break;
                    }
                }
                rate_vawd1 = macMultiMT7530ReadReg(REG_VAWD1_ADDR, 1);
                rate_vawd2 = macMultiMT7530ReadReg(REG_VAWD2_ADDR, 1);
        
                if (macMT7530_APIDbgDumpEn)printk("[#%2d] Mask:%08X,%08X; Rule:%08X,%08X; Rate:%08X,%08X\n",
                    i, vawd1, vawd2, rule_vawd1, rule_vawd2, rate_vawd1, rate_vawd2);
            }
        }
    }
#endif
	return 0;
}

int macMT7530SetAclTblAdd(u8 tbl_idx, u32 vawd1, u32 vawd2)
{
	u32 reg = 0, value = 0;
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
	reg = REG_VTCR_ADDR + GSW_BASE;
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}
	
	switch_reg_write((REG_VAWD1_ADDR + GSW_BASE), vawd1);
	switch_reg_write((REG_VAWD2_ADDR + GSW_BASE), vawd2);
	
	reg = REG_VTCR_ADDR + GSW_BASE;
	value = REG_VTCR_BUSY_MASK | (0x05 << REG_VTCR_FUNC_OFFT) | tbl_idx;
	switch_reg_write(reg, value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
        reg = REG_VTCR_ADDR;
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
        
        macMultiMT7530WriteReg(REG_VAWD1_ADDR, vawd1, 1);
        macMultiMT7530WriteReg(REG_VAWD2_ADDR, vawd2, 1);
        
        reg = REG_VTCR_ADDR;
        value = REG_VTCR_BUSY_MASK | (0x05 << REG_VTCR_FUNC_OFFT) | tbl_idx;
        macMultiMT7530WriteReg(reg, value, 1);
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif
	return 0;
}

int macMT7530SetAclMaskTblAdd(u8 tbl_idx, u32 vawd1, u32 vawd2)
{
	u32 reg = 0, value = 0;
	
	printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
	reg = REG_VTCR_ADDR + GSW_BASE;
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}
	
	switch_reg_write((REG_VAWD1_ADDR + GSW_BASE), vawd1);
	switch_reg_write((REG_VAWD2_ADDR + GSW_BASE), vawd2);
	
	reg = REG_VTCR_ADDR + GSW_BASE;
	value = REG_VTCR_BUSY_MASK | (0x09 << REG_VTCR_FUNC_OFFT) | tbl_idx;
	switch_reg_write(reg, value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
        reg = REG_VTCR_ADDR;
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
        
        macMultiMT7530WriteReg(REG_VAWD1_ADDR, vawd1, 1);
        macMultiMT7530WriteReg(REG_VAWD2_ADDR, vawd2, 1);
        
        reg = REG_VTCR_ADDR;
        value = REG_VTCR_BUSY_MASK | (0x09 << REG_VTCR_FUNC_OFFT) | tbl_idx;
        macMultiMT7530WriteReg(reg, value, 1);
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif
	return 0;
}

int macMT7530SetAclRuleTblAdd(u8 tbl_idx, u32 vawd1, u32 vawd2)
{
	u32 reg = 0, value = 0;
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
	reg = REG_VTCR_ADDR + GSW_BASE;
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}
	
	switch_reg_write((REG_VAWD1_ADDR + GSW_BASE), vawd1);
	switch_reg_write((REG_VAWD2_ADDR + GSW_BASE), vawd2);
	
	reg = REG_VTCR_ADDR + GSW_BASE;
	value = REG_VTCR_BUSY_MASK | (0x0B << REG_VTCR_FUNC_OFFT) | tbl_idx;
	switch_reg_write(reg, value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
        reg = REG_VTCR_ADDR;
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
        
        macMultiMT7530WriteReg(REG_VAWD1_ADDR, vawd1, 1);
        macMultiMT7530WriteReg(REG_VAWD2_ADDR, vawd2, 1);
        
        reg = REG_VTCR_ADDR;
        value = REG_VTCR_BUSY_MASK | (0x0B << REG_VTCR_FUNC_OFFT) | tbl_idx;
        macMultiMT7530WriteReg(reg, value, 1);
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif
	return 0;
}

int macMT7530SetAclRateTblAdd(u8 tbl_idx, u32 vawd1, u32 vawd2)
{
	u32 reg = 0, value = 0;
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
	reg = REG_VTCR_ADDR + GSW_BASE;
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}
	
	switch_reg_write((REG_VAWD1_ADDR + GSW_BASE), vawd1);
	switch_reg_write((REG_VAWD2_ADDR + GSW_BASE), vawd2);
	
	reg = REG_VTCR_ADDR + GSW_BASE;
	value = REG_VTCR_BUSY_MASK | (0x0D << REG_VTCR_FUNC_OFFT) | tbl_idx;
	switch_reg_write(reg, value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
	while (1){	/* wait until not busy*/
		value = switch_reg_read(reg);
		if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
		if ((value & REG_VTCR_BUSY_MASK) == 0){
			break;
		}
	}
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:tbl_idx = %d, vawd1 = %d, vawd2 = %d\n", __FUNCTION__, __LINE__, tbl_idx, vawd1, vawd2);
        reg = REG_VTCR_ADDR;
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
        
        macMultiMT7530WriteReg(REG_VAWD1_ADDR, vawd1, 1);
        macMultiMT7530WriteReg(REG_VAWD2_ADDR, vawd2, 1);
        
        reg = REG_VTCR_ADDR;
        value = REG_VTCR_BUSY_MASK | (0x0D << REG_VTCR_FUNC_OFFT) | tbl_idx;
        macMultiMT7530WriteReg(reg, value, 1);
        if (macMT7530_APIDbgDumpEn)printk("[%s]%d:write reg: %08x, value: %08x\n", __FUNCTION__, __LINE__, reg, value);
        while (1){  /* wait until not busy*/
            value = macMultiMT7530ReadReg(reg, 1);
            if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
            if ((value & REG_VTCR_BUSY_MASK) == 0){
                break;
            }
        }
    }
#endif
	return 0;
}

int macMT7530SetQosBase(u8 base)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:base = %d\n", __FUNCTION__, __LINE__, base);
	u32 value = 0;
	
	value = switch_reg_read(GSW_UPW);
	value &= (~((0x7<<0) | (0x7<<4) | (0x7<<8) | (0x7<<12) | (0x7<<16) | (0x7<<20)));
	switch (base)
	{
		case 0 : /* port-based 0x0044[18:16] */
			value |= ((0x2<<0) | (0x2<<4) | (0x2<<8) | (0x2<<12) | (0x7<<16) | (0x2<<20));
			break;
		case 1 : /* tagged-based 0x0044[10:8] */
			value |= ((0x2<<0) | (0x2<<4) | (0x7<<8) | (0x2<<12) | (0x2<<16) | (0x2<<20));
			break;
		case 2 : /* DSCP-based 0x0044[14:12] */
			value |= ((0x2<<0) | (0x2<<4) | (0x2<<8) | (0x7<<12) | (0x2<<16) | (0x2<<20));
			break;
		case 3 : /* acl-based 0x0044[2:0] */
			value |= ((0x7<<0) | (0x2<<4) | (0x2<<8) | (0x2<<12) | (0x2<<16) | (0x2<<20));
			break;
		case 4 : /* arl-based 0x0044[22:20] */
			value |= ((0x2<<0) | (0x2<<4) | (0x2<<8) | (0x2<<12) | (0x2<<16) | (0x7<<20));
			break;
		case 5 : /* stag-based 0x0044[6:4] */
			value |= ((0x2<<0) | (0x7<<4) | (0x2<<8) | (0x2<<12) | (0x2<<16) | (0x2<<20));
			break;
		default :
			break;
	}
	switch_reg_write(GSW_UPW, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(GSW_UPW-GSW_BASE, value, 1);
    }
#endif

	if (macMT7530_APIDbgDumpEn)printk("write reg: %08x, value: %08x\n", GSW_UPW , value);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(GSW_UPW));
	return 0;
}

int macMT7530SetQosPrioQmap(u8 prio, u8 queue)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:prio = %d, queue = %d\n", __FUNCTION__, __LINE__, prio, queue);
	u32 reg = 0, value = 0;	
	u8 pem_n = 0;
		
	pem_n = prio/2;
	value = switch_reg_read(GSW_PEM(pem_n));
	if (prio%2){
		value &= (~(0x7<<24));
		value |= ((queue&0x7)<<24);
	} else {
		value &= (~(0x7<<8));
		value |= ((queue&0x7)<<8);
	}
	switch_reg_write((GSW_PEM(pem_n)), value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg((GSW_PEM(pem_n))-GSW_BASE, value, 1);
    }
#endif

	if (macMT7530_APIDbgDumpEn)printk("write reg: %08x, value: %08x\n", (GSW_PEM(pem_n)), value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(GSW_PEM(pem_n)));
	return 0;
}

int macMT7530SetQosPrioTagmap(u8 prio, u8 tag)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:prio = %d, tag = %d\n", __FUNCTION__, __LINE__, prio, tag);
	u32 reg = 0, value = 0;	
	u8 pem_n = 0;
		
	pem_n = prio/2;
	value = switch_reg_read(GSW_PEM(pem_n));
	if (prio%2){
		value &= (~(0x7<<27));
		value |= ((tag&0x7)<<27);
	} else {
		value &= (~(0x7<<11));
		value |= ((tag&0x7)<<11);
	}
	switch_reg_write((GSW_PEM(pem_n)), value);
    
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg((GSW_PEM(pem_n))-GSW_BASE, value, 1);
    }
#endif

	if (macMT7530_APIDbgDumpEn)printk("write reg: %08x, value: %08x\n",(GSW_PEM(pem_n)), value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(GSW_PEM(pem_n)));
	return 0;
}

int macMT7530SetQosPrioDscpmap(u8 prio, u8 dscp)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:prio = %d, dscp = %d\n", __FUNCTION__, __LINE__, prio, dscp);
	u32 reg = 0, value = 0;	
	u8 pem_n = 0;
		
	pem_n = prio/2;
	value = switch_reg_read(GSW_PEM(pem_n));
	if (prio%2){
		value &= (~(0x3F<<16));
		value |= ((dscp&0x3F)<<16);
	} else {
		value &= (~0x3F);
		value |= (dscp&0x3F);
	}
	switch_reg_write((GSW_PEM(pem_n)), value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg((GSW_PEM(pem_n))-GSW_BASE, value, 1);
    }
#endif

	if (macMT7530_APIDbgDumpEn)printk("write reg: %08x, value: %08x\n", (GSW_PEM(pem_n)), value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(GSW_PEM(pem_n)));
	return 0;
}


int macMT7530SetQosPortPrio(u8 port, u8 prio)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:port = %d, prio = %d\n", __FUNCTION__, __LINE__, port, prio);
	u32 reg = 0, value = 0;	
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	value = macMultiMT7530ReadReg(GSW_PCR(port)-GSW_BASE, extsw_flag);
	value &= (~(0x7<<24));
	value |= (prio<<24);
	macMultiMT7530WriteReg((GSW_PCR(port))-GSW_BASE, value, extsw_flag);
	if (macMT7530_APIDbgDumpEn)printk("write reg: %08x, value: %08x\n", (GSW_PCR(port)), value);
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(GSW_PCR(port)));
	return 0;
}


int macMT7530SetQosPortWeight(mt7530_switch_api_qos_port_weight_t *qosweight)
{
	u32 reg = 0, value = 0;		
	u8 queue = 0;
	u32 extsw_flag = 0;
	if(qosweight->port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	qosweight->port = qosweight->port & SWITCH_PORT_MASK;

	for (queue=0; queue<8; queue++){
		reg = GSW_MMSCR1_Q(queue) + 0x100*qosweight->port - GSW_BASE;
		value = macMultiMT7530ReadReg(reg, extsw_flag);
		value &= (~(0xf<<24));
		value |= (((qosweight->weight[queue]-1)&0xf)<<24);
		macMultiMT7530WriteReg(reg, value, extsw_flag);
		
		if (macMT7530_APIDbgDumpEn)printk("write reg: %08x, value: %08x\n", reg, value);
		if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(reg));
	}
	return 0;
}



int macMT7530SetQosDscpPrio(u8 dscp, u8 prio)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:dscp = %d, prio = %d\n", __FUNCTION__, __LINE__, dscp, prio);
	u32 reg = 0, value = 0;	
	u8 pim_n = 0, pim_offset = 0;
	
	pim_n = dscp/10;	/* every PIM register contain 10 dscp settings*/
	pim_offset = (dscp - pim_n*10) * 3; /* every dscp priority setting use 3 bits*/
	reg = 0x0058 + pim_n*4 + GSW_BASE;
	
	value = switch_reg_read(reg);
	value &= (~(0x7<<pim_offset));
	value |= ((prio&0x7)<<pim_offset);
	switch_reg_write(reg, value);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg-GSW_BASE, value, 1);
    }
#endif

	if (macMT7530_APIDbgDumpEn)printk("write reg: %08x, value: %08x\n", reg, value);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(reg));
	return 0;
}

int macMT7530GetGSWRead(u32 offset, u32 *p_value)
{
	u32 value = 0;
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:offset = %d\n", __FUNCTION__, __LINE__, offset);
	
	value = switch_reg_read(offset + GSW_BASE);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
    *p_value = value;
	return 0;
}

int macMT7530SetGSWWrite(u32 offset, u32 value)
{
	printk("[%s]%d:offset = %d, value = %d\n", __FUNCTION__, __LINE__, offset, value);
	
	switch_reg_write((offset + GSW_BASE), value);
	printk("write reg: %08x, value: %08x\n", (offset + GSW_BASE), value);
	
	printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read(offset + GSW_BASE));
	return 0;
}

int macMT7530GetGSWRead2(u32 ext_switch, u32 reg, u32 *p_value)
{
	u32 value  = 0;
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:ext_switch = %d, reg = %d\n", __FUNCTION__, __LINE__, ext_switch, reg);
	
	value = switch_reg_read2(ext_switch, (reg + GSW_BASE));
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, value);
    *p_value = value;
	return 0;
}

int macMT7530SetGSWWrite2(u32 ext_switch, u32 reg, u32 value)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:ext_switch = %d, reg = %d, value = %d\n", __FUNCTION__, __LINE__, ext_switch, reg, value);
	
	switch_reg_write2(ext_switch, (reg + GSW_BASE), value);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:read retuen = %08x\n", __FUNCTION__, __LINE__, switch_reg_read2(ext_switch, (reg + GSW_BASE)));
	return 0;
}

int macMT7530Getmiir(mt7530_switch_config_t *paramext)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:paramext->phyaddr = %d, paramext->reg = %d\n", __FUNCTION__, __LINE__,  paramext->phyaddr, paramext->reg);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d\n", __FUNCTION__, __LINE__);
	paramext->value = (u32)mdio_read(NULL, paramext->phyaddr, paramext->reg);
	return 0;
}

int macMT7530Setmiiw(mt7530_switch_config_t *paramext)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:paramext->phyaddr = %d, paramext->reg = %d\n", __FUNCTION__, __LINE__,  paramext->phyaddr, paramext->reg);
	int ret = 0;
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d\n", __FUNCTION__, __LINE__);
	mdio_write(NULL,paramext->phyaddr, paramext->reg, paramext->value);
	if (macMT7530_APIDbgDumpEn)printk("*PhyAddr=%d Reg=%d value:%x\r\n", paramext->phyaddr, paramext->reg, paramext->value);
	return 0;
}

int macMT7530Getmiir2(mt7530_switch_config_t *paramext)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:paramext->ext_switch = %d,paramext->phyaddr = %d, paramext->reg = %d\n", __FUNCTION__, __LINE__, paramext->ext_switch, paramext->phyaddr, paramext->reg);
		
	if (paramext->ext_switch)
	{	
		paramext->value = gswPmiRead(paramext->phyaddr, paramext->reg);
	}
	else
	{
		paramext->value = mdio_read(NULL, paramext->phyaddr, paramext->reg);
	}
	return 0;
}

int macMT7530Setmiiw2(mt7530_switch_config_t *paramext)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:paramext->ext_switch = %d, paramext->phyaddr = %d, paramext->reg = %d, paramext->value = %d\n", __FUNCTION__, __LINE__, paramext->ext_switch, paramext->phyaddr, paramext->reg, paramext->value);
	int ret = 0;
	
	if (paramext->ext_switch)
	{
		ret = gswPmiWrite(paramext->phyaddr, paramext->reg, paramext->value);
	}
	else
	{
		mdio_write(NULL,paramext->phyaddr, paramext->reg, paramext->value);
	}
	return ret;
}

int macMT7530GetEmiir(mt7530_switch_api_paramext4 *paramext)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:phyaddr = %d, devaddr = %d, phyreg = %d\n", __FUNCTION__, __LINE__,  paramext->p1, paramext->p2, paramext->p3);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d\n", __FUNCTION__, __LINE__);
	paramext->p4 = (u16)mdio_cl45_read(paramext->p1, paramext->p2, paramext->p3);
	return 0;
}

int macMT7530SetEmiiw(mt7530_switch_api_paramext4 *paramext)
{
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d:phyaddr = %d, devaddr = %d, phyreg = %d, value=%x\n", __FUNCTION__, __LINE__,  paramext->p1, paramext->p2, paramext->p3, paramext->p4);
	
	if (macMT7530_APIDbgDumpEn)printk("[%s]%d\n", __FUNCTION__, __LINE__);
	mdio_cl45_write(paramext->p1, paramext->p2, paramext->p3, paramext->p4);
	if (macMT7530_APIDbgDumpEn)printk("*PhyAddr=%d DevAddr=%d PhyReg=%d value:%x\r\n", paramext->p1, paramext->p2, paramext->p3, paramext->p4);
	return 0;
}

int macMT7530SetVipPkt(mt7530_switch_vip_info vip)
{
	u32 reg=0,value=0,mark=0;
	switch(vip.type)
	{
		case ARP:
			reg = GSW_APC;
			mark = 1<<ARP_MANG_FR_SHIFT;	
			break;
		case PPOE:
			reg = GSW_APC;
			mark = 1<<PPP_MANG_FR_SHIFT;
			break;
		case IGMP:
			reg = GSW_IMC;
			mark = 1<<IGMP_MANG_FR_SHIFT;
			break;
		case MLD:
			reg = GSW_IMC;
			mark = 1<<MLD_MANG_FR_SHIFT;
			break;
		case DHCP:
			reg = GSW_DPC;
			mark = (1<< DHCP_4_MANG_FR_SHIFT) | (1<< DHCP_6_MANG_FR_SHIFT);
			break;
	    default:
            return 0;
	}
	if(vip.isvip)
	{
		value =  switch_reg_read(reg);
		value |= mark;
		switch_reg_write(reg,value);
	}
	else
	{
		value =  switch_reg_read(reg);
		value &= ~mark;
		switch_reg_write(reg,value);
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        macMultiMT7530WriteReg(reg-GSW_BASE, value, 1);
    }
#endif
	return 0;

}

int macMT7530SetPortMap(int wan_port, int lan_portmap[6])
{
    int port = 0;
    u8 value = 0;
    
    if ( (wan_port < 0 && wan_port != -1) 
        || wan_port >= RT63365_SWIC_PORTNUM - 1)
    {
        return -1;
    }
    for (port = 0; port < 6; port++)
    {
        if ( (lan_portmap[port] < 0 && lan_portmap[port] != -1) 
            || lan_portmap[port] >= RT63365_SWIC_PORTNUM - 1)
        {
            return -1;
        }
    }
    
#if defined(TCSUPPORT_WAN_ETHER)
    wan_port_id = wan_port;
#endif

    memset(lan_port_map, -1, MAX_ETH_ITF_NUM * sizeof(char));
    for (port = 0; port < 6; port++)
    {
        lan_port_map[port] = lan_portmap[port];
    }

    memset(switch_port_map, -1, RT63365_SWIC_PORTNUM * sizeof(char));
    for (port = 0; port < MAX_ETH_ITF_NUM ; port++)
    {
        value = lan_port_map[port];
        if (value < RT63365_SWIC_PORTNUM)
        {
            switch_port_map[value] = port;
        }
    }
    macMT7530SetVlanActive(0);
    macMT7530SetVlanActive(1);

    return 0;
}

int macMT7530ctagWhiteListAdd(u8 port, u16 vid)
{
	u32 value, vawd1;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
	value = (0x80000000 + vid);
	macMultiMT7530WriteReg(VTCR - GSW_BASE, value, extsw_flag);
	while (1) {
		value = macMultiMT7530ReadReg(VTCR - GSW_BASE, extsw_flag);
		if ((value & 0x80000000) == 0) { //table busy
			break;
		}
	}
	vawd1 = macMultiMT7530ReadReg(VAWD1 - GSW_BASE, extsw_flag);
	
	macMultiMT7530WriteReg(VAWD1 - GSW_BASE,(vawd1 | (1 << (port+16))), extsw_flag);
	value = (0x80001000 + vid);
	macMultiMT7530WriteReg(VTCR - GSW_BASE,value, extsw_flag);
	while (1) {
		value = macMultiMT7530ReadReg(VTCR - GSW_BASE, extsw_flag);
		if ((value & 0x80000000) == 0) { //table busy
			break;
		}
	}

	return 0;
}

int macMT7530ctagWhiteListDel(u8 port, u16 vid)
{
	u32 value, vawd1;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

	value = (0x80000000 + vid);
	macMultiMT7530WriteReg(VTCR - GSW_BASE, value, extsw_flag);
	while (1) {
		value = macMultiMT7530ReadReg(VTCR - GSW_BASE, extsw_flag);
		if ((value & 0x80000000) == 0) { //table busy
			break;
		}
	}
	vawd1 = macMultiMT7530ReadReg(VAWD1 - GSW_BASE, extsw_flag);

	macMultiMT7530WriteReg(VAWD1 - GSW_BASE, (vawd1 & (~(1 << (port+16)))), extsw_flag);
	value = (0x80001000 + vid);
	macMultiMT7530WriteReg(VTCR - GSW_BASE, value, extsw_flag);
	while (1) {
		value = macMultiMT7530ReadReg(VTCR - GSW_BASE, extsw_flag);
		if ((value & 0x80000000) == 0) { //table busy
			break;
		}
	}

	return 0;

}

int macMT7530ctagWhiteListMode(u8 type)
{
	int index=0;
	u32 value, vawd1;
	
	for(index=0; index<4096; index++){
		
		value = (0x80000000 + index);
		switch_reg_write(VTCR,value);
		while (1) {
			value = switch_reg_read(VTCR);
			if ((value & 0x80000000) == 0) { //table busy
				break;
			}
		}
		vawd1 = switch_reg_read(VAWD1);
		
		if(type == 0)
			switch_reg_write(VAWD1,(vawd1 | (0x7f<<16)));
		value = (0x80001000 + index);
		switch_reg_write(VTCR,value);
		while (1) {
			value = switch_reg_read(VTCR);
			if ((value & 0x80000000) == 0) { //table busy
				break;
			}
		}

	}	

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if (isEN7526G)
    {
        for(index=0; index<4096; index++){
            
            value = (0x80000000 + index);
            macMultiMT7530WriteReg(VTCR - GSW_BASE, value, 1);
            while (1) {
                value = macMultiMT7530ReadReg(VTCR - GSW_BASE, 1);
                if ((value & 0x80000000) == 0) { //table busy
                    break;
                }
            }
            vawd1 = macMultiMT7530ReadReg(VAWD1 - GSW_BASE, 1);
            
            if(type == 0)
                macMultiMT7530WriteReg(VAWD1 - GSW_BASE,(vawd1 | (0x7f<<16)), 1);
            value = (0x80001000 + index);
            macMultiMT7530WriteReg(VTCR - GSW_BASE, value, 1);
            while (1) {
                value = macMultiMT7530ReadReg(VTCR - GSW_BASE, 1);
                if ((value & 0x80000000) == 0) { //table busy
                    break;
                }
            }
        
        }   
    }
#endif

	return 0;

}

int macMT7530ctagWhiteListPerPortMode(u8 port, u8 type)
{
	int index=0;

    if(type==1)
    {
        for(index=0; index<4096; index++){
            macMT7530ctagWhiteListDel(port, index);
        }
        macMT7530ctagWhiteListAdd(port,macMT7530GetVlanPvid(port));
    }
    else
    {
        for(index=0; index<4096; index++){
            macMT7530ctagWhiteListAdd(port, index);
        }
    }

	return 0;    
}

int macMT7530LedOn(uint8 port, uint8 led_num)
{
    u32 value=0, reg;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;

    if (!extsw_flag)
    {
        value = gswPbusRead(GPIO_DATA_REG);
        value &= ~(1 << (port * 4 + led_num));
        gswPbusWrite(GPIO_DATA_REG, value);
    }
    else
    {
        value = gswPbusRead2(GPIO_DATA_REG);
        value &= ~(1 << (port * 4 + led_num));
        gswPbusWrite2(GPIO_DATA_REG, value);
    }
    return 0;
}

int macMT7530LedOff(uint8 port, uint8 led_num)
{
    u32 value=0, reg;
	u32 extsw_flag = 0;
	if(port & EXT_SWITCH_FLAG)
		extsw_flag = 1;
	port = port & SWITCH_PORT_MASK;
    
    if (!extsw_flag)
    {
        value = gswPbusRead(GPIO_DATA_REG);
        value |= (1 << (port * 4 + led_num));
        gswPbusWrite(GPIO_DATA_REG, value);
    }
    else
    {
        value = gswPbusRead2(GPIO_DATA_REG);
        value |= (1 << (port * 4 + led_num));
        gswPbusWrite2(GPIO_DATA_REG, value);
    }
    return 0;
}

int macMT7530SetWOL(mt7530_switch_api_wol_config_t *woldata)
{
	u8 index = 0;
	u8 type = 0;
	int i = 0;
	u32 value = 0, reg = 0;	

	index = woldata->index;
	type = woldata->type;

	if((index >= 8) || (type >= 4))
	{
		return -1;
	}
	
	if(woldata->enable == 0){
		reg = REG_WUC1 + GSW_BASE;
		value = ~(0x1 << WOL_SFT_RST) & value;
		if(type == 0){ // link status wake
			value = ~(0x1 << WOL_WAKE_EN_LINK_OFFT) & value;
		}else if(type == 1){ //dest mac wake
			value = ~(0x1 << WOL_WAKE_EN_DA_OFFT) & value;
		}else if(type == 2){ //magic packet wake
			value = ~(0x1 << WOL_WAKE_EN_MP_OFFT) & value;
		}else if(type == 3){ //WUL wake
			value = ~(0x1 << WOL_WAKE_EN_WUF_OFFT) & value;		
		}
		switch_reg_write(reg, value);
	}else if(woldata->enable == 1){
		if(type == 0){ // link status wake
			reg = REG_LNK_CHG_EN + GSW_BASE;
			switch_reg_write(reg, 0xF);
			
			reg = REG_WUC1 + GSW_BASE;
			value = switch_reg_read(reg);
			value = (0x1 << WOL_WAKE_EN_LINK_OFFT) | value;
			switch_reg_write(reg, value);
		}else if((type == 1) ||(type == 2)) { //dest mac wake or magic packet wake
			reg = REG_WOL_SMACCR1 + GSW_BASE;
			value = (woldata->sys_mac[0] << 8) | (woldata->sys_mac[1]);
			switch_reg_write(reg, value);
			
			reg = REG_WOL_SMACCR0 + GSW_BASE;
			value = (woldata->sys_mac[2] << 24) | (woldata->sys_mac[3] << 16) | (woldata->sys_mac[4] << 8) | woldata->sys_mac[5];
			switch_reg_write(reg, value);
			for(i=0; i<4; i++)
			{
				reg = REG_MULTI_ADR_RG(i*3) + GSW_BASE;
				value = (woldata->mul_mac[i*2][2] << 24) | (woldata->mul_mac[i*2][3] << 16) | (woldata->mul_mac[i*2][4] << 8) | woldata->mul_mac[i*2][5];
				switch_reg_write(reg, value);
				
				reg = REG_MULTI_ADR_RG(i*3+1) + GSW_BASE;
				value = (woldata->mul_mac[i*2+1][4] << 24) | (woldata->mul_mac[i*2+1][5] << 16) | (woldata->mul_mac[i*2][0] << 8) | (woldata->mul_mac[i*2][1]);
				switch_reg_write(reg, value);

				reg = REG_MULTI_ADR_RG(i*3+2) + GSW_BASE;
				value = (woldata->mul_mac[i*2+1][0] << 24) | (woldata->mul_mac[i*2+1][1] << 16) | (woldata->mul_mac[i*2+1][2] << 8) | (woldata->mul_mac[i*2+1][3]);
				switch_reg_write(reg, value);				
			}
			if(type == 1){  //dest mac wake
				reg = REG_WUC1 + GSW_BASE;
				value = switch_reg_read(reg);
				value = (0x1 << WOL_WAKE_EN_DA_OFFT) | (0x1 << WOL_ACCEPT_MULTI_OFFT) | (0x1 << WOL_ACCEPT_BROAD_OFFT) | value;
				switch_reg_write(reg, value);
			}else if(type == 2){ //magic packet wake
				reg = REG_WUC1 + GSW_BASE;
				value = switch_reg_read(reg);
				value = (0x1 << WOL_WAKE_EN_MP_OFFT) | (0x1 << WOL_ACCEPT_MULTI_OFFT) | (0x1 << WOL_ACCEPT_BROAD_OFFT) | value;
				switch_reg_write(reg, value);
			}
		}else if(type == 3){ //WUL wake
			reg = REG_MATCH_LEN(index/4) + GSW_BASE;
			value = woldata->match_len << (8 * (index%4));
			switch_reg_write(reg, value);
			
			reg = REG_FINAL_BYTE(index/4) + GSW_BASE;
			value = woldata->final_byte << (8 * (index%4));
			switch_reg_write(reg, value);
			
			reg = REG_EXP_CRC(index/2) + GSW_BASE;
			value = woldata->checksum << (16 * (index%2));
			switch_reg_write(reg, value);
			
			reg = REG_WUF_EN + GSW_BASE;
			switch_reg_write(reg, 0xFF);
			
			reg = REG_WUC1 + GSW_BASE;
			value = switch_reg_read(reg);
			value = (0x1 << WOL_WAKE_EN_WUF_OFFT) | value;
			switch_reg_write(reg, value);

			for(i=0; i<woldata->byte_mask_num; i++)
			{
				if(index < 4)
				{
					reg = REG_WUF_MEM_CFG_WDATA_L + GSW_BASE;
					value = woldata->byte_mask[i] << (8 * index);
					switch_reg_write(reg, value);
				}else{
					reg = REG_WUF_MEM_CFG_WDATA_H + GSW_BASE;
					value = woldata->byte_mask[i] << (8 * (index-4));
					switch_reg_write(reg, value);
				}
				
				reg = REG_WUF_MEM_CFG + GSW_BASE;
				value = (i << WOL_WUFMEM_CFG_ADR_OFFT) | (0x1 << WOL_WUFMEM_CFG_FUN_SEL_OFFT) | (0x1 << WOL_WUFMEM_CFG_EN_OFFT); //write
				switch_reg_write(reg, value);
				
				//mdelay(10);
		
				reg = REG_WUF_MEM_CFG + GSW_BASE;
				value = (i << WOL_WUFMEM_CFG_ADR_OFFT) | (0x1 << WOL_WUFMEM_CFG_EN_OFFT); //read
				switch_reg_write(reg, value);
				while (1) {
					value = switch_reg_read(reg);
					if ((value & 0x80000000) == 0) { //write done
						break;
					}
				}
		
			}
		}
	}

	return 0;
}

int macMT7530GetWOLStatus(mt7530_switch_api_wol_status_t *wolresult)
{
	u32 value = 0, reg = 0;

	memset(wolresult, 0x0, sizeof(mt7530_switch_api_wol_status_t));
	
	reg = REG_WAKE_INT_STS + GSW_BASE;
	value = switch_reg_read(reg);
	wolresult->interrupt = value;

	reg = REG_WAKE_CNT_DBG + GSW_BASE;
	value = switch_reg_read(reg);
	printk("%s -- %d reg %x = %x \n", __func__, __LINE__, reg, value);
	wolresult->da_cnt = (value >> WOL_WAKE_CNT_DA_OFFT) & WOL_WAKE_CNT_MASK;
	wolresult->mp_cnt = (value >> WOL_WAKE_CNT_MP_OFFT) & WOL_WAKE_CNT_MASK;
	wolresult->wuf_cnt = (value >> WOL_WAKE_CNT_WUF_OFFT) & WOL_WAKE_CNT_MASK;
	printk("%s -- %d da_cnt = %d  mp_cn = %dt wuf_cnt = %d\n", __func__, __LINE__, wolresult->da_cnt, wolresult->mp_cnt, wolresult->wuf_cnt);

	return 0;
}

#endif /*#ifdef TCSUPPORT_MT7530_SWITCH_API*/

