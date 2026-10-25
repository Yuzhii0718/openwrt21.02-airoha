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
#include <ctype.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/file.h>
#include <signal.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/reboot.h>
#include <linux/types.h>
#include <linux/version.h>
#include "ecnt_mmc.h"

static const cmds_t mmc_operation[] = {
    {"cal", do_mmc_cal_op, 0, 0, NULL},
    {NULL, NULL, 0, 0, NULL}
};

static const cmds_t mmc_cal_operation[] = {
    {"fetch", do_mmc_cal_op_fetch, 0, 0, NULL},
    {"store", do_mmc_cal_op_store, 0, 0, NULL},
    {"del", do_mmc_cal_op_del, 0, 0, NULL},
    {"flip", do_mmc_cal_op_flip, 0, 0, NULL},
    {"dump", do_mmc_cal_op_dump, 0, 0, NULL},
    {NULL, NULL, 0, 0, NULL}
};

int do_mmc_cal_op_fetch(int argc,char* argv[],void *p)
{
    uint64_t start = 0;
    uint64_t size =0;
    char *entptr = NULL;
    char *target_filename = NULL;
    
    start = strtoull(argv[1], &entptr, 0);
    size = strtoull(argv[2], &entptr, 0);
    target_filename = argv[3];

    return mmc_read_cal_to_file(start, size, target_filename);
}

int do_mmc_cal_op_store(int argc,char* argv[],void *p)
{
    uint64_t start = 0;
    uint64_t size =0;
    char *entptr = NULL;
    char *target_filename = NULL;
    
    start = strtoull(argv[1], &entptr, 0);
    size = strtoull(argv[2], &entptr, 0);
    target_filename = argv[3];

    return mmc_write_cal_from_file(start, size, target_filename);
}

int do_mmc_cal_op_del(int argc,char* argv[],void *p)
{
    uint64_t start = 0;
    uint64_t size =0;
    char *entptr = NULL;
    
    start = strtoull(argv[1], &entptr, 0);
    size = strtoull(argv[2], &entptr, 0);

    return mmc_cal_del(start, size);
}

int do_mmc_cal_op_flip(int argc,char* argv[],void *p)
{
    uint64_t start = 0;
    char *entptr = NULL;
    
    start = strtoull(argv[1], &entptr, 0);
    if (mmc_cal_flip_byte(start) != 0 ){
        return -1;
    }
    
    return 0;
}

int do_mmc_op_dump(char *partition_name, uint64_t start, uint64_t size)
{
    void *var = NULL;
    const unsigned char *dump_bytes = NULL;
    size_t i, j;
    var = malloc(size);
    if (var == NULL) {
        return -1;
    }
    
    if (mmc_read_to_var(partition_name, start, size, var) != 0 ) {
        return -1;
    }

    dump_bytes = (const unsigned char *)var;
    for (i = 0; i < size; i += 16) {
#if UINTPTR_MAX == UINT64_MAX
        printf("0x%016llX | ", (void *)(start + i));
#else
        printf("0x%08X | ", (void *)(start + i));
#endif
        for (j = 0; j < 16 && i + j < size; ++j) {
            printf("%02x ", dump_bytes[i + j]);
        }
        for (; j < 16; ++j) {
            printf("   ");
        }
        printf("| ");

        for (j = 0; j < 16 && i + j < size; ++j) {
            unsigned char ch = dump_bytes[i + j];
            if (isprint(ch)) {
                printf("%c", ch);
            } else {
                printf(".");
            }
        }
        printf("\r\n");
    }
    
    return 0;
}

int do_mmc_cal_op_dump(int argc,char* argv[],void *p)
{
    uint64_t start = 0;
    uint64_t size =0;
    char *entptr = NULL;
    
    start = strtoull(argv[1], &entptr, 0);
    size = strtoull(argv[2], &entptr, 0);
    printf("Partition name: %s.\r\n", PARTITION_CAL_DATA);
    if (do_mmc_op_dump(PARTITION_CAL_DATA, start, size) != 0 ){
        printf("Failed to dump this partition info.\r\n");
        return -1;
    }
    
    return 0;
}
int do_mmc_cal_op(int argc, char* argv[], void *p)
{
    return subcmd(mmc_cal_operation, argc, argv, (void *)mmc_cal_operation);
}

int main(int argc, char **argv)
{
    return subcmd(mmc_operation, argc, argv, (void *)mmc_operation);
}


