/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/**
* @file  blapi_perform_cmd.c  
* @brief This program gives cmd apis from blapi_perform.
* @author Zhen.Xu
* @date     2020-07-13
* @version  A001 
* @copyright EcoNet Inc                                                              
*/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <ecnt_lib_dbg.h>
#include "blapi_perform.h"
#include "blapi_perform_cmd.h"
#include "libapi_lib_utility.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static const cmds_t blapi_perform_cmd[] = 
{
	{"set_optimize", do_blapi_perform_optimize,0,0,NULL},
	{"get_speedtest_result", do_blapi_perform_speedtest,0,0,NULL},
	{"update_cpubind_byvoipstate", do_blapi_perform_update_cpubind_byvoipstate,0,0,NULL},
	{NULL,NULL,0,0,NULL}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

int do_blapi_perform_optimize(int argc, char *argv[], void *p)
{
	int ret = 0;
	int flag = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\nblapi_cmd perform set_optimize samba/ntfs/wget 0/1\n");
		return -1;
	}

	flag = atoi(argv[2]);

	if(strcmp(argv[1], "samba") == 0)
		ret = blapi_perform_samba_optimize(flag);
	else if(strcmp(argv[1], "ntfs") == 0)
		ret = blapi_perform_ntfs_optimize(flag);
	else if(strcmp(argv[1], "wget") == 0)
		ret = blapi_perform_wget_optimize(flag);
	else
	{
		ecnt_blapi_printf("\r\n[%s]%d: please input blapi_cmd perform set_optimize samba/ntfs/wget 0/1 \n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set optimize faile\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set optimize sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_perform_update_cpubind_byvoipstate(int argc, char *argv[], void *p)
{
	int ret = 0;
	int voipIdle = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\nblapi_cmd perform update_cpubind_byvoipstate 0/1\n");
		return -1;
	}

	voipIdle = atoi(argv[1]);

	ret = blapi_perform_update_cpubind_byvoipstate(voipIdle);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set optimize faile\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set optimize sucess\n", __FUNCTION__, __LINE__);

	return 0;
}

int do_blapi_perform_speedtest(int argc, char *argv[], void *p)
{
	int ret = 0;
	speed_test_result_t test_result;

	memset(&test_result, 0, sizeof(speed_test_result_t));

	if(argc != 1)
	{
		ecnt_blapi_printf("\r\nblapi_cmd perform get_speedtest_result\n");
		return -1;
	}

	ret = blapi_perf_get_speedtest_result(&test_result);
	
	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set optimize faile\n", __FUNCTION__, __LINE__);
	else{
		ecnt_blapi_printf("\r\n[%s]%d: set optimize sucess\n", __FUNCTION__, __LINE__);
		
		ecnt_blapi_printf("Status:%s \nDW_SPEED_MAX_RATE:%s \nDW_SPEED_RATE:%s \nUP_SPEED_RATE:%s \nUP_SPEED_MAX_RATE:%s\n",
			test_result.error_state, test_result.dw_max_rate, test_result.dw_rate, test_result.up_rate,test_result.up_max_rate);
	}

	return 0;
}

int do_blapi_perform_cmd(int argc,char* argv[],void *p)
{
    return subcmd(blapi_perform_cmd,argc,argv,(void *)blapi_perform_cmd);
}

