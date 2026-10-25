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
* @file  blapi_tc_partition.c 
* @brief This file gives examples on using apis from blapi_tc_partition.h.
* @author Zhen.Xu
* @date     2019-11-26
* @version  A001 
* @copyright EcoNet Inc                                                              
*/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "blapi_tc_partition.h"
#include <flash_layout/tc_partition.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/



unsigned int tc_partition_define[MAX_DEFINE_NUM]={0};
int init_define = 0;

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

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

unsigned int get_tc_partition_value(int name)
{
	unsigned int data = 0;

	if (init_define == 0)
	{
		#ifdef CERM1_RA_SIZE
		tc_partition_define[GET_CERM1_RA_SIZE               ]=CERM1_RA_SIZE;
		#endif
		#ifdef CERM2_RA_SIZE
		tc_partition_define[GET_CERM2_RA_SIZE               ]=CERM2_RA_SIZE;
		#endif
		#ifdef CERM3_RA_SIZE
		tc_partition_define[GET_CERM3_RA_SIZE               ]=CERM3_RA_SIZE;
		#endif
		#ifdef CERM4_RA_SIZE
		tc_partition_define[GET_CERM4_RA_SIZE               ]=CERM4_RA_SIZE;
		#endif
		#ifdef CERM1_RA_OFFSET
		tc_partition_define[GET_CERM1_RA_OFFSET             ]=CERM1_RA_OFFSET;
		#endif
		#ifdef CERM2_RA_OFFSET
		tc_partition_define[GET_CERM2_RA_OFFSET             ]=CERM2_RA_OFFSET;
		#endif
		#ifdef CERM3_RA_OFFSET
		tc_partition_define[GET_CERM3_RA_OFFSET             ]=CERM3_RA_OFFSET;
		#endif
		#ifdef CERM4_RA_OFFSET
		tc_partition_define[GET_CERM4_RA_OFFSET             ]=CERM4_RA_OFFSET;
		#endif
		#ifdef BOB_RA_OFFSET_OLD_MODE
		tc_partition_define[GET_BOB_RA_OFFSET_OLD_MODE      ]=BOB_RA_OFFSET_OLD_MODE;
		#endif
		#ifdef NAND_FLASH_BLOCK_SIZE
		tc_partition_define[GET_NAND_FLASH_BLOCK_SIZE       ]=NAND_FLASH_BLOCK_SIZE;
		#endif
		#ifdef EEPROM_RA_AC_SIZE
		tc_partition_define[GET_EEPROM_RA_AC_SIZE           ]=EEPROM_RA_AC_SIZE;
		#endif
		#ifdef EEPROM_RA_AC_RESERVE_SIZE
		tc_partition_define[GET_EEPROM_RA_AC_RESERVE_SIZE   ]=EEPROM_RA_AC_RESERVE_SIZE;
		#endif
		#ifdef RESERVEAREA_ERASE_SIZE
		tc_partition_define[GET_RESERVEAREA_ERASE_SIZE      ]=RESERVEAREA_ERASE_SIZE;
		#endif
		#ifdef RESERVEAREA_TOTAL_SIZE
		tc_partition_define[GET_RESERVEAREA_TOTAL_SIZE      ]=RESERVEAREA_TOTAL_SIZE;
		#endif
		#ifdef BACKUPROMFILE_RA_SIZE
		tc_partition_define[GET_BACKUPROMFILE_RA_SIZE       ]=BACKUPROMFILE_RA_SIZE;
		#endif
		#ifdef BACKUPROMFILE_RA_OFFSET
		tc_partition_define[GET_BACKUPROMFILE_RA_OFFSET     ]=BACKUPROMFILE_RA_OFFSET;
		#endif
		#ifdef TEMP_RA_SIZE
		tc_partition_define[GET_TEMP_RA_SIZE                ]=TEMP_RA_SIZE;
		#endif
		#ifdef TEMP_RA_OFFSET
		tc_partition_define[GET_TEMP_RA_OFFSET              ]=TEMP_RA_OFFSET;
		#endif
		#ifdef USERNAMEPASSWD_RA_SIZE
		tc_partition_define[GET_USERNAMEPASSWD_RA_SIZE      ]=USERNAMEPASSWD_RA_SIZE;
		#endif
		#ifdef USERNAMEPASSWD_RA_OFFSET
		tc_partition_define[GET_USERNAMEPASSWD_RA_OFFSET    ]=USERNAMEPASSWD_RA_OFFSET;
		#endif
		#ifdef SYSLOG_RA_SIZE
		tc_partition_define[GET_SYSLOG_RA_SIZE              ]=SYSLOG_RA_SIZE;
		#endif
		#ifdef SYSLOG_RA_OFFSET
		tc_partition_define[GET_SYSLOG_RA_OFFSET            ]=SYSLOG_RA_OFFSET;
		#endif
		#ifdef EEPROM_RA_SIZE
		tc_partition_define[GET_EEPROM_RA_SIZE              ]=EEPROM_RA_SIZE;
		#endif
		#ifdef EEPROM_RA_OFFSET
		tc_partition_define[GET_EEPROM_RA_OFFSET            ]=EEPROM_RA_OFFSET;
		#endif
		#ifdef PROLINE_CWMPPARA_RA_SIZE
		tc_partition_define[GET_PROLINE_CWMPPARA_RA_SIZE    ]=PROLINE_CWMPPARA_RA_SIZE;
		#endif
		#ifdef PROLINE_CWMPPARA_RA_OFFSET
		tc_partition_define[GET_PROLINE_CWMPPARA_RA_OFFSET  ]=PROLINE_CWMPPARA_RA_OFFSET;
		#endif
		#ifdef BOB_RA_SIZE
		tc_partition_define[GET_BOB_RA_SIZE                 ]=BOB_RA_SIZE;
		#endif
		#ifdef BOB_RA_OFFSET
		tc_partition_define[GET_BOB_RA_OFFSET               ]=BOB_RA_OFFSET;
		#endif
		#ifdef IMG_BOOT_FLAG_SIZE
		tc_partition_define[GET_IMG_BOOT_FLAG_SIZE          ]=IMG_BOOT_FLAG_SIZE;
		#endif
		#ifdef IMG_BOOT_FLAG_OFFSET
		tc_partition_define[GET_IMG_BOOT_FLAG_OFFSET        ]=IMG_BOOT_FLAG_OFFSET;
		#endif
		#ifdef IMG_BOOT_FLAG_RESERVE_SIZE
		tc_partition_define[GET_IMG_BOOT_FLAG_RESERVE_SIZE  ]=IMG_BOOT_FLAG_RESERVE_SIZE;
		#endif
		#ifdef IMG_BOOT_FLAG_RESERVE_OFFSET
		tc_partition_define[GET_IMG_BOOT_FLAG_RESERVE_OFFSET]=IMG_BOOT_FLAG_RESERVE_OFFSET;
		#endif
		#ifdef EEPROM_RA_AC_OFFSET
		tc_partition_define[GET_EEPROM_RA_AC_OFFSET         ]=EEPROM_RA_AC_OFFSET;
		#endif
		#ifdef EEPROM_RA_AC_RESERVE_OFFSET
		tc_partition_define[GET_EEPROM_RA_AC_RESERVE_OFFSET ]=EEPROM_RA_AC_RESERVE_OFFSET;
		#endif
		#ifdef DEFAULTROMFILE_RA_SIZE
		tc_partition_define[GET_DEFAULTROMFILE_RA_SIZE      ]=DEFAULTROMFILE_RA_SIZE;
		#endif
		#ifdef DEFAULTROMFILE_RA_OFFSET
		tc_partition_define[GET_DEFAULTROMFILE_RA_OFFSET    ]=DEFAULTROMFILE_RA_OFFSET;
		#endif
		#ifdef EEPROM_761511N_RA_SIZE
		tc_partition_define[GET_EEPROM_761511N_RA_SIZE      ]=EEPROM_761511N_RA_SIZE;
		#endif
		#ifdef EEPROM_761511N_RA_OFFSET
		tc_partition_define[GET_EEPROM_761511N_RA_OFFSET    ]=EEPROM_761511N_RA_OFFSET;
		#endif
		#ifdef TRX_SET1_SET2_11N_RA_SIZE
		tc_partition_define[GET_TRX_SET1_SET2_11N_RA_SIZE   ]=TRX_SET1_SET2_11N_RA_SIZE;
		#endif
		#ifdef TRX_SET1_SET2_11N_RA_OFFSET
		tc_partition_define[GET_TRX_SET1_SET2_11N_RA_OFFSET ]=TRX_SET1_SET2_11N_RA_OFFSET;
		#endif
		#ifdef EEPROM_761511AC_RA_SIZE
		tc_partition_define[GET_EEPROM_761511AC_RA_SIZE     ]=EEPROM_761511AC_RA_SIZE;
		#endif
		#ifdef EEPROM_RA_761511AC_OFFSET
		tc_partition_define[GET_EEPROM_RA_761511AC_OFFSET   ]=EEPROM_RA_761511AC_OFFSET;
		#endif
		#ifdef TRX_SET1_SET2_11AC_RA_SIZE
		tc_partition_define[GET_TRX_SET1_SET2_11AC_RA_SIZE  ]=TRX_SET1_SET2_11AC_RA_SIZE;
		#endif
		#ifdef TRX_SET1_SET2_11AC_RA_OFFSET
		tc_partition_define[GET_TRX_SET1_SET2_11AC_RA_OFFSET]=TRX_SET1_SET2_11AC_RA_OFFSET;
		#endif
		#ifdef RESERVED_761511AC_SIZE
		tc_partition_define[GET_RESERVED_761511AC_SIZE      ]=RESERVED_761511AC_SIZE;
		#endif
		#ifdef EEPROM_761311AC_RA_SIZE
		tc_partition_define[GET_EEPROM_761311AC_RA_SIZE     ]=EEPROM_761311AC_RA_SIZE;
		#endif
		#ifdef EEPROM_RA_761311AC_OFFSET
		tc_partition_define[GET_EEPROM_RA_761311AC_OFFSET   ]=EEPROM_RA_761311AC_OFFSET;
		#endif
		#ifdef MRD_RA_SIZE
		tc_partition_define[GET_MRD_RA_SIZE                 ]=MRD_RA_SIZE;
		#endif
		#ifdef MRD_RA_OFFSET
		tc_partition_define[GET_MRD_RA_OFFSET               ]=MRD_RA_OFFSET;
		#endif
		#ifdef ROM_T_RA_SIZE
		tc_partition_define[GET_ROM_T_RA_SIZE               ]=ROM_T_RA_SIZE;
		#endif
		#ifdef ROM_T_RA_OFFSET
		tc_partition_define[GET_ROM_T_RA_OFFSET             ]=ROM_T_RA_OFFSET;
		#endif
		#ifdef MAX_EEPROM_BIN_FILE_SIZE
		tc_partition_define[GET_MAX_EEPROM_BIN_FILE_SIZE    ]=MAX_EEPROM_BIN_FILE_SIZE;
		#endif
		#ifdef OMCI_RA_SIZE
		tc_partition_define[GET_OMCI_RA_SIZE                ]=OMCI_RA_SIZE;
		#endif
	
		init_define = 1;
	}
	
	if (name < MAX_DEFINE_NUM)
	{
		data = tc_partition_define[name];
	}
	else
	{
		printf("Marco is not define !\n");
	}
	
	return data;
}

