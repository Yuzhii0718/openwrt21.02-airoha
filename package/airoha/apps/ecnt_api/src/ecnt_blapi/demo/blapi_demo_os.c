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
* @file  blapi_demo_os.c  
* @brief This file gives examples on using apis from blapi_os.h.
* @author Bryan
* @date     2019-09-06 
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
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>


#include "blapi_os.h"
#include "libapi_lib_utility.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

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



static int do_test_set_sock_vlan_val(int argc,char* argv[],void *p);
static int do_test_get_sock_vlan_val(int argc,char* argv[],void *p);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/


static const cmds_t blapi_os_test_cmd[]=
{
	{"blapi_lib_os_set_sock_vlan_val",do_test_set_sock_vlan_val,0,0,"blapi_lib_os_sample test blapi_lib_os_set_sock_vlan_val family vlan_val"},
	{"blapi_lib_os_get_sock_vlan_val",do_test_get_sock_vlan_val,0,0,"blapi_lib_os_sample test blapi_lib_os_get_sock_vlan_val family "},
	{"help",api_lib_help,0,0,"show blapi_lib_os usage"},
	{NULL,NULL,0,0,NULL}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/



static int do_test_set_sock_vlan_val(int argc,char* argv[],void *p)
{
	//![Set the SKB_VLAN_ID option on socket]
	if(argc < 2 || argc >3)
		goto ErrorHandle;
		
	int family = strcmp(argv[1],"AF_INET") == 0?AF_INET:AF_INET6;
	char *endPtr = NULL;
	unsigned short old_vlan_skb_val = -1;
	unsigned short new_vlan_skb_val = strtoul(argv[2],&endPtr,0);
	
	
	int sockfd = socket(family,SOCK_DGRAM,0);
	if(sockfd < 0)
	{
		printf("Create socket error\n");
		return -1;
	}
	
	if(blapi_os_get_sock_vlan_val(sockfd,family,&old_vlan_skb_val) < 0)
	{
		printf("Get default sock vlan value error\n");
        close(sockfd);
		return -1;
	}
	printf("The default value of SKB_VLAN_ID for %s is %d\n",argv[1],old_vlan_skb_val);
	
	if(blapi_os_set_sock_vlan_val(sockfd,family,&new_vlan_skb_val) < 0)
	{
		printf("Set sock vlan value error\n");
        close(sockfd);
		return -1;
	}
	printf("Set sock SKB_VLAN_ID for %s to %d done.\n",argv[1],new_vlan_skb_val);
	//![Set the SKB_VLAN_ID option on socket]
	
	if(blapi_os_get_sock_vlan_val(sockfd,family,&new_vlan_skb_val) < 0)
	{
		printf("Get new sock vlan value error\n");
        close(sockfd);
		return -1;
	}
	printf("The new value of SKB_VLAN_ID for %s is %d\n",argv[1],new_vlan_skb_val);
	
    close(sockfd);
	
	return 0;
	
		
ErrorHandle:
	printf("do_test_set_sock_vlan_val():wrong parameters.\r\n"
           "    usage:\r\n"
           "    blapi_lib_os_demo test blapi_lib_os_set_sock_vlan_val ...(AF_INET/AF_INET6) ...(vlan_val)\r\n");
	return -1;
}


static int do_test_get_sock_vlan_val(int argc,char* argv[],void *p)
{
	if(argc != 2)
		goto ErrorHandle;
		
	//![Get the SKB_VLAN_ID option on socket]
	int family = strcmp(argv[1],"AF_INET") == 0?AF_INET:AF_INET6;
	time_t t;
	unsigned short old_vlan_skb_val = -1;


    int rand_num = 0;
    int rand_fd = open("/dev/urandom",O_RDONLY);
    if(rand_fd < 0)
    {
        printf("Open /dev/urandom error\n");
        return -1;
    }
    if(read(rand_fd,(char *)&rand_num,sizeof(int)) != sizeof(int))
    {
        printf("read random number from /dev/urandom error\n");
        close(rand_fd);
        return -1;
    }
    close(rand_fd);
    
	unsigned short new_vlan_skb_val = rand_num % 50 + 1;
	
	
	int sockfd = socket(family,SOCK_DGRAM,0);
	if(sockfd < 0)
	{
		printf("Create socket error\n");
		return -1;
	}
	
	if(blapi_os_get_sock_vlan_val(sockfd,family,&old_vlan_skb_val) < 0)
	{
		printf("Get default sock vlan value error\n");
        close(sockfd);
		return -1;
	}
	printf("The default value of SKB_VLAN_ID for %s is %d\n",argv[1],old_vlan_skb_val);
	
	//![Get the SKB_VLAN_ID option on socket]
	if(blapi_os_set_sock_vlan_val(sockfd,family,&new_vlan_skb_val) < 0)
	{
		printf("Set sock vlan value error\n");
        close(sockfd);
		return -1;
	}
	printf("Set sock SKB_VLAN_ID for %s to %d done.\n",argv[1],new_vlan_skb_val);
	
	if(blapi_os_get_sock_vlan_val(sockfd,family,&new_vlan_skb_val) < 0)
	{
		printf("Get new sock vlan value error\n");
        close(sockfd);
		return -1;
	}
	printf("The new value of SKB_VLAN_ID for %s is %d\n",argv[1],new_vlan_skb_val);
	
    close(sockfd);
	
	return 0;
	
		
ErrorHandle:
	printf("do_test_get_sock_vlan_val():wrong parameters.\r\n"
           "    usage:\r\n"
           "    blapi_lib_os_demo test blapi_lib_os_get_sock_vlan_val ...(AF_INET/AF_INET6)");
	return -1;
}





int os_test(int argc,char* argv[],void *p)
{
    return subcmd(blapi_os_test_cmd,argc,argv,(void *)blapi_os_test_cmd);
}



