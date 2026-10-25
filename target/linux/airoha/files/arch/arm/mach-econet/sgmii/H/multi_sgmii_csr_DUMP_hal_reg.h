#ifndef MULTI_SGMII_CSR_DUMP_HAL_REG
#define MULTI_SGMII_CSR_DUMP_HAL_REG

typedef struct {
uint32 ro_dump_000 :32;
}HAL_Debug_REGISTER_0, *pHAL_Debug_REGISTER_0;

typedef struct {
uint32 ro_dump_001 :32;
}HAL_Debug_REGISTER_1, *pHAL_Debug_REGISTER_1;

typedef struct {
uint32 ro_dump_002 :32;
}HAL_Debug_REGISTER_2, *pHAL_Debug_REGISTER_2;

typedef struct {
uint32 ro_dump_003 :32;
}HAL_Debug_REGISTER_3, *pHAL_Debug_REGISTER_3;

typedef struct {
uint32 ro_dump_004 :32;
}HAL_Debug_REGISTER_4, *pHAL_Debug_REGISTER_4;

typedef struct {
uint32 ro_dump_005 :32;
}HAL_Debug_REGISTER_5, *pHAL_Debug_REGISTER_5;

typedef struct {
uint32 ro_dump_006 :32;
}HAL_Debug_REGISTER_6, *pHAL_Debug_REGISTER_6;

typedef struct {
uint32 ro_dump_007 :32;
}HAL_Debug_REGISTER_7, *pHAL_Debug_REGISTER_7;

typedef struct {
uint32 ro_dump_008 :32;
}HAL_Debug_REGISTER_8, *pHAL_Debug_REGISTER_8;

typedef struct {
uint32 ro_dump_009 :32;
}HAL_Debug_REGISTER_9, *pHAL_Debug_REGISTER_9;

typedef struct {
uint32 ro_dump_010 :32;
}HAL_Debug_REGISTER_10, *pHAL_Debug_REGISTER_10;

typedef struct {
uint32 ro_dump_011 :32;
}HAL_Debug_REGISTER_11, *pHAL_Debug_REGISTER_11;

typedef struct {
uint32 ro_dump_012 :32;
}HAL_Debug_REGISTER_12, *pHAL_Debug_REGISTER_12;

typedef struct {
uint32 ro_dump_013 :32;
}HAL_Debug_REGISTER_13, *pHAL_Debug_REGISTER_13;

typedef struct {
uint32 ro_dump_014 :32;
}HAL_Debug_REGISTER_14, *pHAL_Debug_REGISTER_14;

typedef struct {
uint32 ro_dump_015 :32;
}HAL_Debug_REGISTER_15, *pHAL_Debug_REGISTER_15;

typedef struct {
uint32 ro_dump_016 :32;
}HAL_Debug_REGISTER_16, *pHAL_Debug_REGISTER_16;

typedef struct {
uint32 ro_dump_017 :32;
}HAL_Debug_REGISTER_17, *pHAL_Debug_REGISTER_17;

typedef struct {
uint32 ro_dump_018 :32;
}HAL_Debug_REGISTER_18, *pHAL_Debug_REGISTER_18;

typedef struct {
uint32 ro_dump_019 :32;
}HAL_Debug_REGISTER_19, *pHAL_Debug_REGISTER_19;

typedef struct {
uint32 ro_dump_020 :32;
}HAL_Debug_REGISTER_20, *pHAL_Debug_REGISTER_20;

typedef struct {
uint32 ro_dump_021 :32;
}HAL_Debug_REGISTER_21, *pHAL_Debug_REGISTER_21;

typedef struct {
uint32 ro_dump_022 :32;
}HAL_Debug_REGISTER_22, *pHAL_Debug_REGISTER_22;

typedef struct {
uint32 ro_dump_023 :32;
}HAL_Debug_REGISTER_23, *pHAL_Debug_REGISTER_23;

typedef struct {
uint32 ro_dump_024 :32;
}HAL_Debug_REGISTER_24, *pHAL_Debug_REGISTER_24;

typedef struct {
uint32 ro_dump_025 :32;
}HAL_Debug_REGISTER_25, *pHAL_Debug_REGISTER_25;

typedef struct {
uint32 ro_dump_026 :32;
}HAL_Debug_REGISTER_26, *pHAL_Debug_REGISTER_26;

typedef struct {
uint32 ro_dump_027 :32;
}HAL_Debug_REGISTER_27, *pHAL_Debug_REGISTER_27;

typedef struct {
uint32 ro_dump_028 :32;
}HAL_Debug_REGISTER_28, *pHAL_Debug_REGISTER_28;

typedef struct {
uint32 ro_dump_029 :32;
}HAL_Debug_REGISTER_29, *pHAL_Debug_REGISTER_29;

typedef struct {
uint32 ro_dump_030 :32;
}HAL_Debug_REGISTER_30, *pHAL_Debug_REGISTER_30;

typedef struct {
uint32 ro_dump_031 :32;
}HAL_Debug_REGISTER_31, *pHAL_Debug_REGISTER_31;

typedef struct {
uint32 rg_dump_clr :1;
uint32 rg_dump_inv :1;
uint32 rsv_2 :1;
uint32 rsv_3 :1;
uint32 rg_dump_sel :3;
uint32 rsv_7 :1;
uint32 rg_force_dump :1;
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
}HAL_Debug_REGISTER_CTRL_0, *pHAL_Debug_REGISTER_CTRL_0;

typedef struct {
uint32 ro_dump_marker :8;
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
}HAL_Debug_REGISTER_DUMP_CNT, *pHAL_Debug_REGISTER_DUMP_CNT;

#endif