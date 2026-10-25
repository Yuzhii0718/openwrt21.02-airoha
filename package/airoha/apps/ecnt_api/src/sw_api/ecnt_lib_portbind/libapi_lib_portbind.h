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
#ifndef _API_LIB_PORTBIND_H
#define _API_LIB_PORTBIND_H

/**
* \file  libapi_lib_portbind.h  
* \brief This file is libapi lib portbind header file for portbind APIs that will be exported for others to use.
* \author Lei.Zhang
* \date     2020-09-23
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

/************************************************************************
*                  I N C L U D E S
************************************************************************/
//#include <asm/tc3162/cmdparse.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/
#define API_LIB_SUCCESS	(0)
#define API_LIB_FAIL	(-1)

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/
struct pb_route_param
{
	char *ip_ver;			/* ip version v4/v6/all */
	char *if_name;			/* interface name */
	char *dev_name;			/* device name */
	char *gateway;			/* ipv4 gateway */
	char *gateway6;			/* ipv6 gateway */
	char *wanip;			/* ipv4 wan ip */
	char *wanip_netmask;	/* ipv4 wan ip netmask */
	char *lanip;			/* ipv4 lan ip */
	char *lanip_netmask;	/* ipv4 lan ip netmask */
	char *lan_ip6;			/* ipv6 lan ip */
	char *lan_prefix6;		/* ipv6 lan prefix */
	char *wan_orgpd6;		/* ipv6 wan original pd */
	char *wan_pd6;			/* ipv6 wan pd */
	char *wan_ip6;			/* ipv6 wan ip */
	char *wan_prefixlen6;	/* ipv6 wan ip prefixlen */
};

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/
 /**
 * \brief This function delete the route of interface.
 *
 * \param[in] pb: port bind parameter ptr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Delete the route of interface.
 * \par Example
 * \snippet api_lib_portbind.c delete the route of interface.
 */
int portbind_lib_del_route(struct pb_route_param *pb);

/**
* \brief This function add the route of interface.
*
 * \param[in] pb: port bind parameter ptr
* \retval  0   Success.
* \retval  -1  Fail.
* \note
*  Add the route of interface.
* \par Example
* \snippet api_lib_portbind.c add the route of interface.
*/
int portbind_lib_add_route(struct pb_route_param *pb);

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
