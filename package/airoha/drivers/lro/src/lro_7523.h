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
#ifndef _LRO_H_
#define _LRO_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/netdevice.h>
#include <asm/tc3162/tc3162.h>
#include <asm/io.h>
#include "lro_glb.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


#define LRO_RING_NUM			4
#define LRO_RING_START			0xb
#define LRO_MAX_AGG_CNT			30
#define LRO_MAX_AGG_SIZE		44000
//#define LRO_MAX_AGG_SIZE		(65254)	/* alloc_len - sizeof(skb) -sizeof(sk_shareinfo) - 24 */
#define LRO_MAX_AGG_TIME		1000		/* 200us */
#define LRO_SAMBA_AGG_TIME		100
#define LRO_MAX_AGE_TIME		2000	/* 1000us */
#define TCP_HEAD_LEN_TIMESTAMP	8
#define LRO_FLUSH_REASON_NUM	9
#define LRO_SHORT_LEN			1500

#define	LRO_REG_RXQ(wan, ctrl_idx)   (0xBFB50478|((wan&0x1)<<12)|(ctrl_idx&0x4))
#define	LRO_REG_BASE(wan)	(0xBFB50480|((wan&0x1)<<12))

#define LRO_ENABLE_OFF	0x0
#define LRO_GET_QUEUE_IDX(addr, ctrl_idx) ((IO_GREG(addr) >> (ctrl_idx * 4)) & 0xf)
#define LRO_GET_ENABLE(addr, ctrl_idx) (IO_GREG(addr) & (1<<(ctrl_idx + 24)))


#define LRO_INDEX_OFF		0xB


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


/************************************************************************
*                  D A T A     T Y P E S
*************************************************************************
*/

typedef union{
	struct {
#ifdef __BIG_ENDIAN
		uint32 rsv:1;
		uint32 nmic:1;
		uint32 sp_tag:16;
		uint32 rsv2:2;
		uint32 lon:1;
		uint32 runt:1;
		uint32 crc_er:1;
		uint32 oam:1;
		uint32 rxc:5;
		uint32 rsv3:3;
#else
		uint32 rsv3:3;
		uint32 rxc:5;
		uint32 oam:1;
		uint32 crc_er:1;
		uint32 runt:1;
		uint32 lon:1;
		uint32 rsv2:2;
		uint32 sp_tag:16;
		uint32 nmic:1;
		uint32 rsv:1;
#endif

#ifdef __BIG_ENDIAN
		uint32 o:1;
		uint32 dei:1;
		uint32 ip6:1;
		uint32 ip4:1;
		uint32 p4f:1;
		uint32 l4vld:1;
		uint32 l4f:1;
		uint32 sport:5;
		uint32 crsn:5;
		uint32 ppe_entry:15;
#else
		uint32 ppe_entry:15;
		uint32 crsn:5;
		uint32 sport:5;
		uint32 l4f:1;
		uint32 l4vld:1;
		uint32 p4f:1;
		uint32 ip4:1;
		uint32 ip6:1;
		uint32 dei:1;
		uint32 o:1;
#endif
	
#ifdef __BIG_ENDIAN
		uint32 agg_cnt:8;
		uint32 flush_rsn:4;
		uint32 rsv4:13;
		uint32 l2_len:7;
#else
		uint32 l2_len:7;
		uint32 rsv4:13;
		uint32 flush_rsn:4;
		uint32 agg_cnt:8;
#endif
	
#ifdef __BIG_ENDIAN
		uint32 agg_ipv4_total_len:16;
		uint32 tcp_win:16;	
#else
		uint32 tcp_win:16;
		uint32 agg_ipv4_total_len:16;
#endif
}raw;
	uint32  msg[4];
}lro_rx_msg_t;



/***************************************
 struct definition
***************************************/


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


