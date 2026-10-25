/***************************************************************
Copyright Statement:

This software/firmware and related documentation ("Airoha Software") 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to Airoha Limited ("Airoha") and/or 
its licensors. Without the prior written permission of Airoha and/or its licensors, 
any reproduction, modification, use or disclosure of Airoha Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

Airoha Limited. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS ("AIROHA SOFTWARE") RECEIVED FROM AIROHA 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN "AS IS" 
BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES AIROHA PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE AIROHA SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN AIROHA SOFTWARE.

AIROHA SHALL NOT BE RESPONSIBLE FOR ANY AIROHA SOFTWARE RELEASES 
MADE TO RECEIVER'S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
AIROHA'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AIROHA 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT AIROHA'S SOLE OPTION, TO 
REVISE OR REPLACE THE AIROHA SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO AIROHA FOR SUCH 
AIROHA SOFTWARE.
***************************************************************/
#ifndef __UAPI_ECNT_SKB_MARK_INFO_H__
#define __UAPI_ECNT_SKB_MARK_INFO_H__

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*               M A C R O S
*************************************************************************
*/


/**
* \file  ecnt_skb_mark_info.h 
* \brief This file is skb->mark bits use info and definitions.
*        originally, this file is used for FH BSP version.
* \date     2021-04-16
* \version  001 
* \copyright EcoNet Inc
*/

/*******************************************************************************************************************
*skb->mark(32bit)                                                                                                  *
* 31...27 | 26 | 25 | 24 | 23 | 22...16 | 15...12 | 11...8 | 7...4 | 3 | 2 |1 | 0 |                                *
*    |      |         |            |         |         |       |                                                   *
*    |      |         |            |         |         |       |---------------------- QOS_FILTER_MARK             *
*    |      |         |            |         |         |--------------------------- QOS_8021p_MARK                 *
*    |      |         |            |         |---------------------------------- QOS_RULE_INDEX_MARK               *
*    |      |         |            |----------------------------------------- WAN_ITF_MARK                         *
*    |      |         |--------------------------------------------------- SKBUF_COPYTOLAN                         *
*    |      |---------------------------------------------------------- SKBUF_COPYTOLAN                            *
*    |-------------------------------------------------------------- LAN_ITF_MASK                                  *
*                                                                                                                  *
*                                                                                                                  *
*                                                                                                                  *
*******************************************************************************************************************/

#define WAN_ITF_MARK				0x7F0000
#define SKB_WAN_ITF_OFFSET			16

#if defined(TCSUPPORT_XPON_HAL_API_EXT)
#define SKBUF_COPYTOLAN				(1 << 24)
#else
#define SKBUF_COPYTOLAN				(1 << 26)
#endif

#define QOS_8021p_MARK				0x0F00 	/* 8~11 bits used for 802.1p */
#define QOS_RULE_INDEX_MARK			0x0000F002
#define QOS_RULE_INDEX_OFFSET		12
#define QOSTYPE_PBIT_MARK_OFFSET	8
#define QOSTYPE_8021P_MARK			0x0F00 	/* 8~11 bits used for 802.1p */
#define QOSTYPE_QUEUE_INDEX_MARK	0xF000 	/* 12~15 bits used for queue index */
#define QOSTYPE_QUEUE_INDEX_OFFSET	12

#define QOS_DEFAULT_MARK 			0x00000008
#define QOS_FILTER_MARK 			0x000000f0
#define	QOS_HH_PRIORITY				0x00000010
#define QOS_H_PRIORITY				0x00000020
#define QOS_M_PRIORITY				0x00000030
#define QOS_L_PRIORITY				0x00000040
#define QOS_NODROP_MARK				0x00000001
/* no queue marked packets to default queue */
#define QOS_PRIORITY_DEFAULT 		0x00000080
#define QOS_DOT1P_MARK				0x00000f00
#define QOS_RTP_MARK				0x00000004


/* skb->mark operations */
#define clearWanItfMark(x)          do { (x) &= ( ~WAN_ITF_MARK ); }while(0) 
#define setWanItfMark(x,y)          do { (x) |= ( ( (y) << SKB_WAN_ITF_OFFSET ) & WAN_ITF_MARK ); }while(0) 
#define getWanItfMark(x)            ( ( (x) & WAN_ITF_MARK ) >> SKB_WAN_ITF_OFFSET )

#define getWanPvcIndex(x)           ( ( (x) >> SKB_WAN_ITF_OFFSET ) / 8 )
#define getWanEntryIndex(x)         ( ( (x) >> SKB_WAN_ITF_OFFSET ) % 8 )

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

#endif /* __UAPI_ECNT_SKB_MARK_INFO_H__ */

