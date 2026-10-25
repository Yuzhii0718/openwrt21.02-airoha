/*
 * i2s_afe_common.h  --  Econet audio driver common definitions
 *
 * Copyright (c) 2020 Econet Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _I2S_AFE_COMMON_H_
#define _I2S_AFE_COMMON_H_

#define COMMON_CLOCK_FRAMEWORK_API
/* #define DEBUG_AFE_REGISTER_RW */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/regmap.h>
#include <linux/hrtimer.h>
#include <linux/of_gpio.h>
#include <sound/asound.h>
#include "i2s-reg.h"

#define SPDIF_CHSTS_NUM			6
#define SPDIF_USERCODE_NUM		12
#define I2S_ETDM_MAX_CHANNELS	16
/* #define CALI_RESULT_CHECK */

enum {
	I2S_AFE_MEMIF_DL1,
	I2S_AFE_MEMIF_UL1,
	I2S_AFE_MEMIF_NUM,
	I2S_AFE_BACKEND_BASE = I2S_AFE_MEMIF_NUM,
	I2S_AFE_IO_ETDM1_OUT = I2S_AFE_BACKEND_BASE,
	I2S_AFE_IO_ETDM1_IN,
	I2S_AFE_IO_ETDM2_OUT,
	I2S_AFE_IO_ETDM2_IN,
	I2S_AFE_IO_PCM1,
	I2S_AFE_IO_VIRTUAL_DL_SRC,
	I2S_AFE_IO_DMIC,
	I2S_AFE_IO_INT_ADDA,
	I2S_AFE_IO_GASRC0,
	I2S_AFE_IO_GASRC1,
	I2S_AFE_IO_GASRC2,
	I2S_AFE_IO_GASRC3,
	I2S_AFE_IO_SPDIF_OUT,
	I2S_AFE_IO_SPDIF_IN,
	I2S_AFE_IO_MULTI_IN,
	I2S_AFE_BACKEND_END,
	I2S_AFE_BACKEND_NUM = (I2S_AFE_BACKEND_END -
				  I2S_AFE_BACKEND_BASE),
};

enum {
	I2S_AFE_IRQ1, /* SPDIF OUT */
	I2S_AFE_IRQ2, /* SPDIF IN DATA */
	I2S_AFE_IRQ_NUM,
};

enum {
	I2S_TOP_CG_AFE,
	I2S_TOP_CG_APLL,
	I2S_TOP_CG_APLL2,
	I2S_TOP_CG_DAC,
	I2S_TOP_CG_DAC_PREDIS,
	I2S_TOP_CG_ADC,
	I2S_TOP_CG_TML,
	I2S_TOP_CG_UPLINK_TML,
	I2S_TOP_CG_I2S_IN,
	I2S_TOP_CG_TDM_IN,
	I2S_TOP_CG_I2S_OUT,
	I2S_TOP_CG_TDM_OUT,
	I2S_TOP_CG_ASRC11,
	I2S_TOP_CG_ASRC12,
	I2S_TOP_CG_DL_ASRC,
	I2S_TOP_CG_A1SYS,
	I2S_TOP_CG_A2SYS,
	I2S_TOP_CG_AFE_CONN,
	I2S_TOP_CG_PCMIF,
	I2S_TOP_CG_GASRC0,
	I2S_TOP_CG_GASRC1,
	I2S_TOP_CG_GASRC2,
	I2S_TOP_CG_GASRC3,
	I2S_TOP_CG_DMIC0,
	I2S_TOP_CG_DMIC1,
	I2S_TOP_CG_DMIC2,
	I2S_TOP_CG_DMIC3,
	I2S_TOP_CG_A1SYS_TIMING,
	I2S_TOP_CG_A2SYS_TIMING,
	I2S_TOP_CG_SPDIF_OUT,
	I2S_TOP_CG_MULTI_IN,
	I2S_TOP_CG_INTDIR,
	I2S_TOP_CG_NUM
};

enum {
	I2S_CLK_TOP_AUD_26M,
	I2S_CLK_TOP_AUD_BUS,
	I2S_CLK_FA1SYS,
	I2S_CLK_FA2SYS,
	I2S_CLK_HAPLL1,
	I2S_CLK_HAPLL2,
	I2S_CLK_AUD1,
	I2S_CLK_AUD2,
	I2S_CLK_FASM_L,
	I2S_CLK_FASM_M,
	I2S_CLK_FASM_H,
	I2S_CLK_SPDIF_IN,
	I2S_CLK_APLL12_DIV0,
	I2S_CLK_APLL12_DIV3,
	I2S_CLK_APLL12_DIV4,
	I2S_CLK_APLL12_DIV6,
	I2S_CLK_I2S0_M_SEL,
	I2S_CLK_I2S3_M_SEL,
	I2S_CLK_I2S4_M_SEL,
	I2S_CLK_I2S6_M_SEL,
	I2S_CLK_APLL1,
	I2S_CLK_APLL2,
	I2S_CLK_NUM
};

enum {
	I2S_ETDM1 = 0,
	I2S_ETDM_SETS,
};

enum {
	I2S_ETDM_DATA_ONE_PIN = 0,
	I2S_ETDM_DATA_MULTI_PIN,
};

enum {
	I2S_ETDM_SEPARATE_CLOCK = 0,
	I2S_ETDM_SHARED_CLOCK,
};

enum {
	I2S_ETDM_SYNC_NONE = 0,
	I2S_ETDM_SYNC_FROM_IN1,
	I2S_ETDM_SYNC_FROM_IN2,
	I2S_ETDM_SYNC_FROM_OUT1,
	I2S_ETDM_SYNC_FROM_OUT2,
};

enum {
	I2S_ETDM_FORMAT_I2S = 0,
	I2S_ETDM_FORMAT_LJ,
	I2S_ETDM_FORMAT_RJ,
	I2S_ETDM_FORMAT_EIAJ,
	I2S_ETDM_FORMAT_DSPA,
	I2S_ETDM_FORMAT_DSPB,
};

enum {
	I2S_PCM_FORMAT_I2S = 0,
	I2S_PCM_FORMAT_EIAJ,
	I2S_PCM_FORMAT_PCMA,
	I2S_PCM_FORMAT_PCMB,
};

enum {
	I2S_MULTI_IN_FORMAT_I2S = 0,
	I2S_MULTI_IN_FORMAT_LJ,
	I2S_MULTI_IN_FORMAT_RJ,
};

enum {
	I2S_MULTI_IN_UNKNOWN = -1,
	I2S_MULTI_IN_ROUGH_PCM = 0,
	I2S_MULTI_IN_ROUGH_RAW,
	I2S_MULTI_IN_ROUGH_DTSCD16,
	I2S_MULTI_IN_ROUGH_DTSCD14,
};

enum {
	I2S_FS_8K = 0,
	I2S_FS_12K,
	I2S_FS_16K,
	I2S_FS_24K,
	I2S_FS_32K,
	I2S_FS_48K,
	I2S_FS_96K,
	I2S_FS_192K,
	I2S_FS_384K,
	I2S_FS_ETDMOUT1_1X_EN,
	I2S_FS_ETDMOUT2_1X_EN,
	I2S_FS_ETDMIN1_1X_EN = 12,
	I2S_FS_ETDMIN2_1X_EN,
	I2S_FS_EXT_PCM_1X_EN = 15,
	I2S_FS_7D35K,
	I2S_FS_11D025K,
	I2S_FS_14D7K,
	I2S_FS_22D05K,
	I2S_FS_29D4K,
	I2S_FS_44D1K,
	I2S_FS_88D2K,
	I2S_FS_176D4K,
	I2S_FS_352D8K,
	I2S_FS_ETDMIN1_NX_EN,
	I2S_FS_ETDMIN2_NX_EN,
	I2S_FS_AMIC_1X_EN_ASYNC = 28,
};

enum {
	SPDIF_IN_PORT_NONE = 0,
	SPDIF_IN_PORT_OPT,
	SPDIF_IN_PORT_COAXIAL,
	SPDIF_IN_PORT_ARC,
	SPDIF_IN_PORT_NUM
};

enum {
	SPDIF_IN_MUX_0 = 0,
	SPDIF_IN_MUX_1,
	SPDIF_IN_MUX_2,
};

enum {
	I2S_ETDM_FORCE_ON_DEFAULT = 0,
	I2S_ETDM_FORCE_ON_1ST_TRIGGER,
};

enum {
	I2S_AFE_DEBUGFS_ETDM,
	I2S_AFE_DEBUGFS_MEMIF,
	I2S_AFE_DEBUGFS_IRQ,
	I2S_AFE_DEBUGFS_CONN,
	I2S_AFE_DEBUGFS_ADDA,
	I2S_AFE_DEBUGFS_GASRC,
	I2S_AFE_DEBUGFS_SPDIF,
	I2S_AFE_DEBUGFS_DBG,
	I2S_AFE_DEBUGFS_NUM,
};

struct i2s_fe_dai_data {
	bool slave_mode;
	bool use_sram;
	unsigned int sram_phy_addr;
	void __iomem *sram_vir_addr;
	unsigned int sram_size;
	unsigned int prealloc_size;
	unsigned int pbuf_size_conf;
	unsigned int min_hw_irq_period_us;
};

struct i2s_be_dai_data {
	bool prepared[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int fmt_mode;
};

struct i2s_etdm_data {
	int occupied[SNDRV_PCM_STREAM_LAST + 1];
	int active[SNDRV_PCM_STREAM_LAST + 1];
	bool slave_mode[SNDRV_PCM_STREAM_LAST + 1];
	bool lrck_inv[SNDRV_PCM_STREAM_LAST + 1];
	bool bck_inv[SNDRV_PCM_STREAM_LAST + 1];
	bool enable_interlink[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int lrck_width[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int data_mode[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int format[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int mclk_freq[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int clock_mode;
	bool in_disable_ch[I2S_ETDM_MAX_CHANNELS];
	bool force_on[SNDRV_PCM_STREAM_LAST + 1];
	bool force_on_status[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int force_on_policy[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int force_rate[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int force_channels[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int force_bit_width[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int sync_source[SNDRV_PCM_STREAM_LAST + 1];
	bool int_lrck_inv[SNDRV_PCM_STREAM_LAST + 1];
	bool int_bck_inv[SNDRV_PCM_STREAM_LAST + 1];
	unsigned int enable_seq[SNDRV_PCM_STREAM_LAST + 1];
};

struct i2s_pcm_intf_data {
	bool slave_mode;
	bool lrck_inv;
	bool bck_inv;
	unsigned int format;
};

struct i2s_multi_in_data {
	bool lrck_inv;
	bool bck_inv;
	unsigned int format;
	unsigned int period_update_bytes;
	unsigned int notify_irq_count;
	unsigned int current_irq_count;
};

struct i2s_spdif_in_subdata {
	unsigned int rate;
	unsigned int user_code[SPDIF_USERCODE_NUM];
	unsigned int ch_status[SPDIF_CHSTS_NUM];
	unsigned int stream_type;
};

struct i2s_spdif_in_data {
	unsigned int port;
	unsigned int ports_mux[SPDIF_IN_PORT_NUM];
	struct i2s_spdif_in_subdata subdata;
};

struct i2s_multi_in_mon_info {
	int rough_type;
	int data_type;
	int bitstream_number;
	int pc_b12_to_b5;
};

struct i2s_etdm_ctrl_reg {
	unsigned int con0;
	unsigned int con1;
	unsigned int con2;
	unsigned int con3;
	unsigned int con4;
};

struct i2s_control_data {
	bool bypass_cm0;
	bool bypass_cm1;
	bool spdif_output_iec61937;
};

#define DMIC_MAX_CH (8)

struct i2s_dmic_data {
	bool two_wire_mode;
	unsigned int clk_phase_sel_ch1;
	unsigned int clk_phase_sel_ch2;
	unsigned int dmic_src_sel[DMIC_MAX_CH];
	bool iir_on;
	unsigned int setup_time_us;
};

enum {
	I2S_GASRC0 = 0,
	I2S_GASRC1,
	I2S_GASRC2,
	I2S_GASRC3,
	I2S_GASRC_NUM,
};

struct i2s_gasrc_ctrl_reg {
	unsigned int con0;
	unsigned int con1;
	unsigned int con2;
	unsigned int con3;
	unsigned int con4;
	unsigned int con6;
	unsigned int con7;
	unsigned int con10;
	unsigned int con11;
	unsigned int con13;
	unsigned int con14;
};

struct i2s_gasrc_data {
	unsigned int input_mux;
	unsigned int output_mux;
	bool cali_tx;
	bool cali_rx;
	bool one_heart;
	bool iir_on;
	bool duplex;
	bool op_freq_45m;
	unsigned int cali_cycles;
	bool re_enable[SNDRV_PCM_STREAM_LAST + 1];
	atomic_t ref_cnt;
};

enum i2s_afe_gasrc_mux {
	MUX_GASRC_8CH = 0,
	MUX_GASRC_6CH,
	MUX_GASRC_4CH,
	MUX_GASRC_2CH,
};

struct i2s_afe_gasrc_mux_map {
	int gasrc_id;
	int idx;
	int mux;
};

enum i2s_afe_gasrc_lrck_sel_src {
	I2S_AFE_GASRC_LRCK_SEL_ETDM_IN2 = 0,
	I2S_AFE_GASRC_LRCK_SEL_ETDM_IN1,
	I2S_AFE_GASRC_LRCK_SEL_ETDM_OUT2,
	I2S_AFE_GASRC_LRCK_SEL_ETDM_OUT1,
	I2S_AFE_GASRC_LRCK_SEL_PCM_IF,
	I2S_AFE_GASRC_LRCK_SEL_UL_VIRTUAL,
};

enum {
	I2S_APLL1_RATE = 180633600,
	I2S_APLL2_RATE = 196608000,
};

enum {
	CLK_TUNE_SPDIF_IN = 0,
	CLK_TUNE_MULTI_IN,
	CLK_TUNE_SOURCE_NUM,
};

enum {
	TUNE_PHASE_INIT = 0,
	TUNE_PHASE_START_CALI_PROC,
	TUNE_PHASE_GET_CALI_RESULT,
	TUNE_PHASE_CHECK_OUTPUT_ACTIVE,
	TUNE_PHASE_ADJUST_APLL_RATE,
	TUNE_PHASE_ADJUST_DONE,
	TUNE_PHASE_ALL_DONE,
	TUNE_PHASE_ABORT,
};

enum {
	PBUF_SIZE_FULL = 0,
	PBUF_SIZE_HALF,
	PBUF_SIZE_QUARTER,
	PBUF_SIZE_EIGHTH,
	PBUF_SIZE_CONF_NUM,
};

enum {
	EN_IN_PREP_DIS_IN_SD = 0,
	EN_DIS_IN_TRIGGER,
};

struct ext_clk_tune_property {
	bool do_tune;
	unsigned int tune_id;
	unsigned int period_ms;
	unsigned int adj_step_ppm;
	unsigned int adj_step_us;
};

struct i2s_afe_ext_clk_tune_data {
	struct ext_clk_tune_property props[CLK_TUNE_SOURCE_NUM];
	struct ext_clk_tune_property *working;
	struct delayed_work clk_tune_work;
	int working_phase;
	bool start;
	unsigned int asrc_id;
	unsigned int cali_cycles;
	unsigned int input_rate;
	u64 cali_retry_time;
	int cali_retry_cnt;
	u64 cali_input_rate_10uhz;
	unsigned long apll1_rate;
	unsigned long apll2_rate;
	unsigned long apll_target_rate;
	unsigned long apll_current_rate;
	unsigned long apll_restore_rate;
	unsigned long apll_step_hz;
	unsigned int outputs;
	unsigned int output_rate;
	void *apll_clk;
	void *afe;
};

enum {
	I2S_AFE_TDMOUT_CONN_I0 = 0,
	I2S_AFE_TDMOUT_CONN_I15 = 15,
	I2S_AFE_TDMOUT_CONN_CFG_NUM = 16,
};

struct i2s_afe_tdmout_conn_data {
	unsigned int out_cfg[I2S_AFE_TDMOUT_CONN_CFG_NUM];
};

#ifdef CALI_RESULT_CHECK
struct i2s_cali_result {
	struct hrtimer cali_hrt;
	int cnt;
	snd_pcm_sframes_t max;
	snd_pcm_sframes_t min;
	snd_pcm_sframes_t avg;
	snd_pcm_sframes_t cur;
	u64 sum;
	void *afe;
};
#endif

enum {
	AFE_PIN_STATE_DEFAULT = 0,
	AFE_PIN_STATE_ETDM1_OUT_ON,
	AFE_PIN_STATE_ETDM1_OUT_OFF,
	AFE_PIN_STATE_ETDM1_IN_ON,
	AFE_PIN_STATE_ETDM1_IN_OFF,
	AFE_PIN_STATE_ETDM2_OUT_ON,
	AFE_PIN_STATE_ETDM2_OUT_OFF,
	AFE_PIN_STATE_ETDM2_IN_ON,
	AFE_PIN_STATE_ETDM2_IN_OFF,
	AFE_PIN_STATE_MAX
};

struct i2s_afe_private {
	struct clk *clocks[I2S_CLK_NUM];
	struct i2s_fe_dai_data fe_data[I2S_AFE_MEMIF_NUM];
	struct i2s_be_dai_data be_data[I2S_AFE_BACKEND_NUM];
	struct i2s_etdm_data etdm_data[I2S_ETDM_SETS];
	struct i2s_pcm_intf_data pcm_intf_data;
	struct i2s_multi_in_data multi_in_data;
	struct i2s_spdif_in_data spdif_in_data;
	struct i2s_control_data ctrl_data;
	struct i2s_dmic_data dmic_data;
	struct i2s_afe_tdmout_conn_data tdmo_conn;
	struct i2s_gasrc_data gasrc_data[I2S_GASRC_NUM];
	struct i2s_afe_ext_clk_tune_data clk_tune;
	struct i2s_multi_in_mon_info multi_in_mon_info;
#ifdef CALI_RESULT_CHECK
	struct i2s_cali_result cali_res;
#endif
	int afe_on_ref_cnt;
	int top_cg_ref_cnt[I2S_TOP_CG_NUM];
	void __iomem *afe_sram_va;
	u32 afe_sram_pa;
	u32 afe_sram_size;
	bool use_bypass_afe_pinmux;
	u32 be_active_status;
	bool dl8_enable_24ch_output;
	u32 dl8_max_main_channels;
	/* locks */
	spinlock_t afe_ctrl_lock;
	spinlock_t spdifin_ctrl_lock;
	struct regmap *topckgen;
	struct regmap *scpsys;
	int block_dpidle_ref_cnt;
	struct mutex block_dpidle_mutex;
	struct snd_card *card;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pin_states[AFE_PIN_STATE_MAX];
	bool handle_etdm_force_in_suspend_resume;
#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_dentry[I2S_AFE_DEBUGFS_NUM];
#endif
};

enum {
	MTK_AFE_MEMIF_DL1,
	MTK_AFE_MEMIF_DL2,
	MTK_AFE_MEMIF_VUL,
	MTK_AFE_MEMIF_DAI,
	MTK_AFE_MEMIF_AWB,
	MTK_AFE_MEMIF_MOD_DAI,
	MTK_AFE_MEMIF_HDMI,
	MTK_AFE_MEMIF_NUM,
	MTK_AFE_IO_MOD_PCM1 = MTK_AFE_MEMIF_NUM,
	MTK_AFE_IO_MOD_PCM2,
	MTK_AFE_IO_PMIC,
	MTK_AFE_IO_I2S,
	MTK_AFE_IO_2ND_I2S,
	MTK_AFE_IO_HW_GAIN1,
	MTK_AFE_IO_HW_GAIN2,
	MTK_AFE_IO_MRG_O,
	MTK_AFE_IO_MRG_I,
	MTK_AFE_IO_DAIBT,
	MTK_AFE_IO_HDMI,
};

enum {
	MTK_AFE_IRQ_1,
	MTK_AFE_IRQ_2,
	MTK_AFE_IRQ_3,
	MTK_AFE_IRQ_4,
	MTK_AFE_IRQ_5,
	MTK_AFE_IRQ_6,
	MTK_AFE_IRQ_7,
	MTK_AFE_IRQ_8,
	MTK_AFE_IRQ_NUM,
};

enum {
	MTK_CLK_INFRASYS_AUD,
	MTK_CLK_TOP_PDN_AUD,
	MTK_CLK_TOP_PDN_AUD_BUS,
	MTK_CLK_I2S0_M,
	MTK_CLK_I2S1_M,
	MTK_CLK_I2S2_M,
	MTK_CLK_I2S3_M,
	MTK_CLK_I2S3_B,
	MTK_CLK_BCK0,
	MTK_CLK_BCK1,
	MTK_CLK_NUM
};

struct mtk_afe_memif_data {
	int id;
	const char *name;
	int reg_ofs_base;
	int reg_ofs_cur;
	int fs_shift;
	int mono_shift;
	int enable_shift;
	int irq_reg_cnt;
	int irq_cnt_shift;
	int irq_en_shift;
	int irq_fs_shift;
	int irq_clr_shift;
};

struct mtk_afe_memif {
	unsigned int phys_buf_addr;
	int buffer_size;
	unsigned int hw_ptr;		/* Previous IRQ's HW ptr */
	struct snd_pcm_substream *substream;
	const struct mtk_afe_memif_data *data;
	const int *irqdata;
};



bool i2s_afe_rate_supported(unsigned int rate, unsigned int id);
bool i2s_afe_channel_supported(unsigned int channel, unsigned int id);

#endif
