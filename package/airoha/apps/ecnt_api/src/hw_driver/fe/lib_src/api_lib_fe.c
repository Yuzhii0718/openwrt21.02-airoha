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
#include "libapi_lib_fe.h"
#include <ecnt_hook/ecnt_hook_fe_type.h>

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

static int fe_lib_ioctl(int cmd, void *p)
{
    int fd, ret;
    
    fd = open("/dev/fe", O_RDWR);
    if (fd < 0) 
    {
        printf("Open %s pseudo device failed\n","/dev/fe");
        return -1;
    }

    ret = ioctl(fd, 0, p);
    close(fd);

    if(ret >= 0) 
    {
        return 0;
    }
    
    return -1;
}

/**
@brief Set FrameEngine MYMAC
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] mac mac address
@param[in] mask mask len
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_mymac(ECNT_FEMGR_GDM_SEL gdm_sel, u8 *mac, u32 mask)
{
    int ret;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = FE_SET_MAC_ADDR;
    para.api_data.mac_addr.mac  = mac;
    para.gdm_sel                = gdm_sel;
    para.api_data.mac_addr.mask = mask;
    
    ret = fe_lib_ioctl(FE_SET_MAC_ADDR, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine maximum/minimum packet length
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] max_len maximum packet length
@param[in] min_len minimum packet length
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_pkt_length(ECNT_FEMGR_GDM_SEL gdm_sel, u32 max_len, u32 min_len)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_SET_PACKET_LENGTH;
    para.gdm_sel                        = gdm_sel;
    para.api_data.pkt_len.length_long   = max_len;
    para.api_data.pkt_len.length_short  = min_len;
    
    ret = fe_lib_ioctl(FE_SET_PACKET_LENGTH, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine GDMA channel tx/rx enable
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] txrx_sel indicate packet direction
@param[in] channel GDMA channel
@param[in] enable enable/disable channel tx/rx
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_gdma_channel_enable(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_DIRECTION txrx_sel, u32 channel, u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_CHANNEL_ENABLE;
    para.gdm_sel            = gdm_sel;
    para.txrx_sel           = txrx_sel;
    para.api_data.enable    = enable;
    para.channel            = channel;
    
    ret = fe_lib_ioctl(FE_SET_CHANNEL_ENABLE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine CDMA channel enable
@param[in] cdm_sel indicate CDMA1/CDMA2
@param[in] channel CDMA channel
@param[in] enable enable/disable channel
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_cdma_channel_enable(ECNT_FEMGR_CDM_SEL cdm_sel, u32 channel, u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_HWFWD_CHANNEL;
    para.cdm_sel            = cdm_sel;
    para.api_data.enable    = enable;
    para.channel            = channel;
    
    ret = fe_lib_ioctl(FE_SET_HWFWD_CHANNEL, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngineretire mode
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] channel GDMA channel
@param[in] mode channel retire mode
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_channel_retire(ECNT_FEMGR_GDM_SEL gdm_sel, u32 channel, ECNT_FEMGR_CHANNEL_RETIRE_MODE mode)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = FE_SET_CHANNEL_RETIRE;
    para.gdm_sel        = gdm_sel;
    para.channel        = channel;
    para.reg_val        = mode;
    
    ret = fe_lib_ioctl(FE_SET_CHANNEL_RETIRE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Retire all FrameEngine channels less than given
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] channel GDMA channel
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_channel_retire_all(ECNT_FEMGR_GDM_SEL gdm_sel, u32 channel)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = FE_SET_CHANNEL_RETIRE_ALL;
    para.gdm_sel        = gdm_sel;
    para.channel        = channel;
    
    ret = fe_lib_ioctl(FE_SET_CHANNEL_RETIRE_ALL, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Retire one FrameEngine channel
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] channel GDMA channel
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_channel_retire_one(ECNT_FEMGR_GDM_SEL gdm_sel, u32 channel)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = FE_SET_CHANNEL_RETIRE_ONE;
    para.gdm_sel        = gdm_sel;
    para.channel        = channel;
    
    ret = fe_lib_ioctl(FE_SET_CHANNEL_RETIRE_ONE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine CRC strip
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] enable enable/disable crc stripping
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_crc_strip(ECNT_FEMGR_GDM_SEL gdm_sel, u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_CRC_STRIP;
    para.gdm_sel            = gdm_sel;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(FE_SET_CRC_STRIP, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine PPE padding
@param[in] enable enable/disable PPE padding
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_padding(u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_PADDING;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(FE_SET_PADDING, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine ext TPID
@param[in] tpid TPID
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_ext_tpid(u16 tpid)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = FE_SET_EXT_TPID;
    para.reg_val        = tpid;
    
    ret = fe_lib_ioctl(FE_SET_EXT_TPID, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Get FrameEngine ext TPID
@param[out] p_tpid TPID
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_ext_tpid(u16 *p_tpid)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_EXT_TPID;
    
    ret = fe_lib_ioctl(FE_GET_EXT_TPID, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    *p_tpid = (u16)para.reg_val;
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine forward configuration
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] type packet type
@param[in] dst_port dest port of the packet
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_fw_cfg(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_PKT_TYPE type, ECNT_FEMGR_PORT dst_port)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_FW_CFG;
    para.gdm_sel = gdm_sel;
    para.api_data.fw_cfg.dp_sel = type;
    para.api_data.fw_cfg.dp_val = dst_port;
    
    ret = fe_lib_ioctl(FE_SET_FW_CFG, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine drop frame configuration
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] type frame error type
@param[in] enable enable/disable dropping
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_drop_frame_enable(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE type, u8 enable)
{
    int ret = -1, cmd;
    struct ecnt_fe_data para;

    if (ECNT_FEMGR_DROP_FRAME_TYPE_UDP_CHECKSUM_ERR == type)
    {
        cmd = FE_SET_DROP_UDP_CHKSUM_ERR_ENABLE;
    }
    else if (ECNT_FEMGR_DROP_FRAME_TYPE_TCP_CHECKSUM_ERR == type)
    {
        cmd = FE_SET_DROP_TCP_CHKSUM_ERR_ENABLE;
    }
    else if (ECNT_FEMGR_DROP_FRAME_TYPE_IP_CHECKSUM_ERR == type)
    {
        cmd = FE_SET_DROP_IP_CHKSUM_ERR_ENABLE;
    }
    else if (ECNT_FEMGR_DROP_FRAME_TYPE_CRC_ERR == type)
    {
        cmd = FE_SET_DROP_CRC_ERR_ENABLE;
    }
    else if (ECNT_FEMGR_DROP_FRAME_TYPE_RUNT == type)
    {
        cmd = FE_SET_DROP_RUNT_ENABLE;
    }
    else if (ECNT_FEMGR_DROP_FRAME_TYPE_LONG == type)
    {
        cmd = FE_SET_DROP_LONG_ENABLE;
    }
    else
    {
        return ECNT_FEMGR_INVALID_PARAM;
    }
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = cmd;
    para.gdm_sel            = gdm_sel;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine vlan checking
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] enable enable/disable vlan checking
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_vlan_check(ECNT_FEMGR_GDM_SEL gdm_sel, u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_VLAN_CHECK;
    para.gdm_sel            = gdm_sel;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(FE_SET_VLAN_CHECK, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Get FrameEngine GDMA statistics
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] type statistics type
@param[out] p_cnt counter
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_cnt(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_STATISTICS_TYPE type, u32 *p_cnt)
{
    int ret = -1, cmd, txrx_sel = 0;
    struct ecnt_fe_data para;

    if (ECNT_FEMGR_STATISTICS_TYPE_TX_OK == type)
    {
        cmd = FE_GET_OK_CNT;
        txrx_sel = FE_GDM_SEL_TX;
    }
    else if (ECNT_FEMGR_STATISTICS_TYPE_RX_OK == type)
    {
        cmd = FE_GET_OK_CNT;
        txrx_sel = FE_GDM_SEL_RX;
    }
    else if (ECNT_FEMGR_STATISTICS_TYPE_TX_BYTE == type)
    {
        cmd = FE_GET_OK_BYTE_CNT;
        txrx_sel = FE_GDM_SEL_TX;
    }
    else if (ECNT_FEMGR_STATISTICS_TYPE_RX_BYTE == type)
    {
        cmd = FE_GET_OK_BYTE_CNT;
        txrx_sel = FE_GDM_SEL_RX;
    }
    else if (ECNT_FEMGR_STATISTICS_TYPE_RX_DROP_FIFO == type)
    {
        cmd = FE_GET_RX_DROP_FIFO_CNT;
    }
    else if (ECNT_FEMGR_STATISTICS_TYPE_RX_DROP_ERR == type)
    {
        cmd = FE_GET_RX_DROP_ERR_CNT;
    }
    else if (ECNT_FEMGR_STATISTICS_TYPE_TX_GET == type)
    {
        cmd = FE_GET_TX_GET_CNT;
    }
    else if (ECNT_FEMGR_STATISTICS_TYPE_TX_DROP == type)
    {
        cmd = FE_GET_TX_DROP_CNT;
    }
    else
    {
        return ECNT_FEMGR_INVALID_PARAM;
    }
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = cmd;
    para.gdm_sel        = gdm_sel;
    para.txrx_sel       = txrx_sel;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    *p_cnt = para.cnt;
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine timestamp
@param[in] time_stamp timestamp
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_time_stamp(u32 time_stamp)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = FE_SET_TIEM_STAMP;
    para.timeStamp      = time_stamp;
    
    ret = fe_lib_ioctl(FE_SET_TIEM_STAMP, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Get FrameEngine timestamp
@param[out] p_time_stamp timestamp
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_time_stamp(u32 *p_time_stamp)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_TIEM_STAMP;
    
    ret = fe_lib_ioctl(FE_GET_TIEM_STAMP, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    *p_time_stamp = para.cnt;
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine CDM vlan enable
@param[in] cdm_sel indicate CDMA1/CDMA2
@param[in] enable enable/disable CDM vlan
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_vlan_enable(ECNT_FEMGR_CDM_SEL cdm_sel, u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_VLAN_ENABLE;
    para.cdm_sel            = cdm_sel;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(FE_SET_VLAN_ENABLE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine insert vlan TPID
@param[in] cdm_sel indicate CDMA1/CDMA2
@param[in] tpid TPID
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_insert_vlan_tpid(ECNT_FEMGR_CDM_SEL cdm_sel, u16 tpid)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = FE_SET_INS_VLAN_TPID;
    para.cdm_sel        = cdm_sel;
    para.reg_val        = tpid;
    
    ret = fe_lib_ioctl(FE_SET_INS_VLAN_TPID, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine L2B blacklist
@param[in] enable enable/disable black list
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_l2b_black_list(u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_BLACK_LIST;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(FE_SET_BLACK_LIST, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine L2B ether type
@param[in] index index of the entry
@param[in] enable enable/disable
@param[in] is_pppoe indicate pppoe or ether
@param[in] ether_type ether protocol
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_l2b_ether_type(u32 index, u8 enable, u8 is_pppoe, u16 ether_type)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                = FE_SET_ETHER_TYEP;
    para.index                      = index;
    para.api_data.eth_cfg.enable    = enable;
    para.api_data.eth_cfg.is_pppoe  = is_pppoe;
    para.api_data.eth_cfg.value     = ether_type;
    
    ret = fe_lib_ioctl(FE_SET_ETHER_TYEP, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine GDMA pcp coding mode
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] txrx_sel indicate packet direction
@param[in] mode PCP coding mode
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_gdma_pcp_coding(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_DIRECTION txrx_sel, ECNT_FEMGR_PCP_CODING_MODE mode)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = FE_SET_GDM_PCP_CODING;
    para.gdm_sel                = gdm_sel;
    para.txrx_sel               = txrx_sel;
    para.api_data.coding_mode   = mode;
    
    ret = fe_lib_ioctl(FE_SET_GDM_PCP_CODING, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine CDMA pcp coding
@param[in] cdm_sel indicate CDMA1/CDMA2
@param[in] txrx_sel indicate packet direction
@param[in] mode PCP coding mode
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_cdma_pcp_coding(ECNT_FEMGR_CDM_SEL cdm_sel, ECNT_FEMGR_DIRECTION txrx_sel, ECNT_FEMGR_PCP_CODING_MODE mode)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = FE_SET_CDM_PCP_CODING;
    para.gdm_sel                = cdm_sel;
    para.txrx_sel               = txrx_sel;
    para.api_data.coding_mode   = mode;
    
    ret = fe_lib_ioctl(FE_SET_CDM_PCP_CODING, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Get FrameEngine GDMA2 rx statistics
@param[out] p_stats GDMA2 rx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_gdma2_rx_statistics(ECNT_FEMGR_GDMA2_RX_STATISTICS *p_stats)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_ETH_RX_CNT;
    
    ret = fe_lib_ioctl(FE_GET_ETH_RX_CNT, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    p_stats->oversize           = para.api_data.FE_RxCnt.rxOversizeCnt;
    p_stats->undersize          = para.api_data.FE_RxCnt.rxUnderSizeCnt;
    p_stats->frame_cnt          = para.api_data.FE_RxCnt.rxFrameCnt;
    p_stats->frame_len          = para.api_data.FE_RxCnt.rxFrameLen;
    p_stats->drop_cnt           = para.api_data.FE_RxCnt.rxDropCnt;
    p_stats->broadcast          = para.api_data.FE_RxCnt.rxBroadcastCnt;
    p_stats->multicast          = para.api_data.FE_RxCnt.rxMulticastCnt;
    p_stats->crc                = para.api_data.FE_RxCnt.rxCrcCnt;
    p_stats->fragment           = para.api_data.FE_RxCnt.rxFragFameCnt;
    p_stats->jabber             = para.api_data.FE_RxCnt.rxJabberFameCnt;
    p_stats->less_64            = para.api_data.FE_RxCnt.rxLess64Cnt;
    p_stats->more_1518          = para.api_data.FE_RxCnt.rxMore1518Cnt;
    p_stats->eq_64              = para.api_data.FE_RxCnt.rxEq64Cnt;
    p_stats->from_65_to_127     = para.api_data.FE_RxCnt.rxFrom65To127Cnt;
    p_stats->from_128_to_255    = para.api_data.FE_RxCnt.rxFrom128To255Cnt;
    p_stats->from_256_to_511    = para.api_data.FE_RxCnt.rxFrom256To511Cnt;
    p_stats->from_512_to_1023   = para.api_data.FE_RxCnt.rxFrom512To1023Cnt;
    p_stats->from_1024_to_1518  = para.api_data.FE_RxCnt.rxFrom1024To1518Cnt;
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Get FrameEngine GDMA2 tx statistics
@param[out] p_stats GDMA2 tx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_gdma2_tx_statistics(ECNT_FEMGR_GDMA2_TX_STATISTICS *p_stats)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_ETH_TX_CNT;
    
    ret = fe_lib_ioctl(FE_GET_ETH_TX_CNT, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    p_stats->frame_cnt          = para.api_data.FE_TxCnt.txFrameCnt;
    p_stats->frame_len          = para.api_data.FE_TxCnt.txFrameLen;
    p_stats->drop_cnt           = para.api_data.FE_TxCnt.txDropCnt;
    p_stats->broadcast          = para.api_data.FE_TxCnt.txBroadcastCnt;
    p_stats->multicast          = para.api_data.FE_TxCnt.txMulticastCnt;
    p_stats->less_64            = para.api_data.FE_TxCnt.txLess64Cnt;
    p_stats->more_1518          = para.api_data.FE_TxCnt.txMore1518Cnt;
    p_stats->eq_64              = para.api_data.FE_TxCnt.txEq64Cnt;
    p_stats->from_65_to_127     = para.api_data.FE_TxCnt.txFrom65To127Cnt;
    p_stats->from_128_to_255    = para.api_data.FE_TxCnt.txFrom128To255Cnt;
    p_stats->from_256_to_511    = para.api_data.FE_TxCnt.txFrom256To511Cnt;
    p_stats->from_512_to_1023   = para.api_data.FE_TxCnt.txFrom512To1023Cnt;
    p_stats->from_1024_to_1518  = para.api_data.FE_TxCnt.txFrom1024To1518Cnt;
    
    return ECNT_FEMGR_SUCCESS;
}




/**
@brief Get FrameEngine hsgmii lan rx statistics
@param[in] hsgmii_idx 0:pcie0, 1:pcie1, 2:usb, 3:eth(en7581) as hsgmii lan
@param[out] p_stats Hsgmii rx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_hsgmii_rx_statistics(ECNT_FEMGR_GDMA2_RX_STATISTICS *p_stats, int hsgmii_idx)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_HSGMII_LAN_RX_CNT;
    para.index = hsgmii_idx;
    ret = fe_lib_ioctl(FE_GET_HSGMII_LAN_RX_CNT, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    p_stats->oversize           = para.api_data.FE_RxCnt.rxOversizeCnt;
    p_stats->undersize          = para.api_data.FE_RxCnt.rxUnderSizeCnt;
    p_stats->frame_cnt          = para.api_data.FE_RxCnt.rxFrameCnt;
    p_stats->frame_len          = para.api_data.FE_RxCnt.rxFrameLen;
    p_stats->drop_cnt           = para.api_data.FE_RxCnt.rxDropCnt;
    p_stats->broadcast          = para.api_data.FE_RxCnt.rxBroadcastCnt;
    p_stats->multicast          = para.api_data.FE_RxCnt.rxMulticastCnt;
    p_stats->crc                = para.api_data.FE_RxCnt.rxCrcCnt;
    p_stats->fragment           = para.api_data.FE_RxCnt.rxFragFameCnt;
    p_stats->jabber             = para.api_data.FE_RxCnt.rxJabberFameCnt;
    p_stats->less_64            = para.api_data.FE_RxCnt.rxLess64Cnt;
    p_stats->more_1518          = para.api_data.FE_RxCnt.rxMore1518Cnt;
    p_stats->eq_64              = para.api_data.FE_RxCnt.rxEq64Cnt;
    p_stats->from_65_to_127     = para.api_data.FE_RxCnt.rxFrom65To127Cnt;
    p_stats->from_128_to_255    = para.api_data.FE_RxCnt.rxFrom128To255Cnt;
    p_stats->from_256_to_511    = para.api_data.FE_RxCnt.rxFrom256To511Cnt;
    p_stats->from_512_to_1023   = para.api_data.FE_RxCnt.rxFrom512To1023Cnt;
    p_stats->from_1024_to_1518  = para.api_data.FE_RxCnt.rxFrom1024To1518Cnt;
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Get FrameEngine hsgmii lan tx statistics
@param[in] hsgmii_idx 0:pcie 0 1 :pcie 1 2 :usb as hsgmii lan
@param[out] p_stats Hsgmii tx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_hsgmii_tx_statistics(ECNT_FEMGR_GDMA2_TX_STATISTICS *p_stats, int hsgmii_idx)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_HSGMII_LAN_TX_CNT;
    para.index = hsgmii_idx;
    ret = fe_lib_ioctl(FE_GET_HSGMII_LAN_TX_CNT, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    p_stats->frame_cnt          = para.api_data.FE_TxCnt.txFrameCnt;
    p_stats->frame_len          = para.api_data.FE_TxCnt.txFrameLen;
    p_stats->drop_cnt           = para.api_data.FE_TxCnt.txDropCnt;
    p_stats->broadcast          = para.api_data.FE_TxCnt.txBroadcastCnt;
    p_stats->multicast          = para.api_data.FE_TxCnt.txMulticastCnt;
    p_stats->less_64            = para.api_data.FE_TxCnt.txLess64Cnt;
    p_stats->more_1518          = para.api_data.FE_TxCnt.txMore1518Cnt;
    p_stats->eq_64              = para.api_data.FE_TxCnt.txEq64Cnt;
    p_stats->from_65_to_127     = para.api_data.FE_TxCnt.txFrom65To127Cnt;
    p_stats->from_128_to_255    = para.api_data.FE_TxCnt.txFrom128To255Cnt;
    p_stats->from_256_to_511    = para.api_data.FE_TxCnt.txFrom256To511Cnt;
    p_stats->from_512_to_1023   = para.api_data.FE_TxCnt.txFrom512To1023Cnt;
    p_stats->from_1024_to_1518  = para.api_data.FE_TxCnt.txFrom1024To1518Cnt;
    
    return ECNT_FEMGR_SUCCESS;
}


/**
@brief Clear FrameEngine GDMA statistics
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] txrx_sel packet direction
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_clear_gdma_statistics(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_DIRECTION txrx_sel)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id    = FE_SET_CLEAR_MIB;
    para.gdm_sel        = gdm_sel;
    para.txrx_sel       = txrx_sel;
    
    ret = fe_lib_ioctl(FE_SET_CLEAR_MIB, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine GDMA2 tx shaping
@param[in] mode shaping mode
@param[in] rate shaping rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE mode, u32 rate)
{
    int ret;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
	para.gdm_sel = FE_GDM_SEL_GDMA2;
    para.function_id            = FE_SET_TX_RATE;
    para.api_data.rate_cfg.rate = rate;
    para.api_data.rate_cfg.mode = mode;
    
    ret = fe_lib_ioctl(FE_SET_TX_RATE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine GDMA2 rx shaping
@param[in] type pkt type
@param[in] mode shaping mode
@param[in] rate shaping rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_gdma2_rx_rate(ECNT_FEMGR_PKT_TYPE type, ECNT_FEMGR_GDM_SHAPING_MODE mode, u32 rate)
{
    int ret, cmd;
    struct ecnt_fe_data para;

    if (ECNT_FEMGR_PKT_TYPE_OTHERCAST == type)
    {
        cmd = FE_SET_RXOC_RATE;
    }
    else if (ECNT_FEMGR_PKT_TYPE_MULTICAST == type)
    {
        cmd = FE_SET_RXMC_RATE;
    }
    else if (ECNT_FEMGR_PKT_TYPE_BROADCAST == type)
    {
        cmd = FE_SET_RXBC_RATE;
    }
    else if (ECNT_FEMGR_PKT_TYPE_MYMAC == type)
    {
        cmd = FE_SET_RXUC_RATE;
    }
	else if (ECNT_FEMGR_PKT_TYPE_MAC_FILTER == type)
    {
        cmd = FE_SET_RX_MAC_FILTER_RATE;
    }
    else
    {
        return ECNT_FEMGR_INVALID_PARAM;
    }

    memset(&para,0,sizeof(struct ecnt_fe_data));
	/* forward compatible, default gdm2 */
	para.gdm_sel = FE_GDM_SEL_GDMA2;
    para.function_id            = cmd;
    para.api_data.rate_cfg.rate = rate;
    para.api_data.rate_cfg.mode = mode;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine VIP
@param[in] index index of the entry
@param[in] enable enable/disable VIP entry
@param[in] pattern_type VIP pattern type
@param[in] pattern VIP pattern
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_vip(u8 index, u8 enable, ECNT_FEMGR_VIP_PATTERN_TYPE pattern_type, u32 pattern)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_SET_VIP_ENABLE;
    para.index                          = index;
    para.api_data.vip_cfg.enable        = enable;
    para.api_data.vip_cfg.patten_type   = pattern_type;
    para.api_data.vip_cfg.patten        = pattern;
    
    ret = fe_lib_ioctl(FE_SET_VIP_ENABLE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Add FrameEngine VIP entry
@param[in] p_vip VIP entry
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_add_vip(ECNT_FEMGR_VIP_ENTRY *p_vip)
{
    int ret, cmd, mode = 0;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));

    if (ECNT_FEMGR_VIP_PATTERN_TYPE_ETHER_TYPE == p_vip->type)
    {
        cmd = FE_ADD_VIP_ETHER;
        para.api_data.vip.type  = p_vip->u.ether_type;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_PPPOE_PROTOCOL == p_vip->type)
    {
        cmd = FE_ADD_VIP_PPP;
        para.api_data.vip.type  = p_vip->u.pppoe_protocol;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_IP_PROTOCOL == p_vip->type)
    {
        cmd = FE_ADD_VIP_IP;
        para.api_data.vip.type  = p_vip->u.ip_protocol;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS == p_vip->type)
    {
        cmd = FE_ADD_VIP_TCP;
        para.api_data.vip.type  = p_vip->type;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS == p_vip->type)
    {
        cmd = FE_ADD_VIP_UDP;
        para.api_data.vip.type  = p_vip->type;
    }
    else
    {
        return ECNT_FEMGR_INVALID_PARAM;
    }

    if ((ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS == p_vip->type)
        || (ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS == p_vip->type))
    {
        if  (p_vip->u.tcp.src_port_en)
        {
            mode |= 0x1;
        }
        if  (p_vip->u.tcp.dst_port_en)
        {
            mode |= 0x2;
        }
        if  (p_vip->u.tcp.swap_port_en)
        {
            mode |= 0x4;
        }
    }
    
    para.function_id        = cmd;
    para.api_data.vip.src   = p_vip->u.tcp.src_port;
    para.api_data.vip.dst   = p_vip->u.tcp.dst_port;
    para.api_data.vip.mode  = mode;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Del FrameEngine VIP entry
@param[in] p_vip VIP entry
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_del_vip(ECNT_FEMGR_VIP_ENTRY *p_vip)
{
    int ret, cmd, mode = 0;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));

    if (ECNT_FEMGR_VIP_PATTERN_TYPE_ETHER_TYPE == p_vip->type)
    {
        cmd = FE_DEL_VIP_ETHER;
        para.api_data.vip.type  = p_vip->u.ether_type;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_PPPOE_PROTOCOL == p_vip->type)
    {
        cmd = FE_DEL_VIP_PPP;
        para.api_data.vip.type  = p_vip->u.pppoe_protocol;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_IP_PROTOCOL == p_vip->type)
    {
        cmd = FE_DEL_VIP_IP;
        para.api_data.vip.type  = p_vip->u.ip_protocol;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS == p_vip->type)
    {
        cmd = FE_DEL_VIP_TCP;
        para.api_data.vip.type  = p_vip->type;
    }
    else if (ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS == p_vip->type)
    {
        cmd = FE_DEL_VIP_UDP;
        para.api_data.vip.type  = p_vip->type;
    }
    else
    {
        return ECNT_FEMGR_INVALID_PARAM;
    }

    if ((ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS == p_vip->type)
        || (ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS == p_vip->type))
    {
        if  (p_vip->u.tcp.src_port_en)
        {
            mode |= 0x1;
        }
        if  (p_vip->u.tcp.dst_port_en)
        {
            mode |= 0x2;
        }
        if  (p_vip->u.tcp.swap_port_en)
        {
            mode |= 0x4;
        }
    }
    
    para.function_id        = cmd;
    para.api_data.vip.src   = p_vip->u.tcp.src_port;
    para.api_data.vip.dst   = p_vip->u.tcp.dst_port;
    para.api_data.vip.mode  = mode;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Add FrameEngine L2LU key
@param[in] p_l2lu_key L2LU key
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_add_l2lu_key(ECNT_FEMGR_L2LU_KEY_ENTRY *p_l2lu_key)
{
    int ret, cmd, mask = 0;
    struct ecnt_fe_data para;

    if (p_l2lu_key->is_dscp)
    {
        cmd = FE_ADD_L2LU_VLAN_DSCP;
    }
    else
    {
        cmd = FE_ADD_L2LU_VLAN_TRFC;
    }

    if (p_l2lu_key->svid_en)
    {
        mask |= 0x1;
    }

    if (p_l2lu_key->spcp_en)
    {
        mask |= 0x2;
    }

    if (p_l2lu_key->svid_en)
    {
        mask |= 0x4;
    }

    if (p_l2lu_key->spcp_en)
    {
        mask |= 0x8;
    }

    if (p_l2lu_key->dscp_mask_en)
    {
        mask |= 0x10;
    }
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = cmd;
    para.api_data.l2lu.svlan    = p_l2lu_key->svid;
    para.api_data.l2lu.cvlan    = p_l2lu_key->cvid;
    para.api_data.l2lu.dscp     = p_l2lu_key->dscp_or_traffic_class;
    para.api_data.l2lu.mask     = mask;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Del FrameEngine L2LU key
@param[in] p_l2lu_key L2LU key
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_del_l2lu_key(ECNT_FEMGR_L2LU_KEY_ENTRY *p_l2lu_key)
{
    int ret, cmd, mask = 0;
    struct ecnt_fe_data para;

    if (p_l2lu_key->is_dscp)
    {
        cmd = FE_DEL_L2LU_VLAN_DSCP;
    }
    else
    {
        cmd = FE_DEL_L2LU_VLAN_TRFC;
    }

    if (p_l2lu_key->svid_en)
    {
        mask |= 0x1;
    }

    if (p_l2lu_key->spcp_en)
    {
        mask |= 0x2;
    }

    if (p_l2lu_key->svid_en)
    {
        mask |= 0x4;
    }

    if (p_l2lu_key->spcp_en)
    {
        mask |= 0x8;
    }

    if (p_l2lu_key->dscp_mask_en)
    {
        mask |= 0x10;
    }
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = cmd;
    para.api_data.l2lu.svlan    = p_l2lu_key->svid;
    para.api_data.l2lu.cvlan    = p_l2lu_key->cvid;
    para.api_data.l2lu.dscp     = p_l2lu_key->dscp_or_traffic_class;
    para.api_data.l2lu.mask     = mask;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine tx favor oam
@param[in] enable enable/disable tx favor oam
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_tx_favor_oam_enable(u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_TX_FAVOR_OAM_ENABLE;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(FE_SET_TX_FAVOR_OAM_ENABLE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine TLS lan port
@param[in] enable enable/disable TLS lan port
@param[in] lan_port lan port id
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_tls_port(u8 enable, u8 lan_port)
{
    int ret;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = FE_SET_TLS_CFG;
    para.api_data.tls_cfg.cmd   = FE_DEL_TLS_PORT;
    
    ret = fe_lib_ioctl(FE_DEL_TLS_PORT, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    if (enable)
    {
        memset(&para,0,sizeof(struct ecnt_fe_data));
        para.function_id                = FE_SET_TLS_CFG;
        para.api_data.tls_cfg.cmd       = FE_ADD_TLS_PORT ;
        para.api_data.tls_cfg.tls_port  = lan_port;
        
        ret = fe_lib_ioctl(FE_ADD_TLS_PORT, &para);
        if(ret < 0)
        {
            return ECNT_FEMGR_FAIL;
        }
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine TLS vlan
@param[in] enable enable/disable TLS vlan
@param[in] vlan vlan id
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_tls_vlan(u8 enable, u16 vlan)
{
    int ret;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = FE_SET_TLS_CFG;
    para.api_data.tls_cfg.cmd   = FE_UNSET_TLS_VLAN;
    
    ret = fe_lib_ioctl(FE_UNSET_TLS_VLAN, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    if (enable)
    {
        memset(&para,0,sizeof(struct ecnt_fe_data));
        para.function_id                = FE_SET_TLS_CFG;
        para.api_data.tls_cfg.cmd       = FE_SET_TLS_VLAN ;
        para.api_data.tls_cfg.tls_vlan  = vlan;
        
        ret = fe_lib_ioctl(FE_SET_TLS_VLAN, &para);
        if(ret < 0)
        {
            return ECNT_FEMGR_FAIL;
        }
    }
    
    return ECNT_FEMGR_SUCCESS;
}

static int fe_lib_tls_common(int cmd,unchar lan_port,ushort tls_vlan)
{
	int ret;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = FE_SET_TLS_CFG;
	para.api_data.tls_cfg.cmd = cmd ;
	para.api_data.tls_cfg.tls_port = lan_port;
	para.api_data.tls_cfg.tls_vlan = tls_vlan;
	ret = fe_lib_ioctl(cmd,&para);
	return ret;
}

ECNT_FEMGR_RET fe_lib_group_tls_entry_add(unsigned char port_id, unsigned short vlan_tci)
{
	fe_lib_tls_common(FE_ADD_GROUP_TLS_ENTRY,port_id,vlan_tci);
	return 0;
}

ECNT_FEMGR_RET fe_lib_group_tls_entry_del(unsigned char port_id, unsigned short vlan_tci)
{
	fe_lib_tls_common(FE_DEL_GROUP_TLS_ENTRY,port_id,vlan_tci);
	return 0;
}

ECNT_FEMGR_RET fe_lib_group_tls_entry_show(void)
{
	fe_lib_tls_common(FE_SHOW_GROUP_TLS_ENTRY,0,0);
	return 0;
}

ECNT_FEMGR_RET fe_lib_tls_entry_common(int cmd, ECNT_FEMGR_TLS_ENTRY *p_tls_info)
{
    int ret;
    struct ecnt_fe_data para;

#ifndef TCSUPPORT_CSC_EEUROPE
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                            = FE_SET_TLS_CFG;
    para.api_data.tls_cfg.cmd                   = cmd ;
    para.api_data.tls_cfg.tls_info.uni_port_id  = p_tls_info->uni_port_id;
    para.api_data.tls_cfg.tls_info.uni_vlan     = p_tls_info->uni_vlan;
    para.api_data.tls_cfg.tls_info.gem_port     = p_tls_info->gem_port;
    para.api_data.tls_cfg.tls_info.ani_vlan     = p_tls_info->ani_vlan;
    
    ret = fe_lib_ioctl(cmd, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#endif
}

/**
@brief Add Transparet Line Service entry
@param[in] p_tls Transparet Line Service configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_add_tls_entry(ECNT_FEMGR_TLS_ENTRY *p_tls)
{
    return fe_lib_tls_entry_common(FE_ADD_TLS_ENTRY, p_tls);  
}

/**
@brief Del Transparet Line Service entry
@param[in] p_tls Transparet Line Service configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_del_tls_entry(ECNT_FEMGR_TLS_ENTRY *p_tls)
{
    return fe_lib_tls_entry_common(FE_DEL_TLS_ENTRY, p_tls);  
}

ECNT_FEMGR_RET fe_lib_show_tls_entry(void)
{
    ECNT_FEMGR_TLS_ENTRY tls = {0};
    return fe_lib_tls_entry_common(FE_SHOW_TLS_ENTRY, &tls);
}

/**
@brief Reset FrameEngine
@param[in] reset_mode FrameEngine reset mode
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_reset_frame_engine(ECNT_FEMGR_RESET_MODE reset_mode)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id            = FE_DO_RESET;
    para.api_data.reset_mode    = reset_mode;
    
    ret = fe_lib_ioctl(FE_DO_RESET, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

/**
@brief Set FrameEngine meter ratelimit
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_meter_ratelimit(ECNT_FEMGR_METER *p_meter)
{
    int ret;
    struct ecnt_fe_data para;

#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_SET_METER_RATELIMIT;
    para.api_data.meter_cfg.rate        = p_meter->rate;
    para.api_data.meter_cfg.enable      = p_meter->enable;
    para.api_data.meter_cfg.pkt_mode    = p_meter->rate_mode;
    para.api_data.meter_cfg.tick_sel    = p_meter->tick_mode;
    para.api_data.meter_cfg.idx         = p_meter->index;
    if(p_meter->dir == ECNT_FEMGR_TRAFFIC_DIRECTON_UPSTREAM)
        para.api_data.meter_cfg.dir     = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir     = UP_STREAM;
    para.api_data.meter_cfg.mode        = p_meter->mode;
    para.api_data.meter_cfg.bucket_size = p_meter->bucket_size;
    memcpy(para.api_data.meter_cfg.itf_name, p_meter->itf_name, 16);

    ret = fe_lib_ioctl(FE_SET_METER_RATELIMIT, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#endif
}

/**
@brief Set FrameEngine meter ratelimit by ip or itf
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_meter_ratelimit_by(ECNT_FEMGR_METER_INFO *p_meter_info)
{

#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    ECNT_FEMGR_METER p_meter = {0};

    p_meter.bucket_size = 0;
    p_meter.tick_mode = ECNT_FEMGR_METER_TICK_MODE_FAST;
    p_meter.rate_mode = ECNT_FEMGR_METER_RATE_MODE_BPS;
    
    p_meter.enable = p_meter_info->enable;
    p_meter.rate = p_meter_info->rate;
    if(p_meter_info->mode == 1)
        p_meter.mode = ECNT_FEMGR_METER_MODE_IP;
    else
        p_meter.mode = ECNT_FEMGR_METER_MODE_IF;
    p_meter.index = p_meter_info->index;
    
    if(p_meter_info->dir == 1)
        p_meter.dir = ECNT_FEMGR_TRAFFIC_DIRECTON_DOWNSTREAM;
    else
        p_meter.dir = ECNT_FEMGR_TRAFFIC_DIRECTON_UPSTREAM;
    
    return fe_lib_set_meter_ratelimit(&p_meter);
#endif
}


/**
@brief Set FrameEngine meter ratelimit rule
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_rx_ratelimit_rule(ECNT_FEMGR_METER *p_meter)
{
    int ret;
    struct ecnt_fe_data para;

#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_SET_RX_RATELIMIT_RULE;
    para.api_data.meter_cfg.idx         = p_meter->index;
    para.api_data.meter_cfg.mode        = p_meter->mode;
    if(p_meter->dir == ECNT_FEMGR_TRAFFIC_DIRECTON_UPSTREAM)
        para.api_data.meter_cfg.dir     = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir     = UP_STREAM;

    memcpy(&(para.api_data.meter_cfg.if_info), &(p_meter->if_info), sizeof(ECNT_FEMGR_RATELIMIT_IF_INFO));
    memcpy(&(para.api_data.meter_cfg.ip_info), &(p_meter->ip_info), sizeof(ECNT_FEMGR_RATELIMIT_IP_INFO));
    //para.api_data.meter_cfg.if_info     = p_meter->if_info;
    //para.api_data.meter_cfg.ip_info     = p_meter->ip_info;

    ret = fe_lib_ioctl(FE_SET_RX_RATELIMIT_RULE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#endif
}


/**
@brief Set FrameEngine meter ratelimit rule by ip 
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_ip_ratelimit_rule(ECNT_FEMGR_RATELIMIT_IP_INFO *ip_info, u32 dir)
{
    int ret;
    struct ecnt_fe_data para;

#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    ECNT_FEMGR_METER p_meter = {0};
    if(dir == 1)
        p_meter.dir = ECNT_FEMGR_TRAFFIC_DIRECTON_DOWNSTREAM;
    else
        p_meter.dir = ECNT_FEMGR_TRAFFIC_DIRECTON_UPSTREAM;

    p_meter.index = ip_info->index;
    p_meter.mode = ECNT_FEMGR_METER_MODE_IP;
    memcpy(&(p_meter.ip_info), ip_info, sizeof(ECNT_FEMGR_RATELIMIT_IP_INFO));

    return fe_lib_set_rx_ratelimit_rule(&p_meter);

#endif
}


/**
@brief Set FrameEngine meter ratelimit rule by ip 
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_if_ratelimit_rule(ECNT_FEMGR_RATELIMIT_IF_INFO *if_info, u32 dir)
{
    int ret;
    struct ecnt_fe_data para;

#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    ECNT_FEMGR_METER p_meter = {0};
    if(dir == 1)
        p_meter.dir = ECNT_FEMGR_TRAFFIC_DIRECTON_DOWNSTREAM;
    else
        p_meter.dir = ECNT_FEMGR_TRAFFIC_DIRECTON_UPSTREAM;

    p_meter.index = if_info->index;
    p_meter.mode = ECNT_FEMGR_METER_MODE_IP;
    memcpy(&(p_meter.if_info), if_info, sizeof(ECNT_FEMGR_RATELIMIT_IF_INFO));

    return fe_lib_set_rx_ratelimit_rule(&p_meter);

#endif
}

/**
@brief Get FrameEngine meter ratelimit mode
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_meter_ratelimit_mode(u32 dir, u32 idx, u32 *ratelimit_mode)
{
	int ret = 0;
	int cmd = 0;
	struct ecnt_fe_data para;
	
#ifndef TCSUPPORT_CPU_EN7580
	return ECNT_FEMGR_NOT_SUPPORT;
#else
	memset(&para,0,sizeof(struct ecnt_fe_data));
	cmd = FE_GET_RX_RATELIMIT_MODE;
	para.function_id = FE_GET_RX_RATELIMIT_MODE;
	para.api_data.meter_cfg.idx = idx;

    if(dir == 0)
        para.api_data.meter_cfg.dir = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir = UP_STREAM;	

	ret = fe_lib_ioctl(cmd, &para);

	if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

	*ratelimit_mode = para.api_data.meter_cfg.mode;
    
    return ECNT_FEMGR_SUCCESS;
#endif
}

/**
@brief Set FrameEngine meter ratelimit mode
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_meter_ratelimit_mode(u32 dir, u32 idx, u32 mode)
{
    int ret;
    struct ecnt_fe_data para;

#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_SET_RX_RATELIMIT_MODE;
    para.api_data.meter_cfg.idx = idx;
    if(dir == 0)
        para.api_data.meter_cfg.dir     = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir     = UP_STREAM;

    if((mode == 0)&&(dir == 1))
        para.api_data.meter_cfg.mode = CAR_QOS_MODE;
    else if(mode == 1)
        para.api_data.meter_cfg.mode = IP_BIND_MODE;
    else if(mode == 2)
        para.api_data.meter_cfg.mode = IF_BIND_MODE;
    else if(mode == 3)
        para.api_data.meter_cfg.mode = OLT_CTL_MODE;
    else if(mode == 4)
		para.api_data.meter_cfg.mode = TRAFFIC_QOS_MODE;
	else if(mode == 5)
		para.api_data.meter_cfg.mode = DEV_MAC_MODE;
    else if(mode == 6)
        para.api_data.meter_cfg.mode = FORCEBW_WRR_MODE;
	else if(mode == 7)
		para.api_data.meter_cfg.mode = WAN_MODE;
	else
        para.api_data.meter_cfg.mode = NULL_BIND_MODE;

    ret = fe_lib_ioctl(FE_SET_RX_RATELIMIT_MODE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#endif
}

/**
@brief Get FrameEngine meter ratelimit value
@param[in] p_meter meter ratelimit configuration 
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_meter_ratelimit(u32 idx, u32 dir, u32 mode, u32 *rate)
{
    int ret;
    struct ecnt_fe_data para;

#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_GET_METER_RATELIMIT;
    if(dir == 0)
        para.api_data.meter_cfg.dir     = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir     = UP_STREAM;

    
    if(mode == 1)
        para.api_data.meter_cfg.mode    = IP_BIND_MODE;
    else if(mode == 2)
        para.api_data.meter_cfg.mode    = IF_BIND_MODE;
    else
        para.api_data.meter_cfg.mode    = NULL_BIND_MODE;

    para.api_data.meter_cfg.idx         = idx;

    ret = fe_lib_ioctl(FE_GET_METER_RATELIMIT, &para);
    *rate = para.api_data.meter_cfg.rate;
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#endif
}

ECNT_FEMGR_RET fe_lib_get_acnt1_mode(uint dir, uint *acnt1_mode)
{
    int ret = 0; 
    int cmd = 0;
    struct ecnt_fe_data para;
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    cmd = FE_GET_ACNT1_MODE;
    para.function_id = FE_GET_ACNT1_MODE;

    if(dir == 0)
        para.api_data.meter_cfg.dir = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir = UP_STREAM;
    
    ret = fe_lib_ioctl(cmd,&para);
    *acnt1_mode = para.api_data.meter_cfg.acnt_mode;
#endif
    return ret;
}

/**
@brief Get FrameEngine flow byte count
@param[in] p_meter meter ratelimit configuration 
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_flow_byte_cnt(uint dir, uint flowid, unsigned long long *cnt)
{
    int ret;
    struct ecnt_fe_data para;
	uint acnt_mode = TRAFFIC_FLOW_MODE;
	unsigned long long tmp = 0;

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_GET_FLOW_CNT;
    
    if(dir == 0)
        para.api_data.meter_cfg.dir     = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir     = UP_STREAM;
    
    para.api_data.meter_cfg.idx         = flowid;

#if !defined(TCSUPPORT_ACCOUNT_METER_V2)
#if defined(TCSUPPORT_CPU_EN7580)
	fe_lib_get_acnt1_mode(dir, &acnt_mode);
	if(WAN_ITF_AND_TRAFFIC_FLOW_MODE == acnt_mode)
		para.api_data.meter_cfg.idx += WAN_ITF_HAS_FLOW_ACNT_NUM;
#endif
#endif
	para.api_data.meter_cfg.acnt_mode = acnt_mode;

    ret = fe_lib_ioctl(FE_GET_FLOW_CNT, &para);
	tmp = (unsigned long long)para.api_data.meter_cfg.byteCnt_H;
    *cnt = para.api_data.meter_cfg.byteCnt_L|(tmp<<32) ;
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return fe_api_get_flow_byte_cnt(dir,flowid,cnt);
#endif
}

ECNT_FEMGR_RET fe_lib_get_flow_byte_pkt_cnt(uint dir, uint flowid, unsigned long long *byte_cnt, unsigned long long *pkt_cnt)
{
	int ret;
    struct ecnt_fe_data para;
	uint acnt_mode = TRAFFIC_FLOW_MODE;
	unsigned long long byte_tmp = 0;
	unsigned long long pkt_tmp = 0;

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_GET_FLOW_CNT;
    
    if(dir == 0)
        para.api_data.meter_cfg.dir     = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir     = UP_STREAM;
    
    para.api_data.meter_cfg.idx         = flowid;

#if !defined(TCSUPPORT_ACCOUNT_METER_V2)
#if defined(TCSUPPORT_CPU_EN7580)
	fe_lib_get_acnt1_mode(dir, &acnt_mode);
	if(WAN_ITF_AND_TRAFFIC_FLOW_MODE == acnt_mode)
		para.api_data.meter_cfg.idx += WAN_ITF_HAS_FLOW_ACNT_NUM;
#endif
#endif
	para.api_data.meter_cfg.acnt_mode = acnt_mode;

    ret = fe_lib_ioctl(FE_GET_FLOW_CNT, &para);
	byte_tmp = (unsigned long long)para.api_data.meter_cfg.byteCnt_H;
	pkt_tmp = (unsigned long long)para.api_data.meter_cfg.pktCnt_H;
    *byte_cnt = para.api_data.meter_cfg.byteCnt_L|(byte_tmp<<32) ;
	*pkt_cnt = para.api_data.meter_cfg.pktCnt|(pkt_tmp<<32) ;
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
	return ECNT_FEMGR_FAIL;
#endif
}

ECNT_FEMGR_RET fe_lib_get_llid_byte_cnt(uint dir, uint llid, unsigned long long *cnt)
{
    int ret;
    struct ecnt_fe_data para;
	uint acnt_mode = LLID_MODE;
	unsigned long long tmp = 0;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                    = FE_GET_FLOW_CNT;
    
    if(dir == 0)
        para.api_data.meter_cfg.dir     = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir     = UP_STREAM;
    
    para.api_data.meter_cfg.idx         = llid;

	para.api_data.meter_cfg.acnt_mode = acnt_mode;

    ret = fe_lib_ioctl(FE_GET_FLOW_CNT, &para);
	tmp = (unsigned long long)para.api_data.meter_cfg.byteCnt_H;
    *cnt = para.api_data.meter_cfg.byteCnt_L|(tmp<<32) ;
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_get_rx_err_crc_cnt()
{
	int ret = -1;
	struct ecnt_fe_data para;
	unsigned int cnt = 0;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = FE_GET_RX_CRC_ERR_CNT;
	ret = fe_lib_ioctl(FE_GET_RX_CRC_ERR_CNT,&para);
	printf("[%s]%d:ret = %d\n",__FUNCTION__ , __LINE__ , ret);
	if(ret == 0)
	{
		cnt = para.cnt;
		printf("[%s]%d:rx_err_crc_cnt = %d\n",__FUNCTION__ , __LINE__ , cnt);
		return ECNT_FEMGR_SUCCESS;
	}
	else
	{
		return ECNT_FEMGR_FAIL;
	}
}
ECNT_FEMGR_RET fe_lib_get_eth_cnt_common(int cmd)
{
	int ret = -1;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = cmd;
	ret = fe_lib_ioctl(cmd,&para);
	if(ret == 0)
	{
		if(cmd == FE_GET_ETH_RX_CNT )
		{
			printf("\nrxFrameCnt = %llu\n"
                "rxFrameLen = %llu\n"
                "rxDropCnt = %llu\n"
                "rxBroadcastCnt = %llu\n"
                "rxMulticastCnt = %llu\n"
                "rxCrcCnt = %llu\n"
                "rxFragFameCnt = %llu\n"
                "rxJabberFameCnt = %llu\n"
                "rxLess64Cnt = %llu\n"
                "rxMore1518Cnt = %llu\n"
                "rxEq64Cnt = %llu\n"
                "rxFrom65To127Cnt = %llu\n"
                "rxFrom128To255Cnt = %llu\n"
                "rxFrom256To511Cnt = %llu\n"
                "rxFrom512To1023Cnt = %llu\n"
                "rxFrom1024To1518Cnt = %llu\n", 
				para.api_data.FE_RxCnt.rxFrameCnt, 
				para.api_data.FE_RxCnt.rxFrameLen, 
				para.api_data.FE_RxCnt.rxDropCnt, 
				para.api_data.FE_RxCnt.rxBroadcastCnt, 
				para.api_data.FE_RxCnt.rxMulticastCnt, 
				para.api_data.FE_RxCnt.rxCrcCnt, 
				para.api_data.FE_RxCnt.rxFragFameCnt, 
				para.api_data.FE_RxCnt.rxJabberFameCnt, 
				para.api_data.FE_RxCnt.rxLess64Cnt, 
				para.api_data.FE_RxCnt.rxMore1518Cnt, 
				para.api_data.FE_RxCnt.rxEq64Cnt, 
				para.api_data.FE_RxCnt.rxFrom65To127Cnt, 
				para.api_data.FE_RxCnt.rxFrom128To255Cnt, 
				para.api_data.FE_RxCnt.rxFrom256To511Cnt, 
				para.api_data.FE_RxCnt.rxFrom512To1023Cnt, 
				para.api_data.FE_RxCnt.rxFrom1024To1518Cnt);
		}
		else
		{		
			printf("\ntxFrameCnt = %llu\n"
                "txFrameLen = %llu\n"
                "txDropCnt = %llu\n"
                "txBroadcastCnt = %llu\n"
                "txMulticastCnt = %llu\n"
                "txLess64Cnt = %llu\n"
                "txMore1518Cnt = %llu\n"
                "txEq64Cnt = %llu\n"
                "txFrom65To127Cnt = %llu\n"
                "txFrom128To255Cnt = %llu\n"
                "txFrom256To511Cnt = %llu\n"
                "txFrom512To1023Cnt = %llu\n"
                "txFrom1024To1518Cnt = %llu\n", 
				para.api_data.FE_TxCnt.txFrameCnt, 
				para.api_data.FE_TxCnt.txFrameLen, 
				para.api_data.FE_TxCnt.txDropCnt, 
				para.api_data.FE_TxCnt.txBroadcastCnt, 
				para.api_data.FE_TxCnt.txMulticastCnt, 
				para.api_data.FE_TxCnt.txLess64Cnt, 
				para.api_data.FE_TxCnt.txMore1518Cnt, 
				para.api_data.FE_TxCnt.txEq64Cnt, 
				para.api_data.FE_TxCnt.txFrom65To127Cnt, 
				para.api_data.FE_TxCnt.txFrom128To255Cnt, 
				para.api_data.FE_TxCnt.txFrom256To511Cnt, 
				para.api_data.FE_TxCnt.txFrom512To1023Cnt, 
				para.api_data.FE_TxCnt.txFrom1024To1518Cnt);
		}
		return ECNT_FEMGR_SUCCESS;
	}
	else
	{
		return ECNT_FEMGR_FAIL;
	}
}

ECNT_FEMGR_RET fe_lib_get_eth_rx_cnt()
{
	return  fe_lib_get_eth_cnt_common(FE_GET_ETH_RX_CNT);
}

ECNT_FEMGR_RET fe_lib_get_eth_tx_cnt()
{
	return fe_lib_get_eth_cnt_common(FE_GET_ETH_TX_CNT);
}

ECNT_FEMGR_RET fe_lib_get_eth_frame_cnt(ECNT_FEMGR_DIRECTION txrx_sel)
{
	int ret = -1;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = FE_GET_ETH_FRAME_CNT;
	para.txrx_sel = txrx_sel;
	ret = fe_lib_ioctl(FE_GET_ETH_FRAME_CNT,&para);
	if(ret == 0)
	{
		printf("\neth_frame_cnt = 0x%d\n", para.cnt);
		return ECNT_FEMGR_SUCCESS;
	}
	else
	{
		return ECNT_FEMGR_FAIL;
	}
}

ECNT_FEMGR_RET fe_lib_get_eth_err_cnt(uint err_type)
{
	int ret = -1;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = FE_GET_ETH_ERR_CNT;
	para.err_type = err_type;
	ret = fe_lib_ioctl(FE_GET_ETH_ERR_CNT,&para);
	if(ret == 0)
	{
		printf("\neth_err_cnt = %d\n", para.cnt);
		return ECNT_FEMGR_SUCCESS;
	}
	else
	{
		return ECNT_FEMGR_FAIL;
	}
}


ECNT_FEMGR_RET fe_api_set_lan_local_mac(unsigned char* mac)
{
	int ret;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	para.function_id = FE_SET_MAC_ADDR_7516;
	para.macSet_sel = FE_MAC_SET_LAN;
	para.api_data.mac_addr_7516.mac_h = ((mac[0]<<16) | (mac[1]<<8) | mac[2]);
	para.api_data.mac_addr_7516.mac_lmin = ((mac[3]<<16) | (mac[4]<<8) | mac[5]);
	para.api_data.mac_addr_7516.mac_lmax = para.api_data.mac_addr_7516.mac_lmin;

	ret = fe_lib_ioctl(FE_SET_MAC_ADDR_7516, &para);
	if (ret < 0)
	{
	    return ECNT_FEMGR_FAIL;
	}

	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_api_set_wan_local_mac(unsigned char* mac)
{
	int ret;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	para.function_id = FE_SET_MAC_ADDR_7516;
	para.macSet_sel = FE_MAC_SET_WAN;
	para.api_data.mac_addr_7516.mac_h = ((mac[0]<<16) | (mac[1]<<8) | mac[2]);
	para.api_data.mac_addr_7516.mac_lmin = ((mac[3]<<16) | (mac[4]<<8) | mac[5]);
	/* for compatibility, set the WAN MAC number to 16 */
	para.api_data.mac_addr_7516.mac_lmax = (para.api_data.mac_addr_7516.mac_lmin + 15);
	
	ret = fe_lib_ioctl(FE_SET_MAC_ADDR_7516, &para);
	if (ret < 0)
	{
	    return ECNT_FEMGR_FAIL;
	}
	
	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_lan_local_mac_range(unsigned char* mac_high, unsigned char* mac_low_min, unsigned char* mac_low_max)
{
	int ret;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	para.function_id = FE_SET_MAC_ADDR_7516;
	para.macSet_sel = FE_MAC_SET_LAN;
	para.api_data.mac_addr_7516.mac_h = ((mac_high[0]<<16) | (mac_high[1]<<8) | mac_high[2]);
	para.api_data.mac_addr_7516.mac_lmin = ((mac_low_min[0]<<16) | (mac_low_min[1]<<8) | mac_low_min[2]);
	para.api_data.mac_addr_7516.mac_lmax = ((mac_low_max[0]<<16) | (mac_low_max[1]<<8) | mac_low_max[2]);

	ret = fe_lib_ioctl(FE_SET_MAC_ADDR_7516, &para);
	if (ret < 0)
	{
	    return ECNT_FEMGR_FAIL;
	}

	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_wan_local_mac_range(unsigned char* mac_high, unsigned char* mac_low_min, unsigned char* mac_low_max)
{
	int ret;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	para.function_id = FE_SET_MAC_ADDR_7516;
	para.macSet_sel = FE_MAC_SET_WAN;
	para.api_data.mac_addr_7516.mac_h = ((mac_high[0]<<16) | (mac_high[1]<<8) | mac_high[2]);
	para.api_data.mac_addr_7516.mac_lmin = ((mac_low_min[0]<<16) | (mac_low_min[1]<<8) | mac_low_min[2]);
	para.api_data.mac_addr_7516.mac_lmax = ((mac_low_max[0]<<16) | (mac_low_max[1]<<8) | mac_low_max[2]);
	
	ret = fe_lib_ioctl(FE_SET_MAC_ADDR_7516, &para);
	if (ret < 0)
	{
	    return ECNT_FEMGR_FAIL;
	}
	
	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_api_get_gdma2_tx_byte(unsigned int* txByteCnt)
{
	int ret;
	
	ret = fe_lib_get_cnt(ECNT_FEMGR_GDM_SEL_GDMA2, ECNT_FEMGR_STATISTICS_TYPE_TX_BYTE, txByteCnt);

	return ret;
}

ECNT_FEMGR_RET fe_api_get_flow_byte_cnt(unsigned int direction, unsigned int flowid, unsigned long long* cnt)
{
	int ret;
	struct ecnt_fe_data para;
	unsigned long long tmp = 0;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	para.function_id = FE_GET_AC_GROUP_BYTE_CNT;
	if (ECNT_FEMGR_DIRECTON_TX == direction)
	{
		para.index = UP_QUEUE_CNT_START + flowid;
	}
	else
	{
		para.index = DOWN_QUEUE_CNT_START + flowid;
	}
	
	ret = fe_lib_ioctl(FE_GET_AC_GROUP_BYTE_CNT,&para);
	if (ret < 0)
	{
	    return ECNT_FEMGR_FAIL;
	}
	
	tmp = para.cnt_hi;
	*cnt = para.cnt|(tmp<<32);
	
	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_api_set_flow_ratelimit(unsigned int direction, unsigned int flowid, unsigned int rate_bps)
{
	int ret;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	para.function_id = FE_SET_METER_GROUP;
	if (ECNT_FEMGR_DIRECTON_TX == direction)
	{
		para.index = UP_QUEUE_CNT_START + flowid;
	}
	else
	{
		para.index = DOWN_QUEUE_CNT_START + flowid;
	}
	para.api_data.meter_rate = rate_bps;
	ret = fe_lib_ioctl(FE_SET_METER_GROUP,&para);
	if (ret < 0)
	{
	    return ECNT_FEMGR_FAIL;
	}
	
	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_api_get_flow_ratelimit(unsigned int direction, unsigned int flowid, unsigned int* rate_bps)
{
	int ret;
	struct ecnt_fe_data para;
	
	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	para.function_id = FE_GET_METER_GROUP;
	if (ECNT_FEMGR_DIRECTON_TX == direction)
	{
		para.index = UP_QUEUE_CNT_START + flowid;
	}
	else
	{
		para.index = DOWN_QUEUE_CNT_START + flowid;
	}
	
	ret = fe_lib_ioctl(FE_GET_METER_GROUP,&para);
	if (ret < 0)
	{
	    return ECNT_FEMGR_FAIL;
	}
	
	*rate_bps = para.api_data.meter_rate;
	
	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_api_set_upstream_ratelimit_by_itf(ECNT_FEMGR_RATELIMIT_IF_INFO *itf_info, unsigned char enable)
{
	unsigned char cmd_tmp[200] = {};
	
	if (enable)
	{
		system("echo 2 0 1 > /proc/tc3162/hwnat_traffic_ratelimit");
		sprintf(cmd_tmp, "echo 0 %d %d %d > /proc/tc3162/hwnat_traffic_ratelimit" ,
			itf_info->index, itf_info->enable, itf_info->rate);
		system(cmd_tmp);
	}
	else
	{
		system("echo 2 0 0 > /proc/tc3162/hwnat_traffic_ratelimit");
	}
	
	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_api_set_upstream_ratelimit_by_ip(ECNT_FEMGR_RATELIMIT_IP_INFO *ip_info, unsigned char enable)
{
	unsigned char cmd_tmp[200] = {};
	
	if (enable)
	{
		system("echo 2 1 1 > /proc/tc3162/hwnat_traffic_ratelimit");

		/*set IPv4 rule*/
		if (0 == ip_info->ip_version)
		{
			sprintf(cmd_tmp,"echo 1 %d %d %d %d %d.%d.%d.%d~%d.%d.%d.%d > /proc/tc3162/hwnat_traffic_ratelimit",
				ip_info->index , ip_info->enable , ip_info->rate , ip_info->ip_version ,
				ip_info->v4_pair_l[0],ip_info->v4_pair_l[1],ip_info->v4_pair_l[2],ip_info->v4_pair_l[3],
				ip_info->v4_pair_h[0],ip_info->v4_pair_h[1],ip_info->v4_pair_h[2],ip_info->v4_pair_h[3]);
		}
		else
		{
			sprintf(cmd_tmp, "echo 1 %d %d %d %d \
				%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X~%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X \
				> /proc/tc3162/hwnat_traffic_ratelimit",
				ip_info->index , ip_info->enable , ip_info->rate , ip_info->ip_version,
				ip_info->v6_pair_l[0],ip_info->v6_pair_l[1],ip_info->v6_pair_l[2],ip_info->v6_pair_l[3],
				ip_info->v6_pair_l[4],ip_info->v6_pair_l[5],ip_info->v6_pair_l[6],ip_info->v6_pair_l[7],
				ip_info->v6_pair_l[8],ip_info->v6_pair_l[9],ip_info->v6_pair_l[10],ip_info->v6_pair_l[11],
				ip_info->v6_pair_l[12],ip_info->v6_pair_l[13],ip_info->v6_pair_l[14],ip_info->v6_pair_l[15],
				ip_info->v6_pair_h[0],ip_info->v6_pair_h[1],ip_info->v6_pair_h[2],ip_info->v6_pair_h[3],
				ip_info->v6_pair_h[4],ip_info->v6_pair_h[5],ip_info->v6_pair_h[6],ip_info->v6_pair_h[7],
				ip_info->v6_pair_h[8],ip_info->v6_pair_h[9],ip_info->v6_pair_h[10],ip_info->v6_pair_h[11],
				ip_info->v6_pair_h[12],ip_info->v6_pair_h[13],ip_info->v6_pair_h[14],ip_info->v6_pair_h[15]);
		}
		
		system(cmd_tmp);
	}
	else
	{
		system("echo 2 1 0 > /proc/tc3162/hwnat_traffic_ratelimit");
	}
	
	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_api_set_wifi_ssid_ratelimit(unsigned char *itf_name,unsigned char *direction, unsigned int rate_kbps)
{
	unsigned char cmd_tmp[128] = {};

	sprintf(cmd_tmp, "echo %s %s %d > /proc/tc3162/wlan_ssid_ratelimit",itf_name,direction,rate_kbps);
	system(cmd_tmp);

	return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_car_mode_ratelimit(u32 rate, u8 enable, u8 idx)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    int ret;
	struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_METER_RATELIMIT;
    para.api_data.meter_cfg.rate = rate;    
    if(enable == 1)
        para.api_data.meter_cfg.enable = FE_ENABLE;
    else
        para.api_data.meter_cfg.enable = FE_DISABLE;
    para.api_data.meter_cfg.pkt_mode = METER_BYTE_MODE;
    para.api_data.meter_cfg.tick_sel = FAST_TICK;
    para.api_data.meter_cfg.idx = idx;
    para.api_data.meter_cfg.dir = UP_STREAM;
    para.api_data.meter_cfg.mode = CAR_QOS_MODE;
    para.api_data.meter_cfg.bucket_size = 0;

    ret = fe_lib_ioctl(FE_SET_METER_RATELIMIT, &para);
    if (ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    para.api_data.meter_cfg.dir = DOWN_STREAM;
    ret = fe_lib_ioctl(FE_SET_METER_RATELIMIT, &para);
    if (ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    return ECNT_FEMGR_SUCCESS;
#endif
}

ECNT_FEMGR_RET fe_lib_set_forcebw_wrr_mode_ratelimit(u32 rate, u8 enable, u8 idx)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    int ret;
	struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_METER_RATELIMIT;
    para.api_data.meter_cfg.rate = rate;    
    if(enable == 1)
        para.api_data.meter_cfg.enable = FE_ENABLE;
    else
        para.api_data.meter_cfg.enable = FE_DISABLE;
    para.api_data.meter_cfg.pkt_mode = METER_BYTE_MODE;
    para.api_data.meter_cfg.tick_sel = FAST_TICK;
    para.api_data.meter_cfg.idx = idx;
    para.api_data.meter_cfg.dir = UP_STREAM;
    para.api_data.meter_cfg.mode = FORCEBW_WRR_MODE;
    para.api_data.meter_cfg.bucket_size = 0;

    ret = fe_lib_ioctl(FE_SET_METER_RATELIMIT, &para);
    if (ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

    return ECNT_FEMGR_SUCCESS;
#endif
}

ECNT_FEMGR_RET fe_lib_set_acnt0_mode(uint dir, uint mode)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    int ret = 0; 
    int cmd = 0;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    cmd = FE_SET_ACNT0_MODE;
    para.function_id = FE_SET_ACNT0_MODE;

    if(dir == 0)
        para.api_data.meter_cfg.dir = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir = UP_STREAM;

    if(mode == 1)
        para.api_data.meter_cfg.acnt_mode = DEV_MODE;
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
    else if(mode == 0)
        para.api_data.meter_cfg.acnt_mode = TRAFFIC_FLOW_MODE;
    else if(mode == 2)
        para.api_data.meter_cfg.acnt_mode = WAN_ITF_MODE;
#endif
	else if(mode == 5)
		para.api_data.meter_cfg.acnt_mode = LLID_MODE;
    else
        para.api_data.meter_cfg.acnt_mode = DEFAULT_NULL_MODE;
    
    ret = fe_lib_ioctl(cmd,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_set_acnt1_mode(uint dir, uint mode)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    int ret = 0; 
    int cmd = 0;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    cmd = FE_SET_ACNT1_MODE;
    para.function_id = FE_SET_ACNT1_MODE;

    if(dir == 0)
        para.api_data.meter_cfg.dir = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir = UP_STREAM;

    if(mode == 1)
        para.api_data.meter_cfg.acnt_mode = TRAFFIC_FLOW_MODE;
    else if(mode == 2)
        para.api_data.meter_cfg.acnt_mode = WAN_ITF_MODE;
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
    else if(mode == 0)
        para.api_data.meter_cfg.acnt_mode = DEV_MODE;
#else
    else if(mode == 3)
        para.api_data.meter_cfg.acnt_mode = WAN_ITF_AND_TRAFFIC_FLOW_MODE;
#endif
    else
        para.api_data.meter_cfg.acnt_mode = DEFAULT_NULL_MODE;
    
    ret = fe_lib_ioctl(cmd,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_set_acnt2_mode(uint dir, uint mode)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    int ret = 0; 
    int cmd = 0;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    cmd = FE_SET_ACNT2_MODE;
    para.function_id = FE_SET_ACNT2_MODE;

    if(dir == 0)
        para.api_data.meter_cfg.dir = DOWN_STREAM;
    else
        para.api_data.meter_cfg.dir = UP_STREAM;

    if(mode == 1)
        para.api_data.meter_cfg.acnt_mode = TRAFFIC_FLOW_MODE;
    else if(mode == 2)
        para.api_data.meter_cfg.acnt_mode = WAN_ITF_MODE;
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
    else if(mode == 0)
        para.api_data.meter_cfg.acnt_mode = DEV_MODE;
#endif
    else
        para.api_data.meter_cfg.acnt_mode = DEFAULT_NULL_MODE;
    
    ret = fe_lib_ioctl(cmd,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_set_meter_ctl_by_olt(u8 enable)
{
#ifndef TCSUPPORT_CPU_EN7580
    return ECNT_FEMGR_NOT_SUPPORT;
#else
    int cmd = 0;
    int ret = 0;
    struct ecnt_fe_data para;
    memset(&para,0,sizeof(struct ecnt_fe_data));

    cmd = FE_SET_METER_CTL_BY_OLT;
    para.function_id = FE_SET_METER_CTL_BY_OLT;
    if(enable)
    {
        para.api_data.meter_cfg.idx = 1;
    }
    else
    {
        para.api_data.meter_cfg.idx = 0;
    }
    
    ret = fe_lib_ioctl(cmd,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#endif
}

ECNT_FEMGR_RET fe_lib_get_dev_mac_index(u8 *mac, u8 *mac_index)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    int cmd = 0;
    int ret = 0;
    struct ecnt_fe_data para;
    memset(&para,0,sizeof(struct ecnt_fe_data));

    cmd = FE_GET_DEV_MAC_INDEX;
    para.function_id = FE_GET_DEV_MAC_INDEX;
    memcpy(para.api_data.meter_cfg.mac, mac, 6);
    ret = fe_lib_ioctl(cmd,&para);
    *mac_index = para.api_data.meter_cfg.idx;
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}


ECNT_FEMGR_RET fe_lib_get_account_internal(int dir, u8 mac_index , u8 acnt_mode, u32 *cnt_l, u32 *cnt_h, u32 *pkt_cnt, u32 *pkt_cnt_h)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    int ret = 0;
    int cmd = 0;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    cmd = FE_GET_FLOW_CNT;
    para.function_id = FE_GET_FLOW_CNT ;

    para.api_data.meter_cfg.dir = dir ? UP_STREAM : DOWN_STREAM;

    para.api_data.meter_cfg.idx = mac_index;
    para.api_data.meter_cfg.acnt_mode = acnt_mode;
	*pkt_cnt_h = para.api_data.meter_cfg.pktCnt_H ;

    ret = fe_lib_ioctl(cmd,&para);
    *cnt_l = para.api_data.meter_cfg.byteCnt_L ;
    *cnt_h = para.api_data.meter_cfg.byteCnt_H ;
    *pkt_cnt = para.api_data.meter_cfg.pktCnt ;
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}


ECNT_FEMGR_RET fe_lib_set_clear_account(int dir, u8 index, u8 acnt_mode)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    int ret = 0;
    int cmd = 0;
    struct ecnt_fe_data para;

    memset(&para,0,sizeof(struct ecnt_fe_data));
    cmd = FE_CLEAR_FLOW_CNT;
    para.function_id = FE_CLEAR_FLOW_CNT ;

    para.api_data.meter_cfg.dir = dir ? UP_STREAM : DOWN_STREAM;

    para.api_data.meter_cfg.idx = index;
    para.api_data.meter_cfg.acnt_mode = acnt_mode;
    
    ret = fe_lib_ioctl(cmd,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}


ECNT_FEMGR_RET fe_lib_get_dev_mac_account(int dir, u8 *dev_mac , u32 *cnt_l, u32 *cnt_h, u32 *cnt_pkt, u32 *cnt_pkt_h)
{
    u8 mac_index = 0;
    
    fe_lib_get_dev_mac_index(dev_mac, &mac_index);
    
    if(mac_index == 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    fe_lib_get_account_internal(dir, mac_index - 1, DEV_MODE, cnt_l, cnt_h, cnt_pkt, cnt_pkt_h);
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_clear_dev_mac_account(int dir, u8 *dev_mac )
{
	u8 mac_index = 0;
	
	fe_lib_get_dev_mac_index(dev_mac, &mac_index);
	if(mac_index == 0)
    {
        return ECNT_FEMGR_FAIL;
	}
    
	fe_lib_set_clear_account(dir, mac_index - 1, DEV_MODE);
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_dev_mac_ratelimit(int dir, u8 *dev_mac , u32 ratelimit, u8 enable)
{
	u8 mac_index = 0;
    ECNT_FEMGR_METER meter = {0};

	fe_lib_get_dev_mac_index(dev_mac, &mac_index);
	if(mac_index == 0)
    {
        return ECNT_FEMGR_FAIL;
	}
 
#if !defined(TCSUPPORT_ACCOUNT_METER_V2)    
	fe_lib_set_meter_ratelimit_mode(dir, 0, DEV_MAC_MODE);
#endif
    meter.rate = ratelimit;
    if(enable == 1)
        meter.enable = FE_ENABLE;
    else
        meter.enable = FE_DISABLE;
    meter.index = mac_index - 1;
    meter.rate_mode = METER_BYTE_MODE;
    if(dir == 1)
        meter.dir = UP_STREAM;
    else
        meter.dir = DOWN_STREAM;
    meter.mode = DEV_MAC_MODE;
    meter.tick_mode = FAST_TICK;
    meter.bucket_size = 0;

    fe_lib_set_meter_ratelimit(&meter);
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_wan_ratelimit(int dir, u8 *wan_name , u32 ratelimit, u8 enable)
{
    ECNT_FEMGR_METER meter = {0};
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
    unchar itf_index = 0;
    
    fe_lib_get_wan_itf_index(wan_name, &itf_index);
#else
    fe_lib_set_meter_ratelimit_mode(dir, 0, WAN_MODE);
#endif

    meter.rate = ratelimit;
    if(enable == 1)
        meter.enable = FE_ENABLE;
    else
        meter.enable = FE_DISABLE;
    meter.rate_mode = METER_BYTE_MODE;
    if(dir == 1)
        meter.dir = UP_STREAM;
    else
        meter.dir = DOWN_STREAM;
    meter.mode = WAN_MODE;
    meter.tick_mode = FAST_TICK;
    meter.bucket_size = 0;
    memcpy(meter.itf_name, wan_name, 16);

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
    meter.index = itf_index * 2;   //unicast
    fe_lib_set_meter_ratelimit(&meter);
    meter.index = itf_index * 2 + 1; //multicast
#endif
    fe_lib_set_meter_ratelimit(&meter);
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_olt_ctl_ratelimit(u16 dir,  u16 index, u32 ratelimit)
{
    ECNT_FEMGR_METER meter = {0};
 
#if !defined(TCSUPPORT_ACCOUNT_METER_V2)   
    fe_lib_set_meter_ratelimit_mode(dir, 0, OLT_CTL_MODE);
#endif

    meter.rate = ratelimit;
    if(ratelimit > 0)
        meter.enable = FE_ENABLE;
    else
        meter.enable = FE_DISABLE;

    meter.rate_mode = METER_BYTE_MODE;
    meter.dir = dir;
    meter.index = index;
    meter.mode = OLT_CTL_MODE;
    meter.tick_mode = FAST_TICK;
    meter.bucket_size = 0;

    fe_lib_set_meter_ratelimit(&meter);
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_get_multicast_pkt_account(int dir, u32 *cnt_l, u32 *cnt_h, u32 *cnt_pkt, u32 *pkt_cnt_h)
{
    fe_lib_get_account_internal(dir, 0,MULTICAST_MODE, cnt_l, cnt_h, cnt_pkt, pkt_cnt_h);
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_clear_multicast_pkt_account(int dir)
{
	fe_lib_set_clear_account(dir, 0, MULTICAST_MODE);
    return ECNT_FEMGR_SUCCESS;
}

int fe_lib_get_wan_itf_index(char *itf_name, unchar *itf_index){
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
	int cmd = 0;
	int ret = 0;
	struct ecnt_fe_data para;
	memset(&para,0,sizeof(struct ecnt_fe_data));

	cmd = FE_GET_WAN_ITF_INDEX;
	para.function_id = FE_GET_WAN_ITF_INDEX;
	memcpy(para.api_data.meter_cfg.itf_name, itf_name, 16);
	ret = fe_lib_ioctl(cmd,&para);
	*itf_index = para.api_data.meter_cfg.idx;
	return ret;
#else
    return 0;
#endif
}

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
ECNT_FEMGR_RET fe_lib_set_traffic_flow_ratelimit(int dir, u8 index, u32 ratelimit, u8 enable)
{
    ECNT_FEMGR_METER meter = {0};

    meter.rate = ratelimit;
    if(enable == 1)
        meter.enable = FE_ENABLE;
    else
        meter.enable = FE_DISABLE;
    meter.rate_mode = METER_BYTE_MODE;
    if(dir == 1)
        meter.dir = UP_STREAM;
    else
        meter.dir = DOWN_STREAM;
    meter.mode = TRAFFIC_QOS_MODE;
    meter.tick_mode = FAST_TICK;
    meter.bucket_size = 0;
    meter.index = index;
	
    fe_lib_set_meter_ratelimit(&meter);

    return ECNT_FEMGR_SUCCESS;
}

int fe_lib_get_wan_itf_pkt_account(int dir, char *itf_name, unsigned long long *byte_cnt, unsigned long long *pkt_cnt){

	unchar itf_index = 0;
	unsigned int rxbytes_l = 0, rxbytes_h = 0, txbytes_l = 0, txbytes_h = 0, rx_pkt_cnt = 0, tx_pkt_cnt = 0, rx_pkt_cnt_h = 0, tx_pkt_cnt_h = 0;
	unsigned long long tmp;
	
	fe_lib_get_wan_itf_index(itf_name, &itf_index);
	if(itf_index >= ACNT_GRP_WAN_NUM)
		printf("ERROR, Not support wan interface %s=====>\n", itf_name);
	
	fe_lib_get_account_internal(dir, 2*itf_index, WAN_ITF_MODE, &rxbytes_l, &rxbytes_h, &rx_pkt_cnt, &rx_pkt_cnt_h);//unicast
	*pkt_cnt = ((unsigned long long)rx_pkt_cnt_h << 32) + rx_pkt_cnt;
	*byte_cnt = ((unsigned long long)rxbytes_h << 32) + rxbytes_l;

	fe_lib_get_account_internal(dir, 2*itf_index+1, WAN_ITF_MODE, &rxbytes_l, &rxbytes_h, &rx_pkt_cnt, &rx_pkt_cnt_h);//multicast
	*pkt_cnt += ((unsigned long long)rx_pkt_cnt_h << 32) + rx_pkt_cnt;
	*byte_cnt += ((unsigned long long)rxbytes_h << 32) + rxbytes_l;
    
	return 0;
}

int fe_lib_get_llid_pkt_account(int dir, int llid, unsigned long long *byte_cnt, unsigned long long *pkt_cnt){

	unsigned int rxbytes_l = 0, rxbytes_h = 0, txbytes_l = 0, txbytes_h = 0, rx_pkt_cnt = 0, tx_pkt_cnt = 0, rx_pkt_cnt_h = 0, tx_pkt_cnt_h = 0;
	unsigned long long tmp;
	
	fe_lib_get_account_internal(dir, 1+llid, LLID_MODE, &rxbytes_l, &rxbytes_h, &rx_pkt_cnt, &rx_pkt_cnt_h);
	*pkt_cnt = ((unsigned long long)rx_pkt_cnt_h << 32) + rx_pkt_cnt;
	*byte_cnt = ((unsigned long long)rxbytes_h << 32) + rxbytes_l;

    
	return 0;
}

#else
int fe_lib_get_wan_itf_pkt_account(int dir, char *itf_name, uint *cnt_l, uint *cnt_h, uint *pkt_cnt, uint *pkt_cnt_h){

    unchar itf_index = 0;
    uint acnt1_mode = 0;
    
    fe_lib_get_acnt1_mode(dir, &acnt1_mode);
    fe_lib_get_wan_itf_index(itf_name, &itf_index);

    if(itf_index >= 8){
        printf("itf name is not a valid wan interface.\n");
    }else{
        if(acnt1_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE)
            fe_lib_get_account_internal(dir, itf_index, acnt1_mode, cnt_l, cnt_h, pkt_cnt, pkt_cnt_h);
        else
            fe_lib_get_account_internal(dir, itf_index, WAN_ITF_MODE, cnt_l, cnt_h, pkt_cnt, pkt_cnt_h);
    }
    
    return 0;
}
#endif

int fe_lib_clear_wan_itf_pkt_account(int dir, char *itf_name){
    
    unchar itf_index = 0xFF;
    uint acnt1_mode = 0;
    
    fe_lib_get_acnt1_mode(dir, &acnt1_mode);
    fe_lib_get_wan_itf_index(itf_name, &itf_index);
    
    if(itf_index >= 8){
        printf("itf name is not a valid wan interface.\n");
    }else{
        if(acnt1_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE)
            fe_lib_set_clear_account(dir, itf_index, acnt1_mode);
        else
            fe_lib_set_clear_account(dir, itf_index, WAN_ITF_MODE);
    }
    
    return 0;
}

int fe_lib_clear_llid_pkt_account(int dir, int llid){
   	
    if(llid >= 8){
        printf("llid is not a valid wan interface.\n");
    }else{
        fe_lib_set_clear_account(dir, llid, LLID_MODE);
    }
    
    return 0;
}

ECNT_FEMGR_RET fe_lib_add_traffic_class(ECNT_FEMGR_TRFC_INFO trfc)
{
    int ret, i;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = FE_ADD_TRAFFIC_CLASS;
    memcpy(&para.api_data.fe_trfc_cfg, &trfc, sizeof(para.api_data.fe_trfc_cfg));

	ret = fe_lib_ioctl(FE_ADD_TRAFFIC_CLASS,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_del_traffic_class(ECNT_FEMGR_TRFC_INFO trfc)
{
    int ret, i;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = FE_DEL_TRAFFIC_CLASS;
    memcpy(&para.api_data.fe_trfc_cfg, &trfc, sizeof(para.api_data.fe_trfc_cfg));

    ret = fe_lib_ioctl(FE_DEL_TRAFFIC_CLASS,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_get_pppoe_info(ECNT_PPPOE_INFO_DATA *pppoe_info)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_PPPOE_INFO;
    
    ret = fe_lib_ioctl(FE_GET_PPPOE_INFO, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

	pppoe_info->valid = para.api_data.pppoe_info.valid;
	pppoe_info->sessionID = para.api_data.pppoe_info.sessionID;
	pppoe_info->src_ip = para.api_data.pppoe_info.src_ip;
	pppoe_info->dst_ip = para.api_data.pppoe_info.dst_ip;

	memcpy(pppoe_info->smac_add, para.api_data.pppoe_info.smac_add, 6);
	memcpy(pppoe_info->dmac_add, para.api_data.pppoe_info.dmac_add, 6);
	memcpy(pppoe_info->ifname, para.api_data.pppoe_info.ifname, 16);

	pppoe_info->dns_valid = para.api_data.pppoe_info.dns_valid;
	pppoe_info->pri_dns = para.api_data.pppoe_info.pri_dns;
	pppoe_info->snd_dns = para.api_data.pppoe_info.snd_dns;
	
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_pppoe_info_clean(void)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_PPPOE_INFO_CLEAN;
    
    ret = fe_lib_ioctl(FE_SET_PPPOE_INFO_CLEAN, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
	
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_get_tx_rx_common(int cmd, char gdm_sel, int lan_port, uint *outPtr_Val)
{
	int ret;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = cmd;
	para.gdm_sel = gdm_sel ;
	para.api_data.fe_cnt_str.lan_port = lan_port ;
	ret = fe_lib_ioctl(cmd, &para);
    *outPtr_Val = para.api_data.fe_cnt_str.rate;

    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_get_tx_traffic(char gdm_sel, int lan_port, uint *rate)
{
	return fe_lib_get_tx_rx_common(FE_GET_TX_TRAFFIC, gdm_sel, lan_port, rate);
}

ECNT_FEMGR_RET fe_lib_get_rx_traffic(char gdm_sel, int lan_port, uint *rate)
{
	return fe_lib_get_tx_rx_common(FE_GET_RX_TRAFFIC, gdm_sel, lan_port, rate);
}

ECNT_FEMGR_RET fe_lib_get_tx_rate(char gdm_sel, int lan_port, uint *rate)
{
	return fe_lib_get_tx_rx_common(FE_GET_TX_RATE, gdm_sel, lan_port, rate);
}

ECNT_FEMGR_RET fe_lib_get_rx_rate(char gdm_sel, int lan_port, uint *rate)
{
	return fe_lib_get_tx_rx_common(FE_GET_RX_RATE, gdm_sel, lan_port, rate);
}

ECNT_FEMGR_RET fe_lib_get_tx_octets(char gdm_sel, int lan_port, uint *frameByteCnt)
{
	return fe_lib_get_tx_rx_common(FE_GET_TX_OCTETS, gdm_sel, lan_port, frameByteCnt);
}

ECNT_FEMGR_RET fe_lib_get_rx_octets(char gdm_sel, int lan_port, uint *frameByteCnt)
{
	return fe_lib_get_tx_rx_common(FE_GET_RX_OCTETS, gdm_sel, lan_port, frameByteCnt);
}

ECNT_FEMGR_RET fe_lib_get_rx_discard_counter(char gdm_sel, int lan_port, uint *discardCnt)
{
	return fe_lib_get_tx_rx_common(FE_GET_RX_DISCARD_COUNTER, gdm_sel, lan_port, discardCnt);
}

ECNT_FEMGR_RET fe_lib_get_tx_discard_counter(char gdm_sel, int lan_port, uint *discardCnt)
{
	return fe_lib_get_tx_rx_common(FE_GET_TX_DISCARD_COUNTER, gdm_sel, lan_port, discardCnt);
}

ECNT_FEMGR_RET fe_lib_get_rx_error_counter(char gdm_sel, int lan_port, uint *discardCnt)
{
	return fe_lib_get_tx_rx_common(FE_GET_RX_ERROR_COUNTER, gdm_sel, lan_port, discardCnt);
}

ECNT_FEMGR_RET fe_lib_get_tx_error_counter(char gdm_sel, int lan_port, uint *discardCnt)
{
	return fe_lib_get_tx_rx_common(FE_GET_TX_ERROR_COUNTER, gdm_sel, lan_port, discardCnt);
}

ECNT_FEMGR_RET fe_lib_get_account(unsigned char mode, int dir, void *in_data, unsigned long long *bytecnt, unsigned long long *pktcnt)
{
	int cmd = 0;
	int ret = 0;
	unchar idx = 0;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	
	#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7528)
	cmd = FE_GET_FLOW_CNT;
	para.function_id = FE_GET_FLOW_CNT;
	switch(mode)
	{
		case TRAFFIC_FLOW_MODE:
			idx = (*(unsigned char *)in_data);
			break;
		case DEV_MODE:
			fe_lib_get_dev_mac_index((unchar *)in_data, &idx);
			if(idx == 0)
			{
				printf("mac address is not valid\n");
				ret = -1;
			}
			else
			{
				idx--;
			}
			break;
		case WAN_ITF_MODE:
			fe_lib_get_wan_itf_index((char *)in_data, &idx);
			if(idx >= 8)
			{
				printf("itf name is not valid\n");
				ret = -1;
			}
			break;
		case MULTICAST_MODE:
			idx = 0;
			break;
		default:
			ret = -1;
			break;
	}

	/* check paras */
	if(ret < 0)
	{
		return ECNT_FEMGR_FAIL;
	}

	/* set ioctl paras */
	para.api_data.meter_cfg.idx = idx;
	para.api_data.meter_cfg.acnt_mode = mode;
	para.api_data.meter_cfg.dir = (dir ? UP_STREAM : DOWN_STREAM);
	ret = fe_lib_ioctl(cmd,&para);
	if(ret < 0)
	{
		return ECNT_FEMGR_FAIL;
	}

	/* get counter */
	*pktcnt = (para.api_data.meter_cfg.pktCnt + (((unsigned long long)para.api_data.meter_cfg.pktCnt_H) << 32));
	*bytecnt = (para.api_data.meter_cfg.byteCnt_L + (((unsigned long long)para.api_data.meter_cfg.byteCnt_H) << 32));

	return ECNT_FEMGR_SUCCESS;
	#else
	return ECNT_FEMGR_NOT_SUPPORT;
	#endif
}

ECNT_FEMGR_RET fe_lib_clean_account(unsigned char mode, int dir, void *in_data)
{
	int ret = 0;
	int cmd = 0;
	unchar idx = 0;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));

	#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7528)
	cmd = FE_CLEAR_FLOW_CNT;
	para.function_id = FE_CLEAR_FLOW_CNT ;
	switch(mode)
	{
	    case TRAFFIC_FLOW_MODE:
	        idx = (*(unsigned char *)in_data);
	        break;
		case LLID_MODE:
	        idx = (*(unsigned char *)in_data);
	        break;
	    case DEV_MODE:
	        fe_lib_get_dev_mac_index((unchar *)in_data, &idx);
		if(idx == 0)
		{
			printf("mac address is not valid\n");
			ret = -1;
		}
		else
		{
			idx--;
		}
	        break;
	    case WAN_ITF_MODE:
	        fe_lib_get_wan_itf_index((char *)in_data, &idx);
		if(idx >= 8)
		{
			printf("itf name is not valid\n");
			ret = -1;
		}
	        break;
	    case MULTICAST_MODE:
		idx = 0;
	        break;
	    default:
	        ret = -1;
		break;
	}

	/* check paras */
	if(ret < 0)
	{
		return ECNT_FEMGR_FAIL;
	}

	/* set ioctl paras */
	para.api_data.meter_cfg.idx = idx;
	para.api_data.meter_cfg.acnt_mode = mode;
	para.api_data.meter_cfg.dir = (dir ? UP_STREAM : DOWN_STREAM);
	ret = fe_lib_ioctl(cmd,&para);
	if(ret < 0)
	{
		return ECNT_FEMGR_FAIL;
	}

	return ECNT_FEMGR_SUCCESS;
	#else
	return ECNT_FEMGR_NOT_SUPPORT;
	#endif
}


int
inet_pton4(const char *src,char *dst)
{
	int saw_digit, octets, ch;
	unsigned char tmp[4], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {

		if (ch >= '0' && ch <= '9') {
			int new = *tp * 10 + (ch - '0');

			if (new>255)
				return (0);
			*tp = new;
			if (! saw_digit) {
				if (++octets>4)
					return (0);
				saw_digit = 1;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		} else
			return (0);
	}
	if (octets < 4)
		return (0);
	memcpy(dst, tmp, 4);
	return (1);
}

int inet_pton6(src, dst)
	const char *src;
	unsigned char *dst;
{
	static const char xdigits_l[] = "0123456789abcdef",
			  xdigits_u[] = "0123456789ABCDEF";
	unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	const char *xdigits, *curtok;
	int ch, saw_xdigit;
	unsigned int val;

	memset((tp = tmp), '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = *src++) != '\0') 
	{
		const char *pch;

		if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
			pch = strchr((xdigits = xdigits_u), ch);
		if (pch != NULL) 
		{
			val <<= 4;
			val |= (pch - xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') 
		{
			curtok = src;
			if (!saw_xdigit) 
			{
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			}
			if (tp + NS_INT16SZ > endp)
				return (0);
			*tp++ = (unsigned char) (val >> 8) & 0xff;
			*tp++ = (unsigned char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
		    inet_pton4(curtok, tp) > 0) 
		{
			tp += NS_INADDRSZ;
			saw_xdigit = 0;
			break;	/* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit) 
	{
		if (tp + NS_INT16SZ > endp)
			return (0);
		*tp++ = (unsigned char) (val >> 8) & 0xff;
		*tp++ = (unsigned char) val & 0xff;
	}
	if (colonp != NULL) 
	{
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = tp - colonp;
		int i;

		for (i = 1; i <= n; i++) 
		{
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	memcpy(dst, tmp, NS_IN6ADDRSZ);
	return (1);
}

ECNT_FEMGR_RET fe_lib_stb_src_ip_common(int cmd, uint ip_type, uint src_ip4, struct in6_addr *src_ip6)
{
	int ret;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = cmd;
	para.api_data.stb_ip_cfg.ip_type = ip_type ;
	if(ip_type == 0)
		para.api_data.stb_ip_cfg.ip4 = src_ip4;
	else
		memcpy(para.api_data.stb_ip_cfg.ip6.s6_addr, src_ip6->s6_addr, 16);
	ret = fe_lib_ioctl(cmd,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_add_stb_src_ipv4(uint ip4)
{        
	return fe_lib_stb_src_ip_common(FE_ADD_STB_SRC_IP, 0, ip4, NULL);
}

ECNT_FEMGR_RET fe_lib_del_stb_src_ipv4(uint ip4)
{

	return fe_lib_stb_src_ip_common(FE_DEL_STB_SRC_IP, 0, ip4, NULL);
}

ECNT_FEMGR_RET fe_lib_add_stb_src_ipv6(char *ip6)
{
	struct in6_addr src_ip6;
	if(ip6 == NULL)
		return ECNT_FEMGR_FAIL;

	if(inet_pton6(ip6, src_ip6.s6_addr) != 1)
		return ECNT_FEMGR_FAIL;

	return fe_lib_stb_src_ip_common(FE_ADD_STB_SRC_IP, 1, 0, &src_ip6);
}

ECNT_FEMGR_RET fe_lib_del_stb_src_ipv6(char *ip6)
{
	struct in6_addr src_ip6;
	if(ip6 == NULL)
		return ECNT_FEMGR_FAIL;

	if(inet_pton6(ip6, src_ip6.s6_addr) != 1)
		return ECNT_FEMGR_FAIL;

	return fe_lib_stb_src_ip_common(FE_DEL_STB_SRC_IP, 1, 0, &src_ip6);
}
static ECNT_FEMGR_RET fe_lib_set_ratelimit_for_pkt_type(ECNT_FEMGR_RATELIMIT_PKT_TYPE pkt_type, unsigned int mode)
{
	struct ecnt_fe_data para;
	int ret = 0;
    memset(&para,0,sizeof(struct ecnt_fe_data));

	para.api_data.pkt_ratelimit.pkt_formate = pkt_type;
	para.api_data.pkt_ratelimit.mode = mode;
	para.function_id = FE_SET_RATELIMIT_FOR_PKT_FORMATE;
	
	ret = fe_lib_ioctl(FE_SET_RATELIMIT_FOR_PKT_FORMATE, &para);
	if(!ret)
		return ECNT_FEMGR_SUCCESS;
	else
		return ECNT_FEMGR_FAIL;
}
ECNT_FEMGR_RET fe_lib_set_pkt_ratelimit_clear(ECNT_FEMGR_RATELIMIT_PKT_TYPE pkt_type)
{
	return fe_lib_set_ratelimit_for_pkt_type(pkt_type, 0);
}
ECNT_FEMGR_RET fe_lib_set_pkt_ratelimit_total(ECNT_FEMGR_RATELIMIT_PKT_TYPE pkt_type)
{
	return fe_lib_set_ratelimit_for_pkt_type(pkt_type, 1);
}
ECNT_FEMGR_RET fe_lib_set_pkt_ratelimit_singal(ECNT_FEMGR_RATELIMIT_PKT_TYPE pkt_type)
{
	return fe_lib_set_ratelimit_for_pkt_type(pkt_type, 2);
}

ECNT_FEMGR_RET fe_lib_set_mc_vlan_global(ECNT_FEMGR_ENABLE_MODE enable)
{
    int ret;
    struct ecnt_fe_data para;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_MC_VLAN_GLOBAL;
    para.api_data.enable = enable;

    ret = fe_lib_ioctl(FE_SET_MC_VLAN_GLOBAL, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_get_mc_vlan_global(ECNT_FEMGR_ENABLE_MODE *enable)
{
    int ret;
    struct ecnt_fe_data para;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_MC_VLAN_GLOBAL;

    ret = fe_lib_ioctl(FE_GET_MC_VLAN_GLOBAL, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }

	*enable = para.api_data.enable;
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_set_mc_vlan_table_cfg(ECNT_FEMGR_MC_VLAN_TABLE_t *mcVlanTableCfg)
{
    int ret;
    struct ecnt_fe_data para;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_MC_VLAN_TABLE_CFG;
    para.api_data.fe_mcvlan_table_cfg.mc_table_id = mcVlanTableCfg->mc_table_id;
    para.api_data.fe_mcvlan_table_cfg.enable = mcVlanTableCfg->enable;
    para.api_data.fe_mcvlan_table_cfg.ppe_entry_id = mcVlanTableCfg->ppe_entry_id;

    ret = fe_lib_ioctl(FE_SET_MC_VLAN_TABLE_CFG, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_get_mc_vlan_table_cfg(ECNT_FEMGR_MC_VLAN_TABLE_t *mcVlanTableCfg)
{
    int ret;
    struct ecnt_fe_data para;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_MC_VLAN_TABLE_CFG;
    para.api_data.fe_mcvlan_table_cfg.mc_table_id = mcVlanTableCfg->mc_table_id;

    ret = fe_lib_ioctl(FE_GET_MC_VLAN_TABLE_CFG, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
	mcVlanTableCfg->enable = para.api_data.fe_mcvlan_table_cfg.enable;
    mcVlanTableCfg->ppe_entry_id = para.api_data.fe_mcvlan_table_cfg.ppe_entry_id;
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_set_mc_vlan_action_cfg(ECNT_FEMGR_MC_VLAN_ACTION_t *mcVlanActionCfg)
{
    int ret;
    struct ecnt_fe_data para;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_MC_VLAN_ACTION_CFG;
    para.api_data.fe_mcvlan_action_cfg.mc_table_id = mcVlanActionCfg->mc_table_id;
	para.api_data.fe_mcvlan_action_cfg.mc_chnl_id = mcVlanActionCfg->mc_chnl_id;
	para.api_data.fe_mcvlan_action_cfg.mc_vlan_op = mcVlanActionCfg->mc_vlan_op;
	para.api_data.fe_mcvlan_action_cfg.mc_vlan_vpm = mcVlanActionCfg->mc_vlan_vpm;
	para.api_data.fe_mcvlan_action_cfg.mc_vlan = mcVlanActionCfg->mc_vlan;

    ret = fe_lib_ioctl(FE_SET_MC_VLAN_ACTION_CFG, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_get_mc_vlan_action_cfg(ECNT_FEMGR_MC_VLAN_ACTION_t *mcVlanActionCfg)
{
    int ret;
    struct ecnt_fe_data para;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_GET_MC_VLAN_ACTION_CFG;
    para.api_data.fe_mcvlan_action_cfg.mc_table_id = mcVlanActionCfg->mc_table_id;
	para.api_data.fe_mcvlan_action_cfg.mc_chnl_id = mcVlanActionCfg->mc_chnl_id;

    ret = fe_lib_ioctl(FE_GET_MC_VLAN_ACTION_CFG, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
	mcVlanActionCfg->mc_vlan_op = para.api_data.fe_mcvlan_action_cfg.mc_vlan_op;
	mcVlanActionCfg->mc_vlan_vpm = para.api_data.fe_mcvlan_action_cfg.mc_vlan_vpm;
	mcVlanActionCfg->mc_vlan = para.api_data.fe_mcvlan_action_cfg.mc_vlan;
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_set_mc_vlan_clear_all(unsigned char mc_table_id)
{
    int ret;
    struct ecnt_fe_data para;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_MC_VLAN_CLEAR_ALL;
	para.api_data.fe_mcvlan_table_cfg.mc_table_id = mc_table_id;
    ret = fe_lib_ioctl(FE_SET_MC_VLAN_CLEAR_ALL, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}

ECNT_FEMGR_RET fe_lib_set_rx_mac_filter(ECNT_FEMGR_MAC_FILTER_INFO macfilter_cfg)
{
    int ret, i;
	struct ecnt_fe_data para;

	memset(&para,0,sizeof(struct ecnt_fe_data));
	para.function_id = FE_SET_RX_MAC_FILTER;
    memcpy(&(para.api_data.macfilter_cfg), &macfilter_cfg, sizeof(para.api_data.macfilter_cfg));

    ret = fe_lib_ioctl(FE_SET_RX_MAC_FILTER,&para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_hsgmii_rx_port_ratelimit(uint hsgmii_index, uint rate, uint mode)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_HSGMII_RX_PORT_RATE;
    para.index = hsgmii_index;
	para.api_data.rate_cfg.rate = rate;
	para.api_data.rate_cfg.mode = mode;
    ret = fe_lib_ioctl(FE_SET_HSGMII_RX_PORT_RATE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_force_slow_enable(u8 enable)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id        = FE_SET_FORCE_SLOW_ENABLE;
    para.api_data.enable    = enable;
    
    ret = fe_lib_ioctl(FE_SET_FORCE_SLOW_ENABLE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_force_slow_duty(u8 fix_duty, u32 fast_duty, u32 slow_duty)
{
    int ret = -1;
    struct ecnt_fe_data para;
    
    memset(&para,0,sizeof(struct ecnt_fe_data));
    para.function_id                = FE_SET_FORCE_SLOW_DUTY;
    para.api_data.force_slow_cfg.fix_duty_enable    = fix_duty;
	para.api_data.force_slow_cfg.fast_duty    		= fast_duty;
	para.api_data.force_slow_cfg.slow_duty    		= slow_duty;
    
    ret = fe_lib_ioctl(FE_SET_FORCE_SLOW_DUTY, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
}

ECNT_FEMGR_RET fe_lib_set_dev_stat_ratelimit_mode(unsigned int wan_conn_type, char *wan_itf_name)
{
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
    int ret = 0; 
    struct ecnt_fe_data para;
    /* wan_conn_type : 1-wifi 2.4G, 2-wifi 5G, 3-eth lan */
    if(wan_conn_type < 1 || wan_conn_type > 3)
    	return ECNT_FEMGR_INVALID_PARAM;

    memset(&para, 0, sizeof(struct ecnt_fe_data));
    para.function_id = FE_SET_DEV_STAT_RATELIMIT_MODE;
	para.api_data.dev_statistics_cfg.wan_conn_type = wan_conn_type;
	memcpy(para.api_data.dev_statistics_cfg.wan_itf_name, wan_itf_name, 16);

    ret = fe_lib_ioctl(FE_SET_DEV_STAT_RATELIMIT_MODE, &para);
    if(ret < 0)
    {
        return ECNT_FEMGR_FAIL;
    }
    
    return ECNT_FEMGR_SUCCESS;
#else
    return ECNT_FEMGR_NOT_SUPPORT;
#endif
}



