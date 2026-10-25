/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include "eth_lan.h"

#include "./tcphy/phy_api.h"
#include "./tcphy/mtkswitch_api.h"
#include "linux/libcompileoption.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

extern uint8 ratelimit_En ;
extern unsigned char mac_sync_en;
extern char lan_port_map[];
extern char extend_switch_port;
extern char lan_port_map_ext[];

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int eth_mdio_read(struct ECNT_ETHER_Data *eth_data)
{
    eth_data->ether_private.phy.data = tcMiiStationRead(eth_data->ether_private.phy.add,
                                            eth_data->ether_private.phy.reg);
    return 0;
}

int eth_mdio_write(struct ECNT_ETHER_Data *eth_data)
{
    tcMiiStationWrite(eth_data->ether_private.phy.add,
        eth_data->ether_private.phy.reg,
        eth_data->ether_private.phy.data);
    return 0;
}

int eth_mac_send(struct ECNT_ETHER_Data *eth_data)
{
    macSend(WAN2LAN_CH_ID,eth_data->ether_private.skb);
    return 0;
}

int eth_gsw_pbus_read(struct ECNT_ETHER_Data *eth_data)
{
    eth_data->ether_private.phy.data = gswPbusRead(eth_data->ether_private.phy.add);
    return 0;
}

int eth_gsw_pbus_write(struct ECNT_ETHER_Data *eth_data)
{
    eth_data->retValue = gswPbusWrite(eth_data->ether_private.phy.add, eth_data->ether_private.phy.data);
    return 0;
}

int eth_ext_gsw_pbus_read(struct ECNT_ETHER_Data *eth_data)
{
    eth_data->ether_private.phy.data = gswPbusRead2(eth_data->ether_private.phy.add);
    return 0;
}

int eth_ext_gsw_pbus_write(struct ECNT_ETHER_Data *eth_data)
{
    eth_data->retValue = gswPbusWrite2(eth_data->ether_private.phy.add, eth_data->ether_private.phy.data);
    return 0;
}

/*ratelimit_En == 1: means downstream total bandwidth ratelimit works*/
int eth_set_ratelimit_switch(struct ECNT_ETHER_Data *eth_data)
{
	unsigned char tmp_En = eth_data->ether_private.ratelimit_En ;

	/*1. set the switch_value for channel selection, send to channel 0 or send channel by port mask*/
	ratelimit_En = tmp_En ;

	/*2. set flow control enable or not , port6*/
	macMT7530SetMACForceFC( 6 , (!tmp_En) );

	/*3. clear hw_enty*/
#if defined(TCSUPPORT_RA_HWNAT_ENHANCE_HOOK)
	{
		extern int (*ra_sw_nat_hook_clean_table)(void);
		if (ra_sw_nat_hook_clean_table)
		{
			ra_sw_nat_hook_clean_table();
		}
	}
#endif

    return 0;
}

int eth_set_mactable_sync_en(struct ECNT_ETHER_Data *eth_data)
{
    mac_sync_en = eth_data->ether_private.enable ;

    return 0;
}

int rgmii_traffic_set(struct ECNT_ETHER_Data *eth_data)
{
    rgmii_setting(eth_data->ether_private.traffic_setting.wan_type, 
        eth_data->ether_private.traffic_setting.interface);
    return 0;
}

int rgmii_mode(struct ECNT_ETHER_Data *eth_data)
{
    if(eth_data->ether_private.traffic_setting.interface == RGMII){
        rgmii_disable(eth_data->ether_private.traffic_setting.mode);
    }
    return 0;
}

int eth_set_port_matrix(struct ECNT_ETHER_Data *eth_data)
{
	int group[6];
    int type;
	int i = 0;

	for(i = 0;i < 6;i++){
		group[i] = eth_data->ether_private.matrix_setting.portMatrixGroup[i];
        }
	type = eth_data->ether_private.matrix_setting.type;
	macMT7530SetPortMatrix(group, type);
	
	return 0;
}

int eth_get_port_map(struct ECNT_ETHER_Data *eth_data)
{
	unsigned char lan_port = eth_data->ether_private.port_map.lan_port;

	if((support_external_switch()) && (lan_port>4))
	{
		eth_data->ether_private.port_map.switch_port = macextLanPortMap2Switch(lan_port);
		return 0;
	}
	eth_data->ether_private.port_map.switch_port = macMT7530LanPortMap2Switch(lan_port);

	return 0;
}

int eth_rx_for_lro(struct ECNT_ETHER_Data *eth_data)
{
	eth_rx(eth_data->ether_private.rx_info.msg_p,
		eth_data->ether_private.rx_info.msg_len,
		eth_data->ether_private.rx_info.skb,
		eth_data->ether_private.rx_info.rx_len);
	return 0;
}

int eth_set_port_linkstate(struct ECNT_ETHER_Data *eth_data)
{
	unsigned char port_no = eth_data->ether_private.port_state.port_no;
	unsigned char linkstate = eth_data->ether_private.port_state.linkstate;

	macMT7530SetPortLinkState(port_no,linkstate);
	
	return 0;
}

int eth_set_per_vlan_action(struct ECNT_ETHER_Data *eth_data)
{
    unsigned char port_id = eth_data->ether_private.vlantable_setting.port_id;
    unsigned int o_vid = eth_data->ether_private.vlantable_setting.o_vid;
    unsigned int n_vid = eth_data->ether_private.vlantable_setting.n_vid;
    ECNT_SWITCH_VLAN_MODE vlan_mode = eth_data->ether_private.vlantable_setting.vlan_mode;
    unsigned char enable = eth_data->ether_private.vlantable_setting.enable;

    macMT7530SetPerVlanAction(port_id, o_vid, n_vid, vlan_mode, enable);
    
    return 0;
}

int eth_set_per_port_vlan_action(struct ECNT_ETHER_Data *eth_data)
{
    unsigned char port_id = eth_data->ether_private.vlantable_setting.port_id;
    unsigned int n_vid = eth_data->ether_private.vlantable_setting.n_vid;
    ECNT_SWITCH_VLAN_MODE vlan_mode = eth_data->ether_private.vlantable_setting.vlan_mode;
    unsigned char enable = eth_data->ether_private.vlantable_setting.enable;

    macMT7530SetPerPortVlanAction(port_id, n_vid, vlan_mode, enable);
    
    return 0;
}

int eth_clean_mactable(struct ECNT_ETHER_Data *eth_data)
{
	macMT7530CleanMactbl();
	return 0;
}

int eth_use_qdma_wan(struct ECNT_ETHER_Data *eth_data)
{
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
    int set_wan_mode = eth_data->ether_private.set_wan_mode;
	ether_wan_use_qdma_wan(set_wan_mode);
#endif
	return 0;
}
int eth_ctag_white_list_mode(struct ECNT_ETHER_Data *eth_data)
{
	u8 type = eth_data->ether_private.ctag_white_list.mode;
	macMT7530ctagWhiteListMode(type);
	return 0;
}
int eth_ctag_white_list_per_port_mode(struct ECNT_ETHER_Data *eth_data)
{
    u8 port = eth_data->ether_private.ctag_white_list.port;
    u8 type = eth_data->ether_private.ctag_white_list.mode;
    port = macMT7530LanPortMap2Switch(port);
	macMT7530ctagWhiteListPerPortMode(port,type);
	return 0;
}
int eth_ctag_white_list_add(struct ECNT_ETHER_Data *eth_data)
{
	u8 port = eth_data->ether_private.ctag_white_list.port;
	u16 vid = eth_data->ether_private.ctag_white_list.vid;
    port = macMT7530LanPortMap2Switch(port);
	macMT7530ctagWhiteListAdd(port,vid);
	return 0;
}
int eth_ctag_white_list_del(struct ECNT_ETHER_Data *eth_data)
{
	u8 port = eth_data->ether_private.ctag_white_list.port;
	u16 vid = eth_data->ether_private.ctag_white_list.vid;
    port = macMT7530LanPortMap2Switch(port);
	macMT7530ctagWhiteListDel(port,vid);
	return 0;
}

/*it is used to get switch phy addr form lan port id*/
int eth_get_phy_addr(struct ECNT_ETHER_Data *eth_data)
{
    unsigned int lan_port =  eth_data->ether_private.phy.data;
    unsigned int phyaddr = 0;

    if(lan_port >= MAX_ETH_ITF_NUM){
        eth_data->ether_private.phy.add = 0;
        return 0;
    }
        
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if(lan_port_map[lan_port -1] == extend_switch_port)
    {
        phyaddr = lan_port_map_ext[lan_port -1] + 24;
    }
    else if (isEN7526G)
    {
        phyaddr = lan_port_map[lan_port-1];
    }
    else
    {
        phyaddr = lan_port_map[lan_port-1]+8;
    }
#else
    if(isRT63365 || isMT7520G || isMT7525G ||isEN7526G || isFPGA)
        phyaddr = lan_port_map[lan_port-1];
    else
        phyaddr = lan_port_map[lan_port-1]+8;

#endif
    eth_data->ether_private.phy.add = phyaddr;
    return 0;

}

int eth_add_arl_diptbl(struct ECNT_ETHER_Data *eth_data)
{
    mt7530_switch_arl_diptbladd_t diptbl_info = {0};
    memcpy(&diptbl_info, &eth_data->ether_private.arl_diptbl_info, sizeof(mt7530_switch_arl_diptbladd_t));
    macMT7530SetArlDipTblAdd(&diptbl_info);

    return 0;
}

int eth_add_arl_siptbl(struct ECNT_ETHER_Data *eth_data)
{
    mt7530_switch_arl_siptbladd_t siptbl_info = {0};
    memcpy(&siptbl_info, &eth_data->ether_private.arl_siptbl_info, sizeof(mt7530_switch_arl_siptbladd_t));
    macMT7530SetArlSipTblAdd(&siptbl_info);

    return 0;

}

int eth_add_arl_iptbl_multi(struct ECNT_ETHER_Data *eth_data)
{
    mt7530_switch_arl_mul_iptbladd_t multi_info = {0};
    memcpy(&multi_info, &eth_data->ether_private.arl_mul_iptbl_info, sizeof(mt7530_switch_arl_mul_iptbladd_t));
    macMT7530SetMulArlIpTblAdd(&multi_info);

    return 0;
}

int eth_get_per_port_mib_counter(struct ECNT_ETHER_Data *eth_data)
{
    mt7530_switch_api_MibCntType MibCntType = eth_data->ether_private.mib_counter.MibCntType;
    unsigned char port_id = eth_data->ether_private.mib_counter.port_id;
    unsigned long long cnt = 0;
    
    /* turn from lan port to switch port*/
    macMT7530GetMIBCounter(MibCntType, macMT7530LanPortMap2Switch(port_id), &cnt);
    
    eth_data->ether_private.mib_counter.cnt = cnt;
    
    return 0;
}

int eth_get_drop_crc_counter(struct ECNT_ETHER_Data *eth_data)
{
	unsigned char port_id = eth_data->ether_private.drop_crc_counter.port_id;
	unsigned int reg, val;

	/* turn from lan port to switch port*/
	port_id = macMT7530LanPortMap2Switch(port_id);

	reg = MIB_ID_RX_DROP_CNT + (0x100 * port_id);
	val = macMT7530ReadReg(reg);
	eth_data->ether_private.drop_crc_counter.rx_discard = val;

	reg = MIB_ID_TX_DROP_CNT + (0x100 * port_id);
	val = macMT7530ReadReg(reg);
	eth_data->ether_private.drop_crc_counter.tx_discard = val;

	reg = MIB_ID_RX_FCS_ERR_CNT + (0x100 * port_id);
	val = macMT7530ReadReg(reg);
	eth_data->ether_private.drop_crc_counter.rx_error = val;

	reg = MIB_ID_TX_CRC_CNT + (0x100 * port_id);
	val = macMT7530ReadReg(reg);
	eth_data->ether_private.drop_crc_counter.tx_error = val;

	return 0;
}

int eth_set_port_mirror(struct ECNT_ETHER_Data *eth_data)
{
	mt7530_switch_api_port_mirror_port_based_t portbased;
	unsigned char enable = eth_data->ether_private.port_state.linkstate;
	unsigned char port_no = eth_data->ether_private.port_state.port_no;

	portbased.port_tx_mir = 0;
	portbased.port_rx_mir = enable;
	portbased.acl_mir = enable;
	portbased.vlan_mis = enable;
	portbased.igmp_mir = enable;
	
	macMT7530SetPortMirrorEnable(enable, port_no);
	macMT7530SetPortMirrorPortBased(6, &portbased);
	
	return 0;
}

int eth_set_flow_control(struct ECNT_ETHER_Data *eth_data)
{
	unsigned char enable = eth_data->ether_private.port_state.linkstate;
	unsigned char port_no = eth_data->ether_private.port_state.port_no;
	
	macMT7530SetPortFlowControl(port_no, enable);
	
	return 0;
}

int eth_get_flow_control(struct ECNT_ETHER_Data *eth_data)
{
	unsigned char port_no = eth_data->ether_private.port_state.port_no;
	unsigned int val = 0;
	
	val = macMT7530GetPortFlowControl(port_no);
	eth_data->ether_private.port_state.linkstate = val;
	
	return 0;
}


