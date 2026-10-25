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
#ifndef _BLAPI_EXAMPLE_INTERNAL_H
#define _BLAPI_EXAMPLE_INTERNAL_H


/**
* @mainpage Business Logic APIs (BLAPI)
* <table>
* <tr><th> Project   <td> BLAPI
* <tr><th> Source   <td> tclinux_phoenix/app_bsp/private/API/ecnt_blapi
* <tr><th> Version   <td> 1.0.0
* <tr><th> Date   <td> 2022-06-08
* </table>
*/

/**
* @file  blapi_example_internal.h  
* @brief An example of internal defines, declarations, structures, etc.
* @author Bryan.Sun
* @date 2019-09-06 
* @version 1.0.0 
* @copyright Airoha Inc                                                              
*/




/************************************************************************
*               I N C L U D E S
*************************************************************************
*/

#include <string.h>
#include <linux/version.h>


/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*               M A C R O S
*************************************************************************
*/

/**
 * This is a macro define example. 
 */
#define MAX(a,b) (((a)>(b))?(a):(b))

/**
 *  If kernel version is higher than 3.18.21,the value of this macro will be 1;otherwise it will be 0. 
 */	
#define KERNEL_3_18_21 (LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,21))

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/




/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/



/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/
void blapi_lib_example_utility_print_person_info(struct Person *pPtr);

#endif /*_BLAPI_EXAMPLE_INTERNAL_H*/
