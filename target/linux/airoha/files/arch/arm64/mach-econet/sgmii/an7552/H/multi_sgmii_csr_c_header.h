#ifndef __multi_sgmii_csr_REGS_H__
#define __multi_sgmii_csr_REGS_H__

//#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef REG_BASE_C_MODULE
// ----------------- multi_sgmii_csr Bit Field Definitions -------------------

#define PACKING
typedef unsigned int UINT32;
typedef unsigned int FIELD;

typedef struct
{
    

        FIELD rg_tx_5g_mode             : 1;
        FIELD rg_tx_gpii_rst            : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_tx_free_cnt_clr        : 1;
        FIELD rg_qsgmii_txclk_ena       : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_tx_data_rep2_sel       : 1;
        FIELD rg_tx_data_rep4_sel       : 1;
        FIELD rg_tx_data_re             : 1;
        FIELD rsv_11                    : 5;
        FIELD rg_qsgmii_mode            : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_sgmii_mode_force       : 2;
        FIELD rg_ssusb_hsgmii_sel       : 1;
        FIELD rsv_27                    : 1;
        FIELD rg_force_100fx_sigdet     : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_msg_tx_ctrl_0, *Pmulti_sgmii_csr_REG_msg_tx_ctrl_0;

typedef struct
{
    

        FIELD rg_qsgmii_p0_tx500_mac_div_ck_sel : 2;
        FIELD rg_qsgmii_p1_tx500_mac_div_ck_sel : 2;
        FIELD rg_qsgmii_p2_tx500_mac_div_ck_sel : 2;
        FIELD rg_qsgmii_p3_tx500_mac_div_ck_sel : 2;
        FIELD rg_p1_mac_tx_div_ck_reset_n : 1;
        FIELD rg_p2_mac_tx_div_ck_reset_n : 1;
        FIELD rg_p3_mac_tx_div_ck_reset_n : 1;
        FIELD rg_qsgmii_tx500_pcs_div_ck_sel : 1;
        FIELD rsv_12                    : 20;
    
    
} multi_sgmii_csr_REG_msg_tx_ctrl_1, *Pmulti_sgmii_csr_REG_msg_tx_ctrl_1;

typedef struct
{
    

        FIELD rg_qsgmii_20b_tx_hl_swap  : 1;
        FIELD rg_qsgmii_tx_rev          : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_qsgmii_tx_idle_swap_en : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_qsgmii_tx_kd_rev       : 1;
        FIELD rsv_9                     : 23;
    
    
} multi_sgmii_csr_REG_msg_tx_ctrl_2, *Pmulti_sgmii_csr_REG_msg_tx_ctrl_2;

typedef struct
{
    

        FIELD rg_efuse_tphy_rx_imp_sel  : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_efuse_tphy_tx_imp_sel  : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_efuse_tphy_iext_intr_ctrl : 6;
        FIELD rsv_22                    : 2;
        FIELD rg_efuse_tphy_auto_load_valid : 1;
        FIELD rsv_25                    : 7;
    
    
} multi_sgmii_csr_REG_efuse_ctrl_dummy, *Pmulti_sgmii_csr_REG_efuse_ctrl_dummy;

typedef struct
{
    

        FIELD rg_ssusb_ip_sw_rst        : 1;
        FIELD rg_hsgmii_mac_sw_rst_n    : 1;
        FIELD rsv_2                     : 30;
    
    
} multi_sgmii_csr_REG_rst_ctrl_0, *Pmulti_sgmii_csr_REG_rst_ctrl_0;

typedef struct
{
    

        FIELD rg_pcs_int_en_0           : 1;
        FIELD rg_pcs_int_en_1           : 1;
        FIELD rg_pcs_int_en_2           : 1;
        FIELD rg_pcs_int_en_3           : 1;
        FIELD rg_sigdet_up_int_en       : 1;
        FIELD rg_sigdet_down_int_en     : 1;
        FIELD rg_sgmii_link_up_int_en   : 1;
        FIELD rg_sgmii_link_down_int_en : 1;
        FIELD rg_cl22_wr_int_en         : 1;
        FIELD rg_cl22_reset_wr_int_en   : 1;
        FIELD rg_tx_eee_int_en          : 1;
        FIELD rg_rx_eee_int_en          : 1;
        FIELD rg_cl22_rg_phy_rst_cnt    : 4;
        FIELD rsv_16                    : 16;
    
    
} multi_sgmii_csr_REG_INTERRUPT_EN_0, *Pmulti_sgmii_csr_REG_INTERRUPT_EN_0;

typedef struct
{
    

        FIELD rg_force_linkup_p0        : 1;
        FIELD rg_force_linkdown_p0      : 1;
        FIELD rg_force_spd_mode_p0      : 1;
        FIELD rsv_3                     : 1;
        FIELD rg_link_mode_p0           : 2;
        FIELD rsv_6                     : 2;
        FIELD rg_force_linkup_p1        : 1;
        FIELD rg_force_linkdown_p1      : 1;
        FIELD rg_force_spd_mode_p1      : 1;
        FIELD rg_link_mode_p1           : 2;
        FIELD rsv_13                    : 3;
        FIELD rg_force_linkup_p2        : 1;
        FIELD rg_force_linkdown_p2      : 1;
        FIELD rg_force_spd_mode_p2      : 1;
        FIELD rg_link_mode_p2           : 2;
        FIELD rsv_21                    : 3;
        FIELD rg_force_linkup_p3        : 1;
        FIELD rg_force_linkdown_p3      : 1;
        FIELD rg_force_spd_mode_p3      : 1;
        FIELD rg_link_mode_p3           : 2;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_SGMII_STS_CTRL_0, *Pmulti_sgmii_csr_REG_SGMII_STS_CTRL_0;

typedef struct
{
    

        FIELD rg_tx_d_dummy             : 32;
    
    
} multi_sgmii_csr_REG_SGMII_STS_CTRL_1, *Pmulti_sgmii_csr_REG_SGMII_STS_CTRL_1;

typedef struct
{
    

        FIELD rg_tx_c_dummy             : 8;
        FIELD rsv_8                     : 24;
    
    
} multi_sgmii_csr_REG_SGMII_STS_CTRL_2, *Pmulti_sgmii_csr_REG_SGMII_STS_CTRL_2;

typedef struct
{
    

        FIELD rg_fpga_ctrl_31_0         : 32;
    
    
} multi_sgmii_csr_REG_FPGA_STS_CTRL, *Pmulti_sgmii_csr_REG_FPGA_STS_CTRL;

typedef struct
{
    

        FIELD rg_debug_ctrl             : 32;
    
    
} multi_sgmii_csr_REG_SYSYEM_DEBUG_CTRL, *Pmulti_sgmii_csr_REG_SYSYEM_DEBUG_CTRL;

typedef struct
{
    

        FIELD rg_fpga_ctrl_ben          : 32;
    
    
} multi_sgmii_csr_REG_FPGA_BEN_CTRL, *Pmulti_sgmii_csr_REG_FPGA_BEN_CTRL;

typedef struct
{
    

        FIELD rg_phy_id_0               : 16;
        FIELD rg_phy_id_1               : 16;
    
    
} multi_sgmii_csr_REG_CL22_SETTING_0, *Pmulti_sgmii_csr_REG_CL22_SETTING_0;

typedef struct
{
    

        FIELD rg_cl22reg0_4_0_en        : 5;
        FIELD rg_cl22reg0_5_en          : 1;
        FIELD rg_cl22reg0_6_en          : 1;
        FIELD rg_cl22reg0_7_en          : 1;
        FIELD rg_cl22reg0_8_en          : 1;
        FIELD rg_cl22reg0_9_en          : 1;
        FIELD rg_cl22reg0_10_en         : 1;
        FIELD rg_cl22reg0_11_en         : 1;
        FIELD rg_cl22reg0_12_en         : 1;
        FIELD rg_cl22reg0_13_en         : 1;
        FIELD rg_cl22reg0_14_en         : 1;
        FIELD rg_cl22reg0_15_en         : 1;
        FIELD rg_cl22_an_function_en    : 1;
        FIELD rsv_17                    : 3;
        FIELD rg_cl22_rg_status_clr     : 1;
        FIELD rsv_21                    : 11;
    
    
} multi_sgmii_csr_REG_CL22_SETTING_1, *Pmulti_sgmii_csr_REG_CL22_SETTING_1;

typedef struct
{
    

        FIELD rg_cl22_rg4_5_en          : 1;
        FIELD rg_cl22_rg4_6_en          : 1;
        FIELD rg_cl22_rg4_8_7_en        : 1;
        FIELD rg_cl22_rg4_13_12_en      : 1;
        FIELD rg_cl22_rg4_15_en         : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_cl22_rg7_10_0_en       : 1;
        FIELD rg_cl22_rg7_11_en         : 1;
        FIELD rg_cl22_rg7_12_en         : 1;
        FIELD rg_cl22_rg7_13_en         : 1;
        FIELD rg_cl22_rg7_14_en         : 1;
        FIELD rg_cl22_rg7_15_en         : 1;
        FIELD rsv_14                    : 2;
        FIELD rg_100base_t4             : 1;
        FIELD rg_100base_x_fdx          : 1;
        FIELD rg_100base_x_hdx          : 1;
        FIELD rg_10mbps_fdx             : 1;
        FIELD rg_10mbps_hdx             : 1;
        FIELD rg_100base_t2_fdx         : 1;
        FIELD rg_100base_t2_hdx         : 1;
        FIELD rg_ext_status             : 1;
        FIELD rg_mf_pream_sup           : 1;
        FIELD rg_jabber_det             : 1;
        FIELD rg_ext_cap                : 1;
        FIELD rg_uni_direct_en_reserve  : 1;
        FIELD rg_an_ability_en_reserve  : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_CL22_SETTING_2, *Pmulti_sgmii_csr_REG_CL22_SETTING_2;

typedef struct
{
    

        FIELD ro_cl22reg0_4_0           : 5;
        FIELD ro_cl22reg0_5             : 1;
        FIELD ro_cl22reg0_6             : 1;
        FIELD ro_cl22reg0_7             : 1;
        FIELD ro_cl22reg0_8             : 1;
        FIELD ro_cl22reg0_9             : 1;
        FIELD ro_cl22reg0_10            : 1;
        FIELD ro_cl22reg0_11            : 1;
        FIELD ro_cl22reg0_12            : 1;
        FIELD ro_cl22reg0_13            : 1;
        FIELD ro_cl22reg0_14            : 1;
        FIELD ro_cl22reg0_15            : 1;
        FIELD rsv_16                    : 16;
    
    
} multi_sgmii_csr_REG_CL22_STS_0, *Pmulti_sgmii_csr_REG_CL22_STS_0;

typedef struct
{
    

        FIELD rg_mac_t2r_fifo_rx_rst_n  : 1;
        FIELD rg_mac_t2r_fifo_tx_rst_n  : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_mac_t2r_fifo_rx_cnt_clr : 1;
        FIELD rg_mac_t2r_fifo_tx_cnt_clr : 1;
        FIELD rg_mac_t2r_fifo_ss_mode   : 1;
        FIELD rsv_7                     : 1;
        FIELD rg_mac_t2r_fifo_ss_hwm    : 8;
        FIELD rg_mac_t2r_fifo_ss_lwm    : 8;
        FIELD rg_mac_t2r_fifo_eee_test_mode : 1;
        FIELD rg_mac_t2r_fifo_lpi2idle_en : 1;
        FIELD rg_mac_t2r_fifo_rxer_mask : 1;
        FIELD rg_mac_t2r_fifo_odd_nibble_pream_deal_en : 1;
        FIELD rg_mac_t2r_fifo_undrun_intr_en : 1;
        FIELD rg_mac_t2r_fifo_ovrun_intr_en : 1;
        FIELD rsv_30                    : 2;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_CTRL_0, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_CTRL_0;

typedef struct
{
    

        FIELD rg_mac_t2r_fifo_min_ipg_num : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_mac_t2r_fifo_runt_ipg_num : 5;
        FIELD rsv_13                    : 19;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_CTRL_1, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_CTRL_1;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_rx_start_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_0, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_0;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_tx_start_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_1, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_1;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_rx_termi_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_2, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_2;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_tx_termi_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_3, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_3;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_ipg_del_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_4, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_4;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_ipg_ins_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_5, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_5;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_rx_runt_ipg_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_6, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_6;

typedef struct
{
    

        FIELD ro_mac_t2r_fifo_tx_runt_ipg_cnt : 32;
    
    
} multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_7, *Pmulti_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_7;

typedef struct
{
    

        FIELD rg_phy_r2t_fifo_rx_rst_n  : 1;
        FIELD rg_phy_r2t_fifo_tx_rst_n  : 1;
        FIELD rg_phy_r2t_fifo_en        : 1;
        FIELD rsv_3                     : 1;
        FIELD rg_phy_r2t_fifo_rx_cnt_clr : 1;
        FIELD rg_phy_r2t_fifo_tx_cnt_clr : 1;
        FIELD rg_phy_r2t_fifo_ss_mode   : 1;
        FIELD rsv_7                     : 1;
        FIELD rg_phy_r2t_fifo_ss_hwm    : 8;
        FIELD rg_phy_r2t_fifo_ss_lwm    : 8;
        FIELD rg_phy_r2t_fifo_eee_test_mode : 1;
        FIELD rg_phy_r2t_fifo_lpi2idle_en : 1;
        FIELD rg_phy_r2t_fifo_rxer_mask : 1;
        FIELD rg_phy_r2t_fifo_odd_nibble_pream_deal_en : 1;
        FIELD rg_phy_r2t_fifo_undrun_intr_en : 1;
        FIELD rg_phy_r2t_fifo_ovrun_intr_en : 1;
        FIELD rsv_30                    : 2;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_CTRL_0, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_CTRL_0;

typedef struct
{
    

        FIELD rg_phy_r2t_fifo_min_ipg_num : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_phy_r2t_fifo_runt_ipg_num : 5;
        FIELD rsv_13                    : 19;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_CTRL_1, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_CTRL_1;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_rx_start_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_0, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_0;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_tx_start_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_1, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_1;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_rx_termi_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_2, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_2;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_tx_termi_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_3, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_3;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_ipg_del_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_4, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_4;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_ipg_ins_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_5, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_5;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_rx_runt_ipg_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_6, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_6;

typedef struct
{
    

        FIELD ro_phy_r2t_fifo_tx_runt_ipg_cnt : 32;
    
    
} multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_7, *Pmulti_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_7;

typedef struct
{
    

        FIELD rg_force_txd_sel          : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_force_txc_sel          : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_force_txen_sel         : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_force_txer_sel         : 1;
        FIELD rsv_13                    : 19;
    
    
} multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_0, *Pmulti_sgmii_csr_REG_PHY_TX_FORCE_CTRL_0;

typedef struct
{
    

        FIELD rg_force_txd_h            : 32;
    
    
} multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_1, *Pmulti_sgmii_csr_REG_PHY_TX_FORCE_CTRL_1;

typedef struct
{
    

        FIELD rg_force_txd_l            : 32;
    
    
} multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_2, *Pmulti_sgmii_csr_REG_PHY_TX_FORCE_CTRL_2;

typedef struct
{
    

        FIELD rg_force_txc              : 8;
        FIELD rsv_8                     : 4;
        FIELD rg_force_txen             : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_force_txer             : 1;
        FIELD rsv_17                    : 15;
    
    
} multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_3, *Pmulti_sgmii_csr_REG_PHY_TX_FORCE_CTRL_3;

typedef struct
{
    

        FIELD rg_rx_clk_en              : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_rx_5g_mode             : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_rx_xgmii_mode          : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_sw_reset_rxck          : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_dec_err_1t             : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_qsgmii_port_shift      : 2;
        FIELD rsv_26                    : 2;
        FIELD rg_hsgmii_xfi_sel         : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_msg_rx_ctrl_0, *Pmulti_sgmii_csr_REG_msg_rx_ctrl_0;

typedef struct
{
    

        FIELD rg_tbi_20_tx_reverse_pma  : 1;
        FIELD rg_tbi_tx_reverse_10b_pma : 1;
        FIELD rg_tbi_20_rx_reverse_pma  : 1;
        FIELD rg_tbi_10_rx_reverse_pma  : 1;
        FIELD rg_tbi_20_rx_reverse_all  : 1;
        FIELD rg_tbi_20_tx_reverse_all  : 1;
        FIELD rsv_6                     : 26;
    
    
} multi_sgmii_csr_REG_multi_sgmii_reg_phya_65, *Pmulti_sgmii_csr_REG_multi_sgmii_reg_phya_65;

typedef struct
{
    

        FIELD rg_hsgmii_pcs_int         : 1;
        FIELD rg_100fx_int              : 1;
        FIELD rg_symbol_error_int       : 1;
        FIELD rg_false_carrier_int      : 1;
        FIELD rg_rxer100_int            : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_hsgmii_phy_int_force   : 1;
        FIELD rg_hsgmii_phy_int         : 1;
        FIELD rsv_10                    : 22;
    
    
} multi_sgmii_csr_REG_multi_sgmii_reg_interrupt_sel, *Pmulti_sgmii_csr_REG_multi_sgmii_reg_interrupt_sel;

typedef struct
{
    

        FIELD rg_hsgmii_probe_sel       : 1;
        FIELD rg_100fx_probe_sel        : 1;
        FIELD rg_hsgmii_clk_probe_sel   : 1;
        FIELD rg_dwn_shift_probe_sel    : 1;
        FIELD rg_hsgmii_probe_port_sel  : 2;
        FIELD rsv_6                     : 10;
        FIELD rg_probe_clk_sel          : 16;
    
    
} multi_sgmii_csr_REG_multi_sgmii_reg_probe_sel, *Pmulti_sgmii_csr_REG_multi_sgmii_reg_probe_sel;

typedef struct
{
    

        FIELD rg_hwtrap_100fx_en        : 2;
        FIELD rg_hwtrap_100fx_mux       : 1;
        FIELD rg_hwtrap_100fx_mux_en    : 1;
        FIELD rsv_4                     : 28;
    
    
} multi_sgmii_csr_REG_multi_sgmii_fpga_mode_control, *Pmulti_sgmii_csr_REG_multi_sgmii_fpga_mode_control;

typedef struct
{
    

        FIELD rg_hsgmii_async_fifo_wclear : 1;
        FIELD rg_hsgmii_async_fifo_rclear : 1;
        FIELD rg_hsgmii_async_fifo_r_threshold : 5;
        FIELD rg_hsgmii_async_fifo_w_threshold : 5;
        FIELD rg_hsgmii_async_fifo_full_cnt_clear : 1;
        FIELD rg_hsgmii_async_fifo_afull_cnt_clear : 1;
        FIELD rg_hsgmii_async_fifo_empty_cnt_clear : 1;
        FIELD rg_hsgmii_async_fifo_aempty_cnt_clear : 1;
        FIELD rg_hsgmii_async_fifo_read_enable_and : 1;
        FIELD rg_hsgmii_async_fifo_write_enable_and : 1;
        FIELD rg_hsgmii_async_fifo_read_enable_or : 1;
        FIELD rg_hsgmii_async_fifo_write_enable_or : 1;
        FIELD rsv_20                    : 12;
    
    
} multi_sgmii_csr_REG_multi_sgmii_async_fifo_control, *Pmulti_sgmii_csr_REG_multi_sgmii_async_fifo_control;

typedef struct
{
    

        FIELD ro_hsgmii_async_fifo_full : 1;
        FIELD ro_hsgmii_async_fifo_afull : 1;
        FIELD ro_hsgmii_async_fifo_empty : 1;
        FIELD ro_hsgmii_async_fifo_aempty : 1;
        FIELD ro_hsgmii_async_fifo_r_ready : 1;
        FIELD ro_hsgmii_async_fifo_w_ready : 1;
        FIELD ro_hsgmii_async_fifo_wfifo_cnt : 5;
        FIELD rsv_11                    : 5;
        FIELD ro_hsgmii_async_fifo_rfifo_cnt : 5;
        FIELD rsv_21                    : 11;
    
    
} multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo, *Pmulti_sgmii_csr_REG_multi_sgmii_ro_phya_afifo;

typedef struct
{
    

        FIELD ro_hsgmii_async_fifo_full_cnt : 32;
    
    
} multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_0, *Pmulti_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_0;

typedef struct
{
    

        FIELD ro_hsgmii_async_fifo_afull_cnt : 32;
    
    
} multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_1, *Pmulti_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_1;

typedef struct
{
    

        FIELD ro_hsgmii_async_fifo_empty_cnt : 32;
    
    
} multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_2, *Pmulti_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_2;

typedef struct
{
    

        FIELD ro_hsgmii_async_fifo_aempty_cnt : 32;
    
    
} multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_3, *Pmulti_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_3;

typedef struct
{
    

        FIELD rg_led0_en                : 1;
        FIELD rg_led1_en                : 1;
        FIELD rg_led2_en                : 1;
        FIELD rg_led3_en                : 1;
        FIELD rg_led0_pol               : 1;
        FIELD rg_led1_pol               : 1;
        FIELD rg_led2_pol               : 1;
        FIELD rg_led3_pol               : 1;
        FIELD rg_led_mode               : 2;
        FIELD rg_led_enhance            : 1;
        FIELD rsv_11                    : 1;
        FIELD rg_dwn_shift_blk_en       : 1;
        FIELD rg_dwn_shift_en           : 1;
        FIELD rg_led_timing_test        : 1;
        FIELD rsv_15                    : 17;
    
    
} multi_sgmii_csr_REG_msg_led_ctrl_0, *Pmulti_sgmii_csr_REG_msg_led_ctrl_0;

typedef struct
{
    

        FIELD rg_led_on_dur             : 16;
        FIELD rg_led_blk_dur            : 16;
    
    
} multi_sgmii_csr_REG_msg_led_ctrl_1, *Pmulti_sgmii_csr_REG_msg_led_ctrl_1;

typedef struct
{
    

        FIELD rg_led0_on_mask           : 7;
        FIELD rsv_7                     : 1;
        FIELD rg_led0_blk_mask          : 8;
        FIELD rg_led1_on_mask           : 7;
        FIELD rsv_23                    : 1;
        FIELD rg_led1_blk_mask          : 8;
    
    
} multi_sgmii_csr_REG_msg_led_ctrl_2, *Pmulti_sgmii_csr_REG_msg_led_ctrl_2;

typedef struct
{
    

        FIELD rg_led2_on_mask           : 7;
        FIELD rsv_7                     : 1;
        FIELD rg_led2_blk_mask          : 8;
        FIELD rg_led3_on_mask           : 7;
        FIELD rsv_23                    : 1;
        FIELD rg_led3_blk_mask          : 8;
    
    
} multi_sgmii_csr_REG_msg_led_ctrl_3, *Pmulti_sgmii_csr_REG_msg_led_ctrl_3;

typedef struct
{
    

        FIELD ro_led_on_event           : 7;
        FIELD rsv_7                     : 1;
        FIELD ro_led_blk_event          : 8;
        FIELD rsv_16                    : 16;
    
    
} multi_sgmii_csr_REG_msg_led_sts_0, *Pmulti_sgmii_csr_REG_msg_led_sts_0;

typedef struct
{
    

        FIELD ro_k28_1_cnt              : 32;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_0, *Pmulti_sgmii_csr_REG_msg_rx_sts_0;

typedef struct
{
    

        FIELD ro_port0_k28_1_cnt        : 32;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_1, *Pmulti_sgmii_csr_REG_msg_rx_sts_1;

typedef struct
{
    

        FIELD ro_port1_k28_1_cnt        : 32;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_2, *Pmulti_sgmii_csr_REG_msg_rx_sts_2;

typedef struct
{
    

        FIELD ro_port2_k28_1_cnt        : 32;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_3, *Pmulti_sgmii_csr_REG_msg_rx_sts_3;

typedef struct
{
    

        FIELD ro_port3_k28_1_cnt        : 32;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_4, *Pmulti_sgmii_csr_REG_msg_rx_sts_4;

typedef struct
{
    

        FIELD ro_dp5_p                  : 1;
        FIELD rsv_1                     : 3;
        FIELD ro_dp5_n                  : 1;
        FIELD rsv_5                     : 3;
        FIELD ro_dp3_p                  : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_dp3_n                  : 1;
        FIELD rsv_13                    : 3;
        FIELD ro_rd_out_5b_pos          : 1;
        FIELD rsv_17                    : 3;
        FIELD ro_rd_out_5b_neg          : 1;
        FIELD rsv_21                    : 3;
        FIELD ro_rd_out_3b_pos          : 1;
        FIELD rsv_25                    : 3;
        FIELD ro_rd_out_3b_neg          : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_5, *Pmulti_sgmii_csr_REG_msg_rx_sts_5;

typedef struct
{
    

        FIELD ro_except5_p              : 1;
        FIELD rsv_1                     : 3;
        FIELD ro_except5_n              : 1;
        FIELD rsv_5                     : 3;
        FIELD ro_except_pos             : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_except_neg             : 1;
        FIELD rsv_13                    : 3;
        FIELD ro_except_posi            : 1;
        FIELD rsv_17                    : 3;
        FIELD ro_except_negi            : 1;
        FIELD rsv_21                    : 3;
        FIELD ro_except_err             : 1;
        FIELD rsv_25                    : 3;
        FIELD ro_except_okn             : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_6, *Pmulti_sgmii_csr_REG_msg_rx_sts_6;

typedef struct
{
    

        FIELD ro_data_out_3b_index      : 5;
        FIELD rsv_5                     : 3;
        FIELD ro_rd_out_3b_posx         : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_rd_out_3b_negx         : 1;
        FIELD rsv_13                    : 3;
        FIELD ro_rd_out_5b_posx         : 1;
        FIELD rsv_17                    : 3;
        FIELD ro_rd_out_5b_negx         : 1;
        FIELD rsv_21                    : 3;
        FIELD ro_rd_out_5b              : 1;
        FIELD rsv_25                    : 3;
        FIELD ro_carrier_reg            : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_7, *Pmulti_sgmii_csr_REG_msg_rx_sts_7;

typedef struct
{
    

        FIELD ro_error_kout_5b          : 1;
        FIELD rsv_1                     : 3;
        FIELD ro_error_kout_5b_pos      : 1;
        FIELD rsv_5                     : 3;
        FIELD ro_error_kout_5b_neg      : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_error_3b_sc            : 1;
        FIELD rsv_13                    : 3;
        FIELD ro_error_3b_sc_pos        : 1;
        FIELD rsv_17                    : 3;
        FIELD ro_error_3b_sc_neg        : 1;
        FIELD rsv_21                    : 11;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_8, *Pmulti_sgmii_csr_REG_msg_rx_sts_8;

typedef struct
{
    

        FIELD ro_error_3b               : 1;
        FIELD rsv_1                     : 3;
        FIELD ro_error_5b               : 1;
        FIELD rsv_5                     : 3;
        FIELD ro_error_5b_pos           : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_error_5b_neg           : 1;
        FIELD rsv_13                    : 19;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_9, *Pmulti_sgmii_csr_REG_msg_rx_sts_9;

typedef struct
{
    

        FIELD ro_data_out_5b            : 5;
        FIELD rsv_5                     : 11;
        FIELD ro_data_out_3b            : 3;
        FIELD rsv_19                    : 13;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_10, *Pmulti_sgmii_csr_REG_msg_rx_sts_10;

typedef struct
{
    

        FIELD ro_kout_5b_neg            : 1;
        FIELD rsv_1                     : 3;
        FIELD ro_kout_5b_pos            : 1;
        FIELD rsv_5                     : 3;
        FIELD ro_kout_3b_neg            : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_kout_3b_pos            : 1;
        FIELD rsv_13                    : 3;
        FIELD ro_char_err               : 1;
        FIELD rsv_17                    : 3;
        FIELD ro_disp_err               : 1;
        FIELD rsv_21                    : 11;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_11, *Pmulti_sgmii_csr_REG_msg_rx_sts_11;

typedef struct
{
    

        FIELD ro_align_det              : 4;
        FIELD rsv_4                     : 4;
        FIELD ro_rd_out                 : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_sync_int               : 1;
        FIELD rsv_13                    : 3;
        FIELD ro_port0_sync_lost        : 1;
        FIELD rsv_17                    : 3;
        FIELD ro_port1_sync_lost        : 1;
        FIELD rsv_21                    : 3;
        FIELD ro_port2_sync_lost        : 1;
        FIELD rsv_25                    : 3;
        FIELD ro_port3_sync_lost        : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_12, *Pmulti_sgmii_csr_REG_msg_rx_sts_12;

typedef struct
{
    

        FIELD ro_data_align             : 10;
        FIELD rsv_10                    : 22;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_13, *Pmulti_sgmii_csr_REG_msg_rx_sts_13;

typedef struct
{
    

        FIELD ro_par                    : 19;
        FIELD rsv_19                    : 13;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_14, *Pmulti_sgmii_csr_REG_msg_rx_sts_14;

typedef struct
{
    

        FIELD ro_speed_sts_p0           : 3;
        FIELD ro_lnk_sts_p0             : 1;
        FIELD ro_speed_sts_p1           : 3;
        FIELD ro_lnk_sts_p1             : 1;
        FIELD ro_speed_sts_p2           : 3;
        FIELD ro_lnk_sts_p2             : 1;
        FIELD ro_speed_sts_p3           : 3;
        FIELD ro_lnk_sts_p3             : 1;
        FIELD rsv_16                    : 16;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_15, *Pmulti_sgmii_csr_REG_msg_rx_sts_15;

typedef struct
{
    

        FIELD ro_p0_link_up_latch       : 1;
        FIELD ro_p0_link_down_latch     : 1;
        FIELD ro_p1_link_up_latch       : 1;
        FIELD ro_p1_link_down_latch     : 1;
        FIELD ro_p2_link_up_latch       : 1;
        FIELD ro_p2_link_down_latch     : 1;
        FIELD ro_p3_link_up_latch       : 1;
        FIELD ro_p3_link_down_latch     : 1;
        FIELD ro_sig_detect_up_latch    : 1;
        FIELD ro_sig_detect_down_latch  : 1;
        FIELD ro_p0_tx_eee_flag_latch   : 1;
        FIELD ro_p1_tx_eee_flag_latch   : 1;
        FIELD ro_p2_tx_eee_flag_latch   : 1;
        FIELD ro_p3_tx_eee_flag_latch   : 1;
        FIELD ro_p0_rx_eee_flag_latch   : 1;
        FIELD ro_p1_rx_eee_flag_latch   : 1;
        FIELD ro_p2_rx_eee_flag_latch   : 1;
        FIELD ro_p3_rx_eee_flag_latch   : 1;
        FIELD rsv_18                    : 14;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_16, *Pmulti_sgmii_csr_REG_msg_rx_sts_16;

typedef struct
{
    

        FIELD rg_p0_link_up_int_clr     : 1;
        FIELD rg_p0_link_down_int_clr   : 1;
        FIELD rg_p1_link_up_int_clr     : 1;
        FIELD rg_p1_link_down_int_clr   : 1;
        FIELD rg_p2_link_up_int_clr     : 1;
        FIELD rg_p2_link_down_int_clr   : 1;
        FIELD rg_p3_link_up_int_clr     : 1;
        FIELD rg_p3_link_down_int_clr   : 1;
        FIELD rg_sig_detect_up_int_clr  : 1;
        FIELD rg_sig_detect_down_int_clr : 1;
        FIELD rg_p0_tx_eee_int_clr      : 1;
        FIELD rg_p1_tx_eee_int_clr      : 1;
        FIELD rg_p2_tx_eee_int_clr      : 1;
        FIELD rg_p3_tx_eee_int_clr      : 1;
        FIELD rg_p0_rx_eee_int_clr      : 1;
        FIELD rg_p1_rx_eee_int_clr      : 1;
        FIELD rg_p2_rx_eee_int_clr      : 1;
        FIELD rg_p3_rx_eee_int_clr      : 1;
        FIELD rsv_18                    : 14;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_17, *Pmulti_sgmii_csr_REG_msg_rx_sts_17;

typedef struct
{
    

        FIELD p0_sgmii_is_1g            : 1;
        FIELD p0_sgmii_is_100           : 1;
        FIELD p0_sgmii_is_10            : 1;
        FIELD rsv_3                     : 1;
        FIELD p1_sgmii_is_1g            : 1;
        FIELD p1_sgmii_is_100           : 1;
        FIELD p1_sgmii_is_10            : 1;
        FIELD rsv_7                     : 1;
        FIELD p2_sgmii_is_1g            : 1;
        FIELD p2_sgmii_is_100           : 1;
        FIELD p2_sgmii_is_10            : 1;
        FIELD rsv_11                    : 1;
        FIELD p3_sgmii_is_1g            : 1;
        FIELD p3_sgmii_is_100           : 1;
        FIELD p3_sgmii_is_10            : 1;
        FIELD rsv_15                    : 17;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_18, *Pmulti_sgmii_csr_REG_msg_rx_sts_18;

typedef struct
{
    

        FIELD p0_lp_ability             : 16;
        FIELD p1_lp_ability             : 16;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_19, *Pmulti_sgmii_csr_REG_msg_rx_sts_19;

typedef struct
{
    

        FIELD p2_lp_ability             : 16;
        FIELD p3_lp_ability             : 16;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_20, *Pmulti_sgmii_csr_REG_msg_rx_sts_20;

typedef struct
{
    

        FIELD ro_cl22_wrtie_flag        : 1;
        FIELD ro_cl22_reset_wrtie_flag  : 1;
        FIELD rsv_2                     : 30;
    
    
} multi_sgmii_csr_REG_cl22_status_0, *Pmulti_sgmii_csr_REG_cl22_status_0;

typedef struct
{
    

        FIELD rg_cl22_wrtie_flag_clr    : 1;
        FIELD rg_cl22_reset_wrtie_flag_clr : 1;
        FIELD rsv_2                     : 30;
    
    
} multi_sgmii_csr_REG_cl22_status_1, *Pmulti_sgmii_csr_REG_cl22_status_1;

typedef struct
{
    

        FIELD rg_tx_eee_flag_lev_p0_clr : 1;
        FIELD rg_tx_eee_flag_lev_p1_clr : 1;
        FIELD rg_tx_eee_flag_lev_p2_clr : 1;
        FIELD rg_tx_eee_flag_lev_p3_clr : 1;
        FIELD rg_rx_eee_flag_lev_p0_clr : 1;
        FIELD rg_rx_eee_flag_lev_p1_clr : 1;
        FIELD rg_rx_eee_flag_lev_p2_clr : 1;
        FIELD rg_rx_eee_flag_lev_p3_clr : 1;
        FIELD rsv_8                     : 24;
    
    
} multi_sgmii_csr_REG_msg_rx_sts_21, *Pmulti_sgmii_csr_REG_msg_rx_sts_21;

typedef struct
{
    

        FIELD rg_force_port0_cnt_sel    : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_force_port1_cnt_sel    : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_force_port2_cnt_sel    : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_force_port3_cnt_sel    : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_force_port0_cnt        : 2;
        FIELD rsv_18                    : 2;
        FIELD rg_force_port1_cnt        : 2;
        FIELD rsv_22                    : 2;
        FIELD rg_force_port2_cnt        : 2;
        FIELD rsv_26                    : 2;
        FIELD rg_force_port3_cnt        : 2;
        FIELD rsv_30                    : 2;
    
    
} multi_sgmii_csr_REG_msg_rx_ctrl_1, *Pmulti_sgmii_csr_REG_msg_rx_ctrl_1;

typedef struct
{
    

        FIELD rg_force_port0_sync_sel   : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_force_port1_sync_sel   : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_force_port2_sync_sel   : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_force_port3_sync_sel   : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_force_port0_sync       : 1;
        FIELD rsv_17                    : 3;
        FIELD rg_force_port1_sync       : 1;
        FIELD rsv_21                    : 3;
        FIELD rg_force_port2_sync       : 1;
        FIELD rsv_25                    : 3;
        FIELD rg_force_port3_sync       : 1;
        FIELD rsv_29                    : 3;
    
    
} multi_sgmii_csr_REG_msg_rx_ctrl_2, *Pmulti_sgmii_csr_REG_msg_rx_ctrl_2;

typedef struct
{
    

        FIELD rg_port0_k28_1_cnt_clr    : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_port1_k28_1_cnt_clr    : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_port2_k28_1_cnt_clr    : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_port3_k28_1_cnt_clr    : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_k28_1_cnt_clr          : 1;
        FIELD rsv_17                    : 15;
    
    
} multi_sgmii_csr_REG_msg_rx_ctrl_3, *Pmulti_sgmii_csr_REG_msg_rx_ctrl_3;

typedef struct
{
    

        FIELD rg_qsgmii_p0_rx500_mac_div_ck_sel : 2;
        FIELD rg_qsgmii_p1_rx500_mac_div_ck_sel : 2;
        FIELD rg_qsgmii_p2_rx500_mac_div_ck_sel : 2;
        FIELD rg_qsgmii_p3_rx500_mac_div_ck_sel : 2;
        FIELD rg_p1_mac_rx_div_ck_reset_n : 1;
        FIELD rg_p2_mac_rx_div_ck_reset_n : 1;
        FIELD rg_p3_mac_rx_div_ck_reset_n : 1;
        FIELD rg_qsgmii_rx500_pcs_div_ck_sel : 1;
        FIELD rsv_12                    : 20;
    
    
} multi_sgmii_csr_REG_msg_rx_ctrl_4, *Pmulti_sgmii_csr_REG_msg_rx_ctrl_4;

typedef struct
{
    

        FIELD rg_hg_col_test_mode       : 1;
        FIELD rg_hg_crs_test_mode       : 1;
        FIELD rg_fx_col_test_mode       : 1;
        FIELD rg_fx_crs_test_mode       : 1;
        FIELD rg_fx_col_sel             : 1;
        FIELD rg_fx_crs_sel             : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_col_is_fx_mode         : 1;
        FIELD rg_crs_is_fx_mode         : 1;
        FIELD rsv_10                    : 6;
        FIELD rg_hg_col_test            : 4;
        FIELD rg_hg_crs_test            : 4;
        FIELD rg_fx_col_test            : 3;
        FIELD rsv_27                    : 1;
        FIELD rg_fx_crs_test            : 3;
        FIELD rsv_31                    : 1;
    
    
} multi_sgmii_csr_REG_msg_rx_ctrl_5, *Pmulti_sgmii_csr_REG_msg_rx_ctrl_5;

typedef struct
{
    

        FIELD rg_dpx_sts_p0             : 1;
        FIELD rg_rxfc_sts_p0            : 1;
        FIELD rg_txfc_sts_p0            : 1;
        FIELD rg_eee100_sts_p0          : 1;
        FIELD rg_eee1g_sts_p0           : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_dpx_sts_p1             : 1;
        FIELD rg_rxfc_sts_p1            : 1;
        FIELD rg_txfc_sts_p1            : 1;
        FIELD rg_eee100_sts_p1          : 1;
        FIELD rg_eee1g_sts_p1           : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_dpx_sts_p2             : 1;
        FIELD rg_rxfc_sts_p2            : 1;
        FIELD rg_txfc_sts_p2            : 1;
        FIELD rg_eee100_sts_p2          : 1;
        FIELD rg_eee1g_sts_p2           : 1;
        FIELD rsv_21                    : 3;
        FIELD rg_dpx_sts_p3             : 1;
        FIELD rg_rxfc_sts_p3            : 1;
        FIELD rg_txfc_sts_p3            : 1;
        FIELD rg_eee100_sts_p3          : 1;
        FIELD rg_eee1g_sts_p3           : 1;
        FIELD rsv_29                    : 2;
        FIELD rg_eee2p5g_sts            : 1;
    
    
} multi_sgmii_csr_REG_msg_rx_lik_sts_0, *Pmulti_sgmii_csr_REG_msg_rx_lik_sts_0;

typedef struct
{
    

        FIELD rg_lp_an_cap_p0           : 1;
        FIELD rg_lp_an_cap_p1           : 1;
        FIELD rg_lp_an_cap_p2           : 1;
        FIELD rg_lp_an_cap_p3           : 1;
        FIELD rg_pause_sts_p0           : 1;
        FIELD rg_pause_sts_p1           : 1;
        FIELD rg_pause_sts_p2           : 1;
        FIELD rg_pause_sts_p3           : 1;
        FIELD rsv_8                     : 24;
    
    
} multi_sgmii_csr_REG_msg_rx_lik_sts_1, *Pmulti_sgmii_csr_REG_msg_rx_lik_sts_1;

typedef struct
{
    

        FIELD rg_dpx_an_bypass_p0       : 1;
        FIELD rg_dpx_an_bypass_p1       : 1;
        FIELD rg_dpx_an_bypass_p2       : 1;
        FIELD rg_dpx_an_bypass_p3       : 1;
        FIELD rg_txfc_an_bypass_p0      : 1;
        FIELD rg_txfc_an_bypass_p1      : 1;
        FIELD rg_txfc_an_bypass_p2      : 1;
        FIELD rg_txfc_an_bypass_p3      : 1;
        FIELD rg_rxfc_an_bypass_p0      : 1;
        FIELD rg_rxfc_an_bypass_p1      : 1;
        FIELD rg_rxfc_an_bypass_p2      : 1;
        FIELD rg_rxfc_an_bypass_p3      : 1;
        FIELD rsv_12                    : 20;
    
    
} multi_sgmii_csr_REG_msg_rx_lik_sts_2, *Pmulti_sgmii_csr_REG_msg_rx_lik_sts_2;

typedef struct
{
    

        FIELD rg_force_rxd_sel          : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_force_rxc_sel          : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_force_rxdv_sel         : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_force_rxer_sel         : 1;
        FIELD rsv_13                    : 19;
    
    
} multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_0, *Pmulti_sgmii_csr_REG_PHY_RX_FORCE_CTRL_0;

typedef struct
{
    

        FIELD rg_force_rxd_h            : 32;
    
    
} multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_1, *Pmulti_sgmii_csr_REG_PHY_RX_FORCE_CTRL_1;

typedef struct
{
    

        FIELD rg_force_rxd_l            : 32;
    
    
} multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_2, *Pmulti_sgmii_csr_REG_PHY_RX_FORCE_CTRL_2;

typedef struct
{
    

        FIELD rg_force_rxc              : 8;
        FIELD rsv_8                     : 4;
        FIELD rg_force_rxdv             : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_force_rxer             : 1;
        FIELD rsv_17                    : 15;
    
    
} multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_3, *Pmulti_sgmii_csr_REG_PHY_RX_FORCE_CTRL_3;

typedef struct
{
    

        FIELD ro_dump_000               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_0, *Pmulti_sgmii_csr_REG_Debug_REGISTER_0;

typedef struct
{
    

        FIELD ro_dump_001               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_1, *Pmulti_sgmii_csr_REG_Debug_REGISTER_1;

typedef struct
{
    

        FIELD ro_dump_002               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_2, *Pmulti_sgmii_csr_REG_Debug_REGISTER_2;

typedef struct
{
    

        FIELD ro_dump_003               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_3, *Pmulti_sgmii_csr_REG_Debug_REGISTER_3;

typedef struct
{
    

        FIELD ro_dump_004               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_4, *Pmulti_sgmii_csr_REG_Debug_REGISTER_4;

typedef struct
{
    

        FIELD ro_dump_005               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_5, *Pmulti_sgmii_csr_REG_Debug_REGISTER_5;

typedef struct
{
    

        FIELD ro_dump_006               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_6, *Pmulti_sgmii_csr_REG_Debug_REGISTER_6;

typedef struct
{
    

        FIELD ro_dump_007               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_7, *Pmulti_sgmii_csr_REG_Debug_REGISTER_7;

typedef struct
{
    

        FIELD ro_dump_008               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_8, *Pmulti_sgmii_csr_REG_Debug_REGISTER_8;

typedef struct
{
    

        FIELD ro_dump_009               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_9, *Pmulti_sgmii_csr_REG_Debug_REGISTER_9;

typedef struct
{
    

        FIELD ro_dump_010               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_10, *Pmulti_sgmii_csr_REG_Debug_REGISTER_10;

typedef struct
{
    

        FIELD ro_dump_011               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_11, *Pmulti_sgmii_csr_REG_Debug_REGISTER_11;

typedef struct
{
    

        FIELD ro_dump_012               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_12, *Pmulti_sgmii_csr_REG_Debug_REGISTER_12;

typedef struct
{
    

        FIELD ro_dump_013               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_13, *Pmulti_sgmii_csr_REG_Debug_REGISTER_13;

typedef struct
{
    

        FIELD ro_dump_014               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_14, *Pmulti_sgmii_csr_REG_Debug_REGISTER_14;

typedef struct
{
    

        FIELD ro_dump_015               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_15, *Pmulti_sgmii_csr_REG_Debug_REGISTER_15;

typedef struct
{
    

        FIELD ro_dump_016               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_16, *Pmulti_sgmii_csr_REG_Debug_REGISTER_16;

typedef struct
{
    

        FIELD ro_dump_017               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_17, *Pmulti_sgmii_csr_REG_Debug_REGISTER_17;

typedef struct
{
    

        FIELD ro_dump_018               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_18, *Pmulti_sgmii_csr_REG_Debug_REGISTER_18;

typedef struct
{
    

        FIELD ro_dump_019               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_19, *Pmulti_sgmii_csr_REG_Debug_REGISTER_19;

typedef struct
{
    

        FIELD ro_dump_020               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_20, *Pmulti_sgmii_csr_REG_Debug_REGISTER_20;

typedef struct
{
    

        FIELD ro_dump_021               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_21, *Pmulti_sgmii_csr_REG_Debug_REGISTER_21;

typedef struct
{
    

        FIELD ro_dump_022               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_22, *Pmulti_sgmii_csr_REG_Debug_REGISTER_22;

typedef struct
{
    

        FIELD ro_dump_023               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_23, *Pmulti_sgmii_csr_REG_Debug_REGISTER_23;

typedef struct
{
    

        FIELD ro_dump_024               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_24, *Pmulti_sgmii_csr_REG_Debug_REGISTER_24;

typedef struct
{
    

        FIELD ro_dump_025               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_25, *Pmulti_sgmii_csr_REG_Debug_REGISTER_25;

typedef struct
{
    

        FIELD ro_dump_026               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_26, *Pmulti_sgmii_csr_REG_Debug_REGISTER_26;

typedef struct
{
    

        FIELD ro_dump_027               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_27, *Pmulti_sgmii_csr_REG_Debug_REGISTER_27;

typedef struct
{
    

        FIELD ro_dump_028               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_28, *Pmulti_sgmii_csr_REG_Debug_REGISTER_28;

typedef struct
{
    

        FIELD ro_dump_029               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_29, *Pmulti_sgmii_csr_REG_Debug_REGISTER_29;

typedef struct
{
    

        FIELD ro_dump_030               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_30, *Pmulti_sgmii_csr_REG_Debug_REGISTER_30;

typedef struct
{
    

        FIELD ro_dump_031               : 32;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_31, *Pmulti_sgmii_csr_REG_Debug_REGISTER_31;

typedef struct
{
    

        FIELD rg_dump_clr               : 1;
        FIELD rg_dump_inv               : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_dump_sel               : 3;
        FIELD rsv_7                     : 1;
        FIELD rg_force_dump             : 1;
        FIELD rsv_9                     : 23;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_CTRL_0, *Pmulti_sgmii_csr_REG_Debug_REGISTER_CTRL_0;

typedef struct
{
    

        FIELD ro_dump_marker            : 8;
        FIELD rsv_8                     : 24;
    
    
} multi_sgmii_csr_REG_Debug_REGISTER_DUMP_CNT, *Pmulti_sgmii_csr_REG_Debug_REGISTER_DUMP_CNT;

// ----------------- multi_sgmii_csr  Grouping Definitions -------------------
// ----------------- multi_sgmii_csr Register Definition -------------------
#if 0
typedef volatile 
{
    multi_sgmii_csr_REG_msg_tx_ctrl_0 msg_tx_ctrl_0;  // 4000
    multi_sgmii_csr_REG_msg_tx_ctrl_1 msg_tx_ctrl_1;  // 4004
    multi_sgmii_csr_REG_msg_tx_ctrl_2 msg_tx_ctrl_2;  // 4008
    multi_sgmii_csr_REG_efuse_ctrl_dummy efuse_ctrl_dummy; // 400C
    multi_sgmii_csr_REG_rst_ctrl_0  rst_ctrl_0;       // 4010
    multi_sgmii_csr_REG_INTERRUPT_EN_0 INTERRUPT_EN_0; // 4014
    multi_sgmii_csr_REG_SGMII_STS_CTRL_0 SGMII_STS_CTRL_0; // 4018
    multi_sgmii_csr_REG_SGMII_STS_CTRL_1 SGMII_STS_CTRL_1; // 401C
    multi_sgmii_csr_REG_SGMII_STS_CTRL_2 SGMII_STS_CTRL_2; // 4020
    multi_sgmii_csr_REG_FPGA_STS_CTRL FPGA_STS_CTRL;  // 4024
    multi_sgmii_csr_REG_SYSYEM_DEBUG_CTRL SYSYEM_DEBUG_CTRL; // 4028
    UINT32                          rsv_402C;         // 402C
    multi_sgmii_csr_REG_FPGA_BEN_CTRL FPGA_BEN_CTRL;  // 4030
    multi_sgmii_csr_REG_CL22_SETTING_0 CL22_SETTING_0; // 4034
    multi_sgmii_csr_REG_CL22_SETTING_1 CL22_SETTING_1; // 4038
    multi_sgmii_csr_REG_CL22_SETTING_2 CL22_SETTING_2; // 403C
    multi_sgmii_csr_REG_CL22_STS_0  CL22_STS_0;       // 4040
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_CTRL_0 MAC_T2R_LPBK_FIFO_CTRL_0; // 4044
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_CTRL_1 MAC_T2R_LPBK_FIFO_CTRL_1; // 4048
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_0 MAC_T2R_LPBK_FIFO_STS_0; // 404C
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_1 MAC_T2R_LPBK_FIFO_STS_1; // 4050
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_2 MAC_T2R_LPBK_FIFO_STS_2; // 4054
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_3 MAC_T2R_LPBK_FIFO_STS_3; // 4058
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_4 MAC_T2R_LPBK_FIFO_STS_4; // 405C
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_5 MAC_T2R_LPBK_FIFO_STS_5; // 4060
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_6 MAC_T2R_LPBK_FIFO_STS_6; // 4064
    multi_sgmii_csr_REG_MAC_T2R_LPBK_FIFO_STS_7 MAC_T2R_LPBK_FIFO_STS_7; // 4068
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_CTRL_0 PHY_R2T_LPBK_FIFO_CTRL_0; // 406C
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_CTRL_1 PHY_R2T_LPBK_FIFO_CTRL_1; // 4070
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_0 PHY_R2T_LPBK_FIFO_STS_0; // 4074
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_1 PHY_R2T_LPBK_FIFO_STS_1; // 4078
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_2 PHY_R2T_LPBK_FIFO_STS_2; // 407C
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_3 PHY_R2T_LPBK_FIFO_STS_3; // 4080
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_4 PHY_R2T_LPBK_FIFO_STS_4; // 4084
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_5 PHY_R2T_LPBK_FIFO_STS_5; // 4088
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_6 PHY_R2T_LPBK_FIFO_STS_6; // 408C
    multi_sgmii_csr_REG_PHY_R2T_LPBK_FIFO_STS_7 PHY_R2T_LPBK_FIFO_STS_7; // 4090
    multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_0 PHY_TX_FORCE_CTRL_0; // 4094
    multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_1 PHY_TX_FORCE_CTRL_1; // 4098
    multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_2 PHY_TX_FORCE_CTRL_2; // 409C
    multi_sgmii_csr_REG_PHY_TX_FORCE_CTRL_3 PHY_TX_FORCE_CTRL_3; // 40A0
    UINT32                          rsv_40A4[23];     // 40A4..40FC
    multi_sgmii_csr_REG_msg_rx_ctrl_0 msg_rx_ctrl_0;  // 4100
    multi_sgmii_csr_REG_multi_sgmii_reg_phya_65 multi_sgmii_reg_phya_65; // 4104
    UINT32                          rsv_4108[17];     // 4108..4148
    multi_sgmii_csr_REG_multi_sgmii_reg_interrupt_sel multi_sgmii_reg_interrupt_sel; // 414C
    multi_sgmii_csr_REG_multi_sgmii_reg_probe_sel multi_sgmii_reg_probe_sel; // 4150
    multi_sgmii_csr_REG_multi_sgmii_fpga_mode_control multi_sgmii_fpga_mode_control; // 4154
    multi_sgmii_csr_REG_multi_sgmii_async_fifo_control multi_sgmii_async_fifo_control; // 4158
    UINT32                          rsv_415C[68];     // 415C..4268
    multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo multi_sgmii_ro_phya_afifo; // 426C
    multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_0 multi_sgmii_ro_phya_afifo_cnt_0; // 4270
    multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_1 multi_sgmii_ro_phya_afifo_cnt_1; // 4274
    multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_2 multi_sgmii_ro_phya_afifo_cnt_2; // 4278
    multi_sgmii_csr_REG_multi_sgmii_ro_phya_afifo_cnt_3 multi_sgmii_ro_phya_afifo_cnt_3; // 427C
    UINT32                          rsv_4280[32];     // 4280..42FC
    multi_sgmii_csr_REG_msg_led_ctrl_0 msg_led_ctrl_0; // 4300
    multi_sgmii_csr_REG_msg_led_ctrl_1 msg_led_ctrl_1; // 4304
    multi_sgmii_csr_REG_msg_led_ctrl_2 msg_led_ctrl_2; // 4308
    multi_sgmii_csr_REG_msg_led_ctrl_3 msg_led_ctrl_3; // 430C
    multi_sgmii_csr_REG_msg_led_sts_0 msg_led_sts_0;  // 4310
    UINT32                          rsv_4314[59];     // 4314..43FC
    multi_sgmii_csr_REG_msg_rx_sts_0 msg_rx_sts_0;    // 4400
    multi_sgmii_csr_REG_msg_rx_sts_1 msg_rx_sts_1;    // 4404
    multi_sgmii_csr_REG_msg_rx_sts_2 msg_rx_sts_2;    // 4408
    multi_sgmii_csr_REG_msg_rx_sts_3 msg_rx_sts_3;    // 440C
    multi_sgmii_csr_REG_msg_rx_sts_4 msg_rx_sts_4;    // 4410
    multi_sgmii_csr_REG_msg_rx_sts_5 msg_rx_sts_5;    // 4414
    multi_sgmii_csr_REG_msg_rx_sts_6 msg_rx_sts_6;    // 4418
    multi_sgmii_csr_REG_msg_rx_sts_7 msg_rx_sts_7;    // 441C
    multi_sgmii_csr_REG_msg_rx_sts_8 msg_rx_sts_8;    // 4420
    multi_sgmii_csr_REG_msg_rx_sts_9 msg_rx_sts_9;    // 4424
    multi_sgmii_csr_REG_msg_rx_sts_10 msg_rx_sts_10;  // 4428
    multi_sgmii_csr_REG_msg_rx_sts_11 msg_rx_sts_11;  // 442C
    multi_sgmii_csr_REG_msg_rx_sts_12 msg_rx_sts_12;  // 4430
    multi_sgmii_csr_REG_msg_rx_sts_13 msg_rx_sts_13;  // 4434
    multi_sgmii_csr_REG_msg_rx_sts_14 msg_rx_sts_14;  // 4438
    multi_sgmii_csr_REG_msg_rx_sts_15 msg_rx_sts_15;  // 443C
    multi_sgmii_csr_REG_msg_rx_sts_16 msg_rx_sts_16;  // 4440
    multi_sgmii_csr_REG_msg_rx_sts_17 msg_rx_sts_17;  // 4444
    multi_sgmii_csr_REG_msg_rx_sts_18 msg_rx_sts_18;  // 4448
    multi_sgmii_csr_REG_msg_rx_sts_19 msg_rx_sts_19;  // 444C
    multi_sgmii_csr_REG_msg_rx_sts_20 msg_rx_sts_20;  // 4450
    multi_sgmii_csr_REG_cl22_status_0 cl22_status_0;  // 4454
    multi_sgmii_csr_REG_cl22_status_1 cl22_status_1;  // 4458
    multi_sgmii_csr_REG_msg_rx_sts_21 msg_rx_sts_21;  // 445C
    UINT32                          rsv_4460[40];     // 4460..44FC
    multi_sgmii_csr_REG_msg_rx_ctrl_1 msg_rx_ctrl_1;  // 4500
    multi_sgmii_csr_REG_msg_rx_ctrl_2 msg_rx_ctrl_2;  // 4504
    multi_sgmii_csr_REG_msg_rx_ctrl_3 msg_rx_ctrl_3;  // 4508
    multi_sgmii_csr_REG_msg_rx_ctrl_4 msg_rx_ctrl_4;  // 450C
    multi_sgmii_csr_REG_msg_rx_ctrl_5 msg_rx_ctrl_5;  // 4510
    multi_sgmii_csr_REG_msg_rx_lik_sts_0 msg_rx_lik_sts_0; // 4514
    multi_sgmii_csr_REG_msg_rx_lik_sts_1 msg_rx_lik_sts_1; // 4518
    multi_sgmii_csr_REG_msg_rx_lik_sts_2 msg_rx_lik_sts_2; // 451C
    multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_0 PHY_RX_FORCE_CTRL_0; // 4520
    multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_1 PHY_RX_FORCE_CTRL_1; // 4524
    multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_2 PHY_RX_FORCE_CTRL_2; // 4528
    multi_sgmii_csr_REG_PHY_RX_FORCE_CTRL_3 PHY_RX_FORCE_CTRL_3; // 452C
    UINT32                          rsv_4530[52];     // 4530..45FC
    multi_sgmii_csr_REG_Debug_REGISTER_0 Debug_REGISTER_0; // 4600
    multi_sgmii_csr_REG_Debug_REGISTER_1 Debug_REGISTER_1; // 4604
    multi_sgmii_csr_REG_Debug_REGISTER_2 Debug_REGISTER_2; // 4608
    multi_sgmii_csr_REG_Debug_REGISTER_3 Debug_REGISTER_3; // 460C
    multi_sgmii_csr_REG_Debug_REGISTER_4 Debug_REGISTER_4; // 4610
    multi_sgmii_csr_REG_Debug_REGISTER_5 Debug_REGISTER_5; // 4614
    multi_sgmii_csr_REG_Debug_REGISTER_6 Debug_REGISTER_6; // 4618
    multi_sgmii_csr_REG_Debug_REGISTER_7 Debug_REGISTER_7; // 461C
    multi_sgmii_csr_REG_Debug_REGISTER_8 Debug_REGISTER_8; // 4620
    multi_sgmii_csr_REG_Debug_REGISTER_9 Debug_REGISTER_9; // 4624
    multi_sgmii_csr_REG_Debug_REGISTER_10 Debug_REGISTER_10; // 4628
    multi_sgmii_csr_REG_Debug_REGISTER_11 Debug_REGISTER_11; // 462C
    multi_sgmii_csr_REG_Debug_REGISTER_12 Debug_REGISTER_12; // 4630
    multi_sgmii_csr_REG_Debug_REGISTER_13 Debug_REGISTER_13; // 4634
    multi_sgmii_csr_REG_Debug_REGISTER_14 Debug_REGISTER_14; // 4638
    multi_sgmii_csr_REG_Debug_REGISTER_15 Debug_REGISTER_15; // 463C
    multi_sgmii_csr_REG_Debug_REGISTER_16 Debug_REGISTER_16; // 4640
    multi_sgmii_csr_REG_Debug_REGISTER_17 Debug_REGISTER_17; // 4644
    multi_sgmii_csr_REG_Debug_REGISTER_18 Debug_REGISTER_18; // 4648
    multi_sgmii_csr_REG_Debug_REGISTER_19 Debug_REGISTER_19; // 464C
    multi_sgmii_csr_REG_Debug_REGISTER_20 Debug_REGISTER_20; // 4650
    multi_sgmii_csr_REG_Debug_REGISTER_21 Debug_REGISTER_21; // 4654
    multi_sgmii_csr_REG_Debug_REGISTER_22 Debug_REGISTER_22; // 4658
    multi_sgmii_csr_REG_Debug_REGISTER_23 Debug_REGISTER_23; // 465C
    multi_sgmii_csr_REG_Debug_REGISTER_24 Debug_REGISTER_24; // 4660
    multi_sgmii_csr_REG_Debug_REGISTER_25 Debug_REGISTER_25; // 4664
    multi_sgmii_csr_REG_Debug_REGISTER_26 Debug_REGISTER_26; // 4668
    multi_sgmii_csr_REG_Debug_REGISTER_27 Debug_REGISTER_27; // 466C
    multi_sgmii_csr_REG_Debug_REGISTER_28 Debug_REGISTER_28; // 4670
    multi_sgmii_csr_REG_Debug_REGISTER_29 Debug_REGISTER_29; // 4674
    multi_sgmii_csr_REG_Debug_REGISTER_30 Debug_REGISTER_30; // 4678
    multi_sgmii_csr_REG_Debug_REGISTER_31 Debug_REGISTER_31; // 467C
    multi_sgmii_csr_REG_Debug_REGISTER_CTRL_0 Debug_REGISTER_CTRL_0; // 4680
    multi_sgmii_csr_REG_Debug_REGISTER_DUMP_CNT Debug_REGISTER_DUMP_CNT; // 4684
}multi_sgmii_csr_REGS, *Pmulti_sgmii_csr_REGS;

// ---------- multi_sgmii_csr Enum Definitions      ----------
// ---------- multi_sgmii_csr C Macro Definitions   ----------
extern Pmulti_sgmii_csr_REGS g_multi_sgmii_csr_BASE;

#define multi_sgmii_csr_BASE                                   (g_multi_sgmii_csr_BASE)
#endif

#define _multi_sgmii_csr_msg_tx_ctrl_0		0x4000
#define _multi_sgmii_csr_msg_tx_ctrl_1		0x4004
#define _multi_sgmii_csr_msg_tx_ctrl_2		0x4008
#define _multi_sgmii_csr_efuse_ctrl_dummy		0x400C
#define _multi_sgmii_csr_rst_ctrl_0		0x4010
#define _multi_sgmii_csr_INTERRUPT_EN_0		0x4014
#define _multi_sgmii_csr_SGMII_STS_CTRL_0		0x4018
#define _multi_sgmii_csr_SGMII_STS_CTRL_1		0x401C
#define _multi_sgmii_csr_SGMII_STS_CTRL_2		0x4020
#define _multi_sgmii_csr_FPGA_STS_CTRL		0x4024
#define _multi_sgmii_csr_SYSYEM_DEBUG_CTRL		0x4028
#define _multi_sgmii_csr_FPGA_BEN_CTRL		0x4030
#define _multi_sgmii_csr_CL22_SETTING_0		0x4034
#define _multi_sgmii_csr_CL22_SETTING_1		0x4038
#define _multi_sgmii_csr_CL22_SETTING_2		0x403C
#define _multi_sgmii_csr_CL22_STS_0		0x4040
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_CTRL_0		0x4044
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_CTRL_1		0x4048
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_0		0x404C
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_1		0x4050
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_2		0x4054
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_3		0x4058
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_4		0x405C
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_5		0x4060
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_6		0x4064
#define _multi_sgmii_csr_MAC_T2R_LPBK_FIFO_STS_7		0x4068
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_CTRL_0		0x406C
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_CTRL_1		0x4070
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_0		0x4074
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_1		0x4078
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_2		0x407C
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_3		0x4080
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_4		0x4084
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_5		0x4088
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_6		0x408C
#define _multi_sgmii_csr_PHY_R2T_LPBK_FIFO_STS_7		0x4090
#define _multi_sgmii_csr_PHY_TX_FORCE_CTRL_0		0x4094
#define _multi_sgmii_csr_PHY_TX_FORCE_CTRL_1		0x4098
#define _multi_sgmii_csr_PHY_TX_FORCE_CTRL_2		0x409C
#define _multi_sgmii_csr_PHY_TX_FORCE_CTRL_3		0x40A0
#define _multi_sgmii_csr_msg_rx_ctrl_0		0x4100
#define _multi_sgmii_csr_multi_sgmii_reg_phya_65		0x4104
#define _multi_sgmii_csr_multi_sgmii_reg_interrupt_sel		0x414C
#define _multi_sgmii_csr_multi_sgmii_reg_probe_sel		0x4150
#define _multi_sgmii_csr_multi_sgmii_fpga_mode_control		0x4154
#define _multi_sgmii_csr_multi_sgmii_async_fifo_control		0x4158
#define _multi_sgmii_csr_multi_sgmii_ro_phya_afifo		0x426C
#define _multi_sgmii_csr_multi_sgmii_ro_phya_afifo_cnt_0		0x4270
#define _multi_sgmii_csr_multi_sgmii_ro_phya_afifo_cnt_1		0x4274
#define _multi_sgmii_csr_multi_sgmii_ro_phya_afifo_cnt_2		0x4278
#define _multi_sgmii_csr_multi_sgmii_ro_phya_afifo_cnt_3		0x427C
#define _multi_sgmii_csr_msg_led_ctrl_0		0x4300
#define _multi_sgmii_csr_msg_led_ctrl_1		0x4304
#define _multi_sgmii_csr_msg_led_ctrl_2		0x4308
#define _multi_sgmii_csr_msg_led_ctrl_3		0x430C
#define _multi_sgmii_csr_msg_led_sts_0		0x4310
#define _multi_sgmii_csr_msg_rx_sts_0		0x4400
#define _multi_sgmii_csr_msg_rx_sts_1		0x4404
#define _multi_sgmii_csr_msg_rx_sts_2		0x4408
#define _multi_sgmii_csr_msg_rx_sts_3		0x440C
#define _multi_sgmii_csr_msg_rx_sts_4		0x4410
#define _multi_sgmii_csr_msg_rx_sts_5		0x4414
#define _multi_sgmii_csr_msg_rx_sts_6		0x4418
#define _multi_sgmii_csr_msg_rx_sts_7		0x441C
#define _multi_sgmii_csr_msg_rx_sts_8		0x4420
#define _multi_sgmii_csr_msg_rx_sts_9		0x4424
#define _multi_sgmii_csr_msg_rx_sts_10		0x4428
#define _multi_sgmii_csr_msg_rx_sts_11		0x442C
#define _multi_sgmii_csr_msg_rx_sts_12		0x4430
#define _multi_sgmii_csr_msg_rx_sts_13		0x4434
#define _multi_sgmii_csr_msg_rx_sts_14		0x4438
#define _multi_sgmii_csr_msg_rx_sts_15		0x443C
#define _multi_sgmii_csr_msg_rx_sts_16		0x4440
#define _multi_sgmii_csr_msg_rx_sts_17		0x4444
#define _multi_sgmii_csr_msg_rx_sts_18		0x4448
#define _multi_sgmii_csr_msg_rx_sts_19		0x444C
#define _multi_sgmii_csr_msg_rx_sts_20		0x4450
#define _multi_sgmii_csr_cl22_status_0		0x4454
#define _multi_sgmii_csr_cl22_status_1		0x4458
#define _multi_sgmii_csr_msg_rx_sts_21		0x445C
#define _multi_sgmii_csr_msg_rx_ctrl_1		0x4500
#define _multi_sgmii_csr_msg_rx_ctrl_2		0x4504
#define _multi_sgmii_csr_msg_rx_ctrl_3		0x4508
#define _multi_sgmii_csr_msg_rx_ctrl_4		0x450C
#define _multi_sgmii_csr_msg_rx_ctrl_5		0x4510
#define _multi_sgmii_csr_msg_rx_lik_sts_0		0x4514
#define _multi_sgmii_csr_msg_rx_lik_sts_1		0x4518
#define _multi_sgmii_csr_msg_rx_lik_sts_2		0x451C
#define _multi_sgmii_csr_PHY_RX_FORCE_CTRL_0		0x4520
#define _multi_sgmii_csr_PHY_RX_FORCE_CTRL_1		0x4524
#define _multi_sgmii_csr_PHY_RX_FORCE_CTRL_2		0x4528
#define _multi_sgmii_csr_PHY_RX_FORCE_CTRL_3		0x452C
#define _multi_sgmii_csr_Debug_REGISTER_0		0x4600
#define _multi_sgmii_csr_Debug_REGISTER_1		0x4604
#define _multi_sgmii_csr_Debug_REGISTER_2		0x4608
#define _multi_sgmii_csr_Debug_REGISTER_3		0x460C
#define _multi_sgmii_csr_Debug_REGISTER_4		0x4610
#define _multi_sgmii_csr_Debug_REGISTER_5		0x4614
#define _multi_sgmii_csr_Debug_REGISTER_6		0x4618
#define _multi_sgmii_csr_Debug_REGISTER_7		0x461C
#define _multi_sgmii_csr_Debug_REGISTER_8		0x4620
#define _multi_sgmii_csr_Debug_REGISTER_9		0x4624
#define _multi_sgmii_csr_Debug_REGISTER_10		0x4628
#define _multi_sgmii_csr_Debug_REGISTER_11		0x462C
#define _multi_sgmii_csr_Debug_REGISTER_12		0x4630
#define _multi_sgmii_csr_Debug_REGISTER_13		0x4634
#define _multi_sgmii_csr_Debug_REGISTER_14		0x4638
#define _multi_sgmii_csr_Debug_REGISTER_15		0x463C
#define _multi_sgmii_csr_Debug_REGISTER_16		0x4640
#define _multi_sgmii_csr_Debug_REGISTER_17		0x4644
#define _multi_sgmii_csr_Debug_REGISTER_18		0x4648
#define _multi_sgmii_csr_Debug_REGISTER_19		0x464C
#define _multi_sgmii_csr_Debug_REGISTER_20		0x4650
#define _multi_sgmii_csr_Debug_REGISTER_21		0x4654
#define _multi_sgmii_csr_Debug_REGISTER_22		0x4658
#define _multi_sgmii_csr_Debug_REGISTER_23		0x465C
#define _multi_sgmii_csr_Debug_REGISTER_24		0x4660
#define _multi_sgmii_csr_Debug_REGISTER_25		0x4664
#define _multi_sgmii_csr_Debug_REGISTER_26		0x4668
#define _multi_sgmii_csr_Debug_REGISTER_27		0x466C
#define _multi_sgmii_csr_Debug_REGISTER_28		0x4670
#define _multi_sgmii_csr_Debug_REGISTER_29		0x4674
#define _multi_sgmii_csr_Debug_REGISTER_30		0x4678
#define _multi_sgmii_csr_Debug_REGISTER_31		0x467C
#define _multi_sgmii_csr_Debug_REGISTER_CTRL_0		0x4680
#define _multi_sgmii_csr_Debug_REGISTER_DUMP_CNT		0x4684

#endif


#define msg_tx_ctrl_0_FLD_rg_force_100fx_sigdet                REG_FLD(1, 28)
#define msg_tx_ctrl_0_FLD_rg_ssusb_hsgmii_sel                  REG_FLD(1, 26)
#define msg_tx_ctrl_0_FLD_rg_sgmii_mode_force                  REG_FLD(2, 24)
#define msg_tx_ctrl_0_FLD_rg_qsgmii_mode                       REG_FLD(1, 16)
#define msg_tx_ctrl_0_FLD_rg_tx_data_re                        REG_FLD(1, 10)
#define msg_tx_ctrl_0_FLD_rg_tx_data_rep4_sel                  REG_FLD(1, 9)
#define msg_tx_ctrl_0_FLD_rg_tx_data_rep2_sel                  REG_FLD(1, 8)
#define msg_tx_ctrl_0_FLD_rg_qsgmii_txclk_ena                  REG_FLD(1, 5)
#define msg_tx_ctrl_0_FLD_rg_tx_free_cnt_clr                   REG_FLD(1, 4)
#define msg_tx_ctrl_0_FLD_rg_tx_gpii_rst                       REG_FLD(1, 1)
#define msg_tx_ctrl_0_FLD_rg_tx_5g_mode                        REG_FLD(1, 0)

#define msg_tx_ctrl_1_FLD_rg_qsgmii_tx500_pcs_div_ck_sel       REG_FLD(1, 11)
#define msg_tx_ctrl_1_FLD_rg_p3_mac_tx_div_ck_reset_n          REG_FLD(1, 10)
#define msg_tx_ctrl_1_FLD_rg_p2_mac_tx_div_ck_reset_n          REG_FLD(1, 9)
#define msg_tx_ctrl_1_FLD_rg_p1_mac_tx_div_ck_reset_n          REG_FLD(1, 8)
#define msg_tx_ctrl_1_FLD_rg_qsgmii_p3_tx500_mac_div_ck_sel    REG_FLD(2, 6)
#define msg_tx_ctrl_1_FLD_rg_qsgmii_p2_tx500_mac_div_ck_sel    REG_FLD(2, 4)
#define msg_tx_ctrl_1_FLD_rg_qsgmii_p1_tx500_mac_div_ck_sel    REG_FLD(2, 2)
#define msg_tx_ctrl_1_FLD_rg_qsgmii_p0_tx500_mac_div_ck_sel    REG_FLD(2, 0)

#define msg_tx_ctrl_2_FLD_rg_qsgmii_tx_kd_rev                  REG_FLD(1, 8)
#define msg_tx_ctrl_2_FLD_rg_qsgmii_tx_idle_swap_en            REG_FLD(1, 4)
#define msg_tx_ctrl_2_FLD_rg_qsgmii_tx_rev                     REG_FLD(1, 1)
#define msg_tx_ctrl_2_FLD_rg_qsgmii_20b_tx_hl_swap             REG_FLD(1, 0)

#define efuse_ctrl_dummy_FLD_rg_efuse_tphy_auto_load_valid     REG_FLD(1, 24)
#define efuse_ctrl_dummy_FLD_rg_efuse_tphy_iext_intr_ctrl      REG_FLD(6, 16)
#define efuse_ctrl_dummy_FLD_rg_efuse_tphy_tx_imp_sel          REG_FLD(5, 8)
#define efuse_ctrl_dummy_FLD_rg_efuse_tphy_rx_imp_sel          REG_FLD(5, 0)

#define rst_ctrl_0_FLD_rg_hsgmii_mac_sw_rst_n                  REG_FLD(1, 1)
#define rst_ctrl_0_FLD_rg_ssusb_ip_sw_rst                      REG_FLD(1, 0)

#define INTERRUPT_EN_0_FLD_rg_cl22_rg_phy_rst_cnt              REG_FLD(4, 12)
#define INTERRUPT_EN_0_FLD_rg_rx_eee_int_en                    REG_FLD(1, 11)
#define INTERRUPT_EN_0_FLD_rg_tx_eee_int_en                    REG_FLD(1, 10)
#define INTERRUPT_EN_0_FLD_rg_cl22_reset_wr_int_en             REG_FLD(1, 9)
#define INTERRUPT_EN_0_FLD_rg_cl22_wr_int_en                   REG_FLD(1, 8)
#define INTERRUPT_EN_0_FLD_rg_sgmii_link_down_int_en           REG_FLD(1, 7)
#define INTERRUPT_EN_0_FLD_rg_sgmii_link_up_int_en             REG_FLD(1, 6)
#define INTERRUPT_EN_0_FLD_rg_sigdet_down_int_en               REG_FLD(1, 5)
#define INTERRUPT_EN_0_FLD_rg_sigdet_up_int_en                 REG_FLD(1, 4)
#define INTERRUPT_EN_0_FLD_rg_pcs_int_en_3                     REG_FLD(1, 3)
#define INTERRUPT_EN_0_FLD_rg_pcs_int_en_2                     REG_FLD(1, 2)
#define INTERRUPT_EN_0_FLD_rg_pcs_int_en_1                     REG_FLD(1, 1)
#define INTERRUPT_EN_0_FLD_rg_pcs_int_en_0                     REG_FLD(1, 0)

#define SGMII_STS_CTRL_0_FLD_rg_link_mode_p3                   REG_FLD(2, 27)
#define SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p3              REG_FLD(1, 26)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p3              REG_FLD(1, 25)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkup_p3                REG_FLD(1, 24)
#define SGMII_STS_CTRL_0_FLD_rg_link_mode_p2                   REG_FLD(2, 19)
#define SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p2              REG_FLD(1, 18)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p2              REG_FLD(1, 17)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkup_p2                REG_FLD(1, 16)
#define SGMII_STS_CTRL_0_FLD_rg_link_mode_p1                   REG_FLD(2, 11)
#define SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p1              REG_FLD(1, 10)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p1              REG_FLD(1, 9)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkup_p1                REG_FLD(1, 8)
#define SGMII_STS_CTRL_0_FLD_rg_link_mode_p0                   REG_FLD(2, 4)
#define SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p0              REG_FLD(1, 2)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p0              REG_FLD(1, 1)
#define SGMII_STS_CTRL_0_FLD_rg_force_linkup_p0                REG_FLD(1, 0)

#define SGMII_STS_CTRL_1_FLD_rg_tx_d_dummy                     REG_FLD(32, 0)

#define SGMII_STS_CTRL_2_FLD_rg_tx_c_dummy                     REG_FLD(8, 0)

#define FPGA_STS_CTRL_FLD_rg_fpga_ctrl_31_0                    REG_FLD(32, 0)

#define SYSYEM_DEBUG_CTRL_FLD_rg_debug_ctrl                    REG_FLD(32, 0)

#define FPGA_BEN_CTRL_FLD_rg_fpga_ctrl_ben                     REG_FLD(32, 0)

#define CL22_SETTING_0_FLD_rg_phy_id_1                         REG_FLD(16, 16)
#define CL22_SETTING_0_FLD_rg_phy_id_0                         REG_FLD(16, 0)

#define CL22_SETTING_1_FLD_rg_cl22_rg_status_clr               REG_FLD(1, 20)
#define CL22_SETTING_1_FLD_rg_cl22_an_function_en              REG_FLD(1, 16)
#define CL22_SETTING_1_FLD_rg_cl22reg0_15_en                   REG_FLD(1, 15)
#define CL22_SETTING_1_FLD_rg_cl22reg0_14_en                   REG_FLD(1, 14)
#define CL22_SETTING_1_FLD_rg_cl22reg0_13_en                   REG_FLD(1, 13)
#define CL22_SETTING_1_FLD_rg_cl22reg0_12_en                   REG_FLD(1, 12)
#define CL22_SETTING_1_FLD_rg_cl22reg0_11_en                   REG_FLD(1, 11)
#define CL22_SETTING_1_FLD_rg_cl22reg0_10_en                   REG_FLD(1, 10)
#define CL22_SETTING_1_FLD_rg_cl22reg0_9_en                    REG_FLD(1, 9)
#define CL22_SETTING_1_FLD_rg_cl22reg0_8_en                    REG_FLD(1, 8)
#define CL22_SETTING_1_FLD_rg_cl22reg0_7_en                    REG_FLD(1, 7)
#define CL22_SETTING_1_FLD_rg_cl22reg0_6_en                    REG_FLD(1, 6)
#define CL22_SETTING_1_FLD_rg_cl22reg0_5_en                    REG_FLD(1, 5)
#define CL22_SETTING_1_FLD_rg_cl22reg0_4_0_en                  REG_FLD(5, 0)

#define CL22_SETTING_2_FLD_rg_an_ability_en_reserve            REG_FLD(1, 28)
#define CL22_SETTING_2_FLD_rg_uni_direct_en_reserve            REG_FLD(1, 27)
#define CL22_SETTING_2_FLD_rg_ext_cap                          REG_FLD(1, 26)
#define CL22_SETTING_2_FLD_rg_jabber_det                       REG_FLD(1, 25)
#define CL22_SETTING_2_FLD_rg_mf_pream_sup                     REG_FLD(1, 24)
#define CL22_SETTING_2_FLD_rg_ext_status                       REG_FLD(1, 23)
#define CL22_SETTING_2_FLD_rg_100base_t2_hdx                   REG_FLD(1, 22)
#define CL22_SETTING_2_FLD_rg_100base_t2_fdx                   REG_FLD(1, 21)
#define CL22_SETTING_2_FLD_rg_10mbps_hdx                       REG_FLD(1, 20)
#define CL22_SETTING_2_FLD_rg_10mbps_fdx                       REG_FLD(1, 19)
#define CL22_SETTING_2_FLD_rg_100base_x_hdx                    REG_FLD(1, 18)
#define CL22_SETTING_2_FLD_rg_100base_x_fdx                    REG_FLD(1, 17)
#define CL22_SETTING_2_FLD_rg_100base_t4                       REG_FLD(1, 16)
#define CL22_SETTING_2_FLD_rg_cl22_rg7_15_en                   REG_FLD(1, 13)
#define CL22_SETTING_2_FLD_rg_cl22_rg7_14_en                   REG_FLD(1, 12)
#define CL22_SETTING_2_FLD_rg_cl22_rg7_13_en                   REG_FLD(1, 11)
#define CL22_SETTING_2_FLD_rg_cl22_rg7_12_en                   REG_FLD(1, 10)
#define CL22_SETTING_2_FLD_rg_cl22_rg7_11_en                   REG_FLD(1, 9)
#define CL22_SETTING_2_FLD_rg_cl22_rg7_10_0_en                 REG_FLD(1, 8)
#define CL22_SETTING_2_FLD_rg_cl22_rg4_15_en                   REG_FLD(1, 4)
#define CL22_SETTING_2_FLD_rg_cl22_rg4_13_12_en                REG_FLD(1, 3)
#define CL22_SETTING_2_FLD_rg_cl22_rg4_8_7_en                  REG_FLD(1, 2)
#define CL22_SETTING_2_FLD_rg_cl22_rg4_6_en                    REG_FLD(1, 1)
#define CL22_SETTING_2_FLD_rg_cl22_rg4_5_en                    REG_FLD(1, 0)

#define CL22_STS_0_FLD_ro_cl22reg0_15                          REG_FLD(1, 15)
#define CL22_STS_0_FLD_ro_cl22reg0_14                          REG_FLD(1, 14)
#define CL22_STS_0_FLD_ro_cl22reg0_13                          REG_FLD(1, 13)
#define CL22_STS_0_FLD_ro_cl22reg0_12                          REG_FLD(1, 12)
#define CL22_STS_0_FLD_ro_cl22reg0_11                          REG_FLD(1, 11)
#define CL22_STS_0_FLD_ro_cl22reg0_10                          REG_FLD(1, 10)
#define CL22_STS_0_FLD_ro_cl22reg0_9                           REG_FLD(1, 9)
#define CL22_STS_0_FLD_ro_cl22reg0_8                           REG_FLD(1, 8)
#define CL22_STS_0_FLD_ro_cl22reg0_7                           REG_FLD(1, 7)
#define CL22_STS_0_FLD_ro_cl22reg0_6                           REG_FLD(1, 6)
#define CL22_STS_0_FLD_ro_cl22reg0_5                           REG_FLD(1, 5)
#define CL22_STS_0_FLD_ro_cl22reg0_4_0                         REG_FLD(5, 0)

#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ovrun_intr_en REG_FLD(1, 29)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_undrun_intr_en REG_FLD(1, 28)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_odd_nibble_pream_deal_en REG_FLD(1, 27)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rxer_mask REG_FLD(1, 26)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_lpi2idle_en REG_FLD(1, 25)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_eee_test_mode REG_FLD(1, 24)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_lwm    REG_FLD(8, 16)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_hwm    REG_FLD(8, 8)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_mode   REG_FLD(1, 6)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_cnt_clr REG_FLD(1, 5)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_cnt_clr REG_FLD(1, 4)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_rst_n  REG_FLD(1, 1)
#define MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_rst_n  REG_FLD(1, 0)

#define MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_runt_ipg_num REG_FLD(5, 8)
#define MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_min_ipg_num REG_FLD(5, 0)

#define MAC_T2R_LPBK_FIFO_STS_0_FLD_ro_mac_t2r_fifo_rx_start_cnt REG_FLD(32, 0)

#define MAC_T2R_LPBK_FIFO_STS_1_FLD_ro_mac_t2r_fifo_tx_start_cnt REG_FLD(32, 0)

#define MAC_T2R_LPBK_FIFO_STS_2_FLD_ro_mac_t2r_fifo_rx_termi_cnt REG_FLD(32, 0)

#define MAC_T2R_LPBK_FIFO_STS_3_FLD_ro_mac_t2r_fifo_tx_termi_cnt REG_FLD(32, 0)

#define MAC_T2R_LPBK_FIFO_STS_4_FLD_ro_mac_t2r_fifo_ipg_del_cnt REG_FLD(32, 0)

#define MAC_T2R_LPBK_FIFO_STS_5_FLD_ro_mac_t2r_fifo_ipg_ins_cnt REG_FLD(32, 0)

#define MAC_T2R_LPBK_FIFO_STS_6_FLD_ro_mac_t2r_fifo_rx_runt_ipg_cnt REG_FLD(32, 0)

#define MAC_T2R_LPBK_FIFO_STS_7_FLD_ro_mac_t2r_fifo_tx_runt_ipg_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ovrun_intr_en REG_FLD(1, 29)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_undrun_intr_en REG_FLD(1, 28)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_odd_nibble_pream_deal_en REG_FLD(1, 27)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rxer_mask REG_FLD(1, 26)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_lpi2idle_en REG_FLD(1, 25)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_eee_test_mode REG_FLD(1, 24)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_lwm    REG_FLD(8, 16)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_hwm    REG_FLD(8, 8)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_mode   REG_FLD(1, 6)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_cnt_clr REG_FLD(1, 5)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_cnt_clr REG_FLD(1, 4)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_en        REG_FLD(1, 2)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_rst_n  REG_FLD(1, 1)
#define PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_rst_n  REG_FLD(1, 0)

#define PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_runt_ipg_num REG_FLD(5, 8)
#define PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_min_ipg_num REG_FLD(5, 0)

#define PHY_R2T_LPBK_FIFO_STS_0_FLD_ro_phy_r2t_fifo_rx_start_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_STS_1_FLD_ro_phy_r2t_fifo_tx_start_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_STS_2_FLD_ro_phy_r2t_fifo_rx_termi_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_STS_3_FLD_ro_phy_r2t_fifo_tx_termi_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_STS_4_FLD_ro_phy_r2t_fifo_ipg_del_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_STS_5_FLD_ro_phy_r2t_fifo_ipg_ins_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_STS_6_FLD_ro_phy_r2t_fifo_rx_runt_ipg_cnt REG_FLD(32, 0)

#define PHY_R2T_LPBK_FIFO_STS_7_FLD_ro_phy_r2t_fifo_tx_runt_ipg_cnt REG_FLD(32, 0)

#define PHY_TX_FORCE_CTRL_0_FLD_rg_force_txer_sel              REG_FLD(1, 12)
#define PHY_TX_FORCE_CTRL_0_FLD_rg_force_txen_sel              REG_FLD(1, 8)
#define PHY_TX_FORCE_CTRL_0_FLD_rg_force_txc_sel               REG_FLD(1, 4)
#define PHY_TX_FORCE_CTRL_0_FLD_rg_force_txd_sel               REG_FLD(1, 0)

#define PHY_TX_FORCE_CTRL_1_FLD_rg_force_txd_h                 REG_FLD(32, 0)

#define PHY_TX_FORCE_CTRL_2_FLD_rg_force_txd_l                 REG_FLD(32, 0)

#define PHY_TX_FORCE_CTRL_3_FLD_rg_force_txer                  REG_FLD(1, 16)
#define PHY_TX_FORCE_CTRL_3_FLD_rg_force_txen                  REG_FLD(1, 12)
#define PHY_TX_FORCE_CTRL_3_FLD_rg_force_txc                   REG_FLD(8, 0)

#define msg_rx_ctrl_0_FLD_rg_hsgmii_xfi_sel                    REG_FLD(1, 28)
#define msg_rx_ctrl_0_FLD_rg_qsgmii_port_shift                 REG_FLD(2, 24)
#define msg_rx_ctrl_0_FLD_rg_dec_err_1t                        REG_FLD(1, 16)
#define msg_rx_ctrl_0_FLD_rg_sw_reset_rxck                     REG_FLD(1, 12)
#define msg_rx_ctrl_0_FLD_rg_rx_xgmii_mode                     REG_FLD(1, 8)
#define msg_rx_ctrl_0_FLD_rg_rx_5g_mode                        REG_FLD(1, 4)
#define msg_rx_ctrl_0_FLD_rg_rx_clk_en                         REG_FLD(1, 0)

#define multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_all   REG_FLD(1, 5)
#define multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_all   REG_FLD(1, 4)
#define multi_sgmii_reg_phya_65_FLD_rg_tbi_10_rx_reverse_pma   REG_FLD(1, 3)
#define multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_pma   REG_FLD(1, 2)
#define multi_sgmii_reg_phya_65_FLD_rg_tbi_tx_reverse_10b_pma  REG_FLD(1, 1)
#define multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_pma   REG_FLD(1, 0)

#define multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int    REG_FLD(1, 9)
#define multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int_force REG_FLD(1, 8)
#define multi_sgmii_reg_interrupt_sel_FLD_rg_rxer100_int       REG_FLD(1, 4)
#define multi_sgmii_reg_interrupt_sel_FLD_rg_false_carrier_int REG_FLD(1, 3)
#define multi_sgmii_reg_interrupt_sel_FLD_rg_symbol_error_int  REG_FLD(1, 2)
#define multi_sgmii_reg_interrupt_sel_FLD_rg_100fx_int         REG_FLD(1, 1)
#define multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_pcs_int    REG_FLD(1, 0)

#define multi_sgmii_reg_probe_sel_FLD_rg_probe_clk_sel         REG_FLD(16, 16)
#define multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_port_sel REG_FLD(2, 4)
#define multi_sgmii_reg_probe_sel_FLD_rg_dwn_shift_probe_sel   REG_FLD(1, 3)
#define multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_clk_probe_sel  REG_FLD(1, 2)
#define multi_sgmii_reg_probe_sel_FLD_rg_100fx_probe_sel       REG_FLD(1, 1)
#define multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_sel      REG_FLD(1, 0)

#define multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux_en REG_FLD(1, 3)
#define multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux  REG_FLD(1, 2)
#define multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_en   REG_FLD(2, 0)

#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_or REG_FLD(1, 19)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_or REG_FLD(1, 18)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_and REG_FLD(1, 17)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_and REG_FLD(1, 16)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_aempty_cnt_clear REG_FLD(1, 15)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_empty_cnt_clear REG_FLD(1, 14)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_afull_cnt_clear REG_FLD(1, 13)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_full_cnt_clear REG_FLD(1, 12)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_w_threshold REG_FLD(5, 7)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_r_threshold REG_FLD(5, 2)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_rclear REG_FLD(1, 1)
#define multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_wclear REG_FLD(1, 0)

#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_rfifo_cnt REG_FLD(5, 16)
#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_wfifo_cnt REG_FLD(5, 6)
#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_w_ready REG_FLD(1, 5)
#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_r_ready REG_FLD(1, 4)
#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_aempty REG_FLD(1, 3)
#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_empty REG_FLD(1, 2)
#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_afull REG_FLD(1, 1)
#define multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_full REG_FLD(1, 0)

#define multi_sgmii_ro_phya_afifo_cnt_0_FLD_ro_hsgmii_async_fifo_full_cnt REG_FLD(32, 0)

#define multi_sgmii_ro_phya_afifo_cnt_1_FLD_ro_hsgmii_async_fifo_afull_cnt REG_FLD(32, 0)

#define multi_sgmii_ro_phya_afifo_cnt_2_FLD_ro_hsgmii_async_fifo_empty_cnt REG_FLD(32, 0)

#define multi_sgmii_ro_phya_afifo_cnt_3_FLD_ro_hsgmii_async_fifo_aempty_cnt REG_FLD(32, 0)

#define msg_led_ctrl_0_FLD_rg_led_timing_test                  REG_FLD(1, 14)
#define msg_led_ctrl_0_FLD_rg_dwn_shift_en                     REG_FLD(1, 13)
#define msg_led_ctrl_0_FLD_rg_dwn_shift_blk_en                 REG_FLD(1, 12)
#define msg_led_ctrl_0_FLD_rg_led_enhance                      REG_FLD(1, 10)
#define msg_led_ctrl_0_FLD_rg_led_mode                         REG_FLD(2, 8)
#define msg_led_ctrl_0_FLD_rg_led3_pol                         REG_FLD(1, 7)
#define msg_led_ctrl_0_FLD_rg_led2_pol                         REG_FLD(1, 6)
#define msg_led_ctrl_0_FLD_rg_led1_pol                         REG_FLD(1, 5)
#define msg_led_ctrl_0_FLD_rg_led0_pol                         REG_FLD(1, 4)
#define msg_led_ctrl_0_FLD_rg_led3_en                          REG_FLD(1, 3)
#define msg_led_ctrl_0_FLD_rg_led2_en                          REG_FLD(1, 2)
#define msg_led_ctrl_0_FLD_rg_led1_en                          REG_FLD(1, 1)
#define msg_led_ctrl_0_FLD_rg_led0_en                          REG_FLD(1, 0)

#define msg_led_ctrl_1_FLD_rg_led_blk_dur                      REG_FLD(16, 16)
#define msg_led_ctrl_1_FLD_rg_led_on_dur                       REG_FLD(16, 0)

#define msg_led_ctrl_2_FLD_rg_led1_blk_mask                    REG_FLD(8, 24)
#define msg_led_ctrl_2_FLD_rg_led1_on_mask                     REG_FLD(7, 16)
#define msg_led_ctrl_2_FLD_rg_led0_blk_mask                    REG_FLD(8, 8)
#define msg_led_ctrl_2_FLD_rg_led0_on_mask                     REG_FLD(7, 0)

#define msg_led_ctrl_3_FLD_rg_led3_blk_mask                    REG_FLD(8, 24)
#define msg_led_ctrl_3_FLD_rg_led3_on_mask                     REG_FLD(7, 16)
#define msg_led_ctrl_3_FLD_rg_led2_blk_mask                    REG_FLD(8, 8)
#define msg_led_ctrl_3_FLD_rg_led2_on_mask                     REG_FLD(7, 0)

#define msg_led_sts_0_FLD_ro_led_blk_event                     REG_FLD(8, 8)
#define msg_led_sts_0_FLD_ro_led_on_event                      REG_FLD(7, 0)

#define msg_rx_sts_0_FLD_ro_k28_1_cnt                          REG_FLD(32, 0)

#define msg_rx_sts_1_FLD_ro_port0_k28_1_cnt                    REG_FLD(32, 0)

#define msg_rx_sts_2_FLD_ro_port1_k28_1_cnt                    REG_FLD(32, 0)

#define msg_rx_sts_3_FLD_ro_port2_k28_1_cnt                    REG_FLD(32, 0)

#define msg_rx_sts_4_FLD_ro_port3_k28_1_cnt                    REG_FLD(32, 0)

#define msg_rx_sts_5_FLD_ro_rd_out_3b_neg                      REG_FLD(1, 28)
#define msg_rx_sts_5_FLD_ro_rd_out_3b_pos                      REG_FLD(1, 24)
#define msg_rx_sts_5_FLD_ro_rd_out_5b_neg                      REG_FLD(1, 20)
#define msg_rx_sts_5_FLD_ro_rd_out_5b_pos                      REG_FLD(1, 16)
#define msg_rx_sts_5_FLD_ro_dp3_n                              REG_FLD(1, 12)
#define msg_rx_sts_5_FLD_ro_dp3_p                              REG_FLD(1, 8)
#define msg_rx_sts_5_FLD_ro_dp5_n                              REG_FLD(1, 4)
#define msg_rx_sts_5_FLD_ro_dp5_p                              REG_FLD(1, 0)

#define msg_rx_sts_6_FLD_ro_except_okn                         REG_FLD(1, 28)
#define msg_rx_sts_6_FLD_ro_except_err                         REG_FLD(1, 24)
#define msg_rx_sts_6_FLD_ro_except_negi                        REG_FLD(1, 20)
#define msg_rx_sts_6_FLD_ro_except_posi                        REG_FLD(1, 16)
#define msg_rx_sts_6_FLD_ro_except_neg                         REG_FLD(1, 12)
#define msg_rx_sts_6_FLD_ro_except_pos                         REG_FLD(1, 8)
#define msg_rx_sts_6_FLD_ro_except5_n                          REG_FLD(1, 4)
#define msg_rx_sts_6_FLD_ro_except5_p                          REG_FLD(1, 0)

#define msg_rx_sts_7_FLD_ro_carrier_reg                        REG_FLD(1, 28)
#define msg_rx_sts_7_FLD_ro_rd_out_5b                          REG_FLD(1, 24)
#define msg_rx_sts_7_FLD_ro_rd_out_5b_negx                     REG_FLD(1, 20)
#define msg_rx_sts_7_FLD_ro_rd_out_5b_posx                     REG_FLD(1, 16)
#define msg_rx_sts_7_FLD_ro_rd_out_3b_negx                     REG_FLD(1, 12)
#define msg_rx_sts_7_FLD_ro_rd_out_3b_posx                     REG_FLD(1, 8)
#define msg_rx_sts_7_FLD_ro_data_out_3b_index                  REG_FLD(5, 0)

#define msg_rx_sts_8_FLD_ro_error_3b_sc_neg                    REG_FLD(1, 20)
#define msg_rx_sts_8_FLD_ro_error_3b_sc_pos                    REG_FLD(1, 16)
#define msg_rx_sts_8_FLD_ro_error_3b_sc                        REG_FLD(1, 12)
#define msg_rx_sts_8_FLD_ro_error_kout_5b_neg                  REG_FLD(1, 8)
#define msg_rx_sts_8_FLD_ro_error_kout_5b_pos                  REG_FLD(1, 4)
#define msg_rx_sts_8_FLD_ro_error_kout_5b                      REG_FLD(1, 0)

#define msg_rx_sts_9_FLD_ro_error_5b_neg                       REG_FLD(1, 12)
#define msg_rx_sts_9_FLD_ro_error_5b_pos                       REG_FLD(1, 8)
#define msg_rx_sts_9_FLD_ro_error_5b                           REG_FLD(1, 4)
#define msg_rx_sts_9_FLD_ro_error_3b                           REG_FLD(1, 0)

#define msg_rx_sts_10_FLD_ro_data_out_3b                       REG_FLD(3, 16)
#define msg_rx_sts_10_FLD_ro_data_out_5b                       REG_FLD(5, 0)

#define msg_rx_sts_11_FLD_ro_disp_err                          REG_FLD(1, 20)
#define msg_rx_sts_11_FLD_ro_char_err                          REG_FLD(1, 16)
#define msg_rx_sts_11_FLD_ro_kout_3b_pos                       REG_FLD(1, 12)
#define msg_rx_sts_11_FLD_ro_kout_3b_neg                       REG_FLD(1, 8)
#define msg_rx_sts_11_FLD_ro_kout_5b_pos                       REG_FLD(1, 4)
#define msg_rx_sts_11_FLD_ro_kout_5b_neg                       REG_FLD(1, 0)

#define msg_rx_sts_12_FLD_ro_port3_sync_lost                   REG_FLD(1, 28)
#define msg_rx_sts_12_FLD_ro_port2_sync_lost                   REG_FLD(1, 24)
#define msg_rx_sts_12_FLD_ro_port1_sync_lost                   REG_FLD(1, 20)
#define msg_rx_sts_12_FLD_ro_port0_sync_lost                   REG_FLD(1, 16)
#define msg_rx_sts_12_FLD_ro_sync_int                          REG_FLD(1, 12)
#define msg_rx_sts_12_FLD_ro_rd_out                            REG_FLD(1, 8)
#define msg_rx_sts_12_FLD_ro_align_det                         REG_FLD(4, 0)

#define msg_rx_sts_13_FLD_ro_data_align                        REG_FLD(10, 0)

#define msg_rx_sts_14_FLD_ro_par                               REG_FLD(19, 0)

#define msg_rx_sts_15_FLD_ro_lnk_sts_p3                        REG_FLD(1, 15)
#define msg_rx_sts_15_FLD_ro_speed_sts_p3                      REG_FLD(3, 12)
#define msg_rx_sts_15_FLD_ro_lnk_sts_p2                        REG_FLD(1, 11)
#define msg_rx_sts_15_FLD_ro_speed_sts_p2                      REG_FLD(3, 8)
#define msg_rx_sts_15_FLD_ro_lnk_sts_p1                        REG_FLD(1, 7)
#define msg_rx_sts_15_FLD_ro_speed_sts_p1                      REG_FLD(3, 4)
#define msg_rx_sts_15_FLD_ro_lnk_sts_p0                        REG_FLD(1, 3)
#define msg_rx_sts_15_FLD_ro_speed_sts_p0                      REG_FLD(3, 0)

#define msg_rx_sts_16_FLD_ro_p3_rx_eee_flag_latch              REG_FLD(1, 17)
#define msg_rx_sts_16_FLD_ro_p2_rx_eee_flag_latch              REG_FLD(1, 16)
#define msg_rx_sts_16_FLD_ro_p1_rx_eee_flag_latch              REG_FLD(1, 15)
#define msg_rx_sts_16_FLD_ro_p0_rx_eee_flag_latch              REG_FLD(1, 14)
#define msg_rx_sts_16_FLD_ro_p3_tx_eee_flag_latch              REG_FLD(1, 13)
#define msg_rx_sts_16_FLD_ro_p2_tx_eee_flag_latch              REG_FLD(1, 12)
#define msg_rx_sts_16_FLD_ro_p1_tx_eee_flag_latch              REG_FLD(1, 11)
#define msg_rx_sts_16_FLD_ro_p0_tx_eee_flag_latch              REG_FLD(1, 10)
#define msg_rx_sts_16_FLD_ro_sig_detect_down_latch             REG_FLD(1, 9)
#define msg_rx_sts_16_FLD_ro_sig_detect_up_latch               REG_FLD(1, 8)
#define msg_rx_sts_16_FLD_ro_p3_link_down_latch                REG_FLD(1, 7)
#define msg_rx_sts_16_FLD_ro_p3_link_up_latch                  REG_FLD(1, 6)
#define msg_rx_sts_16_FLD_ro_p2_link_down_latch                REG_FLD(1, 5)
#define msg_rx_sts_16_FLD_ro_p2_link_up_latch                  REG_FLD(1, 4)
#define msg_rx_sts_16_FLD_ro_p1_link_down_latch                REG_FLD(1, 3)
#define msg_rx_sts_16_FLD_ro_p1_link_up_latch                  REG_FLD(1, 2)
#define msg_rx_sts_16_FLD_ro_p0_link_down_latch                REG_FLD(1, 1)
#define msg_rx_sts_16_FLD_ro_p0_link_up_latch                  REG_FLD(1, 0)

#define msg_rx_sts_17_FLD_rg_p3_rx_eee_int_clr                 REG_FLD(1, 17)
#define msg_rx_sts_17_FLD_rg_p2_rx_eee_int_clr                 REG_FLD(1, 16)
#define msg_rx_sts_17_FLD_rg_p1_rx_eee_int_clr                 REG_FLD(1, 15)
#define msg_rx_sts_17_FLD_rg_p0_rx_eee_int_clr                 REG_FLD(1, 14)
#define msg_rx_sts_17_FLD_rg_p3_tx_eee_int_clr                 REG_FLD(1, 13)
#define msg_rx_sts_17_FLD_rg_p2_tx_eee_int_clr                 REG_FLD(1, 12)
#define msg_rx_sts_17_FLD_rg_p1_tx_eee_int_clr                 REG_FLD(1, 11)
#define msg_rx_sts_17_FLD_rg_p0_tx_eee_int_clr                 REG_FLD(1, 10)
#define msg_rx_sts_17_FLD_rg_sig_detect_down_int_clr           REG_FLD(1, 9)
#define msg_rx_sts_17_FLD_rg_sig_detect_up_int_clr             REG_FLD(1, 8)
#define msg_rx_sts_17_FLD_rg_p3_link_down_int_clr              REG_FLD(1, 7)
#define msg_rx_sts_17_FLD_rg_p3_link_up_int_clr                REG_FLD(1, 6)
#define msg_rx_sts_17_FLD_rg_p2_link_down_int_clr              REG_FLD(1, 5)
#define msg_rx_sts_17_FLD_rg_p2_link_up_int_clr                REG_FLD(1, 4)
#define msg_rx_sts_17_FLD_rg_p1_link_down_int_clr              REG_FLD(1, 3)
#define msg_rx_sts_17_FLD_rg_p1_link_up_int_clr                REG_FLD(1, 2)
#define msg_rx_sts_17_FLD_rg_p0_link_down_int_clr              REG_FLD(1, 1)
#define msg_rx_sts_17_FLD_rg_p0_link_up_int_clr                REG_FLD(1, 0)

#define msg_rx_sts_18_FLD_p3_sgmii_is_10                       REG_FLD(1, 14)
#define msg_rx_sts_18_FLD_p3_sgmii_is_100                      REG_FLD(1, 13)
#define msg_rx_sts_18_FLD_p3_sgmii_is_1g                       REG_FLD(1, 12)
#define msg_rx_sts_18_FLD_p2_sgmii_is_10                       REG_FLD(1, 10)
#define msg_rx_sts_18_FLD_p2_sgmii_is_100                      REG_FLD(1, 9)
#define msg_rx_sts_18_FLD_p2_sgmii_is_1g                       REG_FLD(1, 8)
#define msg_rx_sts_18_FLD_p1_sgmii_is_10                       REG_FLD(1, 6)
#define msg_rx_sts_18_FLD_p1_sgmii_is_100                      REG_FLD(1, 5)
#define msg_rx_sts_18_FLD_p1_sgmii_is_1g                       REG_FLD(1, 4)
#define msg_rx_sts_18_FLD_p0_sgmii_is_10                       REG_FLD(1, 2)
#define msg_rx_sts_18_FLD_p0_sgmii_is_100                      REG_FLD(1, 1)
#define msg_rx_sts_18_FLD_p0_sgmii_is_1g                       REG_FLD(1, 0)

#define msg_rx_sts_19_FLD_p1_lp_ability                        REG_FLD(16, 16)
#define msg_rx_sts_19_FLD_p0_lp_ability                        REG_FLD(16, 0)

#define msg_rx_sts_20_FLD_p3_lp_ability                        REG_FLD(16, 16)
#define msg_rx_sts_20_FLD_p2_lp_ability                        REG_FLD(16, 0)

#define cl22_status_0_FLD_ro_cl22_reset_wrtie_flag             REG_FLD(1, 1)
#define cl22_status_0_FLD_ro_cl22_wrtie_flag                   REG_FLD(1, 0)

#define cl22_status_1_FLD_rg_cl22_reset_wrtie_flag_clr         REG_FLD(1, 1)
#define cl22_status_1_FLD_rg_cl22_wrtie_flag_clr               REG_FLD(1, 0)

#define msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p3_clr            REG_FLD(1, 7)
#define msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p2_clr            REG_FLD(1, 6)
#define msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p1_clr            REG_FLD(1, 5)
#define msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p0_clr            REG_FLD(1, 4)
#define msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p3_clr            REG_FLD(1, 3)
#define msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p2_clr            REG_FLD(1, 2)
#define msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p1_clr            REG_FLD(1, 1)
#define msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p0_clr            REG_FLD(1, 0)

#define msg_rx_ctrl_1_FLD_rg_force_port3_cnt                   REG_FLD(2, 28)
#define msg_rx_ctrl_1_FLD_rg_force_port2_cnt                   REG_FLD(2, 24)
#define msg_rx_ctrl_1_FLD_rg_force_port1_cnt                   REG_FLD(2, 20)
#define msg_rx_ctrl_1_FLD_rg_force_port0_cnt                   REG_FLD(2, 16)
#define msg_rx_ctrl_1_FLD_rg_force_port3_cnt_sel               REG_FLD(1, 12)
#define msg_rx_ctrl_1_FLD_rg_force_port2_cnt_sel               REG_FLD(1, 8)
#define msg_rx_ctrl_1_FLD_rg_force_port1_cnt_sel               REG_FLD(1, 4)
#define msg_rx_ctrl_1_FLD_rg_force_port0_cnt_sel               REG_FLD(1, 0)

#define msg_rx_ctrl_2_FLD_rg_force_port3_sync                  REG_FLD(1, 28)
#define msg_rx_ctrl_2_FLD_rg_force_port2_sync                  REG_FLD(1, 24)
#define msg_rx_ctrl_2_FLD_rg_force_port1_sync                  REG_FLD(1, 20)
#define msg_rx_ctrl_2_FLD_rg_force_port0_sync                  REG_FLD(1, 16)
#define msg_rx_ctrl_2_FLD_rg_force_port3_sync_sel              REG_FLD(1, 12)
#define msg_rx_ctrl_2_FLD_rg_force_port2_sync_sel              REG_FLD(1, 8)
#define msg_rx_ctrl_2_FLD_rg_force_port1_sync_sel              REG_FLD(1, 4)
#define msg_rx_ctrl_2_FLD_rg_force_port0_sync_sel              REG_FLD(1, 0)

#define msg_rx_ctrl_3_FLD_rg_k28_1_cnt_clr                     REG_FLD(1, 16)
#define msg_rx_ctrl_3_FLD_rg_port3_k28_1_cnt_clr               REG_FLD(1, 12)
#define msg_rx_ctrl_3_FLD_rg_port2_k28_1_cnt_clr               REG_FLD(1, 8)
#define msg_rx_ctrl_3_FLD_rg_port1_k28_1_cnt_clr               REG_FLD(1, 4)
#define msg_rx_ctrl_3_FLD_rg_port0_k28_1_cnt_clr               REG_FLD(1, 0)

#define msg_rx_ctrl_4_FLD_rg_qsgmii_rx500_pcs_div_ck_sel       REG_FLD(1, 11)
#define msg_rx_ctrl_4_FLD_rg_p3_mac_rx_div_ck_reset_n          REG_FLD(1, 10)
#define msg_rx_ctrl_4_FLD_rg_p2_mac_rx_div_ck_reset_n          REG_FLD(1, 9)
#define msg_rx_ctrl_4_FLD_rg_p1_mac_rx_div_ck_reset_n          REG_FLD(1, 8)
#define msg_rx_ctrl_4_FLD_rg_qsgmii_p3_rx500_mac_div_ck_sel    REG_FLD(2, 6)
#define msg_rx_ctrl_4_FLD_rg_qsgmii_p2_rx500_mac_div_ck_sel    REG_FLD(2, 4)
#define msg_rx_ctrl_4_FLD_rg_qsgmii_p1_rx500_mac_div_ck_sel    REG_FLD(2, 2)
#define msg_rx_ctrl_4_FLD_rg_qsgmii_p0_rx500_mac_div_ck_sel    REG_FLD(2, 0)

#define msg_rx_ctrl_5_FLD_rg_fx_crs_test                       REG_FLD(3, 28)
#define msg_rx_ctrl_5_FLD_rg_fx_col_test                       REG_FLD(3, 24)
#define msg_rx_ctrl_5_FLD_rg_hg_crs_test                       REG_FLD(4, 20)
#define msg_rx_ctrl_5_FLD_rg_hg_col_test                       REG_FLD(4, 16)
#define msg_rx_ctrl_5_FLD_rg_crs_is_fx_mode                    REG_FLD(1, 9)
#define msg_rx_ctrl_5_FLD_rg_col_is_fx_mode                    REG_FLD(1, 8)
#define msg_rx_ctrl_5_FLD_rg_fx_crs_sel                        REG_FLD(1, 5)
#define msg_rx_ctrl_5_FLD_rg_fx_col_sel                        REG_FLD(1, 4)
#define msg_rx_ctrl_5_FLD_rg_fx_crs_test_mode                  REG_FLD(1, 3)
#define msg_rx_ctrl_5_FLD_rg_fx_col_test_mode                  REG_FLD(1, 2)
#define msg_rx_ctrl_5_FLD_rg_hg_crs_test_mode                  REG_FLD(1, 1)
#define msg_rx_ctrl_5_FLD_rg_hg_col_test_mode                  REG_FLD(1, 0)

#define msg_rx_lik_sts_0_FLD_rg_eee2p5g_sts                    REG_FLD(1, 31)
#define msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p3                   REG_FLD(1, 28)
#define msg_rx_lik_sts_0_FLD_rg_eee100_sts_p3                  REG_FLD(1, 27)
#define msg_rx_lik_sts_0_FLD_rg_txfc_sts_p3                    REG_FLD(1, 26)
#define msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p3                    REG_FLD(1, 25)
#define msg_rx_lik_sts_0_FLD_rg_dpx_sts_p3                     REG_FLD(1, 24)
#define msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p2                   REG_FLD(1, 20)
#define msg_rx_lik_sts_0_FLD_rg_eee100_sts_p2                  REG_FLD(1, 19)
#define msg_rx_lik_sts_0_FLD_rg_txfc_sts_p2                    REG_FLD(1, 18)
#define msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p2                    REG_FLD(1, 17)
#define msg_rx_lik_sts_0_FLD_rg_dpx_sts_p2                     REG_FLD(1, 16)
#define msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p1                   REG_FLD(1, 12)
#define msg_rx_lik_sts_0_FLD_rg_eee100_sts_p1                  REG_FLD(1, 11)
#define msg_rx_lik_sts_0_FLD_rg_txfc_sts_p1                    REG_FLD(1, 10)
#define msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p1                    REG_FLD(1, 9)
#define msg_rx_lik_sts_0_FLD_rg_dpx_sts_p1                     REG_FLD(1, 8)
#define msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p0                   REG_FLD(1, 4)
#define msg_rx_lik_sts_0_FLD_rg_eee100_sts_p0                  REG_FLD(1, 3)
#define msg_rx_lik_sts_0_FLD_rg_txfc_sts_p0                    REG_FLD(1, 2)
#define msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p0                    REG_FLD(1, 1)
#define msg_rx_lik_sts_0_FLD_rg_dpx_sts_p0                     REG_FLD(1, 0)

#define msg_rx_lik_sts_1_FLD_rg_pause_sts_p3                   REG_FLD(1, 7)
#define msg_rx_lik_sts_1_FLD_rg_pause_sts_p2                   REG_FLD(1, 6)
#define msg_rx_lik_sts_1_FLD_rg_pause_sts_p1                   REG_FLD(1, 5)
#define msg_rx_lik_sts_1_FLD_rg_pause_sts_p0                   REG_FLD(1, 4)
#define msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p3                   REG_FLD(1, 3)
#define msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p2                   REG_FLD(1, 2)
#define msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p1                   REG_FLD(1, 1)
#define msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p0                   REG_FLD(1, 0)

#define msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p3              REG_FLD(1, 11)
#define msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p2              REG_FLD(1, 10)
#define msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p1              REG_FLD(1, 9)
#define msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p0              REG_FLD(1, 8)
#define msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p3              REG_FLD(1, 7)
#define msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p2              REG_FLD(1, 6)
#define msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p1              REG_FLD(1, 5)
#define msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p0              REG_FLD(1, 4)
#define msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p3               REG_FLD(1, 3)
#define msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p2               REG_FLD(1, 2)
#define msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p1               REG_FLD(1, 1)
#define msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p0               REG_FLD(1, 0)

#define PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxer_sel              REG_FLD(1, 12)
#define PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxdv_sel              REG_FLD(1, 8)
#define PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxc_sel               REG_FLD(1, 4)
#define PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxd_sel               REG_FLD(1, 0)

#define PHY_RX_FORCE_CTRL_1_FLD_rg_force_rxd_h                 REG_FLD(32, 0)

#define PHY_RX_FORCE_CTRL_2_FLD_rg_force_rxd_l                 REG_FLD(32, 0)

#define PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxer                  REG_FLD(1, 16)
#define PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxdv                  REG_FLD(1, 12)
#define PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxc                   REG_FLD(8, 0)

#define Debug_REGISTER_0_FLD_ro_dump_000                       REG_FLD(32, 0)

#define Debug_REGISTER_1_FLD_ro_dump_001                       REG_FLD(32, 0)

#define Debug_REGISTER_2_FLD_ro_dump_002                       REG_FLD(32, 0)

#define Debug_REGISTER_3_FLD_ro_dump_003                       REG_FLD(32, 0)

#define Debug_REGISTER_4_FLD_ro_dump_004                       REG_FLD(32, 0)

#define Debug_REGISTER_5_FLD_ro_dump_005                       REG_FLD(32, 0)

#define Debug_REGISTER_6_FLD_ro_dump_006                       REG_FLD(32, 0)

#define Debug_REGISTER_7_FLD_ro_dump_007                       REG_FLD(32, 0)

#define Debug_REGISTER_8_FLD_ro_dump_008                       REG_FLD(32, 0)

#define Debug_REGISTER_9_FLD_ro_dump_009                       REG_FLD(32, 0)

#define Debug_REGISTER_10_FLD_ro_dump_010                      REG_FLD(32, 0)

#define Debug_REGISTER_11_FLD_ro_dump_011                      REG_FLD(32, 0)

#define Debug_REGISTER_12_FLD_ro_dump_012                      REG_FLD(32, 0)

#define Debug_REGISTER_13_FLD_ro_dump_013                      REG_FLD(32, 0)

#define Debug_REGISTER_14_FLD_ro_dump_014                      REG_FLD(32, 0)

#define Debug_REGISTER_15_FLD_ro_dump_015                      REG_FLD(32, 0)

#define Debug_REGISTER_16_FLD_ro_dump_016                      REG_FLD(32, 0)

#define Debug_REGISTER_17_FLD_ro_dump_017                      REG_FLD(32, 0)

#define Debug_REGISTER_18_FLD_ro_dump_018                      REG_FLD(32, 0)

#define Debug_REGISTER_19_FLD_ro_dump_019                      REG_FLD(32, 0)

#define Debug_REGISTER_20_FLD_ro_dump_020                      REG_FLD(32, 0)

#define Debug_REGISTER_21_FLD_ro_dump_021                      REG_FLD(32, 0)

#define Debug_REGISTER_22_FLD_ro_dump_022                      REG_FLD(32, 0)

#define Debug_REGISTER_23_FLD_ro_dump_023                      REG_FLD(32, 0)

#define Debug_REGISTER_24_FLD_ro_dump_024                      REG_FLD(32, 0)

#define Debug_REGISTER_25_FLD_ro_dump_025                      REG_FLD(32, 0)

#define Debug_REGISTER_26_FLD_ro_dump_026                      REG_FLD(32, 0)

#define Debug_REGISTER_27_FLD_ro_dump_027                      REG_FLD(32, 0)

#define Debug_REGISTER_28_FLD_ro_dump_028                      REG_FLD(32, 0)

#define Debug_REGISTER_29_FLD_ro_dump_029                      REG_FLD(32, 0)

#define Debug_REGISTER_30_FLD_ro_dump_030                      REG_FLD(32, 0)

#define Debug_REGISTER_31_FLD_ro_dump_031                      REG_FLD(32, 0)

#define Debug_REGISTER_CTRL_0_FLD_rg_force_dump                REG_FLD(1, 8)
#define Debug_REGISTER_CTRL_0_FLD_rg_dump_sel                  REG_FLD(3, 4)
#define Debug_REGISTER_CTRL_0_FLD_rg_dump_inv                  REG_FLD(1, 1)
#define Debug_REGISTER_CTRL_0_FLD_rg_dump_clr                  REG_FLD(1, 0)

#define Debug_REGISTER_DUMP_CNT_FLD_ro_dump_marker             REG_FLD(8, 0)

#define msg_tx_ctrl_0_GET_rg_force_100fx_sigdet(reg32)         REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_force_100fx_sigdet, (reg32))
#define msg_tx_ctrl_0_GET_rg_ssusb_hsgmii_sel(reg32)           REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_ssusb_hsgmii_sel, (reg32))
#define msg_tx_ctrl_0_GET_rg_sgmii_mode_force(reg32)           REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_sgmii_mode_force, (reg32))
#define msg_tx_ctrl_0_GET_rg_qsgmii_mode(reg32)                REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_qsgmii_mode, (reg32))
#define msg_tx_ctrl_0_GET_rg_tx_data_re(reg32)                 REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_tx_data_re, (reg32))
#define msg_tx_ctrl_0_GET_rg_tx_data_rep4_sel(reg32)           REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_tx_data_rep4_sel, (reg32))
#define msg_tx_ctrl_0_GET_rg_tx_data_rep2_sel(reg32)           REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_tx_data_rep2_sel, (reg32))
#define msg_tx_ctrl_0_GET_rg_qsgmii_txclk_ena(reg32)           REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_qsgmii_txclk_ena, (reg32))
#define msg_tx_ctrl_0_GET_rg_tx_free_cnt_clr(reg32)            REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_tx_free_cnt_clr, (reg32))
#define msg_tx_ctrl_0_GET_rg_tx_gpii_rst(reg32)                REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_tx_gpii_rst, (reg32))
#define msg_tx_ctrl_0_GET_rg_tx_5g_mode(reg32)                 REG_FLD_GET(msg_tx_ctrl_0_FLD_rg_tx_5g_mode, (reg32))

#define msg_tx_ctrl_1_GET_rg_qsgmii_tx500_pcs_div_ck_sel(reg32) REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_qsgmii_tx500_pcs_div_ck_sel, (reg32))
#define msg_tx_ctrl_1_GET_rg_p3_mac_tx_div_ck_reset_n(reg32)   REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_p3_mac_tx_div_ck_reset_n, (reg32))
#define msg_tx_ctrl_1_GET_rg_p2_mac_tx_div_ck_reset_n(reg32)   REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_p2_mac_tx_div_ck_reset_n, (reg32))
#define msg_tx_ctrl_1_GET_rg_p1_mac_tx_div_ck_reset_n(reg32)   REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_p1_mac_tx_div_ck_reset_n, (reg32))
#define msg_tx_ctrl_1_GET_rg_qsgmii_p3_tx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_qsgmii_p3_tx500_mac_div_ck_sel, (reg32))
#define msg_tx_ctrl_1_GET_rg_qsgmii_p2_tx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_qsgmii_p2_tx500_mac_div_ck_sel, (reg32))
#define msg_tx_ctrl_1_GET_rg_qsgmii_p1_tx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_qsgmii_p1_tx500_mac_div_ck_sel, (reg32))
#define msg_tx_ctrl_1_GET_rg_qsgmii_p0_tx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_tx_ctrl_1_FLD_rg_qsgmii_p0_tx500_mac_div_ck_sel, (reg32))

#define msg_tx_ctrl_2_GET_rg_qsgmii_tx_kd_rev(reg32)           REG_FLD_GET(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_kd_rev, (reg32))
#define msg_tx_ctrl_2_GET_rg_qsgmii_tx_idle_swap_en(reg32)     REG_FLD_GET(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_idle_swap_en, (reg32))
#define msg_tx_ctrl_2_GET_rg_qsgmii_tx_rev(reg32)              REG_FLD_GET(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_rev, (reg32))
#define msg_tx_ctrl_2_GET_rg_qsgmii_20b_tx_hl_swap(reg32)      REG_FLD_GET(msg_tx_ctrl_2_FLD_rg_qsgmii_20b_tx_hl_swap, (reg32))

#define efuse_ctrl_dummy_GET_rg_efuse_tphy_auto_load_valid(reg32) REG_FLD_GET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_auto_load_valid, (reg32))
#define efuse_ctrl_dummy_GET_rg_efuse_tphy_iext_intr_ctrl(reg32) REG_FLD_GET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_iext_intr_ctrl, (reg32))
#define efuse_ctrl_dummy_GET_rg_efuse_tphy_tx_imp_sel(reg32)   REG_FLD_GET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_tx_imp_sel, (reg32))
#define efuse_ctrl_dummy_GET_rg_efuse_tphy_rx_imp_sel(reg32)   REG_FLD_GET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_rx_imp_sel, (reg32))

#define rst_ctrl_0_GET_rg_hsgmii_mac_sw_rst_n(reg32)           REG_FLD_GET(rst_ctrl_0_FLD_rg_hsgmii_mac_sw_rst_n, (reg32))
#define rst_ctrl_0_GET_rg_ssusb_ip_sw_rst(reg32)               REG_FLD_GET(rst_ctrl_0_FLD_rg_ssusb_ip_sw_rst, (reg32))

#define INTERRUPT_EN_0_GET_rg_cl22_rg_phy_rst_cnt(reg32)       REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_cl22_rg_phy_rst_cnt, (reg32))
#define INTERRUPT_EN_0_GET_rg_rx_eee_int_en(reg32)             REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_rx_eee_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_tx_eee_int_en(reg32)             REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_tx_eee_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_cl22_reset_wr_int_en(reg32)      REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_cl22_reset_wr_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_cl22_wr_int_en(reg32)            REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_cl22_wr_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_sgmii_link_down_int_en(reg32)    REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_sgmii_link_down_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_sgmii_link_up_int_en(reg32)      REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_sgmii_link_up_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_sigdet_down_int_en(reg32)        REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_sigdet_down_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_sigdet_up_int_en(reg32)          REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_sigdet_up_int_en, (reg32))
#define INTERRUPT_EN_0_GET_rg_pcs_int_en_3(reg32)              REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_3, (reg32))
#define INTERRUPT_EN_0_GET_rg_pcs_int_en_2(reg32)              REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_2, (reg32))
#define INTERRUPT_EN_0_GET_rg_pcs_int_en_1(reg32)              REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_1, (reg32))
#define INTERRUPT_EN_0_GET_rg_pcs_int_en_0(reg32)              REG_FLD_GET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_0, (reg32))

#define SGMII_STS_CTRL_0_GET_rg_link_mode_p3(reg32)            REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p3, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_spd_mode_p3(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p3, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkdown_p3(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p3, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkup_p3(reg32)         REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p3, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_link_mode_p2(reg32)            REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p2, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_spd_mode_p2(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p2, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkdown_p2(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p2, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkup_p2(reg32)         REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p2, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_link_mode_p1(reg32)            REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p1, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_spd_mode_p1(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p1, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkdown_p1(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p1, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkup_p1(reg32)         REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p1, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_link_mode_p0(reg32)            REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p0, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_spd_mode_p0(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p0, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkdown_p0(reg32)       REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p0, (reg32))
#define SGMII_STS_CTRL_0_GET_rg_force_linkup_p0(reg32)         REG_FLD_GET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p0, (reg32))

#define SGMII_STS_CTRL_1_GET_rg_tx_d_dummy(reg32)              REG_FLD_GET(SGMII_STS_CTRL_1_FLD_rg_tx_d_dummy, (reg32))

#define SGMII_STS_CTRL_2_GET_rg_tx_c_dummy(reg32)              REG_FLD_GET(SGMII_STS_CTRL_2_FLD_rg_tx_c_dummy, (reg32))

#define FPGA_STS_CTRL_GET_rg_fpga_ctrl_31_0(reg32)             REG_FLD_GET(FPGA_STS_CTRL_FLD_rg_fpga_ctrl_31_0, (reg32))

#define SYSYEM_DEBUG_CTRL_GET_rg_debug_ctrl(reg32)             REG_FLD_GET(SYSYEM_DEBUG_CTRL_FLD_rg_debug_ctrl, (reg32))

#define FPGA_BEN_CTRL_GET_rg_fpga_ctrl_ben(reg32)              REG_FLD_GET(FPGA_BEN_CTRL_FLD_rg_fpga_ctrl_ben, (reg32))

#define CL22_SETTING_0_GET_rg_phy_id_1(reg32)                  REG_FLD_GET(CL22_SETTING_0_FLD_rg_phy_id_1, (reg32))
#define CL22_SETTING_0_GET_rg_phy_id_0(reg32)                  REG_FLD_GET(CL22_SETTING_0_FLD_rg_phy_id_0, (reg32))

#define CL22_SETTING_1_GET_rg_cl22_rg_status_clr(reg32)        REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22_rg_status_clr, (reg32))
#define CL22_SETTING_1_GET_rg_cl22_an_function_en(reg32)       REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22_an_function_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_15_en(reg32)            REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_15_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_14_en(reg32)            REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_14_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_13_en(reg32)            REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_13_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_12_en(reg32)            REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_12_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_11_en(reg32)            REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_11_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_10_en(reg32)            REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_10_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_9_en(reg32)             REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_9_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_8_en(reg32)             REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_8_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_7_en(reg32)             REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_7_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_6_en(reg32)             REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_6_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_5_en(reg32)             REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_5_en, (reg32))
#define CL22_SETTING_1_GET_rg_cl22reg0_4_0_en(reg32)           REG_FLD_GET(CL22_SETTING_1_FLD_rg_cl22reg0_4_0_en, (reg32))

#define CL22_SETTING_2_GET_rg_an_ability_en_reserve(reg32)     REG_FLD_GET(CL22_SETTING_2_FLD_rg_an_ability_en_reserve, (reg32))
#define CL22_SETTING_2_GET_rg_uni_direct_en_reserve(reg32)     REG_FLD_GET(CL22_SETTING_2_FLD_rg_uni_direct_en_reserve, (reg32))
#define CL22_SETTING_2_GET_rg_ext_cap(reg32)                   REG_FLD_GET(CL22_SETTING_2_FLD_rg_ext_cap, (reg32))
#define CL22_SETTING_2_GET_rg_jabber_det(reg32)                REG_FLD_GET(CL22_SETTING_2_FLD_rg_jabber_det, (reg32))
#define CL22_SETTING_2_GET_rg_mf_pream_sup(reg32)              REG_FLD_GET(CL22_SETTING_2_FLD_rg_mf_pream_sup, (reg32))
#define CL22_SETTING_2_GET_rg_ext_status(reg32)                REG_FLD_GET(CL22_SETTING_2_FLD_rg_ext_status, (reg32))
#define CL22_SETTING_2_GET_rg_100base_t2_hdx(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_100base_t2_hdx, (reg32))
#define CL22_SETTING_2_GET_rg_100base_t2_fdx(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_100base_t2_fdx, (reg32))
#define CL22_SETTING_2_GET_rg_10mbps_hdx(reg32)                REG_FLD_GET(CL22_SETTING_2_FLD_rg_10mbps_hdx, (reg32))
#define CL22_SETTING_2_GET_rg_10mbps_fdx(reg32)                REG_FLD_GET(CL22_SETTING_2_FLD_rg_10mbps_fdx, (reg32))
#define CL22_SETTING_2_GET_rg_100base_x_hdx(reg32)             REG_FLD_GET(CL22_SETTING_2_FLD_rg_100base_x_hdx, (reg32))
#define CL22_SETTING_2_GET_rg_100base_x_fdx(reg32)             REG_FLD_GET(CL22_SETTING_2_FLD_rg_100base_x_fdx, (reg32))
#define CL22_SETTING_2_GET_rg_100base_t4(reg32)                REG_FLD_GET(CL22_SETTING_2_FLD_rg_100base_t4, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg7_15_en(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg7_15_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg7_14_en(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg7_14_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg7_13_en(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg7_13_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg7_12_en(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg7_12_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg7_11_en(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg7_11_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg7_10_0_en(reg32)          REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg7_10_0_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg4_15_en(reg32)            REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg4_15_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg4_13_12_en(reg32)         REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg4_13_12_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg4_8_7_en(reg32)           REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg4_8_7_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg4_6_en(reg32)             REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg4_6_en, (reg32))
#define CL22_SETTING_2_GET_rg_cl22_rg4_5_en(reg32)             REG_FLD_GET(CL22_SETTING_2_FLD_rg_cl22_rg4_5_en, (reg32))

#define CL22_STS_0_GET_ro_cl22reg0_15(reg32)                   REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_15, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_14(reg32)                   REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_14, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_13(reg32)                   REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_13, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_12(reg32)                   REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_12, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_11(reg32)                   REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_11, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_10(reg32)                   REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_10, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_9(reg32)                    REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_9, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_8(reg32)                    REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_8, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_7(reg32)                    REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_7, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_6(reg32)                    REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_6, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_5(reg32)                    REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_5, (reg32))
#define CL22_STS_0_GET_ro_cl22reg0_4_0(reg32)                  REG_FLD_GET(CL22_STS_0_FLD_ro_cl22reg0_4_0, (reg32))

#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_ovrun_intr_en(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ovrun_intr_en, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_undrun_intr_en(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_undrun_intr_en, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_odd_nibble_pream_deal_en(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_odd_nibble_pream_deal_en, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_rxer_mask(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rxer_mask, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_lpi2idle_en(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_lpi2idle_en, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_eee_test_mode(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_eee_test_mode, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_ss_lwm(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_lwm, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_ss_hwm(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_hwm, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_ss_mode(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_mode, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_tx_cnt_clr(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_cnt_clr, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_rx_cnt_clr(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_cnt_clr, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_tx_rst_n(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_rst_n, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_0_GET_rg_mac_t2r_fifo_rx_rst_n(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_rst_n, (reg32))

#define MAC_T2R_LPBK_FIFO_CTRL_1_GET_rg_mac_t2r_fifo_runt_ipg_num(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_runt_ipg_num, (reg32))
#define MAC_T2R_LPBK_FIFO_CTRL_1_GET_rg_mac_t2r_fifo_min_ipg_num(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_min_ipg_num, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_0_GET_ro_mac_t2r_fifo_rx_start_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_0_FLD_ro_mac_t2r_fifo_rx_start_cnt, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_1_GET_ro_mac_t2r_fifo_tx_start_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_1_FLD_ro_mac_t2r_fifo_tx_start_cnt, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_2_GET_ro_mac_t2r_fifo_rx_termi_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_2_FLD_ro_mac_t2r_fifo_rx_termi_cnt, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_3_GET_ro_mac_t2r_fifo_tx_termi_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_3_FLD_ro_mac_t2r_fifo_tx_termi_cnt, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_4_GET_ro_mac_t2r_fifo_ipg_del_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_4_FLD_ro_mac_t2r_fifo_ipg_del_cnt, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_5_GET_ro_mac_t2r_fifo_ipg_ins_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_5_FLD_ro_mac_t2r_fifo_ipg_ins_cnt, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_6_GET_ro_mac_t2r_fifo_rx_runt_ipg_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_6_FLD_ro_mac_t2r_fifo_rx_runt_ipg_cnt, (reg32))

#define MAC_T2R_LPBK_FIFO_STS_7_GET_ro_mac_t2r_fifo_tx_runt_ipg_cnt(reg32) REG_FLD_GET(MAC_T2R_LPBK_FIFO_STS_7_FLD_ro_mac_t2r_fifo_tx_runt_ipg_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_ovrun_intr_en(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ovrun_intr_en, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_undrun_intr_en(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_undrun_intr_en, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_odd_nibble_pream_deal_en(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_odd_nibble_pream_deal_en, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_rxer_mask(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rxer_mask, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_lpi2idle_en(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_lpi2idle_en, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_eee_test_mode(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_eee_test_mode, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_ss_lwm(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_lwm, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_ss_hwm(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_hwm, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_ss_mode(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_mode, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_tx_cnt_clr(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_cnt_clr, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_rx_cnt_clr(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_cnt_clr, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_en(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_en, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_tx_rst_n(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_rst_n, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_0_GET_rg_phy_r2t_fifo_rx_rst_n(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_rst_n, (reg32))

#define PHY_R2T_LPBK_FIFO_CTRL_1_GET_rg_phy_r2t_fifo_runt_ipg_num(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_runt_ipg_num, (reg32))
#define PHY_R2T_LPBK_FIFO_CTRL_1_GET_rg_phy_r2t_fifo_min_ipg_num(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_min_ipg_num, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_0_GET_ro_phy_r2t_fifo_rx_start_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_0_FLD_ro_phy_r2t_fifo_rx_start_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_1_GET_ro_phy_r2t_fifo_tx_start_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_1_FLD_ro_phy_r2t_fifo_tx_start_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_2_GET_ro_phy_r2t_fifo_rx_termi_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_2_FLD_ro_phy_r2t_fifo_rx_termi_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_3_GET_ro_phy_r2t_fifo_tx_termi_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_3_FLD_ro_phy_r2t_fifo_tx_termi_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_4_GET_ro_phy_r2t_fifo_ipg_del_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_4_FLD_ro_phy_r2t_fifo_ipg_del_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_5_GET_ro_phy_r2t_fifo_ipg_ins_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_5_FLD_ro_phy_r2t_fifo_ipg_ins_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_6_GET_ro_phy_r2t_fifo_rx_runt_ipg_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_6_FLD_ro_phy_r2t_fifo_rx_runt_ipg_cnt, (reg32))

#define PHY_R2T_LPBK_FIFO_STS_7_GET_ro_phy_r2t_fifo_tx_runt_ipg_cnt(reg32) REG_FLD_GET(PHY_R2T_LPBK_FIFO_STS_7_FLD_ro_phy_r2t_fifo_tx_runt_ipg_cnt, (reg32))

#define PHY_TX_FORCE_CTRL_0_GET_rg_force_txer_sel(reg32)       REG_FLD_GET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txer_sel, (reg32))
#define PHY_TX_FORCE_CTRL_0_GET_rg_force_txen_sel(reg32)       REG_FLD_GET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txen_sel, (reg32))
#define PHY_TX_FORCE_CTRL_0_GET_rg_force_txc_sel(reg32)        REG_FLD_GET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txc_sel, (reg32))
#define PHY_TX_FORCE_CTRL_0_GET_rg_force_txd_sel(reg32)        REG_FLD_GET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txd_sel, (reg32))

#define PHY_TX_FORCE_CTRL_1_GET_rg_force_txd_h(reg32)          REG_FLD_GET(PHY_TX_FORCE_CTRL_1_FLD_rg_force_txd_h, (reg32))

#define PHY_TX_FORCE_CTRL_2_GET_rg_force_txd_l(reg32)          REG_FLD_GET(PHY_TX_FORCE_CTRL_2_FLD_rg_force_txd_l, (reg32))

#define PHY_TX_FORCE_CTRL_3_GET_rg_force_txer(reg32)           REG_FLD_GET(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txer, (reg32))
#define PHY_TX_FORCE_CTRL_3_GET_rg_force_txen(reg32)           REG_FLD_GET(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txen, (reg32))
#define PHY_TX_FORCE_CTRL_3_GET_rg_force_txc(reg32)            REG_FLD_GET(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txc, (reg32))

#define msg_rx_ctrl_0_GET_rg_hsgmii_xfi_sel(reg32)             REG_FLD_GET(msg_rx_ctrl_0_FLD_rg_hsgmii_xfi_sel, (reg32))
#define msg_rx_ctrl_0_GET_rg_qsgmii_port_shift(reg32)          REG_FLD_GET(msg_rx_ctrl_0_FLD_rg_qsgmii_port_shift, (reg32))
#define msg_rx_ctrl_0_GET_rg_dec_err_1t(reg32)                 REG_FLD_GET(msg_rx_ctrl_0_FLD_rg_dec_err_1t, (reg32))
#define msg_rx_ctrl_0_GET_rg_sw_reset_rxck(reg32)              REG_FLD_GET(msg_rx_ctrl_0_FLD_rg_sw_reset_rxck, (reg32))
#define msg_rx_ctrl_0_GET_rg_rx_xgmii_mode(reg32)              REG_FLD_GET(msg_rx_ctrl_0_FLD_rg_rx_xgmii_mode, (reg32))
#define msg_rx_ctrl_0_GET_rg_rx_5g_mode(reg32)                 REG_FLD_GET(msg_rx_ctrl_0_FLD_rg_rx_5g_mode, (reg32))
#define msg_rx_ctrl_0_GET_rg_rx_clk_en(reg32)                  REG_FLD_GET(msg_rx_ctrl_0_FLD_rg_rx_clk_en, (reg32))

#define multi_sgmii_reg_phya_65_GET_rg_tbi_20_tx_reverse_all(reg32) REG_FLD_GET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_all, (reg32))
#define multi_sgmii_reg_phya_65_GET_rg_tbi_20_rx_reverse_all(reg32) REG_FLD_GET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_all, (reg32))
#define multi_sgmii_reg_phya_65_GET_rg_tbi_10_rx_reverse_pma(reg32) REG_FLD_GET(multi_sgmii_reg_phya_65_FLD_rg_tbi_10_rx_reverse_pma, (reg32))
#define multi_sgmii_reg_phya_65_GET_rg_tbi_20_rx_reverse_pma(reg32) REG_FLD_GET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_pma, (reg32))
#define multi_sgmii_reg_phya_65_GET_rg_tbi_tx_reverse_10b_pma(reg32) REG_FLD_GET(multi_sgmii_reg_phya_65_FLD_rg_tbi_tx_reverse_10b_pma, (reg32))
#define multi_sgmii_reg_phya_65_GET_rg_tbi_20_tx_reverse_pma(reg32) REG_FLD_GET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_pma, (reg32))

#define multi_sgmii_reg_interrupt_sel_GET_rg_hsgmii_phy_int(reg32) REG_FLD_GET(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int, (reg32))
#define multi_sgmii_reg_interrupt_sel_GET_rg_hsgmii_phy_int_force(reg32) REG_FLD_GET(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int_force, (reg32))
#define multi_sgmii_reg_interrupt_sel_GET_rg_rxer100_int(reg32) REG_FLD_GET(multi_sgmii_reg_interrupt_sel_FLD_rg_rxer100_int, (reg32))
#define multi_sgmii_reg_interrupt_sel_GET_rg_false_carrier_int(reg32) REG_FLD_GET(multi_sgmii_reg_interrupt_sel_FLD_rg_false_carrier_int, (reg32))
#define multi_sgmii_reg_interrupt_sel_GET_rg_symbol_error_int(reg32) REG_FLD_GET(multi_sgmii_reg_interrupt_sel_FLD_rg_symbol_error_int, (reg32))
#define multi_sgmii_reg_interrupt_sel_GET_rg_100fx_int(reg32)  REG_FLD_GET(multi_sgmii_reg_interrupt_sel_FLD_rg_100fx_int, (reg32))
#define multi_sgmii_reg_interrupt_sel_GET_rg_hsgmii_pcs_int(reg32) REG_FLD_GET(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_pcs_int, (reg32))

#define multi_sgmii_reg_probe_sel_GET_rg_probe_clk_sel(reg32)  REG_FLD_GET(multi_sgmii_reg_probe_sel_FLD_rg_probe_clk_sel, (reg32))
#define multi_sgmii_reg_probe_sel_GET_rg_hsgmii_probe_port_sel(reg32) REG_FLD_GET(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_port_sel, (reg32))
#define multi_sgmii_reg_probe_sel_GET_rg_dwn_shift_probe_sel(reg32) REG_FLD_GET(multi_sgmii_reg_probe_sel_FLD_rg_dwn_shift_probe_sel, (reg32))
#define multi_sgmii_reg_probe_sel_GET_rg_hsgmii_clk_probe_sel(reg32) REG_FLD_GET(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_clk_probe_sel, (reg32))
#define multi_sgmii_reg_probe_sel_GET_rg_100fx_probe_sel(reg32) REG_FLD_GET(multi_sgmii_reg_probe_sel_FLD_rg_100fx_probe_sel, (reg32))
#define multi_sgmii_reg_probe_sel_GET_rg_hsgmii_probe_sel(reg32) REG_FLD_GET(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_sel, (reg32))

#define multi_sgmii_fpga_mode_control_GET_rg_hwtrap_100fx_mux_en(reg32) REG_FLD_GET(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux_en, (reg32))
#define multi_sgmii_fpga_mode_control_GET_rg_hwtrap_100fx_mux(reg32) REG_FLD_GET(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux, (reg32))
#define multi_sgmii_fpga_mode_control_GET_rg_hwtrap_100fx_en(reg32) REG_FLD_GET(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_en, (reg32))

#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_write_enable_or(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_or, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_read_enable_or(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_or, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_write_enable_and(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_and, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_read_enable_and(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_and, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_aempty_cnt_clear(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_aempty_cnt_clear, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_empty_cnt_clear(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_empty_cnt_clear, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_afull_cnt_clear(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_afull_cnt_clear, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_full_cnt_clear(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_full_cnt_clear, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_w_threshold(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_w_threshold, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_r_threshold(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_r_threshold, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_rclear(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_rclear, (reg32))
#define multi_sgmii_async_fifo_control_GET_rg_hsgmii_async_fifo_wclear(reg32) REG_FLD_GET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_wclear, (reg32))

#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_rfifo_cnt(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_rfifo_cnt, (reg32))
#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_wfifo_cnt(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_wfifo_cnt, (reg32))
#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_w_ready(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_w_ready, (reg32))
#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_r_ready(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_r_ready, (reg32))
#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_aempty(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_aempty, (reg32))
#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_empty(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_empty, (reg32))
#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_afull(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_afull, (reg32))
#define multi_sgmii_ro_phya_afifo_GET_ro_hsgmii_async_fifo_full(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_full, (reg32))

#define multi_sgmii_ro_phya_afifo_cnt_0_GET_ro_hsgmii_async_fifo_full_cnt(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_cnt_0_FLD_ro_hsgmii_async_fifo_full_cnt, (reg32))

#define multi_sgmii_ro_phya_afifo_cnt_1_GET_ro_hsgmii_async_fifo_afull_cnt(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_cnt_1_FLD_ro_hsgmii_async_fifo_afull_cnt, (reg32))

#define multi_sgmii_ro_phya_afifo_cnt_2_GET_ro_hsgmii_async_fifo_empty_cnt(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_cnt_2_FLD_ro_hsgmii_async_fifo_empty_cnt, (reg32))

#define multi_sgmii_ro_phya_afifo_cnt_3_GET_ro_hsgmii_async_fifo_aempty_cnt(reg32) REG_FLD_GET(multi_sgmii_ro_phya_afifo_cnt_3_FLD_ro_hsgmii_async_fifo_aempty_cnt, (reg32))

#define msg_led_ctrl_0_GET_rg_led_timing_test(reg32)           REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led_timing_test, (reg32))
#define msg_led_ctrl_0_GET_rg_dwn_shift_en(reg32)              REG_FLD_GET(msg_led_ctrl_0_FLD_rg_dwn_shift_en, (reg32))
#define msg_led_ctrl_0_GET_rg_dwn_shift_blk_en(reg32)          REG_FLD_GET(msg_led_ctrl_0_FLD_rg_dwn_shift_blk_en, (reg32))
#define msg_led_ctrl_0_GET_rg_led_enhance(reg32)               REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led_enhance, (reg32))
#define msg_led_ctrl_0_GET_rg_led_mode(reg32)                  REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led_mode, (reg32))
#define msg_led_ctrl_0_GET_rg_led3_pol(reg32)                  REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led3_pol, (reg32))
#define msg_led_ctrl_0_GET_rg_led2_pol(reg32)                  REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led2_pol, (reg32))
#define msg_led_ctrl_0_GET_rg_led1_pol(reg32)                  REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led1_pol, (reg32))
#define msg_led_ctrl_0_GET_rg_led0_pol(reg32)                  REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led0_pol, (reg32))
#define msg_led_ctrl_0_GET_rg_led3_en(reg32)                   REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led3_en, (reg32))
#define msg_led_ctrl_0_GET_rg_led2_en(reg32)                   REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led2_en, (reg32))
#define msg_led_ctrl_0_GET_rg_led1_en(reg32)                   REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led1_en, (reg32))
#define msg_led_ctrl_0_GET_rg_led0_en(reg32)                   REG_FLD_GET(msg_led_ctrl_0_FLD_rg_led0_en, (reg32))

#define msg_led_ctrl_1_GET_rg_led_blk_dur(reg32)               REG_FLD_GET(msg_led_ctrl_1_FLD_rg_led_blk_dur, (reg32))
#define msg_led_ctrl_1_GET_rg_led_on_dur(reg32)                REG_FLD_GET(msg_led_ctrl_1_FLD_rg_led_on_dur, (reg32))

#define msg_led_ctrl_2_GET_rg_led1_blk_mask(reg32)             REG_FLD_GET(msg_led_ctrl_2_FLD_rg_led1_blk_mask, (reg32))
#define msg_led_ctrl_2_GET_rg_led1_on_mask(reg32)              REG_FLD_GET(msg_led_ctrl_2_FLD_rg_led1_on_mask, (reg32))
#define msg_led_ctrl_2_GET_rg_led0_blk_mask(reg32)             REG_FLD_GET(msg_led_ctrl_2_FLD_rg_led0_blk_mask, (reg32))
#define msg_led_ctrl_2_GET_rg_led0_on_mask(reg32)              REG_FLD_GET(msg_led_ctrl_2_FLD_rg_led0_on_mask, (reg32))

#define msg_led_ctrl_3_GET_rg_led3_blk_mask(reg32)             REG_FLD_GET(msg_led_ctrl_3_FLD_rg_led3_blk_mask, (reg32))
#define msg_led_ctrl_3_GET_rg_led3_on_mask(reg32)              REG_FLD_GET(msg_led_ctrl_3_FLD_rg_led3_on_mask, (reg32))
#define msg_led_ctrl_3_GET_rg_led2_blk_mask(reg32)             REG_FLD_GET(msg_led_ctrl_3_FLD_rg_led2_blk_mask, (reg32))
#define msg_led_ctrl_3_GET_rg_led2_on_mask(reg32)              REG_FLD_GET(msg_led_ctrl_3_FLD_rg_led2_on_mask, (reg32))

#define msg_led_sts_0_GET_ro_led_blk_event(reg32)              REG_FLD_GET(msg_led_sts_0_FLD_ro_led_blk_event, (reg32))
#define msg_led_sts_0_GET_ro_led_on_event(reg32)               REG_FLD_GET(msg_led_sts_0_FLD_ro_led_on_event, (reg32))

#define msg_rx_sts_0_GET_ro_k28_1_cnt(reg32)                   REG_FLD_GET(msg_rx_sts_0_FLD_ro_k28_1_cnt, (reg32))

#define msg_rx_sts_1_GET_ro_port0_k28_1_cnt(reg32)             REG_FLD_GET(msg_rx_sts_1_FLD_ro_port0_k28_1_cnt, (reg32))

#define msg_rx_sts_2_GET_ro_port1_k28_1_cnt(reg32)             REG_FLD_GET(msg_rx_sts_2_FLD_ro_port1_k28_1_cnt, (reg32))

#define msg_rx_sts_3_GET_ro_port2_k28_1_cnt(reg32)             REG_FLD_GET(msg_rx_sts_3_FLD_ro_port2_k28_1_cnt, (reg32))

#define msg_rx_sts_4_GET_ro_port3_k28_1_cnt(reg32)             REG_FLD_GET(msg_rx_sts_4_FLD_ro_port3_k28_1_cnt, (reg32))

#define msg_rx_sts_5_GET_ro_rd_out_3b_neg(reg32)               REG_FLD_GET(msg_rx_sts_5_FLD_ro_rd_out_3b_neg, (reg32))
#define msg_rx_sts_5_GET_ro_rd_out_3b_pos(reg32)               REG_FLD_GET(msg_rx_sts_5_FLD_ro_rd_out_3b_pos, (reg32))
#define msg_rx_sts_5_GET_ro_rd_out_5b_neg(reg32)               REG_FLD_GET(msg_rx_sts_5_FLD_ro_rd_out_5b_neg, (reg32))
#define msg_rx_sts_5_GET_ro_rd_out_5b_pos(reg32)               REG_FLD_GET(msg_rx_sts_5_FLD_ro_rd_out_5b_pos, (reg32))
#define msg_rx_sts_5_GET_ro_dp3_n(reg32)                       REG_FLD_GET(msg_rx_sts_5_FLD_ro_dp3_n, (reg32))
#define msg_rx_sts_5_GET_ro_dp3_p(reg32)                       REG_FLD_GET(msg_rx_sts_5_FLD_ro_dp3_p, (reg32))
#define msg_rx_sts_5_GET_ro_dp5_n(reg32)                       REG_FLD_GET(msg_rx_sts_5_FLD_ro_dp5_n, (reg32))
#define msg_rx_sts_5_GET_ro_dp5_p(reg32)                       REG_FLD_GET(msg_rx_sts_5_FLD_ro_dp5_p, (reg32))

#define msg_rx_sts_6_GET_ro_except_okn(reg32)                  REG_FLD_GET(msg_rx_sts_6_FLD_ro_except_okn, (reg32))
#define msg_rx_sts_6_GET_ro_except_err(reg32)                  REG_FLD_GET(msg_rx_sts_6_FLD_ro_except_err, (reg32))
#define msg_rx_sts_6_GET_ro_except_negi(reg32)                 REG_FLD_GET(msg_rx_sts_6_FLD_ro_except_negi, (reg32))
#define msg_rx_sts_6_GET_ro_except_posi(reg32)                 REG_FLD_GET(msg_rx_sts_6_FLD_ro_except_posi, (reg32))
#define msg_rx_sts_6_GET_ro_except_neg(reg32)                  REG_FLD_GET(msg_rx_sts_6_FLD_ro_except_neg, (reg32))
#define msg_rx_sts_6_GET_ro_except_pos(reg32)                  REG_FLD_GET(msg_rx_sts_6_FLD_ro_except_pos, (reg32))
#define msg_rx_sts_6_GET_ro_except5_n(reg32)                   REG_FLD_GET(msg_rx_sts_6_FLD_ro_except5_n, (reg32))
#define msg_rx_sts_6_GET_ro_except5_p(reg32)                   REG_FLD_GET(msg_rx_sts_6_FLD_ro_except5_p, (reg32))

#define msg_rx_sts_7_GET_ro_carrier_reg(reg32)                 REG_FLD_GET(msg_rx_sts_7_FLD_ro_carrier_reg, (reg32))
#define msg_rx_sts_7_GET_ro_rd_out_5b(reg32)                   REG_FLD_GET(msg_rx_sts_7_FLD_ro_rd_out_5b, (reg32))
#define msg_rx_sts_7_GET_ro_rd_out_5b_negx(reg32)              REG_FLD_GET(msg_rx_sts_7_FLD_ro_rd_out_5b_negx, (reg32))
#define msg_rx_sts_7_GET_ro_rd_out_5b_posx(reg32)              REG_FLD_GET(msg_rx_sts_7_FLD_ro_rd_out_5b_posx, (reg32))
#define msg_rx_sts_7_GET_ro_rd_out_3b_negx(reg32)              REG_FLD_GET(msg_rx_sts_7_FLD_ro_rd_out_3b_negx, (reg32))
#define msg_rx_sts_7_GET_ro_rd_out_3b_posx(reg32)              REG_FLD_GET(msg_rx_sts_7_FLD_ro_rd_out_3b_posx, (reg32))
#define msg_rx_sts_7_GET_ro_data_out_3b_index(reg32)           REG_FLD_GET(msg_rx_sts_7_FLD_ro_data_out_3b_index, (reg32))

#define msg_rx_sts_8_GET_ro_error_3b_sc_neg(reg32)             REG_FLD_GET(msg_rx_sts_8_FLD_ro_error_3b_sc_neg, (reg32))
#define msg_rx_sts_8_GET_ro_error_3b_sc_pos(reg32)             REG_FLD_GET(msg_rx_sts_8_FLD_ro_error_3b_sc_pos, (reg32))
#define msg_rx_sts_8_GET_ro_error_3b_sc(reg32)                 REG_FLD_GET(msg_rx_sts_8_FLD_ro_error_3b_sc, (reg32))
#define msg_rx_sts_8_GET_ro_error_kout_5b_neg(reg32)           REG_FLD_GET(msg_rx_sts_8_FLD_ro_error_kout_5b_neg, (reg32))
#define msg_rx_sts_8_GET_ro_error_kout_5b_pos(reg32)           REG_FLD_GET(msg_rx_sts_8_FLD_ro_error_kout_5b_pos, (reg32))
#define msg_rx_sts_8_GET_ro_error_kout_5b(reg32)               REG_FLD_GET(msg_rx_sts_8_FLD_ro_error_kout_5b, (reg32))

#define msg_rx_sts_9_GET_ro_error_5b_neg(reg32)                REG_FLD_GET(msg_rx_sts_9_FLD_ro_error_5b_neg, (reg32))
#define msg_rx_sts_9_GET_ro_error_5b_pos(reg32)                REG_FLD_GET(msg_rx_sts_9_FLD_ro_error_5b_pos, (reg32))
#define msg_rx_sts_9_GET_ro_error_5b(reg32)                    REG_FLD_GET(msg_rx_sts_9_FLD_ro_error_5b, (reg32))
#define msg_rx_sts_9_GET_ro_error_3b(reg32)                    REG_FLD_GET(msg_rx_sts_9_FLD_ro_error_3b, (reg32))

#define msg_rx_sts_10_GET_ro_data_out_3b(reg32)                REG_FLD_GET(msg_rx_sts_10_FLD_ro_data_out_3b, (reg32))
#define msg_rx_sts_10_GET_ro_data_out_5b(reg32)                REG_FLD_GET(msg_rx_sts_10_FLD_ro_data_out_5b, (reg32))

#define msg_rx_sts_11_GET_ro_disp_err(reg32)                   REG_FLD_GET(msg_rx_sts_11_FLD_ro_disp_err, (reg32))
#define msg_rx_sts_11_GET_ro_char_err(reg32)                   REG_FLD_GET(msg_rx_sts_11_FLD_ro_char_err, (reg32))
#define msg_rx_sts_11_GET_ro_kout_3b_pos(reg32)                REG_FLD_GET(msg_rx_sts_11_FLD_ro_kout_3b_pos, (reg32))
#define msg_rx_sts_11_GET_ro_kout_3b_neg(reg32)                REG_FLD_GET(msg_rx_sts_11_FLD_ro_kout_3b_neg, (reg32))
#define msg_rx_sts_11_GET_ro_kout_5b_pos(reg32)                REG_FLD_GET(msg_rx_sts_11_FLD_ro_kout_5b_pos, (reg32))
#define msg_rx_sts_11_GET_ro_kout_5b_neg(reg32)                REG_FLD_GET(msg_rx_sts_11_FLD_ro_kout_5b_neg, (reg32))

#define msg_rx_sts_12_GET_ro_port3_sync_lost(reg32)            REG_FLD_GET(msg_rx_sts_12_FLD_ro_port3_sync_lost, (reg32))
#define msg_rx_sts_12_GET_ro_port2_sync_lost(reg32)            REG_FLD_GET(msg_rx_sts_12_FLD_ro_port2_sync_lost, (reg32))
#define msg_rx_sts_12_GET_ro_port1_sync_lost(reg32)            REG_FLD_GET(msg_rx_sts_12_FLD_ro_port1_sync_lost, (reg32))
#define msg_rx_sts_12_GET_ro_port0_sync_lost(reg32)            REG_FLD_GET(msg_rx_sts_12_FLD_ro_port0_sync_lost, (reg32))
#define msg_rx_sts_12_GET_ro_sync_int(reg32)                   REG_FLD_GET(msg_rx_sts_12_FLD_ro_sync_int, (reg32))
#define msg_rx_sts_12_GET_ro_rd_out(reg32)                     REG_FLD_GET(msg_rx_sts_12_FLD_ro_rd_out, (reg32))
#define msg_rx_sts_12_GET_ro_align_det(reg32)                  REG_FLD_GET(msg_rx_sts_12_FLD_ro_align_det, (reg32))

#define msg_rx_sts_13_GET_ro_data_align(reg32)                 REG_FLD_GET(msg_rx_sts_13_FLD_ro_data_align, (reg32))

#define msg_rx_sts_14_GET_ro_par(reg32)                        REG_FLD_GET(msg_rx_sts_14_FLD_ro_par, (reg32))

#define msg_rx_sts_15_GET_ro_lnk_sts_p3(reg32)                 REG_FLD_GET(msg_rx_sts_15_FLD_ro_lnk_sts_p3, (reg32))
#define msg_rx_sts_15_GET_ro_speed_sts_p3(reg32)               REG_FLD_GET(msg_rx_sts_15_FLD_ro_speed_sts_p3, (reg32))
#define msg_rx_sts_15_GET_ro_lnk_sts_p2(reg32)                 REG_FLD_GET(msg_rx_sts_15_FLD_ro_lnk_sts_p2, (reg32))
#define msg_rx_sts_15_GET_ro_speed_sts_p2(reg32)               REG_FLD_GET(msg_rx_sts_15_FLD_ro_speed_sts_p2, (reg32))
#define msg_rx_sts_15_GET_ro_lnk_sts_p1(reg32)                 REG_FLD_GET(msg_rx_sts_15_FLD_ro_lnk_sts_p1, (reg32))
#define msg_rx_sts_15_GET_ro_speed_sts_p1(reg32)               REG_FLD_GET(msg_rx_sts_15_FLD_ro_speed_sts_p1, (reg32))
#define msg_rx_sts_15_GET_ro_lnk_sts_p0(reg32)                 REG_FLD_GET(msg_rx_sts_15_FLD_ro_lnk_sts_p0, (reg32))
#define msg_rx_sts_15_GET_ro_speed_sts_p0(reg32)               REG_FLD_GET(msg_rx_sts_15_FLD_ro_speed_sts_p0, (reg32))

#define msg_rx_sts_16_GET_ro_p3_rx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p3_rx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p2_rx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p2_rx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p1_rx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p1_rx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p0_rx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p0_rx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p3_tx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p3_tx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p2_tx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p2_tx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p1_tx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p1_tx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p0_tx_eee_flag_latch(reg32)       REG_FLD_GET(msg_rx_sts_16_FLD_ro_p0_tx_eee_flag_latch, (reg32))
#define msg_rx_sts_16_GET_ro_sig_detect_down_latch(reg32)      REG_FLD_GET(msg_rx_sts_16_FLD_ro_sig_detect_down_latch, (reg32))
#define msg_rx_sts_16_GET_ro_sig_detect_up_latch(reg32)        REG_FLD_GET(msg_rx_sts_16_FLD_ro_sig_detect_up_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p3_link_down_latch(reg32)         REG_FLD_GET(msg_rx_sts_16_FLD_ro_p3_link_down_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p3_link_up_latch(reg32)           REG_FLD_GET(msg_rx_sts_16_FLD_ro_p3_link_up_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p2_link_down_latch(reg32)         REG_FLD_GET(msg_rx_sts_16_FLD_ro_p2_link_down_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p2_link_up_latch(reg32)           REG_FLD_GET(msg_rx_sts_16_FLD_ro_p2_link_up_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p1_link_down_latch(reg32)         REG_FLD_GET(msg_rx_sts_16_FLD_ro_p1_link_down_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p1_link_up_latch(reg32)           REG_FLD_GET(msg_rx_sts_16_FLD_ro_p1_link_up_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p0_link_down_latch(reg32)         REG_FLD_GET(msg_rx_sts_16_FLD_ro_p0_link_down_latch, (reg32))
#define msg_rx_sts_16_GET_ro_p0_link_up_latch(reg32)           REG_FLD_GET(msg_rx_sts_16_FLD_ro_p0_link_up_latch, (reg32))

#define msg_rx_sts_17_GET_rg_p3_rx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p3_rx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p2_rx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p2_rx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p1_rx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p1_rx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p0_rx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p0_rx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p3_tx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p3_tx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p2_tx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p2_tx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p1_tx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p1_tx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p0_tx_eee_int_clr(reg32)          REG_FLD_GET(msg_rx_sts_17_FLD_rg_p0_tx_eee_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_sig_detect_down_int_clr(reg32)    REG_FLD_GET(msg_rx_sts_17_FLD_rg_sig_detect_down_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_sig_detect_up_int_clr(reg32)      REG_FLD_GET(msg_rx_sts_17_FLD_rg_sig_detect_up_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p3_link_down_int_clr(reg32)       REG_FLD_GET(msg_rx_sts_17_FLD_rg_p3_link_down_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p3_link_up_int_clr(reg32)         REG_FLD_GET(msg_rx_sts_17_FLD_rg_p3_link_up_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p2_link_down_int_clr(reg32)       REG_FLD_GET(msg_rx_sts_17_FLD_rg_p2_link_down_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p2_link_up_int_clr(reg32)         REG_FLD_GET(msg_rx_sts_17_FLD_rg_p2_link_up_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p1_link_down_int_clr(reg32)       REG_FLD_GET(msg_rx_sts_17_FLD_rg_p1_link_down_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p1_link_up_int_clr(reg32)         REG_FLD_GET(msg_rx_sts_17_FLD_rg_p1_link_up_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p0_link_down_int_clr(reg32)       REG_FLD_GET(msg_rx_sts_17_FLD_rg_p0_link_down_int_clr, (reg32))
#define msg_rx_sts_17_GET_rg_p0_link_up_int_clr(reg32)         REG_FLD_GET(msg_rx_sts_17_FLD_rg_p0_link_up_int_clr, (reg32))

#define msg_rx_sts_18_GET_p3_sgmii_is_10(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p3_sgmii_is_10, (reg32))
#define msg_rx_sts_18_GET_p3_sgmii_is_100(reg32)               REG_FLD_GET(msg_rx_sts_18_FLD_p3_sgmii_is_100, (reg32))
#define msg_rx_sts_18_GET_p3_sgmii_is_1g(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p3_sgmii_is_1g, (reg32))
#define msg_rx_sts_18_GET_p2_sgmii_is_10(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p2_sgmii_is_10, (reg32))
#define msg_rx_sts_18_GET_p2_sgmii_is_100(reg32)               REG_FLD_GET(msg_rx_sts_18_FLD_p2_sgmii_is_100, (reg32))
#define msg_rx_sts_18_GET_p2_sgmii_is_1g(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p2_sgmii_is_1g, (reg32))
#define msg_rx_sts_18_GET_p1_sgmii_is_10(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p1_sgmii_is_10, (reg32))
#define msg_rx_sts_18_GET_p1_sgmii_is_100(reg32)               REG_FLD_GET(msg_rx_sts_18_FLD_p1_sgmii_is_100, (reg32))
#define msg_rx_sts_18_GET_p1_sgmii_is_1g(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p1_sgmii_is_1g, (reg32))
#define msg_rx_sts_18_GET_p0_sgmii_is_10(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p0_sgmii_is_10, (reg32))
#define msg_rx_sts_18_GET_p0_sgmii_is_100(reg32)               REG_FLD_GET(msg_rx_sts_18_FLD_p0_sgmii_is_100, (reg32))
#define msg_rx_sts_18_GET_p0_sgmii_is_1g(reg32)                REG_FLD_GET(msg_rx_sts_18_FLD_p0_sgmii_is_1g, (reg32))

#define msg_rx_sts_19_GET_p1_lp_ability(reg32)                 REG_FLD_GET(msg_rx_sts_19_FLD_p1_lp_ability, (reg32))
#define msg_rx_sts_19_GET_p0_lp_ability(reg32)                 REG_FLD_GET(msg_rx_sts_19_FLD_p0_lp_ability, (reg32))

#define msg_rx_sts_20_GET_p3_lp_ability(reg32)                 REG_FLD_GET(msg_rx_sts_20_FLD_p3_lp_ability, (reg32))
#define msg_rx_sts_20_GET_p2_lp_ability(reg32)                 REG_FLD_GET(msg_rx_sts_20_FLD_p2_lp_ability, (reg32))

#define cl22_status_0_GET_ro_cl22_reset_wrtie_flag(reg32)      REG_FLD_GET(cl22_status_0_FLD_ro_cl22_reset_wrtie_flag, (reg32))
#define cl22_status_0_GET_ro_cl22_wrtie_flag(reg32)            REG_FLD_GET(cl22_status_0_FLD_ro_cl22_wrtie_flag, (reg32))

#define cl22_status_1_GET_rg_cl22_reset_wrtie_flag_clr(reg32)  REG_FLD_GET(cl22_status_1_FLD_rg_cl22_reset_wrtie_flag_clr, (reg32))
#define cl22_status_1_GET_rg_cl22_wrtie_flag_clr(reg32)        REG_FLD_GET(cl22_status_1_FLD_rg_cl22_wrtie_flag_clr, (reg32))

#define msg_rx_sts_21_GET_rg_rx_eee_flag_lev_p3_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p3_clr, (reg32))
#define msg_rx_sts_21_GET_rg_rx_eee_flag_lev_p2_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p2_clr, (reg32))
#define msg_rx_sts_21_GET_rg_rx_eee_flag_lev_p1_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p1_clr, (reg32))
#define msg_rx_sts_21_GET_rg_rx_eee_flag_lev_p0_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p0_clr, (reg32))
#define msg_rx_sts_21_GET_rg_tx_eee_flag_lev_p3_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p3_clr, (reg32))
#define msg_rx_sts_21_GET_rg_tx_eee_flag_lev_p2_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p2_clr, (reg32))
#define msg_rx_sts_21_GET_rg_tx_eee_flag_lev_p1_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p1_clr, (reg32))
#define msg_rx_sts_21_GET_rg_tx_eee_flag_lev_p0_clr(reg32)     REG_FLD_GET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p0_clr, (reg32))

#define msg_rx_ctrl_1_GET_rg_force_port3_cnt(reg32)            REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port3_cnt, (reg32))
#define msg_rx_ctrl_1_GET_rg_force_port2_cnt(reg32)            REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port2_cnt, (reg32))
#define msg_rx_ctrl_1_GET_rg_force_port1_cnt(reg32)            REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port1_cnt, (reg32))
#define msg_rx_ctrl_1_GET_rg_force_port0_cnt(reg32)            REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port0_cnt, (reg32))
#define msg_rx_ctrl_1_GET_rg_force_port3_cnt_sel(reg32)        REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port3_cnt_sel, (reg32))
#define msg_rx_ctrl_1_GET_rg_force_port2_cnt_sel(reg32)        REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port2_cnt_sel, (reg32))
#define msg_rx_ctrl_1_GET_rg_force_port1_cnt_sel(reg32)        REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port1_cnt_sel, (reg32))
#define msg_rx_ctrl_1_GET_rg_force_port0_cnt_sel(reg32)        REG_FLD_GET(msg_rx_ctrl_1_FLD_rg_force_port0_cnt_sel, (reg32))

#define msg_rx_ctrl_2_GET_rg_force_port3_sync(reg32)           REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port3_sync, (reg32))
#define msg_rx_ctrl_2_GET_rg_force_port2_sync(reg32)           REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port2_sync, (reg32))
#define msg_rx_ctrl_2_GET_rg_force_port1_sync(reg32)           REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port1_sync, (reg32))
#define msg_rx_ctrl_2_GET_rg_force_port0_sync(reg32)           REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port0_sync, (reg32))
#define msg_rx_ctrl_2_GET_rg_force_port3_sync_sel(reg32)       REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port3_sync_sel, (reg32))
#define msg_rx_ctrl_2_GET_rg_force_port2_sync_sel(reg32)       REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port2_sync_sel, (reg32))
#define msg_rx_ctrl_2_GET_rg_force_port1_sync_sel(reg32)       REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port1_sync_sel, (reg32))
#define msg_rx_ctrl_2_GET_rg_force_port0_sync_sel(reg32)       REG_FLD_GET(msg_rx_ctrl_2_FLD_rg_force_port0_sync_sel, (reg32))

#define msg_rx_ctrl_3_GET_rg_k28_1_cnt_clr(reg32)              REG_FLD_GET(msg_rx_ctrl_3_FLD_rg_k28_1_cnt_clr, (reg32))
#define msg_rx_ctrl_3_GET_rg_port3_k28_1_cnt_clr(reg32)        REG_FLD_GET(msg_rx_ctrl_3_FLD_rg_port3_k28_1_cnt_clr, (reg32))
#define msg_rx_ctrl_3_GET_rg_port2_k28_1_cnt_clr(reg32)        REG_FLD_GET(msg_rx_ctrl_3_FLD_rg_port2_k28_1_cnt_clr, (reg32))
#define msg_rx_ctrl_3_GET_rg_port1_k28_1_cnt_clr(reg32)        REG_FLD_GET(msg_rx_ctrl_3_FLD_rg_port1_k28_1_cnt_clr, (reg32))
#define msg_rx_ctrl_3_GET_rg_port0_k28_1_cnt_clr(reg32)        REG_FLD_GET(msg_rx_ctrl_3_FLD_rg_port0_k28_1_cnt_clr, (reg32))

#define msg_rx_ctrl_4_GET_rg_qsgmii_rx500_pcs_div_ck_sel(reg32) REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_qsgmii_rx500_pcs_div_ck_sel, (reg32))
#define msg_rx_ctrl_4_GET_rg_p3_mac_rx_div_ck_reset_n(reg32)   REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_p3_mac_rx_div_ck_reset_n, (reg32))
#define msg_rx_ctrl_4_GET_rg_p2_mac_rx_div_ck_reset_n(reg32)   REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_p2_mac_rx_div_ck_reset_n, (reg32))
#define msg_rx_ctrl_4_GET_rg_p1_mac_rx_div_ck_reset_n(reg32)   REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_p1_mac_rx_div_ck_reset_n, (reg32))
#define msg_rx_ctrl_4_GET_rg_qsgmii_p3_rx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_qsgmii_p3_rx500_mac_div_ck_sel, (reg32))
#define msg_rx_ctrl_4_GET_rg_qsgmii_p2_rx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_qsgmii_p2_rx500_mac_div_ck_sel, (reg32))
#define msg_rx_ctrl_4_GET_rg_qsgmii_p1_rx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_qsgmii_p1_rx500_mac_div_ck_sel, (reg32))
#define msg_rx_ctrl_4_GET_rg_qsgmii_p0_rx500_mac_div_ck_sel(reg32) REG_FLD_GET(msg_rx_ctrl_4_FLD_rg_qsgmii_p0_rx500_mac_div_ck_sel, (reg32))

#define msg_rx_ctrl_5_GET_rg_fx_crs_test(reg32)                REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_fx_crs_test, (reg32))
#define msg_rx_ctrl_5_GET_rg_fx_col_test(reg32)                REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_fx_col_test, (reg32))
#define msg_rx_ctrl_5_GET_rg_hg_crs_test(reg32)                REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_hg_crs_test, (reg32))
#define msg_rx_ctrl_5_GET_rg_hg_col_test(reg32)                REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_hg_col_test, (reg32))
#define msg_rx_ctrl_5_GET_rg_crs_is_fx_mode(reg32)             REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_crs_is_fx_mode, (reg32))
#define msg_rx_ctrl_5_GET_rg_col_is_fx_mode(reg32)             REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_col_is_fx_mode, (reg32))
#define msg_rx_ctrl_5_GET_rg_fx_crs_sel(reg32)                 REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_fx_crs_sel, (reg32))
#define msg_rx_ctrl_5_GET_rg_fx_col_sel(reg32)                 REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_fx_col_sel, (reg32))
#define msg_rx_ctrl_5_GET_rg_fx_crs_test_mode(reg32)           REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_fx_crs_test_mode, (reg32))
#define msg_rx_ctrl_5_GET_rg_fx_col_test_mode(reg32)           REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_fx_col_test_mode, (reg32))
#define msg_rx_ctrl_5_GET_rg_hg_crs_test_mode(reg32)           REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_hg_crs_test_mode, (reg32))
#define msg_rx_ctrl_5_GET_rg_hg_col_test_mode(reg32)           REG_FLD_GET(msg_rx_ctrl_5_FLD_rg_hg_col_test_mode, (reg32))

#define msg_rx_lik_sts_0_GET_rg_eee2p5g_sts(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee2p5g_sts, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee1g_sts_p3(reg32)            REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p3, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee100_sts_p3(reg32)           REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p3, (reg32))
#define msg_rx_lik_sts_0_GET_rg_txfc_sts_p3(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p3, (reg32))
#define msg_rx_lik_sts_0_GET_rg_rxfc_sts_p3(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p3, (reg32))
#define msg_rx_lik_sts_0_GET_rg_dpx_sts_p3(reg32)              REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p3, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee1g_sts_p2(reg32)            REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p2, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee100_sts_p2(reg32)           REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p2, (reg32))
#define msg_rx_lik_sts_0_GET_rg_txfc_sts_p2(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p2, (reg32))
#define msg_rx_lik_sts_0_GET_rg_rxfc_sts_p2(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p2, (reg32))
#define msg_rx_lik_sts_0_GET_rg_dpx_sts_p2(reg32)              REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p2, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee1g_sts_p1(reg32)            REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p1, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee100_sts_p1(reg32)           REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p1, (reg32))
#define msg_rx_lik_sts_0_GET_rg_txfc_sts_p1(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p1, (reg32))
#define msg_rx_lik_sts_0_GET_rg_rxfc_sts_p1(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p1, (reg32))
#define msg_rx_lik_sts_0_GET_rg_dpx_sts_p1(reg32)              REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p1, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee1g_sts_p0(reg32)            REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p0, (reg32))
#define msg_rx_lik_sts_0_GET_rg_eee100_sts_p0(reg32)           REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p0, (reg32))
#define msg_rx_lik_sts_0_GET_rg_txfc_sts_p0(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p0, (reg32))
#define msg_rx_lik_sts_0_GET_rg_rxfc_sts_p0(reg32)             REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p0, (reg32))
#define msg_rx_lik_sts_0_GET_rg_dpx_sts_p0(reg32)              REG_FLD_GET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p0, (reg32))

#define msg_rx_lik_sts_1_GET_rg_pause_sts_p3(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p3, (reg32))
#define msg_rx_lik_sts_1_GET_rg_pause_sts_p2(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p2, (reg32))
#define msg_rx_lik_sts_1_GET_rg_pause_sts_p1(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p1, (reg32))
#define msg_rx_lik_sts_1_GET_rg_pause_sts_p0(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p0, (reg32))
#define msg_rx_lik_sts_1_GET_rg_lp_an_cap_p3(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p3, (reg32))
#define msg_rx_lik_sts_1_GET_rg_lp_an_cap_p2(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p2, (reg32))
#define msg_rx_lik_sts_1_GET_rg_lp_an_cap_p1(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p1, (reg32))
#define msg_rx_lik_sts_1_GET_rg_lp_an_cap_p0(reg32)            REG_FLD_GET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p0, (reg32))

#define msg_rx_lik_sts_2_GET_rg_rxfc_an_bypass_p3(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p3, (reg32))
#define msg_rx_lik_sts_2_GET_rg_rxfc_an_bypass_p2(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p2, (reg32))
#define msg_rx_lik_sts_2_GET_rg_rxfc_an_bypass_p1(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p1, (reg32))
#define msg_rx_lik_sts_2_GET_rg_rxfc_an_bypass_p0(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p0, (reg32))
#define msg_rx_lik_sts_2_GET_rg_txfc_an_bypass_p3(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p3, (reg32))
#define msg_rx_lik_sts_2_GET_rg_txfc_an_bypass_p2(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p2, (reg32))
#define msg_rx_lik_sts_2_GET_rg_txfc_an_bypass_p1(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p1, (reg32))
#define msg_rx_lik_sts_2_GET_rg_txfc_an_bypass_p0(reg32)       REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p0, (reg32))
#define msg_rx_lik_sts_2_GET_rg_dpx_an_bypass_p3(reg32)        REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p3, (reg32))
#define msg_rx_lik_sts_2_GET_rg_dpx_an_bypass_p2(reg32)        REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p2, (reg32))
#define msg_rx_lik_sts_2_GET_rg_dpx_an_bypass_p1(reg32)        REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p1, (reg32))
#define msg_rx_lik_sts_2_GET_rg_dpx_an_bypass_p0(reg32)        REG_FLD_GET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p0, (reg32))

#define PHY_RX_FORCE_CTRL_0_GET_rg_force_rxer_sel(reg32)       REG_FLD_GET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxer_sel, (reg32))
#define PHY_RX_FORCE_CTRL_0_GET_rg_force_rxdv_sel(reg32)       REG_FLD_GET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxdv_sel, (reg32))
#define PHY_RX_FORCE_CTRL_0_GET_rg_force_rxc_sel(reg32)        REG_FLD_GET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxc_sel, (reg32))
#define PHY_RX_FORCE_CTRL_0_GET_rg_force_rxd_sel(reg32)        REG_FLD_GET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxd_sel, (reg32))

#define PHY_RX_FORCE_CTRL_1_GET_rg_force_rxd_h(reg32)          REG_FLD_GET(PHY_RX_FORCE_CTRL_1_FLD_rg_force_rxd_h, (reg32))

#define PHY_RX_FORCE_CTRL_2_GET_rg_force_rxd_l(reg32)          REG_FLD_GET(PHY_RX_FORCE_CTRL_2_FLD_rg_force_rxd_l, (reg32))

#define PHY_RX_FORCE_CTRL_3_GET_rg_force_rxer(reg32)           REG_FLD_GET(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxer, (reg32))
#define PHY_RX_FORCE_CTRL_3_GET_rg_force_rxdv(reg32)           REG_FLD_GET(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxdv, (reg32))
#define PHY_RX_FORCE_CTRL_3_GET_rg_force_rxc(reg32)            REG_FLD_GET(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxc, (reg32))

#define Debug_REGISTER_0_GET_ro_dump_000(reg32)                REG_FLD_GET(Debug_REGISTER_0_FLD_ro_dump_000, (reg32))

#define Debug_REGISTER_1_GET_ro_dump_001(reg32)                REG_FLD_GET(Debug_REGISTER_1_FLD_ro_dump_001, (reg32))

#define Debug_REGISTER_2_GET_ro_dump_002(reg32)                REG_FLD_GET(Debug_REGISTER_2_FLD_ro_dump_002, (reg32))

#define Debug_REGISTER_3_GET_ro_dump_003(reg32)                REG_FLD_GET(Debug_REGISTER_3_FLD_ro_dump_003, (reg32))

#define Debug_REGISTER_4_GET_ro_dump_004(reg32)                REG_FLD_GET(Debug_REGISTER_4_FLD_ro_dump_004, (reg32))

#define Debug_REGISTER_5_GET_ro_dump_005(reg32)                REG_FLD_GET(Debug_REGISTER_5_FLD_ro_dump_005, (reg32))

#define Debug_REGISTER_6_GET_ro_dump_006(reg32)                REG_FLD_GET(Debug_REGISTER_6_FLD_ro_dump_006, (reg32))

#define Debug_REGISTER_7_GET_ro_dump_007(reg32)                REG_FLD_GET(Debug_REGISTER_7_FLD_ro_dump_007, (reg32))

#define Debug_REGISTER_8_GET_ro_dump_008(reg32)                REG_FLD_GET(Debug_REGISTER_8_FLD_ro_dump_008, (reg32))

#define Debug_REGISTER_9_GET_ro_dump_009(reg32)                REG_FLD_GET(Debug_REGISTER_9_FLD_ro_dump_009, (reg32))

#define Debug_REGISTER_10_GET_ro_dump_010(reg32)               REG_FLD_GET(Debug_REGISTER_10_FLD_ro_dump_010, (reg32))

#define Debug_REGISTER_11_GET_ro_dump_011(reg32)               REG_FLD_GET(Debug_REGISTER_11_FLD_ro_dump_011, (reg32))

#define Debug_REGISTER_12_GET_ro_dump_012(reg32)               REG_FLD_GET(Debug_REGISTER_12_FLD_ro_dump_012, (reg32))

#define Debug_REGISTER_13_GET_ro_dump_013(reg32)               REG_FLD_GET(Debug_REGISTER_13_FLD_ro_dump_013, (reg32))

#define Debug_REGISTER_14_GET_ro_dump_014(reg32)               REG_FLD_GET(Debug_REGISTER_14_FLD_ro_dump_014, (reg32))

#define Debug_REGISTER_15_GET_ro_dump_015(reg32)               REG_FLD_GET(Debug_REGISTER_15_FLD_ro_dump_015, (reg32))

#define Debug_REGISTER_16_GET_ro_dump_016(reg32)               REG_FLD_GET(Debug_REGISTER_16_FLD_ro_dump_016, (reg32))

#define Debug_REGISTER_17_GET_ro_dump_017(reg32)               REG_FLD_GET(Debug_REGISTER_17_FLD_ro_dump_017, (reg32))

#define Debug_REGISTER_18_GET_ro_dump_018(reg32)               REG_FLD_GET(Debug_REGISTER_18_FLD_ro_dump_018, (reg32))

#define Debug_REGISTER_19_GET_ro_dump_019(reg32)               REG_FLD_GET(Debug_REGISTER_19_FLD_ro_dump_019, (reg32))

#define Debug_REGISTER_20_GET_ro_dump_020(reg32)               REG_FLD_GET(Debug_REGISTER_20_FLD_ro_dump_020, (reg32))

#define Debug_REGISTER_21_GET_ro_dump_021(reg32)               REG_FLD_GET(Debug_REGISTER_21_FLD_ro_dump_021, (reg32))

#define Debug_REGISTER_22_GET_ro_dump_022(reg32)               REG_FLD_GET(Debug_REGISTER_22_FLD_ro_dump_022, (reg32))

#define Debug_REGISTER_23_GET_ro_dump_023(reg32)               REG_FLD_GET(Debug_REGISTER_23_FLD_ro_dump_023, (reg32))

#define Debug_REGISTER_24_GET_ro_dump_024(reg32)               REG_FLD_GET(Debug_REGISTER_24_FLD_ro_dump_024, (reg32))

#define Debug_REGISTER_25_GET_ro_dump_025(reg32)               REG_FLD_GET(Debug_REGISTER_25_FLD_ro_dump_025, (reg32))

#define Debug_REGISTER_26_GET_ro_dump_026(reg32)               REG_FLD_GET(Debug_REGISTER_26_FLD_ro_dump_026, (reg32))

#define Debug_REGISTER_27_GET_ro_dump_027(reg32)               REG_FLD_GET(Debug_REGISTER_27_FLD_ro_dump_027, (reg32))

#define Debug_REGISTER_28_GET_ro_dump_028(reg32)               REG_FLD_GET(Debug_REGISTER_28_FLD_ro_dump_028, (reg32))

#define Debug_REGISTER_29_GET_ro_dump_029(reg32)               REG_FLD_GET(Debug_REGISTER_29_FLD_ro_dump_029, (reg32))

#define Debug_REGISTER_30_GET_ro_dump_030(reg32)               REG_FLD_GET(Debug_REGISTER_30_FLD_ro_dump_030, (reg32))

#define Debug_REGISTER_31_GET_ro_dump_031(reg32)               REG_FLD_GET(Debug_REGISTER_31_FLD_ro_dump_031, (reg32))

#define Debug_REGISTER_CTRL_0_GET_rg_force_dump(reg32)         REG_FLD_GET(Debug_REGISTER_CTRL_0_FLD_rg_force_dump, (reg32))
#define Debug_REGISTER_CTRL_0_GET_rg_dump_sel(reg32)           REG_FLD_GET(Debug_REGISTER_CTRL_0_FLD_rg_dump_sel, (reg32))
#define Debug_REGISTER_CTRL_0_GET_rg_dump_inv(reg32)           REG_FLD_GET(Debug_REGISTER_CTRL_0_FLD_rg_dump_inv, (reg32))
#define Debug_REGISTER_CTRL_0_GET_rg_dump_clr(reg32)           REG_FLD_GET(Debug_REGISTER_CTRL_0_FLD_rg_dump_clr, (reg32))

#define Debug_REGISTER_DUMP_CNT_GET_ro_dump_marker(reg32)      REG_FLD_GET(Debug_REGISTER_DUMP_CNT_FLD_ro_dump_marker, (reg32))

#define msg_tx_ctrl_0_SET_rg_force_100fx_sigdet(reg32, val)    REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_force_100fx_sigdet, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_ssusb_hsgmii_sel(reg32, val)      REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_ssusb_hsgmii_sel, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_sgmii_mode_force(reg32, val)      REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_sgmii_mode_force, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_qsgmii_mode(reg32, val)           REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_qsgmii_mode, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_tx_data_re(reg32, val)            REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_tx_data_re, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_tx_data_rep4_sel(reg32, val)      REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_tx_data_rep4_sel, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_tx_data_rep2_sel(reg32, val)      REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_tx_data_rep2_sel, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_qsgmii_txclk_ena(reg32, val)      REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_qsgmii_txclk_ena, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_tx_free_cnt_clr(reg32, val)       REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_tx_free_cnt_clr, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_tx_gpii_rst(reg32, val)           REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_tx_gpii_rst, (reg32), (val))
#define msg_tx_ctrl_0_SET_rg_tx_5g_mode(reg32, val)            REG_FLD_SET(msg_tx_ctrl_0_FLD_rg_tx_5g_mode, (reg32), (val))

#define msg_tx_ctrl_1_SET_rg_qsgmii_tx500_pcs_div_ck_sel(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_qsgmii_tx500_pcs_div_ck_sel, (reg32), (val))
#define msg_tx_ctrl_1_SET_rg_p3_mac_tx_div_ck_reset_n(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_p3_mac_tx_div_ck_reset_n, (reg32), (val))
#define msg_tx_ctrl_1_SET_rg_p2_mac_tx_div_ck_reset_n(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_p2_mac_tx_div_ck_reset_n, (reg32), (val))
#define msg_tx_ctrl_1_SET_rg_p1_mac_tx_div_ck_reset_n(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_p1_mac_tx_div_ck_reset_n, (reg32), (val))
#define msg_tx_ctrl_1_SET_rg_qsgmii_p3_tx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_qsgmii_p3_tx500_mac_div_ck_sel, (reg32), (val))
#define msg_tx_ctrl_1_SET_rg_qsgmii_p2_tx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_qsgmii_p2_tx500_mac_div_ck_sel, (reg32), (val))
#define msg_tx_ctrl_1_SET_rg_qsgmii_p1_tx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_qsgmii_p1_tx500_mac_div_ck_sel, (reg32), (val))
#define msg_tx_ctrl_1_SET_rg_qsgmii_p0_tx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_tx_ctrl_1_FLD_rg_qsgmii_p0_tx500_mac_div_ck_sel, (reg32), (val))

#define msg_tx_ctrl_2_SET_rg_qsgmii_tx_kd_rev(reg32, val)      REG_FLD_SET(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_kd_rev, (reg32), (val))
#define msg_tx_ctrl_2_SET_rg_qsgmii_tx_idle_swap_en(reg32, val) REG_FLD_SET(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_idle_swap_en, (reg32), (val))
#define msg_tx_ctrl_2_SET_rg_qsgmii_tx_rev(reg32, val)         REG_FLD_SET(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_rev, (reg32), (val))
#define msg_tx_ctrl_2_SET_rg_qsgmii_20b_tx_hl_swap(reg32, val) REG_FLD_SET(msg_tx_ctrl_2_FLD_rg_qsgmii_20b_tx_hl_swap, (reg32), (val))

#define efuse_ctrl_dummy_SET_rg_efuse_tphy_auto_load_valid(reg32, val) REG_FLD_SET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_auto_load_valid, (reg32), (val))
#define efuse_ctrl_dummy_SET_rg_efuse_tphy_iext_intr_ctrl(reg32, val) REG_FLD_SET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_iext_intr_ctrl, (reg32), (val))
#define efuse_ctrl_dummy_SET_rg_efuse_tphy_tx_imp_sel(reg32, val) REG_FLD_SET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_tx_imp_sel, (reg32), (val))
#define efuse_ctrl_dummy_SET_rg_efuse_tphy_rx_imp_sel(reg32, val) REG_FLD_SET(efuse_ctrl_dummy_FLD_rg_efuse_tphy_rx_imp_sel, (reg32), (val))

#define rst_ctrl_0_SET_rg_hsgmii_mac_sw_rst_n(reg32, val)      REG_FLD_SET(rst_ctrl_0_FLD_rg_hsgmii_mac_sw_rst_n, (reg32), (val))
#define rst_ctrl_0_SET_rg_ssusb_ip_sw_rst(reg32, val)          REG_FLD_SET(rst_ctrl_0_FLD_rg_ssusb_ip_sw_rst, (reg32), (val))

#define INTERRUPT_EN_0_SET_rg_cl22_rg_phy_rst_cnt(reg32, val)  REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_cl22_rg_phy_rst_cnt, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_rx_eee_int_en(reg32, val)        REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_rx_eee_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_tx_eee_int_en(reg32, val)        REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_tx_eee_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_cl22_reset_wr_int_en(reg32, val) REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_cl22_reset_wr_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_cl22_wr_int_en(reg32, val)       REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_cl22_wr_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_sgmii_link_down_int_en(reg32, val) REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_sgmii_link_down_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_sgmii_link_up_int_en(reg32, val) REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_sgmii_link_up_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_sigdet_down_int_en(reg32, val)   REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_sigdet_down_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_sigdet_up_int_en(reg32, val)     REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_sigdet_up_int_en, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_pcs_int_en_3(reg32, val)         REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_3, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_pcs_int_en_2(reg32, val)         REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_2, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_pcs_int_en_1(reg32, val)         REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_1, (reg32), (val))
#define INTERRUPT_EN_0_SET_rg_pcs_int_en_0(reg32, val)         REG_FLD_SET(INTERRUPT_EN_0_FLD_rg_pcs_int_en_0, (reg32), (val))

#define SGMII_STS_CTRL_0_SET_rg_link_mode_p3(reg32, val)       REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p3, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_spd_mode_p3(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p3, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkdown_p3(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p3, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkup_p3(reg32, val)    REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p3, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_link_mode_p2(reg32, val)       REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p2, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_spd_mode_p2(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p2, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkdown_p2(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p2, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkup_p2(reg32, val)    REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p2, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_link_mode_p1(reg32, val)       REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p1, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_spd_mode_p1(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p1, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkdown_p1(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p1, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkup_p1(reg32, val)    REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p1, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_link_mode_p0(reg32, val)       REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_link_mode_p0, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_spd_mode_p0(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p0, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkdown_p0(reg32, val)  REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p0, (reg32), (val))
#define SGMII_STS_CTRL_0_SET_rg_force_linkup_p0(reg32, val)    REG_FLD_SET(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p0, (reg32), (val))

#define SGMII_STS_CTRL_1_SET_rg_tx_d_dummy(reg32, val)         REG_FLD_SET(SGMII_STS_CTRL_1_FLD_rg_tx_d_dummy, (reg32), (val))

#define SGMII_STS_CTRL_2_SET_rg_tx_c_dummy(reg32, val)         REG_FLD_SET(SGMII_STS_CTRL_2_FLD_rg_tx_c_dummy, (reg32), (val))

#define FPGA_STS_CTRL_SET_rg_fpga_ctrl_31_0(reg32, val)        REG_FLD_SET(FPGA_STS_CTRL_FLD_rg_fpga_ctrl_31_0, (reg32), (val))

#define SYSYEM_DEBUG_CTRL_SET_rg_debug_ctrl(reg32, val)        REG_FLD_SET(SYSYEM_DEBUG_CTRL_FLD_rg_debug_ctrl, (reg32), (val))

#define FPGA_BEN_CTRL_SET_rg_fpga_ctrl_ben(reg32, val)         REG_FLD_SET(FPGA_BEN_CTRL_FLD_rg_fpga_ctrl_ben, (reg32), (val))

#define CL22_SETTING_0_SET_rg_phy_id_1(reg32, val)             REG_FLD_SET(CL22_SETTING_0_FLD_rg_phy_id_1, (reg32), (val))
#define CL22_SETTING_0_SET_rg_phy_id_0(reg32, val)             REG_FLD_SET(CL22_SETTING_0_FLD_rg_phy_id_0, (reg32), (val))

#define CL22_SETTING_1_SET_rg_cl22_rg_status_clr(reg32, val)   REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22_rg_status_clr, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22_an_function_en(reg32, val)  REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22_an_function_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_15_en(reg32, val)       REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_15_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_14_en(reg32, val)       REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_14_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_13_en(reg32, val)       REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_13_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_12_en(reg32, val)       REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_12_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_11_en(reg32, val)       REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_11_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_10_en(reg32, val)       REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_10_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_9_en(reg32, val)        REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_9_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_8_en(reg32, val)        REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_8_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_7_en(reg32, val)        REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_7_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_6_en(reg32, val)        REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_6_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_5_en(reg32, val)        REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_5_en, (reg32), (val))
#define CL22_SETTING_1_SET_rg_cl22reg0_4_0_en(reg32, val)      REG_FLD_SET(CL22_SETTING_1_FLD_rg_cl22reg0_4_0_en, (reg32), (val))

#define CL22_SETTING_2_SET_rg_an_ability_en_reserve(reg32, val) REG_FLD_SET(CL22_SETTING_2_FLD_rg_an_ability_en_reserve, (reg32), (val))
#define CL22_SETTING_2_SET_rg_uni_direct_en_reserve(reg32, val) REG_FLD_SET(CL22_SETTING_2_FLD_rg_uni_direct_en_reserve, (reg32), (val))
#define CL22_SETTING_2_SET_rg_ext_cap(reg32, val)              REG_FLD_SET(CL22_SETTING_2_FLD_rg_ext_cap, (reg32), (val))
#define CL22_SETTING_2_SET_rg_jabber_det(reg32, val)           REG_FLD_SET(CL22_SETTING_2_FLD_rg_jabber_det, (reg32), (val))
#define CL22_SETTING_2_SET_rg_mf_pream_sup(reg32, val)         REG_FLD_SET(CL22_SETTING_2_FLD_rg_mf_pream_sup, (reg32), (val))
#define CL22_SETTING_2_SET_rg_ext_status(reg32, val)           REG_FLD_SET(CL22_SETTING_2_FLD_rg_ext_status, (reg32), (val))
#define CL22_SETTING_2_SET_rg_100base_t2_hdx(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_100base_t2_hdx, (reg32), (val))
#define CL22_SETTING_2_SET_rg_100base_t2_fdx(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_100base_t2_fdx, (reg32), (val))
#define CL22_SETTING_2_SET_rg_10mbps_hdx(reg32, val)           REG_FLD_SET(CL22_SETTING_2_FLD_rg_10mbps_hdx, (reg32), (val))
#define CL22_SETTING_2_SET_rg_10mbps_fdx(reg32, val)           REG_FLD_SET(CL22_SETTING_2_FLD_rg_10mbps_fdx, (reg32), (val))
#define CL22_SETTING_2_SET_rg_100base_x_hdx(reg32, val)        REG_FLD_SET(CL22_SETTING_2_FLD_rg_100base_x_hdx, (reg32), (val))
#define CL22_SETTING_2_SET_rg_100base_x_fdx(reg32, val)        REG_FLD_SET(CL22_SETTING_2_FLD_rg_100base_x_fdx, (reg32), (val))
#define CL22_SETTING_2_SET_rg_100base_t4(reg32, val)           REG_FLD_SET(CL22_SETTING_2_FLD_rg_100base_t4, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg7_15_en(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg7_15_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg7_14_en(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg7_14_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg7_13_en(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg7_13_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg7_12_en(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg7_12_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg7_11_en(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg7_11_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg7_10_0_en(reg32, val)     REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg7_10_0_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg4_15_en(reg32, val)       REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg4_15_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg4_13_12_en(reg32, val)    REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg4_13_12_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg4_8_7_en(reg32, val)      REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg4_8_7_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg4_6_en(reg32, val)        REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg4_6_en, (reg32), (val))
#define CL22_SETTING_2_SET_rg_cl22_rg4_5_en(reg32, val)        REG_FLD_SET(CL22_SETTING_2_FLD_rg_cl22_rg4_5_en, (reg32), (val))

#define CL22_STS_0_SET_ro_cl22reg0_15(reg32, val)              REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_15, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_14(reg32, val)              REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_14, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_13(reg32, val)              REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_13, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_12(reg32, val)              REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_12, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_11(reg32, val)              REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_11, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_10(reg32, val)              REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_10, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_9(reg32, val)               REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_9, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_8(reg32, val)               REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_8, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_7(reg32, val)               REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_7, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_6(reg32, val)               REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_6, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_5(reg32, val)               REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_5, (reg32), (val))
#define CL22_STS_0_SET_ro_cl22reg0_4_0(reg32, val)             REG_FLD_SET(CL22_STS_0_FLD_ro_cl22reg0_4_0, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_ovrun_intr_en(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ovrun_intr_en, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_undrun_intr_en(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_undrun_intr_en, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_odd_nibble_pream_deal_en(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_odd_nibble_pream_deal_en, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_rxer_mask(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rxer_mask, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_lpi2idle_en(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_lpi2idle_en, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_eee_test_mode(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_eee_test_mode, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_ss_lwm(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_lwm, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_ss_hwm(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_hwm, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_ss_mode(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_mode, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_tx_cnt_clr(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_cnt_clr, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_rx_cnt_clr(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_cnt_clr, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_tx_rst_n(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_rst_n, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_SET_rg_mac_t2r_fifo_rx_rst_n(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_rst_n, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_CTRL_1_SET_rg_mac_t2r_fifo_runt_ipg_num(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_runt_ipg_num, (reg32), (val))
#define MAC_T2R_LPBK_FIFO_CTRL_1_SET_rg_mac_t2r_fifo_min_ipg_num(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_min_ipg_num, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_0_SET_ro_mac_t2r_fifo_rx_start_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_0_FLD_ro_mac_t2r_fifo_rx_start_cnt, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_1_SET_ro_mac_t2r_fifo_tx_start_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_1_FLD_ro_mac_t2r_fifo_tx_start_cnt, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_2_SET_ro_mac_t2r_fifo_rx_termi_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_2_FLD_ro_mac_t2r_fifo_rx_termi_cnt, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_3_SET_ro_mac_t2r_fifo_tx_termi_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_3_FLD_ro_mac_t2r_fifo_tx_termi_cnt, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_4_SET_ro_mac_t2r_fifo_ipg_del_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_4_FLD_ro_mac_t2r_fifo_ipg_del_cnt, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_5_SET_ro_mac_t2r_fifo_ipg_ins_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_5_FLD_ro_mac_t2r_fifo_ipg_ins_cnt, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_6_SET_ro_mac_t2r_fifo_rx_runt_ipg_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_6_FLD_ro_mac_t2r_fifo_rx_runt_ipg_cnt, (reg32), (val))

#define MAC_T2R_LPBK_FIFO_STS_7_SET_ro_mac_t2r_fifo_tx_runt_ipg_cnt(reg32, val) REG_FLD_SET(MAC_T2R_LPBK_FIFO_STS_7_FLD_ro_mac_t2r_fifo_tx_runt_ipg_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_ovrun_intr_en(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ovrun_intr_en, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_undrun_intr_en(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_undrun_intr_en, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_odd_nibble_pream_deal_en(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_odd_nibble_pream_deal_en, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_rxer_mask(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rxer_mask, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_lpi2idle_en(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_lpi2idle_en, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_eee_test_mode(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_eee_test_mode, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_ss_lwm(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_lwm, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_ss_hwm(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_hwm, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_ss_mode(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_mode, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_tx_cnt_clr(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_cnt_clr, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_rx_cnt_clr(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_cnt_clr, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_en(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_en, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_tx_rst_n(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_rst_n, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_SET_rg_phy_r2t_fifo_rx_rst_n(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_rst_n, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_CTRL_1_SET_rg_phy_r2t_fifo_runt_ipg_num(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_runt_ipg_num, (reg32), (val))
#define PHY_R2T_LPBK_FIFO_CTRL_1_SET_rg_phy_r2t_fifo_min_ipg_num(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_min_ipg_num, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_0_SET_ro_phy_r2t_fifo_rx_start_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_0_FLD_ro_phy_r2t_fifo_rx_start_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_1_SET_ro_phy_r2t_fifo_tx_start_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_1_FLD_ro_phy_r2t_fifo_tx_start_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_2_SET_ro_phy_r2t_fifo_rx_termi_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_2_FLD_ro_phy_r2t_fifo_rx_termi_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_3_SET_ro_phy_r2t_fifo_tx_termi_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_3_FLD_ro_phy_r2t_fifo_tx_termi_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_4_SET_ro_phy_r2t_fifo_ipg_del_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_4_FLD_ro_phy_r2t_fifo_ipg_del_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_5_SET_ro_phy_r2t_fifo_ipg_ins_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_5_FLD_ro_phy_r2t_fifo_ipg_ins_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_6_SET_ro_phy_r2t_fifo_rx_runt_ipg_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_6_FLD_ro_phy_r2t_fifo_rx_runt_ipg_cnt, (reg32), (val))

#define PHY_R2T_LPBK_FIFO_STS_7_SET_ro_phy_r2t_fifo_tx_runt_ipg_cnt(reg32, val) REG_FLD_SET(PHY_R2T_LPBK_FIFO_STS_7_FLD_ro_phy_r2t_fifo_tx_runt_ipg_cnt, (reg32), (val))

#define PHY_TX_FORCE_CTRL_0_SET_rg_force_txer_sel(reg32, val)  REG_FLD_SET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txer_sel, (reg32), (val))
#define PHY_TX_FORCE_CTRL_0_SET_rg_force_txen_sel(reg32, val)  REG_FLD_SET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txen_sel, (reg32), (val))
#define PHY_TX_FORCE_CTRL_0_SET_rg_force_txc_sel(reg32, val)   REG_FLD_SET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txc_sel, (reg32), (val))
#define PHY_TX_FORCE_CTRL_0_SET_rg_force_txd_sel(reg32, val)   REG_FLD_SET(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txd_sel, (reg32), (val))

#define PHY_TX_FORCE_CTRL_1_SET_rg_force_txd_h(reg32, val)     REG_FLD_SET(PHY_TX_FORCE_CTRL_1_FLD_rg_force_txd_h, (reg32), (val))

#define PHY_TX_FORCE_CTRL_2_SET_rg_force_txd_l(reg32, val)     REG_FLD_SET(PHY_TX_FORCE_CTRL_2_FLD_rg_force_txd_l, (reg32), (val))

#define PHY_TX_FORCE_CTRL_3_SET_rg_force_txer(reg32, val)      REG_FLD_SET(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txer, (reg32), (val))
#define PHY_TX_FORCE_CTRL_3_SET_rg_force_txen(reg32, val)      REG_FLD_SET(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txen, (reg32), (val))
#define PHY_TX_FORCE_CTRL_3_SET_rg_force_txc(reg32, val)       REG_FLD_SET(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txc, (reg32), (val))

#define msg_rx_ctrl_0_SET_rg_hsgmii_xfi_sel(reg32, val)        REG_FLD_SET(msg_rx_ctrl_0_FLD_rg_hsgmii_xfi_sel, (reg32), (val))
#define msg_rx_ctrl_0_SET_rg_qsgmii_port_shift(reg32, val)     REG_FLD_SET(msg_rx_ctrl_0_FLD_rg_qsgmii_port_shift, (reg32), (val))
#define msg_rx_ctrl_0_SET_rg_dec_err_1t(reg32, val)            REG_FLD_SET(msg_rx_ctrl_0_FLD_rg_dec_err_1t, (reg32), (val))
#define msg_rx_ctrl_0_SET_rg_sw_reset_rxck(reg32, val)         REG_FLD_SET(msg_rx_ctrl_0_FLD_rg_sw_reset_rxck, (reg32), (val))
#define msg_rx_ctrl_0_SET_rg_rx_xgmii_mode(reg32, val)         REG_FLD_SET(msg_rx_ctrl_0_FLD_rg_rx_xgmii_mode, (reg32), (val))
#define msg_rx_ctrl_0_SET_rg_rx_5g_mode(reg32, val)            REG_FLD_SET(msg_rx_ctrl_0_FLD_rg_rx_5g_mode, (reg32), (val))
#define msg_rx_ctrl_0_SET_rg_rx_clk_en(reg32, val)             REG_FLD_SET(msg_rx_ctrl_0_FLD_rg_rx_clk_en, (reg32), (val))

#define multi_sgmii_reg_phya_65_SET_rg_tbi_20_tx_reverse_all(reg32, val) REG_FLD_SET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_all, (reg32), (val))
#define multi_sgmii_reg_phya_65_SET_rg_tbi_20_rx_reverse_all(reg32, val) REG_FLD_SET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_all, (reg32), (val))
#define multi_sgmii_reg_phya_65_SET_rg_tbi_10_rx_reverse_pma(reg32, val) REG_FLD_SET(multi_sgmii_reg_phya_65_FLD_rg_tbi_10_rx_reverse_pma, (reg32), (val))
#define multi_sgmii_reg_phya_65_SET_rg_tbi_20_rx_reverse_pma(reg32, val) REG_FLD_SET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_pma, (reg32), (val))
#define multi_sgmii_reg_phya_65_SET_rg_tbi_tx_reverse_10b_pma(reg32, val) REG_FLD_SET(multi_sgmii_reg_phya_65_FLD_rg_tbi_tx_reverse_10b_pma, (reg32), (val))
#define multi_sgmii_reg_phya_65_SET_rg_tbi_20_tx_reverse_pma(reg32, val) REG_FLD_SET(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_pma, (reg32), (val))

#define multi_sgmii_reg_interrupt_sel_SET_rg_hsgmii_phy_int(reg32, val) REG_FLD_SET(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int, (reg32), (val))
#define multi_sgmii_reg_interrupt_sel_SET_rg_hsgmii_phy_int_force(reg32, val) REG_FLD_SET(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int_force, (reg32), (val))
#define multi_sgmii_reg_interrupt_sel_SET_rg_rxer100_int(reg32, val) REG_FLD_SET(multi_sgmii_reg_interrupt_sel_FLD_rg_rxer100_int, (reg32), (val))
#define multi_sgmii_reg_interrupt_sel_SET_rg_false_carrier_int(reg32, val) REG_FLD_SET(multi_sgmii_reg_interrupt_sel_FLD_rg_false_carrier_int, (reg32), (val))
#define multi_sgmii_reg_interrupt_sel_SET_rg_symbol_error_int(reg32, val) REG_FLD_SET(multi_sgmii_reg_interrupt_sel_FLD_rg_symbol_error_int, (reg32), (val))
#define multi_sgmii_reg_interrupt_sel_SET_rg_100fx_int(reg32, val) REG_FLD_SET(multi_sgmii_reg_interrupt_sel_FLD_rg_100fx_int, (reg32), (val))
#define multi_sgmii_reg_interrupt_sel_SET_rg_hsgmii_pcs_int(reg32, val) REG_FLD_SET(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_pcs_int, (reg32), (val))

#define multi_sgmii_reg_probe_sel_SET_rg_probe_clk_sel(reg32, val) REG_FLD_SET(multi_sgmii_reg_probe_sel_FLD_rg_probe_clk_sel, (reg32), (val))
#define multi_sgmii_reg_probe_sel_SET_rg_hsgmii_probe_port_sel(reg32, val) REG_FLD_SET(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_port_sel, (reg32), (val))
#define multi_sgmii_reg_probe_sel_SET_rg_dwn_shift_probe_sel(reg32, val) REG_FLD_SET(multi_sgmii_reg_probe_sel_FLD_rg_dwn_shift_probe_sel, (reg32), (val))
#define multi_sgmii_reg_probe_sel_SET_rg_hsgmii_clk_probe_sel(reg32, val) REG_FLD_SET(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_clk_probe_sel, (reg32), (val))
#define multi_sgmii_reg_probe_sel_SET_rg_100fx_probe_sel(reg32, val) REG_FLD_SET(multi_sgmii_reg_probe_sel_FLD_rg_100fx_probe_sel, (reg32), (val))
#define multi_sgmii_reg_probe_sel_SET_rg_hsgmii_probe_sel(reg32, val) REG_FLD_SET(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_sel, (reg32), (val))

#define multi_sgmii_fpga_mode_control_SET_rg_hwtrap_100fx_mux_en(reg32, val) REG_FLD_SET(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux_en, (reg32), (val))
#define multi_sgmii_fpga_mode_control_SET_rg_hwtrap_100fx_mux(reg32, val) REG_FLD_SET(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux, (reg32), (val))
#define multi_sgmii_fpga_mode_control_SET_rg_hwtrap_100fx_en(reg32, val) REG_FLD_SET(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_en, (reg32), (val))

#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_write_enable_or(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_or, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_read_enable_or(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_or, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_write_enable_and(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_and, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_read_enable_and(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_and, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_aempty_cnt_clear(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_aempty_cnt_clear, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_empty_cnt_clear(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_empty_cnt_clear, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_afull_cnt_clear(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_afull_cnt_clear, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_full_cnt_clear(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_full_cnt_clear, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_w_threshold(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_w_threshold, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_r_threshold(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_r_threshold, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_rclear(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_rclear, (reg32), (val))
#define multi_sgmii_async_fifo_control_SET_rg_hsgmii_async_fifo_wclear(reg32, val) REG_FLD_SET(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_wclear, (reg32), (val))

#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_rfifo_cnt(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_rfifo_cnt, (reg32), (val))
#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_wfifo_cnt(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_wfifo_cnt, (reg32), (val))
#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_w_ready(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_w_ready, (reg32), (val))
#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_r_ready(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_r_ready, (reg32), (val))
#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_aempty(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_aempty, (reg32), (val))
#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_empty(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_empty, (reg32), (val))
#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_afull(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_afull, (reg32), (val))
#define multi_sgmii_ro_phya_afifo_SET_ro_hsgmii_async_fifo_full(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_full, (reg32), (val))

#define multi_sgmii_ro_phya_afifo_cnt_0_SET_ro_hsgmii_async_fifo_full_cnt(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_cnt_0_FLD_ro_hsgmii_async_fifo_full_cnt, (reg32), (val))

#define multi_sgmii_ro_phya_afifo_cnt_1_SET_ro_hsgmii_async_fifo_afull_cnt(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_cnt_1_FLD_ro_hsgmii_async_fifo_afull_cnt, (reg32), (val))

#define multi_sgmii_ro_phya_afifo_cnt_2_SET_ro_hsgmii_async_fifo_empty_cnt(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_cnt_2_FLD_ro_hsgmii_async_fifo_empty_cnt, (reg32), (val))

#define multi_sgmii_ro_phya_afifo_cnt_3_SET_ro_hsgmii_async_fifo_aempty_cnt(reg32, val) REG_FLD_SET(multi_sgmii_ro_phya_afifo_cnt_3_FLD_ro_hsgmii_async_fifo_aempty_cnt, (reg32), (val))

#define msg_led_ctrl_0_SET_rg_led_timing_test(reg32, val)      REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led_timing_test, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_dwn_shift_en(reg32, val)         REG_FLD_SET(msg_led_ctrl_0_FLD_rg_dwn_shift_en, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_dwn_shift_blk_en(reg32, val)     REG_FLD_SET(msg_led_ctrl_0_FLD_rg_dwn_shift_blk_en, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led_enhance(reg32, val)          REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led_enhance, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led_mode(reg32, val)             REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led_mode, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led3_pol(reg32, val)             REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led3_pol, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led2_pol(reg32, val)             REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led2_pol, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led1_pol(reg32, val)             REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led1_pol, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led0_pol(reg32, val)             REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led0_pol, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led3_en(reg32, val)              REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led3_en, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led2_en(reg32, val)              REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led2_en, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led1_en(reg32, val)              REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led1_en, (reg32), (val))
#define msg_led_ctrl_0_SET_rg_led0_en(reg32, val)              REG_FLD_SET(msg_led_ctrl_0_FLD_rg_led0_en, (reg32), (val))

#define msg_led_ctrl_1_SET_rg_led_blk_dur(reg32, val)          REG_FLD_SET(msg_led_ctrl_1_FLD_rg_led_blk_dur, (reg32), (val))
#define msg_led_ctrl_1_SET_rg_led_on_dur(reg32, val)           REG_FLD_SET(msg_led_ctrl_1_FLD_rg_led_on_dur, (reg32), (val))

#define msg_led_ctrl_2_SET_rg_led1_blk_mask(reg32, val)        REG_FLD_SET(msg_led_ctrl_2_FLD_rg_led1_blk_mask, (reg32), (val))
#define msg_led_ctrl_2_SET_rg_led1_on_mask(reg32, val)         REG_FLD_SET(msg_led_ctrl_2_FLD_rg_led1_on_mask, (reg32), (val))
#define msg_led_ctrl_2_SET_rg_led0_blk_mask(reg32, val)        REG_FLD_SET(msg_led_ctrl_2_FLD_rg_led0_blk_mask, (reg32), (val))
#define msg_led_ctrl_2_SET_rg_led0_on_mask(reg32, val)         REG_FLD_SET(msg_led_ctrl_2_FLD_rg_led0_on_mask, (reg32), (val))

#define msg_led_ctrl_3_SET_rg_led3_blk_mask(reg32, val)        REG_FLD_SET(msg_led_ctrl_3_FLD_rg_led3_blk_mask, (reg32), (val))
#define msg_led_ctrl_3_SET_rg_led3_on_mask(reg32, val)         REG_FLD_SET(msg_led_ctrl_3_FLD_rg_led3_on_mask, (reg32), (val))
#define msg_led_ctrl_3_SET_rg_led2_blk_mask(reg32, val)        REG_FLD_SET(msg_led_ctrl_3_FLD_rg_led2_blk_mask, (reg32), (val))
#define msg_led_ctrl_3_SET_rg_led2_on_mask(reg32, val)         REG_FLD_SET(msg_led_ctrl_3_FLD_rg_led2_on_mask, (reg32), (val))

#define msg_led_sts_0_SET_ro_led_blk_event(reg32, val)         REG_FLD_SET(msg_led_sts_0_FLD_ro_led_blk_event, (reg32), (val))
#define msg_led_sts_0_SET_ro_led_on_event(reg32, val)          REG_FLD_SET(msg_led_sts_0_FLD_ro_led_on_event, (reg32), (val))

#define msg_rx_sts_0_SET_ro_k28_1_cnt(reg32, val)              REG_FLD_SET(msg_rx_sts_0_FLD_ro_k28_1_cnt, (reg32), (val))

#define msg_rx_sts_1_SET_ro_port0_k28_1_cnt(reg32, val)        REG_FLD_SET(msg_rx_sts_1_FLD_ro_port0_k28_1_cnt, (reg32), (val))

#define msg_rx_sts_2_SET_ro_port1_k28_1_cnt(reg32, val)        REG_FLD_SET(msg_rx_sts_2_FLD_ro_port1_k28_1_cnt, (reg32), (val))

#define msg_rx_sts_3_SET_ro_port2_k28_1_cnt(reg32, val)        REG_FLD_SET(msg_rx_sts_3_FLD_ro_port2_k28_1_cnt, (reg32), (val))

#define msg_rx_sts_4_SET_ro_port3_k28_1_cnt(reg32, val)        REG_FLD_SET(msg_rx_sts_4_FLD_ro_port3_k28_1_cnt, (reg32), (val))

#define msg_rx_sts_5_SET_ro_rd_out_3b_neg(reg32, val)          REG_FLD_SET(msg_rx_sts_5_FLD_ro_rd_out_3b_neg, (reg32), (val))
#define msg_rx_sts_5_SET_ro_rd_out_3b_pos(reg32, val)          REG_FLD_SET(msg_rx_sts_5_FLD_ro_rd_out_3b_pos, (reg32), (val))
#define msg_rx_sts_5_SET_ro_rd_out_5b_neg(reg32, val)          REG_FLD_SET(msg_rx_sts_5_FLD_ro_rd_out_5b_neg, (reg32), (val))
#define msg_rx_sts_5_SET_ro_rd_out_5b_pos(reg32, val)          REG_FLD_SET(msg_rx_sts_5_FLD_ro_rd_out_5b_pos, (reg32), (val))
#define msg_rx_sts_5_SET_ro_dp3_n(reg32, val)                  REG_FLD_SET(msg_rx_sts_5_FLD_ro_dp3_n, (reg32), (val))
#define msg_rx_sts_5_SET_ro_dp3_p(reg32, val)                  REG_FLD_SET(msg_rx_sts_5_FLD_ro_dp3_p, (reg32), (val))
#define msg_rx_sts_5_SET_ro_dp5_n(reg32, val)                  REG_FLD_SET(msg_rx_sts_5_FLD_ro_dp5_n, (reg32), (val))
#define msg_rx_sts_5_SET_ro_dp5_p(reg32, val)                  REG_FLD_SET(msg_rx_sts_5_FLD_ro_dp5_p, (reg32), (val))

#define msg_rx_sts_6_SET_ro_except_okn(reg32, val)             REG_FLD_SET(msg_rx_sts_6_FLD_ro_except_okn, (reg32), (val))
#define msg_rx_sts_6_SET_ro_except_err(reg32, val)             REG_FLD_SET(msg_rx_sts_6_FLD_ro_except_err, (reg32), (val))
#define msg_rx_sts_6_SET_ro_except_negi(reg32, val)            REG_FLD_SET(msg_rx_sts_6_FLD_ro_except_negi, (reg32), (val))
#define msg_rx_sts_6_SET_ro_except_posi(reg32, val)            REG_FLD_SET(msg_rx_sts_6_FLD_ro_except_posi, (reg32), (val))
#define msg_rx_sts_6_SET_ro_except_neg(reg32, val)             REG_FLD_SET(msg_rx_sts_6_FLD_ro_except_neg, (reg32), (val))
#define msg_rx_sts_6_SET_ro_except_pos(reg32, val)             REG_FLD_SET(msg_rx_sts_6_FLD_ro_except_pos, (reg32), (val))
#define msg_rx_sts_6_SET_ro_except5_n(reg32, val)              REG_FLD_SET(msg_rx_sts_6_FLD_ro_except5_n, (reg32), (val))
#define msg_rx_sts_6_SET_ro_except5_p(reg32, val)              REG_FLD_SET(msg_rx_sts_6_FLD_ro_except5_p, (reg32), (val))

#define msg_rx_sts_7_SET_ro_carrier_reg(reg32, val)            REG_FLD_SET(msg_rx_sts_7_FLD_ro_carrier_reg, (reg32), (val))
#define msg_rx_sts_7_SET_ro_rd_out_5b(reg32, val)              REG_FLD_SET(msg_rx_sts_7_FLD_ro_rd_out_5b, (reg32), (val))
#define msg_rx_sts_7_SET_ro_rd_out_5b_negx(reg32, val)         REG_FLD_SET(msg_rx_sts_7_FLD_ro_rd_out_5b_negx, (reg32), (val))
#define msg_rx_sts_7_SET_ro_rd_out_5b_posx(reg32, val)         REG_FLD_SET(msg_rx_sts_7_FLD_ro_rd_out_5b_posx, (reg32), (val))
#define msg_rx_sts_7_SET_ro_rd_out_3b_negx(reg32, val)         REG_FLD_SET(msg_rx_sts_7_FLD_ro_rd_out_3b_negx, (reg32), (val))
#define msg_rx_sts_7_SET_ro_rd_out_3b_posx(reg32, val)         REG_FLD_SET(msg_rx_sts_7_FLD_ro_rd_out_3b_posx, (reg32), (val))
#define msg_rx_sts_7_SET_ro_data_out_3b_index(reg32, val)      REG_FLD_SET(msg_rx_sts_7_FLD_ro_data_out_3b_index, (reg32), (val))

#define msg_rx_sts_8_SET_ro_error_3b_sc_neg(reg32, val)        REG_FLD_SET(msg_rx_sts_8_FLD_ro_error_3b_sc_neg, (reg32), (val))
#define msg_rx_sts_8_SET_ro_error_3b_sc_pos(reg32, val)        REG_FLD_SET(msg_rx_sts_8_FLD_ro_error_3b_sc_pos, (reg32), (val))
#define msg_rx_sts_8_SET_ro_error_3b_sc(reg32, val)            REG_FLD_SET(msg_rx_sts_8_FLD_ro_error_3b_sc, (reg32), (val))
#define msg_rx_sts_8_SET_ro_error_kout_5b_neg(reg32, val)      REG_FLD_SET(msg_rx_sts_8_FLD_ro_error_kout_5b_neg, (reg32), (val))
#define msg_rx_sts_8_SET_ro_error_kout_5b_pos(reg32, val)      REG_FLD_SET(msg_rx_sts_8_FLD_ro_error_kout_5b_pos, (reg32), (val))
#define msg_rx_sts_8_SET_ro_error_kout_5b(reg32, val)          REG_FLD_SET(msg_rx_sts_8_FLD_ro_error_kout_5b, (reg32), (val))

#define msg_rx_sts_9_SET_ro_error_5b_neg(reg32, val)           REG_FLD_SET(msg_rx_sts_9_FLD_ro_error_5b_neg, (reg32), (val))
#define msg_rx_sts_9_SET_ro_error_5b_pos(reg32, val)           REG_FLD_SET(msg_rx_sts_9_FLD_ro_error_5b_pos, (reg32), (val))
#define msg_rx_sts_9_SET_ro_error_5b(reg32, val)               REG_FLD_SET(msg_rx_sts_9_FLD_ro_error_5b, (reg32), (val))
#define msg_rx_sts_9_SET_ro_error_3b(reg32, val)               REG_FLD_SET(msg_rx_sts_9_FLD_ro_error_3b, (reg32), (val))

#define msg_rx_sts_10_SET_ro_data_out_3b(reg32, val)           REG_FLD_SET(msg_rx_sts_10_FLD_ro_data_out_3b, (reg32), (val))
#define msg_rx_sts_10_SET_ro_data_out_5b(reg32, val)           REG_FLD_SET(msg_rx_sts_10_FLD_ro_data_out_5b, (reg32), (val))

#define msg_rx_sts_11_SET_ro_disp_err(reg32, val)              REG_FLD_SET(msg_rx_sts_11_FLD_ro_disp_err, (reg32), (val))
#define msg_rx_sts_11_SET_ro_char_err(reg32, val)              REG_FLD_SET(msg_rx_sts_11_FLD_ro_char_err, (reg32), (val))
#define msg_rx_sts_11_SET_ro_kout_3b_pos(reg32, val)           REG_FLD_SET(msg_rx_sts_11_FLD_ro_kout_3b_pos, (reg32), (val))
#define msg_rx_sts_11_SET_ro_kout_3b_neg(reg32, val)           REG_FLD_SET(msg_rx_sts_11_FLD_ro_kout_3b_neg, (reg32), (val))
#define msg_rx_sts_11_SET_ro_kout_5b_pos(reg32, val)           REG_FLD_SET(msg_rx_sts_11_FLD_ro_kout_5b_pos, (reg32), (val))
#define msg_rx_sts_11_SET_ro_kout_5b_neg(reg32, val)           REG_FLD_SET(msg_rx_sts_11_FLD_ro_kout_5b_neg, (reg32), (val))

#define msg_rx_sts_12_SET_ro_port3_sync_lost(reg32, val)       REG_FLD_SET(msg_rx_sts_12_FLD_ro_port3_sync_lost, (reg32), (val))
#define msg_rx_sts_12_SET_ro_port2_sync_lost(reg32, val)       REG_FLD_SET(msg_rx_sts_12_FLD_ro_port2_sync_lost, (reg32), (val))
#define msg_rx_sts_12_SET_ro_port1_sync_lost(reg32, val)       REG_FLD_SET(msg_rx_sts_12_FLD_ro_port1_sync_lost, (reg32), (val))
#define msg_rx_sts_12_SET_ro_port0_sync_lost(reg32, val)       REG_FLD_SET(msg_rx_sts_12_FLD_ro_port0_sync_lost, (reg32), (val))
#define msg_rx_sts_12_SET_ro_sync_int(reg32, val)              REG_FLD_SET(msg_rx_sts_12_FLD_ro_sync_int, (reg32), (val))
#define msg_rx_sts_12_SET_ro_rd_out(reg32, val)                REG_FLD_SET(msg_rx_sts_12_FLD_ro_rd_out, (reg32), (val))
#define msg_rx_sts_12_SET_ro_align_det(reg32, val)             REG_FLD_SET(msg_rx_sts_12_FLD_ro_align_det, (reg32), (val))

#define msg_rx_sts_13_SET_ro_data_align(reg32, val)            REG_FLD_SET(msg_rx_sts_13_FLD_ro_data_align, (reg32), (val))

#define msg_rx_sts_14_SET_ro_par(reg32, val)                   REG_FLD_SET(msg_rx_sts_14_FLD_ro_par, (reg32), (val))

#define msg_rx_sts_15_SET_ro_lnk_sts_p3(reg32, val)            REG_FLD_SET(msg_rx_sts_15_FLD_ro_lnk_sts_p3, (reg32), (val))
#define msg_rx_sts_15_SET_ro_speed_sts_p3(reg32, val)          REG_FLD_SET(msg_rx_sts_15_FLD_ro_speed_sts_p3, (reg32), (val))
#define msg_rx_sts_15_SET_ro_lnk_sts_p2(reg32, val)            REG_FLD_SET(msg_rx_sts_15_FLD_ro_lnk_sts_p2, (reg32), (val))
#define msg_rx_sts_15_SET_ro_speed_sts_p2(reg32, val)          REG_FLD_SET(msg_rx_sts_15_FLD_ro_speed_sts_p2, (reg32), (val))
#define msg_rx_sts_15_SET_ro_lnk_sts_p1(reg32, val)            REG_FLD_SET(msg_rx_sts_15_FLD_ro_lnk_sts_p1, (reg32), (val))
#define msg_rx_sts_15_SET_ro_speed_sts_p1(reg32, val)          REG_FLD_SET(msg_rx_sts_15_FLD_ro_speed_sts_p1, (reg32), (val))
#define msg_rx_sts_15_SET_ro_lnk_sts_p0(reg32, val)            REG_FLD_SET(msg_rx_sts_15_FLD_ro_lnk_sts_p0, (reg32), (val))
#define msg_rx_sts_15_SET_ro_speed_sts_p0(reg32, val)          REG_FLD_SET(msg_rx_sts_15_FLD_ro_speed_sts_p0, (reg32), (val))

#define msg_rx_sts_16_SET_ro_p3_rx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p3_rx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p2_rx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p2_rx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p1_rx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p1_rx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p0_rx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p0_rx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p3_tx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p3_tx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p2_tx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p2_tx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p1_tx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p1_tx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p0_tx_eee_flag_latch(reg32, val)  REG_FLD_SET(msg_rx_sts_16_FLD_ro_p0_tx_eee_flag_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_sig_detect_down_latch(reg32, val) REG_FLD_SET(msg_rx_sts_16_FLD_ro_sig_detect_down_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_sig_detect_up_latch(reg32, val)   REG_FLD_SET(msg_rx_sts_16_FLD_ro_sig_detect_up_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p3_link_down_latch(reg32, val)    REG_FLD_SET(msg_rx_sts_16_FLD_ro_p3_link_down_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p3_link_up_latch(reg32, val)      REG_FLD_SET(msg_rx_sts_16_FLD_ro_p3_link_up_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p2_link_down_latch(reg32, val)    REG_FLD_SET(msg_rx_sts_16_FLD_ro_p2_link_down_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p2_link_up_latch(reg32, val)      REG_FLD_SET(msg_rx_sts_16_FLD_ro_p2_link_up_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p1_link_down_latch(reg32, val)    REG_FLD_SET(msg_rx_sts_16_FLD_ro_p1_link_down_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p1_link_up_latch(reg32, val)      REG_FLD_SET(msg_rx_sts_16_FLD_ro_p1_link_up_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p0_link_down_latch(reg32, val)    REG_FLD_SET(msg_rx_sts_16_FLD_ro_p0_link_down_latch, (reg32), (val))
#define msg_rx_sts_16_SET_ro_p0_link_up_latch(reg32, val)      REG_FLD_SET(msg_rx_sts_16_FLD_ro_p0_link_up_latch, (reg32), (val))

#define msg_rx_sts_17_SET_rg_p3_rx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p3_rx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p2_rx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p2_rx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p1_rx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p1_rx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p0_rx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p0_rx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p3_tx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p3_tx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p2_tx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p2_tx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p1_tx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p1_tx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p0_tx_eee_int_clr(reg32, val)     REG_FLD_SET(msg_rx_sts_17_FLD_rg_p0_tx_eee_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_sig_detect_down_int_clr(reg32, val) REG_FLD_SET(msg_rx_sts_17_FLD_rg_sig_detect_down_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_sig_detect_up_int_clr(reg32, val) REG_FLD_SET(msg_rx_sts_17_FLD_rg_sig_detect_up_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p3_link_down_int_clr(reg32, val)  REG_FLD_SET(msg_rx_sts_17_FLD_rg_p3_link_down_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p3_link_up_int_clr(reg32, val)    REG_FLD_SET(msg_rx_sts_17_FLD_rg_p3_link_up_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p2_link_down_int_clr(reg32, val)  REG_FLD_SET(msg_rx_sts_17_FLD_rg_p2_link_down_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p2_link_up_int_clr(reg32, val)    REG_FLD_SET(msg_rx_sts_17_FLD_rg_p2_link_up_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p1_link_down_int_clr(reg32, val)  REG_FLD_SET(msg_rx_sts_17_FLD_rg_p1_link_down_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p1_link_up_int_clr(reg32, val)    REG_FLD_SET(msg_rx_sts_17_FLD_rg_p1_link_up_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p0_link_down_int_clr(reg32, val)  REG_FLD_SET(msg_rx_sts_17_FLD_rg_p0_link_down_int_clr, (reg32), (val))
#define msg_rx_sts_17_SET_rg_p0_link_up_int_clr(reg32, val)    REG_FLD_SET(msg_rx_sts_17_FLD_rg_p0_link_up_int_clr, (reg32), (val))

#define msg_rx_sts_18_SET_p3_sgmii_is_10(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p3_sgmii_is_10, (reg32), (val))
#define msg_rx_sts_18_SET_p3_sgmii_is_100(reg32, val)          REG_FLD_SET(msg_rx_sts_18_FLD_p3_sgmii_is_100, (reg32), (val))
#define msg_rx_sts_18_SET_p3_sgmii_is_1g(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p3_sgmii_is_1g, (reg32), (val))
#define msg_rx_sts_18_SET_p2_sgmii_is_10(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p2_sgmii_is_10, (reg32), (val))
#define msg_rx_sts_18_SET_p2_sgmii_is_100(reg32, val)          REG_FLD_SET(msg_rx_sts_18_FLD_p2_sgmii_is_100, (reg32), (val))
#define msg_rx_sts_18_SET_p2_sgmii_is_1g(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p2_sgmii_is_1g, (reg32), (val))
#define msg_rx_sts_18_SET_p1_sgmii_is_10(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p1_sgmii_is_10, (reg32), (val))
#define msg_rx_sts_18_SET_p1_sgmii_is_100(reg32, val)          REG_FLD_SET(msg_rx_sts_18_FLD_p1_sgmii_is_100, (reg32), (val))
#define msg_rx_sts_18_SET_p1_sgmii_is_1g(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p1_sgmii_is_1g, (reg32), (val))
#define msg_rx_sts_18_SET_p0_sgmii_is_10(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p0_sgmii_is_10, (reg32), (val))
#define msg_rx_sts_18_SET_p0_sgmii_is_100(reg32, val)          REG_FLD_SET(msg_rx_sts_18_FLD_p0_sgmii_is_100, (reg32), (val))
#define msg_rx_sts_18_SET_p0_sgmii_is_1g(reg32, val)           REG_FLD_SET(msg_rx_sts_18_FLD_p0_sgmii_is_1g, (reg32), (val))

#define msg_rx_sts_19_SET_p1_lp_ability(reg32, val)            REG_FLD_SET(msg_rx_sts_19_FLD_p1_lp_ability, (reg32), (val))
#define msg_rx_sts_19_SET_p0_lp_ability(reg32, val)            REG_FLD_SET(msg_rx_sts_19_FLD_p0_lp_ability, (reg32), (val))

#define msg_rx_sts_20_SET_p3_lp_ability(reg32, val)            REG_FLD_SET(msg_rx_sts_20_FLD_p3_lp_ability, (reg32), (val))
#define msg_rx_sts_20_SET_p2_lp_ability(reg32, val)            REG_FLD_SET(msg_rx_sts_20_FLD_p2_lp_ability, (reg32), (val))

#define cl22_status_0_SET_ro_cl22_reset_wrtie_flag(reg32, val) REG_FLD_SET(cl22_status_0_FLD_ro_cl22_reset_wrtie_flag, (reg32), (val))
#define cl22_status_0_SET_ro_cl22_wrtie_flag(reg32, val)       REG_FLD_SET(cl22_status_0_FLD_ro_cl22_wrtie_flag, (reg32), (val))

#define cl22_status_1_SET_rg_cl22_reset_wrtie_flag_clr(reg32, val) REG_FLD_SET(cl22_status_1_FLD_rg_cl22_reset_wrtie_flag_clr, (reg32), (val))
#define cl22_status_1_SET_rg_cl22_wrtie_flag_clr(reg32, val)   REG_FLD_SET(cl22_status_1_FLD_rg_cl22_wrtie_flag_clr, (reg32), (val))

#define msg_rx_sts_21_SET_rg_rx_eee_flag_lev_p3_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p3_clr, (reg32), (val))
#define msg_rx_sts_21_SET_rg_rx_eee_flag_lev_p2_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p2_clr, (reg32), (val))
#define msg_rx_sts_21_SET_rg_rx_eee_flag_lev_p1_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p1_clr, (reg32), (val))
#define msg_rx_sts_21_SET_rg_rx_eee_flag_lev_p0_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p0_clr, (reg32), (val))
#define msg_rx_sts_21_SET_rg_tx_eee_flag_lev_p3_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p3_clr, (reg32), (val))
#define msg_rx_sts_21_SET_rg_tx_eee_flag_lev_p2_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p2_clr, (reg32), (val))
#define msg_rx_sts_21_SET_rg_tx_eee_flag_lev_p1_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p1_clr, (reg32), (val))
#define msg_rx_sts_21_SET_rg_tx_eee_flag_lev_p0_clr(reg32, val) REG_FLD_SET(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p0_clr, (reg32), (val))

#define msg_rx_ctrl_1_SET_rg_force_port3_cnt(reg32, val)       REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port3_cnt, (reg32), (val))
#define msg_rx_ctrl_1_SET_rg_force_port2_cnt(reg32, val)       REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port2_cnt, (reg32), (val))
#define msg_rx_ctrl_1_SET_rg_force_port1_cnt(reg32, val)       REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port1_cnt, (reg32), (val))
#define msg_rx_ctrl_1_SET_rg_force_port0_cnt(reg32, val)       REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port0_cnt, (reg32), (val))
#define msg_rx_ctrl_1_SET_rg_force_port3_cnt_sel(reg32, val)   REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port3_cnt_sel, (reg32), (val))
#define msg_rx_ctrl_1_SET_rg_force_port2_cnt_sel(reg32, val)   REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port2_cnt_sel, (reg32), (val))
#define msg_rx_ctrl_1_SET_rg_force_port1_cnt_sel(reg32, val)   REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port1_cnt_sel, (reg32), (val))
#define msg_rx_ctrl_1_SET_rg_force_port0_cnt_sel(reg32, val)   REG_FLD_SET(msg_rx_ctrl_1_FLD_rg_force_port0_cnt_sel, (reg32), (val))

#define msg_rx_ctrl_2_SET_rg_force_port3_sync(reg32, val)      REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port3_sync, (reg32), (val))
#define msg_rx_ctrl_2_SET_rg_force_port2_sync(reg32, val)      REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port2_sync, (reg32), (val))
#define msg_rx_ctrl_2_SET_rg_force_port1_sync(reg32, val)      REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port1_sync, (reg32), (val))
#define msg_rx_ctrl_2_SET_rg_force_port0_sync(reg32, val)      REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port0_sync, (reg32), (val))
#define msg_rx_ctrl_2_SET_rg_force_port3_sync_sel(reg32, val)  REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port3_sync_sel, (reg32), (val))
#define msg_rx_ctrl_2_SET_rg_force_port2_sync_sel(reg32, val)  REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port2_sync_sel, (reg32), (val))
#define msg_rx_ctrl_2_SET_rg_force_port1_sync_sel(reg32, val)  REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port1_sync_sel, (reg32), (val))
#define msg_rx_ctrl_2_SET_rg_force_port0_sync_sel(reg32, val)  REG_FLD_SET(msg_rx_ctrl_2_FLD_rg_force_port0_sync_sel, (reg32), (val))

#define msg_rx_ctrl_3_SET_rg_k28_1_cnt_clr(reg32, val)         REG_FLD_SET(msg_rx_ctrl_3_FLD_rg_k28_1_cnt_clr, (reg32), (val))
#define msg_rx_ctrl_3_SET_rg_port3_k28_1_cnt_clr(reg32, val)   REG_FLD_SET(msg_rx_ctrl_3_FLD_rg_port3_k28_1_cnt_clr, (reg32), (val))
#define msg_rx_ctrl_3_SET_rg_port2_k28_1_cnt_clr(reg32, val)   REG_FLD_SET(msg_rx_ctrl_3_FLD_rg_port2_k28_1_cnt_clr, (reg32), (val))
#define msg_rx_ctrl_3_SET_rg_port1_k28_1_cnt_clr(reg32, val)   REG_FLD_SET(msg_rx_ctrl_3_FLD_rg_port1_k28_1_cnt_clr, (reg32), (val))
#define msg_rx_ctrl_3_SET_rg_port0_k28_1_cnt_clr(reg32, val)   REG_FLD_SET(msg_rx_ctrl_3_FLD_rg_port0_k28_1_cnt_clr, (reg32), (val))

#define msg_rx_ctrl_4_SET_rg_qsgmii_rx500_pcs_div_ck_sel(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_qsgmii_rx500_pcs_div_ck_sel, (reg32), (val))
#define msg_rx_ctrl_4_SET_rg_p3_mac_rx_div_ck_reset_n(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_p3_mac_rx_div_ck_reset_n, (reg32), (val))
#define msg_rx_ctrl_4_SET_rg_p2_mac_rx_div_ck_reset_n(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_p2_mac_rx_div_ck_reset_n, (reg32), (val))
#define msg_rx_ctrl_4_SET_rg_p1_mac_rx_div_ck_reset_n(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_p1_mac_rx_div_ck_reset_n, (reg32), (val))
#define msg_rx_ctrl_4_SET_rg_qsgmii_p3_rx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_qsgmii_p3_rx500_mac_div_ck_sel, (reg32), (val))
#define msg_rx_ctrl_4_SET_rg_qsgmii_p2_rx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_qsgmii_p2_rx500_mac_div_ck_sel, (reg32), (val))
#define msg_rx_ctrl_4_SET_rg_qsgmii_p1_rx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_qsgmii_p1_rx500_mac_div_ck_sel, (reg32), (val))
#define msg_rx_ctrl_4_SET_rg_qsgmii_p0_rx500_mac_div_ck_sel(reg32, val) REG_FLD_SET(msg_rx_ctrl_4_FLD_rg_qsgmii_p0_rx500_mac_div_ck_sel, (reg32), (val))

#define msg_rx_ctrl_5_SET_rg_fx_crs_test(reg32, val)           REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_fx_crs_test, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_fx_col_test(reg32, val)           REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_fx_col_test, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_hg_crs_test(reg32, val)           REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_hg_crs_test, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_hg_col_test(reg32, val)           REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_hg_col_test, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_crs_is_fx_mode(reg32, val)        REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_crs_is_fx_mode, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_col_is_fx_mode(reg32, val)        REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_col_is_fx_mode, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_fx_crs_sel(reg32, val)            REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_fx_crs_sel, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_fx_col_sel(reg32, val)            REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_fx_col_sel, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_fx_crs_test_mode(reg32, val)      REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_fx_crs_test_mode, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_fx_col_test_mode(reg32, val)      REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_fx_col_test_mode, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_hg_crs_test_mode(reg32, val)      REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_hg_crs_test_mode, (reg32), (val))
#define msg_rx_ctrl_5_SET_rg_hg_col_test_mode(reg32, val)      REG_FLD_SET(msg_rx_ctrl_5_FLD_rg_hg_col_test_mode, (reg32), (val))

#define msg_rx_lik_sts_0_SET_rg_eee2p5g_sts(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee2p5g_sts, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee1g_sts_p3(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p3, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee100_sts_p3(reg32, val)      REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p3, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_txfc_sts_p3(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p3, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_rxfc_sts_p3(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p3, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_dpx_sts_p3(reg32, val)         REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p3, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee1g_sts_p2(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p2, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee100_sts_p2(reg32, val)      REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p2, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_txfc_sts_p2(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p2, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_rxfc_sts_p2(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p2, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_dpx_sts_p2(reg32, val)         REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p2, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee1g_sts_p1(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p1, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee100_sts_p1(reg32, val)      REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p1, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_txfc_sts_p1(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p1, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_rxfc_sts_p1(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p1, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_dpx_sts_p1(reg32, val)         REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p1, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee1g_sts_p0(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p0, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_eee100_sts_p0(reg32, val)      REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p0, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_txfc_sts_p0(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p0, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_rxfc_sts_p0(reg32, val)        REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p0, (reg32), (val))
#define msg_rx_lik_sts_0_SET_rg_dpx_sts_p0(reg32, val)         REG_FLD_SET(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p0, (reg32), (val))

#define msg_rx_lik_sts_1_SET_rg_pause_sts_p3(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p3, (reg32), (val))
#define msg_rx_lik_sts_1_SET_rg_pause_sts_p2(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p2, (reg32), (val))
#define msg_rx_lik_sts_1_SET_rg_pause_sts_p1(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p1, (reg32), (val))
#define msg_rx_lik_sts_1_SET_rg_pause_sts_p0(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_pause_sts_p0, (reg32), (val))
#define msg_rx_lik_sts_1_SET_rg_lp_an_cap_p3(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p3, (reg32), (val))
#define msg_rx_lik_sts_1_SET_rg_lp_an_cap_p2(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p2, (reg32), (val))
#define msg_rx_lik_sts_1_SET_rg_lp_an_cap_p1(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p1, (reg32), (val))
#define msg_rx_lik_sts_1_SET_rg_lp_an_cap_p0(reg32, val)       REG_FLD_SET(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p0, (reg32), (val))

#define msg_rx_lik_sts_2_SET_rg_rxfc_an_bypass_p3(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p3, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_rxfc_an_bypass_p2(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p2, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_rxfc_an_bypass_p1(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p1, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_rxfc_an_bypass_p0(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p0, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_txfc_an_bypass_p3(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p3, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_txfc_an_bypass_p2(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p2, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_txfc_an_bypass_p1(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p1, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_txfc_an_bypass_p0(reg32, val)  REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p0, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_dpx_an_bypass_p3(reg32, val)   REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p3, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_dpx_an_bypass_p2(reg32, val)   REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p2, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_dpx_an_bypass_p1(reg32, val)   REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p1, (reg32), (val))
#define msg_rx_lik_sts_2_SET_rg_dpx_an_bypass_p0(reg32, val)   REG_FLD_SET(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p0, (reg32), (val))

#define PHY_RX_FORCE_CTRL_0_SET_rg_force_rxer_sel(reg32, val)  REG_FLD_SET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxer_sel, (reg32), (val))
#define PHY_RX_FORCE_CTRL_0_SET_rg_force_rxdv_sel(reg32, val)  REG_FLD_SET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxdv_sel, (reg32), (val))
#define PHY_RX_FORCE_CTRL_0_SET_rg_force_rxc_sel(reg32, val)   REG_FLD_SET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxc_sel, (reg32), (val))
#define PHY_RX_FORCE_CTRL_0_SET_rg_force_rxd_sel(reg32, val)   REG_FLD_SET(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxd_sel, (reg32), (val))

#define PHY_RX_FORCE_CTRL_1_SET_rg_force_rxd_h(reg32, val)     REG_FLD_SET(PHY_RX_FORCE_CTRL_1_FLD_rg_force_rxd_h, (reg32), (val))

#define PHY_RX_FORCE_CTRL_2_SET_rg_force_rxd_l(reg32, val)     REG_FLD_SET(PHY_RX_FORCE_CTRL_2_FLD_rg_force_rxd_l, (reg32), (val))

#define PHY_RX_FORCE_CTRL_3_SET_rg_force_rxer(reg32, val)      REG_FLD_SET(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxer, (reg32), (val))
#define PHY_RX_FORCE_CTRL_3_SET_rg_force_rxdv(reg32, val)      REG_FLD_SET(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxdv, (reg32), (val))
#define PHY_RX_FORCE_CTRL_3_SET_rg_force_rxc(reg32, val)       REG_FLD_SET(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxc, (reg32), (val))

#define Debug_REGISTER_0_SET_ro_dump_000(reg32, val)           REG_FLD_SET(Debug_REGISTER_0_FLD_ro_dump_000, (reg32), (val))

#define Debug_REGISTER_1_SET_ro_dump_001(reg32, val)           REG_FLD_SET(Debug_REGISTER_1_FLD_ro_dump_001, (reg32), (val))

#define Debug_REGISTER_2_SET_ro_dump_002(reg32, val)           REG_FLD_SET(Debug_REGISTER_2_FLD_ro_dump_002, (reg32), (val))

#define Debug_REGISTER_3_SET_ro_dump_003(reg32, val)           REG_FLD_SET(Debug_REGISTER_3_FLD_ro_dump_003, (reg32), (val))

#define Debug_REGISTER_4_SET_ro_dump_004(reg32, val)           REG_FLD_SET(Debug_REGISTER_4_FLD_ro_dump_004, (reg32), (val))

#define Debug_REGISTER_5_SET_ro_dump_005(reg32, val)           REG_FLD_SET(Debug_REGISTER_5_FLD_ro_dump_005, (reg32), (val))

#define Debug_REGISTER_6_SET_ro_dump_006(reg32, val)           REG_FLD_SET(Debug_REGISTER_6_FLD_ro_dump_006, (reg32), (val))

#define Debug_REGISTER_7_SET_ro_dump_007(reg32, val)           REG_FLD_SET(Debug_REGISTER_7_FLD_ro_dump_007, (reg32), (val))

#define Debug_REGISTER_8_SET_ro_dump_008(reg32, val)           REG_FLD_SET(Debug_REGISTER_8_FLD_ro_dump_008, (reg32), (val))

#define Debug_REGISTER_9_SET_ro_dump_009(reg32, val)           REG_FLD_SET(Debug_REGISTER_9_FLD_ro_dump_009, (reg32), (val))

#define Debug_REGISTER_10_SET_ro_dump_010(reg32, val)          REG_FLD_SET(Debug_REGISTER_10_FLD_ro_dump_010, (reg32), (val))

#define Debug_REGISTER_11_SET_ro_dump_011(reg32, val)          REG_FLD_SET(Debug_REGISTER_11_FLD_ro_dump_011, (reg32), (val))

#define Debug_REGISTER_12_SET_ro_dump_012(reg32, val)          REG_FLD_SET(Debug_REGISTER_12_FLD_ro_dump_012, (reg32), (val))

#define Debug_REGISTER_13_SET_ro_dump_013(reg32, val)          REG_FLD_SET(Debug_REGISTER_13_FLD_ro_dump_013, (reg32), (val))

#define Debug_REGISTER_14_SET_ro_dump_014(reg32, val)          REG_FLD_SET(Debug_REGISTER_14_FLD_ro_dump_014, (reg32), (val))

#define Debug_REGISTER_15_SET_ro_dump_015(reg32, val)          REG_FLD_SET(Debug_REGISTER_15_FLD_ro_dump_015, (reg32), (val))

#define Debug_REGISTER_16_SET_ro_dump_016(reg32, val)          REG_FLD_SET(Debug_REGISTER_16_FLD_ro_dump_016, (reg32), (val))

#define Debug_REGISTER_17_SET_ro_dump_017(reg32, val)          REG_FLD_SET(Debug_REGISTER_17_FLD_ro_dump_017, (reg32), (val))

#define Debug_REGISTER_18_SET_ro_dump_018(reg32, val)          REG_FLD_SET(Debug_REGISTER_18_FLD_ro_dump_018, (reg32), (val))

#define Debug_REGISTER_19_SET_ro_dump_019(reg32, val)          REG_FLD_SET(Debug_REGISTER_19_FLD_ro_dump_019, (reg32), (val))

#define Debug_REGISTER_20_SET_ro_dump_020(reg32, val)          REG_FLD_SET(Debug_REGISTER_20_FLD_ro_dump_020, (reg32), (val))

#define Debug_REGISTER_21_SET_ro_dump_021(reg32, val)          REG_FLD_SET(Debug_REGISTER_21_FLD_ro_dump_021, (reg32), (val))

#define Debug_REGISTER_22_SET_ro_dump_022(reg32, val)          REG_FLD_SET(Debug_REGISTER_22_FLD_ro_dump_022, (reg32), (val))

#define Debug_REGISTER_23_SET_ro_dump_023(reg32, val)          REG_FLD_SET(Debug_REGISTER_23_FLD_ro_dump_023, (reg32), (val))

#define Debug_REGISTER_24_SET_ro_dump_024(reg32, val)          REG_FLD_SET(Debug_REGISTER_24_FLD_ro_dump_024, (reg32), (val))

#define Debug_REGISTER_25_SET_ro_dump_025(reg32, val)          REG_FLD_SET(Debug_REGISTER_25_FLD_ro_dump_025, (reg32), (val))

#define Debug_REGISTER_26_SET_ro_dump_026(reg32, val)          REG_FLD_SET(Debug_REGISTER_26_FLD_ro_dump_026, (reg32), (val))

#define Debug_REGISTER_27_SET_ro_dump_027(reg32, val)          REG_FLD_SET(Debug_REGISTER_27_FLD_ro_dump_027, (reg32), (val))

#define Debug_REGISTER_28_SET_ro_dump_028(reg32, val)          REG_FLD_SET(Debug_REGISTER_28_FLD_ro_dump_028, (reg32), (val))

#define Debug_REGISTER_29_SET_ro_dump_029(reg32, val)          REG_FLD_SET(Debug_REGISTER_29_FLD_ro_dump_029, (reg32), (val))

#define Debug_REGISTER_30_SET_ro_dump_030(reg32, val)          REG_FLD_SET(Debug_REGISTER_30_FLD_ro_dump_030, (reg32), (val))

#define Debug_REGISTER_31_SET_ro_dump_031(reg32, val)          REG_FLD_SET(Debug_REGISTER_31_FLD_ro_dump_031, (reg32), (val))

#define Debug_REGISTER_CTRL_0_SET_rg_force_dump(reg32, val)    REG_FLD_SET(Debug_REGISTER_CTRL_0_FLD_rg_force_dump, (reg32), (val))
#define Debug_REGISTER_CTRL_0_SET_rg_dump_sel(reg32, val)      REG_FLD_SET(Debug_REGISTER_CTRL_0_FLD_rg_dump_sel, (reg32), (val))
#define Debug_REGISTER_CTRL_0_SET_rg_dump_inv(reg32, val)      REG_FLD_SET(Debug_REGISTER_CTRL_0_FLD_rg_dump_inv, (reg32), (val))
#define Debug_REGISTER_CTRL_0_SET_rg_dump_clr(reg32, val)      REG_FLD_SET(Debug_REGISTER_CTRL_0_FLD_rg_dump_clr, (reg32), (val))

#define Debug_REGISTER_DUMP_CNT_SET_ro_dump_marker(reg32, val) REG_FLD_SET(Debug_REGISTER_DUMP_CNT_FLD_ro_dump_marker, (reg32), (val))

#define msg_tx_ctrl_0_VAL_rg_force_100fx_sigdet(val)           REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_force_100fx_sigdet, (val))
#define msg_tx_ctrl_0_VAL_rg_ssusb_hsgmii_sel(val)             REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_ssusb_hsgmii_sel, (val))
#define msg_tx_ctrl_0_VAL_rg_sgmii_mode_force(val)             REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_sgmii_mode_force, (val))
#define msg_tx_ctrl_0_VAL_rg_qsgmii_mode(val)                  REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_qsgmii_mode, (val))
#define msg_tx_ctrl_0_VAL_rg_tx_data_re(val)                   REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_tx_data_re, (val))
#define msg_tx_ctrl_0_VAL_rg_tx_data_rep4_sel(val)             REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_tx_data_rep4_sel, (val))
#define msg_tx_ctrl_0_VAL_rg_tx_data_rep2_sel(val)             REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_tx_data_rep2_sel, (val))
#define msg_tx_ctrl_0_VAL_rg_qsgmii_txclk_ena(val)             REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_qsgmii_txclk_ena, (val))
#define msg_tx_ctrl_0_VAL_rg_tx_free_cnt_clr(val)              REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_tx_free_cnt_clr, (val))
#define msg_tx_ctrl_0_VAL_rg_tx_gpii_rst(val)                  REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_tx_gpii_rst, (val))
#define msg_tx_ctrl_0_VAL_rg_tx_5g_mode(val)                   REG_FLD_VAL(msg_tx_ctrl_0_FLD_rg_tx_5g_mode, (val))

#define msg_tx_ctrl_1_VAL_rg_qsgmii_tx500_pcs_div_ck_sel(val)  REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_qsgmii_tx500_pcs_div_ck_sel, (val))
#define msg_tx_ctrl_1_VAL_rg_p3_mac_tx_div_ck_reset_n(val)     REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_p3_mac_tx_div_ck_reset_n, (val))
#define msg_tx_ctrl_1_VAL_rg_p2_mac_tx_div_ck_reset_n(val)     REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_p2_mac_tx_div_ck_reset_n, (val))
#define msg_tx_ctrl_1_VAL_rg_p1_mac_tx_div_ck_reset_n(val)     REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_p1_mac_tx_div_ck_reset_n, (val))
#define msg_tx_ctrl_1_VAL_rg_qsgmii_p3_tx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_qsgmii_p3_tx500_mac_div_ck_sel, (val))
#define msg_tx_ctrl_1_VAL_rg_qsgmii_p2_tx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_qsgmii_p2_tx500_mac_div_ck_sel, (val))
#define msg_tx_ctrl_1_VAL_rg_qsgmii_p1_tx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_qsgmii_p1_tx500_mac_div_ck_sel, (val))
#define msg_tx_ctrl_1_VAL_rg_qsgmii_p0_tx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_tx_ctrl_1_FLD_rg_qsgmii_p0_tx500_mac_div_ck_sel, (val))

#define msg_tx_ctrl_2_VAL_rg_qsgmii_tx_kd_rev(val)             REG_FLD_VAL(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_kd_rev, (val))
#define msg_tx_ctrl_2_VAL_rg_qsgmii_tx_idle_swap_en(val)       REG_FLD_VAL(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_idle_swap_en, (val))
#define msg_tx_ctrl_2_VAL_rg_qsgmii_tx_rev(val)                REG_FLD_VAL(msg_tx_ctrl_2_FLD_rg_qsgmii_tx_rev, (val))
#define msg_tx_ctrl_2_VAL_rg_qsgmii_20b_tx_hl_swap(val)        REG_FLD_VAL(msg_tx_ctrl_2_FLD_rg_qsgmii_20b_tx_hl_swap, (val))

#define efuse_ctrl_dummy_VAL_rg_efuse_tphy_auto_load_valid(val) REG_FLD_VAL(efuse_ctrl_dummy_FLD_rg_efuse_tphy_auto_load_valid, (val))
#define efuse_ctrl_dummy_VAL_rg_efuse_tphy_iext_intr_ctrl(val) REG_FLD_VAL(efuse_ctrl_dummy_FLD_rg_efuse_tphy_iext_intr_ctrl, (val))
#define efuse_ctrl_dummy_VAL_rg_efuse_tphy_tx_imp_sel(val)     REG_FLD_VAL(efuse_ctrl_dummy_FLD_rg_efuse_tphy_tx_imp_sel, (val))
#define efuse_ctrl_dummy_VAL_rg_efuse_tphy_rx_imp_sel(val)     REG_FLD_VAL(efuse_ctrl_dummy_FLD_rg_efuse_tphy_rx_imp_sel, (val))

#define rst_ctrl_0_VAL_rg_hsgmii_mac_sw_rst_n(val)             REG_FLD_VAL(rst_ctrl_0_FLD_rg_hsgmii_mac_sw_rst_n, (val))
#define rst_ctrl_0_VAL_rg_ssusb_ip_sw_rst(val)                 REG_FLD_VAL(rst_ctrl_0_FLD_rg_ssusb_ip_sw_rst, (val))

#define INTERRUPT_EN_0_VAL_rg_cl22_rg_phy_rst_cnt(val)         REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_cl22_rg_phy_rst_cnt, (val))
#define INTERRUPT_EN_0_VAL_rg_rx_eee_int_en(val)               REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_rx_eee_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_tx_eee_int_en(val)               REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_tx_eee_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_cl22_reset_wr_int_en(val)        REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_cl22_reset_wr_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_cl22_wr_int_en(val)              REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_cl22_wr_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_sgmii_link_down_int_en(val)      REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_sgmii_link_down_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_sgmii_link_up_int_en(val)        REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_sgmii_link_up_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_sigdet_down_int_en(val)          REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_sigdet_down_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_sigdet_up_int_en(val)            REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_sigdet_up_int_en, (val))
#define INTERRUPT_EN_0_VAL_rg_pcs_int_en_3(val)                REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_pcs_int_en_3, (val))
#define INTERRUPT_EN_0_VAL_rg_pcs_int_en_2(val)                REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_pcs_int_en_2, (val))
#define INTERRUPT_EN_0_VAL_rg_pcs_int_en_1(val)                REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_pcs_int_en_1, (val))
#define INTERRUPT_EN_0_VAL_rg_pcs_int_en_0(val)                REG_FLD_VAL(INTERRUPT_EN_0_FLD_rg_pcs_int_en_0, (val))

#define SGMII_STS_CTRL_0_VAL_rg_link_mode_p3(val)              REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_link_mode_p3, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_spd_mode_p3(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p3, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkdown_p3(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p3, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkup_p3(val)           REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p3, (val))
#define SGMII_STS_CTRL_0_VAL_rg_link_mode_p2(val)              REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_link_mode_p2, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_spd_mode_p2(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p2, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkdown_p2(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p2, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkup_p2(val)           REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p2, (val))
#define SGMII_STS_CTRL_0_VAL_rg_link_mode_p1(val)              REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_link_mode_p1, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_spd_mode_p1(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p1, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkdown_p1(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p1, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkup_p1(val)           REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p1, (val))
#define SGMII_STS_CTRL_0_VAL_rg_link_mode_p0(val)              REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_link_mode_p0, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_spd_mode_p0(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_spd_mode_p0, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkdown_p0(val)         REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkdown_p0, (val))
#define SGMII_STS_CTRL_0_VAL_rg_force_linkup_p0(val)           REG_FLD_VAL(SGMII_STS_CTRL_0_FLD_rg_force_linkup_p0, (val))

#define SGMII_STS_CTRL_1_VAL_rg_tx_d_dummy(val)                REG_FLD_VAL(SGMII_STS_CTRL_1_FLD_rg_tx_d_dummy, (val))

#define SGMII_STS_CTRL_2_VAL_rg_tx_c_dummy(val)                REG_FLD_VAL(SGMII_STS_CTRL_2_FLD_rg_tx_c_dummy, (val))

#define FPGA_STS_CTRL_VAL_rg_fpga_ctrl_31_0(val)               REG_FLD_VAL(FPGA_STS_CTRL_FLD_rg_fpga_ctrl_31_0, (val))

#define SYSYEM_DEBUG_CTRL_VAL_rg_debug_ctrl(val)               REG_FLD_VAL(SYSYEM_DEBUG_CTRL_FLD_rg_debug_ctrl, (val))

#define FPGA_BEN_CTRL_VAL_rg_fpga_ctrl_ben(val)                REG_FLD_VAL(FPGA_BEN_CTRL_FLD_rg_fpga_ctrl_ben, (val))

#define CL22_SETTING_0_VAL_rg_phy_id_1(val)                    REG_FLD_VAL(CL22_SETTING_0_FLD_rg_phy_id_1, (val))
#define CL22_SETTING_0_VAL_rg_phy_id_0(val)                    REG_FLD_VAL(CL22_SETTING_0_FLD_rg_phy_id_0, (val))

#define CL22_SETTING_1_VAL_rg_cl22_rg_status_clr(val)          REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22_rg_status_clr, (val))
#define CL22_SETTING_1_VAL_rg_cl22_an_function_en(val)         REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22_an_function_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_15_en(val)              REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_15_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_14_en(val)              REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_14_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_13_en(val)              REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_13_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_12_en(val)              REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_12_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_11_en(val)              REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_11_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_10_en(val)              REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_10_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_9_en(val)               REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_9_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_8_en(val)               REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_8_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_7_en(val)               REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_7_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_6_en(val)               REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_6_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_5_en(val)               REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_5_en, (val))
#define CL22_SETTING_1_VAL_rg_cl22reg0_4_0_en(val)             REG_FLD_VAL(CL22_SETTING_1_FLD_rg_cl22reg0_4_0_en, (val))

#define CL22_SETTING_2_VAL_rg_an_ability_en_reserve(val)       REG_FLD_VAL(CL22_SETTING_2_FLD_rg_an_ability_en_reserve, (val))
#define CL22_SETTING_2_VAL_rg_uni_direct_en_reserve(val)       REG_FLD_VAL(CL22_SETTING_2_FLD_rg_uni_direct_en_reserve, (val))
#define CL22_SETTING_2_VAL_rg_ext_cap(val)                     REG_FLD_VAL(CL22_SETTING_2_FLD_rg_ext_cap, (val))
#define CL22_SETTING_2_VAL_rg_jabber_det(val)                  REG_FLD_VAL(CL22_SETTING_2_FLD_rg_jabber_det, (val))
#define CL22_SETTING_2_VAL_rg_mf_pream_sup(val)                REG_FLD_VAL(CL22_SETTING_2_FLD_rg_mf_pream_sup, (val))
#define CL22_SETTING_2_VAL_rg_ext_status(val)                  REG_FLD_VAL(CL22_SETTING_2_FLD_rg_ext_status, (val))
#define CL22_SETTING_2_VAL_rg_100base_t2_hdx(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_100base_t2_hdx, (val))
#define CL22_SETTING_2_VAL_rg_100base_t2_fdx(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_100base_t2_fdx, (val))
#define CL22_SETTING_2_VAL_rg_10mbps_hdx(val)                  REG_FLD_VAL(CL22_SETTING_2_FLD_rg_10mbps_hdx, (val))
#define CL22_SETTING_2_VAL_rg_10mbps_fdx(val)                  REG_FLD_VAL(CL22_SETTING_2_FLD_rg_10mbps_fdx, (val))
#define CL22_SETTING_2_VAL_rg_100base_x_hdx(val)               REG_FLD_VAL(CL22_SETTING_2_FLD_rg_100base_x_hdx, (val))
#define CL22_SETTING_2_VAL_rg_100base_x_fdx(val)               REG_FLD_VAL(CL22_SETTING_2_FLD_rg_100base_x_fdx, (val))
#define CL22_SETTING_2_VAL_rg_100base_t4(val)                  REG_FLD_VAL(CL22_SETTING_2_FLD_rg_100base_t4, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg7_15_en(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg7_15_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg7_14_en(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg7_14_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg7_13_en(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg7_13_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg7_12_en(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg7_12_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg7_11_en(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg7_11_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg7_10_0_en(val)            REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg7_10_0_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg4_15_en(val)              REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg4_15_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg4_13_12_en(val)           REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg4_13_12_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg4_8_7_en(val)             REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg4_8_7_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg4_6_en(val)               REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg4_6_en, (val))
#define CL22_SETTING_2_VAL_rg_cl22_rg4_5_en(val)               REG_FLD_VAL(CL22_SETTING_2_FLD_rg_cl22_rg4_5_en, (val))

#define CL22_STS_0_VAL_ro_cl22reg0_15(val)                     REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_15, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_14(val)                     REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_14, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_13(val)                     REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_13, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_12(val)                     REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_12, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_11(val)                     REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_11, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_10(val)                     REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_10, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_9(val)                      REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_9, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_8(val)                      REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_8, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_7(val)                      REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_7, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_6(val)                      REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_6, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_5(val)                      REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_5, (val))
#define CL22_STS_0_VAL_ro_cl22reg0_4_0(val)                    REG_FLD_VAL(CL22_STS_0_FLD_ro_cl22reg0_4_0, (val))

#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_ovrun_intr_en(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ovrun_intr_en, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_undrun_intr_en(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_undrun_intr_en, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_odd_nibble_pream_deal_en(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_odd_nibble_pream_deal_en, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_rxer_mask(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rxer_mask, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_lpi2idle_en(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_lpi2idle_en, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_eee_test_mode(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_eee_test_mode, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_ss_lwm(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_lwm, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_ss_hwm(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_hwm, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_ss_mode(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_ss_mode, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_tx_cnt_clr(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_cnt_clr, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_rx_cnt_clr(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_cnt_clr, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_tx_rst_n(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_tx_rst_n, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_0_VAL_rg_mac_t2r_fifo_rx_rst_n(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_0_FLD_rg_mac_t2r_fifo_rx_rst_n, (val))

#define MAC_T2R_LPBK_FIFO_CTRL_1_VAL_rg_mac_t2r_fifo_runt_ipg_num(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_runt_ipg_num, (val))
#define MAC_T2R_LPBK_FIFO_CTRL_1_VAL_rg_mac_t2r_fifo_min_ipg_num(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_CTRL_1_FLD_rg_mac_t2r_fifo_min_ipg_num, (val))

#define MAC_T2R_LPBK_FIFO_STS_0_VAL_ro_mac_t2r_fifo_rx_start_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_0_FLD_ro_mac_t2r_fifo_rx_start_cnt, (val))

#define MAC_T2R_LPBK_FIFO_STS_1_VAL_ro_mac_t2r_fifo_tx_start_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_1_FLD_ro_mac_t2r_fifo_tx_start_cnt, (val))

#define MAC_T2R_LPBK_FIFO_STS_2_VAL_ro_mac_t2r_fifo_rx_termi_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_2_FLD_ro_mac_t2r_fifo_rx_termi_cnt, (val))

#define MAC_T2R_LPBK_FIFO_STS_3_VAL_ro_mac_t2r_fifo_tx_termi_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_3_FLD_ro_mac_t2r_fifo_tx_termi_cnt, (val))

#define MAC_T2R_LPBK_FIFO_STS_4_VAL_ro_mac_t2r_fifo_ipg_del_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_4_FLD_ro_mac_t2r_fifo_ipg_del_cnt, (val))

#define MAC_T2R_LPBK_FIFO_STS_5_VAL_ro_mac_t2r_fifo_ipg_ins_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_5_FLD_ro_mac_t2r_fifo_ipg_ins_cnt, (val))

#define MAC_T2R_LPBK_FIFO_STS_6_VAL_ro_mac_t2r_fifo_rx_runt_ipg_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_6_FLD_ro_mac_t2r_fifo_rx_runt_ipg_cnt, (val))

#define MAC_T2R_LPBK_FIFO_STS_7_VAL_ro_mac_t2r_fifo_tx_runt_ipg_cnt(val) REG_FLD_VAL(MAC_T2R_LPBK_FIFO_STS_7_FLD_ro_mac_t2r_fifo_tx_runt_ipg_cnt, (val))

#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_ovrun_intr_en(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ovrun_intr_en, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_undrun_intr_en(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_undrun_intr_en, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_odd_nibble_pream_deal_en(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_odd_nibble_pream_deal_en, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_rxer_mask(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rxer_mask, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_lpi2idle_en(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_lpi2idle_en, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_eee_test_mode(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_eee_test_mode, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_ss_lwm(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_lwm, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_ss_hwm(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_hwm, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_ss_mode(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_ss_mode, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_tx_cnt_clr(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_cnt_clr, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_rx_cnt_clr(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_cnt_clr, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_en(val)   REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_en, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_tx_rst_n(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_tx_rst_n, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_0_VAL_rg_phy_r2t_fifo_rx_rst_n(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_0_FLD_rg_phy_r2t_fifo_rx_rst_n, (val))

#define PHY_R2T_LPBK_FIFO_CTRL_1_VAL_rg_phy_r2t_fifo_runt_ipg_num(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_runt_ipg_num, (val))
#define PHY_R2T_LPBK_FIFO_CTRL_1_VAL_rg_phy_r2t_fifo_min_ipg_num(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_CTRL_1_FLD_rg_phy_r2t_fifo_min_ipg_num, (val))

#define PHY_R2T_LPBK_FIFO_STS_0_VAL_ro_phy_r2t_fifo_rx_start_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_0_FLD_ro_phy_r2t_fifo_rx_start_cnt, (val))

#define PHY_R2T_LPBK_FIFO_STS_1_VAL_ro_phy_r2t_fifo_tx_start_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_1_FLD_ro_phy_r2t_fifo_tx_start_cnt, (val))

#define PHY_R2T_LPBK_FIFO_STS_2_VAL_ro_phy_r2t_fifo_rx_termi_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_2_FLD_ro_phy_r2t_fifo_rx_termi_cnt, (val))

#define PHY_R2T_LPBK_FIFO_STS_3_VAL_ro_phy_r2t_fifo_tx_termi_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_3_FLD_ro_phy_r2t_fifo_tx_termi_cnt, (val))

#define PHY_R2T_LPBK_FIFO_STS_4_VAL_ro_phy_r2t_fifo_ipg_del_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_4_FLD_ro_phy_r2t_fifo_ipg_del_cnt, (val))

#define PHY_R2T_LPBK_FIFO_STS_5_VAL_ro_phy_r2t_fifo_ipg_ins_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_5_FLD_ro_phy_r2t_fifo_ipg_ins_cnt, (val))

#define PHY_R2T_LPBK_FIFO_STS_6_VAL_ro_phy_r2t_fifo_rx_runt_ipg_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_6_FLD_ro_phy_r2t_fifo_rx_runt_ipg_cnt, (val))

#define PHY_R2T_LPBK_FIFO_STS_7_VAL_ro_phy_r2t_fifo_tx_runt_ipg_cnt(val) REG_FLD_VAL(PHY_R2T_LPBK_FIFO_STS_7_FLD_ro_phy_r2t_fifo_tx_runt_ipg_cnt, (val))

#define PHY_TX_FORCE_CTRL_0_VAL_rg_force_txer_sel(val)         REG_FLD_VAL(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txer_sel, (val))
#define PHY_TX_FORCE_CTRL_0_VAL_rg_force_txen_sel(val)         REG_FLD_VAL(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txen_sel, (val))
#define PHY_TX_FORCE_CTRL_0_VAL_rg_force_txc_sel(val)          REG_FLD_VAL(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txc_sel, (val))
#define PHY_TX_FORCE_CTRL_0_VAL_rg_force_txd_sel(val)          REG_FLD_VAL(PHY_TX_FORCE_CTRL_0_FLD_rg_force_txd_sel, (val))

#define PHY_TX_FORCE_CTRL_1_VAL_rg_force_txd_h(val)            REG_FLD_VAL(PHY_TX_FORCE_CTRL_1_FLD_rg_force_txd_h, (val))

#define PHY_TX_FORCE_CTRL_2_VAL_rg_force_txd_l(val)            REG_FLD_VAL(PHY_TX_FORCE_CTRL_2_FLD_rg_force_txd_l, (val))

#define PHY_TX_FORCE_CTRL_3_VAL_rg_force_txer(val)             REG_FLD_VAL(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txer, (val))
#define PHY_TX_FORCE_CTRL_3_VAL_rg_force_txen(val)             REG_FLD_VAL(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txen, (val))
#define PHY_TX_FORCE_CTRL_3_VAL_rg_force_txc(val)              REG_FLD_VAL(PHY_TX_FORCE_CTRL_3_FLD_rg_force_txc, (val))

#define msg_rx_ctrl_0_VAL_rg_hsgmii_xfi_sel(val)               REG_FLD_VAL(msg_rx_ctrl_0_FLD_rg_hsgmii_xfi_sel, (val))
#define msg_rx_ctrl_0_VAL_rg_qsgmii_port_shift(val)            REG_FLD_VAL(msg_rx_ctrl_0_FLD_rg_qsgmii_port_shift, (val))
#define msg_rx_ctrl_0_VAL_rg_dec_err_1t(val)                   REG_FLD_VAL(msg_rx_ctrl_0_FLD_rg_dec_err_1t, (val))
#define msg_rx_ctrl_0_VAL_rg_sw_reset_rxck(val)                REG_FLD_VAL(msg_rx_ctrl_0_FLD_rg_sw_reset_rxck, (val))
#define msg_rx_ctrl_0_VAL_rg_rx_xgmii_mode(val)                REG_FLD_VAL(msg_rx_ctrl_0_FLD_rg_rx_xgmii_mode, (val))
#define msg_rx_ctrl_0_VAL_rg_rx_5g_mode(val)                   REG_FLD_VAL(msg_rx_ctrl_0_FLD_rg_rx_5g_mode, (val))
#define msg_rx_ctrl_0_VAL_rg_rx_clk_en(val)                    REG_FLD_VAL(msg_rx_ctrl_0_FLD_rg_rx_clk_en, (val))

#define multi_sgmii_reg_phya_65_VAL_rg_tbi_20_tx_reverse_all(val) REG_FLD_VAL(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_all, (val))
#define multi_sgmii_reg_phya_65_VAL_rg_tbi_20_rx_reverse_all(val) REG_FLD_VAL(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_all, (val))
#define multi_sgmii_reg_phya_65_VAL_rg_tbi_10_rx_reverse_pma(val) REG_FLD_VAL(multi_sgmii_reg_phya_65_FLD_rg_tbi_10_rx_reverse_pma, (val))
#define multi_sgmii_reg_phya_65_VAL_rg_tbi_20_rx_reverse_pma(val) REG_FLD_VAL(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_rx_reverse_pma, (val))
#define multi_sgmii_reg_phya_65_VAL_rg_tbi_tx_reverse_10b_pma(val) REG_FLD_VAL(multi_sgmii_reg_phya_65_FLD_rg_tbi_tx_reverse_10b_pma, (val))
#define multi_sgmii_reg_phya_65_VAL_rg_tbi_20_tx_reverse_pma(val) REG_FLD_VAL(multi_sgmii_reg_phya_65_FLD_rg_tbi_20_tx_reverse_pma, (val))

#define multi_sgmii_reg_interrupt_sel_VAL_rg_hsgmii_phy_int(val) REG_FLD_VAL(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int, (val))
#define multi_sgmii_reg_interrupt_sel_VAL_rg_hsgmii_phy_int_force(val) REG_FLD_VAL(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_phy_int_force, (val))
#define multi_sgmii_reg_interrupt_sel_VAL_rg_rxer100_int(val)  REG_FLD_VAL(multi_sgmii_reg_interrupt_sel_FLD_rg_rxer100_int, (val))
#define multi_sgmii_reg_interrupt_sel_VAL_rg_false_carrier_int(val) REG_FLD_VAL(multi_sgmii_reg_interrupt_sel_FLD_rg_false_carrier_int, (val))
#define multi_sgmii_reg_interrupt_sel_VAL_rg_symbol_error_int(val) REG_FLD_VAL(multi_sgmii_reg_interrupt_sel_FLD_rg_symbol_error_int, (val))
#define multi_sgmii_reg_interrupt_sel_VAL_rg_100fx_int(val)    REG_FLD_VAL(multi_sgmii_reg_interrupt_sel_FLD_rg_100fx_int, (val))
#define multi_sgmii_reg_interrupt_sel_VAL_rg_hsgmii_pcs_int(val) REG_FLD_VAL(multi_sgmii_reg_interrupt_sel_FLD_rg_hsgmii_pcs_int, (val))

#define multi_sgmii_reg_probe_sel_VAL_rg_probe_clk_sel(val)    REG_FLD_VAL(multi_sgmii_reg_probe_sel_FLD_rg_probe_clk_sel, (val))
#define multi_sgmii_reg_probe_sel_VAL_rg_hsgmii_probe_port_sel(val) REG_FLD_VAL(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_port_sel, (val))
#define multi_sgmii_reg_probe_sel_VAL_rg_dwn_shift_probe_sel(val) REG_FLD_VAL(multi_sgmii_reg_probe_sel_FLD_rg_dwn_shift_probe_sel, (val))
#define multi_sgmii_reg_probe_sel_VAL_rg_hsgmii_clk_probe_sel(val) REG_FLD_VAL(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_clk_probe_sel, (val))
#define multi_sgmii_reg_probe_sel_VAL_rg_100fx_probe_sel(val)  REG_FLD_VAL(multi_sgmii_reg_probe_sel_FLD_rg_100fx_probe_sel, (val))
#define multi_sgmii_reg_probe_sel_VAL_rg_hsgmii_probe_sel(val) REG_FLD_VAL(multi_sgmii_reg_probe_sel_FLD_rg_hsgmii_probe_sel, (val))

#define multi_sgmii_fpga_mode_control_VAL_rg_hwtrap_100fx_mux_en(val) REG_FLD_VAL(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux_en, (val))
#define multi_sgmii_fpga_mode_control_VAL_rg_hwtrap_100fx_mux(val) REG_FLD_VAL(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_mux, (val))
#define multi_sgmii_fpga_mode_control_VAL_rg_hwtrap_100fx_en(val) REG_FLD_VAL(multi_sgmii_fpga_mode_control_FLD_rg_hwtrap_100fx_en, (val))

#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_write_enable_or(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_or, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_read_enable_or(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_or, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_write_enable_and(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_write_enable_and, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_read_enable_and(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_read_enable_and, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_aempty_cnt_clear(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_aempty_cnt_clear, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_empty_cnt_clear(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_empty_cnt_clear, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_afull_cnt_clear(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_afull_cnt_clear, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_full_cnt_clear(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_full_cnt_clear, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_w_threshold(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_w_threshold, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_r_threshold(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_r_threshold, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_rclear(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_rclear, (val))
#define multi_sgmii_async_fifo_control_VAL_rg_hsgmii_async_fifo_wclear(val) REG_FLD_VAL(multi_sgmii_async_fifo_control_FLD_rg_hsgmii_async_fifo_wclear, (val))

#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_rfifo_cnt(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_rfifo_cnt, (val))
#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_wfifo_cnt(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_wfifo_cnt, (val))
#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_w_ready(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_w_ready, (val))
#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_r_ready(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_r_ready, (val))
#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_aempty(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_aempty, (val))
#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_empty(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_empty, (val))
#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_afull(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_afull, (val))
#define multi_sgmii_ro_phya_afifo_VAL_ro_hsgmii_async_fifo_full(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_FLD_ro_hsgmii_async_fifo_full, (val))

#define multi_sgmii_ro_phya_afifo_cnt_0_VAL_ro_hsgmii_async_fifo_full_cnt(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_cnt_0_FLD_ro_hsgmii_async_fifo_full_cnt, (val))

#define multi_sgmii_ro_phya_afifo_cnt_1_VAL_ro_hsgmii_async_fifo_afull_cnt(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_cnt_1_FLD_ro_hsgmii_async_fifo_afull_cnt, (val))

#define multi_sgmii_ro_phya_afifo_cnt_2_VAL_ro_hsgmii_async_fifo_empty_cnt(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_cnt_2_FLD_ro_hsgmii_async_fifo_empty_cnt, (val))

#define multi_sgmii_ro_phya_afifo_cnt_3_VAL_ro_hsgmii_async_fifo_aempty_cnt(val) REG_FLD_VAL(multi_sgmii_ro_phya_afifo_cnt_3_FLD_ro_hsgmii_async_fifo_aempty_cnt, (val))

#define msg_led_ctrl_0_VAL_rg_led_timing_test(val)             REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led_timing_test, (val))
#define msg_led_ctrl_0_VAL_rg_dwn_shift_en(val)                REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_dwn_shift_en, (val))
#define msg_led_ctrl_0_VAL_rg_dwn_shift_blk_en(val)            REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_dwn_shift_blk_en, (val))
#define msg_led_ctrl_0_VAL_rg_led_enhance(val)                 REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led_enhance, (val))
#define msg_led_ctrl_0_VAL_rg_led_mode(val)                    REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led_mode, (val))
#define msg_led_ctrl_0_VAL_rg_led3_pol(val)                    REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led3_pol, (val))
#define msg_led_ctrl_0_VAL_rg_led2_pol(val)                    REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led2_pol, (val))
#define msg_led_ctrl_0_VAL_rg_led1_pol(val)                    REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led1_pol, (val))
#define msg_led_ctrl_0_VAL_rg_led0_pol(val)                    REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led0_pol, (val))
#define msg_led_ctrl_0_VAL_rg_led3_en(val)                     REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led3_en, (val))
#define msg_led_ctrl_0_VAL_rg_led2_en(val)                     REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led2_en, (val))
#define msg_led_ctrl_0_VAL_rg_led1_en(val)                     REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led1_en, (val))
#define msg_led_ctrl_0_VAL_rg_led0_en(val)                     REG_FLD_VAL(msg_led_ctrl_0_FLD_rg_led0_en, (val))

#define msg_led_ctrl_1_VAL_rg_led_blk_dur(val)                 REG_FLD_VAL(msg_led_ctrl_1_FLD_rg_led_blk_dur, (val))
#define msg_led_ctrl_1_VAL_rg_led_on_dur(val)                  REG_FLD_VAL(msg_led_ctrl_1_FLD_rg_led_on_dur, (val))

#define msg_led_ctrl_2_VAL_rg_led1_blk_mask(val)               REG_FLD_VAL(msg_led_ctrl_2_FLD_rg_led1_blk_mask, (val))
#define msg_led_ctrl_2_VAL_rg_led1_on_mask(val)                REG_FLD_VAL(msg_led_ctrl_2_FLD_rg_led1_on_mask, (val))
#define msg_led_ctrl_2_VAL_rg_led0_blk_mask(val)               REG_FLD_VAL(msg_led_ctrl_2_FLD_rg_led0_blk_mask, (val))
#define msg_led_ctrl_2_VAL_rg_led0_on_mask(val)                REG_FLD_VAL(msg_led_ctrl_2_FLD_rg_led0_on_mask, (val))

#define msg_led_ctrl_3_VAL_rg_led3_blk_mask(val)               REG_FLD_VAL(msg_led_ctrl_3_FLD_rg_led3_blk_mask, (val))
#define msg_led_ctrl_3_VAL_rg_led3_on_mask(val)                REG_FLD_VAL(msg_led_ctrl_3_FLD_rg_led3_on_mask, (val))
#define msg_led_ctrl_3_VAL_rg_led2_blk_mask(val)               REG_FLD_VAL(msg_led_ctrl_3_FLD_rg_led2_blk_mask, (val))
#define msg_led_ctrl_3_VAL_rg_led2_on_mask(val)                REG_FLD_VAL(msg_led_ctrl_3_FLD_rg_led2_on_mask, (val))

#define msg_led_sts_0_VAL_ro_led_blk_event(val)                REG_FLD_VAL(msg_led_sts_0_FLD_ro_led_blk_event, (val))
#define msg_led_sts_0_VAL_ro_led_on_event(val)                 REG_FLD_VAL(msg_led_sts_0_FLD_ro_led_on_event, (val))

#define msg_rx_sts_0_VAL_ro_k28_1_cnt(val)                     REG_FLD_VAL(msg_rx_sts_0_FLD_ro_k28_1_cnt, (val))

#define msg_rx_sts_1_VAL_ro_port0_k28_1_cnt(val)               REG_FLD_VAL(msg_rx_sts_1_FLD_ro_port0_k28_1_cnt, (val))

#define msg_rx_sts_2_VAL_ro_port1_k28_1_cnt(val)               REG_FLD_VAL(msg_rx_sts_2_FLD_ro_port1_k28_1_cnt, (val))

#define msg_rx_sts_3_VAL_ro_port2_k28_1_cnt(val)               REG_FLD_VAL(msg_rx_sts_3_FLD_ro_port2_k28_1_cnt, (val))

#define msg_rx_sts_4_VAL_ro_port3_k28_1_cnt(val)               REG_FLD_VAL(msg_rx_sts_4_FLD_ro_port3_k28_1_cnt, (val))

#define msg_rx_sts_5_VAL_ro_rd_out_3b_neg(val)                 REG_FLD_VAL(msg_rx_sts_5_FLD_ro_rd_out_3b_neg, (val))
#define msg_rx_sts_5_VAL_ro_rd_out_3b_pos(val)                 REG_FLD_VAL(msg_rx_sts_5_FLD_ro_rd_out_3b_pos, (val))
#define msg_rx_sts_5_VAL_ro_rd_out_5b_neg(val)                 REG_FLD_VAL(msg_rx_sts_5_FLD_ro_rd_out_5b_neg, (val))
#define msg_rx_sts_5_VAL_ro_rd_out_5b_pos(val)                 REG_FLD_VAL(msg_rx_sts_5_FLD_ro_rd_out_5b_pos, (val))
#define msg_rx_sts_5_VAL_ro_dp3_n(val)                         REG_FLD_VAL(msg_rx_sts_5_FLD_ro_dp3_n, (val))
#define msg_rx_sts_5_VAL_ro_dp3_p(val)                         REG_FLD_VAL(msg_rx_sts_5_FLD_ro_dp3_p, (val))
#define msg_rx_sts_5_VAL_ro_dp5_n(val)                         REG_FLD_VAL(msg_rx_sts_5_FLD_ro_dp5_n, (val))
#define msg_rx_sts_5_VAL_ro_dp5_p(val)                         REG_FLD_VAL(msg_rx_sts_5_FLD_ro_dp5_p, (val))

#define msg_rx_sts_6_VAL_ro_except_okn(val)                    REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except_okn, (val))
#define msg_rx_sts_6_VAL_ro_except_err(val)                    REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except_err, (val))
#define msg_rx_sts_6_VAL_ro_except_negi(val)                   REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except_negi, (val))
#define msg_rx_sts_6_VAL_ro_except_posi(val)                   REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except_posi, (val))
#define msg_rx_sts_6_VAL_ro_except_neg(val)                    REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except_neg, (val))
#define msg_rx_sts_6_VAL_ro_except_pos(val)                    REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except_pos, (val))
#define msg_rx_sts_6_VAL_ro_except5_n(val)                     REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except5_n, (val))
#define msg_rx_sts_6_VAL_ro_except5_p(val)                     REG_FLD_VAL(msg_rx_sts_6_FLD_ro_except5_p, (val))

#define msg_rx_sts_7_VAL_ro_carrier_reg(val)                   REG_FLD_VAL(msg_rx_sts_7_FLD_ro_carrier_reg, (val))
#define msg_rx_sts_7_VAL_ro_rd_out_5b(val)                     REG_FLD_VAL(msg_rx_sts_7_FLD_ro_rd_out_5b, (val))
#define msg_rx_sts_7_VAL_ro_rd_out_5b_negx(val)                REG_FLD_VAL(msg_rx_sts_7_FLD_ro_rd_out_5b_negx, (val))
#define msg_rx_sts_7_VAL_ro_rd_out_5b_posx(val)                REG_FLD_VAL(msg_rx_sts_7_FLD_ro_rd_out_5b_posx, (val))
#define msg_rx_sts_7_VAL_ro_rd_out_3b_negx(val)                REG_FLD_VAL(msg_rx_sts_7_FLD_ro_rd_out_3b_negx, (val))
#define msg_rx_sts_7_VAL_ro_rd_out_3b_posx(val)                REG_FLD_VAL(msg_rx_sts_7_FLD_ro_rd_out_3b_posx, (val))
#define msg_rx_sts_7_VAL_ro_data_out_3b_index(val)             REG_FLD_VAL(msg_rx_sts_7_FLD_ro_data_out_3b_index, (val))

#define msg_rx_sts_8_VAL_ro_error_3b_sc_neg(val)               REG_FLD_VAL(msg_rx_sts_8_FLD_ro_error_3b_sc_neg, (val))
#define msg_rx_sts_8_VAL_ro_error_3b_sc_pos(val)               REG_FLD_VAL(msg_rx_sts_8_FLD_ro_error_3b_sc_pos, (val))
#define msg_rx_sts_8_VAL_ro_error_3b_sc(val)                   REG_FLD_VAL(msg_rx_sts_8_FLD_ro_error_3b_sc, (val))
#define msg_rx_sts_8_VAL_ro_error_kout_5b_neg(val)             REG_FLD_VAL(msg_rx_sts_8_FLD_ro_error_kout_5b_neg, (val))
#define msg_rx_sts_8_VAL_ro_error_kout_5b_pos(val)             REG_FLD_VAL(msg_rx_sts_8_FLD_ro_error_kout_5b_pos, (val))
#define msg_rx_sts_8_VAL_ro_error_kout_5b(val)                 REG_FLD_VAL(msg_rx_sts_8_FLD_ro_error_kout_5b, (val))

#define msg_rx_sts_9_VAL_ro_error_5b_neg(val)                  REG_FLD_VAL(msg_rx_sts_9_FLD_ro_error_5b_neg, (val))
#define msg_rx_sts_9_VAL_ro_error_5b_pos(val)                  REG_FLD_VAL(msg_rx_sts_9_FLD_ro_error_5b_pos, (val))
#define msg_rx_sts_9_VAL_ro_error_5b(val)                      REG_FLD_VAL(msg_rx_sts_9_FLD_ro_error_5b, (val))
#define msg_rx_sts_9_VAL_ro_error_3b(val)                      REG_FLD_VAL(msg_rx_sts_9_FLD_ro_error_3b, (val))

#define msg_rx_sts_10_VAL_ro_data_out_3b(val)                  REG_FLD_VAL(msg_rx_sts_10_FLD_ro_data_out_3b, (val))
#define msg_rx_sts_10_VAL_ro_data_out_5b(val)                  REG_FLD_VAL(msg_rx_sts_10_FLD_ro_data_out_5b, (val))

#define msg_rx_sts_11_VAL_ro_disp_err(val)                     REG_FLD_VAL(msg_rx_sts_11_FLD_ro_disp_err, (val))
#define msg_rx_sts_11_VAL_ro_char_err(val)                     REG_FLD_VAL(msg_rx_sts_11_FLD_ro_char_err, (val))
#define msg_rx_sts_11_VAL_ro_kout_3b_pos(val)                  REG_FLD_VAL(msg_rx_sts_11_FLD_ro_kout_3b_pos, (val))
#define msg_rx_sts_11_VAL_ro_kout_3b_neg(val)                  REG_FLD_VAL(msg_rx_sts_11_FLD_ro_kout_3b_neg, (val))
#define msg_rx_sts_11_VAL_ro_kout_5b_pos(val)                  REG_FLD_VAL(msg_rx_sts_11_FLD_ro_kout_5b_pos, (val))
#define msg_rx_sts_11_VAL_ro_kout_5b_neg(val)                  REG_FLD_VAL(msg_rx_sts_11_FLD_ro_kout_5b_neg, (val))

#define msg_rx_sts_12_VAL_ro_port3_sync_lost(val)              REG_FLD_VAL(msg_rx_sts_12_FLD_ro_port3_sync_lost, (val))
#define msg_rx_sts_12_VAL_ro_port2_sync_lost(val)              REG_FLD_VAL(msg_rx_sts_12_FLD_ro_port2_sync_lost, (val))
#define msg_rx_sts_12_VAL_ro_port1_sync_lost(val)              REG_FLD_VAL(msg_rx_sts_12_FLD_ro_port1_sync_lost, (val))
#define msg_rx_sts_12_VAL_ro_port0_sync_lost(val)              REG_FLD_VAL(msg_rx_sts_12_FLD_ro_port0_sync_lost, (val))
#define msg_rx_sts_12_VAL_ro_sync_int(val)                     REG_FLD_VAL(msg_rx_sts_12_FLD_ro_sync_int, (val))
#define msg_rx_sts_12_VAL_ro_rd_out(val)                       REG_FLD_VAL(msg_rx_sts_12_FLD_ro_rd_out, (val))
#define msg_rx_sts_12_VAL_ro_align_det(val)                    REG_FLD_VAL(msg_rx_sts_12_FLD_ro_align_det, (val))

#define msg_rx_sts_13_VAL_ro_data_align(val)                   REG_FLD_VAL(msg_rx_sts_13_FLD_ro_data_align, (val))

#define msg_rx_sts_14_VAL_ro_par(val)                          REG_FLD_VAL(msg_rx_sts_14_FLD_ro_par, (val))

#define msg_rx_sts_15_VAL_ro_lnk_sts_p3(val)                   REG_FLD_VAL(msg_rx_sts_15_FLD_ro_lnk_sts_p3, (val))
#define msg_rx_sts_15_VAL_ro_speed_sts_p3(val)                 REG_FLD_VAL(msg_rx_sts_15_FLD_ro_speed_sts_p3, (val))
#define msg_rx_sts_15_VAL_ro_lnk_sts_p2(val)                   REG_FLD_VAL(msg_rx_sts_15_FLD_ro_lnk_sts_p2, (val))
#define msg_rx_sts_15_VAL_ro_speed_sts_p2(val)                 REG_FLD_VAL(msg_rx_sts_15_FLD_ro_speed_sts_p2, (val))
#define msg_rx_sts_15_VAL_ro_lnk_sts_p1(val)                   REG_FLD_VAL(msg_rx_sts_15_FLD_ro_lnk_sts_p1, (val))
#define msg_rx_sts_15_VAL_ro_speed_sts_p1(val)                 REG_FLD_VAL(msg_rx_sts_15_FLD_ro_speed_sts_p1, (val))
#define msg_rx_sts_15_VAL_ro_lnk_sts_p0(val)                   REG_FLD_VAL(msg_rx_sts_15_FLD_ro_lnk_sts_p0, (val))
#define msg_rx_sts_15_VAL_ro_speed_sts_p0(val)                 REG_FLD_VAL(msg_rx_sts_15_FLD_ro_speed_sts_p0, (val))

#define msg_rx_sts_16_VAL_ro_p3_rx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p3_rx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_p2_rx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p2_rx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_p1_rx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p1_rx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_p0_rx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p0_rx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_p3_tx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p3_tx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_p2_tx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p2_tx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_p1_tx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p1_tx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_p0_tx_eee_flag_latch(val)         REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p0_tx_eee_flag_latch, (val))
#define msg_rx_sts_16_VAL_ro_sig_detect_down_latch(val)        REG_FLD_VAL(msg_rx_sts_16_FLD_ro_sig_detect_down_latch, (val))
#define msg_rx_sts_16_VAL_ro_sig_detect_up_latch(val)          REG_FLD_VAL(msg_rx_sts_16_FLD_ro_sig_detect_up_latch, (val))
#define msg_rx_sts_16_VAL_ro_p3_link_down_latch(val)           REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p3_link_down_latch, (val))
#define msg_rx_sts_16_VAL_ro_p3_link_up_latch(val)             REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p3_link_up_latch, (val))
#define msg_rx_sts_16_VAL_ro_p2_link_down_latch(val)           REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p2_link_down_latch, (val))
#define msg_rx_sts_16_VAL_ro_p2_link_up_latch(val)             REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p2_link_up_latch, (val))
#define msg_rx_sts_16_VAL_ro_p1_link_down_latch(val)           REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p1_link_down_latch, (val))
#define msg_rx_sts_16_VAL_ro_p1_link_up_latch(val)             REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p1_link_up_latch, (val))
#define msg_rx_sts_16_VAL_ro_p0_link_down_latch(val)           REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p0_link_down_latch, (val))
#define msg_rx_sts_16_VAL_ro_p0_link_up_latch(val)             REG_FLD_VAL(msg_rx_sts_16_FLD_ro_p0_link_up_latch, (val))

#define msg_rx_sts_17_VAL_rg_p3_rx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p3_rx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p2_rx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p2_rx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p1_rx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p1_rx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p0_rx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p0_rx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p3_tx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p3_tx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p2_tx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p2_tx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p1_tx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p1_tx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p0_tx_eee_int_clr(val)            REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p0_tx_eee_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_sig_detect_down_int_clr(val)      REG_FLD_VAL(msg_rx_sts_17_FLD_rg_sig_detect_down_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_sig_detect_up_int_clr(val)        REG_FLD_VAL(msg_rx_sts_17_FLD_rg_sig_detect_up_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p3_link_down_int_clr(val)         REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p3_link_down_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p3_link_up_int_clr(val)           REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p3_link_up_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p2_link_down_int_clr(val)         REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p2_link_down_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p2_link_up_int_clr(val)           REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p2_link_up_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p1_link_down_int_clr(val)         REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p1_link_down_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p1_link_up_int_clr(val)           REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p1_link_up_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p0_link_down_int_clr(val)         REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p0_link_down_int_clr, (val))
#define msg_rx_sts_17_VAL_rg_p0_link_up_int_clr(val)           REG_FLD_VAL(msg_rx_sts_17_FLD_rg_p0_link_up_int_clr, (val))

#define msg_rx_sts_18_VAL_p3_sgmii_is_10(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p3_sgmii_is_10, (val))
#define msg_rx_sts_18_VAL_p3_sgmii_is_100(val)                 REG_FLD_VAL(msg_rx_sts_18_FLD_p3_sgmii_is_100, (val))
#define msg_rx_sts_18_VAL_p3_sgmii_is_1g(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p3_sgmii_is_1g, (val))
#define msg_rx_sts_18_VAL_p2_sgmii_is_10(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p2_sgmii_is_10, (val))
#define msg_rx_sts_18_VAL_p2_sgmii_is_100(val)                 REG_FLD_VAL(msg_rx_sts_18_FLD_p2_sgmii_is_100, (val))
#define msg_rx_sts_18_VAL_p2_sgmii_is_1g(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p2_sgmii_is_1g, (val))
#define msg_rx_sts_18_VAL_p1_sgmii_is_10(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p1_sgmii_is_10, (val))
#define msg_rx_sts_18_VAL_p1_sgmii_is_100(val)                 REG_FLD_VAL(msg_rx_sts_18_FLD_p1_sgmii_is_100, (val))
#define msg_rx_sts_18_VAL_p1_sgmii_is_1g(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p1_sgmii_is_1g, (val))
#define msg_rx_sts_18_VAL_p0_sgmii_is_10(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p0_sgmii_is_10, (val))
#define msg_rx_sts_18_VAL_p0_sgmii_is_100(val)                 REG_FLD_VAL(msg_rx_sts_18_FLD_p0_sgmii_is_100, (val))
#define msg_rx_sts_18_VAL_p0_sgmii_is_1g(val)                  REG_FLD_VAL(msg_rx_sts_18_FLD_p0_sgmii_is_1g, (val))

#define msg_rx_sts_19_VAL_p1_lp_ability(val)                   REG_FLD_VAL(msg_rx_sts_19_FLD_p1_lp_ability, (val))
#define msg_rx_sts_19_VAL_p0_lp_ability(val)                   REG_FLD_VAL(msg_rx_sts_19_FLD_p0_lp_ability, (val))

#define msg_rx_sts_20_VAL_p3_lp_ability(val)                   REG_FLD_VAL(msg_rx_sts_20_FLD_p3_lp_ability, (val))
#define msg_rx_sts_20_VAL_p2_lp_ability(val)                   REG_FLD_VAL(msg_rx_sts_20_FLD_p2_lp_ability, (val))

#define cl22_status_0_VAL_ro_cl22_reset_wrtie_flag(val)        REG_FLD_VAL(cl22_status_0_FLD_ro_cl22_reset_wrtie_flag, (val))
#define cl22_status_0_VAL_ro_cl22_wrtie_flag(val)              REG_FLD_VAL(cl22_status_0_FLD_ro_cl22_wrtie_flag, (val))

#define cl22_status_1_VAL_rg_cl22_reset_wrtie_flag_clr(val)    REG_FLD_VAL(cl22_status_1_FLD_rg_cl22_reset_wrtie_flag_clr, (val))
#define cl22_status_1_VAL_rg_cl22_wrtie_flag_clr(val)          REG_FLD_VAL(cl22_status_1_FLD_rg_cl22_wrtie_flag_clr, (val))

#define msg_rx_sts_21_VAL_rg_rx_eee_flag_lev_p3_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p3_clr, (val))
#define msg_rx_sts_21_VAL_rg_rx_eee_flag_lev_p2_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p2_clr, (val))
#define msg_rx_sts_21_VAL_rg_rx_eee_flag_lev_p1_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p1_clr, (val))
#define msg_rx_sts_21_VAL_rg_rx_eee_flag_lev_p0_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_rx_eee_flag_lev_p0_clr, (val))
#define msg_rx_sts_21_VAL_rg_tx_eee_flag_lev_p3_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p3_clr, (val))
#define msg_rx_sts_21_VAL_rg_tx_eee_flag_lev_p2_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p2_clr, (val))
#define msg_rx_sts_21_VAL_rg_tx_eee_flag_lev_p1_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p1_clr, (val))
#define msg_rx_sts_21_VAL_rg_tx_eee_flag_lev_p0_clr(val)       REG_FLD_VAL(msg_rx_sts_21_FLD_rg_tx_eee_flag_lev_p0_clr, (val))

#define msg_rx_ctrl_1_VAL_rg_force_port3_cnt(val)              REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port3_cnt, (val))
#define msg_rx_ctrl_1_VAL_rg_force_port2_cnt(val)              REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port2_cnt, (val))
#define msg_rx_ctrl_1_VAL_rg_force_port1_cnt(val)              REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port1_cnt, (val))
#define msg_rx_ctrl_1_VAL_rg_force_port0_cnt(val)              REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port0_cnt, (val))
#define msg_rx_ctrl_1_VAL_rg_force_port3_cnt_sel(val)          REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port3_cnt_sel, (val))
#define msg_rx_ctrl_1_VAL_rg_force_port2_cnt_sel(val)          REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port2_cnt_sel, (val))
#define msg_rx_ctrl_1_VAL_rg_force_port1_cnt_sel(val)          REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port1_cnt_sel, (val))
#define msg_rx_ctrl_1_VAL_rg_force_port0_cnt_sel(val)          REG_FLD_VAL(msg_rx_ctrl_1_FLD_rg_force_port0_cnt_sel, (val))

#define msg_rx_ctrl_2_VAL_rg_force_port3_sync(val)             REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port3_sync, (val))
#define msg_rx_ctrl_2_VAL_rg_force_port2_sync(val)             REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port2_sync, (val))
#define msg_rx_ctrl_2_VAL_rg_force_port1_sync(val)             REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port1_sync, (val))
#define msg_rx_ctrl_2_VAL_rg_force_port0_sync(val)             REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port0_sync, (val))
#define msg_rx_ctrl_2_VAL_rg_force_port3_sync_sel(val)         REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port3_sync_sel, (val))
#define msg_rx_ctrl_2_VAL_rg_force_port2_sync_sel(val)         REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port2_sync_sel, (val))
#define msg_rx_ctrl_2_VAL_rg_force_port1_sync_sel(val)         REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port1_sync_sel, (val))
#define msg_rx_ctrl_2_VAL_rg_force_port0_sync_sel(val)         REG_FLD_VAL(msg_rx_ctrl_2_FLD_rg_force_port0_sync_sel, (val))

#define msg_rx_ctrl_3_VAL_rg_k28_1_cnt_clr(val)                REG_FLD_VAL(msg_rx_ctrl_3_FLD_rg_k28_1_cnt_clr, (val))
#define msg_rx_ctrl_3_VAL_rg_port3_k28_1_cnt_clr(val)          REG_FLD_VAL(msg_rx_ctrl_3_FLD_rg_port3_k28_1_cnt_clr, (val))
#define msg_rx_ctrl_3_VAL_rg_port2_k28_1_cnt_clr(val)          REG_FLD_VAL(msg_rx_ctrl_3_FLD_rg_port2_k28_1_cnt_clr, (val))
#define msg_rx_ctrl_3_VAL_rg_port1_k28_1_cnt_clr(val)          REG_FLD_VAL(msg_rx_ctrl_3_FLD_rg_port1_k28_1_cnt_clr, (val))
#define msg_rx_ctrl_3_VAL_rg_port0_k28_1_cnt_clr(val)          REG_FLD_VAL(msg_rx_ctrl_3_FLD_rg_port0_k28_1_cnt_clr, (val))

#define msg_rx_ctrl_4_VAL_rg_qsgmii_rx500_pcs_div_ck_sel(val)  REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_qsgmii_rx500_pcs_div_ck_sel, (val))
#define msg_rx_ctrl_4_VAL_rg_p3_mac_rx_div_ck_reset_n(val)     REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_p3_mac_rx_div_ck_reset_n, (val))
#define msg_rx_ctrl_4_VAL_rg_p2_mac_rx_div_ck_reset_n(val)     REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_p2_mac_rx_div_ck_reset_n, (val))
#define msg_rx_ctrl_4_VAL_rg_p1_mac_rx_div_ck_reset_n(val)     REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_p1_mac_rx_div_ck_reset_n, (val))
#define msg_rx_ctrl_4_VAL_rg_qsgmii_p3_rx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_qsgmii_p3_rx500_mac_div_ck_sel, (val))
#define msg_rx_ctrl_4_VAL_rg_qsgmii_p2_rx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_qsgmii_p2_rx500_mac_div_ck_sel, (val))
#define msg_rx_ctrl_4_VAL_rg_qsgmii_p1_rx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_qsgmii_p1_rx500_mac_div_ck_sel, (val))
#define msg_rx_ctrl_4_VAL_rg_qsgmii_p0_rx500_mac_div_ck_sel(val) REG_FLD_VAL(msg_rx_ctrl_4_FLD_rg_qsgmii_p0_rx500_mac_div_ck_sel, (val))

#define msg_rx_ctrl_5_VAL_rg_fx_crs_test(val)                  REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_fx_crs_test, (val))
#define msg_rx_ctrl_5_VAL_rg_fx_col_test(val)                  REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_fx_col_test, (val))
#define msg_rx_ctrl_5_VAL_rg_hg_crs_test(val)                  REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_hg_crs_test, (val))
#define msg_rx_ctrl_5_VAL_rg_hg_col_test(val)                  REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_hg_col_test, (val))
#define msg_rx_ctrl_5_VAL_rg_crs_is_fx_mode(val)               REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_crs_is_fx_mode, (val))
#define msg_rx_ctrl_5_VAL_rg_col_is_fx_mode(val)               REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_col_is_fx_mode, (val))
#define msg_rx_ctrl_5_VAL_rg_fx_crs_sel(val)                   REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_fx_crs_sel, (val))
#define msg_rx_ctrl_5_VAL_rg_fx_col_sel(val)                   REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_fx_col_sel, (val))
#define msg_rx_ctrl_5_VAL_rg_fx_crs_test_mode(val)             REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_fx_crs_test_mode, (val))
#define msg_rx_ctrl_5_VAL_rg_fx_col_test_mode(val)             REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_fx_col_test_mode, (val))
#define msg_rx_ctrl_5_VAL_rg_hg_crs_test_mode(val)             REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_hg_crs_test_mode, (val))
#define msg_rx_ctrl_5_VAL_rg_hg_col_test_mode(val)             REG_FLD_VAL(msg_rx_ctrl_5_FLD_rg_hg_col_test_mode, (val))

#define msg_rx_lik_sts_0_VAL_rg_eee2p5g_sts(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee2p5g_sts, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee1g_sts_p3(val)              REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p3, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee100_sts_p3(val)             REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p3, (val))
#define msg_rx_lik_sts_0_VAL_rg_txfc_sts_p3(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p3, (val))
#define msg_rx_lik_sts_0_VAL_rg_rxfc_sts_p3(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p3, (val))
#define msg_rx_lik_sts_0_VAL_rg_dpx_sts_p3(val)                REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p3, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee1g_sts_p2(val)              REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p2, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee100_sts_p2(val)             REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p2, (val))
#define msg_rx_lik_sts_0_VAL_rg_txfc_sts_p2(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p2, (val))
#define msg_rx_lik_sts_0_VAL_rg_rxfc_sts_p2(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p2, (val))
#define msg_rx_lik_sts_0_VAL_rg_dpx_sts_p2(val)                REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p2, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee1g_sts_p1(val)              REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p1, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee100_sts_p1(val)             REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p1, (val))
#define msg_rx_lik_sts_0_VAL_rg_txfc_sts_p1(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p1, (val))
#define msg_rx_lik_sts_0_VAL_rg_rxfc_sts_p1(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p1, (val))
#define msg_rx_lik_sts_0_VAL_rg_dpx_sts_p1(val)                REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p1, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee1g_sts_p0(val)              REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee1g_sts_p0, (val))
#define msg_rx_lik_sts_0_VAL_rg_eee100_sts_p0(val)             REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_eee100_sts_p0, (val))
#define msg_rx_lik_sts_0_VAL_rg_txfc_sts_p0(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_txfc_sts_p0, (val))
#define msg_rx_lik_sts_0_VAL_rg_rxfc_sts_p0(val)               REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_rxfc_sts_p0, (val))
#define msg_rx_lik_sts_0_VAL_rg_dpx_sts_p0(val)                REG_FLD_VAL(msg_rx_lik_sts_0_FLD_rg_dpx_sts_p0, (val))

#define msg_rx_lik_sts_1_VAL_rg_pause_sts_p3(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_pause_sts_p3, (val))
#define msg_rx_lik_sts_1_VAL_rg_pause_sts_p2(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_pause_sts_p2, (val))
#define msg_rx_lik_sts_1_VAL_rg_pause_sts_p1(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_pause_sts_p1, (val))
#define msg_rx_lik_sts_1_VAL_rg_pause_sts_p0(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_pause_sts_p0, (val))
#define msg_rx_lik_sts_1_VAL_rg_lp_an_cap_p3(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p3, (val))
#define msg_rx_lik_sts_1_VAL_rg_lp_an_cap_p2(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p2, (val))
#define msg_rx_lik_sts_1_VAL_rg_lp_an_cap_p1(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p1, (val))
#define msg_rx_lik_sts_1_VAL_rg_lp_an_cap_p0(val)              REG_FLD_VAL(msg_rx_lik_sts_1_FLD_rg_lp_an_cap_p0, (val))

#define msg_rx_lik_sts_2_VAL_rg_rxfc_an_bypass_p3(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p3, (val))
#define msg_rx_lik_sts_2_VAL_rg_rxfc_an_bypass_p2(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p2, (val))
#define msg_rx_lik_sts_2_VAL_rg_rxfc_an_bypass_p1(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p1, (val))
#define msg_rx_lik_sts_2_VAL_rg_rxfc_an_bypass_p0(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_rxfc_an_bypass_p0, (val))
#define msg_rx_lik_sts_2_VAL_rg_txfc_an_bypass_p3(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p3, (val))
#define msg_rx_lik_sts_2_VAL_rg_txfc_an_bypass_p2(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p2, (val))
#define msg_rx_lik_sts_2_VAL_rg_txfc_an_bypass_p1(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p1, (val))
#define msg_rx_lik_sts_2_VAL_rg_txfc_an_bypass_p0(val)         REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_txfc_an_bypass_p0, (val))
#define msg_rx_lik_sts_2_VAL_rg_dpx_an_bypass_p3(val)          REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p3, (val))
#define msg_rx_lik_sts_2_VAL_rg_dpx_an_bypass_p2(val)          REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p2, (val))
#define msg_rx_lik_sts_2_VAL_rg_dpx_an_bypass_p1(val)          REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p1, (val))
#define msg_rx_lik_sts_2_VAL_rg_dpx_an_bypass_p0(val)          REG_FLD_VAL(msg_rx_lik_sts_2_FLD_rg_dpx_an_bypass_p0, (val))

#define PHY_RX_FORCE_CTRL_0_VAL_rg_force_rxer_sel(val)         REG_FLD_VAL(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxer_sel, (val))
#define PHY_RX_FORCE_CTRL_0_VAL_rg_force_rxdv_sel(val)         REG_FLD_VAL(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxdv_sel, (val))
#define PHY_RX_FORCE_CTRL_0_VAL_rg_force_rxc_sel(val)          REG_FLD_VAL(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxc_sel, (val))
#define PHY_RX_FORCE_CTRL_0_VAL_rg_force_rxd_sel(val)          REG_FLD_VAL(PHY_RX_FORCE_CTRL_0_FLD_rg_force_rxd_sel, (val))

#define PHY_RX_FORCE_CTRL_1_VAL_rg_force_rxd_h(val)            REG_FLD_VAL(PHY_RX_FORCE_CTRL_1_FLD_rg_force_rxd_h, (val))

#define PHY_RX_FORCE_CTRL_2_VAL_rg_force_rxd_l(val)            REG_FLD_VAL(PHY_RX_FORCE_CTRL_2_FLD_rg_force_rxd_l, (val))

#define PHY_RX_FORCE_CTRL_3_VAL_rg_force_rxer(val)             REG_FLD_VAL(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxer, (val))
#define PHY_RX_FORCE_CTRL_3_VAL_rg_force_rxdv(val)             REG_FLD_VAL(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxdv, (val))
#define PHY_RX_FORCE_CTRL_3_VAL_rg_force_rxc(val)              REG_FLD_VAL(PHY_RX_FORCE_CTRL_3_FLD_rg_force_rxc, (val))

#define Debug_REGISTER_0_VAL_ro_dump_000(val)                  REG_FLD_VAL(Debug_REGISTER_0_FLD_ro_dump_000, (val))

#define Debug_REGISTER_1_VAL_ro_dump_001(val)                  REG_FLD_VAL(Debug_REGISTER_1_FLD_ro_dump_001, (val))

#define Debug_REGISTER_2_VAL_ro_dump_002(val)                  REG_FLD_VAL(Debug_REGISTER_2_FLD_ro_dump_002, (val))

#define Debug_REGISTER_3_VAL_ro_dump_003(val)                  REG_FLD_VAL(Debug_REGISTER_3_FLD_ro_dump_003, (val))

#define Debug_REGISTER_4_VAL_ro_dump_004(val)                  REG_FLD_VAL(Debug_REGISTER_4_FLD_ro_dump_004, (val))

#define Debug_REGISTER_5_VAL_ro_dump_005(val)                  REG_FLD_VAL(Debug_REGISTER_5_FLD_ro_dump_005, (val))

#define Debug_REGISTER_6_VAL_ro_dump_006(val)                  REG_FLD_VAL(Debug_REGISTER_6_FLD_ro_dump_006, (val))

#define Debug_REGISTER_7_VAL_ro_dump_007(val)                  REG_FLD_VAL(Debug_REGISTER_7_FLD_ro_dump_007, (val))

#define Debug_REGISTER_8_VAL_ro_dump_008(val)                  REG_FLD_VAL(Debug_REGISTER_8_FLD_ro_dump_008, (val))

#define Debug_REGISTER_9_VAL_ro_dump_009(val)                  REG_FLD_VAL(Debug_REGISTER_9_FLD_ro_dump_009, (val))

#define Debug_REGISTER_10_VAL_ro_dump_010(val)                 REG_FLD_VAL(Debug_REGISTER_10_FLD_ro_dump_010, (val))

#define Debug_REGISTER_11_VAL_ro_dump_011(val)                 REG_FLD_VAL(Debug_REGISTER_11_FLD_ro_dump_011, (val))

#define Debug_REGISTER_12_VAL_ro_dump_012(val)                 REG_FLD_VAL(Debug_REGISTER_12_FLD_ro_dump_012, (val))

#define Debug_REGISTER_13_VAL_ro_dump_013(val)                 REG_FLD_VAL(Debug_REGISTER_13_FLD_ro_dump_013, (val))

#define Debug_REGISTER_14_VAL_ro_dump_014(val)                 REG_FLD_VAL(Debug_REGISTER_14_FLD_ro_dump_014, (val))

#define Debug_REGISTER_15_VAL_ro_dump_015(val)                 REG_FLD_VAL(Debug_REGISTER_15_FLD_ro_dump_015, (val))

#define Debug_REGISTER_16_VAL_ro_dump_016(val)                 REG_FLD_VAL(Debug_REGISTER_16_FLD_ro_dump_016, (val))

#define Debug_REGISTER_17_VAL_ro_dump_017(val)                 REG_FLD_VAL(Debug_REGISTER_17_FLD_ro_dump_017, (val))

#define Debug_REGISTER_18_VAL_ro_dump_018(val)                 REG_FLD_VAL(Debug_REGISTER_18_FLD_ro_dump_018, (val))

#define Debug_REGISTER_19_VAL_ro_dump_019(val)                 REG_FLD_VAL(Debug_REGISTER_19_FLD_ro_dump_019, (val))

#define Debug_REGISTER_20_VAL_ro_dump_020(val)                 REG_FLD_VAL(Debug_REGISTER_20_FLD_ro_dump_020, (val))

#define Debug_REGISTER_21_VAL_ro_dump_021(val)                 REG_FLD_VAL(Debug_REGISTER_21_FLD_ro_dump_021, (val))

#define Debug_REGISTER_22_VAL_ro_dump_022(val)                 REG_FLD_VAL(Debug_REGISTER_22_FLD_ro_dump_022, (val))

#define Debug_REGISTER_23_VAL_ro_dump_023(val)                 REG_FLD_VAL(Debug_REGISTER_23_FLD_ro_dump_023, (val))

#define Debug_REGISTER_24_VAL_ro_dump_024(val)                 REG_FLD_VAL(Debug_REGISTER_24_FLD_ro_dump_024, (val))

#define Debug_REGISTER_25_VAL_ro_dump_025(val)                 REG_FLD_VAL(Debug_REGISTER_25_FLD_ro_dump_025, (val))

#define Debug_REGISTER_26_VAL_ro_dump_026(val)                 REG_FLD_VAL(Debug_REGISTER_26_FLD_ro_dump_026, (val))

#define Debug_REGISTER_27_VAL_ro_dump_027(val)                 REG_FLD_VAL(Debug_REGISTER_27_FLD_ro_dump_027, (val))

#define Debug_REGISTER_28_VAL_ro_dump_028(val)                 REG_FLD_VAL(Debug_REGISTER_28_FLD_ro_dump_028, (val))

#define Debug_REGISTER_29_VAL_ro_dump_029(val)                 REG_FLD_VAL(Debug_REGISTER_29_FLD_ro_dump_029, (val))

#define Debug_REGISTER_30_VAL_ro_dump_030(val)                 REG_FLD_VAL(Debug_REGISTER_30_FLD_ro_dump_030, (val))

#define Debug_REGISTER_31_VAL_ro_dump_031(val)                 REG_FLD_VAL(Debug_REGISTER_31_FLD_ro_dump_031, (val))

#define Debug_REGISTER_CTRL_0_VAL_rg_force_dump(val)           REG_FLD_VAL(Debug_REGISTER_CTRL_0_FLD_rg_force_dump, (val))
#define Debug_REGISTER_CTRL_0_VAL_rg_dump_sel(val)             REG_FLD_VAL(Debug_REGISTER_CTRL_0_FLD_rg_dump_sel, (val))
#define Debug_REGISTER_CTRL_0_VAL_rg_dump_inv(val)             REG_FLD_VAL(Debug_REGISTER_CTRL_0_FLD_rg_dump_inv, (val))
#define Debug_REGISTER_CTRL_0_VAL_rg_dump_clr(val)             REG_FLD_VAL(Debug_REGISTER_CTRL_0_FLD_rg_dump_clr, (val))

#define Debug_REGISTER_DUMP_CNT_VAL_ro_dump_marker(val)        REG_FLD_VAL(Debug_REGISTER_DUMP_CNT_FLD_ro_dump_marker, (val))

#ifdef __cplusplus
}
#endif

#endif // __multi_sgmii_csr_REGS_H__

