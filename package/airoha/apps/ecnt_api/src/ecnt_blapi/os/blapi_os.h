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
THAT IT IS RECEIVER'SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER'S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _BLAPI_OS_H
#define _BLAPI_OS_H


/**
* \file  blapi_os.h  
* \brief Definiations and BLAPIs related to os/kernel.
* \author Bryan.Sun
* \date 2022-06-06 
* \version  V1.0.0 
* \copyright Airoha Inc                                                              
*/




/************************************************************************
*               I N C L U D E S
*************************************************************************
*/



/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define SOCK_FLAG_RCV_MAC			(1)
#define SOCK_FLAG_SOCK_MARK			(2)
#define SYSTEM_RAW  system
#define FPUTS_RAW		fputs
#define FPRINTF_RAW		fprintf

/************************************************************************
*               M A C R O S
*************************************************************************
*/



/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
enum itf_info_type
{
	ITF_TYPE_IP = 0,
	ITF_TYPE_MAC,
	ITF_TYPE_MASK
};



/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/




/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/



/**
 * \brief This function set the SKB_VLAN_ID option on socket.
 *
 * This function sets the SKB_VLAN_ID option for the socket referred to by the file descriptor sockfd using vlan_val.
 * IP_SKB_VLAN_ID or IPV6_SKB_VLAN_ID option will be set according to the value of family.
 * \param[in]	sockfd The file descriptor of the socket that is to be manipulated.
 * \param[in]	family The address domain of the socket,it can be AF_INET or AF_INET6.
 * \param[in]	vlan_val Pointer to the value that the SKB_VLAN_ID option will be set.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 * \par Example
 * \snippet main.c Set the SKB_VLAN_ID option on socket
 */
int blapi_os_set_sock_vlan_val(int sockfd,int family,unsigned short *vlan_val);

/**
 * \brief This function gets the SKB_VLAN_ID option on socket.
 *
 * This function gets the SKB_VLAN_ID option for the socket referred to by the file descriptor sockfd.The value will be put in vlan_val.
 * IP_SKB_VLAN_ID or IPV6_SKB_VLAN_ID option value will be get according to the value of family.
 * \param[in]	sockfd The file descriptor of the socket that is to be manipulated.
 * \param[in]	family The address domain of the socket,it can be AF_INET or AF_INET6.
 * \param[in]	vlan_val Pointer to the current SKB_VLAN_ID option value.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 * \par Example
 * \snippet main.c Get the SKB_VLAN_ID option on socket
 */
int blapi_os_get_sock_vlan_val(int sockfd,int family,unsigned short *vlan_val);

/**
 * \brief This function change current app to non-root
 *
 * This function will change app from root to non-root.
 * \param[in]	NONE
 * \note
 *  Write the things that need to pay attention to when using this function.
 * \
 */
void blapi_os_set_non_root_mode(void);


/**
 * \brief This function is used to get mac .
 *
 * This function is used to get mac .. 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get mac .
 * \par Example
 */
int blapi_os_get_dev_mac(char *itf_name, char *mac, int len);


 /**
 * \brief This function set the socket option for SOL_IP level
 *
 * This function sets the socket option for the socket (SOL_IP level)
*
 * \param[in]	socket The file descriptor of the socket that is to be manipulated.
 * \param[in]	type The sock option flag
 * \param[in]	option_vlaue The option value
 * \param[in]	option_len The option value len
 * \retval	0	Success.
 * \retval	< 0	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 * \par Example
 */
int blapi_os_set_sock_type(int socket, int type, const void *option_vlaue, int option_len);


 /**
 * \brief This function get the socket option for SOL_IP level
 *
 * This function gets the socket option for the socket (SOL_IP level)
*
 * \param[in]	socket The file descriptor of the socket that is to be manipulated.
 * \param[in]	type The sock option flag
 * \param[in]	option_vlaue The option value
 * \param[in]	option_len The option value len
 * \retval	0	Success.
 * \retval	< 0	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 * \par Example
 */
int blapi_os_get_sock_type(int socket, int type, const void *option_vlaue, int *option_len);
 

/**
* \brief This function get the interface information data.
*
* This function gets the information data from interface.
*
* \param[in]	 itf_name interface name
* \param[in]	 info_type information type
* \param[in]	 out_data output data.
* \param[in]	 out_data_len output data length.
* \retval  0	 Success.
* \retval  < 0 Fail.
* \note
*  Write the things that need to pay attention to when using this function.
* \par Example
*/
int blapi_os_get_itf_info(const char *itf_name, int info_type, unsigned char *out_data, unsigned char out_data_len);


/**
* \brief This function will keep system cmd parameter safe.
*
* This function covert injection code to normal string. 
*
* \param[in]	 input parameter string
* \param[out]	 out escape parameter string
* \param[in]	 out escape parameter length.
* \retval void
* \note
*  keep out escape parameter buffer is double size larger than parameter 
* \par Example
*/
void blapi_os_escape_parameter(char *parameter, char *escape_parameter, int escape_parameter_len);

/**
* \brief This function makes sure the input parameter is an IPv4 address.
*
* This function covert injection code to normal string. 
*
* \param[in]	 input parameter string
* \param[out]	 out escape parameter string
* \param[in]	 out escape parameter length.
* \retval void
* \note
*  keep out escape parameter buffer is double size larger than parameter 
* \par Example
*/
void blapi_os_check_ipv4(char *parameter, char *check_parameter, int check_parameter_len);

/**
* \brief This function makes sure the input parameter is an IPv4 address.
*
* This function covert injection code to normal string. 
*
* \param[in]	 input parameter string
* \param[out]	 out checked parameter string
* \param[in]	 out checked parameter length.
* \retval void
* \note
*  keep out escape parameter buffer is double size larger than parameter 
* \par Example
*/
void blapi_os_check_ipv6(char *parameter, char *check_parameter, int check_parameter_len);

/**
* \brief This function makes sure the input parameter is valid mac address.
*
* This function covert injection code to normal string. 
*
* \param[in]	 input parameter string
* \param[out]	 out checked parameter string
* \param[in]	 out checked parameter length.
* \retval void
* \note
*  keep out escape parameter buffer is double size larger than parameter 
* \par Example
*/
void blapi_os_check_mac(char *parameter, char *check_parameter, int check_parameter_len);

/**
* \brief This function makes sure the input parameter contains only digital numbers.
*
* This function covert injection code to normal string. 
*
* \param[in]	 input parameter string
* \param[out]	 out checked parameter string
* \param[in]	 out checked parameter length.
* \retval void
* \note
*  keep out escape parameter buffer is double size larger than parameter 
* \par Example
*/
void blapi_os_check_num(char *parameter, char *check_parameter, int check_parameter_len);

/**
* \brief This function makes sure the input parameter contains only alphabets.
*
* This function covert injection code to normal string. 
*
* \param[in]	 input parameter string
* \param[out]	 out checked parameter string
* \param[in]	 out checked parameter length.
* \retval void
* \note
*  keep out escape parameter buffer is double size larger than parameter 
* \par Example
*/
void blapi_os_check_alpha(char *parameter, char *check_parameter, int check_parameter_len);

#endif /*_BLAPI_LIB_OS_H*/
