#ifndef RATE_ADAPTION_CSR_XGMII_HAL_REG
#define RATE_ADAPTION_CSR_XGMII_HAL_REG

typedef struct {
uint32 rg_rate_adapt_tx_en :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_rate_adapt_rx_en :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_rate_adapt_tx_replicate_mode :1;
uint32 rg_rate_adapt_rx_replicate_mode :1;
uint32 rg_rate_adapt_tx_usxgmii_pch_mode :1;
uint32 rg_rate_adapt_rx_usxgmii_pch_mode :1;
uint32 rg_rate_adapt_tx_usxgmii_eop_mode :1;
uint32 rg_rate_adapt_tx_sgmii_eop_mode :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_rate_adapt_tx_afifo_wr_clr :1;
uint32 rg_rate_adapt_tx_afifo_rd_clr :1;
uint32 rg_rate_adapt_rx_afifo_wr_clr :1;
uint32 rg_rate_adapt_rx_afifo_rd_clr :1;
uint32 rsv_20 :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_rate_adapt_dump_txmac :1;
uint32 rg_rate_adapt_dump_rxmac :1;
uint32 rg_rate_adapt_tx_bypass :1;
uint32 rg_rate_adapt_rx_bypass :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_rate_adapt_ctrl_0, *pHAL_rg_rate_adapt_ctrl_0;

typedef struct {
uint32 rg_rate_adapt_tx_afifo_wr_thr :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_rate_adapt_tx_afifo_rd_thr :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_rate_adapt_rx_afifo_wr_thr :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_rate_adapt_rx_afifo_rd_thr :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_rate_adapt_ctrl_1, *pHAL_rg_rate_adapt_ctrl_1;

typedef struct {
uint32 rg_rate_adapt_tx_xfi_sop_cnt_clr :1;
uint32 rg_rate_adapt_tx_xfi_eop_cnt_clr :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_rate_adapt_tx_mac_sop_cnt_clr :1;
uint32 rg_rate_adapt_tx_mac_eop_cnt_clr :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_rate_adapt_rx_xfi_sop_cnt_clr :1;
uint32 rg_rate_adapt_rx_xfi_eop_cnt_clr :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rg_rate_adapt_rx_mac_sop_cnt_clr :1;
uint32 rg_rate_adapt_rx_mac_eop_cnt_clr :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_rate_adapt_tx_mac_fe_cnt_clr :1;
uint32 rg_rate_adapt_rx_xfi_fe_cnt_clr :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rg_rate_adapt_tx_xfi_ipg_err_cnt_clr :1;
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
}HAL_rg_rate_adapt_ctrl_2, *pHAL_rg_rate_adapt_ctrl_2;

typedef struct {
uint32 rg_rate_adapt_tx_afifo_dout_l :32;
}HAL_rg_rate_adapt_ctrl_3, *pHAL_rg_rate_adapt_ctrl_3;

typedef struct {
uint32 rg_rate_adapt_tx_afifo_dout_h :32;
}HAL_rg_rate_adapt_ctrl_4, *pHAL_rg_rate_adapt_ctrl_4;

typedef struct {
uint32 rg_rate_adapt_tx_afifo_dout_c :8;
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
}HAL_rg_rate_adapt_ctrl_5, *pHAL_rg_rate_adapt_ctrl_5;

typedef struct {
uint32 rg_rate_adapt_rx_afifo_dout_l :32;
}HAL_rg_rate_adapt_ctrl_6, *pHAL_rg_rate_adapt_ctrl_6;

typedef struct {
uint32 rg_rate_adapt_rx_afifo_dout_h :32;
}HAL_rg_rate_adapt_ctrl_7, *pHAL_rg_rate_adapt_ctrl_7;

typedef struct {
uint32 rg_rate_adapt_rx_afifo_dout_c :8;
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
}HAL_rg_rate_adapt_ctrl_8, *pHAL_rg_rate_adapt_ctrl_8;

typedef struct {
uint32 rg_rate_adapt_tx_idle_pattern :32;
}HAL_rg_rate_adapt_ctrl_9, *pHAL_rg_rate_adapt_ctrl_9;

typedef struct {
uint32 rg_rate_adapt_r2t_lpbk :1;
uint32 rsv_1 :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_rate_adapt_r2t_lpbk_afifo_en :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_rate_adapt_r2t_lpbk_afifo_adj :4;
uint32 rg_rate_adapt_r2t_lpbk_afifo_uses_ff :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_rate_adapt_t2r_lpbk :1;
uint32 rsv_17 :1;
uint32 rsv_18 :1;
uint32 rsv_19 :1;
uint32 rg_rate_adapt_t2r_lpbk_afifo_en :1;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_rate_adapt_t2r_lpbk_afifo_adj :4;
uint32 rg_rate_adapt_t2r_lpbk_afifo_uses_ff :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_rg_rate_adapt_ctrl_10, *pHAL_rg_rate_adapt_ctrl_10;

typedef struct {
uint32 rg_rate_adapt_rd_toggle :1;
uint32 rg_xgmii_pkt_rd_toggle :1;
uint32 rg_xgmii_pkt_en :1;
uint32 rsv_3 :1;
uint32 rsv_4 :1;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_force_rate_adapt_mode_en :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rg_force_rate_adapt_mode :4;
uint32 rg_rate_adapt_reserve :16;
}HAL_rg_rate_adapt_ctrl_11, *pHAL_rg_rate_adapt_ctrl_11;

typedef struct {
uint32 ro_rate_adapt_tx_afifo_sts :16;
uint32 ro_rate_adapt_rx_afifo_sts :16;
}HAL_ro_rate_adapt_sts_0, *pHAL_ro_rate_adapt_sts_0;

typedef struct {
uint32 ro_rate_adapt_tx_xfi_sop_cnt :32;
}HAL_ro_rate_adapt_sts_1, *pHAL_ro_rate_adapt_sts_1;

typedef struct {
uint32 ro_rate_adapt_tx_xfi_eop_cnt :32;
}HAL_ro_rate_adapt_sts_2, *pHAL_ro_rate_adapt_sts_2;

typedef struct {
uint32 ro_rate_adapt_tx_mac_sop_cnt :32;
}HAL_ro_rate_adapt_sts_3, *pHAL_ro_rate_adapt_sts_3;

typedef struct {
uint32 ro_rate_adapt_tx_mac_eop_cnt :32;
}HAL_ro_rate_adapt_sts_4, *pHAL_ro_rate_adapt_sts_4;

typedef struct {
uint32 ro_rate_adapt_rx_xfi_sop_cnt :32;
}HAL_ro_rate_adapt_sts_5, *pHAL_ro_rate_adapt_sts_5;

typedef struct {
uint32 ro_rate_adapt_rx_xfi_eop_cnt :32;
}HAL_ro_rate_adapt_sts_6, *pHAL_ro_rate_adapt_sts_6;

typedef struct {
uint32 ro_rate_adapt_rx_mac_sop_cnt :32;
}HAL_ro_rate_adapt_sts_7, *pHAL_ro_rate_adapt_sts_7;

typedef struct {
uint32 ro_rate_adapt_rx_mac_eop_cnt :32;
}HAL_ro_rate_adapt_sts_8, *pHAL_ro_rate_adapt_sts_8;

typedef struct {
uint32 ro_rate_adapt_tx_mac_fe_cnt :32;
}HAL_ro_rate_adapt_sts_9, *pHAL_ro_rate_adapt_sts_9;

typedef struct {
uint32 ro_rate_adapt_rx_xfi_fe_cnt :32;
}HAL_ro_rate_adapt_sts_10, *pHAL_ro_rate_adapt_sts_10;

typedef struct {
uint32 ro_rate_adapt_tx_xfi_ipg_err_cnt :32;
}HAL_ro_rate_adapt_sts_11, *pHAL_ro_rate_adapt_sts_11;

typedef struct {
uint32 ro_rate_adapt_r2t_lpbk_afifo_full_cnt :4;
uint32 ro_rate_adapt_r2t_lpbk_afifo_empty_cnt :4;
uint32 ro_rate_adapt_t2r_lpbk_afifo_full_cnt :4;
uint32 ro_rate_adapt_t2r_lpbk_afifo_empty_cnt :4;
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
}HAL_ro_rate_adapt_sts_12, *pHAL_ro_rate_adapt_sts_12;

typedef struct {
uint32 ro_dbg_xgmii_rx_ipg_ill_cnt :16;
uint32 ro_dbg_xgmii_tx_ipg_ill_cnt :16;
}HAL_XGMII_IPG_CNT, *pHAL_XGMII_IPG_CNT;

typedef struct {
uint32 ro_interface_xgmii_rxd_0 :32;
}HAL_ro_rate_adapt_sts_13, *pHAL_ro_rate_adapt_sts_13;

typedef struct {
uint32 ro_interface_xgmii_rxd_1 :32;
}HAL_ro_rate_adapt_sts_14, *pHAL_ro_rate_adapt_sts_14;

typedef struct {
uint32 ro_interface_xgmii_rxc :8;
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
}HAL_ro_rate_adapt_sts_15, *pHAL_ro_rate_adapt_sts_15;

typedef struct {
uint32 ro_interface_xgmii_txd_0 :32;
}HAL_ro_rate_adapt_sts_16, *pHAL_ro_rate_adapt_sts_16;

typedef struct {
uint32 ro_interface_xgmii_txd_1 :32;
}HAL_ro_rate_adapt_sts_17, *pHAL_ro_rate_adapt_sts_17;

typedef struct {
uint32 ro_interface_xgmii_txc :8;
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
}HAL_ro_rate_adapt_sts_18, *pHAL_ro_rate_adapt_sts_18;

typedef struct {
uint32 rg_rate_adapt_out_pre6_cnt_reset :1;
uint32 rg_rate_adapt_out_pre5_cnt_reset :1;
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
}HAL_rg_rate_adapt_out_cnt_reset, *pHAL_rg_rate_adapt_out_cnt_reset;

typedef struct {
uint32 ro_rate_adapt_out_cnt_pre6 :32;
}HAL_ro_rate_adapt_out_cnt_pre6, *pHAL_ro_rate_adapt_out_cnt_pre6;

typedef struct {
uint32 ro_rate_adapt_out_cnt_pre5 :32;
}HAL_ro_rate_adapt_out_cnt_pre5, *pHAL_ro_rate_adapt_out_cnt_pre5;

typedef struct {
uint32 rg_fpga_mode_reset :8;
uint32 rg_fpga_mode_lpbk :1;
uint32 rg_fpga_cdr_set :2;
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
}HAL_rg_fpga_mode_set, *pHAL_rg_fpga_mode_set;

typedef struct {
uint32 ro_rate_adapt_ctrl_0_sts :32;
}HAL_ro_rate_adapt_ctrl_0_sts, *pHAL_ro_rate_adapt_ctrl_0_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_1_sts :32;
}HAL_ro_rate_adapt_ctrl_1_sts, *pHAL_ro_rate_adapt_ctrl_1_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_2_sts :32;
}HAL_ro_rate_adapt_ctrl_2_sts, *pHAL_ro_rate_adapt_ctrl_2_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_3_sts :32;
}HAL_ro_rate_adapt_ctrl_3_sts, *pHAL_ro_rate_adapt_ctrl_3_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_4_sts :32;
}HAL_ro_rate_adapt_ctrl_4_sts, *pHAL_ro_rate_adapt_ctrl_4_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_5_sts :32;
}HAL_ro_rate_adapt_ctrl_5_sts, *pHAL_ro_rate_adapt_ctrl_5_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_6_sts :32;
}HAL_ro_rate_adapt_ctrl_6_sts, *pHAL_ro_rate_adapt_ctrl_6_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_7_sts :32;
}HAL_ro_rate_adapt_ctrl_7_sts, *pHAL_ro_rate_adapt_ctrl_7_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_8_sts :32;
}HAL_ro_rate_adapt_ctrl_8_sts, *pHAL_ro_rate_adapt_ctrl_8_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_9_sts :32;
}HAL_ro_rate_adapt_ctrl_9_sts, *pHAL_ro_rate_adapt_ctrl_9_sts;

typedef struct {
uint32 ro_rate_adapt_ctrl_10_sts :32;
}HAL_ro_rate_adapt_ctrl_10_sts, *pHAL_ro_rate_adapt_ctrl_10_sts;

typedef struct {
uint32 rg_xgmii_tx_dbg_en :1;
uint32 rg_xgmii_rx_dbg_en :1;
uint32 rsv_2 :1;
uint32 rg_dbg_xgmii_rx_ipg_ill_clr :1;
uint32 rg_dbg_xgmii_tx_ipg_ill_clr :1;
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
}HAL_rg_xgmii_dbg_0, *pHAL_rg_xgmii_dbg_0;

typedef struct {
uint32 rg_xgmii_txc_dbg :8;
uint32 rg_xgmii_rxc_dbg :8;
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
}HAL_rg_xgmii_dbg_1, *pHAL_rg_xgmii_dbg_1;

typedef struct {
uint32 rg_xgmii_txd_dbg_0 :32;
}HAL_rg_xgmii_dbg_2, *pHAL_rg_xgmii_dbg_2;

typedef struct {
uint32 rg_xgmii_txd_dbg_1 :32;
}HAL_rg_xgmii_dbg_3, *pHAL_rg_xgmii_dbg_3;

typedef struct {
uint32 rg_xgmii_rxd_dbg_0 :32;
}HAL_rg_xgmii_dbg_4, *pHAL_rg_xgmii_dbg_4;

typedef struct {
uint32 rg_xgmii_rxd_dbg_1 :32;
}HAL_rg_xgmii_dbg_5, *pHAL_rg_xgmii_dbg_5;

typedef struct {
uint32 rg_dbg_rx_ipg_ill_thres :3;
uint32 rsv_3 :1;
uint32 rg_dbg_tx_ipg_ill_thres :3;
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
}HAL_xgmii_ipg_dbg_0, *pHAL_xgmii_ipg_dbg_0;

typedef struct {
uint32 ro_dbg_rx_fb_fd_at_same_lane :8;
uint32 ro_dbg_tx_fb_fd_at_same_lane :8;
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
}HAL_xgmii_ipg_dbg_1, *pHAL_xgmii_ipg_dbg_1;

#endif