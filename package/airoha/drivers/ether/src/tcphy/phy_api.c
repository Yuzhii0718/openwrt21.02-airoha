/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/dma-mapping.h>
#include <linux/mii.h>
#include <asm/io.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#ifndef TCSUPPORT_CPU_ARMV8
#include <asm/tc3162/TCIfSetQuery_os.h>
#endif 
#include <linux/if_vlan.h>

    
#ifdef TCPHY_SUPPORT
#include <asm/tc3162/cmdparse.h>
#include "../tcphy/tcswitch.h"
#endif
#ifdef TCSUPPORT_AUTOBENCH
#include "../../../auto_bench/autobench.h"
#endif
        
#ifdef TCSUPPORT_RA_HWNAT
#include <linux/foe_hook.h>
#endif
        
#include "tcconsole.h"
#include "../eth_lan.h"
#include "../eth_proc.h"
#include "phy_api.h"


#ifdef MT7510_DMA_DSCP_CACHE
#include <asm/r4kcache.h>
#endif
        
#ifdef LOOPBACK_SUPPORT
#include "../fe_verify.h"
#endif
        

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define USE_MDIO1_FOR_EXTEND_SWITCH 0

void tcMii_write_by_external_switch(uint32 phy_addr, uint32 phy_reg, uint32 phy_data);
uint32 tcMii_read_by_external_switch(uint32 phy_addr, uint32 phy_reg);
/************************************************************************
*                  M A C R O S
*************************************************************************
*/
	
#ifdef TCSUPPORT_CPU_EN7581
	enum PRESEVED_TABLE
	{
		GPIO_MDC_IO_MASTER_OFFSET = 0,
		GPIO_I2C_MASTER_OFFSET,
		GPIO_PCM_INT_OFFSET,
		GPIO_PCM_RESEET_OFFSET,
		FORCE_GPIO1_OFFSET,
		FORCE_GPIO2_OFFSET,
		GPIO2_FLASH_CFG_OFFSET,
		GPIO2_OUT_LED_DIS_OFFSET,
		GPIO2_OUT_LED_DIS_OFFSET_2,
		GPIO2_CTRL_OFFSET,
		GPIO2_DATA_OFFSET,
		GPIO2_OE_CTRL_OFFSET,
	};
	
#endif

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern spinlock_t phy_lock;

#ifdef TCSUPPORT_CPU_AN7583
extern spinlock_t phy_lock_ext0;
extern spinlock_t phy_lock_ext1;
#endif

extern macAdapter_t *mac_wan_p;
extern macAdapter_t *mac_p;


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
uint32 gswPbusRead(uint32 pbus_addr);
int gswPbusWrite(uint32 pbus_addr, uint32 pbus_data);


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/


/************************************************************************
    Funtcion:
    Description:    
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void tcMiiStationWriteUnlock(uint32 phy_addr, uint32 phy_reg, uint32 phy_data)
{
	uint32 reg;
	uint32 cnt=10000;
	u32 GSW_CFG_PIAC_addr;

	GSW_CFG_PIAC_addr = (isMT7530) ? (GSW_CFG_BASE + 0x001C) : GSW_CFG_PIAC;

	do {
		reg=read_reg_word (GSW_CFG_PIAC_addr);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_ST_START << MDIO_ST_SHIFT) | 
    	    (MDIO_CMD_WRITE<<MDIO_CMD_SHIFT) | 
    		(phy_addr << MDIO_PHY_ADDR_SHIFT) | 
    		(phy_reg << MDIO_REG_ADDR_SHIFT) | 
    		(phy_data & MDIO_RW_DATA);
	write_reg_word (GSW_CFG_PIAC_addr, reg);

	cnt = 10000;
	do {
		reg=read_reg_word (GSW_CFG_PIAC_addr);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	if (cnt==0) 
		printk("EER: tcMiiStationWrite timeout!\n");
}

uint32 tcMiiStationReadUnlock(uint32 phy_addr, uint32 phy_reg)
{
	uint32 reg;
	uint32 cnt=10000;
	u32 GSW_CFG_PIAC_addr;
    
	GSW_CFG_PIAC_addr = (isMT7530) ? (GSW_CFG_BASE + 0x001C) : GSW_CFG_PIAC;
	do {
		reg=read_reg_word (GSW_CFG_PIAC_addr);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_ST_START << MDIO_ST_SHIFT) | 
    	    (MDIO_CMD_READ<<MDIO_CMD_SHIFT) | 
    		(phy_addr << MDIO_PHY_ADDR_SHIFT) | 
    		(phy_reg << MDIO_REG_ADDR_SHIFT);
	write_reg_word (GSW_CFG_PIAC_addr, reg);

	cnt = 10000;
	do {
		reg=read_reg_word (GSW_CFG_PIAC_addr);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	reg = reg & MDIO_RW_DATA;

	if (cnt == 0) 
		printk("EER: tcMiiStationRead timeout!\n");

	return reg;
}

void tcMiiStationWriteUnlock_CL45(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg,uint32 phy_data)
{
	uint32 reg;
	uint32 cnt=10000;
	
	cnt = 10000;
	do {
		//pause(1);
		reg = read_reg_word(GSW_CFG_PIAC);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_ADDR << MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) |
            (phy_reg & MDIO_RW_DATA);
	write_reg_word (GSW_CFG_PIAC, reg);

	cnt = 10000;
	do {
		//pause(1);
		reg = read_reg_word(GSW_CFG_PIAC);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	
	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_WRITE<< MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) |
            (phy_data & MDIO_RW_DATA);
	write_reg_word (GSW_CFG_PIAC, reg);

	cnt = 10000;
	do {
		//pause(1);
		reg = read_reg_word(GSW_CFG_PIAC);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));


	if (cnt==0) 
		printk("ERR: tcMiiStationWrite_CL45 timeout!\n");
}

uint32 tcMiiStationReadUnlock_CL45(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg)
{
         uint32 reg;
	uint32 cnt=10000;
         uint16 cl45_value=0;
		 
	cnt = 10000;
	do {
		//pause(1);
		reg = read_reg_word(GSW_CFG_PIAC);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_ADDR << MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) |
            (phy_reg & MDIO_RW_DATA);
	write_reg_word (GSW_CFG_PIAC, reg);

	cnt = 10000;
	do {
		//pause(1);
		reg = read_reg_word(GSW_CFG_PIAC);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_POSTREAD_INCADDR<< MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) ;
   
	write_reg_word (GSW_CFG_PIAC, reg);

	cnt = 10000;
	do {
		//pause(1);
		reg = read_reg_word(GSW_CFG_PIAC);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	
	if (cnt == 0) 
			printk("ERR: tcMiiStationRead_CL45 timeout!\n");
	cl45_value = reg;

	return cl45_value;

}

#ifdef TCSUPPORT_CPU_AN7583

void tcMiiStationWriteUnlock_ext(uint32 phy_addr, uint32 phy_reg, uint32 phy_data, uint32 bank)
{
	volatile uint32 reg;
	uint32 cnt=10000;


	do {
		reg= GET_MDIOIAC(bank);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_ST_START << MDIO_ST_SHIFT) | 
    	    (MDIO_CMD_WRITE<<MDIO_CMD_SHIFT) | 
    		(phy_addr << MDIO_PHY_ADDR_SHIFT) | 
    		(phy_reg << MDIO_REG_ADDR_SHIFT) | 
    		(phy_data & MDIO_RW_DATA);
	SET_MDIOIAC(bank, reg);
	wmb();
	cnt = 10000;
	do {
		reg=GET_MDIOIAC(bank);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	if (cnt==0) 
		printk("EER: tcMiiStationWrite timeout!\n");
}

uint32 tcMiiStationReadUnlock_ext(uint32 phy_addr, uint32 phy_reg, uint32 bank)
{
	volatile uint32 reg;
	uint32 cnt=10000;

	do {
		reg= GET_MDIOIAC(bank);
		mb();
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	if (cnt == 0)
	{
		printk("timeout !!!!\n");
	}
	reg = PHY_ACS_ST | (MDIO_ST_START << MDIO_ST_SHIFT) | 
    	    (MDIO_CMD_READ<<MDIO_CMD_SHIFT) | 
    		(phy_addr << MDIO_PHY_ADDR_SHIFT) | 
    		(phy_reg << MDIO_REG_ADDR_SHIFT);
	SET_MDIOIAC(bank, reg);
	wmb();
	cnt = 10000;
	do {
		reg= GET_MDIOIAC(bank);
//		printk("reg = %x\n",reg);
		mb();
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	if (cnt == 0)
	{
		printk("timeout !!!!\n");
	}	
	reg = reg & MDIO_RW_DATA;
//	printk("after tcMiiStationReadUnlock_ext phy_reg = %x, bank = %x reg = %x\n",phy_reg, bank, reg);

	if (cnt == 0) 
		printk("EER: tcMiiStationRead timeout!\n");

	return reg;
}

void tcMiiStationWriteUnlock_CL45_ext(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg,uint32 phy_data,uint32 bank)
{
	volatile uint32 reg;
	uint32 cnt=10000;
	
	cnt = 10000;
	do {
		//pause(1);
		reg= GET_MDIOIAC(bank);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_ADDR << MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) |
            (phy_reg & MDIO_RW_DATA);
	SET_MDIOIAC(bank, reg);
	wmb();

	cnt = 10000;
	do {
		//pause(1);
		reg= GET_MDIOIAC(bank);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	
	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_WRITE<< MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) |
            (phy_data & MDIO_RW_DATA);
	SET_MDIOIAC(bank, reg);
	wmb();

	cnt = 10000;
	do {
		//pause(1);
		reg= GET_MDIOIAC(bank);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));


	if (cnt==0) 
		printk("ERR: tcMiiStationWrite_CL45 timeout!\n");
}

uint32 tcMiiStationReadUnlock_CL45_ext(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg, uint32 bank)
{
    volatile uint32 reg;
	uint32 cnt=10000;
         uint16 cl45_value=0;
		 
	cnt = 10000;
	do {
		//pause(1);
		reg= GET_MDIOIAC(bank);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_ADDR << MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) |
            (phy_reg & MDIO_RW_DATA);
	SET_MDIOIAC(bank, reg);
	wmb();
	cnt = 10000;
	do {
		//pause(1);
		reg= GET_MDIOIAC(bank);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_CL45_ST_START << MDIO_ST_SHIFT) | 
	        (MDIO_CL45_CMD_POSTREAD_INCADDR<< MDIO_CMD_SHIFT) |
	        (phy_addr << MDIO_PHY_ADDR_SHIFT) | 
	        (dev_addr << MDIO_REG_ADDR_SHIFT) ;
   
	SET_MDIOIAC(bank, reg);
	wmb();

	cnt = 10000;
	do {
		//pause(1);
		reg= GET_MDIOIAC(bank);
		cnt --;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	
	if (cnt == 0) 
			printk("ERR: tcMiiStationRead_CL45 timeout!\n");
	cl45_value = reg;

	return cl45_value;

}

#endif

#ifdef TCSUPPORT_CPU_AN7583
/************************************************************************
    Funtcion: tcMiiStationWrite_CL45
    Description: mdio write using CL45 protocol
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void tcMiiStationWrite_CL45_ext(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg,uint32 phy_data, uint32 bank)
{
    int isIsr = in_softirq();


    if(0 != isIsr)
    {
    	if(bank == 0)
    	{
    		spin_lock(&phy_lock_ext0);
    	}
		else
		{
			spin_lock(&phy_lock_ext1);
		}
	    
    }
	else
	{
		if(bank == 0)
		{
			spin_lock_bh(&phy_lock_ext0);
		}
		else
		{
			spin_lock_bh(&phy_lock_ext1);
		}
		
	}

    tcMiiStationWriteUnlock_CL45_ext(phy_addr,dev_addr,phy_reg,phy_data , bank);
    
    if(0 != isIsr)
    {
    	if(bank == 0)
    	{
    		spin_unlock(&phy_lock_ext0);
    	}
		else
		{
			spin_unlock(&phy_lock_ext1);
		}
	    
    }
	else
	{
		if(bank == 0)
		{
			spin_unlock_bh(&phy_lock_ext0);
		}
		else
		{
			spin_unlock_bh(&phy_lock_ext1);
		}
		
	}

	return;
}

EXPORT_SYMBOL(tcMiiStationWrite_CL45_ext);

/************************************************************************
    Funtcion: tcMiiStationRead_CL45
    Description: mdio read using CL45 protocol
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:     
************************************************************************/

uint32 tcMiiStationRead_CL45_ext(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg, uint32 bank)
{
	uint32 reg;
//#ifdef TCSUPPORT_CPU_EN7581
//	uint32 iomux_bit = 0;;
//#endif
    int isIsr = in_softirq();

    if(0 != isIsr)
    {
    	if(bank == 0)
    	{
    		spin_lock(&phy_lock_ext0);
    	}
		else
		{
			spin_lock(&phy_lock_ext1);
		}
	    
    }
	else
	{
		if(bank == 0)
		{
			spin_lock_bh(&phy_lock_ext0);
		}
		else
		{
			spin_lock_bh(&phy_lock_ext1);
		}
		
	}

	


    reg = tcMiiStationReadUnlock_CL45_ext(phy_addr,dev_addr,phy_reg,bank);



    if(0 != isIsr)
    {
    	if(bank == 0)
    	{
    		spin_unlock(&phy_lock_ext0);
    	}
		else
		{
			spin_unlock(&phy_lock_ext1);
		}
	    
    }
	else
	{
		if(bank == 0)
		{
			spin_unlock_bh(&phy_lock_ext0);
		}
		else
		{
			spin_unlock_bh(&phy_lock_ext1);
		}
		
	}


	return reg;
}


EXPORT_SYMBOL(tcMiiStationRead_CL45_ext);
#endif

#ifdef TCSUPPORT_CPU_EN7581

uint32 force_mdio0_en(uint32 iomux_bit, int enable)
{
	uint32 gpio = 0;
	uint32 iomux_reg = 0;

	if (enable == 1)
	{
		/* preserve the regs status */
		iomux_reg = GET_PON_I2C_MODE();
		if ((iomux_reg & (0x1<<14)) != 0)
		{
			iomux_bit |= (0x1<<GPIO_MDC_IO_MASTER_OFFSET);
		}
			if ((iomux_reg & (0x1<<13)) != 0)
		{
			iomux_bit |= (0x1<<GPIO_I2C_MASTER_OFFSET);
		}
		iomux_reg = GET_IOMUX_CTRL_2();
		if ((iomux_reg & (0x1<<9)) != 0)
		{
			iomux_bit |= (0x1<<GPIO_PCM_INT_OFFSET);
		}
		if ((iomux_reg & (0x1<<8)) != 0)
		{
			iomux_bit |= (0x1<<GPIO_PCM_RESEET_OFFSET);
		}
		iomux_reg = GET_GPIO2_EN();
		if ((iomux_reg & (0x1<<1)) != 0)
		{
			iomux_bit |= (0x1<<FORCE_GPIO1_OFFSET);
		}
		if	((iomux_reg & (0x1<<2))!= 0)
		{
			iomux_bit |= (0x1<<FORCE_GPIO2_OFFSET);
		}
		gpio = GPIOPWM_GET_GPIO_FLASH_MODE_CFG(0);
			if ((gpio & (0x1<<2)) != 0)
		{
			iomux_bit |= (0x1<<GPIO2_FLASH_CFG_OFFSET);
		}
			
		gpio = GET_GPIO_LEDCTRL();
		if ((gpio & (0x1<<4)) != 0)
		{
			iomux_bit |= (0x1<<GPIO2_OUT_LED_DIS_OFFSET);
		}
		if ((gpio & (0x1<<5)) != 0)
		{
			iomux_bit |= (0x1<<GPIO2_OUT_LED_DIS_OFFSET_2);
		}
		
		gpio = GET_GPIO_CTRL();

		if ((gpio & (0x1<<4)) != 0)
		{
			iomux_bit |= (0x1<<GPIO2_CTRL_OFFSET);
		}
		gpio = GET_GPIO_DATA();
		if ((gpio & (0x1<<2)) != 0)
		{
			iomux_bit |= (0x1<<GPIO2_DATA_OFFSET);
		}

		gpio = GET_GPIO_OE();

		if ((gpio & (0x1<<2)) != 0)
		{
			iomux_bit |= (0x1<<GPIO2_OE_CTRL_OFFSET);
		}

		/* if the second MDIO hasn't enabled, start workaround process*/
		if (!((iomux_bit >> GPIO_MDC_IO_MASTER_OFFSET) & 0x1))
		{
			gpio = GET_GPIO_CTRL();
			SET_GPIO_CTRL(gpio | (0x1<<4));
	
			gpio = GET_GPIO_DATA();
			SET_GPIO_DATA(gpio | (0x1<<2));
	
			gpio = GET_GPIO_OE();
			SET_GPIO_OE(gpio | (0x1<<2));
	
			gpio = GET_GPIO_LEDCTRL();
			SET_GPIO_LEDCTRL((gpio & ~((0x3)<<4)));
	
			gpio = GPIOPWM_GET_GPIO_FLASH_MODE_CFG(0);
			GPIOPWM_SET_GPIO_FLASH_MODE_CFG(0, gpio & ~(0x1<<2));
	
			/* set gpio2 and set the IOMUX to enable mdio1*/
			if (((iomux_bit >> FORCE_GPIO1_OFFSET) & 0x1) || ((iomux_bit >> GPIO_PCM_INT_OFFSET) & 0x1) || ((iomux_bit >> GPIO_I2C_MASTER_OFFSET) & 0x1))
			{
				SET_GPIO2_EN((0x1<<2), (0x1<<2));
			}
			else
			{
				SET_GPIO2_EN((0x3 <<1), (0x3<<1));
			}
			SET_IOMUX_CTRL_BIT(0x20008,0);
			SET_IOMUX_CTRL_BIT(0x20009,0);
			SET_IOMUX_CTRL_BIT(0x1000d,0);
			SET_IOMUX_CTRL_BIT(0x1000e,1);			

		}

	}
	else if (enable == 0)
	{
		/* recover the gpio/iomux status*/
		if (!((iomux_bit >> GPIO_MDC_IO_MASTER_OFFSET) & 0x1))
		{
			gpio = GET_GPIO_CTRL();
			if (!((iomux_bit>>GPIO2_CTRL_OFFSET) & 0x1))
			{
				gpio &= ~(0x1<<4);		
			}

			SET_GPIO_CTRL(gpio);

			gpio = GET_GPIO_DATA();

			if (!((iomux_bit>>GPIO2_DATA_OFFSET) & 0x1))
			{
				gpio &= ~(0x1<<2);		
			}
			SET_GPIO_DATA(gpio);

			gpio = GET_GPIO_OE();

			if (!((iomux_bit>>GPIO2_OE_CTRL_OFFSET) & 0x1))
			{
				gpio &= ~(0x1<<2);		
			}
			SET_GPIO_OE(gpio);

			gpio = GET_GPIO_LEDCTRL();

			if (((iomux_bit>>GPIO2_OUT_LED_DIS_OFFSET) & 0x1))
			{
				gpio |= (0x1<<4);
			}
			if (((iomux_bit>>GPIO2_OUT_LED_DIS_OFFSET_2) & 0x1))
			{
				gpio |= (0x1<<5);		
			}
			SET_GPIO_LEDCTRL(gpio);
			
			gpio = GPIOPWM_GET_GPIO_FLASH_MODE_CFG(0);

			if (((iomux_bit>>GPIO2_FLASH_CFG_OFFSET) & 0x1))
			{
				gpio |= (0x1<<2);
			}
			GPIOPWM_SET_GPIO_FLASH_MODE_CFG(0,gpio);

			gpio = 0;
			
			if ((iomux_bit>>FORCE_GPIO2_OFFSET) & 0x1)
			{
				gpio |= (0x1<<2);
			}
			if ((iomux_bit>>FORCE_GPIO1_OFFSET) & 0x1)
			{
				gpio |= (0x1<<1);		
			}
			SET_GPIO2_EN((0x3 <<1), gpio);
			
			SET_IOMUX_CTRL_BIT(0x20008,(iomux_bit>>GPIO_PCM_RESEET_OFFSET) & 0x1);
			SET_IOMUX_CTRL_BIT(0x20009,(iomux_bit>>GPIO_PCM_INT_OFFSET) & 0x1);
			SET_IOMUX_CTRL_BIT(0x1000d,(iomux_bit>>GPIO_I2C_MASTER_OFFSET) & 0x1);
			SET_IOMUX_CTRL_BIT(0x1000e,(iomux_bit>>GPIO_MDC_IO_MASTER_OFFSET) & 0x1);
			delay1us(100);

		}

	}
	else
	{
		;
	}
	return iomux_bit;
	
}

#endif


/************************************************************************
    Funtcion:
    Description:    
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void tcMiiStationWrite(uint32 phy_addr, uint32 phy_reg, uint32 phy_data)
{
    int isIsr = in_softirq();


    if(0 != isIsr)
	    spin_lock(&phy_lock);
	else
	    spin_lock_bh(&phy_lock);

#ifdef TCSUPPORT_CPU_AN7583
	
		if (((phy_addr > 12) || (phy_addr < 9)) && (phy_addr != 7))
		{
			tcMiiStationWriteUnlock_ext(phy_addr, phy_reg, phy_data,0);
		}
		else
#endif
	{
	    if(phy_addr >=23 && phy_addr <=28 && USE_MDIO1_FOR_EXTEND_SWITCH){
			tcMii_write_by_external_switch(phy_addr, phy_reg, phy_data);
		}else{
			tcMiiStationWriteUnlock(phy_addr,phy_reg,phy_data);
		}
	}
    if(0 != isIsr)
	    spin_unlock(&phy_lock);
	else
	    spin_unlock_bh(&phy_lock);
	return;
}


/************************************************************************
    Funtcion:
    Description:    
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:     
************************************************************************/

uint32 tcMiiStationRead(uint32 phy_addr, uint32 phy_reg)
{
	uint32 reg;
#ifdef TCSUPPORT_CPU_EN7581
	uint32 iomux_bit = 0;;
#endif
    int isIsr = in_softirq();

    if(0 != isIsr)
	    spin_lock(&phy_lock);
	else
	    spin_lock_bh(&phy_lock);
#ifdef TCSUPPORT_CPU_EN7581
	if (isEN7581 && ((GET_PDIDR() == 0x1) || (GET_ECO_HWFIX() == 0x0)))
	{
		if((phy_addr > 12) || (phy_addr < 9))
		{
			iomux_bit = force_mdio0_en(0, 1);
		}
	}
#endif

#ifdef TCSUPPORT_CPU_AN7583

	if (((phy_addr > 12) || (phy_addr < 9)) && (phy_addr != 7))
	{
		reg = tcMiiStationReadUnlock_ext(phy_addr, phy_reg, 0);
	}
	else
#endif
	{
	    if(phy_addr >=23 && phy_addr <=28 && USE_MDIO1_FOR_EXTEND_SWITCH){
			reg = tcMii_read_by_external_switch(phy_addr, phy_reg);
	    }else{
			reg = tcMiiStationReadUnlock(phy_addr, phy_reg);
	   	}
	}
#ifdef TCSUPPORT_CPU_EN7581
	if (isEN7581 && ((GET_PDIDR() == 0x1) || (GET_ECO_HWFIX() == 0x0)))
	{
		if((phy_addr > 12) || (phy_addr < 9))
		{
			force_mdio0_en(iomux_bit, 0);
		}
	}
#endif
	if(0 != isIsr)
		spin_unlock(&phy_lock);
	else
		spin_unlock_bh(&phy_lock);
	return reg;
}

/************************************************************************
    Funtcion: tcMiiStationWrite_CL45
    Description: mdio write using CL45 protocol
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void tcMiiStationWrite_CL45(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg,uint32 phy_data)
{
    int isIsr = in_softirq();


    if(0 != isIsr)
	    spin_lock(&phy_lock);
	else
	    spin_lock_bh(&phy_lock);

#ifdef TCSUPPORT_CPU_AN7583
		
	if (((phy_addr > 12) || (phy_addr < 9)) && (phy_addr != 7))
	{
		tcMiiStationWriteUnlock_CL45_ext(phy_addr,dev_addr,phy_reg,phy_data,0);
	}
	else
#endif
	    tcMiiStationWriteUnlock_CL45(phy_addr,dev_addr,phy_reg,phy_data);
    
    if(0 != isIsr)
	    spin_unlock(&phy_lock);
	else
	    spin_unlock_bh(&phy_lock);
	return;
}

EXPORT_SYMBOL(tcMiiStationWrite_CL45);

/************************************************************************
    Funtcion: tcMiiStationRead_CL45
    Description: mdio read using CL45 protocol
    Calls:
    Called by:      
    Input:
    Output:
    Return:
    Others:     
************************************************************************/

uint32 tcMiiStationRead_CL45(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg)
{
	uint32 reg;
#ifdef TCSUPPORT_CPU_EN7581
	uint32 iomux_bit = 0;;
#endif
    int isIsr = in_softirq();

    if(0 != isIsr)
	    spin_lock(&phy_lock);
	else
	    spin_lock_bh(&phy_lock);
	
#ifdef TCSUPPORT_CPU_EN7581
	if (isEN7581 && ((GET_PDIDR() == 0x1) || (GET_ECO_HWFIX() == 0x0)))
	{
		if((phy_addr > 12) || (phy_addr < 9))
		{
			iomux_bit = force_mdio0_en(0, 1);
		}
	}
#endif
#ifdef TCSUPPORT_CPU_AN7583
			
	if (((phy_addr > 12) || (phy_addr < 9)) && (phy_addr != 7))
	{
		reg = tcMiiStationReadUnlock_CL45_ext(phy_addr,dev_addr,phy_reg,0);
	}
	else
#endif

    reg = tcMiiStationReadUnlock_CL45(phy_addr,dev_addr,phy_reg);

#ifdef TCSUPPORT_CPU_EN7581
	if (isEN7581 && ((GET_PDIDR() == 0x1) || (GET_ECO_HWFIX() == 0x0)))
	{
		if((phy_addr > 12) || (phy_addr < 9))
		{
			force_mdio0_en(iomux_bit, 0);
		}
	}
#endif

    if(0 != isIsr)
	    spin_unlock(&phy_lock);
	else
	    spin_unlock_bh(&phy_lock);

	return reg;
}


EXPORT_SYMBOL(tcMiiStationRead_CL45);


/************************************************************************
    Funtcion:       miiStationWrite
    Description:    
    Calls:
    Called by:      phy driver
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void miiStationWrite(macAdapter_t *mac_p, uint32 phy_reg, uint32 phy_data)
{
	tcMiiStationWrite(mac_p->enetPhyAddr, phy_reg, phy_data);
}


/************************************************************************
    Funtcion:       miiStationRead
    Description:    
    Calls:
    Called by:      phy driver
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
uint32 miiStationRead(macAdapter_t *mac_p, uint32 phy_reg)
{
	return tcMiiStationRead(mac_p->enetPhyAddr, phy_reg);
}



int mdio_read(struct net_device *dev, int phy_id, int reg_num)
{
	return tcMiiStationRead(phy_id, reg_num);
}

void mdio_write(struct net_device *dev, int phy_id, int reg_num, int val)
{
	tcMiiStationWrite(phy_id, reg_num, val);
}

/************************************************************************
    Funtcion:       mdio_cl22_read
    Description:    
    Calls:
    Called by:      trgmii setting
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
uint32 mdio_cl22_read(uint32 port_num, uint32 dev_num, uint32 reg_num)
{

    const uint16 MMD_Control_register=0xD;
    const uint16 MMD_addr_data_register=0xE;
  	const uint16 page_reg=31;
	
	uint32 value=0;
    int isIsr = in_softirq();

	if(0 != isIsr)
	    spin_lock(&phy_lock);
	else
	    spin_lock_bh(&phy_lock);
	tcMiiStationWriteUnlock(port_num, page_reg, 0x00); //switch to main page
	tcMiiStationWriteUnlock(port_num, MMD_Control_register, (0<<14)+dev_num);
	tcMiiStationWriteUnlock(port_num, MMD_addr_data_register, reg_num);
	tcMiiStationWriteUnlock(port_num, MMD_Control_register, (1<<14)+dev_num);
	value = tcMiiStationReadUnlock(port_num, MMD_addr_data_register);
	if(0 != isIsr)
	    spin_unlock(&phy_lock);
	else
	    spin_unlock_bh(&phy_lock);
//	printf("* doPhyMMDRead_CL22=>phyaddr=%d,  dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", port_num, dev_num, reg_num, value);
    return value;
}

/************************************************************************
    Funtcion:       mdio_cl22_write
    Description:    
    Calls:
    Called by:      trgmii setting
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void mdio_cl22_write(uint32 port_num,uint32 dev_num,uint32 reg_num,uint32 reg_data)
{
    const uint16 MMD_Control_register=0xD;
    const uint16 MMD_addr_data_register=0xE;
  	const uint16 page_reg=31;
    int isIsr = in_softirq();

	if(0 != isIsr)
	    spin_lock(&phy_lock);
	else
	    spin_lock_bh(&phy_lock);

	tcMiiStationWriteUnlock(port_num, page_reg, 0x00); //switch to main page
	tcMiiStationWriteUnlock(port_num, MMD_Control_register, (0<<14)+dev_num);
	tcMiiStationWriteUnlock(port_num, MMD_addr_data_register, reg_num);
	tcMiiStationWriteUnlock(port_num, MMD_Control_register, (1<<14)+dev_num);
	tcMiiStationWriteUnlock(port_num, MMD_addr_data_register, reg_data);

	if(0 != isIsr)
	    spin_unlock(&phy_lock);
	else
	    spin_unlock_bh(&phy_lock);
//	printf("* doPhyMMDWrite_CL22=> phyaddr=%d, dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", port_num, dev_num, reg_num, reg_data);
}

/************************************************************************
    Funtcion:       mdio_cl45_read
    Description:    
    Calls:
    Called by:      tce emiir
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
uint16 mdio_cl45_read(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg)
{
	return tcMiiStationRead_CL45(phy_addr, dev_addr, phy_reg);
}

EXPORT_SYMBOL(mdio_cl45_read);

/************************************************************************
    Funtcion:       mdio_cl45_write
    Description:    
    Calls:
    Called by:      tce emiir
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void mdio_cl45_write(uint32 phy_addr, uint32 dev_addr, uint32 phy_reg,uint32 phy_data)
{
	tcMiiStationWrite_CL45(phy_addr, dev_addr, phy_reg, phy_data);
}

EXPORT_SYMBOL(mdio_cl45_write);

uint32 gswPbusRead_unlock(uint32 pbus_addr, uint32 phyaddr)
{
	uint32 pbus_data;
	uint32 reg;
	uint32 value;

	// 1. write high-bit page address
	reg = 31;
	value = (pbus_addr >> 6);
  	tcMiiStationWriteUnlock(phyaddr, reg, value);

	// 2. read low DWord
	reg = (pbus_addr>>2) & 0x000f;
	value = tcMiiStationReadUnlock(phyaddr, reg);
	pbus_data = value;

	// 3. read high DWord
	reg = 16;
		value = tcMiiStationReadUnlock(phyaddr, reg);

	pbus_data = (pbus_data) | (value<<16);

	return pbus_data;
}


int gswPbusWrite_unlock(uint32 pbus_addr, uint32 pbus_data, uint32 phyaddr)
{
	uint32 reg;
	uint32 value;
    
	// 1. write high-bit page address
	reg = 31;
	value = (pbus_addr >> 6);
	tcMiiStationWriteUnlock(phyaddr, reg, value);

	// 2. write low DWord
	reg = (pbus_addr>>2) & 0x000f;
	value = pbus_data & 0xffff;
	tcMiiStationWriteUnlock(phyaddr, reg, value);

	// 3. write high DWord
	reg = 16;
	value = (pbus_data>>16) & 0xffff;
	tcMiiStationWriteUnlock(phyaddr, reg, value);

  	return 0;
} 
uint32 gswPbusRead(uint32 pbus_addr)
{
	uint32 pbus_data;

	uint32 phyaddr;
	uint32 reg;
	uint32 value;

    int isIsr = in_softirq();

    if(0 != isIsr)
        spin_lock(&phy_lock);
    else
        spin_lock_bh(&phy_lock);

	pbus_data = gswPbusRead_unlock(pbus_addr,31);

    if(0 != isIsr)
        spin_unlock(&phy_lock);
    else
        spin_unlock_bh(&phy_lock);
	return pbus_data;
} /* end frank modify for rt62806 */

int gswPbusWrite(uint32 pbus_addr, uint32 pbus_data)
{
	uint32 phyaddr;
	uint32 reg;
	uint32 value;

    int isIsr = in_softirq();

    if(0 != isIsr)
        spin_lock(&phy_lock);
    else
        spin_lock_bh(&phy_lock);

	gswPbusWrite_unlock(pbus_addr, pbus_data,31);
    if(0 != isIsr)
        spin_unlock(&phy_lock);
    else
	    spin_unlock_bh(&phy_lock);
  	return 0;
} /* end frank modify for rt62806 */


uint32 gswPbusRead2(uint32 pbus_addr)
{
	uint32 pbus_data;

	uint32 phyaddr;
	uint32 reg;
	uint32 value;

    int isIsr = in_softirq();

    if(0 != isIsr)
        spin_lock(&phy_lock);
    else
        spin_lock_bh(&phy_lock);

	pbus_data = gswPbusRead_unlock(pbus_addr,23);

	if(0 != isIsr)
        spin_unlock(&phy_lock);
    else
	    spin_unlock_bh(&phy_lock);
    
	return pbus_data;
} 
int gswPbusWrite2(uint32 pbus_addr, uint32 pbus_data)
{
	uint32 phyaddr;
	uint32 reg;
	uint32 value;
    int isIsr = in_softirq();

	if(0 != isIsr)
        spin_lock(&phy_lock);
    else
	    spin_lock_bh(&phy_lock);

	gswPbusWrite_unlock(pbus_addr, pbus_data,23);

	if(0 != isIsr)
        spin_unlock(&phy_lock);
    else
	    spin_unlock_bh(&phy_lock);

    //printk("gswPbusWrite2:phy_lock --\n");
  	return 0;
} /* end frank modify for rt62806 */
void tcMii_write_by_external_switch(uint32 phy_addr, uint32 phy_reg, uint32 phy_data)
{
	uint32 reg;
	uint32 cnt=10000;
	u32 GSW_CFG_PIAC_addr;

//	GSW_CFG_PIAC_addr = (isMT7530 || isMT7530ext) ? (GSW_CFG_BASE + 0x001C) : GSW_CFG_PIAC;
	GSW_CFG_PIAC_addr = (isMT7530) ? (GSW_CFG_BASE + 0x001C) : GSW_CFG_PIAC;
	GSW_CFG_PIAC_addr -= GSW_BASE;

	do {
		reg=gswPbusRead_unlock (GSW_CFG_PIAC_addr, 31);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_ST_START << MDIO_ST_SHIFT) | 
    	    (MDIO_CMD_WRITE<<MDIO_CMD_SHIFT) | 
    		(phy_addr << MDIO_PHY_ADDR_SHIFT) | 
    		(phy_reg << MDIO_REG_ADDR_SHIFT) | 
    		(phy_data & MDIO_RW_DATA);
	gswPbusWrite_unlock(GSW_CFG_PIAC_addr, reg, 31);

	cnt = 10000;
	do {
		//pause(1);
		reg=gswPbusRead_unlock (GSW_CFG_PIAC_addr, 31);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	
	if (cnt==0) 
		printk("EER: tcMiiStationWrite timeout!\n");
}

uint32 tcMii_read_by_external_switch(uint32 phy_addr, uint32 phy_reg)
{
	uint32 reg;
	uint32 cnt=10000;
	u32 GSW_CFG_PIAC_addr;
//	GSW_CFG_PIAC_addr = (isMT7530 || isMT7530ext) ? (GSW_CFG_BASE + 0x001C) : GSW_CFG_PIAC;
	GSW_CFG_PIAC_addr = (isMT7530) ? (GSW_CFG_BASE + 0x001C) : GSW_CFG_PIAC;
	GSW_CFG_PIAC_addr -= GSW_BASE;

	do {
		reg=gswPbusRead_unlock (GSW_CFG_PIAC_addr, 31);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));

	reg = PHY_ACS_ST | (MDIO_ST_START << MDIO_ST_SHIFT) | 
    	    (MDIO_CMD_READ<<MDIO_CMD_SHIFT) | 
    		(phy_addr << MDIO_PHY_ADDR_SHIFT) | 
    		(phy_reg << MDIO_REG_ADDR_SHIFT);
	gswPbusWrite_unlock (GSW_CFG_PIAC_addr, reg, 31);

	cnt = 10000;
	do {
		//pause(1);
		reg=gswPbusRead_unlock (GSW_CFG_PIAC_addr, 31);
		cnt--;
	} while ((reg & PHY_ACS_ST) && (cnt != 0));
	reg = reg & MDIO_RW_DATA;

	if (cnt == 0) 
		printk("EER: tcMiiStationRead timeout!\n");

	return reg;
}



/* write extend switch by mdio*/
int
gswPbusWrite_by_mdio1(uint32 pbus_addr, uint32 pbus_data, uint32 phyaddr)
{
	uint32 reg;
	uint32 value;

    int isIsr = in_softirq();

    if(0 != isIsr)
        spin_lock(&phy_lock);
    else
	    spin_lock_bh(&phy_lock);

	// 1. write high-bit page address
	reg = 31;
	value = (pbus_addr >> 6);
	tcMii_write_by_external_switch(phyaddr, reg, value);

	// 2. write low DWord
	reg = (pbus_addr>>2) & 0x000f;
	value = pbus_data & 0xffff;
	tcMii_write_by_external_switch(phyaddr, reg, value);

	// 3. write high DWord
	reg = 16;
	value = (pbus_data>>16) & 0xffff;
	tcMii_write_by_external_switch(phyaddr, reg, value);

	if(0 != isIsr)
        spin_unlock(&phy_lock);
    else
	    spin_unlock_bh(&phy_lock);

    
  	return 0;
} /* end frank modify for rt62806 */


/* read extend switch by mdio*/
uint32
gswPbusRead_by_mdio1(uint32 pbus_addr, uint32 phyaddr)
{
	uint32 pbus_data;

	uint32 reg;
	uint32 value;

    int isIsr = in_softirq();

    if(0 != isIsr)
        spin_lock(&phy_lock);
    else
    	spin_lock_bh(&phy_lock);

	// 1. write high-bit page address
	reg = 31;
	value = (pbus_addr >> 6);
  	tcMii_write_by_external_switch(phyaddr, reg, value);

	// 2. read low DWord
	reg = (pbus_addr>>2) & 0x000f;
	value = tcMii_read_by_external_switch(phyaddr, reg);
	pbus_data = value;

	// 3. read high DWord
	reg = 16;
		value = tcMii_read_by_external_switch(phyaddr, reg);

	pbus_data = (pbus_data) | (value<<16);

	if(0 != isIsr)
        spin_unlock(&phy_lock);
    else
	    spin_unlock_bh(&phy_lock);
	
	return pbus_data;
} /* end frank modify for rt62806 */
//PHY2 read/write
uint32 gswPmiRead(uint32 phy_addr, uint32 phy_reg)
{
	uint32 pbus_addr;
	uint32 pbus_data;
	uint32 phy_data;
	uint32 phy_acs_st;
    int isIsr = in_softirq();
//	uint32 max_wait_cnt = 1000;

	pbus_addr = 0x701c;
	// b31	- phy access 1:start&busy, 0:complete&idle
	// b29:25 - mdio phy reg addr
	// b24:20 - mdio phy addr
	// b19:18 - 2'b01: write, 2'b10: read
	// b17:16 - start field, always 2'b01
	// b15:0	- data

	phy_addr = phy_addr & 0x1f;
	phy_reg  = phy_reg & 0x1f;

	// 1. write phy_addr & phy_reg
	pbus_data = 0x80090000; // read
	pbus_data = pbus_data | (phy_addr<<20);
	pbus_data = pbus_data | (phy_reg<<25);

	if(0 != isIsr)
        spin_lock(&phy_lock);
    else
	    spin_lock_bh(&phy_lock);

	gswPbusWrite_unlock(pbus_addr,pbus_data,31);

	// 2. check phy_acs_st
	phy_acs_st = 1;
	while (phy_acs_st) {
		pbus_data = gswPbusRead_unlock(pbus_addr,31);
		phy_acs_st = (pbus_data>>31) & 0x1;
	}

	if(0 != isIsr)
        spin_unlock(&phy_lock);
    else
	    spin_unlock_bh(&phy_lock);
	// 3. return data
	phy_data = pbus_data & 0xffff;
	return phy_data;
} /* end frank modify for rt62806 */


uint32 gswPmiWrite(uint32 phy_addr, uint32 phy_reg, uint32 phy_data)
{
	uint32 pbus_addr;
	uint32 pbus_data;
//	uint32 phy_acs_st;

	pbus_addr = 0x701c;
	// b31    - phy access 1:start&busy, 0:complete&idle
	// b29:25 - mdio phy reg addr
	// b24:20 - mdio phy addr
	// b19:18 - 2'b01: write, 2'b10: read
	// b17:16 - start field, always 2'b01
	// b15:0  - data

	phy_addr = phy_addr & 0x1f;
	phy_reg  = phy_reg & 0x1f;
	phy_data = phy_data & 0xffff;

	// 1. write phy_addr & phy_reg & phy_data
	pbus_data = 0x80050000; // write
	pbus_data = pbus_data | (phy_addr<<20);
	pbus_data = pbus_data | (phy_reg<<25);
	pbus_data = pbus_data | (phy_data);

	gswPbusWrite(pbus_addr,pbus_data);
//  DBG_PRINTF(" pbus write addr=0x%04x data=0x%08x\r\n", pbus_addr, pbus_data);

	return 0;
} /* end frank modify for rt62806 */

/* frank modify for  8 eth ports */
uint32
gswPbusRead_extend(uint32 pbus_addr, uint32 phy_addr)
{
	uint32 pbus_data = 0;

	uint32 reg;
	uint32 value;
	if(phy_addr == 31)
		pbus_data = gswPbusRead(pbus_addr);
	else if(phy_addr == 23){
		if(USE_MDIO1_FOR_EXTEND_SWITCH){
			/* EN7526G + MT7530,     EN7526 external switch p5 to MT7530 port5*/
			/* MT7530 to EN725G MDIO1*/
			pbus_data = gswPbusRead_by_mdio1(pbus_addr,phy_addr);
		}else{
			/* EN7580/En7527 + MT7530,     soc switch p5/p0 to MT7530 port5*/
			/* MT7530 to EN7580/EN7527 MDIO0*/
			pbus_data = gswPbusRead2(pbus_addr);
		}
	}

	return pbus_data;
} /* end frank modify for rt62806 */


/* frank modify for 8 eth ports */
int
gswPbusWrite_extend(uint32 pbus_addr, uint32 pbus_data, uint32 phy_addr)
{
	uint32 reg;
	uint32 value;

	if(phy_addr == 31)
		gswPbusWrite(pbus_addr, pbus_data);
	else if(phy_addr == 23)
		if(USE_MDIO1_FOR_EXTEND_SWITCH){
			/* EN7526G + MT7530,     EN7526 external switch p5 to MT7530 port5*/
			/* MT7530 to EN725G MDIO1*/
			gswPbusWrite_by_mdio1(pbus_addr, pbus_data, phy_addr);
		}else{
			/* EN7580/En7527 + MT7530,     soc switch p5/p0 to MT7530 port5*/
			/* MT7530 to EN7580/EN7527 MDIO0*/
			gswPbusWrite2(pbus_addr, pbus_data);
		}
		
  	return 0;
} 


EXPORT_SYMBOL(tcMiiStationRead);
EXPORT_SYMBOL(tcMiiStationWrite);



#ifdef TCSUPPORT_INIC_HOST
struct net_device *get_tc3262_gmac_dev()
{
	return lan_dev;
}
EXPORT_SYMBOL(get_tc3262_gmac_dev);
#endif

