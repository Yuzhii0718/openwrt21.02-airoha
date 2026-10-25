#ifndef __pon_hsgmii_dig_csr_REGS_H__
#define __pon_hsgmii_dig_csr_REGS_H__

//#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef REG_BASE_C_MODULE
// ----------------- pon_hsgmii_dig_csr Bit Field Definitions -------------------

#define PACKING
typedef unsigned int FIELD;
typedef unsigned int UINT32; 

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_force_on         : 1;
        FIELD rsv_1                     : 7;
        FIELD rg_lcpll_hw_ctrl_mode     : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_en_inv           : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_sw_lcpll_en            : 1;
        FIELD rg_tdc_lck2ref_sel        : 1;
        FIELD rsv_26                    : 2;
        FIELD rg_tdc_ck_en_and_los      : 1;
        FIELD rg_tdc_los_inv            : 1;
        FIELD rg_int_los_inv            : 1;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_0, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_man_pwdb         : 1;
        FIELD rsv_1                     : 7;
        FIELD rg_lcpll_en_timer         : 8;
        FIELD rg_lcpll_pcw_man_load_timer : 8;
        FIELD rg_lcpll_ck_stb_timer     : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_1, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tdc_ck_stb_timer       : 8;
        FIELD rg_tdc_mst_en             : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_ncpo_ana_msb           : 2;
        FIELD rsv_18                    : 14;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_2, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_ledck_div        : 4;
        FIELD rsv_4                     : 28;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_DBG_SETTING, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_DBG_SETTING;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_tdc_dig_pwdb     : 1;
        FIELD rsv_1                     : 7;
        FIELD rg_lcpll_dpma_autopw_en   : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_dpma_dds1_pwdb   : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_dpma_dds1_iso_disable : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_0, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_dpma_dds1_iso_disable_dly : 2;
        FIELD rsv_2                     : 6;
        FIELD rg_lcpll_dpma_dds1_rstb_dly : 2;
        FIELD rsv_10                    : 6;
        FIELD rg_lcpll_dpma_dds1_rstb   : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_dpma_dds1_pcw_ncpo_chg : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_1, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly : 2;
        FIELD rsv_2                     : 6;
        FIELD rg_lcpll_dpma_dds1_ncpo_en : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_dpma_dds1_ncpo_en_dly : 2;
        FIELD rsv_18                    : 6;
        FIELD rg_lcpll_dpma_txpll1_ddsfbk_en : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_2, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_dpma_txpll1_ddsfbk_en_dly : 2;
        FIELD rsv_2                     : 6;
        FIELD rg_lcpll_tdc_cal_trig     : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_tdc_cal_trig_dly : 3;
        FIELD rsv_19                    : 5;
        FIELD rg_lcpll_tdc_cal          : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_3, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_tdc_cal_bw       : 1;
        FIELD rsv_1                     : 7;
        FIELD rg_lcpll_tdc_cal_offset   : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_tdc_en           : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_tdc_sigdet_cg    : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_4, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_tdc_en_mux       : 1;
        FIELD rsv_1                     : 7;
        FIELD rg_lcpll_tdc_autoen       : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_autok_tdc        : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_tdc_sync_in_mode : 1;
        FIELD rg_ncpo_start_dly_en      : 1;
        FIELD rg_tdc_start_cnt_clr      : 1;
        FIELD rsv_27                    : 1;
        FIELD rg_tdc_start_cnt          : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_5, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_pon_rx_cdr_divtdc : 3;
        FIELD rsv_3                     : 5;
        FIELD rg_lcpll_ki               : 3;
        FIELD rsv_11                    : 5;
        FIELD rg_lcpll_tdc_offset       : 11;
        FIELD rsv_27                    : 5;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_0, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_gpon_sel         : 1;
        FIELD rsv_1                     : 7;
        FIELD rg_lcpll_a_tdc            : 4;
        FIELD rsv_12                    : 4;
        FIELD rg_lcpll_tdc_gain         : 11;
        FIELD rsv_27                    : 5;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_1, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_ncpo_value       : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_ncpo_shift       : 2;
        FIELD rsv_2                     : 6;
        FIELD rg_lcpll_ncpo_load        : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_ncpo_hold        : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_tdc_swrst        : 1;
        FIELD rsv_25                    : 3;
        FIELD rg_sdm_upd                : 1;
        FIELD rg_sdm_upd_disb           : 1;
        FIELD rsv_30                    : 2;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_tdcen_rst_dlf    : 1;
        FIELD rsv_1                     : 7;
        FIELD rg_lcpll_tdc_rst          : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_lcpll_tdcen_chk_lck2ref : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_tdcen_wait_lck2ref : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_4, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_tdc_waitlck_pre_dly : 2;
        FIELD rsv_2                     : 6;
        FIELD rg_lcpll_tdc_waitlck_pos_dly : 3;
        FIELD rsv_11                    : 5;
        FIELD rg_lcpll_tdc_autopw_ncpo  : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_ncpo_chg         : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_ncpo_chg_mask    : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_lcpll_ncpo_chg_delay   : 2;
        FIELD rsv_10                    : 6;
        FIELD rg_lcpll_dlf_mult_en      : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_lcpll_tdc_pcw_sub1en   : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_6, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_6;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_tdcck_inv_en     : 1;
        FIELD rsv_1                     : 31;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_7, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_7;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_pon_hrdds_pcw_ncpo_gpon : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_1, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_pon_hrdds_pcw_ncpo_epon : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_2, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_lcpll_tdc_tdcpw_st     : 4;
        FIELD rsv_4                     : 4;
        FIELD ro_lcpll_tdc_st           : 5;
        FIELD rsv_13                    : 3;
        FIELD ro_lcpll_tdc_dig          : 12;
        FIELD rsv_28                    : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_1, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_lcpll_tdc_dlf_gain     : 20;
        FIELD ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg : 1;
        FIELD ro_lcpll_tdc_dpma_dds1_ncpo_en : 1;
        FIELD ro_lcpll_tdc_dpma_dds1_rstb : 1;
        FIELD ro_lcpll_tdc_dpma_dds1_iso_disable : 1;
        FIELD ro_lcpll_tdc_dpma_dds1_pwdb : 1;
        FIELD ro_lcpll_tdc_en_lck2ref_rst : 1;
        FIELD ro_lcpll_tdc_dpma_rx_ck_stb : 1;
        FIELD ro_lcpll_tdc_dpma_tdc_en  : 1;
        FIELD ro_lcpll_tdc_dpma_tdc_cal_bw : 1;
        FIELD ro_lcpll_tdc_dpma_tdc_cal_offset : 1;
        FIELD ro_lcpll_tdc_dpma_txpll1_ddsfbk_en : 1;
        FIELD ro_lcpll_tdc_dpma_tdc_ck_stb : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_2, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_lcpll_tdc_offset       : 11;
        FIELD ro_lcpll_tdc_cal_done     : 1;
        FIELD ro_lcpll_tdc_en           : 1;
        FIELD ro_lcpll_tdc_decode       : 3;
        FIELD ro_lcpll_tdc_gain         : 11;
        FIELD rsv_27                    : 5;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_3, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_lcpll_tdc_dpma_dds1_pcw_ncpo : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_4, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_lcpll_tdc_dpma_tdc_leadlag : 1;
        FIELD rsv_1                     : 7;
        FIELD ro_lcpll_tdc_dpma_tdc_d   : 8;
        FIELD ro_lcpll_tdc_dpma_tdc_c   : 8;
        FIELD rsv_24                    : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_5, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lcpll_ki_hold          : 1;
        FIELD rsv_1                     : 31;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_CTRL_0, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD eee_lcpll_hold            : 1;
        FIELD rsv_1                     : 7;
        FIELD tdc_sync_pcw_ncpo_chg     : 1;
        FIELD rsv_9                     : 23;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_0, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD tdc_sync_pcw_ncpo         : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_1, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD flagl_sel                 : 8;
        FIELD flagh_sel                 : 8;
        FIELD lcpll_top_flagl_en        : 1;
        FIELD rsv_17                    : 7;
        FIELD lcpll_top_flagh_en        : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_SS_LCPLL_2, *Ppon_hsgmii_dig_csr_REG_SS_LCPLL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_dlf_gain_new           : 8;
        FIELD ro_dlf_gain_rund_new      : 8;
        FIELD rsv_16                    : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_DLF_GAIN_STATUS, *Ppon_hsgmii_dig_csr_REG_TDC_DLF_GAIN_STATUS;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_dlf_gain_new_org       : 20;
        FIELD rsv_20                    : 4;
        FIELD rg_dlf_mode_sel           : 1;
        FIELD rsv_25                    : 6;
        FIELD rg_dlf_gain_latch         : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_DLF_MODE_SETTING, *Ppon_hsgmii_dig_csr_REG_TDC_DLF_MODE_SETTING;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tdc_ncpo_hold_release_time : 16;
        FIELD rg_tdc_los_hold_mux       : 1;
        FIELD rsv_17                    : 3;
        FIELD rg_ncpo_toggle            : 1;
        FIELD rsv_21                    : 3;
        FIELD rg_ncpo_hold_en           : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_0, *Ppon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_ncpo_up_thred          : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_1, *Ppon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_ncpo_dwn_thred         : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_2, *Ppon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_ncpo_output            : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_0, *Ppon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_ncpo_max_output        : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_1, *Ppon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_ncpo_min_output        : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_2, *Ppon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_phya_auto_init         : 1;
        FIELD rg_speed_chg              : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_tphy_speed_mode        : 2;
        FIELD rg_tphy_speed_mode_force  : 1;
        FIELD rg_sgmii_auto_init        : 1;
        FIELD rg_tphy_mode_set          : 2;
        FIELD rsv_10                    : 6;
        FIELD rg_force_tphy_mode        : 1;
        FIELD rg_force_tphy_speed_rx_sel : 1;
        FIELD rg_pcie_device_mode       : 1;
        FIELD rg_force_tphy_speed_tx_sel : 1;
        FIELD rg_force_tphy_speed_tx    : 2;
        FIELD rsv_22                    : 2;
        FIELD rg_ben_inv                : 1;
        FIELD rsv_25                    : 3;
        FIELD rg_tx_data_path_sel       : 1;
        FIELD rsv_29                    : 3;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_cdr_re_calib           : 1;
        FIELD rg_freq_det_frc           : 1;
        FIELD rg_freq_det_frc_en        : 1;
        FIELD rsv_3                     : 1;
        FIELD rg_phya_pwd_mux           : 1;
        FIELD rg_phya_pwd               : 1;
        FIELD rg_tx_disable_mux         : 1;
        FIELD rg_tx_disable             : 1;
        FIELD rg_rx_pwd_mux             : 1;
        FIELD rg_rx_pwd                 : 1;
        FIELD rsv_10                    : 22;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_1, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_sigdet_wait_cond       : 1;
        FIELD rg_sigdet_wait_force      : 1;
        FIELD rg_tx_idle                : 1;
        FIELD rg_tx_idle_mode           : 1;
        FIELD rg_tx_idle_mux            : 1;
        FIELD rg_tx_idle_en             : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_tx_div_mode            : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_bypass_int_dly         : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_epon_benoff_opt        : 1;
        FIELD rsv_17                    : 7;
        FIELD rg_ben_on_opt             : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_2, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bias_pwd_to_bg_lpf_en_dly : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_cdr_bw_sel_dly         : 4;
        FIELD rsv_12                    : 4;
        FIELD rg_cdr_settle_dly         : 11;
        FIELD rsv_27                    : 5;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_3, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_freq_det_en_wait_dly   : 4;
        FIELD rg_lpf_window_sel         : 2;
        FIELD rsv_6                     : 2;
        FIELD rg_ppm_thres              : 6;
        FIELD rsv_14                    : 2;
        FIELD rg_freq_meter_sel         : 1;
        FIELD rsv_17                    : 15;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_4, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_eq_en_h_dly         : 13;
        FIELD rsv_13                    : 3;
        FIELD rg_sig_det_wait_dly       : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_5, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_pll_autok_vco_to_pll_bias_lpf_en_dly : 2;
        FIELD rsv_2                     : 6;
        FIELD rg_rx_pi_cal_en_h_dly     : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_rx_saosc_en_h_dly      : 6;
        FIELD rsv_22                    : 10;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_6, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_6;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_sig_det_h_thres        : 10;
        FIELD rsv_10                    : 6;
        FIELD rg_sig_det_l_thres        : 10;
        FIELD rsv_26                    : 2;
        FIELD rg_syspll_pwd_to_syspll_ready_dly : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_7, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_7;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_eidle_lp_en_ne_to_data_en_pe_dly_g1 : 8;
        FIELD rg_eidle_lp_en_ne_to_data_en_pe_dly_g2 : 8;
        FIELD rg_sigdet_out_sel         : 2;
        FIELD rsv_18                    : 14;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_8, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_8;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_ssusb_pll_band_fail    : 7;
        FIELD rsv_7                     : 1;
        FIELD rg_ssusb_cdr_band_fail    : 7;
        FIELD rsv_15                    : 17;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_9, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_9;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_afifo_rinc_serdes      : 1;
        FIELD rg_afifo_winc_serdes      : 1;
        FIELD rg_pcs_t2r_loopback       : 1;
        FIELD rg_qphy_r2t_loopback      : 1;
        FIELD rg_qphy_r2t_loopback_fifo_rst : 1;
        FIELD rg_rx_bit_polarity        : 1;
        FIELD rg_tbi_rx_disable         : 1;
        FIELD rg_tbi_tx_disable         : 1;
        FIELD rg_tx_bit_polarity        : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_prbs_check             : 1;
        FIELD rg_prbs_en                : 1;
        FIELD rg_prbs_injerr            : 1;
        FIELD rg_renew_mode             : 1;
        FIELD rg_prbs_mode              : 2;
        FIELD rsv_22                    : 2;
        FIELD ro_bist_err               : 1;
        FIELD ro_bist_ok                : 1;
        FIELD ro_bist_run               : 1;
        FIELD rsv_27                    : 5;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_bist_err_cnt           : 16;
        FIELD ro_debug_prbs             : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_STS_0, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_STS_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_udp_data_31_0          : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_11, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_11;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_udp_data_63_32         : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_12, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_12;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_udp_data_79_64         : 16;
        FIELD rsv_16                    : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_13, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_13;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_afifo_r_thres_serdes   : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_afifo_w_thres_serdes   : 5;
        FIELD rsv_13                    : 19;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_14, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_14;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_debug_afifo_serdes     : 18;
        FIELD rsv_18                    : 14;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_STS_1, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_STS_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_prbs_path_sel       : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_rx_prbs7_path_en       : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_rx_prbs23_path_en      : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_rx_prbs31_path_en      : 1;
        FIELD rg_rx_prbs9_path_en       : 1;
        FIELD rg_rx_prbs15_path_en      : 1;
        FIELD rsv_15                    : 1;
        FIELD rg_rx_cid_path_en         : 1;
        FIELD rsv_17                    : 3;
        FIELD rg_data_rx_inv            : 1;
        FIELD rsv_21                    : 3;
        FIELD rg_rx_path_rdy_mux        : 1;
        FIELD rsv_25                    : 3;
        FIELD rg_ben_force_mode         : 1;
        FIELD rg_ben_force_value        : 1;
        FIELD rg_ad_ben_out_clr         : 1;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_15, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_15;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tx_ben_start_dly_mode  : 1;
        FIELD rg_tx_ben_end_dly_mode    : 1;
        FIELD rg_bit_delay_rg_mode      : 1;
        FIELD rsv_3                     : 1;
        FIELD rg_txbit_dly              : 4;
        FIELD rg_tx_bit_dly_mode        : 1;
        FIELD rg_tx_start_dly_sel       : 1;
        FIELD rg_tx_end_dly_sel         : 1;
        FIELD rsv_11                    : 21;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_16, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_16;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tx_bit_start_dly       : 8;
        FIELD rg_tx_bit_end_dly         : 8;
        FIELD rsv_16                    : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_17, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_17;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_l2ref_ne_cnt           : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_force_l2ref_done       : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_force_l2ref_done_sel   : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_rxcal_ne_cnt           : 5;
        FIELD rsv_21                    : 3;
        FIELD rg_force_rxcal_done       : 1;
        FIELD rg_force_rxcal_done_sel   : 1;
        FIELD rsv_26                    : 6;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_18, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_18;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_l2ref_done             : 1;
        FIELD ro_rxcal_done             : 1;
        FIELD rsv_2                     : 30;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_STS_2, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_STS_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_20b_idle_code          : 20;
        FIELD rsv_20                    : 12;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_19, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_19;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_da_pll_en_mux          : 1;
        FIELD rg_force_da_pll_en        : 1;
        FIELD rg_da_pll_en              : 1;
        FIELD rsv_3                     : 5;
        FIELD rg_da_pll_en_dly_cnt      : 8;
        FIELD rg_lpf_en_to_ssc_dly_cnt  : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_20, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_20;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_da_cdr_pwd_dly_cnt     : 8;
        FIELD rg_force_da_cdr_pwd       : 1;
        FIELD rg_da_cdr_pwd             : 1;
        FIELD rsv_10                    : 6;
        FIELD rg_force_pll_vco_cplt     : 1;
        FIELD rg_pll_vco_cplt           : 1;
        FIELD rsv_18                    : 14;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_21, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_21;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_vcocal_state           : 8;
        FIELD rg_vcocal_dly_cnt         : 4;
        FIELD rg_vcocal_fail            : 1;
        FIELD rsv_13                    : 19;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_22, *Ppon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_22;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rsv_0                     : 1;
        FIELD rg_force_map_sel          : 1;
        FIELD rg_force_map_mode         : 1;
        FIELD rg_force_ge_mode          : 1;
        FIELD rg_pon_tx250m_ck_en       : 1;
        FIELD rg_pon_rx500m_ck_en       : 1;
        FIELD rg_hg_tx250m_ck_en        : 1;
        FIELD rg_hg_rx500m_ck_en        : 1;
        FIELD rg_baser_txck_en          : 1;
        FIELD rg_baser_rxck_en          : 1;
        FIELD rsv_10                    : 2;
        FIELD rg_tx_short_ck_inv        : 1;
        FIELD rg_rx_short_ck_inv        : 1;
        FIELD rsv_14                    : 2;
        FIELD rg_txben_clk_inv          : 1;
        FIELD rg_rx_clk_inv             : 1;
        FIELD rg_tx_clk_inv             : 1;
        FIELD rg_baser_tx_clk_inv       : 1;
        FIELD rg_baser_rx_clk_inv       : 1;
        FIELD rsv_21                    : 3;
        FIELD rg_pon_rx_pma_clk_div_sel : 1;
        FIELD rg_hg_rx_pma_clk_div_sel  : 1;
        FIELD rg_rx_pma_clk_sel         : 1;
        FIELD rg_pon_tx_pma_clk_div_sel : 1;
        FIELD rg_hg_tx_pma_clk_div_sel  : 1;
        FIELD rg_tx_pma_clk_sel         : 1;
        FIELD rg_baser_tx_pma_clk_div_sel : 1;
        FIELD rg_xfi_rx_pma_clk_sel     : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0, *Ppon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_mactxrx_reset_n        : 1;
        FIELD rg_macrx_reset_n          : 1;
        FIELD rg_mactx_reset_n          : 1;
        FIELD rg_pcstxrx_reset_n        : 1;
        FIELD rg_pcsrx_reset_n          : 1;
        FIELD rg_pcstx_reset_n          : 1;
        FIELD rg_ref_reset_n            : 1;
        FIELD rg_rx_reset_n             : 1;
        FIELD rg_tx_reset_n             : 1;
        FIELD rg_tx250m_div_reset_n     : 1;
        FIELD rg_tx250m_short_reset_n   : 1;
        FIELD rg_eq_rx250m_reset_n      : 1;
        FIELD rg_eq_rx250m_div_reset_n  : 1;
        FIELD rg_bridge_tx_reset_n      : 1;
        FIELD rg_bridge_rx_reset_n      : 1;
        FIELD rg_trans_rx_data_inv      : 1;
        FIELD rg_force_bg_tx_mode       : 1;
        FIELD rg_force_bg_tx_value      : 1;
        FIELD rsv_18                    : 2;
        FIELD rg_rx_fifo_rst            : 1;
        FIELD rsv_21                    : 3;
        FIELD rg_baser_tx_reset_n       : 1;
        FIELD rg_baser_rx_reset_n       : 1;
        FIELD rg_all_tx_rst_n           : 1;
        FIELD rg_all_rx_rst_n           : 1;
        FIELD rg_rate_adp_tx_fifo_rst_n : 1;
        FIELD rg_rate_adp_rx_fifo_rst_n : 1;
        FIELD rg_rate_adp_rx_pma_fifo_rst_n : 1;
        FIELD rg_div_rst                : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_1, *Ppon_hsgmii_dig_csr_REG_PON_SYS_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_pon_probe_sel          : 2;
        FIELD rsv_2                     : 2;
        FIELD rg_xtal_is_25m            : 1;
        FIELD rg_xtal_mode              : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_pon_mbist_mode_en      : 1;
        FIELD rg_pon_mbist_slow_mode_en : 1;
        FIELD rsv_10                    : 6;
        FIELD rg_1us_25m_cnt            : 8;
        FIELD rg_1us_40m_cnt            : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_2, *Ppon_hsgmii_dig_csr_REG_PON_SYS_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_baser_txck_mode        : 2;
        FIELD rg_baser_rxck_mode        : 2;
        FIELD rsv_4                     : 4;
        FIELD rg_rs_ck_mux              : 1;
        FIELD rg_rs_ck_sel              : 1;
        FIELD rg_rs_ck_cnt              : 1;
        FIELD rsv_11                    : 21;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_CK_CTRL_0, *Ppon_hsgmii_dig_csr_REG_PON_CK_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_hg_pma_tx_reset_n      : 1;
        FIELD rg_hg_pma_rx_reset_n      : 1;
        FIELD rg_hg_pcs_tx_reset_n      : 1;
        FIELD rg_hg_pcs_rx_reset_n      : 1;
        FIELD rg_hg_mac_tx_reset_n      : 1;
        FIELD rg_hg_mac_rx_reset_n      : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_bypass_tx_re_en        : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_xfi_pcs_rx_gearbox_fifo_rst_n : 1;
        FIELD rg_xfi_pcs_tx_gearbox_fifo_rst_n : 1;
        FIELD rg_xfi_pcs_bg_lb_fifo_rst_n : 1;
        FIELD rsv_15                    : 1;
        FIELD rg_hsgmii_mac_txck_free_sel : 1;
        FIELD rg_hsgmii_mac_rxck_free_sel : 1;
        FIELD rsv_18                    : 14;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0, *Ppon_hsgmii_dig_csr_REG_HG_RST_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_serdes_mode            : 1;
        FIELD rg_gpon_sym_mode          : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_sgmii_mode             : 2;
        FIELD rsv_6                     : 2;
        FIELD rg_ip_reset_mode          : 1;
        FIELD rsv_9                     : 7;
        FIELD rg_fpga_mode_ctrl         : 5;
        FIELD rsv_21                    : 3;
        FIELD rg_fpga_tx_swap           : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0, *Ppon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_dac_pwd_mux         : 1;
        FIELD rg_ln0_rx_dac_pwd         : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_tx_dem                 : 2;
        FIELD rsv_6                     : 2;
        FIELD rg_eq_step1               : 2;
        FIELD rsv_10                    : 22;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_HG_RG_CTRL_0, *Ppon_hsgmii_dig_csr_REG_HG_RG_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_gpon_mode              : 1;
        FIELD ro_gpon_tmode             : 1;
        FIELD ro_epon_mode              : 1;
        FIELD ro_sgmii_mode             : 1;
        FIELD ro_hsgmii_mode            : 1;
        FIELD ro_baser_mode             : 1;
        FIELD rsv_6                     : 26;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_STS_0, *Ppon_hsgmii_dig_csr_REG_PON_STS_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tx_osr_sel             : 2;
        FIELD rsv_2                     : 2;
        FIELD rg_rx_osr_sel             : 1;
        FIELD rsv_5                     : 11;
        FIELD rg_tx_eqd_mode            : 1;
        FIELD rsv_17                    : 15;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_OSR_SEL_CTRL, *Ppon_hsgmii_dig_csr_REG_PON_OSR_SEL_CTRL;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bir_ltr_mode_0         : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_bir_ltr_mode_1         : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_bic_ltr_mode_0         : 4;
        FIELD rg_bic_ltr_mode_1         : 4;
        FIELD rsv_24                    : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_CDR_CTRL_0, *Ppon_hsgmii_dig_csr_REG_CDR_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bc_ltr_mode_0          : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_bc_ltr_mode_1          : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_br_ltr_mode_0          : 3;
        FIELD rsv_19                    : 1;
        FIELD rg_br_ltr_mode_1          : 3;
        FIELD rsv_23                    : 1;
        FIELD rg_kvsel_ltr_mode_0       : 1;
        FIELD rg_kvsel_ltr_mode_1       : 1;
        FIELD rsv_26                    : 6;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_CDR_CTRL_1, *Ppon_hsgmii_dig_csr_REG_CDR_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bir_ltd0_mode_0        : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_bir_ltd0_mode_1        : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_bic_ltd0_mode_0        : 4;
        FIELD rg_bic_ltd0_mode_1        : 4;
        FIELD rsv_24                    : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_CDR_CTRL_2, *Ppon_hsgmii_dig_csr_REG_CDR_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bc_ltd0_mode_0         : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_bc_ltd0_mode_1         : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_br_ltd0_mode_0         : 3;
        FIELD rsv_19                    : 1;
        FIELD rg_br_ltd0_mode_1         : 3;
        FIELD rsv_23                    : 1;
        FIELD rg_kvsel_ltd0_mode_0      : 1;
        FIELD rg_kvsel_ltd0_mode_1      : 1;
        FIELD rsv_26                    : 6;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_CDR_CTRL_3, *Ppon_hsgmii_dig_csr_REG_CDR_CTRL_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bir_ltd1_mode_0        : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_bir_ltd1_mode_1        : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_bic_ltd1_mode_0        : 4;
        FIELD rg_bic_ltd1_mode_1        : 4;
        FIELD rsv_24                    : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_CDR_CTRL_4, *Ppon_hsgmii_dig_csr_REG_CDR_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bc_ltd1_mode_0         : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_bc_ltd1_mode_1         : 5;
        FIELD rsv_13                    : 3;
        FIELD rg_br_ltd1_mode_0         : 3;
        FIELD rsv_19                    : 1;
        FIELD rg_br_ltd1_mode_1         : 3;
        FIELD rsv_23                    : 1;
        FIELD rg_kvsel_ltd1_mode_0      : 1;
        FIELD rg_kvsel_ltd1_mode_1      : 1;
        FIELD rsv_26                    : 6;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_CDR_CTRL_5, *Ppon_hsgmii_dig_csr_REG_CDR_CTRL_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tx_dly_data_ftune      : 7;
        FIELD rsv_7                     : 1;
        FIELD rg_tx_dly_ben_ftune       : 7;
        FIELD rsv_15                    : 1;
        FIELD rg_tx_ben_exten_ftune     : 8;
        FIELD rg_tx_ben_start_dly_sel   : 3;
        FIELD rsv_27                    : 1;
        FIELD rg_tx_ben_end_dly_sel     : 3;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_0, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_force_tx_epon_mode     : 1;
        FIELD rg_force_tx_epon_sel      : 1;
        FIELD rg_force_tx_gpon_mode     : 1;
        FIELD rg_force_tx_gpon_sel      : 1;
        FIELD rg_force_rx_pon_mode      : 1;
        FIELD rg_force_rx_pon_sel       : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_tx_non_reverse_gpon    : 1;
        FIELD rg_rx_non_reverse_gpon    : 1;
        FIELD rg_tx_non_reverse_epon    : 1;
        FIELD rg_rx_non_reverse_epon    : 1;
        FIELD rsv_12                    : 20;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_1, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_outben_data_mode       : 3;
        FIELD rsv_3                     : 1;
        FIELD rg_hsdata_en_sel          : 3;
        FIELD rsv_7                     : 1;
        FIELD rg_txben_pre_post_zero_en : 1;
        FIELD rsv_9                     : 6;
        FIELD rg_benoff_cnt_max         : 3;
        FIELD rsv_18                    : 9;
        FIELD rg_bit_delay_mode_sel     : 1;
        FIELD rg_txdata_bdly_mode       : 1;
        FIELD rg_txben_bdly_mode        : 1;
        FIELD rsv_30                    : 2;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_2, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_bist_prbs_init_seed_benoff : 31;
        FIELD rg_bist_prbs_update       : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_3, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_benoff_d0              : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_4, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_benoff_d1              : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_5, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_benoff_d2              : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_6, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_6;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_benoff_d3              : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_7, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_7;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_pon_tx_frq_ppm_thres   : 6;
        FIELD rsv_6                     : 2;
        FIELD rg_pon_rx_frq_ppm_thres   : 6;
        FIELD rsv_14                    : 2;
        FIELD rg_hg_tx_frq_ppm_thres    : 6;
        FIELD rsv_22                    : 2;
        FIELD rg_hg_rx_frq_ppm_thres    : 6;
        FIELD rsv_30                    : 2;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_FRQ_CTRL_0, *Ppon_hsgmii_dig_csr_REG_FRQ_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_baser_tx_frq_ppm_thres : 6;
        FIELD rsv_6                     : 2;
        FIELD rg_baser_rx_frq_ppm_thres : 6;
        FIELD rsv_14                    : 18;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_FRQ_CTRL_1, *Ppon_hsgmii_dig_csr_REG_FRQ_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_pon_tx_freq_det_en     : 1;
        FIELD rg_pon_rx_freq_det_en     : 1;
        FIELD rg_hg_tx_freq_det_en      : 1;
        FIELD rg_hg_rx_freq_det_en      : 1;
        FIELD rg_baser_tx_freq_det_en   : 1;
        FIELD rg_baser_rx_freq_det_en   : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_pon_tx_freq_det_mux    : 1;
        FIELD rg_pon_rx_freq_det_mux    : 1;
        FIELD rg_hg_tx_freq_det_mux     : 1;
        FIELD rg_hg_rx_freq_det_mux     : 1;
        FIELD rg_baser_tx_freq_det_mux  : 1;
        FIELD rg_baser_rx_freq_det_mux  : 1;
        FIELD rsv_14                    : 18;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_FRQ_CTRL_2, *Ppon_hsgmii_dig_csr_REG_FRQ_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_pon_tx_freq_cnt        : 12;
        FIELD rsv_12                    : 4;
        FIELD rg_pon_rx_freq_cnt        : 12;
        FIELD rsv_28                    : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_FRQ_CTRL_3, *Ppon_hsgmii_dig_csr_REG_FRQ_CTRL_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_hg_tx_freq_cnt         : 12;
        FIELD rsv_12                    : 4;
        FIELD rg_hg_rx_freq_cnt         : 12;
        FIELD rsv_28                    : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_FRQ_CTRL_4, *Ppon_hsgmii_dig_csr_REG_FRQ_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_baser_tx_freq_cnt      : 12;
        FIELD rsv_12                    : 4;
        FIELD rg_baser_rx_freq_cnt      : 12;
        FIELD rsv_28                    : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_FRQ_CTRL_5, *Ppon_hsgmii_dig_csr_REG_FRQ_CTRL_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_pll_lock_cyclecnt   : 16;
        FIELD rg_rx_pll_unlock_cyclecnt : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_0, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_pll_lock_target_beg : 16;
        FIELD rg_rx_pll_lock_target_end : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_1, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_pll_unlock_target_beg : 16;
        FIELD rg_rx_pll_unlock_target_end : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_2, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_pll_freqlock_det_en : 3;
        FIELD rsv_3                     : 1;
        FIELD rg_rx_pll_lock_cnt_clear  : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_rx_pll_lock_lockth     : 4;
        FIELD rg_rx_pll_unlockth        : 4;
        FIELD rg_rx_pll_wait_100us      : 8;
        FIELD rg_force_rx_freq_det_en   : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_3, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_rx_fl_out              : 16;
        FIELD ro_rx_lslock_cnt          : 4;
        FIELD ro_rx_fbck_lock           : 1;
        FIELD ro_rx_fbck_lock_inner     : 1;
        FIELD ro_rx_state_freqdet       : 2;
        FIELD ro_rx_prb_out_freqdet     : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_STS_0, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_STS_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tx_test_pattern        : 20;
        FIELD rsv_20                    : 4;
        FIELD rg_tx_test_mode           : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_8, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_8;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_ncpo_lof_en            : 1;
        FIELD rg_ncpo_los_inv           : 1;
        FIELD rg_ncpo_los_pro_en        : 1;
        FIELD rsv_3                     : 1;
        FIELD rg_los_function_sel       : 2;
        FIELD rsv_6                     : 1;
        FIELD rg_ncpo_los_hold_en       : 1;
        FIELD rg_ncpo_los_cnt           : 8;
        FIELD rsv_16                    : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_TX_CTRL_9, *Ppon_hsgmii_dig_csr_REG_TX_CTRL_9;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_cdr_freq_chk_cycle  : 20;
        FIELD rsv_20                    : 12;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_0, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_cdr_freq_golden_cycle : 20;
        FIELD rsv_20                    : 12;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_1, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_cdr_freq_tolerate_cycle : 20;
        FIELD rsv_20                    : 12;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_2, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_cdr_freq_det_en     : 1;
        FIELD rg_rx_cdr_freq_clear_unlck_cnt : 1;
        FIELD rsv_2                     : 2;
        FIELD rg_rx_cdr_freq_relck_num  : 4;
        FIELD rsv_8                     : 8;
        FIELD ro_rx_cdr_freq_lock       : 1;
        FIELD rsv_17                    : 3;
        FIELD ro_rx_cdr_freq_state      : 4;
        FIELD ro_rx_cdr_freq_unlck_cnt  : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_4, *Ppon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_f_da_pll_ssc_en        : 1;
        FIELD rg_c_da_pll_ssc_en        : 1;
        FIELD rg_f_da_pll_autok_vco     : 1;
        FIELD rg_c_da_pll_autok_vco     : 1;
        FIELD rg_f_da_pll_pwd           : 1;
        FIELD rg_c_da_pll_pwd           : 1;
        FIELD rg_f_da_syspll_sdm_iso_en : 1;
        FIELD rg_c_da_syspll_sdm_iso_en : 1;
        FIELD rg_f_da_syspll_sdm_pwr_on : 1;
        FIELD rg_c_da_syspll_sdm_pwr_on : 1;
        FIELD rg_f_da_syspll_pwd        : 1;
        FIELD rg_c_da_syspll_pwd        : 1;
        FIELD rg_f_da_xtal_rx_pwd       : 1;
        FIELD rg_c_da_xtal_rx_pwd       : 1;
        FIELD rg_f_da_pll_bias_pwd      : 1;
        FIELD rg_c_da_pll_bias_pwd      : 1;
        FIELD rg_f_da_bg_lpf_en         : 1;
        FIELD rg_c_da_bg_lpf_en         : 1;
        FIELD rg_f_da_biasimr_pwd       : 1;
        FIELD rg_c_da_biasimr_pwd       : 1;
        FIELD rg_f_da_bias_pwd          : 1;
        FIELD rg_c_da_bias_pwd          : 1;
        FIELD rg_f_da_pll_bias_lpf_en   : 1;
        FIELD rg_c_da_pll_bias_lpf_en   : 1;
        FIELD rsv_24                    : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_0, *Ppon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_f_da_sigdet_cal_offset : 5;
        FIELD rsv_5                     : 3;
        FIELD rg_c_da_sigdet_cal_offset : 5;
        FIELD rsv_13                    : 19;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_1, *Ppon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_f_da_cdr_lpf_lck2ref   : 1;
        FIELD rg_c_da_cdr_lpf_lck2ref   : 1;
        FIELD rsv_2                     : 30;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_2, *Ppon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_pon_tx_gpon_mux        : 1;
        FIELD rg_pon_tx_gpon_sel        : 1;
        FIELD rsv_2                     : 30;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_3, *Ppon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_rx_sigdet_en           : 1;
        FIELD rg_pma_rx_sigdet_en       : 1;
        FIELD rg_pma_rx_sigdet_pro      : 1;
        FIELD rsv_3                     : 29;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_4, *Ppon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_force_da_pon_tx_data   : 20;
        FIELD rsv_20                    : 4;
        FIELD rg_disb_da_pon_tx_data    : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_5, *Ppon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_force_ge_txmapmodesel  : 1;
        FIELD rg_force_ge_rxmapmodesel  : 1;
        FIELD rg_force_ge_txpcsmodesel  : 1;
        FIELD rg_force_ge_rxpcsmodesel  : 1;
        FIELD rsv_4                     : 28;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_MODE_FORCE_CTRL_0, *Ppon_hsgmii_dig_csr_REG_PON_MODE_FORCE_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_or_pon_tx250m_ck       : 1;
        FIELD rg_or_hg_tx250m_ck        : 1;
        FIELD rg_or_sh_tx250m_ck        : 1;
        FIELD rg_or_pon_rx500m_ck       : 1;
        FIELD rg_or_hg_rx500m_ck        : 1;
        FIELD rg_or_baser_txck          : 1;
        FIELD rg_or_baser_rxck          : 1;
        FIELD rsv_7                     : 25;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_CLK_GEN_FORCE_CTRL_0, *Ppon_hsgmii_dig_csr_REG_CLK_GEN_FORCE_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_sigdet_force_mode      : 1;
        FIELD rg_sigdet_force_value     : 1;
        FIELD rsv_2                     : 30;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_ANALOG_FORCE_CTRL_0, *Ppon_hsgmii_dig_csr_REG_ANALOG_FORCE_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_ALGRTHM_SEL   : 1;
        FIELD RG_SSUSB_EQ_CHK_EYE_H     : 1;
        FIELD RG_SSUSB_EQ_DFEXEN_SEL    : 1;
        FIELD RG_SSUSB_EQ_DFEX_DIS      : 1;
        FIELD RG_SSUSB_EQ_DFEX_EN       : 1;
        FIELD RG_SSUSB_EQ_DFEX_RST      : 1;
        FIELD RG_SSUSB_EQ_DFEYEN_STOP_DIS : 1;
        FIELD RG_SSUSB_EQ_DFE_TOG       : 1;
        FIELD RG_SSUSB_EQ_EYE_CNT_EN    : 1;
        FIELD RG_SSUSB_EQ_EYE_MON_EN    : 1;
        FIELD RG_SSUSB_EQ_GATED_RXD_B   : 1;
        FIELD RG_SSUSB_EQ_PILPO_ROUT    : 1;
        FIELD RG_SSUSB_EQ_RX500M_CK_SEL : 1;
        FIELD RG_SSUSB_EQ_STOPTIME      : 1;
        FIELD RG_SSUSB_EQ_TRI_DET_EN    : 1;
        FIELD RG_SSUSB_FORCE_RX_FR_MODE : 1;
        FIELD RG_SSUSB_RX_FR_MODE       : 1;
        FIELD rsv_17                    : 7;
        FIELD RG_SSUSB_RX_DFE_EN        : 1;
        FIELD RG_SSUSB_RX_DFE_RST       : 1;
        FIELD rsv_26                    : 6;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_DD0HOS_LFI    : 7;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_EQ_DD0LOS_LFI    : 7;
        FIELD rsv_15                    : 1;
        FIELD RG_SSUSB_EQ_DD1HOS_LFI    : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_EQ_DD1LOS_LFI    : 7;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_1, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_DE0OS_LFI     : 7;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_EQ_DE1OS_LFI     : 7;
        FIELD rsv_15                    : 1;
        FIELD RG_SSUSB_EQ_DEYE0OS_LFI   : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_EQ_DEYE1OS_LFI   : 7;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_2, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_DFEXEN_DUR    : 3;
        FIELD rsv_3                     : 1;
        FIELD RG_SSUSB_EQ_DFEX_LF_SEL   : 3;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_EQ_DFEYEN_DUR    : 3;
        FIELD rsv_11                    : 5;
        FIELD RG_SSUSB_EQ_DHHLOS_LFI    : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_EQ_DHHL_LFI      : 7;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_3, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_DHHL_LF_SEL   : 3;
        FIELD rsv_3                     : 5;
        FIELD RG_SSUSB_EQ_DLEQOS_LFI    : 5;
        FIELD rsv_13                    : 3;
        FIELD RG_SSUSB_EQ_DLEQ_LFI_GEN1 : 4;
        FIELD rsv_20                    : 4;
        FIELD RG_SSUSB_EQ_DLEQ_LFI_GEN2 : 4;
        FIELD RG_SSUSB_EQ_DLEQ_LFI_GEN3 : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_4, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_DLEQ_LFI_GEN4 : 4;
        FIELD RG_SSUSB_EQ_DLEQ_LF_SEL   : 3;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_EQ_DLHLOS_LFI    : 7;
        FIELD rsv_15                    : 1;
        FIELD RG_SSUSB_EQ_DLHL_LFI      : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_EQ_DLHL_LF_SEL   : 3;
        FIELD rsv_27                    : 1;
        FIELD RG_SSUSB_EQ_DSAOS_LF_SEL  : 3;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_5, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_EYE0_Y        : 7;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_EQ_EYE1_Y        : 7;
        FIELD rsv_15                    : 17;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_EYE_CNT       : 20;
        FIELD rsv_20                    : 12;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_7, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_7;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_EYE_MASK      : 10;
        FIELD rsv_10                    : 6;
        FIELD RG_SSUSB_EQ_EYE_XOFFSET   : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_EQ_ISIFLAG_SEL   : 2;
        FIELD rsv_26                    : 2;
        FIELD RG_SSUSB_EQ_LEQMAX        : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_LEQOSC_DLYCNT : 3;
        FIELD rsv_3                     : 1;
        FIELD RG_SSUSB_EQ_LEQ_SHIFT     : 3;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_EQ_LEQ_STOP_TO   : 2;
        FIELD rsv_10                    : 6;
        FIELD RG_SSUSB_EQ_MON_SEL       : 5;
        FIELD rsv_21                    : 3;
        FIELD RG_SSUSB_EQ_PI0_INI       : 7;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_9, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_9;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_PI90CK_SEL    : 2;
        FIELD rsv_2                     : 6;
        FIELD RG_SSUSB_EQ_PI90_INI      : 7;
        FIELD rsv_15                    : 1;
        FIELD RG_SSUSB_EQ_PIEYE_INI     : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_EQ_PI_KPGAIN     : 3;
        FIELD rsv_27                    : 5;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_10, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_10;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_RESERVED      : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_11, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_11;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_REV_1         : 16;
        FIELD RG_SSUSB_EQ_REV           : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_SD_CNT0       : 6;
        FIELD rsv_6                     : 2;
        FIELD RG_SSUSB_EQ_SD_CNT1       : 6;
        FIELD rsv_14                    : 2;
        FIELD RG_SSUSB_EQ_SIGDET        : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_EQ_STARTTIME     : 2;
        FIELD rsv_26                    : 6;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_13, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_13;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_EQ_TRI_DET_TH    : 7;
        FIELD rsv_7                     : 25;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_14, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_14;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_EYE_CNT_RDY  : 1;
        FIELD RGS_SSUSB_EQ_LEQ_STOP     : 1;
        FIELD rsv_2                     : 30;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_0, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_DCD0H        : 7;
        FIELD rsv_7                     : 1;
        FIELD RGS_SSUSB_EQ_DCD0L        : 7;
        FIELD rsv_15                    : 1;
        FIELD RGS_SSUSB_EQ_DCD1H        : 7;
        FIELD rsv_23                    : 1;
        FIELD RGS_SSUSB_EQ_DCD1L        : 7;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_DCE0         : 7;
        FIELD rsv_7                     : 1;
        FIELD RGS_SSUSB_EQ_DCE1         : 7;
        FIELD rsv_15                    : 1;
        FIELD RGS_SSUSB_EQ_DCEYE0       : 7;
        FIELD rsv_23                    : 1;
        FIELD RGS_SSUSB_EQ_DCEYE1       : 7;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_DCHHL        : 7;
        FIELD rsv_7                     : 1;
        FIELD RGS_SSUSB_EQ_DCLEQ        : 4;
        FIELD rsv_12                    : 4;
        FIELD RGS_SSUSB_EQ_DCLEQOS      : 5;
        FIELD rsv_21                    : 3;
        FIELD RGS_SSUSB_EQ_DCLHL        : 7;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0 : 20;
        FIELD rsv_20                    : 12;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_4, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1 : 20;
        FIELD rsv_20                    : 12;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_5, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_PILPO        : 7;
        FIELD rsv_7                     : 9;
        FIELD RGS_SSUSB_EQ_STATUS       : 8;
        FIELD rsv_24                    : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_7, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_7;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RGS_SSUSB_EQ_PROBE_OUT    : 8;
        FIELD RGS_SSUSB_RX_SD_OUT       : 1;
        FIELD rsv_9                     : 23;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_8, *Ppon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_8;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_RX_IMPCALIB_EN   : 1;
        FIELD RG_PCIE_REFCK_DIV4        : 1;
        FIELD RG_PCIE_DEVICE            : 1;
        FIELD rsv_3                     : 1;
        FIELD RG_SSUSB_XTAL_EXT_EN      : 2;
        FIELD RG_PCIE_CLKRX_EN          : 1;
        FIELD RG_SSUSB_SCAN_MODE        : 1;
        FIELD RG_PCIE_CKDET_EN          : 1;
        FIELD rsv_9                     : 3;
        FIELD RG_SSUSB_PLL_FBKSEL       : 2;
        FIELD rsv_14                    : 1;
        FIELD RG_SSUSB_PLL_LOAD_EN      : 1;
        FIELD RG_SSUSB_PLL_BP           : 4;
        FIELD RG_SSUSB_PLL_IR           : 4;
        FIELD RG_SSUSB_PLL_BR           : 3;
        FIELD rsv_27                    : 1;
        FIELD RG_SSUSB_PLL_IC           : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_PLL_DIVEN        : 3;
        FIELD rsv_3                     : 1;
        FIELD RG_SSUSB_PLL_BC           : 2;
        FIELD rsv_6                     : 2;
        FIELD RG_SSUSB_PLL_PREDIV       : 2;
        FIELD RG_SSUSB_PLL_REFCKDIV     : 1;
        FIELD rsv_11                    : 1;
        FIELD RG_SSUSB_RXDET_EN         : 1;
        FIELD RG_SSUSB_TX_DEM           : 2;
        FIELD rsv_15                    : 9;
        FIELD RG_SSUSB_TX_IMPCALIB_EN   : 1;
        FIELD rsv_25                    : 3;
        FIELD RG_SSUSB_CDR_BPB          : 3;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_CDR_BPA          : 2;
        FIELD rsv_2                     : 6;
        FIELD RG_SSUSB_CDR_FBDIV        : 7;
        FIELD rsv_15                    : 1;
        FIELD RG_SSUSB_CDR_TCADJ        : 6;
        FIELD rsv_22                    : 2;
        FIELD RG_SSUSB_CDR_PI_MODE      : 1;
        FIELD RG_SSUSB_CDR_PI_PWD       : 1;
        FIELD RG_SSUSB_LFPS_PWD         : 1;
        FIELD rsv_27                    : 1;
        FIELD RG_SSUSB_EQ_RSTEP2        : 2;
        FIELD RG_SSUSB_EQ_RSTEP1        : 2;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_LFPS_DEGLITCH    : 2;
        FIELD RG_SSUSB_RX_P1_ENTRY      : 1;
        FIELD RG_SSUSB_RX_P3_ENTRY      : 1;
        FIELD rsv_4                     : 12;
        FIELD RG_SSUSB_RX_HZ            : 1;
        FIELD rsv_17                    : 3;
        FIELD RG_PCIE_CRSDET_OFST       : 4;
        FIELD RG_PCIE_CRSDET_RSTB       : 1;
        FIELD RG_PCIE_CRSDET_EN         : 1;
        FIELD RG_PCIE_CLKTX_EN          : 1;
        FIELD RG_SSUSB_SYSPLL_FBDIV_CHG : 1;
        FIELD rsv_28                    : 4;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_3, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_PLL_FBKDIV       : 9;
        FIELD rsv_9                     : 23;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_FORCE_RX_LEQOS_EN      : 1;
        FIELD RG_FORCE_PLL_BAND         : 1;
        FIELD RG_FORCE_CDR_BAND         : 1;
        FIELD RG_FORCE_PLL_BIAS_LPF_EN  : 1;
        FIELD RG_FORCE_PLL_SSC_EN       : 1;
        FIELD RG_FORCE_PLL_AUTOK_VCO    : 1;
        FIELD RG_FORCE_PLL_PWD          : 1;
        FIELD RG_FORCE_SYSPLL_PWD       : 1;
        FIELD RG_FORCE_XTAL_RX_PWD      : 1;
        FIELD RG_FORCE_PLL_BIAS_PWD     : 1;
        FIELD RG_FORCE_BG_LPF_EN        : 1;
        FIELD RG_FORCE_BIASIMR_PWD      : 1;
        FIELD RG_FORCE_BIAS_PWD         : 1;
        FIELD RG_FORCE_RX_SAOSC_EN      : 1;
        FIELD RG_FORCE_TX_LFPS_EN       : 1;
        FIELD RG_FORCE_TX_LFPS          : 1;
        FIELD RG_FORCE_TX_DRV_EN        : 1;
        FIELD RG_FORCE_RX_PI_CAL_EN     : 1;
        FIELD RG_FORCE_SIGDET_CAL_OFFSET : 1;
        FIELD RG_FORCE_SIGDET_CAL_EN    : 1;
        FIELD RG_FORCE_SIGDET_EN        : 1;
        FIELD RG_FORCE_TX_SER_EN        : 1;
        FIELD RG_FORCE_TX_EIDLE_LP_EN   : 1;
        FIELD RG_FORCE_TX_DATA_EN       : 1;
        FIELD RG_FORCE_RX_AFE_PWD       : 1;
        FIELD RG_FORCE_RX_CMPWD         : 1;
        FIELD RG_FORCE_CDR_LCK2REF      : 1;
        FIELD RG_FORCE_CDR_AUTOK_VCO    : 1;
        FIELD RG_FORCE_CDR_PWD          : 1;
        FIELD RG_FORCE_SYSPLL_SDM_ISO_EN : 1;
        FIELD RG_FORCE_SYSPLL_SDM_PWR_ON : 1;
        FIELD RG_FORCE_CDR_PI0          : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_FORCE_CDR_PI90         : 1;
        FIELD RG_FORCE_PLL_SSC_DELTA1   : 1;
        FIELD RG_FORCE_PLL_SSC_DELTA    : 1;
        FIELD RG_FORCE_PLL_SSC_PRD      : 1;
        FIELD RG_FORCE_SYSPLL_FBDIV     : 1;
        FIELD RG_FORCE_SYSPLL_RESERVE   : 1;
        FIELD RG_FORCE_SYSPLL_PREDIV    : 1;
        FIELD RG_FORCE_IDRV_0DB         : 1;
        FIELD RG_FORCE_IDRV_3P5DB       : 1;
        FIELD RG_FORCE_IDEM_3P5DB       : 1;
        FIELD RG_FORCE_IDRV_6DB         : 1;
        FIELD RG_FORCE_IDEM_6DB         : 1;
        FIELD RG_FORCE_TX_BIASI         : 1;
        FIELD RG_FORCE_TX_BIASI_EN      : 1;
        FIELD RG_FORCE_TX_EIDLE_CM      : 1;
        FIELD RG_FORCE_CDR_BIR          : 1;
        FIELD RG_FORCE_CDR_BIC          : 1;
        FIELD RG_FORCE_CDR_BC           : 1;
        FIELD RG_FORCE_CDR_BR           : 1;
        FIELD RG_FORCE_CDR_KVSEL        : 1;
        FIELD RG_FORCE_RX_CDR_STABLE    : 1;
        FIELD RG_FORCE_RX_SAOSC_RST     : 1;
        FIELD RG_FORCE_RX_EQ_RST        : 1;
        FIELD RG_FORCE_RX_EQ_EN         : 1;
        FIELD RG_FORCE_TX_IMP_SEL       : 1;
        FIELD RG_FORCE_RX_IMP_SEL       : 1;
        FIELD RG_FORCE_IEXT_INTR_CTRL   : 1;
        FIELD rsv_27                    : 5;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_6, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_6;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_PLL_BAND         : 11;
        FIELD rsv_11                    : 1;
        FIELD RG_SSUSB_RX_LEQOS_EN      : 1;
        FIELD rsv_13                    : 3;
        FIELD RG_SSUSB_CDR_BAND         : 7;
        FIELD rsv_23                    : 1;
        FIELD RG_SSUSB_PLL_BIAS_LPF_EN  : 1;
        FIELD RG_SSUSB_PLL_SSC_EN       : 1;
        FIELD RG_SSUSB_PLL_AUTOK_VCO    : 1;
        FIELD RG_SSUSB_PLL_PWD          : 1;
        FIELD RG_SSUSB_SYSPLL_PWD       : 1;
        FIELD RG_SSUSB_XTAL_RX_PWD      : 1;
        FIELD RG_SSUSB_PLL_BIAS_PWD     : 1;
        FIELD RG_SSUSB_BG_LPF_EN        : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_7, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_7;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_BIASIMR_PWD      : 1;
        FIELD RG_SSUSB_BIAS_PWD         : 1;
        FIELD RG_SSUSB_RX_SAOSC_EN      : 1;
        FIELD RG_SSUSB_TX_LFPS_EN       : 1;
        FIELD RG_SSUSB_TX_LFPS          : 1;
        FIELD RG_SSUSB_TX_DRV_EN        : 1;
        FIELD RG_SSUSB_RX_PI_CAL_EN     : 1;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_SIGDET_CAL_OFFSET : 5;
        FIELD RG_PCIE_SIGDET_CAL_EN     : 1;
        FIELD RG_PCIE_SIGDET_EN         : 1;
        FIELD RG_SSUSB_TX_SER_EN        : 1;
        FIELD RG_SSUSB_TX_EIDLE_LP_EN   : 1;
        FIELD RG_SSUSB_TX_DATA_EN       : 1;
        FIELD RG_SSUSB_RX_AFE_PWD       : 1;
        FIELD RG_SSUSB_RX_CMPWD         : 1;
        FIELD RG_SSUSB_CDR_LCK2REF      : 1;
        FIELD RG_SSUSB_CDR_AUTOK_VCO    : 1;
        FIELD RG_SSUSB_CDR_PWD          : 1;
        FIELD RG_SSUSB_SYSPLL_SDM_ISO_EN : 1;
        FIELD RG_SSUSB_SYSPLL_SDM_PWR_ON : 1;
        FIELD rsv_25                    : 7;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_8;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_CDR_PI0          : 7;
        FIELD rsv_7                     : 1;
        FIELD RG_SSUSB_CDR_PI90         : 7;
        FIELD rsv_15                    : 17;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_9, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_9;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_PLL_SSC_DELTA1   : 16;
        FIELD RG_SSUSB_PLL_SSC_DELTA    : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_10, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_10;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_SYSPLL_FBDIV     : 31;
        FIELD rsv_31                    : 1;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_11, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_11;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rsv_0                     : 16;
        FIELD RG_SSUSB_IDRV_0DB         : 6;
        FIELD rsv_22                    : 2;
        FIELD RG_SSUSB_IDRV_3P5DB       : 6;
        FIELD rsv_30                    : 2;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_12, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_12;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_IDEM_3P5DB       : 6;
        FIELD rsv_6                     : 2;
        FIELD RG_SSUSB_IDRV_6DB         : 6;
        FIELD rsv_14                    : 2;
        FIELD RG_SSUSB_IDEM_6DB         : 6;
        FIELD rsv_22                    : 2;
        FIELD RG_SSUSB_TX_BIASI         : 3;
        FIELD rsv_27                    : 1;
        FIELD RG_SSUSB_TX_BIASI_EN      : 1;
        FIELD rsv_29                    : 3;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_13, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_13;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_TX_EIDLE_CM      : 4;
        FIELD RG_SSUSB_CDR_BIR          : 1;
        FIELD RG_SSUSB_RX_CDR_STABLE    : 1;
        FIELD RG_SSUSB_RX_SAOSC_RST     : 1;
        FIELD RG_SSUSB_RX_EQ_RST        : 1;
        FIELD RG_SSUSB_RX_EQ_EN         : 1;
        FIELD rsv_9                     : 7;
        FIELD RG_SSUSB_TX_IMP_SEL       : 5;
        FIELD rsv_21                    : 3;
        FIELD RG_SSUSB_RX_IMP_SEL       : 5;
        FIELD rsv_29                    : 3;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_14, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_14;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rsv_0                     : 8;
        FIELD RG_SSUSB_CDR_BIRLTR       : 5;
        FIELD rsv_13                    : 3;
        FIELD RG_SSUSB_CDR_BIRLTD0      : 5;
        FIELD rsv_21                    : 3;
        FIELD RG_SSUSB_CDR_BIRLTD1      : 5;
        FIELD rsv_29                    : 3;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_15, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_15;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_CDR_BICLTR       : 4;
        FIELD RG_SSUSB_CDR_BICLTD0      : 4;
        FIELD RG_SSUSB_CDR_BICLTD1      : 4;
        FIELD rsv_12                    : 4;
        FIELD RG_SSUSB_CDR_BCLTR        : 5;
        FIELD rsv_21                    : 3;
        FIELD RG_SSUSB_CDR_BCLTD0       : 5;
        FIELD rsv_29                    : 3;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_16, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_16;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_CDR_BCLTD1       : 5;
        FIELD rsv_5                     : 3;
        FIELD RG_SSUSB_CDR_BRLTR        : 3;
        FIELD rsv_11                    : 1;
        FIELD RG_SSUSB_CDR_BRLTD0       : 3;
        FIELD rsv_15                    : 1;
        FIELD RG_SSUSB_CDR_BRLTD1       : 3;
        FIELD RG_SSUSB_CDR_KVSELLTR     : 1;
        FIELD RG_SSUSB_CDR_KVSELLTD0    : 1;
        FIELD RG_SSUSB_CDR_KVSELLTD1    : 1;
        FIELD rsv_22                    : 10;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_17, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_17;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_FORCE_SSUSB_PLL_BPA    : 1;
        FIELD RG_FORCE_SSUSB_PLL_BPB    : 1;
        FIELD rsv_2                     : 30;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_18, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_18;

typedef PACKING union
{
    PACKING struct
    {
        FIELD RG_SSUSB_PLL_BPA          : 4;
        FIELD RG_SSUSB_PLL_BPB          : 2;
        FIELD rsv_6                     : 26;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19, *Ppon_hsgmii_dig_csr_REG_PON_DA_CTRL_19;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_lck2ref                : 1;
        FIELD ro_ad_sigdet_out          : 1;
        FIELD ro_ad_sigdet_cal_out      : 1;
        FIELD ro_eq_done                : 1;
        FIELD ro_rx_eq_en               : 1;
        FIELD ro_rx_path_rdy            : 1;
        FIELD ro_ad_rx_impcal_out       : 1;
        FIELD ro_ad_rx_lfps             : 1;
        FIELD ro_intf_pma_rx_sigdet_out : 1;
        FIELD ro_ad_ben_out             : 1;
        FIELD rsv_10                    : 6;
        FIELD ro_ad_ben_out_cnt         : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_eq_done_clr            : 1;
        FIELD rsv_1                     : 31;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_1, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_1;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_pll_vco_cal_state      : 7;
        FIELD rsv_7                     : 1;
        FIELD ro_txbit_dly_sel          : 3;
        FIELD rsv_11                    : 1;
        FIELD ro_txbit_dly_sel_latch    : 4;
        FIELD ro_da_tx_data_en          : 1;
        FIELD rsv_17                    : 7;
        FIELD ro_qphy_rx_ctrl_debug     : 8;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_2, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_2;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_pon_tx_freq_lock       : 1;
        FIELD ro_pon_rx_freq_lock       : 1;
        FIELD ro_hg_tx_freq_lock        : 1;
        FIELD ro_hg_rx_freq_lock        : 1;
        FIELD ro_baser_tx_freq_lock     : 1;
        FIELD ro_baser_rx_freq_lock     : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_freq_clr               : 1;
        FIELD rsv_9                     : 7;
        FIELD ro_saosc_done             : 1;
        FIELD ro_ad_pcie_ckdet_out      : 1;
        FIELD ro_ad_pcie_crsdet_cal_out : 1;
        FIELD ro_ad_pcie_crsdet_lthout  : 1;
        FIELD ro_ad_pcie_crsdet_out     : 1;
        FIELD rsv_21                    : 11;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_3, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_3;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_ben_counter            : 16;
        FIELD rg_ben_count_en           : 1;
        FIELD rsv_17                    : 15;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_4, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_4;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_dummy_reg              : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DUMMY_REG_0, *Ppon_hsgmii_dig_csr_REG_PON_DUMMY_REG_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_dec_err_insert_en  : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_fec_dec_err_insert_cont : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_fec_dec_usr_def_error_en : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_fec_dec_insert_dbg_clr : 1;
        FIELD rsv_13                    : 19;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_5, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_5;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_dec_err_byte_num   : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_6, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_6;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_dec_usr_def_error  : 16;
        FIELD rsv_16                    : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_7, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_7;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_dec_err_insert_start_w1c : 1;
        FIELD rsv_1                     : 31;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_8, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_8;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_enc_err_insert_en  : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_fec_enc_err_insert_cont : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_fec_enc_usr_def_error_en : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_fec_enc_insert_dbg_clr : 1;
        FIELD rsv_13                    : 19;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_9, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_9;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_enc_err_byte_num   : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_10, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_10;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_enc_usr_def_error  : 16;
        FIELD rsv_16                    : 16;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_11, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_11;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_fec_enc_err_insert_start_w1c : 1;
        FIELD rsv_1                     : 31;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_12, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_12;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_phyrdy_pro_cnt_thr     : 4;
        FIELD rsv_4                     : 4;
        FIELD rg_trans_los_pro_cnt_thr  : 4;
        FIELD rsv_12                    : 20;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_13, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_13;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_int_record_start_clr_w1c : 1;
        FIELD rsv_1                     : 31;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_14, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_14;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_int_record_en          : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_lof_int_mask_en        : 1;
        FIELD rg_lof_int_mask_keep      : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_record_start_dbg_clr   : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_record_start_type      : 3;
        FIELD rsv_15                    : 17;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_15, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_15;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_lof_time_cnt           : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_16, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_16;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_los_time_cnt           : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_17, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_17;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_phyrdy_time_cnt        : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_18, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_18;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_tx_bit_dly             : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_19, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_19;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_int_record_cnt_thr     : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_20, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_20;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_fec_dec_insert_dbg     : 1;
        FIELD rsv_1                     : 3;
        FIELD ro_record_start_dbg       : 1;
        FIELD rsv_5                     : 3;
        FIELD ro_fec_enc_insert_dbg     : 1;
        FIELD rsv_9                     : 3;
        FIELD ro_record_done            : 1;
        FIELD rsv_13                    : 19;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_21, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_21;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_lof_int_mask_cnt_thr   : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_22, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_22;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rgs_pll_autok_band        : 12;
        FIELD rsv_12                    : 20;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_23, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_23;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_fpga_lock_status       : 4;
        FIELD ro_fpga_rx_los_lock_cnt   : 4;
        FIELD ro_fpga_tx_los_lock_cnt   : 4;
        FIELD rsv_12                    : 4;
        FIELD rg_fpga_rx_force_lck      : 1;
        FIELD rg_fpga_rx_lck_rst        : 1;
        FIELD rsv_18                    : 14;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_24, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_24;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_pon_phy_dsram0_delsel  : 2;
        FIELD rg_pon_phy_dsram1_delsel  : 2;
        FIELD rg_pon_phy_dsram2_delsel  : 2;
        FIELD rg_pon_phy_dsram3_delsel  : 2;
        FIELD rg_pon_phy_ssram_delsel   : 2;
        FIELD rg_gepon_phy_epsram_delsel : 2;
        FIELD rg_gepon_phy_dlybnd_delsel : 2;
        FIELD rsv_14                    : 18;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_25, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_25;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_fpga_debug_0           : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_26, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_26;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_fpga_debug_1           : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_27, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_27;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_ad_cdr_vcocal_state    : 7;
        FIELD rsv_7                     : 1;
        FIELD ro_ad_cdr_vcocal_cplt     : 1;
        FIELD ro_ad_cdr_vcocal_fail     : 1;
        FIELD rsv_10                    : 6;
        FIELD ro_ad_cdr_band            : 7;
        FIELD rsv_23                    : 1;
        FIELD ro_ad_sig_cal_offset      : 5;
        FIELD rsv_29                    : 3;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_28, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_28;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_sfp_tx_en_counter      : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_29, *Ppon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_29;

typedef PACKING union
{
    PACKING struct
    {
        FIELD ro_ncpo_max_flag          : 1;
        FIELD ro_ncpo_min_flag          : 1;
        FIELD ro_pma_rx_sigdet_out      : 1;
        FIELD ro_rx_pll_meter_lock      : 1;
        FIELD ro_rx_pll_meter_unlock    : 1;
        FIELD ro_los_int                : 1;
        FIELD ro_rx_sd_int              : 1;
        FIELD ro_tx_en_int              : 1;
        FIELD rsv_8                     : 24;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_INTERRUPT_STS_0, *Ppon_hsgmii_dig_csr_REG_PON_INTERRUPT_STS_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_ncpo_max_int_en        : 1;
        FIELD rg_ncpo_min_int_en        : 1;
        FIELD rg_pma_sigdet_int_en      : 1;
        FIELD rg_rx_pll_meter_lock_int_en : 1;
        FIELD rg_rx_pll_meter_unlock_int_en : 1;
        FIELD rg_los_int_en             : 1;
        FIELD rg_rx_sd_int_en           : 1;
        FIELD rg_tx_en_int_en           : 1;
        FIELD rsv_8                     : 24;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_INTERRUPT_EN_0, *Ppon_hsgmii_dig_csr_REG_PON_INTERRUPT_EN_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_ncpo_clr               : 1;
        FIELD rg_ncpo_flag_clr          : 1;
        FIELD rg_pma_rx_sigdet_intclr   : 1;
        FIELD rg_rx_pll_meter_lock_int_clr : 1;
        FIELD rg_rx_pll_meter_unlock_int_clr : 1;
        FIELD rg_los_int_clr            : 1;
        FIELD rg_rx_sd_int_clr          : 1;
        FIELD rg_tx_en_int_clr          : 1;
        FIELD rsv_8                     : 24;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_INTERRUPT_CLR_0, *Ppon_hsgmii_dig_csr_REG_PON_INTERRUPT_CLR_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_int_los_cnt            : 8;
        FIELD rg_int_los_on_pro         : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_rogue_en               : 1;
        FIELD rsv_13                    : 3;
        FIELD rg_ncpo_det_en            : 1;
        FIELD rsv_17                    : 15;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_0, *Ppon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_0;

typedef PACKING union
{
    PACKING struct
    {
        FIELD rg_tx_en_counter          : 32;
    } Bits;
    UINT32 Raw;
} pon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_1, *Ppon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_1;

// ----------------- pon_hsgmii_dig_csr  Grouping Definitions -------------------
// ----------------- pon_hsgmii_dig_csr Register Definition -------------------
#if 0
typedef volatile PACKING struct
{
    UINT32                          rsv_4000[64];     // 4000..40FC
    pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_0 SS_LCPLL_PWCTL_SETTING_0; // 4100
    pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_1 SS_LCPLL_PWCTL_SETTING_1; // 4104
    pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_2 SS_LCPLL_PWCTL_SETTING_2; // 4108
    pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_DBG_SETTING SS_LCPLL_PWCTL_DBG_SETTING; // 410C
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_0 SS_LCPLL_TDC_PW_0; // 4110
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_1 SS_LCPLL_TDC_PW_1; // 4114
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_2 SS_LCPLL_TDC_PW_2; // 4118
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_3 SS_LCPLL_TDC_PW_3; // 411C
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_4 SS_LCPLL_TDC_PW_4; // 4120
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_5 SS_LCPLL_TDC_PW_5; // 4124
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_0 SS_LCPLL_TDC_FLT_0; // 4128
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_1 SS_LCPLL_TDC_FLT_1; // 412C
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2 SS_LCPLL_TDC_FLT_2; // 4130
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3 SS_LCPLL_TDC_FLT_3; // 4134
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_4 SS_LCPLL_TDC_FLT_4; // 4138
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5 SS_LCPLL_TDC_FLT_5; // 413C
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_6 SS_LCPLL_TDC_FLT_6; // 4140
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_7 SS_LCPLL_TDC_FLT_7; // 4144
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_1 SS_LCPLL_TDC_PCW_1; // 4148
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_2 SS_LCPLL_TDC_PCW_2; // 414C
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_1 SS_LCPLL_TDC_RO_1; // 4150
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_2 SS_LCPLL_TDC_RO_2; // 4154
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_3 SS_LCPLL_TDC_RO_3; // 4158
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_4 SS_LCPLL_TDC_RO_4; // 415C
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_5 SS_LCPLL_TDC_RO_5; // 4160
    pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_CTRL_0 SS_LCPLL_TDC_CTRL_0; // 4164
    pon_hsgmii_dig_csr_REG_SS_LCPLL_0 SS_LCPLL_0;     // 4168
    pon_hsgmii_dig_csr_REG_SS_LCPLL_1 SS_LCPLL_1;     // 416C
    pon_hsgmii_dig_csr_REG_SS_LCPLL_2 SS_LCPLL_2;     // 4170
    pon_hsgmii_dig_csr_REG_TDC_DLF_GAIN_STATUS TDC_DLF_GAIN_STATUS; // 4174
    pon_hsgmii_dig_csr_REG_TDC_DLF_MODE_SETTING TDC_DLF_MODE_SETTING; // 4178
    pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_0 TDC_HOLD_CTRL_0; // 417C
    pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_1 TDC_HOLD_CTRL_1; // 4180
    pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_2 TDC_HOLD_CTRL_2; // 4184
    pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_0 TDC_HOLD_STATUS_0; // 4188
    pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_1 TDC_HOLD_STATUS_1; // 418C
    pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_2 TDC_HOLD_STATUS_2; // 4190
    UINT32                          rsv_4194[27];     // 4194..41FC
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0 PON_SERDES_CTRL_0; // 4200
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_1 PON_SERDES_CTRL_1; // 4204
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_2 PON_SERDES_CTRL_2; // 4208
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_3 PON_SERDES_CTRL_3; // 420C
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_4 PON_SERDES_CTRL_4; // 4210
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_5 PON_SERDES_CTRL_5; // 4214
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_6 PON_SERDES_CTRL_6; // 4218
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_7 PON_SERDES_CTRL_7; // 421C
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_8 PON_SERDES_CTRL_8; // 4220
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_9 PON_SERDES_CTRL_9; // 4224
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10 PON_SERDES_CTRL_10; // 4228
    pon_hsgmii_dig_csr_REG_PON_SERDES_STS_0 PON_SERDES_STS_0; // 422C
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_11 PON_SERDES_CTRL_11; // 4230
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_12 PON_SERDES_CTRL_12; // 4234
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_13 PON_SERDES_CTRL_13; // 4238
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_14 PON_SERDES_CTRL_14; // 423C
    pon_hsgmii_dig_csr_REG_PON_SERDES_STS_1 PON_SERDES_STS_1; // 4240
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_15 PON_SERDES_CTRL_15; // 4244
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_16 PON_SERDES_CTRL_16; // 4248
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_17 PON_SERDES_CTRL_17; // 424C
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_18 PON_SERDES_CTRL_18; // 4250
    pon_hsgmii_dig_csr_REG_PON_SERDES_STS_2 PON_SERDES_STS_2; // 4254
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_19 PON_SERDES_CTRL_19; // 4258
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_20 PON_SERDES_CTRL_20; // 425C
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_21 PON_SERDES_CTRL_21; // 4260
    pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_22 PON_SERDES_CTRL_22; // 4264
    UINT32                          rsv_4268[38];     // 4268..42FC
    pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0 PON_SYS_CTRL_0; // 4300
    pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_1 PON_SYS_CTRL_1; // 4304
    pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_2 PON_SYS_CTRL_2; // 4308
    pon_hsgmii_dig_csr_REG_PON_CK_CTRL_0 PON_CK_CTRL_0; // 430C
    pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0 HG_RST_CTRL_0; // 4310
    pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0 HG_MODE_CTRL_0; // 4314
    pon_hsgmii_dig_csr_REG_HG_RG_CTRL_0 HG_RG_CTRL_0; // 4318
    pon_hsgmii_dig_csr_REG_PON_STS_0 PON_STS_0;       // 431C
    pon_hsgmii_dig_csr_REG_PON_OSR_SEL_CTRL PON_OSR_SEL_CTRL; // 4320
    pon_hsgmii_dig_csr_REG_CDR_CTRL_0 CDR_CTRL_0;     // 4324
    pon_hsgmii_dig_csr_REG_CDR_CTRL_1 CDR_CTRL_1;     // 4328
    pon_hsgmii_dig_csr_REG_CDR_CTRL_2 CDR_CTRL_2;     // 432C
    pon_hsgmii_dig_csr_REG_CDR_CTRL_3 CDR_CTRL_3;     // 4330
    pon_hsgmii_dig_csr_REG_CDR_CTRL_4 CDR_CTRL_4;     // 4334
    pon_hsgmii_dig_csr_REG_CDR_CTRL_5 CDR_CTRL_5;     // 4338
    pon_hsgmii_dig_csr_REG_TX_CTRL_0 TX_CTRL_0;       // 433C
    pon_hsgmii_dig_csr_REG_TX_CTRL_1 TX_CTRL_1;       // 4340
    pon_hsgmii_dig_csr_REG_TX_CTRL_2 TX_CTRL_2;       // 4344
    pon_hsgmii_dig_csr_REG_TX_CTRL_3 TX_CTRL_3;       // 4348
    pon_hsgmii_dig_csr_REG_TX_CTRL_4 TX_CTRL_4;       // 434C
    pon_hsgmii_dig_csr_REG_TX_CTRL_5 TX_CTRL_5;       // 4350
    pon_hsgmii_dig_csr_REG_TX_CTRL_6 TX_CTRL_6;       // 4354
    pon_hsgmii_dig_csr_REG_TX_CTRL_7 TX_CTRL_7;       // 4358
    pon_hsgmii_dig_csr_REG_FRQ_CTRL_0 FRQ_CTRL_0;     // 435C
    pon_hsgmii_dig_csr_REG_FRQ_CTRL_1 FRQ_CTRL_1;     // 4360
    pon_hsgmii_dig_csr_REG_FRQ_CTRL_2 FRQ_CTRL_2;     // 4364
    pon_hsgmii_dig_csr_REG_FRQ_CTRL_3 FRQ_CTRL_3;     // 4368
    pon_hsgmii_dig_csr_REG_FRQ_CTRL_4 FRQ_CTRL_4;     // 436C
    pon_hsgmii_dig_csr_REG_FRQ_CTRL_5 FRQ_CTRL_5;     // 4370
    pon_hsgmii_dig_csr_REG_RX_FREQDET_0 RX_FREQDET_0; // 4374
    pon_hsgmii_dig_csr_REG_RX_FREQDET_1 RX_FREQDET_1; // 4378
    pon_hsgmii_dig_csr_REG_RX_FREQDET_2 RX_FREQDET_2; // 437C
    pon_hsgmii_dig_csr_REG_RX_FREQDET_3 RX_FREQDET_3; // 4380
    pon_hsgmii_dig_csr_REG_RX_FREQDET_STS_0 RX_FREQDET_STS_0; // 4384
    pon_hsgmii_dig_csr_REG_TX_CTRL_8 TX_CTRL_8;       // 4388
    pon_hsgmii_dig_csr_REG_TX_CTRL_9 TX_CTRL_9;       // 438C
    pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_0 RX_FREQDET_DET_CTRL_0; // 4390
    pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_1 RX_FREQDET_DET_CTRL_1; // 4394
    pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_2 RX_FREQDET_DET_CTRL_2; // 4398
    pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_4 RX_FREQDET_DET_CTRL_4; // 439C
    UINT32                          rsv_43A0[24];     // 43A0..43FC
    pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_0 PON_DA_MUX_CTRL_0; // 4400
    pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_1 PON_DA_MUX_CTRL_1; // 4404
    pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_2 PON_DA_MUX_CTRL_2; // 4408
    pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_3 PON_DA_MUX_CTRL_3; // 440C
    pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_4 PON_DA_MUX_CTRL_4; // 4410
    pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_5 PON_DA_MUX_CTRL_5; // 4414
    pon_hsgmii_dig_csr_REG_PON_MODE_FORCE_CTRL_0 PON_MODE_FORCE_CTRL_0; // 4418
    pon_hsgmii_dig_csr_REG_CLK_GEN_FORCE_CTRL_0 CLK_GEN_FORCE_CTRL_0; // 441C
    pon_hsgmii_dig_csr_REG_ANALOG_FORCE_CTRL_0 ANALOG_FORCE_CTRL_0; // 4420
    UINT32                          rsv_4424[55];     // 4424..44FC
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0 PON_RXFEDIG_CTRL_0; // 4500
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_1 PON_RXFEDIG_CTRL_1; // 4504
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_2 PON_RXFEDIG_CTRL_2; // 4508
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_3 PON_RXFEDIG_CTRL_3; // 450C
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_4 PON_RXFEDIG_CTRL_4; // 4510
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_5 PON_RXFEDIG_CTRL_5; // 4514
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6 PON_RXFEDIG_CTRL_6; // 4518
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_7 PON_RXFEDIG_CTRL_7; // 451C
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8 PON_RXFEDIG_CTRL_8; // 4520
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_9 PON_RXFEDIG_CTRL_9; // 4524
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_10 PON_RXFEDIG_CTRL_10; // 4528
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_11 PON_RXFEDIG_CTRL_11; // 452C
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12 PON_RXFEDIG_CTRL_12; // 4530
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_13 PON_RXFEDIG_CTRL_13; // 4534
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_14 PON_RXFEDIG_CTRL_14; // 4538
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_0 PON_RXFEDIG_STS_0; // 453C
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1 PON_RXFEDIG_STS_1; // 4540
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2 PON_RXFEDIG_STS_2; // 4544
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3 PON_RXFEDIG_STS_3; // 4548
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_4 PON_RXFEDIG_STS_4; // 454C
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_5 PON_RXFEDIG_STS_5; // 4550
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_7 PON_RXFEDIG_STS_7; // 4554
    pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_8 PON_RXFEDIG_STS_8; // 4558
    UINT32                          rsv_455C[41];     // 455C..45FC
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0 PON_DA_CTRL_0; // 4600
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1 PON_DA_CTRL_1; // 4604
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2 PON_DA_CTRL_2; // 4608
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_3 PON_DA_CTRL_3; // 460C
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4 PON_DA_CTRL_4; // 4610
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5 PON_DA_CTRL_5; // 4614
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_6 PON_DA_CTRL_6; // 4618
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_7 PON_DA_CTRL_7; // 461C
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8 PON_DA_CTRL_8; // 4620
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_9 PON_DA_CTRL_9; // 4624
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_10 PON_DA_CTRL_10; // 4628
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_11 PON_DA_CTRL_11; // 462C
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_12 PON_DA_CTRL_12; // 4630
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_13 PON_DA_CTRL_13; // 4634
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_14 PON_DA_CTRL_14; // 4638
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_15 PON_DA_CTRL_15; // 463C
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_16 PON_DA_CTRL_16; // 4640
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_17 PON_DA_CTRL_17; // 4644
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_18 PON_DA_CTRL_18; // 4648
    pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19 PON_DA_CTRL_19; // 464C
    UINT32                          rsv_4650[44];     // 4650..46FC
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0 PON_DEBUG_REGISTER_0; // 4700
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_1 PON_DEBUG_REGISTER_1; // 4704
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_2 PON_DEBUG_REGISTER_2; // 4708
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_3 PON_DEBUG_REGISTER_3; // 470C
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_4 PON_DEBUG_REGISTER_4; // 4710
    pon_hsgmii_dig_csr_REG_PON_DUMMY_REG_0 PON_DUMMY_REG_0; // 4714
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_5 PON_DEBUG_REGISTER_5; // 4718
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_6 PON_DEBUG_REGISTER_6; // 471C
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_7 PON_DEBUG_REGISTER_7; // 4720
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_8 PON_DEBUG_REGISTER_8; // 4724
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_9 PON_DEBUG_REGISTER_9; // 4728
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_10 PON_DEBUG_REGISTER_10; // 472C
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_11 PON_DEBUG_REGISTER_11; // 4730
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_12 PON_DEBUG_REGISTER_12; // 4734
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_13 PON_DEBUG_REGISTER_13; // 4738
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_14 PON_DEBUG_REGISTER_14; // 473C
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_15 PON_DEBUG_REGISTER_15; // 4740
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_16 PON_DEBUG_REGISTER_16; // 4744
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_17 PON_DEBUG_REGISTER_17; // 4748
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_18 PON_DEBUG_REGISTER_18; // 474C
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_19 PON_DEBUG_REGISTER_19; // 4750
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_20 PON_DEBUG_REGISTER_20; // 4754
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_21 PON_DEBUG_REGISTER_21; // 4758
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_22 PON_DEBUG_REGISTER_22; // 475C
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_23 PON_DEBUG_REGISTER_23; // 4760
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_24 PON_DEBUG_REGISTER_24; // 4764
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_25 PON_DEBUG_REGISTER_25; // 4768
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_26 PON_DEBUG_REGISTER_26; // 476C
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_27 PON_DEBUG_REGISTER_27; // 4770
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_28 PON_DEBUG_REGISTER_28; // 4774
    pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_29 PON_DEBUG_REGISTER_29; // 4778
    UINT32                          rsv_477C[33];     // 477C..47FC
    pon_hsgmii_dig_csr_REG_PON_INTERRUPT_STS_0 PON_INTERRUPT_STS_0; // 4800
    pon_hsgmii_dig_csr_REG_PON_INTERRUPT_EN_0 PON_INTERRUPT_EN_0; // 4804
    pon_hsgmii_dig_csr_REG_PON_INTERRUPT_CLR_0 PON_INTERRUPT_CLR_0; // 4808
    pon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_0 PON_INTERRUPT_SET_0; // 480C
    pon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_1 PON_INTERRUPT_SET_1; // 4810
}pon_hsgmii_dig_csr_REGS, *Ppon_hsgmii_dig_csr_REGS;

#endif
// ---------- pon_hsgmii_dig_csr Enum Definitions      ----------
// ---------- pon_hsgmii_dig_csr C Macro Definitions   ----------
//extern Ppon_hsgmii_dig_csr_REGS g_pon_hsgmii_dig_csr_BASE;

//#define pon_hsgmii_dig_csr_BASE                                (g_pon_hsgmii_dig_csr_BASE)

#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_0		0x4100
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_1		0x4104
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_SETTING_2		0x4108
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_PWCTL_DBG_SETTING		0x410C
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_0		0x4110
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_1		0x4114
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_2		0x4118
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_3		0x411C
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_4		0x4120
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PW_5		0x4124
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_0		0x4128
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_1		0x412C
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_2		0x4130
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_3		0x4134
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_4		0x4138
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_5		0x413C
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_6		0x4140
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_FLT_7		0x4144
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_1		0x4148
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_PCW_2		0x414C
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_1		0x4150
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_2		0x4154
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_3		0x4158
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_4		0x415C
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_RO_5		0x4160
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_TDC_CTRL_0		0x4164
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_0		0x4168
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_1		0x416C
#define _pon_hsgmii_dig_csr_REG_SS_LCPLL_2		0x4170
#define _pon_hsgmii_dig_csr_REG_TDC_DLF_GAIN_STATUS		0x4174
#define _pon_hsgmii_dig_csr_REG_TDC_DLF_MODE_SETTING		0x4178
#define _pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_0		0x417C
#define _pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_1		0x4180
#define _pon_hsgmii_dig_csr_REG_TDC_HOLD_CTRL_2		0x4184
#define _pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_0		0x4188
#define _pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_1		0x418C
#define _pon_hsgmii_dig_csr_REG_TDC_HOLD_STATUS_2		0x4190
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_0		0x4200
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_1		0x4204
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_2		0x4208
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_3		0x420C
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_4		0x4210
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_5		0x4214
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_6		0x4218
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_7		0x421C
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_8		0x4220
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_9		0x4224
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_10		0x4228
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_STS_0		0x422C
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_11		0x4230
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_12		0x4234
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_13		0x4238
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_14		0x423C
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_STS_1		0x4240
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_15		0x4244
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_16		0x4248
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_17		0x424C
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_18		0x4250
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_STS_2		0x4254
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_19		0x4258
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_20		0x425C
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_21		0x4260
#define _pon_hsgmii_dig_csr_REG_PON_SERDES_CTRL_22		0x4264
#define _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_0		0x4300
#define _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_1		0x4304
#define _pon_hsgmii_dig_csr_REG_PON_SYS_CTRL_2		0x4308
#define _pon_hsgmii_dig_csr_REG_PON_CK_CTRL_0		0x430C
#define _pon_hsgmii_dig_csr_REG_HG_RST_CTRL_0		0x4310
#define _pon_hsgmii_dig_csr_REG_HG_MODE_CTRL_0		0x4314
#define _pon_hsgmii_dig_csr_REG_HG_RG_CTRL_0		0x4318
#define _pon_hsgmii_dig_csr_REG_PON_STS_0		0x431C
#define _pon_hsgmii_dig_csr_REG_PON_OSR_SEL_CTRL		0x4320
#define _pon_hsgmii_dig_csr_REG_CDR_CTRL_0		0x4324
#define _pon_hsgmii_dig_csr_REG_CDR_CTRL_1		0x4328
#define _pon_hsgmii_dig_csr_REG_CDR_CTRL_2		0x432C
#define _pon_hsgmii_dig_csr_REG_CDR_CTRL_3		0x4330
#define _pon_hsgmii_dig_csr_REG_CDR_CTRL_4		0x4334
#define _pon_hsgmii_dig_csr_REG_CDR_CTRL_5		0x4338
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_0		0x433C
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_1		0x4340
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_2		0x4344
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_3		0x4348
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_4		0x434C
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_5		0x4350
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_6		0x4354
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_7		0x4358
#define _pon_hsgmii_dig_csr_REG_FRQ_CTRL_0		0x435C
#define _pon_hsgmii_dig_csr_REG_FRQ_CTRL_1		0x4360
#define _pon_hsgmii_dig_csr_REG_FRQ_CTRL_2		0x4364
#define _pon_hsgmii_dig_csr_REG_FRQ_CTRL_3		0x4368
#define _pon_hsgmii_dig_csr_REG_FRQ_CTRL_4		0x436C
#define _pon_hsgmii_dig_csr_REG_FRQ_CTRL_5		0x4370
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_0		0x4374
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_1		0x4378
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_2		0x437C
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_3		0x4380
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_STS_0		0x4384
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_8		0x4388
#define _pon_hsgmii_dig_csr_REG_TX_CTRL_9		0x438C
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_0		0x4390
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_1		0x4394
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_2		0x4398
#define _pon_hsgmii_dig_csr_REG_RX_FREQDET_DET_CTRL_4		0x439C
#define _pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_0		0x4400
#define _pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_1		0x4404
#define _pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_2		0x4408
#define _pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_3		0x440C
#define _pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_4		0x4410
#define _pon_hsgmii_dig_csr_REG_PON_DA_MUX_CTRL_5		0x4414
#define _pon_hsgmii_dig_csr_REG_PON_MODE_FORCE_CTRL_0		0x4418
#define _pon_hsgmii_dig_csr_REG_CLK_GEN_FORCE_CTRL_0		0x441C
#define _pon_hsgmii_dig_csr_REG_ANALOG_FORCE_CTRL_0		0x4420
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_0		0x4500
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_1		0x4504
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_2		0x4508
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_3		0x450C
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_4		0x4510
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_5		0x4514
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_6		0x4518
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_7		0x451C
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_8		0x4520
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_9		0x4524
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_10		0x4528
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_11		0x452C
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_12		0x4530
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_13		0x4534
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_CTRL_14		0x4538
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_0		0x453C
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_1		0x4540
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_2		0x4544
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_3		0x4548
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_4		0x454C
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_5		0x4550
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_7		0x4554
#define _pon_hsgmii_dig_csr_REG_PON_RXFEDIG_STS_8		0x4558
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_0		0x4600
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_1		0x4604
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_2		0x4608
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_3		0x460C
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_4		0x4610
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_5		0x4614
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_6		0x4618
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_7		0x461C
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_8		0x4620
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_9		0x4624
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_10		0x4628
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_11		0x462C
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_12		0x4630
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_13		0x4634
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_14		0x4638
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_15		0x463C
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_16		0x4640
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_17		0x4644
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_18		0x4648
#define _pon_hsgmii_dig_csr_REG_PON_DA_CTRL_19		0x464C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_0		0x4700
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_1		0x4704
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_2		0x4708
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_3		0x470C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_4		0x4710
#define _pon_hsgmii_dig_csr_REG_PON_DUMMY_REG_0		0x4714
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_5		0x4718
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_6		0x471C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_7		0x4720
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_8		0x4724
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_9		0x4728
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_10		0x472C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_11		0x4730
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_12		0x4734
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_13		0x4738
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_14		0x473C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_15		0x4740
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_16		0x4744
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_17		0x4748
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_18		0x474C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_19		0x4750
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_20		0x4754
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_21		0x4758
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_22		0x475C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_23		0x4760
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_24		0x4764
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_25		0x4768
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_26		0x476C
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_27		0x4770
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_28		0x4774
#define _pon_hsgmii_dig_csr_REG_PON_DEBUG_REGISTER_29		0x4778
#define _pon_hsgmii_dig_csr_REG_PON_INTERRUPT_STS_0		0x4800
#define _pon_hsgmii_dig_csr_REG_PON_INTERRUPT_EN_0		0x4804
#define _pon_hsgmii_dig_csr_REG_PON_INTERRUPT_CLR_0		0x4808
#define _pon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_0		0x480C
#define _pon_hsgmii_dig_csr_REG_PON_INTERRUPT_SET_1		0x4810

#endif


#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_int_los_inv            REG_FLD(1, 30)
#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_los_inv            REG_FLD(1, 29)
#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_ck_en_and_los      REG_FLD(1, 28)
#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_lck2ref_sel        REG_FLD(1, 25)
#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_sw_lcpll_en            REG_FLD(1, 24)
#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_en_inv           REG_FLD(1, 16)
#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_hw_ctrl_mode     REG_FLD(1, 8)
#define SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_force_on         REG_FLD(1, 0)

#define SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_ck_stb_timer     REG_FLD(8, 24)
#define SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_pcw_man_load_timer REG_FLD(8, 16)
#define SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_en_timer         REG_FLD(8, 8)
#define SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_man_pwdb         REG_FLD(1, 0)

#define SS_LCPLL_PWCTL_SETTING_2_FLD_rg_ncpo_ana_msb           REG_FLD(2, 16)
#define SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_mst_en             REG_FLD(1, 8)
#define SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_ck_stb_timer       REG_FLD(8, 0)

#define SS_LCPLL_PWCTL_DBG_SETTING_FLD_rg_lcpll_ledck_div      REG_FLD(4, 0)

#define SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_iso_disable   REG_FLD(1, 24)
#define SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_pwdb          REG_FLD(1, 16)
#define SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_autopw_en          REG_FLD(1, 8)
#define SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_tdc_dig_pwdb            REG_FLD(1, 0)

#define SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg  REG_FLD(1, 24)
#define SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb          REG_FLD(1, 16)
#define SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb_dly      REG_FLD(2, 8)
#define SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_iso_disable_dly REG_FLD(2, 0)

#define SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en   REG_FLD(1, 24)
#define SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en_dly   REG_FLD(2, 16)
#define SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en       REG_FLD(1, 8)
#define SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly REG_FLD(2, 0)

#define SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal                 REG_FLD(1, 24)
#define SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig_dly        REG_FLD(3, 16)
#define SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig            REG_FLD(1, 8)
#define SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en_dly REG_FLD(2, 0)

#define SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_sigdet_cg           REG_FLD(1, 24)
#define SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_en                  REG_FLD(1, 16)
#define SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_offset          REG_FLD(1, 8)
#define SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_bw              REG_FLD(1, 0)

#define SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt                 REG_FLD(4, 28)
#define SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt_clr             REG_FLD(1, 26)
#define SS_LCPLL_TDC_PW_5_FLD_rg_ncpo_start_dly_en             REG_FLD(1, 25)
#define SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_sync_in_mode        REG_FLD(1, 24)
#define SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_autok_tdc               REG_FLD(1, 16)
#define SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_autoen              REG_FLD(1, 8)
#define SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_en_mux              REG_FLD(1, 0)

#define SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_tdc_offset             REG_FLD(11, 16)
#define SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_ki                     REG_FLD(3, 8)
#define SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_pon_rx_cdr_divtdc      REG_FLD(3, 0)

#define SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_tdc_gain               REG_FLD(11, 16)
#define SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_a_tdc                  REG_FLD(4, 8)
#define SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_gpon_sel               REG_FLD(1, 0)

#define SS_LCPLL_TDC_FLT_2_FLD_rg_lcpll_ncpo_value             REG_FLD(31, 0)

#define SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd_disb                 REG_FLD(1, 29)
#define SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd                      REG_FLD(1, 28)
#define SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_tdc_swrst              REG_FLD(1, 24)
#define SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_hold              REG_FLD(1, 16)
#define SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_load              REG_FLD(1, 8)
#define SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_shift             REG_FLD(2, 0)

#define SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_wait_lck2ref     REG_FLD(1, 24)
#define SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_chk_lck2ref      REG_FLD(1, 16)
#define SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdc_rst                REG_FLD(1, 8)
#define SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_rst_dlf          REG_FLD(1, 0)

#define SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_ncpo_chg               REG_FLD(1, 24)
#define SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_autopw_ncpo        REG_FLD(1, 16)
#define SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pos_dly    REG_FLD(3, 8)
#define SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pre_dly    REG_FLD(2, 0)

#define SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_tdc_pcw_sub1en         REG_FLD(1, 24)
#define SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_dlf_mult_en            REG_FLD(1, 16)
#define SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_delay         REG_FLD(2, 8)
#define SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_mask          REG_FLD(5, 0)

#define SS_LCPLL_TDC_FLT_7_FLD_rg_lcpll_tdcck_inv_en           REG_FLD(1, 0)

#define SS_LCPLL_TDC_PCW_1_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_gpon REG_FLD(31, 0)

#define SS_LCPLL_TDC_PCW_2_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_epon REG_FLD(31, 0)

#define SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_dig                 REG_FLD(12, 16)
#define SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_st                  REG_FLD(5, 8)
#define SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_tdcpw_st            REG_FLD(4, 0)

#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_ck_stb     REG_FLD(1, 31)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_txpll1_ddsfbk_en REG_FLD(1, 30)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_offset REG_FLD(1, 29)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_bw     REG_FLD(1, 28)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_en         REG_FLD(1, 27)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_rx_ck_stb      REG_FLD(1, 26)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_en_lck2ref_rst      REG_FLD(1, 25)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pwdb      REG_FLD(1, 24)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_iso_disable REG_FLD(1, 23)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_rstb      REG_FLD(1, 22)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_ncpo_en   REG_FLD(1, 21)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg REG_FLD(1, 20)
#define SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dlf_gain            REG_FLD(20, 0)

#define SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_gain                REG_FLD(11, 16)
#define SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_decode              REG_FLD(3, 13)
#define SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_en                  REG_FLD(1, 12)
#define SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_cal_done            REG_FLD(1, 11)
#define SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_offset              REG_FLD(11, 0)

#define SS_LCPLL_TDC_RO_4_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo  REG_FLD(31, 0)

#define SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_c          REG_FLD(8, 16)
#define SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_d          REG_FLD(8, 8)
#define SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_leadlag    REG_FLD(1, 0)

#define SS_LCPLL_TDC_CTRL_0_FLD_rg_lcpll_ki_hold               REG_FLD(1, 0)

#define SS_LCPLL_0_FLD_tdc_sync_pcw_ncpo_chg                   REG_FLD(1, 8)
#define SS_LCPLL_0_FLD_eee_lcpll_hold                          REG_FLD(1, 0)

#define SS_LCPLL_1_FLD_tdc_sync_pcw_ncpo                       REG_FLD(31, 0)

#define SS_LCPLL_2_FLD_lcpll_top_flagh_en                      REG_FLD(1, 24)
#define SS_LCPLL_2_FLD_lcpll_top_flagl_en                      REG_FLD(1, 16)
#define SS_LCPLL_2_FLD_flagh_sel                               REG_FLD(8, 8)
#define SS_LCPLL_2_FLD_flagl_sel                               REG_FLD(8, 0)

#define TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_rund_new           REG_FLD(8, 8)
#define TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_new                REG_FLD(8, 0)

#define TDC_DLF_MODE_SETTING_FLD_rg_dlf_gain_latch             REG_FLD(1, 31)
#define TDC_DLF_MODE_SETTING_FLD_rg_dlf_mode_sel               REG_FLD(1, 24)
#define TDC_DLF_MODE_SETTING_FLD_ro_dlf_gain_new_org           REG_FLD(20, 0)

#define TDC_HOLD_CTRL_0_FLD_rg_ncpo_hold_en                    REG_FLD(1, 24)
#define TDC_HOLD_CTRL_0_FLD_rg_ncpo_toggle                     REG_FLD(1, 20)
#define TDC_HOLD_CTRL_0_FLD_rg_tdc_los_hold_mux                REG_FLD(1, 16)
#define TDC_HOLD_CTRL_0_FLD_rg_tdc_ncpo_hold_release_time      REG_FLD(16, 0)

#define TDC_HOLD_CTRL_1_FLD_rg_ncpo_up_thred                   REG_FLD(31, 0)

#define TDC_HOLD_CTRL_2_FLD_rg_ncpo_dwn_thred                  REG_FLD(31, 0)

#define TDC_HOLD_STATUS_0_FLD_ro_ncpo_output                   REG_FLD(31, 0)

#define TDC_HOLD_STATUS_1_FLD_ro_ncpo_max_output               REG_FLD(31, 0)

#define TDC_HOLD_STATUS_2_FLD_ro_ncpo_min_output               REG_FLD(31, 0)

#define PON_SERDES_CTRL_0_FLD_rg_tx_data_path_sel              REG_FLD(1, 28)
#define PON_SERDES_CTRL_0_FLD_rg_ben_inv                       REG_FLD(1, 24)
#define PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx           REG_FLD(2, 20)
#define PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx_sel       REG_FLD(1, 19)
#define PON_SERDES_CTRL_0_FLD_rg_pcie_device_mode              REG_FLD(1, 18)
#define PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_rx_sel       REG_FLD(1, 17)
#define PON_SERDES_CTRL_0_FLD_rg_force_tphy_mode               REG_FLD(1, 16)
#define PON_SERDES_CTRL_0_FLD_rg_tphy_mode_set                 REG_FLD(2, 8)
#define PON_SERDES_CTRL_0_FLD_rg_sgmii_auto_init               REG_FLD(1, 7)
#define PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode_force         REG_FLD(1, 6)
#define PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode               REG_FLD(2, 4)
#define PON_SERDES_CTRL_0_FLD_rg_speed_chg                     REG_FLD(1, 1)
#define PON_SERDES_CTRL_0_FLD_rg_phya_auto_init                REG_FLD(1, 0)

#define PON_SERDES_CTRL_1_FLD_rg_rx_pwd                        REG_FLD(1, 9)
#define PON_SERDES_CTRL_1_FLD_rg_rx_pwd_mux                    REG_FLD(1, 8)
#define PON_SERDES_CTRL_1_FLD_rg_tx_disable                    REG_FLD(1, 7)
#define PON_SERDES_CTRL_1_FLD_rg_tx_disable_mux                REG_FLD(1, 6)
#define PON_SERDES_CTRL_1_FLD_rg_phya_pwd                      REG_FLD(1, 5)
#define PON_SERDES_CTRL_1_FLD_rg_phya_pwd_mux                  REG_FLD(1, 4)
#define PON_SERDES_CTRL_1_FLD_rg_freq_det_frc_en               REG_FLD(1, 2)
#define PON_SERDES_CTRL_1_FLD_rg_freq_det_frc                  REG_FLD(1, 1)
#define PON_SERDES_CTRL_1_FLD_rg_cdr_re_calib                  REG_FLD(1, 0)

#define PON_SERDES_CTRL_2_FLD_rg_ben_on_opt                    REG_FLD(1, 24)
#define PON_SERDES_CTRL_2_FLD_rg_epon_benoff_opt               REG_FLD(1, 16)
#define PON_SERDES_CTRL_2_FLD_rg_bypass_int_dly                REG_FLD(1, 12)
#define PON_SERDES_CTRL_2_FLD_rg_tx_div_mode                   REG_FLD(1, 8)
#define PON_SERDES_CTRL_2_FLD_rg_tx_idle_en                    REG_FLD(1, 5)
#define PON_SERDES_CTRL_2_FLD_rg_tx_idle_mux                   REG_FLD(1, 4)
#define PON_SERDES_CTRL_2_FLD_rg_tx_idle_mode                  REG_FLD(1, 3)
#define PON_SERDES_CTRL_2_FLD_rg_tx_idle                       REG_FLD(1, 2)
#define PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_force             REG_FLD(1, 1)
#define PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_cond              REG_FLD(1, 0)

#define PON_SERDES_CTRL_3_FLD_rg_cdr_settle_dly                REG_FLD(11, 16)
#define PON_SERDES_CTRL_3_FLD_rg_cdr_bw_sel_dly                REG_FLD(4, 8)
#define PON_SERDES_CTRL_3_FLD_rg_bias_pwd_to_bg_lpf_en_dly     REG_FLD(5, 0)

#define PON_SERDES_CTRL_4_FLD_rg_freq_meter_sel                REG_FLD(1, 16)
#define PON_SERDES_CTRL_4_FLD_rg_ppm_thres                     REG_FLD(6, 8)
#define PON_SERDES_CTRL_4_FLD_rg_lpf_window_sel                REG_FLD(2, 4)
#define PON_SERDES_CTRL_4_FLD_rg_freq_det_en_wait_dly          REG_FLD(4, 0)

#define PON_SERDES_CTRL_5_FLD_rg_sig_det_wait_dly              REG_FLD(16, 16)
#define PON_SERDES_CTRL_5_FLD_rg_rx_eq_en_h_dly                REG_FLD(13, 0)

#define PON_SERDES_CTRL_6_FLD_rg_rx_saosc_en_h_dly             REG_FLD(6, 16)
#define PON_SERDES_CTRL_6_FLD_rg_rx_pi_cal_en_h_dly            REG_FLD(5, 8)
#define PON_SERDES_CTRL_6_FLD_rg_pll_autok_vco_to_pll_bias_lpf_en_dly REG_FLD(2, 0)

#define PON_SERDES_CTRL_7_FLD_rg_syspll_pwd_to_syspll_ready_dly REG_FLD(4, 28)
#define PON_SERDES_CTRL_7_FLD_rg_sig_det_l_thres               REG_FLD(10, 16)
#define PON_SERDES_CTRL_7_FLD_rg_sig_det_h_thres               REG_FLD(10, 0)

#define PON_SERDES_CTRL_8_FLD_rg_sigdet_out_sel                REG_FLD(2, 16)
#define PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g2 REG_FLD(8, 8)
#define PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g1 REG_FLD(8, 0)

#define PON_SERDES_CTRL_9_FLD_rg_ssusb_cdr_band_fail           REG_FLD(7, 8)
#define PON_SERDES_CTRL_9_FLD_rg_ssusb_pll_band_fail           REG_FLD(7, 0)

#define PON_SERDES_CTRL_10_FLD_ro_bist_run                     REG_FLD(1, 26)
#define PON_SERDES_CTRL_10_FLD_ro_bist_ok                      REG_FLD(1, 25)
#define PON_SERDES_CTRL_10_FLD_ro_bist_err                     REG_FLD(1, 24)
#define PON_SERDES_CTRL_10_FLD_rg_prbs_mode                    REG_FLD(2, 20)
#define PON_SERDES_CTRL_10_FLD_rg_renew_mode                   REG_FLD(1, 19)
#define PON_SERDES_CTRL_10_FLD_rg_prbs_injerr                  REG_FLD(1, 18)
#define PON_SERDES_CTRL_10_FLD_rg_prbs_en                      REG_FLD(1, 17)
#define PON_SERDES_CTRL_10_FLD_rg_prbs_check                   REG_FLD(1, 16)
#define PON_SERDES_CTRL_10_FLD_rg_tx_bit_polarity              REG_FLD(1, 8)
#define PON_SERDES_CTRL_10_FLD_rg_tbi_tx_disable               REG_FLD(1, 7)
#define PON_SERDES_CTRL_10_FLD_rg_tbi_rx_disable               REG_FLD(1, 6)
#define PON_SERDES_CTRL_10_FLD_rg_rx_bit_polarity              REG_FLD(1, 5)
#define PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback_fifo_rst   REG_FLD(1, 4)
#define PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback            REG_FLD(1, 3)
#define PON_SERDES_CTRL_10_FLD_rg_pcs_t2r_loopback             REG_FLD(1, 2)
#define PON_SERDES_CTRL_10_FLD_rg_afifo_winc_serdes            REG_FLD(1, 1)
#define PON_SERDES_CTRL_10_FLD_rg_afifo_rinc_serdes            REG_FLD(1, 0)

#define PON_SERDES_STS_0_FLD_ro_debug_prbs                     REG_FLD(16, 16)
#define PON_SERDES_STS_0_FLD_ro_bist_err_cnt                   REG_FLD(16, 0)

#define PON_SERDES_CTRL_11_FLD_rg_udp_data_31_0                REG_FLD(32, 0)

#define PON_SERDES_CTRL_12_FLD_rg_udp_data_63_32               REG_FLD(32, 0)

#define PON_SERDES_CTRL_13_FLD_rg_udp_data_79_64               REG_FLD(16, 0)

#define PON_SERDES_CTRL_14_FLD_rg_afifo_w_thres_serdes         REG_FLD(5, 8)
#define PON_SERDES_CTRL_14_FLD_rg_afifo_r_thres_serdes         REG_FLD(5, 0)

#define PON_SERDES_STS_1_FLD_ro_debug_afifo_serdes             REG_FLD(18, 0)

#define PON_SERDES_CTRL_15_FLD_rg_ad_ben_out_clr               REG_FLD(1, 30)
#define PON_SERDES_CTRL_15_FLD_rg_ben_force_value              REG_FLD(1, 29)
#define PON_SERDES_CTRL_15_FLD_rg_ben_force_mode               REG_FLD(1, 28)
#define PON_SERDES_CTRL_15_FLD_rg_rx_path_rdy_mux              REG_FLD(1, 24)
#define PON_SERDES_CTRL_15_FLD_rg_data_rx_inv                  REG_FLD(1, 20)
#define PON_SERDES_CTRL_15_FLD_rg_rx_cid_path_en               REG_FLD(1, 16)
#define PON_SERDES_CTRL_15_FLD_rg_rx_prbs15_path_en            REG_FLD(1, 14)
#define PON_SERDES_CTRL_15_FLD_rg_rx_prbs9_path_en             REG_FLD(1, 13)
#define PON_SERDES_CTRL_15_FLD_rg_rx_prbs31_path_en            REG_FLD(1, 12)
#define PON_SERDES_CTRL_15_FLD_rg_rx_prbs23_path_en            REG_FLD(1, 8)
#define PON_SERDES_CTRL_15_FLD_rg_rx_prbs7_path_en             REG_FLD(1, 4)
#define PON_SERDES_CTRL_15_FLD_rg_rx_prbs_path_sel             REG_FLD(1, 0)

#define PON_SERDES_CTRL_16_FLD_rg_tx_end_dly_sel               REG_FLD(1, 10)
#define PON_SERDES_CTRL_16_FLD_rg_tx_start_dly_sel             REG_FLD(1, 9)
#define PON_SERDES_CTRL_16_FLD_rg_tx_bit_dly_mode              REG_FLD(1, 8)
#define PON_SERDES_CTRL_16_FLD_rg_txbit_dly                    REG_FLD(4, 4)
#define PON_SERDES_CTRL_16_FLD_rg_bit_delay_rg_mode            REG_FLD(1, 2)
#define PON_SERDES_CTRL_16_FLD_rg_tx_ben_end_dly_mode          REG_FLD(1, 1)
#define PON_SERDES_CTRL_16_FLD_rg_tx_ben_start_dly_mode        REG_FLD(1, 0)

#define PON_SERDES_CTRL_17_FLD_rg_tx_bit_end_dly               REG_FLD(8, 8)
#define PON_SERDES_CTRL_17_FLD_rg_tx_bit_start_dly             REG_FLD(8, 0)

#define PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done_sel         REG_FLD(1, 25)
#define PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done             REG_FLD(1, 24)
#define PON_SERDES_CTRL_18_FLD_rg_rxcal_ne_cnt                 REG_FLD(5, 16)
#define PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done_sel         REG_FLD(1, 12)
#define PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done             REG_FLD(1, 8)
#define PON_SERDES_CTRL_18_FLD_rg_l2ref_ne_cnt                 REG_FLD(5, 0)

#define PON_SERDES_STS_2_FLD_ro_rxcal_done                     REG_FLD(1, 1)
#define PON_SERDES_STS_2_FLD_ro_l2ref_done                     REG_FLD(1, 0)

#define PON_SERDES_CTRL_19_FLD_rg_20b_idle_code                REG_FLD(20, 0)

#define PON_SERDES_CTRL_20_FLD_rg_lpf_en_to_ssc_dly_cnt        REG_FLD(16, 16)
#define PON_SERDES_CTRL_20_FLD_rg_da_pll_en_dly_cnt            REG_FLD(8, 8)
#define PON_SERDES_CTRL_20_FLD_rg_da_pll_en                    REG_FLD(1, 2)
#define PON_SERDES_CTRL_20_FLD_rg_force_da_pll_en              REG_FLD(1, 1)
#define PON_SERDES_CTRL_20_FLD_rg_da_pll_en_mux                REG_FLD(1, 0)

#define PON_SERDES_CTRL_21_FLD_rg_pll_vco_cplt                 REG_FLD(1, 17)
#define PON_SERDES_CTRL_21_FLD_rg_force_pll_vco_cplt           REG_FLD(1, 16)
#define PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd                   REG_FLD(1, 9)
#define PON_SERDES_CTRL_21_FLD_rg_force_da_cdr_pwd             REG_FLD(1, 8)
#define PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd_dly_cnt           REG_FLD(8, 0)

#define PON_SERDES_CTRL_22_FLD_rg_vcocal_fail                  REG_FLD(1, 12)
#define PON_SERDES_CTRL_22_FLD_rg_vcocal_dly_cnt               REG_FLD(4, 8)
#define PON_SERDES_CTRL_22_FLD_rg_vcocal_state                 REG_FLD(8, 0)

#define PON_SYS_CTRL_0_FLD_rg_xfi_rx_pma_clk_sel               REG_FLD(1, 31)
#define PON_SYS_CTRL_0_FLD_rg_baser_tx_pma_clk_div_sel         REG_FLD(1, 30)
#define PON_SYS_CTRL_0_FLD_rg_tx_pma_clk_sel                   REG_FLD(1, 29)
#define PON_SYS_CTRL_0_FLD_rg_hg_tx_pma_clk_div_sel            REG_FLD(1, 28)
#define PON_SYS_CTRL_0_FLD_rg_pon_tx_pma_clk_div_sel           REG_FLD(1, 27)
#define PON_SYS_CTRL_0_FLD_rg_rx_pma_clk_sel                   REG_FLD(1, 26)
#define PON_SYS_CTRL_0_FLD_rg_hg_rx_pma_clk_div_sel            REG_FLD(1, 25)
#define PON_SYS_CTRL_0_FLD_rg_pon_rx_pma_clk_div_sel           REG_FLD(1, 24)
#define PON_SYS_CTRL_0_FLD_rg_baser_rx_clk_inv                 REG_FLD(1, 20)
#define PON_SYS_CTRL_0_FLD_rg_baser_tx_clk_inv                 REG_FLD(1, 19)
#define PON_SYS_CTRL_0_FLD_rg_tx_clk_inv                       REG_FLD(1, 18)
#define PON_SYS_CTRL_0_FLD_rg_rx_clk_inv                       REG_FLD(1, 17)
#define PON_SYS_CTRL_0_FLD_rg_txben_clk_inv                    REG_FLD(1, 16)
#define PON_SYS_CTRL_0_FLD_rg_rx_short_ck_inv                  REG_FLD(1, 13)
#define PON_SYS_CTRL_0_FLD_rg_tx_short_ck_inv                  REG_FLD(1, 12)
#define PON_SYS_CTRL_0_FLD_rg_baser_rxck_en                    REG_FLD(1, 9)
#define PON_SYS_CTRL_0_FLD_rg_baser_txck_en                    REG_FLD(1, 8)
#define PON_SYS_CTRL_0_FLD_rg_hg_rx500m_ck_en                  REG_FLD(1, 7)
#define PON_SYS_CTRL_0_FLD_rg_hg_tx250m_ck_en                  REG_FLD(1, 6)
#define PON_SYS_CTRL_0_FLD_rg_pon_rx500m_ck_en                 REG_FLD(1, 5)
#define PON_SYS_CTRL_0_FLD_rg_pon_tx250m_ck_en                 REG_FLD(1, 4)
#define PON_SYS_CTRL_0_FLD_rg_force_ge_mode                    REG_FLD(1, 3)
#define PON_SYS_CTRL_0_FLD_rg_force_map_mode                   REG_FLD(1, 2)
#define PON_SYS_CTRL_0_FLD_rg_force_map_sel                    REG_FLD(1, 1)

#define PON_SYS_CTRL_1_FLD_rg_div_rst                          REG_FLD(1, 31)
#define PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_pma_fifo_rst_n       REG_FLD(1, 30)
#define PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_fifo_rst_n           REG_FLD(1, 29)
#define PON_SYS_CTRL_1_FLD_rg_rate_adp_tx_fifo_rst_n           REG_FLD(1, 28)
#define PON_SYS_CTRL_1_FLD_rg_all_rx_rst_n                     REG_FLD(1, 27)
#define PON_SYS_CTRL_1_FLD_rg_all_tx_rst_n                     REG_FLD(1, 26)
#define PON_SYS_CTRL_1_FLD_rg_baser_rx_reset_n                 REG_FLD(1, 25)
#define PON_SYS_CTRL_1_FLD_rg_baser_tx_reset_n                 REG_FLD(1, 24)
#define PON_SYS_CTRL_1_FLD_rg_rx_fifo_rst                      REG_FLD(1, 20)
#define PON_SYS_CTRL_1_FLD_rg_force_bg_tx_value                REG_FLD(1, 17)
#define PON_SYS_CTRL_1_FLD_rg_force_bg_tx_mode                 REG_FLD(1, 16)
#define PON_SYS_CTRL_1_FLD_rg_trans_rx_data_inv                REG_FLD(1, 15)
#define PON_SYS_CTRL_1_FLD_rg_bridge_rx_reset_n                REG_FLD(1, 14)
#define PON_SYS_CTRL_1_FLD_rg_bridge_tx_reset_n                REG_FLD(1, 13)
#define PON_SYS_CTRL_1_FLD_rg_eq_rx250m_div_reset_n            REG_FLD(1, 12)
#define PON_SYS_CTRL_1_FLD_rg_eq_rx250m_reset_n                REG_FLD(1, 11)
#define PON_SYS_CTRL_1_FLD_rg_tx250m_short_reset_n             REG_FLD(1, 10)
#define PON_SYS_CTRL_1_FLD_rg_tx250m_div_reset_n               REG_FLD(1, 9)
#define PON_SYS_CTRL_1_FLD_rg_tx_reset_n                       REG_FLD(1, 8)
#define PON_SYS_CTRL_1_FLD_rg_rx_reset_n                       REG_FLD(1, 7)
#define PON_SYS_CTRL_1_FLD_rg_ref_reset_n                      REG_FLD(1, 6)
#define PON_SYS_CTRL_1_FLD_rg_pcstx_reset_n                    REG_FLD(1, 5)
#define PON_SYS_CTRL_1_FLD_rg_pcsrx_reset_n                    REG_FLD(1, 4)
#define PON_SYS_CTRL_1_FLD_rg_pcstxrx_reset_n                  REG_FLD(1, 3)
#define PON_SYS_CTRL_1_FLD_rg_mactx_reset_n                    REG_FLD(1, 2)
#define PON_SYS_CTRL_1_FLD_rg_macrx_reset_n                    REG_FLD(1, 1)
#define PON_SYS_CTRL_1_FLD_rg_mactxrx_reset_n                  REG_FLD(1, 0)

#define PON_SYS_CTRL_2_FLD_rg_1us_40m_cnt                      REG_FLD(8, 24)
#define PON_SYS_CTRL_2_FLD_rg_1us_25m_cnt                      REG_FLD(8, 16)
#define PON_SYS_CTRL_2_FLD_rg_pon_mbist_slow_mode_en           REG_FLD(1, 9)
#define PON_SYS_CTRL_2_FLD_rg_pon_mbist_mode_en                REG_FLD(1, 8)
#define PON_SYS_CTRL_2_FLD_rg_xtal_mode                        REG_FLD(1, 5)
#define PON_SYS_CTRL_2_FLD_rg_xtal_is_25m                      REG_FLD(1, 4)
#define PON_SYS_CTRL_2_FLD_rg_pon_probe_sel                    REG_FLD(2, 0)

#define PON_CK_CTRL_0_FLD_rg_rs_ck_cnt                         REG_FLD(1, 10)
#define PON_CK_CTRL_0_FLD_rg_rs_ck_sel                         REG_FLD(1, 9)
#define PON_CK_CTRL_0_FLD_rg_rs_ck_mux                         REG_FLD(1, 8)
#define PON_CK_CTRL_0_FLD_rg_baser_rxck_mode                   REG_FLD(2, 2)
#define PON_CK_CTRL_0_FLD_rg_baser_txck_mode                   REG_FLD(2, 0)

#define HG_RST_CTRL_0_FLD_rg_hsgmii_mac_rxck_free_sel          REG_FLD(1, 17)
#define HG_RST_CTRL_0_FLD_rg_hsgmii_mac_txck_free_sel          REG_FLD(1, 16)
#define HG_RST_CTRL_0_FLD_rg_xfi_pcs_bg_lb_fifo_rst_n          REG_FLD(1, 14)
#define HG_RST_CTRL_0_FLD_rg_xfi_pcs_tx_gearbox_fifo_rst_n     REG_FLD(1, 13)
#define HG_RST_CTRL_0_FLD_rg_xfi_pcs_rx_gearbox_fifo_rst_n     REG_FLD(1, 12)
#define HG_RST_CTRL_0_FLD_rg_bypass_tx_re_en                   REG_FLD(1, 8)
#define HG_RST_CTRL_0_FLD_rg_hg_mac_rx_reset_n                 REG_FLD(1, 5)
#define HG_RST_CTRL_0_FLD_rg_hg_mac_tx_reset_n                 REG_FLD(1, 4)
#define HG_RST_CTRL_0_FLD_rg_hg_pcs_rx_reset_n                 REG_FLD(1, 3)
#define HG_RST_CTRL_0_FLD_rg_hg_pcs_tx_reset_n                 REG_FLD(1, 2)
#define HG_RST_CTRL_0_FLD_rg_hg_pma_rx_reset_n                 REG_FLD(1, 1)
#define HG_RST_CTRL_0_FLD_rg_hg_pma_tx_reset_n                 REG_FLD(1, 0)

#define HG_MODE_CTRL_0_FLD_rg_fpga_tx_swap                     REG_FLD(1, 24)
#define HG_MODE_CTRL_0_FLD_rg_fpga_mode_ctrl                   REG_FLD(5, 16)
#define HG_MODE_CTRL_0_FLD_rg_ip_reset_mode                    REG_FLD(1, 8)
#define HG_MODE_CTRL_0_FLD_rg_sgmii_mode                       REG_FLD(2, 4)
#define HG_MODE_CTRL_0_FLD_rg_gpon_sym_mode                    REG_FLD(1, 1)
#define HG_MODE_CTRL_0_FLD_rg_serdes_mode                      REG_FLD(1, 0)

#define HG_RG_CTRL_0_FLD_rg_eq_step1                           REG_FLD(2, 8)
#define HG_RG_CTRL_0_FLD_rg_tx_dem                             REG_FLD(2, 4)
#define HG_RG_CTRL_0_FLD_rg_ln0_rx_dac_pwd                     REG_FLD(1, 1)
#define HG_RG_CTRL_0_FLD_rg_rx_dac_pwd_mux                     REG_FLD(1, 0)

#define PON_STS_0_FLD_ro_baser_mode                            REG_FLD(1, 5)
#define PON_STS_0_FLD_ro_hsgmii_mode                           REG_FLD(1, 4)
#define PON_STS_0_FLD_ro_sgmii_mode                            REG_FLD(1, 3)
#define PON_STS_0_FLD_ro_epon_mode                             REG_FLD(1, 2)
#define PON_STS_0_FLD_ro_gpon_tmode                            REG_FLD(1, 1)
#define PON_STS_0_FLD_ro_gpon_mode                             REG_FLD(1, 0)

#define PON_OSR_SEL_CTRL_FLD_rg_tx_eqd_mode                    REG_FLD(1, 16)
#define PON_OSR_SEL_CTRL_FLD_rg_rx_osr_sel                     REG_FLD(1, 4)
#define PON_OSR_SEL_CTRL_FLD_rg_tx_osr_sel                     REG_FLD(2, 0)

#define CDR_CTRL_0_FLD_rg_bic_ltr_mode_1                       REG_FLD(4, 20)
#define CDR_CTRL_0_FLD_rg_bic_ltr_mode_0                       REG_FLD(4, 16)
#define CDR_CTRL_0_FLD_rg_bir_ltr_mode_1                       REG_FLD(5, 8)
#define CDR_CTRL_0_FLD_rg_bir_ltr_mode_0                       REG_FLD(5, 0)

#define CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_1                     REG_FLD(1, 25)
#define CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_0                     REG_FLD(1, 24)
#define CDR_CTRL_1_FLD_rg_br_ltr_mode_1                        REG_FLD(3, 20)
#define CDR_CTRL_1_FLD_rg_br_ltr_mode_0                        REG_FLD(3, 16)
#define CDR_CTRL_1_FLD_rg_bc_ltr_mode_1                        REG_FLD(5, 8)
#define CDR_CTRL_1_FLD_rg_bc_ltr_mode_0                        REG_FLD(5, 0)

#define CDR_CTRL_2_FLD_rg_bic_ltd0_mode_1                      REG_FLD(4, 20)
#define CDR_CTRL_2_FLD_rg_bic_ltd0_mode_0                      REG_FLD(4, 16)
#define CDR_CTRL_2_FLD_rg_bir_ltd0_mode_1                      REG_FLD(5, 8)
#define CDR_CTRL_2_FLD_rg_bir_ltd0_mode_0                      REG_FLD(5, 0)

#define CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_1                    REG_FLD(1, 25)
#define CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_0                    REG_FLD(1, 24)
#define CDR_CTRL_3_FLD_rg_br_ltd0_mode_1                       REG_FLD(3, 20)
#define CDR_CTRL_3_FLD_rg_br_ltd0_mode_0                       REG_FLD(3, 16)
#define CDR_CTRL_3_FLD_rg_bc_ltd0_mode_1                       REG_FLD(5, 8)
#define CDR_CTRL_3_FLD_rg_bc_ltd0_mode_0                       REG_FLD(5, 0)

#define CDR_CTRL_4_FLD_rg_bic_ltd1_mode_1                      REG_FLD(4, 20)
#define CDR_CTRL_4_FLD_rg_bic_ltd1_mode_0                      REG_FLD(4, 16)
#define CDR_CTRL_4_FLD_rg_bir_ltd1_mode_1                      REG_FLD(5, 8)
#define CDR_CTRL_4_FLD_rg_bir_ltd1_mode_0                      REG_FLD(5, 0)

#define CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_1                    REG_FLD(1, 25)
#define CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_0                    REG_FLD(1, 24)
#define CDR_CTRL_5_FLD_rg_br_ltd1_mode_1                       REG_FLD(3, 20)
#define CDR_CTRL_5_FLD_rg_br_ltd1_mode_0                       REG_FLD(3, 16)
#define CDR_CTRL_5_FLD_rg_bc_ltd1_mode_1                       REG_FLD(5, 8)
#define CDR_CTRL_5_FLD_rg_bc_ltd1_mode_0                       REG_FLD(5, 0)

#define TX_CTRL_0_FLD_rg_tx_ben_end_dly_sel                    REG_FLD(3, 28)
#define TX_CTRL_0_FLD_rg_tx_ben_start_dly_sel                  REG_FLD(3, 24)
#define TX_CTRL_0_FLD_rg_tx_ben_exten_ftune                    REG_FLD(8, 16)
#define TX_CTRL_0_FLD_rg_tx_dly_ben_ftune                      REG_FLD(7, 8)
#define TX_CTRL_0_FLD_rg_tx_dly_data_ftune                     REG_FLD(7, 0)

#define TX_CTRL_1_FLD_rg_rx_non_reverse_epon                   REG_FLD(1, 11)
#define TX_CTRL_1_FLD_rg_tx_non_reverse_epon                   REG_FLD(1, 10)
#define TX_CTRL_1_FLD_rg_rx_non_reverse_gpon                   REG_FLD(1, 9)
#define TX_CTRL_1_FLD_rg_tx_non_reverse_gpon                   REG_FLD(1, 8)
#define TX_CTRL_1_FLD_rg_force_rx_pon_sel                      REG_FLD(1, 5)
#define TX_CTRL_1_FLD_rg_force_rx_pon_mode                     REG_FLD(1, 4)
#define TX_CTRL_1_FLD_rg_force_tx_gpon_sel                     REG_FLD(1, 3)
#define TX_CTRL_1_FLD_rg_force_tx_gpon_mode                    REG_FLD(1, 2)
#define TX_CTRL_1_FLD_rg_force_tx_epon_sel                     REG_FLD(1, 1)
#define TX_CTRL_1_FLD_rg_force_tx_epon_mode                    REG_FLD(1, 0)

#define TX_CTRL_2_FLD_rg_txben_bdly_mode                       REG_FLD(1, 29)
#define TX_CTRL_2_FLD_rg_txdata_bdly_mode                      REG_FLD(1, 28)
#define TX_CTRL_2_FLD_rg_bit_delay_mode_sel                    REG_FLD(1, 27)
#define TX_CTRL_2_FLD_rg_benoff_cnt_max                        REG_FLD(3, 15)
#define TX_CTRL_2_FLD_rg_txben_pre_post_zero_en                REG_FLD(1, 8)
#define TX_CTRL_2_FLD_rg_hsdata_en_sel                         REG_FLD(3, 4)
#define TX_CTRL_2_FLD_rg_outben_data_mode                      REG_FLD(3, 0)

#define TX_CTRL_3_FLD_rg_bist_prbs_update                      REG_FLD(1, 31)
#define TX_CTRL_3_FLD_rg_bist_prbs_init_seed_benoff            REG_FLD(31, 0)

#define TX_CTRL_4_FLD_rg_benoff_d0                             REG_FLD(32, 0)

#define TX_CTRL_5_FLD_rg_benoff_d1                             REG_FLD(32, 0)

#define TX_CTRL_6_FLD_rg_benoff_d2                             REG_FLD(32, 0)

#define TX_CTRL_7_FLD_rg_benoff_d3                             REG_FLD(32, 0)

#define FRQ_CTRL_0_FLD_rg_hg_rx_frq_ppm_thres                  REG_FLD(6, 24)
#define FRQ_CTRL_0_FLD_rg_hg_tx_frq_ppm_thres                  REG_FLD(6, 16)
#define FRQ_CTRL_0_FLD_rg_pon_rx_frq_ppm_thres                 REG_FLD(6, 8)
#define FRQ_CTRL_0_FLD_rg_pon_tx_frq_ppm_thres                 REG_FLD(6, 0)

#define FRQ_CTRL_1_FLD_rg_baser_rx_frq_ppm_thres               REG_FLD(6, 8)
#define FRQ_CTRL_1_FLD_rg_baser_tx_frq_ppm_thres               REG_FLD(6, 0)

#define FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_mux                REG_FLD(1, 13)
#define FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_mux                REG_FLD(1, 12)
#define FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_mux                   REG_FLD(1, 11)
#define FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_mux                   REG_FLD(1, 10)
#define FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_mux                  REG_FLD(1, 9)
#define FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_mux                  REG_FLD(1, 8)
#define FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_en                 REG_FLD(1, 5)
#define FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_en                 REG_FLD(1, 4)
#define FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_en                    REG_FLD(1, 3)
#define FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_en                    REG_FLD(1, 2)
#define FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_en                   REG_FLD(1, 1)
#define FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_en                   REG_FLD(1, 0)

#define FRQ_CTRL_3_FLD_rg_pon_rx_freq_cnt                      REG_FLD(12, 16)
#define FRQ_CTRL_3_FLD_rg_pon_tx_freq_cnt                      REG_FLD(12, 0)

#define FRQ_CTRL_4_FLD_rg_hg_rx_freq_cnt                       REG_FLD(12, 16)
#define FRQ_CTRL_4_FLD_rg_hg_tx_freq_cnt                       REG_FLD(12, 0)

#define FRQ_CTRL_5_FLD_rg_baser_rx_freq_cnt                    REG_FLD(12, 16)
#define FRQ_CTRL_5_FLD_rg_baser_tx_freq_cnt                    REG_FLD(12, 0)

#define RX_FREQDET_0_FLD_rg_rx_pll_unlock_cyclecnt             REG_FLD(16, 16)
#define RX_FREQDET_0_FLD_rg_rx_pll_lock_cyclecnt               REG_FLD(16, 0)

#define RX_FREQDET_1_FLD_rg_rx_pll_lock_target_end             REG_FLD(16, 16)
#define RX_FREQDET_1_FLD_rg_rx_pll_lock_target_beg             REG_FLD(16, 0)

#define RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_end           REG_FLD(16, 16)
#define RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_beg           REG_FLD(16, 0)

#define RX_FREQDET_3_FLD_rg_force_rx_freq_det_en               REG_FLD(1, 24)
#define RX_FREQDET_3_FLD_rg_rx_pll_wait_100us                  REG_FLD(8, 16)
#define RX_FREQDET_3_FLD_rg_rx_pll_unlockth                    REG_FLD(4, 12)
#define RX_FREQDET_3_FLD_rg_rx_pll_lock_lockth                 REG_FLD(4, 8)
#define RX_FREQDET_3_FLD_rg_rx_pll_lock_cnt_clear              REG_FLD(1, 4)
#define RX_FREQDET_3_FLD_rg_rx_pll_freqlock_det_en             REG_FLD(3, 0)

#define RX_FREQDET_STS_0_FLD_ro_rx_prb_out_freqdet             REG_FLD(8, 24)
#define RX_FREQDET_STS_0_FLD_ro_rx_state_freqdet               REG_FLD(2, 22)
#define RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock_inner             REG_FLD(1, 21)
#define RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock                   REG_FLD(1, 20)
#define RX_FREQDET_STS_0_FLD_ro_rx_lslock_cnt                  REG_FLD(4, 16)
#define RX_FREQDET_STS_0_FLD_ro_rx_fl_out                      REG_FLD(16, 0)

#define TX_CTRL_8_FLD_rg_tx_test_mode                          REG_FLD(1, 24)
#define TX_CTRL_8_FLD_rg_tx_test_pattern                       REG_FLD(20, 0)

#define TX_CTRL_9_FLD_rg_ncpo_los_cnt                          REG_FLD(8, 8)
#define TX_CTRL_9_FLD_rg_ncpo_los_hold_en                      REG_FLD(1, 7)
#define TX_CTRL_9_FLD_rg_los_function_sel                      REG_FLD(2, 4)
#define TX_CTRL_9_FLD_rg_ncpo_los_pro_en                       REG_FLD(1, 2)
#define TX_CTRL_9_FLD_rg_ncpo_los_inv                          REG_FLD(1, 1)
#define TX_CTRL_9_FLD_rg_ncpo_lof_en                           REG_FLD(1, 0)

#define RX_FREQDET_DET_CTRL_0_FLD_rg_rx_cdr_freq_chk_cycle     REG_FLD(20, 0)

#define RX_FREQDET_DET_CTRL_1_FLD_rg_rx_cdr_freq_golden_cycle  REG_FLD(20, 0)

#define RX_FREQDET_DET_CTRL_2_FLD_rg_rx_cdr_freq_tolerate_cycle REG_FLD(20, 0)

#define RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_unlck_cnt     REG_FLD(8, 24)
#define RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_state         REG_FLD(4, 20)
#define RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_lock          REG_FLD(1, 16)
#define RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_relck_num     REG_FLD(4, 4)
#define RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_clear_unlck_cnt REG_FLD(1, 1)
#define RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_det_en        REG_FLD(1, 0)

#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_lpf_en          REG_FLD(1, 23)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_lpf_en          REG_FLD(1, 22)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_bias_pwd                 REG_FLD(1, 21)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_bias_pwd                 REG_FLD(1, 20)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_biasimr_pwd              REG_FLD(1, 19)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_biasimr_pwd              REG_FLD(1, 18)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_bg_lpf_en                REG_FLD(1, 17)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_bg_lpf_en                REG_FLD(1, 16)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_pwd             REG_FLD(1, 15)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_pwd             REG_FLD(1, 14)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_xtal_rx_pwd              REG_FLD(1, 13)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_xtal_rx_pwd              REG_FLD(1, 12)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_pwd               REG_FLD(1, 11)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_pwd               REG_FLD(1, 10)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_pwr_on        REG_FLD(1, 9)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_pwr_on        REG_FLD(1, 8)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_iso_en        REG_FLD(1, 7)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_iso_en        REG_FLD(1, 6)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_pwd                  REG_FLD(1, 5)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_pwd                  REG_FLD(1, 4)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_autok_vco            REG_FLD(1, 3)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_autok_vco            REG_FLD(1, 2)
#define PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_ssc_en               REG_FLD(1, 1)
#define PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_ssc_en               REG_FLD(1, 0)

#define PON_DA_MUX_CTRL_1_FLD_rg_c_da_sigdet_cal_offset        REG_FLD(5, 8)
#define PON_DA_MUX_CTRL_1_FLD_rg_f_da_sigdet_cal_offset        REG_FLD(5, 0)

#define PON_DA_MUX_CTRL_2_FLD_rg_c_da_cdr_lpf_lck2ref          REG_FLD(1, 1)
#define PON_DA_MUX_CTRL_2_FLD_rg_f_da_cdr_lpf_lck2ref          REG_FLD(1, 0)

#define PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_sel               REG_FLD(1, 1)
#define PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_mux               REG_FLD(1, 0)

#define PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_pro             REG_FLD(1, 2)
#define PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_en              REG_FLD(1, 1)
#define PON_DA_MUX_CTRL_4_FLD_rg_rx_sigdet_en                  REG_FLD(1, 0)

#define PON_DA_MUX_CTRL_5_FLD_rg_disb_da_pon_tx_data           REG_FLD(1, 24)
#define PON_DA_MUX_CTRL_5_FLD_rg_force_da_pon_tx_data          REG_FLD(20, 0)

#define PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxpcsmodesel     REG_FLD(1, 3)
#define PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txpcsmodesel     REG_FLD(1, 2)
#define PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxmapmodesel     REG_FLD(1, 1)
#define PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txmapmodesel     REG_FLD(1, 0)

#define CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_rxck              REG_FLD(1, 6)
#define CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_txck              REG_FLD(1, 5)
#define CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_rx500m_ck            REG_FLD(1, 4)
#define CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_rx500m_ck           REG_FLD(1, 3)
#define CLK_GEN_FORCE_CTRL_0_FLD_rg_or_sh_tx250m_ck            REG_FLD(1, 2)
#define CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_tx250m_ck            REG_FLD(1, 1)
#define CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_tx250m_ck           REG_FLD(1, 0)

#define ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_value          REG_FLD(1, 1)
#define ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_mode           REG_FLD(1, 0)

#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_RST             REG_FLD(1, 25)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_EN              REG_FLD(1, 24)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_FR_MODE             REG_FLD(1, 16)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_FORCE_RX_FR_MODE       REG_FLD(1, 15)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_TRI_DET_EN          REG_FLD(1, 14)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_STOPTIME            REG_FLD(1, 13)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_RX500M_CK_SEL       REG_FLD(1, 12)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_PILPO_ROUT          REG_FLD(1, 11)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_GATED_RXD_B         REG_FLD(1, 10)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_MON_EN          REG_FLD(1, 9)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_CNT_EN          REG_FLD(1, 8)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFE_TOG             REG_FLD(1, 7)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEYEN_STOP_DIS     REG_FLD(1, 6)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_RST            REG_FLD(1, 5)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_EN             REG_FLD(1, 4)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_DIS            REG_FLD(1, 3)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEXEN_SEL          REG_FLD(1, 2)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_CHK_EYE_H           REG_FLD(1, 1)
#define PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_ALGRTHM_SEL         REG_FLD(1, 0)

#define PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1LOS_LFI          REG_FLD(7, 24)
#define PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1HOS_LFI          REG_FLD(7, 16)
#define PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0LOS_LFI          REG_FLD(7, 8)
#define PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0HOS_LFI          REG_FLD(7, 0)

#define PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE1OS_LFI         REG_FLD(7, 24)
#define PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE0OS_LFI         REG_FLD(7, 16)
#define PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE1OS_LFI           REG_FLD(7, 8)
#define PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE0OS_LFI           REG_FLD(7, 0)

#define PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHL_LFI            REG_FLD(7, 24)
#define PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHLOS_LFI          REG_FLD(7, 16)
#define PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEYEN_DUR          REG_FLD(3, 8)
#define PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEX_LF_SEL         REG_FLD(3, 4)
#define PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEXEN_DUR          REG_FLD(3, 0)

#define PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN3       REG_FLD(4, 28)
#define PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN2       REG_FLD(4, 24)
#define PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN1       REG_FLD(4, 16)
#define PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQOS_LFI          REG_FLD(5, 8)
#define PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DHHL_LF_SEL         REG_FLD(3, 0)

#define PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DSAOS_LF_SEL        REG_FLD(3, 28)
#define PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LF_SEL         REG_FLD(3, 24)
#define PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LFI            REG_FLD(7, 16)
#define PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHLOS_LFI          REG_FLD(7, 8)
#define PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LF_SEL         REG_FLD(3, 4)
#define PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN4       REG_FLD(4, 0)

#define PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE1_Y              REG_FLD(7, 8)
#define PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE0_Y              REG_FLD(7, 0)

#define PON_RXFEDIG_CTRL_7_FLD_RG_SSUSB_EQ_EYE_CNT             REG_FLD(20, 0)

#define PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_LEQMAX              REG_FLD(4, 28)
#define PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_ISIFLAG_SEL         REG_FLD(2, 24)
#define PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_XOFFSET         REG_FLD(7, 16)
#define PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_MASK            REG_FLD(10, 0)

#define PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_PI0_INI             REG_FLD(7, 24)
#define PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_MON_SEL             REG_FLD(5, 16)
#define PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_STOP_TO         REG_FLD(2, 8)
#define PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_SHIFT           REG_FLD(3, 4)
#define PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQOSC_DLYCNT       REG_FLD(3, 0)

#define PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI_KPGAIN          REG_FLD(3, 24)
#define PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PIEYE_INI          REG_FLD(7, 16)
#define PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90_INI           REG_FLD(7, 8)
#define PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90CK_SEL         REG_FLD(2, 0)

#define PON_RXFEDIG_CTRL_11_FLD_RG_SSUSB_EQ_RESERVED           REG_FLD(32, 0)

#define PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV                REG_FLD(16, 16)
#define PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV_1              REG_FLD(16, 0)

#define PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_STARTTIME          REG_FLD(2, 24)
#define PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SIGDET             REG_FLD(7, 16)
#define PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT1            REG_FLD(6, 8)
#define PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT0            REG_FLD(6, 0)

#define PON_RXFEDIG_CTRL_14_FLD_RG_SSUSB_EQ_TRI_DET_TH         REG_FLD(7, 0)

#define PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_LEQ_STOP            REG_FLD(1, 1)
#define PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_EYE_CNT_RDY         REG_FLD(1, 0)

#define PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1L               REG_FLD(7, 24)
#define PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1H               REG_FLD(7, 16)
#define PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0L               REG_FLD(7, 8)
#define PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0H               REG_FLD(7, 0)

#define PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE1              REG_FLD(7, 24)
#define PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE0              REG_FLD(7, 16)
#define PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE1                REG_FLD(7, 8)
#define PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE0                REG_FLD(7, 0)

#define PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLHL               REG_FLD(7, 24)
#define PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQOS             REG_FLD(5, 16)
#define PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQ               REG_FLD(4, 8)
#define PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCHHL               REG_FLD(7, 0)

#define PON_RXFEDIG_STS_4_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0 REG_FLD(20, 0)

#define PON_RXFEDIG_STS_5_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1 REG_FLD(20, 0)

#define PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_STATUS              REG_FLD(8, 16)
#define PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_PILPO               REG_FLD(7, 0)

#define PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_RX_SD_OUT              REG_FLD(1, 8)
#define PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_EQ_PROBE_OUT           REG_FLD(8, 0)

#define PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IC                      REG_FLD(4, 28)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BR                      REG_FLD(3, 24)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IR                      REG_FLD(4, 20)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BP                      REG_FLD(4, 16)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_LOAD_EN                 REG_FLD(1, 15)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_FBKSEL                  REG_FLD(2, 12)
#define PON_DA_CTRL_0_FLD_RG_PCIE_CKDET_EN                     REG_FLD(1, 8)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_SCAN_MODE                   REG_FLD(1, 7)
#define PON_DA_CTRL_0_FLD_RG_PCIE_CLKRX_EN                     REG_FLD(1, 6)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_XTAL_EXT_EN                 REG_FLD(2, 4)
#define PON_DA_CTRL_0_FLD_RG_PCIE_DEVICE                       REG_FLD(1, 2)
#define PON_DA_CTRL_0_FLD_RG_PCIE_REFCK_DIV4                   REG_FLD(1, 1)
#define PON_DA_CTRL_0_FLD_RG_SSUSB_RX_IMPCALIB_EN              REG_FLD(1, 0)

#define PON_DA_CTRL_1_FLD_RG_SSUSB_CDR_BPB                     REG_FLD(3, 28)
#define PON_DA_CTRL_1_FLD_RG_SSUSB_TX_IMPCALIB_EN              REG_FLD(1, 24)
#define PON_DA_CTRL_1_FLD_RG_SSUSB_TX_DEM                      REG_FLD(2, 13)
#define PON_DA_CTRL_1_FLD_RG_SSUSB_RXDET_EN                    REG_FLD(1, 12)
#define PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_REFCKDIV                REG_FLD(1, 10)
#define PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_PREDIV                  REG_FLD(2, 8)
#define PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_BC                      REG_FLD(2, 4)
#define PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_DIVEN                   REG_FLD(3, 0)

#define PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP1                   REG_FLD(2, 30)
#define PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP2                   REG_FLD(2, 28)
#define PON_DA_CTRL_2_FLD_RG_SSUSB_LFPS_PWD                    REG_FLD(1, 26)
#define PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_PWD                  REG_FLD(1, 25)
#define PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_MODE                 REG_FLD(1, 24)
#define PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_TCADJ                   REG_FLD(6, 16)
#define PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_FBDIV                   REG_FLD(7, 8)
#define PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_BPA                     REG_FLD(2, 0)

#define PON_DA_CTRL_3_FLD_RG_SSUSB_SYSPLL_FBDIV_CHG            REG_FLD(1, 27)
#define PON_DA_CTRL_3_FLD_RG_PCIE_CLKTX_EN                     REG_FLD(1, 26)
#define PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_EN                    REG_FLD(1, 25)
#define PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_RSTB                  REG_FLD(1, 24)
#define PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_OFST                  REG_FLD(4, 20)
#define PON_DA_CTRL_3_FLD_RG_SSUSB_RX_HZ                       REG_FLD(1, 16)
#define PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P3_ENTRY                 REG_FLD(1, 3)
#define PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P1_ENTRY                 REG_FLD(1, 2)
#define PON_DA_CTRL_3_FLD_RG_SSUSB_LFPS_DEGLITCH               REG_FLD(2, 0)

#define PON_DA_CTRL_4_FLD_RG_SSUSB_PLL_FBKDIV                  REG_FLD(9, 0)

#define PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PI0                     REG_FLD(1, 31)
#define PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_PWR_ON           REG_FLD(1, 30)
#define PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_ISO_EN           REG_FLD(1, 29)
#define PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PWD                     REG_FLD(1, 28)
#define PON_DA_CTRL_5_FLD_RG_FORCE_CDR_AUTOK_VCO               REG_FLD(1, 27)
#define PON_DA_CTRL_5_FLD_RG_FORCE_CDR_LCK2REF                 REG_FLD(1, 26)
#define PON_DA_CTRL_5_FLD_RG_FORCE_RX_CMPWD                    REG_FLD(1, 25)
#define PON_DA_CTRL_5_FLD_RG_FORCE_RX_AFE_PWD                  REG_FLD(1, 24)
#define PON_DA_CTRL_5_FLD_RG_FORCE_TX_DATA_EN                  REG_FLD(1, 23)
#define PON_DA_CTRL_5_FLD_RG_FORCE_TX_EIDLE_LP_EN              REG_FLD(1, 22)
#define PON_DA_CTRL_5_FLD_RG_FORCE_TX_SER_EN                   REG_FLD(1, 21)
#define PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_EN                   REG_FLD(1, 20)
#define PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_EN               REG_FLD(1, 19)
#define PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_OFFSET           REG_FLD(1, 18)
#define PON_DA_CTRL_5_FLD_RG_FORCE_RX_PI_CAL_EN                REG_FLD(1, 17)
#define PON_DA_CTRL_5_FLD_RG_FORCE_TX_DRV_EN                   REG_FLD(1, 16)
#define PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS                     REG_FLD(1, 15)
#define PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS_EN                  REG_FLD(1, 14)
#define PON_DA_CTRL_5_FLD_RG_FORCE_RX_SAOSC_EN                 REG_FLD(1, 13)
#define PON_DA_CTRL_5_FLD_RG_FORCE_BIAS_PWD                    REG_FLD(1, 12)
#define PON_DA_CTRL_5_FLD_RG_FORCE_BIASIMR_PWD                 REG_FLD(1, 11)
#define PON_DA_CTRL_5_FLD_RG_FORCE_BG_LPF_EN                   REG_FLD(1, 10)
#define PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_PWD                REG_FLD(1, 9)
#define PON_DA_CTRL_5_FLD_RG_FORCE_XTAL_RX_PWD                 REG_FLD(1, 8)
#define PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_PWD                  REG_FLD(1, 7)
#define PON_DA_CTRL_5_FLD_RG_FORCE_PLL_PWD                     REG_FLD(1, 6)
#define PON_DA_CTRL_5_FLD_RG_FORCE_PLL_AUTOK_VCO               REG_FLD(1, 5)
#define PON_DA_CTRL_5_FLD_RG_FORCE_PLL_SSC_EN                  REG_FLD(1, 4)
#define PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_LPF_EN             REG_FLD(1, 3)
#define PON_DA_CTRL_5_FLD_RG_FORCE_CDR_BAND                    REG_FLD(1, 2)
#define PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BAND                    REG_FLD(1, 1)
#define PON_DA_CTRL_5_FLD_RG_FORCE_RX_LEQOS_EN                 REG_FLD(1, 0)

#define PON_DA_CTRL_6_FLD_RG_FORCE_IEXT_INTR_CTRL              REG_FLD(1, 26)
#define PON_DA_CTRL_6_FLD_RG_FORCE_RX_IMP_SEL                  REG_FLD(1, 25)
#define PON_DA_CTRL_6_FLD_RG_FORCE_TX_IMP_SEL                  REG_FLD(1, 24)
#define PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_EN                    REG_FLD(1, 23)
#define PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_RST                   REG_FLD(1, 22)
#define PON_DA_CTRL_6_FLD_RG_FORCE_RX_SAOSC_RST                REG_FLD(1, 21)
#define PON_DA_CTRL_6_FLD_RG_FORCE_RX_CDR_STABLE               REG_FLD(1, 20)
#define PON_DA_CTRL_6_FLD_RG_FORCE_CDR_KVSEL                   REG_FLD(1, 19)
#define PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BR                      REG_FLD(1, 18)
#define PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BC                      REG_FLD(1, 17)
#define PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIC                     REG_FLD(1, 16)
#define PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIR                     REG_FLD(1, 15)
#define PON_DA_CTRL_6_FLD_RG_FORCE_TX_EIDLE_CM                 REG_FLD(1, 14)
#define PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI_EN                 REG_FLD(1, 13)
#define PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI                    REG_FLD(1, 12)
#define PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_6DB                    REG_FLD(1, 11)
#define PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_6DB                    REG_FLD(1, 10)
#define PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_3P5DB                  REG_FLD(1, 9)
#define PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_3P5DB                  REG_FLD(1, 8)
#define PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_0DB                    REG_FLD(1, 7)
#define PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_PREDIV               REG_FLD(1, 6)
#define PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_RESERVE              REG_FLD(1, 5)
#define PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_FBDIV                REG_FLD(1, 4)
#define PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_PRD                 REG_FLD(1, 3)
#define PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA               REG_FLD(1, 2)
#define PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA1              REG_FLD(1, 1)
#define PON_DA_CTRL_6_FLD_RG_FORCE_CDR_PI90                    REG_FLD(1, 0)

#define PON_DA_CTRL_7_FLD_RG_SSUSB_BG_LPF_EN                   REG_FLD(1, 31)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_PWD                REG_FLD(1, 30)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_XTAL_RX_PWD                 REG_FLD(1, 29)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_SYSPLL_PWD                  REG_FLD(1, 28)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_PWD                     REG_FLD(1, 27)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_AUTOK_VCO               REG_FLD(1, 26)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_SSC_EN                  REG_FLD(1, 25)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_LPF_EN             REG_FLD(1, 24)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_CDR_BAND                    REG_FLD(7, 16)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_RX_LEQOS_EN                 REG_FLD(1, 12)
#define PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BAND                    REG_FLD(11, 0)

#define PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_PWR_ON           REG_FLD(1, 24)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_ISO_EN           REG_FLD(1, 23)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_PWD                     REG_FLD(1, 22)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_AUTOK_VCO               REG_FLD(1, 21)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_LCK2REF                 REG_FLD(1, 20)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_RX_CMPWD                    REG_FLD(1, 19)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_RX_AFE_PWD                  REG_FLD(1, 18)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DATA_EN                  REG_FLD(1, 17)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_TX_EIDLE_LP_EN              REG_FLD(1, 16)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_TX_SER_EN                   REG_FLD(1, 15)
#define PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_EN                    REG_FLD(1, 14)
#define PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_CAL_EN                REG_FLD(1, 13)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_SIGDET_CAL_OFFSET           REG_FLD(5, 8)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_RX_PI_CAL_EN                REG_FLD(1, 6)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DRV_EN                   REG_FLD(1, 5)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS                     REG_FLD(1, 4)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS_EN                  REG_FLD(1, 3)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_RX_SAOSC_EN                 REG_FLD(1, 2)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_BIAS_PWD                    REG_FLD(1, 1)
#define PON_DA_CTRL_8_FLD_RG_SSUSB_BIASIMR_PWD                 REG_FLD(1, 0)

#define PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI90                    REG_FLD(7, 8)
#define PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI0                     REG_FLD(7, 0)

#define PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA              REG_FLD(16, 16)
#define PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA1             REG_FLD(16, 0)

#define PON_DA_CTRL_11_FLD_RG_SSUSB_SYSPLL_FBDIV               REG_FLD(31, 0)

#define PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_3P5DB                 REG_FLD(6, 24)
#define PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_0DB                   REG_FLD(6, 16)

#define PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI_EN                REG_FLD(1, 28)
#define PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI                   REG_FLD(3, 24)
#define PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_6DB                   REG_FLD(6, 16)
#define PON_DA_CTRL_13_FLD_RG_SSUSB_IDRV_6DB                   REG_FLD(6, 8)
#define PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_3P5DB                 REG_FLD(6, 0)

#define PON_DA_CTRL_14_FLD_RG_SSUSB_RX_IMP_SEL                 REG_FLD(5, 24)
#define PON_DA_CTRL_14_FLD_RG_SSUSB_TX_IMP_SEL                 REG_FLD(5, 16)
#define PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_EN                   REG_FLD(1, 8)
#define PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_RST                  REG_FLD(1, 7)
#define PON_DA_CTRL_14_FLD_RG_SSUSB_RX_SAOSC_RST               REG_FLD(1, 6)
#define PON_DA_CTRL_14_FLD_RG_SSUSB_RX_CDR_STABLE              REG_FLD(1, 5)
#define PON_DA_CTRL_14_FLD_RG_SSUSB_CDR_BIR                    REG_FLD(1, 4)
#define PON_DA_CTRL_14_FLD_RG_SSUSB_TX_EIDLE_CM                REG_FLD(4, 0)

#define PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD1                REG_FLD(5, 24)
#define PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD0                REG_FLD(5, 16)
#define PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTR                 REG_FLD(5, 8)

#define PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTD0                 REG_FLD(5, 24)
#define PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTR                  REG_FLD(5, 16)
#define PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD1                REG_FLD(4, 8)
#define PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD0                REG_FLD(4, 4)
#define PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTR                 REG_FLD(4, 0)

#define PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD1              REG_FLD(1, 21)
#define PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD0              REG_FLD(1, 20)
#define PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTR               REG_FLD(1, 19)
#define PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD1                 REG_FLD(3, 16)
#define PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD0                 REG_FLD(3, 12)
#define PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTR                  REG_FLD(3, 8)
#define PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BCLTD1                 REG_FLD(5, 0)

#define PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPB              REG_FLD(1, 1)
#define PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPA              REG_FLD(1, 0)

#define PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPB                    REG_FLD(2, 4)
#define PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPA                    REG_FLD(4, 0)

#define PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out_cnt             REG_FLD(16, 16)
#define PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out                 REG_FLD(1, 9)
#define PON_DEBUG_REGISTER_0_FLD_ro_intf_pma_rx_sigdet_out     REG_FLD(1, 8)
#define PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_lfps                 REG_FLD(1, 7)
#define PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_impcal_out           REG_FLD(1, 6)
#define PON_DEBUG_REGISTER_0_FLD_ro_rx_path_rdy                REG_FLD(1, 5)
#define PON_DEBUG_REGISTER_0_FLD_ro_rx_eq_en                   REG_FLD(1, 4)
#define PON_DEBUG_REGISTER_0_FLD_ro_eq_done                    REG_FLD(1, 3)
#define PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_cal_out          REG_FLD(1, 2)
#define PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_out              REG_FLD(1, 1)
#define PON_DEBUG_REGISTER_0_FLD_ro_lck2ref                    REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_1_FLD_rg_eq_done_clr                REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_2_FLD_ro_qphy_rx_ctrl_debug         REG_FLD(8, 24)
#define PON_DEBUG_REGISTER_2_FLD_ro_da_tx_data_en              REG_FLD(1, 16)
#define PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel_latch        REG_FLD(4, 12)
#define PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel              REG_FLD(3, 8)
#define PON_DEBUG_REGISTER_2_FLD_ro_pll_vco_cal_state          REG_FLD(7, 0)

#define PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_out         REG_FLD(1, 20)
#define PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_lthout      REG_FLD(1, 19)
#define PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_cal_out     REG_FLD(1, 18)
#define PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_ckdet_out          REG_FLD(1, 17)
#define PON_DEBUG_REGISTER_3_FLD_ro_saosc_done                 REG_FLD(1, 16)
#define PON_DEBUG_REGISTER_3_FLD_rg_freq_clr                   REG_FLD(1, 8)
#define PON_DEBUG_REGISTER_3_FLD_ro_baser_rx_freq_lock         REG_FLD(1, 5)
#define PON_DEBUG_REGISTER_3_FLD_ro_baser_tx_freq_lock         REG_FLD(1, 4)
#define PON_DEBUG_REGISTER_3_FLD_ro_hg_rx_freq_lock            REG_FLD(1, 3)
#define PON_DEBUG_REGISTER_3_FLD_ro_hg_tx_freq_lock            REG_FLD(1, 2)
#define PON_DEBUG_REGISTER_3_FLD_ro_pon_rx_freq_lock           REG_FLD(1, 1)
#define PON_DEBUG_REGISTER_3_FLD_ro_pon_tx_freq_lock           REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_4_FLD_rg_ben_count_en               REG_FLD(1, 16)
#define PON_DEBUG_REGISTER_4_FLD_ro_ben_counter                REG_FLD(16, 0)

#define PON_DUMMY_REG_0_FLD_rg_dummy_reg                       REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_insert_dbg_clr     REG_FLD(1, 12)
#define PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_usr_def_error_en   REG_FLD(1, 8)
#define PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_cont    REG_FLD(1, 4)
#define PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_en      REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_6_FLD_rg_fec_dec_err_byte_num       REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_7_FLD_rg_fec_dec_usr_def_error      REG_FLD(16, 0)

#define PON_DEBUG_REGISTER_8_FLD_rg_fec_dec_err_insert_start_w1c REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_insert_dbg_clr     REG_FLD(1, 12)
#define PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_usr_def_error_en   REG_FLD(1, 8)
#define PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_cont    REG_FLD(1, 4)
#define PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_en      REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_10_FLD_rg_fec_enc_err_byte_num      REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_11_FLD_rg_fec_enc_usr_def_error     REG_FLD(16, 0)

#define PON_DEBUG_REGISTER_12_FLD_rg_fec_enc_err_insert_start_w1c REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_13_FLD_rg_trans_los_pro_cnt_thr     REG_FLD(4, 8)
#define PON_DEBUG_REGISTER_13_FLD_rg_phyrdy_pro_cnt_thr        REG_FLD(4, 0)

#define PON_DEBUG_REGISTER_14_FLD_rg_int_record_start_clr_w1c  REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_15_FLD_rg_record_start_type         REG_FLD(3, 12)
#define PON_DEBUG_REGISTER_15_FLD_rg_record_start_dbg_clr      REG_FLD(1, 8)
#define PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_keep         REG_FLD(1, 5)
#define PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_en           REG_FLD(1, 4)
#define PON_DEBUG_REGISTER_15_FLD_rg_int_record_en             REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_16_FLD_ro_lof_time_cnt              REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_17_FLD_ro_los_time_cnt              REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_18_FLD_ro_phyrdy_time_cnt           REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_19_FLD_ro_tx_bit_dly                REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_20_FLD_rg_int_record_cnt_thr        REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_21_FLD_ro_record_done               REG_FLD(1, 12)
#define PON_DEBUG_REGISTER_21_FLD_ro_fec_enc_insert_dbg        REG_FLD(1, 8)
#define PON_DEBUG_REGISTER_21_FLD_ro_record_start_dbg          REG_FLD(1, 4)
#define PON_DEBUG_REGISTER_21_FLD_ro_fec_dec_insert_dbg        REG_FLD(1, 0)

#define PON_DEBUG_REGISTER_22_FLD_rg_lof_int_mask_cnt_thr      REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_23_FLD_rgs_pll_autok_band           REG_FLD(12, 0)

#define PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_lck_rst           REG_FLD(1, 17)
#define PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_force_lck         REG_FLD(1, 16)
#define PON_DEBUG_REGISTER_24_FLD_ro_fpga_tx_los_lock_cnt      REG_FLD(4, 8)
#define PON_DEBUG_REGISTER_24_FLD_ro_fpga_rx_los_lock_cnt      REG_FLD(4, 4)
#define PON_DEBUG_REGISTER_24_FLD_ro_fpga_lock_status          REG_FLD(4, 0)

#define PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_dlybnd_delsel   REG_FLD(2, 12)
#define PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_epsram_delsel   REG_FLD(2, 10)
#define PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_ssram_delsel      REG_FLD(2, 8)
#define PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram3_delsel     REG_FLD(2, 6)
#define PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram2_delsel     REG_FLD(2, 4)
#define PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram1_delsel     REG_FLD(2, 2)
#define PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram0_delsel     REG_FLD(2, 0)

#define PON_DEBUG_REGISTER_26_FLD_ro_fpga_debug_0              REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_27_FLD_ro_fpga_debug_1              REG_FLD(32, 0)

#define PON_DEBUG_REGISTER_28_FLD_ro_ad_sig_cal_offset         REG_FLD(5, 24)
#define PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_band               REG_FLD(7, 16)
#define PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_fail        REG_FLD(1, 9)
#define PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_cplt        REG_FLD(1, 8)
#define PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_state       REG_FLD(7, 0)

#define PON_DEBUG_REGISTER_29_FLD_ro_sfp_tx_en_counter         REG_FLD(32, 0)

#define PON_INTERRUPT_STS_0_FLD_ro_tx_en_int                   REG_FLD(1, 7)
#define PON_INTERRUPT_STS_0_FLD_ro_rx_sd_int                   REG_FLD(1, 6)
#define PON_INTERRUPT_STS_0_FLD_ro_los_int                     REG_FLD(1, 5)
#define PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_unlock         REG_FLD(1, 4)
#define PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_lock           REG_FLD(1, 3)
#define PON_INTERRUPT_STS_0_FLD_ro_pma_rx_sigdet_out           REG_FLD(1, 2)
#define PON_INTERRUPT_STS_0_FLD_ro_ncpo_min_flag               REG_FLD(1, 1)
#define PON_INTERRUPT_STS_0_FLD_ro_ncpo_max_flag               REG_FLD(1, 0)

#define PON_INTERRUPT_EN_0_FLD_rg_tx_en_int_en                 REG_FLD(1, 7)
#define PON_INTERRUPT_EN_0_FLD_rg_rx_sd_int_en                 REG_FLD(1, 6)
#define PON_INTERRUPT_EN_0_FLD_rg_los_int_en                   REG_FLD(1, 5)
#define PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_unlock_int_en   REG_FLD(1, 4)
#define PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_lock_int_en     REG_FLD(1, 3)
#define PON_INTERRUPT_EN_0_FLD_rg_pma_sigdet_int_en            REG_FLD(1, 2)
#define PON_INTERRUPT_EN_0_FLD_rg_ncpo_min_int_en              REG_FLD(1, 1)
#define PON_INTERRUPT_EN_0_FLD_rg_ncpo_max_int_en              REG_FLD(1, 0)

#define PON_INTERRUPT_CLR_0_FLD_rg_tx_en_int_clr               REG_FLD(1, 7)
#define PON_INTERRUPT_CLR_0_FLD_rg_rx_sd_int_clr               REG_FLD(1, 6)
#define PON_INTERRUPT_CLR_0_FLD_rg_los_int_clr                 REG_FLD(1, 5)
#define PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_unlock_int_clr REG_FLD(1, 4)
#define PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_lock_int_clr   REG_FLD(1, 3)
#define PON_INTERRUPT_CLR_0_FLD_rg_pma_rx_sigdet_intclr        REG_FLD(1, 2)
#define PON_INTERRUPT_CLR_0_FLD_rg_ncpo_flag_clr               REG_FLD(1, 1)
#define PON_INTERRUPT_CLR_0_FLD_rg_ncpo_clr                    REG_FLD(1, 0)

#define PON_INTERRUPT_SET_0_FLD_rg_ncpo_det_en                 REG_FLD(1, 16)
#define PON_INTERRUPT_SET_0_FLD_rg_rogue_en                    REG_FLD(1, 12)
#define PON_INTERRUPT_SET_0_FLD_rg_int_los_on_pro              REG_FLD(1, 8)
#define PON_INTERRUPT_SET_0_FLD_rg_int_los_cnt                 REG_FLD(8, 0)

#define PON_INTERRUPT_SET_1_FLD_rg_tx_en_counter               REG_FLD(32, 0)

#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_int_los_inv(reg32)     REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_int_los_inv, (reg32))
#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_tdc_los_inv(reg32)     REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_los_inv, (reg32))
#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_tdc_ck_en_and_los(reg32) REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_ck_en_and_los, (reg32))
#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_tdc_lck2ref_sel(reg32) REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_lck2ref_sel, (reg32))
#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_sw_lcpll_en(reg32)     REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_sw_lcpll_en, (reg32))
#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_lcpll_en_inv(reg32)    REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_en_inv, (reg32))
#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_lcpll_hw_ctrl_mode(reg32) REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_hw_ctrl_mode, (reg32))
#define SS_LCPLL_PWCTL_SETTING_0_GET_rg_lcpll_force_on(reg32)  REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_force_on, (reg32))

#define SS_LCPLL_PWCTL_SETTING_1_GET_rg_lcpll_ck_stb_timer(reg32) REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_ck_stb_timer, (reg32))
#define SS_LCPLL_PWCTL_SETTING_1_GET_rg_lcpll_pcw_man_load_timer(reg32) REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_pcw_man_load_timer, (reg32))
#define SS_LCPLL_PWCTL_SETTING_1_GET_rg_lcpll_en_timer(reg32)  REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_en_timer, (reg32))
#define SS_LCPLL_PWCTL_SETTING_1_GET_rg_lcpll_man_pwdb(reg32)  REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_man_pwdb, (reg32))

#define SS_LCPLL_PWCTL_SETTING_2_GET_rg_ncpo_ana_msb(reg32)    REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_ncpo_ana_msb, (reg32))
#define SS_LCPLL_PWCTL_SETTING_2_GET_rg_tdc_mst_en(reg32)      REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_mst_en, (reg32))
#define SS_LCPLL_PWCTL_SETTING_2_GET_rg_tdc_ck_stb_timer(reg32) REG_FLD_GET(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_ck_stb_timer, (reg32))

#define SS_LCPLL_PWCTL_DBG_SETTING_GET_rg_lcpll_ledck_div(reg32) REG_FLD_GET(SS_LCPLL_PWCTL_DBG_SETTING_FLD_rg_lcpll_ledck_div, (reg32))

#define SS_LCPLL_TDC_PW_0_GET_rg_lcpll_dpma_dds1_iso_disable(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_iso_disable, (reg32))
#define SS_LCPLL_TDC_PW_0_GET_rg_lcpll_dpma_dds1_pwdb(reg32)   REG_FLD_GET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_pwdb, (reg32))
#define SS_LCPLL_TDC_PW_0_GET_rg_lcpll_dpma_autopw_en(reg32)   REG_FLD_GET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_autopw_en, (reg32))
#define SS_LCPLL_TDC_PW_0_GET_rg_lcpll_tdc_dig_pwdb(reg32)     REG_FLD_GET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_tdc_dig_pwdb, (reg32))

#define SS_LCPLL_TDC_PW_1_GET_rg_lcpll_dpma_dds1_pcw_ncpo_chg(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg, (reg32))
#define SS_LCPLL_TDC_PW_1_GET_rg_lcpll_dpma_dds1_rstb(reg32)   REG_FLD_GET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb, (reg32))
#define SS_LCPLL_TDC_PW_1_GET_rg_lcpll_dpma_dds1_rstb_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb_dly, (reg32))
#define SS_LCPLL_TDC_PW_1_GET_rg_lcpll_dpma_dds1_iso_disable_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_iso_disable_dly, (reg32))

#define SS_LCPLL_TDC_PW_2_GET_rg_lcpll_dpma_txpll1_ddsfbk_en(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en, (reg32))
#define SS_LCPLL_TDC_PW_2_GET_rg_lcpll_dpma_dds1_ncpo_en_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en_dly, (reg32))
#define SS_LCPLL_TDC_PW_2_GET_rg_lcpll_dpma_dds1_ncpo_en(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en, (reg32))
#define SS_LCPLL_TDC_PW_2_GET_rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly, (reg32))

#define SS_LCPLL_TDC_PW_3_GET_rg_lcpll_tdc_cal(reg32)          REG_FLD_GET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal, (reg32))
#define SS_LCPLL_TDC_PW_3_GET_rg_lcpll_tdc_cal_trig_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig_dly, (reg32))
#define SS_LCPLL_TDC_PW_3_GET_rg_lcpll_tdc_cal_trig(reg32)     REG_FLD_GET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig, (reg32))
#define SS_LCPLL_TDC_PW_3_GET_rg_lcpll_dpma_txpll1_ddsfbk_en_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en_dly, (reg32))

#define SS_LCPLL_TDC_PW_4_GET_rg_lcpll_tdc_sigdet_cg(reg32)    REG_FLD_GET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_sigdet_cg, (reg32))
#define SS_LCPLL_TDC_PW_4_GET_rg_lcpll_tdc_en(reg32)           REG_FLD_GET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_en, (reg32))
#define SS_LCPLL_TDC_PW_4_GET_rg_lcpll_tdc_cal_offset(reg32)   REG_FLD_GET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_offset, (reg32))
#define SS_LCPLL_TDC_PW_4_GET_rg_lcpll_tdc_cal_bw(reg32)       REG_FLD_GET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_bw, (reg32))

#define SS_LCPLL_TDC_PW_5_GET_rg_tdc_start_cnt(reg32)          REG_FLD_GET(SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt, (reg32))
#define SS_LCPLL_TDC_PW_5_GET_rg_tdc_start_cnt_clr(reg32)      REG_FLD_GET(SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt_clr, (reg32))
#define SS_LCPLL_TDC_PW_5_GET_rg_ncpo_start_dly_en(reg32)      REG_FLD_GET(SS_LCPLL_TDC_PW_5_FLD_rg_ncpo_start_dly_en, (reg32))
#define SS_LCPLL_TDC_PW_5_GET_rg_lcpll_tdc_sync_in_mode(reg32) REG_FLD_GET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_sync_in_mode, (reg32))
#define SS_LCPLL_TDC_PW_5_GET_rg_lcpll_autok_tdc(reg32)        REG_FLD_GET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_autok_tdc, (reg32))
#define SS_LCPLL_TDC_PW_5_GET_rg_lcpll_tdc_autoen(reg32)       REG_FLD_GET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_autoen, (reg32))
#define SS_LCPLL_TDC_PW_5_GET_rg_lcpll_tdc_en_mux(reg32)       REG_FLD_GET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_en_mux, (reg32))

#define SS_LCPLL_TDC_FLT_0_GET_rg_lcpll_tdc_offset(reg32)      REG_FLD_GET(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_tdc_offset, (reg32))
#define SS_LCPLL_TDC_FLT_0_GET_rg_lcpll_ki(reg32)              REG_FLD_GET(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_ki, (reg32))
#define SS_LCPLL_TDC_FLT_0_GET_rg_lcpll_pon_rx_cdr_divtdc(reg32) REG_FLD_GET(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_pon_rx_cdr_divtdc, (reg32))

#define SS_LCPLL_TDC_FLT_1_GET_rg_lcpll_tdc_gain(reg32)        REG_FLD_GET(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_tdc_gain, (reg32))
#define SS_LCPLL_TDC_FLT_1_GET_rg_lcpll_a_tdc(reg32)           REG_FLD_GET(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_a_tdc, (reg32))
#define SS_LCPLL_TDC_FLT_1_GET_rg_lcpll_gpon_sel(reg32)        REG_FLD_GET(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_gpon_sel, (reg32))

#define SS_LCPLL_TDC_FLT_2_GET_rg_lcpll_ncpo_value(reg32)      REG_FLD_GET(SS_LCPLL_TDC_FLT_2_FLD_rg_lcpll_ncpo_value, (reg32))

#define SS_LCPLL_TDC_FLT_3_GET_rg_sdm_upd_disb(reg32)          REG_FLD_GET(SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd_disb, (reg32))
#define SS_LCPLL_TDC_FLT_3_GET_rg_sdm_upd(reg32)               REG_FLD_GET(SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd, (reg32))
#define SS_LCPLL_TDC_FLT_3_GET_rg_lcpll_tdc_swrst(reg32)       REG_FLD_GET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_tdc_swrst, (reg32))
#define SS_LCPLL_TDC_FLT_3_GET_rg_lcpll_ncpo_hold(reg32)       REG_FLD_GET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_hold, (reg32))
#define SS_LCPLL_TDC_FLT_3_GET_rg_lcpll_ncpo_load(reg32)       REG_FLD_GET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_load, (reg32))
#define SS_LCPLL_TDC_FLT_3_GET_rg_lcpll_ncpo_shift(reg32)      REG_FLD_GET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_shift, (reg32))

#define SS_LCPLL_TDC_FLT_4_GET_rg_lcpll_tdcen_wait_lck2ref(reg32) REG_FLD_GET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_wait_lck2ref, (reg32))
#define SS_LCPLL_TDC_FLT_4_GET_rg_lcpll_tdcen_chk_lck2ref(reg32) REG_FLD_GET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_chk_lck2ref, (reg32))
#define SS_LCPLL_TDC_FLT_4_GET_rg_lcpll_tdc_rst(reg32)         REG_FLD_GET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdc_rst, (reg32))
#define SS_LCPLL_TDC_FLT_4_GET_rg_lcpll_tdcen_rst_dlf(reg32)   REG_FLD_GET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_rst_dlf, (reg32))

#define SS_LCPLL_TDC_FLT_5_GET_rg_lcpll_ncpo_chg(reg32)        REG_FLD_GET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_ncpo_chg, (reg32))
#define SS_LCPLL_TDC_FLT_5_GET_rg_lcpll_tdc_autopw_ncpo(reg32) REG_FLD_GET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_autopw_ncpo, (reg32))
#define SS_LCPLL_TDC_FLT_5_GET_rg_lcpll_tdc_waitlck_pos_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pos_dly, (reg32))
#define SS_LCPLL_TDC_FLT_5_GET_rg_lcpll_tdc_waitlck_pre_dly(reg32) REG_FLD_GET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pre_dly, (reg32))

#define SS_LCPLL_TDC_FLT_6_GET_rg_lcpll_tdc_pcw_sub1en(reg32)  REG_FLD_GET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_tdc_pcw_sub1en, (reg32))
#define SS_LCPLL_TDC_FLT_6_GET_rg_lcpll_dlf_mult_en(reg32)     REG_FLD_GET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_dlf_mult_en, (reg32))
#define SS_LCPLL_TDC_FLT_6_GET_rg_lcpll_ncpo_chg_delay(reg32)  REG_FLD_GET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_delay, (reg32))
#define SS_LCPLL_TDC_FLT_6_GET_rg_lcpll_ncpo_chg_mask(reg32)   REG_FLD_GET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_mask, (reg32))

#define SS_LCPLL_TDC_FLT_7_GET_rg_lcpll_tdcck_inv_en(reg32)    REG_FLD_GET(SS_LCPLL_TDC_FLT_7_FLD_rg_lcpll_tdcck_inv_en, (reg32))

#define SS_LCPLL_TDC_PCW_1_GET_rg_lcpll_pon_hrdds_pcw_ncpo_gpon(reg32) REG_FLD_GET(SS_LCPLL_TDC_PCW_1_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_gpon, (reg32))

#define SS_LCPLL_TDC_PCW_2_GET_rg_lcpll_pon_hrdds_pcw_ncpo_epon(reg32) REG_FLD_GET(SS_LCPLL_TDC_PCW_2_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_epon, (reg32))

#define SS_LCPLL_TDC_RO_1_GET_ro_lcpll_tdc_dig(reg32)          REG_FLD_GET(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_dig, (reg32))
#define SS_LCPLL_TDC_RO_1_GET_ro_lcpll_tdc_st(reg32)           REG_FLD_GET(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_st, (reg32))
#define SS_LCPLL_TDC_RO_1_GET_ro_lcpll_tdc_tdcpw_st(reg32)     REG_FLD_GET(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_tdcpw_st, (reg32))

#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_tdc_ck_stb(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_ck_stb, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_txpll1_ddsfbk_en(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_txpll1_ddsfbk_en, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_tdc_cal_offset(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_offset, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_tdc_cal_bw(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_bw, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_tdc_en(reg32)  REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_en, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_rx_ck_stb(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_rx_ck_stb, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_en_lck2ref_rst(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_en_lck2ref_rst, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_dds1_pwdb(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pwdb, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_dds1_iso_disable(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_iso_disable, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_dds1_rstb(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_rstb, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_dds1_ncpo_en(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_ncpo_en, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg, (reg32))
#define SS_LCPLL_TDC_RO_2_GET_ro_lcpll_tdc_dlf_gain(reg32)     REG_FLD_GET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dlf_gain, (reg32))

#define SS_LCPLL_TDC_RO_3_GET_ro_lcpll_tdc_gain(reg32)         REG_FLD_GET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_gain, (reg32))
#define SS_LCPLL_TDC_RO_3_GET_ro_lcpll_tdc_decode(reg32)       REG_FLD_GET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_decode, (reg32))
#define SS_LCPLL_TDC_RO_3_GET_ro_lcpll_tdc_en(reg32)           REG_FLD_GET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_en, (reg32))
#define SS_LCPLL_TDC_RO_3_GET_ro_lcpll_tdc_cal_done(reg32)     REG_FLD_GET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_cal_done, (reg32))
#define SS_LCPLL_TDC_RO_3_GET_ro_lcpll_tdc_offset(reg32)       REG_FLD_GET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_offset, (reg32))

#define SS_LCPLL_TDC_RO_4_GET_ro_lcpll_tdc_dpma_dds1_pcw_ncpo(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_4_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo, (reg32))

#define SS_LCPLL_TDC_RO_5_GET_ro_lcpll_tdc_dpma_tdc_c(reg32)   REG_FLD_GET(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_c, (reg32))
#define SS_LCPLL_TDC_RO_5_GET_ro_lcpll_tdc_dpma_tdc_d(reg32)   REG_FLD_GET(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_d, (reg32))
#define SS_LCPLL_TDC_RO_5_GET_ro_lcpll_tdc_dpma_tdc_leadlag(reg32) REG_FLD_GET(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_leadlag, (reg32))

#define SS_LCPLL_TDC_CTRL_0_GET_rg_lcpll_ki_hold(reg32)        REG_FLD_GET(SS_LCPLL_TDC_CTRL_0_FLD_rg_lcpll_ki_hold, (reg32))

#define SS_LCPLL_0_GET_tdc_sync_pcw_ncpo_chg(reg32)            REG_FLD_GET(SS_LCPLL_0_FLD_tdc_sync_pcw_ncpo_chg, (reg32))
#define SS_LCPLL_0_GET_eee_lcpll_hold(reg32)                   REG_FLD_GET(SS_LCPLL_0_FLD_eee_lcpll_hold, (reg32))

#define SS_LCPLL_1_GET_tdc_sync_pcw_ncpo(reg32)                REG_FLD_GET(SS_LCPLL_1_FLD_tdc_sync_pcw_ncpo, (reg32))

#define SS_LCPLL_2_GET_lcpll_top_flagh_en(reg32)               REG_FLD_GET(SS_LCPLL_2_FLD_lcpll_top_flagh_en, (reg32))
#define SS_LCPLL_2_GET_lcpll_top_flagl_en(reg32)               REG_FLD_GET(SS_LCPLL_2_FLD_lcpll_top_flagl_en, (reg32))
#define SS_LCPLL_2_GET_flagh_sel(reg32)                        REG_FLD_GET(SS_LCPLL_2_FLD_flagh_sel, (reg32))
#define SS_LCPLL_2_GET_flagl_sel(reg32)                        REG_FLD_GET(SS_LCPLL_2_FLD_flagl_sel, (reg32))

#define TDC_DLF_GAIN_STATUS_GET_ro_dlf_gain_rund_new(reg32)    REG_FLD_GET(TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_rund_new, (reg32))
#define TDC_DLF_GAIN_STATUS_GET_ro_dlf_gain_new(reg32)         REG_FLD_GET(TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_new, (reg32))

#define TDC_DLF_MODE_SETTING_GET_rg_dlf_gain_latch(reg32)      REG_FLD_GET(TDC_DLF_MODE_SETTING_FLD_rg_dlf_gain_latch, (reg32))
#define TDC_DLF_MODE_SETTING_GET_rg_dlf_mode_sel(reg32)        REG_FLD_GET(TDC_DLF_MODE_SETTING_FLD_rg_dlf_mode_sel, (reg32))
#define TDC_DLF_MODE_SETTING_GET_ro_dlf_gain_new_org(reg32)    REG_FLD_GET(TDC_DLF_MODE_SETTING_FLD_ro_dlf_gain_new_org, (reg32))

#define TDC_HOLD_CTRL_0_GET_rg_ncpo_hold_en(reg32)             REG_FLD_GET(TDC_HOLD_CTRL_0_FLD_rg_ncpo_hold_en, (reg32))
#define TDC_HOLD_CTRL_0_GET_rg_ncpo_toggle(reg32)              REG_FLD_GET(TDC_HOLD_CTRL_0_FLD_rg_ncpo_toggle, (reg32))
#define TDC_HOLD_CTRL_0_GET_rg_tdc_los_hold_mux(reg32)         REG_FLD_GET(TDC_HOLD_CTRL_0_FLD_rg_tdc_los_hold_mux, (reg32))
#define TDC_HOLD_CTRL_0_GET_rg_tdc_ncpo_hold_release_time(reg32) REG_FLD_GET(TDC_HOLD_CTRL_0_FLD_rg_tdc_ncpo_hold_release_time, (reg32))

#define TDC_HOLD_CTRL_1_GET_rg_ncpo_up_thred(reg32)            REG_FLD_GET(TDC_HOLD_CTRL_1_FLD_rg_ncpo_up_thred, (reg32))

#define TDC_HOLD_CTRL_2_GET_rg_ncpo_dwn_thred(reg32)           REG_FLD_GET(TDC_HOLD_CTRL_2_FLD_rg_ncpo_dwn_thred, (reg32))

#define TDC_HOLD_STATUS_0_GET_ro_ncpo_output(reg32)            REG_FLD_GET(TDC_HOLD_STATUS_0_FLD_ro_ncpo_output, (reg32))

#define TDC_HOLD_STATUS_1_GET_ro_ncpo_max_output(reg32)        REG_FLD_GET(TDC_HOLD_STATUS_1_FLD_ro_ncpo_max_output, (reg32))

#define TDC_HOLD_STATUS_2_GET_ro_ncpo_min_output(reg32)        REG_FLD_GET(TDC_HOLD_STATUS_2_FLD_ro_ncpo_min_output, (reg32))

#define PON_SERDES_CTRL_0_GET_rg_tx_data_path_sel(reg32)       REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_tx_data_path_sel, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_ben_inv(reg32)                REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_ben_inv, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_force_tphy_speed_tx(reg32)    REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_force_tphy_speed_tx_sel(reg32) REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx_sel, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_pcie_device_mode(reg32)       REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_pcie_device_mode, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_force_tphy_speed_rx_sel(reg32) REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_rx_sel, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_force_tphy_mode(reg32)        REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_mode, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_tphy_mode_set(reg32)          REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_tphy_mode_set, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_sgmii_auto_init(reg32)        REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_sgmii_auto_init, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_tphy_speed_mode_force(reg32)  REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode_force, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_tphy_speed_mode(reg32)        REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_speed_chg(reg32)              REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_speed_chg, (reg32))
#define PON_SERDES_CTRL_0_GET_rg_phya_auto_init(reg32)         REG_FLD_GET(PON_SERDES_CTRL_0_FLD_rg_phya_auto_init, (reg32))

#define PON_SERDES_CTRL_1_GET_rg_rx_pwd(reg32)                 REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_rx_pwd, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_rx_pwd_mux(reg32)             REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_rx_pwd_mux, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_tx_disable(reg32)             REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_tx_disable, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_tx_disable_mux(reg32)         REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_tx_disable_mux, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_phya_pwd(reg32)               REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_phya_pwd, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_phya_pwd_mux(reg32)           REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_phya_pwd_mux, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_freq_det_frc_en(reg32)        REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_freq_det_frc_en, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_freq_det_frc(reg32)           REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_freq_det_frc, (reg32))
#define PON_SERDES_CTRL_1_GET_rg_cdr_re_calib(reg32)           REG_FLD_GET(PON_SERDES_CTRL_1_FLD_rg_cdr_re_calib, (reg32))

#define PON_SERDES_CTRL_2_GET_rg_ben_on_opt(reg32)             REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_ben_on_opt, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_epon_benoff_opt(reg32)        REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_epon_benoff_opt, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_bypass_int_dly(reg32)         REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_bypass_int_dly, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_tx_div_mode(reg32)            REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_tx_div_mode, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_tx_idle_en(reg32)             REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_tx_idle_en, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_tx_idle_mux(reg32)            REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_tx_idle_mux, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_tx_idle_mode(reg32)           REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_tx_idle_mode, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_tx_idle(reg32)                REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_tx_idle, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_sigdet_wait_force(reg32)      REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_force, (reg32))
#define PON_SERDES_CTRL_2_GET_rg_sigdet_wait_cond(reg32)       REG_FLD_GET(PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_cond, (reg32))

#define PON_SERDES_CTRL_3_GET_rg_cdr_settle_dly(reg32)         REG_FLD_GET(PON_SERDES_CTRL_3_FLD_rg_cdr_settle_dly, (reg32))
#define PON_SERDES_CTRL_3_GET_rg_cdr_bw_sel_dly(reg32)         REG_FLD_GET(PON_SERDES_CTRL_3_FLD_rg_cdr_bw_sel_dly, (reg32))
#define PON_SERDES_CTRL_3_GET_rg_bias_pwd_to_bg_lpf_en_dly(reg32) REG_FLD_GET(PON_SERDES_CTRL_3_FLD_rg_bias_pwd_to_bg_lpf_en_dly, (reg32))

#define PON_SERDES_CTRL_4_GET_rg_freq_meter_sel(reg32)         REG_FLD_GET(PON_SERDES_CTRL_4_FLD_rg_freq_meter_sel, (reg32))
#define PON_SERDES_CTRL_4_GET_rg_ppm_thres(reg32)              REG_FLD_GET(PON_SERDES_CTRL_4_FLD_rg_ppm_thres, (reg32))
#define PON_SERDES_CTRL_4_GET_rg_lpf_window_sel(reg32)         REG_FLD_GET(PON_SERDES_CTRL_4_FLD_rg_lpf_window_sel, (reg32))
#define PON_SERDES_CTRL_4_GET_rg_freq_det_en_wait_dly(reg32)   REG_FLD_GET(PON_SERDES_CTRL_4_FLD_rg_freq_det_en_wait_dly, (reg32))

#define PON_SERDES_CTRL_5_GET_rg_sig_det_wait_dly(reg32)       REG_FLD_GET(PON_SERDES_CTRL_5_FLD_rg_sig_det_wait_dly, (reg32))
#define PON_SERDES_CTRL_5_GET_rg_rx_eq_en_h_dly(reg32)         REG_FLD_GET(PON_SERDES_CTRL_5_FLD_rg_rx_eq_en_h_dly, (reg32))

#define PON_SERDES_CTRL_6_GET_rg_rx_saosc_en_h_dly(reg32)      REG_FLD_GET(PON_SERDES_CTRL_6_FLD_rg_rx_saosc_en_h_dly, (reg32))
#define PON_SERDES_CTRL_6_GET_rg_rx_pi_cal_en_h_dly(reg32)     REG_FLD_GET(PON_SERDES_CTRL_6_FLD_rg_rx_pi_cal_en_h_dly, (reg32))
#define PON_SERDES_CTRL_6_GET_rg_pll_autok_vco_to_pll_bias_lpf_en_dly(reg32) REG_FLD_GET(PON_SERDES_CTRL_6_FLD_rg_pll_autok_vco_to_pll_bias_lpf_en_dly, (reg32))

#define PON_SERDES_CTRL_7_GET_rg_syspll_pwd_to_syspll_ready_dly(reg32) REG_FLD_GET(PON_SERDES_CTRL_7_FLD_rg_syspll_pwd_to_syspll_ready_dly, (reg32))
#define PON_SERDES_CTRL_7_GET_rg_sig_det_l_thres(reg32)        REG_FLD_GET(PON_SERDES_CTRL_7_FLD_rg_sig_det_l_thres, (reg32))
#define PON_SERDES_CTRL_7_GET_rg_sig_det_h_thres(reg32)        REG_FLD_GET(PON_SERDES_CTRL_7_FLD_rg_sig_det_h_thres, (reg32))

#define PON_SERDES_CTRL_8_GET_rg_sigdet_out_sel(reg32)         REG_FLD_GET(PON_SERDES_CTRL_8_FLD_rg_sigdet_out_sel, (reg32))
#define PON_SERDES_CTRL_8_GET_rg_eidle_lp_en_ne_to_data_en_pe_dly_g2(reg32) REG_FLD_GET(PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g2, (reg32))
#define PON_SERDES_CTRL_8_GET_rg_eidle_lp_en_ne_to_data_en_pe_dly_g1(reg32) REG_FLD_GET(PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g1, (reg32))

#define PON_SERDES_CTRL_9_GET_rg_ssusb_cdr_band_fail(reg32)    REG_FLD_GET(PON_SERDES_CTRL_9_FLD_rg_ssusb_cdr_band_fail, (reg32))
#define PON_SERDES_CTRL_9_GET_rg_ssusb_pll_band_fail(reg32)    REG_FLD_GET(PON_SERDES_CTRL_9_FLD_rg_ssusb_pll_band_fail, (reg32))

#define PON_SERDES_CTRL_10_GET_ro_bist_run(reg32)              REG_FLD_GET(PON_SERDES_CTRL_10_FLD_ro_bist_run, (reg32))
#define PON_SERDES_CTRL_10_GET_ro_bist_ok(reg32)               REG_FLD_GET(PON_SERDES_CTRL_10_FLD_ro_bist_ok, (reg32))
#define PON_SERDES_CTRL_10_GET_ro_bist_err(reg32)              REG_FLD_GET(PON_SERDES_CTRL_10_FLD_ro_bist_err, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_prbs_mode(reg32)             REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_prbs_mode, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_renew_mode(reg32)            REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_renew_mode, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_prbs_injerr(reg32)           REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_prbs_injerr, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_prbs_en(reg32)               REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_prbs_en, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_prbs_check(reg32)            REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_prbs_check, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_tx_bit_polarity(reg32)       REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_tx_bit_polarity, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_tbi_tx_disable(reg32)        REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_tbi_tx_disable, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_tbi_rx_disable(reg32)        REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_tbi_rx_disable, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_rx_bit_polarity(reg32)       REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_rx_bit_polarity, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_qphy_r2t_loopback_fifo_rst(reg32) REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback_fifo_rst, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_qphy_r2t_loopback(reg32)     REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_pcs_t2r_loopback(reg32)      REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_pcs_t2r_loopback, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_afifo_winc_serdes(reg32)     REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_afifo_winc_serdes, (reg32))
#define PON_SERDES_CTRL_10_GET_rg_afifo_rinc_serdes(reg32)     REG_FLD_GET(PON_SERDES_CTRL_10_FLD_rg_afifo_rinc_serdes, (reg32))

#define PON_SERDES_STS_0_GET_ro_debug_prbs(reg32)              REG_FLD_GET(PON_SERDES_STS_0_FLD_ro_debug_prbs, (reg32))
#define PON_SERDES_STS_0_GET_ro_bist_err_cnt(reg32)            REG_FLD_GET(PON_SERDES_STS_0_FLD_ro_bist_err_cnt, (reg32))

#define PON_SERDES_CTRL_11_GET_rg_udp_data_31_0(reg32)         REG_FLD_GET(PON_SERDES_CTRL_11_FLD_rg_udp_data_31_0, (reg32))

#define PON_SERDES_CTRL_12_GET_rg_udp_data_63_32(reg32)        REG_FLD_GET(PON_SERDES_CTRL_12_FLD_rg_udp_data_63_32, (reg32))

#define PON_SERDES_CTRL_13_GET_rg_udp_data_79_64(reg32)        REG_FLD_GET(PON_SERDES_CTRL_13_FLD_rg_udp_data_79_64, (reg32))

#define PON_SERDES_CTRL_14_GET_rg_afifo_w_thres_serdes(reg32)  REG_FLD_GET(PON_SERDES_CTRL_14_FLD_rg_afifo_w_thres_serdes, (reg32))
#define PON_SERDES_CTRL_14_GET_rg_afifo_r_thres_serdes(reg32)  REG_FLD_GET(PON_SERDES_CTRL_14_FLD_rg_afifo_r_thres_serdes, (reg32))

#define PON_SERDES_STS_1_GET_ro_debug_afifo_serdes(reg32)      REG_FLD_GET(PON_SERDES_STS_1_FLD_ro_debug_afifo_serdes, (reg32))

#define PON_SERDES_CTRL_15_GET_rg_ad_ben_out_clr(reg32)        REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_ad_ben_out_clr, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_ben_force_value(reg32)       REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_ben_force_value, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_ben_force_mode(reg32)        REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_ben_force_mode, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_path_rdy_mux(reg32)       REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_path_rdy_mux, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_data_rx_inv(reg32)           REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_data_rx_inv, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_cid_path_en(reg32)        REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_cid_path_en, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_prbs15_path_en(reg32)     REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs15_path_en, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_prbs9_path_en(reg32)      REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs9_path_en, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_prbs31_path_en(reg32)     REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs31_path_en, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_prbs23_path_en(reg32)     REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs23_path_en, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_prbs7_path_en(reg32)      REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs7_path_en, (reg32))
#define PON_SERDES_CTRL_15_GET_rg_rx_prbs_path_sel(reg32)      REG_FLD_GET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs_path_sel, (reg32))

#define PON_SERDES_CTRL_16_GET_rg_tx_end_dly_sel(reg32)        REG_FLD_GET(PON_SERDES_CTRL_16_FLD_rg_tx_end_dly_sel, (reg32))
#define PON_SERDES_CTRL_16_GET_rg_tx_start_dly_sel(reg32)      REG_FLD_GET(PON_SERDES_CTRL_16_FLD_rg_tx_start_dly_sel, (reg32))
#define PON_SERDES_CTRL_16_GET_rg_tx_bit_dly_mode(reg32)       REG_FLD_GET(PON_SERDES_CTRL_16_FLD_rg_tx_bit_dly_mode, (reg32))
#define PON_SERDES_CTRL_16_GET_rg_txbit_dly(reg32)             REG_FLD_GET(PON_SERDES_CTRL_16_FLD_rg_txbit_dly, (reg32))
#define PON_SERDES_CTRL_16_GET_rg_bit_delay_rg_mode(reg32)     REG_FLD_GET(PON_SERDES_CTRL_16_FLD_rg_bit_delay_rg_mode, (reg32))
#define PON_SERDES_CTRL_16_GET_rg_tx_ben_end_dly_mode(reg32)   REG_FLD_GET(PON_SERDES_CTRL_16_FLD_rg_tx_ben_end_dly_mode, (reg32))
#define PON_SERDES_CTRL_16_GET_rg_tx_ben_start_dly_mode(reg32) REG_FLD_GET(PON_SERDES_CTRL_16_FLD_rg_tx_ben_start_dly_mode, (reg32))

#define PON_SERDES_CTRL_17_GET_rg_tx_bit_end_dly(reg32)        REG_FLD_GET(PON_SERDES_CTRL_17_FLD_rg_tx_bit_end_dly, (reg32))
#define PON_SERDES_CTRL_17_GET_rg_tx_bit_start_dly(reg32)      REG_FLD_GET(PON_SERDES_CTRL_17_FLD_rg_tx_bit_start_dly, (reg32))

#define PON_SERDES_CTRL_18_GET_rg_force_rxcal_done_sel(reg32)  REG_FLD_GET(PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done_sel, (reg32))
#define PON_SERDES_CTRL_18_GET_rg_force_rxcal_done(reg32)      REG_FLD_GET(PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done, (reg32))
#define PON_SERDES_CTRL_18_GET_rg_rxcal_ne_cnt(reg32)          REG_FLD_GET(PON_SERDES_CTRL_18_FLD_rg_rxcal_ne_cnt, (reg32))
#define PON_SERDES_CTRL_18_GET_rg_force_l2ref_done_sel(reg32)  REG_FLD_GET(PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done_sel, (reg32))
#define PON_SERDES_CTRL_18_GET_rg_force_l2ref_done(reg32)      REG_FLD_GET(PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done, (reg32))
#define PON_SERDES_CTRL_18_GET_rg_l2ref_ne_cnt(reg32)          REG_FLD_GET(PON_SERDES_CTRL_18_FLD_rg_l2ref_ne_cnt, (reg32))

#define PON_SERDES_STS_2_GET_ro_rxcal_done(reg32)              REG_FLD_GET(PON_SERDES_STS_2_FLD_ro_rxcal_done, (reg32))
#define PON_SERDES_STS_2_GET_ro_l2ref_done(reg32)              REG_FLD_GET(PON_SERDES_STS_2_FLD_ro_l2ref_done, (reg32))

#define PON_SERDES_CTRL_19_GET_rg_20b_idle_code(reg32)         REG_FLD_GET(PON_SERDES_CTRL_19_FLD_rg_20b_idle_code, (reg32))

#define PON_SERDES_CTRL_20_GET_rg_lpf_en_to_ssc_dly_cnt(reg32) REG_FLD_GET(PON_SERDES_CTRL_20_FLD_rg_lpf_en_to_ssc_dly_cnt, (reg32))
#define PON_SERDES_CTRL_20_GET_rg_da_pll_en_dly_cnt(reg32)     REG_FLD_GET(PON_SERDES_CTRL_20_FLD_rg_da_pll_en_dly_cnt, (reg32))
#define PON_SERDES_CTRL_20_GET_rg_da_pll_en(reg32)             REG_FLD_GET(PON_SERDES_CTRL_20_FLD_rg_da_pll_en, (reg32))
#define PON_SERDES_CTRL_20_GET_rg_force_da_pll_en(reg32)       REG_FLD_GET(PON_SERDES_CTRL_20_FLD_rg_force_da_pll_en, (reg32))
#define PON_SERDES_CTRL_20_GET_rg_da_pll_en_mux(reg32)         REG_FLD_GET(PON_SERDES_CTRL_20_FLD_rg_da_pll_en_mux, (reg32))

#define PON_SERDES_CTRL_21_GET_rg_pll_vco_cplt(reg32)          REG_FLD_GET(PON_SERDES_CTRL_21_FLD_rg_pll_vco_cplt, (reg32))
#define PON_SERDES_CTRL_21_GET_rg_force_pll_vco_cplt(reg32)    REG_FLD_GET(PON_SERDES_CTRL_21_FLD_rg_force_pll_vco_cplt, (reg32))
#define PON_SERDES_CTRL_21_GET_rg_da_cdr_pwd(reg32)            REG_FLD_GET(PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd, (reg32))
#define PON_SERDES_CTRL_21_GET_rg_force_da_cdr_pwd(reg32)      REG_FLD_GET(PON_SERDES_CTRL_21_FLD_rg_force_da_cdr_pwd, (reg32))
#define PON_SERDES_CTRL_21_GET_rg_da_cdr_pwd_dly_cnt(reg32)    REG_FLD_GET(PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd_dly_cnt, (reg32))

#define PON_SERDES_CTRL_22_GET_rg_vcocal_fail(reg32)           REG_FLD_GET(PON_SERDES_CTRL_22_FLD_rg_vcocal_fail, (reg32))
#define PON_SERDES_CTRL_22_GET_rg_vcocal_dly_cnt(reg32)        REG_FLD_GET(PON_SERDES_CTRL_22_FLD_rg_vcocal_dly_cnt, (reg32))
#define PON_SERDES_CTRL_22_GET_rg_vcocal_state(reg32)          REG_FLD_GET(PON_SERDES_CTRL_22_FLD_rg_vcocal_state, (reg32))

#define PON_SYS_CTRL_0_GET_rg_xfi_rx_pma_clk_sel(reg32)        REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_xfi_rx_pma_clk_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_baser_tx_pma_clk_div_sel(reg32)  REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_baser_tx_pma_clk_div_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_tx_pma_clk_sel(reg32)            REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_tx_pma_clk_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_hg_tx_pma_clk_div_sel(reg32)     REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_hg_tx_pma_clk_div_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_pon_tx_pma_clk_div_sel(reg32)    REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_pon_tx_pma_clk_div_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_rx_pma_clk_sel(reg32)            REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_rx_pma_clk_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_hg_rx_pma_clk_div_sel(reg32)     REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_hg_rx_pma_clk_div_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_pon_rx_pma_clk_div_sel(reg32)    REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_pon_rx_pma_clk_div_sel, (reg32))
#define PON_SYS_CTRL_0_GET_rg_baser_rx_clk_inv(reg32)          REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_baser_rx_clk_inv, (reg32))
#define PON_SYS_CTRL_0_GET_rg_baser_tx_clk_inv(reg32)          REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_baser_tx_clk_inv, (reg32))
#define PON_SYS_CTRL_0_GET_rg_tx_clk_inv(reg32)                REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_tx_clk_inv, (reg32))
#define PON_SYS_CTRL_0_GET_rg_rx_clk_inv(reg32)                REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_rx_clk_inv, (reg32))
#define PON_SYS_CTRL_0_GET_rg_txben_clk_inv(reg32)             REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_txben_clk_inv, (reg32))
#define PON_SYS_CTRL_0_GET_rg_rx_short_ck_inv(reg32)           REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_rx_short_ck_inv, (reg32))
#define PON_SYS_CTRL_0_GET_rg_tx_short_ck_inv(reg32)           REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_tx_short_ck_inv, (reg32))
#define PON_SYS_CTRL_0_GET_rg_baser_rxck_en(reg32)             REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_baser_rxck_en, (reg32))
#define PON_SYS_CTRL_0_GET_rg_baser_txck_en(reg32)             REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_baser_txck_en, (reg32))
#define PON_SYS_CTRL_0_GET_rg_hg_rx500m_ck_en(reg32)           REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_hg_rx500m_ck_en, (reg32))
#define PON_SYS_CTRL_0_GET_rg_hg_tx250m_ck_en(reg32)           REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_hg_tx250m_ck_en, (reg32))
#define PON_SYS_CTRL_0_GET_rg_pon_rx500m_ck_en(reg32)          REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_pon_rx500m_ck_en, (reg32))
#define PON_SYS_CTRL_0_GET_rg_pon_tx250m_ck_en(reg32)          REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_pon_tx250m_ck_en, (reg32))
#define PON_SYS_CTRL_0_GET_rg_force_ge_mode(reg32)             REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_force_ge_mode, (reg32))
#define PON_SYS_CTRL_0_GET_rg_force_map_mode(reg32)            REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_force_map_mode, (reg32))
#define PON_SYS_CTRL_0_GET_rg_force_map_sel(reg32)             REG_FLD_GET(PON_SYS_CTRL_0_FLD_rg_force_map_sel, (reg32))

#define PON_SYS_CTRL_1_GET_rg_div_rst(reg32)                   REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_div_rst, (reg32))
#define PON_SYS_CTRL_1_GET_rg_rate_adp_rx_pma_fifo_rst_n(reg32) REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_pma_fifo_rst_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_rate_adp_rx_fifo_rst_n(reg32)    REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_fifo_rst_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_rate_adp_tx_fifo_rst_n(reg32)    REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_rate_adp_tx_fifo_rst_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_all_rx_rst_n(reg32)              REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_all_rx_rst_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_all_tx_rst_n(reg32)              REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_all_tx_rst_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_baser_rx_reset_n(reg32)          REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_baser_rx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_baser_tx_reset_n(reg32)          REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_baser_tx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_rx_fifo_rst(reg32)               REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_rx_fifo_rst, (reg32))
#define PON_SYS_CTRL_1_GET_rg_force_bg_tx_value(reg32)         REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_force_bg_tx_value, (reg32))
#define PON_SYS_CTRL_1_GET_rg_force_bg_tx_mode(reg32)          REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_force_bg_tx_mode, (reg32))
#define PON_SYS_CTRL_1_GET_rg_trans_rx_data_inv(reg32)         REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_trans_rx_data_inv, (reg32))
#define PON_SYS_CTRL_1_GET_rg_bridge_rx_reset_n(reg32)         REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_bridge_rx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_bridge_tx_reset_n(reg32)         REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_bridge_tx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_eq_rx250m_div_reset_n(reg32)     REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_eq_rx250m_div_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_eq_rx250m_reset_n(reg32)         REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_eq_rx250m_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_tx250m_short_reset_n(reg32)      REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_tx250m_short_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_tx250m_div_reset_n(reg32)        REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_tx250m_div_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_tx_reset_n(reg32)                REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_tx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_rx_reset_n(reg32)                REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_rx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_ref_reset_n(reg32)               REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_ref_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_pcstx_reset_n(reg32)             REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_pcstx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_pcsrx_reset_n(reg32)             REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_pcsrx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_pcstxrx_reset_n(reg32)           REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_pcstxrx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_mactx_reset_n(reg32)             REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_mactx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_macrx_reset_n(reg32)             REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_macrx_reset_n, (reg32))
#define PON_SYS_CTRL_1_GET_rg_mactxrx_reset_n(reg32)           REG_FLD_GET(PON_SYS_CTRL_1_FLD_rg_mactxrx_reset_n, (reg32))

#define PON_SYS_CTRL_2_GET_rg_1us_40m_cnt(reg32)               REG_FLD_GET(PON_SYS_CTRL_2_FLD_rg_1us_40m_cnt, (reg32))
#define PON_SYS_CTRL_2_GET_rg_1us_25m_cnt(reg32)               REG_FLD_GET(PON_SYS_CTRL_2_FLD_rg_1us_25m_cnt, (reg32))
#define PON_SYS_CTRL_2_GET_rg_pon_mbist_slow_mode_en(reg32)    REG_FLD_GET(PON_SYS_CTRL_2_FLD_rg_pon_mbist_slow_mode_en, (reg32))
#define PON_SYS_CTRL_2_GET_rg_pon_mbist_mode_en(reg32)         REG_FLD_GET(PON_SYS_CTRL_2_FLD_rg_pon_mbist_mode_en, (reg32))
#define PON_SYS_CTRL_2_GET_rg_xtal_mode(reg32)                 REG_FLD_GET(PON_SYS_CTRL_2_FLD_rg_xtal_mode, (reg32))
#define PON_SYS_CTRL_2_GET_rg_xtal_is_25m(reg32)               REG_FLD_GET(PON_SYS_CTRL_2_FLD_rg_xtal_is_25m, (reg32))
#define PON_SYS_CTRL_2_GET_rg_pon_probe_sel(reg32)             REG_FLD_GET(PON_SYS_CTRL_2_FLD_rg_pon_probe_sel, (reg32))

#define PON_CK_CTRL_0_GET_rg_rs_ck_cnt(reg32)                  REG_FLD_GET(PON_CK_CTRL_0_FLD_rg_rs_ck_cnt, (reg32))
#define PON_CK_CTRL_0_GET_rg_rs_ck_sel(reg32)                  REG_FLD_GET(PON_CK_CTRL_0_FLD_rg_rs_ck_sel, (reg32))
#define PON_CK_CTRL_0_GET_rg_rs_ck_mux(reg32)                  REG_FLD_GET(PON_CK_CTRL_0_FLD_rg_rs_ck_mux, (reg32))
#define PON_CK_CTRL_0_GET_rg_baser_rxck_mode(reg32)            REG_FLD_GET(PON_CK_CTRL_0_FLD_rg_baser_rxck_mode, (reg32))
#define PON_CK_CTRL_0_GET_rg_baser_txck_mode(reg32)            REG_FLD_GET(PON_CK_CTRL_0_FLD_rg_baser_txck_mode, (reg32))

#define HG_RST_CTRL_0_GET_rg_hsgmii_mac_rxck_free_sel(reg32)   REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hsgmii_mac_rxck_free_sel, (reg32))
#define HG_RST_CTRL_0_GET_rg_hsgmii_mac_txck_free_sel(reg32)   REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hsgmii_mac_txck_free_sel, (reg32))
#define HG_RST_CTRL_0_GET_rg_xfi_pcs_bg_lb_fifo_rst_n(reg32)   REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_xfi_pcs_bg_lb_fifo_rst_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_xfi_pcs_tx_gearbox_fifo_rst_n(reg32) REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_xfi_pcs_tx_gearbox_fifo_rst_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_xfi_pcs_rx_gearbox_fifo_rst_n(reg32) REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_xfi_pcs_rx_gearbox_fifo_rst_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_bypass_tx_re_en(reg32)            REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_bypass_tx_re_en, (reg32))
#define HG_RST_CTRL_0_GET_rg_hg_mac_rx_reset_n(reg32)          REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hg_mac_rx_reset_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_hg_mac_tx_reset_n(reg32)          REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hg_mac_tx_reset_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_hg_pcs_rx_reset_n(reg32)          REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hg_pcs_rx_reset_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_hg_pcs_tx_reset_n(reg32)          REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hg_pcs_tx_reset_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_hg_pma_rx_reset_n(reg32)          REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hg_pma_rx_reset_n, (reg32))
#define HG_RST_CTRL_0_GET_rg_hg_pma_tx_reset_n(reg32)          REG_FLD_GET(HG_RST_CTRL_0_FLD_rg_hg_pma_tx_reset_n, (reg32))

#define HG_MODE_CTRL_0_GET_rg_fpga_tx_swap(reg32)              REG_FLD_GET(HG_MODE_CTRL_0_FLD_rg_fpga_tx_swap, (reg32))
#define HG_MODE_CTRL_0_GET_rg_fpga_mode_ctrl(reg32)            REG_FLD_GET(HG_MODE_CTRL_0_FLD_rg_fpga_mode_ctrl, (reg32))
#define HG_MODE_CTRL_0_GET_rg_ip_reset_mode(reg32)             REG_FLD_GET(HG_MODE_CTRL_0_FLD_rg_ip_reset_mode, (reg32))
#define HG_MODE_CTRL_0_GET_rg_sgmii_mode(reg32)                REG_FLD_GET(HG_MODE_CTRL_0_FLD_rg_sgmii_mode, (reg32))
#define HG_MODE_CTRL_0_GET_rg_gpon_sym_mode(reg32)             REG_FLD_GET(HG_MODE_CTRL_0_FLD_rg_gpon_sym_mode, (reg32))
#define HG_MODE_CTRL_0_GET_rg_serdes_mode(reg32)               REG_FLD_GET(HG_MODE_CTRL_0_FLD_rg_serdes_mode, (reg32))

#define HG_RG_CTRL_0_GET_rg_eq_step1(reg32)                    REG_FLD_GET(HG_RG_CTRL_0_FLD_rg_eq_step1, (reg32))
#define HG_RG_CTRL_0_GET_rg_tx_dem(reg32)                      REG_FLD_GET(HG_RG_CTRL_0_FLD_rg_tx_dem, (reg32))
#define HG_RG_CTRL_0_GET_rg_ln0_rx_dac_pwd(reg32)              REG_FLD_GET(HG_RG_CTRL_0_FLD_rg_ln0_rx_dac_pwd, (reg32))
#define HG_RG_CTRL_0_GET_rg_rx_dac_pwd_mux(reg32)              REG_FLD_GET(HG_RG_CTRL_0_FLD_rg_rx_dac_pwd_mux, (reg32))

#define PON_STS_0_GET_ro_baser_mode(reg32)                     REG_FLD_GET(PON_STS_0_FLD_ro_baser_mode, (reg32))
#define PON_STS_0_GET_ro_hsgmii_mode(reg32)                    REG_FLD_GET(PON_STS_0_FLD_ro_hsgmii_mode, (reg32))
#define PON_STS_0_GET_ro_sgmii_mode(reg32)                     REG_FLD_GET(PON_STS_0_FLD_ro_sgmii_mode, (reg32))
#define PON_STS_0_GET_ro_epon_mode(reg32)                      REG_FLD_GET(PON_STS_0_FLD_ro_epon_mode, (reg32))
#define PON_STS_0_GET_ro_gpon_tmode(reg32)                     REG_FLD_GET(PON_STS_0_FLD_ro_gpon_tmode, (reg32))
#define PON_STS_0_GET_ro_gpon_mode(reg32)                      REG_FLD_GET(PON_STS_0_FLD_ro_gpon_mode, (reg32))

#define PON_OSR_SEL_CTRL_GET_rg_tx_eqd_mode(reg32)             REG_FLD_GET(PON_OSR_SEL_CTRL_FLD_rg_tx_eqd_mode, (reg32))
#define PON_OSR_SEL_CTRL_GET_rg_rx_osr_sel(reg32)              REG_FLD_GET(PON_OSR_SEL_CTRL_FLD_rg_rx_osr_sel, (reg32))
#define PON_OSR_SEL_CTRL_GET_rg_tx_osr_sel(reg32)              REG_FLD_GET(PON_OSR_SEL_CTRL_FLD_rg_tx_osr_sel, (reg32))

#define CDR_CTRL_0_GET_rg_bic_ltr_mode_1(reg32)                REG_FLD_GET(CDR_CTRL_0_FLD_rg_bic_ltr_mode_1, (reg32))
#define CDR_CTRL_0_GET_rg_bic_ltr_mode_0(reg32)                REG_FLD_GET(CDR_CTRL_0_FLD_rg_bic_ltr_mode_0, (reg32))
#define CDR_CTRL_0_GET_rg_bir_ltr_mode_1(reg32)                REG_FLD_GET(CDR_CTRL_0_FLD_rg_bir_ltr_mode_1, (reg32))
#define CDR_CTRL_0_GET_rg_bir_ltr_mode_0(reg32)                REG_FLD_GET(CDR_CTRL_0_FLD_rg_bir_ltr_mode_0, (reg32))

#define CDR_CTRL_1_GET_rg_kvsel_ltr_mode_1(reg32)              REG_FLD_GET(CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_1, (reg32))
#define CDR_CTRL_1_GET_rg_kvsel_ltr_mode_0(reg32)              REG_FLD_GET(CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_0, (reg32))
#define CDR_CTRL_1_GET_rg_br_ltr_mode_1(reg32)                 REG_FLD_GET(CDR_CTRL_1_FLD_rg_br_ltr_mode_1, (reg32))
#define CDR_CTRL_1_GET_rg_br_ltr_mode_0(reg32)                 REG_FLD_GET(CDR_CTRL_1_FLD_rg_br_ltr_mode_0, (reg32))
#define CDR_CTRL_1_GET_rg_bc_ltr_mode_1(reg32)                 REG_FLD_GET(CDR_CTRL_1_FLD_rg_bc_ltr_mode_1, (reg32))
#define CDR_CTRL_1_GET_rg_bc_ltr_mode_0(reg32)                 REG_FLD_GET(CDR_CTRL_1_FLD_rg_bc_ltr_mode_0, (reg32))

#define CDR_CTRL_2_GET_rg_bic_ltd0_mode_1(reg32)               REG_FLD_GET(CDR_CTRL_2_FLD_rg_bic_ltd0_mode_1, (reg32))
#define CDR_CTRL_2_GET_rg_bic_ltd0_mode_0(reg32)               REG_FLD_GET(CDR_CTRL_2_FLD_rg_bic_ltd0_mode_0, (reg32))
#define CDR_CTRL_2_GET_rg_bir_ltd0_mode_1(reg32)               REG_FLD_GET(CDR_CTRL_2_FLD_rg_bir_ltd0_mode_1, (reg32))
#define CDR_CTRL_2_GET_rg_bir_ltd0_mode_0(reg32)               REG_FLD_GET(CDR_CTRL_2_FLD_rg_bir_ltd0_mode_0, (reg32))

#define CDR_CTRL_3_GET_rg_kvsel_ltd0_mode_1(reg32)             REG_FLD_GET(CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_1, (reg32))
#define CDR_CTRL_3_GET_rg_kvsel_ltd0_mode_0(reg32)             REG_FLD_GET(CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_0, (reg32))
#define CDR_CTRL_3_GET_rg_br_ltd0_mode_1(reg32)                REG_FLD_GET(CDR_CTRL_3_FLD_rg_br_ltd0_mode_1, (reg32))
#define CDR_CTRL_3_GET_rg_br_ltd0_mode_0(reg32)                REG_FLD_GET(CDR_CTRL_3_FLD_rg_br_ltd0_mode_0, (reg32))
#define CDR_CTRL_3_GET_rg_bc_ltd0_mode_1(reg32)                REG_FLD_GET(CDR_CTRL_3_FLD_rg_bc_ltd0_mode_1, (reg32))
#define CDR_CTRL_3_GET_rg_bc_ltd0_mode_0(reg32)                REG_FLD_GET(CDR_CTRL_3_FLD_rg_bc_ltd0_mode_0, (reg32))

#define CDR_CTRL_4_GET_rg_bic_ltd1_mode_1(reg32)               REG_FLD_GET(CDR_CTRL_4_FLD_rg_bic_ltd1_mode_1, (reg32))
#define CDR_CTRL_4_GET_rg_bic_ltd1_mode_0(reg32)               REG_FLD_GET(CDR_CTRL_4_FLD_rg_bic_ltd1_mode_0, (reg32))
#define CDR_CTRL_4_GET_rg_bir_ltd1_mode_1(reg32)               REG_FLD_GET(CDR_CTRL_4_FLD_rg_bir_ltd1_mode_1, (reg32))
#define CDR_CTRL_4_GET_rg_bir_ltd1_mode_0(reg32)               REG_FLD_GET(CDR_CTRL_4_FLD_rg_bir_ltd1_mode_0, (reg32))

#define CDR_CTRL_5_GET_rg_kvsel_ltd1_mode_1(reg32)             REG_FLD_GET(CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_1, (reg32))
#define CDR_CTRL_5_GET_rg_kvsel_ltd1_mode_0(reg32)             REG_FLD_GET(CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_0, (reg32))
#define CDR_CTRL_5_GET_rg_br_ltd1_mode_1(reg32)                REG_FLD_GET(CDR_CTRL_5_FLD_rg_br_ltd1_mode_1, (reg32))
#define CDR_CTRL_5_GET_rg_br_ltd1_mode_0(reg32)                REG_FLD_GET(CDR_CTRL_5_FLD_rg_br_ltd1_mode_0, (reg32))
#define CDR_CTRL_5_GET_rg_bc_ltd1_mode_1(reg32)                REG_FLD_GET(CDR_CTRL_5_FLD_rg_bc_ltd1_mode_1, (reg32))
#define CDR_CTRL_5_GET_rg_bc_ltd1_mode_0(reg32)                REG_FLD_GET(CDR_CTRL_5_FLD_rg_bc_ltd1_mode_0, (reg32))

#define TX_CTRL_0_GET_rg_tx_ben_end_dly_sel(reg32)             REG_FLD_GET(TX_CTRL_0_FLD_rg_tx_ben_end_dly_sel, (reg32))
#define TX_CTRL_0_GET_rg_tx_ben_start_dly_sel(reg32)           REG_FLD_GET(TX_CTRL_0_FLD_rg_tx_ben_start_dly_sel, (reg32))
#define TX_CTRL_0_GET_rg_tx_ben_exten_ftune(reg32)             REG_FLD_GET(TX_CTRL_0_FLD_rg_tx_ben_exten_ftune, (reg32))
#define TX_CTRL_0_GET_rg_tx_dly_ben_ftune(reg32)               REG_FLD_GET(TX_CTRL_0_FLD_rg_tx_dly_ben_ftune, (reg32))
#define TX_CTRL_0_GET_rg_tx_dly_data_ftune(reg32)              REG_FLD_GET(TX_CTRL_0_FLD_rg_tx_dly_data_ftune, (reg32))

#define TX_CTRL_1_GET_rg_rx_non_reverse_epon(reg32)            REG_FLD_GET(TX_CTRL_1_FLD_rg_rx_non_reverse_epon, (reg32))
#define TX_CTRL_1_GET_rg_tx_non_reverse_epon(reg32)            REG_FLD_GET(TX_CTRL_1_FLD_rg_tx_non_reverse_epon, (reg32))
#define TX_CTRL_1_GET_rg_rx_non_reverse_gpon(reg32)            REG_FLD_GET(TX_CTRL_1_FLD_rg_rx_non_reverse_gpon, (reg32))
#define TX_CTRL_1_GET_rg_tx_non_reverse_gpon(reg32)            REG_FLD_GET(TX_CTRL_1_FLD_rg_tx_non_reverse_gpon, (reg32))
#define TX_CTRL_1_GET_rg_force_rx_pon_sel(reg32)               REG_FLD_GET(TX_CTRL_1_FLD_rg_force_rx_pon_sel, (reg32))
#define TX_CTRL_1_GET_rg_force_rx_pon_mode(reg32)              REG_FLD_GET(TX_CTRL_1_FLD_rg_force_rx_pon_mode, (reg32))
#define TX_CTRL_1_GET_rg_force_tx_gpon_sel(reg32)              REG_FLD_GET(TX_CTRL_1_FLD_rg_force_tx_gpon_sel, (reg32))
#define TX_CTRL_1_GET_rg_force_tx_gpon_mode(reg32)             REG_FLD_GET(TX_CTRL_1_FLD_rg_force_tx_gpon_mode, (reg32))
#define TX_CTRL_1_GET_rg_force_tx_epon_sel(reg32)              REG_FLD_GET(TX_CTRL_1_FLD_rg_force_tx_epon_sel, (reg32))
#define TX_CTRL_1_GET_rg_force_tx_epon_mode(reg32)             REG_FLD_GET(TX_CTRL_1_FLD_rg_force_tx_epon_mode, (reg32))

#define TX_CTRL_2_GET_rg_txben_bdly_mode(reg32)                REG_FLD_GET(TX_CTRL_2_FLD_rg_txben_bdly_mode, (reg32))
#define TX_CTRL_2_GET_rg_txdata_bdly_mode(reg32)               REG_FLD_GET(TX_CTRL_2_FLD_rg_txdata_bdly_mode, (reg32))
#define TX_CTRL_2_GET_rg_bit_delay_mode_sel(reg32)             REG_FLD_GET(TX_CTRL_2_FLD_rg_bit_delay_mode_sel, (reg32))
#define TX_CTRL_2_GET_rg_benoff_cnt_max(reg32)                 REG_FLD_GET(TX_CTRL_2_FLD_rg_benoff_cnt_max, (reg32))
#define TX_CTRL_2_GET_rg_txben_pre_post_zero_en(reg32)         REG_FLD_GET(TX_CTRL_2_FLD_rg_txben_pre_post_zero_en, (reg32))
#define TX_CTRL_2_GET_rg_hsdata_en_sel(reg32)                  REG_FLD_GET(TX_CTRL_2_FLD_rg_hsdata_en_sel, (reg32))
#define TX_CTRL_2_GET_rg_outben_data_mode(reg32)               REG_FLD_GET(TX_CTRL_2_FLD_rg_outben_data_mode, (reg32))

#define TX_CTRL_3_GET_rg_bist_prbs_update(reg32)               REG_FLD_GET(TX_CTRL_3_FLD_rg_bist_prbs_update, (reg32))
#define TX_CTRL_3_GET_rg_bist_prbs_init_seed_benoff(reg32)     REG_FLD_GET(TX_CTRL_3_FLD_rg_bist_prbs_init_seed_benoff, (reg32))

#define TX_CTRL_4_GET_rg_benoff_d0(reg32)                      REG_FLD_GET(TX_CTRL_4_FLD_rg_benoff_d0, (reg32))

#define TX_CTRL_5_GET_rg_benoff_d1(reg32)                      REG_FLD_GET(TX_CTRL_5_FLD_rg_benoff_d1, (reg32))

#define TX_CTRL_6_GET_rg_benoff_d2(reg32)                      REG_FLD_GET(TX_CTRL_6_FLD_rg_benoff_d2, (reg32))

#define TX_CTRL_7_GET_rg_benoff_d3(reg32)                      REG_FLD_GET(TX_CTRL_7_FLD_rg_benoff_d3, (reg32))

#define FRQ_CTRL_0_GET_rg_hg_rx_frq_ppm_thres(reg32)           REG_FLD_GET(FRQ_CTRL_0_FLD_rg_hg_rx_frq_ppm_thres, (reg32))
#define FRQ_CTRL_0_GET_rg_hg_tx_frq_ppm_thres(reg32)           REG_FLD_GET(FRQ_CTRL_0_FLD_rg_hg_tx_frq_ppm_thres, (reg32))
#define FRQ_CTRL_0_GET_rg_pon_rx_frq_ppm_thres(reg32)          REG_FLD_GET(FRQ_CTRL_0_FLD_rg_pon_rx_frq_ppm_thres, (reg32))
#define FRQ_CTRL_0_GET_rg_pon_tx_frq_ppm_thres(reg32)          REG_FLD_GET(FRQ_CTRL_0_FLD_rg_pon_tx_frq_ppm_thres, (reg32))

#define FRQ_CTRL_1_GET_rg_baser_rx_frq_ppm_thres(reg32)        REG_FLD_GET(FRQ_CTRL_1_FLD_rg_baser_rx_frq_ppm_thres, (reg32))
#define FRQ_CTRL_1_GET_rg_baser_tx_frq_ppm_thres(reg32)        REG_FLD_GET(FRQ_CTRL_1_FLD_rg_baser_tx_frq_ppm_thres, (reg32))

#define FRQ_CTRL_2_GET_rg_baser_rx_freq_det_mux(reg32)         REG_FLD_GET(FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_mux, (reg32))
#define FRQ_CTRL_2_GET_rg_baser_tx_freq_det_mux(reg32)         REG_FLD_GET(FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_mux, (reg32))
#define FRQ_CTRL_2_GET_rg_hg_rx_freq_det_mux(reg32)            REG_FLD_GET(FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_mux, (reg32))
#define FRQ_CTRL_2_GET_rg_hg_tx_freq_det_mux(reg32)            REG_FLD_GET(FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_mux, (reg32))
#define FRQ_CTRL_2_GET_rg_pon_rx_freq_det_mux(reg32)           REG_FLD_GET(FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_mux, (reg32))
#define FRQ_CTRL_2_GET_rg_pon_tx_freq_det_mux(reg32)           REG_FLD_GET(FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_mux, (reg32))
#define FRQ_CTRL_2_GET_rg_baser_rx_freq_det_en(reg32)          REG_FLD_GET(FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_en, (reg32))
#define FRQ_CTRL_2_GET_rg_baser_tx_freq_det_en(reg32)          REG_FLD_GET(FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_en, (reg32))
#define FRQ_CTRL_2_GET_rg_hg_rx_freq_det_en(reg32)             REG_FLD_GET(FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_en, (reg32))
#define FRQ_CTRL_2_GET_rg_hg_tx_freq_det_en(reg32)             REG_FLD_GET(FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_en, (reg32))
#define FRQ_CTRL_2_GET_rg_pon_rx_freq_det_en(reg32)            REG_FLD_GET(FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_en, (reg32))
#define FRQ_CTRL_2_GET_rg_pon_tx_freq_det_en(reg32)            REG_FLD_GET(FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_en, (reg32))

#define FRQ_CTRL_3_GET_rg_pon_rx_freq_cnt(reg32)               REG_FLD_GET(FRQ_CTRL_3_FLD_rg_pon_rx_freq_cnt, (reg32))
#define FRQ_CTRL_3_GET_rg_pon_tx_freq_cnt(reg32)               REG_FLD_GET(FRQ_CTRL_3_FLD_rg_pon_tx_freq_cnt, (reg32))

#define FRQ_CTRL_4_GET_rg_hg_rx_freq_cnt(reg32)                REG_FLD_GET(FRQ_CTRL_4_FLD_rg_hg_rx_freq_cnt, (reg32))
#define FRQ_CTRL_4_GET_rg_hg_tx_freq_cnt(reg32)                REG_FLD_GET(FRQ_CTRL_4_FLD_rg_hg_tx_freq_cnt, (reg32))

#define FRQ_CTRL_5_GET_rg_baser_rx_freq_cnt(reg32)             REG_FLD_GET(FRQ_CTRL_5_FLD_rg_baser_rx_freq_cnt, (reg32))
#define FRQ_CTRL_5_GET_rg_baser_tx_freq_cnt(reg32)             REG_FLD_GET(FRQ_CTRL_5_FLD_rg_baser_tx_freq_cnt, (reg32))

#define RX_FREQDET_0_GET_rg_rx_pll_unlock_cyclecnt(reg32)      REG_FLD_GET(RX_FREQDET_0_FLD_rg_rx_pll_unlock_cyclecnt, (reg32))
#define RX_FREQDET_0_GET_rg_rx_pll_lock_cyclecnt(reg32)        REG_FLD_GET(RX_FREQDET_0_FLD_rg_rx_pll_lock_cyclecnt, (reg32))

#define RX_FREQDET_1_GET_rg_rx_pll_lock_target_end(reg32)      REG_FLD_GET(RX_FREQDET_1_FLD_rg_rx_pll_lock_target_end, (reg32))
#define RX_FREQDET_1_GET_rg_rx_pll_lock_target_beg(reg32)      REG_FLD_GET(RX_FREQDET_1_FLD_rg_rx_pll_lock_target_beg, (reg32))

#define RX_FREQDET_2_GET_rg_rx_pll_unlock_target_end(reg32)    REG_FLD_GET(RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_end, (reg32))
#define RX_FREQDET_2_GET_rg_rx_pll_unlock_target_beg(reg32)    REG_FLD_GET(RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_beg, (reg32))

#define RX_FREQDET_3_GET_rg_force_rx_freq_det_en(reg32)        REG_FLD_GET(RX_FREQDET_3_FLD_rg_force_rx_freq_det_en, (reg32))
#define RX_FREQDET_3_GET_rg_rx_pll_wait_100us(reg32)           REG_FLD_GET(RX_FREQDET_3_FLD_rg_rx_pll_wait_100us, (reg32))
#define RX_FREQDET_3_GET_rg_rx_pll_unlockth(reg32)             REG_FLD_GET(RX_FREQDET_3_FLD_rg_rx_pll_unlockth, (reg32))
#define RX_FREQDET_3_GET_rg_rx_pll_lock_lockth(reg32)          REG_FLD_GET(RX_FREQDET_3_FLD_rg_rx_pll_lock_lockth, (reg32))
#define RX_FREQDET_3_GET_rg_rx_pll_lock_cnt_clear(reg32)       REG_FLD_GET(RX_FREQDET_3_FLD_rg_rx_pll_lock_cnt_clear, (reg32))
#define RX_FREQDET_3_GET_rg_rx_pll_freqlock_det_en(reg32)      REG_FLD_GET(RX_FREQDET_3_FLD_rg_rx_pll_freqlock_det_en, (reg32))

#define RX_FREQDET_STS_0_GET_ro_rx_prb_out_freqdet(reg32)      REG_FLD_GET(RX_FREQDET_STS_0_FLD_ro_rx_prb_out_freqdet, (reg32))
#define RX_FREQDET_STS_0_GET_ro_rx_state_freqdet(reg32)        REG_FLD_GET(RX_FREQDET_STS_0_FLD_ro_rx_state_freqdet, (reg32))
#define RX_FREQDET_STS_0_GET_ro_rx_fbck_lock_inner(reg32)      REG_FLD_GET(RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock_inner, (reg32))
#define RX_FREQDET_STS_0_GET_ro_rx_fbck_lock(reg32)            REG_FLD_GET(RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock, (reg32))
#define RX_FREQDET_STS_0_GET_ro_rx_lslock_cnt(reg32)           REG_FLD_GET(RX_FREQDET_STS_0_FLD_ro_rx_lslock_cnt, (reg32))
#define RX_FREQDET_STS_0_GET_ro_rx_fl_out(reg32)               REG_FLD_GET(RX_FREQDET_STS_0_FLD_ro_rx_fl_out, (reg32))

#define TX_CTRL_8_GET_rg_tx_test_mode(reg32)                   REG_FLD_GET(TX_CTRL_8_FLD_rg_tx_test_mode, (reg32))
#define TX_CTRL_8_GET_rg_tx_test_pattern(reg32)                REG_FLD_GET(TX_CTRL_8_FLD_rg_tx_test_pattern, (reg32))

#define TX_CTRL_9_GET_rg_ncpo_los_cnt(reg32)                   REG_FLD_GET(TX_CTRL_9_FLD_rg_ncpo_los_cnt, (reg32))
#define TX_CTRL_9_GET_rg_ncpo_los_hold_en(reg32)               REG_FLD_GET(TX_CTRL_9_FLD_rg_ncpo_los_hold_en, (reg32))
#define TX_CTRL_9_GET_rg_los_function_sel(reg32)               REG_FLD_GET(TX_CTRL_9_FLD_rg_los_function_sel, (reg32))
#define TX_CTRL_9_GET_rg_ncpo_los_pro_en(reg32)                REG_FLD_GET(TX_CTRL_9_FLD_rg_ncpo_los_pro_en, (reg32))
#define TX_CTRL_9_GET_rg_ncpo_los_inv(reg32)                   REG_FLD_GET(TX_CTRL_9_FLD_rg_ncpo_los_inv, (reg32))
#define TX_CTRL_9_GET_rg_ncpo_lof_en(reg32)                    REG_FLD_GET(TX_CTRL_9_FLD_rg_ncpo_lof_en, (reg32))

#define RX_FREQDET_DET_CTRL_0_GET_rg_rx_cdr_freq_chk_cycle(reg32) REG_FLD_GET(RX_FREQDET_DET_CTRL_0_FLD_rg_rx_cdr_freq_chk_cycle, (reg32))

#define RX_FREQDET_DET_CTRL_1_GET_rg_rx_cdr_freq_golden_cycle(reg32) REG_FLD_GET(RX_FREQDET_DET_CTRL_1_FLD_rg_rx_cdr_freq_golden_cycle, (reg32))

#define RX_FREQDET_DET_CTRL_2_GET_rg_rx_cdr_freq_tolerate_cycle(reg32) REG_FLD_GET(RX_FREQDET_DET_CTRL_2_FLD_rg_rx_cdr_freq_tolerate_cycle, (reg32))

#define RX_FREQDET_DET_CTRL_4_GET_ro_rx_cdr_freq_unlck_cnt(reg32) REG_FLD_GET(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_unlck_cnt, (reg32))
#define RX_FREQDET_DET_CTRL_4_GET_ro_rx_cdr_freq_state(reg32)  REG_FLD_GET(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_state, (reg32))
#define RX_FREQDET_DET_CTRL_4_GET_ro_rx_cdr_freq_lock(reg32)   REG_FLD_GET(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_lock, (reg32))
#define RX_FREQDET_DET_CTRL_4_GET_rg_rx_cdr_freq_relck_num(reg32) REG_FLD_GET(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_relck_num, (reg32))
#define RX_FREQDET_DET_CTRL_4_GET_rg_rx_cdr_freq_clear_unlck_cnt(reg32) REG_FLD_GET(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_clear_unlck_cnt, (reg32))
#define RX_FREQDET_DET_CTRL_4_GET_rg_rx_cdr_freq_det_en(reg32) REG_FLD_GET(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_det_en, (reg32))

#define PON_DA_MUX_CTRL_0_GET_rg_c_da_pll_bias_lpf_en(reg32)   REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_lpf_en, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_pll_bias_lpf_en(reg32)   REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_lpf_en, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_bias_pwd(reg32)          REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_bias_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_bias_pwd(reg32)          REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_bias_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_biasimr_pwd(reg32)       REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_biasimr_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_biasimr_pwd(reg32)       REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_biasimr_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_bg_lpf_en(reg32)         REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_bg_lpf_en, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_bg_lpf_en(reg32)         REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_bg_lpf_en, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_pll_bias_pwd(reg32)      REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_pll_bias_pwd(reg32)      REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_xtal_rx_pwd(reg32)       REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_xtal_rx_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_xtal_rx_pwd(reg32)       REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_xtal_rx_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_syspll_pwd(reg32)        REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_syspll_pwd(reg32)        REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_syspll_sdm_pwr_on(reg32) REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_pwr_on, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_syspll_sdm_pwr_on(reg32) REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_pwr_on, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_syspll_sdm_iso_en(reg32) REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_iso_en, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_syspll_sdm_iso_en(reg32) REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_iso_en, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_pll_pwd(reg32)           REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_pll_pwd(reg32)           REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_pwd, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_pll_autok_vco(reg32)     REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_autok_vco, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_pll_autok_vco(reg32)     REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_autok_vco, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_c_da_pll_ssc_en(reg32)        REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_ssc_en, (reg32))
#define PON_DA_MUX_CTRL_0_GET_rg_f_da_pll_ssc_en(reg32)        REG_FLD_GET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_ssc_en, (reg32))

#define PON_DA_MUX_CTRL_1_GET_rg_c_da_sigdet_cal_offset(reg32) REG_FLD_GET(PON_DA_MUX_CTRL_1_FLD_rg_c_da_sigdet_cal_offset, (reg32))
#define PON_DA_MUX_CTRL_1_GET_rg_f_da_sigdet_cal_offset(reg32) REG_FLD_GET(PON_DA_MUX_CTRL_1_FLD_rg_f_da_sigdet_cal_offset, (reg32))

#define PON_DA_MUX_CTRL_2_GET_rg_c_da_cdr_lpf_lck2ref(reg32)   REG_FLD_GET(PON_DA_MUX_CTRL_2_FLD_rg_c_da_cdr_lpf_lck2ref, (reg32))
#define PON_DA_MUX_CTRL_2_GET_rg_f_da_cdr_lpf_lck2ref(reg32)   REG_FLD_GET(PON_DA_MUX_CTRL_2_FLD_rg_f_da_cdr_lpf_lck2ref, (reg32))

#define PON_DA_MUX_CTRL_3_GET_rg_pon_tx_gpon_sel(reg32)        REG_FLD_GET(PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_sel, (reg32))
#define PON_DA_MUX_CTRL_3_GET_rg_pon_tx_gpon_mux(reg32)        REG_FLD_GET(PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_mux, (reg32))

#define PON_DA_MUX_CTRL_4_GET_rg_pma_rx_sigdet_pro(reg32)      REG_FLD_GET(PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_pro, (reg32))
#define PON_DA_MUX_CTRL_4_GET_rg_pma_rx_sigdet_en(reg32)       REG_FLD_GET(PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_en, (reg32))
#define PON_DA_MUX_CTRL_4_GET_rg_rx_sigdet_en(reg32)           REG_FLD_GET(PON_DA_MUX_CTRL_4_FLD_rg_rx_sigdet_en, (reg32))

#define PON_DA_MUX_CTRL_5_GET_rg_disb_da_pon_tx_data(reg32)    REG_FLD_GET(PON_DA_MUX_CTRL_5_FLD_rg_disb_da_pon_tx_data, (reg32))
#define PON_DA_MUX_CTRL_5_GET_rg_force_da_pon_tx_data(reg32)   REG_FLD_GET(PON_DA_MUX_CTRL_5_FLD_rg_force_da_pon_tx_data, (reg32))

#define PON_MODE_FORCE_CTRL_0_GET_rg_force_ge_rxpcsmodesel(reg32) REG_FLD_GET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxpcsmodesel, (reg32))
#define PON_MODE_FORCE_CTRL_0_GET_rg_force_ge_txpcsmodesel(reg32) REG_FLD_GET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txpcsmodesel, (reg32))
#define PON_MODE_FORCE_CTRL_0_GET_rg_force_ge_rxmapmodesel(reg32) REG_FLD_GET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxmapmodesel, (reg32))
#define PON_MODE_FORCE_CTRL_0_GET_rg_force_ge_txmapmodesel(reg32) REG_FLD_GET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txmapmodesel, (reg32))

#define CLK_GEN_FORCE_CTRL_0_GET_rg_or_baser_rxck(reg32)       REG_FLD_GET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_rxck, (reg32))
#define CLK_GEN_FORCE_CTRL_0_GET_rg_or_baser_txck(reg32)       REG_FLD_GET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_txck, (reg32))
#define CLK_GEN_FORCE_CTRL_0_GET_rg_or_hg_rx500m_ck(reg32)     REG_FLD_GET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_rx500m_ck, (reg32))
#define CLK_GEN_FORCE_CTRL_0_GET_rg_or_pon_rx500m_ck(reg32)    REG_FLD_GET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_rx500m_ck, (reg32))
#define CLK_GEN_FORCE_CTRL_0_GET_rg_or_sh_tx250m_ck(reg32)     REG_FLD_GET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_sh_tx250m_ck, (reg32))
#define CLK_GEN_FORCE_CTRL_0_GET_rg_or_hg_tx250m_ck(reg32)     REG_FLD_GET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_tx250m_ck, (reg32))
#define CLK_GEN_FORCE_CTRL_0_GET_rg_or_pon_tx250m_ck(reg32)    REG_FLD_GET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_tx250m_ck, (reg32))

#define ANALOG_FORCE_CTRL_0_GET_rg_sigdet_force_value(reg32)   REG_FLD_GET(ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_value, (reg32))
#define ANALOG_FORCE_CTRL_0_GET_rg_sigdet_force_mode(reg32)    REG_FLD_GET(ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_mode, (reg32))

#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_RX_DFE_RST(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_RST, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_RX_DFE_EN(reg32)       REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_EN, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_RX_FR_MODE(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_FR_MODE, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_FORCE_RX_FR_MODE(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_FORCE_RX_FR_MODE, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_TRI_DET_EN(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_TRI_DET_EN, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_STOPTIME(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_STOPTIME, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_RX500M_CK_SEL(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_RX500M_CK_SEL, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_PILPO_ROUT(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_PILPO_ROUT, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_GATED_RXD_B(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_GATED_RXD_B, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_EYE_MON_EN(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_MON_EN, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_EYE_CNT_EN(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_CNT_EN, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_DFE_TOG(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFE_TOG, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_DFEYEN_STOP_DIS(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEYEN_STOP_DIS, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_DFEX_RST(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_RST, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_DFEX_EN(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_EN, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_DFEX_DIS(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_DIS, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_DFEXEN_SEL(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEXEN_SEL, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_CHK_EYE_H(reg32)    REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_CHK_EYE_H, (reg32))
#define PON_RXFEDIG_CTRL_0_GET_RG_SSUSB_EQ_ALGRTHM_SEL(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_ALGRTHM_SEL, (reg32))

#define PON_RXFEDIG_CTRL_1_GET_RG_SSUSB_EQ_DD1LOS_LFI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1LOS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_1_GET_RG_SSUSB_EQ_DD1HOS_LFI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1HOS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_1_GET_RG_SSUSB_EQ_DD0LOS_LFI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0LOS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_1_GET_RG_SSUSB_EQ_DD0HOS_LFI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0HOS_LFI, (reg32))

#define PON_RXFEDIG_CTRL_2_GET_RG_SSUSB_EQ_DEYE1OS_LFI(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE1OS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_2_GET_RG_SSUSB_EQ_DEYE0OS_LFI(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE0OS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_2_GET_RG_SSUSB_EQ_DE1OS_LFI(reg32)    REG_FLD_GET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE1OS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_2_GET_RG_SSUSB_EQ_DE0OS_LFI(reg32)    REG_FLD_GET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE0OS_LFI, (reg32))

#define PON_RXFEDIG_CTRL_3_GET_RG_SSUSB_EQ_DHHL_LFI(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHL_LFI, (reg32))
#define PON_RXFEDIG_CTRL_3_GET_RG_SSUSB_EQ_DHHLOS_LFI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHLOS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_3_GET_RG_SSUSB_EQ_DFEYEN_DUR(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEYEN_DUR, (reg32))
#define PON_RXFEDIG_CTRL_3_GET_RG_SSUSB_EQ_DFEX_LF_SEL(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEX_LF_SEL, (reg32))
#define PON_RXFEDIG_CTRL_3_GET_RG_SSUSB_EQ_DFEXEN_DUR(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEXEN_DUR, (reg32))

#define PON_RXFEDIG_CTRL_4_GET_RG_SSUSB_EQ_DLEQ_LFI_GEN3(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN3, (reg32))
#define PON_RXFEDIG_CTRL_4_GET_RG_SSUSB_EQ_DLEQ_LFI_GEN2(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN2, (reg32))
#define PON_RXFEDIG_CTRL_4_GET_RG_SSUSB_EQ_DLEQ_LFI_GEN1(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN1, (reg32))
#define PON_RXFEDIG_CTRL_4_GET_RG_SSUSB_EQ_DLEQOS_LFI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQOS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_4_GET_RG_SSUSB_EQ_DHHL_LF_SEL(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DHHL_LF_SEL, (reg32))

#define PON_RXFEDIG_CTRL_5_GET_RG_SSUSB_EQ_DSAOS_LF_SEL(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DSAOS_LF_SEL, (reg32))
#define PON_RXFEDIG_CTRL_5_GET_RG_SSUSB_EQ_DLHL_LF_SEL(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LF_SEL, (reg32))
#define PON_RXFEDIG_CTRL_5_GET_RG_SSUSB_EQ_DLHL_LFI(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LFI, (reg32))
#define PON_RXFEDIG_CTRL_5_GET_RG_SSUSB_EQ_DLHLOS_LFI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHLOS_LFI, (reg32))
#define PON_RXFEDIG_CTRL_5_GET_RG_SSUSB_EQ_DLEQ_LF_SEL(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LF_SEL, (reg32))
#define PON_RXFEDIG_CTRL_5_GET_RG_SSUSB_EQ_DLEQ_LFI_GEN4(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN4, (reg32))

#define PON_RXFEDIG_CTRL_6_GET_RG_SSUSB_EQ_EYE1_Y(reg32)       REG_FLD_GET(PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE1_Y, (reg32))
#define PON_RXFEDIG_CTRL_6_GET_RG_SSUSB_EQ_EYE0_Y(reg32)       REG_FLD_GET(PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE0_Y, (reg32))

#define PON_RXFEDIG_CTRL_7_GET_RG_SSUSB_EQ_EYE_CNT(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_7_FLD_RG_SSUSB_EQ_EYE_CNT, (reg32))

#define PON_RXFEDIG_CTRL_8_GET_RG_SSUSB_EQ_LEQMAX(reg32)       REG_FLD_GET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_LEQMAX, (reg32))
#define PON_RXFEDIG_CTRL_8_GET_RG_SSUSB_EQ_ISIFLAG_SEL(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_ISIFLAG_SEL, (reg32))
#define PON_RXFEDIG_CTRL_8_GET_RG_SSUSB_EQ_EYE_XOFFSET(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_XOFFSET, (reg32))
#define PON_RXFEDIG_CTRL_8_GET_RG_SSUSB_EQ_EYE_MASK(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_MASK, (reg32))

#define PON_RXFEDIG_CTRL_9_GET_RG_SSUSB_EQ_PI0_INI(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_PI0_INI, (reg32))
#define PON_RXFEDIG_CTRL_9_GET_RG_SSUSB_EQ_MON_SEL(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_MON_SEL, (reg32))
#define PON_RXFEDIG_CTRL_9_GET_RG_SSUSB_EQ_LEQ_STOP_TO(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_STOP_TO, (reg32))
#define PON_RXFEDIG_CTRL_9_GET_RG_SSUSB_EQ_LEQ_SHIFT(reg32)    REG_FLD_GET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_SHIFT, (reg32))
#define PON_RXFEDIG_CTRL_9_GET_RG_SSUSB_EQ_LEQOSC_DLYCNT(reg32) REG_FLD_GET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQOSC_DLYCNT, (reg32))

#define PON_RXFEDIG_CTRL_10_GET_RG_SSUSB_EQ_PI_KPGAIN(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI_KPGAIN, (reg32))
#define PON_RXFEDIG_CTRL_10_GET_RG_SSUSB_EQ_PIEYE_INI(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PIEYE_INI, (reg32))
#define PON_RXFEDIG_CTRL_10_GET_RG_SSUSB_EQ_PI90_INI(reg32)    REG_FLD_GET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90_INI, (reg32))
#define PON_RXFEDIG_CTRL_10_GET_RG_SSUSB_EQ_PI90CK_SEL(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90CK_SEL, (reg32))

#define PON_RXFEDIG_CTRL_11_GET_RG_SSUSB_EQ_RESERVED(reg32)    REG_FLD_GET(PON_RXFEDIG_CTRL_11_FLD_RG_SSUSB_EQ_RESERVED, (reg32))

#define PON_RXFEDIG_CTRL_12_GET_RG_SSUSB_EQ_REV(reg32)         REG_FLD_GET(PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV, (reg32))
#define PON_RXFEDIG_CTRL_12_GET_RG_SSUSB_EQ_REV_1(reg32)       REG_FLD_GET(PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV_1, (reg32))

#define PON_RXFEDIG_CTRL_13_GET_RG_SSUSB_EQ_STARTTIME(reg32)   REG_FLD_GET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_STARTTIME, (reg32))
#define PON_RXFEDIG_CTRL_13_GET_RG_SSUSB_EQ_SIGDET(reg32)      REG_FLD_GET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SIGDET, (reg32))
#define PON_RXFEDIG_CTRL_13_GET_RG_SSUSB_EQ_SD_CNT1(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT1, (reg32))
#define PON_RXFEDIG_CTRL_13_GET_RG_SSUSB_EQ_SD_CNT0(reg32)     REG_FLD_GET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT0, (reg32))

#define PON_RXFEDIG_CTRL_14_GET_RG_SSUSB_EQ_TRI_DET_TH(reg32)  REG_FLD_GET(PON_RXFEDIG_CTRL_14_FLD_RG_SSUSB_EQ_TRI_DET_TH, (reg32))

#define PON_RXFEDIG_STS_0_GET_RGS_SSUSB_EQ_LEQ_STOP(reg32)     REG_FLD_GET(PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_LEQ_STOP, (reg32))
#define PON_RXFEDIG_STS_0_GET_RGS_SSUSB_EQ_EYE_CNT_RDY(reg32)  REG_FLD_GET(PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_EYE_CNT_RDY, (reg32))

#define PON_RXFEDIG_STS_1_GET_RGS_SSUSB_EQ_DCD1L(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1L, (reg32))
#define PON_RXFEDIG_STS_1_GET_RGS_SSUSB_EQ_DCD1H(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1H, (reg32))
#define PON_RXFEDIG_STS_1_GET_RGS_SSUSB_EQ_DCD0L(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0L, (reg32))
#define PON_RXFEDIG_STS_1_GET_RGS_SSUSB_EQ_DCD0H(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0H, (reg32))

#define PON_RXFEDIG_STS_2_GET_RGS_SSUSB_EQ_DCEYE1(reg32)       REG_FLD_GET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE1, (reg32))
#define PON_RXFEDIG_STS_2_GET_RGS_SSUSB_EQ_DCEYE0(reg32)       REG_FLD_GET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE0, (reg32))
#define PON_RXFEDIG_STS_2_GET_RGS_SSUSB_EQ_DCE1(reg32)         REG_FLD_GET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE1, (reg32))
#define PON_RXFEDIG_STS_2_GET_RGS_SSUSB_EQ_DCE0(reg32)         REG_FLD_GET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE0, (reg32))

#define PON_RXFEDIG_STS_3_GET_RGS_SSUSB_EQ_DCLHL(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLHL, (reg32))
#define PON_RXFEDIG_STS_3_GET_RGS_SSUSB_EQ_DCLEQOS(reg32)      REG_FLD_GET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQOS, (reg32))
#define PON_RXFEDIG_STS_3_GET_RGS_SSUSB_EQ_DCLEQ(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQ, (reg32))
#define PON_RXFEDIG_STS_3_GET_RGS_SSUSB_EQ_DCHHL(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCHHL, (reg32))

#define PON_RXFEDIG_STS_4_GET_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0(reg32) REG_FLD_GET(PON_RXFEDIG_STS_4_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0, (reg32))

#define PON_RXFEDIG_STS_5_GET_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1(reg32) REG_FLD_GET(PON_RXFEDIG_STS_5_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1, (reg32))

#define PON_RXFEDIG_STS_7_GET_RGS_SSUSB_EQ_STATUS(reg32)       REG_FLD_GET(PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_STATUS, (reg32))
#define PON_RXFEDIG_STS_7_GET_RGS_SSUSB_EQ_PILPO(reg32)        REG_FLD_GET(PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_PILPO, (reg32))

#define PON_RXFEDIG_STS_8_GET_RGS_SSUSB_RX_SD_OUT(reg32)       REG_FLD_GET(PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_RX_SD_OUT, (reg32))
#define PON_RXFEDIG_STS_8_GET_RGS_SSUSB_EQ_PROBE_OUT(reg32)    REG_FLD_GET(PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_EQ_PROBE_OUT, (reg32))

#define PON_DA_CTRL_0_GET_RG_SSUSB_PLL_IC(reg32)               REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IC, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_PLL_BR(reg32)               REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BR, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_PLL_IR(reg32)               REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IR, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_PLL_BP(reg32)               REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BP, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_PLL_LOAD_EN(reg32)          REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_LOAD_EN, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_PLL_FBKSEL(reg32)           REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_FBKSEL, (reg32))
#define PON_DA_CTRL_0_GET_RG_PCIE_CKDET_EN(reg32)              REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_PCIE_CKDET_EN, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_SCAN_MODE(reg32)            REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_SCAN_MODE, (reg32))
#define PON_DA_CTRL_0_GET_RG_PCIE_CLKRX_EN(reg32)              REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_PCIE_CLKRX_EN, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_XTAL_EXT_EN(reg32)          REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_XTAL_EXT_EN, (reg32))
#define PON_DA_CTRL_0_GET_RG_PCIE_DEVICE(reg32)                REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_PCIE_DEVICE, (reg32))
#define PON_DA_CTRL_0_GET_RG_PCIE_REFCK_DIV4(reg32)            REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_PCIE_REFCK_DIV4, (reg32))
#define PON_DA_CTRL_0_GET_RG_SSUSB_RX_IMPCALIB_EN(reg32)       REG_FLD_GET(PON_DA_CTRL_0_FLD_RG_SSUSB_RX_IMPCALIB_EN, (reg32))

#define PON_DA_CTRL_1_GET_RG_SSUSB_CDR_BPB(reg32)              REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_CDR_BPB, (reg32))
#define PON_DA_CTRL_1_GET_RG_SSUSB_TX_IMPCALIB_EN(reg32)       REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_TX_IMPCALIB_EN, (reg32))
#define PON_DA_CTRL_1_GET_RG_SSUSB_TX_DEM(reg32)               REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_TX_DEM, (reg32))
#define PON_DA_CTRL_1_GET_RG_SSUSB_RXDET_EN(reg32)             REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_RXDET_EN, (reg32))
#define PON_DA_CTRL_1_GET_RG_SSUSB_PLL_REFCKDIV(reg32)         REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_REFCKDIV, (reg32))
#define PON_DA_CTRL_1_GET_RG_SSUSB_PLL_PREDIV(reg32)           REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_PREDIV, (reg32))
#define PON_DA_CTRL_1_GET_RG_SSUSB_PLL_BC(reg32)               REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_BC, (reg32))
#define PON_DA_CTRL_1_GET_RG_SSUSB_PLL_DIVEN(reg32)            REG_FLD_GET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_DIVEN, (reg32))

#define PON_DA_CTRL_2_GET_RG_SSUSB_EQ_RSTEP1(reg32)            REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP1, (reg32))
#define PON_DA_CTRL_2_GET_RG_SSUSB_EQ_RSTEP2(reg32)            REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP2, (reg32))
#define PON_DA_CTRL_2_GET_RG_SSUSB_LFPS_PWD(reg32)             REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_LFPS_PWD, (reg32))
#define PON_DA_CTRL_2_GET_RG_SSUSB_CDR_PI_PWD(reg32)           REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_PWD, (reg32))
#define PON_DA_CTRL_2_GET_RG_SSUSB_CDR_PI_MODE(reg32)          REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_MODE, (reg32))
#define PON_DA_CTRL_2_GET_RG_SSUSB_CDR_TCADJ(reg32)            REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_TCADJ, (reg32))
#define PON_DA_CTRL_2_GET_RG_SSUSB_CDR_FBDIV(reg32)            REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_FBDIV, (reg32))
#define PON_DA_CTRL_2_GET_RG_SSUSB_CDR_BPA(reg32)              REG_FLD_GET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_BPA, (reg32))

#define PON_DA_CTRL_3_GET_RG_SSUSB_SYSPLL_FBDIV_CHG(reg32)     REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_SSUSB_SYSPLL_FBDIV_CHG, (reg32))
#define PON_DA_CTRL_3_GET_RG_PCIE_CLKTX_EN(reg32)              REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_PCIE_CLKTX_EN, (reg32))
#define PON_DA_CTRL_3_GET_RG_PCIE_CRSDET_EN(reg32)             REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_EN, (reg32))
#define PON_DA_CTRL_3_GET_RG_PCIE_CRSDET_RSTB(reg32)           REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_RSTB, (reg32))
#define PON_DA_CTRL_3_GET_RG_PCIE_CRSDET_OFST(reg32)           REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_OFST, (reg32))
#define PON_DA_CTRL_3_GET_RG_SSUSB_RX_HZ(reg32)                REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_HZ, (reg32))
#define PON_DA_CTRL_3_GET_RG_SSUSB_RX_P3_ENTRY(reg32)          REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P3_ENTRY, (reg32))
#define PON_DA_CTRL_3_GET_RG_SSUSB_RX_P1_ENTRY(reg32)          REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P1_ENTRY, (reg32))
#define PON_DA_CTRL_3_GET_RG_SSUSB_LFPS_DEGLITCH(reg32)        REG_FLD_GET(PON_DA_CTRL_3_FLD_RG_SSUSB_LFPS_DEGLITCH, (reg32))

#define PON_DA_CTRL_4_GET_RG_SSUSB_PLL_FBKDIV(reg32)           REG_FLD_GET(PON_DA_CTRL_4_FLD_RG_SSUSB_PLL_FBKDIV, (reg32))

#define PON_DA_CTRL_5_GET_RG_FORCE_CDR_PI0(reg32)              REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PI0, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_SYSPLL_SDM_PWR_ON(reg32)    REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_PWR_ON, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_SYSPLL_SDM_ISO_EN(reg32)    REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_ISO_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_CDR_PWD(reg32)              REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_CDR_AUTOK_VCO(reg32)        REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_AUTOK_VCO, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_CDR_LCK2REF(reg32)          REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_LCK2REF, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_RX_CMPWD(reg32)             REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_CMPWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_RX_AFE_PWD(reg32)           REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_AFE_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_TX_DATA_EN(reg32)           REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_DATA_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_TX_EIDLE_LP_EN(reg32)       REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_EIDLE_LP_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_TX_SER_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_SER_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_SIGDET_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_SIGDET_CAL_EN(reg32)        REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_SIGDET_CAL_OFFSET(reg32)    REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_OFFSET, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_RX_PI_CAL_EN(reg32)         REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_PI_CAL_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_TX_DRV_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_DRV_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_TX_LFPS(reg32)              REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_TX_LFPS_EN(reg32)           REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_RX_SAOSC_EN(reg32)          REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_SAOSC_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_BIAS_PWD(reg32)             REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_BIAS_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_BIASIMR_PWD(reg32)          REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_BIASIMR_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_BG_LPF_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_BG_LPF_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_PLL_BIAS_PWD(reg32)         REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_XTAL_RX_PWD(reg32)          REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_XTAL_RX_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_SYSPLL_PWD(reg32)           REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_PLL_PWD(reg32)              REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_PWD, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_PLL_AUTOK_VCO(reg32)        REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_AUTOK_VCO, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_PLL_SSC_EN(reg32)           REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_SSC_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_PLL_BIAS_LPF_EN(reg32)      REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_LPF_EN, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_CDR_BAND(reg32)             REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_BAND, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_PLL_BAND(reg32)             REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BAND, (reg32))
#define PON_DA_CTRL_5_GET_RG_FORCE_RX_LEQOS_EN(reg32)          REG_FLD_GET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_LEQOS_EN, (reg32))

#define PON_DA_CTRL_6_GET_RG_FORCE_IEXT_INTR_CTRL(reg32)       REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_IEXT_INTR_CTRL, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_RX_IMP_SEL(reg32)           REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_IMP_SEL, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_TX_IMP_SEL(reg32)           REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_IMP_SEL, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_RX_EQ_EN(reg32)             REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_EN, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_RX_EQ_RST(reg32)            REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_RST, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_RX_SAOSC_RST(reg32)         REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_SAOSC_RST, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_RX_CDR_STABLE(reg32)        REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_CDR_STABLE, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_CDR_KVSEL(reg32)            REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_KVSEL, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_CDR_BR(reg32)               REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BR, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_CDR_BC(reg32)               REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BC, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_CDR_BIC(reg32)              REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIC, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_CDR_BIR(reg32)              REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIR, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_TX_EIDLE_CM(reg32)          REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_EIDLE_CM, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_TX_BIASI_EN(reg32)          REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI_EN, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_TX_BIASI(reg32)             REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_IDEM_6DB(reg32)             REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_6DB, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_IDRV_6DB(reg32)             REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_6DB, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_IDEM_3P5DB(reg32)           REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_3P5DB, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_IDRV_3P5DB(reg32)           REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_3P5DB, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_IDRV_0DB(reg32)             REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_0DB, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_SYSPLL_PREDIV(reg32)        REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_PREDIV, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_SYSPLL_RESERVE(reg32)       REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_RESERVE, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_SYSPLL_FBDIV(reg32)         REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_FBDIV, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_PLL_SSC_PRD(reg32)          REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_PRD, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_PLL_SSC_DELTA(reg32)        REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_PLL_SSC_DELTA1(reg32)       REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA1, (reg32))
#define PON_DA_CTRL_6_GET_RG_FORCE_CDR_PI90(reg32)             REG_FLD_GET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_PI90, (reg32))

#define PON_DA_CTRL_7_GET_RG_SSUSB_BG_LPF_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_BG_LPF_EN, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_PLL_BIAS_PWD(reg32)         REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_PWD, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_XTAL_RX_PWD(reg32)          REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_XTAL_RX_PWD, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_SYSPLL_PWD(reg32)           REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_SYSPLL_PWD, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_PLL_PWD(reg32)              REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_PWD, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_PLL_AUTOK_VCO(reg32)        REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_AUTOK_VCO, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_PLL_SSC_EN(reg32)           REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_SSC_EN, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_PLL_BIAS_LPF_EN(reg32)      REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_LPF_EN, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_CDR_BAND(reg32)             REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_CDR_BAND, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_RX_LEQOS_EN(reg32)          REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_RX_LEQOS_EN, (reg32))
#define PON_DA_CTRL_7_GET_RG_SSUSB_PLL_BAND(reg32)             REG_FLD_GET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BAND, (reg32))

#define PON_DA_CTRL_8_GET_RG_SSUSB_SYSPLL_SDM_PWR_ON(reg32)    REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_PWR_ON, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_SYSPLL_SDM_ISO_EN(reg32)    REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_ISO_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_CDR_PWD(reg32)              REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_PWD, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_CDR_AUTOK_VCO(reg32)        REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_AUTOK_VCO, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_CDR_LCK2REF(reg32)          REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_LCK2REF, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_RX_CMPWD(reg32)             REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_CMPWD, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_RX_AFE_PWD(reg32)           REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_AFE_PWD, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_TX_DATA_EN(reg32)           REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DATA_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_TX_EIDLE_LP_EN(reg32)       REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_EIDLE_LP_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_TX_SER_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_SER_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_PCIE_SIGDET_EN(reg32)             REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_PCIE_SIGDET_CAL_EN(reg32)         REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_CAL_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_SIGDET_CAL_OFFSET(reg32)    REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_SIGDET_CAL_OFFSET, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_RX_PI_CAL_EN(reg32)         REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_PI_CAL_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_TX_DRV_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DRV_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_TX_LFPS(reg32)              REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_TX_LFPS_EN(reg32)           REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_RX_SAOSC_EN(reg32)          REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_SAOSC_EN, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_BIAS_PWD(reg32)             REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_BIAS_PWD, (reg32))
#define PON_DA_CTRL_8_GET_RG_SSUSB_BIASIMR_PWD(reg32)          REG_FLD_GET(PON_DA_CTRL_8_FLD_RG_SSUSB_BIASIMR_PWD, (reg32))

#define PON_DA_CTRL_9_GET_RG_SSUSB_CDR_PI90(reg32)             REG_FLD_GET(PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI90, (reg32))
#define PON_DA_CTRL_9_GET_RG_SSUSB_CDR_PI0(reg32)              REG_FLD_GET(PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI0, (reg32))

#define PON_DA_CTRL_10_GET_RG_SSUSB_PLL_SSC_DELTA(reg32)       REG_FLD_GET(PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA, (reg32))
#define PON_DA_CTRL_10_GET_RG_SSUSB_PLL_SSC_DELTA1(reg32)      REG_FLD_GET(PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA1, (reg32))

#define PON_DA_CTRL_11_GET_RG_SSUSB_SYSPLL_FBDIV(reg32)        REG_FLD_GET(PON_DA_CTRL_11_FLD_RG_SSUSB_SYSPLL_FBDIV, (reg32))

#define PON_DA_CTRL_12_GET_RG_SSUSB_IDRV_3P5DB(reg32)          REG_FLD_GET(PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_3P5DB, (reg32))
#define PON_DA_CTRL_12_GET_RG_SSUSB_IDRV_0DB(reg32)            REG_FLD_GET(PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_0DB, (reg32))

#define PON_DA_CTRL_13_GET_RG_SSUSB_TX_BIASI_EN(reg32)         REG_FLD_GET(PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI_EN, (reg32))
#define PON_DA_CTRL_13_GET_RG_SSUSB_TX_BIASI(reg32)            REG_FLD_GET(PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI, (reg32))
#define PON_DA_CTRL_13_GET_RG_SSUSB_IDEM_6DB(reg32)            REG_FLD_GET(PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_6DB, (reg32))
#define PON_DA_CTRL_13_GET_RG_SSUSB_IDRV_6DB(reg32)            REG_FLD_GET(PON_DA_CTRL_13_FLD_RG_SSUSB_IDRV_6DB, (reg32))
#define PON_DA_CTRL_13_GET_RG_SSUSB_IDEM_3P5DB(reg32)          REG_FLD_GET(PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_3P5DB, (reg32))

#define PON_DA_CTRL_14_GET_RG_SSUSB_RX_IMP_SEL(reg32)          REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_IMP_SEL, (reg32))
#define PON_DA_CTRL_14_GET_RG_SSUSB_TX_IMP_SEL(reg32)          REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_TX_IMP_SEL, (reg32))
#define PON_DA_CTRL_14_GET_RG_SSUSB_RX_EQ_EN(reg32)            REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_EN, (reg32))
#define PON_DA_CTRL_14_GET_RG_SSUSB_RX_EQ_RST(reg32)           REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_RST, (reg32))
#define PON_DA_CTRL_14_GET_RG_SSUSB_RX_SAOSC_RST(reg32)        REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_SAOSC_RST, (reg32))
#define PON_DA_CTRL_14_GET_RG_SSUSB_RX_CDR_STABLE(reg32)       REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_CDR_STABLE, (reg32))
#define PON_DA_CTRL_14_GET_RG_SSUSB_CDR_BIR(reg32)             REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_CDR_BIR, (reg32))
#define PON_DA_CTRL_14_GET_RG_SSUSB_TX_EIDLE_CM(reg32)         REG_FLD_GET(PON_DA_CTRL_14_FLD_RG_SSUSB_TX_EIDLE_CM, (reg32))

#define PON_DA_CTRL_15_GET_RG_SSUSB_CDR_BIRLTD1(reg32)         REG_FLD_GET(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD1, (reg32))
#define PON_DA_CTRL_15_GET_RG_SSUSB_CDR_BIRLTD0(reg32)         REG_FLD_GET(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD0, (reg32))
#define PON_DA_CTRL_15_GET_RG_SSUSB_CDR_BIRLTR(reg32)          REG_FLD_GET(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTR, (reg32))

#define PON_DA_CTRL_16_GET_RG_SSUSB_CDR_BCLTD0(reg32)          REG_FLD_GET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTD0, (reg32))
#define PON_DA_CTRL_16_GET_RG_SSUSB_CDR_BCLTR(reg32)           REG_FLD_GET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTR, (reg32))
#define PON_DA_CTRL_16_GET_RG_SSUSB_CDR_BICLTD1(reg32)         REG_FLD_GET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD1, (reg32))
#define PON_DA_CTRL_16_GET_RG_SSUSB_CDR_BICLTD0(reg32)         REG_FLD_GET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD0, (reg32))
#define PON_DA_CTRL_16_GET_RG_SSUSB_CDR_BICLTR(reg32)          REG_FLD_GET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTR, (reg32))

#define PON_DA_CTRL_17_GET_RG_SSUSB_CDR_KVSELLTD1(reg32)       REG_FLD_GET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD1, (reg32))
#define PON_DA_CTRL_17_GET_RG_SSUSB_CDR_KVSELLTD0(reg32)       REG_FLD_GET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD0, (reg32))
#define PON_DA_CTRL_17_GET_RG_SSUSB_CDR_KVSELLTR(reg32)        REG_FLD_GET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTR, (reg32))
#define PON_DA_CTRL_17_GET_RG_SSUSB_CDR_BRLTD1(reg32)          REG_FLD_GET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD1, (reg32))
#define PON_DA_CTRL_17_GET_RG_SSUSB_CDR_BRLTD0(reg32)          REG_FLD_GET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD0, (reg32))
#define PON_DA_CTRL_17_GET_RG_SSUSB_CDR_BRLTR(reg32)           REG_FLD_GET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTR, (reg32))
#define PON_DA_CTRL_17_GET_RG_SSUSB_CDR_BCLTD1(reg32)          REG_FLD_GET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BCLTD1, (reg32))

#define PON_DA_CTRL_18_GET_RG_FORCE_SSUSB_PLL_BPB(reg32)       REG_FLD_GET(PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPB, (reg32))
#define PON_DA_CTRL_18_GET_RG_FORCE_SSUSB_PLL_BPA(reg32)       REG_FLD_GET(PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPA, (reg32))

#define PON_DA_CTRL_19_GET_RG_SSUSB_PLL_BPB(reg32)             REG_FLD_GET(PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPB, (reg32))
#define PON_DA_CTRL_19_GET_RG_SSUSB_PLL_BPA(reg32)             REG_FLD_GET(PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPA, (reg32))

#define PON_DEBUG_REGISTER_0_GET_ro_ad_ben_out_cnt(reg32)      REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out_cnt, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_ad_ben_out(reg32)          REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_intf_pma_rx_sigdet_out(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_intf_pma_rx_sigdet_out, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_ad_rx_lfps(reg32)          REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_lfps, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_ad_rx_impcal_out(reg32)    REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_impcal_out, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_rx_path_rdy(reg32)         REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_rx_path_rdy, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_rx_eq_en(reg32)            REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_rx_eq_en, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_eq_done(reg32)             REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_eq_done, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_ad_sigdet_cal_out(reg32)   REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_cal_out, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_ad_sigdet_out(reg32)       REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_out, (reg32))
#define PON_DEBUG_REGISTER_0_GET_ro_lck2ref(reg32)             REG_FLD_GET(PON_DEBUG_REGISTER_0_FLD_ro_lck2ref, (reg32))

#define PON_DEBUG_REGISTER_1_GET_rg_eq_done_clr(reg32)         REG_FLD_GET(PON_DEBUG_REGISTER_1_FLD_rg_eq_done_clr, (reg32))

#define PON_DEBUG_REGISTER_2_GET_ro_qphy_rx_ctrl_debug(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_2_FLD_ro_qphy_rx_ctrl_debug, (reg32))
#define PON_DEBUG_REGISTER_2_GET_ro_da_tx_data_en(reg32)       REG_FLD_GET(PON_DEBUG_REGISTER_2_FLD_ro_da_tx_data_en, (reg32))
#define PON_DEBUG_REGISTER_2_GET_ro_txbit_dly_sel_latch(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel_latch, (reg32))
#define PON_DEBUG_REGISTER_2_GET_ro_txbit_dly_sel(reg32)       REG_FLD_GET(PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel, (reg32))
#define PON_DEBUG_REGISTER_2_GET_ro_pll_vco_cal_state(reg32)   REG_FLD_GET(PON_DEBUG_REGISTER_2_FLD_ro_pll_vco_cal_state, (reg32))

#define PON_DEBUG_REGISTER_3_GET_ro_ad_pcie_crsdet_out(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_out, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_ad_pcie_crsdet_lthout(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_lthout, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_ad_pcie_crsdet_cal_out(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_cal_out, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_ad_pcie_ckdet_out(reg32)   REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_ckdet_out, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_saosc_done(reg32)          REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_saosc_done, (reg32))
#define PON_DEBUG_REGISTER_3_GET_rg_freq_clr(reg32)            REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_rg_freq_clr, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_baser_rx_freq_lock(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_baser_rx_freq_lock, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_baser_tx_freq_lock(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_baser_tx_freq_lock, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_hg_rx_freq_lock(reg32)     REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_hg_rx_freq_lock, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_hg_tx_freq_lock(reg32)     REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_hg_tx_freq_lock, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_pon_rx_freq_lock(reg32)    REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_pon_rx_freq_lock, (reg32))
#define PON_DEBUG_REGISTER_3_GET_ro_pon_tx_freq_lock(reg32)    REG_FLD_GET(PON_DEBUG_REGISTER_3_FLD_ro_pon_tx_freq_lock, (reg32))

#define PON_DEBUG_REGISTER_4_GET_rg_ben_count_en(reg32)        REG_FLD_GET(PON_DEBUG_REGISTER_4_FLD_rg_ben_count_en, (reg32))
#define PON_DEBUG_REGISTER_4_GET_ro_ben_counter(reg32)         REG_FLD_GET(PON_DEBUG_REGISTER_4_FLD_ro_ben_counter, (reg32))

#define PON_DUMMY_REG_0_GET_rg_dummy_reg(reg32)                REG_FLD_GET(PON_DUMMY_REG_0_FLD_rg_dummy_reg, (reg32))

#define PON_DEBUG_REGISTER_5_GET_rg_fec_dec_insert_dbg_clr(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_insert_dbg_clr, (reg32))
#define PON_DEBUG_REGISTER_5_GET_rg_fec_dec_usr_def_error_en(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_usr_def_error_en, (reg32))
#define PON_DEBUG_REGISTER_5_GET_rg_fec_dec_err_insert_cont(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_cont, (reg32))
#define PON_DEBUG_REGISTER_5_GET_rg_fec_dec_err_insert_en(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_en, (reg32))

#define PON_DEBUG_REGISTER_6_GET_rg_fec_dec_err_byte_num(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_6_FLD_rg_fec_dec_err_byte_num, (reg32))

#define PON_DEBUG_REGISTER_7_GET_rg_fec_dec_usr_def_error(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_7_FLD_rg_fec_dec_usr_def_error, (reg32))

#define PON_DEBUG_REGISTER_8_GET_rg_fec_dec_err_insert_start_w1c(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_8_FLD_rg_fec_dec_err_insert_start_w1c, (reg32))

#define PON_DEBUG_REGISTER_9_GET_rg_fec_enc_insert_dbg_clr(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_insert_dbg_clr, (reg32))
#define PON_DEBUG_REGISTER_9_GET_rg_fec_enc_usr_def_error_en(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_usr_def_error_en, (reg32))
#define PON_DEBUG_REGISTER_9_GET_rg_fec_enc_err_insert_cont(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_cont, (reg32))
#define PON_DEBUG_REGISTER_9_GET_rg_fec_enc_err_insert_en(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_en, (reg32))

#define PON_DEBUG_REGISTER_10_GET_rg_fec_enc_err_byte_num(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_10_FLD_rg_fec_enc_err_byte_num, (reg32))

#define PON_DEBUG_REGISTER_11_GET_rg_fec_enc_usr_def_error(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_11_FLD_rg_fec_enc_usr_def_error, (reg32))

#define PON_DEBUG_REGISTER_12_GET_rg_fec_enc_err_insert_start_w1c(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_12_FLD_rg_fec_enc_err_insert_start_w1c, (reg32))

#define PON_DEBUG_REGISTER_13_GET_rg_trans_los_pro_cnt_thr(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_13_FLD_rg_trans_los_pro_cnt_thr, (reg32))
#define PON_DEBUG_REGISTER_13_GET_rg_phyrdy_pro_cnt_thr(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_13_FLD_rg_phyrdy_pro_cnt_thr, (reg32))

#define PON_DEBUG_REGISTER_14_GET_rg_int_record_start_clr_w1c(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_14_FLD_rg_int_record_start_clr_w1c, (reg32))

#define PON_DEBUG_REGISTER_15_GET_rg_record_start_type(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_15_FLD_rg_record_start_type, (reg32))
#define PON_DEBUG_REGISTER_15_GET_rg_record_start_dbg_clr(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_15_FLD_rg_record_start_dbg_clr, (reg32))
#define PON_DEBUG_REGISTER_15_GET_rg_lof_int_mask_keep(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_keep, (reg32))
#define PON_DEBUG_REGISTER_15_GET_rg_lof_int_mask_en(reg32)    REG_FLD_GET(PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_en, (reg32))
#define PON_DEBUG_REGISTER_15_GET_rg_int_record_en(reg32)      REG_FLD_GET(PON_DEBUG_REGISTER_15_FLD_rg_int_record_en, (reg32))

#define PON_DEBUG_REGISTER_16_GET_ro_lof_time_cnt(reg32)       REG_FLD_GET(PON_DEBUG_REGISTER_16_FLD_ro_lof_time_cnt, (reg32))

#define PON_DEBUG_REGISTER_17_GET_ro_los_time_cnt(reg32)       REG_FLD_GET(PON_DEBUG_REGISTER_17_FLD_ro_los_time_cnt, (reg32))

#define PON_DEBUG_REGISTER_18_GET_ro_phyrdy_time_cnt(reg32)    REG_FLD_GET(PON_DEBUG_REGISTER_18_FLD_ro_phyrdy_time_cnt, (reg32))

#define PON_DEBUG_REGISTER_19_GET_ro_tx_bit_dly(reg32)         REG_FLD_GET(PON_DEBUG_REGISTER_19_FLD_ro_tx_bit_dly, (reg32))

#define PON_DEBUG_REGISTER_20_GET_rg_int_record_cnt_thr(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_20_FLD_rg_int_record_cnt_thr, (reg32))

#define PON_DEBUG_REGISTER_21_GET_ro_record_done(reg32)        REG_FLD_GET(PON_DEBUG_REGISTER_21_FLD_ro_record_done, (reg32))
#define PON_DEBUG_REGISTER_21_GET_ro_fec_enc_insert_dbg(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_21_FLD_ro_fec_enc_insert_dbg, (reg32))
#define PON_DEBUG_REGISTER_21_GET_ro_record_start_dbg(reg32)   REG_FLD_GET(PON_DEBUG_REGISTER_21_FLD_ro_record_start_dbg, (reg32))
#define PON_DEBUG_REGISTER_21_GET_ro_fec_dec_insert_dbg(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_21_FLD_ro_fec_dec_insert_dbg, (reg32))

#define PON_DEBUG_REGISTER_22_GET_rg_lof_int_mask_cnt_thr(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_22_FLD_rg_lof_int_mask_cnt_thr, (reg32))

#define PON_DEBUG_REGISTER_23_GET_rgs_pll_autok_band(reg32)    REG_FLD_GET(PON_DEBUG_REGISTER_23_FLD_rgs_pll_autok_band, (reg32))

#define PON_DEBUG_REGISTER_24_GET_rg_fpga_rx_lck_rst(reg32)    REG_FLD_GET(PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_lck_rst, (reg32))
#define PON_DEBUG_REGISTER_24_GET_rg_fpga_rx_force_lck(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_force_lck, (reg32))
#define PON_DEBUG_REGISTER_24_GET_ro_fpga_tx_los_lock_cnt(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_24_FLD_ro_fpga_tx_los_lock_cnt, (reg32))
#define PON_DEBUG_REGISTER_24_GET_ro_fpga_rx_los_lock_cnt(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_24_FLD_ro_fpga_rx_los_lock_cnt, (reg32))
#define PON_DEBUG_REGISTER_24_GET_ro_fpga_lock_status(reg32)   REG_FLD_GET(PON_DEBUG_REGISTER_24_FLD_ro_fpga_lock_status, (reg32))

#define PON_DEBUG_REGISTER_25_GET_rg_gepon_phy_dlybnd_delsel(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_dlybnd_delsel, (reg32))
#define PON_DEBUG_REGISTER_25_GET_rg_gepon_phy_epsram_delsel(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_epsram_delsel, (reg32))
#define PON_DEBUG_REGISTER_25_GET_rg_pon_phy_ssram_delsel(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_ssram_delsel, (reg32))
#define PON_DEBUG_REGISTER_25_GET_rg_pon_phy_dsram3_delsel(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram3_delsel, (reg32))
#define PON_DEBUG_REGISTER_25_GET_rg_pon_phy_dsram2_delsel(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram2_delsel, (reg32))
#define PON_DEBUG_REGISTER_25_GET_rg_pon_phy_dsram1_delsel(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram1_delsel, (reg32))
#define PON_DEBUG_REGISTER_25_GET_rg_pon_phy_dsram0_delsel(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram0_delsel, (reg32))

#define PON_DEBUG_REGISTER_26_GET_ro_fpga_debug_0(reg32)       REG_FLD_GET(PON_DEBUG_REGISTER_26_FLD_ro_fpga_debug_0, (reg32))

#define PON_DEBUG_REGISTER_27_GET_ro_fpga_debug_1(reg32)       REG_FLD_GET(PON_DEBUG_REGISTER_27_FLD_ro_fpga_debug_1, (reg32))

#define PON_DEBUG_REGISTER_28_GET_ro_ad_sig_cal_offset(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_28_FLD_ro_ad_sig_cal_offset, (reg32))
#define PON_DEBUG_REGISTER_28_GET_ro_ad_cdr_band(reg32)        REG_FLD_GET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_band, (reg32))
#define PON_DEBUG_REGISTER_28_GET_ro_ad_cdr_vcocal_fail(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_fail, (reg32))
#define PON_DEBUG_REGISTER_28_GET_ro_ad_cdr_vcocal_cplt(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_cplt, (reg32))
#define PON_DEBUG_REGISTER_28_GET_ro_ad_cdr_vcocal_state(reg32) REG_FLD_GET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_state, (reg32))

#define PON_DEBUG_REGISTER_29_GET_ro_sfp_tx_en_counter(reg32)  REG_FLD_GET(PON_DEBUG_REGISTER_29_FLD_ro_sfp_tx_en_counter, (reg32))

#define PON_INTERRUPT_STS_0_GET_ro_tx_en_int(reg32)            REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_tx_en_int, (reg32))
#define PON_INTERRUPT_STS_0_GET_ro_rx_sd_int(reg32)            REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_rx_sd_int, (reg32))
#define PON_INTERRUPT_STS_0_GET_ro_los_int(reg32)              REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_los_int, (reg32))
#define PON_INTERRUPT_STS_0_GET_ro_rx_pll_meter_unlock(reg32)  REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_unlock, (reg32))
#define PON_INTERRUPT_STS_0_GET_ro_rx_pll_meter_lock(reg32)    REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_lock, (reg32))
#define PON_INTERRUPT_STS_0_GET_ro_pma_rx_sigdet_out(reg32)    REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_pma_rx_sigdet_out, (reg32))
#define PON_INTERRUPT_STS_0_GET_ro_ncpo_min_flag(reg32)        REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_ncpo_min_flag, (reg32))
#define PON_INTERRUPT_STS_0_GET_ro_ncpo_max_flag(reg32)        REG_FLD_GET(PON_INTERRUPT_STS_0_FLD_ro_ncpo_max_flag, (reg32))

#define PON_INTERRUPT_EN_0_GET_rg_tx_en_int_en(reg32)          REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_tx_en_int_en, (reg32))
#define PON_INTERRUPT_EN_0_GET_rg_rx_sd_int_en(reg32)          REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_rx_sd_int_en, (reg32))
#define PON_INTERRUPT_EN_0_GET_rg_los_int_en(reg32)            REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_los_int_en, (reg32))
#define PON_INTERRUPT_EN_0_GET_rg_rx_pll_meter_unlock_int_en(reg32) REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_unlock_int_en, (reg32))
#define PON_INTERRUPT_EN_0_GET_rg_rx_pll_meter_lock_int_en(reg32) REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_lock_int_en, (reg32))
#define PON_INTERRUPT_EN_0_GET_rg_pma_sigdet_int_en(reg32)     REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_pma_sigdet_int_en, (reg32))
#define PON_INTERRUPT_EN_0_GET_rg_ncpo_min_int_en(reg32)       REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_ncpo_min_int_en, (reg32))
#define PON_INTERRUPT_EN_0_GET_rg_ncpo_max_int_en(reg32)       REG_FLD_GET(PON_INTERRUPT_EN_0_FLD_rg_ncpo_max_int_en, (reg32))

#define PON_INTERRUPT_CLR_0_GET_rg_tx_en_int_clr(reg32)        REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_tx_en_int_clr, (reg32))
#define PON_INTERRUPT_CLR_0_GET_rg_rx_sd_int_clr(reg32)        REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_rx_sd_int_clr, (reg32))
#define PON_INTERRUPT_CLR_0_GET_rg_los_int_clr(reg32)          REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_los_int_clr, (reg32))
#define PON_INTERRUPT_CLR_0_GET_rg_rx_pll_meter_unlock_int_clr(reg32) REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_unlock_int_clr, (reg32))
#define PON_INTERRUPT_CLR_0_GET_rg_rx_pll_meter_lock_int_clr(reg32) REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_lock_int_clr, (reg32))
#define PON_INTERRUPT_CLR_0_GET_rg_pma_rx_sigdet_intclr(reg32) REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_pma_rx_sigdet_intclr, (reg32))
#define PON_INTERRUPT_CLR_0_GET_rg_ncpo_flag_clr(reg32)        REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_ncpo_flag_clr, (reg32))
#define PON_INTERRUPT_CLR_0_GET_rg_ncpo_clr(reg32)             REG_FLD_GET(PON_INTERRUPT_CLR_0_FLD_rg_ncpo_clr, (reg32))

#define PON_INTERRUPT_SET_0_GET_rg_ncpo_det_en(reg32)          REG_FLD_GET(PON_INTERRUPT_SET_0_FLD_rg_ncpo_det_en, (reg32))
#define PON_INTERRUPT_SET_0_GET_rg_rogue_en(reg32)             REG_FLD_GET(PON_INTERRUPT_SET_0_FLD_rg_rogue_en, (reg32))
#define PON_INTERRUPT_SET_0_GET_rg_int_los_on_pro(reg32)       REG_FLD_GET(PON_INTERRUPT_SET_0_FLD_rg_int_los_on_pro, (reg32))
#define PON_INTERRUPT_SET_0_GET_rg_int_los_cnt(reg32)          REG_FLD_GET(PON_INTERRUPT_SET_0_FLD_rg_int_los_cnt, (reg32))

#define PON_INTERRUPT_SET_1_GET_rg_tx_en_counter(reg32)        REG_FLD_GET(PON_INTERRUPT_SET_1_FLD_rg_tx_en_counter, (reg32))

#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_int_los_inv(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_int_los_inv, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_tdc_los_inv(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_los_inv, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_tdc_ck_en_and_los(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_ck_en_and_los, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_tdc_lck2ref_sel(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_lck2ref_sel, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_sw_lcpll_en(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_sw_lcpll_en, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_lcpll_en_inv(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_en_inv, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_lcpll_hw_ctrl_mode(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_hw_ctrl_mode, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_0_SET_rg_lcpll_force_on(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_force_on, (reg32), (val))

#define SS_LCPLL_PWCTL_SETTING_1_SET_rg_lcpll_ck_stb_timer(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_ck_stb_timer, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_1_SET_rg_lcpll_pcw_man_load_timer(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_pcw_man_load_timer, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_1_SET_rg_lcpll_en_timer(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_en_timer, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_1_SET_rg_lcpll_man_pwdb(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_man_pwdb, (reg32), (val))

#define SS_LCPLL_PWCTL_SETTING_2_SET_rg_ncpo_ana_msb(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_ncpo_ana_msb, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_2_SET_rg_tdc_mst_en(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_mst_en, (reg32), (val))
#define SS_LCPLL_PWCTL_SETTING_2_SET_rg_tdc_ck_stb_timer(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_ck_stb_timer, (reg32), (val))

#define SS_LCPLL_PWCTL_DBG_SETTING_SET_rg_lcpll_ledck_div(reg32, val) REG_FLD_SET(SS_LCPLL_PWCTL_DBG_SETTING_FLD_rg_lcpll_ledck_div, (reg32), (val))

#define SS_LCPLL_TDC_PW_0_SET_rg_lcpll_dpma_dds1_iso_disable(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_iso_disable, (reg32), (val))
#define SS_LCPLL_TDC_PW_0_SET_rg_lcpll_dpma_dds1_pwdb(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_pwdb, (reg32), (val))
#define SS_LCPLL_TDC_PW_0_SET_rg_lcpll_dpma_autopw_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_autopw_en, (reg32), (val))
#define SS_LCPLL_TDC_PW_0_SET_rg_lcpll_tdc_dig_pwdb(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_tdc_dig_pwdb, (reg32), (val))

#define SS_LCPLL_TDC_PW_1_SET_rg_lcpll_dpma_dds1_pcw_ncpo_chg(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg, (reg32), (val))
#define SS_LCPLL_TDC_PW_1_SET_rg_lcpll_dpma_dds1_rstb(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb, (reg32), (val))
#define SS_LCPLL_TDC_PW_1_SET_rg_lcpll_dpma_dds1_rstb_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb_dly, (reg32), (val))
#define SS_LCPLL_TDC_PW_1_SET_rg_lcpll_dpma_dds1_iso_disable_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_iso_disable_dly, (reg32), (val))

#define SS_LCPLL_TDC_PW_2_SET_rg_lcpll_dpma_txpll1_ddsfbk_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en, (reg32), (val))
#define SS_LCPLL_TDC_PW_2_SET_rg_lcpll_dpma_dds1_ncpo_en_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en_dly, (reg32), (val))
#define SS_LCPLL_TDC_PW_2_SET_rg_lcpll_dpma_dds1_ncpo_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en, (reg32), (val))
#define SS_LCPLL_TDC_PW_2_SET_rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly, (reg32), (val))

#define SS_LCPLL_TDC_PW_3_SET_rg_lcpll_tdc_cal(reg32, val)     REG_FLD_SET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal, (reg32), (val))
#define SS_LCPLL_TDC_PW_3_SET_rg_lcpll_tdc_cal_trig_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig_dly, (reg32), (val))
#define SS_LCPLL_TDC_PW_3_SET_rg_lcpll_tdc_cal_trig(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig, (reg32), (val))
#define SS_LCPLL_TDC_PW_3_SET_rg_lcpll_dpma_txpll1_ddsfbk_en_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en_dly, (reg32), (val))

#define SS_LCPLL_TDC_PW_4_SET_rg_lcpll_tdc_sigdet_cg(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_sigdet_cg, (reg32), (val))
#define SS_LCPLL_TDC_PW_4_SET_rg_lcpll_tdc_en(reg32, val)      REG_FLD_SET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_en, (reg32), (val))
#define SS_LCPLL_TDC_PW_4_SET_rg_lcpll_tdc_cal_offset(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_offset, (reg32), (val))
#define SS_LCPLL_TDC_PW_4_SET_rg_lcpll_tdc_cal_bw(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_bw, (reg32), (val))

#define SS_LCPLL_TDC_PW_5_SET_rg_tdc_start_cnt(reg32, val)     REG_FLD_SET(SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt, (reg32), (val))
#define SS_LCPLL_TDC_PW_5_SET_rg_tdc_start_cnt_clr(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt_clr, (reg32), (val))
#define SS_LCPLL_TDC_PW_5_SET_rg_ncpo_start_dly_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_5_FLD_rg_ncpo_start_dly_en, (reg32), (val))
#define SS_LCPLL_TDC_PW_5_SET_rg_lcpll_tdc_sync_in_mode(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_sync_in_mode, (reg32), (val))
#define SS_LCPLL_TDC_PW_5_SET_rg_lcpll_autok_tdc(reg32, val)   REG_FLD_SET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_autok_tdc, (reg32), (val))
#define SS_LCPLL_TDC_PW_5_SET_rg_lcpll_tdc_autoen(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_autoen, (reg32), (val))
#define SS_LCPLL_TDC_PW_5_SET_rg_lcpll_tdc_en_mux(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_en_mux, (reg32), (val))

#define SS_LCPLL_TDC_FLT_0_SET_rg_lcpll_tdc_offset(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_tdc_offset, (reg32), (val))
#define SS_LCPLL_TDC_FLT_0_SET_rg_lcpll_ki(reg32, val)         REG_FLD_SET(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_ki, (reg32), (val))
#define SS_LCPLL_TDC_FLT_0_SET_rg_lcpll_pon_rx_cdr_divtdc(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_pon_rx_cdr_divtdc, (reg32), (val))

#define SS_LCPLL_TDC_FLT_1_SET_rg_lcpll_tdc_gain(reg32, val)   REG_FLD_SET(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_tdc_gain, (reg32), (val))
#define SS_LCPLL_TDC_FLT_1_SET_rg_lcpll_a_tdc(reg32, val)      REG_FLD_SET(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_a_tdc, (reg32), (val))
#define SS_LCPLL_TDC_FLT_1_SET_rg_lcpll_gpon_sel(reg32, val)   REG_FLD_SET(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_gpon_sel, (reg32), (val))

#define SS_LCPLL_TDC_FLT_2_SET_rg_lcpll_ncpo_value(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_2_FLD_rg_lcpll_ncpo_value, (reg32), (val))

#define SS_LCPLL_TDC_FLT_3_SET_rg_sdm_upd_disb(reg32, val)     REG_FLD_SET(SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd_disb, (reg32), (val))
#define SS_LCPLL_TDC_FLT_3_SET_rg_sdm_upd(reg32, val)          REG_FLD_SET(SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd, (reg32), (val))
#define SS_LCPLL_TDC_FLT_3_SET_rg_lcpll_tdc_swrst(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_tdc_swrst, (reg32), (val))
#define SS_LCPLL_TDC_FLT_3_SET_rg_lcpll_ncpo_hold(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_hold, (reg32), (val))
#define SS_LCPLL_TDC_FLT_3_SET_rg_lcpll_ncpo_load(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_load, (reg32), (val))
#define SS_LCPLL_TDC_FLT_3_SET_rg_lcpll_ncpo_shift(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_shift, (reg32), (val))

#define SS_LCPLL_TDC_FLT_4_SET_rg_lcpll_tdcen_wait_lck2ref(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_wait_lck2ref, (reg32), (val))
#define SS_LCPLL_TDC_FLT_4_SET_rg_lcpll_tdcen_chk_lck2ref(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_chk_lck2ref, (reg32), (val))
#define SS_LCPLL_TDC_FLT_4_SET_rg_lcpll_tdc_rst(reg32, val)    REG_FLD_SET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdc_rst, (reg32), (val))
#define SS_LCPLL_TDC_FLT_4_SET_rg_lcpll_tdcen_rst_dlf(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_rst_dlf, (reg32), (val))

#define SS_LCPLL_TDC_FLT_5_SET_rg_lcpll_ncpo_chg(reg32, val)   REG_FLD_SET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_ncpo_chg, (reg32), (val))
#define SS_LCPLL_TDC_FLT_5_SET_rg_lcpll_tdc_autopw_ncpo(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_autopw_ncpo, (reg32), (val))
#define SS_LCPLL_TDC_FLT_5_SET_rg_lcpll_tdc_waitlck_pos_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pos_dly, (reg32), (val))
#define SS_LCPLL_TDC_FLT_5_SET_rg_lcpll_tdc_waitlck_pre_dly(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pre_dly, (reg32), (val))

#define SS_LCPLL_TDC_FLT_6_SET_rg_lcpll_tdc_pcw_sub1en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_tdc_pcw_sub1en, (reg32), (val))
#define SS_LCPLL_TDC_FLT_6_SET_rg_lcpll_dlf_mult_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_dlf_mult_en, (reg32), (val))
#define SS_LCPLL_TDC_FLT_6_SET_rg_lcpll_ncpo_chg_delay(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_delay, (reg32), (val))
#define SS_LCPLL_TDC_FLT_6_SET_rg_lcpll_ncpo_chg_mask(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_mask, (reg32), (val))

#define SS_LCPLL_TDC_FLT_7_SET_rg_lcpll_tdcck_inv_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_FLT_7_FLD_rg_lcpll_tdcck_inv_en, (reg32), (val))

#define SS_LCPLL_TDC_PCW_1_SET_rg_lcpll_pon_hrdds_pcw_ncpo_gpon(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PCW_1_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_gpon, (reg32), (val))

#define SS_LCPLL_TDC_PCW_2_SET_rg_lcpll_pon_hrdds_pcw_ncpo_epon(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_PCW_2_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_epon, (reg32), (val))

#define SS_LCPLL_TDC_RO_1_SET_ro_lcpll_tdc_dig(reg32, val)     REG_FLD_SET(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_dig, (reg32), (val))
#define SS_LCPLL_TDC_RO_1_SET_ro_lcpll_tdc_st(reg32, val)      REG_FLD_SET(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_st, (reg32), (val))
#define SS_LCPLL_TDC_RO_1_SET_ro_lcpll_tdc_tdcpw_st(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_tdcpw_st, (reg32), (val))

#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_tdc_ck_stb(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_ck_stb, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_txpll1_ddsfbk_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_txpll1_ddsfbk_en, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_tdc_cal_offset(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_offset, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_tdc_cal_bw(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_bw, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_tdc_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_en, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_rx_ck_stb(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_rx_ck_stb, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_en_lck2ref_rst(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_en_lck2ref_rst, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_dds1_pwdb(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pwdb, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_dds1_iso_disable(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_iso_disable, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_dds1_rstb(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_rstb, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_dds1_ncpo_en(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_ncpo_en, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg, (reg32), (val))
#define SS_LCPLL_TDC_RO_2_SET_ro_lcpll_tdc_dlf_gain(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dlf_gain, (reg32), (val))

#define SS_LCPLL_TDC_RO_3_SET_ro_lcpll_tdc_gain(reg32, val)    REG_FLD_SET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_gain, (reg32), (val))
#define SS_LCPLL_TDC_RO_3_SET_ro_lcpll_tdc_decode(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_decode, (reg32), (val))
#define SS_LCPLL_TDC_RO_3_SET_ro_lcpll_tdc_en(reg32, val)      REG_FLD_SET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_en, (reg32), (val))
#define SS_LCPLL_TDC_RO_3_SET_ro_lcpll_tdc_cal_done(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_cal_done, (reg32), (val))
#define SS_LCPLL_TDC_RO_3_SET_ro_lcpll_tdc_offset(reg32, val)  REG_FLD_SET(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_offset, (reg32), (val))

#define SS_LCPLL_TDC_RO_4_SET_ro_lcpll_tdc_dpma_dds1_pcw_ncpo(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_4_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo, (reg32), (val))

#define SS_LCPLL_TDC_RO_5_SET_ro_lcpll_tdc_dpma_tdc_c(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_c, (reg32), (val))
#define SS_LCPLL_TDC_RO_5_SET_ro_lcpll_tdc_dpma_tdc_d(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_d, (reg32), (val))
#define SS_LCPLL_TDC_RO_5_SET_ro_lcpll_tdc_dpma_tdc_leadlag(reg32, val) REG_FLD_SET(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_leadlag, (reg32), (val))

#define SS_LCPLL_TDC_CTRL_0_SET_rg_lcpll_ki_hold(reg32, val)   REG_FLD_SET(SS_LCPLL_TDC_CTRL_0_FLD_rg_lcpll_ki_hold, (reg32), (val))

#define SS_LCPLL_0_SET_tdc_sync_pcw_ncpo_chg(reg32, val)       REG_FLD_SET(SS_LCPLL_0_FLD_tdc_sync_pcw_ncpo_chg, (reg32), (val))
#define SS_LCPLL_0_SET_eee_lcpll_hold(reg32, val)              REG_FLD_SET(SS_LCPLL_0_FLD_eee_lcpll_hold, (reg32), (val))

#define SS_LCPLL_1_SET_tdc_sync_pcw_ncpo(reg32, val)           REG_FLD_SET(SS_LCPLL_1_FLD_tdc_sync_pcw_ncpo, (reg32), (val))

#define SS_LCPLL_2_SET_lcpll_top_flagh_en(reg32, val)          REG_FLD_SET(SS_LCPLL_2_FLD_lcpll_top_flagh_en, (reg32), (val))
#define SS_LCPLL_2_SET_lcpll_top_flagl_en(reg32, val)          REG_FLD_SET(SS_LCPLL_2_FLD_lcpll_top_flagl_en, (reg32), (val))
#define SS_LCPLL_2_SET_flagh_sel(reg32, val)                   REG_FLD_SET(SS_LCPLL_2_FLD_flagh_sel, (reg32), (val))
#define SS_LCPLL_2_SET_flagl_sel(reg32, val)                   REG_FLD_SET(SS_LCPLL_2_FLD_flagl_sel, (reg32), (val))

#define TDC_DLF_GAIN_STATUS_SET_ro_dlf_gain_rund_new(reg32, val) REG_FLD_SET(TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_rund_new, (reg32), (val))
#define TDC_DLF_GAIN_STATUS_SET_ro_dlf_gain_new(reg32, val)    REG_FLD_SET(TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_new, (reg32), (val))

#define TDC_DLF_MODE_SETTING_SET_rg_dlf_gain_latch(reg32, val) REG_FLD_SET(TDC_DLF_MODE_SETTING_FLD_rg_dlf_gain_latch, (reg32), (val))
#define TDC_DLF_MODE_SETTING_SET_rg_dlf_mode_sel(reg32, val)   REG_FLD_SET(TDC_DLF_MODE_SETTING_FLD_rg_dlf_mode_sel, (reg32), (val))
#define TDC_DLF_MODE_SETTING_SET_ro_dlf_gain_new_org(reg32, val) REG_FLD_SET(TDC_DLF_MODE_SETTING_FLD_ro_dlf_gain_new_org, (reg32), (val))

#define TDC_HOLD_CTRL_0_SET_rg_ncpo_hold_en(reg32, val)        REG_FLD_SET(TDC_HOLD_CTRL_0_FLD_rg_ncpo_hold_en, (reg32), (val))
#define TDC_HOLD_CTRL_0_SET_rg_ncpo_toggle(reg32, val)         REG_FLD_SET(TDC_HOLD_CTRL_0_FLD_rg_ncpo_toggle, (reg32), (val))
#define TDC_HOLD_CTRL_0_SET_rg_tdc_los_hold_mux(reg32, val)    REG_FLD_SET(TDC_HOLD_CTRL_0_FLD_rg_tdc_los_hold_mux, (reg32), (val))
#define TDC_HOLD_CTRL_0_SET_rg_tdc_ncpo_hold_release_time(reg32, val) REG_FLD_SET(TDC_HOLD_CTRL_0_FLD_rg_tdc_ncpo_hold_release_time, (reg32), (val))

#define TDC_HOLD_CTRL_1_SET_rg_ncpo_up_thred(reg32, val)       REG_FLD_SET(TDC_HOLD_CTRL_1_FLD_rg_ncpo_up_thred, (reg32), (val))

#define TDC_HOLD_CTRL_2_SET_rg_ncpo_dwn_thred(reg32, val)      REG_FLD_SET(TDC_HOLD_CTRL_2_FLD_rg_ncpo_dwn_thred, (reg32), (val))

#define TDC_HOLD_STATUS_0_SET_ro_ncpo_output(reg32, val)       REG_FLD_SET(TDC_HOLD_STATUS_0_FLD_ro_ncpo_output, (reg32), (val))

#define TDC_HOLD_STATUS_1_SET_ro_ncpo_max_output(reg32, val)   REG_FLD_SET(TDC_HOLD_STATUS_1_FLD_ro_ncpo_max_output, (reg32), (val))

#define TDC_HOLD_STATUS_2_SET_ro_ncpo_min_output(reg32, val)   REG_FLD_SET(TDC_HOLD_STATUS_2_FLD_ro_ncpo_min_output, (reg32), (val))

#define PON_SERDES_CTRL_0_SET_rg_tx_data_path_sel(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_tx_data_path_sel, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_ben_inv(reg32, val)           REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_ben_inv, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_force_tphy_speed_tx(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_force_tphy_speed_tx_sel(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx_sel, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_pcie_device_mode(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_pcie_device_mode, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_force_tphy_speed_rx_sel(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_rx_sel, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_force_tphy_mode(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_force_tphy_mode, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_tphy_mode_set(reg32, val)     REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_tphy_mode_set, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_sgmii_auto_init(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_sgmii_auto_init, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_tphy_speed_mode_force(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode_force, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_tphy_speed_mode(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_speed_chg(reg32, val)         REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_speed_chg, (reg32), (val))
#define PON_SERDES_CTRL_0_SET_rg_phya_auto_init(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_0_FLD_rg_phya_auto_init, (reg32), (val))

#define PON_SERDES_CTRL_1_SET_rg_rx_pwd(reg32, val)            REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_rx_pwd, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_rx_pwd_mux(reg32, val)        REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_rx_pwd_mux, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_tx_disable(reg32, val)        REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_tx_disable, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_tx_disable_mux(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_tx_disable_mux, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_phya_pwd(reg32, val)          REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_phya_pwd, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_phya_pwd_mux(reg32, val)      REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_phya_pwd_mux, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_freq_det_frc_en(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_freq_det_frc_en, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_freq_det_frc(reg32, val)      REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_freq_det_frc, (reg32), (val))
#define PON_SERDES_CTRL_1_SET_rg_cdr_re_calib(reg32, val)      REG_FLD_SET(PON_SERDES_CTRL_1_FLD_rg_cdr_re_calib, (reg32), (val))

#define PON_SERDES_CTRL_2_SET_rg_ben_on_opt(reg32, val)        REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_ben_on_opt, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_epon_benoff_opt(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_epon_benoff_opt, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_bypass_int_dly(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_bypass_int_dly, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_tx_div_mode(reg32, val)       REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_tx_div_mode, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_tx_idle_en(reg32, val)        REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_tx_idle_en, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_tx_idle_mux(reg32, val)       REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_tx_idle_mux, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_tx_idle_mode(reg32, val)      REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_tx_idle_mode, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_tx_idle(reg32, val)           REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_tx_idle, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_sigdet_wait_force(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_force, (reg32), (val))
#define PON_SERDES_CTRL_2_SET_rg_sigdet_wait_cond(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_cond, (reg32), (val))

#define PON_SERDES_CTRL_3_SET_rg_cdr_settle_dly(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_3_FLD_rg_cdr_settle_dly, (reg32), (val))
#define PON_SERDES_CTRL_3_SET_rg_cdr_bw_sel_dly(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_3_FLD_rg_cdr_bw_sel_dly, (reg32), (val))
#define PON_SERDES_CTRL_3_SET_rg_bias_pwd_to_bg_lpf_en_dly(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_3_FLD_rg_bias_pwd_to_bg_lpf_en_dly, (reg32), (val))

#define PON_SERDES_CTRL_4_SET_rg_freq_meter_sel(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_4_FLD_rg_freq_meter_sel, (reg32), (val))
#define PON_SERDES_CTRL_4_SET_rg_ppm_thres(reg32, val)         REG_FLD_SET(PON_SERDES_CTRL_4_FLD_rg_ppm_thres, (reg32), (val))
#define PON_SERDES_CTRL_4_SET_rg_lpf_window_sel(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_4_FLD_rg_lpf_window_sel, (reg32), (val))
#define PON_SERDES_CTRL_4_SET_rg_freq_det_en_wait_dly(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_4_FLD_rg_freq_det_en_wait_dly, (reg32), (val))

#define PON_SERDES_CTRL_5_SET_rg_sig_det_wait_dly(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_5_FLD_rg_sig_det_wait_dly, (reg32), (val))
#define PON_SERDES_CTRL_5_SET_rg_rx_eq_en_h_dly(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_5_FLD_rg_rx_eq_en_h_dly, (reg32), (val))

#define PON_SERDES_CTRL_6_SET_rg_rx_saosc_en_h_dly(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_6_FLD_rg_rx_saosc_en_h_dly, (reg32), (val))
#define PON_SERDES_CTRL_6_SET_rg_rx_pi_cal_en_h_dly(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_6_FLD_rg_rx_pi_cal_en_h_dly, (reg32), (val))
#define PON_SERDES_CTRL_6_SET_rg_pll_autok_vco_to_pll_bias_lpf_en_dly(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_6_FLD_rg_pll_autok_vco_to_pll_bias_lpf_en_dly, (reg32), (val))

#define PON_SERDES_CTRL_7_SET_rg_syspll_pwd_to_syspll_ready_dly(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_7_FLD_rg_syspll_pwd_to_syspll_ready_dly, (reg32), (val))
#define PON_SERDES_CTRL_7_SET_rg_sig_det_l_thres(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_7_FLD_rg_sig_det_l_thres, (reg32), (val))
#define PON_SERDES_CTRL_7_SET_rg_sig_det_h_thres(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_7_FLD_rg_sig_det_h_thres, (reg32), (val))

#define PON_SERDES_CTRL_8_SET_rg_sigdet_out_sel(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_8_FLD_rg_sigdet_out_sel, (reg32), (val))
#define PON_SERDES_CTRL_8_SET_rg_eidle_lp_en_ne_to_data_en_pe_dly_g2(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g2, (reg32), (val))
#define PON_SERDES_CTRL_8_SET_rg_eidle_lp_en_ne_to_data_en_pe_dly_g1(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g1, (reg32), (val))

#define PON_SERDES_CTRL_9_SET_rg_ssusb_cdr_band_fail(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_9_FLD_rg_ssusb_cdr_band_fail, (reg32), (val))
#define PON_SERDES_CTRL_9_SET_rg_ssusb_pll_band_fail(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_9_FLD_rg_ssusb_pll_band_fail, (reg32), (val))

#define PON_SERDES_CTRL_10_SET_ro_bist_run(reg32, val)         REG_FLD_SET(PON_SERDES_CTRL_10_FLD_ro_bist_run, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_ro_bist_ok(reg32, val)          REG_FLD_SET(PON_SERDES_CTRL_10_FLD_ro_bist_ok, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_ro_bist_err(reg32, val)         REG_FLD_SET(PON_SERDES_CTRL_10_FLD_ro_bist_err, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_prbs_mode(reg32, val)        REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_prbs_mode, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_renew_mode(reg32, val)       REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_renew_mode, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_prbs_injerr(reg32, val)      REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_prbs_injerr, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_prbs_en(reg32, val)          REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_prbs_en, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_prbs_check(reg32, val)       REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_prbs_check, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_tx_bit_polarity(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_tx_bit_polarity, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_tbi_tx_disable(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_tbi_tx_disable, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_tbi_rx_disable(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_tbi_rx_disable, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_rx_bit_polarity(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_rx_bit_polarity, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_qphy_r2t_loopback_fifo_rst(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback_fifo_rst, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_qphy_r2t_loopback(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_pcs_t2r_loopback(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_pcs_t2r_loopback, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_afifo_winc_serdes(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_afifo_winc_serdes, (reg32), (val))
#define PON_SERDES_CTRL_10_SET_rg_afifo_rinc_serdes(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_10_FLD_rg_afifo_rinc_serdes, (reg32), (val))

#define PON_SERDES_STS_0_SET_ro_debug_prbs(reg32, val)         REG_FLD_SET(PON_SERDES_STS_0_FLD_ro_debug_prbs, (reg32), (val))
#define PON_SERDES_STS_0_SET_ro_bist_err_cnt(reg32, val)       REG_FLD_SET(PON_SERDES_STS_0_FLD_ro_bist_err_cnt, (reg32), (val))

#define PON_SERDES_CTRL_11_SET_rg_udp_data_31_0(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_11_FLD_rg_udp_data_31_0, (reg32), (val))

#define PON_SERDES_CTRL_12_SET_rg_udp_data_63_32(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_12_FLD_rg_udp_data_63_32, (reg32), (val))

#define PON_SERDES_CTRL_13_SET_rg_udp_data_79_64(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_13_FLD_rg_udp_data_79_64, (reg32), (val))

#define PON_SERDES_CTRL_14_SET_rg_afifo_w_thres_serdes(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_14_FLD_rg_afifo_w_thres_serdes, (reg32), (val))
#define PON_SERDES_CTRL_14_SET_rg_afifo_r_thres_serdes(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_14_FLD_rg_afifo_r_thres_serdes, (reg32), (val))

#define PON_SERDES_STS_1_SET_ro_debug_afifo_serdes(reg32, val) REG_FLD_SET(PON_SERDES_STS_1_FLD_ro_debug_afifo_serdes, (reg32), (val))

#define PON_SERDES_CTRL_15_SET_rg_ad_ben_out_clr(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_ad_ben_out_clr, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_ben_force_value(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_ben_force_value, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_ben_force_mode(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_ben_force_mode, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_path_rdy_mux(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_path_rdy_mux, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_data_rx_inv(reg32, val)      REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_data_rx_inv, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_cid_path_en(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_cid_path_en, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_prbs15_path_en(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs15_path_en, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_prbs9_path_en(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs9_path_en, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_prbs31_path_en(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs31_path_en, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_prbs23_path_en(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs23_path_en, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_prbs7_path_en(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs7_path_en, (reg32), (val))
#define PON_SERDES_CTRL_15_SET_rg_rx_prbs_path_sel(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_15_FLD_rg_rx_prbs_path_sel, (reg32), (val))

#define PON_SERDES_CTRL_16_SET_rg_tx_end_dly_sel(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_16_FLD_rg_tx_end_dly_sel, (reg32), (val))
#define PON_SERDES_CTRL_16_SET_rg_tx_start_dly_sel(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_16_FLD_rg_tx_start_dly_sel, (reg32), (val))
#define PON_SERDES_CTRL_16_SET_rg_tx_bit_dly_mode(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_16_FLD_rg_tx_bit_dly_mode, (reg32), (val))
#define PON_SERDES_CTRL_16_SET_rg_txbit_dly(reg32, val)        REG_FLD_SET(PON_SERDES_CTRL_16_FLD_rg_txbit_dly, (reg32), (val))
#define PON_SERDES_CTRL_16_SET_rg_bit_delay_rg_mode(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_16_FLD_rg_bit_delay_rg_mode, (reg32), (val))
#define PON_SERDES_CTRL_16_SET_rg_tx_ben_end_dly_mode(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_16_FLD_rg_tx_ben_end_dly_mode, (reg32), (val))
#define PON_SERDES_CTRL_16_SET_rg_tx_ben_start_dly_mode(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_16_FLD_rg_tx_ben_start_dly_mode, (reg32), (val))

#define PON_SERDES_CTRL_17_SET_rg_tx_bit_end_dly(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_17_FLD_rg_tx_bit_end_dly, (reg32), (val))
#define PON_SERDES_CTRL_17_SET_rg_tx_bit_start_dly(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_17_FLD_rg_tx_bit_start_dly, (reg32), (val))

#define PON_SERDES_CTRL_18_SET_rg_force_rxcal_done_sel(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done_sel, (reg32), (val))
#define PON_SERDES_CTRL_18_SET_rg_force_rxcal_done(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done, (reg32), (val))
#define PON_SERDES_CTRL_18_SET_rg_rxcal_ne_cnt(reg32, val)     REG_FLD_SET(PON_SERDES_CTRL_18_FLD_rg_rxcal_ne_cnt, (reg32), (val))
#define PON_SERDES_CTRL_18_SET_rg_force_l2ref_done_sel(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done_sel, (reg32), (val))
#define PON_SERDES_CTRL_18_SET_rg_force_l2ref_done(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done, (reg32), (val))
#define PON_SERDES_CTRL_18_SET_rg_l2ref_ne_cnt(reg32, val)     REG_FLD_SET(PON_SERDES_CTRL_18_FLD_rg_l2ref_ne_cnt, (reg32), (val))

#define PON_SERDES_STS_2_SET_ro_rxcal_done(reg32, val)         REG_FLD_SET(PON_SERDES_STS_2_FLD_ro_rxcal_done, (reg32), (val))
#define PON_SERDES_STS_2_SET_ro_l2ref_done(reg32, val)         REG_FLD_SET(PON_SERDES_STS_2_FLD_ro_l2ref_done, (reg32), (val))

#define PON_SERDES_CTRL_19_SET_rg_20b_idle_code(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_19_FLD_rg_20b_idle_code, (reg32), (val))

#define PON_SERDES_CTRL_20_SET_rg_lpf_en_to_ssc_dly_cnt(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_20_FLD_rg_lpf_en_to_ssc_dly_cnt, (reg32), (val))
#define PON_SERDES_CTRL_20_SET_rg_da_pll_en_dly_cnt(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_20_FLD_rg_da_pll_en_dly_cnt, (reg32), (val))
#define PON_SERDES_CTRL_20_SET_rg_da_pll_en(reg32, val)        REG_FLD_SET(PON_SERDES_CTRL_20_FLD_rg_da_pll_en, (reg32), (val))
#define PON_SERDES_CTRL_20_SET_rg_force_da_pll_en(reg32, val)  REG_FLD_SET(PON_SERDES_CTRL_20_FLD_rg_force_da_pll_en, (reg32), (val))
#define PON_SERDES_CTRL_20_SET_rg_da_pll_en_mux(reg32, val)    REG_FLD_SET(PON_SERDES_CTRL_20_FLD_rg_da_pll_en_mux, (reg32), (val))

#define PON_SERDES_CTRL_21_SET_rg_pll_vco_cplt(reg32, val)     REG_FLD_SET(PON_SERDES_CTRL_21_FLD_rg_pll_vco_cplt, (reg32), (val))
#define PON_SERDES_CTRL_21_SET_rg_force_pll_vco_cplt(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_21_FLD_rg_force_pll_vco_cplt, (reg32), (val))
#define PON_SERDES_CTRL_21_SET_rg_da_cdr_pwd(reg32, val)       REG_FLD_SET(PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd, (reg32), (val))
#define PON_SERDES_CTRL_21_SET_rg_force_da_cdr_pwd(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_21_FLD_rg_force_da_cdr_pwd, (reg32), (val))
#define PON_SERDES_CTRL_21_SET_rg_da_cdr_pwd_dly_cnt(reg32, val) REG_FLD_SET(PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd_dly_cnt, (reg32), (val))

#define PON_SERDES_CTRL_22_SET_rg_vcocal_fail(reg32, val)      REG_FLD_SET(PON_SERDES_CTRL_22_FLD_rg_vcocal_fail, (reg32), (val))
#define PON_SERDES_CTRL_22_SET_rg_vcocal_dly_cnt(reg32, val)   REG_FLD_SET(PON_SERDES_CTRL_22_FLD_rg_vcocal_dly_cnt, (reg32), (val))
#define PON_SERDES_CTRL_22_SET_rg_vcocal_state(reg32, val)     REG_FLD_SET(PON_SERDES_CTRL_22_FLD_rg_vcocal_state, (reg32), (val))

#define PON_SYS_CTRL_0_SET_rg_xfi_rx_pma_clk_sel(reg32, val)   REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_xfi_rx_pma_clk_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_baser_tx_pma_clk_div_sel(reg32, val) REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_baser_tx_pma_clk_div_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_tx_pma_clk_sel(reg32, val)       REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_tx_pma_clk_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_hg_tx_pma_clk_div_sel(reg32, val) REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_hg_tx_pma_clk_div_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_pon_tx_pma_clk_div_sel(reg32, val) REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_pon_tx_pma_clk_div_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_rx_pma_clk_sel(reg32, val)       REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_rx_pma_clk_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_hg_rx_pma_clk_div_sel(reg32, val) REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_hg_rx_pma_clk_div_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_pon_rx_pma_clk_div_sel(reg32, val) REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_pon_rx_pma_clk_div_sel, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_baser_rx_clk_inv(reg32, val)     REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_baser_rx_clk_inv, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_baser_tx_clk_inv(reg32, val)     REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_baser_tx_clk_inv, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_tx_clk_inv(reg32, val)           REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_tx_clk_inv, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_rx_clk_inv(reg32, val)           REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_rx_clk_inv, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_txben_clk_inv(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_txben_clk_inv, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_rx_short_ck_inv(reg32, val)      REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_rx_short_ck_inv, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_tx_short_ck_inv(reg32, val)      REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_tx_short_ck_inv, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_baser_rxck_en(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_baser_rxck_en, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_baser_txck_en(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_baser_txck_en, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_hg_rx500m_ck_en(reg32, val)      REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_hg_rx500m_ck_en, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_hg_tx250m_ck_en(reg32, val)      REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_hg_tx250m_ck_en, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_pon_rx500m_ck_en(reg32, val)     REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_pon_rx500m_ck_en, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_pon_tx250m_ck_en(reg32, val)     REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_pon_tx250m_ck_en, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_force_ge_mode(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_force_ge_mode, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_force_map_mode(reg32, val)       REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_force_map_mode, (reg32), (val))
#define PON_SYS_CTRL_0_SET_rg_force_map_sel(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_0_FLD_rg_force_map_sel, (reg32), (val))

#define PON_SYS_CTRL_1_SET_rg_div_rst(reg32, val)              REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_div_rst, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_rate_adp_rx_pma_fifo_rst_n(reg32, val) REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_pma_fifo_rst_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_rate_adp_rx_fifo_rst_n(reg32, val) REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_fifo_rst_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_rate_adp_tx_fifo_rst_n(reg32, val) REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_rate_adp_tx_fifo_rst_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_all_rx_rst_n(reg32, val)         REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_all_rx_rst_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_all_tx_rst_n(reg32, val)         REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_all_tx_rst_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_baser_rx_reset_n(reg32, val)     REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_baser_rx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_baser_tx_reset_n(reg32, val)     REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_baser_tx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_rx_fifo_rst(reg32, val)          REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_rx_fifo_rst, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_force_bg_tx_value(reg32, val)    REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_force_bg_tx_value, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_force_bg_tx_mode(reg32, val)     REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_force_bg_tx_mode, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_trans_rx_data_inv(reg32, val)    REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_trans_rx_data_inv, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_bridge_rx_reset_n(reg32, val)    REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_bridge_rx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_bridge_tx_reset_n(reg32, val)    REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_bridge_tx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_eq_rx250m_div_reset_n(reg32, val) REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_eq_rx250m_div_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_eq_rx250m_reset_n(reg32, val)    REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_eq_rx250m_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_tx250m_short_reset_n(reg32, val) REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_tx250m_short_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_tx250m_div_reset_n(reg32, val)   REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_tx250m_div_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_tx_reset_n(reg32, val)           REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_tx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_rx_reset_n(reg32, val)           REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_rx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_ref_reset_n(reg32, val)          REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_ref_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_pcstx_reset_n(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_pcstx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_pcsrx_reset_n(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_pcsrx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_pcstxrx_reset_n(reg32, val)      REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_pcstxrx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_mactx_reset_n(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_mactx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_macrx_reset_n(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_macrx_reset_n, (reg32), (val))
#define PON_SYS_CTRL_1_SET_rg_mactxrx_reset_n(reg32, val)      REG_FLD_SET(PON_SYS_CTRL_1_FLD_rg_mactxrx_reset_n, (reg32), (val))

#define PON_SYS_CTRL_2_SET_rg_1us_40m_cnt(reg32, val)          REG_FLD_SET(PON_SYS_CTRL_2_FLD_rg_1us_40m_cnt, (reg32), (val))
#define PON_SYS_CTRL_2_SET_rg_1us_25m_cnt(reg32, val)          REG_FLD_SET(PON_SYS_CTRL_2_FLD_rg_1us_25m_cnt, (reg32), (val))
#define PON_SYS_CTRL_2_SET_rg_pon_mbist_slow_mode_en(reg32, val) REG_FLD_SET(PON_SYS_CTRL_2_FLD_rg_pon_mbist_slow_mode_en, (reg32), (val))
#define PON_SYS_CTRL_2_SET_rg_pon_mbist_mode_en(reg32, val)    REG_FLD_SET(PON_SYS_CTRL_2_FLD_rg_pon_mbist_mode_en, (reg32), (val))
#define PON_SYS_CTRL_2_SET_rg_xtal_mode(reg32, val)            REG_FLD_SET(PON_SYS_CTRL_2_FLD_rg_xtal_mode, (reg32), (val))
#define PON_SYS_CTRL_2_SET_rg_xtal_is_25m(reg32, val)          REG_FLD_SET(PON_SYS_CTRL_2_FLD_rg_xtal_is_25m, (reg32), (val))
#define PON_SYS_CTRL_2_SET_rg_pon_probe_sel(reg32, val)        REG_FLD_SET(PON_SYS_CTRL_2_FLD_rg_pon_probe_sel, (reg32), (val))

#define PON_CK_CTRL_0_SET_rg_rs_ck_cnt(reg32, val)             REG_FLD_SET(PON_CK_CTRL_0_FLD_rg_rs_ck_cnt, (reg32), (val))
#define PON_CK_CTRL_0_SET_rg_rs_ck_sel(reg32, val)             REG_FLD_SET(PON_CK_CTRL_0_FLD_rg_rs_ck_sel, (reg32), (val))
#define PON_CK_CTRL_0_SET_rg_rs_ck_mux(reg32, val)             REG_FLD_SET(PON_CK_CTRL_0_FLD_rg_rs_ck_mux, (reg32), (val))
#define PON_CK_CTRL_0_SET_rg_baser_rxck_mode(reg32, val)       REG_FLD_SET(PON_CK_CTRL_0_FLD_rg_baser_rxck_mode, (reg32), (val))
#define PON_CK_CTRL_0_SET_rg_baser_txck_mode(reg32, val)       REG_FLD_SET(PON_CK_CTRL_0_FLD_rg_baser_txck_mode, (reg32), (val))

#define HG_RST_CTRL_0_SET_rg_hsgmii_mac_rxck_free_sel(reg32, val) REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hsgmii_mac_rxck_free_sel, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_hsgmii_mac_txck_free_sel(reg32, val) REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hsgmii_mac_txck_free_sel, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_xfi_pcs_bg_lb_fifo_rst_n(reg32, val) REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_xfi_pcs_bg_lb_fifo_rst_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_xfi_pcs_tx_gearbox_fifo_rst_n(reg32, val) REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_xfi_pcs_tx_gearbox_fifo_rst_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_xfi_pcs_rx_gearbox_fifo_rst_n(reg32, val) REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_xfi_pcs_rx_gearbox_fifo_rst_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_bypass_tx_re_en(reg32, val)       REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_bypass_tx_re_en, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_hg_mac_rx_reset_n(reg32, val)     REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hg_mac_rx_reset_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_hg_mac_tx_reset_n(reg32, val)     REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hg_mac_tx_reset_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_hg_pcs_rx_reset_n(reg32, val)     REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hg_pcs_rx_reset_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_hg_pcs_tx_reset_n(reg32, val)     REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hg_pcs_tx_reset_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_hg_pma_rx_reset_n(reg32, val)     REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hg_pma_rx_reset_n, (reg32), (val))
#define HG_RST_CTRL_0_SET_rg_hg_pma_tx_reset_n(reg32, val)     REG_FLD_SET(HG_RST_CTRL_0_FLD_rg_hg_pma_tx_reset_n, (reg32), (val))

#define HG_MODE_CTRL_0_SET_rg_fpga_tx_swap(reg32, val)         REG_FLD_SET(HG_MODE_CTRL_0_FLD_rg_fpga_tx_swap, (reg32), (val))
#define HG_MODE_CTRL_0_SET_rg_fpga_mode_ctrl(reg32, val)       REG_FLD_SET(HG_MODE_CTRL_0_FLD_rg_fpga_mode_ctrl, (reg32), (val))
#define HG_MODE_CTRL_0_SET_rg_ip_reset_mode(reg32, val)        REG_FLD_SET(HG_MODE_CTRL_0_FLD_rg_ip_reset_mode, (reg32), (val))
#define HG_MODE_CTRL_0_SET_rg_sgmii_mode(reg32, val)           REG_FLD_SET(HG_MODE_CTRL_0_FLD_rg_sgmii_mode, (reg32), (val))
#define HG_MODE_CTRL_0_SET_rg_gpon_sym_mode(reg32, val)        REG_FLD_SET(HG_MODE_CTRL_0_FLD_rg_gpon_sym_mode, (reg32), (val))
#define HG_MODE_CTRL_0_SET_rg_serdes_mode(reg32, val)          REG_FLD_SET(HG_MODE_CTRL_0_FLD_rg_serdes_mode, (reg32), (val))

#define HG_RG_CTRL_0_SET_rg_eq_step1(reg32, val)               REG_FLD_SET(HG_RG_CTRL_0_FLD_rg_eq_step1, (reg32), (val))
#define HG_RG_CTRL_0_SET_rg_tx_dem(reg32, val)                 REG_FLD_SET(HG_RG_CTRL_0_FLD_rg_tx_dem, (reg32), (val))
#define HG_RG_CTRL_0_SET_rg_ln0_rx_dac_pwd(reg32, val)         REG_FLD_SET(HG_RG_CTRL_0_FLD_rg_ln0_rx_dac_pwd, (reg32), (val))
#define HG_RG_CTRL_0_SET_rg_rx_dac_pwd_mux(reg32, val)         REG_FLD_SET(HG_RG_CTRL_0_FLD_rg_rx_dac_pwd_mux, (reg32), (val))

#define PON_STS_0_SET_ro_baser_mode(reg32, val)                REG_FLD_SET(PON_STS_0_FLD_ro_baser_mode, (reg32), (val))
#define PON_STS_0_SET_ro_hsgmii_mode(reg32, val)               REG_FLD_SET(PON_STS_0_FLD_ro_hsgmii_mode, (reg32), (val))
#define PON_STS_0_SET_ro_sgmii_mode(reg32, val)                REG_FLD_SET(PON_STS_0_FLD_ro_sgmii_mode, (reg32), (val))
#define PON_STS_0_SET_ro_epon_mode(reg32, val)                 REG_FLD_SET(PON_STS_0_FLD_ro_epon_mode, (reg32), (val))
#define PON_STS_0_SET_ro_gpon_tmode(reg32, val)                REG_FLD_SET(PON_STS_0_FLD_ro_gpon_tmode, (reg32), (val))
#define PON_STS_0_SET_ro_gpon_mode(reg32, val)                 REG_FLD_SET(PON_STS_0_FLD_ro_gpon_mode, (reg32), (val))

#define PON_OSR_SEL_CTRL_SET_rg_tx_eqd_mode(reg32, val)        REG_FLD_SET(PON_OSR_SEL_CTRL_FLD_rg_tx_eqd_mode, (reg32), (val))
#define PON_OSR_SEL_CTRL_SET_rg_rx_osr_sel(reg32, val)         REG_FLD_SET(PON_OSR_SEL_CTRL_FLD_rg_rx_osr_sel, (reg32), (val))
#define PON_OSR_SEL_CTRL_SET_rg_tx_osr_sel(reg32, val)         REG_FLD_SET(PON_OSR_SEL_CTRL_FLD_rg_tx_osr_sel, (reg32), (val))

#define CDR_CTRL_0_SET_rg_bic_ltr_mode_1(reg32, val)           REG_FLD_SET(CDR_CTRL_0_FLD_rg_bic_ltr_mode_1, (reg32), (val))
#define CDR_CTRL_0_SET_rg_bic_ltr_mode_0(reg32, val)           REG_FLD_SET(CDR_CTRL_0_FLD_rg_bic_ltr_mode_0, (reg32), (val))
#define CDR_CTRL_0_SET_rg_bir_ltr_mode_1(reg32, val)           REG_FLD_SET(CDR_CTRL_0_FLD_rg_bir_ltr_mode_1, (reg32), (val))
#define CDR_CTRL_0_SET_rg_bir_ltr_mode_0(reg32, val)           REG_FLD_SET(CDR_CTRL_0_FLD_rg_bir_ltr_mode_0, (reg32), (val))

#define CDR_CTRL_1_SET_rg_kvsel_ltr_mode_1(reg32, val)         REG_FLD_SET(CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_1, (reg32), (val))
#define CDR_CTRL_1_SET_rg_kvsel_ltr_mode_0(reg32, val)         REG_FLD_SET(CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_0, (reg32), (val))
#define CDR_CTRL_1_SET_rg_br_ltr_mode_1(reg32, val)            REG_FLD_SET(CDR_CTRL_1_FLD_rg_br_ltr_mode_1, (reg32), (val))
#define CDR_CTRL_1_SET_rg_br_ltr_mode_0(reg32, val)            REG_FLD_SET(CDR_CTRL_1_FLD_rg_br_ltr_mode_0, (reg32), (val))
#define CDR_CTRL_1_SET_rg_bc_ltr_mode_1(reg32, val)            REG_FLD_SET(CDR_CTRL_1_FLD_rg_bc_ltr_mode_1, (reg32), (val))
#define CDR_CTRL_1_SET_rg_bc_ltr_mode_0(reg32, val)            REG_FLD_SET(CDR_CTRL_1_FLD_rg_bc_ltr_mode_0, (reg32), (val))

#define CDR_CTRL_2_SET_rg_bic_ltd0_mode_1(reg32, val)          REG_FLD_SET(CDR_CTRL_2_FLD_rg_bic_ltd0_mode_1, (reg32), (val))
#define CDR_CTRL_2_SET_rg_bic_ltd0_mode_0(reg32, val)          REG_FLD_SET(CDR_CTRL_2_FLD_rg_bic_ltd0_mode_0, (reg32), (val))
#define CDR_CTRL_2_SET_rg_bir_ltd0_mode_1(reg32, val)          REG_FLD_SET(CDR_CTRL_2_FLD_rg_bir_ltd0_mode_1, (reg32), (val))
#define CDR_CTRL_2_SET_rg_bir_ltd0_mode_0(reg32, val)          REG_FLD_SET(CDR_CTRL_2_FLD_rg_bir_ltd0_mode_0, (reg32), (val))

#define CDR_CTRL_3_SET_rg_kvsel_ltd0_mode_1(reg32, val)        REG_FLD_SET(CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_1, (reg32), (val))
#define CDR_CTRL_3_SET_rg_kvsel_ltd0_mode_0(reg32, val)        REG_FLD_SET(CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_0, (reg32), (val))
#define CDR_CTRL_3_SET_rg_br_ltd0_mode_1(reg32, val)           REG_FLD_SET(CDR_CTRL_3_FLD_rg_br_ltd0_mode_1, (reg32), (val))
#define CDR_CTRL_3_SET_rg_br_ltd0_mode_0(reg32, val)           REG_FLD_SET(CDR_CTRL_3_FLD_rg_br_ltd0_mode_0, (reg32), (val))
#define CDR_CTRL_3_SET_rg_bc_ltd0_mode_1(reg32, val)           REG_FLD_SET(CDR_CTRL_3_FLD_rg_bc_ltd0_mode_1, (reg32), (val))
#define CDR_CTRL_3_SET_rg_bc_ltd0_mode_0(reg32, val)           REG_FLD_SET(CDR_CTRL_3_FLD_rg_bc_ltd0_mode_0, (reg32), (val))

#define CDR_CTRL_4_SET_rg_bic_ltd1_mode_1(reg32, val)          REG_FLD_SET(CDR_CTRL_4_FLD_rg_bic_ltd1_mode_1, (reg32), (val))
#define CDR_CTRL_4_SET_rg_bic_ltd1_mode_0(reg32, val)          REG_FLD_SET(CDR_CTRL_4_FLD_rg_bic_ltd1_mode_0, (reg32), (val))
#define CDR_CTRL_4_SET_rg_bir_ltd1_mode_1(reg32, val)          REG_FLD_SET(CDR_CTRL_4_FLD_rg_bir_ltd1_mode_1, (reg32), (val))
#define CDR_CTRL_4_SET_rg_bir_ltd1_mode_0(reg32, val)          REG_FLD_SET(CDR_CTRL_4_FLD_rg_bir_ltd1_mode_0, (reg32), (val))

#define CDR_CTRL_5_SET_rg_kvsel_ltd1_mode_1(reg32, val)        REG_FLD_SET(CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_1, (reg32), (val))
#define CDR_CTRL_5_SET_rg_kvsel_ltd1_mode_0(reg32, val)        REG_FLD_SET(CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_0, (reg32), (val))
#define CDR_CTRL_5_SET_rg_br_ltd1_mode_1(reg32, val)           REG_FLD_SET(CDR_CTRL_5_FLD_rg_br_ltd1_mode_1, (reg32), (val))
#define CDR_CTRL_5_SET_rg_br_ltd1_mode_0(reg32, val)           REG_FLD_SET(CDR_CTRL_5_FLD_rg_br_ltd1_mode_0, (reg32), (val))
#define CDR_CTRL_5_SET_rg_bc_ltd1_mode_1(reg32, val)           REG_FLD_SET(CDR_CTRL_5_FLD_rg_bc_ltd1_mode_1, (reg32), (val))
#define CDR_CTRL_5_SET_rg_bc_ltd1_mode_0(reg32, val)           REG_FLD_SET(CDR_CTRL_5_FLD_rg_bc_ltd1_mode_0, (reg32), (val))

#define TX_CTRL_0_SET_rg_tx_ben_end_dly_sel(reg32, val)        REG_FLD_SET(TX_CTRL_0_FLD_rg_tx_ben_end_dly_sel, (reg32), (val))
#define TX_CTRL_0_SET_rg_tx_ben_start_dly_sel(reg32, val)      REG_FLD_SET(TX_CTRL_0_FLD_rg_tx_ben_start_dly_sel, (reg32), (val))
#define TX_CTRL_0_SET_rg_tx_ben_exten_ftune(reg32, val)        REG_FLD_SET(TX_CTRL_0_FLD_rg_tx_ben_exten_ftune, (reg32), (val))
#define TX_CTRL_0_SET_rg_tx_dly_ben_ftune(reg32, val)          REG_FLD_SET(TX_CTRL_0_FLD_rg_tx_dly_ben_ftune, (reg32), (val))
#define TX_CTRL_0_SET_rg_tx_dly_data_ftune(reg32, val)         REG_FLD_SET(TX_CTRL_0_FLD_rg_tx_dly_data_ftune, (reg32), (val))

#define TX_CTRL_1_SET_rg_rx_non_reverse_epon(reg32, val)       REG_FLD_SET(TX_CTRL_1_FLD_rg_rx_non_reverse_epon, (reg32), (val))
#define TX_CTRL_1_SET_rg_tx_non_reverse_epon(reg32, val)       REG_FLD_SET(TX_CTRL_1_FLD_rg_tx_non_reverse_epon, (reg32), (val))
#define TX_CTRL_1_SET_rg_rx_non_reverse_gpon(reg32, val)       REG_FLD_SET(TX_CTRL_1_FLD_rg_rx_non_reverse_gpon, (reg32), (val))
#define TX_CTRL_1_SET_rg_tx_non_reverse_gpon(reg32, val)       REG_FLD_SET(TX_CTRL_1_FLD_rg_tx_non_reverse_gpon, (reg32), (val))
#define TX_CTRL_1_SET_rg_force_rx_pon_sel(reg32, val)          REG_FLD_SET(TX_CTRL_1_FLD_rg_force_rx_pon_sel, (reg32), (val))
#define TX_CTRL_1_SET_rg_force_rx_pon_mode(reg32, val)         REG_FLD_SET(TX_CTRL_1_FLD_rg_force_rx_pon_mode, (reg32), (val))
#define TX_CTRL_1_SET_rg_force_tx_gpon_sel(reg32, val)         REG_FLD_SET(TX_CTRL_1_FLD_rg_force_tx_gpon_sel, (reg32), (val))
#define TX_CTRL_1_SET_rg_force_tx_gpon_mode(reg32, val)        REG_FLD_SET(TX_CTRL_1_FLD_rg_force_tx_gpon_mode, (reg32), (val))
#define TX_CTRL_1_SET_rg_force_tx_epon_sel(reg32, val)         REG_FLD_SET(TX_CTRL_1_FLD_rg_force_tx_epon_sel, (reg32), (val))
#define TX_CTRL_1_SET_rg_force_tx_epon_mode(reg32, val)        REG_FLD_SET(TX_CTRL_1_FLD_rg_force_tx_epon_mode, (reg32), (val))

#define TX_CTRL_2_SET_rg_txben_bdly_mode(reg32, val)           REG_FLD_SET(TX_CTRL_2_FLD_rg_txben_bdly_mode, (reg32), (val))
#define TX_CTRL_2_SET_rg_txdata_bdly_mode(reg32, val)          REG_FLD_SET(TX_CTRL_2_FLD_rg_txdata_bdly_mode, (reg32), (val))
#define TX_CTRL_2_SET_rg_bit_delay_mode_sel(reg32, val)        REG_FLD_SET(TX_CTRL_2_FLD_rg_bit_delay_mode_sel, (reg32), (val))
#define TX_CTRL_2_SET_rg_benoff_cnt_max(reg32, val)            REG_FLD_SET(TX_CTRL_2_FLD_rg_benoff_cnt_max, (reg32), (val))
#define TX_CTRL_2_SET_rg_txben_pre_post_zero_en(reg32, val)    REG_FLD_SET(TX_CTRL_2_FLD_rg_txben_pre_post_zero_en, (reg32), (val))
#define TX_CTRL_2_SET_rg_hsdata_en_sel(reg32, val)             REG_FLD_SET(TX_CTRL_2_FLD_rg_hsdata_en_sel, (reg32), (val))
#define TX_CTRL_2_SET_rg_outben_data_mode(reg32, val)          REG_FLD_SET(TX_CTRL_2_FLD_rg_outben_data_mode, (reg32), (val))

#define TX_CTRL_3_SET_rg_bist_prbs_update(reg32, val)          REG_FLD_SET(TX_CTRL_3_FLD_rg_bist_prbs_update, (reg32), (val))
#define TX_CTRL_3_SET_rg_bist_prbs_init_seed_benoff(reg32, val) REG_FLD_SET(TX_CTRL_3_FLD_rg_bist_prbs_init_seed_benoff, (reg32), (val))

#define TX_CTRL_4_SET_rg_benoff_d0(reg32, val)                 REG_FLD_SET(TX_CTRL_4_FLD_rg_benoff_d0, (reg32), (val))

#define TX_CTRL_5_SET_rg_benoff_d1(reg32, val)                 REG_FLD_SET(TX_CTRL_5_FLD_rg_benoff_d1, (reg32), (val))

#define TX_CTRL_6_SET_rg_benoff_d2(reg32, val)                 REG_FLD_SET(TX_CTRL_6_FLD_rg_benoff_d2, (reg32), (val))

#define TX_CTRL_7_SET_rg_benoff_d3(reg32, val)                 REG_FLD_SET(TX_CTRL_7_FLD_rg_benoff_d3, (reg32), (val))

#define FRQ_CTRL_0_SET_rg_hg_rx_frq_ppm_thres(reg32, val)      REG_FLD_SET(FRQ_CTRL_0_FLD_rg_hg_rx_frq_ppm_thres, (reg32), (val))
#define FRQ_CTRL_0_SET_rg_hg_tx_frq_ppm_thres(reg32, val)      REG_FLD_SET(FRQ_CTRL_0_FLD_rg_hg_tx_frq_ppm_thres, (reg32), (val))
#define FRQ_CTRL_0_SET_rg_pon_rx_frq_ppm_thres(reg32, val)     REG_FLD_SET(FRQ_CTRL_0_FLD_rg_pon_rx_frq_ppm_thres, (reg32), (val))
#define FRQ_CTRL_0_SET_rg_pon_tx_frq_ppm_thres(reg32, val)     REG_FLD_SET(FRQ_CTRL_0_FLD_rg_pon_tx_frq_ppm_thres, (reg32), (val))

#define FRQ_CTRL_1_SET_rg_baser_rx_frq_ppm_thres(reg32, val)   REG_FLD_SET(FRQ_CTRL_1_FLD_rg_baser_rx_frq_ppm_thres, (reg32), (val))
#define FRQ_CTRL_1_SET_rg_baser_tx_frq_ppm_thres(reg32, val)   REG_FLD_SET(FRQ_CTRL_1_FLD_rg_baser_tx_frq_ppm_thres, (reg32), (val))

#define FRQ_CTRL_2_SET_rg_baser_rx_freq_det_mux(reg32, val)    REG_FLD_SET(FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_mux, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_baser_tx_freq_det_mux(reg32, val)    REG_FLD_SET(FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_mux, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_hg_rx_freq_det_mux(reg32, val)       REG_FLD_SET(FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_mux, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_hg_tx_freq_det_mux(reg32, val)       REG_FLD_SET(FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_mux, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_pon_rx_freq_det_mux(reg32, val)      REG_FLD_SET(FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_mux, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_pon_tx_freq_det_mux(reg32, val)      REG_FLD_SET(FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_mux, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_baser_rx_freq_det_en(reg32, val)     REG_FLD_SET(FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_en, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_baser_tx_freq_det_en(reg32, val)     REG_FLD_SET(FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_en, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_hg_rx_freq_det_en(reg32, val)        REG_FLD_SET(FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_en, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_hg_tx_freq_det_en(reg32, val)        REG_FLD_SET(FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_en, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_pon_rx_freq_det_en(reg32, val)       REG_FLD_SET(FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_en, (reg32), (val))
#define FRQ_CTRL_2_SET_rg_pon_tx_freq_det_en(reg32, val)       REG_FLD_SET(FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_en, (reg32), (val))

#define FRQ_CTRL_3_SET_rg_pon_rx_freq_cnt(reg32, val)          REG_FLD_SET(FRQ_CTRL_3_FLD_rg_pon_rx_freq_cnt, (reg32), (val))
#define FRQ_CTRL_3_SET_rg_pon_tx_freq_cnt(reg32, val)          REG_FLD_SET(FRQ_CTRL_3_FLD_rg_pon_tx_freq_cnt, (reg32), (val))

#define FRQ_CTRL_4_SET_rg_hg_rx_freq_cnt(reg32, val)           REG_FLD_SET(FRQ_CTRL_4_FLD_rg_hg_rx_freq_cnt, (reg32), (val))
#define FRQ_CTRL_4_SET_rg_hg_tx_freq_cnt(reg32, val)           REG_FLD_SET(FRQ_CTRL_4_FLD_rg_hg_tx_freq_cnt, (reg32), (val))

#define FRQ_CTRL_5_SET_rg_baser_rx_freq_cnt(reg32, val)        REG_FLD_SET(FRQ_CTRL_5_FLD_rg_baser_rx_freq_cnt, (reg32), (val))
#define FRQ_CTRL_5_SET_rg_baser_tx_freq_cnt(reg32, val)        REG_FLD_SET(FRQ_CTRL_5_FLD_rg_baser_tx_freq_cnt, (reg32), (val))

#define RX_FREQDET_0_SET_rg_rx_pll_unlock_cyclecnt(reg32, val) REG_FLD_SET(RX_FREQDET_0_FLD_rg_rx_pll_unlock_cyclecnt, (reg32), (val))
#define RX_FREQDET_0_SET_rg_rx_pll_lock_cyclecnt(reg32, val)   REG_FLD_SET(RX_FREQDET_0_FLD_rg_rx_pll_lock_cyclecnt, (reg32), (val))

#define RX_FREQDET_1_SET_rg_rx_pll_lock_target_end(reg32, val) REG_FLD_SET(RX_FREQDET_1_FLD_rg_rx_pll_lock_target_end, (reg32), (val))
#define RX_FREQDET_1_SET_rg_rx_pll_lock_target_beg(reg32, val) REG_FLD_SET(RX_FREQDET_1_FLD_rg_rx_pll_lock_target_beg, (reg32), (val))

#define RX_FREQDET_2_SET_rg_rx_pll_unlock_target_end(reg32, val) REG_FLD_SET(RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_end, (reg32), (val))
#define RX_FREQDET_2_SET_rg_rx_pll_unlock_target_beg(reg32, val) REG_FLD_SET(RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_beg, (reg32), (val))

#define RX_FREQDET_3_SET_rg_force_rx_freq_det_en(reg32, val)   REG_FLD_SET(RX_FREQDET_3_FLD_rg_force_rx_freq_det_en, (reg32), (val))
#define RX_FREQDET_3_SET_rg_rx_pll_wait_100us(reg32, val)      REG_FLD_SET(RX_FREQDET_3_FLD_rg_rx_pll_wait_100us, (reg32), (val))
#define RX_FREQDET_3_SET_rg_rx_pll_unlockth(reg32, val)        REG_FLD_SET(RX_FREQDET_3_FLD_rg_rx_pll_unlockth, (reg32), (val))
#define RX_FREQDET_3_SET_rg_rx_pll_lock_lockth(reg32, val)     REG_FLD_SET(RX_FREQDET_3_FLD_rg_rx_pll_lock_lockth, (reg32), (val))
#define RX_FREQDET_3_SET_rg_rx_pll_lock_cnt_clear(reg32, val)  REG_FLD_SET(RX_FREQDET_3_FLD_rg_rx_pll_lock_cnt_clear, (reg32), (val))
#define RX_FREQDET_3_SET_rg_rx_pll_freqlock_det_en(reg32, val) REG_FLD_SET(RX_FREQDET_3_FLD_rg_rx_pll_freqlock_det_en, (reg32), (val))

#define RX_FREQDET_STS_0_SET_ro_rx_prb_out_freqdet(reg32, val) REG_FLD_SET(RX_FREQDET_STS_0_FLD_ro_rx_prb_out_freqdet, (reg32), (val))
#define RX_FREQDET_STS_0_SET_ro_rx_state_freqdet(reg32, val)   REG_FLD_SET(RX_FREQDET_STS_0_FLD_ro_rx_state_freqdet, (reg32), (val))
#define RX_FREQDET_STS_0_SET_ro_rx_fbck_lock_inner(reg32, val) REG_FLD_SET(RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock_inner, (reg32), (val))
#define RX_FREQDET_STS_0_SET_ro_rx_fbck_lock(reg32, val)       REG_FLD_SET(RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock, (reg32), (val))
#define RX_FREQDET_STS_0_SET_ro_rx_lslock_cnt(reg32, val)      REG_FLD_SET(RX_FREQDET_STS_0_FLD_ro_rx_lslock_cnt, (reg32), (val))
#define RX_FREQDET_STS_0_SET_ro_rx_fl_out(reg32, val)          REG_FLD_SET(RX_FREQDET_STS_0_FLD_ro_rx_fl_out, (reg32), (val))

#define TX_CTRL_8_SET_rg_tx_test_mode(reg32, val)              REG_FLD_SET(TX_CTRL_8_FLD_rg_tx_test_mode, (reg32), (val))
#define TX_CTRL_8_SET_rg_tx_test_pattern(reg32, val)           REG_FLD_SET(TX_CTRL_8_FLD_rg_tx_test_pattern, (reg32), (val))

#define TX_CTRL_9_SET_rg_ncpo_los_cnt(reg32, val)              REG_FLD_SET(TX_CTRL_9_FLD_rg_ncpo_los_cnt, (reg32), (val))
#define TX_CTRL_9_SET_rg_ncpo_los_hold_en(reg32, val)          REG_FLD_SET(TX_CTRL_9_FLD_rg_ncpo_los_hold_en, (reg32), (val))
#define TX_CTRL_9_SET_rg_los_function_sel(reg32, val)          REG_FLD_SET(TX_CTRL_9_FLD_rg_los_function_sel, (reg32), (val))
#define TX_CTRL_9_SET_rg_ncpo_los_pro_en(reg32, val)           REG_FLD_SET(TX_CTRL_9_FLD_rg_ncpo_los_pro_en, (reg32), (val))
#define TX_CTRL_9_SET_rg_ncpo_los_inv(reg32, val)              REG_FLD_SET(TX_CTRL_9_FLD_rg_ncpo_los_inv, (reg32), (val))
#define TX_CTRL_9_SET_rg_ncpo_lof_en(reg32, val)               REG_FLD_SET(TX_CTRL_9_FLD_rg_ncpo_lof_en, (reg32), (val))

#define RX_FREQDET_DET_CTRL_0_SET_rg_rx_cdr_freq_chk_cycle(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_0_FLD_rg_rx_cdr_freq_chk_cycle, (reg32), (val))

#define RX_FREQDET_DET_CTRL_1_SET_rg_rx_cdr_freq_golden_cycle(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_1_FLD_rg_rx_cdr_freq_golden_cycle, (reg32), (val))

#define RX_FREQDET_DET_CTRL_2_SET_rg_rx_cdr_freq_tolerate_cycle(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_2_FLD_rg_rx_cdr_freq_tolerate_cycle, (reg32), (val))

#define RX_FREQDET_DET_CTRL_4_SET_ro_rx_cdr_freq_unlck_cnt(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_unlck_cnt, (reg32), (val))
#define RX_FREQDET_DET_CTRL_4_SET_ro_rx_cdr_freq_state(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_state, (reg32), (val))
#define RX_FREQDET_DET_CTRL_4_SET_ro_rx_cdr_freq_lock(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_lock, (reg32), (val))
#define RX_FREQDET_DET_CTRL_4_SET_rg_rx_cdr_freq_relck_num(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_relck_num, (reg32), (val))
#define RX_FREQDET_DET_CTRL_4_SET_rg_rx_cdr_freq_clear_unlck_cnt(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_clear_unlck_cnt, (reg32), (val))
#define RX_FREQDET_DET_CTRL_4_SET_rg_rx_cdr_freq_det_en(reg32, val) REG_FLD_SET(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_det_en, (reg32), (val))

#define PON_DA_MUX_CTRL_0_SET_rg_c_da_pll_bias_lpf_en(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_lpf_en, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_pll_bias_lpf_en(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_lpf_en, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_bias_pwd(reg32, val)     REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_bias_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_bias_pwd(reg32, val)     REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_bias_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_biasimr_pwd(reg32, val)  REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_biasimr_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_biasimr_pwd(reg32, val)  REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_biasimr_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_bg_lpf_en(reg32, val)    REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_bg_lpf_en, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_bg_lpf_en(reg32, val)    REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_bg_lpf_en, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_pll_bias_pwd(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_pll_bias_pwd(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_xtal_rx_pwd(reg32, val)  REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_xtal_rx_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_xtal_rx_pwd(reg32, val)  REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_xtal_rx_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_syspll_pwd(reg32, val)   REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_syspll_pwd(reg32, val)   REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_syspll_sdm_pwr_on(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_pwr_on, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_syspll_sdm_pwr_on(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_pwr_on, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_syspll_sdm_iso_en(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_iso_en, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_syspll_sdm_iso_en(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_iso_en, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_pll_pwd(reg32, val)      REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_pll_pwd(reg32, val)      REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_pwd, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_pll_autok_vco(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_autok_vco, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_pll_autok_vco(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_autok_vco, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_c_da_pll_ssc_en(reg32, val)   REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_ssc_en, (reg32), (val))
#define PON_DA_MUX_CTRL_0_SET_rg_f_da_pll_ssc_en(reg32, val)   REG_FLD_SET(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_ssc_en, (reg32), (val))

#define PON_DA_MUX_CTRL_1_SET_rg_c_da_sigdet_cal_offset(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_1_FLD_rg_c_da_sigdet_cal_offset, (reg32), (val))
#define PON_DA_MUX_CTRL_1_SET_rg_f_da_sigdet_cal_offset(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_1_FLD_rg_f_da_sigdet_cal_offset, (reg32), (val))

#define PON_DA_MUX_CTRL_2_SET_rg_c_da_cdr_lpf_lck2ref(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_2_FLD_rg_c_da_cdr_lpf_lck2ref, (reg32), (val))
#define PON_DA_MUX_CTRL_2_SET_rg_f_da_cdr_lpf_lck2ref(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_2_FLD_rg_f_da_cdr_lpf_lck2ref, (reg32), (val))

#define PON_DA_MUX_CTRL_3_SET_rg_pon_tx_gpon_sel(reg32, val)   REG_FLD_SET(PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_sel, (reg32), (val))
#define PON_DA_MUX_CTRL_3_SET_rg_pon_tx_gpon_mux(reg32, val)   REG_FLD_SET(PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_mux, (reg32), (val))

#define PON_DA_MUX_CTRL_4_SET_rg_pma_rx_sigdet_pro(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_pro, (reg32), (val))
#define PON_DA_MUX_CTRL_4_SET_rg_pma_rx_sigdet_en(reg32, val)  REG_FLD_SET(PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_en, (reg32), (val))
#define PON_DA_MUX_CTRL_4_SET_rg_rx_sigdet_en(reg32, val)      REG_FLD_SET(PON_DA_MUX_CTRL_4_FLD_rg_rx_sigdet_en, (reg32), (val))

#define PON_DA_MUX_CTRL_5_SET_rg_disb_da_pon_tx_data(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_5_FLD_rg_disb_da_pon_tx_data, (reg32), (val))
#define PON_DA_MUX_CTRL_5_SET_rg_force_da_pon_tx_data(reg32, val) REG_FLD_SET(PON_DA_MUX_CTRL_5_FLD_rg_force_da_pon_tx_data, (reg32), (val))

#define PON_MODE_FORCE_CTRL_0_SET_rg_force_ge_rxpcsmodesel(reg32, val) REG_FLD_SET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxpcsmodesel, (reg32), (val))
#define PON_MODE_FORCE_CTRL_0_SET_rg_force_ge_txpcsmodesel(reg32, val) REG_FLD_SET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txpcsmodesel, (reg32), (val))
#define PON_MODE_FORCE_CTRL_0_SET_rg_force_ge_rxmapmodesel(reg32, val) REG_FLD_SET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxmapmodesel, (reg32), (val))
#define PON_MODE_FORCE_CTRL_0_SET_rg_force_ge_txmapmodesel(reg32, val) REG_FLD_SET(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txmapmodesel, (reg32), (val))

#define CLK_GEN_FORCE_CTRL_0_SET_rg_or_baser_rxck(reg32, val)  REG_FLD_SET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_rxck, (reg32), (val))
#define CLK_GEN_FORCE_CTRL_0_SET_rg_or_baser_txck(reg32, val)  REG_FLD_SET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_txck, (reg32), (val))
#define CLK_GEN_FORCE_CTRL_0_SET_rg_or_hg_rx500m_ck(reg32, val) REG_FLD_SET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_rx500m_ck, (reg32), (val))
#define CLK_GEN_FORCE_CTRL_0_SET_rg_or_pon_rx500m_ck(reg32, val) REG_FLD_SET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_rx500m_ck, (reg32), (val))
#define CLK_GEN_FORCE_CTRL_0_SET_rg_or_sh_tx250m_ck(reg32, val) REG_FLD_SET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_sh_tx250m_ck, (reg32), (val))
#define CLK_GEN_FORCE_CTRL_0_SET_rg_or_hg_tx250m_ck(reg32, val) REG_FLD_SET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_tx250m_ck, (reg32), (val))
#define CLK_GEN_FORCE_CTRL_0_SET_rg_or_pon_tx250m_ck(reg32, val) REG_FLD_SET(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_tx250m_ck, (reg32), (val))

#define ANALOG_FORCE_CTRL_0_SET_rg_sigdet_force_value(reg32, val) REG_FLD_SET(ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_value, (reg32), (val))
#define ANALOG_FORCE_CTRL_0_SET_rg_sigdet_force_mode(reg32, val) REG_FLD_SET(ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_mode, (reg32), (val))

#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_RX_DFE_RST(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_RST, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_RX_DFE_EN(reg32, val)  REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_EN, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_RX_FR_MODE(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_FR_MODE, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_FORCE_RX_FR_MODE(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_FORCE_RX_FR_MODE, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_TRI_DET_EN(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_TRI_DET_EN, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_STOPTIME(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_STOPTIME, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_RX500M_CK_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_RX500M_CK_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_PILPO_ROUT(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_PILPO_ROUT, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_GATED_RXD_B(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_GATED_RXD_B, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_EYE_MON_EN(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_MON_EN, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_EYE_CNT_EN(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_CNT_EN, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_DFE_TOG(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFE_TOG, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_DFEYEN_STOP_DIS(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEYEN_STOP_DIS, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_DFEX_RST(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_RST, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_DFEX_EN(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_EN, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_DFEX_DIS(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_DIS, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_DFEXEN_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEXEN_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_CHK_EYE_H(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_CHK_EYE_H, (reg32), (val))
#define PON_RXFEDIG_CTRL_0_SET_RG_SSUSB_EQ_ALGRTHM_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_ALGRTHM_SEL, (reg32), (val))

#define PON_RXFEDIG_CTRL_1_SET_RG_SSUSB_EQ_DD1LOS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1LOS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_1_SET_RG_SSUSB_EQ_DD1HOS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1HOS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_1_SET_RG_SSUSB_EQ_DD0LOS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0LOS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_1_SET_RG_SSUSB_EQ_DD0HOS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0HOS_LFI, (reg32), (val))

#define PON_RXFEDIG_CTRL_2_SET_RG_SSUSB_EQ_DEYE1OS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE1OS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_2_SET_RG_SSUSB_EQ_DEYE0OS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE0OS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_2_SET_RG_SSUSB_EQ_DE1OS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE1OS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_2_SET_RG_SSUSB_EQ_DE0OS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE0OS_LFI, (reg32), (val))

#define PON_RXFEDIG_CTRL_3_SET_RG_SSUSB_EQ_DHHL_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHL_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_3_SET_RG_SSUSB_EQ_DHHLOS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHLOS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_3_SET_RG_SSUSB_EQ_DFEYEN_DUR(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEYEN_DUR, (reg32), (val))
#define PON_RXFEDIG_CTRL_3_SET_RG_SSUSB_EQ_DFEX_LF_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEX_LF_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_3_SET_RG_SSUSB_EQ_DFEXEN_DUR(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEXEN_DUR, (reg32), (val))

#define PON_RXFEDIG_CTRL_4_SET_RG_SSUSB_EQ_DLEQ_LFI_GEN3(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN3, (reg32), (val))
#define PON_RXFEDIG_CTRL_4_SET_RG_SSUSB_EQ_DLEQ_LFI_GEN2(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN2, (reg32), (val))
#define PON_RXFEDIG_CTRL_4_SET_RG_SSUSB_EQ_DLEQ_LFI_GEN1(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN1, (reg32), (val))
#define PON_RXFEDIG_CTRL_4_SET_RG_SSUSB_EQ_DLEQOS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQOS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_4_SET_RG_SSUSB_EQ_DHHL_LF_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DHHL_LF_SEL, (reg32), (val))

#define PON_RXFEDIG_CTRL_5_SET_RG_SSUSB_EQ_DSAOS_LF_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DSAOS_LF_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_5_SET_RG_SSUSB_EQ_DLHL_LF_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LF_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_5_SET_RG_SSUSB_EQ_DLHL_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_5_SET_RG_SSUSB_EQ_DLHLOS_LFI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHLOS_LFI, (reg32), (val))
#define PON_RXFEDIG_CTRL_5_SET_RG_SSUSB_EQ_DLEQ_LF_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LF_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_5_SET_RG_SSUSB_EQ_DLEQ_LFI_GEN4(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN4, (reg32), (val))

#define PON_RXFEDIG_CTRL_6_SET_RG_SSUSB_EQ_EYE1_Y(reg32, val)  REG_FLD_SET(PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE1_Y, (reg32), (val))
#define PON_RXFEDIG_CTRL_6_SET_RG_SSUSB_EQ_EYE0_Y(reg32, val)  REG_FLD_SET(PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE0_Y, (reg32), (val))

#define PON_RXFEDIG_CTRL_7_SET_RG_SSUSB_EQ_EYE_CNT(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_7_FLD_RG_SSUSB_EQ_EYE_CNT, (reg32), (val))

#define PON_RXFEDIG_CTRL_8_SET_RG_SSUSB_EQ_LEQMAX(reg32, val)  REG_FLD_SET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_LEQMAX, (reg32), (val))
#define PON_RXFEDIG_CTRL_8_SET_RG_SSUSB_EQ_ISIFLAG_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_ISIFLAG_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_8_SET_RG_SSUSB_EQ_EYE_XOFFSET(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_XOFFSET, (reg32), (val))
#define PON_RXFEDIG_CTRL_8_SET_RG_SSUSB_EQ_EYE_MASK(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_MASK, (reg32), (val))

#define PON_RXFEDIG_CTRL_9_SET_RG_SSUSB_EQ_PI0_INI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_PI0_INI, (reg32), (val))
#define PON_RXFEDIG_CTRL_9_SET_RG_SSUSB_EQ_MON_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_MON_SEL, (reg32), (val))
#define PON_RXFEDIG_CTRL_9_SET_RG_SSUSB_EQ_LEQ_STOP_TO(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_STOP_TO, (reg32), (val))
#define PON_RXFEDIG_CTRL_9_SET_RG_SSUSB_EQ_LEQ_SHIFT(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_SHIFT, (reg32), (val))
#define PON_RXFEDIG_CTRL_9_SET_RG_SSUSB_EQ_LEQOSC_DLYCNT(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQOSC_DLYCNT, (reg32), (val))

#define PON_RXFEDIG_CTRL_10_SET_RG_SSUSB_EQ_PI_KPGAIN(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI_KPGAIN, (reg32), (val))
#define PON_RXFEDIG_CTRL_10_SET_RG_SSUSB_EQ_PIEYE_INI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PIEYE_INI, (reg32), (val))
#define PON_RXFEDIG_CTRL_10_SET_RG_SSUSB_EQ_PI90_INI(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90_INI, (reg32), (val))
#define PON_RXFEDIG_CTRL_10_SET_RG_SSUSB_EQ_PI90CK_SEL(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90CK_SEL, (reg32), (val))

#define PON_RXFEDIG_CTRL_11_SET_RG_SSUSB_EQ_RESERVED(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_11_FLD_RG_SSUSB_EQ_RESERVED, (reg32), (val))

#define PON_RXFEDIG_CTRL_12_SET_RG_SSUSB_EQ_REV(reg32, val)    REG_FLD_SET(PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV, (reg32), (val))
#define PON_RXFEDIG_CTRL_12_SET_RG_SSUSB_EQ_REV_1(reg32, val)  REG_FLD_SET(PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV_1, (reg32), (val))

#define PON_RXFEDIG_CTRL_13_SET_RG_SSUSB_EQ_STARTTIME(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_STARTTIME, (reg32), (val))
#define PON_RXFEDIG_CTRL_13_SET_RG_SSUSB_EQ_SIGDET(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SIGDET, (reg32), (val))
#define PON_RXFEDIG_CTRL_13_SET_RG_SSUSB_EQ_SD_CNT1(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT1, (reg32), (val))
#define PON_RXFEDIG_CTRL_13_SET_RG_SSUSB_EQ_SD_CNT0(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT0, (reg32), (val))

#define PON_RXFEDIG_CTRL_14_SET_RG_SSUSB_EQ_TRI_DET_TH(reg32, val) REG_FLD_SET(PON_RXFEDIG_CTRL_14_FLD_RG_SSUSB_EQ_TRI_DET_TH, (reg32), (val))

#define PON_RXFEDIG_STS_0_SET_RGS_SSUSB_EQ_LEQ_STOP(reg32, val) REG_FLD_SET(PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_LEQ_STOP, (reg32), (val))
#define PON_RXFEDIG_STS_0_SET_RGS_SSUSB_EQ_EYE_CNT_RDY(reg32, val) REG_FLD_SET(PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_EYE_CNT_RDY, (reg32), (val))

#define PON_RXFEDIG_STS_1_SET_RGS_SSUSB_EQ_DCD1L(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1L, (reg32), (val))
#define PON_RXFEDIG_STS_1_SET_RGS_SSUSB_EQ_DCD1H(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1H, (reg32), (val))
#define PON_RXFEDIG_STS_1_SET_RGS_SSUSB_EQ_DCD0L(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0L, (reg32), (val))
#define PON_RXFEDIG_STS_1_SET_RGS_SSUSB_EQ_DCD0H(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0H, (reg32), (val))

#define PON_RXFEDIG_STS_2_SET_RGS_SSUSB_EQ_DCEYE1(reg32, val)  REG_FLD_SET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE1, (reg32), (val))
#define PON_RXFEDIG_STS_2_SET_RGS_SSUSB_EQ_DCEYE0(reg32, val)  REG_FLD_SET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE0, (reg32), (val))
#define PON_RXFEDIG_STS_2_SET_RGS_SSUSB_EQ_DCE1(reg32, val)    REG_FLD_SET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE1, (reg32), (val))
#define PON_RXFEDIG_STS_2_SET_RGS_SSUSB_EQ_DCE0(reg32, val)    REG_FLD_SET(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE0, (reg32), (val))

#define PON_RXFEDIG_STS_3_SET_RGS_SSUSB_EQ_DCLHL(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLHL, (reg32), (val))
#define PON_RXFEDIG_STS_3_SET_RGS_SSUSB_EQ_DCLEQOS(reg32, val) REG_FLD_SET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQOS, (reg32), (val))
#define PON_RXFEDIG_STS_3_SET_RGS_SSUSB_EQ_DCLEQ(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQ, (reg32), (val))
#define PON_RXFEDIG_STS_3_SET_RGS_SSUSB_EQ_DCHHL(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCHHL, (reg32), (val))

#define PON_RXFEDIG_STS_4_SET_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0(reg32, val) REG_FLD_SET(PON_RXFEDIG_STS_4_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0, (reg32), (val))

#define PON_RXFEDIG_STS_5_SET_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1(reg32, val) REG_FLD_SET(PON_RXFEDIG_STS_5_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1, (reg32), (val))

#define PON_RXFEDIG_STS_7_SET_RGS_SSUSB_EQ_STATUS(reg32, val)  REG_FLD_SET(PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_STATUS, (reg32), (val))
#define PON_RXFEDIG_STS_7_SET_RGS_SSUSB_EQ_PILPO(reg32, val)   REG_FLD_SET(PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_PILPO, (reg32), (val))

#define PON_RXFEDIG_STS_8_SET_RGS_SSUSB_RX_SD_OUT(reg32, val)  REG_FLD_SET(PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_RX_SD_OUT, (reg32), (val))
#define PON_RXFEDIG_STS_8_SET_RGS_SSUSB_EQ_PROBE_OUT(reg32, val) REG_FLD_SET(PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_EQ_PROBE_OUT, (reg32), (val))

#define PON_DA_CTRL_0_SET_RG_SSUSB_PLL_IC(reg32, val)          REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IC, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_PLL_BR(reg32, val)          REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BR, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_PLL_IR(reg32, val)          REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IR, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_PLL_BP(reg32, val)          REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BP, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_PLL_LOAD_EN(reg32, val)     REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_LOAD_EN, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_PLL_FBKSEL(reg32, val)      REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_FBKSEL, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_PCIE_CKDET_EN(reg32, val)         REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_PCIE_CKDET_EN, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_SCAN_MODE(reg32, val)       REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_SCAN_MODE, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_PCIE_CLKRX_EN(reg32, val)         REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_PCIE_CLKRX_EN, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_XTAL_EXT_EN(reg32, val)     REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_XTAL_EXT_EN, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_PCIE_DEVICE(reg32, val)           REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_PCIE_DEVICE, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_PCIE_REFCK_DIV4(reg32, val)       REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_PCIE_REFCK_DIV4, (reg32), (val))
#define PON_DA_CTRL_0_SET_RG_SSUSB_RX_IMPCALIB_EN(reg32, val)  REG_FLD_SET(PON_DA_CTRL_0_FLD_RG_SSUSB_RX_IMPCALIB_EN, (reg32), (val))

#define PON_DA_CTRL_1_SET_RG_SSUSB_CDR_BPB(reg32, val)         REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_CDR_BPB, (reg32), (val))
#define PON_DA_CTRL_1_SET_RG_SSUSB_TX_IMPCALIB_EN(reg32, val)  REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_TX_IMPCALIB_EN, (reg32), (val))
#define PON_DA_CTRL_1_SET_RG_SSUSB_TX_DEM(reg32, val)          REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_TX_DEM, (reg32), (val))
#define PON_DA_CTRL_1_SET_RG_SSUSB_RXDET_EN(reg32, val)        REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_RXDET_EN, (reg32), (val))
#define PON_DA_CTRL_1_SET_RG_SSUSB_PLL_REFCKDIV(reg32, val)    REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_REFCKDIV, (reg32), (val))
#define PON_DA_CTRL_1_SET_RG_SSUSB_PLL_PREDIV(reg32, val)      REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_PREDIV, (reg32), (val))
#define PON_DA_CTRL_1_SET_RG_SSUSB_PLL_BC(reg32, val)          REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_BC, (reg32), (val))
#define PON_DA_CTRL_1_SET_RG_SSUSB_PLL_DIVEN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_DIVEN, (reg32), (val))

#define PON_DA_CTRL_2_SET_RG_SSUSB_EQ_RSTEP1(reg32, val)       REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP1, (reg32), (val))
#define PON_DA_CTRL_2_SET_RG_SSUSB_EQ_RSTEP2(reg32, val)       REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP2, (reg32), (val))
#define PON_DA_CTRL_2_SET_RG_SSUSB_LFPS_PWD(reg32, val)        REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_LFPS_PWD, (reg32), (val))
#define PON_DA_CTRL_2_SET_RG_SSUSB_CDR_PI_PWD(reg32, val)      REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_PWD, (reg32), (val))
#define PON_DA_CTRL_2_SET_RG_SSUSB_CDR_PI_MODE(reg32, val)     REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_MODE, (reg32), (val))
#define PON_DA_CTRL_2_SET_RG_SSUSB_CDR_TCADJ(reg32, val)       REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_TCADJ, (reg32), (val))
#define PON_DA_CTRL_2_SET_RG_SSUSB_CDR_FBDIV(reg32, val)       REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_FBDIV, (reg32), (val))
#define PON_DA_CTRL_2_SET_RG_SSUSB_CDR_BPA(reg32, val)         REG_FLD_SET(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_BPA, (reg32), (val))

#define PON_DA_CTRL_3_SET_RG_SSUSB_SYSPLL_FBDIV_CHG(reg32, val) REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_SSUSB_SYSPLL_FBDIV_CHG, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_PCIE_CLKTX_EN(reg32, val)         REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_PCIE_CLKTX_EN, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_PCIE_CRSDET_EN(reg32, val)        REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_EN, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_PCIE_CRSDET_RSTB(reg32, val)      REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_RSTB, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_PCIE_CRSDET_OFST(reg32, val)      REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_OFST, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_SSUSB_RX_HZ(reg32, val)           REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_HZ, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_SSUSB_RX_P3_ENTRY(reg32, val)     REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P3_ENTRY, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_SSUSB_RX_P1_ENTRY(reg32, val)     REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P1_ENTRY, (reg32), (val))
#define PON_DA_CTRL_3_SET_RG_SSUSB_LFPS_DEGLITCH(reg32, val)   REG_FLD_SET(PON_DA_CTRL_3_FLD_RG_SSUSB_LFPS_DEGLITCH, (reg32), (val))

#define PON_DA_CTRL_4_SET_RG_SSUSB_PLL_FBKDIV(reg32, val)      REG_FLD_SET(PON_DA_CTRL_4_FLD_RG_SSUSB_PLL_FBKDIV, (reg32), (val))

#define PON_DA_CTRL_5_SET_RG_FORCE_CDR_PI0(reg32, val)         REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PI0, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_SYSPLL_SDM_PWR_ON(reg32, val) REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_PWR_ON, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_SYSPLL_SDM_ISO_EN(reg32, val) REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_ISO_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_CDR_PWD(reg32, val)         REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_CDR_AUTOK_VCO(reg32, val)   REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_AUTOK_VCO, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_CDR_LCK2REF(reg32, val)     REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_LCK2REF, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_RX_CMPWD(reg32, val)        REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_CMPWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_RX_AFE_PWD(reg32, val)      REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_AFE_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_TX_DATA_EN(reg32, val)      REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_DATA_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_TX_EIDLE_LP_EN(reg32, val)  REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_EIDLE_LP_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_TX_SER_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_SER_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_SIGDET_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_SIGDET_CAL_EN(reg32, val)   REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_SIGDET_CAL_OFFSET(reg32, val) REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_OFFSET, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_RX_PI_CAL_EN(reg32, val)    REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_PI_CAL_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_TX_DRV_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_DRV_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_TX_LFPS(reg32, val)         REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_TX_LFPS_EN(reg32, val)      REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_RX_SAOSC_EN(reg32, val)     REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_SAOSC_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_BIAS_PWD(reg32, val)        REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_BIAS_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_BIASIMR_PWD(reg32, val)     REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_BIASIMR_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_BG_LPF_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_BG_LPF_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_PLL_BIAS_PWD(reg32, val)    REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_XTAL_RX_PWD(reg32, val)     REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_XTAL_RX_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_SYSPLL_PWD(reg32, val)      REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_PLL_PWD(reg32, val)         REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_PWD, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_PLL_AUTOK_VCO(reg32, val)   REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_AUTOK_VCO, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_PLL_SSC_EN(reg32, val)      REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_SSC_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_PLL_BIAS_LPF_EN(reg32, val) REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_LPF_EN, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_CDR_BAND(reg32, val)        REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_BAND, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_PLL_BAND(reg32, val)        REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BAND, (reg32), (val))
#define PON_DA_CTRL_5_SET_RG_FORCE_RX_LEQOS_EN(reg32, val)     REG_FLD_SET(PON_DA_CTRL_5_FLD_RG_FORCE_RX_LEQOS_EN, (reg32), (val))

#define PON_DA_CTRL_6_SET_RG_FORCE_IEXT_INTR_CTRL(reg32, val)  REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_IEXT_INTR_CTRL, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_RX_IMP_SEL(reg32, val)      REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_IMP_SEL, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_TX_IMP_SEL(reg32, val)      REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_IMP_SEL, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_RX_EQ_EN(reg32, val)        REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_EN, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_RX_EQ_RST(reg32, val)       REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_RST, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_RX_SAOSC_RST(reg32, val)    REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_SAOSC_RST, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_RX_CDR_STABLE(reg32, val)   REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_RX_CDR_STABLE, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_CDR_KVSEL(reg32, val)       REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_KVSEL, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_CDR_BR(reg32, val)          REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BR, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_CDR_BC(reg32, val)          REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BC, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_CDR_BIC(reg32, val)         REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIC, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_CDR_BIR(reg32, val)         REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIR, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_TX_EIDLE_CM(reg32, val)     REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_EIDLE_CM, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_TX_BIASI_EN(reg32, val)     REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI_EN, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_TX_BIASI(reg32, val)        REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_IDEM_6DB(reg32, val)        REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_6DB, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_IDRV_6DB(reg32, val)        REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_6DB, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_IDEM_3P5DB(reg32, val)      REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_3P5DB, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_IDRV_3P5DB(reg32, val)      REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_3P5DB, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_IDRV_0DB(reg32, val)        REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_0DB, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_SYSPLL_PREDIV(reg32, val)   REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_PREDIV, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_SYSPLL_RESERVE(reg32, val)  REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_RESERVE, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_SYSPLL_FBDIV(reg32, val)    REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_FBDIV, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_PLL_SSC_PRD(reg32, val)     REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_PRD, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_PLL_SSC_DELTA(reg32, val)   REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_PLL_SSC_DELTA1(reg32, val)  REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA1, (reg32), (val))
#define PON_DA_CTRL_6_SET_RG_FORCE_CDR_PI90(reg32, val)        REG_FLD_SET(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_PI90, (reg32), (val))

#define PON_DA_CTRL_7_SET_RG_SSUSB_BG_LPF_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_BG_LPF_EN, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_PLL_BIAS_PWD(reg32, val)    REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_PWD, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_XTAL_RX_PWD(reg32, val)     REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_XTAL_RX_PWD, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_SYSPLL_PWD(reg32, val)      REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_SYSPLL_PWD, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_PLL_PWD(reg32, val)         REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_PWD, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_PLL_AUTOK_VCO(reg32, val)   REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_AUTOK_VCO, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_PLL_SSC_EN(reg32, val)      REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_SSC_EN, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_PLL_BIAS_LPF_EN(reg32, val) REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_LPF_EN, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_CDR_BAND(reg32, val)        REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_CDR_BAND, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_RX_LEQOS_EN(reg32, val)     REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_RX_LEQOS_EN, (reg32), (val))
#define PON_DA_CTRL_7_SET_RG_SSUSB_PLL_BAND(reg32, val)        REG_FLD_SET(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BAND, (reg32), (val))

#define PON_DA_CTRL_8_SET_RG_SSUSB_SYSPLL_SDM_PWR_ON(reg32, val) REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_PWR_ON, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_SYSPLL_SDM_ISO_EN(reg32, val) REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_ISO_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_CDR_PWD(reg32, val)         REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_PWD, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_CDR_AUTOK_VCO(reg32, val)   REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_AUTOK_VCO, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_CDR_LCK2REF(reg32, val)     REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_LCK2REF, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_RX_CMPWD(reg32, val)        REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_CMPWD, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_RX_AFE_PWD(reg32, val)      REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_AFE_PWD, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_TX_DATA_EN(reg32, val)      REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DATA_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_TX_EIDLE_LP_EN(reg32, val)  REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_EIDLE_LP_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_TX_SER_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_SER_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_PCIE_SIGDET_EN(reg32, val)        REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_PCIE_SIGDET_CAL_EN(reg32, val)    REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_CAL_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_SIGDET_CAL_OFFSET(reg32, val) REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_SIGDET_CAL_OFFSET, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_RX_PI_CAL_EN(reg32, val)    REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_PI_CAL_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_TX_DRV_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DRV_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_TX_LFPS(reg32, val)         REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_TX_LFPS_EN(reg32, val)      REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_RX_SAOSC_EN(reg32, val)     REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_SAOSC_EN, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_BIAS_PWD(reg32, val)        REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_BIAS_PWD, (reg32), (val))
#define PON_DA_CTRL_8_SET_RG_SSUSB_BIASIMR_PWD(reg32, val)     REG_FLD_SET(PON_DA_CTRL_8_FLD_RG_SSUSB_BIASIMR_PWD, (reg32), (val))

#define PON_DA_CTRL_9_SET_RG_SSUSB_CDR_PI90(reg32, val)        REG_FLD_SET(PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI90, (reg32), (val))
#define PON_DA_CTRL_9_SET_RG_SSUSB_CDR_PI0(reg32, val)         REG_FLD_SET(PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI0, (reg32), (val))

#define PON_DA_CTRL_10_SET_RG_SSUSB_PLL_SSC_DELTA(reg32, val)  REG_FLD_SET(PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA, (reg32), (val))
#define PON_DA_CTRL_10_SET_RG_SSUSB_PLL_SSC_DELTA1(reg32, val) REG_FLD_SET(PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA1, (reg32), (val))

#define PON_DA_CTRL_11_SET_RG_SSUSB_SYSPLL_FBDIV(reg32, val)   REG_FLD_SET(PON_DA_CTRL_11_FLD_RG_SSUSB_SYSPLL_FBDIV, (reg32), (val))

#define PON_DA_CTRL_12_SET_RG_SSUSB_IDRV_3P5DB(reg32, val)     REG_FLD_SET(PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_3P5DB, (reg32), (val))
#define PON_DA_CTRL_12_SET_RG_SSUSB_IDRV_0DB(reg32, val)       REG_FLD_SET(PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_0DB, (reg32), (val))

#define PON_DA_CTRL_13_SET_RG_SSUSB_TX_BIASI_EN(reg32, val)    REG_FLD_SET(PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI_EN, (reg32), (val))
#define PON_DA_CTRL_13_SET_RG_SSUSB_TX_BIASI(reg32, val)       REG_FLD_SET(PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI, (reg32), (val))
#define PON_DA_CTRL_13_SET_RG_SSUSB_IDEM_6DB(reg32, val)       REG_FLD_SET(PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_6DB, (reg32), (val))
#define PON_DA_CTRL_13_SET_RG_SSUSB_IDRV_6DB(reg32, val)       REG_FLD_SET(PON_DA_CTRL_13_FLD_RG_SSUSB_IDRV_6DB, (reg32), (val))
#define PON_DA_CTRL_13_SET_RG_SSUSB_IDEM_3P5DB(reg32, val)     REG_FLD_SET(PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_3P5DB, (reg32), (val))

#define PON_DA_CTRL_14_SET_RG_SSUSB_RX_IMP_SEL(reg32, val)     REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_IMP_SEL, (reg32), (val))
#define PON_DA_CTRL_14_SET_RG_SSUSB_TX_IMP_SEL(reg32, val)     REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_TX_IMP_SEL, (reg32), (val))
#define PON_DA_CTRL_14_SET_RG_SSUSB_RX_EQ_EN(reg32, val)       REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_EN, (reg32), (val))
#define PON_DA_CTRL_14_SET_RG_SSUSB_RX_EQ_RST(reg32, val)      REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_RST, (reg32), (val))
#define PON_DA_CTRL_14_SET_RG_SSUSB_RX_SAOSC_RST(reg32, val)   REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_SAOSC_RST, (reg32), (val))
#define PON_DA_CTRL_14_SET_RG_SSUSB_RX_CDR_STABLE(reg32, val)  REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_CDR_STABLE, (reg32), (val))
#define PON_DA_CTRL_14_SET_RG_SSUSB_CDR_BIR(reg32, val)        REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_CDR_BIR, (reg32), (val))
#define PON_DA_CTRL_14_SET_RG_SSUSB_TX_EIDLE_CM(reg32, val)    REG_FLD_SET(PON_DA_CTRL_14_FLD_RG_SSUSB_TX_EIDLE_CM, (reg32), (val))

#define PON_DA_CTRL_15_SET_RG_SSUSB_CDR_BIRLTD1(reg32, val)    REG_FLD_SET(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD1, (reg32), (val))
#define PON_DA_CTRL_15_SET_RG_SSUSB_CDR_BIRLTD0(reg32, val)    REG_FLD_SET(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD0, (reg32), (val))
#define PON_DA_CTRL_15_SET_RG_SSUSB_CDR_BIRLTR(reg32, val)     REG_FLD_SET(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTR, (reg32), (val))

#define PON_DA_CTRL_16_SET_RG_SSUSB_CDR_BCLTD0(reg32, val)     REG_FLD_SET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTD0, (reg32), (val))
#define PON_DA_CTRL_16_SET_RG_SSUSB_CDR_BCLTR(reg32, val)      REG_FLD_SET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTR, (reg32), (val))
#define PON_DA_CTRL_16_SET_RG_SSUSB_CDR_BICLTD1(reg32, val)    REG_FLD_SET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD1, (reg32), (val))
#define PON_DA_CTRL_16_SET_RG_SSUSB_CDR_BICLTD0(reg32, val)    REG_FLD_SET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD0, (reg32), (val))
#define PON_DA_CTRL_16_SET_RG_SSUSB_CDR_BICLTR(reg32, val)     REG_FLD_SET(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTR, (reg32), (val))

#define PON_DA_CTRL_17_SET_RG_SSUSB_CDR_KVSELLTD1(reg32, val)  REG_FLD_SET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD1, (reg32), (val))
#define PON_DA_CTRL_17_SET_RG_SSUSB_CDR_KVSELLTD0(reg32, val)  REG_FLD_SET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD0, (reg32), (val))
#define PON_DA_CTRL_17_SET_RG_SSUSB_CDR_KVSELLTR(reg32, val)   REG_FLD_SET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTR, (reg32), (val))
#define PON_DA_CTRL_17_SET_RG_SSUSB_CDR_BRLTD1(reg32, val)     REG_FLD_SET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD1, (reg32), (val))
#define PON_DA_CTRL_17_SET_RG_SSUSB_CDR_BRLTD0(reg32, val)     REG_FLD_SET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD0, (reg32), (val))
#define PON_DA_CTRL_17_SET_RG_SSUSB_CDR_BRLTR(reg32, val)      REG_FLD_SET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTR, (reg32), (val))
#define PON_DA_CTRL_17_SET_RG_SSUSB_CDR_BCLTD1(reg32, val)     REG_FLD_SET(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BCLTD1, (reg32), (val))

#define PON_DA_CTRL_18_SET_RG_FORCE_SSUSB_PLL_BPB(reg32, val)  REG_FLD_SET(PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPB, (reg32), (val))
#define PON_DA_CTRL_18_SET_RG_FORCE_SSUSB_PLL_BPA(reg32, val)  REG_FLD_SET(PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPA, (reg32), (val))

#define PON_DA_CTRL_19_SET_RG_SSUSB_PLL_BPB(reg32, val)        REG_FLD_SET(PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPB, (reg32), (val))
#define PON_DA_CTRL_19_SET_RG_SSUSB_PLL_BPA(reg32, val)        REG_FLD_SET(PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPA, (reg32), (val))

#define PON_DEBUG_REGISTER_0_SET_ro_ad_ben_out_cnt(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out_cnt, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_ad_ben_out(reg32, val)     REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_intf_pma_rx_sigdet_out(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_intf_pma_rx_sigdet_out, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_ad_rx_lfps(reg32, val)     REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_lfps, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_ad_rx_impcal_out(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_impcal_out, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_rx_path_rdy(reg32, val)    REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_rx_path_rdy, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_rx_eq_en(reg32, val)       REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_rx_eq_en, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_eq_done(reg32, val)        REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_eq_done, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_ad_sigdet_cal_out(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_cal_out, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_ad_sigdet_out(reg32, val)  REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_out, (reg32), (val))
#define PON_DEBUG_REGISTER_0_SET_ro_lck2ref(reg32, val)        REG_FLD_SET(PON_DEBUG_REGISTER_0_FLD_ro_lck2ref, (reg32), (val))

#define PON_DEBUG_REGISTER_1_SET_rg_eq_done_clr(reg32, val)    REG_FLD_SET(PON_DEBUG_REGISTER_1_FLD_rg_eq_done_clr, (reg32), (val))

#define PON_DEBUG_REGISTER_2_SET_ro_qphy_rx_ctrl_debug(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_2_FLD_ro_qphy_rx_ctrl_debug, (reg32), (val))
#define PON_DEBUG_REGISTER_2_SET_ro_da_tx_data_en(reg32, val)  REG_FLD_SET(PON_DEBUG_REGISTER_2_FLD_ro_da_tx_data_en, (reg32), (val))
#define PON_DEBUG_REGISTER_2_SET_ro_txbit_dly_sel_latch(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel_latch, (reg32), (val))
#define PON_DEBUG_REGISTER_2_SET_ro_txbit_dly_sel(reg32, val)  REG_FLD_SET(PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel, (reg32), (val))
#define PON_DEBUG_REGISTER_2_SET_ro_pll_vco_cal_state(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_2_FLD_ro_pll_vco_cal_state, (reg32), (val))

#define PON_DEBUG_REGISTER_3_SET_ro_ad_pcie_crsdet_out(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_out, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_ad_pcie_crsdet_lthout(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_lthout, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_ad_pcie_crsdet_cal_out(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_cal_out, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_ad_pcie_ckdet_out(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_ckdet_out, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_saosc_done(reg32, val)     REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_saosc_done, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_rg_freq_clr(reg32, val)       REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_rg_freq_clr, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_baser_rx_freq_lock(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_baser_rx_freq_lock, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_baser_tx_freq_lock(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_baser_tx_freq_lock, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_hg_rx_freq_lock(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_hg_rx_freq_lock, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_hg_tx_freq_lock(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_hg_tx_freq_lock, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_pon_rx_freq_lock(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_pon_rx_freq_lock, (reg32), (val))
#define PON_DEBUG_REGISTER_3_SET_ro_pon_tx_freq_lock(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_3_FLD_ro_pon_tx_freq_lock, (reg32), (val))

#define PON_DEBUG_REGISTER_4_SET_rg_ben_count_en(reg32, val)   REG_FLD_SET(PON_DEBUG_REGISTER_4_FLD_rg_ben_count_en, (reg32), (val))
#define PON_DEBUG_REGISTER_4_SET_ro_ben_counter(reg32, val)    REG_FLD_SET(PON_DEBUG_REGISTER_4_FLD_ro_ben_counter, (reg32), (val))

#define PON_DUMMY_REG_0_SET_rg_dummy_reg(reg32, val)           REG_FLD_SET(PON_DUMMY_REG_0_FLD_rg_dummy_reg, (reg32), (val))

#define PON_DEBUG_REGISTER_5_SET_rg_fec_dec_insert_dbg_clr(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_insert_dbg_clr, (reg32), (val))
#define PON_DEBUG_REGISTER_5_SET_rg_fec_dec_usr_def_error_en(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_usr_def_error_en, (reg32), (val))
#define PON_DEBUG_REGISTER_5_SET_rg_fec_dec_err_insert_cont(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_cont, (reg32), (val))
#define PON_DEBUG_REGISTER_5_SET_rg_fec_dec_err_insert_en(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_en, (reg32), (val))

#define PON_DEBUG_REGISTER_6_SET_rg_fec_dec_err_byte_num(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_6_FLD_rg_fec_dec_err_byte_num, (reg32), (val))

#define PON_DEBUG_REGISTER_7_SET_rg_fec_dec_usr_def_error(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_7_FLD_rg_fec_dec_usr_def_error, (reg32), (val))

#define PON_DEBUG_REGISTER_8_SET_rg_fec_dec_err_insert_start_w1c(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_8_FLD_rg_fec_dec_err_insert_start_w1c, (reg32), (val))

#define PON_DEBUG_REGISTER_9_SET_rg_fec_enc_insert_dbg_clr(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_insert_dbg_clr, (reg32), (val))
#define PON_DEBUG_REGISTER_9_SET_rg_fec_enc_usr_def_error_en(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_usr_def_error_en, (reg32), (val))
#define PON_DEBUG_REGISTER_9_SET_rg_fec_enc_err_insert_cont(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_cont, (reg32), (val))
#define PON_DEBUG_REGISTER_9_SET_rg_fec_enc_err_insert_en(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_en, (reg32), (val))

#define PON_DEBUG_REGISTER_10_SET_rg_fec_enc_err_byte_num(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_10_FLD_rg_fec_enc_err_byte_num, (reg32), (val))

#define PON_DEBUG_REGISTER_11_SET_rg_fec_enc_usr_def_error(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_11_FLD_rg_fec_enc_usr_def_error, (reg32), (val))

#define PON_DEBUG_REGISTER_12_SET_rg_fec_enc_err_insert_start_w1c(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_12_FLD_rg_fec_enc_err_insert_start_w1c, (reg32), (val))

#define PON_DEBUG_REGISTER_13_SET_rg_trans_los_pro_cnt_thr(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_13_FLD_rg_trans_los_pro_cnt_thr, (reg32), (val))
#define PON_DEBUG_REGISTER_13_SET_rg_phyrdy_pro_cnt_thr(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_13_FLD_rg_phyrdy_pro_cnt_thr, (reg32), (val))

#define PON_DEBUG_REGISTER_14_SET_rg_int_record_start_clr_w1c(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_14_FLD_rg_int_record_start_clr_w1c, (reg32), (val))

#define PON_DEBUG_REGISTER_15_SET_rg_record_start_type(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_15_FLD_rg_record_start_type, (reg32), (val))
#define PON_DEBUG_REGISTER_15_SET_rg_record_start_dbg_clr(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_15_FLD_rg_record_start_dbg_clr, (reg32), (val))
#define PON_DEBUG_REGISTER_15_SET_rg_lof_int_mask_keep(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_keep, (reg32), (val))
#define PON_DEBUG_REGISTER_15_SET_rg_lof_int_mask_en(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_en, (reg32), (val))
#define PON_DEBUG_REGISTER_15_SET_rg_int_record_en(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_15_FLD_rg_int_record_en, (reg32), (val))

#define PON_DEBUG_REGISTER_16_SET_ro_lof_time_cnt(reg32, val)  REG_FLD_SET(PON_DEBUG_REGISTER_16_FLD_ro_lof_time_cnt, (reg32), (val))

#define PON_DEBUG_REGISTER_17_SET_ro_los_time_cnt(reg32, val)  REG_FLD_SET(PON_DEBUG_REGISTER_17_FLD_ro_los_time_cnt, (reg32), (val))

#define PON_DEBUG_REGISTER_18_SET_ro_phyrdy_time_cnt(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_18_FLD_ro_phyrdy_time_cnt, (reg32), (val))

#define PON_DEBUG_REGISTER_19_SET_ro_tx_bit_dly(reg32, val)    REG_FLD_SET(PON_DEBUG_REGISTER_19_FLD_ro_tx_bit_dly, (reg32), (val))

#define PON_DEBUG_REGISTER_20_SET_rg_int_record_cnt_thr(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_20_FLD_rg_int_record_cnt_thr, (reg32), (val))

#define PON_DEBUG_REGISTER_21_SET_ro_record_done(reg32, val)   REG_FLD_SET(PON_DEBUG_REGISTER_21_FLD_ro_record_done, (reg32), (val))
#define PON_DEBUG_REGISTER_21_SET_ro_fec_enc_insert_dbg(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_21_FLD_ro_fec_enc_insert_dbg, (reg32), (val))
#define PON_DEBUG_REGISTER_21_SET_ro_record_start_dbg(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_21_FLD_ro_record_start_dbg, (reg32), (val))
#define PON_DEBUG_REGISTER_21_SET_ro_fec_dec_insert_dbg(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_21_FLD_ro_fec_dec_insert_dbg, (reg32), (val))

#define PON_DEBUG_REGISTER_22_SET_rg_lof_int_mask_cnt_thr(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_22_FLD_rg_lof_int_mask_cnt_thr, (reg32), (val))

#define PON_DEBUG_REGISTER_23_SET_rgs_pll_autok_band(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_23_FLD_rgs_pll_autok_band, (reg32), (val))

#define PON_DEBUG_REGISTER_24_SET_rg_fpga_rx_lck_rst(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_lck_rst, (reg32), (val))
#define PON_DEBUG_REGISTER_24_SET_rg_fpga_rx_force_lck(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_force_lck, (reg32), (val))
#define PON_DEBUG_REGISTER_24_SET_ro_fpga_tx_los_lock_cnt(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_24_FLD_ro_fpga_tx_los_lock_cnt, (reg32), (val))
#define PON_DEBUG_REGISTER_24_SET_ro_fpga_rx_los_lock_cnt(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_24_FLD_ro_fpga_rx_los_lock_cnt, (reg32), (val))
#define PON_DEBUG_REGISTER_24_SET_ro_fpga_lock_status(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_24_FLD_ro_fpga_lock_status, (reg32), (val))

#define PON_DEBUG_REGISTER_25_SET_rg_gepon_phy_dlybnd_delsel(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_dlybnd_delsel, (reg32), (val))
#define PON_DEBUG_REGISTER_25_SET_rg_gepon_phy_epsram_delsel(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_epsram_delsel, (reg32), (val))
#define PON_DEBUG_REGISTER_25_SET_rg_pon_phy_ssram_delsel(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_ssram_delsel, (reg32), (val))
#define PON_DEBUG_REGISTER_25_SET_rg_pon_phy_dsram3_delsel(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram3_delsel, (reg32), (val))
#define PON_DEBUG_REGISTER_25_SET_rg_pon_phy_dsram2_delsel(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram2_delsel, (reg32), (val))
#define PON_DEBUG_REGISTER_25_SET_rg_pon_phy_dsram1_delsel(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram1_delsel, (reg32), (val))
#define PON_DEBUG_REGISTER_25_SET_rg_pon_phy_dsram0_delsel(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram0_delsel, (reg32), (val))

#define PON_DEBUG_REGISTER_26_SET_ro_fpga_debug_0(reg32, val)  REG_FLD_SET(PON_DEBUG_REGISTER_26_FLD_ro_fpga_debug_0, (reg32), (val))

#define PON_DEBUG_REGISTER_27_SET_ro_fpga_debug_1(reg32, val)  REG_FLD_SET(PON_DEBUG_REGISTER_27_FLD_ro_fpga_debug_1, (reg32), (val))

#define PON_DEBUG_REGISTER_28_SET_ro_ad_sig_cal_offset(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_28_FLD_ro_ad_sig_cal_offset, (reg32), (val))
#define PON_DEBUG_REGISTER_28_SET_ro_ad_cdr_band(reg32, val)   REG_FLD_SET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_band, (reg32), (val))
#define PON_DEBUG_REGISTER_28_SET_ro_ad_cdr_vcocal_fail(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_fail, (reg32), (val))
#define PON_DEBUG_REGISTER_28_SET_ro_ad_cdr_vcocal_cplt(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_cplt, (reg32), (val))
#define PON_DEBUG_REGISTER_28_SET_ro_ad_cdr_vcocal_state(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_state, (reg32), (val))

#define PON_DEBUG_REGISTER_29_SET_ro_sfp_tx_en_counter(reg32, val) REG_FLD_SET(PON_DEBUG_REGISTER_29_FLD_ro_sfp_tx_en_counter, (reg32), (val))

#define PON_INTERRUPT_STS_0_SET_ro_tx_en_int(reg32, val)       REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_tx_en_int, (reg32), (val))
#define PON_INTERRUPT_STS_0_SET_ro_rx_sd_int(reg32, val)       REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_rx_sd_int, (reg32), (val))
#define PON_INTERRUPT_STS_0_SET_ro_los_int(reg32, val)         REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_los_int, (reg32), (val))
#define PON_INTERRUPT_STS_0_SET_ro_rx_pll_meter_unlock(reg32, val) REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_unlock, (reg32), (val))
#define PON_INTERRUPT_STS_0_SET_ro_rx_pll_meter_lock(reg32, val) REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_lock, (reg32), (val))
#define PON_INTERRUPT_STS_0_SET_ro_pma_rx_sigdet_out(reg32, val) REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_pma_rx_sigdet_out, (reg32), (val))
#define PON_INTERRUPT_STS_0_SET_ro_ncpo_min_flag(reg32, val)   REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_ncpo_min_flag, (reg32), (val))
#define PON_INTERRUPT_STS_0_SET_ro_ncpo_max_flag(reg32, val)   REG_FLD_SET(PON_INTERRUPT_STS_0_FLD_ro_ncpo_max_flag, (reg32), (val))

#define PON_INTERRUPT_EN_0_SET_rg_tx_en_int_en(reg32, val)     REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_tx_en_int_en, (reg32), (val))
#define PON_INTERRUPT_EN_0_SET_rg_rx_sd_int_en(reg32, val)     REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_rx_sd_int_en, (reg32), (val))
#define PON_INTERRUPT_EN_0_SET_rg_los_int_en(reg32, val)       REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_los_int_en, (reg32), (val))
#define PON_INTERRUPT_EN_0_SET_rg_rx_pll_meter_unlock_int_en(reg32, val) REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_unlock_int_en, (reg32), (val))
#define PON_INTERRUPT_EN_0_SET_rg_rx_pll_meter_lock_int_en(reg32, val) REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_lock_int_en, (reg32), (val))
#define PON_INTERRUPT_EN_0_SET_rg_pma_sigdet_int_en(reg32, val) REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_pma_sigdet_int_en, (reg32), (val))
#define PON_INTERRUPT_EN_0_SET_rg_ncpo_min_int_en(reg32, val)  REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_ncpo_min_int_en, (reg32), (val))
#define PON_INTERRUPT_EN_0_SET_rg_ncpo_max_int_en(reg32, val)  REG_FLD_SET(PON_INTERRUPT_EN_0_FLD_rg_ncpo_max_int_en, (reg32), (val))

#define PON_INTERRUPT_CLR_0_SET_rg_tx_en_int_clr(reg32, val)   REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_tx_en_int_clr, (reg32), (val))
#define PON_INTERRUPT_CLR_0_SET_rg_rx_sd_int_clr(reg32, val)   REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_rx_sd_int_clr, (reg32), (val))
#define PON_INTERRUPT_CLR_0_SET_rg_los_int_clr(reg32, val)     REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_los_int_clr, (reg32), (val))
#define PON_INTERRUPT_CLR_0_SET_rg_rx_pll_meter_unlock_int_clr(reg32, val) REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_unlock_int_clr, (reg32), (val))
#define PON_INTERRUPT_CLR_0_SET_rg_rx_pll_meter_lock_int_clr(reg32, val) REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_lock_int_clr, (reg32), (val))
#define PON_INTERRUPT_CLR_0_SET_rg_pma_rx_sigdet_intclr(reg32, val) REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_pma_rx_sigdet_intclr, (reg32), (val))
#define PON_INTERRUPT_CLR_0_SET_rg_ncpo_flag_clr(reg32, val)   REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_ncpo_flag_clr, (reg32), (val))
#define PON_INTERRUPT_CLR_0_SET_rg_ncpo_clr(reg32, val)        REG_FLD_SET(PON_INTERRUPT_CLR_0_FLD_rg_ncpo_clr, (reg32), (val))

#define PON_INTERRUPT_SET_0_SET_rg_ncpo_det_en(reg32, val)     REG_FLD_SET(PON_INTERRUPT_SET_0_FLD_rg_ncpo_det_en, (reg32), (val))
#define PON_INTERRUPT_SET_0_SET_rg_rogue_en(reg32, val)        REG_FLD_SET(PON_INTERRUPT_SET_0_FLD_rg_rogue_en, (reg32), (val))
#define PON_INTERRUPT_SET_0_SET_rg_int_los_on_pro(reg32, val)  REG_FLD_SET(PON_INTERRUPT_SET_0_FLD_rg_int_los_on_pro, (reg32), (val))
#define PON_INTERRUPT_SET_0_SET_rg_int_los_cnt(reg32, val)     REG_FLD_SET(PON_INTERRUPT_SET_0_FLD_rg_int_los_cnt, (reg32), (val))

#define PON_INTERRUPT_SET_1_SET_rg_tx_en_counter(reg32, val)   REG_FLD_SET(PON_INTERRUPT_SET_1_FLD_rg_tx_en_counter, (reg32), (val))

#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_int_los_inv(val)       REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_int_los_inv, (val))
#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_tdc_los_inv(val)       REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_los_inv, (val))
#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_tdc_ck_en_and_los(val) REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_ck_en_and_los, (val))
#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_tdc_lck2ref_sel(val)   REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_tdc_lck2ref_sel, (val))
#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_sw_lcpll_en(val)       REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_sw_lcpll_en, (val))
#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_lcpll_en_inv(val)      REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_en_inv, (val))
#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_lcpll_hw_ctrl_mode(val) REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_hw_ctrl_mode, (val))
#define SS_LCPLL_PWCTL_SETTING_0_VAL_rg_lcpll_force_on(val)    REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_0_FLD_rg_lcpll_force_on, (val))

#define SS_LCPLL_PWCTL_SETTING_1_VAL_rg_lcpll_ck_stb_timer(val) REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_ck_stb_timer, (val))
#define SS_LCPLL_PWCTL_SETTING_1_VAL_rg_lcpll_pcw_man_load_timer(val) REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_pcw_man_load_timer, (val))
#define SS_LCPLL_PWCTL_SETTING_1_VAL_rg_lcpll_en_timer(val)    REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_en_timer, (val))
#define SS_LCPLL_PWCTL_SETTING_1_VAL_rg_lcpll_man_pwdb(val)    REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_1_FLD_rg_lcpll_man_pwdb, (val))

#define SS_LCPLL_PWCTL_SETTING_2_VAL_rg_ncpo_ana_msb(val)      REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_ncpo_ana_msb, (val))
#define SS_LCPLL_PWCTL_SETTING_2_VAL_rg_tdc_mst_en(val)        REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_mst_en, (val))
#define SS_LCPLL_PWCTL_SETTING_2_VAL_rg_tdc_ck_stb_timer(val)  REG_FLD_VAL(SS_LCPLL_PWCTL_SETTING_2_FLD_rg_tdc_ck_stb_timer, (val))

#define SS_LCPLL_PWCTL_DBG_SETTING_VAL_rg_lcpll_ledck_div(val) REG_FLD_VAL(SS_LCPLL_PWCTL_DBG_SETTING_FLD_rg_lcpll_ledck_div, (val))

#define SS_LCPLL_TDC_PW_0_VAL_rg_lcpll_dpma_dds1_iso_disable(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_iso_disable, (val))
#define SS_LCPLL_TDC_PW_0_VAL_rg_lcpll_dpma_dds1_pwdb(val)     REG_FLD_VAL(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_dds1_pwdb, (val))
#define SS_LCPLL_TDC_PW_0_VAL_rg_lcpll_dpma_autopw_en(val)     REG_FLD_VAL(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_dpma_autopw_en, (val))
#define SS_LCPLL_TDC_PW_0_VAL_rg_lcpll_tdc_dig_pwdb(val)       REG_FLD_VAL(SS_LCPLL_TDC_PW_0_FLD_rg_lcpll_tdc_dig_pwdb, (val))

#define SS_LCPLL_TDC_PW_1_VAL_rg_lcpll_dpma_dds1_pcw_ncpo_chg(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg, (val))
#define SS_LCPLL_TDC_PW_1_VAL_rg_lcpll_dpma_dds1_rstb(val)     REG_FLD_VAL(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb, (val))
#define SS_LCPLL_TDC_PW_1_VAL_rg_lcpll_dpma_dds1_rstb_dly(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_rstb_dly, (val))
#define SS_LCPLL_TDC_PW_1_VAL_rg_lcpll_dpma_dds1_iso_disable_dly(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_1_FLD_rg_lcpll_dpma_dds1_iso_disable_dly, (val))

#define SS_LCPLL_TDC_PW_2_VAL_rg_lcpll_dpma_txpll1_ddsfbk_en(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en, (val))
#define SS_LCPLL_TDC_PW_2_VAL_rg_lcpll_dpma_dds1_ncpo_en_dly(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en_dly, (val))
#define SS_LCPLL_TDC_PW_2_VAL_rg_lcpll_dpma_dds1_ncpo_en(val)  REG_FLD_VAL(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_ncpo_en, (val))
#define SS_LCPLL_TDC_PW_2_VAL_rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_2_FLD_rg_lcpll_dpma_dds1_pcw_ncpo_chg_dly, (val))

#define SS_LCPLL_TDC_PW_3_VAL_rg_lcpll_tdc_cal(val)            REG_FLD_VAL(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal, (val))
#define SS_LCPLL_TDC_PW_3_VAL_rg_lcpll_tdc_cal_trig_dly(val)   REG_FLD_VAL(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig_dly, (val))
#define SS_LCPLL_TDC_PW_3_VAL_rg_lcpll_tdc_cal_trig(val)       REG_FLD_VAL(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_tdc_cal_trig, (val))
#define SS_LCPLL_TDC_PW_3_VAL_rg_lcpll_dpma_txpll1_ddsfbk_en_dly(val) REG_FLD_VAL(SS_LCPLL_TDC_PW_3_FLD_rg_lcpll_dpma_txpll1_ddsfbk_en_dly, (val))

#define SS_LCPLL_TDC_PW_4_VAL_rg_lcpll_tdc_sigdet_cg(val)      REG_FLD_VAL(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_sigdet_cg, (val))
#define SS_LCPLL_TDC_PW_4_VAL_rg_lcpll_tdc_en(val)             REG_FLD_VAL(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_en, (val))
#define SS_LCPLL_TDC_PW_4_VAL_rg_lcpll_tdc_cal_offset(val)     REG_FLD_VAL(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_offset, (val))
#define SS_LCPLL_TDC_PW_4_VAL_rg_lcpll_tdc_cal_bw(val)         REG_FLD_VAL(SS_LCPLL_TDC_PW_4_FLD_rg_lcpll_tdc_cal_bw, (val))

#define SS_LCPLL_TDC_PW_5_VAL_rg_tdc_start_cnt(val)            REG_FLD_VAL(SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt, (val))
#define SS_LCPLL_TDC_PW_5_VAL_rg_tdc_start_cnt_clr(val)        REG_FLD_VAL(SS_LCPLL_TDC_PW_5_FLD_rg_tdc_start_cnt_clr, (val))
#define SS_LCPLL_TDC_PW_5_VAL_rg_ncpo_start_dly_en(val)        REG_FLD_VAL(SS_LCPLL_TDC_PW_5_FLD_rg_ncpo_start_dly_en, (val))
#define SS_LCPLL_TDC_PW_5_VAL_rg_lcpll_tdc_sync_in_mode(val)   REG_FLD_VAL(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_sync_in_mode, (val))
#define SS_LCPLL_TDC_PW_5_VAL_rg_lcpll_autok_tdc(val)          REG_FLD_VAL(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_autok_tdc, (val))
#define SS_LCPLL_TDC_PW_5_VAL_rg_lcpll_tdc_autoen(val)         REG_FLD_VAL(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_autoen, (val))
#define SS_LCPLL_TDC_PW_5_VAL_rg_lcpll_tdc_en_mux(val)         REG_FLD_VAL(SS_LCPLL_TDC_PW_5_FLD_rg_lcpll_tdc_en_mux, (val))

#define SS_LCPLL_TDC_FLT_0_VAL_rg_lcpll_tdc_offset(val)        REG_FLD_VAL(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_tdc_offset, (val))
#define SS_LCPLL_TDC_FLT_0_VAL_rg_lcpll_ki(val)                REG_FLD_VAL(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_ki, (val))
#define SS_LCPLL_TDC_FLT_0_VAL_rg_lcpll_pon_rx_cdr_divtdc(val) REG_FLD_VAL(SS_LCPLL_TDC_FLT_0_FLD_rg_lcpll_pon_rx_cdr_divtdc, (val))

#define SS_LCPLL_TDC_FLT_1_VAL_rg_lcpll_tdc_gain(val)          REG_FLD_VAL(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_tdc_gain, (val))
#define SS_LCPLL_TDC_FLT_1_VAL_rg_lcpll_a_tdc(val)             REG_FLD_VAL(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_a_tdc, (val))
#define SS_LCPLL_TDC_FLT_1_VAL_rg_lcpll_gpon_sel(val)          REG_FLD_VAL(SS_LCPLL_TDC_FLT_1_FLD_rg_lcpll_gpon_sel, (val))

#define SS_LCPLL_TDC_FLT_2_VAL_rg_lcpll_ncpo_value(val)        REG_FLD_VAL(SS_LCPLL_TDC_FLT_2_FLD_rg_lcpll_ncpo_value, (val))

#define SS_LCPLL_TDC_FLT_3_VAL_rg_sdm_upd_disb(val)            REG_FLD_VAL(SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd_disb, (val))
#define SS_LCPLL_TDC_FLT_3_VAL_rg_sdm_upd(val)                 REG_FLD_VAL(SS_LCPLL_TDC_FLT_3_FLD_rg_sdm_upd, (val))
#define SS_LCPLL_TDC_FLT_3_VAL_rg_lcpll_tdc_swrst(val)         REG_FLD_VAL(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_tdc_swrst, (val))
#define SS_LCPLL_TDC_FLT_3_VAL_rg_lcpll_ncpo_hold(val)         REG_FLD_VAL(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_hold, (val))
#define SS_LCPLL_TDC_FLT_3_VAL_rg_lcpll_ncpo_load(val)         REG_FLD_VAL(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_load, (val))
#define SS_LCPLL_TDC_FLT_3_VAL_rg_lcpll_ncpo_shift(val)        REG_FLD_VAL(SS_LCPLL_TDC_FLT_3_FLD_rg_lcpll_ncpo_shift, (val))

#define SS_LCPLL_TDC_FLT_4_VAL_rg_lcpll_tdcen_wait_lck2ref(val) REG_FLD_VAL(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_wait_lck2ref, (val))
#define SS_LCPLL_TDC_FLT_4_VAL_rg_lcpll_tdcen_chk_lck2ref(val) REG_FLD_VAL(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_chk_lck2ref, (val))
#define SS_LCPLL_TDC_FLT_4_VAL_rg_lcpll_tdc_rst(val)           REG_FLD_VAL(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdc_rst, (val))
#define SS_LCPLL_TDC_FLT_4_VAL_rg_lcpll_tdcen_rst_dlf(val)     REG_FLD_VAL(SS_LCPLL_TDC_FLT_4_FLD_rg_lcpll_tdcen_rst_dlf, (val))

#define SS_LCPLL_TDC_FLT_5_VAL_rg_lcpll_ncpo_chg(val)          REG_FLD_VAL(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_ncpo_chg, (val))
#define SS_LCPLL_TDC_FLT_5_VAL_rg_lcpll_tdc_autopw_ncpo(val)   REG_FLD_VAL(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_autopw_ncpo, (val))
#define SS_LCPLL_TDC_FLT_5_VAL_rg_lcpll_tdc_waitlck_pos_dly(val) REG_FLD_VAL(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pos_dly, (val))
#define SS_LCPLL_TDC_FLT_5_VAL_rg_lcpll_tdc_waitlck_pre_dly(val) REG_FLD_VAL(SS_LCPLL_TDC_FLT_5_FLD_rg_lcpll_tdc_waitlck_pre_dly, (val))

#define SS_LCPLL_TDC_FLT_6_VAL_rg_lcpll_tdc_pcw_sub1en(val)    REG_FLD_VAL(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_tdc_pcw_sub1en, (val))
#define SS_LCPLL_TDC_FLT_6_VAL_rg_lcpll_dlf_mult_en(val)       REG_FLD_VAL(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_dlf_mult_en, (val))
#define SS_LCPLL_TDC_FLT_6_VAL_rg_lcpll_ncpo_chg_delay(val)    REG_FLD_VAL(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_delay, (val))
#define SS_LCPLL_TDC_FLT_6_VAL_rg_lcpll_ncpo_chg_mask(val)     REG_FLD_VAL(SS_LCPLL_TDC_FLT_6_FLD_rg_lcpll_ncpo_chg_mask, (val))

#define SS_LCPLL_TDC_FLT_7_VAL_rg_lcpll_tdcck_inv_en(val)      REG_FLD_VAL(SS_LCPLL_TDC_FLT_7_FLD_rg_lcpll_tdcck_inv_en, (val))

#define SS_LCPLL_TDC_PCW_1_VAL_rg_lcpll_pon_hrdds_pcw_ncpo_gpon(val) REG_FLD_VAL(SS_LCPLL_TDC_PCW_1_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_gpon, (val))

#define SS_LCPLL_TDC_PCW_2_VAL_rg_lcpll_pon_hrdds_pcw_ncpo_epon(val) REG_FLD_VAL(SS_LCPLL_TDC_PCW_2_FLD_rg_lcpll_pon_hrdds_pcw_ncpo_epon, (val))

#define SS_LCPLL_TDC_RO_1_VAL_ro_lcpll_tdc_dig(val)            REG_FLD_VAL(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_dig, (val))
#define SS_LCPLL_TDC_RO_1_VAL_ro_lcpll_tdc_st(val)             REG_FLD_VAL(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_st, (val))
#define SS_LCPLL_TDC_RO_1_VAL_ro_lcpll_tdc_tdcpw_st(val)       REG_FLD_VAL(SS_LCPLL_TDC_RO_1_FLD_ro_lcpll_tdc_tdcpw_st, (val))

#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_tdc_ck_stb(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_ck_stb, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_txpll1_ddsfbk_en(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_txpll1_ddsfbk_en, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_tdc_cal_offset(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_offset, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_tdc_cal_bw(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_cal_bw, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_tdc_en(val)    REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_tdc_en, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_rx_ck_stb(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_rx_ck_stb, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_en_lck2ref_rst(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_en_lck2ref_rst, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_dds1_pwdb(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pwdb, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_dds1_iso_disable(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_iso_disable, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_dds1_rstb(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_rstb, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_dds1_ncpo_en(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_ncpo_en, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo_chg, (val))
#define SS_LCPLL_TDC_RO_2_VAL_ro_lcpll_tdc_dlf_gain(val)       REG_FLD_VAL(SS_LCPLL_TDC_RO_2_FLD_ro_lcpll_tdc_dlf_gain, (val))

#define SS_LCPLL_TDC_RO_3_VAL_ro_lcpll_tdc_gain(val)           REG_FLD_VAL(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_gain, (val))
#define SS_LCPLL_TDC_RO_3_VAL_ro_lcpll_tdc_decode(val)         REG_FLD_VAL(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_decode, (val))
#define SS_LCPLL_TDC_RO_3_VAL_ro_lcpll_tdc_en(val)             REG_FLD_VAL(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_en, (val))
#define SS_LCPLL_TDC_RO_3_VAL_ro_lcpll_tdc_cal_done(val)       REG_FLD_VAL(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_cal_done, (val))
#define SS_LCPLL_TDC_RO_3_VAL_ro_lcpll_tdc_offset(val)         REG_FLD_VAL(SS_LCPLL_TDC_RO_3_FLD_ro_lcpll_tdc_offset, (val))

#define SS_LCPLL_TDC_RO_4_VAL_ro_lcpll_tdc_dpma_dds1_pcw_ncpo(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_4_FLD_ro_lcpll_tdc_dpma_dds1_pcw_ncpo, (val))

#define SS_LCPLL_TDC_RO_5_VAL_ro_lcpll_tdc_dpma_tdc_c(val)     REG_FLD_VAL(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_c, (val))
#define SS_LCPLL_TDC_RO_5_VAL_ro_lcpll_tdc_dpma_tdc_d(val)     REG_FLD_VAL(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_d, (val))
#define SS_LCPLL_TDC_RO_5_VAL_ro_lcpll_tdc_dpma_tdc_leadlag(val) REG_FLD_VAL(SS_LCPLL_TDC_RO_5_FLD_ro_lcpll_tdc_dpma_tdc_leadlag, (val))

#define SS_LCPLL_TDC_CTRL_0_VAL_rg_lcpll_ki_hold(val)          REG_FLD_VAL(SS_LCPLL_TDC_CTRL_0_FLD_rg_lcpll_ki_hold, (val))

#define SS_LCPLL_0_VAL_tdc_sync_pcw_ncpo_chg(val)              REG_FLD_VAL(SS_LCPLL_0_FLD_tdc_sync_pcw_ncpo_chg, (val))
#define SS_LCPLL_0_VAL_eee_lcpll_hold(val)                     REG_FLD_VAL(SS_LCPLL_0_FLD_eee_lcpll_hold, (val))

#define SS_LCPLL_1_VAL_tdc_sync_pcw_ncpo(val)                  REG_FLD_VAL(SS_LCPLL_1_FLD_tdc_sync_pcw_ncpo, (val))

#define SS_LCPLL_2_VAL_lcpll_top_flagh_en(val)                 REG_FLD_VAL(SS_LCPLL_2_FLD_lcpll_top_flagh_en, (val))
#define SS_LCPLL_2_VAL_lcpll_top_flagl_en(val)                 REG_FLD_VAL(SS_LCPLL_2_FLD_lcpll_top_flagl_en, (val))
#define SS_LCPLL_2_VAL_flagh_sel(val)                          REG_FLD_VAL(SS_LCPLL_2_FLD_flagh_sel, (val))
#define SS_LCPLL_2_VAL_flagl_sel(val)                          REG_FLD_VAL(SS_LCPLL_2_FLD_flagl_sel, (val))

#define TDC_DLF_GAIN_STATUS_VAL_ro_dlf_gain_rund_new(val)      REG_FLD_VAL(TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_rund_new, (val))
#define TDC_DLF_GAIN_STATUS_VAL_ro_dlf_gain_new(val)           REG_FLD_VAL(TDC_DLF_GAIN_STATUS_FLD_ro_dlf_gain_new, (val))

#define TDC_DLF_MODE_SETTING_VAL_rg_dlf_gain_latch(val)        REG_FLD_VAL(TDC_DLF_MODE_SETTING_FLD_rg_dlf_gain_latch, (val))
#define TDC_DLF_MODE_SETTING_VAL_rg_dlf_mode_sel(val)          REG_FLD_VAL(TDC_DLF_MODE_SETTING_FLD_rg_dlf_mode_sel, (val))
#define TDC_DLF_MODE_SETTING_VAL_ro_dlf_gain_new_org(val)      REG_FLD_VAL(TDC_DLF_MODE_SETTING_FLD_ro_dlf_gain_new_org, (val))

#define TDC_HOLD_CTRL_0_VAL_rg_ncpo_hold_en(val)               REG_FLD_VAL(TDC_HOLD_CTRL_0_FLD_rg_ncpo_hold_en, (val))
#define TDC_HOLD_CTRL_0_VAL_rg_ncpo_toggle(val)                REG_FLD_VAL(TDC_HOLD_CTRL_0_FLD_rg_ncpo_toggle, (val))
#define TDC_HOLD_CTRL_0_VAL_rg_tdc_los_hold_mux(val)           REG_FLD_VAL(TDC_HOLD_CTRL_0_FLD_rg_tdc_los_hold_mux, (val))
#define TDC_HOLD_CTRL_0_VAL_rg_tdc_ncpo_hold_release_time(val) REG_FLD_VAL(TDC_HOLD_CTRL_0_FLD_rg_tdc_ncpo_hold_release_time, (val))

#define TDC_HOLD_CTRL_1_VAL_rg_ncpo_up_thred(val)              REG_FLD_VAL(TDC_HOLD_CTRL_1_FLD_rg_ncpo_up_thred, (val))

#define TDC_HOLD_CTRL_2_VAL_rg_ncpo_dwn_thred(val)             REG_FLD_VAL(TDC_HOLD_CTRL_2_FLD_rg_ncpo_dwn_thred, (val))

#define TDC_HOLD_STATUS_0_VAL_ro_ncpo_output(val)              REG_FLD_VAL(TDC_HOLD_STATUS_0_FLD_ro_ncpo_output, (val))

#define TDC_HOLD_STATUS_1_VAL_ro_ncpo_max_output(val)          REG_FLD_VAL(TDC_HOLD_STATUS_1_FLD_ro_ncpo_max_output, (val))

#define TDC_HOLD_STATUS_2_VAL_ro_ncpo_min_output(val)          REG_FLD_VAL(TDC_HOLD_STATUS_2_FLD_ro_ncpo_min_output, (val))

#define PON_SERDES_CTRL_0_VAL_rg_tx_data_path_sel(val)         REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_tx_data_path_sel, (val))
#define PON_SERDES_CTRL_0_VAL_rg_ben_inv(val)                  REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_ben_inv, (val))
#define PON_SERDES_CTRL_0_VAL_rg_force_tphy_speed_tx(val)      REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx, (val))
#define PON_SERDES_CTRL_0_VAL_rg_force_tphy_speed_tx_sel(val)  REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_tx_sel, (val))
#define PON_SERDES_CTRL_0_VAL_rg_pcie_device_mode(val)         REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_pcie_device_mode, (val))
#define PON_SERDES_CTRL_0_VAL_rg_force_tphy_speed_rx_sel(val)  REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_force_tphy_speed_rx_sel, (val))
#define PON_SERDES_CTRL_0_VAL_rg_force_tphy_mode(val)          REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_force_tphy_mode, (val))
#define PON_SERDES_CTRL_0_VAL_rg_tphy_mode_set(val)            REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_tphy_mode_set, (val))
#define PON_SERDES_CTRL_0_VAL_rg_sgmii_auto_init(val)          REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_sgmii_auto_init, (val))
#define PON_SERDES_CTRL_0_VAL_rg_tphy_speed_mode_force(val)    REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode_force, (val))
#define PON_SERDES_CTRL_0_VAL_rg_tphy_speed_mode(val)          REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_tphy_speed_mode, (val))
#define PON_SERDES_CTRL_0_VAL_rg_speed_chg(val)                REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_speed_chg, (val))
#define PON_SERDES_CTRL_0_VAL_rg_phya_auto_init(val)           REG_FLD_VAL(PON_SERDES_CTRL_0_FLD_rg_phya_auto_init, (val))

#define PON_SERDES_CTRL_1_VAL_rg_rx_pwd(val)                   REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_rx_pwd, (val))
#define PON_SERDES_CTRL_1_VAL_rg_rx_pwd_mux(val)               REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_rx_pwd_mux, (val))
#define PON_SERDES_CTRL_1_VAL_rg_tx_disable(val)               REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_tx_disable, (val))
#define PON_SERDES_CTRL_1_VAL_rg_tx_disable_mux(val)           REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_tx_disable_mux, (val))
#define PON_SERDES_CTRL_1_VAL_rg_phya_pwd(val)                 REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_phya_pwd, (val))
#define PON_SERDES_CTRL_1_VAL_rg_phya_pwd_mux(val)             REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_phya_pwd_mux, (val))
#define PON_SERDES_CTRL_1_VAL_rg_freq_det_frc_en(val)          REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_freq_det_frc_en, (val))
#define PON_SERDES_CTRL_1_VAL_rg_freq_det_frc(val)             REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_freq_det_frc, (val))
#define PON_SERDES_CTRL_1_VAL_rg_cdr_re_calib(val)             REG_FLD_VAL(PON_SERDES_CTRL_1_FLD_rg_cdr_re_calib, (val))

#define PON_SERDES_CTRL_2_VAL_rg_ben_on_opt(val)               REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_ben_on_opt, (val))
#define PON_SERDES_CTRL_2_VAL_rg_epon_benoff_opt(val)          REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_epon_benoff_opt, (val))
#define PON_SERDES_CTRL_2_VAL_rg_bypass_int_dly(val)           REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_bypass_int_dly, (val))
#define PON_SERDES_CTRL_2_VAL_rg_tx_div_mode(val)              REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_tx_div_mode, (val))
#define PON_SERDES_CTRL_2_VAL_rg_tx_idle_en(val)               REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_tx_idle_en, (val))
#define PON_SERDES_CTRL_2_VAL_rg_tx_idle_mux(val)              REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_tx_idle_mux, (val))
#define PON_SERDES_CTRL_2_VAL_rg_tx_idle_mode(val)             REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_tx_idle_mode, (val))
#define PON_SERDES_CTRL_2_VAL_rg_tx_idle(val)                  REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_tx_idle, (val))
#define PON_SERDES_CTRL_2_VAL_rg_sigdet_wait_force(val)        REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_force, (val))
#define PON_SERDES_CTRL_2_VAL_rg_sigdet_wait_cond(val)         REG_FLD_VAL(PON_SERDES_CTRL_2_FLD_rg_sigdet_wait_cond, (val))

#define PON_SERDES_CTRL_3_VAL_rg_cdr_settle_dly(val)           REG_FLD_VAL(PON_SERDES_CTRL_3_FLD_rg_cdr_settle_dly, (val))
#define PON_SERDES_CTRL_3_VAL_rg_cdr_bw_sel_dly(val)           REG_FLD_VAL(PON_SERDES_CTRL_3_FLD_rg_cdr_bw_sel_dly, (val))
#define PON_SERDES_CTRL_3_VAL_rg_bias_pwd_to_bg_lpf_en_dly(val) REG_FLD_VAL(PON_SERDES_CTRL_3_FLD_rg_bias_pwd_to_bg_lpf_en_dly, (val))

#define PON_SERDES_CTRL_4_VAL_rg_freq_meter_sel(val)           REG_FLD_VAL(PON_SERDES_CTRL_4_FLD_rg_freq_meter_sel, (val))
#define PON_SERDES_CTRL_4_VAL_rg_ppm_thres(val)                REG_FLD_VAL(PON_SERDES_CTRL_4_FLD_rg_ppm_thres, (val))
#define PON_SERDES_CTRL_4_VAL_rg_lpf_window_sel(val)           REG_FLD_VAL(PON_SERDES_CTRL_4_FLD_rg_lpf_window_sel, (val))
#define PON_SERDES_CTRL_4_VAL_rg_freq_det_en_wait_dly(val)     REG_FLD_VAL(PON_SERDES_CTRL_4_FLD_rg_freq_det_en_wait_dly, (val))

#define PON_SERDES_CTRL_5_VAL_rg_sig_det_wait_dly(val)         REG_FLD_VAL(PON_SERDES_CTRL_5_FLD_rg_sig_det_wait_dly, (val))
#define PON_SERDES_CTRL_5_VAL_rg_rx_eq_en_h_dly(val)           REG_FLD_VAL(PON_SERDES_CTRL_5_FLD_rg_rx_eq_en_h_dly, (val))

#define PON_SERDES_CTRL_6_VAL_rg_rx_saosc_en_h_dly(val)        REG_FLD_VAL(PON_SERDES_CTRL_6_FLD_rg_rx_saosc_en_h_dly, (val))
#define PON_SERDES_CTRL_6_VAL_rg_rx_pi_cal_en_h_dly(val)       REG_FLD_VAL(PON_SERDES_CTRL_6_FLD_rg_rx_pi_cal_en_h_dly, (val))
#define PON_SERDES_CTRL_6_VAL_rg_pll_autok_vco_to_pll_bias_lpf_en_dly(val) REG_FLD_VAL(PON_SERDES_CTRL_6_FLD_rg_pll_autok_vco_to_pll_bias_lpf_en_dly, (val))

#define PON_SERDES_CTRL_7_VAL_rg_syspll_pwd_to_syspll_ready_dly(val) REG_FLD_VAL(PON_SERDES_CTRL_7_FLD_rg_syspll_pwd_to_syspll_ready_dly, (val))
#define PON_SERDES_CTRL_7_VAL_rg_sig_det_l_thres(val)          REG_FLD_VAL(PON_SERDES_CTRL_7_FLD_rg_sig_det_l_thres, (val))
#define PON_SERDES_CTRL_7_VAL_rg_sig_det_h_thres(val)          REG_FLD_VAL(PON_SERDES_CTRL_7_FLD_rg_sig_det_h_thres, (val))

#define PON_SERDES_CTRL_8_VAL_rg_sigdet_out_sel(val)           REG_FLD_VAL(PON_SERDES_CTRL_8_FLD_rg_sigdet_out_sel, (val))
#define PON_SERDES_CTRL_8_VAL_rg_eidle_lp_en_ne_to_data_en_pe_dly_g2(val) REG_FLD_VAL(PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g2, (val))
#define PON_SERDES_CTRL_8_VAL_rg_eidle_lp_en_ne_to_data_en_pe_dly_g1(val) REG_FLD_VAL(PON_SERDES_CTRL_8_FLD_rg_eidle_lp_en_ne_to_data_en_pe_dly_g1, (val))

#define PON_SERDES_CTRL_9_VAL_rg_ssusb_cdr_band_fail(val)      REG_FLD_VAL(PON_SERDES_CTRL_9_FLD_rg_ssusb_cdr_band_fail, (val))
#define PON_SERDES_CTRL_9_VAL_rg_ssusb_pll_band_fail(val)      REG_FLD_VAL(PON_SERDES_CTRL_9_FLD_rg_ssusb_pll_band_fail, (val))

#define PON_SERDES_CTRL_10_VAL_ro_bist_run(val)                REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_ro_bist_run, (val))
#define PON_SERDES_CTRL_10_VAL_ro_bist_ok(val)                 REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_ro_bist_ok, (val))
#define PON_SERDES_CTRL_10_VAL_ro_bist_err(val)                REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_ro_bist_err, (val))
#define PON_SERDES_CTRL_10_VAL_rg_prbs_mode(val)               REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_prbs_mode, (val))
#define PON_SERDES_CTRL_10_VAL_rg_renew_mode(val)              REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_renew_mode, (val))
#define PON_SERDES_CTRL_10_VAL_rg_prbs_injerr(val)             REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_prbs_injerr, (val))
#define PON_SERDES_CTRL_10_VAL_rg_prbs_en(val)                 REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_prbs_en, (val))
#define PON_SERDES_CTRL_10_VAL_rg_prbs_check(val)              REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_prbs_check, (val))
#define PON_SERDES_CTRL_10_VAL_rg_tx_bit_polarity(val)         REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_tx_bit_polarity, (val))
#define PON_SERDES_CTRL_10_VAL_rg_tbi_tx_disable(val)          REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_tbi_tx_disable, (val))
#define PON_SERDES_CTRL_10_VAL_rg_tbi_rx_disable(val)          REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_tbi_rx_disable, (val))
#define PON_SERDES_CTRL_10_VAL_rg_rx_bit_polarity(val)         REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_rx_bit_polarity, (val))
#define PON_SERDES_CTRL_10_VAL_rg_qphy_r2t_loopback_fifo_rst(val) REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback_fifo_rst, (val))
#define PON_SERDES_CTRL_10_VAL_rg_qphy_r2t_loopback(val)       REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_qphy_r2t_loopback, (val))
#define PON_SERDES_CTRL_10_VAL_rg_pcs_t2r_loopback(val)        REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_pcs_t2r_loopback, (val))
#define PON_SERDES_CTRL_10_VAL_rg_afifo_winc_serdes(val)       REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_afifo_winc_serdes, (val))
#define PON_SERDES_CTRL_10_VAL_rg_afifo_rinc_serdes(val)       REG_FLD_VAL(PON_SERDES_CTRL_10_FLD_rg_afifo_rinc_serdes, (val))

#define PON_SERDES_STS_0_VAL_ro_debug_prbs(val)                REG_FLD_VAL(PON_SERDES_STS_0_FLD_ro_debug_prbs, (val))
#define PON_SERDES_STS_0_VAL_ro_bist_err_cnt(val)              REG_FLD_VAL(PON_SERDES_STS_0_FLD_ro_bist_err_cnt, (val))

#define PON_SERDES_CTRL_11_VAL_rg_udp_data_31_0(val)           REG_FLD_VAL(PON_SERDES_CTRL_11_FLD_rg_udp_data_31_0, (val))

#define PON_SERDES_CTRL_12_VAL_rg_udp_data_63_32(val)          REG_FLD_VAL(PON_SERDES_CTRL_12_FLD_rg_udp_data_63_32, (val))

#define PON_SERDES_CTRL_13_VAL_rg_udp_data_79_64(val)          REG_FLD_VAL(PON_SERDES_CTRL_13_FLD_rg_udp_data_79_64, (val))

#define PON_SERDES_CTRL_14_VAL_rg_afifo_w_thres_serdes(val)    REG_FLD_VAL(PON_SERDES_CTRL_14_FLD_rg_afifo_w_thres_serdes, (val))
#define PON_SERDES_CTRL_14_VAL_rg_afifo_r_thres_serdes(val)    REG_FLD_VAL(PON_SERDES_CTRL_14_FLD_rg_afifo_r_thres_serdes, (val))

#define PON_SERDES_STS_1_VAL_ro_debug_afifo_serdes(val)        REG_FLD_VAL(PON_SERDES_STS_1_FLD_ro_debug_afifo_serdes, (val))

#define PON_SERDES_CTRL_15_VAL_rg_ad_ben_out_clr(val)          REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_ad_ben_out_clr, (val))
#define PON_SERDES_CTRL_15_VAL_rg_ben_force_value(val)         REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_ben_force_value, (val))
#define PON_SERDES_CTRL_15_VAL_rg_ben_force_mode(val)          REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_ben_force_mode, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_path_rdy_mux(val)         REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_path_rdy_mux, (val))
#define PON_SERDES_CTRL_15_VAL_rg_data_rx_inv(val)             REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_data_rx_inv, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_cid_path_en(val)          REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_cid_path_en, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_prbs15_path_en(val)       REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_prbs15_path_en, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_prbs9_path_en(val)        REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_prbs9_path_en, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_prbs31_path_en(val)       REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_prbs31_path_en, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_prbs23_path_en(val)       REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_prbs23_path_en, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_prbs7_path_en(val)        REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_prbs7_path_en, (val))
#define PON_SERDES_CTRL_15_VAL_rg_rx_prbs_path_sel(val)        REG_FLD_VAL(PON_SERDES_CTRL_15_FLD_rg_rx_prbs_path_sel, (val))

#define PON_SERDES_CTRL_16_VAL_rg_tx_end_dly_sel(val)          REG_FLD_VAL(PON_SERDES_CTRL_16_FLD_rg_tx_end_dly_sel, (val))
#define PON_SERDES_CTRL_16_VAL_rg_tx_start_dly_sel(val)        REG_FLD_VAL(PON_SERDES_CTRL_16_FLD_rg_tx_start_dly_sel, (val))
#define PON_SERDES_CTRL_16_VAL_rg_tx_bit_dly_mode(val)         REG_FLD_VAL(PON_SERDES_CTRL_16_FLD_rg_tx_bit_dly_mode, (val))
#define PON_SERDES_CTRL_16_VAL_rg_txbit_dly(val)               REG_FLD_VAL(PON_SERDES_CTRL_16_FLD_rg_txbit_dly, (val))
#define PON_SERDES_CTRL_16_VAL_rg_bit_delay_rg_mode(val)       REG_FLD_VAL(PON_SERDES_CTRL_16_FLD_rg_bit_delay_rg_mode, (val))
#define PON_SERDES_CTRL_16_VAL_rg_tx_ben_end_dly_mode(val)     REG_FLD_VAL(PON_SERDES_CTRL_16_FLD_rg_tx_ben_end_dly_mode, (val))
#define PON_SERDES_CTRL_16_VAL_rg_tx_ben_start_dly_mode(val)   REG_FLD_VAL(PON_SERDES_CTRL_16_FLD_rg_tx_ben_start_dly_mode, (val))

#define PON_SERDES_CTRL_17_VAL_rg_tx_bit_end_dly(val)          REG_FLD_VAL(PON_SERDES_CTRL_17_FLD_rg_tx_bit_end_dly, (val))
#define PON_SERDES_CTRL_17_VAL_rg_tx_bit_start_dly(val)        REG_FLD_VAL(PON_SERDES_CTRL_17_FLD_rg_tx_bit_start_dly, (val))

#define PON_SERDES_CTRL_18_VAL_rg_force_rxcal_done_sel(val)    REG_FLD_VAL(PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done_sel, (val))
#define PON_SERDES_CTRL_18_VAL_rg_force_rxcal_done(val)        REG_FLD_VAL(PON_SERDES_CTRL_18_FLD_rg_force_rxcal_done, (val))
#define PON_SERDES_CTRL_18_VAL_rg_rxcal_ne_cnt(val)            REG_FLD_VAL(PON_SERDES_CTRL_18_FLD_rg_rxcal_ne_cnt, (val))
#define PON_SERDES_CTRL_18_VAL_rg_force_l2ref_done_sel(val)    REG_FLD_VAL(PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done_sel, (val))
#define PON_SERDES_CTRL_18_VAL_rg_force_l2ref_done(val)        REG_FLD_VAL(PON_SERDES_CTRL_18_FLD_rg_force_l2ref_done, (val))
#define PON_SERDES_CTRL_18_VAL_rg_l2ref_ne_cnt(val)            REG_FLD_VAL(PON_SERDES_CTRL_18_FLD_rg_l2ref_ne_cnt, (val))

#define PON_SERDES_STS_2_VAL_ro_rxcal_done(val)                REG_FLD_VAL(PON_SERDES_STS_2_FLD_ro_rxcal_done, (val))
#define PON_SERDES_STS_2_VAL_ro_l2ref_done(val)                REG_FLD_VAL(PON_SERDES_STS_2_FLD_ro_l2ref_done, (val))

#define PON_SERDES_CTRL_19_VAL_rg_20b_idle_code(val)           REG_FLD_VAL(PON_SERDES_CTRL_19_FLD_rg_20b_idle_code, (val))

#define PON_SERDES_CTRL_20_VAL_rg_lpf_en_to_ssc_dly_cnt(val)   REG_FLD_VAL(PON_SERDES_CTRL_20_FLD_rg_lpf_en_to_ssc_dly_cnt, (val))
#define PON_SERDES_CTRL_20_VAL_rg_da_pll_en_dly_cnt(val)       REG_FLD_VAL(PON_SERDES_CTRL_20_FLD_rg_da_pll_en_dly_cnt, (val))
#define PON_SERDES_CTRL_20_VAL_rg_da_pll_en(val)               REG_FLD_VAL(PON_SERDES_CTRL_20_FLD_rg_da_pll_en, (val))
#define PON_SERDES_CTRL_20_VAL_rg_force_da_pll_en(val)         REG_FLD_VAL(PON_SERDES_CTRL_20_FLD_rg_force_da_pll_en, (val))
#define PON_SERDES_CTRL_20_VAL_rg_da_pll_en_mux(val)           REG_FLD_VAL(PON_SERDES_CTRL_20_FLD_rg_da_pll_en_mux, (val))

#define PON_SERDES_CTRL_21_VAL_rg_pll_vco_cplt(val)            REG_FLD_VAL(PON_SERDES_CTRL_21_FLD_rg_pll_vco_cplt, (val))
#define PON_SERDES_CTRL_21_VAL_rg_force_pll_vco_cplt(val)      REG_FLD_VAL(PON_SERDES_CTRL_21_FLD_rg_force_pll_vco_cplt, (val))
#define PON_SERDES_CTRL_21_VAL_rg_da_cdr_pwd(val)              REG_FLD_VAL(PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd, (val))
#define PON_SERDES_CTRL_21_VAL_rg_force_da_cdr_pwd(val)        REG_FLD_VAL(PON_SERDES_CTRL_21_FLD_rg_force_da_cdr_pwd, (val))
#define PON_SERDES_CTRL_21_VAL_rg_da_cdr_pwd_dly_cnt(val)      REG_FLD_VAL(PON_SERDES_CTRL_21_FLD_rg_da_cdr_pwd_dly_cnt, (val))

#define PON_SERDES_CTRL_22_VAL_rg_vcocal_fail(val)             REG_FLD_VAL(PON_SERDES_CTRL_22_FLD_rg_vcocal_fail, (val))
#define PON_SERDES_CTRL_22_VAL_rg_vcocal_dly_cnt(val)          REG_FLD_VAL(PON_SERDES_CTRL_22_FLD_rg_vcocal_dly_cnt, (val))
#define PON_SERDES_CTRL_22_VAL_rg_vcocal_state(val)            REG_FLD_VAL(PON_SERDES_CTRL_22_FLD_rg_vcocal_state, (val))

#define PON_SYS_CTRL_0_VAL_rg_xfi_rx_pma_clk_sel(val)          REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_xfi_rx_pma_clk_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_baser_tx_pma_clk_div_sel(val)    REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_baser_tx_pma_clk_div_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_tx_pma_clk_sel(val)              REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_tx_pma_clk_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_hg_tx_pma_clk_div_sel(val)       REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_hg_tx_pma_clk_div_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_pon_tx_pma_clk_div_sel(val)      REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_pon_tx_pma_clk_div_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_rx_pma_clk_sel(val)              REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_rx_pma_clk_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_hg_rx_pma_clk_div_sel(val)       REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_hg_rx_pma_clk_div_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_pon_rx_pma_clk_div_sel(val)      REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_pon_rx_pma_clk_div_sel, (val))
#define PON_SYS_CTRL_0_VAL_rg_baser_rx_clk_inv(val)            REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_baser_rx_clk_inv, (val))
#define PON_SYS_CTRL_0_VAL_rg_baser_tx_clk_inv(val)            REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_baser_tx_clk_inv, (val))
#define PON_SYS_CTRL_0_VAL_rg_tx_clk_inv(val)                  REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_tx_clk_inv, (val))
#define PON_SYS_CTRL_0_VAL_rg_rx_clk_inv(val)                  REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_rx_clk_inv, (val))
#define PON_SYS_CTRL_0_VAL_rg_txben_clk_inv(val)               REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_txben_clk_inv, (val))
#define PON_SYS_CTRL_0_VAL_rg_rx_short_ck_inv(val)             REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_rx_short_ck_inv, (val))
#define PON_SYS_CTRL_0_VAL_rg_tx_short_ck_inv(val)             REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_tx_short_ck_inv, (val))
#define PON_SYS_CTRL_0_VAL_rg_baser_rxck_en(val)               REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_baser_rxck_en, (val))
#define PON_SYS_CTRL_0_VAL_rg_baser_txck_en(val)               REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_baser_txck_en, (val))
#define PON_SYS_CTRL_0_VAL_rg_hg_rx500m_ck_en(val)             REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_hg_rx500m_ck_en, (val))
#define PON_SYS_CTRL_0_VAL_rg_hg_tx250m_ck_en(val)             REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_hg_tx250m_ck_en, (val))
#define PON_SYS_CTRL_0_VAL_rg_pon_rx500m_ck_en(val)            REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_pon_rx500m_ck_en, (val))
#define PON_SYS_CTRL_0_VAL_rg_pon_tx250m_ck_en(val)            REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_pon_tx250m_ck_en, (val))
#define PON_SYS_CTRL_0_VAL_rg_force_ge_mode(val)               REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_force_ge_mode, (val))
#define PON_SYS_CTRL_0_VAL_rg_force_map_mode(val)              REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_force_map_mode, (val))
#define PON_SYS_CTRL_0_VAL_rg_force_map_sel(val)               REG_FLD_VAL(PON_SYS_CTRL_0_FLD_rg_force_map_sel, (val))

#define PON_SYS_CTRL_1_VAL_rg_div_rst(val)                     REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_div_rst, (val))
#define PON_SYS_CTRL_1_VAL_rg_rate_adp_rx_pma_fifo_rst_n(val)  REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_pma_fifo_rst_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_rate_adp_rx_fifo_rst_n(val)      REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_rate_adp_rx_fifo_rst_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_rate_adp_tx_fifo_rst_n(val)      REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_rate_adp_tx_fifo_rst_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_all_rx_rst_n(val)                REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_all_rx_rst_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_all_tx_rst_n(val)                REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_all_tx_rst_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_baser_rx_reset_n(val)            REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_baser_rx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_baser_tx_reset_n(val)            REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_baser_tx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_rx_fifo_rst(val)                 REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_rx_fifo_rst, (val))
#define PON_SYS_CTRL_1_VAL_rg_force_bg_tx_value(val)           REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_force_bg_tx_value, (val))
#define PON_SYS_CTRL_1_VAL_rg_force_bg_tx_mode(val)            REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_force_bg_tx_mode, (val))
#define PON_SYS_CTRL_1_VAL_rg_trans_rx_data_inv(val)           REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_trans_rx_data_inv, (val))
#define PON_SYS_CTRL_1_VAL_rg_bridge_rx_reset_n(val)           REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_bridge_rx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_bridge_tx_reset_n(val)           REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_bridge_tx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_eq_rx250m_div_reset_n(val)       REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_eq_rx250m_div_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_eq_rx250m_reset_n(val)           REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_eq_rx250m_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_tx250m_short_reset_n(val)        REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_tx250m_short_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_tx250m_div_reset_n(val)          REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_tx250m_div_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_tx_reset_n(val)                  REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_tx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_rx_reset_n(val)                  REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_rx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_ref_reset_n(val)                 REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_ref_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_pcstx_reset_n(val)               REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_pcstx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_pcsrx_reset_n(val)               REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_pcsrx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_pcstxrx_reset_n(val)             REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_pcstxrx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_mactx_reset_n(val)               REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_mactx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_macrx_reset_n(val)               REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_macrx_reset_n, (val))
#define PON_SYS_CTRL_1_VAL_rg_mactxrx_reset_n(val)             REG_FLD_VAL(PON_SYS_CTRL_1_FLD_rg_mactxrx_reset_n, (val))

#define PON_SYS_CTRL_2_VAL_rg_1us_40m_cnt(val)                 REG_FLD_VAL(PON_SYS_CTRL_2_FLD_rg_1us_40m_cnt, (val))
#define PON_SYS_CTRL_2_VAL_rg_1us_25m_cnt(val)                 REG_FLD_VAL(PON_SYS_CTRL_2_FLD_rg_1us_25m_cnt, (val))
#define PON_SYS_CTRL_2_VAL_rg_pon_mbist_slow_mode_en(val)      REG_FLD_VAL(PON_SYS_CTRL_2_FLD_rg_pon_mbist_slow_mode_en, (val))
#define PON_SYS_CTRL_2_VAL_rg_pon_mbist_mode_en(val)           REG_FLD_VAL(PON_SYS_CTRL_2_FLD_rg_pon_mbist_mode_en, (val))
#define PON_SYS_CTRL_2_VAL_rg_xtal_mode(val)                   REG_FLD_VAL(PON_SYS_CTRL_2_FLD_rg_xtal_mode, (val))
#define PON_SYS_CTRL_2_VAL_rg_xtal_is_25m(val)                 REG_FLD_VAL(PON_SYS_CTRL_2_FLD_rg_xtal_is_25m, (val))
#define PON_SYS_CTRL_2_VAL_rg_pon_probe_sel(val)               REG_FLD_VAL(PON_SYS_CTRL_2_FLD_rg_pon_probe_sel, (val))

#define PON_CK_CTRL_0_VAL_rg_rs_ck_cnt(val)                    REG_FLD_VAL(PON_CK_CTRL_0_FLD_rg_rs_ck_cnt, (val))
#define PON_CK_CTRL_0_VAL_rg_rs_ck_sel(val)                    REG_FLD_VAL(PON_CK_CTRL_0_FLD_rg_rs_ck_sel, (val))
#define PON_CK_CTRL_0_VAL_rg_rs_ck_mux(val)                    REG_FLD_VAL(PON_CK_CTRL_0_FLD_rg_rs_ck_mux, (val))
#define PON_CK_CTRL_0_VAL_rg_baser_rxck_mode(val)              REG_FLD_VAL(PON_CK_CTRL_0_FLD_rg_baser_rxck_mode, (val))
#define PON_CK_CTRL_0_VAL_rg_baser_txck_mode(val)              REG_FLD_VAL(PON_CK_CTRL_0_FLD_rg_baser_txck_mode, (val))

#define HG_RST_CTRL_0_VAL_rg_hsgmii_mac_rxck_free_sel(val)     REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hsgmii_mac_rxck_free_sel, (val))
#define HG_RST_CTRL_0_VAL_rg_hsgmii_mac_txck_free_sel(val)     REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hsgmii_mac_txck_free_sel, (val))
#define HG_RST_CTRL_0_VAL_rg_xfi_pcs_bg_lb_fifo_rst_n(val)     REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_xfi_pcs_bg_lb_fifo_rst_n, (val))
#define HG_RST_CTRL_0_VAL_rg_xfi_pcs_tx_gearbox_fifo_rst_n(val) REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_xfi_pcs_tx_gearbox_fifo_rst_n, (val))
#define HG_RST_CTRL_0_VAL_rg_xfi_pcs_rx_gearbox_fifo_rst_n(val) REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_xfi_pcs_rx_gearbox_fifo_rst_n, (val))
#define HG_RST_CTRL_0_VAL_rg_bypass_tx_re_en(val)              REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_bypass_tx_re_en, (val))
#define HG_RST_CTRL_0_VAL_rg_hg_mac_rx_reset_n(val)            REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hg_mac_rx_reset_n, (val))
#define HG_RST_CTRL_0_VAL_rg_hg_mac_tx_reset_n(val)            REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hg_mac_tx_reset_n, (val))
#define HG_RST_CTRL_0_VAL_rg_hg_pcs_rx_reset_n(val)            REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hg_pcs_rx_reset_n, (val))
#define HG_RST_CTRL_0_VAL_rg_hg_pcs_tx_reset_n(val)            REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hg_pcs_tx_reset_n, (val))
#define HG_RST_CTRL_0_VAL_rg_hg_pma_rx_reset_n(val)            REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hg_pma_rx_reset_n, (val))
#define HG_RST_CTRL_0_VAL_rg_hg_pma_tx_reset_n(val)            REG_FLD_VAL(HG_RST_CTRL_0_FLD_rg_hg_pma_tx_reset_n, (val))

#define HG_MODE_CTRL_0_VAL_rg_fpga_tx_swap(val)                REG_FLD_VAL(HG_MODE_CTRL_0_FLD_rg_fpga_tx_swap, (val))
#define HG_MODE_CTRL_0_VAL_rg_fpga_mode_ctrl(val)              REG_FLD_VAL(HG_MODE_CTRL_0_FLD_rg_fpga_mode_ctrl, (val))
#define HG_MODE_CTRL_0_VAL_rg_ip_reset_mode(val)               REG_FLD_VAL(HG_MODE_CTRL_0_FLD_rg_ip_reset_mode, (val))
#define HG_MODE_CTRL_0_VAL_rg_sgmii_mode(val)                  REG_FLD_VAL(HG_MODE_CTRL_0_FLD_rg_sgmii_mode, (val))
#define HG_MODE_CTRL_0_VAL_rg_gpon_sym_mode(val)               REG_FLD_VAL(HG_MODE_CTRL_0_FLD_rg_gpon_sym_mode, (val))
#define HG_MODE_CTRL_0_VAL_rg_serdes_mode(val)                 REG_FLD_VAL(HG_MODE_CTRL_0_FLD_rg_serdes_mode, (val))

#define HG_RG_CTRL_0_VAL_rg_eq_step1(val)                      REG_FLD_VAL(HG_RG_CTRL_0_FLD_rg_eq_step1, (val))
#define HG_RG_CTRL_0_VAL_rg_tx_dem(val)                        REG_FLD_VAL(HG_RG_CTRL_0_FLD_rg_tx_dem, (val))
#define HG_RG_CTRL_0_VAL_rg_ln0_rx_dac_pwd(val)                REG_FLD_VAL(HG_RG_CTRL_0_FLD_rg_ln0_rx_dac_pwd, (val))
#define HG_RG_CTRL_0_VAL_rg_rx_dac_pwd_mux(val)                REG_FLD_VAL(HG_RG_CTRL_0_FLD_rg_rx_dac_pwd_mux, (val))

#define PON_STS_0_VAL_ro_baser_mode(val)                       REG_FLD_VAL(PON_STS_0_FLD_ro_baser_mode, (val))
#define PON_STS_0_VAL_ro_hsgmii_mode(val)                      REG_FLD_VAL(PON_STS_0_FLD_ro_hsgmii_mode, (val))
#define PON_STS_0_VAL_ro_sgmii_mode(val)                       REG_FLD_VAL(PON_STS_0_FLD_ro_sgmii_mode, (val))
#define PON_STS_0_VAL_ro_epon_mode(val)                        REG_FLD_VAL(PON_STS_0_FLD_ro_epon_mode, (val))
#define PON_STS_0_VAL_ro_gpon_tmode(val)                       REG_FLD_VAL(PON_STS_0_FLD_ro_gpon_tmode, (val))
#define PON_STS_0_VAL_ro_gpon_mode(val)                        REG_FLD_VAL(PON_STS_0_FLD_ro_gpon_mode, (val))

#define PON_OSR_SEL_CTRL_VAL_rg_tx_eqd_mode(val)               REG_FLD_VAL(PON_OSR_SEL_CTRL_FLD_rg_tx_eqd_mode, (val))
#define PON_OSR_SEL_CTRL_VAL_rg_rx_osr_sel(val)                REG_FLD_VAL(PON_OSR_SEL_CTRL_FLD_rg_rx_osr_sel, (val))
#define PON_OSR_SEL_CTRL_VAL_rg_tx_osr_sel(val)                REG_FLD_VAL(PON_OSR_SEL_CTRL_FLD_rg_tx_osr_sel, (val))

#define CDR_CTRL_0_VAL_rg_bic_ltr_mode_1(val)                  REG_FLD_VAL(CDR_CTRL_0_FLD_rg_bic_ltr_mode_1, (val))
#define CDR_CTRL_0_VAL_rg_bic_ltr_mode_0(val)                  REG_FLD_VAL(CDR_CTRL_0_FLD_rg_bic_ltr_mode_0, (val))
#define CDR_CTRL_0_VAL_rg_bir_ltr_mode_1(val)                  REG_FLD_VAL(CDR_CTRL_0_FLD_rg_bir_ltr_mode_1, (val))
#define CDR_CTRL_0_VAL_rg_bir_ltr_mode_0(val)                  REG_FLD_VAL(CDR_CTRL_0_FLD_rg_bir_ltr_mode_0, (val))

#define CDR_CTRL_1_VAL_rg_kvsel_ltr_mode_1(val)                REG_FLD_VAL(CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_1, (val))
#define CDR_CTRL_1_VAL_rg_kvsel_ltr_mode_0(val)                REG_FLD_VAL(CDR_CTRL_1_FLD_rg_kvsel_ltr_mode_0, (val))
#define CDR_CTRL_1_VAL_rg_br_ltr_mode_1(val)                   REG_FLD_VAL(CDR_CTRL_1_FLD_rg_br_ltr_mode_1, (val))
#define CDR_CTRL_1_VAL_rg_br_ltr_mode_0(val)                   REG_FLD_VAL(CDR_CTRL_1_FLD_rg_br_ltr_mode_0, (val))
#define CDR_CTRL_1_VAL_rg_bc_ltr_mode_1(val)                   REG_FLD_VAL(CDR_CTRL_1_FLD_rg_bc_ltr_mode_1, (val))
#define CDR_CTRL_1_VAL_rg_bc_ltr_mode_0(val)                   REG_FLD_VAL(CDR_CTRL_1_FLD_rg_bc_ltr_mode_0, (val))

#define CDR_CTRL_2_VAL_rg_bic_ltd0_mode_1(val)                 REG_FLD_VAL(CDR_CTRL_2_FLD_rg_bic_ltd0_mode_1, (val))
#define CDR_CTRL_2_VAL_rg_bic_ltd0_mode_0(val)                 REG_FLD_VAL(CDR_CTRL_2_FLD_rg_bic_ltd0_mode_0, (val))
#define CDR_CTRL_2_VAL_rg_bir_ltd0_mode_1(val)                 REG_FLD_VAL(CDR_CTRL_2_FLD_rg_bir_ltd0_mode_1, (val))
#define CDR_CTRL_2_VAL_rg_bir_ltd0_mode_0(val)                 REG_FLD_VAL(CDR_CTRL_2_FLD_rg_bir_ltd0_mode_0, (val))

#define CDR_CTRL_3_VAL_rg_kvsel_ltd0_mode_1(val)               REG_FLD_VAL(CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_1, (val))
#define CDR_CTRL_3_VAL_rg_kvsel_ltd0_mode_0(val)               REG_FLD_VAL(CDR_CTRL_3_FLD_rg_kvsel_ltd0_mode_0, (val))
#define CDR_CTRL_3_VAL_rg_br_ltd0_mode_1(val)                  REG_FLD_VAL(CDR_CTRL_3_FLD_rg_br_ltd0_mode_1, (val))
#define CDR_CTRL_3_VAL_rg_br_ltd0_mode_0(val)                  REG_FLD_VAL(CDR_CTRL_3_FLD_rg_br_ltd0_mode_0, (val))
#define CDR_CTRL_3_VAL_rg_bc_ltd0_mode_1(val)                  REG_FLD_VAL(CDR_CTRL_3_FLD_rg_bc_ltd0_mode_1, (val))
#define CDR_CTRL_3_VAL_rg_bc_ltd0_mode_0(val)                  REG_FLD_VAL(CDR_CTRL_3_FLD_rg_bc_ltd0_mode_0, (val))

#define CDR_CTRL_4_VAL_rg_bic_ltd1_mode_1(val)                 REG_FLD_VAL(CDR_CTRL_4_FLD_rg_bic_ltd1_mode_1, (val))
#define CDR_CTRL_4_VAL_rg_bic_ltd1_mode_0(val)                 REG_FLD_VAL(CDR_CTRL_4_FLD_rg_bic_ltd1_mode_0, (val))
#define CDR_CTRL_4_VAL_rg_bir_ltd1_mode_1(val)                 REG_FLD_VAL(CDR_CTRL_4_FLD_rg_bir_ltd1_mode_1, (val))
#define CDR_CTRL_4_VAL_rg_bir_ltd1_mode_0(val)                 REG_FLD_VAL(CDR_CTRL_4_FLD_rg_bir_ltd1_mode_0, (val))

#define CDR_CTRL_5_VAL_rg_kvsel_ltd1_mode_1(val)               REG_FLD_VAL(CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_1, (val))
#define CDR_CTRL_5_VAL_rg_kvsel_ltd1_mode_0(val)               REG_FLD_VAL(CDR_CTRL_5_FLD_rg_kvsel_ltd1_mode_0, (val))
#define CDR_CTRL_5_VAL_rg_br_ltd1_mode_1(val)                  REG_FLD_VAL(CDR_CTRL_5_FLD_rg_br_ltd1_mode_1, (val))
#define CDR_CTRL_5_VAL_rg_br_ltd1_mode_0(val)                  REG_FLD_VAL(CDR_CTRL_5_FLD_rg_br_ltd1_mode_0, (val))
#define CDR_CTRL_5_VAL_rg_bc_ltd1_mode_1(val)                  REG_FLD_VAL(CDR_CTRL_5_FLD_rg_bc_ltd1_mode_1, (val))
#define CDR_CTRL_5_VAL_rg_bc_ltd1_mode_0(val)                  REG_FLD_VAL(CDR_CTRL_5_FLD_rg_bc_ltd1_mode_0, (val))

#define TX_CTRL_0_VAL_rg_tx_ben_end_dly_sel(val)               REG_FLD_VAL(TX_CTRL_0_FLD_rg_tx_ben_end_dly_sel, (val))
#define TX_CTRL_0_VAL_rg_tx_ben_start_dly_sel(val)             REG_FLD_VAL(TX_CTRL_0_FLD_rg_tx_ben_start_dly_sel, (val))
#define TX_CTRL_0_VAL_rg_tx_ben_exten_ftune(val)               REG_FLD_VAL(TX_CTRL_0_FLD_rg_tx_ben_exten_ftune, (val))
#define TX_CTRL_0_VAL_rg_tx_dly_ben_ftune(val)                 REG_FLD_VAL(TX_CTRL_0_FLD_rg_tx_dly_ben_ftune, (val))
#define TX_CTRL_0_VAL_rg_tx_dly_data_ftune(val)                REG_FLD_VAL(TX_CTRL_0_FLD_rg_tx_dly_data_ftune, (val))

#define TX_CTRL_1_VAL_rg_rx_non_reverse_epon(val)              REG_FLD_VAL(TX_CTRL_1_FLD_rg_rx_non_reverse_epon, (val))
#define TX_CTRL_1_VAL_rg_tx_non_reverse_epon(val)              REG_FLD_VAL(TX_CTRL_1_FLD_rg_tx_non_reverse_epon, (val))
#define TX_CTRL_1_VAL_rg_rx_non_reverse_gpon(val)              REG_FLD_VAL(TX_CTRL_1_FLD_rg_rx_non_reverse_gpon, (val))
#define TX_CTRL_1_VAL_rg_tx_non_reverse_gpon(val)              REG_FLD_VAL(TX_CTRL_1_FLD_rg_tx_non_reverse_gpon, (val))
#define TX_CTRL_1_VAL_rg_force_rx_pon_sel(val)                 REG_FLD_VAL(TX_CTRL_1_FLD_rg_force_rx_pon_sel, (val))
#define TX_CTRL_1_VAL_rg_force_rx_pon_mode(val)                REG_FLD_VAL(TX_CTRL_1_FLD_rg_force_rx_pon_mode, (val))
#define TX_CTRL_1_VAL_rg_force_tx_gpon_sel(val)                REG_FLD_VAL(TX_CTRL_1_FLD_rg_force_tx_gpon_sel, (val))
#define TX_CTRL_1_VAL_rg_force_tx_gpon_mode(val)               REG_FLD_VAL(TX_CTRL_1_FLD_rg_force_tx_gpon_mode, (val))
#define TX_CTRL_1_VAL_rg_force_tx_epon_sel(val)                REG_FLD_VAL(TX_CTRL_1_FLD_rg_force_tx_epon_sel, (val))
#define TX_CTRL_1_VAL_rg_force_tx_epon_mode(val)               REG_FLD_VAL(TX_CTRL_1_FLD_rg_force_tx_epon_mode, (val))

#define TX_CTRL_2_VAL_rg_txben_bdly_mode(val)                  REG_FLD_VAL(TX_CTRL_2_FLD_rg_txben_bdly_mode, (val))
#define TX_CTRL_2_VAL_rg_txdata_bdly_mode(val)                 REG_FLD_VAL(TX_CTRL_2_FLD_rg_txdata_bdly_mode, (val))
#define TX_CTRL_2_VAL_rg_bit_delay_mode_sel(val)               REG_FLD_VAL(TX_CTRL_2_FLD_rg_bit_delay_mode_sel, (val))
#define TX_CTRL_2_VAL_rg_benoff_cnt_max(val)                   REG_FLD_VAL(TX_CTRL_2_FLD_rg_benoff_cnt_max, (val))
#define TX_CTRL_2_VAL_rg_txben_pre_post_zero_en(val)           REG_FLD_VAL(TX_CTRL_2_FLD_rg_txben_pre_post_zero_en, (val))
#define TX_CTRL_2_VAL_rg_hsdata_en_sel(val)                    REG_FLD_VAL(TX_CTRL_2_FLD_rg_hsdata_en_sel, (val))
#define TX_CTRL_2_VAL_rg_outben_data_mode(val)                 REG_FLD_VAL(TX_CTRL_2_FLD_rg_outben_data_mode, (val))

#define TX_CTRL_3_VAL_rg_bist_prbs_update(val)                 REG_FLD_VAL(TX_CTRL_3_FLD_rg_bist_prbs_update, (val))
#define TX_CTRL_3_VAL_rg_bist_prbs_init_seed_benoff(val)       REG_FLD_VAL(TX_CTRL_3_FLD_rg_bist_prbs_init_seed_benoff, (val))

#define TX_CTRL_4_VAL_rg_benoff_d0(val)                        REG_FLD_VAL(TX_CTRL_4_FLD_rg_benoff_d0, (val))

#define TX_CTRL_5_VAL_rg_benoff_d1(val)                        REG_FLD_VAL(TX_CTRL_5_FLD_rg_benoff_d1, (val))

#define TX_CTRL_6_VAL_rg_benoff_d2(val)                        REG_FLD_VAL(TX_CTRL_6_FLD_rg_benoff_d2, (val))

#define TX_CTRL_7_VAL_rg_benoff_d3(val)                        REG_FLD_VAL(TX_CTRL_7_FLD_rg_benoff_d3, (val))

#define FRQ_CTRL_0_VAL_rg_hg_rx_frq_ppm_thres(val)             REG_FLD_VAL(FRQ_CTRL_0_FLD_rg_hg_rx_frq_ppm_thres, (val))
#define FRQ_CTRL_0_VAL_rg_hg_tx_frq_ppm_thres(val)             REG_FLD_VAL(FRQ_CTRL_0_FLD_rg_hg_tx_frq_ppm_thres, (val))
#define FRQ_CTRL_0_VAL_rg_pon_rx_frq_ppm_thres(val)            REG_FLD_VAL(FRQ_CTRL_0_FLD_rg_pon_rx_frq_ppm_thres, (val))
#define FRQ_CTRL_0_VAL_rg_pon_tx_frq_ppm_thres(val)            REG_FLD_VAL(FRQ_CTRL_0_FLD_rg_pon_tx_frq_ppm_thres, (val))

#define FRQ_CTRL_1_VAL_rg_baser_rx_frq_ppm_thres(val)          REG_FLD_VAL(FRQ_CTRL_1_FLD_rg_baser_rx_frq_ppm_thres, (val))
#define FRQ_CTRL_1_VAL_rg_baser_tx_frq_ppm_thres(val)          REG_FLD_VAL(FRQ_CTRL_1_FLD_rg_baser_tx_frq_ppm_thres, (val))

#define FRQ_CTRL_2_VAL_rg_baser_rx_freq_det_mux(val)           REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_mux, (val))
#define FRQ_CTRL_2_VAL_rg_baser_tx_freq_det_mux(val)           REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_mux, (val))
#define FRQ_CTRL_2_VAL_rg_hg_rx_freq_det_mux(val)              REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_mux, (val))
#define FRQ_CTRL_2_VAL_rg_hg_tx_freq_det_mux(val)              REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_mux, (val))
#define FRQ_CTRL_2_VAL_rg_pon_rx_freq_det_mux(val)             REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_mux, (val))
#define FRQ_CTRL_2_VAL_rg_pon_tx_freq_det_mux(val)             REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_mux, (val))
#define FRQ_CTRL_2_VAL_rg_baser_rx_freq_det_en(val)            REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_baser_rx_freq_det_en, (val))
#define FRQ_CTRL_2_VAL_rg_baser_tx_freq_det_en(val)            REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_baser_tx_freq_det_en, (val))
#define FRQ_CTRL_2_VAL_rg_hg_rx_freq_det_en(val)               REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_hg_rx_freq_det_en, (val))
#define FRQ_CTRL_2_VAL_rg_hg_tx_freq_det_en(val)               REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_hg_tx_freq_det_en, (val))
#define FRQ_CTRL_2_VAL_rg_pon_rx_freq_det_en(val)              REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_pon_rx_freq_det_en, (val))
#define FRQ_CTRL_2_VAL_rg_pon_tx_freq_det_en(val)              REG_FLD_VAL(FRQ_CTRL_2_FLD_rg_pon_tx_freq_det_en, (val))

#define FRQ_CTRL_3_VAL_rg_pon_rx_freq_cnt(val)                 REG_FLD_VAL(FRQ_CTRL_3_FLD_rg_pon_rx_freq_cnt, (val))
#define FRQ_CTRL_3_VAL_rg_pon_tx_freq_cnt(val)                 REG_FLD_VAL(FRQ_CTRL_3_FLD_rg_pon_tx_freq_cnt, (val))

#define FRQ_CTRL_4_VAL_rg_hg_rx_freq_cnt(val)                  REG_FLD_VAL(FRQ_CTRL_4_FLD_rg_hg_rx_freq_cnt, (val))
#define FRQ_CTRL_4_VAL_rg_hg_tx_freq_cnt(val)                  REG_FLD_VAL(FRQ_CTRL_4_FLD_rg_hg_tx_freq_cnt, (val))

#define FRQ_CTRL_5_VAL_rg_baser_rx_freq_cnt(val)               REG_FLD_VAL(FRQ_CTRL_5_FLD_rg_baser_rx_freq_cnt, (val))
#define FRQ_CTRL_5_VAL_rg_baser_tx_freq_cnt(val)               REG_FLD_VAL(FRQ_CTRL_5_FLD_rg_baser_tx_freq_cnt, (val))

#define RX_FREQDET_0_VAL_rg_rx_pll_unlock_cyclecnt(val)        REG_FLD_VAL(RX_FREQDET_0_FLD_rg_rx_pll_unlock_cyclecnt, (val))
#define RX_FREQDET_0_VAL_rg_rx_pll_lock_cyclecnt(val)          REG_FLD_VAL(RX_FREQDET_0_FLD_rg_rx_pll_lock_cyclecnt, (val))

#define RX_FREQDET_1_VAL_rg_rx_pll_lock_target_end(val)        REG_FLD_VAL(RX_FREQDET_1_FLD_rg_rx_pll_lock_target_end, (val))
#define RX_FREQDET_1_VAL_rg_rx_pll_lock_target_beg(val)        REG_FLD_VAL(RX_FREQDET_1_FLD_rg_rx_pll_lock_target_beg, (val))

#define RX_FREQDET_2_VAL_rg_rx_pll_unlock_target_end(val)      REG_FLD_VAL(RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_end, (val))
#define RX_FREQDET_2_VAL_rg_rx_pll_unlock_target_beg(val)      REG_FLD_VAL(RX_FREQDET_2_FLD_rg_rx_pll_unlock_target_beg, (val))

#define RX_FREQDET_3_VAL_rg_force_rx_freq_det_en(val)          REG_FLD_VAL(RX_FREQDET_3_FLD_rg_force_rx_freq_det_en, (val))
#define RX_FREQDET_3_VAL_rg_rx_pll_wait_100us(val)             REG_FLD_VAL(RX_FREQDET_3_FLD_rg_rx_pll_wait_100us, (val))
#define RX_FREQDET_3_VAL_rg_rx_pll_unlockth(val)               REG_FLD_VAL(RX_FREQDET_3_FLD_rg_rx_pll_unlockth, (val))
#define RX_FREQDET_3_VAL_rg_rx_pll_lock_lockth(val)            REG_FLD_VAL(RX_FREQDET_3_FLD_rg_rx_pll_lock_lockth, (val))
#define RX_FREQDET_3_VAL_rg_rx_pll_lock_cnt_clear(val)         REG_FLD_VAL(RX_FREQDET_3_FLD_rg_rx_pll_lock_cnt_clear, (val))
#define RX_FREQDET_3_VAL_rg_rx_pll_freqlock_det_en(val)        REG_FLD_VAL(RX_FREQDET_3_FLD_rg_rx_pll_freqlock_det_en, (val))

#define RX_FREQDET_STS_0_VAL_ro_rx_prb_out_freqdet(val)        REG_FLD_VAL(RX_FREQDET_STS_0_FLD_ro_rx_prb_out_freqdet, (val))
#define RX_FREQDET_STS_0_VAL_ro_rx_state_freqdet(val)          REG_FLD_VAL(RX_FREQDET_STS_0_FLD_ro_rx_state_freqdet, (val))
#define RX_FREQDET_STS_0_VAL_ro_rx_fbck_lock_inner(val)        REG_FLD_VAL(RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock_inner, (val))
#define RX_FREQDET_STS_0_VAL_ro_rx_fbck_lock(val)              REG_FLD_VAL(RX_FREQDET_STS_0_FLD_ro_rx_fbck_lock, (val))
#define RX_FREQDET_STS_0_VAL_ro_rx_lslock_cnt(val)             REG_FLD_VAL(RX_FREQDET_STS_0_FLD_ro_rx_lslock_cnt, (val))
#define RX_FREQDET_STS_0_VAL_ro_rx_fl_out(val)                 REG_FLD_VAL(RX_FREQDET_STS_0_FLD_ro_rx_fl_out, (val))

#define TX_CTRL_8_VAL_rg_tx_test_mode(val)                     REG_FLD_VAL(TX_CTRL_8_FLD_rg_tx_test_mode, (val))
#define TX_CTRL_8_VAL_rg_tx_test_pattern(val)                  REG_FLD_VAL(TX_CTRL_8_FLD_rg_tx_test_pattern, (val))

#define TX_CTRL_9_VAL_rg_ncpo_los_cnt(val)                     REG_FLD_VAL(TX_CTRL_9_FLD_rg_ncpo_los_cnt, (val))
#define TX_CTRL_9_VAL_rg_ncpo_los_hold_en(val)                 REG_FLD_VAL(TX_CTRL_9_FLD_rg_ncpo_los_hold_en, (val))
#define TX_CTRL_9_VAL_rg_los_function_sel(val)                 REG_FLD_VAL(TX_CTRL_9_FLD_rg_los_function_sel, (val))
#define TX_CTRL_9_VAL_rg_ncpo_los_pro_en(val)                  REG_FLD_VAL(TX_CTRL_9_FLD_rg_ncpo_los_pro_en, (val))
#define TX_CTRL_9_VAL_rg_ncpo_los_inv(val)                     REG_FLD_VAL(TX_CTRL_9_FLD_rg_ncpo_los_inv, (val))
#define TX_CTRL_9_VAL_rg_ncpo_lof_en(val)                      REG_FLD_VAL(TX_CTRL_9_FLD_rg_ncpo_lof_en, (val))

#define RX_FREQDET_DET_CTRL_0_VAL_rg_rx_cdr_freq_chk_cycle(val) REG_FLD_VAL(RX_FREQDET_DET_CTRL_0_FLD_rg_rx_cdr_freq_chk_cycle, (val))

#define RX_FREQDET_DET_CTRL_1_VAL_rg_rx_cdr_freq_golden_cycle(val) REG_FLD_VAL(RX_FREQDET_DET_CTRL_1_FLD_rg_rx_cdr_freq_golden_cycle, (val))

#define RX_FREQDET_DET_CTRL_2_VAL_rg_rx_cdr_freq_tolerate_cycle(val) REG_FLD_VAL(RX_FREQDET_DET_CTRL_2_FLD_rg_rx_cdr_freq_tolerate_cycle, (val))

#define RX_FREQDET_DET_CTRL_4_VAL_ro_rx_cdr_freq_unlck_cnt(val) REG_FLD_VAL(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_unlck_cnt, (val))
#define RX_FREQDET_DET_CTRL_4_VAL_ro_rx_cdr_freq_state(val)    REG_FLD_VAL(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_state, (val))
#define RX_FREQDET_DET_CTRL_4_VAL_ro_rx_cdr_freq_lock(val)     REG_FLD_VAL(RX_FREQDET_DET_CTRL_4_FLD_ro_rx_cdr_freq_lock, (val))
#define RX_FREQDET_DET_CTRL_4_VAL_rg_rx_cdr_freq_relck_num(val) REG_FLD_VAL(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_relck_num, (val))
#define RX_FREQDET_DET_CTRL_4_VAL_rg_rx_cdr_freq_clear_unlck_cnt(val) REG_FLD_VAL(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_clear_unlck_cnt, (val))
#define RX_FREQDET_DET_CTRL_4_VAL_rg_rx_cdr_freq_det_en(val)   REG_FLD_VAL(RX_FREQDET_DET_CTRL_4_FLD_rg_rx_cdr_freq_det_en, (val))

#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_pll_bias_lpf_en(val)     REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_lpf_en, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_pll_bias_lpf_en(val)     REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_lpf_en, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_bias_pwd(val)            REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_bias_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_bias_pwd(val)            REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_bias_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_biasimr_pwd(val)         REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_biasimr_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_biasimr_pwd(val)         REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_biasimr_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_bg_lpf_en(val)           REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_bg_lpf_en, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_bg_lpf_en(val)           REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_bg_lpf_en, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_pll_bias_pwd(val)        REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_bias_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_pll_bias_pwd(val)        REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_bias_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_xtal_rx_pwd(val)         REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_xtal_rx_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_xtal_rx_pwd(val)         REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_xtal_rx_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_syspll_pwd(val)          REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_syspll_pwd(val)          REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_syspll_sdm_pwr_on(val)   REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_pwr_on, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_syspll_sdm_pwr_on(val)   REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_pwr_on, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_syspll_sdm_iso_en(val)   REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_syspll_sdm_iso_en, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_syspll_sdm_iso_en(val)   REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_syspll_sdm_iso_en, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_pll_pwd(val)             REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_pll_pwd(val)             REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_pwd, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_pll_autok_vco(val)       REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_autok_vco, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_pll_autok_vco(val)       REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_autok_vco, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_c_da_pll_ssc_en(val)          REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_c_da_pll_ssc_en, (val))
#define PON_DA_MUX_CTRL_0_VAL_rg_f_da_pll_ssc_en(val)          REG_FLD_VAL(PON_DA_MUX_CTRL_0_FLD_rg_f_da_pll_ssc_en, (val))

#define PON_DA_MUX_CTRL_1_VAL_rg_c_da_sigdet_cal_offset(val)   REG_FLD_VAL(PON_DA_MUX_CTRL_1_FLD_rg_c_da_sigdet_cal_offset, (val))
#define PON_DA_MUX_CTRL_1_VAL_rg_f_da_sigdet_cal_offset(val)   REG_FLD_VAL(PON_DA_MUX_CTRL_1_FLD_rg_f_da_sigdet_cal_offset, (val))

#define PON_DA_MUX_CTRL_2_VAL_rg_c_da_cdr_lpf_lck2ref(val)     REG_FLD_VAL(PON_DA_MUX_CTRL_2_FLD_rg_c_da_cdr_lpf_lck2ref, (val))
#define PON_DA_MUX_CTRL_2_VAL_rg_f_da_cdr_lpf_lck2ref(val)     REG_FLD_VAL(PON_DA_MUX_CTRL_2_FLD_rg_f_da_cdr_lpf_lck2ref, (val))

#define PON_DA_MUX_CTRL_3_VAL_rg_pon_tx_gpon_sel(val)          REG_FLD_VAL(PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_sel, (val))
#define PON_DA_MUX_CTRL_3_VAL_rg_pon_tx_gpon_mux(val)          REG_FLD_VAL(PON_DA_MUX_CTRL_3_FLD_rg_pon_tx_gpon_mux, (val))

#define PON_DA_MUX_CTRL_4_VAL_rg_pma_rx_sigdet_pro(val)        REG_FLD_VAL(PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_pro, (val))
#define PON_DA_MUX_CTRL_4_VAL_rg_pma_rx_sigdet_en(val)         REG_FLD_VAL(PON_DA_MUX_CTRL_4_FLD_rg_pma_rx_sigdet_en, (val))
#define PON_DA_MUX_CTRL_4_VAL_rg_rx_sigdet_en(val)             REG_FLD_VAL(PON_DA_MUX_CTRL_4_FLD_rg_rx_sigdet_en, (val))

#define PON_DA_MUX_CTRL_5_VAL_rg_disb_da_pon_tx_data(val)      REG_FLD_VAL(PON_DA_MUX_CTRL_5_FLD_rg_disb_da_pon_tx_data, (val))
#define PON_DA_MUX_CTRL_5_VAL_rg_force_da_pon_tx_data(val)     REG_FLD_VAL(PON_DA_MUX_CTRL_5_FLD_rg_force_da_pon_tx_data, (val))

#define PON_MODE_FORCE_CTRL_0_VAL_rg_force_ge_rxpcsmodesel(val) REG_FLD_VAL(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxpcsmodesel, (val))
#define PON_MODE_FORCE_CTRL_0_VAL_rg_force_ge_txpcsmodesel(val) REG_FLD_VAL(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txpcsmodesel, (val))
#define PON_MODE_FORCE_CTRL_0_VAL_rg_force_ge_rxmapmodesel(val) REG_FLD_VAL(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_rxmapmodesel, (val))
#define PON_MODE_FORCE_CTRL_0_VAL_rg_force_ge_txmapmodesel(val) REG_FLD_VAL(PON_MODE_FORCE_CTRL_0_FLD_rg_force_ge_txmapmodesel, (val))

#define CLK_GEN_FORCE_CTRL_0_VAL_rg_or_baser_rxck(val)         REG_FLD_VAL(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_rxck, (val))
#define CLK_GEN_FORCE_CTRL_0_VAL_rg_or_baser_txck(val)         REG_FLD_VAL(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_baser_txck, (val))
#define CLK_GEN_FORCE_CTRL_0_VAL_rg_or_hg_rx500m_ck(val)       REG_FLD_VAL(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_rx500m_ck, (val))
#define CLK_GEN_FORCE_CTRL_0_VAL_rg_or_pon_rx500m_ck(val)      REG_FLD_VAL(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_rx500m_ck, (val))
#define CLK_GEN_FORCE_CTRL_0_VAL_rg_or_sh_tx250m_ck(val)       REG_FLD_VAL(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_sh_tx250m_ck, (val))
#define CLK_GEN_FORCE_CTRL_0_VAL_rg_or_hg_tx250m_ck(val)       REG_FLD_VAL(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_hg_tx250m_ck, (val))
#define CLK_GEN_FORCE_CTRL_0_VAL_rg_or_pon_tx250m_ck(val)      REG_FLD_VAL(CLK_GEN_FORCE_CTRL_0_FLD_rg_or_pon_tx250m_ck, (val))

#define ANALOG_FORCE_CTRL_0_VAL_rg_sigdet_force_value(val)     REG_FLD_VAL(ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_value, (val))
#define ANALOG_FORCE_CTRL_0_VAL_rg_sigdet_force_mode(val)      REG_FLD_VAL(ANALOG_FORCE_CTRL_0_FLD_rg_sigdet_force_mode, (val))

#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_RX_DFE_RST(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_RST, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_RX_DFE_EN(val)         REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_DFE_EN, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_RX_FR_MODE(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_RX_FR_MODE, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_FORCE_RX_FR_MODE(val)  REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_FORCE_RX_FR_MODE, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_TRI_DET_EN(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_TRI_DET_EN, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_STOPTIME(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_STOPTIME, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_RX500M_CK_SEL(val)  REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_RX500M_CK_SEL, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_PILPO_ROUT(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_PILPO_ROUT, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_GATED_RXD_B(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_GATED_RXD_B, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_EYE_MON_EN(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_MON_EN, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_EYE_CNT_EN(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_EYE_CNT_EN, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_DFE_TOG(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFE_TOG, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_DFEYEN_STOP_DIS(val) REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEYEN_STOP_DIS, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_DFEX_RST(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_RST, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_DFEX_EN(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_EN, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_DFEX_DIS(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEX_DIS, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_DFEXEN_SEL(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_DFEXEN_SEL, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_CHK_EYE_H(val)      REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_CHK_EYE_H, (val))
#define PON_RXFEDIG_CTRL_0_VAL_RG_SSUSB_EQ_ALGRTHM_SEL(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_0_FLD_RG_SSUSB_EQ_ALGRTHM_SEL, (val))

#define PON_RXFEDIG_CTRL_1_VAL_RG_SSUSB_EQ_DD1LOS_LFI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1LOS_LFI, (val))
#define PON_RXFEDIG_CTRL_1_VAL_RG_SSUSB_EQ_DD1HOS_LFI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD1HOS_LFI, (val))
#define PON_RXFEDIG_CTRL_1_VAL_RG_SSUSB_EQ_DD0LOS_LFI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0LOS_LFI, (val))
#define PON_RXFEDIG_CTRL_1_VAL_RG_SSUSB_EQ_DD0HOS_LFI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_1_FLD_RG_SSUSB_EQ_DD0HOS_LFI, (val))

#define PON_RXFEDIG_CTRL_2_VAL_RG_SSUSB_EQ_DEYE1OS_LFI(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE1OS_LFI, (val))
#define PON_RXFEDIG_CTRL_2_VAL_RG_SSUSB_EQ_DEYE0OS_LFI(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DEYE0OS_LFI, (val))
#define PON_RXFEDIG_CTRL_2_VAL_RG_SSUSB_EQ_DE1OS_LFI(val)      REG_FLD_VAL(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE1OS_LFI, (val))
#define PON_RXFEDIG_CTRL_2_VAL_RG_SSUSB_EQ_DE0OS_LFI(val)      REG_FLD_VAL(PON_RXFEDIG_CTRL_2_FLD_RG_SSUSB_EQ_DE0OS_LFI, (val))

#define PON_RXFEDIG_CTRL_3_VAL_RG_SSUSB_EQ_DHHL_LFI(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHL_LFI, (val))
#define PON_RXFEDIG_CTRL_3_VAL_RG_SSUSB_EQ_DHHLOS_LFI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DHHLOS_LFI, (val))
#define PON_RXFEDIG_CTRL_3_VAL_RG_SSUSB_EQ_DFEYEN_DUR(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEYEN_DUR, (val))
#define PON_RXFEDIG_CTRL_3_VAL_RG_SSUSB_EQ_DFEX_LF_SEL(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEX_LF_SEL, (val))
#define PON_RXFEDIG_CTRL_3_VAL_RG_SSUSB_EQ_DFEXEN_DUR(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_3_FLD_RG_SSUSB_EQ_DFEXEN_DUR, (val))

#define PON_RXFEDIG_CTRL_4_VAL_RG_SSUSB_EQ_DLEQ_LFI_GEN3(val)  REG_FLD_VAL(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN3, (val))
#define PON_RXFEDIG_CTRL_4_VAL_RG_SSUSB_EQ_DLEQ_LFI_GEN2(val)  REG_FLD_VAL(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN2, (val))
#define PON_RXFEDIG_CTRL_4_VAL_RG_SSUSB_EQ_DLEQ_LFI_GEN1(val)  REG_FLD_VAL(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN1, (val))
#define PON_RXFEDIG_CTRL_4_VAL_RG_SSUSB_EQ_DLEQOS_LFI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DLEQOS_LFI, (val))
#define PON_RXFEDIG_CTRL_4_VAL_RG_SSUSB_EQ_DHHL_LF_SEL(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_4_FLD_RG_SSUSB_EQ_DHHL_LF_SEL, (val))

#define PON_RXFEDIG_CTRL_5_VAL_RG_SSUSB_EQ_DSAOS_LF_SEL(val)   REG_FLD_VAL(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DSAOS_LF_SEL, (val))
#define PON_RXFEDIG_CTRL_5_VAL_RG_SSUSB_EQ_DLHL_LF_SEL(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LF_SEL, (val))
#define PON_RXFEDIG_CTRL_5_VAL_RG_SSUSB_EQ_DLHL_LFI(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHL_LFI, (val))
#define PON_RXFEDIG_CTRL_5_VAL_RG_SSUSB_EQ_DLHLOS_LFI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLHLOS_LFI, (val))
#define PON_RXFEDIG_CTRL_5_VAL_RG_SSUSB_EQ_DLEQ_LF_SEL(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LF_SEL, (val))
#define PON_RXFEDIG_CTRL_5_VAL_RG_SSUSB_EQ_DLEQ_LFI_GEN4(val)  REG_FLD_VAL(PON_RXFEDIG_CTRL_5_FLD_RG_SSUSB_EQ_DLEQ_LFI_GEN4, (val))

#define PON_RXFEDIG_CTRL_6_VAL_RG_SSUSB_EQ_EYE1_Y(val)         REG_FLD_VAL(PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE1_Y, (val))
#define PON_RXFEDIG_CTRL_6_VAL_RG_SSUSB_EQ_EYE0_Y(val)         REG_FLD_VAL(PON_RXFEDIG_CTRL_6_FLD_RG_SSUSB_EQ_EYE0_Y, (val))

#define PON_RXFEDIG_CTRL_7_VAL_RG_SSUSB_EQ_EYE_CNT(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_7_FLD_RG_SSUSB_EQ_EYE_CNT, (val))

#define PON_RXFEDIG_CTRL_8_VAL_RG_SSUSB_EQ_LEQMAX(val)         REG_FLD_VAL(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_LEQMAX, (val))
#define PON_RXFEDIG_CTRL_8_VAL_RG_SSUSB_EQ_ISIFLAG_SEL(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_ISIFLAG_SEL, (val))
#define PON_RXFEDIG_CTRL_8_VAL_RG_SSUSB_EQ_EYE_XOFFSET(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_XOFFSET, (val))
#define PON_RXFEDIG_CTRL_8_VAL_RG_SSUSB_EQ_EYE_MASK(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_8_FLD_RG_SSUSB_EQ_EYE_MASK, (val))

#define PON_RXFEDIG_CTRL_9_VAL_RG_SSUSB_EQ_PI0_INI(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_PI0_INI, (val))
#define PON_RXFEDIG_CTRL_9_VAL_RG_SSUSB_EQ_MON_SEL(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_MON_SEL, (val))
#define PON_RXFEDIG_CTRL_9_VAL_RG_SSUSB_EQ_LEQ_STOP_TO(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_STOP_TO, (val))
#define PON_RXFEDIG_CTRL_9_VAL_RG_SSUSB_EQ_LEQ_SHIFT(val)      REG_FLD_VAL(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQ_SHIFT, (val))
#define PON_RXFEDIG_CTRL_9_VAL_RG_SSUSB_EQ_LEQOSC_DLYCNT(val)  REG_FLD_VAL(PON_RXFEDIG_CTRL_9_FLD_RG_SSUSB_EQ_LEQOSC_DLYCNT, (val))

#define PON_RXFEDIG_CTRL_10_VAL_RG_SSUSB_EQ_PI_KPGAIN(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI_KPGAIN, (val))
#define PON_RXFEDIG_CTRL_10_VAL_RG_SSUSB_EQ_PIEYE_INI(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PIEYE_INI, (val))
#define PON_RXFEDIG_CTRL_10_VAL_RG_SSUSB_EQ_PI90_INI(val)      REG_FLD_VAL(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90_INI, (val))
#define PON_RXFEDIG_CTRL_10_VAL_RG_SSUSB_EQ_PI90CK_SEL(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_10_FLD_RG_SSUSB_EQ_PI90CK_SEL, (val))

#define PON_RXFEDIG_CTRL_11_VAL_RG_SSUSB_EQ_RESERVED(val)      REG_FLD_VAL(PON_RXFEDIG_CTRL_11_FLD_RG_SSUSB_EQ_RESERVED, (val))

#define PON_RXFEDIG_CTRL_12_VAL_RG_SSUSB_EQ_REV(val)           REG_FLD_VAL(PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV, (val))
#define PON_RXFEDIG_CTRL_12_VAL_RG_SSUSB_EQ_REV_1(val)         REG_FLD_VAL(PON_RXFEDIG_CTRL_12_FLD_RG_SSUSB_EQ_REV_1, (val))

#define PON_RXFEDIG_CTRL_13_VAL_RG_SSUSB_EQ_STARTTIME(val)     REG_FLD_VAL(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_STARTTIME, (val))
#define PON_RXFEDIG_CTRL_13_VAL_RG_SSUSB_EQ_SIGDET(val)        REG_FLD_VAL(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SIGDET, (val))
#define PON_RXFEDIG_CTRL_13_VAL_RG_SSUSB_EQ_SD_CNT1(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT1, (val))
#define PON_RXFEDIG_CTRL_13_VAL_RG_SSUSB_EQ_SD_CNT0(val)       REG_FLD_VAL(PON_RXFEDIG_CTRL_13_FLD_RG_SSUSB_EQ_SD_CNT0, (val))

#define PON_RXFEDIG_CTRL_14_VAL_RG_SSUSB_EQ_TRI_DET_TH(val)    REG_FLD_VAL(PON_RXFEDIG_CTRL_14_FLD_RG_SSUSB_EQ_TRI_DET_TH, (val))

#define PON_RXFEDIG_STS_0_VAL_RGS_SSUSB_EQ_LEQ_STOP(val)       REG_FLD_VAL(PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_LEQ_STOP, (val))
#define PON_RXFEDIG_STS_0_VAL_RGS_SSUSB_EQ_EYE_CNT_RDY(val)    REG_FLD_VAL(PON_RXFEDIG_STS_0_FLD_RGS_SSUSB_EQ_EYE_CNT_RDY, (val))

#define PON_RXFEDIG_STS_1_VAL_RGS_SSUSB_EQ_DCD1L(val)          REG_FLD_VAL(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1L, (val))
#define PON_RXFEDIG_STS_1_VAL_RGS_SSUSB_EQ_DCD1H(val)          REG_FLD_VAL(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD1H, (val))
#define PON_RXFEDIG_STS_1_VAL_RGS_SSUSB_EQ_DCD0L(val)          REG_FLD_VAL(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0L, (val))
#define PON_RXFEDIG_STS_1_VAL_RGS_SSUSB_EQ_DCD0H(val)          REG_FLD_VAL(PON_RXFEDIG_STS_1_FLD_RGS_SSUSB_EQ_DCD0H, (val))

#define PON_RXFEDIG_STS_2_VAL_RGS_SSUSB_EQ_DCEYE1(val)         REG_FLD_VAL(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE1, (val))
#define PON_RXFEDIG_STS_2_VAL_RGS_SSUSB_EQ_DCEYE0(val)         REG_FLD_VAL(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCEYE0, (val))
#define PON_RXFEDIG_STS_2_VAL_RGS_SSUSB_EQ_DCE1(val)           REG_FLD_VAL(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE1, (val))
#define PON_RXFEDIG_STS_2_VAL_RGS_SSUSB_EQ_DCE0(val)           REG_FLD_VAL(PON_RXFEDIG_STS_2_FLD_RGS_SSUSB_EQ_DCE0, (val))

#define PON_RXFEDIG_STS_3_VAL_RGS_SSUSB_EQ_DCLHL(val)          REG_FLD_VAL(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLHL, (val))
#define PON_RXFEDIG_STS_3_VAL_RGS_SSUSB_EQ_DCLEQOS(val)        REG_FLD_VAL(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQOS, (val))
#define PON_RXFEDIG_STS_3_VAL_RGS_SSUSB_EQ_DCLEQ(val)          REG_FLD_VAL(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCLEQ, (val))
#define PON_RXFEDIG_STS_3_VAL_RGS_SSUSB_EQ_DCHHL(val)          REG_FLD_VAL(PON_RXFEDIG_STS_3_FLD_RGS_SSUSB_EQ_DCHHL, (val))

#define PON_RXFEDIG_STS_4_VAL_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0(val) REG_FLD_VAL(PON_RXFEDIG_STS_4_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0, (val))

#define PON_RXFEDIG_STS_5_VAL_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1(val) REG_FLD_VAL(PON_RXFEDIG_STS_5_FLD_RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1, (val))

#define PON_RXFEDIG_STS_7_VAL_RGS_SSUSB_EQ_STATUS(val)         REG_FLD_VAL(PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_STATUS, (val))
#define PON_RXFEDIG_STS_7_VAL_RGS_SSUSB_EQ_PILPO(val)          REG_FLD_VAL(PON_RXFEDIG_STS_7_FLD_RGS_SSUSB_EQ_PILPO, (val))

#define PON_RXFEDIG_STS_8_VAL_RGS_SSUSB_RX_SD_OUT(val)         REG_FLD_VAL(PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_RX_SD_OUT, (val))
#define PON_RXFEDIG_STS_8_VAL_RGS_SSUSB_EQ_PROBE_OUT(val)      REG_FLD_VAL(PON_RXFEDIG_STS_8_FLD_RGS_SSUSB_EQ_PROBE_OUT, (val))

#define PON_DA_CTRL_0_VAL_RG_SSUSB_PLL_IC(val)                 REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IC, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_PLL_BR(val)                 REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BR, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_PLL_IR(val)                 REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_IR, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_PLL_BP(val)                 REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_BP, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_PLL_LOAD_EN(val)            REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_LOAD_EN, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_PLL_FBKSEL(val)             REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_PLL_FBKSEL, (val))
#define PON_DA_CTRL_0_VAL_RG_PCIE_CKDET_EN(val)                REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_PCIE_CKDET_EN, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_SCAN_MODE(val)              REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_SCAN_MODE, (val))
#define PON_DA_CTRL_0_VAL_RG_PCIE_CLKRX_EN(val)                REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_PCIE_CLKRX_EN, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_XTAL_EXT_EN(val)            REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_XTAL_EXT_EN, (val))
#define PON_DA_CTRL_0_VAL_RG_PCIE_DEVICE(val)                  REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_PCIE_DEVICE, (val))
#define PON_DA_CTRL_0_VAL_RG_PCIE_REFCK_DIV4(val)              REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_PCIE_REFCK_DIV4, (val))
#define PON_DA_CTRL_0_VAL_RG_SSUSB_RX_IMPCALIB_EN(val)         REG_FLD_VAL(PON_DA_CTRL_0_FLD_RG_SSUSB_RX_IMPCALIB_EN, (val))

#define PON_DA_CTRL_1_VAL_RG_SSUSB_CDR_BPB(val)                REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_CDR_BPB, (val))
#define PON_DA_CTRL_1_VAL_RG_SSUSB_TX_IMPCALIB_EN(val)         REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_TX_IMPCALIB_EN, (val))
#define PON_DA_CTRL_1_VAL_RG_SSUSB_TX_DEM(val)                 REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_TX_DEM, (val))
#define PON_DA_CTRL_1_VAL_RG_SSUSB_RXDET_EN(val)               REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_RXDET_EN, (val))
#define PON_DA_CTRL_1_VAL_RG_SSUSB_PLL_REFCKDIV(val)           REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_REFCKDIV, (val))
#define PON_DA_CTRL_1_VAL_RG_SSUSB_PLL_PREDIV(val)             REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_PREDIV, (val))
#define PON_DA_CTRL_1_VAL_RG_SSUSB_PLL_BC(val)                 REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_BC, (val))
#define PON_DA_CTRL_1_VAL_RG_SSUSB_PLL_DIVEN(val)              REG_FLD_VAL(PON_DA_CTRL_1_FLD_RG_SSUSB_PLL_DIVEN, (val))

#define PON_DA_CTRL_2_VAL_RG_SSUSB_EQ_RSTEP1(val)              REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP1, (val))
#define PON_DA_CTRL_2_VAL_RG_SSUSB_EQ_RSTEP2(val)              REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_EQ_RSTEP2, (val))
#define PON_DA_CTRL_2_VAL_RG_SSUSB_LFPS_PWD(val)               REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_LFPS_PWD, (val))
#define PON_DA_CTRL_2_VAL_RG_SSUSB_CDR_PI_PWD(val)             REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_PWD, (val))
#define PON_DA_CTRL_2_VAL_RG_SSUSB_CDR_PI_MODE(val)            REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_PI_MODE, (val))
#define PON_DA_CTRL_2_VAL_RG_SSUSB_CDR_TCADJ(val)              REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_TCADJ, (val))
#define PON_DA_CTRL_2_VAL_RG_SSUSB_CDR_FBDIV(val)              REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_FBDIV, (val))
#define PON_DA_CTRL_2_VAL_RG_SSUSB_CDR_BPA(val)                REG_FLD_VAL(PON_DA_CTRL_2_FLD_RG_SSUSB_CDR_BPA, (val))

#define PON_DA_CTRL_3_VAL_RG_SSUSB_SYSPLL_FBDIV_CHG(val)       REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_SSUSB_SYSPLL_FBDIV_CHG, (val))
#define PON_DA_CTRL_3_VAL_RG_PCIE_CLKTX_EN(val)                REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_PCIE_CLKTX_EN, (val))
#define PON_DA_CTRL_3_VAL_RG_PCIE_CRSDET_EN(val)               REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_EN, (val))
#define PON_DA_CTRL_3_VAL_RG_PCIE_CRSDET_RSTB(val)             REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_RSTB, (val))
#define PON_DA_CTRL_3_VAL_RG_PCIE_CRSDET_OFST(val)             REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_PCIE_CRSDET_OFST, (val))
#define PON_DA_CTRL_3_VAL_RG_SSUSB_RX_HZ(val)                  REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_HZ, (val))
#define PON_DA_CTRL_3_VAL_RG_SSUSB_RX_P3_ENTRY(val)            REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P3_ENTRY, (val))
#define PON_DA_CTRL_3_VAL_RG_SSUSB_RX_P1_ENTRY(val)            REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_SSUSB_RX_P1_ENTRY, (val))
#define PON_DA_CTRL_3_VAL_RG_SSUSB_LFPS_DEGLITCH(val)          REG_FLD_VAL(PON_DA_CTRL_3_FLD_RG_SSUSB_LFPS_DEGLITCH, (val))

#define PON_DA_CTRL_4_VAL_RG_SSUSB_PLL_FBKDIV(val)             REG_FLD_VAL(PON_DA_CTRL_4_FLD_RG_SSUSB_PLL_FBKDIV, (val))

#define PON_DA_CTRL_5_VAL_RG_FORCE_CDR_PI0(val)                REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PI0, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_SYSPLL_SDM_PWR_ON(val)      REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_PWR_ON, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_SYSPLL_SDM_ISO_EN(val)      REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_SDM_ISO_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_CDR_PWD(val)                REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_CDR_AUTOK_VCO(val)          REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_AUTOK_VCO, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_CDR_LCK2REF(val)            REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_LCK2REF, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_RX_CMPWD(val)               REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_RX_CMPWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_RX_AFE_PWD(val)             REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_RX_AFE_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_TX_DATA_EN(val)             REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_TX_DATA_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_TX_EIDLE_LP_EN(val)         REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_TX_EIDLE_LP_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_TX_SER_EN(val)              REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_TX_SER_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_SIGDET_EN(val)              REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_SIGDET_CAL_EN(val)          REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_SIGDET_CAL_OFFSET(val)      REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_SIGDET_CAL_OFFSET, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_RX_PI_CAL_EN(val)           REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_RX_PI_CAL_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_TX_DRV_EN(val)              REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_TX_DRV_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_TX_LFPS(val)                REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_TX_LFPS_EN(val)             REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_TX_LFPS_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_RX_SAOSC_EN(val)            REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_RX_SAOSC_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_BIAS_PWD(val)               REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_BIAS_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_BIASIMR_PWD(val)            REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_BIASIMR_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_BG_LPF_EN(val)              REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_BG_LPF_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_PLL_BIAS_PWD(val)           REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_XTAL_RX_PWD(val)            REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_XTAL_RX_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_SYSPLL_PWD(val)             REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_SYSPLL_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_PLL_PWD(val)                REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_PWD, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_PLL_AUTOK_VCO(val)          REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_AUTOK_VCO, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_PLL_SSC_EN(val)             REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_SSC_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_PLL_BIAS_LPF_EN(val)        REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BIAS_LPF_EN, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_CDR_BAND(val)               REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_CDR_BAND, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_PLL_BAND(val)               REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_PLL_BAND, (val))
#define PON_DA_CTRL_5_VAL_RG_FORCE_RX_LEQOS_EN(val)            REG_FLD_VAL(PON_DA_CTRL_5_FLD_RG_FORCE_RX_LEQOS_EN, (val))

#define PON_DA_CTRL_6_VAL_RG_FORCE_IEXT_INTR_CTRL(val)         REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_IEXT_INTR_CTRL, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_RX_IMP_SEL(val)             REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_RX_IMP_SEL, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_TX_IMP_SEL(val)             REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_TX_IMP_SEL, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_RX_EQ_EN(val)               REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_EN, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_RX_EQ_RST(val)              REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_RX_EQ_RST, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_RX_SAOSC_RST(val)           REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_RX_SAOSC_RST, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_RX_CDR_STABLE(val)          REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_RX_CDR_STABLE, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_CDR_KVSEL(val)              REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_KVSEL, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_CDR_BR(val)                 REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BR, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_CDR_BC(val)                 REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BC, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_CDR_BIC(val)                REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIC, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_CDR_BIR(val)                REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_BIR, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_TX_EIDLE_CM(val)            REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_TX_EIDLE_CM, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_TX_BIASI_EN(val)            REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI_EN, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_TX_BIASI(val)               REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_TX_BIASI, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_IDEM_6DB(val)               REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_6DB, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_IDRV_6DB(val)               REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_6DB, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_IDEM_3P5DB(val)             REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_IDEM_3P5DB, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_IDRV_3P5DB(val)             REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_3P5DB, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_IDRV_0DB(val)               REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_IDRV_0DB, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_SYSPLL_PREDIV(val)          REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_PREDIV, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_SYSPLL_RESERVE(val)         REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_RESERVE, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_SYSPLL_FBDIV(val)           REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_SYSPLL_FBDIV, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_PLL_SSC_PRD(val)            REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_PRD, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_PLL_SSC_DELTA(val)          REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_PLL_SSC_DELTA1(val)         REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_PLL_SSC_DELTA1, (val))
#define PON_DA_CTRL_6_VAL_RG_FORCE_CDR_PI90(val)               REG_FLD_VAL(PON_DA_CTRL_6_FLD_RG_FORCE_CDR_PI90, (val))

#define PON_DA_CTRL_7_VAL_RG_SSUSB_BG_LPF_EN(val)              REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_BG_LPF_EN, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_PLL_BIAS_PWD(val)           REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_PWD, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_XTAL_RX_PWD(val)            REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_XTAL_RX_PWD, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_SYSPLL_PWD(val)             REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_SYSPLL_PWD, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_PLL_PWD(val)                REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_PWD, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_PLL_AUTOK_VCO(val)          REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_AUTOK_VCO, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_PLL_SSC_EN(val)             REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_SSC_EN, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_PLL_BIAS_LPF_EN(val)        REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BIAS_LPF_EN, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_CDR_BAND(val)               REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_CDR_BAND, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_RX_LEQOS_EN(val)            REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_RX_LEQOS_EN, (val))
#define PON_DA_CTRL_7_VAL_RG_SSUSB_PLL_BAND(val)               REG_FLD_VAL(PON_DA_CTRL_7_FLD_RG_SSUSB_PLL_BAND, (val))

#define PON_DA_CTRL_8_VAL_RG_SSUSB_SYSPLL_SDM_PWR_ON(val)      REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_PWR_ON, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_SYSPLL_SDM_ISO_EN(val)      REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_SYSPLL_SDM_ISO_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_CDR_PWD(val)                REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_PWD, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_CDR_AUTOK_VCO(val)          REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_AUTOK_VCO, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_CDR_LCK2REF(val)            REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_CDR_LCK2REF, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_RX_CMPWD(val)               REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_CMPWD, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_RX_AFE_PWD(val)             REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_AFE_PWD, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_TX_DATA_EN(val)             REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DATA_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_TX_EIDLE_LP_EN(val)         REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_EIDLE_LP_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_TX_SER_EN(val)              REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_SER_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_PCIE_SIGDET_EN(val)               REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_PCIE_SIGDET_CAL_EN(val)           REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_PCIE_SIGDET_CAL_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_SIGDET_CAL_OFFSET(val)      REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_SIGDET_CAL_OFFSET, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_RX_PI_CAL_EN(val)           REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_PI_CAL_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_TX_DRV_EN(val)              REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_DRV_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_TX_LFPS(val)                REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_TX_LFPS_EN(val)             REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_TX_LFPS_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_RX_SAOSC_EN(val)            REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_RX_SAOSC_EN, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_BIAS_PWD(val)               REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_BIAS_PWD, (val))
#define PON_DA_CTRL_8_VAL_RG_SSUSB_BIASIMR_PWD(val)            REG_FLD_VAL(PON_DA_CTRL_8_FLD_RG_SSUSB_BIASIMR_PWD, (val))

#define PON_DA_CTRL_9_VAL_RG_SSUSB_CDR_PI90(val)               REG_FLD_VAL(PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI90, (val))
#define PON_DA_CTRL_9_VAL_RG_SSUSB_CDR_PI0(val)                REG_FLD_VAL(PON_DA_CTRL_9_FLD_RG_SSUSB_CDR_PI0, (val))

#define PON_DA_CTRL_10_VAL_RG_SSUSB_PLL_SSC_DELTA(val)         REG_FLD_VAL(PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA, (val))
#define PON_DA_CTRL_10_VAL_RG_SSUSB_PLL_SSC_DELTA1(val)        REG_FLD_VAL(PON_DA_CTRL_10_FLD_RG_SSUSB_PLL_SSC_DELTA1, (val))

#define PON_DA_CTRL_11_VAL_RG_SSUSB_SYSPLL_FBDIV(val)          REG_FLD_VAL(PON_DA_CTRL_11_FLD_RG_SSUSB_SYSPLL_FBDIV, (val))

#define PON_DA_CTRL_12_VAL_RG_SSUSB_IDRV_3P5DB(val)            REG_FLD_VAL(PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_3P5DB, (val))
#define PON_DA_CTRL_12_VAL_RG_SSUSB_IDRV_0DB(val)              REG_FLD_VAL(PON_DA_CTRL_12_FLD_RG_SSUSB_IDRV_0DB, (val))

#define PON_DA_CTRL_13_VAL_RG_SSUSB_TX_BIASI_EN(val)           REG_FLD_VAL(PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI_EN, (val))
#define PON_DA_CTRL_13_VAL_RG_SSUSB_TX_BIASI(val)              REG_FLD_VAL(PON_DA_CTRL_13_FLD_RG_SSUSB_TX_BIASI, (val))
#define PON_DA_CTRL_13_VAL_RG_SSUSB_IDEM_6DB(val)              REG_FLD_VAL(PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_6DB, (val))
#define PON_DA_CTRL_13_VAL_RG_SSUSB_IDRV_6DB(val)              REG_FLD_VAL(PON_DA_CTRL_13_FLD_RG_SSUSB_IDRV_6DB, (val))
#define PON_DA_CTRL_13_VAL_RG_SSUSB_IDEM_3P5DB(val)            REG_FLD_VAL(PON_DA_CTRL_13_FLD_RG_SSUSB_IDEM_3P5DB, (val))

#define PON_DA_CTRL_14_VAL_RG_SSUSB_RX_IMP_SEL(val)            REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_IMP_SEL, (val))
#define PON_DA_CTRL_14_VAL_RG_SSUSB_TX_IMP_SEL(val)            REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_TX_IMP_SEL, (val))
#define PON_DA_CTRL_14_VAL_RG_SSUSB_RX_EQ_EN(val)              REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_EN, (val))
#define PON_DA_CTRL_14_VAL_RG_SSUSB_RX_EQ_RST(val)             REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_EQ_RST, (val))
#define PON_DA_CTRL_14_VAL_RG_SSUSB_RX_SAOSC_RST(val)          REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_SAOSC_RST, (val))
#define PON_DA_CTRL_14_VAL_RG_SSUSB_RX_CDR_STABLE(val)         REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_RX_CDR_STABLE, (val))
#define PON_DA_CTRL_14_VAL_RG_SSUSB_CDR_BIR(val)               REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_CDR_BIR, (val))
#define PON_DA_CTRL_14_VAL_RG_SSUSB_TX_EIDLE_CM(val)           REG_FLD_VAL(PON_DA_CTRL_14_FLD_RG_SSUSB_TX_EIDLE_CM, (val))

#define PON_DA_CTRL_15_VAL_RG_SSUSB_CDR_BIRLTD1(val)           REG_FLD_VAL(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD1, (val))
#define PON_DA_CTRL_15_VAL_RG_SSUSB_CDR_BIRLTD0(val)           REG_FLD_VAL(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTD0, (val))
#define PON_DA_CTRL_15_VAL_RG_SSUSB_CDR_BIRLTR(val)            REG_FLD_VAL(PON_DA_CTRL_15_FLD_RG_SSUSB_CDR_BIRLTR, (val))

#define PON_DA_CTRL_16_VAL_RG_SSUSB_CDR_BCLTD0(val)            REG_FLD_VAL(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTD0, (val))
#define PON_DA_CTRL_16_VAL_RG_SSUSB_CDR_BCLTR(val)             REG_FLD_VAL(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BCLTR, (val))
#define PON_DA_CTRL_16_VAL_RG_SSUSB_CDR_BICLTD1(val)           REG_FLD_VAL(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD1, (val))
#define PON_DA_CTRL_16_VAL_RG_SSUSB_CDR_BICLTD0(val)           REG_FLD_VAL(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTD0, (val))
#define PON_DA_CTRL_16_VAL_RG_SSUSB_CDR_BICLTR(val)            REG_FLD_VAL(PON_DA_CTRL_16_FLD_RG_SSUSB_CDR_BICLTR, (val))

#define PON_DA_CTRL_17_VAL_RG_SSUSB_CDR_KVSELLTD1(val)         REG_FLD_VAL(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD1, (val))
#define PON_DA_CTRL_17_VAL_RG_SSUSB_CDR_KVSELLTD0(val)         REG_FLD_VAL(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTD0, (val))
#define PON_DA_CTRL_17_VAL_RG_SSUSB_CDR_KVSELLTR(val)          REG_FLD_VAL(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_KVSELLTR, (val))
#define PON_DA_CTRL_17_VAL_RG_SSUSB_CDR_BRLTD1(val)            REG_FLD_VAL(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD1, (val))
#define PON_DA_CTRL_17_VAL_RG_SSUSB_CDR_BRLTD0(val)            REG_FLD_VAL(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTD0, (val))
#define PON_DA_CTRL_17_VAL_RG_SSUSB_CDR_BRLTR(val)             REG_FLD_VAL(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BRLTR, (val))
#define PON_DA_CTRL_17_VAL_RG_SSUSB_CDR_BCLTD1(val)            REG_FLD_VAL(PON_DA_CTRL_17_FLD_RG_SSUSB_CDR_BCLTD1, (val))

#define PON_DA_CTRL_18_VAL_RG_FORCE_SSUSB_PLL_BPB(val)         REG_FLD_VAL(PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPB, (val))
#define PON_DA_CTRL_18_VAL_RG_FORCE_SSUSB_PLL_BPA(val)         REG_FLD_VAL(PON_DA_CTRL_18_FLD_RG_FORCE_SSUSB_PLL_BPA, (val))

#define PON_DA_CTRL_19_VAL_RG_SSUSB_PLL_BPB(val)               REG_FLD_VAL(PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPB, (val))
#define PON_DA_CTRL_19_VAL_RG_SSUSB_PLL_BPA(val)               REG_FLD_VAL(PON_DA_CTRL_19_FLD_RG_SSUSB_PLL_BPA, (val))

#define PON_DEBUG_REGISTER_0_VAL_ro_ad_ben_out_cnt(val)        REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out_cnt, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_ad_ben_out(val)            REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_ad_ben_out, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_intf_pma_rx_sigdet_out(val) REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_intf_pma_rx_sigdet_out, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_ad_rx_lfps(val)            REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_lfps, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_ad_rx_impcal_out(val)      REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_ad_rx_impcal_out, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_rx_path_rdy(val)           REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_rx_path_rdy, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_rx_eq_en(val)              REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_rx_eq_en, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_eq_done(val)               REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_eq_done, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_ad_sigdet_cal_out(val)     REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_cal_out, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_ad_sigdet_out(val)         REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_ad_sigdet_out, (val))
#define PON_DEBUG_REGISTER_0_VAL_ro_lck2ref(val)               REG_FLD_VAL(PON_DEBUG_REGISTER_0_FLD_ro_lck2ref, (val))

#define PON_DEBUG_REGISTER_1_VAL_rg_eq_done_clr(val)           REG_FLD_VAL(PON_DEBUG_REGISTER_1_FLD_rg_eq_done_clr, (val))

#define PON_DEBUG_REGISTER_2_VAL_ro_qphy_rx_ctrl_debug(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_2_FLD_ro_qphy_rx_ctrl_debug, (val))
#define PON_DEBUG_REGISTER_2_VAL_ro_da_tx_data_en(val)         REG_FLD_VAL(PON_DEBUG_REGISTER_2_FLD_ro_da_tx_data_en, (val))
#define PON_DEBUG_REGISTER_2_VAL_ro_txbit_dly_sel_latch(val)   REG_FLD_VAL(PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel_latch, (val))
#define PON_DEBUG_REGISTER_2_VAL_ro_txbit_dly_sel(val)         REG_FLD_VAL(PON_DEBUG_REGISTER_2_FLD_ro_txbit_dly_sel, (val))
#define PON_DEBUG_REGISTER_2_VAL_ro_pll_vco_cal_state(val)     REG_FLD_VAL(PON_DEBUG_REGISTER_2_FLD_ro_pll_vco_cal_state, (val))

#define PON_DEBUG_REGISTER_3_VAL_ro_ad_pcie_crsdet_out(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_out, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_ad_pcie_crsdet_lthout(val) REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_lthout, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_ad_pcie_crsdet_cal_out(val) REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_crsdet_cal_out, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_ad_pcie_ckdet_out(val)     REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_ad_pcie_ckdet_out, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_saosc_done(val)            REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_saosc_done, (val))
#define PON_DEBUG_REGISTER_3_VAL_rg_freq_clr(val)              REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_rg_freq_clr, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_baser_rx_freq_lock(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_baser_rx_freq_lock, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_baser_tx_freq_lock(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_baser_tx_freq_lock, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_hg_rx_freq_lock(val)       REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_hg_rx_freq_lock, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_hg_tx_freq_lock(val)       REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_hg_tx_freq_lock, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_pon_rx_freq_lock(val)      REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_pon_rx_freq_lock, (val))
#define PON_DEBUG_REGISTER_3_VAL_ro_pon_tx_freq_lock(val)      REG_FLD_VAL(PON_DEBUG_REGISTER_3_FLD_ro_pon_tx_freq_lock, (val))

#define PON_DEBUG_REGISTER_4_VAL_rg_ben_count_en(val)          REG_FLD_VAL(PON_DEBUG_REGISTER_4_FLD_rg_ben_count_en, (val))
#define PON_DEBUG_REGISTER_4_VAL_ro_ben_counter(val)           REG_FLD_VAL(PON_DEBUG_REGISTER_4_FLD_ro_ben_counter, (val))

#define PON_DUMMY_REG_0_VAL_rg_dummy_reg(val)                  REG_FLD_VAL(PON_DUMMY_REG_0_FLD_rg_dummy_reg, (val))

#define PON_DEBUG_REGISTER_5_VAL_rg_fec_dec_insert_dbg_clr(val) REG_FLD_VAL(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_insert_dbg_clr, (val))
#define PON_DEBUG_REGISTER_5_VAL_rg_fec_dec_usr_def_error_en(val) REG_FLD_VAL(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_usr_def_error_en, (val))
#define PON_DEBUG_REGISTER_5_VAL_rg_fec_dec_err_insert_cont(val) REG_FLD_VAL(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_cont, (val))
#define PON_DEBUG_REGISTER_5_VAL_rg_fec_dec_err_insert_en(val) REG_FLD_VAL(PON_DEBUG_REGISTER_5_FLD_rg_fec_dec_err_insert_en, (val))

#define PON_DEBUG_REGISTER_6_VAL_rg_fec_dec_err_byte_num(val)  REG_FLD_VAL(PON_DEBUG_REGISTER_6_FLD_rg_fec_dec_err_byte_num, (val))

#define PON_DEBUG_REGISTER_7_VAL_rg_fec_dec_usr_def_error(val) REG_FLD_VAL(PON_DEBUG_REGISTER_7_FLD_rg_fec_dec_usr_def_error, (val))

#define PON_DEBUG_REGISTER_8_VAL_rg_fec_dec_err_insert_start_w1c(val) REG_FLD_VAL(PON_DEBUG_REGISTER_8_FLD_rg_fec_dec_err_insert_start_w1c, (val))

#define PON_DEBUG_REGISTER_9_VAL_rg_fec_enc_insert_dbg_clr(val) REG_FLD_VAL(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_insert_dbg_clr, (val))
#define PON_DEBUG_REGISTER_9_VAL_rg_fec_enc_usr_def_error_en(val) REG_FLD_VAL(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_usr_def_error_en, (val))
#define PON_DEBUG_REGISTER_9_VAL_rg_fec_enc_err_insert_cont(val) REG_FLD_VAL(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_cont, (val))
#define PON_DEBUG_REGISTER_9_VAL_rg_fec_enc_err_insert_en(val) REG_FLD_VAL(PON_DEBUG_REGISTER_9_FLD_rg_fec_enc_err_insert_en, (val))

#define PON_DEBUG_REGISTER_10_VAL_rg_fec_enc_err_byte_num(val) REG_FLD_VAL(PON_DEBUG_REGISTER_10_FLD_rg_fec_enc_err_byte_num, (val))

#define PON_DEBUG_REGISTER_11_VAL_rg_fec_enc_usr_def_error(val) REG_FLD_VAL(PON_DEBUG_REGISTER_11_FLD_rg_fec_enc_usr_def_error, (val))

#define PON_DEBUG_REGISTER_12_VAL_rg_fec_enc_err_insert_start_w1c(val) REG_FLD_VAL(PON_DEBUG_REGISTER_12_FLD_rg_fec_enc_err_insert_start_w1c, (val))

#define PON_DEBUG_REGISTER_13_VAL_rg_trans_los_pro_cnt_thr(val) REG_FLD_VAL(PON_DEBUG_REGISTER_13_FLD_rg_trans_los_pro_cnt_thr, (val))
#define PON_DEBUG_REGISTER_13_VAL_rg_phyrdy_pro_cnt_thr(val)   REG_FLD_VAL(PON_DEBUG_REGISTER_13_FLD_rg_phyrdy_pro_cnt_thr, (val))

#define PON_DEBUG_REGISTER_14_VAL_rg_int_record_start_clr_w1c(val) REG_FLD_VAL(PON_DEBUG_REGISTER_14_FLD_rg_int_record_start_clr_w1c, (val))

#define PON_DEBUG_REGISTER_15_VAL_rg_record_start_type(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_15_FLD_rg_record_start_type, (val))
#define PON_DEBUG_REGISTER_15_VAL_rg_record_start_dbg_clr(val) REG_FLD_VAL(PON_DEBUG_REGISTER_15_FLD_rg_record_start_dbg_clr, (val))
#define PON_DEBUG_REGISTER_15_VAL_rg_lof_int_mask_keep(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_keep, (val))
#define PON_DEBUG_REGISTER_15_VAL_rg_lof_int_mask_en(val)      REG_FLD_VAL(PON_DEBUG_REGISTER_15_FLD_rg_lof_int_mask_en, (val))
#define PON_DEBUG_REGISTER_15_VAL_rg_int_record_en(val)        REG_FLD_VAL(PON_DEBUG_REGISTER_15_FLD_rg_int_record_en, (val))

#define PON_DEBUG_REGISTER_16_VAL_ro_lof_time_cnt(val)         REG_FLD_VAL(PON_DEBUG_REGISTER_16_FLD_ro_lof_time_cnt, (val))

#define PON_DEBUG_REGISTER_17_VAL_ro_los_time_cnt(val)         REG_FLD_VAL(PON_DEBUG_REGISTER_17_FLD_ro_los_time_cnt, (val))

#define PON_DEBUG_REGISTER_18_VAL_ro_phyrdy_time_cnt(val)      REG_FLD_VAL(PON_DEBUG_REGISTER_18_FLD_ro_phyrdy_time_cnt, (val))

#define PON_DEBUG_REGISTER_19_VAL_ro_tx_bit_dly(val)           REG_FLD_VAL(PON_DEBUG_REGISTER_19_FLD_ro_tx_bit_dly, (val))

#define PON_DEBUG_REGISTER_20_VAL_rg_int_record_cnt_thr(val)   REG_FLD_VAL(PON_DEBUG_REGISTER_20_FLD_rg_int_record_cnt_thr, (val))

#define PON_DEBUG_REGISTER_21_VAL_ro_record_done(val)          REG_FLD_VAL(PON_DEBUG_REGISTER_21_FLD_ro_record_done, (val))
#define PON_DEBUG_REGISTER_21_VAL_ro_fec_enc_insert_dbg(val)   REG_FLD_VAL(PON_DEBUG_REGISTER_21_FLD_ro_fec_enc_insert_dbg, (val))
#define PON_DEBUG_REGISTER_21_VAL_ro_record_start_dbg(val)     REG_FLD_VAL(PON_DEBUG_REGISTER_21_FLD_ro_record_start_dbg, (val))
#define PON_DEBUG_REGISTER_21_VAL_ro_fec_dec_insert_dbg(val)   REG_FLD_VAL(PON_DEBUG_REGISTER_21_FLD_ro_fec_dec_insert_dbg, (val))

#define PON_DEBUG_REGISTER_22_VAL_rg_lof_int_mask_cnt_thr(val) REG_FLD_VAL(PON_DEBUG_REGISTER_22_FLD_rg_lof_int_mask_cnt_thr, (val))

#define PON_DEBUG_REGISTER_23_VAL_rgs_pll_autok_band(val)      REG_FLD_VAL(PON_DEBUG_REGISTER_23_FLD_rgs_pll_autok_band, (val))

#define PON_DEBUG_REGISTER_24_VAL_rg_fpga_rx_lck_rst(val)      REG_FLD_VAL(PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_lck_rst, (val))
#define PON_DEBUG_REGISTER_24_VAL_rg_fpga_rx_force_lck(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_24_FLD_rg_fpga_rx_force_lck, (val))
#define PON_DEBUG_REGISTER_24_VAL_ro_fpga_tx_los_lock_cnt(val) REG_FLD_VAL(PON_DEBUG_REGISTER_24_FLD_ro_fpga_tx_los_lock_cnt, (val))
#define PON_DEBUG_REGISTER_24_VAL_ro_fpga_rx_los_lock_cnt(val) REG_FLD_VAL(PON_DEBUG_REGISTER_24_FLD_ro_fpga_rx_los_lock_cnt, (val))
#define PON_DEBUG_REGISTER_24_VAL_ro_fpga_lock_status(val)     REG_FLD_VAL(PON_DEBUG_REGISTER_24_FLD_ro_fpga_lock_status, (val))

#define PON_DEBUG_REGISTER_25_VAL_rg_gepon_phy_dlybnd_delsel(val) REG_FLD_VAL(PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_dlybnd_delsel, (val))
#define PON_DEBUG_REGISTER_25_VAL_rg_gepon_phy_epsram_delsel(val) REG_FLD_VAL(PON_DEBUG_REGISTER_25_FLD_rg_gepon_phy_epsram_delsel, (val))
#define PON_DEBUG_REGISTER_25_VAL_rg_pon_phy_ssram_delsel(val) REG_FLD_VAL(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_ssram_delsel, (val))
#define PON_DEBUG_REGISTER_25_VAL_rg_pon_phy_dsram3_delsel(val) REG_FLD_VAL(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram3_delsel, (val))
#define PON_DEBUG_REGISTER_25_VAL_rg_pon_phy_dsram2_delsel(val) REG_FLD_VAL(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram2_delsel, (val))
#define PON_DEBUG_REGISTER_25_VAL_rg_pon_phy_dsram1_delsel(val) REG_FLD_VAL(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram1_delsel, (val))
#define PON_DEBUG_REGISTER_25_VAL_rg_pon_phy_dsram0_delsel(val) REG_FLD_VAL(PON_DEBUG_REGISTER_25_FLD_rg_pon_phy_dsram0_delsel, (val))

#define PON_DEBUG_REGISTER_26_VAL_ro_fpga_debug_0(val)         REG_FLD_VAL(PON_DEBUG_REGISTER_26_FLD_ro_fpga_debug_0, (val))

#define PON_DEBUG_REGISTER_27_VAL_ro_fpga_debug_1(val)         REG_FLD_VAL(PON_DEBUG_REGISTER_27_FLD_ro_fpga_debug_1, (val))

#define PON_DEBUG_REGISTER_28_VAL_ro_ad_sig_cal_offset(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_28_FLD_ro_ad_sig_cal_offset, (val))
#define PON_DEBUG_REGISTER_28_VAL_ro_ad_cdr_band(val)          REG_FLD_VAL(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_band, (val))
#define PON_DEBUG_REGISTER_28_VAL_ro_ad_cdr_vcocal_fail(val)   REG_FLD_VAL(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_fail, (val))
#define PON_DEBUG_REGISTER_28_VAL_ro_ad_cdr_vcocal_cplt(val)   REG_FLD_VAL(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_cplt, (val))
#define PON_DEBUG_REGISTER_28_VAL_ro_ad_cdr_vcocal_state(val)  REG_FLD_VAL(PON_DEBUG_REGISTER_28_FLD_ro_ad_cdr_vcocal_state, (val))

#define PON_DEBUG_REGISTER_29_VAL_ro_sfp_tx_en_counter(val)    REG_FLD_VAL(PON_DEBUG_REGISTER_29_FLD_ro_sfp_tx_en_counter, (val))

#define PON_INTERRUPT_STS_0_VAL_ro_tx_en_int(val)              REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_tx_en_int, (val))
#define PON_INTERRUPT_STS_0_VAL_ro_rx_sd_int(val)              REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_rx_sd_int, (val))
#define PON_INTERRUPT_STS_0_VAL_ro_los_int(val)                REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_los_int, (val))
#define PON_INTERRUPT_STS_0_VAL_ro_rx_pll_meter_unlock(val)    REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_unlock, (val))
#define PON_INTERRUPT_STS_0_VAL_ro_rx_pll_meter_lock(val)      REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_rx_pll_meter_lock, (val))
#define PON_INTERRUPT_STS_0_VAL_ro_pma_rx_sigdet_out(val)      REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_pma_rx_sigdet_out, (val))
#define PON_INTERRUPT_STS_0_VAL_ro_ncpo_min_flag(val)          REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_ncpo_min_flag, (val))
#define PON_INTERRUPT_STS_0_VAL_ro_ncpo_max_flag(val)          REG_FLD_VAL(PON_INTERRUPT_STS_0_FLD_ro_ncpo_max_flag, (val))

#define PON_INTERRUPT_EN_0_VAL_rg_tx_en_int_en(val)            REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_tx_en_int_en, (val))
#define PON_INTERRUPT_EN_0_VAL_rg_rx_sd_int_en(val)            REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_rx_sd_int_en, (val))
#define PON_INTERRUPT_EN_0_VAL_rg_los_int_en(val)              REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_los_int_en, (val))
#define PON_INTERRUPT_EN_0_VAL_rg_rx_pll_meter_unlock_int_en(val) REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_unlock_int_en, (val))
#define PON_INTERRUPT_EN_0_VAL_rg_rx_pll_meter_lock_int_en(val) REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_rx_pll_meter_lock_int_en, (val))
#define PON_INTERRUPT_EN_0_VAL_rg_pma_sigdet_int_en(val)       REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_pma_sigdet_int_en, (val))
#define PON_INTERRUPT_EN_0_VAL_rg_ncpo_min_int_en(val)         REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_ncpo_min_int_en, (val))
#define PON_INTERRUPT_EN_0_VAL_rg_ncpo_max_int_en(val)         REG_FLD_VAL(PON_INTERRUPT_EN_0_FLD_rg_ncpo_max_int_en, (val))

#define PON_INTERRUPT_CLR_0_VAL_rg_tx_en_int_clr(val)          REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_tx_en_int_clr, (val))
#define PON_INTERRUPT_CLR_0_VAL_rg_rx_sd_int_clr(val)          REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_rx_sd_int_clr, (val))
#define PON_INTERRUPT_CLR_0_VAL_rg_los_int_clr(val)            REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_los_int_clr, (val))
#define PON_INTERRUPT_CLR_0_VAL_rg_rx_pll_meter_unlock_int_clr(val) REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_unlock_int_clr, (val))
#define PON_INTERRUPT_CLR_0_VAL_rg_rx_pll_meter_lock_int_clr(val) REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_rx_pll_meter_lock_int_clr, (val))
#define PON_INTERRUPT_CLR_0_VAL_rg_pma_rx_sigdet_intclr(val)   REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_pma_rx_sigdet_intclr, (val))
#define PON_INTERRUPT_CLR_0_VAL_rg_ncpo_flag_clr(val)          REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_ncpo_flag_clr, (val))
#define PON_INTERRUPT_CLR_0_VAL_rg_ncpo_clr(val)               REG_FLD_VAL(PON_INTERRUPT_CLR_0_FLD_rg_ncpo_clr, (val))

#define PON_INTERRUPT_SET_0_VAL_rg_ncpo_det_en(val)            REG_FLD_VAL(PON_INTERRUPT_SET_0_FLD_rg_ncpo_det_en, (val))
#define PON_INTERRUPT_SET_0_VAL_rg_rogue_en(val)               REG_FLD_VAL(PON_INTERRUPT_SET_0_FLD_rg_rogue_en, (val))
#define PON_INTERRUPT_SET_0_VAL_rg_int_los_on_pro(val)         REG_FLD_VAL(PON_INTERRUPT_SET_0_FLD_rg_int_los_on_pro, (val))
#define PON_INTERRUPT_SET_0_VAL_rg_int_los_cnt(val)            REG_FLD_VAL(PON_INTERRUPT_SET_0_FLD_rg_int_los_cnt, (val))

#define PON_INTERRUPT_SET_1_VAL_rg_tx_en_counter(val)          REG_FLD_VAL(PON_INTERRUPT_SET_1_FLD_rg_tx_en_counter, (val))

#ifdef __cplusplus
}
#endif

#endif // __pon_hsgmii_dig_csr_REGS_H__
