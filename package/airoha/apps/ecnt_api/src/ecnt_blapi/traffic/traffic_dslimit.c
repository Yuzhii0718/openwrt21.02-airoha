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
* @file  blapi_traffic.c  
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
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "traffic_dslimit.h"
#include <ecnt_lib_dbg.h>

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

#if defined(TCSUPPORT_CT_DS_LIMIT)
int dslimit_set_mode_and_queue_speed(int up_dw,int mode, int g_queue_speed[])
{
	int fp = -1;
	int ret = -1;
	int dsmode = mode; 
	/* 0: disabled; 1: base interface; 2: base vlan id; 3: base ip range; */
	
	LIB_BLAPI_DEBUG("enter,mode=%d,up_dw=%d=====>\n", mode, up_dw);
	
	fp = open("/dev/"QOSDSLIMIT_DEV_NAME, O_RDONLY);
	if ( fp < 0)
	{
		LIB_BLAPI_ERROR("leave, open [%s] error, return -1=====>\n", QOSDSLIMIT_DEV_NAME);
		return -1;
	}

	/* up stream mode. */
	if ( DSLIMIT_UP == up_dw )
	{
		ret = ioctl(fp, QOSDSLIMIT_IOC_MODECFG_UP, &dsmode);
		if (ret < 0)
		{
			LIB_BLAPI_DEBUG("ioctl up mode error ,ret = %d=====>\n", ret);
		}
#if defined(TCSUPPORT_WAN_ETHER)
		if(dsmode)
		{
			ret = ioctl(fp, QOSDSLIMIT_IOC_QUEUECFG_UP, g_queue_speed);
			if ( ret < 0 )
			{
				LIB_BLAPI_DEBUG("ioctl up queue error ,ret = %d=====>\n", ret);
			}		
		}
#endif
	}
	else if ( DSLIMIT_DW == up_dw )
	{
		ret = ioctl(fp, QOSDSLIMIT_IOC_MODECFG_DW, &dsmode);
		if (ret < 0)
		{
			LIB_BLAPI_DEBUG("ioctl down mode error ,ret = %d=====>\n", ret);
		}

		if(dsmode)
		{
			ret = ioctl(fp, QOSDSLIMIT_IOC_QUEUECFG_DW, g_queue_speed);
			if ( ret < 0 )
			{
				LIB_BLAPI_DEBUG("ioctl down queue error ,ret = %d=====>\n", ret);
			}
		}
	}
	close(fp);

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}

/*
 get queue index via data speed
 return queue index when data speed match
 create new queue when data speed unmatch
*/
int dslimit_get_queue_index_and_speed(int dataspeed, int g_queue_speed[])
{
	int idx = 0;
	int actspeed = dataspeed * DS_QUEUE_PER_SPEED;

	LIB_BLAPI_DEBUG("enter, dataspeed=%d=====>\n",dataspeed);
	
	if ( dataspeed <=0 )
	{
		LIB_BLAPI_ERROR("leave, dataspeed<=0 ,return -1 =====>\n");
		return -1; /* no limit */
	}

	for ( idx = 0; idx < DSLIMIT_MAX_QUEUE_NUM; idx ++ )
	{
		/* create new speed queue. */
		if ( 0 == g_queue_speed[idx] )
		{
			g_queue_speed[idx] = actspeed;
			LIB_BLAPI_DEBUG("leave, return queueIndex=%d =====>\n", (idx + 1));
			return (idx + 1);
		}
	}

	LIB_BLAPI_DEBUG("leave, queue is full,return 0 =====>\n");
	return 0; /* queue is full. */
}

/* add dataspeend limit stream configure
pair detail see linux/qos_type.h.
*/
int dslimit_add_nodes_data(dslimit_param_t dslimitParam, struct dsnode g_dataspeed_nodes[], int g_queue_speed[])
{
	int idx  = 0, queue_idx = 0;
	int is_dsnode_full = 1;

	LIB_BLAPI_DEBUG("enter,mode=%d,pair=%d/%d/%d/%d,queue_ds=%d,iptype=%d======>\n",
		dslimitParam.mode, dslimitParam.pair0, dslimitParam.pair1, dslimitParam.pair2, dslimitParam.pair3, dslimitParam.queue_ds, dslimitParam.iptype);
	
	if ( dslimitParam.queue_ds <= 0 )
	{
		LIB_BLAPI_ERROR("leave, queue_ds<=0,return -1=====>\n");
		return -1; /* no limit */
	}
		
	for ( idx = 0; idx < DSLIMIT_MAX_NODES; idx ++ )
	{
		if ( 0 == g_dataspeed_nodes[idx].ds_used )
		{
			switch ( dslimitParam.mode)
			{
				case DSLIMIT_MODE_IFACE :
				case DSLIMIT_MODE_VLANID :
				case DSLIMIT_MODE_IPRANGE :
					queue_idx = dslimit_get_queue_index_and_speed(dslimitParam.queue_ds, g_queue_speed);
					if ( queue_idx <= 0 )
					{
						/* no limit or queue full */
						LIB_BLAPI_ERROR("leave,queue_idx <= 0,return -1=====>\n");
						return -1;
					}
						
					if ( DSLIMIT_MODE_IPRANGE == dslimitParam.mode )
					{
						if ( dslimitParam.iptype <= 0 )
						{
							/* ip type invalid*/
							LIB_BLAPI_ERROR("leave,iptype <= 0,return -1=====>\n");
							return -1; 
						}
						g_dataspeed_nodes[idx].ds_iptype = dslimitParam.iptype;
					}
					is_dsnode_full = 0;
					g_dataspeed_nodes[idx].ds_queue = queue_idx;
					g_dataspeed_nodes[idx].ds_queue_speed = dslimitParam.queue_ds * DS_QUEUE_PER_SPEED; /* kbps */
					g_dataspeed_nodes[idx].ds_used = 1;
					break;
				default :
					return -1;
			}

			switch ( dslimitParam.mode)
			{
				case DSLIMIT_MODE_IFACE :
					/* 1-4: LAN1-LAN4, 5-8: SSID1-SSID4 */
					g_dataspeed_nodes[idx].ds_lanif[0] = dslimitParam.pair0;
					break;
				case DSLIMIT_MODE_VLANID :
					g_dataspeed_nodes[idx].ds_vlanid[0] = dslimitParam.pair0;
					break;
				case DSLIMIT_MODE_IPRANGE :
					g_dataspeed_nodes[idx].ds_ip4addr[0] = dslimitParam.pair0;
					g_dataspeed_nodes[idx].ds_ip4addr[1] = dslimitParam.pair1;
					g_dataspeed_nodes[idx].ds_ip4addr[2] = dslimitParam.pair2;
					g_dataspeed_nodes[idx].ds_ip4addr[3] = dslimitParam.pair3;
					break;
				default :
					return -1;
			
			}
			break;
		}
	}

	if ( is_dsnode_full )
	{
		LIB_BLAPI_DEBUG("data speed limit nodes full=====>\n");
	}

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

/*
 set data speed limit nodes.
 up_dw: 1 for upstream  2 for downstrem
*/
int dslimit_set_dataspeed_nodes(int up_dw, struct dsnode g_dataspeed_nodes[])
{
	int fp = -1;
	int ret = -1;

	LIB_BLAPI_DEBUG("enter=====>\n");
	
	fp = open("/dev/"QOSDSLIMIT_DEV_NAME, O_RDONLY);
	if ( fp < 0 )
	{
		LIB_BLAPI_ERROR("leave,open %s error,return -1=====>\n",QOSDSLIMIT_DEV_NAME);
		return -1;
	}

	if ( DSLIMIT_UP == up_dw )
	{
		ret = ioctl(fp, QOSDSLIMIT_IOC_DATACFG_UP, g_dataspeed_nodes);
		if (ret < 0)
		{
			LIB_BLAPI_DEBUG("ioctl up error=====>\n");
		}
	}
	else if ( DSLIMIT_DW == up_dw )
	{
		ret = ioctl(fp, QOSDSLIMIT_IOC_DATACFG_DW, g_dataspeed_nodes);
		if ( ret < 0 )
		{
			LIB_BLAPI_DEBUG("ioctl down error=====>\n");
		}
	}
	close(fp);

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

/*
get LAN index & speed limit value
return code: -1 failed, 1-4: LAN1~LAN4, 5-8: SSID1~SSID4
*/
int8 dslimit_get_interfaceLimitVal(char *val, int *limit)
{
	char s_iface[20] = {0};
	char s_limitv[20] = {0};
	char s_iface_c[12] = {0};
	int i_limitv_len = 0;
	int idx = 0;

	if ( NULL == val || '\0' == val[0] || NULL == limit )
		return -1;

	if ( NULL == strstr(val, "/") )
	{
		return -1;
	}
	else
	{
		sscanf(val, "%[^/]/%s", s_iface, s_limitv);
	
		i_limitv_len = strlen(s_limitv);
		for ( idx = 0; idx < i_limitv_len; idx ++ )
		{
			if ( !(isdigit(s_limitv[idx])) )
				return -1;
		}
		*limit = atoi(s_limitv);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
		for ( idx = 1; idx <= 8; idx ++ )
		{
			snprintf(s_iface_c, sizeof(s_iface_c), "LAN%d", idx);
			if ( 0 == strcmp(s_iface_c, s_iface) )
				return idx;
		}
		for ( idx = 1; idx <= 4; idx ++ )
		{
			snprintf(s_iface_c, sizeof(s_iface_c), "SSID%d", idx);
			if ( 0 == strcmp(s_iface_c, s_iface) )
				return (idx+8);
		}
#else
		for ( idx = 1; idx <= 4; idx ++ )
		{
			snprintf(s_iface_c, sizeof(s_iface_c), "LAN%d", idx);
			if ( 0 == strcmp(s_iface_c, s_iface) )
				return idx;
			snprintf(s_iface_c, sizeof(s_iface_c), "SSID%d", idx);
			if ( 0 == strcmp(s_iface_c, s_iface) )
				return (idx+4);
		}
#endif
	}

	return -1;
}

/*
get VLANID & speed limit value
return code: -1 failed, 1-4094, 4096(untag)
*/
int dalimit_get_vlanidLimitVal(char *val, int *limit)
{
	char s_vlan[20] = {0};
	char s_limitv[20] = {0};
	int i_vlantag = 0;
	int tag_len = 0;
	int tag_maxlen = 4;
	int i_limitv_len = 0;
	int idx = 0;

	if ( NULL == val
		|| '\0' == val[0])
		return -1;

	if ( NULL == strstr(val, "/") )
	{
		return -1;
	}
	else
	{
		sscanf(val, "%[^/]/%s", s_vlan, s_limitv);

		i_limitv_len = strlen(s_limitv);
		for ( idx = 0; idx < i_limitv_len; idx ++ )
		{
			if ( !(isdigit(s_limitv[idx])) )
				return -1;
		}
		*limit = atoi(s_limitv);

		if ( 0 == strcmp("untagged", s_vlan) )
			return DS_VLANID_UNTAG;

		tag_len = strlen(s_vlan);
		if ( tag_len <= tag_maxlen )
		{
			i_vlantag = atoi(s_vlan);
			if ( i_vlantag >= 1 && i_vlantag <= 4094 )
				return i_vlantag;
		}
	}

	return -1;
}

/*
get ipv4 range
return code: -1 failed, 0 succeed
*/
int8 dslimit_get_rangeIPV4addr(char *v4addr, int *s_v4addr, int *e_v4addr)
{
	char start_addr[80];
	char end_addr[80];
	struct in_addr in_s_v4addr;
	struct in_addr in_e_v4addr;

	if ( NULL == v4addr
		|| '\0' == v4addr[0]
		|| NULL == s_v4addr
		|| NULL == e_v4addr)
		return -1;

	memset(&in_s_v4addr, 0, sizeof(struct in_addr));
	memset(&in_e_v4addr, 0, sizeof(struct in_addr));
	
	if ( NULL == strstr(v4addr, "-") ) /* single ip */
	{
		return -1;
	}
	else /* ip range */
	{
		sscanf(v4addr, "%[^-]-%s", start_addr, end_addr);

		if ( 1 != inet_pton(AF_INET, start_addr, &in_s_v4addr) )
			return -1;

		if ( 1 != inet_pton(AF_INET, end_addr, &in_e_v4addr) )
			return -1;

		if ( in_s_v4addr.s_addr > in_e_v4addr.s_addr )
			return -1;
	}

	memcpy(s_v4addr, &in_s_v4addr, sizeof(in_s_v4addr));
	memcpy(e_v4addr, &in_e_v4addr, sizeof(in_e_v4addr));

	return 0;
}


/*
get ipv6 range
return code: -1 failed, 0 succeed
*/
int8 dslimit_get_rangeIPV6addr(char *v6addr,int *pair0, int *pair1, int *pair2, int *pair3)
{
	char start_addr[80];
	char end_addr[80];
	struct in6_addr in_s_v6addr;
	struct in6_addr in_e_v6addr;
	unsigned char *ptr = NULL;
	
	if ( NULL == v6addr
		|| '\0' == v6addr[0]
		|| NULL == pair0
		|| NULL == pair1
		|| NULL == pair2
		|| NULL == pair3)
		return -1;

	memset(&in_s_v6addr, 0, sizeof(struct in6_addr));
	memset(&in_e_v6addr, 0, sizeof(struct in6_addr));
	
	if ( NULL == strstr(v6addr, "-") ) /* single ip */
	{
		return -1;
	}
	else /* ip range */
	{
		sscanf(v6addr, "%[^-]-%s", start_addr, end_addr);
		if ( 1 != inet_pton(AF_INET6, start_addr, &in_s_v6addr) )
			return -1;

		if ( 1 != inet_pton(AF_INET6, end_addr, &in_e_v6addr) )
			return -1;

		if ( memcmp(&in_s_v6addr, &in_e_v6addr, sizeof(struct in6_addr)) > 0 )
			return -1;
	}

	ptr = (unsigned char *)&in_s_v6addr;  
	memcpy(pair0, ptr, 4);
	ptr += 4;
	memcpy(pair1, ptr, 4);
	ptr += 4;
	memcpy(pair2, ptr, 4);
	ptr += 4;
	memcpy(pair3, ptr, 4);

	return 0;
}

/*
get ip range & speed limit value
return code: -1 failed
*/
int dslimit_get_iprangeLimitVal(char *val, int *pair0, int *pair1, int *pair2, int *pair3, int *limit)
{
	char s_ip[180] = {0};
	char s_limitv[180] = {0};
	int i_limitv_len = 0;
	int idx = 0;

	if ( NULL == val
		|| '\0' == val[0])
		return -1;

	if ( NULL == strstr(val, "/") )
	{
		return -1;
	}
	else
	{
		sscanf(val, "%[^/]/%s", s_ip, s_limitv);

		i_limitv_len = strlen(s_limitv);
		for ( idx = 0; idx < i_limitv_len; idx ++ )
		{
			if ( !(isdigit(s_limitv[idx])) )
				return -1;
		}
		*limit = atoi(s_limitv);

		if ( strlen(s_ip) > 79 )
			return -1;

		if ( 0 == dslimit_get_rangeIPV4addr(s_ip, pair0, pair1) )
			return DS_IPTYPE_IPV4;

		if ( 0 == dslimit_get_rangeIPV6addr(s_ip, pair0, pair1, pair2, pair3) )
			return DS_IPTYPE_IPV6;
	}

	return -1;
}

int dslimit_parse_interface_value(int up_dw, char* lanIf, struct dsnode g_dataspeed_nodes[], int g_queue_speed[])
{
	char *pos = NULL;
	int val_len = 0, lanidx = 0, lanlimit = 0;
	const int limitval_maxlen = 20;
	char cmd[128] = {0};
	dslimit_param_t dslimitParam;

	if(lanIf == NULL)
	{
		LIB_BLAPI_ERROR("leave,lanIf= NULL,return -1=====>\n");
		return -1;
	}
	else
	{
		LIB_BLAPI_DEBUG("enter,up_dw=%d,lanIf=%s=====>\n",up_dw, lanIf);
	}
	
	pos = strtok(lanIf, ",");
	while ( pos )
	{
		val_len = strlen(pos);
		if ( val_len > limitval_maxlen )
		{
			LIB_BLAPI_ERROR("leave,invalid item len, val=[%s],return -1=====>\n",pos);
			return -1;
		}

		lanidx = 0;
		lanlimit = 0;
		if ( -1 == (lanidx = dslimit_get_interfaceLimitVal(pos, &lanlimit)) )
		{
			LIB_BLAPI_ERROR("leave,invalid item val chk=[%s],return -1=====>\n",pos);
			return -1;
		}
		if(memcmp(pos, "SSID", 4) == 0) 
		{
			LIB_BLAPI_DEBUG("cmd=%s=====>\n",cmd);
			sprintf(cmd, "echo ra%d %s %d > /proc/tc3162/wlan_ssid_ratelimit", pos[4]-'0'-1, (up_dw == DSLIMIT_UP)?("rx"):("tx"), lanlimit * DS_QUEUE_PER_SPEED);
			system(cmd);
		}

		bzero(&dslimitParam, sizeof(dslimit_param_t));
		dslimitParam.mode = DSLIMIT_MODE_IFACE;
		dslimitParam.pair0 = lanidx;
		dslimitParam.pair1 = 0;
		dslimitParam.pair2 = 0;
		dslimitParam.pair3 = 0;
		dslimitParam.queue_ds = lanlimit;
		dslimitParam.iptype = 0;
		dslimit_add_nodes_data(dslimitParam, g_dataspeed_nodes, g_queue_speed);

		pos = strtok(NULL, ",");
	}

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

int dslimit_parse_vlanID_value(char* vanID, struct dsnode g_dataspeed_nodes[], int g_queue_speed[])
{
	char *pos = NULL;
	int val_len = 0, vlan_id = 0, vlanid_limit = 0;
	const int limitval_maxlen = 20;
	dslimit_param_t dslimitParam;

	if(vanID == NULL)
	{
		LIB_BLAPI_ERROR("leave,vanID= NULL,return -1=====>\n");
		return -1;
	}
	else
	{
		LIB_BLAPI_DEBUG("enter,lanIf=%s=====>\n", vanID);
	}
	
	pos = strtok(vanID, ",");
	while ( pos )
	{
		val_len = strlen(pos);
		if ( val_len > limitval_maxlen )
		{
			LIB_BLAPI_ERROR("leave,invalid item len, val=[%s],return -1=====>\n",pos);
			return -1;
		}
		vlan_id= 0;
		vlanid_limit = 0;
		if ( (vlan_id = dalimit_get_vlanidLimitVal(pos, &vlanid_limit)) <= 0 )
		{
			LIB_BLAPI_ERROR("leave,invalid item val chk=[%s],return -1=====>\n",pos);
			return -1;
		}

		bzero(&dslimitParam, sizeof(dslimit_param_t));
		dslimitParam.mode = DSLIMIT_MODE_VLANID;
		dslimitParam.pair0 = vlan_id;
		dslimitParam.pair1 = 0;
		dslimitParam.pair2 = 0;
		dslimitParam.pair3 = 0;
		dslimitParam.queue_ds = vlanid_limit;
		dslimitParam.iptype = 0;
		dslimit_add_nodes_data(dslimitParam, g_dataspeed_nodes, g_queue_speed);

		pos = strtok(NULL, ",");
	}

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

int dslimit_parse_ipRange_value(char* ipRange, struct dsnode g_dataspeed_nodes[], int g_queue_speed[])
{
	char *pos = NULL;
	int val_len = 0, iprange_type = 0, iprange_limit = 0;
	int pair0 = 0, pair1 = 0, pair2 = 0, pair3 = 0;
	const int limitval_maxlen = 175;
	dslimit_param_t dslimitParam;

	if(ipRange == NULL)
	{
		LIB_BLAPI_ERROR("leave,vanID= NULL,return -1=====>\n");
		return -1;
	}
	else
	{
		LIB_BLAPI_DEBUG("enter,ipRange=%s=====>\n",ipRange);
	}
	
	pos = strtok(ipRange, ",");
	while ( pos )
	{
		val_len = strlen(pos);
		if ( val_len > limitval_maxlen )
		{
			LIB_BLAPI_ERROR("leave,invalid item len, val=[%s],return -1=====>\n",pos);
			return -1;
		}
		iprange_type = 0;
		iprange_limit = 0;
		pair0 = 0;
		pair1 = 0;
		pair2 = 0;
		pair3 = 0;
		if ( (iprange_type = dslimit_get_iprangeLimitVal(pos, &pair0, &pair1, &pair2, &pair3, &iprange_limit)) <= 0 )
		{
			LIB_BLAPI_ERROR("leave,invalid item val chk=[%s],return -1=====>\n",pos);
			return -1;
		}

		bzero(&dslimitParam, sizeof(dslimit_param_t));
		dslimitParam.mode = DSLIMIT_MODE_IPRANGE;
		dslimitParam.pair0 = pair0;
		dslimitParam.pair1 = pair1;
		dslimitParam.pair2 = pair2;
		dslimitParam.pair3 = pair3;
		dslimitParam.queue_ds = iprange_limit;
		dslimitParam.iptype = iprange_type;
		dslimit_add_nodes_data(dslimitParam, g_dataspeed_nodes, g_queue_speed);

		pos = strtok(NULL, ",");
	}	

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}


int dslimit_parse_dsValue(int updwMode, int dsMode, char* dsValue, struct dsnode g_dataspeed_nodes[], int g_queue_speed[] )
{
	int ret = -1;
	
	if(dsValue == NULL)
	{
		LIB_BLAPI_ERROR("enter,dsValue= NULL,return -1=====>\n");
		return -1;
	}
	
	if(strlen(dsValue) == 0)
	{
		LIB_BLAPI_DEBUG("enter,dsValue lenth= 0,return 0=====>\n");
		return 0;
	}
	
	LIB_BLAPI_DEBUG("enter,dsMode=%d,updwMode=%d,dsValue=%s=====>\n",dsMode, updwMode,dsValue);
	
	if(dsMode == DSLIMIT_MODE_IFACE )
	{
		ret =  dslimit_parse_interface_value(updwMode, dsValue, g_dataspeed_nodes, g_queue_speed);
	}
	else if(dsMode == DSLIMIT_MODE_VLANID )
	{
		ret = dslimit_parse_vlanID_value( dsValue, g_dataspeed_nodes, g_queue_speed);	
	}
	else if(dsMode == DSLIMIT_MODE_IPRANGE )
	{
		ret = dslimit_parse_ipRange_value(dsValue, g_dataspeed_nodes, g_queue_speed);		
	}
	
	LIB_BLAPI_DEBUG("leave,return %d=====>\n",ret);
	return ret;
}
#endif
