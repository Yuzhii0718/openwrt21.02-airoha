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
#ifndef _BLAPI_XDSL_CMD_H
#define _BLAPI_XDSL_CMD_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
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
/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/
/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

int do_blapi_xdsl_cmd(int argc,char* argv[],void *p);

int do_blapi_xdsl_reload_ko(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_fwver(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_tsarm(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_linenum(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_used_standard(int argc, char *argv[], void *p);

int do_blapi_xdsl_set_dslline_enable(int argc, char *argv[], void *p);

int do_blapi_xdsl_set_xtmline_enable(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_xtmline_enable(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_DDSLLineNode(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_DDSLLineStatsNode(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_DDSLChannelInfoNode(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_DDSLChannelStatsNode(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_DPTMLinkStatsNode(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_DATMLinkStatsNode(int argc, char *argv[], void *p);

int do_blapi_xdsl_ptm_do_reset_sequence(int argc, char *argv[], void *p);

int do_blapi_xdsl_get_interface_config_file(int argc, char *argv[], void *p);

int do_blapi_xdsl_atm_create_interface(int argc, char *argv[], void *p);

int do_blapi_xdsl_atm_delete_interface(int argc, char *argv[], void *p);

int do_blapi_xdsl_create_device(int argc, char *argv[], void *p);

int do_blapi_set_adsl_annex(int argc, char *argv[], void *p);
int do_blapi_set_adsl_low_power(int argc, char *argv[], void *p);
int do_blapi_set_adsl_aelem(int argc, char *argv[], void *p);
int do_blapi_set_adsl_testlab(int argc, char *argv[], void *p);
int do_blapi_set_adsl_sysvid(int argc, char *argv[], void *p);
int do_blapi_set_adsl_version(int argc, char *argv[], void *p);
int do_blapi_set_adsl_ginp(int argc, char *argv[], void *p);
int do_blapi_set_adsl_sra(int argc, char *argv[], void *p);
int do_blapi_set_adsl_gvector(int argc, char *argv[], void *p);
int do_blapi_set_adsl_bitswap(int argc, char *argv[], void *p);
int do_blapi_set_adsl_sos_roc(int argc, char *argv[], void *p);
int do_blapi_set_adsl_us0(int argc, char *argv[], void *p);
int do_blapi_set_adsl_dying_gasp(int argc, char *argv[], void *p);
int do_blapi_set_adsl_virtual_noise(int argc, char *argv[], void *p);
int do_blapi_set_adsl_mode(int argc, char *argv[], void *p);
int do_blapi_set_adsl_profile(int argc, char *argv[], void *p);
int do_blapi_get_adsl_mode(int argc, char *argv[], void *p);
int do_blapi_xdsl_set_power_up_down(int argc, char *argv[], void *p);
int do_blapi_sfp_set_power_up_down(int argc, char *argv[], void *p);
int do_blapi_get_adsl_annex(int argc, char *argv[], void *p);
int do_blapi_get_vdsl_interface_config(int argc, char *argv[], void *p);
int do_blapi_get_adsl_rx_bear_tpstc_type(int argc, char *argv[], void *p);
int do_blapi_get_adsl_attain_rate(int argc, char *argv[], void *p);
int do_blapi_get_adsl_ch_op_data(int argc, char *argv[], void *p);
int do_blapi_get_adsl_far_op_data(int argc, char *argv[], void *p);
int do_blapi_get_adsl_near_op_data(int argc, char *argv[], void *p);
int do_blapi_get_adsl_pms_param(int argc, char *argv[], void *p);
int do_blapi_get_adsl_pms_inp(int argc, char *argv[], void *p);
int do_blapi_adsl_get_oam_ping_status(int argc, char *argv[], void *p);
int do_blapi_get_adsl_wlan_dsl_inf_conf(int argc, char *argv[], void *p);
int do_blapi_get_adsl_defect_cnt(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cell_cnt1(int argc, char *argv[], void *p);
int do_blapi_get_adsl_near_ituid(int argc, char *argv[], void *p);
int do_blapi_get_adsl_statistic_2(int argc, char *argv[], void *p);
int do_blapi_get_adsl_last_drop_reason(int argc, char *argv[], void *p);
int do_blapi_get_adsl_far_ituid(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic0(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic1(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic2(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic3(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic4(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic5(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic6(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic7(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic8(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic9(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic10(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic11(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic12(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic13(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic14(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic15(int argc, char *argv[], void *p);
int do_blapi_get_adsl_cwmp_diagnostic16(int argc, char *argv[], void *p);

#endif /*_BLAPI_XDSL_CMD_H*/
