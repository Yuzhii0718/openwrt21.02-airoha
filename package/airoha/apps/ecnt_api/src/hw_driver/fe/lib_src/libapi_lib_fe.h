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
///@file libapi_lib_fe.h
///@brief FE Module API
#ifndef _API_LIB_FE_H
#define _API_LIB_FE_H

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/
#define UP_QUEUE_CNT_START		16
#define DOWN_QUEUE_CNT_START	24

#define NS_INT16SZ	 2
#define NS_INADDRSZ	 4
#define NS_IN6ADDRSZ	16

///@brief defines of FE TRFC
#define FE_TRFC_SVID	(1 << 0)
#define FE_TRFC_SPBIT	(1 << 1)
#define FE_TRFC_CVID	(1 << 2)
#define FE_TRFC_CPBIT	(1 << 3)
#define FE_TRFC_TOS		(1 << 4)
#define FE_TRFC_TC		(1 << 5)
#define FE_TRFC_ETYPE	(1 << 6)
#define FE_TRFC_DPORT	(1 << 7)
#define FE_TRFC_SPORT	(1 << 8)
#define FE_TRFC_DIPV6	(1 << 9)
#define FE_TRFC_SIPV6	(1 << 10)
#define FE_TRFC_DIPV4	(1 << 11)
#define FE_TRFC_SIPV4	(1 << 12)
#define FE_TRFC_DMAC	(1 << 13)
#define FE_TRFC_SMAC	(1 << 14)

#define FE_TRFC_CVLAN	(FE_TRFC_CPBIT|FE_TRFC_CVID)
#define FE_TRFC_SVLAN	(FE_TRFC_SPBIT|FE_TRFC_SVID)
#define FE_TRFC_VLAN	(FE_TRFC_CVLAN|FE_TRFC_SVLAN)

#define ECNT_FEMGR_MC_VLAN_TABLE_MAX     (64)
#define ECNT_FEMGR_MC_VLAN_PORT_MAX    	 (16)

///@brief Return value of FE API
typedef enum
{
    ECNT_FEMGR_SUCCESS = 0,     /**< api execution succeed */
    ECNT_FEMGR_INVALID_PARAM,   /**< parameters not valid */
    ECNT_FEMGR_NOT_SUPPORT,     /**< api not supported */
    ECNT_FEMGR_FAIL,            /**< api execution failed */
}ECNT_FEMGR_RET;

///@brief FE GDMA1/GDMA2 selection
typedef enum
{
    ECNT_FEMGR_GDM_SEL_GDMA1 = 0,   /**< GDMA1 */
    ECNT_FEMGR_GDM_SEL_GDMA2,       /**< GDMA2 */
    ECNT_FEMGR_GDM_SEL_GDMA3,       /**< GDMA3 */
    ECNT_FEMGR_GDM_SEL_GDMA4,       /**< GDMA4 */
    ECNT_FEMGR_GDM_SEL_MAX,
}ECNT_FEMGR_GDM_SEL;

///@brief FE CDMA1/CDMA2 selection
typedef enum
{
    ECNT_FEMGR_CDM_SEL_CDMA1 = 0,   /**< CDMA1 */
    ECNT_FEMGR_CDM_SEL_CDMA2,       /**< CDMA2 */
    ECNT_FEMGR_CDM_SEL_MAX,
}ECNT_FEMGR_CDM_SEL;

///@brief FE traffic direction
typedef enum
{
    ECNT_FEMGR_DIRECTON_TX = 0, /**< tx */
    ECNT_FEMGR_DIRECTON_RX,     /**< rx */
    ECNT_FEMGR_DIRECTON_MAX,
}ECNT_FEMGR_DIRECTION;

///@brief FE channel retire mode
typedef enum
{
    ECNT_FEMGR_CHANNEL_RETIRE_MODE_LINK_UP = 0, /**< link down */
    ECNT_FEMGR_CHANNEL_RETIRE_MODE_LINK_DOWN,   /**< link up */
    ECNT_FEMGR_CHANNEL_RETIRE_MODE_MAX,
}ECNT_FEMGR_CHANNEL_RETIRE_MODE;

///@brief FE packet type
typedef enum
{
    ECNT_FEMGR_PKT_TYPE_OTHERCAST = 0,  /**< unicast pkt */
    ECNT_FEMGR_PKT_TYPE_MULTICAST,      /**< multicast pkt */
    ECNT_FEMGR_PKT_TYPE_BROADCAST,      /**< broadcast pkt */
    ECNT_FEMGR_PKT_TYPE_MYMAC,          /**< dst mac is mymac */
    ECNT_FEMGR_PKT_TYPE_MAC_FILTER,
}ECNT_FEMGR_PKT_TYPE;

///@brief FE ports definition
typedef enum
{
    ECNT_FEMGR_PORT_QDMA1_CPU = 0,  /**< QDMA1 CPU port */
    ECNT_FEMGR_PORT_GDMA1,          /**< GDMA1 port */
    ECNT_FEMGR_PORT_GDMA2,          /**< GDMA2 port */
    ECNT_FEMGR_PORT_QDMA1_HWF,      /**< QDMA1 HWF port */
    ECNT_FEMGR_PORT_PPE,            /**< PPE port */
    ECNT_FEMGR_PORT_QDMA2_CPU,      /**< QDMA2 CPU port */
    ECNT_FEMGR_PORT_QDMA2_HWF,      /**< QDMA2 HWF port */
    ECNT_FEMGR_PORT_DISCARD,        /**< Drop port  */
    ECNT_FEMGR_PORT_MAX,
}ECNT_FEMGR_PORT;

///@brief FE drop frame type
typedef enum
{
    ECNT_FEMGR_DROP_FRAME_TYPE_UDP_CHECKSUM_ERR = 0,    /**< udp checksum error */
    ECNT_FEMGR_DROP_FRAME_TYPE_TCP_CHECKSUM_ERR,        /**< tcp checksum error */
    ECNT_FEMGR_DROP_FRAME_TYPE_IP_CHECKSUM_ERR,         /**< ip checksum error */
    ECNT_FEMGR_DROP_FRAME_TYPE_CRC_ERR,                 /**< crc error */
    ECNT_FEMGR_DROP_FRAME_TYPE_RUNT,                    /**< packet shorter than minimum length configured */
    ECNT_FEMGR_DROP_FRAME_TYPE_LONG,                    /**< packet longer than maximum length configured */
}ECNT_FEMGR_DROP_FRAME_TYPE;

///@brief FE GDMA statistics type
typedef enum
{
    ECNT_FEMGR_STATISTICS_TYPE_TX_OK = 0,       /**< tx ok cnt */
    ECNT_FEMGR_STATISTICS_TYPE_RX_OK,           /**< rx ok cnt */
    ECNT_FEMGR_STATISTICS_TYPE_TX_BYTE,         /**< tx byte cnt */
    ECNT_FEMGR_STATISTICS_TYPE_RX_BYTE,         /**< rx byte cnt */
    ECNT_FEMGR_STATISTICS_TYPE_RX_DROP_FIFO,    /**< rx drop fifo cnt */
    ECNT_FEMGR_STATISTICS_TYPE_RX_DROP_ERR,     /**< rx drop err cnt */
    ECNT_FEMGR_STATISTICS_TYPE_TX_GET,          /**< tx get cnt */
    ECNT_FEMGR_STATISTICS_TYPE_TX_DROP,         /**< tx drop cnt */
}ECNT_FEMGR_STATISTICS_TYPE;

///@brief FE PCP coding mode
typedef enum
{
    ECNT_FEMGR_PCP_CODING_MODE_8P0D = 0,    /**< 8P0D */
    ECNT_FEMGR_PCP_CODING_MODE_7P1D,        /**< 7P1D */
    ECNT_FEMGR_PCP_CODING_MODE_6P2D,        /**< 6P2D */
    ECNT_FEMGR_PCP_CODING_MODE_5P3D,        /**< 5P3D */
    ECNT_FEMGR_PCP_CODING_MODE_MAX,
}ECNT_FEMGR_PCP_CODING_MODE;

///@brief FE VIP match type
typedef enum
{
    ECNT_FEMGR_VIP_PATTERN_TYPE_ETHER_TYPE = 0, /**< match ether type */
    ECNT_FEMGR_VIP_PATTERN_TYPE_PPPOE_PROTOCOL, /**< match pppoe protocol */
    ECNT_FEMGR_VIP_PATTERN_TYPE_IP_PROTOCOL,    /**< match ip protocol */
    ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS,      /**< match tcp ports */
    ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS,      /**< match udp ports */
    ECNT_FEMGR_VIP_PATTERN_TYPE_MAX,
}ECNT_FEMGR_VIP_PATTERN_TYPE;

///@brief FE GDM shaping mode
typedef enum
{
    ECNT_FEMGR_GDM_SHAPING_MODE_BPS = 0,    /**< byte per second */
    ECNT_FEMGR_GDM_SHAPING_MODE_PPS,        /**< packet per second */
    ECNT_FEMGR_GDM_SHAPING_MODE_MAX,
}ECNT_FEMGR_GDM_SHAPING_MODE;

///@brief FE reset mode
typedef enum
{
    ECNT_FEMGR_RESET_MODE_SCU,      /**< SCU reset */
    ECNT_FEMGR_RESET_MODE_CORE,     /**< CORE reset */
    ECNT_FEMGR_RESET_MODE_MAX,
}ECNT_FEMGR_RESET_MODE;

///@brief FE traffic direction
typedef enum
{
    ECNT_FEMGR_TRAFFIC_DIRECTON_UPSTREAM = 0, /**<  upstream */
    ECNT_FEMGR_TRAFFIC_DIRECTON_DOWNSTREAM,   /**<  downstream */
    ECNT_FEMGR_TRAFFIC_DIRECTON_MAX,
}ECNT_FEMGR_TRAFFIC_DIRECTION;

///@brief FE meter mode
typedef enum
{
    ECNT_FEMGR_METER_MODE_CAR = 0,  /**< QoS CAR mode */
    ECNT_FEMGR_METER_MODE_IP,       /**< IP bind ratelimit */
    ECNT_FEMGR_METER_MODE_IF,       /**< IF bind ratelimit */
    ECNT_FEMGR_METER_MODE_OLT,      /**< OLT CTRL ratelimit*/
    ECNT_FEMGR_METER_MODE_MAX,
}ECNT_FEMGR_METER_MODE;

///@brief FE meter rate mode
typedef enum
{
    ECNT_FEMGR_METER_RATE_MODE_BPS = 0, /**< byte per second */
    ECNT_FEMGR_METER_RATE_MODE_PPS,     /**< packet per second */
    ECNT_FEMGR_METER_RATE_MODE_MAX,
}ECNT_FEMGR_METER_RATE_MODE;

///@brief FE meter tick mode
typedef enum
{
    ECNT_FEMGR_METER_TICK_MODE_FAST = 0, /**< fast tick mode */
    ECNT_FEMGR_METER_TICK_MODE_SLOW,     /**< slow tick mode */
    ECNT_FEMGR_METER_TICK_MODE_MAX,
}ECNT_FEMGR_METER_TICK_MODE;

///@brief FE enable/disable mode
typedef enum {
	ECNT_FEMGR_DISABLE = 0 ,
	ECNT_FEMGR_ENABLE 
}ECNT_FEMGR_ENABLE_MODE;	

///@brief FE multicast vlan operation mode
typedef enum {
	ECNT_FEMGR_MC_VLAN_KEEP = 0 ,
	ECNT_FEMGR_MC_VLAN_REPLACE,
	ECNT_FEMGR_MC_VLAN_ADD,
	ECNT_FEMGR_MC_VLAN_REMOVE
}ECNT_FEMGR_MCVLAN_OP_MODE;

///@brief FE multicast vlan vpm mode
typedef enum {
	ECNT_FEMGR_MC_VPM_KEEP = 0 ,
	ECNT_FEMGR_MC_VPM_8100,
	ECNT_FEMGR_MC_VPM_88a8,
	ECNT_FEMGR_MC_VPM_CSR
} ECNT_FEMGR_MCVLAN_VPM_MODE;

///@brief FE rx mac filter 
typedef enum
{
    ECNT_FEMGR_MAC_FILTER_DISABLE = 0, /**< disable GDM2/3 Rx MAC filter  */
	ECNT_FEMGR_MAC_FILTER_ENABLE,      /**< config rx mac filter parameter */
	ECNT_FEMGR_MAC_FILTER_ADD,     	   /**< add a mac to filter */
    ECNT_FEMGR_MAC_FILTER_DEL,     	   /**< delete a mac from filter */
    ECNT_FEMGR_MAC_FILTER_MAX,
}ECNT_FEMGR_MAC_FILTER_CMD;

///@brief FE ratelimit packet type
typedef enum
{
    ECNT_FEMGR_PKT_TYPE_BC = 0, /**< broadcast  */		
    ECNT_FEMGR_PKT_TYPE_MC, /**< multicast  */
}ECNT_FEMGR_RATELIMIT_PKT_TYPE;

/************************************************************************
*                            M A C R O S
*************************************************************************
*/

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
#ifndef _uint_defined
#define _uint_defined
typedef unsigned int uint;
#endif

#ifndef _ushort_defined
#define _ushort_defined
typedef unsigned short ushort;
#endif

#ifndef _uchar_defined
#define _uchar_defined
typedef unsigned char uchar;
#endif

#ifndef _unchar_defined
#define _unchar_defined
typedef unsigned char unchar;
#endif

#ifndef _u8_defined
#define _u8_defined
typedef unsigned char u8;
#endif
        
#ifndef _u16_defined
#define _u16_defined
typedef unsigned short u16;
#endif
    
#ifndef _u32_defined
#define _u32_defined
typedef unsigned int u32;
#endif

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
}ECNT_PPPOE_INFO_DATA;

///@brief FE GDMA2 rx statistics
typedef struct
{
    unsigned long long oversize;           /**< over sized cnt */
    unsigned long long undersize;          /**< under sized cnt */
    unsigned long long frame_cnt;          /**< packet cnt */
    unsigned long long frame_len;          /**< byte cnt */
    unsigned long long drop_cnt;           /**< drop cnt */
    unsigned long long broadcast;          /**< broadcast cnt */
    unsigned long long multicast;          /**< multicast cnt */
    unsigned long long crc;                /**< crc cnt */
    unsigned long long fragment;           /**< fragment cnt */
    unsigned long long jabber;             /**< jabber cnt */
    unsigned long long less_64;            /**< packet length < 64*/
    unsigned long long more_1518;          /**< packet length > 1518 */
    unsigned long long eq_64;              /**< packet length = 64*/
    unsigned long long from_65_to_127;     /**< packet length between 65-127 */
    unsigned long long from_128_to_255;    /**< packet length between 128-255 */
    unsigned long long from_256_to_511;    /**< packet length between 256-511 */
    unsigned long long from_512_to_1023;   /**< packet length between 512-1023 */
    unsigned long long from_1024_to_1518;  /**< packet length between 1024-1518*/
}ECNT_FEMGR_GDMA2_RX_STATISTICS;

///@brief FE GDMA2 tx statistics
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
}ECNT_FEMGR_GDMA2_TX_STATISTICS;

///@brief FE VIP configuration
typedef struct
{
    ECNT_FEMGR_VIP_PATTERN_TYPE type;   /**< VIP type */
    union
    {
        u16 ether_type;                 /**< ether type */
        u16 pppoe_protocol;             /**< pppoe protocol */
        u16 ip_protocol;                /**< ip protocol */
        struct
        {
            u16 src_port;               /**< tcp/udp source port*/
            u16 dst_port;               /**< tcp/udp dest port*/
            u8 src_port_en;             /**< enable source port match*/
            u8 dst_port_en;             /**< enable dest port match*/
            u8 swap_port_en;            /**< enable swap src/dst match*/
        }tcp, udp;
    }u;
}ECNT_FEMGR_VIP_ENTRY;

///@brief FE L2LU key configuration
typedef struct
{
    u8 svid_en:1;       /**< check svlan id */
    u8 spcp_en:1;       /**< check svlan pcp */
    u8 cvid_en:1;       /**< check cvlan id */
    u8 cpcp_en:1;       /**< check cvlan pcp */
    u8 dscp_mask_en;    /**< DSCP comparison mask bits */
    u8 is_dscp;         /**< indicate  dscp_or_traffic_class is DSCP or TRAFFIC_CLASS */

    u16 svid;           /**< svlan id, including 3bits pcp */
    u16 cvid;           /**< cvlan id, including 3bits pcp  */
    u8 dscp_or_traffic_class;        /**< DSCP or TRAFFIC_CLASS in IP header. */
}ECNT_FEMGR_L2LU_KEY_ENTRY;

///@brief FE TLS configuration
typedef struct
{
    u8 uni_port_id;  /**< uni port id, range [0~3] */
    u16 uni_vlan;    /**< uni vlan id, range [2~4094,4095], 4095 means invalid */
    u16 gem_port;    /**< gemport id */
    u16 ani_vlan;    /**< range [2~4094,4095], 4095 means invalid */
}ECNT_FEMGR_TLS_ENTRY;

typedef struct
{
    u8 index ;			/*rue index*/
    u8 enable ;			/*this rule if active?*/
    u32 rate ;
    u8 ip_version; 		/*0: IPv4 , 1: IPv6*/
    u8 v4_pair_l[4] ; 	/*IPv4 range*/
    u8 v4_pair_h[4] ;
    u8 v6_pair_l[16] ;	/*IPv6 range*/
    u8 v6_pair_h[16] ;
}ECNT_FEMGR_RATELIMIT_IP_INFO;

typedef struct
{
    u8 index ;			/*rule index*/
    u8 enable ;			/*this rule if active?*/
    u32 rate ;
}ECNT_FEMGR_RATELIMIT_IF_INFO;

typedef struct
{
    u32 rate ;
    u32 enable;
    u8 index;
    u32 dir;    /*1 up stream 0 down stream*/
    u8  mode;   /*1 ip, else:itf*/
}ECNT_FEMGR_METER_INFO;

///@brief FE meter configuration
typedef struct
{
    ECNT_FEMGR_METER_MODE mode;             /**< meter mode */
    u8 index;                               /**< meter index, QoS CAR mode, range[0, 5], IP bind mode, range[0, 7], IF bind mode, range[0, 15]  */
    u8 enable;                              /**< enable/disable meter */
    ECNT_FEMGR_TRAFFIC_DIRECTION dir;       /**< traffic direction: upstream/downstream */
    ECNT_FEMGR_METER_RATE_MODE rate_mode;   /**< meter rate mode: bps/pps */
    u32 rate;                               /**< rate */
    ECNT_FEMGR_METER_TICK_MODE tick_mode;   /**< meter tick mode */
    u32 bucket_size;                        /**< bucket size, 0 by default */
    ECNT_FEMGR_RATELIMIT_IP_INFO ip_info;
    ECNT_FEMGR_RATELIMIT_IF_INFO if_info;
    char itf_name[16];
}ECNT_FEMGR_METER;

///@brief FE TRFC configuration
typedef struct
{
	unsigned char smac[6];
	unsigned char dmac[6];
	unsigned int sip;
	unsigned int dip;
	unsigned short sipv6[8];
	unsigned short dipv6[8];
	unsigned short sport;
	unsigned short dport;
	unsigned short eth_type;
	unsigned char tc_min;
	unsigned char tc_max;
	unsigned char tos_min;
	unsigned char tos_max;
	unsigned char cpbit_min;
	unsigned char cpbit_max;
	unsigned short cvid_min;
	unsigned short cvid_max;
	unsigned char spbit_min;
	unsigned char spbit_max;
	unsigned short svid_min;
	unsigned short svid_max;
	/* each bit means if need to care the key above or not */
	unsigned int mask; 
	unsigned char field_num;
}ECNT_FEMGR_TRFC_INFO;

///@brief FE multicast vlan table cfg
typedef struct
{
	unsigned char   		mc_table_id;

	ECNT_FEMGR_ENABLE_MODE  enable;
	unsigned short  		ppe_entry_id;
}ECNT_FEMGR_MC_VLAN_TABLE_t;

///@brief FE multicast vlan action cfg
typedef struct
{
	unsigned char   			mc_table_id;
	unsigned char   			mc_chnl_id;

	ECNT_FEMGR_MCVLAN_OP_MODE  	mc_vlan_op;
	ECNT_FEMGR_MCVLAN_VPM_MODE 	mc_vlan_vpm;
	unsigned short  			mc_vlan;
}ECNT_FEMGR_MC_VLAN_ACTION_t;

///@brief FE RX_MAC_FILTER configuration
typedef struct
{
	unsigned char cmd_id;
	unsigned char gdm_sel;
	unsigned char pkt_type;
	unsigned char mac_sel;
	unsigned char action;
	unsigned char list_type;
	unsigned char mac[6];
}ECNT_FEMGR_MAC_FILTER_INFO;


/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/**
@brief Set FrameEngine MYMAC
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] mac mac address
@param[in] mask mask len
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_mymac(ECNT_FEMGR_GDM_SEL gdm_sel, u8 *mac, u32 mask);

/**
@brief Set FrameEngine maximum/minimum packet length
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] max_len maximum packet length
@param[in] min_len minimum packet length
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_pkt_length(ECNT_FEMGR_GDM_SEL gdm_sel, u32 max_len, u32 min_len);

/**
@brief Set FrameEngine ext TPID
@param[in] tpid TPID
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_ext_tpid(u16 tpid);

/**
@brief Get FrameEngine ext TPID
@param[out] p_tpid TPID
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_ext_tpid(u16 *p_tpid);

/**
@brief Set FrameEngine drop frame configuration
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] type frame error type
@param[in] enable enable/disable dropping
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_drop_frame_enable(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE type, u8 enable);

/**
@brief Get FrameEngine GDMA statistics
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] type statistics type
@param[out] p_cnt counter
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_cnt(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_STATISTICS_TYPE type, u32 *p_cnt);

/**
@brief Set FrameEngine L2B blacklist
@param[in] enable enable/disable black list
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_l2b_black_list(u8 enable);

/**
@brief Set FrameEngine L2B ether type
@param[in] index index of the entry
@param[in] enable enable/disable
@param[in] is_pppoe indicate pppoe or ether
@param[in] ether_type ether protocol
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_l2b_ether_type(u32 index, u8 enable, u8 is_pppoe, u16 ether_type);

/**
@brief Get FrameEngine GDMA2 rx statistics
@param[out] p_stats GDMA2 rx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_gdma2_rx_statistics(ECNT_FEMGR_GDMA2_RX_STATISTICS *p_stats);

/**
@brief Get FrameEngine GDMA2 tx statistics
@param[out] p_stats GDMA2 tx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_gdma2_tx_statistics(ECNT_FEMGR_GDMA2_TX_STATISTICS *p_stats);

/**
@brief Get FrameEngine hsgmii lan rx statistics
@param[in] hsgmii_idx 0:pcie 0 1 :pcie 1 2 :usb as hsgmii lan
@param[out] p_stats Hsgmii rx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_hsgmii_rx_statistics(ECNT_FEMGR_GDMA2_RX_STATISTICS *p_stats, int hsgmii_idx);


/**
@brief Get FrameEngine hsgmii lan tx statistics
@param[in] hsgmii_idx 0:pcie 0 1 :pcie 1 2 :usb as hsgmii lan
@param[out] p_stats Hsgmii tx statistics
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_hsgmii_tx_statistics(ECNT_FEMGR_GDMA2_TX_STATISTICS *p_stats, int hsgmii_idx);

/**
@brief Clear FrameEngine GDMA statistics
@param[in] gdm_sel indicate GDMA1/GDMA2
@param[in] txrx_sel packet direction
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_clear_gdma_statistics(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_DIRECTION txrx_sel);

/**
@brief Set FrameEngine GDMA2 tx shaping
@param[in] mode shaping mode
@param[in] rate shaping rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_gdma2_tx_rate(ECNT_FEMGR_GDM_SHAPING_MODE mode, u32 rate);

/**
@brief Set FrameEngine GDMA2 rx shaping
@param[in] type pkt type
@param[in] mode shaping mode
@param[in] rate shaping rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_gdma2_rx_rate(ECNT_FEMGR_PKT_TYPE type, ECNT_FEMGR_GDM_SHAPING_MODE mode, u32 rate);

/**
@brief Add FrameEngine VIP entry
@param[in] p_vip VIP entry
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_add_vip(ECNT_FEMGR_VIP_ENTRY *p_vip);

/**
@brief Del FrameEngine VIP entry
@param[in] p_vip VIP entry
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_del_vip(ECNT_FEMGR_VIP_ENTRY *p_vip);

/**
@brief Set FrameEngine meter ratelimit by ip or itf
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_meter_ratelimit_by(ECNT_FEMGR_METER_INFO *p_meter_info);

/**
@brief Set FrameEngine meter ratelimit rule by ip 
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_ip_ratelimit_rule(ECNT_FEMGR_RATELIMIT_IP_INFO *ip_info, u32 dir);

/**
@brief Set FrameEngine meter ratelimit rule by itf 
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_if_ratelimit_rule(ECNT_FEMGR_RATELIMIT_IF_INFO *if_info, u32 dir);

/**
@brief Set FrameEngine meter ratelimit mode
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_meter_ratelimit_mode(u32 dir, u32 idx, u32 mode);

/**
@brief Get FrameEngine meter ratelimit mode
@param[in] p_meter meter ratelimit configuration
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_meter_ratelimit_mode(u32 dir, u32 idx, uint *ratelimit_mode);

/**
@brief Get FrameEngine meter ratelimit value
@param[in] p_meter meter ratelimit configuration 
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_meter_ratelimit(u32 idx, u32 dir, u32 mode, u32 *rate);
/**
@brief Get flow byte count 
@param[in] p_meter meter ratelimit configuration 
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/

ECNT_FEMGR_RET fe_lib_set_olt_ctl_ratelimit(u16 dir,  u16 index, u32 ratelimit);
/**
@brief set meter ratelimit in olt ctrl mode
@param[in]  dir  : 0:upstream   1:downstream
			index: meter index , start from zero
			ratalimit: ratelimit value in kbps
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/

ECNT_FEMGR_RET fe_lib_get_flow_byte_cnt(uint dir, uint flowid, unsigned long long *cnt);

/**
@brief Get flow byte & pkt count 
@param[in] p_meter meter ratelimit configuration 
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_flow_byte_pkt_cnt(uint dir, uint flowid, unsigned long long *byte_cnt, unsigned long long *pkt_cnt);

/**
@brief Get  GDM2 ethernet count
@param[in] cmd RX/TX 
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_eth_cnt_common(int cmd);
/**
@brief Get  GDM2 ethernet rx count
@param[in]  
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_eth_rx_cnt();
/**
@brief Get  GDM2 ethernet tx count
@param[in]  
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_eth_tx_cnt();
/**
@brief Get  GDM2 ethernet frame count
@param[in]  txrx_sel packet direction
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_eth_frame_cnt(ECNT_FEMGR_DIRECTION txrx_sel);
/**
@brief Get  GDM2 ethernet err frame count
@param[in]  err_type error type
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_eth_err_cnt(uint err_type);
/**
@brief Get rx crc err count
@param[in]  
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode

*/
ECNT_FEMGR_RET fe_lib_get_rx_err_crc_cnt();


/**
@brief Set FrameEngine lan mac range
@param[in] mac_high high 24bit of lan mac
@param[in] mac_low_min low 24bit min of lan mac
@param[in] mac_low_max low 24bit max of lan mac
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7516
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_lan_local_mac_range(unsigned char* mac_high, unsigned char* mac_low_min, unsigned char* mac_low_max);

/**
@brief Set FrameEngine wan mac range
@param[in] mac_high high 24bit of wan mac
@param[in] mac_low_min low 24bit min of wan mac
@param[in] mac_low_max low 24bit max of wan mac
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7516
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_wan_local_mac_range(unsigned char* mac_high, unsigned char* mac_low_min, unsigned char* mac_low_max);

/**
@brief Set CAR mode ratelimit
@param[in] rate ratelimit value 
@param[in] enable enable/disable ratelimit
@param[in] idx CAR queue index
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_car_mode_ratelimit(u32 rate, u8 enable, u8 idx);

/**
@brief Set Accouting Group0 mode
@param[in] dir traffic direction
@param[in] mode accounting group mode
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_acnt0_mode(uint dir, uint mode);

/**
@brief Set Accouting Group1 mode
@param[in] dir traffic direction
@param[in] mode accounting group mode
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_acnt1_mode(uint dir, uint mode);

/**
@brief Add traffic class
@param[in] trfc traffic class information
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_add_traffic_class(ECNT_FEMGR_TRFC_INFO trfc);

/**
@brief Remove traffic class
@param[in] trfc traffic class information
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_del_traffic_class(ECNT_FEMGR_TRFC_INFO trfc);


/**
@brief clean hw path packet account statistics
@param[in] mode account mode range in FE_ACNT_Mode_t
@param[in] dir upstream or downstream
@param[in] in_data input parameters
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7528
@endcode
*/
ECNT_FEMGR_RET fe_lib_clean_account(unsigned char mode, int dir, void *in_data);


/**
@brief get hw path packet account statistics
@param[in] mode account mode range in FE_ACNT_Mode_t
@param[in] dir upstream or downstream
@param[in] in_data input parameters
@param[out] bytecnt output byte counter
@param[out] pktcnt output packet counter
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7528
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_account(unsigned char mode, int dir, void *in_data, unsigned long long *bytecnt, unsigned long long *pktcnt);


/**
@brief config gdm rx mac filter
@param[in] mac filter information
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_rx_mac_filter(ECNT_FEMGR_MAC_FILTER_INFO macfilter_cfg);


/**
@brief set FE multicast vlan action enable
@param[in] enable or disable
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_mc_vlan_global(ECNT_FEMGR_ENABLE_MODE enable);


/**
@brief get FE multicast vlan action enable
@param[in] enable or disable
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_mc_vlan_global(ECNT_FEMGR_ENABLE_MODE *enable);


/**
@brief set FE multicast vlan table info
@param[in] mcVlanTableCfg include table id, enable, and PPE entry id
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_mc_vlan_table_cfg(ECNT_FEMGR_MC_VLAN_TABLE_t *mcVlanTableCfg);


/**
@brief get FE multicast vlan table info
@param[in] mcVlanTableCfg include table id, enable, and PPE entry id
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_mc_vlan_table_cfg(ECNT_FEMGR_MC_VLAN_TABLE_t *mcVlanTableCfg);


/**
@brief set FE multicast vlan action info
@param[in] mcVlanActionCfg include table id, channel id, vlan operation, vlan vpm, and vlan id
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_mc_vlan_action_cfg(ECNT_FEMGR_MC_VLAN_ACTION_t *mcVlanActionCfg);


/**
@brief get FE multicast vlan action info
@param[in] mcVlanActionCfg include table id, channel id, vlan operation, vlan vpm, and vlan id
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_mc_vlan_action_cfg(ECNT_FEMGR_MC_VLAN_ACTION_t *mcVlanActionCfg);


/**
@brief set FE multicast vlan table clear
@param[in] mc_table_id, multicast table index
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_mc_vlan_clear_all(unsigned char mc_table_id);

/**
@brief clear ratelimit config
@param[in] 0 broadcast 1 multicast
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_pkt_ratelimit_clear(ECNT_FEMGR_RATELIMIT_PKT_TYPE pkt_type);

/**
@brief send us&ds pkt to same rx ring,other cmd is needed to do ratelimit
@param[in] 0 broadcast 1 multicast
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_pkt_ratelimit_total(ECNT_FEMGR_RATELIMIT_PKT_TYPE pkt_type);

/**
@brief send us&ds pkt to different rx ring,other cmd is needed to do ratelimit
@param[in] 0 broadcast 1 multicast
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7580
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_pkt_ratelimit_singal(ECNT_FEMGR_RATELIMIT_PKT_TYPE pkt_type);

/**
@brief Set LAN mymac for ppe
@param[in] mac address
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_FEMGR_RET fe_api_set_lan_local_mac(unsigned char* mac);

/**
@brief Set WAN mymac for ppe
@param[in] mac address
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_set_wan_local_mac(unsigned char* mac);

/**
@brief get gdm2 tx bytes
@param[in] tx bytes pointer
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_get_gdma2_tx_byte(unsigned int* txByteCnt);

/**
@brief get flow byte,need flow classify to support this api
@param[in] upstream or downstream
@param[in] flowid
@param[in] byte cnt pointer
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_get_flow_byte_cnt(unsigned int direction, unsigned int flowid, unsigned long long* cnt);

/**
@brief set flow ratelimit,need flow classify to support this api
@param[in] upstream or downstream
@param[in] flowid
@param[in] uint is bps
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_set_flow_ratelimit(unsigned int direction, unsigned int flowid, unsigned int rate_bps);

/**
@brief get flow ratelimit,need flow classify to support this api
@param[in] upstream or downstream
@param[in] flowid
@param[in] uint is bps
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_get_flow_ratelimit(unsigned int direction, unsigned int flowid, unsigned int* rate_bps);

/**
@brief set upstream ratelimit by interface
@param[in] interface info
@param[in] enable or disable
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_set_upstream_ratelimit_by_itf(ECNT_FEMGR_RATELIMIT_IF_INFO *itf_info, unsigned char enable);

/**
@brief set upstream ratelimit by ip
@param[in] ip info
@param[in] enable or disable
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_set_upstream_ratelimit_by_ip(ECNT_FEMGR_RATELIMIT_IP_INFO *ip_info, unsigned char enable);

/**
@brief set upstream ratelimit by ip
@param[in] interface name
@param[in] direction (tx or rx)
@param[in] unit is kbps, 0 means no ratelimit
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_api_set_wifi_ssid_ratelimit(unsigned char *itf_name,unsigned char *direction,unsigned int rate_kbps);

/**
@brief get tx traffic for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_tx_traffic(char gdm_sel, int lan_port, uint *rate);

/**
@brief get rx traffic for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_rx_traffic(char gdm_sel, int lan_port, uint *rate);

/**
@brief get tx rate for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_tx_rate(char gdm_sel, int lan_port, uint *rate);

/**
@brief get rx rate for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] rate
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_rx_rate(char gdm_sel, int lan_port, uint *rate);

/**
@brief get tx octets for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] frame bytes count
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_tx_octets(char gdm_sel, int lan_port, uint *frameByteCnt);

/**
@brief get rx octets for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] frame bytes count
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_rx_octets(char gdm_sel, int lan_port, uint *frameByteCnt);

/**
@brief get rx discard counter for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] discard counter
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_rx_discard_counter(char gdm_sel, int lan_port, uint *discardCnt);

/**
@brief get tx discard counter for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] discard counter
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_tx_discard_counter(char gdm_sel, int lan_port, uint *discardCnt);

/**
@brief get rx error counter for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] error counter
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_rx_error_counter(char gdm_sel, int lan_port, uint *discardCnt);

/**
@brief get tx error counter for ME255
@param[in] 0 GDM_LAN 1 GDM_WAN
@param[in] port id
@param[in] error counter
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_get_tx_error_counter(char gdm_sel, int lan_port, uint *discardCnt);

/**
@brief add stb whilte,only configed ip stb is allowed
@param[in] 0 stb ipv4 address
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_add_stb_src_ipv4(uint ip4);

/**
@brief delete stb whilte
@param[in] 0 stb ipv4 address
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_del_stb_src_ipv4(uint ip4);

/**
@brief add stb whilte,only configed ip stb is allowed
@param[in] 0 stb ipv6 address
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_add_stb_src_ipv6(char *ip6);

/**
@brief delete stb whilte
@param[in] 0 stb ipv6 address
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_FEMGR_RET fe_lib_del_stb_src_ipv6(char *ip6);

/**
@brief set gdma pcp code
@param[in] gdm_sel: gdma1/2/3 txrx_sel: tx/rx, mode:pcpMode
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7512, EN7528
@endcode
*/

ECNT_FEMGR_RET fe_lib_set_gdma_pcp_coding(ECNT_FEMGR_GDM_SEL gdm_sel, ECNT_FEMGR_DIRECTION txrx_sel, ECNT_FEMGR_PCP_CODING_MODE mode);

/**
@brief set the statistics and ratelimit of upstream by mesh mode
@param[in] wan_conn_type (pon, lan or wlan)
@param[in] wan_itf_name
@return ::ECNT_FEMGR_RET
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_FEMGR_RET fe_lib_set_dev_stat_ratelimit_mode(unsigned int wan_conn_type, char *wan_itf_name);

// ============== redefine  =================
#define ecnt_fe_lib_set_mymac fe_lib_set_mymac
#define ecnt_fe_lib_set_pkt_length fe_lib_set_pkt_length
#define ecnt_fe_lib_set_ext_tpid fe_lib_set_ext_tpid
#define ecnt_fe_lib_get_ext_tpid fe_lib_get_ext_tpid
#define ecnt_fe_lib_set_drop_frame_enable fe_lib_set_drop_frame_enable
#define ecnt_fe_lib_get_cnt fe_lib_get_cnt
#define ecnt_fe_lib_set_l2b_black_list fe_lib_set_l2b_black_list
#define ecnt_fe_lib_set_l2b_ether_type fe_lib_set_l2b_ether_type
#define ecnt_fe_lib_get_gdma2_rx_statistics fe_lib_get_gdma2_rx_statistics
#define ecnt_fe_lib_get_gdma2_tx_statistics fe_lib_get_gdma2_tx_statistics
#define ecnt_fe_lib_clear_gdma_statistics fe_lib_clear_gdma_statistics
#define ecnt_fe_lib_set_gdma2_tx_rate fe_lib_set_gdma2_tx_rate
#define ecnt_fe_lib_set_gdma2_rx_rate fe_lib_set_gdma2_rx_rate
#define ecnt_fe_lib_add_vip fe_lib_add_vip
#define ecnt_fe_lib_del_vip fe_lib_del_vip
#define ecnt_fe_lib_set_meter_ratelimit_by fe_lib_set_meter_ratelimit_by
#define ecnt_fe_lib_set_ip_ratelimit_rule fe_lib_set_ip_ratelimit_rule
#define ecnt_fe_lib_set_if_ratelimit_rule fe_lib_set_if_ratelimit_rule
#define ecnt_fe_lib_set_meter_ratelimit_mode fe_lib_set_meter_ratelimit_mode
#define ecnt_fe_lib_get_meter_ratelimit fe_lib_get_meter_ratelimit
#define ecnt_fe_lib_get_meter_ratelimit_mode fe_lib_get_meter_ratelimit_mode
#define ecnt_fe_lib_get_flow_byte_cnt fe_lib_get_flow_byte_cnt
#define ecnt_fe_lib_get_eth_rx_cnt fe_lib_get_eth_rx_cnt
#define ecnt_fe_lib_get_eth_tx_cnt fe_lib_get_eth_tx_cnt
#define ecnt_fe_lib_get_eth_frame_cnt fe_lib_get_eth_frame_cnt
#define ecnt_fe_lib_get_eth_err_cnt fe_lib_get_eth_err_cnt
#define ecnt_fe_lib_get_rx_err_crc_cnt fe_lib_get_rx_err_crc_cnt
#define ecnt_fe_lib_set_car_mode_ratelimit fe_lib_set_car_mode_ratelimit
#define ecnt_fe_lib_set_forcebw_wrr_mode_ratelimit fe_lib_set_forcebw_wrr_mode_ratelimit
#define ecnt_fe_lib_set_mc_vlan_global fe_lib_set_mc_vlan_global
#define ecnt_fe_lib_get_mc_vlan_global fe_lib_get_mc_vlan_global
#define ecnt_fe_lib_set_mc_vlan_table_cfg fe_lib_set_mc_vlan_table_cfg
#define ecnt_fe_lib_get_mc_vlan_table_cfg fe_lib_get_mc_vlan_table_cfg
#define ecnt_fe_lib_set_mc_vlan_action_cfg fe_lib_set_mc_vlan_action_cfg
#define ecnt_fe_lib_get_mc_vlan_action_cfg fe_lib_get_mc_vlan_action_cfg
#define ecnt_fe_lib_set_mc_vlan_clear_all fe_lib_set_mc_vlan_clear_all
#define ecnt_fe_lib_set_pkt_ratelimit_clear fe_lib_set_pkt_ratelimit_clear
#define ecnt_fe_lib_set_pkt_ratelimit_total fe_lib_set_pkt_ratelimit_total
#define ecnt_fe_lib_set_pkt_ratelimit_singal fe_lib_set_pkt_ratelimit_singal
#define ecnt_fe_lib_clean_account fe_lib_clean_account
#define ecnt_fe_lib_get_account fe_lib_get_account
#define ecnt_fe_api_set_lan_local_mac fe_api_set_lan_local_mac
#define ecnt_fe_api_set_wan_local_mac fe_api_set_wan_local_mac
#define ecnt_fe_api_get_gdma2_tx_byte fe_api_get_gdma2_tx_byte
#define ecnt_fe_api_get_flow_byte_cnt fe_api_get_flow_byte_cnt
#define ecnt_fe_api_set_flow_ratelimit fe_api_set_flow_ratelimit
#define ecnt_fe_api_get_flow_ratelimit fe_api_get_flow_ratelimit
#define ecnt_fe_api_set_upstream_ratelimit_by_itf fe_api_set_upstream_ratelimit_by_itf
#define ecnt_fe_api_set_upstream_ratelimit_by_ip fe_api_set_upstream_ratelimit_by_ip
#define ecnt_fe_api_set_wifi_ssid_ratelimit fe_api_set_wifi_ssid_ratelimit
#define ecnt_fe_lib_set_acnt0_mode fe_lib_set_acnt0_mode
#define ecnt_fe_lib_set_acnt1_mode fe_lib_set_acnt1_mode
#define ecnt_fe_lib_set_acnt2_mode fe_lib_set_acnt2_mode
#define ecnt_fe_lib_get_dev_mac_account fe_lib_get_dev_mac_account
#define ecnt_fe_lib_clear_dev_mac_account fe_lib_clear_dev_mac_account
#define ecnt_fe_lib_get_multicast_pkt_account fe_lib_get_multicast_pkt_account
#define ecnt_fe_lib_clear_multicast_pkt_account fe_lib_clear_multicast_pkt_account
#define ecnt_fe_lib_get_wan_itf_pkt_account fe_lib_get_wan_itf_pkt_account
#define ecnt_fe_lib_clear_wan_itf_pkt_account fe_lib_clear_wan_itf_pkt_account
#define ecnt_fe_lib_get_llid_pkt_account fe_lib_get_llid_pkt_account
#define ecnt_fe_lib_clear_llid_pkt_account fe_lib_clear_llid_pkt_account
#define ecnt_fe_lib_add_stb_src_ipv4 fe_lib_add_stb_src_ipv4
#define ecnt_fe_lib_del_stb_src_ipv4 fe_lib_del_stb_src_ipv4
#define ecnt_fe_lib_add_stb_src_ipv6 fe_lib_add_stb_src_ipv6
#define ecnt_fe_lib_del_stb_src_ipv6 fe_lib_del_stb_src_ipv6
#define ecnt_fe_lib_get_dev_mac_account_pkts fe_lib_get_dev_mac_account_pkts


#endif /* _API_LIB_FE_H */

