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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/qos_type.h>
#include "libcompileoption.h"
#include "libapi_lib_qosrule.h"
#include "libapi_lib_fe.h"
#include <lan_port/lan_port_info.h>
#include <libapi_lib_switchmgr.h>
#include <libapi_lib_utility.h>

static int MAX_TYPE_RULE_NUM_VAL = 16;

static char lanif[MAX_LAN_PORT_NUM][8] = {0};
static char wanif[16][8] = {0};

/* remark type */
static param_t remark[] = 
{
	{0,			"queue",		NULL,		NULL,		0},
	{0,			"ipp",			NULL,		NULL,		0},
	{0,			"tos",			NULL,		NULL,		0},
	{0,			"dscpORTC",		NULL,		NULL,		0},	
	{0,			"dot1p",		NULL,		NULL,		0},
	{-1,		NULL,			NULL,		NULL,		0}
};

/* type rule match */
static param_t typeRule[] =
{
	{0,			"LanIf",		"-i",			"-i",			0},
	{0,			"WanIf",		"-o",			"-o",			0},
	{0,			"smac",			"-s",			"-s",			0},
	{0,			"dmac",			"-d",			"-d",			0},
	{0,			"sip",			"--ip-src",		"--ip6-src",	0},
	{0,			"dip",			"--ip-dst",		"--ip6-dst",	0},
	{0,			"sport",		"--ip-sport",	"--ip6-sport",	0},
	{0,			"dport",		"--ip-dport",	"--ip6-dport",	0},
	{0,			"ipp",			"--ip-ipp",		NULL,			0},
	{0,			"tos",			"--ip-tos",		"",				0},
#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_ITTIM)
	{0,			"dscpORTC",		"--ip-dscp",	"--ip6-tclass",	0},
#endif
	{0,			"proto",		"--ip-proto",	"--ip6-proto",	0},
	{0,			"vlanid",		"--vlan-id",	"--vlan-id",	0},
	{0,			"dot1p",		"--vlan-prio",	"--vlan-prio",	0},
	{0, 		"EtherType",	"-p",			"-p",			0},
	{0, 		"dscp", 		"--ip-dscp",	"", 			0},
	{0, 		"tc",			"", 			"--ip6-tclass", 0},
#if defined(TCSUPPORT_CT_JOYME4)
	{0, 		"fl",			"", 			"--ip6-fl", 	0},
#endif
	{-1,		NULL,			NULL,			NULL,			0}
};


static int doValPut(char *path, char *value);
static void qosCmdExcute( char *rule );
static int assignVlan( tsarm_ioctl_t *ptsar_ioctl );
static int tsfProtoNameToNum( char *proto );
static int paramParser(int paramNum, typeParam_t paramArr[ ], param_t* pt );
static int parserInterface( int flag, char *arg, int *ifNum );
static int checkIPAddrType( char *addr );

int getFlagByEtherType(int isEtherType, int type)
{
	int op_flag = 0;
	
	if(isEtherType == 0)
	{
		op_flag = 1;
	}
	else
	{
		op_flag = (isEtherType != type);
	}	

	return op_flag;
}
	
static int doValPut(char *path, char *value)
{
	FILE *fp;

	fp = fopen(path,"w");
	if(fp != NULL){
		fprintf(fp, "%s", value);
		fclose(fp);
	}else{
		return -1;
	}
	return 0;
}
	
static void qosCmdExcute( char *rule )
{
	if ( rule == NULL ) {
		printf("==>qosCmdExcute(): rule is null.\n");
		return;
	}

	if ( strlen(rule) == 0 ) {
		return;
	}
	
#ifdef QOS_DEBUG
	printf( "TCCMD=%s\n", rule );
#endif
	system( rule ); 

	return;
}

static int assignVlan( tsarm_ioctl_t *ptsar_ioctl )
{
	tsarm_ioctl_t tsarm_ioctl_var;
	struct atmif_sioc mysio;
	int skfd, err;

	memset( &tsarm_ioctl_var, 0, sizeof(tsarm_ioctl_t) );
	mysio.number = 0;
	skfd = socket( PF_ATMPVC, SOCK_DGRAM, 0 );
	
	if ( skfd < 0 ) {
		printf("create socke fail.\n");
		return -1;
	}

	tsarm_ioctl_var.active = ptsar_ioctl->active;
		
	tsarm_ioctl_var.vpi = ptsar_ioctl->vpi;
	tsarm_ioctl_var.vci = ptsar_ioctl->vci;
	
	tsarm_ioctl_var.vid = ptsar_ioctl->vid;
	
	mysio.arg = (void*)&tsarm_ioctl_var;
	if ( ioctl(skfd, SET_ATM_VLAN, &mysio) < 0 ) {
		err = -1;
	}
	else {
		err = 0;
	}
	
	close( skfd );
	return err;
}


static int tsfProtoNameToNum( char *proto )
{
	if ( proto == NULL ) {
		return -1;
	}

	if ( !strcasecmp(proto, "tcp") ) {
		strncpy( proto, "6", LENTH_PARAM_VALUE - 1 );
	}
	else if ( !strcasecmp(proto, "udp") ) {
		strncpy( proto, "17", LENTH_PARAM_VALUE - 1 );
	}
	else if ( !strcasecmp(proto, "igmp") ) {
		strncpy( proto, "2", LENTH_PARAM_VALUE - 1 );
	}
	else if ( !strcasecmp(proto, "tcp/udp") || !strcasecmp(proto, "TCP,UDP") ) {
		strncpy( proto, "6:17", LENTH_PARAM_VALUE - 1 );
	}
	else if ( !strcasecmp(proto, "icmp") ) {
		strncpy( proto, "1", LENTH_PARAM_VALUE - 1 );	

	}
	else if ( !strcasecmp(proto, "icmp6") ) {
		strncpy( proto, "58", LENTH_PARAM_VALUE - 1 );
	}

	return 0;
}

static int paramParser(int paramNum, typeParam_t paramArr[ ], param_t* pt )
{
	param_t *p = NULL;
	int i = 0;
	int flag = 0;

	if ( pt == NULL || paramArr == NULL )
	{
		return -1;
	}

	for ( p = pt; p->flag != -1; p++ ) 
	{
		p->flag = 0;
		memset(p->value, 0, sizeof(p->value));
	}

	for ( p = pt; p->flag != -1; p++ ) 
	{
		for ( i = 0; i < paramNum; i++ )
		{		
			if ( strcmp(p->argname, paramArr[i].paramName) == 0 )
			{
				p->flag = 1;
				strncpy( p->value,  paramArr[i].value, sizeof(p->value)-1);
				if ( !strcmp(p->argname, "sport") || !strcmp(p->argname, "dport") )
				{
					flag = 1;	
				}
				if ( !strcmp(p->argname, "proto") )
				{
					if(!strcmp(p->value, "all") && 1 == flag)
					{
						/*just support tcp/udp protocol for port type in ebtable */
						strncpy( p->value, "6:17", LENTH_PARAM_VALUE - 1 );		
					}
					else
					{
					if ( tsfProtoNameToNum( p->value ) == -1 )
					{
						p->flag = 0;
					}
				}
			}
		}
	}
	}

	return 0;
}

static int parserInterface( int flag, char *arg, int *ifNum )
{
	int i = 0, find = -1;
	char *p = NULL;
	char *d = ",";
	
	if ( arg == NULL ) {
		printf( "==>parserInterface(): valid arg.\n" );
		return -1;
	}

	p = strtok( arg, d );
	while( p ) {
		if ( flag == LANIF ){
			strncpy( lanif[i], p, sizeof(lanif[i])-1 );
		}
		else if ( flag == WANIF ) {
			strncpy( wanif[i], p, sizeof(wanif[i])-1 );
		}
		
		i++;
		p = strtok(NULL, d );
		find = 0;
	}
	*ifNum = i;
	
	return find;
}

static int checkIPAddrType( char *addr )
{
	char *p = NULL;
	
	if ( addr == NULL )
		return -1;
	
	p = strstr( addr, ":" );

	if (p) 
		return 1; /* ipv6 addr */
	else 
		return 0; /* ipv4 addr */
}

static int get_wan_port(void){
    char buf[4];
    FILE *fp = NULL;

    memset(buf, 0, sizeof(buf));
    fp = fopen("/proc/tc3162/eth_portmap","r");
    if(fp != NULL){
        if(fgets (buf, sizeof(buf), fp) != NULL){
            fclose(fp); 
                return atoi(buf);
        }   
        fclose(fp); 
    }
    return -1;
}

static void setEtherWanPortRatelimit(uint32 uplinkBandwidth)
{
#ifdef CONFIG_TP_IMAGE
    /* 由于sdk默认调用 CMD_SET_PORT_RATE_LIMIT 设置的是lan_port_map[wan_port_id]的port(en7529 sdk有修改为实际传入的)，导致设置成ewan口后一个lan口，所以这里直接调用设置ewan口的API */
    if(uplinkBandwidth == 0)
    {
    /* 这里认为0代表不限速 */
        switchmgr_lib_set_etherwan_port_rate_limit_enable(0);
    }
    else
    {
         switchmgr_lib_set_etherwan_port_ratelimit(uplinkBandwidth, 0);
    }
#else
    int wan_port_id = get_wan_port();
				
    if(wan_port_id != -1){
        switchmgr_lib_set_port_ratelimit(wan_port_id, 0, uplinkBandwidth);
    } 
#endif /* CONFIG_TP_IMAGE */
    return;
}

int qoscmd_lib_do_en75xxQoSDiscPQ(uint32 uplinkBandwidth,int qm)
 {	 
	char tcRule[MAX_RULE_LENGTH] = {0};
	 int i = 0 ;
	uint32 uRate = uplinkBandwidth/8;

    fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE_BPS, uplinkBandwidth/8);
	
#if defined(TCSUPPORT_CPU_EN7580)
	/*disable meter ratelimit of car mode*/
	for(i = 0; i < CAR_QUEUE_NUM; i++)
	{
		 fe_lib_set_car_mode_ratelimit(0, 0, i);
	}
#else	
	/*clear meter group*/
	system(PPE_METER_DEL_ALL_CMD); 
#endif

#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
        doValPut("/proc/qdma_wan/mac_qos_flag", "8QPQ");
	/* set hw to hw_sp mode */	
	doValPut("/proc/qdma_wan/mac_qos", "0 0 0 0 0 0 0 0 0");
#else
        doValPut("/proc/qdma_wan/mac_qos_flag", "HWPQ");
	/* set hw to hw_sp mode */
	doValPut("/proc/qdma_wan/mac_qos", "0 0 0 0 0");
#endif
	/*used to ehck queue enable/disable*/
	snprintf(tcRule, sizeof(tcRule), "%d", qm);
	doValPut("/proc/qdma_wan/mac_default_queuemask", tcRule);
			
#if defined(TCSUPPORT_CT_SWQOS)
	system("echo 0 > /proc/tc3162/qos_enable");
#endif

#if defined(TCSUPPORT_CPU_EN7580)
	/*disable meter ratelimit of wrr fb mode*/
	for(i = 0; i < WRR_QUEUE_NUM; i++) 
	{
		fe_lib_set_forcebw_wrr_mode_ratelimit(0, 0, i);
	}
#else
	 /*clear meter group*/
	 system(PPE_METER_DEL_ALL_CMD); 
#endif

#if defined(TCSUPPORT_WAN_ETHER)
	memset(tcRule, 0, sizeof(tcRule));
	snprintf(tcRule, sizeof(tcRule), "%d", qm);
	doValPut("/proc/tc3162/eth_mac_default_queuemask", tcRule);

	system("echo \"HWPQ\" > /proc/tc3162/eth1_tcqos_disc");
	strncpy(tcRule, "echo 0 0 0 0 0 > /proc/tc3162/eth1_qoswrr", sizeof(tcRule)-1);
	system(tcRule);
	setEtherWanPortRatelimit(uplinkBandwidth);
#endif

	return 0;
}

 int qoscmd_lib_do_en75xxQoSDiscWRR(uint32 uplinkBandwidth,int qm, int* weight, int wTotal,int forcebw){	 
	char tcRule[MAX_RULE_LENGTH] = {0};
	int i = 0;
	int isWeightSet = 0;
	 int w[WRR_QUEUE_NUM];
	int ratelimit = 0;
	int weightTotal = 0;

    fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE_BPS, uplinkBandwidth/8);

#if defined(TCSUPPORT_CPU_EN7580)
	/*disable meter ratelimit of car mode*/
	for(i = 0; i < WRR_QUEUE_NUM; i++) 
	{
		fe_lib_set_forcebw_wrr_mode_ratelimit(0, 0, i);
	}
#else
	 /*clear meter group*/
	 system(PPE_METER_DEL_ALL_CMD); 
#endif

	/* queue from 0 to WRR_QUEUE_NUM */
	for ( i = 0; i < WRR_QUEUE_NUM; i++ ) {
 
		if(weight[i] != 0){
			isWeightSet = 1;
			break;
		}
	}

	if(isWeightSet && !(qm & (1<<3))){	
		/*if there is one weight is not 0, set w[3] enable*/
		qm |= 1<<3;
	}

	doValPut("/proc/qdma_wan/mac_qos_flag", "HWWRR");
	/* set hw to hw_sp mode */
#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
	snprintf(tcRule, sizeof(tcRule), "1 %d %d %d %d %d %d %d %d", weight[0], weight[1], weight[2], weight[3],weight[4], weight[5], weight[6], weight[7]);
#else
	snprintf(tcRule, sizeof(tcRule), "1 %d %d %d %d", weight[0], weight[1], weight[2], weight[3]);
#endif
	doValPut("/proc/qdma_wan/mac_qos", tcRule);
	/*used to ehck queue enable/disable*/
	snprintf(tcRule, sizeof(tcRule), "%d", qm);
	doValPut("/proc/qdma_wan/mac_default_queuemask", tcRule);

	if(forcebw&&uplinkBandwidth)
	{
		for(i = 0;i < WRR_QUEUE_NUM;i++)
			weightTotal += weight[i];
        
#if defined TCSUPPORT_CPU_EN7580
        /*disable meter ratelimit of car mode*/
        for(i = 0; i < WRR_QUEUE_NUM; i++) {
            fe_lib_set_forcebw_wrr_mode_ratelimit(0, 0, i);
        }

        fe_lib_set_meter_ratelimit_mode(1, 0, 6);

        /*qm: queue mask , bit map data; ratelimit :6 different ratelimit in webpage*/
        for(i = 0; i < WRR_QUEUE_NUM; i++) {
            ratelimit = uplinkBandwidth*weight[i]/weightTotal;
            fe_lib_set_forcebw_wrr_mode_ratelimit(ratelimit, 1, i);
        }
#else
		for(i = 0;i < WRR_QUEUE_NUM;i++)
		{
		    ratelimit = uplinkBandwidth*weight[i]/weightTotal;
			snprintf(tcRule, sizeof(tcRule), PPE_METER_ADD_CMD, i+1, ratelimit, i+1);	
			system(tcRule);
        }
#endif
	}

#if defined(TCSUPPORT_CT_SWQOS)
	system("echo 0 > /proc/tc3162/qos_enable");
#endif
 
#if defined(TCSUPPORT_WAN_ETHER)
	memset(tcRule, 0, sizeof(tcRule));
	snprintf(tcRule, sizeof(tcRule), "%d", qm);
	doValPut("/proc/tc3162/eth_mac_default_queuemask", tcRule);

	 system("echo \"HWWRR\" > /proc/tc3162/eth1_tcqos_disc");
#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
	 snprintf(tcRule, sizeof(tcRule), "echo 1 %d %d %d %d %d %d %d %d > /proc/tc3162/eth1_qoswrr", weight[0], weight[1], weight[2], weight[3],weight[4], weight[5], weight[6], weight[7]);
#else
	 snprintf(tcRule, sizeof(tcRule), "echo 1 %d %d %d %d > /proc/tc3162/eth1_qoswrr", weight[0], weight[1], weight[2], weight[3]);
#endif
	 system(tcRule);
	 setEtherWanPortRatelimit(uplinkBandwidth);
#endif

	return 0;
}

int qoscmd_lib_do_en75xxQoSDiscCAR(uint32 uplinkBandwidth,int qm, uint32 *ratelimit,int meter_support)
{	
	char tcRule[MAX_RULE_LENGTH] = {0};
	int i = 0;

    fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE_BPS, uplinkBandwidth/8);
	
	doValPut("/proc/qdma_wan/mac_qos_flag", "CAR");
	/* set hw to hw_car mode */
	doValPut("/proc/qdma_wan/mac_qos", "0 0 0 0 0");
	/*used to ehck queue enable/disable*/
	snprintf(tcRule, sizeof(tcRule), "%d", qm);
	doValPut("/proc/qdma_wan/mac_default_queuemask", tcRule);

	/*to support meter group */
	snprintf(tcRule, sizeof(tcRule), "echo %d > %s", meter_support,HWNAT_METER_PATH);
	system(tcRule); 

	
	system(PPE_METER_DEL_ALL_CMD); 
#if defined(TCSUPPORT_CT_SWQOS)
	if(meter_support == 0)
	{ 				
		system("echo \"CAR\" > /proc/tc3162/qos_discip");
#ifdef TCSUPPORT_QOS_EIGHT_QUEUE
		snprintf(tcRule, sizeof(tcRule), "echo %d %d %d %d %d %d %d %d %d > /proc/tc3162/qos_car",
			ratelimit[0],ratelimit[1],ratelimit[2],ratelimit[3],ratelimit[4],ratelimit[5],ratelimit[6],ratelimit[7],uplinkBandwidth);
#else
		snprintf(tcRule, sizeof(tcRule), "echo %d %d %d %d %d %d %d > /proc/tc3162/qos_car",
			ratelimit[0],ratelimit[1],ratelimit[2],ratelimit[3],ratelimit[4],ratelimit[5],uplinkBandwidth);
#endif
		system(tcRule);
		system("echo 0 > /proc/tc3162/qos_totalBW");
		system("echo 1 > /proc/tc3162/qos_enable");
	}
	else
	{
		system("echo 0 > /proc/tc3162/qos_enable");
		for ( i = 0; i < CAR_QUEUE_NUM; i++ )
		{
			if ( !(qm & (1<<i)) ) {
				continue;
			}

			snprintf(tcRule, sizeof(tcRule), PPE_METER_ADD_CMD, i+1, ratelimit[i], i+1);
			printf("tcRule=%s\n",tcRule);
			system(tcRule); 
		}
	}
#else	
	for ( i = 0; i < CAR_QUEUE_NUM; i++ )
	{
		if ( !(qm & (1<<i)) ) {
			continue;
		}

		snprintf(tcRule, sizeof(tcRule), PPE_METER_ADD_CMD, i+1, ratelimit[i], i+1);
		printf("tcRule=%s\n",tcRule);
		system(tcRule); 
	}
#endif
	
#if defined(TCSUPPORT_WAN_ETHER)
	doValPut("/proc/tc3162/eth1_tcqos_disc", "NULLQOS");
	setEtherWanPortRatelimit(uplinkBandwidth);
#endif
	
	return 0;
}

int qoscmd_lib_do_en7580QoSDiscCAR(uint32 uplinkBandwidth,int qm, uint32 *ratelimit)
{	
    char tcRule[MAX_RULE_LENGTH] = {0};
    int i = 0; 
    int enable_mode = 0;
    
    fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE_BPS, uplinkBandwidth/8);

    doValPut("/proc/qdma_wan/mac_qos_flag", "CAR");
    /* set hw to hw_car mode */
    doValPut("/proc/qdma_wan/mac_qos", "0 0 0 0 0");
    /*used to ehck queue enable/disable*/
	snprintf(tcRule, sizeof(tcRule), "%d", qm);
    doValPut("/proc/qdma_wan/mac_default_queuemask", tcRule);

    /*disable meter ratelimit of car mode*/
    for(i = 0; i < CAR_QUEUE_NUM; i++) {
        fe_lib_set_car_mode_ratelimit(0, 0, i);
    }

#if defined(TCSUPPORT_ACCOUNT_METER_V2) 
	fe_lib_set_meter_ratelimit_mode(1, 1, 0);
#else
	fe_lib_set_meter_ratelimit_mode(1, 0, 0);
#endif	
	
    /*qm: queue mask , bit map data; ratelimit :6 different ratelimit in webpage*/
    for(i = 0; i < CAR_QUEUE_NUM; i++) {
        enable_mode = (qm>>i)&1;
        fe_lib_set_car_mode_ratelimit(ratelimit[i], enable_mode, i);
    }

#if defined(TCSUPPORT_WAN_ETHER)
	setEtherWanPortRatelimit(uplinkBandwidth);
#endif

    return 0;
}


int qoscmd_lib_do_qos_switch(int QoSsw, int IPversion)	
{
	char ebtablesRule[MAX_RULE_LENGTH];
	char tcRule[MAX_RULE_LENGTH];
	int i = 0;
	
	memset( ebtablesRule, 0, sizeof(ebtablesRule) );
	memset( tcRule, 0, sizeof(tcRule) );

	/* check input param */
	if((QoSsw != SW_ON && QoSsw != SW_OFF) || (IPversion != IPV4_FLAG && IPversion != IPV6_FLAG && IPversion != IPALL_FLAG ))
	{
		return -1;
	}
		
	if (QoSsw == SW_ON)
	{	
		/* create new ebtables chain for type rule */
		for ( i = 0; i < MAX_TYPE_RULE_NUM_VAL; i++ )
		{
			/* for type match rules */
			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSTypeRule%d", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );	

			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -P QoSTypeRule%d RETURN", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );

			/* for 802.1p type match rules */
			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSVlanRule%d", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );

			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -P QoSVlanRule%d RETURN", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );

			/* for queue & 802.1p & dscp or TC remark */
			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSTypeRule%dRemark", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );
			
			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -P QoSTypeRule%dRemark RETURN", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );

			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A BROUTING -j QoSTypeRule%d", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );
		}
		
		/* create new iptables chain for app rule */
		for ( i = 0; i < MAX_APP_RULE_NUM; i++ ) 
		{
			if (IPversion == IPV4_FLAG )
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSAppRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );				
			}
#if	QOS_FOR_IPV6_OUT_PACKET
			else if ( IPversion == IPV6_FLAG) 
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSAppRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );				
			}
#endif
			else
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSAppRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );		
#if QOS_FOR_IPV6_OUT_PACKET
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSAppRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
#endif
			}	
		}

		/* create new iptables chain for template rule */
		for ( i = 0; i < MAX_TEM_RULE_NUM; i++ )
		{
			if ( IPversion == IPV4_FLAG )
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSTemRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );
			}
#if QOS_FOR_IPV6_OUT_PACKET
			else if (IPversion == IPV6_FLAG )
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSTemRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
			}
#endif
			else
			{
				/* for ipv4 */
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSTemRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );
#if QOS_FOR_IPV6_OUT_PACKET
				/* for ipv6 */
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -N QoSTemRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
#endif
			}
		}
	}
	else
	{
		doValPut("/proc/qdma_wan/mac_qos_flag", "NULLQOS");
		
		/* delete ebtables chain for type rule */
		for ( i = 0; i < MAX_TYPE_RULE_NUM_VAL; i++ )
		{
			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D BROUTING -j QoSTypeRule%d 2>/dev/null", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );	
			
			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSTypeRule%d", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );

			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSVlanRule%d", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );

			memset( ebtablesRule, 0, sizeof(ebtablesRule) );
			snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSTypeRule%dRemark", QOS_BROUTE, i );
			qosCmdExcute( ebtablesRule );
		}

		/* delete iptables chain for app rule */
		for ( i = 0; i < MAX_APP_RULE_NUM; i++ )
		{
			if (IPversion == IPV4_FLAG)
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );

				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSAppRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );
			}
#if QOS_FOR_IPV6_OUT_PACKET
			else if (IPversion == IPV6_FLAG)
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
				
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSAppRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
			}
#endif
			else 
			{
				/* for ipv4 */
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );

				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSAppRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );				
#if QOS_FOR_IPV6_OUT_PACKET	
				/* for ipv6*/
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );

				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSAppRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
#endif
			}
		}

		/* delete iptables chain for template rule */
		for ( i = 0; i < MAX_TEM_RULE_NUM; i++ )
		{
			if (IPversion == IPV4_FLAG)
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSTemRule%d 2>/dev/null", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );
				
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSTemRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );
			}
#if QOS_FOR_IPV6_OUT_PACKET
			else if (IPversion == IPV6_FLAG)
			{
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSTemRule%d 2>/dev/null", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );

				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSTemRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
			}
#endif
			else
			{
				/* for ipv4 */
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSTemRule%d 2>/dev/null", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );

				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSTemRule%d", QOS_MANGLE, i );
				qosCmdExcute( ebtablesRule );
#if QOS_FOR_IPV6_OUT_PACKET		
				/* for ipv6 */
				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -D OUTPUT -j QoSTemRule%d 2>/dev/null", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );

				memset( ebtablesRule, 0, sizeof(ebtablesRule) );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -X QoSTemRule%d", QOS_MANGLE_IPV6, i );
				qosCmdExcute( ebtablesRule );
#endif
			}
		}
		
		/* del tc rule on imq0 */
		strncpy( tcRule, "tc qdisc del dev imq0 root 2>/dev/null" ,sizeof(tcRule)-1);
		qosCmdExcute( tcRule );
	}
	
	return 0;
}

int qoscmd_lib_do_qos_vlanid(int sw, int vpi, int vci, int vlanid)
{
	tsarm_ioctl_t tsarm_ioctl_var;
	memset( &tsarm_ioctl_var, 0, sizeof(tsarm_ioctl_t) );
	
	/* check input param */
	if((sw != SW_ON && sw != SW_OFF)||(vpi == 0 &&vci == 0) || vlanid == 0)
	{
		return -1;
	}

	if ( sw == SW_ON) 
	{
		tsarm_ioctl_var.active = 1;
	}
	else
	{
		tsarm_ioctl_var.active = 0;
	}

	tsarm_ioctl_var.vpi = (unsigned char)vpi;
	tsarm_ioctl_var.vci = (unsigned char)vci;
	tsarm_ioctl_var.vid = vlanid;
	
	return assignVlan( &tsarm_ioctl_var );
}

int qoscmd_lib_do_qos_type(int type, int ruleIdx, int IPversion,int paramNum, typeParam_t paramArr[ ])
{
	int ret = 0; 
	int i = 0;
	int lanifNum = 0;
	int ipAddrType = -1; /* -1 means both ipv4 and ipv6, 0 means ipv4, 1 means ipv6 */
	int isEtherType = 0;
	int ruleFlag = 0, vlanFlag = 0, ipFlag = 0, rtpFlag = 0;
	int Wandisableflag = 0;
	char etherType[32] = {0};
	char ebtablesIPv4Rule[MAX_RULE_LENGTH*2]={0};
	char tempIPv4Rule[MAX_RULE_LENGTH*2] = {0};
	char ebtablesIPv6Rule[MAX_RULE_LENGTH*2] = {0};
	char tempIPv6Rule[MAX_RULE_LENGTH*2]= {0};
	char ebtVlanRule[MAX_RULE_LENGTH]= {0};
	char ebtablesWanIf[MAX_RULE_LENGTH]= {0};
	param_t *pt = NULL;
	struct qos_type qt;
	
	/* check input param */
	if(( type != TYPE_ADD && type != TYPE_DEL )
		|| ( type != TYPE_DEL && paramArr == NULL )
		|| (IPversion != IPNULL_FLAG && IPversion != IPV4_FLAG && IPversion != IPV6_FLAG && IPversion != IPALL_FLAG )
		|| ( ruleIdx > MAX_TYPE_RULE_NUM_VAL-1))
	{
		return -1;
	}

	memset(&qt, 0, sizeof(struct qos_type));
	qt.rule_no = ruleIdx;
	
	/* flush type rule first */
	snprintf( ebtablesIPv4Rule, sizeof(ebtablesIPv4Rule), "%s -F QoSTypeRule%d", QOS_BROUTE, ruleIdx );
	qosCmdExcute( ebtablesIPv4Rule );
	
	snprintf( ebtVlanRule, sizeof(ebtVlanRule), "%s -F QoSVlanRule%d", QOS_BROUTE, ruleIdx );
	qosCmdExcute( ebtVlanRule );
	
	/* if the cmd is 'del', just return */
	if ( type == TYPE_DEL )
	{		
		return 0;
	}
	
	/* parser param to typeRule struct */
	ret = paramParser(paramNum,paramArr, typeRule );
	if ( ret == -1 )
	{
		return -1;
	}
	
	for ( pt = typeRule; pt->flag != -1; pt++ )
	{
		if ( pt->flag == 1 ) 
		{
			ruleFlag = 1;
			if ( strcmp(pt->argname, "EtherType" ) == 0)
			{
				strncpy( etherType, pt->value, sizeof(etherType)-1 );
				if(strcmp(etherType,"0x0800") == 0)
				{		
					isEtherType= IPV4_ETHER; /*ipv4*/
				}
				else if(strcmp(etherType,"0x86dd") == 0)
				{			
					isEtherType= IPV6_ETHER; /*ipv6	*/
				}
				else if(strcmp(etherType,"0x0806") == 0)
				{	
					isEtherType= ARP_ETHER; /*ARP */
				}
			}
			else if ( 0 == strcmp(pt->argname, "dscp" ) )
			{
				IPversion = IPV4_FLAG;
			}			
			else if ( 0 == strcmp(pt->argname, "tc" ) )
			{
				IPversion = IPV6_FLAG;
			}
#if defined(TCSUPPORT_CT_JOYME4)
			else if ( 0 == strcmp(pt->argname, "tos") )
				IPversion = IPV4_FLAG;
			else if ( 0 == strcmp(pt->argname, "fl") )
				IPversion = IPV6_FLAG;
#endif
		}
	}		

	/* add 802.1p rule cmd, the proto is 802.1q */
	memset( ebtVlanRule, 0, sizeof(ebtVlanRule) );
	snprintf( ebtVlanRule, sizeof(ebtVlanRule), "%s -A QoSVlanRule%d -j QoSTypeRule%dRemark -p 0x8100", QOS_BROUTE, ruleIdx, ruleIdx );
	
	/* add type rule cmd, the proto is IPv4 */		
	if(getFlagByEtherType(isEtherType,IPV6_ETHER)) 
	{	/*not v6 */
		memset( ebtablesIPv4Rule, 0, sizeof(ebtablesIPv4Rule) );
		snprintf( ebtablesIPv4Rule, sizeof(ebtablesIPv4Rule), "%s -A QoSTypeRule%d", QOS_BROUTE, ruleIdx );
	}

	/* add type rule cmd, the proto is IPv6 */	
	if(getFlagByEtherType(isEtherType,IPV4_ETHER))
	{	/*not v4 */
		memset( ebtablesIPv6Rule, 0, sizeof(ebtablesIPv6Rule) );
		snprintf( ebtablesIPv6Rule, sizeof(ebtablesIPv6Rule), "%s -A QoSTypeRule%d", QOS_BROUTE, ruleIdx );
	}
	
	for ( pt = typeRule; pt->flag != -1; pt++ )
	{
		if ( pt->flag == 1 )
		{
			if(isEtherType)
			{
				if ( !strcmp(pt->argname, "EtherType" ))
				{
					continue;  /*Ethertype add at last */
				}

				if(isEtherType == ARP_ETHER) /*ARP */
				{		
					if ( !strcmp(pt->argname, "sip" ) || !strcmp(pt->argname, "dip" ) ||!strcmp(pt->argname, "sport" ) ||
					     !strcmp(pt->argname, "dport" ) || !strcmp(pt->argname, "dscp" ) ||!strcmp(pt->argname, "proto")
					      || !strcmp(pt->argname, "tc")
#if defined(TCSUPPORT_CT_JOYME4)
 					     || !strcmp(pt->argname, "fl") || !strcmp(pt->argname, "tos")	     
#endif
					     ) 
					{
					 	continue;  /*if ether type is APR, do not match these rule */
					}
				}
			}

			ruleFlag = 1;
			if ( !strcmp(pt->argname, "sip" ) ||!strcmp(pt->argname, "dip" ) ||!strcmp(pt->argname, "sport" ) ||
				 !strcmp(pt->argname, "dport" ) || !strcmp(pt->argname, "proto") || !strcmp(pt->argname, "tos")
				)
			{
				ipFlag = 1;
			}
			
			/* handle 802.1p & vlanid match */
			if ( !strcmp(pt->argname, "dot1p" ) || !strcmp(pt->argname, "vlanid") ) 
			{
				snprintf( ebtVlanRule + strlen(ebtVlanRule), sizeof(ebtVlanRule) - strlen(ebtVlanRule), " %s %s", pt->ipv4Cmd, pt->value );
				vlanFlag = 1;
			}
			/* for lan interface and wan interface */
			else if ( !strcmp(pt->argname, "LanIf") ) 
			{
				parserInterface( LANIF, pt->value, &lanifNum );
			}
			else if (!strcmp(pt->argname, "WanIf"))
			{
				/*fix route wan cannot be found in ebtables problem , delete all ebtables rules in Wanif mode */
				Wandisableflag = 1;
				continue;
			}
#if defined(TCSUPPORT_CT_JOYME4)
			else if (!strcmp(pt->argname, "tos"))
			{
				snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s %02x", pt->ipv4Cmd, atoi(pt->value) );
			}
			else if (!strcmp(pt->argname, "fl"))
			{
				snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s %02x", pt->ipv6Cmd, atoi(pt->value) );
			}
#endif
			else if ( !strcmp(pt->argname, "sip") || !strcmp(pt->argname, "dip") ) 
			{
				ipAddrType = checkIPAddrType( pt->value );
				/* for ipv4 */
				if(getFlagByEtherType(isEtherType,IPV6_ETHER))	/*not v6 */
				{
					snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s %s", pt->ipv4Cmd, pt->value );
				}
					
				/* for ipv6 */
				if(getFlagByEtherType(isEtherType,IPV4_ETHER))	/*not v4 */
				{
					snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s %s", pt->ipv6Cmd, pt->value );
				}	
			}
			else if ( !strcmp(pt->argname, "proto") ) 
			{
				if ( !strcmp(pt->value, "1") )
				{
#if 1
					/* for ipv4 */
					if(getFlagByEtherType(isEtherType,IPV6_ETHER))	/*not v6 */
					{
						snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s 1", pt->ipv4Cmd);
					}
						
					/* for ipv6 */
					if(getFlagByEtherType(isEtherType,IPV4_ETHER))	/*not v4 */
					{
						snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s 58", pt->ipv6Cmd);
					}
#else
					ipAddrType = 0;
					/* for ipv4 */
					if(getFlagByEtherType(isEtherType,IPV6_ETHER))	/*not v6 */
					{
						snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s %s", pt->ipv4Cmd, pt->value );
					}	
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2)
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7580)
					system("/userfs/bin/ifc del vip ipv4_proto 1");
#else
					system("/userfs/bin/fe del ip 1");
#endif
#endif/*TCSUPPORT_COMPILE*/
				}
				else if ( !strcmp(pt->value, "58") ) 
				{
					ipAddrType = 1;
					/* for ipv6 */
					if(getFlagByEtherType(isEtherType,IPV4_ETHER))	/*not v4 */
					{
						snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s %s", pt->ipv6Cmd, pt->value );
					}
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2)
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7580)
					system("/userfs/bin/ifc del vip ipv6_nxthdr 3a");
#else
					system("/userfs/bin/fe del ip 3a");
#endif
#endif/*TCSUPPORT_COMPILE*/
				}
				else
				{
					if ( !strcasecmp(pt->value, "rtp") )
					{
						/* if it is rtp proto, just continue */
						rtpFlag = 1;
						continue;
					}
					if ( !strcmp(pt->value, "all") ) 
					{
						/* if proto is 'all', just continue */
						continue;
					}
					/* for ipv4 */
					if(getFlagByEtherType(isEtherType,IPV6_ETHER))	/*not v6 */
					{
						snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s %s", pt->ipv4Cmd, pt->value );
					}
						
					/* for ipv6 */	
						
					if(getFlagByEtherType(isEtherType,IPV4_ETHER))	/*not v4 */
					{
						snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s %s", pt->ipv6Cmd, pt->value );
					}		
				}
			}
            else if ( !strcmp(pt->argname, "tos") ) 
			{
				/* for ipv4 */	
				if(getFlagByEtherType(isEtherType,IPV6_ETHER))	/*not v6 */
				{
					snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s 0x%x", pt->ipv4Cmd, atoi(pt->value) );
				}
				/* for ipv6 */
				if(getFlagByEtherType(isEtherType,IPV4_ETHER))	/*not v4 */
				{
					snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s 0x%x", pt->ipv6Cmd, atoi(pt->value) );
				}	
			}
			else if (!strcmp(pt->argname, "dscp"))
			{
				snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s %s", pt->ipv4Cmd, pt->value );
			}
			else if (!strcmp(pt->argname, "tc"))
			{
				snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s %s", pt->ipv6Cmd, pt->value );
			}
			else
			{
				/* for ipv4 */	
				if(getFlagByEtherType(isEtherType,IPV6_ETHER))	/*not v6 */
				{
					snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " %s %s", pt->ipv4Cmd, pt->value );
				}
				/* for ipv6 */
				if(getFlagByEtherType(isEtherType,IPV4_ETHER))	/*not v4 */
				{
					snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " %s %s", pt->ipv6Cmd, pt->value );
				}	
			}
		}
	}

	/* if there is no rule setting, just return */
	if ( ruleFlag == 0 )
	{	
		return 0;
	}
	
	/* if proto is rtp */
	if ( rtpFlag == 1 ) 
	{
		strncpy(qt.qm[1].type_name, "rtp_proto", sizeof(qt.qm[1].type_name)-1);

		/* borrow ebtablesWanIf to store iptables rule, use layer7 filter to match rtp proto. */
		strncpy( ebtablesWanIf, "iptables -t mangle -D FORWARD -m layer7 --l7proto rtp -j MARK --or-mark 0x04 2>/dev/null", sizeof(ebtablesWanIf)-1 );
		qosCmdExcute( ebtablesWanIf );

		strncpy( ebtablesWanIf, "iptables -t mangle -A FORWARD -m layer7 --l7proto rtp -j MARK --or-mark 0x04", sizeof(ebtablesWanIf)-1 );
		qosCmdExcute( ebtablesWanIf );
	}
	/* set wan interface and rtp protocol here. */
	/*ioctl_qostype(QOSTYPE_IOC_SET_TYPE, &qt, sizeof(qt));*/
	
	if ( vlanFlag == 1 )
	{
		qosCmdExcute( ebtVlanRule );
		/* if there is vlan type setting, jump to 802.1p rule */
		
		if( isEtherType == IPV4_ETHER)
		{	/*ipv4*/			
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " -j QoSVlanRule%d -p 0x0800", ruleIdx );
		}
		else if( isEtherType == IPV6_ETHER)
		{	/*ipv6*/
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " -j QoSVlanRule%d -p 0x86DD", ruleIdx );
		}
		else if( isEtherType == ARP_ETHER)
		{	/*ARP*/
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " -j QoSVlanRule%d -p 0x0806", ruleIdx );
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " -j QoSVlanRule%d -p 0x0806", ruleIdx );
		}
		else if ( ipFlag == 1 )
		{
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " -j QoSVlanRule%d -p 0x0800", ruleIdx );
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " -j QoSVlanRule%d -p 0x86DD", ruleIdx );
		}
		else
		{
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " -j QoSVlanRule%d", ruleIdx );
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule),  " -j QoSVlanRule%d", ruleIdx );
		}
	}
	else
	{
		/* if there is no vlan type setting, jump to remark chain directly */
		if ( rtpFlag == 1 )
		{
			/* if proto is rtp, not to create ebtables QoS Remark rule */
			memset(ebtablesIPv4Rule, 0, sizeof(ebtablesIPv4Rule));
			memset(ebtablesIPv6Rule, 0, sizeof(ebtablesIPv6Rule));
		}
		else if( isEtherType == IPV4_ETHER)
		{	/*ipv4*/
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " -j QoSTypeRule%dRemark -p 0x0800", ruleIdx );
		}
		else if( isEtherType == IPV6_ETHER)
		{	/*ipv6*/
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " -j QoSTypeRule%dRemark -p 0x86DD", ruleIdx );
		}
		else if( isEtherType == ARP_ETHER)
		{	/*ARP*/
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule),  " -j QoSTypeRule%dRemark -p 0x0806", ruleIdx );
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " -j QoSTypeRule%dRemark -p 0x0806", ruleIdx );
		}
		else if ( ipFlag == 1 )
		{
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule),  " -j QoSTypeRule%dRemark -p 0x0800", ruleIdx );
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " -j QoSTypeRule%dRemark -p 0x86DD", ruleIdx );
		}
		else
		{
			snprintf( ebtablesIPv4Rule + strlen(ebtablesIPv4Rule), sizeof(ebtablesIPv4Rule) - strlen(ebtablesIPv4Rule), " -j QoSTypeRule%dRemark", ruleIdx );
			snprintf( ebtablesIPv6Rule + strlen(ebtablesIPv6Rule), sizeof(ebtablesIPv6Rule) - strlen(ebtablesIPv6Rule), " -j QoSTypeRule%dRemark", ruleIdx );
		}
	}
	
	strncpy( tempIPv4Rule, ebtablesIPv4Rule, sizeof(tempIPv4Rule)-1 ); /* for ipv4 */
	strncpy( tempIPv6Rule, ebtablesIPv6Rule, sizeof(tempIPv6Rule)-1 ); /* for ipv6 */
	
	if ( lanifNum == 0 )
	{
		/* there is no lan interface setting */
		if ( ipAddrType == 0 ) 
		{	/* ipv4 only */
			if ( IPversion != IPV6_FLAG ) 
			{
				qosCmdExcute( ebtablesIPv4Rule );
			}
		}
		else if ( ipAddrType == 1 ) 
		{
			/* ipv6 only */
			if ( IPversion != IPV4_FLAG ) 
			{
				qosCmdExcute( ebtablesIPv6Rule );
			}
		}
		else
		{
			/* ipv4 and ipv6 both */
			if ( IPversion == IPV4_FLAG )
			{
				qosCmdExcute( ebtablesIPv4Rule );
			}
			else if ( IPversion == IPV6_FLAG )
			{
				qosCmdExcute( ebtablesIPv6Rule );
			}
			else
			{
				if ( ipFlag == 1 ) 
				{
					if (0 == Wandisableflag){
						qosCmdExcute( ebtablesIPv4Rule );
						qosCmdExcute( ebtablesIPv6Rule );
					}
				}
				else
				{
					qosCmdExcute( ebtablesIPv4Rule );
				}
			}
		}
	}
	else 
	{
		for ( i = 0; i < lanifNum; i++ )
		{
			memset(ebtablesIPv4Rule, 0, sizeof(ebtablesIPv4Rule));
			memset(ebtablesIPv6Rule, 0, sizeof(ebtablesIPv6Rule));
			if ( ipAddrType == 0 )
			{
				/* for ipv4 */
				if ( IPversion != IPV6_FLAG )
				{
					snprintf( ebtablesIPv4Rule , sizeof(ebtablesIPv4Rule), "%s -i %s",tempIPv4Rule, lanif[i] );
					qosCmdExcute( ebtablesIPv4Rule );
				}
			}
			else if ( ipAddrType == 1 ) 
			{
				/* for ipv6 */
				if ( IPversion != IPV4_FLAG )
				{	
					snprintf( ebtablesIPv6Rule, sizeof(ebtablesIPv6Rule), "%s -i %s",tempIPv6Rule, lanif[i] );
					qosCmdExcute( ebtablesIPv6Rule );
				}
			}
			else
			{
				if ( IPversion == IPV4_FLAG )
				{
					/* for ipv4 */
					snprintf( ebtablesIPv4Rule, sizeof(ebtablesIPv4Rule),  "%s -i %s", tempIPv4Rule, lanif[i] );
					qosCmdExcute( ebtablesIPv4Rule );
				}
				else if ( IPversion == IPV6_FLAG )
				{
					/* for ipv6 */
					snprintf( ebtablesIPv6Rule, sizeof(ebtablesIPv6Rule), "%s -i %s",tempIPv6Rule, lanif[i] );
					qosCmdExcute( ebtablesIPv6Rule );
				}
				else
				{
					if ( ipFlag == 1 )
					{
						snprintf( ebtablesIPv4Rule, sizeof(ebtablesIPv4Rule), "%s -i %s", tempIPv4Rule, lanif[i] );
						qosCmdExcute( ebtablesIPv4Rule );
					}
					snprintf( ebtablesIPv6Rule, sizeof(ebtablesIPv6Rule), "%s -i %s",tempIPv6Rule, lanif[i] );
					qosCmdExcute( ebtablesIPv6Rule );
				}
			}
		}
	}
	
	return 0;
}


/*______________________________________________________________________________________  
**cmd:     
**	qoscmd remark add -i IPVersion [IPv4|IPv6|all] {index} queue {queue_num} 
**														   [ipp {ippVal} tos {tosVal} dscpORTC {dscpORTCVal} 802dot1p {802dot1pVal}]]
**	qoscmd remark del -i {index}
**
**eg:
**	qoscmd remark add -i 1 IPVersion v4 queue 1
**	qoscmd remark del -i 1
**_______________________________________________________________________________________
*/
int qoscmd_lib_do_qos_remark(int type, int ruleIdx, int IPversion,int paramNum, typeParam_t paramArr[ ])
{
	int ret = -1;
	param_t *pt = NULL;
	char ebtablesRule[MAX_RULE_LENGTH] = {0};
	int queueNum = -1, dot1pVal = -1;
	int dscpORTC = -1, remarkVal = 0;
	int ipp = -1, tos = -1;
	int ruleFlag = 0;
	unsigned int rule_no = 0x02;
	
	/* check input param */
	if(( type != TYPE_ADD && type != TYPE_DEL )
		|| ( type != TYPE_DEL && paramArr == NULL )
		|| (IPversion != IPNULL_FLAG && IPversion != IPV4_FLAG && IPversion != IPV6_FLAG && IPversion != IPALL_FLAG )
		|| ( ruleIdx > MAX_TYPE_RULE_NUM_VAL-1))
	{
		return -1;
	}

	/* flush the rule first */
	snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -F QoSTypeRule%dRemark", QOS_BROUTE, ruleIdx );
	qosCmdExcute( ebtablesRule );
	
	/* if cmd is 'del', just return */
	if ( type == TYPE_DEL )
	{		
		return 0;
	}
	
	/* parser arg to  remark struct */
	ret = paramParser( paramNum, paramArr, remark );
	if ( ret == -1 )
	{
		return -1;
	}

	/* record rule index(0~15) in skb->mark */
	rule_no |= ruleIdx << 12;
	memset(ebtablesRule, 0, sizeof(ebtablesRule));
	snprintf(ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -j mark --mark-or 0x%x --mark-target CONTINUE", QOS_BROUTE, ruleIdx, rule_no);
	qosCmdExcute(ebtablesRule);

	/* add remark rule cmd */
	
	for ( pt = remark; pt->flag != -1; pt++ )
	{
		memset( ebtablesRule, 0, sizeof(ebtablesRule) );
		if ( pt->flag == 1 ) 
		{
			ruleFlag = 1;
			if ( strcmp(pt->argname, "dscpORTC" ) == 0 ) 
			{
				dscpORTC = atoi( pt->value );
                
				if ( IPversion == IPV4_FLAG)
				{
					/* for ipv4 */
					snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -p 0x0800 -j ftos --set-ftos 0x%x/0xfc", QOS_BROUTE, ruleIdx, (dscpORTC<<2) );
                    qosCmdExcute( ebtablesRule );
				}
				else if ( IPversion == IPV6_FLAG ) 
				{
					/* for ipv6 */
					snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -p 0x86DD -j tc --set-tc 0x%x", QOS_BROUTE, ruleIdx, dscpORTC );
					qosCmdExcute( ebtablesRule );
				}
				else
				{
					/* for ipv4 */
					snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -p 0x0800 -j ftos --set-ftos 0x%x/0xfc", QOS_BROUTE, ruleIdx, (dscpORTC<<2) );
					qosCmdExcute( ebtablesRule );

					/* for ipv6 */
					memset( ebtablesRule, 0, sizeof(ebtablesRule) );
					snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -p 0x86DD -j tc --set-tc 0x%x", QOS_BROUTE, ruleIdx, dscpORTC );
					qosCmdExcute( ebtablesRule );
				}
			}
			else if ( strcmp(pt->argname, "ipp" ) == 0 )
			{
				ipp = atoi( pt->value );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -p 0x0800 -j ftos --set-ftos 0x%x/0xe0", QOS_BROUTE, ruleIdx, (ipp<<5) );
				qosCmdExcute( ebtablesRule );
			}
			else if ( strcmp(pt->argname, "tos" ) == 0 ) 
			{
				tos = atoi( pt->value );
				snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -p 0x0800 -j ftos --set-ftos 0x%x/0x1e", QOS_BROUTE, ruleIdx, tos );
				qosCmdExcute( ebtablesRule );
			}
			else if ( strcmp(pt->argname, "queue" ) == 0 )
			{
				queueNum = atoi( pt->value );
				remarkVal |= (queueNum << 4);
			}
			else if ( strcmp(pt->argname, "dot1p" ) == 0 )
			{
				dot1pVal = atoi( pt->value );
				if ( dot1pVal == 0 ) 
					dot1pVal = 8;
				remarkVal |= (dot1pVal << 8);
			}
		}
	}

	if ( ruleFlag == 0 )
	{
		return 0;
	}
	
	/* excute ebtables rule to remark packet */
	memset( ebtablesRule, 0, sizeof(ebtablesRule) );
	snprintf( ebtablesRule, sizeof(ebtablesRule), "%s -A QoSTypeRule%dRemark -j mark --mark-or 0x%x", QOS_BROUTE, ruleIdx, remarkVal );
	qosCmdExcute( ebtablesRule );
	
	return 0;
}

int qoscmd_lib_do_qos_app(int type, int ruleIdx, int IPversion, appParam_t* param)
{
	int remarkVal = 0;
	int queueNum = 0;
	char iptablesRule[MAX_RULE_LENGTH*2] = {0};
	char sf_destip[128] = {0}, sf_dstProto[64] = {0};

	if(( type != TYPE_ADD && type != TYPE_DEL )
		|| ( type != TYPE_DEL && param == NULL )
		|| (IPversion != IPNULL_FLAG && IPversion != IPV4_FLAG && IPversion != IPV6_FLAG && IPversion != IPALL_FLAG ) 
		|| ( ruleIdx > MAX_APP_RULE_NUM -1))
	{
		return -1;
	}

	if ( IPversion == IPV4_FLAG)
	{
		/* flush the rule first */
		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -F QoSAppRule%d", QOS_MANGLE, ruleIdx );
		qosCmdExcute( iptablesRule );
		
		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE, ruleIdx );
		qosCmdExcute( iptablesRule );
	}
#if QOS_FOR_IPV6_OUT_PACKET
	else if ( IPversion == IPV6_FLAG )
	{
		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -F QoSAppRule%d", QOS_MANGLE_IPV6, ruleIdx );
		qosCmdExcute( iptablesRule );
		
		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE_IPV6, ruleIdx );
		qosCmdExcute( iptablesRule );
	}
#endif
	else
	{
		/* flush the rule first */
		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -F QoSAppRule%d", QOS_MANGLE, ruleIdx );
		qosCmdExcute( iptablesRule );

		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE, ruleIdx );
		qosCmdExcute( iptablesRule );
#if QOS_FOR_IPV6_OUT_PACKET
		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -F QoSAppRule%d", QOS_MANGLE_IPV6, ruleIdx );
		qosCmdExcute( iptablesRule );

		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -D OUTPUT -j QoSAppRule%d 2>/dev/null", QOS_MANGLE_IPV6, ruleIdx );
		qosCmdExcute( iptablesRule );
#endif
	}

	memset( iptablesRule, 0, sizeof(iptablesRule) );

	/* if the cmd is 'del', just return */
	if ( type == TYPE_DEL)
	{
		return 0;
	}
	
	if(strlen(param->appName) == 0 || strlen(param->dstIP) == 0 || strlen(param->dstProto) == 0 || strlen(param->queueNum) == 0)
	{
		return -1;
	}
		
	api_escape_parameter(param->dstIP, sf_destip, sizeof(sf_destip));
	api_escape_parameter(param->dstProto, sf_dstProto, sizeof(sf_dstProto));
	if ( IPversion == IPV4_FLAG )
	{
		queueNum = atoi(param->queueNum);
		queueNum = (queueNum << 4);
		remarkVal |=  queueNum;

		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -A OUTPUT -j QoSAppRule%d", QOS_MANGLE, ruleIdx );
		qosCmdExcute( iptablesRule );

		if ( 0 == strcmp("tr069", param->appName) && 0 != strcmp("tcp", param->dstProto) )
		{
			memset(iptablesRule, 0, sizeof(iptablesRule));
			snprintf( iptablesRule, sizeof(iptablesRule), "%s -A QoSAppRule%d -d %s -p tcp --dport %d -j MARK --set-mark 0x%x/0x000000f0", QOS_MANGLE, ruleIdx, sf_destip, param->dstPort, remarkVal );
			qosCmdExcute( iptablesRule );
			memset(iptablesRule, 0, sizeof(iptablesRule));
			snprintf( iptablesRule, sizeof(iptablesRule), "%s -A QoSAppRule%d -p tcp --sport %s -j MARK --set-mark 0x%x/0x000000f0", QOS_MANGLE, ruleIdx, sf_dstProto, remarkVal );
			qosCmdExcute( iptablesRule );
		}
		else
		{
			memset(iptablesRule, 0, sizeof(iptablesRule));
			snprintf( iptablesRule, sizeof(iptablesRule), "%s -A QoSAppRule%d -d %s -p %s --dport %d -j MARK --set-mark 0x%x/0x000000f0", QOS_MANGLE, ruleIdx, sf_destip, sf_dstProto, param->dstPort, remarkVal );
			qosCmdExcute( iptablesRule );
		}
	}
#if QOS_FOR_IPV6_OUT_PACKET
	else if ( IPversion == IPV6_FLAG )
	{
		queueNum = atoi( param->queueNum );
		queueNum = (queueNum << 4);
		remarkVal |=  queueNum;

		memset(iptablesRule, 0, sizeof(iptablesRule));
		snprintf( iptablesRule, sizeof(iptablesRule), "%s -A OUTPUT -j QoSAppRule%d", QOS_MANGLE_IPV6, ruleIdx );
		qosCmdExcute( iptablesRule );
		
		if ( 0 == strcmp("tr069", param->appName) && 0 != strcmp("tcp", param->dstProto) )
		{
			memset(iptablesRule, 0, sizeof(iptablesRule));
			snprintf( iptablesRule, sizeof(iptablesRule), "%s -A QoSAppRule%d -d %s -p tcp --dport %d -j MARK --set-mark 0x%x/0x000000f0", QOS_MANGLE_IPV6, ruleIdx, sf_destip, param->dstPort, remarkVal );
			qosCmdExcute( iptablesRule );
			memset(iptablesRule, 0, sizeof(iptablesRule));
			snprintf( iptablesRule, sizeof(iptablesRule), "%s -A QoSAppRule%d -p tcp --sport %s -j MARK --set-mark 0x%x/0x000000f0", QOS_MANGLE_IPV6, ruleIdx, sf_dstProto, remarkVal );
			qosCmdExcute( iptablesRule );
		}
		else
		{
			memset(iptablesRule, 0, sizeof(iptablesRule));
			snprintf( iptablesRule, sizeof(iptablesRule), "%s -A QoSAppRule%d -d %s -p %s --dport %d -j MARK --set-mark 0x%x/0x000000f0", QOS_MANGLE_IPV6, ruleIdx, sf_destip, sf_dstProto, param->dstPort, remarkVal );
			qosCmdExcute( iptablesRule );
		}
	}
#endif
	else
	{
		return -1;
	}

	return 0;
}

int qoscmd_lib_do_qos_discPQ(char upBandWith[], char queueMask[])
{
	uint32 uplinkBandwidth = 0;
	int qm = 15;

	/* get uplink bandwidth */
	if(upBandWith[0] != '\0')
	{
		uplinkBandwidth = (uint32)atof( upBandWith);
	}
	else
	{
		uplinkBandwidth = MAX_Uplink_BAND_WIDTH;
	}

	if(queueMask[0] != '\0')
	{
		qm = atoi( queueMask );
	}
	
	qoscmd_lib_do_en75xxQoSDiscPQ(uplinkBandwidth,qm);
	return 0;
}

int qoscmd_lib_do_qos_discWRR(char wrrPri[][32],char upBandWith[], char queueMask[],char forceBW[] )
{
	uint32 uplinkBandwidth;
	int  wTotal = 0;
	int w[WRR_QUEUE_NUM];
	int i, qm = 15;/* 0x0F */
	int forcebw = 0;

	uplinkBandwidth = 0;
	for(i = 0; i < WRR_QUEUE_NUM; i++ )
	{
		if(wrrPri[i][0] == '\0')
		{
			return -1;
		}
	}
	
	/* get uplink bandwidth */
	if(upBandWith[0] != '\0')
	{
		uplinkBandwidth = (uint32)atof( upBandWith);
	}
	else
	{
		uplinkBandwidth = MAX_Uplink_BAND_WIDTH;
	}

	if(queueMask[0] != '\0')
	{
		qm = atoi( queueMask );
	}

	if(strcmp(forceBW,"Yes") == 0)
	{
		forcebw = 1;
	}
	else
	{
		forcebw = 0;
	}
	
	/* get weight from argv */
	for ( i = 0; i < WRR_QUEUE_NUM; i++ )
	{	
		w[i] = atoi( wrrPri[i] );
		wTotal += w[i];
	}
	
	qoscmd_lib_do_en75xxQoSDiscWRR(uplinkBandwidth,qm,w,wTotal,forcebw);
	return 0;

}

int qoscmd_lib_do_qos_discCAR(char carPri[][32],char upBandWith[], char queueMask[],char hwCar[] )
{
	uint32 uplinkBandwidth;
	uint32 b[CAR_QUEUE_NUM];
	int i = 0;
	int qm = 63;
	int meter_group_support = 1;

	uplinkBandwidth = 0;
	for(i = 0; i < CAR_QUEUE_NUM; i++ )
	{
		if(carPri[i][0] == '\0')
		{
			return -1;
		}
	}
	
	if(hwCar[0] != '\0')
	{
		meter_group_support = atoi(hwCar);
	}

/* get uplink bandwidth */		
	if(upBandWith[0] != '\0')
	{
		uplinkBandwidth = (uint32)atof( upBandWith);
	}
	else
	{
		uplinkBandwidth = MAX_Uplink_BAND_WIDTH;
	}

	if(queueMask[0] != '\0')
	{
		qm = atoi( queueMask );
	}	
	
	/* get bandwidth from input argument */
	for ( i = 0; i < CAR_QUEUE_NUM; i++ ) 
	{
		b[i] = (uint32)atof( carPri[i] );	
	}
	
#if defined(TCSUPPORT_CPU_EN7580)
	qoscmd_lib_do_en7580QoSDiscCAR(uplinkBandwidth, qm, b);
#else
	qoscmd_lib_do_en75xxQoSDiscCAR(uplinkBandwidth,qm,b,meter_group_support);
#endif

	return 0;
}

int qoscmd_lib_do_qos_config_rate(char upBandWith[])
{
	uint32 uplinkBandwidth = 0;

	if(upBandWith[0] != '\0')
	{
		uplinkBandwidth = (uint32)atof( upBandWith);
	}
	else
	{
		uplinkBandwidth = MAX_Uplink_BAND_WIDTH;
	}

	fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE_BPS, uplinkBandwidth/8);

	return 0;
}

int qoscmd_lib_do_qos_enable(int enable)
{
	int i = 0;
	uint32 uplinkBandwidth = 0;

	if (!enable){
#if defined(TCSUPPORT_CPU_EN7580)
		/*disable meter ratelimit of car mode*/
		for(i = 0; i < CAR_QUEUE_NUM; i++)
		{
			 fe_lib_set_car_mode_ratelimit(0, 0, i);
		}
#else	
		/*clear meter group*/
		system(PPE_METER_DEL_ALL_CMD); 
#endif

		fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE_BPS, uplinkBandwidth/8);

		doValPut("/proc/qdma_wan/mac_qos_flag", "NULLQOS");
		
#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
		/* set hw to hw_sp mode */	
		doValPut("/proc/qdma_wan/mac_qos", "0 0 0 0 0 0 0 0 0");
#else
		/* set hw to hw_sp mode */
		doValPut("/proc/qdma_wan/mac_qos", "0 0 0 0 0");
#endif
	}

return 0;

}

int qoscmd_lib_do_downlinkqos_dscpremark(int enable, char *wan_if, int dscp)
{
	char cmd[256] = {0};
	char sf_wan_if[32] = {0};

	bzero(sf_wan_if, sizeof(sf_wan_if));
	api_escape_parameter(wan_if, sf_wan_if, sizeof(sf_wan_if));

	snprintf(cmd, sizeof(cmd), "/usr/bin/smuxctl setdscpdown %d %s %d", enable, sf_wan_if, dscp);
	qosCmdExcute(cmd);

	return 0;
	
}
