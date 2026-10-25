#ifndef MULTI_PHY_ANA_RO_HAL_REG
#define MULTI_PHY_ANA_RO_HAL_REG

typedef struct {
uint32 ro_da_ssusb_ln0_cdr_lck2ref :1;
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
}HAL_RO_DEBUG_0, *pHAL_RO_DEBUG_0;

typedef struct {
uint32 ro_ad_ssusb_pll_vcocal_fail :1;
uint32 ro_ad_ssusb_pll_vcocal_cplt :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 ro_ad_ssusb_ln0_cdr_vcocal_fail :1;
uint32 ro_ad_ssusb_ln0_cdr_vcocal_cplt :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 ro_ad_ssusb_ln0_tx_cmdet :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_ad_ssusb_pll_vcocal_state :7;
uint32 rsv_23 :1;
uint32 ro_ad_pcie_ln0_sigdet_out :1;
uint32 ro_ad_pcie_ln0_sigdet_cal_out :1;
uint32 ro_pcie_ln0_sigdet_cal_out :1;
uint32 ro_ssusb_ln0_rx_impcal_out :1;
uint32 ro_ssusb_ln0_rx_lfps :1;
uint32 ro_ad_pcie_ln0_crsdet_out :1;
uint32 ro_ad_pcie_ln0_crsdet_lthout :1;
uint32 ro_ad_pcie_ln0_crsdet_cal_out :1;
}HAL_RO_AD_INTERFACE, *pHAL_RO_AD_INTERFACE;

typedef struct {
uint32 ro_da_ssusb_pll_bc :2;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 ro_da_ssusb_pll_diven :3;
uint32 rsv_7 :1;
uint32 ro_da_ssusb_pll_ic :4;
uint32 ro_da_ssusb_pll_br :2;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_da_ssusb_pll_ir :4;
uint32 ro_da_ssusb_pll_bp :4;
uint32 ro_da_ssusb_pll_band :7;
uint32 rsv_31 :1;
}HAL_RO_DA_INTERFACE_0, *pHAL_RO_DA_INTERFACE_0;

typedef struct {
uint32 ro_da_ssusb_pll_fbksel :2;
uint32 ro_da_ssusb_pll_prediv :2;
uint32 ro_da_ssusb_pll_fbkdiv :7;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_da_tphy_ln0_mode :2;
uint32 ro_da_tphy_ln0_speed :2;
uint32 ro_da_ssusb_ln0_tx_biasi :3;
uint32 rsv_23 :1;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RO_DA_INTERFACE_1, *pHAL_RO_DA_INTERFACE_1;

typedef struct {
uint32 ro_da_ssusb_pll_ssc_delta :16;
uint32 ro_da_ssusb_pll_ssc_delta1 :16;
}HAL_RO_DA_INTERFACE_2, *pHAL_RO_DA_INTERFACE_2;

typedef struct {
uint32 ro_da_ssusb_ln0_idrv_0db :6;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 ro_da_ssusb_ln0_idrv_3p5db :6;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_da_ssusb_ln0_idem_3p5db :6;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 ro_da_ssusb_ln0_idrv_6db :6;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RO_DA_INTERFACE_3, *pHAL_RO_DA_INTERFACE_3;

typedef struct {
uint32 ro_da_ssusb_ln0_idem_6db :6;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 ro_da_ssusb_ln0_tx_imp_sel :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_da_ssusb_ln0_tx_dem :2;
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
}HAL_RO_DA_INTERFACE_4, *pHAL_RO_DA_INTERFACE_4;

typedef struct {
uint32 ro_da_ssusb_ln0_cdr_bir :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 ro_da_ssusb_ln0_cdr_bic :4;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_da_ssusb_ln0_cdr_bc :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 ro_da_ssusb_ln0_cdr_tcadj :6;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RO_DA_INTERFACE_5, *pHAL_RO_DA_INTERFACE_5;

typedef struct {
uint32 ro_da_ssusb_ln0_cdr_bpa :2;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 ro_da_ssusb_ln0_cdr_bpb :3;
uint32 rsv_7 :1;
uint32 ro_da_ssusb_ln0_cdr_br :3;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_da_ssusb_ln0_cdr_fbdiv :7;
uint32 ro_da_ssusb_ln0_cdr_kvsel :1;
uint32 ro_da_ssusb_ln0_cdr_band :7;
uint32 rsv_31 :1;
}HAL_RO_DA_INTERFACE_6, *pHAL_RO_DA_INTERFACE_6;

typedef struct {
uint32 ro_da_ssusb_ln0_lfps_deglitch :2;
uint32 ro_da_ssusb_ln0_eq_rstep1 :2;
uint32 ro_da_ssusb_ln0_eq_rstep2 :2;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 ro_da_ssusb_ln0_rx_imp_sel :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_da_pcie_ln0_sigdet_cal_offset :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 ro_da_pcie_ln0_crsdet_ofst :4;
uint32 ro_da_ssusb_xtal_ext_en :2;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RO_DA_INTERFACE_7, *pHAL_RO_DA_INTERFACE_7;

typedef struct {
uint32 ro_multi_phy_lpbk_async_fifo_full :1;
uint32 ro_multi_phy_lpbk_async_fifo_afull :1;
uint32 ro_multi_phy_lpbk_async_fifo_empty :1;
uint32 ro_multi_phy_lpbk_async_fifo_aempty :1;
uint32 ro_multi_phy_lpbk_async_fifo_r_ready :1;
uint32 ro_multi_phy_lpbk_async_fifo_w_ready :1;
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
uint32 ro_multi_phy_lpbk_async_fifo_wfifo_cnt :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 ro_multi_phy_lpbk_async_fifo_rfifo_cnt :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RO_FIFO_STS_0, *pHAL_RO_FIFO_STS_0;

typedef struct {
uint32 ro_multi_phy_lpbk_async_fifo_full_cnt :4;
uint32 ro_multi_phy_lpbk_async_fifo_afull_cnt :4;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 ro_multi_phy_lpbk_async_fifo_empty_cnt :4;
uint32 ro_multi_phy_lpbk_async_fifo_aempty_cnt :4;
uint32 rsv_24 :1;
uint32 rsv_25 :1;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RO_FIFO_STS_1, *pHAL_RO_FIFO_STS_1;

#endif