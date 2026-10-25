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
#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <libapi_lib_utility.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/


/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/
struct mtd_stress_info{
	unsigned long	rounds;
	char 			name[30];
};

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/
extern int subcmd(const cmds_t tab[], int argc, char *argv[], void *p);
extern int api_lib_help(int argc, char *argv[], void *p);

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/
int do_stress_test(int argc, char *argv[], void *p);

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/
const cmds_t spi_flash_set_command[] = {
/*	name,					func,			flags,	argcmin,	argc_errmsg */
	{"stress_test",		do_stress_test,		0,		2,			"Set spi flash stress"},
	/* Cmd should be added before this line!  */
	{"help",			api_lib_help,		0,		0,			"Show spi_flash set usage."},
	{NULL, NULL, 0, 0, NULL}
};

/******************************************************************************
 Descriptor:	
 Input Args:	
 Ret Value:	success: 0
                       fail: -1
******************************************************************************/
void *mtd_write(void *arg)
{
	cpu_set_t cpuset;
	int i, cpu = 3;
	cpu_set_t mask;
 	pid_t pid = 0;
	struct mtd_stress_info *info = (struct mtd_stress_info *)arg;
	char mtd_str[100];

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);

	sched_setaffinity(0, sizeof(cpuset), &cpuset);

	if (sched_getaffinity(pid, sizeof (mask), &mask) < 0)
 			printf("Failed to %cet pid %d's affinity", 'g', pid);
 	printf("mtd_write. pid %d's affinity \n", pid);
	
	snprintf(mtd_str, sizeof(mtd_str), "mtd writeflash /tmp/test.write 10485760 0 %s", info->name);
	for(i = 0; i < info->rounds; i++){
		printf("=== mtd_write:%d ===\n", i);
		system(mtd_str);
	}

	return (void *)NULL;
}

void *mtd_read(void *arg)
{
	cpu_set_t cpuset;
	int i, cpu = 0;
	cpu_set_t mask;
 	pid_t pid = 0;
	struct mtd_stress_info *info = (struct mtd_stress_info *)arg;
	char mtd_str[100];

	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);

	sched_setaffinity(0, sizeof(cpuset), &cpuset);

	if (sched_getaffinity(pid, sizeof (mask), &mask) < 0)
 			printf("Failed to %cet pid %d's affinity", 'g', pid);
 	printf("mtd_read. pid %d's affinity\n", pid);

	snprintf(mtd_str, sizeof(mtd_str), "mtd readflash /tmp/test.read 10485760 0 %s", info->name);
	for(i = 0; i < info->rounds; i++){
		printf("=== mtd_read:%d ===\n", i);
		system(mtd_str);
	}

	return (void *)NULL;
}


int do_stress_test(int argc, char *argv[], void *p)
{
#if API_CMD_SPI_FLASH_DBG
	printf("enter %s.\n", __func__);
#endif
	pthread_t thread_write, thread_read;
	struct mtd_stress_info info;
	char defaultMtd[] = "yaffs";

	if(argv[1]) {
		info.rounds = atoi(argv[1]);
	} else {
		info.rounds = 50;
	}

	if(argv[2]) {
		strncpy(info.name, argv[2], sizeof(info.name) - 1);
		info.name[sizeof(info.name) - 1] = '\0';
	} else {
		strncpy(info.name, defaultMtd, sizeof(defaultMtd));
	}

	printf("mtd partition:%s, mtd test rounds:%lu\n", info.name, info.rounds);

	printf("create random file 10MB to /tmp/test.write\n");
	system("dd if=/dev/urandom of=/tmp/test.write bs=1M count=10");

	if(0 != pthread_create(&thread_write, NULL, mtd_write, (void *)(&info)))
		printf("pthread create failed\n");
	if(0 != pthread_create(&thread_read, NULL, mtd_read, (void *)(&info)))
		printf("pthread create failed\n");

	pthread_join(thread_write, NULL);
	pthread_join(thread_read, NULL);

	printf("Exec. Successful\n") ;

	return 0;
}

