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
#ifndef _BLAPI_EXAMPLE_H
#define _BLAPI_EXAMPLE_H


/**
* \file  blapi_example.h  
* \brief This file is an example header file of business logic APIs that will be exported for others to use.
           Also this is an example for Doxygen compatible comment format. 
* \author Bryan.Sun
* \date     2019-09-06 
* \version  A001 
* \copyright EcoNet Inc                                                              
*/




/************************************************************************
*               I N C L U D E S
*************************************************************************
*/



/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/**
 *	\brief This is a typedef example 
 */
typedef unsigned int UNINT32;


/************************************************************************
*               M A C R O S
*************************************************************************
*/

/**
 *	\brief This is a macro example 
 */
#define Male 'M'
/**
 *	\brief This is another macro example 
 */
#define Famale 'F'


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/


/**
 * \brief This is a enum example.
 */
enum Career
{
	Programmer,  /*!<Career enumration element1.*/
	Teacher,     /*!<Career enumration element2.*/
	Doctor       /*!<Career enumration element3.*/
};

/**
 * \brief Define enum Career as a new Career type. 
 */
typedef enum Career Career;


/**
 * \brief This is a struct example.
 * 
 *  This struct describes a person information,which contains name,gender,age amd career.
 */
struct Person
{
    int id;
	char name[20]; /*!<The name of a person,the length should be less than 20.*/
	char gender; /*!<The gender of a person,should be 'M' or 'F'.*/
	UNINT32 age; /*!<The age of a person.It should be larger than 0.*/
	Career career; /*!<The career of a person.It should be a value from enum Career.*/
};



/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/** This is just a global variable example,
 *  you should not put a global variable in 
 *  the header file that will be provided to the API user 
 */
extern int global_var;





/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/



/**
 * \brief This function sets the information of a person.
 *
 * This function sets the information of a person,including name,gender and age.
 * \param[in]	pPtr The pointer to the struct Person that is to be filled.
 * \param[in]	name The name of the person,the length should be less than 20.
 * \param[in]	gender The gender of the person,the value can be 'M' or 'F'.
 * \param[in]	age The age of the person.
 * \param[in] career The career of the person,it should be one of the Career enum.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 * \par Example
 * \snippet blapi_lib_demo_example.c Set a person info
 */
int blapi_example_set_person_info(struct Person *pPtr,int id,char *name,char gender,UNINT32 age,Career career);



#endif /*_BLAPI_EXAMPLE_H*/
