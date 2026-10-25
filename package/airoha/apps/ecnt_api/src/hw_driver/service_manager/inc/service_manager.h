#ifndef __LINUX_MTD_CSMINIC_H__
#define __LINUX_MTD_CSMINIC_H__

/* Both files(linux-2.6.36/include/linux/mtd/fttdp_inic.h and bootrom_int/bootram/include/linux/mtd/fttdp_inic.h) 
 * contents must be synchronized. 
 */
 

//extern u32 GET_DRAM_SIZE(void);
//#define INIC_DRAM_SIZE				(GET_DRAM_SIZE() << 20)
#define INIC_DRAM_SIZE				0x4000000

#define INIC_RAM_SIMU_MAX_SIZE		(0x002BC000)	
#define INIC_RAM_BASE				(INIC_DRAM_SIZE-INIC_RAM_SIMU_MAX_SIZE)
#define INIC_KERNEL_OFFSET			(0x100)

#endif


