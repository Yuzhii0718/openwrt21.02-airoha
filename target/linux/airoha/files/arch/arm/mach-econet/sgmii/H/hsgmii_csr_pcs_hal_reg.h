#ifndef HSGMII_CSR_PCS_HAL_REG
#define HSGMII_CSR_PCS_HAL_REG

typedef struct {
uint32 rg_an_transparent :1;
uint32 rg_code_sync_set_en :1;
uint32 rg_code_sync_set_val :1;
uint32 rg_sgmii_an_block_ack_rxck :1;
uint32 rg_sgmii_pat_gen_tx_en_rxck :1;
uint32 rg_sgmii_err_prop_ki_sig_los_r :1;
uint32 rg_sgmii_err_prop_ki_sync_f :1;
uint32 rg_sgmii_err_prop_ko_sig_los_f :1;
uint32 rg_sgmii_err_prop_ko_sync_r :1;
uint32 rg_gmii_dv_err_option :1;
uint32 rg_an_enable :1;
uint32 rg_unidir_ena :1;
uint32 rg_remote_fault_dis :1;
uint32 rg_an_restart :1;
uint32 rg_gmii_txclk_ena :1;
uint32 rg_rx_clk_ena :1;
uint32 rg_sgmii_1us_timer :8;
uint32 rg_sgmii_send_an_error_en :1;
uint32 rg_mr_np_loaded :1;
uint32 rg_sd_sig_det :1;
uint32 rg_sgmii_ena :1;
uint32 rg_sgmii_speed_00 :1;
uint32 rg_sgmii_speed_01 :1;
uint32 rg_tbi_10b_mode :1;
uint32 rsv_31 :1;
}HAL_rg_hsgmii_pcs_ctrol_1, *pHAL_rg_hsgmii_pcs_ctrol_1;

typedef struct {
uint32 rg_mr_np :16;
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
}HAL_rg_hsgmii_pcs_ctrol_2, *pHAL_rg_hsgmii_pcs_ctrol_2;

typedef struct {
uint32 rg_link_ststime :20;
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
}HAL_rg_hsgmii_pcs_ctrol_3, *pHAL_rg_hsgmii_pcs_ctrol_3;

typedef struct {
uint32 rg_an_link_timer :20;
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
}HAL_rg_hsgmii_pcs_ctrol_4, *pHAL_rg_hsgmii_pcs_ctrol_4;

typedef struct {
uint32 rg_an_ability :16;
uint32 rg_sw_reset_rxck :1;
uint32 rg_sw_reset_txck :1;
uint32 rg_sw_reset_refck :1;
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
}HAL_rg_hsgmii_pcs_ctrol_5, *pHAL_rg_hsgmii_pcs_ctrol_5;

typedef struct {
uint32 rg_sgmii_mode2_en :1;
uint32 rg_sgmii_force_rateadapt :1;
uint32 rg_sgmii_force_rateadapt_value :2;
uint32 rg_sgmii_tx_enable :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_sgmii_mac_mode :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rg_sgmii_spd_force_1000 :1;
uint32 rg_sgmii_spd_force_100 :1;
uint32 rg_sgmii_spd_force_10 :1;
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
}HAL_rg_hsgmii_pcs_ctrol_6, *pHAL_rg_hsgmii_pcs_ctrol_6;

typedef struct {
uint32 rg_force_rxfb_in_lane0 :1;
uint32 rg_mode2_gpii_cnt_clr_rx_pcs :1;
uint32 rg_mode2_gpii_cnt_clr_rx_pma :1;
uint32 rg_mode2_is2500basex_rx :1;
uint32 rg_mode2_is2500basex_tx :1;
uint32 rg_mode2_force_remote_tx :1;
uint32 rg_tbi_10_rx_reverse_mode1 :1;
uint32 rg_tbi_20_rx_reverse_mode1 :1;
uint32 rg_tbi_rx_reverse_10b_mode1 :1;
uint32 rg_tbi_10_tx_reverse_mode1 :1;
uint32 rg_tbi_20_tx_reverse_mode1 :1;
uint32 rg_tbi_tx_reverse_10b_mode1 :1;
uint32 rg_tbi_10_rx_reverse_mode2 :1;
uint32 rg_tbi_20_rx_reverse_mode2 :1;
uint32 rg_tbi_rx_reverse_10b_mode2 :1;
uint32 rg_tbi_10_tx_reverse_mode2 :1;
uint32 rg_tbi_20_tx_reverse_mode2 :1;
uint32 rg_tbi_tx_reverse_10b_mode2 :1;
uint32 rg_gpii_rx_fifo_reverse_mode2 :1;
uint32 rg_tbi_20_rx_reverse_all_mode1 :1;
uint32 rg_tbi_20_tx_reverse_all_mode1 :1;
uint32 rg_tbi_20_rx_reverse_all_mode2 :1;
uint32 rg_tbi_20_tx_reverse_all_mode2 :1;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_hsgmii_pcs_gpii_0, *pHAL_rg_hsgmii_pcs_gpii_0;

typedef struct {
uint32 rg_hsgmii_mode_probe_sel :1;
uint32 rg_hsgmii_mode1_probe_pcs_rx :1;
uint32 rg_hsgmii_mode1_probe_pcs_tx :1;
uint32 rg_hsgmii_mode1_probe_pcs_gpiirx :1;
uint32 rg_hsgmii_mode1_probe_pcs_gpiitx :1;
uint32 rg_hsgmii_mode1_probe_an :1;
uint32 rg_hsgmii_mode1_probe_sync :1;
uint32 rg_hsgmii_mode2_probe_pcs_rx :1;
uint32 rg_hsgmii_mode2_probe_pcs_tx :1;
uint32 rg_hsgmii_mode2_probe_pcs_gpiirx :1;
uint32 rg_hsgmii_mode2_probe_pcs_gpiitx :1;
uint32 rg_hsgmii_mode2_probe_an :1;
uint32 rg_hsgmii_mode2_probe_sync :1;
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
}HAL_rg_hsgmii_mode_probe_sel, *pHAL_rg_hsgmii_mode_probe_sel;

typedef struct {
uint32 rg_hsgmii_mode2_an_done_int :1;
uint32 rg_hsgmii_mode2_rx_sync_done_int :1;
uint32 rg_hsgmii_mode2_an_done_int_clear :1;
uint32 rg_hsgmii_mode2_rx_sync_done_int_clear :1;
uint32 rg_hsgmii_mode2_an_mis_int :1;
uint32 rg_hsgmii_mode2_an_mis_int_clear :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_hsgmii_mode2_an_cl37_timerdone_int :1;
uint32 rg_hsgmii_mode2_an_cl37_timerdone_int_clear :1;
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
}HAL_rg_hsgmii_mode_interrupt, *pHAL_rg_hsgmii_mode_interrupt;

typedef struct {
uint32 rg_force_cur_sgmii_mode_sel :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_force_cur_sgmii_mode :2;
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
}HAL_rg_an_sgmii_mode_force, *pHAL_rg_an_sgmii_mode_force;

typedef struct {
uint32 ro_sgmii_false_carrier_cnt :25;
uint32 ro_err_prop_duration :1;
uint32 ro_an_restart_rst :1;
uint32 ro_mr_np_loaded_rst :1;
uint32 ro_mr_page_rx :1;
uint32 ro_mr_page_rx_tog :1;
uint32 ro_mr_is_basepage :1;
uint32 ro_an_restart_state :1;
}HAL_rg_hsgmii_pcs_state_1, *pHAL_rg_hsgmii_pcs_state_1;

typedef struct {
uint32 ro_an_done :1;
uint32 ro_an_ack :1;
uint32 ro_page_receive :1;
uint32 ro_transmit :1;
uint32 ro_dec_sync :1;
uint32 ro_rx_sync :1;
uint32 ro_sgmii_tx_basepage_int_tog :1;
uint32 ro_sgmii_tx_nextpage_int_tog :1;
uint32 ro_sgmii_re_an_int_tog :1;
uint32 ro_sgmii_remote_fault_occur :1;
uint32 ro_sync_lost :1;
uint32 ro_sync_state :4;
uint32 ro_rx_even :1;
uint32 ro_tx_state :5;
uint32 ro_tx_eee_state :2;
uint32 ro_tx_oset_li_d1 :1;
uint32 ro_an_state :4;
uint32 ro_link_timer_dec :1;
uint32 ro_rx_abiliy_cnt :2;
uint32 ro_ability_match :1;
}HAL_rg_hsgmii_pcs_state_2, *pHAL_rg_hsgmii_pcs_state_2;

typedef struct {
uint32 ro_rx_ack_cnt :2;
uint32 ro_ability_zero :1;
uint32 ro_ack_match :1;
uint32 ro_consist_match :1;
uint32 ro_idle_cnt :3;
uint32 ro_idle_match :1;
uint32 ro_an_sync_status :1;
uint32 ro_lp_ability_zero_timeout_latch :1;
uint32 ro_link_loss_latch :1;
uint32 ro_link_fail_remain :1;
uint32 ro_an_error_remain :1;
uint32 ro_tx_is_nextpage :1;
uint32 ro_k285_int :1;
uint32 ro_k285_int_eee :1;
uint32 ro_k285_int_eee_clr :1;
uint32 ro_rx_state :5;
uint32 ro_d_cfg_int :1;
uint32 ro_d_idle_int :1;
uint32 ro_d_lpi_int :1;
uint32 ro_d_lpi_int_eee :1;
uint32 ro_d_lpi_int_eee_clr :1;
uint32 ro_rx_eee_state :3;
uint32 ro_tx_active :1;
}HAL_rg_hsgmii_pcs_state_3, *pHAL_rg_hsgmii_pcs_state_3;

typedef struct {
uint32 ro_rx_active :1;
uint32 ro_rx_invalid :1;
uint32 ro_dec_sync_int :1;
uint32 ro_dec_err_int :1;
uint32 ro_comma :1;
uint32 ro_kchar_int :1;
uint32 ro_char_err_int :1;
uint32 ro_carr_detect_int :1;
uint32 ro_rx_frame :8;
uint32 ro_data_align :10;
uint32 ro_sync_int :1;
uint32 ro_align_det :4;
uint32 ro_kout :1;
}HAL_rg_hsgmii_pcs_state_4, *pHAL_rg_hsgmii_pcs_state_4;

typedef struct {
uint32 ro_kout_5b_neg :1;
uint32 ro_kout_5b_pos :1;
uint32 ro_kout_3b_neg :1;
uint32 ro_kout_3b_pos :1;
uint32 ro_rd_out :1;
uint32 ro_char_err :1;
uint32 ro_disp_err :1;
uint32 ro_data_out_5b :5;
uint32 ro_data_out_3b :3;
uint32 ro_error_3b :1;
uint32 ro_error_5b :1;
uint32 ro_error_5b_pos :1;
uint32 ro_error_5b_neg :1;
uint32 ro_error_kout_5b :1;
uint32 ro_error_kout_5b_pos :1;
uint32 ro_error_kout_5b_neg :1;
uint32 ro_error_3b_sc :1;
uint32 ro_error_3b_sc_pos :1;
uint32 ro_error_3b_sc_neg :1;
uint32 ro_rd_out_5b :1;
uint32 ro_rd_out_5b_posx :1;
uint32 ro_rd_out_5b_negx :1;
uint32 ro_carrier_reg :1;
uint32 ro_kchar :1;
uint32 ro_tx_oset_li :1;
uint32 ro_curr_disparity :1;
}HAL_rg_hsgmii_pcs_state_5, *pHAL_rg_hsgmii_pcs_state_5;

typedef struct {
uint32 ro_par :19;
uint32 ro_tx_frame :8;
uint32 ro_rd_in :1;
uint32 ro_rd_out_6b :1;
uint32 ro_rd_out_4b :1;
uint32 ro_rd_out_sc :1;
uint32 ro_except :1;
}HAL_rg_hsgmii_pcs_state_6, *pHAL_rg_hsgmii_pcs_state_6;

typedef struct {
uint32 ro_data_out_3b_index :5;
uint32 ro_rd_out_3b_posx :1;
uint32 ro_rd_out_3b_negx :1;
uint32 ro_except5_p :1;
uint32 ro_except5_n :1;
uint32 ro_except_pos :1;
uint32 ro_except_neg :1;
uint32 ro_except_posi :1;
uint32 ro_except_negi :1;
uint32 ro_except_err :1;
uint32 ro_except_okn :1;
uint32 ro_dp5_p :1;
uint32 ro_dp5_n :1;
uint32 ro_dp3_p :1;
uint32 ro_dp3_n :1;
uint32 ro_rd_out_5b_pos :1;
uint32 ro_rd_out_5b_neg :1;
uint32 ro_rd_out_3b_pos :1;
uint32 ro_rd_out_3b_neg :1;
uint32 ro_an_txidle :1;
uint32 ro_an_txena :1;
uint32 ro_cdrctl_relock :1;
uint32 ro_tx_mode_quiet :1;
uint32 ro_tx_lpi_active :1;
uint32 ro_rx_mode_quiet :1;
uint32 ro_rx_lpi_active :1;
uint32 ro_tx_ts_timer_start :1;
uint32 ro_tx_tq_timer_start :1;
}HAL_rg_hsgmii_pcs_state_7, *pHAL_rg_hsgmii_pcs_state_7;

typedef struct {
uint32 ro_tx_tr_timer_start :1;
uint32 ro_rx_tq_timer_start :1;
uint32 ro_rx_tw_timer_start :1;
uint32 ro_rx_wf_timer_start :1;
uint32 ro_rx_wake_err :1;
uint32 ro_dec_err :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 ro_an_mismatch :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_mr_page :16;
}HAL_rg_hsgmii_pcs_state_8, *pHAL_rg_hsgmii_pcs_state_8;

typedef struct {
uint32 ro_fb_in_cnt :32;
}HAL_rg_hsgmii_gpii_state_1, *pHAL_rg_hsgmii_gpii_state_1;

typedef struct {
uint32 ro_fd_in_cnt :32;
}HAL_rg_hsgmii_gpii_state_2, *pHAL_rg_hsgmii_gpii_state_2;

typedef struct {
uint32 ro_fe_in_cnt :32;
}HAL_rg_hsgmii_gpii_state_3, *pHAL_rg_hsgmii_gpii_state_3;

typedef struct {
uint32 ro_id_in_cnt :32;
}HAL_rg_hsgmii_gpii_state_4, *pHAL_rg_hsgmii_gpii_state_4;

typedef struct {
uint32 ro_seq_in_cnt :32;
}HAL_rg_hsgmii_gpii_state_5, *pHAL_rg_hsgmii_gpii_state_5;

typedef struct {
uint32 ro_fbo_in_cnt :32;
}HAL_rg_hsgmii_gpii_state_6, *pHAL_rg_hsgmii_gpii_state_6;

typedef struct {
uint32 ro_fdo_in_cnt :32;
}HAL_rg_hsgmii_gpii_state_7, *pHAL_rg_hsgmii_gpii_state_7;

typedef struct {
uint32 ro_fb_out_cnt :32;
}HAL_rg_hsgmii_gpii_state_8, *pHAL_rg_hsgmii_gpii_state_8;

typedef struct {
uint32 ro_fd_out_cnt :32;
}HAL_rg_hsgmii_gpii_state_9, *pHAL_rg_hsgmii_gpii_state_9;

typedef struct {
uint32 ro_fe_out_cnt :32;
}HAL_rg_hsgmii_gpii_state_10, *pHAL_rg_hsgmii_gpii_state_10;

typedef struct {
uint32 ro_id_out_cnt :32;
}HAL_rg_hsgmii_gpii_state_11, *pHAL_rg_hsgmii_gpii_state_11;

typedef struct {
uint32 ro_seq_out_cnt :32;
}HAL_rg_hsgmii_gpii_state_12, *pHAL_rg_hsgmii_gpii_state_12;

typedef struct {
uint32 ro_rx_gpii_state :4;
uint32 ro_tx_gpii_state :4;
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
}HAL_rg_hsgmii_gpii_state_13, *pHAL_rg_hsgmii_gpii_state_13;

typedef struct {
uint32 ro_sgmii_speed :2;
uint32 ro_sgmii_mode_mux :2;
uint32 ro_sgmii_mode_an :2;
uint32 ro_sgmii_mode :2;
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
}HAL_rg_hsgmii_pcs_state_9, *pHAL_rg_hsgmii_pcs_state_9;

typedef struct {
uint32 ro_hsgmii_mode2_an_done :1;
uint32 ro_hsgmii_mode2_rx_sync :1;
uint32 ro_hsgmii_mode2_an_cl37_timerdone_int :1;
uint32 ro_hsgmii_mode2_an_mis :1;
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
}HAL_rg_hsgmii_pcs_int_state, *pHAL_rg_hsgmii_pcs_int_state;

typedef struct {
uint32 ro_tx_frame_pkt :32;
}HAL_ro_pcs_tx_pkt, *pHAL_ro_pcs_tx_pkt;

typedef struct {
uint32 ro_cur_sgmii_mode :2;
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
}HAL_ro_an_sgmii_mode, *pHAL_ro_an_sgmii_mode;

#endif