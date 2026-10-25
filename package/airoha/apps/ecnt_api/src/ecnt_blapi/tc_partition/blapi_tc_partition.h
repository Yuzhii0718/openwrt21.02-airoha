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
#ifndef _BLAPI_TC_PARTITION_H
#define _BLAPI_TC_PARTITION_H

/**
* \file  blapi_tc_partition.h  
* \brief Definiations and BLAPIs related to the firmware partition 
* \author Gareth.Kuo
* \date     2020-01-06
* \version  V1.0.0 
* \copyright Airoha Inc                                                              
*/


/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define BLAPI_SUCCESS 0
#define BLAPI_FAIL -1

/************************************************************************
*               M A C R O S
*************************************************************************
*/
/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/
/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

enum blapi_name {
	GET_CERM1_RA_SIZE,
	GET_CERM2_RA_SIZE,
	GET_CERM3_RA_SIZE,
	GET_CERM4_RA_SIZE,
	GET_CERM1_RA_OFFSET,
	GET_CERM2_RA_OFFSET,
	GET_CERM3_RA_OFFSET,
	GET_CERM4_RA_OFFSET,
	GET_BOB_RA_OFFSET_OLD_MODE,
	GET_NAND_FLASH_BLOCK_SIZE,
	GET_EEPROM_RA_AC_SIZE,
	GET_EEPROM_RA_AC_RESERVE_SIZE,
	GET_RESERVEAREA_ERASE_SIZE,
	GET_RESERVEAREA_TOTAL_SIZE,
	GET_BACKUPROMFILE_RA_SIZE,
	GET_BACKUPROMFILE_RA_OFFSET,
	GET_TEMP_RA_SIZE,
	GET_TEMP_RA_OFFSET,
	GET_USERNAMEPASSWD_RA_SIZE,
	GET_USERNAMEPASSWD_RA_OFFSET,
	GET_SYSLOG_RA_SIZE,
	GET_SYSLOG_RA_OFFSET,
	GET_EEPROM_RA_SIZE,
	GET_EEPROM_RA_OFFSET,
	GET_PROLINE_CWMPPARA_RA_SIZE,
	GET_PROLINE_CWMPPARA_RA_OFFSET,
	GET_BOB_RA_SIZE,
	GET_BOB_RA_OFFSET,
	GET_IMG_BOOT_FLAG_SIZE,
	GET_IMG_BOOT_FLAG_OFFSET,
	GET_IMG_BOOT_FLAG_RESERVE_SIZE,
	GET_IMG_BOOT_FLAG_RESERVE_OFFSET,
	GET_EEPROM_RA_AC_OFFSET,
	GET_EEPROM_RA_AC_RESERVE_OFFSET,
	GET_DEFAULTROMFILE_RA_SIZE,
	GET_DEFAULTROMFILE_RA_OFFSET,
	GET_EEPROM_761511N_RA_SIZE,
	GET_EEPROM_761511N_RA_OFFSET,
	GET_TRX_SET1_SET2_11N_RA_SIZE,
	GET_TRX_SET1_SET2_11N_RA_OFFSET,
	GET_EEPROM_761511AC_RA_SIZE,
	GET_EEPROM_RA_761511AC_OFFSET,
	GET_TRX_SET1_SET2_11AC_RA_SIZE,
	GET_TRX_SET1_SET2_11AC_RA_OFFSET,
	GET_RESERVED_761511AC_SIZE,
	GET_EEPROM_761311AC_RA_SIZE,
	GET_EEPROM_RA_761311AC_OFFSET,
	GET_MRD_RA_SIZE,
	GET_MRD_RA_OFFSET,
	GET_ROM_T_RA_SIZE,
	GET_ROM_T_RA_OFFSET,
	GET_MAX_EEPROM_BIN_FILE_SIZE,
	GET_OMCI_RA_SIZE,
	MAX_DEFINE_NUM,
};

/**
 * \brief This function get the marco of tc partition.
 *
 * This function get the marco of tc partition. submit by Gareth.Kuo 2019.12.27
 * \retval	value	The value of marco.
 * \retval	-1	Fail.
 * \note
 *  Get the marco of tc partition.
 * \par Example
 * \snippet blapi_tc_partition.c Get the marco of tc partition.
 */	

unsigned int get_tc_partition_value(int name);


#endif /*_BLAPI_TC_PARTITION_H*/
