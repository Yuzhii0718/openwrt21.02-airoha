#include <rtl8373_asicdrv.h>

#define NO_FLOAT

#define TM0_CTRL2_0876	0x2D8
#define TM0_CTRL3_0876	0x2DC
#define TM0_RESULT0_0876	0x2E0
#define TM0_RESULT1_0876	0x2E4
#define TM0_RESULT2_0876	0x2E8
#define TM0_RESULT3_0876	0x2EC
#define TM0_RESULT4_0876	0x2F0

#define TM1_CTRL0_0876	0x330
#define TM1_CTRL1_0876	0x334
#define TM1_CTRL2_0876	0x338
#define TM1_CTRL3_0876	0x33C
#define TM1_RESULT0_0876	0x340
#define TM1_RESULT1_0876	0x344
#define TM1_RESULT2_0876	0x348
#define TM1_RESULT3_0876	0x34C
#define TM1_RESULT4_0876	0x350
#define TM1_CTRL_DUMY_0		0x354  //dummy_reg 用于存储Delta T1

rtk_uint32 pow2(rtk_uint16 base, rtk_uint32 bit);
rtk_api_ret_t c2t_true_val(rtk_int32 cplmt_mode, rtk_int32 sign_bit, rtk_int32* ture_val);
extern rtk_api_ret_t t2c_cplmt_mode(rtk_int32 ture_val, rtk_int32 sign_bit, rtk_int32* cplmt_mode);
rtk_api_ret_t  unlatch_tm(rtk_uint16 tm_index);
rtk_api_ret_t  latch_tm(rtk_uint16 tm_index);
rtk_api_ret_t  cmp_low_temp_en(rtk_uint16 tm_index, rtk_enable_t enable);

rtk_api_ret_t cmp_high_temp_en(rtk_uint16 tm_index, rtk_enable_t enable);

rtk_api_ret_t  get_cmp_high_temp(rtk_uint32 tm_index, rtk_int32* HIGH_THR);
rtk_api_ret_t get_cmp_low_temp(rtk_uint32 tm_index ,rtk_int32* LOW_THR);
rtk_api_ret_t  set_cmp_low_temp(rtk_uint32 tm_index, rtk_uint32 LOW_THR);
rtk_api_ret_t  set_cmp_high_temp(rtk_uint32 tm_index, rtk_uint32 HIGH_THR);
#ifdef NO_FLOAT
rtk_api_ret_t  get_junc_temp(rtk_uint32 tm_index, rtk_int32* temp);
rtk_api_ret_t get_reg_a(rtk_uint32 tmid, rtk_uint32* reg_a);
rtk_api_ret_t get_reg_b(rtk_uint32 tmid, rtk_int32* reg_b);
rtk_api_ret_t update_reg_a(rtk_uint32 tmid, rtk_uint32* reg_a_new);
rtk_api_ret_t update_reg_b(rtk_uint32 tmid, rtk_int32* reg_b_new);
#else
rtk_api_ret_t  get_junc_temp(rtk_uint32 tm_index, float* temp);
rtk_api_ret_t get_reg_a(rtk_uint32 tmid, float* reg_a);
rtk_api_ret_t get_reg_b(rtk_uint32 tmid, float* reg_b);
rtk_api_ret_t update_reg_a(rtk_uint32 tmid, float* reg_a_new);
rtk_api_ret_t update_reg_b(rtk_uint32 tmid, float* reg_b_new);
#endif
rtk_api_ret_t tm_init(rtk_uint32 tmid);









