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
#ifndef _API_LIB_CPU_INTERRUPT_H
#define _API_LIB_CPU_INTERRUPT_H

/**
* \file  libapi_lib_cpu_interrupt.h  
* \brief This file is used to manipulate CPU interrupt binding.
* \author ECONET
* \date     2020-12-02
* \version  A001 
* \copyright EcoNet Inc                                                              
*/


/**
 * \brief This function is used to get the irq number according to interrupt name.
 *
 * This function is used to get the irq number according to interrupt name. submit by ECONET
 * \param[in]	intrName: interrupt name.
 * \retval	para.irqNum irq number.
 * \retval	-1	Fail.
 * \note
 *  Get the irq number according to interrupt name.
 * \par Example
 * \snippet api_lib_cpu_interrupt.c Get the irq number according to interrupt name.
 */
int cpu_interrupt_lib_get_irqNum_by_name(char* intrName);

/**
 * \brief This function is used to show interrupt names.
 *
 * This function is used to show interrupt names. submit by ECONET
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Show interrupt names.
 * \par Example
 * \snippet api_lib_cpu_interrupt.c Show interrupt names.
 */
int cpu_interrupt_lib_show_intrNames(void);

/**
 * \brief This function is used to check interrupt name.
 *
 * This function is used to check interrupt name. submit by ECONET
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Check intr name.
 * \par Example
 * \snippet api_lib_cpu_interrupt.c Check interrupt name.
 */
int cpu_interrupt_lib_check_intrName(char* intrName);
/**
 * \brief This function is used to get affinity by interrupt name.
 *
 * This function is used to get affinity by interrupt name. submit by ECONET
 * \param[in]	intrName: interrupt name.
 * \retval	cpu_idx cpu index.
 * \retval	-1	Fail.
 * \note
 *  Get affinity by interrupt name.
 * \par Example
 * \snippet api_lib_cpu_interrupt.c Get affinityCPU by name.
 */
int cpu_interrupt_lib_get_affinityCPU_by_name(char* intrName);
/**
 * \brief This function is used to transfer cpu mask to cpu index.
 *
 * This function is used to transfer cpu mask to cpu index. submit by ECONET
 * \param[in]	cpu_mask: cpu mask.
 * \retval	cpu_idx cpu index.
 * \retval	-1	Fail.
 * \note
 *  Transfer cpu mask to cpu index.
 * \par Example
 * \snippet api_lib_cpu_interrupt.c Transfer cpu mask to cpu index.
 */
int cpu_mask_to_cpu_num (int cpu_mask);
/**
 * \brief This function is used to bind interrupt name to specific cpu.
 *
 *  * \brief This function is used to bind interrupt name to specific cpu. submit by ECONET
 * \param[in]	cpu cpu index.
 * \param[in]	intrName: interrupt name.
 * \retval	 0	 Success.
 * \retval	-1	Fail.
 * \note
 *  Bind interrupt name to specific cpu.
 * \par Example
 * \snippet api_lib_cpu_interrupt.c Bind cpu by intr name.
 */
int cpu_interrupt_lib_bind_cpu_by_name(int cpu, char* intrName);
/**
 * \brief This function is used to check whether the IC is EN7523.
 *
 *  * \brief This function is used to check whether the IC is EN7523. submit by ECONET
 * \retval	 0	 is not EN7523.
 * \retval	1	is EN7523.
 * \note
 *  check whether the IC is EN7523.
 * \par Example
 * \snippet api_lib_cpu_interrupt.c check whether the IC is EN7523.
 */
int check_isen7523(void);

#endif
