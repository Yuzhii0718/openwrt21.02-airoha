/******************************************************************************/
/*
 * Copyright (C) 1994-2008 TrendChip Technologies, Corp.
 * All Rights Reserved.
 *
 * TrendChip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of TrendChip Technologies, Corp. and
 * shall not be reproduced, copied, disclosed, or used in whole or
 * in part for any reason without the prior express written permission of
 * TrendChip Technologies, Corp.
 */
/******************************************************************************/

#ifndef _UAPI_PROLINECMD_H
#define _UAPI_PROLINECMD_H

#include "stdint.h"
#include "uapi/flash_layout/libapi_lib_prolinecmd_flag.h"

#define CMD_ATTR_NAME_LEN (20)

#define CMD_FLAG_LIST_CNT PL_CP_END
#define CMD_INFO_SIZE_MAX (4096)
#define CMD_CMD_BUF_LEN (128)
#define MAX_CMD_LIST_CNT (512)

#define PROLINECMD_CWMPPARA_FILE  "/tmp/prolinecmd_io.log"

#define PROLINECMD_MAGIC 0x12344321
enum WRITE_CMD {
    WRITE_INDEX = 1,
    WRITE_INFO,
    WRITE_ALL,
    WRITE_MSG
};
enum READ_CMD {
    READ_INDEX = 1,
    READ_INFO,
    READ_ALL,
    READ_MSG
};

enum OP_LOCKER {
    LOCKED = 0,
    UNLOCKED,
};

typedef struct cmd_index {
#ifdef LOCAL_TESTER
    char name[CMD_ATTR_NAME_LEN];
#endif
    uint32_t offset;
    uint32_t len;
} cmd_index;

typedef struct cmd_index_list {
    uint32_t magic;
    uint32_t count;
    uint32_t info_len;
    int32_t crc;
    cmd_index cmd_index_list[MAX_CMD_LIST_CNT];
} cmd_index_list;

typedef struct cmd_index_list_desc {
    uint32_t magic;
    uint32_t count;
    uint32_t info_len;
    int32_t crc;
} cmd_index_list_desc;


typedef struct cmd_info {
    char *value;
} cmd_info;

typedef struct cmd_info_list {
    char cmd_info_list[CMD_INFO_SIZE_MAX];
} cmd_info_list;


typedef struct cmd_msg_block {
    cmd_index_list index;
    cmd_info_list info;
} cmd_msg_block;

#define CMD_INDEX_SIZE ( sizeof(cmd_index_list) )
#define CMD_INFO_OFFSET CMD_INDEX_SIZE
#define CMD_MAX_LOAD_SIZE ( sizeof(cmd_msg_block) )

uint32_t proline_remove(int index);
uint32_t proline_read(int index, char *value);
uint32_t proline_read_by_index(uint32_t index_id, char *value);
uint32_t proline_read_by_index_list(uint32_t *index_id_list, uint32_t index_id_list_cnt, char value_list[][CMD_CMD_BUF_LEN]);
uint32_t proline_write(int index, char *val, uint32_t val_len);
uint32_t proline_init();
uint32_t lib_prolinecmd_get(int flag, char *value, int length);
uint32_t lib_prolinecmd_set(int flag, char* value);
uint32_t lib_prolinecmd_delete(int flag);
uint32_t lib_prolinecmd_clearall();

#endif

