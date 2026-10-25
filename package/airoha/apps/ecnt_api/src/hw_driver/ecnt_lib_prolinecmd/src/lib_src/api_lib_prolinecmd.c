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
 * cJSON based ProLineCMD implementation.
 * Designed to do prolinecmd info read/write/update/remove ops in flash.
 * 
 * NOTE: it is the caller's responsibility to guarantee that all the function
 * parameters must NOT be NULL, unless it is designed to be.
 
 * author: mtk23399
 * version: 0.0.1
 * date: Oct. 15, 2020
 *
 * version: 1.0.1
 * date: Oct. 21, 2020
 * Modify: redesign of data structure.
 *************************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <flash_layout/tc_partition.h>
#include "uapi/flash_layout/libapi_lib_prolinecmd.h"
#include "libapi_lib_utility.h"

static cmd_index_list *g_cmd_index_list_p;
static cmd_info_list *g_cmd_info_list_p;
uint32_t g_cmd_info_size;
void *g_msg_block_p;
uint32_t g_table_locker;
static uint32_t proline_variable_clear();
static uint32_t proline_init_without_io();
static uint32_t proline_write_to_all_lists(int index_id, char *val, uint32_t val_len);
static uint32_t proline_write_to_list_index(uint32_t index, char *key, uint32_t key_len, char *val, uint32_t val_len);
static uint32_t proline_write_to_list_info(uint32_t index, char *val, uint32_t val_len);
static uint32_t proline_write_without_io(int index, char *val, uint32_t val_len);
static uint32_t proline_remove_without_io(int index);
static uint32_t proline_read_from_list_index(uint32_t index, uint32_t *offset, uint32_t *len);
static uint32_t proline_read_from_list_info(uint32_t offset, uint32_t len, char *value);
static uint32_t proline_crc32(char *buf, uint32_t buf_len, int32_t *crc);
static uint32_t proline_read_without_io(uint32_t index_id, char *value);

static uint32_t proline_direct_msg_load();
static uint32_t proline_direct_load(uint32_t mode);
static uint32_t proline_direct_msg_dump();
static uint32_t proline_direct_dump(uint32_t mode);

void proline_print_index(char *caller, cmd_index index);
void proline_print_index_list(char *caller);
void proline_print_info_list(char *caller);
void proline_print_msg_table(char *caller, cmd_msg_block block);

uint32_t proline_load();
uint32_t proline_dump();

uint32_t lib_prolinecmd_get(int index, char *value, int length)
{
	uint32_t ret = PLFAIL;
	char val[CMD_CMD_BUF_LEN] = {0};
	int less_len = CMD_CMD_BUF_LEN < length ? CMD_CMD_BUF_LEN : length;
	if (index < PL_CP_END && index >= 0)
	{
		ret = proline_read(index, val);
		strncpy(value, val, less_len - 1);
	}
	return ret;
}

uint32_t lib_prolinecmd_set(int index, char* value)
{
	uint32_t ret = PLFAIL;
	if (strlen(value) >= CMD_CMD_BUF_LEN)
		return ret;
	if (index < PL_CP_END && index >= 0)
		ret = proline_write(index, value, strlen(value));
	return ret;
}

uint32_t lib_prolinecmd_delete(int index)
{
	uint32_t ret = PLFAIL;
	if (index < PL_CP_END && index >= 0)
		ret = proline_remove(index);
	return ret;
}

uint32_t lib_prolinecmd_clearall()
{
	return proline_init();
}
	


/* Init all structure without flash update */
static uint32_t proline_variable_clear()
{
    uint32_t iret = PLSUCCESS;

    g_table_locker = LOCKED;
    g_cmd_index_list_p = g_msg_block_p;
    g_cmd_info_list_p = g_msg_block_p + CMD_INFO_OFFSET;
    g_cmd_index_list_p->count = 0;
    g_table_locker = UNLOCKED;

    return iret;
}

static uint32_t proline_init_without_io()
{
    uint32_t iret = PLSUCCESS;
#ifdef LOCAL_TESTER
    uint32_t i;
#endif
    
    proline_variable_clear();
    g_cmd_index_list_p->count = MAX_CMD_LIST_CNT;
    g_cmd_index_list_p->magic = PROLINECMD_MAGIC;
#if 0//def LOCAL_TESTER
    for (i = 0; i < CMD_FLAG_LIST_CNT; ++i) {
        strcpy(g_cmd_index_list_p->cmd_index_list[i].name, g_cmd_flag_list[i].name);
    }
#endif

    return iret;
}

/* Init data structure and write to flash */
uint32_t proline_init()
{
    uint32_t iret = 0;
    /* prepare memory space to store global variables */
    g_msg_block_p = malloc(CMD_MAX_LOAD_SIZE);
    if (g_msg_block_p == NULL) {
        return PLMALLOC_FAIL;
    }
    memset(g_msg_block_p, 0, CMD_MAX_LOAD_SIZE);

    if (proline_init_without_io() != PLSUCCESS) {
        printf("Failed to init prolinecmd.\r\n");
    }

    iret = proline_dump();
    free(g_msg_block_p);

    return iret;
}

static uint32_t proline_update_index_offset(uint32_t start_id, int32_t gap)
{
    uint32_t i = start_id + 1;
    
    /* Update offset */
    while (i < MAX_CMD_LIST_CNT) {
        g_cmd_index_list_p->cmd_index_list[i++].offset += gap;
    }

    return PLSUCCESS;
}

static uint32_t proline_write_to_list_info(uint32_t index, char *val, uint32_t val_len)
{
    uint32_t iret = PLFAIL;
    char *p0 = NULL, *p1 = NULL, *p2 = NULL, *tmp = NULL;

    p0 = g_cmd_info_list_p->cmd_info_list;
    p1 = p0 + g_cmd_index_list_p->cmd_index_list[index].offset;
    p2 = p1 + g_cmd_index_list_p->cmd_index_list[index].len;

    if ((g_cmd_info_size + val_len - (p2 - p1)) < CMD_INFO_SIZE_MAX) {
        tmp = malloc(CMD_INFO_SIZE_MAX);
        if (tmp == NULL) {
            return PLMALLOC_FAIL;
        }
        memset(tmp, 0, CMD_INFO_SIZE_MAX);

        strncpy(tmp, p0, (p1 - p0));
        strncat(tmp, val, val_len);
        strncat(tmp, p2, g_cmd_info_size - (p2 - p0));
        memset(g_cmd_info_list_p->cmd_info_list, 0, CMD_INFO_SIZE_MAX);

        g_cmd_info_size = g_cmd_info_size + val_len - (p2 - p1);
        strncpy(g_cmd_info_list_p->cmd_info_list, tmp, g_cmd_info_size);
        free (tmp);
        
        iret = PLSUCCESS;
    } else {
        printf("Invalid value size(=%d, g_info_size=%d, total=%d).\r\n", val_len, g_cmd_info_size, g_cmd_info_size + val_len - (p2 - p1));
    }

    return iret;
}

static uint32_t proline_write_to_list_index(uint32_t index, char *key, uint32_t key_len, char *val, uint32_t val_len)
{    
    int32_t gap;

    if (index == 0) {
        g_cmd_index_list_p->cmd_index_list[index].offset = 0;
    } else {
        g_cmd_index_list_p->cmd_index_list[index].offset = g_cmd_index_list_p->cmd_index_list[index - 1].offset + 
                                                        g_cmd_index_list_p->cmd_index_list[index - 1].len;
    }
#ifdef LOCAL_TESTER
    strncpy(g_cmd_index_list_p->cmd_index_list[index].name, key, key_len);
#endif
    gap = val_len - g_cmd_index_list_p->cmd_index_list[index].len;
    g_cmd_index_list_p->cmd_index_list[index].len = val_len;
    g_cmd_index_list_p->info_len = g_cmd_info_size;
    return proline_update_index_offset(index, gap);
}

static uint32_t proline_write_to_all_lists(int index_id, char *val, uint32_t val_len)
{
    uint32_t iret = PLSUCCESS;
    iret |= proline_write_to_list_info(index_id, val, val_len);
    iret |= proline_write_to_list_index(index_id, NULL, 0, val, val_len);        

    if (iret != PLSUCCESS) {
        printf("Error, failed to write info to tables, info may lose sync.\r\n");
    }

    return iret;
}

static uint32_t proline_write_without_io(int index, char *val, uint32_t val_len)
{
    return proline_write_to_all_lists(index, val, val_len);
}

uint32_t proline_write(int index, char *val, uint32_t val_len)
{
    uint32_t iret = 0;
    
    /* prepare memory space to store global variables */
    g_msg_block_p = malloc(CMD_MAX_LOAD_SIZE);
    if (g_msg_block_p == NULL) {
        return PLMALLOC_FAIL;
    }
    memset(g_msg_block_p, 0, CMD_MAX_LOAD_SIZE);
	
    if (proline_load() != PLSUCCESS) {
        printf("Failed to load historical info.\r\n");
        return PLFAIL;
    }
    if (proline_write_without_io(index, val, val_len) != PLSUCCESS) {
        printf("Failed to write index(=%d).\r\n", index);
        return PLFAIL;
    }
    iret = proline_dump();
    free(g_msg_block_p);
    return iret;
}

static uint32_t proline_remove_without_io(int index)
{
    return proline_write_to_all_lists(index, NULL, 0);
}

uint32_t proline_remove(int index)
{
    uint32_t iret = 0;
    /* prepare memory space to store global variables */
    g_msg_block_p = malloc(CMD_MAX_LOAD_SIZE);
    if (g_msg_block_p == NULL) {
        return PLMALLOC_FAIL;
    }
    memset(g_msg_block_p, 0, CMD_MAX_LOAD_SIZE);

    if (proline_load() != PLSUCCESS) {
        printf("Failed to load historical info.\r\n");
        return PLFAIL;
    }    
    if (proline_remove_without_io(index) != PLSUCCESS) {
        printf("Failed to remove index(=%d).\r\n", index);
        return PLFAIL;
    }

    iret = proline_dump();    
    free(g_msg_block_p);

    return iret;
}

static uint32_t proline_read_from_list_index(uint32_t index, uint32_t *offset, uint32_t *len)
{
    uint32_t iret = PLFAIL;
    
    if (index < CMD_FLAG_LIST_CNT) {
        *offset = g_cmd_index_list_p->cmd_index_list[index].offset;
        *len = g_cmd_index_list_p->cmd_index_list[index].len;
        iret = PLSUCCESS;
    } else {
        printf("Invalid index(=%d), MAX=%d.\r\n", index, CMD_FLAG_LIST_CNT);
    }
    if (*len == 0) {
        iret = PLNO_VALUE;
    }
    return iret;
}

static uint32_t proline_read_from_list_info(uint32_t offset, uint32_t len, char *value)
{
    uint32_t iret = PLSUCCESS;

    if (g_cmd_info_size < (offset + len)) {
        printf("Invalid offset(=%d) or len(=%d). Total strlen(=%d).\r\n", offset, len, g_cmd_info_size);
        return iret;
    }
    strncpy(value, g_cmd_info_list_p->cmd_info_list + offset, len);
    return iret;
}

/* Read value by key without flash update */
static uint32_t proline_read_without_io(uint32_t index_id, char *value) 
{
    uint32_t iret = PLFAIL;
    
    uint32_t offset = 0, len = 0;
    if ((iret = proline_read_from_list_index(index_id, &offset, &len)) != PLSUCCESS) {
        if (iret != PLNO_VALUE) {
            printf("Failed to read index(=%d) from table_index.\r\n", index_id);
        }
        return iret;
    }

    if((iret = proline_read_from_list_info(offset, len, value)) != PLSUCCESS) {
        printf("Failed to read info(offset=%d, len=%d) from table_info(=%s).\r\n", offset, len, 
                                                                                   g_cmd_info_list_p->cmd_info_list);
        return iret;
    }

    return iret;
}

uint32_t proline_read_by_index(uint32_t index_id, char *value)
{
    uint32_t iret = PLFAIL;
    uint32_t offset = 0, len = 0;

    if ((iret = proline_read_from_list_index(index_id, &offset, &len)) != PLSUCCESS) {
        if (iret != PLNO_VALUE) {
            printf("Failed to read index(=%d) from list_index.\r\n", index_id);
        }
        return iret;
    }
    if((iret = proline_read_from_list_info(offset, len, value)) != PLSUCCESS) {
        printf("Failed to read info(offset=%d, len=%d) from list_info(=%s).\r\n", offset, len, 
                                                                                   g_cmd_info_list_p->cmd_info_list);
    }

    return iret;
}

uint32_t proline_read_by_index_list(uint32_t *index_list, uint32_t index_id_list_cnt, char value_list[][CMD_CMD_BUF_LEN])
{
   uint32_t iret, i, index = 0;
	char* value = NULL;
    /* prepare memory space to store global variables */
    g_msg_block_p = malloc(CMD_MAX_LOAD_SIZE);
    if (g_msg_block_p == NULL) {
        return PLMALLOC_FAIL;
    }
    memset(g_msg_block_p, 0, CMD_MAX_LOAD_SIZE);

    if((iret = proline_load()) != PLSUCCESS) {
		free(g_msg_block_p);
        printf("Failed to do flash read.\r\n");
        return iret;
    }
	for(i=0; i < index_id_list_cnt; i++)
	{
		index = index_list[i];
		value = value_list[i];
	    if((iret = proline_read_without_io(index, value)) != PLSUCCESS) {
	        if(iret == PLNO_VALUE) {
	            printf("(index=%d) has no value.\r\n", index);
	        } else {
	            printf("Failed to get (index=%d).\r\n", index);
	        }
	    }
	}
    free(g_msg_block_p);
	return iret;
}


uint32_t proline_read(int index, char *value) 
{
    uint32_t iret;
    
    /* prepare memory space to store global variables */
    g_msg_block_p = malloc(CMD_MAX_LOAD_SIZE);
    if (g_msg_block_p == NULL) {
        return PLMALLOC_FAIL;
    }
    memset(g_msg_block_p, 0, CMD_MAX_LOAD_SIZE);

    if((iret = proline_load()) != PLSUCCESS) {
		free(g_msg_block_p);
        printf("Failed to do flash read.\r\n");
        return iret;
    }
    if((iret = proline_read_without_io(index, value)) != PLSUCCESS) {
        if(iret == PLNO_VALUE) {
            printf("(index=%d) has no value.\r\n", index);
        } else {
            printf("Failed to get (index=%d).\r\n", index);
        }
    }
    
    free(g_msg_block_p);

    return iret;
}

static uint32_t proline_crc32(char *buf, uint32_t buf_len, int32_t *crc)
{
    int32_t i, j;
    int32_t byte, crc_tmp, mask;
    i = 0;
    crc_tmp = 0xFFFFFFFF;
    while (i < buf_len) {
        byte = buf[i];
        crc_tmp = crc_tmp ^ byte;
        for (j = 7; j >= 0; --j) {
            mask = -(crc_tmp & 1);
            crc_tmp = (crc_tmp >> 1) ^ (0xEDB88320 & mask);
        }
        ++i;
    }
    *crc = (~crc_tmp);
    
	return PLSUCCESS;
}

static uint32_t proline_direct_msg_load()
{
    uint32_t iret = PLFAIL;
	FILE *fp = NULL;
	char cmd_buf[CMD_CMD_BUF_LEN] = {0};
    uint32_t size, entry_cnt, info_size, index_size;
    int32_t crc;
    cmd_index_list_desc header_desc;
	fp = fopen(PROLINECMD_CWMPPARA_FILE, "r");
	if(fp == NULL)
	{
		if(is_emmc_blk())
			sprintf(cmd_buf, TC_MMC_READ_CMD, PROLINE_CWMPPARA_RA_OFFSET,
					PROLINE_CWMPPARA_RA_SIZE, PROLINECMD_CWMPPARA_FILE, RESERVEAREA_NAME);
		else
			sprintf(cmd_buf, TC_MTD_READ_CMD, PROLINECMD_CWMPPARA_FILE,
					PROLINE_CWMPPARA_RA_SIZE, PROLINE_CWMPPARA_RA_OFFSET, RESERVEAREA_NAME);

		system(cmd_buf);
		fp = fopen(PROLINECMD_CWMPPARA_FILE, "r");
	}
	/* read from file */
	if (fp == NULL) 
	{
		printf("Failed to open file(%s).\r\n", PROLINECMD_CWMPPARA_FILE);
        unlink(PROLINECMD_CWMPPARA_FILE);
        iret = PLFAIL;
		return iret;
	} 
	else 
	{
        memset(g_msg_block_p, 0, CMD_MAX_LOAD_SIZE);
        size = fread(g_msg_block_p, 1, CMD_MAX_LOAD_SIZE, fp);
        if (size > CMD_MAX_LOAD_SIZE) {
            printf("Unmatched file size, read=%d, max=%d.\r\n", size, CMD_MAX_LOAD_SIZE);
            fclose(fp);
            return PLFAIL;
        }
        header_desc = *(cmd_index_list_desc *)g_msg_block_p;
        
        /* first check whether magic is valid */
        if (header_desc.magic != PROLINECMD_MAGIC) {
            printf("Invalid ProlineCMD magic number!\r\n");
            fclose(fp);
            return PLFAIL;
        }
        entry_cnt = header_desc.count;
        info_size = header_desc.info_len;
        index_size = sizeof(cmd_index_list);
        /* backward compatiable */
        if ((index_size + info_size) > CMD_MAX_LOAD_SIZE) {
#if 0
            free(g_msg_block_p);
            g_msg_block_p = malloc(index_size + info_size);
            if (g_msg_block_p == NULL) {
                fclose(fp);
				printf("%s,%d:PLMALLOC_FAIL\n", __FUNCTION__, __LINE__);
                return PLMALLOC_FAIL;
            }
            memset(g_msg_block_p, 0, index_size + info_size);
            size = fread(g_msg_block_p, 1, index_size + info_size, fp);
#else
			/* proline cmd counts are already reserved. 
			MAX_CMD_LIST_CNT (512)
			this case should be: proline area is not valid, (index_size + info_size) may cause outof memory,
			it should be PLFAIL.
			*/
			printf("prolinecmd cmd size over range, current size [%lu].\n", (index_size + info_size));
			fclose(fp);
			return PLFAIL;
#endif
        }
        g_cmd_index_list_p = g_msg_block_p;
        g_cmd_info_size = g_cmd_index_list_p->cmd_index_list[entry_cnt - 1].offset + 
                          g_cmd_index_list_p->cmd_index_list[entry_cnt - 1].len;
        g_cmd_info_list_p = (g_msg_block_p + index_size);
        
        proline_crc32(g_msg_block_p + sizeof(header_desc), index_size + info_size - sizeof(header_desc), &crc);
        if (crc == header_desc.crc) {
            iret = PLSUCCESS;
        } else {
            printf("Crc error: read=%d, calculated=%d.\r\n", header_desc.crc, crc);
            iret = PLFAIL;
        }
    }
	fclose(fp);
	return iret;
}

static uint32_t proline_direct_load(uint32_t mode)
{
    uint32_t iret = PLSUCCESS;

    switch (mode) {
        case READ_MSG:
            iret = proline_direct_msg_load();
            break;

        default:
            printf("Invalid flash read part(=%d).\r\n", mode);
            iret = PLCMD_NOT_FOUND;
            break;
    }

    return iret;
}

/* Load the up-to-date content from flash to global variables */
uint32_t proline_load()
{
    return proline_direct_load(READ_MSG);
}

static uint32_t proline_direct_msg_dump()
{
    uint32_t iret = PLFAIL;
	FILE *fp;
	char cmd_buf[CMD_CMD_BUF_LEN] = {0};
    uint32_t index_size, info_size , tot_size;
    void *dump_block_p;
    int32_t crc;

    fp = fopen(PROLINECMD_CWMPPARA_FILE, "w");
    if (fp == NULL) {
        printf("Failed to open file(%s).\r\n", PROLINECMD_CWMPPARA_FILE);
        unlink(PROLINECMD_CWMPPARA_FILE);
        return iret;
    } else {
        index_size = CMD_INDEX_SIZE;
        info_size = g_cmd_info_size;
        dump_block_p = malloc(index_size + info_size);
        if (dump_block_p == NULL) {
            fclose(fp);
            return PLMALLOC_FAIL;
        }
        memset(dump_block_p, 0, index_size + info_size);
        memcpy(dump_block_p, g_cmd_index_list_p, index_size);
        memcpy(dump_block_p + index_size, g_cmd_info_list_p, info_size);
        (void)proline_crc32(dump_block_p + sizeof(cmd_index_list_desc), 
                            index_size + info_size - sizeof(cmd_index_list_desc), 
                            &crc);
        /* magic + count + info_len */
        *(int32_t *)(dump_block_p + (3 * sizeof(uint32_t))) = crc;
        fwrite(dump_block_p, index_size + info_size, 1, fp);
        free(dump_block_p);
        fclose(fp);
        tot_size=index_size + info_size;
        if(tot_size>PROLINE_CWMPPARA_RA_SIZE)
          tot_size=PROLINE_CWMPPARA_RA_SIZE;
		if(is_emmc_blk())
			sprintf(cmd_buf, TC_MMC_WRITE_CMD, PROLINE_CWMPPARA_RA_OFFSET,
				   tot_size, PROLINECMD_CWMPPARA_FILE, RESERVEAREA_NAME);
		else
			sprintf(cmd_buf, TC_MTD_WRITE_CMD, PROLINECMD_CWMPPARA_FILE,
					PROLINE_CWMPPARA_RA_SIZE, PROLINE_CWMPPARA_RA_OFFSET, RESERVEAREA_NAME);

        system(cmd_buf);
    }
    iret = PLSUCCESS;
    return iret;
}


static uint32_t proline_direct_dump(uint32_t mode)
{
    uint32_t iret = PLSUCCESS;

    switch (mode) {
        case WRITE_MSG:
            iret = proline_direct_msg_dump();
            break;

        default:
            printf("Invalid flash write part(=%d).\r\n", mode);
            iret = PLCMD_NOT_FOUND;
            break;
    }

    return iret;
}

/* Write from global variables to flash */
uint32_t proline_dump()
{
    return proline_direct_dump(WRITE_MSG);
}

#ifdef LOCAL_TESTER
void proline_print_index(char *caller, cmd_index index)
{
    printf("[%s]: name=%s, offset=%d, len=%d.\r\n", caller, index.name, index.offset, index.len);
}

void proline_print_index_list(char *caller)
{
    int i = 0;
    printf("Index_list [%s]:\r\n", caller);
    for (i = 0; i < g_cmd_index_list_p->count; ++i) {
        proline_print_index("N/A", g_cmd_index_list_p->cmd_index_list[i]);
    }
    printf("====================================\r\n");
}

void proline_print_info_list(char *caller)
{
    printf("Info_list [%s]: %s.\r\n", caller, g_cmd_info_list_p->cmd_info_list);
    printf("====================================\r\n");
}

void proline_print_msg_table(char *caller, cmd_msg_block block)
{
    int i = 0;
    printf("-------------- MSG TABLE [%s] -------------\r\n", caller);
    printf("Index_list:\r\n");
    for (i = 0; i < block.index.count; ++i) {
        proline_print_index("N/A", block.index.cmd_index_list[i]);
    }

    printf("Info_list:\r\n");
    printf("[%s]: %s.\r\n", "N/A", g_cmd_info_list_p->cmd_info_list);
    printf("====================================================\r\n");
}
#endif

