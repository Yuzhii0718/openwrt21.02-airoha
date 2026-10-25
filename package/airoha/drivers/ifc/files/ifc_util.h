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
#ifndef _IFC_UTIL_H_
#define _IFC_UTIL_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_ifc.h>
#include "ifc_glb.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#ifdef TCSUPPORT_CPU_EN7523
#define IFC_ACT_MAX_IDX	16
#else
#define IFC_ACT_MAX_IDX	15
#endif

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
void dump_skb(struct sk_buff *skb) ;
uint getLowestBit(uint value) ;
int8_t isTPID(uint16_t eth_type) ; 
int32_t transTPIDtoVPM(uint16_t eth_type) ;
uint16_t transVPMtoTPID(uint16_t vpm) ;
unsigned int skb_chksum(const struct sk_buff *skb, int lens) ;
unsigned int ipv4_checksum(const struct sk_buff *skb, int offset) ;
int32_t getPppoeSid(struct sk_buff * skb, uint32_t pre_len, uint16_t * sid, uint16_t * ppp_tag) ;
unsigned int tcp_udp_checksum(const struct sk_buff *skb, int offset, uint16_t tcp_udp_length, uint8_t tcp_udp_protocol) ;
void get_lut0_mask_key(int index, uint Match[], uint Key[]) ;
void set_lut0_mask_key(int index, uint Match[], uint Key[]) ;
int checkIFCResult(int ifc_type, int index) ;
int write_all_ifc_rule_to_hw(void) ;
int clear_all_ifc_rules(void) ;
int clear_all_ifc_actions(void) ;

int clear_ifc_rules(int ruleId);
int ifc_get_rule_from_hw(int ruleId);
int ifc_get_rule_from_reg(int lut0Idx);
int ifc_enable(void);
int ifc_disable(void);
int ifc_get_ifc_staus(void);
int ifc_parser_lut0_rule(IFC_LUT0_Format *p_LUT0_Rule, int ruleId);
void set_lut0_mask_key_to_array(IFC_LUT0_Format *lut0Rule, uint Mask[], uint Key[]);

#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
int ifc_dump_fcpu_ring(int ifcIndex);
int ifc_set_fcpu_ring(int lut0Id,int fcpu, int rxq);
int ifc_clear_fcpu_ring(int lut0Id);
#endif

int ifc_dump_lut0_rules(void) ;
int ifc_dump_lut1_rules(void) ;
int ifc_dump_all_rules(void) ;
int ifc_set_lut0_rules(uint ifcIndex, IFC_Mode_t enMode, uint fieldIdx, uint command, uint mask_high, uint mask_low, uint key_high, uint key_low) ;
int ifc_set_lut1_rules(uint ifcIndex, IFC_Mode_t enMode, uint endFlag, uint fieldIdx, uint command, uint mask, uint key_high, uint key_low) ;
int ifc_set_action(uint ifcIndex, uint actIdx, IFC_Mode_t enMode, uint value0, uint value1, uint value2, uint value3) ;

#endif /* _IFC_UTIL_H_ */
