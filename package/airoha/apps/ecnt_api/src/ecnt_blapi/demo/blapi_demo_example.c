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
* @file  blapi_demo_example.c  
* @brief This file gives examples on using apis from blapi_example.h.
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

#include "blapi_example.h"
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

static int do_test_blapi_lib_example_set_person_info(int argc,char* argv[],void *p);


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/



static const cmds_t blapi_example_test_cmd[]=
{
	{"blapi_lib_example_set_person_info",do_test_blapi_lib_example_set_person_info,0,0,"test blapi_lib_example_set_person_info"},
	{"help",api_lib_help,0,0,"show blapi_lib_os usage"},
	{NULL,NULL,0,0,NULL}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

static int do_test_blapi_lib_example_set_person_info(int argc,char* argv[],void *p)
{
    if(argc != 6)
		goto ErrorHandle;
	//![Set a person info]
    int id = atoi(argv[1]);
    char *name = argv[2];
    char gender = argv[3][0];
    int age = atoi(argv[4]);

    const char *career_str[3] = {"Programmer","Teacher","Doctor"};
    
    
    enum Career career = Programmer;
    int idx = 0;
    for(idx = 0;idx<3;++idx)
        if(!strcmp(argv[5],career_str[idx]))
            career = Programmer+idx;

    

    struct Person person;
    
    blapi_example_set_person_info(&person,id,name,gender,age,career);
	//![Set a person info]
	
    return 0;
    
ErrorHandle:
	printf("do_test_example_api():wrong parameters.\r\n"
           "    usage:\r\n"
           "    blapi_lib_demo example blapi_lib_example_set_person_info id name gender age career\r\n");
	return -1;
}

int example_test(int argc,char* argv[],void *p)
{
    return subcmd(blapi_example_test_cmd,argc,argv,(void *)blapi_example_test_cmd);
}



