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

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libapi_lib_utility.h"
#include "libapi_lib_igmpmgr.h"

static int do_set_igmp_entry_add(int argc, char **argv)
{
	int i = 0;
	unsigned int ip_tmp[8];
	unsigned int mac_tmp[6];
    ECNT_MC_PORT_INFO pInfo;

	if (argc < 11)
	{
		goto entry_add_err;
	}

	memset(ip_tmp, 0, 8 * sizeof(unsigned int));
	memset(mac_tmp, 0, 6 * sizeof(unsigned int));
	memset(&pInfo, 0, sizeof(ECNT_MC_PORT_INFO));

	/* get port & vlan info */
	pInfo.port_id = strtoul(argv[4], NULL, 10);
	pInfo.vlan_id = strtoul(argv[5], NULL, 10);
	/* get IP info */
	if( 0 == strncmp(argv[1] , "ipv4", 4) ){
		pInfo.group_addr.is_ipv6 = 0;
		if(sscanf(argv[2], "%d.%d.%d.%d", &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3]) != 4){
			goto entry_add_err;
		}
		pInfo.group_addr.IP.ipv4_address = htonl((ip_tmp[0]<<24) | (ip_tmp[1]<<16) | (ip_tmp[2]<<8) | (ip_tmp[3]));
	}else if( 0 == strncmp(argv[1] , "ipv6", 4) ){
		pInfo.group_addr.is_ipv6 = 1;
		if(sscanf(argv[2], "%x:%x:%x:%x:%x:%x:%x:%x", &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3], 
			&ip_tmp[4], &ip_tmp[5], &ip_tmp[6], &ip_tmp[7]) != 8){
			goto entry_add_err;
		}
		for(i = 0; i < 8; i++){
			pInfo.group_addr.IP.ipv6_address.ip_address_array[2*i] = (ip_tmp[i]>>8) & 0xff;
			pInfo.group_addr.IP.ipv6_address.ip_address_array[2*i +1] = (ip_tmp[i]) & 0xff;
		}
	}else{
		goto entry_add_err;
	}
	/* get MAC info */
	if(sscanf(argv[3], "%x:%x:%x:%x:%x:%x", &mac_tmp[0], &mac_tmp[1],
		&mac_tmp[2], &mac_tmp[3], &mac_tmp[4],&mac_tmp[5]) != 6){
		goto entry_add_err;
	}
	for(i = 0; i < 6; i++){
		pInfo.group_mac.mac_address_byte[i] = mac_tmp[i] & 0xff;
	}
	/* get VLAN action info */
	pInfo.vlan_action.vlan_mode = strtoul(argv[6], NULL, 10);
	pInfo.vlan_action.outer_tpid = strtoul(argv[7], NULL, 16);
	pInfo.vlan_action.outer_tci = strtoul(argv[8], NULL, 16);
	pInfo.vlan_action.inner_tpid = strtoul(argv[9], NULL, 16);
	pInfo.vlan_action.inner_tci = strtoul(argv[10], NULL, 16);
    
	if(0 > ecnt_igmp_entry_add(&pInfo)){
		printf("fail.\n");
		goto entry_add_err;
	}else{
		printf("success.\n");
	}
    
	return 0;

entry_add_err:
	printf("Usage: ecnt_igmp_cmd set_entry_add ipv4/ipv6 ip_addr(x.x.x.x/x:x:x:x:x:x:x:x) mac_addr(x:x:x:x:x:x) \
port_id vlan_id vlan_mode outer_tpid outer_tci inner_tpid inner_tci\n");
	return -1;
}

static int do_set_igmp_entry_del(int argc, char **argv)
{
	int i = 0;
	unsigned int ip_tmp[8];
	unsigned int mac_tmp[6];
	ECNT_MC_PORT_INFO pInfo;

	if (argc < 11)
	{
		goto entry_del_err;
	}

	memset(ip_tmp, 0, 8 * sizeof(unsigned int));
	memset(mac_tmp, 0, 6 * sizeof(unsigned int));
	memset(&pInfo, 0, sizeof(ECNT_MC_PORT_INFO));

	/* get port & vlan info */
	pInfo.port_id = strtoul(argv[4], NULL, 10);
	pInfo.vlan_id = strtoul(argv[5], NULL, 10);
	/* get IP info */
	if( 0 == strncmp(argv[1] , "ipv4", 4) ){
		pInfo.group_addr.is_ipv6 = 0;
		if(sscanf(argv[2], "%d.%d.%d.%d", &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3]) != 4){
			goto entry_del_err;
		}
		pInfo.group_addr.IP.ipv4_address = htonl((ip_tmp[0]<<24) | (ip_tmp[1]<<16) | (ip_tmp[2]<<8) | (ip_tmp[3]));
	}else if( 0 == strncmp(argv[1] , "ipv6", 4) ){
		pInfo.group_addr.is_ipv6 = 1;
		if(sscanf(argv[2], "%x:%x:%x:%x:%x:%x:%x:%x", &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3], 
			&ip_tmp[4], &ip_tmp[5], &ip_tmp[6], &ip_tmp[7]) != 8){
			goto entry_del_err;
		}
		for(i = 0; i < 8; i++){
			pInfo.group_addr.IP.ipv6_address.ip_address_array[2*i] = (ip_tmp[i]>>8) & 0xff;
			pInfo.group_addr.IP.ipv6_address.ip_address_array[2*i +1] = (ip_tmp[i]) & 0xff;
		}
	}else{
		goto entry_del_err;
	}
	/* get MAC info */
	if(sscanf(argv[3], "%x:%x:%x:%x:%x:%x", &mac_tmp[0], &mac_tmp[1],
		&mac_tmp[2], &mac_tmp[3], &mac_tmp[4],&mac_tmp[5]) != 6){
		goto entry_del_err;
	}
	for(i = 0; i < 6; i++){
		pInfo.group_mac.mac_address_byte[i] = mac_tmp[i] & 0xff;
	}
	/* get VLAN action info */
	pInfo.vlan_action.vlan_mode = strtoul(argv[6], NULL, 10);
	pInfo.vlan_action.outer_tpid = strtoul(argv[7], NULL, 16);
	pInfo.vlan_action.outer_tci = strtoul(argv[8], NULL, 16);
	pInfo.vlan_action.inner_tpid = strtoul(argv[9], NULL, 16);
	pInfo.vlan_action.inner_tci = strtoul(argv[10], NULL, 16);
	
	if(0 > ecnt_igmp_entry_del(&pInfo)){
		printf("fail.\n");
		goto entry_del_err;
	}else{
		printf("success.\n");
	}
	
	return 0;

entry_del_err:
	printf("Usage: ecnt_igmp_cmd set_entry_del ipv4/ipv6 ip_addr(x.x.x.x/x:x:x:x:x:x:x:x) mac_addr(x:x:x:x:x:x) \
port_id vlan_id vlan_mode outer_tpid outer_tci inner_tpid inner_tci\n");
	return -1;
}

static int do_set_igmp_entry_update(int argc, char **argv)
{
	int i = 0;
	unsigned int ip_tmp[8];
	unsigned int mac_tmp[6];
	ECNT_MC_PORT_INFO pInfo;

	if (argc < 11)
	{
		goto entry_update_err;
	}

	memset(ip_tmp, 0, 8 * sizeof(unsigned int));
	memset(mac_tmp, 0, 6 * sizeof(unsigned int));
	memset(&pInfo, 0, sizeof(ECNT_MC_PORT_INFO));

	/* get port & vlan info */
	pInfo.port_id = strtoul(argv[4], NULL, 10);
	pInfo.vlan_id = strtoul(argv[5], NULL, 10);
	/* get IP info */
	if( 0 == strncmp(argv[1] , "ipv4", 4) ){
		pInfo.group_addr.is_ipv6 = 0;
		if(sscanf(argv[2], "%d.%d.%d.%d", &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3]) != 4){
			goto entry_update_err;
		}
		pInfo.group_addr.IP.ipv4_address = htonl((ip_tmp[0]<<24) | (ip_tmp[1]<<16) | (ip_tmp[2]<<8) | (ip_tmp[3]));
	}else if( 0 == strncmp(argv[1] , "ipv6", 4) ){
		pInfo.group_addr.is_ipv6 = 1;
		if(sscanf(argv[2], "%x:%x:%x:%x:%x:%x:%x:%x", &ip_tmp[0], &ip_tmp[1], &ip_tmp[2], &ip_tmp[3], 
			&ip_tmp[4], &ip_tmp[5], &ip_tmp[6], &ip_tmp[7]) != 8){
			goto entry_update_err;
		}
		for(i = 0; i < 8; i++){
			pInfo.group_addr.IP.ipv6_address.ip_address_array[2*i] = (ip_tmp[i]>>8) & 0xff;
			pInfo.group_addr.IP.ipv6_address.ip_address_array[2*i +1] = (ip_tmp[i]) & 0xff;
		}
	}else{
		goto entry_update_err;
	}
	/* get MAC info */
	if(sscanf(argv[3], "%x:%x:%x:%x:%x:%x", &mac_tmp[0], &mac_tmp[1],
		&mac_tmp[2], &mac_tmp[3], &mac_tmp[4],&mac_tmp[5]) != 6){
		goto entry_update_err;
	}
	for(i = 0; i < 6; i++){
		pInfo.group_mac.mac_address_byte[i] = mac_tmp[i] & 0xff;
	}
	/* get VLAN action info */
	pInfo.vlan_action.vlan_mode = strtoul(argv[6], NULL, 10);
	pInfo.vlan_action.outer_tpid = strtoul(argv[7], NULL, 16);
	pInfo.vlan_action.outer_tci = strtoul(argv[8], NULL, 16);
	pInfo.vlan_action.inner_tpid = strtoul(argv[9], NULL, 16);
	pInfo.vlan_action.inner_tci = strtoul(argv[10], NULL, 16);
	
	if(0 > ecnt_igmp_entry_update(&pInfo)){
		printf("fail.\n");
		goto entry_update_err;
	}else{
		printf("success.\n");
	}
	
	return 0;

entry_update_err:
	printf("Usage: ecnt_igmp_cmd set_entry_update ipv4/ipv6 ip_addr(x.x.x.x/x:x:x:x:x:x:x:x) mac_addr(x:x:x:x:x:x) \
port_id vlan_id vlan_mode outer_tpid outer_tci inner_tpid inner_tci\n");
	return -1;
}

static int do_set_igmp_entry_clear_by_port(int argc, char **argv)
{
    int port_id = 0;

	if (argc < 2)
	{
		printf("Usage: ecnt_igmp_cmd set_entry_clear_by_port port_id\n");
		return -1;
	}
	
    port_id = strtoul(argv[1], NULL, 10);
    
	if(0 > ecnt_igmp_entry_clear_by_port((unsigned char)port_id))
		printf("fail.\n");
	else
		printf("success.\n");
    
	return 0;
}

static int do_set_igmp_entry_clear_all(int argc, char **argv)
{
	if (argc < 1)
	{
		printf("Usage: ecnt_igmp_cmd set_entry_clear_all\n");
		return -1;
	}
    
	if(0 > ecnt_igmp_entry_clear_all())
		printf("fail.\n");
	else
		printf("success.\n");
    
	return 0;
}

#define IGMP_ENTRY_GET_MAX_NUM	64
static int do_get_igmp_entry(int argc, char **argv)
{
	int i = 0;
	int idx = 0;
    unsigned char port_id = 0;
	ECNT_MC_PORT_INFO entry[IGMP_ENTRY_GET_MAX_NUM];
	int array_size = IGMP_ENTRY_GET_MAX_NUM;

	if (argc < 2)
	{
		goto entry_get_err;
	}

	memset(&entry[0], 0, IGMP_ENTRY_GET_MAX_NUM * sizeof(ECNT_MC_PORT_INFO));
	
    port_id = (unsigned char)(strtoul(argv[1], NULL, 10));
    
	if(0 > ecnt_igmp_entry_get(port_id, entry, &array_size)){
		printf("fail.\n");
		goto entry_get_err;
	}else{
		printf("success.\n");
	}

	/* print igmp entry info */
	printf("port	vlan	grp_mac			v_mode	o_tpid	o_tci	i_tpid	i_tci	grp_ip\n");
	for(idx = 0; idx < array_size; idx++)
	{
		printf("%d	%d	", entry[idx].port_id, entry[idx].vlan_id);
		for( i = 0; i < 6; i++){
			printf("%02X", entry[idx].group_mac.mac_address_byte[i]);
			if(i < 5)
				printf(":");
		}
		printf("	%d	0x%04X	0x%04X	0x%04X	0x%04X	", entry[idx].vlan_action.vlan_mode,
				entry[idx].vlan_action.outer_tpid, entry[idx].vlan_action.outer_tci,
				entry[idx].vlan_action.inner_tpid, entry[idx].vlan_action.inner_tci);
		if(entry[idx].group_addr.is_ipv6){
			for( i = 0; i < 16; i++){
				printf("%02X", entry[idx].group_addr.IP.ipv6_address.ip_address_array[i]);
				if((i%2 == 1) && (i < 15))
					printf(":");
			}
		}else{
			printf("%d.%d.%d.%d", ((unsigned char *)&entry[idx].group_addr.IP.ipv4_address)[0],
				((unsigned char *)&entry[idx].group_addr.IP.ipv4_address)[1], ((unsigned char *)&entry[idx].group_addr.IP.ipv4_address)[2],
				((unsigned char *)&entry[idx].group_addr.IP.ipv4_address)[3]);
		}
		printf("\n");
	}

	return 0;

entry_get_err:
	printf("Usage: ecnt_igmp_cmd get_entry port_id\n");
	return -1;
}

static int do_set_igmp_mode(int argc, char **argv)
{
    int mode = 0;

	if (argc < 2)
	{
		printf("Usage: ecnt_igmp_cmd set_mode [0:IP&VID 1:MAC 2:IP&srcIP 3:MAC&VID 4:MAC&IP 5:MAC&IP&VID]\n");
		return -1;
	}
	
    mode = strtoul(argv[1], NULL, 10);
    
	if(0 > ecnt_igmp_mode_set((ECNT_MC_FWD_MODE)mode))
		printf("fail.\n");
	else
		printf("success.\n");
    
	return 0;
}

static int do_get_igmp_mode(int argc, char **argv)
{
    ECNT_MC_FWD_MODE mode = 0;;
	
	if(0 > ecnt_igmp_mode_get(&mode))
		printf("fail.\n");
	else
    	printf("ecnt_igmp filter mode = %d, [0:IP&VID 1:MAC 2:IP&srcIP 3:MAC&VID 4:MAC&IP 5:MAC&IP&VID].\n", mode);
    
	return 0;
}

static int do_set_igmp_snooping_on_off(int argc, char **argv)
{
    unsigned char enable = ECNT_IGMP_SNOOPING_ON;

	if (argc < 2)
	{
		printf("Usage: ecnt_igmp_cmd set_snooping [0:disable 1:enable]\n");
		return -1;
	}
	
    enable = strtoul(argv[1], NULL, 10);
    
	if(0 > ecnt_igmp_snooping_on_off_set(enable))
		printf("fail.\n");
	else
		printf("success.\n");
    
	return 0;
}

static int do_get_igmp_snooping_on_off(int argc, char **argv)
{
    unsigned char enable = ECNT_IGMP_SNOOPING_ON;
	
	if(0 > ecnt_igmp_snooping_on_off_get(&enable))
		printf("fail.\n");
	else
    	printf("snooping enable = %d.\n", enable);
    
	return 0;
}

static cmds_t Ecnt_igmp_Cmds[] = 
{
	{"set_entry_add",      		do_set_igmp_entry_add,			0x0, 0, NULL},
	{"set_entry_del",      		do_set_igmp_entry_del,			0x0, 0, NULL},
	{"set_entry_update",      	do_set_igmp_entry_update,		0x0, 0, NULL},
	{"set_entry_clear_by_port", do_set_igmp_entry_clear_by_port,0x0, 0, NULL},
	{"set_entry_clear_all",		do_set_igmp_entry_clear_all,	0x0, 0, NULL},
	{"get_entry",      		    do_get_igmp_entry,				0x0, 0, NULL},
	{"set_mode",      		    do_set_igmp_mode,				0x0, 0, NULL},
	{"get_mode",				do_get_igmp_mode,				0x0, 0, NULL},
	{"set_snooping",      	    do_set_igmp_snooping_on_off,    0x0, 0, NULL},
	{"get_snooping",			do_get_igmp_snooping_on_off,	0x0, 0, NULL},
	{NULL,                      NULL,                           0x0, 0, NULL},
};

int main(int argc, char **argv) 
{
    return subcmd(Ecnt_igmp_Cmds, argc, argv, NULL);
}


