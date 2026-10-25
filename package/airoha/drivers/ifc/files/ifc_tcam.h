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
#ifndef _IFC_TCAM_H_
#define _IFC_TCAM_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
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
#define IFC_ACK					(1<<31)
#define IFC_ID_SHIFT			(16)
#define IFC_ID_MASK				(0x1FF<<IFC_ID_SHIFT)
#define IFC_OFST_SHIFT			(8)
#define IFC_OFST_MASK			(0xf<<IFC_OFST_SHIFT)
#define IFC_SEL_TCAM			(0)
#define IFC_SEL_LUT1			(1)
#define IFC_SEL_ACT				(2)
#define IFC_SEL_MIB				(3)

#define IFC_SEL_SHIFT			(4)
#define IFC_SEL_MASK			(0x7<<IFC_SEL_SHIFT)
#define IFC_CMD_READ			(0)
#define IFC_CMD_WRITE			(1)
#define IFC_CMD_CLEAR			(2)
#define IFC_CMD_SHIFT			(1)
#define IFC_CMD_MASK			(0x3<<IFC_CMD_SHIFT)
#define IFC_REQ					(1<<0)

#define IFC_TCAM_RULE_NUM		(64)
#define NAME_STR_LEN			(16)
#define EN_BIT_OFFSET			(5)
#define RULE_MSG_LEN			(9)
#define ACTION_MSG_LEN			(6)
#define RULE_TYPE0				(0)
#define RULE_TYPE1				(1)
#define RULE_TYPE2				(2)
#define RULE_TYPE_MAX			(3)
#define RULE_TYPE_MASK			(0x3)
#define RULE_TYPE0_FIELD_NUM	(18)
#define RULE_TYPE1_FIELD_NUM	(24)
#define RULE_TYPE2_FIELD_NUM	(9)

#define IFC_MIB_CLEAR			(0)
#define IFC_MIB_GET				(1)
#define IFC_MIB_MAX_ID			(31)

#define	RULE_ACROSS				(1 << 4)

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
typedef	union{
	struct{
		#ifdef __BIG_ENDIAN
		uint rsv_unuse:24 ;
		uint rule_type:2 ;
		uint sip_eq_dip:1 ;
		uint spt_eq_dpt:1 ;
		uint outter_ipv4:1 ;
		uint outter_ipv6:1 ;
		uint ipv4_vld:1 ;
		uint ipv6_vld:1 ;
		#else
		uint ipv6_vld:1 ;
		uint ipv4_vld:1 ;
		uint outter_ipv6:1 ;
		uint outter_ipv4:1 ;
		uint spt_eq_dpt:1 ;
		uint sip_eq_dip:1 ;
		uint rule_type:2 ;
		uint rsv_unuse:24 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint gem_LLID:16 ;
		uint rsv1:3 ;
		uint snap:1 ;
		uint LLC_other:1 ;
		uint is_PPPoE:1 ;
		uint dmac_type:2 ;
		uint rsv2:2 ;
		uint vlan0_vld:1 ;
		uint vlan1_vld:1 ;
		uint vpm0:2 ;
		uint vpm1:2 ;
		#else
		uint vpm1:2 ;
		uint vpm0:2 ;
		uint vlan1_vld:1 ;
		uint vlan0_vld:1 ;
		uint rsv2:2 ;
		uint dmac_type:2 ;
		uint is_PPPoE:1 ;
		uint LLC_other:1 ;
		uint snap:1 ;
		uint rsv1:3 ;
		uint gem_LLID:16 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint vid0:12 ;
		uint pbit0:4 ;
		uint vid1:12 ;
		uint pbit1:4 ;
		#else
		uint pbit1:4 ;
		uint vid1:12 ;
		uint pbit0:4 ;
		uint vid0:12 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint PPPoE_session:16 ;
		uint etype:16 ;
		#else
		uint etype:16 ;
		uint PPPoE_session:16 ;
		#endif

		uint lut1_hit:32 ;

		uint DMAC_H:32 ;
		#ifdef __BIG_ENDIAN
		uint DMAC_L:16 ;
		uint SMAC_H:16 ;
		#else
		uint SMAC_H:16 ;
		uint DMAC_L:16 ;
		#endif
		uint SMAC_L:32 ;

		uint src_port:32 ;
	};
	uint msg[RULE_MSG_LEN];
}IFC_rule_type0;

typedef	union{
	struct{
		#ifdef __BIG_ENDIAN
		uint rsv_unuse:24 ;
		uint rule_type:2 ;
		uint sip_eq_dip:1 ;
		uint spt_eq_dpt:1 ;
		uint outter_ipv4:1 ;
		uint outter_ipv6:1 ;
		uint ipv4_vld:1 ;
		uint ipv6_vld:1 ;
		#else
		uint ipv6_vld:1 ;
		uint ipv4_vld:1 ;
		uint outter_ipv6:1 ;
		uint outter_ipv4:1 ;
		uint spt_eq_dpt:1 ;
		uint sip_eq_dip:1 ;
		uint rule_type:2 ;
		uint rsv_unuse:24 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint SA_ID:6 ;
		uint tcp_flags:9 ;
		uint dip_eq_atk:3 ;
		uint sip_eq_atk:3 ;
		uint rtsp_ctrl:1 ;
		uint L4_port_vld:1 ;
		uint L4_proto_vld:1 ;
		uint L4_proto:8 ;
		#else
		uint L4_proto:8 ;
		uint L4_proto_vld:1 ;
		uint L4_port_vld:1 ;
		uint rtsp_ctrl:1 ;
		uint sip_eq_atk:3 ;
		uint dip_eq_atk:3 ;
		uint tcp_flags:9 ;
		uint SA_ID:6 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint sport:16 ;
		uint dport:16 ;
		#else
		uint dport:16 ;
		uint sport:16 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint rsv4:4 ;
		uint flow_label:20 ;
		uint ttl:8 ;
		#else
		uint ttl:8 ;
		uint flow_label:20 ;
		uint rsv4:4 ;
		#endif

		uint lut1_hit:32 ;
		uint sipv4:32 ;
		uint dipv4:32 ;

		#ifdef __BIG_ENDIAN
		uint next_header:8 ;
		uint traffic_class:8 ;
		uint dscp:8 ;
		uint ip_protocol:8 ;
		#else
		uint ip_protocol:8 ;
		uint dscp:8 ;
		uint traffic_class:8 ;
		uint next_header:8 ;
		#endif

		uint src_port:32 ;
	};
	uint msg[RULE_MSG_LEN];
}IFC_rule_type1;

typedef	union{
	struct{
		#ifdef __BIG_ENDIAN
		uint rsv_unuse:24 ;
		uint rule_type:2 ;
		uint sip6_eq_dip6:1 ;
		uint spt_eq_dpt:1 ;
		uint outter_ipv4:1 ;
		uint outter_ipv6:1 ;
		uint ipv4_vld:1 ;
		uint ipv6_vld:1 ;
		#else
		uint ipv6_vld:1 ;
		uint ipv4_vld:1 ;
		uint outter_ipv6:1 ;
		uint outter_ipv4:1 ;
		uint spt_eq_dpt:1 ;
		uint sip6_eq_dip6:1 ;
		uint rule_type:2 ;
		uint rsv_unuse:24 ;
		#endif

		uint sipv6_w3:32 ;
		uint sipv6_w2:32 ;
		uint sipv6_w1:32 ;
		uint sipv6_w0:32 ;
		uint dipv6_w3:32 ;
		uint dipv6_w2:32 ;
		uint dipv6_w1:32 ;
		uint dipv6_w0:32 ;
	};
	uint msg[RULE_MSG_LEN];
}IFC_rule_type2;

typedef	union{
	struct{
		#ifdef __BIG_ENDIAN
		uint rule_en:1 ;
		uint rule_end:1 ;
		uint reverse:1 ;
		uint forceCPU:1 ;
		uint mib_en:1 ;
		uint rate_en:1 ;
		uint rate_only:1 ;
		uint rsv3:4 ;
		uint mib_id:5 ;
		uint rsv2:9 ;
		uint rate_id:7 ;
		#else
		uint rate_id:7 ;
		uint rsv2:9 ;
		uint mib_id:5 ;
		uint rsv3:4 ;
		uint rate_only:1 ;
		uint rate_en:1 ;
		uint mib_en:1 ;
		uint forceCPU:1 ;
		uint reverse:1 ;
		uint rule_end:1 ;
		uint rule_en:1 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint rsv1:17 ;
		uint nbq:5 ;
		uint owf:1 ;
		uint enable_h:9 ;
		#else
		uint enable_h:9 ;
		uint owf:1 ;
		uint nbq:5 ;
		uint rsv1:17 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint enable_low:5 ;
		uint dpi:1 ;
		uint kpcp:1 ;
		uint mc:1 ;
		uint fast:1 ;
		uint fqos:1 ;
		uint vpm0:2 ;
		uint vo1:2 ;
		uint vo0:2 ;
		uint pcp1:4 ;
		uint vid1:12 ;
		#else
		uint vid1:12 ;
		uint pcp1:4 ;
		uint vo0:2 ;
		uint vo1:2 ;
		uint vpm0:2 ;
		uint fqos:1 ;
		uint fast:1 ;
		uint mc:1 ;
		uint kpcp:1 ;
		uint dpi:1 ;
		uint enable_low:5 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint pcp0:4 ;
		uint vid0:12 ;
		uint traffic_class:8 ;
		uint dscp:8 ;
		#else
		uint dscp:8 ;
		uint traffic_class:8 ;
		uint vid0:12 ;
		uint pcp0:4 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint rsv0:1 ;
		uint acnt_grp2:7 ;
		uint acnt_grp1:5 ;
		uint acnt_grp0:6 ;
		uint meter_grp1:5 ;
		uint ndp:1 ;
		uint meter_grp0:7 ;
		#else
		uint meter_grp0:7 ;
		uint ndp:1 ;
		uint meter_grp1:5 ;
		uint acnt_grp0:6 ;
		uint acnt_grp1:5 ;
		uint acnt_grp2:7 ;
		uint rsv0:1 ;
		#endif

		#ifdef __BIG_ENDIAN
		uint meter_grp2:4 ;
		uint sp_tag:16 ;
		uint force_port:4 ;
		uint chnl:8 ;
		#else
		uint chnl:8 ;
		uint force_port:4 ;
		uint sp_tag:16 ;
		uint meter_grp2:4 ;
		#endif
	};
	uint msg[ACTION_MSG_LEN];
} IFC_TCAM_Action;

typedef	struct{
	int valid;
	unchar type;
	IFC_rule_type0 ruleType0;
	IFC_rule_type1 ruleType1;
	IFC_rule_type2 ruleType2;
}IFC_TCAM_Rule;

typedef struct{
	char name[NAME_STR_LEN];
	unchar rule_type;
	unchar type_changed;
	unchar action_en;
	uint ifc_idx;
	uint field_idx;
	uint field_key;
	uint field_mask;
	uint action_idx;
	uint action_value0;
	uint action_value1;
	uint action_value2;
}ifc_rule_para;

typedef void (*ifc_set_func)(ifc_rule_para *);
typedef void (*ifc_get_func)(ifc_rule_para *);


/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
*************************************************************************
*/
void ifc_dump_all(void);
void ifc_clear_all(void);
void ifc_get_stat(uint ifc_idx);
void ifc_read_act(uint ifc_idx, uint act_msg[]);
void ifc_read_rule(uint ifc_idx, uint key_msg[], uint mask_msg[]);
int ifc_rule_init_en7581(void);
int ifc_get_valid_idx(void);
int ifc_set_act(ifc_rule_para *para);
int ifc_set_rule(ifc_rule_para *para);
int ifc_set_rules_auto(struct ecnt_ifc_param *para);
int ifc_delete_rules_auto(struct ecnt_ifc_param *para);
int ifc_set_mib(uint mib_cmd, uint mib_id, struct ecnt_mib_cnt *cnt);
int ifc_get_rule_type(ifc_rule_para *para);
int ifc_set_atk_ip(int idx, int value, int mask);
int ifc_set_mac_para(int ifc_idx, int field_idx, int key_high, int mask_high, int key_low, int mask_low);
int ifc_set_rule_para(int ifc_idx, int field_idx, int field_key, int field_mask);
int ifc_set_act_para(int ifc_idx, int enable, int endflag, int action_en, int action_idx, int value0, int value1, int value2);

void ifc_set_field_dummy(ifc_rule_para *para);
void ifc_set_field_dmac(ifc_rule_para *para);
void ifc_set_field_smac(ifc_rule_para *para);
void ifc_set_field_dipv4(ifc_rule_para *para);
void ifc_set_field_sipv4(ifc_rule_para *para);
void ifc_set_field_dipv6_w0(ifc_rule_para *para);
void ifc_set_field_dipv6_w1(ifc_rule_para *para);
void ifc_set_field_dipv6_w2(ifc_rule_para *para);
void ifc_set_field_dipv6_w3(ifc_rule_para *para);
void ifc_set_field_sipv6_w0(ifc_rule_para *para);
void ifc_set_field_sipv6_w1(ifc_rule_para *para);
void ifc_set_field_sipv6_w2(ifc_rule_para *para);
void ifc_set_field_sipv6_w3(ifc_rule_para *para);
void ifc_set_field_flowLabel(ifc_rule_para *para);
void ifc_set_field_sif(ifc_rule_para *para);
void ifc_set_field_SNAP(ifc_rule_para *para);
void ifc_set_field_vpm0(ifc_rule_para *para);
void ifc_set_field_vpm1(ifc_rule_para *para);
void ifc_set_field_vid0(ifc_rule_para *para);
void ifc_set_field_pbit0(ifc_rule_para *para);
void ifc_set_field_vid1(ifc_rule_para *para);
void ifc_set_field_pbit1(ifc_rule_para *para);
void ifc_set_field_etype(ifc_rule_para *para);
void ifc_set_field_GEM_LLID(ifc_rule_para *para);
void ifc_set_field_ip_protocol(ifc_rule_para *para);
void ifc_set_field_next_header(ifc_rule_para *para);
void ifc_set_field_dscp(ifc_rule_para *para);
void ifc_set_field_traffic_class(ifc_rule_para *para);
void ifc_set_field_dport(ifc_rule_para *para);
void ifc_set_field_sport(ifc_rule_para *para);
void ifc_set_field_PPPoE_session(ifc_rule_para *para);
void ifc_set_field_dmac_type(ifc_rule_para *para);
void ifc_set_field_tcpflags(ifc_rule_para *para);
void ifc_set_field_L4Prtcol(ifc_rule_para *para);
void ifc_set_field_L4PrtcoVLD(ifc_rule_para *para);
void ifc_set_field_L4PortVLD(ifc_rule_para *para);
void ifc_set_field_outterIPv4(ifc_rule_para *para);
void ifc_set_field_outterIPv6(ifc_rule_para *para);
void ifc_set_field_IPv4VLD(ifc_rule_para *para);
void ifc_set_field_IPv6VLD(ifc_rule_para *para);
void ifc_set_field_LLCother(ifc_rule_para *para);
void ifc_set_field_isPPPoE(ifc_rule_para *para);
void ifc_set_field_VLan0VLD(ifc_rule_para *para);
void ifc_set_field_VLan1VLD(ifc_rule_para *para);
void ifc_set_field_LUT1_hit(ifc_rule_para *para);
void ifc_set_field_sip_eq_dip(ifc_rule_para *para);
void ifc_set_field_sport_eq_dport(ifc_rule_para *para);
void ifc_set_field_dip_eq_atk(ifc_rule_para *para);
void ifc_set_field_sip_eq_atk(ifc_rule_para *para);
void ifc_set_field_rtsp_ctrl(ifc_rule_para *para);
void ifc_set_field_sip6_eq_dip6(ifc_rule_para *para);
void ifc_set_field_SAID(ifc_rule_para *para);

void ifc_get_field_dummy(ifc_rule_para *para);
void ifc_get_field_dmac(ifc_rule_para *para);
void ifc_get_field_smac(ifc_rule_para *para);
void ifc_get_field_dipv4(ifc_rule_para *para);
void ifc_get_field_sipv4(ifc_rule_para *para);
void ifc_get_field_dipv6_w0(ifc_rule_para *para);
void ifc_get_field_dipv6_w1(ifc_rule_para *para);
void ifc_get_field_dipv6_w2(ifc_rule_para *para);
void ifc_get_field_dipv6_w3(ifc_rule_para *para);
void ifc_get_field_sipv6_w0(ifc_rule_para *para);
void ifc_get_field_sipv6_w1(ifc_rule_para *para);
void ifc_get_field_sipv6_w2(ifc_rule_para *para);
void ifc_get_field_sipv6_w3(ifc_rule_para *para);
void ifc_get_field_flowLabel(ifc_rule_para *para);
void ifc_get_field_sif(ifc_rule_para *para);
void ifc_get_field_SNAP(ifc_rule_para *para);
void ifc_get_field_vpm0(ifc_rule_para *para);
void ifc_get_field_vpm1(ifc_rule_para *para);
void ifc_get_field_vid0(ifc_rule_para *para);
void ifc_get_field_pbit0(ifc_rule_para *para);
void ifc_get_field_vid1(ifc_rule_para *para);
void ifc_get_field_pbit1(ifc_rule_para *para);
void ifc_get_field_etype(ifc_rule_para *para);
void ifc_get_field_GEM_LLID(ifc_rule_para *para);
void ifc_get_field_ip_protocol(ifc_rule_para *para);
void ifc_get_field_next_header(ifc_rule_para *para);
void ifc_get_field_dscp(ifc_rule_para *para);
void ifc_get_field_traffic_class(ifc_rule_para *para);
void ifc_get_field_dport(ifc_rule_para *para);
void ifc_get_field_sport(ifc_rule_para *para);
void ifc_get_field_PPPoE_session(ifc_rule_para *para);
void ifc_get_field_dmac_type(ifc_rule_para *para);
void ifc_get_field_tcpflags(ifc_rule_para *para);
void ifc_get_field_L4Prtcol(ifc_rule_para *para);
void ifc_get_field_L4PrtcoVLD(ifc_rule_para *para);
void ifc_get_field_L4PortVLD(ifc_rule_para *para);
void ifc_get_field_outterIPv4(ifc_rule_para *para);
void ifc_get_field_outterIPv6(ifc_rule_para *para);
void ifc_get_field_IPv4VLD(ifc_rule_para *para);
void ifc_get_field_IPv6VLD(ifc_rule_para *para);
void ifc_get_field_LLCother(ifc_rule_para *para);
void ifc_get_field_isPPPoE(ifc_rule_para *para);
void ifc_get_field_VLan0VLD(ifc_rule_para *para);
void ifc_get_field_VLan1VLD(ifc_rule_para *para);
void ifc_get_field_LUT1_hit(ifc_rule_para *para);
void ifc_get_field_sip_eq_dip(ifc_rule_para *para);
void ifc_get_field_sport_eq_dport(ifc_rule_para *para);
void ifc_get_field_dip_eq_atk(ifc_rule_para *para);
void ifc_get_field_sip_eq_atk(ifc_rule_para *para);
void ifc_get_field_rtsp_ctrl(ifc_rule_para *para);
void ifc_get_field_sip6_eq_dip6(ifc_rule_para *para);
void ifc_get_field_SAID(ifc_rule_para *para);

void ifc_set_action_chnl(ifc_rule_para *para);
void ifc_set_action_forcePort(ifc_rule_para *para);
void ifc_set_action_sptag(ifc_rule_para *para);
void ifc_set_action_meterGrp(ifc_rule_para *para);
void ifc_set_action_acntGrp(ifc_rule_para *para);
void ifc_set_action_dscp(ifc_rule_para *para);
void ifc_set_action_traficlass(ifc_rule_para *para);
void ifc_set_action_vlan0(ifc_rule_para *para);
void ifc_set_action_vlan1(ifc_rule_para *para);
void ifc_set_action_fqos(ifc_rule_para *para);
void ifc_set_action_fast(ifc_rule_para *para);
void ifc_set_action_multicast(ifc_rule_para *para);
void ifc_set_action_kpcp(ifc_rule_para *para);
void ifc_set_action_dpi(ifc_rule_para *para);
void ifc_set_action_forceCPU(ifc_rule_para *para);
void ifc_set_action_mib(ifc_rule_para *para);
void ifc_set_action_ratelimit(ifc_rule_para *para);
void ifc_set_action_ratelimit_only(ifc_rule_para *para);
void ifc_set_action_owf(ifc_rule_para *para);

#endif /* _IFC_TCAM_H_ */
