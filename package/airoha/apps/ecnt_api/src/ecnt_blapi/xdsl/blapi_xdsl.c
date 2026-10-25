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
* @file  blapi_xdsl.c  
* @brief This file gives examples on using apis from blapi_xdsl.h.
* @author Lei.Zhang
* @date     2019-12-3
* @version  A001 
* @copyright EcoNet Inc                                                              
*/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "blapi_xdsl.h"
#include <ecnt_lib_dbg.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

#if defined(TCSUPPORT_SISM)
#include "lib_pt.h"
#endif /*defined(TCSUPPORT_SISM)*/

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define MAX_BUF_SIZE				2000
#define MAX_ADSL_STATUS_BUF_SIZE	4096

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
/*
*If you want to print the debug infomation, you need to create the following files:
*echo [num1] [num2] > /tmp/blapi_dbg_level_enable
*[num1] is debug level. (8=ECNT_DBG_DEBUG, 4=ECNT_DBG_NOTICE, 2=ECNT_DBG_WARN, 1=ECNT_DBG_ERROR)
*[num2] is enable mask. (8=ECNT_BLAPI_XDSL_DBUG_ENABLE, 4=ECNT_BLAPI_TRAFFIC_DBUG_ENABLE, 2=ECNT_BLAPI_SYSTEM_DBUG_ENABLE, 1=ECNT_BLAPI_PERFORM_DBUG_ENABLE)
*For example : echo 8 4 > /tmp/blapi_dbg_level_enable ,means open the debug info in blapi traffic and the debug level is 8. 
*If you want to open the debug info both in blapi traffic and xdsl ,you need set the corresponding bit to  "1", so [num2] is 12 ,binary is 1100.
*/
#define LIB_BLAPI_ERROR(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_ERROR, ECNT_BLAPI_XDSL_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_WARN(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_WARN, ECNT_BLAPI_XDSL_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_NOTICE(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_NOTICE, ECNT_BLAPI_XDSL_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_DEBUG(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_DEBUG, ECNT_BLAPI_XDSL_DBUG_ENABLE, fmt,##__VA_ARGS__)

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

int blapi_xdsl_reload_ko(int state)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;

	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_RELOAD_KO);
	blapi_h->argc = htonl(1);

	blapi_h->argv1.i = htonl(state);

	if(state == 2)
	{
		/* ATM->PTM */
		system("echo 'xtmline_enable= 1' > /tmp/etc/xtmline_enable");
		system("echo 'dslline_enable= 0' > /tmp/etc/dslline_enable");
	}
	else if(state == 1)
	{
		/* PTM->ATM */
		system("echo 'dslline_enable= 1' > /tmp/etc/dslline_enable");
		system("echo 'xtmline_enable= 0' > /tmp/etc/xtmline_enable");
	}

	if (!SRC_blapi(blapi_h))
	{
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	LIB_BLAPI_DEBUG("enter==============>\n");
	if(state == 2)
	{	
		/*ATM->PTM */
		/*printf("\n[%s:%d]ATM TO PTM=======>\n",__FUNCTION__,__LINE__); */
		LIB_BLAPI_DEBUG("ATM to PTM==============>\n");
		system("rmmod mt7510sar.ko");
#ifdef TCSUPPORT_CPU_EN7512
		system("rmmod qdma_wan.ko");
		system("insmod /lib/modules/qdma_wan.ko macType=2");
#ifdef TCSUPPORT_CPU_EN7516
		system("/userfs/bin/qdmamgr_wan set rxratelimit config Enable packet");
		system("/userfs/bin/qdmamgr_wan set rxratelimit value 0 6000");
		system("/userfs/bin/qdmamgr_wan set rxratelimit value 1 1000000");
#endif
#else
		system("rmmod qdma.ko");
		system("sys memwl bfb00070 2");
		system("insmod /lib/modules/qdma.ko");
		system("sys memwl bfb5152c ffffffff");
#endif
		system("insmod /lib/modules/mt7510ptm.ko");
		system("ifconfig ptm0 up");
	}
	else
	{  
		/*PTM->ATM */
		LIB_BLAPI_DEBUG("PTM to ATM==============>\n");
		system("rmmod mt7510ptm.ko");
#ifdef TCSUPPORT_CPU_EN7512
		system("rmmod qdma_wan.ko");
		system("insmod /lib/modules/qdma_wan.ko macType=3");
#else
		system("rmmod qdma.ko");
		system("sys memwl bfb00070 3");
		system("insmod /lib/modules/qdma.ko");
		system("sys memwl bfb5152c ffffffff");
#endif
		system("insmod /lib/modules/mt7510sar.ko");
	}

	LIB_BLAPI_DEBUG("leave,return 0==============>\n");
	return 0;
#endif
}

int blapi_xdsl_get_cwmpdiag_file(char *fileName, const unsigned int size) {
	strncpy(fileName, "/proc/tc3162/adsl_cwmp_diagnostic", size);
	return 0;
}

int blapi_xdsl_get_xdsl_linkstatus(char *state, int *type) {
	int fd = -1, is_read_file_ok = -1;;
	char string[XDSL_STR_BUFF_SIZE] = {0};
	
	/* 2 : vdsl ; 1:ADSL; 0:down ;	-1: fail */

	fd = open("/proc/tc3162/adsl_stats",O_RDONLY|O_NONBLOCK);
	if( -1 == fd )
	{
		return -1;
	}
	memset(string, 0, sizeof(string));
	is_read_file_ok = read(fd, string, sizeof(string) - 1);
	if( is_read_file_ok < sizeof(string) )
	{
		string[is_read_file_ok] = '\0';
	}
	else
	{
		string[is_read_file_ok - 1] = '\0';
	}
	close(fd);
	if( is_read_file_ok <= 0 )
	{
		return -1;
	}	   
	strncpy(state, string, XDSL_STR_BUFF_SIZE);
	if(strstr(string, "up"))
	{
		if(strstr(string, "PTM"))
		{
			*type = 2; /*vdsl*/
#if defined(TCSUPPORT_CT_SWQOS)
			system("echo 1 > /proc/tc3162/qos_adsl_type");
#endif
			return 0;
		}
		else if(strstr(string, "ATM"))
		{
			*type = 1; /*adsl */
#if defined(TCSUPPORT_CT_SWQOS)
			system("echo 0 > /proc/tc3162/qos_adsl_type");
#endif
			return 0;
		}
		else
		{
			*type = 1; /* means this dmt is not 3095	*/
#if defined(TCSUPPORT_CT_SWQOS)	 
			system("echo 0 > /proc/tc3162/qos_adsl_type");
#endif
			return 0;
		}
	}
	else
	{
		return -1;	
	}
}

int blapi_xdsl_get_fwver(char *fwver, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_GET_FWVER);
	blapi_h->argc = htonl(2);

	fwver[size - 1] = '\0';
	strcpy(blapi_h->argv1.ch, fwver);
	blapi_h->argv2.i = htonl(size);

	if (!SRC_blapi(blapi_h))
	{
		strcpy(fwver, blapi_h->argv1.ch);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	int  fd=0, ret=0;

	fd = open("/proc/tc3162/adsl_fwver",O_RDONLY);
	if(fd == -1){
		return -1;
	}

	ret = read(fd, fwver, size);
	if(ret <= 0){
		close(fd);
		fwver[0] = '\0';
		return -1;
	}
		
	if( ret < size )
	{
		fwver[ret] = '\0';
	}
	else
	{
		fwver[ret - 1] = '\0';
	}
	
	if('\n' == fwver[strlen(fwver) - 1])
	{
		fwver[strlen(fwver)-1] = '\0';
	}
	close(fd);
	return 0;
#endif
}

int blapi_xdsl_get_tsarm(char *tsarm, const int size){
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_GET_TSARM);
	blapi_h->argc = htonl(2);

	tsarm[size - 1] = '\0';
	strcpy(blapi_h->argv1.ch, tsarm);
	blapi_h->argv2.i = htonl(size);

	if (!SRC_blapi(blapi_h))
	{
		strcpy(tsarm, blapi_h->argv1.ch);
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	int  fd=0, ret=0;
	
	fd = open("/proc/tc3162/tsarm_stats",O_RDONLY);
	if(fd == -1){
		return -1;
	}

	ret = read(fd, tsarm, size);
	if(ret <= 0){
		close(fd);
		tsarm[0] = '\0';
		return -1;
	}

	if( ret < size )
	{
		tsarm[ret] = '\0';
	}
	else
	{
		tsarm[ret - 1] = '\0';
	}
	close(fd);
	return 0;
#endif
} 
int blapi_xdsl_ptm_do_reset_sequence(const int adsl_status, const int gdsl_status) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_PTM_DO_RESET_SEQUENCE);
	blapi_h->argc = htonl(2);

	blapi_h->argv1.i = htonl(adsl_status);
	blapi_h->argv2.i = htonl(gdsl_status);

	if (!SRC_blapi(blapi_h))
	{
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	if (1 == adsl_status && 2 == gdsl_status) { 
		system("echo 1 > /proc/tc3162/ptm_do_reset_sequence");
		return 0;
	} else if (0 == adsl_status && 0 != gdsl_status) {
		system("echo 0 > /proc/tc3162/ptm_do_reset_sequence");
		return 0;
	}
	return -1;
#endif
}
int blapi_xdsl_get_interface_config_file(char *fileName, const unsigned int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h = NULL;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_GET_INTERFACE_CONFIG_FILE);
	blapi_h->argc = htonl(2);

	/*Ignore blapi_h->argv1*/
	blapi_h->argv2.i = htonl(size);

	if (!SRC_blapi(blapi_h))
	{
		memcpy(fileName, blapi_h->argv1.ch, strlen(blapi_h->argv1.ch));
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
	strncpy(fileName, "/proc/tc3162/vdsl_interface_config", size);
	return 0;
#endif
}

int blapi_xdsl_set_key_val_to_attr(const char *keyname, char *attr, const int attr_size){
	FILE *fp;
	char *str_key= NULL;
	char stream[128]={0};
	int enterOffSet=1;
	int qmarkLength=0;
	int skipQmark=0;
	int totalLength=0;

	fp=fopen("/proc/tc3162/adsl_stats","r");
	if ( fp == NULL )
		return -1;

	memset(attr, 0, attr_size);
	fseek(fp, 0, SEEK_SET);
	qmarkLength=0;
	skipQmark=0;
	while ( fgets(stream, sizeof(stream)-1, fp) != NULL ){
		if ( strrchr(stream,'\n') == NULL ) 
			enterOffSet=0;
		else
			enterOffSet=1;

		str_key = strstr(stream,keyname);
		if ( str_key == NULL || str_key != stream ) {
			memset(stream, 0, sizeof(stream));
			continue;
		}

		totalLength=strlen(stream)-strlen(keyname)-enterOffSet-qmarkLength;
		if ( attr_size < totalLength + 1 ) {
			fclose(fp);
			return -1;
		} else if ( totalLength < 0 ) {
			fclose(fp);
			return -1;
		} else {
			strncpy(attr, stream+strlen(keyname)+skipQmark, totalLength);
			attr[totalLength]= '\0';
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return -1;
}

void send2atmcmdd(atmcmd_msg_t *msg)
{
	int sockfd=0, len=0;
	struct sockaddr_un remote;

	msg->retval = ATMCMD_FAIL;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		LIB_BLAPI_DEBUG("[%s]%d\n", __FUNCTION__, __LINE__);
		perror("socket");
		return;
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, ATMCMD_SOCK_PATH);
	LIB_BLAPI_DEBUG("[%s]%d: remote.sun_path = %s\n", __FUNCTION__, __LINE__, remote.sun_path);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
		LIB_BLAPI_DEBUG("[%s]%d\n", __FUNCTION__, __LINE__);
		perror("connect");
		close(sockfd);
		return;
	}

	if (send(sockfd, msg, sizeof(atmcmd_msg_t), 0) == -1){
		LIB_BLAPI_DEBUG("[%s]%d\n", __FUNCTION__, __LINE__);
		perror("send");
		close(sockfd);
		return;
	}

	memset(msg, 0, sizeof(atmcmd_msg_t));

	if ((recv(sockfd, msg, sizeof(atmcmd_msg_t), 0)) <= 0 ) {
		LIB_BLAPI_DEBUG("[%s]%d\n", __FUNCTION__, __LINE__);
		/*Failure to receive information from cfg manage*/
		perror("recv");
		close(sockfd);
		return;
	}
	close(sockfd);
}

int atmcmdc_create_pvc(int itfnum, int vpi, int vci, int encap, int payload,
		int qos_type, int pcr, int scr, int mbs, int sndbuf)
{
	atmcmd_msg_t msg;

	memset(&msg, 0, sizeof(msg));

	/*Fill information*/
	msg.op_cmd = ATMCMD_CREATE_PVC;
	msg.itfnum = itfnum;
	msg.vpi = vpi;
	msg.vci = vci;
	msg.encap = encap;
	msg.payload = payload;
	msg.qos_type = qos_type;
	msg.pcr = pcr;
	msg.scr = scr;
	msg.mbs = mbs;
	msg.sndbuf = sndbuf;

	send2atmcmdd(&msg);
	return msg.retval;
}

int blapi_xdsl_client_command(char *cmd)
{
#if defined(TCSUPPORT_SISM_HOST)
	int count = 1, ret = 0;
	blapi_hdr_t *blapi_h;
	
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_SRC_CMD);
	blapi_h->argc = htonl(1);

	memcpy(&blapi_h->argv1,cmd, strlen(cmd));

	while (SRC_blapi(blapi_h))
	{
		/* At most send 3 times. */
		if (count++ >= 3)
		{
			free(blapi_h);

			return -1;
		}
	}

	memcpy(cmd, &blapi_h->argv1, strlen(blapi_h->argv1.ch));
	ret = blapi_h->ret;

	free(blapi_h);
	return ret;
#else
	return -1;
#endif
}

static int get_env_intf_mode_state()
{
#if (defined(TCSUPPORT_OPENWRT) || defined(RDKB_BUILD))
	/* Always disable mpoa header in ATM packet. */
	return 0;
#elif defined(TCSUPPORT_SISM_HOST)
	char *str = NULL;
	int enable = 1;

	str = fw_getenv("intf_mode");

	if (str && strcmp(str,"0")==0)
	{
		enable = 0;
	}

	return enable;
#else
	return -1;
#endif
}

void set_7562_sar_driver (int start_ATM)
{
#if defined(TCSUPPORT_SISM_HOST)
	static int upflag_ATM = 0;
	char cmd[256] = {0};

	if (start_ATM)
	{
		if (upflag_ATM == 0)
		{
			upflag_ATM = 1;

#if (defined(TCSUPPORT_OPENWRT) || defined(RDKB_BUILD))
#if defined(TCSUPPORT_OPENWRT)
			/* AN7551 use ae_wan to connect EN7517. */
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"insmod /lib/modules/5.4.55/extra/en7517_host_sar.ko ethwan=\"ae_wan\"");
			system(cmd);
#else		/* defined(RDKB_BUILD) */
			/* AN7551 use nas10 to connect EN7517. */
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"insmod /lib/modules/5.4.55/extra/en7517_host_sar.ko ethwan=\"nas10\"");
			system(cmd);
#endif
			/* Set extend TPID for ATM packet flow */
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"sys memwl 1FB50000 884C182A");
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"sys memwl 1FB50F18 884C");
			system(cmd);

			/* Set vlan mode to EN7517 for ATM */
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"echo %d > /proc/tc3162/single_vlan_mode", get_env_intf_mode_state());
			LIB_BLAPI_DEBUG("[%s]%d: client_cmd = %s\n", __FUNCTION__, __LINE__, cmd);
			if (blapi_xdsl_client_command(cmd) == -1)
			{
				LIB_BLAPI_ERROR("Client execute cmd %s fail\n", cmd);
			}
#else /* Use smuxctl to create interface */
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"/usr/bin/smuxctl add bridge ae_wan atm 2 0 0 0");
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"insmod /lib/modules/en7517_host_sar.ko ethwan=\"atm\"");
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"ifconfig atm up");
			system(cmd);

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"echo %d > /proc/tc3162/adsl_mpoa_header", get_env_intf_mode_state());
			system(cmd);
#endif

			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd,sizeof(cmd),"echo %d > /proc/tc3162/tsarm_mpoa_header", get_env_intf_mode_state());
			system(cmd);
		}
	}
	else if (upflag_ATM == 1)
	{
		upflag_ATM = 0;

#if (defined(TCSUPPORT_OPENWRT) || defined(RDKB_BUILD))
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"rmmod en7517_host_sar");
		system(cmd);

		/* Set extend TPID for ATM packet flow */
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"sys memwl 1FB50000 8100182A");
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"sys memwl 1FB50F18 8100");
		system(cmd);
#else /* Use smuxctl to create interface */
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"ifconfig atm down");
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"rmmod en7517_host_sar");
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd,sizeof(cmd),"/usr/bin/smuxctl rem atm");
		system(cmd);
#endif
	}

	return;
#else
	return;
#endif
}

int blapi_xdsl_atm_create_interface(blapi_atm_interface_obj_t * obj, int * pvcactive_flag)
{
	char pcr_v[32] = {0};
	char encap_t[8] = {0};
	char cmd[128] = {0};
#if defined(TCSUPPORT_SISM_HOST)
	char client_cmd[128] = {0};
	unsigned char pppoa_support = 0;
#endif
	int encap = -1;
	int payload = -1;
	int qos_type = -1;
	int pcr = 0;
	int scr = 0;
	int mbs = 0;
	
	LIB_BLAPI_DEBUG("[%s]%d\n", __FUNCTION__, __LINE__);
	if( 0 == strcmp(obj->transMode, "ATM") )
	{
		if( 0 == obj->str_qos[0] )
		{
			LIB_BLAPI_DEBUG("[%s]%d:fail get QOS\n" , __FUNCTION__, __LINE__);
			return -1;
		}
		if (0 == obj->str_encap[0] )
		{
			LIB_BLAPI_DEBUG("[%s]%d:fail get ENCAP\n" , __FUNCTION__, __LINE__);
			return -1;
		}

		if(0 == obj->str_qos[0] || 0 == obj->str_encap[0])
		{
			LIB_BLAPI_DEBUG("[%s]%d:fail get PVC attribute\n" , __FUNCTION__, __LINE__);
			return -1;
		}

		if(obj->vpi == 0 && obj->vci == 0)
		{
			LIB_BLAPI_DEBUG("[%s]%d:wrong VPI,VCI(%d, %d)\n" , __FUNCTION__, __LINE__, obj->vpi, obj->vci);
			return -1;
		}

		if(!strcmp(obj->str_encap, "1483 Bridged IP LLC") || !strcmp(obj->str_encap, "PPPoE LLC"))
		{
			encap = 0;
			payload = 1;
		}
		else if(!strcmp(obj->str_encap, "1483 Bridged IP VC-Mux") || !strcmp(obj->str_encap, "PPPoE VC-Mux"))
		{
			encap = 1;
			payload = 1;
		}
#if defined(TCSUPPORT_SISM_HOST)
		else if( !strcmp(obj->str_encap, "1483 Routed IP LLC(IPoA)") )
		{
			encap = 2;
			payload = 1;
			LIB_BLAPI_DEBUG("[%s]%d:ipoa:%s\n", __FUNCTION__, __LINE__, obj->str_encap);
		}
		else if( !strcmp(obj->str_encap, "1483 Routed IP VC-Mux") )
		{
			encap = 3;
			payload = 1;
			LIB_BLAPI_DEBUG("[%s]%d:ipoa:%s\n", __FUNCTION__, __LINE__, obj->str_encap);
		}
		else if(!strcmp(obj->str_encap, "PPPoA VC-Mux") || !strcmp(obj->str_encap, "PPPoA LLC"))
		{
			pppoa_support = 1;
			LIB_BLAPI_DEBUG("[%s]%d:support %s\n", __FUNCTION__, __LINE__, obj->str_encap);
		}
#endif
		else
		{
			LIB_BLAPI_DEBUG("[%s]%d:not support encap:%s\n", __FUNCTION__, __LINE__, obj->str_encap);
			return -1;
		}

		if(!strcmp(obj->str_qos, "ubr"))
		{
			qos_type = 1;
		}
		else if(!strcmp(obj->str_qos, "ubr+"))
		{
			qos_type = 1;
			pcr = atoi(obj->str_pcr);
		}
		else if(!strcmp(obj->str_qos, "cbr"))
		{
			qos_type = 2;
			pcr = atoi(obj->str_pcr);
		}
		else if(!strcmp(obj->str_qos, "rt-vbr"))
		{
			qos_type = 3;
			pcr = atoi(obj->str_pcr);
			scr = atoi(obj->str_scr);
			mbs = atoi(obj->str_mbs);
		}
		else if(!strcmp(obj->str_qos, "nrt-vbr"))
		{
			qos_type = 6;
			pcr = atoi(obj->str_pcr);
			scr = atoi(obj->str_scr);
			mbs = atoi(obj->str_mbs);
		}
		else
		{
			LIB_BLAPI_DEBUG("[%s]%d:not support qos:%s\n", __FUNCTION__, __LINE__, obj->str_qos);
			return -1;
		}
#if defined(TCSUPPORT_SISM_HOST)
		if( pppoa_support != 1 )
			atmcmdc_create_pvc((obj->idx / 8), obj->vpi, obj->vci, encap, payload, qos_type, pcr, scr, mbs, 8192);
#else
		atmcmdc_create_pvc((obj->idx / 8), obj->vpi, obj->vci, encap, payload, qos_type, pcr, scr, mbs, 8192);
#endif
		*pvcactive_flag = 1; 
		
		if( 0 == obj->isp[0] )
		{
			LIB_BLAPI_DEBUG("[%s]%d:fail get QOS\n" , __FUNCTION__, __LINE__);
			return -1;
		}
		
		if((0 == atoi(obj->isp)) || (1 == atoi(obj->isp)))
		{
			if(!strcmp(obj->str_qos, "ubr"))
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d", pcr);
			}
			else
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d -q %d -m %d", pcr, scr, mbs);
			}

			if(!strcmp(obj->str_encap, "1483 Bridged IP LLC"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 0");
			}
			else if(!strcmp(obj->str_encap, "1483 Bridged IP VC-Mux"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 1");
			}
			else if(!strcmp(obj->str_encap, "1483 Routed IP LLC(IPoA)"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 2");
			}
			else
			{
				snprintf(encap_t, sizeof(encap_t), "-e 3");
			}

		}
		else if(2 == atoi(obj->isp))
		{
			if(!strcmp(obj->str_qos, "ubr"))
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d", pcr);
			}
			else
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d -q %d -m %d", pcr, scr, mbs);
			}
			
			if(!strcmp(obj->str_encap, "PPPoE LLC"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 0");
			}
			else
			{
				snprintf(encap_t, sizeof(encap_t), "-e 1");
			}
		}
		else
		{
			if(!strcmp(obj->str_qos, "ubr"))
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d", pcr);
			}
			else
			{
				snprintf(pcr_v, sizeof(pcr_v), "-p %d -q %d -m %d", pcr, scr, mbs);
			}
			
			if(!strcmp(obj->str_encap, "1483 Bridged IP LLC"))
			{
				snprintf(encap_t, sizeof(encap_t), "-e 0");
			}
			else
			{
				snprintf(encap_t, sizeof(encap_t), "-e 1");
			}
		}

#if defined(TCSUPPORT_SISM_HOST)
		set_7562_sar_driver (1);

		memset(client_cmd, 0, sizeof(client_cmd));
		snprintf(client_cmd, sizeof(client_cmd), "br2684ctl -c %d %s -t %s %s -a 0.%d.%d -b", 
			(obj->idx / 8), encap_t, obj->str_qos, pcr_v, obj->vpi, obj->vci);
		LIB_BLAPI_DEBUG("[%s]%d: client_cmd = %s\n", __FUNCTION__, __LINE__, client_cmd);
		if (blapi_xdsl_client_command(client_cmd) == -1)
		{
			LIB_BLAPI_ERROR("Client execute cmd %s fail\n", client_cmd);
		}

		if((get_env_intf_mode_state() == 0) && (pppoa_support == 1))
		{
			int encap_type = 0;

			if(!strcmp(obj->str_encap, "PPPoA LLC"))
			{
				encap_type = 1;
			}

			memset(client_cmd, 0, sizeof(client_cmd));
			snprintf(client_cmd, sizeof(client_cmd), "echo \"%d %d %d\" > /proc/tc3162/atm_pppoa_setting",
				obj->vpi, obj->vci, encap_type);
			LIB_BLAPI_DEBUG("[%s]%d: client_cmd = %s\n", __FUNCTION__, __LINE__, client_cmd);
			if (blapi_xdsl_client_command(client_cmd) == -1)
			{
				LIB_BLAPI_ERROR("Client execute cmd %s fail\n", client_cmd);
			}
		}

		if( pppoa_support != 1)
		{
			if(strcmp(obj->str_qos, "ubr") != 0)
			{
				/* 7562 cannot set pcr be 0 when QoS mode is not ubr mode. Set pcr be 5500 whcih is max pcr of Web setting. */
				memset(pcr_v, 0, sizeof(pcr_v));
				snprintf(pcr_v, sizeof(pcr_v), "-p 5500 -q %d -m %d", scr, mbs);
			}
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "br2684ctl -c %d %s -t %s %s -a 0.%d.%d -b", 
				(obj->idx / 8), encap_t, obj->str_qos, pcr_v, obj->vpi, obj->vci);
			LIB_BLAPI_DEBUG("[%s]%d: cmd = %s\n", __FUNCTION__, __LINE__, cmd);
			system(cmd);
		}
#else
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "br2684ctl -c %d %s -t %s %s -a 0.%d.%d &", 
			(obj->idx / 8), encap_t, obj->str_qos, pcr_v, obj->vpi, obj->vci);
		LIB_BLAPI_DEBUG("[%s]%d: cmd = %s\n", __FUNCTION__, __LINE__, cmd);
		system(cmd);
#endif
	}
	return 0;

}

int blapi_xdsl_atm_delete_interface(int ifnum)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = -1;
    blapi_hdr_t *blapi_h;
    blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
    blapi_h->blapi_id = htonl(BLAPI_XDSL_ATM_DELETE_INTERFACE);
    blapi_h->argc = htonl(1);
    blapi_h->argv1.i = htonl(ifnum);

    if (!SRC_blapi(blapi_h))
    {
        ret = blapi_h->ret;
    }

    free(blapi_h);
    return ret;
#else
    return -1;
#endif
}

int blapi_xdsl_create_device(blapi_atm_device_obj_t * obj)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_CREATE_DEVICE);
	blapi_h->argc = htonl(1);

	memcpy(&blapi_h->argv1.atm_d, obj, sizeof(blapi_atm_device_obj_t));

	if (!SRC_blapi(blapi_h))
	{
		memcpy(obj, &blapi_h->argv1.atm_d, sizeof(blapi_atm_device_obj_t));
		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret;
#else
    char cmd[256] = {0};
	int dev_size = sizeof(obj->dev);

	LIB_BLAPI_DEBUG("%s %d \n", __FUNCTION__, __LINE__);

	if( 0 == strcmp(obj->transMode, "PTM"))
	{
		snprintf(obj->dev, dev_size,"ptm0");
	}
	else
	{
		snprintf(obj->dev, dev_size, "nas%d", (obj->idx / 8));
		if( 0 == strlen(obj->mac) )
			return -1;
				
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ifconfig %s hw ether %s", obj->dev, obj->mac);
		system(cmd);
	
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "ifconfig %s up", obj->dev);
		system(cmd);
	}

	return 0;
#endif
}

int blapi_xdsl_set_all_key_val_to_attr(blapi_adsl_info_t *adsl_info){
#if defined(TCSUPPORT_SISM_HOST)
	FILE *fp;
	char *str_key= NULL;
	char stream[128]={0};
	int enterOffSet=1;
	int qmarkLength=0;
	int skipQmark=0;
	int totalLength=0;
	int keywd_idx = 0;

	char adslState_keywd[][40]=
	{
		{"ADSL link status: "},{"Opmode: "},
		{"near-end interleaved channel bit rate: "},{"near-end fast channel bit rate: "},
		{"far-end interleaved channel bit rate: "},{"far-end fast channel bit rate: "},
		{"near-end FEC error fast: "},{"near-end FEC error interleaved: "},
		{"near-end CRC error fast: "},{"near-end CRC error interleaved: "},
		{"near-end HEC error fast: "},{"near-end HEC error interleaved: "},
		{"far-end FEC error fast: "},{"far-end FEC error interleaved: "},
		{"far-end CRC error fast: "},{"far-end CRC error interleaved: "},
		{"far-end HEC error fast: "},{"far-end HEC error interleaved: "},
		{"ADSL uptime :"},{"ADSL activetime :"},
		{"noise margin downstream: "},{"output power upstream: "},
		{"attenuation downstream: "},
		{"noise margin upstream: "},{"output power downstream: "},
		{"attenuation upstream: "},
		{"near end itu identification: "},{"far end itu identification: "},
		{"attain upstream: "},{"attain downstream: "},
#ifdef CWMP 		
		{"ADSL activetime second: "},{"ADSL total ativetime second: "},
		{"ATURANSIRev: "},{"ATUCANSIRev: "},
		{"ATURANSIStd: "},{"ATUCANSIStd: "},
		{"Interleave Depth: "},
#endif
		{"Interleave Depth Down:"},
#if defined(TCSUPPORT_CT_ADSLSETTING)
		{"Adsl Standard: "},
		{"Adsl Type: "},
#endif
		{""}
	};

	fp=fopen("/proc/tc3162/adsl_stats","r");
	if ( fp == NULL )
		return -1;

	fseek(fp, 0, SEEK_SET);
	qmarkLength=0;
	skipQmark=0;
	while ( fgets(stream, sizeof(stream)-1, fp) != NULL ){
		if ( strrchr(stream,'\n') == NULL ) 
			enterOffSet=0;
		else
			enterOffSet=1;

		str_key = strstr(stream,adslState_keywd[keywd_idx]);
		if ( str_key == NULL || str_key != stream ) {
			memset(stream, 0, sizeof(stream));
			continue;
		}

		totalLength=strlen(stream)-strlen(adslState_keywd[keywd_idx])-enterOffSet-qmarkLength;
		if ( sizeof(adsl_info->attr[keywd_idx]) < totalLength + 1 ) {
			fclose(fp);
			return -1;
		} else if ( totalLength < 0 ) {
			fclose(fp);
			return -1;
		} else {
			memset(adsl_info->attr[keywd_idx], 0, sizeof(adsl_info->attr[keywd_idx]));
			strncpy(adsl_info->attr[keywd_idx], stream+strlen(adslState_keywd[keywd_idx])+skipQmark, totalLength);
			adsl_info->attr[keywd_idx][totalLength]= '\0';
			keywd_idx++;

		}
	}
	fclose(fp);
	return 0;
#else
	return -1;
#endif
}

int get_proc_data(char *proc_path,char* buf,int read_size){
#if defined(TCSUPPORT_SISM_HOST)
	int  fd=0, ret=0;

	fd = open(proc_path,O_RDONLY);
	if(fd == -1){
		printf("open %s failed\n",proc_path);
		return -1;
	}
	ret = read(fd, buf, read_size);
	if(ret < 0){
		close(fd);
		buf[0] = '\0';
		return -1;
	}
	buf[read_size - 1] = '\0';
	close(fd);
	return 0;
#else
	return -1;
#endif
}

/*
*	if client will return over 160,
*	please add proc at client_info(virtual driver).
*	Cause the size blapi of ch in blapi_hdr_t is only 160.
*/
int send_to_blapi_interface(int id, char* buf, int size){
#if defined(TCSUPPORT_SISM_HOST)
	blapi_hdr_t blapi_h;
	int ret = -1;
	
	memset(&blapi_h, 0, sizeof(blapi_hdr_t));
	blapi_h.blapi_id = htonl(id);
	blapi_h.argc = htonl(2);
	memcpy(&(blapi_h.argv2), buf, strlen(buf));
	blapi_h.argv2.i = htonl(size);

	if (!SRC_blapi(&blapi_h)){
		memcpy(buf, &(blapi_h.argv1), strlen(blapi_h.argv1.ch));
		ret = blapi_h.ret;
	} 
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_mode(char *mode, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_MODE,mode,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_annex(char *annex, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_ANNEX,annex,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_vdsl_interface_config(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/vdsl_interface_config",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_rx_bear_tpstc_type(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_RX_BEAR_TPSTC_TYPE,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_attain_rate(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_ATTAIN_RATE,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_ch_op_data(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/adsl_ch_op_data",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_far_op_data(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/adsl_far_op_data",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_near_op_data(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_NEAR_OP_DATA,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_pms_param(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/adsl_pms_param",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_pms_inp(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/adsl_pms_inp",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_oam_ping_status(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/oam_ping_status",context,size);
	return ret;
#else
	return -1;
#endif
}


int blapi_xdsl_get_adsl_wlan_dsl_inf_conf(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/adsl_tr69_wlan_dsl_interface_config",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_defect_cnt(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/adsl_defect_cnt",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_cell_cnt1(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = get_proc_data("/proc/tc3162/adsl_cell_cnt1",context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_near_ituid(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_NEAR_ITUID,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_statistic_2(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_STATISTIC_2,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_last_drop_reason(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_LAST_DROP_REASON,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_far_ituid(char *context, const int size) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_to_blapi_interface(BLAPI_XDSL_GET_ADSL_FAR_ITUID,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_adsl_cwmp_diagnostic(char *context, const int size ,int idx) {
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	char str[128]={0};
	sprintf(str,"/proc/tc3162/adsl_cwmp_diagnostic%d",idx);
	ret = get_proc_data(str,context,size);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_get_client_uplink_ratelimit(unsigned int *ratelimit)
{
#if defined(TCSUPPORT_SISM_HOST)
	char keywards[][64] = 
	{
		{"far-end interleaved channel bit rate: "},
		{"far-end fast channel bit rate: "},
		{""}

	};
	char str_upratelimit[16] = {0};
	unsigned int upratelimit = 0;
	int i = 0;
	
    LIB_BLAPI_DEBUG("enter=====>\n");

	for(i = 0; strlen(keywards[i]) != 0; i++)
	{
		blapi_xdsl_set_key_val_to_attr(keywards[i], str_upratelimit, sizeof(str_upratelimit));
		if( 1 == sscanf(str_upratelimit, "%u kbps", &upratelimit) && upratelimit > 0 )
		{
			*ratelimit = upratelimit;
			LIB_BLAPI_DEBUG("leave,return 0 upratelimit=[%u]=====>\n", upratelimit);
			return 0;
		}
	}

	return -1;
#else
	return -1;
#endif
}

int send_setid_to_blapi_interface(int id, char* buf)
{
#if defined(TCSUPPORT_SISM_HOST)
    blapi_hdr_t blapi_h;
	int ret = -1;

	memset(&blapi_h, 0, sizeof(blapi_hdr_t));
	blapi_h.blapi_id = htonl(id);
	blapi_h.argc = htonl(1);
	memcpy(blapi_h.argv1.ch, buf, strlen(buf));

	if (!SRC_blapi(&blapi_h)){
		ret = blapi_h.ret;
	}

	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_power_up_down(int power_st)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_SET_POWER_UP_DOWN);
	blapi_h->argc = htonl(1);

	blapi_h->argv1.i = htonl(power_st);
							
	if (!SRC_blapi(blapi_h))
	{
		power_st= ntohl(blapi_h->argv1.i);

		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	return -1;
#endif
}

int blapi_sfp_set_power_up_down(int power_st)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_SFP_SET_POWER_UP_DOWN);
	blapi_h->argc = htonl(1);

	blapi_h->argv1.i = htonl(power_st);

	if (!SRC_blapi(blapi_h))
	{
		power_st= ntohl(blapi_h->argv1.i);

		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_annex(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_ANNEX,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_low_power(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_LOW_POWER,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_aelem(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_AELEM,context);
	return ret;
#else
	return -1;
#endif
}
    
int blapi_xdsl_set_adsl_testlab(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_TESTLAB,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_sysvid(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_SYSVID,context);
	return ret;
#else
	return -1;
#endif
}
    
int blapi_xdsl_set_adsl_version(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_VERSION,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_ginp(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_GINP,context);
	return ret;
#else
	return -1;
#endif
}
    
int blapi_xdsl_set_adsl_sra(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_SRA,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_gvector(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_GVECTOR,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_bitswap(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_BITSWAP,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_sos_roc(int sos, int roc)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	blapi_hdr_t *blapi_h;
	blapi_h = (blapi_hdr_t *)malloc(sizeof(blapi_hdr_t));
	memset(blapi_h, 0 , sizeof(blapi_hdr_t));
	blapi_h->blapi_id = htonl(BLAPI_XDSL_SET_ADSL_SOS_ROC);
	blapi_h->argc = htonl(2);

	blapi_h->argv1.i = htonl(sos);
	blapi_h->argv2.i = htonl(roc);

	if (!SRC_blapi(blapi_h))
	{
		sos= ntohl(blapi_h->argv1.i);
		roc= ntohl(blapi_h->argv2.i);

		ret = blapi_h->ret;
	}

	free(blapi_h);
	return ret; 
#else
	return -1;
#endif
}
    
int blapi_xdsl_set_adsl_us0(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_US0,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_dying_gasp(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_DYING_GASP,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_virtual_noise(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_VIRTUAL_NOISE,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_mode(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_MODE,context);
	return ret;
#else
	return -1;
#endif
}

int blapi_xdsl_set_adsl_profile(char* context)
{
#if defined(TCSUPPORT_SISM_HOST)
        int ret = 0;
        ret = send_setid_to_blapi_interface(BLAPI_XDSL_SET_ADSL_PROFILE,context);
        return ret;
#else
	return -1;
#endif
}

int blapi_get_procbuff(char *proc_name, char* buff, int size)
{
	FILE *fp = NULL;
	int len;
	
	fp = fopen(proc_name, "r");
	if(!fp)
	{
		return -1;
	}

	len = fread(buff, 1, size, fp);
	if(len <= 0)
	{
		buff[0] = '\0';
		fclose(fp);
		return -1;
	}
	else if(len < size)
	{
		buff[len] = '\0';
	}
	else
	{
		buff[len - 1] = '\0';
	}
	fclose(fp);
	return 0;
}

static uint64_t blapi_get_key_string(char* buff, char *keyword, char *value)
{
	char *pch_itemname	= NULL;

	if(keyword == NULL || strlen(keyword) == 0 || value == NULL)
		return -1;

	value[0] = '\0';
	pch_itemname = strstr(buff, keyword);
	if (pch_itemname != NULL)
	{
		pch_itemname += strlen(keyword);
		sscanf(pch_itemname, ": %[^\n]", value);
		return 0;
	}

	return -1;
}

static uint64_t blapi_get_key_vlaue(char *str, char *keyword, int base)
{
	char *pch_itemname 	= NULL;
	char *pch_temp 		= NULL;
	char value[11]		= {0};

	pch_itemname = strstr(str, keyword);
	if (pch_itemname != NULL)
	{
		pch_temp = strstr(pch_itemname,"= ");
		if (pch_temp != NULL)
		{
			strncpy(value,pch_temp+2, 10);
			return strtoul(value,NULL, base);
		}
	}
	return -1;
}

int blapi_get_adsl_stats_key_vlaue(char *keyword, char *str, int slen)
{
	char f_buf[256] = {0};
	FILE *fp = NULL;
	char *pch_itemname 	= NULL;
	char *pch_temp 		= NULL;
	char value[64]		= {0};
	
	fp = fopen("/proc/tc3162/adsl_stats", "r");
	if(fp)
	{
		memset(f_buf, 0, sizeof(f_buf));
		while (fgets(f_buf, sizeof(f_buf), fp) )
		{
			pch_itemname = strstr(f_buf, keyword);
			if (pch_itemname != NULL)
			{
				pch_temp = strstr(pch_itemname,":");
				if (pch_temp != NULL)
				{
					do
					{
						pch_temp++;
					}while(pch_temp[0]==' '); /* Remove blank */
					strncpy(value, pch_temp, sizeof(value));
					if('\n' == value[strlen(value) - 1])
					{
						value[strlen(value)-1] = '\0';
					}
					strncpy(str, value, slen);
					str[slen] = '\0';
					fclose(fp);
					return 0;
				}
			}				
		}	
		fclose(fp);
	}
	return -1;
}

int blapi_get_vdsl_interface_key_vlaue(char *keyword, char *str, int slen)
{
	char f_buf[256] = {0};
	FILE *fp = NULL;
	char *pch_itemname	= NULL;
	char *pch_temp		= NULL;
	char value[64]		= {0};
	
	fp = fopen("/proc/tc3162/vdsl_interface_config", "r");
	if(fp)
	{
		memset(f_buf, 0, sizeof(f_buf));
		while (fgets(f_buf, sizeof(f_buf), fp) )
		{
			pch_itemname = strstr(f_buf, keyword);
			if (pch_itemname != NULL)
			{
				pch_temp = strstr(pch_itemname,": ");
				if (pch_temp != NULL)
				{
					strncpy(value,pch_temp+2, sizeof(value));
					if('\n' == value[strlen(value) - 1])
					{
						value[strlen(value)-1] = '\0';
					}
					strncpy(str, value, slen);
					str[slen] = '\0';
					fclose(fp);
					return 0;
				}
			}				
		}
		fclose(fp);
	}
	return -1;
}

int blapi_get_adsl_enable_key_vlaue(char *keyword, char *path, char *str, int slen)
{
	char f_buf[256] = {0};
	FILE *fp = NULL;
	char *pch_itemname 	= NULL;
	char *pch_temp 		= NULL;
	char value[64]		= {0};
	
	fp = fopen(path, "r");
	if(fp)
	{
		memset(f_buf, 0, sizeof(f_buf));
		while (fgets(f_buf, sizeof(f_buf), fp) )
		{
			pch_itemname = strstr(f_buf, keyword);
			if (pch_itemname != NULL)
			{
				pch_temp = strstr(pch_itemname,"= ");
				if (pch_temp != NULL)
				{
					strncpy(value,pch_temp+2, sizeof(value));
					if('\n' == value[strlen(value) - 1])
					{
						value[strlen(value)-1] = '\0';
					}
					strncpy(str, value, slen);
					str[slen] = '\0';
					fclose(fp);
					return 0;
				}
			}				
		}
		fclose(fp);
	}
	return -1;
}

uint8_t blapi_get_multiTen_value(char *str, char *strResult)
{
	char *pos;
	char *sub = ".";
	char *curr = str;
	int i = 0;

	char preStr[20] = {0};
	char sufStr[3] = {0};
	int preValue = 0;
	int sufValue = 0;
	int value = 0;
	pos = strstr(str, sub);
	if(pos)
	{
		while(curr < pos)
			preStr[i++] = *curr++;
		preStr[i] = '\0';
		sufStr[0] = *(++curr);

		preValue = atoi(preStr);
		sufValue = atoi(sufStr);
		value = preValue*10 + sufValue;
		sprintf(strResult, "%d", value);
		return 0;
	}else
	{
		ecnt_blapi_printf("\r\nwe cannot found the specific string\n");
		return -1;

	}
}

uint8_t blapi_get_value_withoutkbps(char *origin, char*result)
{
	char *sub = " kbps";
	char *pos;
	char *curr = origin;
	int i = 0;
	pos = strstr(origin, sub);
	if(pos)
	{
		while(curr < pos)
			result[i++] = *curr++;
		result[i] = '\0';
		return 0;
	}else
	{
		ecnt_blapi_printf("\r\nwe cannot found the specific string\n");
		return -1;

	}
}

int blapi_xdsl_get_linenum(int *linenum)
{
	if ( NULL == linenum )
	{
		return -1;
	}

	*linenum = 1;
	
	return 0;
}

int blapi_xdsl_get_used_standard(char *str)
{
	char value[64] = {0};	
  	char* temp = NULL;
	char tmpbuf[10] = {0};

	struct adslpara
	{
		char* standard;
		char* type;
		int flag;
	};
	struct adslpara adslparaarray[]=
	{
		{"G.DMT", "G.992.1_",1},
		{"G.LITE", "G.992.2",0},
		{"ANSI T1.413", "T1.413",0},
		{"ETSI", "ETSI_101_388",0},
		{"ADSL2", "G.992.3_",1},
		{"ADSL2 PLUS", "G.992.5_",1},
		{"Multi-Mode", "G.992.5_",1},
		{"VDSL2", "G.993.2",0},
		{NULL,NULL,0}
	};

	struct adslpara *adslparaptr = NULL;

	memset(value, 0, sizeof(value));
#if 0
	blapi_get_adsl_stats_key_vlaue("Adsl Standard", value, sizeof(value) - 1);
#else
	FILE *fp = NULL;
	fp = fopen("/proc/tc3162/client_info_link_standard", "r");
	if ( NULL != fp )
	{
		fgets(value, 60, fp);
		ecnt_blapi_printf("\r\n[%s] [%d]: get Link standard = %s\n",__FUNCTION__, __LINE__, value);
		fclose(fp);
	}
#endif

	for(adslparaptr = &adslparaarray[0]; adslparaptr->standard != NULL; adslparaptr++) 
	{
		if(!strcmp(value, adslparaptr->standard)) 
		{
			snprintf(value, sizeof(value), "%s", adslparaptr->type);
			break;
		}
	}
	if(adslparaptr->standard == NULL) 
	{
		/*ecnt_blapi_printf("\r\nNot support this type:%s",value);*/
		str = NULL;
		return 0;
	}
	if(1 == adslparaptr->flag) 
	{
		blapi_get_adsl_stats_key_vlaue("Adsl Type", tmpbuf, sizeof(tmpbuf) - 1);
		temp = strstr(tmpbuf, "ANNEX_");
		if(NULL != temp) 
		{
			strncat(value, temp, 7);
		}
	}
	strncpy(str, value, sizeof(value) - 1);

	return 0;
}

int blapi_xdsl_set_dslline_enable(int *enable)
{	
	if ( NULL == enable )
	{
		return -1;
	}

	if( 1 == *enable )
	{
		system("/sbin/insmod /lib/modules/mt7510sar.ko");
		system("echo 'dslline_enable= 1' > /tmp/etc/dslline_enable");
	}
	else if( 0 == *enable )
	{		
		system("rmmod /lib/modules/mt7510sar.ko");
		system("echo 'dslline_enable= 0' > /tmp/etc/dslline_enable");
	}
	return 0;
}

int blapi_xdsl_get_dslline_enable(int *dsl_enable)
{
	int  fd = 0, ret = 0;
	char value[64] = {0};
	char *enable = NULL;

	fd = open("/tmp/etc/dslline_enable",O_RDONLY);
	if(fd == -1){
		return -1;
	}

	enable = (char *)malloc(MAX_BUF_SIZE);
	if(NULL == enable)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		close(fd);
		return -1;
	}
	memset(enable,0, MAX_BUF_SIZE);

	ret = read(fd, enable, MAX_BUF_SIZE);
	if(ret <= 0){
		close(fd);
		enable[0] = '\0';
		free(enable);
		return -1;
	}

	if( ret < MAX_BUF_SIZE )
	{
		enable[ret] = '\0';
	}
	else
	{
		enable[ret - 1] = '\0';
	}
	close(fd);

	*dsl_enable = blapi_get_key_vlaue(enable, "dslline_enable", 10);

	if(NULL != enable)
	{
		free(enable);
		enable = NULL;
	}
	return 0;
}

int blapi_xdsl_set_xtmline_enable(int *enable)
{
	if ( NULL == enable )
	{
		return -1;
	}

	if( 1 == *enable )
	{
		system("/sbin/insmod /lib/modules/mt7510ptm.ko");
		system("echo 'xtmline_enable= 1' > /tmp/etc/xtmline_enable");
	}
	else if( 0 == *enable)
	{		
		system("rmmod /lib/modules/mt7510ptm.ko");		
		system("echo 'xtmline_enable= 0' > /tmp/etc/xtmline_enable");
	}
	return 0;
}

int blapi_xdsl_get_xtmline_enable(int *xtm_enable)
{
	int  fd = 0, ret = 0;
	char value[64] = {0};
	char *enable = NULL;

	fd = open("/tmp/etc/xtmline_enable",O_RDONLY);
	if(fd == -1){
		return -1;
	}

	enable = (char *)malloc(MAX_BUF_SIZE);
	if(NULL == enable)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		close(fd);
		return -1;
	}
	memset(enable,0, MAX_BUF_SIZE);

	ret = read(fd, enable, MAX_BUF_SIZE);
	if(ret <= 0){
		close(fd);
		enable[0] = '\0';
		free(enable);
		return -1;
	}

	if( ret < MAX_BUF_SIZE )
	{
		enable[ret] = '\0';
	}
	else
	{
		enable[ret - 1] = '\0';
	}
	close(fd);

	*xtm_enable = blapi_get_key_vlaue(enable, "xtmline_enable", 10);

	if(NULL != enable)
	{
		free(enable);
		enable = NULL;
	}
	return 0;
}

int blapi_xdsl_get_DDSLLineNode(DDSLLineNode_t *DDSLLine)
{
	char value[64] = {0};	
	char result[64] = {0};
	char* temp = NULL;
	char tmpbuf[10] = {0};

	int  fd = 0, ret = -1;
	char *enable = NULL;

	char *adslStatsBuff = NULL;
	char *vdslIfCfgBuff = NULL;

	adslStatsBuff = (char *)malloc(MAX_ADSL_STATUS_BUF_SIZE);
	if(NULL == adslStatsBuff)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		goto _end_get_dslline;
	}

	vdslIfCfgBuff = (char *)malloc(MAX_BUF_SIZE);
	if(NULL == vdslIfCfgBuff)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		goto _end_get_dslline;
	}

	blapi_get_procbuff("/proc/tc3162/adsl_stats", adslStatsBuff, MAX_ADSL_STATUS_BUF_SIZE);
	blapi_get_procbuff("/proc/tc3162/vdsl_interface_config", vdslIfCfgBuff, MAX_BUF_SIZE);

	//Enable
	fd = open("/tmp/etc/dslline_enable",O_RDONLY);
	if(fd == -1)
	{
		DDSLLine->Enable = 0;
	}
	else
	{
		enable = (char *)malloc(MAX_BUF_SIZE);
		if(NULL == enable)
		{
			ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
			close(fd);
			goto _end_get_dslline;
		}
		memset(enable,0, MAX_BUF_SIZE);
		
		ret = read(fd, enable, MAX_BUF_SIZE);
		if(ret <= 0)
		{
			enable[0] = '\0';
			DDSLLine->Enable = 0;
		}
		else
		{
			if( ret < MAX_BUF_SIZE )
			{
				enable[ret] = '\0';
			}
			else
			{
				enable[ret - 1] = '\0';
			}
			DDSLLine->Enable = blapi_get_key_vlaue(enable, "dslline_enable", 10);
		}
		close(fd);
	}
	
	if(NULL != enable)
	{
		free(enable);
		enable = NULL;
	}

	//Name
	strncpy(DDSLLine->Name, "", sizeof(DDSLLine->Name) - 1);
	
	//LastChange
	DDSLLine->LastChange = 0;
	
	//Upstream
	DDSLLine->Upstream = 1;

	//FirmwareVersion
	memset(value, 0, sizeof(value));	
	blapi_xdsl_get_fwver(value, sizeof(value));
	strncpy(DDSLLine->FirmwareVersion, value, sizeof(DDSLLine->FirmwareVersion) - 1);

	//LinkStatus
	memset(value, 0, sizeof(value));
	blapi_get_key_string(adslStatsBuff, "ADSL link status", value);
	strncpy(DDSLLine->Status, "Down", sizeof(DDSLLine->Status) - 1); /* Expected to be "Down", except only when LinkStatus is "Up" */
	if('\0' != value[0])
	{
		if(!strcmp(value, "up"))
		{
			strncpy(DDSLLine->LinkStatus, "Up", sizeof(DDSLLine->LinkStatus) - 1);
			strncpy(DDSLLine->Status, "Up", sizeof(DDSLLine->Status) - 1);
		}
		else if(!strcmp(value, "down"))
		{
			strncpy(DDSLLine->LinkStatus, "NoSignal", sizeof(DDSLLine->LinkStatus) - 1);
		}
		else if(!strcmp(value, "wait for init") || !strcmp(value, "initializing"))
		{
			strncpy(DDSLLine->LinkStatus, "Initializing", sizeof(DDSLLine->LinkStatus) - 1);
		}
		else
		{
			strncpy(DDSLLine->LinkStatus, "Error", sizeof(DDSLLine->LinkStatus) - 1);
		}
	}
	else
	{
		strncpy(DDSLLine->LinkStatus, "Disabled", sizeof(DDSLLine->LinkStatus) - 1);
	}
	
	//StandardsSupported
	strncpy(DDSLLine->StandardsSupported, 
				"G.992.1_Annex_A,"
				"G.992.1_Annex_B,"
				"T1.413,"
				"T1.413i2,"
				"ETSI_101_388,"
				"G.992.2,"
				"G.992.3_Annex_A,"
				"G.992.3_Annex_B,"
				"G.992.3_Annex_I,"
				"G.992.3_Annex_J,"
				"G.992.3_Annex_L,"
				"G.992.3_Annex_M,"
				"G.992.4,"
				"G.992.5_Annex_A,"
				"G.992.5_Annex_B,"
				"G.992.5_Annex_I,"
				"G.992.5_Annex_J,"
				"G.992.5_Annex_M", sizeof(DDSLLine->StandardsSupported) - 1);

	//StandardUsed
	struct adslpara
	{
		char* standard;
		char* type;
		int flag;
	};
	struct adslpara adslparaarray[]=
	{
		{"G.DMT", "G.992.1_",1},
		{"G.LITE", "G.992.2",0},
		{"ANSI T1.413", "T1.413",0},
		{"ETSI", "ETSI_101_388",0},
		{"ADSL2", "G.992.3_",1},
		{"ADSL2 PLUS", "G.992.5_",1},
		{"Multi-Mode", "G.992.5_",1},
		{"VDSL2", "G.993.2",0},
		{NULL,NULL,0}
	};

	struct adslpara *adslparaptr = NULL;

	memset(value, 0, sizeof(value));
	blapi_get_key_string(adslStatsBuff, "Adsl Standard", value);
	for(adslparaptr = &adslparaarray[0]; adslparaptr->standard != NULL; adslparaptr++) 
	{
		if(!strcmp(value, adslparaptr->standard)) 
		{
			snprintf(value, sizeof(value), "%s", adslparaptr->type);
			break;
		}
	}
	if(adslparaptr->standard == NULL) 
	{
		/*ecnt_blapi_printf("\r\nNot support this type:%s",value);*/
		strncpy(DDSLLine->StandardUsed, "", sizeof(DDSLLine->StandardUsed) - 1);	
	}
	else
	{		
		if(1 == adslparaptr->flag) 
		{
			blapi_get_key_string(adslStatsBuff, "Adsl Type", tmpbuf);
			temp = strstr(tmpbuf, "ANNEX_");
			if(NULL != temp) 
			{
				strncat(value, temp, 7);
			}
		}
		strncpy(DDSLLine->StandardUsed, value, sizeof(DDSLLine->StandardUsed) - 1);
	}
	
	//LineEncoding
	strncpy(DDSLLine->LineEncoding, "DMT", sizeof(DDSLLine->LineEncoding) - 1);

	//AllowedProfiles
	strncpy(DDSLLine->AllowedProfiles, "8a,8b,8c,8d,12a,12b,17a,17b,30a", sizeof(DDSLLine->AllowedProfiles) - 1);

	//CurrentProfile
	memset(value, 0, sizeof(value));
	blapi_get_key_string(vdslIfCfgBuff, "CurrentProfiles", value);
	if('\0' != value[0])
	{
		strncpy(DDSLLine->CurrentProfile, value, sizeof(DDSLLine->CurrentProfile) - 1);
	}

	//PowerManagementState
	strncpy(DDSLLine->PowerManagementState, "", sizeof(DDSLLine->PowerManagementState) - 1);

	//SuccessFailureCause
	DDSLLine->SuccessFailureCause = 0;

	//UPBOKLE
	memset(value, 0, sizeof(value));
	blapi_get_key_string(vdslIfCfgBuff, "UPBOKLE", value);
	if('\0' != value[0])
	{
		DDSLLine->UPBOKLE = atoi(value);	
	}
	else
	{
		DDSLLine->UPBOKLE = 0;
	}

	//TRELLISds	
	memset(value, 0, sizeof(value));
	blapi_get_key_string(vdslIfCfgBuff, "TRELLISds", value);
	if('\0' != value[0])
	{
		DDSLLine->TRELLISds = atoi(value);	
	}
	else
	{
		DDSLLine->TRELLISds = -1;
	}

	//TRELLISus	
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(vdslIfCfgBuff, "TRELLISus", value);
	if('\0' != value[0])
	{
		DDSLLine->TRELLISus = atoi(value);	
	}
	else
	{
		DDSLLine->TRELLISus = -1;
	}

	//ACTSNRMODEds
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(vdslIfCfgBuff, "ACTSNRMODEds", value);
	if('\0' != value[0])
	{
		DDSLLine->ACTSNRMODEds = atoi(value);	
	}
	else
	{
		DDSLLine->ACTSNRMODEds = 0;
	}

	//ACTSNRMODEus
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(vdslIfCfgBuff, "ACTSNRMODEus", value);
	if('\0' != value[0])
	{
		DDSLLine->ACTSNRMODEus = atoi(value);	
	}
	else
	{
		DDSLLine->ACTSNRMODEus = 0;
	}

	//LineNumber
	DDSLLine->LineNumber = 1;

	//UpstreamMaxBitRate
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(adslStatsBuff, "attain upstream", value);
	if('\0' != value[0])
	{
		DDSLLine->UpstreamMaxBitRate = atoi(value);	
	}
	else
	{
		DDSLLine->UpstreamMaxBitRate = 0;
	}

	//DownstreamMaxBitRate
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(adslStatsBuff, "attain downstream", value);
	if('\0' != value[0])
	{
		DDSLLine->DownstreamMaxBitRate = atoi(value); 
	}
	else
	{
		DDSLLine->DownstreamMaxBitRate = 0;
	}

	//UpstreamNoiseMargin
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "noise margin upstream", value);
	if('\0' != value[0] && 0 == blapi_get_multiTen_value(value, result))
	{
		DDSLLine->UpstreamNoiseMargin = atoi(result); 
	}
	else
	{
		DDSLLine->UpstreamNoiseMargin = 0;
	}

	//DownstreamNoiseMargin
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "noise margin downstream", value);
	if('\0' != value[0] && 0 == blapi_get_multiTen_value(value, result))
	{
		DDSLLine->DownstreamNoiseMargin = atoi(result); 
	}
	else
	{
		DDSLLine->DownstreamNoiseMargin = 0;
	}

	//SNRMpbus
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(vdslIfCfgBuff, "SNRMpbus", value);
	if('\0' != value[0])
	{
		strncpy(DDSLLine->SNRMpbus, value, sizeof(DDSLLine->SNRMpbus) - 1);
	}
	else
	{
		strncpy(DDSLLine->SNRMpbus, "", sizeof(DDSLLine->SNRMpbus) - 1);
	}

	//SNRMpbds
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(vdslIfCfgBuff, "SNRMpbds", value);
	if('\0' != value[0])
	{
		strncpy(DDSLLine->SNRMpbds, value, sizeof(DDSLLine->SNRMpbds) - 1);
	}
	else
	{
		strncpy(DDSLLine->SNRMpbds, "", sizeof(DDSLLine->SNRMpbds) - 1);
	}

	//INMIATOds
	DDSLLine->INMIATOds = 0;

	//INMIATSds
	DDSLLine->INMIATSds = 0;

	//INMCCds
	DDSLLine->INMCCds = 0;

	//INMINPEQMODEds
	DDSLLine->INMINPEQMODEds = 0;

	//UpstreamAttenuation
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "attenuation upstream", value);
	if('\0' != value[0] && 0 == blapi_get_multiTen_value(value, result))
	{
		DDSLLine->UpstreamAttenuation = atoi(result); 
	}
	else
	{
		DDSLLine->UpstreamAttenuation = 0;
	}

	//DownstreamAttenuation	
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "attenuation downstream", value);
	if('\0' != value[0] && 0 == blapi_get_multiTen_value(value, result))
	{
		DDSLLine->DownstreamAttenuation = atoi(result); 
	}
	else
	{
		DDSLLine->DownstreamAttenuation = 0;
	}

	//UpstreamPower
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "output power upstream", value);
	if('\0' != value[0] && 0 == blapi_get_multiTen_value(value, result))
	{
		DDSLLine->UpstreamPower = atoi(result); 
	}
	else
	{
		DDSLLine->UpstreamPower = 0;
	}

	//DownstreamPower
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "output power downstream", value);
	if('\0' != value[0] && 0 == blapi_get_multiTen_value(value, result))
	{
		DDSLLine->DownstreamPower = atoi(result); 
	}
	else
	{
		DDSLLine->DownstreamPower = 0;
	}

	//XTURVendor	
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "near end itu identification", value);
	if('\0' != value[0])
	{	
		memcpy(result, value+4, 8);
		result[8] = '\0';
		strncpy(DDSLLine->XTURVendor, result, sizeof(DDSLLine->XTURVendor) - 1);
	}
	
	//XTURCountry	
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "near end itu identification", value);
	if('\0' != value[0])
	{
		memcpy(result, value, 4);
		result[4] = '\0';
		strncpy(DDSLLine->XTURCountry, result, sizeof(DDSLLine->XTURCountry) - 1);
	}

	//XTURANSIStd	
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(adslStatsBuff, "ATURANSIStd", value);
	if('\0' != value[0])
	{
		DDSLLine->XTURANSIStd = atoi(value); 
	}
	else
	{
		DDSLLine->XTURANSIStd = 0;
	}

	//XTURANSIRev
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(adslStatsBuff, "ATURANSIRev", value);
	if('\0' != value[0])
	{
		DDSLLine->XTURANSIRev = atoi(value); 
	}
	else
	{
		DDSLLine->XTURANSIRev = 0;
	}

	//XTUCVendor
	memset(value, 0, sizeof(value));
	memset(result, 0, sizeof(result));
	blapi_get_key_string(adslStatsBuff, "far end itu identification", value);
	if('\0' != value[0])
	{
		memcpy(result, value+4, 8);
		result[8] = '\0';
		strncpy(DDSLLine->XTUCVendor, result, sizeof(DDSLLine->XTUCVendor) - 1);
	}

	//XTUCCountry
	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "far end itu identification", value);
	if('\0' != value[0])
	{
		memcpy(result, value, 4);
		result[4] = '\0';
		strncpy(DDSLLine->XTUCCountry, result, sizeof(DDSLLine->XTUCCountry) - 1);
	}

	//XTUCANSIStd	
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(adslStatsBuff, "ATUCANSIStd", value);
	if('\0' != value[0])
	{
		DDSLLine->XTUCANSIStd = atoi(value); 
	}
	else
	{
		DDSLLine->XTUCANSIStd = 0;
	}

	//XTUCANSIRev
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(adslStatsBuff, "ATUCANSIRev", value);
	if('\0' != value[0])
	{
		DDSLLine->XTUCANSIRev = atoi(value); 
	}
	else
	{
		DDSLLine->XTUCANSIRev = 0;
	}

	//LastStateTransmittedDownstream
	DDSLLine->LastStateTransmittedDownstream = 0;

	//LastStateTransmittedUpstream
	DDSLLine->LastStateTransmittedUpstream = 0;

	//MREFPSDds
	DDSLLine->MREFPSDds = 0;

	//MREFPSDus
	DDSLLine->MREFPSDus = 0;

	//LIMITMASK
	DDSLLine->LIMITMASK = 0;

	//US0MASK
	DDSLLine->US0MASK = 0;

	//VirtualNoisePSDds
	DDSLLine->VirtualNoisePSDds = 0;

	//VirtualNoisePSDus
	DDSLLine->VirtualNoisePSDus = 0;

	//ACTUALCE
	memset(value, 0, sizeof(value));	
	blapi_get_key_string(vdslIfCfgBuff, "ACTUALCE", value);
	if('\0' != value[0])
	{
		DDSLLine->ACTUALCE = atoi(value);
	}
	else
	{
		DDSLLine->ACTUALCE = 99;
	}

	ret = 0;

_end_get_dslline:

	if(NULL != vdslIfCfgBuff)
	{
		free(vdslIfCfgBuff);
		vdslIfCfgBuff = NULL;
	}

	if(NULL != adslStatsBuff)
	{
		free(adslStatsBuff);
		adslStatsBuff = NULL;
	}

	return ret;
}

int blapi_xdsl_get_DDSLLineStatsNode(DDSL_StatsNode_t *Line_Stats)
{
	int  fd = 0, ret = -1;
	char value[64] = {0};
	char *tsarm = NULL;
	char *adslStatsBuff = NULL;

	adslStatsBuff = (char *)malloc(MAX_ADSL_STATUS_BUF_SIZE);
	if(NULL == adslStatsBuff)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		return -1;
	}
	blapi_get_procbuff("/proc/tc3162/adsl_stats", adslStatsBuff, MAX_ADSL_STATUS_BUF_SIZE);
	memset(value, 0, sizeof(value));
	blapi_get_key_string(adslStatsBuff, "ADSL total ativetime second", value);
	if('\0' != value[0])
	{
		Line_Stats->TotalStart = atoi(value);
	}
	else
	{
		Line_Stats->TotalStart = 0;
	}

	memset(value, 0, sizeof(value));
	blapi_get_key_string(adslStatsBuff, "ADSL activetime second", value);
	if('\0' != value[0])
	{
		Line_Stats->ShowtimeStart = atoi(value);
	}
	else
	{
		Line_Stats->ShowtimeStart = 0;
	}

	Line_Stats->LastShowtimeStart      = 0;
	Line_Stats->CurrentDayStart        = 0;
	Line_Stats->QuarterHourStart       = 0;

	memset(value, 0, sizeof(value));
	blapi_get_key_string(adslStatsBuff, "TPSTC type", value);

	free(adslStatsBuff);

	/* ATM only */
	if(strstr(value, "ATM"))
	{
		tsarm = (char *)malloc(MAX_BUF_SIZE);
		if(NULL == tsarm)
		{
			ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
			return -1;
		}
		blapi_get_procbuff("/proc/tc3162/tsarm_stats", tsarm, MAX_BUF_SIZE);
		
		Line_Stats->BytesSent			   = (uint64_t)blapi_get_key_vlaue(tsarm, "outBytes", 16);
		Line_Stats->BytesReceived		   = (uint64_t)blapi_get_key_vlaue(tsarm, "inBytes", 16);
		Line_Stats->PacketsSent 		   = (uint64_t)blapi_get_key_vlaue(tsarm, "outPkts", 16);
		Line_Stats->PacketsReceived 	   = (uint64_t)blapi_get_key_vlaue(tsarm, "inPkts", 16);
		Line_Stats->ErrorsSent			   = 0;
		Line_Stats->ErrorsReceived		   = 0;
		Line_Stats->DiscardPacketsSent	   = (uint32_t)blapi_get_key_vlaue(tsarm, "outDiscards", 16);
		Line_Stats->DiscardPacketsReceived = (uint32_t)blapi_get_key_vlaue(tsarm, "inDiscards", 16);

		free(tsarm);
	}

	return 0;
}

int blapi_xdsl_get_DDSLChannelInfoNode(DDSLChannelInfoNode_t *ChannelInfo)
{
	char value[64] = {0};
	char result[64] = {0};

	char *adslStatsBuff = NULL;

	adslStatsBuff = (char *)malloc(MAX_ADSL_STATUS_BUF_SIZE);
	if(NULL == adslStatsBuff)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		return -1;
	}

	blapi_get_procbuff("/proc/tc3162/adsl_stats", adslStatsBuff, MAX_ADSL_STATUS_BUF_SIZE);

	//ChannelInfo->Enable
	//Not found parameter in driver.
	
	if( 1 == ChannelInfo->Enable )
	{
		strncpy(ChannelInfo->Status, "Up", sizeof(ChannelInfo->Status) - 1);
	}
	else
	{
		strncpy(ChannelInfo->Status, "Down", sizeof(ChannelInfo->Status) - 1);
	}
	strncpy(ChannelInfo->Alias, "", sizeof(ChannelInfo->Alias) - 1);
	strncpy(ChannelInfo->Name, "", sizeof(ChannelInfo->Name) - 1);
	ChannelInfo->LastChange = 0;
	strncpy(ChannelInfo->LowerLayers, "", sizeof(ChannelInfo->LowerLayers) - 1);
	strncpy(ChannelInfo->LinkEncapsulationSupported, "G.992.3_Annex_K_ATM", sizeof(ChannelInfo->LinkEncapsulationSupported) - 1);
	strncpy(ChannelInfo->LinkEncapsulationUsed, "G.992.3_Annex_K_ATM", sizeof(ChannelInfo->LinkEncapsulationUsed) - 1);
	ChannelInfo->LPATH = 0;

	memset(value, 0, sizeof(value));
	blapi_get_key_string(adslStatsBuff, "Interleave Depth", value);
	if('\0' != value[0])
	{
		ChannelInfo->INTLVDEPTH = atoi(value);
	}
	else
	{
		ChannelInfo->INTLVDEPTH = 0;
	}

	
	ChannelInfo->INTLVBLOCK = -1;
	ChannelInfo->ActualInterleavingDelay = 0;
	ChannelInfo->ACTINP= -1;
	ChannelInfo->INPREPORT = 0;
	ChannelInfo->NFEC = -1;
	ChannelInfo->RFEC = -1;
	ChannelInfo->LSYMB = -1;

	memset(value, 0, sizeof(value));
	memset(result, 0, sizeof(result));
	blapi_get_key_string(adslStatsBuff, "far-end interleaved channel bit rate", value);
	if('\0' != value[0] && 0 == blapi_get_value_withoutkbps(value, result))
	{
		ChannelInfo->UpstreamCurrRate = atoi(result);  
	}
	else
	{
		ChannelInfo->UpstreamCurrRate = 0;
	}

	memset(value, 0, sizeof(value));	
	memset(result, 0, sizeof(result));	
	blapi_get_key_string(adslStatsBuff, "near-end interleaved channel bit rate", value);
	if('\0' != value[0] && 0 == blapi_get_value_withoutkbps(value, result))
	{
		ChannelInfo->DownstreamCurrRate = atoi(result);
	}
	else
	{
		ChannelInfo->DownstreamCurrRate = 0;
	}

	if(NULL != adslStatsBuff)
	{
		free(adslStatsBuff);
		adslStatsBuff = NULL;
	}

	return 0;
}

int blapi_xdsl_get_DDSLChannelStatsNode(DDSL_StatsNode_t *Channel_Stats)
{
	int ret = 0;
	DDSL_StatsNode_t Line_Stats;
	memset(&Line_Stats, 0, sizeof(DDSL_StatsNode_t) );
	ret = blapi_xdsl_get_DDSLLineStatsNode(&Line_Stats);
	if(ret < 0)
	{
		return -1;
	}
	else
	{
		Channel_Stats->BytesSent = Line_Stats.BytesSent; 
		Channel_Stats->BytesReceived = Line_Stats.BytesReceived;
		Channel_Stats->PacketsSent = Line_Stats.PacketsSent;
		Channel_Stats->PacketsReceived = Line_Stats.PacketsReceived;		
		Channel_Stats->ErrorsSent = 0;
		Channel_Stats->ErrorsReceived = 0;		
		Channel_Stats->DiscardPacketsSent = Line_Stats.DiscardPacketsSent;
		Channel_Stats->DiscardPacketsReceived = Line_Stats.DiscardPacketsReceived;		
		Channel_Stats->TotalStart = 0;
		Channel_Stats->ShowtimeStart = 0;
		Channel_Stats->LastShowtimeStart = 0;
		Channel_Stats->CurrentDayStart = 0;
		Channel_Stats->QuarterHourStart = 0;
	}
	return 0;
}

static int8_t blapi_get_PTM_Stats(char* name, char* result, char * format)
{
	char Command[128] = {0};
	FILE *Command_temp = NULL;
	char *tempValue = NULL;
	
	strncpy(Command, "cat /proc/tc3162/ptm_b0_stats > /tmp/stats.tmp", sizeof(Command) - 1);
	system(Command);
	Command_temp = fopen("/tmp/stats.tmp", "r");
	if ( !Command_temp )
		return -1;
	memset(Command, 0x00, sizeof(Command));
	fseek(Command_temp,0,0);
	while(fgets(Command, sizeof(Command), Command_temp) != NULL) {
		tempValue = strstr(Command, name);
		if(tempValue != NULL) {
			sscanf(Command, format, result);
			fclose(Command_temp);
			Command_temp = NULL;
			unlink("/tmp/stats.tmp");
			return 0;
		}
	}
	strncpy(result, "0", sizeof(result) - 1);
	fclose(Command_temp);
	Command_temp = NULL;
	unlink("/tmp/stats.tmp");
	return -1;
	
}

static int8_t blapi_get_PTM_Stats1(char * format, char * result, int num, char  * name)
{
	char ifconfig[128] = {0};
	FILE *ifconfig_temp = NULL;
	char ifconfigName[20] = {0};
	char buf[20] = {0};
	int tmp = 0;
	int count = 0;
	
	strncpy(ifconfigName, "nas10", sizeof(ifconfigName) - 1);
	sprintf(ifconfig, "ifconfig %s > /tmp/ifconfig.tmp", ifconfigName);
	system(ifconfig);
	ifconfig_temp = fopen("/tmp/ifconfig.tmp", "r");
	memset(ifconfig, 0x00, sizeof(ifconfig));
	if( !ifconfig_temp )
	{
		return -1;
	}
	fseek(ifconfig_temp,0,0);
	for(count = 1; count < 8; count++) {
		fgets(ifconfig, sizeof(ifconfig), ifconfig_temp);
		if(num == 5) {	//Tx packets, DiscardPkts, errors
			if(num == count) {
				if(strstr(ifconfig, name) != NULL) {
					sscanf(ifconfig, format, buf);
					if(!strcmp(name,"packets:") || !strcmp(name, "dropped:")) {
						tmp = atoi(buf + 8);
					}
					else if(!strcmp(name, "errors:")) {
						tmp = atoi(buf + 7);
					}
					sprintf(result, "%d", tmp);
					fclose(ifconfig_temp);
					ifconfig_temp = NULL;
					unlink("/tmp/ifconfig.tmp");
					return 0;
				}
			}
		}
		else if(num == 4) {	//Rx packets,  errors
			if(num == count) {
				if(strstr(ifconfig, name) != NULL) {
					sscanf(ifconfig, format, buf);
					if(!strcmp(name,"packets:") || !strcmp(name, "dropped:")) {
						tmp = atoi(buf + 8);
					}
					else if(!strcmp(name,"errors:")) {
						tmp = atoi(buf + 7);
					}
					sprintf(result, "%d", tmp);
					fclose(ifconfig_temp);
					ifconfig_temp = NULL;
					unlink("/tmp/ifconfig.tmp");
					return 0;
				}
			}
		}
	}
	strncpy(result, "0", sizeof(result) - 1);
	fclose(ifconfig_temp);
	ifconfig_temp = NULL;
	unlink("/tmp/ifconfig.tmp");
	
	return -1;
}

int blapi_xdsl_get_DPTMLinkNode(DPTMLinkNode_t *DPTMLink)
{
	int enable = 0;
	char value[64] = {0};

	/* Enable */
	blapi_xdsl_get_xtmline_enable(&enable);
	DPTMLink->Enable = (unsigned char)enable;

	/* Status */
	memset(value, 0, sizeof(value));
	blapi_get_adsl_stats_key_vlaue("ADSL link status", value, sizeof(value) - 1);
	if(value[0] != '\0' && 1 == enable && !strcmp(value, "up"))
	{
		strncpy(DPTMLink->Status, "Up", sizeof(DPTMLink->Status) - 1);
	}
	else
	{
		strncpy(DPTMLink->Status, "Down", sizeof(DPTMLink->Status) - 1);
	}

	/* Alias */
	strncpy(DPTMLink->Alias, "", sizeof(DPTMLink->Alias) - 1);

	/* Name */
	strncpy(DPTMLink->Name, "", sizeof(DPTMLink->Name) - 1);

	/**
	* LastChange (ADSL uptime)
	*   format: [_day], [_hr_min], [_sec]
	*   [_day]: "1 day" or "n days" (depends)
	*   [_hr_min]: "12:28" or "32 min"
	*   [_sec]: 6 seconds
	*/
	uint32_t ttotal = 0;
	memset(value, 0, sizeof(value));
	blapi_get_adsl_stats_key_vlaue("ADSL uptime", value, sizeof(value) - 1);
	if(value[0] != '\0')
	{
		char *tVal = strtok(value, ",");

		/*  Get day */
		uint32_t tDay = 0;
		if(strstr(tVal, "day"))
		{
			sscanf(tVal, "%u", &tDay);
			ttotal += tDay*(24*60*60);

			tVal = strtok(NULL, ",");
		}

		/*  Get hour/min or min */
		uint32_t tHr = 0, tMin;
		if(strstr(tVal, ":"))
		{
			/* hh:mm */
			sscanf(tVal, "%u:%u", &tHr, &tMin);
			ttotal += (tHr*60 + tMin)*60;
		}
		else
		{
			/* min */
			sscanf(tVal, "%u", &tMin);
			ttotal += tMin*60;
		}
		tVal = strtok(NULL, ",");

		/* Get second */
		uint32_t tSec;
		sscanf(tVal, "%u", &tSec);
		ttotal += tSec;

		//ecnt_blapi_printf("[%s] [%d]: %ud,%uh,%um,%us = %u\n",__FUNCTION__, __LINE__, tDay, tHr, tMin, tSec, ttotal);
	}
	DPTMLink->LastChange = ttotal;

	/* LowerLayers */
	strncpy(DPTMLink->LowerLayers, "", sizeof(DPTMLink->LowerLayers) - 1);
	/*strncpy(DPTMLink->LowerLayers, "Device.PTM.Link.1", sizeof(DPTMLink->LowerLayers) - 1);*/

	/* MACAddress */
	strncpy(DPTMLink->MACAddress, "", sizeof(DPTMLink->MACAddress) - 1);
	return 0;
}

int blapi_xdsl_get_DPTMLinkStatsNode(DPTMLinkStatsNode_t *DPTMLinkStats)
{
	int  fd = 0, ret = 0;
	char value[64] = {0};
	char format[128] = {0};
	char *eth = NULL;
	
	memset(value, 0, sizeof(value));	
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats("outOctets", value, format);
	DPTMLinkStats->BytesSent = strtoul(value,NULL, 16);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats("inOctets", value, format);
	DPTMLinkStats->BytesReceived = strtoul(value,NULL, 16);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats1(format, value, 5, "packets:");
	DPTMLinkStats->PacketsSent = atoi(value);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));
	strncpy(format, "%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats1(format, value, 4, "packets:");
	DPTMLinkStats->PacketsReceived = atoi(value);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats1(format, value, 5, "errors:");
	DPTMLinkStats->ErrorsSent = atoi(value);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats1(format, value, 4,"errors:");
	DPTMLinkStats->ErrorsReceived = atoi(value);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats("outUnicastPkts", value, format);
	DPTMLinkStats->UnicastPacketsSent = strtoul(value,NULL, 16);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats("inUnicastPkts", value, format);
	DPTMLinkStats->UnicastPacketsReceived = strtoul(value,NULL, 16);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]%*[ ]%*[^ ]%*[ ]%*[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats1(format, value, 5, "dropped:");
	DPTMLinkStats->DiscardPacketsSent = atoi(value);

	memset(value, 0, sizeof(value));
	memset(format, 0, sizeof(format));	
	strncpy(format, "%*[^ ]%*[ ]%*[^ ]%*[ ]%[^ ]", sizeof(format) - 1);
	blapi_get_PTM_Stats("inDiscards", value, format);
	DPTMLinkStats->DiscardPacketsReceived = strtoul(value,NULL, 16);

	fd = open("/proc/tc3162/eth_stats",O_RDONLY);
	if(fd == -1){
		return -1;
	}
	
	eth = (char *)malloc(MAX_BUF_SIZE);
	if(NULL == eth)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		close(fd);
		return -1;
	}
	memset(eth,0, MAX_BUF_SIZE);

	ret = read(fd, eth, MAX_BUF_SIZE);
	if(ret <= 0){
		close(fd);
		eth[0] = '\0';
		free(eth);
		return -1;
	}
	
	if( ret < MAX_BUF_SIZE )
	{
		eth[ret] = '\0';
	}
	else
	{
		eth[ret - 1] = '\0';
	}
	close(fd);

	DPTMLinkStats->MulticastPacketsSent = (uint64_t)blapi_get_key_vlaue(eth, "outMulticastPkts", 16);
	DPTMLinkStats->MulticastPacketsReceived = (uint64_t)blapi_get_key_vlaue(eth, "inMulticastPkts", 16);

	DPTMLinkStats->BroadcastPacketsSent = 0;
	DPTMLinkStats->BroadcastPacketsReceived = 0;
	DPTMLinkStats->UnknownProtoPacketsReceived = 0;
	
	if(NULL != eth)
	{
		free(eth);
		eth = NULL;
	}

	return 0;
}

static const char *ss_fmt[] = {
	"%n%Lu%lu%lu%lu%lu%n%n%n%Lu%lu%lu%lu%lu%lu",
	"%Lu%Lu%lu%lu%lu%lu%n%n%Lu%Lu%lu%lu%lu%lu%lu",
	"%Lu%Lu%lu%lu%lu%lu%lu%lu%Lu%Lu%lu%lu%lu%lu%lu%lu"
};

static void get_dev_info(char *buf, struct blapi_wan_info_stats *stats, int procnetdev_vsn)
{
	memset(stats, 0, sizeof(struct blapi_wan_info_stats));
	
	sscanf(buf, ss_fmt[procnetdev_vsn],
		   &stats->rx_bytes, /* missing for 0 */
		   &stats->rx_packets,
		   &stats->rx_errors,
		   &stats->rx_dropped,
		   &stats->rx_fifo_errors,
		   &stats->rx_frame_errors,
		   &stats->rx_compressed, /* missing for <= 1 */
		   &stats->rx_multicast, /* missing for <= 1 */
		   &stats->tx_bytes, /* missing for 0 */
		   &stats->tx_packets,
		   &stats->tx_errors,
		   &stats->tx_dropped,
		   &stats->tx_fifo_errors,
		   &stats->collisions,
		   &stats->tx_carrier_errors,
		   &stats->tx_compressed /* missing for <= 1 */
		   );

	if ( procnetdev_vsn <= 1 ) {
		if ( procnetdev_vsn == 0 ) 
		{
			stats->rx_bytes = 0;
			stats->tx_bytes = 0;
		}
		stats->rx_multicast = 0;
		stats->rx_compressed = 0;
		stats->tx_compressed = 0;
	}
}

static inline int procnetdev_version(char *buf)
{
	if (strstr(buf, "compressed"))
		return 2;
	
	if (strstr(buf, "bytes"))
		return 1;
	
	return 0;
}

#define IFNAMSIZE	20

static char *get_ifname(char *name, char *p)
{
	int namestart=0, nameend=0, aliasend;
	
	while ( isspace(p[namestart]) )
		namestart++;

	nameend=namestart;
	while ( p[nameend] && p[nameend]!=':' && !isspace(p[nameend]) )
		nameend++;
	
	if ( p[nameend]==':' ) {
		aliasend=nameend+1;
		while ( p[aliasend] && isdigit(p[aliasend]) )
			aliasend++;
		if ( p[aliasend]==':' ) {
			nameend=aliasend;
		}
		if ( (nameend-namestart) < IFNAMSIZE ) {
			memcpy(name,&p[namestart],nameend-namestart);
			name[nameend-namestart]='\0';
			p=&p[nameend];
		} 
		else {
			/* Interface name too large */
			name[0]='\0';
		}
	} 
	else {
		/* first ':' not found - return empty */
		name[0]='\0';
} 
	return p + 1;
}

int blapi_xdsl_get_DATMLinkNode(DATMLinkNode_t *DATMLink)
{
	int enable = 0;
	char value[64] = {0};

	/* Enable */
	blapi_xdsl_get_dslline_enable(&enable);
	DATMLink->Enable = (unsigned char)enable;

	/* Status */
	memset(value, 0, sizeof(value));
	blapi_get_adsl_stats_key_vlaue("ADSL link status", value, sizeof(value) - 1);
	if(value[0] != '\0' && 1 == enable && !strcmp(value, "up"))
	{
		strncpy(DATMLink->Status, "Up", sizeof(DATMLink->Status) - 1);
	}
	else
	{
		strncpy(DATMLink->Status, "Down", sizeof(DATMLink->Status) - 1);
	}

	/* Alias */
	strncpy(DATMLink->Alias, "", sizeof(DATMLink->Alias) - 1);

	/* Name */
	strncpy(DATMLink->Name, "", sizeof(DATMLink->Name) - 1);

	/**
	* LastChange (ADSL uptime)
	*   format: [_day], [_hr_min], [_sec]
	*   [_day]: "1 day" or "n days" (depends)
	*   [_hr_min]: "12:28" or "32 min"
	*   [_sec]: 6 seconds
	*/
	uint32_t ttotal = 0;
	memset(value, 0, sizeof(value));
	blapi_get_adsl_stats_key_vlaue("ADSL uptime", value, sizeof(value) - 1);
	//ecnt_blapi_printf("[%s] [%d]: ADSL uptime(str) = %s\n",__FUNCTION__, __LINE__, value);
	if(value[0] != '\0')
	{
		char *tVal = strtok(value, ",");

		/* Get day */
		uint32_t tDay = 0;
		if(strstr(tVal, "day"))
		{
			sscanf(tVal, "%u", &tDay);
			ttotal += tDay*(24*60*60);

			tVal = strtok(NULL, ",");
		}

		/* Get hour/min or min */
		uint32_t tHr = 0, tMin;
		if(strstr(tVal, ":"))
		{
			/* hh:mm */
			sscanf(tVal, "%u:%u", &tHr, &tMin);
			ttotal += (tHr*60 + tMin)*60;
		}
		else
		{
			/* min */
			sscanf(tVal, "%u", &tMin);
			ttotal += tMin*60;
		}
		tVal = strtok(NULL, ",");

		/* Get second */
		uint32_t tSec;
		sscanf(tVal, "%u", &tSec);
		ttotal += tSec;

		//ecnt_blapi_printf("[%s] [%d]: %ud,%uh,%um,%us\n",__FUNCTION__, __LINE__, tDay, tHr, tMin, tSec);
	}
	//ecnt_blapi_printf("[%s] [%d]: ADSL uptime(uint) = %u\n",__FUNCTION__, __LINE__, ttotal);
	DATMLink->LastChange = ttotal;

	/* LowerLayers */
	strncpy(DATMLink->LowerLayers, "", sizeof(DATMLink->LowerLayers) - 1);
	/*trncpy(DATMLink->LowerLayers, "Device.ATM.Link.1", sizeof(DATMLink->LowerLayers) - 1);*/

	/* LinkType: [EoA/IPoA/PPPoA/CIP/Unconfigured] */
	strncpy(DATMLink->LinkType, "", sizeof(DATMLink->LinkType) - 1);

	/* AutoConfig */
	DATMLink->AutoConfig = 0;

	/* DestinationAddress: [vpi]/[vci] */
	strncpy(DATMLink->DestinationAddress, "", sizeof(DATMLink->DestinationAddress) - 1);

	/* Encapsulation: [LLC/VCMUX] */
	strncpy(DATMLink->Encapsulation, "", sizeof(DATMLink->Encapsulation) - 1);

	/* FCSPreserved */
	DATMLink->FCSPreserved = 0;

	/* VCSearchList */
	strncpy(DATMLink->VCSearchList, "", sizeof(DATMLink->VCSearchList) - 1);

	/* AAL */
	strncpy(DATMLink->AAL, "", sizeof(DATMLink->AAL) - 1);
	return 0;
}

int blapi_xdsl_get_DATMLinkStatsNode(DATMLinkStatsNode_t *DATMLinkStats)
{
	FILE *fp = NULL;
	char *buf = NULL;
	char name[128] = {0}, *s = NULL;
	struct blapi_wan_info_stats stats;
	int procnetdev_vsn = 0, i = 0;
	size_t len = 0;
	
	fp = fopen("/proc/net/dev", "r");
	if ( !fp ) 
		return -1;
	
	if(-1 != getline(&buf, &len, fp) && -1 != getline(&buf, &len, fp))
	{
		procnetdev_vsn = procnetdev_version(buf);
	}
	while ( -1 != getline(&buf, &len, fp) ) 
	{
		memset(name, 0, sizeof(name));
		s = get_ifname(name, buf);
		
		if ( NULL == strstr(name, "nas"))
		{
			continue;
		}

		if ( 0 == strcmp("nas0", name) ) 
		{
			get_dev_info(s, &stats, procnetdev_vsn);

			DATMLinkStats->BytesSent = stats.tx_bytes; 
			DATMLinkStats->BytesReceived = stats.rx_bytes;
			DATMLinkStats->PacketsSent = stats.tx_packets;
			DATMLinkStats->PacketsReceived = stats.rx_packets;
			DATMLinkStats->ErrorsSent = stats.tx_errors;
			DATMLinkStats->ErrorsReceived = stats.rx_errors;
			DATMLinkStats->UnicastPacketsSent = 0;
			DATMLinkStats->UnicastPacketsReceived = 0; 
			DATMLinkStats->DiscardPacketsSent = stats.tx_dropped;
			DATMLinkStats->DiscardPacketsReceived = stats.rx_dropped;
			DATMLinkStats->MulticastPacketsSent = 0;
			DATMLinkStats->MulticastPacketsReceived = 0;
			DATMLinkStats->BroadcastPacketsSent = 0;
			DATMLinkStats->BroadcastPacketsReceived = 0;
			DATMLinkStats->UnknownProtoPacketsReceived = 0;
			DATMLinkStats->TransmittedBlocks = 0;
			DATMLinkStats->ReceivedBlocks = 0;
			DATMLinkStats->CRCErrors = 0;
			DATMLinkStats->HECErrors = 0;

			break;
		}
	}
	
	fclose(fp);
	if(buf)
		free(buf);
	return 0;
}

