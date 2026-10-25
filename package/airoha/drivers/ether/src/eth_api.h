/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
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
#ifndef _ETH_API_H
#define _ETH_API_H

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
int eth_mdio_read(struct ECNT_ETHER_Data *eth_data);
int eth_mdio_write(struct ECNT_ETHER_Data *eth_data);
int eth_mac_send(struct ECNT_ETHER_Data *eth_data);
int eth_gsw_pbus_read(struct ECNT_ETHER_Data *eth_data);
int eth_gsw_pbus_write(struct ECNT_ETHER_Data *eth_data);
int eth_ext_gsw_pbus_read(struct ECNT_ETHER_Data *eth_data);
int eth_ext_gsw_pbus_write(struct ECNT_ETHER_Data *eth_data);
int eth_set_ratelimit_switch(struct ECNT_ETHER_Data *eth_data);
int eth_set_mactable_sync_en(struct ECNT_ETHER_Data *eth_data);
int rgmii_traffic_set(struct ECNT_ETHER_Data *eth_data);
int rgmii_mode(struct ECNT_ETHER_Data *eth_data);
int eth_set_port_matrix(struct ECNT_ETHER_Data *eth_data);
int eth_get_port_map(struct ECNT_ETHER_Data *eth_data);
int eth_set_port_linkstate(struct ECNT_ETHER_Data *eth_data);
int eth_set_per_vlan_action(struct ECNT_ETHER_Data *eth_data);
int eth_set_per_port_vlan_action(struct ECNT_ETHER_Data *eth_data);
int eth_clean_mactable(struct ECNT_ETHER_Data *eth_data);
int eth_use_qdma_wan(struct ECNT_ETHER_Data *eth_data);
int eth_rx_for_lro(struct ECNT_ETHER_Data *eth_data);
int eth_ctag_white_list_mode(struct ECNT_ETHER_Data *eth_data);
int eth_ctag_white_list_per_port_mode(struct ECNT_ETHER_Data * eth_data);
int eth_ctag_white_list_add(struct ECNT_ETHER_Data *eth_data);
int eth_ctag_white_list_del(struct ECNT_ETHER_Data *eth_data);
int eth_get_phy_addr(struct ECNT_ETHER_Data *eth_data);
int eth_add_arl_diptbl(struct ECNT_ETHER_Data *eth_data);
int eth_add_arl_siptbl(struct ECNT_ETHER_Data *eth_data);
int eth_add_arl_iptbl_multi(struct ECNT_ETHER_Data *eth_data);
int eth_get_per_port_mib_counter(struct ECNT_ETHER_Data *eth_data);
int eth_get_drop_crc_counter(struct ECNT_ETHER_Data *eth_data);
int eth_set_port_mirror(struct ECNT_ETHER_Data *eth_data);
int eth_set_flow_control(struct ECNT_ETHER_Data *eth_data);
int eth_get_flow_control(struct ECNT_ETHER_Data *eth_data);

#endif /*_ETH_API_H*/
