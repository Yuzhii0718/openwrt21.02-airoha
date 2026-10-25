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
#include "tcversion.h"
#include "uapi/flash_layout/libapi_lib_prolinecmd.h"


static uint32_t cmd_do_init(void);
static uint32_t cmd_do_get(char *key, uint32_t key_len);
static uint32_t cmd_do_delete(char *key, uint32_t key_len);
static uint32_t cmd_do_set(char *key, uint32_t key_len, char *val, uint32_t val_len);
static uint32_t cmd_do_help_print(int argc, char *argv[]);
static uint32_t cmd_interact(int argc, char *argv[]);


const cmd_flag_list g_cmd_flag_list[] =
{
	{"productclass",		cmd_interact, 		PL_CP_PRODUCTCLASS, 		NULL},/*set or get productclass*/	
	{"manufacturerOUI",		cmd_interact, 		PL_CP_MANUFACUREROUI, 		NULL},/*set or get manufacturerOUI*/ 
	{"serialnum",			cmd_interact, 		PL_CP_SERIALNUM, 			NULL},/*set or get serialnum*/
	{"hwver", 				cmd_interact, 		PL_CP_HW_VER, 				NULL},/*set or get hwVersion */
	{"swver", 				cmd_interact, 		PL_CP_SW_VER, 				NULL},/*set or get swVersion */
	{"xponsn", 				cmd_interact, 		PL_CP_XPONSN, 				NULL},/*set or get XPON serialnumber*/
	{"xponmode", 			cmd_interact, 		PL_CP_XPONMODE, 			NULL},/*set or get xpon mode*/
	{"fstype", 				cmd_interact, 		PL_CP_FSTYPE, 				NULL},/*set or get fs type*/
	{"portmap", 			cmd_interact, 		PL_CP_PORTMAP, 				NULL},/*set or get port map*/
	{"portlist", 			cmd_interact, 		PL_CP_PORTLIST, 			NULL},/*set or get port list*/
	{"clear", 				cmd_interact, 		PL_CP_CLEAR, 				NULL},/*clear para*/ 	
	{"clearall",			cmd_interact,		PL_CP_CLEARALL,				NULL},
	{NULL,					NULL,				PL_CP_END, 					NULL},
};

int find_index(char *key,int* index)
{
	int flag_i = -1;
	int iret = PLFAIL;
	for (flag_i = 0; flag_i < CMD_FLAG_LIST_CNT; ++flag_i) {
        if (strcmp(key, g_cmd_flag_list[flag_i].name) == 0) {
            *index = flag_i;
            iret = PLSUCCESS;
            break;
        }
    } 
	return iret;
}

void do_help()
{
	int index = 0;
	printf("prolinecmd:only the parameters below can be set");
	for(index=0; index < CMD_FLAG_LIST_CNT;index++)
	{
		if((index % 4) == 0)
			printf("\n");
		printf("%-19s", g_cmd_flag_list[index].name);		
	}
	printf("\n");
}

int execute_cmd(int argc, char* argv[])
{
	int index = 0;
	int ret = PLFAIL;

	if ( CMD_FLAG_LIST_CNT > MAX_CMD_LIST_CNT )
	{
		printf("prolinecmd: cmds over range, max support %d cmds.\n", CMD_FLAG_LIST_CNT);
		return -1;
	}

	if(argc<= 1)
		do_help();
	else
	{
		for(index=0; index < CMD_FLAG_LIST_CNT;index++)
		{
			if (strcmp(g_cmd_flag_list[index].name, argv[1]) == 0)
			{
				g_cmd_flag_list[index].func(argc, argv);
				ret = 0;
				break;
			}
		}
		if (index == CMD_FLAG_LIST_CNT)
			do_help();
	}
	
	return ret;
}


static uint32_t cmd_do_init()
{
    uint32_t iret = proline_init();
    if (iret == PLSUCCESS) {
        printf("Prolinecmd init OK.\r\n");
    } else {
        printf("Prolinecmd init Error.\r\n");
    }
    return iret;
}

static uint32_t cmd_do_get(char *key, uint32_t key_len)
{
    char val[CMD_CMD_BUF_LEN] = {0};
	uint32_t iret = PLFAIL;
	int index = -1;
	if (find_index(key, &index) == 0)
	{
		
		iret = proline_read(index, val);
    
	    if (iret == PLSUCCESS) {
	        printf("Prolinecmd get OK.\r\n");
	        printf("Value=%s\r\n", val);
	    } else {
	        printf("Prolinecmd get Error.\r\n");
	    }
	}
	else
		printf("Prolinecmd not found %s.\r\n",key);
    return iret;
}

static uint32_t cmd_do_delete(char *key, uint32_t key_len)
{
	int index = -1;
    uint32_t iret = PLFAIL;
    if (find_index(key, &index) == 0)
    {
    	iret = proline_remove(index);
    }
    if (iret == PLSUCCESS) {
        printf("Prolinecmd delete OK.\r\n");
    } else {
        printf("Prolinecmd delete Error.\r\n");
    }
    return iret;
}
    
static uint32_t cmd_do_set(char *key, uint32_t key_len, char *val, uint32_t val_len)
{
	int index = -1;
    uint32_t iret = PLFAIL;
	if (find_index(key, &index) == PLSUCCESS)
    {
    	iret = proline_write(index, val, val_len);
    }
    if (iret == PLSUCCESS) {
        printf("Prolinecmd set OK.\r\n");
    } else {
        printf("Prolinecmd set Error.\r\n");
    }
    return iret;
}

static uint32_t cmd_do_help_print(int argc, char *argv[])
{
    printf("Usage: \r\n");
    printf("\tprolinecmd clearall\t\t\t clear all prolinecmd info from flash.\r\n");
    printf("\tprolinecmd <flag> get\t\t\t get the value based on flag.\r\n");
    printf("\tprolinecmd <flag> set <value>\t\t set the content of flag as \"value\".\r\n\r\n");
    return 0;
}

static uint32_t cmd_interact(int argc, char *argv[])
{
    uint32_t iret = PLCMD_INVALID;
	int i = 0;
    switch(argc) {

        case 2:
            if (strcmp(argv[1], "clearall") == 0) {
                iret = cmd_do_init();
            }
            break;
        case 3:
            if (strcmp(argv[2], "get") == 0) {
                iret = cmd_do_get(argv[1], strlen(argv[1]));
            } else if (strcmp(argv[2], "delete") == 0) {
                iret = cmd_do_delete(argv[1], strlen(argv[1]));
            } else if ((strcmp(argv[1], "clear") == 0) && (strcmp(argv[2], "1") == 0)) {
                iret = cmd_do_init();
            }
            break;
        case 4:
            if (strcmp(argv[2], "set") == 0) {
                iret = cmd_do_set(argv[1], strlen(argv[1]), argv[3], strlen(argv[3]));
            }
            break;
        default:
            break;
            
    }

    if (iret != PLSUCCESS) {
        if(iret == PLCMD_INVALID) {
            printf("Error command format.\r\n\r\n");
            cmd_do_help_print(argc, argv);
        }
    } else {
        printf("\r\nDone!\r\n");
    }
    
    return iret;
}

int main(int argc, char *argv[]) 
{
	int result = 0;
	result = execute_cmd(argc, argv);
	
	return result;
}

