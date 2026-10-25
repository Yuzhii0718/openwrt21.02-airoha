
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
*************************************************************************
*/
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libapi_lib_utility.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define _SUCCESS	(0)
#define _FAIL	(-1)


#define MODEMDEVICE "/dev/ttyS%d0"





#define BUF_SIZE 256

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/



/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int DoUART2Cmd(int argc, char **argv, void *p);

static int Dou1Tou2(int argc, char **argv, void *p);
#if (TCSUPPORT_CPU_EN7516 | TCSUPPORT_CPU_EN7527 | TCSUPPORT_CPU_EN7580)
static int Dou1Tou3(int argc, char **argv, void *p);
#if (TCSUPPORT_CPU_EN7580)
static int Dou1Tou4(int argc, char **argv, void *p);
static int Dou1Tou5(int argc, char **argv, void *p);
#endif
#endif

static int Dou2Tou1(int argc, char **argv, void *p);
#if (TCSUPPORT_CPU_EN7516 | TCSUPPORT_CPU_EN7527 | TCSUPPORT_CPU_EN7580)
static int Dou3Tou1(int argc, char **argv, void *p);
#if (TCSUPPORT_CPU_EN7580)
static int Dou4Tou1(int argc, char **argv, void *p);
static int Dou5Tou1(int argc, char **argv, void *p);
#endif
#endif


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
static cmds_t UART2DbgCmds[] = {
    {"u1Tou2",           Dou1Tou2,        0x10,   0,  NULL},
    {"u2Tou1",           Dou2Tou1,        0x10,   0,  NULL},
#if (TCSUPPORT_CPU_EN7516 | TCSUPPORT_CPU_EN7527 | TCSUPPORT_CPU_EN7580)
    {"u1Tou3",           Dou1Tou3,        0x10,   0,  NULL},
    {"u3Tou1",           Dou3Tou1,        0x10,   0,  NULL},
#if (TCSUPPORT_CPU_EN7580)
    {"u1Tou4",           Dou1Tou4,        0x10,   0,  NULL},
    {"u4Tou1",           Dou4Tou1,        0x10,   0,  NULL},
    {"u1Tou5",           Dou1Tou5,        0x10,   0,  NULL},
    {"u5Tou1",           Dou5Tou1,        0x10,   0,  NULL},
#endif
#endif
    {NULL,            NULL,            0x10,   0,  NULL},
};


int DoUART2Cmd(int argc, char **argv, void *p)  
{
    return subcmd(UART2DbgCmds, argc, argv, p);   
}




/******************************************************************************
 Descriptor:	It's used to .
 Input Args:	
 Ret Value:	success: _SUCCESS
                       fail: _FAIL
******************************************************************************/
static int Dou1Toux(char **argv, int x)
{
        char cmdbuf[BUF_SIZE+50];
        sprintf(cmdbuf, "echo %s > /dev/ttyS%d0", argv[1], x); 
        system(cmdbuf);
        printf("Exec. Successful\n");
        return _SUCCESS;
}
static int Dou1Tou2(int argc, char **argv, void *p){ Dou1Toux(argv, 2); return _SUCCESS;}
#if (TCSUPPORT_CPU_EN7516 | TCSUPPORT_CPU_EN7527 | TCSUPPORT_CPU_EN7580)
static int Dou1Tou3(int argc, char **argv, void *p){ Dou1Toux(argv, 3); return _SUCCESS;}
#if (TCSUPPORT_CPU_EN7580)
static int Dou1Tou4(int argc, char **argv, void *p){ Dou1Toux(argv, 4); return _SUCCESS;}
static int Dou1Tou5(int argc, char **argv, void *p){ Dou1Toux(argv, 5); return _SUCCESS;}
#endif
#endif

/******************************************************************************
 Descriptor:	It's used to .
 Input Args:	
 Ret Value:	success: _SUCCESS
                       fail: _FAIL
******************************************************************************/
static int DouxTou1(int x)
{
        char cmdbuf[50];
        snprintf(cmdbuf, sizeof(cmdbuf), MODEMDEVICE, x); 

        int fd = 0, res = 0;
        char buf[BUF_SIZE] = {0};
        
        fd = open(cmdbuf, O_RDWR | O_NONBLOCK);
        if (fd < 0)
        {
              perror(cmdbuf);
              exit(-1);
        }
        
        while (1)
        {
              res = read(fd, buf, BUF_SIZE);
              if (res > 1 && res < sizeof(buf) / sizeof(buf[0]))
              {
                buf[res] = 0;
                printf("%s", buf);
                write(fd, "I recieve your message: ", 24);
                write(fd, buf, BUF_SIZE);
        
                memset(buf, 0, BUF_SIZE);
                res = 0;
              }
         }
        
         close(fd);

        printf("Exec. Successful\n") ;
                  
        return _SUCCESS;
}

static int Dou2Tou1(int argc, char **argv, void *p){DouxTou1(2); return _SUCCESS;}
#if (TCSUPPORT_CPU_EN7516 | TCSUPPORT_CPU_EN7527 | TCSUPPORT_CPU_EN7580)
static int Dou3Tou1(int argc, char **argv, void *p){DouxTou1(3); return _SUCCESS;}
#if (TCSUPPORT_CPU_EN7580)
static int Dou4Tou1(int argc, char **argv, void *p){DouxTou1(4); return _SUCCESS;}
static int Dou5Tou1(int argc, char **argv, void *p){DouxTou1(5); return _SUCCESS;}
#endif
#endif