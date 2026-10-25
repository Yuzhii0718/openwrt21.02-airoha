/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libapi_lib_utility.h>
#include <libapi_lib_wifimgr.h>

#include "wifimgr.h"
#include "wifimgr_set.h"
#include "wifimgr_get.h"


static int do_init(int argc, char* argv[],void *p);
static int do_set(int argc, char* argv[],void *p);
static int do_get(int argc, char* argv[],void *p);
static int do_save(int argc, char* argv[],void *p);
static int do_commit(int argc, char* argv[],void *p);
static int do_exit(int argc, char* argv[],void *p);
static int do_dump(int argc, char* argv[],void *p);
static int do_write_calibration(int argc, char* argv[],void *p);
static int do_read_calibration(int argc, char* argv[],void *p);




/*******************************
*
********************************/
static const cmds_t wifimgr_root_cmd[]=
{	//cmd					fun					flag 	  argunum		description
	{"init",				do_init,			0,		1,		"ready to set or get parameters"},
	{"dump",				do_dump,			0,		1,		"print shared memory data"},
	{"write_calibration",	do_write_calibration,	0,		1,		"Write calibration infomation to flash"},
	{"read_calibration",	do_read_calibration,	0,		1,		"Read calibration infomation from flash"},
	{"set",					do_set,				0,		2,		"set wlan parameters or debug level"},
	{"get",					do_get,				0,		3,		"get wlan parameters"},
	{"save",				do_save,			0,		1,		"save wlan parameters"},
	{"commit",				do_commit,			0,		1,		"make settings take effect"},
	{"exit",				do_exit,			0,		0,		"free the share memory"},
	{"help",				api_lib_help,		0,		0,		"Show wifimgr usage."},
	
	{NULL,				NULL,				0,		0,		NULL}
};


static int do_init(int argc, char* argv[],void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
			printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
    if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_init(type))
    {
		printf("Initialize the share memory failly!\n");
	}

	return 0;
}

static int do_dump(int argc, char* argv[],void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
    if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_dump(type))
    {
		printf("Print the share memory failly!\n");
	}

	return 0;
}

static int do_write_calibration(int argc, char* argv[],void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wifi_calibration(type))
	{
		printf("Create the calibration file failed!\n");
	}

	return 0;
}

static int do_read_calibration(int argc, char* argv[],void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_wifi_calibration(type))
	{
		printf("Create the calibration file failed!\n");
	}

	return 0;
}


static int do_set(int argc, char* argv[],void *p)
{	
	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
    return subcmd(wifimgr_set_cmd, argc, argv, (void *)wifimgr_set_cmd);
}

static int do_get(int argc, char* argv[],void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif	
	return subcmd(wifimgr_get_cmd, argc, argv, (void *)wifimgr_get_cmd);
}

static int do_save(int argc, char* argv[],void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif	
	wifi_type type = atoi(argv[1]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_save_dataFile(type))
	{
		printf("save parameter failed");
		return -1;
	}
	else  return 0;
	
}


static int do_commit(int argc, char* argv[],void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_commit(type,index))
	{
		printf("set parameter failly");
		return -1;
	}
	else
	{
		return 0;
	}	
}

static int do_exit(int argc, char* argv[],void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	#if 0
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_exit())
    {
		printf("Free the share memory failly!\n");
	}
	#endif
	return FINSH_FLAG;
}

int 
cmdline_parser(char *cmdline, int *num, char **cmd)
{
    int cnt = 1, pos = 0;
	int issetSSIDflag = 0;
    enum{SEARCH_ARG, HIT_ARG} state = SEARCH_ARG;

    for(pos = 0; cmdline[pos] != '\0'; ++pos)
    {
        switch(state)
        {
            case SEARCH_ARG:
				if(cnt == MAX_NUM_ARG - 1 && strcmp(cmd[1], "set")==0 && 
						(strcmp(cmd[2], "SSID")==0 || strcmp(cmd[2], "WPAPSK")==0))
				{
					issetSSIDflag = 1;
				}
                else if('\x20' != cmdline[pos])
                {
                    state = HIT_ARG;
                    cmd[cnt] = & cmdline[pos];
                }
                break;

            case HIT_ARG:
				if('\x20' == cmdline[pos] || '\n' == cmdline[pos])
                {
					cmdline[pos] = 0;
                    state = SEARCH_ARG;
                    ++ cnt;
                    if(cnt > MAX_NUM_ARG -1)
                    {
						*num = cnt;
						return ECNT_WIFIMGR_SUCCESS;
                    }
                }
                break;

            default:
                return ECNT_WIFIMGR_FAILURE;
        }
		if(issetSSIDflag == 1)
			break;
    }

	if(issetSSIDflag == 1)
	{
		if(cmdline[pos] != '\0')
		{
			cmd[cnt] = & cmdline[pos];
			cnt ++;
		}
		else 
			return ECNT_WIFIMGR_FAILURE;
		while('\0' != cmdline[pos])
		{
			++ pos;
		}
		cmdline[pos] = 0;
	}

    *num = cnt;
    return ECNT_WIFIMGR_SUCCESS;
}


int main(int argc, char* argv[])
{
	char cmdline[MAX_KEYWORD_LEN];
    char* value[MAX_PARA_NUM];	   //which is used to save para
    int count = 0;                 //the num of para
	int result = 0;
	
    while(1){
		
        printf("\nWIFI_API>>");
		
        fgets(cmdline, MAX_KEYWORD_LEN, stdin);

        if(ECNT_WIFIMGR_SUCCESS != cmdline_parser(cmdline,&count,value))
		{
            printf("The number of command parameters is wrong!\n");
            return ECNT_WIFIMGR_CMD_ERR;
        }
      
        result = subcmd(wifimgr_root_cmd, count, value, (void*)wifimgr_root_cmd);
        memset(cmdline, 0, MAX_KEYWORD_LEN);
		if(result == FINSH_FLAG)
			break;
           
    }
	
    return ECNT_WIFIMGR_SUCCESS;

}


