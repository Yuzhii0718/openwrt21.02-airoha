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

#ifndef ECNT_SYS_MMC_H
#define ECNT_SYS_MMC_H

#include "libapi_lib_utility.h"

#define MAX_CMD_LEN 256

struct calibration_data_t {
    uint32_t index;
    uint64_t start;
    uint64_t size;
    uint64_t mode;
    char* data;
} cal_data;

struct calibration_part_t {
    uint32_t index;
    uint64_t start;
    uint64_t size;
    uint32_t mode;
    uint32_t data_count;
    char* name;
} cal_part;

enum {
    CAL_RESERVE = 0,
    CAL_WIFI_6G,
    CAL_BOB,
    CAL_WIFI_24G,
    CAL_WIFI_5G,
    CAL_PROLINECMD,
    CAL_BOOTFLAG,
    CAL_AREA_NUM
} CALIBRATION_LAYOUT;


/* parameters for ECNT_MMC_CMD: command, start, size, target_filename*/
#define ECNT_MMC_CMD                "/userfs/bin/ecnt_mmc cal %s %llu %llu  %s"
#define ECNT_MMC_FLIP_CMD           "/userfs/bin/ecnt_mmc cal flip %llu"

#define ECNT_MMC_CMD_FETCH "fetch"
#define ECNT_MMC_CMD_STORE "store"
#define ECNT_MMC_CMD_DELETE "del"
#define ECNT_MMC_CMD_SWAP "swap"

#define ECNT_MMC_CMD_WRITE_FLASH "writeflash"
#define ECNT_MMC_CMD_READ_FLASH "readflash"

#define ECNT_MMC_CMD_GET "get"
#define ECNT_MMC_CMD_SET "set"
#define ECNT_MMC_CMD_DEL "del"
#define ECNT_MMC_CMD_FLIP "flip"

#define WIFI6_CAL_SIZE                  0xf1000
char *wifi_path[] = {
    "/tmp/wifi_6g.bin",
    "/tmp/wifi_5g.bin",
    "/tmp/wifi_2g.bin",
};

char *wifi_modules[] = {
    "wifi_6G",
    "wifi_5G",
    "wifi_24G",
};

enum {
    WIFI_MODULE_6G = 0,
    WIFI_MODULE_5G,
    WIFI_MODULE_24G,
    WIFI_MODULE_NUM
} WIFI_MODULE;


#define BOOTFLAG_SIZE               0x1
#define BOOTFLAG_MASTER             0
#define BOOTFLAG_SLAVE              1


int do_cal_op_get(int argc,char* argv[],void *p);
int do_cal_op_set(int argc,char* argv[],void *p);
int do_cal_op_swap(int argc,char* argv[],void *p);
int do_cal_op_del(int argc,char* argv[],void *p);

int do_cal_get_bob(int argc, char *argv[], void *p);
int do_cal_set_bob(int argc, char *argv[], void *p);
int do_cal_del_bob(int argc, char *argv[], void *p);

int do_cal_get_reserve(int argc, char *argv[], void *p);
int do_cal_set_reserve(int argc, char *argv[], void *p);

int do_cal_get_wifi6(int argc, char *argv[], void *p);
int do_cal_set_wifi6(int argc, char *argv[], void *p);
int do_cal_del_wifi6(int argc, char *argv[], void *p);
int do_cal_get_wifi24(int argc, char *argv[], void *p);
int do_cal_set_wifi24(int argc, char *argv[], void *p);
int do_cal_get_wifi5(int argc, char *argv[], void *p);
int do_cal_set_wifi5(int argc, char *argv[], void *p);

int do_cal_get_prolinecmd(int argc, char *argv[], void *p);
int do_cal_set_prolinecmd(int argc, char *argv[], void *p);

int do_cal_get_bootflag(int argc, char *argv[], void *p);
int do_cal_set_bootflag(int argc, char *argv[], void *p);
int do_cal_swap_bootflag(int argc, char *argv[], void *p);

#endif
