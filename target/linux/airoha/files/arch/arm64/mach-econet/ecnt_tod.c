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

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define TOD_ADDR_BASE              0x00000000

#define TOD_CTRL                  (TOD_ADDR_BASE + 0x0000)
#define NEW_TOD_SEC_H16           (TOD_ADDR_BASE + 0x0004)
#define NEW_TOD_SEC_L32           (TOD_ADDR_BASE + 0x0008)
#define NEW_TOD_NSEC32            (TOD_ADDR_BASE + 0x000c)
#define OFFSET_SEC_H16            (TOD_ADDR_BASE + 0x0010)
#define OFFSET_SEC_L32            (TOD_ADDR_BASE + 0x0014)
#define OFFSET_NSEC               (TOD_ADDR_BASE + 0x0018)
#define TOD_ADD_UNIT_INT          (TOD_ADDR_BASE + 0x0030)
#define TOD_ADD_UNIT_FRAC_NUME    (TOD_ADDR_BASE + 0x0034)
#define TOD_ADD_UNIT_FRAC_DENO    (TOD_ADDR_BASE + 0x0038)
#define FREQ_ADJ_FRAC_NUME        (TOD_ADDR_BASE + 0x0040)
#define FREQ_ADJ_FRAC_DENO        (TOD_ADDR_BASE + 0x0044)
#define FREQ_ADJ_CTRL             (TOD_ADDR_BASE + 0x0048)
#define TOD_1PPS_WD_CTRL          (TOD_ADDR_BASE + 0x0050)
#define CUR_SEC_H16               (TOD_ADDR_BASE + 0x0054)
#define CUR_SEC_L32               (TOD_ADDR_BASE + 0x0058)
#define CUR_NSEC32                (TOD_ADDR_BASE + 0x005c)
#define INT_ENABLE                (TOD_ADDR_BASE + 0x0060)
#define INT_STATUS                (TOD_ADDR_BASE + 0x0064)
#define DBG_PROBE_CTRL            (TOD_ADDR_BASE + 0x0074)
#define DBG_PROBE_HIGH32          (TOD_ADDR_BASE + 0x0078)
#define DBG_PROBE_LOW32           (TOD_ADDR_BASE + 0x007c)
#define SLV_SYNC_OFS0             (TOD_ADDR_BASE + 0x0080)
#define SLV_SYNC_OFS1             (TOD_ADDR_BASE + 0x0084)
#define SLV_SYNC_OFS2             (TOD_ADDR_BASE + 0x0088)
#define SLV_SYNC_OFS3             (TOD_ADDR_BASE + 0x008c)
#define SRAM_PWR_DOWN             (TOD_ADDR_BASE + 0x00c0)
#define MBIST_FSH_STS             (TOD_ADDR_BASE + 0x00c4)
#define MBIST_DELSEL_GO           (TOD_ADDR_BASE + 0x00c8)
#define MBIST_DONE_GO             (TOD_ADDR_BASE + 0x00cc)
#define MBIST_FAIL_GO             (TOD_ADDR_BASE + 0x00d0)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define TOD_BASE_OFFSET 0x2300

#define BASE_RANGE 0xD4

#define TOD_GEN_INT_MAX_NUM	(1)

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_tod_str {
	struct device *dev;
	void __iomem *tod_base;
	int tod_irq[TOD_GEN_INT_MAX_NUM];
};
typedef union
{
	struct
	{
		unsigned int load_new_tod   : 1;
		unsigned int tod_src_sel    : 1;
		unsigned int resv0          : 6;
		unsigned int adj_tod_ofs    : 1;
		unsigned int resv1          : 23;
	} bits;
	unsigned int word;
} tod_ctrl_t;


//0x2310
typedef union
{
	struct
	{
		unsigned int tod_ofs_sec_h16 : 16;
		unsigned int resv            : 15;
		unsigned int tod_ofs_is_neg  : 1;
	} bits;
	unsigned int word;
} offset_sec_h16_t;


//0x2330
typedef union
{
	struct
	{
		unsigned int tod_add_unit_int    : 8;
		unsigned int resv0               : 24;
	} bits;
	unsigned int word;
} tod_add_unit_int_t;

//0x2348
typedef union
{
	struct
	{
		unsigned int freq_adj_restart   : 1;
		unsigned int freq_adj_mode      : 1;
		unsigned int resv0              : 30;
	} bits;
	unsigned int word;
} freq_adj_ctrl_t;

//0x2360
typedef union
{
	struct
	{
		unsigned int tod_load_done_int_en   : 1;
		unsigned int tod_1pps_int_en        : 1;
		unsigned int freq_adj_done_int_en   : 1;
		unsigned int resv0                  : 29;
	} bits;
	unsigned int word;
}tod_int_enable_t;

//0x2364
typedef union
{
	struct
	{
		unsigned int tod_load_done_int   : 1;
		unsigned int tod_1pps_int        : 1;
		unsigned int freq_adj_done_int   : 1;
		unsigned int resv0               : 29;
	} bits;
	unsigned int word;
}tod_int_status_t;

//0x2380
typedef union
{
	struct
	{
		unsigned int slv_rx_sync_ofs   : 15;
		unsigned int is_rx_negative    :  1;
		unsigned int slv_tx_sync_ofs   : 15;
		unsigned int is_tx_negative    :  1;
	} bits;
	unsigned int word;
}slv_sync_ofs_t;



/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_tod_str *ecnt_tod = NULL;

static const struct of_device_id ecnt_tod_of_id[] = {
    { .compatible = "econet,ecnt-tod"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_tod_of_id);
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/*****************************************************************
 ****  N P    S C U    a c c e s s ********************************
 ******************************************************************/
u32 get_tod_data(u32 reg)
{
	if( (TOD_BASE_OFFSET <= reg) && (reg < (TOD_BASE_OFFSET+BASE_RANGE)) )
	{
		reg = reg - TOD_BASE_OFFSET;
		return readl(ecnt_tod->tod_base + reg);		
	}
	else
	{
		printk("\ntod(%s) (%d)get reg error, reg=0x%08X\n", __func__,__LINE__, reg);
		return -1;
	}
}

EXPORT_SYMBOL(get_tod_data);


void set_tod_data(u32 reg, u32 val)
{
	if( (TOD_BASE_OFFSET <= reg) && (reg < (TOD_BASE_OFFSET+BASE_RANGE)) )
	{
		reg = reg - TOD_BASE_OFFSET;
		writel(val, ecnt_tod->tod_base + reg); 
	}
	else
	{
		printk("\ntod(%s) (%d)set reg error, reg=0x%08X\n", __func__,__LINE__, reg);
		return;
	}
}

EXPORT_SYMBOL(set_tod_data);


/* don't EXPORT this function. Create API for your purpose instead. */
static u32 ecnt_get_tod_data(u32 reg)
{
    return readl(ecnt_tod->tod_base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void ecnt_set_tod_data(u32 reg, u32 val)
{
    writel(val, ecnt_tod->tod_base + reg);
}

/* [For GSW] */
void set_slv0_sync_offset_via_tod(uint8_t is_negative, uint16_t offset)
{
	slv_sync_ofs_t slv_sync_ofs0;

	slv_sync_ofs0.word = ecnt_get_tod_data(SLV_SYNC_OFS0);
	slv_sync_ofs0.bits.slv_tx_sync_ofs = offset;
	slv_sync_ofs0.bits.slv_rx_sync_ofs = offset;

	slv_sync_ofs0.bits.is_rx_negative = is_negative;
	slv_sync_ofs0.bits.is_tx_negative = is_negative;

	/* Configure ToD SLV_SYNC_OFFSET */
	ecnt_set_tod_data(SLV_SYNC_OFS0, slv_sync_ofs0.word);

}
EXPORT_SYMBOL(set_slv0_sync_offset_via_tod);

/* [For XFI ETH serdes] */
void set_slv1_sync_offset_via_tod(uint16_t offset)
{
	slv_sync_ofs_t slv_sync_ofs1;

	slv_sync_ofs1.word = ecnt_get_tod_data(SLV_SYNC_OFS1);
	slv_sync_ofs1.bits.slv_tx_sync_ofs = offset;
	slv_sync_ofs1.bits.slv_rx_sync_ofs = offset;

	/* Configure ToD SLV_SYNC_OFFSET */
	ecnt_set_tod_data(SLV_SYNC_OFS1, slv_sync_ofs1.word);

}
EXPORT_SYMBOL(set_slv1_sync_offset_via_tod);

/* [For XFI PON serdes] */
void set_slv2_sync_offset_via_tod(uint16_t offset)
{
	slv_sync_ofs_t slv_sync_ofs2;

	slv_sync_ofs2.word = ecnt_get_tod_data(SLV_SYNC_OFS2);
	slv_sync_ofs2.bits.slv_tx_sync_ofs = offset;
	slv_sync_ofs2.bits.slv_rx_sync_ofs = offset;

	/* Configure ToD SLV_SYNC_OFFSET */
	ecnt_set_tod_data(SLV_SYNC_OFS2, slv_sync_ofs2.word);

}
EXPORT_SYMBOL(set_slv2_sync_offset_via_tod);


void set_tod_1pps_width(uint32_t tod_1pps_width)
{
	tod_ctrl_t tod_ctrl;

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);

	tod_ctrl.bits.load_new_tod = 1;
	tod_ctrl.bits.tod_src_sel = 1;

	/* Configure ToD 1pps width */
	ecnt_set_tod_data(TOD_1PPS_WD_CTRL, tod_1pps_width);

	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);
}
EXPORT_SYMBOL(set_tod_1pps_width);


void set_tod_add_unit(uint32_t *tod_add_unit)
{
	tod_ctrl_t tod_ctrl;

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);

	tod_ctrl.bits.load_new_tod = 1;
	tod_ctrl.bits.tod_src_sel = 1;

	/* Configure ToD clock unit */
	ecnt_set_tod_data(TOD_ADD_UNIT_INT,       tod_add_unit[0]);
	ecnt_set_tod_data(TOD_ADD_UNIT_FRAC_NUME, tod_add_unit[1]);
	ecnt_set_tod_data(TOD_ADD_UNIT_FRAC_DENO, tod_add_unit[2]);

	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);
}
EXPORT_SYMBOL(set_tod_add_unit);


void set_freq_via_tod(uint32_t *tod_freq)
{
	freq_adj_ctrl_t freq_adj_ctrl;

	freq_adj_ctrl.word = ecnt_get_tod_data(FREQ_ADJ_CTRL);
	freq_adj_ctrl.bits.freq_adj_restart = 0x1;

	/* Configure ToD clock frequency */
	ecnt_set_tod_data(FREQ_ADJ_FRAC_NUME, tod_freq[0]);
	ecnt_set_tod_data(FREQ_ADJ_FRAC_DENO, tod_freq[1]);
	ecnt_set_tod_data(FREQ_ADJ_CTRL, freq_adj_ctrl.word);
}
EXPORT_SYMBOL(set_freq_via_tod);

void set_offset_via_tod(uint8_t is_negative, uint8_t *offset)
{
	tod_ctrl_t tod_ctrl;
	offset_sec_h16_t offset_sec_h16;

	/* Configure ToD timestamp offset */
	offset_sec_h16.word = ecnt_get_tod_data(OFFSET_SEC_H16);
	offset_sec_h16.bits.tod_ofs_is_neg = is_negative;
	offset_sec_h16.bits.tod_ofs_sec_h16 = ntohs(*(uint16_t *)&offset[0]);
	ecnt_set_tod_data(OFFSET_SEC_H16, offset_sec_h16.word);
	ecnt_set_tod_data(OFFSET_SEC_L32, ntohl(*(uint32_t *)&offset[2]));
	ecnt_set_tod_data(OFFSET_NSEC, ntohl(*(uint32_t *)&offset[6]));

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);
	tod_ctrl.bits.adj_tod_ofs = 0x1;
	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);
}
EXPORT_SYMBOL(set_offset_via_tod);


void set_fixed_timestamp_via_tod(u8 *fixed_tstamp)
{
	tod_ctrl_t tod_ctrl;

	ecnt_set_tod_data(TOD_ADD_UNIT_INT, 0x0);

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);
	tod_ctrl.bits.tod_src_sel = 0x1;
	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);

	/* configure constant timestamp value */
	ecnt_set_tod_data(NEW_TOD_SEC_H16, ntohs(*(uint16_t *)&fixed_tstamp[0]));
	ecnt_set_tod_data(NEW_TOD_SEC_L32, ntohl(*(uint32_t *)&fixed_tstamp[2]));
	ecnt_set_tod_data(NEW_TOD_NSEC32, ntohl(*(uint32_t *)&fixed_tstamp[6]));

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);
	tod_ctrl.bits.load_new_tod = 0x1;
	tod_ctrl.bits.tod_src_sel  = 0x1;
	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);
}
EXPORT_SYMBOL(set_fixed_timestamp_via_tod);

void set_offset_time_to_tod(uint8_t is_negative, uint8_t *offset)
{
	tod_ctrl_t tod_ctrl;
	offset_sec_h16_t offset_sec_h16;

	/* Configure ToD timestamp offset */
	offset_sec_h16.word = ecnt_get_tod_data(OFFSET_SEC_H16);
	offset_sec_h16.bits.tod_ofs_is_neg = is_negative;
	offset_sec_h16.bits.tod_ofs_sec_h16 = (*(uint16_t *)&offset[0]);
	ecnt_set_tod_data(OFFSET_SEC_H16, offset_sec_h16.word);
	ecnt_set_tod_data(OFFSET_SEC_L32, (*(uint32_t *)&offset[2]));
	ecnt_set_tod_data(OFFSET_NSEC, (*(uint32_t *)&offset[6]));

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);
	tod_ctrl.bits.adj_tod_ofs = 0x1;
	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);
}
EXPORT_SYMBOL(set_offset_time_to_tod);

int set_timestamp_to_tod(u32 sec, u32 nsec)
{
	tod_ctrl_t tod_ctrl;

	ecnt_set_tod_data(NEW_TOD_SEC_H16, 0x0);
	ecnt_set_tod_data(NEW_TOD_SEC_L32, sec);
	ecnt_set_tod_data(NEW_TOD_NSEC32, nsec);

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);
	printk("tod_ctrl.word = 0x%x\n", tod_ctrl.word);
	tod_ctrl.bits.load_new_tod = 0x1;
	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);

	return 0;
}
EXPORT_SYMBOL(set_timestamp_to_tod);

int get_timestamp_via_tod(void)
{
	int ret = 0;
	uint32_t sec_h, sec_l, nsec;

	sec_h = ecnt_get_tod_data(CUR_SEC_H16);
	sec_l = ecnt_get_tod_data(CUR_SEC_L32);
	nsec = ecnt_get_tod_data(CUR_NSEC32);

	printk("%s().%d %x.%x.%x\n", __func__, __LINE__, sec_h, sec_l, nsec);

	return ret;
}
EXPORT_SYMBOL(get_timestamp_via_tod);

void tod_init_time(void)
{
	tod_ctrl_t tod_ctrl;

	ecnt_set_tod_data(OFFSET_SEC_H16, 0x0);
	ecnt_set_tod_data(OFFSET_SEC_L32, 0x5FEDF580);
	//ecnt_set_tod_data(OFFSET_SEC_L32, 0x61CF2900);
	ecnt_set_tod_data(OFFSET_NSEC, 0x0);

	tod_ctrl.word = ecnt_get_tod_data(TOD_CTRL);
	tod_ctrl.bits.adj_tod_ofs = 0x1;
	ecnt_set_tod_data(TOD_CTRL, tod_ctrl.word);
}
EXPORT_SYMBOL(tod_init_time);

void (*ecnt_tod_isr_func)(uint32_t int_status);

/* Interrupt service routine */
static irqreturn_t tod_interrupt_handler(int irq, void *dev_instance)
{
	tod_int_status_t tod_int_status;

	tod_int_status.word = ecnt_get_tod_data(INT_STATUS);

	if(tod_int_status.bits.tod_load_done_int){
		printk("\n[ISR] Type: TOD_LOAD_DONE_INT\n");
	}
	if(tod_int_status.bits.tod_1pps_int){
		printk("\n[ISR] Type: TOD_1PPS_INT\n");
	}
	if(tod_int_status.bits.freq_adj_done_int){
		printk("\n[ISR] Type: FREQ_ADJ_DONE_INT\n");
	}

	printk("\n[tod_interrupt_handler] tod_int_status.word : 0x%08x\n", tod_int_status.word);
	if(ecnt_tod_isr_func != NULL)
		ecnt_tod_isr_func(tod_int_status.word);

	ecnt_set_tod_data(INT_STATUS, tod_int_status.word);

	return IRQ_HANDLED;
}


void tod_interrupt_init(void (*isr_func)(uint32_t))
{
	if(isr_func != NULL)
	    ecnt_tod_isr_func = isr_func;
}
EXPORT_SYMBOL(tod_interrupt_init);


void tod_interrupt_enable(uint8_t bit)
{
	tod_int_enable_t tod_int_enable;
	tod_int_enable.word = ecnt_get_tod_data(INT_ENABLE);
	tod_int_enable.word |= (1 << bit);
	ecnt_set_tod_data(INT_ENABLE, tod_int_enable.word);
}
EXPORT_SYMBOL(tod_interrupt_enable);


void tod_interrupt_disable(uint8_t bit)
{
	tod_int_enable_t tod_int_enable;
	tod_int_enable.word = ecnt_get_tod_data(INT_ENABLE);
	tod_int_enable.word &= ~((1 << bit));
	ecnt_set_tod_data(INT_ENABLE, tod_int_enable.word);
}
EXPORT_SYMBOL(tod_interrupt_disable);


void show_tod_register(void)
{

	printk("tod_ctrl                 : 0x%08x \n", ecnt_get_tod_data(TOD_CTRL));
	printk("new_tod_sec_h16          : 0x%08x \n", ecnt_get_tod_data(NEW_TOD_SEC_H16));
	printk("new_tod_sec_l32          : 0x%08x \n", ecnt_get_tod_data(NEW_TOD_SEC_L32));
	printk("new_tod_nsec32           : 0x%08x \n", ecnt_get_tod_data(NEW_TOD_NSEC32));
	printk("\n");

	printk("offset_sec_h16           : 0x%08x \n", ecnt_get_tod_data(OFFSET_SEC_H16));
	printk("offset_sec_l32           : 0x%08x \n", ecnt_get_tod_data(OFFSET_SEC_L32));
	printk("offset_nsec              : 0x%08x \n", ecnt_get_tod_data(OFFSET_NSEC));
	printk("\n");

	printk("tod_add_unit_int         : 0x%08x \n", ecnt_get_tod_data(TOD_ADD_UNIT_INT));
	printk("tod_add_unit_frac_nume   : 0x%08x \n", ecnt_get_tod_data(TOD_ADD_UNIT_FRAC_NUME));
	printk("tod_add_unit_frac_deno   : 0x%08x \n", ecnt_get_tod_data(TOD_ADD_UNIT_FRAC_DENO));
	printk("\n");

	printk("freq_adj_frac_nume       : 0x%08x \n", ecnt_get_tod_data(FREQ_ADJ_FRAC_NUME));
	printk("freq_adj_frac_deno       : 0x%08x \n", ecnt_get_tod_data(FREQ_ADJ_FRAC_DENO));
	printk("freq_adj_ctrl            : 0x%08x \n", ecnt_get_tod_data(FREQ_ADJ_CTRL));
	printk("\n");

	printk("tod_1pps_wd_ctrl         : 0x%08x \n", ecnt_get_tod_data(TOD_1PPS_WD_CTRL));
	printk("cur_sec_h16              : 0x%08x \n", ecnt_get_tod_data(CUR_SEC_H16));
	printk("cur_sec_l32              : 0x%08x \n", ecnt_get_tod_data(CUR_SEC_L32));
	printk("cur_nsec32               : 0x%08x \n", ecnt_get_tod_data(CUR_NSEC32));
	printk("\n");

	printk("int_enable               : 0x%08x \n", ecnt_get_tod_data(INT_ENABLE));
	printk("int_status               : 0x%08x \n", ecnt_get_tod_data(INT_STATUS));
	printk("\n");

	printk("slv_sync_ofs0            : 0x%08x \n", ecnt_get_tod_data(SLV_SYNC_OFS0));
	printk("slv_sync_ofs1            : 0x%08x \n", ecnt_get_tod_data(SLV_SYNC_OFS1));
	printk("slv_sync_ofs2            : 0x%08x \n", ecnt_get_tod_data(SLV_SYNC_OFS2));
	printk("slv_sync_ofs3            : 0x%08x \n", ecnt_get_tod_data(SLV_SYNC_OFS3));
	printk("\n");
}
EXPORT_SYMBOL(show_tod_register);

void tod_module_init(void)
{
	uint32_t init_tod[3] = {5, 0, 1}; /* ToD clock:400M integer, numberator, denominator */

	set_tod_add_unit(init_tod);
	tod_init_time();
}
EXPORT_SYMBOL(tod_module_init);


static int ecnt_tod_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
	int irq_idx = 0;
	int irq = -1, ret = 0;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No TOD GEN DT node found");
        return -EINVAL;
    }

    ecnt_tod = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_tod_str), GFP_KERNEL);
    if (!ecnt_tod)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_tod);

    /* get TOD GEN base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_tod->tod_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_tod->tod_base))
        return PTR_ERR(ecnt_tod->tod_base);

    ecnt_tod->dev = &pdev->dev;

	/* ToD interrupt setting */
	for( irq_idx = 0; irq_idx < TOD_GEN_INT_MAX_NUM; irq_idx++ ){
		irq = platform_get_irq(pdev, irq_idx);
		if (irq <= 0)
			return irq;
		ecnt_tod->tod_irq[irq_idx] = irq;
		//debug by da
		printk("irq-%d: %d ", irq_idx, irq);

		ret = request_irq(ecnt_tod->tod_irq[irq_idx], tod_interrupt_handler, 0, "ToD_ISR",
						ecnt_tod->dev);
		if(ret) {
			printk("\n request_irq() (irq number: %d) failed (ret: %d)\n", ecnt_tod->tod_irq[irq_idx], ret);
			return (ret);
		}
	}
#if 0
	printk("[tod] res->name:%s\n", res->name);
	printk("[tod] res->start:0x%llx ===\n", res->start);
	printk("[tod] res->end:0x%llx ===\n", res->end);
	printk("[tod] ecnt_tod->base:0x%lx\n", (unsigned long)ecnt_tod->base);
#endif

    return 0;
}

int get_tod_irq(int index)
{
	return ecnt_tod->tod_irq[index];
}

EXPORT_SYMBOL(get_tod_irq);

struct device* get_tod_dev(void)
{
    if ((ecnt_tod) && (ecnt_tod->dev))
        return ecnt_tod->dev;
    else
        return NULL;
}
EXPORT_SYMBOL(get_tod_dev);

int tod_current_time(u64 *sec, u32 *nsec)
{
	int ret = 0;
	u64 tod_sec_h = 0;
	u64 tod_sec_l = 0;
	u32 tod_nsec = 0;

	tod_sec_h = ecnt_get_tod_data(CUR_SEC_H16);
	tod_sec_l = ecnt_get_tod_data(CUR_SEC_L32);
	tod_nsec = ecnt_get_tod_data(CUR_NSEC32);

	*sec = (tod_sec_h << 32 | tod_sec_l);
	*nsec = tod_nsec;

	return 0;
}
EXPORT_SYMBOL(tod_current_time);

static int ecnt_tod_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_tod_driver = {
    .probe = ecnt_tod_drv_probe,
    .remove = ecnt_tod_drv_remove,
    .driver = {
	    .name = "ecnt-tod",
	    .of_match_table = ecnt_tod_of_id
    },
};
module_platform_driver(ecnt_tod_driver);


MODULE_DESCRIPTION("EcoNet TOD GEN Driver");

