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

#ifndef __LIBAPI_LIB_FLOWMGR_H__
#define __LIBAPI_LIB_FLOWMGR_H__

/**
* \file  libapi_lib_flowmgr.h  
* \brief This file is libapi lib flowmgr header file for flowmgr APIs that will be exported for others to use.
* \author Lei.Zhang
* \date     2020-11-18
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

#include "uapi/modules/traffic_classify_global/traffic_classify_global.h"

#define ecnt_flowmgr_lib_add  				flowmgr_lib_add
#define ecnt_flowmgr_lib_del  				flowmgr_lib_del
#define ecnt_flowmgr_lib_clean  			flowmgr_lib_clean
#define ecnt_flowmgr_lib_ioctl 				flowmgr_lib_ioctl

/* API ·µ»ØÖµ*/
typedef enum
{
    ECNT_E_FLOW_SUCCESS = 0,
	ECNT_E_FLOW_NEXIST,
	ECNT_E_FLOW_EXIST,
	ECNT_E_FLOW_OVER,
	ECNT_E_FLOW_ODEV,
	ECNT_E_FLOW_FAIL
}ECNT_LIB_FLOW_RET;

/**
 * \brief This function add traffic classify entry.
 *
 * \param[in] port_rule: traffic classify entry info
 * \retval	0	Success.
 * \retval	!0	Fail.
 * \note
 *  Add traffic classify entry.
 * \par Example
 * \snippet libapi_lib_flowmgr.c add traffic classify entry.
 */
ECNT_LIB_FLOW_RET flowmgr_lib_add(ecn_traffic_classify_info_t *port_rule);

/**
 * \brief This function delete traffic classify entry.
 *
 * \param[in] port_rule: traffic classify entry info
 * \retval	0	Success.
 * \retval	!0	Fail.
 * \note
 *  Delete traffic classify entry.
 * \par Example
 * \snippet libapi_lib_flowmgr.c delete traffic classify entry.
 */
ECNT_LIB_FLOW_RET flowmgr_lib_del(ecn_traffic_classify_info_t *port_rule);

/**
 * \brief This function clean traffic classify entry.
 *
 * \param[in] port_rule: traffic classify entry info
 * \retval	0	Success.
 * \retval	!0	Fail.
 * \note
 *  Clean traffic classify entry.
 * \par Example
 * \snippet libapi_lib_flowmgr.c clean traffic classify entry.
 */
ECNT_LIB_FLOW_RET flowmgr_lib_clean(ecn_traffic_classify_info_t *port_rule);

/**
 * \brief This function is ioctl for traffic classify entry.
 *
 * \param[in] port_rule: traffic classify entry info
 * \param[in] cmd: TRAFFIC_CLASSIFY_ADD_ENTRY/TRAFFIC_CLASSIFY_DEL_ENTRY/TRAFFIC_CLASSIFY_CLEAN
 * \retval	0	Success.
 * \retval	!0	Fail.
 * \note
 *  Add traffic classify entry.
 * \par Example
 * \snippet libapi_lib_flowmgr.c add traffic classify entry.
 */
ECNT_LIB_FLOW_RET flowmgr_lib_ioctl(unsigned int cmd, ecn_traffic_classify_info_t *port_rule);
#endif 
