#ifndef _SGMII_HAL_TOP_H_
#define _SGMII_HAL_TOP_H_


#include "sgmii_hal_reg.h"



//===================================================================================================
//AN
//======================================================================
typedef union {
	pHAL_sgmii_reg_an0                           sgmii_reg_an0;
	pHAL_sgmii_reg_an_1                          sgmii_reg_an_1;
	pHAL_sgmii_reg_an_2                          sgmii_reg_an_2;
	pHAL_sgmii_reg_an_3                          sgmii_reg_an_3;
	pHAL_sgmii_reg_an_4                          sgmii_reg_an_4;
	pHAL_sgmii_reg_an_5                          sgmii_reg_an_5;
	pHAL_sgmii_reg_an_6                          sgmii_reg_an_6;
	pHAL_sgmii_reg_an_7                          sgmii_reg_an_7;
	pHAL_sgmii_reg_an_8                          sgmii_reg_an_8;
	pHAL_sgmii_reg_an_9                          sgmii_reg_an_9;
	pHAL_sgmii_reg_an_10                         sgmii_reg_an_10;
	pHAL_sgmii_reg_an_11                         sgmii_reg_an_11;
	pHAL_sgmii_reg_an_12                         sgmii_reg_an_12;
	pHAL_sgmii_reg_an_13                         sgmii_reg_an_13;
	pHAL_sgmii_reg_an_14                         sgmii_reg_an_14;
	pHAL_sgmii_reg_an_15                         sgmii_reg_an_15;
	pHAL_sgmii_reg_an_16                         sgmii_reg_an_16;
	pHAL_sgmii_reg_an_17                         sgmii_reg_an_17;
	pHAL_sgmii_reg_an_18                         sgmii_reg_an_18;
	pHAL_sgmii_reg_an_19                         sgmii_reg_an_19;
	pHAL_sgmii_reg_an_20                         sgmii_reg_an_20;
	pHAL_sgmii_reg_an_21                         sgmii_reg_an_21;
	pHAL_sgmii_reg_an_22                         sgmii_reg_an_22;
	pHAL_sgmii_reg_an_force_cl37                 sgmii_reg_an_force_cl37;
}HAL_RG_AN, *pHAL_RG_AN;




//===================================================================================================
//RATE_ADAPT
//======================================================================
typedef union {
	pHAL_rg_rate_adapt_ctrl_0                    rg_rate_adapt_ctrl_0;
	pHAL_rg_rate_adapt_ctrl_1                    rg_rate_adapt_ctrl_1;
	pHAL_rg_rate_adapt_ctrl_2                    rg_rate_adapt_ctrl_2;
	pHAL_rg_rate_adapt_ctrl_3                    rg_rate_adapt_ctrl_3;
	pHAL_rg_rate_adapt_ctrl_4                    rg_rate_adapt_ctrl_4;
	pHAL_rg_rate_adapt_ctrl_5                    rg_rate_adapt_ctrl_5;
	pHAL_rg_rate_adapt_ctrl_6                    rg_rate_adapt_ctrl_6;
	pHAL_rg_rate_adapt_ctrl_7                    rg_rate_adapt_ctrl_7;
	pHAL_rg_rate_adapt_ctrl_8                    rg_rate_adapt_ctrl_8;
	pHAL_rg_rate_adapt_ctrl_9                    rg_rate_adapt_ctrl_9;
	pHAL_rg_rate_adapt_ctrl_10                   rg_rate_adapt_ctrl_10;
	pHAL_rg_rate_adapt_ctrl_11                   rg_rate_adapt_ctrl_11;
	pHAL_ro_rate_adapt_sts_0                     ro_rate_adapt_sts_0;
	pHAL_ro_rate_adapt_sts_1                     ro_rate_adapt_sts_1;
	pHAL_ro_rate_adapt_sts_2                     ro_rate_adapt_sts_2;
	pHAL_ro_rate_adapt_sts_3                     ro_rate_adapt_sts_3;
	pHAL_ro_rate_adapt_sts_4                     ro_rate_adapt_sts_4;
	pHAL_ro_rate_adapt_sts_5                     ro_rate_adapt_sts_5;
	pHAL_ro_rate_adapt_sts_6                     ro_rate_adapt_sts_6;
	pHAL_ro_rate_adapt_sts_7                     ro_rate_adapt_sts_7;
	pHAL_ro_rate_adapt_sts_8                     ro_rate_adapt_sts_8;
	pHAL_ro_rate_adapt_sts_9                     ro_rate_adapt_sts_9;
	pHAL_ro_rate_adapt_sts_10                    ro_rate_adapt_sts_10;
	pHAL_ro_rate_adapt_sts_11                    ro_rate_adapt_sts_11;
	pHAL_ro_rate_adapt_sts_12                    ro_rate_adapt_sts_12;
	pHAL_rg_rate_adapt_out_cnt_reset             rg_rate_adapt_out_cnt_reset;
	pHAL_ro_rate_adapt_out_cnt_pre6              ro_rate_adapt_out_cnt_pre6;
	pHAL_ro_rate_adapt_out_cnt_pre5              ro_rate_adapt_out_cnt_pre5;
	pHAL_rg_fpga_mode_set                        rg_fpga_mode_set;
	pHAL_ro_rate_adapt_ctrl_0_sts                ro_rate_adapt_ctrl_0_sts;
	pHAL_ro_rate_adapt_ctrl_1_sts                ro_rate_adapt_ctrl_1_sts;
	pHAL_ro_rate_adapt_ctrl_2_sts                ro_rate_adapt_ctrl_2_sts;
	pHAL_ro_rate_adapt_ctrl_3_sts                ro_rate_adapt_ctrl_3_sts;
	pHAL_ro_rate_adapt_ctrl_4_sts                ro_rate_adapt_ctrl_4_sts;
	pHAL_ro_rate_adapt_ctrl_5_sts                ro_rate_adapt_ctrl_5_sts;
	pHAL_ro_rate_adapt_ctrl_6_sts                ro_rate_adapt_ctrl_6_sts;
	pHAL_ro_rate_adapt_ctrl_7_sts                ro_rate_adapt_ctrl_7_sts;
	pHAL_ro_rate_adapt_ctrl_8_sts                ro_rate_adapt_ctrl_8_sts;
	pHAL_ro_rate_adapt_ctrl_9_sts                ro_rate_adapt_ctrl_9_sts;
	pHAL_ro_rate_adapt_ctrl_10_sts               ro_rate_adapt_ctrl_10_sts;
}HAL_RG_RATEADAPT, *pHAL_RG_RATEADAPT;



//======================================================================
//MODE2
//======================================================================
typedef union {
	pHAL_rg_hsgmii_pcs_ctrol_1                   rg_hsgmii_pcs_ctrol_1;
	pHAL_rg_hsgmii_pcs_ctrol_2                   rg_hsgmii_pcs_ctrol_2;
	pHAL_rg_hsgmii_pcs_ctrol_3                   rg_hsgmii_pcs_ctrol_3;
	pHAL_rg_hsgmii_pcs_ctrol_4                   rg_hsgmii_pcs_ctrol_4;
	pHAL_rg_hsgmii_pcs_ctrol_5                   rg_hsgmii_pcs_ctrol_5;
	pHAL_rg_hsgmii_pcs_ctrol_6                   rg_hsgmii_pcs_ctrol_6;
	pHAL_rg_hsgmii_pcs_gpii_0                    rg_hsgmii_pcs_gpii_0;
	pHAL_rg_hsgmii_mode_probe_sel                rg_hsgmii_mode_probe_sel;
	pHAL_rg_hsgmii_mode_interrupt                rg_hsgmii_mode_interrupt;
	pHAL_rg_hsgmii_pcs_gpii_1                    rg_hsgmii_pcs_gpii_1;
	pHAL_rg_hsgmii_pcs_gpii_2                    rg_hsgmii_pcs_gpii_2;
	pHAL_rg_hsgmii_pcs_gpii_3                    rg_hsgmii_pcs_gpii_3;
	pHAL_rg_hsgmii_pcs_gpii_4                    rg_hsgmii_pcs_gpii_4;
	pHAL_rg_hsgmii_pcs_gpii_5                    rg_hsgmii_pcs_gpii_5;
	pHAL_rg_hsgmii_pcs_state_1                   rg_hsgmii_pcs_state_1;
	pHAL_rg_hsgmii_pcs_state_2                   rg_hsgmii_pcs_state_2;
	pHAL_rg_hsgmii_pcs_state_3                   rg_hsgmii_pcs_state_3;
	pHAL_rg_hsgmii_pcs_state_4                   rg_hsgmii_pcs_state_4;
	pHAL_rg_hsgmii_pcs_state_5                   rg_hsgmii_pcs_state_5;
	pHAL_rg_hsgmii_pcs_state_6                   rg_hsgmii_pcs_state_6;
	pHAL_rg_hsgmii_pcs_state_7                   rg_hsgmii_pcs_state_7;
	pHAL_rg_hsgmii_pcs_state_8                   rg_hsgmii_pcs_state_8;
	pHAL_rg_hsgmii_gpii_state_1                  rg_hsgmii_gpii_state_1;
	pHAL_rg_hsgmii_gpii_state_2                  rg_hsgmii_gpii_state_2;
	pHAL_rg_hsgmii_gpii_state_3                  rg_hsgmii_gpii_state_3;
	pHAL_rg_hsgmii_gpii_state_4                  rg_hsgmii_gpii_state_4;
	pHAL_rg_hsgmii_gpii_state_5                  rg_hsgmii_gpii_state_5;
	pHAL_rg_hsgmii_gpii_state_6                  rg_hsgmii_gpii_state_6;
	pHAL_rg_hsgmii_gpii_state_7                  rg_hsgmii_gpii_state_7;
	pHAL_rg_hsgmii_gpii_state_8                  rg_hsgmii_gpii_state_8;
	pHAL_rg_hsgmii_gpii_state_9                  rg_hsgmii_gpii_state_9;
	pHAL_rg_hsgmii_gpii_state_10                 rg_hsgmii_gpii_state_10;
	pHAL_rg_hsgmii_gpii_state_11                 rg_hsgmii_gpii_state_11;
	pHAL_rg_hsgmii_gpii_state_12                 rg_hsgmii_gpii_state_12;
	pHAL_rg_hsgmii_gpii_state_13                 rg_hsgmii_gpii_state_13;
	pHAL_rg_hsgmii_pcs_state_9                   rg_hsgmii_pcs_state_9;
	pHAL_rg_hsgmii_pcs_int_state                 rg_hsgmii_pcs_int_state;
}HAL_RG_PCS2, *pHAL_RG_PCS2;





//=========================================================================================================
//PHYA
//======================================================================
typedef union {
	pHAL_sgmii_reg_phya_0                        sgmii_reg_phya_0;
	pHAL_sgmii_reg_phya_1                        sgmii_reg_phya_1;
	pHAL_sgmii_reg_phya_2                        sgmii_reg_phya_2;
	pHAL_sgmii_reg_phya_3                        sgmii_reg_phya_3;
	pHAL_sgmii_reg_phya_4                        sgmii_reg_phya_4;
	pHAL_sgmii_reg_phya_5                        sgmii_reg_phya_5;
	pHAL_sgmii_reg_phya_6                        sgmii_reg_phya_6;
	pHAL_sgmii_reg_phya_7                        sgmii_reg_phya_7;
	pHAL_sgmii_reg_phya_8                        sgmii_reg_phya_8;
	pHAL_sgmii_reg_phya_9                        sgmii_reg_phya_9;
	pHAL_sgmii_reg_phya_10                       sgmii_reg_phya_10;
	pHAL_sgmii_reg_phya_11                       sgmii_reg_phya_11;
	pHAL_sgmii_reg_phya_12                       sgmii_reg_phya_12;
	pHAL_sgmii_reg_phya_13                       sgmii_reg_phya_13;
	pHAL_sgmii_reg_phya_14                       sgmii_reg_phya_14;
	pHAL_sgmii_reg_phya_15                       sgmii_reg_phya_15;
	pHAL_sgmii_reg_phya_16                       sgmii_reg_phya_16;
	pHAL_sgmii_reg_phya_17                       sgmii_reg_phya_17;
	pHAL_sgmii_reg_phya_18                       sgmii_reg_phya_18;
	pHAL_sgmii_reg_phya_19                       sgmii_reg_phya_19;
	pHAL_sgmii_reg_phya_20                       sgmii_reg_phya_20;
	pHAL_sgmii_reg_phya_21                       sgmii_reg_phya_21;
	pHAL_sgmii_reg_phya_22                       sgmii_reg_phya_22;
	pHAL_sgmii_reg_phya_23                       sgmii_reg_phya_23;
	pHAL_sgmii_reg_phya_24                       sgmii_reg_phya_24;
	pHAL_sgmii_reg_phya_25                       sgmii_reg_phya_25;
	pHAL_sgmii_reg_phya_26                       sgmii_reg_phya_26;
	pHAL_sgmii_reg_phya_27                       sgmii_reg_phya_27;
	pHAL_sgmii_reg_phya_28                       sgmii_reg_phya_28;
	pHAL_sgmii_reg_phya_29                       sgmii_reg_phya_29;
	pHAL_sgmii_reg_phya_30                       sgmii_reg_phya_30;
	pHAL_sgmii_reg_phya_31                       sgmii_reg_phya_31;
	pHAL_sgmii_reg_phya_32                       sgmii_reg_phya_32;
	pHAL_sgmii_reg_phya_33                       sgmii_reg_phya_33;
	pHAL_sgmii_reg_phya_34                       sgmii_reg_phya_34;
	pHAL_sgmii_reg_phya_35                       sgmii_reg_phya_35;
	pHAL_sgmii_reg_phya_36                       sgmii_reg_phya_36;
	pHAL_sgmii_reg_phya_37                       sgmii_reg_phya_37;
	pHAL_sgmii_reg_phya_38                       sgmii_reg_phya_38;
	pHAL_sgmii_reg_phya_39                       sgmii_reg_phya_39;
	pHAL_sgmii_reg_phya_40                       sgmii_reg_phya_40;
	pHAL_sgmii_reg_phya_41                       sgmii_reg_phya_41;
	pHAL_sgmii_reg_phya_42                       sgmii_reg_phya_42;
	pHAL_sgmii_reg_phya_43                       sgmii_reg_phya_43;
	pHAL_sgmii_reg_phya_44                       sgmii_reg_phya_44;
	pHAL_sgmii_reg_phya_45                       sgmii_reg_phya_45;
	pHAL_sgmii_reg_phya_46                       sgmii_reg_phya_46;
	pHAL_sgmii_reg_phya_47                       sgmii_reg_phya_47;
	pHAL_sgmii_reg_phya_48                       sgmii_reg_phya_48;
	pHAL_sgmii_reg_phya_49                       sgmii_reg_phya_49;
	pHAL_sgmii_reg_phya_50                       sgmii_reg_phya_50;
	pHAL_sgmii_reg_phya_51                       sgmii_reg_phya_51;
	pHAL_sgmii_reg_phya_52                       sgmii_reg_phya_52;
	pHAL_sgmii_reg_phya_53                       sgmii_reg_phya_53;
	pHAL_sgmii_reg_phya_54                       sgmii_reg_phya_54;
	pHAL_sgmii_reg_phya_55                       sgmii_reg_phya_55;
	pHAL_sgmii_reg_phya_56                       sgmii_reg_phya_56;
	pHAL_sgmii_reg_phya_57                       sgmii_reg_phya_57;
	pHAL_sgmii_reg_phya_58                       sgmii_reg_phya_58;
	pHAL_sgmii_reg_phya_59                       sgmii_reg_phya_59;
	pHAL_sgmii_reg_phya_60                       sgmii_reg_phya_60;
	pHAL_sgmii_reg_phya_61                       sgmii_reg_phya_61;
	pHAL_sgmii_reg_phya_62                       sgmii_reg_phya_62;
	pHAL_sgmii_reg_phya_63                       sgmii_reg_phya_63;
	pHAL_sgmii_reg_phya_64                       sgmii_reg_phya_64;
	pHAL_sgmii_reg_phya_65                       sgmii_reg_phya_65;
	pHAL_sgmii_reg_phya_66                       sgmii_reg_phya_66;
	pHAL_sgmii_reg_phya_67                       sgmii_reg_phya_67;
	pHAL_sgmii_reg_phya_68                       sgmii_reg_phya_68;
	pHAL_sgmii_reg_phya_69                       sgmii_reg_phya_69;
	pHAL_sgmii_reg_phya_70                       sgmii_reg_phya_70;
	pHAL_sgmii_reg_phya_71                       sgmii_reg_phya_71;
	pHAL_sgmii_reg_phya_72                       sgmii_reg_phya_72;
	pHAL_sgmii_reg_phya_73                       sgmii_reg_phya_73;
	pHAL_sgmii_reg_phya_74                       sgmii_reg_phya_74;
	pHAL_sgmii_reg_phya_75                       sgmii_reg_phya_75;
	pHAL_sgmii_reg_phya_76                       sgmii_reg_phya_76;
	pHAL_sgmii_reg_phya_77                       sgmii_reg_phya_77;
	pHAL_sgmii_reg_phya_78                       sgmii_reg_phya_78;
	pHAL_sgmii_reg_phya_79                       sgmii_reg_phya_79;
	pHAL_sgmii_reg_phya_80                       sgmii_reg_phya_80;
	pHAL_sgmii_reg_phya_81                       sgmii_reg_phya_81;
	pHAL_sgmii_reg_phya_82                       sgmii_reg_phya_82;
	pHAL_sgmii_reg_interrupt_sel                 sgmii_reg_interrupt_sel;
	pHAL_sgmii_reg_probe_sel                     sgmii_reg_probe_sel;
	pHAL_sgmii_fpga_mode_control                 sgmii_fpga_mode_control;
	pHAL_sgmii_async_fifo_control                sgmii_async_fifo_control;
	pHAL_sgmii_reg_dwn_shift_ctrl                sgmii_reg_dwn_shift_ctrl;
	pHAL_sgmii_reg_dwn_shift_timer_ctrl          sgmii_reg_dwn_shift_timer_ctrl;
	pHAL_sgmii_ro_dwn_shift_status               sgmii_ro_dwn_shift_status;
	pHAL_sgmii_ro_phya_0                         sgmii_ro_phya_0;
	pHAL_sgmii_ro_phya_1                         sgmii_ro_phya_1;
	pHAL_sgmii_ro_phya_2                         sgmii_ro_phya_2;
	pHAL_sgmii_ro_phya_3                         sgmii_ro_phya_3;
	pHAL_sgmii_ro_phya_4                         sgmii_ro_phya_4;
	pHAL_sgmii_ro_phya_5                         sgmii_ro_phya_5;
	pHAL_sgmii_ro_phya_6                         sgmii_ro_phya_6;
	pHAL_sgmii_ro_phya_7                         sgmii_ro_phya_7;
	pHAL_sgmii_ro_phya_8                         sgmii_ro_phya_8;
	pHAL_sgmii_ro_phya_9                         sgmii_ro_phya_9;
	pHAL_sgmii_ro_phya_10                        sgmii_ro_phya_10;
	pHAL_sgmii_ro_phya_11                        sgmii_ro_phya_11;
	pHAL_sgmii_ro_phya_12                        sgmii_ro_phya_12;
	pHAL_sgmii_ro_phya_13                        sgmii_ro_phya_13;
	pHAL_sgmii_ro_phya_14                        sgmii_ro_phya_14;
	pHAL_sgmii_ro_phya_15                        sgmii_ro_phya_15;
	pHAL_sgmii_ro_phya_16                        sgmii_ro_phya_16;
	pHAL_sgmii_ro_phya_17                        sgmii_ro_phya_17;
	pHAL_sgmii_ro_phya_18                        sgmii_ro_phya_18;
	pHAL_sgmii_ro_phya_19                        sgmii_ro_phya_19;
	pHAL_sgmii_ro_phya_20                        sgmii_ro_phya_20;
	pHAL_sgmii_ro_phya_21                        sgmii_ro_phya_21;
	pHAL_sgmii_ro_phya_22                        sgmii_ro_phya_22;
	pHAL_sgmii_ro_phya_23                        sgmii_ro_phya_23;
	pHAL_sgmii_ro_phya_24                        sgmii_ro_phya_24;
	pHAL_sgmii_ro_phya_25                        sgmii_ro_phya_25;
	pHAL_sgmii_ro_phya_26                        sgmii_ro_phya_26;
	pHAL_sgmii_ro_phya_afifo                     sgmii_ro_phya_afifo;
	pHAL_sgmii_ro_phya_afifo_cnt_0               sgmii_ro_phya_afifo_cnt_0;
	pHAL_sgmii_ro_phya_afifo_cnt_1               sgmii_ro_phya_afifo_cnt_1;
	pHAL_sgmii_ro_phya_afifo_cnt_2               sgmii_ro_phya_afifo_cnt_2;
	pHAL_sgmii_ro_phya_afifo_cnt_3               sgmii_ro_phya_afifo_cnt_3;
	pHAL_sgmii_led_ctrl                          sgmii_led_ctrl;
	pHAL_sgmii_led_dur                           sgmii_led_dur;
	pHAL_sgmii_led01_mask                        sgmii_led01_mask;
	pHAL_sgmii_led23_mask                        sgmii_led23_mask;
	pHAL_sgmii_led_event_status                  sgmii_led_event_status;
	pHAL_debug_data_ctrl                         debug_data_ctrl;
	pHAL_debug_data_dump_0                       debug_data_dump_0;
	pHAL_debug_data_dump_1                       debug_data_dump_1;
	pHAL_debug_data_dump_2                       debug_data_dump_2;
	pHAL_debug_data_dump_3                       debug_data_dump_3;
	pHAL_debug_data_dump_4                       debug_data_dump_4;
	pHAL_debug_data_dump_5                       debug_data_dump_5;
	pHAL_debug_data_dump_6                       debug_data_dump_6;
	pHAL_debug_data_dump_7                       debug_data_dump_7;
	pHAL_debug_data_dump_8                       debug_data_dump_8;
	pHAL_debug_data_dump_9                       debug_data_dump_9;
	pHAL_debug_data_dump_10                      debug_data_dump_10;
	pHAL_debug_data_dump_11                      debug_data_dump_11;
	pHAL_debug_data_dump_12                      debug_data_dump_12;
	pHAL_debug_data_dump_13                      debug_data_dump_13;
	pHAL_debug_data_dump_14                      debug_data_dump_14;
	pHAL_debug_data_dump_15                      debug_data_dump_15;
	pHAL_debug_data_dump_16                      debug_data_dump_16;
	pHAL_debug_data_dump_17                      debug_data_dump_17;
	pHAL_debug_data_dump_18                      debug_data_dump_18;
	pHAL_debug_data_dump_19                      debug_data_dump_19;
	pHAL_debug_data_dump_20                      debug_data_dump_20;
	pHAL_debug_data_dump_21                      debug_data_dump_21;
	pHAL_debug_data_dump_22                      debug_data_dump_22;
	pHAL_debug_data_dump_23                      debug_data_dump_23;
	pHAL_debug_data_dump_24                      debug_data_dump_24;
	pHAL_debug_data_dump_25                      debug_data_dump_25;
	pHAL_debug_data_dump_26                      debug_data_dump_26;
	pHAL_debug_data_dump_27                      debug_data_dump_27;
	pHAL_debug_data_dump_28                      debug_data_dump_28;
	pHAL_debug_data_dump_29                      debug_data_dump_29;
	pHAL_debug_data_dump_30                      debug_data_dump_30;
	pHAL_rg_host_control_1                       rg_host_control_1;
	pHAL_rg_host_control_2                       rg_host_control_2;
	pHAL_rg_irq_mask                             rg_irq_mask;
	pHAL_rgs_irq_status                          rgs_irq_status;
}HAL_RG_PHYA, *pHAL_RG_PHYA;



//=========================================================================================================
//TOP
//======================================================================

typedef union {

    HAL_RG_PHYA      phya;
    HAL_RG_AN        an;
    HAL_RG_RATEADAPT ra;
    HAL_RG_PCS2      pcs2;
    uint32           rg32;
}HAL_RG_TOP;

#endif //_SGMII_HAL_TOP_H_

