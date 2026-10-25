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

#include <libapi_lib_utility.h>
//#include <libapi_lib_cpu_interrupt.h>
#include <ecnt_hook/ecnt_hook_cpu_interrupt_type.h>
#include <common/ecnt_chip_id.h>


static int do_show_interrupts(int argc, char *argv[], void *p);
static int do_show(int argc, char *argv[], void *p);
static int do_set(int argc, char *argv[], void *p);


static cmds_t showCmds[] = {
	{"interrupts",	do_show_interrupts,	0x0, 0, NULL},
    {NULL,              NULL,       		    0x0, 0, NULL}
};

static cmds_t irqCpuCmds[] = {
    {"show",    do_show,    0x0, 0, NULL},
    {"set",   	do_set,   	0x0, 0, NULL},
    {NULL,      NULL,       0x0, 0, NULL}
};

static int do_show_interrupts(int argc, char *argv[], void *p)
{
    cpu_interrupt_lib_show_intrNames();
	
	return 0;
}

static int do_show(int argc, char *argv[], void *p)
{
	if(getHardwareIdx() >= EN7523_HIR) {
		printf("\t(Note: \"cat /proc/interrupts\" to show interrupts' info)\n\n");
	}
	return subcmd(showCmds, argc, argv, NULL);
}

static int do_set(int argc, char *argv[], void *p)
{
	if (argc != 3)
	{
		printf("\nUsage: irqCpuBind set <interruptName> <cpu>, where <cpu> == 0,1,2,or 3\n");

		if(check_isen7523()) {
			printf("\t(Note: <cpu> == 0 or 1 will bind to CPU0\n");
			printf("\t(Note: <cpu> == 2 or 3 will bind to CPU1\n");
			printf("\t(Note: \"cat /proc/interrupts\" to show interrupts' info)\n\n");
		} else {
			printf("\t(Note: \"irqCpuBind show interrupts\" to show interrupts' info)\n\n");
		}
		return 0;
	}

    cpu_interrupt_lib_bind_cpu_by_name(strtoul(argv[2], NULL, 10), argv[1]);

    return 0;
}

int main(int argc, char **argv) 
{
	return subcmd(irqCpuCmds, argc, argv, NULL);
}


