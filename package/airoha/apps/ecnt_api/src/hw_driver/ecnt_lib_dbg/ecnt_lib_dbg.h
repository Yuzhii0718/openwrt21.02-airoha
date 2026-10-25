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

#ifndef __ECNT_LIB_DBG_H__
#define __ECNT_LIB_DBG_H__

/**
* \file  ecnt_lib_dbg.h  
* \brief This file is ecnt lib dbg header file for dbg APIs that will be exported for others to use.
* \author Lei.Zhang
* \date     2020-09-23
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

enum ECNT_DBG_LIB_TYPE 
{
	ECNT_DBG_TYPE_PCIE = 0,
	ECNT_DBG_TYPE_ACL,
	ECNT_DBG_TYPE_VOIP,
	ECNT_DBG_TYPE_GPON,
	ECNT_DBG_TYPE_BRTABELE,
	ECNT_DBG_TYPE_EPON,
	ECNT_DBG_TYPE_WIFI,
	ECNT_DBG_TYPE_MAX	
};	


typedef enum _ecnt_itf_debug_level_
{
	ECNT_DBG_ERROR  = 1 << 0,
	ECNT_DBG_WARN   = 1 << 1,
	ECNT_DBG_NOTICE = 1 << 2,
	ECNT_DBG_DEBUG  = 1 << 3,
}e_ecnt_itf_debug_level_t;

typedef enum _ecnt_blapi_debug_enable_
{
	ECNT_BLAPI_PERFORM_DBUG_ENABLE  = 1 << 0,
	ECNT_BLAPI_SYSTEM_DBUG_ENABLE   = 1 << 1,
	ECNT_BLAPI_TRAFFIC_DBUG_ENABLE = 1 << 2,
	ECNT_BLAPI_XDSL_DBUG_ENABLE  = 1 << 3,
	ECNT_BLAPI_WLAN_DBUG_ENABLE = 1 << 4,
}e_ecnt_blapi_debug_enable_t;


/**
* \brief This function print ecnt dbg message.
*
* \param[in] fmt: Variable parameter list.
* \note
*  Print ecnt dbg message.
* \par Example
* \snippet ecnt_lib_dbg.c print ecnt debug message.
*/
void ecnt_itf_printf(char *fmt,...);								

/**
* \brief This function get ecnt debug switch.
*
* \param[in] level: ECNT_DBG_ERROR=1,ECNT_DBG_WARN = 2,ECNT_DBG_NOTICE=4,ECNT_DBG_DEBUG=8 
* \param[in] type: ECNT_DBG_TYPE_PCIE = 0,ECNT_DBG_TYPE_ACL=1,ECNT_DBG_TYPE_VOIP=2,ECNT_DBG_TYPE_GPON=3,ECNT_DBG_TYPE_BRTABELE=4,ECNT_DBG_TYPE_EPON=5,ECNT_DBG_TYPE_WIFI=6 
* \note
*  Get ecnt dbg switch.
* \par Example
* \snippet ecnt_lib_dbg.c get ecnt dbg switch.
*/
int get_itf_dbg_switch(unsigned int level,unsigned int type);

#define ECNT_LIB_DBG
#ifdef ECNT_LIB_DBG
#define ECNT_LIB_DBG_PRINT(level,type,fmt, ...) do{\
                                            if(get_itf_dbg_switch(level,type))\
                                                ecnt_itf_printf("[%s]:" fmt,__FUNCTION__,##__VA_ARGS__);\
                                         }while(0)

#else
#define ECNT_LIB_DBG_PRINT(level,type,fmt, ...)
#endif

/**
* \brief This function print blapi dbg message.
*
* \param[in] fmt: Variable parameter list.
* \note
*  Print blapi dbg message.
* \par Example
* \snippet ecnt_lib_dbg.c print blapi debug message.
*/
void ecnt_blapi_printf(char *fmt,...);

/**
* \brief This function get blapi debug switch.
*
* \param[in] level: ECNT_DBG_ERROR=1,ECNT_DBG_WARN = 2,ECNT_DBG_NOTICE=4,ECNT_DBG_DEBUG=8  
* \param[in] type: ECNT_BLAPI_PERFORM_DBUG_ENABLE=1,ECNT_BLAPI_SYSTEM_DBUG_ENABLE=2,ECNT_BLAPI_TRAFFIC_DBUG_ENABLE=4,ECNT_BLAPI_XDSL_DBUG_ENABLE=5,ECNT_BLAPI_WLAN_DBUG_ENABLE=6
* \note
*  Get blapi dbg switch.
* \par Example
* \snippet ecnt_lib_dbg.c get blapi dbg switch.
*/
int get_blapi_dbg_switch(unsigned int level,unsigned int enableType);

#define ECNT_LIB_BLAPI_DBG_PRINT(level,enableType,fmt, ...) do{\
                                            if(get_blapi_dbg_switch(level,enableType))\
                                                ecnt_blapi_printf("[%s:%d]:" fmt,__FUNCTION__,__LINE__,##__VA_ARGS__);\
                                         }while(0)
#endif
