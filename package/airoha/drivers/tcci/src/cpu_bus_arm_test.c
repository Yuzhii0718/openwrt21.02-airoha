
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <linux/netdevice.h>
#include <ecnt_hook/ecnt_hook_tso.h>

#include "cpu_bus_test.h"
#include <modules/npu/npuMboxAPI.h>
#include <modules/npu/npu_test_common.h>
#include <asm/tc3162/ecnt_timer.h>

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define NEW_PBUS_ARCH
#endif

#define DRAM_BASE			(0x80000000)
#define NPU_384K_SRAM_BASE  (0x1e800000)
#define HIGHMEM_512M_SIZE	(0x20000000)
#define HIGHMEM_TEST_SIZE	(0x2000) //8192byte
#define HIGHMEM_TEST_ARRAY_MAX_SIZE		(HIGHMEM_512M_SIZE/HIGHMEM_TEST_SIZE)
#define MAX_COUNT_DOWN      (0x3)
#define DISABLE             (0)
#define ENABLE              (1)
//#define DBG_LOG_ON        (1)
#define LOCK_IRQOFF_TEST    (1)
//#define CLK_CHANGE_TEST     (1)

#ifdef TCSUPPORT_CPU_AN7583
#define R2C_BYPASS_LOG      (1)
#endif
#define RG_DRAM_CONFIG     (0x1fb00074)
#define RG_DRAMC1_CONFIG   (0x1fb00974)
#define R2C_RDBYPASSWT_CNT  (0x1fb00984)
static unsigned long r2c_bypassCnt_virtAddr;

#define NUM_MASTER          (4) /*Masters: gdma,npu,cpu2,cpu3*/
#define NUM_PCIE_MAC_REG    (4)

#ifdef NEW_PBUS_ARCH
#if defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_AN7583)
#define NUM_CPUS            (2)
#define NUM_PCIE_DEV        (2)
#define PCIE_EN_BITS        (0x3) /* pcie0,1 */
#define L2C_SRAM_PBUS_BASE	(0x1EFC0000)
#define NPU_SRAM_TSIZE      (NPU_SRAM_T_SIZE)
#else /*7581*/
#define NUM_CPUS            (4)
#define NUM_PCIE_DEV        (3)
#define PCIE_EN_BITS        (0x7) /* pcie0,1,2 */
#define L2C_SRAM_PBUS_BASE	(0x1EF00000)
#define NPU_SRAM_TSIZE      (NPU_SRAM_T_SIZE>>1)
#endif /*7581,7552*/
#define NUM_PCIE_CFG        (3)
#define NUM_MEASURED_REG    (4)
#else /*7523*/
#define NUM_PCIE_DEV        (1)
#define NUM_MEASURED_REG    (2)
#define L2C_SRAM_PBUS_BASE	(0x1EFC0000)
#define NPU_SRAM_TSIZE      (NPU_SRAM_T_SIZE)
#endif

/* used by cpu, gdma in cpu_bus test.
 * Note: gdma needs L2C_SRAM_TSIZE +1K */
#define L2C_SRAM_TSIZE      (L2C_SRAM_SIZE>>3)

#define NUM_PCIE_SEC        (3)
#define NUM_GDMA_BURST      (5)
#define NUM_TESTS           (10)
#define NUM_TEST_REGS       ((NUM_PCIE_DEV<<1)+1)


#define ALL_DBG_CNT_BITS    (0x1fffff)

#define PCIE0_MEM_BASE      (0x20000000)
#define PCIE_DEV_LEN        (0x1000)
#define TEST_REG_APB        (0x1fbe0004)
#define TEST_REG_TZPC       (0x1fbe2d10)
#define TEST_REG_SEC        (0x1fbe2e04)
#define TEST_REG_SCU_SCREG_WF0  (0x1fb00240)
#define TEST_REG_GDMA       (0x1FB30000)
#define TEST_REG_RESV0      (0x1c000000)
#define TEST_REG_RESV1      (0x1dfffff0)
#define TEST_REG_RESV2      (0x1e7ffff0)
#define TEST_REG_RESV3      (0x1ffffff0)
#define TEST_REG_HSDMA      (0x1fa01800)

#define GDMP_SRAM_T_SIZE    (0x800)

void *pcie_dev_base[NUM_PCIE_DEV]; /* pcie_dev mem is read only */
unsigned int pcie_dev_addr[NUM_PCIE_DEV] = {0};
#if defined(TCSUPPORT_CPU_AN7583)
unsigned int pcie_writable_regs[NUM_CPUS][NUM_PCIE_MAC_REG]= {
               {PCIE0_MAC_BASE+PCIE0_OFF0, PCIE0_MAC_BASE+PCIE0_OFF1, PCIE1_MAC_BASE+PCIE1_OFF0, PCIE1_MAC_BASE+PCIE1_OFF1},
               {PCIE0_MAC_BASE+PCIE0_OFF2, PCIE0_MAC_BASE+PCIE0_OFF3, PCIE1_MAC_BASE+PCIE1_OFF2, PCIE1_MAC_BASE+PCIE1_OFF3}};
unsigned int tout_test_regs[] = {
               TEST_REG_HSDMA, PCIE0_MAC_BASE+0x180, 0, PCIE1_MAC_BASE+0x180, 0};

#elif defined(TCSUPPORT_CPU_AN7552)
unsigned int pcie_writable_regs[NUM_CPUS][NUM_PCIE_MAC_REG]= {
                {PCIE0_MAC_BASE+PCIE_OFF0, PCIE0_MAC_BASE+PCIE_OFF1, PCIE1_MAC_BASE+PCIE_OFF0, PCIE1_MAC_BASE+PCIE_OFF1}, 
                {PCIE0_MAC_BASE+PCIE_OFF2, PCIE0_MAC_BASE+PCIE_OFF3, PCIE1_MAC_BASE+PCIE_OFF2, PCIE1_MAC_BASE+PCIE_OFF3}};
unsigned int tout_test_regs[] = {
                TEST_REG_HSDMA, PCIE0_MAC_BASE+0x180, 0, PCIE1_MAC_BASE+0x180, 0};
#else /* 7581 */
unsigned int pcie_writable_regs[NUM_CPUS][NUM_PCIE_MAC_REG]= {
                {PCIE0_MAC_BASE+PCIE_OFF0, PCIE1_MAC_BASE+PCIE_OFF0, PCIE2_MAC_BASE+PCIE_OFF0, PCIE0_MAC_BASE+PCIE_OFF4},
                {PCIE0_MAC_BASE+PCIE_OFF1, PCIE1_MAC_BASE+PCIE_OFF1, PCIE2_MAC_BASE+PCIE_OFF1, PCIE1_MAC_BASE+PCIE_OFF4}, 
                {PCIE0_MAC_BASE+PCIE_OFF2, PCIE1_MAC_BASE+PCIE_OFF2, PCIE2_MAC_BASE+PCIE_OFF2, PCIE2_MAC_BASE+PCIE_OFF4},
                {PCIE0_MAC_BASE+PCIE_OFF3, PCIE1_MAC_BASE+PCIE_OFF3, PCIE2_MAC_BASE+PCIE_OFF3, PCIE0_MAC_BASE+PCIE_OFF5}};
unsigned int tout_test_regs[] = {
                TEST_REG_HSDMA, PCIE0_MAC_BASE+0x180, 0, PCIE1_MAC_BASE+0x180, 0, PCIE2_MAC_BASE+0x180, 0};
#endif

volatile int is_pcie_link_up[NUM_PCIE_DEV];
unsigned int pbus_resv_regs[]= {TEST_REG_RESV0, TEST_REG_RESV1, TEST_REG_RESV2, TEST_REG_RESV3};

int isPcieDevIOremapped=0;
unsigned long g_pcieMemBackAddr=NULL;

#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
#define WDOG_THSLD_TIME     (10)
#define WDOG_REBOOT_DELAY   (2)
volatile int tmr_isr_cnt, g_thsld_cnt, g_thsld_sec, is_wdog_reload;
#endif

void no_multi_issue_test(int arg);
void r_single_normal_test(int arg);
void r_single_pcie_test(int arg);
void cycle_bypass_block_cnt_test(int arg);
void pcie_dev_mem_burst_perf_test(int arg);
void pbus_timeout_test(int arg);
void pcie_mac_reg_read_satrving_test(int arg);

enum newPbusDbgCntr
{
    dcnt_cpu_pre_multi_issue=0,
    dcnt_cpu_post_multi_issue, 
    dcnt_16p_arb_pre_multi_issue,
    dcnt_rbus_pcie0_bypass,
    dcnt_rbus_pcie0_block,
    dcnt_pcie0_post_multi_issue,/*5*/
    dcnt_pcie0_pre_burst_cmd,
    dcnt_pcie0_post_burst_cmd,
    dcnt_pcie1_post_multi_issue,
    dcnt_pcie1_pre_burst_cmd,
    dcnt_pcie1_post_burst_cmd,/*10*/
    dcnt_pcie2_post_multi_issue,
    dcnt_pcie2_pre_burst_cmd,
    dcnt_pcie2_post_burst_cmd,
    dcnt_rbus_pcie1_bypass,
    dcnt_rbus_pcie1_block,/*15*/
    dcnt_rbus_pcie2_bypass,
    dcnt_rbus_pcie2_block,
    dcnt_pcie0_pre_multi_issue,
    dcnt_pcie1_pre_multi_issue,
    dcnt_pcie2_pre_multi_issue/*20*/
};

enum singlePcieTest
{
    test_burst_to_single_cmd=0,
    test_burst_to_burst_cmd,
    test_multi_to_multi_issue,
    test_single_pcie_last,
};

typedef void (*new_pbus_func_t)(int arg);

new_pbus_func_t new_pbus_tests[]= {
        no_multi_issue_test,
        r_single_normal_test,
        r_single_pcie_test,
        cycle_bypass_block_cnt_test,
        pcie_dev_mem_burst_perf_test,
        pbus_timeout_test,
        pcie_mac_reg_read_satrving_test
};

typedef struct pbusRegsVirtAddr 
{
    unsigned long apb;
    unsigned long tzpc;
    unsigned long sec;
    unsigned long pcie[NUM_CPUS][NUM_PCIE_MAC_REG];    
    unsigned long resv[NUM_CPUS];

} pbusRegsVirtAddr_t;

static pbusRegsVirtAddr_t PbusRegs;

int isNewPbusTestDone=0;
enum newPbusCase newPbusTestCase=0xfff;
void pbus_regs_ioremap(void);
unsigned long pcie_mem_backup(dma_addr_t *dmaAddrP);

extern unsigned long cpu_cycles_calculate (unsigned long*, unsigned long*, unsigned long);

typedef void (*set_func_t)(u32 val);
typedef u32 (*get_func_t)(void);

extern u32 GET_SCU_RSTCTRL1(void);
extern void SET_SCU_RSTCTRL1(u32 val);
extern u32 GET_SCU_RST_RG(void);
extern void SET_SCU_RST_RG(u32 val);

extern struct device* get_gdmpSram_dev(void);
#ifdef MBOX_API_TEST
extern struct device* get_npu_dev(void);
#endif
extern void __iomem * get_gdmpSram_base(void);
extern void __iomem * get_l2cSramPbus_base(void);
extern unsigned long get_npu_384k_sram_base(void);
extern u32 GET_DRAM_SIZE(void);
#ifdef CLK_CHANGE_TEST
#if defined(TCSUPPORT_CPU_ARMV8_64)
#include <linux/arm-smccc.h>
#define ECNT_SIP_AVS_HANDLE (0x82000301)
#define AVS_OP_FREQ_TEST    (0xdddddddd)
#endif
extern u32 get_rbus_clk(void);
extern void set_rbus_clk(u32 val);
extern void SET_SCU_RGS_CLK_GSW(u32 val);
extern void SET_SCU_RGS_CLK_EMI(u32 val);
extern void SET_SCU_RGS_CLK_BUS(u32 val);
extern void SET_SCU_RGS_CLK_FE(u32 val);
extern void SET_SCU_RGS_CLK_NPU(u32 val);
extern u32 GET_SCU_RGS_CLK_GSW(void);
extern u32 GET_SCU_RGS_CLK_EMI(void);
extern u32 GET_SCU_RGS_CLK_BUS(void);
extern u32 GET_SCU_RGS_CLK_FE(void);
extern u32 GET_SCU_RGS_CLK_NPU(void);
enum e_clk_src {
    clk_src_gsw=0,
    clk_src_emi,
    clk_src_bus,
    clk_src_fe,
    clk_src_npu,
    #if defined(TCSUPPORT_CPU_ARMV8_64)
    clk_src_cpu,
    #endif
    clk_src_last
};
#endif

extern void (*set_hsdma_block_test_hook)(dma_addr_t, dma_addr_t, u32, u32);
extern void (*enable_hsdma_by_txCpu_hook)(void);
extern int (*wait_hsdma_done_hook)(void);

unsigned long *tmp_addr_baseP, tmp_addr_cnt;
int dmaBlkCntInfo[3]={0};
int dmaBlkCntType = 0;
#ifdef TCSUPPORT_CPU_AN7583
char *dmaNames[8] = {"gdma_hsdma", "ppe_tdma", "qdma1_tx", "qdma1_rx", "qdma2_tx", "npu", "qdma2_rx", "gdma+qdma1_tx"};
#else
char *dmaNames[7] = {"gdma", "ppe", "qdma_lan", "qdma_wan", "tdma", "npu", "gdma_qdmalan"};
#endif

int cpu1_wake_up = 0;

unsigned char testBytePat[] = {0x5a, 0xa5, 0xff, 0x01};
unsigned int testIntPat[] =  {0x5a5a5a5a, 0xa5a5a5a5, 0xff00ff00, 0x00ff00ff, 0x0000ffff};
#ifdef TCSUPPORT_CPU_ARMV8_64
unsigned long testWordPat[] = {0x5a5a5a5a5a5a5a5a, 0xa5a5a5a5a5a5a5a5, 0xff00ff00ff00ff00, 0x00ff00ff00ff00ff};
#else
unsigned long testWordPat[] = {0x5a5a5a5a, 0xa5a5a5a5, 0xff00ff00, 0x00ff00ff, 0x0000ffff};
#endif

extern int is_cpu0_1_bus_test;
extern unsigned int data0, data1, data2, data3;
#ifdef MBOX_API_TEST
static int mboxAPItestDone=0;
#endif
enum cacheTestCase
{
    ctc_dc_wback_inv=0,
    ctc_dma_single_to_dev, 
    ctc_dc_wback_fail,
    ctc_dc_inv,
    ctc_dma_single_from_dev,
    ctc_dc_inv_fail,
    ctc_max_case
};

enum blockCntTestCase
{
    #ifdef TCSUPPORT_CPU_AN7583
    bctc_gdma=0,    /*and hsdma*/
    bctc_tdma,      /*and ppe*/
    bctc_qdma_lan,  /*i.e. qdma1_tx*/
    bctc_qdma1_rx,  /*=3*/
    bctc_qdma_wan,  /*i.e. qdma2_tx*/
    bctc_npu,       /*=5*/
    bctc_qdma2_rx,
    bctc_gdma_qdmalan,/*gdma+qdma1_tx test*/
    bctc_max_case
    
    #else
    bctc_gdma=0,
    bctc_ppe,
    bctc_qdma_lan,
    bctc_qdma_wan,
    bctc_tdma,
    bctc_npu,   
    bctc_gdma_qdmalan,
    bctc_max_case
    #endif
};

unsigned int pcie_devmem_size[NUM_PCIE_DEV];
static int is_pcie_devmem_size_got=0;
extern unsigned int regRead_PCIe(unsigned int reg);
extern void regWrite_PCIe(unsigned int reg, unsigned int val);
int pcie_dev_mem_size_get(void);


void dcache_api_test (int test_case)
{
    uint32 *unc_addr, *cac_addr;
    uint32 *unc_src_addr, *cac_dst_addr;
    uint32 *cac_src_addr, *unc_dst_addr;
    int i;
    unsigned long len=TC_GDMA_TEST_SIZE;
    uint8 *s_dram, *d_dram;
    unsigned long phy_dst_addr, phy_src_addr;
    struct device *dev=NULL;
    dma_addr_t phy_addr;


    if ((dev=get_gdmpSram_dev())==NULL) {
        printk("\nget_gdmpSram_dev failed\n");
		return;
    }
    
    unc_addr = (uint32 *)dma_alloc_coherent(dev, len, &phy_addr, GFP_KERNEL);
    
    if (unc_addr == NULL) {
        printk("\nError(%s):dma_alloc_coherent failed\n", __func__);
        return;
    }
    
    cac_addr = (uint32 *) kmalloc(len, GFP_KERNEL);
    if (cac_addr==NULL) {
        dma_free_coherent(dev, len, unc_addr, phy_addr);
        printk("\nError(%s) kmalloc failed\n", __func__);
        return; 
    }

    if (test_case<=ctc_dc_wback_fail) { /* dcache wback cases */
        cac_src_addr = cac_addr;    
        unc_dst_addr = unc_addr;
        s_dram = (uint8 *)cac_src_addr;
        d_dram = (uint8 *)unc_dst_addr;
        phy_src_addr = virt_to_phys(cac_src_addr);
        phy_dst_addr = phy_addr;
    }
    else { /* dcache inv cases */
        unc_src_addr = unc_addr;
        cac_dst_addr = cac_addr;
        s_dram = (uint8 *)unc_src_addr;
        d_dram = (uint8 *)cac_dst_addr;
        phy_src_addr = phy_addr;
        phy_dst_addr = virt_to_phys(cac_dst_addr);
    }

    for(i = 0; i <len; i++)
    {
        s_dram[i] = ((i+0x1)&0xff);
        d_dram[i] = 0;
    }
    
    if (test_case==ctc_dc_wback_inv) { /* wback_inv src region */
        ecnt_dcache_wback_inv(cac_src_addr, len);
    }
    else if (test_case==ctc_dma_single_to_dev) {
        /* dma_map_single will clean (wback) dcache for DMA_TO_DEVICE */
        phy_src_addr = dma_map_single(dev, (void *)cac_src_addr, len, DMA_TO_DEVICE);
    	if (dma_mapping_error(dev, phy_src_addr)) {
    		printk("dma_map_single TO_DEVICE error\n");
            goto dcache_api_free_resource;
    	}
    }
    else if (test_case==ctc_dc_inv) { /* invalidate dst region */
        phy_dst_addr = virt_to_phys(cac_dst_addr);
        ecnt_dcache_inv(cac_dst_addr, len);
    }
    else if (test_case==ctc_dma_single_from_dev) { 
        /* dma_map_single will invalidate dcache for DMA_FROM_DEVICE */
        phy_dst_addr = dma_map_single(dev, (void*)cac_dst_addr, len, DMA_FROM_DEVICE);
        if (dma_mapping_error(dev, phy_dst_addr)) {
            printk("dma_map_single FROM_DEVICE error\n");
            goto dcache_api_free_resource;
        }
    }
    else {
        /* do nothing for ctc_dc_wback_fail and ctc_dc_inv_fail */
    }
    
    /* enable DMA to move data from phy_src_addr to phy_dst_addr */
    SET_GDMA_CONFIG(0, (dram_phyAddr_to_dmaAddr(phy_src_addr)), (dram_phyAddr_to_dmaAddr(phy_dst_addr)), 
                    ((len&0xffff)<<16)|(0<<3)|(1<<1)|(1<<0), 0x4);

    /* wait until GDMA is done */
    WAIT_GDMA_DONE(0);


    /* because dst region is invalidated, its data should be loaded from DRAM to dcache. */
    for(i = 0; i <len; i++)
    {
        if(s_dram[i] != d_dram[i])
        {
            if ((test_case==ctc_dc_wback_fail) || (test_case==ctc_dc_inv_fail))
                goto dcache_api_test_done;
            printk("\nERROR: s_dram[%d]=0x%x != d_dram[%d]=0x%x for case:%d\n", i, s_dram[i], i, d_dram[i], test_case);
            printk("\ts_dram_addr=0x%lx, d_dram_addr=%lx\r\n",&s_dram[i],&d_dram[i]);
            printk("\tphy_src_addr=0x%lx, phy_dst_addr=%lx\r\n",phy_src_addr,phy_dst_addr);
            goto dcache_api_test_out;
        }
    }

    if ((test_case==ctc_dc_wback_fail) || (test_case==ctc_dc_inv_fail)) {
        printk("\nERROR for case:%d !\n", test_case);
        goto dcache_api_test_out;
    }

dcache_api_test_done:    
    printk("\n%s done for case:%d !\n", __func__, test_case);

dcache_api_test_out:    
    if (test_case==ctc_dma_single_from_dev)
        dma_unmap_single(dev, phy_dst_addr, len, DMA_FROM_DEVICE);
    if (test_case==ctc_dma_single_to_dev)
        dma_unmap_single(dev, phy_src_addr, len, DMA_TO_DEVICE);
dcache_api_free_resource:  
    dma_free_coherent(dev, len, unc_addr, phy_addr);
    kfree(cac_addr);
    return;
}

int doCacheApiTest(int argc, char *argv[], void *p)
{
    int test_case = (int)simple_strtoul(argv[1], NULL, 10);

    if (test_case >= ctc_max_case) {
        printk("\nError: No this test_case:%d\n", test_case);
        return 0;
    }
    
    dcache_api_test(test_case);

	return 0;
}

#ifdef TCSUPPORT_CPU_AN7583
unsigned int diff_cnt (unsigned int cnt0, unsigned int cnt1)
{
    if (cnt1>=cnt0)
        return (cnt1-cnt0);
    else
        return ((ALL_FF-cnt0)+cnt1);
}

void r2c_rdbypasswt_test (int test_cnt)
{
    int i, j;
    unsigned int cycle0, cycle1;
    unsigned int bypass0, bypass1;
    unsigned long dram_unc_addr, dram_phy_addr, test_addr;
    struct device *dev=NULL;
    dma_addr_t dram_phy_srcAddr;
    unsigned int gdma_src, gdma_dst, gdma_ct0, gdma_ct1;
    unsigned int alloc_size = S_4K;
    int channel=0, coherent=1, continuous=1, tout_cnt;
    int burst = 4; /* burst:0/1/2/3/4 means 1/2/4/8/16 DW which is 4/8/16/32/64 bytes */
    int data_size = 256;
    unsigned long addr_off[] = {128, 252, 256, 384, 508, 512, 768};
    int size_off = sizeof(addr_off)/sizeof(addr_off[0]);

    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("get_gdmpSram_dev is NULL\n");
        return;
    }

    dram_unc_addr = (unsigned long) dma_alloc_coherent(dev, alloc_size, &dram_phy_srcAddr, GFP_KERNEL);
    if (((unsigned long)dram_unc_addr)&0xff) {
        printk("\nError: dram_unc_addr:0x%p is not 256-byte alignment, finish testing!\n", dram_unc_addr);
        return;
    }
    dram_phy_addr = (unsigned long)dram_phy_srcAddr;

    /* if gdma burst size is 64 bytes, gdma will read 1st 64 bytes, write 1st 64 bytes,
     * read 2nd 64 bytes, write 2nd 64 bytes, ... 
     * set gdma_src==gdma_dst, so that gdma's read won't trigger r2c_rdBypassWt counter.*/
    gdma_src = dram_phy_addr+data_size;
    gdma_dst = gdma_src;
    gdma_ct0 = ((data_size&0xffff)<<16)|(burst<<3)|(ENABLE<<1)|(1<<0);

    for (j=0; j<size_off; j++) {
    
        /* cpu reads 4 bytes on byte252, 256, 508, 512 respectively while GDMA is writing byte256~512 */
        test_addr = dram_unc_addr+addr_off[j];

        /* enable GDMA with continuous mode */
        gdma_ct1 = ((coherent<<2)|(continuous<<14));
        SET_GDMA_CONFIG(channel, gdma_src, gdma_dst, gdma_ct0, gdma_ct1);
        //set_GDMA_enable_bit(channel);

        bypass0 = readl(r2c_bypassCnt_virtAddr);
        cycle0 = read_c0_count();
        for (i=0; i<test_cnt; i++) {
            rdMemW(test_addr);
        }
        cycle1 = read_c0_count();
        bypass1 = readl(r2c_bypassCnt_virtAddr);

        /*disable GDMA continuous mode */
        gdma_ct1 = (coherent<<2);
        SET_GDMA_CONFIG(channel, gdma_src, gdma_dst, gdma_ct0, gdma_ct1);

        tout_cnt=10;                                
        /* wait until GDMA is done */
        while(!IS_GDMA_DONE(channel)) {
            if ((tout_cnt--)<0) {
                printk("ERROR: GDMA done_bit timeout\n");
                goto r2c_rdbypasswt_test_exit;
            }
            msleep(1);
        }
        CLEAR_GDMA_DONE(channel); /* clear done bit */

        printk("[addr:%d] cycles: %d, bypass: %d (for test_cnt:%d)\n", 
                (int)addr_off[j], (int)diff_cnt(cycle0,cycle1), (int)diff_cnt(bypass0,bypass1), test_cnt);
    }

r2c_rdbypasswt_test_exit:
    dma_free_coherent(dev, alloc_size, dram_unc_addr, dram_phy_srcAddr);
    return;
}

int doR2cRdbypassWt(int argc, char *argv[], void *p)
{
    int test_cnt = (int)simple_strtoul(argv[1], NULL, 10);
    unsigned long virtAddr1, virtAddr2;

    arm_pmu_init();
    r2c_bypassCnt_virtAddr = ioremap(R2C_RDBYPASSWT_CNT, 4);
    virtAddr1 = ioremap(RG_DRAM_CONFIG, 4);
    virtAddr2 = ioremap(RG_DRAMC1_CONFIG, 4);
    
    printk("\n(%s) is_out-of-order:%d, is_rdBypassWt_en:%d\n", __func__, ((readl(virtAddr1))>>31)&0x1, ((readl(virtAddr2))>>5)&0x1);

    r2c_rdbypasswt_test(test_cnt);

    iounmap(r2c_bypassCnt_virtAddr);
    iounmap(virtAddr1);
    iounmap(virtAddr2);
	return 0;
}
#endif
#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
irq_handler_t arht_timer_isr (int irq, void* dev_id)
{
    int tmr_no;
    
    tmr_no = arht_irq_to_tmrNo(irq, dev_id);
    printk("[H%d](%s) irq:%d, tmr_no:%d, isr_cnt:%d\n", smp_processor_id(), __func__, irq, tmr_no, tmr_isr_cnt);
    
    if (arht_timer_intr_sts_check(tmr_no)==1) {
        if (arht_timer_intr_sts_clear(tmr_no)==0) {
            tmr_isr_cnt++;
        }
        else {
            printk("\nError(%s): intr_sts bit can't be clear\n\n", __func__);
        }
    }
    else {
        printk("\nError(%s): intr_sts bit isn't raised\n\n", __func__);
    }
    
    return IRQ_HANDLED;
}

irq_handler_t arht_wdog_isr (int irq, void* dev_id)
{
    int tmr_no, wdog_no;
    int pass_time = g_thsld_cnt*g_thsld_sec;
    
    tmr_no = arht_irq_to_tmrNo(irq, dev_id);
    wdog_no = tmr_no-3;
    printk("[H%d](%s) irq:%d, wdog_no:%d\n", smp_processor_id(), __func__, irq, wdog_no);
    
    if (arht_timer_intr_sts_check(tmr_no)==1) {
        if (arht_timer_intr_sts_clear(tmr_no)==0) {

            if ((tmr_isr_cnt-1)!=pass_time)
                printk("\nError(%s): tmr_isr_cnt:%d != pass_time:%d\n\n", __func__, (tmr_isr_cnt-1), pass_time);
                    
            if ((g_thsld_cnt==1)&&(is_wdog_reload==1)) {
                g_thsld_cnt++;
                printk("wdog%d reload at %d sec\n", wdog_no, pass_time);
                arht_wdog_reload(wdog_no);
            }
            else { /*g_thsld_cnt==2 or is_wdog_reload==0*/
                printk("wdog%d has run %d sec and is going to reboot sytem in %d sec!\n", wdog_no, pass_time, WDOG_REBOOT_DELAY);
            }
        }
        else {
            printk("\nError(%s): intr_sts bit can't be clear\n\n", __func__);
        }
    }
    else {
        printk("\nError(%s): intr_sts bit isn't raised\n\n", __func__);
    }
    
    return IRQ_HANDLED;
}

void arht_timers_test(void)
{
    int tmr_no;
    unsigned int test_sec=30;
    unsigned int tmr_sec=1;
    unsigned int isr_cnt=(test_sec/tmr_sec);
    unsigned int sleep_msec = 100;

    printk("[H%d]%s start\n", smp_processor_id(), __func__);
    

    /* disable and register all timers before test */
    for (tmr_no=0; tmr_no<NUM_TIMERS; tmr_no++) {
        if (is_arht_timer_en(tmr_no)) {
            arht_timer_en_set(tmr_no, DISABLE);
        }
        arht_timer_request_irq(tmr_no, arht_timer_isr);
    }

    for (tmr_no=0; tmr_no<NUM_TIMERS; tmr_no++) {
    
        printk("timer%d test for %d sec\n", tmr_no, test_sec);

        tmr_isr_cnt=1;
        
        arht_timer_load_ms_set(tmr_no, tmr_sec*1000);
        arht_timer_en_set(tmr_no, ENABLE);

        while(1) {
            if (tmr_isr_cnt>isr_cnt)
                break;
            msleep_interruptible(sleep_msec);
        }

        arht_timer_en_set(tmr_no, DISABLE);
        msleep(sleep_msec);
    }

    return;
}

/* 1. after g_thsld_sec, wdog_isr is executed,
 * 2. in wdog_isr, reload wdog,
 * 3. after g_thsld_sec, wdog_isr is executed again,
 * 4. in wdog_isr, do nothing but print msg, 
 * 5. after WDOG_REBOOT_DELAY, system reboots. 
 * (Note: if thsld_sec is set, no need to do reload) */
void arht_wdogs_test(int wdog_no, int thsld_sec)
{
    int tmr_no = 0;
    int tmr_sec = 1;
    int wdog_tmr_no = wdog_no+3;
    int reboot_sec, reboot_time;
    unsigned int sleep_msec = 100;

    if (wdog_no>=NUM_WDOGS) {
        printk("Error(%s) wdog_no:%d >= %d\n", __func__, wdog_no, NUM_WDOGS);
        return;
    }

    if (thsld_sec) {
        reboot_sec = thsld_sec+WDOG_REBOOT_DELAY;
        reboot_time = reboot_sec;
        is_wdog_reload = 0;
        g_thsld_sec = thsld_sec;
    }
    else {
        reboot_sec = WDOG_THSLD_TIME+WDOG_REBOOT_DELAY;
        reboot_time = reboot_sec+WDOG_THSLD_TIME;
        is_wdog_reload = 1;
        g_thsld_sec = WDOG_THSLD_TIME;
    }

    printk("wdog%d test for thsld_time:%d, reboot_time:%d sec\n", wdog_no, g_thsld_sec, reboot_sec);

    /* disable and register timer0 and wdogX before test */
    arht_timer_en_set(tmr_no, DISABLE);
    arht_timer_en_set(wdog_tmr_no, DISABLE);
    arht_timer_request_irq(tmr_no, arht_timer_isr);
    arht_timer_request_irq(wdog_tmr_no, arht_wdog_isr);

    /* set and enable timer0 */
    tmr_isr_cnt=1;
    arht_timer_load_ms_set(tmr_no, tmr_sec*1000);
    arht_timer_en_set(tmr_no, ENABLE);
    
    /* set and enable wdogX */
    g_thsld_cnt = 1;
    arht_timer_load_ms_set(wdog_tmr_no, reboot_sec*1000);
    arht_wdog_thsld_ms_set(wdog_no, WDOG_REBOOT_DELAY*1000);
    arht_wdog_en_set(wdog_no, ENABLE);

    printk("wait for wdog%d to reboot system in %d secs\n", wdog_no, reboot_time);
    while(1) {
        msleep_interruptible(sleep_msec);
    }
}
#endif

int doL2cSramTest(int argc, char *argv[], void *p)
{
    unsigned int t_size;

    t_size = get_l2c_sram_size();

    if (t_size){
        enable_npu_rbus_decoder(1);
        if (l2c_sram_test(0, t_size, 0)==0)
            printk("l2c_sram_test done for size:0x%x\n", t_size);
    }
    else
        printk("no l2c sram\n");

	return 0;
}

#ifdef MULTI_MODULES_SUPPORT
int cpuAccessRegisters(unsigned int cnt)
{
    int i;
    int cpuid = smp_processor_id();
    int regs[4] = {0,0,0,0};

    #if defined(TCSUPPORT_CPU_EN7581) && !defined(TCSUPPORT_CPU_AN7583)
    regs[cpuid&0x3] = 1;
    #else /*7552,7523,7583 has only 2 cpus*/
    regs[(cpuid<<1)&0x3] = 1;
    regs[((cpuid<<1)+1)&0x3] = 1;
    #endif

    if (regs[0]) {
        
        /* RBus regs r/w test */
        if (rbus_regs_test(cnt)==-1)
            return -1;

        /* gdmp regs r/w test */
        if (gdump_regs_test(cnt)==-1)
            return -1;
    }
    if (regs[1]) {
        
        /* apb regs r/w test */
        writel(cnt, PbusRegs.apb);
        if (readl(PbusRegs.apb)!=cnt) {
            printk("Error: readl(0x%x):0x%lx != 0x%x\n", TEST_REG_APB, readl(PbusRegs.apb), cnt);
            return -1;
        }

        /* gsw regs r/w test */
        set_frame_engine_data(0x1fb58000, cnt);
        if ((get_frame_engine_data(0x1fb58000)!=cnt)) {
            printk("gsw_reg(0x1fb58000):0x%x != 0x%x\n",
                    get_frame_engine_data(0x1fb58000), cnt);
            return -1;
        }
    }
    if (regs[2]) {
        
        /* fe regs r/w test */
        set_frame_engine_data(0x1fb52200, cnt);
        if (get_frame_engine_data(0x1fb52200)!=cnt) {
            printk("fe_reg(0x1fb52200):0x%x != 0x%x\n",
                    get_frame_engine_data(0x1fb52200), cnt);
            return -1;
        }
        #if 0 //cause npu's hsdma rx done bit timeout
        /* hsdma regs r/w test */
        if (ecnt_hsdma_reg_access_test(cnt)==-1)
            return -1;
        #endif
    }
    if (regs[3]) {
        
        /* tzpc regs r/w test */
        writel(cnt, PbusRegs.tzpc);
        if (readl(PbusRegs.tzpc)!=0xdeadbeef) {
            printk("Error: readl(0x%x):0x%lx != 0xdeadbeef\n", TEST_REG_TZPC, readl(PbusRegs.tzpc));
            return -1;
        }

        /* SEC regs r/w test */
        writel(cnt, PbusRegs.sec);
        if (readl(PbusRegs.sec)!=0xdeadbeef) {
            printk("Error: readl(0x%x):0x%lx != 0xdeadbeef\n", TEST_REG_SEC, readl(PbusRegs.sec));
            return -1;
        }

        /* PBus regs r/w test */
        if (pbus_regs_test(cnt)==-1)
            return -1;
    }

    /* pcie mac regs r/w test */
    for (i=0; i<NUM_PCIE_MAC_REG; i++) {
        writel(cnt+i, PbusRegs.pcie[cpuid][i]);
        if (readl(PbusRegs.pcie[cpuid][i])!=(cnt+i)) {
            printk("Error: readl(0x%x):0x%lx != 0x%x\n", pcie_writable_regs[cpuid][i], readl(PbusRegs.pcie[cpuid][i]), cnt+i);
            return -1;
        }
    }

    /* pbus resv regs r/w test */
    writel(cnt+i, PbusRegs.resv[cpuid]);
    if (readl(PbusRegs.resv[cpuid])!=0xdeadbeef) {
        printk("Error: readl(0x%x):0x%lx != 0xdeadbeef\n", pbus_resv_regs[cpuid], readl(PbusRegs.resv[cpuid]));
        return -1;
    }

    return 0;
}
#endif

/* CPU accesses Host_Regs/NPU_Regs/GDMP_SRAM/NPU_384K_SRAM/L2C_SRAM */
int cpuAccessRegsRams(unsigned long cnt)
{
    int cpuid = smp_processor_id();

    if (is_cpu0_1_bus_test==0) {
        if (RW_SCREG_REG_TEST(cnt)) {
            printk("[H%d]ERROR: RW_SCREG_REG_TEST failed\n", cpuid);
            return -1;
        }
    }

    #ifdef MULTI_MODULES_SUPPORT
    if (cpuAccessRegisters(cnt)) {
        printk("[H%d]ERROR: cpuAccessRegisters failed\n", cpuid);
        return -1;
    }
    #endif

    if (test_npu_mib_regs()) {
        printk("[H%d]ERROR: test_npu_mib_regs failed\n", cpuid);
        return -1;
    }
    
    if (RW_GDMP_SRAM_TEST(cpuid*GDMP_SRAM_T_SIZE, GDMP_SRAM_T_SIZE, cnt)) {
        printk("[H%d]ERROR: RW_GDMP_SRAM_TEST failed\n", cpuid);
        return -1;
    }

    /* the first (NPU_SRAM_T_SIZE*MAX_CORE_NUM) bytes are used by NPU Cores */
    if (test_npu_384k_sram((NPU_SRAM_T_SIZE*MAX_CORE_NUM)+(NPU_SRAM_TSIZE*cpuid), NPU_SRAM_TSIZE, cnt)) {
        printk("[H%d]ERROR: test_npu_384k_sram failed\n", cpuid);
        return -1;
    }
    
    /* first half for Host CPUs, last half for NPU Cores */
    if (get_l2c_sram_size()==L2C_SRAM_SIZE) {
        if (l2c_sram_test(L2C_SRAM_TSIZE*cpuid, L2C_SRAM_TSIZE, cnt)) {
            printk("[H%d]ERROR: l2c_sram_test failed\n", cpuid);
            return -1;
        }
    }
    
    return 0;
}

#ifdef CLK_CHANGE_TEST
void RbusDynamicChangeClkTtest_fpga(void)
{
	unsigned int npu_clkdiv_sel = 0;
	unsigned short npu_clksrc_sel = 0;
	unsigned int gsw_clkdiv_sel = 0;
	unsigned short gsw_clksrc_sel = 0;
	unsigned int emi_clkdiv_sel = 0;
	unsigned short emi_clksrc_sel = 0;
	//printk("============Rbus Dynamic CPU is %d\n", smp_processor_id());
	(get_random_bytes(&npu_clkdiv_sel, sizeof(unsigned int)));
	(get_random_bytes(&npu_clksrc_sel, sizeof(unsigned short)));
	(get_random_bytes(&gsw_clkdiv_sel, sizeof(unsigned int)));
	(get_random_bytes(&emi_clkdiv_sel, sizeof(unsigned int)));
	(get_random_bytes(&gsw_clksrc_sel, sizeof(unsigned short)));
	(get_random_bytes(&emi_clksrc_sel, sizeof(unsigned short)));
	npu_clkdiv_sel=(npu_clkdiv_sel&0x7);
	npu_clksrc_sel=(npu_clksrc_sel&0x1);
	gsw_clkdiv_sel=(gsw_clkdiv_sel&0x7);
	gsw_clksrc_sel=(gsw_clksrc_sel&0x1);
	emi_clkdiv_sel=(emi_clkdiv_sel&0x7);
	emi_clksrc_sel=(emi_clksrc_sel&0x1);

		
	set_rbus_clk(((npu_clkdiv_sel << 17) | (npu_clksrc_sel << 16 )| (gsw_clkdiv_sel << 9) | (gsw_clksrc_sel << 8 )| (emi_clkdiv_sel <<5) | (emi_clksrc_sel << 4)));
		printk("[H1] change clk npu_clkdiv_sel = %x; npu_clksrc_sel = %x, gsw_clkdiv_sel = %x, gsw_clksrc_sel = %x, emi_clkdiv_sel = %x, emi_clksrc_sel = %x\n",
			npu_clkdiv_sel, npu_clksrc_sel,gsw_clkdiv_sel,gsw_clksrc_sel,emi_clkdiv_sel,emi_clksrc_sel	);
		printk("[H1] now the clk data is %x\n",get_rbus_clk());

}

/* For 7581: max_emi_clk: 540MHz, max_bus_clk: 300MHz, max_fe_clk: 270MHz, max_npu_clk: 800MHz */
void RbusDynamicChangeClkTtest_asic(void)
{
    #if defined(TCSUPPORT_CPU_EN7581)
    unsigned int gsw_clksrc[] = {400, 500};             /*0x1fa201b4[8]*/
    unsigned int emi_clksrc[] = {540, 480, 400, 300};   /*0x1fa201b8[9:8]*/
    unsigned int bus_clksrc[] = {600, 540};             /*0x1fa201bc[8], no div1*/
    unsigned int fe2x_clksrc[]= {600, 540, 480, 400};   /*0x1fa201c0[9:8]*/
    unsigned int npu_clksrc[] = {800, 750, 720, 600};   /*0x1fa201fc[9:8]*/
    unsigned int cpu_clksrc[] = {500,550,600,650,700,750,800,850,900,950,1000,1050,1100,1150,1200};
    int num_clksrc[] =          {2, 4, 2, 4, 4, 15};
    unsigned int max_fe2x_clk=540;
    #else
    unsigned int gsw_clksrc[] = {400, 500};             /*0x1fa201b4[8]*/
    unsigned int emi_clksrc[] = {333, 400};             /*0x1fa201b8[8]*/
    unsigned int bus_clksrc[] = {500, 540};             /*0x1fa201bc[8], mo div1*/
    unsigned int fe2x_clksrc[]= {0};                    /*dummy, fe_clksrc is fixed*/
    unsigned int npu_clksrc[] = {333, 400, 500};        /*0x1fa201fc[9:8]*/
    unsigned int cpu_clksrc[] = {500,550,600,650,700,750,800,850,900,950};
    int num_clksrc[] =          {2, 2, 2, 1, 3, 10};
    #endif

    int num_div=8, shift_src=8;
    unsigned int *clksrcs[] = {gsw_clksrc, emi_clksrc, bus_clksrc, fe2x_clksrc, npu_clksrc, cpu_clksrc};
    char *clksrc_name[] = {"gsw", "emi", "bus", "fe", "npu", "cpu"};
    set_func_t set_clksrc_func[]= {SET_SCU_RGS_CLK_GSW, SET_SCU_RGS_CLK_EMI, SET_SCU_RGS_CLK_BUS, SET_SCU_RGS_CLK_FE, SET_SCU_RGS_CLK_NPU};
    get_func_t get_clksrc_func[]= {GET_SCU_RGS_CLK_GSW, GET_SCU_RGS_CLK_EMI, GET_SCU_RGS_CLK_BUS, GET_SCU_RGS_CLK_FE, GET_SCU_RGS_CLK_NPU}; 
    unsigned int *clksrc;
    int src;
    unsigned int src_sel, div_sel, clksrc_val;
    int cpuid = smp_processor_id();
    #if defined(TCSUPPORT_CPU_ARMV8_64)
    struct arm_smccc_res res;
    #endif
    
    for (src=0; src<clk_src_last; src++) {

        clksrc = clksrcs[src];
        src_sel = (read_c0_count()) % num_clksrc[src];

        #if defined(TCSUPPORT_CPU_ARMV8_64)
        if (src==clk_src_cpu) {
            printk("[H%d] %s freq:%d\n\n", cpuid, clksrc_name[src], clksrc[src_sel]);
	        arm_smccc_smc(ECNT_SIP_AVS_HANDLE, AVS_OP_FREQ_TEST, 0, src_sel, 0, 0, 0 ,0, &res);
            continue;
        }
        #endif
        
        if ((src==clk_src_gsw) || /* DE suggests not to adjust gsw_clk reg's div */
            (src==clk_src_npu) )  /* npu_clk must be larger than max_fe2x_clk, so just don't adjust div_sel */
            div_sel=0;
        else
            div_sel = (read_c0_count()) % num_div;

        if ((src==clk_src_bus)&&(div_sel==0)) /* bus' div_sel can't be 0 */
            div_sel=1;
        
        #if defined(TCSUPPORT_CPU_EN7581)
        if ((src==clk_src_fe) && ((clksrc[src_sel]/(div_sel+1))>max_fe2x_clk)) { /* fe's 2x clk can't exceed 540 */
            src_sel=1; div_sel=1;
        }
        #endif

        clksrc_val = ((src_sel<<shift_src)|div_sel);
        printk("[H%d] %s src_sel:0x%x, div_sel:0x%x, clksrc_val:0x%x\n", cpuid, clksrc_name[src], src_sel, div_sel, clksrc_val);
        set_clksrc_func[src](clksrc_val);
        printk("\t-> clk:%d/div:%d == %d MHz (RG_CLK:0x%x)\n\n",
                (int)clksrc[src_sel], (int)(div_sel+1), (int)(clksrc[src_sel]/(div_sel+1)), get_clksrc_func[src]());
    }

    return;
}

void RbusDynamicChangeClkTtest(void)
{
    if(isFPGA)
        RbusDynamicChangeClkTtest_fpga();
    else
        RbusDynamicChangeClkTtest_asic();
    return;
}
#endif

void log_print_func(unsigned long *cnt) {

    unsigned long printMask=0;
    int cpuid = smp_processor_id();
    int i, shift;


    if (cpuid==0) {
        shift = data0;
    }
    else if (cpuid==2) {
        shift = data2;
    }
    else if (cpuid==3) {
        shift = data3;
    }
    else { /*cpu1*/
        shift = data1;
    }
   
    for (i=0; i<shift; i++)
        printMask |= (1<<i);

    if (((*cnt)&printMask) == printMask) {
        #if 0
        printk("\n[H%d] Access test round:0x%lx\n", cpuId, (*cnt)); 
        #else /* reduce log when testing with NPU Cores */
        printk("[H%d(%lx)]", cpuid, *cnt);
        #endif
        #ifdef R2C_BYPASS_LOG
        if (cpuid==0) {
            printk(" -- r2c_rd_bypass_wt_cnt: 0x%lx", readl(r2c_bypassCnt_virtAddr));
        }        
        #endif
        printk("\n");
    }
    
    if ((*cnt) == ALL_BITS_ONE) 
        (*cnt)=0;
    else
        (*cnt)++;

    return;    
}

int read_compare_pcie_dev(unsigned long cmp_addr, unsigned int pcieAccBitMap /*bit0,1,2 for pcie0,1,2 respectively*/)
{
    volatile u32 *pcie_base, *cmp_base;
    int u32_len = sizeof(u32);
    unsigned int tWords=(PCIE_DEV_LEN/u32_len);
    int i, j, s;
    int cpuid = smp_processor_id();
    unsigned long offset;

    if ((is_pcie_devmem_size_got==0)||(isPcieDevIOremapped==0)) {
        printk("[H%d]ERROR(%s): is_pcie_devmem_size_got:%d or isPcieDevIOremapped:%d is zero\n", 
            cpuid, __func__, is_pcie_devmem_size_got, isPcieDevIOremapped);
        return -1;
    }

    /* read and compare pcie device mem */
    for (i=0; i<NUM_PCIE_DEV; i++) 
    {    
        if ( (is_pcie_link_up[i]==0) || ((pcieAccBitMap & (1<<i))==0) )
            continue;

        /* s==0,1,2 are for accessing pcie's first,middle,last PCIE_DEV_LEN bytes respectively. */
        for (s=0; s<NUM_PCIE_SEC; s++) {

            if (s==0) offset=0;
            else if (s==1) offset=(pcie_devmem_size[i]>>1);
            else offset=(pcie_devmem_size[i]-PCIE_DEV_LEN);
        
            cmp_base = (u32*)(cmp_addr+(PCIE_DEV_LEN*((s*NUM_PCIE_DEV)+i)));
            pcie_base = (u32*)(((unsigned long)pcie_dev_base[i])+offset);
            for (j=0; j<tWords; j++) {
                if (cmp_base[j] != readl(&pcie_base[j])) {
                    printk("[H%d]ERROR: cmp_base[%d]:0x%x != readl(&pcie_base[%d]):0x%x for pcie_dev%d, sec:%d\n", 
                            cpuid, j, cmp_base[j], j, readl(&pcie_base[j]), i, s);
                    printk("\tNote: check if wifi cards on pcie0 and pcie1 are 7915 and 7615 respectively\n");
                    return -1;
                }
            }
        }
    }

    return 0;
}

/* 
 * CPU0 cached access DRAM,
 *      uncached access Host_Regs/NPU_Regs/GDMP_SRAM/NPU_384K_SRAM/L2C_SRAM,
 *      monitor dram utilization. 
 */
int doCpu0AccessTest(void *arg) 
{
    unsigned long cached_addr=NULL;
    unsigned long cnt=0;
    unsigned long flags;
    int cpuid = smp_processor_id();
    spinlock_t accessLock;
    #if defined(MULTI_MODULES_SUPPORT) && (defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_AN7583))
    int wait_cnt = 100;
    #endif
    #ifdef R2C_BYPASS_LOG
    unsigned long virt_addr;
    #endif

    cached_addr = (unsigned long) kmalloc(BUS_CACHE_DRAM_TEST_SIZE, GFP_KERNEL);
    if (cached_addr==NULL) {
        printk("\nERROR(%s) kmalloc fail\n",__func__);
        return 0;
    }

    printk("\n[H%d] %s start\n", cpuid, __func__);

    #ifdef R2C_BYPASS_LOG
    virt_addr = ioremap(RG_DRAMC1_CONFIG, 4);
    printk("  -- is_r2c_rd_bypass_wt_enable: %d\n", ((readl(virt_addr))>>5)&0x1);
    iounmap(virt_addr);
    r2c_bypassCnt_virtAddr = ioremap(R2C_RDBYPASSWT_CNT, 4);
    #endif

    #ifdef MULTI_MODULES_SUPPORT
    pbus_regs_ioremap();
    #if defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_AN7583)
    while (g_pcieMemBackAddr==NULL) {
        if ((wait_cnt--)<=0) {
            printk("\nError: [H%d] wait timeout for g_pcieMemBackAddr \n", cpuid);
            kfree(cached_addr);
            return 0;
        }
        mdelay(10);
    }
    #endif
    #endif
    #ifdef LOCK_IRQOFF_TEST
    busTest_locking(&accessLock, &flags);
    #endif
    
    while (1)
    {
        if (ecnt_rw_ram_test(cached_addr, BUS_CACHE_DRAM_TEST_SIZE, cnt)==-1)
            break;

        if (cpuAccessRegsRams(cnt)==-1)
            break;

        #if !defined(NEW_PBUS_ARCH) /* 7581,7552 use pbus_monitor to replace bus_util */
        busUtiliCntCheck();
        #endif

        #if defined(MULTI_MODULES_SUPPORT) && (defined(TCSUPPORT_CPU_AN7552)||defined(TCSUPPORT_CPU_AN7583))
        if (g_pcieMemBackAddr) {
            if (read_compare_pcie_dev(g_pcieMemBackAddr, PCIE_EN_BITS)==-1)
                break;
        }
        #endif

        log_print_func(&cnt);

        #if !defined(LOCK_IRQOFF_TEST)
        msleep(1000);
        #endif
    }

    #ifdef LOCK_IRQOFF_TEST
    busTest_unlocking(&accessLock, &flags);
    #endif
    #ifdef MULTI_MODULES_SUPPORT
    pbus_regs_iounmap();
    #endif
    #ifdef R2C_BYPASS_LOG
    iounmap(r2c_bypassCnt_virtAddr);
    #endif
    
    kfree(cached_addr);
    printk("\n%s end\n", __func__);

    return 0;    
}

/* 
 * CPU1 uncached access DRAM,
 *      uncached access Host_Regs/NPU_Regs/GDMP_SRAM/NPU_384K_SRAM/L2C_SRAM,
 *      trigger gdma data moving between DRAM and GDMP_SRAM. 
 */
int doCpu1AccessTest(void *arg)
{    
    unsigned long uncached_addr=NULL;
    unsigned long cnt=0;
    dma_addr_t phy_srcAddr;
    unsigned long flags;
    struct device *dev=NULL;
    struct physAddr *physAddrP=NULL;
    unsigned long sram_base;
    unsigned long *sram_s_addr, *sram_d_addr;
    unsigned long *l2cSram_s_addr, *l2cSram_d_addr;
    unsigned long *dram_s_addr = NULL;
    unsigned long *dram_d_addr = NULL;
    dma_addr_t dram_s_phy, dram_d_phy;
    int cpuid = smp_processor_id();
    spinlock_t accessLock;
    #ifdef MULTI_MODULES_SUPPORT
    int pcie_no, burst;
    unsigned long dstUncAddr=NULL;
    dma_addr_t pcieBackDmaAddr, dstDmaAddr;
    #endif


    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("(%s) get_gdmpSram_dev is NULL\n", __func__);
        return 0;
    }

    uncached_addr = (unsigned long) dma_alloc_coherent(dev, BUS_DRAM_TEST_SIZE, &phy_srcAddr, GFP_KERNEL);
    if ((uncached_addr==NULL)) {
        printk("(%s) dma_alloc_coherent failed\n", __func__);
        return 0;
    }

    sram_base = (unsigned long)get_gdmpSram_base();
    sram_s_addr = (unsigned long *)(sram_base+SRAM_S_ADDR_OFF);
    sram_d_addr = (unsigned long *)(sram_base+SRAM_D_ADDR_OFF);

    sram_base = (unsigned long)get_l2cSramPbus_base();
    l2cSram_s_addr = (unsigned long *)(sram_base+(L2C_SRAM_TSIZE*cpuid));
    l2cSram_d_addr = (unsigned long *)(sram_base+(L2C_SRAM_TSIZE*cpuid)+(L2C_SRAM_TSIZE>>1));
    
    dram_s_addr = (unsigned long *)dma_alloc_coherent(dev, (TC_GDMA_TEST_SIZE+32), &dram_s_phy, GFP_KERNEL);
    if (dram_s_addr == NULL) {
            printk("\nError:dram_s_addr NULL\n");
            goto access_test_exit_0;
    }
    
    dram_d_addr = (unsigned long *)dma_alloc_coherent(dev, (TC_GDMA_TEST_SIZE+32)*GDMA_CH_NUM, &dram_d_phy, GFP_KERNEL);
    if ((dram_s_addr == NULL) || (dram_d_addr == NULL)) {
        printk("\nError:dram_s_addr or dram_d_addr is NULL\n");
        goto access_test_exit_1;
    }

    physAddrP = (struct physAddr *)kzalloc(sizeof(struct physAddr), GFP_KERNEL);
    if ((physAddrP == NULL)) {
        printk("\nphysAddrP is NULL\n");
        goto access_test_exit_2;
    }

    physAddrP->dram_s_phys=dram_s_phy;
    physAddrP->dram_d_phys=dram_d_phy;

    /* set GDMP sram to be accessed via PBUS (by CPU or GDMA) */
    set_share_unzipmem_sel(0);

    #ifdef MULTI_MODULES_SUPPORT
    g_pcieMemBackAddr = pcie_mem_backup(&pcieBackDmaAddr);
    if ((g_pcieMemBackAddr==NULL)) {
        printk("[H%d](%s) pcie_mem_backup failed\n", cpuid, __func__);
        goto access_test_exit_3;
    }

    dstUncAddr = (unsigned long) dma_alloc_coherent(dev, (PCIE_DEV_LEN<<1), &dstDmaAddr, GFP_KERNEL);
    if ((dstUncAddr==NULL)) {
        printk("[H%d](%s) dma_alloc_coherent failed\n", cpuid, __func__);
        goto access_test_exit_4;
    }
    #endif

    #if 0
    printk("sram_s_addr:0x%lx  sram_s_phys:0x%lx\n", sram_s_addr, physAddrP->sram_s_phys);
    printk("sram_d_addr:0x%lx  sram_d_phys:0x%lx\n", sram_d_addr, physAddrP->sram_d_phys);
    printk("dram_s_addr:0x%lx  dram_s_phys:0x%lx\n", dram_s_addr, physAddrP->dram_s_phys);
    printk("dram_d_addr:0x%lx  dram_d_phys:0x%lx\n", dram_d_addr, physAddrP->dram_d_phys);    
    #endif


    printk("\n[H%d] %s start\n", cpuid, __func__);

    
    #ifdef MULTI_MODULES_SUPPORT
    pbus_regs_ioremap();
    #endif
    #ifdef LOCK_IRQOFF_TEST
    busTest_locking(&accessLock, &flags);
    #endif
    
    while (1)
    {
        if (ecnt_rw_ram_test(uncached_addr, BUS_DRAM_TEST_SIZE, cnt)==-1)
            break;

        if (cpuAccessRegsRams(cnt)==-1)
            break;

        /* gdma data moving between gdmpSram and DRAM */
        physAddrP->sram_s_phys=(SRAM_GDMP_PHY_BASE+SRAM_S_ADDR_OFF);
        physAddrP->sram_d_phys=(SRAM_GDMP_PHY_BASE+SRAM_D_ADDR_OFF);
        if (gdmacopy(dram_d_addr, dram_s_addr,sram_d_addr, sram_s_addr, physAddrP, 1)==-1)
            break;

        /* gdma data moving between l2cSram and DRAM */
        if (get_l2c_sram_size()==L2C_SRAM_SIZE) {       
            physAddrP->sram_s_phys=(L2C_SRAM_PBUS_BASE+(L2C_SRAM_TSIZE*cpuid));
            physAddrP->sram_d_phys=(L2C_SRAM_PBUS_BASE+(L2C_SRAM_TSIZE*cpuid)+(L2C_SRAM_TSIZE>>1));
            if (gdmacopy(dram_d_addr, dram_s_addr,l2cSram_d_addr, l2cSram_s_addr, physAddrP, 1)==-1)
                break;
        }

        #ifdef MULTI_MODULES_SUPPORT
        /* gdma data moving from pcieDev to DRAM */
        for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {
            
            if (is_pcie_link_up[pcie_no]==0) continue;
            
            for(burst=0; burst<NUM_GDMA_BURST; burst++) { /* burst:0/1/2/3/4 means 1/2/4/8/16 DW which is 4/8/16/32/64 bytes */
                if (gdma_read_pcie_dev_mem(pcie_no, dstUncAddr, g_pcieMemBackAddr, dstDmaAddr, burst)==-1) {
                    printk("[H%d] Error(%s) at pcie:%d, burst:%d\n", cpuid, __func__, pcie_no, burst);
                    goto access_test_exit_5;
                }
            }
        }
        #endif
        
        #ifdef CLK_CHANGE_TEST
		/* only for Rbus clk ctl test*/
		if ((cnt&0x1) == 0x1)
		{
			RbusDynamicChangeClkTtest();
		}
        #endif

        log_print_func(&cnt);

        #if !defined(LOCK_IRQOFF_TEST)
        msleep(1000);
        #endif
    }

access_test_exit_5:
    #ifdef LOCK_IRQOFF_TEST
    busTest_unlocking(&accessLock, &flags);
    #endif
    #ifdef MULTI_MODULES_SUPPORT
    pbus_regs_iounmap();
    #endif
#ifdef MULTI_MODULES_SUPPORT
    dma_free_coherent(dev, (PCIE_DEV_LEN<<1), dstUncAddr, dstDmaAddr);
access_test_exit_4:
    pcie_mem_free(g_pcieMemBackAddr, pcieBackDmaAddr); 
    g_pcieMemBackAddr=NULL;
access_test_exit_3:
#endif
    kfree(physAddrP);
access_test_exit_2:
    dma_free_coherent(dev, (TC_GDMA_TEST_SIZE+32)*GDMA_CH_NUM, dram_d_addr, dram_d_phy);
access_test_exit_1:
    dma_free_coherent(dev, (TC_GDMA_TEST_SIZE+32), dram_s_addr, dram_s_phy);
access_test_exit_0:
    dma_free_coherent(dev, BUS_DRAM_TEST_SIZE, uncached_addr, phy_srcAddr);
    
    printk("\n%s end\n", __func__);
    return 0;    
}

int doCpuAccessTest(void) 
{
    unsigned long cached_addr=NULL;
    unsigned long uncached_addr=NULL;
    dma_addr_t phy_srcAddr;
    unsigned long cnt=0;
    unsigned long flags;
    int cpuid = smp_processor_id();
    struct device *dev;
    int wait_cnt=100;
    spinlock_t accessLock;
    

    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("[H%d](%s) get_gdmpSram_dev is NULL\n", cpuid, __func__);
        return 0;
    }

    cached_addr = (unsigned long) kmalloc(BUS_CACHE_DRAM_TEST_SIZE, GFP_KERNEL);
    if (cached_addr==NULL) {
        printk("\n[H%d]ERROR(%s) kmalloc fail\n", cpuid,__func__);
        return 0;
    }

    uncached_addr = (unsigned long) dma_alloc_coherent(dev, BUS_DRAM_TEST_SIZE, &phy_srcAddr, GFP_KERNEL);
    if ((uncached_addr==NULL)) {
        kfree(cached_addr);
        printk("[H%d](%s) dma_alloc_coherent failed\n", cpuid, __func__);
        return 0;
    }

    printk("\n[H%d] %s start\n", cpuid, __func__);

    
    #ifdef MULTI_MODULES_SUPPORT
    pbus_regs_ioremap();
    while (g_pcieMemBackAddr==NULL) {
        if ((wait_cnt--)<=0) {
            printk("\nError: [H%d] wait timeout for g_pcieMemBackAddr \n", cpuid);
            kfree(cached_addr);
            dma_free_coherent(dev, BUS_DRAM_TEST_SIZE, uncached_addr, phy_srcAddr);
            return 0;
        }
        mdelay(10);
    }
    #endif
    #ifdef LOCK_IRQOFF_TEST
    busTest_locking(&accessLock, &flags);
    #endif
    
    while (1)
    {
        if (ecnt_rw_ram_test(cached_addr, BUS_CACHE_DRAM_TEST_SIZE, cnt)==-1) {
            printk("\nError: [H%d] cached access DRAM failed\n", cpuid);
            break;
        }
        
        if (ecnt_rw_ram_test(uncached_addr, BUS_DRAM_TEST_SIZE, cnt)==-1) {
            printk("\nError: [H%d] uncached access DRAM failed\n", cpuid);
            break;
        }
    
        if (cpuAccessRegsRams(cnt)==-1)
            break;

        #ifdef MULTI_MODULES_SUPPORT
        if (g_pcieMemBackAddr) {
            if (read_compare_pcie_dev(g_pcieMemBackAddr, PCIE_EN_BITS)==-1)
                break;
        }
        #endif
            
        log_print_func(&cnt);

        #if !defined(LOCK_IRQOFF_TEST)
        msleep(1000);
        #endif
    }

    #ifdef LOCK_IRQOFF_TEST
    busTest_unlocking(&accessLock, &flags);
    #endif
    #ifdef MULTI_MODULES_SUPPORT
    pbus_regs_iounmap();
    #endif
    
    kfree(cached_addr);
    dma_free_coherent(dev, BUS_DRAM_TEST_SIZE, uncached_addr, phy_srcAddr);
    printk("[H%d]\n%s end\n", cpuid, __func__);

    return 0;    
}

int doCpu2AccessTest(void *arg) 
{
    return doCpuAccessTest();
}

int doCpu3AccessTest(void *arg) 
{
     return doCpuAccessTest(); 
}

void pbus_regs_ioremap(void)
{
    int i;
    int cpuid = smp_processor_id();

    if (cpuid==0) {
        PbusRegs.apb = ioremap(TEST_REG_APB, 4);
        PbusRegs.tzpc = ioremap(TEST_REG_TZPC, 4);
        PbusRegs.sec = ioremap(TEST_REG_SEC, 4);
    }

    for (i=0; i<NUM_PCIE_MAC_REG; i++) {
        PbusRegs.pcie[cpuid][i] = ioremap(pcie_writable_regs[cpuid][i], 4);
    }

    PbusRegs.resv[cpuid] = ioremap(pbus_resv_regs[cpuid], 4);

    #if 0 /*debug*/
    if (cpuid==0) {
        printk("[H%d]PbusRegs.apb:0x%lx\n", cpuid, PbusRegs.apb);
        printk("[H%d]PbusRegs.tzpc:0x%lx\n", cpuid, PbusRegs.tzpc);
        printk("[H%d]PbusRegs.sec:0x%lx\n", cpuid, PbusRegs.sec);
    }
    for (i=0; i<NUM_PCIE_MAC_REG; i++)
        printk("[H%d]PbusRegs.pcie[%d][%d]:0x%lx\n", cpuid, cpuid, i, PbusRegs.pcie[cpuid][i]);
    printk("[H%d]PbusRegs.resv[%d]:0x%lx\n", cpuid, cpuid, PbusRegs.resv[cpuid]);
    #endif

    return;
}

int pcie_dev_mem_ioremap(void)
{
    int i;

    if (isPcieDevIOremapped) 
        return 0;

    host_check_pcie_link_up();

    if (pcie_dev_mem_size_get())
        return -1;

    for (i=0; i<NUM_PCIE_DEV; i++) {
        
        #ifdef NEW_PBUS_ARCH
        pcie_dev_addr[i] = pcie_mem_base_get(i);
        #else
        pcie_dev_addr[i] = PCIE0_MEM_BASE+(0x1000000*i);
        #endif
        pcie_dev_base[i] = ioremap(pcie_dev_addr[i], pcie_devmem_size[i]);
        #if 0
        printk("pcie_dev_addr[%d]:0x%lx, pcie_dev_base[%d]:0x%lx\n", i, pcie_dev_addr[i], i, pcie_dev_base[i]);
        #endif
    }

    isPcieDevIOremapped=1;
    return 0;
}

void pbus_regs_iounmap(void)
{
    int i;
    int cpuid = smp_processor_id();

    if (cpuid==0) {
        iounmap(PbusRegs.apb);
        iounmap(PbusRegs.tzpc);
        iounmap(PbusRegs.sec);
    }
    
    for (i=0; i<NUM_PCIE_MAC_REG; i++) {
        iounmap(PbusRegs.pcie[cpuid][i]);
    }
    iounmap(PbusRegs.resv[cpuid]);

    return;
}

void pcie_dev_mem_iounmap(void)
{
    int i;

    if (isPcieDevIOremapped==0) return;

    for (i=0; i<NUM_PCIE_DEV; i++) {
        iounmap(pcie_dev_base[i]);
    }

    isPcieDevIOremapped=0;
    return;
}

#ifdef NEW_PBUS_ARCH
/* For CPU Master only (including cpu0~3) access:
 *  - cpu pre-multi-issue counter should keep counting
 *  - cpu post-multi-issue, all pcie post-multi-issue and burst-cmd counters should be 0 
 */
int cpu_multi_issue_cnt_check(unsigned long cnt)
{
    u32 curr_cnt;
    u32 cnt_mask = 0;
    int cpuid = smp_processor_id();
    int i;
    static u32 pre_pbus_dbg_cnt=0;

    for (i=0; i<(data0-1); i++)
        cnt_mask |= (1<<i); /* print frequency */

    /* cpu post-multi-issue, all pcie post-multi-issue and burst-cmd counters should be 0 */
    #if defined(TCSUPPORT_CPU_AN7552)
    /* 7552 pcie uses pbus, instead of AXI bus, to connect to system. Pbus does,'t support multi-issue and
     * burst cmds, so dcnt_pcieX_xxx_burst_cmd and dcnt_pcieX_xxx_multi_issue counters are removed. */
    for (i=dcnt_cpu_post_multi_issue; i<=dcnt_16p_arb_pre_multi_issue; i++) {
    #else /* 7581 */
    for (i=dcnt_cpu_post_multi_issue; i<=dcnt_pcie2_post_burst_cmd; i++) {
    #endif
        if ((i==dcnt_rbus_pcie0_bypass) || (i==dcnt_rbus_pcie0_block)) 
            continue;
        if (pbus_dbg_cnt_get(i)) {
            printk("\n[H%d]Error: newPbusDbgCnt(%d):0x%x !=0 (at round%d)\n", cpuid, i, pbus_dbg_cnt_get(i), cnt);
            return -1;
        }
    }

    /* cpu pre-multi-issue counter should keep counting */
    if ((cnt&cnt_mask)==cnt_mask) {
        curr_cnt = pbus_dbg_cnt_get(dcnt_cpu_pre_multi_issue);
        if (curr_cnt==pre_pbus_dbg_cnt) {
            printk("\n[H%d]Error: cpu pre_multi_issue_cnt has not counted for 0x%x rounds (at round%d)\n", cpuid, cnt_mask, cnt);
            return -1;
        }
        printk("[H%d] cpu pre_multi_issue_cnt:0x%x\n", cpuid, curr_cnt);
        pre_pbus_dbg_cnt = curr_cnt;
    }

    return 0;
}
#endif

void new_pbus_access_test (int arg)
{
    spinlock_t cpuPbusLock;
    unsigned long flags;
    unsigned long cnt=0;
    #ifdef R2C_BYPASS_LOG
    unsigned long virt_addr;
    #endif

    data0=arg; /*for log_print_func*/
    pbus_regs_ioremap();
    busTest_locking(&cpuPbusLock, &flags);

    #ifdef R2C_BYPASS_LOG
    virt_addr = ioremap(RG_DRAMC1_CONFIG, 4);
    printk("  -- is_r2c_rd_bypass_wt_enable: %d\n", ((readl(virt_addr))>>5)&0x1);
    iounmap(virt_addr);
    r2c_bypassCnt_virtAddr = ioremap(R2C_RDBYPASSWT_CNT, 4);
    #endif    
    
    while (1)
    {
        if (cpuAccessRegsRams(cnt)==-1)
            isNewPbusTestDone=1;

        #ifdef NEW_PBUS_ARCH
        if (newPbusTestCase==npCase_no_multi_issue) {
            if (cpu_multi_issue_cnt_check(cnt)==-1)
                isNewPbusTestDone=1;
        }
        #endif
        
        log_print_func(&cnt);
        if (isNewPbusTestDone) break;
    }
    
    /* iounmap can only be done when irq_disable, so irqrestore before iounmap */
    busTest_unlocking(&cpuPbusLock, &flags);
    pbus_regs_iounmap();
    return;
}

void host_check_pcie_link_up(void)
{
    int pcie_no;
    unsigned int reg, sts_bit;

    for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {
        #if 1 //TreyDbg regRead_PCIe will crash if the pcie_no is not link up
        if (pcie_no==2) {
            is_pcie_link_up[pcie_no]=0;
            printk("[H] is_pcie_link_up[%d] is fixed to 0 currently\n", pcie_no);
            continue;
        }
        #endif
        #ifdef TCSUPPORT_CPU_AN7583
        if (pcie_no==0) {
            reg = CR_PCIE0_STATUS;
            sts_bit = PCIE0_STS_BIT;
        }
        else {
            reg = CR_PCIE1_STATUS;
            sts_bit = PCIE1_STS_BIT;
        }
        
        #else
        reg = CR_PCIE0_STATUS+(PCIE_STS_OFFSET*pcie_no);
        sts_bit = PCIE_STS_BIT;
        #endif
        
        if (regRead_PCIe(reg)&(1<<sts_bit))
            is_pcie_link_up[pcie_no]=1;
        else
            is_pcie_link_up[pcie_no]=0;
            
        printk("[H] is_pcie_link_up[%d]==%d\n", pcie_no, is_pcie_link_up[pcie_no]);
    }
    
    return;
}

/* coding steps are provided by Hui Ma */
int pcie_dev_mem_size_get(void)
{
    int pcie_no, i, num, num_pcie;
    unsigned int base, cfg0, cfg1, cfg2, cfg3;
    unsigned int val1, val2, val3, val4;
    unsigned int tmp[NUM_PCIE_CFG], val[NUM_PCIE_CFG];
    int cpuid = smp_processor_id();

    if (is_pcie_devmem_size_got)
        return 0;

    /* check if NPU has done pcie_dev_mem_size_get, if yes, get pcie_devmem_size from MIB regs. 
     * don't do pcie_dev_mem_size_get while NPU is reading pcie_devmem, that will change devmem's value, 
     * resulting in npu's comparing data fail. */
    if (num_pcie=get_npu_mbox_mib(31)) {

        printk("[H%d] get %d pcie_devmem_size from NPU\n", cpuid, num_pcie);
        for (pcie_no=0; pcie_no<num_pcie; pcie_no++) {
            pcie_devmem_size[pcie_no] = get_npu_mbox_mib(30-pcie_no);
            printk("[H%d] pcie_devmem_size[%d]:0x%x\n", cpuid, pcie_no, pcie_devmem_size[pcie_no]);
        }
        is_pcie_devmem_size_got=1;
        return 0;
    }

    for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {

        if (is_pcie_link_up[pcie_no]==0) continue;

        #if defined(TCSUPPORT_CPU_AN7583)
        pcie_devmem_size[pcie_no] = 0x2000;
        
        #elif defined(TCSUPPORT_CPU_AN7552)
        cfg0 = REG_PCIE0_BASE+(pcie_no*PCIE_BASE_OFFSET);
        val1 = (regRead_PCIe(cfg0) &0x1f);        
        pcie_devmem_size[pcie_no] = (1<<(val1-1));

        #else /* 7581 */
        base = REG_PCIE0_BASE+(pcie_no*PCIE_BASE_OFFSET);
        cfg0 = base + PCIE_CFG0_OFFSET;
        cfg1 = base + PCIE_CFG1_OFFSET;
        cfg2 = base + PCIE_CFG2_OFFSET;
        cfg3 = base + PCIE_CFG3_OFFSET;
        
        regWrite_PCIe(cfg0, 0x1f0100);
        val1 = regRead_PCIe(cfg1);
        val2 = regRead_PCIe(cfg2);
        val3 = regRead_PCIe(cfg3);

        regWrite_PCIe(cfg1, 0xffffffff);
        regWrite_PCIe(cfg2, 0xffffffff);
        regWrite_PCIe(cfg3, 0xffffffff);

        tmp[0] = (regRead_PCIe(cfg1) &0xffffff80);
        tmp[1] = (regRead_PCIe(cfg2) &0xffffff80);
        tmp[2] = (regRead_PCIe(cfg3) &0xffffff80);

        regWrite_PCIe(cfg1, val1);
        regWrite_PCIe(cfg2, val2);
        regWrite_PCIe(cfg3, val3);
        
        /* store value>0 */
        for (i=0, num=0; i<NUM_PCIE_CFG; i++) {
            if (tmp[i]>0) {
                val[num] = tmp[i];
                num++;
            }
        }

        if (num==0) {
            printk("ERROR(%s): all values are 0 for pcie%d\n", __func__, pcie_no);
            return -1;
        }
        else if (num==1) {
            val4 = val[0];
        }
        else {
            /* find the smallest one */
            for (i=1, val4=val[0]; i<num; i++) {
                if (val4>val[i])
                    val4 = val[i];
            }
        }

        pcie_devmem_size[pcie_no] = ((~val4)+1);
        #endif
        #if defined(INTEL_WIFI_CARD) || defined(WIFI_7916)
        /* fix pcie_devmem_size becase the value for rest of it may change */
        #if defined(WIFI_7916)
        pcie_devmem_size[pcie_no] = 0x2000;
        #else
        pcie_devmem_size[pcie_no] = 0xFC2000;
        #endif
        printk("[H%d] Fix pcie_devmem_size[%d] to 0x%x\n", cpuid, pcie_no, pcie_devmem_size[pcie_no]);
        #else
        printk("[H%d] pcie_devmem_size[%d]:0x%x\n", cpuid, pcie_no, pcie_devmem_size[pcie_no]);
        #endif
    }

    is_pcie_devmem_size_got=1;
    return 0;
}

/* Stored pcie devmem layout(in DRAM):
 *  -- 1st PCIE_DEV_LEN bytes for pcie0's first PCIE_DEV_LEN bytes
 *  -- 2nd PCIE_DEV_LEN bytes for pcie1's first PCIE_DEV_LEN bytes
 *  -- 3rd PCIE_DEV_LEN bytes for pcie2's first PCIE_DEV_LEN bytes
 *  -- 4th PCIE_DEV_LEN bytes for pcie0's middle PCIE_DEV_LEN bytes
 *  -- 5th PCIE_DEV_LEN bytes for pcie1's middle PCIE_DEV_LEN bytes
 *  -- 6th PCIE_DEV_LEN bytes for pcie2's middle PCIE_DEV_LEN bytes
 *  -- 7th PCIE_DEV_LEN bytes for pcie0's last PCIE_DEV_LEN bytes
 *  -- 8th PCIE_DEV_LEN bytes for pcie1's last PCIE_DEV_LEN bytes
 *  -- 9th PCIE_DEV_LEN bytes for pcie2's last PCIE_DEV_LEN bytes
 */
unsigned long pcie_mem_backup(dma_addr_t *dmaAddrP)
{
    int cpuid = smp_processor_id();
    struct device *dev=NULL;
    unsigned long unc_addr=NULL, offset;
    int i, j, s;
    int u32_len = sizeof(u32);
    unsigned int tWords=(PCIE_DEV_LEN/u32_len);
    volatile u32 *pcie_base, *unc_base, tmpVal;
    dma_addr_t dmaAddr;


    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("[H%d](%s) get_gdmpSram_dev is NULL\n", cpuid, __func__);
        return NULL;
    }

    unc_addr = (unsigned long) dma_alloc_coherent(dev, PCIE_DEV_LEN*NUM_PCIE_DEV*NUM_PCIE_SEC, &dmaAddr, GFP_KERNEL);
    if ((unc_addr==NULL)) {
        printk("[H%d](%s) dma_alloc_coherent failed\n", cpuid, __func__);
        return NULL;
    }
    
    if (pcie_dev_mem_ioremap()) {
        printk("[H%d](%s) pcie devmem_ioremap failed\n", cpuid, __func__);
        dma_free_coherent(dev, PCIE_DEV_LEN*NUM_PCIE_DEV*NUM_PCIE_SEC, unc_addr, dmaAddr);
        return NULL;
    }
    
    /* store pcie device mem */
    /* s==0,1,2 are for accessing pcie's first,middle,last PCIE_DEV_LEN bytes respectively. */
    for (s=0; s<NUM_PCIE_SEC; s++) {
        
        for (i=0; i<NUM_PCIE_DEV; i++) {

            if (is_pcie_link_up[i]==0) continue;

            if (s==0) offset=0;
            else if (s==1) offset=(pcie_devmem_size[i]>>1);
            else offset=(pcie_devmem_size[i]-PCIE_DEV_LEN);
            
            unc_base = (u32*)(unc_addr+(PCIE_DEV_LEN*((s*NUM_PCIE_DEV)+i)));
            pcie_base = (u32*)(((unsigned long)pcie_dev_base[i])+offset);
            
            for (j=0; j<tWords; j++) {
                tmpVal = pcie_base[j];
                if (tmpVal==0xdeadbeef) { /* if deadbeef, that means the address can't be accessed. */
                    printk("[H%d](%s) ERROR: tmpVal:0x%x ==0xdeadbeef at word%d,pcie%d,offset:0x%x\n", 
                            cpuid, __func__, tmpVal, j, i, offset);
                    printk("\t-> pcie_base:0x%lx, pcie_dev_base:0x%lx, pcie_dev_addr:0x%lx\n", 
                            pcie_base, pcie_dev_base[i], pcie_dev_addr[i]);
                    dma_free_coherent(dev, PCIE_DEV_LEN*NUM_PCIE_DEV*NUM_PCIE_SEC, unc_addr, dmaAddr);
                    pcie_dev_mem_iounmap();
                    return NULL;
                }
                unc_base[j] = tmpVal;
            }
        }
    }

    (*dmaAddrP) = dmaAddr;
    return unc_addr;
}

void pcie_mem_free(unsigned long virtAddr, dma_addr_t dmaAddr)
{
    struct device *dev=get_gdmpSram_dev();
    
    pcie_dev_mem_iounmap();
    dma_free_coherent(dev, PCIE_DEV_LEN*NUM_PCIE_DEV*NUM_PCIE_SEC, virtAddr, dmaAddr);
    return;
}

/* CPU0 access original pbus path while cpu1~3 accesses pcie_dev path. 
 * This test is to prevent the scenario: 
 *  - if CPU1 reads pcie_dev first, then CPU0 reads uart register.
 *  - Because pcie_dev path has longer latency, if multi-issue can happen,
 *  - CPU1 may get response from uart register, which is wrong! */
void no_multi_issue_test(int arg)
{
    int cpuid = smp_processor_id();
    spinlock_t cpuPbusLock;
    unsigned long flags;
    unsigned long cnt=0;
    int pcie_id;
    static dma_addr_t dmaAddr;
    static volatile unsigned long unc_addr=NULL;

    if (cpuid<NUM_CPUS)
        printk("[H%d] do %s (arg:%d)\n", cpuid, __func__, arg);
    else {
        printk("\nERROR: Wrong cpuid:%d\n", cpuid);
        return;
    }

    if (cpuid==0) {
        new_pbus_access_test(arg);
    }
    else /* cpuid==1~3 for 7581, cpuid==1 for 7552/7583 */ {

        if (cpuid==1) {
            data1=(arg+8); /*for log_print_func*/
            data2=(arg+8); /*for log_print_func*/
            data3=(arg+8); /*for log_print_func*/
            unc_addr = pcie_mem_backup(&dmaAddr);
            if ((unc_addr==NULL)) {
                printk("[H%d](%s) pcie_mem_backup failed\n", cpuid, __func__);
                return;
            }
        }
        else { /*cpu2,3*/
            while (unc_addr==NULL) {} /* wait for pcie mem backup address */
        }

        pcie_id = cpuid-1; /* cpu1~3 access pcie0~2 respecively */
        if (is_pcie_link_up[pcie_id]==0) {
            printk("[H%d](%s) just return due to is_pcie_link_up[%d]==0\n", cpuid, __func__, pcie_id);
            return;
        }
        busTest_locking(&cpuPbusLock, &flags);

        while (1)
        {
            if (read_compare_pcie_dev(unc_addr, (1<<(pcie_id)))==-1)
                isNewPbusTestDone=1;
            
            log_print_func(&cnt);
            if (isNewPbusTestDone) break;

            #if defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_AN7583)
            /* 7552 has only 2 cpus, so let cpu1 reads all pcie devs sequentially */
            pcie_id++;
            if (pcie_id==NUM_PCIE_DEV) pcie_id=0;
            #endif
        }

        /* iounmap can be done when irq_disable, so irqrestore before iounmap */
        busTest_unlocking(&cpuPbusLock, &flags);
        if (cpuid==1)
            pcie_mem_free(unc_addr, dmaAddr);
    }
    
    return;
}

/* CPU, GDMA, NPU Masters do stress test on original pbus path at the same time.
 * Note: NPU starts stress test by "echo 17 > /proc/npu_test_cases". */
void r_single_normal_test(int arg)
{
    int cpuid = smp_processor_id();
    unsigned long sram_base;
    unsigned long *sram_s_addr, *sram_d_addr;
    spinlock_t cpuPbusLock;
    unsigned long flags;
    unsigned long cnt=0;
    uint32 src_phy_base=0, dst_phy_base=0;

    if (cpuid<=1) {
        printk("[H%d] do %s (arg:%d)\n", cpuid, __func__, arg);
    }
    else {
        printk("[H%d] no need, just return!\n", cpuid);
        return;
    }


    if (cpuid==0) {
        new_pbus_access_test(arg);
    }
    else /*(cpuid==1)*/ {

        data1=(arg); /*for log_print_func*/

        sram_base = (unsigned long)get_gdmpSram_base();
        sram_s_addr = (unsigned long *)(sram_base+SRAM_S_ADDR_OFF);
        sram_d_addr = (unsigned long *)(sram_base+SRAM_D_ADDR_OFF);
        src_phy_base=(SRAM_GDMP_PHY_BASE+SRAM_S_ADDR_OFF);
        dst_phy_base=(SRAM_GDMP_PHY_BASE+SRAM_D_ADDR_OFF);

        #if 1
        printk("sram_src_virt_base:0x%lx  sram_dst_virt_base:0x%lx\n", sram_s_addr, sram_d_addr);
        printk("sram_src_phys_base:0x%lx  sram_dst_phys_base:0x%lx\n", src_phy_base, dst_phy_base);
        #endif

        /* set GDMP sram to be accessed via PBUS (by CPU or GDMA) */
        set_share_unzipmem_sel(0);

        busTest_locking(&cpuPbusLock, &flags);

        while (1)
        {
    		if( gdmatest(sram_d_addr, sram_s_addr, TC_GDMA_TEST_SIZE, src_phy_base, dst_phy_base) == -1 ){		
    			printk("\n[H%d] GDMA SRAM Copy to SRAM Test Fail!!\r\n", cpuid);
    			isNewPbusTestDone=1;
    		}

            log_print_func(&cnt);
            if (isNewPbusTestDone) break;
        }

        busTest_unlocking(&cpuPbusLock, &flags);        
    }
    
    return;

}

int single_pcie_dcnt_check( int pcie_no, int isMultiMasters,
                            u32 expectPreBurstCnt, u32 expectPostBurstCnt, 
                            u32 expectPreMultiCnt, u32 expectPostMultiCnt)
{
#ifdef NEW_PBUS_ARCH
    u32 offset = 3*pcie_no;
    u32 realPreBurstCnt = pbus_dbg_cnt_get(dcnt_pcie0_pre_burst_cmd+offset);
    u32 realPostBurstCnt = pbus_dbg_cnt_get(dcnt_pcie0_post_burst_cmd+offset);
    u32 realPreMultiCnt = pbus_dbg_cnt_get(dcnt_pcie0_pre_multi_issue+pcie_no);
    u32 realPostMultiCnt = pbus_dbg_cnt_get(dcnt_pcie0_post_multi_issue+offset);

    /* when multiple Masters access pcie at the same time, it's hard to check counters precisely.
     * Instead, just check if counter is zero or non-zero as expected. */
    if (isMultiMasters) {
        if ( ((expectPreBurstCnt!=0) &&(realPreBurstCnt==0))  || ((expectPreBurstCnt==0) &&(realPreBurstCnt!=0))  ||
             ((expectPostBurstCnt!=0)&&(realPostBurstCnt==0)) || ((expectPostBurstCnt==0)&&(realPostBurstCnt!=0)) ||
             ((expectPreMultiCnt!=0) &&(realPreMultiCnt==0))  || ((expectPreMultiCnt==0) &&(realPreMultiCnt!=0))  ||
             ((expectPostMultiCnt!=0)&&(realPostMultiCnt==0)) || ((expectPostMultiCnt==0)&&(realPostMultiCnt!=0)) )
        {
            printk("\nError:\n");
            printk("\tisPreBurstCounts: %d  not matching realPreBurstCnt: 0x%x or\n", expectPreBurstCnt,  realPreBurstCnt);
            printk("\tisPostBurstCounts:%d  not matching realPostBurstCnt:0x%x or\n", expectPostBurstCnt, realPostBurstCnt);
            printk("\tisPreMultiCounts: %d  not matching realPreMultiCnt: 0x%x or\n", expectPreMultiCnt,  realPreMultiCnt);
            printk("\tisPostMultiCounts:%d  not matching realPostMultiCnt:0x%x \n\n", expectPostMultiCnt, realPostMultiCnt);
            return -1;
        }
    }
    else {
        if ( (expectPreBurstCnt!=realPreBurstCnt) || (expectPostBurstCnt!=realPostBurstCnt) ||
                 (expectPreMultiCnt!=realPreMultiCnt) || (expectPostMultiCnt!=realPostMultiCnt) )
        {
            printk("\nError:\n");
            printk("\texpectPreBurstCounts: 0x%x  != realPreBurstCnt: 0x%x or\n", expectPreBurstCnt,  realPreBurstCnt);
            printk("\texpectPostBurstCounts:0x%x  != realPostBurstCnt:0x%x or\n", expectPostBurstCnt, realPostBurstCnt);
            printk("\texpectPreMultiCounts: 0x%x  != realPreMultiCnt: 0x%x or\n", expectPreMultiCnt,  realPreMultiCnt);
            printk("\texpectPostMultiCounts:0x%x  != realPostMultiCnt:0x%x \n\n", expectPostMultiCnt, realPostMultiCnt);
            return -1;
        }
    }
#endif
    return 0;
}

/*
 * NPU reads data from pcie_no dev, writes data to dstPhysAddr, and compare data with cmpPhysAddr.
 *
 * flags:
 *  - 0x0: stop NPU from continuous mode.
 *  - 0x1: host/npu need to compare result after reading.
 *  - 0x2: npu in Continuous Mode, no need to compare. 
 *  - 0x3: for pbus timeout test. 
 *    - npu just does 4-byte read for one time.
 *    - pcie_no stands for element_no in tout_test_regs[]
 */
int npu_read_pcie_dev_mem(int pcie_no, u32 dstPhysAddr, u32 cmpPhysAddr, u32 flags)
{
    int cpuid = smp_processor_id();
    u32 srcPhysAddr, testLen;
    int isCheckResult = 0;
    int isForPbusTout = 0;

    if (flags==0x1)
        isCheckResult=1;
    if (flags==0x3)
        isForPbusTout=1;

    if (((isForPbusTout==0) && ((pcie_no>=NUM_PCIE_DEV)||(is_pcie_link_up[pcie_no]==0))) ||
        /* for isForPbusTout==1, pcie_no stands for element_no in tout_test_regs[] */
        ((isForPbusTout==1) && (pcie_no>=NUM_TEST_REGS))) {
        return 0;
    }

    set_npu_mbox_mib(15, flags); /* flags for npu */

    if (is_npu_idle()) /* make sure npu is idle */
    {
        /* check previous status of npu_do_dma_test */
        if ((isCheckResult) && (get_npu_mbox_mib(14)==1)) { 
            set_npu_mbox_mib(14, 0);
            printk("[H%d](%s) Failed\n", cpuid, __func__);
            return -1;
        }

        #ifdef NEW_PBUS_ARCH
        if (isForPbusTout) {
            srcPhysAddr = tout_test_regs[pcie_no];
            testLen=4;
        }
        else {
            srcPhysAddr = pcie_mem_base_get(pcie_no);
            testLen=PCIE_DEV_LEN;
        }
        #else
        srcPhysAddr = PCIE0_MEM_BASE+(0x1000000*pcie_no);
        #endif
        #if 0
        printk("[H%d](%s) pcie%d, srcPhysAddr:0x%x, dstPhysAddr:0x%x, cmpPhysAddr:0x%x, flags:0x%x\n", 
                cpuid, __func__, pcie_no, srcPhysAddr, dstPhysAddr, cmpPhysAddr, flags);
        #endif
        set_npu_mbox_mib(10, srcPhysAddr);      /* src addr for npu */
        set_npu_mbox_mib(11, dstPhysAddr);      /* dst addr for npu */
        if (isCheckResult)
            set_npu_mbox_mib(12, cmpPhysAddr);  /* cmp addr for npu */
        set_npu_mbox_mib(13, testLen);          /* length to move for npu */
        set_npu_mbox_mib(8, 49);                /* start test_case 49: npu_do_dma_test */
    }

    return 0;
}

/*
 * CPU reads data from pcie_no dev, writes data to dstVirtAddr, and compare data with cmpVirtAddr.
 */
int cpu_read_pcie_dev_mem(int pcie_no, unsigned long dstVirtAddr, unsigned long cmpVirtAddr)
{
    int cpuid = smp_processor_id();
    unsigned long srcVirtAddr, tsize, bsize;
    volatile unsigned char *src_addr_charP, *dst_addr_charP;
    volatile unsigned short *src_addr_shortP, *dst_addr_shortP;
    volatile unsigned int *src_addr_intP, *dst_addr_intP, *cmp_addr_intP;
    volatile unsigned long *src_addr_longP, *dst_addr_longP;
    int s_char, s_short, s_int, s_long;
    int i, test_len;

    if ((pcie_no>=NUM_PCIE_DEV)||(is_pcie_link_up[pcie_no]==0))
        return 0;

    s_char = sizeof(unsigned char);
    s_short = sizeof(unsigned short);
    s_int = sizeof(unsigned int);
    s_long = sizeof(unsigned long);

    srcVirtAddr = pcie_dev_base[pcie_no];
    tsize = PCIE_DEV_LEN;
    cmp_addr_intP = (unsigned int*)cmpVirtAddr;

    src_addr_charP = (unsigned char*)srcVirtAddr;
    dst_addr_charP = (unsigned char*)dstVirtAddr;
    src_addr_shortP = (unsigned short*)srcVirtAddr;
    dst_addr_shortP = (unsigned short*)dstVirtAddr;
    src_addr_intP = (unsigned int*)srcVirtAddr;
    dst_addr_intP = (unsigned int*)dstVirtAddr;
    src_addr_longP = (unsigned long*)srcVirtAddr;
    dst_addr_longP = (unsigned long*)dstVirtAddr;

    #if 0
    printk("[H%d](%s) pcie%d, srcVirtAddr:0x%lx, dstVirtAddr:0x%lx, cmpVirtAddr:0x%lx\n", 
            cpuid, __func__, pcie_no, srcVirtAddr, dstVirtAddr, cmpVirtAddr);
    #endif
    
    for (bsize=0; bsize<4; bsize++) {

        if (bsize==0) {
            test_len = tsize/s_char;
            for (i=0; i<test_len; i++)
                dst_addr_charP[i]=src_addr_charP[i];
        }
        else if (bsize==1) {
            test_len = tsize/s_short;
            for (i=0; i<test_len; i++)
                dst_addr_shortP[i]=src_addr_shortP[i];
        }
        else if (bsize==2) {
            test_len = tsize/s_int;
            for (i=0; i<test_len; i++)
                dst_addr_intP[i]=src_addr_intP[i];
        }
        else /*(bsize==3)*/ {
            test_len = tsize/s_long;
            for (i=0; i<test_len; i++)
                dst_addr_longP[i]=src_addr_longP[i];
        }

        /* compare data */
        test_len = tsize/s_int;
        for (i=0; i<test_len; i++) {
            if (dst_addr_intP[i]!=cmp_addr_intP[i]) {
                printk("Error(%s): dst[%d]:0x%x != cmp[%d]:0x%x at bsize==%d\n", __func__, i, dst_addr_intP[i],  i, cmp_addr_intP[i], bsize);
                printk("\tNote: src_addr:0x%x, dst_addr:0x%x cmp_addr:0x%x, pcie_no:%d\n", srcVirtAddr, dstVirtAddr, cmpVirtAddr, pcie_no);
                return -1;
            }
        }
    }

    return 0;
}

/*
 * CPU triggers GDMA to reads data (with burst size) from pcie_no dev and to writes data to dstPhysAddr.
 * Then CPU compares data with cmpVirtAddr.
 */
int gdma_read_pcie_dev_mem(int pcie_no, unsigned long dstVirtAddr, unsigned long cmpVirtAddr, unsigned int dstPhysAddr, int burst)
{
    int cpuid = smp_processor_id();
    u32 *pcieMemBackP, *dstUncP;
    unsigned int tWords, j, channel=0, wait_cnt;
    volatile unsigned long tmp;
    int s;
    unsigned int offset;

    tWords=(PCIE_DEV_LEN/(sizeof(u32)));

    /* s==0,1,2 are for accessing pcie's first,middle,last PCIE_DEV_LEN bytes respectively. */
    for (s=0; s<NUM_PCIE_SEC; s++) {

        if (s==0) offset=0;
        else if (s==1) offset=(pcie_devmem_size[pcie_no]>>1);
        else offset=(pcie_devmem_size[pcie_no]-PCIE_DEV_LEN);

        /* clear dst region */
        dstUncP = (u32*)dstVirtAddr;
        for (j=0; j<tWords; j++) {
            dstUncP[j]=0; 
        }
        /* read another address to make sure previous writes have done */
        tmp = *(volatile unsigned long*)(dstVirtAddr+PCIE_DEV_LEN);
        dsb(st);

        SET_GDMA_CONFIG(channel, (u32)pcie_dev_addr[pcie_no]+offset, (u32)dstPhysAddr, ((PCIE_DEV_LEN&0xffff)<<16)|(burst<<3)|(1<<1)|(1<<0), 0x4);

        /* wait until GDMA is done */
        wait_cnt=10;
        while(!IS_GDMA_DONE(channel)) {
            if ((wait_cnt--)<=0) { 
                printk("[H%d] Error: waiting GDMA Done bit for too long!\n", cpuid);
                return -1;
            }
            mdelay(1);
        }
        CLEAR_GDMA_DONE(channel); /* clear done bit */

        /* read and compare data */
        pcieMemBackP = (u32*)(cmpVirtAddr+(PCIE_DEV_LEN*((s*NUM_PCIE_DEV)+pcie_no)));
        dstUncP = (u32*)dstVirtAddr;
        for (j=0; j<tWords; j++) {
            if (pcieMemBackP[j] != dstUncP[j]) {
                printk("[H%d] Compare Error: pcieMemBackP[%d]:0x%x != dstUncP[%d]:0x%x\n", 
                            cpuid, j, pcieMemBackP[j], j, dstUncP[j]);
                printk("\tNote: pcieMemBackP:0x%lx, dstUncP:0x%lx\n", pcieMemBackP, dstUncP);
                return -1;
            }
        }
    }

    return 0;
}

/*
 * - CPU0 triggers gdma to do burst_to_single_cmd test: 
 *   -- gdma can issue burst_cmd (burst_size>4) that cpu/npu can't.
 *   -- when pcie_single_cut enabled, make sure pcie_single_cut cuts gdma's burst cmds into correct number of single cmds
 *      by checking preBurstCnt and postBurstCnt.
 *   -- multi-issue will happen when burst_cmd is cut into multiple single_cmds by checking preMultiCnt and postMultiCnt.
 * - CPU0 triggers gdma to do burst_to_burst_cmd test: 
 *   -- when pcie_single_cut disabled, make sure burst_cmd remains burst_cmd by checking preBurstCnt and postBurstCnt.
 *   -- no multi-issue will happen by checking preMultiCnt and postMultiCnt.
 * - CPU0 triggers gdma, CPU1 triggers npu, CPU2/3 multi_to_multi_issue test:
 *   -- multiple Masters read data from pcie_dev at the same time, so multi_issue should happen by checking preMultiCnt and postMultiCnt.
 *   -- check if post_burst_cmd==0 when pcie_single_cut enabled.
 */
void r_single_pcie_test(int arg)
{
    int cpuid = smp_processor_id();
    static int test=0, sub=0, round=0, burst=0, i=0;
    static int res=0, num_sub_test=1, num_round=1, isMultiMasters=0;
    static unsigned long pcieMemBackAddr=NULL, dstUncAddr=NULL;
    static dma_addr_t pcieBackDmaAddr, dstDmaAddr;
    static struct device *dev;
    static unsigned int gdmaBurstBytes[NUM_GDMA_BURST]={4,8,16,32,64};
    static unsigned int burstCmds[NUM_GDMA_BURST];
    static unsigned int singleCutNum[NUM_GDMA_BURST]={0,1,3,7,15};
    static unsigned int multiIssues[NUM_GDMA_BURST];
    volatile static int syncFlag=0, startFlag=0, endFlag=0;
    volatile static int pcie_no=0;


    #if defined(TCSUPPORT_CPU_AN7552)
    /* 7552 pcie uses pbus, instead of AXI bus, to connect to system. Pbus does,'t support multi-issue and
     * burst cmds, so no need to verify pcie single cut. */
    if (cpuid==0) {
        printk("Just Return! Because 7552 pcie uses pbus, which doesn't support multi-issue and burst cmds.");
        printk("So no need to verify pcie single cut\n");
    }
    return;
    #endif

    if (cpuid > 0) { /* cpu1~3 */
        
        /* cpu1~3 wait for CPU0 finishing burst_to_single_cmd and burst_to_burst_cmd tests */
        while (syncFlag==0) {}
        if (syncFlag==-1) return;

        /* cpu1~3 only join test_multi_to_multi_issue */
        while (syncFlag==1) {

            while (startFlag==0) {} /* wait for cpu0 to start the multi_to_multi_issue test */

            if (cpuid==1) { /* cpu1 triggers npu to read pcie and compare */
                if (npu_read_pcie_dev_mem(pcie_no, (u32)(dstDmaAddr+PCIE_DEV_LEN), (u32)(pcieBackDmaAddr+(PCIE_DEV_LEN*pcie_no)), 0x1)==-1)
                    goto host_rw_pcie_fail;
            }
            else { /* cpu2,3 read pcie and compare */
                if (cpu_read_pcie_dev_mem(pcie_no, (dstUncAddr+(PCIE_DEV_LEN*cpuid)), (pcieMemBackAddr+(PCIE_DEV_LEN*pcie_no)))==-1)
                    goto host_rw_pcie_fail;
            }

            if (endFlag==1)
                goto host_rw_pcie_fail;
        }

host_rw_pcie_fail:
        /* other CPUs return here */
        endFlag=1;
        mdelay(100);
        return;
    }

    /***********************************************************
     **** C P U 0   O N L Y ************************************
     ***********************************************************/

    pcieMemBackAddr = pcie_mem_backup(&pcieBackDmaAddr);
    if ((pcieMemBackAddr==NULL)) {
        printk("[H%d](%s) pcie_mem_backup failed\n", cpuid, __func__);
        syncFlag=-1;
        mdelay(10);
        syncFlag=0;
        return;
    }

    dev=get_gdmpSram_dev();
    dstUncAddr = (unsigned long) dma_alloc_coherent(dev, PCIE_DEV_LEN*NUM_MASTER, &dstDmaAddr, GFP_KERNEL);
    if ((dstUncAddr==NULL)) {
        printk("[H%d](%s) dma_alloc_coherent failed\n", cpuid, __func__);
        pcie_mem_free(pcieMemBackAddr, pcieBackDmaAddr);
        syncFlag=-1;
        mdelay(10);
        syncFlag=0;
        return;
    }

    burstCmds[0]=0;
    multiIssues[0]=0;
    for (i=1; i<NUM_GDMA_BURST; i++) {
        /* translate PCIE_DEV_LEN bytes to how_many gdma burst cmds */
        burstCmds[i] = (PCIE_DEV_LEN/gdmaBurstBytes[i]);
        /* translate gdma burst cmds to how_many multi-issue counts */
        multiIssues[i] = burstCmds[i]*singleCutNum[i];
        /* in gdma_read_pcie_dev_mem(), gdma will read pcie for 3 times, so exected multiIssues & burstCmds should multiply 3 */
        burstCmds[i] = burstCmds[i]*3;
        multiIssues[i] = multiIssues[i]*3;
    }
    
    #if 1
    for (i=0; i<NUM_PCIE_DEV; i++) {
        printk("[H%d] pcie_phys_addr[%d]:0x%lx, pcie_virt_addr[%d]:0x%lx\n", cpuid, i, pcie_dev_addr[i], i, pcie_dev_base[i]);
    }
    printk("[H%d] dstDmaAddr:0x%lx, dstUncAddr:0x%lx\n", cpuid, dstDmaAddr, dstUncAddr);
    printk("[H%d] pcieBackDmaAddr:0x%lx, pcieMemBackAddr:0x%lx\n", cpuid, pcieBackDmaAddr, pcieMemBackAddr);
    #endif

    /* test==0: cpu0 triggers gdma to do burst_cmd to single_cmd test,
     * test==1: cpu0 triggers gdma to do burst_cmd to burst_cmd test,
     * test==2: cpu0 triggers gdma, cpu1 triggers npu, cpu2/3 do muitl_issue to muitl_issue test. */
    for (test=0; test<test_single_pcie_last; test++) {

        if (test==test_multi_to_multi_issue) {
            syncFlag=1; /* notify other CPUs to be ready for tests */
            num_sub_test=2;  /* one for single cut enabled, another for single cut disabled */
            num_round=100; /* more rounds, so that all CPUs can work at the same time */
            isMultiMasters=1;
        }

        for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {

            #ifdef TCSUPPORT_CPU_AN7583
            if (pcie_no!=0) continue; /* 7583 only pcie0 supports r_single_cut */
            #endif

            if (is_pcie_link_up[pcie_no]==0) {
                printk("[H%d] skip pcie%d because it's not link up\n", cpuid, pcie_no);
                continue;
            }

            /* in multi_to_multi_issue test, sub==0 for single cut enable, sub==1 for single cut disable */
            for (sub=0; sub<num_sub_test; sub++) {
            
                printk("[H%d] do pcie%d %s:%d_%d\n", cpuid, pcie_no, __func__, test, sub);

                #ifdef NEW_PBUS_ARCH
                if ((test==test_burst_to_single_cmd) || ((test==test_multi_to_multi_issue)&&(sub==0)))
                    pcie_single_enable(pcie_no, 1);
                else
                    pcie_single_enable(pcie_no, 0);
                #endif

                if (test==test_multi_to_multi_issue) {
                    #ifdef NEW_PBUS_ARCH
                    pbus_dbg_cnt_clear(ALL_DBG_CNT_BITS);
                    #endif
                    startFlag=1; /* release cpu1~3 for tests */
                }

                /* more rounds for test_multi_to_multi_issue, so that all CPUs can work at the same time */
                for (round=0; round<num_round; round++) {

                    /* for gdma, burst:0/1/2/3/4 means 1/2/4/8/16 DW which is 4/8/16/32/64 bytes */
                    for(burst=0; burst<NUM_GDMA_BURST; burst++) {

                        /* clear dbg counters */
                        if ((test==test_burst_to_single_cmd)||(test==test_burst_to_burst_cmd)) {
                            #ifdef NEW_PBUS_ARCH
                            pbus_dbg_cnt_clear(ALL_DBG_CNT_BITS);
                            #endif
                        }
                        
                        if (gdma_read_pcie_dev_mem(pcie_no, dstUncAddr, pcieMemBackAddr, (u32)dstDmaAddr, burst)==-1) {
                            res=-1;
                            goto single_pcie_test_exit;
                        }
                        
                        /* check dbg counters */
                        if ((test==test_burst_to_single_cmd)||(test==test_burst_to_burst_cmd)) {
                            if (burst==0) { 
                                /* for burst_size == 4 bytes (which belongs to single_cmd), we expect:
                                 * pre_burstCmd_cnt==0, post_burstCmd_cnt==0, pre_multiIssue_cnt==0, post_multiIssue_cnt==0 */
                                if (res=single_pcie_dcnt_check(pcie_no,isMultiMasters,0,0,0,0)) 
                                    goto single_pcie_test_exit;
                            }
                            else { /* burst_size > 4 bytes (which belongs to burst_cmd) */
                                if (test==test_burst_to_single_cmd) {
                                    /* with single cut enabled, a burst_cmd should be cut as multiple single_cmds */
                                    if (res=single_pcie_dcnt_check(pcie_no, isMultiMasters, burstCmds[burst], 0, 0, multiIssues[burst])) 
                                        goto single_pcie_test_exit;
                                }
                                else /*test_burst_to_burst_cmd*/ {
                                    /* with single cut disabled, burst_cmd remains burst_cmd, so no multi-issue happens */
                                    if (res=single_pcie_dcnt_check(pcie_no, isMultiMasters, burstCmds[burst], burstCmds[burst], 0, 0)) 
                                        goto single_pcie_test_exit;
                                }
                            }
                        }
                        
                        if (endFlag==1) { /* end signal from other CPUs */
                            res=-1;
                            goto single_pcie_test_exit;
                        }
                        
                    } /* burst */
                } /* round */

                /* roughly check dbg counters for multi_to_multi_issue test */
                if (test==test_multi_to_multi_issue) {
                    startFlag=0; /* hold other CPUs for next test */
                    mdelay(100);
                    if (sub==0) {
                        /* with single cut enabled, a burst_cmd should become multiple single_cmds */
                        if (res=single_pcie_dcnt_check(pcie_no, isMultiMasters, 1, 0, 1, 1)) 
                            goto single_pcie_test_exit;
                    }
                    else /*sub==1*/ {
                        /* with single cut disabled, a burst_cmd remains burst_cmds */
                        if (res=single_pcie_dcnt_check(pcie_no, isMultiMasters, 1, 1, 1, 1)) 
                            goto single_pcie_test_exit;
                    }
                }
            
            } /* sub */
        } /* pcie */
    } /* test */

    
single_pcie_test_exit:
    pcie_mem_free(pcieMemBackAddr, pcieBackDmaAddr);
    dma_free_coherent(dev, PCIE_DEV_LEN*NUM_PCIE_DEV, dstUncAddr, dstDmaAddr);
    syncFlag=-1;
    startFlag=-1;
    mdelay(10);
    syncFlag=0;
    startFlag=0;
    if (res==0)
        printk("[H%d] do %s -> Done\n", cpuid, __func__);
    else
        printk("\n[H%d] Error(%s) at pcie%d, test%d_%d, round:%d, burst:%d, endFlag:%d\n", 
                cpuid, __func__, pcie_no, test, sub, round, burst, endFlag);
    return;
}

/* 
 * stage0: (while npu is in idle) 
 *   - cpu reads registers on original pbus and measures cpu_cycle, pcie_bypass, pcie_block counters.
 *     - pcie_bypass and pcie_block counters should be 0, because only one Master is accessing bus.
 * stage1: (while npu is reading pcie devmem)
 *   - cpu reads registers on original pbus and measures cpu_cycle, pcie_bypass, pcie_block counters.
 *     - pcie_bypass should count, which means original-pbus-read can bypass pcie-devmem-read.
 *     - pcie_block counter should be 0, which means original-pbus-read didn't be blocked by pcie-devmem-read.
 *     - cpu_cycle measured in stage0 and stage1 should be the same.
 * [7523] does the same test as 7581 does in stage1
 *   - 7581's cpu_cycle should be better (smaller) than 7523's.
 */
void cycle_bypass_block_cnt_test(int arg)
{
    int cpuid = smp_processor_id();
    static int stage, pcie_no, reg, round, read;
    #ifdef NEW_PBUS_ARCH
    #ifdef TCSUPPORT_CPU_AN7583
    static unsigned int measured_regs[NUM_MEASURED_REG]= {
            TEST_REG_SCU_SCREG_WF0, TEST_REG_GDMA, PCIE0_MAC_BASE+PCIE0_OFF0, PCIE1_MAC_BASE+PCIE1_OFF0};
    #else
    static unsigned int measured_regs[NUM_MEASURED_REG]= {
            TEST_REG_SCU_SCREG_WF0, TEST_REG_GDMA, PCIE0_MAC_BASE+PCIE_OFF0, PCIE1_MAC_BASE+PCIE_OFF0};
    #endif
    static char *reg_names[NUM_MEASURED_REG]= {"[SCU]      ","[GDMA]     ","[PCIE0_MAC]","[PCIE1_MAC]"};
    #else /* 7523, for comparing with 7581 */
    static unsigned int measured_regs[NUM_MEASURED_REG]= {TEST_REG_SCU_SCREG_WF0, TEST_REG_GDMA};
    static char *reg_names[NUM_MEASURED_REG]= {"[SCU]","[GDMA]"};
    #endif
    static void *measured_virtAddr[NUM_MEASURED_REG];
    static int pcie_bypass_dbg_no[]={dcnt_rbus_pcie0_bypass, dcnt_rbus_pcie1_bypass, dcnt_rbus_pcie2_bypass};
    static int pcie_block_dbg_no[]={dcnt_rbus_pcie0_block, dcnt_rbus_pcie1_block, dcnt_rbus_pcie2_block};
    static unsigned long cycle_cnt0[NUM_PCIE_DEV][NUM_MEASURED_REG], cycle_cnt1[NUM_PCIE_DEV][NUM_MEASURED_REG];
    static unsigned int bypass_cnt0[NUM_PCIE_DEV][NUM_MEASURED_REG], bypass_cnt1[NUM_PCIE_DEV][NUM_MEASURED_REG];
    static unsigned int block_cnt0[NUM_PCIE_DEV][NUM_MEASURED_REG], block_cnt1[NUM_PCIE_DEV][NUM_MEASURED_REG];
    static struct device *dev=NULL;
    static unsigned long dstUncAddr;
    static dma_addr_t dstDmaAddr;

            
    printk("[H%d] do %s (read_cnt:%d)\n", cpuid, __func__, arg);

    dev=get_gdmpSram_dev();
    if (dev==NULL) {
        printk("[H%d](%s) get_gdmpSram_dev failed\n", cpuid, __func__);
        return;
    }
    dstUncAddr = (unsigned long) dma_alloc_coherent(dev, PCIE_DEV_LEN, &dstDmaAddr, GFP_KERNEL);
    if ((dstUncAddr==NULL)) {
        printk("[H%d](%s) dma_alloc_coherent failed\n", cpuid, __func__);
        return;
    }

    host_check_pcie_link_up();
    
    arm_pmu_init();
    for (reg=0; reg<NUM_MEASURED_REG; reg++)
        measured_virtAddr[reg] = ioremap(measured_regs[reg], 4);
    local_irq_disable();

    /* stage==0: npu is in idle while cpu does latency, block_cnt, bypass_cnt measurement 
     * stage==1: npu keeps reading pcie devmem while cpu does latency, block_cnt, bypass_cnt measurement */
    for (stage=0; stage<2; stage++) {
        for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {

            if (is_pcie_link_up[pcie_no]==0) {
                printk("[H%d] skip pcie%d because it's not link up\n", cpuid, pcie_no);
                continue;
            }
            /* just do once, because when npu is not reading pcie dev, the result should be the same for all pcie-no */
            if ((stage==0)&&(pcie_no>0))
                continue;

            /* Host does latency, block_cnt, bypass_cnt measurement */
            for (reg=0; reg<NUM_MEASURED_REG; reg++) {
                /* 1st round won't be recorded. just for putting code to icache */
                for (round=0; round<2; round++) {

                    #ifdef NEW_PBUS_ARCH
                    pbus_dbg_cnt_clear(ALL_DBG_CNT_BITS);

                    bypass_cnt0[pcie_no][reg] = pbus_dbg_cnt_get(pcie_bypass_dbg_no[pcie_no]);
                    block_cnt0[pcie_no][reg] = pbus_dbg_cnt_get(pcie_block_dbg_no[pcie_no]);
                    #endif
                    if ((stage==1)&&(round==1)) {
                        npu_read_pcie_dev_mem(pcie_no, (u32)dstDmaAddr, 0, 0x2); /* npu continuously reads pcie devmem */
                        mdelay(100); /* make sure npu is reading before cpu does measurement */
                    }
                    
                    cycle_cnt0[pcie_no][reg] = read_c0_count();

                    for (read=0; read<arg; read++) {
                        rdMemW(measured_virtAddr[reg]);
                    }

                    cycle_cnt1[pcie_no][reg] = read_c0_count();
                    
                    /* block/bypass cnt belong to normal pbus, so read them after stopping npu reading pcie devmem,
                    * otherwise, they may count when they are accessed. */
                    if ((stage==1)&&(round==1)) {
                        npu_read_pcie_dev_mem(pcie_no, (u32)dstDmaAddr, 0, 0x0); /* stop npu */
                        mdelay(200);
                    }
                    #ifdef NEW_PBUS_ARCH;
                    block_cnt1[pcie_no][reg] = pbus_dbg_cnt_get(pcie_block_dbg_no[pcie_no]);
                    bypass_cnt1[pcie_no][reg] = pbus_dbg_cnt_get(pcie_bypass_dbg_no[pcie_no]);
                    #endif
                    mdelay(10);

                } /* round */
            }   /* reg */

            /* show results */
            if (stage==0)
                printk("[stage%d] npu is in idle\n", stage);
            else
                printk("[stage%d-%d] npu is reading pcie%d\n", stage, pcie_no, pcie_no);

            for (reg=0; reg<NUM_MEASURED_REG; reg++) {
                #ifdef NEW_PBUS_ARCH
                printk("\t%s cycle_cnt:%d, bypass_cnt:%d, block_cnt:%d\n", reg_names[reg], 
                                    (int)(cycle_cnt1[pcie_no][reg]-cycle_cnt0[pcie_no][reg]),
                                    (int)(bypass_cnt1[pcie_no][reg]-bypass_cnt0[pcie_no][reg]),
                                    (int)(block_cnt1[pcie_no][reg]-block_cnt0[pcie_no][reg]));
                #else
                printk("\t%s cycle_cnt:%d\n", reg_names[reg], (int)(cycle_cnt1[pcie_no][reg]-cycle_cnt0[pcie_no][reg]));
                #endif
            }
        } /* pcie_no */
    }   /* stage */

    local_irq_enable();
    for (reg=0; reg<NUM_MEASURED_REG; reg++)
        iounmap(measured_virtAddr[reg]);
    dma_free_coherent(dev, PCIE_DEV_LEN, dstUncAddr, dstDmaAddr);
    
    return;
}

/* 
 * CPU measure latency for gdma reading pcie_devmem, for pcie_single_cut enabled and disabled, for all gdma burst sizes.
 *   - the latency for pcie_single_cut disabled should be better than pcie_single_cut enabled
 * [7523] does the same measurement (Note: 7523 has no pcie_single_cut)
 *   - 7581's latency for pcie_single_cut disabled should be better than 7523's.
 * Note: both 7523 and 7552's pcie connect to system by pbus which doesn't support single cut.
 */
void pcie_dev_mem_burst_perf_test(int arg)
{
    int cpuid = smp_processor_id();
    static int test, pcie_no, burst, round, channel=0;
    static unsigned long cycle_cnt0[NUM_TESTS], cycle_cnt1[NUM_TESTS];
    static struct device *dev=NULL;
    static unsigned long dstUncAddr;
    static dma_addr_t dstDmaAddr;
    static int burst_bytes[NUM_GDMA_BURST]={4,8,16,32,64};
    #if defined(TCSUPPORT_CPU_EN7581)
    static int num_test=2;
    #else /* 7523, 7522 */
    static int num_test=1;
    #endif

    printk("[H%d] do %s (arg:%d)\n", cpuid, __func__, arg);

    dev=get_gdmpSram_dev();
    if (dev==NULL) {
        printk("[H%d](%s) get_gdmpSram_dev failed\n", cpuid, __func__);
        return;
    }
    dstUncAddr = (unsigned long) dma_alloc_coherent(dev, PCIE_DEV_LEN, &dstDmaAddr, GFP_KERNEL);
    if ((dstUncAddr==NULL)) {
        printk("[H%d](%s) dma_alloc_coherent failed\n", cpuid, __func__);
        return;
    }

    if (pcie_dev_mem_ioremap()) {
        printk("[H%d](%s) pcie devmem ioremap failed\n", cpuid, __func__);
        dma_free_coherent(dev, PCIE_DEV_LEN, dstUncAddr, dstDmaAddr);
        return;
    }
    
    arm_pmu_init();
    local_irq_disable();


    for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {

        #ifdef TCSUPPORT_CPU_AN7583
        if (pcie_no!=0) continue; /* 7583 only pcie0 supports r_single_cut */
        #endif

        if (is_pcie_link_up[pcie_no]==0) {
            printk("[H%d] skip pcie%d because it's not link up\n", cpuid, pcie_no);
            continue;
        }
        
        /* one for pcie_single_cut enabled, another for pcie_single_cut disabled */
        for (test=0; test<num_test; test++) {

            #if defined(TCSUPPORT_CPU_EN7581)
            if (test==0) {
                pcie_single_enable(pcie_no, 1);
                printk("\npcie%d single_cut enabled\n", pcie_no);
            }
            else {
                pcie_single_enable(pcie_no, 0);
                printk("\npcie%d single_cut disabled\n", pcie_no);
            }
            #endif
            
            for(burst=0; burst<NUM_GDMA_BURST; burst++) { /* burst:0/1/2/3/4 means 1/2/4/8/16 DW which is 4/8/16/32/64 bytes */
                
                for (round=0; round<NUM_TESTS; round++) {

                    SET_GDMA_CONFIG(channel, (u32)pcie_dev_addr[pcie_no], (u32)dstDmaAddr, ((PCIE_DEV_LEN&0xffff)<<16)|(burst<<3)|(1<<1)|(1<<0), 0x4);

                    cycle_cnt0[round] = read_c0_count();

                    /* wait until GDMA is done */
                    while(!IS_GDMA_DONE(channel)) {}

                    cycle_cnt1[round] = read_c0_count();

                    /* clear GDMA done bit */
                    CLEAR_GDMA_DONE(channel);
                
                } /* round */

                printk("\tcpu_cycles (GDMA_burst==%02d): %d\n", burst_bytes[burst], cpu_cycles_calculate(cycle_cnt0,cycle_cnt1,NUM_GDMA_BURST));

            } /* burst */
        } /* test */
    } /* pcie */

    local_irq_enable();
    pcie_dev_mem_iounmap();
    dma_free_coherent(dev, PCIE_DEV_LEN, dstUncAddr, dstDmaAddr);
    
    return;
}

/*
 * CPU resets modules while npu is accessing it, to see if pbus_timeout will happen and if timeout_info is correct.
 */
void pbus_timeout_test(int arg)
{
    int cpuid = smp_processor_id();
    #if defined(TCSUPPORT_CPU_AN7583)
    unsigned int rst_bit[] = {22, 26, 26, 29, 29};
    #elif defined(TCSUPPORT_CPU_EN7581)
    unsigned int rst_bit[] = {22, 26, 26, 27, 27, 27, 27};
    #else /*7552,7523*/
    unsigned int rst_bit[] = {22, 29, 29, 29, 29};
    #endif
    char *module_name[] = {"hsdma", "pcie0_mac", "pcie0_dev", "pcie1_mac", "pcie1_dev", "pcie2_mac", "pcie2_dev"};
    set_func_t set_func[]= {SET_SCU_RST_RG, SET_SCU_RSTCTRL1, SET_SCU_RSTCTRL1, SET_SCU_RSTCTRL1, SET_SCU_RSTCTRL1, SET_SCU_RST_RG, SET_SCU_RST_RG};
    get_func_t get_func[]= {GET_SCU_RST_RG, GET_SCU_RSTCTRL1, GET_SCU_RSTCTRL1, GET_SCU_RSTCTRL1, GET_SCU_RSTCTRL1, GET_SCU_RST_RG, GET_SCU_RST_RG};
    unsigned int test_reg, reg, val, errAddr;
    int i, pcie_no;

    host_check_pcie_link_up();

    /* asign pcie devmem base addr */
    for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {
        if (is_pcie_link_up[pcie_no]) {
            tout_test_regs[1+(pcie_no<<1)+1] = pcie_mem_base_get(pcie_no);
        }
        else {
            tout_test_regs[1+(pcie_no<<1)]=0;
            tout_test_regs[1+(pcie_no<<1)+1]=0;
        }
    }

    for (i=0; i<NUM_TEST_REGS; i++) {

        if (tout_test_regs[i])
            printk("\n[H%d] do %s for %s\n", cpuid, __func__, module_name[i]);
        else {
            printk("\n[H%d] skip %s for %s\n", cpuid, __func__, module_name[i]);
            continue;
        }

        set_npu_mbox_mib(14, 0); /* clear before test */

        if (npu_read_pcie_dev_mem(i, 0, 0, 0x3)==-1) /* trigger npu to read tout_test_regs for pbus timeout test */ 
            return;
        /* reset module by scu_reset reg */
        set_func[i](get_func[i]()|(1<<rst_bit[i]));
        
        while (get_npu_mbox_mib(14)==0) {} /* wait until NPU reads tout_test_regs */
        
        /* release module by scu_reset reg */
        set_func[i](get_func[i]()&(~(1<<rst_bit[i])));
        mdelay(1000);

        /* check pbus timeout error address */
        errAddr = (get_pbus_err_addr()&0x3fffffff);
        if (errAddr!=tout_test_regs[i]) {
            printk("Error: pbus timeout ERR_ADDR:0x%x != 0x%x for %s\n", errAddr, tout_test_regs[i], module_name[i]);
            return;
        }
    }
    
    return;
}

/* 7552 pcie uses pbus to connect with system and pcie mac registers and devmem belong to the same path.
 * this test aims to make sure that reading pcie mac registers can survive (bus timeout won't happen) 
 * when another Master (NPU) keeps reading pcie devmem. */
void pcie_mac_reg_read_satrving_test(int arg)
{
    int cpuid = smp_processor_id();
    int pcie_no, round, idx;
    struct device *dev=NULL;
    unsigned long dstUncAddr;
    dma_addr_t dstDmaAddr;
    unsigned int regVal, errAddr;
    unsigned long pcie_mac_addr[NUM_PCIE_DEV][NUM_PCIE_MAC_REG];
    #ifdef TCSUPPORT_CPU_AN7583
    unsigned int pcie_mac_regs[][NUM_PCIE_MAC_REG]= {
                {PCIE0_MAC_BASE+PCIE0_OFF0, PCIE0_MAC_BASE+PCIE0_OFF1, PCIE0_MAC_BASE+PCIE0_OFF2, PCIE0_MAC_BASE+PCIE0_OFF3},
                {PCIE1_MAC_BASE+PCIE1_OFF0, PCIE1_MAC_BASE+PCIE1_OFF1, PCIE1_MAC_BASE+PCIE1_OFF2, PCIE1_MAC_BASE+PCIE1_OFF3}};
    #else
    unsigned int pcie_mac_regs[][NUM_PCIE_MAC_REG]= {
                {PCIE0_MAC_BASE+PCIE_OFF0, PCIE0_MAC_BASE+PCIE_OFF1, PCIE0_MAC_BASE+PCIE_OFF2, PCIE0_MAC_BASE+PCIE_OFF3},
                {PCIE1_MAC_BASE+PCIE_OFF0, PCIE1_MAC_BASE+PCIE_OFF1, PCIE1_MAC_BASE+PCIE_OFF2, PCIE1_MAC_BASE+PCIE_OFF3}};
    #endif
            
    printk("[H%d] do %s for %d rounds\n", cpuid, __func__, arg);

    if (isPbusTimeoutEn()==0) {
        printk("Error: pbus timeout isn't enabled, just return\n");
        return;
    }

    dev=get_gdmpSram_dev();
    if (dev==NULL) {
        printk("[H%d](%s) get_gdmpSram_dev failed\n", cpuid, __func__);
        return;
    }
    dstUncAddr = (unsigned long) dma_alloc_coherent(dev, PCIE_DEV_LEN, &dstDmaAddr, GFP_KERNEL);
    if ((dstUncAddr==NULL)) {
        printk("[H%d](%s) dma_alloc_coherent failed\n", cpuid, __func__);
        return;
    }

    for (pcie_no=0; pcie_no<NUM_PCIE_DEV; pcie_no++) {

        /* ioremap pcie mac regs and assign value before test */
        for (idx=0; idx<NUM_PCIE_MAC_REG; idx++) {
            pcie_mac_addr[pcie_no][idx] = ioremap(pcie_mac_regs[pcie_no][idx], 4);
            wtMemW(pcie_mac_addr[pcie_no][idx], testIntPat[idx]);
        }

        npu_read_pcie_dev_mem(pcie_no, (u32)dstDmaAddr, 0, 0x2); /* npu continuously reads pcie devmem */
        mdelay(100);

        for (round=0; round<arg; round++) {
            for (idx=0; idx<NUM_PCIE_MAC_REG; idx++) {
                regVal = rdMemW(pcie_mac_addr[pcie_no][idx]);
                /* check if value is correct */
                if (regVal!=testIntPat[idx]) {
                    printk("\nError: regVal:0x%x != 0x%x (Note: pcie_mac_addr[%d][%d]:0x%x at round%d)\n",
                        regVal, testIntPat[idx], pcie_no, idx, pcie_mac_addr[pcie_no][idx], round);
                }
                /* check if pbus timeout happened */
                errAddr = (get_pbus_err_addr()&0x3fffffff);
                if (errAddr==pcie_mac_regs[pcie_no][idx]) {
                    printk("\nError: pbus timeout ERR_ADDR:0x%x for pcie:%d,round:%d,idx:%d\n", errAddr, pcie_no, round, idx);
                    return;
                }
            }
        }
        
        npu_read_pcie_dev_mem(pcie_no, (u32)dstDmaAddr, 0, 0x0); /* stop npu */
        mdelay(200);

        /* iounmap after test */
        for (idx=0; idx<NUM_PCIE_MAC_REG; idx++)
            iounmap(pcie_mac_addr[pcie_no][idx]);
    }

    dma_free_coherent(dev, PCIE_DEV_LEN, dstUncAddr, dstDmaAddr);
    printk("[H%d] %s done\n", cpuid, __func__);
    return;
}

int doNewPBusTest(void *arg) 
{   
    #ifdef NEW_PBUS_ARCH
    int cpuid = smp_processor_id();
    if (cpuid==0)
        pbus_dbg_cnt_clear(ALL_DBG_CNT_BITS);
    #endif
    /* case3~5 is done by one cpu in cpu_bus_test.c */
    new_pbus_tests[data0](data1);
    return 0;
}

/* testCase==0: cpu writes dram (needs wmb() after writes)
 * testCase==1: cpu writes npu_sram (needs uncached read after writes) */
void cpuWrites_beforeDMA(
        unsigned long src_addr, unsigned long dst_addr, unsigned int len, 
        int isCacAddr, int wtBytes, int testCase)
{
    volatile unsigned char *src_addr_charP, *dst_addr_charP;
    volatile unsigned int *src_addr_intP;
    volatile unsigned long *src_addr_longP;
    int i, test_len;
    int s_char = sizeof(unsigned char);
    int s_int = sizeof(unsigned int);
    int s_long = sizeof(unsigned long);
    volatile unsigned long tmp;


    dst_addr_charP = (unsigned char*)dst_addr;
    test_len = len/s_char;

    /* clear dst area */
    for (i=0; i<test_len; i++)
        dst_addr_charP[i]=0;

    if (isCacAddr)
        ecnt_dcache_wback_inv(dst_addr, len);

    /* (Byte/Word/DWord) writes to src area */
    
    if (wtBytes==s_char) {

        src_addr_charP = (unsigned char*)src_addr;

        for (i=0; i<test_len; i++)
            src_addr_charP[i]=testBytePat[i&0x3];

        if (testCase==1) /* cpu->npu_sram needs uncached read after writes */
            tmp = src_addr_charP[test_len-1];
    }
    else if (wtBytes==s_int) {
        
        src_addr_intP = (unsigned int*)src_addr;
        test_len = len/s_int;

        for (i=0; i<test_len; i++)
            src_addr_intP[i]=testIntPat[i&0x3];

        if (testCase==1) /* cpu->npu_sram needs uncached read after writes */
            tmp = src_addr_intP[test_len-1];
    }
    else if (wtBytes==s_long) {

        src_addr_longP = (unsigned long*)src_addr;
        test_len = len/s_long;

        for (i=0; i<test_len; i++)
            src_addr_longP[i]=testWordPat[i&0x3];

        if (testCase==1) /* cpu->npu_sram needs uncached read after writes */
            tmp = src_addr_longP[test_len-1];
    }
    else {
        printk("ERROR: Wrong wtBytes: %d\n", wtBytes);
    }

    if (isCacAddr)
        ecnt_dcache_wback_inv(src_addr, len);

    if (testCase==0) /* cpu->dram needs wmb() after writes */
        wmb();
    
    return;
}

int cpuCompare_afterDMA(unsigned long src_addr, unsigned long dst_addr, unsigned int len, int printFlag)
{
    volatile unsigned long *src_addr_longP, *dst_addr_longP;
    int i, test_len; 
    int s_long = sizeof(unsigned long);


    src_addr_longP = (unsigned long*)src_addr;
    dst_addr_longP = (unsigned long*)dst_addr;
    test_len=len/s_long;

    for (i=0; i<test_len; i++) {
        if (src_addr_longP[i]!=dst_addr_longP[i]) {
            if (printFlag!=1) /* printFlag==1: bufferable+dma_blcok_off. Error is expected, so no need to print */
                printk("ERROR: src_addr_longP[%d]:0x%lx != dst_addr_longP[%d]:0x%lx\n", 
                        i, src_addr_longP[i], i, dst_addr_longP[i]);
            return -1;
        }
    }

    return 0;
}

static void free_mem_after_blkCnt_test(void)
{
    int i;

    for (i=0; i<tmp_addr_cnt; i++)
        kfree(tmp_addr_baseP[i]);
    
    kfree(tmp_addr_baseP);

    return;
}

/* Note: dma_block_cnt test needs rbus_pending. when rbus_pending is enabled, all address-matched  
 *       CPU read/write cmds to DRAM will be blocked in cmd_fifo, including code fetching. 
 *       In order to prevent code fetching from being blocked, the block-matched address needs to
 *       differs from code's. That is what this function is for. It recursively allocates memory 
 *       from kernel until it gets the memory (for test) whose highest 8-bit address differs from 
 *       code's highest 8-bit address. 
 *       For example, if 1st allocated memory's phys_addr is 0xab800000, code's memory should be 
 *       above 0xab800000. Then this function needs to allocate memory recursively until it gets 
 *       the memory (for test) whose address is "0xaa------" */
static int alloc_mem_before_blkCnt_test(unsigned int alloc_size)
{
    unsigned long tmp_cac_addr;
    unsigned int tmp_phy_addr1=0, tmp_phy_addr2=0;
    int i;
    unsigned int max_addr_cnt = S_32K/sizeof(unsigned long);

    /* used to store the allocated memory's base address in the following */
    tmp_addr_baseP = (unsigned long*) kzalloc(S_32K, GFP_KERNEL);
    if ((tmp_addr_baseP == NULL)) {
        printk("ERROR: kzalloc for tmp_addr_baseP failed\n");
        return -1;
    }
    tmp_addr_cnt=0;

    /* allocate recursively in order to find memory whose address range differs from kernel/driver code's */
    while(1) {
        
        tmp_cac_addr = (unsigned long) kzalloc(alloc_size, GFP_KERNEL);
        if ((tmp_cac_addr == NULL) || (tmp_addr_cnt>=max_addr_cnt)) {
            printk("ERROR: recursively kzalloc failed\n");
            free_mem_after_blkCnt_test();
            return -1;
        }
        
        tmp_addr_baseP[tmp_addr_cnt] = tmp_cac_addr;
        tmp_addr_cnt++;

        if (tmp_phy_addr1) { 
            tmp_phy_addr2 = virt_to_phys(tmp_cac_addr);
            tmp_phy_addr2 &= (0xff<<24);
            /* until finding the memory whose address' hightest 8 bits differs from kernel/driver code's */
            if (tmp_phy_addr2!=tmp_phy_addr1) 
                break;
        }
        else{
            tmp_phy_addr1 = virt_to_phys(tmp_cac_addr);
            printk("1st memory physAddr:0x%x\n", tmp_phy_addr1);
            tmp_phy_addr1 &= (0xff<<24);
        }
    }

    return 0;
}

static int alloc_dram_before_blkCnt_test(
        unsigned int alloc_size, unsigned long *unc_addrP, 
        unsigned long *cac_addrP, dma_addr_t *dma_addrP)
{
    struct device *dev=NULL;

    
    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("ERROR: get_gdmpSram_dev is NULL\n");
        return -1;
    }

    if (alloc_mem_before_blkCnt_test(alloc_size)) {
        printk("ERROR: alloc_mem_before_blkCnt_test failed\n");
        return -1;
    }

    *unc_addrP = (unsigned long) dma_alloc_coherent(dev, alloc_size, dma_addrP, GFP_KERNEL);
    if ((*unc_addrP==NULL)) {
        printk("ERROR: dma_alloc_coherent failed\n");
        free_mem_after_blkCnt_test();
        return -1;
    }

    *cac_addrP = (unsigned long) kzalloc(alloc_size, GFP_KERNEL);
    if ((*cac_addrP == NULL)) {
        printk("ERROR: kzalloc failed\n");
        free_mem_after_blkCnt_test();
        dma_free_coherent(dev, alloc_size, *unc_addrP, *dma_addrP);
        return -1;
    }

    return 0;
}

static void free_dram_after_blkCnt_test(
        unsigned int alloc_size, unsigned long *unc_addrP, 
        unsigned long *cac_addrP, dma_addr_t *dma_addrP)
{
    int i;
    struct device *dev=NULL;
       
    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("ERROR: get_gdmpSram_dev is NULL\n");
    }

    free_mem_after_blkCnt_test();

    kfree(*cac_addrP);
    dma_free_coherent(dev, alloc_size, *unc_addrP, *dma_addrP);
    
    return;
}

static void set_en_block_dma_mask(u32 mask)
{
    /* disable, configure, then enable block_dma */
    enable_block_dma_mechanism(0);
    set_block_dma_mask(mask);
    enable_block_dma_mechanism(1);
    return;
}

static void set_bufferable_dmaBlock(int s)
{
    if (s==0) {
        enable_bufferable(1);
        enable_block_dma_mechanism(1);
    }
    else if (s==1) {
        enable_bufferable(1);
        enable_block_dma_mechanism(0);
    }
    else if (s==2) {
        enable_bufferable(0);
        enable_block_dma_mechanism(1);
    }
    else /*s==3*/ {
        enable_bufferable(0);
        enable_block_dma_mechanism(0);
    }
    return;
}

static void set_wBytes (int i, int *wBytesP)
{
    int s_char = sizeof(unsigned char);
    int s_int = sizeof(unsigned int);
    int s_long = sizeof(unsigned long);

    if (i==0) *wBytesP = s_char;
    else if (i==1) *wBytesP = s_int;
    else /*i==2*/ *wBytesP = s_long;

    #ifdef DBG_LOG_ON
    printk("wBytes:0x%x\n", *wBytesP);
    #endif
    return;
}

static void set_start_offset(int k, unsigned long *start_offsetP, unsigned int block_range)
{
    
    int s_long = sizeof(unsigned long);

    if (k==0)
        *start_offsetP=0;
    else if (k==1)
        *start_offsetP=s_long;
    else if (k==2)
        *start_offsetP=(block_range>>1);
    else /*(k==3)*/
        *start_offsetP=(block_range-s_long);

    #ifdef DBG_LOG_ON
    printk("start_offset:0x%lx\n", *start_offsetP);
    #endif
    return;
}

/*
 * including two test cases:
 *   -- testCase==0: cpu writes dram (cpu->dram path)
 *   -- testCase==1: cpu writes npu_sram (cpu->npu_sram path)
 *
 * each tes case has the following testing parameters:
 *   -- dma_block mask
 *     -- m==0: dma_block mask==0x3f (i.e. dma_block range==64B)
 *     -- m==1: dma_block mask==0x7f (i.e. dma_block range==128B)
 *     -- Note: only testCase==0 (cpu->dram path) needs this
 *   -- bufferable and dma_blcok on/off
 *     -- s==0: bufferable     + dma_blcok_on 
 *     -- s==1: bufferable     + dma_blcok_off
 *     -- s==2: non-bufferable + dma_blcok_on
 *     -- s==3: non-bufferable + dma_blcok_off 
 *     -- Note: only testCase==0 (cpu->dram path) has bufferable and dma_block mechanism
 *   -- cpu writes with different bytes
 *     -- i==0: 1-byte cpu writes
 *     -- i==1: 4-byte cpu writes
 *     -- i==2: 8-byte cpu writes
 *   -- cpu uncached or cached access 
 *     -- j==0: cpu uncached access dram or npu_sram 
 *     -- j==1: cpu cached access dram (only)
 *   -- cpu writes, starting at different offest
 *     -- k==0: cpu writes, starting at offset 0x0
 *     -- k==1: cpu writes, starting at offset 0x8
 *     -- k==2: cpu writes, starting at offset (block_range/2)
 *     -- k==3: cpu writes, starting at offset (block_range-8)
 *   -- dma data moving with different burst size (GDMA as example)
 *     -- b==0: GDMA uses burst size of 1DW
 *     -- b==1: GDMA uses burst size of 2DW
 *     -- b==2: GDMA uses burst size of 4DW
 *     -- b==3: GDMA uses burst size of 5DW
 *     -- b==4: GDMA uses burst size of 16DW
 */
int doCpuWt_dmaNpuRd(int argc, char *argv[], void *p)
{
    unsigned long unc_addr, cac_addr;
    dma_addr_t dma_addr;
    unsigned long src_addr1, src_addr2, src_addr3;
    unsigned long dst_addr1, dst_addr2, dst_addr3;
    unsigned int src_phy_addr1, src_phy_addr2, src_phy_addr3;
    unsigned int dst_phy_addr1, dst_phy_addr2, dst_phy_addr3;
    int channel=0;
    int cpuid = smp_processor_id();
    int i, j, k, b, s, m, wBytes, isCacAddr, ret=0;
    int s_long = sizeof(unsigned long);
    unsigned int block_range, test_len;
    unsigned int dma_mask[2] = {0x3f, 0x7f};
    unsigned long start_offset;
    unsigned int gdma_pre_blk_cnt, gdma_cur_blk_cnt;
    unsigned int npu_pre_blk_cnt, npu_cur_blk_cnt;
    unsigned int hsdma_pre_blk_cnt, hsdma_cur_blk_cnt;
    unsigned int pending_cnt[3] = {0};
    unsigned int alloc_size = CPU_WR_DMA_RD_TSIZE*6;
    unsigned int gdma_cmp_err=0, npu_cmp_err=0, hsdma_cmp_err=0;
    unsigned int gdma_cnt_diff, npu_cnt_diff, hsdma_cnt_diff;
    int testCase=0, tout_cnt;


    /* testCase==0: cpu writes dram ********
     * testCase==1: cpu writes npu_sram ****/
    testCase = simple_strtoul(argv[1], NULL, 10); 
    
    pending_cnt[0] = simple_strtoul(argv[2], NULL, 10); /* for pending gdma */
    pending_cnt[1] = simple_strtoul(argv[3], NULL, 10); /* for pending npu */
    pending_cnt[2] = simple_strtoul(argv[4], NULL, 10); /* for pending hsdma */
    
    printk("%s case%d start with pending_cnts: (gdma,npu,hsdma)==(%d %d %d)\n", 
            __func__, testCase, pending_cnt[0], pending_cnt[1],pending_cnt[2]);

    if ((set_hsdma_block_test_hook==NULL) || (enable_hsdma_by_txCpu_hook==NULL) || (wait_hsdma_done_hook==NULL)) {
        printk("ERROR: (set_hsdma_block_test_hook==NULL) || (enable_hsdma_by_txCpu_hook==NULL) || (wait_hsdma_done_hook==NULL)\n");
        return -1;
    }

    if (testCase==0) {
        if (alloc_dram_before_blkCnt_test(alloc_size, &unc_addr, &cac_addr, &dma_addr)) {
            printk("ERROR: alloc_dram_before_blkCnt_test failed\n");
            return -1;
        }

        set_rbus_pending_addr(virt_to_phys(cac_addr));
    }

    #ifdef DBG_LOG_ON
    if (testCase==0) {
        printk("unc_addr:0x%lx  dma_addr:0x%lx\n", unc_addr, dma_addr);
        printk("cac_addr:0x%lx  phys_addr:0x%lx\n", cac_addr, virt_to_phys(cac_addr));
    }
    #endif

    /* m==0: dma_block mask==0x3f (i.e. dma_block range==64B) *************
     * m==1: dma_block mask==0x7f (i.e. dma_block range==128B) ************ 
     * (Note: only testCase==0 (cpu->dram path) has dma_block mechanism) **/
    for (m=0; m<2; m++) {

        if (testCase==0) {
            set_en_block_dma_mask(dma_mask[m]);
        }
        else { /* testCase==1 (cpu->npu_sram path) has no dma_block mechanism, so just run one loop (m==0) */
            if (m!=0) continue;
        }
        
        block_range = (0xffffffff - get_block_dma_mask())+1;
        test_len = block_range<<1;
        #ifdef DBG_LOG_ON
        printk("test_len:0x%x\n", test_len);
        #endif
        
        /* s==0: bufferable     + dma_blcok_on  *********************************************
         * s==1: bufferable     + dma_blcok_off *********************************************
         * s==2: non-bufferable + dma_blcok_on  *********************************************
         * s==3: non-bufferable + dma_blcok_off *********************************************
         * (Note: only testCase==0 (cpu->dram path) has bufferable and dma_block mechanism) */
        for (s=0; s<4; s++) {

            if (testCase==0) {
                set_bufferable_dmaBlock(s);
            }
            else { 
                /* testCase==1, cpu->npu_sram path is fixed as bufferable and has no dma_block mechanism.
                 * so, after cpu writes, uncached read last byte is needed before triggering dma */
                if (s!=0) continue;
            }
            
            /* i==0: 1-byte cpu writes **************
             * i==1: 4-byte cpu writes **************
             * i==2: 8-byte cpu writes **************/
            for (i=0; i<3; i++) {

                set_wBytes(i, &wBytes);
                
                /* j==0: cpu uncached access dram or npu_sram ****************
                 * j==1: cpu cached access dram (only) ***********************/
                for (j=0; j<2; j++) {

                    if (j==0) {
                        if (testCase==0) {
                            src_addr1 = unc_addr;
                            src_phy_addr1 = dma_addr;
                        }
                        else { /* testCase==1 */
                            /* in case "NPU_CODE_IN_SRAM", use last 128K to test */
                            src_addr1 = get_npu_384k_sram_base()+S_256K;
                            src_phy_addr1 = NPU_384K_SRAM_BASE+S_256K;
                        }
                        isCacAddr = 0;
                    }
                    else { /*j==1*/
                        if (testCase==0) {
                            src_addr1 = cac_addr;
                            src_phy_addr1 = virt_to_phys(cac_addr);
                        }
                        else { /* testCase==1 (cpu->npu_sram path is always uncached access) */
                            continue;
                        }
                        isCacAddr = 1;
                    }

                    dst_addr1 = src_addr1+CPU_WR_DMA_RD_TSIZE;
                    src_addr2 = dst_addr1+CPU_WR_DMA_RD_TSIZE;
                    dst_addr2 = src_addr2+CPU_WR_DMA_RD_TSIZE;
                    src_addr3 = dst_addr2+CPU_WR_DMA_RD_TSIZE;
                    dst_addr3 = src_addr3+CPU_WR_DMA_RD_TSIZE;

                    dst_phy_addr1 = src_phy_addr1+CPU_WR_DMA_RD_TSIZE;
                    src_phy_addr2 = dst_phy_addr1+CPU_WR_DMA_RD_TSIZE;
                    dst_phy_addr2 = src_phy_addr2+CPU_WR_DMA_RD_TSIZE;
                    src_phy_addr3 = dst_phy_addr2+CPU_WR_DMA_RD_TSIZE;
                    dst_phy_addr3 = src_phy_addr3+CPU_WR_DMA_RD_TSIZE;

                    #ifdef DBG_LOG_ON
                    printk("src_addr1:0x%lx  src_phy_addr1:0x%lx\n", src_addr1, src_phy_addr1);
                    printk("dst_addr1:0x%lx  dst_phy_addr1:0x%lx\n", dst_addr1, dst_phy_addr1);
                    printk("src_addr2:0x%lx  src_phy_addr2:0x%lx\n", src_addr2, src_phy_addr2);
                    printk("dst_addr2:0x%lx  dst_phy_addr2:0x%lx\n", dst_addr2, dst_phy_addr2);
                    printk("src_addr3:0x%lx  src_phy_addr3:0x%lx\n", src_addr3, src_phy_addr3);
                    printk("dst_addr3:0x%lx  dst_phy_addr3:0x%lx\n", dst_addr3, dst_phy_addr3);
                    #endif
                    
                    /* k==0: cpu writes, starting at offset 0x0 ****************
                     * k==1: cpu writes, starting at offset 0x8 ****************
                     * k==2: cpu writes, starting at offset (block_range/2) ****
                     * k==3: cpu writes, starting at offset (block_range-8) ****/
                    for (k=0; k<4; k++) {

                        set_start_offset(k, &start_offset, block_range);
                        
                        /* (GDMA as example) ********************************
                         * b==0: GDMA/HSDMA uses burst size of 1DW(4 bytes)/4DW(16 bytes)  ****************
                         * b==1: GDMA/HSDMA uses burst size of 2DW(8 bytes)/8DW(32 bytes)  ****************
                         * b==2: GDMA/HSDMA uses burst size of 4DW(16 bytes)/16DW(64 bytes) ****************
                         * b==3: GDMA/HSDMA uses burst size of 8DW(20 bytes)/32DW(128 bytes) ****************
                         * b==4: GDMA/HSDMA uses burst size of 16DW(32 bytes)/64DW(256 bytes) ****************/
                        /* !! Note: if block_mask_size < dma_burst_size, dma can go read DRAM even if !! 
                         * !!       the last part of src data is still in Write_Buffer !! */
                        for (b=0; b<5; b++) {

                            #ifdef DBG_LOG_ON
                            printk("bsize:%d\n", b);
                            #endif

                            /**************************************************************************************
                             * Examples of CPU and DMA's access range during test:
                             *   -- if block_range== 64B, test_len==128B, and start_offset==  0B, cpu/dma will access byte0~127
                             *   -- if block_range== 64B, test_len==128B, and start_offset== 32B, cpu/dma will access byte32~127
                             *   -- if block_range==128B, test_len==256B, and start_offset==  4B, cpu/dma will access byte4~255
                             *   -- if block_range==128B, test_len==256B, and start_offset==124B, cpu/dma will access byte124~255
                             **************************************************************************************/

                            /**************************************************************************************
                             **** CPU Write, GDMA read (testCase==0 only, because GDMA can't access npu_sram) *****
                             **************************************************************************************/

                            if (testCase==0){ /* Note: GDMA can't access npu_sram */
                            
                                /* configure GDMA but not enable */
                                SET_GDMA_CONFIG(channel, src_phy_addr1+start_offset, dst_phy_addr1+start_offset, (((test_len -start_offset)&0xffff)<<16)|(b<<3)|(DISABLE<<1)|(1<<0), 0x4);
                                tout_cnt=10;
                                    
                                gdma_pre_blk_cnt = get_block_dma_counter(bctc_gdma);

                                set_rbus_pending_cnt(pending_cnt[0]);
                                enable_rbus_pending(1);

                                /* prepare data for GDMA, then enable right away */
                                cpuWrites_beforeDMA(src_addr1+start_offset, dst_addr1+start_offset, test_len-start_offset, isCacAddr, wBytes, testCase);
                                set_GDMA_enable_bit(channel);
                                
                                /* wait until GDMA is done */
                                while(!IS_GDMA_DONE(channel)) {
                                    if ((tout_cnt--)<0) {
                                        printk("ERROR: GDMA done_bit timeout\n");
                                        ret=-1;
                                        goto cpuWtdmaNpuRd_out;
                                    }
                                    msleep(1);
                                }
                                CLEAR_GDMA_DONE(channel); /* clear done bit */

                                enable_rbus_pending(0);

                                gdma_cur_blk_cnt = get_block_dma_counter(bctc_gdma);
                                        
                                /* cpu compares data after dma dtat moving */
                                if (cpuCompare_afterDMA(src_addr1+start_offset, dst_addr1+start_offset, test_len-start_offset, s)) {
                                    
                                    if ((testCase==0) && (s==1)) { /* expect error for bufferable+dma_blcok_off */
                                        gdma_cmp_err++;
                                    }
                                    else {
                                        printk("ERROR: cpuCompare GDMA failed\n");
                                        ret=-1;
                                        goto cpuWtdmaNpuRd_out;
                                    }
                                }
                            
                            } /* testCase==0 */

                            /*******************************************************************************
                             **** CPU Write, NPU read ******************************************************
                             *******************************************************************************/
                            
                            /* configure NPU but not enable */
                            set_npu_dma_config(src_phy_addr2+start_offset, dst_phy_addr2+start_offset, test_len -start_offset, b);
                            tout_cnt=10;

                            if (testCase==0) {
                                npu_pre_blk_cnt = get_block_dma_counter(bctc_npu);
                                set_rbus_pending_cnt(pending_cnt[1]);
                                enable_rbus_pending(1);
                            }

                            /* prepare data for NPU, then enable right away */
                            cpuWrites_beforeDMA(src_addr2+start_offset, dst_addr2+start_offset, test_len-start_offset, isCacAddr, wBytes, testCase); 
                            enable_npu_dma();
                                            
                            /* wait until NPU is done */
                            while(!is_npu_dma_done()) {
                                if ((tout_cnt--)<0) {
                                    printk("ERROR: NPU done_bit timeout\n");
                                    ret=-1;
                                    goto cpuWtdmaNpuRd_out;
                                }
                                msleep(1);
                            }

                            if (testCase==0) {
                                enable_rbus_pending(0);
                                npu_cur_blk_cnt = get_block_dma_counter(bctc_npu);
                            }
                            
                            /* cpu compares data after dma dtat moving */
                            if (cpuCompare_afterDMA(src_addr2+start_offset, dst_addr2+start_offset, test_len-start_offset, s)) {
                                
                                if ((testCase==0) && (s==1)) { /* expect error for bufferable+dma_blcok_off */
                                    npu_cmp_err++;
                                }
                                else {
                                    printk("ERROR: cpuCompare NPU failed\n");
                                    ret=-1;
                                    goto cpuWtdmaNpuRd_out;
                                }
                            }

                            /*******************************************************************************
                             **** CPU Write, HSDMA read ****************************************************
                             *******************************************************************************/

                            if ((testCase==1) && (b==4)) /* HSDMA burst_szie==256B only supports DRAM */
                                continue;

                            /* configure HSDMA but not enable */
                            set_hsdma_block_test_hook(src_phy_addr3+start_offset, dst_phy_addr3+start_offset, test_len-start_offset, b); 

                            if (testCase==0) {
                                /* hsdma uses the same block_cnt as gdma */
                                hsdma_pre_blk_cnt = get_block_dma_counter(bctc_gdma);
                                set_rbus_pending_cnt(pending_cnt[2]);
                                enable_rbus_pending(1);
                            }

                            /* prepare data for HSDMA, then enable right away */
                            cpuWrites_beforeDMA(src_addr3+start_offset, dst_addr3+start_offset, test_len-start_offset, isCacAddr, wBytes, testCase);
                            enable_hsdma_by_txCpu_hook();

                            /* wait until hsdma is done */
                            if (wait_hsdma_done_hook()) {
                                ret=-1;
                                goto cpuWtdmaNpuRd_out;
                            }

                            if (testCase==0) {
                                enable_rbus_pending(0);
                                /* hsdma uses the same block_cnt as gdma */
                                hsdma_cur_blk_cnt = get_block_dma_counter(bctc_gdma);
                            }
                                    
                            /* cpu compares data after dma dtat moving */
                            if (cpuCompare_afterDMA(src_addr3+start_offset, dst_addr3+start_offset, test_len-start_offset, s)) {

                                if ((testCase==0) && (s==1)) { /* expect error for bufferable+dma_blcok_off */
                                    hsdma_cmp_err++;
                                }
                                else {
                                    printk("ERROR: cpuCompare HSDMA failed\n");
                                    ret=-1;
                                    goto cpuWtdmaNpuRd_out;
                                }
                            }        

                            /*******************************************************************************
                             **** Block Counters Statistics ************************************************
                             *******************************************************************************/

                            if (testCase==0) /* only cpu->dram path has dma_block counters */
                            {
                                gdma_cnt_diff = gdma_cur_blk_cnt-gdma_pre_blk_cnt;
                                npu_cnt_diff = npu_cur_blk_cnt-npu_pre_blk_cnt;
                                hsdma_cnt_diff = hsdma_cur_blk_cnt-hsdma_pre_blk_cnt;

                                if (s==0) { /* bufferable+dma_blcok_on */
                                    printk("(m:%d, s:%d, i:%d, j:%d, k:%d, b:%d) gdma_blkCnt_diff:0x%x  npu_blkCnt_diff:0x%x  hsdma_blkCnt_diff:0x%x\n", 
                                        m, s, i, j, k, b, gdma_cnt_diff, npu_cnt_diff, hsdma_cnt_diff);
                                }
                                else {
                                    if ((gdma_cnt_diff!=0) || (npu_cnt_diff!=0) || (hsdma_cnt_diff!=0)) {
                                        printk("ERROR: ((gdma_cnt_diff:0x%x !=0) || (npu_cnt_diff:0x%x !=0) || (hsdma_cnt_diff:0x%x !=0))\n",
                                                gdma_cnt_diff, npu_cnt_diff, hsdma_cnt_diff);
                                        ret=-1;
                                        goto cpuWtdmaNpuRd_out;
                                    }
                                }
                            }

                        } /* "b" loop */
                    } /* "k" loop */
                } /* "j" loop */
            } /* "i" loop */
        } /* "s" loop */
    } /* "m" loop */

cpuWtdmaNpuRd_out:
    if (ret) {
        printk("ERROR: cpuWtdmaNpuRd case%d failed at m:%d, s:%d, i:%d, j:%d, k:%d, b:%d\n", testCase, m, s, i, j, k, b);
    }
    else {
        if (testCase==0) { /* xxx_cmp_err are only recorded for ((testCase==0) && (s==1)) case */
            
            printk("(gdma_cmp_err:0x%x  npu_cmp_err:0x%x  hsdma_cmp_err:0x%x)\n", gdma_cmp_err, npu_cmp_err, hsdma_cmp_err);
            
            if ((gdma_cmp_err==0) || (npu_cmp_err==0) || (hsdma_cmp_err==0))
                printk("ERROR: (gdma_cmp_err==0) || (npu_cmp_err==0) || (hsdma_cmp_err==0)\n");
            else
                printk("\n%s case%d done\n", __func__, testCase);
        }
        else { /* testCase==1 */
            printk("\n%s case%d done\n", __func__, testCase);
        }
    }

    if (testCase==0)
        free_dram_after_blkCnt_test(alloc_size, &unc_addr, &cac_addr, &dma_addr);
    
    return ret;
}

/*
 * e==0: block mechanism off, e==1: block mechanism on
 * i==0 or 1: cpu_write's and dma_read's masked addresses are matched, i==2: not matched.
 * phase==1: during dma data moving, phase==2: after dma data moving done 
 */
static int check_dma_block_cnt(int e, int i, int phase)
{
    u32 cur_block_cnt=0, pre_block_cnt=0, block_cnt_diff=0;
    u32 cur_block_cnt2=0, pre_block_cnt2=0, block_cnt_diff2=0;
    u32 count_down=MAX_COUNT_DOWN;
    int delay_ms=10;

    if ((phase==1) && (e==1) && (i<2)) { /* dma block counters should count */
                
        /* make sure that dma_block_cnt is counting */
        if (dmaBlkCntType==bctc_gdma_qdmalan) {
            cur_block_cnt = get_block_dma_counter(bctc_gdma);
            cur_block_cnt2 = get_block_dma_counter(bctc_qdma_lan);
        }
        else
            cur_block_cnt = get_block_dma_counter(dmaBlkCntType);
        
        while(1) {
            
            mdelay(delay_ms);
            pre_block_cnt = cur_block_cnt;
            if (dmaBlkCntType==bctc_gdma_qdmalan) {
                pre_block_cnt2 = cur_block_cnt2;
                cur_block_cnt = get_block_dma_counter(bctc_gdma);
                cur_block_cnt2 = get_block_dma_counter(bctc_qdma_lan);
            }
            else
                cur_block_cnt = get_block_dma_counter(dmaBlkCntType);
            
            block_cnt_diff = cur_block_cnt-pre_block_cnt;
            if (dmaBlkCntType==bctc_gdma_qdmalan)
                block_cnt_diff2 = cur_block_cnt2-pre_block_cnt2;
            
            if (((dmaBlkCntType!=bctc_gdma_qdmalan) && (block_cnt_diff>0)) ||
                ((dmaBlkCntType==bctc_gdma_qdmalan) && (block_cnt_diff>0) && (block_cnt_diff2>0)))
                break;
            else {
                if (count_down) {
                    mdelay(delay_ms);
                    count_down--;
                    printk("Note: not counting for %d times\n", MAX_COUNT_DOWN-count_down);
                }
                else {
                    if (dmaBlkCntType==bctc_gdma_qdmalan) {
                        printk("ERROR1: gdma or qdma_lan is not counting\n");
                        printk("\tpre_cnt: 0x%x, cur_cnt: 0x%x, diff: 0x%x\n", pre_block_cnt, cur_block_cnt, block_cnt_diff);
                        printk("\tpre_cnt2:0x%x, cur_cnt2:0x%x, diff2:0x%x\n", pre_block_cnt2, cur_block_cnt2, block_cnt_diff2);
                    }
                    else
                        printk("ERROR1: %s block_cnt is not counting\n", dmaNames[dmaBlkCntType]);
                    show_block_dbg_regs();
                    return -1;
                }
            }
        }

        if (dmaBlkCntType==bctc_gdma_qdmalan)
            printk("gdma_block_cnt:0x%x, qdmalan_block_cnt:0x%x in %d ms (phase%d)\n", block_cnt_diff, block_cnt_diff2, delay_ms, phase);
        else
            printk("%s_block_cnt:0x%x in %d ms (phase%d)\n", dmaNames[dmaBlkCntType], block_cnt_diff, delay_ms, phase);
    }
    else { /* make sure that gdma_block_cnt won't count */

        if (dmaBlkCntType==bctc_gdma_qdmalan) {
            pre_block_cnt = get_block_dma_counter(bctc_gdma);
            pre_block_cnt2 = get_block_dma_counter(bctc_qdma_lan);
        }
        else {
            pre_block_cnt = get_block_dma_counter(dmaBlkCntType);
        }
        
        mdelay(delay_ms);
        if (dmaBlkCntType==bctc_gdma_qdmalan) {
            cur_block_cnt = get_block_dma_counter(bctc_gdma);
            cur_block_cnt2 = get_block_dma_counter(bctc_qdma_lan);
        }
        else {
            cur_block_cnt = get_block_dma_counter(dmaBlkCntType);
        }
        
        if ((pre_block_cnt != cur_block_cnt) || (pre_block_cnt2 != cur_block_cnt2)) {
            if (dmaBlkCntType==bctc_gdma_qdmalan)
                printk("\nERROR2: [gdma] pre_cnt:0x%x != cur_cnt:0x%x  or  [qdmalan] pre_cnt:0x%x != cur_cnt:0x%x\n", 
                        pre_block_cnt, cur_block_cnt, pre_block_cnt2, cur_block_cnt2);
            else {
                printk("\nERROR2: %s_block_cnt: pre_block_cnt:0x%x != cur_block_cnt:0x%x\n", 
                        dmaNames[dmaBlkCntType], pre_block_cnt, cur_block_cnt);
            }
            show_block_dbg_regs();
            return -1;
        }

        if (dmaBlkCntType!=bctc_gdma_qdmalan)
            printk("%s_block_cnt:0x0 which is correct (phase%d)\n", dmaNames[dmaBlkCntType], phase);
    }


    return 0;
}

static int read_compare_data(int e, int i, unsigned long cmp_addr, unsigned long cmp_val)
{
    /* when block is enabled, gdma-read should happen "after" cpu-write, so comparison should succeed */
    if ((e==1) && (i<2)) {
        if (rdMeml(cmp_addr)!=cmp_val) {
            printk("ERROR3: rdMeml(0x%lx):0x%lx != cmp_val:0x%lx\n", cmp_addr, rdMeml(cmp_addr), cmp_val);
            return -1;
        }
    }

    /* Note: it makes no sense to compare data for ((e==0) && (i<2)) cases because CPU0 keeps writing to src */

    return 0;
}

static void do_gdma_config(u32 srcAddr, u32 dstAddr, u32 test_len)
{
    int channel=0;
    int coherent_en=0;
    int enable=0;

    /* gdma-coherent is disabled here, otherwise "i==2" case will fail, because 
     * if gdma-coherent is enabled, after gdma finishes writing to dst area, 
     * it will uncached read back last byte of dst area which matches cpu_wt_addr's
     * 64-byte-masked value, which triggers block_cnt to count */
    /* configure GDMA but not enable */
    SET_GDMA_CONFIG(channel, srcAddr, dstAddr, ((test_len&0xffff)<<16)|(0<<3)|(enable<<1)|(1<<0), (coherent_en<<2));

    return;
}

int dma_block_cnt_test(void)
{
    int i, m, e, r, j;
    u32 test_len, test_offset;
    unsigned long cpu_wt_addr, cpu_cmp_addr;
    unsigned long cpu_wt_addr2, cpu_cmp_addr2;
    u32 block_mask[] = {0x3f, 0x7f};
    int masks = sizeof(block_mask)/sizeof(block_mask[0]);
    unsigned long wtVal = 0x12345678;
    unsigned long wtVal2 = 0x23456789;
    u32 dma_src_addr, dma_dst_addr, dma_src_addr2;
    unsigned int alloc_size = CPU_WR_DMA_RD_TSIZE*6;
    unsigned long unc_addr, tmp_addr;
    dma_addr_t dma_addr;
    int channel=0;
    struct device *dev=NULL;
    QDMA_DramTestCfg_T qdmaDramTestCfg;
    int qdmaRxPollCnt;
    tsoTransTestInfo_t tdmaDramTestCfg;

    if ((dmaBlkCntType<bctc_gdma) || (dmaBlkCntType>=bctc_max_case)) {
        printk("ERROR: dmaBlkCntType:%d is wrong\n");
        return -1;
    }

    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("ERROR: get_gdmpSram_dev is NULL\n");
        return -1;
    }

    if ((ecnt_get_rdbypass_cfg()&0x1)==0) {
        printk("ERROR: rd_bypass_wt is off\n");
        return -1;
    }

    if (alloc_mem_before_blkCnt_test(alloc_size)) {
        printk("ERROR: alloc_mem_before_blkCnt_test failed\n");
        return -1;
    }

    /* dma_alloc_coherent should use the same "alloc_size" as alloc_mem_before_blkCnt_test,
     * otherwise, dma_alloc_coherent may get unc_addr in between those memory allocated in alloc_mem_before_blkCnt_test */
    unc_addr = (unsigned long) dma_alloc_coherent(dev, alloc_size, &dma_addr, GFP_KERNEL);
    if ((unc_addr==NULL)) {
        printk("ERROR: dma_alloc_coherent failed\n");
        free_mem_after_blkCnt_test();
        return -1;
    }
    
    printk("[H%d]unc_addr:0x%lx, dma_addr:0x%lx, dmaName:%s\n", smp_processor_id(), unc_addr, dma_addr, dmaNames[dmaBlkCntType]);

    /* set pending (high-8bits) address */
    set_rbus_pending_addr(dma_addr);
    /* set pending time for matched address, 0 means forever */
    set_rbus_pending_cnt(data1);
    printk("rbus_pending_cnt:%d\n\n", data1);

    /* 
     * e==1 for enabling, e==0 for disabling block mechanism.
     * m==0 for 64B, m==1 for 128B block range.
     * i==0 or 1 for block counting case, i==2 for block not counting case.
     *
     * in case (e,m,i)==(1,0,0), cpu writes to addr:0x00 and dma reads addr:0x0~0x3f,  so block should heppen. 
     * in case (e,m,i)==(1,0,1), cpu writes to addr:0x20 and dma reads addr:0x0~0x3f, so block should heppen. 
     * in case (e,m,i)==(1,0,2), cpu writes to addr:0x40 and dma reads addr:0x0~0x3f, so block should not heppen. 
     */
    for (e=1; e>=0; e--) {
        for (m=0; m<masks; m++) {
            for (i=0; i<3; i++) {

                if (dmaBlkCntType==bctc_gdma_qdmalan) {
                    if ((e==0) || (i>=2)) continue; /* only focus on testing 2 dma block counters */
                }

                printk("(e,m,i)==(%d,%d,%d)\n", e,m,i);

                test_len = block_mask[m]+1;
                test_offset = (test_len>>1);
                cpu_wt_addr = unc_addr+(test_offset*i);
                dma_src_addr = (u32)dma_addr;
                dma_dst_addr = (u32)dma_addr+test_len;
                
                /* for gdma_qdmalan case, cpu_wt_addr is for gdma and cpu_wt_addr2 is for qdma_lan */
                if (dmaBlkCntType==bctc_gdma_qdmalan) {
                    cpu_wt_addr2 = unc_addr+(test_len<<2)+(test_offset*i);
                    dma_src_addr2 = dma_addr+(test_len<<2);
                }
                
                /* qdma will read 16 more bytes from source area, so cpu should write to the 3rd test_len area 
                 * in case (i==2) that qdma blcok counters don't want to be triggered */
                if ((i==2) && ((dmaBlkCntType==bctc_qdma_lan) || (dmaBlkCntType==bctc_qdma_wan) || (dmaBlkCntType==bctc_gdma_qdmalan))) {
                    if (dmaBlkCntType==bctc_gdma_qdmalan)
                        cpu_wt_addr2 += test_len;
                    else /* qdma_lan, qdma_wan */
                        cpu_wt_addr += test_len;
                }
                
                /* reset dma src & dst area */
                tmp_addr = unc_addr;
                for (r=0; r<(test_len<<1); r++, tmp_addr++){
                    wtMemB(tmp_addr, 0);
                }
                if (dmaBlkCntType==bctc_gdma_qdmalan) {
                    tmp_addr = unc_addr+(test_len<<2);
                    for (r=0; r<(test_len<<1); r++, tmp_addr++){
                        wtMemB(tmp_addr, 0);
                    }
                }
                wmb();

                /* disable block_dma */
                enable_block_dma_mechanism(0);
                        
                if (e) { /* set and enable block_dma */
                    set_block_dma_mask(block_mask[m]);
                    enable_block_dma_mechanism(1);
                    printk("block_mask:0x%x, cpu_wt_addr:0x%lx\n", block_mask[m], cpu_wt_addr);
                    if (dmaBlkCntType==bctc_gdma_qdmalan)
                        printk("cpu_wt_addr2:0x%lx\n", cpu_wt_addr2);
                }
                else {
                    printk("block disable case for m=%d,i=%d\n", m,i);
                }

                /* do DMA configuration before test */

                switch (dmaBlkCntType) {
                
                    case bctc_gdma:
                    case bctc_gdma_qdmalan:
                        
                        do_gdma_config(dma_src_addr, dma_dst_addr, test_len);
                        
                        if (dmaBlkCntType!=bctc_gdma_qdmalan) break;

                    case bctc_qdma_lan:
                    case bctc_qdma_wan:
                        
                        memset(&qdmaDramTestCfg, 0, sizeof(QDMA_DramTestCfg_T));
                        
                        if (dmaBlkCntType==bctc_gdma_qdmalan) {
                            #if 1 //Test: gdma & qdma read the same area
                            qdmaDramTestCfg.dataP = dma_src_addr;
                            #else
                            qdmaDramTestCfg.dataP = dma_src_addr2;
                            #endif
                        }
                        else {
                            qdmaDramTestCfg.dataP = dma_src_addr;
                        }
                        qdmaDramTestCfg.dataLen = test_len;
                        
                        if ((dmaBlkCntType==bctc_qdma_lan) || (dmaBlkCntType==bctc_gdma_qdmalan))
                            QDMA_API_DRAM_TEST_DMA_CONFIG(ECNT_QDMA_LAN, &qdmaDramTestCfg);
                        else
                            QDMA_API_DRAM_TEST_DMA_CONFIG(ECNT_QDMA_WAN, &qdmaDramTestCfg);

                        break;
                        
                    case bctc_tdma:

                        memset(&qdmaDramTestCfg, 0, sizeof(QDMA_DramTestCfg_T)); /* tdma will use qdma_lan's RX_DONE and DST_GET APIs */
                        memset(&tdmaDramTestCfg, 0, sizeof(tsoTransTestInfo_t));
                        tdmaDramTestCfg.dataP = dma_src_addr;
                        tdmaDramTestCfg.dataLen = test_len;
                        TSO_TDMA_DMA_CONFIG(&tdmaDramTestCfg);

                        break;
                    
                    default:
                        printk("ERROR: dmaBlkCntType:%d is not supported\n", dmaBlkCntType);
                        goto dma_block_cnt_test_end;        
                }
                
                /* prepare Info for CPU1 */
                dmaBlkCntInfo[0] = 0;
                dmaBlkCntInfo[1] = e;
                dmaBlkCntInfo[2] = i;

                /* enable_rbus_pending, so that cpu-write will stay in Wbuff longer */        
                enable_rbus_pending(1);
                while(is_rbus_pending_enabled()==0);

                /* cpu0 wakes up cpu1 then keeps writing until cpu1 finishes checking blkCnt. 
                 * cpu0's continuous writing make it possible that cpu-write can stay in Wbuff when gdma-read happens */
                cpu1_wake_up = 1;
                while (dmaBlkCntInfo[0]==0) {
                    for (j=0; j<10000; j++) {
                        wtMeml(cpu_wt_addr, wtVal);
                        #if 0 //Test: gdma & qdma read the same area
                        if (dmaBlkCntType==bctc_gdma_qdmalan)
                            wtMeml(cpu_wt_addr2, wtVal2);
                        #endif
                    }
                }
                
                /* cpu1 failed at check_dma_block_cnt() */
                if (dmaBlkCntInfo[0]==-1) {
                    enable_rbus_pending(0);
                    goto dma_block_cnt_test_end;
                }

                /* disable rbus_cmd_pending to let the cpu-wt-cmd out, otherwise, 
                 * gdma-read will be blocked by cpu-wt-cmd forever. */
                enable_rbus_pending(0);

                /* Polling RX Done and get cpu compare address */

                switch (dmaBlkCntType) {

                    case bctc_gdma:
                    case bctc_gdma_qdmalan:
                        /* wait until GDMA is done */
                        while(!IS_GDMA_DONE(channel));
                        CLEAR_GDMA_DONE(channel); /* clear done bit */

                        /* get cpu compare addr */
                        cpu_cmp_addr = cpu_wt_addr+test_len;

                        if (dmaBlkCntType!=bctc_gdma_qdmalan) break;
                    
                    case bctc_qdma_lan:
                    case bctc_qdma_wan:
                    case bctc_tdma:
                    /* qdma_lan, qdma_wan, and tdma all share qdma_lan's RX_DONE and DST_GET APIs */
                    
                        /* wait until qdma_lan rx is done */
                        qdmaRxPollCnt=3;
                        while(1) {
                            QDMA_API_DRAM_TEST_RX_DONE(ECNT_QDMA_LAN, &qdmaDramTestCfg);
                            if (qdmaDramTestCfg.rxDone) break;
                            else msleep(10); /* don't use "mdelay()" because current CPU may also be used by qdma_lan driver to receive packet */
                            
                            if ((qdmaRxPollCnt--)<0) {
                                printk("ERROR: qdmaRxPollCnt<0\n");
                                goto dma_block_cnt_test_end;
                            }
                        }

                        /* get cpu compare addr */
                        QDMA_API_DRAM_TEST_DST_GET(ECNT_QDMA_LAN, &qdmaDramTestCfg);
                        if (dmaBlkCntType==bctc_gdma_qdmalan)
                            cpu_cmp_addr2 = ((unsigned long)qdmaDramTestCfg.skb->data)+(test_offset*i);
                        else
                            cpu_cmp_addr = ((unsigned long)qdmaDramTestCfg.skb->data)+(test_offset*i);

                        break;
                    
                    default:
                        goto dma_block_cnt_test_end;
                }

                if (check_dma_block_cnt(e, i, 2))
                    goto dma_block_cnt_test_end;

                if (read_compare_data(e, i, cpu_cmp_addr, wtVal))
                    goto dma_block_cnt_test_end;

                if (dmaBlkCntType==bctc_gdma_qdmalan) {
                    #if 1 //Test: gdma & qdma read the same area
                    if (read_compare_data(e, i, cpu_cmp_addr2, wtVal))
                    #else
                    if (read_compare_data(e, i, cpu_cmp_addr2, wtVal2))
                    #endif
                        goto dma_block_cnt_test_end;
                }

                if ((dmaBlkCntType==bctc_qdma_lan) || (dmaBlkCntType==bctc_qdma_wan) || (dmaBlkCntType==bctc_tdma) || (dmaBlkCntType==bctc_gdma_qdmalan))
                    dev_kfree_skb_any(qdmaDramTestCfg.skb);

                printk("\n");
                
            } /* i loop */
        } /* m loop */
    } /* e loop */
    

    printk("\n[%s] %s done\n", dmaNames[dmaBlkCntType], __func__);
    free_mem_after_blkCnt_test();
    dma_free_coherent(dev, alloc_size, unc_addr, dma_addr);
    return 0;

dma_block_cnt_test_end:
    free_mem_after_blkCnt_test();
    dma_free_coherent(dev, alloc_size, unc_addr, dma_addr);
    printk("\nERROR for [%s] when (e,m,i)==(%d,%d,%d)\n", dmaNames[dmaBlkCntType], e , m, i);
    
    return -1;
}

void dma_enable_and_blkCnt_check(void)
{
    int channel = 0;    
    int e = dmaBlkCntInfo[1];
    int i = dmaBlkCntInfo[2];

    printk("[H%d] enable %s\n", smp_processor_id(), dmaNames[dmaBlkCntType]);


    if (dmaBlkCntType==bctc_gdma) {
        /* enable GDMA */
        set_GDMA_enable_bit(channel);
    }
    else if (dmaBlkCntType==bctc_qdma_lan) {
        /* enable QDMA_LAN TX */
        QDMA_API_DRAM_TEST_DMA_ENABLE(ECNT_QDMA_LAN);
    }
    else if (dmaBlkCntType==bctc_qdma_wan) {
        /* enable QDMA_WAN TX */
        QDMA_API_DRAM_TEST_DMA_ENABLE(ECNT_QDMA_WAN);
    }
    else if (dmaBlkCntType==bctc_tdma) {
        /* enable TDMA TX */
        TSO_TDMA_DMA_ENABLE();
    }
    else if (dmaBlkCntType==bctc_gdma_qdmalan) {
        /* enable GDMA */
        set_GDMA_enable_bit(channel);
        /* enable QDMA_LAN TX */
        QDMA_API_DRAM_TEST_DMA_ENABLE(ECNT_QDMA_LAN);
    }
    else {
        return;
    }
    
    if (check_dma_block_cnt(e, i, 1))
        dmaBlkCntInfo[0]=-1; /* check fail */
    else
        dmaBlkCntInfo[0]=1; /* check OK */

    return;
}

int doCpu0dmaBlkCntTest(void *arg)
{
    dmaBlkCntType = data0;

    dma_block_cnt_test();

    return 0;
}

int doCpu1triggerDma(void *arg)
{
    while (1) {
        
        while (cpu1_wake_up==0) 
            msleep(1);

        msleep(1);
        dma_enable_and_blkCnt_check();

        cpu1_wake_up = 0;
    }
    
    return 0;
}

int dohighMemTest(int argc, char *argv[], void *p){
	int i = 0;
	int k = 0;
	int j = 0;

	int test_result = -1;
	unsigned long **secondArray;

	unsigned long highMemBase = 0;
	unsigned long checkValue = 0;
	unsigned int dram_size = 0;

	dram_size = GET_DRAM_SIZE();

	if (dram_size < 512)
	{
		printk("System DRAM size is %x less than 512M, test abort!\r\n", dram_size);
		return 0;
	}

	printk("DRAM size is %dM\n", (int)dram_size);
	
	highMemBase = DRAM_BASE + HIGHMEM_512M_SIZE;
	

		secondArray = (unsigned long **) kmalloc(HIGHMEM_TEST_ARRAY_MAX_SIZE * sizeof(unsigned long *), GFP_KERNEL);
		
		for (i = 0; i < HIGHMEM_TEST_ARRAY_MAX_SIZE; i++)
		{
			secondArray[i] = (unsigned long *)kmalloc(HIGHMEM_TEST_SIZE,GFP_KERNEL);
			

			if( secondArray[i] == NULL ){
				printk("secondArray[%d] memory allocate fail!!\r\n",i);
				test_result = -1;
				goto testdone;
			}
			
			if (virt_to_phys(secondArray[i])>highMemBase)
			{
				printk("alloc virtual addr = %lx phy = %lx\n",(unsigned long)secondArray[i], (unsigned long)virt_to_phys(secondArray[i]));
				
				/* highMem write*/
				for (j = 0; j < (HIGHMEM_TEST_SIZE/4)-1; j++)
				{
					wtMeml((secondArray[i]+j),testWordPat[j%4]);
					//printk ("secondArray[i]+j) = %lx, data = %d\n",secondArray[i]+j, rdMeml(secondArray[i]+j));
				}
				printk("highMemTest: start read data and check !\n");
				
				for (j = 0;j<(HIGHMEM_TEST_SIZE/4)-1;j++)
				{
					checkValue = rdMeml(secondArray[i]+j);
					//printk ("checkValue = %d, data = %d\n",checkValue, rdMeml(secondArray[i]+j));
					if (checkValue != testWordPat[j%4])
					{
						printk ("highMemTest: data compare error !!! position is %d %d checkValue = %lx\n", i, j, checkValue);
						test_result = -1;
						goto testdone;
					}
					
				}
				test_result = 0;
				/* Test succeed */
				goto testdone;	
			}
		}

testdone:
	/*free allocated mem*/

	for (j = 0; j < i; j++)
	{
		kfree(secondArray[j]);
	}
	kfree(secondArray);
	
	if (test_result == 0)
		printk("highMemTest: Test Pass !\n");
	else
		printk("highMemTest: Test Fail !\n");
	return test_result;

}

#ifdef MBOX_API_TEST
int h_mfunc_wifi_cb0(unsigned long addr, unsigned short len)
{
    printk("%s addr:0x%x, len:0x%x\n", __func__, addr, len);
    return 1;
}
int h_mfunc_notify_cb0(unsigned long addr, unsigned short len)
{
    printk("%s addr:0x%x, len:0x%x, content:0x%x\n", __func__, addr, len, rdMemW(addr));
    return 2;
}
int h_mfunc_wifi_cb1(unsigned long addr, unsigned short len)
{
    printk("%s addr:0x%x, len:0x%x\n", __func__, addr, len);
    return 4;
}
int h_mfunc_wifi_cb2(unsigned long addr, unsigned short len)
{
    printk("%s addr:0x%x, len:0x%x\n", __func__, addr, len);
    return 5;
}
int h_mfunc_wifi_cb3(unsigned long addr, unsigned short len)
{
    printk("%s addr:0x%x, len:0x%x\n", __func__, addr, len);
    mboxAPItestDone=1;
    return 6;
}

int doMboxAPItest(int argc, char *argv[], void *p)
{
    struct device *dev=NULL;
    dma_addr_t dma_addr;
    unsigned long addr, cac_addr, unc_addr, unc_addr2;
    unsigned int phy_addr, phy_addr2;
    unsigned long val;
    unsigned int sizel = sizeof(unsigned long);
    unsigned int len = S_4K;
    unsigned short len2;
    unsigned int word = (len/sizel);
    unsigned int i;
    int res;
    npuMboxInfo_t mboxInfo;

    
    if ((dev=get_npu_dev())==NULL) {
        printk("%s get_npu_dev failed\n", __func__);
        return 0;
    }
    unc_addr = (unsigned long) dma_alloc_coherent(dev, len, &dma_addr, GFP_KERNEL);
    if (unc_addr==NULL) {
        printk("%s dma_alloc_coherent failed\n", __func__);
        return 0;
    }

    cac_addr = (unsigned long) kmalloc(len, GFP_KERNEL);
    if (cac_addr==NULL) {
        printk("%s kmalloc failed\n", __func__);
        dma_free_coherent(dev, len, unc_addr, dma_addr);
        return 0;
    }
    phy_addr = virt_to_phys(cac_addr);

    printk("uncAddr:0x%x, dma:0x%x, cacAddr:0x%x, phy:0x%x\n", unc_addr, dma_addr, cac_addr, phy_addr);
    mboxAPItestDone=0;

    if (create_mbox_notify(CORE0, MFUNC_NOTIFY, 0x400, h_mfunc_notify_cb0)==-1) {
        printk("%s create_mbox_notify failed\n", __func__);
        dma_free_coherent(dev, len, unc_addr, dma_addr);
        kfree(cac_addr);
        return 0;
    }

    for (i=0, addr=unc_addr, val=0x87654321; i<word; i++, addr+=sizel, val--) {
        wtMeml(addr, val);
    }

    memset(&mboxInfo, 0, sizeof(npuMboxInfo_t));
    mboxInfo.core_id=CORE0;
    mboxInfo.func_id=MFUNC_WIFI;
    mboxInfo.virtAddr=unc_addr;
    mboxInfo.physAddr=dma_addr;
    mboxInfo.len=len;
    mboxInfo.flags.isBlockingMode=1;
    mboxInfo.blockTimeout = 1000; /* 100ms */
    mboxInfo.cb=NULL;
    
    if ((res=host_notify_npuMbox(&mboxInfo))==0)
        goto mboxAPI_fail_exit;
    printk("host block wifi0_1 (res:%d)\n", res);

    mboxInfo.virtAddr=0;
    mboxInfo.physAddr=0;
    mboxInfo.len=0;

    if ((res=host_notify_npuMbox(&mboxInfo))==0)
        goto mboxAPI_fail_exit;
    printk("host block wifi0_2 (res:%d)\n", res);

    mboxInfo.virtAddr=unc_addr;
    mboxInfo.physAddr=dma_addr;
    mboxInfo.len=len;
    mboxInfo.blockTimeout = 0; /* default: 30ms*/
    mboxInfo.core_id=CORE1;
    mboxInfo.func_id=MFUNC_WIFI;
    
    if ((res=host_notify_npuMbox(&mboxInfo))==0)
        goto mboxAPI_fail_exit;
    printk("host block 1 (res:%d)\n", res);

    mboxInfo.core_id=CORE2;
    
    if ((res=host_notify_npuMbox(&mboxInfo))==0)
        goto mboxAPI_fail_exit;
    printk("host block 2 (res:%d)\n", res);

    mboxInfo.core_id=CORE3;
    
    if ((res=host_notify_npuMbox(&mboxInfo))==0)
        goto mboxAPI_fail_exit;
    printk("host block 3 (res:%d)\n", res);
    
    for (i=0, addr=cac_addr, val=0x12345678; i<word; i++, addr+=sizel, val++) {
        wtMeml(addr, val);
    }
    ecnt_dcache_wback_inv(cac_addr, len);

    memset(&mboxInfo, 0, sizeof(npuMboxInfo_t));
    mboxInfo.core_id=CORE0;
    mboxInfo.func_id=MFUNC_WIFI;
    mboxInfo.virtAddr=cac_addr;
    mboxInfo.physAddr=phy_addr;
    mboxInfo.len=len;
    mboxInfo.flags.isBlockingMode=0;
    mboxInfo.cb=h_mfunc_wifi_cb0;

    if (host_notify_npuMbox(&mboxInfo)!=1)
        goto mboxAPI_fail_exit;
    printk("host non-block wifi0\n");

    mboxInfo.core_id=CORE1;
    mboxInfo.cb=h_mfunc_wifi_cb1;
    
    if (host_notify_npuMbox(&mboxInfo)!=1)
        goto mboxAPI_fail_exit;
    printk("host non-block 1\n");

    mboxInfo.core_id=CORE2;
    mboxInfo.cb=NULL;
    
    if (host_notify_npuMbox(&mboxInfo)!=1)
        goto mboxAPI_fail_exit;
    printk("host non-block 2\n");

    mboxInfo.core_id=CORE3;
    mboxInfo.cb=h_mfunc_wifi_cb3;
    
    if (host_notify_npuMbox(&mboxInfo)!=1)
        goto mboxAPI_fail_exit;
    printk("host non-block 3\n");
    #if 0
    if (host_get_mboxBuff(CORE0, MFUNC_NOTIFY, &unc_addr2, &phy_addr2, &len2))
        return 0;

    for (i=0, addr=unc_addr2, val=0x76543210; i<(len2/sizel); i++, addr+=sizel, val--) {
        wtMeml(addr, val);
    }

    memset(&mboxInfo, 0, sizeof(npuMboxInfo_t));
    mboxInfo.core_id=CORE0;
    mboxInfo.func_id=MFUNC_NOTIFY;
    mboxInfo.virtAddr=unc_addr2;
    mboxInfo.physAddr=phy_addr2;
    mboxInfo.len=len2;
    mboxInfo.flags.isBlockingMode=0;
    mboxInfo.cb=NULL;
    
    if ((res=host_notify_npuMbox(&mboxInfo))==0)
        goto mboxAPI_fail_exit;
    printk("host notify (res:%d)\n", res);
    #endif

    /* before releasing resource, wait until test is done. */
    while(mboxAPItestDone==0) schedule();
    printk("\n%s Done\n\n", __func__);

mboxAPI_fail_exit:    
    dma_free_coherent(dev, len, unc_addr, dma_addr);
    kfree(cac_addr);
    delete_mbox_notify(CORE0, MFUNC_NOTIFY);
    return 0;
}
#endif

