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
///@file libapi_lib_ppe.h
///@brief PPE Module API

#ifndef _API_LIB_PPE_H
#define _API_LIB_PPE_H

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/
///@brief Return value of PPE API
typedef enum
{
    ECNT_PPEMGR_SUCCESS = 0,    /**< api execution success */
    ECNT_PPEMGR_INVALID_PARAM,  /**< invalid parameter */
    ECNT_PPEMGR_NO_MEMORY,      /**< not enough memory */
    ECNT_PPEMGR_FAIL,           /**< api execution fail */
}ECNT_PPEMGR_RET;

///@brief PPE transparent line service source port
typedef enum
{
    ECNT_PPEMGR_TLS_SPORT_WAN  = 2,     /**< wan */
    ECNT_PPEMGR_TLS_SPORT_LAN0 = 8,     /**< lan0 */
    ECNT_PPEMGR_TLS_SPORT_LAN1 = 9,     /**< lan1 */
    ECNT_PPEMGR_TLS_SPORT_LAN2 = 10,    /**< lan2 */
    ECNT_PPEMGR_TLS_SPORT_LAN3 = 11,    /**< lan3 */
}ECNT_PPEMGR_TLS_SPORT_TYPE;

///@brief PPE transparent line service mode
typedef enum
{
    ECNT_PPEMGR_TLS_MODE_UNI = 0,   /**< uni */
    ECNT_PPEMGR_TLS_MODE_UNTAG,     /**< un-tag */
    ECNT_PPEMGR_TLS_MODE_PRITAG,    /**< pri-tag */
    ECNT_PPEMGR_TLS_MODE_CTAG,      /**< c-tag */
    ECNT_PPEMGR_TLS_MODE_STAG_SIN,  /**< s-tag-sin */
    ECNT_PPEMGR_TLS_MODE_DBLTAG,    /**< dbl-tag */
    ECNT_PPEMGR_TLS_MODE_NON_CTAG,  /**< non-c-tag */
    ECNT_PPEMGR_TLS_MODE_STAG_SPE,  /**< s-tag-spe */
    ECNT_PPEMGR_TLS_MODE_TYPE_MAX,
}ECNT_PPEMGR_TLS_MODE_TYPE;

///@brief PPE transparent line service miss action
typedef enum
{
    ECNT_PPEMGR_TLS_MISS_ACTION_NON_TLS = 0,    /**< non tls */
    ECNT_PPEMGR_TLS_MISS_ACTION_DROP,           /**< drop */
    ECNT_PPEMGR_TLS_MISS_ACTION_TYPE_MAX,
}ECNT_PPEMGR_TLS_MISS_ACTION_TYPE;

///@brief PPE transparent line service vlan config pattern
typedef enum
{
    ECNT_PPEMGR_TLS_VID_PATTERN_NONE = 0,   /**< do not care about vlan id */
    ECNT_PPEMGR_TLS_VID_PATTERN_VID,        /**< input is vlanid, 4 ids at most */
    ECNT_PPEMGR_TLS_VID_PATTERN_RANGE,      /**< input is some range */
    ECNT_PPEMGR_TLS_VID_TYPE_MAX,
}ECNT_PPEMGR_TLS_VID_PATTERN_TYPE;

///@brief PPE rate mode
typedef enum
{
    ECNT_PPEMGR_RATE_MODE_BYTE = 0,    /**< byte mode */
    ECNT_PPEMGR_RATE_MODE_PACKET,      /**< packet mode */
    ECNT_PPEMGR_RATE_MODE_TYPE_MAX,
}ECNT_PPEMGR_RATE_MODE_TYPE;

///@brief PPE rate byte mode scale
typedef enum
{
    ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_K = 0,      /**<  1Kbps */
    ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_M,          /**<  1Mbps */
    ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_G,          /**<  1Gbps */
    ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_TYPE_MAX,   
}ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_TYPE;

///@brief PPE meter traffic direction
typedef enum
{
    ECNT_PPEMGR_TRAFFIC_DIR_UPSTREAM = 0, /**<  upstream */
    ECNT_PPEMGR_TRAFFIC_DIR_DOWNSTREAM,   /**<  downstream */
    ECNT_PPEMGR_TRAFFIC_DIR_TYPE_MAX,   
}ECNT_PPEMGR_TRAFFIC_DIR_TYPE;

///@brief PPE TLS hash type
typedef enum
{
    ECNT_PPEMGR_HASH_TLS_DMAC = 0,      /**< dmac */
    ECNT_PPEMGR_HASH_TLS_PBIT,          /**< pbits */
    ECNT_PPEMGR_HASH_TLS_ETYPE_DSCP,    /**< others: etype, dscp */
    ECNT_PPEMGR_HASH_TLS_TYPE_MAX,
}ECNT_PPEMGR_HASH_TLS_TYPE;

///@brief PPE FOE binding traffic direction
typedef enum
{
    ECNT_PPEMGR_FOE_BINDING_DIR_UPSTREAM = 0, /**<  upstream */
    ECNT_PPEMGR_FOE_BINDING_DIR_DOWNSTREAM,   /**<  downstream */
    ECNT_PPEMGR_FOE_BINDING_DIR_BIDIRECTION,  /**<  bi-direction */
    ECNT_PPEMGR_FOE_BINDING_DIR_TYPE_MAX,   
}ECNT_PPEMGR_FOE_BINDING_DIR_TYPE;

///@brief PPE FOE ip protocol check type
typedef enum
{
    ECNT_PPEMGR_FOE_IPPROTO_CHECK_MODE_WHITE_LIST = 0, /**<  white list mode */
    ECNT_PPEMGR_FOE_IPPROTO_CHECK_MODE_BLACK_LIST,     /**<  black list mode */
    ECNT_PPEMGR_FOE_IPPROTO_CHECK_MODE_MAX,   
}ECNT_PPEMGR_FOE_IPPROTO_CHECK_MODE;

///@brief PPE FOE flow type
typedef enum
{
    ECNT_PPEMGR_FOE_FLOW_TYPE_IPV4NAPT = 0, /**< ipv4 napt */
    ECNT_PPEMGR_FOE_FLOW_TYPE_IPV4NAT,      /**< ipv4 nat */
    ECNT_PPEMGR_FOE_FLOW_TYPE_L2B,          /**< l2b */
    ECNT_PPEMGR_FOE_FLOW_TYPE_DSLITE,       /**< dslite */
    ECNT_PPEMGR_FOE_FLOW_TYPE_IPV63T,       /**< ipv6 3tuple */
    ECNT_PPEMGR_FOE_FLOW_TYPE_IPV65T,       /**< ipv6 5tuple */
    ECNT_PPEMGR_FOE_FLOW_TYPE_6RD,          /**< 6rd */
    ECNT_PPEMGR_FOE_FLOW_TYPE_MAX,
}ECNT_PPEMGR_FOE_FLOW_TYPE;

///@brief PPE FOE destination port
typedef enum
{
    ECNT_PPEMGR_FOR_DST_PORT_QDMA_LAN_CPU = 0,  /**< QDMA1 CPU port */
    ECNT_PPEMGR_FOR_DST_PORT_GDM_LAN,           /**< GDM1 port */
    ECNT_PPEMGR_FOR_DST_PORT_GDM_WAN,           /**< GDM2 port */
    ECNT_PPEMGR_FOR_DST_PORT_QDMA_LAN_HW,       /**< QDMA1 HW port */
    ECNT_PPEMGR_FOR_DST_PORT_PPE,               /**< PPE port */
    ECNT_PPEMGR_FOR_DST_PORT_QDMA_WAN_CPU,      /**< QDMA2 CPU port */
    ECNT_PPEMGR_FOR_DST_PORT_QDMA_WAN_HW,       /**< QDMA2 HW port */
    ECNT_PPEMGR_FOR_DST_PORT_DROP,              /**< DROP port */
    ECNT_PPEMGR_FOR_DST_PORT_TYPE_MAX,
}ECNT_PPEMGR_FOR_DST_PORT_TYPE;

///@brief PPE FOE tunnel action
typedef enum
{
    ECNT_PPEMGR_FOE_TUNNEL_ACTION_INSERT = 0,  /**< insert tunnel header */
    ECNT_PPEMGR_FOE_TUNNEL_ACTION_REMOVE,      /**< remove tunnel header */
    ECNT_PPEMGR_FOE_TUNNEL_ACTION_TYPE_MAX,
}ECNT_PPEMGR_FOE_TUNNEL_ACTION_TYPE;

///@brief PPE FOE L2B rule entry type
typedef enum
{
    ECNT_PPEMGR_FOE_L2B_ENTRY_DMAC_SMAC = 0,            /**< dmac , smac is valid of a L2B entry */
    ECNT_PPEMGR_FOE_L2B_ENTRY_DMAC_INVLAN_HSKL2,       /**< dmac , in_vlan, hsk_l2 is valid of a L2B entry */
    ECNT_PPEMGR_FOE_L2B_ENTRY_DMAC_SMAC_INVLAN_HSKL2,  /**< dmac , smac, in_vlan, hsk_l2 is valid of a L2B entry */
    ECNT_PPEMGR_FOE_L2B_ENTRY_MAX,
}ECNT_PPEMGR_FOE_L2B_ENTRY_TYPE;

///@brief PPE FOE entry retrieve type
typedef enum
{
    ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE_INVALID,    /**< invalid entry */
    ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE_UNBOUND,    /**< unbound entry */
    ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE_BOUND,      /**< bound entry */
    ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE_MAX,
}ECNT_PPEMGR_FOE_ENRTRY_GET_TYPE;

///@brief PPE IP PROTO
typedef enum
{
    ECNT_PPEMGR_IP_PROTO_IPV4 = 4,   
    ECNT_PPEMGR_IP_PROTO_IPV6 = 6, 
}ECNT_PPEMGR_IP_PROTO;

/************************************************************************
*                            M A C R O S
*************************************************************************
*/
///@brief PPE transparent line service miss action
#define ECNT_PPEMGR_TLS_MAXIMUM_VID_NUM 5

///@brief PPE meter maximum group id
#define ECNT_PPEMGR_METER_MAXIMUM_GROUP_ID 62

///@brief PPE trtcm maximum group id
#define ECNT_PPEMGR_TRTCM_MAXIMUM_GROUP_ID 30

///@brief PPE minimum rate of byte mode
#define ECNT_PPEMGR_RATE_BYTE_MODE_MIN_RATE (16 * 1000)

///@brief PPE maximum rate of byte mode
#define ECNT_PPEMGR_RATE_BYTE_MODE_MAX_RATE (2 * 1024 * 1024 * 1000)

///@brief PPE accounting group maximum group id
#define ECNT_PPEMGR_ACCOUNTING_MAXIMUM_GROUP_ID 62

///@brief PPE cache maximum mode value
#define ECNT_PPEMGR_CACHE_MODE_MAX_VALUE 3

///@brief PPE hash maximum mode value
#define ECNT_PPEMGR_HASH_MODE_MAX_VALUE 3

///@brief PPE hash maximum bit shift value
#define ECNT_PPEMGR_HASH_BITSHIFT_MAX_VALUE 15

///@brief PPE FOE maximum table size
#define ECNT_PPEMGR_MAXIMUM_TABLE_SIZE (16 * 1024)

///@brief PPE FOE invalid accounting group id
#define ECNT_PPEMGR_INVALID_ACCOUNTING_GROUP_ID 0x3f

///@brief PPE FOE invalid meter group id
#define ECNT_PPEMGR_INVALID_METER_GROUP_ID 0x3f

///@brief PPE FOE invalid info2 
#define ECNT_PPEMGR_INVALID_INFO2_VALUE 0xffffffff

///@brief PPE FOE invalid vlan_layer
#define ECNT_PPEMGR_INVALID_VLAN_LAYER 0xff


/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
#ifndef u8
    typedef unsigned char u8;
#endif
        
#ifndef u16
    typedef unsigned short u16;
#endif
    
#ifndef u32
    typedef unsigned int u32;
#endif

///@brief PPE transparent line service config entry
typedef struct
{
    ECNT_PPEMGR_TLS_SPORT_TYPE  sport;  /**< transparent line service source port */
    ECNT_PPEMGR_TLS_MODE_TYPE   mode;   /**< transparent line service vlan mode */
    ECNT_PPEMGR_TLS_MISS_ACTION_TYPE    miss_action;    /**< transparent line service miss action */
    ECNT_PPEMGR_TLS_VID_PATTERN_TYPE    vlan_type;      /**< transparent line service vlan config pattern */
    u8 vid_num;                                 /**< transparent line service vlan number configured */
    u16 vids[ECNT_PPEMGR_TLS_MAXIMUM_VID_NUM];  /**< transparent line service vlan ids, only valid when  vlan_type is set to ::ECNT_PPEMGR_TLS_VID_PATTERN_VID*/
    u16 vid_min;                                /**< transparent line service minimum vlan, only valid when  vlan_type is set to ::ECNT_PPEMGR_TLS_VID_PATTERN_RANGE*/
    u16 vid_max;                                /**< transparent line service maximum vlan, only valid when  vlan_type is set to ::ECNT_PPEMGR_TLS_VID_PATTERN_RANGE*/
}ECNT_PPEMGR_TLS_ENTRY;

///@brief PPE meter group config
typedef struct
{
    u8 group_id;    /**< meter group id, should be <= ::ECNT_PPEMGR_METER_MAXIMUM_GROUP_ID */
    u8 enable;      /**< enable/disable meter group */
    ECNT_PPEMGR_RATE_MODE_TYPE mode;   /**< meter rate mode, valid when enable is set TRUE */
    ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_TYPE scale;   /**<  meter scale, only for byte mode, valid when enable is set TRUE */
    u32 rate;       /**< meter rate, valid when enable is set TRUE */
}ECNT_PPEMGR_METER_GROUP;

///@brief PPE meter service config entry
typedef struct
{
    ECNT_PPEMGR_TRAFFIC_DIR_TYPE direction;   /**< traffic direction */
    u16 vlan;       /**< ingress vlan id */
    u8 queue;       /**< destination queue id */
    u8 group_id;    /**< meter group id, should be <= ::ECNT_PPEMGR_METER_MAXIMUM_GROUP_ID */
}ECNT_PPEMGR_METER_FLOW;

///@brief PPE trtcm group config
typedef struct
{
    u8 group_id;    /**< trtcm group id, should be <= ::ECNT_PPEMGR_TRTCM_MAXIMUM_GROUP_ID */
    u8 enable;      /**< enable/disable trtcm group */
    ECNT_PPEMGR_RATE_MODE_TYPE mode;   /**< trtcm rate mode, valid when enable is set TRUE */
    ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_TYPE cir_scale;   /**<  trtcm scale for cir, only for byte mode, valid when enable is set TRUE */
    ECNT_PPEMGR_RATE_BYTE_MODE_SCALE_TYPE pir_scale;   /**<  trtcm scale for pir, only for byte mode, valid when enable is set TRUE */
    u32 cir;       /**< committed information rate, valid when enable is set TRUE */
    u32 pir;       /**< peak information rate, valid when enable is set TRUE */
}ECNT_PPEMGR_TRTCM_GROUP;

///@brief PPE trtcm service config entry
typedef struct
{
    ECNT_PPEMGR_TRAFFIC_DIR_TYPE direction;   /**< traffic direction */
    u16 vlan;       /**< ingress vlan id */
    u8 group_id;    /**< meter group id, should be <= ::ECNT_PPEMGR_TRTCM_MAXIMUM_GROUP_ID */
}ECNT_PPEMGR_TRTCM_FLOW;

///@brief PPE accounting group flow entry
typedef struct
{
    ECNT_PPEMGR_TRAFFIC_DIR_TYPE direction;   /**< traffic direction */
    u16 vlan;       /**< ingress vlan id */
    u8 group_id;    /**< account group id, should be <= ::ECNT_PPEMGR_ACCOUNTING_MAXIMUM_GROUP_ID */
}ECNT_PPEMGR_ACCOUNT_FLOW;

///@brief PPE accounting group statistics
typedef struct
{
    u32 byte_cnt_high;  /**< high 32bit of byte counter */
    u32 byte_cnt_low;   /**< low 32bit of byte counter */
    u32 packet_cnt;     /**< ingress vlan id */
}ECNT_PPEMGR_ACCOUNT_STATS;

///@brief PPE foe entry
typedef struct
{
    ECNT_PPEMGR_FOE_FLOW_TYPE type;     /**< FOE rule type */
    u8 smac[6];             /**< ingress source mac */
    u8 dmac[6];             /**< ingress dest mac */
    u32 src_ip4;            /**< ingress source IPv4 address */
    u32 dst_ip4;            /**< ingress dest IPv4 address */
    u32 src_ip6[4];         /**< ingress source IPv6 address */
    u32 dst_ip6[4];         /**< ingress dest IPv6 address */
    u32 src_port;           /**< ingress source l4 port */
    u32 dst_port;           /**< ingress dest l4 port */
    u32 new_src_ip4;        /**< egress source IPv4 address */
    u32 new_dst_ip4;        /**< egress dest IPv4 address */
    u32 new_src_ip6[4];     /**< egress source IPv6 address */
    u32 new_dst_ip6[4];     /**< egress dest IPv6 address */
    u32 new_src_port;       /**< egress source l4 port */
    u32 new_dst_port;       /**< egress dest l4 port */
    u8 ttl;                 /**< ttl decrease value, the default value should be set to 1 */
    u16 vlan1;              /**< vlan1 */
    u16 vlan2;              /**< vlan2 */
    u16 stag;               /**< STAG */
    u16 pppoe_id;           /**< PPPoE id */
    u8 is_udp;              /**< tcp/udp indicator */
    ECNT_PPEMGR_FOR_DST_PORT_TYPE dst_port_id;          /**< FOE rule dest port */
    u8 hash_index_shift;    /**< hash index offset */
    u8 new_dscp;            /**< egress packet dscp */
    u32 info2;              /**< INFO2 value, should be set to ::ECNT_PPEMGR_INVALID_INFO2_VALUE if not used */
    ECNT_PPEMGR_FOE_TUNNEL_ACTION_TYPE tunnel_action;   /**< tunnel action */
    u8 accounting_group_id; /**< accounting group id, should be set to ::ECNT_PPEMGR_INVALID_ACCOUNTING_GROUP_ID if not used */
    u8 meter_group_id;      /**< meter group id, should be set to ::ECNT_PPEMGR_INVALID_METER_GROUP_ID if not used */
    u8 ipv6_flow_label;     /**< IPv6 flow label */
    u8 vpm;                 /**< VPM */
    u8 protocol;            /**< IPv4 protocol/IPv6 next header */
    u8 tunnel_dscp;         /**< tunnel:DSCP/TRFC */
    u8 tunnel_ttl;          /**< tunnel:IPv4 TTL/IPv6 HPOL */
    u8 tunnel_ipv4_flag;    /**< tunnel: Ipv4 flag */
    u8 tsid;                /**< TSID[5:1]/TSE[bit 0] */
    u8 queue_id;            /**< [QID] */
    u8 pcp;                 /**< PCP */
    u8 vlan_layer;          /**< vlan layer number, should be set to ::ECNT_PPEMGR_INVALID_VLAN_LAYER if not used */
    u16 in_vlan;            /**< ingress vlan id, only for L2B type */
    u32 hsk_l2;             /**< HSK_L2, only for L2B type */
    u8 channel;             /**< channel */
    u32 hsk_mc;             /**< HSK_MC, only for Multicast type */
    u16 in_gre_call_id;     /**< ingress gre call id */
    u16 out_gre_call_id;    /**< egress gre call id */
    u16 ifc_idx;            /**< ifc index after en7580 */

    ECNT_PPEMGR_FOE_L2B_ENTRY_TYPE l2b_type; /**< L2B entry type, only for get operation */
    u16 hash_index;         /**< hash index of the FOE entry, only for get operation */
}ECNT_PPEMGR_FOE_ENTRY;

typedef struct hwnat_wan_acnt {
    unsigned long long uniTxCnt;
    unsigned long long uniRxCnt;  
    unsigned long long mulTxCnt;
    unsigned long long mulRxCnt;
    unsigned long long uniTxBytes;
    unsigned long long uniRxBytes;
    unsigned long long mulTxBytes;
    unsigned long long mulRxBytes;
	unsigned long long txRate;
    unsigned long long rxRate;
    unsigned long jiffies;
}ECNT_PPEMGR_WAN_ACCOUNT;

struct hwnat_wan_acnt_args {
    unsigned char wanid;
    unsigned char action;
    struct hwnat_wan_acnt wan_acnt;
};


/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/**
@brief Add PPE transparent line service
@param[in] p_tls TLS configuration
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_transparet_line_service(ECNT_PPEMGR_TLS_ENTRY *p_tls);

/**
@brief Remove PPE transparent line service
@param[in] p_tls TLS configuration
@return ::ECNT_PPEMGR_RET
@note In this case, only ECNT_SWITCHMGR_LAN2LAN_SRC_PORT::sport is needed in the parameter.
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_transparet_line_service(ECNT_PPEMGR_TLS_ENTRY *p_tls);

/**
@brief Set PPE hash mode
@param[in] mode PPE hash mode, range [0, ::ECNT_PPEMGR_HASH_MODE_MAX_VALUE]
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_mode(u8 mode);

/**
@brief Set PPE hash bit shift
@param[in] shift PPE hash bit shift, range [0, ::ECNT_PPEMGR_HASH_BITSHIFT_MAX_VALUE]
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_bitshift(u8 shift);

/**
@brief Set PPE hash tls
@param[in] type tls hash type
@param[in] enable enable/disable tls hash type
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_tls(ECNT_PPEMGR_HASH_TLS_TYPE type, u8 enable);

/**
@brief Set PPE L2B source mac hash enable
@param[in] enable enable/disable l2b smac hash
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_hash_l2b_smac(u8 enable);

/**
@brief Set PPE localin offload enable
@param[in] enable enable/disable localin
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_localin_enable(u8 enable);

/**
@brief Enable PPE localin offload for specific dst port
@param[in] port_id port id, range [0, 8], 0 means enable all port
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_localin_dst_port(u16 port_id);

/**
@brief Disable PPE localin offload for specific dst port
@param[in] port_id port id, range [0, 8], 0 means disable all port
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_localin_dst_port(u16 port_id);

/**
@brief Set PPE FOE binding threshold
@param[in] num binding threshold, packet number per second. Default value is 30.
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_binding_threshold(u16 num);

/**
@brief Set PPE FOE binding entry number limit during a time stamp unit
@param[in] quater_limit entry number limit when total entries less than 1/4 of the total num
@param[in] half_limit entry number limit when total entries between 1/4 and 1/2 of the total num
@param[in] full_limit entry number limit when total entries more than 1/2 of the total num
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_binding_limit(u16 quater_limit, u16 half_limit, u16 full_limit);

/**
@brief Set PPE FOE entry keepalive internal for tcp and udp rules
@param[in] tcp_ka_time keep alive internal for tcp, unit: 4 seconds
@param[in] udp_ka_time keep alive internal for udp, unit: 4 seconds
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_keep_alive_interval(u8 tcp_ka_time, u8 udp_ka_time);

/**
@brief Set PPE FOE unbound entry age time
@param[in] time age time for unbound entry, unit: 1 second
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_unbound_age_time(u8 time);

/**
@brief Set PPE FOE bound entry age time
@param[in] tcp_time time age time for bound tcp entry, unit: 1 second
@param[in] udp_time time age time for bound udp entry, unit: 1 second
@param[in] tcp_fin_time time age time for bound tcp fin entry, unit: 1 second
@param[in] ntu_time time age time for bound non tcp/udp entry, unit: 1 second
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_bound_age_time(u16 tcp_time, u16 udp_time, u16 tcp_fin_time, u16 ntu_time);

/**
@brief Set PPE FOE entry binding direction
@param[in] direction enable traffic flow offload by specific direction
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_foe_binding_direction(ECNT_PPEMGR_FOE_BINDING_DIR_TYPE direction);

/**
@brief query ipv6 unknow multicast drop list in router mode
@param[in] mca multicast address
@param[in] proto ipv4/ipv6
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_query_multicast_drop_list(u8 *mca, ECNT_PPEMGR_IP_PROTO proto);

/**
@brief drop ipv6 unknow multicast in router mode
@param[in] entry_index foe entry index
@param[in] foe_ai foe cpu reason
@param[in] foe_magic foe magic
@param[in] proto ipv4/ipv6
@param[in] grp_addr multicast group address
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_drop_unknow_multicast_by_index(u32 entry_index,u8 foe_ai,u32 foe_magic,ECNT_PPEMGR_IP_PROTO proto,u8* grp_addr);

/**
@brief ppe set meter ratelimit
@param[in] meterGroup meter group index
@param[in] rate unit is Kbps
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_meter_ratelimit(unsigned int meterGroup, unsigned int rate);

/**
@brief Set WAN rate limit use pps mode threshold value
@param[in] threshold, threshold value
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_wan_rate_threshold(unsigned int threshold);

/**
@brief Set WAN MTU value
@param[in] wan_type, 1: router, 0: bridge
@param[in] mtu value
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_wan_mtu(unsigned char wan_type, unsigned int mtu);

/**
@brief ppe set wan ratelimit
@param[in] wanid wan interface id
@param[in] direction upstream or downstream
@param[in] ratelimit unit is Kbps
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_wan_ratelimit(unsigned char wanid, unsigned char direction, unsigned int ratelimit);

/**
@brief ppe add black list ip/port/protocol
@param[in] ip ip address
@param[in] port port number
@param[in] protocol ip protocol
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_add(unsigned int ip, unsigned int port, unsigned int protocol);

/**
@brief ppe delete black list ip/port/protocol
@param[in] ip ip address
@param[in] port port number
@param[in] protocol ip protocol
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_delete(unsigned int ip, unsigned int port, unsigned int protocol);

/**
@brief ppe delete black list all
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_delete_all(void);

/**
@brief ppe delete black list show
@return ::ECNT_PPEMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ip_port_protocol_show(void);

/**
@brief mirror the HWF packet to LAN
@param[in] port 1~4:lan1~lan4
@return ::HWNAT_SUCCESS = 0, HWNAT_FAIL = 1
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_enable(unsigned char port);

/**
@brief mirror the HWF packet to LAN
@return ::HWNAT_SUCCESS = 0, HWNAT_FAIL = 1
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_disable();

/**
@brief set ppe mirror mode
@param[in] mode
@return ::HWNAT_SUCCESS = 0, HWNAT_FAIL = 1
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_mode(unsigned char mode);

/**
@brief set ppe mirror mode
@param[in] enable,fport,nbq,sptag
@return ::HWNAT_SUCCESS = 0, HWNAT_FAIL = 1
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_enable_config(int enable,int fport,int nbq,int sptag);

/**
@brief set ppe mirror mode
@param[in] mode,mirror_dst,mirror_src,mirror_mask
@return ::HWNAT_SUCCESS = 0, HWNAT_FAIL = 1
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_packet_mirror_mode_config(int mode,int mirror_dst,int mirror_src,int mirror_mask);

/**
@brief set assigned WAN interface MTU
@param[in] assigned WAN interface index
@param[in] MTU
@return ::HWNAT_SUCCESS = 0, HWNAT_FAIL = 1
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_wan_mtu(int wan_idx, int wan_mtu);

/**
@brief ppe add monitor ip
@param[in] the format of ip should be in hexadecimal
@param[in] if it is fixed ip ,mask should be 32;if it is ip range,mask should from 0-31
@param[in] 0 means only monitor up stream; 1 means minotor up and down stream;2 means monitor up and down stream
@param[in] sign the same type ip;from 0-31;
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ip_add(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign);

/**
@brief ppe add monitor ipv6
@param[in] ipv6 addr
@param[in] if it is fixed ip ,mask should be 128;if it is ip range,mask should from 0-128;
@param[in] 0 means only monitor up stream; 1 means minotor up and down stream;2 means monitor up and down stream
@param[in] sign the same type ip;from 0-31;
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ipv6_add(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign);

/**
@brief ppe delete monitor ip
@param[in] the format of ip should be in hexadecimal
@param[in] if it is fixed ip ,mask should be 32;if it is ip range,mask should from 0-31
@param[in] 0 means only monitor up stream; 1 means minotor up and down stream;2 means monitor up and down stream
@param[in] sign the same type ip;from 0-31;
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ip_delete(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign);

/**
@brief ppe delete monitor ipv6
@param[in] ipv6 addr
@param[in] if it is fixed ip ,mask should be 128;if it is ip range,mask should from 0-128;
@param[in] 0 means only monitor up stream; 1 means minotor up and down stream;2 means monitor up and down stream
@param[in] sign the same type ip;from 0-31;
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ipv6_delete(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign);

/**
@brief ppe delete monitor ip in same type(same sign)
@param[in] sign the same type ip;from 0-31;
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_black_ip_delete_same_sign(unsigned int sign);

/**
@brief ppe set ppe mtu
@param[in] mtu, the mtu of ppe mtu;
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_mtu(unsigned short mtu);

/**
@brief ppe add nptv6 prefix
@param[in] ifname, vr wan interface name;
@param[in] snpt_str, format is xxxx:xxxx::/xx;
@param[in] dnpt_str, format is xxxx:xxxx::/xx;
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_add_nptv6_prefix(unsigned char* ifname,unsigned char* snpt_str,unsigned char* dnpt_str);

/**
@brief ppe del nptv6 prefix
@param[in] ifname, vr wan interface name;
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_nptv6_prefix(unsigned char* ifname);

/**
@brief ppe del all nptv6 prefix
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_del_nptv6_prefix_all(void);

/**
@brief ppe del nptv6 prefix
@param[in] mode 0 -- reverse mode 1 -- prefix mode
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_nptv6_mode(unsigned char mode);

/**
@brief ppe del nptv6 prefix
@param[out] p_mode 0 -- reverse mode 1 -- prefix mode
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_get_nptv6_mode(unsigned char* p_mode);

/******************************************************************************
function name:	ppemgr_lib_clean_unicast_entry
 Descriptor:	It's used to delete unicast hwnat entry
 Input Args:		

 Ret Value:		0: success
 				1: fail
 other:
******************************************************************************/
int ppemgr_lib_clean_unicast_entry(void);

/**
@brief ppe clear wan acnt
@param[in] wan_id 
@par Supported Chip:
@code
EN7523
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_set_wan_acnt_clear(u16 wan_id);

/**
@brief enable ppe2
@param[in] void 
@par Supported Chip:
@code
EN7581
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_ppe2_enable(void);

/**
@brief disable ppe2
@param[in] void 
@par Supported Chip:
@code
EN7581
@endcode
*/
ECNT_PPEMGR_RET ppemgr_lib_ppe2_disable(void);

// ============== redefine  =================
#define ecnt_ppemgr_lib_add_transparet_line_service ppemgr_lib_add_transparet_line_service
#define ecnt_ppemgr_lib_del_transparet_line_service ppemgr_lib_del_transparet_line_service
#define ecnt_ppemgr_lib_set_hash_mode ppemgr_lib_set_hash_mode
#define ecnt_ppemgr_lib_set_hash_bitshift ppemgr_lib_set_hash_bitshift
#define ecnt_ppemgr_lib_set_hash_tls ppemgr_lib_set_hash_tls
#define ecnt_ppemgr_lib_set_hash_l2b_smac ppemgr_lib_set_hash_l2b_smac
#define ecnt_ppemgr_lib_set_localin_enable ppemgr_lib_set_localin_enable
#define ecnt_ppemgr_lib_add_localin_dst_port ppemgr_lib_add_localin_dst_port
#define ecnt_ppemgr_lib_del_localin_dst_port ppemgr_lib_del_localin_dst_port
#define ecnt_ppemgr_lib_set_foe_binding_threshold ppemgr_lib_set_foe_binding_threshold
#define ecnt_ppemgr_lib_set_foe_binding_limit ppemgr_lib_set_foe_binding_limit
#define ecnt_ppemgr_lib_set_foe_keep_alive_interval ppemgr_lib_set_foe_keep_alive_interval
#define ecnt_ppemgr_lib_set_foe_unbound_age_time ppemgr_lib_set_foe_unbound_age_time
#define ecnt_ppemgr_lib_set_foe_bound_age_time ppemgr_lib_set_foe_bound_age_time
#define ecnt_ppemgr_lib_set_foe_binding_direction ppemgr_lib_set_foe_binding_direction
#define ecnt_ppemgr_lib_query_multicast_drop_list ppemgr_lib_query_multicast_drop_list
#define ecnt_ppemgr_lib_drop_unknow_multicast_by_index ppemgr_lib_drop_unknow_multicast_by_index
#define ecnt_ppemgr_lib_set_meter_ratelimit ppemgr_lib_set_meter_ratelimit
#define ecnt_ppemgr_lib_set_wan_ratelimit ppemgr_lib_set_wan_ratelimit
#define ecnt_ppemgr_lib_black_ip_port_protocol_add ppemgr_lib_black_ip_port_protocol_add
#define ecnt_ppemgr_lib_black_ip_port_protocol_delete ppemgr_lib_black_ip_port_protocol_delete
#define ecnt_ppemgr_lib_black_ip_port_protocol_delete_all ppemgr_lib_black_ip_port_protocol_delete_all
#define ecnt_ppemgr_lib_black_ip_port_protocol_show ppemgr_lib_black_ip_port_protocol_show
#define ecnt_ppemgr_lib_packet_mirror ppemgr_lib_packet_mirror
#define ecnt_ppemgr_lib_wan_mtu ppemgr_lib_wan_mtu
#define ecnt_ppemgr_lib_black_ip_add ppemgr_lib_black_ip_add
#define ecnt_ppemgr_lib_black_ipv6_add ppemgr_lib_black_ipv6_add
#define ecnt_ppemgr_lib_black_ip_delete ppemgr_lib_black_ip_delete
#define ecnt_ppemgr_lib_black_ipv6_delete ppemgr_lib_black_ipv6_delete
#define ecnt_ppemgr_lib_black_ip_delete_same_sign ppemgr_lib_black_ip_delete_same_sign

#endif /* _API_LIB_PPE_H */

