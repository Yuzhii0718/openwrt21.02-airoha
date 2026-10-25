#ifndef MULTI_SGMII_CSR_LED_HAL_REG
#define MULTI_SGMII_CSR_LED_HAL_REG

typedef struct {
uint32 rg_led0_en :1;
uint32 rg_led1_en :1;
uint32 rg_led2_en :1;
uint32 rg_led3_en :1;
uint32 rg_led0_pol :1;
uint32 rg_led1_pol :1;
uint32 rg_led2_pol :1;
uint32 rg_led3_pol :1;
uint32 rg_led_mode :2;
uint32 rg_led_enhance :1;
uint32 rsv_11 :1;
uint32 rg_dwn_shift_blk_en :1;
uint32 rg_dwn_shift_en :1;
uint32 rg_led_timing_test :1;
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
}HAL_msg_led_ctrl_0, *pHAL_msg_led_ctrl_0;

typedef struct {
uint32 rg_led_on_dur :16;
uint32 rg_led_blk_dur :16;
}HAL_msg_led_ctrl_1, *pHAL_msg_led_ctrl_1;

typedef struct {
uint32 rg_led0_on_mask :7;
uint32 rsv_7 :1;
uint32 rg_led0_blk_mask :8;
uint32 rg_led1_on_mask :7;
uint32 rsv_23 :1;
uint32 rg_led1_blk_mask :8;
}HAL_msg_led_ctrl_2, *pHAL_msg_led_ctrl_2;

typedef struct {
uint32 rg_led2_on_mask :7;
uint32 rsv_7 :1;
uint32 rg_led2_blk_mask :8;
uint32 rg_led3_on_mask :7;
uint32 rsv_23 :1;
uint32 rg_led3_blk_mask :8;
}HAL_msg_led_ctrl_3, *pHAL_msg_led_ctrl_3;

typedef struct {
uint32 ro_led_on_event :7;
uint32 rsv_7 :1;
uint32 ro_led_blk_event :8;
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
}HAL_msg_led_sts_0, *pHAL_msg_led_sts_0;

#endif