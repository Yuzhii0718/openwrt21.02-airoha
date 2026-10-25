/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°Airoha Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to Airoha Limited (¡°Airoha¡±) and/or 
its licensors. Without the prior written permission of Airoha and/or its licensors, 
any reproduction, modification, use or disclosure of Airoha Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

Airoha Limited. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°AIROHA SOFTWARE¡±) RECEIVED FROM AIROHA 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES AIROHA PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE AIROHA SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN AIROHA SOFTWARE.

AIROHA SHALL NOT BE RESPONSIBLE FOR ANY AIROHA SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
AIROHA'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AIROHA 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT AIROHA'S SOLE OPTION, TO 
REVISE OR REPLACE THE AIROHA SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO AIROHA FOR SUCH 
AIROHA SOFTWARE.
***************************************************************/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include <asm/io.h>

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <asm/tc3162/tc3162.h>
#include <linux/irqreturn.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <bus_mon/bus_monitor.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define PBUS_MONITOR_BASE					(0x000000)  /*phys:0x1fbe3400*/

/*===========for PCIe begin============================================ */
#define PBUS_PCIE0_MEM_BASE				(PBUS_MONITOR_BASE+0x00)
#define PBUS_PCIE0_MEM_MASK				(PBUS_MONITOR_BASE+0x04)
#define PBUS_PCIE1_MEM_BASE				(PBUS_MONITOR_BASE+0x08)
#define PBUS_PCIE1_MEM_MASK				(PBUS_MONITOR_BASE+0x0C)
#define PBUS_PCIE2_MEM_BASE				(PBUS_MONITOR_BASE+0x10)
#define PBUS_PCIE2_MEM_MASK				(PBUS_MONITOR_BASE+0x14)
/*===========for PCIe end============================================ */

#define CR_PCIE_SINGLE_EN               (PBUS_MONITOR_BASE+0x18)
#define CR_PBUS_PROBE_SEL               (PBUS_MONITOR_BASE+0x1c)
#define CR_PBUS_DBG_CNT_CLR             (PBUS_MONITOR_BASE+0x20)
#define CR_PBUS_DBG_CNT_0               (PBUS_MONITOR_BASE+0x24) /* cpu pre_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_1               (PBUS_MONITOR_BASE+0x28) /* cpu post_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_2               (PBUS_MONITOR_BASE+0x2c) /* 16p_arb pre_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_3               (PBUS_MONITOR_BASE+0x30) /* rbus_decoder bypass_cnt for pcie0 */
#define CR_PBUS_DBG_CNT_4               (PBUS_MONITOR_BASE+0x34) /* rbus_decoder block_cnt for pcie0 */
#define CR_PBUS_DBG_CNT_5               (PBUS_MONITOR_BASE+0x38) /* pcie0 post_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_6               (PBUS_MONITOR_BASE+0x3c) /* pcie0 pre_burst_cmd_cnt */
#define CR_PBUS_DBG_CNT_7               (PBUS_MONITOR_BASE+0x40) /* pcie0 post_burst_cmd_cnt */
#define CR_PBUS_DBG_CNT_8               (PBUS_MONITOR_BASE+0x44) /* pcie1 post_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_9               (PBUS_MONITOR_BASE+0x48) /* pcie1 pre_burst_cmd_cnt */
#define CR_PBUS_DBG_CNT_10              (PBUS_MONITOR_BASE+0x4c) /* pcie1 post_burst_cmd_cnt */
#define CR_PBUS_DBG_CNT_11              (PBUS_MONITOR_BASE+0x50) /* pcie2 post_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_12              (PBUS_MONITOR_BASE+0x54) /* pcie2 pre_burst_cmd_cnt */
#define CR_PBUS_DBG_CNT_13              (PBUS_MONITOR_BASE+0x58) /* pcie2 post_burst_cmd_cnt */
#define CR_PBUS_DBG_CNT_14              (PBUS_MONITOR_BASE+0x5c) /* rbus_decoder bypass_cnt for pcie1 */
#define CR_PBUS_DBG_CNT_15              (PBUS_MONITOR_BASE+0x60) /* rbus_decoder block_cnt for pcie1 */
#define CR_PBUS_DBG_CNT_16              (PBUS_MONITOR_BASE+0x64) /* rbus_decoder bypass_cnt for pcie2 */
#define CR_PBUS_DBG_CNT_17              (PBUS_MONITOR_BASE+0x68) /* rbus_decoder block_cnt for pcie2 */
#define CR_PBUS_DBG_CNT_18              (PBUS_MONITOR_BASE+0x84) /* pcie0 pre_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_19              (PBUS_MONITOR_BASE+0x88) /* pcie1 pre_multi_issue_cnt */
#define CR_PBUS_DBG_CNT_20              (PBUS_MONITOR_BASE+0x8C) /* pcie2 pre_multi_issue_cnt */

#define PBUS_MON_GLO_CFG				(PBUS_MONITOR_BASE+0x90)
#define PBUS_MON_0_ADDR_MAX				(PBUS_MONITOR_BASE+0x94)
#define PBUS_MON_0_ADDR_MIN				(PBUS_MONITOR_BASE+0x98)
#define PBUS_MON_0_ACC_CNT_L			(PBUS_MONITOR_BASE+0x9C)
#define PBUS_MON_0_ACC_CNT_H			(PBUS_MONITOR_BASE+0xA0)
#define PBUS_MON_0_ACC_PRD_L			(PBUS_MONITOR_BASE+0xA4)
#define PBUS_MON_0_ACC_PRD_H			(PBUS_MONITOR_BASE+0xA8)
#define PBUS_MON_0_ACC_MAX				(PBUS_MONITOR_BASE+0xAC)
#define PBUS_MON_0_ACC_TOTAL_L			(PBUS_MONITOR_BASE+0xB0)
#define PBUS_MON_0_ACC_TOTAL_H			(PBUS_MONITOR_BASE+0xB4)

#define MON_IDX_OFFSET					(0x24)
#define MON_IDX_BIT_OFFSET				(0x4)
#define MON_WR_BIT_OFFSET				(0x1)

#define MON_EN_BIT_OFFSET				(0x4)
#define MON_CLR_BIT_OFFSET				(0x2)
#define PMON_ENTRY_SIZE					(0x8)

#define PMON_MAX_NUM 					(0x3)
#define PMON_IDX_VAL					(0x7) /* (0x1 | 0x2 | 0x4) */



/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

struct ecnt_pbus_monitor {
	struct device *dev;
	void __iomem *base;
	u32 irq;

};


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_pbus_monitor *ecnt_pbus_monitor = NULL;


static const struct of_device_id ecnt_pbus_monitor_of_id[] = {
    { .compatible = "econet,ecnt-pbus_monitor"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_pbus_monitor_of_id);



/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
static u32 get_pbus_monitor_data(u32 reg)
{
    return readl(ecnt_pbus_monitor->base + reg);
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_pbus_monitor_data(u32 reg, u32 val)
{
    writel(val, ecnt_pbus_monitor->base + reg); 
}

/*===========for PCIe begin============================================ */
/*======================PCIe0========================================== */
u32 GET_PBUS_PCIE0_BASE(void)
{
	return get_pbus_monitor_data(PBUS_PCIE0_MEM_BASE);
}

void SET_PBUS_PCIE0_BASE(u32 val)
{
	set_pbus_monitor_data(PBUS_PCIE0_MEM_BASE, val);
}

u32 GET_PBUS_PCIE0_MASK(void)
{
	return get_pbus_monitor_data(PBUS_PCIE0_MEM_MASK);
}

void SET_PBUS_PCIE0_MASK(u32 val)
{
	set_pbus_monitor_data(PBUS_PCIE0_MEM_MASK, val);
}

/*======================PCIe1========================================== */
u32 GET_PBUS_PCIE1_BASE(void)
{
	return get_pbus_monitor_data(PBUS_PCIE1_MEM_BASE);
}

void SET_PBUS_PCIE1_BASE(u32 val)
{
	set_pbus_monitor_data(PBUS_PCIE1_MEM_BASE, val);
}

u32 GET_PBUS_PCIE1_MASK(void)
{
	return get_pbus_monitor_data(PBUS_PCIE1_MEM_MASK);
}

void SET_PBUS_PCIE1_MASK(u32 val)
{
	set_pbus_monitor_data(PBUS_PCIE1_MEM_MASK, val);
}

/*======================PCIe2========================================== */
u32 GET_PBUS_PCIE2_BASE(void)
{
	return get_pbus_monitor_data(PBUS_PCIE2_MEM_BASE);
}

void SET_PBUS_PCIE2_BASE(u32 val)
{
	set_pbus_monitor_data(PBUS_PCIE2_MEM_BASE, val);
}

u32 GET_PBUS_PCIE2_MASK(void)
{
	return get_pbus_monitor_data(PBUS_PCIE2_MEM_MASK);
}

void SET_PBUS_PCIE2_MASK(u32 val)
{
	set_pbus_monitor_data(PBUS_PCIE2_MEM_MASK, val);
}
EXPORT_SYMBOL(GET_PBUS_PCIE0_BASE);
EXPORT_SYMBOL(SET_PBUS_PCIE0_BASE);
EXPORT_SYMBOL(GET_PBUS_PCIE0_MASK);
EXPORT_SYMBOL(SET_PBUS_PCIE0_MASK);
EXPORT_SYMBOL(GET_PBUS_PCIE1_BASE);
EXPORT_SYMBOL(SET_PBUS_PCIE1_BASE);
EXPORT_SYMBOL(GET_PBUS_PCIE1_MASK);
EXPORT_SYMBOL(SET_PBUS_PCIE1_MASK);
EXPORT_SYMBOL(GET_PBUS_PCIE2_BASE);
EXPORT_SYMBOL(SET_PBUS_PCIE2_BASE);
EXPORT_SYMBOL(GET_PBUS_PCIE2_MASK);
EXPORT_SYMBOL(SET_PBUS_PCIE2_MASK);
/*===========for PCIe end============================================ */
int pbus_mon_set(pbus_mon_t *mon_in)
{
	unsigned int pbus_cfg = 0;
    unsigned int shift;

	/* Check if input data is valid*/
	if (mon_in->mon_idx > PMON_IDX_VAL)
	{
		printk("[%s] Wrong idx input !\n",__FUNCTION__);
		return -1;
	}

	if ((mon_in->addr_max == 0)||(mon_in->addr_min ==0) || (mon_in->addr_min > mon_in->addr_max))
	{
		printk("[%s] Addr_max/min set error\n",__FUNCTION__);
		return -1;
	}

	pbus_cfg = get_pbus_monitor_data(PBUS_MON_GLO_CFG);

    shift = (MON_IDX_BIT_OFFSET + (mon_in->mon_idx * PMON_ENTRY_SIZE));

    if (shift<=28) {

        pbus_cfg &= ~(0xf<< shift);

        pbus_cfg |= (mon_in->master_id) <<shift;

    }

    shift = (MON_WR_BIT_OFFSET + (mon_in->mon_idx* PMON_ENTRY_SIZE));

    if (shift<=31) {

    	pbus_cfg &= ~(0x1<< shift);

    	pbus_cfg |= mon_in->wr_sel << shift;

    }

	set_pbus_monitor_data(PBUS_MON_0_ADDR_MAX + (MON_IDX_OFFSET * mon_in->mon_idx), mon_in->addr_max);

	set_pbus_monitor_data(PBUS_MON_0_ADDR_MIN + (MON_IDX_OFFSET * mon_in->mon_idx), mon_in->addr_min);
	
	set_pbus_monitor_data(PBUS_MON_GLO_CFG, pbus_cfg);

	printk("[%s] set monitor %x range in 0x%x ~ 0x%x\n",__FUNCTION__,mon_in->mon_idx, mon_in->addr_min, mon_in->addr_max);
	
	return 0;

}
EXPORT_SYMBOL(pbus_mon_set);
u32 pbus_mon_get_cfg(void)
{
	return get_pbus_monitor_data(PBUS_MON_GLO_CFG);
}
EXPORT_SYMBOL(pbus_mon_get_cfg);

int pbus_mon_get(pbus_mon_t *mon_out)
{

	unsigned int pbus_cfg = 0;
	if (mon_out->mon_idx > PMON_MAX_NUM)
	{
		printk("[%s] Wrong idx input !\n",__FUNCTION__);
		return -1;
	}
	pbus_cfg = pbus_mon_get_cfg();
	
	mon_out->addr_max = get_pbus_monitor_data(PBUS_MON_0_ADDR_MAX + (MON_IDX_OFFSET * mon_out->mon_idx));
	mon_out->addr_min = get_pbus_monitor_data(PBUS_MON_0_ADDR_MIN + (MON_IDX_OFFSET * mon_out->mon_idx));
	mon_out->wr_sel = (pbus_cfg & (0x1 << (MON_WR_BIT_OFFSET + (mon_out->mon_idx * PMON_ENTRY_SIZE)))) >> (MON_WR_BIT_OFFSET + (mon_out->mon_idx * PMON_ENTRY_SIZE)); 
	mon_out->master_id = (pbus_cfg & (0xf << (MON_IDX_BIT_OFFSET + (mon_out->mon_idx * PMON_ENTRY_SIZE)))) >> (MON_IDX_BIT_OFFSET + (mon_out->mon_idx * PMON_ENTRY_SIZE)); 
	return 0;

}
EXPORT_SYMBOL(pbus_mon_get);

int pbus_mon_en(unsigned int idx, int enable)
{
	unsigned int pbus_cfg = 0;
	int i = 0 ;
	/* Check if indexis valid*/
	if (idx > PMON_IDX_VAL)
	{
		printk("[%s] Wrong idx input !\n",__FUNCTION__);
		return -1;
	}

	pbus_cfg = get_pbus_monitor_data(PBUS_MON_GLO_CFG);

	for (i = 0 ; i <PMON_MAX_NUM; i++)
	{

		if ((idx & (0x1<< i))!=0)
		{
	
			if (!enable)
			{
				
				pbus_cfg &= ~(0x1 << (i* PMON_ENTRY_SIZE));
		
			}
			else
			{
				
				pbus_cfg |= (0x1 << (i* PMON_ENTRY_SIZE));
		
			}
		}
	}

	set_pbus_monitor_data(PBUS_MON_GLO_CFG, pbus_cfg);
	printk("%s Pbus monitor id 0x%x\n", (enable ? "Enable" : "Disable"), idx);

	return 0;
}
EXPORT_SYMBOL(pbus_mon_en);

int pbus_mon_clr(unsigned int idx)
{
	unsigned int pbus_cfg = 0;
	int i = 0 ;

	/* Check if indexis valid*/
	if (idx > PMON_IDX_VAL)
	{
		printk("[%s] Wrong idx input !\n",__FUNCTION__);
		return -1;
	}
	pbus_cfg = get_pbus_monitor_data(PBUS_MON_GLO_CFG);

	for (i = 0 ; i <PMON_MAX_NUM; i++)
	{
		if ((idx & (0x1<< i))!=0)
		{
			pbus_cfg |= (0x1 << (MON_CLR_BIT_OFFSET+(i* PMON_ENTRY_SIZE)));

		}
	}
	set_pbus_monitor_data(PBUS_MON_GLO_CFG, pbus_cfg);
	printk("%s Clear Pbus monitor id 0x%x\n",__FUNCTION__, idx);

	return 0;

}
EXPORT_SYMBOL(pbus_mon_clr);

int pbus_mon_period_set(int idx, uint64_t cnt)
{
	uint64_t CNT_L = cnt & 0xffffffff;
	uint64_t CNT_H = (cnt >> REG_SIZE) & 0xffffffff;

	set_pbus_monitor_data(PBUS_MON_0_ACC_TOTAL_H + (MON_IDX_OFFSET * idx),(unsigned int)CNT_H);
	set_pbus_monitor_data(PBUS_MON_0_ACC_TOTAL_L + (MON_IDX_OFFSET * idx),(unsigned int)CNT_L);
	
	return 0;
}
EXPORT_SYMBOL(pbus_mon_period_set);

int pbus_mon_cnt_get(int idx, pbus_mon_cnt_t *cnt)
{
	/* Check if indexis valid*/
	if (idx > PMON_MAX_NUM)
	{
		printk("[%s] Wrong idx input !\n",__FUNCTION__);
		return -1;
	}

	cnt->mon_acc_cnt = ((uint64_t)get_pbus_monitor_data(PBUS_MON_0_ACC_CNT_H + (MON_IDX_OFFSET * idx))) << REG_SIZE;
	cnt->mon_acc_cnt |= (uint64_t)get_pbus_monitor_data(PBUS_MON_0_ACC_CNT_L + (MON_IDX_OFFSET * idx));
	cnt->mon_acc_prd = ((uint64_t)get_pbus_monitor_data(PBUS_MON_0_ACC_PRD_H + (MON_IDX_OFFSET * idx))) << REG_SIZE;
	cnt->mon_acc_prd |= (uint64_t)get_pbus_monitor_data(PBUS_MON_0_ACC_PRD_L + (MON_IDX_OFFSET * idx));
	cnt->mon_acc_total = ((uint64_t)get_pbus_monitor_data(PBUS_MON_0_ACC_TOTAL_H + (MON_IDX_OFFSET * idx))) << REG_SIZE;
	cnt->mon_acc_total |= (uint64_t)get_pbus_monitor_data(PBUS_MON_0_ACC_TOTAL_L + (MON_IDX_OFFSET * idx));
	cnt->mon_acc_max = get_pbus_monitor_data(PBUS_MON_0_ACC_MAX + (MON_IDX_OFFSET * idx));
	return 0;
}
EXPORT_SYMBOL(pbus_mon_cnt_get);


int pbus_regs_test(u32 cnt)
{
    set_pbus_monitor_data(CR_PBUS_PROBE_SEL, cnt);
    if (get_pbus_monitor_data(CR_PBUS_PROBE_SEL)!=cnt) {
        printk("Error: (CR_PBUS_PROBE_SEL:0x%x != 0x%x))\n", get_pbus_monitor_data(CR_PBUS_PROBE_SEL), cnt);
        return -1;
    }
    return 0;
}
EXPORT_SYMBOL(pbus_regs_test);

u32 pcie_mem_base_get(int pcie_no)
{
    return get_pbus_monitor_data(PBUS_PCIE0_MEM_BASE+(pcie_no<<3));
}
EXPORT_SYMBOL(pcie_mem_base_get);

void pcie_single_enable(int pcie_no, int enable)
{
    u32 val = get_pbus_monitor_data(CR_PCIE_SINGLE_EN);

    if (enable)
        val |= (1<<pcie_no);
    else
        val &= ~(1<<pcie_no);

    set_pbus_monitor_data(CR_PCIE_SINGLE_EN, val);
    return;
}
EXPORT_SYMBOL(pcie_single_enable);

void pbus_dbg_cnt_clear(u32 clearBits)
{
    set_pbus_monitor_data(CR_PBUS_DBG_CNT_CLR, clearBits);
    return;
}
EXPORT_SYMBOL(pbus_dbg_cnt_clear);

u32 pbus_dbg_cnt_get(int dbg_cnt_no)
{
    if (dbg_cnt_no>=18)
        dbg_cnt_no += 6;
    return get_pbus_monitor_data(CR_PBUS_DBG_CNT_0+(dbg_cnt_no<<2));
}
EXPORT_SYMBOL(pbus_dbg_cnt_get);


static int ecnt_pbus_monitor_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No pbus_monitor DT node found\n");
        return -EINVAL;
    }

    ecnt_pbus_monitor = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_pbus_monitor), GFP_KERNEL);
    if (!ecnt_pbus_monitor)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_pbus_monitor);

    /* get pbus_monitor base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_pbus_monitor->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_pbus_monitor->base))
        return PTR_ERR(ecnt_pbus_monitor->base);

    ecnt_pbus_monitor->dev = &pdev->dev;
	
    printk("\npbus_monitor init(%s) done\n", __func__);
    #if 0
	printk("[pbus_monitor] res->name:%s\n", res->name);
	printk("[pbus_monitor] res->start:0x%llx ===\n", res->start);
	printk("[pbus_monitor] res->end:0x%llx ===\n", res->end);
	printk("[pbus_monitor] ecnt_pbus_monitor->base:0x%lx\n", (unsigned long)ecnt_pbus_monitor->base);
    #endif

    return 0;
}

static int ecnt_pbus_monitor_drv_remove(struct platform_device *pdev)
{
    return 0;
}


/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_pbus_monitor_driver = {
    .probe = ecnt_pbus_monitor_drv_probe,
    .remove = ecnt_pbus_monitor_drv_remove,
    .driver = {
        .name = "ecnt-pbus_monitor",
        .of_match_table = ecnt_pbus_monitor_of_id
    },
};
module_platform_driver(ecnt_pbus_monitor_driver);


MODULE_DESCRIPTION("EcoNet pbus_monitor Driver");


