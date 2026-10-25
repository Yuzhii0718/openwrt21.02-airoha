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
#include <dirent.h>

enum {
    MMC_LOCK,
    MMC_UNLOCK,
    MMC_NONE
} MMC_LOCKER;

enum {
    CMD_ERASE,
    CMD_WRITE,
    CMD_READ,
    CMD_ERASESECTOR,
    CMD_BOB_GET,
    CMD_BOB_SET,
    CMD_QDMADSCP_GET,
    CMD_QDMADSCP_SET,
    CMD_UNLOCK
} MMC_CMD;


int mmc_read_locker()
{
    return MMC_UNLOCK;
}

int mmc_write_locker()
{
    return MMC_UNLOCK;
}

static int find_mmcblk0pX_device(char *partname, char* dev_entry, size_t len) 
{
    DIR *mmcblk = NULL;
    struct dirent *dent = NULL;
    char name_path[64] = {0};
    char name[16] = {0};
	FILE *fp = NULL;

    mmcblk = opendir("/sys/block/mmcblk0");
    if (mmcblk == NULL) {
        printf("Failed to open /sys/block/mmcblk0\n");
        return 1;
    }

    while ((dent = readdir(mmcblk)) != NULL) {
        if (dent->d_name[0] == '.' && (dent->d_name[1] == '\0' || dent->d_name[1] == '.')) {
            continue;
        }
		if (strncmp(dent->d_name, "mmcblk0p", 8) == 0) {
			snprintf(name_path, sizeof(name_path), "/sys/block/mmcblk0/%s/name", dent->d_name);
			fp = fopen(name_path, "r");
            if (fp == NULL)
                continue;
			bzero(name, sizeof(name));
			fread(name, 1, sizeof(name), fp);
            fclose(fp);
			
			if (strncmp(name, partname,3) == 0) {
                snprintf(dev_entry, len, "%s%s", "/dev/", dent->d_name);
				closedir(mmcblk);
                return 0;
            }
		}
    }

    closedir(mmcblk);

    return 1;

}

int get_target_by_symlink(char* symlink, char* target, size_t max_link_len)
{
    ssize_t len = 0;
    
    len = readlink(symlink, target, max_link_len -1);
    if (len == -1) {
        return -1;
    }

    target[len] = '\0';
    return 0;
}

static int mmc_partition_to_dev_node(char* partition, char* dev_node)
{
    /* given partition name, convert to dev_node */
    char symlink[PATH_MAX] = {0};

    snprintf(symlink, sizeof(symlink), "%s/%s", DEV_DISK_LABEL_DIR, partition);
    if (get_target_by_symlink(symlink, dev_node, PATH_MAX) != 0) {
        return -1;
    }

    return 0;
}

int mmc_read_to_var(char* partition_name, uint64_t start, uint64_t size, void* var)
{
    int ret = -1;
    uint64_t fd_size = 0;

#if !defined(TCSUPPORT_OPENWRT)    
    char dev_node_relative[PATH_MAX] = {0};
#endif
    char dev_node_absolute[PATH_MAX] = {0};
    void *mmc_mapping =NULL;

    if (partition_name == NULL) {
        return -1;
    }
    /* convert to dev_node */
#if defined(TCSUPPORT_OPENWRT)
	if (find_mmcblk0pX_device(partition_name, dev_node_absolute, PATH_MAX) != 0)
	{
		printf("Failed to find the dev node of %s partition\n",partition_name);
		return -1;
	}
#else
    if (mmc_partition_to_dev_node(partition_name, (char *)dev_node_relative) != 0) {
        return -1;
    }
    snprintf(dev_node_absolute, sizeof(dev_node_absolute), "%s/%s", DEV_DISK_LABEL_DIR, dev_node_relative);
#endif
    int fd = open(dev_node_absolute, O_RDWR);
    ioctl(fd, BLKGETSIZE64, &fd_size);

    mmc_mapping = mmap(NULL, fd_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmc_mapping == MAP_FAILED) {
		printf("Failed to map mmc\n");
        return ret;
    }

    memcpy(var, mmc_mapping + start, size);

    return 0;
}

int mmc_read_to_file(char* partition_name, uint64_t start, uint64_t size, char* output_filename)
{
    int ret = -1;

    void *output_raw = NULL;

    FILE *output_file = NULL;
    
    /* 1. do memcpy */
    // memcpy
    output_raw = malloc(size);
    if (output_raw == NULL) {
        return ret;
    }
    if (mmc_read_to_var(partition_name, start, size, output_raw) != 0 ) {
        free(output_raw);
        return -1;
    }

    /* 2. dump to filename*/
    output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        free(output_raw);
        return ret;
    }
    if (fwrite(output_raw, 1, size, output_file) != size) {
        ret = -1;
    } else {
        ret = 0;
    }

    fclose(output_file);
    free(output_raw);
    fprintf(stdout, "Reading from %s to %s ...\r\n", partition_name, output_filename);
    return ret;
}

int mmc_read_cal_to_file(uint64_t start, uint64_t size, char *output_filename)
{
    return mmc_read_to_file(PARTITION_CAL_DATA, start, size, output_filename);
}


int mmc_write_from_var(char *partition_name, uint64_t start, uint64_t size, void *var)
{
    int ret = -1;
    uint64_t fd_size = 0;
    void *mmc_mapping =NULL;
#if !defined(TCSUPPORT_OPENWRT)
    char dev_node_relative[PATH_MAX] = {0};
#endif
    char dev_node_absolute[PATH_MAX] = {0};

    /* 1. read from file */
    if (var == NULL || partition_name == NULL) {
        return -1;
    }
#if defined(TCSUPPORT_OPENWRT)
	if (find_mmcblk0pX_device(partition_name, dev_node_absolute, PATH_MAX) != 0)
	{
		printf("Failed to find the dev node of %s partition\n",partition_name);
		return -1;
	}
#else
    if (mmc_partition_to_dev_node(partition_name, (char *)dev_node_relative) != 0) {
        return -1;
    }
    snprintf(dev_node_absolute, sizeof(dev_node_absolute), "%s/%s", DEV_DISK_LABEL_DIR, dev_node_relative);
#endif
    int fd = open(dev_node_absolute, O_RDWR);
    ioctl(fd, BLKGETSIZE64, &fd_size);
    
    mmc_mapping = mmap(NULL, fd_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmc_mapping == MAP_FAILED) {
        printf("Failed to map mmc\n");
        goto END;
    }

    memcpy(mmc_mapping + start, var, size);

    printf("sync data in memory to file\n");
    ret = msync(mmc_mapping, fd_size, MS_SYNC);
    if (ret < 0)
    {
        printf("Failed to sync to file(%d)\n", ret);
        goto END;
    }

    ret = munmap(mmc_mapping, fd_size);
    if (ret < 0)
    {
        printf("Failed to unmap mmc(%d)\n", ret);
        goto END;
    }

    ret = 0;

END:
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }

    return ret;
}

int mmc_write_from_file(char *partition_name, uint64_t start, uint64_t size, char *input_filename)
{
    int ret = -1;
    void *input_raw = NULL;
    FILE *input_file = NULL;

    input_file = fopen(input_filename, "rb");
    if (input_file == NULL) {
        return ret;
    }

    input_raw = malloc(size);
    if (input_raw == NULL) {
        return ret;
    }
    
    if (fread(input_raw, 1, size, input_file) != size) {
        ret = -1;
    } else {
        if (mmc_write_from_var(partition_name, start, size, input_raw) != 0) {
            ret = -1;
        } else {
            ret = 0;
        }
    }
    fclose(input_file);
    free(input_raw);
    
    return ret;
}

int mmc_write_cal_from_file(uint64_t start, uint64_t size, char *input_filename)
{
    return mmc_write_from_file(PARTITION_CAL_DATA, start, size, input_filename);
}

/* define ioctl operations to support ecnt_sys */
int mmc_io_control(char *partition_name, char*operation, uint64_t start, uint64_t size, char *filename)
{
    if (strcmp(operation, "fetch") == 0) {
        return mmc_read_to_file(partition_name, start, size, filename);
    } else if (strcmp(operation, "store") == 0) {
        return mmc_write_from_file(partition_name, start, size, filename);
    } else if (strcmp(operation, "del") == 0) {
        return mmc_del(partition_name, start, size);
    } else if (strcmp(operation, "flip") == 0) {
        return mmc_cal_flip_byte(start);
    } else {
        return -1;
    }
}

int mmc_cal_io_control(char*operation, uint64_t start, uint64_t size, char *filename)
{
    return mmc_io_control(PARTITION_CAL_DATA, operation, start, size, filename);
}

int mmc_del(char *partition_name, uint64_t start, uint64_t size)

{
    void *null_data = NULL;

    null_data = malloc(size);
    if (null_data == NULL) {
        return -1;
    }
    memset(null_data, 0, size);
    if (mmc_write_from_var(partition_name, start, size, null_data) != 0) {
        free(null_data);
        return -1;
    }

    free(null_data);
    return 0;
}

int mmc_cal_del(uint64_t start, uint64_t size)
{
    return mmc_del(PARTITION_CAL_DATA, start, size);
}

int mmc_flip_byte(char *partition_name, uint64_t start)
{
    char var = 0;
    if (mmc_read_to_var(partition_name, start, 1, &var) != 0) {
        return -1;
    }
    if (var == '1') {
        var = '0';
    } else {
        var = '1';
    }
    if (mmc_write_from_var(partition_name, start, 1, &var) != 0) {
        return -1;
    }
    return 0;
}

int mmc_cal_flip_byte(uint64_t start)
{
    return mmc_flip_byte(PARTITION_CAL_DATA, start);
}

