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
 * Feature : The file includes QoS module high-layer API defination
 *
 */

#ifndef __DAL_RTL8373_MACSEC_H__
#define __DAL_RTL8373_MACSEC_H__

#include "macsec.h"
#include "rtk_types.h"


#define BIT_0_IN32   0x00000001U
#define BIT_1_IN32   0x00000002U
#define BIT_2_IN32   0x00000004U
#define BIT_3_IN32   0x00000008U
#define BIT_4_IN32   0x00000010U
#define BIT_5_IN32   0x00000020U
#define BIT_6_IN32   0x00000040U
#define BIT_7_IN32   0x00000080U
#define BIT_8_IN32   0x00000100U
#define BIT_9_IN32   0x00000200U
#define BIT_10_IN32  0x00000400U
#define BIT_11_IN32  0x00000800U
#define BIT_12_IN32  0x00001000U
#define BIT_13_IN32  0x00002000U
#define BIT_14_IN32  0x00004000U
#define BIT_15_IN32  0x00008000U
#define BIT_16_IN32  0x00010000U
#define BIT_17_IN32  0x00020000U
#define BIT_18_IN32  0x00040000U
#define BIT_19_IN32  0x00080000U
#define BIT_20_IN32  0x00100000U
#define BIT_21_IN32  0x00200000U
#define BIT_22_IN32  0x00400000U
#define BIT_23_IN32  0x00800000U
#define BIT_24_IN32  0x01000000U
#define BIT_25_IN32  0x02000000U
#define BIT_26_IN32  0x04000000U
#define BIT_27_IN32  0x08000000U
#define BIT_28_IN32  0x10000000U
#define BIT_29_IN32  0x20000000U
#define BIT_30_IN32  0x40000000U
#define BIT_31_IN32  0x80000000U

#define MASK_1_BITS     (BIT_1_IN32 - 1)
#define MASK_2_BITS     (BIT_2_IN32 - 1)
#define MASK_3_BITS     (BIT_3_IN32 - 1)
#define MASK_4_BITS     (BIT_4_IN32 - 1)
#define MASK_5_BITS     (BIT_5_IN32 - 1)
#define MASK_6_BITS     (BIT_6_IN32 - 1)
#define MASK_7_BITS     (BIT_7_IN32 - 1)
#define MASK_8_BITS     (BIT_8_IN32 - 1)
#define MASK_9_BITS     (BIT_9_IN32 - 1)
#define MASK_10_BITS    (BIT_10_IN32 - 1)
#define MASK_11_BITS    (BIT_11_IN32 - 1)
#define MASK_12_BITS    (BIT_12_IN32 - 1)
#define MASK_13_BITS    (BIT_13_IN32 - 1)
#define MASK_14_BITS    (BIT_14_IN32 - 1)
#define MASK_15_BITS    (BIT_15_IN32 - 1)
#define MASK_16_BITS    (BIT_16_IN32 - 1)
#define MASK_17_BITS    (BIT_17_IN32 - 1)
#define MASK_18_BITS    (BIT_18_IN32 - 1)
#define MASK_19_BITS    (BIT_19_IN32 - 1)
#define MASK_20_BITS    (BIT_20_IN32 - 1)
#define MASK_21_BITS    (BIT_21_IN32 - 1)
#define MASK_22_BITS    (BIT_22_IN32 - 1)
#define MASK_23_BITS    (BIT_23_IN32 - 1)
#define MASK_24_BITS    (BIT_24_IN32 - 1)
#define MASK_25_BITS    (BIT_25_IN32 - 1)
#define MASK_26_BITS    (BIT_26_IN32 - 1)
#define MASK_27_BITS    (BIT_27_IN32 - 1)
#define MASK_28_BITS    (BIT_28_IN32 - 1)
#define MASK_29_BITS    (BIT_29_IN32 - 1)
#define MASK_30_BITS    (BIT_30_IN32 - 1)
#define MASK_31_BITS    (BIT_31_IN32 - 1)


#define RTK_MACSEC_PORT_COMMON          0
#define RTK_MACSEC_PORT_RESERVED        1
#define RTK_MACSEC_PORT_CONTROLLED      2
#define RTK_MACSEC_PORT_UNCONTROLLED    3




// Mask last byte received of MAC source address
#define MACSEC_SA_MATCH_MASK_MAC_SA_0         BIT_0_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_1         BIT_1_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_2         BIT_2_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_3         BIT_3_IN32
#define MACSEC_SA_MATCH_MASK_MAC_SA_4         BIT_4_IN32
// Mask first byte received of MAC source address
#define MACSEC_SA_MATCH_MASK_MAC_SA_5         BIT_5_IN32

#define MACSEC_SA_MATCH_MASK_MAC_SA_FULL      (BIT_0_IN32 | BIT_1_IN32 | BIT_2_IN32 | BIT_3_IN32 | BIT_4_IN32 | BIT_5_IN32)

// Mask last byte received of MAC destination address
#define MACSEC_SA_MATCH_MASK_MAC_DA_0         BIT_6_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_1         BIT_7_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_2         BIT_8_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_3         BIT_9_IN32
#define MACSEC_SA_MATCH_MASK_MAC_DA_4         BIT_10_IN32
// Mask first byte received of MAC destination address
#define MACSEC_SA_MATCH_MASK_MAC_DA_5         BIT_11_IN32

#define MACSEC_SA_MATCH_MASK_MAC_DA_FULL      (BIT_6_IN32 | BIT_7_IN32 | BIT_8_IN32 | BIT_9_IN32 | BIT_10_IN32 | BIT_11_IN32)

#define MACSEC_SA_MATCH_MASK_MAC_ETYPE        BIT_12_IN32
#define MACSEC_SA_MATCH_MASK_VLAN_VALID       BIT_13_IN32
#define MACSEC_SA_MATCH_MASK_QINQ_FOUND       BIT_14_IN32
#define MACSEC_SA_MATCH_MASK_STAG_VALID       BIT_15_IN32
#define MACSEC_SA_MATCH_MASK_QTAG_VALID       BIT_16_IN32
#define MACSEC_SA_MATCH_MASK_VLAN_UP          BIT_17_IN32
#define MACSEC_SA_MATCH_MASK_VLAN_ID          BIT_18_IN32
#define MACSEC_SA_MATCH_MASK_SRC_PORT         BIT_19_IN32
#define MACSEC_SA_MATCH_MASK_CTRL_PKT         BIT_20_IN32


// For ingress only
#define MACSEC_SA_MATCH_MASK_MACSEC_SCI       BIT_23_IN32
#define MACSEC_SA_MATCH_MASK_MACSEC_TCI_AN_SC (BIT_24_IN32 | BIT_25_IN32 | BIT_29_IN32)

#define MACSEC_SAB_CW0_MACSEC_EG32 0x9241e066
#define MACSEC_SAB_CW0_MACSEC_IG32 0xd241e06f
#define MACSEC_SAB_CW0_MACSEC_EG64 0xa241e066
#define MACSEC_SAB_CW0_MACSEC_IG64 0xe241a0ef
#define MACSEC_SAB_CW0_AES128      0x000a0000
#define MACSEC_SAB_CW0_AES256      0x000e0000



#define RTK_MACSEC_PORT_COMMON          0
#define RTK_MACSEC_PORT_RESERVED        1
#define RTK_MACSEC_PORT_CONTROLLED      2
#define RTK_MACSEC_PORT_UNCONTROLLED    3

#define MACSEC_REG_OFFS               4

#define RTL8373_MACSEC_PORT_NUM         8

#define RTL8373_MACSEC_HW_SA_ID(sc_id, an)       (sc_id * 4 + an)
#define RTL8373_MACSEC_HW_FLOW_ID(sc_id)         (sc_id * 4)
#define RTL8373_MACSEC_HW_SA_TO_AN(sa_id)        (sa_id % 4)

#define RTL8373_MACSEC_MAX_SA_EN_SIZE     24 //32-bit words
#define RTL8373_MACSEC_MAX_SA_IN_SIZE     20
#define RTL8373_MACSEC_MAX_SA_SIZE    RTL8373_MACSEC_MAX_SA_EN_SIZE


#define MACSEC_XFORM_REC_BASE         (0x0000)
#define MACSEC_XFORM_REC_SIZE(dir)   ((MACSEC_DIRECTION_EGRESS == dir) ? RTL8373_MACSEC_MAX_SA_EN_SIZE : RTL8373_MACSEC_MAX_SA_IN_SIZE )

#define MACSEC_REG_XFORM_REC(n, dir)        (MACSEC_XFORM_REC_BASE + MACSEC_XFORM_REC_SIZE(dir) * \
                                             4 * (n % 128))
#define MACSEC_REG_XFORM_REC_OFFS(n, dir, off) (MACSEC_REG_XFORM_REC(n, dir) + off * 4)



#define RTL8373_MACSEC_SA_FLAG_XPN    0x00000001U    //Extended Packet Numbering

#define MACSEC_SA_IS_USED(port, dir, sa_id)     BITMAP_IS_SET(port_macsec_info->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_IS_USED(port, dir, sc_id)     BITMAP_IS_SET(port_macsec_info->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_IS_CLEAR(port, dir, sa_id)    BITMAP_IS_CLEAR(port_macsec_info->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_IS_CLEAR(port, dir, sc_id)    BITMAP_IS_CLEAR(port_macsec_info->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_SET_USED(port, dir, sa_id)    BITMAP_SET(port_macsec_info->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_SET_USED(port, dir, sc_id)    BITMAP_SET(port_macsec_info->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_UNSET_USED(port, dir, sa_id)  BITMAP_CLEAR(port_macsec_info->port[port]->sa_used[dir], sa_id)
#define MACSEC_SC_UNSET_USED(port, dir, sc_id)  BITMAP_CLEAR(port_macsec_info->port[port]->sc_used[dir], sc_id)

#define MACSEC_SA_MAX(port)  port_macsec_info->port[port]->max_sa_num
#define MACSEC_SC_MAX(port)  (port_macsec_info->port[port]->max_sa_num/4)
#define MACSEC_SC_CS(port, dir, sc_id)   port_macsec_info->port[port]->cipher_suite[dir][sc_id]
#define MACSEC_SC_MATCH(port, dir, sc_id)   port_macsec_info->port[port]->flow_match[dir][sc_id]

#define MACSEC_SA_SSCI(port, sa_id) port_macsec_info->port[port]->sa_info[sa_id].ssci




typedef struct {
    rtk_uint8 key_offs;
    rtk_uint8 hkey_offs;
    rtk_uint8 seq_offs;
    rtk_uint8 mask_offs;
    rtk_uint8 ctx_salt_offs;
    rtk_uint8 iv_offs;
    rtk_uint8 upd_ctrl_offs;
} rtl8373_macsec_sa_offset_t;

typedef struct rtl8373_macsec_sa_info_s
{
    rtk_uint8   ssci[4];
} rtl8373_macsec_sa_info_t;


typedef struct rtl8373_macsec_port_info_s
{
    rtk_uint16               sa_gen_seq;
    rtk_uint32               max_sa_num;
    rtk_macsec_cipher_t  cipher_suite[MACSEC_DIRECTION_END][RTK_MAX_MACSEC_SC_PER_PORT];
    rtk_uint32               flow_match[MACSEC_DIRECTION_END][RTK_MAX_MACSEC_SC_PER_PORT];
    rtk_bitmap_t         sc_used[MACSEC_DIRECTION_END][BITMAP_ARRAY_CNT(RTK_MAX_MACSEC_SC_PER_PORT)];
    rtk_bitmap_t         sa_used[MACSEC_DIRECTION_END][BITMAP_ARRAY_CNT(RTK_MAX_MACSEC_SA_PER_PORT)];
    rtl8373_macsec_sa_info_t sa_info[RTK_MAX_MACSEC_SA_PER_PORT];
} rtl8373_macsec_port_info_t;

typedef struct rtl8373_macsec_info_s
{
    rtl8373_macsec_port_info_t *port[RTL8373_MACSEC_PORT_NUM];
} rtl8373_macsec_info_t;




typedef struct
{
    rtk_uint32 context_id;  //keep 0 for create
    rtk_macsec_dir_t direction;
    rtk_uint32 flow_index; //the flow entry index apply to this SA
    rtk_uint32 flags;  // a bitmap of RTK_PHY_MACSEC_SA_FLAG_*

    rtk_uint8 an;      // 2-bit AN inserted in SecTAG (egress).
    rtk_uint8 sci[8];  // 8-byte SCI.([0:5] = MAC address, [6:7] = port index)

    rtk_uint8 key[RTK_MACSEC_MAX_KEY_LEN];  // MACsec Key.
    rtk_uint32 key_bytes; // Size of the MACsec key in bytes (16 for AES128, 32 for AES256).

    rtk_uint8 salt[12];  // 12-byte salt (64-bit sequence numbers).
    rtk_uint8 ssci[4];  // 4-byte SSCI value (64-bit sequence numbers).

    rtk_uint32 seq; // sequence number.
    rtk_uint32 seq_h; // High part of sequence number (64-bit sequence numbers)
    rtk_uint32 replay_window; // Size of the replay window, 0 for strict ordering (ingress).

    /* update ctrl */
    rtk_uint32 next_sa_index; // SA index of the next chained SA (egress).
    rtk_uint8 sa_expired_irq; // 1 if SA expired IRQ is to be generated.
    rtk_uint8 next_sa_valid; // SA Index field is a valid SA.
    rtk_uint8 update_en;  // Set to true if the SA must be updated.
} rtl8373_macsec_sa_params_t;


/* Function Name:
 *      dal_rtl8373_macsec_enable_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_enable_set(rtk_uint32 port, rtk_uint32 ingress_en, rtk_uint32 egress_en);

/* Function Name:
 *      dal_rtl8373_macsec_enable_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_enable_get(rtk_uint32 port, rtk_uint32 *ingress_en, rtk_uint32 *egress_en);

/* Function Name:
 *      dal_rtl8373_macsec_reset
 * Description:
 *      reset macsec ip.
 * Input:
 *      port   - port id
 *      ingress_rst  - ingress reset
 *      egress_rst  -  egress reset
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will reset macsec.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_reset(rtk_uint32 port, rtk_uint32 ingress_rst, rtk_uint32 egress_rst);

/* Function Name:
 *      dal_rtl8373_macsec_egress_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_egress_set(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 value);

/* Function Name:
 *      dal_rtl8373_macsec_egress_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_egress_get(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 *value);

/* Function Name:
 *      dal_rtl8373_macsec_ingress_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_ingress_set(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 value);

/* Function Name:
 *      dal_rtl8373_macsec_ingress_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_ingress_get(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 *value);

/* Function Name:
 *      dal_rtl8373_macsec_rxgating_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_rxgating_set(rtk_uint32 port);

/* Function Name:
 *      dal_rtl8373_macsec_rxgating_cancel
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
extern rtk_api_ret_t dal_rtl8373_macsec_rxgating_cancel(rtk_uint32 port);

/* Function Name:
 *      dal_rtl8373_macsec_txgating_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_txgating_set(rtk_uint32 port);

/* Function Name:
 *      dal_rtl8373_macsec_txgating_cancel
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
extern rtk_api_ret_t dal_rtl8373_macsec_txgating_cancel(rtk_uint32 port);

/* Function Name:
 *      dal_rtl8373_macsec_rxIPbypass_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_rxIPbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      dal_rtl8373_macsec_rxIPbypass_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_rxIPbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      dal_rtl8373_macsec_txIPbypass_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_txIPbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      dal_rtl8373_macsec_txIPbypass_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_txIPbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      dal_rtl8373_macsec_rxIPbypass_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_rxbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      dal_rtl8373_macsec_rxbypass_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_rxbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      dal_rtl8373_macsec_txbypass_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_txbypass_set(rtk_uint32 port, rtk_uint32 enable);

/* Function Name:
 *      dal_rtl8373_macsec_txbypass_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_txbypass_get(rtk_uint32 port, rtk_uint32 *enable);

/* Function Name:
 *      dal_rtl8373_wrapper_int_control_set
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
extern rtk_api_ret_t dal_rtl8373_wrapper_int_control_set(rtk_uint32 port, rtk_macsec_int_type_t type, rtk_enable_t enable);

/* Function Name:
 *      dal_rtl8373_wrapper_int_control_get
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
extern rtk_api_ret_t dal_rtl8373_wrapper_int_control_get(rtk_uint32 port, rtk_macsec_int_type_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8373_wrapper_int_status_set
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
extern rtk_api_ret_t dal_rtl8373_wrapper_int_status_set(rtk_uint32 port, rtk_uint32 statusMask);

/* Function Name:
 *      dal_rtl8373_wrapper_int_status_get
 * Description:
 *      Configure MACsec interrupt status.
 * Input:
 *      port  -  port id
 
 * Output:
 *      pStatusMask  -  interrupt status mask
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
extern rtk_api_ret_t dal_rtl8373_wrapper_int_status_get(rtk_uint32 port, rtk_uint32 *pStatusMask);

/* Function Name:
 *      dal_rtl8373_wrapper_mib_reset
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
extern rtk_api_ret_t dal_rtl8373_wrapper_mib_reset(rtk_uint32 port, rtk_uint32 reset);

/* Function Name:
 *      dal_rtl8373_wrapper_mib_counter
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
extern rtk_api_ret_t dal_rtl8373_wrapper_mib_counter(rtk_uint32 port, RTL8373_WRAPPER_MIBCOUNTER mibIdx, rtk_uint64* pCounter);

/* Function Name:
 *      dal_rtl8373_macsec_ipg_len_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_ipg_len_set(rtk_uint32 port, rtk_uint32 length);

/* Function Name:
 *      dal_rtl8373_macsec_ipg_len_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_ipg_len_get(rtk_uint32 port, rtk_uint32 *plength);

/* Function Name:
 *      dal_rtl8373_macsec_ipg_mode_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_ipg_mode_set(rtk_uint32 port, rtk_uint32 mode);

/* Function Name:
 *      dal_rtl8373_macsec_ipg_mode_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_ipg_mode_get(rtk_uint32 port, rtk_uint32 *pmode);

/* Function Name:
 *      dal_rtl8373_macsec_eth_set
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
extern rtk_api_ret_t dal_rtl8373_macsec_eth_set(rtk_uint32 port, rtk_uint32 entry, rtk_uint32 ethertype);

/* Function Name:
 *      dal_rtl8373_macsec_eth_get
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
extern rtk_api_ret_t dal_rtl8373_macsec_eth_get(rtk_uint32 port, rtk_uint32 entry, rtk_uint32 *pethertype);

/* Function Name:
 *      dal_rtl8373_macsec_init
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
extern rtk_api_ret_t dal_rtl8373_macsec_init(rtk_uint32 port_mask);



/* Function Name:
 *      dal_rtl8373_macsec_AIC_int_mask_set
 * Description:
 *      Set Macsec IP Advanced Interrupt Control mask bits.
 * Input:
 *      port   -  port id, 0~7
 *      dir     -  ingress / egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_AIC_int_mask_set(rtk_uint32 port, rtk_macsec_dir_t dir, rtk_macsec_aic_int_type_t type, rtk_enable_t enable);


/* Function Name:
 *      dal_rtl8373_macsec_AIC_int_mask_set
 * Description:
 *      Set Macsec IP Advanced Interrupt Control mask bits.
 * Input:
 *      port   -  port id, 0~7
 *      dir     -  ingress / egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_AIC_int_mask_get(rtk_uint32 port, rtk_macsec_dir_t dir, rtk_macsec_aic_int_type_t type, rtk_enable_t* pEnable);
extern rtk_api_ret_t dal_rtl8373_macsec_context_id_gen(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index);
extern rtk_api_ret_t dal_rtl8373_macsec_hw_sa_build(rtk_port_t port, rtk_uint32 sa_index, rtl8373_macsec_sa_params_t *pSa,  rtk_uint32 *pSa_raw);
extern rtk_api_ret_t dal_rtl8373_macsec_hw_sa_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index);
extern rtk_api_ret_t dal_rtl8373_macsec_hw_sa_set(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index, rtl8373_macsec_sa_params_t *pSa);
extern rtk_api_ret_t dal_rtl8373_macsec_hw_sa_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index, rtl8373_macsec_sa_params_t *pSa);

/* Function Name:
 *      dal_rtl8373_macsec_match_rule_set
 * Description:
 *      setup a match-rule entry that specify how to classify a packet
 * Input:
 *      port          - port id
 *      flow_index     - table entry index
 *      data          - pointer to the struct for describe the rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_INPUT              - invalid parameter
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_match_rule_set(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_matchrule_t *pMatch);
extern rtk_api_ret_t dal_rtl8373_macsec_match_rule_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_matchrule_t *pMatch);
extern rtk_api_ret_t dal_rtl8373_macsec_match_rule_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index);


/* Function Name:
 *      dal_rtl8373_macsec_action_rule_set
 * Description:
 *      setup a flow control entry
 * Input:
 *      port          - port id
 *      flow_index    - table entry index
 *      pAct          - pointer to the struct for describe the rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_INPUT              - invalid parameter
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_action_rule_set(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_action_t *pAct);
extern rtk_api_ret_t dal_rtl8373_macsec_action_rule_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_action_t *pAct);
extern rtk_api_ret_t dal_rtl8373_macsec_action_rule_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index);
extern rtk_api_ret_t dal_rtl8373_macsec_flow_enable_set(rtk_uint32 port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_enable_t enable);
extern rtk_api_ret_t dal_rtl8373_macsec_flow_enable_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8373_macsec_port_cfg_set
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
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_port_cfg_set(rtk_port_t port, rtk_macsec_port_cfg_t *pPortcfg);

/* Function Name:
 *      dal_rtl8373_macsec_port_cfg_get
 * Description:
 *      Get per-port configurations for MACsec
 * Input:
 *      port     - port id
 * Output:
 *      pPortcfg - pointer to macsec port configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_port_cfg_get(rtk_port_t port,rtk_macsec_port_cfg_t *pPortcfg);


/* Function Name:
 *      dal_rtl8373_macsec_sc_create
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
 *      RT_ERR_EXCEEDS_CAPACITY
 * Note:
 *      None
 */

extern rtk_api_ret_t dal_rtl8373_macsec_sc_create(rtk_port_t port, rtk_macsec_dir_t dir, rtk_macsec_sc_t *pSc, rtk_uint32 *pSc_id);


/* Function Name:
 *      dal_rtl8373_macsec_sc_get
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
 *      RT_ERR_ENTRY_NOTFOUND
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_sc_get(rtk_port_t port, rtk_macsec_dir_t dir,rtk_uint32 sc_id, rtk_macsec_sc_t *pSc);


/* Function Name:
 *      dal_rtl8373_macsec_sc_del
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
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_sc_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id);

/* Function Name:
 *      dal_rtl8373_macsec_sc_status_get
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
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_sc_status_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_sc_status_t *pSc_status);


/* Function Name:
 *      dal_rtl8373_macsec_sa_activate
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
 * Note:
 *      For egress, this function will change running SA.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_sa_activate(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an);

/* Function Name:
 *      dal_rtl8373_macsec_rxsa_disable
 * Description:
 *      Disable a ingress MACsec Secure Association (inUse = 0)
 * Input:
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_rxsa_disable(rtk_port_t port, rtk_uint32 rxsc_id, rtk_macsec_an_t an);

/* Function Name:
 *      dal_rtl8373_macsec_txsa_disable
 * Description:
 *      Disable the running egress MACsec Secure Association (inUse = 0)
 * Input:
 *      port     - port id
 *      txsc_id  - egress SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_txsa_disable(rtk_port_t port, rtk_uint32 txsc_id);


/* Function Name:
 *      dal_rtl8373_macsec_sa_create
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
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_sa_create(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);


/* Function Name:
 *      dal_rtl8373_macsec_sa_get
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
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_sa_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa);


/* Function Name:
 *      dal_rtl8373_macsec_sa_del
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
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_sa_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an);


/* Function Name:
 *      dal_rtl8373_macsec_stat_clear
 * Description:
 *      Clear all statistics counter
 * Input:
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_stat_clear(rtk_port_t port);


/* Function Name:
 *      dal_rtl8373_macsec_stat_port_get
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
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_stat_port_get(rtk_port_t port, rtk_macsec_stat_t stat, rtk_uint64 *pCnt);


/* Function Name:
 *      dal_rtl8373_macsec_stat_txsa_get
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
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_stat_txsa_get(rtk_port_t port, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, rtk_uint64 *pCnt);


/* Function Name:
 *      dal_rtl8373_macsec_stat_rxsa_get
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
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_stat_rxsa_get(rtk_port_t port, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, rtk_uint64 *pCnt);


/* Function Name:
 *      dal_rtl8373_macsec_intr_status_get
 * Description:
 *      Get status information for MACsec interrupt
 * Input:
 *      port     - port id
 * Output:
 *      pIntr_status - interrupt status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern rtk_api_ret_t dal_rtl8373_macsec_intr_status_get(rtk_port_t port,rtk_macsec_intr_status_t *pIntr_status);


/* Function Name:
 *      dal_rtl8373_macsec_ra_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      mode    - 0: don't insert ipg for macsec
 *                1: insert ipg according to feedback signal
 *                2: insert ipg according to ethtype & ipg_length
 *                3: always insert ipg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_ra_set(rtk_uint32 portid, rtk_uint32 mode);

/* Function Name:
 *      dal_rtl8373_macsec_ra_get
 * Description:
 *      Get RA function.
 * Input:
 *      portid   -  port id
 *      pMode    - 0: don't insert ipg for macsec
 *                1: insert ipg according to feedback signal
 *                2: insert ipg according to ethtype & ipg_length
 *                3: always insert ipg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_ra_get(rtk_uint32 portid, rtk_uint32* pMode);

/* Function Name:
 *      dal_rtl8373_macsec_ipglen_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      len      - additional ipg length for macsec
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_ipglen_set(rtk_uint32 portid, rtk_uint32 len);


/* Function Name:
 *      dal_rtl8373_macsec_ipglen_get
 * Description:
 *      Get RA function.
 * Input:
 *      portid   -  port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_ipglen_get(rtk_uint32 portid, rtk_uint32* pLen);

/* Function Name:
 *      dal_rtl8373_macsec_ra_ethtype_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      id      - ether type id 0-7
 *      type    - ether type value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_ra_ethtype_set(rtk_uint32 portid, rtk_uint32 id, rtk_uint32 type);


/* Function Name:
 *      dal_rtl8373_macsec_ra_ethtype_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      id      - ether type id 0-7
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
extern rtk_api_ret_t dal_rtl8373_macsec_ra_ethtype_get(rtk_uint32 portid, rtk_uint32 id, rtk_uint32* pType);



#endif


