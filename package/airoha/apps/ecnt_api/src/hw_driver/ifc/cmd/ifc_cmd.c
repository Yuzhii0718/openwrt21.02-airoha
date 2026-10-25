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
#include <asm/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>

#include "libapi_lib_utility.h"
#include "libapi_lib_ifc.h"

static int do_add(int argc, char *argv[], void *p);
static int do_del(int argc, char *argv[], void *p);
static int do_add_vip(int argc, char *argv[], void *p);
static int do_add_vip_etype(int argc,char* argv[],void *p);
static int do_add_vip_ipv4_proto(int argc,char* argv[],void *p);
static int do_add_vip_ipv6_nxthdr(int argc,char* argv[],void *p);
static int do_add_vip_port(int argc,char* argv[],void *p);
static int do_add_vip_sip_and_sport(int argc,char* argv[],void *p);
static int do_add_vip_dip_and_dport(int argc,char* argv[],void *p);
static int do_add_vip_pbit(int argc,char* argv[],void *p);
static int do_add_vip_dscp(int argc,char* argv[],void *p);
static int do_add_vip_traffic_class(int argc,char* argv[],void *p);
static int do_del_vip(int argc, char *argv[], void *p);
static int do_del_vip_etype(int argc,char* argv[],void *p);
static int do_del_vip_ipv4_proto(int argc,char* argv[],void *p);
static int do_del_vip_ipv6_nxthdr(int argc,char* argv[],void *p);
static int do_del_vip_port(int argc,char* argv[],void *p);
static int do_del_vip_sip_and_sport(int argc,char* argv[],void *p);
static int do_del_vip_dip_and_dport(int argc,char* argv[],void *p);
static int do_del_vip_pbit(int argc,char* argv[],void *p);
static int do_del_vip_dscp(int argc,char* argv[],void *p);
static int do_del_vip_traffic_class(int argc,char* argv[],void *p);

static cmds_t addVipCmds[] = {
	{"etype",do_add_vip_etype,0x0,0,NULL},
	{"ipv4_proto",do_add_vip_ipv4_proto,0x0,0,NULL},
	{"ipv6_nxthdr",do_add_vip_ipv6_nxthdr,0x0,0,NULL},
	{"port",do_add_vip_port,0x0,0,NULL},	
	{"sip_sport",do_add_vip_sip_and_sport,0x0,0,NULL},
	{"dip_dport",do_add_vip_dip_and_dport,0x0,0,NULL},
	{"pbit",do_add_vip_pbit,0x0,0,NULL},
	{"dscp",do_add_vip_dscp,0x0,0,NULL},
	{"traffic_class",do_add_vip_traffic_class,0x0,0,NULL},
	{NULL,NULL,0x0, 0, NULL}
};

static cmds_t delVipCmds[] = {
	{"etype",do_del_vip_etype,0x0,0,NULL},		
	{"ipv4_proto",do_del_vip_ipv4_proto,0x0,0,NULL},	
	{"ipv6_nxthdr",do_del_vip_ipv6_nxthdr,0x0,0,NULL},	
	{"port",do_del_vip_port,0x0,0,NULL},
	{"sip_sport",do_del_vip_sip_and_sport,0x0,0,NULL},
	{"dip_dport",do_del_vip_dip_and_dport,0x0,0,NULL},
	{"pbit",do_del_vip_pbit,0x0,0,NULL},
	{"dscp",do_del_vip_dscp,0x0,0,NULL},
	{"traffic_class",do_del_vip_traffic_class,0x0,0,NULL},
	{NULL,NULL,0x0, 0, NULL}
};

static cmds_t addCmds[] = {
    {"vip", do_add_vip,   		0x0, 0, NULL},
    {NULL,  NULL,       		0x0, 0, NULL}
};

static cmds_t delCmds[] = {
    {"vip", do_del_vip,   		0x0, 0, NULL},
    {NULL,  NULL,       		0x0, 0, NULL}
};

static cmds_t ifcCmds[] = {
    {"add",   	do_add,   	0x10, 0, NULL},
    {"del",   	do_del,   	0x10, 0, NULL},    
    {NULL,      NULL,       0x10, 0, NULL}
};

static int do_add_vip_etype(int argc,char* argv[],void *p)
{
	unsigned short etype = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc add vip etype [etype(hex)]\n");
		return 0;
	}

	etype = strtoul(argv[1],NULL,16);
	printf("\nifc add vip etype 0x%x\n",etype);

	return ifc_lib_set_vip_by_etype(etype);
}

static int do_add_vip_ipv4_proto(int argc,char* argv[],void *p)
{
	unsigned short proto = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc add vip ipv4_proto [proto(hex)]\n");
		return 0;
	}

	proto = strtoul(argv[1],NULL,16);
	printf("\nifc add vip ipv4_proto 0x%x\n",proto);

	return ifc_lib_set_vip_by_ipv4_proto(proto);
}

static int do_add_vip_ipv6_nxthdr(int argc,char* argv[],void *p)
{
	unsigned short nxthdr = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc add vip ipv6_nxthdr [nxthdr(hex)]\n");
		return 0;
	}

	nxthdr = strtoul(argv[1],NULL,16);
	printf("\nifc add vip ipv6_nxthdr 0x%x\n",nxthdr);

	return ifc_lib_set_vip_by_ipv6_nxthdr(nxthdr);
}

static int do_add_vip_port(int argc,char* argv[],void *p)
{
	unsigned short sport = 0;
	unsigned short dport = 0;
	unsigned short ringIdx = 0;
	unsigned int swap = 0;
	
	if(argc < 3)
	    goto help;

	sport = strtoul(argv[1],NULL,10);
	dport = strtoul(argv[2],NULL,10);
	if(argc >= 4) {
		ringIdx = strtoul(argv[3],NULL,10);
	}
	if(argc >= 5) {
		if(strncmp(argv[4], "swap", 4) == 0)
			swap = 1;
		else
			goto help;
	}
	printf("\nifc add vip port %d %d %d %s\n",sport,dport,ringIdx,(swap==1)?("swap"):(""));

	return ifc_lib_set_vip_by_port(sport,dport,ringIdx,swap);

help:
	printf("usage:ifc add vip port [sport] [dport] [ringIdx] [swap]\n");
	printf("note:sport or dport set to 0 means not care\n");
	return 0;
}

static int do_add_vip_sip_and_sport(int argc,char* argv[],void *p)
{
	unsigned int sip = 0;
	unsigned short sport = 0;
	
	if(argc != 3)
	{
		printf("usage:ifc add vip sip_sport [sip] [sport]\n");
		printf("note:sip or sport set to 0 means not care\n");
		printf("note:sip format hex,ex 192.168.1.1->c0a80101\n");
		return 0;
	}

	sip = strtoul(argv[1],NULL,16);
	sport = strtoul(argv[2],NULL,10);
	printf("\nifc add vip sip_sport %x %d\n",sip,sport);

	return ifc_lib_set_vip_by_sip_and_sport(sip,sport);
}

static int do_add_vip_dip_and_dport(int argc,char* argv[],void *p)
{
	unsigned int dip = 0;
	unsigned short dport = 0;
	
	if(argc != 3)
	{
		printf("usage:ifc add vip dip_dport [dip] [dport]\n");
		printf("note:dip or dport set to 0 means not care\n");
		printf("note:dip format hex,ex 192.168.1.1->c0a80101\n");
		return 0;
	}

	dip = strtoul(argv[1],NULL,16);
	dport = strtoul(argv[2],NULL,10);
	printf("\nifc add vip dip_dport %x %d\n",dip,dport);

	return ifc_lib_set_vip_by_dip_and_dport(dip,dport);
}

static int do_add_vip_pbit(int argc,char* argv[],void *p)
{
	unsigned char pbit = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc add vip pbit [pbit(hex)]\n");
		return 0;
	}

	pbit = strtoul(argv[1],NULL,16);
	printf("\nifc add vip pbit 0x%x\n",pbit);

	return ifc_lib_set_flow_by_pbit(pbit);
}

static int do_add_vip_dscp(int argc,char* argv[],void *p)
{
	unsigned char dscp = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc add vip dscp [dscp(hex)]\n");
		return 0;
	}

	dscp = strtoul(argv[1],NULL,16);
	printf("\nifc add vip dscp 0x%x\n",dscp);

	return ifc_lib_set_flow_by_dscp(dscp);
}

static int do_add_vip_traffic_class(int argc,char* argv[],void *p)
{
	unsigned char tc = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc add vip traffic_class [traffic_class(hex)]\n");
		return 0;
	}

	tc = strtoul(argv[1],NULL,16);
	printf("\nifc add vip traffic_class 0x%x\n",tc);

	return ifc_lib_set_flow_by_traffic_class(tc);
}

static int do_del_vip_etype(int argc,char* argv[],void *p)
{
	unsigned short etype = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc del vip etype [etype(hex)]\n");
		return 0;
	}

	etype = strtoul(argv[1],NULL,16);
	printf("\nifc del vip etype 0x%x\n",etype);

	return ifc_lib_del_vip_by_etype(etype);
}

static int do_del_vip_ipv4_proto(int argc,char* argv[],void *p)
{
	unsigned short proto = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc del vip ipv4_proto [proto(hex)]\n");
		return 0;
	}

	proto = strtoul(argv[1],NULL,16);
	printf("\nifc del vip ipv4_proto 0x%x\n",proto);

	return ifc_lib_del_vip_by_ipv4_proto(proto);
}

static int do_del_vip_ipv6_nxthdr(int argc,char* argv[],void *p)
{
	unsigned short nxthdr = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc del vip ipv6_nxthdr [nxthdr(hex)]\n");
		return 0;
	}

	nxthdr = strtoul(argv[1],NULL,16);
	printf("\nifc del vip ipv6_nxthdr 0x%x\n",nxthdr);

	return ifc_lib_del_vip_by_ipv6_nxthdr(nxthdr);
}

static int do_del_vip_port(int argc,char* argv[],void *p)
{
	unsigned short sport = 0;
	unsigned short dport = 0;
	unsigned int swap = 0;
	
	if(argc < 3)
		goto help;

	sport = strtoul(argv[1],NULL,10);
	dport = strtoul(argv[2],NULL,10);
    
	if(argc >= 4) {
		if(strncmp(argv[3], "swap", 4) == 0)
			swap = 1;
		else
			goto help;
	}
	printf("\nifc del vip port %d %d %s\n",sport,dport,(swap==1)?("swap"):(""));

	return ifc_lib_del_vip_by_port(sport,dport,swap);

help:
	printf("usage:ifc del vip port [sport] [dport] [swap]\n");
	printf("note:sport or dport set to 0 means not care\n");
	return 0;
}

static int do_del_vip_sip_and_sport(int argc,char* argv[],void *p)
{
	unsigned int sip = 0;
	unsigned short sport = 0;
	
	if(argc != 3)
	{
		printf("usage:ifc del vip sip_sport [sip] [sport]\n");
		printf("note:sport  set to 0 means not care\n");		
		printf("note:sip format hex,ex 192.168.1.1->c0a80101\n");
		return 0;
	}

	sip = strtoul(argv[1],NULL,16);
	sport = strtoul(argv[2],NULL,10);
	printf("\nifc del vip sip_sport %x %d\n",sip,sport);

	return ifc_lib_del_vip_by_sip_and_sport(sip,sport);
}

static int do_del_vip_dip_and_dport(int argc,char* argv[],void *p)
{
	unsigned int dip = 0;
	unsigned short dport = 0;
	
	if(argc != 3)
	{
		printf("usage:ifc del vip dip_dport [dip] [dport]\n");
		printf("note:dport  set to 0 means not care\n");		
		printf("note:dip format hex,ex 192.168.1.1->c0a80101\n");
		return 0;
	}

	dip = strtoul(argv[1],NULL,16);
	dport = strtoul(argv[2],NULL,10);
	printf("\nifc del vip dip_dport %x %d\n",dip,dport);

	return ifc_lib_del_vip_by_dip_and_dport(dip,dport);
}

static int do_del_vip_pbit(int argc,char* argv[],void *p)
{
	unsigned char pbit = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc del vip pbit [pbit(hex)]\n");
		return 0;
	}

	pbit = strtoul(argv[1],NULL,16);
	printf("\nifc del vip pbit 0x%x\n",pbit);

	return ifc_lib_del_flow_by_pbit(pbit);
}

static int do_del_vip_dscp(int argc,char* argv[],void *p)
{
	unsigned char dscp = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc del vip dscp [dscp(hex)]\n");
		return 0;
	}

	dscp = strtoul(argv[1],NULL,16);
	printf("\nifc del vip dscp 0x%x\n",dscp);

	return ifc_lib_del_flow_by_dscp(dscp);
}

static int do_del_vip_traffic_class(int argc,char* argv[],void *p)
{
	unsigned char tc = 0;
	
	if(argc != 2)
	{
		printf("usage:ifc del vip traffic_class [traffic_class(hex)]\n");
		return 0;
	}

	tc = strtoul(argv[1],NULL,16);
	printf("\nifc del vip traffic_class 0x%x\n",tc);

	return ifc_lib_del_flow_by_traffic_class(tc);
}

static int do_add_vip(int argc, char *argv[], void *p)
{
	return subcmd(addVipCmds,argc,argv,NULL);
}

static int do_del_vip(int argc, char *argv[], void *p)
{
	return subcmd(delVipCmds,argc,argv,NULL);
}

static int do_del(int argc, char *argv[], void *p)
{
	return subcmd(delCmds, argc, argv, NULL);
}

static int do_add(int argc, char *argv[], void *p)
{
	return subcmd(addCmds, argc, argv, NULL);
}

int main(int argc, char **argv) 
{
	return subcmd(ifcCmds, argc, argv, NULL);
}


