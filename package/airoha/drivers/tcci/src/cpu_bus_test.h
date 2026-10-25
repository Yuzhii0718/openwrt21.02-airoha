#ifndef _CPU_BUS_TEST_H
#define _CPU_BUS_TEST_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/tc3162/cmdparse.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/dma-mapping.h>
#include "tcci.h"

#define BUS_DRAM_TEST_SIZE	(1<<20)
#define LOG_BIT_SHIFT       4
#define LOG_BIT_MASK        0xf
#define CYCLES_TEST_NUM     100
#define PERF_TEST_NUM       10
#define S_4K                (0x1000)
#define S_32K               (0x8000)
#define S_48K               (0xc000)
#define S_128K              (0x20000)
#define S_256K              (0x40000)
#define S_512K              (0x80000)

#define SIZE_64K            0x10000
#define SIZE_128K           0x20000
#define SIZE_1M             0x100000

#define CPU_WR_DMA_RD_TSIZE	S_4K

#ifdef TCSUPPORT_CPU_ARMV8
#define REG_OFF (0x0)
#else
#define REG_OFF (0xA0000000)
#endif

#define SCREG_WR0           (0x1fb00280+REG_OFF)
#define SCREG_WR1           (0x1fb00284+REG_OFF)
#define DRAM_ADDR           0xa0000020
#define FE_SRAM_ADDR        (0x1FA30020+REG_OFF)
#define GDMP_SRAM_ADDR      (0x1fa40040+REG_OFF)
#define GDMP_SRAM_CAC_ADDR  0x9fa40040

#define SHARE_FEMEM_SEL     0xbfb00958
#define GDMA_BASE_ADDR      0xbfb30000
#define GDMA_CH0_SRC        (GDMA_BASE_ADDR)
#define GDMA_CH0_DST        (GDMA_BASE_ADDR+0x4)
#define GDMA_CH0_CTL0       (GDMA_BASE_ADDR+0x8)
#define GDMA_CH0_CTL1       (GDMA_BASE_ADDR+0xc)
#define GDMA_DONE_INT       (GDMA_BASE_ADDR+0x204)

#define ALL_FF              0xffffffff
#define FE_SRAM_PHY_SRC     0x1f310000
#define FE_SRAM_PHY_DST     0x1f320000
#define SRAM_TEST_SIZE  0x8
#define SRAM_TEST_WORDS (SRAM_TEST_SIZE>>2)
#define SRAM_TEST_START 0xbfa47400
#define SRAM_T_SIZE     (0x400)
#define SRAM_TEST_END   (SRAM_TEST_START+SRAM_T_SIZE)
#define SRAM_OFF_MASK   (0xffff)

#define BUS_UTI_CTL_REG     0xbfa000a0
#define BUS_UTI_STS_REG     0xbfa000a4
#define BUS_UTI_VLD_CNT     0xbfa000a8
#define BUS_UTI_IDEL_CNT    0xbfa000b0

#define UTIL_CNT_UNIT       0x400

#define PROFILING_DRAM_TEST_SIZE    32
#define PROFILING_TEST_NUM          1000

#define RBUS_CFG0                   0xbfb0094c
#define RBUS_CFG1                   0xbfb00950
#define RDBYPASSWT_CNT_REG      0xbfa000b8
#define GCR_CTL_REG         ((GCMP_BASE_ADDR | 0xA0000000) + 0x10)
#define L2_SYNC_CNT_REG     0xbfa000b4

#define L2_CACHE_TEST_SIZE          256
#define L2_CACHE_TEST_WORDS         (L2_CACHE_TEST_SIZE>>2)


#define RDBYPASSWT_CNT_MASK     0xff
#define RDBYPASSWT_TEST_SIZE    0x80

#define BYPASS_DRAM_TEST_SIZE   128
#define REGIONS_DISTANCE        64
#define REGION_SIZE             32
#define REGION_WORDS            (REGION_SIZE>>2)

#define SRAM_GDMP_PHY_BASE  (0x1fa40000)
#define SRAM_GDMP_BASE      (SRAM_GDMP_PHY_BASE+0xa0000000)
#define SRAM_S_ADDR_OFF     (0x4000)
#define SRAM_D_ADDR_OFF     (0x5000)
#define SYNCCTL_BIT_SHIFT   16
  
#define SYNC_CNT_BITS_MASK  (0xff)
#define SYNC_DONE_BIT_MASK  (0x80000000)

#define ALL_32BIT_ONE       (0xffffffff)

#define BUS_CACHE_DRAM_TEST_SIZE    (1<<20)
#define BUS_UTIL_LOG_CNT    (4)

#ifdef TCSUPPORT_CPU_ARMV8_64
#define ALL_BITS_ONE       (0xffffffffffffffff)
#else
#define ALL_BITS_ONE       (0xffffffff)
#endif

enum newPbusCase {
        npCase_no_multi_issue=0,
        npCase_r_single_normal,
        npCase_r_single_pcie,
        npCase_cycle_bypass_block_cnt,
        npCase_pcie_dev_mem_burst_perf,
        npCase_pbus_stress,
        npCase_pbus_timeout,
        npCase_last_one
};

#ifdef TCSUPPORT_CPU_ARMV8
/*
 * for arm64, go to linux-ecnt/arch/arm/include/asm/tc3162/arm64_pmu.h 
 * for arm32, go to linux-ecnt/arch/arm/include/asm/tc3162/arm_v7_pmu.h 
 */
#else /* MIPS */
#define _measure_read_base_by_addr(_cnt1,_cnt2,_addr)				\
({								\
	unsigned long _tmp;					\
    __asm__ __volatile__(   \
         "mfc0    %0, $9, 0\n\t"    \
         "la      %2, %3\n\t"    \
         "addiu   %2, 1\n\t"    \
         "mfc0    %1, $9, 0\n\t"    \
         : "=r" (_cnt1), "=r" (_cnt2), "=r" (_tmp)  \
         : "i" (_addr) \
     ); \
})

#define _measure_read_by_addr(_cnt1,_cnt2,_addr)				\
({								\
	unsigned long _tmp;					\
    __asm__ __volatile__(   \
         "mfc0    %0, $9, 0\n\t"    \
         "la      %2, %3\n\t"    \
         "lw      %2, 0(%2)\n\t"   \
         "addiu   %2, 1\n\t"    \
         "mfc0    %1, $9, 0\n\t"    \
         : "=r" (_cnt1), "=r" (_cnt2), "=r" (_tmp)  \
         : "i" (_addr) \
     ); \
})


#define _measure_write_base_by_addr(_cnt1,_cnt2,_addr)				\
({								\
	unsigned long _tmp;					\
    __asm__ __volatile__(   \
         "mfc0    %0, $9, 0\n\t"    \
         "la      %2, %3\n\t"    \
         "mfc0    %1, $9, 0\n\t"    \
         : "=r" (_cnt1), "=r" (_cnt2), "=r" (_tmp)  \
         : "i" (_addr) \
     ); \
})

#define _measure_write_by_addr(_cnt1,_cnt2,_addr)				\
({								\
	unsigned long _tmp;					\
    __asm__ __volatile__(   \
         "mfc0    %0, $9, 0\n\t"    \
         "la      %2, %3\n\t"    \
         "sw      %2, 0(%2)\n\t"   \
         "mfc0    %1, $9, 0\n\t"    \
         : "=r" (_cnt1), "=r" (_cnt2), "=r" (_tmp)  \
         : "i" (_addr) \
     ); \
})

#define _measure_base(_cnt1,_cnt2)				\
({								\
    __asm__ __volatile__(   \
        "mfc0    %0, $9, 0\n\t"    \
        "mfc0    %1, $9, 0\n\t"    \
         : "=r" (_cnt1), "=r" (_cnt2)  \
     ); \
})

#define _measure_sync(_cnt1,_cnt2)				\
({								\
    __asm__ __volatile__(   \
        "sync\n\t"    \
        "mfc0    %0, $9, 0\n\t"    \
        "sync\n\t"    \
        "mfc0    %1, $9, 0\n\t"    \
        : "=r" (_cnt1), "=r" (_cnt2)  \
     ); \
})

#define _Dcache_inv_by_addr(_addr)				\
({								\
    __asm__ __volatile__(   \
         "cache   0x11, %0\n\t"    \
         "sync\n\t"    \
         :              \
         : "i" (_addr)  \
     ); \
})

#define _L2cache_inv_by_addr(_addr)				\
({                              \
    __asm__ __volatile__(   \
        "cache   0x13, %0\n\t"    \
        "sync\n\t"    \
        :               \
        : "i" (_addr)  \
    ); \
})

#define _Dcache_wback_inv_by_addr(_addr)				\
({                              \
    __asm__ __volatile__(   \
        "cache   0x15, %0\n\t"    \
        "sync\n\t"    \
        :               \
        : "i" (_addr)  \
    ); \
})

#define _measure_Dcache_inv_by_addr(_cnt1,_cnt2,_addr)				\
({                              \
    __asm__ __volatile__(   \
        "sync\n\t"    \
        "mfc0    %0, $9, 0\n\t"    \
        "cache   0x11, %2\n\t"    \
        "sync\n\t"    \
        "mfc0    %1, $9, 0\n\t"    \
        : "=r" (_cnt1), "=r" (_cnt2)  \
        : "i" (_addr)  \
    ); \
})

#define _measure_L2cache_inv_by_addr(_cnt1,_cnt2,_addr)				\
({                              \
    __asm__ __volatile__(   \
        "sync\n\t"    \
        "mfc0    %0, $9, 0\n\t"    \
        "cache   0x13, %2\n\t"    \
        "sync\n\t"    \
        "mfc0    %1, $9, 0\n\t"    \
        : "=r" (_cnt1), "=r" (_cnt2)  \
        : "i" (_addr)  \
    ); \
})

#define _measure_Dcache_wback_inv_by_addr(_cnt1,_cnt2,_addr)				\
({                              \
    __asm__ __volatile__(   \
        "sync\n\t"    \
        "mfc0    %0, $9, 0\n\t"    \
        "cache   0x15, %2\n\t"    \
        "sync\n\t"    \
        "mfc0    %1, $9, 0\n\t"    \
        : "=r" (_cnt1), "=r" (_cnt2)  \
        : "i" (_addr)  \
    ); \
})

#define _measure_L2cache_wback_inv_by_addr(_cnt1,_cnt2,_addr)				\
({                              \
    __asm__ __volatile__(   \
        "sync\n\t"    \
        "mfc0    %0, $9, 0\n\t"    \
        "cache   0x17, %2\n\t"    \
        "sync\n\t"    \
        "mfc0    %1, $9, 0\n\t"    \
        : "=r" (_cnt1), "=r" (_cnt2)  \
        : "i" (_addr)  \
    ); \
})
#endif /* TCSUPPORT_CPU_ARMV8 */

#endif
