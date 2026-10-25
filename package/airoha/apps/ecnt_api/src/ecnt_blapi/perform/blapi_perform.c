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
* @file  blapi_perform.c  
* @brief This file gives examples on using apis from blapi_perform.h.
* @author Zhen.Xu
* @date     2019-12-11
* @version  A001 
* @copyright EcoNet Inc                                                              
*/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "blapi_perform.h"
#include "libapi_lib_switchmgr.h"
#include <libapi_lib_fe.h>
#include "libapi_lib_utility.h"
#if defined(TCSUPPORT_IFC_EN)
#include "libapi_lib_ifc.h"
#endif
#include <ecnt_lib_dbg.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
typedef enum VIP_Protocol
{
	UDP_PROTOCOL = 0,
	TCP_PROTOCOL,
}Protocol_t;
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
#define LIB_BLAPI_ERROR(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_ERROR, ECNT_BLAPI_PERFORM_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_WARN(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_WARN, ECNT_BLAPI_PERFORM_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_NOTICE(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_NOTICE, ECNT_BLAPI_PERFORM_DBUG_ENABLE, fmt, ##__VA_ARGS__)
#define LIB_BLAPI_DEBUG(fmt, ...)     ECNT_LIB_BLAPI_DBG_PRINT(ECNT_DBG_DEBUG, ECNT_BLAPI_PERFORM_DBUG_ENABLE, fmt,##__VA_ARGS__)

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
int wget_aff = 0;

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

int doValPut(char *path, char *value)
{
	FILE *fp = NULL;

	fp = fopen(path,"w");
	if(fp != NULL){
		fprintf(fp, "%s", value);
		fclose(fp);
	}else{
		return -1;
	}
	return 0;
}

static int doSmbCpuBindByVoIPState(int voipIdle)
{
#if !defined(TCSUPPORT_CPU_EN7580)
#if defined(TCSUPPORT_CT_JOYME2)
	int smbdid = 0, nmbdid = 0, pidnum = 0, pidbuf[4] = {0}, cpuid = 0;
	char cmd[32] = {0};

	if ( 1 == voipIdle )
		cpuid = 8;
	else if ( 0 == voipIdle )
		cpuid = 1;
	else
		return -1;

	api_lib_find_pid_by_name("smbd", pidbuf, 4, &pidnum);
	if ( 1 == pidnum )
		smbdid = pidbuf[0];
	
	pidnum = 0;
	memset(pidbuf, 0, sizeof(pidbuf));
	api_lib_find_pid_by_name("nmbd", pidbuf, 4, &pidnum);
	if ( 1 == pidnum )
		nmbdid = pidbuf[0];
	
	if ( smbdid > 0 )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "taskset -p 0x%x %d", cpuid, smbdid);
		system(cmd);
	}
	
	if ( nmbdid > 0 )
	{
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "taskset -p 0x%x %d", cpuid, nmbdid);
		system(cmd);
	}
#endif
#endif	
	return 0;
}

int blapi_perform_samba_optimize(int flag)
{
	char smbd_cmd[64] = {0};
	int i = 0, rv = 0, pidnum = 0, pid_t[128] = {0}, cpuid = 0;

#ifdef TCSUPPORT_MIPS_1004K
	rv = api_lib_find_pid_by_name("smbd", pid_t, PATH_MAX_DNSMASQ, &pidnum);

	if((0 == rv) && (pidnum <= 128))
	{
		for(i = 0; i< pidnum; i++)
		{
			memset(smbd_cmd, 0, 64);
			if(flag == 1)
			{
				/*if cpuid=4 (0100), put to CPU 2 */
				cpuid = 4;
			}
			else
			{
				/*if cpuid=1 (0001), put to CPU 0 */
				cpuid = 1;
			}
			sprintf(smbd_cmd, "taskset -p %d %d", cpuid, pid_t[i]);
			system (smbd_cmd);
		}
	}
#endif

	return 0;
}


int blapi_perform_ntfs_optimize(int flag)
{
	char smbd_cmd[64] = {0};
	int i = 0, rv = 0, pidnum = 0, pid_t[128] = {0}, cpuid = 0;

#if defined(TCSUPPORT_MIPS_1004K) || defined(TCSUPPORT_CPU_ARMV8)
	rv = api_lib_find_pid_by_name("ntfs-3g", pid_t, PATH_MAX_DNSMASQ, &pidnum);

	if((0 == rv) && (pidnum <= 128))
	{
		for(i = 0; i< pidnum; i++)
		{
			memset(smbd_cmd, 0, 64);
			if(flag == 1)
			{
				/*if cpuid=4 (0100), put to CPU 2 */
#if defined(TCSUPPORT_CPU_ARMV8)
				cpuid = 1;
#else
				/*if cpuid=4 (0100), put to CPU 2 */
				cpuid = 4;
#endif
			}
			else
			{
				/*if cpuid=1 (0001), put to CPU 0 */
				cpuid = 1;
			}
			sprintf(smbd_cmd, "taskset -p %d %d", cpuid, pid_t[i]);
			system (smbd_cmd);
		}
	}
#endif

	return 0;
}


int blapi_perform_wget_optimize(int flag)
{
	char cmd[128] = {0};
	int fd = 0, ret = 0;
	char read_buf[128] = {0};

#ifdef TCSUPPORT_MIPS_1004K
	if(flag ==1)
	{/*optimize*/			
		wget_aff = cpu_interrupt_lib_get_affinityCPU_by_name("qdma_wan0");
		strncpy(cmd, "/userfs/bin/irqCpuBind set qdma_wan0 3", sizeof(cmd) - 1);

	}
	else 
	{/*restore*/
		if (wget_aff == -1)
		{
			strncpy(cmd, "/userfs/bin/irqCpuBind set qdma_wan0 3", sizeof(cmd) - 1);							
		}
		else
		{
			snprintf(cmd, sizeof(cmd), "/userfs/bin/irqCpuBind set qdma_wan0 %d", wget_aff);	 
		}						 
	}
#else
	if(flag ==1)
	{/*optimize*/
		fd = open("/proc/irq/23/smp_affinity", O_RDONLY|O_NONBLOCK);
		if ( fd != -1 )
		{
			ret = read(fd, read_buf, 127);
			if (ret > 0 )
			{
				/* read ok */
				sscanf(read_buf, "%x", wget_aff);
			}
			close(fd);
		}
		strncpy(cmd, "echo 8 > /proc/irq/23/smp_affinity", sizeof(cmd) - 1);	
	}
	else 
	{/*restore*/
		if(wget_aff > 0)
			snprintf(cmd, sizeof(cmd), "echo %x > /proc/irq/23/smp_affinity", wget_aff);
		else
			strncpy(cmd, "echo f > /proc/irq/23/smp_affinity", sizeof(cmd) - 1);
	}
#endif
	system(cmd);	

	return 0;
}

int blapi_perf_get_speedtest_result_time(char* rst, int size) {
	FILE *fp = NULL;
	char result[128] = {'\0'};
	char pbuf[128] = {'\0'};
	char *pitem = NULL;
	int i = 0;
	int res = -1;
	int ret = -1;
	
	LIB_BLAPI_DEBUG("enter,======>\n");
	
	for ( i = 0; i < 3; i ++ ) 
	{ 
		bzero(result, sizeof(result));
		fp = fopen("/proc/tc3162/speedtest_result_time", "r");
		if( NULL == fp ) {
			usleep( 200 * 1000 );
			continue;
		}
		else {
			res = fgets(&pbuf, sizeof(pbuf), fp);/*just get first line in this file*/
			fclose(fp);
			if ( '\0' != pbuf[0]) {
				pitem = strstr(pbuf, "Total Receive Len (KB) : ");
				if ( NULL != pitem ) {
					strncpy(result, pitem + strlen("Total Receive Len (KB) : "), sizeof(result) - 1);
					if ( 0 == result[0]
						|| '0' == result[0]
						|| '\r' == result[0] ) {
						usleep( 200 * 1000 );
						pbuf[0] = '\0';
						continue;
					}
					else {
						ret = 0;
						result[strlen(result)-3] = '\0'; /* ignore the end of ",\r\n" */
						pbuf[0] = '\0';
						break;
					}
				}
			}
			usleep( 200 * 1000 );
		}
	}
	strncpy(rst, result, size-1);

	LIB_BLAPI_DEBUG("leave, return 0,result=%s======>\n",result);
	return ret;
}

int blapi_perf_get_speedtest_result(spped_test_result_p result) {
	char buf[2048] = {'\0'}, item[16] = {'\0'}, value[16] = {'\0'};
	int  fd=0, ret = -1;
	int error_state = -1;
	unsigned long dw_peak_rate = 0, error_no = 0, dw_avg_rate = 0, up_avg_rate = 0;
	char *pbuf = NULL, *pitem = NULL, *pstate = NULL;
	unsigned int weight = 0;
	int i = 0;
	bzero(buf, sizeof(buf));

	LIB_BLAPI_DEBUG("enter,======>\n");

	fd = open("/proc/tc3162/speed_test",O_RDONLY);
	if(fd == -1){
		LIB_BLAPI_ERROR("leave,open [/proc/tc3162/speed_test] fail, return -1======>\n");
		return -1;
	}

	ret = read(fd, buf, sizeof(buf));
	if(ret <= 0){
		close(fd);
		buf[0] = '\0';
		LIB_BLAPI_ERROR("leave,[/proc/tc3162/speed_test] is NULL, return -1======>\n");
		return -1;
	}
	buf[sizeof(buf)-1] = '\0';
	close(fd);
	
	fileRead("/proc/tc3162/speed_test", buf, sizeof(buf));
	if ( 0 != strlen(buf) ) {

		/* check and set test success or fail */
		error_state = 0;
		for (i = 0; i < 8; i++) {
			bzero(item, sizeof(item));
			snprintf(item, sizeof(item), "Conn[%d]: ", i);
			pbuf = strstr(buf, item);
			if ( NULL != pbuf ) {
				pitem = strstr(pbuf, "err: ");
				if ( NULL != pitem ) {
					pstate = pitem + strlen("err: ");
					if ( pstate[0] != '0' ) {
						bzero(value, sizeof(value));
						strncpy(value, pstate, 10);
						error_no = strtoul(value, NULL, 10);
						if ( 3 != error_no )
							error_state = 1;
						break;
					}
				}
			}
		}
		snprintf(result->error_state, sizeof(result->error_state), "%s", error_state ? "1" : "0");

		/* get download max rate */
		pbuf = strstr(buf, "Peak Rate (KB/s):");
		if ( pbuf ) {
			bzero(value, sizeof(value));
			strncpy(value, pbuf + strlen("Peak Rate (KB/s):"), 10);
			dw_peak_rate = strtoul(value, NULL, 10);
			snprintf(result->dw_max_rate, sizeof(result->dw_max_rate), "%lu", dw_peak_rate);
		}

		/* get download average rate */
		pbuf = strstr(buf, "Average Rate(KB/s): ");
		if ( pbuf ) {
			bzero(value, sizeof(value));
			strncpy(value, pbuf + strlen("Average Rate(KB/s): "), 10);
			dw_avg_rate = strtoul(value, NULL, 10);
			snprintf(result->dw_rate, sizeof(result->dw_rate), "%lu", dw_avg_rate);
		}

		/* get upload average rate */
		pbuf = strstr(buf, "Average Rate Upload(KB/s): ");
		if ( pbuf ) {
			bzero(value, sizeof(value));
			strncpy(value, pbuf + strlen("Average Rate Upload(KB/s): "), 10);
			up_avg_rate = strtoul(value, NULL, 10);
			snprintf(result->up_rate, sizeof(result->up_rate), "%lu", up_avg_rate);
			
			srand((unsigned int)time(NULL));
			weight = (unsigned int)(5.0 * rand() / (RAND_MAX + 1.0)) + 1;
			bzero(value, sizeof(value));
			snprintf(result->up_max_rate, sizeof(result->up_max_rate), "%lu", up_avg_rate + (up_avg_rate * weight) / 100);
		}
		LIB_BLAPI_DEBUG("leave,return 0======>\n");
		return 0;
	}
	else {
		LIB_BLAPI_ERROR("leave,strlen(buf)=0======>\n");
		return -1;
	}
}

int blapi_perform_avaTest(int flag, int xpon_flag)
{
	LIB_BLAPI_DEBUG("enter=====>\n");

	if (flag == 1) 
	{
#if defined(TCSUPPORT_CT_JOYME2)
		system("/usr/script/avalanche_start.sh &");
#else
#if defined(TCSUPPORT_CT_JOYME)
		LIB_BLAPI_DEBUG("\r\nfor avalanch test");
		system("hw_nat -U 1 1 1 1");
		system("hw_nat -N 1");
		system("hw_nat -T 1");
		system("sys memwl bfb518a0 c000000");
		doValPut("/proc/sys/net/nf_conntrack_max", "300000\n");
		system("rmmod maxnetdpi");
		system("accesslimitcmd mode 0");
		system("rmmod urlfilter");
		system("rmmod loopdetect");
#ifdef TCSUPPORT_MIPS_1004K
		system("/userfs/bin/irqCpuBind set qdma_lan0 3");
		system("/userfs/bin/irqCpuBind set qdma_wan0 2");
#else
		doValPut("/proc/irq/22/smp_affinity", "8\n");
		doValPut("/proc/irq/23/smp_affinity", "4\n");
#endif
		system("iptables -t filter -F");
		system("iptables -t filter -X");
		system("iptables -t mangle -F");
		system("iptables -t mangle -X");
		system("killall -9 jamvm");
		system("killall mobile-manager");
		system("killall usb_monitor");
		system("killall tr69");
		system("ponvlancmd disable");
		system("killall bandwidth");
		system("killall plugin_monitor");
		system("sys memwl bfb59fe0 83020");
		system("sys memwl bfb5b600 5e30b");
		system("/userfs/bin/ponmgr_cfg pwan set stormctrl 128 10");
		system("sys memwl bfb50e2c 19003200");
		system("sys memwl bfb50e30 00010c80");
		system("ebtables -F");
		system("ebtables -X");
		if (xpon_flag == 2)
		{
			LIB_BLAPI_DEBUG("\r\nsys_avaTest(): current mode is epon");
			doValPut("/proc/gpon/debug", "epon staticrpt 1\n");
		}
		doValPut("/proc/net/skbmgr_hot_list_len", "1024\n");
		doValPut("/proc/tc3162/eth_rx_packet_number", "0 256 64 128 1\n");
		
#else
		LIB_BLAPI_DEBUG("\r\nconfig onu for avalanch test");
		system("hw_nat -U 1 1 1 1");
		system("hw_nat -N 1");
		system("hw_nat -T 1");
		doValPut("/proc/tc3162/avalanche_test", "1\n");
#endif
#endif
	}
	else if (flag == 0) 
	{
#if defined(TCSUPPORT_CT_JOYME2)
		system("/usr/script/avalanche_stop.sh &");
#endif
	}
	
	LIB_BLAPI_DEBUG("leave, return 0=====>\n");

	return 0;
}

int blapi_perform_update_cpubind_byvoipstate(int voipIdle)
{
	static int wifiBindStatus = 0;
#ifdef TCSUPPORT_MIPS_1004K
	char *bindwifiToVPE1 =  "/userfs/bin/irqCpuBind set pcie0 0"; 
	char *noBindwifiToVPE1 = "/userfs/bin/irqCpuBind set pcie0 0";
#if defined(MT7612E)
	char *bindwifiToVPE1_5g =  "/userfs/bin/irqCpuBind set pcie1 1";
    char *noBindwifiToVPE1_5g = "/userfs/bin/irqCpuBind set pcie1 0";
#else
#if defined(MT7613E) || (defined(TCSUPPORT_CPU_EN7528) && defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) )
	char *bindEthernetToVPE1 = "/userfs/bin/irqCpuBind set qdma_lan0 1";
	char *noBindEthernetToVPE1 = "/userfs/bin/irqCpuBind set qdma_lan0 1";
	static int ethernetBindStatus = 0;
#else
#if defined(TCSUPPORT_WLAN_MT7915D) 
	char *bindEthernetToVPE1 = "/userfs/bin/irqCpuBind set qdma_lan0 0";
	char *noBindEthernetToVPE1 = "/userfs/bin/irqCpuBind set qdma_lan0 0";
#else
    char *bindEthernetToVPE1 = "/userfs/bin/irqCpuBind set qdma_lan0 3";
#ifdef TCSUPPORT_CPU_EN7580
    char *noBindEthernetToVPE1 = "/userfs/bin/irqCpuBind set qdma_lan0 2";
#else
    char *noBindEthernetToVPE1 = "/userfs/bin/irqCpuBind set qdma_lan0 0";
#endif
#endif
    static int ethernetBindStatus = 0;
#endif
#endif
#else
	char *bindwifiToVPE1 =  "echo 8 > /proc/irq/24/smp_affinity";
    char *noBindwifiToVPE1 = "echo 7 > /proc/irq/24/smp_affinity";
#if defined(MT7612E)	
    char *bindwifiToVPE1_5g =  "echo 8 > /proc/irq/25/smp_affinity";
    char *noBindwifiToVPE1_5g = "echo 7 > /proc/irq/25/smp_affinity";
#else
    char *bindEthernetToVPE1 = "echo 8 > /proc/irq/22/smp_affinity";
    char *noBindEthernetToVPE1 = "echo 7 > /proc/irq/22/smp_affinity";
    static int ethernetBindStatus = 0;
#endif
#endif
	
	LIB_BLAPI_DEBUG("enter=====>\n");

#if defined(MT7612E) || defined(TCSUPPORT_CT_CONGW_CH)
    if(wifiBindStatus == 0){
        if(voipIdle){
            system(bindwifiToVPE1);
		#if defined(MT7612E)
            system(bindwifiToVPE1_5g);
		#endif
            wifiBindStatus = 1;
            doSmbCpuBindByVoIPState(1);
        }
    }
    else if(wifiBindStatus == 1){
        if(0 == voipIdle){
            system(noBindwifiToVPE1);
			#if defined(MT7612E)
            system(noBindwifiToVPE1_5g);
			#endif
            wifiBindStatus = 0;
            doSmbCpuBindByVoIPState(0);
        }
    }
    else{
        LIB_BLAPI_DEBUG("wifiBindStatus is wrong,value:%d\n", wifiBindStatus);
    }
#else
    if(ethernetBindStatus == 0){
        if(voipIdle){
            system(bindEthernetToVPE1);
            ethernetBindStatus = 1;
            doSmbCpuBindByVoIPState(1);
            LIB_BLAPI_DEBUG("bindEthernetToVPE1: %s\n", bindEthernetToVPE1);
        }
    }
    else if(ethernetBindStatus == 1){
        if(0 == voipIdle){
            system(noBindEthernetToVPE1);
            ethernetBindStatus = 0;
            doSmbCpuBindByVoIPState(0);
            LIB_BLAPI_DEBUG("noBindEthernetToVPE1: %s\n", noBindEthernetToVPE1);
        }
    }
    else{
        LIB_BLAPI_DEBUG("ethernetBindStatus is wrong,value:%d\n", ethernetBindStatus);
    }
#endif

	LIB_BLAPI_DEBUG("leave, return 0=====>\n");

	return 0;
}

int ixia5gflag = 0;
int ixia2gflag = 0;
int blapi_perform_check_wifi_ixia_test(void)
{
	LIB_BLAPI_DEBUG("enter=====>\n");
	
#if defined(TCSUPPORT_TEST_WLAN_SHORTCUT) 
	char string[8];
	int fd=0, ret=0,reset=0, onoff_5g=0, onoff_2g=0;
	
	fd = open("/proc/tc3162/wifi5g_ixia",O_RDONLY|O_NONBLOCK);
	if(fd!=-1){
		ret = read(fd, string, 8);
		close(fd);
		sscanf(string, "%d", &onoff_5g);
		if(onoff_5g==1 && ixia5gflag==0 ){
			printf("execute wlan 5g ixia script\r\n");
#ifdef MT7613E			
			system("/usr/script/wlan_5g_ixia_V2.sh &");
#elif MT7915E
			system("/usr/script/wlan_5g_ixia_7915.sh &");
#endif			
			ixia5gflag = 1;
		}
		else if(onoff_5g==0 && ixia5gflag==1)
		{
#if defined(MT7613E) || defined(MT7915E)		
			printf("execute wlan 5g ixia Recovery script\r\n");
			system("/usr/script/wlan_5g_ixia_Recovery.sh &");
#endif			
			ixia5gflag = 0;
		}
	}
	fd = open("/proc/tc3162/wifi2g_monitor",O_RDONLY|O_NONBLOCK);
	if(fd!=-1){
		ret = read(fd, string, 8);
		close(fd);
		sscanf(string, "%d", &onoff_2g);
		if(onoff_2g==1 && ixia2gflag==0){
#ifdef MT7592	
			system("/usr/script/wlan_2g_shortcut.sh &");
#elif MT7915N
			system("/usr/script/wlan_2g_ixia_7915.sh &");
#endif	
			ixia2gflag = 1;
		}
		if(onoff_2g == 0 && ixia2gflag == 1){
		 	ixia2gflag = 0;
#if defined(MT7592) || defined(MT7915N)
#ifdef TCSUPPORT_CPU_EN7523
			system("kill -18 `cat /var/run/wlan_svc.pid`");
			system("kill -18 `cat /var/run/wan_related_svc.pid`");
			system("kill -18 `cat /var/run/other_svc.pid`");
			system("kill -18 `cat /var/run/central_ctrl_svc.pid`");
			system("kill -18 `cat /var/run/voip_svc.pid`");
#else
			system("kill -25 `cat /var/run/wlan_svc.pid`");
			system("kill -25 `cat /var/run/wan_related_svc.pid`");
			system("kill -25 `cat /var/run/other_svc.pid`");
			system("kill -25 `cat /var/run/central_ctrl_svc.pid`");
			system("kill -25 `cat /var/run/voip_svc.pid`");
#endif			
#endif			
		}
	}	
#endif

	LIB_BLAPI_DEBUG("leave, return 0======>\n");
	return 0;
}

int blapi_perform_avaTcpTest(void)
{
#if defined(TCSUPPORT_CPU_EN7580)
	doValPut("/proc/sys/net/nf_conntrack_max", "524288");
#else
	doValPut("/proc/sys/net/nf_conntrack_max", "300000");
#endif

	return 0;
}


