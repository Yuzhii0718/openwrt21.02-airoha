#ifndef RATE_ADAPTION_CSR_MII_RATE_ADP_HAL_REG
#define RATE_ADAPTION_CSR_MII_RATE_ADP_HAL_REG

typedef struct {
uint32 rg_p0_mii_ra_tx_mode :1;
uint32 rg_p0_mii_ra_rx_mode :1;
uint32 rg_p0_mii_ra_tx_en :1;
uint32 rg_p0_mii_ra_rx_en :1;
uint32 rg_p0_mii_ra_tx_wr_clr :1;
uint32 rg_p0_mii_ra_tx_rd_clr :1;
uint32 rg_p0_mii_ra_rx_wr_clr :1;
uint32 rg_p0_mii_ra_rx_rd_clr :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p0_mii_ra_idle_data :10;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rg_p0_mii_mode :1;
uint32 rg_p0_mii_tx_eee_en :1;
uint32 rg_p0_mii_rx_eee_en :1;
uint32 rg_p0_dis_mii_mode :1;
}HAL_RATE_ADP_P0_CTRL_0, *pHAL_RATE_ADP_P0_CTRL_0;

typedef struct {
uint32 rg_p0_mii_ra_tx_wr_thr :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_p0_mii_ra_tx_rd_thr :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p0_mii_ra_rx_wr_thr :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_p0_mii_ra_rx_rd_thr :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RATE_ADP_P0_CTRL_1, *pHAL_RATE_ADP_P0_CTRL_1;

typedef struct {
uint32 rg_p1_mii_ra_tx_mode :1;
uint32 rg_p1_mii_ra_rx_mode :1;
uint32 rg_p1_mii_ra_tx_en :1;
uint32 rg_p1_mii_ra_rx_en :1;
uint32 rg_p1_mii_ra_tx_wr_clr :1;
uint32 rg_p1_mii_ra_tx_rd_clr :1;
uint32 rg_p1_mii_ra_rx_wr_clr :1;
uint32 rg_p1_mii_ra_rx_rd_clr :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p1_mii_ra_idle_data :10;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rg_p1_mii_tx_eee_en :1;
uint32 rg_p1_mii_rx_eee_en :1;
uint32 rsv_31 :1;
}HAL_RATE_ADP_P1_CTRL_0, *pHAL_RATE_ADP_P1_CTRL_0;

typedef struct {
uint32 rg_p1_mii_ra_tx_wr_thr :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_p1_mii_ra_tx_rd_thr :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p1_mii_ra_rx_wr_thr :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_p1_mii_ra_rx_rd_thr :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RATE_ADP_P1_CTRL_1, *pHAL_RATE_ADP_P1_CTRL_1;

typedef struct {
uint32 rg_p2_mii_ra_tx_mode :1;
uint32 rg_p2_mii_ra_rx_mode :1;
uint32 rg_p2_mii_ra_tx_en :1;
uint32 rg_p2_mii_ra_rx_en :1;
uint32 rg_p2_mii_ra_tx_wr_clr :1;
uint32 rg_p2_mii_ra_tx_rd_clr :1;
uint32 rg_p2_mii_ra_rx_wr_clr :1;
uint32 rg_p2_mii_ra_rx_rd_clr :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p2_mii_ra_idle_data :10;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rg_p2_mii_tx_eee_en :1;
uint32 rg_p2_mii_rx_eee_en :1;
uint32 rsv_31 :1;
}HAL_RATE_ADP_P2_CTRL_0, *pHAL_RATE_ADP_P2_CTRL_0;

typedef struct {
uint32 rg_p2_mii_ra_tx_wr_thr :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_p2_mii_ra_tx_rd_thr :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p2_mii_ra_rx_wr_thr :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_p2_mii_ra_rx_rd_thr :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RATE_ADP_P2_CTRL_1, *pHAL_RATE_ADP_P2_CTRL_1;

typedef struct {
uint32 rg_p3_mii_ra_tx_mode :1;
uint32 rg_p3_mii_ra_rx_mode :1;
uint32 rg_p3_mii_ra_tx_en :1;
uint32 rg_p3_mii_ra_rx_en :1;
uint32 rg_p3_mii_ra_tx_wr_clr :1;
uint32 rg_p3_mii_ra_tx_rd_clr :1;
uint32 rg_p3_mii_ra_rx_wr_clr :1;
uint32 rg_p3_mii_ra_rx_rd_clr :1;
uint32 rsv_8 :1;
uint32 rsv_9 :1;
uint32 rsv_10 :1;
uint32 rsv_11 :1;
uint32 rsv_12 :1;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p3_mii_ra_idle_data :10;
uint32 rsv_26 :1;
uint32 rsv_27 :1;
uint32 rsv_28 :1;
uint32 rg_p3_mii_tx_eee_en :1;
uint32 rg_p3_mii_rx_eee_en :1;
uint32 rsv_31 :1;
}HAL_RATE_ADP_P3_CTRL_0, *pHAL_RATE_ADP_P3_CTRL_0;

typedef struct {
uint32 rg_p3_mii_ra_tx_wr_thr :5;
uint32 rsv_5 :1;
uint32 rsv_6 :1;
uint32 rsv_7 :1;
uint32 rg_p3_mii_ra_tx_rd_thr :5;
uint32 rsv_13 :1;
uint32 rsv_14 :1;
uint32 rsv_15 :1;
uint32 rg_p3_mii_ra_rx_wr_thr :5;
uint32 rsv_21 :1;
uint32 rsv_22 :1;
uint32 rsv_23 :1;
uint32 rg_p3_mii_ra_rx_rd_thr :5;
uint32 rsv_29 :1;
uint32 rsv_30 :1;
uint32 rsv_31 :1;
}HAL_RATE_ADP_P3_CTRL_1, *pHAL_RATE_ADP_P3_CTRL_1;

typedef struct {
uint32 rg_p0_tx_idle_pattern :8;
uint32 rg_p1_tx_idle_pattern :8;
uint32 rg_p2_tx_idle_pattern :8;
uint32 rg_p3_tx_idle_pattern :8;
}HAL_RATE_ADP_IDL_CTRL_0, *pHAL_RATE_ADP_IDL_CTRL_0;

typedef struct {
uint32 rg_p0_mac_mii_tx_dbg_clr :1;
uint32 rg_p0_mac_mii_tx_dbg_latch :1;
uint32 rg_p0_phy_mii_tx_dbg_clr :1;
uint32 rg_p0_phy_mii_tx_dbg_latch :1;
uint32 rg_p0_mac_mii_rx_dbg_clr :1;
uint32 rg_p0_mac_mii_rx_dbg_latch :1;
uint32 rg_p0_phy_mii_rx_dbg_clr :1;
uint32 rg_p0_phy_mii_rx_dbg_latch :1;
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
}HAL_RATE_ADP_DBG_p0_0, *pHAL_RATE_ADP_DBG_p0_0;

typedef struct {
uint32 ro_p0_dbg_mac_mii_tx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p0_1, *pHAL_RATE_ADP_DBG_p0_1;

typedef struct {
uint32 ro_p0_dbg_mac_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p0_2, *pHAL_RATE_ADP_DBG_p0_2;

typedef struct {
uint32 ro_p0_dbg_mac_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p0_3, *pHAL_RATE_ADP_DBG_p0_3;

typedef struct {
uint32 ro_p0_dbg_mac_mii_rx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p0_4, *pHAL_RATE_ADP_DBG_p0_4;

typedef struct {
uint32 ro_p0_dbg_mac_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p0_5, *pHAL_RATE_ADP_DBG_p0_5;

typedef struct {
uint32 ro_p0_dbg_mac_mii_tx_6x55_cnt :16;
uint32 ro_p0_dbg_mac_mii_tx_8x55_cnt :16;
}HAL_RATE_ADP_DBG_p0_7, *pHAL_RATE_ADP_DBG_p0_7;

typedef struct {
uint32 ro_p0_dbg_mac_mii_rx_6x55_cnt :16;
uint32 ro_p0_dbg_mac_mii_rx_8x55_cnt :16;
}HAL_RATE_ADP_DBG_p0_8, *pHAL_RATE_ADP_DBG_p0_8;

typedef struct {
uint32 ro_p0_dbg_phy_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p0_9, *pHAL_RATE_ADP_DBG_p0_9;

typedef struct {
uint32 ro_p0_dbg_phy_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p0_10, *pHAL_RATE_ADP_DBG_p0_10;

typedef struct {
uint32 ro_p0_dbg_phy_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p0_11, *pHAL_RATE_ADP_DBG_p0_11;

typedef struct {
uint32 rg_p1_mac_mii_tx_dbg_clr :1;
uint32 rg_p1_mac_mii_tx_dbg_latch :1;
uint32 rg_p1_phy_mii_tx_dbg_clr :1;
uint32 rg_p1_phy_mii_tx_dbg_latch :1;
uint32 rg_p1_mac_mii_rx_dbg_clr :1;
uint32 rg_p1_mac_mii_rx_dbg_latch :1;
uint32 rg_p1_phy_mii_rx_dbg_clr :1;
uint32 rg_p1_phy_mii_rx_dbg_latch :1;
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
}HAL_RATE_ADP_DBG_p1_0, *pHAL_RATE_ADP_DBG_p1_0;

typedef struct {
uint32 ro_p1_dbg_mac_mii_tx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p1_1, *pHAL_RATE_ADP_DBG_p1_1;

typedef struct {
uint32 ro_p1_dbg_mac_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p1_2, *pHAL_RATE_ADP_DBG_p1_2;

typedef struct {
uint32 ro_p1_dbg_mac_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p1_3, *pHAL_RATE_ADP_DBG_p1_3;

typedef struct {
uint32 ro_p1_dbg_mac_mii_rx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p1_4, *pHAL_RATE_ADP_DBG_p1_4;

typedef struct {
uint32 ro_p1_dbg_mac_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p1_5, *pHAL_RATE_ADP_DBG_p1_5;

typedef struct {
uint32 ro_p1_dbg_mac_mii_tx_6x55_cnt :16;
uint32 ro_p1_dbg_mac_mii_tx_8x55_cnt :16;
}HAL_RATE_ADP_DBG_p1_7, *pHAL_RATE_ADP_DBG_p1_7;

typedef struct {
uint32 ro_p1_dbg_mac_mii_rx_6x55_cnt :16;
uint32 ro_p1_dbg_mac_mii_rx_8x55_cnt :16;
}HAL_RATE_ADP_DBG_p1_8, *pHAL_RATE_ADP_DBG_p1_8;

typedef struct {
uint32 ro_p1_dbg_phy_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p1_9, *pHAL_RATE_ADP_DBG_p1_9;

typedef struct {
uint32 ro_p1_dbg_phy_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p1_10, *pHAL_RATE_ADP_DBG_p1_10;

typedef struct {
uint32 ro_p1_dbg_phy_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p1_11, *pHAL_RATE_ADP_DBG_p1_11;

typedef struct {
uint32 rg_p2_mac_mii_tx_dbg_clr :1;
uint32 rg_p2_mac_mii_tx_dbg_latch :1;
uint32 rg_p2_phy_mii_tx_dbg_clr :1;
uint32 rg_p2_phy_mii_tx_dbg_latch :1;
uint32 rg_p2_mac_mii_rx_dbg_clr :1;
uint32 rg_p2_mac_mii_rx_dbg_latch :1;
uint32 rg_p2_phy_mii_rx_dbg_clr :1;
uint32 rg_p2_phy_mii_rx_dbg_latch :1;
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
}HAL_RATE_ADP_DBG_p2_0, *pHAL_RATE_ADP_DBG_p2_0;

typedef struct {
uint32 ro_p2_dbg_mac_mii_tx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p2_1, *pHAL_RATE_ADP_DBG_p2_1;

typedef struct {
uint32 ro_p2_dbg_mac_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p2_2, *pHAL_RATE_ADP_DBG_p2_2;

typedef struct {
uint32 ro_p2_dbg_mac_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p2_3, *pHAL_RATE_ADP_DBG_p2_3;

typedef struct {
uint32 ro_p2_dbg_mac_mii_rx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p2_4, *pHAL_RATE_ADP_DBG_p2_4;

typedef struct {
uint32 ro_p2_dbg_mac_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p2_5, *pHAL_RATE_ADP_DBG_p2_5;

typedef struct {
uint32 ro_p2_dbg_mac_mii_tx_6x55_cnt :16;
uint32 ro_p2_dbg_mac_mii_tx_8x55_cnt :16;
}HAL_RATE_ADP_DBG_p2_7, *pHAL_RATE_ADP_DBG_p2_7;

typedef struct {
uint32 ro_p2_dbg_mac_mii_rx_6x55_cnt :16;
uint32 ro_p2_dbg_mac_mii_rx_8x55_cnt :16;
}HAL_RATE_ADP_DBG_p2_8, *pHAL_RATE_ADP_DBG_p2_8;

typedef struct {
uint32 ro_p2_dbg_phy_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p2_9, *pHAL_RATE_ADP_DBG_p2_9;

typedef struct {
uint32 ro_p2_dbg_phy_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p2_10, *pHAL_RATE_ADP_DBG_p2_10;

typedef struct {
uint32 ro_p2_dbg_phy_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p2_11, *pHAL_RATE_ADP_DBG_p2_11;

typedef struct {
uint32 rg_p3_mac_mii_tx_dbg_clr :1;
uint32 rg_p3_mac_mii_tx_dbg_latch :1;
uint32 rg_p3_phy_mii_tx_dbg_clr :1;
uint32 rg_p3_phy_mii_tx_dbg_latch :1;
uint32 rg_p3_mac_mii_rx_dbg_clr :1;
uint32 rg_p3_mac_mii_rx_dbg_latch :1;
uint32 rg_p3_phy_mii_rx_dbg_clr :1;
uint32 rg_p3_phy_mii_rx_dbg_latch :1;
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
}HAL_RATE_ADP_DBG_p3_0, *pHAL_RATE_ADP_DBG_p3_0;

typedef struct {
uint32 ro_p3_dbg_mac_mii_tx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p3_1, *pHAL_RATE_ADP_DBG_p3_1;

typedef struct {
uint32 ro_p3_dbg_mac_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p3_2, *pHAL_RATE_ADP_DBG_p3_2;

typedef struct {
uint32 ro_p3_dbg_mac_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p3_3, *pHAL_RATE_ADP_DBG_p3_3;

typedef struct {
uint32 ro_p3_dbg_mac_mii_rx_d5_cnt :32;
}HAL_RATE_ADP_DBG_p3_4, *pHAL_RATE_ADP_DBG_p3_4;

typedef struct {
uint32 ro_p3_dbg_mac_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p3_5, *pHAL_RATE_ADP_DBG_p3_5;

typedef struct {
uint32 ro_p3_dbg_mac_mii_tx_6x55_cnt :16;
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
}HAL_RATE_ADP_DBG_p3_7, *pHAL_RATE_ADP_DBG_p3_7;

typedef struct {
uint32 ro_p3_dbg_mac_mii_rx_6x55_cnt :16;
uint32 ro_p3_dbg_mac_mii_rx_8x55_cnt :16;
}HAL_RATE_ADP_DBG_p3_8, *pHAL_RATE_ADP_DBG_p3_8;

typedef struct {
uint32 ro_p3_dbg_phy_mii_tx_en_cnt :32;
}HAL_RATE_ADP_DBG_p3_9, *pHAL_RATE_ADP_DBG_p3_9;

typedef struct {
uint32 ro_p3_dbg_phy_mii_rx_dv_cnt :32;
}HAL_RATE_ADP_DBG_p3_10, *pHAL_RATE_ADP_DBG_p3_10;

typedef struct {
uint32 ro_p3_dbg_phy_mii_tx_er_cnt :16;
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
}HAL_RATE_ADP_DBG_p3_11, *pHAL_RATE_ADP_DBG_p3_11;

typedef struct {
uint32 ro_p0_dbg_phy_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P0_0, *pHAL_RATE_DBG_RX_ER_P0_0;

typedef struct {
uint32 ro_p0_dbg_phy_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P0_1, *pHAL_RATE_DBG_RX_ER_P0_1;

typedef struct {
uint32 ro_p0_dbg_mac_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P0_2, *pHAL_RATE_DBG_RX_ER_P0_2;

typedef struct {
uint32 ro_p0_dbg_mac_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P0_3, *pHAL_RATE_DBG_RX_ER_P0_3;

typedef struct {
uint32 ro_p1_dbg_phy_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P1_0, *pHAL_RATE_DBG_RX_ER_P1_0;

typedef struct {
uint32 ro_p1_dbg_phy_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P1_1, *pHAL_RATE_DBG_RX_ER_P1_1;

typedef struct {
uint32 ro_p1_dbg_mac_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P1_2, *pHAL_RATE_DBG_RX_ER_P1_2;

typedef struct {
uint32 ro_p1_dbg_mac_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P1_3, *pHAL_RATE_DBG_RX_ER_P1_3;

typedef struct {
uint32 ro_p2_dbg_phy_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P2_0, *pHAL_RATE_DBG_RX_ER_P2_0;

typedef struct {
uint32 ro_p2_dbg_phy_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P2_1, *pHAL_RATE_DBG_RX_ER_P2_1;

typedef struct {
uint32 ro_p2_dbg_mac_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P2_2, *pHAL_RATE_DBG_RX_ER_P2_2;

typedef struct {
uint32 ro_p2_dbg_mac_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P2_3, *pHAL_RATE_DBG_RX_ER_P2_3;

typedef struct {
uint32 ro_p3_dbg_phy_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P3_0, *pHAL_RATE_DBG_RX_ER_P3_0;

typedef struct {
uint32 ro_p3_dbg_phy_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P3_1, *pHAL_RATE_DBG_RX_ER_P3_1;

typedef struct {
uint32 ro_p3_dbg_mac_mii_rx_er_outdv_cnt :32;
}HAL_RATE_DBG_RX_ER_P3_2, *pHAL_RATE_DBG_RX_ER_P3_2;

typedef struct {
uint32 ro_p3_dbg_mac_mii_rx_er_indv_cnt :32;
}HAL_RATE_DBG_RX_ER_P3_3, *pHAL_RATE_DBG_RX_ER_P3_3;

typedef struct {
uint32 ro_p0_dbg_rx_ipg_ill_cnt :16;
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
}HAL_MII_IPG_CNT_p0, *pHAL_MII_IPG_CNT_p0;

typedef struct {
uint32 rg_p0_dbg_rx_ipg_hold_thres :8;
uint32 rg_p0_dbg_rx_ipg_ill_thres :8;
uint32 rg_p0_dbg_rx_ipg_ill_clr :1;
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
}HAL_MII_IPG_CTRL_p0, *pHAL_MII_IPG_CTRL_p0;

typedef struct {
uint32 ro_p1_dbg_rx_ipg_ill_cnt :16;
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
}HAL_MII_IPG_CNT_p1, *pHAL_MII_IPG_CNT_p1;

typedef struct {
uint32 rg_p1_dbg_rx_ipg_hold_thres :8;
uint32 rg_p1_dbg_rx_ipg_ill_thres :8;
uint32 rg_p1_dbg_rx_ipg_ill_clr :1;
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
}HAL_MII_IPG_CTRL_p1, *pHAL_MII_IPG_CTRL_p1;

typedef struct {
uint32 ro_p2_dbg_rx_ipg_ill_cnt :16;
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
}HAL_MII_IPG_CNT_p2, *pHAL_MII_IPG_CNT_p2;

typedef struct {
uint32 rg_p2_dbg_rx_ipg_hold_thres :8;
uint32 rg_p2_dbg_rx_ipg_ill_thres :8;
uint32 rg_p2_dbg_rx_ipg_ill_clr :1;
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
}HAL_MII_IPG_CTRL_p2, *pHAL_MII_IPG_CTRL_p2;

typedef struct {
uint32 ro_p3_dbg_rx_ipg_ill_cnt :16;
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
}HAL_MII_IPG_CNT_p3, *pHAL_MII_IPG_CNT_p3;

typedef struct {
uint32 rg_p3_dbg_rx_ipg_hold_thres :8;
uint32 rg_p3_dbg_rx_ipg_ill_thres :8;
uint32 rg_p3_dbg_rx_ipg_ill_clr :1;
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
}HAL_MII_IPG_CTRL_p3, *pHAL_MII_IPG_CTRL_p3;

typedef struct {
uint32 rg_p0_ra_an_en :1;
uint32 rg_p1_ra_an_en :1;
uint32 rg_p2_ra_an_en :1;
uint32 rg_p3_ra_an_en :1;
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
}HAL_MII_RA_AN_ENABLE, *pHAL_MII_RA_AN_ENABLE;

#endif