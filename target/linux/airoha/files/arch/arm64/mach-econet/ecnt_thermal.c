/***************************************************************
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
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <asm/tc3162/tc3162.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

#include <asm/io.h>
#include <modules/avs/avs.h>

/* #include <ecnt_hook/ecnt_hook_cpu_power.h> */

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define DUMMY_REG 0xf20 //PTPSPARE0 debug 0x1efb d000
#define DUMMY_REG_1 0xf24 //PTPSPARE1 debug 0x1efb d000
#define DUMMY_REG_2 0xf28 //PTPSPARE1 debug 0x1efb d000
#define INT_STATUS 0x810 //TEMPMONINTSTS
#define ptp_RightShift_bits 4	//according to 0x1efbd888 setting
#define iddq_high 100
#define iddq_low 30

/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_thermal_phy {
	struct device *dev;
	//void __iomem *thermal_base;
	void __iomem *ptp_base;
	u32 irq;

};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_thermal_phy *thermal_phy = NULL;

//struct tasklet_struct *tsklt;
struct work_struct powerworkq;

static u32 ptp_interrupt_status;

static const struct of_device_id ecnt_thermal_phy_of_id[] = {
    { .compatible = "econet,ecnt-thermal_phy"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_thermal_phy_of_id);
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/*****************************************************************
 ****  Reg   a c c e s s ********************************
 ******************************************************************/
// u32 get_efuse_data(u32 addr)
//{
//	return 0;
//}


u32 get_ptp_dummy(void)
{
	return readl(thermal_phy->ptp_base + (DUMMY_REG));
}

//use ptp dummy RG to store AVS ADC efuse value, and get value for AVS calculating voltage.
u32 get_ptp_dummy_AVS(void)
{	
return readl(thermal_phy->ptp_base + (DUMMY_REG_1));
}

//use ptp dummy RG to store iddq efuse value, and get the iddq value.
u32 get_ptp_dummy_iddq(void)
{	
	return readl(thermal_phy->ptp_base + (DUMMY_REG_2));
}
EXPORT_SYMBOL(get_ptp_dummy_iddq);

u32 get_ptp_interrupt_status(void)
{
	return readl(thermal_phy->ptp_base + (INT_STATUS));
}


void set_ptp_reg(u32 reg, u32 val)
{
	writel(val, (thermal_phy->ptp_base + (reg))); 
}

void set_ptp_high(u16 val)
{
	set_ptp_reg(0x824,  val>>ptp_RightShift_bits);			//High_temp_threshold
	set_ptp_reg(0x828, (val-0x10)>>ptp_RightShift_bits);	//High_temp_threshold, the higher value of 0x824/0x828  take effect
}

EXPORT_SYMBOL(set_ptp_high);

void set_ptp_low(u16 val)
{
	set_ptp_reg(0x82c,  val>>ptp_RightShift_bits);	//Back_normal_threshold
}

EXPORT_SYMBOL(set_ptp_low);

u32 get_ptp_pdid(void)
{
	return GET_PDIDR();
}
EXPORT_SYMBOL(get_ptp_pdid);

void set_ptp_dis(void)
{
	set_ptp_reg(0x800,  0x0);
}
EXPORT_SYMBOL(set_ptp_dis);


void ptp_init(u32 high_thd, u32 low_thd, u8 ptp_dis)
{
	if(GET_PDIDR()>1)
	{
		set_ptp_reg(0x804,  0x3ff);
		set_ptp_reg(0x808,  0x10001);
		set_ptp_reg(0x840,  0x1);
		set_ptp_reg(0x838,  0x6db);
		set_ptp_reg(0x814,  0x0);
		set_ptp_reg(0x818,  0x0);
		set_ptp_reg(0x81c,  0x0);
		set_ptp_reg(0x848,  0x0);
		set_ptp_reg(0x84c,  0x1);
		set_ptp_reg(0x850,  0x2);
		set_ptp_reg(0x8b4,  0x3);
		set_ptp_reg(0x860,  0x1);
		set_ptp_high(high_thd);
		set_ptp_low(low_thd);	//Back_normal_threshold ~110'C
		set_ptp_reg(0x864,  0x1efbdf2c);	
		set_ptp_reg(0x874,  0x1efbdf2c);	
		set_ptp_reg(0x878,  0x1efbd80c);	//valid addr
		set_ptp_reg(0x87c,  0x1fa202f0);	//ADC addr
		set_ptp_reg(0x880,  0x1);
		set_ptp_reg(0x884,  0x20);
		set_ptp_reg(0x888,  ptp_RightShift_bits);		//threhold right shift bits
		set_ptp_reg(0x88c,  0x1);
		set_ptp_reg(0x80c,  0x1);	//enable interrupt bit 0
		set_ptp_reg(0x800,  ~(ptp_dis)&0x1);
		set_ptp_reg(0x810,  0x1fffffff); //clear int status
	}
	printk(" ptp_init finished \n");
	
}
EXPORT_SYMBOL(ptp_init);



EXPORT_SYMBOL(get_ptp_dummy);

static void thermal_powerctrl(void)
{
	int iddq;
	static int hight_temp_flag = 0;
	
	if(((ptp_interrupt_status & 0x1)== 1) && (hight_temp_flag == 0)) {	//Temp too High
		
		//flag for high
		hight_temp_flag = 1;
		//set_ptp_reg(0x80c,  0x0);	//disable interrupt bit 0, 1, 4
		//clear int status
		//set_ptp_reg(0x810,  0x1fffffff);
		#if 0
		iddq = get_ptp_dummy_iddq();
        if(iddq > IDDQ_LIMIT)
		{
			//call HIGH temp action API
			TXPOWER_HOOK_CHECK_UP();
			AVS_Set(LV, iddq);
		}
		#endif
		printk(" High temp interrupt trigged, ADC code: %x \n", get_thermal_ADC());
		set_ptp_reg(0x80c,  0x13);	//enable interrupt bit 0, 1, 4
	} else if (((ptp_interrupt_status & 0x2)== 0x2) && (hight_temp_flag == 1)) { //Temp from high back to normal
		
		//clear hight_temp_flag
		hight_temp_flag = 0;
		//set_ptp_reg(0x80c,  0x0);	//disable interrupt bit 0, 1, 4
		//clear int status
		//set_ptp_reg(0x810,  0x1fffffff);
		#if 0
		iddq = get_ptp_dummy_iddq();

        if(iddq > IDDQ_LIMIT)
		{
			AVS_Set(NV, iddq);
			//call LOW temp action API
			TXPOWER_HOOK_CHECK_DOWN();
		}
		#endif
		printk(" Back to normal interrupt trigged, ADC code: %x \n", get_thermal_ADC());	
		set_ptp_reg(0x80c,  0x1);	//enable interrupt bit 0
	}
}

static irqreturn_t thermal_interrupt_handler(int irq, void *dev_id)
{

	ptp_interrupt_status = get_ptp_interrupt_status();
	set_ptp_reg(0x80c,  0x0);	//disable interrupt bit 0, 1, 4	
	//clear int status
	set_ptp_reg(0x810,  0x1fffffff);
	printk("ptp_interrupt_status = %u\n",ptp_interrupt_status); 
	//tasklet_schedule(tsklt);
	schedule_work(&powerworkq);
	//set_ptp_reg(0x80c,  0x13);	//enable interrupt bit 0, 1, 4
	return IRQ_HANDLED;
}

int ptp_register_interrupt(void)
{
	int ret = 0;

#ifdef TCSUPPORT_CPU_ARMV8
	ret = request_irq(thermal_phy->irq, thermal_interrupt_handler, 0,
	    "Thermal_PTP", thermal_phy->dev);
#else
	ret = request_irq(AUTO_MANUAL_INT, thermal_interrupt_handler, 0,
	    "Thermal_PTP", NULL);
#endif

	return ret;
}


/* APIs */

static int thermal_phy_drv_probe(struct platform_device *pdev)
{
	struct resource *res = NULL;
	int ret = 0;
	int irq;
	int iddq;
	
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No thermal_phy DT node found");
        return -EINVAL;
    }

    thermal_phy = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_thermal_phy), GFP_KERNEL);
    if (!thermal_phy)
        return -ENOMEM;

    platform_set_drvdata(pdev, thermal_phy);

    /* get uphy base address */
/*	
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    thermal_phy->thermal_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(thermal_phy->thermal_base))
        return PTR_ERR(thermal_phy->thermal_base);
*/
 res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    thermal_phy->ptp_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(thermal_phy->ptp_base))
        return PTR_ERR(thermal_phy->ptp_base);

    thermal_phy->dev = &pdev->dev;	
	
	if(isEN7523)
	{		
		printk("7523 Thermal Sensor address probe done_20220609\n");
	}
	
	if(GET_PDIDR()>1)
	{
		//set BG_CKEN for AVS DAC output stability.
		set_BG_CKEN();
		#if 0
		iddq = get_ptp_dummy_iddq();
		printk("%d\n", iddq);
	
		if(iddq > 0 && iddq < iddq_low)
		{
			AVS_Set(HV, iddq);
		}
		else
		#endif
		{
			AVS_Set(NV, iddq);
		}
	}
	#if 0
	/* get irq num */
	thermal_phy->irq = platform_get_irq(pdev, 0);
	printk("[thermal] thermal_phy->irq: %d\n", thermal_phy->irq);		

	//tsklt = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	//tasklet_init(tsklt, thermal_powerctrl, 0);
	INIT_WORK(&powerworkq, thermal_powerctrl);
	
	ret = devm_request_irq(&pdev->dev, thermal_phy->irq, thermal_interrupt_handler, 
	                           0, dev_name(&pdev->dev), thermal_phy);
	if(ret){
		dev_err(&(pdev->dev), "devm_request_irq failed with err %d\n", ret);
		return -EINVAL;
	}
	#endif
    return 0;
}


static int thermal_phy_drv_remove(struct platform_device *pdev)
{
	if(thermal_phy)  {
  		printk("thermal_phy_drv_remove\n");
	}
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver thermal_phy_driver = {
    .probe = thermal_phy_drv_probe,
    .remove = thermal_phy_drv_remove,
    .driver = {
	    .name = "ecnt-thermal_phy",
	    .of_match_table = ecnt_thermal_phy_of_id
    },
};

//builtin_platform_driver(thermal_phy_driver);
module_platform_driver(thermal_phy_driver); 

MODULE_DESCRIPTION("EcoNet thermal Driver");


