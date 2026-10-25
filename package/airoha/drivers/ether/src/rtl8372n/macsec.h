/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : RTL8371c switch high-level API
 *
 * Feature : The file includes MACsec module high-layer API defination
 *
 */

#ifndef __RTK_API_MACSEC_H__
#define __RTK_API_MACSEC_H__


#define RTK_MACSEC_MATCH_RULE_MAX   16
#define RTK_MACSEC_ACTION_RULE_MAX   16
#define RTK_MAX_MACSEC_SA_PER_PORT                  16                              /* max number of Secure Association by a port*/
#define RTK_MAX_MACSEC_SC_PER_PORT                  RTK_MAX_MACSEC_SA_PER_PORT/2    /* max number of Secure Channel by a port (4 AN per SC) */

#define RTK_MACSEC_MAX_KEY_LEN        32



typedef enum rtk_macsec_dir_e
{
    MACSEC_DIRECTION_INGRESS = 0,
    MACSEC_DIRECTION_EGRESS,
    MACSEC_DIRECTION_END
}rtk_macsec_dir_t;



typedef enum rtk_macsec_flow_e
{
    RTK_MACSEC_FLOW_BYPASS = 0,
    RTK_MACSEC_FLOW_DROP,
    RTK_MACSEC_FLOW_INGRESS,
    RTK_MACSEC_FLOW_EGRESS,
} rtk_macsec_flow_type_t;

typedef enum rtk_macsec_validate_e
{
    RTK_MACSEC_VALIDATE_STRICT = 0,
    RTK_MACSEC_VALIDATE_CHECK,
    RTK_MACSEC_VALIDATE_DISABLE,
} rtk_macsec_validate_t;





typedef struct rtk_macsec_matchrule_e
{
    // index for flow control rule entry
    rtk_uint32 flow_index;

    // Packet field values to match

    // MAC source address
    rtk_uint8 mac_sa[6];

    // MAC destination address
    rtk_uint8 mac_da[6];

    // EtherType
    rtk_uint16 etherType;

    // SCI, for ingress only
    rtk_uint8 sci[8];

    // Parsed VLAN ID compare value
    rtk_uint16 vlan_id;

    // Parsed VLAN valid flag compare value
    rtk_uint8 fVLANValid;

    // Parsed QinQ found flag compare value
    rtk_uint8 fQinQFound;

    // Parsed STAG valid flag compare value
    rtk_uint8 fSTagValid;

    // Parsed QTAG valid flag compare value
    rtk_uint8 fQTagValid;

    // Parsed VLAN User Priority compare value
    rtk_uint8 vlanUserPriority;

    // Packet is a control packet (as pre-decoded) compare value
    rtk_uint8 fControlPacket;

    // true - allow packets without a MACsec tag to match
    rtk_uint8 fUntagged;

    // true - allow packets with a standard and valid MACsec tag to match
    rtk_uint8 fTagged;

    // true - allow packets with an invalid MACsec tag to match
    rtk_uint8 fBadTag;

    // true - allow packets with a MACsec tag indicating KaY handling
    // to be done to match
    rtk_uint8 fKayTag;

    // Source port compare value
    rtk_uint8 sourcePort;

    // Priority of this entry for determining the actual transform used
    // on a match when multiple entries match, 0 = lowest, 15 = highest.
    // In case of identical priorities, the lowest numbered entry takes
    // precedence.
    rtk_uint8 matchPriority;

    // MACsec TCI/AN byte compare value, bits are individually masked for
    // comparing. The TCI bits are in bits [7:2] while the AN bits reside
    // in bits [1:0]. the macsec_TCI_AN field should only be set to
    // non-zero for an actual MACsec packet.
    rtk_uint8 macsec_TCI_AN;

    // Match mask for the SA flow rules, see MACSEC_SA_MATCH_MASK_*
    rtk_uint32 matchMask;

    // Parsed inner VLAN ID compare value
    rtk_uint16 vlanIdInner;

    // Parsed inner VLAN UP compare value
    rtk_uint8 vlanUpInner;

} rtk_macsec_matchrule_t;




typedef struct rtk_macsec_action_e_s
{
    // 1 - enable frame protection,
    // 0 - bypass frame through device
    rtk_uint8 protect_frame;

    // 1 - SA is in use, packets classified for it can be transformed
    // 0 - SA not in use, packets classified for it can not be transformed
    rtk_uint8 sa_in_use;

    // 1 - inserts explicit SCI in the packet,
    // 0 - use implicit SCI (not transferred)
    rtk_uint8 include_sci;

    // 1 - enable ES bit in the generated SecTAG
    // 0 - disable ES bit in the generated SecTAG
    rtk_uint8 use_es;

    // 1 - enable SCB bit in the generated SecTAG
    // 0 - disable SCB bit in the generated SecTAG
    rtk_uint8 use_scb;

    // Number of VLAN tags to bypass for egress processing.
    // Valid values: 0, 1 and 2.
    // This feature is only available on HW4.1 and possibly later versions.
    rtk_uint8 tag_bypass_size;

    // 1 - Does not update sa_in_use flag
    // 0 - Update sa_in_use flag
    rtk_uint8 sa_index_update_by_hw;

    // The number of bytes (in the range of 0-127) that are authenticated but
    // not encrypted following the SecTAG in the encrypted packet. Values
    // 65-127 are reserved in HW < 4.0 and should not be used there.
    rtk_uint8 confidentiality_offset;

    // 1 - enable confidentiality protection
    // 0 - disable confidentiality protection
    rtk_uint8 conf_protect;

} rtk_macsec_action_e_t;

typedef struct rtk_macsec_action_i_s
{
    // 1 - enable replay protection
    // 0 - disable replay protection
    rtk_uint8 replay_protect;

    // true - SA is in use, packets classified for it can be transformed
    // false - SA not in use, packets classified for it can not be transformed
    rtk_uint8 sa_in_use;

    // MACsec frame validation level
    rtk_macsec_validate_t validate_frames;

    // The number of bytes (in the range of 0-127) that are authenticated but
    // not encrypted following the SecTAG in the encrypted packet.
    rtk_uint8 confidentiality_offset;

} rtk_macsec_action_i_t;

typedef struct rtk_macsec_action_bd_s
{
    // 1 - enable statistics counting for the associated SA
    // 0 - disable statistics counting for the associated SA
    rtk_uint8 sa_in_use;
} rtk_macsec_action_bd_t;


typedef struct rtk_macsec_action_s
{
    rtk_uint32 sa_index;

    rtk_macsec_flow_type_t flow_type;
    union
    {
        rtk_macsec_action_e_t egress;
        rtk_macsec_action_i_t ingress;
        rtk_macsec_action_bd_t bypass_drop;
    } params;

    rtk_uint8 dest_port;

} rtk_macsec_action_t;

typedef enum rtk_macsec_match_tx_e
{
    RTK_MACSEC_MATCH_NON_CTRL = 0, /* match all non-control and untagged packets */
    RTK_MACSEC_MATCH_MAC_DA, /* match all non-control and untagged packets with specific MAC DA */
} rtk_macsec_match_tx_t;

typedef enum rtk_macsec_cipher_e
{
    RTK_MACSEC_CIPHER_GCM_ASE_128 = 0,
    RTK_MACSEC_CIPHER_GCM_ASE_256,
    RTK_MACSEC_CIPHER_GCM_ASE_XPN_128,
    RTK_MACSEC_CIPHER_GCM_ASE_XPN_256,
    RTK_MACSEC_CIPHER_MAX,
} rtk_macsec_cipher_t;



typedef struct rtk_macsec_txsc_s
{
    /* 8-byte SCI([0:5] = MAC address, [6:7] = port index) for this secure channel  */
    rtk_uint8      sci[8];

    /* cipher suite for this SC */
    rtk_macsec_cipher_t    cipher_suite;

    /* packet flow type to match this SC */
    rtk_macsec_match_tx_t  flow_match;
    rtk_mac_t  mac_da; /* the target address for RTK_MACSEC_MATCH_MAC_DA */

    rtk_uint8 protect_frame;  /* 1 = enable frame protection */
    rtk_uint8 include_sci;    /* 1 = include explicit SCI in packet */
    rtk_uint8 use_es;         /* 1 = set ES (End Station) bit in TCI field */
    rtk_uint8 use_scb;        /* 1 = set SCB (Single Copy Broadcast) bit in TCI field */
    rtk_uint8 conf_protect;   /* 1 = enable confidentiality protection, */
}rtk_macsec_txsc_t;

typedef enum rtk_macsec_match_rx_e
{
    RTK_MACSEC_MATCH_SCI = 0,
    RTK_MACSEC_MATCH_MAC_SA,  //for pkt without SCI field/TCI.SC=0,
} rtk_macsec_match_rx_t;

typedef struct rtk_macsec_rxsc_s
{
    /* 8-byte SCI([0:5] = MAC address, [6:7] = port index) for this secure channel  */
    rtk_uint8      sci[8];

    /* cipher suite for this SC */
    rtk_macsec_cipher_t   cipher_suite;

    /* packet flow type to match this SC */
    rtk_macsec_match_rx_t flow_match;
    rtk_mac_t  mac_sa; /* the target address for RTK_MACSEC_MATCH_MAC_SA */

    /* frame validation level */
    rtk_macsec_validate_t validate_frames;

    /* replay protection */
    rtk_uint8  replay_protect;  /* 1 = enable replay protection */
    rtk_uint32 replay_window;   /* the window size for replay protection, range for PN: 0 ~ 2^32 - 1, for XPN: 0 ~ 2^30 */

}rtk_macsec_rxsc_t;

typedef union rtk_macsec_sc_u
{
    rtk_macsec_txsc_t tx;
    rtk_macsec_rxsc_t rx;
}
rtk_macsec_sc_t;

typedef enum rtk_macsec_an_e
{
    RTK_MACSEC_AN0 = 0,
    RTK_MACSEC_AN1,
    RTK_MACSEC_AN2,
    RTK_MACSEC_AN3,
    RTK_MACSEC_AN_MAX,
} rtk_macsec_an_t; 


typedef struct rtk_macsec_txsc_status_s
{
    rtk_uint32 hw_flow_index;
    rtk_uint32 hw_sa_index;
    rtk_uint8  sa_inUse;
    rtk_uint32 hw_flow_data;
    rtk_uint8  hw_sc_flow_status;
    rtk_macsec_an_t running_an;
}
rtk_macsec_txsc_status_t;

typedef struct rtk_macsec_rxsc_status_s
{
    rtk_uint32 hw_flow_base;
    rtk_uint32 hw_sa_index[RTK_MACSEC_AN_MAX];
    rtk_uint8  sa_inUse[RTK_MACSEC_AN_MAX];
    rtk_uint32 hw_flow_data[RTK_MACSEC_AN_MAX];
    rtk_uint8  hw_sc_flow_status[RTK_MACSEC_AN_MAX];
}
rtk_macsec_rxsc_status_t;

typedef union rtk_macsec_sc_status_u
{
    rtk_macsec_txsc_status_t tx;
    rtk_macsec_rxsc_status_t rx;
}
rtk_macsec_sc_status_t;

typedef struct rtk_macsec_sa_s
{
    rtk_uint8 key[RTK_MACSEC_MAX_KEY_LEN];  // MACsec Key.
    rtk_uint32 key_bytes; // Size of the MACsec key in bytes (16 for AES128, 32 for AES256).

    rtk_uint32 pn;      // PN (32-bit) or lower 32-bit of XPN (64-bit)
    rtk_uint32 pn_h;    // higher 32-bit of XPN (64-bit)
    rtk_uint8 salt[12]; // 12-byte salt (for XPN).
    rtk_uint8 ssci[4];  // 4-byte SSCI value (for XPN).
} rtk_macsec_sa_t;


#define RTK_MACSEC_INTR_EGRESS_PN_THRESHOLD          0x00000001
#define RTK_MACSEC_INTR_EGRESS_PN_ROLLOVER           0x00000002

typedef struct rtk_macsec_intr_status_s
{
    /* a bitmap of RTK_MACSEC_INTR_*  to present occured event */
    rtk_uint32 status;

    /* When read 1b, the corresponding MACsec egress SA is about to expire due to
       the packet number crossing the rtk_macsec_port_cfg_t.pn_intr_threshold or xpn_intr_threshold*/
    rtk_uint8  egress_pn_thr_an_bmap[RTK_MAX_MACSEC_SC_PER_PORT]; //bitmap of AN3~0.

    /* When read 1b, the corresponding MACsec egress SA has expired due to
       the packet number reaching the maximum allowed value. */
    rtk_uint8  egress_pn_exp_an_bmap[RTK_MAX_MACSEC_SC_PER_PORT]; //bitmap of AN3~0.
}
rtk_macsec_intr_status_t;

/* enable, the packet flow go through MACsec logic */
#define RTK_MACSEC_PORT_F_ENABLE             0x00000001U
/* drop non-ctrl frame with KaY tag(C=0/E=1), should not enable when KaY is used for key agressment) */
#define RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY   0x00000002U

typedef struct rtk_macsec_port_cfg_s
{
    /* a bitmap of RTK_MACSEC_PORT_F_* */
    rtk_uint32 flags;

    /* Egress PX/XPN threshold value to trigger interrupt.
       With this interrupt, the system has time to start a re-key before the rollover error fires,
       default is 0, interrupt trigger when PN/XPN rollover*/
    rtk_uint32 pn_intr_threshold; //only for cipher suites with 32-bit packet numbering
    rtk_uint64 xpn_intr_threshold; //only for cipher suites with 64-bit packet numbering
    /*
       counter threshold value for trigger interrupt.
    */
    rtk_uint64 frame_cnt_intr_threshold;
    rtk_uint64 octet_cnt_intr_threshold;

    /*
       This value is used for 802.1AE compliant processing when no MACsec secure channel is found.
       If a MACsec secure channel was found, the validate_frames field from the corresponding RXSC is used.
    */
    rtk_macsec_validate_t nm_validate_frames;
}
rtk_macsec_port_cfg_t;



typedef enum rtk_macsec_aic_int_type_e
{
    AIC_INT_TYPE_DROP_CLASS = 0,
    AIC_INT_TYPE_DROP_PP,
    AIC_INT_TYPE_DROP_MTU,
    AIC_INT_TYPE_ENG_IRQ,
    AIC_INT_TYPE_STAT_SA_THR,
    AIC_INT_TYPE_STAT_GLOBAL_THR,
    AIC_INT_TYPE_STAT_VLAN_THR,
    AIC_INT_TYPE_DROP_CC,
    AIC_INT_TYPE_SA_PN_THR,
    AIC_INT_TYPE_SA_EXPIRED,
    AIC_INT_TYPE__END
}rtk_macsec_aic_int_type_t;





typedef enum rtk_macsec_int_type_e
{
	INT_TYPE_TX_IPE_GLB = 0,
	INT_TYPE_TX_IPESECFAIL,
	INT_TYPE_TX_IPELOCK,
	INT_TYPE_TX_IPELOCK_XG,
	INT_TYPE_RX_IPI_GLB,
	INT_TYPE_RX_IPISECFAIL,
	INT_TYPE_RX_IPILOCK,
	INT_TYPE_RX_IPILOCK_XG,
	INT_TYPE_MACSEC_END
}rtk_macsec_int_type_t;

typedef enum RTL8373_WRAPPER_MIBCOUNTER_e
{
	/* TX */
	TXSYS_CRCERR = 0,
	TXSYS_PKTERR,
	TXSYS_OK, //64bit
	TXSYS_GERR = 4,
	TXSYS_GLPIERR,
	TXLINE_CRCERR = 8,
	TXLINE_PKTERR,
	TXLINE_OK, //64bit
	TXLINE_DROP = 12,
	TXLINE_SRTPKT,
	TXLINE_GERR,

	/* RX */
	RXLINE_CRCERR = 16,
	RXLINE_PKTERR,
	RXLINE_OK, //64bit
	RXLINE_SHORTPKT = 20,
	RXLINE_GERR,
	RXLINE_GLPIERR,
	RXSYS_CRCERR = 24,
	RXSYS_PKTERR,
	RXSYS_OK, //64bit
	RXSYS_DROP = 28,
	RXSYS_DECRYPTSRT,
	RXSYS_GERR		
}RTL8373_WRAPPER_MIBCOUNTER;


typedef enum rtk_macsec_stat_e
{
    RTK_MACSEC_STAT_InPktsUntagged = 0,
    RTK_MACSEC_STAT_InPktsNoTag,
    RTK_MACSEC_STAT_InPktsBadTag,
    RTK_MACSEC_STAT_InPktsUnknownSCI,
    RTK_MACSEC_STAT_InPktsNoSCI,
    RTK_MACSEC_STAT_OutPktsUntagged,
    RTK_MACSEC_STAT_MAX,
} rtk_macsec_stat_t;

typedef enum rtk_macsec_txsa_stat_e
{
    RTK_MACSEC_TXSA_STAT_OutPktsTooLong = 0,
    RTK_MACSEC_TXSA_STAT_OutOctetsProtectedEncrypted,
    RTK_MACSEC_TXSA_STAT_OutPktsProtectedEncrypted,
    RTK_MACSEC_TXSA_STAT_MAX,
} rtk_macsec_txsa_stat_t;

typedef enum rtk_macsec_rxsa_stat_e
{
    RTK_MACSEC_RXSA_STAT_InPktsUnusedSA = 0,
    RTK_MACSEC_RXSA_STAT_InPktsNotUsingSA,
    RTK_MACSEC_RXSA_STAT_InPktsUnchecked,
    RTK_MACSEC_RXSA_STAT_InPktsDelayed,
    RTK_MACSEC_RXSA_STAT_InPktsLate,
    RTK_MACSEC_RXSA_STAT_InPktsOK,
    RTK_MACSEC_RXSA_STAT_InPktsInvalid,
    RTK_MACSEC_RXSA_STAT_InPktsNotValid,
    RTK_MACSEC_RXSA_STAT_InOctetsDecryptedValidated,
    RTK_MACSEC_RXSA_STAT_MAX,
} rtk_macsec_rxsa_stat_t;



/* Function Name:
 *      rtk_macsec_enable_set
 * Description:
 *      Configure macsec enable.
 * Input:
 *      port   - port id
 *      ingress_en  - ingress enable
 *      egress_en  -  egress enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec enable.
 */
extern rtk_api_ret_t rtk_macsec_enable_set(rtk_uint32 port, rtk_uint32 ingress_en, rtk_uint32 egress_en);

/* Function Name:
 *      rtk_macsec_enable_get
 * Description:
 *      get macsec enable status.
 * Input:
 *      port   - port id
 * Output:
 *      ingress_en  - ingress enable
 *      egress_en  -  egress enable
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec enable status.
 */
extern rtk_api_ret_t rtk_macsec_enable_get(rtk_uint32 port, rtk_uint32 *ingress_en, rtk_uint32 *egress_en);

/* Function Name:
 *      rtk_macsec_egress_set
 * Description:
 *      Configure macsec egress rule.
 * Input:
 *      port  - port id
 *      addr - macsec ip core register address
 *      value - data for rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec egress rule:SA match rule, flow control register and transform record.
 */
extern rtk_api_ret_t rtk_macsec_egress_set(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 value);

/* Function Name:
 *      rtk_macsec_egress_get
 * Description:
 *      get macsec egress rule.
 * Input:
 *      port  -  port id
 *      addr -  macsec ip core register address
 * Output:
 *      value  - data for rule
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec egress rule:SA match rule, flow control register and transform record.
 */
extern rtk_api_ret_t rtk_macsec_egress_get(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 *value);

/* Function Name:
 *      rtk_macsec_ingress_set
 * Description:
 *      Configure macsec ingress rule.
 * Input:
 *      port  - port id
 *      addr - macsec ip core register address
 *      value - data for rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec ingress rule:SA match rule, flow control register and transform record.
 */
extern rtk_api_ret_t rtk_macsec_ingress_set(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 value);

/* Function Name:
 *      rtk_macsec_ingress_get
 * Description:
 *      get macsec egress rule.
 * Input:
 *      port  -  port id
 *      addr -  macsec ip core register address
 * Output:
 *      value  - data for rule
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec ingress rule:SA match rule, flow control register and transform record.
 */
extern rtk_api_ret_t rtk_macsec_ingress_get(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 *value);

/* Function Name:
 *      rtk_macsec_rxgating_set
 * Description:
 *      Configure macsec rx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec rx gating value,before set the packet flow path called this API.
 */
extern rtk_api_ret_t rtk_macsec_rxgating_set(rtk_uint32 port);

/* Function Name:
 *      rtk_macsec_rxgating_cancel
 * Description:
 *      Configure macsec rx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec rx gating value, called after packet flow path changed.
 */
extern rtk_api_ret_t rtk_macsec_rxgating_cancel(rtk_uint32 port);

/* Function Name:
 *      rtk_macsec_txgating_set
 * Description:
 *      Configure macsec tx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec tx gating value, before set the packet flow path called this API.
 */
extern rtk_api_ret_t rtk_macsec_txgating_set(rtk_uint32 port);

/* Function Name:
 *      rtk_macsec_txgating_cancel
 * Description:
 *      Configure macsec tx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec tx gating value, called after packet flow path changed..
 */
extern rtk_api_ret_t rtk_macsec_txgating_cancel(rtk_uint32 port);

/* Function Name:
 *      rtk_macsec_rxIPbypass_set
 * Description:
 *      Configure macsec bypass MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass MACsec IP function.
 */
extern rtk_api_ret_t rtk_macsec_rxIPbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      rtk_macsec_rxIPbypass_get
 * Description:
 *      get macsec bypass MACsec IP function status.
 * Input:
 *      port  - port id
 * Output:
 *      enable  - enable ip bypass
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec bypass MACsec IP function status.
 */
extern rtk_api_ret_t rtk_macsec_rxIPbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      rtk_macsec_txIPbypass_set
 * Description:
 *      Configure macsec bypass MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass MACsec IP function.
 */
extern rtk_api_ret_t rtk_macsec_txIPbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      rtk_macsec_txIPbypass_get
 * Description:
 *      Configure macsec bypass MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass MACsec IP function.
 */
extern rtk_api_ret_t rtk_macsec_txIPbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      rtk_macsec_rxbypass_set
 * Description:
 *      Configure macsec bypass in MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass in MACsec IP function.
 */
extern rtk_api_ret_t rtk_macsec_rxbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      rtk_macsec_rxbypass_get
 * Description:
 *      get macsec bypass in MACsec IP function status.
 * Input:
 *      port  - port id
 * Output:
 *      enable  - enable ip bypass
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec bypass in MACsec IP function status.
 */
extern rtk_api_ret_t rtk_macsec_rxbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      rtk_macsec_txbypass_set
 * Description:
 *      Configure macsec bypass in MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass in MACsec IP function.
 */
extern rtk_api_ret_t rtk_macsec_txbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      rtk_macsec_txbypass_get
 * Description:
 *      get macsec bypass in MACsec IP function status.
 * Input:
 *      port  - port id
 * Output:
 *      enable  - enable ip bypass
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec bypass in MACsec IP function status.
 */
extern rtk_api_ret_t rtk_macsec_txbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      rtk_wrapper_int_control_set
 * Description:
 *      Configure MACsec interrupt.
 * Input:
 *      port  -  port id
 *      type  -  interrupt type
 *      enable - enable interrupt
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure MACsec interrupt.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB
 *      - INT_TYPE_TX_IPESECFAIL
 *      - INT_TYPE_TX_IPELOCK
 *      - INT_TYPE_TX_IPELOCK_XG
 *      - INT_TYPE_RX_IPI_GLB
 *      - INT_TYPE_RX_IPISECFAIL
 *      - INT_TYPE_RX_IPILOCK
 *      - INT_TYPE_RX_IPILOCK_XG
 */
extern rtk_api_ret_t rtk_wrapper_int_control_set(rtk_uint32 port, rtk_macsec_int_type_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_wrapper_int_control_get
 * Description:
 *      gonfigure MACsec interrupt.
 * Input:
 *      port  -  port id
 *      type  -  interrupt type
 * Output:
 *      pEnable - enable interrupt
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get MACsec interrupt.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB
 *      - INT_TYPE_TX_IPESECFAIL
 *      - INT_TYPE_TX_IPELOCK
 *      - INT_TYPE_TX_IPELOCK_XG
 *      - INT_TYPE_RX_IPI_GLB
 *      - INT_TYPE_RX_IPISECFAIL
 *      - INT_TYPE_RX_IPILOCK
 *      - INT_TYPE_RX_IPILOCK_XG
 */
extern rtk_api_ret_t rtk_wrapper_int_control_get(rtk_uint32 port, rtk_macsec_int_type_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_wrapper_int_status_set
 * Description:
 *      Configure MACsec interrupt status.
 * Input:
 *      port  -  port id
 *      statusMask  -  interrupt status mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will clean MACsec interrupt status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB [bit[0]]
 *      - INT_TYPE_TX_IPESECFAIL [bit[1]]
 *      - INT_TYPE_TX_IPELOCK [bit[2]]
 *      - INT_TYPE_TX_IPELOCK_XG [bit[3]]
 *      - INT_TYPE_RX_IPI_GLB [bit[8]]
 *      - INT_TYPE_RX_IPISECFAIL [bit[9]]
 *      - INT_TYPE_RX_IPILOCK [bit[10]]
 *      - INT_TYPE_RX_IPILOCK_XG [bit[11]]
 */
extern rtk_api_ret_t rtk_wrapper_int_status_set(rtk_uint32 port, rtk_uint32 statusMask);

/* Function Name:
 *      rtk_wrapper_int_status_get
 * Description:
 *      Configure MACsec interrupt status.
 * Input:
 *      port  -  port id
 *      statusMask  -  interrupt status mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will clean MACsec interrupt status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB [bit[0]]
 *      - INT_TYPE_TX_IPESECFAIL [bit[1]]
 *      - INT_TYPE_TX_IPELOCK [bit[2]]
 *      - INT_TYPE_TX_IPELOCK_XG [bit[3]]
 *      - INT_TYPE_RX_IPI_GLB [bit[8]]
 *      - INT_TYPE_RX_IPISECFAIL [bit[9]]
 *      - INT_TYPE_RX_IPILOCK [bit[10]]
 *      - INT_TYPE_RX_IPILOCK_XG [bit[11]]
 */
extern rtk_api_ret_t rtk_wrapper_int_status_get(rtk_uint32 port, rtk_uint32 *pStatusMask);

/* Function Name:
 *      rtk_wrapper_mib_reset
 * Description:
 *      Configure wrapper mib reset.
 * Input:
 *      port  - port id
 *      reset  -  reset value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure wrapper mib reset.
 */
extern rtk_api_ret_t rtk_wrapper_mib_reset(rtk_uint32 port, rtk_uint32 reset);

/* Function Name:
 *      rtk_wrapper_mib_counter
 * Description:
 *      get wrapper mib counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get wrapper mib counters.
 */
extern rtk_api_ret_t rtk_wrapper_mib_counter(rtk_uint32 port, RTL8373_WRAPPER_MIBCOUNTER mibIdx, rtk_uint64* pCounter);

/* Function Name:
 *      rtk_macsec_ipg_len_set
 * Description:
 *      mac mode MACsec ipg length set.
 * Input:
 *      port   -  port number
 *      length - ipg length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will set mac mode MACsec ipg length.
 */
extern rtk_api_ret_t rtk_macsec_ipg_len_set(rtk_uint32 port, rtk_uint32 length);

/* Function Name:
 *      rtk_macsec_ipg_len_get
 * Description:
 *      mac mode MACsec ipg length get.
 * Input:
 *      port   -  port number
 * Output:
 *      plength - ipg length
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get mac mode MACsec ipg length.
 */
extern rtk_api_ret_t rtk_macsec_ipg_len_get(rtk_uint32 port, rtk_uint32 *plength);

/* Function Name:
 *      rtk_macsec_ipg_mode_set
 * Description:
 *      mac mode MACsec ipg mode set.
 * Input:
 *      port   -  port number
 *      mode -  ipg config mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will set mac mode MACsec ipg mode.
 *	  mode[1:0]:
		0: don't insert ipg for macsec
		1: insert ipg for macsec according to macsec feedback signal
		2: insert ipg for masec according to ethertype & cfg_macsec_ipg_length
		3: always insert ipg for macsec, length according to cfg_macsec_ipg_length
 */
extern rtk_api_ret_t rtk_macsec_ipg_mode_set(rtk_uint32 port, rtk_uint32 mode);

/* Function Name:
 *      rtk_macsec_ipg_mode_get
 * Description:
 *      mac mode MACsec ipg mode get.
 * Input:
 *      port   -  port number
 * Output:
 *      pmode - ipg config mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get mac mode MACsec ipg mode.
 *	  mode[1:0]:
		0: don't insert ipg for macsec
		1: insert ipg for macsec according to macsec feedback signal
		2: insert ipg for masec according to ethertype & cfg_macsec_ipg_length
		3: always insert ipg for macsec, length according to cfg_macsec_ipg_length
 */
extern rtk_api_ret_t rtk_macsec_ipg_mode_get(rtk_uint32 port, rtk_uint32 *pmode);

/* Function Name:
 *      rtk_macsec_eth_set
 * Description:
 *      mac mode MACsec eth set.
 * Input:
 *      port   -  port number
 *      entry - entry number(0-7)
 *      ethertype - ether type value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will set mac mode MACsec eth.
 */
extern rtk_api_ret_t rtk_macsec_eth_set(rtk_uint32 port, rtk_uint32 entry, rtk_uint32 ethertype);

/* Function Name:
 *      rtk_macsec_eth_get
 * Description:
 *      mac mode MACsec eth get.
 * Input:
 *      port   -  port number
 *      entry - entry number(0-7)
 * Output:
 *      pethertype - ether type value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get mac mode MACsec eth.
 */
extern rtk_api_ret_t rtk_macsec_eth_get(rtk_uint32 port, rtk_uint32 entry, rtk_uint32 *pethertype);

/* Function Name:
 *      dal_rtl8371c_macsec_init
 * Description:
 *      Initialize MACsec information.
 * Input:
 *      port_mask   -  port mask, bit[4:7]
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t rtk_macsec_init(rtk_uint32 port_mask);


/* Function Name:
 *      rtk_macsec_port_cfg_set
 * Description:
 *      Set per-port configurations for MACsec
 * Input:
 *      port     - port id
 *      pPortcfg - pointer to macsec port configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_port_cfg_set(rtk_port_t port, rtk_macsec_port_cfg_t *pPortcfg);   /* end of rtk_macsec_port_cfg_set */

/* Function Name:
 *      rtk_macsec_port_cfg_get
 * Description:
 *      Get per-port configurations for MACsec
 * Input:
 *      port     - port id
 * Output:
 *      pPortcfg - pointer to macsec port configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_port_cfg_get(rtk_port_t port,rtk_macsec_port_cfg_t *pPortcfg);   /* end of rtk_macsec_port_cfg_get */

/* Function Name:
 *      rtk_macsec_sc_create
 * Description:
 *      Create a MACsec Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      pSc      - pointer to macsec sc configuration structure
 * Output:
 *      pSc_id   - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */

rtk_api_ret_t rtk_macsec_sc_create(rtk_port_t port, rtk_macsec_dir_t dir, rtk_macsec_sc_t *pSc, rtk_uint32 *pSc_id);   /* end of rtk_macsec_sc_create */

/* Function Name:
 *      rtk_macsec_sc_get
 * Description:
 *      Get configuration info for a created Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_index - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_sc_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_sc_t *pSc);   /* end of rtk_macsec_sc_get */

/* Function Name:
 *      rtk_macsec_sc_del
 * Description:
 *      Delete a Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_sc_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id);   /* end of rtk_macsec_sc_del */

/* Function Name:
 *      rtk_macsec_sc_status_get
 * Description:
 *      Get hardware status for a Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_status - pointer to macsec SC status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_sc_status_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_sc_status_t *pSc_status);   /* end of rtk_macsec_sc_status_get */

/* Function Name:
 *      rtk_macsec_sa_create
 * Description:
 *      Create a MACsec Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 *      pSa      - pointer to macsec SA configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_sa_create(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);   /* end of rtk_macsec_sa_create */

/* Function Name:
 *      rtk_macsec_sa_get
 * Description:
 *      Get configuration info for a Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      pSa      - pointer to macsec SA configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_sa_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);   /* end of rtk_macsec_sa_get */

/* Function Name:
 *      rtk_macsec_sa_del
 * Description:
 *      Delete a MACsec Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_sa_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an);   /* end of rtk_macsec_sa_del */

/* Function Name:
 *      rtk_macsec_sa_activate
 * Description:
 *      Activate a MACsec Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - Secure Channel id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      For egress, this function will change running SA.
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_sa_activate(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an);   /* end of rtk_macsec_sa_activate */

/* Function Name:
 *      rtk_macsec_rxsa_disable
 * Description:
 *      Disable a ingress MACsec Secure Association
 * Input:
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_rxsa_disable(rtk_port_t port, rtk_uint32 rxsc_id, rtk_macsec_an_t an);   /* end of rtk_macsec_rxsa_disable */

/* Function Name:
 *      rtk_macsec_txsa_disable
 * Description:
 *      Disable the running egress MACsec Secure Association
 * Input:
 *      port     - port id
 *      txsc_id  - egress SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_txsa_disable(rtk_port_t port, rtk_uint32 txsc_id);   /* end of rtk_macsec_txsa_disable */

/* Function Name:
 *      rtk_macsec_stat_clear
 * Description:
 *      Clear all statistics counter
 * Input:
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      None
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_stat_clear(rtk_port_t port);   /* end of rtk_macsec_stat_clear */

/* Function Name:
 *      rtk_macsec_stat_port_get
 * Description:
 *      get per-port statistics counter
 * Input:
 *      port     - port id
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_stat_port_get(rtk_port_t port, rtk_macsec_stat_t stat, rtk_uint64 *pCnt);   /* end of rtk_macsec_stat_port_get */

/* Function Name:
 *      rtk_macsec_stat_txsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      port     - port id
 *      txsc_id  - egress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_stat_txsa_get(rtk_port_t port, rtk_uint32 txsc_id, rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, rtk_uint64 *pCnt);   /* end of rtk_macsec_stat_txsa_get */

/* Function Name:
 *      rtk_macsec_stat_rxsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 * Changes:
 *      [SDK_NEXT_RELEASE_VERSION]
 *          New added function.
 */
rtk_api_ret_t rtk_macsec_stat_rxsa_get(rtk_port_t port, rtk_uint32 rxsc_id, rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, rtk_uint64 *pCnt);   /* end of rtk_macsec_stat_rxsa_get */

/* Function Name:
 *      rtk_macsec_intr_status_get
 * Description:
 *      Get status information for MACsec interrupt
 * Input:
 *      port     - port id
 * Output:
 *      pIntr_status - interrupt status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
  * Note:
  *      None
  * Changes:
  *      [SDK_NEXT_RELEASE_VERSION]
  *          New added function.
  */
rtk_api_ret_t  rtk_macsec_intr_status_get(rtk_port_t port,rtk_macsec_intr_status_t *pIntr_status);   /* end of rtk_macsec_intr_status_get */




#endif


