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
#ifndef _API_LIB_IFC_H
#define _API_LIB_IFC_H

int ifc_lib_set_vip_by_etype(unsigned short ether_ype);
int ifc_lib_set_vip_by_ipv4_proto(unsigned short proto);
int ifc_lib_set_vip_by_ipv6_nxthdr(unsigned short nxthdr);
int ifc_lib_set_vip_by_port(unsigned short sport,unsigned short dport, unsigned short ringIdx, unsigned int swap);
int ifc_lib_set_vip_by_sip_and_sport(unsigned int sip,unsigned short sport);
int ifc_lib_set_vip_by_dip_and_dport(unsigned int dip,unsigned short dport);
int ifc_lib_del_vip_by_etype(unsigned short ether_ype);
int ifc_lib_del_vip_by_ipv4_proto(unsigned short proto);
int ifc_lib_del_vip_by_ipv6_nxthdr(unsigned short nxthdr);
int ifc_lib_del_vip_by_port(unsigned short sport,unsigned short dport, unsigned int swap);
int ifc_lib_del_vip_by_sip_and_sport(unsigned int sip,unsigned short sport);
int ifc_lib_del_vip_by_dip_and_dport(unsigned int dip,unsigned short dport);

/* *************************************************************
* function description: used to set the ifc rule by pbit
* input parameters
*	unsigned char pbit: should be hex
* output parameters
*	none
* return value
*	0: success
*	-1: failed
***************************************************************/
int ifc_lib_set_flow_by_pbit(unsigned char pbit);


/* *************************************************************
* function description: used to set the ifc rule by IPv4 DSCP
* input parameters
*	unsigned char dscp: should be hex
* output parameters
*	none
* return value
*	0: success
*	-1: failed
***************************************************************/
int ifc_lib_set_flow_by_dscp(unsigned char dscp);


/* *************************************************************
* function description: used to set the ifc rule by IPv6 traffic_class
* input parameters
*	unsigned char tc: should be hex
* output parameters
*	none
* return value
*	0: success
*	-1: failed
***************************************************************/
int ifc_lib_set_flow_by_traffic_class(unsigned char tc);


/* *************************************************************
* function description: used to delete the ifc rule by pbit
* input parameters
*	unsigned char pbit: should be hex
* output parameters
*	none
* return value
*	0: success
*	-1: failed
***************************************************************/
int ifc_lib_del_flow_by_pbit(unsigned char pbit);


/* *************************************************************
* function description: used to delete the ifc rule by IPv4 DSCP
* input parameters
*	unsigned char dscp: should be hex
* output parameters
*	none
* return value
*	0: success
*	-1: failed
***************************************************************/
int ifc_lib_del_flow_by_dscp(unsigned char dscp);


/* *************************************************************
* function description: used to delete the ifc rule by IPv6 traffic_class
* input parameters
*	unsigned char tc: should be hex
* output parameters
*	none
* return value
*	0: success
*	-1: failed
***************************************************************/
int ifc_lib_del_flow_by_traffic_class(unsigned char tc);

#endif
