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
#include "libapi_lib_qdmamgr.h"

struct sk_buff; /* avoid coverity error, do not remove */
#include <ecnt_hook/ecnt_hook_qdma_type.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/
#define qdma_lan_path   "/dev/qdma_lan"
#define qdma_wan_path   "/dev/qdma_wan"

/************************************************************************
*                  M A C R O S
************************************************************************/
#define ECNT_QDMAMGR_SANITY_CHECK_NULL(p)  \
    if (!p)\
    {\
        return ECNT_QDMAMGR_INVALID_PARAM;\
    }

#define ECNT_QDMAMGR_SANITY_CHECK_MAX(p, max)  \
    if ((p) >= (max))\
    {\
        return ECNT_QDMAMGR_INVALID_PARAM;\
    }

#define ECNT_QDMAMGR_SANITY_CHECK_RANGE(p, min, max)  \
    if (((p) < (min)) || ((p) > (max)))\
    {\
        return ECNT_QDMAMGR_INVALID_PARAM;\
    }
    
/************************************************************************
*                  D A T A   T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

static int qdmamgr_lib_ioctl(ECNT_QDMAMGR_QDMA_SEL qdma_sel, struct ECNT_QDMA_Data *qdma_data)
{
    int fd, ret;

    if ((qdma_sel != ECNT_QDMAMGR_QDMA_SEL_LAN) && (qdma_sel != ECNT_QDMAMGR_QDMA_SEL_WAN))
    {
        return -1;
    }

    if (qdma_sel == ECNT_QDMAMGR_QDMA_SEL_LAN)
    {
        fd = open(qdma_lan_path, O_RDWR);
    }
    else
    {
        fd = open(qdma_wan_path, O_RDWR);
    }
    
    if (fd < 0) 
    {
        printf("Open qdma pseudo device failed\n");
        return -1;
    }

    ret = ioctl(fd, IO_QDMA_CMD_OPT, qdma_data);
    close(fd);

    if(ret >= 0) 
    {
        return 0;
    } 
    else 
    {
        return -1;
    }
}

/**
@brief Set QDMA WRR configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] base WRR scheduling by packet or byte
@param[in] scale WRR scheduling scale
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_wrr_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRR_WEIGHT_BASE base, ECNT_QDMAMGR_WRR_WEIGHT_SCALE scale)
{
    int ret;
    QDMA_TxQosWeightScale_t weight_scale;
    struct ECNT_QDMA_Data qdma_data;

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
    if (ECNT_QDMAMGR_WRR_WEIGHT_SCALE_1B == scale)
    {
        weight_scale = QDMA_TXQOS_WEIGHT_SCALE_1B;
    }
    else if (ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B == scale)
    {
        weight_scale = QDMA_TXQOS_WEIGHT_SCALE_16B;
    }
#else
    if (ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B == scale)
    {
        weight_scale = QDMA_TXQOS_WEIGHT_SCALE_16B;
    }
    else if (ECNT_QDMAMGR_WRR_WEIGHT_SCALE_64B == scale)
    {
        weight_scale = QDMA_TXQOS_WEIGHT_SCALE_64B;
    }
#endif
    else
    {
        return ECNT_QDMAMGR_INVALID_PARAM;
    }

    qdma_data.function_id = QDMA_FUNCTION_SET_TX_QOS_WEIGHT;
    qdma_data.qdma_private.qdma_tx_qos.weightBase   = base;
    qdma_data.qdma_private.qdma_tx_qos.weightScale  = weight_scale;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA WRR configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_base WRR scheduling by packet or byte
@param[in] p_scale WRR scheduling scale
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_wrr_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRR_WEIGHT_BASE *p_base, ECNT_QDMAMGR_WRR_WEIGHT_SCALE *p_scale)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_GET_TX_QOS_WEIGHT;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_base = qdma_data.qdma_private.qdma_tx_qos.weightBase;
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
    if (QDMA_TXQOS_WEIGHT_SCALE_1B == qdma_data.qdma_private.qdma_tx_qos.weightScale)
    {
        *p_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_1B;
    }
    else if (QDMA_TXQOS_WEIGHT_SCALE_16B == qdma_data.qdma_private.qdma_tx_qos.weightScale)
    {
        *p_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B;
    }
#else
    if (QDMA_TXQOS_WEIGHT_SCALE_16B == qdma_data.qdma_private.qdma_tx_qos.weightScale)
    {
        *p_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_16B;
    }
    else if (QDMA_TXQOS_WEIGHT_SCALE_64B == qdma_data.qdma_private.qdma_tx_qos.weightScale)
    {
        *p_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_64B;
    }
#endif
    else
    {
        *p_scale = ECNT_QDMAMGR_WRR_WEIGHT_SCALE_MAX;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA channel qos
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[in] p_channel_qos channel qos configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_CHANNEL_QOS *p_channel_qos)
{
    int ret, i;
    QDMA_TxQosScheduler_T tx_qos = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_channel_qos);

    tx_qos.channel = channel;
    tx_qos.qosType = p_channel_qos->qos_mode;
    for (i = 0; i < ECNT_QDMAMGR_CHANNEL_QUEUE_NUM; i++)
    {
        tx_qos.queue[i].weight = p_channel_qos->queue_weight[i];
    }

    qdma_data.function_id = QDMA_FUNCTION_SET_TX_QOS;
    qdma_data.qdma_private.qdma_tx_qos.pTxQos = &tx_qos;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA channel qos
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[out] p_channel_qos channel qos configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_CHANNEL_QOS *p_channel_qos)
{
    int ret, i;
    QDMA_TxQosScheduler_T tx_qos = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_channel_qos);

    tx_qos.channel = channel;

    qdma_data.function_id = QDMA_FUNCTION_GET_TX_QOS;
    qdma_data.qdma_private.qdma_tx_qos.pTxQos = &tx_qos;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_channel_qos->qos_mode = tx_qos.qosType;
    for (i = 0; i < ECNT_QDMAMGR_CHANNEL_QUEUE_NUM; i++)
    {
        p_channel_qos->queue_weight[i] = tx_qos.queue[i].weight;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA ratectl global configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_cfg ratectl global configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATECTL_CFG *p_cfg)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmCfg_T cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);
    
    cfg.trtcmModule         = p_cfg->module;
    cfg.trtcmEn             = p_cfg->enable;
    cfg.trtcmMode           = p_cfg->mode;
    cfg.trtcmSlowTickRatio  = p_cfg->slow_tick_ratio;
    cfg.trtcmFastTick       = p_cfg->fast_tick; 

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_SET_TRTCM_CFG;
    qdma_data.qdma_private.generalTrtcmCfgPtr = &cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA ratectl global configuration
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_cfg ratectl global configuration
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATECTL_CFG *p_cfg)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmCfg_T cfg = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);
    cfg.trtcmModule         = p_cfg->module;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_GET_TRTCM_CFG;
    qdma_data.qdma_private.generalTrtcmCfgPtr = &cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_cfg->enable           = cfg.trtcmEn;
    p_cfg->mode             = cfg.trtcmMode;
    p_cfg->slow_tick_ratio  = cfg.trtcmSlowTickRatio;
    p_cfg->fast_tick        = cfg.trtcmFastTick;
    
    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA ratectl ratelimit mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_ratelimit_cfg ratectl ratelimit mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_CFG *p_ratelimit_cfg)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmRatelimitCfg_T ratelimit_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_ratelimit_cfg);

    ratelimit_cfg.trtcmModule   = p_ratelimit_cfg->module;
    ratelimit_cfg.Index         = p_ratelimit_cfg->index;
    ratelimit_cfg.MeterEn       = p_ratelimit_cfg->enable;
    ratelimit_cfg.PktMode       = p_ratelimit_cfg->rate_mode;
    ratelimit_cfg.TickSel       = p_ratelimit_cfg->tick_type;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_SET_RATELIMIT_MODE_CFG;
    qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr = &ratelimit_cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA ratectl ratelimit mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_ratelimit_cfg ratectl ratelimit mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_ratelimit_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_CFG *p_ratelimit_cfg)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmRatelimitCfg_T ratelimit_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_ratelimit_cfg);

    ratelimit_cfg.trtcmModule   = p_ratelimit_cfg->module;
    ratelimit_cfg.Index         = p_ratelimit_cfg->index;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_CFG;
    qdma_data.qdma_private.generalTrtcmRatelimitCfgPtr = &ratelimit_cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_ratelimit_cfg->enable     = ratelimit_cfg.MeterEn;
    p_ratelimit_cfg->rate_mode  = ratelimit_cfg.PktMode;
    p_ratelimit_cfg->tick_type  = ratelimit_cfg.TickSel;

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA ratectl ratelimit mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_ratelimit ratectl ratelimit mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_ratelimit_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_VALUE *p_ratelimit)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmRatelimitSet_T ratelimit_value = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_ratelimit);

    ratelimit_value.trtcmModule     = p_ratelimit->module;
    ratelimit_value.Index           = p_ratelimit->index;
    ratelimit_value.RateLimitValue  = p_ratelimit->rate;
    ratelimit_value.BucketSize      = p_ratelimit->bucket_size;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_SET_RATELIMIT_MODE_VALUE;
    qdma_data.qdma_private.generalTrtcmRatelimitSetPtr = &ratelimit_value;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA ratectl ratelimit mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_ratelimit ratectl ratelimit mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_ratelimit_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_VALUE *p_ratelimit)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmRatelimitSet_T ratelimit_value = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_ratelimit);

    ratelimit_value.trtcmModule = p_ratelimit->module;
    ratelimit_value.Index       = p_ratelimit->index;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_GET_RATELIMIT_MODE_VALUE;
    qdma_data.qdma_private.generalTrtcmRatelimitSetPtr = &ratelimit_value;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_ratelimit->rate           = ratelimit_value.RateLimitValue;
    p_ratelimit->bucket_size    = ratelimit_value.BucketSize;

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA ratectl ratelimit mode bucket size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_ratelimit ratectl ratelimit mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_ratelimit_bucket_size(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATELIMIT_MODE_VALUE *p_ratelimit)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmRatelimitSet_T ratelimit_value = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_ratelimit);

    ratelimit_value.trtcmModule     = p_ratelimit->module;
    ratelimit_value.Index           = p_ratelimit->index;
    ratelimit_value.RateLimitValue  = p_ratelimit->rate;
    ratelimit_value.BucketSize      = p_ratelimit->bucket_size;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_SET_RATELIMIT_BUCKET_SIZE;
    qdma_data.qdma_private.generalTrtcmRatelimitSetPtr = &ratelimit_value;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA ratectl trtcm mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_trtcm_cfg ratectl trtcm mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_trtcm_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_CFG *p_trtcm_cfg)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmCbsPbsCfg_T trtcm_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_trtcm_cfg);

    trtcm_cfg.trtcmModule  = p_trtcm_cfg->module;
    trtcm_cfg.Index        = p_trtcm_cfg->index;
    trtcm_cfg.CbsMeterEn   = p_trtcm_cfg->cbs_meter_en;
    trtcm_cfg.CbsPktMode   = p_trtcm_cfg->cbs_rate_mode;
    trtcm_cfg.CbsTickSel   = p_trtcm_cfg->cbs_tick_type;
    trtcm_cfg.PbsMeterEn   = p_trtcm_cfg->pbs_meter_en;
    trtcm_cfg.PbsPktMode   = p_trtcm_cfg->pbs_rate_mode;
    trtcm_cfg.PbsTickSel   = p_trtcm_cfg->pbs_tick_type;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_SET_TRTCM_MODE_CFG;
    qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr = &trtcm_cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA ratectl trtcm mode config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_trtcm_cfg ratectl trtcm mode config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_trtcm_mode_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_CFG *p_trtcm_cfg)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmCbsPbsCfg_T trtcm_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_trtcm_cfg);

    trtcm_cfg.trtcmModule  = p_trtcm_cfg->module;
    trtcm_cfg.Index        = p_trtcm_cfg->index;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_CFG;
    qdma_data.qdma_private.generalTrtcmCbsPbsCfgPtr = &trtcm_cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_trtcm_cfg->cbs_meter_en  = trtcm_cfg.CbsMeterEn;
    p_trtcm_cfg->cbs_rate_mode = trtcm_cfg.CbsPktMode;
    p_trtcm_cfg->cbs_tick_type = trtcm_cfg.CbsTickSel;
    p_trtcm_cfg->pbs_meter_en  = trtcm_cfg.PbsMeterEn;
    p_trtcm_cfg->pbs_rate_mode = trtcm_cfg.PbsPktMode;
    p_trtcm_cfg->pbs_tick_type = trtcm_cfg.PbsTickSel;
    
    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA ratectl trtcm mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_trtcm ratectl trtcm mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_trtcm_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_VALUE *p_trtcm)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmCbsPbsSet_T trtcm_value = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_trtcm);

    trtcm_value.trtcmModule = p_trtcm->module;
    trtcm_value.Index = p_trtcm->index;
    trtcm_value.CbsParamValue = p_trtcm->cbs;
    trtcm_value.CbsBucketSize = p_trtcm->cbs_bucket_size;
    trtcm_value.PbsParamValue = p_trtcm->pbs;
    trtcm_value.PbsBucketSize = p_trtcm->pbs_bucket_size;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_SET_TRTCM_MODE_VALUE;
    qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr = &trtcm_value;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA ratectl trtcm mode value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_trtcm ratectl trtcm mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_ratectl_trtcm_mode_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_VALUE *p_trtcm)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmCbsPbsSet_T trtcm_value = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_trtcm);

    trtcm_value.trtcmModule = p_trtcm->module;
    trtcm_value.Index = p_trtcm->index;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_GET_TRTCM_MODE_VALUE;
    qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr = &trtcm_value;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_trtcm->cbs = trtcm_value.CbsParamValue;
    p_trtcm->cbs_bucket_size = trtcm_value.CbsBucketSize;
    p_trtcm->pbs = trtcm_value.PbsParamValue;
    p_trtcm->pbs_bucket_size = trtcm_value.PbsBucketSize;

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA ratectl trtcm mode bucket size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_trtcm ratectl trtcm mode value
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_ratectl_trtcm_bucket_size(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TRTCM_MODE_VALUE *p_trtcm)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    GENERAL_TrtcmCbsPbsSet_T trtcm_value = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_trtcm);

    trtcm_value.trtcmModule = p_trtcm->module;
    trtcm_value.Index = p_trtcm->index;
    trtcm_value.CbsParamValue = p_trtcm->cbs;
    trtcm_value.CbsBucketSize = p_trtcm->cbs_bucket_size;
    trtcm_value.PbsParamValue = p_trtcm->pbs;
    trtcm_value.PbsBucketSize = p_trtcm->pbs_bucket_size;

    qdma_data.function_id = QDMA_FUNCTION_GENERAL_SET_TRTCM_BUCKET_SIZE;
    qdma_data.qdma_private.generalTrtcmCbsPbsSetPtr = &trtcm_value;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA multicast enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_multicast_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
    struct ECNT_QDMA_Data qdma_data;

    qdma_data.function_id = QDMA_FUNCTION_SET_MULTICAST_EN;
    qdma_data.qdma_private.mode = enable;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA multicast enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_multicast_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret = 0;
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);

	qdma_data.function_id = QDMA_FUNCTION_GET_MULTICAST_EN;
    
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}
    
	*p_enable = qdma_data.qdma_private.mode;
	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA flow cnt cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] grpIdx flowcnt group index
@param[in] incDropEn INC drop or not
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_flow_cntr_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 grpIdx, u8 incDropEn)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_FlowCntIncDropCfg_T flowCntIncDropCfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    qdma_data.function_id = QDMA_FUNCTION_SET_FLOW_CNTR_CFG;
    flowCntIncDropCfg.grpIdx = grpIdx;
	flowCntIncDropCfg.enable = incDropEn;
	qdma_data.qdma_private.flowCntIncDropCfg = &flowCntIncDropCfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA flow cnt cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] grpIdx flowcnt group index
@param[in] incDropEn INC drop or not
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_flow_cntr_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 grpIdx, u8 *p_incDropEn)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_FlowCntIncDropCfg_T flowCntIncDropCfg = {0};
    struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_incDropEn);

    qdma_data.function_id = QDMA_FUNCTION_GET_FLOW_CNTR_CFG;
    flowCntIncDropCfg.grpIdx = grpIdx;
	qdma_data.qdma_private.flowCntIncDropCfg = &flowCntIncDropCfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

	*p_incDropEn = flowCntIncDropCfg.enable;

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA flow cnt value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] cntValue flow cnt info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_flow_cntr_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_FLOW_CNT *p_cntValue)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_FlowCntValueGet_T flowCntValueGet = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cntValue);

	qdma_data.function_id = QDMA_FUNCTION_GET_FLOW_CNTR_VALUE;
	flowCntValueGet.grpIdx = p_cntValue->grpIdx;
	flowCntValueGet.cntIdx = p_cntValue->cntIdx;
	qdma_data.qdma_private.flowCntValueGet = &flowCntValueGet;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	p_cntValue->byteLow = flowCntValueGet.byteLow;
	p_cntValue->byteHigh = flowCntValueGet.byteHigh;
	p_cntValue->pktCnt = flowCntValueGet.pktCnt;
	p_cntValue->pktHigh = flowCntValueGet.pktHigh;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA flow cnt value clear
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] grpIdx group index
@param[in] cntIdx cnt index
@param[in] clearType byte/packet/all
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_clear_flow_cntr_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 grpIdx, u8 cntIdx, ECNT_QDMAMGR_FLOW_CNT_CLEAR_TYPE clearType)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_FlowCntValueClear_T flowCntValueClear = {0};
	struct ECNT_QDMA_Data qdma_data;

	qdma_data.function_id = QDMA_FUNCTION_CLEAR_FLOW_CNTR_VALUE;
	flowCntValueClear.grpIdx = grpIdx;
	flowCntValueClear.cntIdx = cntIdx;
	flowCntValueClear.clearMode = clearType;
	qdma_data.qdma_private.flowCntValueClear = &flowCntValueClear;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA tx wred mode
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable on/off
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_tx_wred_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_TxWredCfg_T txWredCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	txWredCfg.txWredEn = enable;
	qdma_data.function_id = QDMA_FUNCTION_SET_TX_WRED_MODE;
	qdma_data.qdma_private.txWredCfgPtr = &txWredCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA tx wred mode
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable on/off
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_tx_wred_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_TxWredCfg_T txWredCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);

	qdma_data.function_id = QDMA_FUNCTION_GET_TX_WRED_MODE;
	qdma_data.qdma_private.txWredCfgPtr = &txWredCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	*p_enable = txWredCfg.txWredEn;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA tx wred threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_txWred, packet type & threshold info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_tx_wred_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TX_WRED_CFG *p_txWred)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int i;
	int ret;
	QDMA_TxWredCfg_T txWredCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_txWred);

	txWredCfg.txWredPacketType = p_txWred->txWredPacketType;
	for(i = 0; i < TX_WRED_THR_NUM; i++)
	{
		txWredCfg.txWredThrValue[i] = p_txWred->txWredThrValue[i];
	}
	qdma_data.function_id = QDMA_FUNCTION_SET_TX_WRED_THRESHOLD;
	qdma_data.qdma_private.txWredCfgPtr = &txWredCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA tx wred probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_txWred, packet type & probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_tx_wred_probability(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TX_WRED_CFG *p_txWred)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int i;
	int ret;
	QDMA_TxWredCfg_T txWredCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_txWred);

	txWredCfg.txWredPacketType = p_txWred->txWredPacketType;
	for(i = 0; i < TX_WRED_PROBABILITY_NUM; i++)
	{
		txWredCfg.txWredThrProbability[i] = p_txWred->txWredThrProbability[i];
	}
	qdma_data.function_id = QDMA_FUNCTION_SET_TX_WRED_PROBABILITY;
	qdma_data.qdma_private.txWredCfgPtr = &txWredCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA tx wred threshold & probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_txWred, packet type & threshold & probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_tx_wred_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TX_WRED_CFG *p_txWred)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int i;
	int ret;
	QDMA_TxWredCfg_T txWredCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_txWred);

	txWredCfg.txWredPacketType = p_txWred->txWredPacketType;
	qdma_data.function_id = QDMA_FUNCTION_GET_TX_WRED_CFG;
	qdma_data.qdma_private.txWredCfgPtr = &txWredCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	for(i = 0; i < TX_WRED_THR_NUM; i++)
	{
		p_txWred->txWredThrValue[i] = txWredCfg.txWredThrValue[i];
	}

	for(i = 0; i < TX_WRED_PROBABILITY_NUM; i++)
	{
		p_txWred->txWredThrProbability[i] = txWredCfg.txWredThrProbability[i];
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA rx red probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] type, packet type, normal/DEI
@param[in] probability, probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_rx_red_probability(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRED_PACKET_Type type, unchar probability)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int i;
	int ret;
	QDMA_RxRedCfg_T rxRedCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	rxRedCfg.rxRedPacketType = type;
	rxRedCfg.rxRedThrProbability = probability;
	qdma_data.function_id = QDMA_FUNCTION_SET_CPU_RX_RED_PROBABILITY;
	qdma_data.qdma_private.rxRedCfgPtr = &rxRedCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA rx red probability
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] type, packet type, normal/DEI
@param[in] probability, probability info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_rx_red_probability(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_WRED_PACKET_Type type, unchar *p_probability)
{
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int i;
	int ret;
	QDMA_RxRedCfg_T rxRedCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_probability);

	rxRedCfg.rxRedPacketType = type;
	qdma_data.function_id = QDMA_FUNCTION_GET_CPU_RX_RED_PROBABILITY;
	qdma_data.qdma_private.rxRedCfgPtr = &rxRedCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	*p_probability = rxRedCfg.rxRedThrProbability;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA sla channel cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] sla index
@param[in] enable
@param[in] channel index
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_sla_channel_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 slaIdx, u8 enable, u8 chnlIdx)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret = 0;
	struct ECNT_QDMA_Data qdma_data;
	QDMA_SlaChnlCfg_T slaChnlCfg;

    qdma_data.function_id = QDMA_FUNCTION_SET_SLA_CHANNEL_CFG;
	slaChnlCfg.slaIdx = slaIdx;
	slaChnlCfg.slaEn = enable;
	slaChnlCfg.chnlIdx = chnlIdx;
    qdma_data.qdma_private.slaChnlCfgPtr = &slaChnlCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA sla channel cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] sla index
@param[in] enable
@param[in] channel index
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_sla_channel_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 slaIdx, u8 *p_enable, u8 *p_chnlIdx)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret = 0;
    struct ECNT_QDMA_Data qdma_data;
	QDMA_SlaChnlCfg_T slaChnlCfg;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);
	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_chnlIdx);

    qdma_data.function_id = QDMA_FUNCTION_GET_SLA_CHANNEL_CFG;
	slaChnlCfg.slaIdx = slaIdx;
    qdma_data.qdma_private.slaChnlCfgPtr = &slaChnlCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

	*p_enable = slaChnlCfg.slaEn;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA qos aging config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_qosAgingCfg aging config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_qos_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_QOS_AGING_CFG *p_qosAgingCfg)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
    QDMA_QoSAgingCfg_T qosAgingCfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_qosAgingCfg);

    qosAgingCfg.agingEn				= p_qosAgingCfg->agingEn;
    qosAgingCfg.agingMethod			= p_qosAgingCfg->agingMethod;
    qosAgingCfg.agingFastReplaceEn	= p_qosAgingCfg->agingFastReplaceEn;
    qosAgingCfg.agingTime			= p_qosAgingCfg->agingTime;

    qdma_data.function_id = QDMA_FUNCTION_SET_QOS_AGING_CFG;
    qdma_data.qdma_private.qosAgingCfgPtr = &qosAgingCfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA qos aging config
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_qosAgingCfg aging config
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_qos_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_QOS_AGING_CFG *p_qosAgingCfg)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_QoSAgingCfg_T qosAgingCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_qosAgingCfg);

	qdma_data.function_id = QDMA_FUNCTION_GET_QOS_AGING_CFG;
	qdma_data.qdma_private.qosAgingCfgPtr = &qosAgingCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	p_qosAgingCfg->agingEn = qosAgingCfg.agingEn;
	p_qosAgingCfg->agingMethod = qosAgingCfg.agingMethod;
	p_qosAgingCfg->agingFastReplaceEn = qosAgingCfg.agingFastReplaceEn;
	p_qosAgingCfg->agingTime = qosAgingCfg.agingTime;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA qos aging per-queue enable/disable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] queueIdx queue index
@param[in] agingEn enable/disable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_per_queue_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 queueIdx, u8 agingEn)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_TxQAgingCfg_T txqAgingCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	txqAgingCfg.chnlIdx = chnlIdx;
	txqAgingCfg.queueIdx= queueIdx;
	txqAgingCfg.queueAgingEn = agingEn;

	qdma_data.function_id = QDMA_FUNCTION_SET_PER_QUEUE_AGING_CFG;
	qdma_data.qdma_private.txqAgingCfgPtr = &txqAgingCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA qos aging per-queue enable/disable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] queueIdx queue index
@param[in] p_agingEn enable/disable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_per_queue_aging_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 queueIdx, u8 *p_agingEn)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_TxQAgingCfg_T txqAgingCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_agingEn);

	txqAgingCfg.chnlIdx = chnlIdx;
	txqAgingCfg.queueIdx= queueIdx;

	qdma_data.function_id = QDMA_FUNCTION_GET_PER_QUEUE_AGING_CFG;
	qdma_data.qdma_private.txqAgingCfgPtr = &txqAgingCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	*p_agingEn = txqAgingCfg.queueAgingEn;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA multicast sptag cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] sptagKeepHiEn keep sp_tag hi-bits or not
@param[in] sptag special tag info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_multicast_sptag_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 sptagKeepHiEn, u16 sptag)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_MulticastSptagCfg_T multicastSptagCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	multicastSptagCfg.chnlIdx = chnlIdx;
	multicastSptagCfg.sptagKeepHiEn = sptagKeepHiEn;
	multicastSptagCfg.sptag = sptag;

	qdma_data.function_id = QDMA_FUNCTION_SET_MULTICAST_SPTAG_CFG;
	qdma_data.qdma_private.multicastSptagCfgPtr = &multicastSptagCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA multicast sptag cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] sptagKeepHiEn keep sp_tag hi-bits or not
@param[in] sptag special tag info
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_multicast_sptag_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 *p_sptagKeepHiEn, u16 *p_sptag)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_MulticastSptagCfg_T multicastSptagCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_sptagKeepHiEn);
	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_sptag);

	multicastSptagCfg.chnlIdx = chnlIdx;

	qdma_data.function_id = QDMA_FUNCTION_GET_MULTICAST_SPTAG_CFG;
	qdma_data.qdma_private.multicastSptagCfgPtr = &multicastSptagCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	*p_sptagKeepHiEn = multicastSptagCfg.sptagKeepHiEn;
	*p_sptag = multicastSptagCfg.sptag;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA multicast force port cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] fport force port
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_multicast_fport_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u16 fport)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_MulticastFportCfg_T multicastFportCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	multicastFportCfg.chnlIdx = chnlIdx;
	multicastFportCfg.fport = fport;

	qdma_data.function_id = QDMA_FUNCTION_SET_MULTICAST_FPORT_CFG;
	qdma_data.qdma_private.multicastFportCfgPtr = &multicastFportCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA multicast force port cfg
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] fport force port
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_multicast_fport_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u16 *p_fport)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	QDMA_MulticastFportCfg_T multicastFportCfg = {0};
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_fport);

	multicastFportCfg.chnlIdx = chnlIdx;

	qdma_data.function_id = QDMA_FUNCTION_GET_MULTICAST_FPORT_CFG;
	qdma_data.qdma_private.multicastFportCfgPtr = &multicastFportCfg;

	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	*p_fport = multicastFportCfg.fport;

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA static cngst channel enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] chnlIdx channel index
@param[in] enable
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_channel_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 chnlIdx, u8 enable)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	QDMA_TxQStaticCfg_T staticThrhCfg = {0};

	qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_STATIC_CHANNEL_EN;
	staticThrhCfg.chnlIdx = chnlIdx;
	staticThrhCfg.staticThrEn = enable;
	qdma_data.qdma_private.staticThrhCfgPtr = &staticThrhCfg;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA static cngst queue ratio value
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_static_ratio, per-queue ratio
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_queue_ratio(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_STATIC_RATIO *p_static_ratio)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret, i;
	QDMA_TxQStaticThrRatio_T staticThrhRatio;
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_static_ratio);
	
	memset(&staticThrhRatio, 0, sizeof(QDMA_TxQStaticThrRatio_T));
	for( i = 0; i < ECNT_QDMAMGR_CHANNEL_QUEUE_NUM; i++){
		staticThrhRatio.queueThrRatio[i] = p_static_ratio->queueThrRatio[i];
	}
	qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_RATIO;
	qdma_data.qdma_private.staticThrhRatioPtr = &staticThrhRatio;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA static cngst queue cfg info
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] p_total_thr, total max/min threshold
@param[in] p_static_cfg, channel static threshold enable or not
@param[in] p_static_ratio, per-queue ratio
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_static_cngst_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel,
															ECNT_QDMAMGR_TOTAL_THR *p_total_thr, 
															ECNT_QDMAMGR_STATIC_CFG *p_static_cfg,
															ECNT_QDMAMGR_STATIC_RATIO *p_static_ratio)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
	int ret, i;
	QDMA_TxQDynCngstTotalThrh_T totalThrh;
	QDMA_TxQStaticCfg_T staticThrhCfg;
	QDMA_TxQStaticThrRatio_T staticThrhRatio;
	struct ECNT_QDMA_Data qdma_data;

	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_total_thr);
	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_static_cfg);
	ECNT_QDMAMGR_SANITY_CHECK_NULL(p_static_ratio);

	memset(&totalThrh, 0, sizeof(QDMA_TxQDynCngstTotalThrh_T));
	memset(&staticThrhCfg, 0, sizeof(QDMA_TxQStaticCfg_T));
	memset(&staticThrhRatio, 0, sizeof(QDMA_TxQStaticThrRatio_T));
	memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));

	staticThrhCfg.chnlIdx = p_static_cfg->chnlIdx;
	
	qdma_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_STATIC_INFO;
	qdma_data.qdma_private.staticCfgPtr->totalThrhPtr = &totalThrh;
	qdma_data.qdma_private.staticCfgPtr->staticThrhCfgPtr = &staticThrhCfg;
	qdma_data.qdma_private.staticCfgPtr->staticThrhRatioPtr = &staticThrhRatio;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	p_total_thr->totalMaxThrh = totalThrh.dynCngstTotalMaxThrh;
	p_total_thr->totalMinThrh = totalThrh.dynCngstTotalMinThrh;
	p_static_cfg->staticThrEn = staticThrhCfg.staticThrEn;
	for( i = 0; i < ECNT_QDMAMGR_CHANNEL_QUEUE_NUM; i++){
		p_static_ratio->queueThrRatio[i] = staticThrhRatio.queueThrRatio[i];
	}

	return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA egress rate meter time
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] div 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR div, u16 time_slice)
{
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    QDMA_TxRateMeter_T egs_ratemeter_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    egs_ratemeter_cfg.txRateMeterTimeDivisor = div;
    egs_ratemeter_cfg.txRateMeterTimeSlice = time_slice;    

    qdma_data.function_id = QDMA_FUNCTION_SET_TX_RATEMETER;
    qdma_data.qdma_private.txRateMeterPtr = &egs_ratemeter_cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA egress rate meter time
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] div 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_egress_ratemeter_time_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATEMETER_TIME_DIVISOR *p_div, u16 *p_time_slice)
{
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    QDMA_TxRateMeter_T egs_ratemeter_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_div);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_time_slice);
    
    qdma_data.function_id = QDMA_FUNCTION_GET_TX_RATEMETER;
    qdma_data.qdma_private.txRateMeterPtr = &egs_ratemeter_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_div          = egs_ratemeter_cfg.txRateMeterTimeDivisor;
    *p_time_slice   = egs_ratemeter_cfg.txRateMeterTimeSlice;

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Set QDMA egress rate meter time window size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] windowSize 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 windowSize, u16 timeSlice)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    QDMA_TxRateMeter_T egs_ratemeter_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    egs_ratemeter_cfg.txRateMeterWindowSize = windowSize;
    egs_ratemeter_cfg.txRateMeterTimeSlice = timeSlice;    

    qdma_data.function_id = QDMA_FUNCTION_SET_TX_RATEMETER;
    qdma_data.qdma_private.txRateMeterPtr = &egs_ratemeter_cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

/**
@brief Get QDMA egress rate meter time window size
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] windowSize 
@param[in] time_slice
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7523, EN7581
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_egress_ratemeter_window_cfg(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_windowSize, u16 *p_timeSlice)
{
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
	return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    QDMA_TxRateMeter_T egs_ratemeter_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_windowSize);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_timeSlice);
    
    qdma_data.function_id = QDMA_FUNCTION_GET_TX_RATEMETER;
    qdma_data.qdma_private.txRateMeterPtr = &egs_ratemeter_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_windowSize  = egs_ratemeter_cfg.txRateMeterWindowSize;
    *p_timeSlice   = egs_ratemeter_cfg.txRateMeterTimeSlice;

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATELIMIT_CFG *p_cfg)
{   
    int ret, i;
    QDMA_TxRateLimitCfg_T egs_ratemeter_cfg;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);

    memset(&egs_ratemeter_cfg, 0, sizeof(egs_ratemeter_cfg));
    for (i = 0; i < ECNT_QDMAMGR_PER_CHANNEL_TICK_SEL_NUM; i++)
    {
        egs_ratemeter_cfg.txRateLimitUnit[i] = p_cfg->unit[i];
    }
    egs_ratemeter_cfg.txRateLimitBucketScale = p_cfg->scale;

    qdma_data.function_id = QDMA_FUNCTION_SET_TX_RATELIMIT_CFG;
    qdma_data.qdma_private.txRateLimitCfgPtr = &egs_ratemeter_cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_egress_ratelimit_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_EGS_RATELIMIT_CFG *p_cfg)
{   
    int ret, i;
    QDMA_TxRateLimitCfg_T egs_ratemeter_cfg;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);

    qdma_data.function_id = QDMA_FUNCTION_GET_TX_RATELIMIT_CFG;
    qdma_data.qdma_private.txRateLimitCfgPtr = &egs_ratemeter_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    memset(&egs_ratemeter_cfg, 0, sizeof(egs_ratemeter_cfg));
    for (i = 0; i < ECNT_QDMAMGR_PER_CHANNEL_TICK_SEL_NUM; i++)
    {
        p_cfg->unit[i] = egs_ratemeter_cfg.txRateLimitUnit[i];
    }
    p_cfg->scale = egs_ratemeter_cfg.txRateLimitBucketScale;
    
    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA tx ratelimit
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[in] enable enable/disable rx ratelimit
@param[in] rate limit rate
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_tx_ratelimit_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, u8 enable, u32 rate)
{   
    int ret;
    QDMA_TxRateLimitSet_T tx_ratelimit = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    tx_ratelimit.chnlIdx            = channel;
    tx_ratelimit.chnlRateLimitEn    = enable;
    tx_ratelimit.rateLimitValue     = rate;

    qdma_data.function_id = QDMA_FUNCTION_SET_TX_RATELIMIT;
    qdma_data.qdma_private.txRateLimitSetPtr = &tx_ratelimit;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA tx ratelimit
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[out] p_enable enable/disable rx ratelimit
@param[out] p_rate limit rate
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_tx_ratelimit_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, u8 *p_enable, u32 *p_rate)
{   
    int ret;
    QDMA_TxRateLimitGet_T tx_ratelimit = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_rate);
    
    tx_ratelimit.chnlIdx            = channel;

    qdma_data.function_id = QDMA_FUNCTION_GET_TX_RATELIMIT;
    qdma_data.qdma_private.txRateLimitGetPtr = &tx_ratelimit;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_enable   = tx_ratelimit.chnlRateLimitEn;
    *p_rate     = tx_ratelimit.rateLimitValue;
    
    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA rx ratelimit enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable enable/disable QDMA rx ratelimit
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_rx_ratelimit_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_RX_RATELIMIT_EN;
    qdma_data.qdma_private.mode = enable;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA rx ratelimit enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable enable/disable QDMA rx ratelimit
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_rx_ratelimit_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable)
{
    int ret;
    QDMA_RxRateLimitCfg_T rx_ratelimit_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);
    
    qdma_data.function_id = QDMA_FUNCTION_GET_RX_RATELIMIT_CFG;
    qdma_data.qdma_private.rxRateLimitCfgPtr = &rx_ratelimit_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_enable   = rx_ratelimit_cfg.rxRateLimitEn;
    
    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA rx ratelimit packet mode
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] mode ratelimit by packet/byte
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_rx_ratelimit_pkt_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATECTL_MODE mode)
{   
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_RX_RATELIMIT_PKT_MODE;
    qdma_data.qdma_private.pktMode = mode;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA rx ratelimit packet mode
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_mode ratelimit by packet/byte
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_rx_ratelimit_pkt_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RATECTL_MODE *p_mode)
{   
    int ret;
    QDMA_RxRateLimitCfg_T rx_ratelimit_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_mode);
    
    qdma_data.function_id = QDMA_FUNCTION_GET_RX_RATELIMIT_CFG;
    qdma_data.qdma_private.rxRateLimitCfgPtr = &rx_ratelimit_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_mode   = rx_ratelimit_cfg.rxRateLimitPktMode;
    
    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA rx ring ratelimit
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] ring_index rx ring index
@param[in] rate limit rate
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_rx_ratelimit_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 ring_index, u32 rate)
{   
    int ret;
    QDMA_RxRateLimitSet_T rx_ratelimit = {0};
    struct ECNT_QDMA_Data qdma_data;

    rx_ratelimit.ringIdx        = ring_index;
    rx_ratelimit.rateLimitValue = rate;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_RX_RATELIMIT;
    qdma_data.qdma_private.rxRateLimitSetPtr = &rx_ratelimit;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA rx ring ratelimit
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] ring_index rx ring index
@param[out] p_rate limit rate
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_rx_ratelimit_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 ring_index, u32 *p_rate)
{   
    int ret;
    QDMA_RxRateLimitGet_T rx_ratelimit = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_rate);

    rx_ratelimit.ringIdx        = ring_index;
    
    qdma_data.function_id = QDMA_FUNCTION_GET_RX_RATELIMIT;
    qdma_data.qdma_private.rxRateLimitGetPtr = &rx_ratelimit;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_rate = rx_ratelimit.rateLimitValue;
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_virtual_channel_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable, ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE mode)
{   
    int ret;
    QDMA_VirtualChannelMode_T cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    cfg.virChnlEn = enable;
    cfg.virChnlMode = mode;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_MODE;
    qdma_data.qdma_private.virChnlModePtr = &cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_virtual_channel_config(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable, ECNT_QDMAMGR_VIRTUAL_CHANNEL_MODE *p_mode)
{   
    int ret;
    QDMA_VirtualChannelMode_T cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_mode);
   
    qdma_data.function_id = QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_MODE;
    qdma_data.qdma_private.virChnlModePtr = &cfg;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_enable = cfg.virChnlEn;
    *p_mode = cfg.virChnlMode;

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_virtual_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 phy_channel, ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS *p_qos)
{   
    int ret, i;
    QDMA_VirtualChannelQoS_T qos;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_qos);

    qos.phyChnl = phy_channel;
    qos.qosType = p_qos->qos_mode;
    for (i = 0; i < ECNT_QDMAMGR_VIRTUAL_CHANNEL_QUEUE_NUM; i++)
    {
        qos.queue[i].weight = p_qos->queue_weight[i];
    }

    qdma_data.function_id = QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_QOS;
    qdma_data.qdma_private.virChnlQoSPtr = &qos;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_virtual_channel_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 phy_channel, ECNT_QDMAMGR_VIRTUAL_CHANNEL_QOS *p_qos)
{   
    int ret, i;
    QDMA_VirtualChannelQoS_T qos;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_qos);

    qos.phyChnl = phy_channel;

    qdma_data.function_id = QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_QOS;
    qdma_data.qdma_private.virChnlQoSPtr = &qos;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_qos->qos_mode = qos.phyChnl;
    for (i = 0; i < ECNT_QDMAMGR_VIRTUAL_CHANNEL_QUEUE_NUM; i++)
    {
        p_qos->queue_weight[i] = qos.queue[i].weight;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_rx_ring_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD *p_threshold)
{   
    int ret;
    QDMA_RxLowThreshold_T rx_threshold;
    struct ECNT_QDMA_Data qdma_data;
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
    int i;
#endif

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_threshold);

    memset(&rx_threshold, 0, sizeof(rx_threshold));
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
    for (i = 0; i < ECNT_QDMAMGR_MAX_RX_RING_NUM; i++)
    {
        rx_threshold.rxRingLowThrh[i] = p_threshold->threshold[i];
    }
#else
    rx_threshold.rxRing0LowThrh = p_threshold->threshold[0];
    rx_threshold.rxRing1LowThrh = p_threshold->threshold[1];
#endif
    qdma_data.function_id = QDMA_FUNCTION_SET_RX_LOW_THRESHOLD;
    qdma_data.qdma_private.rxLowThresholdPtr = &rx_threshold;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_rx_ring_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_RX_RING_LOW_THRESHOLD *p_threshold)
{   
    int ret;
    QDMA_RxLowThreshold_T rx_threshold;
    struct ECNT_QDMA_Data qdma_data;
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
    int i;
#endif

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_threshold);

    qdma_data.function_id = QDMA_FUNCTION_GET_RX_LOW_THRESHOLD;
    qdma_data.qdma_private.rxLowThresholdPtr = &rx_threshold;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    memset(&rx_threshold, 0, sizeof(rx_threshold));
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
    for (i = 0; i < ECNT_QDMAMGR_MAX_RX_RING_NUM; i++)
    {
        p_threshold->threshold[i] = rx_threshold.rxRingLowThrh[i];
    }
#else
    p_threshold->threshold[0] = rx_threshold.rxRing0LowThrh;
    p_threshold->threshold[1] = rx_threshold.rxRing1LowThrh;
#endif

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA DEI drop enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable enable/disable DEI drop
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_dei_drop_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable)
{   
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_DEI_DROP_MODE;
    qdma_data.qdma_private.mode = enable ;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA DEI drop enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable enable/disable DEI drop
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_dei_drop_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable)
{   
    int ret = 0;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);

    qdma_data.function_id = QDMA_FUNCTION_GET_TXQ_DEI_DROP_MODE;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }
    
    *p_enable = qdma_data.qdma_private.mode;
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 threshold)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_LMGR_LOW_THRESHOLD;
    qdma_data.qdma_private.threshold = threshold ;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_lmgr_low_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 *p_threshold)
{
    int ret = 0 ;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_threshold);

    qdma_data.function_id = QDMA_FUNCTION_GET_LMGR_LOW_THRESHOLD;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_threshold = qdma_data.qdma_private.threshold;
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_overhead_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_PKTSIZE_OVERHEAD_EN;
    qdma_data.qdma_private.mode = enable ;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_overhead_en(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable)
{
    int ret = 0 ;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);

    qdma_data.function_id = QDMA_FUNCTION_GET_PKTSIZE_OVERHEAD_EN;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }
    
    *p_enable = qdma_data.qdma_private.mode;
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_overhead_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 cnt)
{
    int ret = 0 ;
    struct ECNT_QDMA_Data qdma_data;

    qdma_data.function_id = QDMA_FUNCTION_SET_PKTSIZE_OVERHEAD_VALUE;
    qdma_data.qdma_private.cnt = cnt ;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_overhead_value(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_cnt)
{
    int ret = 0;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cnt);

    qdma_data.function_id = QDMA_FUNCTION_GET_PKTSIZE_OVERHEAD_VALUE;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }
    
    *p_cnt = (u8)qdma_data.qdma_private.cnt;
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 channel_threshold, u32 queue_threshold)
{
    int ret;
    QDMA_MacLimitThrh_T threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    threshold.chnlThrh = channel_threshold;
    threshold.queueThrh = queue_threshold;    

    qdma_data.function_id = QDMA_FUNCTION_SET_MAC_LIMIT_THRESHOLD;
    qdma_data.qdma_private.qdma_mac_limit_thrh = &threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_mac_resource_limit(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u32 *p_channel_threshold, u32 *p_queue_threshold)
{
    int ret;
    QDMA_MacLimitThrh_T threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_channel_threshold);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_queue_threshold);

    qdma_data.function_id = QDMA_FUNCTION_GET_MAC_LIMIT_THRESHOLD;
    qdma_data.qdma_private.qdma_mac_limit_thrh = &threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_channel_threshold = threshold.chnlThrh;
    *p_queue_threshold = threshold.queueThrh;
    
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable, u16 channel_threshold, u16 total_threshold)
{
    int ret;
    QDMA_TxBufCtrl_T cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    cfg.mode = enable;
    cfg.chnThreshold = channel_threshold;
    cfg.totalThreshold = total_threshold;    

    qdma_data.function_id = QDMA_FUNCTION_SET_TXBUF_THRESHOLD;
    qdma_data.qdma_private.pQdmaTxBufCtrl = &cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_txbuf_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable, u16 *p_channel_threshold, u16 *p_total_threshold)
{
    int ret;
    QDMA_TxBufCtrl_T cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_channel_threshold);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_total_threshold);

    qdma_data.function_id = QDMA_FUNCTION_GET_TXBUF_THRESHOLD;
    qdma_data.qdma_private.pQdmaTxBufCtrl = &cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_enable = cfg.mode;
    *p_channel_threshold = cfg.chnThreshold;
    *p_total_threshold = cfg.totalThreshold;

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_prefetch(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 pre_fetch_en, u8 overdrag_en, u32 overdrag_cnt)
{
    int ret;
    QDMA_PrefetchMode_T cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    cfg.prefecthMode = pre_fetch_en ? QDMA_ENABLE : QDMA_DISABLE;
    cfg.overDragMode = overdrag_en ? QDMA_ENABLE : QDMA_DISABLE;
    cfg.overDragCnt  = overdrag_cnt;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_PREFETCH_MODE;
    qdma_data.qdma_private.qdma_prefetch = &cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA tx DBA report
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[in] p_dba_report dba report
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_dba_report(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_DBA_REPORT *p_dba_report)
{
    int ret;
    QDMA_TxDbaReport_T dba = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_dba_report);

    dba.channel = channel;
    dba.cirParamValue = p_dba_report->cir;
    dba.cbsParamValue = p_dba_report->cbs;
    dba.pirParamValue = p_dba_report->pir;
    dba.pbsParamValue = p_dba_report->pbs;

    qdma_data.function_id = QDMA_FUNCTION_SET_TX_DBAREPORT;
    qdma_data.qdma_private.txDbaReportPtr = &dba;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA tx DBA report
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] channel channel id
@param[out] p_dba_report dba report
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_dba_report(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 channel, ECNT_QDMAMGR_DBA_REPORT *p_dba_report)
{
    int ret;
    QDMA_TxDbaReport_T dba = {0};
    struct ECNT_QDMA_Data qdma_data;
    
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_dba_report);

    dba.channel = channel;

    qdma_data.function_id = QDMA_FUNCTION_GET_TX_DBAREPORT;
    qdma_data.qdma_private.txDbaReportPtr = &dba;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_dba_report->cir = dba.cirParamValue;
    p_dba_report->cbs = dba.cbsParamValue;
    p_dba_report->pir = dba.pirParamValue;
    p_dba_report->pbs = dba.pbsParamValue;

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA txq dynamic congestion enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable enable/disable QDMA txq dynamic congestion
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_MODE;
    qdma_data.qdma_private.mode = enable;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA txq dynamic congestion enable
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable enable/disable QDMA txq dynamic congestion
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_mode(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_enable);

    qdma_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_MODE;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_enable = qdma_data.qdma_private.mode;
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_dei_scale(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_DEI_THR_SCALE scale)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_DEI_THRH_SCALE;
    qdma_data.qdma_private.threshold = scale;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_dei_scale(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_DEI_THR_SCALE *p_scale)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_scale);

    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_DEI_THRH_SCALE;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_scale = qdma_data.qdma_private.threshold;
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_autoconfig(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST *p_tx_cngst)
{
    int ret;
    QDMA_txCngstCfg_t cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_tx_cngst);

    cfg.txCngstMode = p_tx_cngst->mode;
    cfg.peekRateMargin = p_tx_cngst->peak_rate_margin;
    cfg.peekRateDuration = p_tx_cngst->peak_rate_duration;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_AUTO_CONFIG;
    qdma_data.qdma_private.pTxCngstCfg = &cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_autoconfig(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST *p_tx_cngst)
{
    int ret;
    QDMA_txCngstCfg_t cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_tx_cngst);
    
    qdma_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_AUTO_CONFIG;
    qdma_data.qdma_private.pTxCngstCfg = &cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }
    
    p_tx_cngst->mode = cfg.txCngstMode;
    p_tx_cngst->peak_rate_margin = cfg.peekRateMargin;
    p_tx_cngst->peak_rate_duration = cfg.peekRateDuration;

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA congestion total threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] max_threshold maximum threshold
@param[in] min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_total_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 max_threshold, u16 min_threshold)
{
    int ret;
    QDMA_TxQDynCngstTotalThrh_T total_threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    total_threshold.dynCngstTotalMaxThrh = max_threshold;
    total_threshold.dynCngstTotalMinThrh = min_threshold;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_TOTAL_THRESHOLD;
    qdma_data.qdma_private.totalThrhPtr = &total_threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA congestion total threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_max_threshold maximum threshold
@param[out] p_min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_total_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_max_threshold, u16 *p_min_threshold)
{
    int ret;
    QDMA_TxQDynCngstTotalThrh_T total_threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_max_threshold);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_min_threshold);
    
    qdma_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_TOTAL_THRESHOLD;
    qdma_data.qdma_private.totalThrhPtr = &total_threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_max_threshold = total_threshold.dynCngstTotalMaxThrh;
    *p_min_threshold = total_threshold.dynCngstTotalMinThrh;
    
    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA congestion channel threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] max_threshold maximum threshold
@param[in] min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_channel_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 max_threshold, u16 min_threshold)
{
    int ret;
    QDMA_TxQDynCngstChnlThrh_T channel_threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    channel_threshold.dynCngstChnlMaxThrh = max_threshold;
    channel_threshold.dynCngstChnlMinThrh = min_threshold;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_CHANNEL_THRESHOLD;
    qdma_data.qdma_private.chnlThrhPtr = &channel_threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0) 
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA congestion channel threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_max_threshold maximum threshold
@param[out] p_min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_channel_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_max_threshold, u16 *p_min_threshold)
{
    int ret;
    QDMA_TxQDynCngstChnlThrh_T channel_threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_max_threshold);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_min_threshold);

    qdma_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_CHANNEL_THRESHOLD;
    qdma_data.qdma_private.chnlThrhPtr = &channel_threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0) 
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_max_threshold = channel_threshold.dynCngstChnlMaxThrh;
    *p_min_threshold = channel_threshold.dynCngstChnlMinThrh;

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA congestion queue threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] max_threshold maximum threshold
@param[in] min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_queue_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 max_threshold, u16 min_threshold)
{
    int ret;
    QDMA_TxQDynCngstQueueThrh_T queue_threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    queue_threshold.dynCngstQueueMaxThrh= max_threshold;
    queue_threshold.dynCngstQueueMinThrh= min_threshold;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_CNGST_QUEUE_THRESHOLD;
    qdma_data.qdma_private.queueThrhPtr = &queue_threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA congestion queue threshold
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_max_threshold maximum threshold
@param[out] p_min_threshold minimum threshold
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_queue_threshold(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u16 *p_max_threshold, u16 *p_min_threshold)
{
    int ret;
    QDMA_TxQDynCngstQueueThrh_T queue_threshold = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_max_threshold);
    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_min_threshold);

    qdma_data.function_id = QDMA_FUNCTION_GET_TXQ_CNGST_QUEUE_THRESHOLD;
    qdma_data.qdma_private.queueThrhPtr = &queue_threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    *p_max_threshold = queue_threshold.dynCngstQueueMaxThrh;
    *p_min_threshold = queue_threshold.dynCngstQueueMinThrh;

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_txq_cngst_peekrate(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST_PEAK_RATE_CFG *p_cfg)
{
    int ret;
    QDMA_PeekRateCfg_t peak_rate_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);

    peak_rate_cfg.peekRateEn = p_cfg->enable;
    peak_rate_cfg.peekRateMargin = p_cfg->peak_rate_margin;
    peak_rate_cfg.peekRateDuration = p_cfg->peak_rate_duration;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_PEEKRATE_PARAMS;
    qdma_data.qdma_private.peekrateCfgPtr = &peak_rate_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst_peekrate(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST_PEAK_RATE_CFG *p_cfg)
{
    int ret;
    QDMA_PeekRateCfg_t peak_rate_cfg = {0};
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);

    qdma_data.function_id = QDMA_FUNCTION_GET_PEEKRATE_PARAMS;
    qdma_data.qdma_private.peekrateCfgPtr = &peak_rate_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    p_cfg->enable = peak_rate_cfg.peekRateEn;
    p_cfg->peak_rate_margin = peak_rate_cfg.peekRateMargin;
    p_cfg->peak_rate_duration = peak_rate_cfg.peekRateDuration;
    
    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_normal(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_NORMAL_THRESHOLD *p_cfg)
{
    int ret, i;
    QDMA_TxQStaticNormalCfg_T normal_threshold;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);

    memset(&normal_threshold, 0, sizeof(normal_threshold));
    for (i = 0; i < ECNT_QDMAMGR_CHANNEL_QUEUE_NUM; i++)
    {
        normal_threshold.normalThrh[i] = p_cfg->threshold[i];
    }
        
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_NORMAL_THRESHOLD;
    qdma_data.qdma_private.normThrhPtr = &normal_threshold;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_static_cngst_dei(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_TXCNGST_STATIC_QUEUE_DEI_THRESHOLD *p_cfg)
{
    int ret, i;
    QDMA_TxQStaticDeiCfg_T dei_cfg;
    struct ECNT_QDMA_Data qdma_data;

    ECNT_QDMAMGR_SANITY_CHECK_NULL(p_cfg);

    memset(&dei_cfg, 0, sizeof(dei_cfg));
    for (i = 0; i < ECNT_QDMAMGR_CHANNEL_QUEUE_NUM; i++)
    {
        dei_cfg.deiThrh[i] = p_cfg->threshold[i];
    }
    
    qdma_data.function_id = QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_DEI_THRESHOLD;
    qdma_data.qdma_private.deiThrhPtr = &dei_cfg;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/*******************************************************************************/
/*  Internal Debug APIs                                                                                                        */
/*******************************************************************************/
ECNT_QDMAMGR_RET qdmamgr_lib_set_dbgcntr_channel(ECNT_QDMAMGR_QDMA_SEL qdma_sel, ECNT_QDMAMGR_DEBUG_CHANNEL_TYPE type)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_DBGCNTR_CHANNEL;
    qdma_data.qdma_private.cntrSrc = type;
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_dbgcntr_queue(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_DBGCNTR_QUEUE;
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_dbgcntr_ring(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
#if !defined(TCSUPPORT_CPU_EN7527) && !defined(TCSUPPORT_CPU_EN7516)
    return ECNT_QDMAMGR_NOT_SUPPORT;
#else
    int ret;
    struct ECNT_QDMA_Data qdma_data;


    qdma_data.function_id = QDMA_FUNCTION_SET_DBGCNTR_RING;
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
#endif
}

ECNT_QDMAMGR_RET qdmamgr_lib_clear_dbgcntr(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_CLEAR_DBGCNTR;
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_dbgcntr(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data; 

    qdma_data.function_id = QDMA_FUNCTION_DUMP_DBGCNTR;
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_clear_cpu_counter(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_CLEAR_CPU_COUNTER;
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_cpu_counter(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_DUMP_CPU_COUNTER;
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_register(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
    	int ret;
	struct ECNT_QDMA_Data qdma_data;

	qdma_data.function_id = QDMA_FUNCTION_DUMP_REGISTER_INFO;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_descriptor(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	
	qdma_data.function_id = QDMA_FUNCTION_DUMP_DESCRIPTOR_INFO;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_irq(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;

	qdma_data.function_id = QDMA_FUNCTION_DUMP_IRQ_INFO;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_hwfwd(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	
	qdma_data.function_id = QDMA_FUNCTION_DUMP_HWFWD_INFO;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_all(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;

	qdma_data.function_id = QDMA_FUNCTION_DUMP_INFO_ALL;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_test_dbglevel(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint level)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	
	qdma_data.function_id = QDMA_FUNCTION_SET_DEBUG_LEVEL;
	qdma_data.qdma_private.qdma_old_proc.dbgLevel = level ;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_test_dma_busy(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint busyDuration)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	
	qdma_data.function_id = QDMA_FUNCTION_DUMP_DMA_BUSY;
	qdma_data.qdma_private.qdma_old_proc.busyDuration = busyDuration ;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_test_reg_polling(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint regOffset, uint pollingDuration)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;

	qdma_data.function_id = QDMA_FUNCTION_DUMP_REG_POLLING;
	qdma_data.qdma_private.qdma_old_proc.regOffset = regOffset ;
	qdma_data.qdma_private.qdma_old_proc.pollingDuration = pollingDuration ;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_set_test_force_rx1(ECNT_QDMAMGR_QDMA_SEL qdma_sel,uint forceEn)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	
	qdma_data.function_id = QDMA_FUNCTION_SET_FORCE_RX_RING1;
	qdma_data.qdma_private.qdma_old_proc.forceEn = forceEn ;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_txq_cngst(ECNT_QDMAMGR_QDMA_SEL qdma_sel)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	
	qdma_data.function_id = QDMA_FUNCTION_DUMP_TXQ_CNGST;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_get_lmgr_status(ECNT_QDMAMGR_QDMA_SEL qdma_sel,ECNT_QDMAMGR_LMGR_STATUS *lmgrPtr)
{
	int ret;
	struct ECNT_QDMA_Data qdma_data;	
	QDMA_LmgrStatus_T lmgr_status;

	memset(&lmgr_status,0,sizeof(QDMA_LmgrStatus_T));

	lmgr_status.freeLmgr = lmgrPtr->freeLmgr;
	lmgr_status.usedBuf = lmgrPtr->usedBuf;
	lmgr_status.usedLmgr = lmgrPtr->usedLmgr;
	
	qdma_data.function_id = QDMA_FUNCTION_GET_LMGR_STATUS;
	qdma_data.qdma_private.qdma_lmgr_status = &lmgr_status;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

ECNT_QDMAMGR_RET qdmamgr_lib_dump_dbareport(ECNT_QDMAMGR_QDMA_SEL qdma_sel,int channel)
{	
	int ret;
	struct ECNT_QDMA_Data qdma_data;
	
	qdma_data.function_id = QDMA_FUNCTION_DUMP_TX_DBA_REPORT;
	qdma_data.qdma_private.channel = channel;
	ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
	if (ret < 0)
	{
		return ECNT_QDMAMGR_FAIL;
	}

	return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Set QDMA DownStream_Qos 
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[in] enable enable/disable DownStream_Qos
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7529 EN7581 AN7552 AN7583
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_set_downstream_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 enable)
{   
    int ret;
    struct ECNT_QDMA_Data qdma_data;
    
    qdma_data.function_id = QDMA_FUNCTION_SET_DOWNSTREAM_QOS;
    qdma_data.qdma_private.mode = enable ;

    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }

    return ECNT_QDMAMGR_SUCCESS;
}

/**
@brief Get QDMA DownStream_Qos 
@param[in] qdma_sel indicate QDMA_LAN/QDMA_WAN
@param[out] p_enable enable/disable DownStream_Qos
@return ::ECNT_QDMAMGR_RET
@par Supported Chip:
@code
EN7529 EN7581 AN7552 AN7583
@endcode
*/
ECNT_QDMAMGR_RET qdmamgr_lib_get_downstream_qos(ECNT_QDMAMGR_QDMA_SEL qdma_sel, u8 *p_enable)
{   
    int ret = 0;
    struct ECNT_QDMA_Data qdma_data;

    qdma_data.function_id = QDMA_FUNCTION_GET_DOWNSTREAM_QOS;
    
    ret = qdmamgr_lib_ioctl(qdma_sel, &qdma_data);
    if (ret < 0)
    {
        return ECNT_QDMAMGR_FAIL;
    }
    
    *p_enable = qdma_data.qdma_private.mode;
    return ECNT_QDMAMGR_SUCCESS;
}

