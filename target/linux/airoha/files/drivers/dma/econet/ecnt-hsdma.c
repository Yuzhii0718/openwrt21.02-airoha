#include <linux/bitops.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/iopoll.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/smp.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "../virt-dma.h"
#include "ecnt-hsdma.h"

static u16 hsdma_pkt_len = ECNT_HSDMA_MAX_LEN;
module_param_cb(hsdma_pkt_len, &param_ops_ushort, &hsdma_pkt_len, (S_IRUGO | S_IWUSR));

static struct proc_dir_entry *proc_hsdma_reg = NULL;
static struct ecnt_hsdma_device *g_hsdma = NULL;

void (*test_irq_handler)(void) = NULL;
EXPORT_SYMBOL(test_irq_handler);

/* HSDMA_CPU_BUS_TEST */
/* for hadms block cnt test in modules/private/tcci/cpu_bus_test.c */
void (*set_hsdma_block_test_hook)(dma_addr_t, dma_addr_t, u32, u32)=NULL;
void (*enable_hsdma_by_txCpu_hook)(void)=NULL;
int (*wait_hsdma_done_hook)(void)=NULL;
EXPORT_SYMBOL(set_hsdma_block_test_hook);
EXPORT_SYMBOL(enable_hsdma_by_txCpu_hook);
EXPORT_SYMBOL(wait_hsdma_done_hook);
/* HSDMA_CPU_BUS_TEST */

static struct ecnt_hsdma_device *to_hsdma_dev(struct dma_chan *chan)
{
	return container_of(chan->device, struct ecnt_hsdma_device, ddev);
}

static struct ecnt_hsdma_vchan *to_hsdma_vchan(struct dma_chan *chan)
{
	return container_of(chan, struct ecnt_hsdma_vchan, vc.chan);
}

static struct ecnt_hsdma_vdesc *to_hsdma_vdesc(struct virt_dma_desc *vd)
{
	return container_of(vd, struct ecnt_hsdma_vdesc, vd);
}

static struct device *hsdma2dev(struct ecnt_hsdma_device *hsdma)
{
	return hsdma->ddev.dev;
}

static u32 ecnt_dma_read(struct ecnt_hsdma_device *hsdma, u32 reg)
{
	return readl(hsdma->base + reg);
}

static void ecnt_dma_write(struct ecnt_hsdma_device *hsdma, u32 reg, u32 val)
{
	writel(val, hsdma->base + reg);
}

static void ecnt_dma_rmw(struct ecnt_hsdma_device *hsdma, u32 reg, u32 mask, u32 set)
{
	u32 val = 0;

	val = ecnt_dma_read(hsdma, reg);
	val &= ~mask;
	val |= set;
	ecnt_dma_write(hsdma, reg, val);
}

static void ecnt_dma_set(struct ecnt_hsdma_device *hsdma, u32 reg, u32 val)
{
	ecnt_dma_rmw(hsdma, reg, 0, val);
}

static void ecnt_dma_clr(struct ecnt_hsdma_device *hsdma, u32 reg, u32 val)
{
	ecnt_dma_rmw(hsdma, reg, val, 0);
}

static void ecnt_hsdma_vdesc_free(struct virt_dma_desc *vd)
{
	kfree(container_of(vd, struct ecnt_hsdma_vdesc, vd));
}

static int ecnt_hsdma_busy_wait(struct ecnt_hsdma_device *hsdma)
{
	u32 status = 0;

	return readl_poll_timeout(hsdma->base + ECNT_HSDMA_GLO, status,
				  !(status & ECNT_HSDMA_GLO_BUSY),
				  ECNT_HSDMA_USEC_POLL,
				  ECNT_HSDMA_TIMEOUT_POLL);
}

void ecnt_hsdma_reg_dump(void)
{
    printk("ECNT_HSDMA_TX_BASE    0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_BASE),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_BASE));
    printk("ECNT_HSDMA_TX_CNT     0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_CNT),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_CNT));
    printk("ECNT_HSDMA_TX_CPU     0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_CPU),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_CPU));
    printk("ECNT_HSDMA_TX_DMA     0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_DMA),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_DMA));
    printk("ECNT_HSDMA_TX_BASE_1  0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_BASE_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_BASE_1));
    printk("ECNT_HSDMA_TX_CNT_1   0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_CNT_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_CNT_1));
    printk("ECNT_HSDMA_TX_CPU_1   0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_CPU_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_CPU_1));
    printk("ECNT_HSDMA_TX_DMA_1   0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_TX_DMA_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_TX_DMA_1));

    printk("ECNT_HSDMA_RX_BASE    0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_BASE),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_BASE));
    printk("ECNT_HSDMA_RX_CNT     0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_CNT),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_CNT));
    printk("ECNT_HSDMA_RX_CPU     0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_CPU),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_CPU));
    printk("ECNT_HSDMA_RX_DMA     0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_DMA),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_DMA));
    printk("ECNT_HSDMA_RX_BASE_1  0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_BASE_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_BASE_1));
    printk("ECNT_HSDMA_RX_CNT_1   0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_CNT_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_CNT_1));
    printk("ECNT_HSDMA_RX_CPU_1   0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_CPU_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_CPU_1));
    printk("ECNT_HSDMA_RX_DMA_1   0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RX_DMA_1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RX_DMA_1));

    printk("ECNT_HSDMA_INFO       0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_INFO),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_INFO));
    printk("ECNT_HSDMA_GLO        0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_GLO),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_GLO));
    printk("ECNT_HSDMA_RESET      0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_RESET),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_RESET));
    printk("ECNT_HSDMA_DLYINT     0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_DLYINT),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_DLYINT));
    printk("ECNT_HSDMA_FRQ_THR    0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_FRQ_THR),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_FRQ_THR));
    printk("ECNT_HSDMA_INT_STATUS 0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_INT_STATUS),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_INT_STATUS));
    printk("ECNT_HSDMA_INT_ENABLE 0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_INT_ENABLE),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_INT_ENABLE));
    printk("ECNT_HSDMA_DBG0       0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_DBG0),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_DBG0));
    printk("ECNT_HSDMA_DBG1       0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_DBG1),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_DBG1));
    printk("ECNT_HSDMA_DBG2       0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_DBG2),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_DBG2));
    printk("ECNT_HSDMA_DBG3       0x%08x= 0x%08x\n", (ECNT_HSDMA_PHYS + ECNT_HSDMA_DBG3),
						ecnt_dma_read(g_hsdma, ECNT_HSDMA_DBG3));
}
EXPORT_SYMBOL(ecnt_hsdma_reg_dump);

int ecnt_hsdma_reg_access_test(unsigned int cnt)
{
	ecnt_dma_write(g_hsdma, ECNT_HSDMA_DLYINT, cnt);
    if (ecnt_dma_read(g_hsdma, ECNT_HSDMA_DLYINT)!=cnt) {
        printk("hsdma_read(DLYINT):0x%x != 0x%x\n", ecnt_dma_read(g_hsdma, ECNT_HSDMA_DLYINT), cnt);
        return -1;
    }
    return 0;
}
EXPORT_SYMBOL(ecnt_hsdma_reg_access_test);

void ecnt_hsdma_disable_test_mode(u32 old_cfg, u32 old_intr_dly)
{
	ecnt_dma_write(g_hsdma, ECNT_HSDMA_INT_STATUS, ECNT_HSDMA_INT_ALL);
	ecnt_dma_write(g_hsdma, ECNT_HSDMA_GLO, old_cfg);
	ecnt_dma_write(g_hsdma, ECNT_HSDMA_DLYINT, old_intr_dly);
}
EXPORT_SYMBOL(ecnt_hsdma_disable_test_mode);

void ecnt_hsdma_enable_test_mode(u32 *old_cfg, u32 *old_intr_dly, u32 *old_intr, __le32 *ddone, __le32 *nls)
{
	*old_cfg = ecnt_dma_read(g_hsdma, ECNT_HSDMA_GLO);
	*old_intr_dly = ecnt_dma_read(g_hsdma, ECNT_HSDMA_DLYINT);
	*old_intr = ecnt_dma_read(g_hsdma, ECNT_HSDMA_INT_ENABLE);
	*ddone = g_hsdma->soc->ddone;
	*nls = g_hsdma->soc->nls;
	ecnt_dma_write(g_hsdma, ECNT_HSDMA_DLYINT, 0);
	ecnt_dma_write(g_hsdma, ECNT_HSDMA_INT_STATUS, ECNT_HSDMA_INT_ALL);
}
EXPORT_SYMBOL(ecnt_hsdma_enable_test_mode);

void ecnt_hsdma_free_dma(u32 buf_size, u8 *unc, dma_addr_t phys)
{
	dma_free_coherent(hsdma2dev(g_hsdma), buf_size, unc, phys);
}
EXPORT_SYMBOL(ecnt_hsdma_free_dma);

u8 *ecnt_hsdma_alloc_dma(u32 buf_size, dma_addr_t *phys)
{
	return dma_alloc_coherent(hsdma2dev(g_hsdma), buf_size, phys, GFP_KERNEL | __GFP_ZERO);
}
EXPORT_SYMBOL(ecnt_hsdma_alloc_dma);

struct ecnt_hsdma_ring *ecnt_hsdma_get_ring(int chan_num)
{
	if (!test_irq_handler)
		return 0;
	return &(g_hsdma->pc[chan_num].ring);
}
EXPORT_SYMBOL(ecnt_hsdma_get_ring);

u32 ecnt_hsdma_get_cfg(void)
{
	if (!test_irq_handler)
		return 0;
	return ecnt_dma_read(g_hsdma, ECNT_HSDMA_GLO);
}
EXPORT_SYMBOL(ecnt_hsdma_get_cfg);

void ecnt_hsdma_disable_dly_intr(int drct)
{
	if (!test_irq_handler)
		return;
	if (drct == ECNT_HSDMA_TX)
	{
		ecnt_dma_clr(g_hsdma, ECNT_HSDMA_DLYINT,
							(ECNT_HSDMA_TXDLY_INT_EN		|
							 ECNT_HSDMA_TXMAX_PINT(0x7F)	|
							 ECNT_HSDMA_TXMAX_PTIME(0xFF)));
	}
	else if (drct == ECNT_HSDMA_RX)
	{
		ecnt_dma_clr(g_hsdma, ECNT_HSDMA_DLYINT,
							(ECNT_HSDMA_RXDLY_INT_EN		|
							 ECNT_HSDMA_RXMAX_PINT(0x7F)	|
							 ECNT_HSDMA_RXMAX_PTIME(0xFF)));
	}
}
EXPORT_SYMBOL(ecnt_hsdma_disable_dly_intr);

void ecnt_hsdma_enable_dly_intr(int drct, u8 count, u8 times)
{
	if (!test_irq_handler)
		return;
	ecnt_hsdma_disable_dly_intr(drct);
	if (drct == ECNT_HSDMA_TX)
	{
		ecnt_dma_write(g_hsdma, ECNT_HSDMA_DLYINT,
							(ECNT_HSDMA_TXDLY_INT_EN		|
							 ECNT_HSDMA_TXMAX_PINT(count)	|
							 ECNT_HSDMA_TXMAX_PTIME(times)));
	}
	else if (drct == ECNT_HSDMA_RX)
	{
		ecnt_dma_write(g_hsdma, ECNT_HSDMA_DLYINT,
							(ECNT_HSDMA_RXDLY_INT_EN		|
							 ECNT_HSDMA_RXMAX_PINT(count)	|
							 ECNT_HSDMA_RXMAX_PTIME(times)));
	}
}
EXPORT_SYMBOL(ecnt_hsdma_enable_dly_intr);

void ecnt_hsdma_disable_rdbk(void)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_clr(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_READ_BACK);
}
EXPORT_SYMBOL(ecnt_hsdma_disable_rdbk);

void ecnt_hsdma_enable_rdbk(void)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_set(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_READ_BACK);
}
EXPORT_SYMBOL(ecnt_hsdma_enable_rdbk);

void ecnt_hsdma_disable_tx_wbd(void)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_clr(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_TX_WB_DDONE);
}
EXPORT_SYMBOL(ecnt_hsdma_disable_tx_wbd);

void ecnt_hsdma_enable_tx_wbd(void)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_set(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_TX_WB_DDONE);
}
EXPORT_SYMBOL(ecnt_hsdma_enable_tx_wbd);

void ecnt_hsdma_disable_swap(void)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_clr(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_BYTE_SWAP);
}
EXPORT_SYMBOL(ecnt_hsdma_disable_swap);

void ecnt_hsdma_enable_swap(void)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_set(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_BYTE_SWAP);
}
EXPORT_SYMBOL(ecnt_hsdma_enable_swap);

void ecnt_hsdma_set_burst_size(u32 size)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_rmw(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_BURST_128BYTES, size);
}
EXPORT_SYMBOL(ecnt_hsdma_set_burst_size);

void ecnt_hsdma_clear_intr_sts(u32 intr)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_write(g_hsdma, ECNT_HSDMA_INT_STATUS, intr);
}
EXPORT_SYMBOL(ecnt_hsdma_clear_intr_sts);

u32 ecnt_hsdma_get_intr_sts(void)
{
	if (!test_irq_handler)
		return 0;
	return ecnt_dma_read(g_hsdma, ECNT_HSDMA_INT_STATUS);
}
EXPORT_SYMBOL(ecnt_hsdma_get_intr_sts);

u32 ecnt_hsdma_get_intr(void)
{
	if (!test_irq_handler)
		return 0;
	return ecnt_dma_read(g_hsdma, ECNT_HSDMA_INT_ENABLE);
}
EXPORT_SYMBOL(ecnt_hsdma_get_intr);

void ecnt_hsdma_disable_intr(u32 intr)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_clr(g_hsdma, ECNT_HSDMA_INT_ENABLE, intr);
}
EXPORT_SYMBOL(ecnt_hsdma_disable_intr);

void ecnt_hsdma_enable_intr(u32 intr)
{
	if (!test_irq_handler)
		return;
	ecnt_dma_set(g_hsdma, ECNT_HSDMA_INT_ENABLE, intr);
}
EXPORT_SYMBOL(ecnt_hsdma_enable_intr);

void ecnt_hsdma_set_idx(int chan_num, int drct, int type, u32 idx)
{
	struct ecnt_hsdma_pchan *pc = &(g_hsdma->pc[chan_num]);

	if (!test_irq_handler)
		return;
	if (drct == ECNT_HSDMA_TX)
	{
		if (type == ECNT_HSDMA_CPU)
		{
			ecnt_dma_write(g_hsdma, pc->tx_cpu, idx);
		}
	}
	else if (drct == ECNT_HSDMA_RX)
	{
		if (type == ECNT_HSDMA_CPU)
		{
			ecnt_dma_write(g_hsdma, pc->rx_cpu, idx);
		}
	}
}
EXPORT_SYMBOL(ecnt_hsdma_set_idx);

u32 ecnt_hsdma_get_idx(int chan_num, int drct, int type)
{
	struct ecnt_hsdma_pchan *pc = &(g_hsdma->pc[chan_num]);

	if (!test_irq_handler)
		return 0;
	if (drct == ECNT_HSDMA_TX)
	{
		if (type == ECNT_HSDMA_CPU)
		{
			return ecnt_dma_read(g_hsdma, pc->tx_cpu);
		}
		else if (type == ECNT_HSDMA_DMA)
		{
			return ecnt_dma_read(g_hsdma, pc->tx_dma);
		}
	}
	else if (drct == ECNT_HSDMA_RX)
	{
		if (type == ECNT_HSDMA_CPU)
		{
			return ecnt_dma_read(g_hsdma, pc->rx_cpu);
		}
		else if (type == ECNT_HSDMA_DMA)
		{
			return ecnt_dma_read(g_hsdma, pc->rx_dma);
		}
	}

	return 0;
}
EXPORT_SYMBOL(ecnt_hsdma_get_idx);

void ecnt_hsdma_reset(void)
{
	int i = 0, j = 0;
	struct ecnt_hsdma_pchan *pc = NULL;
	struct ecnt_hsdma_ring *ring = NULL;

	if (!test_irq_handler)
		return;
	for (i = 0; i < ECNT_HSDMA_NR_MAX_PCHANS; i++)
	{
		pc = &(g_hsdma->pc[i]);
		ring = &(pc->ring);

		memset(ring->txd, 0, pc->sz_ring);

		for (j = 0; j < ECNT_DMA_SIZE; j++)
		{
			ring->txd[j].desc2 |= g_hsdma->soc->ddone;
			ring->rxd[j].desc2 &= ~(g_hsdma->soc->ddone);
		}

		ring->cur_tptr = 0;
		ring->cur_rptr = ECNT_DMA_SIZE - 1;

		/* Reset */
		ecnt_dma_write(g_hsdma, ECNT_HSDMA_RESET, pc->reset);

		/* Setup HSDMA initial pointer in the ring */
		ecnt_dma_write(g_hsdma, pc->tx_cpu, ring->cur_tptr);
		ecnt_dma_write(g_hsdma, pc->rx_cpu, ring->cur_rptr);
	}
}
EXPORT_SYMBOL(ecnt_hsdma_reset);

void ecnt_hsdma_stop_dma(u32 value)
{
	if (!test_irq_handler)
		return;
	if (value & ECNT_HSDMA_TX)
		ecnt_dma_clr(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_TX_DMA);
	if (value & ECNT_HSDMA_RX)
		ecnt_dma_clr(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_RX_DMA);
}
EXPORT_SYMBOL(ecnt_hsdma_stop_dma);

void ecnt_hsdma_start_dma(u32 value)
{
	if (!test_irq_handler)
		return;
	if (value & ECNT_HSDMA_TX)
		ecnt_dma_set(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_TX_DMA);
	if (value & ECNT_HSDMA_RX)
		ecnt_dma_set(g_hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_RX_DMA);
}
EXPORT_SYMBOL(ecnt_hsdma_start_dma);

static void ecnt_hsdma_init_pchan(struct ecnt_hsdma_pchan *pc, int chan_num)
{
	memset(pc, 0, sizeof(*pc));
	pc->chan_num = chan_num;

	pc->tx_base = (ECNT_HSDMA_TX_BASE + (chan_num << 4));
	pc->tx_cnt = (ECNT_HSDMA_TX_CNT + (chan_num << 4));
	pc->tx_cpu = (ECNT_HSDMA_TX_CPU + (chan_num << 4));
	pc->tx_dma = (ECNT_HSDMA_TX_DMA + (chan_num << 4));
	pc->rx_base = (ECNT_HSDMA_RX_BASE + (chan_num << 4));
	pc->rx_cnt = (ECNT_HSDMA_RX_CNT + (chan_num << 4));
	pc->rx_cpu = (ECNT_HSDMA_RX_CPU + (chan_num << 4));
	pc->rx_dma = (ECNT_HSDMA_RX_DMA + (chan_num << 4));
	pc->reset = ((ECNT_HSDMA_RST_TX | ECNT_HSDMA_RST_RX) << chan_num);
	pc->intr = (ECNT_HSDMA_INT_RXDONE << chan_num);
}

static int ecnt_hsdma_alloc_pchan(struct ecnt_hsdma_device *hsdma,
					struct ecnt_hsdma_pchan *pc, int chan_num)
{
	struct ecnt_hsdma_ring *ring = &(pc->ring);
	int i = 0, err = 0;

	ecnt_hsdma_init_pchan(pc, chan_num);

	/*
	 * Allocate ring space where [0 ... ECNT_DMA_SIZE - 1] is for TX ring
	 * and [ECNT_DMA_SIZE ... 2 * ECNT_DMA_SIZE - 1] is for RX ring.
	 */
	pc->sz_ring = 2 * ECNT_DMA_SIZE * sizeof(*ring->txd);
	ring->txd = dma_alloc_coherent(hsdma2dev(hsdma), pc->sz_ring, &(ring->tphys), GFP_KERNEL | __GFP_ZERO);
	if (!ring->txd)
	{
		dev_err(hsdma2dev(hsdma), "Allocate descripter failed\n");
		return -ENOMEM;
	}

	ring->rxd = &(ring->txd[ECNT_DMA_SIZE]);
	ring->rphys = ring->tphys + ECNT_DMA_SIZE * sizeof(*ring->txd);
	ring->cur_tptr = 0;
	ring->cur_rptr = ECNT_DMA_SIZE - 1;

	for (i = 0; i < ECNT_DMA_SIZE; i++)
	{
		ring->txd[i].desc2 |= hsdma->soc->ddone;
		ring->rxd[i].desc2 &= ~(hsdma->soc->ddone);
	}

	wmb();

	ring->cb = kcalloc(ECNT_DMA_SIZE, sizeof(*ring->cb), GFP_KERNEL);
	if (!ring->cb)
	{
		err = -ENOMEM;
		dev_err(hsdma2dev(hsdma), "Allocate cb failed\n");
		goto err_free_dma;
	}

	atomic_set(&(pc->nr_free), ECNT_DMA_SIZE - 1);

	/* Disable HSDMA and wait for the completion */
	ecnt_dma_clr(hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_DMA);
	err = ecnt_hsdma_busy_wait(hsdma);
	if (err)
		goto err_free_cb;

	/* Reset */
	ecnt_dma_write(hsdma, ECNT_HSDMA_RESET, pc->reset);

	/* Setup HSDMA initial pointer in the ring */
	ecnt_dma_write(hsdma, pc->tx_base, ring->tphys);
	ecnt_dma_write(hsdma, pc->tx_cnt , ECNT_DMA_SIZE);
	ecnt_dma_write(hsdma, pc->tx_cpu, ring->cur_tptr);
	ecnt_dma_write(hsdma, pc->rx_base, ring->rphys);
	ecnt_dma_write(hsdma, pc->rx_cnt, ECNT_DMA_SIZE);
	ecnt_dma_write(hsdma, pc->rx_cpu, ring->cur_rptr);

	/* Enable HSDMA */
	ecnt_dma_set(hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_DMA);

	/* Setup delayed interrupt */
	ecnt_dma_write(hsdma, ECNT_HSDMA_DLYINT, ECNT_HSDMA_DLYINT_DEFAULT);

	/* Enable interrupt */
	ecnt_dma_set(hsdma, ECNT_HSDMA_INT_ENABLE, pc->intr);

	return 0;

err_free_cb:
	kfree(ring->cb);

err_free_dma:
	dma_free_coherent(hsdma2dev(hsdma), pc->sz_ring, ring->txd, ring->tphys);
	return err;
}

static void ecnt_hsdma_free_pchan(struct ecnt_hsdma_device *hsdma,
					struct ecnt_hsdma_pchan *pc)
{
	struct ecnt_hsdma_ring *ring = &(pc->ring);

	/* Disable HSDMA and then wait for the completion */
	ecnt_dma_clr(hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_DMA);
	ecnt_hsdma_busy_wait(hsdma);

	/* Reset pointer in the ring */
	ecnt_dma_clr(hsdma, ECNT_HSDMA_INT_ENABLE, pc->intr);
	ecnt_dma_write(hsdma, pc->tx_base, 0);
	ecnt_dma_write(hsdma, pc->tx_cnt, 0);
	ecnt_dma_write(hsdma, pc->tx_cpu, 0);
	ecnt_dma_write(hsdma, pc->rx_base, 0);
	ecnt_dma_write(hsdma, pc->rx_cnt, 0);
	ecnt_dma_write(hsdma, pc->rx_cpu, ECNT_DMA_SIZE - 1);

	kfree(ring->cb);
	dma_free_coherent(hsdma2dev(hsdma), pc->sz_ring, ring->txd, ring->tphys);
}

static int ecnt_hsdma_issue_pending_vdesc(struct ecnt_hsdma_device *hsdma,
					struct ecnt_hsdma_pchan *pc,
					struct ecnt_hsdma_vdesc *hvd)
{
	struct ecnt_hsdma_ring *ring = &(pc->ring);
	volatile struct ecnt_hsdma_pdesc *txd = NULL, *rxd = NULL;
	u16 reserved = 0, prev = 0, tlen = 0, num_sgs = 0;
	__le32 desc2 = 0;
	unsigned long flags = 0;

	/* Protect against PC is accessed by multiple VCs simultaneously */
	spin_lock_irqsave(&(pc->lock), flags);

	/*
	 * Reserve rooms, where pc->nr_free is used to track how many free
	 * rooms in the ring being updated in user and IRQ context.
	 */
	num_sgs = DIV_ROUND_UP(hvd->len, hsdma_pkt_len);
	reserved = min_t(u16, num_sgs, atomic_read(&(pc->nr_free)));

	if (!reserved) {
		spin_unlock_irqrestore(&(pc->lock), flags);
		return -ENOSPC;
	}

	atomic_sub(reserved, &(pc->nr_free));

	while (reserved--)
	{
		/*
		 * Setup PDs using the remaining VD info mapped on those
		 * reserved rooms. And since RXD is shared memory between the
		 * host and the device allocated by dma_alloc_coherent call,
		 * the helper macro WRITE_ONCE can ensure the data written to
		 * RAM would really happens.
		 */
		txd = &(ring->txd[ring->cur_tptr]);
		rxd = &(ring->rxd[ring->cur_tptr]);

		WARN(!(txd->desc2 & hsdma->soc->ddone),
			  "TXD done bit is 0\n");
		WARN((rxd->desc2 & hsdma->soc->ddone),
			  "RXD done bit is 1\n");

		desc2 = 0;
		/* Limit size by PD capability for valid data moving */
		if (hvd->len > hsdma_pkt_len)
		{
			tlen = hsdma_pkt_len;
			desc2 |= hsdma->soc->nls;
		}
		else
			tlen = hvd->len;

		desc2 |= ECNT_HSDMA_DESC_PLEN(tlen);

		WRITE_ONCE(rxd->desc3, hvd->dest);
		WRITE_ONCE(rxd->desc2, desc2);
		WRITE_ONCE(txd->desc3, hvd->src);
		WRITE_ONCE(txd->desc2, desc2);

		/* Associate VD, the PD belonged to */
		ring->cb[ring->cur_tptr].vd = &(hvd->vd);

		/* Move forward the pointer of TX ring */
		ring->cur_tptr = ECNT_HSDMA_NEXT_DESP_IDX(ring->cur_tptr,
							 ECNT_DMA_SIZE);

		/* Update VD with remaining data */
		hvd->src  += tlen;
		hvd->dest += tlen;
		hvd->len  -= tlen;
	}

	/*
	 * Tagging flag for the last PD for VD will be responsible for
	 * completing VD.
	 */
	if (!hvd->len)
	{
		prev = ECNT_HSDMA_LAST_DESP_IDX(ring->cur_tptr, ECNT_DMA_SIZE);
		ring->cb[prev].flag = ECNT_HSDMA_VDESC_FINISHED;
	}

	/* Ensure all changes indeed done before we're going on */
	wmb();


	/* Updating into hardware the pointer of TX ring lets HSDMA to take
	 * action for those pending PDs.
	 */
	ecnt_dma_write(hsdma, pc->tx_cpu, ring->cur_tptr);

	spin_unlock_irqrestore(&(pc->lock), flags);

	return 0;
}

static void ecnt_hsdma_issue_vchan_pending(struct ecnt_hsdma_device *hsdma,
					  struct ecnt_hsdma_vchan *hvc, int chan_num)
{
	struct virt_dma_desc *vd = NULL, *vd2 = NULL;
	struct ecnt_hsdma_vdesc *hvd = NULL;
	int err = 0;

	lockdep_assert_held(&(hvc->vc.lock));

	list_for_each_entry_safe(vd, vd2, &(hvc->vc.desc_issued), node)
	{
		hvd = to_hsdma_vdesc(vd);

		/* Map VD into PC and all VCs shares a single PC */
		err = ecnt_hsdma_issue_pending_vdesc(hsdma, &(hsdma->pc[chan_num]), hvd);

		/*
		 * Move VD from desc_issued to desc_hw_processing when entire
		 * VD is fit into available PDs. Otherwise, the uncompleted
		 * VDs would stay in list desc_issued and then restart the
		 * processing as soon as possible once underlying ring space
		 * got freed.
		 */
		if (err == -ENOSPC || hvd->len > 0)
			break;

		/*
		 * The extra list desc_hw_processing is used because
		 * hardware can't provide sufficient information allowing us
		 * to know what VDs are still working on the underlying ring.
		 * Through the additional list, it can help us to implement
		 * terminate_all, residue calculation and such thing needed
		 * to know detail descriptor status on the hardware.
		 */
		list_move_tail(&(vd->node), &(hvc->desc_hw_processing));
	}
}

static void ecnt_hsdma_free_rooms_in_ring(struct ecnt_hsdma_device *hsdma)
{
	volatile struct ecnt_hsdma_pdesc *rxd = NULL;
	struct ecnt_hsdma_vchan *hvc = NULL;
	struct ecnt_hsdma_vdesc *hvd = NULL;
	struct ecnt_hsdma_pchan *pc = NULL;
	struct ecnt_hsdma_cb *cb = NULL;
	int i = ECNT_DMA_SIZE, j = 0;
	__le32 desc2 = 0;
	u32 status = 0;
	u16 next = 0;

	/* Read IRQ status */
	status = ecnt_dma_read(hsdma, ECNT_HSDMA_INT_STATUS);
	if (unlikely(!(status & (ECNT_HSDMA_INT_RXDONE | ECNT_HSDMA_INT_RXDONE_1))))
		goto rx_done;

	for (j = 0; j < ECNT_HSDMA_NR_MAX_PCHANS; j++)
	{

		pc = &(hsdma->pc[j]);

		if (!(status & pc->intr))
			continue;

		/*
		 * Using a fail-safe loop with iterations of up to ECNT_DMA_SIZE to
		 * reclaim these finished descriptors: The most number of PDs the ISR
		 * can handle at one time shouldn't be more than ECNT_DMA_SIZE so we
		 * take it as limited count instead of just using a dangerous infinite
		 * poll.
		 */
		while (i--)
		{
			next = ECNT_HSDMA_NEXT_DESP_IDX(pc->ring.cur_rptr, ECNT_DMA_SIZE);
			rxd = &(pc->ring.rxd[next]);

			/*
			 * If ECNT_HSDMA_DESC_DDONE is no specified, that means data
			 * moving for the PD is still under going.
			 */
			desc2 = READ_ONCE(rxd->desc2);
			if (!(desc2 & hsdma->soc->ddone))
				break;

			cb = &(pc->ring.cb[next]);
			if (unlikely(!cb->vd))
			{
				dev_err(hsdma2dev(hsdma), "cb->vd cannot be null\n");
				break;
			}

			/* Update residue of VD the associated PD belonged to */
			hvd = to_hsdma_vdesc(cb->vd);
			hvd->residue -= ECNT_HSDMA_DESC_PLEN_GET(rxd->desc2);

			/* Complete VD until the relevant last PD is finished */
			if (IS_ECNT_HSDMA_VDESC_FINISHED(cb->flag))
			{
				hvc = to_hsdma_vchan(cb->vd->tx.chan);

				spin_lock(&(hvc->vc.lock));

				/* Remove VD from list desc_hw_processing */
				list_del(&cb->vd->node);

				/* Add VD into list desc_completed */
				vchan_cookie_complete(cb->vd);

				if (hvc->issue_synchronize && list_empty(&(hvc->desc_hw_processing)))
				{
					complete(&(hvc->issue_completion));
					hvc->issue_synchronize = false;
				}
				spin_unlock(&(hvc->vc.lock));

				cb->flag = 0;
			}

			cb->vd = 0;

			/*
			 * Recycle the RXD with the helper WRITE_ONCE that can ensure
			 * data written into RAM would really happens.
			 */
			WRITE_ONCE(rxd->desc3, 0);
			WRITE_ONCE(rxd->desc2, 0);
			pc->ring.cur_rptr = next;

			/* Release rooms */
			atomic_inc(&(pc->nr_free));
		}

		/* Ensure all changes indeed done before we're going on */
		wmb();

		ecnt_dma_write(hsdma, pc->rx_cpu, pc->ring.cur_rptr);

		/*
		 * Acking the pending IRQ allows hardware no longer to keep the used
		 * IRQ line in certain trigger state when software has completed all
		 * the finished physical descriptors.
		 */
		if (atomic_read(&(pc->nr_free)) >= ECNT_DMA_SIZE - 1)
			ecnt_dma_write(hsdma, ECNT_HSDMA_INT_STATUS, pc->intr);

		/* ASAP handles pending VDs in all VCs after freeing some rooms */
		for (i = 0; i < hsdma->dma_requests; i++)
		{
			hvc = &(hsdma->vc[i]);
			spin_lock(&(hvc->vc.lock));
			ecnt_hsdma_issue_vchan_pending(hsdma, hvc, i);
			spin_unlock(&(hvc->vc.lock));
		}
	}

rx_done:
	/* All completed PDs are cleaned up, so enable interrupt again */
	ecnt_dma_set(hsdma, ECNT_HSDMA_INT_ENABLE, (ECNT_HSDMA_INT_RXDONE | ECNT_HSDMA_INT_RXDONE_1));
}

static irqreturn_t ecnt_hsdma_irq(int irq, void *devid)
{
	struct ecnt_hsdma_device *hsdma = devid;

	if (test_irq_handler != NULL)
	{
		test_irq_handler();
	}
	else
	{
		/*
		 * Disable interrupt until all completed PDs are cleaned up in
		 * ecnt_hsdma_free_rooms call.
		 */
		ecnt_dma_clr(hsdma, ECNT_HSDMA_INT_ENABLE, (ECNT_HSDMA_INT_RXDONE | ECNT_HSDMA_INT_RXDONE_1));

		ecnt_hsdma_free_rooms_in_ring(hsdma);
	}

	return IRQ_HANDLED;
}

static struct virt_dma_desc *ecnt_hsdma_find_active_desc(struct dma_chan *c,
					dma_cookie_t cookie)
{
	struct ecnt_hsdma_vchan *hvc = NULL;
	struct virt_dma_desc *vd = NULL;

	if (c != NULL)
	{
		hvc = to_hsdma_vchan(c);

		list_for_each_entry(vd, &(hvc->desc_hw_processing), node)
			if (vd->tx.cookie == cookie)
				return vd;

		list_for_each_entry(vd, &(hvc->vc.desc_issued), node)
			if (vd->tx.cookie == cookie)
				return vd;
	}

	return NULL;
}

static enum dma_status ecnt_hsdma_tx_status(struct dma_chan *c,
					dma_cookie_t cookie,
					struct dma_tx_state *txstate)
{
	struct ecnt_hsdma_vchan *hvc = NULL;
	struct ecnt_hsdma_vdesc *hvd = NULL;
	struct virt_dma_desc *vd = NULL;
	enum dma_status ret = 0;
	unsigned long flags = 0;
	size_t bytes = 0;

	if (c != NULL)
	{
		hvc = to_hsdma_vchan(c);
		ret = dma_cookie_status(c, cookie, txstate);
		if (ret == DMA_COMPLETE || !txstate)
			return ret;

		spin_lock_irqsave(&(hvc->vc.lock), flags);
		vd = ecnt_hsdma_find_active_desc(c, cookie);
		spin_unlock_irqrestore(&(hvc->vc.lock), flags);

		if (vd)
		{
			hvd = to_hsdma_vdesc(vd);
			bytes = hvd->residue;
		}

		dma_set_residue(txstate, bytes);
	}

	return ret;
}

static void ecnt_hsdma_issue_pending(struct dma_chan *c)
{
	struct ecnt_hsdma_device *hsdma = NULL;
	struct ecnt_hsdma_vchan *hvc = NULL;
	unsigned long flags = 0;

	if (c != NULL)
	{
		hsdma = to_hsdma_dev(c);
		hvc = to_hsdma_vchan(c);

		spin_lock_irqsave(&(hvc->vc.lock), flags);

		if (vchan_issue_pending(&(hvc->vc)))
			ecnt_hsdma_issue_vchan_pending(hsdma, hvc, c->chan_id);

		spin_unlock_irqrestore(&(hvc->vc.lock), flags);
	}
}

static struct dma_async_tx_descriptor *
ecnt_hsdma_prep_dma_memcpy(struct dma_chan *c, dma_addr_t dest,
					dma_addr_t src, size_t len, unsigned long flags)
{
	struct ecnt_hsdma_vdesc *hvd = NULL;

	if (c != NULL)
	{
		hvd = kzalloc(sizeof(*hvd), GFP_KERNEL);
		if (!hvd)
			return NULL;

		hvd->len = len;
		hvd->residue = len;
		hvd->src = src;
		hvd->dest = dest;

		return vchan_tx_prep(to_virt_chan(c), &(hvd->vd), flags);
	}
	else
		return NULL;
}

static void ecnt_hsdma_free_all_desc(struct dma_chan *c)
{
	struct ecnt_hsdma_vchan *hvc = NULL;
	bool sync_needed = false;

	if (c != NULL)
	{
		hvc = to_hsdma_vchan(c);
		/*
		 * Once issue_synchronize is being set, which means once the hardware
		 * consumes all descriptors for the channel in the ring, the
		 * synchronization must be be notified immediately it is completed.
		 */
		spin_lock(&(hvc->vc.lock));
		if (!list_empty(&(hvc->desc_hw_processing)))
		{
			hvc->issue_synchronize = true;
			sync_needed = true;
		}
		spin_unlock(&(hvc->vc.lock));

		if (sync_needed)
			wait_for_completion(&(hvc->issue_completion));
		/*
		 * At the point, we expect that all remaining descriptors in the ring
		 * for the channel should be all processing done.
		 */
		WARN_ONCE(!list_empty(&(hvc->desc_hw_processing)),
			  "Desc pending still in list desc_hw_processing\n");

		/* TO DO */
		vchan_free_chan_resources(to_virt_chan(c));

		WARN_ONCE(!list_empty(&(hvc->vc.desc_completed)),
			  "Desc pending still in list desc_completed\n");
	}
}

static int ecnt_hsdma_terminate_all(struct dma_chan *c)
{
	/* TO DO */
	ecnt_hsdma_free_all_desc(c);

	return 0;
}

static int ecnt_hsdma_alloc_chan_resources(struct dma_chan *c)
{
	struct ecnt_hsdma_device *hsdma = NULL;
	int err = 0;

	if (c != NULL)
	{
		hsdma = to_hsdma_dev(c);

		if (!atomic_read(&(hsdma->pc_refcnt)))
		{
			atomic_set(&(hsdma->pc_refcnt), 1);
		}
		else
		{
			atomic_inc(&(hsdma->pc_refcnt));
		}
	}

	return err;
}

static void ecnt_hsdma_free_chan_resources(struct dma_chan *c)
{
	struct ecnt_hsdma_device *hsdma = NULL;

	if (c != NULL)
	{
		hsdma = to_hsdma_dev(c);

		ecnt_hsdma_terminate_all(c);

		atomic_dec(&(hsdma->pc_refcnt));
	}
}

static int ecnt_hsdma_hw_init(struct ecnt_hsdma_device *hsdma)
{
	int i = 0, j = 0;
	int err = 0;

	ecnt_dma_write(hsdma, ECNT_HSDMA_FRQ_THR, 0);
	ecnt_dma_write(hsdma, ECNT_HSDMA_INT_ENABLE, 0);
	ecnt_dma_write(hsdma, ECNT_HSDMA_GLO, ECNT_HSDMA_GLO_DEFAULT);

	for (i = 0; i < ECNT_HSDMA_NR_MAX_PCHANS; i++)
	{
		err = ecnt_hsdma_alloc_pchan(hsdma, &(hsdma->pc[i]), i);

		if (err)
		{
			for (j = 0; j < i; j++)
			{
				ecnt_hsdma_free_pchan(hsdma, &(hsdma->pc[j]));
			}
			ecnt_dma_write(hsdma, ECNT_HSDMA_GLO, 0);
			return err;
		}
	}

	return 0;
}

static int ecnt_hsdma_hw_deinit(struct ecnt_hsdma_device *hsdma)
{
	int i = 0;

	ecnt_dma_write(hsdma, ECNT_HSDMA_GLO, 0);
	for (i = 0; i < ECNT_HSDMA_NR_MAX_PCHANS; i++)
	{
		ecnt_hsdma_free_pchan(hsdma, &(hsdma->pc[i]));
	}

	return 0;
}

static int hsdma_reg_dump(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	ecnt_hsdma_reg_dump();
    return 0;
}

static int ecnt_hsdma_probe(struct platform_device *pdev)
{
	struct ecnt_hsdma_device *hsdma = NULL;
	struct ecnt_hsdma_vchan *vc = NULL;
	struct dma_device *dd = NULL;
	struct resource *res = NULL;
	int i = 0, err = 0;

	hsdma = devm_kzalloc(&(pdev->dev), sizeof(*hsdma), GFP_KERNEL);
	if (!hsdma)
		return -ENOMEM;

	dd = &(hsdma->ddev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	hsdma->base = devm_ioremap_resource(&(pdev->dev), res);
	if (IS_ERR(hsdma->base))
	{
		err = PTR_ERR(hsdma->base);
		goto err_free_hsdma;
	}

	hsdma->soc = of_device_get_match_data(&(pdev->dev));
	if (!hsdma->soc)
	{
		dev_err(&(pdev->dev), "No device match found\n");
		err = -ENODEV;
		goto err_iounmap;
	}

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res)
	{
		dev_err(&(pdev->dev), "No irq resource for %s\n", dev_name(&(pdev->dev)));
		err =  -EINVAL;
		goto err_free_hsdma;
	}
	hsdma->irq = res->start;

	atomic_set(&(hsdma->pc_refcnt), 0);

	dma_cap_set(DMA_MEMCPY, dd->cap_mask);

	dd->copy_align = ECNT_HSDMA_ALIGN_SIZE;
	dd->device_alloc_chan_resources = ecnt_hsdma_alloc_chan_resources;
	dd->device_free_chan_resources = ecnt_hsdma_free_chan_resources;
	dd->device_tx_status = ecnt_hsdma_tx_status;
	dd->device_issue_pending = ecnt_hsdma_issue_pending;
	dd->device_prep_dma_memcpy = ecnt_hsdma_prep_dma_memcpy;
	dd->device_terminate_all = ecnt_hsdma_terminate_all;
	dd->src_addr_widths = ECNT_HSDMA_DMA_BUSWIDTHS;
	dd->dst_addr_widths = ECNT_HSDMA_DMA_BUSWIDTHS;
	dd->directions = BIT(DMA_MEM_TO_MEM) | BIT(DMA_MEM_TO_DEV) | BIT(DMA_DEV_TO_MEM);
	dd->residue_granularity = DMA_RESIDUE_GRANULARITY_SEGMENT;
	dd->dev = &(pdev->dev);
	INIT_LIST_HEAD(&(dd->channels));

	hsdma->dma_requests = ECNT_HSDMA_NR_VCHANS;
	if (pdev->dev.of_node && of_property_read_u32(pdev->dev.of_node, "dma-requests", &(hsdma->dma_requests)))
	{
		dev_info(&(pdev->dev), "Using %u as missing dma-requests property\n", ECNT_HSDMA_NR_VCHANS);
	}

	hsdma->pc = devm_kcalloc(&(pdev->dev), ECNT_HSDMA_NR_MAX_PCHANS, sizeof(*hsdma->pc), GFP_KERNEL);
	if (!hsdma->pc)
	{
		err = -ENOMEM;
		goto err_free_hsdma;
	}

	hsdma->vc = devm_kcalloc(&(pdev->dev), hsdma->dma_requests, sizeof(*hsdma->vc), GFP_KERNEL);
	if (!hsdma->vc)
	{
		err = -ENOMEM;
		goto err_free_pc;
	}

	for (i = 0; i < ECNT_HSDMA_NR_MAX_PCHANS; i++)
	{
		spin_lock_init(&(hsdma->pc[i].lock));
	}

	for (i = 0; i < hsdma->dma_requests; i++)
	{
		vc = &(hsdma->vc[i]);
		vc->vc.desc_free = ecnt_hsdma_vdesc_free;
		vchan_init(&(vc->vc), dd);
		init_completion(&(vc->issue_completion));
		INIT_LIST_HEAD(&(vc->desc_hw_processing));
	}

	err = dma_async_device_register(dd);
	if (err)
		goto err_free_vc;

	err = of_dma_controller_register(pdev->dev.of_node, of_dma_xlate_by_chan_id, hsdma);
	if (err)
	{
		dev_err(&(pdev->dev), "Econet HSDMA OF registration failed %d\n", err);
		goto err_unregister;
	}

	err = ecnt_hsdma_hw_init(hsdma);
	if (err)
	{
		dev_err(&(pdev->dev), "allocate resource err %d\n", err);
		goto err_free_of;
	}

	err = devm_request_irq(&(pdev->dev), hsdma->irq, ecnt_hsdma_irq, 0, dev_name(&(pdev->dev)), hsdma);
	if (err)
	{
		dev_err(&(pdev->dev), "request_irq failed with err %d\n", err);
		goto err_hw_deinit;
	}

	proc_hsdma_reg = create_proc_entry("tc3162/hsdma_reg_dump", 0, NULL);
	if (!proc_hsdma_reg)
	{
		dev_err(&(pdev->dev), "create proc for hsdma_reg_dump filed\n");
		devm_free_irq(&(pdev->dev), hsdma->irq, hsdma);
		err =  -ENOMEM;
		goto err_hw_deinit;
	}
	proc_hsdma_reg->read_proc = hsdma_reg_dump;

	platform_set_drvdata(pdev, hsdma);
	dev_info(&(pdev->dev), "Econet HSDMA driver registered\n");

	g_hsdma = hsdma;

	return 0;

err_hw_deinit:
	ecnt_hsdma_hw_deinit(hsdma);
err_free_of:
	of_dma_controller_free(pdev->dev.of_node);
err_unregister:
	dma_async_device_unregister(dd);
err_free_vc:
	devm_kfree(&(pdev->dev), hsdma->vc);
err_free_pc:
	devm_kfree(&(pdev->dev), hsdma->pc);
err_iounmap:
	devm_iounmap(&(pdev->dev), hsdma->base);
err_free_hsdma:
	devm_kfree(&(pdev->dev), hsdma);

	return err;
}

static int ecnt_hsdma_remove(struct platform_device *pdev)
{
	struct ecnt_hsdma_device *hsdma = platform_get_drvdata(pdev);
	struct ecnt_hsdma_vchan *vc = NULL;
	int i = 0;

	/* Kill VC task */
	for (i = 0; i < hsdma->dma_requests; i++) {
		vc = &(hsdma->vc[i]);

		list_del(&(vc->vc.chan.device_node));
		tasklet_kill(&(vc->vc.task));
	}

	/* Disable DMA interrupt */
	ecnt_dma_write(hsdma, ECNT_HSDMA_INT_ENABLE, 0);

	if (proc_hsdma_reg)
		remove_proc_entry("tc3162/hsdma_reg_dump", NULL);

	/* Waits for any pending IRQ handlers to complete */
	synchronize_irq(hsdma->irq);
	devm_free_irq(&(pdev->dev), hsdma->irq, hsdma);

	/* Disable hardware */
	ecnt_hsdma_hw_deinit(hsdma);
	of_dma_controller_free(pdev->dev.of_node);
	dma_async_device_unregister(&(hsdma->ddev));
	devm_kfree(&(pdev->dev), hsdma->vc);
	devm_kfree(&(pdev->dev), hsdma->pc);
	devm_iounmap(&(pdev->dev), hsdma->base);
	devm_kfree(&(pdev->dev), hsdma);

	dev_info(&(pdev->dev), "Econet HSDMA driver unregistered\n");
	return 0;
}

static const struct ecnt_hsdma_soc ecnt_soc = {
	.ddone		= BIT(31),
	.nls		= BIT(29),
};

static const struct of_device_id ecnt_hsdma_match[] = {
	{ .compatible = "econet,ecnt-hsdma", .data = &ecnt_soc},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ecnt_hsdma_match);

static struct platform_driver ecnt_hsdma_driver = {
	.probe		= ecnt_hsdma_probe,
	.remove		= ecnt_hsdma_remove,
	.driver		= {
		.name			= "ecnt-hsdma",
		.of_match_table	= ecnt_hsdma_match,
	},
};

static int ecnt_hsdma_init(void)
{
	return platform_driver_register(&ecnt_hsdma_driver);
}
subsys_initcall(ecnt_hsdma_init);

static void ecnt_hsdma_exit(void)
{
	platform_driver_unregister(&ecnt_hsdma_driver);
}
module_exit(ecnt_hsdma_exit);

MODULE_ALIAS("platform:ecnt-hsdma");
MODULE_DESCRIPTION("ECONET High-Speed DMA Controller Driver");
MODULE_LICENSE("GPL v2");
