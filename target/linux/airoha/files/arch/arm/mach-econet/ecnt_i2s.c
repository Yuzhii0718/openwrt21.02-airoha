/*----------------------------------------------------------------------------*
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * $Author:  $
 * $Date: 2020/06/12 $
 * $RCSfile: ecnt_i2s.c $
 * $Revision: #0 $
 *
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------
#include <asm/io.h>
#include <linux/types.h>
#include <linux/of_irq.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include <modules/i2s_global.h>
#include "scu/ecnt_scu.h"

//#define I2S_LOOPBACK_TEST
//---------------------------------------------------------------------------
// Macro definitions
//---------------------------------------------------------------------------
#define I2S_ADDR_BASE1        (ecnt_i2s->base1)
#define I2S_ADDR_BASE2        (ecnt_i2s->base2)

/* I2S Register Base1 Group */
#define AFE_DAC_CON0				(I2S_ADDR_BASE1 + 0x000)
#define AFE_DAC_CON1				(I2S_ADDR_BASE1 + 0x004)
#define AFE_MEMIF_BURST_CFG 	 	(I2S_ADDR_BASE1 + 0x008)
#define AFE_MEMIF_BUF_MON1  		(I2S_ADDR_BASE1 + 0x01c)
#define AFE_MEMIF_BUF_MON6  		(I2S_ADDR_BASE1 + 0x034)
#define ETDM_COWORK_CON0  			(I2S_ADDR_BASE1 + 0x04c)
#define ETDM_COWORK_CON1  			(I2S_ADDR_BASE1 + 0x050)
#define ETDM_IN1_CON0  				(I2S_ADDR_BASE1 + 0x05c)
#define ETDM_IN1_CON1  				(I2S_ADDR_BASE1 + 0x060)
#define ETDM_IN1_CON2  				(I2S_ADDR_BASE1 + 0x064)
#define ETDM_IN1_CON3  				(I2S_ADDR_BASE1 + 0x068)
#define ETDM_IN1_CON4				(I2S_ADDR_BASE1 + 0x06c)
#define ETDM_IN1_CON5  				(I2S_ADDR_BASE1 + 0x070)
#define ETDM_IN1_CON6  				(I2S_ADDR_BASE1 + 0x074)
#define ETDM_IN1_MONITOR			(I2S_ADDR_BASE1 + 0x078)
#define ETDM_OUT1_CON0				(I2S_ADDR_BASE1 + 0x07c)
#define ETDM_OUT1_CON1				(I2S_ADDR_BASE1 + 0x080)
#define ETDM_OUT1_CON2				(I2S_ADDR_BASE1 + 0x084)
#define ETDM_OUT1_CON3				(I2S_ADDR_BASE1 + 0x088)
#define ETDM_OUT1_CON4				(I2S_ADDR_BASE1 + 0x08c)
#define ETDM_OUT1_CON6				(I2S_ADDR_BASE1 + 0x094)
#define ETDM_OUT1_CON7				(I2S_ADDR_BASE1 + 0x098)
#define ETDM_OUT1_MONITOR			(I2S_ADDR_BASE1 + 0x09c)
#define AFE_DL1_CHK_SUM1			(I2S_ADDR_BASE1 + 0x0a0)
#define AFE_DL1_CHK_SUM2			(I2S_ADDR_BASE1 + 0x0a4)
#define AFE_DL1_BASE				(I2S_ADDR_BASE1 + 0x0a8)
#define AFE_DL1_CUR					(I2S_ADDR_BASE1 + 0x0ac)
#define AFE_DL1_END					(I2S_ADDR_BASE1 + 0x0b0)
#define AFE_DL1_CON0				(I2S_ADDR_BASE1 + 0x0b4)
#define AFE_UL1_CHK_SUM1			(I2S_ADDR_BASE1 + 0x0bc)
#define AFE_UL1_CHK_SUM2			(I2S_ADDR_BASE1 + 0x0c0)
#define AFE_UL1_BASE				(I2S_ADDR_BASE1 + 0x0c4)
#define AFE_UL1_END					(I2S_ADDR_BASE1 + 0x0c8)
#define AFE_UL1_CUR					(I2S_ADDR_BASE1 + 0x0cc)
#define AFE_UL1_CON0				(I2S_ADDR_BASE1 + 0x0d0)
#define AFE_SIDEBAND0				(I2S_ADDR_BASE1 + 0x0dc)
#define AFE_SIDEBAND1				(I2S_ADDR_BASE1 + 0x0e0)
#define AFE_IRQ_CON0				(I2S_ADDR_BASE1 + 0x0e4)
#define AFE_IRQ_CNT					(I2S_ADDR_BASE1 + 0x0e8)
#define AFE_IRQ_CNT_MON				(I2S_ADDR_BASE1 + 0x0ec)
#define AFE_IRQ_MON0				(I2S_ADDR_BASE1 + 0x0f0)
#define AFE_IRQ_MON1				(I2S_ADDR_BASE1 + 0x0f4)
#define AFE_IRQ_STS					(I2S_ADDR_BASE1 + 0x0f8)

/* I2S Register Base2 Group */
#define AFE_IRQ1_CON0					(I2S_ADDR_BASE2 + 0x0100)
#define AFE_IRQ1_CNT					(I2S_ADDR_BASE2 + 0x0104)
#define AFE_IRQ1_CNT_MON				(I2S_ADDR_BASE2 + 0x0108)
#define AFE_IRQ1_MON0					(I2S_ADDR_BASE2 + 0x010c)
#define AFE_IRQ1_MON1					(I2S_ADDR_BASE2 + 0x0110)

#define ECNT_IOWRITE32(Reg, ONE_BITS_SUM, LEFT_SHIFT_NUM, NEW_BITS_SUM) \
         iowrite32(((ioread32(Reg) & (~(ONE_BITS_SUM<<LEFT_SHIFT_NUM))) | \
		 (NEW_BITS_SUM << LEFT_SHIFT_NUM)) , Reg)

//---------------------------------------------------------------------------
// Type definitions
//---------------------------------------------------------------------------
struct ecnt_i2s {
	struct device *dev;
	void __iomem *base1;
	void __iomem *base2;
	int irq;	
};


//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
struct ecnt_i2s *ecnt_i2s = NULL;

//---------------------------------------------------------------------------
// Static Variables
//---------------------------------------------------------------------------
static const struct of_device_id ecnt_i2s_of_id[] = {
    { .compatible = "econet,ecnt-i2s"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_i2s_of_id);

//---------------------------------------------------------------------------
// Function Declarations
//---------------------------------------------------------------------------
void (*ecnt_i2s_isr_func)(void);

//---------------------------------------------------------------------------
// Static Functions
//---------------------------------------------------------------------------

/* don't EXPORT this function. Create API for your purpose instead. */
static u32 get_i2s_data(void __iomem  *reg)
{
    return readl(reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_i2s_data(u32 val, void __iomem  *reg)
{
    writel(val, reg); 
}

//---------------------------------------------------------------------------
// Exported Functions
//---------------------------------------------------------------------------

/*  get_afe_irq_mon
 *  AFE_IRQ_MON0  : regiter for monitoring irq_b counter in rising edge
 *                  and in falling edge
 *  AFE_IRQ_MON1  : register for irq0 miss flag, which means miss handle irq0 flag
 *  AFE_IRQ1_MON0 : same as IRQ_MON0, but for IRQ1
 *  AFE_IRQ1_MON1 : same as IRQ_MON1, but for IRQ1
 *
 *  @param  uint8_t irq_reg_num: 0 means IRQ0  1 means IRQ1
 *          uint8_t mon_num:  0 means MON0  1 means MON1
 *  @return  void
 */
static u32 get_i2s_data_common(u32 reg)
{
	if(reg < 0x100)
		return readl(ecnt_i2s->base1 + reg);
	else
		return readl(ecnt_i2s->base2 + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_i2s_data_common(u32 val, u32 reg)
{
	if(reg < 0x100)
		writel(val, ecnt_i2s->base1 + reg); 
	else
		writel(val, ecnt_i2s->base2 + reg); 
}


uint32_t read_i2s_reg(u32 reg)
{
	return get_i2s_data_common(reg);	
}
EXPORT_SYMBOL(read_i2s_reg);

uint32_t write_i2s_reg(u32 val, u32 reg)
{
	set_i2s_data_common(val, reg);	
	return 0;
}
EXPORT_SYMBOL(write_i2s_reg);

uint32_t  i2s_io_write(u32 Reg, u32 one_bits_sum,u32 left_shift_num, u32 new_bits_sum) 
{
	set_i2s_data_common(((get_i2s_data_common(Reg) & (~(one_bits_sum<<left_shift_num))) | 
		 (new_bits_sum << left_shift_num)) , Reg);

	return 0;
}
EXPORT_SYMBOL(i2s_io_write);

uint32_t get_afe_irq_mon(uint8_t irq_reg_num, uint8_t mon_num)
{
    if(irq_reg_num == 0 && mon_num == 0)
        return get_i2s_data(AFE_IRQ_MON0);
    else if(irq_reg_num == 0 && mon_num == 1)
	    return get_i2s_data(AFE_IRQ_MON1);
    else if(irq_reg_num == 1 && mon_num == 0)
	    return get_i2s_data(AFE_IRQ1_MON0);
    else if(irq_reg_num == 1 && mon_num == 1)
	    return get_i2s_data(AFE_IRQ1_MON1);	
    else{
        printk("[error] wrong irq_reg_num and mon_num combination!\n");
    }
    return 0;	
}
EXPORT_SYMBOL(get_afe_irq_mon);
 
#define i2s_dram_phyAddr_to_dmaAddr(x) 				(((__u64)(x)&0x3fffffff)+0x80000000)  //dram_phyAddr_to_dmaAddr(x)

void i2s_write_afe_dl1_base(dma_addr_t afe_dl1_base_addr)
{
	set_i2s_data(i2s_dram_phyAddr_to_dmaAddr(afe_dl1_base_addr), AFE_DL1_BASE);
}
EXPORT_SYMBOL(i2s_write_afe_dl1_base);

void i2s_write_afe_dl_end(dma_addr_t afe_dl1_end_addr)
{
	set_i2s_data(i2s_dram_phyAddr_to_dmaAddr(afe_dl1_end_addr), AFE_DL1_END);	
}
EXPORT_SYMBOL(i2s_write_afe_dl_end);

void i2s_write_afe_ul1_base(dma_addr_t afe_ul1_base_addr)
{
	set_i2s_data(i2s_dram_phyAddr_to_dmaAddr(afe_ul1_base_addr), AFE_UL1_BASE);
}
EXPORT_SYMBOL(i2s_write_afe_ul1_base);

void i2s_write_afe_ul1_end(dma_addr_t afe_ul1_end_addr)
{
	set_i2s_data(i2s_dram_phyAddr_to_dmaAddr(afe_ul1_end_addr), AFE_UL1_END);
}
EXPORT_SYMBOL(i2s_write_afe_ul1_end);


/*  etdm_in1_slave_sel
 *  Select etdmin1 slave mode source
 *  Bitfield: ETDM_COWORK_CON0[27:24] = 0x8
 *    8: from etdmout1 master
 *    9: from etdmut1 slave 
 *  @param  void
 *  @return  void
 */
void etdm_in1_slave_sel(uint8_t etdmSrc)
{
    ECNT_IOWRITE32(ETDM_COWORK_CON0, 15, 24, 8);
}
EXPORT_SYMBOL(etdm_in1_slave_sel);

/*  etdm_in1_sdata0_sel
 *  Select etdemin1 sdata0 source
 *  Bitfield: ETDM_COWORK_CON1[3:0] = 0x8
 *     8: from etdmout1 
 *    10: from etdmout2
 *  @param  void
 *  @return  void
 */
void etdm_in1_sdata0_sel(uint8_t etdmSdataSrc)
{
    ECNT_IOWRITE32(ETDM_COWORK_CON1, 15, 0, 8);	
}
EXPORT_SYMBOL(etdm_in1_sdata0_sel);

/*  set_irq_count
 *  After interrupt is enabled, irq_count will be decreased to 0,
 *  and then interrupt handler will be triggered
 *  Bitfield: AFE_IRQ_CNT[31:0] = 0x8000 frames
 *     If there is 2 channels within a frame, then the transmission length of 
 *     1 frame = 2 * 4 bytes. That is to say, 0x8000 frames = 256 KBytes 
 *  @param  void
 *  @return  void
 */
void set_irq_count(uint32_t irq_count, uint8_t irq_idx)
{
    if(irq_idx == 0)
       set_i2s_data(irq_count , AFE_IRQ_CNT);
    else if(irq_idx == 1)
       set_i2s_data(irq_count , AFE_IRQ1_CNT);
    else
       printk("[error] wrong irq index!\n");
}
EXPORT_SYMBOL(set_irq_count);

uint32_t get_irq_count(uint8_t irq_idx)
{
    if(irq_idx == 0)
       return get_i2s_data( AFE_IRQ_CNT);
    else if(irq_idx == 1)
       return get_i2s_data(AFE_IRQ1_CNT);
    else{
       printk("[error] wrong irq index!\n");
	   return 0;
    }
}
EXPORT_SYMBOL(get_irq_count);

void i2s_interrupt_init(void (*isr_func)(void))
{
	
	if(isr_func != NULL)
	    ecnt_i2s_isr_func = isr_func;
}
EXPORT_SYMBOL(i2s_interrupt_init);


/*  get_irq_status
 *
 *  @param  void
 *  @return  void
 */
uint32_t get_irq_status(void)
{
    return get_i2s_data(AFE_IRQ_STS);
}
EXPORT_SYMBOL(get_irq_status);

/*  get_irq_ctrl_0_status
 *
 *  @param  void
 *  @return  void
 */
uint32_t get_irq_ctrl_status(uint8_t irq_reg_index)
{
    if(irq_reg_index == 0)
        return get_i2s_data(AFE_IRQ_CON0);
    else if (irq_reg_index == 1)
        return get_i2s_data(AFE_IRQ1_CON0);
    else{
        printk("[error] wrong irq register index!\n"); 
	    return 0;		
    }
}
EXPORT_SYMBOL(get_irq_ctrl_status); 


void I2S_Clear_Interrupt(void)
{
    /* Clear IRQ */
	printk("\n Clear IRQ0 status.\n");	
    ECNT_IOWRITE32(AFE_IRQ_CON0, 0x1, 2, 0x1);				
    ECNT_IOWRITE32(AFE_IRQ_CON0, 0x1, 2, 0x0);

    printk(" Clear IRQ1 status.\n");						
    ECNT_IOWRITE32(AFE_IRQ1_CON0, 0x1, 2, 0x1);				
    ECNT_IOWRITE32(AFE_IRQ1_CON0, 0x1, 2, 0x0);		

    /* Clear miss flag */
    printk(" Clear IRQ0 miss flag.\n");		
    ECNT_IOWRITE32(AFE_IRQ_CON0, 0x1, 3, 0x1);				
    ECNT_IOWRITE32(AFE_IRQ_CON0, 0x1, 3, 0x0);					

    printk(" Clear IRQ1 miss flag.\n");							
    ECNT_IOWRITE32(AFE_IRQ1_CON0, 0x1, 3, 0x1);				
    ECNT_IOWRITE32(AFE_IRQ1_CON0, 0x1, 3, 0x0);					
}
EXPORT_SYMBOL(I2S_Clear_Interrupt);

void i2s_global_reset(void)
{
    u32 val = GET_SCU_RST_RG();
    SET_SCU_RST_RG(val | (0x1 << 10) );        //set 1	
    SET_SCU_RST_RG(val & (~(0x1 << 10)));	   //set 0
}
EXPORT_SYMBOL(i2s_global_reset);

/*  I2sLoopbackTest_HwTrig_Control
 *  Enable or disable I2S module. 
 *  Procedure:
 *    5. Enable MEMIF agent 
 *      5-1) Enable downlink1(TX) memory path
 *           AFE_DAC_CON0[17] = 0x1
 *      5-2) Enable uplink1(RX) memory path
 *           AFE_DAC_CON0[1] = 0x1
 *      5-3) Enable the whole AFE module
 *           AFE_DAC_CON0[0] = 0x1
 *
 *    6. Enable ETDM
 *      6-1) Enable ETDM Out
 *           ETDM_OUT1_CON0[0] = 0x1
 *      6-2) Enable ETDM In
 *           ETDM_IN1_CON0[0] = 0x1
 *
 *  @param  void
 *  @return  void
 */
void I2sLoopbackTest_HwTrig_Control(uint8_t isEnable)
{
    if(isEnable == 1)
        printk("\n I2S hardware enable \n");
	else if(isEnable == 0)
		printk(" I2S hardware disable \n");
	else
		printk(" [Error] wrong I2S hardware control parameter \n");

	ECNT_IOWRITE32(AFE_DAC_CON0  , 0x1, 17, isEnable);
    ECNT_IOWRITE32(AFE_DAC_CON0  , 0x1, 1 , isEnable);
    ECNT_IOWRITE32(AFE_DAC_CON0  , 0x1, 0 , isEnable);
    ECNT_IOWRITE32(ETDM_OUT1_CON0, 0x1, 0 , isEnable);
    ECNT_IOWRITE32(ETDM_IN1_CON0 , 0x1, 0 , isEnable);	
}
EXPORT_SYMBOL(I2sLoopbackTest_HwTrig_Control);

void I2S_Etdm_Cfg0(ETDM_CFG0 in1, ETDM_CFG0 out1)
{
    set_i2s_data(out1.word, ETDM_OUT1_CON0);	
    set_i2s_data(in1.word, ETDM_IN1_CON0);
}
EXPORT_SYMBOL(I2S_Etdm_Cfg0);

void I2S_Etdm_Cfg1(ETDM_CFG1 in1, ETDM_CFG1 out1)
{
    in1.bits.initial_count = 0xe;
    in1.bits.initial_point = 0xe;
    in1.bits.lrck_auto_off = 0x1;
    in1.bits.chen_sel = 0x1;
    in1.bits.lr_align = 0x1;

    out1.bits.initial_count = 0xe;
    out1.bits.initial_point = 0xe;
    out1.bits.lrck_auto_off = 0x1;
    out1.bits.chen_sel = 0x1;

    set_i2s_data(out1.word, ETDM_OUT1_CON1);
    set_i2s_data(in1.word, ETDM_IN1_CON1);
}
EXPORT_SYMBOL(I2S_Etdm_Cfg1);

void I2S_AFE_Cfg0_DL1(AFE_DL1_CFG0 dl1)
{
    dl1.bits.hd_audio_on = 0x1;
    dl1.bits.axi_maxLen = 0x1;	
    dl1.bits.pbuf_size = 0x3;
    set_i2s_data(dl1.word, AFE_DL1_CON0);

}
EXPORT_SYMBOL(I2S_AFE_Cfg0_DL1);

void I2S_AFE_Cfg0_UL1(AFE_UL1_CFG0 ul1)
{
    ul1.bits.hd_audio_on = 0x1;	
    ul1.bits.axi_maxLen = 0x1;
    ul1.bits.force_no_mask = 0x1;
    set_i2s_data(ul1.word, AFE_UL1_CON0);	
}
EXPORT_SYMBOL(I2S_AFE_Cfg0_UL1);

void I2S_Afe_Irq_Cfg0(AFE_IRQ_CFG0 afe_irq, uint8_t irq_reg_sel)
{
    if(irq_reg_sel == 0){
        set_i2s_data(afe_irq.word, AFE_IRQ_CON0);
	}else if(irq_reg_sel == 1){
        set_i2s_data(afe_irq.word, AFE_IRQ1_CON0);
	}else{
        printk("[Error] wrong irq_reg_sel number! \n");
	}	
}
EXPORT_SYMBOL(I2S_Afe_Irq_Cfg0);

static irqreturn_t i2s_interrupt_top_half_handler (
    int irq, void * dev_id )
{
	if(ecnt_i2s_isr_func != NULL)
	    ecnt_i2s_isr_func();

	return IRQ_HANDLED;
}

struct device* get_i2s_dev(void)
{
	if ((ecnt_i2s) && (ecnt_i2s->dev))
        return ecnt_i2s->dev;
	else{
		printk("[i2s] get_i2s_dev failed !\n");
    	return NULL;
	}	
}
EXPORT_SYMBOL(get_i2s_dev);

int get_i2s_irq(void)
{
	return ecnt_i2s->irq;
}
EXPORT_SYMBOL(get_i2s_irq);

static int ecnt_i2s_drv_probe(struct platform_device *pdev)
{	
	struct resource *res = NULL;
	
	printk("[i2s] ecnt_i2s_drv_probe\n");
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No i2s DT node found");
        return -EINVAL;
    }

	/* I2S allocate memory */
    ecnt_i2s = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_i2s), GFP_KERNEL);
    if (!ecnt_i2s)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_i2s);

	/* get irq num */
	ecnt_i2s->irq = platform_get_irq(pdev, 0);

#ifdef I2S_LOOPBACK_TEST
	res = devm_request_irq(&pdev->dev, ecnt_i2s->irq, i2s_interrupt_top_half_handler, 
	                           0, dev_name(&pdev->dev), ecnt_i2s);
	if(res){
		dev_err(&(pdev->dev), "devm_request_irq failed with err %d\n", res);
		return -EINVAL;
	}

    printk("[i2s] ecnt_i2s->irq: %d\n", ecnt_i2s->irq);
#endif
    /* get i2s base1 address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_i2s->dev = &pdev->dev;	
    ecnt_i2s->base1 = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_i2s->base1))
        return PTR_ERR(ecnt_i2s->base1); 
	
    #if 0
    printk("[i2s] res->name:%s\n", res->name);
    printk("[i2s] res->start:0x%llx ===\n", res->start);
    printk("[i2s] res->end:0x%llx ===\n", res->end);
    printk("[i2s] ecnt_i2s->base1:0x%lx\n", (unsigned long)ecnt_i2s->base1);
    #endif	
	
    /* get i2s base2 address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    ecnt_i2s->base2 = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_i2s->base2))
        return PTR_ERR(ecnt_i2s->base2); 
		
    #if 0
    printk("[i2s] res->name:%s\n", res->name);
    printk("[i2s] res->start:0x%llx ===\n", res->start);
    printk("[i2s] res->end:0x%llx ===\n", res->end);
    printk("[i2s] ecnt_i2s->base2:0x%lx\n", (unsigned long)ecnt_i2s->base2);
    #endif
	
    return 0;
}

static int ecnt_i2s_drv_remove(struct platform_device *pdev)
{
    printk("[i2s] ecnt_i2s_drv_remove\n");	
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_i2s_driver = {
    .probe = ecnt_i2s_drv_probe,
    .remove = ecnt_i2s_drv_remove,
    .driver = {
        .name = "ecnt-i2s",
        .of_match_table = ecnt_i2s_of_id
    },
};
module_platform_driver(ecnt_i2s_driver);
MODULE_DESCRIPTION("EcoNet i2s Driver");


