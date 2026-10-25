
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
#ifndef _LIB_ECNT_EVENT_H_
#define _LIB_ECNT_EVENT_H_
#include <ecnt_event_global/ecnt_event_global.h>
/**
* \file  libecntevent.h  
* \brief This file is ecnt lib event header file that will be exported for others to use.
* \author Lei.Zhang
* \date     2020-09-23
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

struct ecnt_data;

/**
 * \brief This function execute ecnt event.
 *
 * \param[in]	in_data: ecnt data
 * \param[in]	event_handle: ecnt event handle info
 * \retval	1	Success.
 * \note
 *  Execute ecnt event.
 * \par Example
 * \snippet libecntevent.c execute ecnt event.
 */
int ecnt_event_execute(struct ecnt_data *in_data, struct ecnt_event_handle *event_handle);

/**
 * \brief This function register ecnt event.
 *
 * \param[in]	name: ecnt event name
 * \param[in]	hook: function pointer of handle event
 * \retval	source:	ecnt event source pointer
 * \retval	NULL:	fail
 * \note
 *  Register ecnt event.
 * \par Example
 * \snippet libecntevent.c register ecnt event.
 */
struct ecnt_event_source* ecnt_event_register(char *name,int (*hook)(struct ecnt_data *in_data));

/**
 * \brief This function unregister ecnt event.
 *
 * \param[in]	source: ecnt event source
 * \retval	0:	success
 * \note
 *  Unregister ecnt event.
 * \par Example
 * \snippet libecntevent.c unregister ecnt event.
 */
int ecnt_event_unregister(struct ecnt_event_source* source);

/**
 * \brief This function send ecnt event.
 *
 * \param[in]	main_type: main ecnt event type
 * \param[in]	sub_type: sub ecnt event type
 * \param[in]	in_data: ecnt event data
 * \param[in]	len: ecnt event data lenth
 * \retval	0:	success
 * \retval	-1:	fail
 * \note
 *  Send ecnt event.
 * \par Example
 * \snippet libecntevent.c send ecnt event.
 */
int ecnt_event_send(int main_type , int sub_type, struct ecnt_event_data *in_data, int len);

#endif