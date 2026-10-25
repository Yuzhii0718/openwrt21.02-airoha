#ifndef MULTI_PHY_ANA_HAL_REG
#define MULTI_PHY_ANA_HAL_REG

typedef struct {
uint32 rg_ssusb_bias_pwd :1;
uint32 rg_ssusb_biasimr_pwd :1;
uint32 rg_ssusb_bg_lpf_en :1;
uint32 rg_ssusb_pll_bias_pwd :1;
uint32 rg_ssusb_xtal_rx_pwd :1;
uint32 rg_pcie_ckdet_en :1;
uint32 rg_ssusb_syspll_pwd :1;
uint32 rg_ssusb_scan_mode :1;
uint32 rg_pcie_device :1;
uint32 rg_pcie_clkrx_en :1;
uint32 rg_ssusb_xtal_ext_en :2;
uint32 rg_ssusb_pll_pwd :1;
uint32 rg_ssusb_pll_autok_vco :1;
uint32 rg_ssusb_pll_band :7;
uint32 rg_ssusb_pll_ssc_en :1;
uint32 rg_ssusb_rx_impcalib_en :1;
uint32 rg_pcie_refck_div4 :1;
uint32 rg_ssusb_pll_band_fail :7;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_0, *pHAL_sgmii_reg_phya_0;

typedef struct {
uint32 rg_ssusb_pll_bc :2;
uint32 rg_ssusb_pll_diven :3;
uint32 rg_ssusb_pll_ic :4;
uint32 rg_ssusb_pll_br :2;
uint32 rg_ssusb_pll_ir :4;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_1, *pHAL_sgmii_reg_phya_1;

typedef struct {
uint32 rsv_0 :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_pll_bp :4;
uint32 rg_ssusb_pll_fbksel :2;
uint32 rg_ssusb_rx_leqos_en :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_2, *pHAL_sgmii_reg_phya_2;

typedef struct {
uint32 rg_ssusb_pll_fbkdiv :7;
uint32 rg_ssusb_pll_refckdiv :1;
uint32 rg_ssusb_pll_prediv :2;
uint32 rg_ssusb_pll_bias_lpf_en :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_3, *pHAL_sgmii_reg_phya_3;

typedef struct {
uint32 rg_ssusb_pll_ssc_delta1 :16;
uint32 rg_ssusb_pll_ssc_delta :16;
}HAL_sgmii_reg_phya_4, *pHAL_sgmii_reg_phya_4;

typedef struct {
uint32 rg_force_bias_pwd :1;
uint32 rg_force_biasimr_pwd :1;
uint32 rg_force_bg_lpf_en :1;
uint32 rg_force_pll_bias_pwd :1;
uint32 rg_force_xtal_rx_pwd :1;
uint32 rg_force_syspll_pwd :1;
uint32 rg_force_pll_pwd :1;
uint32 rg_force_pll_autok_vco :1;
uint32 rg_force_pll_band :1;
uint32 rg_force_tx_eidle_cm :1;
uint32 rg_force_syspll_prediv :1;
uint32 rg_force_syspll_reserve :1;
uint32 rg_force_syspll_fbdiv :1;
uint32 rg_force_pll_ssc_prd :1;
uint32 rg_force_pll_ssc_delta1 :1;
uint32 rg_force_pll_ssc_delta :1;
uint32 rg_force_rx_leqos_en :1;
uint32 rg_force_pll_bias_lpf_en :1;
uint32 rg_force_pll_ssc_en :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_5, *pHAL_sgmii_reg_phya_5;

typedef struct {
uint32 rg_force_tx_biasi_en :1;
uint32 rg_force_tx_biasi :1;
uint32 rg_force_idrv_0db :1;
uint32 rg_force_idrv_3p5db :1;
uint32 rg_force_idem_3p5db :1;
uint32 rg_force_idrv_6db :1;
uint32 rg_force_idem_6db :1;
uint32 rg_force_tx_data_en :1;
uint32 rg_force_tx_eidle_lp_en :1;
uint32 rg_force_tx_ser_en :1;
uint32 rg_force_sigdet_en :1;
uint32 rg_force_sigdet_cal_en :1;
uint32 rg_force_sigdet_cal_offset :1;
uint32 rg_force_rx_pi_cal_en :1;
uint32 rg_force_tx_drv_en :1;
uint32 rg_force_tx_lfps :1;
uint32 rg_force_tx_lfps_en :1;
uint32 rg_force_rx_saosc_en :1;
uint32 rg_force_cdr_pi0 :1;
uint32 rg_force_cdr_br :1;
uint32 rg_force_cdr_bic :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_6, *pHAL_sgmii_reg_phya_6;

typedef struct {
uint32 rg_force_cdr_kvsel :1;
uint32 rg_force_cdr_bir :1;
uint32 rg_force_cdr_bc :1;
uint32 rg_force_cdr_pwd :1;
uint32 rg_force_cdr_autok_vco :1;
uint32 rg_force_cdr_lck2ref :1;
uint32 rg_force_cdr_pi90 :1;
uint32 rg_force_cdr_band :1;
uint32 rg_force_rx_cmpwd :1;
uint32 rg_force_rx_afe_pwd :1;
uint32 rg_force_tx_imp_sel :1;
uint32 rg_force_rx_imp_sel :1;
uint32 rg_force_iext_intr_ctrl :1;
uint32 rg_force_syspll_sdm_pwr_on :1;
uint32 rg_force_syspll_sdm_iso_en :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_7, *pHAL_sgmii_reg_phya_7;

typedef struct {
uint32 rg_ssusb_cdr_bicltd0 :4;
uint32 rg_ssusb_cdr_bicltd1 :4;
uint32 rg_ssusb_cdr_bicltr :4;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_8, *pHAL_sgmii_reg_phya_8;

typedef struct {
uint32 rg_ssusb_tx_biasi_en :1;
uint32 rg_ssusb_tx_biasi :3;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_ssusb_idrv_0db :6;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_idrv_3p5db :6;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_ssusb_idem_3p5db :6;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_9, *pHAL_sgmii_reg_phya_9;

typedef struct {
uint32 rg_ssusb_idrv_6db :6;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_ssusb_idem_6db :6;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_rxdet_en :1;
uint32 rg_ssusb_tx_data_en :1;
uint32 rg_ssusb_tx_eidle_lp_en :1;
uint32 rg_ssusb_tx_ser_en :1;
uint32 rg_pcie_sigdet_en :1;
uint32 rg_pcie_sigdet_cal_en :1;
uint32 rg_pcie_sigdet_cal_offset :5;
uint32 rg_ssusb_rx_pi_cal_en :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_10, *pHAL_sgmii_reg_phya_10;

typedef struct {
uint32 rg_tphy_mode :2;
uint32 rg_tphy_speed :2;
uint32 rg_ssusb_tx_drv_en :1;
uint32 rg_ssusb_tx_lfps_en :1;
uint32 rg_ssusb_tx_impcalib_en :1;
uint32 rg_ssusb_tx_imp_sel :5;
uint32 rg_ssusb_tx_lfps :1;
uint32 rg_ssusb_tx_dem_g1 :2;
uint32 rg_ssusb_rx_saosc_en :1;
uint32 rg_ssusb_tx_dem_g2 :2;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_ssusb_cdr_pi0 :7;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_11, *pHAL_sgmii_reg_phya_11;

typedef struct {
uint32 rg_ssusb_cdr_bpa :2;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_ssusb_cdr_bpb :3;
uint32 rsv_7 :1;
uint32 rg_ssusb_cdr_brltr :3;
uint32 rsv_11 :1;
uint32 rg_ssusb_cdr_brltd0 :3;
uint32 rsv_15 :1;
uint32 rg_ssusb_cdr_brltd1 :3;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_12, *pHAL_sgmii_reg_phya_12;

typedef struct {
uint32 rg_ssusb_cdr_fbdiv :7;
uint32 rg_ssusb_cdr_kvselltr :1;
uint32 rg_ssusb_cdr_kvselltd0 :1;
uint32 rg_ssusb_cdr_kvselltd1 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_13, *pHAL_sgmii_reg_phya_13;

typedef struct {
uint32 rg_ssusb_cdr_birltr :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_ssusb_cdr_birltd0 :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_cdr_birltd1 :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_ssusb_cdr_bcltr :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_14, *pHAL_sgmii_reg_phya_14;

typedef struct {
uint32 rg_ssusb_cdr_bcltd0 :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_ssusb_cdr_bcltd1 :5;
uint32 rg_ssusb_cdr_pwd :1;
uint32 rg_ssusb_cdr_pi_pwd :1;
uint32 rg_ssusb_cdr_pi_mode :1;
uint32 rg_ssusb_cdr_tcadj :6;
uint32 rg_ssusb_cdr_autok_vco :1;
uint32 rg_ssusb_cdr_lck2ref :1;
uint32 rg_ssusb_cdr_pi90 :7;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_15, *pHAL_sgmii_reg_phya_15;

typedef struct {
uint32 rg_ssusb_cdr_band :7;
uint32 rg_ssusb_rx_p1_entry :1;
uint32 rg_ssusb_lfps_deglitch :2;
uint32 rg_ssusb_eq_rstep1 :2;
uint32 rg_ssusb_eq_rstep2 :2;
uint32 rg_ssusb_lfps_pwd :1;
uint32 rg_ssusb_rx_cmpwd :1;
uint32 rg_ssusb_cdr_band_fail :7;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_16, *pHAL_sgmii_reg_phya_16;

typedef struct {
uint32 rg_ssusb_rx_afe_pwd :1;
uint32 rg_ssusb_rx_hz :1;
uint32 rg_ssusb_rx_p3_entry :1;
uint32 rsv_3 :1;
uint32 rg_ssusb_rx_imp_sel :5;
uint32 rg_pcie_clkdrv_offset :2;
uint32 rg_pcie_clkdrv_slew :2;
uint32 rg_pcie_clkdrv_amp :3;
uint32 rg_pcie_clktx_en :1;
uint32 rg_pcie_crsdet_vth :2;
uint32 rg_pcie_crsdet_en :1;
uint32 rg_pcie_crsdet_rstb :1;
uint32 rg_pcie_crsdet_ofst :4;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_17, *pHAL_sgmii_reg_phya_17;

typedef struct {
uint32 rg_ssusb_ext_bias_sel :1;
uint32 rg_ssusb_int_bias_sel :1;
uint32 rg_ssusb_bg_monen :1;
uint32 rg_ssusb_ref_en :1;
uint32 rg_ssusb_vrt_vref_sel :4;
uint32 rg_ssusb_iext_intr_ctrl :6;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_mpx_sel :8;
uint32 rg_ssusb_mpx_en :1;
uint32 rg_ssusb_intr_en :1;
uint32 rg_ssusb_vrt_25m_en :1;
uint32 rg_ssusb_bg_div :2;
uint32 rg_ssusb_chpen :1;
uint32 rg_ssusb_bgr_en :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_18, *pHAL_sgmii_reg_phya_18;

typedef struct {
uint32 rg_ssusb_syspll_bp :1;
uint32 rg_ssusb_syspll_blp :1;
uint32 rg_ssusb_syspll_rst_dly :2;
uint32 rg_ssusb_syspll_vod_en :1;
uint32 rg_ssusb_syspll_vco_div_sel :1;
uint32 rg_ssusb_syspll_posdiv :2;
uint32 rg_ssusb_syspll_prediv :2;
uint32 rg_ssusb_xtal_top_reserve :16;
uint32 rg_ssusb_xtal_hys :1;
uint32 rg_ssusb_xtal_mon_en :1;
uint32 rg_ssusb_xtal_tst_a2dck_en :1;
uint32 rg_vusb10_on :1;
uint32 rg_pcie_clkdet_vth :2;
}HAL_sgmii_reg_phya_19, *pHAL_sgmii_reg_phya_19;

typedef struct {
uint32 rg_ssusb_syspll_fbdiv_chg :1;
uint32 rg_ssusb_syspll_sdm_iso_en :1;
uint32 rg_ssusb_syspll_sdm_pwr_on :1;
uint32 rg_ssusb_syspll_sdm_out :1;
uint32 rg_ssusb_syspll_sdm_ifm :1;
uint32 rg_ssusb_syspll_monref_en :1;
uint32 rg_ssusb_syspll_monvc_en :1;
uint32 rg_ssusb_syspll_monck_en :1;
uint32 rg_ssusb_syspll_bc :1;
uint32 rg_ssusb_syspll_br :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_20, *pHAL_sgmii_reg_phya_20;

typedef struct {
uint32 rg_ssusb_syspll_hr_en :1;
uint32 rg_ssusb_syspll_fbdiv :31;
}HAL_sgmii_reg_phya_21, *pHAL_sgmii_reg_phya_21;

typedef struct {
uint32 rg_ssusb_pll_posdiv :2;
uint32 rg_ssusb_pll_rst_dly :2;
uint32 rg_ssusb_pll_autok_kf :2;
uint32 rg_ssusb_pll_autok_ks :2;
uint32 rg_ssusb_pll_rlh_en :1;
uint32 rg_ssusb_pll_monvc_en :1;
uint32 rg_ssusb_pll_monck_en :1;
uint32 rg_ssusb_pll_monref_en :1;
uint32 rg_ssusb_pll_vod_en :1;
uint32 rg_ssusb_pll_autok_load :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_syspll_reserve :8;
uint32 rg_ssusb_syspll_sdm_mode :2;
uint32 rg_ssusb_syspll_sdm_ord :2;
uint32 rg_ssusb_syspll_sdm_di_ls :2;
uint32 rg_ssusb_syspll_sdm_di_en :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_22, *pHAL_sgmii_reg_phya_22;

typedef struct {
uint32 rg_ssusb_pll_ssc_prd :16;
uint32 rg_ssusb_pll_reserve :16;
}HAL_sgmii_reg_phya_23, *pHAL_sgmii_reg_phya_23;

typedef struct {
uint32 rg_ssusb_ln0_lnmpx_sel :4;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_pcie_ln0_rx_reserve :8;
uint32 rg_ssusb_ln0_rxafe_reserve :8;
uint32 rg_ssusb_ln0_cdr_reserve :8;
}HAL_sgmii_reg_phya_24, *pHAL_sgmii_reg_phya_24;

typedef struct {
uint32 rg_ssusb_ln0_ckmon_sel :4;
uint32 rg_ssusb_ln0_tx_vlmon_sel :2;
uint32 rg_ssusb_ln0_tx_vlmon_en :1;
uint32 rg_ssusb_ln0_ckmon_en :1;
uint32 rg_ssusb_ln0_rxdet_vthsel_h :2;
uint32 rg_ssusb_ln0_rxdet_vthsel_l :2;
uint32 rg_ssusb_ln0_rxdet_updn_sel :1;
uint32 rg_ssusb_ln0_rxdet_updn_force :1;
uint32 rg_ssusb_ln0_rxdet_rsel :2;
uint32 rg_ssusb_ln0_tx_sr :3;
uint32 rg_ssusb_ln0_tx_r50_fon :1;
uint32 rg_ssusb_ln0_idem_bias :4;
uint32 rg_ssusb_ln0_idrv_calib :6;
uint32 rg_ssusb_ln0_idrv_itailop_en :1;
uint32 rg_ssusb_ln0_tx250mck_invb :1;
}HAL_sgmii_reg_phya_25, *pHAL_sgmii_reg_phya_25;

typedef struct {
uint32 rg_ssusb_ln0_cdr_epen :1;
uint32 rg_ssusb_ln0_cdr_pi_slew :2;
uint32 rg_ssusb_ln0_cdr_bypass :2;
uint32 rg_ssusb_ln0_cdr_rstb_manual :1;
uint32 rg_ssusb_ln0_cdr_rst_dly :2;
uint32 rg_ssusb_ln0_cdr_monmux :1;
uint32 rg_ssusb_ln0_cdr_oscdet_en :1;
uint32 rg_ssusb_ln0_cdr_cpbias_sel :1;
uint32 rg_ssusb_ln0_cdr_pd_div_sel :1;
uint32 rg_ssusb_ln0_cdr_pd_div_bypass :1;
uint32 rg_ssusb_ln0_txlbrx_en :1;
uint32 rg_ssusb_ln0_rxlbtx_en :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_ln0_tx_eidle_cm :4;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_26, *pHAL_sgmii_reg_phya_26;

typedef struct {
uint32 rg_pcie_ln0_sigdet_vth :2;
uint32 rg_ssusb_ln0_rx_sw_ctrl :4;
uint32 rg_ssusb_ln0_rx_50_tar :2;
uint32 rg_ssusb_ln0_rx_50_lower :3;
uint32 rg_ssusb_ln0_rx_t2r_en :1;
uint32 rg_ssusb_ln0_rx_r2t_en :1;
uint32 rg_ssusb_ln0_eq_cursel :1;
uint32 rg_ssusb_ln0_rx_dac_pwd :1;
uint32 rg_ssusb_ln0_rx_dac_en :1;
uint32 rg_ssusb_ln0_rx_dac_mux :5;
uint32 rg_ssusb_ln0_cdr_autok_kf :2;
uint32 rg_ssusb_ln0_cdr_autok_ks :2;
uint32 rg_ssusb_ln0_cdr_regod :2;
uint32 rsv_27 :1;
uint32 rg_ssusb_ln0_cdr_monen_dig :1;
uint32 rg_ssusb_ln0_cdr_monen :1;
uint32 rg_ssusb_ln0_cdr_autok_load :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_27, *pHAL_sgmii_reg_phya_27;

typedef struct {
uint32 rg_ssusb_ln0_rx_dac_meas_en :1;
uint32 rg_ssusb_ln0_rx_pd_pical_swap :1;
uint32 rg_ssusb_ln0_rx_pd_rst_pass :1;
uint32 rg_ssusb_ln0_rx_pd_rst :1;
uint32 rg_ssusb_ln0_rx_p1_entry_pass :1;
uint32 rg_ssusb_ln0_rxafe_dcmon_sel :4;
uint32 rg_ssusb_ln0_lfps_mon_en :1;
uint32 rg_ssusb_ln0_lfps_lpf :2;
uint32 rg_ssusb_ln0_lfps_slcout_sel :1;
uint32 rg_ssusb_ln0_lfps_cmpout_sel :1;
uint32 rg_pcie_ln0_sigdet_hf :2;
uint32 rg_pcie_ln0_sigdet_lpf :2;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_28, *pHAL_sgmii_reg_phya_28;

typedef struct {
uint32 rg_ssusb_ln0_reserve :16;
uint32 rg_serdes_debug_sel :8;
uint32 rg_serdes_debug_en :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_29, *pHAL_sgmii_reg_phya_29;

typedef struct {
uint32 rg_sgmii_unidir_ena :4;
uint32 rg_sgmii_speed_msb :1;
uint32 rg_sgmii_col_test :1;
uint32 rg_sgmii_duplex :1;
uint32 rg_sgmii_an_restart :1;
uint32 rg_sgmii_isolate :1;
uint32 rg_sgmii_powerdown :1;
uint32 rg_sgmii_an_enable :1;
uint32 rg_sgmii_speed_lsb :1;
uint32 rg_sgmii_loopback_ena :1;
uint32 rg_sgmii_remote_fault :1;
uint32 rg_sgmii_an_expansion_clr :1;
uint32 rg_sgmii_link_rst :1;
uint32 rg_sgmii_dev_ability_0 :1;
uint32 rg_sgmii_dev_ability_1 :1;
uint32 rg_sgmii_dev_ability_2 :1;
uint32 rg_sgmii_dev_ability_3 :1;
uint32 rg_sgmii_dev_ability_13_4 :10;
uint32 rg_sgmii_dev_ability_15 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_30, *pHAL_sgmii_reg_phya_30;

typedef struct {
uint32 rg_sgmii_np_tx_2_0 :3;
uint32 rg_sgmii_np_tx_7_3 :5;
uint32 rg_sgmii_np_tx_15_8 :8;
uint32 rg_sgmii_scratch :16;
}HAL_sgmii_reg_phya_31, *pHAL_sgmii_reg_phya_31;

typedef struct {
uint32 rg_sgmii_link_timer :20;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_32, *pHAL_sgmii_reg_phya_32;

typedef struct {
uint32 rg_sgmii_if_mode_5_0 :6;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_sgmii_remote_fault_dis :1;
uint32 rg_sgmii_code_sync_set_val :1;
uint32 rg_sgmii_code_sync_set_en :1;
uint32 rg_sgmii_send_an_error_en :1;
uint32 rg_sgmii_an_transparent :1;
uint32 rg_sgmii_an_block_ack :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_sgmii_txbuf_full_thr :4;
uint32 rg_sgmii_txbuf_empty_thr :4;
uint32 rg_sgmii_rxbuf_full_thr :4;
uint32 rg_sgmii_rxbuf_empty_thr :4;
}HAL_sgmii_reg_phya_33, *pHAL_sgmii_reg_phya_33;

typedef struct {
uint32 rg_sgmii_reserved_reg :32;
}HAL_sgmii_reg_phya_34, *pHAL_sgmii_reg_phya_34;

typedef struct {
uint32 rg_sgmii_ela_mac_txck_sel :4;
uint32 rg_sgmii_ela_mac_rxck_sel :4;
uint32 rg_sgmii_ela_pma_txck_sel :4;
uint32 rg_sgmii_ela_pma_rxck_sel :4;
uint32 rg_sgmii_ela_csr_sel :4;
uint32 rg_sgmii_err_prop_ki_sig_los_r :1;
uint32 rg_sgmii_err_prop_ki_sync_f :1;
uint32 rg_sgmii_err_prop_ko_sig_los_f :1;
uint32 rg_sgmii_err_prop_ko_sync_r :1;
uint32 rg_sgmii_1us_timer :8;
}HAL_sgmii_reg_phya_35, *pHAL_sgmii_reg_phya_35;

typedef struct {
uint32 rg_sgmii_sw_reset :1;
uint32 rg_sgmii_tx_fault_latch_clr :1;
uint32 rg_sgmii_rx_fault_latch_clr :1;
uint32 rg_sgmii_pat_gen_tx_en :1;
uint32 rg_sgmii_pcs_mode :3;
uint32 rg_sgmii_pcs_dec_error_cnt_read_latch :1;
uint32 rg_sgmii_false_carrier_cnt_read_latch :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_36, *pHAL_sgmii_reg_phya_36;

typedef struct {
uint32 rg_bias_pwd_to_bg_lpf_en_dly :5;
uint32 rg_syspll_pwd_to_syspll_ready_dly :4;
uint32 rg_pll_autok_vco_to_pll_bias_lpf_en_dly :2;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_eidle_lp_en_ne_to_data_en_pe_dly_g2 :8;
uint32 rg_eidle_lp_en_ne_to_data_en_pe_dly_g1 :8;
}HAL_sgmii_reg_phya_37, *pHAL_sgmii_reg_phya_37;

typedef struct {
uint32 rg_rx_pi_cal_en_h_dly :5;
uint32 rg_rx_saosc_en_h_dly :6;
uint32 rg_cdr_bw_sel_dly :4;
uint32 rsv_15 :1;
uint32 rg_freq_det_en_wait_dly :4;
uint32 rg_cdr_settle_dly :11;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_38, *pHAL_sgmii_reg_phya_38;

typedef struct {
uint32 rg_sig_det_wait_dly :16;
uint32 rg_rx_eq_en_h_dly :13;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_39, *pHAL_sgmii_reg_phya_39;

typedef struct {
uint32 rg_sig_det_h_thres :10;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_sig_det_l_thres :10;
uint32 rg_sig_det_cr :1;
uint32 rg_frc_sig_det :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_40, *pHAL_sgmii_reg_phya_40;

typedef struct {
uint32 rg_ppm_thres :6;
uint32 rg_freq_det_frc :1;
uint32 rg_freq_det_frc_en :1;
uint32 rg_ssusb_eq_dleq_lfi_gen3 :4;
uint32 rg_ssusb_eq_dleq_lfi_gen4 :4;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_41, *pHAL_sgmii_reg_phya_41;

typedef struct {
uint32 rg_ssusb_eq_dd0los_lfi :7;
uint32 rsv_7 :1;
uint32 rg_ssusb_eq_dd0hos_lfi :7;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_dhhl_lfi :7;
uint32 rsv_23 :1;
uint32 rg_ssusb_eq_dlhl_lfi :7;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_42, *pHAL_sgmii_reg_phya_42;

typedef struct {
uint32 rg_ssusb_eq_dd1los_lfi :7;
uint32 rsv_7 :1;
uint32 rg_ssusb_eq_dd1hos_lfi :7;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_de0os_lfi :7;
uint32 rsv_23 :1;
uint32 rg_ssusb_eq_de1os_lfi :7;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_43, *pHAL_sgmii_reg_phya_43;

typedef struct {
uint32 rg_ssusb_eq_dlhl_lf_sel :3;
uint32 rg_ssusb_eq_dleq_lf_sel :3;
uint32 rg_ssusb_eq_starttime :2;
uint32 rg_ssusb_eq_dsaos_lf_sel :3;
uint32 rg_ssusb_eq_dhhl_lf_sel :3;
uint32 rg_ssusb_eq_stoptime :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_dhhlos_lfi :7;
uint32 rsv_23 :1;
uint32 rg_ssusb_eq_dlhlos_lfi :7;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_44, *pHAL_sgmii_reg_phya_44;

typedef struct {
uint32 rg_ssusb_eq_tri_det_th :7;
uint32 rg_ssusb_eq_tri_det_en :1;
uint32 rg_ssusb_eq_deye1os_lfi :7;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_deye0os_lfi :7;
uint32 rsv_23 :1;
uint32 rg_ssusb_eq_dleq_lfi_gen1 :4;
uint32 rg_ssusb_eq_dleq_lfi_gen2 :4;
}HAL_sgmii_reg_phya_45, *pHAL_sgmii_reg_phya_45;

typedef struct {
uint32 rg_ssusb_eq_eye_cnt_en :1;
uint32 rg_ssusb_eq_pi_kpgain :3;
uint32 rg_ssusb_eq_pilpo_rout :1;
uint32 rg_ssusb_eq_eye1_y :7;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_eye0_y :7;
uint32 rsv_23 :1;
uint32 rg_ssusb_eq_eye_mon_en :1;
uint32 rg_ssusb_eq_eye_xoffset :7;
}HAL_sgmii_reg_phya_46, *pHAL_sgmii_reg_phya_46;

typedef struct {
uint32 rg_ssusb_eq_eye_mask :10;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_sigdet :7;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_47, *pHAL_sgmii_reg_phya_47;

typedef struct {
uint32 rg_ssusb_eq_sd_cnt0 :6;
uint32 rg_ssusb_eq_isiflag_sel :2;
uint32 rg_ssusb_eq_sd_cnt1 :6;
uint32 rg_ssusb_eq_rx500m_ck_sel :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_48, *pHAL_sgmii_reg_phya_48;

typedef struct {
uint32 rg_ssusb_eq_pi0_ini :7;
uint32 rsv_7 :1;
uint32 rg_ssusb_eq_pi90_ini :7;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_pieye_ini :7;
uint32 rg_ssusb_eq_chk_eye_h :1;
uint32 rg_ssusb_eq_dfex_lf_sel :3;
uint32 rg_ssusb_eq_dfex_en :1;
uint32 rg_ssusb_eq_leqmax :4;
}HAL_sgmii_reg_phya_49, *pHAL_sgmii_reg_phya_49;

typedef struct {
uint32 rg_ssusb_eq_dfexen_sel :1;
uint32 rg_ssusb_eq_dfeyen_stop_dis :1;
uint32 rg_ssusb_eq_dfex_dis :1;
uint32 rsv_3 :1;
uint32 rg_ssusb_eq_pi90ck_sel :2;
uint32 rg_ssusb_eq_gated_rxd_b :1;
uint32 rg_ssusb_eq_dfex_rst :1;
uint32 rg_ssusb_eq_dfexen_dur :3;
uint32 rsv_11 :1;
uint32 rg_ssusb_eq_dfeyen_dur :3;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_rev :16;
}HAL_sgmii_reg_phya_50, *pHAL_sgmii_reg_phya_50;

typedef struct {
uint32 rg_ssusb_eq_leq_stop_to :2;
uint32 rg_ssusb_eq_dfe_tog :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_ssusb_eq_dleqos_lfi :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_ssusb_eq_leqosc_dlycnt :3;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_ssusb_eq_mon_sel :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_51, *pHAL_sgmii_reg_phya_51;

typedef struct {
uint32 rg_ssusb_eq_reserved :32;
}HAL_sgmii_reg_phya_52, *pHAL_sgmii_reg_phya_52;

typedef struct {
uint32 rg_ssusb_eq_rev_1 :16;
uint32 rg_ssusb_eq_algrthm_sel :1;
uint32 rg_ssusb_eq_probe_out_sel :1;
uint32 rg_ssusb_rx_fr_mode :1;
uint32 rg_ssusb_force_rx_fr_mode :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_53, *pHAL_sgmii_reg_phya_53;

typedef struct {
uint32 rg_ssusb_eq_eye_cnt :20;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_ssusb_eq_leq_shift :3;
uint32 rsv_27 :1;
uint32 rg_ssusb_rx_dfe_rst :1;
uint32 rg_ssusb_rx_dfe_en :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_54, *pHAL_sgmii_reg_phya_54;

typedef struct {
uint32 rg_force_rx_cdr_stable :1;
uint32 rg_force_rx_saosc_rst :1;
uint32 rg_force_rx_eq_rst :1;
uint32 rg_force_rx_eq_en :1;
uint32 rg_ssusb_rx_cdr_stable :1;
uint32 rg_ssusb_rx_saosc_rst :1;
uint32 rg_ssusb_rx_eq_rst :1;
uint32 rg_ssusb_rx_eq_en :1;
uint32 rg_ssusb_rx_sigdet_en :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_55, *pHAL_sgmii_reg_phya_55;

typedef struct {
uint32 rg_prbs_en :1;
uint32 rg_prbs_check :1;
uint32 rg_prbs_mode :2;
uint32 rg_renew_mode :1;
uint32 rg_prbs_injerr :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_udp_data_15_0 :16;
}HAL_sgmii_reg_phya_56, *pHAL_sgmii_reg_phya_56;

typedef struct {
uint32 rg_udp_data_47_16 :32;
}HAL_sgmii_reg_phya_57, *pHAL_sgmii_reg_phya_57;

typedef struct {
uint32 rg_udp_data_79_48 :32;
}HAL_sgmii_reg_phya_58, *pHAL_sgmii_reg_phya_58;

typedef struct {
uint32 rg_sw_rst_ctrl :11;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_59, *pHAL_sgmii_reg_phya_59;

typedef struct {
uint32 rg_afifo_w_thres_serdes :5;
uint32 rg_afifo_r_thres_serdes :5;
uint32 rg_afifo_winc_serdes :1;
uint32 rg_afifo_rinc_serdes :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_afifo_w_thres_pcs :5;
uint32 rg_afifo_r_thres_pcs :5;
uint32 rg_afifo_wclear_pcs :1;
uint32 rg_afifo_rclear_pcs :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_60, *pHAL_sgmii_reg_phya_60;

typedef struct {
uint32 rg_speed_chg :1;
uint32 rg_cdr_re_calib :1;
uint32 rg_tx250m_ck_en :1;
uint32 rg_rx250m_ck_en :1;
uint32 rg_cdr_refclk_en :1;
uint32 rg_cdr_fbclk_en :1;
uint32 rg_sigdet_wait_force :1;
uint32 rg_sigdet_wait_cond :1;
uint32 rg_lpf_window_sel :2;
uint32 rg_phya_pwd :1;
uint32 rg_tx_idle :1;
uint32 rg_tx_idle_mode :1;
uint32 rg_tx_disable :1;
uint32 rg_rx_pwd :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_61, *pHAL_sgmii_reg_phya_61;

typedef struct {
uint32 rg_tx_bit_polarity :1;
uint32 rg_rx_bit_polarity :1;
uint32 rg_pcs_t2r_loopback :1;
uint32 rg_qphy_r2t_loopback :1;
uint32 rg_pcs_r2t_loopback :1;
uint32 rg_tbi_tx_disable :1;
uint32 rg_tbi_rx_disable :1;
uint32 rg_frc_rx_path_rdy :1;
uint32 rg_rx_path_rdy_cr :1;
uint32 rg_frc_tx_path_rdy :1;
uint32 rg_tx_path_rdy_cr :1;
uint32 rg_qphy_r2t_loopback_fifo_rst :1;
uint32 rg_lsb8bit_sel :3;
uint32 rsv_15 :1;
uint32 rg_msb8bit_sel :3;
uint32 rg_probe_out_clk_en :1;
uint32 rg_xtal_sel :1;
uint32 rg_tx_div4_phase_sel :2;
uint32 rg_tx_div2_phase_sel :1;
uint32 rg_rx_div2_phase_sel :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_62, *pHAL_sgmii_reg_phya_62;

typedef struct {
uint32 rg_sgmii_tx_imp_sel :5;
uint32 rg_sgmii_rx_imp_sel :5;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_sgmii_iext_intr_ctrl :6;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_63, *pHAL_sgmii_reg_phya_63;

typedef struct {
uint32 rg_ssusb_phyrst_b :1;
uint32 rg_ssusb_rosc_fine_sel :4;
uint32 rg_ssusb_rosc_coarse_sel :4;
uint32 rg_pcie_clkdrv_force :2;
uint32 rg_pcie_clkdrv_impsel :5;
uint32 rg_pcie_clkdrv_hz :1;
uint32 rg_ssusb_rosc_en :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_64, *pHAL_sgmii_reg_phya_64;

typedef struct {
uint32 rg_tbi_20_tx_reverse_pma :1;
uint32 rg_tbi_tx_reverse_10b_pma :1;
uint32 rg_tbi_20_rx_reverse_pma :1;
uint32 rg_tbi_10_rx_reverse_pma :1;
uint32 rg_tbi_20_rx_reverse_all :1;
uint32 rg_tbi_20_tx_reverse_all :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_ana_sgmii_reg_phya_65, *pHAL_ana_sgmii_reg_phya_65;

typedef struct {
uint32 rg_bist_pat_sel :5;
uint32 rg_bist_pat_tx_en :1;
uint32 rg_bist_pat_rx_check_en :1;
uint32 rg_bist_io_data_inv :2;
uint32 rg_bist_program_reach :4;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_66, *pHAL_sgmii_reg_phya_66;

typedef struct {
uint32 rg_bist_align_pattern :32;
}HAL_sgmii_reg_phya_67, *pHAL_sgmii_reg_phya_67;

typedef struct {
uint32 rg_bist_program_data_0 :32;
}HAL_sgmii_reg_phya_68, *pHAL_sgmii_reg_phya_68;

typedef struct {
uint32 rg_bist_program_data_1 :32;
}HAL_sgmii_reg_phya_69, *pHAL_sgmii_reg_phya_69;

typedef struct {
uint32 rg_bist_program_data_2 :32;
}HAL_sgmii_reg_phya_70, *pHAL_sgmii_reg_phya_70;

typedef struct {
uint32 rg_bist_program_data_3 :32;
}HAL_sgmii_reg_phya_71, *pHAL_sgmii_reg_phya_71;

typedef struct {
uint32 rg_bist_program_data_4 :32;
}HAL_sgmii_reg_phya_72, *pHAL_sgmii_reg_phya_72;

typedef struct {
uint32 rg_bist_program_data_5 :32;
}HAL_sgmii_reg_phya_73, *pHAL_sgmii_reg_phya_73;

typedef struct {
uint32 rg_bist_program_data_6 :32;
}HAL_sgmii_reg_phya_74, *pHAL_sgmii_reg_phya_74;

typedef struct {
uint32 rg_bist_program_data_7 :32;
}HAL_sgmii_reg_phya_75, *pHAL_sgmii_reg_phya_75;

typedef struct {
uint32 rg_bist_prbs_init_seed :31;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_76, *pHAL_sgmii_reg_phya_76;

typedef struct {
uint32 rg_bist_prbs_fail_threshold :16;
uint32 rg_bist_prbs_errcnt_read_latch :1;
uint32 rg_bist_tx_data_pollution_latch :1;
uint32 rg_bist_prbs_match_times :4;
uint32 rg_bist_prbs_align_times :4;
uint32 rg_bist_tx_rate_ctrl :3;
uint32 rg_bist_rx_rate_ctrl :3;
}HAL_sgmii_reg_phya_77, *pHAL_sgmii_reg_phya_77;

typedef struct {
uint32 rg_bist_tx_data_pollution :32;
}HAL_sgmii_reg_phya_78, *pHAL_sgmii_reg_phya_78;

typedef struct {
uint32 rg_bist_prbs_check_timer :24;
uint32 rg_bist_bit_error_rst_sel :1;
uint32 rg_tx_rx_max_sel :4;
uint32 rg_bist_rx_data_rden :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_79, *pHAL_sgmii_reg_phya_79;

typedef struct {
uint32 rg_xpon_rx_rate_ctrl :2;
uint32 rg_xpon_tx_rate_ctrl :2;
uint32 rg_sgmii_100m_10m :2;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_80, *pHAL_sgmii_reg_phya_80;

typedef struct {
uint32 rg_force_bg_tx_mode :1;
uint32 rg_force_bg_tx_value :1;
uint32 rg_tx_250m_ck_en :1;
uint32 rg_rx_250m_ck_en :1;
uint32 rg_tx_clk_inv :1;
uint32 rg_rx_clk_inv :1;
uint32 rg_force_hg_mode :1;
uint32 rg_force_map_mode :1;
uint32 rg_force_map_sel :1;
uint32 rg_force_hg_txpcsmodesel :1;
uint32 rg_force_hg_rxpcsmodesel :1;
uint32 rg_force_hg_txmapmodesel :1;
uint32 rg_bridge_tx_reset_n :1;
uint32 rg_tx250m_short_reset_n :1;
uint32 rg_tx250m_div_reset_n :1;
uint32 rg_tx_reset_n :1;
uint32 rg_rx_reset_n :1;
uint32 rg_pcstx_reset_n :1;
uint32 rg_pcsrx_reset_n :1;
uint32 rg_pcstxrx_reset_n :1;
uint32 rg_mactx_reset_n :1;
uint32 rg_macrx_reset_n :1;
uint32 rg_mactxrx_reset_n :1;
uint32 rg_eq_rx250m_reset_n :1;
uint32 rg_eq_rx250m_div_reset_n :1;
uint32 rg_ref_reset_n :1;
uint32 rg_tx_250m_ck_en_force :1;
uint32 rg_rx_250m_ck_en_force :1;
uint32 rg_xtal_sel_20m_25m :1;
uint32 rg_tx_short_clk_inv :1;
uint32 rg_tx_clk_source_sel :1;
uint32 rg_rx_short_clk_inv :1;
}HAL_sgmii_reg_phya_81, *pHAL_sgmii_reg_phya_81;

typedef struct {
uint32 rg_pma_rx_ck_reset_n :1;
uint32 rg_pma_rx_div2_ck_reset_n :1;
uint32 rg_pcs_rx_ck_reset_n :1;
uint32 rg_o_mac_rx_div_ck_reset_n :1;
uint32 rg_pma_tx_ck_reset_n :1;
uint32 rg_pma_tx_div2_ck_reset_n :1;
uint32 rg_pcs_tx_ck_reset_n :1;
uint32 rg_o_mac_tx_div_ck_reset_n :1;
uint32 rg_100fx_rx_ck_reset_n :1;
uint32 rg_100fx_tx_ck_reset_n :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_rate_adapt_fifo_rx_reset_n :1;
uint32 rg_rate_adapt_fifo_tx_reset_n :1;
uint32 rg_rate_adapt_fifo_rxpma_reset_n :1;
uint32 rg_hsgmii_fifo_rx_reset_n :1;
uint32 rg_eq_rx_reset_n :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_phya_82, *pHAL_sgmii_reg_phya_82;

typedef struct {
uint32 rg_hsgmii_pcs_int :1;
uint32 rg_100fx_int :1;
uint32 rg_symbol_error_int :1;
uint32 rg_false_carrier_int :1;
uint32 rg_rxer100_int :1;
uint32 rg_dwn_sft_trip_up_int :1;
uint32 rg_dwn_sft_trip_dn_int :1;
uint32 rsv_7 :1;
uint32 rg_hsgmii_phy_int_force :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_interrupt_sel, *pHAL_sgmii_reg_interrupt_sel;

typedef struct {
uint32 rg_hsgmii_probe_sel :1;
uint32 rg_100fx_probe_sel :1;
uint32 rg_hsgmii_clk_probe_sel :1;
uint32 rg_dwn_shift_probe_sel :1;
uint32 rg_hsgmii_probe_port_sel :2;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_probe_clk_sel :16;
}HAL_sgmii_reg_probe_sel, *pHAL_sgmii_reg_probe_sel;

typedef struct {
uint32 rg_hwtrap_100fx_en :2;
uint32 rg_xilinx_cdr_set :2;
uint32 rg_fpga_sig_det_ctrl :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_fpga_mode_control, *pHAL_sgmii_fpga_mode_control;

typedef struct {
uint32 rg_hsgmii_async_fifo_wclear :1;
uint32 rg_hsgmii_async_fifo_rclear :1;
uint32 rg_hsgmii_async_fifo_r_threshold :5;
uint32 rg_hsgmii_async_fifo_w_threshold :5;
uint32 rg_hsgmii_async_fifo_full_cnt_clear :1;
uint32 rg_hsgmii_async_fifo_afull_cnt_clear :1;
uint32 rg_hsgmii_async_fifo_empty_cnt_clear :1;
uint32 rg_hsgmii_async_fifo_aempty_cnt_clear :1;
uint32 rg_hsgmii_async_fifo_read_enable_and :1;
uint32 rg_hsgmii_async_fifo_write_enable_and :1;
uint32 rg_hsgmii_async_fifo_read_enable_or :1;
uint32 rg_hsgmii_async_fifo_write_enable_or :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_async_fifo_control, *pHAL_sgmii_async_fifo_control;

typedef struct {
uint32 rg_dwn_shift_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_dwn_shift_sw_rst :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_dwn_shift_thold :4;
uint32 rg_dwn_shift_trig_up_int_clr :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_dwn_shift_trig_dn_int_clr :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_dwn_shift_ctrl, *pHAL_sgmii_reg_dwn_shift_ctrl;

typedef struct {
uint32 rg_shorten_timer_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_time_base_sel :2;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_dwn_shift_timer_1g :8;
uint32 rg_dwn_shift_timer_100m :8;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_reg_dwn_shift_timer_ctrl, *pHAL_sgmii_reg_dwn_shift_timer_ctrl;

typedef struct {
uint32 ro_dummy_31_0 :32;
}HAL_sgmii_ro_phya_0, *pHAL_sgmii_ro_phya_0;

typedef struct {
uint32 ro_dummy_63_32 :32;
}HAL_sgmii_ro_phya_1, *pHAL_sgmii_ro_phya_1;

typedef struct {
uint32 ro_rgs_ssusb_eq_dcleq :4;
uint32 ro_rgs_ssusb_eq_dcd0h :7;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_rgs_ssusb_eq_dcd0l :7;
uint32 ro_rgs_ssusb_eq_dcd1h :7;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_9, *pHAL_sgmii_ro_phya_9;

typedef struct {
uint32 ro_rgs_ssusb_eq_dcd1l :7;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_10, *pHAL_sgmii_ro_phya_10;

typedef struct {
uint32 ro_rgs_ssusb_eq_dce0 :7;
uint32 ro_rgs_ssusb_eq_dce1 :7;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_rgs_ssusb_eq_dchhl :7;
uint32 ro_rgs_ssusb_eq_dclhl :7;
uint32 ro_rgs_ssusb_eq_leq_stop :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_11, *pHAL_sgmii_ro_phya_11;

typedef struct {
uint32 ro_rgs_ssusb_eq_status :8;
uint32 ro_rgs_ssusb_eq_dceye0 :7;
uint32 rsv_15 :1;
uint32 ro_rgs_ssusb_eq_dceye1 :7;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_12, *pHAL_sgmii_ro_phya_12;

typedef struct {
uint32 ro_rgs_ssusb_eq_eye_monitor_errcnt_0 :20;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_13, *pHAL_sgmii_ro_phya_13;

typedef struct {
uint32 ro_rgs_ssusb_eq_eye_monitor_errcnt_1 :20;
uint32 ro_rgs_ssusb_eq_dcleqos :5;
uint32 ro_rgs_ssusb_eq_eye_cnt_rdy :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_14, *pHAL_sgmii_ro_phya_14;

typedef struct {
uint32 ro_rgs_ssusb_eq_pilpo :7;
uint32 ro_rgs_ssusb_eq_probe_out :8;
uint32 ro_rgs_ssusb_rx_sd_out :1;
uint32 ro_debug_prbs :16;
}HAL_sgmii_ro_phya_15, *pHAL_sgmii_ro_phya_15;

typedef struct {
uint32 ro_bist_err_cnt :16;
uint32 ro_bist_ok :1;
uint32 ro_bist_run :1;
uint32 ro_bist_err :1;
uint32 ro_test_code :8;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_16, *pHAL_sgmii_ro_phya_16;

typedef struct {
uint32 ro_utif_out :16;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_17, *pHAL_sgmii_ro_phya_17;

typedef struct {
uint32 ro_bist_prbs_done :1;
uint32 ro_bist_prbs_fail :1;
uint32 ro_bist_prbs_compare :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_bist_prbs_err_cnt :16;
}HAL_sgmii_ro_phya_18, *pHAL_sgmii_ro_phya_18;

typedef struct {
uint32 ro_tx_max :16;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_19, *pHAL_sgmii_ro_phya_19;

typedef struct {
uint32 ro_gpio14_max :16;
uint32 ro_rx_max :16;
}HAL_sgmii_ro_phya_20, *pHAL_sgmii_ro_phya_20;

typedef struct {
uint32 rsv_0 :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_bist_prbs_bit_err_fw_using :16;
}HAL_sgmii_ro_phya_21, *pHAL_sgmii_ro_phya_21;

typedef struct {
uint32 ro_tx_max_lsb :32;
}HAL_sgmii_ro_phya_22, *pHAL_sgmii_ro_phya_22;

typedef struct {
uint32 ro_rx_max_lsb :32;
}HAL_sgmii_ro_phya_23, *pHAL_sgmii_ro_phya_23;

typedef struct {
uint32 ro_lane0_err_bits :8;
uint32 ro_lane1_err_bits :8;
uint32 ro_lane2_err_bits :8;
uint32 ro_lane3_err_bits :8;
}HAL_sgmii_ro_phya_24, *pHAL_sgmii_ro_phya_24;

typedef struct {
uint32 ro_bist_prbs_bit_err_period_num :24;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_25, *pHAL_sgmii_ro_phya_25;

typedef struct {
uint32 ro_bist_px_lnx_flag :8;
uint32 ro_tx_data_pollution_pulse :1;
uint32 ro_bist_px_lnx_active :1;
uint32 ro_bist_tx_data_en :1;
uint32 ro_bist_tx_init_en :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_26, *pHAL_sgmii_ro_phya_26;

typedef struct {
uint32 ro_hsgmii_async_fifo_full :1;
uint32 ro_hsgmii_async_fifo_afull :1;
uint32 ro_hsgmii_async_fifo_empty :1;
uint32 ro_hsgmii_async_fifo_aempty :1;
uint32 ro_hsgmii_async_fifo_r_ready :1;
uint32 ro_hsgmii_async_fifo_w_ready :1;
uint32 ro_hsgmii_async_fifo_wfifo_cnt :5;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_hsgmii_async_fifo_rfifo_cnt :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_sgmii_ro_phya_afifo, *pHAL_sgmii_ro_phya_afifo;

typedef struct {
uint32 ro_hsgmii_async_fifo_full_cnt :32;
}HAL_sgmii_ro_phya_afifo_cnt_0, *pHAL_sgmii_ro_phya_afifo_cnt_0;

typedef struct {
uint32 ro_hsgmii_async_fifo_afull_cnt :32;
}HAL_sgmii_ro_phya_afifo_cnt_1, *pHAL_sgmii_ro_phya_afifo_cnt_1;

typedef struct {
uint32 ro_hsgmii_async_fifo_empty_cnt :32;
}HAL_sgmii_ro_phya_afifo_cnt_2, *pHAL_sgmii_ro_phya_afifo_cnt_2;

typedef struct {
uint32 ro_hsgmii_async_fifo_aempty_cnt :32;
}HAL_sgmii_ro_phya_afifo_cnt_3, *pHAL_sgmii_ro_phya_afifo_cnt_3;

typedef struct {
uint32 rg_sgmii_mode_force :2;
uint32 rg_ssusb_hsgmii_sel :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_ssusb_ip_sw_rst :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_qsgmii_test_mode :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_mode_ctrl_0, *pHAL_mode_ctrl_0;

typedef struct {
uint32 ro_hsgmii_tx_short_fifo_full :1;
uint32 ro_hsgmii_tx_short_fifo_afull :1;
uint32 ro_hsgmii_tx_short_fifo_empty :1;
uint32 ro_hsgmii_tx_short_fifo_aempty :1;
uint32 ro_hsgmii_tx_short_fifo_r_ready :1;
uint32 ro_hsgmii_tx_short_fifo_w_ready :1;
uint32 ro_hsgmii_tx_short_fifo_wfifo_cnt :5;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_hsgmii_tx_short_fifo_rfifo_cnt :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_tx_short_ro_phya_afifo, *pHAL_tx_short_ro_phya_afifo;

typedef struct {
uint32 rg_tx_short_fifo_dis :1;
uint32 rg_tx_short_fifo_en :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_hsgmii_tx_short_fifo_wclear :1;
uint32 rg_hsgmii_tx_short_fifo_rclear :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_hsgmii_tx_short_fifo_r_threshold :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_hsgmii_tx_short_fifo_w_threshold :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_tx_short_fifo_ctrl, *pHAL_tx_short_fifo_ctrl;

typedef struct {
uint32 pbus_int_add_0 :16;
uint32 pbus_int_add_1 :16;
}HAL_pbus_int_ctrl_0, *pHAL_pbus_int_ctrl_0;

typedef struct {
uint32 rg_pbus_int_en :1;
uint32 rg_pbus_int_clr :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 ro_pbus_int_sts :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rsv_16 :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_pbus_int_ctrl_1, *pHAL_pbus_int_ctrl_1;

typedef struct {
uint32 rg_multi_phy_r2t_lpbk_en :1;
uint32 rg_multi_phy_lpbk_async_fifo_wclear :1;
uint32 rg_multi_phy_lpbk_async_fifo_rclear :1;
uint32 rg_multi_phy_lpbk_async_fifo_full_cnt_clear :1;
uint32 rg_multi_phy_lpbk_async_fifo_afull_cnt_clear :1;
uint32 rg_multi_phy_lpbk_async_fifo_empty_cnt_clear :1;
uint32 rg_multi_phy_lpbk_async_fifo_aempty_cnt_clear :1;
uint32 rsv_7 :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_multi_phy_lpbk_async_fifo_w_threshold :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_multi_phy_lpbk_async_fifo_r_threshold :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_LPBK_FIFO_CTRL, *pHAL_LPBK_FIFO_CTRL;

typedef struct {
uint32 rg_efuse_tphy_rx_imp_sel :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_efuse_tphy_tx_imp_sel :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_efuse_tphy_iext_intr_ctrl :6;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_efuse_tphy_auto_load_valid :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_efuse_ctrl_0, *pHAL_efuse_ctrl_0;

typedef struct {
uint32 rg_fpga_ctrl_31_0 :32;
}HAL_FPGA_CTRL_0, *pHAL_FPGA_CTRL_0;

#endif