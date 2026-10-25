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
#ifndef _BLAPI_TRAFFIC_BSP_H_
#define _BLAPI_TRAFFIC_BSP_H_

/**
* \file  blapi_traffic_bsp.h  
* \brief Definiations and BLAPIs related to data link/traffic. These APIs are designed for both BSP and SDK users.
* \author zhang.lei
* \date     2020-01-03
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define ECNT_SWITCHMGR_TRTCM_MATCH_MAC_RULE_NUM 10

typedef struct
{
    unsigned long long oversize;           /**< over sized cnt */
    unsigned long long  undersize;          /**< under sized cnt */
    unsigned long long  frame_cnt;          /**< packet cnt */
    unsigned long long  frame_len;          /**< byte cnt */
    unsigned long long  drop_cnt;           /**< drop cnt */
    unsigned long long  broadcast;          /**< broadcast cnt */
    unsigned long long  multicast;          /**< multicast cnt */
    unsigned long long  crc;                /**< crc cnt */
    unsigned long long  fragment;           /**< fragment cnt */
    unsigned long long  jabber;             /**< jabber cnt */
    unsigned long long  less_64;            /**< packet length < 64*/
    unsigned long long  more_1518;          /**< packet length > 1518 */
    unsigned long long  eq_64;              /**< packet length = 64*/
    unsigned long long  from_65_to_127;     /**< packet length between 65-127 */
    unsigned long long  from_128_to_255;    /**< packet length between 128-255 */
    unsigned long long  from_256_to_511;    /**< packet length between 256-511 */
    unsigned long long  from_512_to_1023;   /**< packet length between 512-1023 */
    unsigned long long  from_1024_to_1518;  /**< packet length between 1024-1518*/
}ECNT_TRAFFIC_WAN_RX_STATISTICS;

typedef struct
{
    unsigned long long inBytes;
    unsigned long long inPkts;
    unsigned long long inUnicast;
    unsigned long long inMulticast;
    unsigned long long inBroadcast;
    unsigned long long inError;
    unsigned long long inDiscard;
    unsigned long long outBytes;
    unsigned long long outPkts;
    unsigned long long outUnicast;
    unsigned long long outMulticast;
    unsigned long long outBroadcast;
    unsigned long long outError;
    unsigned long long outDiscard;
}ECNT_TRAFFIC_GMAC_ETH_STA;

typedef struct
{
    unsigned char link_status;//0:down 1:up
    unsigned char auto_ng_status; //0:auto 1:force
    unsigned char duplex_status; //0:half-duplex 1:full-duplex
    unsigned char speed_status; //0:10Mbps 1:100Mbps 2:1000Mbps 3:2.5G bps
}ECNT_TRAFFIC_GMAC_ETH_LINKST;

typedef enum
{
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_AUTONEG    = 1, /**< auto negotiation */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_10M_HALF   = 2, /**< 10M Half Duplex  */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_10M_FULL   = 3, /**< 10M Full Duplex  */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_100M_HALF  = 4, /**< 100M Half Duplex */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_100M_FULL  = 5, /**< 100M Full Duplex */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_1000M_FULL = 6, /**< 1000M Full Duplex>*/
}ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE;

typedef enum 
{
	EPON_SLA = 0,
	GEMPORT_UP,
	GEMPORT_DOWN,
}ECNT_TRAFFICE_PON_RATELIMIT_MODE;

typedef struct dev_bandwidth_tmp_entry_s
{
	int dev_mac[6]; 				/**< match mac address */
	int ds_queue_speed; 			/**< down stream speed */
	int us_queue_speed; 			/**< up stream speed*/
	int conn_type;					/**< type of connection: lan/wifi*/
}dev_bandwidth_tmp_entry_t, *dev_bandwidth_tmp_entry_p;

typedef struct dev_bandwidth_tmp_s
{
	dev_bandwidth_tmp_entry_t trtcm_acl_mac_table[ECNT_SWITCHMGR_TRTCM_MATCH_MAC_RULE_NUM];    /**< mac acl entries */
}dev_bandwidth_tmp_t, *dev_bandwidth_tmp_p;

typedef struct wan_acnt {
	unsigned long long uniTxBytes;
    unsigned long long uniRxBytes;
	unsigned long long mulTxBytes;
    unsigned long long mulRxBytes;
	unsigned long long uniTxCnt;	
	unsigned long long uniRxCnt;
	unsigned long long mulTxCnt;	
	unsigned long long mulRxCnt;
	unsigned long long txRate;
    unsigned long long rxRate;
    unsigned long jiffies;
}wan_acnt_t, *wan_acnt_p;

typedef enum
{
	VIP_P_TCP = 0,
	VIP_P_UDP,
	VIP_P_ALL,
}ECNT_TRAFFIC_VIP_PROTOCOL;

typedef struct
{
    unsigned long long frame_cnt;          /**< packet cnt */
    unsigned long long frame_len;          /**< byte cnt */
    unsigned long long drop_cnt;           /**< drop cnt */
    unsigned long long broadcast;          /**< broadcast cnt */
    unsigned long long multicast;          /**< multicast cnt */
    unsigned long long less_64;            /**< packet length < 64*/
    unsigned long long more_1518;          /**< packet length > 1518 */
    unsigned long long eq_64;              /**< packet length = 64*/
    unsigned long long from_65_to_127;     /**< packet length between 65-127 */
    unsigned long long from_128_to_255;    /**< packet length between 128-255 */
    unsigned long long from_256_to_511;    /**< packet length between 256-511 */
    unsigned long long from_512_to_1023;   /**< packet length between 512-1023 */
    unsigned long long from_1024_to_1518;  /**< packet length between 1024-1518*/
}ECNT_TRAFFIC_WAN_TX_STATISTICS;

typedef enum
{
	RATELIMIT_MODE_DEV_MAC = 0,
	RATELIMIT_MODE_WAN,
	RATELIMIT_MODE_TRAFFIC_QOS,
	RATELIMIT_MODE_OLT_CTL,
}ECNT_TRAFFIC_RATELIMIT_MODE;

typedef enum{
	DOWN_DIR = 0,
	UP_DIR,
}Stream_Dir_t;


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

/**
 * \brief This function get lan tx real time rate.
 *
 * This function  get lan tx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] rate unit is kbps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan tx real time rate.
 * \par Example
 */
int blapi_traffic_get_lan_tx_traffic(int lan_port,unsigned int* rate);

/**
 * \brief This function get lan rx real time rate.
 *
 * This function  get lan rx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] rate unit is kbps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan rx real time rate.
 * \par Example
 */
int blapi_traffic_get_lan_rx_traffic(int lan_port,unsigned int* rate);

/**
 * \brief This function get lan tx real time rate.
 *
 * This function  get lan tx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] rate unit is pps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan tx real time rate.
 * \par Example
 */
int blapi_traffic_get_lan_tx_rate(int lan_port,unsigned int* rate);

/**
 * \brief This function get lan rx real time rate.
 *
 * This function  get lan rx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] rate unit is pps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan rx real time rate.
 * \par Example
 */
int blapi_traffic_get_lan_rx_rate(int lan_port,unsigned int* rate);

/**
 * \brief This function get lan tx frame byte cnt in 15minutes.
 *
 * This function  get lan tx frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan tx frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_lan_tx_octets(int lan_port, unsigned int *frameByteCnt);

/**
 * \brief This function get lan rx frame byte cnt in 15minutes.
 *
 * This function  get lan rx frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan rx frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_lan_rx_octets(int lan_port, unsigned int *frameByteCnt);

/**
 * \brief This function get lan tx discard frame byte cnt in 15minutes.
 *
 * This function  get lan tx discard frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] discard frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan tx discard frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_lan_tx_discard_counter(int lan_port, unsigned int *discardCnt);

/**
 * \brief This function get lan rx discard frame byte cnt in 15minutes.
 *
 * This function  get lan rx discard frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan rx discard frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_lan_rx_discard_counter(int lan_port, unsigned int *discardCnt);

/**
 * \brief This function get lan tx error frame byte cnt in 15minutes.
 *
 * This function  get lan tx error frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] discard frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan tx error frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_lan_tx_error_counter(int lan_port, unsigned int *errorCnt);

/**
 * \brief This function get lan rx error frame byte cnt in 15minutes.
 *
 * This function  get lan rx error frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[in] lan port  lan port num
 * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get lan rx error frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_lan_rx_error_counter(int lan_port, unsigned int *errorCnt);

/**
 * \brief This function get wan tx real time rate.
 *
 * This function  get wan tx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[out] rate unit is kbps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan tx real time rate.
 * \par Example
 */
int blapi_traffic_get_wan_tx_traffic(unsigned int* rate);

/**
 * \brief This function get wan rx real time rate.
 *
 * This function  get wan rx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[out] rate unit is kbps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan rx real time rate.
 * \par Example
 */
int blapi_traffic_get_wan_rx_traffic(unsigned int* rate);

/**
 * \brief This function get wan tx real time rate.
 *
 * This function  get wan tx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[out] rate unit is pps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan tx real time rate.
 * \par Example
 */
int blapi_traffic_get_wan_tx_rate(unsigned int* rate);

/**
 * \brief This function get wan rx real time rate.
 *
 * This function  get wan rx real time rate.submit by Feijie.Wang 2020.7.1
 * \param[out] rate unit is pps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan rx real time rate.
 * \par Example
 */
int blapi_traffic_get_wan_rx_rate(unsigned int* rate);

/**
 * \brief This function get wan tx frame byte cnt in 15minutes.
 *
 * This function  get wan tx frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan tx frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_wan_tx_octets(unsigned int *frameByteCnt);

/**
 * \brief This function get wan rx frame byte cnt in 15minutes.
 *
 * This function  get wan rx frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan rx frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_wan_rx_octets(unsigned int *frameByteCnt);

/**
 * \brief This function get wan tx discard frame byte cnt in 15minutes.
 *
 * This function  get wan tx discard frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[out] discard frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan tx discard frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_wan_tx_discard_counter(unsigned int *discardCnt);

/**
 * \brief This function get wan rx discard frame byte cnt in 15minutes.
 *
 * This function  get wan rx discard frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
 * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan rx discard frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_wan_rx_discard_counter(unsigned int *discardCnt);

/**
 * \brief This function get wan tx error frame byte cnt in 15minutes.
 *
 * This function  get wn tx error frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
  * \param[out] discard frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan tx error frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_wan_tx_error_counter(unsigned int *errorCnt);

/**
 * \brief This function get wan rx error frame byte cnt in 15minutes.
 *
 * This function  get wan rx error frame byte cnt in 15minutes.submit by Feijie.Wang 2020.7.1
  * \param[out] frame byte cnt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan rx error frame byte cnt in 15minutes.
 * \par Example
 */
int blapi_traffic_get_wan_rx_error_counter(unsigned int *errorCnt);

/**
 * \brief This function get wan rx statistics.
 *
 * This function  get wan rx statistics.submit by Feijie.Wang 2020.7.1
  * \param[out] wan rx statistics
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan rx statistics.
 * \par Example
 */
int blapi_traffic_get_wan_rx_statistics(ECNT_TRAFFIC_WAN_RX_STATISTICS* p_stat);

/**
 * \brief This function get wan tx statistics.
 *
 * This function  get wan tx statistics.submit by Feijie.Wang 2020.7.1
  * \param[out] wan tx statistics
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get wan tx statistics.
 * \par Example
 */
int blapi_traffic_get_wan_tx_statistics(ECNT_TRAFFIC_WAN_TX_STATISTICS* p_stat);

/**
 * \brief This function set ratelimit for pon.
 *
 * This function set ratelimit for pon.submit by Feijie.Wang 2020.7.1
 * \param[in] mode,reference to  ECNT_TRAFFICE_PON_RATELIMIT_MODE
 * \param[in] index 
 * \param[in] unit is kbps
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \set ratelimit for pon.
 * \par Example
 */
int blapi_traffic_set_pon_ratelimit(ECNT_TRAFFICE_PON_RATELIMIT_MODE mode,unsigned int index,unsigned int rate);

/**
 * \brief This function will enable the rate limit of wan/lan based on meter_mode
 *
 * This function set mode of the rate limit of wan/lan. submit by ShengQian.Wang  2019.03.16
 * \param[in]	meter_mode: 1: dev mac mode, 2: wan mode.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set mode of the rate limit of wan/lan. 
 * \par Example
 * \snippet blapi_traffic.c set mode of the rate limit of wan/lan. 
 */

int blapi_traffic_set_ratelimit_mode(ECNT_TRAFFIC_RATELIMIT_MODE mode);

/**
 * \brief This function will enable the flow rate statistics 
 *
 * This function will enable the flow rate statistics . 
 * \param[in]	void: 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  enable the flow rate statistics 
 * \par Example
 * \snippet blapi_traffic.c enable the flow rate statistics.
 */
int blapi_traffic_set_flowid_acnt_enable(int enable);

/**
 * \brief This function will enable the llid statistics 
 *
 * This function will enable the llid statistics . 
 * \param[in]	void: 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  enable the llid rate statistics 
 * \par Example
 * \snippet blapi_traffic.c enable the llid statistics.
 */

int blapi_traffic_set_llid_acnt_enable(int enable);

/**
 * \brief This function set the ratelimit of qos flow .
 *
 * This function set the ratelimit of qos flow .submit by SS.Zhao 2019.12.25
 * \param[in]	direction : METER_DIR_UP=0, METER_DIR_DOWN=1
 * \param[in]	flowid :  flow ID
 * \param[in]	ratelimit : limit rate
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set the ratelimit of qos flow .
 * \par Example
 * \snippet blapi_traffic.c set the ratelimit of qos flow .
 */
 
int blapi_traffic_set_flowid_ratelimit(unsigned char direction, unsigned char flowid, unsigned int ratelimit);

/**
 * \brief This function set the ratelimit rule of device.
 *
 * This function set the ratelimit rule of device. submit by Zhen.Xu 2019.12.05
 * \param[in]	p_dev_mac_acl: The info of device.
 * \param[in]	bandwidth_cnt: The count of bandwidth.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ratelimit rule of device.
 * \par Example
 * \snippet blapi_traffic.c Set the ratelimit rule of device
 */

int blapi_traffic_set_dev_ratelimit_by_mac(dev_bandwidth_tmp_t *p_dev_mac_acl, int bandwidth_cnt);

/**
 * \brief This function will enable the wanif rate statistics 
 *
 * This function will enable the wanif rate statistics . submit by Lei.Zhang 2019.12.16
 * \param[in]	void: 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  enable the wanif rate statistics 
 * \par Example
 * \snippet blapi_traffic.c enable the wanif rate statistics.
 */

int blapi_traffic_set_wan_acnt_enable(int enable);

/**
* \brief This function will set the wan counter clear.
*
* This function set the wan counter clear.
* \param[in]   wanid: wan port index
* \retval  0   Success.
* \retval  -1  Fail.
* \note
*  Set clear of wan counter.
* \par Example
* \snippet blapi_traffic.c Set the wan counter clear.
*/
int blapi_traffic_set_wan_acnt_clear(unsigned int wanid);

/**
 * \brief This function get wan acct  according to wan port index.
 *
 * This function get wan acct  according to wan port index. submit by Lei.Zhang 2019.12.16
 * \param[in]	i: wan port index.
 * \param[out] cnt: The data you get.
 * \param[out] flag: 1:only get multi packet info, 0:get all info
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get wan acct according to wan port index.
 * \par Example
 * \snippet blapi_traffic.c Get wan acct according to wan port index.
 */

int blapi_traffic_get_wan_acnt(int i, wan_acnt_t *cnt);

/**
 * \brief set vip by port
 *
 * This function  set vip by port.submit by feijie.wang 2020.09.16
 * \param[in] sport  udp/tcp source port,0 means not care
 * \param[in] dport udp/tcp dest port,0 means not care
 * \param[in] vip_p tcp/udp/all
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note set vip by port
 * \par Example
 */
int blapi_traffic_set_vip_by_port(unsigned short sport, unsigned short dport,ECNT_TRAFFIC_VIP_PROTOCOL vip_p);

/**
 * \brief del vip by port
 *
 * This function  del vip by port.submit by feijie.wang 2020.09.16
 * \param[in] sport  udp/tcp source port,0 means not care
 * \param[in] dport udp/tcp dest port ,0 means not care
 * \param[in] vip_p tcp/udp/all
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note del vip by port
 * \par Example
 */
int blapi_traffic_del_vip_by_port(unsigned short sport, unsigned short dport,ECNT_TRAFFIC_VIP_PROTOCOL vip_p);

/**
 * \brief This function get peer wan port (blapi client's) wan/lan statistics.
 *
 * This function  get peer wan port statistics
  * \param[in] static_buf:to store data from peer, port_id  1:peer wan port 2:peer lan port
  * \param[out] peer wan port statistics
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get peer wan port statistics.
 * \par Example
 */
int blapi_traffic_get_wan_port_statistics(ECNT_TRAFFIC_GMAC_ETH_STA * static_buf, int port_id);
/**
 * \brief This function get peer wan port (blapi client's) wan link state.
 *
 * This function  get peer wan port statistics
  * \param[in] state_buf:to store data from peer, port_id  1:peer wan port
  * \param[out] peer wan port link state
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get peer wan port link state.
 * \par Example
 */
int blapi_traffic_get_wan_port_link_state(ECNT_TRAFFIC_GMAC_ETH_LINKST * state_buf, int port_id);
/**
 * \brief This function set peer wan port (blapi client's) wan port link mode
 *
 * This function  set peer wan port link mode
  * \param[in] mode:ECNT_GMAC_ETH_SPEED_MODE:1(auto)/2(10M Half)/3(10M Full)/4(100M Half)/5(100M Full)/6(1000M Full), port_id  1:peer wan port 
  * \param[out] peer wan port statistics
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * \get peer wan port link mode
 * \par Example
 */
int blapi_traffic_set_wan_port_link_mode(ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE mode, int port_id);
/**
 * \brief This function will check if dsl is enabled
 *
 * This function check if dsl is enabled .submit by Fenghao.Zhang 2024.06.14
 * \param[out] dsl enable
 * \retval	0	disabled.
 * \retval	1	enabled.
 * \note
 *  check if dsl is enabled.
 * \par Example
 * \snippet blapi_traffic.c check if dsl is enabled.
 */
int blapi_traffic_check_dsl_enable();

#endif /*_BLAPI_TRAFFIC_BSP_H_*/
