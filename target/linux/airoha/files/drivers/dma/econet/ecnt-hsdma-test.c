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
#include <linux/random.h>
#include <linux/uaccess.h>
#include <linux/smp.h>
#include <linux/module.h>
#include <linux/moduleparam.h>


#include <asm/tc3162/tc3162.h>
#include "../virt-dma.h"
#include "ecnt-hsdma.h"

#define SIZE_2M		0x200000
#define SIZE_384K	0x60000
#define SIZE_192K	0x30000
#define SIZE_4K		0x1000
#define NSRAM_PHYS	0x1E800000
#define TIMER_PHYS	0x1FBF0100
#define TIMEOUT		0x1000000

struct hsdma_test_req
{
	u8 *src_array0;
	u8 *dst_array0;
	u8 *src_array1;
	u8 *dst_array1;
	dma_addr_t src_addr0;
	dma_addr_t dst_addr0;
	dma_addr_t src_addr1;
	dma_addr_t dst_addr1;
	u32 len0;
	u32 len1;
};

struct channel_data
{
	spinlock_t page_lock;
	u32 hsdma_rx_dma_owner_idx;
	u32 hsdma_rx_calc_idx;
	u32 hsdma_tx_cpu_owner_idx;
	u32 hsdma_tx_cpu_idx;
};

struct hsdma_test_global
{
	dma_addr_t src_phys0;
	dma_addr_t dst_phys0;
	dma_addr_t src_phys1;
	dma_addr_t dst_phys1;
	u8 *src_unc0;
	u8 *dst_unc0;
	u8 *src_unc1;
	u8 *dst_unc1;
	void __iomem *nsram_base;
	void __iomem *timer_base;
	struct proc_dir_entry *proc_hsdma_test;
	struct channel_data chan[2];
};

extern void (*test_irq_handler)(void);

//#define HSDMA_CPU_BUS_TEST
#ifdef HSDMA_CPU_BUS_TEST
/* for hadms block cnt test in modules/private/tcci/cpu_bus_test.c */
extern void (*set_hsdma_block_test_hook)(dma_addr_t, dma_addr_t, u32, u32);
extern void (*enable_hsdma_by_txCpu_hook)(void);
extern int (*wait_hsdma_done_hook)(void);

volatile struct ecnt_hsdma_pdesc *g_rxd = NULL;
volatile struct hsdma_test_req t_req;
static int hsdma_cpu_bus_t_stage=0;
#endif

static __le32 ddone = 0;
static __le32 nls = 0;

static u32 buf_size = SIZE_2M;
static u32 old_cfg = 0;
static u32 old_intr = 0;
static u32 old_intr_dly = 0;

static struct hsdma_test_global hg;

static u8 quite = 0;
module_param_cb(quite, &param_ops_byte, &quite, (S_IRUGO | S_IWUSR));
static u8 hsdma_pattern_data = 0;
module_param_cb(hsdma_pattern_data, &param_ops_byte, &hsdma_pattern_data, (S_IRUGO | S_IWUSR));
static u16 hsdma_pkt_len = ECNT_HSDMA_MAX_LEN;
module_param_cb(hsdma_pkt_len, &param_ops_ushort, &hsdma_pkt_len, (S_IRUGO | S_IWUSR));
static u32 hsdma_test_length = 0;
module_param_cb(hsdma_test_length, &param_ops_uint, &hsdma_test_length, (S_IRUGO | S_IWUSR));

u32 bswap32(u32 data, int swap_flag)
{
	u32 data_swap = 0;
	if (swap_flag)
	{
		data_swap = (((data & 0x000000FF) << 24)	|
					 ((data & 0x0000FF00) << 8)		|
					 ((data & 0x00FF0000) >>8)		|
					 ((data & 0xFF000000) >> 24));
		return data_swap;
	}
	else
		return data;
}

int comapre(u8 *src_array, u8 *dst_array, u32 len)
{
	int i = 0;
	printk("***compare src (0x%p) des(0x%p) len(0x%x)***\n",
						src_array, dst_array, len);
	for(i = 0; i < len; i++)
	{
		if(src_array[i] != dst_array[i])
		{
			printk("0x%p(0x%x) != 0x%p(0x%x)\n",
								src_array, src_array[i],
								dst_array, dst_array[i]);
			return (i + 1);
		}
	}
	return 0;
}

int dump(void *addr, unsigned int len)
{
	register int n = 0, m = 0, r = 0;
	unsigned char temp[16] = {0};

	printk("\r\n[Addr=0x%p Length=0x%x]\r\n", addr, len);
	for ( n = len; n > 0; )
	{
		printk("%p ", addr);
		r = n < 16? n: 16;
		memcpy((void *) temp, (void *) addr, r);
		addr += r;
		for ( m = 0; m < r; ++m )
		{
			printk("%c", (m & 3) == 0 && m > 0? '.': ' ');
			printk("%.2x", temp[m]);
		}
		for (; m < 16; ++m)
			printk("   ");
		n -= r;
		printk("\n");
	}

	printk("\n");
	return 0;
}

void reset_cpu_index(void)
{
	ecnt_hsdma_stop_dma(ECNT_HSDMA_TX | ECNT_HSDMA_RX);
	ecnt_hsdma_reset();

	hg.chan[0].hsdma_rx_calc_idx = ecnt_hsdma_get_idx(0, ECNT_HSDMA_RX, ECNT_HSDMA_CPU);
	hg.chan[0].hsdma_rx_dma_owner_idx = ecnt_hsdma_get_idx(0, ECNT_HSDMA_RX, ECNT_HSDMA_DMA);
	hg.chan[1].hsdma_rx_calc_idx = ecnt_hsdma_get_idx(1, ECNT_HSDMA_RX, ECNT_HSDMA_CPU);
	hg.chan[1].hsdma_rx_dma_owner_idx = ecnt_hsdma_get_idx(1, ECNT_HSDMA_RX, ECNT_HSDMA_DMA);
	hg.chan[0].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(0, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);
	hg.chan[1].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(1, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);

	ecnt_hsdma_start_dma(ECNT_HSDMA_TX | ECNT_HSDMA_RX);

	wmb();
}

void hsdma_prepare_desc(struct ecnt_hsdma_ring *ring, u32 *testLen, int chan_num,
		dma_addr_t src_addr, dma_addr_t dst_addr, u32 NLS_bit, int swap_flag, int i)
{
	volatile struct ecnt_hsdma_pdesc *txd = NULL, *rxd = NULL;
	__le32 desc2 = 0;

	desc2 = 0;
	if (*testLen > hsdma_pkt_len)
	{
		desc2 |= ECNT_HSDMA_DESC_PLEN(hsdma_pkt_len);
		*testLen -= hsdma_pkt_len;
	}
	else
	{
		desc2 |= ECNT_HSDMA_DESC_PLEN(*testLen);
		*testLen = 0;
	}
	if (NLS_bit)
		desc2 |= nls;

	hg.chan[chan_num].hsdma_rx_dma_owner_idx = (hg.chan[chan_num].hsdma_rx_calc_idx + 1) % ECNT_DMA_SIZE;

	txd = &(ring->txd[hg.chan[chan_num].hsdma_tx_cpu_owner_idx]);
	rxd = &(ring->rxd[hg.chan[chan_num].hsdma_rx_dma_owner_idx]);

	WARN(!(txd->desc2 & ddone),
		  "TXD done bit is 0\n");
	WARN((rxd->desc2 & ddone),
		  "RXD done bit is 1\n");

	WRITE_ONCE(rxd->desc3, bswap32((dst_addr + (hsdma_pkt_len * i)), swap_flag));
	WRITE_ONCE(rxd->desc2, bswap32(desc2, swap_flag));
	WRITE_ONCE(txd->desc3, bswap32((src_addr + (hsdma_pkt_len * i)), swap_flag));
	WRITE_ONCE(txd->desc2, bswap32(desc2, swap_flag));

	hg.chan[chan_num].hsdma_tx_cpu_idx = hg.chan[chan_num].hsdma_tx_cpu_owner_idx;
	hg.chan[chan_num].hsdma_tx_cpu_owner_idx = ((hg.chan[chan_num].hsdma_tx_cpu_owner_idx + 1) % ECNT_DMA_SIZE);
	hg.chan[chan_num].hsdma_rx_calc_idx = ((hg.chan[chan_num].hsdma_rx_calc_idx + 1) % ECNT_DMA_SIZE);

	WARN((txd->desc2 & ddone),
		  "TXD done bit is 1\n");

	if (swap_flag)
	{
		printk("ECNT_HSDMA_GLO %08x\n", ecnt_hsdma_get_cfg());
        printk("Rx Descript [%d] desc2 = %08x desc3 = %08x\n", hg.chan[chan_num].hsdma_rx_dma_owner_idx, rxd->desc2, rxd->desc3);
        printk("Tx Descript [%d] desc2 = %08x desc3 = %08x\n", hg.chan[chan_num].hsdma_tx_cpu_idx, txd->desc2, txd->desc3);
	}
}

int HS_DmaMem2Mem(volatile struct hsdma_test_req *p_req, u32 NLS_bit, int chan_num)
{
	struct ecnt_hsdma_ring *ring = ecnt_hsdma_get_ring(chan_num);
	volatile struct ecnt_hsdma_pdesc *txd = NULL, *rxd = NULL;
	unsigned long flags;
	int i = 0;
	u32 timeout = TIMEOUT;
	u32	testLen = 0;
	dma_addr_t src_addr = 0, dst_addr = 0;

  #ifdef HSDMA_CPU_BUS_TEST
  if (hsdma_cpu_bus_t_stage==0) {
  #endif

	if (chan_num == 0)
	{
		testLen = p_req->len0;
		src_addr = p_req->src_addr0;
		dst_addr = p_req->dst_addr0;
	}
	else if (chan_num == 1)
	{
		testLen = p_req->len1;
		src_addr = p_req->src_addr1;
		dst_addr = p_req->dst_addr1;
	}
	else
		return 0;

	spin_lock_irqsave(&(hg.chan[chan_num].page_lock), flags);
	hg.chan[chan_num].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(chan_num, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);
	while(testLen)
	{
		hsdma_prepare_desc(ring, &testLen, chan_num, src_addr, dst_addr, NLS_bit, 0, i);
		i++;
		wmb();
	}

  #ifdef HSDMA_CPU_BUS_TEST
    g_rxd = &(ring->rxd[hg.chan[chan_num].hsdma_rx_dma_owner_idx]);
    spin_unlock_irqrestore(&(hg.chan[chan_num].page_lock), flags);
    return 0;
  }
  else if (hsdma_cpu_bus_t_stage==1) {
  #endif
    
	ecnt_hsdma_set_idx(chan_num, ECNT_HSDMA_TX, ECNT_HSDMA_CPU, hg.chan[chan_num].hsdma_tx_cpu_owner_idx);
  
  #ifdef HSDMA_CPU_BUS_TEST
    return 0;
  }
  else { /* hsdma_cpu_bus_t_stage==2 */
  #endif

	txd = &(ring->txd[hg.chan[chan_num].hsdma_tx_cpu_idx]);
	rxd = &(ring->rxd[hg.chan[chan_num].hsdma_rx_dma_owner_idx]);
    #ifdef HSDMA_CPU_BUS_TEST
    rxd = g_rxd;
    #endif
	do
	{
		timeout--;
	} while (!(rxd->desc2 & ddone) && timeout);

	if (!timeout)
	{
		printk("Wait Rx%d ddone bit time out\n", chan_num);
	}
	ecnt_hsdma_set_idx(chan_num, ECNT_HSDMA_RX, ECNT_HSDMA_CPU, hg.chan[chan_num].hsdma_rx_calc_idx);

  #ifdef HSDMA_CPU_BUS_TEST
    if (timeout)
        return 0; /* done */
    else
        return -1; /* timeout */
  }
  #endif
    
	spin_unlock_irqrestore(&(hg.chan[chan_num].page_lock), flags);

	if (!quite)
	{
		dump((void *) txd, 0x20);
		dump((void *) rxd, 0x20);
	}
	return 0;
}

int HS_DmaMem2Mem_loop(volatile struct hsdma_test_req *p_req, int loop, int NLS_bit)

{
	struct ecnt_hsdma_ring *ring = ecnt_hsdma_get_ring(0);
	struct ecnt_hsdma_ring *ring1 = ecnt_hsdma_get_ring(1);
	volatile struct ecnt_hsdma_pdesc *rxd = NULL, *rxd1 = NULL;
	unsigned long flags;
	int i = 0;
	int count = 0;
	int timeout = TIMEOUT;
	u32 testLen = 0;

	printk("\n HS_DmaMem2Mem loop>>\n\n");
	printk("Src 0x%p Dst 0x%p len 0x%x\n", p_req->src_array0, p_req->dst_array0, p_req->len0);
	printk("Src 0x%p Dst 0x%p len 0x%x\n", p_req->src_array1, p_req->dst_array1, p_req->len1);
	printk("loop %d\n",loop);

	ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
	for(count = 0; count < loop; count++)
	{
		if(NLS_bit == 0)
		{
			prandom_bytes(p_req->src_array0, p_req->len0);
			prandom_bytes(p_req->src_array1, p_req->len1);
		}
		else
		{
			memset(p_req->src_array0, 0x5A, p_req->len0);
			memset(p_req->src_array1, 0x5A, p_req->len1);
		}
		wmb();
		spin_lock_irqsave(&(hg.chan[0].page_lock), flags);
		reset_cpu_index();
		i = 0;
		testLen = p_req->len0;
		hg.chan[0].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(0, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);
		while(testLen)
		{
			hsdma_prepare_desc(ring, &testLen, 0, p_req->src_addr0, p_req->dst_addr0, NLS_bit, 0, i);
			i++;
			wmb();
		}

		i = 0;
		testLen = p_req->len1;
		hg.chan[1].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(1, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);
		while(testLen)
		{
			hsdma_prepare_desc(ring1, &testLen, 1, p_req->src_addr1, p_req->dst_addr1, NLS_bit, 0, i);
			i++;
			wmb();
		}
		ecnt_hsdma_set_idx(0, ECNT_HSDMA_TX, ECNT_HSDMA_CPU, hg.chan[0].hsdma_tx_cpu_owner_idx);
		ecnt_hsdma_set_idx(1, ECNT_HSDMA_TX, ECNT_HSDMA_CPU, hg.chan[1].hsdma_tx_cpu_owner_idx);

		rxd = &(ring->rxd[hg.chan[0].hsdma_rx_dma_owner_idx]);
		rxd1 = &(ring->rxd[hg.chan[1].hsdma_rx_dma_owner_idx]);
		do
		{
			timeout--;
		} while (!((rxd->desc2 & ddone) && (rxd1->desc2 & ddone))&& timeout);

		if (!timeout)
		{
			if (!(rxd->desc2 & ddone))
				printk("Wait Rx0 ddone bit time out\n");
			if (!(rxd1->desc2 & ddone))
				printk("Wait Rx0 ddone bit time out\n");
		}
		timeout = TIMEOUT;
		ecnt_hsdma_set_idx(0, ECNT_HSDMA_RX, ECNT_HSDMA_CPU, hg.chan[0].hsdma_rx_calc_idx);
		ecnt_hsdma_set_idx(1, ECNT_HSDMA_RX, ECNT_HSDMA_CPU, hg.chan[1].hsdma_rx_calc_idx);
		spin_unlock_irqrestore(&(hg.chan[0].page_lock), flags);

		//compare copy result
		if ((count + 1) % 100 == 0)
			printk(". ");

		for (i = 0; i < p_req->len0; i++)
		{
			if (p_req->src_array0[i] != p_req->dst_array0[i])
			{
				printk("channel 0 compare copy data err count %d\n", count);
				printk("***compare src (0x%p) des(0x%p) len(0x%x)***\n",
									p_req->src_array0, p_req->dst_array0, p_req->len0);
				printk("0x%p(0x%x) != 0x%p(0x%x)\n",
									(p_req->src_array0 + i), p_req->src_array0[i],
									(p_req->dst_array0 + i), p_req->dst_array0[i]);

				dump(p_req->src_array0, p_req->len0);
				dump(p_req->dst_array0, p_req->len0);
				goto stop;
			}
		}
		for (i = 0; i < p_req->len1; i++)
		{
			if (p_req->src_array1[i] != p_req->dst_array1[i])
			{
				printk("channel 1 compare copy data err count %d\n", count);
				printk("***compare src (0x%p) des(0x%p) len(0x%x)***\n",
									p_req->src_array1, p_req->dst_array1, p_req->len1);
				printk("0x%p(0x%x) != 0x%p(0x%x)\n",
									(p_req->src_array1 + i), p_req->src_array1[i],
									(p_req->dst_array1 + i), p_req->dst_array1[i]);

				dump(p_req->src_array1, p_req->len1);
				dump(p_req->dst_array1, p_req->len1);
				goto stop;
			}
		}
	}

stop:
	ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);

	return 0;
}

int HS_DmaMem2Mem_swap(volatile struct hsdma_test_req *p_req, int swap_flag)
{
	struct ecnt_hsdma_ring *ring = ecnt_hsdma_get_ring(0);
	volatile struct ecnt_hsdma_pdesc *txd = NULL, *rxd = NULL;
	unsigned long flags;
	int i = 0;
	u32 timeout = TIMEOUT;
	u32 testLen = 0;

	spin_lock_irqsave(&(hg.chan[0].page_lock), flags);
	i = 0;
	testLen = p_req->len0;
	hg.chan[0].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(0, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);
	while(testLen)
	{
		hsdma_prepare_desc(ring, &testLen, 0, p_req->src_addr0, p_req->dst_addr0, 0, swap_flag, i);
		i++;
		wmb();
	}
	ecnt_hsdma_set_idx(0, ECNT_HSDMA_TX, ECNT_HSDMA_CPU, hg.chan[0].hsdma_tx_cpu_owner_idx);

	txd = &(ring->txd[hg.chan[0].hsdma_tx_cpu_idx]);
	rxd = &(ring->rxd[hg.chan[0].hsdma_rx_dma_owner_idx]);
	do
	{
		timeout--;
	} while (!(rxd->desc2 & bswap32(ddone, swap_flag)) && timeout);

	if (!timeout)
	{
		printk("Wait Rx0 ddone bit time out\n");
	}
	ecnt_hsdma_set_idx(0, ECNT_HSDMA_RX, ECNT_HSDMA_CPU, hg.chan[0].hsdma_rx_calc_idx);
	spin_unlock_irqrestore(&(hg.chan[0].page_lock), flags);

	if (!quite)
	{
		dump((void *) txd, 0x20);
		dump((void *) rxd, 0x20);
	}

	return 0;
}

int HS_DmaMem2Mem_time_measurement(volatile struct hsdma_test_req *p_req, int chan_num)
{
	struct ecnt_hsdma_ring *ring = ecnt_hsdma_get_ring(chan_num);
	volatile struct ecnt_hsdma_pdesc *rxd = NULL;
	unsigned long flags;
	int i = 0;
	u32 timeout = TIMEOUT;
	u32	testLen = 0;
	dma_addr_t src_addr = 0, dst_addr = 0;
	unsigned long count = 0;
	unsigned long c1[0x20] = {0};
	unsigned long c2[0x20] = {0};
	unsigned long max_c = ~(0UL);

	printk("ECNT_HSDMA_GLO %08x\n", ecnt_hsdma_get_cfg());
	spin_lock_irqsave(&(hg.chan[chan_num].page_lock), flags);
	for(count = 0; count < 0x20; count++)
	{
		reset_cpu_index();
		i = 0;
		if (chan_num == 0)
		{
			prandom_bytes(p_req->src_array0, p_req->len0);
			testLen = p_req->len0;
			src_addr = p_req->src_addr0;
			dst_addr = p_req->dst_addr0;
		}
		else if (chan_num == 1)
		{
			prandom_bytes(p_req->src_array1, p_req->len1);
			testLen = p_req->len1;
			src_addr = p_req->src_addr1;
			dst_addr = p_req->dst_addr1;
		}
		else
			return 0;

		wmb();
		hg.chan[chan_num].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(chan_num, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);
		while(testLen)
		{
			hsdma_prepare_desc(ring, &testLen, chan_num, src_addr, dst_addr, 0, 0, i);
			i++;
			wmb();
		}
		c1[count] = read_c0_count();
		ecnt_hsdma_set_idx(chan_num, ECNT_HSDMA_TX, ECNT_HSDMA_CPU, hg.chan[chan_num].hsdma_tx_cpu_owner_idx);

		rxd = &(ring->rxd[hg.chan[chan_num].hsdma_rx_dma_owner_idx]);
		do
		{
			timeout--;
		} while (!(rxd->desc2 & ddone) && timeout);

		if (!timeout)
		{
			printk("Wait Rx%d ddone bit time out\n", chan_num);
		}
		c2[count] = read_c0_count();
		timeout = TIMEOUT;
		ecnt_hsdma_set_idx(chan_num, ECNT_HSDMA_RX, ECNT_HSDMA_CPU, hg.chan[chan_num].hsdma_rx_calc_idx);
	}
	spin_unlock_irqrestore(&(hg.chan[chan_num].page_lock), flags);

	count = 0;
    for(i = 0; i < 0x20; i++)
    {
		if (c2[i] >= c1[i])
		{
			count = count + c2[i] - c1[i];
		}
		else
		{
			count = count + (max_c - c1[i]) + c2[i];
		}
    }
	printk("execution count %lu c2[0]=0x%lx c1[0]=0x%lx\n", (count >> 5), c2[0], c1[0]);

	return 0;

}

int HS_DmaMem2Mem_coherent(volatile struct hsdma_test_req *p_req, int tx_coherent)
{
	struct ecnt_hsdma_ring *ring = ecnt_hsdma_get_ring(0);
	volatile struct ecnt_hsdma_pdesc *txd = NULL, *rxd = NULL;
	__le32 desc2 = 0;
	unsigned long flags;
	int i = 0;
	u32 timeout = TIMEOUT;
	u32 testLen = p_req->len0;
	u32	int_status = 0;

	if (tx_coherent)
		int_status = ECNT_HSDMA_INT_TXCOHER;
	else
		int_status = ECNT_HSDMA_INT_RXCOHER;

	spin_lock_irqsave(&(hg.chan[0].page_lock), flags);
	hg.chan[0].hsdma_tx_cpu_owner_idx = ecnt_hsdma_get_idx(0, ECNT_HSDMA_TX, ECNT_HSDMA_CPU);
	while(testLen)
	{
		desc2 = 0;
		if (testLen > hsdma_pkt_len)
		{
			desc2 |= ECNT_HSDMA_DESC_PLEN(hsdma_pkt_len);
			testLen -= hsdma_pkt_len;
		}
		else
		{
			desc2 |= ECNT_HSDMA_DESC_PLEN(testLen);
			testLen = 0;
		}

		hg.chan[0].hsdma_rx_dma_owner_idx = (hg.chan[0].hsdma_rx_calc_idx + 1) % ECNT_DMA_SIZE;

		txd = &(ring->txd[hg.chan[0].hsdma_tx_cpu_owner_idx]);
		rxd = &(ring->rxd[hg.chan[0].hsdma_rx_dma_owner_idx]);

		WARN(!(txd->desc2 & ddone),
			  "TXD done bit is 0\n");
		WARN((rxd->desc2 & ddone),
			  "RXD done bit is 1\n");

		if (tx_coherent)
			desc2 &= ~(ddone);
		else
			desc2 |= ddone;
		WRITE_ONCE(rxd->desc3, (p_req->dst_addr0 + (hsdma_pkt_len * i)));
		WRITE_ONCE(rxd->desc2, desc2);
		if (tx_coherent)
			desc2 |= ddone;
		else
			desc2 &= ~(ddone);
		WRITE_ONCE(txd->desc3, (p_req->src_addr0 + (hsdma_pkt_len * i)));
		WRITE_ONCE(txd->desc2, desc2);


		hg.chan[0].hsdma_tx_cpu_idx = hg.chan[0].hsdma_tx_cpu_owner_idx;
		hg.chan[0].hsdma_tx_cpu_owner_idx = ((hg.chan[0].hsdma_tx_cpu_owner_idx + 1) % ECNT_DMA_SIZE);
		hg.chan[0].hsdma_rx_calc_idx = ((hg.chan[0].hsdma_rx_calc_idx + 1) % ECNT_DMA_SIZE);
		i++;
		wmb();
	}
	ecnt_hsdma_set_idx(0, ECNT_HSDMA_TX, ECNT_HSDMA_CPU, hg.chan[0].hsdma_tx_cpu_owner_idx);

	txd = &(ring->txd[hg.chan[0].hsdma_tx_cpu_idx]);
	rxd = &(ring->rxd[hg.chan[0].hsdma_rx_dma_owner_idx]);
	do
	{
		timeout--;
	} while (!(ecnt_hsdma_get_intr_sts() & int_status) && timeout);

	if (!timeout)
	{
		printk("Wait coherent interrupt time out\n");
	}
	ecnt_hsdma_set_idx(0, ECNT_HSDMA_RX, ECNT_HSDMA_CPU, hg.chan[0].hsdma_rx_calc_idx);
	spin_unlock_irqrestore(&(hg.chan[0].page_lock), flags);

	dump((void *) txd, 0x20);
	dump((void *) rxd, 0x20);
	return 0;
}

#ifdef HSDMA_CPU_BUS_TEST
void set_hsdma_block_test(dma_addr_t src_phys0, dma_addr_t dst_phys0, u32 testLen, u32 bsize)
{
    reset_cpu_index();
    memset((void *) &t_req, 0, sizeof(struct hsdma_test_req));

    ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);

    if (bsize==0)
	    ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_16BYTES);
    else if (bsize==1)
        ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_32BYTES);
    else if (bsize==2)
        ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_64BYTES);
    else /*(bsize==3)*/
        ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_128BYTES);

    t_req.src_addr0 = src_phys0;
	t_req.dst_addr0 = dst_phys0;
	t_req.len0 = testLen;

    hsdma_cpu_bus_t_stage=0;
    HS_DmaMem2Mem(&t_req, 0, 0);

    return;
}

void enable_hsdma_by_txCpu(void)
{
    hsdma_cpu_bus_t_stage=1;
    HS_DmaMem2Mem(&t_req, 0, 0);
    return;
}

int wait_hsdma_done(void)
{
    hsdma_cpu_bus_t_stage=2;
    return HS_DmaMem2Mem(&t_req, 0, 0);
}
#endif

int hsdma_run_test(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	char valString[4] = {0};
	int type = 0;
	u32 src_offset = 0, dst_offset = 0;
	u32 testLen = SIZE_384K;
	volatile struct hsdma_test_req req;

	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buffer, count))
		return -EFAULT;

	valString[count] = '\0';

	sscanf(valString, "%d", &type);

	printk("\nHSDMA cpu%x test type: %d\n\n", smp_processor_id(), type);

	if(hsdma_test_length != 0)
	{
		testLen = hsdma_test_length;
	}

	reset_cpu_index();

	memset(hg.src_unc0, 0, buf_size);
	memset(hg.dst_unc0, 0xff, buf_size);
	memset((void *) &req, 0, sizeof(struct hsdma_test_req));

	if(hsdma_pattern_data == 0)
	{
		prandom_bytes(hg.src_unc0, buf_size);
	}
	else
	{
		memset(hg.src_unc0, hsdma_pattern_data, buf_size);
	}
	memcpy_toio(hg.nsram_base, hg.src_unc0, SIZE_384K);
	wmb();
	/* readback npu sram data */
	readb((hg.nsram_base + SIZE_384K - 1));
	rmb();

	quite = 0;
	enablePMU();
	enableCCNT();

	switch(type)
	{
		case 0:
			quite = 1;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("No Alignment Test Channel 0\n");
			for (src_offset = 0; src_offset < 4; src_offset++)
			{
				for (dst_offset = 0; dst_offset < 4; dst_offset++)
				{
					req.src_addr0 = (hg.src_phys0 + src_offset);
					req.dst_addr0 = (hg.dst_phys0 + dst_offset);
					req.src_array0 = (hg.src_unc0 + src_offset);
					req.dst_array0 = (hg.dst_unc0 + dst_offset);
					req.len0 = (testLen + src_offset + dst_offset);
					HS_DmaMem2Mem(&req, 0, 0);
					comapre(req.src_array0, req.dst_array0, req.len0);
				}
			}
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 1:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			printk("DRAM2DRAM\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
		break;
		case 2:
			testLen = SIZE_192K;
			req.src_addr0 = NSRAM_PHYS;
			req.dst_addr0 = NSRAM_PHYS + testLen;
			req.src_array0 = hg.nsram_base;
			req.dst_array0 = hg.nsram_base + testLen;
			req.len0 = testLen;
			printk("SRAM2SRAM\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
		break;
		case 3:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = NSRAM_PHYS;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.nsram_base;
			req.len0 = testLen;
			printk("DRAM2SRAM\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
		break;
		case 4:
			req.src_addr0 = NSRAM_PHYS;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.nsram_base;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			printk("SRAM2DRAM\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
		break;
		case 5:
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_TXCOHER);
			printk("TX Coherent\n");
			HS_DmaMem2Mem_coherent(&req, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 6:
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_RXCOHER);
			printk("RX Coherent\n");
			HS_DmaMem2Mem_coherent(&req, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 7:
			quite = 1;
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_TXDLY);
			ecnt_hsdma_enable_dly_intr(ECNT_HSDMA_TX, 5, 0);
			printk("TX Intr Delay by count\n");
			for (count = 0; count < 5; count ++)
			{
				req.src_addr0 += req.len0;
				req.dst_addr0 += req.len0;
				req.src_array0 += req.len0;
				req.dst_array0 += req.len0;
				HS_DmaMem2Mem(&req, 0, 0);
			}
			printk("TX Intr Delay by timeout\n");
			ecnt_hsdma_enable_dly_intr(ECNT_HSDMA_TX, 5, 0xFF);
			for (count = 0; count < 2; count ++)
			{
				req.src_addr0 += req.len0;
				req.dst_addr0 += req.len0;
				req.src_array0 += req.len0;
				req.dst_array0 += req.len0;
				HS_DmaMem2Mem(&req, 0, 0);
			}
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_disable_dly_intr(ECNT_HSDMA_TX);
		break;
		case 8:
			quite = 1;
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_RXDLY);
			ecnt_hsdma_enable_dly_intr(ECNT_HSDMA_RX, 5, 0);
			printk("RX Intr Delay by count\n");
			for (count = 0; count < 5; count ++)
			{
				req.src_addr0 += req.len0;
				req.dst_addr0 += req.len0;
				req.src_array0 += req.len0;
				req.dst_array0 += req.len0;
				HS_DmaMem2Mem(&req, 0, 0);
			}
			printk("RX Intr Delay by timeout\n");
			ecnt_hsdma_enable_dly_intr(ECNT_HSDMA_RX, 5, 0xFF);
			for (count = 0; count < 2; count ++)
			{
				req.src_addr0 += req.len0;
				req.dst_addr0 += req.len0;
				req.src_array0 += req.len0;
				req.dst_array0 += req.len0;
				HS_DmaMem2Mem(&req, 0, 0);
			}
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_disable_dly_intr(ECNT_HSDMA_RX);
		break;
		case 9:
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.src_addr1 = hg.src_phys1;
			req.dst_addr1 = hg.dst_phys1;
			req.src_array1 = hg.src_unc1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			printk("Channel 0 NLS is 0\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			printk("Channel 1 NLS is 1\n");
			HS_DmaMem2Mem(&req, 1, 1);
			comapre(req.src_array1, req.dst_array1, req.len1);
		break;
		case 10:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.src_addr1 = hg.src_phys1;
			req.dst_addr1 = hg.dst_phys1;
			req.src_array1 = hg.src_unc1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			printk("Channel 0 NLS is 0\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			printk("Channel 1 NLS is 0\n");
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array1, req.dst_array1, req.len1);
		break;
		case 11:
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.src_addr1 = hg.src_phys1;
			req.dst_addr1 = hg.dst_phys1;
			req.src_array1 = hg.src_unc1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			printk("Mutil Channel\n");
			HS_DmaMem2Mem_loop(&req, 1000, 0);
		break;
		case 12:
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			printk("Swap Disable\n");
			HS_DmaMem2Mem_swap(&req, 0);
			ecnt_hsdma_enable_swap();
			comapre(req.src_array0, req.dst_array0, req.len0);

			prandom_bytes(req.src_array0, req.len0);
			printk("Swap Enable\n");
			HS_DmaMem2Mem_swap(&req, 1);
			ecnt_hsdma_disable_swap();
			comapre(req.src_array0, req.dst_array0, req.len0);
		break;
		case 13:
			testLen = SIZE_4K;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_16BYTES);
			printk("16Bytes Burst\n");
			HS_DmaMem2Mem_time_measurement(&req, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);

			reset_cpu_index();
			ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_32BYTES);
			printk("32Bytes Burst\n");
			HS_DmaMem2Mem_time_measurement(&req, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);

			reset_cpu_index();
			ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_64BYTES);
			printk("64Bytes Burst\n");
			HS_DmaMem2Mem_time_measurement(&req, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);

			reset_cpu_index();
			ecnt_hsdma_set_burst_size(ECNT_HSDMA_BURST_128BYTES);
			printk("128Bytes Burst\n");
			HS_DmaMem2Mem_time_measurement(&req, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 14:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			ecnt_hsdma_disable_tx_wbd();
			printk("WD Bone Disable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);

			prandom_bytes(req.src_array0, req.len0);
			ecnt_hsdma_enable_tx_wbd();
			printk("WD Bone Enable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
		break;
		case 15:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			printk("RX DMA Enable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);

			prandom_bytes(req.src_array0, req.len0);
			ecnt_hsdma_stop_dma(ECNT_HSDMA_RX);
			printk("RX DMA Disable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			ecnt_hsdma_start_dma(ECNT_HSDMA_RX);
		break;
		case 16:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			printk("TX DMA Enable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);

			prandom_bytes(req.src_array0, req.len0);
			ecnt_hsdma_stop_dma(ECNT_HSDMA_TX);
			printk("TX DMA Disable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			ecnt_hsdma_start_dma(ECNT_HSDMA_TX);
		break;
		case 17:
			quite = 1;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.src_addr1 = hg.src_phys1;
			req.dst_addr1 = hg.dst_phys1;
			req.src_array1 = hg.src_unc1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("All Intr Disable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			comapre(req.src_array1, req.dst_array1, req.len1);

			prandom_bytes(req.src_array0, req.len0);
			prandom_bytes(req.src_array1, req.len1);
			ecnt_hsdma_enable_intr((ECNT_HSDMA_INT_TXDONE | ECNT_HSDMA_INT_TXDONE_1));
			printk("TX Intr Enable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			comapre(req.src_array1, req.dst_array1, req.len1);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 18:
			quite = 1;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.src_addr1 = hg.src_phys1;
			req.dst_addr1 = hg.dst_phys1;
			req.src_array1 = hg.src_unc1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("All Intr Disable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			comapre(req.src_array1, req.dst_array1, req.len1);

			prandom_bytes(req.src_array0, req.len0);
			prandom_bytes(req.src_array1, req.len1);
			ecnt_hsdma_enable_intr((ECNT_HSDMA_INT_RXDONE | ECNT_HSDMA_INT_RXDONE_1));
			printk("RX Intr Enable\n");
			HS_DmaMem2Mem(&req, 0, 0);
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			comapre(req.src_array1, req.dst_array1, req.len1);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 19:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			ecnt_hsdma_disable_rdbk();
			printk("Read Back Disable\n");
			HS_DmaMem2Mem_time_measurement(&req, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);

			reset_cpu_index();
			ecnt_hsdma_enable_rdbk();
			printk("Read Back Ensable\n");
			HS_DmaMem2Mem_time_measurement(&req, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 20:
			quite = 1;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("No Alignment Test\n");
			for (src_offset = 0; src_offset < 2048; src_offset++)
			{
					req.src_addr0 = (req.src_addr0 + (src_offset % 16));
					req.dst_addr0 = (req.dst_addr0 + (src_offset % 16));
					req.src_array0 = (req.src_array0 + (src_offset % 16));
					req.dst_array0 = (req.dst_array0 + (src_offset % 16));
					req.len0 = (1 + (src_offset % 16));
					HS_DmaMem2Mem(&req, 0, 0);
					comapre(req.src_array0, req.dst_array0, req.len0);
			}
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 21:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.src_addr1 = hg.src_phys1;
			req.dst_addr1 = hg.dst_phys1;
			req.src_array1 = hg.src_unc1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("DRAM2DRAM Channel 0/1\n");
			HS_DmaMem2Mem(&req, 0, 0);
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			comapre(req.src_array1, req.dst_array1, req.len1);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 22:
			testLen = SIZE_192K;
			req.src_addr0 = NSRAM_PHYS;
			req.dst_addr0 = NSRAM_PHYS + testLen;
			req.src_array0 = hg.nsram_base;
			req.dst_array0 = hg.nsram_base + testLen;
			req.src_addr1 = req.dst_addr0;
			req.dst_addr1 = req.src_addr0;
			req.src_array1 = req.dst_array0;
			req.dst_array1 = req.src_array0;
			req.len1 = req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("SRAM2SRAM Channel 0/1\n");
			memset_io(req.src_array0, 0x5A, testLen);
			/* readback npu sram data */
			readb((req.src_array0 + testLen - 1));
			rmb();
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			memset_io(req.src_array1, 0xA5, testLen);
			/* readback npu sram data */
			readb((req.src_array1 + testLen - 1));
			rmb();
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array1, req.dst_array1, req.len1);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 23:
			req.src_addr0 = hg.src_phys0;
			req.dst_addr1 = req.dst_addr0 = NSRAM_PHYS;
			req.src_array0 = hg.src_unc0;
			req.dst_array1 = req.dst_array0 = hg.nsram_base;
			req.src_addr1 = hg.src_phys1;
			req.src_array1 = hg.src_unc1;
			req.len1 = req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("DRAM2SRAM Channel 0/1\n");
			HS_DmaMem2Mem(&req, 0, 0);
			comapre(req.src_array0, req.dst_array0, req.len0);
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array1, req.dst_array1, req.len1);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 24:
			req.src_addr1 = req.src_addr0 = NSRAM_PHYS;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array1 = req.src_array0 = hg.nsram_base;
			req.dst_array0 = hg.dst_unc0;
			req.dst_addr1 = hg.dst_phys1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("SRAM2DRAM Channel 0/1\n");
			HS_DmaMem2Mem(&req, 0, 0);
			HS_DmaMem2Mem(&req, 0, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			comapre(req.src_array1, req.dst_array1, req.len1);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 25:
			quite = 1;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("No Alignment Test Channel 0/1\n");
			for (src_offset = 0; src_offset < 4; src_offset++)
			{
				for (dst_offset = 0; dst_offset < 4; dst_offset++)
				{
					req.src_addr0 = (hg.src_phys0 + src_offset);
					req.dst_addr0 = (hg.dst_phys0 + dst_offset);
					req.src_array0 = (hg.src_unc0 + src_offset);
					req.dst_array0 = (hg.dst_unc0 + dst_offset);
					req.src_addr1 = (hg.src_phys1 + src_offset);
					req.dst_addr1 = (hg.dst_phys1 + dst_offset);
					req.src_array1 = (hg.src_unc1 + src_offset);
					req.dst_array1 = (hg.dst_unc1 + dst_offset);
					req.len1 = req.len0 = (testLen + src_offset + dst_offset);
					HS_DmaMem2Mem(&req, 0, 0);
					HS_DmaMem2Mem(&req, 0, 1);
					comapre(req.src_array0, req.dst_array0, req.len0);
					comapre(req.src_array1, req.dst_array1, req.len1);
				}
			}
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		case 26:
			quite = 1;
			req.src_addr0 = hg.src_phys0;
			req.dst_addr0 = hg.dst_phys0;
			req.src_array0 = hg.src_unc0;
			req.dst_array0 = hg.dst_unc0;
			req.src_addr1 = hg.src_phys1;
			req.dst_addr1 = hg.dst_phys1;
			req.src_array1 = hg.src_unc1;
			req.dst_array1 = hg.dst_unc1;
			req.len1 = req.len0 = testLen;
			ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
			printk("Porformance Test Channel 0/1\n");
			HS_DmaMem2Mem_time_measurement(&req, 0);
			HS_DmaMem2Mem_time_measurement(&req, 1);
			comapre(req.src_array0, req.dst_array0, req.len0);
			comapre(req.src_array1, req.dst_array1, req.len1);
			ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);
		break;
		default:
			ecnt_hsdma_reg_dump();
		break;
	}

	disableCCNT();
	disablePMU();
	printk("\n hsdma_run_test<<\n\n");
	return count;
}

void HSdmaIrqHandler(void)
{
	u32 value = ecnt_hsdma_get_intr_sts();

	if (value)
		printk("***HSdmaIrqHandler cpu%x irq 0x%08x***\n", smp_processor_id(), value);
	//clear interrupt status
	if(value & ECNT_HSDMA_INT_TXDONE)
		printk("Channel 0 Tx interrupt\n");
	if(value & ECNT_HSDMA_INT_TXDONE_1)
		printk("Channel 1 Tx interrupt\n");
	if(value & ECNT_HSDMA_INT_RXDONE)
		printk("Channel 0 Rx interrupt\n");
	if(value & ECNT_HSDMA_INT_RXDONE_1)
		printk("Channel 1 Rx interrupt\n");
	if(value & ECNT_HSDMA_INT_TXDLY)
		printk("Tx Delay interrupt\n");
	if(value & ECNT_HSDMA_INT_RXDLY)
		printk("Rx Delay interrupt\n");
	if(value & ECNT_HSDMA_INT_TXCOHER)
		printk("Tx Coherent interrupt\n");
	if(value & ECNT_HSDMA_INT_RXCOHER)
		printk("Rx Coherent interrupt\n");

	if ((value & ECNT_HSDMA_INT_RXCOHER) | (value & ECNT_HSDMA_INT_TXCOHER))
	{
		ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_RXCOHER | ECNT_HSDMA_INT_TXCOHER);
	}
	ecnt_hsdma_clear_intr_sts(ECNT_HSDMA_INT_ALL); /* Write one clear INT_status */
}
EXPORT_SYMBOL(HSdmaIrqHandler);

static int RalinkHSdmaInit(void)
{
	memset(&hg, 0, sizeof(struct hsdma_test_global));

	hg.src_unc0 = ecnt_hsdma_alloc_dma(buf_size, &(hg.src_phys0));
	if (hg.src_unc0 == NULL)
	{
		printk("\nFAILED: alloc src_unc0\n");
		goto err;
	}
	hg.dst_unc0 = ecnt_hsdma_alloc_dma(buf_size, &(hg.dst_phys0));
	if (hg.dst_unc0 == NULL)
	{
		printk("\nFAILED: alloc dst_unc0\n");
		goto err;
	}

	hg.src_unc1 = (hg.src_unc0 + (buf_size / 2));
	hg.dst_unc1 = (hg.dst_unc0 + (buf_size / 2));
	hg.src_phys1 = (hg.src_phys0 + (buf_size / 2));
	hg.dst_phys1 =  (hg.dst_phys0 + (buf_size / 2));

	hg.proc_hsdma_test = create_proc_entry("tc3162/hsdma_test", 0, NULL);
	if (!hg.proc_hsdma_test)
	{
		printk("\nFAILED: create proc for hsdma_test\n");
		goto err;
	}
	hg.proc_hsdma_test->write_proc = hsdma_run_test;

	spin_lock_init(&(hg.chan[0].page_lock));
	spin_lock_init(&(hg.chan[1].page_lock));

	ecnt_hsdma_enable_test_mode(&old_cfg, &old_intr_dly, &old_intr, &ddone, &nls);
	test_irq_handler = HSdmaIrqHandler;
	ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
	reset_cpu_index();
	ecnt_hsdma_enable_intr(ECNT_HSDMA_INT_ALL);

	hg.nsram_base = ioremap(NSRAM_PHYS, SIZE_384K);
	hg.timer_base= ioremap(TIMER_PHYS, 0x100);

    #ifdef HSDMA_CPU_BUS_TEST
    set_hsdma_block_test_hook = set_hsdma_block_test;
    enable_hsdma_by_txCpu_hook = enable_hsdma_by_txCpu;
    wait_hsdma_done_hook = wait_hsdma_done;
    #endif
    
	printk("nsram_base %p\n", hg.nsram_base);

	return 0;

err:
	if (hg.src_unc0)
		ecnt_hsdma_free_dma(buf_size, hg.src_unc0, hg.src_phys0);
	if (hg.dst_unc0)
		ecnt_hsdma_free_dma(buf_size, hg.dst_unc0, hg.dst_phys0);
	if (hg.proc_hsdma_test)
		remove_proc_entry("tc3162/hsdma_test", NULL);

	return -ENOMEM;
}

static void RalinkHSdmaExit(void)
{
	iounmap(hg.nsram_base);
	iounmap(hg.timer_base);

	ecnt_hsdma_disable_intr(ECNT_HSDMA_INT_ALL);
	reset_cpu_index();
	ecnt_hsdma_enable_intr(old_intr);
	test_irq_handler = NULL;
	ecnt_hsdma_disable_test_mode(old_cfg, old_intr_dly);

	if (hg.src_unc0)
		ecnt_hsdma_free_dma(buf_size, hg.src_unc0, hg.src_phys0);
	if (hg.dst_unc0)
		ecnt_hsdma_free_dma(buf_size, hg.dst_unc0, hg.dst_phys0);
	if (hg.proc_hsdma_test)
		remove_proc_entry("tc3162/hsdma_test", NULL);
}


module_init(RalinkHSdmaInit);
module_exit(RalinkHSdmaExit);

MODULE_ALIAS("platform:ecnt-hsdma-test");
MODULE_DESCRIPTION("ECONET High-Speed DMA Controller Verify Driver");
MODULE_LICENSE("GPL v2");

