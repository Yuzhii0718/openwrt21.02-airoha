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
***************************************************************


	Module Name:
	qosmgr.c

	Abstract:

	Revision History:
	Who			  When			What
	--------	   ----------		---------------------------------------
	Name		  Date			Modification logs
	zhiwei.Cheng    2018/01/05	       Create
*/


#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "qosmgr.h"
#include "libapi_lib_qosmgr.h"
#include "libapi_lib_utility.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/


/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/
const cmds_t qosmgr_set_command[];

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/
int do_set_discp_PQ(int argc, char *argv[], void *p);
int do_set_discp_WRR(int argc, char *argv[], void *p);
int do_set_discp_CAR(int argc, char *argv[], void *p);

/*********************** qosmgr_root_command **************************/
static int do_set(int argc, char* argv[], void *p);
static int do_set_discpline(int argc, char* argv[], void *p);


/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/
static const cmds_t qosmgr_root_command[] = {
 /*	name,		func, 			flags,	argcmin,	       argc_errmsg */
	{"set",		do_set,			0,		0,			"Set qos status"},
	
	/* Cmd should be added before this line!  */
	{"help",	api_lib_help,	0,		0,			"qosmgr set discpline [...]"},
	{NULL, NULL, 0, 0, NULL}
};


const cmds_t qosmgr_set_command[] = {
/*	name,					func,						flags,	argcmin,		argc_errmsg */
	{"discpline",			do_set_discpline,			0,		0,			"Set qos information"},
	
	/* Cmd should be added before this line!  */
	{"help",				api_lib_help,				0,		0,			"qosmgr set discpline PQ/WRR/CAR [...]"},
	{NULL, NULL, 0, 0, NULL}
};



static const cmds_t qosmgr_discpline_command[] = {
/*	name,		func, 			                flags,	       argcmin,	        argc_errmsg */
	{"PQ",		do_set_discp_PQ,			0,		0,			"qosmgr set discpline PQ (uplink-bandwidth/kbps) [queuemask/default(0xF)]"},	
	{"WRR",		do_set_discp_WRR,			0,		0,			"qosmgr set discpline WRR w4 w3 w2 w1 (uplink-bandwidth/kbps) [queuemask/default(0xF)]"},	
	{"CAR",		do_set_discp_CAR,			0,		0,			"qosmgr set discpline CAR rate1 rate2 rate3 rate4 rate5 rate6 (uplink-bandwidth/kbps) [queuemask/default(0x3F)]"},
	
	/* Cmd should be added before this line!  */
	{"help",	api_lib_help,	            0,		0,			"Show qosmgr usage."},
	{NULL, NULL, 0, 0, NULL}
};


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/******************************************************************************
 Descriptor:	It's used to set qdma driver status.
 Input Args:	see qdmamgr_root_command.
 Ret Value:	none.
******************************************************************************/
static int do_set(int argc, char* argv[], void *p)
{
	return subcmd(qosmgr_set_command, argc, argv, (void *)qosmgr_set_command);
}


static int do_set_discpline(int argc, char* argv[], void *p)
{
	return subcmd(qosmgr_discpline_command, argc, argv, (void *)qosmgr_discpline_command);
}


int do_set_discp_PQ(int argc, char *argv[], void *p){
	char *endPtr = NULL, queueMask = 0xF;
    unsigned int uplinkBandwidth;
    int i = 0;
    
    if(argc<2 || argc > 3){
        goto ErrorHandle;

    }
    
    uplinkBandwidth = (strtoul(argv[1], &endPtr, 0))/8;
    uplinkBandwidth = uplinkBandwidth>0 ? uplinkBandwidth : 1;

    if(argc == 3){
        queueMask =  strtoul(argv[2], &endPtr, 0);
        if(queueMask <= 0 || queueMask > WRR_QUEUE_MASK){
            printf("Exec. Failed: queueMask should be 1~%d\n", WRR_QUEUE_MASK) ;
            return ECNT_QOSMGR_FAILED;
        }
    }
    
    if (qosmgr_lib_del_qos_config_meter_flow(0, 0))
    {
		printf("Exec. Failed: 'qosmgr set discpline PQ (uplink-bandwidth/kbps) [queuemask/default(0xF)]' command failed\n") ;
		return ECNT_QOSMGR_FAILED;
    }
    
	/* set tx rate limit value */
	if(qosmgr_lib_set_qos_config_PQ(queueMask) || qosmgr_lib_set_qos_config_rate(uplinkBandwidth) ) {
		printf("Exec. Failed: 'qosmgr set discpline PQ (uplink-bandwidth/kbps) [queuemask/default(0xF)]' command failed\n") ;
		return ECNT_QOSMGR_FAILED;
	}
    printf("QoS: HW PQ\n");    
    printf("QueueMask: %d\n", queueMask);    
    printf("Ratelimit: %d kbps\n", (uplinkBandwidth*8)); 
	printf("Exec. Successful\n");
    return ECNT_QOSMGR_SUCCESS;
    
ErrorHandle:
    printf( "do_set_discp_PQ(): wrong parameters. \r\n"
            "       usage: \r\n" 
            "       qosmgr set discpline PQ (uplink-bandwidth/kbps) [queuemask/default(0xF)]\r\n") ;
    return ECNT_QOSMGR_FAILED;

}

int do_set_discp_WRR(int argc, char *argv[], void *p){
 	char *endPtr = NULL, queueMask = 0xF;
    unsigned int uplinkBandwidth;
    int i=0, weight[WRR_QUEUE_NUM]={0};
    
    if(argc<6 || argc > 7)
		goto ErrorHandle;

    weight[0] = strtoul(argv[1], &endPtr, 0);
    weight[1] = strtoul(argv[2], &endPtr, 0);
    weight[2] = strtoul(argv[3], &endPtr, 0);
    weight[3] = strtoul(argv[4], &endPtr, 0);

    uplinkBandwidth = (strtoul(argv[5], &endPtr, 0))/8;
    uplinkBandwidth = uplinkBandwidth>0 ? uplinkBandwidth : 1;

    if(argc == 7){
        queueMask =  strtoul(argv[6], &endPtr, 0);
        if(queueMask <= 0 || queueMask > WRR_QUEUE_MASK){
            printf("Exec. Failed: queueMask should be 1~%d\n", WRR_QUEUE_MASK) ;
		    return ECNT_QOSMGR_FAILED;
        }
    }
    
    for ( i = 0; i < WRR_QUEUE_NUM; i++ ) {
    	if (!(queueMask & (1<<i))) {
    		weight[i] = 0;
    		continue;
    	}
    }

    if (qosmgr_lib_del_qos_config_meter_flow(0, 0))
    {
		printf("Exec. Failed: 'qosmgr set discpline WRR w4 w3 w2 w1 (uplink-bandwidth/kbps) [queuemask/default(0xF)]' command failed\n") ;
		return ECNT_QOSMGR_FAILED;
    }
    
	/* set rate limit & WRR */
	if(qosmgr_lib_set_qos_config_WRR(queueMask, weight) || qosmgr_lib_set_qos_config_rate(uplinkBandwidth)) {
		printf("Exec. Failed: 'qosmgr set discpline WRR w4 w3 w2 w1 (uplink-bandwidth/kbps) [queuemask/default(0xF)]' command failed\n") ;
		return ECNT_QOSMGR_FAILED;
	}
    
    printf("QoS: HW WRR\n");     
    printf("QueueMask: %d\n", queueMask);    
    printf("qos_wrr_info: %d %d %d %d\n", weight[0], weight[1], weight[2], weight[3]);      
    printf("Ratelimit: %d kbps\n", (uplinkBandwidth*8));    
	printf("Exec. Successful\n");
    return ECNT_QOSMGR_SUCCESS;
    
ErrorHandle:
    printf( "do_set_discp_WRR(): wrong parameters. \r\n"
            "       usage: \r\n" 
            "       qosmgr set discpline WRR w4 w3 w2 w1 (uplink-bandwidth/kbps) [queuemask/default(0xF)]\r\n") ;
    return ECNT_QOSMGR_FAILED;   

}



int do_set_discp_CAR(int argc, char *argv[], void *p){
 	char *endPtr = NULL, queueMask = 0x3F;
    unsigned int uplinkBandwidth;
    int i=0, isWeightSet=0, rate[CAR_QUEUE_NUM]={0};
    
    if(argc<8 || argc > 9)
		goto ErrorHandle;

    rate[0] = strtoul(argv[1], &endPtr, 0);
    rate[1] = strtoul(argv[2], &endPtr, 0);
    rate[2] = strtoul(argv[3], &endPtr, 0);
    rate[3] = strtoul(argv[4], &endPtr, 0);
    rate[4] = strtoul(argv[5], &endPtr, 0);
    rate[5] = strtoul(argv[6], &endPtr, 0);

    uplinkBandwidth = (strtoul(argv[7], &endPtr, 0))/8;
    uplinkBandwidth = uplinkBandwidth>0 ? uplinkBandwidth : 1;
    
    if(argc == 9){
        queueMask =  strtoul(argv[8], &endPtr, 0);
        if(queueMask <= 0 || queueMask > CAR_QUEUE_MASK){
            printf("Exec. Failed: queueMask should be 1~%d\n", CAR_QUEUE_MASK) ;
		    return ECNT_QOSMGR_FAILED;
        }
    }

    if (qosmgr_lib_del_qos_config_meter_flow(0, 0))
    {
		printf("Exec. Failed: 'qosmgr set discpline CAR rate1 rate2 rate3 rate4 rate5 rate6 (uplink-bandwidth/kbps) [queuemask/default(0x3F)]' command failed\n") ;
		return ECNT_QOSMGR_FAILED;
    }
    
	/* set rate limit & WRR */
	if(qosmgr_lib_set_qos_config_CAR(queueMask, rate) || qosmgr_lib_set_qos_config_rate(uplinkBandwidth)) {
		printf("Exec. Failed: 'qosmgr set discpline CAR rate1 rate2 rate3 rate4 rate5 rate6 (uplink-bandwidth/kbps) [queuemask/default(0x3F)]' command failed\n") ;
		return ECNT_QOSMGR_FAILED;
	}

    for (i=0; i< CAR_QUEUE_NUM; i++)
    {        
        if (qosmgr_lib_add_qos_config_meter_flow(0, 0, i+1, i+1))
        {
            printf("Exec. Failed: 'qosmgr_demo set discpline CAR rate1 rate2 rate3 rate4 rate5 rate6 (uplink-bandwidth/kbps) [queuemask/default(0x3F)]' command failed\n") ;
            return ECNT_QOSMGR_FAILED;
        }
    }

    printf("QoS: HW CAR\n"); 
    printf("QueueMask: %d\n", queueMask);    
    printf("CAR_Info: %dkbps %dkbps %dkbps %dkbps %dkbps %dkbps\n", rate[0], rate[1], rate[2], rate[3], rate[4],rate[5]);      
    printf("Ratelimit: %d kbps\n", (uplinkBandwidth*8)); 
	printf("Exec. Successful\n") ;
    return ECNT_QOSMGR_SUCCESS;
    
ErrorHandle:
    printf( "do_set_discp_CAR(): wrong parameters. \r\n"
            "       usage: \r\n" 
            "       qosmgr set discpline CAR rate1 rate2 rate3 rate4 rate5 rate6 (uplink-bandwidth/kbps) [queuemask/default(0x3F)]\r\n") ;
    return ECNT_QOSMGR_FAILED;   

}


/******************************************************************************
 Descriptor:	It's used to config qosmgr.
 Input Args:	see qosmgr_root_command
 Ret Value: Success: ECNT_QOSMGR_SUCCESS
                      Fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int main (int argc, char* argv[])
{
	return subcmd(qosmgr_root_command, argc, argv, (void *)qosmgr_root_command);
}
