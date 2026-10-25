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
///@file libapi_lib_switchmgr.h
///@brief Switch Module API
#ifndef _API_LIB_SWITCHMGR_H
#define _API_LIB_SWITCHMGR_H


/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/
///@brief Return value of switch API
typedef enum
{
    ECNT_SWITCHMGR_SUCCESS = 0, /**< api execution success */
    ECNT_SWITCHMGR_FAIL,        /**< api execution fail */
}ECNT_SWITCHMGR_RET;

///@brief Device chip type
typedef enum
{
    ECNT_SWITCH_CHIP_MT7520S = 0,  /**< MT7520S */
    ECNT_SWITCH_CHIP_MT7520F = 1,  /**< MT7520F */
    ECNT_SWITCH_CHIP_MT7520G = 2,  /**< MT7520G */
    ECNT_SWITCH_CHIP_MT7525F = 3,  /**< MT7525S */
    ECNT_SWITCH_CHIP_MT7525G = 4,  /**< MT7525G */
    ECNT_SWITCH_CHIP_EN7521F = 5,  /**< EN7521F */
    ECNT_SWITCH_CHIP_EN7526F = 6,  /**< EN7526F */
    ECNT_SWITCH_CHIP_EN751627 = 7,  /**< EN751627 */
    ECNT_SWITCH_CHIP_EN7528 = 8,  /**< EN7528 */
    ECNT_SWITCH_CHIP_EN7580  = 10, /**< EN7580 */
}ECNT_SWITCHMGR_CHIP_TYPE;

///@brief Port stp status
typedef enum
{
    ECNT_SWITCHMGR_STP_DISABLED      = 0, /**< all packets discarded */
    ECNT_SWITCHMGR_STP_BLOCKING      = 1, /**< only recv BPDU, discard all other packets */
    ECNT_SWITCHMGR_STP_LEARNING      = 2, /**< recv BPDU, discard all other packets but do mac learning */
    ECNT_SWITCHMGR_STP_FORWARDING    = 3, /**< forward all packets, */
    ECNT_SWITCHMGR_STP_PORT_LINKDOWN = 5, /**< link is down, this value is only for get action */
    ECNT_SWITCHMGR_STP_STATUS_MAX
}ECNT_SWITCHMGR_STP_STATUS;

///@brief Port speed mode configuration
typedef enum
{
    ECNT_SWITCHMGR_SPEED_MODE_AUTONEG   = 1, /**< auto negotiation */
    ECNT_SWITCHMGR_SPEED_MODE_100M_FULL = 2, /**< 100M Full Duplex */
    ECNT_SWITCHMGR_SPEED_MODE_100M_HALF = 3, /**< 100M Half Duplex */
    ECNT_SWITCHMGR_SPEED_MODE_10M_FULL  = 4, /**< 10M Full Duplex*/
    ECNT_SWITCHMGR_SPEED_MODE_10M_HALF  = 5, /**< 10M Half Duplex */
    ECNT_SWITCHMGR_SPEED_MODE_MAX,
}ECNT_SWITCHMGR_SPEED_MODE;

///@brief Port link speed state
typedef enum
{
    ECNT_SWITCHMGR_LINK_SPEED_1000M_FULL = 1, /**< 1000M Full Duplex */
    ECNT_SWITCHMGR_LINK_SPEED_100M_FULL  = 2, /**< 100M Full Duplex */
    ECNT_SWITCHMGR_LINK_SPEED_100M_HALF  = 3, /**< 100M Half Duplex */
    ECNT_SWITCHMGR_LINK_SPEED_10M_FULL   = 4, /**< 10M Full Duplex*/
    ECNT_SWITCHMGR_LINK_SPEED_10M_HALF   = 5, /**< 10M Half Duplex */
    ECNT_SWITCHMGR_LINK_SPEED_1000M_HALF = 6, /**< 1000M Half Duplex */
}ECNT_SWITCHMGR_LINK_SPEED;

///@brief Port link state
typedef enum
{
    ECNT_SWITCHMGR_LINK_DOWN = 0, /**< link up */
    ECNT_SWITCHMGR_LINK_UP   = 1, /**< link down */
}ECNT_SWITCHMGR_LINK_STATE;

/************************************************************************
*                            M A C R O S
*************************************************************************
*/
///@brief switch port number
#define ECNT_SWITCHMGR_PORT_NUM 7
///@brief switch port mac limit value for no limit
#define ECNT_SWITCHMGR_PORT_MAC_LIMIT_VALUE_NO_LIMIT 0xFFF
///@brief maximum mac acl rule number
#define ECNT_SWITCHMGR_TRTCM_MATCH_MAC_RULE_NUM 10

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
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

///@brief Port statistics
typedef struct
{
    u32 TxPktsCnt;              /**< tx packet count */
    u32 TxUniPktsCnt;           /**< tx unicast packet count */
    u32 TxBroadPktsCnt;         /**< tx broadcast packet count */
    u32 TxMultiPktsCnt;         /**< tx multicast packet count */
    u32 TxNonUniPktsCnt;        /**< tx not unicast packet count */
    u32 TxBytesCnt_Lo;          /**< tx bytes count (low 32bit) */
    u32 TxBytesCnt_Hi;          /**< tx bytes count (high 32bit) */
    u32 TxUnderSizePktsCnt;     /**< tx undersize packet count */
    u32 Tx64BytePktsCnt;        /**< tx 64 byte packet count */
    u32 Tx65_127BytePktsCnt;    /**< tx 65~127 byte packet count */
    u32 Tx128_255BytePktsCnt;   /**< tx 128~255 byte packet count */
    u32 Tx256_511BytePktsCnt;   /**< tx 256~511 byte packet count */
    u32 Tx512_1023BytePktsCnt;  /**< tx 512~1023 byte packet count */
    u32 Tx1024_MAXBytePktsCnt; /**< tx 1024~MAX byte packet count */
	u32 Tx1024_1518BytePktsCnt; /**< tx 1024~1518 byte packet count */
	u32 Tx1519_MAXBytePktsCnt; /**< tx 1518~MAX byte packet count */
    u32 TxOverSizePktsCnt;      /**< tx oversize packet count */
    u32 TxDropFramesCnt;        /**< tx drop frame count */
    u32 TxPauseFramesCnt;       /**< tx pause frame count */
    u32 TxCRCFramesCnt;         /**< tx crc frame count */
    u32 TxSingleCollsionEvent;  /**< tx single collision event count */
    u32 TxMultiCollsionEvent;   /**< tx multi collision event count */
    u32 TxLateCollisionEvent;   /**< tx late collision event count */
    u32 TxExcessCollisionEvent; /**< tx excessive collision event count */
    u32 TxDeferredEvent;        /**< tx deferred event count */
    u32 TxOverFlowFrameCnt;     /**< tx over flow frame count */
    u32 TxInternalMacErrorCnt;  /**< tx internal mac error count */
    u32 TXCarrierSenseErrorCnt; /**< tx carrier sense error count */
    u32 RxPktsCnt;              /**< rx packet count */
    u32 RxUniPktsCnt;           /**< rx unicast packet count */
    u32 RxBroadPktsCnt;         /**< rx broadcast packet count */
    u32 RxMultiPktsCnt;         /**< rx multicast packet count */
    u32 RxNonUniPktsCnt;        /**< rx not unicast packet count */
    u32 RxBytesCnt_Lo;          /**< rx bytes count (low 32bit) */
    u32 RxBytesCnt_Hi;          /**< rx bytes count (high 32bit) */
    u32 RxUnderSizePktsCnt;     /**< rx undersize packet count */
    u32 Rx64BytePktsCnt;        /**< rx 64 byte packet count */
    u32 Rx65_127BytePktsCnt;    /**< rx 65~127 byte packet count */
    u32 Rx128_255BytePktsCnt;   /**< rx 128~255 byte packet count */
    u32 Rx256_511BytePktsCnt;   /**< rx 256~511 byte packet count */
    u32 Rx512_1023BytePktsCnt;  /**< rx 512~1023 byte packet count */
	u32 Rx1024_MAXBytePktsCnt; /**< rx 1024~MAX byte packet count */
    u32 Rx1024_1518BytePktsCnt; /**< rx 1024~1518 byte packet count */
	u32 Rx1519_MAXBytePktsCnt; /**< rx 1519~MAX byte packet count */
    u32 RxOverSizePktsCnt;      /**< rx oversize frame count */
    u32 RxDropFramesCnt;        /**< rx drop frame count */
    u32 RxPauseFramesCnt;       /**< rx pause frame count */
    u32 RxCRCFramesCnt;         /**< rx crc frame count*/
    u32 RxOverFlowFrameCnt;     /**< rx over flow frame count*/
    u32 RxFragmentErrorCnt;     /**< rx fragment error count*/
    u32 RxJabberErrorCnt;       /**< rx jabber error count*/
    u32 RxAlignmentErrorCnt;    /**< rx allignment error count*/
}ECNT_SWITCHMGR_PORT_STATISTICS;

///@brief Mirror source port configuration
typedef struct
{
    u8 port;      /**< lan port number as source mirror port */
    u8 tx_enable; /**< mirror tx to destination port */
    u8 rx_enable; /**< mirror rx to destination port */
}ECNT_SWITCHMGR_LAN2LAN_SRC_PORT;

///@brief Retrieve all switch configuration
typedef struct
{
    u8 enable;                                  /**< global mirror enable/disable */
    ECNT_SWITCHMGR_LAN2LAN_SRC_PORT srcport[7]; /**< mirror source port */
    u8 srcportnum;                              /**< number of mirror source port enabled */
    u8 dstport;                                 /**< global mirror destination port */
}ECNT_SWITCHMGR_LAN2LAN_INFO;

///@brief Mirror session configuration
typedef struct
{
    u8 srcport;     /**< mirror source port */
    u8 tx_enable;   /**< mirror tx to destination port */
    u8 rx_enable;   /**< mirror rx to destination port */
    u8 dstport;     /**< global mirror destination port */
}ECNT_SWITCHMGR_LAN2LAN;

///@brief Switch mac table entry
typedef struct
{
    u8 port;    /**< switch port id*/
    u8 timer;   /**< aging time */
    u8 mac[6];  /**< mac address */
    u16 vid;    /**< vlan id */
}ECNT_SWITCHMGR_MAC_ENTRY;

///@brief Switch port qos mode configuration
typedef struct
{
    u8 qosmode;         /**< qos mode */
    u8 wrr_queue_cnt;   /**< wrr queue count */
    u8 wi[8];           /**< wrr queue weight */
	u8 eight_queue;
}ECNT_SWITCHMGR_QOS_CFG;

///@brief Switch mac table
typedef struct
{
    u32 count;  /**< mac table entry count */
    ECNT_SWITCHMGR_MAC_ENTRY macinfo[2048]; /**< mac table entry */
}ECNT_SWITCHMGR_BRIDGE_TABLE;

///@brief Switch port queue buffer threshold
typedef struct
{
    u8 Enable;              /**< enable */
    u32 time;               /**< time in us, only for get action */
    u16 MaxQueueThreshold;  /**< queue max threshold*/
    u16 MinQueueThreshold;  /**< queue min threshold*/
}ECNT_SWITCHMGR_QUEUE_BACK_PRESSURE;

///@brief Switch mac table entry
typedef struct
{
    u8 mac[6];              /**< mac address */
}ECNT_SWITCHMGR_PORT_MAC_ENTRY;

///@brief Switch mac table
typedef struct
{
    u8 vaild_entry_num; /**< mac table entry number */
    ECNT_SWITCHMGR_PORT_MAC_ENTRY portmac[16];  /**< mac table entry */
}ECNT_SWITCHMGR_PORT_ALL_MAC;

///@brief TRTCM entry
typedef struct
{
    u8  trtcmId;    /**< entry id */
    u16 CIR;        /**< committed information rate */
    u16 PIR;        /**< peak information rate  */
    u16 CBS;        /**< committed burst size  */
    u16 PBS;        /**< peak burst size */
}ECNT_SWITCHMGR_TRTCM;

///@brief TRTCM configuration
typedef struct
{
    u8 enable;  /**< enable/disable */
    ECNT_SWITCHMGR_TRTCM trtcm_acl_table[32];   /**< TRTCM entry */
}ECNT_SWITCHMGR_TRTCM_ACL;

///@brief Switch mac entry
typedef struct
{
    u32 mac[6];         /**< destination mac address*/
    u32 destportmap;    /**< destination port bitmap*/
    u32 leaky_en;       /**< leaky enable */
    u32 eg_tag;         /**< egress tag */
    u32 usr_pri;        /**< user priority */
    u32 sa_mir_en;      /**< source address mirror enable */
    u32 sa_port_fw;     /**< source address forward port */
}ECNT_SWITCHMGR_ARL_MAC_ENTRY;

///@brief Switch vlan configuration
typedef struct
{
    u8 index;       /**< index */
    u8 active;      /**< active */
    u8 portMap;     /**< port bitmap */
    u8 tagPortMap;  /**< tag port bitmap */
    u16 vid;        /**< vlan id */
    u8 ivl_en;      /**< ivl enable */
    u8 fid;         /**< fid */
    u16 stag;       /**< stag */
}ECNT_SWITCHMGR_VLAN_ENTRY;

///@brief Mirror source port configuration
typedef struct
{
    u8 port_tx_mir; /**< mirror tx packet */
    u8 port_rx_mir; /**< mirror rx packet */
    u8 vlan_mis;    /**< mirror vlan miss packet */
    u8 acl_mir;     /**< mirror packet matching acl */
    u8 igmp_mir;    /**< mirror igmp packet */
}ECNT_SWITCHMGR_PORT_MIRROR;

///@brief Switch qos drop policy
typedef struct
{
    u16 greenPacketDropQueueMaxThr;     /**< queue max threshold of green packet drop */
    u16 greenPacketDropQueueMinThr;     /**< queue min threshold of green packet drop */
    u16 yellowPacketDropQueueMaxThr;    /**< queue max threshold of yellow packet drop */
    u16 yellowPacketDropQueueMinThr;    /**< queue min threshold of yellow packet drop */
    u8 greenPacketDropMax_p;            /**< green packet drop max page */
    u8 yellowPacketDropMaxn_p;          /**< yellow packet drop max page */
    u8 QueueDropW_q;                    /**< queue drop page */
    u8 DropPrecdenceColourMarking;      /**< drop precdence color marking */
}ECNT_SWITCHMGR_DROP_POLICY;

///@brief switch traffic descriptor configuration
typedef struct
{
    u32 CIR;                    /**< cir */
    u32 PIR;                    /**< pir */
    u32 CBS;                    /**< cbs */
    u32 PBS;                    /**< pbs */
    u8 colourMode;              /**< color mode */
    u8 ingressColourMarking;    /**< ingress color marking */
    u8 engressColourMarking;    /**< egress color marking */
    u8 meterType;               /**< meter type */
    u8 direction;               /**< direction */
}ECNT_SWITCHMGR_TRAFFIC_DESCRIPTOR;

///@brief Switch timestamp
typedef struct
{
    u32 seconds;        /**< second */
    u32 nanoseconds;    /**< nano second*/
}ECNT_SWITCHMGR_TIMESTAMP;

///@brief Switch timestamp info
typedef struct
{
    ECNT_SWITCHMGR_TIMESTAMP tstamp;    /**< switch time stamp */
    u8 msg_type;                        /**< message type */
    u8 seq_id;                          /**< sequence id */
}ECNT_SWITCHMGR_TIMESTAMP_FIFO;

///@brief Switch port queue weight
typedef struct
{
    u8 port;        /**< lan port id */
    u8 weight[8];   /**< weight of queue0~7 */
}ECNT_SWITCHMGR_PORT_WEIGHT;

///@brief Entry of rate control by acl mac
typedef struct 
{
    int dev_mac[6];                 /**< match mac address */
    unsigned char ds_used;          /**< not used */
    unsigned char mac_entry_index;  /**< not used */
    int ds_queue_speed;             /**< down stream speed */
    int us_queue_speed;             /**< up stream speed*/
}ECNT_SWITCHMGR_ACL_MAC_SPEED_ENTRY;

///@brief Rate control by acl mac
typedef struct
{
    u8 enable;  /**< enable/disable */
    ECNT_SWITCHMGR_ACL_MAC_SPEED_ENTRY trtcm_acl_mac_table[ECNT_SWITCHMGR_TRTCM_MATCH_MAC_RULE_NUM];    /**< mac acl entries */
}ECNT_SWITCHMGR_DEV_SPEED;

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
}ECNT_GMAC_ETH_STA;

typedef struct
{
    u8 link_status;//0:down 1:up
    u8 auto_ng_status; //0:auto 1:force
    u8 duplex_status; //0:half-duplex 1:full-duplex
    u8 speed_status; //0:10Mbps 1:100Mbps 2:1000Mbps 3:2.5G bps
}ECNT_GMAC_ETH_LINKST;

typedef enum
{
    ECNT_GMAC_ETH_SPEED_MODE_AUTONEG    = 1, /**< auto negotiation */
    ECNT_GMAC_ETH_SPEED_MODE_10M_HALF   = 2, /**< 10M Half Duplex  */
    ECNT_GMAC_ETH_SPEED_MODE_10M_FULL   = 3, /**< 10M Full Duplex  */
    ECNT_GMAC_ETH_SPEED_MODE_100M_HALF  = 4, /**< 100M Half Duplex */
    ECNT_GMAC_ETH_SPEED_MODE_100M_FULL  = 5, /**< 100M Full Duplex */
    ECNT_GMAC_ETH_SPEED_MODE_1000M_FULL = 6, /**< 1000M Full Duplex>*/
}ECNT_GMAC_ETH_SPEED_MODE;

typedef struct
{
	u8 interrupt;
	u8 wuf_cnt;
	u8 mp_cnt;
	u8 da_cnt;
}ECNT_SWITCHMGR_WOL_STATUS;

typedef struct  
{
	u8 enable;
	u8 type;
	u8 sys_mac[6];
	u8 mul_mac[6][6];
	u8 index;
	u8 byte_mask[16];
	int byte_mask_num;
	u32 checksum;
	u32 final_byte;
	u32 match_len;
}ECNT_SWITCHMGR_WOL_CONFIG;

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_rx_byte(int port, unsigned int* RxByte_H, unsigned int* RxByte_L);

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_tx_byte(int port, unsigned int* TxByte_H, unsigned int* TxByte_L);

/**
@brief Set switch port mapping between lan(ether wan) ports and switch hw ports
@param[in] wan_port       switch hw port id for ether wan, range [-1, 6]
@param[in] lan_portmap  switch hw port id array for lan ports, range [-1, 6]
@return ::ECNT_SWITCHMGR_RET
@note when a wan_port or lan port is invalid, set the corresponding switch hw port id to -1.
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
@par Example:
@code
    int ret = 0;
    int wan_port = -1;
    int lan_portmap[6] = {1, 2, 3, 4, -1, -1};
    
    ret = switchmgr_lib_set_portmap(wan_port, lan_portmap);
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_portmap(int wan_port, int lan_portmap[6]);
ECNT_SWITCHMGR_RET switchmgr_lib_get_portmap(char *lan_portmap);

/**
@brief Set switch vlan function
@param[in] enable   enable/disable switch vlan function
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_active(u8 enable);

/**
@brief Set switch global flow control
@param[in] enable   enable/disable switch global flow control
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_flow_control(u8 enable);

/**
@brief Get switch global flow control
@param[out] p_enable    global flow control enable/disable
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_flow_control(u8 *p_enable);

/**
@brief Set switch port maximum frame size
@param[in] size maximum frame size
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_max_framesize(u16 size);

/**
@brief Get switch port maximum frame size
@param[out] p_size maximum frame size
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_max_framesize(u16 *p_size);

/**
@brief Set switch port isolate
@param[in] enable   enable/disable switch port isolate
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_isolate(u8 enable);

/**
@brief Set switch port matrix
@param[in] type             port group type:\n
0: general group\n
1: vlanbind group\n
2: portbind group
@param[in] port_group   lan port group
@return ::ECNT_SWITCHMGR_RET
@note if eth0.x is not exist, set -1;if no need to group, set 0.
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_matrix(int type, int port_group[6]); 


/**
@brief Set switch mirror info
@param[in] lan2lan     mirror configuration
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_lan2lan(ECNT_SWITCHMGR_LAN2LAN lan2lan);

/**
@brief Get switch mirror info
@param[out] p_lan2lan     mirror configuration
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_lan2lan(ECNT_SWITCHMGR_LAN2LAN_INFO *p_lan2lan);

/**
@brief Clear switch mirror configuration
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_clear_lan2lan(void);

/**
@brief Set switch dstq
@param[in] mode     lan dstq mode:\n
0: disabled\n
1: down stream forced to gdm1\n
2: down stream forced to qdma_lan
@param[in] fc_enable     flow control enable/disable
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_dstq(u8 mode, u8 fc_enable);

/**
@brief Set switch global mirror configuration
@param[in] enable   enable/disable mirror
@param[in] mirror_port  mirror destination port
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_mirror_enable(u8 enable, u8 mirror_port);

/**
@brief Set switch port qos enable
@param[in] port   lan port id
@param[in] enable  enable/disable port qos
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_qos(u8 port, u8 enable);

/**
@brief Set switch port qos configuration
@param[in] port   lan port id
@param[in] qos_param  qos configuration(mode, wrr weight..)
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_qos_param(u8 port, ECNT_SWITCHMGR_QOS_CFG qos_param);

/**
@brief Power up/down switch port phy
@param[in] port   lan port id
@param[in] up  0:power down, other: power up
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_admin(u8 port, u8 up);

/**
@brief Get switch port phy power state
@param[in] port   lan port id
@param[out] p_up  0:power down, 1: power up
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_admin(u8 port, u8 *p_up);

/**
@brief Set switch port rate limit
@param[in] port   lan port id
@param[in] dir   direction
@param[in] rate  limit speed
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_ratelimit(u8 port, u8 dir, u32 rate);

/**
@brief Get switch port rate limit
@param[in] port   lan port id
@param[in] dir   direction
@param[out] p_rate  limit speed
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_ratelimit(u8 port, u8 dir, u32 *p_rate);

/**
@brief Get switch etherwan port rate limit
@param[in] dir   direction
@param[out] p_rate  limit speed
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_etherwan_port_ratelimit(u32 dir, u32 *rate);

/**
@brief Set switch etherwan port rate limit
@param[in] rate  limit speed
@param[in] dir   direction
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_etherwan_port_ratelimit(u32 rate, u8 dir);

#ifdef CONFIG_TP_IMAGE
ECNT_SWITCHMGR_RET switchmgr_lib_set_etherwan_port_rate_limit_enable(u8 enable);
ECNT_SWITCHMGR_RET switchmgr_lib_get_etherwan_port_rate_limit_enable(u8 *p_enable);
#endif /* CONFIG_TP_IMAGE */

/**
@brief Set switch port phy loopback
@param[in] port   lan port id
@param[in] enable enable/disable phy loopback
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_loopback(u8 port, u8 enable);

/**
@brief Get switch port phy loopback
@param[in] port   lan port id
@param[out] p_enable phy loopback state
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_loopback(u8 port, u8 *p_enable);

/**
@brief Set switch port stp state
@param[in] port   lan port id
@param[in] stp_status STP state
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_stp_status(u8 port, ECNT_SWITCHMGR_STP_STATUS stp_status);

/**
@brief Get switch port stp state
@param[in] port   lan port id
@param[out] p_stp_status STP state
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_stp_status(u8 port, ECNT_SWITCHMGR_STP_STATUS *p_stp_status);

/**
@brief Get switch port speed mode
@param[in] port   lan port id
@param[out] p_mode speed mode
@return ::ECNT_SWITCHMGR_RET
@note The speed mode retrieved by this api only means the configuration set by operator.\n
The running speed might be different(eg, config:auto-negotiate/running:negotiated speed).
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_speed_mode(u8 port, ECNT_SWITCHMGR_SPEED_MODE *p_mode);

/**
@brief Set switch port speed mode
@param[in] port   lan port id
@param[out] mode speed mode
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_speed_mode(u8 port, ECNT_SWITCHMGR_SPEED_MODE mode);

/**
@brief Set switch port flow control
@param[in] port   lan port id
@param[in] enable enable/disable port flow control
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_flow_control(u8 port, u8 enable);

/**
@brief Get switch port flow control
@param[in] port   lan port id
@param[out] p_enable port flow control state
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_flow_control(u8 port, u8 *p_enable);

/**
@brief Set switch port mac learning limit
@param[in] port lan port id
@param[in] cnt number of mac table  entries could be learned on the port specified
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_mac_limit(u8 port, u32 cnt);

/**
@brief Get switch port mac learning limit
@param[in] port lan port id
@param[out] p_cnt number of mac table  entries could be learned on the port specified
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mac_limit(u8 port, u32 *p_cnt);

/**
@brief Get switch port mac learning num
@param[in] port lan port id
@param[out] p_cnt number of mac table  entries could be learned on the port specified
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mac_learn_num(u8 port, u32 *p_cnt);


/**
@brief Get switch port hardware mac address
@param[in] port lan port id
@param[out] mac HW mac address
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mac(u8 port, u8 mac[6]);

/**
@brief Set switch port learning config
@param[in] port lan port id
@param[in] enable enable/disable port learning
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_learning(u8 port, u8 enable);

/**
@brief Get switch port learning config
@param[in] port lan port id
@param[in] p_enable port learning config state
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_learning(u8 port, u8 *p_enable);

/**
@brief Set switch port phy max bit rate
@param[in] port lan port id
@param[in] speed maximum bit rate, should be "Auto" or "100" or "10"
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_max_bitrate(int port, char* speed);

/**
@brief Get switch port phy max bit rate
@param[in] port lan port id
@param[out] speed maximum bit rate, should be "Auto" or "100" or "10"
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_max_bitrate(int port, char* speed);

/**
@brief Set switch port phy duplex
@param[in] port lan port id
@param[in] Mode duplex mode, should be "Auto" or "Half" or "Full"
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_duplex(int port, char* Mode);

/**
@brief Get switch port phy duplex
@param[in] port lan port id
@param[out] Mode duplex mode, should be "Auto" or "Half" or "Full"
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_duplex(int port, char* Mode);

/**
@brief Set switch port phy auto negotiate config
@param[in] port lan port id
@param[in] enable enable/disable auto negotiate
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_autoneg_enable(u8 port, u8 enable);

/**
@brief Get switch port phy auto negotiate config
@param[in] port lan port id
@param[out] p_enable enable/disable auto negotiate
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_autoneg_enable(u8 port, u8 *p_enable);

/**
@brief Restart port phy auto negotiate
@param[in] port lan port id
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_autoneg_restart(u8 port);

/**
@brief Set switch port mirror config
@param[in] port lan port id
@param[in] portbased port mirror config
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_mirror(u8 port, ECNT_SWITCHMGR_PORT_MIRROR *portbased);

/**
@brief Set switch port mac transmitting and receiving
@param[in] port lan port id
@param[in] direction 0:tx, 1:rx
@param[in] enable port mirror config
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_mac_txrx(u8 port, u8 direction, u8 enable);

/**
@brief Show switch vlan configuration
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_disp();

/**
@brief Set switch mac table age time
@param[in] time age time
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_age_time(u32 time);

/**
@brief Get switch mac table age time
@param[out] p_time age time
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_age_time( u32 *p_time);

/**
@brief Clear switch mac table
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_clr_mac_table(void);

/**
@brief Clear switch mac table of a specific port
@param[in] port lan port id
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_clr_port_mac_table(u8 port);

/**
@brief Get switch mac table of a specific port
@param[in] port lan port id
@param[out] p_br_table mac table
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_bridge_table(u8 port, ECNT_SWITCHMGR_BRIDGE_TABLE *p_br_table);

/**
@brief Get 16 mac table entries of specific port
@param[in]  port      lan port id
@param[out]  mac    mac table
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_all_mac(u8 port, ECNT_SWITCHMGR_PORT_ALL_MAC *mac);

/**
@brief Get port by specific mac address
@param[in]  inptr_PortMac      mac info
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_by_mac(ECNT_SWITCHMGR_MAC_ENTRY * inptr_PortMac);

/**
@brief Set switch mac table aging
@param[in] aging_en enable/disable aging
@param[in] time age time
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_aging(u8 aging_en, u32 time);

/**
@brief Show switch mac table
@param[in] ac_mat 0: MAC, 1: DIP, 2: SIP
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_mactbldisp(int ac_mat);

/**
@brief Clear switch mac table
@param[in] ac_mat 0: MAC, 1: DIP, 2: SIP
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_mactblclr(int ac_mat);

/**
@brief Set switch QoS queue mark 
@param[in] enable enable/disable QoS queue mark
@param[in] shift queue mark shift
@param[in] width queue mark width
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_queue_mark(u8 enable, u8 shift, u8 width);

/**
@brief Set switch port queue weight
@param[in] port lan port id
@param[in] queueId queue id
@param[in] weight QoS weight
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_queue_weight(u8 port, u8 queueId, u8 weight);

/**
@brief Get switch port queue weight
@param[in] port lan port id
@param[in] queueId queue id
@param[out] p_weight QoS weight
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_queue_weight(u8 port, u8 queueId, u8 *p_weight);

/**
@brief Clear switch all port statistics
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_clr_all_port_statistics(void);

/**
@brief Get switch port statistics
@param[in] port lan port id
@param[in] p_stats port statistics
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_statistics(u8 port, ECNT_SWITCHMGR_PORT_STATISTICS *p_stats);

/**
@brief Get switch port mib_count
@param[in] port lan port id
@param[in] type mibcnt type
@param[out] p_cnt mibcount
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mib_count(u8 port, u32 type, u32 *p_cnt);

/**
@brief Get switch port txpkts_count
@param[in] port lan port id
@param[out] p_cnt mibtx pktcount
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_txpkts_count(u8 port, u32 *p_cnt);

/**
@brief Get switch port rxpkts_count
@param[in] port lan port id
@param[out] p_cnt mibrx pktcount
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_port_rxpkts_count(u8 port, u32 *p_cnt);

/**
@brief Set switch storm control port
@param[in] port lan port id
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_port(u16 port);

/**
@brief Set switch port storm control unit
@param[in] unit storm control unit:\n
0: 64 packets per second\n
1: 256 packets per second\n
2: 1k packets per second\n
3: 4k packets per second
@note The switch port to set is specified by ::switchmgr_lib_set_storm_ctrl_port.
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_unit(u8 unit);

/**
@brief Set switch port storm control type
@param[in] type storm control type\n
1: unknown mac unicast\n
2: multicast\n
4: broadcast
@note The switch port to set is specified by ::switchmgr_lib_set_storm_ctrl_port.
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_type(u8 type);

/**
@brief Set switch port storm control mode
@param[in] mode storm control mode
@note The switch port to set is specified by ::switchmgr_lib_set_storm_ctrl_port.
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_mode(u8 mode);

/**
@brief Set switch port storm control period
@param[in] period storm control period
@note The switch port to set is specified by ::switchmgr_lib_set_storm_ctrl_port.
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_period(u8 period);

/**
@brief Set internal switch register value
@param[in] offset register offset
@param[in] value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_gsww(u32 offset, u32 value);

/**
@brief Get internal switch register value
@param[in] offset register offset
@param[out] p_value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_gswr(u32 offset, u32 *p_value);

/**
@brief Set internal/external switch register value
@param[in] ext_switch indicate internal/external, 0: internal, other: external
@param[in] reg register offset
@param[in] value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_gsww2(u32 ext_switch, u32 reg, u32 value);

/**
@brief Get internal/external switch register value
@param[in] ext_switch indicate internal/external, 0: internal, other: external
@param[in] reg register offset
@param[out] p_value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_gswr2(u32 ext_switch, u32 reg, u32 *p_value);

/**
@brief Set internal switch phy register value
@param[in] phyaddr phy address
@param[in] reg register address
@param[in] value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_mii(int phyaddr, int reg, u32 value);

/**
@brief Get internal switch phy register value
@param[in] phyaddr phy address
@param[in] reg register address
@param[out] p_value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_mii(int phyaddr, int reg, u32 *p_value);

/**
@brief Set internal/external switch phy register value
@param[in] ext_switch indicate internal/external, 0: internal, other: external
@param[in] phyAddr phy address
@param[in] reg register address
@param[in] value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_mii2(int ext_switch, int phyAddr, int reg, u32 value);

/**
@brief Get internal/external switch phy register value
@param[in] ext_switch indicate internal/external, 0: internal, other: external
@param[in] phyAddr phy address
@param[in] reg register address
@param[out] p_value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_mii2(int ext_switch, int phyAddr, int reg, u32 *p_value);

/**
@brief Get Ephy register value by MDIO CL45 read
@param[in] phyAddr phy address
@param[in] devAddr dev address
@param[in] reg register address
@param[out] p_value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_get_emii(u32 phyaddr, u32 devaddr, u32 reg, u32 *p_value);

/**
@brief Set Ephy register value by MDIO CL45 write
@param[in] phyAddr phy address
@param[in] devAddr dev address
@param[in] reg register address
@param[in] value register value
@return ::ECNT_SWITCHMGR_RET
@par Supported Chip:
@code
EN7512, EN7521, EN7580
@endcode
*/
ECNT_SWITCHMGR_RET switchmgr_lib_set_emii(u32 phyaddr, u32 devaddr, u32 reg, u32 value);

ECNT_SWITCHMGR_RET switchmgr_lib_get_wol(ECNT_SWITCHMGR_WOL_STATUS *wol_p);

ECNT_SWITCHMGR_RET switchmgr_lib_set_wol(ECNT_SWITCHMGR_WOL_CONFIG *wol_p);

/*socket related function*/
int init_swconfig(void);
struct swconfig* getSwcfg(void);

// ============== redefine  =================
#define ecnt_switchmgr_lib_set_portmap switchmgr_lib_set_portmap
#define ecnt_switchmgr_lib_set_vlan_active switchmgr_lib_set_vlan_active
#define ecnt_switchmgr_lib_set_flow_control switchmgr_lib_set_flow_control
#define ecnt_switchmgr_lib_get_flow_control switchmgr_lib_get_flow_control
#define ecnt_switchmgr_lib_set_max_framesize switchmgr_lib_set_max_framesize
#define ecnt_switchmgr_lib_get_max_framesize switchmgr_lib_get_max_framesize 
#define ecnt_switchmgr_lib_set_port_isolate switchmgr_lib_set_port_isolate
#define ecnt_switchmgr_lib_set_port_matrix switchmgr_lib_set_port_matrix
#define ecnt_switchmgr_lib_set_lan2lan switchmgr_lib_set_lan2lan
#define ecnt_switchmgr_lib_get_lan2lan switchmgr_lib_get_lan2lan
#define ecnt_switchmgr_lib_clear_lan2lan switchmgr_lib_clear_lan2lan
#define ecnt_switchmgr_lib_set_dstq switchmgr_lib_set_dstq
#define ecnt_switchmgr_lib_set_mirror_enable switchmgr_lib_set_mirror_enable
#define ecnt_switchmgr_lib_set_port_qos switchmgr_lib_set_port_qos
#define ecnt_switchmgr_lib_set_port_qos_param switchmgr_lib_set_port_qos_param
#define ecnt_switchmgr_lib_set_port_admin switchmgr_lib_set_port_admin
#define ecnt_switchmgr_lib_get_port_admin switchmgr_lib_get_port_admin
#define ecnt_switchmgr_lib_set_port_ratelimit switchmgr_lib_set_port_ratelimit
#define ecnt_switchmgr_lib_get_port_ratelimit switchmgr_lib_get_port_ratelimit
#define ecnt_switchmgr_lib_set_port_loopback switchmgr_lib_set_port_loopback
#define ecnt_switchmgr_lib_get_port_loopback switchmgr_lib_get_port_loopback
#define ecnt_switchmgr_lib_set_port_stp_status switchmgr_lib_set_port_stp_status
#define ecnt_switchmgr_lib_get_port_stp_status switchmgr_lib_get_port_stp_status
#define ecnt_switchmgr_lib_get_port_speed_mode switchmgr_lib_get_port_speed_mode
#define ecnt_switchmgr_lib_set_port_flow_control switchmgr_lib_set_port_flow_control
#define ecnt_switchmgr_lib_get_port_flow_control switchmgr_lib_get_port_flow_control
#define ecnt_switchmgr_lib_set_port_mac_limit switchmgr_lib_set_port_mac_limit
#define ecnt_switchmgr_lib_get_port_mac_limit switchmgr_lib_get_port_mac_limit
#define ecnt_switchmgr_lib_get_port_mac switchmgr_lib_get_port_mac
#define ecnt_switchmgr_lib_set_port_learning switchmgr_lib_set_port_learning
#define ecnt_switchmgr_lib_get_port_learning switchmgr_lib_get_port_learning
#define ecnt_switchmgr_lib_set_port_max_bitrate switchmgr_lib_set_port_max_bitrate
#define ecnt_switchmgr_lib_get_port_max_bitrate switchmgr_lib_get_port_max_bitrate
#define ecnt_switchmgr_lib_set_port_duplex switchmgr_lib_set_port_duplex
#define ecnt_switchmgr_lib_get_port_duplex switchmgr_lib_get_port_duplex
#define ecnt_switchmgr_lib_set_port_autoneg_enable switchmgr_lib_set_port_autoneg_enable
#define ecnt_switchmgr_lib_get_port_autoneg_enable switchmgr_lib_get_port_autoneg_enable
#define ecnt_switchmgr_lib_set_port_autoneg_restart switchmgr_lib_set_port_autoneg_restart
#define ecnt_switchmgr_lib_set_port_mirror switchmgr_lib_set_port_mirror
#define ecnt_switchmgr_lib_set_port_mac_txrx switchmgr_lib_set_port_mac_txrx
#define ecnt_switchmgr_lib_set_vlan_disp switchmgr_lib_set_vlan_disp
#define ecnt_switchmgr_lib_set_age_time switchmgr_lib_set_age_time
#define ecnt_switchmgr_lib_get_age_time switchmgr_lib_get_age_time
#define ecnt_switchmgr_lib_clr_mac_table switchmgr_lib_clr_mac_table
#define ecnt_switchmgr_lib_clr_port_mac_table switchmgr_lib_clr_port_mac_table
#define ecnt_switchmgr_lib_get_port_bridge_table switchmgr_lib_get_port_bridge_table
#define ecnt_switchmgr_lib_get_port_all_mac switchmgr_lib_get_port_all_mac
#define ecnt_switchmgr_lib_set_arl_aging switchmgr_lib_set_arl_aging
#define ecnt_switchmgr_lib_set_arl_mactbldisp switchmgr_lib_set_arl_mactbldisp
#define ecnt_switchmgr_lib_set_arl_mactblclr switchmgr_lib_set_arl_mactblclr
#define ecnt_switchmgr_lib_set_queue_mark switchmgr_lib_set_queue_mark
#define ecnt_switchmgr_lib_set_port_queue_weight switchmgr_lib_set_port_queue_weight
#define ecnt_switchmgr_lib_get_port_queue_weight switchmgr_lib_get_port_queue_weight
#define ecnt_switchmgr_lib_clr_all_port_statistics switchmgr_lib_clr_all_port_statistics
#define ecnt_switchmgr_lib_get_port_statistics switchmgr_lib_get_port_statistics
#define ecnt_switchmgr_lib_set_storm_ctrl_port switchmgr_lib_set_storm_ctrl_port
#define ecnt_switchmgr_lib_set_storm_ctrl_unit switchmgr_lib_set_storm_ctrl_unit
#define ecnt_switchmgr_lib_set_storm_ctrl_type switchmgr_lib_set_storm_ctrl_type
#define ecnt_switchmgr_lib_set_storm_ctrl_mode switchmgr_lib_set_storm_ctrl_mode
#define ecnt_switchmgr_lib_set_storm_ctrl_period switchmgr_lib_set_storm_ctrl_period
#define ecnt_switchmgr_lib_set_gsww switchmgr_lib_set_gsww
#define ecnt_switchmgr_lib_get_gswr switchmgr_lib_get_gswr
#define ecnt_switchmgr_lib_set_gsww2 switchmgr_lib_set_gsww2
#define ecnt_switchmgr_lib_get_gswr2 switchmgr_lib_get_gswr2
#define ecnt_switchmgr_lib_set_mii switchmgr_lib_set_mii
#define ecnt_switchmgr_lib_get_mii switchmgr_lib_get_mii
#define ecnt_switchmgr_lib_set_mii2 switchmgr_lib_set_mii2
#define ecnt_switchmgr_lib_get_mii2 switchmgr_lib_get_mii2

//added new
#define ecnt_switchmgr_lib_get_chip_type switchmgr_lib_get_chip_type
#define ecnt_switchmgr_lib_get_port_link_state switchmgr_lib_get_port_link_state
#define ecnt_switchmgr_lib_set_port_link_state switchmgr_lib_set_port_link_state
#define ecnt_switchmgr_lib_set_eth_general_api switchmgr_lib_set_eth_general_api
#define ecnt_switchmgr_lib_set_acl_upstream_notunicast_ratelimit switchmgr_lib_set_acl_upstream_notunicast_ratelimit
#define ecnt_switchmgr_lib_set_acl_upstream_vlan_ratelimit switchmgr_lib_set_acl_upstream_vlan_ratelimit
#define ecnt_switchmgr_lib_set_port_speed_mode switchmgr_lib_set_port_speed_mode
#define ecnt_switchmgr_lib_get_port_configuration_status switchmgr_lib_get_port_configuration_status
#define ecnt_switchmgr_lib_set_mac_match_acl switchmgr_lib_set_mac_match_acl
#define ecnt_switchmgr_lib_set_emii switchmgr_lib_set_emii
#define ecnt_switchmgr_lib_get_emii switchmgr_lib_get_emii
#define ecnt_switchmgr_lib_get_port_mib_count switchmgr_lib_get_port_mib_count
#define ecnt_switchmgr_lib_get_port_txpkts_count switchmgr_lib_get_port_txpkts_count
#define ecnt_switchmgr_lib_get_port_rxpkts_count switchmgr_lib_get_port_rxpkts_count

#endif /* _API_LIB_SWITCHMGR_H */

