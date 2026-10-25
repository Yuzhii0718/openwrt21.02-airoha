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
#ifndef _BLAPI_TRAFFIC_H_
#define _BLAPI_TRAFFIC_H_

/**
* \file  blapi_traffic.h  
* \brief Definiations and BLAPIs related to data link/traffic. These APIs are designed for Airoha's SDK products only.
* \author zhang.lei
* \date     2020-01-03
* \version  V1.0.0 
* \copyright Airoha Inc                                                              
*/

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include "blapi_traffic_bsp.h"

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


typedef enum{
	DEL_TRAFFIC_CLASS=0,
	ADD_TRAFFIC_CLASS,
}Traffic_Flag_t;

typedef enum
{
	MCST_ACNT_MODE_BYTES = 0,
	MCST_ACNT_MODE_PACKET,
}ECNT_TRAFFIC_MCST_ACNT_MODE;

typedef enum
{
    FLOWID_RATELIMIT_DIR_UP = 0,
    FLOWID_RATELIMIT_DIR_DOWN,
}ECNT_FLOWID_RATELIMIT_DIR;


typedef struct{
	int valid;
	char ifname[16];
	unsigned char smac_add[6];
	unsigned char dmac_add[6];
	unsigned int sessionID;
	unsigned int src_ip;
	unsigned int dst_ip;
	int dns_valid;
	unsigned int pri_dns;
	unsigned int snd_dns;
}ECNT_TRAFFIC_PPPOE_INFO_DATA;


/************************************************************************
*               M A C R O S
*************************************************************************
*/
#define IFC_ADD (1)
#define IFC_DEL (2)


#define D_TYPE_IP_IP6	0
#define D_TYPE_PPP		1


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
enum IFC_TYPE{
	SIP_SPORT_CHG = 0,
	DIP_DPORT_CHG,
	IFC_TYPE_END,
};

enum HGSMII_LAN_PORT
{
	HSGMII_USB = 0x09,
	HSGMII_PCIE0,
	HSGMII_PCIE1,
	HSGMII_ETH,
};

typedef struct ifc_param_s{
	int ifc_type;
	unsigned long sip;
	unsigned short sport;
	unsigned long dip;
	unsigned short dport;
}ifc_param_t;


#define TRTCM_MATCH_MAC_RULE_NUM 10
#define FH_VLAN_FILTER_DEV_FILE	"/dev/fhvlanfilter"
#define VLAN_FILTER_LIST_ADD 0
#define VLAN_FILTER_LIST_DELETE 1

typedef struct lanport_info_s
{
	char link_rate[8][64];
	char lan_port[8][32];
	char lan_status[8][32];
}lanport_info_t, *lanport_info_p;

typedef struct qos_wanIf_value_s {
	int index;
	char wanType_name[64];
	char wanType_value[64];
	int wanType_protocol_type;
	int wanType_mask;
}qos_wanIf_value_t, *qos_wanIf_value_p;

typedef struct MaxBandWidth_node_info_s{
	char macAddr[32];
	char upRate[32];
	char downRate[32];
}MaxBandWidth_node_info_t, *MaxBandWidth_node_info_p;

struct SockInfo{	
	unsigned short ppe_magic;	
	unsigned short ppe_foe_entry;
	unsigned char ppe_ai;
	unsigned char wan_type;
	unsigned short wan_index;
};

typedef struct DevBandWidth_t{
	char mac[20];
	unsigned long long UpRate;
	unsigned long long DwRate;
}DevBandWidth_Info_t;


typedef struct _blapi_ob_monitor_data_s
{
	char ipaddr[40];
	unsigned char mac[6];
	unsigned short type; /* 0: IP/IPv6, 1: PPP */
	unsigned int mark;
}blapi_ob_monitor_data;

typedef struct _blapi_info_ioctl_OB_map_data_s
{
	int		wan_idx;
	int		data_len;
	void	*data;
}blapi_info_ioctl_OB_map_data;

enum vlanFilterListResult
{
	OP_SUCCESS,
	OP_FAIL,
	ADD_ALREADY_EXIST,
	DELETE_NOT_FOUND,
};

typedef struct vlanfilter_args_s 
{
	unsigned short vid;
	unsigned short port;
	enum vlanFilterListResult result;
}vlanfilter_args_t;


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
 * \brief This function add the black ip to hwnat.
 *
 * This function add black ip to hwnat, and let it do not go through hwnat.
 * \param[in]	ip
 * \param[in]	port
 * \param[in]	protocol
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_add_black_ip_port_protocol(unsigned int ip, unsigned int port, unsigned int protocol);

/**
 * \brief This function delete the black ip from hwnat.
 *
 * This function delete black ip from hwnat, and let it go through hwnat again.
 * \param[in]	ip
 * \param[in]	port
 * \param[in]	protocol
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_delete_black_ip_port_protocol(unsigned int ip, unsigned int port, unsigned int protocol);

/**
 * \brief This function delete all the black ip from hwnat.
 *
 * This function delete all black ip from hwnat, and let it go through hwnat again.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_delete_all_black_ip_port_protocol(void);

/**
 * \brief This function show black ip  port protocol from hwnat.
 *
 * This function show black ip port protocol from hwnat, and let it go through hwnat again.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_show_black_ip_port_protocol(void);

/**
 * \brief This function gets the pppoe information of PCs.
 *
 * This function gets the pppoe information of PCs. Such as interface name
 * \param[in]	pppoe_info_val Pointer to the info struct.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_get_pppoe_info_val(ECNT_TRAFFIC_PPPOE_INFO_DATA *pppoe_info_val);
 
 /**
 * \brief This function will set wan ratalimit(UP/DOWN).
 *
 * This function set the value of wan ratalimit(UP/DOWN). submit by ShengQian.Wang  2019.03.16
 * \param[in]	wanname: name of wan interface;
 * \param[in]	wanid: wan port index
 * \param[in]	up_rate: the value of upstream ratelimit
 * \param[in]	dn_rate: the value of downstream ratelimit
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the value of wan ratalimit(UP/DOWN).
 * \par Example
 * \snippet blapi_traffic.c Set the value of wan ratalimit(UP/DOWN).
 */
int blapi_traffic_set_wan_ratelimit(char *wanname, unsigned int wanid, unsigned int up_rate, unsigned int dn_rate);

/**
 * \brief This function get the bytes  according to type.
 *
 * This function get the bytes  of multicast. submit by Lei.Zhang 2019.12.16
 * \param[in]	type: The type of data you will get.  0:mulbytes, 1:packet
 * \param[out] bytes: The data you get.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the bytes  according to type.
 * \par Example
 * \snippet blapi_traffic.c Get the bytes  according to type.
 */
 
int blapi_traffic_get_multicast_acnt(ECNT_TRAFFIC_MCST_ACNT_MODE mode, unsigned long long *cnt);

/**
 * \brief This function get flow bytes according to the direction and flow id.
 *
 * This function get flow bytes according to the direction and flow id. submit by Lei.Zhang 2019.12.16
 * \param[in]	dir: flow direction. DOWN_DIR=0, UP_DIR=1
 * \param[in] flowid: flow ID.
 * \param[out] flag: flow bytes you get
 * \param[out] flag: 1:flow rate can be more than max, need to check outside
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get flow bytes according to the direction and flow id.
 * \par Example
 * \snippet blapi_traffic.c Get flow bytes according to the direction and flow id.
 */

int blapi_traffic_get_flowid_cnt(unsigned int dir, unsigned int flowid, unsigned long long *cnt, int* flag);

/**
 * \brief This function get flow bytes according to the direction and llid.
 *
 * \param[in]	dir: flow direction. DOWN_DIR=0, UP_DIR=1
 * \param[in] llid: llid ID.
 * \param[out] flag: llid bytes you get
 * \param[out] flag: 1:llid can be more than max, need to check outside
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get flow bytes according to the direction and llid id.
 * \par Example
 * \snippet blapi_traffic.c Get flow bytes according to the direction and llid id.
 */
 int blapi_traffic_get_llid_cnt(unsigned int dir, unsigned int llid, unsigned long long *cnt);

/**
 * \brief This function set wan rate use pps mode threshold.
 *
 * This function set wan rate threshold value.
 * \param[in]	wan rate limit threshold value.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *	Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_set_wan_rate_threshold(unsigned int threshold);

/**
 * \brief This function set wan type & MTU value.
 *
 * This function set wan type & MTU value.
 * \param[in]	wan_type, route or bridge; wan mtu value.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *	Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_set_wan_mtu_val(unsigned char wan_type, unsigned int wan_mtu);

/**
 * \brief This function will enable the rate statistics based on device mode.
 *
 * This function will enable rate statistics based on devices mode. submit by Lei.Zhang 2019.12.16
 * \param[in]	mode: 1:DEV_MODE, other:DEFAULT_NULL_MODE
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Enable the rate statistics based on devices mode.
 * \par Example
 * \snippet blapi_traffic.c Enable the rate statistics based on devices mode.
 */
int blapi_traffic_set_dev_acnt_enable(unsigned char sw_acnt_en, unsigned char hw_acnt_en);

/**
 * \brief This function set the bandwidth of lan device.
 *
 * This function enable/disable the ratelimit of wifi device. 
 * \param[in]	enable:  1 enable 0 disable.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  enable/disable the ratelimit of wifi device.
 * \par Example
 * \snippet blapi_traffic.c enable/disable the ratelimit of wifi device
 */
 
int blapi_traffic_set_wifi_dev_ratelimit_enable(int enable);

/**
 * \brief This function get the tx/rx bytes and mac info.
 *
 * This function get the tx/rx bytes and mac info . submit by ShengQian.Wang 2020.07.06
 * \param[out]	DevBandWidth_Info: DevBandwidth info 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the tx/rx bytes and mac info.
 * \par Example
 * \snippet blapi_traffic.c get the tx/rx bytes and mac info
 */

int blapi_traffic_get_dev_bandwidth(DevBandWidth_Info_t *DevBandWidth_Info);

/**
 * \brief This function get the tx/rx bytes by device mac.
 *
 * This function get the tx/rx bytes by device mac. submit by Zhen.Xu 2019.12.05
 * \param[in]	mac: The mac of sta
 * \param[in]	EnableStatus: The EnableStatus in the node of lanhost2_common
 * \param[out]	rxbytes: The rxbytes of device.
 * \param[out]	txbytes: The txbytes of device.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the tx/rx bytes by device mac.
 * \par Example
 * \snippet blapi_traffic.c get the tx/rx bytes by device mac
 */

int blapi_traffic_get_dev_acnt_by_mac(int EnableStats, char *mac, unsigned long long *rxbytes, unsigned long long *txbytes);

/**
 * \brief This function set the traffic class by type & flag.
 *
 * This function set the traffic class by type & flag. submit by Lei.Zhang 2019.12.24
 * \param[in]	type: traffic class type(Pbit/DSCP/TOS/VLANID)
 * \param[in]	minVal: the Min value.
 * \param[in]	maxVal: the Max value.
 * \param[in]	flag: DEL_TRAFFIC_CLASS=0,ADD_TRAFFIC_CLASS=1
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set the traffic class by type & flag.
 * \par Example
 * \snippet blapi_traffic.c set the traffic class by type & flag.
 */

int blapi_traffic_set_traffic_class(char* type, int minVal, int maxVal, int flag);

/**
 * \brief This function set wlan rate limit by wlan index & stream direction.
 *
 * This function set the set wlan rate limit by wlan index & stream direction. submit by Lei.Zhang 2019.12.24
 * \param[in]	dir: stream direction(up/down)
 * \param[in]	wifiIdx: wlan interface index
 * \param[in]	rate: limit rate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set wlan rate limit by wlan index & stream direction.
 * \par Example
 * \snippet blapi_traffic.c set wlan rate limit by wlan index & stream direction.
 */

int blapi_traffic_set_wifi_interface_ratelimit(unsigned int dir, unsigned char wifiIdx, unsigned int rate);

/**
 * \brief This function check the status of loop detece.
 *
 * This function check the status of loop detece. submit by Zhen.Xu 2019.12.24
 * \param[in]	ifname: eth0.x/ra/rai
 * \param[out]	Status: The status of eth0.x.
 * \param[out]	detectionStatus: The detectionStatus of eth0.x.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  check the status of loop detece.
 * \par Example
 * \snippet blapi_traffic.c check the status of loop detece.
 */

int blapi_traffic_check_loop_detect(char *ifname, char *Status, char *detectionStatus);

/**
 * \brief This function set the switch parameter of lan port.
 *
 * This function set the switch parameter of lan port.submit by Zhen.Xu 2019.12.24
 * \param[in]	switchpara_id :The index of lan port
 * \param[in]	maxbitrate: The max bit rate of lan port.
 * \param[in]	duplexmode: The duplex mode of lan port.
 * \param[in]	enable: The status of lan port.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * set the switch parameter of lan port.
 * \par Example
 * \snippet blapi_traffic.c set the switch parameter of lan port.
 */

int blapi_traffic_set_switch_linkmode(char *enable,int switchpara_id, char *maxbitrate, char *duplexmode);

/**
 * \brief This function set the switch auto parameter of lan port.
 *
 * This function set the switch parameter of lan port 
 * \param[in]	switchpara_id :The index of lan port
 * \param[in]	maxbitrate: The max bit rate of lan port.
 * \param[in]	duplexmode: The duplex auto mode of lan port.
 * \param[in]	enable: The status of lan port.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * set the switch parameter of lan port.
 * \par Example
 * \snippet blapi_traffic.c set the switch parameter of lan port.
 */
int blapi_traffic_set_switch_auto_linkmode(char *enable, int switchpara_id, char *maxbitrate, char *duplexmode);

/**
 * \brief This function set the broadcast ratelimit.
 *
 * This function set the broadcast ratelimit.submit by Zhen.Xu 2019.12.24
 * \param[in]	bsr_lan: The index of lan port
 * \param[in]	rate: The max rate of lan port.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set the broadcast ratelimit.
 * \par Example
 * \snippet blapi_traffic.c set the broadcast ratelimit.
 */

int blapi_traffic_set_broadcast_ratelimit(int bsr_lan, int rate);

/**
 * \brief This function set switch port mapping.
 *
 * This function set switch port mapping.submit by Zhen.Xu 2019.12.24
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set defualt switch port mapping.
 * \par Example
 * \snippet blapi_traffic.c set switch port mapping.
 */

int blapi_traffic_set_default_switch_portmap(void);

/**
 * \brief This function set switch port mapping.
 *
 * This function set switch port mapping.submit by ShengQian.Wang 2020.10.28 
 * \param[in]	portmap : portmap set in romfile 
 * \param[in]	portvlan : portvlan set in romfile 
  * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set switch port mapping from romfile.
 * \par Example
 * \snippet blapi_traffic.c set switch port mapping.
 */

int blapi_traffic_set_switch_portmap(char *portmap, char *portvlan);

/**
 * \brief This function set the number of lan port .
 *
 * This function set the number of lan port .submit by Zhen.Xu 2019.12.24
 * \param[in]	portNum : The number of lan port 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set the number of lan port .
 * \par Example
 * \snippet blapi_traffic.c set the number of lan port .
 */

int blapi_traffic_get_ether_port_num(int *portNum);

/**
 * \brief This function set the black ip of hwnat .
 *
 * This function set  the black ip of hwnat .submit by SS.Zhao 2019.12.25
 * \param[in]	ip: IPv4 address
 * \param[in]	mask:  Mask num
 * \param[in]	type:  PPEMGR_TYPE_UPSTREAM=0,PPEMGR_TYPE_DWSTREAM=1,PPEMGR_TYPE_BOTH=2
 * \param[in]	sign: PPEMGR_SIGN_TRAFFIC_DETAIL=0,PPEMGR_SIGN_TRAFFIC_MIRROR=1,PPEMGR_SIGN_TRAFFIC_MONITOR=2
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set  the black ip of hwnat.
 * \par Example
 * \snippet blapi_traffic.c set  the black ip of hwnat .
 */

int blapi_traffic_black_ip_for_hwnat_add(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign);

/**
 * \brief This function set the black ipv6 addr of hwnat .
 *
 * This function set  the black ipv6 addr of hwnat .submit by SS.Zhao 2019.12.25
 * \param[in]	u6_addr8: IPv6 address
 * \param[in]	mask: Mask num
 * \param[in]	type: PPEMGR_TYPE_UPSTREAM=0,PPEMGR_TYPE_DWSTREAM=1,PPEMGR_TYPE_BOTH=2
 * \param[in]	sign: PPEMGR_SIGN_TRAFFIC_DETAIL=0,PPEMGR_SIGN_TRAFFIC_MIRROR=1,PPEMGR_SIGN_TRAFFIC_MONITOR=2
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set  the black ipv6 addr of hwnat .
 * \par Example
 * \snippet blapi_traffic.c set  the black ipv6 addr of hwnat .
 */

int blapi_traffic_black_ipv6_for_hwnat_add(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign);

/**
 * \brief This function delete black ip addr for hwnat .
 *
 * This function delete black ip addr for hwnat .submit by SS.Zhao 2019.12.25
 * \param[in]	sign: PPEMGR_SIGN_TRAFFIC_DETAIL=0,PPEMGR_SIGN_TRAFFIC_MIRROR=1,PPEMGR_SIGN_TRAFFIC_MONITOR=2
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  delete black ip addr for hwnat .
 * \par Example
 * \snippet blapi_traffic.c delete black ip addr for hwnat .
 */

int blapi_traffic_black_ip_for_hwnat_delete(unsigned int sign);
/**
 * \brief This function get the rx cnt of port .
 *
 * This function get the rx cnt of port .submit by SS.Zhao 2019.12.25
 * \param[in]	port: lan prot index
 * \param[out]	outptr_Cnt: rx cnt of port
 * \retval	1	Success.
 * \note
 *  get the rx cnt of port .
 * \par Example
 * \snippet blapi_traffic.c get the rx cnt of port .
 */

int blapi_traffic_get_port_rx_cnt(unsigned char port, unsigned long long *outptr_Cnt);

/**
 * \brief This function get the tx cnt of port .
 *
 * This function get the tx cnt of port .submit by SS.Zhao 2019.12.25
 * \param[in]	port: lan prot index
 * \param[out]	outptr_Cnt: tx cnt of port
 * \retval	1	Success.
 * \note
 *  get the tx cnt of port .
 * \par Example
 * \snippet blapi_traffic.c get the tx cnt of port .
 */

int blapi_traffic_get_port_tx_cnt(unsigned char port, unsigned long long *outptr_Cnt);

/**
 * \brief This function get the rx pkts of port .
 *
 * This function get the rx cnt of port .submit by Jiawei.Xu 2022.01.24
 * \param[in]	port: lan prot index
 * \param[out]	outptr_Cnt: rx pkts of port
 * \retval	1	Success.
 * \note
 *  get the rx pkts of port .
 * \par Example
 * \snippet blapi_traffic.c get the rx pkts of port .
 */

int blapi_traffic_get_port_rx_pkts(unsigned char port, unsigned int *outptr_Cnt);

/**
 * \brief This function get the tx pkts of port .
 *
 * This function get the tx cnt of port .submit by Jiawei.Xu 2022.01.24
 * \param[in]	port: lan prot index
 * \param[out]	outptr_Cnt: tx pkts of port
 * \retval	1	Success.
 * \note
 *  get the tx pkts of port .
 * \par Example
 * \snippet blapi_traffic.c get the tx pkts of port .
 */

int blapi_traffic_get_port_tx_pkts(unsigned char port, unsigned int *outptr_Cnt);


/**
 * \brief This function is used to write context to rtsp_alg_port
 *
 * This function is used to write context to rtsp_alg_port submit by JinChao.Yao 2019.12.26
 * \param[in]	RtspPort:rtsp port
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   RtspPort.
 * \par Example
 * \snippet blapi_traffic.c  This function is used to write context to rtsp_alg_port.
 */
int blapi_traffic_set_alg_rtsp_port(const char *RtspPort) ;

/**
 * \brief This function is used to write wanname to proc file.
 *
 * This function is used to write wanname to proc file. submit by JinChao.Yao 2019.12.26
 * \param[in]	wanname: the name of wan
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function is used to write wanname to proc file.
 * \par Example
 * \snippet blapi_traffic.c  This function is used to write wanname to proc file.
 */

int blapi_traffic_set_default_wan_interface(const char* wanname);

/**
 * \brief This function set wan Interface qos type.
 *
 * This function set wan Interface qos type. submit by Lei.Zhang 2020.01.02
 * \param[in]	wanIfValue: the struct include index,wan type name,value and mask.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set wan Interface qos type.
 * \par Example
 * \snippet blapi_traffic.c set wan Interface qos type.
 */
int blapi_traffic_set_wanInterface_qostype(qos_wanIf_value_t* wanIfValue);

/**
 * \brief This function set wan Interface qos template.
 *
 * This function set wan Interface qos template.
 * \param[in]	wanIfIdx: the arry of wan index
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set wan Interface qos template.
 * \par Example
 * \snippet blapi_traffic.c set wan Interface qos template.
 */
int blapi_traffic_set_wanInterface_qos_template(unsigned char* wanIfIdx);


#if defined(TCSUPPORT_CT_DS_LIMIT)
/**
 * \brief This function set up data of data speed limit. 
 *
 * This function set up data of data speed limit. submit by Lei.Zhang 2020.01.02
 * \param[in]	dsMode: LAN interface mode,VLAN id mode or IP range mode.
 * \param[in]	dsValue: data speed value from Dataspeedlimit node in cfg_ng.
 * \param[out] g_queue_speed: the array of speed queue, will be used for QOS car discipline.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set up data of data speed limit
 * \par Example
 * \snippet blapi_traffic.c set up data of data speed limit.
 */
int  blapi_traffic_dslimit_set_up_data(int dsMode, char* dsValue, int g_queue_speed[]);

/**
 * \brief This function set down data of data speed limit. 
 *
 * This function set down data of data speed limit. submit by Lei.Zhang 2020.01.02
 * \param[in]	dsMode: LAN interface mode,VLAN id mode or IP range mode.
 * \param[in]	dsValue: data speed value from Dataspeedlimit node in cfg_ng.
 * \param[in] is_boot:  boot flag of Dataspeedlimit node.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set down data of data speed limit.
 * \par Example
 * \snippet blapi_traffic.c set down data of data speed limit.
 */
int  blapi_traffic_dslimit_set_down_data(int dsMode, char* dsValue, int is_boot);
#endif

/**
 * \brief This function set the value of general_rx_ratelimit .
 *
 * This function  set the value of general_rx_ratelimit .submit by Yifan.Chen 2020.1.2
 * \retval	0	Success.
 * \note
 *  set the value of general_rx_ratelimit .
 * \par Example
 * \snippet blapi_traffic.c set the value of general_rx_ratelimit .
 */
 
int blapi_traffic_set_general_rx_ratelimit(void);

/**
 * \brief This function get the link status of ether port.
 *
 * This function get the link status of ether port.submit by Yifan.Chen 2019.1.9
 * \param[out]	flag:the status of eth link 1:down;2:up.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the link status of ether port.
 * \par Example
 * \snippet blapi_traffic.c get the link status of ether port.
 */

int blapi_traffic_check_eth_link_status(int *flag);

/**
 * \brief This function get the real lan num.
 *
 * This function get the real lan num.submit by Yifan.Chen 2019.1.9
 * \param[out]	lanNum:the sreal lan num.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the real lan num.
 * \par Example
 * \snippet blapi_traffic.c get the real lan num.
 */

int blapi_traffic_check_real_lan_num(int* lanNum);

/**
 * \brief This function get the port status of ether port.
 *
 * This function get the port status of ether port.submit by Yifan.Chen 2019.1.9
 * \param[out]	buf:port status make buffer
 * \param[out]	portStatus:port status of ether port
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the port status of ether port.
 * \par Example
 * \snippet blapi_traffic.c get the port status of ether port.
 */

int blapi_traffic_check_eth_port_status(char *buf, int portStatus[]);

/**
 * \brief This function get portrate information .
 *
 * This function  get portrate information .submit by Hao.Tang 2020.6.19
 * \param[out]	buf:port rate make buffer
 * \param[in]	size:the size of buff.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get portrate information .
 * \par Example
 * \snippet blapi_traffic.c get portrate information .
 */

int blapi_traffic_get_portrate_info(char * buf, int size);

/**
 * \brief This function get lanport information .
 *
 * This function  get lanport information .submit by Yifan.Chen 2019.1.9
 * \param[out]	lanport_info:information of lan port.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get lanport information .
 * \par Example
 * \snippet blapi_traffic.c get lanport information .
 */

int blapi_traffic_get_lanport_info(lanport_info_t *lanport_info);

/**
 * \brief This function set lan information to lan_ip_mask.
 *
 * This function  set lan information to lan_ip_mask.submit by Yifan.Chen 2019.1.9
 * \param[in]	lan_ip:lan ip 
 * \param[in]	lan_mask:lan mask 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set lan information to lan_ip_mask.
 * \par Example
 * \snippet blapi_traffic.c set lan information to lan_ip_mask.
 */

int blapi_traffic_set_lan_info(char lan_ip[], char lan_mask[]);

/**
 * \brief This function set the max bandwidth data .
 *
 * This function  set the max bandwidth data.submit by Lei.Zhang 2020.1.10
 * \param[in]	dslflag: 1=LAN interface,2=VLAN id,3=IP range,0= Disable
 * \param[in]	MaxBandWidthNodeInfo: all value of MaxBandWidth_Entry
 * \param[out] isRuleExist: upRate or downRate is exist or not.	
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set the max bandwidth data .
 * \par Example
 * \snippet blapi_traffic.c set the max bandwidth data .
 */

int blapi_traffic_set_max_bandwidth_data(int dslflag, struct MaxBandWidth_node_info_s MaxBandWidthNodeInfo[], int* isRuleExist);

/**
 * \brief This function get the foe info from sock.
 *
 * This function  get the foe info from sock.submit by Jason.Gong 2020.6.1
 * \param[out] sock_info: foe info in sock.	
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get the foe info from sock.
 * \par Example
 */
int blapi_traffic_get_foe_info_from_sock(int sock, struct SockInfo * sockinfo);


/**
 * \brief This function st the foe into sock.
 *
 * This function  set the foe info from sock.submit by Jason.Gong 2020.6.1
 * \param[out] sock_info: foe info in sock.	
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get the foe info from sock.
 * \par Example
 */

int blapi_traffic_set_foe_info_to_sock(int sock, struct SockInfo * sockinfo);

/**
 * \brief This function gets ether wan state.
 *
 * This function gets ether wan state.
 * \param[in]	sock: socket ID.
 * \param[in]	sockinfo: socket infomation include 
 * \retval	0	down.
 * \retval	1	up.
 * \note
 *	gets ether wan state
 * \par Example
 * \snippet blapi_traffic.c gets ether wan state. 
*/
int blapi_traffic_get_ether_wan_state(int *etherStatus);

/**
 * \brief This function gets the ratelimit mode.
 *
 * This function gets the the ratelimit mode. Such as WAN_MODE
 * \param[in]	upsream(1) or downstream(0)
 * \param[out]  mode: 0:DEV MAC 1: WAN_MODE 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  gets the ratelimit mode.
 * \par Example
 * \snippet blapi_traffic.c gets the ratelimit mode. 
 */
int blapi_traffic_get_ratelimit_mode(Stream_Dir_t dir, ECNT_TRAFFIC_RATELIMIT_MODE *mode);

/**
 * \brief This function set the mtu for HW_NAT_SET_MTU.
 *
 * This function  set the mtu for HW_NAT_SET_MTU.submit by Bin.Wang 2020.6.23
 * \param[in]	mtu: max mtu for interface
 * \retval	1	Success.
 * \retval	-1	Fail.
 * \note
 *  set the mtu for HW_NAT_SET_MTU.
 * \par Example
 * \snippet blapi_traffic.c set the mtu for HW_NAT_SET_MTU.
 */
int blapi_traffic_set_mtu(unsigned int mtu);

/**
 * \brief This function set tls port.
 *
 * This function  set tls port.submit by Feijie.Wang 2020.7.1
 * \param[in] enable 0-Disable 1-Enable
 * \param[in] lan_port lan port num
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set tls port.
 * \par Example
 */
int blapi_traffic_set_tls_port(int enable,unsigned char lan_port);

/**
 * \brief This function set tls port.
 *
 * This function  set tls vlan.submit by Feijie.Wang 2020.7.1
 * \param[in] enable 0-Disable 1-Enable
 * \param[in] vlan vlan id
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set tls vlan.
 * \par Example
 */
int blapi_traffic_set_tls_vlan(int enable,unsigned short vlan);

/**
 * \brief This function drop ipv6 unknown mc by socket info,used in mld proxy.
 *
 * This function  drop ipv6 unknown mc by socket info.submit by Feijie.Wang 2020.7.1
 * \param[in] socket
 * \param[in] ipv6 multicast address
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \drop ipv6 unknown mc by socket info.
 * \par Example
 */
int blapi_traffic_hwdrop_ipv6_unknown_mc_by_sock(int socket,unsigned char* grp_addr);

/**
 * \brief This function delete ipv6 unknown mc hwdrop learned by blapi_traffic_hwdrop_ipv6_unknown_mc_by_sock,used in mld proxy.
 *
 * This function  delete ipv6 unknown mc hwdrop.submit by Feijie.Wang 2020.7.1
 * \param[in] ipv6 multicast address
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \delete ipv6 unknown mc hwdrop.
 * \par Example
 */
int blapi_traffic_del_hwdrop_ipv6_unknown_mc(unsigned char* grp_addr);

/**
 * \brief This function set ipv6 mc wan info to socket,used in mld proxy.
 *
 * This function  drop ipv6 unknown mc by socket info.submit by Feijie.Wang 2020.8.12
 * \param[in] socket
 * \param[in] wan type:1 pppoe 2 ipoe
 * \param[in] wan index
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set wan info to socket.
 * \par Example
 */
int blapi_traffic_ipv6_mc_set_wan_info_to_sock(int  socket,unsigned char wan_type,unsigned char wan_index);

/**
 * \brief This function clear foe info in socket,used in mld proxy.
 *
 * This function  clear foe info in socket.submit by Feijie.Wang 2020.8.12
 * \param[in] socket
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \clear foe info in socket.
 * \par Example
 */
int blapi_traffic_ipv6_mc_clear_sock_info(int  socket);

/**
*\brief ppe add nptv6 prefix
*
* This function add nptv6 prefix.submit by Feijie.Wang 2020.7.30
*\param[in] ifname, vr wan interface name;
*\param[in] snpt_str, format is xxxx:xxxx::/xx;
*\param[in] dnpt_str, format is xxxx:xxxx::/xx;
* \retval  0   Success.
* \retval  -1  Fail.
* \note
* \add nptv6 prefix.
* \par Example
*/
int blapi_traffic_add_nptv6_prefix(unsigned char* ifname,unsigned char* snpt_str,unsigned char* dnpt_str);

/**
*\brief ppe del nptv6 prefix
*
* This function del nptv6 prefix.submit by Feijie.Wang 2020.7.30
*\param[in] ifname, vr wan interface name;
* \retval  0   Success.
* \retval  -1  Fail.
* \note
* \del nptv6 prefix.
* \par Example
*/
int blapi_traffic_del_nptv6_prefix(unsigned char* ifname);

/**
*\brief ppe del nptv6 prefix all
*
* This function del nptv6 prefix all.submit by Feijie.Wang 2020.7.30
* \retval  0   Success.
* \retval  -1  Fail.
* \note
* \del nptv6 prefix all.
* \par Example
*/
int blapi_traffic_del_nptv6_prefix_all(void);

/**
 * \brief set wan interface mtu
 *
 * This function  set wan mtu.submit by kevin.shi 2020.08.04
 * \param[in] wan_idx  wan interface index ranged 0 - 7
 * \param[in] wan_mtu the mtu size example for 1518, and 0x3fff means MTU disable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set wan interface mtu
 * \par Example
 */
int blapi_traffic_set_wan_mtu(int wan_idx, int wan_mtu);

/**
 * \brief add vip by sip and sport
 *
 * This function  add vip by sip and sport.submit by feijie.wang 2020.09.16
 * \param[in] sip  ipv4 source ip,0 means not care
 * \param[in] sport udp/tcp source port ,0 means not care
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \add vip by sip and sport
 * \par Example
 */
int blapi_traffic_set_vip_by_sip_and_sport(unsigned int dip, unsigned short dport);

/**
 * \brief del vip by sip and sport
 *
 * This function  del vip by sip and sport.submit by feijie.wang 2020.09.16
 * \param[in] sip  ipv4 source ip,0 means not care
 * \param[in] sport udp/tcp source port ,0 means not care
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \del vip by sip and sport
 * \par Example
 */
int blapi_traffic_del_vip_by_sip_and_sport(unsigned int sip, unsigned short sport);

/**
 * \brief add vip by dip and dport
 *
 * This function  add vip by dip and dport.submit by feijie.wang 2020.09.16
 * \param[in] dip  ipv4 dest ip,0 means not care
 * \param[in] dport udp/tcp dest port ,0 means not care
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \add vip by dip and dport
 * \par Example
 */
int blapi_traffic_set_vip_by_dip_and_dport(unsigned int dip, unsigned short dport);

/**
 * \brief del vip by dip and dport
 *
 * This function  add vip by dip and dport.submit by feijie.wang 2020.09.16
 * \param[in] dip  ipv4 dest ip,0 means not care
 * \param[in] dport udp/tcp dest port ,0 means not care
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \add vip by dip and dport
 * \par Example
 */
int blapi_traffic_del_vip_by_dip_and_dport(unsigned int dip, unsigned short dport);

/**
 * \brief This function set vip protocol.
 *
 * This function set vip protocol. 
 * \param[in]	ip_protocol
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  
 */
int blapi_traffic_set_vip_by_ip_protocol(unsigned short ip_protocol);

/**
 * \brief This function del vip protocol.
 *
 * This function del vip protocol. 
 * \param[in]	ip_protocol
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  
 */
int blapi_traffic_del_vip_by_ip_protocol(unsigned short ip_protocol);

/**
 * \brief This function update ifc vip.
 *
 * This function update ifc vip.
 * \param[in]	opt(add:1 or del:2)
 * \param[in]	ifc_data
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  
 */
int blapi_traffic_update_ifc_vip_params(int opt, ifc_param_t *ifc_data);

/**
 * \brief This function set qdma wan rxratelimit.
 *
 * This function set qdma wan rxratelimit.
 * \param[in]	ringidx
 * \param[in]	rate
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  
 */
int blapi_traffic_set_qdma_wan_rxratelimit(int ringidx, unsigned int rate);


/**
 * \brief blapi_traffic_set_sla_cfg
 *
 * This function  set sla cfg
 * \param[in] direction  0: down stream,set qdma_lan 1:upstream set qdma_wan
 * \param[in] sla_index  range 0-3, hw support max 4 sla. 
 * \param[in] enable   range 0-1
 * \param[in] chnl_index range 0-31 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set blapi_traffic_set_sla_cfg
 * \par Example
 */

int blapi_traffic_set_sla_cfg(unsigned int direction, unsigned int sla_index, unsigned int enable ,unsigned int chnl_index);

/**
 * \brief blapi_traffic_set_qos_config_NULLQOS
 *
 * This function  set qos NULL
 * \param[in] queue_mask  
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set blapi_traffic_set_qos_config_NULLQOS
 * \par Example
 */

int blapi_traffic_set_qos_config_NULLQOS(char queue_mask);

/**
 * \brief blapi_traffic_set_assure_max_bandwidth
 *
 * This function  set sla cfg
 * \param[in] direction  0: down stream,set qdma_lan 1:upstream set qdma_wan
 * \param[in] sla_index  range 0-3, hw support max 4 sla. 
 * \param[in] queue_index   range 0-1
 * \param[in] cbs_value  unkt  kbps
 * \param[in] pbs_value  unkt  kbps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set blapi_traffic_set_assure_max_bandwidth
 * \par Example
 */
int blapi_traffic_set_assure_max_bandwidth(unsigned int direction,unsigned int sla_index,unsigned int queue_index, unsigned int cbs_value, unsigned int pbs_value);

/**
 * get etherwan port.  
 */
 /**
 * \brief This function gets ether wan port.
 *
 * This function gets ether wan state.
 * \param[out]	ether wan port index
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *	gets ether wan port
 * \par Example
 * \snippet blapi_traffic.c gets ether wan port. 
*/
int blapi_traffic_get_etherwan_port(int *port);

/**
 * \brief blapi_traffic_ob_info_restart 
 *
 * This function clear other bridge wan data, recorded in driver.
 * \param[in]	data  point to operate other bridge wan data.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 */
int blapi_traffic_ob_info_restart(blapi_info_ioctl_OB_map_data* data);

/**
 * \brief blapi_traffic_ob_info_get_cnt 
 *
 * This function get record cnt of src ip via other bridge wan in driver.
 * \param[in]	data  point to operate other bridge wan data.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 */
int blapi_traffic_ob_info_get_cnt(blapi_info_ioctl_OB_map_data* data);

/**
 * \brief blapi_traffic_ob_info_get_data 
 *
 * This function get data info of src ip via other bridge wan in driver.
 * \param[in]	data  point to operate other bridge wan data.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 */
int blapi_traffic_ob_info_get_data(blapi_info_ioctl_OB_map_data* data);

/**
 * \brief This function judge if need get TR69/VOICE wan ant
 *
 * This function judge if need get TR69/VOICE wan ant.
 * \param[in]	flag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note	flag 1: need get TR69/VOICE wan ant
			flag 0: no need get TR69/VOICE wan ant
 *  Write the things that need to pay attention to when using this function.
 */
int blapi_traffic_get_wancnt_flag(int *flag);


/**
 * \brief This function get hsgmii lan  rx statistics.
 *
 * This function  get hsgmii lan rx statistics.submit by qi.shi 2021.1.7
  * \param[in] hsgmii_index  0: pcie0 1:pcie1 2:usb as hsgmii lan
  * \param[out] hsgmii lan rx statistics
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get hsgmii lan rx statistics.
 * \par Example
 */
int blapi_traffic_get_hsgmii_lan_rx_statistics(ECNT_TRAFFIC_WAN_RX_STATISTICS* p_stat,int hsgmii_index);

/**
 * \brief This function get hsgmii lan  tx statistics.
 *
 * This function  get hsgmii lan rx statistics.submit by qi.shi 2021.1.7
  * \param[in] hsgmii_index  0: pcie0 1:pcie1 2:usb as hsgmii lan
  * \param[out] hsgmii lan tx statistics
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get hsgmii lan tx statistics.
 * \par Example
 */
int blapi_traffic_get_hsgmii_lan_tx_statistics(ECNT_TRAFFIC_WAN_TX_STATISTICS* p_stat,int hsgmii_index);
/**
 * \brief This function get multi hsgmii lan link stats.
 *
 * This function  get hsgmii lan rx statistics.submit by qi.shi 2022.4.6
  * \param[in] hsgmii_index  0: pcie0 1:pcie1 2:usb 3:eth as hsgmii lan
  * \param[out] hsgmii lan tx statistics
 * \retval	0	down.
 * \retval	1	up.
 * \note
 * \get hsgmii lan link state.
 * \par Example
 */
int blapi_traffic_get_multi_hsgmii_lan_state(int *etherStatus, int hsgmii_index);

/**
 * \brief This function get multi hsgmii lan link rate.
 *
 * This function  get hsgmii lan link rate.
  * \param[in] hsgmii_index  0: pcie0 1:pcie1 2:usb 3:eth as hsgmii lan
  * \param[out] hsgmii lan link rate
 * \retval	0:10g, 1:5g, 2:2.5g, 3: 1g, 4:100M, 5:10M
 * \note
 * \get hsgmii lan link rate.
 * \par Example
 */
int blapi_traffic_get_multi_hsgmii_lan_linkrate(int *etherStatus, int hsgmii_index);

/**
 * \brief MDIO read
 *
 * This function  MIDO read.submit by youchao.wu 2021.02.24
 * \param[in] phyaddr  ephy phy address 
 * \param[in] devaddr  ephy dev address
 * \param[in] reg  ephy reg
 * \param[out] p_value MDIO read value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \MDIO read
 * \par Example
 */
int blapi_traffic_mdio_read(unsigned int phyaddr, unsigned int devaddr, unsigned int reg, unsigned int *p_value);

/**
 * \brief MDIO write
 *
 * This function  MIDO write.submit by youchao.wu 2021.02.24
 * \param[in] phyaddr  ephy phy address 
 * \param[in] devaddr  ephy dev address
 * \param[in] reg  ephy reg
 * \param[in] value MDIO write value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \MDIO write
 * \par Example
 */
int blapi_traffic_mdio_write(unsigned int phyaddr, unsigned int devaddr, unsigned int reg, unsigned int value);

/**
* \brief This function will add vlan filter vlan.
*
* This function add vlan filter vlan.
* \param[in]   vlan: vlan to add
* \retval  0   OP_SUCCESS.
* \retval  1   OP_FAIL.
* \retval  2   ADD_ALREADY_EXIST
* \note
*  add vlan filter vlan.
* \par Example
* \snippet blapi_traffic.c add vlan filter vlan.
*/
int blapi_traffic_add_vlan_filter_vlan(unsigned int vlan, unsigned int port);

/**
* \brief This function will del vlan filter vlan.
*
* This function del vlan filter vlan.
* \param[in]   wanid: vlan to del.
* \retval  0   OP_SUCCESS.
* \retval  1   OP_FAIL.
* \retval  3   DELETE_NOT_FOUND.
* \note
*  del vlan filter vlan.
* \par Example
* \snippet blapi_traffic.c del vlan filter vlan.
*/
int blapi_traffic_del_vlan_filter_vlan(unsigned int vlan, unsigned int port);

/**
* \brief This function is for app speedtest, use kernel speedtest module
*
* \param[in]   lport: local tcp port.
* \param[in]   rport: remote tco port.
* \param[in]   ipl: local IP address.
* \param[in]   ipr: remote IP address.

* \retval  0   OP_SUCCESS.
* \retval  1   OP_FAIL.
* \note
*/
int blapi_traffic_speedtest_setting(int lport, int rport, unsigned int ipl, unsigned int ipr);

#endif /*_BLAPI_TRAFFIC_H_*/
