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

#ifndef WIFI_FILE_COMM_HEAD
#define WIFI_FILE_COMM_HEAD

#include<sys/sem.h>
/*************************************
*the data struct is saved in share memmory
***************************************/

#define	SPECIAL_PARA_NUM		    12
#define	SPECIAL_PARA_NUM_COMMON		10
#define SPECIAL_PARA_NUM_EXTENED 	2
#define SPECIAL_PARA_COUNT			14
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#define WIFI_MULLTI_CHIP_PATH "/proc/tc3162/wifi_chip_info"
typedef struct 
{
	char chip_name[16];
	unsigned int count;
}wifi_get_driver_info;

extern unsigned int chipflag2g;
extern unsigned int chipflag5g;
extern unsigned int current2gindex;
extern unsigned int current5gindex;
extern unsigned char WLAN_2G_CHIPYPTE[WLAN_2G_MAX_ID];
extern unsigned char WLAN_5G_CHIPYPTE[WLAN_5G_MAX_ID];

#endif
union semun{
int  			val;
struct semid_ds *buf;
unsigned short 	*array;
};

int set_semvalue(int sem_id, int value);
int del_semvalue(int sem_id);
int semaphore_p(int sem_id);
int semaphore_v(int sem_id);


/***********************************************************
*init the share memmory base on wlan_table
***********************************************************/

int init_shm(wifi_type type);
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
int wifi_shm_lock(void);
int wifi_shm_unlock(void);
int init_global_shm(void);
#endif

/***********************************************************
*free the share memmory 
***********************************************************/

int free_shm(void);
/***********************************************************
*print the share memmory data
***********************************************************/
int print_shm(wifi_type type);

/***********************************************************
*creat share memmory data base on data file
***********************************************************/

int creat_shm_from_datafile(wifi_type type);
/***********************************************************
*parse attr_value to every value of ATTR_NODE struct
***********************************************************/

int attr_value_parser(char *attr_name, char *attr_value, int *num, char **para);

/***********************************************************
*parse data to ATTR_NODE struct data from every line in data file
***********************************************************/
int parse_data_from_line(char *linebuffer,ATTR_NODE *shm_node_p);

/***********************************************************
*check the attr_name is saved in share memmory or not
***********************************************************/
int is_attr_exist(ATTR_NODE *shm_addr, char *attr_name, int* num);

/***********************************************************
*modify the attr_value in share memmory for common attribution
***********************************************************/

int update_common_attr(ATTR_NODE *shm_addr, int num, char* attr_name, char* input);
/***********************************************************
*modify the attr_value in share memmory  for multi SSID
***********************************************************/
int	update_attr_by_ssid(ATTR_NODE *shm_addr, int num, int index, char *attr_name, char *input);


/***********************************************************
*modify or add the attr_value in share memmory 
***********************************************************/

int update_attr(wifi_type type, int index, char *attr_name,  char *input);

/***********************************************************
*get the attr_value in share memmory 
***********************************************************/
int read_attr(wifi_type type, int index, char *attr_name, char* output, unsigned int size);

/***********************************************************
*merge some values to one line parameter of datafile 
***********************************************************/

int merge_para(ATTR_NODE *shm_addr, char *buffer, int count);

/***********************************************************
*get the ssid num of para which starts from 1 ,for example: SSID1 ,get the last chracter '1'
***********************************************************/

int check_num_of_para(ATTR_NODE *shm_addr, char *buffer, int *num);

/***********************************************************
*get the ssid num of para which starts from 0 ,for example: AccessPolicy0 ,get the last chracter '0'
***********************************************************/
int check_num_of_para_extended(ATTR_NODE *shm_addr, char *buffer, int *num); 


/***********************************************************
*keep the data in share memmory to a data file 
***********************************************************/
int parse_driver(char *flag, unsigned int size);
int update_datafile_from_shm(wifi_type type, int count);
int check_para_valid(char *input, char *str1, char *str2, char *str3, char *str4);
int check_is_num(char *str);
int check_hex_valid(char *str);

/***********************************************************
*      common function 
***********************************************************/
int doCmdWrite(char *path, char *value);

void
tcdbg_blapi_printf(char *fmt,...);

#endif
