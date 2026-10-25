/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
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

/**
* @file  blapi_traffic_bsp.c  
* @brief This file gives examples on using apis from blapi_traffic.h.
* @author Lei.Zhang
* @date     2019-12-3
* @version  A001 
* @copyright EcoNet Inc                                                              
*/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "traffic_dslimit.h"
#include "blapi_extra.h"
#include "blapi_traffic_bsp.h"
#if defined(TCSUPPORT_IFC_EN)
#include "libapi_lib_ifc.h"
#endif

#include "libapi_lib_qdmamgr.h"

#include "libapi_lib_utility.h"
#include "libapi_lib_switchmgr.h"
#if defined(TCSUPPORT_MULTI_USER_ITF) 
#include <lan_port/lan_port_info.h>
#endif
#include "traffic_process_global_def.h"
#include <linux/qos_type.h>
#include <ecnt_lib_dbg.h>

#include "libapi_lib_fe.h"
#include "libapi_lib_ppe.h"
#include <modules/eth_global_def.h>

#if defined(TCSUPPORT_SISM)
#include "lib_pt.h"
#endif /*defined(TCSUPPORT_SISM)*/

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
/*
*If you want to print the debug infomation, you need to create the following files:
*echo [num1] [num2] > /tmp/blapi_dbg_level_enable
*[num1] is debug level. (8=ECNT_DBG_DEBUG, 4=ECNT_DBG_NOTICE, 2=ECNT_DBG_WARN, 1=ECNT_DBG_ERROR)
*[num2] is enable mask. (8=ECNT_BLAPI_XDSL_DBUG_ENABLE, 4=ECNT_BLAPI_TRAFFIC_DBUG_ENABLE, 2=ECNT_BLAPI_SYSTEM_DBUG_ENABLE, 1=ECNT_BLAPI_PERFORM_DBUG_ENABLE)
*For example : echo 8 4 > /tmp/blapi_dbg_level_enable ,means open the debug info in blapi traffic and the debug level is 8. 
*If you want to open the debug info both in blapi traffic and xdsl ,you need set the corresponding bit to  "1", so [num2] is 12 ,binary is 1100.
*/
#define LIB_BLAPI_ERROR(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_ERROR, ECNT_BLAPI_TRAFFIC_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_WARN(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_WARN, ECNT_BLAPI_TRAFFIC_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_NOTICE(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_NOTICE, ECNT_BLAPI_TRAFFIC_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_DEBUG(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_DEBUG, ECNT_BLAPI_TRAFFIC_DBUG_ENABLE, fmt,##__VA_ARGS__)

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int blapi_traffic_get_lan_tx_traffic(int lan_port,unsigned int* rate)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_traffic(0,lan_port,rate))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_rx_traffic(int lan_port,unsigned int* rate)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_traffic(0,lan_port,rate))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_tx_rate(int lan_port,unsigned int* rate)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_rate(0,lan_port,rate))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_rx_rate(int lan_port,unsigned int* rate)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_rate(0,lan_port,rate))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_tx_octets(int lan_port, unsigned int *frameByteCnt)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_octets(0,lan_port,frameByteCnt))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_rx_octets(int lan_port, unsigned int *frameByteCnt)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_octets(0,lan_port,frameByteCnt))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_rx_discard_counter(int lan_port, unsigned int *discardCnt)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_discard_counter(0,lan_port,discardCnt))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_tx_discard_counter(int lan_port, unsigned int *discardCnt)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_discard_counter(0,lan_port,discardCnt))
		return 0;
	else
		return -1;
}


int blapi_traffic_get_lan_rx_error_counter(int lan_port, uint *errorCnt)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_error_counter(0,lan_port,errorCnt))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_lan_tx_error_counter(int lan_port, uint *errorCnt)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_error_counter(0,lan_port,errorCnt))
		return 0;
	else
		return -1;
}

int blapi_traffic_get_wan_tx_traffic(unsigned int* rate)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_TX_TRAFFIC);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*rate);
		
	if (!SRC_blapi(blapi_h))
	{
		*rate = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;	
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_traffic(1,0,rate))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_rx_traffic(unsigned int* rate)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_RX_TRAFFIC);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*rate);
	if (!SRC_blapi(blapi_h))
	{
		*rate = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_traffic(1,0,rate))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_tx_rate(unsigned int* rate)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_TX_RATE);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*rate);

	if (!SRC_blapi(blapi_h))
	{
		*rate = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_rate(1,0,rate))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_rx_rate(unsigned int* rate)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_RX_RATE);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*rate);
	if (!SRC_blapi(blapi_h))
	{
		*rate = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_rate(1,0,rate))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_tx_octets(unsigned int *frameByteCnt)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_TX_OCTETS);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*frameByteCnt);

	if (!SRC_blapi(blapi_h))
	{
		*frameByteCnt = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_octets(1,0,frameByteCnt))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_rx_octets(unsigned int *frameByteCnt)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_RX_OCTETS);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*frameByteCnt);

	if (!SRC_blapi(blapi_h))
	{
		*frameByteCnt = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_octets(1,0,frameByteCnt))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_rx_discard_counter(unsigned int *discardCnt)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_RX_DISCARD_COUNTER);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*discardCnt);

	if (!SRC_blapi(blapi_h))
	{
		*discardCnt = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_discard_counter(1,0,discardCnt))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_tx_discard_counter(unsigned int *discardCnt)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_TX_DISCARD_COUNTER);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*discardCnt);

	if (!SRC_blapi(blapi_h))
	{
		*discardCnt = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else

	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_discard_counter(1,0,discardCnt))
		return 0;
	else
		return -1;
#endif
}


int blapi_traffic_get_wan_rx_error_counter(uint *errorCnt)
{
#if defined(TCSUPPORT_SISM_HOST)
		int ret = -1;
		blapi_hdr_t *blapi_h;
		blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
		memset(blapi_h, 0 , sizeof(blapi_hdr_t));
		blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_RX_ERROR_COUNTER);
		blapi_h->argc = htonl(1);
		blapi_h->argv1.i = htonl(*errorCnt);

		if (!SRC_blapi(blapi_h))
		{
			*errorCnt = ntohl(blapi_h->argv1.iu);
			ret = blapi_h->ret;
		}

		free(blapi_h);
		return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_rx_error_counter(1,0,errorCnt))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_get_wan_tx_error_counter(uint *errorCnt)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_TX_ERROR_COUNTER);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*errorCnt);
	if (!SRC_blapi(blapi_h))
	{
		*errorCnt = ntohl(blapi_h->argv1.iu);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_tx_error_counter(1,0,errorCnt))
		return 0;
	else
		return -1;
#endif
}

int blapi_traffic_check_dsl_enable()
{	
	int fd=0, ret=0;
	char path_wan_type[32] = "/proc/tc3162/wan_type";
	char buf[16] = {0};
	int result = 0;
	fd = open(path_wan_type,O_RDONLY);	
	if(fd == -1)
	{	
		printf("Cannot open this file /proc/tc3162/wan_type\n");
		return 0;
	}
	ret = read(fd, buf, sizeof(buf) -1);
	if(ret <= 0)
	{
		printf("This file is empty /proc/tc3162/wan_type\n");
		close(fd);
		return 0;
	}
	close(fd);
	result = atoi(buf);
	return result;
}


int blapi_traffic_get_wan_rx_statistics(ECNT_TRAFFIC_WAN_RX_STATISTICS* p_stat)
{
#if defined(TCSUPPORT_SISM_HOST)
	if(1 == blapi_traffic_check_dsl_enable())
	{
		int ret = -1;
		uint64_t temp_value;
		blapi_hdr_t *blapi_h;
		blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
		memset(blapi_h, 0 , sizeof(blapi_hdr_t));
		blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_RX_STATISTICS);
		blapi_h->argc = htonl(1);

		if (!SRC_blapi(blapi_h))
		{
			memcpy(p_stat, &blapi_h->argv1, sizeof(ECNT_TRAFFIC_WAN_RX_STATISTICS));
			ntohll(p_stat->oversize,&temp_value);
			p_stat->oversize = temp_value;
			ntohll(p_stat->undersize,&temp_value);
			p_stat->undersize = temp_value;
			ntohll(p_stat->frame_cnt,&temp_value);
			p_stat->frame_cnt = temp_value;
			ntohll(p_stat->frame_len,&temp_value);
			p_stat->frame_len = temp_value;
			ntohll(p_stat->drop_cnt,&temp_value);
			p_stat->drop_cnt = temp_value;
			ntohll(p_stat->broadcast,&temp_value);
			p_stat->broadcast = temp_value;
			ntohll(p_stat->multicast,&temp_value);
			p_stat->multicast = temp_value;
			ntohll(p_stat->crc,&temp_value); 
			p_stat->crc = temp_value;
			ntohll(p_stat->fragment,&temp_value);
			p_stat->fragment = 	temp_value;
			ntohll(p_stat->jabber,&temp_value);
			p_stat->jabber = temp_value;
			ntohll(p_stat->less_64,&temp_value);
			p_stat->less_64 = temp_value;
			ntohll(p_stat->more_1518,&temp_value);
			p_stat->more_1518 = temp_value;
			ntohll(p_stat->eq_64,&temp_value);
			p_stat->eq_64 = temp_value;
			ntohll(p_stat->from_65_to_127,&temp_value);
			p_stat->from_65_to_127 = temp_value;
			ntohll(p_stat->from_128_to_255,&temp_value);
			p_stat->from_128_to_255 = temp_value;
			ntohll(p_stat->from_256_to_511,&temp_value);
			p_stat->from_256_to_511 = temp_value;
			ntohll(p_stat->from_512_to_1023,&temp_value);
			p_stat->from_512_to_1023 = temp_value;
			ntohll(p_stat->from_1024_to_1518,&temp_value);
			p_stat->from_1024_to_1518 = temp_value;
			ret = blapi_h->ret;
		}

		free(blapi_h);
		return ret; 
	}
	else
#endif
	{
		ECNT_FEMGR_GDMA2_RX_STATISTICS stat;

		memset(&stat,0,sizeof(ECNT_FEMGR_GDMA2_RX_STATISTICS));
		if(ECNT_FEMGR_SUCCESS == fe_lib_get_gdma2_rx_statistics(&stat))
		{
			p_stat->oversize = stat.oversize;
			p_stat->undersize = stat.undersize; 
			p_stat->frame_cnt = stat.frame_cnt; 
			p_stat->frame_len = stat.frame_len;      
			p_stat->drop_cnt = stat.drop_cnt; 
			p_stat->broadcast = stat.broadcast;      
			p_stat->multicast = stat.multicast;      
			p_stat->crc = stat.crc;             
			p_stat->fragment = stat.fragment;        
			p_stat->jabber = stat.jabber;         
			p_stat->less_64 = stat.less_64;           
			p_stat->more_1518 = stat.more_1518;       
			p_stat->eq_64 = stat.eq_64;      
			p_stat->from_65_to_127 = stat.from_65_to_127;    
			p_stat->from_128_to_255 = stat.from_128_to_255;    
			p_stat->from_256_to_511 = stat.from_256_to_511; 
			p_stat->from_512_to_1023 = stat.from_512_to_1023; 
			p_stat->from_1024_to_1518 = stat.from_1024_to_1518;
			
			return 0;
		}
		else
			return -1;
	}
}

int blapi_traffic_get_wan_tx_statistics(ECNT_TRAFFIC_WAN_TX_STATISTICS* p_stat)
{
#if defined(TCSUPPORT_SISM_HOST)
	if(1 == blapi_traffic_check_dsl_enable())
	{
		int ret = -1;
		uint64_t temp_value;
		blapi_hdr_t *blapi_h;
		blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
		memset(blapi_h, 0 , sizeof(blapi_hdr_t));
		blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_TX_STATISTICS);
		blapi_h->argc = htonl(1);

		if (!SRC_blapi(blapi_h))
		{
			memcpy(p_stat, &blapi_h->argv1, sizeof(ECNT_TRAFFIC_WAN_TX_STATISTICS));
			ntohll(p_stat->frame_cnt,&temp_value);
			p_stat->frame_cnt = temp_value;
			ntohll(p_stat->frame_len,&temp_value);
			p_stat->frame_len = temp_value;
			ntohll(p_stat->drop_cnt,&temp_value);
			p_stat->drop_cnt = temp_value;
			ntohll(p_stat->broadcast,&temp_value);
			p_stat->broadcast = temp_value;
			ntohll(p_stat->multicast,&temp_value);
			p_stat->multicast = temp_value;
			ntohll(p_stat->less_64,&temp_value);
			p_stat->less_64 = temp_value;
			ntohll(p_stat->more_1518,&temp_value);
			p_stat->more_1518 = temp_value;
			ntohll(p_stat->eq_64,&temp_value);
			p_stat->eq_64 = temp_value;
			ntohll(p_stat->from_65_to_127,&temp_value);
			p_stat->from_65_to_127 = temp_value;
			ntohll(p_stat->from_128_to_255,&temp_value);
			p_stat->from_128_to_255 = temp_value;
			ntohll(p_stat->from_256_to_511,&temp_value);
			p_stat->from_256_to_511 = temp_value;
			ntohll(p_stat->from_512_to_1023,&temp_value);
			p_stat->from_512_to_1023 = temp_value;
			ntohll(p_stat->from_1024_to_1518,&temp_value);
			p_stat->from_1024_to_1518 = temp_value;
			ret = blapi_h->ret;
		}

		free(blapi_h);
		return ret; 
	}
	else
#endif
	{
		ECNT_FEMGR_GDMA2_TX_STATISTICS stat;

		memset(&stat,0,sizeof(ECNT_FEMGR_GDMA2_TX_STATISTICS));
		if(ECNT_FEMGR_SUCCESS == fe_lib_get_gdma2_tx_statistics(&stat))
		{
			p_stat->frame_cnt = stat.frame_cnt; 
			p_stat->frame_len = stat.frame_len;      
			p_stat->drop_cnt = stat.drop_cnt; 
			p_stat->broadcast = stat.broadcast;      
			p_stat->multicast = stat.multicast;      
			p_stat->less_64 = stat.less_64;           
			p_stat->more_1518 = stat.more_1518;       
			p_stat->eq_64 = stat.eq_64;      
			p_stat->from_65_to_127 = stat.from_65_to_127;    
			p_stat->from_128_to_255 = stat.from_128_to_255;    
			p_stat->from_256_to_511 = stat.from_256_to_511; 
			p_stat->from_512_to_1023 = stat.from_512_to_1023; 
			p_stat->from_1024_to_1518 = stat.from_1024_to_1518;
			
			return 0;
		}
		else
			return -1;
	}
}

int blapi_traffic_set_pon_ratelimit(ECNT_TRAFFICE_PON_RATELIMIT_MODE mode,unsigned int index,unsigned int rate)
{
#if defined(TCSUPPORT_CPU_EN7580)
	unsigned char direction = 0;

	if(GEMPORT_UP == mode)
	{
		direction = UP_DIR; //upstream
	}
	else if(GEMPORT_DOWN == mode)
	{
		direction = DOWN_DIR;//downstream
	}
	else
		return -1;

	return fe_lib_set_olt_ctl_ratelimit(direction,index,rate);
#else
	unsigned int meter_start = 0;

	if(EPON_SLA == mode)
		meter_start = 10;
	else if(GEMPORT_UP == mode)
	{
		#if defined(TCSUPPORT_ALPHION_PON)
		meter_start = 0;
		#else
		meter_start = 10;
		#endif
	}
	else if(GEMPORT_DOWN == mode)
	{
		#if defined(TCSUPPORT_ALPHION_PON)
		meter_start = 32;
		#else
		meter_start = 10+32;
		#endif
	}
	
	if(ECNT_PPEMGR_SUCCESS == ppemgr_lib_set_meter_ratelimit(meter_start+index, rate))
		return 0;
	else
		return -1;
	#endif
}

int blapi_traffic_set_ratelimit_mode(ECNT_TRAFFIC_RATELIMIT_MODE mode)
{
	LIB_BLAPI_DEBUG("enter,  meter_mode=%d=====>\n", mode);

	switch(mode)
	{
		case RATELIMIT_MODE_DEV_MAC:	
		#if defined(TCSUPPORT_CPU_EN7580)
			doValPut("/proc/tc3162/hwnat_bandwidth", "1 1");
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, 0, 5);	
			fe_lib_set_meter_ratelimit_mode(UP_DIR, 0, 5);
		#elif defined(TCSUPPORT_CPU_EN7528)	
			doValPut("/proc/tc3162/hwnat_meter_mode", "1");
		#endif
			break;
		case RATELIMIT_MODE_WAN:
		#if defined(TCSUPPORT_CPU_EN7580)
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, 0, 7);	
			fe_lib_set_meter_ratelimit_mode(UP_DIR, 0, 7);
		#elif defined(TCSUPPORT_CPU_EN7528)	
			doValPut("/proc/tc3162/hwnat_meter_mode", "2");
		#endif
			break;
		case RATELIMIT_MODE_TRAFFIC_QOS:
		#if defined(TCSUPPORT_CPU_EN7580)
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, 0, 4);	
			fe_lib_set_meter_ratelimit_mode(UP_DIR, 0, 4);
		#endif
			break;
		case RATELIMIT_MODE_OLT_CTL:
#if defined(TCSUPPORT_CPU_EN7580)
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, 0, 3);
			fe_lib_set_meter_ratelimit_mode(UP_DIR, 0, 3);
#endif
			break;
		default:
			LIB_BLAPI_DEBUG("not support this mode=====>\n");
			return -1;
	}
	
	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
int blapi_traffic_set_meter_ratelimit_mode(ECNT_TRAFFIC_RATELIMIT_MODE mode, unsigned int idx)
{
	LIB_BLAPI_DEBUG("enter,  meter_mode=%d=====>\n", mode);

	switch(mode)
	{
		case RATELIMIT_MODE_DEV_MAC:	
			doValPut("/proc/tc3162/hwnat_bandwidth", "1 1");
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, idx, 5);	
			fe_lib_set_meter_ratelimit_mode(UP_DIR, idx, 5);
			break;
		case RATELIMIT_MODE_WAN:
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, idx, 7);	
			fe_lib_set_meter_ratelimit_mode(UP_DIR, idx, 7);
			break;
		case RATELIMIT_MODE_TRAFFIC_QOS:
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, idx, 4);	
			fe_lib_set_meter_ratelimit_mode(UP_DIR, idx, 4);
			break;
		case RATELIMIT_MODE_OLT_CTL:
			fe_lib_set_meter_ratelimit_mode(DOWN_DIR, idx, 3);
			fe_lib_set_meter_ratelimit_mode(UP_DIR, idx, 3);
			break;
		default:
			LIB_BLAPI_DEBUG("not support this mode=====>\n");
			return -1;
	}
	
	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}
#endif

int blapi_traffic_set_flowid_acnt_enable(int enable)
{
	LIB_BLAPI_DEBUG("enter=====>\n");
	
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	if(enable)
	{
		fe_lib_set_acnt0_mode(DOWN_DIR, 0);
		fe_lib_set_acnt0_mode(UP_DIR, 0);
	}
	else
	{
		fe_lib_set_acnt0_mode(DOWN_DIR, 6);
		fe_lib_set_acnt0_mode(UP_DIR, 6);
	}
#else
#if defined(TCSUPPORT_CPU_EN7580)
	if(enable)
	{
		fe_lib_set_acnt1_mode(DOWN_DIR, 1);
		fe_lib_set_acnt1_mode(UP_DIR, 1);
	}
	else
	{
		fe_lib_set_acnt1_mode(DOWN_DIR, 6);
		fe_lib_set_acnt1_mode(UP_DIR, 6);
	}
#else
	return -1;
#endif
#endif				
	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

int blapi_traffic_set_llid_acnt_enable(int enable)
{
	LIB_BLAPI_DEBUG("enter=====>\n");
	
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	if(enable)
	{
		fe_lib_set_acnt0_mode(DOWN_DIR, 5);
		fe_lib_set_acnt0_mode(UP_DIR, 5);
	}
	else
	{
		fe_lib_set_acnt0_mode(DOWN_DIR, 6);
		fe_lib_set_acnt0_mode(UP_DIR, 6);
	}
#endif				
	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

int blapi_traffic_set_flowid_ratelimit(unsigned char direction, unsigned char flowid, unsigned int ratelimit)
{
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	return fe_lib_set_traffic_flow_ratelimit(direction, flowid, ratelimit, ratelimit?1:0);
#else
	return ppemgr_lib_set_traffic_qos_flow_ratelimit(flowid,direction,ratelimit);
#endif
}

int blapi_traffic_set_dev_ratelimit_by_mac(dev_bandwidth_tmp_t *p_dev_mac_acl, int bandwidth_cnt)
{
	int i = 0, j = 0;
	char cmd[128] = {0};
	ECNT_SWITCHMGR_DEV_SPEED devMacAcl;
	unsigned char fe_mac[6] = {0};
	unsigned char conn_type = 0;
#if defined(TCSUPPORT_CT_JOYME4)
	int meter_mode = -1, ret = 0;
#endif

	LIB_BLAPI_DEBUG("enter=====>\n");
	
	for (i = 0; i < bandwidth_cnt; i++)
	{
		for(j = 0; j < 6; j++)
		{
			fe_mac[j] = (unsigned char)p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[j];
		}

#if defined(TCSUPPORT_CT_JOYME4)
	/* 7528 & 7580 need check mether first when clear rules. */
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
		if ( 0 == p_dev_mac_acl->trtcm_acl_mac_table[i].ds_queue_speed
			&& 0 == p_dev_mac_acl->trtcm_acl_mac_table[i].us_queue_speed )
		{
			ret = blapi_traffic_get_ratelimit_mode(DOWN_DIR, &meter_mode);
			if ( RATELIMIT_MODE_DEV_MAC != meter_mode ) /* no need to clear rules if NOT DEV MAC mode. */
				continue;
		}
#endif
#endif

#if defined(TCSUPPORT_CPU_EN7580)/*7580 7523*/
		fe_lib_set_dev_mac_ratelimit(0, fe_mac , 0, 0);
		fe_lib_set_dev_mac_ratelimit(1, fe_mac , 0, 0);

		if (p_dev_mac_acl->trtcm_acl_mac_table[i].ds_queue_speed > 0)
		{
			fe_lib_set_dev_mac_ratelimit(0, fe_mac ,
				p_dev_mac_acl->trtcm_acl_mac_table[i].ds_queue_speed, 1);
		}
		if (p_dev_mac_acl->trtcm_acl_mac_table[i].us_queue_speed > 0)
		{
			fe_lib_set_dev_mac_ratelimit(1, fe_mac ,
				p_dev_mac_acl->trtcm_acl_mac_table[i].us_queue_speed, 1);
		}
#elif defined(TCSUPPORT_CPU_EN7528)/*7528*/
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"3 %02x:%02x:%02x:%02x:%02x:%02x,%u,%u",fe_mac[0],fe_mac[1],fe_mac[2],fe_mac[3],fe_mac[4],fe_mac[5],\
		p_dev_mac_acl->trtcm_acl_mac_table[i].us_queue_speed,p_dev_mac_acl->trtcm_acl_mac_table[i].ds_queue_speed);
	doValPut("/proc/tc3162/hwnat_bandwidth", cmd);	
	memset(cmd, 0, sizeof(cmd));	
	strncpy(cmd, "/userfs/bin/hw_nat -!\n", sizeof(cmd) - 1);	
	system(cmd);
#else/*7527 7526*/
		conn_type = p_dev_mac_acl->trtcm_acl_mac_table[i].conn_type;
		if ((0 == conn_type)||(3 == conn_type))
		{
			/* ethernet */
			devMacAcl.trtcm_acl_mac_table[i].us_queue_speed = getACLRate(p_dev_mac_acl->trtcm_acl_mac_table[i].us_queue_speed);
			devMacAcl.trtcm_acl_mac_table[i].ds_queue_speed = getACLRate(p_dev_mac_acl->trtcm_acl_mac_table[i].ds_queue_speed);

			devMacAcl.trtcm_acl_mac_table[i].dev_mac[0] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[0];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[1] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[1];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[2] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[2];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[3] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[3];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[4] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[4];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[5] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[5];
		}
		if ((1 == conn_type)||(3 == conn_type)) 
		{
			/* wireless */			
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "%s,%d,%d\n", fe_mac, p_dev_mac_acl->trtcm_acl_mac_table[i].us_queue_speed,p_dev_mac_acl->trtcm_acl_mac_table[i].ds_queue_speed);
			LIB_BLAPI_DEBUG("cmd=%s=====>\n", cmd);
			doValPut("/proc/tc3162/wifi_dev_maxbandwidth", cmd);
		}
#endif
	}	

#if !defined(TCSUPPORT_CPU_EN7580)&&!defined(TCSUPPORT_CPU_EN7528)
	/* enable bandwidth */
	if (bandwidth_cnt >= 0) 
	{
		devMacAcl.enable = 1;
	}
	switchmgr_lib_set_mac_match_acl(&devMacAcl);
	
	if (bandwidth_cnt >= 0) 
	{
		doValPut("/proc/tc3162/wifi_dev_maxbandwidth", "1");
	}
#endif
		
	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}

int blapi_traffic_set_wan_acnt_enable(int enable)
{
	LIB_BLAPI_DEBUG("enter=====>\n");
	
#if defined(TCSUPPORT_CPU_EN7580) ||defined(TCSUPPORT_CPU_EN7527)
	if(enable)
	{
		fe_lib_set_acnt1_mode(DOWN_DIR, 2);
		fe_lib_set_acnt1_mode(UP_DIR, 2);
	}
	else
	{
		fe_lib_set_acnt1_mode(DOWN_DIR, 6);
		fe_lib_set_acnt1_mode(UP_DIR, 6);
	}
#else
	return -1;
#endif
				
	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

int blapi_traffic_set_wan_acnt_clear(unsigned int wanid)
{  
	int ret = -1;
	LIB_BLAPI_DEBUG("enter, wanid=%d=====>\n", wanid);

	ret = ppemgr_lib_set_wan_acnt_clear(wanid);/* 0~31 */
	
	return ret;
}


int blapi_traffic_get_wan_acnt(int i, wan_acnt_t *cnt)
{  
	LIB_BLAPI_DEBUG("enter, i=%d=====>\n", i);

	ECNT_PPEMGR_WAN_ACCOUNT wan_acnt;
	memset(&wan_acnt, 0, sizeof(ECNT_PPEMGR_WAN_ACCOUNT));
	ppemgr_lib_get_wan_acnt(i, &wan_acnt);/* 0~31 */
	cnt->mulRxBytes = wan_acnt.mulRxBytes;
	cnt->uniRxBytes = wan_acnt.uniRxBytes;
	cnt->mulTxBytes = wan_acnt.mulTxBytes;
	cnt->uniTxBytes = wan_acnt.uniTxBytes;
	cnt->uniRxCnt = wan_acnt.uniRxCnt;
	cnt->mulRxCnt = wan_acnt.mulRxCnt;
	cnt->uniTxCnt = wan_acnt.uniTxCnt;
	cnt->mulTxCnt = wan_acnt.mulTxCnt;
	cnt->txRate = wan_acnt.txRate;
	cnt->rxRate = wan_acnt.rxRate;
	cnt->jiffies = wan_acnt.jiffies;	

	LIB_BLAPI_DEBUG("leave, cnt->mulRxBytes=%llu, cnt->uniRxBytes=%llu=====>\n",cnt->mulRxBytes, cnt->uniRxBytes);
	return 0;
}

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
int blapi_traffic_get_wan_acnt_by_name(char *itf_name, wan_acnt_t *cnt)
{
	unchar itf_index = 99;

	LIB_BLAPI_DEBUG("enter, wan name=%s=====>\n", itf_name);
	
	ECNT_PPEMGR_WAN_ACCOUNT wan_acnt;
	memset(&wan_acnt, 0, sizeof(ECNT_PPEMGR_WAN_ACCOUNT));
	fe_lib_get_wan_itf_index(itf_name, &itf_index);
	LIB_BLAPI_DEBUG("wan itf_index=%d=====>\n", itf_index);
	ppemgr_lib_get_wan_acnt(itf_index, &wan_acnt);/* 0~7 */
	
	cnt->mulRxBytes = wan_acnt.mulRxBytes;
	cnt->uniRxBytes = wan_acnt.uniRxBytes;
	cnt->mulTxBytes = wan_acnt.mulTxBytes;
	cnt->uniTxBytes = wan_acnt.uniTxBytes;
	cnt->uniRxCnt = wan_acnt.uniRxCnt;
	cnt->mulRxCnt = wan_acnt.mulRxCnt;
	cnt->uniTxCnt = wan_acnt.uniTxCnt;
	cnt->mulTxCnt = wan_acnt.mulTxCnt;
	cnt->txRate = wan_acnt.txRate;
	cnt->rxRate = wan_acnt.rxRate;
	cnt->jiffies = wan_acnt.jiffies;	

	LIB_BLAPI_DEBUG("leave, cnt->mulRxBytes=%llu, cnt->uniRxBytes=%llu=====>\n",cnt->mulRxBytes, cnt->uniRxBytes);
	return 0;
}
#endif

int blapi_traffic_set_vip_by_port(unsigned short sport, unsigned short dport,ECNT_TRAFFIC_VIP_PROTOCOL vip_p)
{
	ECNT_FEMGR_VIP_ENTRY entry;
	memset(&entry,0,sizeof(ECNT_FEMGR_VIP_ENTRY));
	
	#if defined(TCSUPPORT_CPU_EN7580)&&!defined(TCSUPPORT_CPU_EN7523)
	/*set ifc*/
	#if defined(TCSUPPORT_IFC_EN)
	if(0 == ifc_lib_set_vip_by_port(sport,dport,1,0))
		return 0;
	#endif
	
	return -1;
	#else
	/*set vip*/
	if(sport)
	{
		entry.u.tcp.src_port_en = 1;
		entry.u.tcp.src_port = sport;
	}

	if(dport)
	{
		entry.u.tcp.dst_port_en = 1;
		entry.u.tcp.dst_port = dport;
	}

	switch(vip_p)
	{
		case VIP_P_TCP:
			entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS;
			if(ECNT_FEMGR_SUCCESS == fe_lib_add_vip(&entry))
				return 0;
			break;
		case VIP_P_UDP:
			entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS;
			if(ECNT_FEMGR_SUCCESS == fe_lib_add_vip(&entry))
				return 0;
			break;
		case VIP_P_ALL:
			entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS;
			if(ECNT_FEMGR_SUCCESS == fe_lib_add_vip(&entry))
			{
				entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS;
				if(ECNT_FEMGR_SUCCESS == fe_lib_add_vip(&entry))
					return 0;
				else
				{
					entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS;
					fe_lib_del_vip(&entry);
				}
			}
			break;
		default:
			ecnt_blapi_printf("vip_p  error,should be 0/1/2\n");
			return -1;
	}

	#if defined(TCSUPPORT_IFC_EN)
	/*if set vip fail,set ifc*/
	if(0 == ifc_lib_set_vip_by_port(sport,dport,1,0))
		return 0;
	#endif

	return -1; 
	
	#endif
}

int blapi_traffic_del_vip_by_port(unsigned short sport, unsigned short dport,ECNT_TRAFFIC_VIP_PROTOCOL vip_p)
{
	ECNT_FEMGR_VIP_ENTRY entry;
	int ret=-1,ret1 = -1;
	
	memset(&entry,0,sizeof(ECNT_FEMGR_VIP_ENTRY));
	
	#if defined(TCSUPPORT_CPU_EN7580)&&!defined(TCSUPPORT_CPU_EN7523)
	/*del ifc*/
	#if defined(TCSUPPORT_IFC_EN)
	if(0 == ifc_lib_del_vip_by_port(sport,dport,0))
		return 0;
	#endif
	
	return -1;
	#else
	/*del vip*/
	if(sport)
	{
		entry.u.tcp.src_port_en = 1;
		entry.u.tcp.src_port = sport;
	}

	if(dport)
	{
		entry.u.tcp.dst_port_en = 1;
		entry.u.tcp.dst_port = dport;
	}

	switch(vip_p)
	{
		case VIP_P_TCP:
			entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS;
			if(ECNT_FEMGR_SUCCESS == fe_lib_del_vip(&entry))
				return 0;
			break;
		case VIP_P_UDP:
			entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS;
			if(ECNT_FEMGR_SUCCESS == fe_lib_del_vip(&entry))
				return 0;
			break;
		case VIP_P_ALL:
			entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS;
			ret = fe_lib_del_vip(&entry);
			entry.type = ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS;
			ret1 = fe_lib_del_vip(&entry);

			if((ECNT_FEMGR_SUCCESS==ret)&&(ECNT_FEMGR_SUCCESS==ret1))
				return 0;
			break;
		default:
			ecnt_blapi_printf("vip_p error,should be 0/1/2\n");
			return -1;
	}

	#if defined(TCSUPPORT_IFC_EN)
	/*if del vip fail,del ifc*/
	if(0 == ifc_lib_del_vip_by_port(sport,dport,0))
		return 0;
	#endif

	return -1;
	
	#endif
}

int blapi_traffic_get_wan_port_statistics(ECNT_TRAFFIC_GMAC_ETH_STA * static_buf, int port_id) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	uint64_t temp_value;

	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	if(!blapi_h){
		LIB_BLAPI_DEBUG("blapi_traffic_get_wan_port_statistics malloc blapi_h failed\n");
		return -1;
	}
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_PORT_STATISTICS);
	blapi_h->argc = htonl(2);
	blapi_h->argv2.i = htonl(port_id);

	if (!SRC_blapi(blapi_h))
	{
		memcpy(static_buf, &blapi_h->argv1, sizeof(ECNT_GMAC_ETH_STA));
		ntohll(static_buf->inBytes,&temp_value);
		static_buf->inBytes = temp_value;
		ntohll(static_buf->inPkts,&temp_value);
		static_buf->inPkts = temp_value;
		ntohll(static_buf->inUnicast,&temp_value);
		static_buf->inUnicast = temp_value;
		ntohll(static_buf->inMulticast,&temp_value);
		static_buf->inMulticast = temp_value;
		ntohll(static_buf->inBroadcast,&temp_value);
		static_buf->inBroadcast = temp_value;
		ntohll(static_buf->inError,&temp_value);
		static_buf->inError = temp_value;
		ntohll(static_buf->inDiscard,&temp_value);
		static_buf->inDiscard = temp_value;
		ntohll(static_buf->outBytes,&temp_value);
		static_buf->outBytes = temp_value;
		ntohll(static_buf->outPkts,&temp_value);
		static_buf->outPkts = temp_value;
		ntohll(static_buf->outUnicast,&temp_value);
		static_buf->outUnicast = temp_value;
		ntohll(static_buf->outMulticast,&temp_value);
		static_buf->outMulticast = 	temp_value;
		ntohll(static_buf->outBroadcast,&temp_value);
		static_buf->outBroadcast =  temp_value;
		ntohll(static_buf->outError,&temp_value);
		static_buf->outError = temp_value;
		ntohll(static_buf->outDiscard,&temp_value);
		static_buf->outDiscard = temp_value;
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	return -1;
#endif
}

int blapi_traffic_get_wan_port_link_state(ECNT_TRAFFIC_GMAC_ETH_LINKST * state_buf, int port_id) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	if(!blapi_h){
		LIB_BLAPI_DEBUG("blapi_traffic_get_wan_port_link_state malloc blapi_h failed\n");
		return -1;
	}
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_WAN_PORT_LINK_STATISTICS);
	blapi_h->argc = htonl(2);
	blapi_h->argv2.i = htonl(port_id);
	
	if (!SRC_blapi(blapi_h))
	{
		LIB_BLAPI_DEBUG("blapi_traffic_get_wan_port_link_state SRC_blapi\n");
		memcpy(state_buf, &blapi_h->argv1, sizeof(ECNT_GMAC_ETH_LINKST));	
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	return -1;
#endif
}

int blapi_traffic_set_wan_port_link_mode(ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE mode, int port_id) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	if(!blapi_h){
		LIB_BLAPI_DEBUG("blapi_traffic_set_wan_port_link_mode malloc blapi_h failed\n");
		return -1;
	}
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_SET_WAN_PORT_LINK_MODE);
	blapi_h->argc = htonl(3);
	blapi_h->argv2.i = htonl(port_id);
	blapi_h->argv3.i = htonl(mode);
	
	if (!SRC_blapi(blapi_h))
	{
		LIB_BLAPI_DEBUG("blapi_traffic_set_wan_port_link_mode SRC_blapi\n");
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	return -1;
#endif
}
