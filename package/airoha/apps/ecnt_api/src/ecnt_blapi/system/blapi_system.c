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
* @file  blapi_system.c  
* @brief This file gives examples on using apis from blapi_system.h.
* @author Zhen.Xu
* @date     2019-11-26
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
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <spi/secure_upgrade.h>
#include <fcntl.h>
#include "blapi_system.h"
#include "blapi_system_bsp.h"
#include "libapi_lib_switchmgr.h"
#include "libapi_lib_fe.h"
#include "ecnt_lib_dbg.h"
#include "trx.h"
#include "libapi_lib_utility.h"
#if defined(TCSUPPORT_CT_JOYME2)
#include "vpn_info/info_ioctl.h"
#endif
#include "url_filter_global_def.h"
#include "modules/app_route/app_route_global_def.h"
#include "flash_layout/tc_partition.h"
#include "flash_layout/prolinecmd.h"
#include "ecnt_event_global/ecnt_event_serdes.h"

#if defined(TCSUPPORT_SISM)
#include "modules/service_manager/wan_mode_sel.h"
#include "lib_pt.h"
#endif /*defined(TCSUPPORT_SISM)*/

#ifndef MTD_OLD
# include <mtd/mtd-user.h>
#endif

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

//#if defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_MT7570)
    #define BOBLEN 225
    #define BOB_CONF  "/tmp/7570_bob.conf"
#else
    #define BOBLEN 1024
    #define BOB_CONF  "/tmp/etc/lddla/bob.conf"
#endif
//#endif

#ifdef TCSUPPORT_OPENWRT
#define 	MTD_CMD		 "/sbin/ecnt_mtd"
#else
#define 	MTD_CMD		 "/userfs/bin/mtd"
#endif
#define 	BOOT_NAME 	 "bootloader"

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
#define LIB_BLAPI_ERROR(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_ERROR, ECNT_BLAPI_SYSTEM_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_WARN(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_WARN, ECNT_BLAPI_SYSTEM_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_NOTICE(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_NOTICE, ECNT_BLAPI_SYSTEM_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_DEBUG(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_DEBUG, ECNT_BLAPI_SYSTEM_DBUG_ENABLE, fmt,##__VA_ARGS__)

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

#define UG_TCLINUX				4

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int __ioctl_data(int cmd, char *dev, void *data);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static serdesCfgIoctl_t serdes;
#ifdef TCSUPPORT_BOARD_SELECT
static boardCfgIoctl_t board;
#endif

typedef struct env_info
{
	env_index index;
	char* envname;
}env_info_t;
	

env_info_t envmsg[] = 
{
	{ARCH_ENV,              "arch"},
	{BAUDRATE_ENV,          "baudrate"},
	{BOARD_ENV,             "board"},
	{BOARD_NAME_ENV,        "board_name"},
	{BOOTCMD_ENV,           "bootcmd"},
	{BOOTDELAY_ENV,         "bootdelay"},
	{BOOTFILE_ENV,          "bootfile"},
	{CONSOLE_ENV,           "console"},
	{COUNTRY_CODE_ENV,      "country_code"},
	{CPU_ENV,               "cpu"},
	{DRAM_LIMIT_ENV,        "dram_limit"},
	{DSL_GPIO_ENV,          "dsl_gpio"},
	{ETHACT_ENV,            "ethact"},
	{ETHADDR_ENV,           "ethaddr"},
	{ETHER_GPIO_ENV,        "ether_gpio"},
	{FDT_HIGH_ENV,          "fdt_high"},
	{GPIO_ACTIVE_HIGH_ENV,  "gpio_active_high"},
	{INTERNET_GPIO_ENV,     "internet_gpio"},
	{INVALID_ENV,           "invaild_env"},
	{IPADDR_ENV,            "ipaddr"},
	{KERNEL_FILENAME_ENV,   "kernel_filename"},
	{LOADADDR_ENV,          "loadaddr"},
	{MULTI_UPGRADE_GPIO_ENV,"multi_upgrade_gpio"},
	{ONU_TYPE_ENV,          "onu_type"},
	{PASSWORD_ENV,          "password"},
	{POWER_GPIO_ENV,        "power_gpio"},
	{PRODUCT_NAME_ENV,      "product_name"},
	{QDMA_INIT_ENV,         "qdma_init"},
	{ROOT_ENV,              "root"},
	{SDRAM_CONF_ENV,        "sdram_conf"},
	{SERDES_SEL_ENV,        "serdes_sel"},
	{SERVERIP_ENV,          "serverip"},
	{SNMP_SYSOBJID_ENV,     "snmp_sysobjid"},
	{SOC_ENV,               "soc"},
	{STDERR_ENV,            "stderr"},
	{STDIN_ENV,             "stdin"},
	{STDOUT_ENV,            "stdout"},
	{UBOOT_FILENAME_ENV,    "uboot_filename"},
	{USERNAME_ENV,          "username"},
	{VENDOR_ENV,            "vendor"},
	{VENDOR_NAME_ENV,       "vendor_name"},
	{HYBRID_ENV,            "hybrid"},
        {ARHT_LOWPOWER_ENV,     "arht_low_power"},
};


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

int blapi_system_get_envmsg(env_index index, char** envvalue)
{
	char envname[64] = {0};
	char* str = NULL;
	
	if(index <= 0 || index >= MAX_ENV_NUM)
	{
		return -1;
	}

	strncpy(envname, envmsg[index].envname, sizeof(envname) - 1);
	fw_getenv(envname,&str);
	if(str == NULL)
	{
		ecnt_blapi_printf("\r\nfw_getenv failed ! \n");
		return -1;
	}
	else
	{
		*envvalue = str;
	}

	return 0;
}

int blapi_system_get_OnuType_offset_L(long *addr)
{
#ifdef TCSUPPORT_BB_256KB
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
	*addr = 0x0003fe9cL;
#else
	*addr = 0x0000ff9cL;
#endif
	return 0;
}

int blapi_system_get_EtherAddr_offset(int *addr)
{
#ifdef TCSUPPORT_BB_256KB
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
	*addr = 0x0003fe48;
#else
	*addr = 0x0000ff48;
#endif
	return 0;
}

int blapi_system_get_EtherAddr_offset_L(long *addr)
{
#ifdef TCSUPPORT_BB_256KB
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
	*addr = 0x0003fe48L;
#else
	*addr = 0x0000ff48L;
#endif
	return 0;
}


int blapi_system_get_EtherAddr_range_offset_L(long *addr)
{
#ifdef TCSUPPORT_BB_256KB
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
    * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
    */
	*addr = 0x0003feb5L;
#else
	*addr = 0x0000ffb5L;
#endif
	return 0;
}

int blapi_system_get_product_vendor_name_offset_L(long *productAddr, long* vendorAddr)
{
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
#ifdef TCSUPPORT_BB_256KB
	*productAddr = 0x0003fe24L;
	*vendorAddr = 0x0003fe04L;
#else
	*productAddr = 0x0000ff24L;
	*vendorAddr = 0x0000ff04L;
#endif
	return 0;
}

int blapi_system_get_ether_snmp_countryCode_offset_L(long *etherAddr, long* snmpAddr, long* countryCodeAddr)
{
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
#ifdef TCSUPPORT_BB_256KB
	*etherAddr = 0x0003fe48L;
	*snmpAddr = 0x0003fe4eL;
	*countryCodeAddr = 0x0003fe64L;
#else
	*etherAddr = 0x0000ff48L;
	*snmpAddr = 0x0000ff4eL;
	*countryCodeAddr = 0x0000ff64L;
#endif
	return 0;
}

int blapi_system_get_sn_offset_L(long *addr)
{
#ifdef TCSUPPORT_BB_256KB
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
	*addr = 0x0003fe9d;
#else
	*addr = 0x0000ff9d;
#endif

	return 0;
}

int blapi_system_write_bob_to_flash(void)
{	
#ifdef TCSUPPORT_NAND_FLASH	
	FILE *fp = NULL; 
	char buf[128] = {0};
	char	*pStart = NULL;
	char strBuf[8]={0};
	int pageSize=0;
	size_t len = 0;
	
	fp = fopen("/proc/driver/nand", "r");
	if(fp==NULL)
	{
		printf("open file error!!\n");
		return -1;
	}

	while ( -1 != getline(&buf, &len, fp) ) 
	{
		if((pStart=strstr(buf,"page size: ")) != NULL)
		{
			pStart += strlen("page size: ");
			break;
		}
		memset(buf, 0x00, sizeof(buf));
	}	
	fclose(fp);

	if(pStart == NULL)
	{
		printf("doSysBobCarlibration fail!!\n");
		return -1;
	}
	sscanf(pStart, "%s", strBuf);
	pageSize = atoi(strBuf); 

	memset(buf, 0x00, sizeof(buf));
#if ((TCSUPPORT_RESERVEAREA_BLOCK==5) || (TCSUPPORT_RESERVEAREA_BLOCK==7 && (defined(MT7615E)|| defined(MT7615_11N))))
	snprintf(buf, sizeof(buf), "/userfs/bin/mtd -f writeflash /tmp/gpon_bob.conf 1280 %lu reservearea", (unsigned long)GPON_BOB_OFFSET);
#else
	snprintf(buf, sizeof(buf), "/userfs/bin/mtd -f writeflash /tmp/gpon_bob.conf 1280 %d reservearea", (pageSize<<6));
#endif
	system(buf);
#else
	system("/userfs/bin/mtd -f writeflash /tmp/gpon_bob.conf 1280 65536 reservearea");
#endif

	return 0;
}

int blapi_system_get_tcboot_size(int *size)
{
	FILE *fp = NULL;
	char buf[256] = {0};
	char Info[3][24] ={0};
	if(fp = fopen("/proc/mtd", "r"))
	{
		while(fgets(buf, 255, fp))
		{
			if(strstr(buf ,"bootloader"))
			{
				memset(Info,0,sizeof(Info));
				sscanf(buf,"%s %x %s %s", Info[0], size, Info[1], Info[2]);
				break;
			}
		}
		fclose(fp);
	}

	return 0;
}

int blapi_system_check_WifiDevice(int *action)
{
	FILE *fp = NULL;
	char buf[1024] = {0};

	*action = 1;
	system("ifconfig ra0 > /tmp/check_wifidevice.txt");
	fp = fopen("/tmp/check_wifidevice.txt", "r");
	if( fp == NULL )
	{
		unlink("/tmp/check_wifidevice.txt");        
    	return -1;
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		fgets(buf, 1023, fp);
		if(strstr(buf, "HWaddr") == NULL){
			*action = 0;
		}

	}
	fclose(fp);
	unlink("/tmp/check_wifidevice.txt"); 
#if defined(TCSUPPORT_WLAN_AC)
	system("ifconfig rai0 > /tmp/check_wifidevice.txt");
	fp = fopen("/tmp/check_wifidevice.txt", "r");
	if( fp == NULL )
	{
		unlink("/tmp/check_wifidevice.txt");       
    	return -1;
	}
	else
	{
		memset(buf, 0, sizeof(buf));
		fgets(buf, 1023, fp);
		if(strstr(buf, "HWaddr") == NULL){
			*action = 0;
		}

	}
	fclose(fp);
	unlink("/tmp/check_wifidevice.txt"); 
#endif

	return 0;
}

int blapi_system_get_UpLinkValue(char *Mode, char *LinkSta, char *tempValue, int len)
{
	int uplinkFlag =  0 ;

#if defined(TCSUPPORT_CPU_MT7510)
		snprintf(tempValue, len, "%d", 3);	/*VDSL*/
#else
		snprintf(tempValue, len, "%s", Mode);
#if defined(TCSUPPORT_CMCCV2)
		switch ( atoi(Mode) )
		{
			case 1:
				snprintf(tempValue, len, "%s", "GPON"); /*GPON*/
				break;
			case 2:
				snprintf(tempValue, len, "%s", "EPON"); /*EPON*/
				break;
			case 3: 
			case 4:
			case 5:
				snprintf(tempValue, len, "%s", "GE"); /*GE*/
				break;
			case 6:
				snprintf(tempValue, len, "%s", "XG-PON"); /*XG-PON*/
				break;
			case 7:
				snprintf(tempValue, len, "%s", "XGS-PON"); /*XGS-PON*/
				break;
			default:
				break;
		}
#else
#if defined(TCSUPPORT_CPU_MT7520)
#if defined(TCSUPPORT_CPU_EN7580)
		switch(atoi(Mode))
		{
			case 1:
				uplinkFlag = 5;/*GPON*/
				break;
			case 3: 
				uplinkFlag = 6;/*10G-EPON*/
				break;
			case 4:
				uplinkFlag = 8;/*10G-EPON*/
				break;
			case 5:
				uplinkFlag = 4;/*EPON*/
				break;
			case 6:
				uplinkFlag = 7;/*XGPON*/
				break;
			case 7:
				uplinkFlag = 7;/*XGPON*/
				break;
			default:
				break;
		}
		snprintf(tempValue, len, "%d", uplinkFlag);  
#else
		if(strcmp(LinkSta, "1") == 0)
			snprintf(tempValue, len, "%d", 5);	/*GPON*/
		else
			snprintf(tempValue, len, "%d", 4);	/*EPON*/		
#endif
#else
		snprintf(tempValue,len, "%d", 1); /*ADSL*/	
#endif
#endif
#endif

	return 0;
}

int blapi_system_get_cpu_default_affinity(unsigned long *val){
#if (VOIP_MAX_CHAN<8) && (defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7580))
	*val= 0x8;
#else
	*val= 0x2;
#endif
	return 0;
}

int blapi_system_get_max_receive_file_size(unsigned long *val) {
#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
	*val = 0xfa0000; 					/*16MB*/
#else
#if defined(TCSUPPORT_CPU_MT7510) || defined(TCSUPPORT_CPU_MT7520)
	*val = 0xe00000; 				        /*14MB*/
#else
	*val = 0x800000; 				        /*5.5MB*/
#endif
#endif
	return 0;
}

int blapi_system_bad_block_rate(void){
	char buf[20]= {0};
    FILE *fp;
	int rate = 0;

    fp = fopen("/proc/driver/spi_nand_bad_block_rate", "r");

    if (!fp)
    {
        return 0;
    }

	memset(buf, 0, sizeof(buf));
	if (NULL == fgets(buf, sizeof(buf), fp)){
		fclose(fp);
		return 0;
	}

	sscanf(buf, "%d\n", &rate);

    fclose(fp);

	LIB_BLAPI_DEBUG("return bad block rate:%d\n", rate);
    return rate;
}

int blapi_system_set_lan_led(int lanPort, int led_st){
	char buf[20]= {0};
    FILE *xpon_lan_led_proc;

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CUC_C5_SFU)
    FILE *gse_link_sta_proc;

    gse_link_sta_proc = fopen("/proc/tc3162/gsw_link_st", "r"); 
    memset(buf, 0, sizeof(buf));
    fgets(buf, sizeof(buf), gse_link_sta_proc);
    fclose(gse_link_sta_proc);
#endif/*TCSUPPORT_COMPILE*/

    xpon_lan_led_proc = fopen("/proc/tc3162/led_752x", "w+");

    if (!xpon_lan_led_proc)
    {
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d %d", lanPort, led_st);
    fwrite(buf, 1, strlen(buf), xpon_lan_led_proc);
    fclose(xpon_lan_led_proc);

    return 0;
}

int blapi_system_secure_trx(int length, int offset, char *file_path, unsigned char firmware_type)
{
	int fd = 0;
	int ret = 0;
	struct secure_upgrade_ioctl_cmd cmd;

	LIB_BLAPI_DEBUG("enter=====>\n");
#if defined(TCSUPPORT_SECURE_BOOT) ||  defined(TCSUPPORT_ARM_SECURE_BOOT)
	if(firmware_type == UG_TCLINUX)
	{

		fd = open("/dev/secure_upgrade", O_RDWR);
		if (fd == -1) 
		{
			LIB_BLAPI_ERROR("open file /dev/secure_upgrade fail");
			return -1;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.length = length;
		cmd.offset = offset;
		strncpy(cmd.path,file_path, sizeof(cmd.path) - 1);

		LIB_BLAPI_DEBUG("app SECURE_IOC_GET = %u !\n",SECURE_IOC_GET);
		ret = ioctl(fd, SECURE_IOC_GET, &cmd);
		if (ret == -1)
		{
			close(fd);
			LIB_BLAPI_ERROR("secure ioctl error !\n");
			return 0;
		}
		LIB_BLAPI_DEBUG("secure ioctl success !\n");
		close(fd);
		ret = cmd.offset;
	}
#endif

	LIB_BLAPI_DEBUG("leave, return offset=====>\n");

	return ret;
}

int blapi_system_secure_upgrade(int length, int offset, char *file_path, unsigned char firmware_type)
{
	int fd = 0;
	int ret = 0;
	struct secure_upgrade_ioctl_cmd cmd;
	
	LIB_BLAPI_DEBUG("enter=====>\n");
#if defined(TCSUPPORT_SECURE_BOOT) ||  defined(TCSUPPORT_ARM_SECURE_BOOT)
	if(firmware_type == UG_TCLINUX)
	{

		fd = open("/dev/secure_upgrade", O_RDWR);
		if (fd == -1) 
		{
			LIB_BLAPI_ERROR("open file /dev/secure_upgrade fail");
			return -1;
		}

		memset(&cmd, 0, sizeof(cmd));
		cmd.length = length;
		cmd.offset = offset;
		strncpy(cmd.path,file_path, sizeof(cmd.path) - 1);

		LIB_BLAPI_DEBUG("app SECURE_IOC_SET = %u !\n",SECURE_IOC_SET);
		ret = ioctl(fd, SECURE_IOC_SET, &cmd);
		if (ret == -1)
		{
			close(fd);
			LIB_BLAPI_ERROR("secure ioctl error !\n");
			return -1;
		}
		LIB_BLAPI_DEBUG("secure ioctl success !\n");
		close(fd);
	}
#endif

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");

	return 0;
}

int blapi_system_doSysBobCarlibration(){

	char buf[128]={0};
	
#if defined(TCSUPPORT_BOSA_DATA_TO_FLASH)	
		printf("start to do BOB carlibration, please wait!\n");
		system("echo b >/proc/tc3162/gpon_phy_test");
		blapi_system_write_bob_to_flash();
		return 0;
#else
#ifdef TCSUPPORT_CT
#if (TCSUPPORT_RESERVEAREA_BLOCK >=5)
		printf("start to do BOB carlibration, please wait!\n");
		memset(buf, 0x00, sizeof(buf));
		sprintf(buf, "/userfs/bin/mtd -f writeflash /tmp/7570_bob.conf %lu %lu reservearea", 
			(unsigned long)BOB_RA_SIZE, (unsigned long)BOB_RA_OFFSET);
		printf("%s",buf);
		system(buf);
#endif
#else	
		printf("Carlibrate area not support!\n");
#endif
#endif
	
	return 0;
}


int blapi_system_get_clearHWNAT_str(char *buf, int size){

	strncpy(buf, "/userfs/bin/hw_nat -!\n", size-1); 
	
	return 0;
}


static const unsigned int crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define UPDC32(octet,crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

int blapi_system_calculate_crc32(int imagefd, int len, unsigned int* pcrc)
{
	long length=len;	
	int check_len=0, i=0, count = 0;
	char flag=0;/*for stop loop*/
	unsigned char *buf;

	unsigned int crc = 0xFFFFFFFF;
#define CHECKSUM_TEMP_BUF_SIZE 4096
	
	/*Because read 1 byte at a time will spent much time, 
		we read a bigger size at a time and use this space to 
		do checksum. */
   	buf = (unsigned char *)malloc(CHECKSUM_TEMP_BUF_SIZE);
	if(buf == NULL)
		 return -1;

 	while(flag == 0)
 	{
		/*decide add length*/
		if(length <= CHECKSUM_TEMP_BUF_SIZE)
		{
			check_len=length;
			flag = 1;	
		}
		else
		{
			check_len = CHECKSUM_TEMP_BUF_SIZE;
		}

		length -= check_len;
		count = read(imagefd, buf, check_len); 
		

		for(i=0;i < check_len;i++)
		{
			crc = UPDC32(*(buf+i), crc);	
		}  
	}  	

    free(buf);

	*pcrc = crc;
    /*tcdbg_printf(" crc %x\n",crc);*/
    return 0;
}


#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
uint32_t blapi_system_compute_crc32buf(char *buf, size_t len)
{
	uint32_t crc;

	crc = 0xFFFFFFFF;

	for ( ; len; --len, ++buf)
	{
		crc = UPDC32(*buf, crc);
	}

	return (uint32_t)crc;
}
#endif

int blapi_system_checkimageCrc(int img, int *checksum)
{
#if defined(TCSUPPORT_CT_UPG_PINGPONG) || defined(TCSUPPORT_NOR_FLASH_USED) || defined(CONFIG_DUAL_IMAGE)
	int fd = -1;
	unsigned long read_conut = 0, count = 0, buflen = 0;
	char img_buf[sizeof(struct trx_header)];
	struct trx_header *trx = (struct trx_header *) img_buf;
	unsigned int ctc32 = 0;
	int ret = -1;

	if ( 0 == img )
		fd = mtd_open("tclinux", O_RDONLY | O_SYNC);
	else
		fd = mtd_open("tclinux_slave", O_RDONLY | O_SYNC);
	
	if(fd < 0) 
	{
		printf("\nerror: Could not open device, image[%d]\n", img);
		return -1;
	}
	lseek(fd,0, SEEK_SET);

	buflen = read(fd, img_buf, sizeof(struct trx_header));
	if (buflen < sizeof(struct trx_header)) 
	{
		close(fd);
		printf("\nerror: Could not get image header, file too small (%lu bytes), image[%d]\n", buflen, img);
		return -1;
	}
	/*magic check*/
	if ((trx->magic != TRX_MAGIC2) || (trx->len < sizeof(struct trx_header)))
	{
		close(fd);
		printf("\nerror: Bad trx header, image[%d]\n", img);
		return -1;
	}
	/*crc check*/
	ret = blapi_system_calculate_crc32(fd, (trx->len-sizeof(struct trx_header)), &ctc32);
	if(trx->crc32 != ctc32)
	{
		close(fd);
		printf("\nerror: crc32 check fail, checksum error image[%d]\n", img);
		return -1;
	}
	*checksum = trx->crc32;

	close(fd);
#endif
	return 0;
}

int blapi_system_set_boot_flag(char *mtd)
{
	char cmds[256] = {0};
	FILE *fp=NULL;
	char boot_flag = -1;
	
	if(NULL == mtd)
	{
		printf("%s: invalid parameter.\n",__FUNCTION__);
		return -1;
	}

	if(0 == strcmp(mtd,"tclinux_slave"))
	{
		boot_flag = '1';
	}
	else if(0 == strcmp(mtd,"tclinux"))
	{
		boot_flag = '0';
	}
	else
	{
		printf("%s: invalid mtd:%s\n",__FUNCTION__,mtd);
		return -1;
	}
	
	fp = fopen(DUAL_IMG_BOOT_FLAG_FILE, "w");
	if (fp == NULL)
	{
		return -1;
	}
	
	if (1 != fwrite(&boot_flag, 1, 1, fp))
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);

#ifdef TCSUPPORT_OPENWRT
	snprintf(cmds, sizeof(cmds), "ecnt_sys set bootflag %s", DUAL_IMG_BOOT_FLAG_FILE);
#else
	snprintf(cmds, sizeof(cmds), TC_FLASH_WRITE_CMD, DUAL_IMG_BOOT_FLAG_FILE, 
		(unsigned long)IMG_BOOT_FLAG_SIZE, (unsigned long)IMG_BOOT_FLAG_OFFSET, 
		RESERVEAREA_NAME);
#endif

	system(cmds);

	return 0;
}

int blapi_system_get_mtdsize(char *mtd, int *mtd_size, int *mtd_erasesize)
{
	struct mtd_info_user mtdInfo;
	int fd = 0;

	/* check if image(or romfile) fits to mtd device */
	fd = mtd_open(mtd, O_RDWR | O_SYNC);
	if(fd < 0) 
	{
		printf("Could not open mtd device: %s\n", mtd);
		return -1;
	}

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) 
	{
		close(fd);
		printf("Could not get MTD device info from %s\n", mtd);
		return -1;
	}

	close(fd);

	if(mtd_size){
	*mtd_size = mtdInfo.size;
	}

	if(mtd_erasesize){
		*mtd_erasesize = mtdInfo.erasesize;
	}

	return 0;
}


int QdmaInit(FILE *fp,unsigned char mask,unsigned char value)
{
	unsigned char flashQdmaInit = 0;

	if (NULL == fp) {
		return -1;
	}

#ifdef TCSUPPORT_BB_256KB
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
	if( fseek(fp, 0x0003feb4L, SEEK_SET) < 0 ){
		return -1;
	}
#else
	if( fseek(fp, 0x0000ffb4L, SEEK_SET) < 0 ){
		return -1;
	}
#endif

	if( 1 != fread(&flashQdmaInit, sizeof(flashQdmaInit), 1, fp) ){
		return -1;
	}
	/* [1:0], 0x0--qdma_lan payload size 2k, 0x1--qdma_lan 1k, 0x2--qdma_lan 512, 0x3--qdma_lan 256
	** [2], reserve
	** [3], 0x0--qdma_lan dscp in DRAM, 0x1--qdma_lan dscp in SRAM
	** [5:4], 0x0--qdma_wan payload size 2k, 0x1--qdma_wan 1k, 0x2--qdma_wan 512, 0x3--qdma_wan 256
	** [6], reserve
	** [7], 0x0--qdma_wan dscp in DRAM, 0x1--qdma_wan dscp in SRAM */
	flashQdmaInit = (flashQdmaInit & (~mask)) | value; 
	
#ifdef TCSUPPORT_BB_256KB
	/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
	 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
	 */
	if( fseek(fp, 0x0003feb4L, SEEK_SET) < 0 ){
		return -1;
	}
#else
	if( fseek(fp, 0x0000ffb4L, SEEK_SET) < 0 ){
		return -1;
	}
#endif
	fwrite(&flashQdmaInit, sizeof(flashQdmaInit), 1, fp );

	return 0;
}


int blapi_system_doRomReset(int rebootFlag)
{
	if(rebootFlag == 1){ 
		system("/userfs/bin/mtd -r write /userfs/romfile.cfg romfile");
	}
	else if(rebootFlag == 0){
		system("/userfs/bin/mtd write /userfs/romfile.cfg romfile");
	}
	else{
		return -1;
	}
	
	return 0;
}

int blapi_system_do_Sys_Atspv_show()
{
    FILE *fp; 
	long productAddr = 0, vendorAddr = 0;
	char ProductName[PRODUCTNAMELEN]={0};
	char VendorName[VENDORNAMELEN]= {0};
	int ret = -1;
#if defined(TCSUPPORT_CPU_EN7523)	
	char *str = NULL;
	
	if(blapi_system_get_envmsg(PRODUCT_NAME_ENV, &str) != 0)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else
		strncpy(ProductName, str, sizeof(ProductName) - 1);

	str = NULL;
	if(blapi_system_get_envmsg(VENDOR_NAME_ENV, &str) != 0)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else
		strncpy(VendorName, str, sizeof(VendorName) - 1);

#else
	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");
	
	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");	  
		return -1;
	}
	
	blapi_system_get_product_vendor_name_offset_L(&productAddr, &vendorAddr);
	ret = fseek(fp, productAddr, SEEK_SET);
	ret = fread(ProductName,PRODUCTNAMELEN-1, 1, fp );
	ProductName[PRODUCTNAMELEN - 1] = '\0';

	ret = fseek(fp, vendorAddr, SEEK_SET);
	ret = fread(VendorName, VENDORNAMELEN-1, 1, fp );
	VendorName[VENDORNAMELEN - 1] = '\0';
	
	fclose(fp);
#endif	

	printf("ProductName: %s\r\n",ProductName);
	printf("VendorName: %s\r\n",VendorName);
	
	return 0;
}

int blapi_system_do_Sys_Atspv_write(char *ProductName, char *VendorName)
{
    FILE *fp; 
	long productAddr = 0, vendorAddr = 0;
	int ret = -1;
#if defined(TCSUPPORT_CPU_EN7523)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;

	if( !ProductName || !VendorName)
		return -1;

	argv_env[0] = "fw_setenv";
    argv_env[1] = "product_name";
    snprintf(argv_env[2], 32, "%s", ProductName);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

	argv_env[1] = "vendor_name";
	snprintf(argv_env[2], 32, "%s", VendorName);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

#else

	if( !ProductName || !VendorName)
		return -1;

	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");

	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");    
		return -1;
	}
    
	blapi_system_get_product_vendor_name_offset_L(&productAddr, &vendorAddr);
	ret = fseek(fp, productAddr, SEEK_SET);
	fwrite(ProductName, PRODUCTNAMELEN, 1, fp );
    
	ret = fseek(fp, vendorAddr, SEEK_SET);
	fwrite(VendorName, VENDORNAMELEN, 1, fp );
    
	fflush(fp);
	fclose(fp);
  
	/* Write the bootloader back to flash and reboot*/
	system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");
#endif	

	return 0;
}

int blapi_system_doSysPONMode(unsigned char ponMode, unsigned char wanRamMode, unsigned char wanPayloadMode, int noReboot)
{
	char onuType = 0;
	unsigned int temp = 0;
    FILE *fp = NULL; 
	size_t bytes_read;
	unsigned char qdmaValue =0, qdmaMask = 0;
	long addr = 0;

#if defined(TCSUPPORT_CPU_EN7581)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;
	char *str = NULL;	
	int ret;

	if(blapi_system_get_envmsg(ONU_TYPE_ENV, &str) != 0)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else{
		ret = sscanf(str, "%02x", &temp);
		if(ret != 1) {
			fprintf(stderr, "parse %s error:%d\n", str, ret);
			return -1;
		}
	}
	onuType = (char)temp;
	ponMode = (onuType & ONUTYPEMASK) | (onuType & BBF247MASK) | (onuType & COMBOPONMASK) | (ponMode << 4);
	
	argv_env[0] = "fw_setenv";
	argv_env[1] = "onu_type";
	sprintf(argv_env[2], "%02x", ponMode);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

	if(noReboot == 0){
		//system("reboot");
		sync();
		system("killall tcwdog");
		system("echo 1 > /proc/watchdog_reset");

	}
#else
	unsigned char tmp[256] = {0};

    /* Read bootloader from flash */
#ifdef TCSUPPORT_OPENWRT
	system("cat /dev/mtd0 > /tmp/boot.bin");
#else
	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");
#endif

    fp = fopen("/tmp/boot.bin", "r+b");
    if( fp == NULL ){
        printf("open file error!!\n");
        system("rm /tmp/boot.bin");        
        return -1;
    }

	blapi_system_get_OnuType_offset_L(&addr);
	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to %x fail.\n",addr);
		fclose(fp);
		return -1;
	}

	bytes_read = fread(&onuType, sizeof(onuType), 1, fp );
	if(bytes_read != sizeof(onuType)) {
		printf("/tmp/boot.bin fread fail, length=%zu.\n", bytes_read);
		fclose(fp);
		return -1;
	}
	ponMode = (onuType & ONUTYPEMASK)  | (onuType & BBF247MASK) | (onuType & COMBOPONMASK) | (ponMode << 4); 

	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to %x fail.\n",addr);
		fclose(fp);
		return -1;
	}

	fwrite(&ponMode, sizeof(ponMode), 1, fp );

	qdmaValue = (wanPayloadMode << 4) | (wanRamMode << 7);
	qdmaMask =(QDMA_MASK_WAN_PAYLOAD|QDMA_MASK_WAN_DSCP);
	if(QdmaInit(fp, qdmaMask, qdmaValue)!= 0 ){
		printf("operate qdma file error!!\n");
		fclose(fp);
		system("rm /tmp/boot.bin"); 
		return -1;
	}

    fflush(fp);
    fclose(fp);
 
    /* Write the bootloader back to flash and reboot*/
	/*system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");*/	
    memset(tmp, 0, sizeof(tmp));  
	snprintf(tmp,sizeof(tmp),"%s -rf write /tmp/boot.bin %s", MTD_CMD, BOOT_NAME);
	system(tmp);
#endif
    return 0;
}

 int blapi_system_doSysAtsn(char *buf, int size, long addr) 
{
	FILE *fp = NULL; 
	int ret = -1;

	if( !buf )
		return -1;
	
	/* Read bootloader from flash */
	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");

	/* Modify the SN value in the bootloader */	  
	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
	  printf("open file error!!\n");
	  system("rm /tmp/boot.bin");		 
	  return -1;
	}
	printf("new SN: %s\n",buf);
	ret = fseek(fp, addr, SEEK_SET);
	fwrite(buf, size, 1, fp);
	fflush(fp);
	fclose(fp);

	/* Write the bootloader back to flash and reboot*/
	system("/userfs/bin/mtd -f write /tmp/boot.bin bootloader");
	system("rm /tmp/boot.bin");	 
	return 0;
}

int blapi_system_DisMacLearning()
{
	char tmp[80] = {0};
	int  i = 0;
#ifdef TCSUPPORT_CPU_ARMV8
	unsigned int regAddr[7] = {0x1fb5a00c, 0x1fb5a10c, 0x1fb5a20c, 0x1fb5a30c, 
	0x1fb5a40c, 0x1fb5a50c, 0x1fb5a60c};
#else
	unsigned int regAddr[7] = {0xbfb5a00c, 0xbfb5a10c, 0xbfb5a20c, 0xbfb5a30c, 
	0xbfb5a40c, 0xbfb5a50c, 0xbfb5a60c};
#endif
	for (i=0; i < 7; i++){
		memset(tmp,0,80);
		snprintf(tmp,sizeof(tmp)-1,"sys memwl %x 0xfff10", regAddr[i]);
		system(tmp);
	}
	
	return 0;
}

int blapi_system_setbootloader(unsigned char *mac_addr, unsigned char *mode_name)
{
    FILE *fp = NULL; 
	long productAddr = 0, vendorAddr = 0, addr = 0;
	
#if defined(TCSUPPORT_CPU_EN7523)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;

	argv_env[0] = "fw_setenv";
    argv_env[1] = "ethaddr";
    snprintf(argv_env[2], 20, "%02x:%02x:%02x:%02x:%02x:%02x", 
		mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

	argv_env[1] = "product_name";
	memset(argv_env[2], 0, 20);
	memcpy(argv_env[2], mode_name, strlen(mode_name));

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}
#else

    /* Read bootloader from flash */
	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");
  
	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");
		system("rm /tmp/boot.bin");        
		return -1;
    }
	
	blapi_system_get_EtherAddr_offset_L(&addr);
	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to 0x0003fe48L fail.\n");
		fclose(fp);
		return -1;
	}

    fwrite(mac_addr, 6, 1, fp );
	
	blapi_system_get_product_vendor_name_offset_L(&productAddr, &vendorAddr);
	if(fseek(fp, productAddr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to 0x0003fe48L fail.\n");
		fclose(fp);
		return -1;
	}

    fwrite(mode_name, 32, 1, fp );
    fflush(fp);
    fclose(fp);
        
    /* Write the bootloader back to flash and reboot*/
    system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader"); 
#endif
	return 0;
}

int blapi_system_get_EtherAddr(unsigned char *macaddr)
{
    FILE *fp = NULL; 
	int addr = 0;
	int ret = -1;
#if defined(TCSUPPORT_CPU_EN7523)	
	char *str = NULL;
	unsigned int mac[6] = {0};

	if ( !macaddr )
	{
		return -1;
	}

	if(blapi_system_get_envmsg(ETHADDR_ENV, &str) != 0)
	{
		ecnt_blapi_printf("\r\nfw_getenv failed ! \n");
		return -1;
	}
	else
	{
		sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x", 
			&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

		macaddr[0] = mac[0];
		macaddr[1] = mac[1];
		macaddr[2] = mac[2];
		macaddr[3] = mac[3];
		macaddr[4] = mac[4];
		macaddr[5] = mac[5];		
	}
	
#else
	if ( !macaddr )
		return -1;

	system("mtd readflash /tmp/boot.bin bootloader");
	
	fp = fopen("/tmp/boot.bin", "rb");
	if( !fp )
	{
		system("rm -f /tmp/boot.bin");
		printf("Mac Addr is: no value\r\n");
		return -1;
	}
	blapi_system_get_EtherAddr_offset(&addr);
	ret = fseek(fp, addr, SEEK_SET);
	ret = fread(macaddr, 6, 1, fp);
	system("rm -f /tmp/boot.bin");
	close(fp);
#endif

	return 0;
}


int blapi_system_show_Vendor_Name(FILE *fp)
{
	long productAddr = 0, vendorAddr = 0;
	char buf[VENDORNAMELEN] = {0};
	char* vendorName = NULL;
	int ret = -1;

	if( !fp )
	{
		return -1;
	}
#if defined(TCSUPPORT_CPU_EN7523)
	if(blapi_system_get_envmsg(VENDOR_NAME_ENV, &vendorName) != 0)
	{
		ecnt_blapi_printf("\r\nblapi_system_show_Vendor_Name failed!");
		return -1;
	}
	else
	{
		ecnt_blapi_printf("Vendor Name 	: %s\n", vendorName);
	}
#else
	blapi_system_get_product_vendor_name_offset_L(&productAddr, &vendorAddr);

	memset(buf, 0, sizeof(buf));
	ret = fseek(fp, vendorAddr, SEEK_SET);/*vendor name*/
	ret = fread(buf,VENDORNAMELEN-1,1, fp);
	buf[VENDORNAMELEN - 1] = '\0';
	ecnt_blapi_printf("Vendor Name 	: %s\n", buf);

#endif
	return 0;
}

int blapi_system_show_Product_Name(FILE *fp)
{
	long productAddr = 0, vendorAddr = 0;
	char buf[PRODUCTNAMELEN] = {0};
	char* productName = NULL;
	int ret = -1;
	
	if( !fp )
	{
		return -1;
	}
#if defined(TCSUPPORT_CPU_EN7523)
	if(blapi_system_get_envmsg(PRODUCT_NAME_ENV, &productName) != 0)
	{
		ecnt_blapi_printf("\r\nblapi_system_show_Product_Name failed!");
		return -1;
	}
	else
	{
		ecnt_blapi_printf("Product Name 	: %s\n", productName);
	}
#else
	blapi_system_get_product_vendor_name_offset_L(&productAddr, &vendorAddr);

	memset(buf,0,sizeof(buf));
	ret = fseek(fp, productAddr, SEEK_SET);/*product name */
	ret = fread(buf, PRODUCTNAMELEN - 1, 1, fp);
	buf[PRODUCTNAMELEN - 1] = '\0';
	ecnt_blapi_printf("Product Name		: %s\n", buf);
#endif

	return 0;
}

int blapi_system_show_Mac_Addr(FILE *fp)
{
	char buf[64] = {0};
	long etherAddr = 0, snmpAddr = 0, countryCodeAddr = 0;
	int i = 0, ret = -1;

	if( !fp )
		return -1;

	blapi_system_get_ether_snmp_countryCode_offset_L(&etherAddr, &snmpAddr, &countryCodeAddr);

	memset(buf,0,sizeof(buf));
	ret = fseek(fp, etherAddr, SEEK_SET);/*mac addr*/
	ret = fread(buf, ETHERLEN, 1,fp);
	printf("Mac Address		: ");
	for(i=0; i<6; i++){
		printf("%02x",(unsigned char)buf[i]);
	}
	printf("\n");

	return 0;
}

int blapi_system_show_snmpObjID(FILE *fp)
{
	char buf[64] = {0};
	long etherAddr = 0, snmpAddr = 0, countryCodeAddr = 0;
	int i = 0,ret = -1;
	
	if( !fp )
		return -1;

	blapi_system_get_ether_snmp_countryCode_offset_L(&etherAddr, &snmpAddr, &countryCodeAddr);

	memset(buf,0,sizeof(buf));
	ret = fseek(fp, snmpAddr, SEEK_SET);/*snmpObjID*/
	ret = fread(buf, SNMPSYSOBJLEN,1, fp);
	printf("SNMP MIB level & OID	: ");
	for(i=0;i<21;i++){
		printf("%02x",(unsigned char)buf[i]);
	}
	printf("\n");

	return 0;
}

int blapi_system_show_country_Code(FILE *fp)
{
	char buf[64] = {0};
	long etherAddr = 0, snmpAddr = 0, countryCodeAddr = 0;
	int ret = -1;
	
	if( !fp )
		return -1;

	blapi_system_get_ether_snmp_countryCode_offset_L(&etherAddr, &snmpAddr, &countryCodeAddr);

	memset(buf,0,sizeof(buf));
	ret = fseek(fp, countryCodeAddr, SEEK_SET);/*country id*/
	ret = fread(buf, COUNTRYCODELEN, 1,fp);
	printf("Default Country Code	: %x\n",(unsigned char)buf[0]);

	return 0;
}

int blapi_system_GetBosaTemp(int *phyTemp)
{

#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
	if(-1 == blapi_pon_get_phy_trans_temperature(phyTemp))
#endif
#endif
	{
		return -1;
	}

	return 0;

}

#define STB_LIST_PATH "/proc/br_fdb_host/stb_list"
#if  defined(TCSUPPORT_CT_STBMAC_REPORT)
char stb_mac_old[256] = {0};

int blapi_system_check_stb_change(void)
{
	FILE *f = NULL;
	char tmpbuf[512] = {0};
	char statebuf[12] = {0};
	int stbcount = 0;
	char stb_mac_new[256] = {0};
	int ret = 0;
	
	f = fopen(STB_LIST_PATH, "r");
	if ( f )
	{
		memset(tmpbuf, 0 , sizeof(tmpbuf));
		fgets(tmpbuf, sizeof(tmpbuf) - 1, f);
		sscanf(tmpbuf, "%[^=]=%d=%[^=]", statebuf, &stbcount, stb_mac_new);
		fclose(f);
		
		if (0 != strcmp(stb_mac_new, stb_mac_old))
		{
			if (strlen(stb_mac_new) > 0)
			{
				memset(stb_mac_old, 0, sizeof(stb_mac_old));
				strncpy(stb_mac_old, stb_mac_new, sizeof(stb_mac_old) - 1);
				ret = 1;
			}
			
		}
	}

	return ret;
}
#endif

#if defined(TCSUPPORT_CT_PON)
int blapi_system_get_bob(char *bob, int len)
{
	FILE *fp = NULL;
	int i = 0;
	
	fp = fopen(BOB_CONF, "r");
	if(fp != NULL)
	{
		if(1 != fread(bob, len, 1, fp))
		{
			LIB_BLAPI_ERROR("\r\nclearProLinePara:read file(bob.conf) fail!");
			fclose(fp);
			return -1;			
		}
		fclose(fp);
	}
	else
	{
		LIB_BLAPI_ERROR("\r\nclearProLinePara:open file(bob.conf) fail!");
		return -1;		
	}

	for(i= 0; i< BOBLEN; i++){
		if((i&0x0f) == 0) LIB_BLAPI_DEBUG("\n%08xh:",i);
		LIB_BLAPI_DEBUG(" %02X", (unsigned char)bob[i]);			
	}

	return 0;
}

int blapi_system_set_bob(char *bob, int len)
{
	FILE *fp = NULL;

	fp =  fopen(BOB_CONF, "w+");
	if (fp == NULL) 
	{
		printf("\r\nsetProLineCwmpPara:open file(bob.conf) fail!");						
		return -1;
	}
	fwrite(bob, len, 1, fp);
	fclose(fp);


	return 0;
}
#endif

int blapi_system_cpu_remap_taskset(unsigned long *mask)
{
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
	unsigned long temp_mask = 0;
	int cpu_num = 0;

	cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
	if ( 2 == cpu_num )
	{
		if ((*mask & 0xC) != 0)
		{
			temp_mask |= 0x2;
		}

		if ((*mask & 0x3) != 0)
		{
			temp_mask |= 0x1;
		}
		*mask = temp_mask;
	}
	else if ( 1 == cpu_num )
	{
		temp_mask |= 0x1;
		*mask = temp_mask;
	}
#endif
	return 0;	
}

int blapi_system_vpn_free_ip_hashmap(int idx)
{
#if defined(TCSUPPORT_CT_JOYME2)
	int fd = -1;
	int vpn_entry_idx = -1;

	vpn_entry_idx = idx;

	fd = open("/dev/vpn_info", O_RDWR);
	if (fd < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d] Open /dev/vpn_info fail.\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (0 != ioctl(fd, CMD_VPN_CLEAR_IPS, &vpn_entry_idx))
	{
		ecnt_blapi_printf("\r\n[%s] [%d] Info ioctl fail.\n", __FUNCTION__, __LINE__);
		close(fd);
		return -1;
	}
	close(fd);
#endif

	return 0;
}

int ioctl_url_filter(int cmd, void *data, int datealen)
{
	int fp = -1;
	int ret = -1;
	
	fp = open("/dev/urlfilter", O_RDONLY);
	if(fp == -1)
	{
		LIB_BLAPI_ERROR("ioctl_urlfilter: fp is [%d]\n", fp);
		return -1;
	}
	
	ret = ioctl(fp, cmd, data);
	if(ret < 0)
	{
		LIB_BLAPI_ERROR("\r\nioctl_urlfilter failed!");
	}

	LIB_BLAPI_DEBUG("ioctl_urlfilter: fp is [%d],ret is [%d]\n", fp, ret);
	close(fp);
	return 0;
}

int blapi_system_url_filter_set_enable(void *data, int datealen)
{
	return ioctl_url_filter(URL_FILTER_IOC_SET_ENABLE, data, datealen);
}

int blapi_system_url_filter_set_mode(void *data, int datealen)
{
	return ioctl_url_filter(URL_FILTER_IOC_SET_MODE, data, datealen);
}

int blapi_system_url_filter_set_url_host(url_info_cfg* cfg_urlInfo)
{
	url_info urlInfo;

	if ( NULL == cfg_urlInfo )
		return -1;

	memset(&urlInfo, 0, sizeof(urlInfo));
	urlInfo.type = cfg_urlInfo->type;
	urlInfo.index = cfg_urlInfo->index;
	strncpy(urlInfo.host, cfg_urlInfo->host, sizeof(urlInfo.host) - 1);
	strncpy(urlInfo.path, cfg_urlInfo->path, sizeof(urlInfo.path) - 1);
	urlInfo.blockedtimes = cfg_urlInfo->blockedtimes;
	strncpy(urlInfo.MacAddr, cfg_urlInfo->MacAddr, sizeof(urlInfo.MacAddr) - 1);
	urlInfo.type = URL_TYPE_HOST;

	return ioctl_url_filter(URL_FILTER_SET_URL, (void*)&urlInfo, sizeof(url_info));
}

int blapi_system_url_filter_set_url_path(url_info_cfg* cfg_urlInfo)
{
	url_info urlInfo;

	if ( NULL == cfg_urlInfo )
		return -1;

	memset(&urlInfo, 0, sizeof(urlInfo));
	urlInfo.type = cfg_urlInfo->type;
	urlInfo.index = cfg_urlInfo->index;
	strncpy(urlInfo.host, cfg_urlInfo->host, sizeof(urlInfo.host) - 1);
	strncpy(urlInfo.path, cfg_urlInfo->path, sizeof(urlInfo.path) - 1);
	urlInfo.blockedtimes = cfg_urlInfo->blockedtimes;
	strncpy(urlInfo.MacAddr, cfg_urlInfo->MacAddr, sizeof(urlInfo.MacAddr) - 1);
	urlInfo.type = URL_TYPE_PATH;

	return ioctl_url_filter(URL_FILTER_SET_URL, (void*)&urlInfo, sizeof(url_info));
}

int blapi_system_url_filter_get_blocktimes(void *data, int datealen)
{
	return ioctl_url_filter(URL_FILTER_GET_INFO, data, datealen);
}

int blapi_system_url_filter_clear_all_rule(void)
{
	urlfilter_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.cmd_type = E_URL_REM_ALL;	

	return ioctl_url_filter(URL_FILTER_SET_HASHDATA, (void*)&data, sizeof(urlfilter_ioctl_data));
}

int blapi_system_url_filter_clear_global_setting(urlfilter_ioctl_cfg_data* cfg_data)
{
	urlfilter_ioctl_data data;
	int ret = 0;

	memset(&data, 0, sizeof(data));
	data.cmd_type = E_URL_REM_GLOBAL_ALL;
	ret = ioctl_url_filter(URL_FILTER_SET_HASHDATA, (void*)&data, sizeof(urlfilter_ioctl_data));
	return ret;
}

int blapi_system_url_filter_clear_all_basicmac_rule(urlfilter_ioctl_cfg_data* cfg_data)
{
	urlfilter_ioctl_data data;
	urlfilter_obj_data *pdest = NULL;
	urlfilter_obj_cfg_data *psrc = NULL;
	int i = 0;
	int ret = 0;

	if ( NULL == cfg_data )
		return -1;

	memset(&data, 0, sizeof(data));
	data.urlfilter_obj_cnt = cfg_data->urlfilter_obj_cnt;
	data.clearbtflag = cfg_data->clearbtflag;
	if ( data.urlfilter_obj_cnt )
	{
		data.obj = malloc(sizeof(urlfilter_obj_data) * data.urlfilter_obj_cnt);
		if ( NULL == data.obj )
			return -1;
		
		memset(data.obj, 0, sizeof(urlfilter_obj_data) * data.urlfilter_obj_cnt);
		pdest = data.obj;
		psrc = cfg_data->obj;
		for ( i = 0; i < data.urlfilter_obj_cnt; i++ )
		{
			pdest->cmd_type = psrc->cmd_type;
			pdest->index = psrc->index;
			pdest->mode = psrc->mode;
			pdest->match_all = psrc->match_all;
			pdest->action = psrc->action;
			pdest->port = psrc->port;
			pdest->times = psrc->times;
			strncpy(pdest->domain, psrc->domain, sizeof(pdest->domain) - 1);
			pdest++;
			psrc++;
		}
	}
	strncpy(data.mac_addr, cfg_data->mac_addr, sizeof(data.mac_addr) - 1);
	
	data.cmd_type = E_URL_REM_BASEMAC_ALL;
	ret = ioctl_url_filter(URL_FILTER_SET_HASHDATA, (void*)&data, sizeof(urlfilter_ioctl_data));

	if ( data.obj )
		free(data.obj);
	return ret;
}

int blapi_system_url_filter_add_url_rule(urlfilter_ioctl_cfg_data* cfg_data)
{
	urlfilter_ioctl_data data;
	urlfilter_obj_data *pdest = NULL;
	urlfilter_obj_cfg_data *psrc = NULL;
	int i = 0;
	int ret = 0;

	if ( NULL == cfg_data )
		return -1;
	
	memset(&data, 0, sizeof(data));
	data.urlfilter_obj_cnt = cfg_data->urlfilter_obj_cnt;
	data.clearbtflag = cfg_data->clearbtflag;
	if ( data.urlfilter_obj_cnt )
	{
		data.obj = malloc(sizeof(urlfilter_obj_data) * data.urlfilter_obj_cnt);
		if ( NULL == data.obj )
			return -1;
		
		memset(data.obj, 0, sizeof(urlfilter_obj_data) * data.urlfilter_obj_cnt);
		pdest = data.obj;
		psrc = cfg_data->obj;
		for ( i = 0; i < data.urlfilter_obj_cnt; i++ )
		{
			pdest->cmd_type = psrc->cmd_type;
			pdest->index = psrc->index;
			pdest->mode = psrc->mode;
			pdest->match_all = psrc->match_all;
			pdest->action = psrc->action;
			pdest->port = psrc->port;
			pdest->times = psrc->times;
			strncpy(pdest->domain, psrc->domain, sizeof(pdest->domain) - 1);
			pdest++;
			psrc++;
		}
	}
	strncpy(data.mac_addr, cfg_data->mac_addr, sizeof(data.mac_addr) - 1);
	
	data.cmd_type = E_URL_ADD;
	ret = ioctl_url_filter(URL_FILTER_SET_HASHDATA, (void*)&data, sizeof(urlfilter_ioctl_data));

	if ( data.obj )
		free(data.obj);
	return ret;
}

int blapi_system_dns_filter_clear_all_rule(void)
{
	urlfilter_ioctl_data data;
	
	memset(&data, 0, sizeof(data));
	data.cmd_type = E_DNS_REM_ALL;	

	return ioctl_url_filter(URL_FILTER_SET_HASHDATA, (void*)&data, sizeof(urlfilter_ioctl_data));
}

int blapi_system_dns_filter_set_lanip(dnsfilter_ioctl_cfg_lanip_t* cfg_data)
{
	dnsfilter_ioctl_lanip_t data;

	if ( NULL == cfg_data )
		return -1;
	
	memset(&data, 0, sizeof(data));
	strncpy(data.lanip, cfg_data->lanip, sizeof(data.lanip) - 1);
	strncpy(data.lanip6, cfg_data->lanip6, sizeof(data.lanip6) - 1);

	return ioctl_url_filter(URL_FILTER_SET_LANIP, (void*)&data, sizeof(dnsfilter_ioctl_lanip_t));
}

int blapi_system_dns_filter_add_dns_rule(urlfilter_ioctl_cfg_data* cfg_data)
{
	urlfilter_ioctl_data data;
	urlfilter_obj_data *pdest = NULL;
	urlfilter_obj_cfg_data *psrc = NULL;
	int i = 0;
	int ret = 0;

	if ( NULL == cfg_data )
		return -1;

	memset(&data, 0, sizeof(data));
	data.urlfilter_obj_cnt = cfg_data->urlfilter_obj_cnt;
	data.clearbtflag = cfg_data->clearbtflag;
	if ( data.urlfilter_obj_cnt )
	{
		data.obj = malloc(sizeof(urlfilter_obj_data) * data.urlfilter_obj_cnt);
		if ( NULL == data.obj )
			return -1;
		
		memset(data.obj, 0, sizeof(urlfilter_obj_data) * data.urlfilter_obj_cnt);
		pdest = data.obj;
		psrc = cfg_data->obj;
		for ( i = 0; i < data.urlfilter_obj_cnt; i++ )
		{
			pdest->cmd_type = psrc->cmd_type;
			pdest->index = psrc->index;
			pdest->mode = psrc->mode;
			pdest->match_all = psrc->match_all;
			pdest->action = psrc->action;
			pdest->port = psrc->port;
			pdest->times = psrc->times;
			strncpy(pdest->domain, psrc->domain, sizeof(pdest->domain) - 1);
			pdest++;
			psrc++;
		}
	}
	strncpy(data.mac_addr, cfg_data->mac_addr, sizeof(data.mac_addr) - 1);
	
	data.cmd_type = E_DNS_ADD;
	ret = ioctl_url_filter(URL_FILTER_SET_HASHDATA, (void*)&data, sizeof(urlfilter_ioctl_data));

	if ( data.obj )
		free(data.obj);
	return ret;
}

static int serdesCfgIoctl(int cmd, serdesCfgIoctl_t *data)
{
	int ret = -1;

	ret = __ioctl_data(cmd, SERDES_CFG_DEV, (void *)data);
	if(ret != 0){
		LIB_BLAPI_ERROR("[%s] IOCTL error, ret=%d!\n", __func__, ret);
	}

	return ret;
}

#ifdef TCSUPPORT_BOARD_SELECT
static int boardCfgIoctl(int cmd, boardCfgIoctl_t *data)
{
	int ret = -1;

	ret = __ioctl_data(cmd, BOARD_CFG_DEV, (void *)data);
	if(ret != 0){
		LIB_BLAPI_ERROR("[%s] IOCTL error, ret=%d!\n", __func__, ret);
	}

	return ret;
}
#endif

static int system_parseNumber(const char *cp, unsigned int base, unsigned long *parseAddr)
{
	int retVal = 0;
	char *strtolEnd = NULL;

	errno = 0;
	*parseAddr = strtoul(cp, &strtolEnd, base);

	if (strtolEnd == cp) {
		LIB_BLAPI_ERROR("%s: not a heximal number\n", cp);
		retVal = -1;
	} else if ('\0' != *strtolEnd) {
		LIB_BLAPI_ERROR("%s: extra characters at end of input: %s\n", cp, strtolEnd);
		retVal = -1;
	} else if (ULONG_MAX == *parseAddr && ERANGE == errno) {
		LIB_BLAPI_ERROR("%s out of range of type unsigned long\n", cp);
		retVal = -1;
	} else if (0 == *parseAddr && EINVAL == errno) {
		LIB_BLAPI_ERROR("%s is invalid of type unsigned long\n", cp);
		retVal = -1;
	}

	//LIB_BLAPI_DEBUG("parseAddr:0x%lu\n", *parseAddr);

	return retVal;
}

#if 0 /* Unused */
int blapi_system_get_serdes_port_cnt(void)
{
	int ret;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_PORT_CNT, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes port count error.\n");
		return 0;
	}

	return serdes.port_cnt;
}

int blapi_system_get_serdes_if_cnt(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	int ret;

	serdes.port_idx = port_idx;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_IF_CNT, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes interface count of specfic serdes port:%d error.\n", (int)port_idx);
		return 0;
	}

	return serdes.serdes_if_cnt;
}

int blapi_system_get_ether_type_cnt(void)
{
	int ret;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_ETHER_TYPE_CNT, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes ether type error.\n");
		return 0;
	}

	return serdes.ether_type_cnt;
}

int blapi_system_get_serdes_port_name(serdesCfgIoctl_t *_serdes)
{
	int ret = 0;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_PORT_NAME, _serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes port:%d name error.\n", (int)_serdes->port_idx);
	} 

	return ret;
}

int blapi_system_get_serdes_if_name(serdesCfgIoctl_t *_serdes)
{
	int ret;
	
	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_IF_NAME, _serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes port:%d interface:%d name error.\n", (int)_serdes->port_idx, (int)_serdes->serdes_if);
	} 

	return ret;
}

static int get_env_serdes_ether_type_name(serdesCfgIoctl_t *_serdes)
{
	int ret;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_ETHER_TYPE_NAME, _serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes ether type:%d name error.\n", (int)_serdes->ether_type);
	} 

	return ret;
}
#endif

int blapi_system_get_serdes_if_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	int ret;

	serdes.port_idx = port_idx;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_IF_SEL, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes interface selection of specfic serdes port:%d error.\n", (int)port_idx);
		return ret;
	}

	return serdes.serdes_if;
}
#ifdef TCSUPPORT_BOARD_SELECT
int blapi_system_get_serdes_count(void)
{
	int ret;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_PORT_CNT, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes count:%d error.\n", __func__, serdes.port_cnt);
		return ret;
	}

	return serdes.port_cnt;
}
#endif


ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t blapi_system_get_serdes_ether_type_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	int ret;

	serdes.port_idx = port_idx;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_ETHER_TYPE_SEL, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes ether type selection of specfic serdes port:%d error.\n", (int)port_idx);
		return ret;
	}

	return serdes.ether_type;
}

static int get_env_serdes_port_name(serdesCfgIoctl_t *_serdes)
{
	int ret;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_PORT_ENV_NAME, _serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes port:%d env name error.\n", (int)_serdes->port_idx);
	} 

	return ret;
}

#ifdef TCSUPPORT_BOARD_SELECT
static int get_rfb_id(boardCfgIoctl_t *_board)
{
	int ret;
	ret = boardCfgIoctl(BOARD_CFG_IOCTL_GET_RFB_NO, _board);
	if(ret < 0) {
		LIB_BLAPI_ERROR("get board id error.\n");
	} 
	return ret;
}
#endif

int blapi_system_set_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel, int rebootFlag, int isHSGMII)
{
#if defined(TCSUPPORT_CPU_EN7523)
	int argc_env = 3;
	char *argv_env[3];
	char arr[20] = {0};
	char *str = NULL;
	int ret;
	char env_val;
	int bit_idx = (int)sel;
	
#ifdef TCSUPPORT_BOARD_SELECT
	if(sel >= blapi_system_get_serdes_count()) 
#else
	if(sel >= ECNT_EVENT_SERDES_SEL_MAX)
#endif
	{
		fprintf(stderr, "error sel:%d.\n", (int)sel);
		return -1;
	}

	argv_env[0] = "fw_setenv";
	argv_env[1] = "serdes_sel";
	argv_env[2] = arr;
	
	fw_getenv(argv_env[1], &str);
	if(str == NULL) {
		fprintf(stderr, "fw_getenv %s failed ! \n", argv_env[1]);
		return -1;
	}
	
	ret = sscanf(str, "%02x", &env_val);
	if(ret != 1) {
		fprintf(stderr, "parse %s error:%d\n", str, ret);
		return -1;
	}
	
	if(isHSGMII == 0) {
		env_val &= ~(0x1 << bit_idx);
	} else {
		env_val |= (0x1 << bit_idx);
	}
	
	ret = snprintf(argv_env[2], sizeof(arr) - 1, "%02x", env_val);
	if(ret != 1 && ret != 2) {
		fprintf(stderr, "parse env_val:0x%02x error:%d\n", env_val, ret);
		return -1;
	}
	
	if(fw_setenv(argc_env, argv_env) != 0){
		fprintf(stderr, "fw_setenv %s failed ! \n", argv_env[1]);
		return -1;
	}
	
	if(rebootFlag == 1) {
	    system("reboot");
	}
	
	return 0;
#else
	return -1;
#endif
}

int blapi_system_set_serdes_sel_v2(serdesCfgIoctl_t *_serdes, int rebootFlag)
{
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_BOARD_SELECT)
	int argc_env = 3;
	char *argv_env[3];
	char arr[20] = {0};
	int ret;
	int env_val;
	int bit_idx = (int)_serdes->port_idx;
	char *str;
#ifdef TCSUPPORT_BOARD_SELECT
	char *argv_env_rfb[3];
	char arr_rfb[50] = {0};
	char *srds_param;
	int srds_param_num = 0;
	char *rfb_cfg;
	char rfb_name[25];
	int cnt = 1;
#endif

	if(_serdes->ether_type >= ECNT_EVENT_SERDES_ETHER_TYPE_MAX) {
		LIB_BLAPI_ERROR("[%s] ether_type:%d is equal or more than %d.\n", (int)ECNT_EVENT_SERDES_ETHER_TYPE_MAX);
		return -1;
	}
#ifdef TCSUPPORT_BOARD_SELECT
	ret = get_rfb_id(&board);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get rfb no error.\n", __func__);
		return ret;
	}
#else
	ret = get_env_serdes_port_name(_serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get env serdes port:%d name error.\n", (int)_serdes->port_idx);
		return ret;
	}
#endif

#ifdef TCSUPPORT_BOARD_SELECT
	snprintf(rfb_name, sizeof(rfb_name), "rfb_%d%02d", board.rfb_id.id1, board.rfb_id.id2);
	argv_env_rfb[0] = "fw_setenv";
	argv_env_rfb[1] = rfb_name;
	argv_env_rfb[2] = arr_rfb;

	fw_getenv("srdsParmCntPerPort", &srds_param);
	srds_param_num = strtol(srds_param, NULL, 10);

	if(!fw_getenv(rfb_name, &rfb_cfg)){
		fw_getenv("rfb_defult", &rfb_cfg);
		argv_env_rfb[1] = "rfb_defult";
	} else {
		fw_getenv(rfb_name, &rfb_cfg);
	}
	rfb_cfg[0+(srds_param_num+1)*(int)(_serdes->port_idx)] = (_serdes->phy_sel)+'0';
	rfb_cfg[1+(srds_param_num+1)*(int)(_serdes->port_idx)] = (_serdes->ether_type)+'0';
	rfb_cfg[2+(srds_param_num+1)*(int)(_serdes->port_idx)] = (_serdes->serdes_if)+'0';
#else
	argv_env[0] = "fw_setenv";
	argv_env[1] = _serdes->name;
	argv_env[2] = arr;
#endif

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
	env_val = ((_serdes->phy_sel & SERDES_PHY_SEL_MASK) << SERDES_PHY_SEL_OFFSET) |
			  ((_serdes->ether_type & SERDES_ETHER_TYPE_SEL_MASK) << SERDES_ETHER_TYPE_SEL_OFFSET) |
			  ((_serdes->serdes_if & SERDES_IF_SEL_MASK) << SERDES_IF_SEL_OFFSET);

#elif defined(TCSUPPORT_CPU_EN7523)
	fw_getenv(argv_env[1], &str);
	if(str == NULL) {
		LIB_BLAPI_ERROR("fw_getenv %s failed ! \n", argv_env[1]);
		return -1;
	}
	
	ret = sscanf(str, "%02x", (char *)(&env_val));
	if(ret != 1) {
		LIB_BLAPI_ERROR("parse %s error:%d\n", str, ret);
		return -1;
	}
	
	if(_serdes->serdes_if == 0) {
		env_val &= ~(0x1 << bit_idx);
	} else {
		env_val |= (0x1 << bit_idx);
	}

	if(_serdes->ether_type == ECNT_EVENT_SERDES_ETHER_TYPE_LAN - 1) {
		env_val &= ~(0x1 << (bit_idx + SERDES_ETHER_TYPE_SEL_OFFSET));
	} else {
		env_val |= (0x1 << (bit_idx + SERDES_ETHER_TYPE_SEL_OFFSET));
	}
#endif

#ifdef TCSUPPORT_BOARD_SELECT
	ret = snprintf(argv_env_rfb[2], sizeof(arr_rfb) - 1, "%s", rfb_cfg);
	if(fw_setenv(argc_env, argv_env_rfb) != 0){
		LIB_BLAPI_ERROR("fw_setenv %s failed ! \n", argv_env_rfb[1]);
		return -1;
	}
#else
	ret = snprintf(argv_env[2], sizeof(arr) - 1, "%03x", (int)env_val);

	if(ret != 1 && ret != 2 && ret != 3) {
		LIB_BLAPI_ERROR("parse env_val:0x%03x, arr:%s,  error:%d\n", (int)env_val, argv_env[2], ret);
		return -1;
	}

	if(fw_setenv(argc_env, argv_env) != 0){		
		LIB_BLAPI_ERROR("fw_setenv %s failed ! \n", argv_env[1]);
		return -1;
	}
#endif

	if(rebootFlag == 1) {
	    system("reboot");
	}

	return 0;
#else
	return -1;
#endif
}

static void showSerdesSelUsage(void)
{
	int ret;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_USAGE, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes usage error.\n");
	}
	printf("%s", serdes.name);
	fflush(stdout);
}

static void showSerdesSelStatus(void)
{
	int ret;

	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_GET_STATUS, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] get serdes usage error.\n");
	}
	printf("%s", serdes.name);

	fflush(stdout);
}

static void serdesUpdatePhyType(void)
{
	int ret = -1;
	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_SET_PHYTYPE, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] set serdes usage error.\n");
	}

	printf("serdes %d phy type updated", serdes.port_idx);
}


static void etherSerdesUpdateEthType(ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t set_ether_type)
{
	int ret = -1;
	serdes.ether_type=set_ether_type;
	ret = serdesCfgIoctl(SERDES_CFG_IOCTL_SET_ETHSEL_ETHTYPE, &serdes);
	if(ret < 0) {
		LIB_BLAPI_ERROR("[%s] set serdes usage error.\n");
	}

	printf("%s", "serdes-Ethernet etherType updated");
}
#ifdef TCSUPPORT_BOARD_SELECT
int blapi_system_show_board_info(int argc, char *argv[])
{
	int ret = 0;
	char rfb_cfg_name[25];
	char *srdsPortCnt;
	char *rfb_cfg;
	char *rfbParmCnt;
	int rfbParmCnt_num = 0;
	int cnt = 1;

	if(argc == 2) {
		ret = get_rfb_id(&board);
		if(ret != 0) {
			printf("get_rfb_id Fail!\n");
			return -1;
		}
		snprintf(rfb_cfg_name, sizeof(rfb_cfg_name), "rfb_%d%02d", board.rfb_id.id1, board.rfb_id.id2);
		if(!fw_getenv(rfb_cfg_name, &rfb_cfg)){
			printf("The rfb_%u%02u is not in mi.conf, use default\n", board.rfb_id.id1, board.rfb_id.id2);
			fw_getenv("rfb_defult", &rfb_cfg);
			board.rfb_id.id1 = 0;
			board.rfb_id.id2 = 0;
		} else {
			fw_getenv(rfb_cfg_name, &rfb_cfg);
		}
		for(int i = 0; rfb_cfg[i]; i++) {
			if(rfb_cfg[i] == ',') {
				cnt++;
			}
		}

		fw_getenv("rfbParmCnt", &rfbParmCnt);
		for(int i =0; rfbParmCnt[i] != '\0'; i++) {
			rfbParmCnt_num = rfbParmCnt_num*10+rfbParmCnt[i]-'0';
		}
		if(cnt > rfbParmCnt_num) {
			printf("[Warning]RFB_Cfg Cnt is %d, the rfbParmCnt is %d\n", cnt, rfbParmCnt_num);
		}

		fw_getenv("srdsPortCnt", &srdsPortCnt);

		printf("RFB ID is rfb_%d%02d\n", board.rfb_id.id1, board.rfb_id.id2);
		printf("RFB cfg: %s\n", rfb_cfg);
		printf("Serdes port cnt: %s\n", srdsPortCnt);
		printf("RFB total param: %s\n", rfbParmCnt);
	} else {
		printf("boardinfo show error!\n");
		return -1;
	}
	return ret;
}
int blapi_system_cfg_board_info(int argc, char *argv[])
{
	int ret = 0;
	int argc_env = 3;
	char *argv_env[3];
	char rfb_cfg_name[25];
	char *rfb_cfg;
	char *rfbParmCnt;
	int rfbParmCnt_num = 0;
	int cnt = 1;

	if(argc == 3) {
		ret = get_rfb_id(&board);
		if(ret != 0) {
			printf("get_rfb_id Fail!\n");
			return -1;
		}
		argv_env[0] = "fw_setenv";
		snprintf(rfb_cfg_name, 25, "rfb_%d%02d", board.rfb_id.id1, board.rfb_id.id2);
		if(!fw_getenv(rfb_cfg_name, &rfb_cfg)){
			strcpy(rfb_cfg_name, "rfb_defult");
		}
		argv_env[1] = rfb_cfg_name;

		for(int i = 0; argv[2][i]; i++) {
			if(argv[2][i] == ',') {
				cnt++;
			}
		}

		fw_getenv("rfbParmCnt", &rfbParmCnt);
		for(int i =0; rfbParmCnt[i] != '\0'; i++) {
			rfbParmCnt_num = rfbParmCnt_num*10+rfbParmCnt[i]-'0';
		}

		if(cnt != rfbParmCnt_num) {
			printf("The input rfb_cfg param is not correct, rfbParmCnt: %d\n", rfbParmCnt_num);
			return -1;
		} else {
			argv_env[2] = argv[2];
		}
		if(fw_setenv(argc_env, argv_env) != 0){
			LIB_BLAPI_ERROR("fw_setenv %s failed ! \n", argv_env[1]);
			return -1;
		}
		printf("set %s board param: %s success!\n", rfb_cfg_name, argv[2]);
	} else {
		printf("boardinfo cfg error!\n");
		return -1;
	}
	return ret;
}
#endif
int blapi_system_doSysSerdesSel(int argc, char *argv[])
{
	int ret = -1;
	unsigned long val = 0;
	int rebootFlag = 1;
	int changePhyType = 0;

	if(argc == 1) {
		showSerdesSelStatus();
	} else if((argc == 3 || argc == 4 || argc == 5 || argc == 6)) {
		if(system_parseNumber(argv[1], 10, &val) == -1) {
			showSerdesSelUsage();
			return -1;
		}
		serdes.port_idx = (ECNT_EVENT_SYSTEM_SERDES_SEL_t)val;

		if(system_parseNumber(argv[2], 10, &val) == -1) {
			showSerdesSelUsage();
			return -1;
		}
		serdes.serdes_if = (uint8_t)val;

		if(argc == 6) {
			if(system_parseNumber(argv[4], 10, &val) != 0) {
				showSerdesSelUsage();
				return -1;
			}
			serdes.phy_sel = (ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t)val;

			if(system_parseNumber(argv[3], 10, &val) != 0) {
				showSerdesSelUsage();
				return -1;
			}
			serdes.ether_type = (ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t)val;
			if (strncmp(argv[5], "-n", 2) == 0) {
				rebootFlag = 0;
			}
			else if (strncmp(argv[5], "-p", 2) == 0) {
				rebootFlag = 0;
				changePhyType = 1;
			}
		} else if(argc == 5) {
			if (strncmp(argv[4], "-n", 2) == 0) {
				rebootFlag = 0;
				serdes.phy_sel = ECNT_EVENT_SERDES_PHY_NONE;

				if(system_parseNumber(argv[3], 10, &val) != 0) {
					showSerdesSelUsage();
					return -1;
				}
				serdes.ether_type = (ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t)val;
			}else {
				rebootFlag = 1;
				if(system_parseNumber(argv[4], 10, &val) != 0) {
					showSerdesSelUsage();
					return -1;
				}
				serdes.phy_sel = (ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t)val;

				if(system_parseNumber(argv[3], 10, &val) != 0) {
					showSerdesSelUsage();
					return -1;
				}
				serdes.ether_type = (ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t)val;
			}
		} else if(argc == 4) {
			if (strncmp(argv[3], "-n", 2) == 0) {
				rebootFlag = 0;
				serdes.phy_sel = ECNT_EVENT_SERDES_PHY_NONE;
				serdes.ether_type = ECNT_EVENT_SERDES_ETHER_TYPE_NONE;
			}else {
				rebootFlag = 1;
				serdes.phy_sel = ECNT_EVENT_SERDES_PHY_NONE;
				
				if(system_parseNumber(argv[3], 10, &val) != 0) {
					showSerdesSelUsage();
					return -1;
				}
				serdes.ether_type = (ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t)val;
			}
		}else if(argc == 3) {
			serdes.phy_sel = ECNT_EVENT_SERDES_PHY_NONE;
			serdes.ether_type = ECNT_EVENT_SERDES_ETHER_TYPE_NONE;
		}

		if (changePhyType == 1)
		{
			serdesUpdatePhyType();
			ret = 0;
		}
		else 
		{
			ret = blapi_system_set_serdes_sel_v2(&serdes, rebootFlag);
		}
		if(ret == 0) {
			ecnt_blapi_printf("set serdesSel success.\n");
		} else {
			ecnt_blapi_printf("set serdesSel fail.\n");
			showSerdesSelUsage();
			return -1;
		}
	} else if(strncmp(argv[1], "help", 4) == 0) {
		showSerdesSelUsage();
	}
	else if (strncmp(argv[1], "ethSel_switch_etherLAN", 22) == 0){
		etherSerdesUpdateEthType(ECNT_EVENT_SERDES_ETHER_TYPE_LAN);
	}
	else if (strncmp(argv[1], "ethSel_switch_etherWAN", 22) == 0){
		etherSerdesUpdateEthType(ECNT_EVENT_SERDES_ETHER_TYPE_WAN);
	}
	else {
		showSerdesSelUsage();
		return -1;
	}

	return 0;
}


int blapi_system_create_mac_conf()
{
	system("ifconfig eth0 | sed -n '1p' | sed 's/^.*HWaddr /WAN_MAC=/g' | sed 's/[ \t]*$//g' >> /etc/mac.conf");
	system("ifconfig eth0 | sed -n '1p' | sed 's/^.*HWaddr /WAN_MAC=/g' | sed 's/[ \t]*$//g' >> /etc/mac.conf");

	return 0;
}

int blapi_system_get_eth0_mac(char * mac)
{
    FILE* fp;
	size_t len = 0;
	ssize_t read = 0;
	char *line = NULL;

	if ((fp = fopen("/etc/mac.conf","r")) == NULL)
	{
		LIB_BLAPI_DEBUG("[%s]%d open /etc/mac.conf fail\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	while((read =  getline(&line, &len, fp)) != -1)
	{
		if(strstr(line, "WAN_MAC=") != NULL)
		{
			sscanf(line, "WAN_MAC=%s", mac);
			break;
		}
	}
	
	if(line)
	{
		free(line);
	}
		
	fclose(fp);

	return 0;

}

int __ioctl_data(int cmd, char *dev, void *data)
{
	int fd = -1;

	if ( NULL == data || NULL == dev )
		return -1;

	fd = open(dev, O_RDWR);
	if (fd < 0 )
	{
		printf("%s:%d Open %s fail.\n", __FUNCTION__, __LINE__, dev);
		return -1;
	}

	if ( 0 != ioctl(fd, cmd, data) )
	{
		printf("%s:%d Info ioctl fail %d.\n", __FUNCTION__, __LINE__, cmd);
		close(fd);
		return -2;
	}
	close(fd);

	return 0;
}

int blapi_system_app_route_data_ctl(int cmd, void *data)
{
	if ( NULL == data )
		return -1;

	return __ioctl_data(cmd, APP_ROUTE_DEV_NAME, data);
}

int blapi_system_app_filter_data_ctl(int cmd, char *dev, void *data)
{
	if ( NULL == data || NULL == dev )
		return -1;

	return __ioctl_data(cmd, dev, data);
}

int blapi_system_app_get_cpu_mask(int cpu_index, int *cpu_mask)
{	
#if defined(TCSUPPORT_CPU_EN7581)
	*cpu_mask = 1;
#else
	switch( cpu_index )
	{
		case 0:
			*cpu_mask = 1;
			break;
		case 1:
			*cpu_mask = 2;
			break;
		case 2:
			*cpu_mask = 4;
			break;
		case 3:
			*cpu_mask = 8;
			break;
		default:
			break;
	}
#endif

	return 0;
}

int blapi_system_check_mac_in_stb_list(unsigned char *mac)
{
	FILE *f = NULL;
	char *buf = NULL;
	size_t len = 0;
	int i_port = 0, ret = 0;
	unsigned char addr[6];

	f = fopen(STB_LIST_PATH, "r");
	if ( f )
	{
		while ( getline(&buf, &len, f) != -1 )
		{
			bzero(addr, sizeof(addr));
			i_port = 0;
			sscanf(buf, "%d=%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &i_port,
				&addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);

			if ( 0 == memcmp(mac, addr, 6) )
			{
				ret = 1;
				break;
			}
		}

		fclose(f);
	}

	if ( buf )
	{
		free(buf);
	}

	return ret;
}

/* upflag: 0->down, 1->adsl, 2->vdsl, 3->ether, 4->SFP */
int blapi_system_get_spf_linkup(int *upflag)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_GET_WAN_LINK_MODE);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(*upflag);
						
	if (!SRC_blapi(blapi_h))
	{
		*upflag = ntohl(blapi_h->argv1.i);

		ret = blapi_h->ret;
	}
	else
	{
		ret = -1;
	}

	free(blapi_h);
	return ret; 
#else
	return -1;
#endif
}

int blapi_system_set_wan_mode(WanMode_t wan_mode)
{
#if defined(TCSUPPORT_SISM_HOST)
	char cmd[128] = {0};
	int ret = 0;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_SET_WAN_MODE);
	blapi_h->argc = htonl(1);
	blapi_h->argv1.i = htonl(wan_mode);
							
	if (!SRC_blapi(blapi_h))
	{
		wan_mode = ntohl(blapi_h->argv1.i);

		ret = blapi_h->ret;
	}
	else
	{
		ret = -1;
	}

	memset(cmd, 0, sizeof(cmd));
	if(WAN_MODE_ATM==wan_mode)
	{
		snprintf(cmd,sizeof(cmd),"echo 0x10000 0x100 > /proc/qdma_wan/mac_resource_limit");	
	}
	else
	{
		snprintf(cmd,sizeof(cmd),"echo 0x10000 0x200 > /proc/qdma_wan/mac_resource_limit");	
	}
	system(cmd);

	free(blapi_h);
	return ret; 
#else
	return -1;
#endif
}

int blapi_system_prepare_interface(blapi_intf_obj_t *intf)
{
#if defined(TCSUPPORT_SISM_HOST)
	char dev[16] = {0};
	char cmd[256] = {0};
	char brMac[16] = {0};

	if( 0 == strcmp(intf->transMode, "ATM") )
	{
		/*ADSL*/
		snprintf(dev, sizeof(dev), "nas%d", (intf->idx / 8));

		/* Disable IPv6 feature for nasX interface. It should be ATM bridge on 7562. */
		memset(cmd,0,sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6", dev);
		if (blapi_system_client_command(cmd) == -1)
		{
			LIB_BLAPI_ERROR("Client execute cmd %s fail\n", cmd);
		}
		system(cmd);

		memset(cmd,0,sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ifconfig %s up", dev);
		if (blapi_system_client_command(cmd) == -1)
		{
			LIB_BLAPI_ERROR("Client execute cmd %s fail\n", cmd);
		}
		system(cmd);
		LIB_BLAPI_DEBUG("%s %d cmd=[%s]\n", __FUNCTION__, __LINE__, cmd);
	}

	return 0;
#else
	return -1;
#endif
}


int blapi_system_del_prepared_interface(blapi_intf_obj_t *intf)
{
#if defined(TCSUPPORT_SISM_HOST)
	char dev[16] = {0};
	char cmd[256] = {0};
	char nasname[16] = {0};
	char pid_path[64] = {0};
	char pid[8]={0};

	if( 0 == strcmp(intf->transMode, "ATM") )
	{
		blapi_xdsl_atm_delete_interface(intf->idx/8);
	}

	return 0;
#else
	return -1;
#endif
}

int blapi_system_client_command(char *cmd)
{
#if defined(TCSUPPORT_SISM_HOST)
	int count = 1, ret = 0;
	blapi_hdr_t *blapi_h;
	
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_SRC_CMD);
	blapi_h->argc = htonl(1);

	memcpy(&blapi_h->argv1,cmd, strlen(cmd));

	while (SRC_blapi(blapi_h))
	{
		/* At most send 3 times. */
		if (count++ >= 3)
		{
			free(blapi_h);

			return -1;
		}
	}

	memcpy(cmd, &blapi_h->argv1, strlen(blapi_h->argv1.ch));
	ret = blapi_h->ret;

	free(blapi_h);
	return ret;
#else
	return -1;
#endif
}

int blapi_system_get_crc(char *crcFile, unsigned int *crc32)
{
	int fd = -1;
	int fipsize = 0x2000;
	char buf[sizeof(struct trx_header)];
	struct trx_header *trx = (struct trx_header *)buf;

	if((fd = open(crcFile, O_RDONLY)) == -1) 
	{
		ecnt_blapi_printf("open file %s fail\n", crcFile);
		return -1;
	}

	lseek(fd, fipsize, SEEK_SET);
	read(fd, buf, sizeof(struct trx_header));
	blapi_system_calculate_crc32(fd, trx->len-sizeof(struct trx_header), crc32);

	if(fd >= 0)
		close(fd);
	return 0;
}

int blapi_system_create_interface(blapi_intf_obj_t *intf)
{
#if defined(TCSUPPORT_SISM_HOST)
	char cmd[256] = {0};

	LIB_BLAPI_DEBUG("%s %d intf->vid=[%d] \n", __FUNCTION__, __LINE__, intf->vid);
    memset(cmd, 0, sizeof(cmd));

	/* obj->idx / 8 + 100: choose which tunnel. 100-109 */
	if( !strcmp(intf->transMode, "ATM") )
	{
		snprintf(intf->dev, sizeof(intf->dev), "nas%d", (intf->idx / 8));
		intf->vtag = 1;
	}
	else
	{
		memset(intf->dev, 0, sizeof(intf->dev));
		strncpy(intf->dev, "ae_wan", sizeof(intf->dev)- 1);
	}
	snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl add %s %s %s %d %d %d %d",
						intf->linkMode,intf->dev,intf->o_dev, intf->vtag, intf->vid,intf->dot1p,intf->mvid);
	system(cmd);
	LIB_BLAPI_DEBUG("%s %d cmd=[%s] \n", __FUNCTION__, __LINE__, cmd);

	if( !strcmp(intf->transMode, "ATM") )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl ipversion %s %d",
											intf->o_dev, intf->version == 3 ? 0: intf->version);
		system(cmd);
		LIB_BLAPI_DEBUG("%s %d cmd=[%s] \n", __FUNCTION__, __LINE__, cmd);
	}
	
	return 0;
#else
	return -1;
#endif
}
