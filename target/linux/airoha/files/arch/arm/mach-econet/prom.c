#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <linux/bootmem.h>
#endif
#include <linux/blkdev.h>

#ifndef TCSUPPORT_CPU_ARMV8
#include <asm/mipsmtregs.h>
#include <asm/addrspace.h>
#include <asm/bootinfo.h>
#include <asm/time.h>
#endif
#include <asm/cpu.h>
#include <asm/tc3162/tc3162.h>
#include <asm/traps.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#include <linux/memblock.h>
#include <linux/kernel.h>
#include <linux/irqchip.h>
#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#ifndef TCSUPPORT_CPU_ARMV8_64
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#endif

#if defined(TCSUPPORT_CPU_EN7580)
#include <boot/packageInfo.h>
#define PACKAE_INFO_DBG 0
#endif

#ifdef TCSUPPORT_INIC_CLIENT
#include <linux/mtd/fttdp_inic.h>
#endif
#if defined (CONFIG_MIPS_CMP)
#include <asm/mips-cm.h>
#include <asm/smp-ops.h>
#endif
extern int __imem, __dmem;

#include "scu/ecnt_npu.h"
#ifdef L2CACHE_LOCK_CODE
#include <asm/r4kcache.h>
#include <asm/barrier.h>
#include <asm/tc3162/irq.h>
#define L2C_FETCH_LOCK      0x1f
#define L2C_HIT_INVALID     0x13
#define Index_Load_Tag_L2   0x07
#define PHY_ADDR_MASK       0x1fffffff
#define GDMP_SRAM_BOOT2_BASE    0x9fa41000

extern int __l2cmem, _l2cmem_end;
extern unsigned int l2c_next_dest;
extern unsigned int boot2_lock_size;
static unsigned int l2c_lock_size_left = MAX_L2C_LOCK_SIZE;
static unsigned int isL2cReleaseAll=0;
#endif


#ifdef CONFIG_RCU_STALL_COMMON
extern int rcu_cpu_stall_suppress __read_mostly;
#endif

#ifdef TCSUPPORT_CPU_ARMV8
extern unsigned int ecnt_qdma_lan_get_buffer_size(void);
extern unsigned int ecnt_qdma_wan_get_buffer_size(void);
extern void ecnt_qdma_lan_set_buffer_base(phys_addr_t buffer_base);
extern void ecnt_qdma_wan_set_buffer_base(phys_addr_t buffer_base);
#endif


/* frankliao added 20101215 */
unsigned long flash_base;
EXPORT_SYMBOL(flash_base);
unsigned int (*ranand_read_byte)(unsigned long long) = NULL;
EXPORT_SYMBOL(ranand_read_byte);
unsigned int (*ranand_read_dword)(unsigned long long) = NULL;
EXPORT_SYMBOL(ranand_read_dword);


static void __init econet_early_init(void)
{
#ifdef TCSUPPORT_CPU_ARMV8
	unsigned int qdma_lan_buffer_size = 0;
	unsigned int qdma_wan_buffer_size = 0;
	phys_addr_t qdma_lan_res_phy_addr = 0;
	phys_addr_t qdma_wan_res_phy_addr = 0;

	/* Reserve QDMA LAN buffer */
	qdma_lan_buffer_size = ecnt_qdma_lan_get_buffer_size();
	if(qdma_lan_buffer_size)
	{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
		qdma_lan_res_phy_addr = memblock_phys_alloc(qdma_lan_buffer_size, (64<<10));
#else
		qdma_lan_res_phy_addr = memblock_alloc(qdma_lan_buffer_size, (64<<10));
#endif
		if(qdma_lan_res_phy_addr)
		{
			memblock_reserve(qdma_lan_res_phy_addr, qdma_lan_buffer_size);
			ecnt_qdma_lan_set_buffer_base(qdma_lan_res_phy_addr);
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
	qdma_wan_buffer_size = ecnt_qdma_wan_get_buffer_size();
	if(qdma_wan_buffer_size)
	{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
		qdma_wan_res_phy_addr = memblock_phys_alloc(qdma_wan_buffer_size, (64<<10));
#else
		qdma_wan_res_phy_addr = memblock_alloc(qdma_wan_buffer_size, (64<<10));
#endif
		if(qdma_wan_res_phy_addr)
		{
			memblock_reserve(qdma_wan_res_phy_addr, qdma_wan_buffer_size);
			ecnt_qdma_wan_set_buffer_base(qdma_wan_res_phy_addr);
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
#ifdef TCSUPPORT_NPU_WIFI_OFFLOAD
// Reserve 16M block to save packet, and NPU will get the address after npu bring up
#define NPU_PKT_BUF_SIZE (0x1600000)
	phys_addr_t npu_pkt_buf_phy_addr = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
	npu_pkt_buf_phy_addr = memblock_phys_alloc(NPU_PKT_BUF_SIZE, 4);
#else
	npu_pkt_buf_phy_addr = memblock_alloc(NPU_PKT_BUF_SIZE, 4);
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
	
/* when TDMA tx is used with DRAM, need these func
#define NPU_TDMA_TX_PKT_BUF_SIZE (0x40000) //4M for TDMA ring desc for text only.
	phys_addr_t npu_tdma_tx_pkt_buf_phy_addr = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
	npu_tdma_tx_pkt_buf_phy_addr = memblock_phys_alloc_range(NPU_TDMA_TX_PKT_BUF_SIZE, 64, 0x80000000, 0xffffffff);
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
#define NPU_TX_PKT_BUF_SIZE (0x1000000) //16M(2048*8193 due to token queue mechanism) for tx 2.4G+5G 
	phys_addr_t npu_tx_pkt_buf_phy_addr = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 55))
	npu_tx_pkt_buf_phy_addr = memblock_phys_alloc(NPU_TX_PKT_BUF_SIZE, 4);
#else
	npu_tx_pkt_buf_phy_addr = memblock_alloc(NPU_TX_PKT_BUF_SIZE, 4);
#endif

	if(npu_tx_pkt_buf_phy_addr)
	{
		memblock_reserve(npu_tx_pkt_buf_phy_addr, NPU_TX_PKT_BUF_SIZE);
		npu_wifi_offload_set_tx_pkt_buf_addr(npu_tx_pkt_buf_phy_addr);
		//printk("npu_tx_pkt_buffer memblock_alloc:%x,buf_size=%x\n\r", npu_tx_pkt_buf_phy_addr, NPU_TX_PKT_BUF_SIZE);
	}
	else
	{
		printk("\r\n npu_tx_pkt_buffer memblock_alloc fail \n\r");
	}
#endif

#endif
#endif
}

static const char *const econet_compat[] __initconst = {
	"econet,en7523",
	NULL,
};
#ifndef TCSUPPORT_CPU_ARMV8_64
DT_MACHINE_START(ECONET_DT, "ECONET (Device Tree)")
	.dt_compat      = econet_compat,
	.init_early	= econet_early_init,
MACHINE_END
#endif

#if 0

#ifdef CONFIG_MIPS_TC3262
unsigned char io_swap_noneed=0;
EXPORT_SYMBOL(io_swap_noneed);
#endif

static void tc3162_component_setup(void)
{
	unsigned int controlReg;
	unsigned long flags;

	/* setup bus timeout value */
	VPint(CR_AHB_AACS) = 0xffff;

	/* reset hwnat */
	if (isRT65168) {
		/* table reset */
		VPint(0xbfbe0024) = 0x0;
		VPint(0xbfbe0024) = 0xffff;
		
		/* hwnat swreset */
		VPint(0xbfbe0000) = (1<<1);
	}

#ifdef CONFIG_CPU_TC3162
#ifdef CONFIG_TC3162_IMEM
	/* setup imem start address */
	VPint(CR_IMEM) = CPHYSADDR(&__imem);

	/* clear internal imem */
	local_irq_save(flags);
	controlReg = read_c0_cctl();
	write_c0_cctl(controlReg & ~CCTL_IMEMOFF);
	write_c0_cctl(controlReg | CCTL_IMEMOFF);
	write_c0_cctl(controlReg);
	local_irq_restore(flags);

	/* refill internal imem */
	local_irq_save(flags);
	controlReg = read_c0_cctl();
	write_c0_cctl(controlReg & ~CCTL_IMEMFILL4);
	write_c0_cctl(controlReg | CCTL_IMEMFILL4);
	write_c0_cctl(controlReg);
	local_irq_restore(flags);
	
	printk("Enable IMEM addr=%x\n", CPHYSADDR(&__imem));
#endif

#ifdef CONFIG_TC3162_DMEM
	/* setup dmem start address */
	VPint(CR_DMEM) = CPHYSADDR(&__dmem);

	memcpy((void *) 0xa0001000, (void *) KSEG1ADDR(&__dmem), 0x800);

	/* clear internal dmem */
	local_irq_save(flags);
	controlReg = read_c0_cctl();
	write_c0_cctl(controlReg & ~CCTL_DMEMOFF);
	write_c0_cctl(controlReg | CCTL_DMEMOFF);
	write_c0_cctl(controlReg);
	local_irq_restore(flags);

	/* internal dmem on */
	local_irq_save(flags);
	controlReg = read_c0_cctl();
	write_c0_cctl(controlReg & ~CCTL_DMEMON);
	write_c0_cctl(controlReg | CCTL_DMEMON);
	write_c0_cctl(controlReg);
	local_irq_restore(flags);

	printk("Enable DMEM addr=%x\n", CPHYSADDR(&__dmem));

	memcpy((void *) KSEG1ADDR(&__dmem), (void *) 0xa0001000, 0x800);
#endif
#endif
}

/* frankliao added 20101215 */
void flash_init(void)
{

	if ((IS_NANDFLASH) && (isRT63165 || isRT63365 || isMT751020 || isEN751221 || isEN751627 || isEN7580)) {
		flash_base = 0x0;
	} else {
		if(isMT751020 || isMT7505 || isEN751221 || isEN751627 || isEN7580){
			#ifdef TCSUPPORT_INIC_CLIENT
			/* To use last INIC_CLIENT_RAM_SIMU_OFFSET size of RAM as flash */
			flash_base = 0xA0000000 + (0x800000 * (1 << (((VPint(0xbfb0008c) >> 13) & 0x7) - 1))- INIC_CLIENT_RAM_SIMU_MAX_SIZE);
			#else
			flash_base = 0xbc000000;
			#endif
		} else if (isTC3162U || isRT63260 || isRT65168 || isTC3182 || isRT63165 || isRT63365) {
			flash_base = 0xb0000000;
		} else {
			flash_base = 0xbfc00000;
		}
		printk("%s: flash_base:%x \n",__func__,flash_base);
	}
}

const char *get_system_type(void)
{
#ifdef CONFIG_MIPS_TC3262
	if( isEN7528){
		io_swap_noneed = 1;
		return "EcoNet EN7528 SOC";
	}else if( isEN751627){
		io_swap_noneed = 1;
		return "EcoNet EN751627 SOC";	
       }else if(isEN7580){
		io_swap_noneed = 1;
		return "EcoNet EN7580 SOC";
	}else if(isEN751221){
		io_swap_noneed = 1;
		return "EcoNet EN751221 SOC";	
	}else if (isTC3182)
		return "TrendChip TC3182 SOC";
	else if (isRT65168)
		return "Ralink RT65168 SOC";
	else if (isRT63165){
		io_swap_noneed = 1;
		return "Ralink RT63165 SOC";
	} else if (isRT63365) {
		io_swap_noneed = 1;
#ifdef TCSUPPORT_DYING_GASP		
		if(!isRT63368){
			//gpio 4 is share pin for rt63365.
			VPint(0xbfb00860) &= ~(1<<13);//disable port 4 led when use rt63365.
		}
#endif		
		return "Ralink RT63365 SOC";
	}else if (isMT751020){
		io_swap_noneed = 1;
		return "Ralink MT751020 SOC";
	}else if (isMT7505){
		io_swap_noneed = 1;
		return "Ralink MT7505 SOC";
	}
	else
		return "TrendChip TC3169 SOC";
#else
	if (isRT63260)	
		return "Ralink RT63260 SOC";
	else if (isTC3162U)
		return "TrendChip TC3162U SOC";
	else if (isTC3162L5P5)
		return "TrendChip TC3162L5/P5 SOC";
	else if (isTC3162L4P4)
		return "TrendChip TC3162L4/P4 SOC";	
	else if (isTC3162L3P3)
		return "TrendChip TC3162L2F/P2F";
	else if (isTC3162L2P2)
		return "TrendChip TC3162L2/P2";
	else 
		return "TrendChip TC3162";
#endif
}

extern struct plat_smp_ops msmtc_smp_ops;
#define VECTORSPACING 0x100	/* for EI/VI mode */


void __init mips_nmi_setup (void)
{
	void *base;
	extern char except_vec_nmi;
	#if 0
	base = cpu_has_veic ?
		(void *)(CAC_BASE + 0xa80) :
		(void *)(CAC_BASE + 0x380);
	#endif

	base = cpu_has_veic ?
		(void *)(ebase + 0x200 + VECTORSPACING*64) :
		(void *)(ebase + 0x380);
		
	printk("nmi base is %x\n",base);

	//Fill the NMI_Handler address in a register, which is a R/W register
	//start.S will read it, then jump to NMI_Handler address
	VPint(0xbfb00244) = base;
	
	memcpy(base, &except_vec_nmi, 0x80);
	flush_icache_range((unsigned long)base, (unsigned long)base + 0x80);
}

void cpu_dma_round_robin(uint8 mode)
{
    uint32 reg_value=0;
    reg_value = VPint(ARB_CFG);
    if(mode == ENABLE){
        reg_value |= ROUND_ROBIN_ENABLE;
    } else {
        reg_value &= ROUND_ROBIN_DISBALE;
    }
    VPint(ARB_CFG) = reg_value;
}

#define	QDMA_WAN_DBG_MEM_XS_DATA_HI		0xbfb57108
uint cal_qdma_init_buffer(void)
{
	uint32 flashQdmaInit = 0;
	uint8 qdmaLanPayloadMode = 0, qdmaLanDscpMode = 0;
	uint8 qdmaWanPayloadMode = 0, qdmaWanDscpMode = 0;
	uint qdmaLanBufferSize = 0, qdmaWanBufferSize = 0;
	/*--16K*2048, 16K*1024, 16K*512, 16K*256--*/
	uint qdmaSramBuffer[4] = {32, 16, 8, 4};
	/*--64K*2048, 64K*1024, 64K*512, 64K*256--*/
	uint qdmaDramBuffer[4] = {128, 64, 32, 16};

	/* [1:0], 0x0--qdma_lan payload size 2k, 0x1--qdma_lan 1k, 0x2--qdma_lan 512, 0x3--qdma_lan 256
	** [2], reserve
	** [3], 0x0--qdma_lan dscp in DRAM, 0x1--qdma_lan dscp in SRAM
	** [5:4], 0x0--qdma_wan payload size 2k, 0x1--qdma_wan 1k, 0x2--qdma_wan 512, 0x3--qdma_wan 256
	** [6], reserve
	** [7], 0x0--qdma_wan dscp in DRAM, 0x1--qdma_wan dscp in SRAM */
	flashQdmaInit = VPint(QDMA_WAN_DBG_MEM_XS_DATA_HI);
	VPint(QDMA_WAN_DBG_MEM_XS_DATA_HI) = 0;
	qdmaLanPayloadMode = (flashQdmaInit & 0x3);
	qdmaLanDscpMode = (flashQdmaInit & 0x8) >> 3;
	qdmaWanPayloadMode = (flashQdmaInit & 0x30) >> 4;
	qdmaWanDscpMode = (flashQdmaInit & 0x80) >> 7;

	/*calculate QDMA LAN need buffer size*/
	if(qdmaLanDscpMode){/*DSCP in SRAM: 16K-dscp*/
		qdmaLanBufferSize = qdmaSramBuffer[qdmaLanPayloadMode];
	}else{/*DSCP in DRAM: 64K-dscp*/
		qdmaLanBufferSize = qdmaDramBuffer[qdmaLanPayloadMode];
	}

	/*calculate QDMA WAN need buffer size*/
	if(qdmaWanDscpMode){/*DSCP in SRAM: 16K-dscp*/
		qdmaWanBufferSize = qdmaSramBuffer[qdmaWanPayloadMode];
	}else{/*DSCP in DRAM: 64K-dscp*/
		qdmaWanBufferSize = qdmaDramBuffer[qdmaWanPayloadMode];
	}

	return (qdmaLanBufferSize + qdmaWanBufferSize);
}


#ifdef TCSUPPORT_CPU_EN7580
void sync_func_cmd (void) {
	__asm__ __volatile__(			
		".set	push\n\t"		
		".set	noreorder\n\t"		
		".set	mips2\n\t"		
		"sync\n\t"			
		".set	pop"			
		: /* no output */		
		: /* no input */		
		: "memory");
}

void sync_func_uncachedRead (void) {
	unsigned long int tmp = 0x0;

	sync_func_cmd();
    tmp = VPint(0xA0000000);
}

void (*ecnt_sync_func)(void) = sync_func_cmd;
EXPORT_SYMBOL(ecnt_sync_func);
#endif

#ifdef L2CACHE_LOCK_CODE
void l2c_dump_range (unsigned long start, unsigned long end)
{
    unsigned int cp0Config2 = __read_32bit_c0_register($16, 2);
    unsigned int lsize_bits = (((cp0Config2>>4)&0xf)+1);
    unsigned long lsize = (1<<lsize_bits);
    unsigned long addr = start & ~(lsize - 1);
    unsigned long aend = (end - 1) & ~(lsize - 1);
    unsigned int phyAddr;
    unsigned int idx, way, l2TagVal, dwordIdx, tagIdx, i;
    unsigned int l2c_idx_mask=0, l2c_way_num, l2c_highPhyAddr_mask=0, dword_num;
    unsigned int sets_bits, index_bits;

    if (start>=end) {
        printk("%s Fail: (start:0x%08x) >= (end:0x%08x)\n", 
            __func__, (unsigned int)start, (unsigned int)end);
        return;
    }

    if (((start&0x1f)!=0) || ((end&0x1f)!=0)) {
        printk("%s Fail: start:0x%08x or end:0x%08x is not 32B alignment\n", 
                __func__, (unsigned int)start, (unsigned int)end);
        return;
    }

    printk("%s: 0x%08x ~ 0x%08x\n", __func__, (unsigned int)start, (unsigned int)(end - 1));
    
    l2c_way_num = ((cp0Config2&0xf)+1);
    sets_bits = (((cp0Config2>>8)&0xf)+6);
    dword_num = lsize>>3;
    index_bits = sets_bits+lsize_bits;
    for (i=0; i<index_bits; i++)
        l2c_idx_mask |= (1<<i);
    for (i=index_bits; i<29; i++)
        l2c_highPhyAddr_mask |= (1<<i);

    printk("lsize:%d, way:%d, index_bits:%d, l2c_idx_mask:0x%08x, l2c_highPhyAddr_mask:0x%08x, dword_num:%d\n\n", 
            (int)lsize, (int)l2c_way_num, (int)index_bits, l2c_idx_mask, l2c_highPhyAddr_mask, (int)dword_num);


    for (phyAddr=(addr&PHY_ADDR_MASK); (phyAddr&l2c_idx_mask)<=(aend&l2c_idx_mask); phyAddr+=lsize) 
    {
        idx = (phyAddr&l2c_idx_mask);
        /* cpu will randomly choose a way for a cache index when storing/locking,
           so go through all ways to know which way is for the cache index */
        for (way=0; way<l2c_way_num; way++) 
        {
            tagIdx = (idx|(way<<index_bits));
            cache_op(Index_Load_Tag_L2, tagIdx);
            __sync();
            l2TagVal = __read_32bit_c0_register($28, 4);

            if (((l2TagVal>>7)&0x1) && /*valid bit*/
                ((l2TagVal&l2c_highPhyAddr_mask)==(phyAddr&l2c_highPhyAddr_mask))) /*high phyAddr matchs*/
            {
                printk("L2_CACHE_LINE TAG:\n  phyAddr:0x%08x, Valid:%d, Lock:%d, Dirty:%d\n", 
                        (l2TagVal&l2c_highPhyAddr_mask)|idx, (int)((l2TagVal>>7)&0x1),
                        (int)((l2TagVal>>5)&0x1), (int)((l2TagVal>>6)&0x1));
                
                printk("L2_CACHE_LINE DATA:\n");
                for (dwordIdx=0; dwordIdx<dword_num; dwordIdx++)
                {   /* dump data by Index_Load_Tag op */
                    tagIdx = ((idx|(way<<index_bits))+(dwordIdx<<3));
                    cache_op(Index_Load_Tag_L2, tagIdx);
                    __sync();
                    if ((dwordIdx&0x1)==0)
                        printk("  0x%08x:", phyAddr+(dwordIdx<<3));
                    #ifdef TCSUPPORT_LITTLE_ENDIAN
                    printk("  0x%08x  0x%08x", __read_32bit_c0_register($28, 5), __read_32bit_c0_register($29, 5));
                    #else
                    printk("  0x%08x  0x%08x", __read_32bit_c0_register($29, 5), __read_32bit_c0_register($28, 5));
                    #endif
                    if (dwordIdx&0x1)
                        printk("\n");
                }
                break;
            }
        }
        printk("\n");
    }
    printk("%s OK\n", __func__);

    return;
}

void l2c_dump_all (void)
{
    l2c_dump_range((unsigned long)&__l2cmem, (unsigned long)l2c_next_dest);
    return;
}

void l2c_dump_info (void)
{
    unsigned int cp0Config2;
    int lsize, l2c_way_num, sets_per_way;

    cp0Config2 = __read_32bit_c0_register($16, 2);
    lsize = (1<<(((cp0Config2>>4)&0xf)+1));    
    l2c_way_num = ((cp0Config2&0xf)+1);
    sets_per_way = (1<<(((cp0Config2>>8)&0xf)+6));

    printk("L2 Cache Size: %d KBytes (lsize:%d, way:%d, sets_per_way:%d)\n", 
            (int)((lsize*l2c_way_num*sets_per_way)>>10), lsize, l2c_way_num, sets_per_way);

    if (isL2cReleaseAll==0)
    {
        printk("L2 Cache lock_range: 0x%08x ~ 0x%08x, lock_size:%d Bytes\n", 
               (unsigned int)&__l2cmem, l2c_next_dest-1, (int)(((unsigned int)l2c_next_dest)-((unsigned int)&__l2cmem)));

        if (isEN7528) {
            printk("L2 Cache lock_range for boot2: 0x%08x ~ 0x%08x, lock_size:%d Bytes\n", 
               (unsigned int)GDMP_SRAM_BOOT2_BASE, (unsigned int)(GDMP_SRAM_BOOT2_BASE+boot2_lock_size-1), (int)boot2_lock_size);
        }
    }

    printk("MAX_L2C_LOCK_SIZE:%d Bytes, size_left:%d Bytes\n", (int)MAX_L2C_LOCK_SIZE, (int)l2c_lock_size_left);

    return;
}

int l2c_op_range(unsigned int op, unsigned long start, unsigned long end)
{
    unsigned int cp0Config2 = __read_32bit_c0_register($16, 2);
	unsigned long lsize = (1<<(((cp0Config2>>4)&0xf)+1));
	unsigned long addr = start & ~(lsize - 1);
	unsigned long aend = (end - 1) & ~(lsize - 1);

    if (start==end) {
        printk("%s: start==end, so just return\n", __func__);
        return 0;
    }

    if (start>end) {
        printk("%s Fail: (start:0x%08x) >= (end:0x%08x)\n", 
            __func__, (unsigned int)start, (unsigned int)end);
        return -1;
    }

    if (((start&0x1f)!=0) || ((end&0x1f)!=0)) {
        printk("%s Fail: start:0x%08x or end:0x%08x is not 32B alignment\n", 
            __func__, (unsigned int)start, (unsigned int)end);
        return -1;
    }

    printk("%s: 0x%08x ~ 0x%08x, op: 0x%x\n", __func__, (unsigned int)start, (unsigned int)(end - 1), op);


	while (1) {
        if (op==L2C_FETCH_LOCK) {
            cache_op(L2C_FETCH_LOCK, addr);
            l2c_lock_size_left -= lsize;
        }
        else {
            cache_op(L2C_HIT_INVALID, addr);
            l2c_lock_size_left += lsize;
        }
		if (addr >= aend)
			break;
		addr += lsize;
	}
    __sync();
    
	return 0;
}

int l2c_lock_range(unsigned long start, unsigned long end)
{
    if (l2c_lock_size_left < (end-start)) {
        printk("%s Fail: (l2c_lock_size_left:%d bytes) < (lock_size:%d bytes)\n", 
            __func__, (int)l2c_lock_size_left, (int)(end-start));
        return -1;
    }
    
	return l2c_op_range(L2C_FETCH_LOCK, start, end);
}
EXPORT_SYMBOL(l2c_lock_range);

int l2c_release_range(unsigned long start, unsigned long end)
{
    if ((l2c_lock_size_left+(end-start)) > MAX_L2C_LOCK_SIZE) {
        printk("%s Fail: (l2c_lock_size_left:%d bytes) + (release_size:%d bytes) > (MAX_L2C_LOCK_SIZE:%d bytes)\n", 
            __func__, (int)l2c_lock_size_left, (int)(end-start), (int)MAX_L2C_LOCK_SIZE);
        return -1;
    }

    return l2c_op_range(L2C_HIT_INVALID, start, end);
}

void l2c_release_all (void)
{
    if (l2c_release_range((unsigned long)&__l2cmem, (unsigned long)l2c_next_dest)==0)
        printk("%s: OK\n", __func__);

    #ifdef TCSUPPORT_NEW_WDOG
    if (isEN7528) {
        if (l2c_release_range((unsigned long)GDMP_SRAM_BOOT2_BASE, (unsigned long)(GDMP_SRAM_BOOT2_BASE+boot2_lock_size))==0)
            printk("l2c_release for 7528 boot2: OK\n");
    }
    #endif

    isL2cReleaseAll=1;
    
    return;
}
EXPORT_SYMBOL(l2c_release_all);
#endif

void __init prom_init(void)
{
	unsigned long memsize;
	unsigned char samt;
	unsigned long col;
	unsigned long row;
    unsigned int qdma_res = 0;
    #ifdef L2CACHE_LOCK_CODE
    unsigned int l2c_lock_size = 0;
    #endif

#ifdef CONFIG_RCU_STALL_COMMON
	rcu_cpu_stall_suppress =1;
#endif

#ifdef TCSUPPORT_CPU_EN7580
if (PDIDR == 1) {/*7580 e1 IC*/
    	ecnt_sync_func = sync_func_uncachedRead;
	}

	parse_package_info();
#if PACKAE_INFO_DBG
	for(i = 0; i < PACKAGE_INFO_IDX_MAX_NO; i++) {
		printk("pkgInfo[%d].data:%x\n", i, pkgInfo[i].data);
	}
#endif
#endif

#ifdef L2CACHE_LOCK_CODE
    l2c_lock_size = ((unsigned int)&_l2cmem_end)-((unsigned int)&__l2cmem);
    printk("__l2cmem:0x%08x, _l2cmem_end:0x%08x, l2cmem_size:%d Bytes\n", 
        (unsigned int)&__l2cmem, (unsigned int)&_l2cmem_end, (int)l2c_lock_size);
    
    if (l2c_lock_size>0) {
        if (l2c_lock_range((unsigned long)&__l2cmem, (unsigned long)&_l2cmem_end)==0)
            printk("l2c_lock_range OK\n");
    }
#endif

	/* frankliao added 20101222 */
	flash_init();

#ifdef CONFIG_MIPS_TC3262

#ifdef TCSUPPORT_IS_FH_PON			
#if !defined(TCSUPPORT_FH_JOYMEV2_PON) 
		/*support squash fs  for Test */
		/*strcat(arcs_cmdline, "rootfstype=jffs2 ro init=/etc/preinit.sh");*/
#endif
#endif		

	if (isRT63165 || isRT63365 || isMT751020 || isMT7505 || isEN751221 ) {
		/* enable external sync */
		strcat(arcs_cmdline, " es=1");

#ifndef TCSUPPORT_MIPS_1004K
	/* 1004K and IA CPUs don't have to do this because their External Sync bit is set by default */
	/* enable external sync */
	{
		/* when kernel is UP, set ES=1. Otherwise, set in mips_mt_set_cpuoptions */
		unsigned int oconfig7 = read_c0_config7();
		unsigned int nconfig7 = oconfig7;

		nconfig7 |= (1 << 8);

		__asm__ __volatile("sync");
		write_c0_config7(nconfig7);
		ehb();
		printk("Config7: 0x%08x\n", read_c0_config7());
	}
#endif
	}

	if(isMT751020){
		memsize = 0x800000 * (1 << (((VPint(0xbfb0008c) >> 13) & 0x7) - 1));	
		if(memsize >= (448<<20)){
			memsize = (448<<20);
		}
		printk("memsize:%dMB\n", (memsize>>20));
	}
	else if(isMT7505){
		if(isFPGA)
			memsize = 0x800000 * (1 << (((VPint(0xbfb0008c) >> 13) & 0x7) - 1));
		else{
			if(VPint(CR_AHB_HWCONF) & (1<<10))
			{
				/* DDR1 */
				memsize = 0x4000000 / (1 << ((VPint(CR_AHB_HWCONF) >> 11) & 0x3));
			}
			else{
				/* DDR2 */
				if(!((VPint(CR_AHB_HWCONF) >> 11) & 0x3))
					memsize = 256 * 0x100000;
				if(((VPint(CR_AHB_HWCONF) >> 11) & 0x3) == 0x1)
					memsize = 32 * 0x100000;
				if(((VPint(CR_AHB_HWCONF) >> 11) & 0x3) == 0x2)
					memsize =  128 * 0x100000;
				else
					memsize = 64 * 0x100000;
			}
		}
		printk("memsize:%dMB\n", (memsize>>20));
	}else if(isEN751221 || isEN751627 || isEN7580){
		memsize = GET_DRAM_SIZE;
		if(memsize>512)
			memsize = 512;

		if(isEN7580){
			if(memsize == 512)
				memsize -= 64;
			qdma_res = cal_qdma_init_buffer();
			if( qdma_res >= memsize )
				printk("memsize is not enough for QDMA init\n");
			else
				memsize = memsize - qdma_res;
		}else{ /*EN751221 & EN751627*/
#if (TCSUPPORT_QDMA_WAN_DSCP_NUM == 8)
			qdma_res = 16;
#else
			qdma_res = 8;
#endif
			if(memsize == 512){			
				memsize = 448-qdma_res;
			}
#if !defined(TCSUPPORT_SLM_EN) ||(TCSUPPORT_QDMA_WAN_DSCP_NUM == 8)
			else{
				memsize = memsize-qdma_res;
			}
#endif
		}
		
		memsize = memsize << 20;
		printk("memsize:%dMB\n", (memsize>>20));
	}else if (isRT63165 || isRT63365) {
		/* DDR */
		if (VPint(CR_AHB_HWCONF) & (1<<25)) {
			memsize = 0x800000 * (1 << (((VPint(CR_DMC_DDR_CFG1) >> 18) & 0x7) - 1));

		/* SDRAM */
		} else {
			unsigned long sdram_cfg1;
			
			/* calculate SDRAM size */
			sdram_cfg1 = VPint(0xbfb20004);
			row = 11 + ((sdram_cfg1>>16) & 0x3);
			col = 8 + ((sdram_cfg1>>20) & 0x3);
			/* 4 bands and 16 bit width */
			memsize = (1 << row) * (1 << col) * 4 * 2;
		}
	} else {
		memsize = 0x800000 * (1 << (((VPint(CR_DMC_CTL1) >> 18) & 0x7) - 1));
	}
#else
	/* calculate SDRAM size */
	samt = VPchar(CR_DMC_SAMT);
	row = 8 + (samt & 0x3);
	col = 11 + ((samt>>2) & 0x3);
	/* 4 bands and 16 bit width */
	memsize = (1 << row) * (1 << col) * 4 * 2;
#endif

	printk("%s prom init\n", get_system_type());

	tc3162_component_setup();

	#ifdef TCSUPPORT_INIC_CLIENT
	add_memory_region(0 + 0x20000, memsize - 0x20000 - INIC_CLIENT_RAM_SIMU_MAX_SIZE, BOOT_MEM_RAM);
	#else
	add_memory_region(0 + 0x2000, memsize - 0x2000, BOOT_MEM_RAM);
    #ifdef TCSUPPORT_HIGHMEM
    if ((448<(GET_DRAM_SIZE)) && ((GET_DRAM_SIZE)<=2048)) {
        add_memory_region(0x9c000000, ((GET_DRAM_SIZE)-448)<<20, BOOT_MEM_RAM);
        printk("add %dMB highmem\n", (GET_DRAM_SIZE)-448);
    }
    else
        printk("FAIL: add highmem for GET_DRAM_SIZE==%dMB\n", (GET_DRAM_SIZE));
    #endif
	#endif
	if (isMT751020 || isMT7505 || isEN751221 || isEN751627 || isEN7580) {
		board_nmi_handler_setup = mips_nmi_setup;
	}

	//mips_machgroup = MACH_GROUP_TRENDCHIP;
	//mips_machtype = MACH_TRENDCHIP_TC3162;
	/*set CPU DMA RR */
	if (isMT751020 || isEN751221 || isEN751627 || isEN7580) {
		cpu_dma_round_robin(ENABLE);
	}
#if defined (CONFIG_MIPS_CMP)
    /* Early detection of CMP support */
    mips_cm_probe();

    if (register_cmp_smp_ops())
        printk("\n\nError: register_cmp_smp_ops failed due to CM being absent !!!\n\n");

#else
#ifdef CONFIG_MIPS_MT_SMP
	extern struct plat_smp_ops vsmp_smp_ops;
	register_smp_ops(&vsmp_smp_ops);
#endif
#endif
#ifdef CONFIG_MIPS_MT_SMTC
	register_smp_ops(&msmtc_smp_ops);
#endif
}

void __init prom_free_prom_memory(void)
{
	/* We do not have any memory to free */
}

int prom_putchar(char data)
{
	while (!(LSR_INDICATOR & LSR_THRE))
		;
	VPchar(CR_UART_THR) = data; 
	return 1;
}
EXPORT_SYMBOL(prom_putchar);

char prom_getchar(void)
{
	while (!(LSR_INDICATOR & LSR_RECEIVED_DATA_READY))
		;
	return VPchar(CR_UART_RBR);
}

static char ppbuf[1024];

void
prom_write(const char *buf, unsigned int n)
{
	char ch;

	while (n != 0) {
		--n;
		if ((ch = *buf++) == '\n')
			prom_putchar('\r');
		prom_putchar(ch);
	}
}
EXPORT_SYMBOL(prom_write);

void
prom_printf(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(ppbuf, sizeof(ppbuf), fmt, args);
	va_end(args);

	prom_write(ppbuf, i);
}
EXPORT_SYMBOL(prom_printf);

#ifdef CONFIG_KGDB
static unsigned long  uclk_65000[13]={
	357500, 	// uclk 5.5     Baud Rate 115200
	175500, 	// uclk 2.7     Baud Rate 57600
	119808, 	// uclk 1.8432  Baud Rate 38400
	89856,	// uclk	1.3824	Baud Rate 28800
	59904,	// uclk 0.9216	Baud Rate 19200
	44928,	// uclk 0.6912	Baud Rate 14400
	29952,	// uclk 0.4608	Baud Rate 9600
	14976,	// uclk 0.2304	Baud Rate 4800
	7488,	// uclk 0.1152	Baud Rate 2400 
	3744,	// uclk 0.0576	Baud Rate 1200
	1872,	// uclk 0.0288	Baud Rate 600
	936,		// uclk 0.0144	Baud Rate 300
	343		// uclk 0.00528	Baud Rate 110
};

static void hsuartInit(void)
{
	unsigned long	div_x,div_y;
	unsigned long	word;
	unsigned long   tmp;

	tmp = VPint(CR_GPIO_CTRL);
	tmp &= ~0x0fa30000;
	tmp |= 0x0fa30000;
	VPint(CR_GPIO_CTRL) = tmp; // set GPIO pin 13 & pin 12 are alternative outputs, GPIO pin 11 & pin 10 are alternative inputs
	tmp = VPint(CR_GPIO_ODRAIN);
	tmp &= ~0x00003000;
	tmp |= 0x00003000;
	VPint (CR_GPIO_ODRAIN) = tmp; // set GPIO output enable

// Set FIFO controo enable, reset RFIFO, TFIFO, 16550 mode, watermark=0x00 (1 byte)
	VPchar(CR_HSUART_FCR) = UART_FCR|UART_WATERMARK;

// Set modem control to 0
	VPchar(CR_HSUART_MCR) = UART_MCR;

// Disable IRDA, Disable Power Saving Mode, RTS , CTS flow control
	VPchar(CR_HSUART_MISCC) = UART_MISCC;

	/* access the bardrate divider */
	VPchar(CR_HSUART_LCR) = UART_BRD_ACCESS;

	div_y = UART_XYD_Y;
	div_x = (unsigned int)(uclk_65000[0]/SYS_HCLK)*2;
	word = (div_x<<16)|div_y;
	VPint(CR_HSUART_XYD) = word;

/* Set Baud Rate Divisor to 3*16		*/
	VPchar(CR_HSUART_BRDL) = UART_BRDL;
	VPchar(CR_HSUART_BRDH) = UART_BRDH;

/* Set DLAB = 0, clength = 8, stop =1, no parity check 	*/
	VPchar(CR_HSUART_LCR) = UART_LCR;

// Set interrupt Enable to, enable Tx, Rx and Line status
	VPchar(CR_HSUART_IER) = UART_IER;	
}

static int hsuartInitialized = 0;

int putDebugChar(char c)
{
	if (!hsuartInitialized) {
		hsuartInit();
		hsuartInitialized = 1;
	}

	while (!(VPchar(CR_HSUART_LSR) & LSR_THRE))
		;
	VPchar(CR_HSUART_THR) = c; 

	return 1;
}

char getDebugChar(void)
{
	if (!hsuartInitialized) {
		hsuartInit();
		hsuartInitialized = 1;
	}

	while (!(VPchar(CR_HSUART_LSR) & LSR_RECEIVED_DATA_READY))
		;
	return VPchar(CR_HSUART_RBR);
}
#endif
#if defined(TCSUPPORT_DYING_GASP) && (defined(CONFIG_MIPS_RT65168) || defined(CONFIG_MIPS_RT63365))
extern void (*cpu_wait)(void);
__IMEM
void dying_gasp_setup_mem_cpu(void){
#ifdef CONFIG_MIPS_RT65168	
		VPint(0xbfb20000) |= (1<<12); //set ddr to self refresh mode. 
		VPint(0xbfb000c0) &= ~((1<<5)|(1<<6)|(1<<7));//CPU divide to 32 and ram divide to 3
		VPint(0xbfb000c0) |= (1<<3)|(1<<4)|(1<<5)|(1<<7);
#endif
#ifdef CONFIG_MIPS_RT63365	
#if defined(TCSUPPORT_CPU_MT7510)|| defined(TCSUPPORT_CPU_MT7520) || defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512)
		VPint(0xbfb00044) = 1; //Enable DDR Self Refresh Mode
		VPint(0xbfb20004) &= ~(1<<15);
		VPint(0xbfb200e4) &= ~(1<<2);
		VPint(0xbfb00074) |= (1<<4);
		VPint(0xbfb00040) |= (1<<0); // reset ddr device
#else 	
		VPint(0xbfb00040) |= (1<<0); // reset ddr device
		//do not kill CPU because we need do watchdog interrupt
		//kill CPU
		//VPint(0xbfb001c8) |= (1<<24); // bypass pll 2 700M 	
		//VPint(0xbfb001cc) |= (1<<24); // bypass pll 2 665M	
		//VPint(0xbfb001d0) |= (1<<24); // bypass pll 2 500
#endif
#endif
	if (cpu_wait)
		(*cpu_wait)();
}
EXPORT_SYMBOL(dying_gasp_setup_mem_cpu);
#endif


#ifdef CONFIG_OF
static int __init customize_machine(void)
{

	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
	return 0;
}

void __init device_tree_init(void)
{
	__dt_setup_arch(__dtb_start);
	unflatten_and_copy_device_tree();
}

arch_initcall(customize_machine);
#endif

#endif
