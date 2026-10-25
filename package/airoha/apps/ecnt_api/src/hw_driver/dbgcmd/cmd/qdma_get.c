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
************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libapi_lib_utility.h>
#include <libapi_lib_qdmamgr.h>
#include "qdma.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/
extern int api_lib_help(int argc, char *argv[], void *p);

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/
int do_get_txratemeter(int argc, char **argv, void *p);
int do_get_txq_cngst(int argc, char **argv, void *p);
int do_get_txqcngst_mode(int argc, char *argv[], void *p);
int do_get_static_cngst(int argc, char **argv, void *p);
int do_get_lmgr_status(int argc, char **argv, void *p);
int do_get_lmgr_low_threshold(int argc, char **argv, void *p);
int do_get_mac_resource_limit(int argc, char **argv, void *p);
int do_get_overhead(int argc, char **argv, void *p);
int do_get_txbuf_threshold(int argc, char **argv, void *p);
int do_dump_dbareport(int argc, char **argv, void *p);


/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/
const cmds_t QdmaGetCmds[] = {
/*	name,					func,						flags,	argcmin,		argc_errmsg */
	{"txratemeter",			do_get_txratemeter,			0,		0,			"Get QDMA tx ratemeter value"},
	{"txq_cngst",			do_get_txq_cngst,			0,		0,			"Get QDMA txq cngst config"},
	{"txqcngst_mode",		do_get_txqcngst_mode,		0,		0,			"Get QDMA txq cngst mode"},
	{"static_cngst",		do_get_static_cngst,		0,		0,			"Get QDMA static cngst config"},
	{"lmgr_status",			do_get_lmgr_status,			0,		0,			"Get QDMA LMGR Status"},
	{"lmgr_low_thrh",		do_get_lmgr_low_threshold,	0,		0,			"Get QDMA LMGR low threshold"},
	{"overhead",			do_get_overhead,			0,		0,			"Get QDMA OverHead value"},
	{"mac_limit",			do_get_mac_resource_limit,	0,		0,			"Get QDMA Mac Resource Limit"},
	{"txbuf_thrh",			do_get_txbuf_threshold,		0,		0,			"Get QDMA tx buffer threshold"},
	{"dbareport",			do_dump_dbareport,			0,		0,			"Get QDMA DBA Report value"},
	/* Cmd should be added before this line!  */
	{"help",				api_lib_help,				0,		0,			"Show qdmamgr get usage."},
	{NULL, NULL, 0, 0, NULL}
};

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/******************************************************************************
 Descriptor:	It's used to get qdma lmgr status:free dscp, used dscp, used buffer.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_txratemeter(int argc, char *argv[], void *p)
{
	int ret=0;	
	ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR div;
	ushort time_slice = 0;
	ushort windowSize = 0;
    	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		ret = qdmamgr_lib_get_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL_LAN,&windowSize,&time_slice);
		#else
		ret = qdmamgr_lib_get_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL_LAN,&div,&time_slice);
		#endif
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		ret = qdmamgr_lib_get_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL_WAN,&windowSize,&time_slice);
		#else
		ret = qdmamgr_lib_get_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL_WAN,&div,&time_slice);
		#endif
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	if(check_fail(ret))
		return QDMAMGR_FAIL;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	printf("%s TX Rate Meter:\nWindowSize:%u, TimeSlice:%uus\n\n", (char *)p
		, windowSize, time_slice);
#else
	printf("%s TX Rate Meter:\nTimeDivisor:%u, TimeSlice:%uus\n\n", (char *)p
		, 1<<div, time_slice);
#endif

	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to get qdma txq cngst infomation.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_txq_cngst(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_txq_cngst(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_txq_cngst(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to get QDMA txq cngst mode: dynamic/static.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_txqcngst_mode(int argc, char *argv[], void *p)
{
	int ret=0;
	u8 dynCngstEn = QDMA_DISABLE;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL_LAN,&dynCngstEn);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL_WAN,&dynCngstEn);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	printf("txq dyn cngst is %s\n", (dynCngstEn == QDMA_ENABLE)?"Enable":"Disable") ;

	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to get qdma static cngst infomation.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_static_cngst(int argc, char *argv[], void *p)
{
	int i=0;
	int ret=0;
	ECNT_QDMAMGR_TOTAL_THR totalThrhPtr;
	ECNT_QDMAMGR_STATIC_CFG staticInfo;	
	ECNT_QDMAMGR_STATIC_RATIO queueThrRatio;

	memset(&totalThrhPtr, 0, sizeof(ECNT_QDMAMGR_TOTAL_THR));
	memset(&staticInfo, 0, sizeof(ECNT_QDMAMGR_STATIC_CFG));
	memset(&queueThrRatio,0,sizeof(ECNT_QDMAMGR_STATIC_RATIO));
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_static_cngst_cfg(ECNT_QDMAMGR_QDMA_SEL_LAN,&totalThrhPtr,&staticInfo,&queueThrRatio);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_static_cngst_cfg(ECNT_QDMAMGR_QDMA_SEL_WAN,&totalThrhPtr,&staticInfo,&queueThrRatio);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	printf("toatl max threshold: %dkByte\n", totalThrhPtr.totalMaxThrh) ;
	printf("toatl min threshold: %dkByte\n", totalThrhPtr.totalMinThrh) ;
	printf("channel Idx %d,static threshold %s\n",staticInfo.chnlIdx,staticInfo.staticThrEn?"ENABLE":"DISABLE");
	printf("queue threshold ratio: ") ;
	for( i = 0 ; i <  ECNT_QDMAMGR_CHANNEL_QUEUE_NUM ; i++ ){
		printf("q%d-%d ", i, queueThrRatio.queueThrRatio[i]) ;
	}
	printf("\n") ;

	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to get qdma lmgr status:free dscp, used dscp, used buffer.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_lmgr_status(int argc, char *argv[], void *p)
{
	int ret=0;
	ECNT_QDMAMGR_LMGR_STATUS lmgrStatus;
    
	memset(&lmgrStatus, 0, sizeof(ECNT_QDMAMGR_LMGR_STATUS));
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_lmgr_status(ECNT_QDMAMGR_QDMA_SEL_LAN,&lmgrStatus);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_lmgr_status(ECNT_QDMAMGR_QDMA_SEL_WAN,&lmgrStatus);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	printf("%s LMGR Status:\nFree DSCP:%u\nUsed DSCP:%u\nUsed Memory:%uKByte\n\n"
		, (char *)p, lmgrStatus.freeLmgr, lmgrStatus.usedLmgr, lmgrStatus.usedBuf>>10);
	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to get qdma lmgr low threshold.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_lmgr_low_threshold(int argc, char *argv[], void *p)
{
	int ret=0;
	uint threshold=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL_LAN,&threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL_WAN,&threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	printf("%s LMGR Low Threshold:%u\n\n", (char *)p, threshold);
	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to get qdma overhead value.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_overhead(int argc, char *argv[], void *p)
{
	int ret=0;
	unchar mode=0, cnt=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_overhead_en(ECNT_QDMAMGR_QDMA_SEL_LAN,&mode) ;
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_overhead_en(ECNT_QDMAMGR_QDMA_SEL_WAN,&mode) ;
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_overhead_value(ECNT_QDMAMGR_QDMA_SEL_LAN,&cnt) ;
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_overhead_value(ECNT_QDMAMGR_QDMA_SEL_WAN,&cnt) ;
	}
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	printf("%s OVER HEAD Status:\nmode:%s, overhead size:%uByte\n\n"
		, (char *)p, ((mode==QDMA_ENABLE)?("Enable"):("Disable")), cnt);
	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to get qdma mac resource limit.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_mac_resource_limit(int argc, char *argv[], void *p)
{
	int ret=0;
	u32 channel_threshold,queue_threshold;
    	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL_LAN,&channel_threshold,&queue_threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL_WAN,&channel_threshold,&queue_threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	printf("%s mac resource limit:\nchannel threshold:%u\nqueue threshold:%u\n\n", (char *)p
		, channel_threshold, queue_threshold);
	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to get qdma lmgr low threshold.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_get_txbuf_threshold(int argc, char *argv[], void *p)
{
	int ret=0;
	u8 enable;
	u16 channel_threshold,total_threshold;
        
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL_LAN,&enable,&channel_threshold,&total_threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL_WAN,&enable,&channel_threshold,&total_threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	if(check_fail(ret))
		return QDMAMGR_FAIL;

#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
	printf("%s TX Buffer Threshold:\nmode:%s\nTotal Threshold:%uPage\nChannel Threshold:%uPage\n\n", (char *)p
		, ((enable ==QDMA_ENABLE)?("Enable"):("Disable")), total_threshold, channel_threshold);
#else
	printf("%s TX Buffer Threshold:\nmode:%s\nTotal Threshold:%uByte\nChannel Threshold:%uByte\n\n", (char *)p
		, ((enable ==QDMA_ENABLE)?("Enable"):("Disable")), total_threshold<<8, channel_threshold<<8);
#endif
	return QDMAMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to dump qdma DBA Report value.
 Input Args:	see QdmaGetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_dbareport(int argc, char *argv[], void *p)
{
	int ret=0, qdmaType=0;
	uint channel=0, channel_max=0;
	char *endPtr = NULL;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		qdmaType = ECNT_QDMAMGR_QDMA_SEL_LAN;
		channel_max = CONFIG_QDMA_LAN_CHANNEL;
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		qdmaType = ECNT_QDMAMGR_QDMA_SEL_WAN;
		channel_max = CONFIG_QDMA_WAN_CHANNEL;
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	if(argc>=2) {
		channel = strtol(argv[1], &endPtr, 0);
		if((*endPtr!='\0') || (channel>channel_max)) {
			printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], channel_max);
			return QDMAMGR_FAIL;
		}
	}
	
	if((argc<2) || (channel == channel_max)) {
		ret = qdmamgr_lib_dump_dbareport(qdmaType,channel_max) ;
	} else if(channel < channel_max){
		ret = qdmamgr_lib_dump_dbareport(qdmaType,channel) ;
	}
	
	if(check_fail(ret))
		return QDMAMGR_FAIL;

	return QDMAMGR_SUCCESS;
}

