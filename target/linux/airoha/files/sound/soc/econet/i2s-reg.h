/*
 * i2s-reg.h  --  Econet audio driver reg definition
 *
 * Copyright (c) 2020 Econet Inc.
 * 
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

#ifndef _I2S_REG_H_
#define _I2S_REG_H_

/*****************************************************************************
 *                  R E G I S T E R       D E F I N I T I O N
 *****************************************************************************/
#define AFE_DAC_CON0_AFE_ON		(0x1 << 0)

/*the input reg params of i2s read/write functions are the offset values of i2s base addr, so set I2S_ADDR_BASE1 and I2S_ADDR_BASE2 0*/
#define I2S_ADDR_BASE1        0x0  //0xbfbe2200
#define I2S_ADDR_BASE2        0x0  //0xbfbe2e00

/* I2S Register Base1 Group */
#define AFE_DAC_CON0				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x000)
#define AFE_DAC_CON1				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x004)
#define AFE_MEMIF_BURST_CFG 	 	(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x008)
#define AFE_MEMIF_BUF_MON1  		(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x01c)
#define AFE_MEMIF_BUF_MON6  		(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x034)
#define ETDM_COWORK_CON0  			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x04c)
#define ETDM_COWORK_CON1  			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x050)
#define ETDM_IN1_CON0  				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x05c)
#define ETDM_IN1_CON1  				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x060)
#define ETDM_IN1_CON2  				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x064)
#define ETDM_IN1_CON3  				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x068)
#define ETDM_IN1_CON4				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x06c)
#define ETDM_IN1_CON5  				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x070)
#define ETDM_IN1_CON6  				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x074)
#define ETDM_IN1_MONITOR			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x078)
#define ETDM_OUT1_CON0				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x07c)
#define ETDM_OUT1_CON1				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x080)
#define ETDM_OUT1_CON2				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x084)
#define ETDM_OUT1_CON3				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x088)
#define ETDM_OUT1_CON4				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x08c)
#define ETDM_OUT1_CON6				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x094)
#define ETDM_OUT1_CON7				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x098)
#define ETDM_OUT1_MONITOR			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x09c)
#define AFE_DL1_CHK_SUM1			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0a0)
#define AFE_DL1_CHK_SUM2			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0a4)
#define AFE_DL1_BASE				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0a8)
#define AFE_DL1_CUR					(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0ac)
#define AFE_DL1_END					(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0b0)
#define AFE_DL1_CON0				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0b4)
#define AFE_UL1_CHK_SUM1			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0bc)
#define AFE_UL1_CHK_SUM2			(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0c0)
#define AFE_UL1_BASE				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0c4)
#define AFE_UL1_END					(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0c8)
#define AFE_UL1_CUR					(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0cc)
#define AFE_UL1_CON0				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0d0)
#define AFE_SIDEBAND0				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0dc)
#define AFE_SIDEBAND1				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0e0)
#define AFE_IRQ_CON0				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0e4)
#define AFE_IRQ_CNT					(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0e8)
#define AFE_IRQ_CNT_MON				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0ec)
#define AFE_IRQ_MON0				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0f0)
#define AFE_IRQ_MON1				(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0f4)
#define AFE_IRQ_STS					(volatile void __iomem *)(I2S_ADDR_BASE1 + 0x0f8)


/* I2S Register Base2 Group */
#define AFE_IRQ1_CON0					(I2S_ADDR_BASE2 + 0x0100)
#define AFE_IRQ1_CNT					(I2S_ADDR_BASE2 + 0x0104)
#define AFE_IRQ1_CNT_MON				(I2S_ADDR_BASE2 + 0x0108)
#define AFE_IRQ1_MON0					(I2S_ADDR_BASE2 + 0x010c)
#define AFE_IRQ1_MON1					(I2S_ADDR_BASE2 + 0x0110)

#define IRQ_RECORD_MASK					0X2   //IRQ0 for RECORD
#define IRQ_PLAY_MASK					0X1   //IRQ1 for PLAY


enum {
	I2S_PLAY,
	I2S_RECORD,
};

typedef enum { 
	I2S_POWER_ON, 
	I2S_POWER_OFF
} I2s_Power_e;

#endif
