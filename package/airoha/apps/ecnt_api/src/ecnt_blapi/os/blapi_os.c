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
* @file  blapi_lib_os.c  
* @brief This file contains the implementations of os/kernel related business logic APIs.
* @author Bryan
* @date     2019-09-06 
* @version  A001 
* @copyright EcoNet Inc                                                              
*/




/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include "blapi_os.h"
#include "blapi_os_internal.h"
#include "ecnt_lib_dbg.h"
#include "libapi_lib_utility.h"
#include <asm-generic/socket.h>

#if KERNEL_3_18_21
#include <linux/ecnt_in.h>
#ifdef TCSUPPORT_OPENWRT
#include <netinet/in.h>
#else
#include <linux/in6.h>
#endif
#endif
#include <common/ecnt_global_macro.h>
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

#ifdef TCSUPPORT_OPENWRT
#if KERNEL_3_18_21
#define	IPV6_SKB_VLAN_ID		78
#endif
#else
#if(defined(TCSUPPORT_CT_VLAN_BIND) && defined(TCSUPPORT_CT))
#if KERNEL_3_18_21
#define INVALID_VLAN_ID 4096
#else
#define IP_SKB_VLAN_ID 51
#define IPV6_SKB_VLAN_ID 75
#define INVALID_VLAN_ID 4096
#endif
#endif
#endif
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




int blapi_os_set_sock_vlan_val(int sockfd,int family,unsigned short *vlan_val)
{
	if(AF_INET6 == family)
	{
		if(setsockopt(sockfd,SOL_IPV6,IPV6_SKB_VLAN_ID,vlan_val,sizeof(*vlan_val)) == -1)
		{
			printf("%s,setsockopt v6 error!\n",__FUNCTION__);
			return -1;
		}
	}
	else
	{
		if(setsockopt(sockfd,SOL_IP,IP_SKB_VLAN_ID,vlan_val,sizeof(*vlan_val)) == -1)
		{
			printf("%s,setsockopt v4 error!\n",__FUNCTION__);
			return -1;
		}
	}
	
	return 0;
}

int blapi_os_get_sock_vlan_val(int sockfd,int family,unsigned short *vlan_val)
{
    socklen_t vlan_val_optLen = sizeof(*vlan_val);
    if(AF_INET6 == family)
	{
		if(getsockopt(sockfd, SOL_IPV6, IPV6_SKB_VLAN_ID, vlan_val, &vlan_val_optLen) == -1)
		{
			printf("%s,getsockopt v6 error!\n",__FUNCTION__);
			return -1;
		}
	}
	else
	{
		if(getsockopt(sockfd, SOL_IP,IP_SKB_VLAN_ID, vlan_val, &vlan_val_optLen) == -1)
		{
			printf("%s,getsockopt v4 error!\n",__FUNCTION__);
			return -1;
		}
	}
	
	return 0;
}

void blapi_os_set_non_root_mode(void)
{
	/* change gid first, otherwise can not change gid when uid is NOT ZERO.*/
	setgid(ECNT_GID);
	setuid(ECNT_UID);

	return;
}


int blapi_os_get_dev_mac(char *itf_name, char *mac, int len)
{
	int sock = -1;
	struct ifreq ifbuf;
	uint8_t *tmphwaddr = NULL;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sock < 0 ) 
	{
		printf("%s %d: socket open fail\n", __FUNCTION__,__LINE__);
		return -1;
	}

	memset(&ifbuf, 0, sizeof(ifbuf));
	strncpy(ifbuf.ifr_name, itf_name, sizeof(ifbuf.ifr_name) - 1);
	if ( ioctl(sock, SIOCGIFHWADDR, &ifbuf) == -1 ) 
	{
		printf("%s %d: ioctl open fail\n", __FUNCTION__,__LINE__);
		close(sock);
		return -1;
	}
	tmphwaddr = (uint8_t *)ifbuf.ifr_ifru.ifru_hwaddr.sa_data;
	snprintf(mac, len, "%02X:%02X:%02X:%02X:%02X:%02X",
		tmphwaddr[0], tmphwaddr[1], tmphwaddr[2], tmphwaddr[3], tmphwaddr[4], tmphwaddr[5]);

	close(sock);

	return 0;
}

int blapi_os_set_sock_type(int socket, int type, const void *option_vlaue, int option_len)
{
	int option_name = 0;

	if ( NULL == option_vlaue )
		return -1;

	switch ( type )
	{
		case SOCK_FLAG_RCV_MAC:
			option_name = SO_TYPE_COPY_SRC_MAC;
			break;
		case SOCK_FLAG_SOCK_MARK:
			option_name = SO_TYPE_COPY_SKB_MARK;
			break;
		default:
			return -2;
	}

	if ( setsockopt(socket, SOL_SOCKET, option_name, option_vlaue, option_len) < 0 )
	{
		printf("%s() setsockopt fail.\n",__FUNCTION__);
		return -3;
	}

	return 0;
}

int blapi_os_get_sock_type(int socket, int type, const void *option_value, int *option_len)
{
	int option_name = 0;
    void *option_value_t = (char *)option_value;
    
	if ( NULL == option_value || NULL == option_len )
		return -1;

	switch ( type )
	{
		case SOCK_FLAG_RCV_MAC:
			option_name = SO_TYPE_SRC_MAC;
			break;
		case SOCK_FLAG_SOCK_MARK:
			option_name	= SO_MARK;
			break;
		default:
			return -2;
	}

	if ( getsockopt(socket, SOL_SOCKET, option_name, option_value_t, (socklen_t *)option_len) < 0 )
	{
		printf("%s() getsockopt fail.\n",__FUNCTION__);
		return -3;
	}

	return 0;
}


int blapi_os_get_itf_info(const char *itf_name, int info_type, unsigned char *out_data, unsigned char out_data_len)
{
	int sock = -1, err_code = 0;
	struct ifreq ifbuf;
	struct sockaddr_in *p_saddr = NULL;

	if ( NULL == out_data || NULL == itf_name )
		return -1;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sock < 0 ) 
	{
		printf("%s %d: socket open fail\n", __FUNCTION__,__LINE__);
		return -2;
	}

	bzero(&ifbuf, sizeof(ifbuf));
	snprintf(ifbuf.ifr_name, sizeof(ifbuf.ifr_name), "%s", itf_name);

	if ( ITF_TYPE_MAC == info_type )
	{
		if ( out_data_len < 6 )
		{
			err_code = -3;
			goto err_end;
		}

		if ( -1 == ioctl(sock, SIOCGIFHWADDR, &ifbuf) ) 
		{
			printf("%s %d: ioctl fail\n", __FUNCTION__, __LINE__);
			err_code = -4;
			goto err_end;
		}

		memcpy(out_data, ifbuf.ifr_hwaddr.sa_data, 6);
	}
	else if ( ITF_TYPE_IP == info_type || ITF_TYPE_MASK == info_type )
	{
		if ( out_data_len < 4 )
		{
			err_code = -5;
			goto err_end;
		}

		if ( ITF_TYPE_IP == info_type )
		{
			if ( -1 == ioctl(sock, SIOCGIFADDR, &ifbuf) ) 
			{
				printf("%s %d: ioctl fail\n", __FUNCTION__, __LINE__);
				err_code = -6;
				goto err_end;
			}
		}
		else
		{
			if ( -1 == ioctl(sock, SIOCGIFNETMASK, &ifbuf) ) 
			{
				printf("%s %d: ioctl fail\n", __FUNCTION__, __LINE__);
				err_code = -7;
				goto err_end;
			}
		}

		p_saddr = (struct sockaddr_in *)&ifbuf.ifr_addr;
		memcpy(out_data, &p_saddr->sin_addr, 4);
	}


err_end:
	close(sock);

	return err_code;
}



void blapi_os_escape_parameter(char *parameter, char *escape_parameter, int escape_parameter_len)
{
	api_escape_parameter(parameter, escape_parameter, escape_parameter_len);
}

void blapi_os_check_ipv4(char *parameter, char *check_parameter, int check_parameter_len)
{
	blapi_os_escape_parameter(parameter, check_parameter, check_parameter_len);
}

void blapi_os_check_ipv6(char *parameter, char *check_parameter, int check_parameter_len)
{
	blapi_os_escape_parameter(parameter, check_parameter, check_parameter_len);
}

void blapi_os_check_mac(char *parameter, char *check_parameter, int check_parameter_len)
{
	blapi_os_escape_parameter(parameter, check_parameter, check_parameter_len);
}

void blapi_os_check_num(char *parameter, char *check_parameter, int check_parameter_len)
{
	blapi_os_escape_parameter(parameter, check_parameter, check_parameter_len);
}


void blapi_os_check_alpha(char *parameter, char *check_parameter, int check_parameter_len)
{
	blapi_os_escape_parameter(parameter, check_parameter, check_parameter_len);
}


