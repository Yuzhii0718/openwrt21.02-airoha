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
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#ifndef _GMAC_MT7530_API_H_
#define _GMAC_MT7530_API_H_

/**
* \file  mtkswitch.h  
* \brief This file is mtkswitch header file of business logic APIs that will be exported for others to use.
* \author Feijie.Wang
* \date     2020-11-16
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <modules/eth_global_def.h>


/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
/************************************************************************
*				M A C R O S
*************************************************************************
*/

/************************************************************************
*				D A T A   T Y P E S
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

/**
 * \brief This function get bridging between UNI ports status (disable/enable).
 *
 * This function get bridging between UNI ports status (disable/enable).
 * corresponding register:
 * 		0x3P00[14] MAC_TX_EN_Pn
 * 		0x3P00[13] MAC_RX_EN_Pn
 * \param[in]	port id
 * \param[out]	0:disable, 1:enable
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get bridging between UNI ports status.
 */
int macMT7530GetPortBrgInd(u8 port, u8 *Ind);

/**
 * \brief This function set bridging between UNI ports status (disable/enable).
 *
 * This function set bridging between UNI ports status (disable/enable).
 * corresponding register:
 * 		0x3P00[14] MAC_TX_EN_Pn
 * 		0x3P00[13] MAC_RX_EN_Pn
 * \param[in]	port id
 * \param[in]	0:disable, 1:enable
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set bridging between UNI ports status.
 */
int macMT7530SetPortBrgInd(u8 port, u8 Ind);

/**
 * \brief This function get discard unknown Mac address status (disable/enable).
 *
 * This function get discard unknown Mac address status (disable/enable).
 * corresponding register:
 * 		0x0010[31:24] BC_FFP
 *      0x0010[23:16] MC_FFP
 *      0x0010[15:8]  UC_FFP
 * \param[in]	port id
 * \retval	0	Discard.
 * \retval	1	Not Discard.
 * \note
 *  get discard unknown Mac address status.
 */
int macMT7530GetDiscardUnknownMacInd(u8 port);

/**
 * \brief This function set discard unknown Mac address status (disable/enable).
 *
 * This function set discard unknown Mac address status (disable/enable).
 * corresponding register:
 * 		0x0010[31:24] BC_FFP
 *      0x0010[23:16] MC_FFP
 *      0x0010[15:8]  UC_FFP
 * \param[in]	port id
 * \param[in]	0:Discard, 1:Not Discard
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set discard unknown Mac address status.
 */
int macMT7530SetDiscardUnknownMacInd(u8 port, u8 Ind);

/**
 * \brief This function get address table age time (in seconds).
 *
 * This function get address table age time (in seconds).
 * corresponding register:
 * 		0x00A0[19:12] AGE_CNT, default=0x95
 * 		0x00A0[11:0]  AGE_UNIT, default=1
 * 		--> (AGE_CNT+1)*(AGE_UNIT+1) seconds
 * \param[out]	MaxAgeTime,unit is seconds
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get address table age time (in seconds).
 */
int macMT7530GetAgeTime(u32 *outptr_MaxAgeTime);

/**
 * \brief This function set address table age time (in seconds).
 *
 * This function set address table age time (in seconds).
 * corresponding register:
 * 		0x00A0[19:12] AGE_CNT, default=0x95
 * 		0x00A0[11:0]  AGE_UNIT, default=1
 * 		--> (AGE_CNT+1)*(AGE_UNIT+1) seconds
 * \param[out]	MaxAgeTime,unit is seconds
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set address table age time (in seconds).
 */
int macMT7530SetAgeTime(u32 MaxAgeTime);

/**
 * \brief This function get port mac address.
 *
 * This function get port mac address.
 * corresponding register:
 * 		0x0080, 0x0084, 0x0088, 0x008C
 * \param[in]	port id
 * \param[out]	port mac address,u8 portMac[6]
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port mac address.
 */
int macMT7530GetPortMac(u8 port, u8 *outptr_PortMac);

/**
 * \brief This function get port status.
 *
 * This function get port status.
 * corresponding register:
 * 		0x2P00[1:0] FID0_PST
 *		0x3P08[0] MAC_LNK_STS_Pn
 * \param[in]	port id
 * \retval	0	Disabled.
 * \retval	1	Listening.
 * \retval	2	Learning
 * \retval	3   Forwarding
 * \retval	4	Blocking (same as 1 for MT7530 h/w)
 * \retval	5	Linkdown
 * \retval	6	(R)Stp_off (N/A)
 * \retval	-1	failure
 * \note
 *  get port status.
 */
int macMT7530GetPortStatus(u8 port);

/**
 * \brief This function get port link status.
 *
 * This function get port link status.
 * corresponding register:
 * \param[in]	port id
 * \retval	0x01	10BaseT full duplex.
 * \retval	0x02	100BaseT full duplex.
 * \retval	0x03	Gigabit Ethernet full duplex
 * \retval	0x11    10BaseT half duplex
 * \retval	0x12	100BaseT half duplex
 * \retval	0x13	Gigabit Ethernet half duplex
 * \retval	-1	failure
 * \note
 *  get port link status.
 */
int macMT7530GetConfigurationStatus(u8 port);

/**
 * \brief This function set queue priority.
 *
 * This function set queue priority.
 * corresponding register:
 * \param[in]	priority,0~7
 * \param[in]	queue id,0~7
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set queue priority.
 */
int macMT7530SetQueuePriority(u16 priority, u8 queueId);

/**
 * \brief This function get queue max queue size.
 *
 * This function get queue max queue size.
 *	8 queues share total 256 units
 *	every unit = 2 blocks = 2 * 256 bytes
 *	every queue max 256 * 2 * 256 / 8 = 16384 bytes (0x4000)
 *	but FC_BLK_THD_Q0~Q7 only have 4 bits to select units = 15 * 2 * 256 = 7680 (0x1E00)
 * corresponding register:
 * \param[in]	queue id,0~7
 * \retval	max queue size.
 * \note
 *  get queue max queue size.
 */
int macMT7530GetMaximumQueueSize(u8 queueId);

/**
 * \brief This function get allocate queue size.
 *
 * This function get allocate queue size.
 * corresponding register:
 * \param[in]	queue id,0~7
 * \retval	alloc queue size.
 * \note
 *  get allocate queue size.
 */
int macMT7530GetAllocatedQueueSize(u8 queueId);

/**
 * \brief This function set allocate queue size.
 *
 * This function set allocate queue size.
 * corresponding register:
 * \param[in]	alloc queue size,every queue threshold max 15 units (every unit = 2 * 256 bytes)
 * \param[in]	queue id,0~7
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set allocate queue size.
 */
int macMT7530SetAllocatedQueueSize(u16 AllocatedQueueSize, u8 queueId);

/**
 * \brief This function get queue weight.
 *
 * This function get queue weight.
 * corresponding register:
 * \param[in]	queue id,0~7
 * \param[in]	port id
 * \retval	queue weight.
 * \note
 *  get queue weight.
 */
int macMT7530GetWeight(u8 queueId, u8 port);

/**
 * \brief This function set queue weight.
 *
 * This function set queue weight.
 * corresponding register:
 * \param[in]	queue weight
 * \param[in]	queue id,0~7
 * \param[in]	port id
 * \retval	queue weight.
 * \note
 *  set queue weight.
 */
int macMT7530SetWeight(u8 weight, u8 queueId, u8 port);

/**
 * \brief This function get back pressure.
 *
 * This function get back pressure.
 * 		return result in us; back-pressure fixed 64 bytes length + 96 bytes IPG
 * corresponding register:
 * \param[in]	backPressure_p struct 
 * \param[in]	queue id,0~7
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get back pressure.
 */
int macMT7530GetBackPressure(mt7530_switch_api_backPressure_t *backPressure_p, u8 queueId, u8 port);

/**
 * \brief This function set back pressure.
 *
 * This function set back pressure.
 * corresponding register:
 * \param[in]	backPressure_p struct 
 * \param[in]	queue id,0~7
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set back pressure.
 */
int macMT7530SetBackPressure(mt7530_switch_api_backPressure_t *backPressure_p, u8 queueId, u8 port);

/**
 * \brief This function get drop policy.
 *
 * This function get drop policy.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	queue id,0~7
 * \param[in]	drop policy struct 
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get drop policy.
 */
int macMT7530GetDropPolicy(u8 port, u8 queueId, mt7530_switch_DropPolicy_t *outptr_DropPolicy);

/**
 * \brief This function set drop policy.
 *
 * This function set drop policy.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	queue id,0~7
 * \param[in]	drop policy struct 
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set drop policy.
 */
int macMT7530SetDropPolicy(u8 port, u8 queueId, mt7530_switch_DropPolicy_t *DropPolicy);

/**
 * \brief This function set traffic descriptor.
 *
 * This function set traffic descriptor.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	traffic descriptor struct 
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set traffic descriptor.
 */
int macMT7530SetTrafficDescriptor(u8 port, mt7530_switch_api_trafficDescriptor_t *trafficDescriptor_p);

/**
 * \brief This function set traffic descriptor.
 *
 * This function set traffic descriptor.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	direction 0:TX 1:RX 
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set traffic descriptor.
 */
int macMT7530CleanTrafficDescriptor(u8 port, u8 direction);

/**
 * \brief This function set ds trtcm.
 *
 * This function set ds trtcm.
 * corresponding register:
 * \param[in]	trtcm struct,rate unit is kbps 
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set ds trtcm.
 */
int macMT7530SetDsTrtcm(mt7530_switch_api_trtcm_t *trtcm_p);

/**
 * \brief This function set ds trtcm enable.
 *
 * This function set ds trtcm enable.
 * corresponding register:
 * \param[in]	0 disable 1 enable 
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set ds trtcm enable.
 */
int macMT7530SetDsTrtcmEnable(u8 enable);

/**
 * \brief This function get fcs error counter.
 *
 * This function get fcs error counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	fcs error counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get fcs error counter.
 */
int macMT7530GetFCSErrors(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get excessive collision counter.
 *
 * This function get excessive collision counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	excessive collision counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get excessive collision counter.
 */
int macMT7530GetExcessiveCollisionCounter(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get late collision counter.
 *
 * This function get late collision counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	late collision counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get late collision counter.
 */
int macMT7530GetLateCollisionCounter(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get too long frame counter.
 *
 * This function get too long frame counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	too long frame counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get too long frame counter.
 */
int macMT7530GetFramesTooLong(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get buffer over flow rx counter.
 *
 * This function get buffer over flow rx counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	buffer over flow rx counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get buffer over flow rx counter.
 */
int macMT7530GetBufferOverflowsRx(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get single collision frame counter.
 *
 * This function get single collision frame counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	single collision frame counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get single collision frame counter.
 */
int macMT7530GetSingleCollisionFrameCounter(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get multiple collision frame counter.
 *
 * This function get multiple collision frame counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	multiple collision frame counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get multiple collision frame counter.
 */
int macMT7530GetMultipleCollisionsFrameCounter(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get deferred tx counter.
 *
 * This function get deferred tx counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	deferred tx counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get deferred tx counter.
 */
int macMT7530GetDeferredTxCounter(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get alignment error counter.
 *
 * This function get alignment error counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	alignment error counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get alignment error counter.
 */
int macMT7530GetAlignmentErrorCounter(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get fragments counter.
 *
 * This function get fragments counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	fragments counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get fragments counter.
 */
int macMT7530GetFragments(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get jabbers counter.
 *
 * This function get jabbers counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	jabbers counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get jabbers counter.
 */
int macMT7530GetJabbers(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get tx 64bytes counter.
 *
 * This function get tx 64bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	tx 64bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get tx 64bytes counter.
 */
int macMT7530GetPortTx64PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get rx 64bytes counter.
 *
 * This function get rx 64bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	rx 64bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get rx 64bytes counter.
 */
int macMT7530GetPortRx64PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get tx 65-127bytes counter.
 *
 * This function get tx 65-127bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	tx 65-127bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get tx 65-127bytes counter.
 */
int macMT7530GetPortTx127PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get rx 65-127bytes counter.
 *
 * This function get rx 65-127bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	rx 65-127bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get rx 65-127bytes counter.
 */
int macMT7530GetPortRx127PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get tx 128-255bytes counter.
 *
 * This function get tx 128-255bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	tx 128-255bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get tx 128-255bytes counter.
 */
int macMT7530GetPortTx255PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get rx 128-255bytes counter.
 *
 * This function get rx 128-255bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	rx 128-255bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get rx 128-255bytes counter.
 */
int macMT7530GetPortRx255PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get tx 256-511bytes counter.
 *
 * This function get tx 256-511bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	tx 256-511bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get tx 256-511bytes counter.
 */
int macMT7530GetPortTx511PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get rx 256-511bytes counter.
 *
 * This function get rx 256-511bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	rx 256-511bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get rx 256-511bytes counter.
 */
int macMT7530GetPortRx511PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get tx 512-1023bytes counter.
 *
 * This function get tx 512-1023bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	tx 512-1023bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get tx 512-1023bytes counter.
 */
int macMT7530GetPortTx1023PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get rx 512-1023bytes counter.
 *
 * This function get rx 512-1023bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	rx 512-1023bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get rx 512-1023bytes counter.
 */
int macMT7530GetPortRx1023PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get tx 1024-1518bytes counter.
 *
 * This function get tx 1024-1518bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	tx 1024-1518bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get tx 1024-1518bytes counter.
 */
int macMT7530GetPortTx1518PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get rx 1024-1518bytes counter.
 *
 * This function get rx 1024-1518bytes counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	rx 1024-1518bytes counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get rx 1024-1518bytes counter.
 */
int macMT7530GetPortRx1518PktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get max frame size.
 *
 * This function get max frame size.
 * corresponding register: 
 * \param[out]	max frame size
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get max frame size.
 */
int macMT7530GetMaxFrameSize(u16 *outptr_MaxFrameSize, u8 port);

/**
 * \brief This function set max frame size.
 *
 * This function set max frame size.
 * corresponding register: 
 * \param[in]	max frame size
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set max frame size.
 */
int macMT7530SetMaxFrameSize(u16 MaxFrameSize, u8 port);

/**
 * \brief This function get pause time.
 *
 * This function get pause time.
 * corresponding register: 
 * \param[out]	pause time
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get pause time.
 */
int macMT7530GetPauseTime(u16 *outptr_pauseTime, u8 port);

/**
 * \brief This function set pause time.
 *
 * This function set pause time.
 * corresponding register: 
 * \param[out]	pause time
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set pause time.
 */
int macMT7530SetPauseTime(u16 pauseTime, u8 port);

/**
 * \brief This function set port admin.
 *
 * This function set port admin.
 * corresponding register: 
 * \param[out]	port admin
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port admin.
 */
int macMT7530SetPortAdmin(u8 admin, u8 port);

/**
 * \brief This function get port rx lack drop packet counter.
 *
 * This function get port rx lack drop packet counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx lack drop packet counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx lack drop packet counter.
 */
int macMT7530GetPortRxLackDropPktCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx octets counter.
 *
 * This function get port rx octets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx octets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx octets counter.
 */
int macMT7530GetPortRxOctetsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx under size pakcets counter.
 *
 * This function get port rx under size pakcets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx under size pakcets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx under size pakcets counter.
 */
int macMT7530GetPortRxUnderSizePktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port tx success pakcets counter.
 *
 * This function get port tx success pakcets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx success pakcets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx success pakcets counter.
 */
int macMT7530GetPortTxSuccFramesCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx drop pakcets counter.
 *
 * This function get port rx drop pakcets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx drop pakcets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx drop pakcets counter.
 */
int macMT7530GetPortRxDropFramesCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx over size pakcets counter.
 *
 * This function get port rx over size pakcets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx over size pakcets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx over size pakcets counter.
 */
int macMT7530GetPortRxOverSizePktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port tx octets counter.
 *
 * This function get port tx octets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx octets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx octets counter.
 */
int macMT7530GetPortTxOctetsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port tx packets counter.
 *
 * This function get port tx packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx packets counter.
 */
int macMT7530GetPortTxPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx packets counter.
 *
 * This function get port rx packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx packets counter.
 */
int macMT7530GetPortRxPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port tx broadcast packets counter.
 *
 * This function get port tx broadcast packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx broadcast packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx broadcast packets counter.
 */
int macMT7530GetPortTxBroadPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx broadcast packets counter.
 *
 * This function get port rx broadcast packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx broadcast packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx broadcast packets counter.
 */
int macMT7530GetPortRxBroadPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port tx multicast packets counter.
 *
 * This function get port tx multicast packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx multicast packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx multicast packets counter.
 */
int macMT7530GetPortTxMultiPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx multicast packets counter.
 *
 * This function get port rx multicast packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx multicast packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx multicast packets counter.
 */
int macMT7530GetPortRxMultiPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port tx crc error packets counter.
 *
 * This function get port tx crc error packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx crc error packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx crc error packets counter.
 */
int macMT7530GetPortTxCRCPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port rx crc error packets counter.
 *
 * This function get port rx crc error packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx crc error packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx crc error packets counter.
 */
int macMT7530GetPortRxCRCPktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function set port pause.
 *
 * This function set port pause.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	0 disable 1 enalbe
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port pause.
 */
int macMT7530SetPortPause(u8 port, u8 enable);

/**
 * \brief This function set port policing enable.
 *
 * This function set port policing enable.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	0 disable 1 enalbe
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port policing enable.
 */
int macMT7530SetPortPolicingEnable(u8 port, u8 enable);

/**
 * \brief This function set port policing.
 *
 * This function set port policing.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	unit is kbps
 * \param[in]	unit is bytes
 * \param[in]	unit is bytes
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port policing.
 */
int macMT7530SetPortPolicing(u8 port, u32 cir, u32 cbs, u32 ebs);

/**
 * \brief This function set port ratelimiting enable.
 *
 * This function set port ratelimiting enable.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	0 disable 1 enalbe
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port ratelimiting enable.
 */
int macMT7530SetPortRatelimitingEnable(u8 port, u8 enable);

/**
 * \brief This function set port ratelimiting.
 *
 * This function set port ratelimiting.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	unit is kbps
 * \param[in]	unit is kbps
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port ratelimiting.
 */
int macMT7530SetPortRatelimiting(u8 port, u32 cir, u32 pir);

/**
 * \brief This function set port active.
 *
 * This function set port active.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	 0 disable 1 enalbe
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port active.
 */
int macMT7530SetPortActive(u8 port, u8 enable);

/**
 * \brief This function set port auto negoation active.
 *
 * This function set port auto negoation active.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	 0 disable 1 enalbe
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port auto negoation active.
 */
int macMT7530PortAutoNegActive(u8 port, u8 enable);

/**
 * \brief This function restart port auto negoation auto config.
 *
 * This function restart port auto negoation auto config.
 * corresponding register:
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  restart port auto negoation auto config.
 */
int macMT7530PortAutoNegRestartAutoConfig(u8 port);

/**
 * \brief This function get eth port los.
 *
 * This function get eth port los.
 * corresponding register:
 * \param[in]	port id
 * \retval	-1	Fail.
 * \retval	0	Signal Not Loss.
 * \retval	1	Signal Loss.
 * \note
 *  restart port auto negoation auto config.
 */
int macMT7530GetEthPortLos(u8 port);

/**
 * \brief This function set port mac limit.
 *
 * This function set port mac limit.
 * corresponding register:
 * \param[in]	port id
 * \param[in]	mac cnt
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set port mac limit.
 */
int macMT7530SetPortMacLimit(u8 port, u32 cnt);

/**
 * \brief This function get port admin.
 *
 * This function get port admin.
 * corresponding register:
 * \param[out]	admin
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port admin.
 */
int macMT7530GetPortAdmin(u8 *outptr_admin, u8 port);

/**
 * \brief This function get port auto negoation state.
 *
 * This function get port auto negoation state.
 * corresponding register:
 * \param[in]	port id
 * \retval	0	disable.
 * \retval	1	enable.
 * \note
 *  get port auto negoation state.
 */
int macMT7530GetPortAutoNegState(u8 port);

/**
 * \brief This function get port pause.
 *
 * This function get port pause.
 * corresponding register:
 * \param[out]	admin
 * \param[in]	port id
 * \retval	0	disable.
 * \retval	1	enable.
 * \note
 *  get port pause.
 */
int macMT7530GetPortPause(u8 port);

/**
 * \brief This function get port policing enable.
 *
 * This function get port policing enable.
 * corresponding register:
 * \param[in]	port id
 * \retval	0	disable.
 * \retval	1	enable.
 * \note
 *  get port policing enable.
 */
int macMT7530GetPortPolicingEnable(u8 port);

/**
 * \brief This function get port policing.
 *
 * This function get port policing.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	unit is kbps
 * \param[out]	unit is bytes
 * \param[out]	unit is bytes
 * \retval	0	success.
 * \retval	1	fail.
 * \note
 *  get port policing.
 */
int macMT7530GetPortPolicing(u8 port, u32 *outptr_cir, u32 *outptr_cbs, u32 *outptr_ebs);

/**
 * \brief This function get port ratelimiting enable.
 *
 * This function get port ratelimiting enable.
 * corresponding register:
 * \param[in]	port id
 * \retval	0	disable.
 * \retval	1	enable.
 * \note
 *  get port ratelimiting enable.
 */
int macMT7530GetPortRatelimitingEnable(u8 port);

/**
 * \brief This function get port ratelimiting.
 *
 * This function get port ratelimiting.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	unit is kbps
 * \param[out]	unit is kbps
 * \retval	0	disable.
 * \retval	1	enable.
 * \note
 *  get port ratelimiting enable.
 */
int macMT7530GetPortRatelimiting(u8 port, u32 *outptr_cir, u32 *outptr_pir);

/**
 * \brief This function get port transmitted frames drop counter.
 *
 * This function get port transmitted frames drop counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port txFrames drop counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port txFrames drop conuter.
 */
int macMT7530GetPortTxDropFramesCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port transmitted undersize packets counter.
 *
 * This function get port transmitted undersize packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx undersize packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx undersize packets conuter.
 */
int macMT7530GetPortTxUnderSizePktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port transmitted oversize packets counter.
 *
 * This function get port transmitted oversize packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx oversize packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx oversize packets conuter.
 */
int macMT7530GetPortTxOverSizePktsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port received discards packets counter.
 *
 * This function get port received discards packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port rx discards packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port rx discards packets counter.
 */
int macMT7530GetPortRxPacketsDiscardsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get port transmitted discards packets counter.
 *
 * This function get port transmitted discards packets counter.
 * corresponding register:
 * \param[in]	port id
 * \param[out]	port tx discards packets counter
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get port tx discards packets counter.
 */
int macMT7530GetPortTxPacketsDiscardsCnt(u8 port, u32 *outptr_Cnt);

/**
 * \brief This function get eth port autoNeg failure.
 *
 * This function get eth port autoNeg failure.
 * corresponding register:
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get eth port autoNeg failure.
 */
int macMT7530GetEthPortAutoNegFailure(u8 port);

/**
 * \brief This function get eth port failure.
 *
 * This function get eth port failure.
 * corresponding register:
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get eth port failure.
 */
int macMT7530GetEthPortFailure(u8 port);

/**
 * \brief This function get eth port congestion.
 *
 * This function get eth port congestion.
 * corresponding register:
 * \param[in]	port id
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get eth port congestion.
 */
int macMT7530GetEthPortCongestion(u8 port);

#endif /* #ifndef _GMAC_MT7530_API_H_*/
