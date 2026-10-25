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
#include <stdarg.h>
#include <string.h>  
#include <unistd.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/file.h>
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#include <errno.h>
#endif
#include "wifi_comm.h"
#include "wifi_file_comm.h"
#include "wifi_dbg.h"
#include "libapi_lib_wifimgr.h"

#define SHMKEY_2_4G 	0x12345
#define SHMKEY_5G 		0x12346

#define SEMKEY_2_4G 	0x12348
#define SEMKEY_5G 		0x12349

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#define SHMKEY_GLOBAL 		0x12347
#define SEMKEY_GLOBAL 		0x12350
#define SHM_GLOBAL_ADDR		0x20000000
#endif

char dat_path_2_4g[MAX_PATH_LEN] = "/etc/Wireless/RT2860AP/RT2860AP.dat";
char dat_path_5g[MAX_PATH_LEN]   = "/etc/Wireless/RT2860AP_AC/RT2860AP.dat";

static int shmid_2_4g = -1;
static int shmid_5g   = -1;

static int num_2_4g = 0;
static int num_5g = 0;

int semid_2_4g =- 1;
int semid_5g   =- 1;

ATTR_NODE * shm_addr_2_4g = NULL; 
ATTR_NODE * shm_addr_5g = NULL; 
//common function
int doCmdWrite(char *path, char *value)
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

void
tcdbg_blapi_printf(char *fmt,...){

	FILE *proc_file;
	char msg[256];
	va_list args;

	va_start(args, fmt);
	vsnprintf(msg, 256, fmt, args);	

    proc_file = fopen("/proc/tc3162/dbg_msg", "w");
	if (!proc_file) {
		printf("open /proc/tc3162/dbg_msg fail\n");
		va_end(args);
		return;
	}
	fprintf(proc_file, "%s", msg);
	fclose(proc_file);
	va_end(args);
}

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
PARA_NODE * share_global = NULL; 
unsigned int chipflag2g = 0;
unsigned int chipflag5g = 0;
//2.4g flag
unsigned char WLAN_2G_CHIPYPTE[WLAN_2G_MAX_ID] = {0};
unsigned int current2gindex = 0;;
//5g variable 
unsigned char WLAN_5G_CHIPYPTE[WLAN_5G_MAX_ID] = {0};
unsigned int current5gindex = 0;;
#endif

char* special_para_one[SPECIAL_PARA_NUM] = 
	{"SSID","RADIUS_Key","WscDefaultSSID","WPAPSK","Key1Str","Key2Str",
	"Key3Str","Key4Str","AccessPolicy","AccessControlList","WSC_UUID_E","WSC_UUID_Str"};
char* special_para_two[SPECIAL_PARA_NUM_COMMON] =
	{"SSID","RADIUS_Key","WscDefaultSSID","WPAPSK","Key1Str","Key2Str",
	"Key3Str","Key4Str","WSC_UUID_E","WSC_UUID_Str"};
char* special_para_three[SPECIAL_PARA_NUM_EXTENED] = 
	{"AccessPolicy","AccessControlList"};
char *special_para_four[SPECIAL_PARA_COUNT] = 
	{"APAifsn","APCwmin","APCwmax","APTxop","APACM","BSSAifsn",
	"BSSCwmin","BSSCwmax","BSSTxop","BSSACM","AckPolicy",
	"WdsList","WdsEncrypType","WdsPhyMode"}; 

int cfg_in_shm_count = 0;

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
int 
check_chip_combine(CHIP_INFO_NODE *band0, CHIP_INFO_NODE *band1)
{
	FILE *fd = NULL;
	unsigned int band0idx = 0;
	unsigned int band1idx = 0;
	unsigned int readband0idx = 0;
	unsigned int readband1idx = 0;
	int match = 0;
	char linebuffer[1024] = {0};
	int i = 0;
	int tempret = 0;
	int combination = 0;
	typedef enum _chip_id{
		CHIP_7592 = 0,
		CHIP_7613,
		CHIP_7615,
		CHIP_7915,
		CHIP_7916,
		CHIP_7906,
		CHIP_790A,
	}chip_id;

	wifi_get_driver_info driver[] = 
	{
		{"7603",	0},
		{"7663",	0},
		{"7615",	0},
		{"7915",	0},
		{"7916",	0},
		{"7906",	0},
		{"790A",	0},
		{"",		0},
	};

	system("cat /proc/bus/pci/devices > /tmp/devices");
	fd = fopen("/tmp/devices", READ_MODE);  
    if(fd) 
    {
		while(fgets(linebuffer, 1024, fd) > 0)
		{
			i = 0;
			while(0 != strlen(driver[i].chip_name))
			{
				if(NULL != strstr(linebuffer, driver[i].chip_name))
				{
					driver[i].count++;
				}
				i++;
			}
		}

		i = 0;
		
		if(driver[CHIP_7592].count && driver[CHIP_7613].count)
		{
			readband0idx = WLAN_2G_7592_ID;
			readband1idx = WLAN_5G_7613E_ID;	
			combination = 1;
		}
		else if(driver[CHIP_7615].count == 2)
		{
			readband0idx = WLAN_2G_7615N_ID;
			readband1idx = WLAN_5G_7615E_ID;	
			combination = 2;
		}
		else if(driver[CHIP_7915].count == 1 /*|| (driver[CHIP_7915].count == 1 && driver[CHIP_7916].count == 1)*/)
		{
			readband0idx = WLAN_2G_7915D_ID;
			readband1idx = WLAN_5G_7915D_ID;	
			combination = 3;
		}
		else if((driver[CHIP_7915].count == 2))
		{
			readband0idx = WLAN_2G_7915N_ID;
			readband1idx = WLAN_5G_7915E_ID;
			combination = 4;
		}
		else if(driver[CHIP_7906].count == 1 /*|| (driver[CHIP_7906].count == 1 && driver[CHIP_790A].count == 1)*/)
		{
			readband0idx = WLAN_2G_7916D_ID;
			readband1idx = WLAN_5G_7916D_ID;	
			combination = 5;
		}

		band0->id = 1 << readband0idx;
		band1->id = 1 << readband1idx;

		//below only for show in console
		switch(combination)
		{
			case 1:
				tcdbg_blapi_printf("airoha pre choise:band0 chip=7592,band1 chip=7613\n");
			break;
			case 2:
				tcdbg_blapi_printf("airoha pre choise:band0 chip=7615N,band1 chip=7615E\n");
			break;
			case 3:
				tcdbg_blapi_printf("airoha pre choise:band0 chip=7915D,band1 chip=7915D\n");
			break;
			case 4:
				tcdbg_blapi_printf("airoha pre choise:band0 chip=7915N,band1 chip=7915E\n");
			break;
			case 5:
				tcdbg_blapi_printf("airoha pre choise:band0 chip=7916D,band1 chip=7916D\n");
			break;	
			default:
				tcdbg_blapi_printf("airoha pre choise:nowifi\n");
		}

		fclose(fd);
    }
	
	RETURN_WIFIMGR_SUCCESS;
}

int init_global_shm(void)
{
	int shmid,semid;
	int first = 0;
	union semun sem_un;
	PARA_NODE * share_local = NULL; 
	int ret = WLAN_LIBAPI_SUCCESS;
	
	if(share_global && share_global->isconfig)
	{
		goto INIT_DONE;
	}
	
	//semphore
	semid = semget(SEMKEY_GLOBAL,0,0666);
	if (semid < 0)
	{
		semid = semget(SEMKEY_GLOBAL,1,0666 | IPC_CREAT);
		memset(&sem_un,0,sizeof(sem_un));
		sem_un.val = 1;
		if (semid < 0 || semctl(semid,0,SETVAL,sem_un) < 0) 
			ret = WLAN_LIBAPI_CREATE_SHARED_FAIL;
			WIFIMGR_FAIL_MSG("init_global_shm semid failed.\n");
	}

	//share memory
	shmid = shmget(SHMKEY_GLOBAL,sizeof(PARA_NODE),0666);
		
	if (shmid < 0)
	{
		shmid = shmget(SHMKEY_GLOBAL,sizeof(PARA_NODE),0666 | IPC_CREAT);
		first = 1;
	}

	if (shmid < 0 || semid < 0)
	{
		WIFIMGR_FAIL_MSG("init_global_shm shmid failed.\n");
		goto INIT_DONE;
	}

	share_local = (struct PARA_NODE* )shmat(shmid,NULL,0);
	if (NULL == share_local){
        WIFIMGR_FAIL_MSG("Attach share memory failed.\n");
		goto INIT_DONE;
    }

	if (first)
	{
		memset(share_local, 0, sizeof(PARA_NODE));
		share_local->shmid = shmid;
		share_local->semid = semid;
	}
	
	share_local->user++;
	share_global = share_local;

	//do something at first
	ret = init_chip_operation();

INIT_DONE:
	return ret;
}

#if 0
void deinit_global_shm(void)
{
	int shmid,semid;
	int res = 0;

	WIFIMGR_FAIL_MSG("shm_lib_deinit   \n");	
	
	if (!share_global)
		return;

	shmid = share_global->shmid;
	semid = share_global->semid;
	share_global->user--;
	
	if (share_global->user <= 0)
	{
		WIFIMGR_FAIL_MSG("deinit_global_shm : remove share mem  \n");	
		res = shmctl(shmid,IPC_RMID,NULL);
		res = semctl(semid,0,IPC_RMID,0);
		share_global = NULL;
	}
	return;
}
#endif

int write_shm_chip_para(void)
{
	PARA_NODE globalpara;
	char op_buf[32] = {0};
	int i;
	int ret = 0,tempret = 0;
	
	/*
		need to do more
		1.get chip configuration from prolinecmd
		2.set the running chip info to kernel
		3.set the sharedmemory
		4.set global variables
	*/	
	//below for end loop
	wifi_shm_lock();
	share_global->isconfig = SHAREDCONFIGING;
	wifi_shm_unlock();
	
	memset(&globalpara, 0, sizeof(globalpara));
	globalpara.chippara[LIB_BAND0].id = 1 << WLAN_2G_NOCHIP_ID;
	globalpara.chippara[LIB_BAND1].id = 1 << WLAN_5G_NOCHIP_ID;
	//only for test proc file can be used or not!
	if(doCmdWrite(WIFI_MULLTI_CHIP_PATH,op_buf) < 0)
	{
		ret = WLAN_LIBAPI_WRITE_INFO_FAIL;
		goto configfail;
	}

	check_chip_combine(&(globalpara.chippara[LIB_BAND0]),&(globalpara.chippara[LIB_BAND1]));

	//step2
	/*
		set global variables for kernel use
	*/
	sprintf(op_buf,"%x %x",globalpara.chippara[LIB_BAND0].id,globalpara.chippara[LIB_BAND1].id);
	if(doCmdWrite(WIFI_MULLTI_CHIP_PATH,op_buf) < 0)
	{
		ret = WLAN_LIBAPI_WRITE_INFO_FAIL;
		goto configfail;
	}

	//step3
	wifi_shm_lock();
	share_global->isconfig = SHAREDCONFIG;
	share_global->chippara[LIB_BAND0].id = globalpara.chippara[LIB_BAND0].id;
	share_global->chippara[LIB_BAND1].id = globalpara.chippara[LIB_BAND1].id;

	//step4
	/*
		set global variables for lib use
	*/
	memset(WLAN_2G_CHIPYPTE,0,WLAN_2G_MAX_ID);
	memset(WLAN_5G_CHIPYPTE,0,WLAN_5G_MAX_ID);

	wifi_shm_unlock();
	return WLAN_LIBAPI_SUCCESS;

configfail:
	wifi_shm_lock();
	share_global->isconfig = SHAREDNOTCONFIG;
	wifi_shm_unlock();
	return ret;
		
}

void init_global_val()
{
	int i;
	wifi_shm_lock();
	i = WLAN_2G_MAX_ID;
	if(share_global->chippara[LIB_BAND0].id)
	{
		chipflag2g = share_global->chippara[LIB_BAND0].id;
		for(i = WLAN_2G_NOCHIP_ID;i < WLAN_2G_MAX_ID;i++)
		{
			if(chipflag2g & BIT(i))
			{
				WLAN_2G_CHIPYPTE[i] = 1;
				current2gindex = i;
				break;
			}		
		}
		
		if(i == WLAN_2G_MAX_ID)
			WLAN_2G_CHIPYPTE[WLAN_2G_NOCHIP_ID] = 1;
	}

	i = WLAN_5G_MAX_ID;
	if(share_global->chippara[LIB_BAND1].id)
	{
		chipflag5g = share_global->chippara[LIB_BAND1].id;
		for(i = WLAN_5G_NOCHIP_ID;i < WLAN_5G_MAX_ID;i++)
		{
			if(chipflag5g & BIT(i))
			{
				WLAN_5G_CHIPYPTE[i] = 1;
				current5gindex = i;
				break;
			}		
		}
		
		if(i == WLAN_5G_MAX_ID)
			WLAN_2G_CHIPYPTE[WLAN_5G_NOCHIP_ID] = 1;
	}
	wifi_shm_unlock();

}
int init_chip_operation(void)
{
	int ret = WLAN_LIBAPI_SUCCESS;
	int configflag = SHAREDNOTCONFIG;

	wifi_shm_lock();
	configflag = share_global->isconfig;
	wifi_shm_unlock();
	
	switch(configflag)
	{
		case SHAREDNOTCONFIG:
			ret = write_shm_chip_para();
		break;
		case SHAREDCONFIGING:
			ret = WLAN_LIBAPI_OPER_FAIL;
		break;
			
		case SHAREDCONFIG:
		default:	
			break;
	}

	if(WLAN_LIBAPI_SUCCESS == ret)
	init_global_val();
	
	return ret;
}

int wifi_shm_lock(void)
{

	struct sembuf  semb;
	unsigned char count = 3;
	int ret = 0;
	int semid = share_global->semid;

	if (semid < 0)
		return -1;
	
	semb.sem_num = 0;
	semb.sem_op = -1;
	semb.sem_flg = SEM_UNDO;
	while(1){
		ret = semop(semid, &semb, 1);
		if(ret != 0 && errno == EINTR && count){
			count--;
			usleep(1000);	
		}
		else{
			count = 3;
			break;
		}
	}
	
	return 0;
}

int wifi_shm_unlock(void)
{
	struct sembuf  semb;
	unsigned char count = 3;
	int ret= 0;
	int semid = share_global->semid;
	if (semid < 0)
		return -1;

	semb.sem_num = 0;
	semb.sem_op = 1;
	semb.sem_flg = SEM_UNDO;
	while(1){
		ret = semop(semid, &semb, 1);
		if(ret != 0 && errno == EINTR && count){
			count--;
			usleep(1000);	
		}
		else{
			count = 3;
			break;
		}
	}
	
	return 0;
}
#endif
int 
set_semvalue(int sem_id, int value)
{
	union semun sem_union;
	sem_union.val = value;
	if(-1 == semctl(sem_id, 0, SETVAL, sem_union)){
		RETURN_WIFIMGR_FAIL_MSG("set_semvalue failed.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int
del_semvalue(int sem_id)
{
#if 1
	if(-1 == semctl(sem_id, 0, IPC_RMID)){
		RETURN_WIFIMGR_FAIL_MSG("del_semvalue failed.\n");
	}
#else
	union semun sem_union;
	if(-1 == semctl(sem_id, 0, IPC_RMID, sem_union)){
		RETURN_WIFIMGR_FAIL_MSG("del_semvalue failed.\n");
	}
#endif
	RETURN_WIFIMGR_SUCCESS;
}


int 
semaphore_p(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op  = -1;
	sem_b.sem_flg = SEM_UNDO;
	if( -1 == semop(sem_id, &sem_b, 1)){
		RETURN_WIFIMGR_FAIL_MSG("seamphore_p failed.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
semaphore_v(int sem_id)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op  = 1;
	sem_b.sem_flg = SEM_UNDO;
	if( -1 == semop(sem_id, &sem_b, 1)) {
		RETURN_WIFIMGR_FAIL_MSG("seamphore_v failed.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int get_dat_para_num(wifi_type type)
{
	FILE *fp = NULL;
	char linebuffer[BUFFER_ITEM_SIZE] = {0};
	int num = 0;

	if (WIFI_2_4G == type)
	{
		fp = fopen(dat_path_2_4g, "r");
	}
	else if (WIFI_5G == type)
	{
		fp = fopen(dat_path_5g, "r");
	}

	if (NULL == fp)
	{
		return 0;
	}

	while(fgets(linebuffer, BUFFER_ITEM_SIZE, fp) > 0)
	{
		num++;
	}
	fclose(fp);

	return num + 5;
}

int 
init_shm(wifi_type type)
{
	WIFI_FUNC_TRACE_ENTER;
	int num = 0;

	if (WIFI_2_4G == type)
	{
		num = get_dat_para_num(type);
		if (num > num_2_4g)
			num_2_4g = num;

		shmid_2_4g = shmget(SHMKEY_2_4G, sizeof(ATTR_NODE) * num_2_4g, 0666);
		if (-1 == shmid_2_4g)
		{
			LIB_WIFI_DEBUG("Share memory do not exist.\n");
			shmid_2_4g= shmget(SHMKEY_2_4G, sizeof(ATTR_NODE) * num_2_4g, 0666|IPC_CREAT);
			if (-1  == shmid_2_4g)
			{
				RETURN_WIFIMGR_FAIL_MSG("Create share memory failed.\n");
			}
			shm_addr_2_4g = (ATTR_NODE *)shmat(shmid_2_4g,NULL,0);
			if (NULL == shm_addr_2_4g)
			{
           	 	RETURN_WIFIMGR_FAIL_MSG("Attach share memory failed.\n");
        	}
		}
		else
		{
			shm_addr_2_4g = (ATTR_NODE *)shmat(shmid_2_4g, NULL, 0);
        	if (NULL == shm_addr_2_4g)
			{
            	RETURN_WIFIMGR_FAIL_MSG("Attach share memory failed.\n");
        	}
		}

		semid_2_4g = semget(SEMKEY_2_4G, 1, 0666);
		if (-1 == semid_2_4g)
		{
			semid_2_4g = semget(SEMKEY_2_4G, 1, 0666|IPC_CREAT);
			if (-1 == semid_2_4g)
			{
				RETURN_WIFIMGR_FAIL_MSG("semget failed.\n");
			}
		}
		if (ECNT_WIFIMGR_FAILURE == set_semvalue(semid_2_4g, 1))
		{
			RETURN_WIFIMGR_FAIL_MSG("Set sem value failed.\n");
		}	
		
		if(ECNT_WIFIMGR_SUCCESS != semaphore_p(semid_2_4g))
		{
			RETURN_WIFIMGR_FAIL_MSG("p operation failed.\n");
		}
		
		memset((char *)shm_addr_2_4g, 0, sizeof(ATTR_NODE) * num_2_4g);

		if(ECNT_WIFIMGR_SUCCESS != semaphore_v(semid_2_4g))
		{
			RETURN_WIFIMGR_FAIL_MSG("v operation failed.\n");
		}	
	}
	else if(WIFI_5G == type)
	{
		num = get_dat_para_num(type);
		if (num > num_5g)
			num_5g = num;
	
		shmid_5g = shmget (SHMKEY_5G, sizeof(ATTR_NODE) * num_5g, 0666);
		if(-1 == shmid_5g)
		{
			LIB_WIFI_DEBUG("Share memory do not exist.\n");
			shmid_5g = shmget(SHMKEY_5G, sizeof(ATTR_NODE) * num_5g, 0666|IPC_CREAT);
			if(-1  == shmid_5g)
			{
				RETURN_WIFIMGR_FAIL_MSG("Create share memory failed.\n");
			}
			shm_addr_5g = (ATTR_NODE *)shmat(shmid_5g,NULL,0);
			if (NULL == shm_addr_5g)
			{
           	 	RETURN_WIFIMGR_FAIL_MSG("Attach share memory failed.\n");
        	}
		}
		else
		{
			shm_addr_5g = (ATTR_NODE *)shmat(shmid_5g, NULL, 0);
        
        	if (NULL == shm_addr_5g)
			{
            	RETURN_WIFIMGR_FAIL_MSG("Attach share memory failed.\n");
        	}
		}
		
		semid_5g = semget(SEMKEY_5G, 1, 0666);
		if(-1 == semid_5g)
		{
			semid_5g = semget(SEMKEY_5G, 1, 0666|IPC_CREAT);
			if(-1 == semid_5g)
			{
				RETURN_WIFIMGR_FAIL_MSG("semget failed.\n");
			}
		}
		if(ECNT_WIFIMGR_FAILURE == set_semvalue(semid_5g, 1))
		{
			//free share memmory
			RETURN_WIFIMGR_FAIL_MSG("Set sem value failed.\n");
		}	
		
		if(ECNT_WIFIMGR_SUCCESS != semaphore_p(semid_5g))
		{
			RETURN_WIFIMGR_FAIL_MSG("p operation failed.\n");
		}
		memset((char *)shm_addr_5g, 0, sizeof(ATTR_NODE) * num_5g);
		if(ECNT_WIFIMGR_SUCCESS != semaphore_v(semid_5g))
		{
			RETURN_WIFIMGR_FAIL_MSG("v operation failed.\n");
		}		
	}
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	
	RETURN_WIFIMGR_SUCCESS;
}

#if 0
int 
free_shm(void)
{
	WIFI_FUNC_TRACE_ENTER;
	if(-1 !=shmid_2_4g)
	{
		del_semvalue(semid_2_4g);
		semid_2_4g = -1;	
		if(0 != shmctl(shmid_2_4g,IPC_RMID,0))
		{	
				shmid_2_4g = -1;
				RETURN_WIFIMGR_FAIL_MSG("Free share memory for 2.4G data failed.\n");
		}			
		else
			shmid_2_4g = -1;
	}
	if(-1 !=shmid_5g) {
		del_semvalue(semid_5g);
		semid_5g = -1;
		if(0 != shmctl(shmid_5g,IPC_RMID,0)) {
			shmid_5g = -1;
			RETURN_WIFIMGR_FAIL_MSG("Free share memory for 5G data failed.\n");
		}	
		else
			shmid_5g = -1;
	}
	RETURN_WIFIMGR_SUCCESS;
}
#endif

int 
creat_shm_from_datafile(wifi_type type)
{
	WIFI_FUNC_TRACE_ENTER;
	ATTR_NODE shm_node;
	ATTR_NODE *shm_addr = NULL, *shm_tmp= NULL;
	char path[MAX_PATH_LEN] = {0};
	char linebuffer[BUFFER_ITEM_SIZE] = {0};
	FILE *fd = NULL;
	int semid = -1;
	int i = 0, j = 0;
	int num = 0;

	if(WIFI_2_4G == type)
	{
		if(-1 == shmid_2_4g)
		{
			if(ECNT_WIFIMGR_SUCCESS != init_shm(type))
			{
				RETURN_WIFIMGR_FAIL_MSG("Initialize share memory for 2.4G failed.\n");
			}
		}
		shm_addr = shm_addr_2_4g;
		strcpy(path,dat_path_2_4g);
		semid = semget(SEMKEY_2_4G, 1, 0666);
		num = num_2_4g;
	}	
	else if(WIFI_5G == type)
	{
		if(-1 == shmid_5g)
		{
			if(ECNT_WIFIMGR_SUCCESS != init_shm(type))
			{
				RETURN_WIFIMGR_FAIL_MSG("Initialize share memory for 5G failed.\n");
			}
		}
		shm_addr = shm_addr_5g;
		strcpy(path,dat_path_5g);
		semid = semget(SEMKEY_5G, 1, 0666);
		num = num_5g;
	}
	else
	{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	fd = fopen(path, READ_MODE);  
    if (NULL == fd)
    {
		RETURN_WIFIMGR_FAIL_MSG("Open the file %s failed.\n",path);
    }
	
	shm_tmp = shm_addr;
	if(NULL == shm_tmp)
	{
		fclose(fd);
		RETURN_WIFIMGR_FAIL_MSG("shm_tmp is NULL!\n");
		//RETURN_WIFIMGR_FAIL_MSG("There is no %s in the share memory!\n",shm_node.attr_name);
	}
	if(ECNT_WIFIMGR_SUCCESS != semaphore_p(semid))
	{
		fclose(fd);
		RETURN_WIFIMGR_FAIL_MSG("p operation failed.\n");
	}
	
	memset((char *)shm_tmp, 0, sizeof(ATTR_NODE) * num);

	if(ECNT_WIFIMGR_SUCCESS != semaphore_v(semid))
	{
		fclose(fd);
		RETURN_WIFIMGR_FAIL_MSG("v operation failed.\n");
	}

	while(fgets(linebuffer, BUFFER_ITEM_SIZE, fd) > 0 && j < num)
	{
		memset(&shm_node, 0, sizeof(shm_node));		
		if(ECNT_WIFIMGR_SUCCESS == parse_data_from_line(linebuffer, &shm_node))
		{
			if(0 != semaphore_p(semid))
			{
				fclose(fd);
				RETURN_WIFIMGR_FAIL_MSG("p operation failed.\n");
			}
			
			memset(shm_tmp->attr_name, 0, sizeof(shm_tmp->attr_name));
			memcpy(shm_tmp->attr_name, shm_node.attr_name, 
									sizeof(shm_tmp->attr_name));

			for (i = 0; i < MAX_SSID_NUM && strlen(shm_node.attr_value[i]) > 0; ++i)
			{
				memset(shm_tmp->attr_value[i], 0, sizeof(shm_tmp->attr_value[i]));
				memcpy(shm_tmp->attr_value[i], shm_node.attr_value[i], 
										sizeof(shm_tmp->attr_value[i]));
			}
			if(0 != semaphore_v(semid))
			{
				fclose(fd);
				RETURN_WIFIMGR_FAIL_MSG("v operation failed.\n");
			}
			shm_tmp++;
		}
		memset(linebuffer, 0, sizeof(linebuffer));	
		j++;
	}
	fclose(fd);

	RETURN_WIFIMGR_SUCCESS;
}

int parserstr(char *input,int inputlen)
{
	char *pf = NULL;
	char *ps = input;
	int  iLen = 0;
	int reallen = 0;	
	
	if(inputlen <= 2)
		return ECNT_WIFIMGR_INTENAL_ERR;
	
	pf = strrchr(ps, '\r');
	if(NULL != pf){
		iLen++;
	}
	
	pf = strrchr(ps, '\n');
	if(NULL != pf){
		iLen++;
	}

	reallen =inputlen -iLen;
	input[reallen] = '\0';
	return ECNT_WIFIMGR_SUCCESS;	

}

int 
attr_value_parser(char *attr_name, char *attr_value, int *num, char **para)
{
	WIFI_FUNC_TRACE_ENTER;
	int i = 0;	
    int cnt = 0, pos = 0;
    enum{VALID_FLAG, INVALID_FLAG} state = VALID_FLAG;
	
	for(i = 0; i < SPECIAL_PARA_COUNT; ++i) {
		if(0 == strncmp(attr_name, special_para_four[i], strlen(special_para_four[i]))) {
	parserstr(attr_value,strlen(attr_value));
			para[cnt] = & attr_value[pos];
			*num = cnt + 1;
			RETURN_WIFIMGR_SUCCESS;
		}
	}
	
    for(pos = 0; 0 != attr_value[pos] && '\n' != attr_value[pos]; ++pos)
    {
        switch(state)
        {
            case VALID_FLAG:
                if(';' != attr_value[pos])
                {
                    state = INVALID_FLAG;
                    para[cnt] = & attr_value[pos];
                }
                break;

            case INVALID_FLAG:
                if(';' == attr_value[pos])
                {
                    attr_value[pos] = 0;
                    state = VALID_FLAG;
                    ++ cnt;
                    if(cnt > MAX_SSID_NUM - 1)
                    {
                    	*num = cnt;
                    	RETURN_WIFIMGR_SUCCESS;
                    }
                }
                break;
        }
    }
	*num = cnt + 1;
	RETURN_WIFIMGR_SUCCESS;
}

int 
parse_data_from_line(char *linebuffer, ATTR_NODE *shm_node_p)
{
	char buffer1[BUFFER_ITEM_SIZE] = {0};  
 	char buffer2[BUFFER_ITEM_SIZE] = {0}; 
	char* para[MAX_SSID_NUM] = {NULL};
	int i = 0;
	int num = 0;
	memset(buffer1, 0, sizeof(buffer1));
	memset(buffer2, 0, sizeof(buffer2));
	/*filter the comment*/
	if (strlen(linebuffer) > 0 && *linebuffer == '#') {
		RETURN_WIFIMGR_DEBUG_MSG("This is a comment, and do not need to parse.\n");
	}
	/*filter the "Default"*/
	if (0 == strncmp("Default", linebuffer, strlen(linebuffer)-1)) {
		RETURN_WIFIMGR_DEBUG_MSG("This is a \"Default\", and do not need to parse.\n");
	}

	sscanf(linebuffer, "%[^=]=%[^=]", buffer1, buffer2 );
	memcpy(shm_node_p->attr_name, buffer1,sizeof(shm_node_p->attr_name));
	if(ECNT_WIFIMGR_SUCCESS == attr_value_parser(buffer1, buffer2, &num, para)) {
		for( i = 0; i < num && NULL != para[i]; ++i ) {
			memcpy(shm_node_p->attr_value[i], para[i], strlen(para[i])+1);	
		}
	}
	else 
		RETURN_WIFIMGR_FAIL_MSG("Parse the attr_value failed.\n");
	RETURN_WIFIMGR_SUCCESS;
}


int 
is_attr_exist(ATTR_NODE *shm_addr, char *attr_name, int *num)
{
	int found = KEYWORD_NOT_EXIST;
	int i = 0;
	
	for (i = 0; '\0' != shm_addr->attr_name[0]; shm_addr++, i++) {
		if (0 == strncmp(shm_addr->attr_name, attr_name, 
			(strlen(shm_addr->attr_name) > strlen(attr_name)) ?
			strlen(shm_addr->attr_name) : strlen(attr_name))) {
			found = KEYWORD_EXIST;
			*num = i;
			break;
		}		
	}
	return found;
}

int 
update_attr_by_ssid(ATTR_NODE *shm_addr, int num, int index, char *attr_name, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	int i = 0;
	
	for (i = 0; i < SPECIAL_PARA_NUM; ++i) {
		if(0 == strncmp(attr_name, special_para_one[i], strlen(special_para_one[i]))) 
			break;		
	}
	
	if (i == SPECIAL_PARA_NUM) {
		shm_addr += num;
		if(0 == strcmp(shm_addr->attr_name, attr_name)) {
			memset(shm_addr->attr_value[index-1] , 0, sizeof(shm_addr->attr_value[index-1]));
			if(sizeof(shm_addr->attr_value[index-1]) > strlen(input) )
				memcpy(shm_addr->attr_value[index-1] , input, strlen(input));
			else
				memcpy(shm_addr->attr_value[index-1] , input, sizeof(shm_addr->attr_value[index-1]) - 1);
		}	
	}
	else {
		if( ECNT_WIFIMGR_SUCCESS == update_common_attr(shm_addr, num, attr_name, input))
			RETURN_WIFIMGR_SUCCESS;
		else
			RETURN_WIFIMGR_FAIL;
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
update_common_attr(ATTR_NODE *shm_addr, int num, char *attr_name, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	int size = sizeof(shm_addr->attr_value[0]);
	shm_addr += num;
	if(0 == strcmp(shm_addr->attr_name, attr_name)) {
		strncpy(shm_addr->attr_value[0], input, size - 1);
		shm_addr->attr_value[0][size-1] = '\0';
		RETURN_WIFIMGR_SUCCESS;
	}	
	RETURN_WIFIMGR_FAIL;
}

int 
update_attr(wifi_type type, int index, char *attr_name,  char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	ATTR_NODE *shm_addr = NULL;
	int num = 0;
	int semid = -1;
	
	if(WIFI_2_4G == type) {
		shm_addr = shm_addr_2_4g;
		semid = semget(SEMKEY_2_4G, 1, 0666);
	}	
	else if(WIFI_5G == type) {
		shm_addr = shm_addr_5g;
		semid = semget(SEMKEY_5G, 1, 0666);
	}	
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");	
	}
	
	if(ECNT_WIFIMGR_SUCCESS != semaphore_p(semid)) {
		RETURN_WIFIMGR_FAIL_MSG("p operation is wrong.\n");
	}
	if(KEYWORD_EXIST == is_attr_exist(shm_addr, attr_name, &num)) {
		if(-1 == index) {
			if( ECNT_WIFIMGR_SUCCESS != update_common_attr(shm_addr, num, attr_name, input)){
				semaphore_v(semid);
				RETURN_WIFIMGR_FAIL_MSG("Modify the attr_value failed.\n");
		}	
		}	
		else if( index >= 1 && index <= MAX_SSID_NUM) {
			if( ECNT_WIFIMGR_SUCCESS != update_attr_by_ssid(shm_addr, num, index, attr_name, input)) {
				semaphore_v(semid);
				RETURN_WIFIMGR_FAIL_MSG("Modify the attr_value failed.\n");
		}	
		}	
		else{
			semaphore_v(semid);
			RETURN_WIFIMGR_FAIL_MSG("The index is invalid.\n");	
	}	
	}	
	else {
		semaphore_v(semid);
		RETURN_WIFIMGR_FAIL_MSG("The attr_name do not exist.\n");
	}
	
	if(ECNT_WIFIMGR_SUCCESS != semaphore_v(semid)){
		RETURN_WIFIMGR_FAIL_MSG("v operaion failed.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
read_attr(wifi_type type, int index, char *attr_name, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	int num = 0;
	ATTR_NODE *shm_addr = NULL;
	int semid = -1;
	int i = 0;

	if(WIFI_2_4G == type) {
		shm_addr = shm_addr_2_4g;
		/*semid = semid_2_4g;*/
		semid = semget(SEMKEY_2_4G, 1, 0666);
	}
	else if(WIFI_5G == type) {
		shm_addr = shm_addr_5g;
		/*semid = semid_5g;*/
		semid = semget(SEMKEY_5G, 1, 0666);
	}		
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");

	if(ECNT_WIFIMGR_SUCCESS != semaphore_p(semid)){
		RETURN_WIFIMGR_FAIL_MSG("p operation failed.\n");
	}
	if(KEYWORD_EXIST == is_attr_exist(shm_addr, attr_name, &num)) {
		shm_addr += num;
		for (i = 0; i < SPECIAL_PARA_NUM; ++i) {
			if(0 == strncmp(shm_addr->attr_name, special_para_one[i], strlen(special_para_one[i]))) 
				break;		
		}
		
		if (i < SPECIAL_PARA_NUM) {
			strncpy(output ,shm_addr->attr_value[0], size);
		}	
		
		if (i == SPECIAL_PARA_NUM) {
			if(size > ATTR_VALUE_SIZE) {
				if(-1 == index) {
					memcpy(output, shm_addr->attr_value[0], sizeof(shm_addr->attr_value[0]));
				}
				else if(index >= 1 && index <= MAX_SSID_NUM) {
					memcpy(output, shm_addr->attr_value[index-1], sizeof(shm_addr->attr_value[index-1]));
				}	
			}	
			else{
				semaphore_v(semid);
				RETURN_WIFIMGR_FAIL_MSG("The size of buffer is too small!\n");
		}
	}
	}
	else {
		semaphore_v(semid);
		RETURN_WIFIMGR_FAIL_MSG("The attr_name do not exist.\n");
	}
	
	if(ECNT_WIFIMGR_SUCCESS != semaphore_v(semid)) {
		RETURN_WIFIMGR_FAIL_MSG("v operation failed.\n");
	}
	RETURN_WIFIMGR_SUCCESS;
}

int 
merge_para(ATTR_NODE *shm_addr, char *buffer, int count) 
{
	int i = 0;
	char tmp[BUFFER_ITEM_SIZE] = {0}; 
	for(i = 1; i <= count; ++i) {
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, shm_addr->attr_value[i-1], strlen(shm_addr->attr_value[i-1]));	
		if( strlen(tmp) > 0) {
			if(tmp[strlen(tmp)-1] == '\n') {
				tmp[strlen(tmp)-1] = ';';
			}
			else {
				tmp[strlen(tmp)] = ';';
			}	
			strcat(buffer,tmp);
		}
		else 
			break;
	}	
	buffer[strlen(buffer)-1] = '\0';
	RETURN_WIFIMGR_SUCCESS;
}

int
get_attr_num(ATTR_NODE *shm_addr, char *buffer, int *num) {
	int i = 0;
	char *str = NULL;
	for( i = 0; i < SPECIAL_PARA_NUM_COMMON; ++i ) {
		if( 0 == strncmp(shm_addr->attr_name, special_para_two[i], 
										strlen(special_para_two[i]))) {
			str = &buffer[strlen(special_para_two[i])];
			*num = atoi(str);
			RETURN_WIFIMGR_SUCCESS;
		}
	}

	RETURN_WIFIMGR_FAIL;
}

int
get_attr_num_extended(ATTR_NODE *shm_addr, char *buffer, int *num) {
	int i = 0;
	char *str = NULL;
	for( i = 0; i < SPECIAL_PARA_NUM_EXTENED; ++i ) {
		if( 0 == strncmp(shm_addr->attr_name, special_para_three[i], 
									strlen(special_para_three[i]))) {
			str = &buffer[strlen(special_para_three[i])];
			*num = atoi(str);
			RETURN_WIFIMGR_SUCCESS;
		}
	}

	RETURN_WIFIMGR_FAIL;
}

int 
update_datafile_from_shm(wifi_type type , int count)
{
	WIFI_FUNC_TRACE_ENTER;
	ATTR_NODE *shm_addr = NULL;
	char path[MAX_PATH_LEN] = {0};
	FILE* fd = NULL;
	int i = 0;
	char buffer1[BUFFER_ITEM_SIZE] = {0};
	char buffer2[BUFFER_ITEM_SIZE] = {0};
	int semid = -1;
	int num = 0;
	
	if(WIFI_2_4G == type) {
		strcpy(path, dat_path_2_4g);
		shm_addr = shm_addr_2_4g;
		semid = semget(SEMKEY_2_4G, 1, 0666);
	}
	else if(WIFI_5G == type) {
		strcpy(path, dat_path_5g);
		shm_addr = shm_addr_5g;
		semid = semget(SEMKEY_5G, 1, 0666);
	}	
	else 
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	
	fd = fopen(path, WRITE_MODE);  
    if (NULL == fd)  
		RETURN_WIFIMGR_FAIL_MSG("Open the file %s failed.\n",path);
	
	flockfile(fd);
		
	fseek(fd, 0, SEEK_SET); 
	
	char buffer[BUFFER_ITEM_SIZE] = "Default\n";
	if(ECNT_WIFIMGR_SUCCESS != semaphore_p(semid)) {
		funlockfile(fd); 
		fclose(fd);
		RETURN_WIFIMGR_FAIL_MSG("p operation failed.\n");
	}
	fwrite(buffer, strlen(buffer), 1, fd);

	for(; NULL != shm_addr && strlen(shm_addr->attr_name) > 0; shm_addr++, i++) {
		memset(buffer1,0,sizeof(buffer1));
		memset(buffer2,0,sizeof(buffer2));

		sprintf(buffer1, "%s", shm_addr->attr_name);
		buffer1[strlen(buffer1)] = '\0';
		/*get the num of parameter which starts from 1*/
		if(ECNT_WIFIMGR_SUCCESS == get_attr_num(shm_addr, buffer1, &num)) {
			if( num <= count) {
				if(ECNT_WIFIMGR_SUCCESS != merge_para(shm_addr, buffer2, 1)) {
					semaphore_v(semid);
					funlockfile(fd); 
					fclose(fd);
					RETURN_WIFIMGR_FAIL_MSG("merge some values to one line failed.\n");
				}
		 		buffer1[strlen(buffer1)] = '='; 
    			strcat(buffer1, buffer2); 
				if('\n' != buffer1[strlen(buffer1)-1]) {
					buffer1[strlen(buffer1)] = '\n';
				}
				fwrite(buffer1, strlen(buffer1), 1, fd);//include '\n'
			}
		}
		/*get the num of parameter which starts from 0*/
		else if(ECNT_WIFIMGR_SUCCESS == get_attr_num_extended(shm_addr, buffer1, &num)) {
			if( num < count) {
				if(ECNT_WIFIMGR_SUCCESS != merge_para(shm_addr, buffer2, 1)) {
					semaphore_v(semid);
					funlockfile(fd); 
					fclose(fd);
					RETURN_WIFIMGR_FAIL_MSG("merge some values to one line failed.\n");
				}
		 		buffer1[strlen(buffer1)] = '='; 
    			strcat(buffer1, buffer2); 
				if('\n' != buffer1[strlen(buffer1)-1]) {
					buffer1[strlen(buffer1)] = '\n';
				}
				fwrite(buffer1, strlen(buffer1), 1, fd);//include '\n'
			}
		}
		/*there is no number in parameter*/
		else {
			if(ECNT_WIFIMGR_SUCCESS != merge_para(shm_addr, buffer2, count)) {
				semaphore_v(semid);
				funlockfile(fd); 
				fclose(fd);
				RETURN_WIFIMGR_FAIL_MSG("merge some values to one line failed.\n");
			}
		 	buffer1[strlen(buffer1)] = '='; 
    		strcat(buffer1, buffer2); 
			if('\n' != buffer1[strlen(buffer1)-1]) {
				buffer1[strlen(buffer1)] = '\n';
			}
			fwrite(buffer1, strlen(buffer1), 1, fd);//include '\n'
		}
	}
	
	if(ECNT_WIFIMGR_SUCCESS != semaphore_v(semid)) {
		funlockfile(fd); 
		fclose(fd);
		RETURN_WIFIMGR_FAIL_MSG("v operation failed.\n");
	}
	funlockfile(fd); 
	fclose(fd);
	
	RETURN_WIFIMGR_SUCCESS;
}

int 
check_is_num(char *str) 
{
	ECNT_WIFIMGR_FLAG flag = ECNT_WIFIMGR_TRUE;
	int i = 0;
	for(i = 0; str[i] != '\0' && str[i] != '\n'; ++i) {
		int dist = str[i] - '0';
		if(dist < 0 || dist > 9)
			flag = ECNT_WIFIMGR_FALSE;
	}
	return flag;
}

int
check_para_valid(char *input, char *str1, char *str2, char *str3, char *str4) 
{
	int count = 0;
	char *str = input;
	if(';' == *str){
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	}
	while(*str != '\0' && *str!= '\n') {
		if(*str == ';') {
			count++;
		}
		if(*str == ';' && *(str+1) == ';') {
			RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
		}
		str++;
	}
	if(count != 3) {
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	}
	sscanf(input,"%[^;];%[^;];%[^;];%[^;]",str1,str2,str3,str4);
	if(0 == strlen(str4)) {
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	}
	if( ECNT_WIFIMGR_TRUE == check_is_num(str1) 
		&& ECNT_WIFIMGR_TRUE == check_is_num(str2)
		&& ECNT_WIFIMGR_TRUE == check_is_num(str3) 
		&& ECNT_WIFIMGR_TRUE == check_is_num(str4)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	else {
		RETURN_WIFIMGR_FAIL_MSG("The input parameter is invalid.\n");
	}	
}

int 
check_hex_valid(char *str) 
{
	ECNT_WIFIMGR_FLAG flag = ECNT_WIFIMGR_TRUE;
	for (; '\0' != *str && '\n' != *str; str++) {
		if(*str < 0x30 || (*str > 0x39 && *str < 0x41) ||
			(*str > 0x46 && *str < 0x61 ) || *str > 0x66)
			flag = ECNT_WIFIMGR_FALSE;
	}
	return flag;
}

int
print_shm(wifi_type type) 
{
	char buffer[PRINT_SHM_SIZE] = {0};
	ATTR_NODE *shm_addr = NULL;
	int i = 0;
	if(WIFI_2_4G == type) {
		shm_addr = shm_addr_2_4g;
	}
	else if(WIFI_5G == type) {
		shm_addr = shm_addr_5g;
	}
	else
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");

	for (; '\0' != shm_addr->attr_name[0]; shm_addr++) {
		printf("%s:\n", shm_addr->attr_name);
		memset(buffer, ' ', sizeof(buffer));
		buffer[PRINT_SHM_SIZE-2] = '\n';
		buffer[PRINT_SHM_SIZE-1] = '\0';
		for (i=0; i < MAX_SSID_NUM && strlen(shm_addr->attr_value[i]) > 0; i++) {
			if('\n' == shm_addr->attr_value[i][strlen(shm_addr->attr_value[i])-1])
				shm_addr->attr_value[i][strlen(shm_addr->attr_value[i])-1] = '\0';
			strncpy(&buffer[(i%4)*30], shm_addr->attr_value[i], 
							strlen(shm_addr->attr_value[i]));
			if((i+1)%4 == 0){
				printf("%s", buffer);
				memset(buffer, ' ', sizeof(buffer));
				buffer[PRINT_SHM_SIZE-2] = '\n';
				buffer[PRINT_SHM_SIZE-1] = '\0';
			}
		}
		if(i == 1)
			printf("%s", buffer);
	}	
	RETURN_WIFIMGR_SUCCESS;
}

int 
parse_driver(char *flag, unsigned int size)
{
	FILE *fd = NULL;
	char driver[7][BUFFER_ITEM_SIZE] = {"7603","7612","7615","7663","7915","7916","7905"};
	char linebuffer[1024] = {0};
	int i = 0;

	system("cat /proc/bus/pci/devices > /tmp/devices");
	fd = fopen("/tmp/devices", READ_MODE);  
    if (NULL == fd)  
		RETURN_WIFIMGR_FAIL_MSG("Open the file /tmp/devices failed.\n");

	while(fgets(linebuffer, 1024, fd) > 0) {
		for(i = 0; i < 7; i++) {
			if(NULL != strstr(linebuffer, driver[i])){
				flag[i] = '1';
			}		
		}
	}
	fclose(fd);
	RETURN_WIFIMGR_SUCCESS;
}
	

