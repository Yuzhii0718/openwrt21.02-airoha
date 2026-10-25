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
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <vpn_info/info_ioctl.h>
#include "traffic_dslimit.h"
#include "blapi_extra.h"
#include "blapi_traffic_bsp.h"
#include "blapi_traffic.h"
#if defined(TCSUPPORT_IFC_EN)
#include "libapi_lib_ifc.h"
#endif

#include "libapi_lib_qdmamgr.h"
#include "libapi_lib_fe.h"
#include "libapi_lib_utility.h"
#include "libapi_lib_switchmgr.h"
#if defined(TCSUPPORT_MULTI_USER_ITF) 
#include <lan_port/lan_port_info.h>
#endif
#include "traffic_process_global_def.h"
#include <linux/qos_type.h>
#include <ecnt_lib_dbg.h>

#define ETHER_WAN_PATH	"/proc/tc3162/eth_portmap"

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
#define ETHLINK_PATH	"/proc/tc3162/eth_link_st"
#define ETH_PORT_STATUS_FILE_PATH "/proc/tc3162/eth_port_status"
#define GSW_LINK_ST_PATH "/proc/tc3162/gsw_link_st"
#define ETH_PORTMAP_PATH "/proc/tc3162/eth_portmap"
#if defined(TCSUPPORT_CT)
#define LAN_IP_MASK_PATH "/proc/tc3162/lan_ip_mask"
#endif

#define ETHER_WAN_STATE_PATH        "/proc/tc3162/eth1_link_st"
#define ACTIVE_ETHERNET_WAN_STATE_PATH        "/proc/tc3162/ae_wan_link_status"
#define HSGMII_LAN_STATE_PATH        		  "/proc/tc3162/hsgmii_lan_link_status"

#define HSGMII_PCIE0_LAN_STATE_PATH        		  "/proc/tc3162/hsgmii_pcie0_lan_link_status"
#define HSGMII_PCIE1_LAN_STATE_PATH        		  "/proc/tc3162/hsgmii_pcie1_lan_link_status"
#define HSGMII_USB_LAN_STATE_PATH        		  "/proc/tc3162/hsgmii_usb_lan_link_status"
#define HSGMII_ETH_LAN_STATE_PATH        		  "/proc/tc3162/hsgmii_eth_lan_link_status"





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

#ifdef TCSUPPORT_3_18_21_KERNEL
#define PACKET_SKB_FOE_INFO 	21
#else
#define PACKET_SKB_FOE_INFO	23
#endif

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
#define SWITCHPARA_CMD "/userfs/bin/ethphxcmd eth0 media-type %s port %d"
#define SWITCHPARA_CMD_ENABLE "/userfs/bin/ethphxcmd eth0 lanchip %s port %d"
#define SWITCHPARA_AUTO_CMD "/userfs/bin/switchmgr phy autodetect %d %s"

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

int blapi_traffic_add_black_ip_port_protocol(unsigned int ip, unsigned int port, unsigned int protocol)
{
	return ppemgr_lib_black_ip_port_protocol_add(ip, port, protocol);
}

int blapi_traffic_delete_black_ip_port_protocol(unsigned int ip, unsigned int port, unsigned int protocol)
{
	return ppemgr_lib_black_ip_port_protocol_delete(ip, port, protocol);
}

int blapi_traffic_delete_all_black_ip_port_protocol(void)
{
	return ppemgr_lib_black_ip_port_protocol_delete_all();
}

int blapi_traffic_show_black_ip_port_protocol(void)
{
	return ppemgr_lib_black_ip_port_protocol_show();
}

int blapi_traffic_get_pppoe_info_val(ECNT_TRAFFIC_PPPOE_INFO_DATA *pppoe_info_val)
{
	ECNT_PPPOE_INFO_DATA data;

	memset(&data,0,sizeof(ECNT_PPPOE_INFO_DATA));
	if(ECNT_FEMGR_SUCCESS != fe_lib_get_pppoe_info(&data))
		return -1;

	memmove(pppoe_info_val->ifname,data.ifname,16);
	memmove(pppoe_info_val->dmac_add,data.dmac_add,6);
	memmove(pppoe_info_val->smac_add,data.smac_add,6);
	pppoe_info_val->valid = data.valid;
	pppoe_info_val->sessionID = data.sessionID;
	pppoe_info_val->src_ip = data.src_ip;
	pppoe_info_val->dst_ip = data.dst_ip;
	pppoe_info_val->dns_valid = data.dns_valid;
	pppoe_info_val->pri_dns = data.pri_dns;
	pppoe_info_val->snd_dns = data.snd_dns;

	return 0;
}

int blapi_traffic_set_wan_ratelimit(char *wanname, unsigned int wanid, unsigned int up_rate, unsigned int dn_rate)
{
	int ret = -1;
	
#if defined(TCSUPPORT_CPU_EN7580)
	ret = fe_lib_set_wan_ratelimit(1, wanname, up_rate, up_rate?1:0);
	ret = fe_lib_set_wan_ratelimit(0, wanname, dn_rate, dn_rate?1:0);
#else
	ret = ppemgr_lib_set_wan_ratelimit(wanid, 0, up_rate);
	ret = ppemgr_lib_set_wan_ratelimit(wanid, 1, dn_rate);
#endif

	return ret;
}

int doValPut(char *path, char *value)
{
	FILE *fp = NULL;

	fp = fopen(path,"w");
	if(fp != NULL)
	{
		fprintf(fp, "%s", value);
		fclose(fp);
	}
	else
	{
		return -1;
	}
	
	return 0;
}

int getACLRate(int rate)
{
	int aclRate = 0x3FFF;
	int r1 = 0;
	int r2 = 0;

	r1 = rate/64;
	r2 = rate%32; 
	if(rate != 0){							
		if(r1 == 0){
			aclRate = 1;			/*ACL rate is n*64kbps */
		}else{
			if(r2 < 32){
				aclRate = r1;			/*ACL rate is n*64kbps */
			}
		}	
	}

	return aclRate;
}

int blapi_traffic_get_multicast_acnt(ECNT_TRAFFIC_MCST_ACNT_MODE mode, unsigned long long *cnt)
{
	FILE *fp = NULL;	
	char *line = NULL;
	int size = 0, ret = 0, j = 0;
	unsigned long long packet = 0, mulbytes = 0;

	LIB_BLAPI_DEBUG("enter,type=%d=====>\n",mode);
	/* get mulrxnytes */
	fp = fopen("/proc/tc3162/hwnat_multicast_account","r");
	if (fp != NULL){
		while ((ret = getline(&line, &size, fp)) != -1){	
			if( 1 == j){	
				sscanf(line, "%llu%llu", &packet, &mulbytes);
			}
			j++;
		}
		fclose(fp);
		if (line){
			free(line);
			line = NULL;
		}	
	}
	else
	{
		LIB_BLAPI_ERROR("leave,fp = NULL, return -1=====>\n");
		return -1;
	}

	if ( mode == MCST_ACNT_MODE_BYTES ){
		*cnt = mulbytes;
	}
	else if ( mode == MCST_ACNT_MODE_PACKET ){
		*cnt = packet;
	}
			
	LIB_BLAPI_DEBUG("leave, cnt=%lld, return 0=====>\n", *cnt);
	return 0;	
}


int blapi_traffic_get_flowid_cnt(uint dir, uint flowid, unsigned long long *cnt, int* flag)
{  
	LIB_BLAPI_DEBUG("enter,dir=%u,flowid=%u=====>\n", dir, flowid);
	
	if ( 0 != fe_lib_get_flow_byte_cnt(dir, flowid, cnt))
	{
		LIB_BLAPI_ERROR("get byte faild=====>\n");
	}

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
	*flag = 0;
#else
	*flag = 1;
#endif
				
	LIB_BLAPI_DEBUG("leave, return 0, cnt=%llu, flag=%d=====>\n", *cnt, *flag);
	return 0;
}

int blapi_traffic_get_llid_cnt(uint dir, uint llid, unsigned long long *cnt)
{  
	LIB_BLAPI_DEBUG("enter,dir=%u,llid=%u=====>\n", dir, llid);
	
	if ( 0 != fe_lib_get_flow_byte_cnt(dir, llid, cnt))
	{
		LIB_BLAPI_ERROR("get byte faild=====>\n");
	}
				
	LIB_BLAPI_DEBUG("leave, return 0, cnt=%llu\n", *cnt);
	return 0;
}

int blapi_traffic_get_flowid_byte_pkt_cnt(uint dir, uint flowid, unsigned long long *byte_cnt, unsigned long long *pkt_cnt, int* flag)
{
	LIB_BLAPI_DEBUG("enter,dir=%u,flowid=%u=====>\n", dir, flowid);
	
	if ( 0 != fe_lib_get_flow_byte_pkt_cnt(dir, flowid, byte_cnt, pkt_cnt))
	{
		LIB_BLAPI_ERROR("get cnt failed=====>\n");
	}

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
	*flag = 0;
#else
	*flag = 1;
#endif
				
	LIB_BLAPI_DEBUG("leave, return 0, byte_cnt=%llu, pkt_cnt=%llu, flag=%d=====>\n", *byte_cnt, *pkt_cnt, *flag);
	
	return 0;
}

int blapi_traffic_set_wan_rate_threshold(unsigned int threshold)
{
	int ret = -1;
	
	ret = ppemgr_lib_set_wan_rate_threshold(threshold);
	
	return ret;
}

int blapi_traffic_set_wan_mtu_val(unsigned char wan_type, unsigned int wan_mtu)
{
	int ret = -1;
	
	ret = ppemgr_lib_set_wan_mtu(wan_type, wan_mtu);

	return ret;
}


int blapi_traffic_set_dev_acnt_enable(unsigned char sw_acnt_en, unsigned char hw_acnt_en)
{
	LIB_BLAPI_DEBUG("enter, sw_en=%d, hw_en=%d=====>\n", sw_acnt_en, hw_acnt_en);

	printf("sw_acnt_en:%x hw_acnt_en:%x\n",sw_acnt_en,hw_acnt_en);

#if (!defined TCSUPPORT_CPU_EN7580) || (defined TCSUPPORT_WLAN_MT7615_TXOFFLOAD)
	if(sw_acnt_en)
		doValPut("/proc/tc3162/devBandwidth", "1");
	else
		doValPut("/proc/tc3162/devBandwidth", "0");
#endif
	if(hw_acnt_en)
	{
		doValPut("/proc/tc3162/hwnat_bandwidth", "1 1");
#if !defined(TCSUPPORT_ACCOUNT_METER_V2)		
		#if defined(TCSUPPORT_CPU_EN7580)|| defined(TCSUPPORT_CPU_EN7527)
		fe_lib_set_acnt0_mode(DOWN_DIR, 1);	
		fe_lib_set_acnt0_mode(UP_DIR, 1);
		#endif
#endif
	}
	else
	{
		doValPut("/proc/tc3162/hwnat_bandwidth", "0 0");
#if !defined(TCSUPPORT_ACCOUNT_METER_V2)		
		#if defined(TCSUPPORT_CPU_EN7580)|| defined(TCSUPPORT_CPU_EN7527)
		fe_lib_set_acnt0_mode(DOWN_DIR, 6);	
		fe_lib_set_acnt0_mode(UP_DIR, 6);
		#endif
#endif
	}

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}

int blapi_traffic_set_wifi_dev_ratelimit_enable(int enable)
{
	LIB_BLAPI_DEBUG("enter, enable=%d=====>\n", enable);

#if !defined(TCSUPPORT_CPU_EN7580)&&!defined(TCSUPPORT_CPU_EN7528)
	if(enable)
		doValPut("/proc/tc3162/wifi_dev_maxbandwidth", "1");
	else
		doValPut("/proc/tc3162/wifi_dev_maxbandwidth", "0");
#endif

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}


int blapi_traffic_get_dev_bandwidth(DevBandWidth_Info_t *DevBandWidth_Info)
{
	FILE *fp = NULL;
	char buf[128] = {0};
	int line = 0, i = 0, index = 0;

	LIB_BLAPI_DEBUG("blapi_traffic_get_dev_bandwidth() enter=====>\n" );
#if (!defined TCSUPPORT_CPU_EN7580) || (defined TCSUPPORT_WLAN_MT7615_TXOFFLOAD)
	fp = fopen("/proc/tc3162/devBandwidth", "r");
	if(fp == NULL){
		LIB_BLAPI_ERROR("blapi_traffic_get_dev_bandwidth():open /proc/tc3162/devBandwidth failed!\n");
		return -1;
	}
	
	/* get count from sw */
	while (fgets(buf, 100, fp) != NULL) 
	{
		/* skip first line */
		if (line++ == 0 )
			continue;

		if(i < 30)
			sscanf(buf, "%d,%llu,%llu,%s", &index, &DevBandWidth_Info[i].UpRate, &DevBandWidth_Info[i].DwRate, DevBandWidth_Info[i].mac);
		else
			break;
		i++;
	}
	fclose(fp);
#endif

	LIB_BLAPI_DEBUG("blapi_traffic_get_dev_bandwidth() leave=====>\n" );
	return 0;
}


int blapi_traffic_get_dev_acnt_by_mac(int EnableStats,char *mac, unsigned long long *rxbytes, unsigned long long *txbytes)
{
	unsigned long long sw_rxbytes = 0, sw_txbytes = 0, hw_rxbytes = 0, hw_txbytes = 0;
	unsigned long long temp_sw_rxbytes = 0, temp_sw_txbytes = 0, temp_hw_rxbytes = 0, temp_hw_txbytes = 0;
	unsigned int rxbytes_l = 0, rxbytes_h = 0, txbytes_l = 0, txbytes_h = 0;
	unsigned int rxpkt_cnt = 0, txpkt_cnt = 0, rxpkt_cnt_h = 0, txpkt_cnt_h = 0;
	unsigned long long 	rxpackets = 0, txpackets = 0;
	
	FILE *fp = NULL;
	char buf[128] = {0}, mac_addr[32] = {0};
	int line = 0;
	int index = 0, found_sw = 0, found_hw = 0;
	unsigned char dev_macaddr[6] = {0};

	LIB_BLAPI_DEBUG("enter, mac=%s, EnableStats=%d=====>\n" ,mac, EnableStats);
	
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	sscanf(mac, "%02X%02X%02X%02X%02X%02X",&(dev_macaddr[0]), &(dev_macaddr[1]), &(dev_macaddr[2]), &(dev_macaddr[3]), &(dev_macaddr[4]), &(dev_macaddr[5]));

	LIB_BLAPI_DEBUG("mac is %02x%02x%02x%02x%02x%02x \n", dev_macaddr[0], dev_macaddr[1], dev_macaddr[2], dev_macaddr[3], dev_macaddr[4], dev_macaddr[5]);
	ecnt_fe_lib_get_dev_mac_account(DOWN_DIR, dev_macaddr, &rxbytes_l, &rxbytes_h, &rxpkt_cnt, &rxpkt_cnt_h);
	ecnt_fe_lib_get_dev_mac_account(UP_DIR, dev_macaddr, &txbytes_l, &txbytes_h, &txpkt_cnt, &txpkt_cnt_h);
	*rxbytes = rxbytes_l + (((unsigned long long)rxbytes_h) << 32);
	*txbytes = txbytes_l + (((unsigned long long)txbytes_h) << 32);
	rxpackets = rxpkt_cnt + (((unsigned long long)rxpkt_cnt_h) << 32);
	txpackets = txpkt_cnt + (((unsigned long long)txpkt_cnt_h) << 32);
	LIB_BLAPI_DEBUG("##rxbytes=%lld, txbytes=%lld, rxpkt_cnt=%lld, txpkt_cnt=%lld##>\n", *rxbytes, *txbytes, rxpackets, txpackets);

#else
#if !defined TCSUPPORT_CPU_EN7528
#if (!defined TCSUPPORT_CPU_EN7580) || (defined TCSUPPORT_WLAN_MT7615_TXOFFLOAD)
	/* get count from sw */
	fp = fopen("/proc/tc3162/devBandwidth", "r");
	if (fp) 
	{
		while (fgets(buf, 100, fp) != NULL) 
		{
			/* skip first line */
			if (line++ == 0)
				continue;

			sscanf(buf, "%d,%llu,%llu,%s", &index, &temp_sw_txbytes, &temp_sw_rxbytes, mac_addr);
			
			if (strcasecmp(mac, mac_addr) == 0) 
			{
				sw_rxbytes = temp_sw_rxbytes;
				sw_txbytes = temp_sw_txbytes;

				break;
			}
		}
		fclose(fp);
	}
#endif

	if (EnableStats == 1) 
	{
#endif
#if (defined TCSUPPORT_CPU_EN7528)&& (!defined TCSUPPORT_CPU_EN7580)

		sscanf(mac, "%02X%02X%02X%02X%02X%02X",&(dev_macaddr[0]), &(dev_macaddr[1]), &(dev_macaddr[2]), &(dev_macaddr[3]), 
			&(dev_macaddr[4]), &(dev_macaddr[5]));

		ecnt_fe_lib_get_dev_mac_account(UP_DIR, dev_macaddr, &txbytes_l, &txbytes_h);
		ecnt_fe_lib_get_dev_mac_account(DOWN_DIR, dev_macaddr, &rxbytes_l, &rxbytes_h);
		*rxbytes = rxbytes_l + (((unsigned long long)rxbytes_h) << 32);
		*txbytes = txbytes_l + (((unsigned long long)txbytes_h) << 32);
		LIB_BLAPI_DEBUG("rxbytes=%lld, txbytes=%lld=====>\n",*rxbytes, *txbytes);
#else
		line = 0;
		/* get count from hw */
		fp = fopen("/proc/tc3162/hwnat_bandwidth", "r");
		if (fp) {
			while (fgets(buf, 100, fp) != NULL) 
			{
				/* skip first line */
				if (line++ == 0)
					continue;

				sscanf(buf, "%d,%llu,%llu,%s", &index, &temp_hw_txbytes, &temp_hw_rxbytes, mac_addr);
				
				if (strcasecmp(mac, mac_addr) == 0) 
				{
					hw_rxbytes = temp_hw_rxbytes;
					hw_txbytes = temp_hw_txbytes;
					break;
				}
			}
			fclose(fp);
		}
		*rxbytes = sw_rxbytes + hw_rxbytes;
		*txbytes = sw_txbytes + hw_txbytes;
		LIB_BLAPI_DEBUG("rxbytes=%lld, txbytes=%lld=====>\n",*rxbytes, *txbytes);
#endif
#if !defined TCSUPPORT_CPU_EN7528
	}
	else
	{

		*rxbytes = sw_rxbytes;
		*txbytes = sw_txbytes;
		LIB_BLAPI_DEBUG("rxbytes=%lld, txbytes=%lld=====>\n", *rxbytes, *txbytes);
	}
#endif
#endif
	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}

int blapi_traffic_set_traffic_class(char* type, int minVal, int maxVal, int flag)
{
	ECNT_FEMGR_TRFC_INFO trfc1;
	ECNT_FEMGR_TRFC_INFO trfc2;

	if(!type || !strlen(type))
	{
		LIB_BLAPI_ERROR("leave, type=NULL, return -1=====>\n");
		return -1;
	}
	else
	{
		LIB_BLAPI_DEBUG("enter, type=%s,minVal=%d, maxVal=%d, flag=%d=====>\n", type, minVal, maxVal, flag);
	}

	memset(&trfc1, 0, sizeof(trfc1));
	memset(&trfc2, 0, sizeof(trfc2));
	trfc1.field_num = 0;
	trfc2.field_num = 0;
	
	if(0 == strcmp(type, "Pbit"))
	{
		trfc1.cpbit_min = minVal;
		trfc1.cpbit_max = maxVal;
		trfc1.mask = FE_TRFC_CPBIT;
		trfc1.field_num = 1;	
	}
	else if(0 == strcmp(type, "DSCP"))
	{
		trfc1.tc_min = minVal;
		trfc1.tc_max = maxVal;
		trfc1.mask = FE_TRFC_TC;
		trfc1.field_num = 1;	

		
		trfc2.tos_min = minVal;
		trfc2.tos_max = maxVal;
		trfc2.mask = FE_TRFC_TOS;
		trfc2.field_num = 1;
	}
	else if(0 == strcmp(type, "TOS"))
	{
		/* TOS can't set range on web */
		trfc1.tos_min = minVal >> 2;
		trfc1.tos_max = maxVal >> 2; 
		trfc1.mask = FE_TRFC_TOS;
		trfc1.field_num = 1;
	}
	else if(0 == strcmp(type, "VLANID"))
	{
		if(minVal == maxVal)
		{
			
			trfc1.cvid_max = minVal;
			trfc1.cvid_min = maxVal;
			trfc1.mask = FE_TRFC_CVID;
			trfc1.field_num = 1;
		}
		else
		{
			trfc1.cvid_max = minVal;
			trfc1.cvid_min = minVal;
			trfc1.mask = FE_TRFC_CVID;
			trfc1.field_num = 1;
			
			trfc2.cvid_max = maxVal;
			trfc2.cvid_min = maxVal;
			trfc2.mask = FE_TRFC_CVID;
			trfc2.field_num = 1;
		}
	}

	if(ADD_TRAFFIC_CLASS == flag)
	{	/*Add*/
		if(trfc1.field_num)
		{
			fe_lib_add_traffic_class(trfc1);
		}

		if(trfc2.field_num)
		{
			fe_lib_add_traffic_class(trfc2);
		}
	}
	else
	{	/*del*/
		if(trfc1.field_num)
		{
			fe_lib_del_traffic_class(trfc1);
		}

		if(trfc2.field_num)
		{
			fe_lib_del_traffic_class(trfc2);
		}
	}
	
	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}


static int set_wifi_interface_ratelimit_by_lib(unsigned int dir, unsigned char wifiIdx, unsigned int rate)
{
#if defined(TCSUPPORT_WLAN)
	int enable = 0;
	int raIdx = 0;
	ECNT_FEMGR_RATELIMIT_IF_INFO if_info = {0};
	ECNT_FEMGR_METER_INFO p_meter_info = {0};

	enable = rate ? 1 : 0;
	if ( wifiIdx < MAX_ECNT_WALN_PORT_NUM )
	{
		raIdx = wifiIdx + MAX_ECNT_WALN_PORT_NUM + 4;
	}
	else
	{
		raIdx = wifiIdx - MAX_ECNT_WALN_PORT_NUM + 4;	
	}

	LIB_BLAPI_DEBUG("enter,enable=%d, dir=%d, wifiIdx=%d, raIdx=%d, rate=%d=====>\n",enable,dir, wifiIdx, raIdx, rate);

	memset(&if_info, 0, sizeof(if_info));
	if_info.index = raIdx;
	if_info.enable = enable;
	if_info.rate = rate;

	memset(&p_meter_info, 0, sizeof(p_meter_info));
	p_meter_info.dir = dir;
	p_meter_info.enable = enable;
	p_meter_info.rate = rate;
	p_meter_info.index = raIdx;
	p_meter_info.mode = 2;

	/*1. set the mode IF based*/
	if (0 != fe_lib_set_meter_ratelimit_mode(dir, 0, 2))
	{
		LIB_BLAPI_ERROR("leave,set meter mode fail,return -1=====>\n");
		return -1;
	}

	fe_lib_set_meter_ratelimit_by(&p_meter_info);
	fe_lib_set_if_ratelimit_rule(&if_info, dir);

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
#else
	return -1;
#endif
}

static int set_wifi_interface_ratelimit_by_proc(unsigned int dir, unsigned char wifiIdx, unsigned int rate)
{
#if defined(TCSUPPORT_WLAN)
	int raIdx = 0;
	char* ptrRxTx = NULL;
	char *ptrRaIf = NULL;
	char cmdStr[256] = {0};
	char *path = "/proc/tc3162/wlan_ssid_ratelimit";

	LIB_BLAPI_DEBUG("enter, dir=%d, wifiIdx=%d, rate=%d=====>\n", dir, wifiIdx, rate);
	
	ptrRxTx = dir ? "rx" : "tx";
	if ( wifiIdx < MAX_ECNT_WALN_PORT_NUM )
	{
		raIdx = wifiIdx;
		ptrRaIf = "ra";
	}
	else
	{
		raIdx = wifiIdx - MAX_ECNT_WALN_PORT_NUM;
		ptrRaIf = "rai";
	}
	
	memset(cmdStr, 0, sizeof(cmdStr));
	snprintf(cmdStr, sizeof(cmdStr), "%s%d %s %d", ptrRaIf, raIdx, ptrRxTx, rate);
	LIB_BLAPI_DEBUG("cmdStr=%s=====>\n", cmdStr);
	doValPut(path, cmdStr);

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
#else
	return -1;
#endif
}

int blapi_traffic_set_wifi_interface_ratelimit(unsigned int dir, unsigned char wifiIdx, unsigned int rate)
{
	int ret = -1;
#if defined(TCSUPPORT_WLAN)
	LIB_BLAPI_DEBUG("enter,dir=%d,wifiIdx=%d, rate=%d=====>\n", dir, wifiIdx, rate);
	
#ifdef TCSUPPORT_CPU_EN7580
	ret =  set_wifi_interface_ratelimit_by_lib(dir, wifiIdx, rate);
#else
	ret =  set_wifi_interface_ratelimit_by_proc(dir, wifiIdx, rate);
#endif

	LIB_BLAPI_DEBUG("leave,return %d=====>\n", ret);
#endif
	return ret;
}

int blapi_traffic_check_loop_detect(char *ifname, char *Status, char *detectionStatus)
{
	FILE *fp = NULL;
	char *tmp = NULL;
	char  tempvalue[2] = {0};
	int index = -1;
	char *info_path = "/tmp/ether_status";
	int res = 0;
	int iStatus = 0;
	char value[20] = {0};
	char flag = 0;
	char cmd[128] = {0}; 

	LIB_BLAPI_DEBUG("enter,ifname=%s=====>\n", ifname);
	
	if ( (tmp=strstr(ifname, "eth0.")) != NULL ) 
	{
		tempvalue[0] = *(tmp + strlen("eth0.") );
		tempvalue[1] = '\0';
		index = atoi(tempvalue) - 1;
		snprintf(cmd, sizeof(cmd), "/userfs/bin/ethphxcmd eth0 status port %d > %s", index, info_path);
		system(cmd);

		fp = fopen(info_path,"r");
		if ( fp != NULL ) 
		{
			res = fscanf(fp,"%d",&iStatus);
			snprintf(Status, 4, "%d", iStatus);
			fclose(fp);
		}	
		unlink(info_path);

#if defined(TCSUPPORT_CT_LOOPDETECT)
		fp = fopen("/proc/tc3162/loop_detect_status", "r");
		if ( fp != NULL ) 
		{
			res = fscanf(fp, "%8s", value);
			flag = value[index];
			snprintf(detectionStatus, 20, "%c", flag);
			fclose(fp);
		}
	}
	else if ( (tmp = strstr(ifname, "ra")) != NULL ) 
	{
		fp = fopen("/proc/tc3162/loop_detect_status", "r");
		if ( fp != NULL ) 
		{
			tempvalue[0] = *(tmp + strlen("ra") );
			tempvalue[1] = '\0';
			index = atoi(tempvalue) - 1;
			res = fscanf(fp, "%8s", value);
			flag = value[index+4];
			snprintf(detectionStatus, 20, "%c", flag);
			fclose(fp);
		}
#endif
	}
	
	LIB_BLAPI_DEBUG("leave,return 0, Status=%s, detectionStatus=%s=====>\n", Status, detectionStatus);
    return 0;
}

int blapi_traffic_set_switch_linkmode(char *enable,int switchpara_id, char *maxbitrate, char *duplexmode)
{
	char cmdbuf[100] = {0}; 

	if(enable == NULL || maxbitrate == NULL || duplexmode == NULL)
	{
		return 0;
	}

	LIB_BLAPI_DEBUG("enter,enable=%s, switchpara_id=%d, maxbitrate=%s, duplexmode=%s,=====>\n", enable, switchpara_id, maxbitrate, duplexmode);
	
	if(strcmp(enable,"Yes"))
	{
		snprintf(cmdbuf,sizeof(cmdbuf),SWITCHPARA_CMD_ENABLE,"disable",switchpara_id);
		system(cmdbuf);
		LIB_BLAPI_DEBUG("leave, return 0, cmdbuf=%s=====>\n", cmdbuf);
		return 0;
	}
	else
	{
		snprintf(cmdbuf,sizeof(cmdbuf),SWITCHPARA_CMD_ENABLE,"enable",switchpara_id);
		LIB_BLAPI_DEBUG("cmdbuf=%s=====>\n", cmdbuf);
		system(cmdbuf);
		usleep(100000);
	}	

	if('\0' == maxbitrate[0] &&  '\0' == duplexmode[0])
	{
		return 0;
	}	
		
	if(!strcmp(maxbitrate,"10") && !strcmp(duplexmode,"Half"))
	{
		snprintf(cmdbuf,sizeof(cmdbuf),SWITCHPARA_CMD,"10HD",switchpara_id);
	}
	else if(!strcmp(maxbitrate,"10") && !strcmp(duplexmode,"Full"))
	{
		snprintf(cmdbuf,sizeof(cmdbuf),SWITCHPARA_CMD,"10FD",switchpara_id);
	}
	else if(!strcmp(maxbitrate,"100") && !strcmp(duplexmode,"Half"))
	{
		snprintf(cmdbuf,sizeof(cmdbuf),SWITCHPARA_CMD,"100HD",switchpara_id);
	}
	else if(!strcmp(maxbitrate,"100") && !strcmp(duplexmode,"Full"))
	{
		snprintf(cmdbuf,sizeof(cmdbuf),SWITCHPARA_CMD,"100FD",switchpara_id);
	}
	else
	{
		snprintf(cmdbuf,sizeof(cmdbuf),SWITCHPARA_CMD,"auto",switchpara_id);
	}
	
	LIB_BLAPI_DEBUG("cmdbuf=%s=====>\n", cmdbuf);
	system(cmdbuf);

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}

int blapi_traffic_set_switch_auto_linkmode
(char *enable, int switchpara_id, char *maxbitrate, char *duplexmode)
{
	char cmdbuf[256] = {0}; 

	if ( NULL == enable || NULL == maxbitrate || NULL == duplexmode )
	{
		return -1;
	}

	bzero(cmdbuf, sizeof(cmdbuf));

	LIB_BLAPI_DEBUG("enter, enable=%s, switchpara_id=%d, maxbitrate=%s, duplexmode=%s=====>\n", enable, switchpara_id, maxbitrate, duplexmode);

	if ( 0 != strcmp(enable,"Yes") )
	{
		snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_CMD_ENABLE, "disable", switchpara_id);
		system(cmdbuf);
		LIB_BLAPI_DEBUG("leave, return 0, cmdbuf=%s=====>\n", cmdbuf);
		return 0;
	}
	else
	{
		snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_CMD_ENABLE, "enable", switchpara_id);
		LIB_BLAPI_DEBUG("cmdbuf=%s=====>\n", cmdbuf);
		system(cmdbuf);
		usleep(100000);
	}

	if ( 0 == maxbitrate[0] && 0 == duplexmode[0] )
	{
		return 0;
	}

	/* duplex mode check */
	if ( 0 == strcmp(duplexmode, "Auto") )
	{
		if ( 0 == strcmp(maxbitrate, "Auto") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x00");
		}
		else if ( 0 == strcmp(maxbitrate, "10") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x10");
		}
		else if ( 0 == strcmp(maxbitrate, "1000") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x20");
		}
		else if ( 0 == strcmp(maxbitrate, "100") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x30");
		}
	}
	else if ( 0 == strcmp(duplexmode, "Full") )
	{
		if ( 0 == strcmp(maxbitrate, "10") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x01");
		}
		else if ( 0 == strcmp(maxbitrate, "100") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x02");
		}
		else if ( 0 == strcmp(maxbitrate, "1000") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x03");
		}
		else if ( 0 == strcmp(maxbitrate, "Auto") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x04");
		}
	}
	else if ( 0 == strcmp(duplexmode, "Half") )
	{
		if ( 0 == strcmp(maxbitrate, "10") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x11");
		}
		else if ( 0 == strcmp(maxbitrate, "100") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x12");
		}
		else if ( 0 == strcmp(maxbitrate, "1000") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x13");
		}
		else if ( 0 == strcmp(maxbitrate, "Auto") )
		{
			snprintf(cmdbuf, sizeof(cmdbuf), SWITCHPARA_AUTO_CMD, switchpara_id, "0x14");
		}
	}
	LIB_BLAPI_DEBUG("cmdbuf=%s=====>\n", cmdbuf);
	system(cmdbuf);

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}


int blapi_traffic_set_broadcast_ratelimit(int bsr_lan, int rate)
{
	int bsr_index = 0;
	char cmd[128] = {0}; 

	LIB_BLAPI_DEBUG("enter, bsr_lan=%d, rate=%d=====>\n", bsr_lan, rate);
	for(bsr_index = 0; bsr_index < 4; bsr_index++)
	{
		if(bsr_lan & (1 << bsr_index))
		{
			snprintf(cmd, sizeof(cmd), "/userfs/bin/ethphxcmd gsww 2%d1c ce0%d0%d0%d", 
				bsr_index+1, rate, rate, rate);

			LIB_BLAPI_DEBUG("cmd=%s=====>\n", cmd);
			system(cmd);
		}
	}

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");
	return 0;
}

int get_hsgmii_lan(int *hsgmii_lan)
{
	FILE *fp = NULL;
	char buf[32] = {0};
	
	fp = fopen("/proc/tc3162/hsgmii_lan", "r");
	if( fp )
	{
		if (fgets(buf, 32, fp) != NULL) 
		{
			sscanf(buf, "%d\n", hsgmii_lan);
		}
		fclose(fp);
	}
	return 0;
}

int blapi_traffic_set_default_switch_portmap(void)
{
	char buf[16] = {0};
	int hsgmii_lan = -1;
	LIB_BLAPI_DEBUG("enter=====>\n");

#if !defined(TCSUPPORT_WAN_XSI)	
#if !defined(TCSUPPORT_MANUAL_ETHERNET_PORTMAP)
	/*Set switch portmap for Etherwan Mode*/
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	system("echo 0 1 2 3 5 5 5 5 -1 -1 -1 -1 > /proc/tc3162/eth_portmap"); 
	system("echo -1 -1 -1 8 9 10 11 -1 -1 -1 -1 -1 > /proc/tc3162/eth_port_vlan"); 
#else
	system("echo 4 1 2 3 -1 -1 -1 > /proc/tc3162/eth_portmap");
#endif
	system("/userfs/bin/ethphxcmd eth0 vlanpt disable");
	system("/userfs/bin/ethphxcmd eth0 vlanpt enable");
	
#if defined(TCSUPPORT_HSGMII_LAN) && !defined(TCSUPPORT_CPU_EN7581)
	get_hsgmii_lan(&hsgmii_lan);

	if( 0 == hsgmii_lan )
	{
		system("/sbin/rmmod hsgmii_lan");
		system("/sbin/insmod /lib/modules/hsgmii_lan.ko hsgmii_usb_itf=\"eth0.4\"");
		system("echo eth0.4 > /tmp/hsgmii_itf");		
		system("/sbin/ifconfig eth0.4 up");
		system("/usr/bin/brctl addif br0 eth0.4");
	}
#endif
	
#if defined(TCSUPPORT_ECNT_MAP) && defined(TCSUPPORT_CPU_EN7528)
	system("/userfs/bin/ethphxcmd gsww 200c 10");
	system("/userfs/bin/ethphxcmd gsww 210c 10");
	system("/userfs/bin/ethphxcmd gsww 220c 10");
	system("/userfs/bin/ethphxcmd gsww 230c 10");
	system("/userfs/bin/ethphxcmd gsww 240c 10");
	system("/userfs/bin/ethphxcmd gsww 260c 10");
	system("/userfs/bin/ethphxcmd arl mactbl-clr");
#endif
#endif
#endif

	LIB_BLAPI_DEBUG("leave=====>\n");
	return 0;

}

int blapi_traffic_set_switch_portmap(char *portmap, char *portvlan)
{
	LIB_BLAPI_DEBUG("enter=====>\n");
	char cmdbuf[128] = {0};
	
	/*Set switch portmap from romfile for Etherwan Mode*/
	if(portmap && portmap[0])
	{	
		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "echo %s > /proc/tc3162/eth_portmap", portmap);
		system(cmdbuf);
	}
	
	if(portvlan && portvlan[0])

	{	
		memset(cmdbuf, 0, sizeof(cmdbuf));
		snprintf(cmdbuf, sizeof(cmdbuf), "echo %s > /proc/tc3162/eth_port_vlan", portvlan);
		system(cmdbuf);
	}

	system("/userfs/bin/ethphxcmd eth0 vlanpt disable");
	system("/userfs/bin/ethphxcmd eth0 vlanpt enable");
#if defined(TCSUPPORT_ECNT_MAP) && defined(TCSUPPORT_CPU_EN7528)
	system("/userfs/bin/ethphxcmd gsww 200c 10");
	system("/userfs/bin/ethphxcmd gsww 210c 10");
	system("/userfs/bin/ethphxcmd gsww 220c 10");
	system("/userfs/bin/ethphxcmd gsww 230c 10");
	system("/userfs/bin/ethphxcmd gsww 240c 10");
	system("/userfs/bin/ethphxcmd gsww 260c 10");
	system("/userfs/bin/ethphxcmd arl mactbl-clr");
#endif

	LIB_BLAPI_DEBUG("leave=====>\n");
	return 0;
}

int blapi_traffic_set_general_rx_ratelimit(void)
{	
#if defined(TCSUPPORT_CPU_EN7580)
		system("/userfs/bin/qdmamgr_lan set general_rx_ratelimit value 0 0");
		system("/userfs/bin/qdmamgr_wan set general_rx_ratelimit value 0 0");
#else
		system("/userfs/bin/qdmamgr_lan set rxratelimit value 0 0");
		system("/userfs/bin/qdmamgr_wan set rxratelimit value 0 0");
#endif

	return 0;
}

int blapi_traffic_get_ether_port_num(int *portNum)
{
	char *portNum_path = "/tmp/ether_portNum";
	char cmd[64]={0};
	int iNum;	
	int count = 0;
	FILE *fp = NULL;
	
	LIB_BLAPI_DEBUG("enter=====>\n");
	
	sprintf(cmd, "/userfs/bin/ethphxcmd eth0 portNum > %s", portNum_path);
	system(cmd);

	fp = fopen(portNum_path,"r");
	if(fp != NULL)
	{
		count = fscanf(fp,"%d",&iNum);
		fclose(fp);
		if( iNum != 1 )
		{
#if defined(TCSUPPORT_CT_1PORT)
			*portNum = 1;
#elif defined(TCSUPPORT_CT_2PWIFI) || defined(TCSUPPORT_CUC_2PORT) || defined(TCSUPPORT_CT_2PORTS)
			*portNum = 2;
#else
			*portNum = 4;
#endif
		}
	}
	unlink(portNum_path);

	LIB_BLAPI_DEBUG("leave,portNum=%d,return 0=====>\n", *portNum);
	return 0;
}

int blapi_traffic_black_ip_for_hwnat_add(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign)
{
	return ppemgr_lib_black_ip_add(ip, mask, type, sign);
}

int blapi_traffic_black_ipv6_for_hwnat_add(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign)
{
	return ppemgr_lib_black_ipv6_add(u6_addr8, mask, type, sign);
}

int blapi_traffic_black_ip_for_hwnat_delete(unsigned int sign)
{
	return ppemgr_lib_black_ip_delete_same_sign(sign);
}

int blapi_traffic_get_port_rx_cnt(unsigned char port, unsigned long long *outptr_Cnt)
{
    unsigned int RxByte_H = 0,RxByte_L = 0;
    RxByte_L = (*outptr_Cnt) & 0xffff;
    RxByte_H = (*outptr_Cnt) >>16;
    
    if( switchmgr_lib_get_port_rx_byte(port,&RxByte_H,&RxByte_L) )
		return -1;
	
	*outptr_Cnt = ((unsigned long long)RxByte_H << 32) | RxByte_L;
	return 0;
}

int blapi_traffic_get_port_tx_cnt(unsigned char port, unsigned long long *outptr_Cnt)
{
    unsigned int TxByte_H = 0,TxByte_L = 0;
    TxByte_L = (*outptr_Cnt) & 0xffff;
    TxByte_H = (*outptr_Cnt) >>16;
    
    if( switchmgr_lib_get_port_tx_byte(port,&TxByte_H,&TxByte_L) )	
		return -1;
	
	*outptr_Cnt = ((unsigned long long)TxByte_H << 32) | TxByte_L;
	return 0;
}

int blapi_traffic_get_port_rx_pkts(unsigned char port, unsigned int *outptr_Cnt)
{
	int ret = 0;
	ECNT_SWITCHMGR_PORT_STATISTICS portcnt = {0};

	memset(&portcnt, 0, sizeof(portcnt));
    ret = switchmgr_lib_get_port_statistics(port, &portcnt);
    if( ECNT_SWITCHMGR_SUCCESS == ret )
    {
		*outptr_Cnt = portcnt.RxPktsCnt;
		return 0;
	}

	return -1;
}

int blapi_traffic_get_port_tx_pkts(unsigned char port, unsigned int *outptr_Cnt)
{
	int ret = 0;
	ECNT_SWITCHMGR_PORT_STATISTICS portcnt = {0};

	memset(&portcnt, 0, sizeof(portcnt));
    ret = switchmgr_lib_get_port_statistics(port, &portcnt);
    if( ECNT_SWITCHMGR_SUCCESS == ret )
    {
		*outptr_Cnt = portcnt.TxPktsCnt;
		return 0;
	}

	return -1;
}

int blapi_traffic_set_alg_rtsp_port(const char *RtspPort) {
	char *port = NULL;
	char newechoPort[64] = {0};
	char oldechoPort[64] = {0};
	int i = 0;
	FILE *fp = NULL;
	char FileStr[168] = {0};
	char OldPotsStr[64] = {0};
	int res = 0;	
	/*get old rtsp ports & set command to delete port*/
	fp = fopen("/proc/tc3162/rtsp_alg_port", "r");
	if(NULL == fp)
		return -1;
	res = fgets(FileStr, sizeof(FileStr), fp);/*just one line in this file*/
	fclose(fp);

	if( '\0' != FileStr[0])
	{
		sscanf(FileStr, "%*[^:]:%[^\n]", OldPotsStr);
		port = strtok(OldPotsStr, " ");
		while(NULL != port)
		{
			if((0 != strcmp(port, "8554")) && (0 != strcmp(port, "554")))
			{
				snprintf(oldechoPort, sizeof(oldechoPort), "DEL %s\n", port);
				doValPut("/proc/tc3162/rtsp_alg_port", oldechoPort);
			}
			port = strtok(NULL, " ");
			i++;
			if(8 == i)
				break;
		}
	}
	
	/*get new rtsp ports & set command to add port*/
	port = NULL;
	i = 0;
	port = strtok(RtspPort, ",");
	while(NULL != port)
	{
		snprintf(newechoPort, sizeof(newechoPort), "ADD %s\n", port);
		doValPut("/proc/tc3162/rtsp_alg_port", newechoPort);
		port = strtok(NULL, ",");
		i++;
		if(6 == i)
			break;
	}
	return 0;
}

int blapi_traffic_set_default_wan_interface(const char* wanname) {
	char cmd[64] = {'0'};
	snprintf(cmd,sizeof(cmd)-1,"echo %s > /proc/tc3162/fe_default_wan_itf",wanname);
	system(cmd);

	return 0;
}

int blapi_ioctl_traffic(int cmd, traffic_ioctl_data *data)
{	
	int fp = -1;
	int ret = -1;
	
	fp = open("/dev/"TRAFFIC_MODULE_NAME, O_RDONLY);
	if ( fp < 0 )
	{
		LIB_BLAPI_ERROR("leave,open error,return -1=====>\n");
		return -1;
	}

	ret = ioctl(fp, cmd, data);
	if (ret < 0)
	{
		LIB_BLAPI_DEBUG("leave,ioctl error=====>\n");
	}

	LIB_BLAPI_DEBUG("leave: fp is [%d],ret is [%d]\n", fp, ret);
	close(fp);
	return 0;
}


int blapi_traffic_uninstall_mirror_traffic(void)
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MIRROR;

	return blapi_ioctl_traffic(E_CMD_UNINSTALL, &data);
}


int blapi_traffic_add_mirror_traffic(blapi_traffic_mirror_data* mirror_data, int datalen)
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MIRROR;
	data.data = mirror_data;
	data.data_len = datalen;

	return blapi_ioctl_traffic(E_CMD_ADD_MIRROR_RULE, &data);
}


int blapi_traffic_del_mirror_traffic(blapi_traffic_mirror_data* mirror_data, int datalen)
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MIRROR;
	data.data = mirror_data;
	data.data_len = datalen;

	return blapi_ioctl_traffic(E_CMD_DEL_MIRROR_RULE, &data);
}


int blapi_traffic_install_mirror_traffic(blapi_traffic_mirror_install_data* install_data, int datalen)
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MIRROR;
	data.data = install_data;
	data.data_len = datalen;

	return blapi_ioctl_traffic(E_CMD_INSTALL, &data);
}


int blapi_traffic_monitor_cnt_traffic(int *devCnt, int datalen)
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MONITOR;
	data.data = devCnt;
	data.data_len = datalen;

	return blapi_ioctl_traffic(E_CMD_MONITOR_CNT, &data);
}

int blapi_traffic_monitor_date_traffic(blapi_traffic_monitor_data *list, int datalen)
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MONITOR;
	data.data = list;
	data.data_len = datalen;

	return blapi_ioctl_traffic(E_CMD_MONITOR_DATE, &data);
}


int blapi_traffic_uninstall_monitor_traffic(void)
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MONITOR;

	return blapi_ioctl_traffic(E_CMD_UNINSTALL, &data);
}

int blapi_traffic_install_monitor_traffic()
{	
	traffic_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.traffic_type = E_TRAFFIC_MONITOR;

	return blapi_ioctl_traffic(E_CMD_INSTALL, &data);
}

int blapi_traffic_set_wanInterface_qostype(qos_wanIf_value_t* wanIfValue)
{
	int fp = -1;
	int ret = -1;	
	struct qos_type qt = {0};

	LIB_BLAPI_DEBUG("enter=====>\n");
	
	memset(&qt, 0, sizeof(struct qos_type));
	qt.rule_no = wanIfValue->index;
	strncpy(qt.qm[0].type_name, wanIfValue->wanType_name, sizeof(qt.qm[0].type_name)-1);
	strncpy(qt.qm[0].type_value, wanIfValue->wanType_value, sizeof(qt.qm[0].type_value)-1);
	qt.qm[0].type_mask = wanIfValue->wanType_mask;
	qt.qm[0].type_protocol_type = wanIfValue->wanType_protocol_type;
	fp = open("/dev/qostype", O_RDONLY);
	if (fp < 0)
	{
		LIB_BLAPI_ERROR("leave,open error,return -1=====>\n");
		return -1;
	}
	ret = ioctl(fp, QOSTYPE_IOC_SET_TYPE, &qt);
	if (ret < 0) 
	{
		LIB_BLAPI_DEBUG("leave,ioctl error=====>\n");
	}
	close(fp);

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

int blapi_traffic_set_wanInterface_qos_template(unsigned char* wanIfIdx)
{
	int fp = -1;
	int ret = -1;	

	fp = open("/dev/qostype", O_RDONLY);
	if (fp < 0)
	{
		LIB_BLAPI_ERROR("leave,open error,return -1=====>\n");
		return -1;
	}
	ret = ioctl(fp, QOSTYPE_IOC_SET_TEMPLATE_WAN, wanIfIdx);
	if (ret < 0) 
	{
		LIB_BLAPI_DEBUG("leave,ioctl error=====>\n");
	}
	close(fp);

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");

	return 0;
}


#if defined(TCSUPPORT_CT_DS_LIMIT)
int  blapi_traffic_dslimit_set_up_data(int dsMode, char* dsValue, int g_queue_speed[])
{
	int idx = 0;
	int ret = -1;
	char cmd[128] = {0};
	struct dsnode g_dataspeed_nodes[DSLIMIT_MAX_NODES];

	if(dsValue == NULL)
	{
		LIB_BLAPI_ERROR("enter,dsValue = NULL,return -1=====>\n");
		return -1;
	}
	
	LIB_BLAPI_DEBUG("enter,dsMode=%d,dsValue=%s=====>\n",dsMode, dsValue);
	
	/* always close dataspeed limit up stream first. */
	dslimit_set_mode_and_queue_speed(DSLIMIT_UP, DSLIMIT_MODE_DISABLE, g_queue_speed );
		
	if ( DSLIMIT_MODE_DISABLE != dsMode )
	{
		bzero(g_dataspeed_nodes, sizeof(g_dataspeed_nodes));
		ret = dslimit_parse_dsValue(DSLIMIT_UP, dsMode, dsValue, g_dataspeed_nodes, g_queue_speed);
		if(ret != 0)
		{
			LIB_BLAPI_DEBUG("leave,parse dsValue fail,return -1=====>\n");
			return -1;
		}
		dslimit_set_dataspeed_nodes(DSLIMIT_UP, g_dataspeed_nodes); 
		dslimit_set_mode_and_queue_speed(DSLIMIT_UP, dsMode, g_queue_speed );
	}
	else
	{
		for(idx=0; idx<4; idx++)
		{
			snprintf(cmd, sizeof(cmd), "echo ra%d rx 0 > /proc/tc3162/wlan_ssid_ratelimit", idx);
			system(cmd);
		}
	}

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}


int  blapi_traffic_dslimit_set_down_data(int dsMode, char* dsValue, int is_boot)
{
	int idx = 0;
	int ret = -1;
	char cmd[128] = {0};
	int g_queue_speed[DSLIMIT_MAX_QUEUE_NUM] = {0};
	struct dsnode g_dataspeed_nodes[DSLIMIT_MAX_NODES];

	if(dsValue == NULL)
	{
		LIB_BLAPI_ERROR("enter,dsValue = NULL,return -1=====>\n");
		return -1;
	}

	LIB_BLAPI_DEBUG("enter,dsMode=%d,dsValue=%s,is_boot=%d=====>\n",dsMode, dsValue, is_boot);

#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
	if ( 0 == is_boot )
#endif
	/* always close dataspeed limit down stream first. */
	dslimit_set_mode_and_queue_speed( DSLIMIT_DW, DSLIMIT_MODE_DISABLE, g_queue_speed );
		
	if ( DSLIMIT_MODE_DISABLE != dsMode )
	{
		bzero(g_queue_speed, sizeof(g_queue_speed));
		bzero(g_dataspeed_nodes, sizeof(g_dataspeed_nodes));
		ret = dslimit_parse_dsValue(DSLIMIT_DW, dsMode, dsValue, g_dataspeed_nodes, g_queue_speed);
		if(ret != 0)
		{
			LIB_BLAPI_DEBUG("leave,parse dsValue fail,return -1=====>\n");
			return -1;
		}
		dslimit_set_dataspeed_nodes(DSLIMIT_DW, g_dataspeed_nodes); 
		dslimit_set_mode_and_queue_speed(DSLIMIT_DW, dsMode, g_queue_speed );
	}
	else
	{
		for(idx=0; idx<4; idx++)
		{
			snprintf(cmd, sizeof(cmd),"echo ra%d tx 0 > /proc/tc3162/wlan_ssid_ratelimit", idx);
			system(cmd);
		}
	}

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}
#endif


int blapi_traffic_set_lan_info(char lan_ip[], char lan_mask[])
{
	LIB_BLAPI_DEBUG("enter=====>\n");
	
#if defined(TCSUPPORT_CT)
	char ip[10] 	= {0};
	char mask[10] 	= {0};
	char cmdBuf[128]= {0};
	unsigned int tmp1=0, tmp2=0, tmp3=0, tmp4=0;

	LIB_BLAPI_DEBUG("lan_ip = [%s], lan_mask = [%s]=====>\n", lan_ip, lan_ip);
	
	sscanf(lan_ip, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);
	snprintf(ip, sizeof(ip), "%02x%02x%02x%02x", tmp1, tmp2, tmp3, tmp4);
	snprintf(cmdBuf, sizeof(cmdBuf), "echo IP= %s > %s\n", ip, LAN_IP_MASK_PATH);
	system(cmdBuf);
	
	LIB_BLAPI_DEBUG("enter lan_ip,cmdBuf =[%s]=====>\n",cmdBuf);
	
	memset(cmdBuf, 0, sizeof(cmdBuf));
	sscanf(lan_mask, "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4);
	snprintf(mask, sizeof(mask), "%02x%02x%02x%02x", tmp1, tmp2, tmp3, tmp4);
	snprintf(cmdBuf, sizeof(cmdBuf), "echo Mask= %s >> %s\n", mask, LAN_IP_MASK_PATH);
	system(cmdBuf);
	LIB_BLAPI_DEBUG("enter lan_mask,cmdBuf =[%s]=====>\n",cmdBuf);
#endif

	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}



int blapi_traffic_check_eth_link_status(int *flag)
{
	char buf[64] = {0};
	FILE *fp = NULL;

	LIB_BLAPI_DEBUG("enter=====>\n");
	fp = fopen(ETHLINK_PATH, "r");
	if( NULL == fp )
	{
		LIB_BLAPI_ERROR("open %s failed! return -1=====>\n",ETHLINK_PATH);
		return -1;
	}
	while (fgets(buf, sizeof(buf), fp))
	{
		if (!strstr(buf,"Down"))
		{
			*flag = 1;
			LIB_BLAPI_DEBUG("enter,flag = %d, etherlink is up=====>\n",*flag);
		}
		else
		{
			*flag = 2;
			LIB_BLAPI_DEBUG("enter,flag = %d, etherlink is down=====>\n",*flag);
		}
	}

	fclose(fp);
	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}

int blapi_traffic_check_real_lan_num(int* lanNum)
{
	FILE *fp = NULL;
	char filebuf[64] = {0}, tmp[32] = {0};
	unsigned int lanbuf[8] = {0};
	int i = 0, realLanNum = 0;

	if((fp = fopen("/proc/tc3162/board_args_hsgmii", "r")) != NULL)
	{
		if(fgets(filebuf, sizeof(filebuf), fp))
		{
			if(sscanf(filebuf, "%02x %02x %02x %02x %02x %02x %02x %02x %s", &lanbuf[0], &lanbuf[1], &lanbuf[2], &lanbuf[3], &lanbuf[4], &lanbuf[5], &lanbuf[6], &lanbuf[7], tmp) != 9)
			{
				fclose(fp);
				ecnt_blapi_printf("the number of port is wrong, filebuf=[%s]\n", filebuf);
				return -1;
			}
		}
		else
		{
			ecnt_blapi_printf("file /proc/tc3162/board_args_hsgmii is empty\n");
			fclose(fp);
			return -1;
		}
	}
	else
	{
		return -1;
	}

	for(i = 0; i < 8; i++)
	{
		if(lanbuf[i] != 0x00)
			realLanNum++;
	}
	
	*lanNum = realLanNum;
	fclose(fp);

	return 0;
}

int blapi_traffic_check_eth_port_status(char *buf, int portStatus[])
{
	FILE *fp = NULL;
	char line[256] = {0};
	unsigned int lanbuf[8] = {0};
	char tmp[32] = {0}, filebuf[64] = {0};
	int i = 0, etherUp = 0, ret = 0, blapi_idx = -1;
	int lanstatus[4] = {0};

	fp = fopen(ETH_PORT_STATUS_FILE_PATH, "r");
	if(NULL != fp)
	{
		if(fgets(line, sizeof(line), fp))
		{
			if(sscanf(line, "%d %d %d %d", &lanstatus[0], &lanstatus[1], &lanstatus[2], &lanstatus[3]) != 4)
			{	
				fclose(fp);
				ecnt_blapi_printf("the number of port is wrong, return -1\n");
				return -1;
			}
		}
	}
	else
	{
		ecnt_blapi_printf("open %s failed! return -1\n", ETH_PORT_STATUS_FILE_PATH);
		return -1;
	}
	fclose(fp);

#if !defined(TCSUPPORT_HSGMII_LAN)
	memcpy(portStatus, lanstatus, sizeof(lanstatus));		
	sprintf(buf, "%d %d %d %d", portStatus[0],portStatus[1],portStatus[2],portStatus[3]);
#else
	fp = fopen("/proc/tc3162/board_args_hsgmii", "r");
	if(NULL != fp)
	{
		if(fgets(filebuf, sizeof(filebuf), fp))
		{
			if(sscanf(filebuf, "%02x %02x %02x %02x %02x %02x %02x %02x %s", &lanbuf[0], &lanbuf[1], &lanbuf[2], &lanbuf[3], &lanbuf[4], &lanbuf[5], &lanbuf[6], &lanbuf[7], tmp) != 9)
			{
				fclose(fp);
				ecnt_blapi_printf("the number of port is wrong, filebuf=[%s]\n", filebuf);
				return -1;
			}
		}
	}
	else
	{
		return -1;
	}

	fclose(fp);
	memcpy(portStatus, lanstatus, sizeof(lanstatus));
	for(i = 0; i < 8; i++)
	{
		if(lanbuf[i] >= HSGMII_USB && lanbuf[i] <= HSGMII_ETH)
		{
			if(lanbuf[i] == HSGMII_USB) 		 
				blapi_idx = 0x02;
			else if(lanbuf[i] == HSGMII_PCIE0) 
				blapi_idx = 0x00;
			else if(lanbuf[i] == HSGMII_PCIE1) 
				blapi_idx = 0x01;
			else if(lanbuf[i] == HSGMII_ETH)   
				blapi_idx = 0x03;

			ret = blapi_traffic_get_multi_hsgmii_lan_state(&etherUp, blapi_idx);
			if(ret == 0 && etherUp == 1)
				portStatus[i] = 1;

		}
	}

	sprintf(buf, "%d %d %d %d %d %d %d %d", portStatus[0],portStatus[1],portStatus[2],portStatus[3],portStatus[4],portStatus[5],portStatus[6],portStatus[7]);
#endif
	return 0;
}

int blapi_traffic_get_portrate_info(char * buf, int size)
{
	char line[256] = {0};
	FILE *fp = NULL;
	fp = fopen(GSW_LINK_ST_PATH, "r");
	if (fp == NULL)
	{
		LIB_BLAPI_ERROR("open %s failed! return -1=====>\n",GSW_LINK_ST_PATH);
		return -1;
	}
	while(fgets(line, sizeof(line), fp)){
		strncat(buf, line, strlen(line));
	}
	buf[size - 1] = '\0';
	fclose(fp);
	return 0;
}

int blapi_traffic_get_lanport_info(lanport_info_t *lanport_info)
{
	FILE *fp = NULL;
	char rd_buf[512] = {0}, *p_switch_map = NULL, rate_buf[64] = {0};
	char buf[64] = {0}, tmp[32] = {0};
	int i_swtichmap[6][2] = {0};
	char s_switchmap[6][80] = {0};
	int rd_cnt = 0, idx = 0, i_lan_port = 0, etherUp = 0, ret = 0, etherrate = -1;;
	lanport_info_t lanstatus;
	char *idx_name[4] = {"pcie0_hsgmii_idx","pcie1_hsgmii_idx","usb_hsgmii_idx","eth_hsgmii_idx"};
	int port_idx[4] = {-1,-1,-1,-1};
	char *linkrate[6] = {"10000M/Full","5000M/Full","2500M/Full","1000M/Full","100M/Full","10M/Full"};

	LIB_BLAPI_DEBUG("enter=====>\n");
	bzero(&lanstatus, sizeof(lanstatus));
	fp = fopen("/proc/tc3162/eth_portmap", "r");
	if ( fp )
	{
		rd_cnt = fread(rd_buf, 1, sizeof(rd_buf) - 1, fp);
		if ( rd_cnt > 0 )
		{
			rd_buf[rd_cnt] = 0;
			p_switch_map = strstr(rd_buf, "switch_port_map");
			if ( p_switch_map )
			{
				sscanf(p_switch_map,
						"switch_port_map\n"
						"%d %d\n"
						"%d %d\n"
						"%d %d\n"
						"%d %d\n"
						"%d %d\n"
						"%d %d"
						, &i_swtichmap[0][0], &i_swtichmap[0][1]
						, &i_swtichmap[1][0], &i_swtichmap[1][1]
						, &i_swtichmap[2][0], &i_swtichmap[2][1]
						, &i_swtichmap[3][0], &i_swtichmap[3][1]
						, &i_swtichmap[4][0], &i_swtichmap[4][1]
						, &i_swtichmap[5][0], &i_swtichmap[5][1]);
			}

		}

		fclose(fp);
	}

	fp = fopen("/proc/tc3162/gsw_link_st", "r");
	if ( fp )
	{
		rd_cnt = fread(rd_buf, 1, sizeof(rd_buf) - 1, fp);
		if ( rd_cnt > 0 )
		{
			rd_buf[rd_cnt] = 0;
			p_switch_map = strstr(rd_buf, "Internal switch");
			if ( p_switch_map )
			{
				sscanf(p_switch_map,
						"Internal switch: \n"
						"Port0 : %[^\n]\n"
						"Port1 : %[^\n]\n"
						"Port2 : %[^\n]\n"
						"Port3 : %[^\n]\n"
						"Port4 : %[^\n]\n"
						"Port5 : %[^\n]"
						, s_switchmap[0]
						, s_switchmap[1]
						, s_switchmap[2]
						, s_switchmap[3]
						, s_switchmap[4]
						, s_switchmap[5]);
			}
		}

		fclose(fp);
	}

	for ( idx = 0; idx < 6; idx ++ )
	{
		i_lan_port = i_swtichmap[idx][1];
		if ( i_lan_port >= 0 && i_lan_port <= 3 )
		{
			sscanf(s_switchmap[idx], "%s", rate_buf);
			snprintf(lanstatus.lan_port[i_lan_port], sizeof(lanstatus.lan_port[i_lan_port])
								, "LAN%d", i_lan_port + 1);
			snprintf(lanstatus.link_rate[i_lan_port], sizeof(lanstatus.link_rate[i_lan_port])
								, "%s", rate_buf);

			if ( strstr(rate_buf, "Down") )
			{
				snprintf(lanstatus.lan_status[i_lan_port], sizeof(lanstatus.lan_status[i_lan_port])
												, "%s", "DOWN");
			}
			else
			{
				snprintf(lanstatus.lan_status[i_lan_port], sizeof(lanstatus.lan_status[i_lan_port])
												, "%s", "UP");
			}
		}
	}

#if !defined(TCSUPPORT_HSGMII_LAN)
	memcpy(lanport_info, &lanstatus, sizeof(lanstatus));
#else
	if((fp = fopen("/proc/tc3162/hsgmii_idx", "r")) != NULL)
	{
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			for(idx = 0; idx < 4; idx++)
			{
				if (strstr(buf,idx_name[idx]))
				{
					if(sscanf(buf, "%*[^=]= %d\n", &port_idx[idx]) != 1)
					{
						fclose(fp);
						ecnt_blapi_printf("the number of port is wrong, filebuf=[%s]\n", buf);
						return -1;
					}
					break;
				}
			}
		}
	}
	else
		return -1;
	
	fclose(fp);
	memcpy(lanport_info, &lanstatus, sizeof(lanstatus));
	for(idx = 4; idx < 8; idx++)
	{
		snprintf(lanport_info->lan_port[idx], sizeof(lanport_info->lan_port[idx]), "LAN%d", idx+1);
		snprintf(lanport_info->lan_status[idx], sizeof(lanport_info->lan_status[idx]), "%s", "DOWN");
		snprintf(lanport_info->link_rate[idx], sizeof(lanport_info->link_rate[idx]), "%s", "Down");
	}

	for(idx = 0; idx < 4; idx++)
	{
		if(port_idx[idx] != -1)
		{
			ret = blapi_traffic_get_multi_hsgmii_lan_state(&etherUp, idx);
			if(ret == 0 && etherUp == 1)
			{
				snprintf(lanport_info->lan_status[port_idx[idx]], sizeof(lanport_info->lan_status[port_idx[idx]]), "%s", "UP");
				ret = blapi_traffic_get_multi_hsgmii_lan_linkrate(&etherrate, idx);
				if(ret == 0 && etherrate >= 0 && etherrate < 6)
				{
					snprintf(lanport_info->link_rate[port_idx[idx]], sizeof(lanport_info->link_rate[port_idx[idx]]), "%s", linkrate[etherrate]);
				}
				else
				{
					snprintf(lanport_info->link_rate[port_idx[idx]], sizeof(lanport_info->link_rate[port_idx[idx]]), "%s", "-");
				}
			}
		}
	}
#endif
	
	LIB_BLAPI_DEBUG("leave,return 0=====>\n");
	return 0;
}


int blapi_traffic_set_max_bandwidth_data(int dslflag, struct MaxBandWidth_node_info_s MaxBandWidthNodeInfo[], int* isRuleExist)
{
#if defined(TCSUPPORT_CT_JOYME_BANDWIDTH) 
	int i = 0;
	int ret = 0;
	int mac[6];
	int rate = 0;
	char upRate[32] = {0};
	char downRate[32] = {0};
	int upRuleExist = 0;	
	int downRuleExist = 0;
	char wifiBandwidthBuf[128] = {0};
	struct mt7530_switch_dev_speed_acl_s devMacAcl;
	dev_bandwidth_tmp_t tmp;

	LIB_BLAPI_DEBUG("enter,dslflag=%d======>\n",dslflag);
	
	bzero(&devMacAcl, sizeof(struct mt7530_switch_dev_speed_acl_s));
	for(i = 0; i < TRTCM_MATCH_MAC_RULE_NUM; i++)
	{	
		LIB_BLAPI_DEBUG("MaxBandwidth_Entry%d:mac=[%s],upRate=[%s],downRate=[%s]======>\n",
			i, MaxBandWidthNodeInfo[i].macAddr, MaxBandWidthNodeInfo[i].upRate, MaxBandWidthNodeInfo[i].downRate);
		
		if( '\0' != MaxBandWidthNodeInfo[i].macAddr[0])
		{
			sscanf(MaxBandWidthNodeInfo[i].macAddr, "%02x%02x%02x%02x%02x%02x",&mac[0], &mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[0] = mac[0];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[1] = mac[1];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[2] = mac[2];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[3] = mac[3];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[4] = mac[4];
			devMacAcl.trtcm_acl_mac_table[i].dev_mac[5] = mac[5];
			
			if( '\0' != MaxBandWidthNodeInfo[i].upRate[0])
			{
				rate = atoi(MaxBandWidthNodeInfo[i].upRate);	/*kbps*/		
				devMacAcl.trtcm_acl_mac_table[i].us_queue_speed = getACLRate(rate);
				if((upRuleExist == 0) && (rate != 0))
				{
					upRuleExist = 1;
				}
			}
			else
			{
				continue; 
			}
			
			if( '\0' != MaxBandWidthNodeInfo[i].downRate[0])
			{
				rate = atoi(MaxBandWidthNodeInfo[i].downRate);
				devMacAcl.trtcm_acl_mac_table[i].ds_queue_speed = getACLRate(rate);
				if((downRuleExist == 0) && (rate != 0))
				{
					downRuleExist = 1;
				}
			}
			else
			{
				continue;
			}
			
			memset(&tmp,0,sizeof(dev_bandwidth_tmp_t));
			memmove(tmp.trtcm_acl_mac_table[0].dev_mac,mac,6);
			tmp.trtcm_acl_mac_table[0].us_queue_speed = atoi(MaxBandWidthNodeInfo[i].upRate);			
			tmp.trtcm_acl_mac_table[0].ds_queue_speed = atoi(MaxBandWidthNodeInfo[i].downRate);
			tmp.trtcm_acl_mac_table[0].conn_type = 3;
			ret = blapi_traffic_set_dev_ratelimit_by_mac(&tmp,1);
		}
		else
		{
			continue;
		}	
	}

	if(downRuleExist || upRuleExist)
	{
		*isRuleExist = 1;
		devMacAcl.enable = 1;		
		ret = blapi_traffic_set_dev_acnt_enable(1,1);
		ret = blapi_traffic_set_wifi_dev_ratelimit_enable(1);
	}
	else
	{
		*isRuleExist = 0;
	}
	
#if defined(TCSUPPORT_CT_DS_LIMIT)
	if(*isRuleExist == 1 ||  (*isRuleExist == 0 && dslflag <= 0) )
#endif
	{
		ecnt_switchmgr_lib_set_mac_match_acl(&devMacAcl);
	}

#endif

	LIB_BLAPI_DEBUG("leave, return 0======>\n");
	return 0;
}

int blapi_traffic_get_foe_info_from_sock(int sock, struct SockInfo * sockinfo)
{
	memset(sockinfo,0,sizeof(struct SockInfo));
    int foelen = sizeof(struct SockInfo);
	
	if(getsockopt(sock, SOL_PACKET, PACKET_SKB_FOE_INFO, sockinfo, &foelen))
        	return -1;
	
    return 0;
}

int blapi_traffic_set_foe_info_to_sock(int sock, struct SockInfo * sockinfo)
{
	int foelen = sizeof(struct SockInfo);
	if (setsockopt(sock, SOL_PACKET, PACKET_SKB_FOE_INFO, sockinfo, foelen))
        return -1;
    return 0;
}

int blapi_traffic_get_ether_wan_state(int *etherStatus)
{
#if defined(TCSUPPORT_SISM_HOST)
	if(1 == blapi_traffic_check_dsl_enable())
	{
		int ret = -1;
		blapi_hdr_t *blapi_h;
		blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
		memset(blapi_h, 0 , sizeof(blapi_hdr_t));
		blapi_h->blapi_id = htonl(BLAPI_TRAFFIC_GET_ETHER_WAN_STATE);
		blapi_h->argc = htonl(1);

		blapi_h->argv1.i = htonl(*etherStatus);
			
		if (!SRC_blapi(blapi_h))
		{
			ret = blapi_h->ret;
		}

		free(blapi_h);
		return ret;	
	}
	else
#endif
	{
	    FILE *fp = NULL;
		char string[80] = {0};
		char etherState[8] = {0};
		int etherUp = 0;
		char buf[3] = {0};
		fp = fopen("/proc/tc3162/eth_portmap","r");
		if(fp != NULL)
		{
			if(fgets(buf, sizeof(buf), fp) != NULL)
			{	
				buf[2]='\0';
			}
			fclose(fp);	
		}
		if(strcmp(buf, "-1") == 0)
	    {
		    fp = fopen(ACTIVE_ETHERNET_WAN_STATE_PATH, "r");
		    if(fp)
		    {
				while(fgets(string, sizeof(string), fp))
				{     
					if(sscanf(string, "ae_wan_phy_link_status = %s\n" , etherState) == 1
	                && strcmp(etherState, "up") == 0)
					{
	                etherUp=1;
					break;
					}          
					memset(string, 0,sizeof(string));
				}
				fclose(fp);
			}
		}else
		{
			fp = fopen(ETHER_WAN_STATE_PATH, "r");       
			if(fp)
			{
				while(fgets(string, sizeof(string), fp))
				{     
					if(strstr(string, "10"))
					{
					etherUp=1;
					break;
					}       
	            memset(string, 0,sizeof(string));
			    }
			fclose(fp);
			}
		}
		*etherStatus = etherUp;
		
		return 0;
	}
}

int blapi_traffic_get_active_ether_wan_state(int *etherStatus)
{
    FILE *fp = NULL;
	char string[80] = {0};
	char etherState[8] = {0};
	int etherUp = 0;
	char buf[3] = {0};
	fp = fopen("/proc/tc3162/eth_portmap","r");
	if(fp != NULL)
	{
		if(fgets(buf, sizeof(buf), fp) != NULL)
		{	
			buf[2]='\0';
		}
		fclose(fp);	
	}
	if(strcmp(buf, "-1") == 0)
    {
	    fp = fopen(ACTIVE_ETHERNET_WAN_STATE_PATH, "r");
	    if(fp)
	    {
			while(fgets(string, sizeof(string), fp))
			{     
				if(sscanf(string, "ae_wan_phy_link_status = %s\n" , etherState) == 1
                && strcmp(etherState, "up") == 0)
				{
                etherUp=1;
				break;
				}          
				memset(string, 0,sizeof(string));
			}
			fclose(fp);
		}
	}else
	{
		fp = fopen(ETHER_WAN_STATE_PATH, "r");       
		if(fp)
		{
			while(fgets(string, sizeof(string), fp))
			{     
				if(strstr(string, "10"))
				{
				etherUp=1;
				break;
				}       
            memset(string, 0,sizeof(string));
		    }
		fclose(fp);
		}
	}
	*etherStatus = etherUp;
	
	return 0;

}


int blapi_traffic_get_hsgmii_lan_state(int *etherStatus)
{
    FILE *fp = NULL;
	char string[80] = {0};
	char etherState[8] = {0};
	int etherUp = 0;
	
	fp = fopen(HSGMII_LAN_STATE_PATH, "r");        
	if(fp)
	{
		while(fgets(string, sizeof(string), fp))
		{     
            if(sscanf(string, "hsgmii_lan_phy_link_status = %s\n" , etherState) == 1
                && strcmp(etherState, "up") == 0)
            {
                etherUp=1;
				break;
            }

            memset(string, 0,sizeof(string));
		}
		fclose(fp);
	}

	*etherStatus = etherUp;
	
	return 0;
}

int blapi_traffic_get_multi_hsgmii_lan_state(int *etherStatus, int hsgmii_index)
{
    FILE *fp = NULL;
	char string[80] = {0};
	char etherState[8] = {0};
	int etherUp = 0;
	if(hsgmii_index == 0)
		fp = fopen(HSGMII_PCIE0_LAN_STATE_PATH, "r"); 
	else if(hsgmii_index == 1)
		fp = fopen(HSGMII_PCIE1_LAN_STATE_PATH, "r"); 
	else if(hsgmii_index == 2)
		fp = fopen(HSGMII_USB_LAN_STATE_PATH, "r"); 
	else if(hsgmii_index == 3)
		fp = fopen(HSGMII_ETH_LAN_STATE_PATH, "r"); 
	else
		return -1;
	
	if(fp)
	{
		while(fgets(string, sizeof(string), fp))
		{     
            if(sscanf(string, "hsgmii_lan_phy_link_status = %s\n" , etherState) == 1
                && strcmp(etherState, "up") == 0)
            {
                etherUp=1;
				break;
            }

            memset(string, 0,sizeof(string));
		}
		fclose(fp);
	}

	*etherStatus = etherUp;
	
	return 0;
}

int blapi_traffic_get_multi_hsgmii_lan_linkrate(int *linkrate, int hsgmii_index)
{
    FILE *fp = NULL;
	char string[80] = {0};
	int rate = -1;
	if(hsgmii_index == 0)
		fp = fopen(HSGMII_PCIE0_LAN_STATE_PATH, "r"); 
	else if(hsgmii_index == 1)
		fp = fopen(HSGMII_PCIE1_LAN_STATE_PATH, "r"); 
	else if(hsgmii_index == 2)
		fp = fopen(HSGMII_USB_LAN_STATE_PATH, "r"); 
	else if(hsgmii_index == 3)
		fp = fopen(HSGMII_ETH_LAN_STATE_PATH, "r"); 
	else
		return -1;
	
	if(fp)
	{
		while(fgets(string, sizeof(string), fp))
		{     
            if(sscanf(string, "hsgmii_lan_linkrate = %d\n" , &rate) == 1)
            {
				break;
            }

            memset(string, 0,sizeof(string));
		}
		fclose(fp);
	}

	*linkrate = rate;
	
	return 0;
}



int blapi_traffic_get_ratelimit_mode(Stream_Dir_t dir, ECNT_TRAFFIC_RATELIMIT_MODE *mode)
{
	LIB_BLAPI_DEBUG("enter,  dir=%d=====>\n", dir);

	FILE *fp = NULL;
	char line[160] = {0};
	char mode_string[20] = {0};
	unsigned int ratelimit_mode = 0;

#if defined(TCSUPPORT_CPU_EN7580)
	fe_lib_get_meter_ratelimit_mode(dir, 0, &ratelimit_mode);

	if(ratelimit_mode == 5)
		*mode = RATELIMIT_MODE_DEV_MAC;
	else if (ratelimit_mode == 7)
		*mode = RATELIMIT_MODE_WAN;
	else if (ratelimit_mode == 4)
		*mode = RATELIMIT_MODE_TRAFFIC_QOS;
	else{
		LIB_BLAPI_DEBUG("not support this mode=====>\n");
		return -1;
	}
	
#elif defined(TCSUPPORT_CPU_EN7528)
	fp = fopen("/proc/tc3162/hwnat_meter_mode", "r");

	if(fp){
		if(fgets(line, sizeof(line),fp )){
			sscanf(line,"%[^:]", mode_string );
			ratelimit_mode = atoi(mode_string);
		}
		fclose(fp);
	}
	else
	{
		LIB_BLAPI_DEBUG("fp is NULL!!\n");
	}

	if (ratelimit_mode == 1)
		*mode = RATELIMIT_MODE_DEV_MAC;
	else if(ratelimit_mode == 2)
		*mode = RATELIMIT_MODE_WAN;
	else{
		LIB_BLAPI_DEBUG("not support this mode=====>\n");
		return -1;
	}
#endif

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");

	return 0;
}

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
int blapi_traffic_get_meter_ratelimit_mode(Stream_Dir_t dir, unsigned int idx, ECNT_TRAFFIC_RATELIMIT_MODE *mode)
{
	LIB_BLAPI_DEBUG("enter,  dir=%d=====>\n", dir);
	unsigned int ratelimit_mode = 0;

	fe_lib_get_meter_ratelimit_mode(dir, idx, &ratelimit_mode);

	if(ratelimit_mode == 5)
		*mode = RATELIMIT_MODE_DEV_MAC;
	else if (ratelimit_mode == 7)
		*mode = RATELIMIT_MODE_WAN;
	else if (ratelimit_mode == 4)
		*mode = RATELIMIT_MODE_TRAFFIC_QOS;
	else{
		LIB_BLAPI_DEBUG("not support this mode=====>\n");
		return -1;
	}

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");

	return 0;
}
#endif

int blapi_traffic_set_mtu(unsigned int mtu)
{
	int ret = -1;

	ret = ppemgr_lib_set_mtu(mtu);

	return ret;
}

int blapi_traffic_set_tls_port(int enable,unsigned char lan_port)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_set_tls_port(enable,lan_port))
		return 0;
	else
		return -1;
}

int blapi_traffic_set_tls_vlan(int enable,unsigned short vlan)
{
	if(ECNT_FEMGR_SUCCESS == fe_lib_set_tls_vlan(enable,vlan))
		return 0;
	else
		return -1;
}

int blapi_traffic_hwdrop_ipv6_unknown_mc_by_sock(int socket,unsigned char* grp_addr)
{
	struct SockInfo  info;

	memset(&info,0,sizeof(struct SockInfo));
	if(0 == blapi_traffic_get_foe_info_from_sock(socket,&info))
	{
		if(ECNT_PPEMGR_SUCCESS == ppemgr_lib_drop_unknow_multicast_by_index(info.ppe_foe_entry,info.ppe_ai,info.ppe_magic,ECNT_PPEMGR_IP_PROTO_IPV6,grp_addr))
			return 0;
		else
			return -1;
	}

	return -1;
}

int blapi_traffic_del_hwdrop_ipv6_unknown_mc(unsigned char* grp_addr)
{
	if(ECNT_PPEMGR_SUCCESS == ppemgr_lib_query_multicast_drop_list(grp_addr,ECNT_PPEMGR_IP_PROTO_IPV6))
		return 0;
	else
		return -1;
}

int blapi_traffic_ipv6_mc_set_wan_info_to_sock(int  socket,unsigned char wan_type,unsigned char wan_index)
{
	struct SockInfo  info;

	memset(&info,0,sizeof(struct SockInfo));
	if(0 == blapi_traffic_get_foe_info_from_sock(socket,&info))
	{
		info.wan_index = wan_index;
		info.wan_type = wan_type;
		if(0 == blapi_traffic_set_foe_info_to_sock(socket,&info))
			return 0;
		else
			return -1;
	}

	return -1;
}

int blapi_traffic_ipv6_mc_clear_sock_info(int  socket)
{
	struct SockInfo  info;

	memset(&info,0,sizeof(struct SockInfo));

	if(0 == blapi_traffic_set_foe_info_to_sock(socket,&info))
		return 0;
	
	return -1;
}

int blapi_traffic_add_nptv6_prefix(unsigned char* ifname,unsigned char* snpt_str,unsigned char* dnpt_str)
{
	if(ECNT_PPEMGR_SUCCESS == ppemgr_lib_add_nptv6_prefix(ifname,snpt_str,dnpt_str))
		return 0;
	else
		return -1;
}

int blapi_traffic_del_nptv6_prefix(unsigned char* ifname)
{
	if(ECNT_PPEMGR_SUCCESS == ppemgr_lib_del_nptv6_prefix(ifname))
		return 0;
	else
		return -1;
}

int blapi_traffic_del_nptv6_prefix_all(void)
{
	if(ECNT_PPEMGR_SUCCESS == ppemgr_lib_del_nptv6_prefix_all())
		return 0;
	else
		return -1;
}

int blapi_traffic_set_wan_mtu(int wan_idx, int wan_mtu)
{
	if(ECNT_PPEMGR_SUCCESS == ppemgr_lib_wan_mtu(wan_idx,wan_mtu))
		return 0;
	else
		return -1;
}

int blapi_traffic_set_vip_by_sip_and_sport(unsigned int sip, unsigned short sport)
{
	#if defined(TCSUPPORT_IFC_EN)
	if(0 == ifc_lib_set_vip_by_sip_and_sport(sip,sport))
		return 0;
	#endif

	return -1;
}

int blapi_traffic_del_vip_by_sip_and_sport(unsigned int sip, unsigned short sport)
{
	#if defined(TCSUPPORT_IFC_EN)
	if(0 == ifc_lib_del_vip_by_sip_and_sport(sip,sport))
		return 0;
	#endif

	return -1;
}

int blapi_traffic_set_vip_by_dip_and_dport(unsigned int dip, unsigned short dport)
{
	#if defined(TCSUPPORT_IFC_EN)
	if(0 == ifc_lib_set_vip_by_dip_and_dport(dip,dport))
		return 0;
	#endif

	return -1;
}

int blapi_traffic_del_vip_by_dip_and_dport(unsigned int dip, unsigned short dport)
{
	#if defined(TCSUPPORT_IFC_EN)
	if(0 == ifc_lib_del_vip_by_dip_and_dport(dip,dport))
		return 0;
	#endif

	return -1;
}


int blapi_traffic_set_vip_by_ip_protocol(unsigned short ip_protocol)
{
#if !defined(TCSUPPORT_IFC_EN)
	/* When Firewall on, add ICMP to VIP, because ONU needs to ping WAN server suceessfully under LAND attack. */
	system("/userfs/bin/fe add ip 1");
#endif

	return 0;
}

int blapi_traffic_del_vip_by_ip_protocol(unsigned short ip_protocol)
{
#if !defined(TCSUPPORT_IFC_EN)
	/* When Firewall off, delete ICMP from VIP, because it was been added when Firewall on. */
	system("/userfs/bin/fe del ip 1");
#endif

	return 0;
}


int blapi_traffic_update_ifc_vip_params(int opt, ifc_param_t *ifc_data)
{
#if defined(TCSUPPORT_IFC_EN)
	char cmd[64] = {0};
	char s_option[4] = {0};

	if ( NULL == ifc_data )
		return -1;
	
	if ( IFC_ADD == opt )
	{
		strcpy(s_option, "add");
	}
	else if ( IFC_DEL == opt )
	{
		strcpy(s_option, "del");
	}
	else
		return -1;

	switch ( ifc_data->ifc_type )
	{
		case SIP_SPORT_CHG:
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/userfs/bin/ifc %s vip sip_sport %x %d", s_option, ifc_data->sip, ifc_data->sport);
			system(cmd);
			break;
		}
		case DIP_DPORT_CHG:
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/userfs/bin/ifc %s vip dip_dport %x %d", s_option, ifc_data->dip, ifc_data->dport);
			system(cmd);
			break;
		}
		default:
		{
			break;
		}
	}
#endif
	return 0;
}


int blapi_traffic_set_qdma_wan_rxratelimit(int ringidx, unsigned int rate)
{
	char cmd[128] = {0};

	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "/userfs/bin/qdmamgr_wan set rxratelimit value %d %u", ringidx, rate);
	system(cmd);
	
	return 0;
}

int blapi_traffic_set_sla_cfg(unsigned int direction, unsigned int sla_index, unsigned int enable ,unsigned int chnl_index)
{
	ECNT_QDMAMGR_QDMA_SEL g_qdma_sel;

	if(direction)
		g_qdma_sel = ECNT_QDMAMGR_QDMA_SEL_WAN;
	else 
		g_qdma_sel = ECNT_QDMAMGR_QDMA_SEL_LAN;

	
	if(ECNT_QDMAMGR_SUCCESS == qdmamgr_lib_set_sla_channel_cfg(g_qdma_sel, sla_index, enable, chnl_index))
		return 0;
	else
		return -1;

}

int blapi_traffic_set_qos_config_NULLQOS(char queue_mask)
{	
	if(0 == qosmgr_lib_set_qos_config_NULLQOS(queue_mask))
		return 0;
	else
		return -1;

}

int blapi_traffic_set_assure_max_bandwidth(unsigned int direction,unsigned int sla_index,unsigned int queue_index, unsigned int cbs_value, unsigned int pbs_value)
{

	ECNT_QDMAMGR_TRTCM_MODE_VALUE cfg = {0};

	
	ECNT_QDMAMGR_QDMA_SEL g_qdma_sel;
	
	if(direction)
		g_qdma_sel = ECNT_QDMAMGR_QDMA_SEL_WAN;
	else 
		g_qdma_sel = ECNT_QDMAMGR_QDMA_SEL_LAN;

	cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_SLA_TRTCM;

	cfg.index = ((sla_index<<3) | queue_index);
	
	cfg.cbs = cbs_value;

	cfg.pbs = pbs_value;
	

	if(ECNT_QDMAMGR_SUCCESS == qdmamgr_lib_set_ratectl_trtcm_mode_value(g_qdma_sel, &cfg))
		return 0;
	else
		return -1;
 
}

int blapi_traffic_get_etherwan_port(int *port)
{
	FILE *fp = NULL;
	char str_port[4] = {0};
	fp = fopen(ETHER_WAN_PATH, "r");
	if( fp == NULL )
		return -1;

	fgets(str_port, sizeof(str_port), fp);
	fclose(fp);
	if( '\0' == str_port[0] || 0 == strcmp(str_port, "-1") )
		return -1;

	*port = atoi(str_port);
	return 0;
}



int ioctl_OB_info(int cmd, blapi_info_ioctl_OB_map_data *data)
{
	int fd = -1;

	if ( NULL == data )
		return -1;

	fd = open("/dev/vpn_info", O_RDWR);
	if (fd < 0)
	{
		printf("%s:%d Open /dev/vpn_info fail.\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (0 != ioctl(fd, cmd, data))
	{
		printf("%s:%d Info ioctl fail %d.\n", __FUNCTION__, __LINE__, cmd);
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}


int blapi_traffic_ob_info_restart(blapi_info_ioctl_OB_map_data* data)
{	
	if ( NULL == data )
		return -1;

	return ioctl_OB_info(CMD_OB_INFO_RESTART, data);
}


int blapi_traffic_ob_info_get_cnt(blapi_info_ioctl_OB_map_data* data)
{	
	if ( NULL == data )
		return -1;

	return ioctl_OB_info(CMD_OB_INFO_GET_CNT, data);
}

int blapi_traffic_ob_info_get_data(blapi_info_ioctl_OB_map_data* data)
{	
	if ( NULL == data )
		return -1;

	return ioctl_OB_info(CMD_OB_INFO_GET_DATA, data);
}

int blapi_traffic_get_wancnt_flag(int *flag)
{
#if defined(TCSUPPORT_CPU_EN7580)
	*flag = 1;
#else
	*flag = 0;
#endif

	return 0;
}


int blapi_traffic_get_hsgmii_lan_rx_statistics(ECNT_TRAFFIC_WAN_RX_STATISTICS* p_stat, int hsgmii_index)
{
	ECNT_FEMGR_GDMA2_RX_STATISTICS stat;

	memset(&stat,0,sizeof(ECNT_FEMGR_GDMA2_RX_STATISTICS));
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_hsgmii_rx_statistics(&stat, hsgmii_index))
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

int blapi_traffic_get_hsgmii_lan_tx_statistics(ECNT_TRAFFIC_WAN_TX_STATISTICS* p_stat, int hsgmii_index)
{
	ECNT_FEMGR_GDMA2_TX_STATISTICS stat;

	memset(&stat,0,sizeof(ECNT_FEMGR_GDMA2_TX_STATISTICS));
	if(ECNT_FEMGR_SUCCESS == fe_lib_get_hsgmii_tx_statistics(&stat,hsgmii_index))
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

int blapi_traffic_mdio_read(unsigned int phyaddr, unsigned int devaddr, unsigned int reg, unsigned int *p_value)
{
	if(ECNT_SWITCHMGR_SUCCESS == switchmgr_lib_get_emii(phyaddr, devaddr, reg, p_value))
		return 0;
	else
		return -1;
}

int blapi_traffic_mdio_write(unsigned int phyaddr, unsigned int devaddr, unsigned int reg, unsigned int value)
{
	if(ECNT_SWITCHMGR_SUCCESS == switchmgr_lib_set_emii(phyaddr, devaddr, reg, value))
		return 0;
	else
		return -1;
}

int XMCS_VLAN_IOCTL_SDI(unsigned int cmd, unsigned long arg)
{
	int fd = -1;
	
	fd = open(FH_VLAN_FILTER_DEV_FILE, O_RDONLY);
	if (fd < 0)
	{
		printf("Couldn't open %s, fd = %d.\n", "/dev/fhvlanfilter", fd);
		return -1;
	}

	if (ioctl(fd, cmd, arg) < 0) 
	{
		printf("do XMCS_VLAN_IOCTL_SDI failed.Reason is %s\n",strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);
	return 0; 
}

int blapi_traffic_add_vlan_filter_vlan(unsigned int vlan, unsigned int port)
{
	int ret = -1;
	vlanfilter_args_t  local_VlanFilterParams;

	if(vlan > 4094)
		return -1;
	memset(&local_VlanFilterParams, 0, sizeof(vlanfilter_args_t));
	local_VlanFilterParams.vid = vlan;
	local_VlanFilterParams.port = port;
	
    ret = XMCS_VLAN_IOCTL_SDI(VLAN_FILTER_LIST_ADD, (unsigned long)&local_VlanFilterParams);
	if(ret < 0)
    {
    	printf("add vlan filter fail\n");
		local_VlanFilterParams.result = OP_FAIL;
	}

	printf("add vlan result %d\n",local_VlanFilterParams.result);
	return local_VlanFilterParams.result;
}

int blapi_traffic_del_vlan_filter_vlan(unsigned int vlan, unsigned int port)
{
	int ret = -1;
	vlanfilter_args_t  local_VlanFilterParams ;

	if(vlan > 4094)
		return -1;

	memset(&local_VlanFilterParams, 0, sizeof(vlanfilter_args_t));
	local_VlanFilterParams.vid = vlan;
	local_VlanFilterParams.port = port;
	
	ret = XMCS_VLAN_IOCTL_SDI(VLAN_FILTER_LIST_DELETE, (unsigned long)&local_VlanFilterParams);
	if(ret < 0)
    {
    	printf("del vlan filter fail\n");
		local_VlanFilterParams.result = OP_FAIL;
	}

	printf("del vlan result %d\n",local_VlanFilterParams.result);
	return local_VlanFilterParams.result;
}

int blapi_traffic_set_hsgmii_port_ratelimit(unsigned int direction, unsigned int ratelimit)
{
	int ret = -1;
	ECNT_QDMAMGR_QDMA_SEL g_qdma_sel;
	ECNT_QDMAMGR_RATELIMIT_MODE_CFG ratelimit_cfg = {0};
	ECNT_QDMAMGR_RATELIMIT_MODE_VALUE ratelimit_value = {0};
	
	if(direction){
		if(ECNT_FEMGR_SUCCESS == fe_lib_set_hsgmii_rx_port_ratelimit(4, ratelimit, 0))
			ret = 0;
		else
			ret = -1;		
		return ret;
	}
	else 
		g_qdma_sel = ECNT_QDMAMGR_QDMA_SEL_LAN;

	ratelimit_cfg.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
	ratelimit_cfg.enable = 1;
	ratelimit_cfg.rate_mode = ECNT_QDMAMGR_RATECTL_MODE_BYTE;
	ratelimit_cfg.tick_type = ECNT_QDMAMGR_TICK_TYPE_FAST;
	ratelimit_cfg.index = 13;

	ratelimit_value.module = ECNT_QDMAMGR_RATECTL_MODULE_EGRESS_TRTCM;
	ratelimit_value.index = 13;
	ratelimit_value.rate = ratelimit;
	

	if(ECNT_QDMAMGR_SUCCESS == qdmamgr_lib_set_ratectl_ratelimit_mode_cfg(g_qdma_sel, &ratelimit_cfg))
		ret = 0;
	else{
		ret = -1;
		return ret;
	}
	if(ECNT_QDMAMGR_SUCCESS == qdmamgr_lib_set_ratectl_ratelimit_mode_value(g_qdma_sel, &ratelimit_value))
		ret = 0;
	else{
		ret = -1;
		return ret;
	}

	return ret;
}

static inline char* skip_whitespace(char *s)
{
	while (*s == ' ' || *s == '\t') ++s;

	return s;
}

static char *get_name(char *name, char *p)
{
	char *nameend;
	char *namestart = skip_whitespace(p);

	nameend = namestart;
	while (*nameend && *nameend != ':' && !isspace(*nameend))
		nameend++;
	if (*nameend == ':') {
		if ((nameend - namestart) < 16) {
			memcpy(name, namestart, nameend - namestart);
			name[nameend - namestart] = '\0';
			p = nameend;
		} else {
			/* Interface name too large */
			name[0] = '\0';
		}
	} else {
		/* trailing ':' not found - return empty */
		name[0] = '\0';
	}
	return p + 1;
}

static int is_dev_ip_addr_match(char *wan_name, unsigned int ipl)
{
	struct ifreq ifr;
	int skfd;
	struct sockaddr_in *p_saddr = NULL;
	int ret = 0;
	
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd < 0)
		return 0;
	strncpy(ifr.ifr_name, wan_name, 16);
	ifr.ifr_name[15]='\0';
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0) {
		p_saddr = (struct sockaddr_in *)&ifr.ifr_addr;
		if(p_saddr->sin_addr.s_addr == ipl){
			ret = 1;
		}
	}

	close(skfd);
	return ret;
}

static int get_dev_by_ip(char *wan_dev, unsigned int ipl)
{
	FILE *fh;
	char buf[512];
	int err = -1;

	fh = fopen("/proc/net/dev", "r");
	if (!fh) {
		return -1;
	}
	fgets(buf, sizeof buf, fh);	/* eat line */
	fgets(buf, sizeof buf, fh);

	while (fgets(buf, sizeof buf, fh)) {
		char *s, name[128];
		s = get_name(name, buf);
		if(is_dev_ip_addr_match(name, ipl)){
			strncpy(wan_dev, name, 16);
			wan_dev[16]='\0';
			err = 0;
			break;
		}
	}

	fclose(fh);
	return err;
}

int blapi_traffic_speedtest_setting(int lport, int rport, unsigned int ipl, unsigned int ipr)
{
	static int cport[8] = {0};
	int i = 0;
	char cmd[128] = {0};
	char wandev[32];
	
	for(i=0;i<8;i++){
		if(lport == cport[i])
			return 1;
	}
	for(i=0;i<8;i++){
		if(cport[i] == 0){
			cport[i] = lport;
			break;
		}
	}

	if(0 == get_dev_by_ip(wandev, ipl))
	{
		memset(cmd,0,sizeof(cmd));
		sprintf(cmd, "echo wandev=%s > /proc/tc3162/speed_test", wandev);
		system(cmd);
	}

	system("echo ip > /proc/tc3162/speed_test");

	ipr = htonl(ipr);
	ipl = htonl(ipl);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo gateway=%d.%d.%d.%d > /proc/tc3162/speed_test",(ipr>>24), (ipr>>16)&0xFF, (ipr>>8)&0xFF, ipr&0xFF);
	system(cmd);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo mask=255.255.255.255 > /proc/tc3162/speed_test");
	system(cmd);

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo destip=%d.%d.%d.%d > /proc/tc3162/speed_test",(ipr>>24), (ipr>>16)&0xFF, (ipr>>8)&0xFF, ipr&0xFF);
	system(cmd);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo localip=%d.%d.%d.%d > /proc/tc3162/speed_test",(ipl>>24), (ipl>>16)&0xFF, (ipl>>8)&0xFF, ipl&0xFF);
	system(cmd);

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo destport=%d > /proc/tc3162/speed_test",rport);
	system(cmd);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo localport=%d > /proc/tc3162/speed_test",lport);
	system(cmd);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"echo action=APPTEST_P > /proc/tc3162/speed_test");
	system(cmd);	
	
	return 0;	
}
