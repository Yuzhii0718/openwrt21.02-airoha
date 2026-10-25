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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <modules/ra_hwnat/hwnat_ioctl.h>
#include "libapi_lib_ppe.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

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
int do_ppe_ioctl(void *opt, unsigned long command)
{
    int fd, retVal = 1;
    struct ppe_tls_cfg *tlsCfg = opt;
    struct ppe_meter_cfg *meterCfg = opt;
    struct ppe_account_cfg *accountCfg = opt;
    struct ppe_cache_cfg *cacheCfg = opt;
    struct ppe_hash_cfg *hashCfg = opt;
    struct ppe_local_in_cfg *localInCfg = opt;
	struct black_ip_port_portocol *blackIpPort = opt;

    fd = open("/dev/"HW_NAT_DEVNAME, O_RDONLY);
    if (fd < 0)
    {
        printf("Open %s pseudo device failed\n","/dev/"HW_NAT_DEVNAME);
        return 1;
    }

    if(ioctl(fd, command, opt)<0)
    {
        printf("PPE_API: ioctl error\n");
        close(fd);
        return 1;
    }

    close(fd);

    if (command == PPE_COMMAND_TLS)
        retVal = tlsCfg->retVal;
    else if (command == PPE_COMMAND_METER || command == PPE_COMMAND_TRTCM)
        retVal = meterCfg->retVal;
    else if (command == PPE_COMMAND_ACCOUNT)
        retVal = accountCfg->retVal;
    else if (command == PPE_COMMAND_CACHE)
        retVal = cacheCfg->retVal;
    else if (command == PPE_COMMAND_HASH)
        retVal = hashCfg->retVal;
    else if (command == PPE_COMMAND_LOCAL_IN)
        retVal = localInCfg->retVal;
	else if (command == PPE_COMMAND_BLACK_IP_PROTOCOL_PORT)
		retVal = blackIpPort->retVal;
    else 
        retVal = 0;      

    return retVal;
}

/**
@brief Add PPE transparent line service
@param[in] p_tls TLS configuration
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_transparet_line_service(ECNT_PPEMGR_TLS_ENTRY *p_tls)
{
    int i;
    struct ppe_tls_cfg tls_cfg = {0};

    /* TODO: sanity check */
    
    tls_cfg.method = PPE_METHOD_ADD;
    tls_cfg.uni = p_tls->sport;
    tls_cfg.mode = p_tls->mode;
    if (tls_cfg.mode == TLS_MODE_CTAG_NON)
    {
        tls_cfg.cm = TLS_CONCATENATE_AND;
    }
    else
    {
        tls_cfg.cm = TLS_CONCATENATE_OR;
    }
    tls_cfg.ma = p_tls->miss_action;

    if (ECNT_PPEMGR_TLS_VID_PATTERN_VID == p_tls->vlan_type)
    {
        tls_cfg.tagEn = 1;
        for (i = 0; (i < ECNT_PPEMGR_TLS_MAXIMUM_VID_NUM) && (i < p_tls->vid_num); i++) 
        {
            tls_cfg.vids[i] = p_tls->vids[i];
            tls_cfg.tags++;
        }
    }
    else if (ECNT_PPEMGR_TLS_VID_PATTERN_RANGE == p_tls->vlan_type)
    {
        tls_cfg.tagEn = 1;
        tls_cfg.rng = 1;
        tls_cfg.vids[0] = p_tls->vid_min;
        tls_cfg.vids[1] = p_tls->vid_max;                
        tls_cfg.tags = 2;
    }   
    tls_cfg.en = 1;

    if (do_ppe_ioctl(&tls_cfg, PPE_COMMAND_TLS))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Remove PPE transparent line service
@param[in] p_tls TLS configuration
@return ::ECNT_SWITCHMGR_RET
@note In this case, only ECNT_SWITCHMGR_LAN2LAN_SRC_PORT::sport is needed in the parameter.
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_transparet_line_service(ECNT_PPEMGR_TLS_ENTRY *p_tls)
{
    struct ppe_tls_cfg tls_cfg = {0};

    /* TODO: sanity check */

    tls_cfg.method = PPE_METHOD_DEL;
    tls_cfg.uni = p_tls->sport;
    tls_cfg.en = 0;

    if (do_ppe_ioctl(&tls_cfg, PPE_COMMAND_TLS))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE meter group
@param[in] p_group meter group configuration
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_meter_group(ECNT_PPEMGR_METER_GROUP *p_group)
{
    struct ppe_meter_cfg meter = {0};

    /* TODO: sanity check */

    meter.group = p_group->group_id;
    if (p_group->enable)
    {
        meter.method = PPE_METHOD_ADD;
        meter.mode = p_group->mode;
        meter.scale = p_group->scale;
        meter.rate = p_group->rate;
        meter.enable = 1;
        meter.addFlowOnly = 2;
    }
    else
    {
        meter.method = PPE_METHOD_DEL;
    }
    
    if (do_ppe_ioctl(&meter, PPE_COMMAND_METER_GROUP))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Add PPE meter flow
@param[in] p_flow meter flow
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_meter_flow(ECNT_PPEMGR_METER_FLOW *p_flow)
{
    struct ppe_meter_cfg meter = {0};

    /* TODO: sanity check */

    meter.method = PPE_METHOD_ADD;
    meter.dir = p_flow->direction;
    meter.vlan = p_flow->vlan;
    meter.queue = p_flow->queue;
    meter.group = p_flow->group_id;
    meter.addFlowOnly = 1;
    
    if (do_ppe_ioctl(&meter, PPE_COMMAND_METER_FLOW))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Remove PPE meter flow
@param[in] p_flow meter flow
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_meter_flow(ECNT_PPEMGR_METER_FLOW *p_flow)
{
    struct ppe_meter_cfg meter = {0};

    /* TODO: sanity check */

    meter.method = PPE_METHOD_DEL;
    meter.dir = p_flow->direction;
    meter.vlan = p_flow->vlan;

    if (do_ppe_ioctl(&meter, PPE_COMMAND_METER_FLOW))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE trtcm group
@param[in] p_group trtcm group configuration
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_trtcm_group(ECNT_PPEMGR_TRTCM_GROUP *p_group)
{
    struct ppe_meter_cfg meter = {0};

    /* TODO: sanity check */

    meter.group = p_group->group_id;
    if (p_group->enable)
    {
        meter.method = PPE_METHOD_ADD;
        meter.mode = p_group->mode;
        meter.scale = p_group->pir_scale;
        meter.rate = p_group->pir;
        meter.scale2 = p_group->cir_scale;
        meter.rate2 = p_group->cir;
        meter.enable = 1;
        meter.trtcm = 1;
        meter.addFlowOnly = 2;
    }
    else
    {
        meter.method = PPE_METHOD_DEL;
    }
    
    if (do_ppe_ioctl(&meter, PPE_COMMAND_TRTCM_GROUP))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Add PPE trtcm flow
@param[in] p_flow trtcm flow
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_trtcm_flow(ECNT_PPEMGR_TRTCM_FLOW *p_flow)
{
    struct ppe_meter_cfg meter = {0};

    /* TODO: sanity check */

    meter.method = PPE_METHOD_ADD;
    meter.dir = p_flow->direction;
    meter.vlan = p_flow->vlan;
    meter.group = p_flow->group_id;
    meter.addFlowOnly = 1;
    
    if (do_ppe_ioctl(&meter, PPE_COMMAND_TRTCM_FLOW))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Remove PPE trtcm flow
@param[in] p_flow trtcm flow
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_trtcm_flow(ECNT_PPEMGR_TRTCM_FLOW *p_flow)
{
    struct ppe_meter_cfg meter = {0};

    /* TODO: sanity check */

    meter.method = PPE_METHOD_DEL;
    meter.dir = p_flow->direction;
    meter.vlan = p_flow->vlan;

    if (do_ppe_ioctl(&meter, PPE_COMMAND_TRTCM_FLOW))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Add PPE accounting group  flow
@param[in] p_flow accounting group flow
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_accounting_flow(ECNT_PPEMGR_ACCOUNT_FLOW *p_flow)
{
    struct ppe_account_cfg account_cfg = {0};

    /* TODO: sanity check */

    account_cfg.method = PPE_METHOD_ADD;
    account_cfg.group = p_flow->group_id;
    account_cfg.vlan = p_flow->vlan;
    account_cfg.dir = p_flow->direction;

    if (do_ppe_ioctl(&account_cfg, PPE_COMMAND_ACCOUNT))
    {
        return ECNT_PPEMGR_FAIL;
    }
    
    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Remove PPE accounting group flow
@param[in] p_flow accounting group flow
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_accounting_flow(ECNT_PPEMGR_ACCOUNT_FLOW *p_flow)
{
    struct ppe_account_cfg account_cfg = {0};

    /* TODO: sanity check */

    account_cfg.method = PPE_METHOD_DEL;
    account_cfg.vlan = p_flow->vlan;
    account_cfg.dir = p_flow->direction;
    account_cfg.grpOrFlow = METER_HANDLE_BY_FLOW;

    if (do_ppe_ioctl(&account_cfg, PPE_COMMAND_ACCOUNT))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Remove all PPE accounting group flows
@param[in] group_id accounting group id, range [0, ::ECNT_PPEMGR_ACCOUNTING_MAXIMUM_GROUP_ID]
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_accounting_all_flows(u8 group_id)
{
    struct ppe_account_cfg account_cfg = {0};

    /* TODO: sanity check */

    account_cfg.method = PPE_METHOD_DEL;
    account_cfg.group = group_id;
    account_cfg.grpOrFlow = METER_HANDLE_BY_GROUP;

    if (do_ppe_ioctl(&account_cfg, PPE_COMMAND_ACCOUNT))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Get PPE accounting group statistics
@param[in] group_id accounting group id, range [0, ::ECNT_PPEMGR_ACCOUNTING_MAXIMUM_GROUP_ID]
@param[out] p_stats accounting grouo statistics
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_get_accounting_statistics(u8 group_id, ECNT_PPEMGR_ACCOUNT_STATS *p_stats)
{
    struct ppe_account_cfg account_cfg = {0};

    /* TODO: sanity check */

    account_cfg.method = PPE_METHOD_GET;
    account_cfg.group = group_id;

    if (do_ppe_ioctl(&account_cfg, PPE_COMMAND_ACCOUNT_STATS))
    {
        return ECNT_PPEMGR_FAIL;
    }

    p_stats->byte_cnt_high = account_cfg.byteCntH;
    p_stats->byte_cnt_low = account_cfg.byteCntL;
    p_stats->packet_cnt = account_cfg.pktCntH;

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE cache enable
@param[in] enable enable/disable PPE cache
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_cache_enable(u8 enable)
{
    struct ppe_cache_cfg cache_cfg = {0};

    if (enable)
    {
        cache_cfg.method = CACHE_METHOD_ENABLE;
    }
    else
    {
        cache_cfg.method = CACHE_METHOD_DISABLE;
    }

    if (do_ppe_ioctl(&cache_cfg, PPE_COMMAND_CACHE))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE cache mode
@param[in] mode PPE cache mode, range [0, ::ECNT_PPEMGR_CACHE_MODE_MAX_VALUE]
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_cache_mode(u8 mode)
{
    struct ppe_cache_cfg cache_cfg = {0};

    cache_cfg.method = CACHE_METHOD_SET_MODE;
    cache_cfg.mode = mode;

    if (do_ppe_ioctl(&cache_cfg, PPE_COMMAND_CACHE))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Clear PPE cache
@param[in] index specify cache index, set to 0 to clear all cache entries
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_clear_cache(u32 index)
{
    struct ppe_cache_cfg cache_cfg = {0};

    cache_cfg.method = CACHE_METHOD_CLEAR;
    cache_cfg.foeIdx = index;

    if (do_ppe_ioctl(&cache_cfg, PPE_COMMAND_CACHE))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE hash mode
@param[in] mode PPE hash mode, range [0, ::ECNT_PPEMGR_HASH_MODE_MAX_VALUE]
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_mode(u8 mode)
{
    struct ppe_hash_cfg hash_cfg = {0};

    hash_cfg.method = HASH_METHOD_SET_MODE;
    hash_cfg.mode = mode;

    if (do_ppe_ioctl(&hash_cfg, PPE_COMMAND_HASH))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE hash bit shift
@param[in] shift PPE hash bit shift, range [0, ::ECNT_PPEMGR_HASH_BITSHIFT_MAX_VALUE]
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_bitshift(u8 shift)
{
    struct ppe_hash_cfg hash_cfg = {0};

    hash_cfg.method = HASH_METHOD_SET_BITSHIFT;
    hash_cfg.bitShift = shift;

    if (do_ppe_ioctl(&hash_cfg, PPE_COMMAND_HASH))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE hash tls
@param[in] type tls hash type
@param[in] enable enable/disable tls hash type
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_tls(ECNT_PPEMGR_HASH_TLS_TYPE type, u8 enable)
{
    struct ppe_hash_cfg hash_cfg = {0};

    hash_cfg.method = HASH_METHOD_SET_TLS;

    if (ECNT_PPEMGR_HASH_TLS_DMAC == type)
    {
        hash_cfg.tlsDmacDis = enable ? 2 : 3;
    }
    else if (ECNT_PPEMGR_HASH_TLS_PBIT == type)
    {
        hash_cfg.tlsPbitsDis = enable ? 2 : 3;
    }
    else if (ECNT_PPEMGR_HASH_TLS_ETYPE_DSCP == type)
    {
        hash_cfg.tlsEtypeDis = enable ? 2 : 3;
    }

    if (do_ppe_ioctl(&hash_cfg, PPE_COMMAND_HASH))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE L2B source mac hash enable
@param[in] enable enable/disable l2b smac hash
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_l2b_smac(u8 enable)
{
    struct ppe_hash_cfg hash_cfg = {0};

    hash_cfg.method = HASH_METHOD_SET_L2B;
    hash_cfg.l2bSmacEn = enable;

    if (do_ppe_ioctl(&hash_cfg, PPE_COMMAND_HASH))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE traffic qos flow rate
@param[in] flowid traffic flow id
@param[in] direction upstream or downstream
@param[in] rate flow rate limit
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_traffic_qos_flow_ratelimit(u8 flowid, ECNT_PPEMGR_TRAFFIC_DIR_TYPE direction, u32 rate)
{
    struct hwnat_tr_qos_flow_rate trafficQosCfg = {0};
    
    trafficQosCfg.direction = direction;
    trafficQosCfg.action = PPE_SET;
    trafficQosCfg.flowid = flowid;
    trafficQosCfg.ratelimit = rate;
    
    if (do_ppe_ioctl(&trafficQosCfg, HW_NAT_TRAFFIC_QOS_FLOW_RATE))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Get PPE traffic qos flow rate
@param[in] flowid traffic flow id
@param[in] direction upstream or downstream
@param[out] p_rate flow rate
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_get_traffic_qos_flow_ratelimit(u8 flowid, ECNT_PPEMGR_TRAFFIC_DIR_TYPE direction, u32 *p_rate)
{
    struct hwnat_tr_qos_flow_rate trafficQosCfg = {0};
    
    trafficQosCfg.direction = direction;
    trafficQosCfg.action = PPE_GET;
    trafficQosCfg.flowid = flowid;

    if (do_ppe_ioctl(&trafficQosCfg, HW_NAT_TRAFFIC_QOS_FLOW_RATE))
    {
        return ECNT_PPEMGR_FAIL;
    }

    *p_rate = trafficQosCfg.ratelimit;
    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Get PPE traffic qos flow counter
@param[in] flowid traffic flow id
@param[in] direction upstream or downstream
@param[out] p_byte_cnt byte counter
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_get_traffic_qos_flow_cnt(u8 flowid, ECNT_PPEMGR_TRAFFIC_DIR_TYPE direction, unsigned long long *p_byte_cnt)
{
    struct hwnat_tr_qos_flow_cnt trafficQosCfg = {0};
    int ret;
    
    trafficQosCfg.direction = direction;
    trafficQosCfg.flowid = flowid;

    if (do_ppe_ioctl(&trafficQosCfg, HW_NAT_TRAFFIC_QOS_FLOW_CNT))
    {
        return ECNT_PPEMGR_FAIL;
    }

    *p_byte_cnt = trafficQosCfg.cntbyte;
    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE localin offload enable
@param[in] enable enable/disable localin
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_localin_enable(u8 enable)
{
    struct ppe_local_in_cfg cfg = {0}; 

    if (enable)
    {
        cfg.method = LOCAL_IN_METHOD_ENABLE;
    }
    else
    {
        cfg.method = LOCAL_IN_METHOD_DISABLE;
    }

    if (do_ppe_ioctl(&cfg, PPE_COMMAND_LOCAL_IN))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Enable PPE localin offload for specific dst port
@param[in] port_id port id, range [0, 8], 0 means enable all port
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_localin_dst_port(u16 port_id)
{
    u16 port = port_id;
    struct ppe_local_in_cfg cfg = {0}; 

    if (0 == port_id)
    {
        cfg.method = LOCAL_IN_METHOD_ADD_ALL;
    }
    else
    {
        cfg.method = LOCAL_IN_METHOD_ADD_DPORT;
        cfg.dport_num = 1;
        cfg.dport_ptr = &port;
    }
    
    if (do_ppe_ioctl(&cfg, PPE_COMMAND_LOCAL_IN))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Disable PPE localin offload for specific dst port
@param[in] port_id port id, range [0, 8], 0 means disable all port
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_localin_dst_port(u16 port_id)
{
    u16 port = port_id;
    struct ppe_local_in_cfg cfg = {0}; 

    if (0 == port_id)
    {
        cfg.method = LOCAL_IN_METHOD_DEL_ALL;
    }
    else
    {
        cfg.method = LOCAL_IN_METHOD_DEL_DPORT;
        cfg.dport_num = 1;
        cfg.dport_ptr = &port;
    }
    
    if (do_ppe_ioctl(&cfg, PPE_COMMAND_LOCAL_IN))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE binding threshold
@param[in] num binding threshold, packet number per second. Default value is 30.
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_binding_threshold(u16 num)
{
    struct hwnat_config_args opt = {0 };

    opt.bind_threshold = num;
    if (do_ppe_ioctl(&opt, HW_NAT_BIND_THRESHOLD))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE binding entry number limit during a time stamp unit
@param[in] quater_limit entry number limit when total entries less than 1/4 of the total num
@param[in] half_limit entry number limit when total entries between 1/4 and 1/2 of the total num
@param[in] full_limit entry number limit when total entries more than 1/2 of the total num
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_binding_limit(u16 quater_limit, u16 half_limit, u16 full_limit)
{
    struct hwnat_config_args opt = {0 };

    opt.foe_qut_lmt = quater_limit;
    opt.foe_half_lmt = half_limit;
    opt.foe_full_lmt = full_limit;
    if (do_ppe_ioctl(&opt, HW_NAT_MAX_ENTRY_LMT))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE entry keepalive internal for tcp and udp rules
@param[in] tcp_ka_time keep alive internal for tcp, unit: 4 seconds
@param[in] udp_ka_time keep alive internal for udp, unit: 4 seconds
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_keep_alive_interval(u8 tcp_ka_time, u8 udp_ka_time)
{
    struct hwnat_config_args opt = {0 };

    opt.foe_tcp_ka = tcp_ka_time;
    opt.foe_udp_ka = udp_ka_time;
    if (do_ppe_ioctl(&opt, HW_NAT_KA_INTERVAL))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE unbound entry age time
@param[in] time age time for unbound entry, unit: 1 second
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_unbound_age_time(u8 time)
{
    struct hwnat_config_args opt = {0 };

    opt.foe_unb_dlta = time;
    if (do_ppe_ioctl(&opt, HW_NAT_UB_LIFETIME))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE bound entry age time
@param[in] tcp_time time age time for bound tcp entry, unit: 1 second
@param[in] udp_time time age time for bound udp entry, unit: 1 second
@param[in] tcp_fin_time time age time for bound tcp fin entry, unit: 1 second
@param[in] ntu_time time age time for bound non tcp/udp entry, unit: 1 second
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_bound_age_time(u16 tcp_time, u16 udp_time, u16 tcp_fin_time, u16 ntu_time)
{
    struct hwnat_config_args opt = {0 };

    opt.foe_tcp_dlta = tcp_time;
    opt.foe_udp_dlta = udp_time;
    opt.foe_fin_dlta = tcp_fin_time;
    opt.foe_ntu_dlta = ntu_time;
    if (do_ppe_ioctl(&opt, HW_NAT_BIND_LIFETIME))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE entry binding direction
@param[in] direction enable traffic flow offload by specific direction
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_binding_direction(ECNT_PPEMGR_FOE_BINDING_DIR_TYPE direction)
{
    struct hwnat_args opt = {0};

    opt.entry_num = direction;
    if (do_ppe_ioctl(&opt, HW_NAT_BIND_DIRECTION))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE multicast entry support
@param[in] enable enable/disable FOE multicast support
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_multicast_enable(u8 enable)
{
    struct hwnat_config_args opt = {0 };

    opt.enable = enable;
    if (do_ppe_ioctl(&opt, HW_NAT_MULTICAST_EN))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Set PPE FOE ip protocol check mode
@param[in] mode ip protocol check mode
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_ip_proto_check_type(ECNT_PPEMGR_FOE_IPPROTO_CHECK_MODE mode)
{
    struct hwnat_config_args opt = {0 };

    opt.ip_chk_type = mode;
    if (do_ppe_ioctl(&opt, HW_NAT_SET_IP_CHK))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Add PPE FOE entry
@param[in] p_foe flow offload engine rule
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip: 
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_foe_entry(ECNT_PPEMGR_FOE_ENTRY *p_foe)
{
    struct hwnat_tuple rule = {0};

    rule.pkt_type = p_foe->type;
    if (ECNT_PPEMGR_FOE_FLOW_TYPE_L2B == p_foe->type)
    {
        memcpy(rule.in_smac, p_foe->smac, 6);
        memcpy(rule.in_dmac, p_foe->dmac, 6);
    }
    else
    {
        memcpy(rule.smac, p_foe->smac, 6);
        memcpy(rule.dmac, p_foe->dmac, 6);
    }

    rule.ing_sipv4 =   p_foe->src_ip4;
    rule.ing_dipv4 =   p_foe->dst_ip4;
    rule.ing_sipv6_0 = p_foe->src_ip6[0];
    rule.ing_sipv6_1 = p_foe->src_ip6[1];
    rule.ing_sipv6_2 = p_foe->src_ip6[2];
    rule.ing_dipv6_3 = p_foe->src_ip6[3];
    rule.ing_dipv6_0 = p_foe->dst_ip6[0];
    rule.ing_dipv6_1 = p_foe->dst_ip6[1];
    rule.ing_dipv6_2 = p_foe->dst_ip6[2];
    rule.ing_dipv6_3 = p_foe->dst_ip6[3];
    rule.ing_sp      = p_foe->src_port;
    rule.ing_dp      = p_foe->dst_port;
    rule.eg_sipv4    = p_foe->new_src_ip4;
    rule.eg_dipv4    = p_foe->new_dst_ip4;
    rule.eg_sipv6_0 = p_foe->new_src_ip6[0];
    rule.eg_sipv6_1 = p_foe->new_src_ip6[1];
    rule.eg_sipv6_2 = p_foe->new_src_ip6[2];
    rule.eg_dipv6_3 = p_foe->new_src_ip6[3];
    rule.eg_dipv6_0 = p_foe->new_dst_ip6[0];
    rule.eg_dipv6_1 = p_foe->new_dst_ip6[1];
    rule.eg_dipv6_2 = p_foe->new_dst_ip6[2];
    rule.eg_dipv6_3 = p_foe->new_dst_ip6[3];
    rule.eg_sp      = p_foe->new_src_port;
    rule.eg_dp      = p_foe->new_dst_port;
    rule.vlan1      = p_foe->vlan1;
    rule.vlan2      = p_foe->vlan2;
    rule.stag       = p_foe->stag;
    rule.pppoe_id   = p_foe->pppoe_id;
    rule.is_udp     = p_foe->is_udp ? 1 : 0;
    rule.dst_port   = p_foe->dst_port_id;
    rule.hash_index_shift = p_foe->hash_index_shift;
    rule.eg_dscp    = p_foe->new_dscp;
    rule.info2      = p_foe->info2;
    rule.RMT        = p_foe->tunnel_action;
    rule.ag         = p_foe->accounting_group_id;
    rule.mg         = p_foe->meter_group_id;
    rule.ipv6_flowlabel = p_foe->ipv6_flow_label;
    rule.vpm        = p_foe->vpm;
    rule.prot       = p_foe->protocol;
    rule.tunnel_dscp        = p_foe->tunnel_dscp;
    rule.tunnel_TTL         = p_foe->tunnel_ttl;
    rule.tunnel_IPv4_flag   = p_foe->tunnel_ipv4_flag;
    rule.tsid       = p_foe->tsid;
    rule.qid        = p_foe->queue_id;
    rule.pcp        = p_foe->pcp;
    rule.vlan_layer = p_foe->vlan_layer;
    rule.in_vlan    = p_foe->in_vlan;
    rule.hsk_l2     = p_foe->hsk_l2;
    rule.channel    = p_foe->channel;
    rule.hsk_mc     = p_foe->hsk_mc;
    rule.in_gre_call_id     = p_foe->in_gre_call_id;
    rule.out_gre_call_id    = p_foe->out_gre_call_id;

    if (do_ppe_ioctl(&rule, HW_NAT_ADD_ENTRY))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Delete PPE FOE entry
@param[in] index entry index to clear, 0 means all entries.
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_foe_entry(u32 index)
{
    struct hwnat_args opt = {0};

    opt.entry_num = index;
    if (do_ppe_ioctl(&opt, HW_NAT_FOE_REMOVE))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Get PPE FOE entries
@param[in] type get entry type
@param[out] p_foe pointer to all FOE entries
@param[out] p_num FOE entry number
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
@note User should allocate memory for ECNT_PPEMGR_MAXIMUM_TABLE_SIZE foe entries.
*/
ECNT_PPEMGR_RET ppemgr_lib_get_foe_entry(ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE type, ECNT_PPEMGR_FOE_ENTRY *p_foe, u32 *p_num)
{
    int i;
    struct hwnat_args *args = NULL;

    if (type >= ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE_MAX)
    {
        return ECNT_PPEMGR_INVALID_PARAM;
    }

    /* Max table size is 16K */
    args=malloc(sizeof(struct hwnat_args) + sizeof(struct hwnat_tuple) * 1024 * 16);
    if (!args)
    {
        return ECNT_PPEMGR_NO_MEMORY;
    }
    
    memset(args, 0, (sizeof(struct hwnat_args) + sizeof(struct hwnat_tuple) * 1024 * 16));
    if (ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE_BOUND == type)
    {
        args->entry_state = 2;
    }
    else if (ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE_UNBOUND == type)
    {
        args->entry_state = 1;
    }
    else
    {
        args->entry_state = 0;
    }
    
    if (do_ppe_ioctl(args, HW_NAT_GET_ALL_ENTRIES))
    {
        free(args);
        return ECNT_PPEMGR_FAIL;
    }

    *p_num = args->num_of_entries;
    for(i = 0; i < args->num_of_entries; i++)
    {
        p_foe[i].type = args->entries[i].pkt_type;     
        if (ECNT_PPEMGR_FOE_FLOW_TYPE_L2B == p_foe[i].type)
        {
            memcpy(p_foe[i].smac, args->entries[i].in_smac, 6);
            memcpy(p_foe[i].dmac, args->entries[i].in_dmac, 6);
        }
        else
        {
            memcpy(p_foe[i].smac, args->entries[i].smac, 6);
            memcpy(p_foe[i].dmac, args->entries[i].dmac, 6);
        }

        p_foe[i].src_ip4 = args->entries[i].ing_sipv4;
        p_foe[i].dst_ip4 = args->entries[i].ing_dipv4;
        p_foe[i].src_ip6[0] = args->entries[i].ing_sipv6_0;
        p_foe[i].src_ip6[1] = args->entries[i].ing_sipv6_1;
        p_foe[i].src_ip6[2] = args->entries[i].ing_sipv6_2;
        p_foe[i].src_ip6[3] = args->entries[i].ing_dipv6_3;
        p_foe[i].dst_ip6[0] = args->entries[i].ing_dipv6_0;
        p_foe[i].dst_ip6[1] = args->entries[i].ing_dipv6_1;
        p_foe[i].dst_ip6[2] = args->entries[i].ing_dipv6_2;
        p_foe[i].dst_ip6[3] = args->entries[i].ing_dipv6_3;
        p_foe[i].src_port = args->entries[i].ing_sp;
        p_foe[i].dst_port = args->entries[i].ing_dp;
        p_foe[i].new_src_ip4 = args->entries[i].eg_sipv4;
        p_foe[i].new_dst_ip4 = args->entries[i].eg_dipv4;
        p_foe[i].new_src_ip6[0] = args->entries[i].eg_sipv6_0;
        p_foe[i].new_src_ip6[1] = args->entries[i].eg_sipv6_1;
        p_foe[i].new_src_ip6[2] = args->entries[i].eg_sipv6_2;
        p_foe[i].new_src_ip6[3] = args->entries[i].eg_dipv6_3;
        p_foe[i].new_dst_ip6[0] = args->entries[i].eg_dipv6_0;
        p_foe[i].new_dst_ip6[1] = args->entries[i].eg_dipv6_1;
        p_foe[i].new_dst_ip6[2] = args->entries[i].eg_dipv6_2;
        p_foe[i].new_dst_ip6[3] = args->entries[i].eg_dipv6_3;
        p_foe[i].new_src_port = args->entries[i].eg_sp;
        p_foe[i].new_dst_port = args->entries[i].eg_dp;
        p_foe[i].vlan1 = args->entries[i].vlan1;
        p_foe[i].vlan2 = args->entries[i].vlan2;
        p_foe[i].stag = args->entries[i].stag;
        p_foe[i].pppoe_id = args->entries[i].pppoe_id;
        p_foe[i].is_udp = args->entries[i].is_udp;
        p_foe[i].dst_port_id = args->entries[i].dst_port;
        p_foe[i].hash_index_shift = args->entries[i].hash_index_shift;
        p_foe[i].new_dscp = args->entries[i].eg_dscp;
        p_foe[i].info2 = args->entries[i].info2;
        p_foe[i].tunnel_action = args->entries[i].RMT;
        p_foe[i].accounting_group_id = args->entries[i].ag;
        p_foe[i].meter_group_id = args->entries[i].mg;
        p_foe[i].ipv6_flow_label = args->entries[i].ipv6_flowlabel;
        p_foe[i].vpm = args->entries[i].vpm;
        p_foe[i].protocol = args->entries[i].prot;
        p_foe[i].tunnel_dscp = args->entries[i].tunnel_dscp;
        p_foe[i].tunnel_ttl = args->entries[i].tunnel_TTL;
        p_foe[i].tunnel_ipv4_flag = args->entries[i].tunnel_IPv4_flag;
        p_foe[i].tsid = args->entries[i].tsid;
        p_foe[i].queue_id = args->entries[i].qid;
        p_foe[i].pcp = args->entries[i].pcp;
        p_foe[i].vlan_layer = args->entries[i].vlan_layer;
        p_foe[i].in_vlan = args->entries[i].in_vlan;
        p_foe[i].hsk_l2 = args->entries[i].hsk_l2;
        p_foe[i].channel = args->entries[i].channel;
        p_foe[i].hsk_mc = args->entries[i].hsk_mc;
        p_foe[i].in_gre_call_id = args->entries[i].in_gre_call_id;
        p_foe[i].out_gre_call_id = args->entries[i].out_gre_call_id;
        p_foe[i].l2b_type = args->entries[i].new_l2b;
        p_foe[i].hash_index = args->entries[i].hash_index;
    }

    free(args);    
    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Clear all PPE LAN FOE entries of specific lan port
@param[in] port_id lan port id
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_clear_lan_foe_entries(u8 port_id)
{
    struct hwnat_args opt = {0};

    opt.entry_num = port_id;
    if (do_ppe_ioctl(&opt, HW_NAT_CLEAN_LAN))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/**
@brief Clear all PPE WAN FOE entries of specific vlan
@param[in] vlan_id wan vlan id
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_clear_wan_foe_entries(u16 vlan_id)
{
    struct hwnat_args opt = {0};

    opt.entry_num = vlan_id;
    if (do_ppe_ioctl(&opt, HW_NAT_CLEAN_WAN))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

/*************************************************************************************/
/* legacy api, keep as is for now                                                                                                      */
/*************************************************************************************/
ECNT_PPEMGR_RET ppemgr_lib_set_meter_ratelimit(unsigned int meterGroup, unsigned int rate)
{
    struct ppe_meter_cfg meterCfg;
    memset(&meterCfg, 0, sizeof(struct ppe_meter_cfg));

    if(meterGroup >= METER_GROUP_MAX)
        return -1;
    
    meterCfg.group = meterGroup;
    meterCfg.rate = rate;//kbps
    meterCfg.mode = METER_MODE_BYTE;
    meterCfg.vlan = 0;//just  config register
    meterCfg.scale = METER_SCALE_KBPS;//Kbps
    meterCfg.enable = 1;
    meterCfg.addFlowOnly = 2;// only set the meter register
    meterCfg.method = PPE_METHOD_ADD;

	if (do_ppe_ioctl((void *)(&meterCfg), PPE_COMMAND_METER))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;

}

/**
@brief get wan account 
@param[in] wan id
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_get_wan_acnt(u16 wan_id, ECNT_PPEMGR_WAN_ACCOUNT *cnt)
{
    struct hwnat_wan_acnt_args args;

	memset(&args, 0, sizeof(struct hwnat_wan_acnt_args));
    args.wanid = wan_id;
	args.action = PPE_METHOD_READ;
	
    if (do_ppe_ioctl(&args, HW_NAT_WAN_ACNT))
    {
        return ECNT_PPEMGR_FAIL;
    }

	memcpy(cnt, &(args.wan_acnt), sizeof(struct hwnat_wan_acnt));

    return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_set_wan_rate_threshold(unsigned int threshold)
{
	struct hwnat_wan_ratelimit_args hwnat_wan_ratelimit = {0};

	hwnat_wan_ratelimit.method = WAN_RATELIMIT_METHOD_SET_THRESHOLD;
	hwnat_wan_ratelimit.threshold = threshold;

	if (do_ppe_ioctl((void *)(&hwnat_wan_ratelimit), HW_NAT_WAN_RATELIMIT))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_set_wan_mtu(unsigned char wan_type, unsigned int mtu)
{
	struct hwnat_wan_ratelimit_args hwnat_wan_ratelimit = {0};

	hwnat_wan_ratelimit.method = WAN_RATELIMIT_METHOD_SET_MTU;
	hwnat_wan_ratelimit.wan_type = wan_type;
	hwnat_wan_ratelimit.mtu = mtu;

	if (do_ppe_ioctl((void *)(&hwnat_wan_ratelimit), HW_NAT_WAN_RATELIMIT))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_set_wan_ratelimit(unsigned char wanid, unsigned char direction, unsigned int ratelimit)
{
	struct hwnat_wan_ratelimit_args hwnat_wan_ratelimit = {0};

	hwnat_wan_ratelimit.method = WAN_RATELIMIT_METHOD_SET_RATE;
	hwnat_wan_ratelimit.direction = direction;
	hwnat_wan_ratelimit.ratelimit = ratelimit;
	hwnat_wan_ratelimit.wanid = wanid;

	if (do_ppe_ioctl((void *)(&hwnat_wan_ratelimit), HW_NAT_WAN_RATELIMIT))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}


ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_add(unsigned int ip, unsigned int port, unsigned int protocol)
{
	struct black_ip_port_portocol blackip_port_protocol_add = {0};

	blackip_port_protocol_add.action = PPE_BLACK_IP_PORT_PROTOCOL_ADD;
	blackip_port_protocol_add.ip = ip;
	blackip_port_protocol_add.port = port;
	blackip_port_protocol_add.protocol = protocol;

	if (do_ppe_ioctl((void *)(&blackip_port_protocol_add), PPE_COMMAND_BLACK_IP_PROTOCOL_PORT))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_delete(unsigned int ip, unsigned int port, unsigned int protocol)
{
	struct black_ip_port_portocol blackip_port_protocol_delete = {0};

	blackip_port_protocol_delete.action = PPE_BLACK_IP_PORT_PROTOCOL_DELETE;
	blackip_port_protocol_delete.ip = ip;
	blackip_port_protocol_delete.port = port;
	blackip_port_protocol_delete.protocol = protocol;

	if (do_ppe_ioctl((void *)(&blackip_port_protocol_delete), PPE_COMMAND_BLACK_IP_PROTOCOL_PORT))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_delete_all(void)
{
	struct black_ip_port_portocol blackip_port_protocol_delete_all = {0};

	blackip_port_protocol_delete_all.action = PPE_BLACK_IP_PORT_PROTOCOL_DELETE_ALL;

	if (do_ppe_ioctl((void *)(&blackip_port_protocol_delete_all), PPE_COMMAND_BLACK_IP_PROTOCOL_PORT))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_show(void)
{
	struct black_ip_port_portocol blackip_port_protocol_show = {0};

	blackip_port_protocol_show.action = PPE_BLACK_IP_PORT_PROTOCOL_SHOW;

	if (do_ppe_ioctl((void *)(&blackip_port_protocol_show), PPE_COMMAND_BLACK_IP_PROTOCOL_PORT))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_drop_unknow_multicast_by_index(u32 entry_index,u8 foe_ai,u32 foe_magic,ECNT_PPEMGR_IP_PROTO proto,u8* grp_addr)
{
	struct hwnat_args opt;

	opt.multi_drop.entry_index = entry_index;
	opt.multi_drop.foe_ai = foe_ai;
	opt.multi_drop.foe_magic = foe_magic;
	opt.multi_drop.proto = proto;
	memcpy(opt.multi_drop.grp_addr,grp_addr,16);	

	if (do_ppe_ioctl(&opt, HW_NAT_UN_MULTI_DROP_BY_INDEX))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_query_multicast_drop_list(u8 *mca, ECNT_PPEMGR_IP_PROTO proto)
{
	struct hwnat_args opt;

	opt.multi_drop.proto = proto;
	memcpy(opt.multi_drop.grp_addr,mca,16);

	if (do_ppe_ioctl(&opt,HW_NAT_QUERY_MULTI_DROP_LIST))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_enable(unsigned char port)
{
	int ret;
	struct ppe_mirror_args mirCfg;

	memset(&mirCfg, 0, sizeof(struct ppe_mirror_args));
    mirCfg.func_id = PPE_MIRROR_FUNC_ID_ENABLE;
	mirCfg.port = port;

	if (do_ppe_ioctl((void *)(&mirCfg), PPE_COMMAND_MIRROR_EN))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_disable(void)
{
	int ret;
	struct ppe_mirror_args mirCfg;

	memset(&mirCfg, 0, sizeof(struct ppe_mirror_args));
    mirCfg.func_id = PPE_MIRROR_FUNC_ID_DISABLE;

	if (do_ppe_ioctl((void *)(&mirCfg), PPE_COMMAND_MIRROR_EN))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_mode(unsigned char mode)
{
	int ret;
	struct ppe_mirror_args mirCfg;

	memset(&mirCfg, 0, sizeof(struct ppe_mirror_args));
    mirCfg.func_id = PPE_MIRROR_FUNC_ID_MODE;
    mirCfg.mode = mode;

	if (do_ppe_ioctl((void *)(&mirCfg), PPE_COMMAND_MIRROR_EN))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_enable_config(int enable,int fport,int nbq,int sptag)
{
	int ret;
	struct ppe_mirror_args mirCfg;

	memset(&mirCfg, 0, sizeof(struct ppe_mirror_args));
    mirCfg.func_id = PPE_MIRROR_FUNC_ID_ENABLE_CONFIG;
    mirCfg.enable_config.enable = enable;
    mirCfg.enable_config.fport = fport;
    mirCfg.enable_config.nbq = nbq;
    mirCfg.enable_config.sptag = sptag;

	if (do_ppe_ioctl((void *)(&mirCfg), PPE_COMMAND_MIRROR_EN))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_mode_config(int mode,int mirror_dst,int mirror_src,int mirror_mask)
{
	int ret;
	struct ppe_mirror_args mirCfg;

	memset(&mirCfg, 0, sizeof(struct ppe_mirror_args));
    mirCfg.func_id = PPE_MIRROR_FUNC_ID_MODE_CONFIG;
    mirCfg.mode_config.mode = mode;
    mirCfg.mode_config.mirror_dst = mirror_dst;
    mirCfg.mode_config.mirror_src = mirror_src;
    mirCfg.mode_config.mirror_mask = mirror_mask;

	if (do_ppe_ioctl((void *)(&mirCfg), PPE_COMMAND_MIRROR_EN))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_wan_mtu(int wan_idx, int wan_mtu)
{
	int ret;
	struct ppe_wan_mtu_args mtuCfg;

	memset(&mtuCfg, 0, sizeof(struct ppe_wan_mtu_args));
	mtuCfg.wan_mtu = wan_mtu;
	mtuCfg.wan_idx = wan_idx;

	if (do_ppe_ioctl((void *)(&mtuCfg), PPE_COMMAND_WAN_MTU))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_black_ip_add(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign)
{   
	struct black_ip_cnt blackip_add = {0};

	blackip_add.ip = ip;
	blackip_add.mask = mask;
	blackip_add.type = type;
	blackip_add.sign = sign;
	blackip_add.ipv6_mode = 0;
	
	if (do_ppe_ioctl((void *)(&blackip_add), PPE_COMMAND_BLACK_IP_ADD))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_black_ipv6_add(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign)
{   
	struct black_ip_cnt blackip_add= {0};

	memcpy(blackip_add.u6_addr8, u6_addr8,sizeof(blackip_add.u6_addr8));
	blackip_add.mask = mask;
	blackip_add.type = type;
	blackip_add.sign = sign;
	blackip_add.ipv6_mode = 1;
	
	if (do_ppe_ioctl((void *)(&blackip_add), PPE_COMMAND_BLACK_IP_ADD))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_black_ip_delete(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign)
{   
	struct black_ip_cnt blackip_delete = {0}; 

	blackip_delete.ip = ip;
	blackip_delete.mask = mask;
	blackip_delete.type = type;
	blackip_delete.sign = sign;

	if (do_ppe_ioctl((void *)(&blackip_delete), PPE_COMMAND_BLACK_IP_DELETE))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_black_ipv6_delete(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign)
{   
	struct black_ip_cnt blackip_delete = {0};

	memcpy(blackip_delete.u6_addr8, u6_addr8,sizeof(blackip_delete.u6_addr8));
	blackip_delete.mask = mask;
	blackip_delete.type = type;
	blackip_delete.sign = sign;
	blackip_delete.ipv6_mode = 1;

	if (do_ppe_ioctl((void *)(&blackip_delete), PPE_COMMAND_BLACK_IP_DELETE))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}


ECNT_PPEMGR_RET ppemgr_lib_black_ip_delete_same_sign(unsigned int sign)
{
	if (do_ppe_ioctl((void *)(&sign), PPE_COMMAND_BLACK_IP_DELETE_SIGN))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_set_mtu(unsigned short mtu)
{
	if (do_ppe_ioctl((void *)(&mtu), HW_NAT_SET_MTU))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

static int parse_npt_str(unsigned char* str,unsigned short* p_prefix,unsigned int* p_prefix_len)
{
	unsigned char* ptr = NULL;
	unsigned short tmp[8];
	unsigned int len = 0;
	int i = 0;
	unsigned char* start = NULL;
	unsigned char tmp_str[32];	 

	ptr = strstr(str,":");
	if(!ptr)
		return -1;
	
	start = str;
	while(ptr)
	{
		memset(tmp_str,0,32);
		memmove(tmp_str,start,ptr-start);
		tmp[i] = strtoul(tmp_str,NULL,16);
		p_prefix[i] = tmp[i];
		if(*(ptr+1) == ':')
			break;
		start = ptr+1;
		ptr = strstr(start,":");
		i++;
	}

	ptr = strstr(str,"/");
	if(!ptr)
		return -1;
	
	memset(tmp_str,0,32);
	memmove(tmp_str,ptr+1,strlen(ptr)-1);
	len = strtoul(tmp_str,NULL,10);
	*p_prefix_len = len;
	
	return 0;
}

ECNT_PPEMGR_RET ppemgr_lib_add_nptv6_prefix(unsigned char* ifname,unsigned char* snpt_str,unsigned char* dnpt_str)
{
	struct hwnat_nptv6_prefix_args arg;
	memset(&arg, 0, sizeof(struct hwnat_nptv6_prefix_args));

	arg.function_id = HWNAT_NPTV6_PREFIX_ID_ADD;

	if(strlen(ifname) > 16)
		return ECNT_PPEMGR_FAIL;

	strcpy(arg.ifname,ifname);
	
	if(-1 == parse_npt_str(snpt_str,arg.s_prefix,&(arg.s_prefix_len)))
		return ECNT_PPEMGR_FAIL;
	
	if(-1 == parse_npt_str(dnpt_str,arg.d_prefix,&(arg.d_prefix_len)))
		return ECNT_PPEMGR_FAIL;

	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_SET_NPTV6_PREFIX))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_del_nptv6_prefix(unsigned char* ifname)
{
	struct hwnat_nptv6_prefix_args arg;
	memset(&arg, 0, sizeof(struct hwnat_nptv6_prefix_args));

	arg.function_id = HWNAT_NPTV6_PREFIX_ID_DEL;

	if(strlen(ifname) > 16)
		return ECNT_PPEMGR_FAIL;
	
	strcpy(arg.ifname,ifname);

	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_SET_NPTV6_PREFIX))	
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_del_nptv6_prefix_all(void)
{
	struct hwnat_nptv6_prefix_args arg;
	memset(&arg, 0, sizeof(struct hwnat_nptv6_prefix_args));

	arg.function_id = HWNAT_NPTV6_PREFIX_ID_DEL_ALL;

	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_SET_NPTV6_PREFIX))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}


ECNT_PPEMGR_RET ppemgr_lib_set_nptv6_mode(unsigned char mode)
{
	struct hwnat_nptv6_mode_args arg;
	memset(&arg, 0, sizeof(struct hwnat_nptv6_mode_args));

	arg.function_id = HWNAT_NPTV6_MODE_ID_SET;

	arg.mode = mode;
	
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_NPTV6_MODE))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_get_nptv6_mode(unsigned char* p_mode)
{
	struct hwnat_nptv6_mode_args arg;
	memset(&arg, 0, sizeof(struct hwnat_nptv6_mode_args));

	arg.function_id = HWNAT_NPTV6_MODE_ID_GET;
	
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_NPTV6_MODE))
		return ECNT_PPEMGR_FAIL;

	printf("arg.mode = %d\n",arg.mode);

	*p_mode = arg.mode;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_ppe2_enable(void)
{
	struct hwnat_ppe2_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_ppe2_cmd_args));

	arg.function_id = HWNAT_PPE2_CMD_ID_ENABLE;
    
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_PPE2))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_ppe2_disable(void)
{
	struct hwnat_ppe2_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_ppe2_cmd_args));

	arg.function_id = HWNAT_PPE2_CMD_ID_DISABLE;
	
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_PPE2))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

int ppemgr_lib_clean_unicast_entry(void)
{
	struct hwnat_args opt;

	return do_ppe_ioctl(&opt, PPE_CLEAN_UNI_ENTRY);
}    

ECNT_PPEMGR_RET ppemgr_lib_set_wan_acnt_clear(u16 wan_id)
{
    struct hwnat_wan_acnt_args args;

	memset(&args, 0, sizeof(struct hwnat_wan_acnt_args));
    args.wanid = wan_id;
	args.action = PPE_METHOD_CLEAR;
	
    if (do_ppe_ioctl(&args, HW_NAT_WAN_ACNT))
    {
        return ECNT_PPEMGR_FAIL;
    }

    return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_vxlan_hash_enable(void)
{
	struct hwnat_vxlan_hash_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_vxlan_hash_cmd_args));

	arg.function_id = HWNAT_VXLAN_HASH_CMD_ID_ENABLE;
    
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_VXLAN_HASH))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_vxlan_hash_disable(void)
{
	struct hwnat_vxlan_hash_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_vxlan_hash_cmd_args));

	arg.function_id = HWNAT_VXLAN_HASH_CMD_ID_DISABLE;
	
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_VXLAN_HASH))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_vxlan_hash_show(void)
{
	struct hwnat_vxlan_hash_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_vxlan_hash_cmd_args));

	arg.function_id = HWNAT_VXLAN_HASH_CMD_ID_SHOW;

	if (do_ppe_ioctl((void *)(&arg), PPE_COMMAND_VXLAN_HASH))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_l2tp_hash_enable(void)
{
	struct hwnat_l2tp_hash_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_l2tp_hash_cmd_args));

	arg.function_id = HWNAT_L2TP_HASH_CMD_ID_ENABLE;
    
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_L2TP_HASH))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_l2tp_hash_disable(void)
{
	struct hwnat_l2tp_hash_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_l2tp_hash_cmd_args));

	arg.function_id = HWNAT_L2TP_HASH_CMD_ID_DISABLE;
	
	if(do_ppe_ioctl((void *)(&arg), PPE_COMMAND_L2TP_HASH))
		return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

ECNT_PPEMGR_RET ppemgr_lib_l2tp_hash_show(void)
{
	struct hwnat_l2tp_hash_cmd_args arg;
	memset(&arg, 0, sizeof(struct hwnat_l2tp_hash_cmd_args));

	arg.function_id = HWNAT_L2TP_HASH_CMD_ID_SHOW;

	if (do_ppe_ioctl((void *)(&arg), PPE_COMMAND_L2TP_HASH))
	    return ECNT_PPEMGR_FAIL;

	return ECNT_PPEMGR_SUCCESS;
}

