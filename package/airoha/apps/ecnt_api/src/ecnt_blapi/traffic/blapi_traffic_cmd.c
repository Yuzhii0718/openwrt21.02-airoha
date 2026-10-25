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
* @file  blapi_traffic_cmd.c  
* @brief This program gives cmd apis from blapi_traffic.
* @author Zhen.Xu
* @date     2020-07-14
* @version  A001 
* @copyright EcoNet Inc                                                              
*/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ecnt_lib_dbg.h>
#include "blapi_traffic.h"
#include "blapi_traffic_cmd.h"
#include "libapi_lib_utility.h"
#include "libapi_lib_qosrule.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define DSLIMIT_MAX_QUEUE_NUM 6

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

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
static const cmds_t blapi_traffic_cmd[] = 
{
	{"get_multicast_acnt", do_blapi_traffic_get_multicast_acnt,0,0,NULL},
	{"get_wan_acnt", do_blapi_traffic_get_wan_acnt,0,0,NULL},
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	{"get_wan_acnt_by_name", do_blapi_traffic_get_wan_acnt_by_name,0,0,NULL},
#endif
	{"get_flowid_cnt", do_blapi_traffic_get_flowid_cnt,0,0,NULL},
	{"get_llid_cnt", do_blapi_traffic_get_llid_cnt,0,0,NULL},
	{"get_flowid_byte_pkt_cnt", do_blapi_traffic_get_flowid_byte_pkt_cnt,0,0,NULL},
	{"get_dev_bandwidth", do_blapi_traffic_get_dev_bandwidth,0,0,NULL},
	{"get_dev_acnt_by_mac", do_blapi_traffic_get_dev_acnt_by_mac,0,0,NULL},
	{"get_ether_port_num", do_blapi_traffic_get_ether_port_num,0,0,NULL},
	{"get_portrate_info", do_blapi_traffic_get_portrate_info,0,0,NULL},
	{"get_lanport_info", do_blapi_traffic_get_lanport_info,0,0,NULL},
	{"get_pppoe_info", do_blapi_traffic_get_pppoe_info,0,0,NULL},
	{"get_port_rx_byte_cnt", do_blapi_traffic_get_port_rx_byte_cnt,0,0,NULL},
	{"get_port_tx_byte_cnt", do_blapi_traffic_get_port_tx_byte_cnt,0,0,NULL},
	{"get_ether_wan_state", do_blapi_traffic_get_ether_wan_state,0,0,NULL},
	{"get_ratelimit_mode", do_blapi_traffic_get_ratelimit_mode,0,0,NULL},
	{"check_loop_detect", do_blapi_traffic_check_loop_detect,0,0,NULL},
	{"check_eth_link_status", do_blapi_traffic_check_eth_link_status,0,0,NULL},
	{"check_eth_port_status", do_blapi_traffic_check_eth_port_status,0,0,NULL},
	{"set_black_ip_port_proto", do_blapi_traffic_set_black_ip_port_protocol,0,0,NULL},
	{"set_wan_acnt_enable", do_blapi_traffic_set_wan_acnt_enable,0,0,NULL},
	{"set_flow_acnt_enable", do_blapi_traffic_set_flow_acnt_enable,0,0,NULL},
	{"set_llid_acnt_enable", do_blapi_traffic_set_llid_acnt_enable,0,0,NULL},
	{"set_dev_acnt_enable", do_blapi_traffic_set_dev_acnt_enable,0,0,NULL},
	{"set_wan_ratelimit", do_blapi_traffic_set_wan_ratelimit,0,0,NULL},
	{"set_wan_rate_threshold", do_blapi_traffic_set_wan_rate_threshold,0,0,NULL},
	{"set_wan_mtu_val", do_blapi_traffic_set_wan_mtu_val,0,0,NULL},
	{"set_ratelimit_mode", do_blapi_traffic_set_ratelimit_mode,0,0,NULL},
	{"set_flow_ratelimit", do_blapi_traffic_set_flowid_ratelimit,0,0,NULL},
	{"set_dev_ratelimit", do_blapi_traffic_set_dev_ratelimit,0,0,NULL},
	{"set_max_bandwidth_data", do_blapi_traffic_set_max_bandwidth_data,0,0,NULL},
	{"set_traffic_class", do_blapi_traffic_set_traffic_class,0,0,NULL},
	{"set_wifiIFRatelimit", do_blapi_traffic_set_wifi_interface_ratelimit,0,0,NULL},
	{"set_switch_linkmode", do_blapi_traffic_set_switch_linkmode,0,0,NULL},
	{"set_broadcast_ratelimit", do_blapi_traffic_set_broadcast_ratelimit,0,0,NULL},
	{"set_EtherwanPortmap", do_blapi_traffic_set_etherwan_portmap,0,0,NULL},
	{"set_general_rx_ratelimit", do_blapi_traffic_set_general_rx_ratelimit,0,0,NULL},
	{"set_black_ip_for_hwnat", do_blapi_traffic_set_black_ip_for_hwnat,0,0,NULL},
	{"set_alg_rtsp_ports", do_blapi_traffic_set_alg_rtsp_port,0,0,NULL},
	{"set_DefaultWanInterface", do_blapi_traffic_set_default_wan_interface,0,0,NULL},
	{"uninstall_mirror_traffic", do_blapi_traffic_uninstall_mirror_traffic,0,0,NULL},
	{"set_wanIF_qostype", do_blapi_traffic_set_wanInterface_qostype,0,0,NULL},
	{"set_WanIFQosTemplate", do_blapi_traffic_set_wanInterface_qos_template,0,0,NULL},
#if defined(TCSUPPORT_CT_DS_LIMIT)
	{"set_dslimit_data", do_blapi_traffic_set_dslimit_data,0,0,NULL},
#endif
	{"set_lan_info", do_blapi_traffic_set_lan_info,0,0,NULL},
	{"get_wan_tx_stats", do_blapi_traffic_get_wan_tx_stats,0,0,NULL},
	{"get_wan_rx_stats", do_blapi_traffic_get_wan_rx_stats,0,0,NULL},
	{"set_vip", do_blapi_traffic_set_vip,0,0,NULL},	
	{"del_vip", do_blapi_traffic_del_vip,0,0,NULL},	
	{"nptv6", do_blapi_traffic_nptv6,0,0,NULL},
	{"set_wan_mtu_7523", do_blapi_traffic_set_wan_mtu,0,0,NULL},
	{"get_lan_traffic_stats", do_blapi_traffic_get_lan_pm_stats,0,0,NULL},	
	{"get_wan_traffic_stats", do_blapi_traffic_get_wan_pm_stats,0,0,NULL},
	{"set_assure_max_bandwidth", do_blapi_traffic_set_assure_max_bandwidth,0,0,NULL},
	{"set_sla_cfg", do_blapi_traffic_set_sla_cfg,0,0,NULL},
	{"get_active_ether_wan_state", do_blapi_traffic_get_active_ether_wan_state,0,0,NULL},
	{"get_hsgmii_lan_state", do_blapi_traffic_get_hsgmii_lan_state,0,0,NULL},
	{"get_hsgmii_lan_tx_stats", do_blapi_traffic_get_hsgmii_lan_tx_stats,0,0,NULL},
	{"get_hsgmii_lan_rx_stats", do_blapi_traffic_get_hsgmii_lan_rx_stats,0,0,NULL},
	{"mdio_read", do_blapi_traffic_mdio_read,0,0,NULL},
	{"mdio_write", do_blapi_traffic_mdio_write,0,0,NULL},
	{"set_wan_acnt_clear", do_blapi_traffic_set_wan_acnt_clear,0,0,NULL},
	{"add_vlan_filter_vlan", do_blapi_traffic_add_vlan_filter_vlan, 0, 0, NULL},
	{"del_vlan_filter_vlan", do_blapi_traffic_del_vlan_filter_vlan, 0, 0, NULL},
	{"get_port_rx_cnt", do_blapi_traffic_get_port_rx_cnt, 0, 0, NULL},
	{"get_port_tx_cnt", do_blapi_traffic_get_port_tx_cnt, 0, 0, NULL},
	{"set_hsgmii_port_ratelimit", do_blapi_traffic_set_hsgmii_port_ratelimit, 0, 0, NULL},
	{"get_wan_port_statistics", do_blapi_traffic_get_wan_port_statistics,0,0,NULL},
	{"get_wan_port_link_state", do_blapi_traffic_get_wan_port_link_state,0,0,NULL},
	{"set_wan_port_link_mode", do_blapi_traffic_set_wan_port_link_mode,0,0,NULL},
	{"set_dev_stat_ratelim_mode", do_blapi_traffic_set_dev_stat_ratelimit_mode, 0, 0, NULL},
	{NULL,NULL,0,0,NULL}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int do_blapi_traffic_set_black_ip_port_protocol(int argc, char *argv[], void *p)
{
	unsigned int ip, port, protocol;
	unsigned int ipv4_add[4] = {0};
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_port_proto add/del <ip(X.X.X.X,255.255.255.255 means all ip)> <port(0~65536,65536 means all port)> <proto(1:TCP, 2:UDP, 3:ICMP, 4:ALL)>\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_port_proto del_all/show \n");
		return -1;
	}
	else
	{
		if((strcmp(argv[1], "add") == 0) || (strcmp(argv[1], "del") == 0))
		{
			if(argc < 5)
			{
				ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_port_proto add/del <ip(X.X.X.X,255.255.255.255 means all ip)> <port(0~65536,65536 means all port)> <proto(1:TCP, 2:UDP, 3:ICMP, 4:ALL)>\n");
				return -1;
			}	
		}
	}

	if(strcmp(argv[1], "show") == 0)
		ret = blapi_traffic_show_black_ip_port_protocol();
	else if(strcmp(argv[1], "del_all") == 0)
		ret = blapi_traffic_delete_all_black_ip_port_protocol();
	else if((strcmp(argv[1], "add") == 0) || (strcmp(argv[1], "del") == 0))
	{
		sscanf(argv[2], "%d.%d.%d.%d", &ipv4_add[0],  &ipv4_add[1],  &ipv4_add[2],  &ipv4_add[3]);
		ip = ipv4_add[0]<<24 | ipv4_add[1]<<16 | ipv4_add[2]<<8 | ipv4_add[3];

		port = atoi(argv[3]);
		protocol = atoi(argv[4]);

		if(strcmp(argv[1], "add") == 0)
			ret = blapi_traffic_add_black_ip_port_protocol(ip, port, protocol);
		else
			ret = blapi_traffic_delete_black_ip_port_protocol(ip, port, protocol);
	}
	else
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_port_proto add/del <ip(X.X.X.X,255.255.255.255 means all ip)> <port(0~65536,65536 means all port)> <proto(1:TCP, 2:UDP, 3:ICMP, 4:ALL)>\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_port_proto del_all/show\n");
		return -1;
	}
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set black ip port protocol fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set black ip port protocol sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_wan_ratelimit(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 5)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_wan_ratelimit wanname wanid up_rate dn_rate\n");
		return -1;
	}
	
	ret = blapi_traffic_set_wan_ratelimit(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wan ratelimit fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wan ratelimit sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_flowid_ratelimit(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 4)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_flow_ratelimit dir<0:up/1:down> flowid ratelimit\n");
		return -1;
	}
	
	ret = blapi_traffic_set_flowid_ratelimit(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set flow ratelimit fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set flow ratelimit sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_dev_ratelimit(int argc, char *argv[], void *p)
{
	int ret = 0, i = 0, index = 0, bandwidth_cnt = 0;
	dev_bandwidth_tmp_t dev_mac_ratelimit;

	memset(&dev_mac_ratelimit, 0, sizeof(dev_bandwidth_tmp_t));

	if(argc < 5)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_dev_ratelimit cnt mac uprate downrate ...\n");
		return -1;
	}

	bandwidth_cnt = atoi(argv[1]);

	for(i = 2;i < argc && index < bandwidth_cnt; i++)
	{
		sscanf(argv[i], "%02X%02X%02X%02X%02X%02X",&(dev_mac_ratelimit.trtcm_acl_mac_table[index].dev_mac[0])
			, &(dev_mac_ratelimit.trtcm_acl_mac_table[index].dev_mac[1])
			, &(dev_mac_ratelimit.trtcm_acl_mac_table[index].dev_mac[2])
			, &(dev_mac_ratelimit.trtcm_acl_mac_table[index].dev_mac[3])
			, &(dev_mac_ratelimit.trtcm_acl_mac_table[index].dev_mac[4])
			, &(dev_mac_ratelimit.trtcm_acl_mac_table[index].dev_mac[5]));
		dev_mac_ratelimit.trtcm_acl_mac_table[index].us_queue_speed = atoi(argv[++i]);
		dev_mac_ratelimit.trtcm_acl_mac_table[index].ds_queue_speed = atoi(argv[++i]);
		index ++;
	}
	
	ret = blapi_traffic_set_dev_ratelimit_by_mac(&dev_mac_ratelimit, bandwidth_cnt);
		
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set dev ratelimit fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set dev ratelimit sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_get_multicast_acnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long long cnt = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_multicast_acnt 0/1\n");
		return -1;
	}
	
	ret = blapi_traffic_get_multicast_acnt(atoi(argv[1]), &cnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get multicast acnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: multicast acnt = %lld\n", __FUNCTION__, __LINE__, cnt);

	return 0;
}

int do_blapi_traffic_get_wan_acnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	wan_acnt_t cnt;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_wan_acnt 0~31\n");
		return -1;
	}
	
	ret = blapi_traffic_get_wan_acnt(atoi(argv[1]), &cnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get multicast acnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d:" 
			"cnt.mulRxBytes = %lld\n"
			"cnt.uniRxBytes = %lld\n"
			"cnt.mulTxBytes = %lld\n"
			"cnt.uniTxBytes = %lld\n"
			"cnt.uniRxCnt = %lld\n"
			"cnt.mulRxCnt = %lld\n"
			"cnt.uniTxCnt = %lld\n"
			"cnt.mulTxCnt = %lld\n"
			"cnt.txRate = %lld\n"
			"cnt.rxRate = %lld\n"
			"cnt.jiffies = %ld\n",
			__FUNCTION__, __LINE__, 
			cnt.mulRxBytes,
			cnt.uniRxBytes,
			cnt.mulTxBytes,
			cnt.uniTxBytes,
			cnt.uniRxCnt,
			cnt.mulRxCnt,
			cnt.uniTxCnt,
			cnt.mulTxCnt,
			cnt.txRate,
			cnt.rxRate,
			cnt.jiffies);

	return 0;
}

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
int do_blapi_traffic_get_wan_acnt_by_name(int argc, char *argv[], void *p)
{
	int ret = 0;
	wan_acnt_t cnt;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_wan_acnt_by_name <name>\n");
		return -1;
	}
	
	ret = blapi_traffic_get_wan_acnt_by_name(argv[1], &cnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get multicast acnt fail\n", __FUNCTION__, __LINE__);
	else
	{
		ecnt_blapi_printf("\r\n[%s]%d:\n" 
			"cnt.mulRxBytes = %lld\n"
			"cnt.uniRxBytes = %lld\n"
			"cnt.mulTxBytes = %lld\n"
			"cnt.uniTxBytes = %lld\n",
			__FUNCTION__, __LINE__, 
			cnt.mulRxBytes,
			cnt.uniRxBytes,
			cnt.mulTxBytes,
			cnt.uniTxBytes);

		ecnt_blapi_printf("cnt.uniRxCnt = %lld\n"
			"cnt.mulRxCnt = %lld\n"
			"cnt.uniTxCnt = %lld\n"
			"cnt.mulTxCnt = %lld\n"
			"cnt.txRate = %lld\n"
			"cnt.rxRate = %lld\n"
			"cnt.jiffies = %ld\n",
			cnt.uniRxCnt,
			cnt.mulRxCnt,
			cnt.uniTxCnt,
			cnt.mulTxCnt,
			cnt.txRate,
			cnt.rxRate,
			cnt.jiffies);
	}

	return 0;
}
#endif

int do_blapi_traffic_get_flowid_cnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long long cnt = 0;
	int flag = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_flowid_cnt 0/1<0:down/1:up> flowid\n");
		return -1;
	}
	
	ret = blapi_traffic_get_flowid_cnt(atoi(argv[1]), atoi(argv[2]), &cnt, &flag);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get flowid cnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: cnt = 0x%llx, flag = %d\n", __FUNCTION__, __LINE__, cnt, flag);

	return 0;
}

int do_blapi_traffic_get_flowid_byte_pkt_cnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long long byte_cnt = 0;
	unsigned long long pkt_cnt = 0;

	int flag = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_flowid_byte_pkt_cnt 0/1<0:down/1:up> flowid\n");
		return -1;
	}
	
	ret = blapi_traffic_get_flowid_byte_pkt_cnt(atoi(argv[1]), atoi(argv[2]), &byte_cnt, &pkt_cnt, &flag);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get flowid cnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: byte_cnt = %lld, pkt_cnt = %lld, flag = %d\n", __FUNCTION__, __LINE__, byte_cnt, pkt_cnt, flag);
	return 0;
}

int do_blapi_traffic_get_llid_cnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long long cnt = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_llid_cnt 0/1<0:down/1:up> llid\n");
		return -1;
	}
	
	ret = blapi_traffic_get_llid_cnt(atoi(argv[1]), atoi(argv[2]), &cnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get llid cnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: cnt = 0x%llx\n", __FUNCTION__, __LINE__, cnt);

	return 0;
}

int do_blapi_traffic_set_wan_acnt_enable(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_wan_acnt_enable 0/1\n");
		return -1;
	}
	
	ret = blapi_traffic_set_wan_acnt_enable(atoi(argv[1]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wan acnt enable fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wan acnt enable sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_flow_acnt_enable(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_flow_acnt_enable 0/1\n");
		return -1;
	}
	
	ret = blapi_traffic_set_flowid_acnt_enable(atoi(argv[1]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set flow acnt enable fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set flow acnt enable sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_llid_acnt_enable(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_llid_acnt_enable 0/1\n");
		return -1;
	}
	
	ret = blapi_traffic_set_llid_acnt_enable(atoi(argv[1]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set llid acnt enable fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set llid acnt enable sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_dev_acnt_enable(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_dev_acnt_enable sw_acnt_en<0/1> hw_acnt_en<0/1>\n");
		return -1;
	}
	
	ret = blapi_traffic_set_dev_acnt_enable(atoi(argv[1]), atoi(argv[2]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set dev acnt enable fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set dev acnt enable sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_wan_rate_threshold(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_wan_rate_threshold 0/1\n");
		return -1;
	}
	
	ret = blapi_traffic_set_wan_rate_threshold(atoi(argv[1]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wan rate threshold fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wan rate threshold sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_wan_mtu_val(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_wan_mtu_val wan_type wan_mtu\n");
		return -1;
	}
	
	ret = blapi_traffic_set_wan_mtu_val(argv[1], argv[2]);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wan mtu val fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wan mtu val sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_ratelimit_mode(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_ratelimit_mode 0/1/2<0:dev mode;1:wan mode;2 traffic qos mode> 0/1/2<meter_group>\n");
#else
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_ratelimit_mode 0/1/2<0:dev mode;1:wan mode;2 traffic qos mode>\n");
#endif
		return -1;
	}
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	ret = blapi_traffic_set_meter_ratelimit_mode(atoi(argv[1]), atoi(argv[2]));
#else
	ret = blapi_traffic_set_ratelimit_mode(atoi(argv[1]));
#endif	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set ratelimit mode fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set ratelimit mode sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_max_bandwidth_data(int argc, char *argv[], void *p)
{
	int ret = 0;
	int i = 0;
	char *str = NULL;
	struct MaxBandWidth_node_info_s MaxBandWidthNodeInfo[TRTCM_MATCH_MAC_RULE_NUM];

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_max_bandwidth_data 0/1 mac_uprate_downrate\n");
		return -1;
	}

	str = strtok(argv[2], ",");

	while(str != NULL)
	{
		if(i == TRTCM_MATCH_MAC_RULE_NUM)
			break;

		bzero(MaxBandWidthNodeInfo[i].macAddr, sizeof(MaxBandWidthNodeInfo[i].macAddr));
		bzero(MaxBandWidthNodeInfo[i].upRate, sizeof(MaxBandWidthNodeInfo[i].upRate));
		bzero(MaxBandWidthNodeInfo[i].downRate, sizeof(MaxBandWidthNodeInfo[i].downRate));

		sscanf(str, "%[^_]_%[^_]_%s", MaxBandWidthNodeInfo[i].macAddr, MaxBandWidthNodeInfo[i].upRate, MaxBandWidthNodeInfo[i].downRate);

		i++;
		str = strtok(NULL, ",");
	}
	
	ret = blapi_traffic_set_max_bandwidth_data(atoi(argv[1]), MaxBandWidthNodeInfo, 0);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set max bandwidth data fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set max bandwidth data sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_get_dev_bandwidth(int argc, char *argv[], void *p)
{
	int ret = 0;
	int i = 0;
	int index = 0;
	DevBandWidth_Info_t DevBandWidth_Info[30];
	
	/* initialize DevBandWidth_Info */
	for (index = 0; index < 30; ++index)
	{
		DevBandWidth_Info[index].UpRate = 0;
		DevBandWidth_Info[index].DwRate = 0;
		memset (&DevBandWidth_Info[index].mac, '\0', sizeof(char) * 20);
	}
	
	ret = blapi_traffic_get_dev_bandwidth(DevBandWidth_Info);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get dev bandwidth fail\n", __FUNCTION__, __LINE__);
	else
	{
		while(DevBandWidth_Info[i].mac[0] != '\0')
		{
			ecnt_blapi_printf("\r\n[%s]%d: UpRate = %llu, DwRate = %llu, mac = %s\n", 
				__FUNCTION__, __LINE__, DevBandWidth_Info[i].UpRate, DevBandWidth_Info[i].DwRate, DevBandWidth_Info[i].mac);
			i++;

			if(i == 30)
				break;
		}
	}

	return 0;
}

int do_blapi_traffic_get_dev_acnt_by_mac(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long long rxbytes = 0;
	unsigned long long txbytes = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_dev_acnt_by_mac 0/1 mac\n");
		return -1;
	}
	
	ret = blapi_traffic_get_dev_acnt_by_mac(atoi(argv[1]), argv[2],&rxbytes, &txbytes);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get dev acnt by mac fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: rxbytes = %lld, txbytes = %lld\n", __FUNCTION__, __LINE__, rxbytes, txbytes);

	return 0;
}

int do_blapi_traffic_set_traffic_class(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 5)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_traffic_class Pbit/DSCP/TOS/VLANID minVal maxVal 0/1\n");
		return -1;
	}
	
	ret = blapi_traffic_set_traffic_class(argv[1], atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set traffic class fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set traffic class sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_wifi_interface_ratelimit(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 4)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_wifiIFRatelimit dir wifiIdx rate\n");
		return -1;
	}
	
	ret = blapi_traffic_set_wifi_interface_ratelimit(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wifi interface ratelimit fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wifi interface ratelimit sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_check_loop_detect(int argc, char *argv[], void *p)
{
	int ret = 0;
	char Status[4] = {0};
	char detectionStatus[20] = {0};

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic check_loop_detect eth0./ra\n");
		return -1;
	}
	
	ret = blapi_traffic_check_loop_detect(argv[1], Status, detectionStatus);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: check loop detect fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: Status = %s, detectionStatus = %s \n", __FUNCTION__, __LINE__, Status, detectionStatus);

	return 0;
}

int do_blapi_traffic_set_switch_linkmode(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 5)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_switch_linkmode Yes/No switchport_id 10/100 Half/Full\n");
		return -1;
	}
	
	ret = blapi_traffic_set_switch_linkmode(argv[1], atoi(argv[2]), argv[3], argv[4]);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set switch linkmode fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set switch linkmode sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_broadcast_ratelimit(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_broadcast_ratelimit port_index<1/2/4/8> rate\n");
		return -1;
	}
	
	ret = blapi_traffic_set_broadcast_ratelimit(atoi(argv[1]), atoi(argv[2]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set broadcast ratelimit fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set broadcast ratelimit sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_etherwan_portmap(int argc, char *argv[], void *p)
{
	int ret = 0;

	
	ret = blapi_traffic_set_default_switch_portmap();
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set etherwan portmap fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set etherwan portmap sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_general_rx_ratelimit(int argc, char *argv[], void *p)
{
	int ret = 0;

	
	ret = blapi_traffic_set_general_rx_ratelimit();
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set general rx ratelimit fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set general rx ratelimit sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_get_ether_port_num(int argc, char *argv[], void *p)
{
	int ret = 0;
	int portNum = 0;
	
	ret = blapi_traffic_get_ether_port_num(&portNum);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get ether port num fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: portNum = %d\n", __FUNCTION__, __LINE__, portNum);

	return 0;
}

int do_blapi_traffic_set_black_ip_for_hwnat(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int ip, i;	
	unsigned char u6_addr8[16];	
	unsigned int ipv4_add[4] = {0};	
	unsigned int ipv6_add[16] = {0};

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat ipv4 ip(x.x.x.x) mask(0~32) type(0~2) sign(0~31)\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat ipv6 ip(x:x:x:x:x:x:x:x) mask(0~128) type(0~2) sign(0~31)\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat del_sign sign\n");
		return -1;
	}
	else
	{
		if(strcmp(argv[1], "del_sign") == 0)
		{
			if(argc < 3)
			{
				ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat del_sign sign\n");
				return -1;
			}
		}
		else
		{
			if(argc < 6)
			{
				ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat ipv4 ip(x.x.x.x) mask(0~32) type(0~2) sign(0~31)\n");
				ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat ipv6 ip(x:x:x:x:x:x:x:x) mask(0~128) type(0~2) sign(0~31)\n");
				return -1;
			}
		}
	}
	
	if(strcmp(argv[1], "ipv4") == 0)
	{
		sscanf(argv[2], "%d.%d.%d.%d", &ipv4_add[0],&ipv4_add[1],&ipv4_add[2],&ipv4_add[3]);		
		ip = ipv4_add[0]<<24 | ipv4_add[1]<<16 | ipv4_add[2]<<8 | ipv4_add[3];
		ret = blapi_traffic_black_ip_for_hwnat_add(ip, atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	}
	else if(strcmp(argv[1], "ipv6") == 0)
	{
		sscanf(argv[2], "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X",			   
			&(ipv6_add[0]), &(ipv6_add[1]), &(ipv6_add[2]), &(ipv6_add[3]),			   
			&(ipv6_add[4]), &(ipv6_add[5]), &(ipv6_add[6]), &(ipv6_add[7]),			  
			&(ipv6_add[8]), &(ipv6_add[9]), &(ipv6_add[10]), &(ipv6_add[11]), 			
			&(ipv6_add[12]), &(ipv6_add[13]), &(ipv6_add[14]), &(ipv6_add[15]));	   
		for(i = 0; i < 16; i++)
		{ 		   
			u6_addr8[i] = (unsigned char)ipv6_add[i];		
		}

		ret = blapi_traffic_black_ipv6_for_hwnat_add(u6_addr8, atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	}
	else if(strcmp(argv[1], "del_sign") == 0)
		ret = blapi_traffic_black_ip_for_hwnat_delete(atoi(argv[2]));
	else
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat ipv4 ip(x.x.x.x) mask(0~32) type(0~2) sign(0~31)\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat ipv6 ip(x:x:x:x:x:x:x:x) mask(0~128) type(0~2) sign(0~31)\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_black_ip_for_hwnat del_sign sign\n");
		return -1;
	}
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set black ip for hwnat fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set black ip for hwnat sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_alg_rtsp_port(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_alg_rtsp_ports rtspport\n");
		return -1;
	}
	
	ret = blapi_traffic_set_alg_rtsp_port(argv[1]);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set alg rtsp ports fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set alg rtsp ports sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_default_wan_interface(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_DefaultWanInterface wanname\n");
		return -1;
	}
	
	ret = blapi_traffic_set_default_wan_interface(argv[1]);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set default wan interface fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set default wan interface sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_uninstall_mirror_traffic(int argc, char *argv[], void *p)
{
	int ret = 0;
	
	ret = blapi_traffic_uninstall_mirror_traffic();
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: uninstall mirror traffic fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: uninstall mirror traffic sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_wanInterface_qostype(int argc, char *argv[], void *p)
{
	int ret = 0;
	qos_wanIf_value_t wanIfValue;

	if(argc < 4)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_wanIF_qostype index wanname wanmask\n");
		return -1;
	}

	memset(&wanIfValue, 0, sizeof(qos_wanIf_value_t));
	wanIfValue.index = atoi(argv[1]);
	wanIfValue.wanType_mask = atoi(argv[3]);
	strncpy(wanIfValue.wanType_name, "wan_if", sizeof(wanIfValue.wanType_name) - 1);
	strncpy(wanIfValue.wanType_value, argv[2], sizeof(wanIfValue.wanType_value) - 1);
	
	ret = blapi_traffic_set_wanInterface_qostype(&wanIfValue);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wanInterface qostype fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wanInterface qostype sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_wanInterface_qos_template(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_WanIFQosTemplate wanIfIdx\n");
		return -1;
	}
	
	ret = blapi_traffic_set_wanInterface_qos_template(argv[1]);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wanInterface qos template fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wanInterface qos template sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

#if defined(TCSUPPORT_CT_DS_LIMIT)
int getQueueMaskByChar(char s[], char symbol)
{
	int i = 0;
	int count = 0;
	int qm = 0;

	if(s[0] == '\0'){
		return -1;		
	}

	for(i = 0; i < strlen(s); i++){
		if(symbol == s[i]){
			count++;
		}
	}

	for (i = 0; i < count+1; i++){
		qm += (1 << i);
	}
	
	return qm;
}

int do_set_up_dslimit_data(char* mode, char* dsvaue)
{
	int ret = 0;
	int g_queue_speed[DSLIMIT_MAX_QUEUE_NUM] = {0};
	int i_dsupmode = 0;
	int queueMask = 0;
	int count = 0;
	
	if(NULL == mode || NULL == dsvaue)
	{
		return -1;
	}

	queueMask = getQueueMaskByChar(dsvaue,',');
	i_dsupmode = atoi(mode);
	bzero(g_queue_speed, sizeof(g_queue_speed));
	ret = blapi_traffic_dslimit_set_up_data(i_dsupmode, dsvaue, g_queue_speed);
	if(ret == -1 || queueMask < 0)
	{
		ecnt_blapi_printf("\r\n[%s:%d]:set_up_dslimit data fail====>\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	if(i_dsupmode)
	{
#if defined(TCSUPPORT_CPU_EN7580)
		qoscmd_lib_do_en7580QoSDiscCAR(0, queueMask, g_queue_speed);
#else
		qoscmd_lib_do_en75xxQoSDiscCAR(0, queueMask, g_queue_speed, 1);
#endif
	}
	else
		{
		qoscmd_lib_do_en75xxQoSDiscPQ(0, 0);	
		}
	
	return 0;
	}

int do_set_down_dslimit_data(char* mode, char* dsvaue)
{
	int ret = 0;
	int i_dsdownmode = 0;
	
	if(NULL == mode || NULL == dsvaue)
	{
		return -1;
	}

	i_dsdownmode = atoi(mode);
	ret = blapi_traffic_dslimit_set_down_data(i_dsdownmode, dsvaue, 0);
	if(ret == -1)
		{
		ecnt_blapi_printf("\r\n[%s:%d]:set_down_dslimit data fail====>\n", __FUNCTION__, __LINE__);
			return -1;
		}
	
	return 0;
	}
	
int do_blapi_traffic_set_dslimit_data(int argc, char *argv[], void *p)
{
	int ret = 0;
	int g_queue_speed[DSLIMIT_MAX_QUEUE_NUM] = {0};

	if(argc != 4 || (strcmp(argv[1], "up") && strcmp(argv[1], "down")) || NULL == strstr(argv[3], "/"))
	{
		goto ErrorHandle;
	}

	if(strcmp(argv[1], "up") == 0)
	{
		do_set_up_dslimit_data(argv[2], argv[3]);
		
	}
	else if(strcmp(argv[1], "down") == 0)
	{
		do_set_down_dslimit_data(argv[2], argv[3]);
	}

	return 0;
	
ErrorHandle:
	ecnt_blapi_printf("Invalid parameters! \r\n");
	ecnt_blapi_printf("Usage: blapi_cmd traffic set_dslimit_data {up/down} {mode} {rule} \r\n");
	ecnt_blapi_printf("For example:(unit:512KB) \r\n"); 
	ecnt_blapi_printf("<1> Close dataspeedlimit: mode=0, rule=0/0\r\n");
	ecnt_blapi_printf("<2> Base Lan Interface: mode=1, rule=LAN1/20,LAN2/40,LAN3/60,LAN4/80 \r\n");
	ecnt_blapi_printf("<3> Base user vlan: mode=2, rule=111/20,222/40,333/60,444/80 \r\n");
	ecnt_blapi_printf("<4> Base user IP: mode=3, rule=192.168.1.11-192.168.1.11/20,192.168.1.22-192.168.1.22/40,192.168.1.33-192.168.1.33/60 \r\n");

	return -1;
}

#endif

int do_blapi_traffic_set_lan_info(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_lan_info lanip lanmask\n");
		return -1;
	}
	
	ret = blapi_traffic_set_lan_info(argv[1], argv[2]);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set lan info fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set lan info sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_check_eth_link_status(int argc, char *argv[], void *p)
{
	int ret = 0;
	int flag = 0;
	
	ret = blapi_traffic_check_eth_link_status(&flag);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: check eth link status fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: eth link status = %d\n", __FUNCTION__, __LINE__, flag);

	return 0;
}

int do_blapi_traffic_check_eth_port_status(int argc, char *argv[], void *p)
{
	int ret = 0;
	char buf[160] = {0};
	int val[16] = {0};
	
	ret = blapi_traffic_check_eth_port_status(buf, val);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: check eth port status fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: eth port status = %s\n", __FUNCTION__, __LINE__, buf);

	return 0;
}

int do_blapi_traffic_get_portrate_info(int argc, char *argv[], void *p)
{
	int ret = 0;
	char buf[512] = {0};
	
	ret = blapi_traffic_get_portrate_info(buf, 512);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get port rate info fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: port rate info= %s\n", __FUNCTION__, __LINE__, buf);

	return 0;
}

int do_blapi_traffic_get_lanport_info(int argc, char *argv[], void *p)
{
	int ret = 0;
	int i = 0;
	lanport_info_t lanport_info;

	ret = blapi_traffic_get_lanport_info(&lanport_info);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get lanport info fail\n", __FUNCTION__, __LINE__);
	else
	{
		for(i = 0; i < 8; i++)
		{
			ecnt_blapi_printf("[%s]%d: lan_port = %s, lan_status = %s, link_rate = %s\n",
				__FUNCTION__, __LINE__, lanport_info.lan_port[i], lanport_info.lan_status[i], lanport_info.link_rate[i]);
		}
	}

	return 0;
}

int do_blapi_traffic_get_wan_tx_stats(int argc, char *argv[], void *p)
{
	int ret = 0;
	ECNT_TRAFFIC_WAN_TX_STATISTICS tx_stats;

	memset(&tx_stats, 0x0, sizeof(tx_stats));
	ret = blapi_traffic_get_wan_tx_statistics(&tx_stats);

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s]%d: get wan tx stats fail\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ecnt_blapi_printf(
		"tx_stats.frame_cnt = %llu\n"
		"tx_stats.frame_byte = %llu\n"
		"tx_stats.drop_cnt = %llu\n"
		"tx_stats.broadcast = %llu\n"
		"tx_stats.multicast = %llu\n"
		"tx_stats.less_64 = %llu\n"
		"tx_stats.more_1518 = %llu\n",
		tx_stats.frame_cnt,
		tx_stats.frame_len,
		tx_stats.drop_cnt,
		tx_stats.broadcast,
		tx_stats.multicast,
		tx_stats.less_64,
		tx_stats.more_1518);
	ecnt_blapi_printf(
		"tx_stats.eq_64 = %llu\n"
		"tx_stats.from_65_to_127 = %llu\n"
		"tx_stats.from_128_to_255 = %llu\n"
		"tx_stats.from_256_to_511 = %llu\n"
		"tx_stats.from_512_to_1023 = %llu\n"
		"tx_stats.from_1024_to_1518 = %llu\n",
		tx_stats.eq_64,
		tx_stats.from_65_to_127,
		tx_stats.from_128_to_255,
		tx_stats.from_256_to_511,
		tx_stats.from_512_to_1023,
		tx_stats.from_1024_to_1518);

	return 0;
}

int do_blapi_traffic_get_wan_rx_stats(int argc, char *argv[], void *p)
{
	int ret = 0;
	ECNT_TRAFFIC_WAN_RX_STATISTICS rx_stats;

	memset(&rx_stats, 0x0, sizeof(rx_stats));
	ret = blapi_traffic_get_wan_rx_statistics(&rx_stats);

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s]%d: get wan rx stats fail\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ecnt_blapi_printf(
		"rx_stats.oversize = %llu\n"
		"rx_stats.undersize = %llu\n"
		"rx_stats.frame_cnt = %llu\n"
		"rx_stats.frame_len = %llu\n"
		"rx_stats.drop_cnt = %llu\n"
		"rx_stats.broadcast = %llu\n"
		"rx_stats.multicast = %llu\n",
		rx_stats.oversize,
		rx_stats.undersize,
		rx_stats.frame_cnt,
		rx_stats.frame_len,
		rx_stats.drop_cnt,
		rx_stats.broadcast,
		rx_stats.multicast);
	ecnt_blapi_printf(
		"rx_stats.crc = %llu\n"
		"rx_stats.fragment = %llu\n"
		"rx_stats.jabber = %llu\n"
		"rx_stats.less_64 = %llu\n"
		"rx_stats.more_1518 = %llu\n",
		rx_stats.crc,
		rx_stats.fragment,
		rx_stats.jabber,
		rx_stats.less_64,
		rx_stats.more_1518);
	ecnt_blapi_printf(
		"rx_stats.eq_64 = %llu\n"
		"rx_stats.from_65_to_127 = %llu\n"
		"rx_stats.from_128_to_255 = %llu\n"
		"rx_stats.from_256_to_511 = %llu\n"
		"rx_stats.from_512_to_1023 = %llu\n"
		"rx_stats.from_1024_to_1518 = %llu\n",
		rx_stats.eq_64,
		rx_stats.from_65_to_127,
		rx_stats.from_128_to_255,
		rx_stats.from_256_to_511,
		rx_stats.from_512_to_1023,
		rx_stats.from_1024_to_1518);

	return 0;
}

int do_blapi_traffic_set_vip(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int sport,dport,dip;
	unsigned char protocol;

	if((5 == argc)&&(strcmp(argv[1], "port") == 0))
	{
		sport = atoi(argv[2]);
		dport = atoi(argv[3]);
		protocol = atoi(argv[4]);

		ret = blapi_traffic_set_vip_by_port(sport,dport,protocol);

	}
	else if((4 == argc)&&(strcmp(argv[1], "dip_dport") == 0))
	{
		sscanf(argv[2],"%x",&dip);
		dport = atoi(argv[3]);

		ret = blapi_traffic_set_vip_by_dip_and_dport(dip,dport);
	}
	else
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_vip port [sport] [dport] [protocol] (0:TCP 1:UDP 2:ALL)\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_vip dip_dport [dip] [dport] (dip in hex)\n");
		return -1;
	}
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set vip fail\n", __FUNCTION__, __LINE__); 
	else
		ecnt_blapi_printf("\r\n[%s]%d: set vip sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_del_vip(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int sport,dport,dip;
	unsigned char protocol;

	if((5 == argc)&&(strcmp(argv[1], "port") == 0))
	{
		sport = atoi(argv[2]);
		dport = atoi(argv[3]);
		protocol = atoi(argv[4]);

		ret = blapi_traffic_del_vip_by_port(sport,dport,protocol);

	}
	else if((4 == argc)&&(strcmp(argv[1], "dip_dport") == 0))
	{
		sscanf(argv[2],"%x",&dip);
		dport = atoi(argv[3]);

		ret = blapi_traffic_del_vip_by_dip_and_dport(dip,dport);
	}
	else
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic del_vip port [sport] [dport] [protocol] (0:TCP 1:UDP 2:ALL)\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic del_vip dip_dport [dip] [dport] (dip in hex)\n");
		return -1;
	}
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: del vip fail\n", __FUNCTION__, __LINE__); 
	else
		ecnt_blapi_printf("\r\n[%s]%d: del vip sucess\n", __FUNCTION__, __LINE__);

	return 0;
}
	
int do_blapi_traffic_get_pppoe_info(int argc, char *argv[], void *p)
{
	int ret = 0;
	ECNT_TRAFFIC_PPPOE_INFO_DATA pppoe_info = {0};

	ret = blapi_traffic_get_pppoe_info_val(&pppoe_info);

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get pppoe info fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("valid: %d \n"
			   "pppoe SessionId: %d \n"
			   "wan_name: %s \n"
			   "smac: %02X:%02X:%02X:%02X:%02X:%02X \n"
			   "dmac: %02X:%02X:%02X:%02X:%02X:%02X \n"
			   "scr_ip: %d.%d.%d.%d \n"
			   "dst_ip: %d.%d.%d.%d \n"
			   "dns_valid: %d \n"
			   "pri_dns: %d.%d.%d.%d \n"
			   "snd_dns: %d.%d.%d.%d \n",
			   pppoe_info.valid,
			   pppoe_info.sessionID,
			   pppoe_info.ifname,
			   pppoe_info.smac_add[0],pppoe_info.smac_add[1],pppoe_info.smac_add[2],
			   pppoe_info.smac_add[3],pppoe_info.smac_add[4],pppoe_info.smac_add[5],
			   pppoe_info.dmac_add[0],pppoe_info.dmac_add[1],pppoe_info.dmac_add[2],
			   pppoe_info.dmac_add[3],pppoe_info.dmac_add[4],pppoe_info.dmac_add[5],
			   ((pppoe_info.src_ip) &(0xFF000000))>>24,((pppoe_info.src_ip)&(0x00FF0000))>>16, 
				((pppoe_info.src_ip)&(0x0000FF00))>>8, ((pppoe_info.src_ip)&(0x000000FF)), 
				((pppoe_info.dst_ip) &(0xFF000000))>>24,((pppoe_info.dst_ip)&(0x00FF0000))>>16, 
				((pppoe_info.dst_ip)&(0x0000FF00))>>8, ((pppoe_info.dst_ip)&(0x000000FF)),
				pppoe_info.dns_valid,
				(pppoe_info.pri_dns & 0xFF000000)>>24, (pppoe_info.pri_dns & 0xFF0000)>>16,
				(pppoe_info.pri_dns & 0xFF00)>>8, (pppoe_info.pri_dns & 0xFF),
				(pppoe_info.snd_dns & 0xFF000000)>>24, (pppoe_info.snd_dns & 0xFF0000)>>16,
				(pppoe_info.snd_dns & 0xFF00)>>8, (pppoe_info.snd_dns & 0xFF)
				);

	return 0;
}

int do_blapi_traffic_get_port_rx_byte_cnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long long portRxByteCnt = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_port_rx_byte_cnt portid\n");
		return -1;
	}
	
	ret = blapi_traffic_get_port_rx_cnt(atoi(argv[1]),&portRxByteCnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get port rx byte cnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: port%d_rx_byte_cnt = %llu\n", __FUNCTION__, __LINE__,atoi(argv[1]),portRxByteCnt);

	return 0;
}

int do_blapi_traffic_get_port_tx_byte_cnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long long portTxByteCnt = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_port_tx_byte_cnt portid\n");
		return -1;
	}
	
	ret = blapi_traffic_get_port_tx_cnt(atoi(argv[1]),&portTxByteCnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get port tx byte cnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: port%d_tx_byte_cnt = %llu\n", __FUNCTION__, __LINE__,atoi(argv[1]),portTxByteCnt);

	return 0;

}

int do_blapi_traffic_get_ether_wan_state(int argc, char *argv[], void *p)
{
	int ret = 0;
	int etherUp = 0;

	ret = blapi_traffic_get_ether_wan_state(&etherUp);

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get ether wan state fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: ether wan state is %s\n", __FUNCTION__, __LINE__,etherUp?"up":"down");

	return 0;

}

int do_blapi_traffic_get_active_ether_wan_state(int argc, char *argv[], void *p)
{
	int ret = 0;
	int etherUp = 0;

	ret = blapi_traffic_get_active_ether_wan_state(&etherUp);

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get active ether wan state fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: active ether wan state is %s\n", __FUNCTION__, __LINE__,etherUp?"up":"down");

	return 0;

}

int do_blapi_traffic_get_hsgmii_lan_state(int argc, char *argv[], void *p)
{
	int ret = 0;
	int etherUp = 0;

	
#if defined(TCSUPPORT_CPU_EN7581)
	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_hsgmii_lan_tx_stats (0-3,pcie0/pcie1/usb/eth as hsgmii lan)\n");
		return -1;
	}
	ret = blapi_traffic_get_multi_hsgmii_lan_state(&etherUp,atoi(argv[1]));
#else
	ret = blapi_traffic_get_hsgmii_lan_state(&etherUp);		
#endif

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get hsgmii lan state fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: hsgmii lan state is %s\n", __FUNCTION__, __LINE__,etherUp?"up":"down");

	return 0;

}



int do_blapi_traffic_get_ratelimit_mode(int argc, char *argv[], void *p)
{
	int ret = 0;
	int meter_mode = -1;

	if(argc < 2)
	{
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_ratelimit_mode dir meter_group\n");
#else
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_ratelimit_mode dir\n");
#endif
		return -1;
	}
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	ret = blapi_traffic_get_meter_ratelimit_mode(atoi(argv[1]), atoi(argv[2]), &meter_mode);
#else
	ret = blapi_traffic_get_ratelimit_mode(atoi(argv[1]), &meter_mode);
#endif
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get ratelimit mode fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: ratelimit mode is %d\n", __FUNCTION__, __LINE__,meter_mode);

	return 0;

}


int do_blapi_traffic_nptv6(int argc, char *argv[], void *p)
{
	int ret = 0;

	if((5 == argc)&&(!strcmp(argv[1], "add")))
	{
		ret = blapi_traffic_add_nptv6_prefix(argv[2],argv[3],argv[4]);
	}
	else if((3 == argc)&&(!strcmp(argv[1], "del")))
	{
		if(!strcmp(argv[2], "all"))
			ret = blapi_traffic_del_nptv6_prefix_all();
		else
			ret = blapi_traffic_del_nptv6_prefix(argv[2]);
	}
	else
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic nptv6 add [if_name] [snpt] [dnpt] \n");		
		ecnt_blapi_printf("\r\nEX:blapi_cmd traffic nptv6 add nas0_0 2000::/64 2001::/64\n");
		ecnt_blapi_printf("\r\nblapi_cmd traffic nptv6 del [if_name]/all\n");
		return -1;
	}
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set nptv6 fail\n", __FUNCTION__, __LINE__); 
	else
		ecnt_blapi_printf("\r\n[%s]%d: set nptv6 sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_set_wan_mtu(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_wan_mtu_7523 wan_idx wan_mtu\n");
		return -1;
	}

	ret = blapi_traffic_set_wan_mtu(atoi(argv[1]), atoi(argv[2]));

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set wan mtu fail\n", __FUNCTION__, __LINE__); 
	else
		ecnt_blapi_printf("\r\n[%s]%d: set wan mtu sucess\n", __FUNCTION__, __LINE__);

	return 0;

}
int do_blapi_traffic_get_lan_pm_stats(int argc, char *argv[], void *p)
{

	unsigned int cnt = 0;
	PPTPEthernetUNI4Pmcounters_t data;
	int validPortId =0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\ndo_blapi_traffic_get_lan_traffic_stats lanport<0,1,2,3> fail\n");
		return -1;
	}
	validPortId =  atoi(argv[1]);

	if(blapi_traffic_get_lan_tx_traffic(validPortId,&cnt) == 0)//The validPortId should be 0,1,2,3
	{
		data.transmitted_traffic = cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_rx_traffic(validPortId,&cnt) == 0)
	{
		data.received_traffic = cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_tx_rate(validPortId,&cnt) == 0)
	{
		data.transmitted_rate= cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_rx_rate(validPortId,&cnt) == 0)
	{
		data.received_rate= cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_tx_octets(validPortId,&cnt) == 0)
	{
		data.transmitted_Octets= cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_rx_octets(validPortId,&cnt) == 0)
	{
		data.received_Octets= cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_rx_discard_counter(validPortId,&cnt) == 0)
	{
		data.received_dis_cout= cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_tx_discard_counter(validPortId,&cnt) == 0)
	{
		data.transmitted_dis_cout= cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_rx_error_counter(validPortId,&cnt) == 0)
	{
		data.received_err_cout= cnt;
	}else
		return -1;
	if(blapi_traffic_get_lan_tx_error_counter(validPortId,&cnt) == 0)
	{
		data.transmitted_err_cout= cnt;
	}else
		return -1;
	
	ecnt_blapi_printf(
		"lan port :%d\n"
		"transmitted_traffic = %d\n"
		"received_traffic = %d\n"
		"transmitted_rate = %d\n"
		"received_rate = %d\n"
		"transmitted_Octets = %d\n"
		"received_Octets = %d\n",
		validPortId,
		data.transmitted_traffic,
		data.received_traffic,
		data.transmitted_rate,
		data.received_rate,
		data.transmitted_Octets,
		data.received_Octets);
	ecnt_blapi_printf(
		"transmitted_dis_cout = %d\n"
		"received_dis_cout = %d\n"
		"transmitted_err_cout = %d\n"
		"received_err_cout = %d\n",
		data.transmitted_dis_cout,
		data.received_dis_cout,
		data.transmitted_err_cout,
		data.received_err_cout);
	return 0;
}

int do_blapi_traffic_get_wan_pm_stats(int argc, char *argv[], void *p)
{
	unsigned int cnt = 0;
	PPTPEthernetUNI4Pmcounters_t data;

	if(blapi_traffic_get_wan_tx_traffic(&cnt) == 0)
	{
		data.transmitted_traffic = cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_rx_traffic(&cnt) == 0)
	{
		data.received_traffic = cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_tx_rate(&cnt) == 0)
	{
		data.transmitted_rate= cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_rx_rate(&cnt) == 0)
	{
		data.received_rate= cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_tx_octets(&cnt) == 0)
	{
		data.transmitted_Octets= cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_rx_octets(&cnt) == 0)
	{
		data.received_Octets= cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_rx_discard_counter(&cnt) == 0)
	{
		data.received_dis_cout= cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_tx_discard_counter(&cnt) == 0)
	{
		data.transmitted_dis_cout= cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_rx_error_counter(&cnt) == 0)
	{
		data.received_err_cout= cnt;
	}else
		return -1;
	if(blapi_traffic_get_wan_tx_error_counter(&cnt) == 0)
	{
		data.transmitted_err_cout= cnt;
	}else
		return -1;
	
	ecnt_blapi_printf(
		"wan.transmitted_traffic = %d\n"
		"wan.received_traffic = %d\n"
		"wan.transmitted_rate = %d\n"
		"wan.received_rate = %d\n"
		"wan.transmitted_Octets = %d\n"
		"wan.received_Octets = %d\n",
		data.transmitted_traffic,
		data.received_traffic,
		data.transmitted_rate,
		data.received_rate,
		data.transmitted_Octets,
		data.received_Octets);
	ecnt_blapi_printf(
		"wan.transmitted_dis_cout = %d\n"
		"wan.received_dis_cout = %d\n"
		"wan.transmitted_err_cout = %d\n"
		"wan.received_err_cout = %d\n",
		data.transmitted_dis_cout,
		data.received_dis_cout,
		data.transmitted_err_cout,
		data.received_err_cout);
	return 0;
}



int do_blapi_traffic_set_sla_cfg(int argc, char *argv[], void *p){

	int ret = 0;

	if(argc < 4)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic do_blapi_traffic_set_sla_cfg dir(0/1) slaindex(0-3) enable(0/1) chnl_index(0-31) \n");
		return -1;
	}
	
	ret = blapi_traffic_set_sla_cfg(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]), atoi(argv[4]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: do_blapi_traffic_set_sla_cfg fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: do_blapi_traffic_set_sla_cfg success\n", __FUNCTION__, __LINE__);

	return 0;




}


int do_blapi_traffic_set_assure_max_bandwidth(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 5)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic set_assure_max_bandwidth dir(0/1) slaindex(0-3) queue_index(0-7) csb_value pbs_value\n");
		return -1;
	}
	
	ret = blapi_traffic_set_assure_max_bandwidth(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: do_blapi_traffic_set_assure_max_bandwidth fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: do_blapi_traffic_set_assure_max_bandwidth success\n", __FUNCTION__, __LINE__);

	return 0;
}


int do_blapi_traffic_get_hsgmii_lan_tx_stats(int argc, char *argv[], void *p)
{
	int ret = 0;
	ECNT_TRAFFIC_WAN_TX_STATISTICS tx_stats;

	memset(&tx_stats, 0x0, sizeof(tx_stats));
	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_hsgmii_lan_tx_stats (0-2,pcie0/pcie1/usb as hsgmii lan)\n");
		return -1;
	}

	
	ret = blapi_traffic_get_hsgmii_lan_tx_statistics(&tx_stats,atoi(argv[1]));

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s]%d: get wan tx stats fail\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ecnt_blapi_printf(
		"tx_stats.frame_cnt = %llu\n"
		"tx_stats.frame_byte = %llu\n"
		"tx_stats.drop_cnt = %llu\n"
		"tx_stats.broadcast = %llu\n"
		"tx_stats.multicast = %llu\n"
		"tx_stats.less_64 = %llu\n"
		"tx_stats.more_1518 = %llu\n",
		tx_stats.frame_cnt,
		tx_stats.frame_len,
		tx_stats.drop_cnt,
		tx_stats.broadcast,
		tx_stats.multicast,
		tx_stats.less_64,
		tx_stats.more_1518);
	ecnt_blapi_printf(
		"tx_stats.eq_64 = %llu\n"
		"tx_stats.from_65_to_127 = %llu\n"
		"tx_stats.from_128_to_255 = %llu\n"
		"tx_stats.from_256_to_511 = %llu\n"
		"tx_stats.from_512_to_1023 = %llu\n"
		"tx_stats.from_1024_to_1518 = %llu\n",
		tx_stats.eq_64,
		tx_stats.from_65_to_127,
		tx_stats.from_128_to_255,
		tx_stats.from_256_to_511,
		tx_stats.from_512_to_1023,
		tx_stats.from_1024_to_1518);

	return 0;
}

int do_blapi_traffic_get_hsgmii_lan_rx_stats(int argc, char *argv[], void *p)
{
	int ret = 0;
	ECNT_TRAFFIC_WAN_RX_STATISTICS rx_stats;
	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_hsgmii_lan_tx_stats (0-2,pcie0/pcie1/usb as hsgmii lan)\n");
		return -1;
	}
	memset(&rx_stats, 0x0, sizeof(rx_stats));
	ret = blapi_traffic_get_hsgmii_lan_rx_statistics(&rx_stats,atoi(argv[1]));

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s]%d: get wan rx stats fail\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ecnt_blapi_printf(
		"rx_stats.oversize = %llu\n"
		"rx_stats.undersize = %llu\n"
		"rx_stats.frame_cnt = %llu\n"
		"rx_stats.frame_len = %llu\n"
		"rx_stats.drop_cnt = %llu\n"
		"rx_stats.broadcast = %llu\n"
		"rx_stats.multicast = %llu\n",
		rx_stats.oversize,
		rx_stats.undersize,
		rx_stats.frame_cnt,
		rx_stats.frame_len,
		rx_stats.drop_cnt,
		rx_stats.broadcast,
		rx_stats.multicast);
	ecnt_blapi_printf(
		"rx_stats.crc = %llu\n"
		"rx_stats.fragment = %llu\n"
		"rx_stats.jabber = %llu\n"
		"rx_stats.less_64 = %llu\n"
		"rx_stats.more_1518 = %llu\n",
		rx_stats.crc,
		rx_stats.fragment,
		rx_stats.jabber,
		rx_stats.less_64,
		rx_stats.more_1518);
	ecnt_blapi_printf(
		"rx_stats.eq_64 = %llu\n"
		"rx_stats.from_65_to_127 = %llu\n"
		"rx_stats.from_128_to_255 = %llu\n"
		"rx_stats.from_256_to_511 = %llu\n"
		"rx_stats.from_512_to_1023 = %llu\n"
		"rx_stats.from_1024_to_1518 = %llu\n",
		rx_stats.eq_64,
		rx_stats.from_65_to_127,
		rx_stats.from_128_to_255,
		rx_stats.from_256_to_511,
		rx_stats.from_512_to_1023,
		rx_stats.from_1024_to_1518);

	return 0;
}

int do_blapi_traffic_mdio_read(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int phyaddr = 0, devaddr =0, reg = 0;
         unsigned int value = 0;
		 
	if(argc < 4)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic mdio read phyaddr devaddr reg\n");
		return -1;
	}
	
         phyaddr = atoi(argv[1]);
         sscanf(argv[2], "%lx", &devaddr);   	//hex
         sscanf(argv[3], "%lx", &reg);	       //hex

	ret = blapi_traffic_mdio_read(phyaddr, devaddr, reg, &value);

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s]%d: mdio read fail\n", __FUNCTION__, __LINE__);
		return -1;
	}
	else ecnt_blapi_printf("\r\n[%s]%d: *MIDO Read PhyAddr=%d DevAddr=%d Reg=0x%04lx value:0x%04lx\r\n", __FUNCTION__, __LINE__ ,phyaddr ,devaddr ,reg ,value);

	return 0;
}

int do_blapi_traffic_mdio_write(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int phyaddr = 0, devaddr =0, reg = 0;
         unsigned int value = 0;
		 
	if(argc < 5)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic mdio write phyaddr devaddr reg value\n");
		return -1;
	}
	
         phyaddr = atoi(argv[1]);
         sscanf(argv[2], "%lx", &devaddr);   	//hex
         sscanf(argv[3], "%lx", &reg);	        //hex
	sscanf(argv[4], "%lx", &value);	        //hex
	 
	ret = blapi_traffic_mdio_write(phyaddr, devaddr, reg, value);

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s]%d: mdio write fail\n", __FUNCTION__, __LINE__);
		return -1;
	}
	else ecnt_blapi_printf("\r\n[%s]%d: *MIDO Write PhyAddr=%d DevAddr=%d Reg=0x%04lx value:0x%04lx\r\n", __FUNCTION__, __LINE__ ,phyaddr ,devaddr ,reg ,value);

	return 0;

}

int do_blapi_traffic_set_wan_acnt_clear(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n[%s]%d:blapi_cmd traffic set_wan_acnt 0~31\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	ret = blapi_traffic_set_wan_acnt_clear(atoi(argv[1]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: clear wan acnt fail\n", __FUNCTION__, __LINE__);
	else		
		ecnt_blapi_printf("\r\n[%s]%d: clear wan acnt ok\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_add_vlan_filter_vlan(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n[%s]%d:blapi_cmd traffic add_vlan_filter vlan \n", __FUNCTION__, __LINE__);
		return -1;
	}

	ret = blapi_traffic_add_vlan_filter_vlan(atoi(argv[1]),atoi(argv[2]));
	
	if(ret != 0)
		ecnt_blapi_printf("\r\n[%s]%d: add vlan fail\n", __FUNCTION__, __LINE__);
	else		
		ecnt_blapi_printf("\r\n[%s]%d: add vlan ok\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_del_vlan_filter_vlan(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n[%s]%d:blapi_cmd traffic del_vlan_filter vlan \n", __FUNCTION__, __LINE__);
		return -1;
	}

	ret = blapi_traffic_del_vlan_filter_vlan(atoi(argv[1]),atoi(argv[2]));
	
	if(ret != 0)
		ecnt_blapi_printf("\r\n[%s]%d: del vlan fail\n", __FUNCTION__, __LINE__);
	else		
		ecnt_blapi_printf("\r\n[%s]%d: del vlan ok\n", __FUNCTION__, __LINE__);

	return 0;
}


int do_blapi_traffic_get_port_rx_cnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int portRxCnt = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_port_rx_cnt portid\n");
		return -1;
	}
	
	ret = blapi_traffic_get_port_rx_pkts(atoi(argv[1]),&portRxCnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get port rx cnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: port%d_rx_cnt = %u\n", __FUNCTION__, __LINE__,atoi(argv[1]),portRxCnt);

	return 0;

}

int do_blapi_traffic_get_port_tx_cnt(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int portTxCnt = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_port_tx_cnt portid\n");
		return -1;
	}
	
	ret = blapi_traffic_get_port_tx_pkts(atoi(argv[1]),&portTxCnt);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: get port tx cnt fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: port%d_tx_cnt = %u\n", __FUNCTION__, __LINE__,atoi(argv[1]),portTxCnt);

	return 0;

}

int do_blapi_traffic_set_hsgmii_port_ratelimit(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\n[%s]%d:blapi_cmd traffic set_hsgmii_port_ratelimit dir[0:down, 1:up] rate[kbps] \n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	ret = blapi_traffic_set_hsgmii_port_ratelimit(atoi(argv[1]), atoi(argv[2]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set hsgmii port ratelimit fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set hsgmii port ratelimit sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_get_wan_port_statistics(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	int pord_id;
	ECNT_TRAFFIC_GMAC_ETH_STA port_stats;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_wan_port_state <pord_id:1/2>\n");
		return -1;
	}

	memset(&port_stats, 0x0, sizeof(port_stats));
	pord_id=atoi(argv[1]);
	ecnt_blapi_printf("blapi_cmd get port[%d] statistics\r\n", pord_id);
	if(pord_id != 1 && pord_id != 2){
		ecnt_blapi_printf("\r\nblapi_cmd traffic pord_id muse be 1:wan or 2:lan\n");
		return -1;
	}

	ret = blapi_traffic_get_wan_port_statistics(&port_stats, pord_id);
	
	if(ret < 0) {
		ecnt_blapi_printf("\r\n[%s]%d: get wan port stats fail\n", __FUNCTION__, __LINE__);
		ret = -1;
	} else {
		ecnt_blapi_printf("\r\n[%s] [%d]: get wan port stats succeed:\n",__FUNCTION__, __LINE__);
		ecnt_blapi_printf("port[%d]\r\n", pord_id);
		ecnt_blapi_printf("inBytes      0x%016llx, inPkts       0x%016llx\n", port_stats.inBytes, port_stats.inPkts);
		ecnt_blapi_printf("inUnicast    0x%016llx, inMulticast  0x%016llx\n", port_stats.inUnicast, port_stats.inMulticast);
		ecnt_blapi_printf("inBroadcast  0x%016llx, inError      0x%016llx\n", port_stats.inBroadcast, port_stats.inError);
		ecnt_blapi_printf("inDiscard    0x%016llx\n", port_stats.inDiscard);
		ecnt_blapi_printf("outBytes     0x%016llx, outPkts      0x%016llx\n", port_stats.outBytes, port_stats.outPkts);
		ecnt_blapi_printf("outUnicast   0x%016llx, outMulticast 0x%016llx\n", port_stats.outUnicast, port_stats.outMulticast);
		ecnt_blapi_printf("outBroadcast 0x%016llx, outError     0x%016llx\n", port_stats.outBroadcast, port_stats.outError);
		ecnt_blapi_printf("outDiscard   0x%016llx\n", port_stats.outDiscard);
		ecnt_blapi_printf("\r\n");		
	}

	return ret;
#else
	return -1;
#endif
}

int do_blapi_traffic_get_wan_port_link_state(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	int pord_id;
	ECNT_TRAFFIC_GMAC_ETH_LINKST link_state;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_wan_port_state <port_id:1>\n");
		return -1;
	}

	memset(&link_state, 0x0, sizeof(link_state));
	pord_id=atoi(argv[1]);
	ecnt_blapi_printf("blapi_cmd get port[%d] link state\r\n", pord_id);
	if(pord_id != 1){
		ecnt_blapi_printf("\r\nblapi_cmd get pord_id must be 1\n");
		return -1;
	}

	ret = blapi_traffic_get_wan_port_link_state(&link_state, pord_id);
	
	if(ret < 0) {
		ecnt_blapi_printf("\r\n[%s]%d: get wan port link state fail\n", __FUNCTION__, __LINE__);
		ret = -1;
	} else {
		ecnt_blapi_printf("port[%d]\r\n", pord_id);
		ecnt_blapi_printf("link_status    %s\n", ((link_state.link_status) ? "up" : "down"));
		ecnt_blapi_printf("auto_ng_status %s\n", ((link_state.auto_ng_status) ? "force" : "auto"));
		ecnt_blapi_printf("duplex_status  %s\n", ((link_state.duplex_status) ? "full-duplex" : "half-duplex"));
		if(link_state.speed_status == 0)
			ecnt_blapi_printf("speed_status   10Mbps\n");
		else if(link_state.speed_status == 1)
			ecnt_blapi_printf("speed_status   100Mbps\n");
		else if(link_state.speed_status == 2)
			ecnt_blapi_printf("speed_status   1000Mbps\n");
		else if(link_state.speed_status == 3)
			ecnt_blapi_printf("speed_status   2.5Gbps\n");
		else
			ecnt_blapi_printf("error speed status %d\n", link_state.speed_status);	
	}

	return ret;
#else
	return -1;
#endif
}

int do_blapi_traffic_set_wan_port_link_mode(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	int pord_id;
	ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE mode;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd traffic get_wan_port_state <port_id:1> <mode:1/2/3/4/5/6>\n");
		return -1;
	}

	pord_id = atoi(argv[1]);
	mode = atoi(argv[2]);
	ecnt_blapi_printf("blapi_cmd set port[%d] link mode as %d\r\n", pord_id,mode);
	if(pord_id != 1){
		ecnt_blapi_printf("\r\nblapi_cmd set link mode pord_id muse be 1\n");
		return -1;
	}

	if(mode != 1 && mode != 2 && mode != 3 && mode != 4 && mode != 5 && mode != 6){
		ecnt_blapi_printf("\r\nblapi_cmd set link mode must be 1~6\n");
		return -1;
	}

	ret = blapi_traffic_set_wan_port_link_mode(mode, pord_id);
	
	if(ret < 0) {
		ecnt_blapi_printf("\r\n[%s]%d: set wan port link state fail\n", __FUNCTION__, __LINE__);
		ret = -1;
	} else {
		ecnt_blapi_printf("\r\n[%s] [%d]: set wan port stats succed\n",__FUNCTION__, __LINE__);
		ecnt_blapi_printf("\r\n");		
	}

	return ret;
#else
	return -1;
#endif
}

int do_blapi_traffic_set_dev_stat_ratelimit_mode(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned int wan_conn_type = 0;
	char wan_itf_name[16];

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\n[%s]%d:blapi_cmd traffic set_dev_stat_ratelim_mode [wan_conn_type] [wan_itf_name]\n", __FUNCTION__, __LINE__);
		ecnt_blapi_printf("\r\nUsage:blapi_cmd traffic set_dev_stat_ratelim_mode [1:wifi 2.4G, 2:wifi 5G, 3:eth wan] [apcli0, apclii0, eth0.4]\n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* wan_conn_type : 1-wifi 2.4G, 2-wifi 5G, 3-eth wan */
    wan_conn_type = atoi(argv[1]);
    if(wan_conn_type < 1 || wan_conn_type > 3){
    	ecnt_blapi_printf("\r\n[%s]%d: set dev stat and ratelimit mode failed, invalid param!\n", __FUNCTION__, __LINE__);
    	return -1;
    }

    memcpy(wan_itf_name, argv[2], 16);
	ret = fe_lib_set_dev_stat_ratelimit_mode(wan_conn_type, wan_itf_name);
		
	if(!ret)
		ecnt_blapi_printf("\r\n[%s]%d: set dev stat and ratelimit mode success.\n", __FUNCTION__, __LINE__);		
	else
		ecnt_blapi_printf("\r\n[%s]%d: set dev stat and ratelimit mode failed.\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_traffic_cmd(int argc,char* argv[],void *p)
{
	return subcmd(blapi_traffic_cmd,argc,argv,(void *)blapi_traffic_cmd);
}

