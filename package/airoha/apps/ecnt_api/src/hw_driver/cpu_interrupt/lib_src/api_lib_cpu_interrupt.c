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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "libapi_lib_cpu_interrupt.h"
#include <ecnt_hook/ecnt_hook_cpu_interrupt_type.h>
#include <errno.h>
#include <common/ecnt_chip_id.h>

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

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/
unsigned int getHardwareIdx(void)
{
	FILE *fp = NULL;
	unsigned int chidIdx = END_PACKAGE_ID;
	int ret = 0;

	if ((fp = fopen("/proc/tc3162/hid", "r")) == NULL){
		printf("Critical Bugs, Can't get package id!\n");
		return END_PACKAGE_ID;
	}

	ret = fscanf(fp, "%x", &chidIdx);
	if(ret <= 0)
	{
		printf("Critical Bugs, Can't assign chidIdx!\n");
		fclose(fp);
		return END_PACKAGE_ID;
	}

	fclose(fp);

	return chidIdx;
}

int check_isen7523(void)
{
	int isEN7523 = 0;

	if (getHardwareIdx() == EN7523_HIR)
		isEN7523 = 1;

	return isEN7523;
}

extern int errno;
static int cpu_interrupt_lib_ioctl(int cmd, void *p)
{
	int fd, ret;
	struct ecnt_cpu_interrupt_data *para = (struct ecnt_cpu_interrupt_data*) p;
	//printf("0===errno:%d\n errmessage:%s\n", errno, strerror(errno));
	fd = open("/dev/cpu_interrupt", O_RDWR);
	//printf("1===errno:%d\n errmessage:%s\n", errno, strerror(errno));
    if (fd < 0) {
		printf("Open %s pseudo device failed\n","/dev/cpu_interrupt");
		return -1;
    }
	//printf("2==errno:%d\n errmessage:%s\n", errno, strerror(errno));
	para->function_id = cmd;
	
	ret = ioctl(fd, cmd, para);
	
	close(fd);

	if(ret >= 0) 
		return 0;
	
	return -1;
}

int cpu_interrupt_lib_get_irqNum_by_name(char* intrName)
{
	int ret;
	struct ecnt_cpu_interrupt_data para;

    if (strlen(intrName) >= IRQ_STRING_LEN_MAX) {
		printf("\nError: irq string too long. Max length is %d\n", IRQ_STRING_LEN_MAX);
		return -1;
    }

	memset(&para, 0, sizeof(struct ecnt_cpu_interrupt_data));
    
	strncpy(para.irqString, intrName, strlen(intrName));
    
	ret = cpu_interrupt_lib_ioctl(CPU_INTERRUPT_GET_IRQNUM, &para);

    if (ret==-1 || para.retValue==-1) {
        printf("\nError: %s return fail\n", __func__);
        return -1;
    }
    
	return para.irqNum;
}

int cpu_mask_to_cpu_num (int cpu_mask)
{
    int cpu_num=0;

    if (cpu_mask==1) 
        cpu_num=0;
    else if (cpu_mask==2) 
        cpu_num=1;
    else if (cpu_mask==4) 
        cpu_num=2;
    else if (cpu_mask==8) 
        cpu_num=3;
    else {
        printf("\nError: cpu_mask:0x%x is unexpected at %s\n", cpu_mask, __func__);
        return -1;
    }

    return cpu_num;
}

int cpu_interrupt_lib_get_affinityCPU_by_name(char* intrName)
{
	int fd, ret = 0;
	char read_buf[128] = {0};
    int irqNum;
    char irqProc[32] = {0};
    unsigned int cpu_p;

    irqNum = cpu_interrupt_lib_get_irqNum_by_name(intrName);

    if (irqNum<0) {
        printf("\nError: get_irqNum fail at %s\n", __func__);
        return -1;
    }

    sprintf(irqProc, "/proc/irq/%d/smp_affinity", irqNum);
    fd = open(irqProc, O_RDONLY|O_NONBLOCK);

    if (fd == -1) {
        printf("\nError: open fail at %s\n", __func__);
        return -1;
    }

    ret = read(fd, read_buf, 127);
    close(fd);

    if (ret <= 0) {
        printf("\nError: read fail at %s\n", __func__);
        return -1;
    }

    sscanf(read_buf, "%x", &cpu_p);

    return cpu_mask_to_cpu_num(cpu_p);
}

int cpu_interrupt_lib_show_intrNames(void)
{
	int ret;
	struct ecnt_cpu_interrupt_data para;

	memset(&para, 0, sizeof(struct ecnt_cpu_interrupt_data));
    
	ret = cpu_interrupt_lib_ioctl(CPU_INTERRUPT_SHOW_INTERRUPTS, &para);

    if (ret==-1 || para.retValue==-1) {
        printf("\nError: %s return fail\n", __func__);
    }
    
	return ret;
}

int cpu_interrupt_lib_check_intrName(char* intrName)
{
	int ret;
	struct ecnt_cpu_interrupt_data para;

    if (strlen(intrName) >= IRQ_STRING_LEN_MAX) {
		printf("\nError: irq string too long. Max length is %d\n", IRQ_STRING_LEN_MAX);
		return -1;
    }

	memset(&para, 0, sizeof(struct ecnt_cpu_interrupt_data));
    
	strncpy(para.irqString, intrName, strlen(intrName));
    
	ret = cpu_interrupt_lib_ioctl(CPU_INTERRUPT_CHECK_INTRNAME, &para);

    if (ret==-1) {
        printf("\nError: %s return fail\n", __func__);
        return -1;
    }
    
	return para.retValue;
}

static int doValPut(char *path, char *value)
{
	FILE *fp;

	fp = fopen(path,"w");
	if(fp != NULL){
		fprintf(fp,"%s", value);
		fclose(fp);
	}else{
		return -1;
	}
	return 0;
}

int cpu_interrupt_lib_bind_cpu_by_name(int cpu, char* intrName)
{
    int i, irqNum, ret;
    char path[48];
    char cpuStr[4];
	int cpu_ori = cpu;


    if (0<=cpu && cpu<=3) {
		if(check_isen7523()) {
			if(cpu == 0 || cpu == 1) {
				cpu = 1<<0;
			} else {
				cpu = 1<<1;
			}
		} else {
			cpu = 1<<cpu;
		}
    }
    else {
        printf("\nError: wrong cpu: %d\n", cpu);
		if(check_isen7523()) {
	        printf("\t(Note: <cpu> == 0 or 1 will bind to CPU0\n");
			printf("\t(Note: <cpu> == 2 or 3 will bind to CPU1\n");
		} else {
			printf("\t(Note: <cpu> == 0,1,2,or 3\n\n");
		}
        return -1;
    }

    irqNum = cpu_interrupt_lib_get_irqNum_by_name(intrName);

    if (irqNum<0) {
        printf("\nError: %s fail\n", __func__);
        return -1;
    }

    memset(path, 0, sizeof(path));
    memset(cpuStr, 0, sizeof(cpuStr));
    
    sprintf(path, "/proc/irq/%d/smp_affinity", irqNum);
    sprintf(cpuStr, "%d", cpu);
    
    ret = doValPut(path, cpuStr);

    if (ret<0) {
        printf("\nError: doValPut fail\n");
        return -1;
    }

	if(check_isen7523()) {
		printf("Bind %s to CPU %d success!\r\n", intrName, cpu - 1);
	} else {
		printf("Bind %s to CPU %d success!\r\n", intrName, cpu_ori);
	}
    return 0;
}

