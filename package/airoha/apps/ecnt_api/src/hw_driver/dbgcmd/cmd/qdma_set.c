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
extern int subcmd(const cmds_t tab[], int argc, char *argv[], void *p);
extern int api_lib_help(int argc, char *argv[], void *p);

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/
/* tx rate limit */
int do_set_txratemeter(int argc, char **argv, void *p);
int do_set_txratelimit(int argc, char **argv, void *p);
/* txq cngst */
int do_set_txq_cngst(int argc, char **argv, void *p);
int do_set_txqcngst_mode(int argc, char **argv, void *p);
int do_set_txqcngst_dei_scale(int argc, char **argv, void *p);
int do_set_txqcngst_autoconfig(int argc, char **argv, void *p);
int do_set_txqcngst_total_thrh(int argc, char **argv, void *p);
int do_set_txqcngst_channel_thrh(int argc, char **argv, void *p);
int do_set_txqcngst_queue_thrh(int argc, char **argv, void *p);
int do_set_txqcngst_peekrate(int argc, char **argv, void *p);
int do_set_static_cngst(int argc, char **argv, void *p);
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
int do_set_static_cngst_channel_en(int argc, char **argv, void *p);
int do_set_static_cngst_queue_ratio(int argc, char **argv, void *p);
#else
int do_set_static_cngst_normal(int argc, char **argv, void *p);
int do_set_static_cngst_dei(int argc, char **argv, void *p);
#endif
/* dbg cntr */
int do_set_dbgcntr(int argc, char **argv, void *p);
int do_set_dbgcntr_channel(int argc, char **argv, void *p);
int do_set_dbgcntr_queue(int argc, char **argv, void *p);
#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
int do_set_dbgcntr_ring(int argc, char **argv, void *p);
#endif
int do_clear_dbgcntr(int argc, char **argv, void *p);
int do_dump_dbgcntr(int argc, char **argv, void *p);
/* cpu counter */
int do_set_cpu_counter(int argc, char **argv, void *p);
int do_clear_cpu_counter(int argc, char **argv, void *p);
int do_dump_cpu_counter(int argc, char **argv, void *p);
int do_dump_register(int argc, char **argv, void *p);
int do_dump_descriptor(int argc, char **argv, void *p);
int do_dump_irq(int argc, char **argv, void *p);
int do_dump_hwfwd(int argc, char **argv, void *p);
int do_dump_all(int argc, char **argv, void *p);

/* test */
static int do_set_test(int argc, char **argv, void *p);
int do_set_test_dbglevel(int argc, char **argv, void *p);
int do_set_test_dma_busy(int argc, char **argv, void *p);
int do_set_test_reg_poll(int argc, char **argv, void *p);
int do_set_test_force_rx1(int argc, char **argv, void *p);
/* other */
int do_set_lmgr_low_threshold(int argc, char **argv, void *p);
int do_set_overhead(int argc, char **argv, void *p);
int do_set_mac_resource_limit(int argc, char **argv, void *p);
int do_set_txbuf_threshold(int argc, char **argv, void *p);
int do_set_prefetch(int argc, char **argv, void *p);
int do_set_dbareport(int argc, char **argv, void *p);


/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/
const cmds_t QdmaSetCmds[] = {
/*	name,					func,						flags,	argcmin,		argc_errmsg */
	{"txratemeter",			do_set_txratemeter,			0,		0,			"Set QDMA tx ratemeter value"},
	{"txratelimit",			do_set_txratelimit,			0,		0,			"Set QDMA tx ratelimit config"},
	{"txq_cngst",			do_set_txq_cngst,			0,		0,			"Set QDMA txq cngst"},	
	{"static_cngst",		do_set_static_cngst,		0,		0,			"Set QDMA txq static cngst"},	
	{"dbgcntr",				do_set_dbgcntr,				0,		0,			"Set QDMA dbg cntr"},
	{"cpu_counter",			do_set_cpu_counter,			0,		0,			"Set QDMA cpu counter"},
	{"register_dump",		do_dump_register,			0,		0,			"Dump QDMA register info"},
	{"descriptor_dump",		do_dump_descriptor,			0,		0,			"Dump QDMA descriptor info"},
	{"irq_dump",			do_dump_irq,				0,		0,			"Dump QDMA irq info"},
	{"hwfwd_dump",			do_dump_hwfwd,				0,		0,			"Dump QDMA hw_fwd info"},
	{"all_dump",			do_dump_all,				0,		0,			"Dump QDMA hw_fwd info"},
	{"test",				do_set_test,				0,		0,			"Set QDMA Test"},
	{"lmgr_low_thrh",		do_set_lmgr_low_threshold,	0,		0,			"Set QDMA LMGR low threshold"},
	{"overhead",			do_set_overhead,			0,		0,			"Set QDMA OverHead value"},
	{"mac_limit",			do_set_mac_resource_limit,	0,		0,			"Set QDMA Mac Resource Limit"},
	{"txbuf_thrh",			do_set_txbuf_threshold,		0,		0,			"Set QDMA tx buffer threshold"},
	{"prefetch",			do_set_prefetch,			0,		0,			"Set QDMA Prefetch config"},
	{"dbareport",			do_set_dbareport,			0,		0,			"Set QDMA DBA Report value"},
	/* Cmd should be added before this line!  */
	{"help",				api_lib_help,				0,		0,			"Show qdma set usage."},
	{NULL, NULL, 0, 0, NULL}
};

const cmds_t QdmaSetTxqCngstCmds[] = {
/*	name,			func,							flags,	argcmin,		argc_errmsg */
	{"mode",		do_set_txqcngst_mode,			0,		0,			"set QDMA txq cngst mode <dynamic/static>"},
	{"dei_scale",	do_set_txqcngst_dei_scale,		0,		0,			"set QDMA txq cngst dei scale <1/2~1/16>"},
	{"autoconfig",	do_set_txqcngst_autoconfig,		0,		0,			"set QDMA txq cngst auto config <cngstMode, margin, duration>"},
	{"total",		do_set_txqcngst_total_thrh,		0,		0,			"set QDMA txq cngst total threshold <totalMin, totalMax>"},
	{"channel",		do_set_txqcngst_channel_thrh,	0,		0,			"set QDMA txq cngst channel threshold <chnlMin, chnlMax>"},
	{"queue",		do_set_txqcngst_queue_thrh,		0,		0,			"set QDMA txq cngst queue threshold <queueMin, queueMax>"},
	{"peekrate",	do_set_txqcngst_peekrate,		0,		0,			"set QDMA txq cngst peek rate <peekEn, margin, duration>"},
	/* Cmd should be added before this line!  */
	{"help",		api_lib_help,					0,		0,			"Show qdma set txq cngst usage."},
	{NULL, NULL, 0, 0, NULL}
};

const cmds_t QdmaSetStaticCngstCmds[] = {
/*	name,		func,						flags,	argcmin,		argc_errmsg */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	{"channel",	do_set_static_cngst_channel_en,		0,		0,		"set static cngst channel index enable/disable"},
	{"q_ratio",	do_set_static_cngst_queue_ratio,	0,		0,		"set static cngst queue ratio <q[0], q[1], ... ,q[7]>"},
#else
	{"normal",	do_set_static_cngst_normal,	0,		0,			"set static cngst normal threshold <norm[0], norm[1], ... ,norm[7]>"},
	{"dei",		do_set_static_cngst_dei,	0,		0,			"set static cngst dei threshold <dei[0], dei[1], ... ,dei[7]>"},
#endif
	/* Cmd should be added before this line!  */
	{"help",	api_lib_help,				0,		0,			"Show qdma set static cngst usage."},
	{NULL, NULL, 0, 0, NULL}
};

const cmds_t QdmaSetDbgCntrCmds[] = {
/*	name,		func,						flags,	argcmin,		argc_errmsg */
	{"channel",	do_set_dbgcntr_channel,		0,		0,			"set dbgcntr as channel groups"},
	{"queue",	do_set_dbgcntr_queue,		0,		0,			"set dbgcntr as queue groups"},
#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
	{"ring",	do_set_dbgcntr_ring,		0,		0,			"set dbgcntr as ring groups"},
#endif
	{"clear",	do_clear_dbgcntr,			0,		0,			"Clear dbgcntr counters"},
	{"dump",	do_dump_dbgcntr,			0,		0,			"Dump dbgcntr counters"},
	/* Cmd should be added before this line!  */
	{"help",	api_lib_help,				0,		0,			"Show qdma set dbgcntr usage."},
	{NULL, NULL, 0, 0, NULL}
};

const cmds_t QdmaSetCpuCounterCmds[] = {
/*	name,		func,						flags,	argcmin,		argc_errmsg */
	{"clear",	do_clear_cpu_counter,		0,		0,			"Clear QDMA cpu counter"},
	{"dump",	do_dump_cpu_counter,		0,		0,			"Dump QDMA cpu counter"},
	/* Cmd should be added before this line!  */
	{"help",	api_lib_help,				0,		0,			"Show qdma set cpu counter usage."},
	{NULL, NULL, 0, 0, NULL}
};

const cmds_t QdmaSetTestCmds[] = {
/*	name,		func,						flags,	argcmin,		argc_errmsg */
	{"dbglevel",	do_set_test_dbglevel,	0,		0,			"Set QDMA debug level"},
	{"dma_busy",	do_set_test_dma_busy,	0,		0,			"Set QDMA dma busy timer"},
	{"reg_polling",	do_set_test_reg_poll,	0,		0,			"Set QDMA dma busy timer"},
	{"force_rx1",	do_set_test_force_rx1,	0,		0,			"Set QDMA force to RX1 enable or disable"},
	/* Cmd should be added before this line!  */
	{"help",		api_lib_help,			0,		0,			"Show qdma set cpu counter usage."},
	{NULL, NULL, 0, 0, NULL}
};


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/******************************************************************************
 Descriptor:	It's used to set QDMA tx rate meter value.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txratemeter(int argc, char *argv[], void *p)
{
	int ret = 0;
	uint windowSize=0;
	uint timeSlice=0 ;
	ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR div;
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<3)
		goto err;
	
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	windowSize = (uint)strtoul(argv[1], NULL, 10) ;
	if((windowSize < 1) || (windowSize > 31))
		goto err;
#else
	if(strncasecmp(argv[1], "1", strlen("1")) == 0) {
		div = ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_1;
	} else if(strncasecmp(argv[1], "2", strlen("2")) == 0) {
		div = ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_2;
	} else if(strncasecmp(argv[1], "4", strlen("4")) == 0) {
		div = ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_4;
	} else if(strncasecmp(argv[1], "8", strlen("8")) == 0) {
		div = ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR_8;
	} else {
		goto err;
	}
#endif
	
	timeSlice = (uint)strtoul(argv[2], NULL, 10) ;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if((timeSlice < 1) || (timeSlice > 2047))
		goto err;
#else
	if((timeSlice < 1) || (timeSlice > 65535))
		goto err;
#endif
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		ret = qdmamgr_lib_set_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL_LAN,(ushort)windowSize,(ushort)timeSlice );
		#else
		ret = qdmamgr_lib_set_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL_LAN,div,(ushort)timeSlice );
		#endif
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		ret = qdmamgr_lib_set_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL_WAN,(ushort)windowSize,(ushort)timeSlice );
		#else
		ret = qdmamgr_lib_set_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL_WAN,div,(ushort)timeSlice );
		#endif
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
	
err:
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	printf("Format: dbgmgr %s set txratemeter [windowSize:1~31] [timeSlice:1~2047(us)]\n", (char *)p) ;
#else
	printf("Format: dbgmgr %s set txratemeter [timeDivesel:1/2/4/8] [timeSlice:1~65535(us)]\n", (char *)p) ;
#endif
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA tx rate limit config.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txratelimit(int argc, char *argv[], void *p)
{
	int ret = 0;
	ECNT_QDMAMGR_EGS_RATELIMIT_CFG txRateLimitCfg;
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<3)
		goto err;
	/*only  can change ticker1, sync for 7521*/
	txRateLimitCfg.scale = (uint)strtoul(argv[1], NULL, 10) ;
	txRateLimitCfg.unit[1] = (uint)strtoul(argv[2], NULL, 10) ;
	if((txRateLimitCfg.scale >= ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_MAX)
	|| (txRateLimitCfg.unit[1] < 1) || (txRateLimitCfg.unit[1] > 65))
		goto err;
	
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL_LAN,&txRateLimitCfg);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL_WAN,&txRateLimitCfg);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set txratelimit [buckScale:0~%d(2^nByte)] [unit:1~65(kbps)]\n", (char *)p, ECNT_QDMAMGR_BUCKET_SCALE_FACTOR_MAX-1) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst Commands.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txq_cngst(int argc, char *argv[], void *p)
{
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	return subcmd(QdmaSetTxqCngstCmds, argc, argv, p);
}

/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst mode: dynamic/static.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txqcngst_mode(int argc, char *argv[], void *p)
{
	int ret=0;
	QDMA_Mode_t dynCngstEn;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<2)
		goto err;

	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) {
		dynCngstEn = QDMA_ENABLE;
	} else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) {
		dynCngstEn = QDMA_DISABLE;
	} else {
		goto err;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL_LAN,dynCngstEn);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL_WAN,dynCngstEn);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set txq_cngst mode [Enable/Disable]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst dei scale: 1/2~1/16.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txqcngst_dei_scale(int argc, char *argv[], void *p)
{
	int ret=0;
	ECNT_QDMAMGR_DEI_THR_SCALE threshold = 0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<2)
		goto err;

	if(strncasecmp(argv[1], "1/2", strlen("1/2")) == 0) {
		threshold = ECNT_QDMAMGR_DEI_THR_SCALE_2;
	} else if(strncasecmp(argv[1], "1/4", strlen("1/4")) == 0) {
		threshold = ECNT_QDMAMGR_DEI_THR_SCALE_4;
	} else if(strncasecmp(argv[1], "1/8", strlen("1/8")) == 0) {
		threshold = ECNT_QDMAMGR_DEI_THR_SCALE_8;
	} else if(strncasecmp(argv[1], "1/16", strlen("1/16")) == 0) {
		threshold = ECNT_QDMAMGR_DEI_THR_SCALE_16;
	} else {
		goto err;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_dei_scale(ECNT_QDMAMGR_QDMA_SEL_LAN,threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_dei_scale(ECNT_QDMAMGR_QDMA_SEL_WAN,threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set txq_cngst dei_scale [1/2, 1/4, 1/8, 1/16]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst autoconfig.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txqcngst_autoconfig(int argc, char *argv[], void *p)
{
	int ret=0;
	ECNT_QDMAMGR_TXCNGST txCngstCfg;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<2)
		goto err;

	memset(&txCngstCfg,0,sizeof(ECNT_QDMAMGR_TXCNGST));

	if(strncasecmp(argv[1], "static", strlen("static")) == 0) {
		txCngstCfg.mode = ECNT_QDMAMGR_TXCNGST_MODE_STATIC;
	} else if(strncasecmp(argv[1], "dynamic", strlen("dynamic")) == 0) {
		txCngstCfg.mode = ECNT_QDMAMGR_TXCNGST_MODE_NORMAL;
	} else if(strncasecmp(argv[1], "peekrate", strlen("peekrate")) == 0) {
		if(argc<4)
			goto err;
		txCngstCfg.mode = ECNT_QDMAMGR_TXCNGST_MODE_PEAKRATE_MARGIN;
		
		if(strncasecmp(argv[2], "0", strlen("0")) == 0) {
			txCngstCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_0;
		} else if(strncasecmp(argv[2], "25%", strlen("25%")) == 0) {
			txCngstCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_25;
		} else if(strncasecmp(argv[2], "50%", strlen("50%")) == 0) {
			txCngstCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_50;
		} else if(strncasecmp(argv[2], "100%", strlen("100%")) == 0) {
			txCngstCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_100;
		} else {
			goto err;
		}
		
		txCngstCfg.peak_rate_duration = (uint)strtoul(argv[3], NULL, 10) ;
		if((txCngstCfg.peak_rate_duration<16) || (txCngstCfg.peak_rate_duration>4080))
			goto err;
	} else {
		goto err;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_autoconfig(ECNT_QDMAMGR_QDMA_SEL_LAN,&txCngstCfg);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_autoconfig(ECNT_QDMAMGR_QDMA_SEL_WAN,&txCngstCfg);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format-1: dbgmgr %s set txq_cngst autoconfig [static/dynamic]\n", (char *)p) ;
	printf("Format-2: dbgmgr %s set txq_cngst autoconfig peekrate [margin:0/25%%/50%%/100%%] [duration:16~4080(ms)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst total threshold: totalMin & totalMax.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txqcngst_total_thrh(int argc, char *argv[], void *p)
{
	int ret=0;
	u16 max_threshold, min_threshold;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<3)
		goto err;

	min_threshold = (uint)strtoul(argv[1], NULL, 10) ;
	max_threshold = (uint)strtoul(argv[2], NULL, 10) ;
	if((min_threshold<1) || (min_threshold>4096))
		goto err;
	if((max_threshold<1) || (max_threshold>4096))
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_total_threshold(ECNT_QDMAMGR_QDMA_SEL_LAN,max_threshold,min_threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_total_threshold(ECNT_QDMAMGR_QDMA_SEL_WAN,max_threshold,min_threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set txq_cngst total [totalMin:1~4096(unit:kByte)] [totalMax:1~4096(unit:kByte)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst channel threshold: chnlMin & chnlMax.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txqcngst_channel_thrh(int argc, char *argv[], void *p)
{
	int ret=0;
	 u16 max_threshold, min_threshold;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<3)
		goto err;

	min_threshold = (uint)strtoul(argv[1], NULL, 10) ;
	max_threshold = (uint)strtoul(argv[2], NULL, 10) ;
	if((min_threshold<1) || (min_threshold>4096))
		goto err;
	if((max_threshold<1) || (max_threshold>4096))
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_channel_threshold(ECNT_QDMAMGR_QDMA_SEL_LAN,max_threshold,min_threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_channel_threshold(ECNT_QDMAMGR_QDMA_SEL_WAN,max_threshold,min_threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set txq_cngst channel [chnlMin:1~4096(unit:kByte)] [chnlMax:1~4096(unit:kByte)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}


/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst queue threshold: queueMin & queueMax.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txqcngst_queue_thrh(int argc, char *argv[], void *p)
{
	int ret=0;
	 u16 max_threshold,min_threshold;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<3)
		goto err;

	min_threshold = (uint)strtoul(argv[1], NULL, 10) ;
	max_threshold = (uint)strtoul(argv[2], NULL, 10) ;
	if((min_threshold<1) || (min_threshold>4096))
		goto err;
	if((max_threshold<1) || (max_threshold>4096))
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_queue_threshold(ECNT_QDMAMGR_QDMA_SEL_LAN,max_threshold,min_threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_queue_threshold(ECNT_QDMAMGR_QDMA_SEL_WAN,max_threshold,min_threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set txq_cngst queue [queueMin:1~4096(unit:kByte)] [queueMax:1~4096(unit:kByte)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA txq cngst peekrate.
 Input Args:	see QdmaSetTxqCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txqcngst_peekrate(int argc, char *argv[], void *p)
{
	int ret=0;
	ECNT_QDMAMGR_TXCNGST_PEAK_RATE_CFG peekrateCfg;
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<4)
		goto err;

	if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) {
		peekrateCfg.enable = QDMA_ENABLE;
	} else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) {
		peekrateCfg.enable = QDMA_DISABLE;
	} else {
		goto err;
	}
		
	if(strncasecmp(argv[2], "0", strlen("0")) == 0) {
		peekrateCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_0;
	} else if(strncasecmp(argv[2], "25%", strlen("25%")) == 0) {
		peekrateCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_25;
	} else if(strncasecmp(argv[2], "50%", strlen("50%")) == 0) {
		peekrateCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_50;
	} else if(strncasecmp(argv[2], "100%", strlen("100%")) == 0) {
		peekrateCfg.peak_rate_margin = ECNT_QDMAMGR_TXCNGST_PEAKRATE_MARGIN_100;
	} else {
		goto err;
	}
	
	peekrateCfg.peak_rate_duration = (uint)strtoul(argv[3], NULL, 10) ;
	if((peekrateCfg.peak_rate_duration<16) || (peekrateCfg.peak_rate_duration>4080))
		goto err;
		
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_peekrate(ECNT_QDMAMGR_QDMA_SEL_LAN,&peekrateCfg);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txq_cngst_peekrate(ECNT_QDMAMGR_QDMA_SEL_WAN,&peekrateCfg);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set txq_cngst peekrate [Enable/Disable] [margin:0/25%%/50%%/100%%] [duration:16~4080(ms)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA static cngst Commands.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_static_cngst(int argc, char *argv[], void *p)
{
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	return subcmd(QdmaSetStaticCngstCmds, argc, argv, p);
}

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
/******************************************************************************
 Descriptor:	It's used to set QDMA static cngst channel enable/disable.
 Input Args:	see QDMA_TxQStaticCfg_T.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_static_cngst_channel_en(int argc, char *argv[], void *p)
{
	int ret=0;
	uint normThrh=0;
	u8 chnlIdx,enable;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc < 3)
		goto err;

	chnlIdx = (unchar)strtoul(argv[1], NULL, 10);

	if(strncasecmp(argv[2], "Enable", strlen("Enable")) == 0) {
		enable = QDMA_ENABLE;
	}else if(strncasecmp(argv[2], "Disable", strlen("Disable")) == 0) {
		enable = QDMA_DISABLE;
	}else{
		goto err;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_channel_en(ECNT_QDMAMGR_QDMA_SEL_LAN,chnlIdx,enable);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_channel_en(ECNT_QDMAMGR_QDMA_SEL_WAN,chnlIdx,enable);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set static_cngst channel index enable/disable\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA static cngst queue ratio
 Input Args:	see QDMA_TxQStaticThrRatio_T.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_static_cngst_queue_ratio(int argc, char *argv[], void *p)
{
	int ret=0, i=0;
	ECNT_QDMAMGR_STATIC_RATIO staticThrhRatio;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc < (1+CONFIG_QDMA_QUEUE))
		goto err;

	for(i=0; i<CONFIG_QDMA_QUEUE; i++) {
		staticThrhRatio.queueThrRatio[i] = (unchar)strtoul(argv[i+1], NULL, 10);
		if(staticThrhRatio.queueThrRatio[i] > 15)
			goto err;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_queue_ratio(ECNT_QDMAMGR_QDMA_SEL_LAN,&staticThrhRatio);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_queue_ratio(ECNT_QDMAMGR_QDMA_SEL_WAN,&staticThrhRatio);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set static_cngst q_ratio [q_0: 0~15] ... [q_7: 0~15]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}
#else

/******************************************************************************
 Descriptor:	It's used to set QDMA static cngst normal threshold.
 Input Args:	see QdmaSetStaticCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_static_cngst_normal(int argc, char *argv[], void *p)
{
	int ret=0, i=0;
	uint normThrh=0;
	ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_NORMAL_THRESHOLD normCfg;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<1+CONFIG_QDMA_QUEUE)
		goto err;

	for(i=0; i<CONFIG_QDMA_QUEUE; i++) {
		normThrh = (uint)strtoul(argv[i+1], NULL, 10);
		if(normThrh>4096)
			goto err;
		normCfg.threshold[i] = normThrh;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_normal(ECNT_QDMAMGR_QDMA_SEL_LAN,&normCfg);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_normal(ECNT_QDMAMGR_QDMA_SEL_WAN,&normCfg);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set static_cngst normal [normThrh_0: 1~4096(unit:kByte)] ... [normThrh_7: 1~4096(unit:kByte)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA static cngst dei threshold.
 Input Args:	see QdmaSetStaticCngstCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_static_cngst_dei(int argc, char *argv[], void *p)
{
	int ret=0, i=0;
	uint deiThrh=0;
	ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_DEI_THRESHOLD deiCfg;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<1+CONFIG_QDMA_QUEUE)
		goto err;

	for(i=0; i<CONFIG_QDMA_QUEUE; i++) {
		deiThrh = (uint)strtoul(argv[i+1], NULL, 10);
		if(deiThrh>4096)
			goto err;
		deiCfg.threshold[i] = deiThrh;
	}

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_dei(ECNT_QDMAMGR_QDMA_SEL_LAN,&deiCfg);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_static_cngst_dei(ECNT_QDMAMGR_QDMA_SEL_WAN,&deiCfg);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set static_cngst dei [deiThrh_0: 1~4096(unit:kByte)] ... [deiThrh_7: 1~4096(unit:kByte)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}
#endif

/******************************************************************************
 Descriptor:	It's used to set QDMA DbgCntr Commands.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_dbgcntr(int argc, char *argv[], void *p)
{
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	return subcmd(QdmaSetDbgCntrCmds, argc, argv, p);
}

/******************************************************************************
 Descriptor:	It's used to set QDMA DbgCntr Channel related Commands.
 Input Args:	see QdmaSetDbgCntrCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_dbgcntr_channel(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_dbgcntr_channel(ECNT_QDMAMGR_QDMA_SEL_LAN,ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_TXCPU);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		if(argc < 2)
			goto err;
		if(strncasecmp(argv[1], "cpu", strlen("cpu")) == 0) {
			ret = qdmamgr_lib_set_dbgcntr_channel(ECNT_QDMAMGR_QDMA_SEL_WAN,ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_TXCPU);
		} else if(strncasecmp(argv[1], "fwd1", strlen("fwd1")) == 0) {
			ret = qdmamgr_lib_set_dbgcntr_channel(ECNT_QDMAMGR_QDMA_SEL_WAN,ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_TXFWD1);
		} else if(strncasecmp(argv[1], "fwd2", strlen("fwd2")) == 0) {
			ret = qdmamgr_lib_set_dbgcntr_channel(ECNT_QDMAMGR_QDMA_SEL_WAN,ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE_TXFWD2);
		} else {
			goto err;
		}
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set dbgcntr channel [cpu/fwd1/fwd2]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA DbgCntr Counters as queue groups.
 Input Args:	see QdmaSetDbgCntrCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_dbgcntr_queue(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_dbgcntr_queue(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_dbgcntr_queue(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);

	return 0;
}

#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
int do_set_dbgcntr_ring(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_dbgcntr_ring(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_dbgcntr_ring(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);

	return 0;
}
#endif

/******************************************************************************
 Descriptor:	It's used to clear QDMA DbgCntr Counters.
 Input Args:	see QdmaSetDbgCntrCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_clear_dbgcntr(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_clear_dbgcntr(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_clear_dbgcntr(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to dump QDMA DbgCntr Counters.
 Input Args:	see QdmaSetDbgCntrCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_dbgcntr(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_dbgcntr(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_dump_dbgcntr(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA CPU Counters.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_cpu_counter(int argc, char *argv[], void *p)
{
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	return subcmd(QdmaSetCpuCounterCmds, argc, argv, p);
}

/******************************************************************************
 Descriptor:	It's used to clear QDMA CPU Counters.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_clear_cpu_counter(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_clear_cpu_counter(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_clear_cpu_counter(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to dump QDMA CPU Counters.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_cpu_counter(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_cpu_counter(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_dump_cpu_counter(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to dump QADMA Register info.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_register(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_register(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_dump_register(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to dump QADMA descriptor info.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_descriptor(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_descriptor(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_dump_descriptor(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to dump QADMA IRQ info.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_irq(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_irq(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_dump_irq(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to dump QADMA hw fwd info.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_hwfwd(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_hwfwd(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_hwfwd(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to dump QADMA all info.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_dump_all(int argc, char *argv[], void *p)
{
	int ret=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_all(ECNT_QDMAMGR_QDMA_SEL_LAN);
	} else if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_dump_all(ECNT_QDMAMGR_QDMA_SEL_WAN);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
}

/******************************************************************************
 Descriptor:	It's used to set QDMA Test Commands.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
static int do_set_test(int argc, char *argv[], void *p)
{
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	return subcmd(QdmaSetTestCmds, argc, argv, p);
}

/******************************************************************************
 Descriptor:	It's used to set QADMA debug level.
 Input Args:	see QdmaSetTestCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_test_dbglevel(int argc, char *argv[], void *p)
{
	int ret=0;
	uint level=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<2)
		goto err;

	level = (uint)strtoul(argv[1], NULL, 10) ;
	if(level>=ECNT_QDMAMGR_DBG_LEVEL_MAX)
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_test_dbglevel(ECNT_QDMAMGR_QDMA_SEL_LAN,level);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_test_dbglevel(ECNT_QDMAMGR_QDMA_SEL_WAN,level);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set dbglevel [0~%d]\n", (char *)p, ECNT_QDMAMGR_DBG_LEVEL_MAX-1) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QADMA dma busy timer per 10ms.
 Input Args:	see QdmaSetTestCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_test_dma_busy(int argc, char *argv[], void *p)
{
	int ret=0;
	uint duration=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<2)
		goto err;

	duration = (uint)strtoul(argv[1], NULL, 10) ;
	if(duration>ECNT_QDMAMGR_BUSY_TIMER_MAX)
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_test_dma_busy(ECNT_QDMAMGR_QDMA_SEL_LAN,duration);
	} else if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_test_dma_busy(ECNT_QDMAMGR_QDMA_SEL_WAN,duration);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set dma_busy [0~%d(unit:second)]\n", (char *)p, ECNT_QDMAMGR_BUSY_TIMER_MAX) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QADMA register polling per 10ms.
 Input Args:	see QdmaSetTestCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_test_reg_poll(int argc, char *argv[], void *p)
{
	int ret=0;
	uint offset=0, duration=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<3)
		goto err;

	offset = (uint)strtoul(argv[1], NULL, 16) ;
	if(offset>=0x440)
		goto err;

	duration = (uint)strtoul(argv[2], NULL, 10) ;
	if(duration>ECNT_QDMAMGR_REG_POLLING_MAX)
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_test_reg_polling(ECNT_QDMAMGR_QDMA_SEL_LAN,offset, duration);
	} else if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_test_reg_polling(ECNT_QDMAMGR_QDMA_SEL_WAN,offset, duration);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set reg_polling [0~0x440] [0~%d(unit:second)]\n", (char *)p, ECNT_QDMAMGR_REG_POLLING_MAX) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QADMA receive packet force set to RX1.
 Input Args:	see QdmaSetTestCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_test_force_rx1(int argc, char *argv[], void *p)
{
	int ret=0;
	QDMA_Mode_t mode=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<2)
		goto err;
	
	if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) {
		mode = QDMA_ENABLE;
	} else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) {
		mode = QDMA_DISABLE;
	} else {
		goto err;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_test_force_rx1(ECNT_QDMAMGR_QDMA_SEL_LAN,mode);
	} else if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_test_force_rx1(ECNT_QDMAMGR_QDMA_SEL_WAN,mode);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set force_rx1 [Enable/Disable]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA lmgr low threshold.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_lmgr_low_threshold(int argc, char *argv[], void *p)
{
	int ret=0, qdmaType=0;
	uint threshold=0, dscpMax=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		qdmaType = ECNT_QDMAMGR_QDMA_SEL_LAN;
		dscpMax = 1024;
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		qdmaType = ECNT_QDMAMGR_QDMA_SEL_WAN;
		dscpMax = 4096;
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}

	if(argc<2)
		goto err;
	
	threshold = (uint)strtoul(argv[1], NULL, 10) ;
	if(threshold > dscpMax)
		goto err;
		
	ret = qdmamgr_lib_set_lmgr_low_threshold(qdmaType,threshold);
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set lmgr_low_thrh [threshold:0~%u]\n", (char *)p, dscpMax-1) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA overhead value.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_overhead(int argc, char *argv[], void *p)
{
	int ret=0, cnt=0;
	QDMA_Mode_t mode=0;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	if(argc<3)
		goto err;
	
	if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) {
		mode = QDMA_ENABLE;
	} else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) {
		mode = QDMA_DISABLE;
	} else {
		goto err;
	}
	
	cnt = (int)strtoul(argv[2], NULL, 10) ;
	if((cnt < 0) || (cnt > 255))
		goto err;
		
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_overhead_en(ECNT_QDMAMGR_QDMA_SEL_LAN,mode);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_overhead_en(ECNT_QDMAMGR_QDMA_SEL_WAN,mode);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	if(ret == ECNT_QDMAMGR_FAIL) {
		goto err;
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_overhead_value(ECNT_QDMAMGR_QDMA_SEL_LAN,cnt);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_overhead_value(ECNT_QDMAMGR_QDMA_SEL_WAN,cnt);
	}	
	if(ret == ECNT_QDMAMGR_FAIL) {
		goto err;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set overhead [Enable/Disable] [0~255(unit:Byte)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA mac resource limit.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_mac_resource_limit(int argc, char *argv[], void *p)
{
	int ret=0;
	u32 channel_threshold,queue_threshold;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif
	
	if(argc<3)
		goto err;	
	
	channel_threshold = (uint)strtoul(argv[1], NULL, 10) ;
	queue_threshold = (uint)strtoul(argv[2], NULL, 10) ;
	if((channel_threshold > 65536) || (queue_threshold > 65536))
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL_LAN,channel_threshold,queue_threshold);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL_WAN,channel_threshold,queue_threshold);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set mac_limit [chnlThrh:0~65536] [queueThrh:0~65536]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA tx buffer threshold.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_txbuf_threshold(int argc, char *argv[], void *p)
{
	int ret=0;
	uint totalThrh=0, chnlThrh=0;
	u8 enable;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif
	
	if(argc<2)
		goto err;
	
	if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) {
		enable = QDMA_ENABLE;
	} else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) {
		enable = QDMA_DISABLE;
	} else {
		goto err;
	}

	if(enable == QDMA_ENABLE) {
		if(argc<4)
			goto err;
		
		totalThrh = (uint)strtoul(argv[2], NULL, 10) ;
		chnlThrh = (uint)strtoul(argv[3], NULL, 10) ;
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
		if((totalThrh > 2047) || (chnlThrh > 2047))
			goto err;
#else
		if((totalThrh > 63) || (chnlThrh > 63))
			goto err;
		totalThrh = totalThrh<<2 ;
		chnlThrh = chnlThrh<<2 ;
#endif
	}
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL_LAN,enable,(ushort)chnlThrh,(ushort)totalThrh);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL_WAN,enable,(ushort)chnlThrh,(ushort)totalThrh);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
	printf("Format-1: dbgmgr %s set txbuf_thrh Enable [totalThrh:0~2047(unit:Page)] [chnlThrh:0~2047(unit:Page)]\n", (char *)p) ;
#else
	printf("Format-1: dbgmgr %s set txbuf_thrh Enable [totalThrh:0~63(unit:KByte)] [chnlThrh:0~63(unit:KByte)]\n", (char *)p) ;
#endif
	printf("Format-2: dbgmgr %s set txbuf_thrh Disable\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA prefetch config.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_prefetch(int argc, char *argv[], void *p)
{
	int ret=0;
	u8 pre_fetch_en,overdrag_en;
	u32 overdrag_cnt;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif
	
	if(argc<4)
		goto err;
	
	if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) {
		pre_fetch_en = QDMA_ENABLE;
	} else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) {
		pre_fetch_en = QDMA_DISABLE;
	} else {
		goto err;
	}
	
	if(strncasecmp(argv[2], "Enable", strlen("Enable")) == 0) {
		overdrag_en = QDMA_ENABLE;
	} else if(strncasecmp(argv[2], "Disable", strlen("Disable")) == 0) {
		overdrag_en = QDMA_DISABLE;
	} else {
		goto err;
	}
	
	overdrag_cnt = (uint)strtoul(argv[3], NULL, 10) ;
	if(overdrag_cnt > 32767)
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_set_prefetch(ECNT_QDMAMGR_QDMA_SEL_LAN,pre_fetch_en,overdrag_en,overdrag_cnt);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_prefetch(ECNT_QDMAMGR_QDMA_SEL_WAN,pre_fetch_en,overdrag_en,overdrag_cnt);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set prefetch [Enable/Disable] [overDragEn: Enable/Disable] [overDragValue:0~32767(unit:Byte)]\n", (char *)p) ;
	return QDMAMGR_FAIL;
}

/******************************************************************************
 Descriptor:	It's used to set QDMA DBA Report.
 Input Args:	see QdmaSetCmds.
 Ret Value:	success: QDMAMGR_SUCCESS
                     fail: QDMAMGR_FAIL
******************************************************************************/
int do_set_dbareport(int argc, char *argv[], void *p)
{
	int ret=0;
	uint channel=0, channel_max=0;
	char *endPtr = NULL;
	ECNT_QDMAMGR_DBA_REPORT txDbaReport;
	ECNT_QDMAMGR_EGS_RATELIMIT_CFG txRateLimitCfg;
	
#if API_CMD_QDMAMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	memset(&txRateLimitCfg, 0, sizeof(ECNT_QDMAMGR_EGS_RATELIMIT_CFG));
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		channel_max = CONFIG_QDMA_LAN_CHANNEL;
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		channel_max = CONFIG_QDMA_WAN_CHANNEL;
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}
	
	if(argc<6)
		goto err;
	
	channel = (uint)strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (channel>=channel_max)) {
		printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], channel_max-1);
		return QDMAMGR_FAIL;
	}
	
	txDbaReport.cir = (uint)strtoul(argv[2], NULL, 10) ;
	txDbaReport.cbs = (uint)strtoul(argv[3], NULL, 10) ;
	txDbaReport.pir = (uint)strtoul(argv[4], NULL, 10) ;
	txDbaReport.pbs = (uint)strtoul(argv[5], NULL, 10) ;
	if((txDbaReport.cir> (4<<20)) || (txDbaReport.cbs > 65535)
	|| (txDbaReport.pir > (4<<20)) || (txDbaReport.pbs > 65535))
		goto err;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {
		ret = qdmamgr_lib_get_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL_LAN,&txRateLimitCfg);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_get_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL_WAN,&txRateLimitCfg);
	} else {
		printf("Exec. Failed: argc is %d\n", argc) ;
		return QDMAMGR_FAIL;
	}	
	if(ret == ECNT_QDMAMGR_FAIL) {
		printf("Exec. Failed: 'dbgmgr %s set dbareport' command failed\n", (char *)p) ;
		return QDMAMGR_FAIL;
	}
	else if(ret == ECNT_QDMAMGR_NOT_SUPPORT) 
	{
		printf("NOT SUPPORT\n");
		return QDMAMGR_FAIL;
	}

	txDbaReport.cbs <<= txRateLimitCfg.scale ;
	txDbaReport.pbs <<= txRateLimitCfg.scale ;
	
	if(strcmp((char *)p, QDMA_TYPE_LAN) == 0) {	
		ret = qdmamgr_lib_set_dba_report(ECNT_QDMAMGR_QDMA_SEL_LAN,(unchar)channel,&txDbaReport);
	} else if(strcmp((char *)p, QDMA_TYPE_WAN) == 0) {
		ret = qdmamgr_lib_set_dba_report(ECNT_QDMAMGR_QDMA_SEL_WAN,(unchar)channel,&txDbaReport);
	}	
	
	return check_result(ret);
	
err:
	printf("Format: dbgmgr %s set dbareport [ChnlIdx:0~%d] [cir:1~1000000(unit:kbps)] [cbs:0~65535] [pir:1~1000000(unit:kbps)] [pbs:0~65535]\n", (char *)p, channel_max-1) ;
	return QDMAMGR_FAIL;
}

