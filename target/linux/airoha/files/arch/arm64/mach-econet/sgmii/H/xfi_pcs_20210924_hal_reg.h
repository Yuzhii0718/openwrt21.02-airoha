#ifndef XFI_PCS_20210924_HAL_REG
#define XFI_PCS_20210924_HAL_REG

typedef struct {
uint32 rg_xfi_tx_xgmii2pcs_data_reverse :1;
uint32 rg_xfi_tx_xgmii2pcs_ctrl_reverse :1;
uint32 rg_speed_sel :4;
uint32 rg_speed_sel_l :1;
uint32 rg_reserved_001007 :4;
uint32 rg_lowpower :1;
uint32 rg_reserved_0012 :1;
uint32 rg_speed_sel_h :1;
uint32 rg_pcs_loop_back :1;
uint32 rg_rxpcs_sw_rst_n :1;
uint32 rg_txpcs_sw_rst_n :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rg_force_xfi_mac_tx_mode_en :1;
uint32 rg_force_xfi_mac_rx_mode_en :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_force_xfi_mac_tx_mode :4;
uint32 rg_force_xfi_mac_rx_mode :4;
}HAL_rg_pcs_ctrol_1, *pHAL_rg_pcs_ctrol_1;

typedef struct {
uint32 ro_reserved_0100 :1;
uint32 ro_low_power_ability :1;
uint32 ro_pcs_rx_link_status :1;
uint32 ro_reserved_010603 :4;
uint32 ro_fault :1;
uint32 ro_reserved_011508 :8;
uint32 rg_tog_pcs_st1 :1;
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
}HAL_ro_pcs_stus_1, *pHAL_ro_pcs_stus_1;

typedef struct {
uint32 rg_pcs_dev_id_h :16;
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
}HAL_rg_pcs_dev_id_hi, *pHAL_rg_pcs_dev_id_hi;

typedef struct {
uint32 rg_pcs_dev_id_l :16;
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
}HAL_rg_pcs_dev_id_low, *pHAL_rg_pcs_dev_id_low;

typedef struct {
uint32 ro_10g_cpb :1;
uint32 ro_10pass_ts_2b_tl_cpb :1;
uint32 ro_40g_cpb :1;
uint32 ro_100g_cpb :1;
uint32 ro_reserved_041504 :12;
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
}HAL_ro_pcs_sped_ability, *pHAL_ro_pcs_sped_ability;

typedef struct {
uint32 ro_reserved_051200 :13;
uint32 ro_clause22_exten_prest :1;
uint32 ro_vendor_spec_dev1_prest :1;
uint32 ro_vendor_spec_dev2_prest :1;
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
}HAL_ro_pcs_dev1, *pHAL_ro_pcs_dev1;

typedef struct {
uint32 ro_clause22_reg_prest :1;
uint32 ro_pmd_pma_prest :1;
uint32 ro_wis_prest :1;
uint32 ro_pcs_prest :1;
uint32 ro_phy_xs_prest :1;
uint32 ro_dte_xs_prest :1;
uint32 ro_tc_prest :1;
uint32 ro_auto_negotiation_prest :1;
uint32 ro_reserved_061508 :8;
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
}HAL_ro_pcs_dev2, *pHAL_ro_pcs_dev2;

typedef struct {
uint32 rg_pcs_tp_sel :3;
uint32 rg_reserved_071503 :13;
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
}HAL_rg_pcs_ctrol_2, *pHAL_rg_pcs_ctrol_2;

typedef struct {
uint32 ro_10gb_r_cpb :1;
uint32 ro_10gb_x_cpb :1;
uint32 ro_10gb_w_cpb :1;
uint32 ro_10gb_t_cpb :1;
uint32 ro_40gb_r_cpb :1;
uint32 ro_100gb_r_cpb :1;
uint32 ro_reserved_080906 :4;
uint32 ro_rx_fault :1;
uint32 ro_tx_fault :1;
uint32 ro_reserved_081312 :2;
uint32 ro_dev_prest :2;
uint32 rg_tog_pcs_st2 :1;
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
}HAL_ro_pcs_stus_2, *pHAL_ro_pcs_stus_2;

typedef struct {
uint32 rg_pkg_id_h :16;
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
}HAL_rg_pkg_id_h, *pHAL_rg_pkg_id_h;

typedef struct {
uint32 rg_pkg_id_l :16;
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
}HAL_rg_pkg_id_l, *pHAL_rg_pkg_id_l;

typedef struct {
uint32 ro_reserved_0b00 :1;
uint32 ro_100b_tx_eee :1;
uint32 ro_1000b_kr_eee :1;
uint32 ro_10gb_t_eee :1;
uint32 ro_1000b_kx_eee :1;
uint32 ro_10gb_kx4_eee :1;
uint32 ro_10gb_kr_eee :1;
uint32 ro_reserved_0b1507 :9;
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
}HAL_ro_eee_cpb, *pHAL_ro_eee_cpb;

typedef struct {
uint32 ro_base_r_10gb_t_pcs_blk_lk :1;
uint32 ro_base_r_10gb_t_pcs_hi_ber :1;
uint32 ro_10gb_r_prbs31_patt_tst_ability :1;
uint32 ro_10gb_r_prbs9_patt_tst_ability :1;
uint32 ro_reserved_0c1104 :8;
uint32 ro_base_r_10gb_t_rx_link_stus :1;
uint32 ro_reserved_0c1513 :3;
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
}HAL_ro_base_r_10gb_t_pcs_stus1, *pHAL_ro_base_r_10gb_t_pcs_stus1;

typedef struct {
uint32 ro_err_blk :8;
uint32 ro_ber_cnt :6;
uint32 ro_lah_hi_ber :1;
uint32 ro_lah_blk_lk :1;
uint32 rg_baser_10gb_tog :1;
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
}HAL_ro_base_r_10gb_t_pcs_stus2, *pHAL_ro_base_r_10gb_t_pcs_stus2;

typedef struct {
uint32 rg_tst_patt_seed_a0 :16;
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
}HAL_rg_pcs_tst_patt_seeda0, *pHAL_rg_pcs_tst_patt_seeda0;

typedef struct {
uint32 rg_tst_patt_seed_a1 :16;
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
}HAL_rg_pcs_tst_patt_seeda1, *pHAL_rg_pcs_tst_patt_seeda1;

typedef struct {
uint32 rg_tst_patt_seed_a2 :16;
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
}HAL_rg_pcs_tst_patt_seeda2, *pHAL_rg_pcs_tst_patt_seeda2;

typedef struct {
uint32 rg_tst_patt_seed_a3 :10;
uint32 rg_reserved_111510 :6;
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
}HAL_rg_pcs_tst_patt_seeda3, *pHAL_rg_pcs_tst_patt_seeda3;

typedef struct {
uint32 rg_tst_patt_seed_b0 :16;
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
}HAL_rg_pcs_tst_patt_seedb0, *pHAL_rg_pcs_tst_patt_seedb0;

typedef struct {
uint32 rg_tst_patt_seed_b1 :16;
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
}HAL_rg_pcs_tst_patt_seedb1, *pHAL_rg_pcs_tst_patt_seedb1;

typedef struct {
uint32 rg_tst_patt_seed_b2 :16;
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
}HAL_rg_pcs_tst_patt_seedb2, *pHAL_rg_pcs_tst_patt_seedb2;

typedef struct {
uint32 rg_tst_patt_seed_b3 :10;
uint32 rg_reserved_151510 :6;
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
}HAL_rg_pcs_tst_patt_seedb3, *pHAL_rg_pcs_tst_patt_seedb3;

typedef struct {
uint32 rg_tst_patt_din_sel :1;
uint32 rg_tst_patt_gen_sel :1;
uint32 rg_rx_tst_patt_en :1;
uint32 rg_tx_tst_patt_en :1;
uint32 rg_10gb_r_prbs31_tx_tst_patt_en :1;
uint32 rg_10gb_r_prbs31_rx_tst_patt_en :1;
uint32 rg_10gb_r_prbs09_tx_tst_patt_en :1;
uint32 rg_scram_idle_tst_patt_en :1;
uint32 rg_reserved_161508 :8;
uint32 rg_10gb_r_prbs09_rx_tst_patt_en :1;
uint32 rg_10gb_r_prbs07_tx_tst_patt_en :1;
uint32 rg_10gb_r_prbs07_rx_tst_patt_en :1;
uint32 rg_series_tst_patt_en :1;
uint32 rg_prog_tst_patt_en :1;
uint32 rg_tx_tst_patt_din_inv_en :1;
uint32 rg_rx_tst_patt_din_inv_en :1;
uint32 rg_reserved_1623 :1;
uint32 rg_sqwv_n_len :4;
uint32 rg_reserved_163128 :4;
}HAL_rg_pcs_tst_patt_ctrl, *pHAL_rg_pcs_tst_patt_ctrl;

typedef struct {
uint32 ro_dscr_tst_patt_err_cnt :16;
uint32 ro_prbs_tst_patt_err_cnt :16;
}HAL_ro_pcs_tst_patt_cnt, *pHAL_ro_pcs_tst_patt_cnt;

typedef struct {
uint32 rg_lpb_fifo_thd :4;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_tx_dbg_out_sel :3;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_txtorx_sync_tst :3;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_scr_en :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_pcstx_ctrl0, *pHAL_rg_pcstx_ctrl0;

typedef struct {
uint32 rg_tx_tst_p00_b0 :32;
}HAL_rg_vendor_define001, *pHAL_rg_vendor_define001;

typedef struct {
uint32 rg_tx_tst_p00_b1 :32;
}HAL_rg_vendor_define002, *pHAL_rg_vendor_define002;

typedef struct {
uint32 rg_tx_tst_p01_b0 :32;
}HAL_rg_vendor_define003, *pHAL_rg_vendor_define003;

typedef struct {
uint32 rg_tx_tst_p01_b1 :32;
}HAL_rg_vendor_define004, *pHAL_rg_vendor_define004;

typedef struct {
uint32 rg_tx_tst_p02_b0 :32;
}HAL_rg_vendor_define005, *pHAL_rg_vendor_define005;

typedef struct {
uint32 rg_tx_tst_p02_b1 :32;
}HAL_rg_vendor_define006, *pHAL_rg_vendor_define006;

typedef struct {
uint32 rg_tx_tst_p03_b0 :32;
}HAL_rg_vendor_define007, *pHAL_rg_vendor_define007;

typedef struct {
uint32 rg_tx_tst_p03_b1 :32;
}HAL_rg_vendor_define008, *pHAL_rg_vendor_define008;

typedef struct {
uint32 rg_tx_tst_p04_b0 :32;
}HAL_rg_vendor_define009, *pHAL_rg_vendor_define009;

typedef struct {
uint32 rg_tx_tst_p04_b1 :32;
}HAL_rg_vendor_define010, *pHAL_rg_vendor_define010;

typedef struct {
uint32 rg_tx_tst_p05_b0 :32;
}HAL_rg_vendor_define011, *pHAL_rg_vendor_define011;

typedef struct {
uint32 rg_tx_tst_p05_b1 :32;
}HAL_rg_vendor_define012, *pHAL_rg_vendor_define012;

typedef struct {
uint32 rg_tx_tst_p06_b0 :32;
}HAL_rg_vendor_define013, *pHAL_rg_vendor_define013;

typedef struct {
uint32 rg_tx_tst_p06_b1 :32;
}HAL_rg_vendor_define014, *pHAL_rg_vendor_define014;

typedef struct {
uint32 rg_tx_tst_p07_b0 :32;
}HAL_rg_vendor_define015, *pHAL_rg_vendor_define015;

typedef struct {
uint32 rg_tx_tst_p07_b1 :32;
}HAL_rg_vendor_define016, *pHAL_rg_vendor_define016;

typedef struct {
uint32 rg_tx_tst_p08_b0 :32;
}HAL_rg_vendor_define017, *pHAL_rg_vendor_define017;

typedef struct {
uint32 rg_tx_tst_p08_b1 :32;
}HAL_rg_vendor_define018, *pHAL_rg_vendor_define018;

typedef struct {
uint32 rg_tx_tst_p09_b0 :32;
}HAL_rg_vendor_define019, *pHAL_rg_vendor_define019;

typedef struct {
uint32 rg_tx_tst_p09_b1 :32;
}HAL_rg_vendor_define020, *pHAL_rg_vendor_define020;

typedef struct {
uint32 rg_tx_tst_p10_b0 :32;
}HAL_rg_vendor_define021, *pHAL_rg_vendor_define021;

typedef struct {
uint32 rg_tx_tst_p10_b1 :32;
}HAL_rg_vendor_define022, *pHAL_rg_vendor_define022;

typedef struct {
uint32 rg_tx_tst_p11_b0 :32;
}HAL_rg_vendor_define023, *pHAL_rg_vendor_define023;

typedef struct {
uint32 rg_tx_tst_p11_b1 :32;
}HAL_rg_vendor_define024, *pHAL_rg_vendor_define024;

typedef struct {
uint32 rg_tx_tst_p12_b0 :32;
}HAL_rg_vendor_define025, *pHAL_rg_vendor_define025;

typedef struct {
uint32 rg_tx_tst_p12_b1 :32;
}HAL_rg_vendor_define026, *pHAL_rg_vendor_define026;

typedef struct {
uint32 rg_tx_tst_p13_b0 :32;
}HAL_rg_vendor_define027, *pHAL_rg_vendor_define027;

typedef struct {
uint32 rg_tx_tst_p13_b1 :32;
}HAL_rg_vendor_define028, *pHAL_rg_vendor_define028;

typedef struct {
uint32 rg_tx_tst_p14_b0 :32;
}HAL_rg_vendor_define029, *pHAL_rg_vendor_define029;

typedef struct {
uint32 rg_tx_tst_p14_b1 :32;
}HAL_rg_vendor_define030, *pHAL_rg_vendor_define030;

typedef struct {
uint32 rg_tx_tst_p15_b0 :32;
}HAL_rg_vendor_define031, *pHAL_rg_vendor_define031;

typedef struct {
uint32 rg_tx_tst_p15_b1 :32;
}HAL_rg_vendor_define032, *pHAL_rg_vendor_define032;

typedef struct {
uint32 rg_tx_tst_p16_b0 :32;
}HAL_rg_vendor_define033, *pHAL_rg_vendor_define033;

typedef struct {
uint32 rg_tx_tst_p16_b1 :32;
}HAL_rg_vendor_define034, *pHAL_rg_vendor_define034;

typedef struct {
uint32 rg_tx_tst_p17_b0 :32;
}HAL_rg_vendor_define035, *pHAL_rg_vendor_define035;

typedef struct {
uint32 rg_tx_tst_p17_b1 :32;
}HAL_rg_vendor_define036, *pHAL_rg_vendor_define036;

typedef struct {
uint32 rg_tx_tst_p18_b0 :32;
}HAL_rg_vendor_define037, *pHAL_rg_vendor_define037;

typedef struct {
uint32 rg_tx_tst_p18_b1 :32;
}HAL_rg_vendor_define038, *pHAL_rg_vendor_define038;

typedef struct {
uint32 rg_tx_tst_p19_b0 :32;
}HAL_rg_vendor_define039, *pHAL_rg_vendor_define039;

typedef struct {
uint32 rg_tx_tst_p19_b1 :32;
}HAL_rg_vendor_define040, *pHAL_rg_vendor_define040;

typedef struct {
uint32 rg_tx_tst_p20_b0 :32;
}HAL_rg_vendor_define041, *pHAL_rg_vendor_define041;

typedef struct {
uint32 rg_tx_tst_p20_b1 :32;
}HAL_rg_vendor_define042, *pHAL_rg_vendor_define042;

typedef struct {
uint32 rg_tx_tst_p21_b0 :32;
}HAL_rg_vendor_define043, *pHAL_rg_vendor_define043;

typedef struct {
uint32 rg_tx_tst_p21_b1 :32;
}HAL_rg_vendor_define044, *pHAL_rg_vendor_define044;

typedef struct {
uint32 rg_tx_tst_p22_b0 :32;
}HAL_rg_vendor_define045, *pHAL_rg_vendor_define045;

typedef struct {
uint32 rg_tx_tst_p22_b1 :32;
}HAL_rg_vendor_define046, *pHAL_rg_vendor_define046;

typedef struct {
uint32 rg_tx_tst_p23_b0 :32;
}HAL_rg_vendor_define047, *pHAL_rg_vendor_define047;

typedef struct {
uint32 rg_tx_tst_p23_b1 :32;
}HAL_rg_vendor_define048, *pHAL_rg_vendor_define048;

typedef struct {
uint32 rg_tx_tst_p24_b0 :32;
}HAL_rg_vendor_define049, *pHAL_rg_vendor_define049;

typedef struct {
uint32 rg_tx_tst_p24_b1 :32;
}HAL_rg_vendor_define050, *pHAL_rg_vendor_define050;

typedef struct {
uint32 rg_tx_tst_p25_b0 :32;
}HAL_rg_vendor_define051, *pHAL_rg_vendor_define051;

typedef struct {
uint32 rg_tx_tst_p25_b1 :32;
}HAL_rg_vendor_define052, *pHAL_rg_vendor_define052;

typedef struct {
uint32 rg_tsl_limit :10;
uint32 rg_reserved_673126 :22;
}HAL_rg_vendor_define055, *pHAL_rg_vendor_define055;

typedef struct {
uint32 rg_tql_limit :19;
uint32 rg_reserved_683119 :13;
}HAL_rg_vendor_define056, *pHAL_rg_vendor_define056;

typedef struct {
uint32 rg_twl_limit :11;
uint32 rg_reserved_691511 :5;
uint32 rg_t1u_limit :8;
uint32 rg_reserved_693124 :8;
}HAL_rg_vendor_define057, *pHAL_rg_vendor_define057;

typedef struct {
uint32 ro_rx_tst_p00_b0 :32;
}HAL_ro_vendor_define058, *pHAL_ro_vendor_define058;

typedef struct {
uint32 ro_rx_tst_p00_b1 :32;
}HAL_ro_vendor_define059, *pHAL_ro_vendor_define059;

typedef struct {
uint32 ro_rx_tst_p01_b0 :32;
}HAL_ro_vendor_define060, *pHAL_ro_vendor_define060;

typedef struct {
uint32 ro_rx_tst_p01_b1 :32;
}HAL_ro_vendor_define061, *pHAL_ro_vendor_define061;

typedef struct {
uint32 ro_rx_tst_p02_b0 :32;
}HAL_ro_vendor_define062, *pHAL_ro_vendor_define062;

typedef struct {
uint32 ro_rx_tst_p02_b1 :32;
}HAL_ro_vendor_define063, *pHAL_ro_vendor_define063;

typedef struct {
uint32 ro_rx_tst_p03_b0 :32;
}HAL_ro_vendor_define064, *pHAL_ro_vendor_define064;

typedef struct {
uint32 ro_rx_tst_p03_b1 :32;
}HAL_ro_vendor_define065, *pHAL_ro_vendor_define065;

typedef struct {
uint32 ro_rx_tst_p04_b0 :32;
}HAL_ro_vendor_define066, *pHAL_ro_vendor_define066;

typedef struct {
uint32 ro_rx_tst_p04_b1 :32;
}HAL_ro_vendor_define067, *pHAL_ro_vendor_define067;

typedef struct {
uint32 ro_rx_tst_p05_b0 :32;
}HAL_ro_vendor_define068, *pHAL_ro_vendor_define068;

typedef struct {
uint32 ro_rx_tst_p05_b1 :32;
}HAL_ro_vendor_define069, *pHAL_ro_vendor_define069;

typedef struct {
uint32 ro_rx_tst_p06_b0 :32;
}HAL_ro_vendor_define070, *pHAL_ro_vendor_define070;

typedef struct {
uint32 ro_rx_tst_p06_b1 :32;
}HAL_ro_vendor_define071, *pHAL_ro_vendor_define071;

typedef struct {
uint32 ro_rx_tst_p07_b0 :32;
}HAL_ro_vendor_define072, *pHAL_ro_vendor_define072;

typedef struct {
uint32 ro_rx_tst_p07_b1 :32;
}HAL_ro_vendor_define073, *pHAL_ro_vendor_define073;

typedef struct {
uint32 ro_rx_tst_p08_b0 :32;
}HAL_ro_vendor_define074, *pHAL_ro_vendor_define074;

typedef struct {
uint32 ro_rx_tst_p08_b1 :32;
}HAL_ro_vendor_define075, *pHAL_ro_vendor_define075;

typedef struct {
uint32 ro_rx_tst_p09_b0 :32;
}HAL_ro_vendor_define076, *pHAL_ro_vendor_define076;

typedef struct {
uint32 ro_rx_tst_p09_b1 :32;
}HAL_ro_vendor_define077, *pHAL_ro_vendor_define077;

typedef struct {
uint32 ro_rx_tst_p10_b0 :32;
}HAL_ro_vendor_define078, *pHAL_ro_vendor_define078;

typedef struct {
uint32 ro_rx_tst_p10_b1 :32;
}HAL_ro_vendor_define079, *pHAL_ro_vendor_define079;

typedef struct {
uint32 ro_rx_tst_p11_b0 :32;
}HAL_ro_vendor_define080, *pHAL_ro_vendor_define080;

typedef struct {
uint32 ro_rx_tst_p11_b1 :32;
}HAL_ro_vendor_define081, *pHAL_ro_vendor_define081;

typedef struct {
uint32 ro_rx_tst_p12_b0 :32;
}HAL_ro_vendor_define082, *pHAL_ro_vendor_define082;

typedef struct {
uint32 ro_rx_tst_p12_b1 :32;
}HAL_ro_vendor_define083, *pHAL_ro_vendor_define083;

typedef struct {
uint32 ro_rx_tst_p13_b0 :32;
}HAL_ro_vendor_define084, *pHAL_ro_vendor_define084;

typedef struct {
uint32 ro_rx_tst_p13_b1 :32;
}HAL_ro_vendor_define085, *pHAL_ro_vendor_define085;

typedef struct {
uint32 ro_rx_tst_p14_b0 :32;
}HAL_ro_vendor_define086, *pHAL_ro_vendor_define086;

typedef struct {
uint32 ro_rx_tst_p14_b1 :32;
}HAL_ro_vendor_define087, *pHAL_ro_vendor_define087;

typedef struct {
uint32 ro_rx_tst_p15_b0 :32;
}HAL_ro_vendor_define088, *pHAL_ro_vendor_define088;

typedef struct {
uint32 ro_rx_tst_p15_b1 :32;
}HAL_ro_vendor_define089, *pHAL_ro_vendor_define089;

typedef struct {
uint32 ro_rx_tst_p16_b0 :32;
}HAL_ro_vendor_define090, *pHAL_ro_vendor_define090;

typedef struct {
uint32 ro_rx_tst_p16_b1 :32;
}HAL_ro_vendor_define091, *pHAL_ro_vendor_define091;

typedef struct {
uint32 ro_rx_tst_p17_b0 :32;
}HAL_ro_vendor_define092, *pHAL_ro_vendor_define092;

typedef struct {
uint32 ro_rx_tst_p17_b1 :32;
}HAL_ro_vendor_define093, *pHAL_ro_vendor_define093;

typedef struct {
uint32 ro_rx_tst_p18_b0 :32;
}HAL_ro_vendor_define094, *pHAL_ro_vendor_define094;

typedef struct {
uint32 ro_rx_tst_p18_b1 :32;
}HAL_ro_vendor_define095, *pHAL_ro_vendor_define095;

typedef struct {
uint32 ro_rx_tst_p19_b0 :32;
}HAL_ro_vendor_define096, *pHAL_ro_vendor_define096;

typedef struct {
uint32 ro_rx_tst_p19_b1 :32;
}HAL_ro_vendor_define097, *pHAL_ro_vendor_define097;

typedef struct {
uint32 ro_rx_tst_p20_b0 :32;
}HAL_ro_vendor_define098, *pHAL_ro_vendor_define098;

typedef struct {
uint32 ro_rx_tst_p20_b1 :32;
}HAL_ro_vendor_define099, *pHAL_ro_vendor_define099;

typedef struct {
uint32 ro_rx_tst_p21_b0 :32;
}HAL_ro_vendor_define100, *pHAL_ro_vendor_define100;

typedef struct {
uint32 ro_rx_tst_p21_b1 :32;
}HAL_ro_vendor_define101, *pHAL_ro_vendor_define101;

typedef struct {
uint32 ro_rx_tst_p22_b0 :32;
}HAL_ro_vendor_define102, *pHAL_ro_vendor_define102;

typedef struct {
uint32 ro_rx_tst_p22_b1 :32;
}HAL_ro_vendor_define103, *pHAL_ro_vendor_define103;

typedef struct {
uint32 ro_rx_tst_p23_b0 :32;
}HAL_ro_vendor_define104, *pHAL_ro_vendor_define104;

typedef struct {
uint32 ro_rx_tst_p23_b1 :32;
}HAL_ro_vendor_define105, *pHAL_ro_vendor_define105;

typedef struct {
uint32 ro_rx_tst_p24_b0 :32;
}HAL_ro_vendor_define106, *pHAL_ro_vendor_define106;

typedef struct {
uint32 ro_rx_tst_p24_b1 :32;
}HAL_ro_vendor_define107, *pHAL_ro_vendor_define107;

typedef struct {
uint32 ro_rx_tst_p25_b0 :32;
}HAL_ro_vendor_define108, *pHAL_ro_vendor_define108;

typedef struct {
uint32 ro_rx_tst_p25_b1 :32;
}HAL_ro_vendor_define109, *pHAL_ro_vendor_define109;

typedef struct {
uint32 rg_tqr_limit :19;
uint32 rg_reserved_a03119 :13;
}HAL_rg_vendor_define113, *pHAL_rg_vendor_define113;

typedef struct {
uint32 rg_twr_limit :11;
uint32 rg_reserved_a13111 :21;
}HAL_rg_vendor_define114, *pHAL_rg_vendor_define114;

typedef struct {
uint32 rg_twtf_limit :21;
uint32 rg_reserved_a23121 :11;
}HAL_rg_vendor_define115, *pHAL_rg_vendor_define115;

typedef struct {
uint32 rg_ber_time_limit :15;
uint32 rg_reserved_a33115 :17;
}HAL_rg_vendor_define116, *pHAL_rg_vendor_define116;

typedef struct {
uint32 rg_prbs_align_patt :32;
}HAL_rg_vendor_define119, *pHAL_rg_vendor_define119;

typedef struct {
uint32 rg_prbs_init_seed :32;
}HAL_rg_vendor_define120, *pHAL_rg_vendor_define120;

typedef struct {
uint32 rg_prbs_pollution_trig :1;
uint32 rg_reserved_a80701 :7;
uint32 rg_prbs_err_cnt_rst_sel :1;
uint32 rg_reserved_a81509 :7;
uint32 rg_prbs_fail_thold :16;
}HAL_rg_vendor_define121, *pHAL_rg_vendor_define121;

typedef struct {
uint32 rg_prbs_pollution_patt :16;
uint32 rg_reserved_a93116 :16;
}HAL_rg_vendor_define122, *pHAL_rg_vendor_define122;

typedef struct {
uint32 rg_err_cnt_rst :1;
uint32 rg_reserved_aa0701 :7;
uint32 rg_prog_data_max_depth :3;
uint32 rg_reserved_aa1511 :5;
uint32 rg_prbs_match_times :4;
uint32 rg_reserved_aa2320 :4;
uint32 rg_prbs_align_times :4;
uint32 rg_reserved_aa3128 :4;
}HAL_rg_vendor_define123, *pHAL_rg_vendor_define123;

typedef struct {
uint32 rg_prbs_check_timer :24;
uint32 rg_reserved_ab3124 :8;
}HAL_rg_vendor_define124, *pHAL_rg_vendor_define124;

typedef struct {
uint32 ro_prbs_done :1;
uint32 ro_reserved_ac0701 :7;
uint32 ro_prbs_comp :1;
uint32 ro_reserved_ac1509 :7;
uint32 ro_prbs_fail :1;
uint32 ro_reserved_ac2317 :7;
uint32 ro_prbs_pass :1;
uint32 ro_reserved_ac3125 :7;
}HAL_rg_vendor_define125, *pHAL_rg_vendor_define125;

typedef struct {
uint32 rg_prog_tst_patt_data0 :16;
uint32 rg_prog_tst_patt_data1 :16;
}HAL_rg_vendor_define126, *pHAL_rg_vendor_define126;

typedef struct {
uint32 rg_prog_tst_patt_data2 :16;
uint32 rg_prog_tst_patt_data3 :16;
}HAL_rg_vendor_define127, *pHAL_rg_vendor_define127;

typedef struct {
uint32 rg_prog_tst_patt_data4 :16;
uint32 rg_prog_tst_patt_data5 :16;
}HAL_rg_vendor_define128, *pHAL_rg_vendor_define128;

typedef struct {
uint32 rg_prog_tst_patt_data6 :16;
uint32 rg_prog_tst_patt_data7 :16;
}HAL_rg_vendor_define129, *pHAL_rg_vendor_define129;

typedef struct {
uint32 rg_rd_toggle :1;
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
}HAL_rg_read_toggle, *pHAL_rg_read_toggle;

typedef struct {
uint32 rg_tst_rx_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_tst_tx_mode :2;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_tst_tx_en :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_tx_test_trig :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_test_set, *pHAL_rg_test_set;

typedef struct {
uint32 rg_tx_gb_thd :5;
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
}HAL_greabox_thd, *pHAL_greabox_thd;

typedef struct {
uint32 rsv_0 :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_rx_dbg_out_sel :4;
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
uint32 rg_descr_en :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_pcsrx_ctrl0, *pHAL_rg_pcsrx_ctrl0;

typedef struct {
uint32 ro_test_gen_dbg_l :32;
}HAL_ro_debug_pcstx0, *pHAL_ro_debug_pcstx0;

typedef struct {
uint32 ro_test_gen_dbg_h :32;
}HAL_ro_debug_pcstx1, *pHAL_ro_debug_pcstx1;

typedef struct {
uint32 ro_mpi2xgmii_dbg_l :32;
}HAL_ro_debug_pcstx2, *pHAL_ro_debug_pcstx2;

typedef struct {
uint32 ro_mpi2xgmii_dbg_h :32;
}HAL_ro_debug_pcstx3, *pHAL_ro_debug_pcstx3;

typedef struct {
uint32 ro_enc_64b66b_dbg_l :32;
}HAL_ro_debug_pcstx4, *pHAL_ro_debug_pcstx4;

typedef struct {
uint32 ro_enc_64b66b_dbg_h :32;
}HAL_ro_debug_pcstx5, *pHAL_ro_debug_pcstx5;

typedef struct {
uint32 ro_scr_dbg_l :32;
}HAL_ro_debug_pcstx6, *pHAL_ro_debug_pcstx6;

typedef struct {
uint32 ro_scr_dbg_h :32;
}HAL_ro_debug_pcstx7, *pHAL_ro_debug_pcstx7;

typedef struct {
uint32 ro_tx_gb_dbg_l :32;
}HAL_ro_debug_pcstx8, *pHAL_ro_debug_pcstx8;

typedef struct {
uint32 ro_tx_gb_dbg_h :32;
}HAL_ro_debug_pcstx9, *pHAL_ro_debug_pcstx9;

typedef struct {
uint32 ro_dec_64b66b_dbg_l :32;
}HAL_ro_debug_pcsrx0, *pHAL_ro_debug_pcsrx0;

typedef struct {
uint32 ro_dec_64b66b_dbg_h :32;
}HAL_ro_debug_pcsrx1, *pHAL_ro_debug_pcsrx1;

typedef struct {
uint32 ro_rx_gb_dbg_l :32;
}HAL_ro_debug_pcsrx2, *pHAL_ro_debug_pcsrx2;

typedef struct {
uint32 ro_rx_gb_dbg_h :32;
}HAL_ro_debug_pcsrx3, *pHAL_ro_debug_pcsrx3;

typedef struct {
uint32 ro_test_chk_dbg_l :32;
}HAL_ro_debug_pcsrx4, *pHAL_ro_debug_pcsrx4;

typedef struct {
uint32 ro_test_chk_dbg_h :32;
}HAL_ro_debug_pcsrx5, *pHAL_ro_debug_pcsrx5;

typedef struct {
uint32 ro_dec_scr_dbg_l :32;
}HAL_ro_debug_pcsrx6, *pHAL_ro_debug_pcsrx6;

typedef struct {
uint32 ro_dec_scr_dbg_h :32;
}HAL_ro_debug_pcsrx7, *pHAL_ro_debug_pcsrx7;

typedef struct {
uint32 rg_t_type_s_int_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_t_type_c_int_en :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_t_type_d_int_en :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_t_type_t_int_en :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_xfi_pcs_int_ctrl_0, *pHAL_rg_xfi_pcs_int_ctrl_0;

typedef struct {
uint32 rg_t_type_e_int_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_txpcs_fsm_enc_err_int_en :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_r_type_s_int_en :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_r_type_c_int_en :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_xfi_pcs_int_ctrl_1, *pHAL_rg_xfi_pcs_int_ctrl_1;

typedef struct {
uint32 rg_r_type_d_int_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_r_type_t_int_en :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_r_type_e_int_en :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_rxpcs_fsm_dec_err_int_en :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_xfi_pcs_int_ctrl_2, *pHAL_rg_xfi_pcs_int_ctrl_2;

typedef struct {
uint32 rg_hi_ber_st_int_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_link_up_st_int_en :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_rx_block_lock_st_int_en :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_fail_sync_xor_st_int_en :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_xfi_pcs_int_ctrl_3, *pHAL_rg_xfi_pcs_int_ctrl_3;

typedef struct {
uint32 t_type_s_int :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 t_type_c_int :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 t_type_d_int :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 t_type_t_int :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_xfi_pcs_int_sta_0, *pHAL_xfi_pcs_int_sta_0;

typedef struct {
uint32 t_type_e_int :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 txpcs_fsm_enc_err_int :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 r_type_s_int :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 r_type_c_int :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_xfi_pcs_int_sta_1, *pHAL_xfi_pcs_int_sta_1;

typedef struct {
uint32 r_type_d_int :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 r_type_t_int :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 r_type_e_int :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rxpcs_fsm_dec_err_int :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_xfi_pcs_int_sta_2, *pHAL_xfi_pcs_int_sta_2;

typedef struct {
uint32 hi_ber_st_int :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 link_up_st_int :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rx_block_lock_st_int :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 fail_sync_xor_st_int :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_xfi_pcs_int_sta_3, *pHAL_xfi_pcs_int_sta_3;

typedef struct {
uint32 rg_link_down_st_int_en :1;
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
}HAL_rg_xfi_pcs_int_ctrl_4, *pHAL_rg_xfi_pcs_int_ctrl_4;

typedef struct {
uint32 link_down_st_int :1;
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
}HAL_xfi_pcs_int_sta_4, *pHAL_xfi_pcs_int_sta_4;

typedef struct {
uint32 rg_user_define_rxd_lsb :32;
}HAL_rg_user_define_xgmii_data_lsb, *pHAL_rg_user_define_xgmii_data_lsb;

typedef struct {
uint32 rg_user_define_rxd_msb :32;
}HAL_rg_user_define_xgmii_data_msb, *pHAL_rg_user_define_xgmii_data_msb;

typedef struct {
uint32 rg_user_define_rxc :8;
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
}HAL_rg_user_define_xgmii_control, *pHAL_rg_user_define_xgmii_control;

typedef struct {
uint32 rg_user_define_sel :1;
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
}HAL_rg_user_define_sel, *pHAL_rg_user_define_sel;

typedef struct {
uint32 rg_an_enable :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_rate_change_moment :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_an_restart :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rg_no_receive_idle_cnt_clr :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_max_rx_ability_cnt :3;
uint32 rsv_19 :1;
uint32 rg_max_rx_ack_cnt :3;
uint32 rsv_23 :1;
uint32 rg_max_idle_cnt :3;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_usxgmii_an_control_0, *pHAL_rg_usxgmii_an_control_0;

typedef struct {
uint32 rg_an_ability :16;
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
}HAL_rg_usxgmii_an_control_1, *pHAL_rg_usxgmii_an_control_1;

typedef struct {
uint32 rg_max_link_timer_an_restart_state :10;
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
}HAL_rg_usxgmii_an_control_2, *pHAL_rg_usxgmii_an_control_2;

typedef struct {
uint32 rg_max_link_timer_complete_ack_detect_state :10;
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
}HAL_rg_usxgmii_an_control_3, *pHAL_rg_usxgmii_an_control_3;

typedef struct {
uint32 rg_max_link_timer_idle_detect_state :10;
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
}HAL_rg_usxgmii_an_control_4, *pHAL_rg_usxgmii_an_control_4;

typedef struct {
uint32 rg_max_an_sync_cnt :20;
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
}HAL_rg_usxgmii_an_control_5, *pHAL_rg_usxgmii_an_control_5;

typedef struct {
uint32 ro_partner_ability :16;
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
uint32 ro_cur_usxgmii_mode :3;
uint32 rsv_31 :1;
}HAL_ro_usxgmii_an_stats_0, *pHAL_ro_usxgmii_an_stats_0;

typedef struct {
uint32 ro_autoneg_dbg_l :32;
}HAL_ro_usxgmii_an_stats_1, *pHAL_ro_usxgmii_an_stats_1;

typedef struct {
uint32 ro_autoneg_dbg_h :32;
}HAL_ro_usxgmii_an_stats_2, *pHAL_ro_usxgmii_an_stats_2;

typedef struct {
uint32 rg_tog_pcs_autoneg_sts :1;
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
}HAL_rg_usxgmii_an_control_6, *pHAL_rg_usxgmii_an_control_6;

typedef struct {
uint32 rg_xfi_tx_mode_int :3;
uint32 rsv_3 :1;
uint32 rg_xfi_rx_mode_int :3;
uint32 rsv_7 :1;
uint32 rg_usxgmii_mode :3;
uint32 rsv_11 :1;
uint32 rg_usxgmii_rate_update_mode :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_xfi_mode_tx_sel :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rg_xfi_mode_rx_sel :1;
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
}HAL_rg_usxgmii_an_control_7, *pHAL_rg_usxgmii_an_control_7;

typedef struct {
uint32 ro_xfi_debug_0 :32;
}HAL_ro_xfi_debug_0, *pHAL_ro_xfi_debug_0;

typedef struct {
uint32 ro_xfi_debug_1 :32;
}HAL_ro_xfi_debug_1, *pHAL_ro_xfi_debug_1;

typedef struct {
uint32 ro_xfi_debug_2 :32;
}HAL_ro_xfi_debug_2, *pHAL_ro_xfi_debug_2;

typedef struct {
uint32 ro_xfi_debug_3 :32;
}HAL_ro_xfi_debug_3, *pHAL_ro_xfi_debug_3;

#endif