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
#include <linux/module.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/delay.h>
#include "scu/ecnt_scu.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define TRNG_ADDR_BASE       0x00000000

/* M TRNG Register */
#define RNG_ADDR_RO_VERSION  (TRNG_ADDR_BASE + 0x000)
#define RNG_ADDR_RO_STATUS   (TRNG_ADDR_BASE + 0x004)
#define RNG_ADDR_WO_IRQ_CLR  (TRNG_ADDR_BASE + 0x008)
#define RNG_ADDR_RW_SWRST    (TRNG_ADDR_BASE + 0x010)
#define RNG_ADDR_RW_IRQ_CFG  (TRNG_ADDR_BASE + 0x014)
#define RNG_ADDR_RW_EN       (TRNG_ADDR_BASE + 0x020)
#define RNG_ADDR_RW_PRM      (TRNG_ADDR_BASE + 0x024)
#define RNG_ADDR_RW_HTEST    (TRNG_ADDR_BASE + 0x028)
#define RNG_ADDR_RW_DRBG     (TRNG_ADDR_BASE + 0x02C)
#define RNG_ADDR_RO_OUT      (TRNG_ADDR_BASE + 0x030)
#define RNG_ADDR_RO_SEED     (TRNG_ADDR_BASE + 0x034)
#define RNG_ADDR_RO_RAW      (TRNG_ADDR_BASE + 0x038)
#define RNG_ADDR_WO_EXT      (TRNG_ADDR_BASE + 0x03C)

#define M_RAW_NRBG_DRBG_INIT   0x00000111

/* m TRNG Register */
#define TRNG_IP_RDY			(TRNG_ADDR_BASE + 0x800)
#define NS_SEL_AND_DAT_EN	(TRNG_ADDR_BASE + 0x804)
#define HEALTH_TEST_SW_RST	(TRNG_ADDR_BASE + 0x808)
#define HEALTH_TEST_EN		(TRNG_ADDR_BASE + 0x80C)
#define	HEALTH_TEST_CUTOFF	(TRNG_ADDR_BASE + 0x810)
#define DRBG_OUT_FORBID		(TRNG_ADDR_BASE + 0x814)
#define INTR_EN				(TRNG_ADDR_BASE + 0x818)
#define CONTINUE_FAIL_CLR	(TRNG_ADDR_BASE + 0x81C)
#define TEST_CUTOFF_REAL	(TRNG_ADDR_BASE + 0x820)	
#define HEALTH_TEST_STATUS	(TRNG_ADDR_BASE + 0x824)
#define RAW_DATA_OUT		(TRNG_ADDR_BASE + 0x828)
#define NOISE_DATA_IN		(TRNG_ADDR_BASE + 0x82C)
#define SHA_TEST_MODE		(TRNG_ADDR_BASE + 0x830)
#define RESEED_COUNT		(TRNG_ADDR_BASE + 0x834)
#define TEST_MODE_SHA_DONE	(TRNG_ADDR_BASE + 0x838)
#define DRBG_DATA_OUT		(TRNG_ADDR_BASE + 0x83C)
#define DRBG_DEBUG_OUT_0	(TRNG_ADDR_BASE + 0x840)
#define DRBG_DEBUG_OUT_1	(TRNG_ADDR_BASE + 0x844)
#define DRBG_DEBUG_OUT_2	(TRNG_ADDR_BASE + 0x848)
#define DRBG_DEBUG_OUT_3	(TRNG_ADDR_BASE + 0x84C)
#define INTR_SOURCE			(TRNG_ADDR_BASE + 0xC00)

#define RAW_HW_INIT   0x80010002
#define DRBG_HW_INIT  0x80000002

/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_trng {
	struct device  *dev;
	void __iomem   *base;
	u32            irq;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_trng *ecnt_trng = NULL;
static const struct of_device_id ecnt_trng_of_id[] = {
    { .compatible = "econet,ecnt-trng"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_trng_of_id);

char g_MmSel = 'm';

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
extern void scu_Enable_Module_Clock (u8 module);

/************************************************************************
*                  S T A T I C   F U N C T I O N 
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
static u32 get_trng_data(u32 reg)
{
    return readl(ecnt_trng->base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_trng_data(u32 reg, u32 val)
{
    writel(val, ecnt_trng->base + reg); 
}

/* M TRNG API */
static u32 TRNG_GET_M_RNG_ADDR_RO_OUT(void)
{
    return get_trng_data(RNG_ADDR_RO_OUT);
}

static u32 TRNG_GET_M_RNG_ADDR_RO_STATUS(void)
{
    return get_trng_data(RNG_ADDR_RO_STATUS);
}

static void TRNG_SET_M_RNG_ADDR_RW_EN(u32 val)
{
    set_trng_data(RNG_ADDR_RW_EN, val);
    return;
}

static void TRNG_SET_M_RNG_ADDR_WO_IRQ_CLR(u32 val)
{
    set_trng_data(RNG_ADDR_WO_IRQ_CLR, val);
    return;
}

/* m TRNG API */
static u32 TRNG_GET_HEALTH_TEST_STATUS(void)
{
    return get_trng_data(HEALTH_TEST_STATUS);
}

static u32 TRNG_GET_RAW_DATA_OUT(void)
{
    return get_trng_data(RAW_DATA_OUT);
}

static u32 TRNG_GET_TEST_MODE_SHA_DONE(void)
{
    return get_trng_data(TEST_MODE_SHA_DONE);
}

static u32 TRNG_GET_DRBG_DATA_OUT(void)
{
    return get_trng_data(DRBG_DATA_OUT);
}

static u32 TRNG_GET_TRNG_IP_RDY(void)
{
    return get_trng_data(TRNG_IP_RDY);
}

static u32 TRNG_GET_INTR_SOURCE(void)
{
    return get_trng_data(INTR_SOURCE);
}

static void TRNG_SET_INTR_EN(u32 val)
{
    set_trng_data(INTR_EN, val);
    return;
}

static void TRNG_SET_CONTINUE_FAIL_CLR(u32 val)
{
    set_trng_data(CONTINUE_FAIL_CLR, val);
    return;
}

static void TRNG_SET_NS_SEL_AND_DAT_EN(u32 val)
{
    set_trng_data(NS_SEL_AND_DAT_EN, val);
    return;
}

/* Shared TRNG API of Mm */
static int is_drbg_data_ready(void)
{
    if('M' == g_MmSel){
        return ((TRNG_GET_M_RNG_ADDR_RO_STATUS() >> 4) & 0x1);
    }else{
        return ((TRNG_GET_TEST_MODE_SHA_DONE() >> 31) & 0x1);     
    }
}

static unsigned int get_drbg_data(void)
{
    if('M' == g_MmSel){
        return TRNG_GET_M_RNG_ADDR_RO_OUT();
    }else{
        return TRNG_GET_DRBG_DATA_OUT();
    }	
}

int ecnt_trng_get_data(uint8_t raw_drbg_sel, unsigned int* data)
{
    unsigned int count = 0;
    switch(raw_drbg_sel){
        case 0:
            if('M' == g_MmSel){
                printk("No support for raw data acuqision from M trng hardware module.\n");
                return -1;
            }		
            while( ((TRNG_GET_HEALTH_TEST_STATUS() >> 7) & 0x1) == 0x0){	
                /* go inside the while-loop if data is not ready */
                mdelay(1);
                if(++count >= 10){
                    printk("[Error] Failed to get RAW TRNG data !\n");
                    return -1;
                }
            }
            /* read data from RAW register & output using ASCII format */		
            *data = TRNG_GET_RAW_DATA_OUT();			    
            break;
        case 1:
            while(!is_drbg_data_ready()){
                mdelay(1);
                if(++count >= 10){
                    printk("[Error] Failed to get DRBG TRNG data !\n");
                    return -1;
                }                
            }
            *data = get_drbg_data();	            
            break;
        default:
            printk("[Warnning] No such choice!\n");
            break;
	}	

	return 0;
}
EXPORT_SYMBOL(ecnt_trng_get_data);

static void trng_raw_hw_init(void)
{
    if('M' == g_MmSel){
        printk("No support for raw data acuqision from M trng hardware module.\n");
        return -1;
    }
    /* Select noise source, enable raw data output, enable ring oscilator */
    /* RAW_HW_INIT: 0x80010002 (m)*/			
    TRNG_SET_NS_SEL_AND_DAT_EN(RAW_HW_INIT);	
}

static void trng_drbg_hw_init(void)
{
    if('M' == g_MmSel){
        TRNG_SET_M_RNG_ADDR_RW_EN(M_RAW_NRBG_DRBG_INIT);	
    }else{
        /* Select noise source, enable drbg data output, enable ring oscilator */
        /* DRBG_HW_INIT: 0x80000002 (m)*/				
        TRNG_SET_NS_SEL_AND_DAT_EN(DRBG_HW_INIT);
    }	
}

void hw_init_trng_module (void)
{
	/* Disable TRNG module in boot loader. Need to enable TRNG at insert TRNG module. */
	scu_Enable_Module_Clock(1);
}
EXPORT_SYMBOL(hw_init_trng_module);

static void check_trng_ip_status(void)
{
    unsigned int count = 0;

    if('M' == g_MmSel){
        /* No such register feature offered in M TRNG hardware module */
    }else{
        while(!(TRNG_GET_TRNG_IP_RDY() & 0x00000001)){		
            /* go inside the while-loop if TRNG IP is not ready */
            mdelay(1);
            if(++count >= 10){
                printk("[Error] TRNG IP is not ready !\n");
        	    return -1;
        	}		
        }
    }	
}

int trng_hw_init(uint8_t raw_drbg_sel, char MmSel)
{
    g_MmSel = MmSel;
    switch(raw_drbg_sel){
        case 0:
            trng_raw_hw_init();
            break;
        case 1:
            trng_drbg_hw_init();
            break;
        default:
            printk("[Warnning] No such hardware init choice!\n");
            break;
    }	

    check_trng_ip_status();

    return 0;
}
EXPORT_SYMBOL(trng_hw_init);

/* Interrupt service routine */
static irqreturn_t trng_interrupt_handler(int irq, void *dev_instance)
{
    if((TRNG_GET_INTR_SOURCE() & 0x1) == 0x1){
        printk("[m TRNG Interrupt]\n");
        /* Clear irq status */
        TRNG_SET_CONTINUE_FAIL_CLR(0x00000001);
        
        /* Global reset */
        SET_SCU_RST_RG((GET_SCU_RST_RG() | (1 << 12)));
        SET_SCU_RST_RG((GET_SCU_RST_RG() & (~(1 << 12))));		
    }else{
        printk("[M TRNG Interrupt]\n");	
        /* Clear irq status */		
        TRNG_SET_M_RNG_ADDR_WO_IRQ_CLR(0x00000001);		

        /* Global reset */
        SET_SCU_RST_RG((GET_SCU_RST_RG() | (1 << 11)));
        SET_SCU_RST_RG((GET_SCU_RST_RG() & (~(1 << 11))));		
    }
	
    return IRQ_HANDLED;
}

static int ecnt_trng_drv_probe(struct platform_device *pdev)
{	
    struct resource *res = NULL;
    int ret = 0;
	
    printk("[trng] ecnt_trng_drv_probe\n");
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No trng DT node found");
        return -EINVAL;
    }

    /* TRNG allocate memory */
    ecnt_trng = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_trng), GFP_KERNEL);
    if (!ecnt_trng)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_trng);
	
    /* get TRNG base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_trng->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_trng->base))
        return PTR_ERR(ecnt_trng->base); 

    ecnt_trng->dev = &pdev->dev;
    /* TRNG Interrupt Setting */
    ecnt_trng->irq = platform_get_irq(pdev, 0);
    if(ecnt_trng->irq <= 0) {
        printk("\n get pbus timeout irq number failed\n");
        return ecnt_trng->irq;
    }	
    ret = request_irq(ecnt_trng->irq, trng_interrupt_handler, 0, "TRNG_Data_Ready_ISR",
                       ecnt_trng->dev);
    if(ret) {
        printk("\n request_irq() (irq number: %d) failed (ret: %d)\n", ecnt_trng->irq, ret);
        return (ret);
    }
	
#if 0
    printk("[trng] res->name:%s\n", res->name);
    printk("[trng] res->start:0x%llx ===\n", res->start);
    printk("[trng] res->end:0x%llx ===\n", res->end);
    printk("[trng] ecnt_trng->base:0x%lx\n", (unsigned long)ecnt_trng->base);
    printk("[trng] ecnt_trng->irq:0x%lx\n", (unsigned long)ecnt_trng->irq);	
#endif
	
    return 0;
}

static int ecnt_trng_drv_remove(struct platform_device *pdev)
{
    printk("[trng] ecnt_trng_drv_remove\n");
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_trng_driver = {
    .probe = ecnt_trng_drv_probe,
    .remove = ecnt_trng_drv_remove,
    .driver = {
        .name = "ecnt-trng",
        .of_match_table = ecnt_trng_of_id
    },
};
module_platform_driver(ecnt_trng_driver);
MODULE_DESCRIPTION("EcoNet trng Driver");


