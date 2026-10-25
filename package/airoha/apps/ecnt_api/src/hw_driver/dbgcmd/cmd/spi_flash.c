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

/************************************************************************
*                  I N C L U D E S
************************************************************************/
#include <stddef.h>
#include <libapi_lib_utility.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/
extern const cmds_t spi_flash_get_command[];
extern const cmds_t spi_flash_set_command[];

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/

/*********************** gponmgr_root_command **************************/
static int do_set(int argc, char* argv[], void *p);
static int do_get(int argc, char* argv[], void *p);

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/
static const cmds_t spi_flash_root_command[] = {
/*	name,		func, 			flags,	argcmin,	argc_errmsg */
	{"set",		do_set,			0,		1,			"Set spi flash driver status"},
	{"get",		do_get,			0,		1,			"Get spi flash driver status"},
	/* Cmd should be added before this line!  */
	{"help",	api_lib_help,	0,		0,			"Show spi flash usage."},
	{NULL, NULL, 0, 0, NULL}
};

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/******************************************************************************
 Descriptor:	It's used to set spi flash driver status.
 Input Args:	see spi_flash_set_command.
 Ret Value:	none.
******************************************************************************/
static int do_set(int argc, char* argv[], void *p)
{
#if API_CMD_GPONMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	return subcmd(spi_flash_set_command, argc, argv, (void *)spi_flash_set_command);
}

/******************************************************************************
 Descriptor:	It's used to get spi flash driver status.
 Input Args:	see spi_flash_get_command.
 Ret Value:	none.
******************************************************************************/
static int do_get(int argc, char* argv[], void *p)
{
#if API_CMD_GPONMGR_DBG
	printf("enter %s.\n", __func__);
#endif

	return subcmd(spi_flash_get_command, argc, argv, (void *)spi_flash_get_command);
}

/******************************************************************************
 Descriptor:	It's used to config spi flash driver.
 Input Args:	see spi_flash_root_command
 Ret Value: Success: ECONET_E_SUCCESS
                      Fail: ECONET_E_FAIL
******************************************************************************/
int DoSpiFlashCmd (int argc, char* argv[], void *p)
{
	return subcmd(spi_flash_root_command, argc, argv, (void *)spi_flash_root_command);
}
