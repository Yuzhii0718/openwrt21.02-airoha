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
#ifndef _BLAPI_TRAFFIC_CMD_H
#define _BLAPI_TRAFFIC_CMD_H

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

typedef struct PPTPEthernetUNI4Pmcounters_s
{
	unsigned int  transmitted_traffic;
	unsigned int  received_traffic;
	unsigned int  transmitted_rate;
	unsigned int  received_rate;
	unsigned int  transmitted_Octets;
	unsigned int  received_Octets;
	unsigned int  transmitted_dis_cout;
	unsigned int  received_dis_cout;
	unsigned int  transmitted_err_cout;
	unsigned int  received_err_cout;
}PPTPEthernetUNI4Pmcounters_t;

int do_blapi_traffic_cmd(int argc,char* argv[],void *p);
int do_blapi_traffic_set_black_ip_port_protocol(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wan_ratelimit(int argc, char *argv[], void *p);
int do_blapi_traffic_get_multicast_acnt(int argc, char *argv[], void *p);
int do_blapi_traffic_get_wan_acnt(int argc, char *argv[], void *p);
#if defined(TCSUPPORT_ACCOUNT_METER_V2)
int do_blapi_traffic_get_wan_acnt_by_name(int argc, char *argv[], void *p);
#endif
int do_blapi_traffic_get_flowid_cnt(int argc, char *argv[], void *p);
int do_blapi_traffic_get_llid_cnt(int argc, char *argv[], void *p);
int do_blapi_traffic_get_flowid_byte_pkt_cnt(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wan_acnt_enable(int argc, char *argv[], void *p);
int do_blapi_traffic_set_flow_acnt_enable(int argc, char *argv[], void *p);
int do_blapi_traffic_set_llid_acnt_enable(int argc, char *argv[], void *p);
int do_blapi_traffic_set_dev_acnt_enable(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wan_rate_threshold(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wan_mtu_val(int argc, char *argv[], void *p);
int do_blapi_traffic_set_ratelimit_mode(int argc, char *argv[], void *p);
int do_blapi_traffic_set_flowid_ratelimit(int argc, char *argv[], void *p);
int do_blapi_traffic_set_dev_ratelimit(int argc, char *argv[], void *p);
int do_blapi_traffic_set_max_bandwidth_data(int argc, char *argv[], void *p);
int do_blapi_traffic_get_dev_bandwidth(int argc, char *argv[], void *p);
int do_blapi_traffic_get_dev_acnt_by_mac(int argc, char *argv[], void *p);
int do_blapi_traffic_set_traffic_class(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wifi_interface_ratelimit(int argc, char *argv[], void *p);
int do_blapi_traffic_check_loop_detect(int argc, char *argv[], void *p);
int do_blapi_traffic_set_switch_linkmode(int argc, char *argv[], void *p);
int do_blapi_traffic_set_broadcast_ratelimit(int argc, char *argv[], void *p);
int do_blapi_traffic_set_etherwan_portmap(int argc, char *argv[], void *p);
int do_blapi_traffic_set_general_rx_ratelimit(int argc, char *argv[], void *p);
int do_blapi_traffic_get_ether_port_num(int argc, char *argv[], void *p);
int do_blapi_traffic_set_black_ip_for_hwnat(int argc, char *argv[], void *p);
int do_blapi_traffic_set_alg_rtsp_port(int argc, char *argv[], void *p);
int do_blapi_traffic_set_default_wan_interface(int argc, char *argv[], void *p);
int do_blapi_traffic_uninstall_mirror_traffic(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wanInterface_qostype(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wanInterface_qos_template(int argc, char *argv[], void *p);
#if defined(TCSUPPORT_CT_DS_LIMIT)
int do_blapi_traffic_set_dslimit_data(int argc, char *argv[], void *p);
#endif
int do_blapi_traffic_set_lan_info(int argc, char *argv[], void *p);
int do_blapi_traffic_check_eth_link_status(int argc, char *argv[], void *p);
int do_blapi_traffic_check_eth_port_status(int argc, char *argv[], void *p);
int do_blapi_traffic_get_portrate_info(int argc, char *argv[], void *p);
int do_blapi_traffic_get_lanport_info(int argc, char *argv[], void *p);
int do_blapi_traffic_get_wan_tx_stats(int argc, char *argv[], void *p);
int do_blapi_traffic_get_wan_rx_stats(int argc, char *argv[], void *p);
int do_blapi_traffic_get_pppoe_info(int argc, char *argv[], void *p);
int do_blapi_traffic_get_port_rx_byte_cnt(int argc, char *argv[], void *p);
int do_blapi_traffic_get_port_tx_byte_cnt(int argc, char *argv[], void *p);
int do_blapi_traffic_get_ether_wan_state(int argc, char *argv[], void *p);
int do_blapi_traffic_get_active_ether_wan_state(int argc, char *argv[], void *p);
int do_blapi_traffic_get_hsgmii_lan_state(int argc, char *argv[], void *p);
int do_blapi_traffic_get_hsgmii_lan_tx_stats(int argc, char *argv[], void *p);
int do_blapi_traffic_get_hsgmii_lan_rx_stats(int argc, char *argv[], void *p);
int do_blapi_traffic_get_ratelimit_mode(int argc, char *argv[], void *p);
int do_blapi_traffic_set_vip(int argc, char *argv[], void *p);
int do_blapi_traffic_del_vip(int argc, char *argv[], void *p);
int do_blapi_traffic_nptv6(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wan_mtu(int argc, char *argv[], void *p);
int do_blapi_traffic_get_lan_pm_stats(int argc, char *argv[], void *p);
int do_blapi_traffic_get_wan_pm_stats(int argc, char *argv[], void *p);

int do_blapi_traffic_set_assure_max_bandwidth(int argc, char *argv[], void *p);
	
int do_blapi_traffic_set_sla_cfg(int argc, char *argv[], void *p);

int do_blapi_traffic_mdio_read(int argc, char *argv[], void *p);
int do_blapi_traffic_mdio_write(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wan_acnt_clear(int argc, char *argv[], void *p);
int do_blapi_traffic_add_vlan_filter_vlan(int argc, char *argv[], void *p);
int do_blapi_traffic_del_vlan_filter_vlan(int argc, char *argv[], void *p);
int do_blapi_traffic_get_port_rx_cnt(int argc, char *argv[], void *p);
int do_blapi_traffic_get_port_tx_cnt(int argc, char *argv[], void *p);
int do_blapi_traffic_set_hsgmii_port_ratelimit(int argc, char *argv[], void *p);
int do_blapi_traffic_get_wan_port_statistics(int argc, char *argv[], void *p);
int do_blapi_traffic_get_wan_port_link_state(int argc, char *argv[], void *p);
int do_blapi_traffic_set_wan_port_link_mode(int argc, char *argv[], void *p);

int do_blapi_traffic_get_llid_statistics(int argc, char *argv[], void *p);
int do_blapi_traffic_get_llid_state(int argc, char *argv[], void *p);
int do_blapi_traffic_set_llid_mode(int argc, char *argv[], void *p);
int do_blapi_traffic_set_dev_stat_ratelimit_mode(int argc, char *argv[], void *p);

#endif /*_BLAPI_TRAFFIC_CMD_H*/

