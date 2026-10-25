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
#include <fcntl.h>
#include "blapi_system_bsp.h"
#include "libapi_lib_utility.h"
#include "flash_layout/tc_partition.h"
#include "flash_layout/prolinecmd.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*                  M A C R O S
*************************************************************************
*/

#ifdef TCSUPPORT_OPENWRT
#define 	MTD_CMD		 "/sbin/ecnt_mtd"
#else
#define 	MTD_CMD		 "/userfs/bin/mtd"
#endif
#define 	BOOT_NAME 	 "bootloader"

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
int blapi_system_doSysMac(unsigned char *mac_addr, unsigned char mac_range, int rebootFlag)
{


#if defined(TCSUPPORT_CPU_EN7523)	
    int argc_env = 3;
    char *argv_env[3];
	char arr[18];
	argv_env[2] = arr;
#endif	

	if(!mac_addr)
		return -1;
#if defined(TCSUPPORT_CPU_EN7523)
	argv_env[0] = "fw_setenv";
    argv_env[1] = "ethaddr";
    sprintf(argv_env[2], "%02x:%02x:%02x:%02x:%02x:%02x", \
			mac_addr[0], \
			mac_addr[1], \
			mac_addr[2], \
			mac_addr[3], \
			mac_addr[4], \
			mac_addr[5]
    );

	if(fw_setenv(argc_env, argv_env) != 0){
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

	if(rebootFlag == 1)
	    system("reboot");

#else
    FILE *fp;
	long addr = 0;
	unsigned char tmp[256] = {0};
	/* Read bootloader from flash */
#ifdef TCSUPPORT_OPENWRT
	system("cat /dev/mtd0 > /tmp/boot.bin");
#else
	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");
#endif
	
	/* 1. write mac addr to Flash */
	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");
		system("rm /tmp/boot.bin"); 	   
		return -1;
	}

	blapi_system_get_EtherAddr_offset_L(&addr);
	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to %x fail.\n",addr);
		fclose(fp);
		return -1;
	}

	fwrite(mac_addr, 6, 1, fp );
	fflush(fp);
	fclose(fp);
		
	/* 2. write mac addr range to Flash */
	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");
		system("rm /tmp/boot.bin"); 	   
		return -1;
	}
	
	blapi_system_get_EtherAddr_range_offset_L(&addr);
	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to %x fail.\n",addr);
		fclose(fp);
		return -1;
	}

	fwrite(&mac_range, sizeof(mac_range), 1, fp );
	fflush(fp);
	fclose(fp);
		
	/* Write the bootloader back to flash and reboot*/
	
	memset(tmp, 0, sizeof(tmp));  
	if (rebootFlag == 0){
		/*system("/userfs/bin/mtd -f write /tmp/boot.bin bootloader");*/
		snprintf(tmp,sizeof(tmp),"%s -f write /tmp/boot.bin %s", MTD_CMD, BOOT_NAME);
	}else
	{
		/*system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");*/
		snprintf(tmp,sizeof(tmp),"%s -rf write /tmp/boot.bin %s", MTD_CMD, BOOT_NAME);
	}
	system(tmp);

#endif
	return 0;
}
	
int blapi_system_set_boot_flag_swap(void)
{
#if defined(TCSUPPORT_CT_UPG_PINGPONG) || defined(TCSUPPORT_NOR_FLASH_USED) || defined(CONFIG_DUAL_IMAGE)
	char cmds[256] = {0};
	FILE *fp=NULL;
	char boot_flag = -1;
	int ret = 0;
	
	ret = blapi_system_get_current_boot_flag(&boot_flag);
	if (boot_flag == 0)
		boot_flag = '1';
	else
		boot_flag = '0';
	
	fp = fopen(DUAL_IMG_BOOT_FLAG_FILE, "w");
	if (fp == NULL) {
		return -1;
	}
	
	if (1 != fwrite(&boot_flag, 1, 1, fp)) {
		fclose(fp);
		unlink(DUAL_IMG_BOOT_FLAG_FILE);
		return -1;
	}
	fclose(fp);

#ifdef TCSUPPORT_OPENWRT
	snprintf(cmds, sizeof(cmds), "ecnt_sys set bootflag %s", DUAL_IMG_BOOT_FLAG_FILE);
#else
	snprintf(cmds, sizeof(cmds), TC_FLASH_WRITE_CMD, DUAL_IMG_BOOT_FLAG_FILE, (unsigned long)IMG_BOOT_FLAG_SIZE, (unsigned long)IMG_BOOT_FLAG_OFFSET, RESERVEAREA_NAME);
#endif
	system(cmds);
	
	unlink(DUAL_IMG_BOOT_FLAG_FILE);
#endif
	return 0;
	
}

int blapi_system_get_current_boot_flag(char *boot_flag)
{
#if defined(TCSUPPORT_CT_UPG_PINGPONG) || defined(TCSUPPORT_NOR_FLASH_USED) || defined(CONFIG_DUAL_IMAGE)
	char cmds[256] = {0};
	FILE *fp=NULL;

#ifdef TCSUPPORT_OPENWRT
	snprintf(cmds, sizeof(cmds), "ecnt_sys get bootflag %s", DUAL_IMG_BOOT_FLAG_FILE);
#else
	snprintf(cmds, sizeof(cmds), TC_FLASH_READ_CMD, DUAL_IMG_BOOT_FLAG_FILE, (unsigned long)IMG_BOOT_FLAG_SIZE, (unsigned long)IMG_BOOT_FLAG_OFFSET, RESERVEAREA_NAME);
#endif
	system(cmds);

	fp = fopen(DUAL_IMG_BOOT_FLAG_FILE, "r");
	if (fp == NULL) {
		return -1;
	}
	
	if (1 != fread(boot_flag, 1, 1, fp)) {
		fclose(fp);
		unlink(DUAL_IMG_BOOT_FLAG_FILE);
		return -1;
	}
	fclose(fp);
	unlink(DUAL_IMG_BOOT_FLAG_FILE);

	*boot_flag = *boot_flag - '0';
	if ( *boot_flag != 0 && *boot_flag != 1 )
		*boot_flag = 0;
#endif

	return 0;
}

int blapi_system_doSysQdmaInit(unsigned char qdmaValue, unsigned char qdmaMask) 
{
    FILE *fp = NULL; 
#if defined(TCSUPPORT_CPU_EN7523)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;
	char *str = NULL;
	unsigned char flashQdmaInit = 0;

	if(blapi_system_get_envmsg(QDMA_INIT_ENV, &str) != 0)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else
		flashQdmaInit = atoi(str);

	flashQdmaInit = (flashQdmaInit & (~qdmaMask)) | qdmaValue; 

	argv_env[0] = "fw_setenv";
	argv_env[1] = "qdma_init";
	sprintf(argv_env[2], "%02x", flashQdmaInit);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

	system("reboot");
#else

  
	/* Read bootloader from flash */
	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");    

	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");
		system("rm /tmp/boot.bin");        
		return -1;
	}

	if(QdmaInit(fp,qdmaMask,qdmaValue)!= 0 ){
		printf("operate file error!!\n");
		fclose(fp);
		system("rm /tmp/boot.bin"); 
		return -1;
	}
	
	fflush(fp);
	fclose(fp);
 
	/* Write the bootloader back to flash and reboot*/
	system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");
   
#endif	
	return 0;
}

int blapi_system_doSysQdmaInit_int32(unsigned int qdmaValue,unsigned int qdmaMask) 
{
    FILE *fp = NULL; 
#if defined(TCSUPPORT_CPU_ARMV8)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;
	char *str = NULL;
	unsigned int flashQdmaInit = 0;

	if(blapi_system_get_envmsg(QDMA_INIT_ENV, &str) != 0)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else
        sscanf(str,"%x",&flashQdmaInit);    

	flashQdmaInit = (flashQdmaInit & (~qdmaMask)) | qdmaValue; 

	argv_env[0] = "fw_setenv";
	argv_env[1] = "qdma_init";
	sprintf(argv_env[2], "%x", flashQdmaInit);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

	system("reboot");
#else
    unsigned char mask = qdmaMask&0xff;
    unsigned char value = qdmaValue&0xff;
    /* Read bootloader from flash */
	system("/userfs/bin/mtd readflash /tmp/boot.bin bootloader");    

	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");
		system("rm /tmp/boot.bin");        
		return -1;
	}

	if(QdmaInit(fp,mask,value)!= 0 ){
		printf("operate file error!!\n");
		fclose(fp);
		system("rm /tmp/boot.bin"); 
		return -1;
	}
	
	fflush(fp);
	fclose(fp);
 
	/* Write the bootloader back to flash and reboot*/
	system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");
#endif

	return 0;
}

int blapi_system_doSysOnuType(unsigned char type)
{
	char orgType = 0;

#if defined(TCSUPPORT_CPU_EN7523)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;
	
#if defined(TCSUPPORT_CPU_EN7581)
	char *str = NULL;	
	int ret;
	
	if(blapi_system_get_envmsg(ONU_TYPE_ENV, &str) != 0)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else{
		ret = sscanf(str, "%02x", (unsigned int *)&orgType);
		if(ret != 1) {
			fprintf(stderr, "parse %s error:%d\n", str, ret);
			return -1;
		}
	}
	type = (orgType & ONUMODEMASK) + (orgType & BBF247MASK) + (orgType & COMBOPONMASK) + type;	
#endif
	argv_env[0] = "fw_setenv";
	argv_env[1] = "onu_type";
	sprintf(argv_env[2], "%02x", type);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}
	
#else
    FILE *fp = NULL;
	long addr = 0;
	size_t bytes_read;
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

	bytes_read = fread(&orgType, sizeof(orgType), 1, fp );
	if(bytes_read != sizeof(orgType)) {
		printf("/tmp/boot.bin fread fail, length=%zu.\n", bytes_read);
		fclose(fp);
		return -1;
	}
	type = (orgType & ONUMODEMASK) + (orgType & BBF247MASK) + (orgType & COMBOPONMASK) + type; 

	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to %x fail.\n",addr);
		fclose(fp);
		return -1;
	}
	
    fwrite(&type, sizeof(type), 1, fp );
    fflush(fp);
    fclose(fp);
        
    /* Write the bootloader back to flash and reboot*/
	/*system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");*/
	
    snprintf(tmp,sizeof(tmp),"%s -rf write /tmp/boot.bin %s", MTD_CMD, BOOT_NAME);
	system(tmp);
#endif	
   
    return 0;
}

int blapi_system_doSysBBF247(unsigned char enable)
{
	char orgType = 0;
	FILE *fp = NULL; 
	size_t bytes_read;
	long addr = 0;
	unsigned char tmp[256] = {0};
	unsigned char type = 0;

#if defined(TCSUPPORT_CPU_EN7523)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;
	
#if defined(TCSUPPORT_CPU_EN7581)
	char name[32] = {0};		
	char *str = NULL;	
	int ret;
	
	strncpy(name, "onu_type", sizeof(name) - 1);
	fw_getenv(name, &str);
	if(str == NULL)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else{
		ret = sscanf(str, "%02x", &orgType);
		if(ret != 1) {
			fprintf(stderr, "parse %s error:%d\n", str, ret);
			return -1;
		}
	}
	type = (orgType & ONUMODEMASK) + (orgType & ONUTYPEMASK) + (orgType & COMBOPONMASK) + (enable << 3);	
#endif
	argv_env[0] = "fw_setenv";
	argv_env[1] = "onu_type";
	sprintf(argv_env[2], "%02x", type);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}
	
#else

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

	bytes_read = fread(&orgType, sizeof(orgType), 1, fp );
	if(bytes_read != sizeof(orgType)) {
		printf("/tmp/boot.bin fread fail, length=%zu.\n", bytes_read);
		fclose(fp);
		return -1;
	}
	type = (orgType & ONUMODEMASK) + (orgType & ONUTYPEMASK) + (orgType & COMBOPONMASK) + (enable << 3);

	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to %x fail.\n",addr);
		fclose(fp);
		return -1;
	}
	
    fwrite(&type, sizeof(type), 1, fp );
    fflush(fp);
    fclose(fp);
        
    /* Write the bootloader back to flash and reboot*/
	/*system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");*/
	
    snprintf(tmp,sizeof(tmp),"%s -rf write /tmp/boot.bin %s", MTD_CMD, BOOT_NAME);
	system(tmp);
#endif	
   
    return 0;
}

int blapi_system_doSysComboPon(unsigned char enable)
{
	char orgType = 0;
	FILE *fp = NULL; 
	size_t bytes_read;
	long addr = 0;
	unsigned char tmp[256] = {0};
	unsigned char type = 0;

#if defined(TCSUPPORT_CPU_EN7523)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;
	
#if defined(TCSUPPORT_CPU_EN7581)
	char name[32] = {0};		
	char *str = NULL;	
	int ret;
	
	strncpy(name, "onu_type", sizeof(name) - 1);
	fw_getenv(name, &str);
	if(str == NULL)
	{
		fprintf(stderr, "fw_getenv failed ! \n");
		return -1;
	}
	else{
		ret = sscanf(str, "%02x", &orgType);
		if(ret != 1) {
			fprintf(stderr, "parse %s error:%d\n", str, ret);
			return -1;
		}
	}
	type = (orgType & ONUMODEMASK) + (orgType & ONUTYPEMASK) + (orgType & BBF247MASK)  + (enable << 2);	
#endif
	argv_env[0] = "fw_setenv";
	argv_env[1] = "onu_type";
	sprintf(argv_env[2], "%02x", type);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}
	
#else

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

	bytes_read = fread(&orgType, sizeof(orgType), 1, fp );
	if(bytes_read != sizeof(orgType)) {
		printf("/tmp/boot.bin fread fail, length=%zu.\n", bytes_read);
		fclose(fp);
		return -1;
	}
	type = (orgType & ONUMODEMASK) + (orgType & ONUTYPEMASK) + (orgType & BBF247MASK) + (enable << 2);	

	if(fseek(fp, addr, SEEK_SET) != 0) {
		printf("/tmp/boot.bin fseek to %x fail.\n",addr);
		fclose(fp);
		return -1;
	}
	
    fwrite(&type, sizeof(type), 1, fp );
    fflush(fp);
    fclose(fp);
        
    /* Write the bootloader back to flash and reboot*/
	/*system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");*/
	
    snprintf(tmp,sizeof(tmp),"%s -rf write /tmp/boot.bin %s", MTD_CMD, BOOT_NAME);
	system(tmp);
#endif	
   
    return 0;
}

int blapi_system_doSysHybrid (unsigned char enable)
{
	FILE *fp = NULL; 

#if defined(TCSUPPORT_CPU_AN7583)
	int argc_env = 3;
	char *argv_env[3];
	char arr[18] = {0};
	argv_env[2] = arr;
	char *str = NULL;
	unsigned char flashhybrid = enable;

	argv_env[0] = "fw_setenv";
	argv_env[1] = "hybrid";
	sprintf(argv_env[2], "%01x", flashhybrid);

	if(fw_setenv(argc_env, argv_env) != 0)
	{
		fprintf(stderr, "fw_setenv failed ! \n");
		return -1;
	}

	printf("the setting will take affect after reboot.\n");
#else

	printf("not support hybrid mode!!!\n");
   
#endif	
	return 0;
}



int blapi_system_get_temperature(int* temp_x10)
{
#if defined(TCSUPPORT_THERMAL_SENSOR)
	int ret = 0;
	char buf[64] = {0};
	FILE *proc_file = NULL;
	
	proc_file = fopen("/proc/tc3162/thermal_temp", "r");
	if (!proc_file)
		return -1;
	memset(buf, 0, sizeof(buf));
	if (NULL == fgets(buf, sizeof(buf), proc_file)){
		fclose(proc_file);
		return -1;
	}
	
	fclose(proc_file);
	
	sscanf(buf, "%d\n", temp_x10);
	
#endif
	return 0;
}

int blapi_system_check_crcflag()
{
	char boot_flag = -1;
	int checksum = 0;
	int ret = -1;

	ret = blapi_system_get_current_boot_flag(&boot_flag);
	boot_flag = boot_flag ? 0 : 1;
	if ( 0 == blapi_system_checkimageCrc(boot_flag, &checksum)){
		printf("\nimage[%d] checksum=[%x] is ok\n", boot_flag, checksum);
		return -1;
	}

	return 0;
}

int blapi_system_get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel)
{
#if defined(TCSUPPORT_CPU_EN7523)
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

	if(blapi_system_get_envmsg(SERDES_SEL_ENV, &str) != 0)
	{
		fprintf(stderr, "fw_getenv serdes_sel failed ! \n");
		return -1;
	}

	ret = sscanf(str, "%02x", (unsigned int*)&env_val);
	if(ret != 1) {
		fprintf(stderr, "parse %s error:%d\n", str, ret);
		return -1;
	}

	if((env_val & (0x1 << bit_idx)) == 0) {
		ret = 0;
	} else {
		ret = 1;
	}

	return ret;
#else
	return -1;
#endif
}

int blapi_system_get_err_cnt(unsigned char *error_flag, uint64_t *prbs_error_count)
{
#if defined(TCSUPPORT_CPU_EN7581)
	FILE *file = NULL;
	char *buf = NULL;
	size_t len = 0;
	uint64_t err_cnt = 0;
	
	if (NULL == error_flag || NULL == prbs_error_count)
		return -1;

	file = fopen("/proc/pon_phy/err_cnt", "r");
	if (NULL == file)
		return -1;

	while (-1 != getline(&buf, &len, file)) 
	{		
		if ( NULL != strstr(buf, "0x1fa8b37c"))
		{
			sscanf(buf,"0x1fa8b37c = %llx", &err_cnt);
			break;
		}
	}

	*prbs_error_count = err_cnt;
	if (err_cnt > 0)
		*error_flag = 1;
	else
		*error_flag = 0;
	
	fclose(file);
	if (buf)
	{
		free(buf);
	}
#endif
	return 0;
}

int blapi_system_get_secureboot_en(unsigned int *enable)
{
	if(enable == NULL)
		return -1;
#if defined(TCSUPPORT_SECURE_BOOT) ||  defined(TCSUPPORT_ARM_SECURE_BOOT)
	*enable = 1;
#else
	*enable = 0;
#endif
	return 0;
}