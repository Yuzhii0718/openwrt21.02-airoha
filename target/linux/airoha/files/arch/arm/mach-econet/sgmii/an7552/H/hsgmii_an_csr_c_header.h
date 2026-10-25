#ifndef __hsgmii_an_csr_REGS_H__
#define __hsgmii_an_csr_REGS_H__

//#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef REG_BASE_C_MODULE
// ----------------- hsgmii_an_csr Bit Field Definitions -------------------

#define PACKING
typedef unsigned int UINT32;
typedef unsigned int FIELD;

typedef struct
{
    

        FIELD rsv_0                     : 2;
        FIELD SGMII_UNIDIR_ENA          : 4;
        FIELD SGMII_SPEED_MSB           : 1;
        FIELD SGMII_COL_TEST            : 1;
        FIELD SGMII_DUPLEX              : 1;
        FIELD SGMII_AN_RESTART          : 1;
        FIELD SGMII_ISOLATE             : 1;
        FIELD SGMII_POWERDOWN           : 1;
        FIELD SGMII_AN_ENABLE           : 1;
        FIELD SGMII_SPEED_LSB           : 1;
        FIELD SGMII_LOOPBACK_ENA        : 1;
        FIELD SGMII_RESET_PHY           : 1;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an0, *PREG_sgmii_reg_an0;

typedef struct
{
    

        FIELD rsv_0                     : 2;
        FIELD SGMII_LINK_STATUS         : 1;
        FIELD SGMII_AN_ABILITY          : 1;
        FIELD SGMII_REMOTE_FAULT        : 1;
        FIELD SGMII_AN_COMPLETE         : 1;
        FIELD SGMII_UNIDIR_ABILITY      : 1;
        FIELD SGMII_PCS_FAULT           : 1;
        FIELD rsv_8                     : 6;
        FIELD SGMII_AN_EXPANSION_CLR    : 1;
        FIELD SGMII_LINK_RST            : 1;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_1, *PREG_sgmii_reg_an_1;

typedef struct
{
    

        FIELD SGMII_PHY_IDENTIFIER_REG2 : 16;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_2, *PREG_sgmii_reg_an_2;

typedef struct
{
    

        FIELD SGMII_PHY_IDENTIFIER_REG3 : 16;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_3, *PREG_sgmii_reg_an_3;

typedef struct
{
    

        FIELD SGMII_DEV_ABILITY_0       : 1;
        FIELD SGMII_DEV_ABILITY_1       : 1;
        FIELD SGMII_DEV_ABILITY_2       : 1;
        FIELD SGMII_DEV_ABILITY_3       : 1;
        FIELD SGMII_DEV_ABILITY_13_4    : 10;
        FIELD SGMII_AN_ACK              : 1;
        FIELD SGMII_DEV_ABILITY_15      : 1;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_4, *PREG_sgmii_reg_an_4;

typedef struct
{
    

        FIELD SGMII_PARTNER_ABILITY_0   : 1;
        FIELD SGMII_PARTNER_ABILITY_1   : 1;
        FIELD SGMII_PARTNER_ABILITY_2   : 1;
        FIELD SGMII_PARTNER_ABILITY_3   : 1;
        FIELD SGMII_PARTNER_ABILITY_4   : 1;
        FIELD SGMII_PARTNER_ABILITY_5   : 1;
        FIELD SGMII_PARTNER_ABILITY_6   : 1;
        FIELD SGMII_PARTNER_ABILITY_7   : 1;
        FIELD SGMII_PARTNER_ABILITY_8   : 1;
        FIELD SGMII_PARTNER_ABILITY_9   : 1;
        FIELD SGMII_PARTNER_ABILITY_10  : 1;
        FIELD SGMII_PARTNER_ABILITY_15_11 : 5;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_5, *PREG_sgmii_reg_an_5;

typedef struct
{
    

        FIELD SGMII_AN_EXPANSION_12_0   : 13;
        FIELD SGMII_AN_EXPANSION_13     : 1;
        FIELD SGMII_AN_EXPANSION_14     : 1;
        FIELD SGMII_AN_EXPANSION_15     : 1;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_6, *PREG_sgmii_reg_an_6;

typedef struct
{
    

        FIELD SGMII_NP_TX_2_0           : 3;
        FIELD SGMII_NP_TX_7_3           : 5;
        FIELD SGMII_NP_TX_15_8          : 8;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_7, *PREG_sgmii_reg_an_7;

typedef struct
{
    

        FIELD rsv_0                     : 10;
        FIELD SGMII_RX_FAULT_LATCH      : 1;
        FIELD SGMII_TX_FAULT_LATCH      : 1;
        FIELD rsv_12                    : 20;
    
    
} REG_sgmii_reg_an_8, *PREG_sgmii_reg_an_8;

typedef struct
{
    

        FIELD SGMII_SCRATCH             : 16;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_9, *PREG_sgmii_reg_an_9;

typedef struct
{
    

        FIELD SGMII_DEV_VERSION         : 16;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_10, *PREG_sgmii_reg_an_10;

typedef struct
{
    

        FIELD SGMII_LINK_TIMER          : 20;
        FIELD rsv_20                    : 12;
    
    
} REG_sgmii_reg_an_11, *PREG_sgmii_reg_an_11;

typedef struct
{
    

        FIELD SGMII_DEC_ERROR_CNT       : 16;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_12, *PREG_sgmii_reg_an_12;

typedef struct
{
    

        FIELD SGMII_IF_MODE_5_0         : 6;
        FIELD rsv_6                     : 2;
        FIELD SGMII_REMOTE_FAULT_DIS    : 1;
        FIELD SGMII_CODE_SYNC_SET_VAL   : 1;
        FIELD SGMII_CODE_SYNC_SET_EN    : 1;
        FIELD SGMII_SEND_AN_ERROR_EN    : 1;
        FIELD SGMII_AN_TRANSPARENT      : 1;
        FIELD SGMII_AN_BLOCK_ACK        : 1;
        FIELD rsv_14                    : 2;
        FIELD SGMII_TXBUF_FULL_THR      : 4;
        FIELD SGMII_TXBUF_EMPTY_THR     : 4;
        FIELD SGMII_RXBUF_FULL_THR      : 4;
        FIELD SGMII_RXBUF_EMPTY_THR     : 4;
    
    
} REG_sgmii_reg_an_13, *PREG_sgmii_reg_an_13;

typedef struct
{
    

        FIELD SGMII_RESERVED_REG        : 32;
    
    
} REG_sgmii_reg_an_14, *PREG_sgmii_reg_an_14;

typedef struct
{
    

        FIELD SGMII_FALSE_CARRIER_CNT   : 32;
    
    
} REG_sgmii_reg_an_15, *PREG_sgmii_reg_an_15;

typedef struct
{
    

        FIELD SGMII_RO_NP_TX            : 16;
        FIELD SGMII_RO_LINK_RST         : 1;
        FIELD SGMII_RO_AN_EXPANSION_CLR : 1;
        FIELD SGMII_RO_AN_RESTART       : 1;
        FIELD SGMII_RO_SW_RESET         : 1;
        FIELD rsv_20                    : 12;
    
    
} REG_sgmii_reg_an_16, *PREG_sgmii_reg_an_16;

typedef struct
{
    

        FIELD SGMII_ELA_MAC_TXCK_SEL    : 4;
        FIELD SGMII_ELA_MAC_RXCK_SEL    : 4;
        FIELD SGMII_ELA_PMA_TXCK_SEL    : 4;
        FIELD SGMII_ELA_PMA_RXCK_SEL    : 4;
        FIELD SGMII_ELA_CSR_SEL         : 4;
        FIELD SGMII_ERR_PROP_KI_SIG_LOS_R : 1;
        FIELD SGMII_ERR_PROP_KI_SYNC_F  : 1;
        FIELD SGMII_ERR_PROP_KO_SIG_LOS_F : 1;
        FIELD SGMII_ERR_PROP_KO_SYNC_R  : 1;
        FIELD SGMII_1US_TIMER           : 8;
    
    
} REG_sgmii_reg_an_17, *PREG_sgmii_reg_an_17;

typedef struct
{
    

        FIELD SGMII_SW_RESET            : 1;
        FIELD rsv_1                     : 31;
    
    
} REG_sgmii_reg_an_18, *PREG_sgmii_reg_an_18;

typedef struct
{
    

        FIELD SGMII_LP_NP_RX_0          : 1;
        FIELD SGMII_LP_NP_RX_1          : 1;
        FIELD SGMII_LP_NP_RX_2          : 1;
        FIELD SGMII_LP_NP_RX_3          : 1;
        FIELD SGMII_LP_NP_RX_4          : 1;
        FIELD SGMII_LP_NP_RX_5          : 1;
        FIELD SGMII_LP_NP_RX_9_6        : 4;
        FIELD SGMII_LP_NP_RX_10         : 1;
        FIELD SGMII_LP_NP_RX_11         : 1;
        FIELD SGMII_LP_NP_RX_13_12      : 2;
        FIELD SGMII_LP_NP_RX_15_14      : 2;
        FIELD rsv_16                    : 16;
    
    
} REG_sgmii_reg_an_19, *PREG_sgmii_reg_an_19;

typedef struct
{
    

        FIELD rsv_0                     : 2;
        FIELD SGMII_TX_FAULT_LATCH_CLR  : 1;
        FIELD SGMII_RX_FAULT_LATCH_CLR  : 1;
        FIELD rsv_4                     : 28;
    
    
} REG_sgmii_reg_an_20, *PREG_sgmii_reg_an_20;

typedef struct
{
    

        FIELD SGMII_PAT_GEN_TX_EN       : 1;
        FIELD rsv_1                     : 26;
        FIELD SGMII_PCS_MODE            : 3;
        FIELD SGMII_PCS_DEC_ERROR_CNT_READ_LATCH : 1;
        FIELD SGMII_FALSE_CARRIER_CNT_READ_LATCH : 1;
    
    
} REG_sgmii_reg_an_21, *PREG_sgmii_reg_an_21;

typedef struct
{
    

        FIELD SGMII_REGWR_NP_TX_2_0_EN  : 1;
        FIELD SGMII_REGWR_NP_TX_7_3_EN  : 1;
        FIELD SGMII_REGWR_NP_TX_15_8_EN : 1;
        FIELD rsv_3                     : 29;
    
    
} REG_sgmii_reg_an_22, *PREG_sgmii_reg_an_22;

typedef struct
{
    

        FIELD rg_force_an_done          : 1;
        FIELD rsv_1                     : 31;
    
    
} REG_sgmii_reg_an_force_cl37, *PREG_sgmii_reg_an_force_cl37;

// ----------------- hsgmii_an_csr  Grouping Definitions -------------------
// ----------------- hsgmii_an_csr Register Definition -------------------
#if 0
typedef volatile 
{
    REG_sgmii_reg_an0               sgmii_reg_an0;    // 0000
    REG_sgmii_reg_an_1              sgmii_reg_an_1;   // 0004
    REG_sgmii_reg_an_2              sgmii_reg_an_2;   // 0008
    REG_sgmii_reg_an_3              sgmii_reg_an_3;   // 000C
    REG_sgmii_reg_an_4              sgmii_reg_an_4;   // 0010
    REG_sgmii_reg_an_5              sgmii_reg_an_5;   // 0014
    REG_sgmii_reg_an_6              sgmii_reg_an_6;   // 0018
    REG_sgmii_reg_an_7              sgmii_reg_an_7;   // 001C
    REG_sgmii_reg_an_8              sgmii_reg_an_8;   // 0020
    REG_sgmii_reg_an_9              sgmii_reg_an_9;   // 0024
    REG_sgmii_reg_an_10             sgmii_reg_an_10;  // 0028
    REG_sgmii_reg_an_11             sgmii_reg_an_11;  // 002C
    REG_sgmii_reg_an_12             sgmii_reg_an_12;  // 0030
    REG_sgmii_reg_an_13             sgmii_reg_an_13;  // 0034
    REG_sgmii_reg_an_14             sgmii_reg_an_14;  // 0038
    REG_sgmii_reg_an_15             sgmii_reg_an_15;  // 003C
    REG_sgmii_reg_an_16             sgmii_reg_an_16;  // 0040
    REG_sgmii_reg_an_17             sgmii_reg_an_17;  // 0044
    REG_sgmii_reg_an_18             sgmii_reg_an_18;  // 0048
    REG_sgmii_reg_an_19             sgmii_reg_an_19;  // 004C
    REG_sgmii_reg_an_20             sgmii_reg_an_20;  // 0050
    REG_sgmii_reg_an_21             sgmii_reg_an_21;  // 0054
    REG_sgmii_reg_an_22             sgmii_reg_an_22;  // 0058
    UINT32                          rsv_005C;         // 005C
    REG_sgmii_reg_an_force_cl37     sgmii_reg_an_force_cl37; // 0060
}hsgmii_an_csr_REGS, *Phsgmii_an_csr_REGS;

// ---------- hsgmii_an_csr Enum Definitions      ----------
// ---------- hsgmii_an_csr C Macro Definitions   ----------
extern Phsgmii_an_csr_REGS g_hsgmii_an_csr_BASE;

#define hsgmii_an_csr_BASE                                     (g_hsgmii_an_csr_BASE)
#endif

#define _sgmii_reg_an0		0x0000
#define _sgmii_reg_an_1		0x0004
#define _sgmii_reg_an_2		0x0008
#define _sgmii_reg_an_3		0x000C
#define _sgmii_reg_an_4		0x0010
#define _sgmii_reg_an_5		0x0014
#define _sgmii_reg_an_6		0x0018
#define _sgmii_reg_an_7		0x001C
#define _sgmii_reg_an_8		0x0020
#define _sgmii_reg_an_9		0x0024
#define _sgmii_reg_an_10		0x0028
#define _sgmii_reg_an_11		0x002C
#define _sgmii_reg_an_12		0x0030
#define _sgmii_reg_an_13		0x0034
#define _sgmii_reg_an_14		0x0038
#define _sgmii_reg_an_15		0x003C
#define _sgmii_reg_an_16		0x0040
#define _sgmii_reg_an_17		0x0044
#define _sgmii_reg_an_18		0x0048
#define _sgmii_reg_an_19		0x004C
#define _sgmii_reg_an_20		0x0050
#define _sgmii_reg_an_21		0x0054
#define _sgmii_reg_an_22		0x0058
#define _sgmii_reg_an_force_cl37		0x0060

#endif


#define sgmii_reg_an0_FLD_SGMII_RESET_PHY                      REG_FLD(1, 15)
#define sgmii_reg_an0_FLD_SGMII_LOOPBACK_ENA                   REG_FLD(1, 14)
#define sgmii_reg_an0_FLD_SGMII_SPEED_LSB                      REG_FLD(1, 13)
#define sgmii_reg_an0_FLD_SGMII_AN_ENABLE                      REG_FLD(1, 12)
#define sgmii_reg_an0_FLD_SGMII_POWERDOWN                      REG_FLD(1, 11)
#define sgmii_reg_an0_FLD_SGMII_ISOLATE                        REG_FLD(1, 10)
#define sgmii_reg_an0_FLD_SGMII_AN_RESTART                     REG_FLD(1, 9)
#define sgmii_reg_an0_FLD_SGMII_DUPLEX                         REG_FLD(1, 8)
#define sgmii_reg_an0_FLD_SGMII_COL_TEST                       REG_FLD(1, 7)
#define sgmii_reg_an0_FLD_SGMII_SPEED_MSB                      REG_FLD(1, 6)
#define sgmii_reg_an0_FLD_SGMII_UNIDIR_ENA                     REG_FLD(4, 2)

#define sgmii_reg_an_1_FLD_SGMII_LINK_RST                      REG_FLD(1, 15)
#define sgmii_reg_an_1_FLD_SGMII_AN_EXPANSION_CLR              REG_FLD(1, 14)
#define sgmii_reg_an_1_FLD_SGMII_PCS_FAULT                     REG_FLD(1, 7)
#define sgmii_reg_an_1_FLD_SGMII_UNIDIR_ABILITY                REG_FLD(1, 6)
#define sgmii_reg_an_1_FLD_SGMII_AN_COMPLETE                   REG_FLD(1, 5)
#define sgmii_reg_an_1_FLD_SGMII_REMOTE_FAULT                  REG_FLD(1, 4)
#define sgmii_reg_an_1_FLD_SGMII_AN_ABILITY                    REG_FLD(1, 3)
#define sgmii_reg_an_1_FLD_SGMII_LINK_STATUS                   REG_FLD(1, 2)

#define sgmii_reg_an_2_FLD_SGMII_PHY_IDENTIFIER_REG2           REG_FLD(16, 0)

#define sgmii_reg_an_3_FLD_SGMII_PHY_IDENTIFIER_REG3           REG_FLD(16, 0)

#define sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_15                REG_FLD(1, 15)
#define sgmii_reg_an_4_FLD_SGMII_AN_ACK                        REG_FLD(1, 14)
#define sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_13_4              REG_FLD(10, 4)
#define sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_3                 REG_FLD(1, 3)
#define sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_2                 REG_FLD(1, 2)
#define sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_1                 REG_FLD(1, 1)
#define sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_0                 REG_FLD(1, 0)

#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_15_11         REG_FLD(5, 11)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_10            REG_FLD(1, 10)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_9             REG_FLD(1, 9)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_8             REG_FLD(1, 8)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_7             REG_FLD(1, 7)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_6             REG_FLD(1, 6)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_5             REG_FLD(1, 5)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_4             REG_FLD(1, 4)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_3             REG_FLD(1, 3)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_2             REG_FLD(1, 2)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_1             REG_FLD(1, 1)
#define sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_0             REG_FLD(1, 0)

#define sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_15               REG_FLD(1, 15)
#define sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_14               REG_FLD(1, 14)
#define sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_13               REG_FLD(1, 13)
#define sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_12_0             REG_FLD(13, 0)

#define sgmii_reg_an_7_FLD_SGMII_NP_TX_15_8                    REG_FLD(8, 8)
#define sgmii_reg_an_7_FLD_SGMII_NP_TX_7_3                     REG_FLD(5, 3)
#define sgmii_reg_an_7_FLD_SGMII_NP_TX_2_0                     REG_FLD(3, 0)

#define sgmii_reg_an_8_FLD_SGMII_TX_FAULT_LATCH                REG_FLD(1, 11)
#define sgmii_reg_an_8_FLD_SGMII_RX_FAULT_LATCH                REG_FLD(1, 10)

#define sgmii_reg_an_9_FLD_SGMII_SCRATCH                       REG_FLD(16, 0)

#define sgmii_reg_an_10_FLD_SGMII_DEV_VERSION                  REG_FLD(16, 0)

#define sgmii_reg_an_11_FLD_SGMII_LINK_TIMER                   REG_FLD(20, 0)

#define sgmii_reg_an_12_FLD_SGMII_DEC_ERROR_CNT                REG_FLD(16, 0)

#define sgmii_reg_an_13_FLD_SGMII_RXBUF_EMPTY_THR              REG_FLD(4, 28)
#define sgmii_reg_an_13_FLD_SGMII_RXBUF_FULL_THR               REG_FLD(4, 24)
#define sgmii_reg_an_13_FLD_SGMII_TXBUF_EMPTY_THR              REG_FLD(4, 20)
#define sgmii_reg_an_13_FLD_SGMII_TXBUF_FULL_THR               REG_FLD(4, 16)
#define sgmii_reg_an_13_FLD_SGMII_AN_BLOCK_ACK                 REG_FLD(1, 13)
#define sgmii_reg_an_13_FLD_SGMII_AN_TRANSPARENT               REG_FLD(1, 12)
#define sgmii_reg_an_13_FLD_SGMII_SEND_AN_ERROR_EN             REG_FLD(1, 11)
#define sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_EN             REG_FLD(1, 10)
#define sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_VAL            REG_FLD(1, 9)
#define sgmii_reg_an_13_FLD_SGMII_REMOTE_FAULT_DIS             REG_FLD(1, 8)
#define sgmii_reg_an_13_FLD_SGMII_IF_MODE_5_0                  REG_FLD(6, 0)

#define sgmii_reg_an_14_FLD_SGMII_RESERVED_REG                 REG_FLD(32, 0)

#define sgmii_reg_an_15_FLD_SGMII_FALSE_CARRIER_CNT            REG_FLD(32, 0)

#define sgmii_reg_an_16_FLD_SGMII_RO_SW_RESET                  REG_FLD(1, 19)
#define sgmii_reg_an_16_FLD_SGMII_RO_AN_RESTART                REG_FLD(1, 18)
#define sgmii_reg_an_16_FLD_SGMII_RO_AN_EXPANSION_CLR          REG_FLD(1, 17)
#define sgmii_reg_an_16_FLD_SGMII_RO_LINK_RST                  REG_FLD(1, 16)
#define sgmii_reg_an_16_FLD_SGMII_RO_NP_TX                     REG_FLD(16, 0)

#define sgmii_reg_an_17_FLD_SGMII_1US_TIMER                    REG_FLD(8, 24)
#define sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SYNC_R           REG_FLD(1, 23)
#define sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SIG_LOS_F        REG_FLD(1, 22)
#define sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SYNC_F           REG_FLD(1, 21)
#define sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SIG_LOS_R        REG_FLD(1, 20)
#define sgmii_reg_an_17_FLD_SGMII_ELA_CSR_SEL                  REG_FLD(4, 16)
#define sgmii_reg_an_17_FLD_SGMII_ELA_PMA_RXCK_SEL             REG_FLD(4, 12)
#define sgmii_reg_an_17_FLD_SGMII_ELA_PMA_TXCK_SEL             REG_FLD(4, 8)
#define sgmii_reg_an_17_FLD_SGMII_ELA_MAC_RXCK_SEL             REG_FLD(4, 4)
#define sgmii_reg_an_17_FLD_SGMII_ELA_MAC_TXCK_SEL             REG_FLD(4, 0)

#define sgmii_reg_an_18_FLD_SGMII_SW_RESET                     REG_FLD(1, 0)

#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_15_14               REG_FLD(2, 14)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_13_12               REG_FLD(2, 12)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_11                  REG_FLD(1, 11)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_10                  REG_FLD(1, 10)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_9_6                 REG_FLD(4, 6)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_5                   REG_FLD(1, 5)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_4                   REG_FLD(1, 4)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_3                   REG_FLD(1, 3)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_2                   REG_FLD(1, 2)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_1                   REG_FLD(1, 1)
#define sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_0                   REG_FLD(1, 0)

#define sgmii_reg_an_20_FLD_SGMII_RX_FAULT_LATCH_CLR           REG_FLD(1, 3)
#define sgmii_reg_an_20_FLD_SGMII_TX_FAULT_LATCH_CLR           REG_FLD(1, 2)

#define sgmii_reg_an_21_FLD_SGMII_FALSE_CARRIER_CNT_READ_LATCH REG_FLD(1, 31)
#define sgmii_reg_an_21_FLD_SGMII_PCS_DEC_ERROR_CNT_READ_LATCH REG_FLD(1, 30)
#define sgmii_reg_an_21_FLD_SGMII_PCS_MODE                     REG_FLD(3, 27)
#define sgmii_reg_an_21_FLD_SGMII_PAT_GEN_TX_EN                REG_FLD(1, 0)

#define sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_15_8_EN          REG_FLD(1, 2)
#define sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_7_3_EN           REG_FLD(1, 1)
#define sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_2_0_EN           REG_FLD(1, 0)

#define sgmii_reg_an_force_cl37_FLD_rg_force_an_done           REG_FLD(1, 0)

#define sgmii_reg_an0_GET_SGMII_RESET_PHY(reg32)               REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_RESET_PHY, (reg32))
#define sgmii_reg_an0_GET_SGMII_LOOPBACK_ENA(reg32)            REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_LOOPBACK_ENA, (reg32))
#define sgmii_reg_an0_GET_SGMII_SPEED_LSB(reg32)               REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_SPEED_LSB, (reg32))
#define sgmii_reg_an0_GET_SGMII_AN_ENABLE(reg32)               REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_AN_ENABLE, (reg32))
#define sgmii_reg_an0_GET_SGMII_POWERDOWN(reg32)               REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_POWERDOWN, (reg32))
#define sgmii_reg_an0_GET_SGMII_ISOLATE(reg32)                 REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_ISOLATE, (reg32))
#define sgmii_reg_an0_GET_SGMII_AN_RESTART(reg32)              REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_AN_RESTART, (reg32))
#define sgmii_reg_an0_GET_SGMII_DUPLEX(reg32)                  REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_DUPLEX, (reg32))
#define sgmii_reg_an0_GET_SGMII_COL_TEST(reg32)                REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_COL_TEST, (reg32))
#define sgmii_reg_an0_GET_SGMII_SPEED_MSB(reg32)               REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_SPEED_MSB, (reg32))
#define sgmii_reg_an0_GET_SGMII_UNIDIR_ENA(reg32)              REG_FLD_GET(sgmii_reg_an0_FLD_SGMII_UNIDIR_ENA, (reg32))

#define sgmii_reg_an_1_GET_SGMII_LINK_RST(reg32)               REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_LINK_RST, (reg32))
#define sgmii_reg_an_1_GET_SGMII_AN_EXPANSION_CLR(reg32)       REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_AN_EXPANSION_CLR, (reg32))
#define sgmii_reg_an_1_GET_SGMII_PCS_FAULT(reg32)              REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_PCS_FAULT, (reg32))
#define sgmii_reg_an_1_GET_SGMII_UNIDIR_ABILITY(reg32)         REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_UNIDIR_ABILITY, (reg32))
#define sgmii_reg_an_1_GET_SGMII_AN_COMPLETE(reg32)            REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_AN_COMPLETE, (reg32))
#define sgmii_reg_an_1_GET_SGMII_REMOTE_FAULT(reg32)           REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_REMOTE_FAULT, (reg32))
#define sgmii_reg_an_1_GET_SGMII_AN_ABILITY(reg32)             REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_AN_ABILITY, (reg32))
#define sgmii_reg_an_1_GET_SGMII_LINK_STATUS(reg32)            REG_FLD_GET(sgmii_reg_an_1_FLD_SGMII_LINK_STATUS, (reg32))

#define sgmii_reg_an_2_GET_SGMII_PHY_IDENTIFIER_REG2(reg32)    REG_FLD_GET(sgmii_reg_an_2_FLD_SGMII_PHY_IDENTIFIER_REG2, (reg32))

#define sgmii_reg_an_3_GET_SGMII_PHY_IDENTIFIER_REG3(reg32)    REG_FLD_GET(sgmii_reg_an_3_FLD_SGMII_PHY_IDENTIFIER_REG3, (reg32))

#define sgmii_reg_an_4_GET_SGMII_DEV_ABILITY_15(reg32)         REG_FLD_GET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_15, (reg32))
#define sgmii_reg_an_4_GET_SGMII_AN_ACK(reg32)                 REG_FLD_GET(sgmii_reg_an_4_FLD_SGMII_AN_ACK, (reg32))
#define sgmii_reg_an_4_GET_SGMII_DEV_ABILITY_13_4(reg32)       REG_FLD_GET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_13_4, (reg32))
#define sgmii_reg_an_4_GET_SGMII_DEV_ABILITY_3(reg32)          REG_FLD_GET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_3, (reg32))
#define sgmii_reg_an_4_GET_SGMII_DEV_ABILITY_2(reg32)          REG_FLD_GET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_2, (reg32))
#define sgmii_reg_an_4_GET_SGMII_DEV_ABILITY_1(reg32)          REG_FLD_GET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_1, (reg32))
#define sgmii_reg_an_4_GET_SGMII_DEV_ABILITY_0(reg32)          REG_FLD_GET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_0, (reg32))

#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_15_11(reg32)  REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_15_11, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_10(reg32)     REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_10, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_9(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_9, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_8(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_8, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_7(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_7, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_6(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_6, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_5(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_5, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_4(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_4, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_3(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_3, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_2(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_2, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_1(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_1, (reg32))
#define sgmii_reg_an_5_GET_SGMII_PARTNER_ABILITY_0(reg32)      REG_FLD_GET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_0, (reg32))

#define sgmii_reg_an_6_GET_SGMII_AN_EXPANSION_15(reg32)        REG_FLD_GET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_15, (reg32))
#define sgmii_reg_an_6_GET_SGMII_AN_EXPANSION_14(reg32)        REG_FLD_GET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_14, (reg32))
#define sgmii_reg_an_6_GET_SGMII_AN_EXPANSION_13(reg32)        REG_FLD_GET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_13, (reg32))
#define sgmii_reg_an_6_GET_SGMII_AN_EXPANSION_12_0(reg32)      REG_FLD_GET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_12_0, (reg32))

#define sgmii_reg_an_7_GET_SGMII_NP_TX_15_8(reg32)             REG_FLD_GET(sgmii_reg_an_7_FLD_SGMII_NP_TX_15_8, (reg32))
#define sgmii_reg_an_7_GET_SGMII_NP_TX_7_3(reg32)              REG_FLD_GET(sgmii_reg_an_7_FLD_SGMII_NP_TX_7_3, (reg32))
#define sgmii_reg_an_7_GET_SGMII_NP_TX_2_0(reg32)              REG_FLD_GET(sgmii_reg_an_7_FLD_SGMII_NP_TX_2_0, (reg32))

#define sgmii_reg_an_8_GET_SGMII_TX_FAULT_LATCH(reg32)         REG_FLD_GET(sgmii_reg_an_8_FLD_SGMII_TX_FAULT_LATCH, (reg32))
#define sgmii_reg_an_8_GET_SGMII_RX_FAULT_LATCH(reg32)         REG_FLD_GET(sgmii_reg_an_8_FLD_SGMII_RX_FAULT_LATCH, (reg32))

#define sgmii_reg_an_9_GET_SGMII_SCRATCH(reg32)                REG_FLD_GET(sgmii_reg_an_9_FLD_SGMII_SCRATCH, (reg32))

#define sgmii_reg_an_10_GET_SGMII_DEV_VERSION(reg32)           REG_FLD_GET(sgmii_reg_an_10_FLD_SGMII_DEV_VERSION, (reg32))

#define sgmii_reg_an_11_GET_SGMII_LINK_TIMER(reg32)            REG_FLD_GET(sgmii_reg_an_11_FLD_SGMII_LINK_TIMER, (reg32))

#define sgmii_reg_an_12_GET_SGMII_DEC_ERROR_CNT(reg32)         REG_FLD_GET(sgmii_reg_an_12_FLD_SGMII_DEC_ERROR_CNT, (reg32))

#define sgmii_reg_an_13_GET_SGMII_RXBUF_EMPTY_THR(reg32)       REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_RXBUF_EMPTY_THR, (reg32))
#define sgmii_reg_an_13_GET_SGMII_RXBUF_FULL_THR(reg32)        REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_RXBUF_FULL_THR, (reg32))
#define sgmii_reg_an_13_GET_SGMII_TXBUF_EMPTY_THR(reg32)       REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_TXBUF_EMPTY_THR, (reg32))
#define sgmii_reg_an_13_GET_SGMII_TXBUF_FULL_THR(reg32)        REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_TXBUF_FULL_THR, (reg32))
#define sgmii_reg_an_13_GET_SGMII_AN_BLOCK_ACK(reg32)          REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_AN_BLOCK_ACK, (reg32))
#define sgmii_reg_an_13_GET_SGMII_AN_TRANSPARENT(reg32)        REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_AN_TRANSPARENT, (reg32))
#define sgmii_reg_an_13_GET_SGMII_SEND_AN_ERROR_EN(reg32)      REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_SEND_AN_ERROR_EN, (reg32))
#define sgmii_reg_an_13_GET_SGMII_CODE_SYNC_SET_EN(reg32)      REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_EN, (reg32))
#define sgmii_reg_an_13_GET_SGMII_CODE_SYNC_SET_VAL(reg32)     REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_VAL, (reg32))
#define sgmii_reg_an_13_GET_SGMII_REMOTE_FAULT_DIS(reg32)      REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_REMOTE_FAULT_DIS, (reg32))
#define sgmii_reg_an_13_GET_SGMII_IF_MODE_5_0(reg32)           REG_FLD_GET(sgmii_reg_an_13_FLD_SGMII_IF_MODE_5_0, (reg32))

#define sgmii_reg_an_14_GET_SGMII_RESERVED_REG(reg32)          REG_FLD_GET(sgmii_reg_an_14_FLD_SGMII_RESERVED_REG, (reg32))

#define sgmii_reg_an_15_GET_SGMII_FALSE_CARRIER_CNT(reg32)     REG_FLD_GET(sgmii_reg_an_15_FLD_SGMII_FALSE_CARRIER_CNT, (reg32))

#define sgmii_reg_an_16_GET_SGMII_RO_SW_RESET(reg32)           REG_FLD_GET(sgmii_reg_an_16_FLD_SGMII_RO_SW_RESET, (reg32))
#define sgmii_reg_an_16_GET_SGMII_RO_AN_RESTART(reg32)         REG_FLD_GET(sgmii_reg_an_16_FLD_SGMII_RO_AN_RESTART, (reg32))
#define sgmii_reg_an_16_GET_SGMII_RO_AN_EXPANSION_CLR(reg32)   REG_FLD_GET(sgmii_reg_an_16_FLD_SGMII_RO_AN_EXPANSION_CLR, (reg32))
#define sgmii_reg_an_16_GET_SGMII_RO_LINK_RST(reg32)           REG_FLD_GET(sgmii_reg_an_16_FLD_SGMII_RO_LINK_RST, (reg32))
#define sgmii_reg_an_16_GET_SGMII_RO_NP_TX(reg32)              REG_FLD_GET(sgmii_reg_an_16_FLD_SGMII_RO_NP_TX, (reg32))

#define sgmii_reg_an_17_GET_SGMII_1US_TIMER(reg32)             REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_1US_TIMER, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ERR_PROP_KO_SYNC_R(reg32)    REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SYNC_R, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ERR_PROP_KO_SIG_LOS_F(reg32) REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SIG_LOS_F, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ERR_PROP_KI_SYNC_F(reg32)    REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SYNC_F, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ERR_PROP_KI_SIG_LOS_R(reg32) REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SIG_LOS_R, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ELA_CSR_SEL(reg32)           REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ELA_CSR_SEL, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ELA_PMA_RXCK_SEL(reg32)      REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ELA_PMA_RXCK_SEL, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ELA_PMA_TXCK_SEL(reg32)      REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ELA_PMA_TXCK_SEL, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ELA_MAC_RXCK_SEL(reg32)      REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ELA_MAC_RXCK_SEL, (reg32))
#define sgmii_reg_an_17_GET_SGMII_ELA_MAC_TXCK_SEL(reg32)      REG_FLD_GET(sgmii_reg_an_17_FLD_SGMII_ELA_MAC_TXCK_SEL, (reg32))

#define sgmii_reg_an_18_GET_SGMII_SW_RESET(reg32)              REG_FLD_GET(sgmii_reg_an_18_FLD_SGMII_SW_RESET, (reg32))

#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_15_14(reg32)        REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_15_14, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_13_12(reg32)        REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_13_12, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_11(reg32)           REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_11, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_10(reg32)           REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_10, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_9_6(reg32)          REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_9_6, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_5(reg32)            REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_5, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_4(reg32)            REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_4, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_3(reg32)            REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_3, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_2(reg32)            REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_2, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_1(reg32)            REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_1, (reg32))
#define sgmii_reg_an_19_GET_SGMII_LP_NP_RX_0(reg32)            REG_FLD_GET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_0, (reg32))

#define sgmii_reg_an_20_GET_SGMII_RX_FAULT_LATCH_CLR(reg32)    REG_FLD_GET(sgmii_reg_an_20_FLD_SGMII_RX_FAULT_LATCH_CLR, (reg32))
#define sgmii_reg_an_20_GET_SGMII_TX_FAULT_LATCH_CLR(reg32)    REG_FLD_GET(sgmii_reg_an_20_FLD_SGMII_TX_FAULT_LATCH_CLR, (reg32))

#define sgmii_reg_an_21_GET_SGMII_FALSE_CARRIER_CNT_READ_LATCH(reg32) REG_FLD_GET(sgmii_reg_an_21_FLD_SGMII_FALSE_CARRIER_CNT_READ_LATCH, (reg32))
#define sgmii_reg_an_21_GET_SGMII_PCS_DEC_ERROR_CNT_READ_LATCH(reg32) REG_FLD_GET(sgmii_reg_an_21_FLD_SGMII_PCS_DEC_ERROR_CNT_READ_LATCH, (reg32))
#define sgmii_reg_an_21_GET_SGMII_PCS_MODE(reg32)              REG_FLD_GET(sgmii_reg_an_21_FLD_SGMII_PCS_MODE, (reg32))
#define sgmii_reg_an_21_GET_SGMII_PAT_GEN_TX_EN(reg32)         REG_FLD_GET(sgmii_reg_an_21_FLD_SGMII_PAT_GEN_TX_EN, (reg32))

#define sgmii_reg_an_22_GET_SGMII_REGWR_NP_TX_15_8_EN(reg32)   REG_FLD_GET(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_15_8_EN, (reg32))
#define sgmii_reg_an_22_GET_SGMII_REGWR_NP_TX_7_3_EN(reg32)    REG_FLD_GET(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_7_3_EN, (reg32))
#define sgmii_reg_an_22_GET_SGMII_REGWR_NP_TX_2_0_EN(reg32)    REG_FLD_GET(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_2_0_EN, (reg32))

#define sgmii_reg_an_force_cl37_GET_rg_force_an_done(reg32)    REG_FLD_GET(sgmii_reg_an_force_cl37_FLD_rg_force_an_done, (reg32))

#define sgmii_reg_an0_SET_SGMII_RESET_PHY(reg32, val)          REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_RESET_PHY, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_LOOPBACK_ENA(reg32, val)       REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_LOOPBACK_ENA, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_SPEED_LSB(reg32, val)          REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_SPEED_LSB, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_AN_ENABLE(reg32, val)          REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_AN_ENABLE, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_POWERDOWN(reg32, val)          REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_POWERDOWN, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_ISOLATE(reg32, val)            REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_ISOLATE, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_AN_RESTART(reg32, val)         REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_AN_RESTART, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_DUPLEX(reg32, val)             REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_DUPLEX, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_COL_TEST(reg32, val)           REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_COL_TEST, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_SPEED_MSB(reg32, val)          REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_SPEED_MSB, (reg32), (val))
#define sgmii_reg_an0_SET_SGMII_UNIDIR_ENA(reg32, val)         REG_FLD_SET(sgmii_reg_an0_FLD_SGMII_UNIDIR_ENA, (reg32), (val))

#define sgmii_reg_an_1_SET_SGMII_LINK_RST(reg32, val)          REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_LINK_RST, (reg32), (val))
#define sgmii_reg_an_1_SET_SGMII_AN_EXPANSION_CLR(reg32, val)  REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_AN_EXPANSION_CLR, (reg32), (val))
#define sgmii_reg_an_1_SET_SGMII_PCS_FAULT(reg32, val)         REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_PCS_FAULT, (reg32), (val))
#define sgmii_reg_an_1_SET_SGMII_UNIDIR_ABILITY(reg32, val)    REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_UNIDIR_ABILITY, (reg32), (val))
#define sgmii_reg_an_1_SET_SGMII_AN_COMPLETE(reg32, val)       REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_AN_COMPLETE, (reg32), (val))
#define sgmii_reg_an_1_SET_SGMII_REMOTE_FAULT(reg32, val)      REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_REMOTE_FAULT, (reg32), (val))
#define sgmii_reg_an_1_SET_SGMII_AN_ABILITY(reg32, val)        REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_AN_ABILITY, (reg32), (val))
#define sgmii_reg_an_1_SET_SGMII_LINK_STATUS(reg32, val)       REG_FLD_SET(sgmii_reg_an_1_FLD_SGMII_LINK_STATUS, (reg32), (val))

#define sgmii_reg_an_2_SET_SGMII_PHY_IDENTIFIER_REG2(reg32, val) REG_FLD_SET(sgmii_reg_an_2_FLD_SGMII_PHY_IDENTIFIER_REG2, (reg32), (val))

#define sgmii_reg_an_3_SET_SGMII_PHY_IDENTIFIER_REG3(reg32, val) REG_FLD_SET(sgmii_reg_an_3_FLD_SGMII_PHY_IDENTIFIER_REG3, (reg32), (val))

#define sgmii_reg_an_4_SET_SGMII_DEV_ABILITY_15(reg32, val)    REG_FLD_SET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_15, (reg32), (val))
#define sgmii_reg_an_4_SET_SGMII_AN_ACK(reg32, val)            REG_FLD_SET(sgmii_reg_an_4_FLD_SGMII_AN_ACK, (reg32), (val))
#define sgmii_reg_an_4_SET_SGMII_DEV_ABILITY_13_4(reg32, val)  REG_FLD_SET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_13_4, (reg32), (val))
#define sgmii_reg_an_4_SET_SGMII_DEV_ABILITY_3(reg32, val)     REG_FLD_SET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_3, (reg32), (val))
#define sgmii_reg_an_4_SET_SGMII_DEV_ABILITY_2(reg32, val)     REG_FLD_SET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_2, (reg32), (val))
#define sgmii_reg_an_4_SET_SGMII_DEV_ABILITY_1(reg32, val)     REG_FLD_SET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_1, (reg32), (val))
#define sgmii_reg_an_4_SET_SGMII_DEV_ABILITY_0(reg32, val)     REG_FLD_SET(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_0, (reg32), (val))

#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_15_11(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_15_11, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_10(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_10, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_9(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_9, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_8(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_8, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_7(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_7, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_6(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_6, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_5(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_5, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_4(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_4, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_3(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_3, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_2(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_2, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_1(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_1, (reg32), (val))
#define sgmii_reg_an_5_SET_SGMII_PARTNER_ABILITY_0(reg32, val) REG_FLD_SET(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_0, (reg32), (val))

#define sgmii_reg_an_6_SET_SGMII_AN_EXPANSION_15(reg32, val)   REG_FLD_SET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_15, (reg32), (val))
#define sgmii_reg_an_6_SET_SGMII_AN_EXPANSION_14(reg32, val)   REG_FLD_SET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_14, (reg32), (val))
#define sgmii_reg_an_6_SET_SGMII_AN_EXPANSION_13(reg32, val)   REG_FLD_SET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_13, (reg32), (val))
#define sgmii_reg_an_6_SET_SGMII_AN_EXPANSION_12_0(reg32, val) REG_FLD_SET(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_12_0, (reg32), (val))

#define sgmii_reg_an_7_SET_SGMII_NP_TX_15_8(reg32, val)        REG_FLD_SET(sgmii_reg_an_7_FLD_SGMII_NP_TX_15_8, (reg32), (val))
#define sgmii_reg_an_7_SET_SGMII_NP_TX_7_3(reg32, val)         REG_FLD_SET(sgmii_reg_an_7_FLD_SGMII_NP_TX_7_3, (reg32), (val))
#define sgmii_reg_an_7_SET_SGMII_NP_TX_2_0(reg32, val)         REG_FLD_SET(sgmii_reg_an_7_FLD_SGMII_NP_TX_2_0, (reg32), (val))

#define sgmii_reg_an_8_SET_SGMII_TX_FAULT_LATCH(reg32, val)    REG_FLD_SET(sgmii_reg_an_8_FLD_SGMII_TX_FAULT_LATCH, (reg32), (val))
#define sgmii_reg_an_8_SET_SGMII_RX_FAULT_LATCH(reg32, val)    REG_FLD_SET(sgmii_reg_an_8_FLD_SGMII_RX_FAULT_LATCH, (reg32), (val))

#define sgmii_reg_an_9_SET_SGMII_SCRATCH(reg32, val)           REG_FLD_SET(sgmii_reg_an_9_FLD_SGMII_SCRATCH, (reg32), (val))

#define sgmii_reg_an_10_SET_SGMII_DEV_VERSION(reg32, val)      REG_FLD_SET(sgmii_reg_an_10_FLD_SGMII_DEV_VERSION, (reg32), (val))

#define sgmii_reg_an_11_SET_SGMII_LINK_TIMER(reg32, val)       REG_FLD_SET(sgmii_reg_an_11_FLD_SGMII_LINK_TIMER, (reg32), (val))

#define sgmii_reg_an_12_SET_SGMII_DEC_ERROR_CNT(reg32, val)    REG_FLD_SET(sgmii_reg_an_12_FLD_SGMII_DEC_ERROR_CNT, (reg32), (val))

#define sgmii_reg_an_13_SET_SGMII_RXBUF_EMPTY_THR(reg32, val)  REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_RXBUF_EMPTY_THR, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_RXBUF_FULL_THR(reg32, val)   REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_RXBUF_FULL_THR, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_TXBUF_EMPTY_THR(reg32, val)  REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_TXBUF_EMPTY_THR, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_TXBUF_FULL_THR(reg32, val)   REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_TXBUF_FULL_THR, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_AN_BLOCK_ACK(reg32, val)     REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_AN_BLOCK_ACK, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_AN_TRANSPARENT(reg32, val)   REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_AN_TRANSPARENT, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_SEND_AN_ERROR_EN(reg32, val) REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_SEND_AN_ERROR_EN, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_CODE_SYNC_SET_EN(reg32, val) REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_EN, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_CODE_SYNC_SET_VAL(reg32, val) REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_VAL, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_REMOTE_FAULT_DIS(reg32, val) REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_REMOTE_FAULT_DIS, (reg32), (val))
#define sgmii_reg_an_13_SET_SGMII_IF_MODE_5_0(reg32, val)      REG_FLD_SET(sgmii_reg_an_13_FLD_SGMII_IF_MODE_5_0, (reg32), (val))

#define sgmii_reg_an_14_SET_SGMII_RESERVED_REG(reg32, val)     REG_FLD_SET(sgmii_reg_an_14_FLD_SGMII_RESERVED_REG, (reg32), (val))

#define sgmii_reg_an_15_SET_SGMII_FALSE_CARRIER_CNT(reg32, val) REG_FLD_SET(sgmii_reg_an_15_FLD_SGMII_FALSE_CARRIER_CNT, (reg32), (val))

#define sgmii_reg_an_16_SET_SGMII_RO_SW_RESET(reg32, val)      REG_FLD_SET(sgmii_reg_an_16_FLD_SGMII_RO_SW_RESET, (reg32), (val))
#define sgmii_reg_an_16_SET_SGMII_RO_AN_RESTART(reg32, val)    REG_FLD_SET(sgmii_reg_an_16_FLD_SGMII_RO_AN_RESTART, (reg32), (val))
#define sgmii_reg_an_16_SET_SGMII_RO_AN_EXPANSION_CLR(reg32, val) REG_FLD_SET(sgmii_reg_an_16_FLD_SGMII_RO_AN_EXPANSION_CLR, (reg32), (val))
#define sgmii_reg_an_16_SET_SGMII_RO_LINK_RST(reg32, val)      REG_FLD_SET(sgmii_reg_an_16_FLD_SGMII_RO_LINK_RST, (reg32), (val))
#define sgmii_reg_an_16_SET_SGMII_RO_NP_TX(reg32, val)         REG_FLD_SET(sgmii_reg_an_16_FLD_SGMII_RO_NP_TX, (reg32), (val))

#define sgmii_reg_an_17_SET_SGMII_1US_TIMER(reg32, val)        REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_1US_TIMER, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ERR_PROP_KO_SYNC_R(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SYNC_R, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ERR_PROP_KO_SIG_LOS_F(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SIG_LOS_F, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ERR_PROP_KI_SYNC_F(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SYNC_F, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ERR_PROP_KI_SIG_LOS_R(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SIG_LOS_R, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ELA_CSR_SEL(reg32, val)      REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ELA_CSR_SEL, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ELA_PMA_RXCK_SEL(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ELA_PMA_RXCK_SEL, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ELA_PMA_TXCK_SEL(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ELA_PMA_TXCK_SEL, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ELA_MAC_RXCK_SEL(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ELA_MAC_RXCK_SEL, (reg32), (val))
#define sgmii_reg_an_17_SET_SGMII_ELA_MAC_TXCK_SEL(reg32, val) REG_FLD_SET(sgmii_reg_an_17_FLD_SGMII_ELA_MAC_TXCK_SEL, (reg32), (val))

#define sgmii_reg_an_18_SET_SGMII_SW_RESET(reg32, val)         REG_FLD_SET(sgmii_reg_an_18_FLD_SGMII_SW_RESET, (reg32), (val))

#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_15_14(reg32, val)   REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_15_14, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_13_12(reg32, val)   REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_13_12, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_11(reg32, val)      REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_11, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_10(reg32, val)      REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_10, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_9_6(reg32, val)     REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_9_6, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_5(reg32, val)       REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_5, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_4(reg32, val)       REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_4, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_3(reg32, val)       REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_3, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_2(reg32, val)       REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_2, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_1(reg32, val)       REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_1, (reg32), (val))
#define sgmii_reg_an_19_SET_SGMII_LP_NP_RX_0(reg32, val)       REG_FLD_SET(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_0, (reg32), (val))

#define sgmii_reg_an_20_SET_SGMII_RX_FAULT_LATCH_CLR(reg32, val) REG_FLD_SET(sgmii_reg_an_20_FLD_SGMII_RX_FAULT_LATCH_CLR, (reg32), (val))
#define sgmii_reg_an_20_SET_SGMII_TX_FAULT_LATCH_CLR(reg32, val) REG_FLD_SET(sgmii_reg_an_20_FLD_SGMII_TX_FAULT_LATCH_CLR, (reg32), (val))

#define sgmii_reg_an_21_SET_SGMII_FALSE_CARRIER_CNT_READ_LATCH(reg32, val) REG_FLD_SET(sgmii_reg_an_21_FLD_SGMII_FALSE_CARRIER_CNT_READ_LATCH, (reg32), (val))
#define sgmii_reg_an_21_SET_SGMII_PCS_DEC_ERROR_CNT_READ_LATCH(reg32, val) REG_FLD_SET(sgmii_reg_an_21_FLD_SGMII_PCS_DEC_ERROR_CNT_READ_LATCH, (reg32), (val))
#define sgmii_reg_an_21_SET_SGMII_PCS_MODE(reg32, val)         REG_FLD_SET(sgmii_reg_an_21_FLD_SGMII_PCS_MODE, (reg32), (val))
#define sgmii_reg_an_21_SET_SGMII_PAT_GEN_TX_EN(reg32, val)    REG_FLD_SET(sgmii_reg_an_21_FLD_SGMII_PAT_GEN_TX_EN, (reg32), (val))

#define sgmii_reg_an_22_SET_SGMII_REGWR_NP_TX_15_8_EN(reg32, val) REG_FLD_SET(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_15_8_EN, (reg32), (val))
#define sgmii_reg_an_22_SET_SGMII_REGWR_NP_TX_7_3_EN(reg32, val) REG_FLD_SET(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_7_3_EN, (reg32), (val))
#define sgmii_reg_an_22_SET_SGMII_REGWR_NP_TX_2_0_EN(reg32, val) REG_FLD_SET(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_2_0_EN, (reg32), (val))

#define sgmii_reg_an_force_cl37_SET_rg_force_an_done(reg32, val) REG_FLD_SET(sgmii_reg_an_force_cl37_FLD_rg_force_an_done, (reg32), (val))

#define sgmii_reg_an0_VAL_SGMII_RESET_PHY(val)                 REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_RESET_PHY, (val))
#define sgmii_reg_an0_VAL_SGMII_LOOPBACK_ENA(val)              REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_LOOPBACK_ENA, (val))
#define sgmii_reg_an0_VAL_SGMII_SPEED_LSB(val)                 REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_SPEED_LSB, (val))
#define sgmii_reg_an0_VAL_SGMII_AN_ENABLE(val)                 REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_AN_ENABLE, (val))
#define sgmii_reg_an0_VAL_SGMII_POWERDOWN(val)                 REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_POWERDOWN, (val))
#define sgmii_reg_an0_VAL_SGMII_ISOLATE(val)                   REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_ISOLATE, (val))
#define sgmii_reg_an0_VAL_SGMII_AN_RESTART(val)                REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_AN_RESTART, (val))
#define sgmii_reg_an0_VAL_SGMII_DUPLEX(val)                    REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_DUPLEX, (val))
#define sgmii_reg_an0_VAL_SGMII_COL_TEST(val)                  REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_COL_TEST, (val))
#define sgmii_reg_an0_VAL_SGMII_SPEED_MSB(val)                 REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_SPEED_MSB, (val))
#define sgmii_reg_an0_VAL_SGMII_UNIDIR_ENA(val)                REG_FLD_VAL(sgmii_reg_an0_FLD_SGMII_UNIDIR_ENA, (val))

#define sgmii_reg_an_1_VAL_SGMII_LINK_RST(val)                 REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_LINK_RST, (val))
#define sgmii_reg_an_1_VAL_SGMII_AN_EXPANSION_CLR(val)         REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_AN_EXPANSION_CLR, (val))
#define sgmii_reg_an_1_VAL_SGMII_PCS_FAULT(val)                REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_PCS_FAULT, (val))
#define sgmii_reg_an_1_VAL_SGMII_UNIDIR_ABILITY(val)           REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_UNIDIR_ABILITY, (val))
#define sgmii_reg_an_1_VAL_SGMII_AN_COMPLETE(val)              REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_AN_COMPLETE, (val))
#define sgmii_reg_an_1_VAL_SGMII_REMOTE_FAULT(val)             REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_REMOTE_FAULT, (val))
#define sgmii_reg_an_1_VAL_SGMII_AN_ABILITY(val)               REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_AN_ABILITY, (val))
#define sgmii_reg_an_1_VAL_SGMII_LINK_STATUS(val)              REG_FLD_VAL(sgmii_reg_an_1_FLD_SGMII_LINK_STATUS, (val))

#define sgmii_reg_an_2_VAL_SGMII_PHY_IDENTIFIER_REG2(val)      REG_FLD_VAL(sgmii_reg_an_2_FLD_SGMII_PHY_IDENTIFIER_REG2, (val))

#define sgmii_reg_an_3_VAL_SGMII_PHY_IDENTIFIER_REG3(val)      REG_FLD_VAL(sgmii_reg_an_3_FLD_SGMII_PHY_IDENTIFIER_REG3, (val))

#define sgmii_reg_an_4_VAL_SGMII_DEV_ABILITY_15(val)           REG_FLD_VAL(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_15, (val))
#define sgmii_reg_an_4_VAL_SGMII_AN_ACK(val)                   REG_FLD_VAL(sgmii_reg_an_4_FLD_SGMII_AN_ACK, (val))
#define sgmii_reg_an_4_VAL_SGMII_DEV_ABILITY_13_4(val)         REG_FLD_VAL(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_13_4, (val))
#define sgmii_reg_an_4_VAL_SGMII_DEV_ABILITY_3(val)            REG_FLD_VAL(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_3, (val))
#define sgmii_reg_an_4_VAL_SGMII_DEV_ABILITY_2(val)            REG_FLD_VAL(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_2, (val))
#define sgmii_reg_an_4_VAL_SGMII_DEV_ABILITY_1(val)            REG_FLD_VAL(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_1, (val))
#define sgmii_reg_an_4_VAL_SGMII_DEV_ABILITY_0(val)            REG_FLD_VAL(sgmii_reg_an_4_FLD_SGMII_DEV_ABILITY_0, (val))

#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_15_11(val)    REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_15_11, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_10(val)       REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_10, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_9(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_9, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_8(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_8, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_7(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_7, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_6(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_6, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_5(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_5, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_4(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_4, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_3(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_3, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_2(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_2, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_1(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_1, (val))
#define sgmii_reg_an_5_VAL_SGMII_PARTNER_ABILITY_0(val)        REG_FLD_VAL(sgmii_reg_an_5_FLD_SGMII_PARTNER_ABILITY_0, (val))

#define sgmii_reg_an_6_VAL_SGMII_AN_EXPANSION_15(val)          REG_FLD_VAL(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_15, (val))
#define sgmii_reg_an_6_VAL_SGMII_AN_EXPANSION_14(val)          REG_FLD_VAL(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_14, (val))
#define sgmii_reg_an_6_VAL_SGMII_AN_EXPANSION_13(val)          REG_FLD_VAL(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_13, (val))
#define sgmii_reg_an_6_VAL_SGMII_AN_EXPANSION_12_0(val)        REG_FLD_VAL(sgmii_reg_an_6_FLD_SGMII_AN_EXPANSION_12_0, (val))

#define sgmii_reg_an_7_VAL_SGMII_NP_TX_15_8(val)               REG_FLD_VAL(sgmii_reg_an_7_FLD_SGMII_NP_TX_15_8, (val))
#define sgmii_reg_an_7_VAL_SGMII_NP_TX_7_3(val)                REG_FLD_VAL(sgmii_reg_an_7_FLD_SGMII_NP_TX_7_3, (val))
#define sgmii_reg_an_7_VAL_SGMII_NP_TX_2_0(val)                REG_FLD_VAL(sgmii_reg_an_7_FLD_SGMII_NP_TX_2_0, (val))

#define sgmii_reg_an_8_VAL_SGMII_TX_FAULT_LATCH(val)           REG_FLD_VAL(sgmii_reg_an_8_FLD_SGMII_TX_FAULT_LATCH, (val))
#define sgmii_reg_an_8_VAL_SGMII_RX_FAULT_LATCH(val)           REG_FLD_VAL(sgmii_reg_an_8_FLD_SGMII_RX_FAULT_LATCH, (val))

#define sgmii_reg_an_9_VAL_SGMII_SCRATCH(val)                  REG_FLD_VAL(sgmii_reg_an_9_FLD_SGMII_SCRATCH, (val))

#define sgmii_reg_an_10_VAL_SGMII_DEV_VERSION(val)             REG_FLD_VAL(sgmii_reg_an_10_FLD_SGMII_DEV_VERSION, (val))

#define sgmii_reg_an_11_VAL_SGMII_LINK_TIMER(val)              REG_FLD_VAL(sgmii_reg_an_11_FLD_SGMII_LINK_TIMER, (val))

#define sgmii_reg_an_12_VAL_SGMII_DEC_ERROR_CNT(val)           REG_FLD_VAL(sgmii_reg_an_12_FLD_SGMII_DEC_ERROR_CNT, (val))

#define sgmii_reg_an_13_VAL_SGMII_RXBUF_EMPTY_THR(val)         REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_RXBUF_EMPTY_THR, (val))
#define sgmii_reg_an_13_VAL_SGMII_RXBUF_FULL_THR(val)          REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_RXBUF_FULL_THR, (val))
#define sgmii_reg_an_13_VAL_SGMII_TXBUF_EMPTY_THR(val)         REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_TXBUF_EMPTY_THR, (val))
#define sgmii_reg_an_13_VAL_SGMII_TXBUF_FULL_THR(val)          REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_TXBUF_FULL_THR, (val))
#define sgmii_reg_an_13_VAL_SGMII_AN_BLOCK_ACK(val)            REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_AN_BLOCK_ACK, (val))
#define sgmii_reg_an_13_VAL_SGMII_AN_TRANSPARENT(val)          REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_AN_TRANSPARENT, (val))
#define sgmii_reg_an_13_VAL_SGMII_SEND_AN_ERROR_EN(val)        REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_SEND_AN_ERROR_EN, (val))
#define sgmii_reg_an_13_VAL_SGMII_CODE_SYNC_SET_EN(val)        REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_EN, (val))
#define sgmii_reg_an_13_VAL_SGMII_CODE_SYNC_SET_VAL(val)       REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_CODE_SYNC_SET_VAL, (val))
#define sgmii_reg_an_13_VAL_SGMII_REMOTE_FAULT_DIS(val)        REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_REMOTE_FAULT_DIS, (val))
#define sgmii_reg_an_13_VAL_SGMII_IF_MODE_5_0(val)             REG_FLD_VAL(sgmii_reg_an_13_FLD_SGMII_IF_MODE_5_0, (val))

#define sgmii_reg_an_14_VAL_SGMII_RESERVED_REG(val)            REG_FLD_VAL(sgmii_reg_an_14_FLD_SGMII_RESERVED_REG, (val))

#define sgmii_reg_an_15_VAL_SGMII_FALSE_CARRIER_CNT(val)       REG_FLD_VAL(sgmii_reg_an_15_FLD_SGMII_FALSE_CARRIER_CNT, (val))

#define sgmii_reg_an_16_VAL_SGMII_RO_SW_RESET(val)             REG_FLD_VAL(sgmii_reg_an_16_FLD_SGMII_RO_SW_RESET, (val))
#define sgmii_reg_an_16_VAL_SGMII_RO_AN_RESTART(val)           REG_FLD_VAL(sgmii_reg_an_16_FLD_SGMII_RO_AN_RESTART, (val))
#define sgmii_reg_an_16_VAL_SGMII_RO_AN_EXPANSION_CLR(val)     REG_FLD_VAL(sgmii_reg_an_16_FLD_SGMII_RO_AN_EXPANSION_CLR, (val))
#define sgmii_reg_an_16_VAL_SGMII_RO_LINK_RST(val)             REG_FLD_VAL(sgmii_reg_an_16_FLD_SGMII_RO_LINK_RST, (val))
#define sgmii_reg_an_16_VAL_SGMII_RO_NP_TX(val)                REG_FLD_VAL(sgmii_reg_an_16_FLD_SGMII_RO_NP_TX, (val))

#define sgmii_reg_an_17_VAL_SGMII_1US_TIMER(val)               REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_1US_TIMER, (val))
#define sgmii_reg_an_17_VAL_SGMII_ERR_PROP_KO_SYNC_R(val)      REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SYNC_R, (val))
#define sgmii_reg_an_17_VAL_SGMII_ERR_PROP_KO_SIG_LOS_F(val)   REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KO_SIG_LOS_F, (val))
#define sgmii_reg_an_17_VAL_SGMII_ERR_PROP_KI_SYNC_F(val)      REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SYNC_F, (val))
#define sgmii_reg_an_17_VAL_SGMII_ERR_PROP_KI_SIG_LOS_R(val)   REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ERR_PROP_KI_SIG_LOS_R, (val))
#define sgmii_reg_an_17_VAL_SGMII_ELA_CSR_SEL(val)             REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ELA_CSR_SEL, (val))
#define sgmii_reg_an_17_VAL_SGMII_ELA_PMA_RXCK_SEL(val)        REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ELA_PMA_RXCK_SEL, (val))
#define sgmii_reg_an_17_VAL_SGMII_ELA_PMA_TXCK_SEL(val)        REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ELA_PMA_TXCK_SEL, (val))
#define sgmii_reg_an_17_VAL_SGMII_ELA_MAC_RXCK_SEL(val)        REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ELA_MAC_RXCK_SEL, (val))
#define sgmii_reg_an_17_VAL_SGMII_ELA_MAC_TXCK_SEL(val)        REG_FLD_VAL(sgmii_reg_an_17_FLD_SGMII_ELA_MAC_TXCK_SEL, (val))

#define sgmii_reg_an_18_VAL_SGMII_SW_RESET(val)                REG_FLD_VAL(sgmii_reg_an_18_FLD_SGMII_SW_RESET, (val))

#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_15_14(val)          REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_15_14, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_13_12(val)          REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_13_12, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_11(val)             REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_11, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_10(val)             REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_10, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_9_6(val)            REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_9_6, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_5(val)              REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_5, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_4(val)              REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_4, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_3(val)              REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_3, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_2(val)              REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_2, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_1(val)              REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_1, (val))
#define sgmii_reg_an_19_VAL_SGMII_LP_NP_RX_0(val)              REG_FLD_VAL(sgmii_reg_an_19_FLD_SGMII_LP_NP_RX_0, (val))

#define sgmii_reg_an_20_VAL_SGMII_RX_FAULT_LATCH_CLR(val)      REG_FLD_VAL(sgmii_reg_an_20_FLD_SGMII_RX_FAULT_LATCH_CLR, (val))
#define sgmii_reg_an_20_VAL_SGMII_TX_FAULT_LATCH_CLR(val)      REG_FLD_VAL(sgmii_reg_an_20_FLD_SGMII_TX_FAULT_LATCH_CLR, (val))

#define sgmii_reg_an_21_VAL_SGMII_FALSE_CARRIER_CNT_READ_LATCH(val) REG_FLD_VAL(sgmii_reg_an_21_FLD_SGMII_FALSE_CARRIER_CNT_READ_LATCH, (val))
#define sgmii_reg_an_21_VAL_SGMII_PCS_DEC_ERROR_CNT_READ_LATCH(val) REG_FLD_VAL(sgmii_reg_an_21_FLD_SGMII_PCS_DEC_ERROR_CNT_READ_LATCH, (val))
#define sgmii_reg_an_21_VAL_SGMII_PCS_MODE(val)                REG_FLD_VAL(sgmii_reg_an_21_FLD_SGMII_PCS_MODE, (val))
#define sgmii_reg_an_21_VAL_SGMII_PAT_GEN_TX_EN(val)           REG_FLD_VAL(sgmii_reg_an_21_FLD_SGMII_PAT_GEN_TX_EN, (val))

#define sgmii_reg_an_22_VAL_SGMII_REGWR_NP_TX_15_8_EN(val)     REG_FLD_VAL(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_15_8_EN, (val))
#define sgmii_reg_an_22_VAL_SGMII_REGWR_NP_TX_7_3_EN(val)      REG_FLD_VAL(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_7_3_EN, (val))
#define sgmii_reg_an_22_VAL_SGMII_REGWR_NP_TX_2_0_EN(val)      REG_FLD_VAL(sgmii_reg_an_22_FLD_SGMII_REGWR_NP_TX_2_0_EN, (val))

#define sgmii_reg_an_force_cl37_VAL_rg_force_an_done(val)      REG_FLD_VAL(sgmii_reg_an_force_cl37_FLD_rg_force_an_done, (val))

#ifdef __cplusplus
}
#endif

#endif // __hsgmii_an_csr_REGS_H__

