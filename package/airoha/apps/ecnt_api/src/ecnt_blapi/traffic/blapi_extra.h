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
#ifndef _BLAPI_EXTRA_H_
#define _BLAPI_EXTRA_H_
#include <arpa/inet.h>

/**
* \file  blapi_extra.h  
* \brief Extra definiations and BLAPIs of blapi_traffic
* \author Zhengping.Zhang
* \date     2022-06-06
* \version  V1.0.0 
* \copyright Airoha Inc                                                              
*/


typedef struct _blapi_ecnt_ip_addr_ {
  union {
	struct in_addr addr4;
	struct in6_addr addr6;
  } addr;
}blapi_ecnt_ip_addr;

typedef struct _blapi_ecnt_sock_addr_ {
  union {
	struct sockaddr_in sock_dst;
	struct sockaddr_in6 sock6_dst;
  } sock;
}blapi_ecnt_sock_addr;

typedef struct _blapi_traffic_monitor_data_s
{
	blapi_ecnt_ip_addr ecnt_addr;
	int port;
}blapi_traffic_monitor_data;

typedef struct _blapi_traffic_mirror_data_s
{
	char ipaddress[40];
	blapi_ecnt_sock_addr mirror_dst;
	int to_ai_family;
	int mirror_isloop;
	void *socket;
}blapi_traffic_mirror_data;

typedef struct _blapi_traffic_mirror_install_data_s
{
	int sockfd_loop;
	int sockfd_wan;
	int sockfd_loopv6;
	int sockfd_wanv6;
	int sockfd_lanv6;
}blapi_traffic_mirror_install_data;

/**
 * \brief This function uninstall mirror traffic.
 *
 * This function uninstall mirror traffic. submit by Lei.Zhang 2020.01.02
 * \param[in]	void
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  uninstall mirror traffic.
 * \par Example
 * \snippet blapi_traffic.c uninstall mirror traffic.
 */
int blapi_traffic_uninstall_mirror_traffic(void);

/**
 * \brief This function add mirror traffic.
 *
 * This function add mirror traffic. submit by ShengQian.Wang 2020.11.25 
 * \param[in]	mirror_data: traffic mirror data
 * \param[in]	datealen: mirror data lenth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  add mirror traffic.
 * \par Example
 * \snippet blapi_traffic.c add mirror traffic.
 */
int blapi_traffic_add_mirror_traffic(blapi_traffic_mirror_data* mirror_data, int datalen);

/**
 * \brief This function del mirror traffic.
 *
 * This function del mirror traffic. submit by ShengQian.Wang 2020.11.25 
 * \param[in]	mirror_data: traffic mirror data
 * \param[in]	datealen: mirror data lenth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  del mirror traffic.
 * \par Example
 * \snippet blapi_traffic.c del mirror traffic.
 */
int blapi_traffic_del_mirror_traffic(blapi_traffic_mirror_data* mirror_data, int datalen);

/**
 * \brief This function install mirror traffic.
 *
 * This function install mirror traffic. submit by ShengQian.Wang 2020.11.25 
 * \param[in]	install_data: traffic mirror data
 * \param[in]	datealen: mirror data lenth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  install mirror traffic.
 * \par Example
 * \snippet blapi_traffic.c install mirror traffic.
 */
int blapi_traffic_install_mirror_traffic(blapi_traffic_mirror_install_data* install_data, int datalen);

/**
 * \brief This function count monitor traffic.
 *
 * This function count monitor traffic. submit by ShengQian.Wang 2020.11.25 
 * \param[in]	devCnt: traffic mirror cnt
 * \param[in]	datealen: monitor data lenth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  count monitor traffic.
 * \par Example
 * \snippet blapi_traffic.c count monitor traffic.
 */
int blapi_traffic_monitor_cnt_traffic(int *devCnt, int datalen);

/**
 * \brief This function date monitor traffic.
 *
 * This function date monitor traffic. submit by ShengQian.Wang 2020.11.25 
 * \param[in]	list: traffic monitor data list
 * \param[in]	datealen: monitor data lenth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  date monitor traffic.
 * \par Example
 * \snippet blapi_traffic.c date monitor traffic.
 */
int blapi_traffic_monitor_date_traffic(blapi_traffic_monitor_data *list, int datalen);

/**
 * \brief This function uninstall monitor traffic.
 *
 * This function uninstall monitor traffic. submit by ShengQian.Wang 2020.11.25 
 * \param[in]	void
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  uninstall monitor traffic.
 * \par Example
 * \snippet blapi_traffic.c uninstall monitor traffic.
 */
int blapi_traffic_uninstall_monitor_traffic(void);

/**
 * \brief This function install monitor traffic.
 *
 * This function install monitor traffic. submit by ShengQian.Wang 2020.11.25 
 * \param[in]	void
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  install monitor traffic.
 * \par Example
 * \snippet blapi_traffic.c install monitor traffic.
 */
int blapi_traffic_install_monitor_traffic();


#endif /*_BLAPI_EXTRA_H_*/


