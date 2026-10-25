#ifndef _AE_WAN_IC_DIS_H
#define _AE_WAN_IC_DIS_H

#define SERDES_EOF				    (isEN7581 && (1 == GET_PDIDR()))	    /*7581_E1 serdes EOF*/
#define FC_FRAG_ERR				    (isEN7581 && (1 == GET_PDIDR()))	    /*7581_E1 serdes EOF*/

#define SUPPORT_USB_PCIE_ETH_IN_GDMA3		(isEN7523)
#define SUPPORT_USB_ETH_IN_GDMA4			(isEN7581 || isAN7552)
#define SUPPORT_USB_PCIE_IN_GDMA4			(isAN7583)
#define SUPPORT_ETH_IN_GDMA3				(isAN7583)
#define SUPPORT_GDMA4 						(isEN7581 || isAN7552 || isAN7583)

#define SUPPORT_TX_TOTAL_RATELIMIT			(isAN7583)
#define SUPPORT_MAC_EEE						(isAN7583)	

#if defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_GDMA_CHANNEL_RETIRE 1
#else
#define SUPPORT_GDMA_CHANNEL_RETIRE 0
#endif

/*new xfi arch and phy api after 7523*/
#if defined(TCSUPPORT_CPU_EN7581)||defined(TCSUPPORT_CPU_AN7552)||defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_AE_WAN_NEW_ARCH 1
#else
#define SUPPORT_AE_WAN_NEW_ARCH 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)||defined(TCSUPPORT_CPU_AN7552)||defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_SERDES_PON 1
#else
#define SUPPORT_SERDES_PON 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_PCIE0 1
#else
#define SUPPORT_SERDES_PCIE0 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_PCIE1 1
#else
#define SUPPORT_SERDES_PCIE1 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_USB 1
#else
#define SUPPORT_SERDES_USB 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_SERDES_ETHER 1
#else
#define SUPPORT_SERDES_ETHER 0
#endif

#if defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_AS21xx_WA 1
#else
#define SUPPORT_AS21xx_WA 0
#endif
#if defined(TCSUPPORT_ETHER_AS21XX)
#define SUPPORT_AS21xx 1
#else
#define SUPPORT_AS21xx 0
#endif


#if defined(TCSUPPORT_CPU_AN7583)
#include "xsi_reg_7583.h"
#elif defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#include "xsi_reg_7581.h"
#else
#include "xsi_reg_7523.h"
#endif

#endif
