#ifndef __hsgmii_csr_pcs_REGS_H__
#define __hsgmii_csr_pcs_REGS_H__

//#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef REG_BASE_C_MODULE
// ----------------- hsgmii_csr_pcs Bit Field Definitions -------------------

#define PACKING
typedef unsigned int UINT32;
typedef unsigned int FIELD;

typedef struct
{
    

        FIELD rg_an_transparent         : 1;
        FIELD rg_code_sync_set_en       : 1;
        FIELD rg_code_sync_set_val      : 1;
        FIELD rg_sgmii_an_block_ack_rxck : 1;
        FIELD rg_sgmii_pat_gen_tx_en_rxck : 1;
        FIELD rg_sgmii_err_prop_ki_sig_los_r : 1;
        FIELD rg_sgmii_err_prop_ki_sync_f : 1;
        FIELD rg_sgmii_err_prop_ko_sig_los_f : 1;
        FIELD rg_sgmii_err_prop_ko_sync_r : 1;
        FIELD rg_gmii_dv_err_option     : 1;
        FIELD rg_an_enable              : 1;
        FIELD rg_unidir_ena             : 1;
        FIELD rg_remote_fault_dis       : 1;
        FIELD rg_an_restart             : 1;
        FIELD rg_gmii_txclk_ena         : 1;
        FIELD rg_rx_clk_ena             : 1;
        FIELD rg_sgmii_1us_timer        : 8;
        FIELD rg_sgmii_send_an_error_en : 1;
        FIELD rg_mr_np_loaded           : 1;
        FIELD rg_sd_sig_det             : 1;
        FIELD rg_sgmii_ena              : 1;
        FIELD rg_sgmii_speed_00         : 1;
        FIELD rg_sgmii_speed_01         : 1;
        FIELD rg_tbi_10b_mode           : 1;
        FIELD rsv_31                    : 1;
    
    
} REG_rg_hsgmii_pcs_ctrol_1, *PREG_rg_hsgmii_pcs_ctrol_1;

typedef struct
{
    

        FIELD rg_mr_np                  : 16;
        FIELD rsv_16                    : 16;
    
    
} REG_rg_hsgmii_pcs_ctrol_2, *PREG_rg_hsgmii_pcs_ctrol_2;

typedef struct
{
    

        FIELD rg_link_ststime           : 20;
        FIELD rsv_20                    : 12;
    
    
} REG_rg_hsgmii_pcs_ctrol_3, *PREG_rg_hsgmii_pcs_ctrol_3;

typedef struct
{
    

        FIELD rg_an_link_timer          : 20;
        FIELD rsv_20                    : 12;
    
    
} REG_rg_hsgmii_pcs_ctrol_4, *PREG_rg_hsgmii_pcs_ctrol_4;

typedef struct
{
    

        FIELD rg_an_ability             : 16;
        FIELD rg_sw_reset_rxck          : 1;
        FIELD rg_sw_reset_txck          : 1;
        FIELD rg_sw_reset_refck         : 1;
        FIELD rsv_19                    : 13;
    
    
} REG_rg_hsgmii_pcs_ctrol_5, *PREG_rg_hsgmii_pcs_ctrol_5;

typedef struct
{
    

        FIELD rg_sgmii_mode2_en         : 1;
        FIELD rg_sgmii_force_rateadapt  : 1;
        FIELD rg_sgmii_force_rateadapt_value : 2;
        FIELD rg_sgmii_tx_enable        : 1;
        FIELD rsv_5                     : 3;
        FIELD rg_sgmii_mac_mode         : 1;
        FIELD rsv_9                     : 3;
        FIELD rg_sgmii_spd_force_1000   : 1;
        FIELD rg_sgmii_spd_force_100    : 1;
        FIELD rg_sgmii_spd_force_10     : 1;
        FIELD rsv_15                    : 17;
    
    
} REG_rg_hsgmii_pcs_ctrol_6, *PREG_rg_hsgmii_pcs_ctrol_6;

typedef struct
{
    

        FIELD rg_force_rxfb_in_lane0    : 1;
        FIELD rg_mode2_gpii_cnt_clr_rx_pcs : 1;
        FIELD rg_mode2_gpii_cnt_clr_rx_pma : 1;
        FIELD rg_mode2_is2500basex_rx   : 1;
        FIELD rg_mode2_is2500basex_tx   : 1;
        FIELD rg_mode2_force_remote_tx  : 1;
        FIELD rg_tbi_10_rx_reverse_mode1 : 1;
        FIELD rg_tbi_20_rx_reverse_mode1 : 1;
        FIELD rg_tbi_rx_reverse_10b_mode1 : 1;
        FIELD rg_tbi_10_tx_reverse_mode1 : 1;
        FIELD rg_tbi_20_tx_reverse_mode1 : 1;
        FIELD rg_tbi_tx_reverse_10b_mode1 : 1;
        FIELD rg_tbi_10_rx_reverse_mode2 : 1;
        FIELD rg_tbi_20_rx_reverse_mode2 : 1;
        FIELD rg_tbi_rx_reverse_10b_mode2 : 1;
        FIELD rg_tbi_10_tx_reverse_mode2 : 1;
        FIELD rg_tbi_20_tx_reverse_mode2 : 1;
        FIELD rg_tbi_tx_reverse_10b_mode2 : 1;
        FIELD rg_gpii_rx_fifo_reverse_mode2 : 1;
        FIELD rg_tbi_20_rx_reverse_all_mode1 : 1;
        FIELD rg_tbi_20_tx_reverse_all_mode1 : 1;
        FIELD rg_tbi_20_rx_reverse_all_mode2 : 1;
        FIELD rg_tbi_20_tx_reverse_all_mode2 : 1;
        FIELD rg_hsgmii_2p5gii_new_or_old : 1;
        FIELD rsv_24                    : 8;
    
    
} REG_rg_hsgmii_pcs_gpii_0, *PREG_rg_hsgmii_pcs_gpii_0;

typedef struct
{
    

        FIELD rg_hsgmii_mode_probe_sel  : 1;
        FIELD rg_hsgmii_mode1_probe_pcs_rx : 1;
        FIELD rg_hsgmii_mode1_probe_pcs_tx : 1;
        FIELD rg_hsgmii_mode1_probe_pcs_gpiirx : 1;
        FIELD rg_hsgmii_mode1_probe_pcs_gpiitx : 1;
        FIELD rg_hsgmii_mode1_probe_an  : 1;
        FIELD rg_hsgmii_mode1_probe_sync : 1;
        FIELD rg_hsgmii_mode2_probe_pcs_rx : 1;
        FIELD rg_hsgmii_mode2_probe_pcs_tx : 1;
        FIELD rg_hsgmii_mode2_probe_pcs_gpiirx : 1;
        FIELD rg_hsgmii_mode2_probe_pcs_gpiitx : 1;
        FIELD rg_hsgmii_mode2_probe_an  : 1;
        FIELD rg_hsgmii_mode2_probe_sync : 1;
        FIELD rsv_13                    : 19;
    
    
} REG_rg_hsgmii_mode_probe_sel, *PREG_rg_hsgmii_mode_probe_sel;

typedef struct
{
    

        FIELD rg_hsgmii_mode2_an_done_int : 1;
        FIELD rg_hsgmii_mode2_rx_sync_done_int : 1;
        FIELD rg_hsgmii_mode2_an_done_int_clear : 1;
        FIELD rg_hsgmii_mode2_rx_sync_done_int_clear : 1;
        FIELD rg_hsgmii_mode2_an_mis_int : 1;
        FIELD rg_hsgmii_mode2_an_mis_int_clear : 1;
        FIELD rsv_6                     : 2;
        FIELD rg_hsgmii_mode2_an_cl37_timerdone_int : 1;
        FIELD rg_hsgmii_mode2_an_cl37_timerdone_int_clear : 1;
        FIELD rg_hsgmii_mode2_remote_fault_occur_int : 1;
        FIELD rg_hsgmii_mode2_remote_fault_occur_int_clear : 1;
        FIELD rsv_12                    : 20;
    
    
} REG_rg_hsgmii_mode_interrupt, *PREG_rg_hsgmii_mode_interrupt;

typedef struct
{
    

        FIELD rg_force_cur_sgmii_mode_sel : 1;
        FIELD rsv_1                     : 3;
        FIELD rg_force_cur_sgmii_mode   : 2;
        FIELD rsv_6                     : 26;
    
    
} REG_rg_an_sgmii_mode_force, *PREG_rg_an_sgmii_mode_force;

typedef struct
{
    

        FIELD ro_sgmii_false_carrier_cnt : 25;
        FIELD ro_err_prop_duration      : 1;
        FIELD ro_an_restart_rst         : 1;
        FIELD ro_mr_np_loaded_rst       : 1;
        FIELD ro_mr_page_rx             : 1;
        FIELD ro_mr_page_rx_tog         : 1;
        FIELD ro_mr_is_basepage         : 1;
        FIELD ro_an_restart_state       : 1;
    
    
} REG_rg_hsgmii_pcs_state_1, *PREG_rg_hsgmii_pcs_state_1;

typedef struct
{
    

        FIELD ro_an_done                : 1;
        FIELD ro_an_ack                 : 1;
        FIELD ro_page_receive           : 1;
        FIELD ro_transmit               : 1;
        FIELD ro_dec_sync               : 1;
        FIELD ro_rx_sync                : 1;
        FIELD ro_sgmii_tx_basepage_int_tog : 1;
        FIELD ro_sgmii_tx_nextpage_int_tog : 1;
        FIELD ro_sgmii_re_an_int_tog    : 1;
        FIELD ro_sgmii_remote_fault_occur : 1;
        FIELD ro_sync_lost              : 1;
        FIELD ro_sync_state             : 4;
        FIELD ro_rx_even                : 1;
        FIELD ro_tx_state               : 5;
        FIELD ro_tx_eee_state           : 2;
        FIELD ro_tx_oset_li_d1          : 1;
        FIELD ro_an_state               : 4;
        FIELD ro_link_timer_dec         : 1;
        FIELD ro_rx_abiliy_cnt          : 2;
        FIELD ro_ability_match          : 1;
    
    
} REG_rg_hsgmii_pcs_state_2, *PREG_rg_hsgmii_pcs_state_2;

typedef struct
{
    

        FIELD ro_rx_ack_cnt             : 2;
        FIELD ro_ability_zero           : 1;
        FIELD ro_ack_match              : 1;
        FIELD ro_consist_match          : 1;
        FIELD ro_idle_cnt               : 3;
        FIELD ro_idle_match             : 1;
        FIELD ro_an_sync_status         : 1;
        FIELD ro_lp_ability_zero_timeout_latch : 1;
        FIELD ro_link_loss_latch        : 1;
        FIELD ro_link_fail_remain       : 1;
        FIELD ro_an_error_remain        : 1;
        FIELD ro_tx_is_nextpage         : 1;
        FIELD ro_k285_int               : 1;
        FIELD ro_k285_int_eee           : 1;
        FIELD ro_k285_int_eee_clr       : 1;
        FIELD ro_rx_state               : 5;
        FIELD ro_d_cfg_int              : 1;
        FIELD ro_d_idle_int             : 1;
        FIELD ro_d_lpi_int              : 1;
        FIELD ro_d_lpi_int_eee          : 1;
        FIELD ro_d_lpi_int_eee_clr      : 1;
        FIELD ro_rx_eee_state           : 3;
        FIELD ro_tx_active              : 1;
    
    
} REG_rg_hsgmii_pcs_state_3, *PREG_rg_hsgmii_pcs_state_3;

typedef struct
{
    

        FIELD ro_rx_active              : 1;
        FIELD ro_rx_invalid             : 1;
        FIELD ro_dec_sync_int           : 1;
        FIELD ro_dec_err_int            : 1;
        FIELD ro_comma                  : 1;
        FIELD ro_kchar_int              : 1;
        FIELD ro_char_err_int           : 1;
        FIELD ro_carr_detect_int        : 1;
        FIELD ro_rx_frame               : 8;
        FIELD ro_data_align             : 10;
        FIELD ro_sync_int               : 1;
        FIELD ro_align_det              : 4;
        FIELD ro_kout                   : 1;
    
    
} REG_rg_hsgmii_pcs_state_4, *PREG_rg_hsgmii_pcs_state_4;

typedef struct
{
    

        FIELD ro_kout_5b_neg            : 1;
        FIELD ro_kout_5b_pos            : 1;
        FIELD ro_kout_3b_neg            : 1;
        FIELD ro_kout_3b_pos            : 1;
        FIELD ro_rd_out                 : 1;
        FIELD ro_char_err               : 1;
        FIELD ro_disp_err               : 1;
        FIELD ro_data_out_5b            : 5;
        FIELD ro_data_out_3b            : 3;
        FIELD ro_error_3b               : 1;
        FIELD ro_error_5b               : 1;
        FIELD ro_error_5b_pos           : 1;
        FIELD ro_error_5b_neg           : 1;
        FIELD ro_error_kout_5b          : 1;
        FIELD ro_error_kout_5b_pos      : 1;
        FIELD ro_error_kout_5b_neg      : 1;
        FIELD ro_error_3b_sc            : 1;
        FIELD ro_error_3b_sc_pos        : 1;
        FIELD ro_error_3b_sc_neg        : 1;
        FIELD ro_rd_out_5b              : 1;
        FIELD ro_rd_out_5b_posx         : 1;
        FIELD ro_rd_out_5b_negx         : 1;
        FIELD ro_carrier_reg            : 1;
        FIELD ro_kchar                  : 1;
        FIELD ro_tx_oset_li             : 1;
        FIELD ro_curr_disparity         : 1;
    
    
} REG_rg_hsgmii_pcs_state_5, *PREG_rg_hsgmii_pcs_state_5;

typedef struct
{
    

        FIELD ro_par                    : 19;
        FIELD ro_tx_frame               : 8;
        FIELD ro_rd_in                  : 1;
        FIELD ro_rd_out_6b              : 1;
        FIELD ro_rd_out_4b              : 1;
        FIELD ro_rd_out_sc              : 1;
        FIELD ro_except                 : 1;
    
    
} REG_rg_hsgmii_pcs_state_6, *PREG_rg_hsgmii_pcs_state_6;

typedef struct
{
    

        FIELD ro_data_out_3b_index      : 5;
        FIELD ro_rd_out_3b_posx         : 1;
        FIELD ro_rd_out_3b_negx         : 1;
        FIELD ro_except5_p              : 1;
        FIELD ro_except5_n              : 1;
        FIELD ro_except_pos             : 1;
        FIELD ro_except_neg             : 1;
        FIELD ro_except_posi            : 1;
        FIELD ro_except_negi            : 1;
        FIELD ro_except_err             : 1;
        FIELD ro_except_okn             : 1;
        FIELD ro_dp5_p                  : 1;
        FIELD ro_dp5_n                  : 1;
        FIELD ro_dp3_p                  : 1;
        FIELD ro_dp3_n                  : 1;
        FIELD ro_rd_out_5b_pos          : 1;
        FIELD ro_rd_out_5b_neg          : 1;
        FIELD ro_rd_out_3b_pos          : 1;
        FIELD ro_rd_out_3b_neg          : 1;
        FIELD ro_an_txidle              : 1;
        FIELD ro_an_txena               : 1;
        FIELD ro_cdrctl_relock          : 1;
        FIELD ro_tx_mode_quiet          : 1;
        FIELD ro_tx_lpi_active          : 1;
        FIELD ro_rx_mode_quiet          : 1;
        FIELD ro_rx_lpi_active          : 1;
        FIELD ro_tx_ts_timer_start      : 1;
        FIELD ro_tx_tq_timer_start      : 1;
    
    
} REG_rg_hsgmii_pcs_state_7, *PREG_rg_hsgmii_pcs_state_7;

typedef struct
{
    

        FIELD ro_tx_tr_timer_start      : 1;
        FIELD ro_rx_tq_timer_start      : 1;
        FIELD ro_rx_tw_timer_start      : 1;
        FIELD ro_rx_wf_timer_start      : 1;
        FIELD ro_rx_wake_err            : 1;
        FIELD ro_dec_err                : 1;
        FIELD rsv_6                     : 2;
        FIELD ro_an_mismatch            : 1;
        FIELD rsv_9                     : 7;
        FIELD ro_mr_page                : 16;
    
    
} REG_rg_hsgmii_pcs_state_8, *PREG_rg_hsgmii_pcs_state_8;

typedef struct
{
    

        FIELD ro_fb_in_cnt              : 32;
    
    
} REG_rg_hsgmii_gpii_state_1, *PREG_rg_hsgmii_gpii_state_1;

typedef struct
{
    

        FIELD ro_fd_in_cnt              : 32;
    
    
} REG_rg_hsgmii_gpii_state_2, *PREG_rg_hsgmii_gpii_state_2;

typedef struct
{
    

        FIELD ro_fe_in_cnt              : 32;
    
    
} REG_rg_hsgmii_gpii_state_3, *PREG_rg_hsgmii_gpii_state_3;

typedef struct
{
    

        FIELD ro_id_in_cnt              : 32;
    
    
} REG_rg_hsgmii_gpii_state_4, *PREG_rg_hsgmii_gpii_state_4;

typedef struct
{
    

        FIELD ro_seq_in_cnt             : 32;
    
    
} REG_rg_hsgmii_gpii_state_5, *PREG_rg_hsgmii_gpii_state_5;

typedef struct
{
    

        FIELD ro_fbo_in_cnt             : 32;
    
    
} REG_rg_hsgmii_gpii_state_6, *PREG_rg_hsgmii_gpii_state_6;

typedef struct
{
    

        FIELD ro_fdo_in_cnt             : 32;
    
    
} REG_rg_hsgmii_gpii_state_7, *PREG_rg_hsgmii_gpii_state_7;

typedef struct
{
    

        FIELD ro_fb_out_cnt             : 32;
    
    
} REG_rg_hsgmii_gpii_state_8, *PREG_rg_hsgmii_gpii_state_8;

typedef struct
{
    

        FIELD ro_fd_out_cnt             : 32;
    
    
} REG_rg_hsgmii_gpii_state_9, *PREG_rg_hsgmii_gpii_state_9;

typedef struct
{
    

        FIELD ro_fe_out_cnt             : 32;
    
    
} REG_rg_hsgmii_gpii_state_10, *PREG_rg_hsgmii_gpii_state_10;

typedef struct
{
    

        FIELD ro_id_out_cnt             : 32;
    
    
} REG_rg_hsgmii_gpii_state_11, *PREG_rg_hsgmii_gpii_state_11;

typedef struct
{
    

        FIELD ro_seq_out_cnt            : 32;
    
    
} REG_rg_hsgmii_gpii_state_12, *PREG_rg_hsgmii_gpii_state_12;

typedef struct
{
    

        FIELD ro_rx_gpii_state          : 4;
        FIELD ro_tx_gpii_state          : 4;
        FIELD rsv_8                     : 24;
    
    
} REG_rg_hsgmii_gpii_state_13, *PREG_rg_hsgmii_gpii_state_13;

typedef struct
{
    

        FIELD ro_sgmii_speed            : 2;
        FIELD ro_sgmii_mode_mux         : 2;
        FIELD ro_sgmii_mode_an          : 2;
        FIELD ro_sgmii_mode             : 2;
        FIELD rsv_8                     : 24;
    
    
} REG_rg_hsgmii_pcs_state_9, *PREG_rg_hsgmii_pcs_state_9;

typedef struct
{
    

        FIELD ro_hsgmii_mode2_an_done   : 1;
        FIELD ro_hsgmii_mode2_rx_sync   : 1;
        FIELD ro_hsgmii_mode2_an_cl37_timerdone_int : 1;
        FIELD ro_hsgmii_mode2_an_mis    : 1;
        FIELD ro_hsgmii_mode2_remote_fault_occur_int : 1;
        FIELD rsv_5                     : 27;
    
    
} REG_rg_hsgmii_pcs_int_state, *PREG_rg_hsgmii_pcs_int_state;

typedef struct
{
    

        FIELD ro_tx_frame_pkt           : 32;
    
    
} REG_ro_pcs_tx_pkt, *PREG_ro_pcs_tx_pkt;

typedef struct
{
    

        FIELD ro_cur_sgmii_mode         : 2;
        FIELD rsv_2                     : 30;
    
    
} REG_ro_an_sgmii_mode, *PREG_ro_an_sgmii_mode;

// ----------------- hsgmii_csr_pcs  Grouping Definitions -------------------
// ----------------- hsgmii_csr_pcs Register Definition -------------------
#if 0
typedef volatile 
{
    UINT32                          rsv_0000[640];    // 0000..09FC
    REG_rg_hsgmii_pcs_ctrol_1       rg_hsgmii_pcs_ctrol_1; // 0A00
    REG_rg_hsgmii_pcs_ctrol_2       rg_hsgmii_pcs_ctrol_2; // 0A04
    REG_rg_hsgmii_pcs_ctrol_3       rg_hsgmii_pcs_ctrol_3; // 0A08
    REG_rg_hsgmii_pcs_ctrol_4       rg_hsgmii_pcs_ctrol_4; // 0A0C
    REG_rg_hsgmii_pcs_ctrol_5       rg_hsgmii_pcs_ctrol_5; // 0A10
    REG_rg_hsgmii_pcs_ctrol_6       rg_hsgmii_pcs_ctrol_6; // 0A14
    REG_rg_hsgmii_pcs_gpii_0        rg_hsgmii_pcs_gpii_0; // 0A18
    REG_rg_hsgmii_mode_probe_sel    rg_hsgmii_mode_probe_sel; // 0A1C
    REG_rg_hsgmii_mode_interrupt    rg_hsgmii_mode_interrupt; // 0A20
    REG_rg_an_sgmii_mode_force      rg_an_sgmii_mode_force; // 0A24
    UINT32                          rsv_0A28[54];     // 0A28..0AFC
    REG_rg_hsgmii_pcs_state_1       rg_hsgmii_pcs_state_1; // 0B00
    REG_rg_hsgmii_pcs_state_2       rg_hsgmii_pcs_state_2; // 0B04
    REG_rg_hsgmii_pcs_state_3       rg_hsgmii_pcs_state_3; // 0B08
    REG_rg_hsgmii_pcs_state_4       rg_hsgmii_pcs_state_4; // 0B0C
    REG_rg_hsgmii_pcs_state_5       rg_hsgmii_pcs_state_5; // 0B10
    REG_rg_hsgmii_pcs_state_6       rg_hsgmii_pcs_state_6; // 0B14
    REG_rg_hsgmii_pcs_state_7       rg_hsgmii_pcs_state_7; // 0B18
    REG_rg_hsgmii_pcs_state_8       rg_hsgmii_pcs_state_8; // 0B1C
    REG_rg_hsgmii_gpii_state_1      rg_hsgmii_gpii_state_1; // 0B20
    UINT32                          rsv_0B24;         // 0B24
    REG_rg_hsgmii_gpii_state_2      rg_hsgmii_gpii_state_2; // 0B28
    REG_rg_hsgmii_gpii_state_3      rg_hsgmii_gpii_state_3; // 0B2C
    REG_rg_hsgmii_gpii_state_4      rg_hsgmii_gpii_state_4; // 0B30
    REG_rg_hsgmii_gpii_state_5      rg_hsgmii_gpii_state_5; // 0B34
    REG_rg_hsgmii_gpii_state_6      rg_hsgmii_gpii_state_6; // 0B38
    REG_rg_hsgmii_gpii_state_7      rg_hsgmii_gpii_state_7; // 0B3C
    REG_rg_hsgmii_gpii_state_8      rg_hsgmii_gpii_state_8; // 0B40
    REG_rg_hsgmii_gpii_state_9      rg_hsgmii_gpii_state_9; // 0B44
    REG_rg_hsgmii_gpii_state_10     rg_hsgmii_gpii_state_10; // 0B48
    REG_rg_hsgmii_gpii_state_11     rg_hsgmii_gpii_state_11; // 0B4C
    REG_rg_hsgmii_gpii_state_12     rg_hsgmii_gpii_state_12; // 0B50
    REG_rg_hsgmii_gpii_state_13     rg_hsgmii_gpii_state_13; // 0B54
    REG_rg_hsgmii_pcs_state_9       rg_hsgmii_pcs_state_9; // 0B58
    REG_rg_hsgmii_pcs_int_state     rg_hsgmii_pcs_int_state; // 0B5C
    REG_ro_pcs_tx_pkt               ro_pcs_tx_pkt;    // 0B60
    REG_ro_an_sgmii_mode            ro_an_sgmii_mode; // 0B64
}hsgmii_csr_pcs_REGS, *Phsgmii_csr_pcs_REGS;

// ---------- hsgmii_csr_pcs Enum Definitions      ----------
// ---------- hsgmii_csr_pcs C Macro Definitions   ----------
extern Phsgmii_csr_pcs_REGS g_hsgmii_csr_pcs_BASE;

#define hsgmii_csr_pcs_BASE                                    (g_hsgmii_csr_pcs_BASE)
#endif

#define _rg_hsgmii_pcs_ctrol_1		0x0A00
#define _rg_hsgmii_pcs_ctrol_2		0x0A04
#define _rg_hsgmii_pcs_ctrol_3		0x0A08
#define _rg_hsgmii_pcs_ctrol_4		0x0A0C
#define _rg_hsgmii_pcs_ctrol_5		0x0A10
#define _rg_hsgmii_pcs_ctrol_6		0x0A14
#define _rg_hsgmii_pcs_gpii_0		0x0A18
#define _rg_hsgmii_mode_probe_sel		0x0A1C
#define _rg_hsgmii_mode_interrupt		0x0A20
#define _rg_an_sgmii_mode_force		0x0A24
#define _rg_hsgmii_pcs_state_1		0x0B00
#define _rg_hsgmii_pcs_state_2		0x0B04
#define _rg_hsgmii_pcs_state_3		0x0B08
#define _rg_hsgmii_pcs_state_4		0x0B0C
#define _rg_hsgmii_pcs_state_5		0x0B10
#define _rg_hsgmii_pcs_state_6		0x0B14
#define _rg_hsgmii_pcs_state_7		0x0B18
#define _rg_hsgmii_pcs_state_8		0x0B1C
#define _rg_hsgmii_gpii_state_1		0x0B20
#define _rg_hsgmii_gpii_state_2		0x0B28
#define _rg_hsgmii_gpii_state_3		0x0B2C
#define _rg_hsgmii_gpii_state_4		0x0B30
#define _rg_hsgmii_gpii_state_5		0x0B34
#define _rg_hsgmii_gpii_state_6		0x0B38
#define _rg_hsgmii_gpii_state_7		0x0B3C
#define _rg_hsgmii_gpii_state_8		0x0B40
#define _rg_hsgmii_gpii_state_9		0x0B44
#define _rg_hsgmii_gpii_state_10		0x0B48
#define _rg_hsgmii_gpii_state_11		0x0B4C
#define _rg_hsgmii_gpii_state_12		0x0B50
#define _rg_hsgmii_gpii_state_13		0x0B54
#define _rg_hsgmii_pcs_state_9		0x0B58
#define _rg_hsgmii_pcs_int_state		0x0B5C
#define _ro_pcs_tx_pkt		0x0B60
#define _ro_an_sgmii_mode		0x0B64

#endif


#define rg_hsgmii_pcs_ctrol_1_FLD_rg_tbi_10b_mode              REG_FLD(1, 30)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_01            REG_FLD(1, 29)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_00            REG_FLD(1, 28)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_ena                 REG_FLD(1, 27)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sd_sig_det                REG_FLD(1, 26)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_mr_np_loaded              REG_FLD(1, 25)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_send_an_error_en    REG_FLD(1, 24)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_1us_timer           REG_FLD(8, 16)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_rx_clk_ena                REG_FLD(1, 15)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_txclk_ena            REG_FLD(1, 14)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_an_restart                REG_FLD(1, 13)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_remote_fault_dis          REG_FLD(1, 12)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_unidir_ena                REG_FLD(1, 11)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_an_enable                 REG_FLD(1, 10)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_dv_err_option        REG_FLD(1, 9)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sync_r  REG_FLD(1, 8)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sig_los_f REG_FLD(1, 7)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sync_f  REG_FLD(1, 6)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sig_los_r REG_FLD(1, 5)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_pat_gen_tx_en_rxck  REG_FLD(1, 4)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_an_block_ack_rxck   REG_FLD(1, 3)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_val         REG_FLD(1, 2)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_en          REG_FLD(1, 1)
#define rg_hsgmii_pcs_ctrol_1_FLD_rg_an_transparent            REG_FLD(1, 0)

#define rg_hsgmii_pcs_ctrol_2_FLD_rg_mr_np                     REG_FLD(16, 0)

#define rg_hsgmii_pcs_ctrol_3_FLD_rg_link_ststime              REG_FLD(20, 0)

#define rg_hsgmii_pcs_ctrol_4_FLD_rg_an_link_timer             REG_FLD(20, 0)

#define rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_refck            REG_FLD(1, 18)
#define rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_txck             REG_FLD(1, 17)
#define rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_rxck             REG_FLD(1, 16)
#define rg_hsgmii_pcs_ctrol_5_FLD_rg_an_ability                REG_FLD(16, 0)

#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_10        REG_FLD(1, 14)
#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_100       REG_FLD(1, 13)
#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_1000      REG_FLD(1, 12)
#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mac_mode            REG_FLD(1, 8)
#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_tx_enable           REG_FLD(1, 4)
#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt_value REG_FLD(2, 2)
#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt     REG_FLD(1, 1)
#define rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mode2_en            REG_FLD(1, 0)

#define rg_hsgmii_pcs_gpii_0_FLD_rg_hsgmii_2p5gii_new_or_old   REG_FLD(1, 23)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode2 REG_FLD(1, 22)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode2 REG_FLD(1, 21)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode1 REG_FLD(1, 20)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode1 REG_FLD(1, 19)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_gpii_rx_fifo_reverse_mode2 REG_FLD(1, 18)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode2   REG_FLD(1, 17)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode2    REG_FLD(1, 16)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode2    REG_FLD(1, 15)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode2   REG_FLD(1, 14)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode2    REG_FLD(1, 13)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode2    REG_FLD(1, 12)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode1   REG_FLD(1, 11)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode1    REG_FLD(1, 10)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode1    REG_FLD(1, 9)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode1   REG_FLD(1, 8)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode1    REG_FLD(1, 7)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode1    REG_FLD(1, 6)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_force_remote_tx      REG_FLD(1, 5)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_tx       REG_FLD(1, 4)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_rx       REG_FLD(1, 3)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pma  REG_FLD(1, 2)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pcs  REG_FLD(1, 1)
#define rg_hsgmii_pcs_gpii_0_FLD_rg_force_rxfb_in_lane0        REG_FLD(1, 0)

#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_sync REG_FLD(1, 12)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_an  REG_FLD(1, 11)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiitx REG_FLD(1, 10)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiirx REG_FLD(1, 9)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_tx REG_FLD(1, 8)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_rx REG_FLD(1, 7)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_sync REG_FLD(1, 6)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_an  REG_FLD(1, 5)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiitx REG_FLD(1, 4)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiirx REG_FLD(1, 3)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_tx REG_FLD(1, 2)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_rx REG_FLD(1, 1)
#define rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode_probe_sel  REG_FLD(1, 0)

#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int_clear REG_FLD(1, 11)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int REG_FLD(1, 10)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int_clear REG_FLD(1, 9)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int REG_FLD(1, 8)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int_clear REG_FLD(1, 5)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int REG_FLD(1, 4)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int_clear REG_FLD(1, 3)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int_clear REG_FLD(1, 2)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int REG_FLD(1, 1)
#define rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int REG_FLD(1, 0)

#define rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode     REG_FLD(2, 4)
#define rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode_sel REG_FLD(1, 0)

#define rg_hsgmii_pcs_state_1_FLD_ro_an_restart_state          REG_FLD(1, 31)
#define rg_hsgmii_pcs_state_1_FLD_ro_mr_is_basepage            REG_FLD(1, 30)
#define rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx_tog            REG_FLD(1, 29)
#define rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx                REG_FLD(1, 28)
#define rg_hsgmii_pcs_state_1_FLD_ro_mr_np_loaded_rst          REG_FLD(1, 27)
#define rg_hsgmii_pcs_state_1_FLD_ro_an_restart_rst            REG_FLD(1, 26)
#define rg_hsgmii_pcs_state_1_FLD_ro_err_prop_duration         REG_FLD(1, 25)
#define rg_hsgmii_pcs_state_1_FLD_ro_sgmii_false_carrier_cnt   REG_FLD(25, 0)

#define rg_hsgmii_pcs_state_2_FLD_ro_ability_match             REG_FLD(1, 31)
#define rg_hsgmii_pcs_state_2_FLD_ro_rx_abiliy_cnt             REG_FLD(2, 29)
#define rg_hsgmii_pcs_state_2_FLD_ro_link_timer_dec            REG_FLD(1, 28)
#define rg_hsgmii_pcs_state_2_FLD_ro_an_state                  REG_FLD(4, 24)
#define rg_hsgmii_pcs_state_2_FLD_ro_tx_oset_li_d1             REG_FLD(1, 23)
#define rg_hsgmii_pcs_state_2_FLD_ro_tx_eee_state              REG_FLD(2, 21)
#define rg_hsgmii_pcs_state_2_FLD_ro_tx_state                  REG_FLD(5, 16)
#define rg_hsgmii_pcs_state_2_FLD_ro_rx_even                   REG_FLD(1, 15)
#define rg_hsgmii_pcs_state_2_FLD_ro_sync_state                REG_FLD(4, 11)
#define rg_hsgmii_pcs_state_2_FLD_ro_sync_lost                 REG_FLD(1, 10)
#define rg_hsgmii_pcs_state_2_FLD_ro_sgmii_remote_fault_occur  REG_FLD(1, 9)
#define rg_hsgmii_pcs_state_2_FLD_ro_sgmii_re_an_int_tog       REG_FLD(1, 8)
#define rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_nextpage_int_tog REG_FLD(1, 7)
#define rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_basepage_int_tog REG_FLD(1, 6)
#define rg_hsgmii_pcs_state_2_FLD_ro_rx_sync                   REG_FLD(1, 5)
#define rg_hsgmii_pcs_state_2_FLD_ro_dec_sync                  REG_FLD(1, 4)
#define rg_hsgmii_pcs_state_2_FLD_ro_transmit                  REG_FLD(1, 3)
#define rg_hsgmii_pcs_state_2_FLD_ro_page_receive              REG_FLD(1, 2)
#define rg_hsgmii_pcs_state_2_FLD_ro_an_ack                    REG_FLD(1, 1)
#define rg_hsgmii_pcs_state_2_FLD_ro_an_done                   REG_FLD(1, 0)

#define rg_hsgmii_pcs_state_3_FLD_ro_tx_active                 REG_FLD(1, 31)
#define rg_hsgmii_pcs_state_3_FLD_ro_rx_eee_state              REG_FLD(3, 28)
#define rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee_clr         REG_FLD(1, 27)
#define rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee             REG_FLD(1, 26)
#define rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int                 REG_FLD(1, 25)
#define rg_hsgmii_pcs_state_3_FLD_ro_d_idle_int                REG_FLD(1, 24)
#define rg_hsgmii_pcs_state_3_FLD_ro_d_cfg_int                 REG_FLD(1, 23)
#define rg_hsgmii_pcs_state_3_FLD_ro_rx_state                  REG_FLD(5, 18)
#define rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee_clr          REG_FLD(1, 17)
#define rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee              REG_FLD(1, 16)
#define rg_hsgmii_pcs_state_3_FLD_ro_k285_int                  REG_FLD(1, 15)
#define rg_hsgmii_pcs_state_3_FLD_ro_tx_is_nextpage            REG_FLD(1, 14)
#define rg_hsgmii_pcs_state_3_FLD_ro_an_error_remain           REG_FLD(1, 13)
#define rg_hsgmii_pcs_state_3_FLD_ro_link_fail_remain          REG_FLD(1, 12)
#define rg_hsgmii_pcs_state_3_FLD_ro_link_loss_latch           REG_FLD(1, 11)
#define rg_hsgmii_pcs_state_3_FLD_ro_lp_ability_zero_timeout_latch REG_FLD(1, 10)
#define rg_hsgmii_pcs_state_3_FLD_ro_an_sync_status            REG_FLD(1, 9)
#define rg_hsgmii_pcs_state_3_FLD_ro_idle_match                REG_FLD(1, 8)
#define rg_hsgmii_pcs_state_3_FLD_ro_idle_cnt                  REG_FLD(3, 5)
#define rg_hsgmii_pcs_state_3_FLD_ro_consist_match             REG_FLD(1, 4)
#define rg_hsgmii_pcs_state_3_FLD_ro_ack_match                 REG_FLD(1, 3)
#define rg_hsgmii_pcs_state_3_FLD_ro_ability_zero              REG_FLD(1, 2)
#define rg_hsgmii_pcs_state_3_FLD_ro_rx_ack_cnt                REG_FLD(2, 0)

#define rg_hsgmii_pcs_state_4_FLD_ro_kout                      REG_FLD(1, 31)
#define rg_hsgmii_pcs_state_4_FLD_ro_align_det                 REG_FLD(4, 27)
#define rg_hsgmii_pcs_state_4_FLD_ro_sync_int                  REG_FLD(1, 26)
#define rg_hsgmii_pcs_state_4_FLD_ro_data_align                REG_FLD(10, 16)
#define rg_hsgmii_pcs_state_4_FLD_ro_rx_frame                  REG_FLD(8, 8)
#define rg_hsgmii_pcs_state_4_FLD_ro_carr_detect_int           REG_FLD(1, 7)
#define rg_hsgmii_pcs_state_4_FLD_ro_char_err_int              REG_FLD(1, 6)
#define rg_hsgmii_pcs_state_4_FLD_ro_kchar_int                 REG_FLD(1, 5)
#define rg_hsgmii_pcs_state_4_FLD_ro_comma                     REG_FLD(1, 4)
#define rg_hsgmii_pcs_state_4_FLD_ro_dec_err_int               REG_FLD(1, 3)
#define rg_hsgmii_pcs_state_4_FLD_ro_dec_sync_int              REG_FLD(1, 2)
#define rg_hsgmii_pcs_state_4_FLD_ro_rx_invalid                REG_FLD(1, 1)
#define rg_hsgmii_pcs_state_4_FLD_ro_rx_active                 REG_FLD(1, 0)

#define rg_hsgmii_pcs_state_5_FLD_ro_curr_disparity            REG_FLD(1, 31)
#define rg_hsgmii_pcs_state_5_FLD_ro_tx_oset_li                REG_FLD(1, 30)
#define rg_hsgmii_pcs_state_5_FLD_ro_kchar                     REG_FLD(1, 29)
#define rg_hsgmii_pcs_state_5_FLD_ro_carrier_reg               REG_FLD(1, 28)
#define rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_negx            REG_FLD(1, 27)
#define rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_posx            REG_FLD(1, 26)
#define rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b                 REG_FLD(1, 25)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_neg           REG_FLD(1, 24)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_pos           REG_FLD(1, 23)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc               REG_FLD(1, 22)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_neg         REG_FLD(1, 21)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_pos         REG_FLD(1, 20)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b             REG_FLD(1, 19)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_5b_neg              REG_FLD(1, 18)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_5b_pos              REG_FLD(1, 17)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_5b                  REG_FLD(1, 16)
#define rg_hsgmii_pcs_state_5_FLD_ro_error_3b                  REG_FLD(1, 15)
#define rg_hsgmii_pcs_state_5_FLD_ro_data_out_3b               REG_FLD(3, 12)
#define rg_hsgmii_pcs_state_5_FLD_ro_data_out_5b               REG_FLD(5, 7)
#define rg_hsgmii_pcs_state_5_FLD_ro_disp_err                  REG_FLD(1, 6)
#define rg_hsgmii_pcs_state_5_FLD_ro_char_err                  REG_FLD(1, 5)
#define rg_hsgmii_pcs_state_5_FLD_ro_rd_out                    REG_FLD(1, 4)
#define rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_pos               REG_FLD(1, 3)
#define rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_neg               REG_FLD(1, 2)
#define rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_pos               REG_FLD(1, 1)
#define rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_neg               REG_FLD(1, 0)

#define rg_hsgmii_pcs_state_6_FLD_ro_except                    REG_FLD(1, 31)
#define rg_hsgmii_pcs_state_6_FLD_ro_rd_out_sc                 REG_FLD(1, 30)
#define rg_hsgmii_pcs_state_6_FLD_ro_rd_out_4b                 REG_FLD(1, 29)
#define rg_hsgmii_pcs_state_6_FLD_ro_rd_out_6b                 REG_FLD(1, 28)
#define rg_hsgmii_pcs_state_6_FLD_ro_rd_in                     REG_FLD(1, 27)
#define rg_hsgmii_pcs_state_6_FLD_ro_tx_frame                  REG_FLD(8, 19)
#define rg_hsgmii_pcs_state_6_FLD_ro_par                       REG_FLD(19, 0)

#define rg_hsgmii_pcs_state_7_FLD_ro_tx_tq_timer_start         REG_FLD(1, 31)
#define rg_hsgmii_pcs_state_7_FLD_ro_tx_ts_timer_start         REG_FLD(1, 30)
#define rg_hsgmii_pcs_state_7_FLD_ro_rx_lpi_active             REG_FLD(1, 29)
#define rg_hsgmii_pcs_state_7_FLD_ro_rx_mode_quiet             REG_FLD(1, 28)
#define rg_hsgmii_pcs_state_7_FLD_ro_tx_lpi_active             REG_FLD(1, 27)
#define rg_hsgmii_pcs_state_7_FLD_ro_tx_mode_quiet             REG_FLD(1, 26)
#define rg_hsgmii_pcs_state_7_FLD_ro_cdrctl_relock             REG_FLD(1, 25)
#define rg_hsgmii_pcs_state_7_FLD_ro_an_txena                  REG_FLD(1, 24)
#define rg_hsgmii_pcs_state_7_FLD_ro_an_txidle                 REG_FLD(1, 23)
#define rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_neg             REG_FLD(1, 22)
#define rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_pos             REG_FLD(1, 21)
#define rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_neg             REG_FLD(1, 20)
#define rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_pos             REG_FLD(1, 19)
#define rg_hsgmii_pcs_state_7_FLD_ro_dp3_n                     REG_FLD(1, 18)
#define rg_hsgmii_pcs_state_7_FLD_ro_dp3_p                     REG_FLD(1, 17)
#define rg_hsgmii_pcs_state_7_FLD_ro_dp5_n                     REG_FLD(1, 16)
#define rg_hsgmii_pcs_state_7_FLD_ro_dp5_p                     REG_FLD(1, 15)
#define rg_hsgmii_pcs_state_7_FLD_ro_except_okn                REG_FLD(1, 14)
#define rg_hsgmii_pcs_state_7_FLD_ro_except_err                REG_FLD(1, 13)
#define rg_hsgmii_pcs_state_7_FLD_ro_except_negi               REG_FLD(1, 12)
#define rg_hsgmii_pcs_state_7_FLD_ro_except_posi               REG_FLD(1, 11)
#define rg_hsgmii_pcs_state_7_FLD_ro_except_neg                REG_FLD(1, 10)
#define rg_hsgmii_pcs_state_7_FLD_ro_except_pos                REG_FLD(1, 9)
#define rg_hsgmii_pcs_state_7_FLD_ro_except5_n                 REG_FLD(1, 8)
#define rg_hsgmii_pcs_state_7_FLD_ro_except5_p                 REG_FLD(1, 7)
#define rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_negx            REG_FLD(1, 6)
#define rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_posx            REG_FLD(1, 5)
#define rg_hsgmii_pcs_state_7_FLD_ro_data_out_3b_index         REG_FLD(5, 0)

#define rg_hsgmii_pcs_state_8_FLD_ro_mr_page                   REG_FLD(16, 16)
#define rg_hsgmii_pcs_state_8_FLD_ro_an_mismatch               REG_FLD(1, 8)
#define rg_hsgmii_pcs_state_8_FLD_ro_dec_err                   REG_FLD(1, 5)
#define rg_hsgmii_pcs_state_8_FLD_ro_rx_wake_err               REG_FLD(1, 4)
#define rg_hsgmii_pcs_state_8_FLD_ro_rx_wf_timer_start         REG_FLD(1, 3)
#define rg_hsgmii_pcs_state_8_FLD_ro_rx_tw_timer_start         REG_FLD(1, 2)
#define rg_hsgmii_pcs_state_8_FLD_ro_rx_tq_timer_start         REG_FLD(1, 1)
#define rg_hsgmii_pcs_state_8_FLD_ro_tx_tr_timer_start         REG_FLD(1, 0)

#define rg_hsgmii_gpii_state_1_FLD_ro_fb_in_cnt                REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_2_FLD_ro_fd_in_cnt                REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_3_FLD_ro_fe_in_cnt                REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_4_FLD_ro_id_in_cnt                REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_5_FLD_ro_seq_in_cnt               REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_6_FLD_ro_fbo_in_cnt               REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_7_FLD_ro_fdo_in_cnt               REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_8_FLD_ro_fb_out_cnt               REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_9_FLD_ro_fd_out_cnt               REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_10_FLD_ro_fe_out_cnt              REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_11_FLD_ro_id_out_cnt              REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_12_FLD_ro_seq_out_cnt             REG_FLD(32, 0)

#define rg_hsgmii_gpii_state_13_FLD_ro_tx_gpii_state           REG_FLD(4, 4)
#define rg_hsgmii_gpii_state_13_FLD_ro_rx_gpii_state           REG_FLD(4, 0)

#define rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode                REG_FLD(2, 6)
#define rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_an             REG_FLD(2, 4)
#define rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_mux            REG_FLD(2, 2)
#define rg_hsgmii_pcs_state_9_FLD_ro_sgmii_speed               REG_FLD(2, 0)

#define rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_remote_fault_occur_int REG_FLD(1, 4)
#define rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_mis     REG_FLD(1, 3)
#define rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_cl37_timerdone_int REG_FLD(1, 2)
#define rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_rx_sync    REG_FLD(1, 1)
#define rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_done    REG_FLD(1, 0)

#define ro_pcs_tx_pkt_FLD_ro_tx_frame_pkt                      REG_FLD(32, 0)

#define ro_an_sgmii_mode_FLD_ro_cur_sgmii_mode                 REG_FLD(2, 0)

#define rg_hsgmii_pcs_ctrol_1_GET_rg_tbi_10b_mode(reg32)       REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_tbi_10b_mode, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_speed_01(reg32)     REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_01, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_speed_00(reg32)     REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_00, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_ena(reg32)          REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_ena, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sd_sig_det(reg32)         REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sd_sig_det, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_mr_np_loaded(reg32)       REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_mr_np_loaded, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_send_an_error_en(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_send_an_error_en, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_1us_timer(reg32)    REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_1us_timer, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_rx_clk_ena(reg32)         REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_rx_clk_ena, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_gmii_txclk_ena(reg32)     REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_txclk_ena, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_an_restart(reg32)         REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_restart, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_remote_fault_dis(reg32)   REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_remote_fault_dis, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_unidir_ena(reg32)         REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_unidir_ena, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_an_enable(reg32)          REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_enable, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_gmii_dv_err_option(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_dv_err_option, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_err_prop_ko_sync_r(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sync_r, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_err_prop_ko_sig_los_f(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sig_los_f, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_err_prop_ki_sync_f(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sync_f, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_err_prop_ki_sig_los_r(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sig_los_r, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_pat_gen_tx_en_rxck(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_pat_gen_tx_en_rxck, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_sgmii_an_block_ack_rxck(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_an_block_ack_rxck, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_code_sync_set_val(reg32)  REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_val, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_code_sync_set_en(reg32)   REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_en, (reg32))
#define rg_hsgmii_pcs_ctrol_1_GET_rg_an_transparent(reg32)     REG_FLD_GET(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_transparent, (reg32))

#define rg_hsgmii_pcs_ctrol_2_GET_rg_mr_np(reg32)              REG_FLD_GET(rg_hsgmii_pcs_ctrol_2_FLD_rg_mr_np, (reg32))

#define rg_hsgmii_pcs_ctrol_3_GET_rg_link_ststime(reg32)       REG_FLD_GET(rg_hsgmii_pcs_ctrol_3_FLD_rg_link_ststime, (reg32))

#define rg_hsgmii_pcs_ctrol_4_GET_rg_an_link_timer(reg32)      REG_FLD_GET(rg_hsgmii_pcs_ctrol_4_FLD_rg_an_link_timer, (reg32))

#define rg_hsgmii_pcs_ctrol_5_GET_rg_sw_reset_refck(reg32)     REG_FLD_GET(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_refck, (reg32))
#define rg_hsgmii_pcs_ctrol_5_GET_rg_sw_reset_txck(reg32)      REG_FLD_GET(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_txck, (reg32))
#define rg_hsgmii_pcs_ctrol_5_GET_rg_sw_reset_rxck(reg32)      REG_FLD_GET(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_rxck, (reg32))
#define rg_hsgmii_pcs_ctrol_5_GET_rg_an_ability(reg32)         REG_FLD_GET(rg_hsgmii_pcs_ctrol_5_FLD_rg_an_ability, (reg32))

#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_spd_force_10(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_10, (reg32))
#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_spd_force_100(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_100, (reg32))
#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_spd_force_1000(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_1000, (reg32))
#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_mac_mode(reg32)     REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mac_mode, (reg32))
#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_tx_enable(reg32)    REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_tx_enable, (reg32))
#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_force_rateadapt_value(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt_value, (reg32))
#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_force_rateadapt(reg32) REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt, (reg32))
#define rg_hsgmii_pcs_ctrol_6_GET_rg_sgmii_mode2_en(reg32)     REG_FLD_GET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mode2_en, (reg32))

#define rg_hsgmii_pcs_gpii_0_GET_rg_hsgmii_2p5gii_new_or_old(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_hsgmii_2p5gii_new_or_old, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_tx_reverse_all_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_rx_reverse_all_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_tx_reverse_all_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_rx_reverse_all_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_gpii_rx_fifo_reverse_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_gpii_rx_fifo_reverse_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_tx_reverse_10b_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_tx_reverse_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_10_tx_reverse_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_rx_reverse_10b_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_rx_reverse_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_10_rx_reverse_mode2(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode2, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_tx_reverse_10b_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_tx_reverse_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_10_tx_reverse_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_rx_reverse_10b_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_20_rx_reverse_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_tbi_10_rx_reverse_mode1(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode1, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_mode2_force_remote_tx(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_force_remote_tx, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_mode2_is2500basex_tx(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_tx, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_mode2_is2500basex_rx(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_rx, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_mode2_gpii_cnt_clr_rx_pma(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pma, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_mode2_gpii_cnt_clr_rx_pcs(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pcs, (reg32))
#define rg_hsgmii_pcs_gpii_0_GET_rg_force_rxfb_in_lane0(reg32) REG_FLD_GET(rg_hsgmii_pcs_gpii_0_FLD_rg_force_rxfb_in_lane0, (reg32))

#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode2_probe_sync(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_sync, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode2_probe_an(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_an, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode2_probe_pcs_gpiitx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiitx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode2_probe_pcs_gpiirx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiirx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode2_probe_pcs_tx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_tx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode2_probe_pcs_rx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_rx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode1_probe_sync(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_sync, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode1_probe_an(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_an, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode1_probe_pcs_gpiitx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiitx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode1_probe_pcs_gpiirx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiirx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode1_probe_pcs_tx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_tx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode1_probe_pcs_rx(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_rx, (reg32))
#define rg_hsgmii_mode_probe_sel_GET_rg_hsgmii_mode_probe_sel(reg32) REG_FLD_GET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode_probe_sel, (reg32))

#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_remote_fault_occur_int_clear(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int_clear, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_remote_fault_occur_int(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_an_cl37_timerdone_int_clear(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int_clear, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_an_cl37_timerdone_int(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_an_mis_int_clear(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int_clear, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_an_mis_int(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_rx_sync_done_int_clear(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int_clear, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_an_done_int_clear(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int_clear, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_rx_sync_done_int(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int, (reg32))
#define rg_hsgmii_mode_interrupt_GET_rg_hsgmii_mode2_an_done_int(reg32) REG_FLD_GET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int, (reg32))

#define rg_an_sgmii_mode_force_GET_rg_force_cur_sgmii_mode(reg32) REG_FLD_GET(rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode, (reg32))
#define rg_an_sgmii_mode_force_GET_rg_force_cur_sgmii_mode_sel(reg32) REG_FLD_GET(rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode_sel, (reg32))

#define rg_hsgmii_pcs_state_1_GET_ro_an_restart_state(reg32)   REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_an_restart_state, (reg32))
#define rg_hsgmii_pcs_state_1_GET_ro_mr_is_basepage(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_mr_is_basepage, (reg32))
#define rg_hsgmii_pcs_state_1_GET_ro_mr_page_rx_tog(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx_tog, (reg32))
#define rg_hsgmii_pcs_state_1_GET_ro_mr_page_rx(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx, (reg32))
#define rg_hsgmii_pcs_state_1_GET_ro_mr_np_loaded_rst(reg32)   REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_mr_np_loaded_rst, (reg32))
#define rg_hsgmii_pcs_state_1_GET_ro_an_restart_rst(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_an_restart_rst, (reg32))
#define rg_hsgmii_pcs_state_1_GET_ro_err_prop_duration(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_err_prop_duration, (reg32))
#define rg_hsgmii_pcs_state_1_GET_ro_sgmii_false_carrier_cnt(reg32) REG_FLD_GET(rg_hsgmii_pcs_state_1_FLD_ro_sgmii_false_carrier_cnt, (reg32))

#define rg_hsgmii_pcs_state_2_GET_ro_ability_match(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_ability_match, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_rx_abiliy_cnt(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_rx_abiliy_cnt, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_link_timer_dec(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_link_timer_dec, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_an_state(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_an_state, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_tx_oset_li_d1(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_tx_oset_li_d1, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_tx_eee_state(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_tx_eee_state, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_tx_state(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_tx_state, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_rx_even(reg32)            REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_rx_even, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_sync_state(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_sync_state, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_sync_lost(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_sync_lost, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_sgmii_remote_fault_occur(reg32) REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_remote_fault_occur, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_sgmii_re_an_int_tog(reg32) REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_re_an_int_tog, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_sgmii_tx_nextpage_int_tog(reg32) REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_nextpage_int_tog, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_sgmii_tx_basepage_int_tog(reg32) REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_basepage_int_tog, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_rx_sync(reg32)            REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_rx_sync, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_dec_sync(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_dec_sync, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_transmit(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_transmit, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_page_receive(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_page_receive, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_an_ack(reg32)             REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_an_ack, (reg32))
#define rg_hsgmii_pcs_state_2_GET_ro_an_done(reg32)            REG_FLD_GET(rg_hsgmii_pcs_state_2_FLD_ro_an_done, (reg32))

#define rg_hsgmii_pcs_state_3_GET_ro_tx_active(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_tx_active, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_rx_eee_state(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_rx_eee_state, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_d_lpi_int_eee_clr(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee_clr, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_d_lpi_int_eee(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_d_lpi_int(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_d_idle_int(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_d_idle_int, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_d_cfg_int(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_d_cfg_int, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_rx_state(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_rx_state, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_k285_int_eee_clr(reg32)   REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee_clr, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_k285_int_eee(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_k285_int(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_k285_int, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_tx_is_nextpage(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_tx_is_nextpage, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_an_error_remain(reg32)    REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_an_error_remain, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_link_fail_remain(reg32)   REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_link_fail_remain, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_link_loss_latch(reg32)    REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_link_loss_latch, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_lp_ability_zero_timeout_latch(reg32) REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_lp_ability_zero_timeout_latch, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_an_sync_status(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_an_sync_status, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_idle_match(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_idle_match, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_idle_cnt(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_idle_cnt, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_consist_match(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_consist_match, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_ack_match(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_ack_match, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_ability_zero(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_ability_zero, (reg32))
#define rg_hsgmii_pcs_state_3_GET_ro_rx_ack_cnt(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_3_FLD_ro_rx_ack_cnt, (reg32))

#define rg_hsgmii_pcs_state_4_GET_ro_kout(reg32)               REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_kout, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_align_det(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_align_det, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_sync_int(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_sync_int, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_data_align(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_data_align, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_rx_frame(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_rx_frame, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_carr_detect_int(reg32)    REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_carr_detect_int, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_char_err_int(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_char_err_int, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_kchar_int(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_kchar_int, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_comma(reg32)              REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_comma, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_dec_err_int(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_dec_err_int, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_dec_sync_int(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_dec_sync_int, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_rx_invalid(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_rx_invalid, (reg32))
#define rg_hsgmii_pcs_state_4_GET_ro_rx_active(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_4_FLD_ro_rx_active, (reg32))

#define rg_hsgmii_pcs_state_5_GET_ro_curr_disparity(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_curr_disparity, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_tx_oset_li(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_tx_oset_li, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_kchar(reg32)              REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_kchar, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_carrier_reg(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_carrier_reg, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_rd_out_5b_negx(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_negx, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_rd_out_5b_posx(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_posx, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_rd_out_5b(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_3b_sc_neg(reg32)    REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_neg, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_3b_sc_pos(reg32)    REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_pos, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_3b_sc(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_kout_5b_neg(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_neg, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_kout_5b_pos(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_pos, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_kout_5b(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_5b_neg(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_5b_neg, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_5b_pos(reg32)       REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_5b_pos, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_5b(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_5b, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_error_3b(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_data_out_3b(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_data_out_3b, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_data_out_5b(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_data_out_5b, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_disp_err(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_disp_err, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_char_err(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_char_err, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_rd_out(reg32)             REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_kout_3b_pos(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_pos, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_kout_3b_neg(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_neg, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_kout_5b_pos(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_pos, (reg32))
#define rg_hsgmii_pcs_state_5_GET_ro_kout_5b_neg(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_neg, (reg32))

#define rg_hsgmii_pcs_state_6_GET_ro_except(reg32)             REG_FLD_GET(rg_hsgmii_pcs_state_6_FLD_ro_except, (reg32))
#define rg_hsgmii_pcs_state_6_GET_ro_rd_out_sc(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_sc, (reg32))
#define rg_hsgmii_pcs_state_6_GET_ro_rd_out_4b(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_4b, (reg32))
#define rg_hsgmii_pcs_state_6_GET_ro_rd_out_6b(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_6b, (reg32))
#define rg_hsgmii_pcs_state_6_GET_ro_rd_in(reg32)              REG_FLD_GET(rg_hsgmii_pcs_state_6_FLD_ro_rd_in, (reg32))
#define rg_hsgmii_pcs_state_6_GET_ro_tx_frame(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_6_FLD_ro_tx_frame, (reg32))
#define rg_hsgmii_pcs_state_6_GET_ro_par(reg32)                REG_FLD_GET(rg_hsgmii_pcs_state_6_FLD_ro_par, (reg32))

#define rg_hsgmii_pcs_state_7_GET_ro_tx_tq_timer_start(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_tx_tq_timer_start, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_tx_ts_timer_start(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_tx_ts_timer_start, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rx_lpi_active(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rx_lpi_active, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rx_mode_quiet(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rx_mode_quiet, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_tx_lpi_active(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_tx_lpi_active, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_tx_mode_quiet(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_tx_mode_quiet, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_cdrctl_relock(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_cdrctl_relock, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_an_txena(reg32)           REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_an_txena, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_an_txidle(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_an_txidle, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rd_out_3b_neg(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_neg, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rd_out_3b_pos(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_pos, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rd_out_5b_neg(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_neg, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rd_out_5b_pos(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_pos, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_dp3_n(reg32)              REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_dp3_n, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_dp3_p(reg32)              REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_dp3_p, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_dp5_n(reg32)              REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_dp5_n, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_dp5_p(reg32)              REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_dp5_p, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except_okn(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except_okn, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except_err(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except_err, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except_negi(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except_negi, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except_posi(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except_posi, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except_neg(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except_neg, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except_pos(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except_pos, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except5_n(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except5_n, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_except5_p(reg32)          REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_except5_p, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rd_out_3b_negx(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_negx, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_rd_out_3b_posx(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_posx, (reg32))
#define rg_hsgmii_pcs_state_7_GET_ro_data_out_3b_index(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_7_FLD_ro_data_out_3b_index, (reg32))

#define rg_hsgmii_pcs_state_8_GET_ro_mr_page(reg32)            REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_mr_page, (reg32))
#define rg_hsgmii_pcs_state_8_GET_ro_an_mismatch(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_an_mismatch, (reg32))
#define rg_hsgmii_pcs_state_8_GET_ro_dec_err(reg32)            REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_dec_err, (reg32))
#define rg_hsgmii_pcs_state_8_GET_ro_rx_wake_err(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_rx_wake_err, (reg32))
#define rg_hsgmii_pcs_state_8_GET_ro_rx_wf_timer_start(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_rx_wf_timer_start, (reg32))
#define rg_hsgmii_pcs_state_8_GET_ro_rx_tw_timer_start(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_rx_tw_timer_start, (reg32))
#define rg_hsgmii_pcs_state_8_GET_ro_rx_tq_timer_start(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_rx_tq_timer_start, (reg32))
#define rg_hsgmii_pcs_state_8_GET_ro_tx_tr_timer_start(reg32)  REG_FLD_GET(rg_hsgmii_pcs_state_8_FLD_ro_tx_tr_timer_start, (reg32))

#define rg_hsgmii_gpii_state_1_GET_ro_fb_in_cnt(reg32)         REG_FLD_GET(rg_hsgmii_gpii_state_1_FLD_ro_fb_in_cnt, (reg32))

#define rg_hsgmii_gpii_state_2_GET_ro_fd_in_cnt(reg32)         REG_FLD_GET(rg_hsgmii_gpii_state_2_FLD_ro_fd_in_cnt, (reg32))

#define rg_hsgmii_gpii_state_3_GET_ro_fe_in_cnt(reg32)         REG_FLD_GET(rg_hsgmii_gpii_state_3_FLD_ro_fe_in_cnt, (reg32))

#define rg_hsgmii_gpii_state_4_GET_ro_id_in_cnt(reg32)         REG_FLD_GET(rg_hsgmii_gpii_state_4_FLD_ro_id_in_cnt, (reg32))

#define rg_hsgmii_gpii_state_5_GET_ro_seq_in_cnt(reg32)        REG_FLD_GET(rg_hsgmii_gpii_state_5_FLD_ro_seq_in_cnt, (reg32))

#define rg_hsgmii_gpii_state_6_GET_ro_fbo_in_cnt(reg32)        REG_FLD_GET(rg_hsgmii_gpii_state_6_FLD_ro_fbo_in_cnt, (reg32))

#define rg_hsgmii_gpii_state_7_GET_ro_fdo_in_cnt(reg32)        REG_FLD_GET(rg_hsgmii_gpii_state_7_FLD_ro_fdo_in_cnt, (reg32))

#define rg_hsgmii_gpii_state_8_GET_ro_fb_out_cnt(reg32)        REG_FLD_GET(rg_hsgmii_gpii_state_8_FLD_ro_fb_out_cnt, (reg32))

#define rg_hsgmii_gpii_state_9_GET_ro_fd_out_cnt(reg32)        REG_FLD_GET(rg_hsgmii_gpii_state_9_FLD_ro_fd_out_cnt, (reg32))

#define rg_hsgmii_gpii_state_10_GET_ro_fe_out_cnt(reg32)       REG_FLD_GET(rg_hsgmii_gpii_state_10_FLD_ro_fe_out_cnt, (reg32))

#define rg_hsgmii_gpii_state_11_GET_ro_id_out_cnt(reg32)       REG_FLD_GET(rg_hsgmii_gpii_state_11_FLD_ro_id_out_cnt, (reg32))

#define rg_hsgmii_gpii_state_12_GET_ro_seq_out_cnt(reg32)      REG_FLD_GET(rg_hsgmii_gpii_state_12_FLD_ro_seq_out_cnt, (reg32))

#define rg_hsgmii_gpii_state_13_GET_ro_tx_gpii_state(reg32)    REG_FLD_GET(rg_hsgmii_gpii_state_13_FLD_ro_tx_gpii_state, (reg32))
#define rg_hsgmii_gpii_state_13_GET_ro_rx_gpii_state(reg32)    REG_FLD_GET(rg_hsgmii_gpii_state_13_FLD_ro_rx_gpii_state, (reg32))

#define rg_hsgmii_pcs_state_9_GET_ro_sgmii_mode(reg32)         REG_FLD_GET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode, (reg32))
#define rg_hsgmii_pcs_state_9_GET_ro_sgmii_mode_an(reg32)      REG_FLD_GET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_an, (reg32))
#define rg_hsgmii_pcs_state_9_GET_ro_sgmii_mode_mux(reg32)     REG_FLD_GET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_mux, (reg32))
#define rg_hsgmii_pcs_state_9_GET_ro_sgmii_speed(reg32)        REG_FLD_GET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_speed, (reg32))

#define rg_hsgmii_pcs_int_state_GET_ro_hsgmii_mode2_remote_fault_occur_int(reg32) REG_FLD_GET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_remote_fault_occur_int, (reg32))
#define rg_hsgmii_pcs_int_state_GET_ro_hsgmii_mode2_an_mis(reg32) REG_FLD_GET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_mis, (reg32))
#define rg_hsgmii_pcs_int_state_GET_ro_hsgmii_mode2_an_cl37_timerdone_int(reg32) REG_FLD_GET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_cl37_timerdone_int, (reg32))
#define rg_hsgmii_pcs_int_state_GET_ro_hsgmii_mode2_rx_sync(reg32) REG_FLD_GET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_rx_sync, (reg32))
#define rg_hsgmii_pcs_int_state_GET_ro_hsgmii_mode2_an_done(reg32) REG_FLD_GET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_done, (reg32))

#define ro_pcs_tx_pkt_GET_ro_tx_frame_pkt(reg32)               REG_FLD_GET(ro_pcs_tx_pkt_FLD_ro_tx_frame_pkt, (reg32))

#define ro_an_sgmii_mode_GET_ro_cur_sgmii_mode(reg32)          REG_FLD_GET(ro_an_sgmii_mode_FLD_ro_cur_sgmii_mode, (reg32))

#define rg_hsgmii_pcs_ctrol_1_SET_rg_tbi_10b_mode(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_tbi_10b_mode, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_speed_01(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_01, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_speed_00(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_00, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_ena(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_ena, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sd_sig_det(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sd_sig_det, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_mr_np_loaded(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_mr_np_loaded, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_send_an_error_en(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_send_an_error_en, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_1us_timer(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_1us_timer, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_rx_clk_ena(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_rx_clk_ena, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_gmii_txclk_ena(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_txclk_ena, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_an_restart(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_restart, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_remote_fault_dis(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_remote_fault_dis, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_unidir_ena(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_unidir_ena, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_an_enable(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_enable, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_gmii_dv_err_option(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_dv_err_option, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_err_prop_ko_sync_r(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sync_r, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_err_prop_ko_sig_los_f(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sig_los_f, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_err_prop_ki_sync_f(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sync_f, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_err_prop_ki_sig_los_r(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sig_los_r, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_pat_gen_tx_en_rxck(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_pat_gen_tx_en_rxck, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_sgmii_an_block_ack_rxck(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_an_block_ack_rxck, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_code_sync_set_val(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_val, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_code_sync_set_en(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_en, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_1_SET_rg_an_transparent(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_transparent, (reg32), (val))

#define rg_hsgmii_pcs_ctrol_2_SET_rg_mr_np(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_ctrol_2_FLD_rg_mr_np, (reg32), (val))

#define rg_hsgmii_pcs_ctrol_3_SET_rg_link_ststime(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_ctrol_3_FLD_rg_link_ststime, (reg32), (val))

#define rg_hsgmii_pcs_ctrol_4_SET_rg_an_link_timer(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_4_FLD_rg_an_link_timer, (reg32), (val))

#define rg_hsgmii_pcs_ctrol_5_SET_rg_sw_reset_refck(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_refck, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_5_SET_rg_sw_reset_txck(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_txck, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_5_SET_rg_sw_reset_rxck(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_rxck, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_5_SET_rg_an_ability(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_ctrol_5_FLD_rg_an_ability, (reg32), (val))

#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_spd_force_10(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_10, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_spd_force_100(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_100, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_spd_force_1000(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_1000, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_mac_mode(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mac_mode, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_tx_enable(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_tx_enable, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_force_rateadapt_value(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt_value, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_force_rateadapt(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt, (reg32), (val))
#define rg_hsgmii_pcs_ctrol_6_SET_rg_sgmii_mode2_en(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mode2_en, (reg32), (val))

#define rg_hsgmii_pcs_gpii_0_SET_rg_hsgmii_2p5gii_new_or_old(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_hsgmii_2p5gii_new_or_old, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_tx_reverse_all_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_rx_reverse_all_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_tx_reverse_all_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_rx_reverse_all_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_gpii_rx_fifo_reverse_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_gpii_rx_fifo_reverse_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_tx_reverse_10b_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_tx_reverse_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_10_tx_reverse_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_rx_reverse_10b_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_rx_reverse_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_10_rx_reverse_mode2(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode2, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_tx_reverse_10b_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_tx_reverse_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_10_tx_reverse_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_rx_reverse_10b_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_20_rx_reverse_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_tbi_10_rx_reverse_mode1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode1, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_mode2_force_remote_tx(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_force_remote_tx, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_mode2_is2500basex_tx(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_tx, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_mode2_is2500basex_rx(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_rx, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_mode2_gpii_cnt_clr_rx_pma(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pma, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_mode2_gpii_cnt_clr_rx_pcs(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pcs, (reg32), (val))
#define rg_hsgmii_pcs_gpii_0_SET_rg_force_rxfb_in_lane0(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_gpii_0_FLD_rg_force_rxfb_in_lane0, (reg32), (val))

#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode2_probe_sync(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_sync, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode2_probe_an(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_an, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode2_probe_pcs_gpiitx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiitx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode2_probe_pcs_gpiirx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiirx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode2_probe_pcs_tx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_tx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode2_probe_pcs_rx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_rx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode1_probe_sync(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_sync, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode1_probe_an(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_an, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode1_probe_pcs_gpiitx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiitx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode1_probe_pcs_gpiirx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiirx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode1_probe_pcs_tx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_tx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode1_probe_pcs_rx(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_rx, (reg32), (val))
#define rg_hsgmii_mode_probe_sel_SET_rg_hsgmii_mode_probe_sel(reg32, val) REG_FLD_SET(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode_probe_sel, (reg32), (val))

#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_remote_fault_occur_int_clear(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int_clear, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_remote_fault_occur_int(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_an_cl37_timerdone_int_clear(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int_clear, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_an_cl37_timerdone_int(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_an_mis_int_clear(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int_clear, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_an_mis_int(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_rx_sync_done_int_clear(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int_clear, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_an_done_int_clear(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int_clear, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_rx_sync_done_int(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int, (reg32), (val))
#define rg_hsgmii_mode_interrupt_SET_rg_hsgmii_mode2_an_done_int(reg32, val) REG_FLD_SET(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int, (reg32), (val))

#define rg_an_sgmii_mode_force_SET_rg_force_cur_sgmii_mode(reg32, val) REG_FLD_SET(rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode, (reg32), (val))
#define rg_an_sgmii_mode_force_SET_rg_force_cur_sgmii_mode_sel(reg32, val) REG_FLD_SET(rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode_sel, (reg32), (val))

#define rg_hsgmii_pcs_state_1_SET_ro_an_restart_state(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_an_restart_state, (reg32), (val))
#define rg_hsgmii_pcs_state_1_SET_ro_mr_is_basepage(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_mr_is_basepage, (reg32), (val))
#define rg_hsgmii_pcs_state_1_SET_ro_mr_page_rx_tog(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx_tog, (reg32), (val))
#define rg_hsgmii_pcs_state_1_SET_ro_mr_page_rx(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx, (reg32), (val))
#define rg_hsgmii_pcs_state_1_SET_ro_mr_np_loaded_rst(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_mr_np_loaded_rst, (reg32), (val))
#define rg_hsgmii_pcs_state_1_SET_ro_an_restart_rst(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_an_restart_rst, (reg32), (val))
#define rg_hsgmii_pcs_state_1_SET_ro_err_prop_duration(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_err_prop_duration, (reg32), (val))
#define rg_hsgmii_pcs_state_1_SET_ro_sgmii_false_carrier_cnt(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_1_FLD_ro_sgmii_false_carrier_cnt, (reg32), (val))

#define rg_hsgmii_pcs_state_2_SET_ro_ability_match(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_ability_match, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_rx_abiliy_cnt(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_rx_abiliy_cnt, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_link_timer_dec(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_link_timer_dec, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_an_state(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_an_state, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_tx_oset_li_d1(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_tx_oset_li_d1, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_tx_eee_state(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_tx_eee_state, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_tx_state(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_tx_state, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_rx_even(reg32, val)       REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_rx_even, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_sync_state(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_sync_state, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_sync_lost(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_sync_lost, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_sgmii_remote_fault_occur(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_remote_fault_occur, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_sgmii_re_an_int_tog(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_re_an_int_tog, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_sgmii_tx_nextpage_int_tog(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_nextpage_int_tog, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_sgmii_tx_basepage_int_tog(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_basepage_int_tog, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_rx_sync(reg32, val)       REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_rx_sync, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_dec_sync(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_dec_sync, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_transmit(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_transmit, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_page_receive(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_page_receive, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_an_ack(reg32, val)        REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_an_ack, (reg32), (val))
#define rg_hsgmii_pcs_state_2_SET_ro_an_done(reg32, val)       REG_FLD_SET(rg_hsgmii_pcs_state_2_FLD_ro_an_done, (reg32), (val))

#define rg_hsgmii_pcs_state_3_SET_ro_tx_active(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_tx_active, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_rx_eee_state(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_rx_eee_state, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_d_lpi_int_eee_clr(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee_clr, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_d_lpi_int_eee(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_d_lpi_int(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_d_idle_int(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_d_idle_int, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_d_cfg_int(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_d_cfg_int, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_rx_state(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_rx_state, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_k285_int_eee_clr(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee_clr, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_k285_int_eee(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_k285_int(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_k285_int, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_tx_is_nextpage(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_tx_is_nextpage, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_an_error_remain(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_an_error_remain, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_link_fail_remain(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_link_fail_remain, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_link_loss_latch(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_link_loss_latch, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_lp_ability_zero_timeout_latch(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_lp_ability_zero_timeout_latch, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_an_sync_status(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_an_sync_status, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_idle_match(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_idle_match, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_idle_cnt(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_idle_cnt, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_consist_match(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_consist_match, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_ack_match(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_ack_match, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_ability_zero(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_ability_zero, (reg32), (val))
#define rg_hsgmii_pcs_state_3_SET_ro_rx_ack_cnt(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_3_FLD_ro_rx_ack_cnt, (reg32), (val))

#define rg_hsgmii_pcs_state_4_SET_ro_kout(reg32, val)          REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_kout, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_align_det(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_align_det, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_sync_int(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_sync_int, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_data_align(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_data_align, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_rx_frame(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_rx_frame, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_carr_detect_int(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_carr_detect_int, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_char_err_int(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_char_err_int, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_kchar_int(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_kchar_int, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_comma(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_comma, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_dec_err_int(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_dec_err_int, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_dec_sync_int(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_dec_sync_int, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_rx_invalid(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_rx_invalid, (reg32), (val))
#define rg_hsgmii_pcs_state_4_SET_ro_rx_active(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_4_FLD_ro_rx_active, (reg32), (val))

#define rg_hsgmii_pcs_state_5_SET_ro_curr_disparity(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_curr_disparity, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_tx_oset_li(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_tx_oset_li, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_kchar(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_kchar, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_carrier_reg(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_carrier_reg, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_rd_out_5b_negx(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_negx, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_rd_out_5b_posx(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_posx, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_rd_out_5b(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_3b_sc_neg(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_neg, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_3b_sc_pos(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_3b_sc(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_kout_5b_neg(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_neg, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_kout_5b_pos(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_kout_5b(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_5b_neg(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_5b_neg, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_5b_pos(reg32, val)  REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_5b_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_5b(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_5b, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_error_3b(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_error_3b, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_data_out_3b(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_data_out_3b, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_data_out_5b(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_data_out_5b, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_disp_err(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_disp_err, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_char_err(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_char_err, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_rd_out(reg32, val)        REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_rd_out, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_kout_3b_pos(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_kout_3b_neg(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_neg, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_kout_5b_pos(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_5_SET_ro_kout_5b_neg(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_neg, (reg32), (val))

#define rg_hsgmii_pcs_state_6_SET_ro_except(reg32, val)        REG_FLD_SET(rg_hsgmii_pcs_state_6_FLD_ro_except, (reg32), (val))
#define rg_hsgmii_pcs_state_6_SET_ro_rd_out_sc(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_sc, (reg32), (val))
#define rg_hsgmii_pcs_state_6_SET_ro_rd_out_4b(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_4b, (reg32), (val))
#define rg_hsgmii_pcs_state_6_SET_ro_rd_out_6b(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_6b, (reg32), (val))
#define rg_hsgmii_pcs_state_6_SET_ro_rd_in(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_state_6_FLD_ro_rd_in, (reg32), (val))
#define rg_hsgmii_pcs_state_6_SET_ro_tx_frame(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_6_FLD_ro_tx_frame, (reg32), (val))
#define rg_hsgmii_pcs_state_6_SET_ro_par(reg32, val)           REG_FLD_SET(rg_hsgmii_pcs_state_6_FLD_ro_par, (reg32), (val))

#define rg_hsgmii_pcs_state_7_SET_ro_tx_tq_timer_start(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_tx_tq_timer_start, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_tx_ts_timer_start(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_tx_ts_timer_start, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rx_lpi_active(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rx_lpi_active, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rx_mode_quiet(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rx_mode_quiet, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_tx_lpi_active(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_tx_lpi_active, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_tx_mode_quiet(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_tx_mode_quiet, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_cdrctl_relock(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_cdrctl_relock, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_an_txena(reg32, val)      REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_an_txena, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_an_txidle(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_an_txidle, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rd_out_3b_neg(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_neg, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rd_out_3b_pos(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rd_out_5b_neg(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_neg, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rd_out_5b_pos(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_dp3_n(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_dp3_n, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_dp3_p(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_dp3_p, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_dp5_n(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_dp5_n, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_dp5_p(reg32, val)         REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_dp5_p, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except_okn(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except_okn, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except_err(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except_err, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except_negi(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except_negi, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except_posi(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except_posi, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except_neg(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except_neg, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except_pos(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except_pos, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except5_n(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except5_n, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_except5_p(reg32, val)     REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_except5_p, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rd_out_3b_negx(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_negx, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_rd_out_3b_posx(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_posx, (reg32), (val))
#define rg_hsgmii_pcs_state_7_SET_ro_data_out_3b_index(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_7_FLD_ro_data_out_3b_index, (reg32), (val))

#define rg_hsgmii_pcs_state_8_SET_ro_mr_page(reg32, val)       REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_mr_page, (reg32), (val))
#define rg_hsgmii_pcs_state_8_SET_ro_an_mismatch(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_an_mismatch, (reg32), (val))
#define rg_hsgmii_pcs_state_8_SET_ro_dec_err(reg32, val)       REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_dec_err, (reg32), (val))
#define rg_hsgmii_pcs_state_8_SET_ro_rx_wake_err(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_rx_wake_err, (reg32), (val))
#define rg_hsgmii_pcs_state_8_SET_ro_rx_wf_timer_start(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_rx_wf_timer_start, (reg32), (val))
#define rg_hsgmii_pcs_state_8_SET_ro_rx_tw_timer_start(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_rx_tw_timer_start, (reg32), (val))
#define rg_hsgmii_pcs_state_8_SET_ro_rx_tq_timer_start(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_rx_tq_timer_start, (reg32), (val))
#define rg_hsgmii_pcs_state_8_SET_ro_tx_tr_timer_start(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_8_FLD_ro_tx_tr_timer_start, (reg32), (val))

#define rg_hsgmii_gpii_state_1_SET_ro_fb_in_cnt(reg32, val)    REG_FLD_SET(rg_hsgmii_gpii_state_1_FLD_ro_fb_in_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_2_SET_ro_fd_in_cnt(reg32, val)    REG_FLD_SET(rg_hsgmii_gpii_state_2_FLD_ro_fd_in_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_3_SET_ro_fe_in_cnt(reg32, val)    REG_FLD_SET(rg_hsgmii_gpii_state_3_FLD_ro_fe_in_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_4_SET_ro_id_in_cnt(reg32, val)    REG_FLD_SET(rg_hsgmii_gpii_state_4_FLD_ro_id_in_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_5_SET_ro_seq_in_cnt(reg32, val)   REG_FLD_SET(rg_hsgmii_gpii_state_5_FLD_ro_seq_in_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_6_SET_ro_fbo_in_cnt(reg32, val)   REG_FLD_SET(rg_hsgmii_gpii_state_6_FLD_ro_fbo_in_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_7_SET_ro_fdo_in_cnt(reg32, val)   REG_FLD_SET(rg_hsgmii_gpii_state_7_FLD_ro_fdo_in_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_8_SET_ro_fb_out_cnt(reg32, val)   REG_FLD_SET(rg_hsgmii_gpii_state_8_FLD_ro_fb_out_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_9_SET_ro_fd_out_cnt(reg32, val)   REG_FLD_SET(rg_hsgmii_gpii_state_9_FLD_ro_fd_out_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_10_SET_ro_fe_out_cnt(reg32, val)  REG_FLD_SET(rg_hsgmii_gpii_state_10_FLD_ro_fe_out_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_11_SET_ro_id_out_cnt(reg32, val)  REG_FLD_SET(rg_hsgmii_gpii_state_11_FLD_ro_id_out_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_12_SET_ro_seq_out_cnt(reg32, val) REG_FLD_SET(rg_hsgmii_gpii_state_12_FLD_ro_seq_out_cnt, (reg32), (val))

#define rg_hsgmii_gpii_state_13_SET_ro_tx_gpii_state(reg32, val) REG_FLD_SET(rg_hsgmii_gpii_state_13_FLD_ro_tx_gpii_state, (reg32), (val))
#define rg_hsgmii_gpii_state_13_SET_ro_rx_gpii_state(reg32, val) REG_FLD_SET(rg_hsgmii_gpii_state_13_FLD_ro_rx_gpii_state, (reg32), (val))

#define rg_hsgmii_pcs_state_9_SET_ro_sgmii_mode(reg32, val)    REG_FLD_SET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode, (reg32), (val))
#define rg_hsgmii_pcs_state_9_SET_ro_sgmii_mode_an(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_an, (reg32), (val))
#define rg_hsgmii_pcs_state_9_SET_ro_sgmii_mode_mux(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_mux, (reg32), (val))
#define rg_hsgmii_pcs_state_9_SET_ro_sgmii_speed(reg32, val)   REG_FLD_SET(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_speed, (reg32), (val))

#define rg_hsgmii_pcs_int_state_SET_ro_hsgmii_mode2_remote_fault_occur_int(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_remote_fault_occur_int, (reg32), (val))
#define rg_hsgmii_pcs_int_state_SET_ro_hsgmii_mode2_an_mis(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_mis, (reg32), (val))
#define rg_hsgmii_pcs_int_state_SET_ro_hsgmii_mode2_an_cl37_timerdone_int(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_cl37_timerdone_int, (reg32), (val))
#define rg_hsgmii_pcs_int_state_SET_ro_hsgmii_mode2_rx_sync(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_rx_sync, (reg32), (val))
#define rg_hsgmii_pcs_int_state_SET_ro_hsgmii_mode2_an_done(reg32, val) REG_FLD_SET(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_done, (reg32), (val))

#define ro_pcs_tx_pkt_SET_ro_tx_frame_pkt(reg32, val)          REG_FLD_SET(ro_pcs_tx_pkt_FLD_ro_tx_frame_pkt, (reg32), (val))

#define ro_an_sgmii_mode_SET_ro_cur_sgmii_mode(reg32, val)     REG_FLD_SET(ro_an_sgmii_mode_FLD_ro_cur_sgmii_mode, (reg32), (val))

#define rg_hsgmii_pcs_ctrol_1_VAL_rg_tbi_10b_mode(val)         REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_tbi_10b_mode, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_speed_01(val)       REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_01, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_speed_00(val)       REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_speed_00, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_ena(val)            REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_ena, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sd_sig_det(val)           REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sd_sig_det, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_mr_np_loaded(val)         REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_mr_np_loaded, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_send_an_error_en(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_send_an_error_en, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_1us_timer(val)      REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_1us_timer, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_rx_clk_ena(val)           REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_rx_clk_ena, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_gmii_txclk_ena(val)       REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_txclk_ena, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_an_restart(val)           REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_restart, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_remote_fault_dis(val)     REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_remote_fault_dis, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_unidir_ena(val)           REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_unidir_ena, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_an_enable(val)            REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_enable, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_gmii_dv_err_option(val)   REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_gmii_dv_err_option, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_err_prop_ko_sync_r(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sync_r, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_err_prop_ko_sig_los_f(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ko_sig_los_f, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_err_prop_ki_sync_f(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sync_f, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_err_prop_ki_sig_los_r(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_err_prop_ki_sig_los_r, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_pat_gen_tx_en_rxck(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_pat_gen_tx_en_rxck, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_sgmii_an_block_ack_rxck(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_sgmii_an_block_ack_rxck, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_code_sync_set_val(val)    REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_val, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_code_sync_set_en(val)     REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_code_sync_set_en, (val))
#define rg_hsgmii_pcs_ctrol_1_VAL_rg_an_transparent(val)       REG_FLD_VAL(rg_hsgmii_pcs_ctrol_1_FLD_rg_an_transparent, (val))

#define rg_hsgmii_pcs_ctrol_2_VAL_rg_mr_np(val)                REG_FLD_VAL(rg_hsgmii_pcs_ctrol_2_FLD_rg_mr_np, (val))

#define rg_hsgmii_pcs_ctrol_3_VAL_rg_link_ststime(val)         REG_FLD_VAL(rg_hsgmii_pcs_ctrol_3_FLD_rg_link_ststime, (val))

#define rg_hsgmii_pcs_ctrol_4_VAL_rg_an_link_timer(val)        REG_FLD_VAL(rg_hsgmii_pcs_ctrol_4_FLD_rg_an_link_timer, (val))

#define rg_hsgmii_pcs_ctrol_5_VAL_rg_sw_reset_refck(val)       REG_FLD_VAL(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_refck, (val))
#define rg_hsgmii_pcs_ctrol_5_VAL_rg_sw_reset_txck(val)        REG_FLD_VAL(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_txck, (val))
#define rg_hsgmii_pcs_ctrol_5_VAL_rg_sw_reset_rxck(val)        REG_FLD_VAL(rg_hsgmii_pcs_ctrol_5_FLD_rg_sw_reset_rxck, (val))
#define rg_hsgmii_pcs_ctrol_5_VAL_rg_an_ability(val)           REG_FLD_VAL(rg_hsgmii_pcs_ctrol_5_FLD_rg_an_ability, (val))

#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_spd_force_10(val)   REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_10, (val))
#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_spd_force_100(val)  REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_100, (val))
#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_spd_force_1000(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_spd_force_1000, (val))
#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_mac_mode(val)       REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mac_mode, (val))
#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_tx_enable(val)      REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_tx_enable, (val))
#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_force_rateadapt_value(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt_value, (val))
#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_force_rateadapt(val) REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_force_rateadapt, (val))
#define rg_hsgmii_pcs_ctrol_6_VAL_rg_sgmii_mode2_en(val)       REG_FLD_VAL(rg_hsgmii_pcs_ctrol_6_FLD_rg_sgmii_mode2_en, (val))

#define rg_hsgmii_pcs_gpii_0_VAL_rg_hsgmii_2p5gii_new_or_old(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_hsgmii_2p5gii_new_or_old, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_tx_reverse_all_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_rx_reverse_all_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_tx_reverse_all_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_all_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_rx_reverse_all_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_all_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_gpii_rx_fifo_reverse_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_gpii_rx_fifo_reverse_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_tx_reverse_10b_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_tx_reverse_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_10_tx_reverse_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_rx_reverse_10b_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_rx_reverse_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_10_rx_reverse_mode2(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode2, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_tx_reverse_10b_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_tx_reverse_10b_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_tx_reverse_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_tx_reverse_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_10_tx_reverse_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_tx_reverse_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_rx_reverse_10b_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_rx_reverse_10b_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_20_rx_reverse_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_20_rx_reverse_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_tbi_10_rx_reverse_mode1(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_tbi_10_rx_reverse_mode1, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_mode2_force_remote_tx(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_force_remote_tx, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_mode2_is2500basex_tx(val)  REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_tx, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_mode2_is2500basex_rx(val)  REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_is2500basex_rx, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_mode2_gpii_cnt_clr_rx_pma(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pma, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_mode2_gpii_cnt_clr_rx_pcs(val) REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_mode2_gpii_cnt_clr_rx_pcs, (val))
#define rg_hsgmii_pcs_gpii_0_VAL_rg_force_rxfb_in_lane0(val)   REG_FLD_VAL(rg_hsgmii_pcs_gpii_0_FLD_rg_force_rxfb_in_lane0, (val))

#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode2_probe_sync(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_sync, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode2_probe_an(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_an, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode2_probe_pcs_gpiitx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiitx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode2_probe_pcs_gpiirx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_gpiirx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode2_probe_pcs_tx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_tx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode2_probe_pcs_rx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode2_probe_pcs_rx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode1_probe_sync(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_sync, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode1_probe_an(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_an, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode1_probe_pcs_gpiitx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiitx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode1_probe_pcs_gpiirx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_gpiirx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode1_probe_pcs_tx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_tx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode1_probe_pcs_rx(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode1_probe_pcs_rx, (val))
#define rg_hsgmii_mode_probe_sel_VAL_rg_hsgmii_mode_probe_sel(val) REG_FLD_VAL(rg_hsgmii_mode_probe_sel_FLD_rg_hsgmii_mode_probe_sel, (val))

#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_remote_fault_occur_int_clear(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int_clear, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_remote_fault_occur_int(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_remote_fault_occur_int, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_an_cl37_timerdone_int_clear(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int_clear, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_an_cl37_timerdone_int(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_cl37_timerdone_int, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_an_mis_int_clear(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int_clear, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_an_mis_int(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_mis_int, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_rx_sync_done_int_clear(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int_clear, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_an_done_int_clear(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int_clear, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_rx_sync_done_int(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_rx_sync_done_int, (val))
#define rg_hsgmii_mode_interrupt_VAL_rg_hsgmii_mode2_an_done_int(val) REG_FLD_VAL(rg_hsgmii_mode_interrupt_FLD_rg_hsgmii_mode2_an_done_int, (val))

#define rg_an_sgmii_mode_force_VAL_rg_force_cur_sgmii_mode(val) REG_FLD_VAL(rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode, (val))
#define rg_an_sgmii_mode_force_VAL_rg_force_cur_sgmii_mode_sel(val) REG_FLD_VAL(rg_an_sgmii_mode_force_FLD_rg_force_cur_sgmii_mode_sel, (val))

#define rg_hsgmii_pcs_state_1_VAL_ro_an_restart_state(val)     REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_an_restart_state, (val))
#define rg_hsgmii_pcs_state_1_VAL_ro_mr_is_basepage(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_mr_is_basepage, (val))
#define rg_hsgmii_pcs_state_1_VAL_ro_mr_page_rx_tog(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx_tog, (val))
#define rg_hsgmii_pcs_state_1_VAL_ro_mr_page_rx(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_mr_page_rx, (val))
#define rg_hsgmii_pcs_state_1_VAL_ro_mr_np_loaded_rst(val)     REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_mr_np_loaded_rst, (val))
#define rg_hsgmii_pcs_state_1_VAL_ro_an_restart_rst(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_an_restart_rst, (val))
#define rg_hsgmii_pcs_state_1_VAL_ro_err_prop_duration(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_err_prop_duration, (val))
#define rg_hsgmii_pcs_state_1_VAL_ro_sgmii_false_carrier_cnt(val) REG_FLD_VAL(rg_hsgmii_pcs_state_1_FLD_ro_sgmii_false_carrier_cnt, (val))

#define rg_hsgmii_pcs_state_2_VAL_ro_ability_match(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_ability_match, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_rx_abiliy_cnt(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_rx_abiliy_cnt, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_link_timer_dec(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_link_timer_dec, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_an_state(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_an_state, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_tx_oset_li_d1(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_tx_oset_li_d1, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_tx_eee_state(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_tx_eee_state, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_tx_state(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_tx_state, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_rx_even(val)              REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_rx_even, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_sync_state(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_sync_state, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_sync_lost(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_sync_lost, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_sgmii_remote_fault_occur(val) REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_remote_fault_occur, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_sgmii_re_an_int_tog(val)  REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_re_an_int_tog, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_sgmii_tx_nextpage_int_tog(val) REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_nextpage_int_tog, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_sgmii_tx_basepage_int_tog(val) REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_sgmii_tx_basepage_int_tog, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_rx_sync(val)              REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_rx_sync, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_dec_sync(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_dec_sync, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_transmit(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_transmit, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_page_receive(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_page_receive, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_an_ack(val)               REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_an_ack, (val))
#define rg_hsgmii_pcs_state_2_VAL_ro_an_done(val)              REG_FLD_VAL(rg_hsgmii_pcs_state_2_FLD_ro_an_done, (val))

#define rg_hsgmii_pcs_state_3_VAL_ro_tx_active(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_tx_active, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_rx_eee_state(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_rx_eee_state, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_d_lpi_int_eee_clr(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee_clr, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_d_lpi_int_eee(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int_eee, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_d_lpi_int(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_d_lpi_int, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_d_idle_int(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_d_idle_int, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_d_cfg_int(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_d_cfg_int, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_rx_state(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_rx_state, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_k285_int_eee_clr(val)     REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee_clr, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_k285_int_eee(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_k285_int_eee, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_k285_int(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_k285_int, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_tx_is_nextpage(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_tx_is_nextpage, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_an_error_remain(val)      REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_an_error_remain, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_link_fail_remain(val)     REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_link_fail_remain, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_link_loss_latch(val)      REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_link_loss_latch, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_lp_ability_zero_timeout_latch(val) REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_lp_ability_zero_timeout_latch, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_an_sync_status(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_an_sync_status, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_idle_match(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_idle_match, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_idle_cnt(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_idle_cnt, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_consist_match(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_consist_match, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_ack_match(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_ack_match, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_ability_zero(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_ability_zero, (val))
#define rg_hsgmii_pcs_state_3_VAL_ro_rx_ack_cnt(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_3_FLD_ro_rx_ack_cnt, (val))

#define rg_hsgmii_pcs_state_4_VAL_ro_kout(val)                 REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_kout, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_align_det(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_align_det, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_sync_int(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_sync_int, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_data_align(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_data_align, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_rx_frame(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_rx_frame, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_carr_detect_int(val)      REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_carr_detect_int, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_char_err_int(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_char_err_int, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_kchar_int(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_kchar_int, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_comma(val)                REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_comma, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_dec_err_int(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_dec_err_int, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_dec_sync_int(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_dec_sync_int, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_rx_invalid(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_rx_invalid, (val))
#define rg_hsgmii_pcs_state_4_VAL_ro_rx_active(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_4_FLD_ro_rx_active, (val))

#define rg_hsgmii_pcs_state_5_VAL_ro_curr_disparity(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_curr_disparity, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_tx_oset_li(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_tx_oset_li, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_kchar(val)                REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_kchar, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_carrier_reg(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_carrier_reg, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_rd_out_5b_negx(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_negx, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_rd_out_5b_posx(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b_posx, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_rd_out_5b(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_rd_out_5b, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_3b_sc_neg(val)      REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_neg, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_3b_sc_pos(val)      REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc_pos, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_3b_sc(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_3b_sc, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_kout_5b_neg(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_neg, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_kout_5b_pos(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b_pos, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_kout_5b(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_kout_5b, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_5b_neg(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_5b_neg, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_5b_pos(val)         REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_5b_pos, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_5b(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_5b, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_error_3b(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_error_3b, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_data_out_3b(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_data_out_3b, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_data_out_5b(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_data_out_5b, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_disp_err(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_disp_err, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_char_err(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_char_err, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_rd_out(val)               REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_rd_out, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_kout_3b_pos(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_pos, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_kout_3b_neg(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_kout_3b_neg, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_kout_5b_pos(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_pos, (val))
#define rg_hsgmii_pcs_state_5_VAL_ro_kout_5b_neg(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_5_FLD_ro_kout_5b_neg, (val))

#define rg_hsgmii_pcs_state_6_VAL_ro_except(val)               REG_FLD_VAL(rg_hsgmii_pcs_state_6_FLD_ro_except, (val))
#define rg_hsgmii_pcs_state_6_VAL_ro_rd_out_sc(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_sc, (val))
#define rg_hsgmii_pcs_state_6_VAL_ro_rd_out_4b(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_4b, (val))
#define rg_hsgmii_pcs_state_6_VAL_ro_rd_out_6b(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_6_FLD_ro_rd_out_6b, (val))
#define rg_hsgmii_pcs_state_6_VAL_ro_rd_in(val)                REG_FLD_VAL(rg_hsgmii_pcs_state_6_FLD_ro_rd_in, (val))
#define rg_hsgmii_pcs_state_6_VAL_ro_tx_frame(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_6_FLD_ro_tx_frame, (val))
#define rg_hsgmii_pcs_state_6_VAL_ro_par(val)                  REG_FLD_VAL(rg_hsgmii_pcs_state_6_FLD_ro_par, (val))

#define rg_hsgmii_pcs_state_7_VAL_ro_tx_tq_timer_start(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_tx_tq_timer_start, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_tx_ts_timer_start(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_tx_ts_timer_start, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rx_lpi_active(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rx_lpi_active, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rx_mode_quiet(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rx_mode_quiet, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_tx_lpi_active(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_tx_lpi_active, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_tx_mode_quiet(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_tx_mode_quiet, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_cdrctl_relock(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_cdrctl_relock, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_an_txena(val)             REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_an_txena, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_an_txidle(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_an_txidle, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rd_out_3b_neg(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_neg, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rd_out_3b_pos(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_pos, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rd_out_5b_neg(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_neg, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rd_out_5b_pos(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_5b_pos, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_dp3_n(val)                REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_dp3_n, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_dp3_p(val)                REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_dp3_p, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_dp5_n(val)                REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_dp5_n, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_dp5_p(val)                REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_dp5_p, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except_okn(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except_okn, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except_err(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except_err, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except_negi(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except_negi, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except_posi(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except_posi, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except_neg(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except_neg, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except_pos(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except_pos, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except5_n(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except5_n, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_except5_p(val)            REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_except5_p, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rd_out_3b_negx(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_negx, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_rd_out_3b_posx(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_rd_out_3b_posx, (val))
#define rg_hsgmii_pcs_state_7_VAL_ro_data_out_3b_index(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_7_FLD_ro_data_out_3b_index, (val))

#define rg_hsgmii_pcs_state_8_VAL_ro_mr_page(val)              REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_mr_page, (val))
#define rg_hsgmii_pcs_state_8_VAL_ro_an_mismatch(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_an_mismatch, (val))
#define rg_hsgmii_pcs_state_8_VAL_ro_dec_err(val)              REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_dec_err, (val))
#define rg_hsgmii_pcs_state_8_VAL_ro_rx_wake_err(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_rx_wake_err, (val))
#define rg_hsgmii_pcs_state_8_VAL_ro_rx_wf_timer_start(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_rx_wf_timer_start, (val))
#define rg_hsgmii_pcs_state_8_VAL_ro_rx_tw_timer_start(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_rx_tw_timer_start, (val))
#define rg_hsgmii_pcs_state_8_VAL_ro_rx_tq_timer_start(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_rx_tq_timer_start, (val))
#define rg_hsgmii_pcs_state_8_VAL_ro_tx_tr_timer_start(val)    REG_FLD_VAL(rg_hsgmii_pcs_state_8_FLD_ro_tx_tr_timer_start, (val))

#define rg_hsgmii_gpii_state_1_VAL_ro_fb_in_cnt(val)           REG_FLD_VAL(rg_hsgmii_gpii_state_1_FLD_ro_fb_in_cnt, (val))

#define rg_hsgmii_gpii_state_2_VAL_ro_fd_in_cnt(val)           REG_FLD_VAL(rg_hsgmii_gpii_state_2_FLD_ro_fd_in_cnt, (val))

#define rg_hsgmii_gpii_state_3_VAL_ro_fe_in_cnt(val)           REG_FLD_VAL(rg_hsgmii_gpii_state_3_FLD_ro_fe_in_cnt, (val))

#define rg_hsgmii_gpii_state_4_VAL_ro_id_in_cnt(val)           REG_FLD_VAL(rg_hsgmii_gpii_state_4_FLD_ro_id_in_cnt, (val))

#define rg_hsgmii_gpii_state_5_VAL_ro_seq_in_cnt(val)          REG_FLD_VAL(rg_hsgmii_gpii_state_5_FLD_ro_seq_in_cnt, (val))

#define rg_hsgmii_gpii_state_6_VAL_ro_fbo_in_cnt(val)          REG_FLD_VAL(rg_hsgmii_gpii_state_6_FLD_ro_fbo_in_cnt, (val))

#define rg_hsgmii_gpii_state_7_VAL_ro_fdo_in_cnt(val)          REG_FLD_VAL(rg_hsgmii_gpii_state_7_FLD_ro_fdo_in_cnt, (val))

#define rg_hsgmii_gpii_state_8_VAL_ro_fb_out_cnt(val)          REG_FLD_VAL(rg_hsgmii_gpii_state_8_FLD_ro_fb_out_cnt, (val))

#define rg_hsgmii_gpii_state_9_VAL_ro_fd_out_cnt(val)          REG_FLD_VAL(rg_hsgmii_gpii_state_9_FLD_ro_fd_out_cnt, (val))

#define rg_hsgmii_gpii_state_10_VAL_ro_fe_out_cnt(val)         REG_FLD_VAL(rg_hsgmii_gpii_state_10_FLD_ro_fe_out_cnt, (val))

#define rg_hsgmii_gpii_state_11_VAL_ro_id_out_cnt(val)         REG_FLD_VAL(rg_hsgmii_gpii_state_11_FLD_ro_id_out_cnt, (val))

#define rg_hsgmii_gpii_state_12_VAL_ro_seq_out_cnt(val)        REG_FLD_VAL(rg_hsgmii_gpii_state_12_FLD_ro_seq_out_cnt, (val))

#define rg_hsgmii_gpii_state_13_VAL_ro_tx_gpii_state(val)      REG_FLD_VAL(rg_hsgmii_gpii_state_13_FLD_ro_tx_gpii_state, (val))
#define rg_hsgmii_gpii_state_13_VAL_ro_rx_gpii_state(val)      REG_FLD_VAL(rg_hsgmii_gpii_state_13_FLD_ro_rx_gpii_state, (val))

#define rg_hsgmii_pcs_state_9_VAL_ro_sgmii_mode(val)           REG_FLD_VAL(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode, (val))
#define rg_hsgmii_pcs_state_9_VAL_ro_sgmii_mode_an(val)        REG_FLD_VAL(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_an, (val))
#define rg_hsgmii_pcs_state_9_VAL_ro_sgmii_mode_mux(val)       REG_FLD_VAL(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_mode_mux, (val))
#define rg_hsgmii_pcs_state_9_VAL_ro_sgmii_speed(val)          REG_FLD_VAL(rg_hsgmii_pcs_state_9_FLD_ro_sgmii_speed, (val))

#define rg_hsgmii_pcs_int_state_VAL_ro_hsgmii_mode2_remote_fault_occur_int(val) REG_FLD_VAL(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_remote_fault_occur_int, (val))
#define rg_hsgmii_pcs_int_state_VAL_ro_hsgmii_mode2_an_mis(val) REG_FLD_VAL(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_mis, (val))
#define rg_hsgmii_pcs_int_state_VAL_ro_hsgmii_mode2_an_cl37_timerdone_int(val) REG_FLD_VAL(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_cl37_timerdone_int, (val))
#define rg_hsgmii_pcs_int_state_VAL_ro_hsgmii_mode2_rx_sync(val) REG_FLD_VAL(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_rx_sync, (val))
#define rg_hsgmii_pcs_int_state_VAL_ro_hsgmii_mode2_an_done(val) REG_FLD_VAL(rg_hsgmii_pcs_int_state_FLD_ro_hsgmii_mode2_an_done, (val))

#define ro_pcs_tx_pkt_VAL_ro_tx_frame_pkt(val)                 REG_FLD_VAL(ro_pcs_tx_pkt_FLD_ro_tx_frame_pkt, (val))

#define ro_an_sgmii_mode_VAL_ro_cur_sgmii_mode(val)            REG_FLD_VAL(ro_an_sgmii_mode_FLD_ro_cur_sgmii_mode, (val))

#ifdef __cplusplus
}
#endif

#endif // __hsgmii_csr_pcs_REGS_H__

