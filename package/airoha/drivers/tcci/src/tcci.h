
#ifndef _TCCI_H_
#define _TCCI_H_

#include <linux/types.h>

#define ADSLBUFINFO 4096

#define GDMA_CH_NUM         8
#define TC_GDMA_TEST_SIZE	1024

struct physAddr
{
    u32 dram_s_phys;
    u32 dram_d_phys;
    u32 sram_s_phys;
    u32 sram_d_phys;
    u32 testCase;
};

#ifdef TCSUPPORT_CPU_ARMV8

#define dram_phyAddr_to_dmaAddr(x) ((x&0x3fffffff)+0x80000000)

static inline uint32 regRead32(unsigned long reg)		\
{						  	\
	return readl((void*)reg);			  	\
}		

static inline void regWrite32(unsigned long reg, uint32 vlaue)	\
{                                                	\
	writel(vlaue, (void*)reg);                      	\
}
#endif

#define rdMeml(addr)			*(volatile unsigned long*)(addr)
#define wtMeml(addr,val)        *(volatile unsigned long*)(addr)=(unsigned long)(val)

#define rdMemW(addr)			*(volatile unsigned int*)(addr)
#define wtMemW(addr,val)        *(volatile unsigned int*)(addr)=(unsigned int)(val)

#define rdMemB(addr)			*(volatile unsigned char*)(addr)
#define wtMemB(addr,val)        *(volatile unsigned char*)(addr)=(unsigned char)(val)

/* support SysQdmaInit */
#define SUPPORT_QDMAINIT			(isEN7580 || isEN7523 || isEN7581 || isAN7583)


#endif /*_TCCI_H_*/
