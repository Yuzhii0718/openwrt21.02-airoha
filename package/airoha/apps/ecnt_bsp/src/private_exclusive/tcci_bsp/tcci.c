/************************************************************************
 *
 *	Copyright (C) 2006 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include "tcversion.h"
#include <stdint.h>
#include "blapi_system.h"
#include "common.h"


#ifdef TCSUPPORT_OPENWRT
#define 	MTD_CMD		 "/sbin/ecnt_mtd"
#define 	BOOT_NAME 	 "bootloader"
#elif TCSUPPORT_IS_CF_PON
#define 	MTD_CMD		 "/userfs/bin/mtd"
#define 	BOOT_NAME 	 "boot"
#else
#define 	MTD_CMD		 "/userfs/bin/mtd"
#define 	BOOT_NAME 	 "bootloader"
#endif

typedef int (*func_t)(int argc,char *argv[]);

#define ONUTYPEMASK 0x3
#define COMBOPONMASK  0x4
#define BBF247MASK  0x8
#define ONUMODEMASK 0xf0

func_t ci_func[] ={
	dummy,
    doSysMac,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    doSysOnuType,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    doSysBobCarlibration, 	/* 23*/
#if defined(TCSUPPORT_CT_UPG_PINGPONG) || defined(TCSUPPORT_NOR_FLASH_USED) || defined(CONFIG_DUAL_IMAGE)
    doSysBootFlag,			/* 24 */
#else
    NULL,
#endif
    NULL,
    NULL,
    doSysSifm,
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
    doSysPONMode,
#else
	NULL,
#endif
#else
	NULL,
#endif
	doSysQdmaInit,
    NULL,
    NULL,
    doSysSerdesSel,	/* 32 */
    NULL,
    doSysBBF247,  /* 34 */
    doSysComboPon, /* 35 */	
#ifdef TCSUPPORT_BOARD_SELECT
    doSysBoardInfo,  /* 36 */
#endif
	doSysHybrid,   /*37*/
};


int main(int argc, char **argv) 
{
	FILE *proc_file;
	char cmd[160];
	const char *s;
	const char *applet_name;
	int i;
	char str[160];
	int func_num=0;

	if(argc == 2){
		if(!strcmp(argv[1],"version")){
			printf("\r\n tcci version: %s\n",MODULE_VERSION_TCCI);
			return 0;
		}
	}
	
	applet_name = argv[0];

	for (s = applet_name; *s != '\0';) {
		if (*s++ == '/')
			applet_name = s;
	}

	strncpy(cmd, applet_name, sizeof(cmd) - 1);
	for (i = 1; i < argc; i++) {
		strcat(cmd, " ");
		strncat(cmd, argv[i], sizeof(cmd) - strlen(cmd) - 1);
	}

    proc_file = fopen("/proc/tc3162/tcci_cmd", "w");
	if (!proc_file) {
		printf("open /proc/tc3162/tcci_cmd fail\n");
		return 0;
	}

	fprintf(proc_file, "%s", cmd);
	fclose(proc_file);

    proc_file = fopen("/proc/tc3162/tcci_cmd", "r");
	if (!proc_file) {
		printf("open /proc/tc3162/tcci_cmd fail\n");
		return 0;
	}
	fgets(str, 160, proc_file);
	func_num = atoi(str);
//	printf("data=%d\n", func_num);
	fclose(proc_file);
	
	if(func_num == 0 )
	   return 0;
	   
	if( (func_num >= 0 && func_num < (sizeof(ci_func)/sizeof(ci_func[0]))) && ci_func[func_num] != NULL )	
        	ci_func[func_num](argc-1, &argv[1]);
	return 0;
}
