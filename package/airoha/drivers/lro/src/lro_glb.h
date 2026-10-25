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
#ifndef _LRO_GLB_H_
#define _LRO_GLB_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/netdevice.h>
#include <asm/tc3162/tc3162.h>
#include <asm/io.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


#if defined(LRO_LAN)
#define lro_path		"lro_lan"
#define QDMA_TYPE		ECNT_QDMA_LAN
#define LRO_DEV_NAME	"lro_lan_dev"
#define IS_WAN 			0
#define LRO_NAPI_LOCK	lro_lan_napilock
#define ECNT_LRO		ECNT_LRO_LAN
#else
#define lro_path		"lro_wan"
#define QDMA_TYPE		ECNT_QDMA_WAN
#define LRO_DEV_NAME	"lro_wan_dev"
#define IS_WAN			1
#define LRO_NAPI_LOCK	lro_wan_napilock
#define ECNT_LRO		ECNT_LRO_WAN
#endif


#define LRO_AGGNUM_OFF		0x4
#define LRO_AGGNUM_MASK		0xFF
#define LRO_AGGNUM_SHIFT	0x10

#define LRO_AGGSIZE_OFF		0x4
#define LRO_AGGSIZE_MASK	0xFFFF
#define LRO_AGGSIZE_SHIFT	0x0

#define LRO_AGETIME_OFF		0x8
#define LRO_AGETIME_MASK	0xFFFF
#define LRO_AGETIME_SHIFT	0x10

#define LRO_AGGTIME_OFF		0x8
#define LRO_AGGTIME_MASK	0xFFFF
#define LRO_AGGTIME_SHIFT	0x0

#define LRO_RXMSG_FLUSHRESON_MASK	0xF
#define LRO_RXMSG_FLUSHRESON_SHIFT	0x14


/* LRO Flush Reason. */
#define LRO_AGG_FLUSH			0x0
#define LRO_AGE_FLUSH			0x1
#define LRO_AGG_SIZE_FLUSH		0x2
#define LRO_AGG_CNT_FLUSH		0x3
#define LRO_SEQ_FLUSH			0x4
#define LRO_TS_FLUSH			0x5
#define LRO_NEXT_NONRULE_FLUSH	0x6
#define LRO_THIS_NONRULE_FLUSH	0x7
#define LRO_QDMA_DROP_FLUSH		0x8

#define SET_VALUE(addr, mask, shift, value) IO_SREG(addr, ((IO_GREG(addr) & ~(mask << shift)) | ((value & mask) << shift)))


/********************************
 *        VIP  Register         *
 ********************************/


/***************************************
    proc name
***************************************/





/************************************************************************
*                  M A C R O S
*************************************************************************
*/    


#define CONFIG_DEBUG 1
#ifdef CONFIG_DEBUG
	#define LRO_MSG(level, F, B...)			{ \
												if(lroDbgLevel >= level)	\
												printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B) ; \
											}
	#define LRO_MSG_STRIP(level, F, B...)	{ \
												if(lroDbgLevel >= level)	\
												printk(F, ##B) ; \
											}
	#define LRO_ERR(F, B...)				printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B)
	#define LRO_LOG(F, B...)				printk("%s [%d]: " F, strrchr(__FILE__, '/')+1, __LINE__, ##B)
#else
	#define LRO_MSG(level, F, B...)	
	#define LRO_ERR(F,B...)					printk(F, ##B)
	#define LRO_LOG(F,B...)					printk(F, ##B)
#endif


/************************************************************************
*                  D A T A     T Y P E S
*************************************************************************
*/

typedef struct {
	struct net_device	*dev;
	struct napi_struct	napi;
} LRO_Adapter_T;


typedef struct {
	uint32 tcp_ts_rly;
#ifdef __BIG_ENDIAN
	uint32 done:1;
	uint32 rsv:15;
	uint32 pkt_len:16;
#else
	uint32 pkt_len:16;
	uint32 rsv:15;
	uint32 done:1;
#endif
	uint32 pkt_ptr;
	uint32 tcp_ack_num;
} lro_rx_dscp_t;


/***************************************
 struct definition
***************************************/

#define SUPPORT_RING_AGGREGATION  (isEN7581 || isAN7552 || isAN7583)

/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/



/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
    


#endif /* _LRO_H_ */


