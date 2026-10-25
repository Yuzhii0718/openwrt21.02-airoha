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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <libapi_lib_utility.h>
#include "libapi_lib_qdmamgr.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/
typedef enum
{
    QDMAMGR_FAIL = -1,
    QDMAMGR_SUCCESS = 0,
}QDMAMGR_RET;

#if defined(QDMA_LAN)
#if defined(TCSUPPORT_CPU_AN7583)
#define CONFIG_QDMA_CHANNEL 48
#elif defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
#define CONFIG_QDMA_CHANNEL 32
#elif defined(TCSUPPORT_CPU_EN7580)
#define CONFIG_QDMA_CHANNEL 13
#else
#define CONFIG_QDMA_CHANNEL 8
#endif
#else
#define CONFIG_QDMA_CHANNEL 32
#endif

#define CONFIG_QDMA_QUEUE 8

#define QUEUE_ALL_NUM (CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE)

#ifdef TCSUPPORT_CPU_EN7580
#define TX_RING_NUM 8
#else /*TCSUPPORT_CPU_EN7527 || TCSUPPORT_CPU_EN7516 and before */
#define TX_RING_NUM 2
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define RX_RING_NUM 32
#elif defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
#define RX_RING_NUM 16
#else
#define RX_RING_NUM 2
#endif

#define QDMAMGR_GET_METER_IDX(index) (index&0xff)
#define QDMAMGR_GET_METER_GRP(index) ((index>>8)&0x3)

///@brief QDMA flow cnt maxinum
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
#define ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM            (3)
#define ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM         (63)	/* ACNT_GRP[5:0] */
#define ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM         (31)	/* ACNT_GRP[10:6] */
#define ECNT_QDMAMGR_FLOWCNT_GRP2_MAX_IDX_NUM         (127)	/* MTR_GRP[6:0] */
#else
#define ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM            (2)
#define ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM         (31)
#define ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM         (16)
#endif

///@brief QDMA SLA channel maxinum
#define ECNT_QDMAMGR_SLA_MAX_NUM						(4)     /*EN7581 || EN7523: SLA support 4 channels at most*/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A   T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
************************************************************************/
extern ECNT_QDMAMGR_RET qdmamgr_lib_set_virtual_channel_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable, ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE mode);
extern ECNT_QDMAMGR_RET qdmamgr_lib_get_virtual_channel_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable, ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE *p_mode);
extern ECNT_QDMAMGR_RET qdmamgr_lib_set_virtual_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 phy_channel, ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS *p_qos);
extern ECNT_QDMAMGR_RET qdmamgr_lib_get_virtual_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 phy_channel, ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS *p_qos);
extern ECNT_QDMAMGR_RET qdmamgr_lib_set_rx_ring_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD *p_threshold);
extern ECNT_QDMAMGR_RET qdmamgr_lib_get_rx_ring_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD *p_threshold);
extern ECNT_QDMAMGR_RET qdmamgr_lib_set_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 threshold);
extern ECNT_QDMAMGR_RET qdmamgr_lib_get_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 *p_threshold);
extern ECNT_QDMAMGR_RET qdmamgr_lib_set_overhead_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable);
extern ECNT_QDMAMGR_RET qdmamgr_lib_get_overhead_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable);
extern ECNT_QDMAMGR_RET qdmamgr_lib_set_overhead_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 cnt);
extern ECNT_QDMAMGR_RET qdmamgr_lib_get_overhead_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_cnt);

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
***********************************************************************/


/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/
#ifdef QDMA_LAN
ECNT_QDMAMGR_QDMA_SEL g_qdma_sel = ECNT_QDMAMGR_QDMA_SEL_LAN;
char qdmamgr_name[] = "qdmamgr_lan";
#else
ECNT_QDMAMGR_QDMA_SEL g_qdma_sel = ECNT_QDMAMGR_QDMA_SEL_WAN;
char qdmamgr_name[] = "qdmamgr_wan";
#endif

static unchar qosTypeStr[ECNT_QDMAMGR_QOS_MODE_MAX][16] = 
{
    "WRR",
    "SP",
    "SPWRR7", 
    "SPWRR6", 
    "SPWRR5", 
    "SPWRR4", 
    "SPWRR3",
    "SPWRR2",
};

static unchar vChnlQosTypeStr[ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_MAX][16] = 
{
    "WRR",
    "SP",
    "SPWRR3",
    "SPWRR2",
};

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/*******************************************************************************/
/*  SET commands                                                                                                               */
/*******************************************************************************/
int do_set_qos_config(int argc, char *argv[], void *p)
{
    ECNT_QDMAMGR_WRR_WEIGHT_BASE weight_base;
    ECNT_QDMAMGR_WRR_WEIGHT_SCALE weight_scale;
    
    if (argc<3)
    {
        goto err;
    }
    
    if(strncasecmp(argv[1], "packet", strlen("packet")) == 0) 
    {
        weight_base = ECNT_QDMAMGR_WRR_WEIGHT_BASE_BY_PACKET;
    }
    else if(strncasecmp(argv[1], "byte", strlen("byte")) == 0) 
    {
        weight_base = ECNT_QDMAMGR_WRR_WEIGHT_BASE_BY_BYTE;
    }
    else 
    {
        goto err;
    }
    
#if defined(TCSUPPORT_CPU_EN7580)
    if(strncasecmp(argv[2], "1Byte", strlen("1Byte")) == 0) 
    {
        weight_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_1B;
    }
    else if(strncasecmp(argv[2], "16Byte", strlen("16Byte")) == 0) 
    {
        weight_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B;
    }
    else
    {
        goto err;
    }
#else
    if(strncasecmp(argv[2], "64Byte", strlen("64Byte")) == 0)
    {
        weight_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_64B;
    }
    else if(strncasecmp(argv[2], "16Byte", strlen("16Byte")) == 0)
    {
        weight_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B;
    }
    else 
    {
        goto err;
    }
#endif

    if(qdmamgr_lib_set_wrr_mode_cfg(g_qdma_sel, weight_base, weight_scale)) 
    {
        printf("Exec. Failed: '%s set qos config [weightBase] [weightScale]' command failed\n", qdmamgr_name);
        return -1;
    }
    
    printf("Exec. Successful!\n");
    return 0;

err:
    printf("Format: %s set qos config [packet/byte] [64Byte/16Byte]\n", qdmamgr_name);
    return -1;
}

int do_set_qos_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    int idx = 0, weight = 0;
    u8 channel = 0;
    ECNT_QDMAMGR_CHANNEL_QOS txQos = {0};
    int argcMin[ECNT_QDMAMGR_QOS_MODE_MAX] = {11, 3, 10, 9, 8, 7, 6, 5};
        
    /* get channel idx */
    channel = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (channel >= CONFIG_QDMA_CHANNEL)) 
    {
        printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
        goto err;
    }
    
    /* get qos type */
    for(idx = 0; idx < ECNT_QDMAMGR_QOS_MODE_MAX; idx++) 
    {
        if(strncasecmp(argv[2], (char *)qosTypeStr[idx], strlen((char *)argv[2])) == 0) 
        {
            txQos.qos_mode = idx;
            break;
        }
    }
    
    if(idx == ECNT_QDMAMGR_QOS_MODE_MAX) /* qosType error */
    {
        goto err;
    }
    
    /* get wrr weight */
    if(argc < argcMin[txQos.qos_mode]) 
    {
        printf("Fault: qosType %s need at least %d wrr weight\n", qosTypeStr[txQos.qos_mode], argcMin[txQos.qos_mode] - 3);
        goto err;
    }
    
    for(idx = 0; idx < ECNT_QDMAMGR_QOS_MODE_MAX; idx++) 
    {
        /* skipped the redundant wrr weight */
        if(idx >= argcMin[txQos.qos_mode] - 3)
            break;
            
        weight = strtol(argv[3 + idx], &endPtr, 0);
#ifndef TCSUPPORT_CPU_EN7580 /*EN7580: can set value = 0*/
        if((*endPtr!='\0') || (weight < 1) || (weight > 100)) 
        {
            printf("Fault: qos weight should not be 0, which should between 1 and 100!\n");
            goto err;
        }
#endif
        txQos.queue_weight[idx] = weight;
    }

    /* set qos value */
    if(qdmamgr_lib_set_channel_qos(g_qdma_sel, channel, &txQos)) 
    {
        printf("Exec. Failed: '%s set qos value [ChnlID] [qosType] [weight0~weight7]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set qos value [ChnlID:0~%d] [SP/WRR/SPWRR2/../SPWRR7] [weight0:1~100] ... [weightn:1~100]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL-1);
    return QDMAMGR_FAIL;
}

const cmds_t qos_set_command[] = 
{
    {"config",  do_set_qos_config,  0, 2, "Set QDMA qos config <weightBase, weightScale>"},
    {"value",   do_set_qos_value,   0, 2, "Set QDMA qos value <chnlIdx, qosType, weight0, ..., weight7>"},
    {"help",    api_lib_help,       0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,               0, 0, NULL}
};

int do_set_qos(int argc, char* argv[], void *p)
{
    return subcmd(qos_set_command, argc, argv, (void *)qos_set_command);
}

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
int do_general_set_glb_rate_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

	if(argc < 5)
	{
		goto err;
	}

	/*1. set glb module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;

	/*2. get glb module enable */
	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
	{
		cfg.enable = 1;
	}
	else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0)
	{
		cfg.enable = 0;
	}
	else
	{
		goto err;
	}
		
	/*3. get glb module working mode */
	if(strncasecmp(argv[2], "ratelimit", strlen("ratelimit")) == 0)
	{
		cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT;
	}
	else if(strncasecmp(argv[2], "trtcm", strlen("trtcm")) == 0)
	{
		cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_TRTCM;
	}
	else
	{
		goto err;
	}

	/*4. get slow tick ratio */
	cfg.slow_tick_ratio = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') 
	{
		printf("Error: input slowtick ratio is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/*5. get fast tick */
	cfg.fast_tick = strtoul(argv[4], &endPtr, 0);
	if(*endPtr!='\0') 
	{
		printf("Error: input fasttick is '%s', which should be digital\n", argv[4]);
		goto err;
	}

	/* set tx ratelimit/trtcm module init */
	if (qdmamgr_lib_set_ratectl_cfg(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: %s set general_glb_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n");
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_glb_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
	return QDMAMGR_FAIL;
}

int do_general_set_glb_ratelimit_mode_cfg(int argc, char *argv[], void *p)
{
	ECNT_QDMAMGR_RATELIMIT_MODE_CFG cfg = {0};

	if(argc<4)
	{
		goto err;
	}

	/*1. set glb module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;

	/*3. get glb enable mode */
	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
	{
		cfg.enable = 1;
	} 
	else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) 
	{
		cfg.enable = 0;
	} 
	else 
	{
		goto err;
	}
		
	/*4. get glb rate limit packet mode */
	if(strncasecmp(argv[2], "packet", strlen("packet")) == 0) 
	{
		cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
	} 
	else if(strncasecmp(argv[2], "byte", strlen("byte")) == 0) 
	{
		cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
	} 
	else 
	{
		goto err;
	}

	/*5. get glb rate limit tickSel index */
	if(strncasecmp(argv[3], "fast", strlen("fast")) == 0) 
	{
		cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
	} 
	else if(strncasecmp(argv[3], "slow", strlen("slow")) == 0) 
	{
		cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
	} 
	else 
	{
		goto err;
	}

	/* set glb rate limit config */
	if(qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: %s set general_glb_ratelimit config [Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name);
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n");
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_glb_ratelimit config [Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name);
	return QDMAMGR_FAIL;
}

int do_general_set_glb_ratelimit_mode_value(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

	if(argc<2)
	{
		goto err;
	}

	/* 1. set glb module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;

	/* 2. no need index, set any value */
	cfg.index = 0;
	
	/* 3. get glb rate limit value */
	cfg.rate = strtoul(argv[1], &endPtr, 0);
	if(*endPtr!='\0') 
	{
		printf("Error: input tx rate limit value is '%s', which should be digital\n", argv[1]);
		goto err;
	}
	
	if(cfg.rate > (0xa<<20)) 
	{
		printf("Error: tx rate limit value is %d, which shold less than 10000000!", cfg.rate);
		goto err;
	}

	/* set glb rate limit value */
	if (qdmamgr_lib_set_ratectl_ratelimit_mode_value(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: '%s set general_glb_ratelimit value [value]' command failed\n", qdmamgr_name);
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n");
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_glb_ratelimit value [1~10000000]\n", qdmamgr_name);
	return QDMAMGR_FAIL;
}

int do_general_set_glb_ratelimit_bucket_size(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

	if(argc<2)
		goto err;

	/* 1. set glb module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;

	/* 2. no need index, set any value */
	cfg.index = 0;
	
	/* 3. get bucket size value */
	cfg.bucket_size = strtoul(argv[1], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input glb bucket size value is '%s', which should be digital\n", argv[1]);
		goto err;
	}
	if(cfg.bucket_size > (0x8<<20)) {
		printf("Error: tx rate limit value is %d, which shold less than 8000000!", cfg.bucket_size);
		goto err;
	}

	/* set bucket size value */
	if(qdmamgr_lib_set_ratectl_ratelimit_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_glb_ratelimit bsize [value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_glb_ratelimit bsize [1~8000000]\n", qdmamgr_name) ;
	return QDMAMGR_FAIL;
}

const cmds_t general_set_glb_ratelimit_command[] = {
	{"config",				do_general_set_glb_ratelimit_mode_cfg,  0,		1,			"Set QDMA general_glb_ratelimit config <enMode, pktMode, tickSelIdx>"},
	{"value",				do_general_set_glb_ratelimit_mode_value,0,		1,			"Set QDMA general_glb_ratelimit value <value>"},
	{"bsize",				do_general_set_glb_ratelimit_bucket_size,0,		1,			"Set QDMA general_glb_ratelimit bsize <value>"},
	{"help",				api_lib_help,							0,		0,			"Show qdmamgr set usage."},
	{NULL, NULL, 0, 0, NULL}
};

int do_general_set_glb_ratelimit_mode(int argc, char *argv[], void *p)
{
	return subcmd(general_set_glb_ratelimit_command, argc, argv, (void *)general_set_glb_ratelimit_command);
}
#endif

#if defined(TCSUPPORT_CPU_EN7580)
int do_general_set_tx_trtcm_cfg(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

    if(argc < 5)
    {
        goto err;
    }

    /*1. set tx module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;

    /*2. get tx module enable */
    if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
    {
        cfg.enable = 1;
    }
    else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0)
    {
        cfg.enable = 0;
    }
    else
    {
        goto err;
    }
        
    /*3. get tx module working mode */
    if(strncasecmp(argv[2], "ratelimit", strlen("ratelimit")) == 0)
    {
        cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT;
    }
    else if(strncasecmp(argv[2], "trtcm", strlen("trtcm")) == 0)
    {
        cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_TRTCM;
    }
    else
    {
        goto err;
    }

    /*4. get slow tick ratio */
    cfg.slow_tick_ratio = strtoul(argv[3], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input slowtick ratio is '%s', which should be digital\n", argv[3]);
        goto err;
    }

    /*5. get fast tick */
    cfg.fast_tick = strtoul(argv[4], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input fasttick is '%s', which should be digital\n", argv[4]);
        goto err;
    }

    /* set tx ratelimit/trtcm module init */
    if (qdmamgr_lib_set_ratectl_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: %s set general_tx_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_tx_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_general_set_tx_ratelimit_mode_cfg(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_RATELIMIT_MODE_CFG cfg = {0};

    if(argc<5)
    {
        goto err;
    }

    /*1. set tx module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;

    /*2. get tx index */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (cfg.index >= CONFIG_QDMA_CHANNEL)) 
    {
        printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
        goto err;
    }

    /*3. get tx enable mode */
    if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) 
    {
        cfg.enable = 1;
    } 
    else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) 
    {
        cfg.enable = 0;
    } 
    else 
    {
        goto err;
    }
        
    /*4. get tx rate limit packet mode */
    if(strncasecmp(argv[3], "packet", strlen("packet")) == 0) 
    {
        cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
    } 
    else if(strncasecmp(argv[3], "byte", strlen("byte")) == 0) 
    {
        cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
    } 
    else 
    {
        goto err;
    }

    /*5. get tx rate limit tickSel index */
    if(strncasecmp(argv[4], "fast", strlen("fast")) == 0) 
    {
        cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
    } 
    else if(strncasecmp(argv[4], "slow", strlen("slow")) == 0) 
    {
        cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
    } 
    else 
    {
        goto err;
    }

    /* set tx rate limit config */
    if(qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: %s set general_tx_ratelimit config [ChnlID] [Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_tx_ratelimit config [ChnlID:0~%d][Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
    return QDMAMGR_FAIL;
}

int do_general_set_tx_ratelimit_mode_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

    if(argc<3)
    {
        goto err;
    }

    /* 1. set tx module */
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;

    /* 2. get channel idx */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (cfg.index >= CONFIG_QDMA_CHANNEL)) 
    {
        printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
        goto err;
    }
    
    /* 3. get tx rate limit value */
    cfg.rate = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx rate limit value is '%s', which should be digital\n", argv[2]);
        goto err;
    }
    
    if(cfg.rate > (0xa<<20)) 
    {
        printf("Error: tx rate limit value is %d, which shold less than 10000000!", cfg.rate);
        goto err;
    }

    /* set tx rate limit value */
    if (qdmamgr_lib_set_ratectl_ratelimit_mode_value(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s set general_tx_ratelimit value [ChnlID] [value]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_tx_ratelimit value [ChnlID:0~%d] [1~10000000]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
    return QDMAMGR_FAIL;
}

int do_general_set_tx_ratelimit_bucket_size(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

	if(argc<3)
		goto err;

	/* 1. set tx module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;

	/* 2. get channel idx */
	cfg.index = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (cfg.index>=CONFIG_QDMA_CHANNEL)) {
		printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL-1);
		goto err;
	}
	
	/* 3. get tx bucketsize */
	cfg.bucket_size = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input tx BucketSize value is '%s', which should be digital\n", argv[2]);
		goto err;
	}
	if(cfg.bucket_size > (0x400<<17)) {
		printf("Error: BucketSize is %d, which shold less than 128M!", cfg.bucket_size);
		goto err;
	}

	/* set tx  bucketsize value */
	if(qdmamgr_lib_set_ratectl_ratelimit_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_tx_ratelimit bsize [ChnlID] [value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_tx_ratelimit bsize [ChnlID:0~%d] [1~128000000]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL-1) ;
	return QDMAMGR_FAIL;
}

const cmds_t general_set_tx_ratelimit_command[] = 
{
    {"config",  do_general_set_tx_ratelimit_mode_cfg,   0, 2, "Set QDMA general_tx_ratelimit config <chnlIdx, enMode, pktMode, tickSelIdx>"},
    {"value",   do_general_set_tx_ratelimit_mode_value, 0, 2, "Set QDMA general_tx_ratelimit value <chnlIdx, value>"},
    {"bsize",	do_general_set_tx_ratelimit_bucket_size,0, 2, "Set QDMA general_tx_ratelimit bsize <chnlIdx, value>"},
    {"help",    api_lib_help,                           0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,                                   0, 0, NULL}
};

int do_general_set_tx_ratelimit_mode(int argc, char *argv[], void *p)
{
    return subcmd(general_set_tx_ratelimit_command, argc, argv, (void *)general_set_tx_ratelimit_command);
}

#if defined(TCSUPPORT_CPU_EN7581)
int do_general_set_tx_queue_ratelimit_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

	if(argc < 4)
	{
		goto err;
	}

	/*1. set tx module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT;

	/*2. get tx module enable */
	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
	{
		cfg.enable = 1;
	}
	else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0)
	{
		cfg.enable = 0;
	}
	else
	{
		goto err;
	}
		
	/*3. get tx module working mode */
	cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT;

	/*4. get slow tick ratio */
	cfg.slow_tick_ratio = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') 
	{
		printf("Error: input slowtick ratio is '%s', which should be digital\n", argv[2]);
		goto err;
	}

	/*5. get fast tick */
	cfg.fast_tick = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') 
	{
		printf("Error: input fasttick is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/* set tx ratelimit/trtcm module init */
	if (qdmamgr_lib_set_ratectl_cfg(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: %s set general_tx_q_init [enable/disable] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n");
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_tx_q_init [enable/disable] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
	return QDMAMGR_FAIL;
}

int do_general_set_tx_queue_ratelimit_mode_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_CFG cfg = {0};

	if(argc<5)
	{
		goto err;
	}

	/*1. set tx module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT;

	/*2. get tx index */
	cfg.index = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (cfg.index >= QUEUE_ALL_NUM)) 
	{
		printf("Error: input queue idx is '%s', which should between 0 and %d\n", argv[1], QUEUE_ALL_NUM - 1);
		goto err;
	}

	/*3. get tx enable mode */
	if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) 
	{
		cfg.enable = 1;
	} 
	else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) 
	{
		cfg.enable = 0;
	} 
	else 
	{
		goto err;
	}
		
	/*4. get tx rate limit packet mode */
	if(strncasecmp(argv[3], "packet", strlen("packet")) == 0) 
	{
		cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
	} 
	else if(strncasecmp(argv[3], "byte", strlen("byte")) == 0) 
	{
		cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
	} 
	else 
	{
		goto err;
	}

	/*5. get tx rate limit tickSel index */
	if(strncasecmp(argv[4], "fast", strlen("fast")) == 0) 
	{
		cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
	} 
	else if(strncasecmp(argv[4], "slow", strlen("slow")) == 0) 
	{
		cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
	} 
	else 
	{
		goto err;
	}

	/* set tx queue rate limit config */
	if(qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: %s set general_tx_q_ratelimit config [QueID] [Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name);
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n");
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_tx_q_ratelimit config [QueID:0~%d][Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name, QUEUE_ALL_NUM - 1);
	return QDMAMGR_FAIL;
}

int do_general_set_tx_queue_ratelimit_mode_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

    if(argc<3)
    {
        goto err;
    }

    /* 1. set tx module */
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT;

    /* 2. get queue idx */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (cfg.index >= QUEUE_ALL_NUM)) 
    {
        printf("Error: input queue is '%s', which should between 0 and %d\n", argv[1], QUEUE_ALL_NUM - 1);
        goto err;
    }
    
    /* 3. get tx rate limit value */
    cfg.rate = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx rate limit value is '%s', which should be digital\n", argv[2]);
        goto err;
    }
    
    if(cfg.rate > (0xa<<20)) 
    {
        printf("Error: tx rate limit value is %d, which shold less than 10000000!", cfg.rate);
        goto err;
    }

    /* set tx rate limit value */
    if (qdmamgr_lib_set_ratectl_ratelimit_mode_value(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s set general_tx_q_ratelimit value [QueID] [value]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_tx_q_ratelimit value [QueID:0~%d] [1~10000000]\n", qdmamgr_name, QUEUE_ALL_NUM - 1);
    return QDMAMGR_FAIL;
}

int do_general_set_tx_queue_ratelimit_bucket_size(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

	if(argc<3)
		goto err;

	/* 1. set tx module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT;

	/* 2. get queue idx */
	cfg.index = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (cfg.index>=QUEUE_ALL_NUM)) {
		printf("Error: input queue is '%s', which should between 0 and %d\n", argv[1], QUEUE_ALL_NUM-1);
		goto err;
	}
	
	/* 3. get tx bucketsize */
	cfg.bucket_size = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input tx BucketSize value is '%s', which should be digital\n", argv[2]);
		goto err;
	}
	if(cfg.bucket_size > (0x400<<17)) {
		printf("Error: BucketSize is %d, which shold less than 128M!", cfg.bucket_size);
		goto err;
	}

	/* set tx  bucketsize value */
	if(qdmamgr_lib_set_ratectl_ratelimit_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_tx_q_ratelimit bsize [QueID] [value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_tx_q_ratelimit bsize [QueID:0~%d] [1~128000000]\n", qdmamgr_name, QUEUE_ALL_NUM-1) ;
	return QDMAMGR_FAIL;
}

const cmds_t general_set_tx_queue_ratelimit_command[] = 
{
    {"config",  do_general_set_tx_queue_ratelimit_mode_cfg,   0, 2, "Set QDMA general_tx_q_ratelimit config <queueIdx, enMode, pktMode, tickSelIdx>"},
    {"value",   do_general_set_tx_queue_ratelimit_mode_value, 0, 2, "Set QDMA general_tx_q_ratelimit value <queueIdx, value>"},
    {"bsize",	do_general_set_tx_queue_ratelimit_bucket_size,0, 2, "Set QDMA general_tx_q_ratelimit bsize <queueIdx, value>"},
    {"help",    api_lib_help,                           0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,                                   0, 0, NULL}
};

int do_general_set_tx_queue_ratelimit_mode(int argc, char *argv[], void *p)
{
    return subcmd(general_set_tx_queue_ratelimit_command, argc, argv, (void *)general_set_tx_queue_ratelimit_command);
}
#endif


int do_general_set_tx_trtcm_mode_cfg(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_TRTCM_MODE_CFG cfg = {0};

    if(argc<8)
    {
        goto err;
    }

    /*1. set tx trtcm module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;

    /*2. get tx trtcm index */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (cfg.index >= CONFIG_QDMA_CHANNEL)) 
    {
        printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
        goto err;
    }

    /*3. get tx trtcm cbs enable */
    if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) 
    {
        cfg.cbs_meter_en = 1;
    } 
    else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) 
    {
        cfg.cbs_meter_en = 0;
    } 
    else 
    {
        goto err;
    }
        
    /*4. get tx trtcm cbs packet mode */
    if(strncasecmp(argv[3], "packet", strlen("packet")) == 0) 
    {
        cfg.cbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
    } 
    else if(strncasecmp(argv[3], "byte", strlen("byte")) == 0) 
    {
        cfg.cbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
    } 
    else 
    {
        goto err;
    }

    /*5. get tx trtcm cbs tickSel index */
    if(strncasecmp(argv[4], "fast", strlen("fast")) == 0) 
    {
        cfg.cbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
    } 
    else if(strncasecmp(argv[4], "slow", strlen("slow")) == 0) 
    {
        cfg.cbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
    } 
    else 
    {
        goto err;
    }

    /*6. get tx trtcm pbs enable */
    if(strncasecmp(argv[5], "enable", strlen("enable")) == 0) 
    {
        cfg.pbs_meter_en = 1;
    } 
    else if(strncasecmp(argv[5], "disable", strlen("disable")) == 0) 
    {
        cfg.pbs_meter_en = 0;
    } 
    else 
    {
        goto err;
    }
        
    /*7. get tx trtcm pbs packet mode */
    if(strncasecmp(argv[6], "packet", strlen("packet")) == 0) 
    {
        cfg.pbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
    } 
    else if(strncasecmp(argv[6], "byte", strlen("byte")) == 0) 
    {
        cfg.pbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
    } 
    else 
    {
        goto err;
    }

    /*8. get tx trtcm pbs tickSel index */
    if(strncasecmp(argv[7], "fast", strlen("fast")) == 0) 
    {
        cfg.pbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
    } 
    else if(strncasecmp(argv[7], "slow", strlen("slow")) == 0) 
    {
        cfg.pbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
    } 
    else 
    {
        goto err;
    }

    /* set tx rate limit config */
    if(qdmamgr_lib_set_ratectl_trtcm_mode_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: %s set general_tx_trtcm config [ChnlID] (cbs:)[Enable/Disable] [packet/byte] [tickSel:fast/slow] (pbs)[Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_tx_trtcm config [ChnlID:0~%d] (cbs:)[Enable/Disable] [packet/byte] [tickSel:fast/slow] (pbs)[Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
    return QDMAMGR_FAIL;

}

int do_general_set_tx_trtcm_mode_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_TRTCM_MODE_VALUE cfg = {0};

    if(argc<4)
    {
        goto err;
    }

    /* 1. set tx module */
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;

    /* 2. get channel idx */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (cfg.index >= CONFIG_QDMA_CHANNEL)) 
    {
        printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
        goto err;
    }
    
    /* 3. get tx trtcm cbs value */
    cfg.cbs = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx CbsParamValue is '%s', which should be digital\n", argv[2]);
        goto err;
    }

    /* 4. get tx trtcm pbs value */
    cfg.pbs= strtoul(argv[3], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx PbsParamValue is '%s', which should be digital\n", argv[3]);
        goto err;
    }

    /* set tx rate limit value */
    if(qdmamgr_lib_set_ratectl_trtcm_mode_value(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s set general_tx_trtcm value [ChnlID] [cbs_value] [pbs_value]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_tx_trtcm value [ChnlID:0~%d] [1~10000000] [1~10000000]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
    return QDMAMGR_FAIL;
}

int do_general_set_tx_trtcm_bucket_size(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_TRTCM_MODE_VALUE cfg = {0};

	if(argc<4)
		goto err;

	/* 1. set tx module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;

	/* 2. get channel idx */
	cfg.index = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (cfg.index>=CONFIG_QDMA_CHANNEL)) {
		printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL-1);
		goto err;
	}
	
	/* 3. get tx trtcm cbs bucket value */
	cfg.cbs_bucket_size = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input tx CbsBucketSize is '%s', which should be digital\n", argv[2]);
		goto err;
	}

	/* 4. get tx trtcm pbs bucket value */
	cfg.pbs_bucket_size = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input tx PbsBucketSize is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/* set tx bucket size value */
	if(qdmamgr_lib_set_ratectl_trtcm_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_tx_trtcm bsize [ChnlID] [cbs_value] [pbs_value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_tx_trtcm bsize [ChnlID:0~%d] [1~128000000] [1~128000000]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL-1) ;
	return QDMAMGR_FAIL;
}

const cmds_t general_set_tx_trtcm_command[] = 
{
    {"config",  do_general_set_tx_trtcm_mode_cfg,       0, 2, "Set QDMA general_tx_trtcm config <chnlIdx, cbs_enMode, cbs_pktMode, cbs_tickSelIdx, pbs_enMode, pbs_pktMode, pbs_tickSelIdx>"},
    {"value",   do_general_set_tx_trtcm_mode_value,     0, 2, "Set QDMA general_tx_trtcm value <chnlIdx, cbs_value, pbs_value>"},
    {"bsize",	do_general_set_tx_trtcm_bucket_size,	0, 2, "Set QDMA general_tx_trtcm bsize <chnlIdx, cbs_value, pbs_value>"},
    {"help",    api_lib_help,                           0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,                                   0, 0, NULL}
};

int do_general_set_tx_trtcm_mode(int argc, char *argv[], void *p)
{
    return subcmd(general_set_tx_trtcm_command, argc, argv, (void *)general_set_tx_trtcm_command);
}

int do_general_set_rx_trtcm_cfg(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

    if(argc<5)
    {
        goto err;
    }

    /*1. set rx module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;

    /*2. get rx module enable */
    if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
    {
        cfg.enable = 1;
    } 
    else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) 
    {
        cfg.enable = 0;
    } 
    else 
    {
        goto err;
    }
        
    /*3. get rx module working mode */
    if(strncasecmp(argv[2], "ratelimit", strlen("ratelimit")) == 0) 
    {
        cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT;
    } 
    else if(strncasecmp(argv[2], "trtcm", strlen("trtcm")) == 0) 
    {
        cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_TRTCM;
    } 
    else 
    {
        goto err;
    }

    /*4. get slow tick ratio */
    cfg.slow_tick_ratio = strtoul(argv[3], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input slowtick ratio is '%s', which should be digital\n", argv[3]);
        goto err;
    }

    /*5. get fast tick */
    cfg.fast_tick = strtoul(argv[4], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input fasttick is '%s', which should be digital\n", argv[4]);
        goto err;
    }

    /* set tx ratelimit/trtcm module init */
    if(qdmamgr_lib_set_ratectl_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s set general_rx_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_rx_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_general_set_rx_ratelimit_mode_cfg(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_RATELIMIT_MODE_CFG cfg = {0};
    unsigned char meterIdMax[3] = {128,32,16};

    if(argc<5)
    {
        goto err;
    }

    /*1. set rx module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;

    /*2. get rx meter index */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (QDMAMGR_GET_METER_IDX(cfg.index)>=(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)]))) 
    {
        printf("Error: input value is '%s', meter_grp %d: meter_id %dshould between 0 and %d\n", argv[1], QDMAMGR_GET_METER_GRP(cfg.index),QDMAMGR_GET_METER_IDX(cfg.index),(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)])-1);
        goto err;
    }

    /*3. get rx enable mode */
    if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) 
    {
        cfg.enable = 1;
    } 
    else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) 
    {
        cfg.enable = 0;
    } 
    else 
    {
        goto err;
    }
        
    /*4. get rx rate limit packet mode */
    if(strncasecmp(argv[3], "packet", strlen("packet")) == 0) 
    {
        cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
    } 
    else if(strncasecmp(argv[3], "byte", strlen("byte")) == 0) 
    {
        cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
    } 
    else 
    {
        goto err;
    }

    /*5. get rx rate limit tickSel index */
    if(strncasecmp(argv[4], "fast", strlen("fast")) == 0) 
    {
        cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
    } 
    else if(strncasecmp(argv[4], "slow", strlen("slow")) == 0) 
    {
        cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
    } 
    else 
    {
        goto err;
    }

    /* set rx rate limit config */
    if(qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: %s set general_rx_ratelimit config [MeterID] [Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_rx_ratelimit config [MeterID:0~%d][Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name, 128 - 1);
    return QDMAMGR_FAIL;

}

int do_general_set_rx_ratelimit_mode_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};
    unsigned char meterIdMax[3] = {128,32,16};

    if(argc<3)
    {
        goto err;
    }

    /* 1. set rx module */
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;

    /* 2. get rx meter idx */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (QDMAMGR_GET_METER_IDX(cfg.index)>=(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)]))) 
    {
        printf("Error: input value is '%s', meter_grp %d: meter_id %dshould between 0 and %d\n", argv[1], QDMAMGR_GET_METER_GRP(cfg.index),QDMAMGR_GET_METER_IDX(cfg.index),(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)])-1);
        goto err;
    }
    
    /* 3. get rx rate limit value */
    cfg.rate = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx rate limit value is '%s', which should be digital\n", argv[2]);
        goto err;
    }
    
    if(cfg.rate > (0xa<<20)) 
    {
        printf("Error: tx rate limit value is %d, which shold less than 10000000!", cfg.rate);
        goto err;
    }

    /* set rx rate limit value */
    if(qdmamgr_lib_set_ratectl_ratelimit_mode_value(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s set general_rx_ratelimit value [MeterID] [value]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n") ;
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_rx_ratelimit value [MeterID:0~%d] [1~10000000]\n", qdmamgr_name, 128 - 1) ;
    return QDMAMGR_FAIL;

}

int do_general_set_rx_ratelimit_bucket_size(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};
    unsigned char meterIdMax[3] = {128,32,16};

	if(argc<3)
		goto err;

	/* 1. set rx module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;

	/* 2. get rx meter idx */
	cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (QDMAMGR_GET_METER_IDX(cfg.index)>=(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)]))) 
    {
        printf("Error: input value is '%s', meter_grp %d: meter_id %dshould between 0 and %d\n", argv[1], QDMAMGR_GET_METER_GRP(cfg.index),QDMAMGR_GET_METER_IDX(cfg.index),(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)])-1);
        goto err;
    }

	/* 3. get rx bucket size value */
	cfg.bucket_size = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input rx bucket size value is '%s', which should be digital\n", argv[2]);
		goto err;
	}
	if(cfg.bucket_size > (0x100<<20)) {
		printf("Error: rx bucket size value is %d, which shold less than 256000000!", cfg.bucket_size);
		goto err;
	}

	/* set rx bucket size value */
	if(qdmamgr_lib_set_ratectl_ratelimit_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_rx_ratelimit bsize [MeterID] [value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_rx_ratelimit bsize [MeterID:0~%d] [1~256000000]\n", qdmamgr_name, 128-1) ;
	return QDMAMGR_FAIL;

}

const cmds_t general_set_rx_ratelimit_command[] = 
{
    {"config",  do_general_set_rx_ratelimit_mode_cfg,   0, 2, "Set QDMA general_rx_ratelimit config <MeterIdx, enMode, pktMode, tickSelIdx>"},
    {"value",   do_general_set_rx_ratelimit_mode_value, 0, 2, "Set QDMA general_rx_ratelimit value <MeterIdx, value>"},
    {"bsize",	do_general_set_rx_ratelimit_bucket_size,0, 2, "Set QDMA general_rx_ratelimit bsize <MeterIdx, value>"},
    {"help",    api_lib_help,                           0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,                                   0, 0, NULL}
};

int do_general_set_rx_ratelimit_mode(int argc, char *argv[], void *p)
{
    return subcmd(general_set_rx_ratelimit_command, argc, argv, (void *)general_set_rx_ratelimit_command);
}

int do_general_set_rx_trtcm_mode_cfg(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_TRTCM_MODE_CFG cfg = {0};
    unsigned char meterIdMax[3] = {128,32,16};

    if(argc<8)
    {
        goto err;
    }

    /*1. set rx trtcm module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;

    /*2. get rx trtcm index */
    cfg.index = strtoul(argv[1], &endPtr, 0);

    if((*endPtr!='\0') || (QDMAMGR_GET_METER_GRP(cfg.index) != 0)) 
    {
        printf("Error: input value is '%s', meter_grp %d not support trtcm mode\n", argv[1], QDMAMGR_GET_METER_GRP(cfg.index));
        goto err;
    }
        
    if((*endPtr!='\0') || (QDMAMGR_GET_METER_IDX(cfg.index)>=(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)]>>1))) 
    {
        printf("Error: input value is '%s', meter_grp %d: meter_id %d should between 0 and %d\n", argv[1], QDMAMGR_GET_METER_GRP(cfg.index),QDMAMGR_GET_METER_IDX(cfg.index),(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)]>>1)-1);
        goto err;
    }

    /*3. get rx trtcm cbs enable */
    if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) 
    {
        cfg.cbs_meter_en = 1;
    } 
    else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) 
    {
        cfg.cbs_meter_en = 0;
    } 
    else 
    {
        goto err;
    }
        
    /*4. get rx trtcm cbs packet mode */
    if(strncasecmp(argv[3], "packet", strlen("packet")) == 0) 
    {
        cfg.cbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET ;
    } 
    else if(strncasecmp(argv[3], "byte", strlen("byte")) == 0) 
    {
        cfg.cbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE ;
    } 
    else 
    {
        goto err;
    }

    /*5. get rx trtcm cbs tickSel index */
    if(strncasecmp(argv[4], "fast", strlen("fast")) == 0) 
    {
        cfg.cbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
    } 
    else if(strncasecmp(argv[4], "slow", strlen("slow")) == 0) 
    {
        cfg.cbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
    } 
    else 
    {
        goto err;
    }

    /*6. get rx trtcm pbs enable */
    if(strncasecmp(argv[5], "enable", strlen("enable")) == 0) 
    {
        cfg.pbs_meter_en = 1;
    } 
    else if(strncasecmp(argv[5], "disable", strlen("disable")) == 0) 
    {
        cfg.pbs_meter_en = 0;
    } 
    else 
    {
        goto err;
    }
        
    /*7. get rx trtcm pbs packet mode */
    if(strncasecmp(argv[6], "packet", strlen("packet")) == 0) 
    {
        cfg.pbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET ;
    } 
    else if(strncasecmp(argv[6], "byte", strlen("byte")) == 0) 
    {
        cfg.pbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE ;
    } 
    else 
    {
        goto err;
    }

    /*8. get rx trtcm pbs tickSel index */
    if(strncasecmp(argv[7], "fast", strlen("fast")) == 0) 
    {
        cfg.pbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
    } 
    else if(strncasecmp(argv[7], "slow", strlen("slow")) == 0) 
    {
        cfg.pbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
    } 
    else 
    {
        goto err;
    }

    /* set tx rate limit config */
    if(qdmamgr_lib_set_ratectl_trtcm_mode_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: %s set general_rx_trtcm config [MeterID] (cbs:)[Enable/Disable] [packet/byte] [tickSel:fast/slow] (pbs)[Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_rx_trtcm config [MeterID:0~%d] (cbs:)[Enable/Disable] [packet/byte] [tickSel:fast/slow] (pbs)[Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name, (128>>1) - 1);
    return QDMAMGR_FAIL;

}
int do_general_set_rx_trtcm_mode_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    ECNT_QDMAMGR_TRTCM_MODE_VALUE cfg = {0};
    unsigned char meterIdMax[3] = {128,32,16};

    if(argc<4)
    {
        goto err;
    }

    /* 1. set rx module */
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;

    /* 2. get rx meter idx */
    cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (QDMAMGR_GET_METER_GRP(cfg.index) != 0)) 
    {
        printf("Error: input value is '%s', meter_grp %d not support trtcm mode\n", argv[1], QDMAMGR_GET_METER_GRP(cfg.index));
        goto err;
    }
    
    if((*endPtr!='\0') || (QDMAMGR_GET_METER_IDX(cfg.index)>=(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)]>>1))) 
    {
        printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], (128>>1) - 1);
        goto err;
    }
    
    /* 3. get rx trtcm cbs value */
    cfg.cbs = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx CbsParamValue is '%s', which should be digital\n", argv[2]);
        goto err;
    }

    /* 4. get rx trtcm pbs value */
    cfg.pbs = strtoul(argv[3], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx PbsParamValue is '%s', which should be digital\n", argv[3]);
        goto err;
    }

    /* set rx rate limit value */
    if(qdmamgr_lib_set_ratectl_trtcm_mode_value(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s set general_rx_trtcm value [MeterID] [cbs_value] [pbs_value]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set general_rx_trtcm value [MeterID:0~%d] [1~10000000] [1~10000000]\n", qdmamgr_name, (128>>1)-1);
    return QDMAMGR_FAIL;

}

int do_general_set_rx_trtcm_bucket_size(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_TRTCM_MODE_VALUE cfg = {0};
    unsigned char meterIdMax[3] = {128,32,16};

	if(argc<4)
		goto err;

	/* 1. set rx module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;

	/* 2. get rx meter idx */
	cfg.index = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (QDMAMGR_GET_METER_GRP(cfg.index) != 0)) 
    {
        printf("Error: input value is '%s', meter_grp %d not support trtcm mode\n", argv[1], QDMAMGR_GET_METER_GRP(cfg.index));
        goto err;
    }
    
	if((*endPtr!='\0') || (QDMAMGR_GET_METER_IDX(cfg.index)>=(meterIdMax[QDMAMGR_GET_METER_GRP(cfg.index)]>>1))) {
		printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], (128>>1)-1);
		goto err;
	}
	
	/* 3. get rx trtcm cbs value */
	cfg.cbs_bucket_size = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input rx CbsBucketSize is '%s', which should be digital\n", argv[2]);
		goto err;
	}

	/* 4. get rx trtcm pbs value */
	cfg.pbs_bucket_size = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input rx PbsBucketSize is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/* set rx bucket sizet value */
	if(qdmamgr_lib_set_ratectl_trtcm_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_rx_trtcm bsize [MeterID] [cbs_value] [pbs_value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_rx_trtcm bsize [MeterID:0~%d] [1~256000000] [1~256000000]\n", qdmamgr_name, (128>>1)-1) ;
	return QDMAMGR_FAIL;

}

const cmds_t general_set_rx_trtcm_command[] = 
{
    {"config",  do_general_set_rx_trtcm_mode_cfg,       0, 2, "Set QDMA general_rx_trtcm config <MeterIdx, cbs_enMode, cbs_pktMode, cbs_tickSelIdx, pbs_enMode, pbs_pktMode, pbs_tickSelIdx>"},
    {"value",   do_general_set_rx_trtcm_mode_value,     0, 2, "Set QDMA general_rx_trtcm value <MeterIdx(bit[7:0] meter_id,bit[9:8] meter_group), cbs_value, pbs_value>"},
    {"bsize",	do_general_set_rx_trtcm_bucket_size,	0, 2, "Set QDMA general_rx_trtcm bsize <MeterIdx, cbs_value, pbs_value>"},
    {"help",    api_lib_help,                           0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,                                   0, 0, NULL}
};

int do_general_set_rx_trtcm_mode(int argc, char *argv[], void *p)
{
    return subcmd(general_set_rx_trtcm_command, argc, argv, (void *)general_set_rx_trtcm_command);
}

int do_set_multicast_en(int argc,char * argv [ ],void * p)
{
	u8 multicast_mode = 0;

	if(argc<2)
		goto err;

	/* 1. get channel index */
	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) {
		multicast_mode = 1;
	} else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) {
		multicast_mode = 0;
	} else {
		goto err;
	}

	if(qdmamgr_lib_set_multicast_en(g_qdma_sel, multicast_mode)) {
		printf("Exec. Failed: '%s set QDMA multicast <enable/disable>' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s Set QDMA multicast <enable/disable>\n", qdmamgr_name) ;
	return QDMAMGR_FAIL;
}

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
int do_general_set_sla_trtcm_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

	if(argc < 5)
	{
		goto err;
	}

	/*1. set sla module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/*2. get sla module enable */
	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
	{
		cfg.enable = 1;
	}
	else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0)
	{
		cfg.enable = 0;
	}
	else
	{
		goto err;
	}
		
	/*3. get sla module working mode */
	if(strncasecmp(argv[2], "ratelimit", strlen("ratelimit")) == 0)
	{
		cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT;
	}
	else if(strncasecmp(argv[2], "trtcm", strlen("trtcm")) == 0)
	{
		cfg.mode = ECNT_QDMAMGR_TRTCM_MODE_TRTCM;
	}
	else
	{
		goto err;
	}

	/*4. get slow tick ratio */
	cfg.slow_tick_ratio = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') 
	{
		printf("Error: input slowtick ratio is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/*5. get fast tick */
	cfg.fast_tick = strtoul(argv[4], &endPtr, 0);
	if(*endPtr!='\0') 
	{
		printf("Error: input fasttick is '%s', which should be digital\n", argv[4]);
		goto err;
	}

	/* set sla ratelimit/trtcm module init */
	if (qdmamgr_lib_set_ratectl_cfg(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: %s set general_sla_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n");
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_sla_init [enable/disable] [ratelimit/trtcm] [slowtickratio] [fasttick(unit:us)]\n", qdmamgr_name);
	return QDMAMGR_FAIL;
}

int do_general_set_sla_ratelimit_mode_cfg(int argc, char *argv[], void *p)
{
	unsigned char Sindex = 0, Qindex = 0;
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_CFG cfg = {0};

	if(argc<6)
		goto err;

	/*1. set sla module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/*2. get sla index */
	Sindex = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (Sindex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
		printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
		goto err;
	}

	/*3. get sla queue index */
	Qindex = strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (Qindex>=CONFIG_QDMA_QUEUE)) {
		printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
		goto err;
	}

	cfg.index = ((Sindex<<3) | Qindex);

	/*4. get sla enable mode */
	if(strncasecmp(argv[3], "enable", strlen("enable")) == 0) {
		cfg.enable = 1;
	} else if(strncasecmp(argv[3], "disable", strlen("disable")) == 0) {
		cfg.enable = 0;
	} else {
		goto err;
	}
		
	/*5. get sla rate limit packet mode */
	if(strncasecmp(argv[4], "packet", strlen("packet")) == 0) {
		cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
	} else if(strncasecmp(argv[4], "byte", strlen("byte")) == 0) {
		cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
	} else {
		goto err;
	}

	/*6. get sla rate limit tickSel index */
	if(strncasecmp(argv[5], "fast", strlen("fast")) == 0) {
		cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
	} else if(strncasecmp(argv[5], "slow", strlen("slow")) == 0) {
		cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
	} else {
		goto err;
	}

	/* set sla rate limit config */
	if(qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: %s set general_sla_ratelimit config [SLAID] [QueueID] [Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_sla_ratelimit config [SLAID:0~%d] [QueueID:0~%d] [Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", 
						qdmamgr_name, ECNT_QDMAMGR_SLA_MAX_NUM-1, CONFIG_QDMA_QUEUE-1) ;
	return QDMAMGR_FAIL;

}

int do_general_set_sla_ratelimit_mode_value(int argc, char *argv[], void *p)
{
	unsigned char Sindex = 0, Qindex = 0;
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

	if(argc<4)
		goto err;

	/* 1. set sla module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/* 2. get sla idx */
	Sindex = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (Sindex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
		printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
		goto err;
	}

	/* 3. get sla queue idx */
	Qindex= strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (Qindex>=CONFIG_QDMA_QUEUE)) {
		printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
		goto err;
	}

	cfg.index = ((Sindex<<3) | Qindex);
	
	/* 4. get sla rate limit value */
	cfg.rate = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: sla rate limit value is '%s', which should be digital\n", argv[3]);
		goto err;
	}
	if(cfg.rate > (0xa<<20)) {
		printf("Error: sla rate limit value is %d, which shold less than 10000000!", cfg.rate);
		goto err;
	}

	/* set sla rate limit value */
	if (qdmamgr_lib_set_ratectl_ratelimit_mode_value(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_sla_ratelimit value [SLAID] [QueueID] [value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_sla_ratelimit value [SLAID:0~%d] [QueueID:0~%d] [1~10000000]\n", 
						qdmamgr_name, ECNT_QDMAMGR_SLA_MAX_NUM-1, CONFIG_QDMA_QUEUE-1) ;
	return QDMAMGR_FAIL;

}

int do_general_set_sla_ratelimit_bucket_size(int argc, char *argv[], void *p)
{
	unsigned char Sindex = 0, Qindex = 0;
	char *endPtr = NULL;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE cfg = {0};

	if(argc<4)
		goto err;

	/* 1. set sla module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/* 2. get sla idx */
	Sindex = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (Sindex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
		printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
		goto err;
	}

	/* 3. get sla queue idx */
	Qindex= strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (Qindex>=CONFIG_QDMA_QUEUE)) {
		printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
		goto err;
	}

	cfg.index = ((Sindex<<3) | Qindex);
	
	/* 4. get sla bucket size value */
	cfg.bucket_size = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: sla bucket size value is '%s', which should be digital\n", argv[3]);
		goto err;
	}
	if(cfg.bucket_size > (0x400<<17)) {
		printf("Error: sla bucket size value is %d, which shold less than 128000000!", cfg.bucket_size);
		goto err;
	}

	/* set sla rate limit value */
	if(qdmamgr_lib_set_ratectl_ratelimit_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_sla_ratelimit bsize [SLAID] [QueueID] [value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_sla_ratelimit bsize [SLAID:0~%d] [QueueID:0~%d] [1~128000000]\n", 
						qdmamgr_name, ECNT_QDMAMGR_SLA_MAX_NUM-1, CONFIG_QDMA_QUEUE-1) ;
	return QDMAMGR_FAIL;

}

const cmds_t general_set_sla_ratelimit_command[] = 
{
	{"config",	do_general_set_sla_ratelimit_mode_cfg,		0,	2,	"Set QDMA general_rx_ratelimit config <SLAIdx, QueueIdx, enMode, pktMode, tickSelIdx>"},
	{"value",	do_general_set_sla_ratelimit_mode_value,	0,	2,	"Set QDMA general_rx_ratelimit value <SLAIdx, QueueIdx, value>"},
	{"bsize",	do_general_set_sla_ratelimit_bucket_size,	0,	2,	"Set QDMA general_rx_ratelimit bsize <SLAIdx, QueueIdx, value>"},
	{"help",	api_lib_help,								0,	0,	"Show qdmamgr set usage."},
	{NULL, NULL, 0, 0, NULL}
};

int do_general_set_sla_ratelimit_mode(int argc, char *argv[], void *p)
{
	return subcmd(general_set_sla_ratelimit_command, argc, argv, (void *)general_set_sla_ratelimit_command);
}

int do_general_set_sla_trtcm_mode_cfg(int argc, char *argv[], void *p)
{
	unsigned char Sindex = 0, Qindex = 0;
	char *endPtr = NULL;
	ECNT_QDMAMGR_TRTCM_MODE_CFG cfg = {0};

	if(argc<9)
		goto err;

	/*1. set sla trtcm module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/*2. get sla trtcm index */
	Sindex = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (Sindex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
		printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
		goto err;
	}

	/*3. get sla trtcm queue index */
	Qindex = strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (Qindex>=CONFIG_QDMA_QUEUE)) {
		printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
		goto err;
	}

	cfg.index = ((Sindex<<3) | Qindex);

	/*4. get sla trtcm cbs enable */
	if(strncasecmp(argv[3], "enable", strlen("enable")) == 0) {
		cfg.cbs_meter_en = 1;
	} else if(strncasecmp(argv[3], "disable", strlen("disable")) == 0) {
		cfg.cbs_meter_en = 0;
	} else {
		goto err;
	}
		
	/*5. get sla trtcm cbs packet mode */
	if(strncasecmp(argv[4], "packet", strlen("packet")) == 0) {
		cfg.cbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
	} else if(strncasecmp(argv[4], "byte", strlen("byte")) == 0) {
		cfg.cbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
	} else {
		goto err;
	}

	/*6. get sla trtcm cbs tickSel index */
	if(strncasecmp(argv[5], "fast", strlen("fast")) == 0) {
		cfg.cbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
	} else if(strncasecmp(argv[5], "slow", strlen("slow")) == 0) {
		cfg.cbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
	} else {
		goto err;
	}

	/*7. get sla trtcm pbs enable */
	if(strncasecmp(argv[6], "enable", strlen("enable")) == 0) {
		cfg.pbs_meter_en = 1;
	} else if(strncasecmp(argv[6], "disable", strlen("disable")) == 0) {
		cfg.pbs_meter_en = 0;
	} else {
		goto err;
	}
		
	/*8. get sla trtcm pbs packet mode */
	if(strncasecmp(argv[7], "packet", strlen("packet")) == 0) {
		cfg.pbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
	} else if(strncasecmp(argv[7], "byte", strlen("byte")) == 0) {
		cfg.pbs_rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
	} else {
		goto err;
	}

	/*9. get sla trtcm pbs tickSel index */
	if(strncasecmp(argv[8], "fast", strlen("fast")) == 0) {
		cfg.pbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
	} else if(strncasecmp(argv[8], "slow", strlen("slow")) == 0) {
		cfg.pbs_tick_type = ECNT_QDMAMGR_TICK_TYPE_SLOW;
	} else {
		goto err;
	}

	/* set sla rate limit config */
	if(qdmamgr_lib_set_ratectl_trtcm_mode_cfg(g_qdma_sel, &cfg))  {
		printf("Exec. Failed: %s set general_sla_trtcm config [SLAID] [QueueID] (cbs:)[Enable/Disable] [packet/byte] [tickSel:fast/slow] (pbs)[Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_sla_trtcm config [SLAID:0~%d] [QueueID:0~%d] (cbs:)[Enable/Disable] [packet/byte] [tickSel:fast/slow] (pbs)[Enable/Disable] [packet/byte] [tickSel:fast/slow]\n", 
						qdmamgr_name, ECNT_QDMAMGR_SLA_MAX_NUM-1, CONFIG_QDMA_QUEUE-1) ;
	return QDMAMGR_FAIL;

}

int do_general_set_sla_trtcm_mode_value(int argc, char *argv[], void *p)
{
	unsigned char Sindex = 0, Qindex = 0;
	char *endPtr = NULL;
	ECNT_QDMAMGR_TRTCM_MODE_VALUE cfg = {0};

	if(argc<5)
		goto err;

	/* 1. set sla module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/* 2. get sla idx */
	Sindex = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (Sindex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
		printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
		goto err;
	}

	/* 3. get sla queue idx */
	Qindex = strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (Qindex>=CONFIG_QDMA_QUEUE)) {
		printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
		goto err;
	}

	cfg.index = ((Sindex<<3) | Qindex);
	
	/* 4. get sla trtcm cbs value */
	cfg.cbs = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input sla CbsParamValue is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/* 5. get sla trtcm pbs value */
	cfg.pbs = strtoul(argv[4], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input sla PbsParamValue is '%s', which should be digital\n", argv[4]);
		goto err;
	}

	/* set sla rate limit value */
	if(qdmamgr_lib_set_ratectl_trtcm_mode_value(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_sla_trtcm value [SLAID] [QueueID] [cbs_value] [pbs_value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_sla_trtcm value [SLAID:0~%d] [QueueID:0~%d] [1~10000000] [1~10000000]\n", 
					qdmamgr_name, ECNT_QDMAMGR_SLA_MAX_NUM-1, CONFIG_QDMA_QUEUE-1) ;
	return QDMAMGR_FAIL;

}

int do_general_set_sla_trtcm_bucket_size(int argc, char *argv[], void *p)
{
	unsigned char Sindex = 0, Qindex = 0;
	char *endPtr = NULL;
	ECNT_QDMAMGR_TRTCM_MODE_VALUE cfg = {0};

	if(argc<5)
		goto err;

	/* 1. set sla module */
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/* 2. get sla idx */
	Sindex = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (Sindex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
		printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
		goto err;
	}

	/* 3. get sla queue idx */
	Qindex = strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (Qindex>=CONFIG_QDMA_QUEUE)) {
		printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
		goto err;
	}

	cfg.index = ((Sindex<<3) | Qindex);
	
	/* 4. get sla trtcm cbs value */
	cfg.cbs_bucket_size = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input sla CbsBucketSize is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/* 5. get sla trtcm pbs value */
	cfg.pbs_bucket_size = strtoul(argv[4], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input sla PbsBucketSize is '%s', which should be digital\n", argv[4]);
		goto err;
	}

	/* set sla bucket size value */
	if(qdmamgr_lib_set_ratectl_trtcm_bucket_size(g_qdma_sel, &cfg)) {
		printf("Exec. Failed: '%s set general_sla_trtcm bsize [SLAID] [QueueID] [cbs_value] [pbs_value]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set general_sla_trtcm bsize [SLAID:0~%d] [QueueID:0~%d] [1~128000000] [1~128000000]\n", 
					qdmamgr_name, ECNT_QDMAMGR_SLA_MAX_NUM-1, CONFIG_QDMA_QUEUE-1) ;
	return QDMAMGR_FAIL;

}

const cmds_t general_set_sla_trtcm_command[] = 
{
	{"config",	do_general_set_sla_trtcm_mode_cfg,		0,	2,	"Set QDMA general_rx_trtcm config <SLAIdx, QueueIdx, cbs_enMode, cbs_pktMode, cbs_tickSelIdx, pbs_enMode, pbs_pktMode, pbs_tickSelIdx>"},
	{"value",	do_general_set_sla_trtcm_mode_value,	0,	2,	"Set QDMA general_rx_trtcm value <SLAIdx, QueueIdx, cbs_value, pbs_value>"},
	{"bsize",	do_general_set_sla_trtcm_bucket_size,	0,	2,	"Set QDMA general_rx_trtcm bsize <SLAIdx, QueueIdx, cbs_value, pbs_value>"},
	{"help",	api_lib_help,							0,	0,	"Show qdmamgr set usage."},
	{NULL, NULL, 0, 0, NULL}
};

int do_general_set_sla_trtcm_mode(int argc, char *argv[], void *p)
{
	return subcmd(general_set_sla_trtcm_command, argc, argv, (void *)general_set_sla_trtcm_command);
}

int do_set_sla_channel_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	unsigned char SLAindex = 0, CHNLindex = 0;
	unsigned char SLAenable = 0;

	if(argc<4)
		goto err;

	/* 1. get sla index */
	SLAindex = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (SLAindex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
		printf("Error: input SLA index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
		goto err;
	}

	/* 2. get sla enable */
	if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) {
		SLAenable = 1;
	} else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) {
		SLAenable = 0;
	} else {
		goto err;
	}

	/* 3. get sla channel idx */
	CHNLindex = strtoul(argv[3], &endPtr, 0);
	if((*endPtr!='\0') || (CHNLindex>=CONFIG_QDMA_CHANNEL)) {
		printf("Error: input channel is '%s', which should between 0 and %d\n", argv[3], CONFIG_QDMA_CHANNEL-1);
		goto err;
	}

	/* set sla rate limit value */
	if(qdmamgr_lib_set_sla_channel_cfg(g_qdma_sel, SLAindex, SLAenable, CHNLindex)) {
		printf("Exec. Failed: '%s set sla_cfg [SLAID] [enable/disable] [ChannelID]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set sla_cfg [SLAID:0~%d] [enable/disable] [ChannelID:0~%d]\n", qdmamgr_name, ECNT_QDMAMGR_SLA_MAX_NUM-1, CONFIG_QDMA_CHANNEL-1) ;
	return QDMAMGR_FAIL;

}

int do_set_aging_global_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_QOS_AGING_CFG agingCfg = {0};

	if(argc<5)
		goto err;

	/* 1. get aging global setting enable/disable */
	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) {
		agingCfg.agingEn = ECNT_QDMAMGR_ENABLE;
	} else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) {
		agingCfg.agingEn = ECNT_QDMAMGR_DISABLE;
	} else {
		goto err;
	}

	/* 2. get aging method setting threshold/always */
	if(strncasecmp(argv[2], "threshold", strlen("threshold")) == 0) {
		agingCfg.agingMethod = ECNT_QDMAMGR_QOS_AGING_THRESHOLD;
	} else if(strncasecmp(argv[2], "always", strlen("always")) == 0) {
		agingCfg.agingMethod = ECNT_QDMAMGR_QOS_AGING_ALWAYS;
	} else {
		goto err;
	}

	/* 3. get aging fast replace setting enable/disable */
	if(strncasecmp(argv[3], "enable", strlen("enable")) == 0) {
		agingCfg.agingFastReplaceEn = ECNT_QDMAMGR_ENABLE;
	} else if(strncasecmp(argv[3], "disable", strlen("disable")) == 0) {
		agingCfg.agingFastReplaceEn = ECNT_QDMAMGR_DISABLE;
	} else {
		goto err;
	}

	/* 4. get aging time */
	agingCfg.agingTime = strtoul(argv[4], &endPtr, 0);
	if((*endPtr!='\0') || (agingCfg.agingTime > 1310700)) {
		printf("Error: input aging time is '%s', which should between 0 and %d\n", argv[4], 1310700);
		goto err;
	}

	/* set qos aging cfg */
	if(qdmamgr_lib_set_qos_aging_cfg(g_qdma_sel, &agingCfg)) {
		printf("Exec. Failed: '%s set aging_cfg global [global_en] [aging_method] [fast_replace_en] [aging_time]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set aging_cfg global [global_en] [aging_method<threshold/always>] [fast_replace_en] [aging_time]\n", qdmamgr_name) ;
	return QDMAMGR_FAIL;

}

int do_set_aging_queue_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	unsigned char chnlIdx = 0, queueIdx = 0, enable = 0;

	if(argc<4)
		goto err;

	/* 1. get channel index */
	chnlIdx = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (chnlIdx>=CONFIG_QDMA_CHANNEL)) {
		printf("Error: input SLA index is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL-1);
		goto err;
	}

	/* 2. get queuel idx */
	queueIdx = strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (queueIdx>=CONFIG_QDMA_QUEUE)) {
		printf("Error: input channel is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
		goto err;
	}

	/* 3. get per-queue aging enable */
	if(strncasecmp(argv[3], "enable", strlen("enable")) == 0) {
		enable = ECNT_QDMAMGR_ENABLE;
	} else if(strncasecmp(argv[3], "disable", strlen("disable")) == 0) {
		enable = ECNT_QDMAMGR_DISABLE;
	} else {
		goto err;
	}

	/* set per-queue aging cfg */
	if(qdmamgr_lib_set_per_queue_aging_cfg(g_qdma_sel, chnlIdx, queueIdx, enable)) {
		printf("Exec. Failed: '%s set aging_cfg queue [ChannelID] [QueueID] [enable/disable]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set aging_cfg queue [ChannelID:0~%d] [QueueID:0~%d] [enable/disable]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL-1, CONFIG_QDMA_QUEUE-1) ;
	return QDMAMGR_FAIL;

}

const cmds_t set_aging_command[] = 
{
	{"global",	do_set_aging_global_cfg,	0,	2,	"Set QDMA aging global <aging enable, max thres/always, fast replace enable, aging time>"},
	{"queue",	do_set_aging_queue_cfg,		0,	2,	"Set QDMA aging queue <channel_idx, queue_idx, enable/disable>"},
	{"help",	api_lib_help,				0,	0,	"Show qdmamgr set usage."},
	{NULL, NULL, 0, 0, NULL}
};

int do_set_aging_cfg(int argc, char *argv[], void *p)
{
	return subcmd(set_aging_command, argc, argv, (void *)set_aging_command);
}

int do_set_multicast_sptag_cfg(int argc,char * argv [ ],void * p)
{
	char *endPtr = NULL;
	unsigned char chnlIdx = 0;
	unsigned char keepHi = 0;
	unsigned int sp_tag = 0;

	if(argc<4)
		goto err;

	/* 1. get channel index */
	chnlIdx = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (chnlIdx>=CONFIG_QDMA_CHANNEL)) {
		printf("Error: input channel index is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL-1);
		goto err;
	}

	/* 2. get sp_tag keep hi-field enable */
	if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) {
		keepHi = ECNT_QDMAMGR_ENABLE;
	} else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) {
		keepHi = ECNT_QDMAMGR_DISABLE;
	} else {
		goto err;
	}

	/* 3. get sp_tag info */
	sp_tag = strtoul(argv[3], &endPtr, 16);
	if((*endPtr!='\0') || (sp_tag>0xffff)) {
		printf("Error: input sp_tag is '%s', which should between 0 and 0x%X\n", argv[2], 0xffff);
		goto err;
	}

	if(qdmamgr_lib_set_multicast_sptag_cfg(g_qdma_sel, chnlIdx, keepHi, (sp_tag & 0xffff))) {
		printf("Exec. Failed: '%s set mul_sptag_cfg [ChannelID] [Sptag keep hi-field enable] [Sptag]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set mul_sptag_cfg [ChannelID:0~%d] [enable/disable:keep hi-field] [Sptag:0~0xffff]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL-1) ;
	return QDMAMGR_FAIL;
}

int do_set_multicast_fport_cfg(int argc,char * argv [ ],void * p)
{
	char *endPtr = NULL;
	unsigned char chnlIdx = 0, portIdx = 0, nbqIdx = 0;

	if(argc<4)
		goto err;

	/* 1. get channel index */
	chnlIdx = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (chnlIdx>=16)) {
		printf("Error: input channel index is '%s', which should between 0 and %d\n", argv[1], 16-1);
		goto err;
	}

	/* 2. get port idx */
	portIdx = strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (portIdx>=16)) {
		printf("Error: input pse port is '%s', which should between 0 and %d\n", argv[2], 16-1);
		goto err;
	}

	/* 3. get nbq index */
	nbqIdx = strtoul(argv[3], &endPtr, 0);
	if((*endPtr!='\0') || (nbqIdx>=32)) {
		printf("Error: input nbq is '%s', which should between 0 and %d\n", argv[2], 32-1);
		goto err;
	}

	if(qdmamgr_lib_set_multicast_fport_cfg(g_qdma_sel, chnlIdx, ((portIdx << 5) | nbqIdx))) {
		printf("Exec. Failed: '%s set mul_fport_cfg [ChannelID] [PortID] [NbqID]' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set mul_fport_cfg [ChannelID:0~%d] [PortID:0~15] [NbqID:0~31]\n", qdmamgr_name, 16-1) ;
	return QDMAMGR_FAIL;
}
#endif

int do_set_flowcnt_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	unsigned char grpIdx = 0;
	unsigned char incDropEn = 0;

	if(argc<3)
		goto err;

	/*1. get cnt group index */
	grpIdx = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (grpIdx>(ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1))) {
		printf("Error: input group index is '%s', which should between 0 and %d\n", argv[1], (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1));
		goto err;
	}

	/*2. get inc drop enable set */
	if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) {
		incDropEn = ECNT_QDMAMGR_ENABLE;
	} else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) {
		incDropEn = ECNT_QDMAMGR_DISABLE;
	} else {
		goto err;
	}

	/*3.  set flowcnt config */
	if(qdmamgr_lib_set_flow_cntr_cfg(g_qdma_sel, grpIdx, incDropEn)) {
		printf("Exec. Failed: %s set flowcnt config [grp_idx](0~%d)[inc_drop_enable:enable/disable]\n", 
			qdmamgr_name, (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1)) ;
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set flowcnt config [grp_idx](0~1)[inc_drop_enable:enable/disable]\n", qdmamgr_name) ;
	return QDMAMGR_FAIL;

}

int do_set_flowcnt_value(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	unsigned char grpIdx = 0, cntIdx = 0;
	ECNT_QDMAMGR_FLOW_CNT_CLEAR_TYPE clearType = ECNT_QDMAMGR_FLOWCNT_CLEAR_ALL;

	if(argc<5)
		goto err;

	/*1. get cnt group index */
	grpIdx = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (grpIdx>(ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1))) {
		printf("Error: input group index is '%s', which should between 0 and %d\n", argv[1], (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1));
		goto err;
	}

	/*2. get cnt index */
	cntIdx = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') {
		goto err;
	}
	if( grpIdx == 0 ){
		if(cntIdx>ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM) {
    			printf("Error: input cnt index is '%s', which should between 0 and %d\n", argv[2], ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM);
    			goto err;
		}
	}else if( grpIdx == 1 ){
		if(cntIdx>ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM) {
    			printf("Error: input cnt index is '%s', which should between 0 and %d\n", argv[2], ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM);
    			goto err;
		}
	}
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	else if( grpIdx == 2 ){
		if(cntIdx>ECNT_QDMAMGR_FLOWCNT_GRP2_MAX_IDX_NUM) {
    			printf("Error: input cnt index is '%s', which should between 0 and %d\n", argv[2], ECNT_QDMAMGR_FLOWCNT_GRP2_MAX_IDX_NUM);
    			goto err;
		}
	}
#endif

	/*3. get operate mode*/
	if(strncasecmp(argv[3], "clear", strlen("clear")) != 0) {
		goto err;
	}

	/*4.  set clear mode */
	if(strncasecmp(argv[4], "byte", strlen("byte")) == 0) {
		clearType = ECNT_QDMAMGR_FLOWCNT_CLEAR_BYTE;
	}else if(strncasecmp(argv[4], "packet", strlen("packet")) == 0) {
		clearType = ECNT_QDMAMGR_FLOWCNT_CLEAR_PKT;
	}else if(strncasecmp(argv[4], "all", strlen("all")) == 0) {
		clearType = ECNT_QDMAMGR_FLOWCNT_CLEAR_ALL;
	}else{
		goto err;
	}

	/*5.  set the value clear */
	if(qdmamgr_lib_clear_flow_cntr_value(g_qdma_sel, grpIdx, cntIdx, clearType)) {
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		printf("Exec. Failed: %s set flowcnt value [grp_idx](0~%d) [cnt_idx](grp0:0~%d grp1:0~%d grp2:0~%d) clear clear_mode(byte/packet/all)\n", 
			qdmamgr_name, (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1), 
			(ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM-1), (ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM-1), (ECNT_QDMAMGR_FLOWCNT_GRP2_MAX_IDX_NUM-1)) ;
#else
		printf("Exec. Failed: %s set flowcnt value [grp_idx](0~%d) [cnt_idx](grp0:0~%d grp1:0~%d) clear clear_mode(byte/packet/all)\n", 
            qdmamgr_name, (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1), 
            (ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM-1), (ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM-1)) ;
#endif
		return QDMAMGR_FAIL;
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	printf("Format: %s set flowcnt value [grp_idx](0~%d) [cnt_idx](grp0:0~%d grp1:0~%d grp2:0~%d) clear clear_mode(byte/packet/all)\n", 
		qdmamgr_name, (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1), 
		(ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM-1), (ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM-1), (ECNT_QDMAMGR_FLOWCNT_GRP2_MAX_IDX_NUM-1)) ;
#else

	printf("Format: %s set flowcnt value [grp_idx](0~%d) [cnt_idx](grp0:0~%d grp1:0~%d) clear clear_mode(byte/packet/all)\n", 
            qdmamgr_name, (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1), 
            (ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM-1) ,(ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM-1)) ;
#endif
	return QDMAMGR_FAIL;

}

const cmds_t set_flowcnt_command[] = 
{
	{"config",	do_set_flowcnt_cfg,		0,	2,	"Set QDMA flowcnt config <grp_idx, inc_drop_enable>"},
	{"value",	do_set_flowcnt_value,	0,	2,	"Set QDMA flowcnt value <grp_idx, cnt_idx, clear, clear_mode>"},
	{"help",	api_lib_help,			0,	0,	"Show qdmamgr set usage."},
	{NULL, NULL, 0, 0, NULL}
};

int do_set_flowcnt(int argc, char *argv[], void *p)
{
	return subcmd(set_flowcnt_command, argc, argv, (void *)set_flowcnt_command);
}

int do_set_wred_mode(int argc, char *argv[], void *p)
{
    u8 enable;

    if(argc<2)
        goto err;
           
    if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) {
        enable = ECNT_QDMAMGR_ENABLE;
    }else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0){
        enable = ECNT_QDMAMGR_DISABLE;
    }else{
        goto err;
    }        

    if(qdmamgr_lib_set_tx_wred_mode(g_qdma_sel, enable)) {        
        printf("Exec. Failed: '%s set txwred mode [enable/disable]' command failed\\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n") ;
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set txwred mode [enable/disable]\n", qdmamgr_name) ;
    return QDMAMGR_FAIL;

}

int do_set_wred_thrval(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_TX_WRED_CFG txWred = {0};
	uint idx=0;

	if(argc<7)
		goto err;
    
	if(strncasecmp(argv[1], "normal", strlen("normal")) == 0) {
		txWred.txWredPacketType = ECNT_QDMAMGR_NORMAL_PACKET;
	}else if(strncasecmp(argv[1], "dei", strlen("dei")) == 0){
		txWred.txWredPacketType = ECNT_QDMAMGR_DEI_PACKET;
	}else{
		goto err;
	}
    
	for(idx=0; idx<ECNT_QDMAMGR_TX_WRED_THR_NUM; idx++) {
		txWred.txWredThrValue[idx]= (unchar)strtoul(argv[2+idx], &endPtr, 0);
		if((*endPtr!='\0') || (txWred.txWredThrValue[idx] > 56)) {
			printf("Fault: Tx Wred threshold range: 0 ~ 56 !\n");
			goto err;
		}
	}
	if( qdmamgr_lib_set_tx_wred_threshold(g_qdma_sel, &txWred)) {        
		printf("Exec. Failed: '%s set txwred threshold [normal/dei] [thr0][thr1][thr2][thr3][thr4](thr:0~8(unit:dynThr/8))' command failed\\n", 
			qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
    
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set txwred threshold [normal/dei] [thr0][thr1][thr2][thr3][thr4](thr:0~8(unit:dynThr/8))\n", qdmamgr_name) ;
	return QDMAMGR_FAIL;

}

int do_set_wred_probability(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_TX_WRED_CFG txWred = {0};
	uint idx=0;

	if(argc<6)
		goto err;
    
	if(strncasecmp(argv[1], "normal", strlen("normal")) == 0) {
		txWred.txWredPacketType = ECNT_QDMAMGR_NORMAL_PACKET;
	}else if(strncasecmp(argv[1], "dei", strlen("dei")) == 0){
		txWred.txWredPacketType = ECNT_QDMAMGR_DEI_PACKET;
	}else{
		goto err;
	}

	for(idx=0; idx<ECNT_QDMAMGR_TX_WRED_PROBABILITY_NUM; idx++) {
		txWred.txWredThrProbability[idx]= (unchar)strtoul(argv[2+idx], &endPtr, 0);
		if((*endPtr!='\0') || (txWred.txWredThrProbability[idx] > 100)) {
			printf("Fault: Tx Wred probability range: 0 ~ 100 [%%] !\n");
			goto err;
		}
	}

	if(qdmamgr_lib_set_tx_wred_probability(g_qdma_sel, &txWred)) {        
		printf("Exec. Failed: '%s set txwred probability [normal/dei] [p0][p1][p2][p3](thr:0~100(unit:%%))' command failed\\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}

	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;
    
err:
	printf("Format: %s set txwred probability [normal/dei] [p0][p1][p2][p3](thr:0~100(unit:%%))\n", qdmamgr_name) ;
	return QDMAMGR_FAIL;

}

const cmds_t set_wred_command[] = 
{
	{"mode",			do_set_wred_mode,			0,	0,	"Set QDMA wred mode <enable/disable>"},
	{"threshold",		do_set_wred_thrval,			0,	0,	"Set QDMA wred threshold <normal/dei, thr0, thr1, thr2, thr3 thr4>"},
	{"probability",		do_set_wred_probability,	0,	0,	"Set QDMA wred probability <normal/dei, p0, p1, p2, p3>"},
	{"help",			api_lib_help,				0,	0,	"Show qdmamgr set usage."},
	{NULL, NULL, 0, 0, NULL}
};

int do_set_tx_wred_cfg(int argc, char *argv[], void *p)
{
	return subcmd(set_wred_command, argc, argv, (void *)set_wred_command);
}

int do_set_red_probability(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_QDMAMGR_WRED_PACKET_Type type = 0;
	unchar probability = 0;
    
	if(argc<3)
		goto err;    

	if(strncasecmp(argv[1], "normal", strlen("normal")) == 0) {
		type = ECNT_QDMAMGR_NORMAL_PACKET;
	}else if(strncasecmp(argv[1], "dei", strlen("dei")) == 0){
		type = ECNT_QDMAMGR_DEI_PACKET;
	}else{
		goto err;
	}

	probability = (unchar)strtoul(argv[2], &endPtr, 0);
	if((*endPtr!='\0') || (probability > 100)) {
		printf("Fault: Rx Red probability range: 0 ~ 100 [%%] !\n");
		goto err;
	}
	if(qdmamgr_lib_set_rx_red_probability(g_qdma_sel, type, probability)) {        
		printf("Exec. Failed: '%s set rxred probability [normal/dei] [P](thr:0~100(unit:%%))' command failed\\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}

	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s set rxred probability [normal/dei] [P](thr:0~100(unit:%%))\n", qdmamgr_name) ;
	return QDMAMGR_FAIL;        
}

const cmds_t set_red_command[] = 
{
	{"probability",	do_set_red_probability,	0,	0,	"qdmamgr_lan/wan set rxred probability <normal/dei> <ProbabilityValue[0-100]>"},
	{"help",		api_lib_help,			0,	0,	"Show qdmamgr set usage."},
	{NULL, NULL, 0, 0, NULL}
};

int do_set_rx_red_cfg(int argc, char *argv[], void *p)
{
     return subcmd(set_red_command, argc, argv, (void *)set_red_command);
}
#endif

int do_set_txratelimit_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 channel, enable;
    u32 rate;
        
    if(argc<4)
    {
        goto err;
    }

    /* get channel idx */
    channel = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (channel >= CONFIG_QDMA_CHANNEL)) 
    {
        printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
        goto err;
    }
    
    /* get tx rate limit enable/disable */  
    if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) 
    {
        enable = 1;
    } 
    else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) 
    {
        enable = 0;
    } 
    else 
    {
        goto err;
    }
    
    /* get tx rate limit value in kbps */
    rate = strtoul(argv[3], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input tx rate limit value is '%s', which should be digital\n", argv[3]);
        goto err;
    }

#if defined(TCSUPPORT_CHANNEL_10G)
    if(rate > (10<<20)) {
        printf("Error: tx rate limit value is %dkbps, which shold less than 10gbps!", rate);
        goto err;
    }
#else
    if(rate > (4<<20)) {
        printf("Error: tx rate limit value is %dkbps, which shold less than 4gbps!", rate);
        goto err;
    }
#endif /* defined(TCSUPPORT_CHANNEL_10G) */

    /* set tx rate limit value */
    if(qdmamgr_lib_set_tx_ratelimit_value(g_qdma_sel, channel, enable, rate)) 
    {
        printf("Exec. Failed: '%s set txratelimit [ChnlID] [Enable/Disable] [pir(unit:kbps)]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set txratelimit [ChnlID:0~%d] [Enable/Disable] [1~1000000(unit:kbps)]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
    return QDMAMGR_FAIL;
}

int do_set_rxratelimit_config(int argc, char *argv[], void *p)
{
    u8 enable;
    ECNT_QDMAMGR_RATECTL_MODE mode;
           
    if(argc<3)
    {
        goto err;
    }

    /* get rx rate limit enable mode */
    if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
    {
        enable = 1;
    } 
    else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) 
    {
        enable = 0;
    } 
    else 
    {
        goto err;
    }
        
    /* get rx rate limit packet mode */
    if(strncasecmp(argv[2], "packet", strlen("packet")) == 0) 
    {
        mode = ECNT_QDMAMGR_RATECTL_MODE_PACKET;
    } 
    else if(strncasecmp(argv[2], "byte", strlen("byte")) == 0) 
    {
        mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
    } 
    else 
    {
        goto err;
    }

    /* set rx rate limit config */
    if(qdmamgr_lib_set_rx_ratelimit_en(g_qdma_sel, enable)) 
    {
        printf("Exec. Failed: '%s set rxratelimit config [enMode] [pktMode]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    if(qdmamgr_lib_set_rx_ratelimit_pkt_mode(g_qdma_sel, mode)) 
    {
        printf("Exec. Failed: '%s set rxratelimit config [enMode] [pktMode]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n") ;
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set rxratelimit config [Enable/Disable] [packet/byte]\n", qdmamgr_name) ;
    return QDMAMGR_FAIL;
}

int do_set_rxratelimit_value(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 ring;
    u32 rate;
        
    if(argc<3)
    {
        goto err;
    }
        
    /* get channel idx */
    ring = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (ring >= RX_RING_NUM)) 
    {
        printf("Error: input ringIdx is '%s', which should between 0 and %d\n", argv[1], (RX_RING_NUM - 1));
        goto err;
    }
    
    /* get rx rate limit value in kbps */
    rate = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input rx rate limit value is '%s', which should be digital\n", argv[2]);
        goto err;
    }
    
    if(rate > (4<<20)) 
    {
        printf("Error: rx rate limit value is %d, which should be less than 4000000!", rate);
        goto err;
    }

    /* set rx rate limit value */
    if(qdmamgr_lib_set_rx_ratelimit_value(g_qdma_sel, ring, rate))
    {
        printf("Exec. Failed: '%s set rxratelimit value [ChnlID] [pir]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set rxratelimit value [RingIdx:0~%d] [1~1000000]\n", qdmamgr_name, RX_RING_NUM - 1);
    return QDMAMGR_FAIL;
}

const cmds_t rxratelimit_set_command[] = 
{
    {"config",  do_set_rxratelimit_config,  0, 2, "Set QDMA rx ratelimit config <enMode, pktMode>"},
    {"value",   do_set_rxratelimit_value,   0, 2, "Set QDMA rx ratelimit value <ringIdx, value>"},
    {"help",    api_lib_help,               0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,                       0, 0, NULL}
};

int do_set_rxratelimit(int argc, char* argv[], void *p)
{
    return subcmd(rxratelimit_set_command, argc, argv, (void *)rxratelimit_set_command);
}

int do_set_virtual_channel_config(int argc, char *argv[], void *p)
{
    u8 enable;
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE mode;

    if(argc < 3)
    {
        goto err;
    }

    if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) 
    {
        enable = 1;
    }
    else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) 
    {
        enable = 0;
    }
    else
    {
        goto err;
    }
    
    if(strncasecmp(argv[2], "32Queue", strlen("32Queue")) == 0) 
    {
        mode = ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE_32QUEUE ;
    } 
    else if(strncasecmp(argv[2], "16Queue", strlen("16Queue")) == 0) 
    {
        mode = ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE_16QUEUE ;
    } 
    else 
    {
        goto err;
    }

    if(qdmamgr_lib_set_virtual_channel_config(g_qdma_sel, enable, mode)) 
    {
        printf("Exec. Failed: '%s set virtual_channel config [enMode] [virChnlMode]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set virtual_channel config [Enable/Disable] [32Queue/16Queue]\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_virtual_channel_value(int argc, char *argv[], void *p)
{
    int idx = 0;
    char *endPtr = NULL;
    u8 phy_channel;
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS qos = {0};
    int argcMin[ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_MAX]={7, 3, 6, 5};
        
    /* get channel idx */
    phy_channel = strtoul(argv[1], &endPtr, 0);
    if((*endPtr!='\0') || (phy_channel >= (CONFIG_QDMA_CHANNEL>>1))) 
    {
        printf("Error: input physical channel is '%s', which should between 0 and %d\n", argv[1], (CONFIG_QDMA_CHANNEL>>1) - 1);
        goto err;
    }
    
    /* get qos type */
    for(idx = 0; idx < ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_MAX; idx++) 
    {
        if(strncasecmp(argv[2], (char *)vChnlQosTypeStr[idx], strlen(argv[2])) == 0) 
        {
            qos.qos_mode = idx;
            break;
        }
    }
    if(idx == ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_MAX) /* qosType error */
    {
        goto err;
    }
    
    /* get wrr weight */
    if(argc < argcMin[qos.qos_mode]) 
    {
        printf("Fault: qosType %s need at least %d wrr weight\n", vChnlQosTypeStr[qos.qos_mode], argcMin[qos.qos_mode] - 3);
        goto err;
    }
    
    for(idx = 0; idx < ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_MAX; idx++) 
    {
        /* skipped the redundant wrr weight */
        if(idx >= argcMin[qos.qos_mode] - 3)
            break;
            
        qos.queue_weight[idx] = strtoul(argv[3 + idx], &endPtr, 0);
        if((*endPtr!='\0') || (qos.queue_weight[idx] == 0)) 
        {
            printf("Fault: qos weight should not be 0, which should between 1 and 100!\n");
            goto err;
        }
    }

    /* set qos value */
    if(qdmamgr_lib_set_virtual_channel_qos(g_qdma_sel, phy_channel, &qos))
    {
        printf("Exec. Failed: '%s set virtual_channel value [phyChnlID] [phyQosType] [weight0~weight3]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set qos virtual_channel [phyChnlID:0~%d] [SP/WRR/SPWRR2/SPWRR3] [weight0:1~100] ... [weightn:1~100]\n", qdmamgr_name, (CONFIG_QDMA_CHANNEL>>1) - 1);
    return QDMAMGR_FAIL;
}

const cmds_t virtual_channel_set_command[] = 
{
    {"config",  do_set_virtual_channel_config,  0, 2, "Set QDMA virtual channel config <enMode, virChnlMode>"},
    {"value",   do_set_virtual_channel_value,   0, 2, "Set QDMA virtual channel value <phyChnl, phyQosType, weight0, ..., weight7>"},
    {"help",    api_lib_help,                   0, 0, "Show qdmamgr set usage."},
    {NULL,      NULL,                           0, 0, NULL}
};

int do_set_virtual_channel(int argc, char* argv[], void *p)
{
    return subcmd(virtual_channel_set_command, argc, argv, (void *)virtual_channel_set_command);
}

int do_set_rx_low_thrh(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    int ringIdx = 0;
    ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD threshold;
    
    if(argc < (RX_RING_NUM + 1))
    {
        goto err;
    }

    memset(&threshold, 0, sizeof(threshold));
    for(ringIdx = 0; ringIdx < RX_RING_NUM; ringIdx++)
    {
        threshold.threshold[ringIdx] = strtoul(argv[ringIdx + 1], &endPtr, 0);
        if(*endPtr!='\0') 
        {
            printf("Error: input ring%d low threshold is '%s', which should be digital\n", ringIdx, argv[ringIdx + 1]);
            goto err;
        }
    }
    
    if(qdmamgr_lib_set_rx_ring_low_threshold(g_qdma_sel, &threshold)) 
    {
        printf("Exec. Failed: '%s set rx_low_thrh [ring0LowThrh] [ring1LowThrh] ...' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set rx_low_thrh [ring0LowThrh] [ring1LowThrh] ...\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_dei_drop_en(int argc, char *argv[], void *p)
{
    u8 enable;
    
    if(argc<2)
    {
        goto err;
    }

    if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) 
    {
        enable = 1;
    } 
    else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) 
    {
        enable = 0;
    } 
    else 
    {
        goto err;
    }
    
    if(qdmamgr_lib_set_dei_drop_en(g_qdma_sel, enable)) 
    {
        printf("Exec. Failed: '%s set dei_drop_en [enMode]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set dei_drop_en [Enable/Disable]\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_lmgr_low_thrh(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u32 threshold;

    if(argc < 2)
    {
        goto err;
    }

    threshold = strtoul(argv[1], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input lmgr low threshold is '%s', which should be digital\n", argv[1]);
        goto err;
    }
    
    if(qdmamgr_lib_set_lmgr_low_threshold(g_qdma_sel, threshold))
    {
        printf("Exec. Failed: '%s set lmgr_low_thrh LowThrh' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set lmgr_low_thrh LowThrh\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_overhead(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 enable, cnt = 0;

    if(argc < 3)
    {
        goto err;
    }

    if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
    {
        enable = 1;
    }
    else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0)
    {
        enable = 0;
    }
    else
    {
        goto err;
    }
        
    cnt = (u8)strtoul(argv[2], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input cnt is '%s', which should be digital\n", argv[2]);
        goto err;
    }

    if (qdmamgr_lib_set_overhead_en(g_qdma_sel, enable)) 
    {
        printf("Exec. Failed: %s set overhead <enable/disable> <cnt>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    if (qdmamgr_lib_set_overhead_value(g_qdma_sel, cnt)) 
    {
        printf("Exec. Failed: %s set overhead <enable/disable> <cnt>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set overhead <enable/disable> <cnt>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_mac_res_limit(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u32 channel_threshold, queue_threshold;

    if(argc < 3)
    {
        goto err;
    }

    channel_threshold = strtoul(argv[1], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input channel_thrh is '%s', which should be digital\n", argv[1]);
        goto err;
    }
        
    queue_threshold = strtoul(argv[2], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input queue_thrh is '%s', which should be digital\n", argv[2]);
        goto err;
    }

    if (qdmamgr_lib_set_mac_resource_limit(g_qdma_sel, channel_threshold, queue_threshold)) 
    {
        printf("Exec. Failed: %s set mac_res_limit <channel_thrh> <queue_thrh>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
   
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set set mac_res_limit <channel_thrh> <queue_thrh>\n", qdmamgr_name);
    return QDMAMGR_FAIL;

}

int do_set_tx_buf_thrh(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 enable;
    u8 channel_threshold, total_threshold;

    if(argc < 4)
    {
        goto err;
    }

    if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) 
    {
        enable = 1;
    } 
    else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) 
    {
        enable = 0;
    } 
    else 
    {
        goto err;
    }

    channel_threshold = strtoul(argv[2], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input channel_thrh is '%s', which should be digital\n", argv[2]);
        goto err;
    }
        
    total_threshold = strtoul(argv[3], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input total_thrh is '%s', which should be digital\n", argv[3]);
        goto err;
    }

    if (qdmamgr_lib_set_txbuf_threshold(g_qdma_sel, enable, channel_threshold, total_threshold)) 
    {
        printf("Exec. Failed: %s set mac_res_limit <channel_thrh> <queue_thrh>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
   
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set set mac_res_limit <channel_thrh> <queue_thrh>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_prefetch(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 prefetch_en, overdrag_en;
    u32 overdrag_cnt;

    if(argc < 4)
    {
        goto err;
    }

    if(strncasecmp(argv[1], "Enable", strlen("Enable")) == 0) 
    {
        prefetch_en = 1;
    } 
    else if(strncasecmp(argv[1], "Disable", strlen("Disable")) == 0) 
    {
        prefetch_en = 0;
    } 
    else 
    {
        goto err;
    }

    if(strncasecmp(argv[2], "Enable", strlen("Enable")) == 0) 
    {
        overdrag_en = 1;
    } 
    else if(strncasecmp(argv[2], "Disable", strlen("Disable")) == 0) 
    {
        overdrag_en = 0;
    } 
    else 
    {
        goto err;
    }

    overdrag_cnt = strtoul(argv[3], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input overdrag_cnt is '%s', which should be digital\n", argv[3]);
        goto err;
    }
        
    if (qdmamgr_lib_set_prefetch(g_qdma_sel, prefetch_en, overdrag_en, overdrag_cnt)) 
    {
        printf("Exec. Failed: %s set prefetch <prefetch_en:enable/disable> <overdrag_en:enable/disable> <overdrag_cnt>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
   
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set set prefetch <prefetch_en:enable/disable> <overdrag_en:enable/disable> <overdrag_cnt>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_tx_dba_report(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 channel;
    ECNT_QDMAMGR_DBA_REPORT dba_report;

    if(argc < 6)
    {
        goto err;
    }

    channel = strtoul(argv[1], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input channel is '%s', which should be digital\n", argv[1]);
        goto err;
    }

    dba_report.cir = strtoul(argv[2], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input cir is '%s', which should be digital\n", argv[2]);
        goto err;
    }

    dba_report.cbs = strtoul(argv[3], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input cbs is '%s', which should be digital\n", argv[3]);
        goto err;
    }

    dba_report.pir = strtoul(argv[4], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input pir is '%s', which should be digital\n", argv[4]);
        goto err;
    }

    dba_report.pbs = strtoul(argv[5], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input pbs is '%s', which should be digital\n", argv[5]);
        goto err;
    }

    if (qdmamgr_lib_set_dba_report(g_qdma_sel, channel, &dba_report)) 
    {
        printf("Exec. Failed: %s set tx_dba_report <channel> <cir> <cbs> <pir> <pbs>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
   
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set tx_dba_report <channel> <cir> <cbs> <pir> <pbs>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_tx_dyn_cngst(int argc, char *argv[], void *p)
{
    u8 enable = 0;

    if(argc < 2)
    {
        goto err;
    }

    if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
    {
        enable = 1;
    }
    else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0)
    {
        enable = 0;
    }
    else
    {
        goto err;
    }
        
    if (qdmamgr_lib_set_txq_cngst_mode(g_qdma_sel, enable)) 
    {
        printf("Exec. Failed: %s set tx_dyn_cngst <enable/disable> <cnt>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set tx_dyn_cngst <enable/disable> <cnt>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_cngst_total_thrh(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u16 min_threshold, max_threshold;

    if(argc < 3)
    {
        goto err;
    }

    max_threshold = strtoul(argv[1], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input cngst_total_thrh max threshold is '%s', which should be digital\n", argv[1]);
        goto err;
    }

    min_threshold = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input cngst_total_thrh min threshold is '%s', which should be digital\n", argv[2]);
        goto err;
    }
    
    if(qdmamgr_lib_set_txq_cngst_total_threshold(g_qdma_sel, max_threshold, min_threshold))
    {
        printf("Exec. Failed: '%s set cngst_total_thrh <max> <min>' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set cngst_total_thrh <max> <min>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_cngst_channel_thrh(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u16 min_threshold, max_threshold;

    if(argc < 3)
    {
        goto err;
    }

    max_threshold = strtoul(argv[1], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input cngst_channel_thrh max threshold is '%s', which should be digital\n", argv[1]);
        goto err;
    }

    min_threshold = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input cngst_channel_thrh min threshold is '%s', which should be digital\n", argv[2]);
        goto err;
    }
    
    if(qdmamgr_lib_set_txq_cngst_channel_threshold(g_qdma_sel, max_threshold, min_threshold))
    {
        printf("Exec. Failed: '%s set cngst_channel_thrh <max> <min>' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set cngst_channel_thrh <max> <min>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_cngst_queue_thrh(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u16 min_threshold, max_threshold;

    if(argc < 3)
    {
        goto err;
    }

    max_threshold = strtoul(argv[1], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input cngst_queue_thrh max threshold is '%s', which should be digital\n", argv[1]);
        goto err;
    }

    min_threshold = strtoul(argv[2], &endPtr, 0);
    if(*endPtr!='\0') 
    {
        printf("Error: input cngst_queue_thrh min threshold is '%s', which should be digital\n", argv[2]);
        goto err;
    }
    
    if(qdmamgr_lib_set_txq_cngst_queue_threshold(g_qdma_sel, max_threshold, min_threshold))
    {
        printf("Exec. Failed: '%s set cngst_queue_thrh <max> <min>' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set cngst_queue_thrh <max> <min>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_set_downstream_qos(int argc, char *argv[], void *p)
{
	u8 enable;
    
    if(argc<2)
    {
        goto err;
    }

    if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) 
    {
        enable = 1;
    } 
    else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) 
    {
        enable = 0;
    } 
    else 
    {
        goto err;
    }
    
    if(qdmamgr_lib_set_downstream_qos(g_qdma_sel, enable)) 
    {
        printf("Exec. Failed: '%s set downstream_qos [enMode]' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Exec. Successful\n");
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s set downstream_qos [enable/disable]\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

const cmds_t qdmamgr_set_command[] = 
{
    {"qos",                 do_set_qos,                         0, 0, "Set QDMA qos information"},
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
	{"general_glb_init",	 do_general_set_glb_rate_cfg,		0, 0, "Set QDMA general_glb_init <enable/disable> <ratelimit/trtcm> <slowtickratio> <fasttick(us)>"},
	{"general_glb_ratelimit",do_general_set_glb_ratelimit_mode, 0, 0, "Set QDMA general_glb_ratelimit <config/value>"},
#endif
#if defined(TCSUPPORT_CPU_EN7580)
    {"general_tx_init",     do_general_set_tx_trtcm_cfg,        0, 0, "Set QDMA general_tx_init <enable/disable> <ratelimit/trtcm> <slowtickratio> <fasttick(us)>"},
    {"general_tx_ratelimit",do_general_set_tx_ratelimit_mode,   0, 0, "Set QDMA general_tx_ratelimit <config/value>"},
    {"general_tx_trtcm",    do_general_set_tx_trtcm_mode,       0, 0, "Set QDMA general_tx_trtcm <config/value>"},
#if defined(TCSUPPORT_CPU_EN7581)
	{"general_tx_q_init",   do_general_set_tx_queue_ratelimit_cfg, 0, 0, "Set QDMA general_tx_q_init <enable/disable> <slowtickratio> <fasttick(us)>"},
	{"general_tx_q_ratelimit",do_general_set_tx_queue_ratelimit_mode, 0, 0, "Set QDMA general_tx_q_ratelimit <config/value>"},
#endif
    {"general_rx_init",     do_general_set_rx_trtcm_cfg,        0, 0, "Set QDMA general_rx_init <enable/disable> <ratelimit/trtcm> <slowtickratio> <fasttick(us)>"},
    {"general_rx_ratelimit",do_general_set_rx_ratelimit_mode,   0, 0, "Set QDMA general_rx_ratelimit <config/value>"},
    {"general_rx_trtcm",    do_general_set_rx_trtcm_mode,       0, 0, "Set QDMA general_rx_trtcm <config/value>"},

	{"multicast",			do_set_multicast_en,				0,		0,          "Set QDMA multicast <enable/disable>"},
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	{"general_sla_init", 	do_general_set_sla_trtcm_cfg,        0,		0,          "Set QDMA general_sla_init <slowtickratio> <fasttick(us)>"},
	{"general_sla_ratelimit",do_general_set_sla_ratelimit_mode,  0,		0,          "Set QDMA general_sla_ratelimit <config/value>"},
	{"general_sla_trtcm",	do_general_set_sla_trtcm_mode,       0,		0,          "Set QDMA general_sla_trtcm <config/value>"},

	{"sla_cfg",				do_set_sla_channel_cfg,				0,		0,          "Set QDMA sla_cfg <sla_idx> <enable/disable> <channel_ix>"},

	{"aging_cfg",			do_set_aging_cfg,					0,		0,          "Set QDMA aging_cfg <global/queue>"},

	{"mul_sptag_cfg",		do_set_multicast_sptag_cfg, 		0,		0,			"Set QDMA mul_sptag_cfg <chnl_idx> <enable/disable(keep hifield)> <sp_tag>"},
	{"mul_fport_cfg",		do_set_multicast_fport_cfg,			0,		0,          "Set QDMA mul_fport_cfg <chnl_idx> <port> <nbq>"},
#endif

    {"flowcnt",	            do_set_flowcnt,                     0,		0,          "Set QDMA flowcnt <config/value>"},
    
    {"txwred",              do_set_tx_wred_cfg,                 0,      0,          "Set QDMA TX WRED <threshold/probability>"},
    {"rxred",               do_set_rx_red_cfg,                  0,      0,          "Set QDMA RX RED <probability>"},
#endif
    {"txratelimit",         do_set_txratelimit_value,           0, 0, "Set QDMA txratelimit <chnlIdx, enMode, value>"},
    {"rxratelimit",         do_set_rxratelimit,                 0, 0, "Set QDMA rxratelimit <config/value>"},
    {"virtual_channel",     do_set_virtual_channel,             0, 0, "Set QDMA virtual channel <config/value>"},
    {"rx_low_thrh",         do_set_rx_low_thrh,                 0, 0, "Set QDMA rx low threshold <ring0LowThrh, ring1LowThrh, ...>"},
    {"dei_drop_en",         do_set_dei_drop_en,                 0, 0, "Set QDMA dei drop <enable/disable>"},
    {"lmgr_low_thrh",       do_set_lmgr_low_thrh,               0, 0, "Set LMGR low threshold <LowThrh>"},
    {"overhead",            do_set_overhead,                    0, 0, "Set QDMA overhead <enable/disable> <cnt>"},
    {"mac_res_limit",       do_set_mac_res_limit,               0, 0, "Set QDMA mac_res_limit <channel_thrh> <queue_thrh>"},
    {"tx_buf_thrh",         do_set_tx_buf_thrh,                 0, 0, "Set QDMA tx_buf_thrh <enable/disable> <channel_thrh> <total_thrh>"},
    {"prefetch",            do_set_prefetch,                    0, 0, "Set QDMA prefetch <prefetch_en:enable/disable> <overdrag_en:enable/disable> <overdrag_cnt>"},
    {"tx_dba_report",       do_set_tx_dba_report,               0, 0, "Set QDMA tx_dba_report <channel> <cir> <cbs> <pir> <pbs>"},
    {"tx_dyn_cngst",        do_set_tx_dyn_cngst,                0, 0, "Set QDMA tx_dyn_cngst <enable/disable>"},
    {"cngst_total_thrh",    do_set_cngst_total_thrh,            0, 0, "Set QDMA cngst_total_thrh <max> <min>"},
    {"cngst_channel_thrh",  do_set_cngst_channel_thrh,          0, 0, "Set QDMA cngst_channel_thrh <max> <min>"},
    {"cngst_queue_thrh",    do_set_cngst_queue_thrh,            0, 0, "Set QDMA cngst_queue_thrh <max> <min>"},
    {"downstream_qos",    	do_set_downstream_qos,            	0, 0, "Set QDMA downstream_qos <enable/disable>"},
    {"help",                api_lib_help,                       0, 0, "Show qdmamgr set usage."},
    {NULL,                  NULL,                               0, 0, NULL}
};

/*******************************************************************************/
/*  GET commands                                                                                                               */
/*******************************************************************************/
int do_get_qos(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 channel = 0;
    ECNT_QDMAMGR_WRR_WEIGHT_BASE base;
    ECNT_QDMAMGR_WRR_WEIGHT_SCALE scale;
    ECNT_QDMAMGR_CHANNEL_QOS channel_qos = {0};
     
    /* check argc */
    if (argc < 2)
    {
        printf("Format: %s get qos value [ChnlID:0~%d]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
        return QDMAMGR_FAIL;
    }
    else if (argc >= 2)
    {
        channel = strtol(argv[1],&endPtr,0);
        
        if ((channel >= CONFIG_QDMA_CHANNEL) || (*endPtr != '\0'))
        {
            printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
            return QDMAMGR_FAIL;
        }
    }

    /* get qos parameter */
    if (qdmamgr_lib_get_wrr_mode_cfg(g_qdma_sel, &base, &scale))
    {
        printf("Exec. Failed: '%s get qos value' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    if (qdmamgr_lib_get_channel_qos(g_qdma_sel, channel, &channel_qos))
    {
        printf("Exec. Failed: '%s get qos value' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    /* output the parameter for test */
    switch (channel_qos.qos_mode)
    {
        case ECNT_QDMAMGR_QOS_MODE_WRR:
            printf("channel-%d: qosType = WRR\n", channel);
            printf("q7:q6:q5:q4:q3:q2:q1:q0 = %d:%d:%d:%d:%d:%d:%d:%d\n",
                channel_qos.queue_weight[7],
                channel_qos.queue_weight[6],
                channel_qos.queue_weight[5],
                channel_qos.queue_weight[4],
                channel_qos.queue_weight[3],
                channel_qos.queue_weight[2],
                channel_qos.queue_weight[1],
                channel_qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_QOS_MODE_SP:
            printf("channel-%d: qosType = SP\n", channel);
            break;
        case ECNT_QDMAMGR_QOS_MODE_SPWRR7:
            printf("channel-%d: qosType = SPWRR7\n", channel);
            printf("q6:q5:q4:q3:q2:q1:q0 = %d:%d:%d:%d:%d:%d:%d\n",
                channel_qos.queue_weight[6],
                channel_qos.queue_weight[5],
                channel_qos.queue_weight[4],
                channel_qos.queue_weight[3],
                channel_qos.queue_weight[2],
                channel_qos.queue_weight[1],
                channel_qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_QOS_MODE_SPWRR6:
            printf("channel-%d: qosType = SPWRR6\n", channel);
            printf("q5:q4:q3:q2:q1:q0 = %d:%d:%d:%d:%d:%d\n",
                channel_qos.queue_weight[5],
                channel_qos.queue_weight[4],
                channel_qos.queue_weight[3],
                channel_qos.queue_weight[2],
                channel_qos.queue_weight[1],
                channel_qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_QOS_MODE_SPWRR5:
            printf("channel-%d: qosType = SPWRR5\n", channel);
            printf("q4:q3:q2:q1:q0 = %d:%d:%d:%d:%d\n",
                channel_qos.queue_weight[4],
                channel_qos.queue_weight[3],
                channel_qos.queue_weight[2],
                channel_qos.queue_weight[1],
                channel_qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_QOS_MODE_SPWRR4:
            printf("channel-%d: qosType = SPWRR4\n", channel);
            printf("q3:q2:q1:q0 = %d:%d:%d:%d\n",
                channel_qos.queue_weight[3],
                channel_qos.queue_weight[2],
                channel_qos.queue_weight[1],
                channel_qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_QOS_MODE_SPWRR3:
            printf("channel-%d: qosType = SPWRR3\n", channel);
            printf("q2:q1:q0 = %d:%d:%d\n",
                channel_qos.queue_weight[2],
                channel_qos.queue_weight[1],
                channel_qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_QOS_MODE_SPWRR2:
            printf("channel-%d: qosType = SPWRR2\n", channel);
            printf("q1:q0 = %d:%d\n",
                channel_qos.queue_weight[1],
                channel_qos.queue_weight[0]);
            break;
        default:
            printf("channel-%d: qosType is not correct\n", channel);
            break;
    }
    
    printf("weightBase = %s\n", ((ECNT_QDMAMGR_WRR_WEIGHT_BASE_BY_PACKET == base) ? ("packet mode"):("byte mode")));
    if (ECNT_QDMAMGR_WRR_WEIGHT_SCALE_1B == scale)
    {
        printf("weightScale = %s\n", "1Byte");
    }
    else if (ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B == scale)
    {
        printf("weightScale = %s\n", "16Byte");
    }
    else if (ECNT_QDMAMGR_WRR_WEIGHT_SCALE_64B == scale)
    {
        printf("weightScale = %s\n", "64Byte");
    }
    return QDMAMGR_SUCCESS;
}

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
int do_general_get_glb_trtcm_cfg(int argc, char *argv[], void *p)
{
	ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

	/*set glb module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;

	/* get glb ratelimit/trtcm module init */
	if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: '%s get general_glb_init' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}

	/* print info */
	printf("GLOBAL RATELIMIT CONFIG\n");
	printf("%s, %s, slow_tick ratio = %d, slow_tick = %dus, fast_tick = %dus\n",
			(cfg.enable == ECNT_QDMAMGR_ENABLE) ? ("ENABLE"):("DISABLE"),
			(cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"),
			cfg.slow_tick_ratio, (cfg.slow_tick_ratio * cfg.fast_tick),
			cfg.fast_tick);

	return QDMAMGR_SUCCESS;
}

int do_general_get_glb_ratelimit_mode_info(int argc, char *argv[], void *p)
{
    unsigned int cur_ticksel=0, unit_base=0, cur_unit=0;
    ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_CFG glb_rateLimit_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_VALUE glb_rateLimit_value;
    
    /* get glb ratelimit/trtcm module init */
    ratectl_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;
	if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) {
		printf("Exec. Failed: '%s get general_glb_ratelimit' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	printf("THE GLOBAL MODULE WORKS MODE is:%s\n", (ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"));

	glb_rateLimit_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;
	glb_rateLimit_cfg.index = 0 ;
	if(qdmamgr_lib_get_ratectl_ratelimit_mode_cfg(g_qdma_sel, &glb_rateLimit_cfg)) {
		printf("Exec. Failed: '%s get general_glb_ratelimit' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	glb_rateLimit_value.module = ECNT_QDMAMGR_RATECTL_MODULE_GLOBAL;
	glb_rateLimit_value.index = 0;
	if(qdmamgr_lib_get_ratectl_ratelimit_mode_value(g_qdma_sel, &glb_rateLimit_value)) {
		printf("Exec. Failed: '%s get general_glb_ratelimit' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	/*cal the unit*/
	if(glb_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
		unit_base = 8000;     /**8bits *1000/(curTicksel*10e-6 ) kbps **/
	else
		unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
 	cur_ticksel = (glb_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
					ratectl_cfg.fast_tick: (ratectl_cfg.slow_tick_ratio*ratectl_cfg.fast_tick);
	if(cur_ticksel == 0)
		cur_unit = 0 ;
	else
		cur_unit = unit_base / cur_ticksel ;
	/*print info*/
	printf("%s, %s:%dus , Unit: %d%s , %s , ratelimit value = %d%s , PBS = %d%s , Tocken = %d%s\n" , 
		(glb_rateLimit_cfg.enable == ECNT_QDMAMGR_ENABLE) ? ("ENABLE"):("DISABLE"),
		(glb_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
		cur_ticksel,
		cur_unit,
		(glb_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
		(glb_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
		glb_rateLimit_value.rate, 
		(glb_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
		glb_rateLimit_value.bucket_size,
		(glb_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
		(glb_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (glb_rateLimit_value.rate * cur_ticksel / 1000000):(glb_rateLimit_value.rate * cur_ticksel / 8000), 
		(glb_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
		);

	return QDMAMGR_SUCCESS;
}
#endif

#if defined(TCSUPPORT_CPU_EN7580)
int do_general_get_tx_trtcm_cfg(int argc, char *argv[], void *p)
{
    ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

    /*set tx module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM ;

    /* get tx ratelimit/trtcm module init */
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s get general_tx_init' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    /* print info */
    printf("EGRESS TRTCM CONFIG\n");
    printf("%s, %s, slow_tick ratio = %d, slow_tick = %dus, fast_tick = %dus\n",
            cfg.enable ? ("ENABLE"):("DISABLE"),
            (cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"),
            cfg.slow_tick_ratio, (cfg.slow_tick_ratio * cfg.fast_tick),
            cfg.fast_tick);

    return QDMAMGR_SUCCESS;
}

int do_general_get_tx_ratelimit_mode_info(int argc, char *argv[], void *p)
{
    unsigned char channel = 0 , index = 0;
    char *endPtr = NULL;
    unsigned int cur_ticksel=0, unit_base=0, cur_unit=0;
    ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_CFG tx_rateLimit_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_VALUE tx_rateLimit_value;
    
    /* get tx ratelimit/trtcm module init */
    ratectl_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) 
    { 
        printf("Exec. Failed: '%s get general_tx_ratelimit' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    printf("THE TRTCM MODULE WORKS MODE is:%s\n", (ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"));
    
    if(argc >= 2) 
    {
        channel = strtol(argv[1], &endPtr, 0);
        if((*endPtr!='\0') || (channel > CONFIG_QDMA_CHANNEL)) 
        {
            printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
            return QDMAMGR_FAIL;
        }
    }

    for(index = 0; index < CONFIG_QDMA_CHANNEL; index++)
    {
        if ((argc >= 2) && (channel < CONFIG_QDMA_CHANNEL))
        {
            if(index != channel)
                continue;
        }
        
        tx_rateLimit_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
        tx_rateLimit_cfg.index = index;
        if(qdmamgr_lib_get_ratectl_ratelimit_mode_cfg(g_qdma_sel, &tx_rateLimit_cfg)) 
        {
            printf("Exec. Failed: '%s get general_tx_ratelimit' command failed\n", qdmamgr_name);
            return QDMAMGR_FAIL;
        }
        
        tx_rateLimit_value.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
        tx_rateLimit_value.index = index;
        if(qdmamgr_lib_get_ratectl_ratelimit_mode_value(g_qdma_sel, &tx_rateLimit_value)) 
        {
            printf("Exec. Failed: '%s get general_tx_ratelimit' command failed\n", qdmamgr_name);
            return QDMAMGR_FAIL;
        }
        
        /*cal the unit*/
        if(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
        {
            unit_base = 8000;     /**8bits *1000/(curTicksel*10e-6 ) kbps **/
        }
        else
        {
            unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
        }
            
        cur_ticksel = (tx_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
                        ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio * ratectl_cfg.fast_tick);
        if(cur_ticksel == 0)
        {
            cur_unit = 0;
        }
        else
        {
            cur_unit = unit_base / cur_ticksel;
        }
        
        /*print info*/
        printf("channel-%d:\n",index);
        printf("%s, %s:%dus , Unit: %d%s , %s , ratelimit value = %d%s , PBS = %d%s , Tocken = %d%s \n" , 
            tx_rateLimit_cfg.enable ? ("ENABLE"):("DISABLE"),
            (tx_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
            cur_ticksel,
            cur_unit,
            (tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
            (tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
            tx_rateLimit_value.rate, 
            (tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
            tx_rateLimit_value.bucket_size,
            (tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (tx_rateLimit_value.rate * cur_ticksel / 1000000):(tx_rateLimit_value.rate * cur_ticksel / 8000), 
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
            );
    }

    return QDMAMGR_SUCCESS;
}

#if defined(TCSUPPORT_CPU_EN7581)
int do_general_get_tx_queue_ratelimit_cfg(int argc, char *argv[], void *p)
{
    ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

    /*set tx module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT ;

    /* get tx ratelimit module init */
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s get general_tx_q_init' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    /* print info */
    printf("EGRESS TRTCM CONFIG\n");
    printf("%s, %s, slow_tick ratio = %d, slow_tick = %dus, fast_tick = %dus\n",
            cfg.enable ? ("ENABLE"):("DISABLE"),
            (cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"),
            cfg.slow_tick_ratio, (cfg.slow_tick_ratio * cfg.fast_tick),
            cfg.fast_tick);

    return QDMAMGR_SUCCESS;
}

int do_general_get_tx_queue_ratelimit_mode_info(int argc, char *argv[], void *p)
{
	unsigned short queue = 0 , index = 0;
	char *endPtr = NULL;
	unsigned int cur_ticksel=0, unit_base=0, cur_unit=0;
	ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
	ECNT_QDMAMGR_RATELIMIT_MODE_CFG tx_rateLimit_cfg;
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE tx_rateLimit_value;
	
	/* get tx ratelimit/trtcm module init */
	ratectl_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT;
	if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) 
	{ 
		printf("Exec. Failed: '%s get general_tx_q_ratelimit' command failed\n", qdmamgr_name);
		return QDMAMGR_FAIL;
	}
	printf("THE TRTCM MODULE WORKS MODE is:%s\n", (ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"));
	
	if(argc >= 2) 
	{
		queue = strtol(argv[1], &endPtr, 0);
		if((*endPtr!='\0') || (queue > QUEUE_ALL_NUM)) 
		{
			printf("Error: input queue is '%s', which should between 0 and %d\n", argv[1], QUEUE_ALL_NUM - 1);
			return QDMAMGR_FAIL;
		}
	}

	for(index = 0; index < QUEUE_ALL_NUM; index++)
	{
		if ((argc >= 2) && (queue < QUEUE_ALL_NUM))
		{
			if(index != queue)
				continue;
		}
		
		tx_rateLimit_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT;
		tx_rateLimit_cfg.index = index;
		if(qdmamgr_lib_get_ratectl_ratelimit_mode_cfg(g_qdma_sel, &tx_rateLimit_cfg)) 
		{
			printf("Exec. Failed: '%s get general_tx_q_ratelimit' command failed\n", qdmamgr_name);
			return QDMAMGR_FAIL;
		}
		
		tx_rateLimit_value.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_QUEUE_RATELIMIT;
		tx_rateLimit_value.index = index;
		if(qdmamgr_lib_get_ratectl_ratelimit_mode_value(g_qdma_sel, &tx_rateLimit_value)) 
		{
			printf("Exec. Failed: '%s get general_tx_q_ratelimit' command failed\n", qdmamgr_name);
			return QDMAMGR_FAIL;
		}
		
		/*cal the unit*/
		if(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
		{
			unit_base = 8000;	  /**8bits *1000/(curTicksel*10e-6 ) kbps **/
		}
		else
		{
			unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
		}
			
		cur_ticksel = (tx_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
						ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio * ratectl_cfg.fast_tick);
		if(cur_ticksel == 0)
		{
			cur_unit = 0;
		}
		else
		{
			cur_unit = unit_base / cur_ticksel;
		}
		
		/*print info*/
		printf("queue-%d:\n",index);
		printf("%s, %s:%dus , Unit: %d%s , %s , ratelimit value = %d%s , PBS = %d%s , Tocken = %d%s \n" , 
			tx_rateLimit_cfg.enable ? ("ENABLE"):("DISABLE"),
			(tx_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
			cur_ticksel,
			cur_unit,
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
			tx_rateLimit_value.rate, 
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
			tx_rateLimit_value.bucket_size,
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (tx_rateLimit_value.rate * cur_ticksel / 1000000):(tx_rateLimit_value.rate * cur_ticksel / 8000), 
			(tx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
			);
	}

	return QDMAMGR_SUCCESS;
}
#endif

int do_general_get_tx_trtcm_mode_info(int argc, char *argv[], void *p)
{
    unsigned char channel = 0 , index = 0;
    char *endPtr = NULL;
    unsigned int cur_ticksel = 0, unit_base = 0, cur_unit = 0;
    ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
    ECNT_QDMAMGR_TRTCM_MODE_CFG tx_trtcm_cfg;
    ECNT_QDMAMGR_TRTCM_MODE_VALUE tx_trtcm_value;
    
    /* get tx ratelimit/trtcm module init */
    ratectl_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) 
    {
        printf("Exec. Failed: '%s get general_tx_trtcm' command failed\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }
    
    printf("THE TRTCM MODULE WORKS MODE is:%s\n", (ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"));
    /*if ratelimit mode, just return*/
    if(ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT)
    {
        printf("the mode is not trtcm mode, no info for show\n");
        return QDMAMGR_SUCCESS;
    }
    
    if(argc >= 2) 
    {
        channel = strtol(argv[1], &endPtr, 0);
        if((*endPtr!='\0') || (channel > CONFIG_QDMA_CHANNEL)) 
        {
            printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
            return QDMAMGR_FAIL;
        }
    }

    for(index = 0; index < CONFIG_QDMA_CHANNEL; index++)
    {
        if ((argc >= 2) && (channel < CONFIG_QDMA_CHANNEL))
        {
            if (index != channel)
                continue;
        }
        
        tx_trtcm_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
        tx_trtcm_cfg.index = index;
        if(qdmamgr_lib_get_ratectl_trtcm_mode_cfg(g_qdma_sel, &tx_trtcm_cfg)) 
        {
            printf("Exec. Failed: '%s get general_tx_trtcm' command failed\n", qdmamgr_name);
            return QDMAMGR_FAIL;
        }
        
        tx_trtcm_value.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
        tx_trtcm_value.index = index;
        if(qdmamgr_lib_get_ratectl_trtcm_mode_value(g_qdma_sel, &tx_trtcm_value)) 
        {
            printf("Exec. Failed: '%s get general_tx_trtcm' command failed\n", qdmamgr_name);
            return QDMAMGR_FAIL;
        }

        printf("channel-%d:\n", index);
        /*CBS*/
        printf("cbs: ");
        /*cal the unit*/
        if(tx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
        {
            unit_base = 8000;      /**8bits *1000/(curTicksel*10e-6 ) kbps **/
        }
        else
        {
            unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
        }
        cur_ticksel = (tx_trtcm_cfg.cbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
                        ratectl_cfg.fast_tick: (ratectl_cfg.slow_tick_ratio * ratectl_cfg.fast_tick);
        if(cur_ticksel == 0)
        {
            cur_unit = 0;
        }
        else
        {
            cur_unit = unit_base / cur_ticksel;
        }
        /*print info*/
        printf("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s , Tocken = %d%s \n" , 
            tx_trtcm_cfg.cbs_meter_en ? ("ENABLE"):("DISABLE"),
            (tx_trtcm_cfg.cbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
            cur_ticksel,
            cur_unit,
            (tx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
            (tx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
            tx_trtcm_value.cbs, 
            (tx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
            tx_trtcm_value.cbs_bucket_size,
            (tx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
			(tx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (tx_trtcm_value.cbs * cur_ticksel / 1000000):(tx_trtcm_value.cbs * cur_ticksel / 8000), 
			(tx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
            );
            
        /*PBS*/
        printf("pbs: ");
        /*cal the unit*/
        if(tx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
            unit_base = 8000;      /**8bits *1000/(curTicksel*10e-6 ) kbps **/
        else
            unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
        cur_ticksel = (tx_trtcm_cfg.pbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
                        ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio * ratectl_cfg.fast_tick);
        if(cur_ticksel == 0)
            cur_unit = 0 ;
        else
            cur_unit = unit_base / cur_ticksel ;
        /*print info*/
        printf("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s , Tocken = %d%s \n" , 
            tx_trtcm_cfg.pbs_meter_en ? ("ENABLE"):("DISABLE"),
            (tx_trtcm_cfg.pbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
            cur_ticksel,
            cur_unit,
            (tx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
            (tx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
            tx_trtcm_value.pbs, 
            (tx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
            tx_trtcm_value.pbs_bucket_size,
            (tx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
			(tx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (tx_trtcm_value.pbs * cur_ticksel / 1000000):(tx_trtcm_value.pbs * cur_ticksel / 8000), 
			(tx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
            );
    }

    return QDMAMGR_SUCCESS;
}

int do_general_get_rx_trtcm_cfg(int argc, char *argv[], void *p)
{
    ECNT_QDMAMGR_RATECTL_CFG cfg = {0};

    /*set tx module*/
    cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM ;

    /* get tx ratelimit/trtcm module init */
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &cfg)) 
    {
        printf("Exec. Failed: '%s get general_rx_init' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    /* print info */
    printf("INGRESS TRTCM CONFIG\n");
    printf("%s, %s, slow_tick ratio = %d, slow_tick = %dus, fast_tick = %dus\n",
            cfg.enable ? ("ENABLE"):("DISABLE"),
            (cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"),
            cfg.slow_tick_ratio, (cfg.slow_tick_ratio * cfg.fast_tick),
            cfg.fast_tick);

    return QDMAMGR_SUCCESS;
}

int do_general_get_rx_ratelimit_mode_info(int argc, char *argv[], void *p)
{
    unsigned char meter_index = 0, index = 0;
    unsigned short tmp = 0,meter_group = 0;
    char *endPtr = NULL;
    unsigned int cur_ticksel = 0, unit_base = 0, cur_unit = 0;
    ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_CFG rx_rateLimit_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_VALUE rx_rateLimit_value;
    unsigned char maxMeterIndex[3] = {127,31,15} ; /*128 meters for ratelimit ; or 64 pairs for trtcm*/
    int meterGrpId = 0,meterGrpIdStart = 0,maxMeterGrpId = 0;
    
    /* get rx ratelimit/trtcm module init */
    ratectl_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM ;
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) 
    {
        printf("Exec. Failed: '%s get general_rx_ratelimit' command failed\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }
    printf("THE TRTCM MODULE WORKS MODE is:%s\n", (ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"));

    if(argc >= 2) 
    {
        tmp = strtol(argv[1], &endPtr, 0);
        meter_index = QDMAMGR_GET_METER_IDX(tmp);
        meter_group = QDMAMGR_GET_METER_GRP(tmp);
        if((*endPtr!='\0') || (meter_index > maxMeterIndex[meter_group])) 
        {
             printf("Error: input value is '%s', meter_grp %d:meter_idx %d which should between 0 and %d\n", argv[1], meter_group,meter_index,maxMeterIndex[meter_group] - 1);
             return QDMAMGR_FAIL;
        }

        if((ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM)&&(0 == meter_group))
        {
            printf("METER GROUP:0 is trtcm mode,skip\n");
            return QDMAMGR_FAIL;
        }
    }
    else
    {
        if(ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM)
        {
            printf("METER GROUP:0 is trtcm mode,skip\n");
            meterGrpIdStart = 1;
        }
    }

    #ifdef TCSUPPORT_CPU_EN7581
    maxMeterGrpId = 2;
    #endif

    for(meterGrpId = meterGrpIdStart;meterGrpId < maxMeterGrpId+1;meterGrpId++)
    {
        if((argc >= 2)&&(meterGrpId != meter_group))
            continue;
        printf("METER GROUP:%d\n",meterGrpId);
        for(index = 0; index < maxMeterIndex[meterGrpId]; index++)
        {
            if ((argc >= 2) && (meter_index < maxMeterIndex[meterGrpId]))
            {
                if (index != meter_index)
                    continue;
            }
            
            rx_rateLimit_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;
            rx_rateLimit_cfg.index = index|(meterGrpId<<8) ;
            if(qdmamgr_lib_get_ratectl_ratelimit_mode_cfg(g_qdma_sel, &rx_rateLimit_cfg)) 
            {
                printf("Exec. Failed: '%s get general_rx_ratelimit' command failed\n", qdmamgr_name);
                return QDMAMGR_FAIL;
            }
            
            rx_rateLimit_value.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;
            rx_rateLimit_value.index = index|(meterGrpId<<8);
            if(qdmamgr_lib_get_ratectl_ratelimit_mode_value(g_qdma_sel, &rx_rateLimit_value)) 
            {
                printf("Exec. Failed: '%s get general_rx_ratelimit' command failed\n", qdmamgr_name);
                return QDMAMGR_FAIL;
            }
            
            /*cal the unit*/
            if(rx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
                unit_base = 8000;      /**8bits *1000/(curTicksel*10e-6 ) kbps **/
            else
                unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
            cur_ticksel = (rx_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
                            ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio * ratectl_cfg.fast_tick);
            if(cur_ticksel == 0)
                cur_unit = 0 ;
            else
                cur_unit = unit_base / cur_ticksel ;
            
            /*print info*/
            if((index < RX_RING_NUM)&&(0 == meterGrpId))
                printf("meter-%d(ring%d):\n", index, index);
            else
                printf("meter-%d:\n", index);
            
            printf("%s, %s:%dus , Unit: %d%s , %s , ratelimit value = %d%s , PBS = %d%s , Tocken = %d%s \n" , 
                rx_rateLimit_cfg.enable ? ("ENABLE"):("DISABLE"),
                (rx_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
                cur_ticksel,
                cur_unit,
                (rx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
                (rx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
                rx_rateLimit_value.rate, 
                (rx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
                rx_rateLimit_value.bucket_size,
                (rx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
    			(rx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (rx_rateLimit_value.rate * cur_ticksel / 1000000):(rx_rateLimit_value.rate * cur_ticksel / 8000), 
    			(rx_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
                );
        }
    }

    return QDMAMGR_SUCCESS;
}

int do_general_get_rx_trtcm_mode_info(int argc, char *argv[], void *p)
{
    unsigned short meter_index = 0 , index = 0;
    unsigned short tmp = 0,meter_group = 0;
    char *endPtr = NULL;
    unsigned int cur_ticksel = 0, unit_base = 0, cur_unit = 0;
    ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
    ECNT_QDMAMGR_TRTCM_MODE_CFG rx_trtcm_cfg;
    ECNT_QDMAMGR_TRTCM_MODE_VALUE rx_trtcm_value;
    unsigned char maxMeterIndex[3] = {63,15,7} ; /*128 meters for ratelimit ; or 64 pairs for trtcm*/
    int meterGrpId = 0,maxMeterGrpId = 0;/*maxMeterGrpId = 0,only get meter grp0 info in trtcm mode now*/
    
    /* get rx ratelimit/trtcm module init */
    ratectl_cfg.module= ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM ;
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) 
    {
        printf("Exec. Failed: '%s get general_rx_trtcm' command failed\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }
    printf("THE TRTCM MODULE WORKS MODE is:%s\n", (ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_TRTCM) ? ("TRTCM MODE"):("RATELIMIT MODE"));
    /*if ratelimit mode, just return*/
    if(ratectl_cfg.mode == ECNT_QDMAMGR_TRTCM_MODE_RATELIMIT)
    {
        printf("the mode is not trtcm mode, no info for show\n");
        return QDMAMGR_SUCCESS;
    }
    
    if(argc >= 2) 
    {
        tmp = strtol(argv[1], &endPtr, 0);
        meter_index = QDMAMGR_GET_METER_IDX(tmp);
        meter_group = QDMAMGR_GET_METER_GRP(tmp);
        if((*endPtr!='\0') || (meter_index > maxMeterIndex[meter_group])) 
        {
            printf("Error: input value is '%s', meter_grp %d:meter_idx %d which should between 0 and %d\n", argv[1], meter_group,meter_index,maxMeterIndex[meter_group] - 1);
            return QDMAMGR_FAIL;
        }
    }

    for(meterGrpId = 0;meterGrpId < maxMeterGrpId+1;meterGrpId++)
    {
        if((argc >= 2)&&(meterGrpId != meter_group))
            continue;
        
        printf("METER GROUP:%d\n",meterGrpId);
        for(index = 0 ; index < maxMeterIndex[meterGrpId] ; index++)
        {
            if ((argc >= 2) && (meter_index < maxMeterIndex[meterGrpId]))
            {
                if (index != meter_index)
                    continue;
            }
            
            rx_trtcm_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;
            rx_trtcm_cfg.index = index|(meterGrpId<<8);
            if(qdmamgr_lib_get_ratectl_trtcm_mode_cfg(g_qdma_sel, &rx_trtcm_cfg)) 
            {
                printf("Exec1. Failed: '%s get general_rx_trtcm' command failed\n", qdmamgr_name);
                return QDMAMGR_FAIL;
            }
            
            rx_trtcm_value.module = ECNT_QDMAMGR_RATECTL_MODULE_INGRESS_TRTCM;
            rx_trtcm_value.index = index|(meterGrpId<<8);
            if(qdmamgr_lib_get_ratectl_trtcm_mode_value(g_qdma_sel, &rx_trtcm_value)) 
            {
                printf("Exec2. Failed: '%s get general_rx_trtcm' command failed\n", qdmamgr_name);
                return QDMAMGR_FAIL;
            }

            /*print info*/
            if ((index < RX_RING_NUM)&&(0 == meterGrpId))
            {
                printf("meter-%d(ring%d):\n", index, index);
            }
            else
            {
                printf("meter-%d:\n", index);
            }
            /*CBS*/
            printf("cbs: ");
            /*cal the unit*/
            if(rx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
                unit_base = 8000;      /**8bits *1000/(curTicksel*10e-6 ) kbps **/
            else
                unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
            cur_ticksel = (rx_trtcm_cfg.cbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
                            ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio * ratectl_cfg.fast_tick);
            if(cur_ticksel == 0)
                cur_unit = 0 ;
            else
                cur_unit = unit_base / cur_ticksel ;
            /*print info*/
            printf("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s , Tocken = %d%s \n" , 
                rx_trtcm_cfg.cbs_meter_en ? ("ENABLE"):("DISABLE"),
                (rx_trtcm_cfg.cbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
                cur_ticksel,
                cur_unit,
                (rx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
                (rx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
                rx_trtcm_value.cbs, 
                (rx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
                rx_trtcm_value.cbs_bucket_size,
                (rx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
    			(rx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (rx_trtcm_value.cbs * cur_ticksel / 1000000):(rx_trtcm_value.cbs * cur_ticksel / 8000), 
    			(rx_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
                );
                
            /*PBS*/
            printf("pbs: ");
            /*cal the unit*/
            if(rx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
                unit_base = 8000;      /**8bits *1000/(curTicksel*10e-6 ) kbps **/
            else
                unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
            cur_ticksel = (rx_trtcm_cfg.pbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
                            ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio * ratectl_cfg.fast_tick);
            if(cur_ticksel == 0)
                cur_unit = 0 ;
            else
                cur_unit = unit_base / cur_ticksel ;
            /*print info*/
            printf("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s , Tocken = %d%s \n" , 
                rx_trtcm_cfg.pbs_meter_en ? ("ENABLE"):("DISABLE"),
                (rx_trtcm_cfg.pbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
                cur_ticksel,
                cur_unit,
                (rx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
                (rx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
                rx_trtcm_value.pbs, 
                (rx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
                rx_trtcm_value.pbs_bucket_size,
                (rx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
    			(rx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (rx_trtcm_value.pbs * cur_ticksel / 1000000):(rx_trtcm_value.pbs * cur_ticksel / 8000), 
    			(rx_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
                );
        }
    }

    return QDMAMGR_SUCCESS;
}

int do_get_multicast_en(int argc, char *argv[], void *p)
{
	unsigned char multicast_en =0;

	if(qdmamgr_lib_get_multicast_en(g_qdma_sel, &multicast_en)) {
		printf("Exec. Failed: '%s get multicast'\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}

	printf("QDMA multicast is %s\n", ((multicast_en==ECNT_QDMAMGR_ENABLE)?("Enable"):("Disable")));
	return QDMAMGR_SUCCESS;
}

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
int do_general_get_sla_trtcm_cfg(int argc, char *argv[], void *p)
{
	ECNT_QDMAMGR_RATECTL_CFG cfg = {0};
	
	/*set sla module*/
	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	/* get sla ratelimit/trtcm module init */
	if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &cfg)) 
	{
		printf("Exec. Failed: '%s get general_glb_init' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}

	/* print info */
	printf("SLA TRTCM CONFIG\n");
	printf("slow_tick ratio = %d, slow_tick = %dus, fast_tick = %dus\n",
			cfg.slow_tick_ratio, (cfg.slow_tick_ratio * cfg.fast_tick),
			cfg.fast_tick);

	return QDMAMGR_SUCCESS;
}

int do_general_get_sla_ratelimit_mode_info(int argc, char *argv[], void *p)
{
	unsigned char slaIndex = 0, queueIndex = 0; 
	unsigned char Sindex = 0, Qindex = 0;
    char *endPtr = NULL;
    unsigned int cur_ticksel=0, unit_base=0, cur_unit=0;
    ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_CFG sla_rateLimit_cfg;
    ECNT_QDMAMGR_RATELIMIT_MODE_VALUE sla_rateLimit_value;
    
    /* get tx ratelimit/trtcm module init */
    ratectl_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) {
		printf("Exec. Failed: '%s get general_sla_ratelimit' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	/* SLA fix TRTCM mode */
	printf("THE SLA MODULE WORKS MODE is: TRTCM MODE\n");
	
	if(argc >= 2){
		slaIndex = strtol(argv[1], &endPtr, 0);
		if((*endPtr!='\0') || (slaIndex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
			printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
			return QDMAMGR_FAIL;
		}
	}
	if(argc >= 3){
		queueIndex = strtol(argv[2], &endPtr, 0);
		if((*endPtr!='\0') || (queueIndex>=CONFIG_QDMA_QUEUE)) {
			printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
			return QDMAMGR_FAIL;
		}
	}

	/* SLA support 4 channels at most */
	for(Sindex = 0 ; Sindex < ECNT_QDMAMGR_SLA_MAX_NUM ; Sindex++)
	{
		if( (argc >= 2) && (slaIndex < ECNT_QDMAMGR_SLA_MAX_NUM) )
		{
			if( Sindex != slaIndex )
				continue ;
		}

		for(Qindex = 0 ; Qindex < CONFIG_QDMA_QUEUE ; Qindex++)
		{
			if( (argc >= 3) && (queueIndex < CONFIG_QDMA_QUEUE) )
			{
				if( Qindex != queueIndex )
					continue ;
			}
		
			sla_rateLimit_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM ;
			sla_rateLimit_cfg.index = ((Sindex<<3) | Qindex);
			if(qdmamgr_lib_get_ratectl_ratelimit_mode_cfg(g_qdma_sel, &sla_rateLimit_cfg)) {
				printf("Exec. Failed: '%s get general_sla_ratelimit' command failed\n", qdmamgr_name) ;
				return QDMAMGR_FAIL;
			}
			sla_rateLimit_value.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM ;
			sla_rateLimit_value.index = ((Sindex<<3) | Qindex);
			if(qdmamgr_lib_get_ratectl_ratelimit_mode_value(g_qdma_sel, &sla_rateLimit_value)) {
				printf("Exec. Failed: '%s get general_sla_ratelimit' command failed\n", qdmamgr_name) ;
				return QDMAMGR_FAIL;
			}
			/*cal the unit*/
			if(sla_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
				unit_base = 8000;	 /**8bits *1000/(curTicksel*10e-6 ) kbps **/
			else
				unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
			cur_ticksel = (sla_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
							ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio*ratectl_cfg.fast_tick);
			if(cur_ticksel == 0)
				cur_unit = 0 ;
			else
				cur_unit = unit_base / cur_ticksel ;
			/*print info*/
			printf("SLA-%d, queue-%d:\n", Sindex, Qindex);
			printf("%s, %s:%dus , Unit: %d%s , %s , ratelimit value = %d%s , PBS = %d%s , Tocken = %d%s\n" , 
				(sla_rateLimit_cfg.enable == ECNT_QDMAMGR_ENABLE) ? ("ENABLE"):("DISABLE"),
				(sla_rateLimit_cfg.tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
				cur_ticksel,
				cur_unit,
				(sla_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
				(sla_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
				sla_rateLimit_value.rate, 
				(sla_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
				sla_rateLimit_value.bucket_size,
				(sla_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
				(sla_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (sla_rateLimit_value.rate * cur_ticksel / 1000000):(sla_rateLimit_value.rate * cur_ticksel / 8000), 
				(sla_rateLimit_cfg.rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
				);
		}
	}

	return QDMAMGR_SUCCESS;
}
int do_general_get_sla_trtcm_mode_info(int argc, char *argv[], void *p)
{
	unsigned char slaIndex = 0, queueIndex = 0;
	unsigned char Sindex = 0, Qindex = 0;
	char *endPtr = NULL;
    unsigned int cur_ticksel = 0, unit_base = 0, cur_unit = 0;
    ECNT_QDMAMGR_RATECTL_CFG ratectl_cfg;
    ECNT_QDMAMGR_TRTCM_MODE_CFG sla_trtcm_cfg;
    ECNT_QDMAMGR_TRTCM_MODE_VALUE sla_trtcm_value;
    
    /* get SLA ratelimit/trtcm module init */
    ratectl_cfg.module= ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM ;
    if(qdmamgr_lib_get_ratectl_cfg(g_qdma_sel, &ratectl_cfg)) {
		printf("Exec. Failed: '%s get general_sla_trtcm' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	/* SLA fix TRTCM mode */
	printf("THE SLA MODULE WORKS MODE is: TRTCM MODE\n");
	
	if(argc>=2) {
		slaIndex = strtol(argv[1], &endPtr, 0);
		if((*endPtr!='\0') || (slaIndex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
			printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
			return QDMAMGR_FAIL;
		}
	}
	if(argc>=3) {
		queueIndex = strtol(argv[2], &endPtr, 0);
		if((*endPtr!='\0') || (queueIndex>=CONFIG_QDMA_QUEUE)) {
			printf("Error: input queue is '%s', which should between 0 and %d\n", argv[2], CONFIG_QDMA_QUEUE-1);
			return QDMAMGR_FAIL;
		}
	}

	for(Sindex = 0 ; Sindex < ECNT_QDMAMGR_SLA_MAX_NUM ; Sindex++)
	{
		if( (argc >= 2) && (slaIndex < ECNT_QDMAMGR_SLA_MAX_NUM) )
		{
			if( Sindex != slaIndex )
				continue ;
		}

		for(Qindex = 0 ; Qindex < CONFIG_QDMA_QUEUE ; Qindex++)
		{
			if( (argc >= 3) && (queueIndex < CONFIG_QDMA_QUEUE) )
			{
				if( Qindex != queueIndex )
					continue ;
			}
			sla_trtcm_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM ;
			sla_trtcm_cfg.index = ((Sindex<<3) | Qindex);
			if(qdmamgr_lib_get_ratectl_trtcm_mode_cfg(g_qdma_sel, &sla_trtcm_cfg)) {
				printf("Exec. Failed: '%s get general_sla_trtcm' command failed\n", qdmamgr_name) ;
				return QDMAMGR_FAIL;
			}
			sla_trtcm_value.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM ;
			sla_trtcm_value.index = ((Sindex<<3) | Qindex);
			if(qdmamgr_lib_get_ratectl_trtcm_mode_value(g_qdma_sel, &sla_trtcm_value)) {
				printf("Exec. Failed: '%s get general_sla_trtcm' command failed\n", qdmamgr_name) ;
				return QDMAMGR_FAIL;
			}

			printf("SLA-%d, queue-%d:\n", Sindex, Qindex);
			/*CBS*/
			printf("cbs: ");
			/*cal the unit*/
			if(sla_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
				unit_base = 8000;	  /**8bits *1000/(curTicksel*10e-6 ) kbps **/
			else
				unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
			cur_ticksel = (sla_trtcm_cfg.cbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
							ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio*ratectl_cfg.fast_tick);
			if(cur_ticksel == 0)
				cur_unit = 0 ;
			else
				cur_unit = unit_base / cur_ticksel ;
			/*print info*/
			printf("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s , Tocken = %d%s\n" , 
				(sla_trtcm_cfg.cbs_meter_en == ECNT_QDMAMGR_ENABLE) ? ("ENABLE"):("DISABLE"),
				(sla_trtcm_cfg.cbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
				cur_ticksel,
				cur_unit,
				(sla_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
				(sla_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
				sla_trtcm_value.cbs, 
				(sla_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
				sla_trtcm_value.cbs_bucket_size,
				(sla_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
				(sla_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (sla_trtcm_value.cbs * cur_ticksel / 1000000):(sla_trtcm_value.cbs * cur_ticksel / 8000), 
				(sla_trtcm_cfg.cbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
				);
			/*PBS*/
			printf("pbs: ");
			/*cal the unit*/
			if(sla_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_BYTE)
				unit_base = 8000;	  /**8bits *1000/(curTicksel*10e-6 ) kbps **/
			else
				unit_base = 1000000;   /**1/(curTicksel*10e-6) pps **/
			cur_ticksel = (sla_trtcm_cfg.pbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? 
							ratectl_cfg.fast_tick : (ratectl_cfg.slow_tick_ratio*ratectl_cfg.fast_tick);
			if(cur_ticksel == 0)
				cur_unit = 0 ;
			else
				cur_unit = unit_base / cur_ticksel ;
			/*print info*/
			printf("%s, %s:%dus , Unit: %d%s , %s , para value = %d%s , PBS = %d%s , Tocken = %d%s\n" , 
				(sla_trtcm_cfg.pbs_meter_en == ECNT_QDMAMGR_ENABLE) ? ("ENABLE"):("DISABLE"),
				(sla_trtcm_cfg.pbs_tick_type == ECNT_QDMAMGR_TICK_TYPE_FAST) ? ("FastTick"):("SlowTick"),
				cur_ticksel,
				cur_unit,
				(sla_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
				(sla_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet Mode"):("Byte Mode"),
				sla_trtcm_value.pbs, 
				(sla_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("pps"):("kbps"),
				sla_trtcm_value.pbs_bucket_size,
				(sla_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte"),
				(sla_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? (sla_trtcm_value.pbs * cur_ticksel / 1000000):(sla_trtcm_value.pbs * cur_ticksel / 8000), 
				(sla_trtcm_cfg.pbs_rate_mode == ECNT_QDMAMGR_RATECTL_MODE_PACKET) ? ("Packet"):("Byte")
				);
		}
	}

	return QDMAMGR_SUCCESS;
}

int do_get_sla_channel_cfg(int argc, char *argv[], void *p)
{
	unsigned char SLAIndex = 0;
	unsigned char tmpIndex = 0;
	unsigned char enable = 0;
	unsigned char chnlIdx = 0;
	char *endPtr = NULL;
	
	if(argc>=2) {
		SLAIndex = strtol(argv[1], &endPtr, 0);
		if((*endPtr!='\0') || (SLAIndex>=ECNT_QDMAMGR_SLA_MAX_NUM)) {
			printf("Error: input sla index is '%s', which should between 0 and %d\n", argv[1], ECNT_QDMAMGR_SLA_MAX_NUM-1);
			return QDMAMGR_FAIL;
		}
	}

	for(tmpIndex = 0 ; tmpIndex < ECNT_QDMAMGR_SLA_MAX_NUM ; tmpIndex++)
	{
		if( (argc >= 2) && (SLAIndex < ECNT_QDMAMGR_SLA_MAX_NUM) )
		{
			if( tmpIndex != SLAIndex )
				continue ;
		}

		if(qdmamgr_lib_get_sla_channel_cfg(g_qdma_sel, tmpIndex, &enable, &chnlIdx)) {
			printf("Exec. Failed: '%s get sla_cfg' command failed\n", qdmamgr_name) ;
			return QDMAMGR_FAIL;
		}

		printf("sla_index:%d,\t%s,\tchannel_index:%d\n", tmpIndex, enable?"Enable":"Disable", chnlIdx);
	}

	return QDMAMGR_SUCCESS;
}

int do_get_aging_cfg(int argc, char *argv[], void *p)
{
	ECNT_QDMAMGR_QOS_AGING_CFG qosAgingCfg = {0};
	unsigned char chnlIdx = 0, queueIdx = 0, agingEn = 0;

	if(qdmamgr_lib_get_qos_aging_cfg(g_qdma_sel, &qosAgingCfg)) {
		printf("Exec. Failed: '%s get aging_cfg' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	printf("AgingEN:\t\t%s \nAgingMethod:\t\t%s \nAgingFastReplaceEN:\t%s \nAgingTime:\t\t%dus\n", 
			(qosAgingCfg.agingEn == ECNT_QDMAMGR_ENABLE)?"Enable":"Disable", 
			(qosAgingCfg.agingMethod == ECNT_QDMAMGR_QOS_AGING_THRESHOLD)?"When used dscpnum greater than total_max_threshold":"Always",
			qosAgingCfg.agingFastReplaceEn?"Enable":"Disable",
			qosAgingCfg.agingTime);

	for(chnlIdx = 0 ; chnlIdx < CONFIG_QDMA_CHANNEL ; chnlIdx++)
	{
		printf("channel-%d: ", chnlIdx);
		for(queueIdx = 0 ; queueIdx < CONFIG_QDMA_QUEUE ; queueIdx++)
		{
			if(qdmamgr_lib_get_per_queue_aging_cfg(g_qdma_sel, chnlIdx, queueIdx, &agingEn)) {
				printf("Exec. Failed: '%s get aging_cfg' command failed\n", qdmamgr_name) ;
				return QDMAMGR_FAIL;
			}

			printf("q%d %s, ", queueIdx, agingEn?"Enable":"Disable");
		}
		printf("\n");
	}

	return QDMAMGR_SUCCESS;
}

int do_get_multicast_sptag_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	unsigned char chnlIdx = 0;
	unsigned char index = 0;
	unsigned char sptagKeepHiEn = 0;
	unsigned short sptag = 0;

	if(argc<2)
		goto err;

	/* 1. get channel index */
	chnlIdx = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (chnlIdx>CONFIG_QDMA_CHANNEL)) {
		printf("Error: input channel index is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL-1);
		goto err;
	}

	if( chnlIdx == CONFIG_QDMA_CHANNEL ) {/* get all channel cfg info */
		for( index = 0; index < CONFIG_QDMA_CHANNEL; index++ ) {
			if(qdmamgr_lib_get_multicast_sptag_cfg(g_qdma_sel, index, &sptagKeepHiEn, &sptag)) {
				printf("Exec. Failed: '%s get mul_sptag_cfg [ChannelID]' command failed\n", qdmamgr_name);
				return QDMAMGR_FAIL;
			}
			printf("channel-%d: sptag keep hi-field-%d, sptag-0x%X, hi-0x%X, lo-0x%X\n", index, 
				sptagKeepHiEn, sptag, sptag & 0xffc0, sptag & 0x3f);
		}
	} else {
		if(qdmamgr_lib_get_multicast_sptag_cfg(g_qdma_sel, chnlIdx, &sptagKeepHiEn, &sptag)) {
			printf("Exec. Failed: '%s get mul_sptag_cfg [ChannelID]' command failed\n", qdmamgr_name);
			return QDMAMGR_FAIL;
		}
		printf("channel-%d: sptag keep hi-field-%d, sptag-0x%X, hi-0x%X, lo-0x%X\n", chnlIdx, 
				sptagKeepHiEn, sptag, sptag & 0xffc0, sptag & 0x3f);
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s get mul_sptag_cfg [ChannelID:0~%d]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL-1) ;
	return QDMAMGR_FAIL;
}

int do_get_multicast_fport_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	unsigned char chnlIdx = 0;
	unsigned char index = 0;
	unsigned short fport = 0;
	
	if(argc<2)
		goto err;

	/* 1. get channel index */
	chnlIdx = strtoul(argv[1], &endPtr, 0);
	if((*endPtr!='\0') || (chnlIdx>16)) {
		printf("Error: input channel index is '%s', which should between 0 and %d\n", argv[1], 16-1);
		goto err;
	}

	if( chnlIdx == 16 ) {/* get all channel cfg info */
		for( index = 0; index < 16; index++ ) {
			if(qdmamgr_lib_get_multicast_fport_cfg(g_qdma_sel, index, &fport)) {
				printf("Exec. Failed: '%s get mul_fport_cfg [ChannelID]' command failed\n", qdmamgr_name);
				return QDMAMGR_FAIL;
			}
			printf("channel-%d: port-%d, nbq-%d\n", index, fport >> 5, fport & 0x1F);
		}
	} else {
		if(qdmamgr_lib_get_multicast_fport_cfg(g_qdma_sel, chnlIdx, &fport)) {
			printf("Exec. Failed: '%s get mul_fport_cfg [ChannelID]' command failed\n", qdmamgr_name);
			return QDMAMGR_FAIL;
		}
		printf("channel-%d: port-%d, nbq-%d\n", chnlIdx, fport >> 5, fport & 0x1F);
	}
	
	printf("Exec. Successful\n") ;
	return QDMAMGR_SUCCESS;

err:
	printf("Format: %s get mul_fport_cfg [ChannelID:0~%d]\n", qdmamgr_name, 16-1) ;
	return QDMAMGR_FAIL;
}
#endif

int do_get_flowcnt_info(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
	unsigned char incDropEn = 0;
    ECNT_QDMAMGR_FLOW_CNT cntValue = {0};
    unsigned char grp_idx = 0;
    unsigned char cnt_idx = 0;
    unsigned char index = 0;
    unsigned char max_index = 0;

    /*1. get group index*/
    if(argc>=2) {
        grp_idx = strtoul(argv[1], &endPtr, 0);
        if((*endPtr!='\0') || (grp_idx>(ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1))) {
            printf("Error: input group index is '%s', which should between 0 and %d\n", argv[1], (ECNT_QDMAMGR_FLOWCNT_MAX_DROUP_NUM-1));
            printf("Exec. Failed: '%s get flowcnt' command failed\n", qdmamgr_name) ;
            return QDMAMGR_FAIL;
        }
    }else{
        printf("Exec. Failed: '%s get flowcnt' command failed\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }

    if(grp_idx ==0)
        max_index = ECNT_QDMAMGR_FLOWCNT_GRP0_MAX_IDX_NUM;
    else if(grp_idx ==1)
        max_index = ECNT_QDMAMGR_FLOWCNT_GRP1_MAX_IDX_NUM;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    else if(grp_idx ==2)
        max_index = ECNT_QDMAMGR_FLOWCNT_GRP2_MAX_IDX_NUM;
#endif

    /*2. get inc index*/
    if(argc>=3) {
        cnt_idx = strtoul(argv[2], &endPtr, 0);
        if( (*endPtr!='\0') || (cnt_idx>(max_index-1)) ) {
            printf("Error: input cnt index is '%s', which should between 0 and %d\n", argv[2], (max_index-1));
            printf("Exec. Failed: '%s get flowcnt' command failed\n", qdmamgr_name) ;
            return QDMAMGR_FAIL;
    	}
    }else{
        cnt_idx = max_index;
    }

	/*3.  get cnt inc drop enable? */
	if(qdmamgr_lib_get_flow_cntr_cfg(g_qdma_sel, grp_idx, &incDropEn)) {
		printf("Exec. Failed: '%s get flowcnt' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	printf("QDMA FLOW CNT GROUP%d INC DROP PKT:%s\n", grp_idx, (incDropEn == ECNT_QDMAMGR_ENABLE) ? ("ENABLE"):("DISABLE"));

	for(index = 0 ; index < max_index ; index++)
	{
		if( (argc >= 3) && (cnt_idx <  max_index) )
		{
			if( index != cnt_idx )
				continue ;
		}
		cntValue.grpIdx = grp_idx ;
		cntValue.cntIdx = index ;
		if(qdmamgr_lib_get_flow_cntr_value(g_qdma_sel, &cntValue)) {
			printf("Exec. Failed: '%s get flowcnt' command failed\n", qdmamgr_name) ;
			return QDMAMGR_FAIL;
		}
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		printf("group:%d , cnt_idx:%d , Byte Cnt:H-%u L-%u , PktCnt:H-%u L-%u \n" , cntValue.grpIdx , cntValue.cntIdx,
			cntValue.byteHigh, cntValue.byteLow, cntValue.pktHigh, cntValue.pktCnt);
#else
		printf("group:%d , cnt_idx:%d , Byte Cnt:H-%u L-%u , PktCnt:%u \n" , cntValue.grpIdx , cntValue.cntIdx,
			cntValue.byteHigh, cntValue.byteLow, cntValue.pktCnt);
#endif
	}

	return QDMAMGR_SUCCESS;
}

int do_get_tx_wred_info(int argc, char *argv[], void *p)
{
	unsigned char enable = 0;
	ECNT_QDMAMGR_TX_WRED_CFG txNormalWred;
	ECNT_QDMAMGR_TX_WRED_CFG txDeiWred;

	memset(&txNormalWred, 0, sizeof(ECNT_QDMAMGR_TX_WRED_CFG));
	memset(&txNormalWred, 0, sizeof(ECNT_QDMAMGR_TX_WRED_CFG));

	/* get tx wred info*/
	if(qdmamgr_lib_get_tx_wred_mode(g_qdma_sel, &enable)) {
		printf("Exec. Failed: '%s get txwred' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}

	txNormalWred.txWredPacketType = ECNT_QDMAMGR_NORMAL_PACKET;
	if(qdmamgr_lib_get_tx_wred_cfg(g_qdma_sel, &txNormalWred)) {
		printf("Exec. Failed: '%s get txwred' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
    
	txDeiWred.txWredPacketType = ECNT_QDMAMGR_DEI_PACKET;
	if(qdmamgr_lib_get_tx_wred_cfg(g_qdma_sel, &txDeiWred)) {
		printf("Exec. Failed: '%s get txwred' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}

    printf("WRED:%s\n",(enable==ECNT_QDMAMGR_ENABLE)?"ENABLE":"DISABLE");
    printf("NORMAL WRED CFG:\n");
    printf("P0:%2d%%   P1:%2d%%   P2:%2d%%   P3:%2d%%\n", txNormalWred.txWredThrProbability[0], txNormalWred.txWredThrProbability[1], 
														txNormalWred.txWredThrProbability[2], txNormalWred.txWredThrProbability[3]);    
    printf("[Threshold ratio]Thr0:%d/8  Thr1:%d/8  Thr2:%d/8  Thr3:%d/8  Thr4:%d/8 [base_dyn_thr]\n", txNormalWred.txWredThrValue[0], 
														txNormalWred.txWredThrValue[1], txNormalWred.txWredThrValue[2], 
														txNormalWred.txWredThrValue[3], txNormalWred.txWredThrValue[4]);
    printf("DEI WRED CFG:\n");
    printf("P0:%2d%%   P1:%2d%%   P2:%2d%%   P3:%2d%%\n", txDeiWred.txWredThrProbability[0], txDeiWred.txWredThrProbability[1], 
														txDeiWred.txWredThrProbability[2], txDeiWred.txWredThrProbability[3]);    
    printf("[Threshold ratio]Thr0:%d/8  Thr1:%d/8  Thr2:%d/8  Thr3:%d/8  Thr4:%d/8 [base_dyn_thr]\n", txDeiWred.txWredThrValue[0], 
														txDeiWred.txWredThrValue[1], txDeiWred.txWredThrValue[2], 
														txDeiWred.txWredThrValue[3], txDeiWred.txWredThrValue[4]);
    
    return QDMAMGR_SUCCESS;

}

int do_get_rx_red_info(int argc, char *argv[], void *p)
{
	unsigned char normalProb = 0;
	unsigned char deiProb = 0;
    ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD lowThr;
    uint ringIdx=0;

	memset(&lowThr, 0, sizeof(ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD));

    if(qdmamgr_lib_get_rx_red_probability(g_qdma_sel, ECNT_QDMAMGR_NORMAL_PACKET, &normalProb)) {
        printf("Exec. Failed: '%s get rxred' command failed\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }

    if(qdmamgr_lib_get_rx_red_probability(g_qdma_sel, ECNT_QDMAMGR_DEI_PACKET, &deiProb)) {
        printf("Exec. Failed: '%s get rxred' command failed\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }
	
	if(qdmamgr_lib_get_rx_ring_low_threshold(g_qdma_sel, &lowThr)) {
        printf("Exec. Failed: '%s get rxred' command failed\n", qdmamgr_name) ;
		return QDMAMGR_FAIL;
	}
	
    printf("NORMAL RED Probability %d%%\n", normalProb);
    printf("DEI RED Probability %d%%\n", deiProb);
    printf("Rx Ring Low Threshold:\n");
    for(ringIdx = 0; ringIdx < RX_RING_NUM; ringIdx++){
        printf("ring%2d:%4d  ", ringIdx, lowThr.threshold[ringIdx]);
        if(ringIdx%4 == 3)
            printf("\n");
    }
   
    return QDMAMGR_SUCCESS;
}
#endif

int do_get_txratelimit(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 channel, enable = 0;
    u32 rate = 0;
    
    /* check argc */
    if (argc < 2)
    {
        printf("Format: %s get txratelimit [ChnlID:0~%d]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
        return QDMAMGR_FAIL;
    }
    else
    {
        channel = strtol(argv[1], &endPtr, 0);
        if ((channel >= CONFIG_QDMA_CHANNEL) || (*endPtr!='\0'))
        {
            printf("Error: input channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
            return QDMAMGR_FAIL;
        }
    }

    /* get tx ratelimit parameter */
    if (qdmamgr_lib_get_tx_ratelimit_value(g_qdma_sel, channel, &enable, &rate))
    {
        printf("Exec. Failed: '%s get txratelimit' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    /* output the parameter for test */
    printf("Channel-%d: txRateLimit mode = %s\n", channel, enable ? "Enable" : "Disable");
    printf("rateLimit Value = %ukbps\n", rate);
    //printf("pbs Value = %ubyte\n",txRateLimit.pbsParamValue);
    
    return QDMAMGR_SUCCESS;
}

int do_get_rxratelimit(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    int ring;
    u8 enable;
    ECNT_QDMAMGR_RATECTL_MODE mode;
    u32 rate;
       
    /* check argc */
    if (argc < 2)
    {
        printf("Format: %s get rxratelimit [RingID:0~%d]\n", qdmamgr_name, RX_RING_NUM - 1);
        return QDMAMGR_FAIL;
    }
    else
    {
        ring = strtol(argv[1], &endPtr, 0);
        
        if ((ring >= RX_RING_NUM) || (*endPtr!='\0'))
        {
            printf("Error: input ringIdx is '%s', which should between 0 and 1\n", argv[1]);
            return QDMAMGR_FAIL;
        }
    }

    /* get rx ratelimit parameter */
    if (qdmamgr_lib_get_rx_ratelimit_en(g_qdma_sel, &enable))
    {
        printf("Exec. Failed: '%s get rxratelimit' command failed\n",qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    if (qdmamgr_lib_get_rx_ratelimit_pkt_mode(g_qdma_sel, &mode))
    {
        printf("Exec. Failed: '%s get rxratelimit' command failed\n",qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    if (qdmamgr_lib_get_rx_ratelimit_value(g_qdma_sel, ring, &rate))
    {
        printf("Exec. Failed: '%s get rxratelimit' command failed\n",qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    /* output the parameter for test */
    printf("rxRateLimit: enable mode = %s\n", enable ? "Enable" : "Disable");
    printf("rxRateLimit: packet mode = %s\n", mode ? "packet" : "byte");
    
    printf("RingIdx-%d: ratelimit value:%u%s\n", ring, rate, mode ? "pps" : "kbps"); 
    //printf("pbs value = %ubyte\n",rxRateLimit.pbsParamValue);
    
    return QDMAMGR_SUCCESS;
}

int do_get_virtual_channel(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    int phy_channel;
    u8 enable;
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE mode;
    ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS qos = {0};
    
    /* check argc */
    if (argc < 2)
    {
        printf("Format: %s get virtual_channel [ChnlID:0~%d]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
        return QDMAMGR_FAIL;
    }
    else
    {
        phy_channel = strtol(argv[1], &endPtr, 0);
        if((*endPtr!='\0') || (phy_channel > CONFIG_QDMA_CHANNEL)) 
        {
            printf("Error: input physical channel is '%s', which should between 0 and %d\n", argv[1], CONFIG_QDMA_CHANNEL - 1);
            return QDMAMGR_FAIL;
        }
    }

    memset(&qos, 0, sizeof(qos));

    if (qdmamgr_lib_get_virtual_channel_config(g_qdma_sel, &enable, &mode))
    {
        printf("Format: %s get virtual_channel [ChnlID:0~%d]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
        return QDMAMGR_FAIL;
    }

    if (qdmamgr_lib_get_virtual_channel_qos(g_qdma_sel, phy_channel, &qos))
    {
        printf("Format: %s get virtual_channel [ChnlID:0~%d]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
        return QDMAMGR_FAIL;
    }

    printf("virtual channel: enable mode = %s\n", enable ? "Enable" : "Disable");
    printf("virtual channel: queue mode = %s\n", (mode == ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE_16QUEUE) ? "16Queue" : "32Queue");

    /* output the parameter for test */
    switch (qos.qos_mode)
    {
        case ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_WRR:
            printf("virtual channel-%d: qosType = WRR\n", phy_channel);
            printf("q7:q6:q5:q4:q3:q2:q1:q0 = %d:%d:%d:%d:%d:%d:%d:%d\n",
                qos.queue_weight[7],
                qos.queue_weight[6],
                qos.queue_weight[5],
                qos.queue_weight[4],
                qos.queue_weight[3],
                qos.queue_weight[2],
                qos.queue_weight[1],
                qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_SP:
            printf("virtual channel-%d: qosType = SP\n", phy_channel);
            break;
        case ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_SPWRR3:
            printf("virtual channel-%d: qosType = SPWRR3\n", phy_channel);
            printf("q2:q1:q0 = %d:%d:%d\n",
                qos.queue_weight[2],
                qos.queue_weight[1],
                qos.queue_weight[0]);
            break;
        case ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS_MODE_SPWRR2:
            printf("virtual channel-%d: qosType = SPWRR2\n", phy_channel);
            printf("q1:q0 = %d:%d\n",
                qos.queue_weight[1],
                qos.queue_weight[0]);
            break;
        default:
            printf("virtual channel-%d: qosType is not correct\n", phy_channel);
            break;
    }

    return QDMAMGR_SUCCESS;
}

int do_get_rx_low_threshold(int argc, char *argv[], void *p)
{
    int ring =0;
    ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD threshold;

    memset(&threshold, 0, sizeof(threshold));
    if(qdmamgr_lib_get_rx_ring_low_threshold(g_qdma_sel, &threshold)) 
    {
        printf("Exec. Failed: '%s get rx_low_thrh'\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }
    
    printf("rx low threshold:  \n");
    for(ring = 0; ring < RX_RING_NUM; ring++)
    {
        printf( "Ring%d:    %u \n" , ring, threshold.threshold[ring]);
    }
        
    return QDMAMGR_SUCCESS;
}

int do_get_dei_drop_en(int argc, char *argv[], void *p)
{
    u8 enable;
    
    if(qdmamgr_lib_get_dei_drop_en(g_qdma_sel, &enable)) 
    {
        printf("Exec. Failed: '%s get dei_drop_en'\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }

    printf("DEI drop:   %s\n", enable ? "Enable" : "Disable");
    return QDMAMGR_SUCCESS;
}

int do_get_lmgr_low_threshold(int argc, char *argv[], void *p)
{
    u32 threshold;

    if(qdmamgr_lib_get_lmgr_low_threshold(g_qdma_sel, &threshold)) 
    {
        printf("Exec. Failed: '%s get lmgr_low_thrh'\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }
    
    printf("lmgr low threshold:  %u\n", threshold);
    return QDMAMGR_SUCCESS;
}

int do_get_overhead(int argc, char *argv[], void *p)
{
    u8 enable, cnt = 0;

    if (qdmamgr_lib_get_overhead_en(g_qdma_sel, &enable)) 
    {
        printf("Exec. Failed: '%s get ovrehead' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    if (qdmamgr_lib_get_overhead_value(g_qdma_sel, &cnt)) 
    {
        printf("Exec. Failed: '%s get ovrehead' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    printf("Overhead Enable:    %s\n", enable ? "Enable" : "Disable");
    printf("Overhead Cnt   :    %u\n", cnt);
    return QDMAMGR_SUCCESS;
}

int do_get_mac_res_limit(int argc, char *argv[], void *p)
{
    u32 channel_threshold, queue_threshold;

    if (qdmamgr_lib_set_mac_resource_limit(g_qdma_sel, &channel_threshold, &queue_threshold)) 
    {
        printf("Exec. Failed: '%s get mac_res_limit' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    printf("Channel threshold:    %u\n", channel_threshold);
    printf("Queue   threshold:    %u\n", queue_threshold);
    return QDMAMGR_SUCCESS;
}

int do_get_tx_buf_thrh(int argc, char *argv[], void *p)
{
    u8 enable;
    u8 channel_threshold, total_threshold;

    if (qdmamgr_lib_get_txbuf_threshold(g_qdma_sel, &enable, &channel_threshold, &total_threshold)) 
    {
        printf("Exec. Failed: '%s get tx_buf_thrh' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    printf("TXBUF CTRL Enable:    %s\n", enable ? "Enable" : "Disable");
    printf("Channel threshold:    %u\n", channel_threshold);
    printf("Total   threshold:    %u\n", total_threshold);
    return QDMAMGR_SUCCESS;
}

int do_get_tx_dba_report(int argc, char *argv[], void *p)
{
    char *endPtr = NULL;
    u8 channel;
    ECNT_QDMAMGR_DBA_REPORT dba_report;

    if (argc < 2)
    {
        printf("Format: %s get tx_dba_report [ChnlID:0~%d]\n", qdmamgr_name, CONFIG_QDMA_CHANNEL - 1);
        return QDMAMGR_FAIL;
    }

    channel = strtoul(argv[1], &endPtr, 0);
    if(*endPtr != '\0') 
    {
        printf("Error: input channel is '%s', which should be digital\n", argv[1]);
        goto err;
    }

    if (qdmamgr_lib_get_dba_report(g_qdma_sel, channel, &dba_report)) 
    {
        printf("Exec. Failed: %s get tx_dba_report <channel>\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
   
    printf("Channel: %u\n", channel);
    printf("CIR:     %u\n", dba_report.cir);
    printf("CBS:     %u\n", dba_report.cbs);
    printf("PIR:     %u\n", dba_report.pir);
    printf("PBS:     %u\n", dba_report.pbs);
    return QDMAMGR_SUCCESS;

err:
    printf("Format: %s get tx_dba_report <channel>\n", qdmamgr_name);
    return QDMAMGR_FAIL;
}

int do_get_tx_dyn_cngst(int argc, char *argv[], void *p)
{
    u16 total_min, total_max;
    u16 channel_min, channel_max;
    u16 queue_min, queue_max;
    
    if(qdmamgr_lib_get_txq_cngst_total_threshold(g_qdma_sel, &total_max, &total_min))
    {
        printf("Exec. Failed: '%s get tx_dyn_cngst' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    if(qdmamgr_lib_get_txq_cngst_channel_threshold(g_qdma_sel, &channel_max, &channel_min))
    {
        printf("Exec. Failed: '%s get tx_dyn_cngst' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }

    if(qdmamgr_lib_get_txq_cngst_queue_threshold(g_qdma_sel, &queue_max, &queue_min))
    {
        printf("Exec. Failed: '%s get tx_dyn_cngst' command failed\n", qdmamgr_name);
        return QDMAMGR_FAIL;
    }
    
    printf("Total min  : 0x%x\n", total_min);
    printf("Total max  : 0x%x\n", total_max);
    printf("Channel min: 0x%x\n", channel_min);
    printf("Channel max: 0x%x\n", channel_max);
    printf("Queue min  : 0x%x\n", queue_min);
    printf("Queue max  : 0x%x\n", queue_max);
    return QDMAMGR_SUCCESS;
}

int do_get_downstream_qos(int argc, char *argv[], void *p)
{
	u8 enable;
    
    if(qdmamgr_lib_get_downstream_qos(g_qdma_sel, &enable)) 
    {
        printf("Exec. Failed: '%s get downstream_qos state'\n", qdmamgr_name) ;
        return QDMAMGR_FAIL;
    }

    printf("downstream_qos:   %s\n", enable ? "Enable" : "Disable");
    return QDMAMGR_SUCCESS;
}

const cmds_t qdmamgr_get_command[] = 
{
    {"qos",                 do_get_qos,                             0, 0, "Get QDMA qos information"},
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
	{"general_glb_init",	 do_general_get_glb_trtcm_cfg,			0, 0, "Get QDMA glb ratelimit cfg infomation "},
	{"general_glb_ratelimit",do_general_get_glb_ratelimit_mode_info,0, 0, "Get QDMA glb ratelimit infomation "},
#endif
#if defined(TCSUPPORT_CPU_EN7580)
    {"general_tx_init",     do_general_get_tx_trtcm_cfg,            0, 0, "Get QDMA tx ratelimit/trtcm cfg infomation "},
    {"general_tx_ratelimit",do_general_get_tx_ratelimit_mode_info,  0, 0, "Get QDMA tx ratelimit infomation "},
    {"general_tx_trtcm",    do_general_get_tx_trtcm_mode_info,      0, 0, "Get QDMA tx trtcm infomation "},
#if defined(TCSUPPORT_CPU_EN7581)
	{"general_tx_q_init", 	do_general_get_tx_queue_ratelimit_cfg,			0, 0, "Get QDMA tx queue ratelimit cfg infomation "},
	{"general_tx_q_ratelimit",do_general_get_tx_queue_ratelimit_mode_info,	0, 0, "Get QDMA tx queue ratelimit infomation"},
#endif
    {"general_rx_init",     do_general_get_rx_trtcm_cfg,            0, 0, "Get QDMA rx ratelimit/trtcm cfg infomation "},
    {"general_rx_ratelimit",do_general_get_rx_ratelimit_mode_info,  0, 0, "Get QDMA rx ratelimit infomation "},
    {"general_rx_trtcm",    do_general_get_rx_trtcm_mode_info,      0, 0, "Get QDMA rx trtcm infomation "},

	{"multicast",			do_get_multicast_en,					0, 0, "Get QDMA multicast infomation"},
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	{"general_sla_init",	 do_general_get_sla_trtcm_cfg,			0, 0, "Get QDMA sla ratelimit/trtcm cfg infomation "},
	{"general_sla_ratelimit",do_general_get_sla_ratelimit_mode_info,0, 0, "Get QDMA sla ratelimit infomation "},
	{"general_sla_trtcm",	 do_general_get_sla_trtcm_mode_info, 	0, 0, "Get QDMA sla trtcm infomation "},

	{"sla_cfg", 			do_get_sla_channel_cfg, 				0, 0, "Get QDMA sla_cfg infomation"},

	{"aging_cfg",			do_get_aging_cfg,						0, 0, "Get QDMA aging_cfg infomation"},

	{"mul_sptag_cfg",		do_get_multicast_sptag_cfg, 			0, 0, "Get QDMA mul_sptag_cfg <chnl_idx>"},
	{"mul_fport_cfg",		do_get_multicast_fport_cfg, 			0, 0, "Get QDMA mul_fport_cfg <chnl_idx>"},
#endif
	{"flowcnt", 			do_get_flowcnt_info,					0, 0, "Get QDMA flow cnt infomation "},
	{"txwred",				do_get_tx_wred_info,					0, 0, "Get QDMA tx wred infomation "},
	{"rxred",				do_get_rx_red_info, 					0, 0, "Get QDMA rx red infomation "},
#endif
    {"txratelimit",         do_get_txratelimit,                     0, 0, "Get QDMA tx rate limit infomation "},
    {"rxratelimit",         do_get_rxratelimit,                     0, 0, "Get QDMA rx rate limit information"},
    {"virtual_channel",     do_get_virtual_channel,                 0, 0, "Get QDMA virtual channel information"},
    {"rx_low_thrh",         do_get_rx_low_threshold,                0, 0, "Get QDMA rx low threshold"},
    {"dei_drop_en",         do_get_dei_drop_en,                     0, 0, "Get QDMA dei drop enable or not"},
    {"lmgr_low_thrh",       do_get_lmgr_low_threshold,              0, 0, "Get QDMA lmgr low threshold"},
    {"overhead",            do_get_overhead,                        0, 0, "Get QDMA overhead cfg infomation "},
    {"mac_res_limit",       do_get_mac_res_limit,                   0, 0, "Get QDMA mac_res_limit cfg infomation "},
    {"tx_buf_thrh",         do_get_tx_buf_thrh,                     0, 0, "Get QDMA tx_buf_thrh cfg infomation "},
    {"tx_dba_report",       do_get_tx_dba_report,                   0, 0, "Get QDMA tx_dba_report cfg infomation "},
    {"tx_dyn_cngst",        do_get_tx_dyn_cngst,                    0, 0, "Get QDMA tx_dyn_cngst cfg infomation "},
    {"downstream_qos",      do_get_downstream_qos,                  0, 0, "Get QDMA downstream_qos infomation "},
    {"help",                api_lib_help,                           0, 0, "Show qdmamgr get usage."},
    {NULL,                  NULL,                                   0, 0, NULL}
};

/*******************************************************************************/
/*  QDMA commands                                                                                                            */
/*******************************************************************************/
static int do_set(int argc, char* argv[], void *p)
{
    return subcmd(qdmamgr_set_command, argc, argv, (void *)qdmamgr_set_command);
}

static int do_get(int argc, char* argv[], void *p)
{
    return subcmd(qdmamgr_get_command, argc, argv, (void *)qdmamgr_get_command);
}

static const cmds_t qdmamgr_root_command[] = 
{
    {"set",     do_set,         0,      0,          "Set qdma driver status"},
    {"get",     do_get,         0,      0,          "Get qdma driver status"},
    {"help",    api_lib_help,   0,      0,          "Show qdmamgr usage."},
    {NULL, NULL, 0, 0, NULL}
};

int main (int argc, char* argv[])
{
    return subcmd(qdmamgr_root_command, argc, argv, (void *)qdmamgr_root_command);
}

