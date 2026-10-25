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
#ifndef _BLAPI_PERFORM_H
#define _BLAPI_PERFORM_H

/**
* \file  blapi_perform.h  
* \brief Definiations and BLAPIs related to system performance, such as optimization and performance evaluation.
* \author Zhen.Xu
* \date     2022-01-06
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

typedef struct vip_satus_s
{
	int protocol;
	int mode;
	unsigned int srcport;
	unsigned int dstport;
	int flag;
	unsigned short ringIdx;
	unsigned int swap;
}vip_satus_t, *vip_satus_p;

typedef struct _speed_test_result {
	char error_state[2];
	char dw_rate[16];
	char dw_max_rate[16];
	char up_rate[16];
	char up_max_rate[16];
}speed_test_result_t, *spped_test_result_p;

/************************************************************************
*               M A C R O S
*************************************************************************
*/
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

/**
 * \brief This function bind the cpuid of smbd process.
 *
 * This function bind the cpuid of smbd process. submit by Zhen.Xu 2019.12.04
 * \param[in]	flag 0:enable; 1:disable.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Bind the cpuid of smbd process.
 * \par Example
 * \snippet blapi_perform.c Bind the cpuid of smbd process
 */

int blapi_perform_samba_optimize(int flag);

/**
 * \brief This function bind the cpuid of ntfs-3g process.
 *
 * This function bind the cpuid of ntfs-3g process. submit by Shengqian.Wang 2020.04.23
 * \param[in]	flag 0:enable; 1:disable.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Bind the cpuid of ntfs-3g process.
 * \par Example
 * \snippet blapi_perform.c Bind the cpuid of ntfs-3g process
 */

int blapi_perform_ntfs_optimize(int flag);


/**
 * \brief This function set the affinity of qdma_wan0 in cpu.
 *
 * This function set the affinity of qdma_wan0 in cpu. submit by Zhen.Xu 2019.12.05
 * \param[flag]	flag 1:set the affinity of qdma_wan0 in cpu.;0:restore the affinity of qdma_wan0 in cpu.;
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ratelimit rule of device.
 * \par Example
 * \snippet blapi_perform.c set the affinity of qdma_wan0 in cpu.
 */

int blapi_perform_wget_optimize(int flag);

/**
 * \brief This function is used to get result time.
 *
 * This function  is used to get result time. submit by JinChao.Yao 2019.12.26
 * \param[out]rst:	speedtest result time
 * \param[in]	size: size of rst
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function is used to get result time.
 * \par Example
 * \snippet blapi_perform.c  This function is used to get result time.
 */

int blapi_perf_get_speedtest_result_time(char* rst, int size);

/**
 * \brief This function is used to get speed test result .
 *
 * his function is used to get speed test result .. submit by JinChao.Yao 2019.12.26
 * \param[out]result:	speed test result
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function is used to get speed test result .
 * \par Example
 * \snippet blapi_perform.c  This function is used to get speed test result .
 */

int blapi_perf_get_speedtest_result(spped_test_result_p result);


/**
 * \brief This function execute avalanche test.
 *
 * This function  execute avalanche test. submit by Zhen.Xu 20200107
 * \param[in]	flag: 1:start;0:stop.
 * \param[in]	xpon_flag: xpon flag.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function  execute avalanche test.
 * \par Example
 * \snippet blapi_perform.c  This function execute avalanche test.
 */

int blapi_perform_avaTest(int flag, int xpon_flag);

/**
 * \brief This function execute when voip state change.
 *
 * This function  execute when voip state change. submit by Jinhua.Zhang 20200109
 * \param[in]	voipIdle: 1:voip idle;0:voip busy.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function  execute when voip state change.
 * \par Example
 * \snippet blapi_perform.c  This function execute when voip state change.
 */

int blapi_perform_update_cpubind_byvoipstate(int voipIdle);

/**
 * \brief This function check whether ixia test.
 *
 * This function check whether ixia test. submit by xiang.nie 20200418
 * \retval	0	Success.
 * \note
 *  check whether ixia test and then run script.
 * \par Example
 * \snippet blapi_perform.c check whether ixia test.
 */
int blapi_perform_check_wifi_ixia_test(void);

/**
 * \brief This function execute avalanche tcp test.
 *
 * This function  execute avalanche tcp test. 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function  execute avalanche tcp test.
 * \par Example
 * \snippet blapi_perform.c  This function execute avalanche tcp test.
 */

int blapi_perform_avaTcpTest(void);


#endif /*_BLAPI_PERFORM_H*/
