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
#include "global.h"
#include "qosrule.h"
#include "tcversion.h"
#include <linux/qos_type.h>
#include "libcompileoption.h"
#include "libapi_lib_fe.h"
#include "libapi_lib_utility.h"
#if defined(TCSUPPORT_CPU_EN7521)
#include "libapi_lib_switchmgr.h"
#endif
#include <lan_port/lan_port_info.h>


static int TCSUPPORT_HW_QOS_VAL = 0;
static int MAX_TYPE_RULE_NUM_VAL = 16;

static void qosCmdExcute( char *rule );
static uint32 getDefMaxUplinkBandwidth(void);
static int checkCmdAndRule( int argc, char *argv[], int *pRuleIdx, char *ipVer, int maxRuleNum );
#if 0
static int ioctl_get_ifindex(char *dev, unsigned int *wanIf );
#endif

static int doQoSDiscPQ(int argc, char *argv[], void *p);
static int doQoSDiscWRR(int argc, char *argv[], void *p);
static int doQoSDiscCAR(int argc, char *argv[], void *p);
static int doQoSDiscRate(int argc, char *argv[], void *p);
static int doQoSDiscEnable(int argc, char *argv[], void *p);

static int doQoSSwitch(int argc, char *argv[], void *p);
static int doQoSDiscpline(int argc, char *argv[], void *p);
static int doQoSRemark(int argc, char *argv[], void *p);
static int doQoSType(int argc, char *argv[], void *p);
static int doQoSApp(int argc, char *argv[], void *p);
static int doQoSVlan(int argc, char *argv[], void *p);
static int doQoSHelp(int argc, char *argv[], void *p);

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
	{0,			"tos",			"--ip-tos",		NULL,			0},
	{0,			"dscpORTC",		"--ip-dscp",	"--ip6-tclass",	0},
	{0,			"proto",		"--ip-proto",	"--ip6-proto",	0},
	{0,			"vlanid",		"--vlan-id",	"--vlan-id",	0},
	{0,			"dot1p",		"--vlan-prio",	"--vlan-prio",	0},
	{0, 		"EtherType",	"-p",			"-p",			0},
	{-1,		NULL,			NULL,			NULL,			0}
};

static cmds_t QoSDiscplineCmds[] = 
{
	{"PQ",					doQoSDiscPQ,		0x10,		0,		NULL},
	{"WRR",					doQoSDiscWRR,		0x10,		0,		NULL},
	{"CAR",					doQoSDiscCAR,		0x10,		0,		NULL},
	{"Rate",				doQoSDiscRate,		0x10,		0,		NULL},
	{"Enable",				doQoSDiscEnable,	0x10,		0,		NULL},
	{NULL,					NULL,				0x10,		0,		NULL}
};


static cmds_t QosCmds[] = 
{
    {"switch",             doQoSSwitch,       	0x10,   	0,  	NULL},
    {"discpline",          doQoSDiscpline,      0x10,   	0,  	NULL},
    {"remark",		       doQoSRemark,    		0x10,   	0,  	NULL},
    {"type",               doQoSType,       	0x10,   	0,  	NULL},
	{"app",                doQoSApp,	     	0x10,   	0,  	NULL},
    {"vlan",			   doQoSVlan,			0x10,		0,		NULL},
    {"help",               doQoSHelp,        	0x10,   	0,  	NULL},
    {NULL,          	   NULL,               	0x10,   	0,  	NULL}
};

/*______________________________________________________________________________
**function :
*		doEN75xxQosSwitch
**description:
*    		Use this function to set qos switch if support EN75XX or EN7580 CPU.
**parameters:
*		[argc]:Numbers of command line parameters.
		[agv]: Array of command line parameters.

**global:
* 		none
**return: 
		[-1]: Fail.
		[0]: Success.
*		
**____________________________________________________________________________*/

static int doEN75xxQosSwitch(int argc, char *argv[], void *p)
{
	int IPversion = 0;
	int QoSsw = 0;
	
	if ( argc < 4 ) {
		goto ErrorHandle;
	}
	if ( strcmp( argv[1], "on" ) == 0)
	{
		QoSsw = SW_ON;
	}
	else if( strcmp( argv[1], "off" ) == 0)
	{
		QoSsw = SW_OFF;
	}
	else	
	{
		goto ErrorHandle;
	}

	if ( strcmp( argv[2], "IPVersion" ) ==0)
	{
		if(strcmp( argv[3], "v4" ) == 0)
		{
			IPversion = IPV4_FLAG;
		}
		else if(strcmp( argv[3], "v6" ) == 0)
		{
			IPversion = IPV6_FLAG;
		}
		else if(strcmp( argv[3], "all") == 0)
		{
			IPversion = IPALL_FLAG; 
		}
		else
		{
			goto ErrorHandle;
		}
	}
	else
	{
		goto ErrorHandle;
	}	
	qoscmd_lib_do_qos_switch(QoSsw,IPversion);	
	return 0;
	
ErrorHandle:
	printf( "==>doQoSSwitch(): wrong parameters. \r\n"
			"           usage: \r\n"
			"           qosrule switch on|off IPVersion {v4|v6|all}\r\n" );
	return -1;
}

/*___________________________________________________________________________
**cmd:     
**	qosrule switch {on|off} IPVersion [v4|v6|all]
**
**eg:
**	qosrule switch on IPVersion v4
**____________________________________________________________________________
*/
static int doQoSSwitch(int argc, char *argv[], void *p)
{
	if(TCSUPPORT_CPU_EN75XX_VAL   || TCSUPPORT_CPU_EN758X_VAL)
	{
		return doEN75xxQosSwitch(argc, argv, p);
	}
	else
	{
		return -1;
	}
}

static int doQoSDiscPQ(int argc, char *argv[], void *p)
{
	uint32 uplinkBandwidth;
	int qm = 15;
	uplinkBandwidth	=0;

	/* get uplink bandwidth */
	if ( argc == 1 ) {
		/* it does not set uplink bandwidth, 
		   it will user default max uplink bandwidth 
		*/
		uplinkBandwidth = getDefMaxUplinkBandwidth();
	}
	else if ( argc >= 3 && strcmp( argv[1], "uplink-bandwidth" ) == 0 ) {
		uplinkBandwidth = (uint32)atof( argv[2] );
		if ( argc == 5 && strcmp( argv[3], "queuemask" ) == 0 ) {
			qm = atoi( argv[4] );
		}
	}
	else if ( argc == 3 && strcmp( argv[1], "queuemask" ) == 0 ) {
		uplinkBandwidth = getDefMaxUplinkBandwidth();
		qm = atoi( argv[2] );
	}
	else {
		goto ErrorHandle;
	}

    if(TCSUPPORT_CPU_EN75XX_VAL||TCSUPPORT_CPU_EN758X_VAL){
		qoscmd_lib_do_en75xxQoSDiscPQ(uplinkBandwidth,qm);
    }

	return 0;
	
ErrorHandle:
	printf( "==>doQoSDiscPQ(): wrong parameters. \r\n"
			"           usage: \r\n" 
			"           qosrule discpline PQ [uplink-bandwidth {bandwidth}] [queuemask {qm}]\r\n" );
	return -1;
}

static int doQoSDiscWRR(int argc, char *argv[], void *p)
{
	uint32 uplinkBandwidth, burst_val;
	int  wTotal = 0, w[8] = {0};
	double  wTotal_f = 0, w_f[8];
	int i = 0, qm = 15;/* 0x0F */
	int forcebw = 0, arg_num = 0;
	int qos_queue_num = 0;
	uplinkBandwidth	=0;

	/* get uplink bandwidth */
	if ((argc == 6 || argc == 8 || argc == 10) && !strcmp(argv[argc-1], "forcebw")) {
		forcebw = 1;
		arg_num = argc - 1;
	}
	else if (argc == 5 || argc == 7 || argc == 9) {
		forcebw = 0;
		arg_num = argc;
	}
	else {
		goto ErrorHandle;
	}
	
	if ( arg_num == 5 ) {
		uplinkBandwidth = getDefMaxUplinkBandwidth();
	}
	else if ( arg_num >= 7 && strcmp( argv[5], "uplink-bandwidth" ) == 0 ) {
		uplinkBandwidth = (uint32)atof( argv[6] );
		if ( arg_num == 9 && strcmp( argv[7], "queuemask" ) == 0 ) {
			qm = atoi( argv[8] );
		}
	}
	else if ( arg_num == 7 && strcmp( argv[5], "queuemask" ) == 0 ) {
		uplinkBandwidth = getDefMaxUplinkBandwidth();
		qm = atoi( argv[6] );
	}
	else {
		goto ErrorHandle;
	}

	/* set burst val (by test result) */
	burst_val = (uint32)(uplinkBandwidth * 55 / 100);
	
	/* get weight from argv */
	qos_queue_num = TCSUPPORT_QOS_EIGHT_QUEUE_VAL ? 8 : WRR_QUEUE_NUM;
	for ( i = 0; i < qos_queue_num; i++ ) {
		if(TCSUPPORT_HW_QOS_VAL || TCSUPPORT_CT_HWQOS_VAL || TCSUPPORT_CT_SWQOS_VAL || TCSUPPORT_CPU_EN75XX_VAL){
			w[i] = atoi( argv[i + 1] );
			wTotal += w[i];
		}
		else{
			w_f[i] = atof( argv[i + 1] );
			wTotal_f += w_f[i];
		}
	}

    if(TCSUPPORT_CPU_EN75XX_VAL||TCSUPPORT_CPU_EN758X_VAL){
		qoscmd_lib_do_en75xxQoSDiscWRR(uplinkBandwidth,qm,w,wTotal,forcebw);
    }

	return 0;
ErrorHandle:
	printf( "==>doQoSDiscWRR(): wrong parameters. \r\n"
			"            usage: \r\n" 
			"            qosrule discpline WRR w1 w2 w3 w4 [uplink-bandwidth {bandwidth}] [queuemask {qm}] [forcebw]\r\n" );
	return -1;
}

static int doQoSDiscCAR(int argc, char *argv[], void *p)
{
	uint32 uplinkBandwidth, b[8] = {0}, burst_val, sub_bur_val[8] = {0};
	int i, qm = 63;
	int meter_group_support = 1;
	int qos_queue_num = 0;

	uplinkBandwidth	=0;
    if(TCSUPPORT_CPU_EN75XX_VAL||TCSUPPORT_CPU_EN758X_VAL){
		/* get uplink bandwidth */
		if ( argc == 8 ) {
			uplinkBandwidth = getDefMaxUplinkBandwidth();
			meter_group_support = atoi(argv[7]);
		}
		else if ( argc >= 10 && strcmp( argv[7], "uplink-bandwidth" ) == 0 ) {
			uplinkBandwidth = (uint32)atof( argv[8] );
			if ( argc == 12 && strcmp( argv[9], "queuemask" ) == 0 ) {
				qm = atoi( argv[10] );
				meter_group_support = atoi(argv[11]);
			}else{
				meter_group_support = atoi(argv[9]);
			}
		}
		else if ( argc == 10 && strcmp( argv[7], "queuemask" ) == 0 ) {
			uplinkBandwidth = getDefMaxUplinkBandwidth();
			qm = atoi( argv[8] );
			meter_group_support = atoi(argv[9]);
		}
		else {
				goto ErrorHandle;
		}

    }else{
		/* get uplink bandwidth */
		if ( argc == 7 ) {
			uplinkBandwidth = getDefMaxUplinkBandwidth();
		}
		else if ( argc >= 9 && strcmp( argv[7], "uplink-bandwidth" ) == 0 ) {
			uplinkBandwidth = (uint32)atof( argv[8] );
			if ( argc == 11 && strcmp( argv[9], "queuemask" ) == 0 ) {
				qm = atoi( argv[10] );
			}
		}
		else if ( argc == 9 && strcmp( argv[7], "queuemask" ) == 0 ) {
			uplinkBandwidth = getDefMaxUplinkBandwidth();
			qm = atoi( argv[8] );
		}
		else {
			goto ErrorHandle;
		}
    }
	/* set burst val (by test result) */
	burst_val = (uint32)(uplinkBandwidth * 55 / 100);
	
	/* get bandwidth from input argument */
	qos_queue_num = TCSUPPORT_QOS_EIGHT_QUEUE_VAL ? 8 : CAR_QUEUE_NUM;
	for ( i = 0; i < qos_queue_num; i++ ) {
		b[i] = (uint32)atof( argv[i + 1] );	
		sub_bur_val[i] = (uint32)(b[i] * 55 / 100);
	}
    
	if( TCSUPPORT_CPU_EN758X_VAL)
	{
		qoscmd_lib_do_en7580QoSDiscCAR(uplinkBandwidth, qm, b);
        return 0;
    }

	if(TCSUPPORT_CPU_EN75XX_VAL)
	{
		qoscmd_lib_do_en75xxQoSDiscCAR(uplinkBandwidth,qm,b,meter_group_support);
    	return 0;
    }
	
	return 0;
	
ErrorHandle:
	printf( "==>doQoSDiscCAR(): wrong parameters. \r\n"
			"            usage: \r\n" 
			"            qosrule discpline CAR b1 b2 b3 b4 b5 b6 [uplink-bandwidth {bandwidth}] [queuemask {qm}]\r\n" );
	return -1;
}

static int doQoSDiscRate(int argc, char *argv[], void *p)
{
	if ( argc == 3 && strcmp( argv[1], "uplink-bandwidth" ) == 0){
		qoscmd_lib_do_qos_config_rate(argv[2]);
	}
	else{
		goto ErrorHandle;
	}
		
	return 0;
ErrorHandle:
	printf( "==>doQoSDiscRate(): wrong parameters. \r\n"
			"			 usage: \r\n" 
			"			 qosrule discpline Rate [uplink-bandwidth {bandwidth}]\r\n" );
	return -1;
}

static int doQoSDiscEnable(int argc, char *argv[], void *p)
{
	if ( argc == 2){
		qoscmd_lib_do_qos_enable(atoi(argv[1]));
	}
	else{
		goto ErrorHandle;
	}
		
	return 0;
ErrorHandle:
	printf( "==>doQoSDiscEnable(): wrong parameters. \r\n"
			"			 usage: \r\n" 
			"			 qosrule discpline Enable {1/0}\r\n" );
	return -1;
}



/*______________________________________________________________________________
**function :
*		setParamArr
**description:
*    		Use this function to set argv to  the array of paramArr.
**parameters:
*		[argc]:Numbers of command line parameters.
		[agv]: Array of command line parameters.
		[paramNum]: Storage the element number of the array of paramArr.
		[paramArr]:Storage command line parameters.
		
**global:
* 		none
**return: 
		[-1]: Fail.
		[0]: Success.
*		
**____________________________________________________________________________*/

static int setParamArr( int argc, char *argv[], param_t* pt, int* paramNum, typeParam_t paramArr[] )
{
	param_t *p = NULL;
	int i = 0;
	int j = 0;

	/* check arg valid */
	if ( argc % 2 != 0 ) {
		printf( "==>paramParser(): valid arg.\n" );
		return -1;
	}
	if ( pt == NULL ) {
		printf( "==>paramParser(): valid arg.\n" );
		return -1;
	}
	
	for ( p = pt; p->flag != -1; p++ ) 
	{
		for ( i = 0; i < argc - 4; i+=2 ) 
		{		
			if ( strcmp(p->argname, argv[i + 4]) == 0 ) 
			{
				strncpy(paramArr[j].paramName, argv[i + 4], sizeof(paramArr[j].paramName)-1);
				strncpy(paramArr[j].value, argv[i + 5], sizeof(paramArr[j].value)-1);
				j++;
			}
		}
	}
	*paramNum = j;

	return 0;
}


static int doEN75xxQoSRemark(int argc, char *argv[], void *p)
{
	int ret = 0;
	int type = 0;
	int ruleIdx = 0;
	char ipVer[16] = {0};
	int IPversion = 0;
	int paramNum = 0;
	typeParam_t paramArr[MAX_TYPE_PARAM_NUM];

	/* check cmd and rule index */
	ret = checkCmdAndRule( argc, argv, &ruleIdx, ipVer, MAX_TYPE_RULE_NUM_VAL );
	if ( ret == -1 ) {
		goto ErrorHandle;
	}
	
	if ( strcmp(argv[1], "add") == 0 ) {		
		type = TYPE_ADD;	
	}
	else{
		type = TYPE_DEL;	
	}

	if ( strcmp(ipVer, "v6") == 0 ) {
		IPversion = IPV6_FLAG;
	}
	else if(strcmp(ipVer, "v4") == 0 ) {
		IPversion = IPV4_FLAG;
	}
	else if(strcmp(ipVer, "all") == 0) {
		IPversion = IPALL_FLAG;
	}

	memset(paramArr, 0, sizeof(paramArr));
	ret = setParamArr( argc, argv, remark, &paramNum, paramArr );
	if ( ret == -1 ) {
		goto ErrorHandle;
	}
	
	qoscmd_lib_do_qos_remark(type, ruleIdx, IPversion, paramNum, paramArr);
	
	return 0;
	
ErrorHandle:
	printf( "==>doQoSRemark(): wrong parameters. \r\n"
			"           usage: \r\n" 
			"           qosrule remark add -i {index} IPVersion [IPv4|IPv6|all] queue {queue_num} \r\n"
			"										 [ipp {ippVal} tos {tosVal} dscpORTC {dscpORTCVal} dot1p {802dot1pVal}]] \r\n" 
			"			qosrule remark del -i {index} \r\n" );
	return -1;
}

/*______________________________________________________________________________________  
**cmd:     
**	qosrule remark add -i IPVersion [IPv4|IPv6|all] {index} queue {queue_num} 
**														   [ipp {ippVal} tos {tosVal} dscpORTC {dscpORTCVal} dot1p {802dot1pVal}]]
**	qosrule remark del -i {index}
**
**eg:
**	qosrule remark add -i 1 IPVersion v4 queue 1
**	qosrule remark del -i 1
**_______________________________________________________________________________________
*/

static int doQoSRemark(int argc, char *argv[], void *p)
{
	if(TCSUPPORT_CPU_EN75XX_VAL  || TCSUPPORT_CPU_EN758X_VAL)
	{
		return doEN75xxQoSRemark(argc, argv, p);
	}
	else
	{
		return -1;
	}
}

static int doQoSDiscpline(int argc, char *argv[], void *p)
{
	return subcmd(QoSDiscplineCmds, argc, argv, p);
}



/*______________________________________________________________________________
**function :
*		doEN75xxQoSType
**description:
*    		Use this function to set qos type if support EN75XX or EN7580 CPU.
**parameters:
*		[argc]:Numbers of command line parameters.
		[agv]: Array of command line parameters.
		
**global:
* 		none
**return: 
		[-1]: Fail.
		[0]: Success.
*		
**____________________________________________________________________________*/
static int doEN75xxQoSType(int argc, char *argv[], void *p)
{
	int ret = 0;
	int type = 0;
	int ruleIdx = 0;
	char ipVer[16] = {0};
	int IPversion = 0;
	int paramNum = 0;
	typeParam_t paramArr[MAX_TYPE_PARAM_NUM];

	/* check cmd and rule index */
	ret = checkCmdAndRule( argc, argv, &ruleIdx, ipVer, MAX_TYPE_RULE_NUM_VAL );
	if ( ret == -1 ) {
		goto ErrorHandle;
	}
	
	if ( strcmp(argv[1], "add") == 0 ) {		
		type = TYPE_ADD;	
	}
	else{
		type = TYPE_DEL;	
	}

	if ( strcmp(ipVer, "v6") == 0 ) {
		IPversion = IPV6_FLAG;
	}
	else if(strcmp(ipVer, "v4") == 0 ) {
		IPversion = IPV4_FLAG;
	}
	else if(strcmp(ipVer, "all") == 0) {
		IPversion = IPALL_FLAG;
	}

	memset(paramArr, 0, sizeof(paramArr));
	ret = setParamArr( argc, argv, typeRule, &paramNum, paramArr );
	if ( ret == -1 ) {
		goto ErrorHandle;
	}
	
	qoscmd_lib_do_qos_type(type, ruleIdx, IPversion, paramNum, paramArr);
	return 0;
	
ErrorHandle:
	printf( "==>doQoSType(): wrong parameters. \r\n"
			"         usage: \r\n" 
			"         qosrule type add -i {index} [{type_name} {type_value} ...] \r\n" 
			"	      qosrule type del -i {index} \r\n" );
	return -1;
}

/*___________________________________________________________________________
**cmd:     
**	qosrule type add -i {index} IPVersion {v4|v6|all} [{type_name} {type_value} ...]
**	qosrule type del -i {index} 
**
**eg:
**	qosrule type add -i 1 IPVersion v4 sip 192.168.85.1 
**	qosrule type del -i 1
**____________________________________________________________________________
*/

static int doQoSType(int argc, char *argv[], void *p)
{
	if(TCSUPPORT_CPU_EN75XX_VAL  || TCSUPPORT_CPU_EN758X_VAL)
	{
		return doEN75xxQoSType(argc, argv, p);
	}
	else
	{
		return -1;
	}
}


/*______________________________________________________________________________
**function :
*		doEN75xxQoSApp
**description:
*    		Use this function to set qos app if support EN75XX or EN7580 CPU.
**parameters:
*		[argc]:Numbers of command line parameters.
		[agv]: Array of command line parameters.
		
**global:
* 		none
**return: 
		[-1]: Fail.
		[0]: Success.
*		
**____________________________________________________________________________*/

static int doEN75xxQoSApp(int argc, char *argv[], void *p)
{
	int ret = 0;
	int type = 0;
	int ruleIdx = 0;
	char ipVer[16] = {0} ;
	int IPversion = 0;
	appParam_t param;

	memset(&param, 0, sizeof(param));
	ret = checkCmdAndRule( argc, argv, &ruleIdx, ipVer, MAX_APP_RULE_NUM );
	if ( ret == -1 ) {
		goto ErrorHandle;
	}

	if( argc != 11 ){
		goto ErrorHandle;
	}
	
	if ( strcmp(argv[1], "add") == 0 ) {		
		type = TYPE_ADD;	
		strncpy(param.appName, argv[6], sizeof(param.appName)-1);
		strncpy(param.dstIP, argv[7], sizeof(param.dstIP)-1);
		param.dstPort = atoi(argv[8]);
		strncpy(param.dstProto, argv[9], sizeof(param.dstProto)-1);
		strncpy(param.queueNum, argv[10], sizeof(param.queueNum)-1);
	}
	else{
		type = TYPE_DEL;	
	}

	if ( strcmp(ipVer, "v6") == 0 ) {
		IPversion = IPV6_FLAG;
	}
	else if(strcmp(ipVer, "v4") == 0 ) {
		IPversion = IPV4_FLAG;
	}
	else if(strcmp(ipVer, "all") == 0) {
		IPversion = IPALL_FLAG;
	}

	qoscmd_lib_do_qos_app(type, ruleIdx, IPversion, &param);

	return 0;
	
ErrorHandle:
	printf( "==>doQoSApp(): wrong parameters. \r\n"
			"         usage: \r\n" 
			"         qosrule app add -i {index} IPVersion {v4|v6} {app_name} {dst_ip} {dst_proto} {dst_port} {queue_num} \r\n" 
			"	      qosrule app del -i {index} \r\n" );
	return -1;
}

/*___________________________________________________________________________
**cmd:     
**	qosrule app add -i {index} IPVersion {v4|v6} {app_name} {dst ip} {dst proto} {dst port} {queue_num}
**	qosrule app del -i {index}
**
**eg:
**	qosrule app add -i 1 IPVersion v4 tr069 122.193.99.166 tcp 7547 1
**	qosrule app del -i 1
**____________________________________________________________________________
*/

static int doQoSApp(int argc, char *argv[], void *p)
{
	if(TCSUPPORT_CPU_EN75XX_VAL  || TCSUPPORT_CPU_EN758X_VAL)
	{
		return doEN75xxQoSApp(argc, argv, p);
	}
	else
	{
		return -1;
	}
}

/*______________________________________________________________________________
**function :
*		doEN75xxQoSVlan
**description:
*    		Use this function to set qos vlan  if support EN75XX or EN7580 CPU.
**parameters:
*		[argc]:Numbers of command line parameters.
		[agv]: Array of command line parameters.
		
**global:
* 		none
**return: 
		[-1]: Fail.
		[0]: Success.
*		
**____________________________________________________________________________*/

static int doEN75xxQoSVlan(int argc, char *argv[], void *p)
{
	char *pvpi = NULL;
	char *pvci = NULL;
	char *vid = NULL;
	int i = 0;
	int sw = 0;
	
	/* check input param */
	if ( argc != 6 ) {
		goto ErrorHandle;
	}

	if ( strcmp( argv[1], "on" ) != 0 && strcmp( argv[1], "off" ) != 0 ) {
		goto ErrorHandle;
	}

	if ( (strcmp(argv[2], "vpidotvci")!=0) || (strcmp(argv[4], "vlanid")!=0) ) {
		goto ErrorHandle;
	}

	if ( !strcmp( argv[1], "on" ) )
	{
		sw = 1;
	}
	else
	{	
		sw = 0;
	}
	
	/* vpi & vci */
	pvpi = strtok(argv[3], ".");
	pvci = strtok(NULL, ".");
	if ( !pvpi || !pvci )
	{
		printf( "Wrong format for {vpi.vci} input\r\n" );
		return -1;
	}
	else 
	{
		for ( i = 0; i < strlen(pvpi); i++ )
		{
			if ( !isdigit(pvpi[i]) )
			{
				printf( "==>doQoSVlan():invalid vpi\n" );
				return -1;
			}
		}
		for ( i = 0; i < strlen(pvci); i++ )
		{
			if ( !isdigit(pvci[i]) )
			{
			    printf( "==>doQoSVlan():invalid vci\n" );
			    return -1;
			}
       	 }
	}
	
	/* vid */
	vid = argv[5];
	for ( i = 0; i < strlen(vid); i++ )
	{
		if ( !isdigit(vid[i]) )
		{
			printf( "==>doQoSVlan():invalid vid\n" );
			return -1;
		}
	}

	return qoscmd_lib_do_qos_vlanid(sw, atoi(pvpi), atoi(pvci), atoi(vid));
	
ErrorHandle:
	printf( "==>doQoSVlan(): wrong parameters. \r\n"
			"         usage: \r\n" 
			"         qosrule vlan {on|off} vpidotvci {vpi.vci} vlanid {vlanid} \r\n" );
	return -1;
}

/*___________________________________________________________________________
**cmd:     
**	qosrule vlan {on|off} vpidotvci {vpi.vci} vlanid {vlanid}
**
**eg:
**	qosrule vlan on vpidotvci 0.35 vlanid 12 
**____________________________________________________________________________
*/
static int doQoSVlan(int argc, char *argv[], void *p)
{
	
	if(TCSUPPORT_CPU_EN75XX_VAL   || TCSUPPORT_CPU_EN758X_VAL)
	{
		return doEN75xxQoSVlan(argc, argv, p);
	}
	else
	{
		return -1;
	}
}

static int doQoSHelp(int argc, char *argv[], void *p)
{
	printf("Usage: qosrule switch {on|off} IPVersion [v4|v6|all]\r\n"
		   "	   qosrule discpline PQ [uplink-bandwidth {b}] [queuemask {qm}]\r\n"
		   "	   qosrule discpline WRR {w1 w2 w3 w4} [uplink-bandwidth {b}] [queuemask {qm}] [forcebw]\r\n"
		   "       qosrule discpline CAR {b1 b2 b3 b4 b5 b6} [uplink-bandwidth {b}] [queuemask {qm}]\r\n"
		   "	   qosrule discpline off \r\n"
		   "	   qosrule remark add -i {index} IPVersion {v4|v6|all} queue {queue_num} \r\n"
		   "									[ipp {ippVal} tos {tosVal} dscpORTC dot1p {802dot1pVal}] \r\n"
		   "	   qosrule remark del -i {index} \r\n"
		   "	   qosrule type add -i {index} IPVersion {v4|v6|all} [{type_name} {type_value} ...] \r\n"
		   "       qosrule type del -i {index} \r\n"
		   "       qosrule app add -i {index} IPVersion {v4|v6} {app_name} {dst_ip} {dst_proto} {dst_port} {queue_num} \r\n"
		   "	   qosrule app del -i {index} \r\n"
		   "	   qosrule vlan on|off vpidotvci {vpi.vci} vlanid {vlanid} \r\n"
		   "	   qosrule help \r\n");

	return 0;
}

int subcmd  (
    const cmds_t tab[], int argc, char *argv[], void *p
)
{
    register const cmds_t *cmdp;
    int found = 0;
    int i, len;

    /* Strip off first token and pass rest of line to subcommand */
    if (argc < 2) {
        if (argc < 1)
            printf("SUBCMD - Don't know what to do?\n");
    	else{
            argv++;
            goto print_out_cmds;
    	}
    	return -1;
    }
	argc--;
	argv++;
	for(cmdp = tab;cmdp->name != NULL;cmdp++){
	    if(strncmp(argv[0],cmdp->name,strlen(argv[0])) == 0){
	       found = 1;
	       break;
		}
	}
	if(!found){
        char buf[66];

print_out_cmds:
        printf("valid subcommands of %s:\n", (argv-1)[0]);
        memset(buf,' ',sizeof(buf));
        buf[64] = '\n';
        buf[65] = '\0';
        for(i=0,cmdp = tab;cmdp->name != NULL;cmdp++){
			len = strlen(cmdp->name);
            strncpy(&buf[i*16],cmdp->name,(len > 16) ? 16 : len);
            if(i == 3){
		       printf(buf);
		       memset(buf,' ',sizeof(buf));
		       buf[64] = '\n';
		       buf[65] = '\0';
		    }
		    i = (i+1)%4;
		}
		if(i != 0)
		   printf(buf);
		return -1;
	}
	if(argc <= cmdp->argcmin){
	    if(cmdp->argc_errmsg != NULL)
	       printf("Usage: %s\n",cmdp->argc_errmsg);
	    return -1;
	}
	return (*cmdp->func)(argc,argv,p);
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

static uint32 getDefMaxUplinkBandwidth(void)
{
	/* set def mac uplink bandwidth to 1M=1024kbps */
	return 1024;
}

static int checkCmdAndRule( int argc, char *argv[], int *pRuleIdx, char *ipVer, int maxRuleNum )
{
	int ruleIdx;
	int cnt1 = 0, cnt2 = 0;

	if ( ipVer == NULL || pRuleIdx == NULL ) {
		return -1;
	}

	/* check cmd */
		cnt1 = 4;
		cnt2 = 6;

	if ( (argc < cnt1) || (argc < cnt2 && strcmp(argv[1], "del") != 0) ) 
	{
		return -1;
	}
	if ( strcmp(argv[1], "add") != 0 && strcmp(argv[1], "del") != 0 ) {
		return -1;
	}
	if ( strcmp(argv[2], "-i") != 0 ) {
		return -1;
	}

	if ( !strcmp(argv[1], "add") ) {
		/* check ip version */
		if ( strcmp( argv[4], "IPVersion" ) != 0 ||
			 (strcmp( argv[5], "v4" ) != 0 &&
			  strcmp( argv[5], "v6" ) != 0 &&
			  strcmp( argv[5], "all") != 0) ){
			return -1;
		}
	}
	
	ruleIdx = atoi( argv[3] );
	
	/* check rule index */
	if ( ruleIdx > maxRuleNum - 1 ) {
		return -1;
	}
	*pRuleIdx = ruleIdx;
	strcpy( ipVer, argv[5] );
	
	return 0;
}

int main(int argc, char **argv) 
{
	void *p = NULL;
	int ret = 0;
	int pidfd = 0;
	int res = 0;

	init_compileoption_val();
	if(TCSUPPORT_CT_VAL && TCSUPPORT_C1_CUC_VAL)
		TCSUPPORT_HW_QOS_VAL = 1;
	if(TCSUPPORT_HWNAT_VAL)
		MAX_TYPE_RULE_NUM_VAL = 15;

	pidfd =open( QOSCMD_LOCK_FILE, O_RDWR | O_CREAT );
	if ( pidfd < 0 ) {
		printf( "open lock file error!\n" );
		ret = subcmd( QosCmds, argc, argv, p );
	}
	else {
		res = writew_lock( pidfd, 0, SEEK_SET, 0 );
		ret = subcmd( QosCmds, argc, argv, p );
		res = un_lock( pidfd, 0, SEEK_SET, 0 );
		//close( pidfd );
	}
	
	return ret;
}
