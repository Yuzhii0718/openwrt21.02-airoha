/*
 * i2s-afe-pcm.c  --  Econet ALSA SoC AFE platform driver
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

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/mfd/syscon.h>
#include <linux/atomic.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include "i2s-afe-common.h"
#include "i2s-reg.h"
#include "i2s-base-afe.h"
#include "i2s-afe-platform-driver.h"
#include "i2s-afe-fe-dai.h"

/*ALSA_I2S_RECORD_TEST and ALSA_I2S_APLAY_TEST should not be defined at the same time*/
//#define ALSA_I2S_RECORD_TEST
//#define ALSA_I2S_APLAY_TEST

extern struct device* get_i2s_dev(void);
extern int get_i2s_irq(void);


//TODO: revise registers to backup
unsigned int i2s_afe_backup_list[] = {
	AFE_DAC_CON0,			
	AFE_DAC_CON1,			
	AFE_MEMIF_BURST_CFG,		
	AFE_MEMIF_BUF_MON1,			
	AFE_MEMIF_BUF_MON6,			
	ETDM_COWORK_CON0,			
	ETDM_COWORK_CON1,			
	ETDM_IN1_CON0,				
	ETDM_IN1_CON1,				
	ETDM_IN1_CON2,				
	ETDM_IN1_CON3,				
	ETDM_IN1_CON4,				
	ETDM_IN1_CON5,			
	ETDM_IN1_CON6,				
	ETDM_IN1_MONITOR,			
	ETDM_OUT1_CON0,				
	ETDM_OUT1_CON1,				
	ETDM_OUT1_CON2,				
	ETDM_OUT1_CON3,				
	ETDM_OUT1_CON4,			
	ETDM_OUT1_CON6,			
	ETDM_OUT1_CON7,			
	ETDM_OUT1_MONITOR,			
	AFE_DL1_CHK_SUM1,			
	AFE_DL1_CHK_SUM2,			
	AFE_DL1_BASE,				
	AFE_DL1_CUR,				
	AFE_DL1_END, 				
	AFE_DL1_CON0,			
	AFE_UL1_CHK_SUM1,			
	AFE_UL1_CHK_SUM2,			
	AFE_UL1_BASE,				
	AFE_UL1_END, 			
	AFE_UL1_CUR, 			
	AFE_UL1_CON0,			
	AFE_SIDEBAND0,			
	AFE_SIDEBAND1,			
	AFE_IRQ_CON0,				
	AFE_IRQ_CNT, 				
	AFE_IRQ_CNT_MON, 			
	AFE_IRQ_MON0,			
	AFE_IRQ_MON1,			
	AFE_IRQ_STS, 			
	AFE_IRQ1_CON0,			
	AFE_IRQ1_CNT,				
	AFE_IRQ1_CNT_MON,				
	AFE_IRQ1_MON0,				
	AFE_IRQ1_MON1,					
	IRQ_RECORD_MASK, 			
	IRQ_PLAY_MASK,
};


struct mtk_afe {
	/* address for ioremap audio hardware register */
	void __iomem *base_addr;
	struct device *dev;
	struct regmap *regmap;
	struct mtk_afe_memif memif[MTK_AFE_MEMIF_NUM];
	struct clk *clocks[MTK_CLK_NUM];
	unsigned int backup_regs[ARRAY_SIZE(i2s_afe_backup_list)];
	bool suspended;
};

#define PCM_STREAM_STR(x) \
	(((x) == SNDRV_PCM_STREAM_CAPTURE) ? "Capture" : "Playback")

static const int memif_specified_irqs[I2S_AFE_MEMIF_NUM] = {
	[I2S_AFE_MEMIF_DL1] = I2S_AFE_IRQ1,
	[I2S_AFE_MEMIF_UL1] = I2S_AFE_IRQ2,
};

static const struct snd_pcm_hardware i2s_afe_hardware = {
	.info = (SNDRV_PCM_INFO_MMAP |
		 SNDRV_PCM_INFO_INTERLEAVED |
		 SNDRV_PCM_INFO_MMAP_VALID),
	.buffer_bytes_max = 1024 * 1024,
	.period_bytes_min = 64,
	.period_bytes_max = 512 * 1024,
	.periods_min = 2,
	.periods_max = 256,
	.fifo_size = 0,
};

static int i2s_afe_suspend(struct device *dev, struct mtk_base_afe *afe)
{
	struct i2s_afe_private *priv = afe->platform_priv;
	struct regmap *regmap = afe->regmap;
	int i;

	dev_dbg(afe->dev, "%s suspend %d %d >>\n", __func__,
		pm_runtime_status_suspended(dev), afe->suspended);

	if (pm_runtime_status_suspended(dev) || afe->suspended)
		return 0;

	i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x0);

	afe->suspended = true;

	dev_dbg(afe->dev, "%s <<\n", __func__);

	return 0;
}

/*i2s i2s need*/
static int i2s_afe_resume(struct device *dev, struct mtk_base_afe *afe)
{
	struct i2s_afe_private *priv = afe->platform_priv;
	struct regmap *regmap = afe->regmap;
	int i = 0;

	dev_dbg(afe->dev, "%s suspend %d %d >>\n", __func__,
		pm_runtime_status_suspended(dev), afe->suspended);

	if (pm_runtime_status_suspended(dev) || !afe->suspended)
		return 0;
	
	i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x1);

	afe->suspended = false;

	dev_dbg(afe->dev, "%s <<\n", __func__);

	return 0;
}

static int i2s_afe_dai_suspend(struct snd_soc_dai *dai)
{
	struct mtk_base_afe *afe = dev_get_drvdata(dai->dev);

	dev_dbg(afe->dev, "%s '%s' >>\n", __func__, dai->name);

	i2s_afe_suspend(afe->dev, afe);

	dev_dbg(afe->dev, "%s '%s' <<\n", __func__, dai->name);

	return 0;
}

static int i2s_afe_dai_resume(struct snd_soc_dai *dai)
{
	struct mtk_base_afe *afe = dev_get_drvdata(dai->dev);

	dev_dbg(afe->dev, "%s '%s' >>\n", __func__, dai->name);

	i2s_afe_resume(afe->dev, afe);

	dev_dbg(afe->dev, "%s '%s' <<\n", __func__, dai->name);

	return 0;
}



static int i2s_alloc_dmabuf(struct snd_pcm_substream *substream,
			       struct snd_pcm_hw_params *params,
			       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct i2s_afe_private *afe_priv = afe->platform_priv;
	const int dai_id = rtd->cpu_dai->id;
	struct mtk_base_afe_memif *memif = &afe->memif[dai_id];
	const struct mtk_base_memif_data *data = memif->data;
	struct i2s_fe_dai_data *fe_data = &afe_priv->fe_data[dai_id];
	const size_t request_size = params_buffer_bytes(params);
	int ret;

	if (request_size > fe_data->sram_size) {
		ret = snd_pcm_lib_malloc_pages(substream, request_size);
		if (ret < 0) {
			dev_err(afe->dev,
				"%s %s malloc pages %zu bytes failed %d\n",
				__func__, data->name, request_size, ret);
			return ret;
		}

		fe_data->use_sram = false;
	} else {
		struct snd_dma_buffer *dma_buf = &substream->dma_buffer;

		dma_buf->dev.type = SNDRV_DMA_TYPE_DEV;
		dma_buf->dev.dev = substream->pcm->card->dev;
		dma_buf->area = (unsigned char *)fe_data->sram_vir_addr;
		dma_buf->addr = fe_data->sram_phy_addr;
		dma_buf->bytes = request_size;
		snd_pcm_set_runtime_buffer(substream, dma_buf);

		fe_data->use_sram = true;
	}

	return 0;
}


static int i2s_free_dmabuf(struct snd_pcm_substream *substream,
			      struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct i2s_afe_private *afe_priv = afe->platform_priv;
	const int dai_id = rtd->cpu_dai->id;
	struct i2s_fe_dai_data *fe_data = &afe_priv->fe_data[dai_id];
	int ret = 0;

	if (fe_data->use_sram) {
		snd_pcm_set_runtime_buffer(substream, NULL);
	} else {
		ret = snd_pcm_lib_free_pages(substream);
	}

	return ret;
}


/* For Hardware initialization */
dma_addr_t TxMem_PhyAddr = NULL, RxMem_PhyAddr = NULL;
unsigned char *TxMem_VirAddr=NULL, *RxMem_VirAddr=NULL;
uint32_t num_channel, num_partition;
uint32_t size_partition;
//#define txrx_buffer_size   (512*1024)	//512 KBytes
uint32_t txrx_buffer_size = 0;
struct device* i2s_dev; 


void i2s_reset(int type)
{
	if(type == I2S_PLAY){
		i2s_io_write(ETDM_OUT1_CON0, 1, 4, 1);
		i2s_io_write(ETDM_OUT1_CON0, 1, 4, 0);
	}else if(type == I2S_RECORD){
		i2s_io_write(ETDM_IN1_CON0, 1, 4, 1);
		i2s_io_write(ETDM_IN1_CON0, 1, 4, 0);
	}
}


int get_sample_rate_index(u_int rate){
	int i = 0;
	switch (rate){
		case 8000:
			i = 0;
			break;
		case 12000: 
			i = 1;		
			break;
		case 16000:
			i = 2;
			break;
		case 24000: 
			i = 3;		
			break;
		case 32000:
			i = 4;
			break;
		case 48000: 
			i = 5;		
			break;
		case 96000:
			i = 6;
			break;
		case 192000: 
			i = 7;		
			break;
		case 384000:
			i = 8;
			break;
		case 7350: 
			i = 16;		
			break;
		case 11025:
			i = 17;
			break;
		case 14700: 
			i = 18;		
			break;
		case 22050:
			i = 19;
			break;
		case 29400: 
			i = 20;		
			break;
		case 44100:
			i = 21;
			break;
		case 88200: 
			i = 22;		
			break;
		case 176400:
			i = 23;
			break;
		case 352800: 
			i = 24; 	
			break;
		default:
			i = 0; 
			break;
	}
	return i;
}

#if defined(ALSA_I2S_RECORD_TEST) || defined(ALSA_I2S_APLAY_TEST)
struct file *tmp_play_file;
#endif

int pcm_datafile_write(struct file * pfile, unsigned char *pdata, unsigned int datalen)
{
    mm_segment_t old_fs;
    if(pfile == NULL){
        return -1;
    } 
    
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    if(pfile->f_mode & FMODE_CAN_WRITE){
        __vfs_write(pfile, pdata, datalen, &pfile->f_pos);
    }
    set_fs(old_fs);
    
    return 0;
}

static int i2s_power_save(I2s_Power_e type)
{
	if(type == I2S_POWER_ON){
		SET_PCM_NP_SCU(0x9c,((GET_PCM_NP_SCU(0x9c) & (~(1<<2))) | 
		 (0 << 2)));
		SET_PCM_CHIP_SCU(0x1e4,((GET_PCM_CHIP_SCU(0x1e4) & (~(1<<27))) | 
		 (1 << 27)));
		SET_PCM_CHIP_SCU(0x1ec,((GET_PCM_CHIP_SCU(0x1ec) & (~(1<<13))) | 
		 (1 << 13)));
	}else if(type == I2S_POWER_OFF){
		SET_PCM_NP_SCU(0x9c,((GET_PCM_NP_SCU(0x9c) & (~(1<<2))) | 
		 (1 << 2)));
		SET_PCM_CHIP_SCU(0x1e4,((GET_PCM_CHIP_SCU(0x1e4) & (~(1<<27))) | 
		 (0 << 27)));
		SET_PCM_CHIP_SCU(0x1ec,((GET_PCM_CHIP_SCU(0x1ec) & (~(1<<13))) | 
		 (0 << 13)));
	}

	return 0;

}


static int 
I2s_HwInit(int type, u_int rate, u_int channels, u_int bits, u_int period_size, u_int buffer_size)
{
		int i, k;
		uint32_t currValue=0, newValue=0;
		unsigned int backupMemory = 0x40;
		txrx_buffer_size = buffer_size*(bits/8)*channels;


#if defined(ALSA_I2S_RECORD_TEST) || defined(ALSA_I2S_APLAY_TEST)
	
		tmp_play_file = filp_open("/tmp/play", O_CREAT | O_WRONLY, 0644);
		if (IS_ERR(tmp_play_file)){
			printk("DEBUG[%s:%d] open fail!\n\r", __FUNCTION__, __LINE__);//yafei0929
		}

		
		/* 1-1) Allocate uncached memory space for tx & rx */
		if(TxMem_VirAddr == NULL)
			TxMem_VirAddr = (unsigned char*) dma_alloc_coherent(i2s_dev, txrx_buffer_size + backupMemory, 
																&TxMem_PhyAddr, GFP_KERNEL);	
		if(RxMem_VirAddr == NULL)
			RxMem_VirAddr = (unsigned char*) dma_alloc_coherent(i2s_dev, txrx_buffer_size + backupMemory, 
																&RxMem_PhyAddr, GFP_KERNEL);
	
		if((TxMem_PhyAddr & 0x3f) != 0x0){
			TxMem_PhyAddr = (TxMem_PhyAddr & 0xffffffc0) +	backupMemory;
			TxMem_VirAddr = (unsigned char*)(((unsigned long)(TxMem_VirAddr) & 0xffffffc0) +  backupMemory);
		}
	
		if((RxMem_PhyAddr & 0x3f) != 0x0){
			RxMem_PhyAddr = (RxMem_PhyAddr & 0xffffffc0) +	backupMemory;
			RxMem_VirAddr = (unsigned char*)(((unsigned long)(RxMem_VirAddr) & 0xffffffc0) +  backupMemory);
		}

	
		/* 1-2) Write physical address of uncached memory into AFE base register(DL/UL) */
		write_i2s_reg(TxMem_PhyAddr, AFE_DL1_BASE);  //Tx	
		write_i2s_reg(TxMem_PhyAddr+txrx_buffer_size-1, AFE_DL1_END);
		write_i2s_reg(RxMem_PhyAddr, AFE_UL1_BASE);  //Rx	
		write_i2s_reg(RxMem_PhyAddr+txrx_buffer_size-1, AFE_UL1_END);
	
		
		/* 2. Configure I2S module into loopback mode in which the data path of ETDM IN1 is loopbacked from ETDM OUT1
			 2-1) Select slave mode source for etdm in1 (from etdmout1 master)
				  ETDM_COWORK_CON0[27:24] = 0x8
			 2-2) Select sdata0 source for etdm in1 (from etdmout1)
				  ETDM_COWORK_CON1[3:0] = 0x8
		*/
		i2s_io_write(ETDM_COWORK_CON0, 15, 24, 8);
		i2s_io_write(ETDM_COWORK_CON1, 15, 0, 8); 
		
	
		/* 3. Configure ETDM based on IN1 & OUT1's requirement
			 3-1) Select the work mode for etdm in1 (0:master  1:slave mode)
				  ETDM_IN1_CON0[5] = 0x1
			 3-2) Configure the transmission standard of ETDM IN1 & OUT1 
				  (0:I2S 1:LJ 2:RJ 3:EIAJ 4:DSPA/.../PCMA(2CH) 5:DSPB/TDM/PCMB(2CH))
					<1> General setting  
						ETDM_IN1_CON0[8:6] = 0x0
						ETDM_OUT1_CON0[8:6] = 0x0
					<2> Different setting for Test Item6
						ETDM_IN1_CON0[8:6] = ETDM_IP_format[idx]
						ETDM_OUT1_CON0[8:6] = (ETDM_IP_format[idx]
			 3-3) Configure valid data bit number in each channel (31:32bits  30:31bits  ~~ 6:7bits)
				  ETDM_OUT1_CON0[15:11] = 0x1f
				  ETDM_IN1_CON0[15:11] = 0x1f
			 3-4) Configure BCLK cycle number in each channel 
				  (BCLK = serial clock bit number, which must be larger than data bit number)
				  ETDM_OUT1_CON0[20:16] = 0x1f
				  ETDM_IN1_CON0[20:16] = 0x1f
		*/	
		
	
		/* 3-1 */
		i2s_io_write(ETDM_IN1_CON0, 1, 5, 1); 
		
		/* 3-2 */
		i2s_io_write(ETDM_IN1_CON0, 7, 6, 0); 	
		i2s_io_write(ETDM_OUT1_CON0, 7, 6, 0);	

		
		/* 3-3 */
		i2s_io_write(ETDM_OUT1_CON0, 31, 11, (bits-1));  //3-3 Tx
		i2s_io_write(ETDM_IN1_CON0, 31, 11, (bits-1));   //3-3 Rx 
	
		/* 3-4 */
		i2s_io_write(ETDM_IN1_CON0, 0x1f, 16, 0x1f);	
		i2s_io_write(ETDM_OUT1_CON0, 0x1f, 16, 0x1f); 	
 		
		/*
			3-5) Configure channel number of ETDM to 2(left & right)
				 ETDM_OUT1_CON0[26:23] = 1 (default)
				 ETDM_IN1_CON0[26:23] = 1 (default)
			3-6) Configure downlink agent channel number to 2 (left & right)
				 AFE_DL1_CON0[4:0] = 2	
				  
			Add extra setting for Test Item 5 
			3-a) Set up ETDM OUT to manual mode (default: auto mode)
				 ETDM_OUT1_CON1[29] =  1(auto) / 0(manual)
				 ETDM_IN1_CON1[29] = 1(auto) / 0(manual)	   
			3-b) For manual mode, set up reg_lrck_width by W 
				 ETDM_OUT1_CON1[28:20] = 31, 63, 95, 127, 159, 191, 223
				 ETDM_IN1_CON1[28:20] = 31, 63, 95, 127, 159, 191, 223			
			3-5) Configure channel number of ETDM to 4(W=32, 64, 96) & 8(W=32, 64, 96, 128, 160, 192, 224)
				 ETDM_OUT1_CON0[26:23] = 3 & 7
				 ETDM_IN1_CON0[26:23] = 3 & 7
			3-6) Configure downlink agent channel number to 4 & 8	
				 AFE_DL1_CON0[4:0] = 4 & 8
			
			3-7) System generate master BCLK
				 ETDM_OUT1_CON1[30] = 0x1

			3-8) Configure afe memif agent data format control, 0:16bits 1:32bits(default)
				 AFE_DL1_CON0[5] = 0,1    
				 AFE_UL1_CON0[5] = 0,1
		*/
		/* 3-a */
		i2s_io_write(ETDM_OUT1_CON1, 0x1, 29, 1); 	
		i2s_io_write(ETDM_IN1_CON1, 0x1, 29, 1);

		/* 3-b (Not necessary in auto mode) */
		
		/* 3-5 */	
		i2s_io_write(ETDM_IN1_CON0, 0xf, 23, (channels-1));		
		i2s_io_write(ETDM_OUT1_CON0, 0xf, 23, (channels-1));

		/* 3-6 */
		i2s_io_write(AFE_DL1_CON0, 0x1f, 0, channels);
	
		num_channel = (read_i2s_reg(AFE_DL1_CON0) & 0x1f);
		//printk("\n Channel number is %d \n", (ioread32(AFE_DL1_CON0)&0x1f));	
	
		/* 3-7 */	
		i2s_io_write(ETDM_OUT1_CON1, 0x1, 30, 0x1);

		/* 3-8 */
		if(bits == 16){
			i2s_io_write(AFE_DL1_CON0, 0x1, 5, 0);
			i2s_io_write(AFE_UL1_CON0, 0x1, 5, 0);
		}
	
		/*
		  4. Set up Interrupt request in register AFE_IRQ_CON0
			4-1) Pick out I2S-In(RX) as the interrupt requester
				 AFE_IRQ_CON0[4] = 0 (0:i2sIn  1:i2sOut)
				 AFE_IRQ1_CON0[4] = 0 (0:i2sIn	1:i2sOut)
			4-2) Register interrupt service routine
				 (refer to I2S_Interrupts_Init)
			4-3) Set up initial value(256KB, 1 frame = 8 bytes) for countdown that is used to 
				 determine when to trigger interrupt service routine
				 If there is 2 channels within a frame, then the transmission length of 1 frame = 2 * 4 bytes.
				 That is to say, 0x8000 frames = 256 KBytes 
				 AFE_IRQ_CNT[31:0] = 0x8000 (default = 0)
				 AFE_IRQ1_CNT[31:0] = 0x8000 (default = 0)
			4-4) Check if the initial value is set up correctly (Not completely necessary)
			4-5) Enable IRQ (0: disable  1: eanble)
				 AFE_IRQ_CON0[0] = 1
				 AFE_IRQ1_CON0[0] = 1
		*/			

#ifdef ALSA_I2S_RECORD_TEST
		/* 4-1 */
		i2s_io_write(AFE_IRQ_CON0, 0x1, 4, 0x0);//rx
		/* 4-3 */
		/* Add this for Test Item 6, since 8 channels cause address space access overflows valid address */
		write_i2s_reg(period_size , AFE_IRQ_CNT);
		/* 4.5 */	
		i2s_io_write(AFE_IRQ_CON0, 0x1, 0, 0x1);

		size_partition = (read_i2s_reg(AFE_IRQ_CNT)*((bits/8)*num_channel));		//unit: byte
		num_partition = txrx_buffer_size/size_partition;

		/* 5-1
			ETDM_IN1_CON3[30:26] = 0 (0:8K 1:12K 2:16K 3:24K 4:32K 5:48K 6:96K 7:192K 8:384K 16:7.35K 17:11.025K 18:14.7K 19:22.05K 20:29.4K 21:44.1K 22:88.2K 23:176.4K 24:352.8K)
			ETDM_OUT1_CON4[4:0] = 0 (0:8K 1:12K 2:16K 3:24K 4:32K 5:48K 6:96K 7:192K 8:384K 16:7.35K 17:11.025K 18:14.7K 19:22.05K 20:29.4K 21:44.1K 22:88.2K 23:176.4K 24:352.8K)
		*/
		/* 5-1 */
		i2s_io_write(ETDM_IN1_CON3, 0x1f, 26, get_sample_rate_index(rate));

		/* Fill in data into Tx memory buffer */
		uint8_t random_number;
		memset(TxMem_VirAddr, 0, txrx_buffer_size); 	
		for(i=0; i<txrx_buffer_size; i++) {
			//get_random_bytes(&random_number, sizeof(random_number));
			TxMem_VirAddr[i] = i;		
		}
		memset(RxMem_VirAddr, 0xff, txrx_buffer_size);

#else
		/* 4-1 */
		i2s_io_write(AFE_IRQ1_CON0, 0x1, 4, 0x1);//tx
		/* 4-3 */
		/* Add this for Test Item 6, since 8 channels cause address space access overflows valid address */
		write_i2s_reg(period_size , AFE_IRQ1_CNT);
		/* 4.5 */	
		i2s_io_write(AFE_IRQ1_CON0, 0x1, 0, 0x1);

		size_partition = (read_i2s_reg(AFE_IRQ1_CNT)*((bits/8)*num_channel));		//unit: byte
		num_partition = txrx_buffer_size/size_partition;
		
		/* 5-1
			ETDM_IN1_CON3[30:26] = 0 (0:8K 1:12K 2:16K 3:24K 4:32K 5:48K 6:96K 7:192K 8:384K 16:7.35K 17:11.025K 18:14.7K 19:22.05K 20:29.4K 21:44.1K 22:88.2K 23:176.4K 24:352.8K)
			ETDM_OUT1_CON4[4:0] = 0 (0:8K 1:12K 2:16K 3:24K 4:32K 5:48K 6:96K 7:192K 8:384K 16:7.35K 17:11.025K 18:14.7K 19:22.05K 20:29.4K 21:44.1K 22:88.2K 23:176.4K 24:352.8K)
		*/
		/* 5-1 */
		i2s_io_write(ETDM_OUT1_CON4, 0x1f, 0, get_sample_rate_index(rate));
		
		/* Fill in data into Tx memory buffer */
		memset(TxMem_VirAddr, 0xff, txrx_buffer_size); 
		memset(RxMem_VirAddr, 0xff, txrx_buffer_size);
#endif

#else
	if ((i2s_dev=get_i2s_dev()) == NULL) { 	
		printk("\n get_i2s_dev failed !\n"); 	
		return -ENOMEM; 
	}	

	if(type == I2S_PLAY){
		/* 1-1) Allocate uncached memory space for tx & rx */
		if(TxMem_VirAddr == NULL){
			TxMem_VirAddr = (unsigned char*) dma_alloc_coherent(i2s_dev, txrx_buffer_size + backupMemory, 
		                                                        &TxMem_PhyAddr, GFP_KERNEL);
		}
		if((TxMem_PhyAddr & 0x3f) != 0x0){
			TxMem_PhyAddr = (TxMem_PhyAddr & 0xffffffc0) +	backupMemory;
			TxMem_VirAddr = (unsigned char*)(((unsigned long)(TxMem_VirAddr) & 0xffffffc0) +  backupMemory);
		}
		/* 1-2) Write physical address of uncached memory into AFE base register(DL/UL) */
		write_i2s_reg(TxMem_PhyAddr, AFE_DL1_BASE);  //Tx	
		write_i2s_reg(TxMem_PhyAddr+txrx_buffer_size-1, AFE_DL1_END);


		/* 3-1 Select the work mode for etdm  (0:master  1:slave mode)*/
		i2s_io_write(ETDM_OUT1_CON0, 1, 5, 1);	
		/* 3-2 Configure the transmission standard of ETDM 0:I2S*/
		i2s_io_write(ETDM_OUT1_CON0, 7, 6, 0);
		/* 3-3 Configure valid data bit number in each channel 31:32bits */
		i2s_io_write(ETDM_OUT1_CON0, 31, 11, (bits-1));
		/* 3-4 Configure BCLK cycle number in each channel 31:32bits */
		i2s_io_write(ETDM_OUT1_CON0, 0x1f, 16, 0x1f);	
		/* 3-5 Configure channel number of ETDM to 2(left & right) */
		i2s_io_write(ETDM_OUT1_CON0, 0xf, 23, (channels-1));	
        /* 3-6 Configure downlink agent channel number to 2 (left & right)*/
		i2s_io_write(AFE_DL1_CON0, 0x1f, 0, channels);
		/* 3-a Set up ETDM OUT to manual mode 1(auto) / 0(manual)*/
		i2s_io_write(ETDM_OUT1_CON1, 0x1, 29, 1);	
		num_channel = (read_i2s_reg(AFE_DL1_CON0) & 0x1f);
	    /* 3-7 */	
		i2s_io_write(ETDM_OUT1_CON1, 0x1, 30, 0x1);
		/* 3-8 */
		if(bits == 16){
			i2s_io_write(AFE_DL1_CON0, 0x1, 5, 0);
		}

		/*4-1) Pick out I2S-Out(TX) as the interrupt requester*/
		i2s_io_write(AFE_IRQ1_CON0, 0x1, 4, 0x1);
		/*4-3) Set up initial value(256KB, 1 frame = 8 bytes) for countdown that is used to 
			  determine when to trigger interrupt service routine
			  If there is 2 channels within a frame, then the transmission length of 1 frame = 2 * 4 bytes.
			  That is to say, 0x8000 frames = 256 KBytes */
		write_i2s_reg(period_size , AFE_IRQ1_CNT);
		/* 4.5 Enable IRQ (0: disable  1: eanble)*/	
        i2s_io_write(AFE_IRQ1_CON0, 0x1, 0, 0x1);

		size_partition = (read_i2s_reg(AFE_IRQ1_CNT)*((bits/8)*num_channel));		//unit: byte
		num_partition = txrx_buffer_size/size_partition;	
		memset(TxMem_VirAddr, 0, txrx_buffer_size); 



	}else if(type == I2S_RECORD){
		/* 1-1) Allocate uncached memory space for tx & rx */
		if(RxMem_VirAddr == NULL){
			RxMem_VirAddr = (unsigned char*) dma_alloc_coherent(i2s_dev, txrx_buffer_size + backupMemory, 
	                                                            &RxMem_PhyAddr, GFP_KERNEL);
		}
		if((RxMem_PhyAddr & 0x3f) != 0x0){
			RxMem_PhyAddr = (RxMem_PhyAddr & 0xffffffc0) +	backupMemory;
			RxMem_VirAddr = (unsigned char*)(((unsigned long)(RxMem_VirAddr) & 0xffffffc0) +  backupMemory);
		}
		/* 1-2) Write physical address of uncached memory into AFE base register(DL/UL) */
		write_i2s_reg(RxMem_PhyAddr, AFE_UL1_BASE);  //Rx	
		write_i2s_reg(RxMem_PhyAddr+txrx_buffer_size-1, AFE_UL1_END);


		/* 3-1 Select the work mode for etdm  (0:master  1:slave mode)*/
		i2s_io_write(ETDM_IN1_CON0, 1, 5, 1); 
		/* 3-2 Configure the transmission standard of ETDM 0:I2S*/
		i2s_io_write(ETDM_IN1_CON0, 7, 6, 0);
		/* 3-3 Configure valid data bit number in each channel 31:32bits */
		i2s_io_write(ETDM_IN1_CON0, 31, 11, (bits-1));
		/* 3-4 Configure BCLK cycle number in each channel 31:32bits */
		i2s_io_write(ETDM_IN1_CON0, 0x1f, 16, 0x1f);	
		/* 3-5 Configure channel number of ETDM to 2(left & right) */
		i2s_io_write(ETDM_IN1_CON0, 0xf, 23, (channels-1));
		/* 3-8 */
		if(bits == 16){
			i2s_io_write(AFE_UL1_CON0, 0x1, 5, 0);
		}
		/* 3-a Set up ETDM IN to manual mode 1(auto) / 0(manual)*/
		i2s_io_write(ETDM_IN1_CON1, 0x1, 29, 1);	
		num_channel = ((read_i2s_reg(ETDM_IN1_CON0) & 0x07800000)>23) + 1;
		/*4-1) Pick out I2S-In(RX) as the interrupt requester*/
		i2s_io_write(AFE_IRQ_CON0, 0x1, 4, 0x0);
		/*4-3) Set up initial value(256KB, 1 frame = 8 bytes) for countdown that is used to 
			  determine when to trigger interrupt service routine
			  If there is 2 channels within a frame, then the transmission length of 1 frame = 2 * 4 bytes.
			  That is to say, 0x8000 frames = 256 KBytes */
		write_i2s_reg(period_size , AFE_IRQ_CNT);
		/* 4.5 Enable IRQ (0: disable  1: eanble)*/	
        i2s_io_write(AFE_IRQ_CON0, 0x1, 0, 0x1);
		
		size_partition = (read_i2s_reg(AFE_IRQ1_CNT)*((bits/8)*num_channel));		//unit: byte
		num_partition = txrx_buffer_size/size_partition;	
		memset(RxMem_VirAddr, 0xff, txrx_buffer_size);
	}
#endif
}

static int 
I2s_HwEnable(int type)
{
	i2s_power_save(I2S_POWER_ON);

#if defined(ALSA_I2S_RECORD_TEST) || defined(ALSA_I2S_APLAY_TEST)
	/* 5-1 */
	i2s_io_write(AFE_DAC_CON0, 0x1, 17, 0x1);

    /* 5-2 */
    i2s_io_write(AFE_DAC_CON0, 0x1, 1, 0x1);
	
	/* 5-3 */
    i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x1);
	
    /* 6-1 */
    i2s_io_write(ETDM_OUT1_CON0, 0x1, 0, 0x1);
	
    /* 6-2 */
    i2s_io_write(ETDM_IN1_CON0, 0x1, 0, 0x1);	

#else
	if(type == I2S_PLAY){	
		/* 5-1 Enable downlink1(TX) memory path*/
		i2s_io_write(AFE_DAC_CON0, 0x1, 17, 0x1);
		/* 5-3 Enable the whole AFE module*/
    	i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x1);
		/* 6-1 Enable ETDM Out*/
		i2s_io_write(ETDM_OUT1_CON0, 0x1, 0, 0x1);	
	}else if(type == I2S_RECORD){
	    /* 5-2 Enable uplink1(RX) memory path*/
    	i2s_io_write(AFE_DAC_CON0, 0x1, 1, 0x1);
		/* 5-3 Enable the whole AFE module*/
    	i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x1);
		/* 6-2 Enable ETDM In*/
		i2s_io_write(ETDM_IN1_CON0, 0x1, 0, 0x1); 
	}
#endif
}	
	
/*  I2sLoopbackTest_HwDisable
 *  Disable I2sLoopbackTest module. 
 *  Procedure:
 *    Reverse the procedure executing inside I2sLoopbackTest_HwEnable,
 *    and configure the value from 0x1 to 0x0 for each step
 *
 *  @param  void
 *  @return  void
 */	
static int 
I2s_HwDisable(int type)
{
#if defined(ALSA_I2S_RECORD_TEST) || defined(ALSA_I2S_APLAY_TEST)
	/* 6-1 */
	i2s_io_write(ETDM_OUT1_CON0, 0x1, 0, 0x0);	

	/* 6-2 */	
	i2s_io_write(ETDM_IN1_CON0, 0x1, 0, 0x0); 	

	/* 5-1 */
	i2s_io_write(AFE_DAC_CON0, 0x1, 17, 0x0);

	/* 5-2 */
	i2s_io_write(AFE_DAC_CON0, 0x1, 1, 0x0);

	/* 5-3 */
	i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x0);
#else
	if(type == I2S_PLAY){	
		/* 6-1 Disable ETDM Out*/
		i2s_io_write(ETDM_OUT1_CON0, 0x1, 0, 0x0);	
		/* 5-1 Disable downlink1(TX) memory path*/
		i2s_io_write(AFE_DAC_CON0, 0x1, 17, 0x0);
		/* 5-3 Disable the whole AFE module*/
    	i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x0);

	}else if(type == I2S_RECORD){
		/* 6-2 Disable ETDM In*/
		printk("ETDM_IN1_CON0=%x\n",read_i2s_reg(ETDM_IN1_CON0));
		i2s_io_write(ETDM_IN1_CON0, 0x1, 0, 0x0); 
	    /* 5-2 Disable uplink1(RX) memory path*/
    	i2s_io_write(AFE_DAC_CON0, 0x1, 1, 0x0);
		/* 5-3 Disable the whole AFE module*/
    	i2s_io_write(AFE_DAC_CON0, 0x1, 0, 0x0);
	}
#endif
	i2s_power_save(I2S_POWER_OFF);
}	

static void i2s_afe_fe_dl_shutdown(struct snd_pcm_substream *substream,
				   struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	int id = dai->id;
	struct mtk_base_afe_memif *memif = &afe->memif[id];
	const struct mtk_base_memif_data *data = memif->data;

	I2s_HwDisable(I2S_PLAY);


}

static void i2s_afe_fe_ul_shutdown(struct snd_pcm_substream *substream,
				   struct snd_soc_dai *dai)
{
	I2s_HwDisable(I2S_RECORD);
}

static int i2s_afe_fe_hw_dl_params(struct snd_pcm_substream *substream,
				   struct snd_pcm_hw_params *params,
				   struct snd_soc_dai *dai)
{

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_pcm_runtime * const runtime = substream->runtime;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct i2s_afe_private *afe_priv = afe->platform_priv;
	const int dai_id = rtd->cpu_dai->id;
	struct mtk_base_afe_memif *memif = &afe->memif[dai_id];
	const struct mtk_base_memif_data *data = memif->data;
	struct i2s_control_data *ctrl_data = &afe_priv->ctrl_data;
	struct i2s_fe_dai_data *fe_data = &afe_priv->fe_data[dai_id];
	unsigned int rate = params_rate(params);
	unsigned int channels = params_channels(params);
	unsigned int bits = params_width(params);
	unsigned int period_size = params_period_size(params);
	unsigned int buffer_size = params_buffer_size(params);
	unsigned int aux_channels = 0;
	int ret, fs;

	I2s_HwInit(I2S_PLAY, rate, channels, bits, period_size, buffer_size);
	runtime->dma_area = TxMem_VirAddr;
	return 0;
}

static int i2s_afe_fe_hw_ul_params(struct snd_pcm_substream *substream,
				   struct snd_pcm_hw_params *params,
				   struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_pcm_runtime * const runtime = substream->runtime;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct i2s_afe_private *afe_priv = afe->platform_priv;
	const int dai_id = rtd->cpu_dai->id;
	struct mtk_base_afe_memif *memif = &afe->memif[dai_id];
	const struct mtk_base_memif_data *data = memif->data;
	struct i2s_control_data *ctrl_data = &afe_priv->ctrl_data;
	struct i2s_fe_dai_data *fe_data = &afe_priv->fe_data[dai_id];
	unsigned int rate = params_rate(params);
	unsigned int channels = params_channels(params);
	unsigned int bits = params_width(params);
	unsigned int period_size = params_period_size(params);
	unsigned int buffer_size = params_buffer_size(params);
	unsigned int aux_channels = 0;
	int ret, fs;

	I2s_HwInit(I2S_RECORD, rate, channels, bits, period_size, buffer_size);
	runtime->dma_area = RxMem_VirAddr;
	return 0;
}



int i2s_afe_fe_ul_trigger(struct snd_pcm_substream *substream, int cmd,
			  struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	const int id = dai->id;
	struct mtk_base_afe_memif *memif = &afe->memif[id];
	const struct mtk_base_memif_data *data = memif->data;

	I2s_HwEnable(I2S_RECORD);

#if 0
	printk("AFE_DAC_CON0:%x\n",read_i2s_reg(AFE_DAC_CON0));
	printk("AFE_DAC_CON1:%x\n",read_i2s_reg(AFE_DAC_CON1));
	printk("AFE_MEMIF_BURST_CFG:%x\n",read_i2s_reg(AFE_MEMIF_BURST_CFG));
	printk("AFE_MEMIF_BUF_MON1:%x\n",read_i2s_reg(AFE_MEMIF_BUF_MON1));
	printk("AFE_MEMIF_BUF_MON6:%x\n",read_i2s_reg(AFE_MEMIF_BUF_MON6));
	printk("ETDM_COWORK_CON0:%x\n",read_i2s_reg(ETDM_COWORK_CON0));
	printk("ETDM_COWORK_CON1:%x\n",read_i2s_reg(ETDM_COWORK_CON1));
	printk("ETDM_IN1_CON0:%x\n",read_i2s_reg(ETDM_IN1_CON0));
	printk("ETDM_IN1_CON1:%x\n",read_i2s_reg(ETDM_IN1_CON1));
	printk("ETDM_IN1_CON2:%x\n",read_i2s_reg(ETDM_IN1_CON2));
	printk("ETDM_IN1_CON3:%x\n",read_i2s_reg(ETDM_IN1_CON3));
	printk("ETDM_IN1_CON4:%x\n",read_i2s_reg(ETDM_IN1_CON4));
	printk("ETDM_IN1_CON5:%x\n",read_i2s_reg(ETDM_IN1_CON5));
	printk("ETDM_IN1_CON6:%x\n",read_i2s_reg(ETDM_IN1_CON6));
	printk("ETDM_IN1_MONITOR:%x\n",read_i2s_reg(ETDM_IN1_MONITOR));
	printk("ETDM_OUT1_CON0:%x\n",read_i2s_reg(ETDM_OUT1_CON0));
	printk("ETDM_OUT1_CON1:%x\n",read_i2s_reg(ETDM_OUT1_CON1));
	printk("ETDM_OUT1_CON2:%x\n",read_i2s_reg(ETDM_OUT1_CON2));
	printk("ETDM_OUT1_CON3:%x\n",read_i2s_reg(ETDM_OUT1_CON3));
	printk("ETDM_OUT1_CON4:%x\n",read_i2s_reg(ETDM_OUT1_CON4));
	printk("ETDM_OUT1_CON6:%x\n",read_i2s_reg(ETDM_OUT1_CON6));
	printk("ETDM_OUT1_CON7:%x\n",read_i2s_reg(ETDM_OUT1_CON7));
	printk("ETDM_OUT1_MONITOR:%x\n",read_i2s_reg(ETDM_OUT1_MONITOR));
	printk("AFE_DL1_CHK_SUM1:%x\n",read_i2s_reg(AFE_DL1_CHK_SUM1));
	printk("AFE_DL1_CHK_SUM2:%x\n",read_i2s_reg(AFE_DL1_CHK_SUM2));
	printk("AFE_DL1_BASE:%x\n",read_i2s_reg(AFE_DL1_BASE));
	printk("AFE_DL1_CUR:%x\n",read_i2s_reg(AFE_DL1_CUR));
	printk("AFE_DL1_END:%x\n",read_i2s_reg(AFE_DL1_END));
	printk("AFE_DL1_CON0:%x\n",read_i2s_reg(AFE_DL1_CON0));
	printk("AFE_UL1_CHK_SUM1:%x\n",read_i2s_reg(AFE_UL1_CHK_SUM1));
	printk("AFE_UL1_CHK_SUM2:%x\n",read_i2s_reg(AFE_UL1_CHK_SUM2));
	printk("AFE_UL1_BASE:%x\n",read_i2s_reg(AFE_UL1_BASE));
	printk("AFE_UL1_END:%x\n",read_i2s_reg(AFE_UL1_END));
	printk("AFE_UL1_CUR:%x\n",read_i2s_reg(AFE_UL1_CUR));
	printk("AFE_UL1_CON0:%x\n",read_i2s_reg(AFE_UL1_CON0));
	printk("AFE_SIDEBAND0:%x\n",read_i2s_reg(AFE_SIDEBAND0));
	printk("AFE_SIDEBAND1:%x\n",read_i2s_reg(AFE_SIDEBAND1));
	printk("AFE_IRQ_CON0:%x\n",read_i2s_reg(AFE_IRQ_CON0));
	printk("AFE_IRQ_CNT:%x\n",read_i2s_reg(AFE_IRQ_CNT));
	printk("AFE_IRQ_CNT_MON:%x\n",read_i2s_reg(AFE_IRQ_CNT_MON));
	printk("AFE_IRQ_MON0:%x\n",read_i2s_reg(AFE_IRQ_MON0));
	printk("AFE_IRQ_MON1:%x\n",read_i2s_reg(AFE_IRQ_MON1));
	printk("AFE_IRQ_STS:%x\n",read_i2s_reg(AFE_IRQ_STS));
#endif


	return 0;
}


int i2s_afe_fe_dl_trigger(struct snd_pcm_substream *substream, int cmd,
			  struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	const int id = dai->id;
	struct mtk_base_afe_memif *memif = &afe->memif[id];
	const struct mtk_base_memif_data *data = memif->data;

	I2s_HwEnable(I2S_PLAY);

#if 0
	printk("AFE_DAC_CON0:%x\n",read_i2s_reg(AFE_DAC_CON0));
	printk("AFE_DAC_CON1:%x\n",read_i2s_reg(AFE_DAC_CON1));
	printk("AFE_MEMIF_BURST_CFG:%x\n",read_i2s_reg(AFE_MEMIF_BURST_CFG));
	printk("AFE_MEMIF_BUF_MON1:%x\n",read_i2s_reg(AFE_MEMIF_BUF_MON1));
	printk("AFE_MEMIF_BUF_MON6:%x\n",read_i2s_reg(AFE_MEMIF_BUF_MON6));
	printk("ETDM_COWORK_CON0:%x\n",read_i2s_reg(ETDM_COWORK_CON0));
	printk("ETDM_COWORK_CON1:%x\n",read_i2s_reg(ETDM_COWORK_CON1));
	printk("ETDM_IN1_CON0:%x\n",read_i2s_reg(ETDM_IN1_CON0));
	printk("ETDM_IN1_CON1:%x\n",read_i2s_reg(ETDM_IN1_CON1));
	printk("ETDM_IN1_CON2:%x\n",read_i2s_reg(ETDM_IN1_CON2));
	printk("ETDM_IN1_CON3:%x\n",read_i2s_reg(ETDM_IN1_CON3));
	printk("ETDM_IN1_CON4:%x\n",read_i2s_reg(ETDM_IN1_CON4));
	printk("ETDM_IN1_CON5:%x\n",read_i2s_reg(ETDM_IN1_CON5));
	printk("ETDM_IN1_CON6:%x\n",read_i2s_reg(ETDM_IN1_CON6));
	printk("ETDM_IN1_MONITOR:%x\n",read_i2s_reg(ETDM_IN1_MONITOR));
	printk("ETDM_OUT1_CON0:%x\n",read_i2s_reg(ETDM_OUT1_CON0));
	printk("ETDM_OUT1_CON1:%x\n",read_i2s_reg(ETDM_OUT1_CON1));
	printk("ETDM_OUT1_CON2:%x\n",read_i2s_reg(ETDM_OUT1_CON2));
	printk("ETDM_OUT1_CON3:%x\n",read_i2s_reg(ETDM_OUT1_CON3));
	printk("ETDM_OUT1_CON4:%x\n",read_i2s_reg(ETDM_OUT1_CON4));
	printk("ETDM_OUT1_CON6:%x\n",read_i2s_reg(ETDM_OUT1_CON6));
	printk("ETDM_OUT1_CON7:%x\n",read_i2s_reg(ETDM_OUT1_CON7));
	printk("ETDM_OUT1_MONITOR:%x\n",read_i2s_reg(ETDM_OUT1_MONITOR));
	printk("AFE_DL1_CHK_SUM1:%x\n",read_i2s_reg(AFE_DL1_CHK_SUM1));
	printk("AFE_DL1_CHK_SUM2:%x\n",read_i2s_reg(AFE_DL1_CHK_SUM2));
	printk("AFE_DL1_BASE:%x\n",read_i2s_reg(AFE_DL1_BASE));
	printk("AFE_DL1_CUR:%x\n",read_i2s_reg(AFE_DL1_CUR));
	printk("AFE_DL1_END:%x\n",read_i2s_reg(AFE_DL1_END));
	printk("AFE_DL1_CON0:%x\n",read_i2s_reg(AFE_DL1_CON0));
	printk("AFE_UL1_CHK_SUM1:%x\n",read_i2s_reg(AFE_UL1_CHK_SUM1));
	printk("AFE_UL1_CHK_SUM2:%x\n",read_i2s_reg(AFE_UL1_CHK_SUM2));
	printk("AFE_UL1_BASE:%x\n",read_i2s_reg(AFE_UL1_BASE));
	printk("AFE_UL1_END:%x\n",read_i2s_reg(AFE_UL1_END));
	printk("AFE_UL1_CUR:%x\n",read_i2s_reg(AFE_UL1_CUR));
	printk("AFE_UL1_CON0:%x\n",read_i2s_reg(AFE_UL1_CON0));
	printk("AFE_SIDEBAND0:%x\n",read_i2s_reg(AFE_SIDEBAND0));
	printk("AFE_SIDEBAND1:%x\n",read_i2s_reg(AFE_SIDEBAND1));
	printk("AFE_IRQ_CON0:%x\n",read_i2s_reg(AFE_IRQ_CON0));
	printk("AFE_IRQ_CNT:%x\n",read_i2s_reg(AFE_IRQ_CNT));
	printk("AFE_IRQ_CNT_MON:%x\n",read_i2s_reg(AFE_IRQ_CNT_MON));
	printk("AFE_IRQ_MON0:%x\n",read_i2s_reg(AFE_IRQ_MON0));
	printk("AFE_IRQ_MON1:%x\n",read_i2s_reg(AFE_IRQ_MON1));
	printk("AFE_IRQ_STS:%x\n",read_i2s_reg(AFE_IRQ_STS));
#endif


	return 0;
}


int i2s_afe_fe_hw_free(struct snd_pcm_substream *substream,
		       struct snd_soc_dai *dai)
{
#if defined(ALSA_I2S_RECORD_TEST) || defined(ALSA_I2S_APLAY_TEST)
	if (!(IS_ERR(tmp_play_file))){
		filp_close(tmp_play_file, NULL);
	}
#endif

	struct mtk_base_afe *afe = snd_soc_dai_get_drvdata(dai);

	return 0;
}
EXPORT_SYMBOL_GPL(i2s_afe_fe_hw_free);

static const struct snd_pcm_hardware mtk_afe_hardware = {
	.info = (SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
		 SNDRV_PCM_INFO_MMAP_VALID),
	.buffer_bytes_max = 256 * 1024,
	.period_bytes_min = 512,
	.period_bytes_max = 128 * 1024,
	.periods_min = 2,
	.periods_max = 256,
	.fifo_size = 0,
};


static int i2s_afe_dais_startup(struct snd_pcm_substream *substream,
				struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct mtk_base_afe_memif *memif = &afe->memif[rtd->cpu_dai->id];
	int ret;

	memif->substream = substream;

	snd_soc_set_runtime_hwparams(substream, &mtk_afe_hardware);

	/*
	 * Capture cannot use ping-pong buffer since hw_ptr at IRQ may be
	 * smaller than period_size due to AFE's internal buffer.
	 * This easily leads to overrun when avail_min is period_size.
	 * One more period can hold the possible unread buffer.
	 */
	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		ret = snd_pcm_hw_constraint_minmax(runtime,
						   SNDRV_PCM_HW_PARAM_PERIODS,
						   3,
						   mtk_afe_hardware.periods_max);
		if (ret < 0) {
			dev_err(afe->dev, "hw_constraint_minmax failed\n");
			return ret;
		}
	}
	ret = snd_pcm_hw_constraint_integer(runtime,
					    SNDRV_PCM_HW_PARAM_PERIODS);
	if (ret < 0)
		dev_err(afe->dev, "snd_pcm_hw_constraint_integer failed\n");
	return ret;
}


/* dl DAIs */
static const struct snd_soc_dai_ops i2s_afe_fe_dai_dl_ops = {
	.startup	= i2s_afe_dais_startup,
	.shutdown	= i2s_afe_fe_dl_shutdown,
	.hw_params	= i2s_afe_fe_hw_dl_params,
	.hw_free	= i2s_afe_fe_hw_free,
	.trigger	= i2s_afe_fe_dl_trigger,
};

/* ul DAIs */
static const struct snd_soc_dai_ops i2s_afe_fe_dai_ul_ops = {
	.startup	= i2s_afe_dais_startup,
	.shutdown	= i2s_afe_fe_ul_shutdown,
	.hw_params	= i2s_afe_fe_hw_ul_params,
	.hw_free	= i2s_afe_fe_hw_free,
	.trigger	= i2s_afe_fe_ul_trigger,
};

static struct snd_soc_dai_driver i2s_afe_pcm_dais[] = {
	/* FE DAIs: memory intefaces to CPU */
	{
		.name = "DL1",
		.id = I2S_AFE_MEMIF_DL1,
		.suspend = i2s_afe_dai_suspend,
		.resume = i2s_afe_dai_resume,
		.playback = {
			.stream_name = "DL1",
			.channels_min = 1,
			.channels_max = 8,
			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &i2s_afe_fe_dai_dl_ops,
		.bus_control = true,
	}, {
		.name = "UL1",
		.id = I2S_AFE_MEMIF_UL1,
		.suspend = i2s_afe_dai_suspend,
		.resume = i2s_afe_dai_resume,
		.capture = {
			.stream_name = "UL1",
			.channels_min = 1,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &i2s_afe_fe_dai_ul_ops,
		.bus_control = true,
	},
};

static struct snd_soc_dai_driver *i2s_get_dai_drv(int id)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(i2s_afe_pcm_dais); i++) {
		if (i2s_afe_pcm_dais[i].id == id)
			return &i2s_afe_pcm_dais[i];
	}

	return NULL;
}


static const struct snd_soc_dapm_widget i2s_afe_pcm_widgets[] = {
	
};

static const struct snd_soc_dapm_route i2s_afe_pcm_routes[] = {
	
};

static const struct snd_soc_component_driver i2s_afe_pcm_dai_component = {
	.name = "i2s-afe-pcm-dai",
	.dapm_widgets = i2s_afe_pcm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(i2s_afe_pcm_widgets),
	.dapm_routes = i2s_afe_pcm_routes,
	.num_dapm_routes = ARRAY_SIZE(i2s_afe_pcm_routes),
};

static const struct mtk_base_memif_data memif_data[I2S_AFE_MEMIF_NUM] = {
	{
		.name = "DL1",
		.id = I2S_AFE_MEMIF_DL1,
		.reg_ofs_base = AFE_DL1_BASE,
		.reg_ofs_cur = AFE_DL1_CUR,
		.fs_reg = -1,
		.fs_shift = -1,
		.fs_maskbit = -1,
		.mono_reg = -1,
		.mono_shift = -1,
		.hd_reg = -1,
		.hd_shift = -1,
		.enable_reg = AFE_DAC_CON0,
		.enable_shift = 17,
		.msb_reg = -1,
		.msb_shift = -1,
		.msb2_reg = -1,
		.msb2_shift = -1,
		.agent_disable_reg = -1,
		.agent_disable_shift = -1,
	}, {
		.name = "UL1",
		.id = I2S_AFE_MEMIF_UL1,
		.reg_ofs_base = AFE_UL1_BASE,
		.reg_ofs_cur = AFE_UL1_CUR,
		.fs_reg = -1,
		.fs_shift = -1,
		.fs_maskbit = -1,
		.mono_reg = -1,
		.mono_shift = -1,
		.hd_reg = -1,
		.hd_shift = -1,
		.enable_reg = AFE_DAC_CON0,
		.enable_shift = 1,
		.msb_reg = -1,
		.msb_shift = -1,
		.msb2_reg = -1,
		.msb2_shift = -1,
		.agent_disable_reg = -1,
		.agent_disable_shift = -1,
	},
};

static const struct mtk_base_irq_data irq_data[I2S_AFE_IRQ_NUM] = {
	{
		.id = I2S_AFE_IRQ1,
		.irq_cnt_reg = -1,
		.irq_cnt_shift = -1,
		.irq_cnt_maskbit = -1,
		.irq_en_reg = AFE_IRQ1_CON0,
		.irq_en_shift = 4,
		.irq_fs_reg = -1,
		.irq_fs_shift = -1,
		.irq_fs_maskbit = -1,
		.irq_clr_reg = AFE_IRQ1_CON0,
		.irq_clr_shift = 0,
	}, {
		.id = I2S_AFE_IRQ2,
		.irq_cnt_reg = -1,
		.irq_cnt_shift = -1,
		.irq_cnt_maskbit = -1,
		.irq_en_reg = AFE_IRQ_CON0,
		.irq_en_shift = 4,
		.irq_fs_reg = -1,
		.irq_fs_shift = -1,
		.irq_fs_maskbit = -1,
		.irq_clr_reg = AFE_IRQ_CON0,
		.irq_clr_shift = 1,
	}, 	
};

static irqreturn_t i2s_afe_irq_handler(int irq_id, void *dev_id)
{
	int id;
	struct mtk_base_afe *afe = dev_id;
	struct mtk_base_afe_memif *memif;
	struct mtk_base_afe_irq *irq;
	u32 status;
	u32 curadr,baseadr;

	status = read_i2s_reg(AFE_IRQ_STS);

	if(status & IRQ_RECORD_MASK){
		/* Clear IRQ */	
		i2s_io_write(AFE_IRQ_CON0, 0x1, 2, 0x1);				
		i2s_io_write(AFE_IRQ_CON0, 0x1, 2, 0x0);		
		/* Clear IRQ miss flag */	
		i2s_io_write(AFE_IRQ_CON0, 0x1, 3, 0x1);				
		i2s_io_write(AFE_IRQ_CON0, 0x1, 3, 0x0);
	}else if(status & IRQ_PLAY_MASK){	
		/* Clear IRQ1 */	
		i2s_io_write(AFE_IRQ1_CON0, 0x1, 2, 0x1);				
		i2s_io_write(AFE_IRQ1_CON0, 0x1, 2, 0x0);		
		/* Clear IRQ1 miss flag */	
		i2s_io_write(AFE_IRQ1_CON0, 0x1, 3, 0x1);				
		i2s_io_write(AFE_IRQ1_CON0, 0x1, 3, 0x0);	
	}

	for (id = 0; id < I2S_AFE_MEMIF_NUM; ++id) {
		memif = &afe->memif[id];
		if (memif->irq_usage < 0){
			continue;
		}
		irq = &afe->irqs[memif->irq_usage];

		if (status & 1 << (irq->irq_data->irq_clr_shift)){
			snd_pcm_period_elapsed(memif->substream);
		}
	}

	

	return IRQ_HANDLED;
}

static int i2s_afe_runtime_suspend(struct device *dev)
{
	return 0;
}

static int i2s_afe_runtime_resume(struct device *dev)
{
	return 0;
}

static int i2s_afe_dev_runtime_suspend(struct device *dev)
{
	struct mtk_base_afe *afe = dev_get_drvdata(dev);

	dev_dbg(afe->dev, "%s >>\n", __func__);

	i2s_afe_suspend(afe->dev, afe);

	dev_dbg(afe->dev, "%s <<\n", __func__);

	return 0;
}

static int i2s_afe_dev_runtime_resume(struct device *dev)
{
	struct mtk_base_afe *afe = dev_get_drvdata(dev);

	dev_dbg(afe->dev, "%s >>\n", __func__);

	i2s_afe_resume(afe->dev, afe);

	dev_dbg(afe->dev, "%s <<\n", __func__);

	return 0;
}

static int i2s_afe_pcm_probe(struct snd_soc_platform *platform)
{
	int ret, i;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(platform);
	struct i2s_afe_private *priv = afe->platform_priv;
	struct i2s_afe_ext_clk_tune_data *clk_tune = &priv->clk_tune;
	bool need_clk_tune = false;
#ifdef CALI_RESULT_CHECK
	struct i2s_cali_result *cali_res = &priv->cali_res;
#endif

	return 0;
}

static int i2s_afe_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct i2s_afe_private *priv = afe->platform_priv;
	int id = rtd->cpu_dai->id;
	struct i2s_fe_dai_data *fe_data = &priv->fe_data[id];
	struct snd_pcm_substream *substream;
	int stream;
	int ret = 0;

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (substream) {
			struct snd_dma_buffer *buf = &substream->dma_buffer;

			buf->dev.type = SNDRV_DMA_TYPE_DEV;
			buf->dev.dev = card->dev;
			buf->private_data = NULL;
		}
	}

	if (fe_data->prealloc_size > 0) {
		unsigned int max = afe->mtk_afe_hardware->buffer_bytes_max;

		ret = snd_pcm_lib_preallocate_pages_for_all(pcm,
			SNDRV_DMA_TYPE_DEV, card->dev,
			min(fe_data->prealloc_size, max),
			max);
	}

	return ret;
}

static void i2s_afe_pcm_free(struct snd_pcm *pcm){
	snd_pcm_lib_preallocate_free_for_all(pcm);
}

static snd_pcm_uframes_t i2s_afe_pcm_pointer
	(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct mtk_base_afe_memif *memif = &afe->memif[rtd->cpu_dai->id];

	struct mtk_afe *tmp_afe = snd_soc_platform_get_drvdata(rtd->platform);
	struct mtk_afe_memif *tmp_memif = &tmp_afe->memif[rtd->cpu_dai->id];

	u32 curadr,baseadr;
	baseadr = read_i2s_reg(memif->data->reg_ofs_base);
	curadr = read_i2s_reg(memif->data->reg_ofs_cur);

	return bytes_to_frames(substream->runtime,(curadr - baseadr));
}

static int i2s_afe_pcm_copy(struct snd_pcm_substream *substream,
	int channel,
	snd_pcm_uframes_t pos,
	void __user *buf,
	snd_pcm_uframes_t count)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mtk_base_afe *afe = snd_soc_platform_get_drvdata(rtd->platform);
	int id = rtd->cpu_dai->id;
	struct mtk_base_afe_memif *memif = &afe->memif[id];
	int stream = substream->stream;
	ssize_t copy_bytes = frames_to_bytes(runtime, count);
	char *hwbuf = runtime->dma_area + frames_to_bytes(runtime, pos);	

	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {	
		if (copy_from_user_toio(hwbuf, buf, copy_bytes)){
			return -EFAULT;
		}
#ifdef ALSA_I2S_APLAY_TEST
		if (!(IS_ERR(tmp_play_file))){
			pcm_datafile_write(tmp_play_file, hwbuf, copy_bytes);
		}
#endif

	} else if (stream == SNDRV_PCM_STREAM_CAPTURE) {
		if (copy_to_user_fromio(buf, hwbuf, copy_bytes)){
			return -EFAULT;
		}
	}

	return 0;
}

const struct snd_pcm_ops i2s_afe_pcm_ops = {
	.ioctl = snd_pcm_lib_ioctl,
	.pointer = i2s_afe_pcm_pointer,
	.copy = i2s_afe_pcm_copy,
};

const struct snd_soc_platform_driver i2s_afe_pcm_platform = {
	.probe = i2s_afe_pcm_probe,
	.pcm_new = i2s_afe_pcm_new,
	.pcm_free = i2s_afe_pcm_free,
	.ops = &i2s_afe_pcm_ops,
};

static int i2s_afe_pcm_dev_probe(struct platform_device *pdev)
{
	int ret, i, sel_irq;
	struct mtk_base_afe *afe;
	struct i2s_afe_private *afe_priv;
	struct device *dev = &pdev->dev;
	unsigned int irq_id;

	afe = devm_kzalloc(dev, sizeof(*afe), GFP_KERNEL);
	if (!afe)
		return -ENOMEM;

	afe->platform_priv = devm_kzalloc(dev, sizeof(*afe_priv), GFP_KERNEL);
	afe_priv = afe->platform_priv;
	if (!afe_priv)
		return -ENOMEM;

	afe->dev = dev;

	irq_id = get_i2s_irq();	
	if (!irq_id) {
		return -ENXIO;
	}

	ret = devm_request_irq(afe->dev, irq_id, i2s_afe_irq_handler,
			       0, "Afe_ISR_Handle", (void *)afe);
	if (ret) {
		dev_err(afe->dev, "could not request_irq\n");
		return ret;
	}

	/* memif % irq initialize*/
	afe->memif_size = I2S_AFE_MEMIF_NUM;
	afe->memif = devm_kcalloc(afe->dev, afe->memif_size,
				  sizeof(*afe->memif), GFP_KERNEL);
	if (!afe->memif)
		return -ENOMEM;

	afe->irqs_size = I2S_AFE_IRQ_NUM;
	afe->irqs = devm_kcalloc(afe->dev, afe->irqs_size,
				 sizeof(*afe->irqs), GFP_KERNEL);
	if (!afe->irqs)
		return -ENOMEM;

	for (i = 0; i < afe->irqs_size; i++)
		afe->irqs[i].irq_data = &irq_data[i];

	for (i = 0; i < afe->memif_size; i++) {
		//afe->memif[i].afe = afe;
		afe->memif[i].data = &memif_data[i];
		sel_irq = memif_specified_irqs[i];
		if (sel_irq >= 0) {
			afe->memif[i].irq_usage = sel_irq;
			afe->memif[i].const_irq = 1;
			afe->irqs[sel_irq].irq_occupyed = true;
		} else {
			afe->memif[i].irq_usage = -1;
		}
	}

	if (afe_priv->dl8_enable_24ch_output) {
		static struct snd_soc_dai_driver *dai_drv;

		dai_drv = i2s_get_dai_drv(I2S_AFE_IO_ETDM1_OUT);
		if (dai_drv)
			dai_drv->playback.channels_max = 24;

		dai_drv = i2s_get_dai_drv(I2S_AFE_IO_ETDM2_OUT);
		if (dai_drv)
			dai_drv->playback.channels_max = 24;
	}

	afe->mtk_afe_hardware = &i2s_afe_hardware;
	afe->alloc_dmabuf = i2s_alloc_dmabuf;
	afe->free_dmabuf = i2s_free_dmabuf;

	platform_set_drvdata(pdev, afe);

	pm_runtime_enable(dev);
	if (!pm_runtime_enabled(dev)) {
		dev_warn(afe->dev, "%s pm_runtime not enabled\n", __func__);
		ret = i2s_afe_runtime_resume(dev);
		if (ret)
			goto err_pm_disable;
	}

	pm_runtime_get_sync(dev);

	afe->reg_back_up_list = i2s_afe_backup_list;
	afe->reg_back_up_list_num = ARRAY_SIZE(i2s_afe_backup_list);
	afe->runtime_resume = i2s_afe_runtime_resume;
	afe->runtime_suspend = i2s_afe_runtime_suspend;

	ret = snd_soc_register_platform(dev, &i2s_afe_pcm_platform);
	if (ret)
		goto err_platform;

	ret = snd_soc_register_component(dev,
					 &i2s_afe_pcm_dai_component,
					 i2s_afe_pcm_dais,
					 ARRAY_SIZE(i2s_afe_pcm_dais));
	if (ret)
		goto err_component;


	dev_info(dev, "I2S AFE driver initialized.\n");
	i2s_power_save(I2S_POWER_OFF);
	return 0;

err_component:
	snd_soc_unregister_platform(dev);
err_platform:
	pm_runtime_put_sync(dev);
err_pm_disable:
	pm_runtime_disable(dev);
	return ret;
}

static int i2s_afe_pcm_dev_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	if (!pm_runtime_status_suspended(dev))
		i2s_afe_runtime_suspend(dev);

	pm_runtime_put_sync(dev);
	pm_runtime_disable(dev);
	snd_soc_unregister_component(dev);
	snd_soc_unregister_platform(dev);
	return 0;
}

static const struct of_device_id i2s_afe_pcm_dt_match[] = {
	{ .compatible = "i2s,i2s-afe-pcm", },
	{ }
};
MODULE_DEVICE_TABLE(of, i2s_afe_pcm_dt_match);

static const struct dev_pm_ops i2s_afe_pm_ops = {
	SET_RUNTIME_PM_OPS(i2s_afe_dev_runtime_suspend,
			   i2s_afe_dev_runtime_resume, NULL)
};

static struct platform_driver i2s_afe_pcm_driver = {
	.driver = {
		   .name = "i2s-afe-pcm",
		   .of_match_table = i2s_afe_pcm_dt_match,
		   .pm = &i2s_afe_pm_ops,
	},
	.probe = i2s_afe_pcm_dev_probe,
	.remove = i2s_afe_pcm_dev_remove,
};

module_platform_driver(i2s_afe_pcm_driver);

MODULE_DESCRIPTION("Econet ALSA SoC AFE platform driver");
MODULE_AUTHOR("Econet");
MODULE_LICENSE("GPL v2");
