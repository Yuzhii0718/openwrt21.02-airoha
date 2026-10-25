#ifndef _LINUX_ECNT_ARCH_ARM64_MM_H
#define _LINUX_ECNT_ARCH_ARM64_MM_H

#include "scu/ecnt_npu.h"
extern unsigned int ecnt_qdma_lan_get_buffer_size(void);
extern unsigned int ecnt_qdma_wan_get_buffer_size(void);
extern void ecnt_qdma_lan_set_buffer_base(phys_addr_t buffer_base);
extern void ecnt_qdma_wan_set_buffer_base(phys_addr_t buffer_base);
extern void ecnt_ppe_set_buffer_base(phys_addr_t buffer_base);


static inline void __init ecnt_arm_memblock_init_inline_hook(void)
{
	phys_addr_t buffer_size = 0;
	phys_addr_t qdma_res_phy_addr = 0;
	phys_addr_t ppe_res_phy_addr = 0;

#if defined(TCSUPPORT_CPU_EN7581)	
	/* Reserve QDMA LAN buffer */
	buffer_size = ecnt_qdma_lan_get_buffer_size();
	printk("QDMA LAN buffer_size = 0x%lx\n", buffer_size);
	if(buffer_size)
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,20,0) 
		qdma_res_phy_addr = memblock_alloc(buffer_size, (64<<10));
#else
		/* Fix alloc memory in 0x80000000 ~ 0xffffffff because
		    1. DMA only recognize phy addr 0x00000000 ~ 0xffffffff.
		    2. DRAM start at 0x80000000.
		*/
		qdma_res_phy_addr = memblock_phys_alloc_range(buffer_size, (64<<10), 0x80000000, 0xffffffff);
#endif
		printk("qdma_lan_res_phy_addr = 0x%lx\n", qdma_res_phy_addr);
		if(qdma_res_phy_addr)
		{
			memblock_reserve(qdma_res_phy_addr, buffer_size);
			ecnt_qdma_lan_set_buffer_base(qdma_res_phy_addr);
		}
		else
		{
			printk("\r\n qdma_lan_buffer memblock_alloc fail \n\r");
		}
	}
	else
	{
		printk("\r\n qdma_lan_buffer_size get fail \n\r");
	}
	/* Reserve QDMA WAN buffer */
	buffer_size = ecnt_qdma_wan_get_buffer_size();
	printk("QDMA WAN buffer_size = 0x%lx\n", buffer_size);
	if(buffer_size)
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,20,0) 
		qdma_res_phy_addr = memblock_alloc(buffer_size, (64<<10));
#else
		/* Fix alloc memory in 0x80000000 ~ 0xffffffff because
		    1. DMA only recognize phy addr 0x00000000 ~ 0xffffffff.
		    2. DRAM start at 0x80000000.
		*/

		qdma_res_phy_addr = memblock_phys_alloc_range(buffer_size, (64<<10), 0x80000000, 0xffffffff);
#endif
		printk("qdma_wan_res_phy_addr = 0x%lx\n", qdma_res_phy_addr);
		if(qdma_res_phy_addr)
		{
			memblock_reserve(qdma_res_phy_addr, buffer_size);
			ecnt_qdma_wan_set_buffer_base(qdma_res_phy_addr);
		}
		else
		{
			printk("\r\n qdma_wan_buffer memblock_alloc fail \n\r");
		}
	}
	else
	{
		printk("\r\n qdma_wan_buffer_size get fail \n\r");
	}
#endif

#if defined(TCSUPPORT_CPU_EN7581)
	/* Reserve PPE buffer */
	buffer_size = 64*1024*80;
	if(buffer_size)
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,20,0) 
		ppe_res_phy_addr = memblock_alloc(buffer_size, 64);
#else
		/* Fix alloc memory in 0x80000000 ~ 0xffffffff because
		    1. DMA only recognize phy addr 0x00000000 ~ 0xffffffff.
		    2. DRAM start at 0x80000000.
		*/
        #if defined(TCSUPPORT_CPU_EN7581)
		ppe_res_phy_addr = memblock_phys_alloc_range(buffer_size, 64, 0x80000000, 0xffffffff);
        #else
       		ppe_res_phy_addr = memblock_phys_alloc(buffer_size, (64<<10));
        #endif
#endif
		printk("ppe_res_phy_addr = 0x%lx\n", ppe_res_phy_addr);
		if(ppe_res_phy_addr)
		{
			memblock_reserve(ppe_res_phy_addr, buffer_size);
			ecnt_ppe_set_buffer_base(ppe_res_phy_addr);
		}
		else
		{
			printk("\r\n ppe_buffer memblock_alloc fail \n\r");
		}
	}
	else
	{
		printk("\r\n ppe_buffer_size get fail \n\r");
	}
#endif
#if defined(TCSUPPORT_CPU_EN7581)
#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
// Reserve 16M block to save packet, and NPU will get the address after npu bring up
#ifdef TCSUPPORT_WLAN_INODE
#define NPU_PKT_BUF_SIZE (0x2e00000)
#else
#ifdef TCSUPPORT_WLAN_MT7990
#define NPU_PKT_BUF_SIZE (0x2c00000)
#elif defined(TCSUPPORT_WLAN_MT7992)
#define NPU_PKT_BUF_SIZE (0x2c00000)
#else
#define NPU_PKT_BUF_SIZE (0x1600000)
#endif
#endif
#define NPU_BA_NODE_SIZE (0x200000)
	phys_addr_t npu_pkt_buf_phy_addr = 0;
	phys_addr_t npu_ba_node_phy_addr = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
	npu_pkt_buf_phy_addr = memblock_phys_alloc_range(NPU_PKT_BUF_SIZE, 64, 0x80000000, 0xbfffffff); //npu support phy address range ~0xbfffffff
#else
	npu_pkt_buf_phy_addr = memblock_alloc(NPU_PKT_BUF_SIZE, 64);
#endif
	if(npu_pkt_buf_phy_addr)
	{
		memblock_reserve(npu_pkt_buf_phy_addr, NPU_PKT_BUF_SIZE);
		npu_wifi_offload_set_pkt_buf_addr(npu_pkt_buf_phy_addr);
		printk("npu_pkt_buffer memblock_alloc:%x,buf_size=%x\n\r", npu_pkt_buf_phy_addr, NPU_PKT_BUF_SIZE);
	}
	else
	{
		printk("\r\n npu_pkt_buffer memblock_alloc fail \n\r");
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
	npu_ba_node_phy_addr = memblock_phys_alloc_range(NPU_BA_NODE_SIZE, 64, 0x80000000, 0xbfffffff); //npu support phy address range ~0xbfffffff
#else
	npu_ba_node_phy_addr = memblock_alloc(NPU_BA_NODE_SIZE, 64);
#endif
	if(npu_ba_node_phy_addr)
	{
		memblock_reserve(npu_ba_node_phy_addr, NPU_BA_NODE_SIZE);
		npu_wifi_offload_set_ba_node_addr(npu_ba_node_phy_addr);
		printk("npu_ba_node memblock_alloc:%x,buf_size=%x\n\r", npu_ba_node_phy_addr, NPU_BA_NODE_SIZE);
	}
	else
	{
		printk("\r\n npu_ba_node memblock_alloc fail \n\r");
	}
	
/* when TDMA tx is used with DRAM, need these func
#define NPU_TDMA_TX_PKT_BUF_SIZE (0x40000) //4M for TDMA ring desc for text only.
	phys_addr_t npu_tdma_tx_pkt_buf_phy_addr = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
	npu_tdma_tx_pkt_buf_phy_addr = memblock_phys_alloc_range(NPU_TDMA_TX_PKT_BUF_SIZE, 64, 0x80000000, 0xbfffffff);
#else
	npu_tdma_tx_pkt_buf_phy_addr = memblock_alloc(NPU_TDMA_TX_PKT_BUF_SIZE, 4);
#endif

	if(npu_tdma_tx_pkt_buf_phy_addr)
	{
		memblock_reserve(npu_tdma_tx_pkt_buf_phy_addr, NPU_TDMA_TX_PKT_BUF_SIZE);
		npu_wifi_offload_set_tdma_tx_pkt_buf_addr(npu_tdma_tx_pkt_buf_phy_addr);
		//printk("npu_tdma_tx_pkt_buffer 7581 memblock_alloc:%x, buf_size=%x\n\r", npu_tdma_tx_pkt_buf_phy_addr, NPU_TDMA_TX_PKT_BUF_SIZE);
	}
	else
	{
		printk("\r\n npu_tx_pkt_buffer memblock_alloc fail \n\r");
	}
*/
#ifdef TCSUPPORT_NPU_WIFI_TX
// for eagle project -- Geo Yang
#define NPU_TX_PKT_BUF_SIZE (0x2800000) //40M(2048*(16384+4096) due to token queue mechanism)
//#define NPU_TX_PKT_BUF_SIZE (0x3000000)
	// 4096 : tdma rx ring desc num
	// 2048 : buffer for tdma rx
	// 1024 : tx slow path buffer
	// *2   : since there are 2 rings
	// 2048 : pkt size
	phys_addr_t npu_tx_pkt_buf_phy_addr = 0;
	//printk("try alloc for tx pkt buf\n");
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
	npu_tx_pkt_buf_phy_addr = memblock_phys_alloc_range(NPU_TX_PKT_BUF_SIZE, 64, 0x80000000, 0xbfffffff);
#else
	npu_tx_pkt_buf_phy_addr = memblock_alloc(NPU_TX_PKT_BUF_SIZE, 4);
#endif

	if(npu_tx_pkt_buf_phy_addr)
	{
		memblock_reserve(npu_tx_pkt_buf_phy_addr, NPU_TX_PKT_BUF_SIZE);
		npu_wifi_offload_set_tx_pkt_buf_addr(npu_tx_pkt_buf_phy_addr);
		printk("npu_tx_pkt_buffer 7581 memblock_alloc:%x, buf_size=%x\n\r", npu_tx_pkt_buf_phy_addr, NPU_TX_PKT_BUF_SIZE);
	}
	else
	{
		printk("\r\n npu_tx_pkt_buffer memblock_alloc fail \n\r");
	}
#endif
#endif
#endif
}



#endif

