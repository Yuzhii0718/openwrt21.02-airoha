
#include "cpu_bus_test.h"
#ifdef TCSUPPORT_CPU_ARMV8
#ifndef __L2CMEM
#define __L2CMEM /* only for mips, so define it as nothing to prevent compile error */
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/dma-direct.h>
#endif
#else
#include <asm/addrspace.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/tc3162/kprofile_hook.h>
#endif
#include <linux/random.h>

static unsigned int busUtilInfo[3] = {0};
void busUtiliCntCheck(void);

unsigned char rdBypassWt_end=0;
#ifdef TCSUPPORT_CPU_ARMV8
unsigned long *rdBypassWt_uncached_addr;
#else
uint32 *rdBypassWt_uncached_addr;
#endif

static int doBusUtiliTest(void *);
static int doGdmaRamTest(void *);
static int doCpuDramTest(void *);
static int doCpuRegSramTest(void *);
static int doCpuCacheDramTest(void *);
static void doRdBypassWt_cnt(void);


static struct task_struct *task_bus_utili;
static struct task_struct *task_gdma_ram;
static struct task_struct *task_cpu_dram;
static struct task_struct *task_cpu_reg_sram;
static struct task_struct *task_cpu_cache_dram;
static struct task_struct *task_rdBypassWt_cpu2;
static struct task_struct *task_rdBypassWt_cpu3;
#ifdef TCSUPPORT_CPU_ARMV8
static struct task_struct *task_cpu0_access_Test;
static struct task_struct *task_cpu1_access_Test;
static struct task_struct *task_cpu2_access_Test;
static struct task_struct *task_cpu3_access_Test;
static struct task_struct *task_cpu0_dmaBlkCnt_Test;
static struct task_struct *task_cpu1_triggerDma;
static struct task_struct *task_cpu0_pbus_Test;
static struct task_struct *task_cpu1_pbus_Test;
static struct task_struct *task_cpu2_pbus_Test;
static struct task_struct *task_cpu3_pbus_Test;
#endif

unsigned int data0, data1, data2, data3;
unsigned int isBusTestAll=0;

spinlock_t regSramLock, gdmaRamLock, dramLock, utilLock, cpu0rdBypassLock, cpu1rdBypassLock;

#define RDBYPASSWT_CNT_MASK     0xFFFFFFFF
#define RDBYPASSWT_TEST_SIZE    0x200

#define BYPASS_DRAM_TEST_SIZE   RDBYPASSWT_TEST_SIZE
#define REGIONS_DISTANCE        64
#define REGION_SIZE             (1 << 6)
static unsigned long cpuCountBefore[CYCLES_TEST_NUM];
static unsigned long cpuCountAfter[CYCLES_TEST_NUM];

int is_cpu0_1_bus_test=0;

unsigned long g_dram_unc_addr, g_dram_cac_addr;
int is_cycles_dbgOn = 0;

/* for dram read/write measurement in doCpuCycles()
 * reason: 
 *   - during read_dram test, when dram refresh happens, the measured cpuCycle will be very
 *     large which makes the final result incorrect, so need to skip those large values. 
 * method:
 *   - set try_dramRead=0 when dramReadBase.
 *   - increase try_dramRead each time doing dramRead.
 *   - when try_dramRead==1, calculate the 1st average dramReadCycle
 *   - when try_dramRead > 1, use the above dramReadCycle to skip large values to get more correct dram_read cycles.*/
int try_dramRead=0, is_dramRW=0;


#ifdef TCSUPPORT_CPU_ARMV8
extern int doCpu0AccessTest(void *arg);
extern int doCpu1AccessTest(void *arg);
extern int doCpu2AccessTest(void *arg);
extern int doCpu3AccessTest(void *arg);
extern int doCpu0dmaBlkCntTest(void *arg);
extern int doCpu1triggerDma(void *arg);
extern int doNewPBusTest(void *arg);
extern int isNewPbusTestDone;
extern enum newPbusCase newPbusTestCase;
typedef void (*new_pbus_func_t)(int arg);
extern new_pbus_func_t new_pbus_tests[];
extern void enable_npu_rbus_decoder(int enable);
extern void gdump_mode_sel(u32 val);
extern void __iomem * get_gdmpSram_base(void);
#endif

typedef struct baseValue 
{
    unsigned long readBase;
    unsigned long writeBase;
    unsigned long syncBase;

} baseValue_t;

static baseValue_t perfBaseVal = {0,0,0};


enum baseValType
{
    noBaseT=0,
    readBaseT,
    readBaseDelT,
    writeBaseT,
    writeBaseDelT,
    syncBaseT,
    syncBaseDelT,
    #ifdef TCSUPPORT_CPU_ARMV8
    notApplicableT /* for those cases that can't be measured in ARMv8*/
    #endif
};

enum perfIdxItem
{
    readBaseI=0, /*0*/
    readScuRegI,
    readDramI,
    readGdmpSramI,
    readFeSramI,
    writeBaseI, /*5*/
    writeScuRegI,
    writeDramI,
    syncBaseI,
    D_L2_cacheMissI,
    D_cacheInvI, /*10*/
    D_cacheMissI,
    D_cacheHitI,
    L2_cacheInvI,
    D_cacheWbInvI,
    L2_cacheWbInvI, /*15*/
    GDMA_Dram_Dram_1DwI,
    GDMA_Dram_Dram_16DwI,
    GDMA_Dram_feSram_1DwI,
    GDMA_Dram_feSram_16DwI,
    GDMA_feSram_feSram_1DwI, /*20*/
    GDMA_feSram_feSram_16DwI,
    GDMA_feSram_Dram_1DwI,
    GDMA_feSram_Dram_16DwI,
    #if defined(TCSUPPORT_CPU_ARMV8)
    /* ARM32 barrier cmds, listed in arch/arm/include/asm/barrier.h */
    B_dsb_I,
    B_dsb_st_I,
    B_dmb_osh_I,
    B_dmb_oshst_I,
    #endif
};

typedef struct perfIdxTest 
{
    char* name;
    unsigned char type; /* the type to delete base value */
    unsigned long cntBef[PERF_TEST_NUM];
    unsigned long cntAft[PERF_TEST_NUM];

} perfIdxTest_t;

perfIdxTest_t piTest[] =
{
    {"read base", readBaseT, {0}, {0}}, /*0*/
    {"read SCU REG", readBaseDelT, {0}, {0}},
    {"read DRAM", readBaseDelT, {0}, {0}},
    {"read GDMP SRAM", readBaseDelT, {0}, {0}},
    {"read FE SRAM", readBaseDelT, {0}, {0}},
    {"write base", writeBaseT, {0}, {0}}, /*5*/
    {"write SCU REG", writeBaseDelT, {0}, {0}},
    {"write DRAM", writeBaseDelT, {0}, {0}},
    {"Sync cmd", syncBaseT, {0}, {0}},
    {"D+L2-cache_miss+ram_uncached_32B", readBaseDelT, {0}, {0}},
    #ifdef TCSUPPORT_CPU_ARMV8
    {"D+L2-cache inv", syncBaseDelT, {0}, {0}}, /*10*/
    {"N/A for D-cache miss", notApplicableT, {0}, {0}},
    #else
    {"D-cache inv", syncBaseDelT, {0}, {0}}, /*10*/
    {"D-cache miss", readBaseDelT, {0}, {0}},
    #endif
    {"D-cache hit", readBaseDelT, {0}, {0}},
    #ifdef TCSUPPORT_CPU_ARMV8
    {"N/A for L2-cache inv", notApplicableT, {0}, {0}},
    {"D+L2-cache wback_inv", syncBaseDelT, {0}, {0}},
    {"N/A for L2-cache wback_inv", notApplicableT, {0}, {0}}, /*15*/
    #else
    {"L2-cache inv", syncBaseDelT, {0}, {0}},
    {"D-cache wback_inv", syncBaseDelT, {0}, {0}},
    {"L2-cache wback_inv", syncBaseDelT, {0}, {0}}, /*15*/
    #endif
    {"GDMA dram->dram 64kB_1DW", noBaseT, {0}, {0}},
    {"GDMA dram->dram 64kB_16DW", noBaseT, {0}, {0}},
    {"GDMA dram->feSram 64kB_1DW", noBaseT, {0}, {0}},
    {"GDMA dram->feSram 64kB_16DW", noBaseT, {0}, {0}},
    {"GDMA feSram->feSram 64kB_1DW", noBaseT, {0}, {0}}, /*20*/
    {"GDMA feSram->feSram 64kB_16DW", noBaseT, {0}, {0}},
    {"GDMA feSram->dram 64kB_1DW", noBaseT, {0}, {0}},
    {"GDMA feSram->dram 64kB_16DW", noBaseT, {0}, {0}},
    #if defined(TCSUPPORT_CPU_ARMV8)
    /* ARM32 barrier cmds, listed in arch/arm/include/asm/barrier.h */
    {"dsb() (rmb) cmd", noBaseT, {0}, {0}},
    {"dsb(st) (wmb) cmd", noBaseT, {0}, {0}},
    {"dmb(osh) (dma_rmb) cmd", noBaseT, {0}, {0}},
    {"dmb(oshst) (dma_wmb) cmd", noBaseT, {0}, {0}},
    #endif
    {NULL, noBaseT, {0}, {0}}
};


extern int gdmacopy(uint8 *dram_d_addr,uint8 *dram_s_addr,uint32 *sram_d_addr, uint32 *sram_s_addr, struct physAddr *physAddrP, unsigned long test_cnt);
extern int sramTest(void);
#ifdef TCSUPPORT_CPU_ARMV8
extern struct device* get_gdmpSram_dev(void);
#endif

#ifdef TCSUPPORT_CPU_ARMV8
static void chk_en_rd_bypass_wt(void)
{
    u32 val = ecnt_get_rdbypass_cfg();

    if ((val&0x1)==0){ /* if not enabled, set default value and enable */
        ecnt_enable_rd_bypass_wt(1, 0xffffffc0, 0x8, 0x4);
    }
    //printk("rdbypass_cfg0:0x%x\n", ecnt_get_rdbypass_cfg());

    return;
}
#endif

int bustest_kthread_process(int argc, char *argv[])
{
  if(argc == 2) 
  {
    if (strncmp(argv[1], "bustest_all", strlen(argv[1])) == 0) {
        isBusTestAll=1;
        data3 = 0x1fffff;
        wake_up_process(task_cpu_reg_sram);
        data1 = 0x1f;
        wake_up_process(task_gdma_ram);
        data2 = 0x7fff;
        wake_up_process(task_cpu_dram);
        data0 = 0x1;
        wake_up_process(task_bus_utili);
        return 0;
    }
  }
  if(argc == 3) 
  {
    if(strncmp(argv[1], "bustest_gdma_ram", strlen(argv[1])) == 0) {
        data1 = simple_strtoul(argv[2], NULL, 16);
        if (data1)
            wake_up_process(task_gdma_ram);
        else
            kthread_stop(task_gdma_ram);
        return 0;
    }
    else if (strncmp(argv[1], "bustest_cpu_dram", strlen(argv[1])) == 0) {
        data2 = simple_strtoul(argv[2], NULL, 16);
        if (data2)
            wake_up_process(task_cpu_dram);
        else
            kthread_stop(task_cpu_dram);
        return 0;
    }
    else if (strncmp(argv[1], "bustest_cpu_reg_sram", strlen(argv[1])) == 0) {
        data3 = simple_strtoul(argv[2], NULL, 16);
        if (data3)
            wake_up_process(task_cpu_reg_sram);
        else
            kthread_stop(task_cpu_reg_sram);
        return 0;
    }
    else if (strncmp(argv[1], "bustest_cpu_cache_dram", strlen(argv[1])) == 0) {
        data1 = simple_strtoul(argv[2], NULL, 16);
        if (data1)
            wake_up_process(task_cpu_cache_dram);
        else
            kthread_stop(task_cpu_cache_dram);
        return 0;
    }
    else if (strncmp(argv[1], "bus_utili_test", strlen(argv[1])) == 0) {
        data0 = simple_strtoul(argv[2], NULL, 16);
        if(data0)
            wake_up_process(task_bus_utili);
        else
            kthread_stop(task_bus_utili);
        return 0;
    }
  }

#ifdef TCSUPPORT_CPU_ARMV8
    if ((argc == 4) || (argc == 5) || (argc == 6)) {
      if ((strncmp(argv[1], "cpu0_1_bus_test", strlen(argv[1]))==0) ||
           (strncmp(argv[1], "new_pbus_test", strlen(argv[1]))==0)) {
        data0 = simple_strtoul(argv[2], NULL, 16);
        data1 = simple_strtoul(argv[3], NULL, 16);
        data2 = 0;
        data3 = 0;
		/*this parameter is used for enable rbus dynamic clk ctl. */
		if (argc >= 5)
			data2 = simple_strtoul(argv[4], NULL, 16);
        if (argc == 6)
			data3 = simple_strtoul(argv[5], NULL, 16);
        is_cpu0_1_bus_test=1;
        SET_RCU_CPU_STALL_SUPRESS();
        enable_npu_rbus_decoder(1);
        chk_en_rd_bypass_wt();
        if (strncmp(argv[1], "new_pbus_test", strlen(argv[1]))==0) {
            isNewPbusTestDone=0;
            newPbusTestCase=data0;
            if ((data0==3)||(data0==4)||(data0==5)||(data0==6)) {
                /* only need one cpu to do the test */
                new_pbus_tests[data0](data1);
            }
            else {
                wake_up_process(task_cpu0_pbus_Test);
                wake_up_process(task_cpu1_pbus_Test);
                if (CONFIG_NR_CPUS==4) {
                    wake_up_process(task_cpu2_pbus_Test);
                    wake_up_process(task_cpu3_pbus_Test);
                }
            }
        }
        else {
            if (data0)
                wake_up_process(task_cpu0_access_Test);
            if (data1)
                wake_up_process(task_cpu1_access_Test);
            if (CONFIG_NR_CPUS==4) {
                if (data2)
                    wake_up_process(task_cpu2_access_Test);
                if (data3)
                    wake_up_process(task_cpu3_access_Test);
            }
        }
        return 0;
      }  
      else if ((argc == 4) && (strncmp(argv[1], "dmaBlkCnt_test", strlen(argv[1])) == 0)) {
          data0 = simple_strtoul(argv[2], NULL, 10);
          data1 = simple_strtoul(argv[3], NULL, 10);
          wake_up_process(task_cpu0_dmaBlkCnt_Test);
          wake_up_process(task_cpu1_triggerDma);
          return 0;
      }
    }
#endif

  if ((argc == 5) && (strncmp(argv[1], "rdBypassWt_cnt", strlen(argv[1])) == 0)) {
      data2 = simple_strtoul(argv[2], NULL, 10);
      data0 = simple_strtoul(argv[3], NULL, 10);
      data1 = simple_strtoul(argv[4], NULL, 10);
      doRdBypassWt_cnt();
      return 0;
  }

  return 1;
}

void busTest_locking(spinlock_t *lock, unsigned long *flags)
{    
    if(isBusTestAll || is_cpu0_1_bus_test) {
        spin_lock_init(lock);
        spin_lock_irqsave(lock, (*flags)) ;
    }
    return;
}

void busTest_unlocking(spinlock_t *lock, unsigned long *flags)
{    
    if(isBusTestAll || is_cpu0_1_bus_test)
        spin_unlock_irqrestore(lock, (*flags)) ;

    return;
}

int gdmacopy_counting(unsigned long cnt)
{
    if (is_cpu0_1_bus_test) /*do nothing*/
        return 1;

    if ((cnt & data1) == data1) {
        if ((cnt & (data1<<LOG_BIT_SHIFT | LOG_BIT_MASK)) == (data1<<LOG_BIT_SHIFT | LOG_BIT_MASK))
            printk("\nGDMA test round:0x%lx at CPU%d\n", cnt, smp_processor_id());
        if(isBusTestAll==0)
            msleep(1);
    }
    if ((isBusTestAll==0) && kthread_should_stop()) {
        printk("\n%s is stopped\n", __func__);
        return 0;
    }

	return 1;
}

void set_share_unzipmem_sel(uint32 val)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    gdump_mode_sel(val);
    #else
    VPint(0xbfb00954) = val;
    #endif
    return;
}

int doGdmaTest2(void)
{
    uint8 *dram_s_addr = NULL;
    uint8 *dram_d_addr = NULL;
    dma_addr_t dram_s_phy, dram_d_phy;
    uint32 *sram_s_addr, *sram_d_addr;
    unsigned long test_cnt = ALL_BITS_ONE;
    unsigned long flags;
    struct physAddr *physAddrP=NULL;
    unsigned long sram_base=SRAM_GDMP_BASE;
    struct device *dev=NULL;
    

    #ifdef TCSUPPORT_CPU_ARMV8
    if ((dev=get_gdmpSram_dev())==NULL) {
        printk("\nget_gdmpSram_dev failed\n");
		return -ENOMEM;
    }

    physAddrP = (struct physAddr *)kzalloc(sizeof(struct physAddr), GFP_KERNEL);
    if ((physAddrP == NULL)) {
        printk("\nphysAddrP is NULL\n");
        goto doGdmaTest2_exit;
    }
    
    sram_base = (unsigned long)get_gdmpSram_base();
    #endif
    
    sram_s_addr = (uint32 *)(sram_base+SRAM_S_ADDR_OFF);
    sram_d_addr = (uint32 *)(sram_base+SRAM_D_ADDR_OFF);
    
    dram_s_addr = (uint8 *)dma_alloc_coherent(dev, (TC_GDMA_TEST_SIZE+32), &dram_s_phy, GFP_KERNEL);
    dram_d_addr = (uint8 *)dma_alloc_coherent(dev, (TC_GDMA_TEST_SIZE+32)*GDMA_CH_NUM, &dram_d_phy, GFP_KERNEL);
    if ((dram_s_addr == NULL) || (dram_d_addr == NULL)) {
        printk("\nError:dram_s_addr or dram_d_addr is NULL\n");
        return 0;
    }

    #ifdef TCSUPPORT_CPU_ARMV8
    physAddrP->dram_s_phys=dram_phyAddr_to_dmaAddr((u32)dram_s_phy);
    physAddrP->dram_d_phys=dram_phyAddr_to_dmaAddr((u32)dram_d_phy);
    physAddrP->sram_s_phys=(SRAM_GDMP_PHY_BASE+SRAM_S_ADDR_OFF);
    physAddrP->sram_d_phys=(SRAM_GDMP_PHY_BASE+SRAM_D_ADDR_OFF);

    #if 1
    printk("sram_s_addr:0x%lx  sram_s_phys:0x%x\n", (unsigned long)sram_s_addr, physAddrP->sram_s_phys);
    printk("sram_d_addr:0x%lx  sram_d_phys:0x%x\n", (unsigned long)sram_d_addr, physAddrP->sram_d_phys);
    printk("dram_s_addr:0x%lx  dram_s_phys:0x%x\n", (unsigned long)dram_s_addr, physAddrP->dram_s_phys);
    printk("dram_d_addr:0x%lx  dram_d_phys:0x%x\n", (unsigned long)dram_d_addr, physAddrP->dram_d_phys);    
    #endif
    #endif
    
    /* set GDMP sram to be accessed via PBUS (by CPU or GDMA) */
    set_share_unzipmem_sel(0);

    printk("\ndoGdmaTest start at CPU%d\n", smp_processor_id());

    busTest_locking(&gdmaRamLock, &flags);

    gdmacopy(dram_d_addr, dram_s_addr,sram_d_addr, sram_s_addr, physAddrP, test_cnt);

    busTest_unlocking(&gdmaRamLock, &flags);

doGdmaTest2_exit:
    dma_free_coherent(dev, (TC_GDMA_TEST_SIZE+32), dram_s_addr, dram_s_phy);
    dma_free_coherent(dev, (TC_GDMA_TEST_SIZE+32)*GDMA_CH_NUM, dram_d_addr, dram_d_phy);
    kfree(physAddrP);

    printk("\ndoGdmaTest end\n");

    return 0;
}

int doGdmaRamTest(void *arg)
{
    doGdmaTest2();
    return 0;
}

int doCpuDramTest(void *arg) /* for CPU accessing DRAM */
{
    unsigned long *dram_s_addr, *uncached_s_addr=NULL;
    unsigned long testVal;
    uint32 i, j, cnt=0;
    dma_addr_t phy_srcAddr;
    unsigned long flags;
    struct device *dev=NULL;
    uint32 test_len = (BUS_DRAM_TEST_SIZE/sizeof(unsigned long));

    #ifdef TCSUPPORT_CPU_ARMV8  
    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("get_gdmpSram_dev is NULL\n");
        return 0;
    }
    #endif

    uncached_s_addr = (unsigned long *) dma_alloc_coherent(dev, BUS_DRAM_TEST_SIZE, &phy_srcAddr, GFP_KERNEL);
    if (((unsigned long)uncached_s_addr) % 32 != 0) {
        printk("\nError: uncached_s_addr:0x%p is not 32-byte alignment, finish testing!\n", uncached_s_addr);
        goto end_cpuDram_test;
    }

    printk("\n[H%d]doCpuDramTest start (isBusTestAll:%d)\n", smp_processor_id(), isBusTestAll);

    busTest_locking(&dramLock, &flags);
    
    while (1)
    {
        /* reset for the test */
        testVal=cnt;
        dram_s_addr = uncached_s_addr;
    
        /* CPU write BUS_DRAM_TEST_SIZE bytes to DRAM */
        for (i=0; i<test_len; i++, dram_s_addr++, testVal++) {
            if (testVal==(unsigned long)ALL_32BIT_ONE) testVal=0;
            wtMeml(dram_s_addr,testVal);
        }


        /* reset for next test */
        testVal=cnt;
        dram_s_addr = uncached_s_addr;

        /* CPU read BUS_DRAM_TEST_SIZE bytes from DRAM and compare */
        for (i=0; i<test_len; i++, dram_s_addr++, testVal++) {
            if (testVal==(unsigned long)ALL_32BIT_ONE) testVal=0;
            if ((rdMeml(dram_s_addr)) != testVal) {
                printk("\nERROR1 (*dram_s_addr):0x%lx != testVal:0x%lx at word:%d at dram_s_addr:0x%p\n", 
                                    rdMeml(dram_s_addr), testVal, i, dram_s_addr);
                goto end_cpuDram_test;
            }
        }


        /* reset for next test */
        testVal=cnt+1;
        dram_s_addr = uncached_s_addr;

        /* CPU write a byte then read it back and compare */
        for (i=0; i<test_len; i++, dram_s_addr++, testVal++) {
            if (testVal==(unsigned long)ALL_32BIT_ONE) testVal=0;
            wtMeml(dram_s_addr,testVal);
            if ((rdMeml(dram_s_addr)) != testVal) {
                printk("\nERROR2 (*dram_s_addr):0x%lx != testVal:0x%lx at word:%d at dram_s_addr:0x%p\n", 
                                    rdMeml(dram_s_addr), testVal, i, dram_s_addr);
                goto end_cpuDram_test;
            }
        }

        cnt++;
        if ((cnt & data2) == data2) {
            if ((cnt & (data2<<LOG_BIT_SHIFT | LOG_BIT_MASK)) == (data2<<LOG_BIT_SHIFT | LOG_BIT_MASK))
                printk("\n[H%d]CpuDram test round:0x%x\n", smp_processor_id(), cnt);
            if(isBusTestAll==0)
                msleep(1);
        }
        if (cnt == ALL_32BIT_ONE) cnt=0;

        if ((isBusTestAll==0) && kthread_should_stop()) {
            printk("\n%s is stopped\n", __func__);
            goto end_cpuDram_test;
        }
    }

end_cpuDram_test:
    busTest_unlocking(&dramLock, &flags);
    dma_free_coherent(dev, BUS_DRAM_TEST_SIZE, uncached_s_addr, phy_srcAddr);
    printk("\ndoCpuDramTest end\n");


    return 0;
}

int doCpuCacheDramTest(void *arg) /* for CPU cached accessing DRAM */
{
    unsigned long *dram_s_addr, *cached_s_addr;
    unsigned long testVal;
    unsigned long i, j, cnt=0;
    unsigned long testLen = (BUS_CACHE_DRAM_TEST_SIZE/(sizeof(unsigned long)));
    

    cached_s_addr = (unsigned long *) kmalloc(BUS_CACHE_DRAM_TEST_SIZE, GFP_KERNEL);
    if (cached_s_addr==NULL) {
        printk("\nError(%s) kmalloc fail\n",__func__);
        return 0;
    }

    printk("\n[H%d]doCpuCacheDramTest start (cac_addr_base:0x%lx)\n", smp_processor_id(), cached_s_addr);
    
    while (1)
    {
        /* reset for the test */
        testVal=cnt;
        dram_s_addr = cached_s_addr;
    
        /* CPU write BUS_CACHE_DRAM_TEST_SIZE bytes to DRAM */
        for (i=0; i<testLen; i++, dram_s_addr++, testVal++) {
            if (testVal==ALL_BITS_ONE) testVal=0;
            wtMeml(dram_s_addr,testVal);
        }

        /* reset for next test */
        testVal=cnt;
        dram_s_addr = cached_s_addr;

        /* CPU read BUS_CACHE_DRAM_TEST_SIZE bytes from DRAM and compare */
        for (i=0; i<testLen; i++, dram_s_addr++, testVal++) {
            if (testVal==ALL_BITS_ONE) testVal=0;
            if ((rdMeml(dram_s_addr)) != testVal) {
                printk("\nERROR1 rdMeml(dram_s_addr:0x%lx != testVal:0x%lx at word:%d at dram_s_addr:0x%lx\n", 
                                    rdMeml(dram_s_addr), testVal, i, dram_s_addr);
                goto end_cpuCacheDram_test;
            }
        }
    
        cnt++;
        if ((cnt & data1) == data1) {
            if ((cnt & (data1<<LOG_BIT_SHIFT | LOG_BIT_MASK)) == (data1<<LOG_BIT_SHIFT | LOG_BIT_MASK))
                printk("\n[H%d]CpuCacheDram test round:0x%x\n", smp_processor_id(), cnt);
            msleep(1);
        }
        if (cnt == ALL_BITS_ONE) cnt=0;

        if (kthread_should_stop()) {
            printk("\n%s is stopped\n", __func__);
            goto end_cpuCacheDram_test;
        }
    }

end_cpuCacheDram_test:
    kfree(cached_s_addr);
    printk("\ndoCpuCacheDramTest end\n");


    return 0;
}

int doCpuRegSramTest(void *arg) /* for CPU accessing registers & Sram */
{
    uint32 cnt=0;
    int i;
    /* GDMP SRAM uncache addr: 0xbfa40000~bfa47fff */
    uint32 *uncache_addr = (uint32 *)SRAM_TEST_START;
    uint32 value = 0x12345678;
    uint32 tmpVal;
    unsigned long flags;
    
    printk("\ndoCpuRegSramTest start\n");
    #ifdef TCSUPPORT_CPU_ARMV8
    gdump_mode_sel(0); /*for cpu/gdma access gdump_sram via pbus */
    #endif
    busTest_locking(&regSramLock, &flags);

    while (1)
    {
        #ifdef TCSUPPORT_CPU_ARMV8
        if (RW_SCREG_REG_TEST(cnt))
            return 0;

        if (RW_GDMP_SRAM_TEST(SRAM_TEST_START, SRAM_TEST_END, cnt)) {
            return 0;
        }   
        
        #else /* TCSUPPORT_CPU_ARMV8 */
        
        VPint(SCREG_WR0) = cnt;
        VPint(SCREG_WR1) = cnt;

        if ((VPint(SCREG_WR0) != cnt) || (VPint(SCREG_WR1) != cnt)) {
            printk("\nERROR1 VPint(SCREG_WR0):%x,  VPint(SCREG_WR1):%x, at byte:%x\n", VPint(SCREG_WR0), VPint(SCREG_WR1), cnt);
            return 0;
        }

        tmpVal = value;
        for (i=0; i<SRAM_TEST_WORDS; i++, tmpVal++) {
            uncache_addr[i] = tmpVal;
        }

        tmpVal = value;
        for (i=0; i<SRAM_TEST_WORDS; i++, tmpVal++) {
            if (uncache_addr[i] != tmpVal) {
                printk("\nERROR2 uncache_addr[%d]:0x%08x != tmpVal:0x%08x at uncache_addr:0x%p at Round:0x%08x\n", 
                                            i, uncache_addr[i], tmpVal, uncache_addr, cnt);
                return 0;
            }
        }

        value += SRAM_TEST_WORDS;
        uncache_addr += SRAM_TEST_WORDS;
        if (value > 0xf0000000)
           value = 0x12345678;
        if (((uint32)uncache_addr) > (SRAM_TEST_END-SRAM_TEST_SIZE))
           uncache_addr = (uint32 *)SRAM_TEST_START;
        
        #endif /* TCSUPPORT_CPU_ARMV8 */

        cnt++;
        if ((cnt & data3) == data3) {
            if ((cnt & (data3<<LOG_BIT_SHIFT | LOG_BIT_MASK)) == (data3<<LOG_BIT_SHIFT | LOG_BIT_MASK))
                printk("\nCpuRegSram test round:0x%x at CPU%d\n", cnt, smp_processor_id());
            if(isBusTestAll==0)
            msleep(1);
        }
        if (cnt == 0xffffffff) cnt=0;

        if ((isBusTestAll==0) && kthread_should_stop()) {
            printk("\n%s is stopped\n", __func__);
            return 0;
        }
    }

    busTest_unlocking(&regSramLock, &flags);

    return 0;
}

void enable_bus_util(void)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    ENABLE_BUS_UTIL();
    #else
    regWrite32(BUS_UTI_CTL_REG, 0x1);
    #endif
    return;
}

void disable_bus_util(void)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    DISABLE_BUS_UTIL();
    #else
    regWrite32(BUS_UTI_CTL_REG, 0x2);
    #endif
    return;
}

int is_bus_util_done(void)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    return IS_BUS_UTIL_DONE();
    #else
    return (regRead32(BUS_UTI_STS_REG)&0x1);
    #endif
}

void get_bus_util_cnt(uint32 *validCnt, uint32 *idleCnt)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    GET_BUS_UTIL_CNT(validCnt, idleCnt);
    #else
    *validCnt = regRead32(BUS_UTI_VLD_CNT);
    *idleCnt = regRead32(BUS_UTI_IDEL_CNT);
    #endif
    return;
}

/* busUtilInfo[0]: 1 means busUtiliCnt is enabled already 
 * busUtilInfo[1]: used to store total utilization percentage
 * busUtilInfo[1]: used to store total utilization counts */
void busUtiliCntCheck(void)
{
    unsigned int validCnt, idleCnt;

    if (busUtilInfo[0]==0) {
        busUtilInfo[0]=1;
        enable_bus_util();
        printk("\n%s start at CPU%d\n", __func__, smp_processor_id());
    }

    if (is_bus_util_done()) { /* when done bit is raised, enable bit is cleared */
    
        get_bus_util_cnt(&validCnt, &idleCnt);
        
        validCnt = validCnt/UTIL_CNT_UNIT;
        idleCnt = idleCnt/UTIL_CNT_UNIT;
        
        if (validCnt==0 || idleCnt==0) { /* if value is too small, don't get into account */
            get_bus_util_cnt(&validCnt, &idleCnt);
            printk("\nWarning: validCnt:0x%x or idleCnt:0x%x is too small\n", 
                        validCnt, idleCnt);
            enable_bus_util();
            return;
        }

        busUtilInfo[1] += ((validCnt*100)/(validCnt+idleCnt));
        busUtilInfo[2]++;

        if (busUtilInfo[2] == BUS_UTIL_LOG_CNT) {
            
            if (is_cpu0_1_bus_test) { /* reduce log */
                printk("[H%d(U%d%%)]\n", smp_processor_id(), (int)(busUtilInfo[1]/BUS_UTIL_LOG_CNT));
            }
            else {
                printk("\nAverage Bus Utilization: %d%%  at cpu%d\n", 
                    (int)(busUtilInfo[1]/BUS_UTIL_LOG_CNT), smp_processor_id());
            }
            busUtilInfo[1]=0;
            busUtilInfo[2]=0;
        }

        enable_bus_util();
    }
    
    return;
}

int doBusUtiliTest(void *arg) /* bus utilization tool */
{
    unsigned int utilization=0, roundCnt=0;
    unsigned int validCnt, idleCnt, totalCnt;
    unsigned long flags;    
    
    busTest_locking(&utilLock, &flags);

    while (1) {

        busUtiliCntCheck();

        if ((isBusTestAll==0) && kthread_should_stop()) {
            printk("\n%s is stopped\n", __func__);
            return 0;
        }

        if(isBusTestAll)
            mdelay(10);
        else
            msleep(10);
    }

    busTest_unlocking(&utilLock, &flags);

    return 0;
}

#define MSLEEP_1SEC   1000

int doBusUtilCntTest(int argc, char *argv[], void *p)
{
    int time = simple_strtoul(argv[1], NULL, 10);
    unsigned int validCnt, idleCnt;
    int timeout;

    printk("\n%s (time:%d secs)\n", __func__, time);
    get_bus_util_cnt(&validCnt, &idleCnt);
    printk("Breofe_Test: validCnt:0x%x, idleCnt:0x%x\n", validCnt, idleCnt);

    /* set start bit */
    enable_bus_util();
    msleep(100);


    if (time==0) { /* counter stops when it's full */

        timeout=150;
        while (!is_bus_util_done()) { /*done bit*/
            get_bus_util_cnt(&validCnt, &idleCnt);
            printk("validCnt:0x%x, idleCnt:0x%x\n", validCnt, idleCnt);
            msleep(MSLEEP_1SEC);
            if ((timeout--)<0) {
                printk("Error: bus_util done bit timeout\n");
                return 0;
            }
        }

        get_bus_util_cnt(&validCnt, &idleCnt);
        printk("CNT_FULL_STOP: validCnt:0x%x, idleCnt:0x%x\n", validCnt, idleCnt);
        msleep(MSLEEP_1SEC);
        get_bus_util_cnt(&validCnt, &idleCnt);
        printk("CNT_FULL_STOP after 1 sec: validCnt:0x%x, idleCnt:0x%x\n", validCnt, idleCnt);
    }
    else if (time>0) {

        while ((time--)>0) {
            get_bus_util_cnt(&validCnt, &idleCnt);
            printk("validCnt:0x%x, idleCnt:0x%x\n", validCnt, idleCnt);
            msleep(MSLEEP_1SEC);
        }

        /* set end bit */
        disable_bus_util();
        msleep(MSLEEP_1SEC);

        if (is_bus_util_done()) { /* done bit */
            get_bus_util_cnt(&validCnt, &idleCnt);
            printk("CNT_END: validCnt:0x%x, idleCnt:0x%x\n", validCnt, idleCnt);
            msleep(MSLEEP_1SEC);
            get_bus_util_cnt(&validCnt, &idleCnt);
            printk("CNT_END aftere 1 sec: validCnt:0x%x, idleCnt:0x%x\n", validCnt, idleCnt);            
        }
    }
    else {
        printk("\nERROR: time:%d < 0\n", time); 
    }
    
    return 0;
}

#ifdef TCSUPPORT_KPROFILE
int doCpuProfiling(int argc, char *argv[], void *p)
{
    uint32 aaa, i, j, isWrongType=0;
    uint32 *cached_addr, *uncached_addr;
    uint32 type = simple_strtoul(argv[1], NULL, 10);
    uint32 value = 0x12345678;
    uint32 cycle1, cycle2;


    cached_addr = (uint32*)kzalloc(PROFILING_DRAM_TEST_SIZE, GFP_KERNEL);
    if (((uint32)cached_addr) % 32 != 0) {
        printk("\nError: cached_addr:0x%p is not 32-byte alignment, finish testing!\n", cached_addr);
        return 0;
    }

    uncached_addr = (uint32 *)((uint32)cached_addr | 0x20000000);

    local_irq_disable();
    aaa = 0; /* load value to aaa's cache */
    kprofileStart();

    if (type == 0) { /* profiling base value */
        for (i=0; i<PROFILING_TEST_NUM; i++) {
            kprofileLog(0, 0);
            kprofileLog(1, 0);
        }
        printk("doing CPU profiling for base value\n\n");
    }
    else if (type == 1) { /* cached memory read */
        for (i=0; i<PROFILING_TEST_NUM; i++) {
            #ifndef TCSUPPORT_CPU_ARMV8
            dma_cache_inv(cached_addr, PROFILING_DRAM_TEST_SIZE);
            #endif
            kprofileLog(0, 0);
            aaa = rdMeml(cached_addr);
            kprofileLog(1, 0);
        }
        printk("doing CPU profiling for cached memory read\n\n");
    }
    else if (type == 2) { /* uncached memory read */
        for (i=0; i<PROFILING_TEST_NUM; i++) {
            kprofileLog(0, 0);
            aaa = rdMeml(uncached_addr);
            kprofileLog(1, 0);
        }
        printk("doing CPU profiling for uncached memory read\n\n");
    }
    else {
        isWrongType=1;
        printk("\nWrong CPU profiling type:%d\n", type);
        printk("0:base value, 1:memory read, 2:uncached read\n");
    }

    kprofileStop();
    local_irq_enable();

    kfree(cached_addr);

    if (isWrongType==0)
        kprofileDump();

    return 0;
}
#endif

int doBusData(int argc, char *argv[], void *p)
{
	if(argc != 3 || (strcmp(argv[1], "data0") && strcmp(argv[1], "data1") &&
                     strcmp(argv[1], "data2") && strcmp(argv[1], "data3"))){
		printk("Usage: sys busData [data0||data1||data2||data3] hex-value\r\n");
		return 0;
	}

    if (strcmp(argv[1], "data0") == 0) {
        data0 = simple_strtoul(argv[2], NULL, 16);
        printk("\ndata0==0x%08x\n", data0);
    }
    else if (strcmp(argv[1], "data1") == 0) {
        data1 = simple_strtoul(argv[2], NULL, 16);
        printk("\ndata1==0x%08x\n", data1);
    }
    else if (strcmp(argv[1], "data2") == 0) {
        data2 = simple_strtoul(argv[2], NULL, 16);
        printk("\ndata2==0x%08x\n", data2);
    }
    else if (strcmp(argv[1], "data3") == 0) {
        data3 = simple_strtoul(argv[2], NULL, 16);
        printk("\ndata3==0x%08x\n", data3);
    }
    else
        printk("\ndoBusData fail\n");

    return 0;
}

__L2CMEM void gdma_data_moving_conf (uint32 phy_srcAddr, uint32 phy_dstAddr, uint32 i, uint32 is16DW, uint32 item) 
{
    uint32 _cnt1, _cnt2;
    
    #ifdef TCSUPPORT_CPU_ARMV8
    uint32 ch=0, ct0, ct1=0x4;

    if (is16DW)
        ct0 = 0xffff0027; /*size:64KB, burst:16DW, DoneIntEn, channelEn*/
    else
        ct0 = 0xffff0007; /*size:64KB, burst:16DW, DoneIntEn, channelEn*/
    
    SET_GDMA_CONFIG(ch, phy_srcAddr, phy_dstAddr, ct0, ct1);
    wmb();

    _cnt1 = read_c0_count();
    WAIT_GDMA_DONE(ch); /*wait until done*/
    _cnt2 = read_c0_count();

    CLEAR_GDMA_DONE(ch);

    #else
    VPint(GDMA_CH0_SRC)=phy_srcAddr;
    VPint(GDMA_CH0_DST)=phy_dstAddr;
    if (is16DW)
        VPint(GDMA_CH0_CTL0)=0xffff0027;/*size:64KB, burst:16DW, DoneIntEn, channelEn*/
    else
        VPint(GDMA_CH0_CTL0)=0xffff0007;/*size:64KB, burst:1DW, DoneIntEn, channelEn*/
    
    _cnt1 = read_c0_count();
    while(VPint(GDMA_DONE_INT)==0); /*wait until done*/
    _cnt2 = read_c0_count();
    
    VPint(GDMA_DONE_INT)=ALL_FF;    /*clear all bits*/
    #endif
    
    piTest[item].cntBef[i] = _cnt1;
    piTest[item].cntAft[i] = _cnt2;

    return;
}

unsigned long cpu_cycles_calculate
    (unsigned long* cnt1_p, unsigned long* cnt2_p, unsigned long testRound)
{
    unsigned long totalCpuCounts=0, diff, skipCnt=0, aveCpuCounts;
    int i;
    static unsigned long dramReadCycle=0x0fffffff;

    for (i=0; i<testRound; i++) {

        if (i==0) /* the 1st result may be too large due to icache miss, so skip it */
            continue;
        
        if (cnt2_p[i] > cnt1_p[i])
            diff = (cnt2_p[i] - cnt1_p[i]);
        else
            diff = (cnt1_p[i] - cnt2_p[i]);

        if ((is_dramRW==1)&&(try_dramRead>1)) {
            /* during read_dram test, when dram refresh happens, the measured cpuCycle will be very large
             * which makes the final aveCpuCounts incorrect, so skip it! */
            if (diff>(dramReadCycle+10)) {
                skipCnt++;
                if (is_cycles_dbgOn)        
                    printk("\n[SKIP] cnt1:0x%lx  cnt2:0x%lx  diff:0x%lx\n", cnt1_p[i], cnt2_p[i], diff);
                continue;
            }
        }
        
        totalCpuCounts+=diff;

        if (is_cycles_dbgOn)        
            printk("\ncnt1:0x%lx  cnt2:0x%lx  diff:0x%lx\n", cnt1_p[i], cnt2_p[i], diff);
    }

    testRound--; /* due to 1st result being skipped */
    if ((is_dramRW==1)&&(try_dramRead>1)) 
        testRound -= skipCnt;

    #ifdef TCSUPPORT_CPU_ARMV8
    aveCpuCounts = (totalCpuCounts/testRound);
    #else
    aveCpuCounts = ((totalCpuCounts/testRound)<<1);
    #endif

    if ((is_dramRW==1)&&(try_dramRead==1)) /* calculate dramReadCycle for later use */
        dramReadCycle = aveCpuCounts;

    return aveCpuCounts;
}

void perfIndex_result_print(void)
{
    unsigned long result;
    int item;


    for (item=0; piTest[item].name!=NULL; item++) {

        #ifdef TCSUPPORT_CPU_ARMV8
        if (piTest[item].type == notApplicableT) {
            printk("\n%s\n", piTest[item].name);
            continue;
        }
        #endif
        
        result = cpu_cycles_calculate(piTest[item].cntBef, piTest[item].cntAft, PERF_TEST_NUM);

        /* adjust the result */
        switch(piTest[item].type)
        {
        case readBaseT:
            perfBaseVal.readBase = result;
            break;
        case readBaseDelT:
            result -= perfBaseVal.readBase;
            break;
        case writeBaseT:
            perfBaseVal.writeBase = result;
            break;
        case writeBaseDelT:
            result -= perfBaseVal.writeBase;
            break;
        case syncBaseT:
            perfBaseVal.syncBase = result;
            break;
        case syncBaseDelT:
            result -= perfBaseVal.syncBase;
            break;
        default:
            break;
        }
        
        printk("\nmeasure CPU cycles for %s\n", piTest[item].name);
        printk("Average CPU cycles: %d\n", result);
    }

    return;
}

__L2CMEM void measure_read_base(void) 
{
    unsigned long _cnt1, _cnt2;
    int i;

    for (i=0; i<PERF_TEST_NUM; i++) {
        
        _measure_read_base_by_addr(_cnt1,_cnt2,SCREG_WR0);
        piTest[readBaseI].cntBef[i] = _cnt1;
        piTest[readBaseI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void measure_read_scuReg(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = ioremap(SCREG_WR0, 8);
    #else
    _addr = SCREG_WR0;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {
    
        _measure_read_by_addr(_cnt1,_cnt2,_addr);
        piTest[readScuRegI].cntBef[i] = _cnt1;
        piTest[readScuRegI].cntAft[i] = _cnt2;
    }

    #ifdef TCSUPPORT_CPU_ARMV8
    iounmap(_addr);
    #endif
    return;
}

__L2CMEM void measure_read_dram(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = g_dram_unc_addr;
    #else
    _addr = DRAM_ADDR;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {

        _measure_read_by_addr(_cnt1,_cnt2,_addr);
        piTest[readDramI].cntBef[i] = _cnt1;
        piTest[readDramI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void measure_read_gdmpSram(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = ioremap(GDMP_SRAM_ADDR, 8);
    #else
    _addr = GDMP_SRAM_ADDR;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {

        _measure_read_by_addr(_cnt1,_cnt2,_addr);
        piTest[readGdmpSramI].cntBef[i] = _cnt1;
        piTest[readGdmpSramI].cntAft[i] = _cnt2;
    }

    #ifdef TCSUPPORT_CPU_ARMV8
    iounmap(_addr);
    #endif
    return;
}

__L2CMEM void measure_read_feSram(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = ioremap(FE_SRAM_ADDR, 8);
    #else
    _addr = FE_SRAM_ADDR;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {

        _measure_read_by_addr(_cnt1,_cnt2,_addr);
        piTest[readFeSramI].cntBef[i] = _cnt1;
        piTest[readFeSramI].cntAft[i] = _cnt2;
    }

    #ifdef TCSUPPORT_CPU_ARMV8
    iounmap(_addr);
    #endif
    return;
}

__L2CMEM void measure_write_base(void) 
{
    unsigned long _cnt1, _cnt2;
    int i;

    for (i=0; i<PERF_TEST_NUM; i++) {

        _measure_write_base_by_addr(_cnt1,_cnt2,SCREG_WR0);
        piTest[writeBaseI].cntBef[i] = _cnt1;
        piTest[writeBaseI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void measure_write_scuReg(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = ioremap(SCREG_WR0, 8);
    #else
    _addr = SCREG_WR0;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {

        _measure_write_by_addr(_cnt1,_cnt2,_addr);
        piTest[writeScuRegI].cntBef[i] = _cnt1;
        piTest[writeScuRegI].cntAft[i] = _cnt2;
    }

    #ifdef TCSUPPORT_CPU_ARMV8
    iounmap(_addr);
    #endif
    return;
}

__L2CMEM void measure_write_dram(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = g_dram_unc_addr;
    #else
    _addr = DRAM_ADDR;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {

        _measure_write_by_addr(_cnt1,_cnt2,_addr);
        piTest[writeDramI].cntBef[i] = _cnt1;
        piTest[writeDramI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void measure_sync(void) 
{
    unsigned long _cnt1, _cnt2;
    int i;
 
    for (i=0; i<PERF_TEST_NUM; i++) {
        
        _measure_sync(_cnt1,_cnt2);
        piTest[syncBaseI].cntBef[i] = _cnt1;
        piTest[syncBaseI].cntAft[i] = _cnt2;
    }
    return;
}

#if defined(TCSUPPORT_CPU_ARMV8)
__L2CMEM void measure_barrier(int type) 
{
    unsigned long _cnt1, _cnt2;
    int i;
 
    for (i=0; i<PERF_TEST_NUM; i++) {

        switch(type) {
            case 0:
                _measure_dsb(_cnt1,_cnt2);
                break;
            case 1:
                _measure_dsbst(_cnt1,_cnt2);
                break;
            case 2:
                _measure_dmbosh(_cnt1,_cnt2);
                break;
            case 3:
                _measure_dmboshst(_cnt1,_cnt2);
                break;
            default:
                break;
        }
        
        piTest[B_dsb_I+type].cntBef[i] = _cnt1;
        piTest[B_dsb_I+type].cntAft[i] = _cnt2;
    }
    return;
}
#endif

__L2CMEM void measure_D_L2_cacheMiss(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = g_dram_cac_addr;
    #else
    _addr = GDMP_SRAM_CAC_ADDR;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {
        
        /* invalidate D/L2 cache before test */
        #ifdef TCSUPPORT_CPU_ARMV8
        _dcache_inv_by_addr_to_PoC(_addr);
        #else
        _Dcache_inv_by_addr(_addr);
        _L2cache_inv_by_addr(_addr);
        #endif

        /* CPU cached read addr (D+L2-cache_miss + sram_uncached_32B time) */
        _measure_read_by_addr(_cnt1,_cnt2, _addr);
        piTest[D_L2_cacheMissI].cntBef[i] = _cnt1;
        piTest[D_L2_cacheMissI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void measure_D_cache_inv(void) 
{
    unsigned long _cnt1, _cnt2, _addr, tmp;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = g_dram_cac_addr;
    #else
    _addr = GDMP_SRAM_CAC_ADDR;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {
        
        /* read D-cache before test */
        tmp=rdMeml(_addr);
        tmp++;

        #ifdef TCSUPPORT_CPU_ARMV8
        /* D+L2-cache inv time */
        _measure_dcache_inv_by_addr_to_PoC(_cnt1,_cnt2, _addr);
        #else
        /* D-cache inv time */
        _measure_Dcache_inv_by_addr(_cnt1,_cnt2, _addr);
        #endif
        piTest[D_cacheInvI].cntBef[i] = _cnt1;
        piTest[D_cacheInvI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void D_L2_cache_loaded(void)
{
    unsigned long tmp, _addr;

    
    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = g_dram_cac_addr;
    #else
    _addr = GDMP_SRAM_CAC_ADDR;
    #endif

    #ifdef TCSUPPORT_CPU_ARMV8
    _dcache_inv_by_addr_to_PoC(_addr);
    #else
    _Dcache_inv_by_addr(_addr);
    _L2cache_inv_by_addr(_addr);
    #endif
    
    tmp=rdMeml(_addr);
    return;
}

__L2CMEM void measure_D_cache_miss(void) 
{
    unsigned long _cnt1, _cnt2, addr, tmp;
    int i;

    for (i=0; i<PERF_TEST_NUM; i++) {

        
        #ifndef TCSUPPORT_CPU_ARMV8 /* ARM can't just invalidate dcache */

        /* L2-cache loaded then D-cache inv before test */
        addr=GDMP_SRAM_CAC_ADDR;
        D_L2_cache_loaded();
        _Dcache_inv_by_addr(addr);

        /* D-cache miss time */
        _measure_read_by_addr(_cnt1,_cnt2, addr);

        piTest[D_cacheMissI].cntBef[i] = _cnt1;
        piTest[D_cacheMissI].cntAft[i] = _cnt2;
        #endif
    }
    return;
}

__L2CMEM void measure_D_cache_hit(void) 
{
    unsigned long _cnt1, _cnt2, _addr, tmp;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = g_dram_cac_addr;
    #else
    _addr = GDMP_SRAM_CAC_ADDR;
    #endif

    /* read D-cache before test */
    tmp=rdMeml(_addr);
    tmp++;

    for (i=0; i<PERF_TEST_NUM; i++) {

        /* D-cache hit time */
        _measure_read_by_addr(_cnt1,_cnt2, _addr);
        piTest[D_cacheHitI].cntBef[i] = _cnt1;
        piTest[D_cacheHitI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void measure_L2_cache_inv(void) 
{
    unsigned long _cnt1, _cnt2, addr, tmp;
    int i;

    for (i=0; i<PERF_TEST_NUM; i++) {

        #ifndef TCSUPPORT_CPU_ARMV8 /* ARM can't just invalidate L2cache */

        /* L2-cache loaded before test */
        addr=GDMP_SRAM_CAC_ADDR;
        D_L2_cache_loaded();

        /* L2-cache inv time */
        _measure_L2cache_inv_by_addr(_cnt1,_cnt2, addr);

        piTest[L2_cacheInvI].cntBef[i] = _cnt1;
        piTest[L2_cacheInvI].cntAft[i] = _cnt2;
        #endif        
    }
    return;
}

__L2CMEM void measure_D_cache_wback_inv(void) 
{
    unsigned long _cnt1, _cnt2, _addr;
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    _addr = g_dram_cac_addr;
    #else
    _addr = GDMP_SRAM_CAC_ADDR;
    #endif

    for (i=0; i<PERF_TEST_NUM; i++) {
        
        /* make sure D-cache is written before wback_inv it */
        D_L2_cache_loaded();
        wtMeml(_addr, 0);
        wmb();

        /* D-cache wback_inv time (including Sync) */
        #ifdef TCSUPPORT_CPU_ARMV8
        _measure_dcache_wback_inv_by_addr_to_PoC(_cnt1,_cnt2, _addr);
        #else
        _measure_Dcache_wback_inv_by_addr(_cnt1,_cnt2, _addr);
        #endif
        piTest[D_cacheWbInvI].cntBef[i] = _cnt1;
        piTest[D_cacheWbInvI].cntAft[i] = _cnt2;
    }
    return;
}

__L2CMEM void measure_L2_cache_wback_inv(void) 
{
    unsigned long _cnt1, _cnt2, addr, tmp;
    int i;

    for (i=0; i<PERF_TEST_NUM; i++) {

        #ifndef TCSUPPORT_CPU_ARMV8 /* ARM can't just wback_inv L2cache */

        /* make sure L2-cache is written before wback_inv it */
        addr=GDMP_SRAM_CAC_ADDR;
        D_L2_cache_loaded();
        VPint(addr) = tmp;
        _Dcache_wback_inv_by_addr(addr);

        /* L2-cache wback_inv time (including Sync) */
        _measure_L2cache_wback_inv_by_addr(_cnt1,_cnt2, addr);
        piTest[L2_cacheWbInvI].cntBef[i] = _cnt1;
        piTest[L2_cacheWbInvI].cntAft[i] = _cnt2;
        #endif
    }
    return;
}

void qdma_txrx_dma_en (int enable)
{
    unsigned long addr;
    unsigned int val;

    addr = ioremap(0x1fb54004, 8);
    val = rdMemW(addr);
    
    if (enable)
        val |= 0x5;
    else
        val &= (~0x5);
    
    wtMemW(addr, val);
    iounmap(addr);
    
    return;
}

__L2CMEM void gdma_data_moving(uint32 srcAddr, uint32 dstAddr, uint32 is16DW, uint32 item)
{
    int i;

    #ifdef TCSUPPORT_CPU_ARMV8
    SET_SHARE_FEMEM_SEL(1);
    for (i=0; i<16; i++)
        CLEAR_GDMA_DONE(i);

    #else
    /* GDMA common config*/
    VPint(SHARE_FEMEM_SEL)=1;       /*FE_MEM acts as memory*/
    VPint(GDMA_CH0_CTL1)=0x4;       /*GDMA coherent*/
    VPint(GDMA_DONE_INT)=ALL_FF;    /*clear all bits*/
    #endif

    /* DRAM to DRAM 64KB_1DW */
    for (i=0; i<PERF_TEST_NUM; i++)
        gdma_data_moving_conf(srcAddr,dstAddr,i,is16DW, item);

    /* set back to 0, otherwise, the result for "read reg" will be affected */
    #ifdef TCSUPPORT_CPU_ARMV8
    SET_SHARE_FEMEM_SEL(0);
    #else
    VPint(SHARE_FEMEM_SEL)=0;
    #endif
    wmb();

    return;
}

#ifdef TCSUPPORT_CPU_ARMV8
void arm_pmu_init(void)
{
#ifdef TCSUPPORT_CPU_ARMV8_64
    /* enable all PMU events */
    pmu_set_pmcr_el0(pmu_get_pmcr_el0()|PMCR_EL0_EN);
    /* enable cnt31 for the cycle event */
    pmu_set_pmcnten_el0(pmu_get_pmcnten_el0() |PMCNTC);
    #if 0
    /* enable cnt0 for the inst_retired event */
    pmu_set_pmcnten_el0(pmu_get_pmcnten_el0() |PMCNT0);
    pmu_set_pmselr(PMCNT0);
    pmu_set_pmxevtyper(TYPE_INST_RETIRED);
    /* enable cnt1 for the inst_spec event */
    pmu_set_pmcnten_el0(pmu_get_pmcnten_el0() |PMCNT1);
    pmu_set_pmselr(PMCNT1);
    pmu_set_pmxevtyper(TYPE_INST_SPEC);
    /* get inst_retired and inst_spec */
    printk("inst_retired:0x%lx\n", pmu_get_event_counter(PMCNT0));
    printk("inst_spec:0x%lx\n", pmu_get_event_counter(PMCNT1));
    #endif

#else /* ARM32 PMU init */

    enablePMU() ;               /* Enable the PMU */

    #if 0 /* reset counters */
    resetCCNT();                /* Reset the CCNT (cycle counter) */
    resetPMN();                 /* Reset the configurable counters */
    #endif

    /* Configure CCNT to count events in all PLs
     * NOTE: For the CCNT the event code is ignored
     * NOTE: Only needed on processors that implement PMUv2 */
    pmnConfig(V7_PMU_CCNT, 0, V7_PMU_EVENT_FILTER_ALL_PL);
    enableCCNT();               /* Enable CCNT */

    #if 0 /* enable pmu for instructions */
    /* Configure counter 0 to count event code 0x03 (for instructions) */
    pmnConfig(V7_PMU_COUNTER0, 0x03, V7_PMU_EVENT_FILTER_ALL_PL);
    enablePMN(0);               /* Enable counter */
    #endif
    #if 0 /* print inst and cycle counters */
    printk("Counter_0 = %d,  CCNT = %d\n", readPMN(0), read_c0_count());
    mdelay(1);
    printk("Counter_0 = %d,  CCNT = %d\n", readPMN(0), read_c0_count());
    #endif

#endif

    return;
}
#endif

__L2CMEM int doCpuCycles(int argc, char *argv[], void *p)
{
    uint32 aaa, i, j, printResult=1;
    uint32 *cached_addr, *uncached_addr;
    uint32 value = 0x12345678;
    uint32 reg0, reg1, addr;
    uint32 *tmpAddr1, *tmpAddr2;
    uint32 *dram_unc_addr;
    dma_addr_t dram_phy_srcAddr;
    uint32 dram_phy_dstAddr;
    struct device *dev=NULL;
    #ifdef TCSUPPORT_CPU_ARMV8
    void *io_reg = 0;
    unsigned long uncAddr, aaal;
    uint32 testCnt=0, s_long=sizeof(unsigned long);
	uint32 vir_flag = 0;
    unsigned int gdump_sel;
    #endif

    #ifndef TCSUPPORT_CPU_ARMV8 /* not used */
	if(argc != 2 || (strcmp(argv[1], "base") && strcmp(argv[1], "l2cache") &&
                     strcmp(argv[1], "dramRead") && strcmp(argv[1], "regRead") &&
                     strcmp(argv[1], "dramReadBase") && strcmp(argv[1], "regReadBase") &&
                     strcmp(argv[1], "perfIndex"))){
		printk("Usage: sys cpuCycles [base||l2cache||dramReadBase||dramRead||regRead||perfIndex]\r\n");
		return 0;
	}
    #endif
    
    #ifdef TCSUPPORT_CPU_ARMV8
    gdump_sel = gdump_get_mode_sel();
    gdump_mode_sel(0); /*for cpu/gdma access gump_sram via pbus */
    SET_SHARE_FEMEM_SEL(1); /*for cpu/gdma access fe_sram via pbus */
    
    if ((dev = get_gdmpSram_dev())==NULL) {
        printk("get_gdmpSram_dev is NULL\n");
        return 0;
    }
    #endif

    dram_unc_addr = (uint32 *) dma_alloc_coherent(dev, SIZE_128K, &dram_phy_srcAddr, GFP_KERNEL);
    if (((uint32)dram_unc_addr)&0x1f) {
        printk("\nError: dram_unc_addr:0x%p is not 32-byte alignment, finish testing!\n", dram_unc_addr);
        return 0;
    }
    dram_phy_dstAddr = (uint32)dram_phy_srcAddr+SIZE_64K;
    for (i=0; i<CYCLES_TEST_NUM; i++) {
        cpuCountBefore[i]=0;
        cpuCountAfter[i]=0;
    }
    
    cached_addr = (uint32*)kzalloc(L2_CACHE_TEST_SIZE, GFP_KERNEL);
    if (((uint32)cached_addr) % 32 != 0) {
        printk("\nError: cached_addr:0x%p is not 32-byte alignment, finish testing!\n", cached_addr);
        dma_free_coherent(dev, SIZE_128K, dram_unc_addr, dram_phy_srcAddr);
        return 0;
    }

    
    #ifdef TCSUPPORT_CPU_ARMV8
    arm_pmu_init();
    #endif

    local_irq_disable();
    aaa = 0; /* load value to aaa's cache */
    is_dramRW=0;

    if (strcmp(argv[1], "base") == 0) { /* base value */
        
        for (i=0; i<CYCLES_TEST_NUM; i++) {
            cpuCountBefore[i] = read_c0_count();
            cpuCountAfter[i] = read_c0_count();
        }
    }
    else if (strcmp(argv[1], "debug") == 0) { /* enable to print more info during cycles measurement */

        if (strcmp(argv[2], "on") == 0)
            is_cycles_dbgOn=1;
        else
            is_cycles_dbgOn=0;
        
        printk("is_cycles_dbgOn:%d\n", is_cycles_dbgOn);
        printResult=0;
    }
    else if (strcmp(argv[1], "dramReadBase") == 0) { /* uncache read DRAM Base */
        
        for (i=0; i<CYCLES_TEST_NUM; i++) {
            
            cpuCountBefore[i] = read_c0_count();
            #ifndef TCSUPPORT_CPU_ARMV8
            aaa = rdMeml(dram_unc_addr);
            /* cpu will read c0_count without waiting "uncached read dram",
             * becasue "aaa" and "cpuCountAfter[]" have no dependency. 
             * So the value for cpuCountAfter[] is not really the time for
             * cpu uncached read dram */
            #endif
            cpuCountAfter[i] = read_c0_count();
        }
        try_dramRead=0;
        is_dramRW=1;
    }
    else if (strcmp(argv[1], "dramRead") == 0) { /* uncache read DRAM */

        for (i=0; i<CYCLES_TEST_NUM; i++) {
            
            cpuCountBefore[i] = read_c0_count();
            #ifdef TCSUPPORT_CPU_ARMV8
			/* ARM CPU doesn't need dependency to measure "read" */
            aaa = rdMemW(dram_unc_addr);
            #else
            aaa = rdMeml(dram_unc_addr);
            /* cpu has to wait the 1st "uncached read dram" before 
             * it can do the 2nd "uncached read dram",
             * becasue 1st "aaa" and 2st "aaa" are the same (have dependency). 
             * So the value for cpuCountAfter[] is really the time for
             * cpu uncached read dram */
            aaa = rdMeml(dram_unc_addr);
            #endif
            cpuCountAfter[i] = read_c0_count();
        }
        try_dramRead++;
        is_dramRW=1;
    }
    else if (strcmp(argv[1], "regReadBase") == 0) { /* uncache read register */

        for (i=0; i<CYCLES_TEST_NUM; i++) {

            cpuCountBefore[i] = read_c0_count();
            #ifndef TCSUPPORT_CPU_ARMV8
            aaa = VPint(SCREG_WR0);
            #endif
            cpuCountAfter[i] = read_c0_count();
        }
    }
    else if (strcmp(argv[1], "regRead") == 0) { /* uncache read register */

        #ifdef TCSUPPORT_CPU_ARMV8
        if (argc == 3) {
    		io_reg = ((void *) simple_strtoul(argv[2], NULL, 16)); /* for pcie reg read by virt addr */
            /* 0x20000000~0x30000000 for pcie devmem */
            if (((unsigned long)io_reg)<0x30000000) /* for reg read by phys addr */
                io_reg = ioremap(io_reg, 8);
			else
				vir_flag = 1;
        	}
    	else /* default one */
    	{
    		io_reg = ioremap(SCREG_WR0, 8);
    	}
        #endif
        
        for (i=0; i<CYCLES_TEST_NUM; i++) {

            cpuCountBefore[i] = read_c0_count();
            #ifdef TCSUPPORT_CPU_ARMV8
			/* ARM CPU doesn't need dependency to measure "read" */
			aaa = rdMemW(io_reg);
			//dsb(st);
            #else
            aaa = VPint(SCREG_WR0);
            aaa = VPint(SCREG_WR0);
            #endif
            cpuCountAfter[i] = read_c0_count();
        }
        #ifdef TCSUPPORT_CPU_ARMV8
		if (vir_flag == 0)
        iounmap(io_reg);
        #endif
    }
    #ifdef TCSUPPORT_CPU_ARMV8
    else if (strcmp(argv[1], "memReadl") == 0) { /* uncache read 8 bytes */

		io_reg = ((void *) simple_strtoul(argv[2], NULL, 16));
        io_reg = ioremap(io_reg, 8);
        for (i=0; i<CYCLES_TEST_NUM; i++) {
            cpuCountBefore[i] = read_c0_count();
			aaal = rdMeml(io_reg);
            cpuCountAfter[i] = read_c0_count();
        }
        iounmap(io_reg);
    }
    else if (strcmp(argv[1], "wmb") == 0) { /* wmb */
        /* CONFIG_ARM_HEAVY_MB is enabled, so wmb() includes dsb(st) and arm_heavy_mb().
         * Both outer_cache.sync() and soc_mb() in arm_heavy_mb() in flush.c are NULL, 
         * but wmb() still takes 14 more cpu cycles than dsb(st). */
        wmb();
        for (i=0; i<CYCLES_TEST_NUM; i++) {
            cpuCountBefore[i] = read_c0_count();
             wmb();
            cpuCountAfter[i] = read_c0_count();
        }
    }
    else if (strcmp(argv[1], "dsbst") == 0) { /* dsbst */

        dsb(st);
        for (i=0; i<CYCLES_TEST_NUM; i++) {
            cpuCountBefore[i] = read_c0_count();
             dsb(st);
            cpuCountAfter[i] = read_c0_count();
        }
    }
    else if (strcmp(argv[1], "writes_base") == 0) { /* Base value for writes */

        if (argc == 3)
            testCnt = simple_strtoul(argv[2], NULL, 10);

        dsb(st);
        for (i=0; i<CYCLES_TEST_NUM; i++) {
            cpuCountBefore[i] = read_c0_count();
            for (j=0, uncAddr=dram_unc_addr; j<testCnt; j++) {
                wtMeml(uncAddr, 0);
                uncAddr+=s_long;
            }
            cpuCountAfter[i] = read_c0_count();
        }
        try_dramRead=0;
        is_dramRW=1;
    }
    else if (strcmp(argv[1], "writes_dsbst") == 0) { /* Base value for writes + dsbst */

        if (argc == 3)
            testCnt = simple_strtoul(argv[2], NULL, 10);

        dsb(st);
        for (i=0; i<CYCLES_TEST_NUM; i++) {
            cpuCountBefore[i] = read_c0_count();
            for (j=0, uncAddr=dram_unc_addr; j<testCnt; j++) {
                wtMeml(uncAddr, 0);
                uncAddr+=s_long;
                dsb(st);
            }
            //dsb(st);
            cpuCountAfter[i] = read_c0_count();
        }
        try_dramRead++;
        is_dramRW=1;
    }
    #endif
    else if (strcmp(argv[1], "perfIndex") == 0) { /* cpu_bus performance Index */

        g_dram_unc_addr = (unsigned long)dram_unc_addr;
        g_dram_cac_addr = (unsigned long)cached_addr;

        /* CPU uncached read base */
        measure_read_base();

        /* CPU uncached read SCU REG */
        measure_read_scuReg();

        /* CPU uncached read DRAM */
        measure_read_dram();
        
        /* CPU uncached read GDMP SRAM */
        measure_read_gdmpSram();

        /* CPU uncached read FE SRAM */
        measure_read_feSram();
        
        /* CPU uncached write base */
        measure_write_base();
        
        /* CPU uncached write SCU REG */
        measure_write_scuReg();

        /* CPU uncached write DRAM */
        measure_write_dram();

        /* CPU issues Sync cmd */
        /* CPU issues "dsb st" cmd for ARM */
        measure_sync();

        /* CPU cached read RAM (D+L2-cache_miss + sram_uncached_32B time) */
        measure_D_L2_cacheMiss();

        /* D-cache inv time (including Sync) */
        /* D+L2-cache inv time for ARM (including "dsb st") */
        measure_D_cache_inv();

        /* CPU cached read RAM (D-cache_miss) */
        /* N/A for D-cache_miss for ARM */
        measure_D_cache_miss();

        /* CPU cached read RAM (D-cache_hit) */
        measure_D_cache_hit();

        /* L2-cache inv time (including Sync) */
        /* N/A for L2-cache inv for ARM */
        measure_L2_cache_inv();

        /* D-cache wback_inv time (including Sync) */
        /* D+L2-cache wback_inv time for ARM (including "dsb st") */
        measure_D_cache_wback_inv();

        /* L2-cache wback_inv time (including Sync) */
        /* N/A for L2-cache wback_inv for ARM */
        measure_L2_cache_wback_inv();

        /* qdma will access fe_sram also, so disable it before test */
        qdma_txrx_dma_en(0);
                
        /* DRAM to DRAM 64KB_1DW */
        gdma_data_moving(dram_phy_srcAddr,dram_phy_dstAddr,0,GDMA_Dram_Dram_1DwI);
        
        /* DRAM to DRAM 64KB_16DW */
        gdma_data_moving(dram_phy_srcAddr,dram_phy_dstAddr,1,GDMA_Dram_Dram_16DwI);
        
        /* DRAM to FE_SRAM 64KB_1DW */
        gdma_data_moving(dram_phy_srcAddr,FE_SRAM_PHY_DST,0,GDMA_Dram_feSram_1DwI);

        /* DRAM to FE_SRAM 64KB_16DW */
        gdma_data_moving(dram_phy_srcAddr,FE_SRAM_PHY_DST,1,GDMA_Dram_feSram_16DwI);
        
        /* FE_SRAM to FE_SRAM 64KB_1DW */
        gdma_data_moving(FE_SRAM_PHY_SRC,FE_SRAM_PHY_DST,0,GDMA_feSram_feSram_1DwI);

        /* FE_SRAM to FE_SRAM 64KB_16DW */
        gdma_data_moving(FE_SRAM_PHY_SRC,FE_SRAM_PHY_DST,1,GDMA_feSram_feSram_16DwI);

        /* FE_SRAM to DRAM 64KB_1DW */
        gdma_data_moving(FE_SRAM_PHY_SRC,dram_phy_dstAddr,0,GDMA_feSram_Dram_1DwI);  

        /* FE_SRAM to DRAM 64KB_1DW */
        gdma_data_moving(FE_SRAM_PHY_SRC,dram_phy_dstAddr,1,GDMA_feSram_Dram_16DwI);

        /* recover qdma after test */
        qdma_txrx_dma_en(1);

        #if defined(TCSUPPORT_CPU_ARMV8)
        /* measure many types of arm barrier cmds */
        for (i=0; i<4; i++)
            measure_barrier(i);
        #endif

        perfIndex_result_print();
        printResult=0;
    }
#ifndef TCSUPPORT_CPU_ARMV8
    else if (strcmp(argv[1], "l2cache") == 0) { /* L2 cache */

        for (i=0; i<CYCLES_TEST_NUM; i++) {
            
            tmpAddr1 = cached_addr;
            tmpAddr2 = cached_addr;
            dma_cache_wback_inv(cached_addr, L2_CACHE_TEST_SIZE);
            
            cpuCountBefore[i] = read_c0_count();
            for (j=0; j<L2_CACHE_TEST_WORDS; j++, tmpAddr1++)
                VPint(tmpAddr1) = value;
            for (j=0; j<L2_CACHE_TEST_WORDS; j++, tmpAddr2++)
                aaa = VPint(tmpAddr2);
            cpuCountAfter[i] = read_c0_count();
        }
    }
#endif
    else {
        printResult=0;
        printk("\nWrong doCpuCycles type:%s\n", argv[1]);
    }

    local_irq_enable();

    #ifdef TCSUPPORT_CPU_ARMV8
    gdump_mode_sel(gdump_sel);
    SET_SHARE_FEMEM_SEL(0);
    #endif

    dma_free_coherent(dev, SIZE_128K, dram_unc_addr, dram_phy_srcAddr);
    kfree(cached_addr);

    if (printResult) {
        printk("\nmeasure CPU cycles for %s\n", argv[1]);
        printk("Average CPU cycles: %d\n", 
                cpu_cycles_calculate(cpuCountBefore, cpuCountAfter, CYCLES_TEST_NUM));
    }

    return 0;
}

#ifndef TCSUPPORT_CPU_ARMV8
int doRdBypassWt_cpu2(void *arg)
{
    unsigned long *uncached_addr;
    dma_addr_t phy_addr;
    uint32 i, j, value=0x12345678;
    uint32 reg0, reg1;
    uint32 rounds=50000, time=1;
    uint32 idxMask = ((32/sizeof(unsigned long))-1);
    struct device *dev=NULL;

    #ifdef TCSUPPORT_CPU_ARMV8    
    if ((dev=get_gdmpSram_dev())==NULL) {
        printk("\nget_gdmpSram_dev failed\n");
        goto doRdBypassWt_cpu2_end;
    }
    #endif
    uncached_addr = (unsigned long *)dma_alloc_coherent(dev, RDBYPASSWT_TEST_SIZE, &phy_addr, GFP_KERNEL);
    if (((unsigned long)uncached_addr) % 32 != 0) {
        printk("\nError: uncached_addr:0x%p is not 32-byte alignment, finish testing!\n", uncached_addr);
        goto doRdBypassWt_cpu2_end;
    }

    if (data0)
        rounds = data0;
    #ifndef TCSUPPORT_CPU_ARMV8 /* rbus_cfg0 & cfg1 don't exist on 7523 bus, and rdBypassWt deault enabled */
    /* enable "read bypass write" */
    reg0 = VPint(RBUS_CFG0);
    reg1 = VPint(RBUS_CFG1);
    VPint(RBUS_CFG0) &= (~0x1);
    VPint(RBUS_CFG1) = 0xffffffff;
    VPint(RBUS_CFG0) |= (0x1);
    #endif

    printk("\n%s Start...(with base_addr:0x%p)(writeRounds:%d)\n", __func__, uncached_addr,rounds);

    rdBypassWt_uncached_addr = uncached_addr;

    wake_up_process(task_rdBypassWt_cpu3);

    /* keep doing uncached write in 32-byte range */
    while (1) {
        
        for (i=0, j=0; j<rounds; i++, j++, value++) {
            wtMeml((&(uncached_addr[i&idxMask])), value);
        }

        if (rdBypassWt_end) break;

        msleep(time);
    }

    dma_free_coherent(dev, RDBYPASSWT_TEST_SIZE, uncached_addr, phy_addr);

    #ifndef TCSUPPORT_CPU_ARMV8 /* rbus_cfg0 & cfg1 don't exist on 7523 bus, and rdBypassWt deault enabled */
    VPint(RBUS_CFG0) = reg0;
    VPint(RBUS_CFG1) = reg1;
    #endif
    
    printk("\n%s End\n", __func__);

doRdBypassWt_cpu2_end:
    rdBypassWt_end++;
    return 0;
}

int doRdBypassWt_cpu3(void *arg)
{
    unsigned long *uncached_addr = rdBypassWt_uncached_addr;
    uint32 i, j, k, cnt0, cnt1, tmpVal, bypassCnt0, bypassCnt1;
    uint32 cpuTotalCnt_near=0, cpuTotalCnt_far=0, bypassTotalCnt_near=0, bypassTotalCnt_far=0;
    unsigned long *tmp_addr;
    uint32 is32NearByteTest=1;
    uint32 rounds=1000, time=1;
    uint32 testLen = (32/sizeof(unsigned long));
   
    if ((uncached_addr==0) || (data2==0)) {
        printk("\ncached_addr==0, exit %s!\n", __func__);
        goto doRdBypassWt_cpu3_end;
    }

    if (data1)
        rounds = data1;
    printk("%s Start...(readRounds:%d)\n\n", __func__,rounds);

    for (k=0; k<(data2<<1); k++) {

        if (is32NearByteTest) {
            tmp_addr = uncached_addr;
        }
        else {
            tmp_addr = uncached_addr + ((RDBYPASSWT_TEST_SIZE>>1)/sizeof(unsigned long));
        }
        #ifndef TCSUPPORT_CPU_ARMV8 /* rdBypassWt_cnt address is changed for 7523 bus */
        bypassCnt0 = (VPint(RDBYPASSWT_CNT_REG)&RDBYPASSWT_CNT_MASK);
        cnt0 = read_c0_count();
        #endif

        /* doing uncache read when cpu2 is doing uncache write */
        for (i=0; i<rounds; i++) {
            for (j=0; j<testLen; j++)
                tmpVal = rdMeml(&tmp_addr[j]);
        }
        #ifndef TCSUPPORT_CPU_ARMV8 /* rdBypassWt_cnt address is changed for 7523 bus */
        cnt1 = read_c0_count();
        bypassCnt1 = (VPint(RDBYPASSWT_CNT_REG)&RDBYPASSWT_CNT_MASK);
        #endif
        
        if (is32NearByteTest) {
            
            is32NearByteTest=0;
            if (cnt1 > cnt0)
                cpuTotalCnt_near += (cnt1 - cnt0);
            else
                cpuTotalCnt_near += (cnt0 - cnt1);

            if (bypassCnt1 > bypassCnt0)
                bypassTotalCnt_near += (bypassCnt1 - bypassCnt0);
            else
                bypassTotalCnt_near += (bypassCnt0 - bypassCnt1);
        }
        else {
            
            is32NearByteTest=1;
            if (cnt1 > cnt0)
                cpuTotalCnt_far += (cnt1 - cnt0);
            else
                cpuTotalCnt_far += (cnt0 - cnt1);

            if (bypassCnt1 > bypassCnt0)
                bypassTotalCnt_far += (bypassCnt1 - bypassCnt0);
            else
                bypassTotalCnt_far += (bypassCnt0 - bypassCnt1);
        }

        msleep(time);
    }

    #ifdef TCSUPPORT_CPU_ARMV8
    printk("for 32-byte-near: Average cpuCycles:%d\n", (int)((cpuTotalCnt_near<<1)/data2));
    printk("for 32-byte-far:  Average cpuCycles:%d, rdBypassWtCnt:%d\n", (int)((cpuTotalCnt_far<<1)/data2));
    #else
    printk("for 32-byte-near: Average cpuCycles:%d, rdBypassWtCnt:%d\n", 
                        (int)((cpuTotalCnt_near<<1)/data2), (int)(bypassTotalCnt_near/data2));
    printk("for 32-byte-far:  Average cpuCycles:%d, rdBypassWtCnt:%d\n", 
                        (int)((cpuTotalCnt_far<<1)/data2), (int)(bypassTotalCnt_far/data2));
    #endif

doRdBypassWt_cpu3_end:
    rdBypassWt_end++;
    return 0;
}

int doRdBypassWtTest(int argc, char *argv[], void *p)
{
    uint32 reg0, reg1;
    uint32 *cached_region0, *uncached_region0, *cached_region1, *uncached_region1;
    uint32 array_32B[REGION_WORDS] = {0x12345678, 0x23456789, 0x34567890, 0x45678901,
                                      0x56789012, 0x67890123, 0x78901234, 0x89012345};
    uint32 i, round=0;
    uint32 *tmpAddr;
    uint32 value = 0x12345678, tmpVal;
    uint32 readCnt=0;

    cached_region0 = (uint32*)kzalloc(BYPASS_DRAM_TEST_SIZE, GFP_KERNEL);
    if (((uint32)cached_region0) % 32 != 0) {
        printk("\nError: cached_addr:0x%p is not 32-byte alignment, finish testing!\n", cached_region0);
        return 0;
    }

    printk("\ndoRdBypassWtTest Start...\n\n");

    uncached_region0 = (uint32 *)((uint32)cached_region0 | 0x20000000);
    cached_region1 = cached_region0+(REGIONS_DISTANCE>>2);
    uncached_region1 = uncached_region0+(REGIONS_DISTANCE>>2);
    #ifndef TCSUPPORT_CPU_ARMV8
    dma_cache_wback_inv(cached_region1, REGION_SIZE);
    #endif

    for (i=0; i<(REGION_WORDS); i++) {
        uncached_region1[i] = array_32B[i];
    }

    #ifndef TCSUPPORT_CPU_ARMV8
    /* enable "read bypass write" */
    reg0 = VPint(RBUS_CFG0);
    reg1 = VPint(RBUS_CFG1);
    VPint(RBUS_CFG0) &= (~0x1);
    VPint(RBUS_CFG1) = 0xffffffff;
    VPint(RBUS_CFG0) |= (0x1);
    #endif

    while (1)
    {
        /* cache-write-cache-read for 32-byte-nearby address */
    
        tmpVal = value;
        for (i=0; i<(REGION_WORDS); i++) {
            cached_region0[i] = tmpVal++;
        }
        #ifndef TCSUPPORT_CPU_ARMV8
        dma_cache_wback_inv(cached_region0, REGION_SIZE);
        #endif

        if (cached_region0[readCnt] != (value+readCnt)) {
            printk("\nError1! cached_region0[%d]:0x%08x != 0x%08x at Round%d\n", 
                        readCnt, cached_region0[readCnt], (value+readCnt), round);
            break;
        }

        value += REGION_WORDS;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;

        /* cache-write-uncache-read for 32-byte-nearby address */

        tmpVal = value;
        for (i=0; i<(REGION_WORDS); i++) {
            cached_region0[i] = tmpVal++;
        }
        #ifndef TCSUPPORT_CPU_ARMV8
        dma_cache_wback_inv(cached_region0, REGION_SIZE);
        #endif

        if (uncached_region0[readCnt] != (value+readCnt)) {
            printk("\nError2! uncached_region0[%d]:0x%08x != 0x%08x at Round%d\n", 
                        readCnt, uncached_region0[readCnt], (value+readCnt), round);
            break;
        }

        value += REGION_WORDS;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;


        /* uncache-write-cache-read for 32-byte-nearby address */

         tmpVal = value;
        for (i=0; i<(REGION_WORDS); i++) {
            uncached_region0[i] = tmpVal++;
        }

        if (cached_region0[readCnt] != (value+readCnt)) {
            printk("\nError3! cached_region0[%d]:0x%08x != 0x%08x at Round%d\n", 
                        readCnt, cached_region0[readCnt], (value+readCnt), round);
            break;
        }

        value += REGION_WORDS;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;       

        /* uncache-write-uncache-read for 32-byte-nearby address */

         tmpVal = value;
        for (i=0; i<(REGION_WORDS); i++) {
            uncached_region0[i] = tmpVal++;
        }

        if (uncached_region0[readCnt] != (value+readCnt)) {
            printk("\nError4! uncached_region0[%d]:0x%08x != 0x%08x at Round%d\n", 
                        readCnt, uncached_region0[readCnt], (value+readCnt), round);
            break;
        }

        value += REGION_WORDS;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;


        /* cache-write-cache-read for 32-byte-far address */
        #ifndef TCSUPPORT_CPU_ARMV8
        dma_cache_wback_inv(cached_region1, REGION_SIZE);
        #endif
        
        cached_region0[readCnt] = value;
        #ifndef TCSUPPORT_CPU_ARMV8
        dma_cache_wback_inv(cached_region0, REGION_SIZE);
        #endif

        if (cached_region1[readCnt] != array_32B[readCnt]) {
            printk("\nError5! cached_region1[%d]:0x%08x != array_32B[%d]:0x%08x at Round%d\n", 
                        readCnt, cached_region0[readCnt], readCnt, array_32B[readCnt], round);
            break;
        }

        value ++;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;

        /* cache-write-uncache-read for 32-byte-far address */
        
        cached_region0[readCnt] = value;
        #ifndef TCSUPPORT_CPU_ARMV8
        dma_cache_wback_inv(cached_region0, REGION_SIZE);
        #endif

        if (uncached_region1[readCnt] != array_32B[readCnt]) {
            printk("\nError6! cached_region1[%d]:0x%08x != array_32B[%d]:0x%08x at Round%d\n", 
                        readCnt, uncached_region0[readCnt], readCnt, array_32B[readCnt], round);
            break;
        }

        value ++;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;

        /* uncache-write-cache-read for 32-byte-far address */
        #ifndef TCSUPPORT_CPU_ARMV8
        dma_cache_wback_inv(cached_region1, REGION_SIZE);
        #endif
        
        uncached_region0[readCnt] = value;

        if (cached_region1[readCnt] != array_32B[readCnt]) {
            printk("\nError7! cached_region1[%d]:0x%08x != array_32B[%d]:0x%08x at Round%d\n", 
                        readCnt, cached_region0[readCnt], readCnt, array_32B[readCnt], round);
            break;
        }

        value ++;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;

        /* uncache-write-uncache-read for 32-byte-far address */
        
        uncached_region0[readCnt] = value;

        if (uncached_region1[readCnt] != array_32B[readCnt]) {
            printk("\nError8! uncached_region1[%d]:0x%08x != array_32B[%d]:0x%08x at Round%d\n", 
                        readCnt, cached_region0[readCnt], readCnt, array_32B[readCnt], round);
            break;
        }

        value ++;        
        readCnt++;
        if (readCnt == REGION_WORDS)
            readCnt = 0;

        if (round==0xffffffff) {
            printk("\ndoRdBypassWtTest succeed!\n");
            break;
        }

        if ((round & 0x3ffff) == 0x3ffff) {
            printk("\nTest Round:0x%08x\n", round);
            schedule();
        }

        round++;
    }

    #ifndef TCSUPPORT_CPU_ARMV8
    /* recover Read Bypass Write config */
    VPint(RBUS_CFG1) = reg1;
    VPint(RBUS_CFG0) = reg0;
    #endif

    return 0;
}

#else
int doRdBypassWt_read(void *arg)
{
	struct task_struct *task = current;
	u32 *uncached_addr = (u32 *) arg;
	u32	bypass_cnt = 0, bypass_cnt_before = 0, bypass_cnt_after = 0;
	unsigned long cpu_cycle = 0, cpu_cycle_before = 0, cpu_cycle_after = 0;
	u32 i = 0, j = 0;
	u32 tmpVal = 0;
	unsigned long rounds = 1;
	unsigned long flags;

	arm_pmu_init();
	busTest_locking(&cpu1rdBypassLock, &flags);

	while (rounds)
	{
		bypass_cnt_before = ecnt_get_rdbypass_cnt();
		cpu_cycle_before = read_c0_count();

		/* doing uncache read when cpu2 is doing uncache write */
		for (i = 0; i < 1000000; i++)
		{
			for (j = 0; j < REGION_WORDS; j++)
				tmpVal = rdMeml(&uncached_addr[j]);
		}

		cpu_cycle_after = read_c0_count();
		bypass_cnt_after = ecnt_get_rdbypass_cnt();

		if (cpu_cycle_after >= cpu_cycle_before)
		{
			cpu_cycle = cpu_cycle_after - cpu_cycle_before;
		}
		else
		{
			cpu_cycle = (~(0UL) - cpu_cycle_before) + cpu_cycle_after;
		}

		if (bypass_cnt_after >= bypass_cnt_before)
		{
			bypass_cnt = bypass_cnt_after - bypass_cnt_before;
		}
		else
		{
			bypass_cnt = (~(0U) - bypass_cnt_before) + bypass_cnt_after;
		}

        if ((rounds&0xf)==1)
		    printk("cpu %d rounds %lu bypass_cnt %x cpu_cycle %lx\n", task_cpu(task), rounds, bypass_cnt, cpu_cycle);
		rounds++;
	}
	busTest_unlocking(&cpu1rdBypassLock, &flags);
	return 0;
}

int doRdBypassWtTest(int argc, char *argv[], void *p)
{
	u32 *cached_region0 = NULL, *uncached_region0 = NULL;
	u32 *cached_region1 = NULL, *uncached_region1 = NULL;
	u32 array[REGION_WORDS] = {0};
	u32 i = 0, j = 0;
	u32 value = 0x12345678;
	dma_addr_t dma_addr = 0;
	struct device *dev = NULL;
	struct task_struct *read_task = NULL;
	struct task_struct *task = current;
	unsigned int other_cpu = ~task_cpu(task) & 0x1;
	unsigned long rounds = 1;
	unsigned long flags;

	if ((dev = get_gdmpSram_dev())==NULL)
	{
		printk("\nget_gdmpSram_dev failed\n");
		return 0;
	}

	uncached_region0 = (u32 *)dma_alloc_coherent(dev, BYPASS_DRAM_TEST_SIZE, &dma_addr, GFP_KERNEL);
	if (!uncached_region0)
	{
		printk("\nalloc resource failed\n");
		return 0;
	}
	is_cpu0_1_bus_test=1;
	SET_RCU_CPU_STALL_SUPRESS();

	cached_region0 = phys_to_virt(dma_to_phys(dev, dma_addr));
	cached_region1 = cached_region0 + ( REGIONS_DISTANCE >> 2);
	uncached_region1 = uncached_region0 + (REGIONS_DISTANCE >> 2);

	for (j = 0; j < REGION_WORDS; j++)
	{
		uncached_region1[j] = (value + j);
	}


	read_task = kthread_create_on_node(doRdBypassWt_read, uncached_region1, cpu_to_node(other_cpu),"rdBypassWt_read");
	kthread_bind(read_task, other_cpu);
	wake_up_process(read_task);


	busTest_locking(&cpu0rdBypassLock, &flags);

	while(rounds)
	{
		for (i = 0; i < 500000; i++)
		{
			prandom_bytes((void *) array, sizeof(array));

			for (j = 0; j < REGION_WORDS; j++)
			{
				cached_region0[j] = array[j];
			}

			ecnt_dcache_wback_inv(cached_region0, REGION_SIZE);

			for (j = 0; j < REGION_WORDS; j++)
			{
				if (cached_region0[j] != array[j])
				{
					printk("\nError1! cpu %d cached_region0[%d]:0x%08x != 0x%08x at Round%lu\n",
								task_cpu(task), j, cached_region0[j], array[j], rounds);
					goto error;
				}
			}

			for (j = 0; j < REGION_WORDS; j++)
			{
				if (uncached_region1[j] != (value + j))
				{
					printk("\nError1! cpu %d uncached_region1[%d]:0x%08x != 0x%08x at Round%lu\n",
								task_cpu(task), j, uncached_region1[j], (value + j), rounds);
					goto error;
				}
			}

			prandom_bytes((void *) array, sizeof(array));
			for (j = 0; j < REGION_WORDS; j++)
			{
				uncached_region0[j] = array[j];
			}

			for (j = 0; j < REGION_WORDS; j++)
			{
				if (uncached_region0[j] != array[j])
				{
					printk("\nError1! cpu %d uncached_region0[%d]:0x%08x != 0x%08x at Round%lu\n",
								task_cpu(task), j, uncached_region0[j], array[j], rounds);
					goto error;
				}
			}

			for (j = 0; j < REGION_WORDS; j++)
			{
				if (uncached_region1[j] != (value + j))
				{
					printk("\nError1! cpu %d uncached_region1[%d]:0x%08x != 0x%08x at Round%lu\n",
								task_cpu(task), j, uncached_region1[j], (value + j), rounds);
					goto error;
				}
			}
		}
		rounds++;
	}

error:
	busTest_unlocking(&cpu0rdBypassLock, &flags);
	kthread_stop(read_task);
	dma_free_coherent(dev, BYPASS_DRAM_TEST_SIZE, uncached_region0, dma_addr);

	return 0;
}

int doRdBypassWt_cpu2(void *arg)
{
	unsigned long *uncached_addr = NULL;
	dma_addr_t phy_addr = 0;
	u32 i = 0, j = 0;
	u32 rounds = 50000, time = 1;
	u32 idxMask = ((REGION_SIZE / sizeof(unsigned long)) - 1);
	unsigned long value = 0;
	unsigned long flags;
	struct device *dev = NULL;

	is_cpu0_1_bus_test = 1;
    SET_RCU_CPU_STALL_SUPRESS();
    
	while (!kthread_should_stop())
	{
		if ((dev=get_gdmpSram_dev()) == NULL)
		{
			printk("\nget_gdmpSram_dev failed\n");
			goto doRdBypassWt_cpu2_end;
		}

		uncached_addr = (unsigned long *)dma_alloc_coherent(dev, RDBYPASSWT_TEST_SIZE, &phy_addr, GFP_KERNEL);

		if (data0)
			rounds = data0;

		printk("\n%s Start...(with base virtAddr:0x%lx, physAddr:0x%lx)(writeRounds:%d)\n", 
		        __func__, uncached_addr, (unsigned long)phy_addr, rounds);


		rdBypassWt_uncached_addr = uncached_addr;

		wake_up_process(task_rdBypassWt_cpu3);

		busTest_locking(&cpu0rdBypassLock, &flags);

        prandom_bytes((void *) &value, sizeof(value));
        
		/* keep doing uncached write in 32-byte range */
		while (1)
		{
			for (i = 0; i < rounds; i++) {
			    #ifdef TCSUPPORT_CPU_AN7583
			    /* use 64-byte aligned address for R/W, otherwise rdBypassWt_cnt seldom counts. (SEC178071) */
                wtMeml((&(uncached_addr[0])), value+i);
			    #else
				wtMeml((&(uncached_addr[i&idxMask])), value+i);
				#endif
                dsb(st);
			}

			if (rdBypassWt_end)
				break;
		}
		busTest_unlocking(&cpu0rdBypassLock, &flags);

		dma_free_coherent(dev, RDBYPASSWT_TEST_SIZE, uncached_addr, phy_addr);

		printk("\n%s End\n", __func__);

doRdBypassWt_cpu2_end:
		rdBypassWt_end++;
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}
	return 0;
}

int doRdBypassWt_cpu3(void *arg)
{
	unsigned long *uncached_addr = NULL;
	unsigned long cnt0 = 0, cnt1 = 0, cpuTotalCnt_near = 0, cpuTotalCnt_far = 0;
	u32 i = 0, j = 0, k = 0,  tmpVal = 0;
	u32 bypassCnt0 = 0, bypassCnt1 = 0, bypassTotalCnt_near = 0, bypassTotalCnt_far = 0;
	unsigned long *tmp_addr = NULL;
	unsigned long flags;
	u32 is32NearByteTest = 1;
	u32 rounds=1000, time = 1;
	u32 testLen = ((REGION_SIZE / sizeof(unsigned long)) - 1);

	arm_pmu_init();
	busTest_locking(&cpu1rdBypassLock, &flags);
    
	while (!kthread_should_stop())
	{
		uncached_addr = rdBypassWt_uncached_addr;
		cpuTotalCnt_near = cpuTotalCnt_far = 0;
		bypassTotalCnt_near = bypassTotalCnt_far = 0;

		if ((uncached_addr==0) || (data2==0))
		{
			printk("\ncached_addr==0, exit %s!\n", __func__);
			goto doRdBypassWt_cpu3_end;
		}

		if (data1)
			rounds = data1;
		printk("%s Start...(readRounds:%d)\n\n", __func__,rounds);

		for (k = 0; k < (data2 << 1); k++)
		{
			if (is32NearByteTest)
			{
			    #ifdef TCSUPPORT_CPU_AN7583
			     /* use 64-byte aligned address for R/W, otherwise rdBypassWt_cnt seldom counts. (SEC178071) */
                tmp_addr = (uncached_addr);
			    #else
				tmp_addr = (uncached_addr + 1);
				#endif
			}
			else
			{
				tmp_addr = (uncached_addr + ((RDBYPASSWT_TEST_SIZE >> 1) / sizeof(unsigned long)));
			}

			bypassCnt0 = ecnt_get_rdbypass_cnt();
			cnt0 = read_c0_count();

			/* doing uncache read when cpu2 is doing uncache write */
			for (i = 0; i < rounds; i++)
			{
				for (j = 0; j < testLen; j++) {
					tmpVal = rdMeml(&tmp_addr[j]);
                    rmb();
                }
			}

			cnt1 = read_c0_count();
			bypassCnt1 = ecnt_get_rdbypass_cnt();
			
			if (is32NearByteTest)
			{
				is32NearByteTest = 0;
				if (cnt1 >= cnt0)
					cpuTotalCnt_near += (cnt1 - cnt0);
				else
					cpuTotalCnt_near += ((~(0UL) - cnt1) + cnt0);

				if (bypassCnt1 >= bypassCnt0)
					bypassTotalCnt_near += (bypassCnt1 - bypassCnt0);
				else
					bypassTotalCnt_near += ((~(0U) - bypassCnt1) + bypassCnt0);
			}
			else
			{
				is32NearByteTest = 1;
				if (cnt1 >= cnt0)
					cpuTotalCnt_far += (cnt1 - cnt0);
				else
					cpuTotalCnt_far += ((~(0UL) - cnt1) + cnt0);

				if (bypassCnt1 >= bypassCnt0)
					bypassTotalCnt_far += (bypassCnt1 - bypassCnt0);
				else
					bypassTotalCnt_far += ((~(0U) - bypassCnt1) + bypassCnt0);
			}
		}

		printk("cfg:%x mask:%x\n",
				ecnt_get_rdbypass_cfg(), ecnt_get_rdbypass_mask());
		#ifdef TCSUPPORT_CPU_AN7583
		 /* use 64-byte aligned address for R/W, otherwise rdBypassWt_cnt seldom counts. (SEC178071) */
        printk("uncached_addr:%lx tmp_addr_near:%lx tmp_addr_far:%lx\n",
				uncached_addr, (uncached_addr), ((uncached_addr + ((RDBYPASSWT_TEST_SIZE >> 1) / sizeof(unsigned long)))));
		#else
		printk("uncached_addr:%lx tmp_addr_near:%lx tmp_addr_far:%lx\n",
				uncached_addr, (uncached_addr + 1), ((uncached_addr + ((RDBYPASSWT_TEST_SIZE >> 1) / sizeof(unsigned long)))));
		#endif
		printk("for bypass-unhit: Average cpuCycles:%lu, rdBypassWtCnt:%u\n",
				((cpuTotalCnt_near / data2) << 1), (bypassTotalCnt_near / data2));
		printk("for bypass-hit:   Average cpuCycles:%lu, rdBypassWtCnt:%u\n",
				((cpuTotalCnt_far / data2) << 1), (bypassTotalCnt_far / data2));


	doRdBypassWt_cpu3_end:
		busTest_unlocking(&cpu1rdBypassLock, &flags);
		rdBypassWt_end++;
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}
	return 0;
}
#endif

static void doRdBypassWt_cnt(void)
{
    rdBypassWt_end=0;

    wake_up_process(task_rdBypassWt_cpu2);

    while (rdBypassWt_end!=2)
        schedule();

    kthread_stop(task_rdBypassWt_cpu2);
    kthread_stop(task_rdBypassWt_cpu3);

    printk("%s END\n", __func__);

    return;
}

int doSyncCmdTest(int argc, char *argv[], void *p)
{
    int syncCnt, syncCnt2;
    uint32 test_cnt, testCnt;

	if(argc != 3 || (strcmp(argv[1], "sync0"))){
		printk("Usage: sys syncCmdTest [sync0] testCnt\r\n");
		return 0;
	}

    testCnt = simple_strtoul(argv[2], NULL, 10);
    test_cnt = testCnt;

    printk("\n doSyncCmdTest Start ...\n");

  while (test_cnt) {
    
    if (strcmp(argv[1], "sync0")==0) {
        #ifndef TCSUPPORT_CPU_ARMV8
        /* disable SYNCCTL (so that sync cmd won't be sent to bus) */
        VPint(GCR_CTL_REG) &= ~(1<<SYNCCTL_BIT_SHIFT);
        
        syncCnt = (VPint(L2_SYNC_CNT_REG) & SYNC_CNT_BITS_MASK);
        __sync();

        while((VPint(L2_SYNC_CNT_REG) & SYNC_DONE_BIT_MASK) != 0); /*wait until sync done*/
        
        if ((VPint(L2_SYNC_CNT_REG) & SYNC_CNT_BITS_MASK) != syncCnt) {
            printk("\nERROR1: VPint(L2_SYNC_CNT_REG):%d != syncCnt:%d\n", (int)VPint(L2_SYNC_CNT_REG), syncCnt);
            return 0;
        }

        /* enable SYNCCTL (meaning that sync cmd will be sent to bus) */
        VPint(GCR_CTL_REG) |= (1<<SYNCCTL_BIT_SHIFT);
        
        syncCnt = (VPint(L2_SYNC_CNT_REG) & SYNC_CNT_BITS_MASK);
        syncCnt++;
        __sync();

        while((VPint(L2_SYNC_CNT_REG) & SYNC_DONE_BIT_MASK) != 0); /*wait until sync done*/

        syncCnt2 = (VPint(L2_SYNC_CNT_REG) & SYNC_CNT_BITS_MASK);
        if (syncCnt2 != syncCnt) {
            /* 1. during test, __sync() may be executed somewhere else, so if syncCnt2
                  is a little bit larger than syncCnt, that is accetable. 
               2. the max value of syncCnt2 is 255 (then become 0), so if syncCnt2 is
                  0 and syncCnt2 is 255, that is ok. */
            printk("\nWarning: syncCnt2:%d != syncCnt:%d\n", syncCnt2, syncCnt);
            return 0;
        }
        #endif
    }
    else {
        printk("\nWrong syncCmdTest Type:%s\n\n", argv[1]);
        return 0;
    }

  test_cnt--;
  }

    printk("\nsync0 test OK for ocp_l2 path Pass for %d times!\n", testCnt);

    return 0;
}

int doSramTest(int argc, char *argv[], void *p)
{
    sramTest();
    
    return 0;
}

void bustest_kthread_init(void)
{
    task_rdBypassWt_cpu2 = kthread_create(doRdBypassWt_cpu2, &data2, "rdBypassWt_cpu2");
    task_rdBypassWt_cpu3 = kthread_create(doRdBypassWt_cpu3, &data3, "rdBypassWt_cpu3");
    task_cpu_dram = kthread_create(doCpuDramTest, &data2, "cpuDramTest");
    task_cpu_cache_dram = kthread_create(doCpuCacheDramTest, &data1, "cpuCacheDramTest");

    #ifdef TCSUPPORT_CPU_ARMV8
    task_cpu0_access_Test = kthread_create(doCpu0AccessTest, &data0, "cpu0_access_Test");
    task_cpu1_access_Test = kthread_create(doCpu1AccessTest, &data1, "cpu1_access_Test");
    if (CONFIG_NR_CPUS==4) {
        task_cpu2_access_Test = kthread_create(doCpu2AccessTest, &data2, "cpu2_access_Test");
        task_cpu3_access_Test = kthread_create(doCpu3AccessTest, &data3, "cpu3_access_Test");
    }
    task_cpu0_dmaBlkCnt_Test = kthread_create(doCpu0dmaBlkCntTest, &data0, "cpu0_dmaBlkCnt_Test");
    task_cpu1_triggerDma = kthread_create(doCpu1triggerDma, &data1, "cpu1_triggerDma");
    task_cpu0_pbus_Test = kthread_create(doNewPBusTest, &data0, "cpu0_pbus_Test");
    task_cpu1_pbus_Test = kthread_create(doNewPBusTest, &data0, "cpu1_pbus_Test");
    if (CONFIG_NR_CPUS==4) {
        task_cpu2_pbus_Test = kthread_create(doNewPBusTest, &data0, "cpu2_pbus_Test");
        task_cpu3_pbus_Test = kthread_create(doNewPBusTest, &data0, "cpu3_pbus_Test");
    }

    kthread_bind(task_rdBypassWt_cpu2, 0); /* bind to CPU0 */
    kthread_bind(task_rdBypassWt_cpu3, 1); /* bind to CPU1 */
    kthread_bind(task_cpu0_access_Test, 0);     /* bind to CPU0 */
    kthread_bind(task_cpu1_access_Test, 1);     /* bind to CPU1 */
    if (CONFIG_NR_CPUS==4) {
        kthread_bind(task_cpu2_access_Test, 2);     /* bind to CPU2 */
        kthread_bind(task_cpu3_access_Test, 3);     /* bind to CPU3 */
    }
    kthread_bind(task_cpu0_dmaBlkCnt_Test, 0);  /* bind to CPU0 */
    kthread_bind(task_cpu1_triggerDma, 1);      /* bind to CPU1 */
    if (CONFIG_NR_CPUS==4)
        kthread_bind(task_cpu_dram, 2);         /* bind to CPU2 */
    else
        kthread_bind(task_cpu_dram, 1);         /* bind to CPU1 */
    kthread_bind(task_cpu_cache_dram, 1);   /* bind to CPU1 */
    kthread_bind(task_cpu0_pbus_Test, 0);     /* bind to CPU0 */
    kthread_bind(task_cpu1_pbus_Test, 1);     /* bind to CPU1 */
    if (CONFIG_NR_CPUS==4) {
        kthread_bind(task_cpu2_pbus_Test, 2);     /* bind to CPU2 */
        kthread_bind(task_cpu3_pbus_Test, 3);     /* bind to CPU3 */
    }

    #else

    task_bus_utili = kthread_create(doBusUtiliTest, &data0, "busUtiliTest");
    task_gdma_ram = kthread_create(doGdmaRamTest, &data1, "gdmaRamTest");
    task_cpu_reg_sram = kthread_create(doCpuRegSramTest, &data3, "cpuRegSramTest");
    
    kthread_bind(task_bus_utili, 0); /* bind to CPU0 */
    kthread_bind(task_gdma_ram, 1); /* bind to CPU1 */
    kthread_bind(task_cpu_cache_dram, 1); /* bind to CPU1 */
    kthread_bind(task_cpu_dram, 2); /* bind to CPU2 */
    kthread_bind(task_cpu_reg_sram, 3);  /* bind to CPU3 */ 
    kthread_bind(task_rdBypassWt_cpu2, 2); /* bind to CPU2 */
    kthread_bind(task_rdBypassWt_cpu3, 3); /* bind to CPU3 */
    #endif

	return;
}
