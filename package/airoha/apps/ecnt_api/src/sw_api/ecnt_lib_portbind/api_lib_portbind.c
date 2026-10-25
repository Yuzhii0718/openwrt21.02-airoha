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
#include <asm/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif
#include <lan_port/itf_policy_route_table.h>
#include <libapi_lib_utility.h>

#include "libapi_lib_portbind.h"


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/
#if !defined(TCSUPPORT_MULTI_USER_ITF)
#define MAX_LAN_PORT_NUM					(14)
#endif

#define LINE_E	"\n"

#if defined(TCSUPPORT_MULTI_USER_ITF)
	unsigned int port_bind_cmd_lanif_mask = 0xf8000000;
#else
	unsigned int port_bind_cmd_lanif_mask = 0xf0000000;
#endif

struct ifPolicyRoute ifpr_extention[] = 
{
	/* ethernet interface */
	/* for four ports */
	{"eth0.1",	"0x10000000",	"204", 0x1},
	{"eth0.2",	"0x20000000",	"205", 0x2},
	{"eth0.3",	"0x30000000",	"206", 0x3},
	{"eth0.4",	"0x40000000",	"207", 0x4},
	/* wireless interface */
	{"ra0", 	"0x50000000",	"208", 0x5},
	{"ra1", 	"0x60000000",	"209", 0x6},
	{"ra2", 	"0x70000000",	"210", 0x7},
	{"ra3", 	"0x80000000",	"211", 0x8},
	/* usb interface */
	{"rai0",	"0x90000000",	"212", 0x9},
	{"rai1",	"0xa0000000",	"213", 0xa},
	{"rai2",	"0xb0000000",	"214", 0xb},
	{"rai3",	"0xc0000000",	"215", 0xc},
	{"usb0",	"0xd0000000",	"216", 0xd},
	{NULL,	NULL,	NULL, 0}
};

static int calc_mask_bits(struct in_addr *addr)
{
	int idx = 0, cnt = 0;
	unsigned long net_addr = 0;

	if ( NULL == addr )
		return 32;

	net_addr = htonl(addr->s_addr);
	for ( idx = 31; idx >=0; idx -- )
	{
		if ( 0 == (net_addr & ( 1 << idx )) )
			break;
		cnt ++;
	}

	return cnt;
}

/******************************************************************************
 Descriptor:	It's used to show p(cmds_t table) command usage.
 Input Args:	argc:
                 argv:
                     g: cmds_t table
 Ret Value:	API_LIB_SUCCESS.
******************************************************************************/
int portbind_lib_del_route(struct pb_route_param *pb)
{
	char lanMk[20], lanTabId[10];
	int i, type, ret = 0;
	char prBuf[128], shell_path[128] = {0};
	struct ifPolicyRoute ifpr_ext[MAX_LAN_PORT_NUM] = {};
	char *ip_ver = NULL, *if_name = NULL;
	FILE *fp = NULL;

	if ( NULL == pb )
		return -1;
	ip_ver = pb->ip_ver;
	if_name = pb->if_name;

	if ( ip_ver == NULL || if_name == NULL )
		return -2;
	
	memset(lanMk, 0, sizeof(lanMk));
	memset(lanTabId, 0, sizeof(lanTabId));
	memset(prBuf, 0, sizeof(prBuf));
	memset(ifpr_ext, 0, sizeof(ifpr_ext));
#if defined(TCSUPPORT_MULTI_USER_ITF)
	memcpy(ifpr_ext,ifpr,sizeof(ifpr));
#else
	memcpy(ifpr_ext,ifpr_extention,sizeof(ifpr_extention));
#endif

	for (i = 0; i < sizeof(ifpr_ext) / sizeof(ifpr_ext[0]); i++) {
		if ( NULL == ifpr_ext[i].ifName )
			break;
		if (!strcmp(ifpr_ext[i].ifName, if_name)) {
			strncpy(lanMk, ifpr_ext[i].mark, sizeof(lanMk) - 1);
			strncpy(lanTabId, ifpr_ext[i].tabid, sizeof(lanTabId) - 1);
			break;
		}
	}
	if (strlen(lanMk) == 0) {
		printf("%s: Get Lan interface failed, so set policy route failed. \n", __FUNCTION__);
		return -1;
	}

	if (!strcmp(ip_ver, "v4")) {
		type = 0;
	}
	else if (!strcmp(ip_ver, "v6")) {
		type = 1;
	}
	else if (!strcmp(ip_ver, "all")) {
		type = 2;
	}
	else {
		return -1;
	}

	/* create shell file for speed up. */
	snprintf(shell_path, sizeof(shell_path), "/tmp/pb_rules_del_exec_%d_%s", type, if_name);
	fp = fopen(shell_path, "w");
	if ( NULL == fp )
		return -1;

	switch (type) {
		case 2:
		case 1:
			/* ipv6 */
			fprintf(fp, "/usr/bin/ip -6 route flush table %s 2>/dev/null"LINE_E, lanTabId);
			fprintf(fp, "/usr/bin/ip -6 rule del fwmark %s/0x%x table %s 2>/dev/null"LINE_E, lanMk, port_bind_cmd_lanif_mask, lanTabId);
			if (1 == type) {
				break;
			}
			/* fall thought if type is 2(ipv4 & ipv6) */
		case 0:
			/* ipv4 */
			fprintf(fp, "/usr/bin/ip route flush table %s 2>/dev/null"LINE_E, lanTabId);
			fprintf(fp, "/usr/bin/ip rule del fwmark %s/0x%x table %s 2>/dev/null"LINE_E, lanMk, port_bind_cmd_lanif_mask, lanTabId);
			fprintf(fp, "/usr/bin/ip route flush cache"LINE_E);
			break;
	}

	/* exec shell */
	fclose(fp);
	ret = chmod(shell_path ,777);
	API_SYSTEM_RAW(shell_path);
	unlink(shell_path);

	return 0;
	
}

int portbind_lib_add_route(struct pb_route_param *pb)
{
	char lanMk[20], lanTabId[10];
	char wan_subnetip[32], lan_subnetip[32], shell_path[128] = {0}, *pos = NULL;
	char prBuf[128];
	int i, type, pd_len = 0;
	int fd=0, ret2=0, wan_netmask_len = 0, lan_netmask_len, ret = 0;
	char buf[5] = {0};
	unsigned int entryActive = 0;
	int vbind_index = 0;
	int op_flag = 0;
	struct ifPolicyRoute ifpr_ext[MAX_LAN_PORT_NUM] = {};
	char *ip_ver = NULL, *if_name = NULL, *dev_name = NULL, *gateway = NULL, *gateway6 = NULL;
	char *wan_ip6 = NULL, *wan_prefixlen6 = NULL;
	struct in_addr lanip_addr, wanip_addr, netmask_addr;
	FILE *fp = NULL;
	char sf_dev_name[64] = {0}, sf_gateway[128] = {0}, sf_gateway6[128] = {0};
	char sf_lan_ip6[128] = {0}, sf_lan_prefix6[128] = {0}, sf_lanip[128] = {0};
	char sf_wan_orgpd6[128] = {0}, sf_wan_pd6[128] = {0};
	char sf_wan_ip6[128] = {0}, sf_wan_prefixlen6[64] = {0};

	if ( NULL == pb )
		return -1;

	ip_ver = pb->ip_ver;
	if_name = pb->if_name;
	dev_name = pb->dev_name;
	gateway = pb->gateway;
	gateway6 = pb->gateway6;
	wan_ip6 = pb->wan_ip6;
	wan_prefixlen6 = pb->wan_prefixlen6;

	if ( ip_ver == NULL || if_name == NULL || dev_name == NULL )
		return -2;

	memset(wan_subnetip, 0, sizeof(wan_subnetip));
	memset(lan_subnetip, 0, sizeof(lan_subnetip));
	memset(lanMk, 0, sizeof(lanMk));
	memset(lanTabId, 0, sizeof(lanTabId));
	memset(prBuf, 0, sizeof(prBuf));
	memset(ifpr_ext, 0, sizeof(ifpr_ext));
	
#if defined(TCSUPPORT_MULTI_USER_ITF)
	memcpy(ifpr_ext,ifpr,sizeof(ifpr));
#else
	memcpy(ifpr_ext,ifpr_extention,sizeof(ifpr_extention));
#endif

	/*_______________________________________________
	** remark packet from different lan interfac,  
	** use the highest 4 bits.
		** ethernet for signal ports **
	** interface		skb->mark		tab id
	** eth0	  			0x10000000		201
		** ethernet interface for four ports **
	** eth0.1 			0x10000000		204
	** eth0.2 			0x20000000		205
	** eth0.3 			0x30000000		206
	** eth0.4 			0x40000000		207
	    ** wireless interfaces **
	** ra0	  			0x50000000		208
	** ra1    			0x60000000		209
	** ra2    			0x70000000		210
	** ra3    			0x80000000		211
	    ** usb interface **
	** usb0   			0x90000000		212
	**_______________________________________________
	*/
	for (i = 0; i < sizeof(ifpr_ext) / sizeof(ifpr_ext[0]); i++) {
		if ( NULL == ifpr_ext[i].ifName )
			break;
		if (!strcmp(ifpr_ext[i].ifName, if_name)) {
			strncpy(lanMk, ifpr_ext[i].mark, sizeof(lanMk) - 1);
			strncpy(lanTabId, ifpr_ext[i].tabid, sizeof(lanTabId) - 1);
#if defined(TCSUPPORT_MULTI_USER_ITF)
			vbind_index = ifpr_ext[i].sk_mark - 1;
#endif
			break;
		}
	}

	if (strlen(lanMk) == 0) {
		printf("%s: Get LAN interface failed, so set policy route for lan interface failed.\n", __FUNCTION__);
		return -1;
	}
	if (!strcmp(ip_ver, "v4")) 
	{
		type = 0;
		if(gateway== NULL || gateway[0] == '\0')
			return -1;
	}
	else if (!strcmp(ip_ver, "v6")) 
	{
		type = 1;
		if(gateway6== NULL || gateway6[0] == '\0')
			return -1;
	}
	else if (!strcmp(ip_ver, "all")) 
	{
		type = 2;
		if(gateway== NULL || gateway[0] == '\0'
			|| gateway6== NULL || gateway6[0] == '\0')
			return -1;
	}
	else 
	{
		return -1;
	}

	/* ipv4 lan ip info. */
	if ( 0 == type || 2 == type )
	{
		if ( pb->lanip && 0 != pb->lanip[0]
			&& pb->lanip_netmask && 0 != pb->lanip_netmask[0] )
		{
			/* 1. calc netmask len. */
			if ( 1 != inet_pton(AF_INET, pb->lanip_netmask, &netmask_addr) )
				return -1;

			lan_netmask_len = calc_mask_bits(&netmask_addr);
			/* 2. calc subnet addr. */
			if ( 1 != inet_pton(AF_INET, pb->lanip, &lanip_addr) )
				return -1;

			lanip_addr.s_addr &= netmask_addr.s_addr;
			bzero(lan_subnetip, sizeof(lan_subnetip));
			inet_ntop(AF_INET, &lanip_addr, lan_subnetip, sizeof(lan_subnetip));
		}
		else
			return -1;
	}

#if defined(TCSUPPORT_CT_VLAN_BIND)
#if !defined(TCSUPPORT_MULTI_USER_ITF)
		if (strstr(if_name, "eth")) 
		{
			vbind_index = *(if_name + 5) - '0' - 1;
		}
#if defined(TCSUPPORT_WLAN_AC)

		else if (strstr(if_name, "rai")) 
		{
			vbind_index = *(if_name + 3) - '0' + 10;
		}
#endif		
		else if (strstr(if_name, "ra")) 
		{
			vbind_index = *(if_name + 2) - '0' + 4;
		}
		
#endif
		if ((vbind_index < 0) || (vbind_index >= MAX_LAN_PORT_NUM))
			return -1;

		fd = open("/proc/tc3162/vbind_entry_active",O_RDONLY|O_NONBLOCK);
		if(fd<0) {
			printf("%s:open %s error.\n", __FUNCTION__, "/proc/tc3162/vbind_entry_active");
			return -1;
		}
		ret2 = read(fd, buf, sizeof(buf) );
		close(fd);
		if(ret2 <= 0) {
			printf("%s:read %s error.\n", __FUNCTION__, "/proc/tc3162/vbind_entry_active");
			return -1;
		}	
		entryActive = atoi(buf);
		if (entryActive & (1 << vbind_index)) {
			printf("%s: %s is VLAN binded.\n", __FUNCTION__, if_name);
			return -1;
		}
#endif

		/* create shell file for speed up. */
		snprintf(shell_path, sizeof(shell_path), "/tmp/pb_rules_add_exec_%d_%s", type, if_name);
		fp = fopen(shell_path, "w");
		if ( NULL == fp )
			return -1;

		/* add mark lan interface */
		switch (type) {
			case 2:
			case 1:
				/* ipv6 & ipv4 */
				fprintf(fp, "/usr/bin/ip -6 route flush table %s 2>/dev/null"LINE_E, lanTabId);
				fprintf(fp, "/usr/bin/ip -6 rule del fwmark %s/0x%x table %s 2>/dev/null"LINE_E, \
					lanMk, port_bind_cmd_lanif_mask, lanTabId);
				fprintf(fp, "/usr/bin/ip -6 rule add fwmark %s/0x%x table %s 2>/dev/null"LINE_E, \
					lanMk, port_bind_cmd_lanif_mask, lanTabId);

				if ( pb->lan_ip6 && 0 != pb->lan_ip6[0]
					&& pb->lan_prefix6 && 0 != pb->lan_prefix6[0] )
				{
					api_escape_parameter(pb->lan_ip6, sf_lan_ip6, sizeof(sf_lan_ip6));
					api_escape_parameter(pb->lan_prefix6, sf_lan_prefix6, sizeof(sf_lan_prefix6));
					
					fprintf(fp, "/usr/bin/ip -6 route add %s/%s dev br0 table %s 2>/dev/null"LINE_E
										, sf_lan_ip6, sf_lan_prefix6
										, lanTabId);
				}
				else
				{
					fprintf(fp, "/usr/bin/ip -6 route add %s/%s dev br0 table %s 2>/dev/null"LINE_E
										, "fe80::1", "64"
										, lanTabId);
				}

				if ( pb->wan_orgpd6 && 0 != pb->wan_orgpd6[0] )
				{
					/* check original pd whether less 64*/
					pos = strrchr(pb->wan_orgpd6, '/');
					if ( pos )
					{
						pd_len = atoi(pos + 1);
						
						/* no need add reject route when PD more than 64. */
						if ( pd_len < 64 )
						{
						api_escape_parameter(pb->wan_orgpd6, sf_wan_orgpd6, sizeof(sf_wan_orgpd6));
							fprintf(fp
								, "/usr/bin/ip -6 route add %s dev lo table %s 2>/dev/null"LINE_E
								, sf_wan_orgpd6
								, lanTabId );
						}
					}
				}

				if ( pb->wan_pd6 && 0 != pb->wan_pd6[0] )
				{
					api_escape_parameter(pb->wan_pd6, sf_wan_pd6, sizeof(sf_wan_pd6));
					fprintf(fp
						, "/usr/bin/ip -6 route add %s dev br0 table %s 2>/dev/null"LINE_E
						, sf_wan_pd6
						, lanTabId );
				}

				api_escape_parameter(dev_name, sf_dev_name, sizeof(sf_dev_name));

				/* add ipv6 link route */
				if ( wan_ip6 &&  0 != wan_ip6[0]  && wan_prefixlen6 && 0 != wan_prefixlen6 )
				{
					api_escape_parameter(wan_ip6, sf_wan_ip6, sizeof(sf_wan_ip6));
					api_escape_parameter(wan_prefixlen6, sf_wan_prefixlen6, sizeof(sf_wan_prefixlen6));
					fprintf(fp, "/usr/bin/ip -6 route add %s/%s dev %s table %s 2>/dev/null"LINE_E, sf_wan_ip6, sf_wan_prefixlen6, sf_dev_name, lanTabId);
				}

#if defined(TCSUPPORT_CT_PPP_ONDEMAND)
				if(NULL == gateway6 || gateway6[0]=='\0')
				{
					fprintf(fp, "/usr/bin/ip -6 route add default dev %s table %s 2>/dev/null"LINE_E, sf_dev_name, lanTabId);
				}
				else
#endif
				{
					api_escape_parameter(gateway6, sf_gateway6, sizeof(sf_gateway6));
					fprintf(fp, "/usr/bin/ip -6 route add default via %s dev %s table %s 2>/dev/null"LINE_E, sf_gateway6, sf_dev_name, lanTabId);
				}

				if (type == 1) 
				{
					break;
				}
				/* fall thought if type is 2(ipv4 & ipv6) */
			case 0:
				/* ipv4 start */
				/* 1. flush route table */
				fprintf(fp, "/usr/bin/ip route flush table %s 2>/dev/null"LINE_E, lanTabId);

				/* 2. flush rule */
				fprintf(fp, "/usr/bin/ip rule del fwmark %s/0x%x table %s 2>/dev/null"LINE_E, \
					lanMk, port_bind_cmd_lanif_mask, lanTabId);

				fprintf(fp, "/usr/bin/ip rule add fwmark %s/0x%x table %s 2>/dev/null"LINE_E, \
					lanMk, port_bind_cmd_lanif_mask, lanTabId);			

				api_escape_parameter(pb->lanip, sf_lanip, sizeof(sf_lanip));
				/* 3. add route */
				fprintf(fp, "/usr/bin/ip route add %s/%d via %s dev br0 table %s 2>/dev/null"LINE_E
									, lan_subnetip, lan_netmask_len, sf_lanip, lanTabId);

				api_escape_parameter(dev_name, sf_dev_name, sizeof(sf_dev_name));

				if ( NULL == strstr(dev_name, "ppp") )
				{
					if ( pb->wanip && 0 != pb->wanip[0]
						&& pb->wanip_netmask && 0 != pb->wanip_netmask[0] )
					{
						/* 1. calc netmask len. */
						if ( 1 == inet_pton(AF_INET, pb->wanip_netmask, &netmask_addr) )
						{
							wan_netmask_len = calc_mask_bits(&netmask_addr);

							/* 2. calc subnet addr. */
							if ( 1 == inet_pton(AF_INET, pb->wanip, &wanip_addr) )
							{
								wanip_addr.s_addr &= netmask_addr.s_addr;
								bzero(wan_subnetip, sizeof(wan_subnetip));
								inet_ntop(AF_INET, &wanip_addr, wan_subnetip, sizeof(wan_subnetip));
								fprintf(fp
										, "/usr/bin/ip route add %s/%d dev %s table %s 2>/dev/null"LINE_E
										, wan_subnetip, wan_netmask_len, sf_dev_name, lanTabId);
							}
						}
					}
				}

				/* only ipv4 */
				op_flag = 0;
#if defined(TCSUPPORT_CT_DSLITE) || defined(TCSUPPORT_CT_PPP_ONDEMAND)
				op_flag = ( '\0' == gateway[0] );
#endif
				if ( op_flag )
				{
					fprintf(fp, "/usr/bin/ip route add default dev %s table %s 2>/dev/null"LINE_E, sf_dev_name, lanTabId);
				}
				else
				{
					api_escape_parameter(gateway, sf_gateway, sizeof(sf_gateway));
					
					fprintf(fp, "/usr/bin/ip route add default via %s dev %s table %s 2>/dev/null"LINE_E, sf_gateway, sf_dev_name, lanTabId);
				}

				/* ipv4 end */
				fprintf(fp, "/usr/bin/ip route flush cache 2> /dev/null"LINE_E);
				break;
		}

	/* exec shell */
	fclose(fp);
	ret = chmod(shell_path ,777);
	API_SYSTEM_RAW(shell_path);
	unlink(shell_path);

	return 0;
}


