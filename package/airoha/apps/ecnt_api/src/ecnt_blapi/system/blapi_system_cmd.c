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
* @file  blapi_system_cmd.c  
* @brief This program gives cmd apis from blapi_system.
* @author Zhen.Xu
* @date     2020-07-06
* @version  A001 
* @copyright EcoNet Inc                                                              
*/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <ecnt_lib_dbg.h>
#include "blapi_system.h"
#include "blapi_system_cmd.h"
#include "libapi_lib_utility.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define SNLEN 18
#define HTTP "http://"
#define HTTPS "https://"

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
static const cmds_t blapi_system_cmd[] = 
{
	{"get_EtherAddr", do_blapi_system_get_EtherAddr,0,0,NULL},
	{"get_Value_from_mtd0",do_blapi_system_get_Value_from_file,0,0,NULL},
	{"get_sn_offset_L", do_blapi_system_get_sn_offset_L,0,0,NULL},
	{"get_tcboot_size", do_blapi_system_get_tcboot_size,0,0,NULL},
	{"check_WifiDevice", do_blapi_system_check_WifiDevice,0,0,NULL},
	{"get_UpLinkValue", do_blapi_system_get_UpLinkValue,0,0,NULL},
	{"get_cpu_default_affinity", do_blapi_system_get_cpu_default_affinity,0,0,NULL},
	{"get_max_receive_file_size", do_blapi_system_get_max_receive_file_size,0,0,NULL},
	{"get_clearHWNAT_str", do_blapi_system_get_clearHWNAT_str,0,0,NULL},
	{"get_mtdsize", do_blapi_system_get_mtdsize,0,0,NULL},
	{"get_serdes_sel", do_blapi_system_get_serdes_sel,0,0,NULL},
	{"set_serdes_sel", do_blapi_system_set_serdes_sel,0,0,NULL},
	{"set_serdes_sel_v2", do_blapi_system_set_serdes_sel_v2,0,0,NULL},
	{"set_bootloader", do_blapi_system_set_bootloader,0,0,NULL},
	{"set_lan_led", do_blapi_system_set_lan_led,0,0,NULL},
	{"set_boot_flag", do_blapi_system_set_boot_flag,0,0,NULL},
	{"vpn_free_ip_hashmap", do_blapi_system_vpn_free_ip_hashmap,0,0,NULL},
	{"set_urlfilter_enable", do_blapi_system_set_urlfilter_enable,0,0,NULL},
	{"set_urlfilter_mode", do_blapi_system_set_urlfilter_mode,0,0,NULL},
	{"set_urlfilter_url", do_blapi_system_set_urlfilter_url,0,0,NULL},
	{"create_mac_conf", do_blapi_system_create_mac_conf,0,0,NULL},
	{"get_eth0_mac", do_blapi_system_get_eth0_mac,0,0,NULL},
	{"get_temp", do_blapi_system_get_temperature, 0, 0, NULL},
	{"check_secure_tclinux", do_blapi_system_check_secure_tclinux, 0, 0, NULL},
	{"get_err_cnt", do_blapi_system_get_err_cnt, 0, 0, NULL},
	{"client_command",  do_blapi_system_client_command,0,0,NULL},
	{"prepare_interface", do_blapi_system_prepare_interface,0,0,NULL},
	{"del_prepared_interface", do_blapi_system_del_prepared_interface,0,0,NULL},
	{"set_wan_mode", do_blapi_system_set_wan_mode,0,0,NULL},
	{"get_spf_linkup",do_blapi_system_get_spf_linkup,0,0,NULL},
	{"get_crc",do_blapi_system_get_crc,0,0,NULL},
	{NULL,NULL,0,0,NULL}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int do_blapi_system_check_secure_tclinux(int argc, char *argv[], void *p)
{
	char *filepath = NULL, result[10] = {0};
	int ret = -1, offset = 0, fip_header_length = 0;

	if(argc != 3)
	{
		ecnt_blapi_printf("Invalid parameters! \r\n"
					"Usage: blapi_cmd system check_secure_tclinux [offset] [path]\r\n");

		return ret;
	}

	offset = atoi(argv[1]);
	filepath = argv[2];
	fip_header_length = -1;

	if (blapi_system_secure_upgrade (256, offset, filepath, 4) == 0)
	{
		ret = 0;

		fip_header_length = blapi_system_secure_trx (256, offset, filepath, 4);
	}

	if (fip_header_length != -1)
	{
		sprintf (result, "success");
	}
	else
	{
		sprintf (result, "fail");		
	}

	/* Do not modify message format. Shell get fip header length by this message. */
	printf ("Secure tclinux check %s. FIP header length is %d \n", result, fip_header_length);

	/* Cannot use return to reply fip header length since the range of $? at shell is 0 ~ 255. */
	return ret;
}

int do_blapi_system_get_temperature(int argc, char *argv[], void *p)
{
	int ret = 0;
	int temp_x10 = 0;
	ret = blapi_system_get_temperature(&temp_x10);
  	if(temp_x10 < -9)
        	ecnt_blapi_printf("The temperature of the thermal sensor is %d.%01d C\n", temp_x10/10, -1*temp_x10%10);
  	else if(temp_x10 < 0)
        	ecnt_blapi_printf("The temperature of the thermal sensor is -%d.%01d C\n", temp_x10/10, -1*temp_x10%10);
  	else
		ecnt_blapi_printf("The temperature of the thermal sensor is %d.%01d C\n", temp_x10/10, temp_x10%10);
	return 0;	
}

int do_blapi_system_get_EtherAddr(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned char macaddr[6] = {0};
	
	ret = blapi_system_get_EtherAddr(macaddr);
	
	ecnt_blapi_printf("\r\n[%s]%d: EtherAddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
		__FUNCTION__, __LINE__, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	return 0;
}

int do_blapi_system_get_Value_from_file(int argc, char *argv[], void *p)
{
	int ret = 0;
	FILE *fp = NULL;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd system get_Value_from_mtd0 VendorName/ProductName/MacAddr/snmpObjID/countryCode\n");
		return -1;
	}
	
	fp = fopen("/dev/mtd0", "r+b");
	if( fp == NULL )
	{
		ecnt_blapi_printf("open file error!!\n");
		return -1;
	}
	
	if(strcmp(argv[1], "VendorName") == 0)
	{
		ret = blapi_system_show_Vendor_Name(fp);
	}
	else if(strcmp(argv[1], "ProductName") == 0)
	{
		ret = blapi_system_show_Product_Name(fp);
	}
	else if(strcmp(argv[1], "MacAddr") == 0)
	{
		ret = blapi_system_show_Mac_Addr(fp);
	}
	else if(strcmp(argv[1], "snmpObjID") == 0)
	{
		ret = blapi_system_show_snmpObjID(fp);
	}
	else if(strcmp(argv[1], "countryCode") == 0)
	{
		ret = blapi_system_show_country_Code(fp);
	}
	else
	{
		ecnt_blapi_printf("\r\nblapi_cmd system get_Value_from_mtd0 VendorName/ProductName/MacAddr/snmpObjID/countryCode\n");
	}

	fclose(fp);

	return 0;
}

int do_blapi_system_get_sn_offset_L(int argc, char *argv[], void *p)
{
	int ret = 0;
	long addr = 0;
	
	ret = blapi_system_get_sn_offset_L(&addr);
	
	ecnt_blapi_printf("\r\n[%s]%d: sn_offset_L = 0x%08x\n", __FUNCTION__, __LINE__, addr);
	return 0;
}

int do_blapi_system_get_tcboot_size(int argc, char *argv[], void *p)
{
	int ret = 0;
	int addr = 0;
	
	ret = blapi_system_get_tcboot_size(&addr);
	
	ecnt_blapi_printf("\r\n[%s]%d: tcboot_size = 0x%x\n", __FUNCTION__, __LINE__, addr);
	return 0;
}

int do_blapi_system_check_WifiDevice(int argc, char *argv[], void *p)
{
	int ret = 0;
	int action = 0;
	
	ret = blapi_system_check_WifiDevice(&action);
	
	ecnt_blapi_printf("\r\n[%s]%d: action = %d\n", __FUNCTION__, __LINE__, action);
	return 0;
}

int do_blapi_system_get_UpLinkValue(int argc, char *argv[], void *p)
{
	int ret = 0;
	int action = 0;
	char tempValue[32] = {0};
	
	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd system get_UpLinkValue 0(linkmode) 1(linkstate)\n");
		return -1;
	}

	ret = blapi_system_get_UpLinkValue(argv[1], argv[2], tempValue, sizeof(tempValue));
	
	ecnt_blapi_printf("\r\n[%s]%d: UpLinkValue = %s\n", __FUNCTION__, __LINE__, tempValue);
	return 0;
}

int do_blapi_system_get_cpu_default_affinity(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long val = 0;
	
	ret = blapi_system_get_cpu_default_affinity(&val);
	
	ecnt_blapi_printf("\r\n[%s]%d: cpu_default_affinity = 0x%x\n", __FUNCTION__, __LINE__, val);
	return 0;
}

int do_blapi_system_get_max_receive_file_size(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned long val = 0;
	
	ret = blapi_system_get_max_receive_file_size(&val);
	
	ecnt_blapi_printf("\r\n[%s]%d: max_receive_file_size = 0x%x\n", __FUNCTION__, __LINE__, val);
	return 0;
}

int do_blapi_system_get_clearHWNAT_str(int argc, char *argv[], void *p)
{
	int ret = 0;
	char buf[128] = {0};
	
	ret = blapi_system_get_clearHWNAT_str(buf, sizeof(buf));
	
	ecnt_blapi_printf("\r\n[%s]%d: clearHWNAT_str = %s\n", __FUNCTION__, __LINE__, buf);
	return 0;
}

int do_blapi_system_get_mtdsize(int argc, char *argv[], void *p)
{
	int ret = 0;
	int mtd_size = 0;
	int mtd_erasesize = 0;
	
	ret = blapi_system_get_mtdsize(argv[1], &mtd_size, &mtd_erasesize);
	
	ecnt_blapi_printf("\r\n[%s]%d: mtd_size = %d,mtd_erasesize=%d\n", __FUNCTION__, __LINE__, mtd_size, mtd_erasesize);
	return 0;
}

int do_blapi_system_get_serdes_sel(int argc, char *argv[], void *p)
{
	int ret = 0;
	int sel = 0;

    if (argc != 2) {
		ecnt_blapi_printf("\r\nblapi_cmd system get_serdes_sel 0(serdes_id)\n");
		return -1;
    }
    sel = atoi(argv[1]);
	ret = blapi_system_get_serdes_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)sel);

    if(ret < 0) {
		printf("\r\n[%s]%d:get serdes[%d] error.\n", __FUNCTION__, __LINE__, sel);
    } else {
		printf("\r\n[%s]%d:get serdes[%d]=%d.\n", __FUNCTION__, __LINE__, sel, ret);
    }

	return 0;
}

int do_blapi_system_set_serdes_sel(int argc, char *argv[], void *p)
{
	int ret = 0;
    int sel = 0, boot_flag = 0, is_hsgmii = 0;

    if (argc != 4) {
		ecnt_blapi_printf("\r\nblapi_cmd system set_serdes_sel 0(serdes_id) 0(boot_flag) 0(is_hsgmii)\n");
		return -1;
    }
    sel = atoi(argv[1]);
    boot_flag = atoi(argv[2]);
    is_hsgmii = atoi(argv[3]);
	ret = blapi_system_set_serdes_sel(sel, boot_flag, is_hsgmii);
    
    if(ret < 0) {
		ecnt_blapi_printf("\r\n[%s]%d:set serdes[%d] error.\n", __FUNCTION__, __LINE__, sel);
    } else {
		ecnt_blapi_printf("\r\n[%s]%d:set serdes[%d]=%d success.\n", __FUNCTION__, __LINE__, sel, is_hsgmii);
    }

    return 0;
}

int do_blapi_system_set_serdes_sel_v2(int argc, char *argv[], void *p)
{
	int ret = 0;
    int sel = 0, boot_flag = 0, serdes_if = 0;
	serdesCfgIoctl_t serdes;

    if (argc != 4 && argc != 5) {
		ecnt_blapi_printf("\r\nblapi_cmd system set_serdes_sel_v2 0(serdes_id) 0(boot_flag) 0(serdes_if) 0[ether type]\n");
		return -1;
    }

	serdes.ether_type = ECNT_EVENT_SERDES_ETHER_TYPE_NONE;
	
    serdes.port_idx= atoi(argv[1]);
    boot_flag = atoi(argv[2]);
    serdes.serdes_if = atoi(argv[3]);
	serdes.ether_type = atoi(argv[4]);
	ret = blapi_system_set_serdes_sel_v2(&serdes, serdes_if);

    if(ret < 0) {
		ecnt_blapi_printf("\r\n[%s]%d:set serdes_v2[%d] error.\n", __FUNCTION__, __LINE__, sel);
    } else {
		ecnt_blapi_printf("\r\n[%s]%d:set serdes_v2[%d]=%d success.\n", __FUNCTION__, __LINE__, sel, serdes_if);
    }

    return 0;
}

int do_blapi_system_set_bootloader(int argc, char *argv[], void *p)
{
    unsigned char mac_addr[6] = {0};
	unsigned char mode_name[32]={0};
	int modellen = 0, ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd system set_bootloader mac modename\n");
		return -1;
	}
	    
    /* Modify the MAC address in the bootloader */  
	sscanf(argv[1], "%02x:%02x:%02x:%02x:%02x:%02x",
		&mac_addr[0], &mac_addr[1], &mac_addr[2], &mac_addr[3], &mac_addr[4], &mac_addr[5]);
	
	memcpy(mode_name, argv[2], sizeof(mode_name));
	
	ret = blapi_system_setbootloader(mac_addr, mode_name);  

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d:set bootloader fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d:set bootloader sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_system_set_lan_led(int argc, char *argv[], void *p)
{
	int ret = 0;

	
	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd system set_lan_led lanport ledstate\n");
		return -1;
	}

	ret = blapi_system_set_lan_led(atoi(argv[1]), atoi(argv[2]));
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set lan led fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set lan led sucess\n", __FUNCTION__, __LINE__);
	
	return 0;
}

int do_blapi_system_set_boot_flag(int argc, char *argv[], void *p)
{
	int ret = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd system set_boot_flag tclinux_slave/tclinux\n");
		return -1;
	}
	
	ret = blapi_system_set_boot_flag(argv[1]);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set boot flag fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set boot flag sucess\n", __FUNCTION__, __LINE__);
	
	return 0;
}

int do_blapi_system_vpn_free_ip_hashmap(int argc, char *argv[], void *p)
{
	int ret = 0;
	int index = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n blapi_cmd system vpn_free_ip_hashmap index\n");
		return -1;
	}

#if !defined(TCSUPPORT_CT_JOYME2)
	ecnt_blapi_printf("\r\n[%s] [%d]: TCSUPPORT_CT_JOYME2 not define,do nothing\n",__FUNCTION__, __LINE__);
	return 0;
#endif

	index = atoi(argv[1]);
	ret = blapi_system_vpn_free_ip_hashmap(index);
	
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: vpn free ip hashmap fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: vpn free ip hashmap success\n",__FUNCTION__, __LINE__);
	}
	
	return 0;
}

int do_blapi_system_set_urlfilter_enable(int argc, char *argv[], void *p)
{	
	int ret = 0;
	unsigned char enable = 0;
	if(argc < 2){
		goto ErrorHandle;
	}
	else{
		enable = atoi(argv[1]);
		if(0 != enable && 1 != enable){
			goto ErrorHandle;
		}
	}
	
	ret = blapi_system_url_filter_set_enable((void *)&enable, sizeof(enable));
	ecnt_blapi_printf("\r\nset_urlfilter_enable [%d] %s======>\n", enable, ret ? "fail" : "success");
	return 0;

ErrorHandle:
	ecnt_blapi_printf("Invalid parameters! \r\n"
	  			"Usage: blapi_cmd system set_urlfilter_enable {0 or 1} \r\n"
	   			"Description: [1:enable] [0:disable]\r\n");
	return -1;
}


int do_blapi_system_set_urlfilter_mode(int argc, char *argv[], void *p)
{	
	int ret = 0;
	unsigned char mode = 0;
	
	if(argc < 2){
		goto ErrorHandle;
	}
	else{
		mode = atoi(argv[1]);
		if(0 != mode && 1 != mode){
			goto ErrorHandle;
		}
	}
	
	ret =blapi_system_url_filter_set_mode((void *)&mode, sizeof(mode));
	ecnt_blapi_printf("\r\nset_urlfilter_mode [%d] %s======>\n", mode, ret ? "fail" : "success");
	return 0;
	
ErrorHandle:
	ecnt_blapi_printf("Invalid parameters! \r\n"
	  			"Usage: blapi_cmd system set_urlfilter_mode {0 or 1} \r\n"
	   			"Description: [0:blacklist] [1:whitelist]\r\n");
	return -1;
}

int do_blapi_system_set_urlfilter_url(int argc, char *argv[], void *p)
{	
	int i = 0;
	int len = 0;
	int ret = 0;
	int index = 0;
	url_info_cfg urlInfo;
	char* url = NULL;
	char host[64] = {0};

	if(argc < 3){
		goto ErrorHandle;
	}
	else{
		index = atoi(argv[1]);
		url = argv[2];
		if(0 > index || index > 100 || strlen(url) > 48){
			goto ErrorHandle;
		}
	}

	if(strstr(url, HTTP)){
		url = url + strlen(HTTP);
	}
	else if(strstr(url, HTTPS)){
		url = url + strlen(HTTPS);
	}

	len = strlen(url);
	for(i=0; i<len; i++){
		if(url[i] == '/')
		{
			break;
		}
		snprintf(host+strlen(host), sizeof(host)-strlen(host), "%c", url[i]);
	}
	if(host[0] != '\0'){
		memset(&urlInfo, 0, sizeof(urlInfo));
		urlInfo.index = index;
		strncpy(urlInfo.host, host, sizeof(urlInfo.host) - 1);
		ret = blapi_system_url_filter_set_url_host(&urlInfo);
		ecnt_blapi_printf("\r\nset_url_host [%d] [%s] %s======>\n", index, urlInfo.host, ret ? "fail" : "success");
	}

	if(i<len && url[i] == '/')
	{
		memset(&urlInfo, 0, sizeof(urlInfo));
		urlInfo.index =index;
		strncpy(urlInfo.path, &url[i], sizeof(urlInfo.path) - 1);
		ret = blapi_system_url_filter_set_url_path(&urlInfo);
		ecnt_blapi_printf("\r\nset_url_path [%d] [%s] %s======>\n", index, urlInfo.path, ret ? "fail" : "success");
	}
	
	return 0;
	
ErrorHandle:
	ecnt_blapi_printf("Invalid parameters! \r\n"
	  			"Usage: blapi_cmd system set_urlfilter_url {index} {url} \r\n"
	   			"Description: [index:0-100] [url:such as www.163.com or www.163.com/newcar,max lenth is 48]\r\n");
	return -1;
}

int do_blapi_system_create_mac_conf(int argc, char *argv[], void *p)
{
	int ret = 0;

	ret = blapi_system_create_mac_conf();

	return ret;
}

int do_blapi_system_get_eth0_mac(int argc, char *argv[], void *p)
{
	char mac[32] = {0};
	int ret = 0;

	ret = blapi_system_get_eth0_mac(mac);
	if( 0 == ret )
		ecnt_blapi_printf("\r\n mac=[%s]\n", mac);
	else
		ecnt_blapi_printf("\r\n get mac fail\n");

	return 0;
}

int do_blapi_system_get_spf_linkup(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int upflag = -1;
	
	blapi_system_get_spf_linkup(&upflag);
	if(upflag == -1)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get_spf_linkup fail\n",__FUNCTION__, __LINE__);
		return -1;
	}
	
	ecnt_blapi_printf("\r\n[%s] [%d]: spf_linkup = [%d]\n",__FUNCTION__, __LINE__, upflag);
	return 0;
#else
	return -1;
#endif
}

int do_blapi_system_get_crc(int argc, char *argv[], void *p)
{
	unsigned int crc32 = 0;
	char buf[32] = {0};
	char *file = NULL, *temp = NULL;
	
	if(argc < 1)
	{
		ecnt_blapi_printf("\r\n Usage: blapi_cmd system get_crc /tmp/tclinux.bin\n");
		return -1;
	}
	
	file = argv[1];
	temp = argv[2];
	blapi_system_get_crc(file, &crc32);
	crc32 = htonl(crc32);
	if(temp != NULL)
	{	
		snprintf(buf, sizeof(buf), "echo %08x > %s", crc32, temp);
		system(buf);
	}

	ecnt_blapi_printf("crc32 = [0x%x]\n", crc32);
	return 0;
}

int do_blapi_system_client_command(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    char * cmd=NULL;
	cmd = argv[1];
	blapi_system_client_command(cmd);

	ecnt_blapi_printf("\r\n[%s] [%d]: exec cmd = [%s]\n",__FUNCTION__, __LINE__, cmd);

	return 0;
#else
	return -1;
#endif
}

int do_blapi_system_prepare_interface(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	blapi_intf_obj_t obj = {0};

	if(argc < 10)
	{
		ecnt_blapi_printf("\r\n blapi_cmd system system_prepare_interface wanidx vid mvid used_vlan transMode linkMode version vtag dot1p\n");
		return -1;
	}

	obj.idx = atoi(argv[1]);
	obj.vid = atoi(argv[2]);
	obj.mvid = atoi(argv[3]);
	obj.used_vlan = atoi(argv[4]);
	strncpy(obj.transMode, argv[5], sizeof(obj.transMode) - 1);
	strncpy(obj.linkMode, argv[6], sizeof(obj.linkMode) - 1);
	obj.version = atoi(argv[7]);
	obj.vtag = atoi(argv[8]);
	obj.dot1p = atoi(argv[9]);

	ret = blapi_system_prepare_interface(&obj);

	return 0;
#else
	return -1;
#endif
}

int do_blapi_system_del_prepared_interface(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	blapi_intf_obj_t obj = {0};

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\n blapi_cmd system system_del_prepared_interface wanidx transMode\n");
		return -1;
	}

	obj.idx = atoi(argv[1]);
	strncpy(obj.transMode, argv[2], sizeof(obj.transMode) - 1);
	
	ret = blapi_system_del_prepared_interface(&obj);

	return 0;
#else
	return -1;
#endif
}

int do_blapi_system_set_wan_mode(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	int wan = 0;
	if(argc < 2){
		return -1;
	}

	wan = atoi(argv[1]);
	
	ret = blapi_system_set_wan_mode(wan);

	return 0;
#else
	return -1;
#endif
}

int do_blapi_system_get_err_cnt(int argc, char *argv[], void *p)
{
	unsigned char error_flag = 0;
	uint64_t error_count = 0;
	int ret = 0;

	ret = blapi_system_get_err_cnt(&error_flag, &error_count);
	if( 0 == ret )
		ecnt_blapi_printf("\r\n error_flag=[%d], error_count=[%llu]\n", error_flag, error_count);
	else
		ecnt_blapi_printf("\r\n get err cnt fail\n");

	return 0;
}

int do_blapi_system_cmd(int argc,char* argv[],void *p)
{
    return subcmd(blapi_system_cmd,argc,argv,(void *)blapi_system_cmd);
}

