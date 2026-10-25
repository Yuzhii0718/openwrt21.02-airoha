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
 *
 * 
 * This header file contains the list of all registered prolinecmd flags
 * Add or remove from this table will affect the structure of prolinecmd msg
 * tables/lists
 */
/******************************************************************************/

#ifndef _UAPI_PROLINECMD_FLAG_H
#define _UAPI_PROLINECMD_FLAG_H

#define CMD_ATTR_NAME_LEN (20)

#define TC_MMC_READ_CMD		"/sbin/ecnt_mmc cal fetch %llu %llu %s %s"
#define TC_MMC_WRITE_CMD	"/sbin/ecnt_mmc cal store %llu %llu %s %s"

#define TC_MTD_READ_CMD		"/sbin/ecnt_mtd readflash %s %lu %lu %s"
#define TC_MTD_WRITE_CMD	"/sbin/ecnt_mtd writeflash %s %lu %lu %s"

#define RESERVEAREA_NAME "art"

typedef struct cmd_flag_list {
    char name[CMD_ATTR_NAME_LEN];
	uint32_t	(*func)(int argc,char *argv[]);
	int pl_cp_flag;
    void *path;
} cmd_flag_list;

enum PL_CP_Flags {
    PL_CP_PRODUCTCLASS=0,
    PL_CP_MANUFACUREROUI,
    PL_CP_SERIALNUM,
	PL_CP_HW_VER,
	PL_CP_SW_VER,
	PL_CP_XPONSN,
	PL_CP_XPONMODE,
	PL_CP_FSTYPE,
	PL_CP_PORTMAP,
	PL_CP_PORTLIST,
	PL_CP_CLEAR,
	PL_CP_CLEARALL,
    PL_CP_END
};


#define CMD_FLASH_SIZE  (4 * 1024)



enum IRET {
    PLSUCCESS = 0,
    PLFAIL,
    PLMALLOC_FAIL,
    PLKEY_NOT_FOUND,
    PLCMD_NOT_FOUND,
    PLNO_VALUE,
    PLCMD_INVALID
};


#endif
