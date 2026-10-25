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
#ifndef _TRAFFIC_DSLIMIT_H_
#define _TRAFFIC_DSLIMIT_H_

/**
* \file  traffic_dslimit.h  
* \brief Definiations and BLAPIs related to data speed limit.
* \author Zhen.Xu
* \date     2020-01-06
* \version  A001 
* \copyright Airoha Inc                                                              
*/

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <linux/qos_type.h>

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*               M A C R O S
*************************************************************************
*/
#define isdigit(x)	((x)>='0'&&(x)<='9')

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
typedef signed char 	int8;

typedef struct dslimit_param_s{
	int mode;
	int pair0;
	int pair1;
	int pair2;
	int pair3;
	int queue_ds;
	int iptype;
}dslimit_param_t, *dslimit_param_p;

/************************************************************************
*			D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*			F U N C T I O N   D E C L A R A T I O N S
			I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

#if defined(TCSUPPORT_CT_DS_LIMIT)
int dslimit_set_mode_and_queue_speed(int up_dw,int mode, int g_queue_speed[]);
int dslimit_get_queue_index_and_speed(int dataspeed, int g_queue_speed[]);
int dslimit_add_nodes_data(dslimit_param_t dslimitParam, struct dsnode g_dataspeed_nodes[], int g_queue_speed[]);
int dslimit_set_dataspeed_nodes(int up_dw, struct dsnode g_dataspeed_nodes[]);
int8 dslimit_get_interfaceLimitVal(char *val, int *limit);
int dalimit_get_vlanidLimitVal(char *val, int *limit);
int8 dslimit_get_rangeIPV4addr(char *v4addr, int *s_v4addr, int *e_v4addr);
int8 dslimit_get_rangeIPV6addr(char *v6addr,int *pair0, int *pair1, int *pair2, int *pair3);
int dslimit_get_iprangeLimitVal(char *val, int *pair0, int *pair1, int *pair2, int *pair3, int *limit);
int dslimit_parse_interface_value(int up_dw, char* lanIf, struct dsnode g_dataspeed_nodes[], int g_queue_speed[]);
int dslimit_parse_vlanID_value(char* vanID, struct dsnode g_dataspeed_nodes[], int g_queue_speed[]);
int dslimit_parse_ipRange_value(char* ipRange, struct dsnode g_dataspeed_nodes[], int g_queue_speed[]);
int dslimit_parse_dsValue(int updwMode, int dsMode, char* dsValue, struct dsnode g_dataspeed_nodes[], int g_queue_speed[]);
#endif

#endif /*_TRAFFIC_DSLIMIT_H_*/
