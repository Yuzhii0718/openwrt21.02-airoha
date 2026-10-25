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
#ifndef _API_LIB_QOSRULE_H_
#define _API_LIB_QOSRULE_H_

/**
* \file  libapi_lib_qosrule.h  
* \brief This file is qosrule header file of business logic APIs that will be exported for others to use.
* \author shengli.lee
* \date     2020-10-16
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

#include <linux/version.h>

#define 	QOS_FOR_IPV6_OUT_PACKET	0
#define 	MAX_TYPE_PARAM_NUM 	16
#define 	SET_ATM_VLAN	0x1111
#define  LENTH_PARAM_VALUE		64
#define MAX_Uplink_BAND_WIDTH		1024
#define MAX_QOS_PARAM_LENGTH		(128)

#define HWNAT_METER_PATH 		"/proc/tc3162/hwnat_meter"

#define PPE_METER_DEL_ALL_CMD 		"/userfs/bin/ppe meter del group all"
#define PPE_METER_ADD_CMD 			"/userfs/bin/ppe meter add group %d mode byte scale K rate %d vlan 0 queue %d dir up"

#define	SW_ON		1
#define   SW_OFF		0

#define  TYPE_ADD 	1
#define  TYPE_DEL	0

enum IPVersion{
	IPNULL_FLAG,
	IPV4_FLAG,
	IPV6_FLAG,
	IPALL_FLAG,
};

enum EtherType{
	NULL_ETHER,
	IPV4_ETHER,
	IPV6_ETHER,
	ARP_ETHER,
};

#define QOSCMD_LOCK_FILE 			"/tmp/qoslockfd"
#define QOS_BROUTE  				"ebtables -t broute"
#define QOS_MANGLE					"iptables -t mangle"
#define QOS_MANGLE_IPV6				"ip6tables -t mangle"
#define MAX_RULE_LENGTH				128
#define MAX_APP_RULE_NUM			4
#define MAX_TEM_RULE_NUM			8
#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
#ifndef PQ_QUEUE_NUM
#define 	PQ_QUEUE_NUM			8
#endif
#ifndef WRR_QUEUE_NUM
#define 	WRR_QUEUE_NUM			8
#endif
#ifndef CAR_QUEUE_NUM
#define 	CAR_QUEUE_NUM			8
#endif
#else
#ifndef PQ_QUEUE_NUM
#define 	PQ_QUEUE_NUM			4
#endif
#ifndef WRR_QUEUE_NUM
#define 	WRR_QUEUE_NUM			4
#endif
#ifndef CAR_QUEUE_NUM
#define 	CAR_QUEUE_NUM			6
#endif
#endif

#define HWNAT_QOS_WR				"hwnat qos wr"
#define MAC_DEFAULT_MASK			"ff:ff:ff:ff:ff:ff"

#define	 MAX_BUF_LEN				64

#define LANIF						0
#define	WANIF						1

typedef unsigned long uint32;

typedef struct appParam_s{
	char appName[32];
	char dstIP[64];
	char dstProto[32];
	int dstPort;
	char queueNum[32];
}appParam_t;

typedef struct param_s{
	int 		flag;
	char 		*argname;
	char 		*ipv4Cmd;
	char		*ipv6Cmd;
	char 		value[MAX_QOS_PARAM_LENGTH];
}param_t;

typedef struct typeParam_s{
	char  paramName[32];
	char  value[MAX_QOS_PARAM_LENGTH];	
}typeParam_t;

typedef struct tsarm_ioctl{
	unsigned short int active;
	unsigned int vid;
	unsigned char vpi;
	unsigned char vci;
}tsarm_ioctl_t;

struct atmif_sioc {
    int number;
    int length;
    void *arg;
};


#define	INSMODE	1
#define	RMMODE		0

#define QOS_VECTOR	(1<<7)
#define MAX_VLAN_RULE_NUM		8
#define MAX_TYPE_RULE_NUM			10
#define MAX_APP_TYPE_NUM			15
#define MAX_CMD_LENGTH			256
#define CMD_BUF_LENGTH			64
#define MAX_TYPE_NUM				10


#define MAX_ARG_NUM	3
#define MAX_NODE_NAME 32

#define RTP_PRIORITY_QUEUE_PATH		"/proc/tc3162/RtpPriority"
#define QOS 					"QoS"
#define QOS_COMMON 				"Common"

#define TR069	1
#define VOIP		2
#define IPTV		3
#define INTERNET 4

#define TCAPI_PROCESS_OK	0

/**
 * \brief This function set qos rule on/off.
 *
 * This function  set qos rule on/off.submit by Shengli.Lee 2020.10.15
 * \param[in] QoSsw:1 means qos on, 0 means off
 * \param[in] IPVersion: 1=ipv4, 2=ipv6, 3=all 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos rule on/off.
 * \par Example
 */
int qoscmd_lib_do_qos_switch(int QoSsw, int IPversion);

/**
 * \brief This function set qos vlanid of ATM.
 *
 * This function set qos vlanid of ATM.submit by Shengli.Lee 2020.10.15
 * \param[in] sw:1 means on, 0 means off
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos vlanid of ATM.
 * \par Example
 */
int qoscmd_lib_do_qos_vlanid(int sw, int vpi, int vci, int vlanid);

/**
 * \brief This function set qos type.
 *
 * This function set qos type.submit by Shengli.Lee 2020.10.15
 * \param[in] type:1=add,0=delete
 * \param[in] ruleIdx:rule index
 * \param[in] IPVersion:1=ipv4, 2=ipv6, 3=all 
 * \param[in] paramNum:param Number
 * \param[in] paramArr[ ]:param
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos type.
 * \par Example
 */
int qoscmd_lib_do_qos_type(int type, int ruleIdx, int IPversion,int paramNum, typeParam_t paramArr[ ]);

/**
 * \brief This function set qos remark.
 *
 * This function set qos remark.submit by Shengli.Lee 2020.10.15
 * \param[in] type:1=add,0=delete
 * \param[in] ruleIdx:rule index
 * \param[in] IPVersion:1=ipv4, 2=ipv6, 3=all 
 * \param[in] paramNum:param Number
 * \param[in] paramArr[ ]:param
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos remark.
 * \par Example
 */
int qoscmd_lib_do_qos_remark (int type, int ruleIdx, int IPversion,int paramNum, typeParam_t paramArr[ ]);

/**
 * \brief This function set qos app.
 *
 * This function set qos app.submit by Shengli.Lee 2020.10.15
 * \param[in] type:1=add,0=delete
 * \param[in] ruleIdx:rule index
 * \param[in] IPVersion:1=ipv4, 2=ipv6, 3=all 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos app.
 * \par Example
 */
int qoscmd_lib_do_qos_app(int type, int ruleIdx, int IPversion, appParam_t* param);

/**
 * \brief This function set qos PQ.
 *
 * This function set qos PQ.submit by Shengli.Lee 2020.10.15
 * \param[in] uplinkBandwidth:uplink bandwidth
 * \param[in] qm:queue mask
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos PQ.
 * \par Example
 */
int qoscmd_lib_do_en75xxQoSDiscPQ(uint32 uplinkBandwidth,int qm);

/**
 * \brief This function set qos WRR.
 *
 * This function set qos WRR.submit by Shengli.Lee 2020.10.15
 * \param[in] uplinkBandwidth:uplink bandwidth
 * \param[in] qm:queue mask
 * \param[in] weight:qos wrr weight
 * \param[in] wTotal:weight total
 * \param[in] forcebw:force band wrr 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos WRR.
 * \par Example
 */
int qoscmd_lib_do_en75xxQoSDiscWRR(uint32 uplinkBandwidth,int qm, int* weight, int wTotal,int forcebw);

/**
 * \brief This function set qos CAR.
 *
 * This function set qos CAR.submit by Shengli.Lee 2020.10.15
 * \param[in] uplinkBandwidth:uplink bandwidth
 * \param[in] qm:queue mask
 * \param[in] ratelimit:ratelimit of queue
 * \param[in] meter_support: meter support
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos CAR.
 * \par Example
 */
int qoscmd_lib_do_en75xxQoSDiscCAR(uint32 uplinkBandwidth,int qm, uint32 *ratelimit,int meter_support);

/**
 * \brief This function set 7580 qos CAR.
 *
 * This function set 7580 qos CAR.submit by Shengli.Lee 2020.10.15
 * \param[in] uplinkBandwidth:uplink bandwidth
 * \param[in] qm:queue mask
 * \param[in] ratelimit:ratelimit of queue
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set 7580 qos CAR.
 * \par Example
 */
int qoscmd_lib_do_en7580QoSDiscCAR(uint32 uplinkBandwidth,int qm, uint32 *ratelimit);

/**
 * \brief This function set qos PQ.
 *
 * This function set qos PQ.submit by Shengli.Lee 2020.10.15
 * \param[in] uplink bandwidth
 * \param[in] queue mask
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos PQ.
 * \par Example
 */
int qoscmd_lib_do_qos_discPQ (char upBandWith[], char queueMask[]);

/**
 * \brief This function set qos WRR.
 *
 * This function set qos WRR.submit by Shengli.Lee 2020.10.15
 * \param[in] uplink bandwidth
 * \param[in] queue mask
 * \param[in] qos wrr weight
 * \param[in] force band wrr 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos WRR.
 * \par Example
 */
int qoscmd_lib_do_qos_discWRR(char wrrPri[][32], char upBandWith[], char queueMask[],char forceBW[] );

/**
 * \brief This function set qos CAR.
 *
 * This function set qos CAR.submit by Shengli.Lee 2020.10.15
 * \param[in] uplink bandwidth
 * \param[in] queue mask
 * \param[in] ratelimit of queue
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos CAR.
 * \par Example
 */
int qoscmd_lib_do_qos_discCAR(char carPri[][32],char upBandWith[], char queueMask[],char hwCar[] );

/**
 * \brief This function set qos config rate.
 *
 * This function set qos config rate.submit by Shengli.Lee 2021.01.14
 * \param[in] uplink bandwidth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos config rate.
 * \par Example
 */
int qoscmd_lib_do_qos_config_rate(char upBandWith[]);

/**
 * \brief This function set qos enable.
 *
 * This function set qos enable.submit by Shengli.Lee 2021.01.14
 * \param[in] enable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set qos enable.
 * \par Example
 */
int qoscmd_lib_do_qos_enable(int enable);

/**
 * \brief This function set dscp remark.
 *
 * This function set dscp remark.submit by Shengli.Lee 2020.10.15
 * \param[in] enable/disable
 * \param[in] wan interface
 * \param[in] dscp value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set dscp remark.
 * \par Example
 */
int qoscmd_lib_do_downlinkqos_dscpremark(int enable, char *wan_if, int dscp);

#endif
