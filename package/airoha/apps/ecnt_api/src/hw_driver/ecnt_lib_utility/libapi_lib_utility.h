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
#ifndef _API_LIB_UTILITY_H
#define _API_LIB_UTILITY_H

/**
* \file  libapi_lib_utility.h  
* \brief This file is libapi lib utility header file for utility APIs that will be exported for others to use.
* \author Lei.Zhang
* \date     2020-09-23
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

/************************************************************************
*                  I N C L U D E S
************************************************************************/

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/
#define API_LIB_SUCCESS	(0)
#define API_LIB_FAIL	(-1)

typedef struct {
    char *name;
    int (*func)(int argc,char *argv[],void *p);
    int flags;
    int argcmin;
    char *argc_errmsg;
} cmds_t;

#define PATH_MAX_DNSMASQ	128

#define API_SYSTEM_RAW		system
#define API_FPUTS_RAW		fputs
#define API_FPRINTF_RAW		fprintf

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/

/**
* \brief This function show the usage of cmd.
*
* \param[in] argc: Parameter number.
* \param[in] argv: Variable parameter list.
* \param[in] p: void pointer
* \retval  0   Success.
* \retval  -1  Fail.
* \note
* Show the usage of cmd.
* \par Example
* \snippet libapi_lib_utility.c show the usage of cmd.
*/
int api_lib_help(int argc, char* argv[], void *p);

/**
* \brief This function call the subcmd.
*
* \param[in] tab: Sub cmd table.
* \param[in] argc: Parameter number.
* \param[in] argv: Variable parameter list.
* \param[in] p: void pointer
* \retval  0   Success.
* \retval  -1  Fail.
* \note
* Call the subcmd.
* \par Example
* \snippet libapi_lib_utility.c call the subcmd.
*/
int subcmd(const cmds_t tab[], int argc, char *argv[], void *p);

/**
* \brief This function find the pid by proc name.
*
* \param[in] ProcName: proc file name.
* \param[in] pidNum: Pid number.
* \param[out] foundpid: pid of found.
* \param[out] foundnum: pid num of found.
* \retval  0   Success.
* \retval  -1  Fail.
* \note
* Find the pid by proc name.
* \par Example
* \snippet libapi_lib_utility.c find the pid by proc name.
*/
int api_lib_find_pid_by_name( char *ProcName, int *foundpid, int pidNum, int *foundnum);

/**
* \brief This function read the file data to buf.
*
* \param[in] path: file name.
* \param[in] size: buf size
* \param[out] buf: buf of storing the info read from file.
* \retval  0   Success.
* \retval  -1  Fail.
* \note
* Read the file data to buf.
* \par Example
* \snippet libapi_lib_utility.c read the file data to buf.
*/
void fileRead(char *path, char *buf, int size);

/**
* \brief This function get the fd of mtd file.
*
* \param[in] mtd: mtd file name.
* \param[in] flags: flag of open file.
* \retval  fd : fd of open file
* \note
* Get the fd of mtd file.
* \par Example
* \snippet libapi_lib_utility.c get the fd of mtd file.
*/
int mtd_open(const char *mtd, int flags);
/**
* \brief This function will keep system cmd parameter safe.
*
* This function covert injection code to normal string. 
*
* \param[in]	 input parameter string
* \param[out]	 out escape parameter string
* \param[in]	 out escape parameter length.
* \retval void
* \note
*  keep out escape parameter buffer is double size larger than parameter 
* \par Example
*/
void api_escape_parameter(char *parameter, char *escape_parameter, int escape_parameter_len);

int is_emmc_blk(void);

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

#endif
