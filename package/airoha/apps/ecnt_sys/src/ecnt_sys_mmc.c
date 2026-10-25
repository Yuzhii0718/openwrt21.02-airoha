/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°Airoha Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to Airoha (HK) Limited (¡°Airoha¡±) and/or 
its licensors. Without the prior written permission of Airoha and/or its licensors, 
any reproduction, modification, use or disclosure of Airoha Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

Airoha (HK) Limited  AIROHA. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°AIROHA SOFTWARE¡±) RECEIVED FROM AIROHA 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES AIROHA PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE AIROHA SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN AIROHA SOFTWARE.

AIROHA SHALL NOT BE RESPONSIBLE FOR ANY AIROHA SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
AIROHA'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AIROHA 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT AIROHA'S SOLE OPTION, TO 
REVISE OR REPLACE THE AIROHA SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO AIROHA FOR SUCH 
AIROHA SOFTWARE.
***************************************************************/


#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <string.h>
#include <linux/reboot.h>
#include <linux/types.h>
#include <trx.h>
#include <mtd/mtd-user.h>
#include "ecnt_sys.h"
#include "ecnt_mmc.h"
#include "ecnt_sys_mmc.h"


static const cmds_t cal_operation[] = {
    {"get", do_cal_op_get, 0, 0, NULL},
    {"set", do_cal_op_set, 0, 0, NULL},
    {"swap", do_cal_op_swap, 0, 0, NULL},
    {"del", do_cal_op_del, 0, 0, NULL},
    {NULL,NULL,0,0,NULL}
};

static const cmds_t cal_get_target[] = {
    {"bob", do_cal_get_bob, 0, 0, NULL},
    {"reserve", do_cal_get_reserve, 0, 0, NULL},
    {"wifi_6G", do_cal_get_wifi6, 0, 0, NULL},
    {"wifi_2.4G", do_cal_get_wifi24, 0, 0, NULL},
    {"wifi_5G", do_cal_get_wifi5, 0, 0, NULL},
    {"prolinecmd", do_cal_get_prolinecmd, 0, 0, NULL},
    {"bootflag", do_cal_get_bootflag, 0, 0, NULL},
    {NULL,NULL,0,0,NULL}
};

static const cmds_t cal_set_target[] = {
    {"bob", do_cal_set_bob, 0, 0, NULL},
    {"reserve", do_cal_set_reserve, 0, 0, NULL},
    {"wifi_6G", do_cal_set_wifi6, 0, 0, NULL},
    {"wifi_2.4G", do_cal_set_wifi24, 0, 0, NULL},
    {"wifi_5G", do_cal_set_wifi5, 0, 0, NULL},
    {"prolinecmd", do_cal_set_prolinecmd, 0, 0, NULL},
    {"bootflag", do_cal_set_bootflag, 0, 0, NULL},
    {NULL,NULL,0,0,NULL}
};

static const cmds_t cal_swap_target[] = {
    {"bootflag", do_cal_swap_bootflag, 0, 0, NULL},
    {NULL,NULL,0,0,NULL}
};

static const cmds_t cal_del_target[] = {
    {"bob", do_cal_del_bob, 0, 0, NULL},
    {"wifi_6G", do_cal_del_wifi6, 0, 0, NULL},
    {NULL,NULL,0,0,NULL}
};

int do_cal_bob_operations(char *lddla_module, char *operation)
{
    int ret = 0, i = 0;
    uint64_t start;
    uint64_t size;
    if ((strcmp(operation, ECNT_MMC_CMD_FETCH) != 0) && 
        (strcmp(operation, ECNT_MMC_CMD_STORE) != 0) &&
        (strcmp(operation, ECNT_MMC_CMD_DEL) != 0)) {
        fprintf(stderr, "Usage: ecnt_sys <get/set/del> bob <7570/7572/dualbob_7572>\r\n");
        return -1;
    }

    start = 0;
    i = 0;
    while (i < CAL_BOB) {
        start += size_art[i++];
    }

    if (strcmp(lddla_module, "7570") == 0) {
        ret = mmc_cal_io_control(operation, start, BOB_7570_LEN, bob_path[0]);
    } else if (strcmp(lddla_module, "7572") == 0) {
        ret = mmc_cal_io_control(operation, start, BOB_7572_LEN, bob_path[1]);
	} else if (strcmp(lddla_module, "8901") == 0) {
        ret = mmc_cal_io_control(operation, start, BOB_8901_LEN, bob_path[1]);	
    } else if (strcmp(lddla_module, "dualbob_7572") == 0) {
        ret = mmc_cal_io_control(operation, start, BOB_7572_LEN, bob_path[2]);
        ret |= mmc_cal_io_control(operation, start+size_art[CAL_BOB]/2, BOB_7572_LEN, bob_path[3]);
    } else {
        fprintf(stderr, "Usage: ecnt_sys <get/set/del> bob <7570/7572/dualbob_7572>\r\n");
    }

    return ret;
}

int do_cal_get_bob(int argc, char *argv[], void *p)
{
    if(argc == 2) {
        return do_cal_bob_operations(argv[1], ECNT_MMC_CMD_FETCH);
    }
    return -1;
}

int do_cal_set_bob(int argc, char *argv[], void *p)
{
    if(argc == 2) {
        return do_cal_bob_operations(argv[1], ECNT_MMC_CMD_STORE);
    }
    return -1;
}

int do_cal_del_bob(int argc, char *argv[], void *p)
{
    if(argc == 2) {
        return do_cal_bob_operations(argv[1], ECNT_MMC_CMD_DEL);
    }
    return -1;
}

int do_cal_get_reserve(int argc, char *argv[], void *p)
{
    int ret = 0;
    return ret;
}

int do_cal_set_reserve(int argc, char *argv[], void *p)
{
    int ret = 0;
    return ret;
}

int do_cal_wifi_operations(char* wifi_module, char *operation)
{
    int i = 0;
    uint64_t start;
    uint64_t size;
    int wifi_index = 0;

    if (strcmp(wifi_module, wifi_modules[WIFI_MODULE_6G]) == 0) {
        wifi_index = CAL_WIFI_6G;
    } else if (strcmp(wifi_module, wifi_modules[WIFI_MODULE_5G]) == 0) {
        wifi_index = CAL_WIFI_5G;
    } else if (strcmp(wifi_module, wifi_modules[WIFI_MODULE_24G]) == 0) {
        wifi_index = CAL_WIFI_24G;
    } else {
        fprintf(stderr, "Usage: ecnt_sys <get/set/del> <wifi_module>\r\n");
        return -1;
    }

    start = 0;
    i = 0;
    while (i < wifi_index) {
        start += size_art[i++];
    }

    if (wifi_index == CAL_WIFI_6G) {
        /* Eagle uses Wi-Fi 6G only. Wi-Fi 5G and 2.4G to be filled.*/
        size = WIFI6_CAL_SIZE;
        return mmc_cal_io_control(operation, start, size, wifi_path[0]);
    } else {
        return -1;
    }

    return 0;
}

int do_cal_wifi_operations_manual(char* wifi_module, char *operation, char *size_s, char *filename)
{
    int i = 0;
    uint64_t start;
    uint64_t size;
    char *entptr = NULL;
    int wifi_index = 0;
    
    if (strcmp(wifi_module, wifi_modules[WIFI_MODULE_6G]) == 0) {
        wifi_index = CAL_WIFI_6G;
    } else if (strcmp(wifi_module, wifi_modules[WIFI_MODULE_5G]) == 0) {
        wifi_index = CAL_WIFI_5G;
    } else if (strcmp(wifi_module, wifi_modules[WIFI_MODULE_24G]) == 0) {
        wifi_index = CAL_WIFI_24G;
    } else {
        fprintf(stderr, "Usage: ecnt_sys <get/set/del> <wifi_module> <size> <filename>\r\n");
        return -1;
    }
    start = 0;
    i = 0;
    while (i < wifi_index) {
        start += size_art[i++];
    }
    size = strtoull(size_s, &entptr, 0);
    
    return mmc_cal_io_control(operation, start, size, filename);
}

int do_cal_get_wifi6(int argc, char *argv[], void *p)
{
    if (argc == 1) {
        return do_cal_wifi_operations(wifi_modules[WIFI_MODULE_6G], ECNT_MMC_CMD_FETCH);
    } else if (argc == 3) {
        return do_cal_wifi_operations_manual(wifi_modules[WIFI_MODULE_6G], 
            ECNT_MMC_CMD_FETCH, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys <get> wifi_6G [size] [filename]\r\n");
    
    return -1;
}

int do_cal_set_wifi6(int argc, char *argv[], void *p)
{
    if (argc == 1) {
        return do_cal_wifi_operations(wifi_modules[WIFI_MODULE_6G], ECNT_MMC_CMD_STORE);
    } else if (argc == 3) {
        return do_cal_wifi_operations_manual(wifi_modules[WIFI_MODULE_6G], 
            ECNT_MMC_CMD_STORE, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys <set> wifi_6G [size] [filename]\r\n");
    
    return -1;
}

int do_cal_del_wifi6(int argc, char *argv[], void *p)
{
    if (argc == 1)
    {
        return do_cal_wifi_operations(wifi_modules[WIFI_MODULE_6G], ECNT_MMC_CMD_DELETE);
    }
    else
    {
        fprintf(stderr, "Usage: ecnt_sys <del> wifi_6G [size] [filename]\r\n");
    }

    return -1;
}

int do_cal_get_wifi24(int argc, char *argv[], void *p)
{
    if (argc == 1) {
        return do_cal_wifi_operations(wifi_modules[WIFI_MODULE_24G], ECNT_MMC_CMD_FETCH);
    } else if (argc == 3) {
        return do_cal_wifi_operations_manual(wifi_modules[WIFI_MODULE_24G], 
            ECNT_MMC_CMD_FETCH, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys <get> wifi_24G [size] [filename]\r\n");
    
    return -1;

}
int do_cal_set_wifi24(int argc, char *argv[], void *p)
{
    if (argc == 1) {
        return do_cal_wifi_operations(wifi_modules[WIFI_MODULE_24G], ECNT_MMC_CMD_STORE);
    } else if (argc == 3) {
        return do_cal_wifi_operations_manual(wifi_modules[WIFI_MODULE_24G], 
            ECNT_MMC_CMD_STORE, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys <set> wifi_24G [size] [filename]\r\n");
    
    return -1;

}

int do_cal_get_wifi5(int argc, char *argv[], void *p)
{
    if (argc == 1) {
        return do_cal_wifi_operations(wifi_modules[WIFI_MODULE_5G], ECNT_MMC_CMD_FETCH);
    } else if (argc == 3) {
        return do_cal_wifi_operations_manual(wifi_modules[WIFI_MODULE_5G], 
            ECNT_MMC_CMD_FETCH, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys <get> wifi_5G [size] [filename]\r\n");
    
    return -1;

}
int do_cal_set_wifi5(int argc, char *argv[], void *p)
{
    if (argc == 1) {
        return do_cal_wifi_operations(wifi_modules[WIFI_MODULE_5G], ECNT_MMC_CMD_STORE);
    } else if (argc == 3) {
        return do_cal_wifi_operations_manual(wifi_modules[WIFI_MODULE_5G], 
            ECNT_MMC_CMD_STORE, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys <set> wifi_5G [size] [filename]\r\n");
    
    return -1;

}

int do_cal_prolinecmd_operations_manual(char *operation, char *size_s, char *filename)
{
    int i = 0;
    uint64_t start;
    uint64_t size;
    char *entptr = NULL;
    int wifi_index = 0;

    start = 0;
    i = 0;
    while (i < CAL_PROLINECMD) {
        start += size_art[i++];
    }
    size = strtoull(size_s, &entptr, 0);

    return mmc_cal_io_control(operation, start, size, filename);
}

int do_cal_get_prolinecmd(int argc, char *argv[], void *p)
{
    if (argc == 3) {
        return do_cal_prolinecmd_operations_manual(ECNT_MMC_CMD_FETCH, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys get prolinecmd <size> <filename>\r\n");
    
    return -1;
}

int do_cal_set_prolinecmd(int argc, char *argv[], void *p)
{
    if (argc == 3) {
        return do_cal_prolinecmd_operations_manual(ECNT_MMC_CMD_STORE, argv[1], argv[2]);
    }
    fprintf(stderr, "Usage: ecnt_sys set prolinecmd <size> <filename>\r\n");
    
    return -1;
}

int do_cal_bootflag_operations(char *operation, char *bootfile)
{
    int i = 0;

    uint64_t start;
    char *entptr = NULL;
    char *bootflag_op = NULL;
    int wifi_index = 0;

    start = 0;
    i = 0;
    while (i < CAL_BOOTFLAG) {
        start += size_art[i++];
    }
    if (strcmp(operation, ECNT_MMC_CMD_SWAP) == 0) {
        mmc_cal_io_control(ECNT_MMC_CMD_FLIP, start, 0, NULL);
        mmc_cal_io_control(ECNT_MMC_CMD_FETCH, start, BOOTFLAG_SIZE, bootfile);
    } else {
        return mmc_cal_io_control(operation, start, BOOTFLAG_SIZE, bootfile);
    }

    return 0;
}

int do_cal_get_bootflag(int argc, char *argv[], void *p)
{
    if (argc == 2) {
        return do_cal_bootflag_operations(ECNT_MMC_CMD_FETCH, argv[1]);
    }
    fprintf(stderr, "Usage: ecnt_sys <get/set> bootflag /tmp/boot\r\n"
		"       ecnt_sys swap bootflag\n");

    return -1;
}
int do_cal_set_bootflag(int argc, char *argv[], void *p)
{
    if (argc == 2) {
        return do_cal_bootflag_operations(ECNT_MMC_CMD_STORE,argv[1]);
    }
    fprintf(stderr, "Usage: ecnt_sys <get/set> bootflag /tmp/boot\r\n"
		"       ecnt_sys swap bootflag\n");

    return -1;
}
int do_cal_swap_bootflag(int argc, char *argv[], void *p)
{
    if (argc == 1) {
        return do_cal_bootflag_operations(ECNT_MMC_CMD_SWAP, BOOT_FLAG_FILE);
    }
    fprintf(stderr, "Usage: ecnt_sys <get/set> bootflag /tmp/boot\r\n"
		"       ecnt_sys swap bootflag\n");

    return -1;
}


int do_cal_op_get(int argc,char* argv[],void *p) {
    return subcmd(cal_get_target,argc,argv,(void *)cal_get_target);
}

int do_cal_op_set(int argc,char* argv[],void *p) {
    return subcmd(cal_set_target,argc,argv,(void *)cal_set_target);
}

int do_cal_op_swap(int argc,char* argv[],void *p) {
    return subcmd(cal_swap_target,argc,argv,(void *)cal_swap_target);
}

int do_cal_op_del(int argc,char* argv[],void *p) {
    return subcmd(cal_del_target,argc,argv,(void *)cal_del_target);
}

int main_mmc(int argc, char **argv)
{
    return subcmd(cal_operation, argc, argv, (void *)cal_operation);
}


