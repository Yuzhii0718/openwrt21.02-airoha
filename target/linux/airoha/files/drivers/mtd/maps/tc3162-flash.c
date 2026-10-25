#include <linux/init.h>
#include <linux/types.h>
#include <linux/root_dev.h>
#include <linux/kernel.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include <asm/tc3162/tc3162.h>
#include <linux/version.h>
#include <linux/module.h>

#include "spi/spi_controller.h"
#include "scu/ecnt_scu.h"
#include "scu/ecnt_bootargs.h"
#include <linux/mtd/rt_flash.h>
#include <trx/trx.h>

#ifdef TCSUPPORT_CPU_ARMV8
#include <linux/libfdt.h>
#endif

#if defined(TCSUPPORT_DUAL_IMAGE_ENHANCE) || defined(TCSUPPORT_OPENWRT)
#include "../chips/spiflash_tc3162.h"
#endif

#if defined(TCSUPPORT_INIC_CLIENT) || defined(TCSUPPORT_INIC_HOST) || defined(TCSUPPORT_DSL_PHYMODE)
#include <dsl_phy_global/fttdp_inic.h>
#endif

#if defined (TCSUPPORT_GPON_DUAL_IMAGE) || defined (TCSUPPORT_EPON_DUAL_IMAGE)
#include "flash_layout/tc_partition.h"
#endif



#ifdef TCSUPPORT_MTD_PARTITIONS_CMDLINE
#include <linux/slab.h>

#define STR_LEN	256
/* special size referring to all the remaining space in a partition */
#define SIZE_REMAINING UINT_MAX
#define SIZE_TO_GET (UINT_MAX-1)
#define OFFSET_CONTINUOUS UINT_MAX
#define OFFSET_BACK_FORWARD	(UINT_MAX-1)

/* error message prefix */
#define ERRP "mtd: "
#define TCLINUX "tclinux"
#define TCLINUX_SLAVE "tclinux_slave"
#define TCLINUX_REAL_SIZE "tclinux_real_size"
#define TCLINUX_SLAVE_REAL_SIZE "tclinux_slave_real_size"
#ifdef TCSUPPORT_OPENWRT
#define RESERVEAREA "art"
#else
#define RESERVEAREA "reservearea"
#endif
#define KERNEL_PART "kernel"
#define ROOTFS_PART "rootfs"
#define KERNEL_SLAVE_PART "kernel_slave"
#define ROOTFS_SLAVE_PART "rootfs_slave"
#define BOOTLOADER_PART "bootloader"
#define ROMFILE_PART "romfile"
#define BOOTLOADER_PART_STR "0[bootloader],"
#define ROMFILE_PART_STR "0[romfile],"

/*#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_OPENWRT)*/
extern int nand_flash_avalable_size;
/*#endif*/

/* mtdpart_setup() parses into here */
static struct mtd_partition *ecnt_parts;
static int num_parts = 0;
static int has_remaining_part_flag = 0;
static uint64_t tclinux_part_size = 0;			
static uint64_t tclinux_part_offset = OFFSET_CONTINUOUS;	
static int kernel_part_index = -1;
static int kernel_slave_part_index = -1;
#endif

#if defined(TCSUPPORT_SECURE_BOOT)
#include <boot/sHeader.h>
#endif

#define WINDOW_ADDR 0x1fc00000
#define WINDOW_SIZE 0x400000
#define BUSWIDTH 	2

#define TRX_LEN	256
#define KERNEL_PARTITION(a)	(a + TRX_LEN)  //Include trx header
#define ROOTFS_PARTITION(a)	((a + 0x10000) & ~(0x10000-1))
//#define ROOTFS_PARTITION(a)	(a)

#ifdef CONFIG_DUAL_IMAGE
#ifdef TCSUPPORT_FREE_BOOTBASE
#define FLAG_ADDR		(START_ADDR - 1)
#else
#define	FLAG_ADDR		0x8001ffff
#endif
#if defined(TCSUPPORT_CT_DUAL_IMAGE)
#if defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_RESERVEAREA_EXTEND)
#ifdef TCSUPPORT_NAND_FLASH//tony add
#define	MAIN_IMAGE_SIZE		(0x1000000)//16M for tclinux.bin
#else
#if defined(TCSUPPORT_CT_PON_CAU)
#define	MAIN_IMAGE_SIZE		(0x3A0000)
#else
#define	MAIN_IMAGE_SIZE		(0x7B0000)
#endif
#endif
#else
#define	MAIN_IMAGE_SIZE		(0x780000)
#endif

#ifdef TCSUPPORT_NAND_FLASH
#define	SLAVE_IMAGE_OFFSET	(0x1080000)
#else
#if defined(TCSUPPORT_BOOTROM_LARGE_SIZE)
#if defined(TCSUPPORT_RESERVEAREA_EXTEND)
#if defined(TCSUPPORT_CT_PON_CAU)
#define	SLAVE_IMAGE_OFFSET	(0x3D0000)
#else
#define	SLAVE_IMAGE_OFFSET	(0x7E0000)
#endif
#else
#define	SLAVE_IMAGE_OFFSET	(0x7b0000)
#endif
#endif
/*NAND_FLASH*/
#endif

#ifdef TCSUPPORT_NAND_FLASH
#define	SLAVE_IMAGE_SIZE	(0x1000000)
#else
#if defined(TCSUPPORT_RESERVEAREA_EXTEND)
#if defined(TCSUPPORT_CT_PON_CAU)
#define	SLAVE_IMAGE_SIZE	(0x3A0000)
#else
#define	SLAVE_IMAGE_SIZE	(0x3B0000)
#endif
#else
#define	SLAVE_IMAGE_SIZE	(0x400000)
#endif
/*NAND_FLASH*/
#endif

#else
#define	MAIN_IMAGE_SIZE		(0x680000)
#if defined(TCSUPPORT_BOOTROM_LARGE_SIZE)
#define	SLAVE_IMAGE_OFFSET	(0x6b0000)
#endif
#define	SLAVE_IMAGE_SIZE	(0x500000)
#endif
#else
#if defined(TCSUPPORT_CUC_DUAL_IMAGE)
#define	MAIN_IMAGE_SIZE		(0x700000)
#if defined(TCSUPPORT_BOOTROM_LARGE_SIZE)
#define	SLAVE_IMAGE_OFFSET	(0x730000)
#endif
#define	SLAVE_IMAGE_SIZE	(0x700000)
#else
#ifdef TCSUPPORT_NAND_BMT
#define	MAIN_IMAGE_SIZE		(0x1000000)//16M for tclinux.bin
#define	SLAVE_IMAGE_OFFSET	(0x7E0000)
#define	SLAVE_IMAGE_SIZE	(0x3B0000)
#else
#define	MAIN_IMAGE_SIZE		(0x500000)
#define	SLAVE_IMAGE_OFFSET	(0x520000)
#define	SLAVE_IMAGE_SIZE	(0x400000)
#endif
#endif
#endif
#endif

#if defined(TCSUPPORT_CT_OSGI)
#define JOYME_ADD_JFFS2		(0x2000000)
#define JOYME_ADD_YAFS2		(0x2400000)
#else
#define JOYME_ADD_JFFS2		(0)
#define JOYME_ADD_YAFS2		(0)
#endif

#ifdef TCSUPPORT_JFFS2_BLOCK
#define BLOCK_FOR_JFFS2		(0x400000)
#if defined(TCSUPPORT_RESERVEAREA_EXTEND)
#define	JFFS2_OFFSET		(0xb90000)
#else
#define	JFFS2_OFFSET		(0xbc0000)
#endif
#endif

void tc3162_check_mtdpart_str(char *dst, char *src);
/* frankliao added 20101215 */
extern int nand_flash_avalable_size;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 36)
extern int __devinit ra_nand_init(void);
extern void __devinit ra_nand_remove(void);
#else
extern int ra_nand_init(void);
extern void ra_nand_remove(void);

#endif

 #ifdef TCSUPPORT_MTD_ENCHANCEMENT
 #if 0
 #if defined ( TCSUPPORT_RESERVEAREA_1_BLOCK)
 #define BLOCK_NUM_FOR_RESERVEAREA 1
 #elif defined(TCSUPPORT_RESERVEAREA_2_BLOCK)
  #define BLOCK_NUM_FOR_RESERVEAREA 2
 #elif defined(TCSUPPORT_RESERVEAREA_3_BLOCK)
 #define BLOCK_NUM_FOR_RESERVEAREA 3
#else //TCSUPPORT_RESERVEAREA_4_BLOCK
 #define BLOCK_NUM_FOR_RESERVEAREA 4
#endif
#endif
#if ((TCSUPPORT_RESERVEAREA_BLOCK != 1)&& (TCSUPPORT_RESERVEAREA_BLOCK != 2)&& (TCSUPPORT_RESERVEAREA_BLOCK !=3)&& (TCSUPPORT_RESERVEAREA_BLOCK !=4) && (TCSUPPORT_RESERVEAREA_BLOCK !=5) && (TCSUPPORT_RESERVEAREA_BLOCK !=6) && (TCSUPPORT_RESERVEAREA_BLOCK !=7)&& (TCSUPPORT_RESERVEAREA_BLOCK !=9))
#define BLOCK_NUM_FOR_RESERVEAREA 4
#else
 #define BLOCK_NUM_FOR_RESERVEAREA TCSUPPORT_RESERVEAREA_BLOCK
#endif
#endif

static struct mtd_info *tc3162_mtd_info;

static struct map_info tc3162_map = {
       .name = "tc3162",
       .size = WINDOW_SIZE,
       .bankwidth = BUSWIDTH,
       .phys = WINDOW_ADDR,
};
#ifdef TCSUPPORT_SQUASHFS_ADD_YAFFS
#define	SQUASHFS_ADD_YAFFS_SIZE (0x500000 + JOYME_ADD_YAFS2)	//5M
#endif

#ifdef TCSUPPORT_OPENWRT
static struct mtd_partition tc3162_parts[] = {
	{									 	/* First partition */
		  name 	     : "u-boot",	 	/* Bootloader section MTD0*/
		  size	  	 : 0x00080000, 		 	/* Size  128 + 64(Romfile) KB */ 
		  offset     : 0				 	/* Offset from start of flash- location 0x0*/
//		  mask_flags : MTD_WRITEABLE	 	/* This partition is not writable */
	},
	{									 	/* Third partition */
		  name       : "kernel",		 	/* Kernel section MTD1*/
		  size		 :	MTDPART_SIZ_FULL, //0x000d0000,
		/* 
		 * frank modify for nand flash support
		 * for nand flash, romfile partition is put in the last block,
		 * so the kernel partition cannot append after romfile section
		 */
#ifdef TCSUPPORT_CPU_ARMV8
		  offset	 : 0xc0000  /* Append after bootloader section */
#else
		  offset     : 0x80000	/* Append after bootloader section */
#endif		  
	},
	{									 	/* rootfs partition MTD2*/
		  name       : "rootfs", 		 	/* Root filesystem section /rom + /overlay (MTD3) */
		  size	     : MTDPART_SIZ_FULL, 	/* Occupy rest of flash - art size */
		  offset     : MTDPART_OFS_APPEND 	/* Append after kernel section */
	},
	{
		  name       : "rootfs_data",            /* rootfs_data partition MTD3*/	
		  size       : MTDPART_SIZ_FULL,    /* Occupy rest of flash */
		  offset     : 0x00030000   
	}
 	,
	 {
		  name       : "art",            /*art partition mtd4, 64KB*/	
		  size       : 0x00080000,   /* occupy the last 1 blocks */
		  offset     : MTDPART_OFS_APPEND   
	}
	,
	{
		  name       : "firmware",            /* firmware partition MTD5 kernel+rootfs*/	
		  size       : MTDPART_SIZ_FULL - 0x40000,    /* Occupy rest of flash - art size - bootloader size */
		  offset     : 0x00080000   
	}	
};
#else
static struct mtd_partition tc3162_parts[] = {
	{									 	/* First partition */
		  name 	     : "bootloader",	 	/* Bootloader section */
#ifdef TCSUPPORT_BOOTROM_LARGE_SIZE
		  size	  	 : 0x00020000, 		 	/* Size =128k */		
#else
		  size	  	 : 0x00010000, 		 	/* Size  */
#endif
		  offset     : 0				 	/* Offset from start of flash- location 0x0*/
//		  mask_flags : MTD_WRITEABLE	 	/* This partition is not writable */
	},
	{									 	/* Second partition */
		  name       : "romfile",			/* config filesystem section */
		  size	     :  0x00010000,		 	/* Size */
		  offset     : MTDPART_OFS_APPEND	/* Append after bootloader section */
	},
	{									 	/* Third partition */
		  name       : "kernel",		 	/* Kernel section */
#ifdef CONFIG_MTD_PURE_BRIDGE
		  size	     :  0x000a0000,		 	/* Size */
#else
		  size		 :	0x000d0000,
#endif
		/* 
		 * frank modify for nand flash support
		 * for nand flash, romfile partition is put in the last block,
		 * so the kernel partition cannot append after romfile section
		 */
		  offset     : 0x20000	/* Append after bootloader section */
//		  offset     : MTDPART_OFS_APPEND	/* Append after bootloader section */
	},
	{									 	/* Fourth partition */
		  name       : "rootfs", 		 	/* Root filesystem section */
		  size	     : MTDPART_SIZ_FULL, 	/* Occupy rest of flash */
		  offset     : MTDPART_OFS_APPEND 	/* Append after kernel section */
	},
	{
		  name       : "tclinux",            /* tclinux partition */	
		  size       : MTDPART_SIZ_FULL,    /* Occupy rest of flash */
		  offset     : 0x00020000   
	}
#ifdef CONFIG_DUAL_IMAGE
	,
	{
		  name       : "kernel_slave",            /* tclinux slave partition */	
#ifdef CONFIG_MTD_PURE_BRIDGE
		  size	     :  0x000a0000,		 	/* Size */
#else
		  size		 :	0x000d0000,
#endif	  
		   offset	 :  SLAVE_IMAGE_OFFSET   
	},
	{
		  name       : "rootfs_slave",            /* tclinux slave partition */	
		  size       : MTDPART_SIZ_FULL,    /* Occupy rest of flash */
		  offset     : MTDPART_OFS_APPEND   
	},
	{
		  name       : "tclinux_slave",            /* tclinux slave partition */	
		  size       : MTDPART_SIZ_FULL,    /* Occupy rest of flash */
		  offset 	 : SLAVE_IMAGE_OFFSET	
	}
#endif 
#ifdef TCSUPPORT_INIC_HOST
	,		
	{
		  name       : INIC_CLIENT_ROMFILE_NAME,
		  size       : INIC_CLIENT_ROMFILE_SIZE,
		  offset     : MTDPART_OFS_APPEND   
	}
#endif
#ifdef TCSUPPORT_JFFS2_BLOCK
   ,
	{
		 name		: "jffs2",			/*test partition */    
		 size		: BLOCK_FOR_JFFS2, //MTDPART_SIZ_FULL,    /* Occupy rest of flash */
		 offset 	: JFFS2_OFFSET   
	}
#endif
#if defined(TCSUPPORT_CT_OSGI)
#if defined(TCSUPPORT_SQUASHFS_ADD_YAFFS)
	 ,
	  {
		   name 	  : "yaffs",			/*nand yaffs partition */	 
		   size 	  : SQUASHFS_ADD_YAFFS_SIZE,   /* occupy 5M*/
		   offset	  : MTDPART_OFS_APPEND	 
	 }
#endif
#endif
 #ifdef TCSUPPORT_MTD_ENCHANCEMENT
 	,
	 {
		  name       : "reservearea",            /*test partition */	
		  size       : 0x00040000,   /* occupy the last 4 blocks */
		  offset     : MTDPART_OFS_APPEND   
	}
#endif
#if defined(TCSUPPORT_MULTI_BOOT) && !defined(TCSUPPORT_C1_ZY_SFU)
 	,
	 {
		  name       : "romd",            /*test partition */	
		  size       : 0x00010000,   /* occupy one block*/
		  offset     : MTDPART_OFS_APPEND   
	}
#endif
#if !defined(TCSUPPORT_CT_OSGI)
#if defined(TCSUPPORT_SQUASHFS_ADD_YAFFS)
	,
	 {
		  name       : "yaffs",            /*nand yaffs partition */	
		  size       : SQUASHFS_ADD_YAFFS_SIZE,   /* occupy 5M*/
		  offset     : MTDPART_OFS_APPEND   
	}
#endif
#endif

};
#endif
extern int add_mtd_partitions(struct mtd_info *, const struct mtd_partition *, int);
extern int del_mtd_partitions(struct mtd_info *);

static int tc3162_parts_size = sizeof(tc3162_parts) / sizeof(tc3162_parts[0]);

static struct mtd_info *get_mtd_named(char *name)
{
	int i;
	struct mtd_info *mtd;

	for (i = 0; i < 32; i++) {
		mtd = get_mtd_device(NULL, i);
		if (mtd) {
			if (strcmp(mtd->name, name) == 0)
				return(mtd);
			put_mtd_device(mtd);
		}
	}
	return(NULL);
}

/******************************************************************************
 Function:		tc3162_map_init
 Description:	It's used to init tc3162_map
 Input:		void
 Return:	    	0: Success,  -EIO: fail
******************************************************************************/
int tc3162_map_init(void){	
#if defined(TCSUPPORT_INIC_CLIENT) || defined(TCSUPPORT_DSL_PHYMODE)
	u_int32_t ram_base;
#endif
	uint32 tmpVal;

	/*add address mapping on 7510. Pork*/
	if(GET_HIR() >= MT751020_HIR){
#ifdef TCSUPPORT_CPU_ARMV8
		tmpVal = GET_SMB0ALS();
#else
		tmpVal = regRead32(0xbfb00038);
#endif
		tmpVal &= 0xffe0e0e0;
		tmpVal |= 0x80070f00;
#ifdef TCSUPPORT_CPU_ARMV8
		SET_SMB0ALS(tmpVal);
#else
		regWrite32(0xbfb00038,tmpVal);
#endif
		//VPint(0xbfb00038) |= 0x80070F00;
		#ifdef TCSUPPORT_DSL_PHYMODE
			ram_base = DSL_PHYMODE_DRAM_SIZE - DSL_PHYMODE_RAM_SIMU_MAX_SIZE;
			printk("tc3162: dsl_phymode simulated flash device 0x%08x at 0x%08x\n", DSL_PHYMODE_RAM_SIMU_MAX_SIZE, ram_base);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
			tc3162_map.virt = ioremap(ram_base, DSL_PHYMODE_RAM_SIMU_MAX_SIZE);
#else
			tc3162_map.virt = ioremap_nocache(ram_base, DSL_PHYMODE_RAM_SIMU_MAX_SIZE);
#endif
			tc3162_map.phys = ram_base;
			tc3162_map.size = DSL_PHYMODE_RAM_SIMU_MAX_SIZE;
		#elif TCSUPPORT_INIC_CLIENT
			ram_base = 0x800000 * (1 << (((GET_HWCONF() >> 13) & 0x7) - 1))- INIC_CLIENT_RAM_SIMU_MAX_SIZE;
			printk("tc3162: iNIC simulated flash device 0x%08x at 0x%08x\n", INIC_CLIENT_RAM_SIMU_MAX_SIZE, ram_base);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
			tc3162_map.virt = ioremap(ram_base, INIC_CLIENT_RAM_SIMU_MAX_SIZE);
#else
			tc3162_map.virt = ioremap_nocache(ram_base, INIC_CLIENT_RAM_SIMU_MAX_SIZE);
#endif
			tc3162_map.phys = ram_base;
			tc3162_map.size = INIC_CLIENT_RAM_SIMU_MAX_SIZE;
		#else
			#ifdef TCSUPPORT_CPU_ARMV8
				tc3162_map.phys = 0x00000000;
				tc3162_map.size = 0x2000000;
				printk("tc3162: flash device 0x%08x at 0x%08x\n", tc3162_map.size, tc3162_map.phys);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
				tc3162_map.virt = ioremap(tc3162_map.phys, tc3162_map.size);
#else
				tc3162_map.virt = ioremap_nocache(tc3162_map.phys, tc3162_map.size);
#endif				
			#else
			printk("tc3162: flash device 0x%08x at 0x%08x\n", 0x1000000, 0x1c000000);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
			tc3162_map.virt = ioremap(0x1c000000, 0x1000000);
#else
			tc3162_map.virt = ioremap_nocache(0x1c000000, 0x1000000);
#endif
			tc3162_map.phys = 0x1c000000;
			tc3162_map.size = 0x1000000;
		#endif
		#endif
		#ifndef TCSUPPORT_CPU_ARMV8
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		ioremap(WINDOW_ADDR, WINDOW_SIZE);
#else
		ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);
#endif
		#endif
	}
	/*add 8M 16M flash support. shnwind*/
	else if (isTC3162U || isTC3182 || isRT65168 || isRT63165 || isRT63365 || isRT63260){
//		header = (unsigned int *)0xb0020000;
		/*enable addr bigger than 4M support.*/
#ifdef TCSUPPORT_CPU_ARMV8
		tmpVal = GET_SMB0ALS();
		tmpVal |= 0x80000000;
		SET_SMB0ALS(tmpVal);
#else
		VPint(0xbfb00038) |= 0x80000000;
#endif
		printk("tc3162: flash device 0x%08x at 0x%08x\n", 0x1000000, 0x10000000);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		tc3162_map.virt = ioremap(0x10000000, 0x1000000);
#else
		tc3162_map.virt = ioremap_nocache(0x10000000, 0x1000000);
#endif
		tc3162_map.phys = 0x10000000;
		tc3162_map.size = 0x1000000;
		ioremap(WINDOW_ADDR, WINDOW_SIZE);
	}else{
	
//		header = (unsigned int *)0xbfc20000;
		printk("tc3162: flash device 0x%08x at 0x%08x\n", WINDOW_SIZE, WINDOW_ADDR);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		tc3162_map.virt = ioremap(WINDOW_ADDR, WINDOW_SIZE);
#else
		tc3162_map.virt = ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);
#endif
	}
	if (!tc3162_map.virt) {
   		printk("tc3162: Failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(&tc3162_map);

	return 0;
}

static int tc3162_mtd_info_init(void){	
	#if defined(TCSUPPORT_INIC_CLIENT) || defined(TCSUPPORT_DSL_PHYMODE)
		printk("MT75XX: INIC mode\n");
		tc3162_mtd_info = do_map_probe("map_ram", &tc3162_map);
	#else
		/* check if boot from SPI flash */
#ifdef TCSUPPORT_EMMC
		if(GET_NP_SCU_EMMC()) {
			tc3162_mtd_info = get_mtd_device(NULL, 0);
			tc3162_mtd_info->usecount = 0;
			mtd_device_unregister(tc3162_mtd_info);
		} else
#endif
		if (IS_NANDFLASH) {
			tc3162_mtd_info = do_map_probe("nandflash_probe", &tc3162_map);
		} else if (IS_SPIFLASH) {
			tc3162_mtd_info = do_map_probe("spiflash_probe", &tc3162_map);
		} else {
			tc3162_mtd_info = do_map_probe("cfi_probe", &tc3162_map);
		}
	#endif

	if (!tc3162_mtd_info) {
		#if defined(TCSUPPORT_INIC_CLIENT) || defined(TCSUPPORT_DSL_PHYMODE)
		printk("iNIC flash fail\n");
		#endif
		iounmap(tc3162_map.virt);
		return -ENXIO;
	}

  	tc3162_mtd_info->owner = THIS_MODULE;
	
	return 0;
}

static void tc3162_put_rootfs(void){
	struct mtd_info *mtd;
	
#ifdef CONFIG_DUAL_IMAGE
#ifdef TCSUPPORT_CPU_ARMV8
	const char flagvalue = 1;//not change!!because we use this flag to judge which image
	char tmp[8] = {0};
	tmp[0] = get_bootflag();
#else
#if !defined(TCSUPPORT_FH_UNIFIED_PLATFORM) 
	char *bufaddr = (char*)FLAG_ADDR;
	const char flagvalue = 1;//not change!!because we use this flag to judge which image
	char tmp[8] = {0};

	memcpy(tmp,(char*)bufaddr,sizeof(char));
#endif
#endif
	if(flagvalue == tmp[0])
	{
#if !defined(TCSUPPORT_FH_ENV)	
		printk("\r\nrootfs_slave");
		mtd = get_mtd_named("rootfs_slave");
#endif
	}
	else
	{
#if !defined(TCSUPPORT_FH_ENV)
		printk("\r\nrootfs");
		mtd = get_mtd_named("rootfs");
#endif
	}
#else
	mtd = get_mtd_named("rootfs");
	#endif	
	if (mtd) {
		ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, mtd->index);
		put_mtd_device(mtd);
	}
}

#ifndef TCSUPPORT_MTD_PARTITIONS_CMDLINE
int tc3162_add_partitions(void){
	unsigned int *header;
	unsigned int addr;
	#if defined(TCSUPPORT_BOOTROM_LARGE_SIZE)
		u_int32_t tclinux_flash_offset = 0x30000;
	#else
		u_int32_t tclinux_flash_offset = 0x20000;
	#endif
	
	#ifdef TCSUPPORT_SQUASHFS_ADD_YAFFS
		u_int32_t nand_yaffs_size = SQUASHFS_ADD_YAFFS_SIZE;
	#else
		u_int32_t nand_yaffs_size = 0;
	#endif
	
	#if defined(CONFIG_DUAL_IMAGE) || defined(TCSUPPORT_MTD_ENCHANCEMENT) || defined(TCSUPPORT_MULTI_BOOT) || defined(TCSUPPORT_NAND_BADBLOCK_CHECK)
		int i = 0;
	#endif
	#ifdef CONFIG_DUAL_IMAGE
		char *bufaddr = (char*)FLAG_ADDR;
		const char flagvalue = 1;//not change!!because we use this flag to judge which image
		char tmp[8] = {0};
	#endif
	//#if defined(CONFIG_DUAL_IMAGE) && (defined(TCSUPPORT_MTD_ENCHANCEMENT) ||defined(TCSUPPORT_MULTI_BOOT))
#if defined(CONFIG_DUAL_IMAGE)
#ifdef TCSUPPORT_DUAL_IMAGE_ENHANCE	
		u_int32_t tclinux_slave_offset = offset+tclinux_flash_offset;
#else
		u_int32_t tclinux_slave_offset = MAIN_IMAGE_SIZE+tclinux_flash_offset;	
#endif
		u_int32_t tclinux_slave_size = 0;
#endif
#if defined(TCSUPPORT_MTD_ENCHANCEMENT) || defined(TCSUPPORT_MULTI_BOOT)
		u_int32_t tclinux_size = 0;
#endif
#if defined(TCSUPPORT_CT_DUAL_IMAGE) || defined(TCSUPPORT_CUC_DUAL_IMAGE) || defined(TCSUPPORT_NAND_BMT)
		struct trx_header *trx = NULL;
		char *trx_addr;
		unsigned int magic;
#endif
	
  #ifdef TCSUPPORT_NAND_RT63368
		header = (unsigned int *)(flash_base + 0x40000);
  #elif defined(TCSUPPORT_NAND_BADBLOCK_CHECK)	
		header = (unsigned int *)(flash_base + 0x280000);
  #else
		/* frankliao added 20101223 */
  #if defined(TCSUPPORT_INIC_CLIENT) || defined(TCSUPPORT_DSL_PHYMODE)
		header = (unsigned int *)(flash_base + INIC_CLIENT_BOOTLOADER_SIZE + INIC_CLIENT_ROMFILE_SIZE);
  #else
		header = (unsigned int *)(flash_base + tclinux_flash_offset);
  #endif
  #endif

	#ifdef TCSUPPORT_BOOTROM_LARGE_SIZE
	if (IS_NANDFLASH) {
		if(tc3162_mtd_info->erasesize >= 0x20000){
			/*tclinux offset is 0x80000 for 128k&256k block size*/
			//tclinux_flash_offset = tc3162_mtd_info->erasesize*2;
			tclinux_flash_offset = 0x40000*2;
			
#if defined(CONFIG_DUAL_IMAGE)
#ifdef TCSUPPORT_DUAL_IMAGE_ENHANCE	
			tclinux_slave_offset = offset+tclinux_flash_offset;
#else
			tclinux_slave_offset = MAIN_IMAGE_SIZE+tclinux_flash_offset;	
#endif
#endif
			header = (unsigned int *)(flash_base + tclinux_flash_offset);
		}
	}
	#endif

    #if defined(TCSUPPORT_NAND_BADBLOCK_CHECK) || defined(TCSUPPORT_NAND_RT63368)
    for(i= 0; i < tc3162_parts_size; i++)
    {
        if(!strcmp(tc3162_parts[i].name, "bootloader"))
        {
            tc3162_parts[i].size = 0x20000;
        }

        if(!strcmp(tc3162_parts[i].name, "romfile"))
        {
            tc3162_parts[i].size = 0x20000;
            #ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
            tc3162_parts[i].offset = 0xe0000;
            #else
            tc3162_parts[i].offset = 0x20000;
            #endif
        }

    }
    #endif

	#ifdef CONFIG_DUAL_IMAGE
	for(i= 0; i < tc3162_parts_size; i++)
	{
		if(!strcmp(tc3162_parts[i].name,"kernel"))
		{
			addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 20 * sizeof(unsigned int)));  
			tc3162_parts[i].size = KERNEL_PARTITION( addr );
//			tc3162_parts[i].size = KERNEL_PARTITION(header[20]);
			addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 21 * sizeof(unsigned int)));   
			tc3162_parts[i+1].size = ROOTFS_PARTITION( addr );
//			tc3162_parts[i+1].size = ROOTFS_PARTITION(header[21]);
			#if defined(TCSUPPORT_MTD_ENCHANCEMENT) || defined(TCSUPPORT_MULTI_BOOT)
			tclinux_size = tc3162_parts[i].size+tc3162_parts[i+1].size;
			#endif

			#ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
            tc3162_parts[i].offset = 0x280000;
            tc3162_parts[i+2].offset = tc3162_parts[i].offset;
            #elif defined(TCSUPPORT_NAND_RT63368)
            tc3162_parts[i].offset = 0x40000;
            tc3162_parts[i+2].offset = tc3162_parts[i].offset;
			#endif
		}
		if(!strcmp(tc3162_parts[i].name,"kernel_slave"))
		{
#ifdef TCSUPPORT_DUAL_IMAGE_ENHANCE  
			unsigned int *header_slave = (unsigned int *)(flash_base + tclinux_flash_offset + offset);
			tc3162_parts[i].offset = offset + tclinux_flash_offset;
			tc3162_parts[i+2].offset = offset + tclinux_flash_offset;

#elif defined(TCSUPPORT_NAND_BADBLOCK_CHECK)
            unsigned int *header_slave = (unsigned int *)(flash_base + 0x2280000);
            tc3162_parts[i].offset = 0x2280000;
            tc3162_parts[i+2].offset = tc3162_parts[i].offset;
#else
			unsigned int *header_slave = (unsigned int *)(flash_base + tclinux_flash_offset + MAIN_IMAGE_SIZE);
//			unsigned int *header_slave = (unsigned int *)(0xb0020000+0x500000);
			tc3162_parts[i].offset = MAIN_IMAGE_SIZE + tclinux_flash_offset;
			tc3162_parts[i+2].offset = MAIN_IMAGE_SIZE + tclinux_flash_offset;
#endif
//			tc3162_parts[i].size = KERNEL_PARTITION(header_slave[20]);
//			tc3162_parts[i+1].size = ROOTFS_PARTITION(header_slave[21]);
			addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header_slave + 20 * sizeof(unsigned int)));
//			tc3162_parts[i].size = KERNEL_PARTITION(header_slave[20]);
			tc3162_parts[i].size = KERNEL_PARTITION( addr );

			addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header_slave + 21 * sizeof(unsigned int)));		
			tc3162_parts[i+1].size = ROOTFS_PARTITION( addr );
//			tc3162_parts[i+1].size = ROOTFS_PARTITION(header_slave[21]);

		 #if defined(TCSUPPORT_MTD_ENCHANCEMENT) || defined(TCSUPPORT_MULTI_BOOT)
			tclinux_slave_offset = tc3162_parts[i].offset;
		 	tclinux_slave_size = tc3162_parts[i].size + tc3162_parts[i+1].size;
		 #endif

#if defined(TCSUPPORT_CT_DUAL_IMAGE) || defined(TCSUPPORT_CUC_DUAL_IMAGE) || defined(TCSUPPORT_NAND_BMT)
		trx = (struct trx_header *)header_slave;
		trx_addr = &(trx->magic);
		magic = READ_FLASH_DWORD(trx_addr);
		if(magic !=TRX_MAGIC2){
			tclinux_slave_offset = SLAVE_IMAGE_OFFSET;
		 	tclinux_slave_size = SLAVE_IMAGE_SIZE;
			tc3162_parts[i].size = 0;	//no slave image
			tc3162_parts[i+1].size = 0; //no slave image
		}
#endif

		}
		if (IS_NANDFLASH) {	
			/* frankliao enhance 20110112, for nand flash romfile  */
			if(!strcmp(tc3162_parts[i].name,"tclinux_slave")) {
				/* frankliao add 20110112, for 128K block size nand flash romfile */
				if (tc3162_mtd_info->erasesize >= 0x20000) {
				#ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
                    tc3162_parts[i].size = 0x2000000;
				#else
					tc3162_parts[i].size = tc3162_mtd_info->size - nand_yaffs_size - 6*tc3162_mtd_info->erasesize - tclinux_slave_offset;
				#endif
				} else {
					tc3162_parts[i].size = tc3162_mtd_info->size - tclinux_slave_offset;
				}	
			}	
	
			/* 
			 * frankliao enhance 20110112 
			 * the tclinux partition start from 0x20000, end at tclinux_slave_offset 
			 */
			if(!strcmp(tc3162_parts[i].name,"tclinux")) {
			#ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
			    tc3162_parts[i].size = 0x2000000;
			#else
				tc3162_parts[i].size = tclinux_slave_offset - tclinux_flash_offset;
			#endif
			}	
		}		
	}
	#else 
//	tc3162_parts[2].size = KERNEL_PARTITION(header[20]);
//	tc3162_parts[2].size = KERNEL_PARTITION(header[21]);
	addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 20 * sizeof(unsigned int)));
	tc3162_parts[2].size = KERNEL_PARTITION( addr );
	addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 21 * sizeof(unsigned int)));
	tc3162_parts[3].size = ROOTFS_PARTITION( addr );

	#if defined(TCSUPPORT_NAND_BADBLOCK_CHECK) || defined(TCSUPPORT_NAND_RT63368)
	#ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
        tc3162_parts[2].offset = 0x280000;
        #else
        tc3162_parts[2].offset = 0x40000;
        #endif
        tc3162_parts[4].offset = tc3162_parts[2].offset;
	#endif

	if (IS_NANDFLASH) {
		/* frankliao added 20110112, for 128K block size NAND Flash */
		if (tc3162_mtd_info->erasesize >= 0x20000) {
		#ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
		    tc3162_parts[4].size = 0x2000000;
		#else
			/* the last block store nand flash romfile */
			tc3162_parts[4].size = tc3162_mtd_info->size - nand_yaffs_size - 6*tc3162_mtd_info->erasesize - tclinux_flash_offset;
		#endif
		}
	}

	#if defined(TCSUPPORT_MTD_ENCHANCEMENT) || defined(TCSUPPORT_MULTI_BOOT)
	tclinux_size = tc3162_parts[2].size + tc3162_parts[3].size;
	#endif
	#endif 

//use last 4 block as reserve area for storing data(for example:syslog,backupromfile,and so on)
 #ifdef TCSUPPORT_MTD_ENCHANCEMENT
 	for(i= 0; i < tc3162_parts_size; i++)
	{
		if(!strcmp(tc3162_parts[i].name,"reservearea"))
		{
			/* 
			 * frankliao modify 20110112 
			 * 64K block size SPI Flash & 128K block size NAND Flash 
			 */
			if (tc3162_mtd_info->erasesize >= 0x10000) {
			#ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
                tc3162_parts[i].offset = 0x7c80000;
                tc3162_parts[i].size = 0x380000;
			#else
			#ifdef TCSUPPORT_NAND_FLASH
#if defined(TCSUPPORT_CT_PON)
			if (tc3162_mtd_info->erasesize >= 0x20000){ /* 128*0x20000 is used for bmt */
			#if 1//
				tc3162_parts[i].offset = nand_flash_avalable_size - BLOCK_NUM_FOR_RESERVEAREA * 0x40000;
			#else
				tc3162_parts[i].offset = tc3162_mtd_info->size - BLOCK_NUM_FOR_RESERVEAREA * 0x40000;
			#endif
				tc3162_parts[i].size = BLOCK_NUM_FOR_RESERVEAREA * 0x40000;
			}
#else
				tc3162_parts[i].offset = tc3162_mtd_info->size - nand_yaffs_size - BLOCK_NUM_FOR_RESERVEAREA*( tc3162_mtd_info->erasesize);
				tc3162_parts[i].size = BLOCK_NUM_FOR_RESERVEAREA*(tc3162_mtd_info->erasesize);
#endif
			#else
				tc3162_parts[i].offset = tc3162_mtd_info->size - nand_yaffs_size - BLOCK_NUM_FOR_RESERVEAREA*( tc3162_mtd_info->erasesize);
				tc3162_parts[i].size = BLOCK_NUM_FOR_RESERVEAREA*(tc3162_mtd_info->erasesize);
			#endif
			#endif
			/* 16 block size NAND Flash */
			} else {
				tc3162_parts[i].offset = tc3162_mtd_info->size - nand_yaffs_size - BLOCK_NUM_FOR_RESERVEAREA*(0x10000);
				tc3162_parts[i].size = BLOCK_NUM_FOR_RESERVEAREA*(0x10000);
			}
		}

  #ifndef TCSUPPORT_NAND_BADBLOCK_CHECK

		#ifdef CONFIG_DUAL_IMAGE
		memcpy(tmp,(char*)bufaddr,sizeof(char));
		if(flagvalue != tmp[0])//use main image
		{
		#endif
		if(!strcmp(tc3162_parts[i].name,"tclinux"))
		{
			#ifdef CONFIG_DUAL_IMAGE
			tc3162_parts[i].size = tclinux_slave_offset -tclinux_flash_offset ; //reserve the last 4 blocks
			#else
			
			/* 
			 * frankliao modify 201100112 
			 * 64K block size SPI Flash & 128K block size NAND Flash
			 */
			if (tc3162_mtd_info->erasesize == 0x10000) {
				tc3162_parts[i].size = tc3162_mtd_info->size - BLOCK_NUM_FOR_RESERVEAREA*( tc3162_mtd_info->erasesize) - tclinux_flash_offset; //reserve the last 4 blocks
			} 
			else if (tc3162_mtd_info->erasesize >= 0x20000) {
				tc3162_parts[i].size = tc3162_mtd_info->size - nand_yaffs_size - BLOCK_NUM_FOR_RESERVEAREA*( tc3162_mtd_info->erasesize) - tclinux_flash_offset; 
			/* 16K block size NAND Flash */
			} else {
				tc3162_parts[i].size = tc3162_mtd_info->size - BLOCK_NUM_FOR_RESERVEAREA*(0x10000) - tclinux_flash_offset;
			}
			#endif
			if(tclinux_size > tc3162_parts[i].size)
			{
				printk("\r\ntclinux size is beyond the limit!!");
				return -1;
			}
		}
		#ifdef CONFIG_DUAL_IMAGE
		}
		if(!strcmp(tc3162_parts[i].name,"tclinux_slave"))
		{
			/* 
			 * frankliao modify 201100112 
			 * 64K block size SPI Flash & 128K block size NAND Flash
			 */
			if (tc3162_mtd_info->erasesize == 0x10000) {
				tc3162_parts[i].size = tc3162_mtd_info->size - nand_yaffs_size - BLOCK_NUM_FOR_RESERVEAREA*( tc3162_mtd_info->erasesize) -tclinux_slave_offset; //reserve the last 4 blocks
			} else if (tc3162_mtd_info->erasesize >= 0x20000) {
			#ifdef TCSUPPORT_NAND_FLASH
#if defined(TCSUPPORT_CT_PON)
				tc3162_parts[i].size = MAIN_IMAGE_SIZE;
#else
				tc3162_parts[i].size = tc3162_mtd_info->size - nand_yaffs_size - BLOCK_NUM_FOR_RESERVEAREA*( tc3162_mtd_info->erasesize) -tclinux_slave_offset; 
#endif
			#else
				tc3162_parts[i].size = tc3162_mtd_info->size - nand_yaffs_size - 6*( tc3162_mtd_info->erasesize) -tclinux_slave_offset; 
			#endif
			/* 16K block size NAND Flash */
			} else {
				tc3162_parts[i].size = tc3162_mtd_info->size -BLOCK_NUM_FOR_RESERVEAREA*(0x10000) - tclinux_slave_offset;
			}
		#ifdef TCSUPPORT_JFFS2_BLOCK
			#ifndef TCSUPPORT_NAND_FLASH)
#if !defined(TCSUPPORT_CT_PON)
			tc3162_parts[i].size -= BLOCK_FOR_JFFS2;
#endif
			#endif
		#endif
			if(tclinux_slave_size > tc3162_parts[i].size)
			{
				printk("\r\ntclinux_slave size is beyond the limit!!");
									
#if !defined(TCSUPPORT_CY_PON)
			//	return -1;
#endif
			}
		}
		#endif
		#endif
		#ifdef TCSUPPORT_NAND_FLASH
#if defined(TCSUPPORT_CT_PON)
		#ifdef TCSUPPORT_JFFS2_BLOCK
		if(!strcmp(tc3162_parts[i].name,"jffs2"))
		{
			tc3162_parts[i].offset = tclinux_slave_offset + MAIN_IMAGE_SIZE;
			tc3162_parts[i].size = BLOCK_FOR_JFFS2;
		}
		#endif
#endif
		#endif		
		#ifdef TCSUPPORT_SQUASHFS_ADD_YAFFS
  		if(!strcmp(tc3162_parts[i].name,"yaffs"))
  		{
#if defined(TCSUPPORT_CT_OSGI)
  			tc3162_parts[i].offset = tclinux_slave_offset + MAIN_IMAGE_SIZE;
#else
  			tc3162_parts[i].offset = tc3162_mtd_info->size - nand_yaffs_size; 
#endif
  		}
  #endif
		
 	} 	
 #endif

	#if defined(TCSUPPORT_MULTI_BOOT)
 	for(i= 0; i < tc3162_parts_size; i++)
	{
		#if !defined(TCSUPPORT_C1_ZY_SFU)
		if(!strcmp(tc3162_parts[i].name,"romd"))
		{
			if (tc3162_mtd_info->erasesize >= 0x10000) {
				tc3162_parts[i].offset = tc3162_mtd_info->size -5*( tc3162_mtd_info->erasesize);
				tc3162_parts[i].size = (tc3162_mtd_info->erasesize);
			} else {
				tc3162_parts[i].offset = tc3162_mtd_info->size -5*(0x10000);
				tc3162_parts[i].size = 0x10000;
			}
		}
		#endif


		#ifdef CONFIG_DUAL_IMAGE
		memcpy(tmp,(char*)bufaddr,sizeof(char));
		if(flagvalue != tmp[0])//use main image
		{
		#endif
		
			if(!strcmp(tc3162_parts[i].name,"tclinux"))
			{
				#ifdef CONFIG_DUAL_IMAGE
				tc3162_parts[i].size = tclinux_slave_offset -tclinux_flash_offset ; 
				#else				
				if (tc3162_mtd_info->erasesize >= 0x20000) {
					tc3162_parts[i].size = tc3162_mtd_info->size -6*( tc3162_mtd_info->erasesize) -tclinux_flash_offset; 
				} else if (tc3162_mtd_info->erasesize == 0x10000) {
					tc3162_parts[i].size = tc3162_mtd_info->size -5*( tc3162_mtd_info->erasesize) -tclinux_flash_offset; 
				} else {
					tc3162_parts[i].size = tc3162_mtd_info->size -5*(0x10000) -tclinux_flash_offset; 
				}
				#endif
		
				if(tclinux_size > tc3162_parts[i].size)
				{
					printk("tclinux size is beyond the limit!!\r\n");					
					return -1;
				}
			}

		
		#ifdef CONFIG_DUAL_IMAGE
		}
		if(flagvalue == tmp[0])//use slave image
		{
			if(!strcmp(tc3162_parts[i].name,"tclinux_slave"))
			{
				if (tc3162_mtd_info->erasesize >= 0x20000) {
					tc3162_parts[i].size = tc3162_mtd_info->size -6*tc3162_mtd_info->erasesize -tclinux_slave_offset;
				} else if (tc3162_mtd_info->erasesize == 0x10000) {
					tc3162_parts[i].size = tc3162_mtd_info->size -5*tc3162_mtd_info->erasesize -tclinux_slave_offset;
				} else {
					tc3162_parts[i].size = tc3162_mtd_info->size -5*(0x10000) -tclinux_slave_offset;
				}
				if(tclinux_slave_size > tc3162_parts[i].size)
				{
					printk("tclinux_slave size is beyond the limit!!\r\n");
#if !defined(TCSUPPORT_CY_PON)
				//	return -1;
#endif
				}
			}
		}
		#endif
 	} 	
#endif

#if !defined(TCSUPPORT_NAND_BADBLOCK_CHECK) && !defined(TCSUPPORT_NAND_RT63368)
	/*Reset the kernel partition offset*/
	tc3162_parts[2].offset = tclinux_flash_offset;
	/*Reset the tclinux partition offset*/
	tc3162_parts[4].offset = tclinux_flash_offset;

	/* frank added 20110111 for 128K block size NAND Flash*/
	if (tc3162_mtd_info->erasesize >= 0x20000) {
		#ifdef TCSUPPORT_BOOTROM_LARGE_SIZE
		/* reset the bootloader partition size to 0x40000 which is a blocksize of 128K&256k NAND Flash */
		tc3162_parts[0].size = 0x40000; 
		/*romfile offset is 0x40000 for 128k&256k block size*/
		//tc3162_parts[1].offset = tc3162_mtd_info->erasesize;
		tc3162_parts[1].offset = 0x40000;

		/* reset the romfile partition size */
		tc3162_parts[1].size = 0x40000;
		#else
		/* reset the bootloader partition size to 0x20000 which is a blocksize of 128K NAND Flash */
		tc3162_parts[0].size = tc3162_mtd_info->erasesize; 
		/* 
		 * reset the romfile partition offset. 
		 * the romfile partition starts from the last block address
		 */
		tc3162_parts[1].offset = tc3162_mtd_info->size - nand_yaffs_size - 6*tc3162_mtd_info->erasesize;
		/* reset the romfile partition size */
		tc3162_parts[1].size = tc3162_mtd_info->erasesize;
		#endif
	} 
#endif	

#ifdef TCSUPPORT_INIC_HOST
	for(i = 0; i < tc3162_parts_size; i++) {	
		if(!strcmp(tc3162_parts[i].name,INIC_CLIENT_ROMFILE_NAME)) {
			tc3162_parts[i - 1].size -= INIC_CLIENT_ROMFILE_SIZE;
			tc3162_parts[i].offset = tc3162_parts[i - 1].offset + tc3162_parts[i - 1].size;
		}
	}
#endif


	add_mtd_partitions(tc3162_mtd_info, tc3162_parts, tc3162_parts_size);
	return 0;
}
#else
unsigned long long tc3162_memparse(const char *ptr, char **retptr, unsigned int blocksize)
{
	char *endptr;	/* local pointer to end of parsed string */

	unsigned long long ret = simple_strtoull(ptr, &endptr, 0);

	switch (*endptr) {
	case 'G':
	case 'g':
		ret <<= 10;
	case 'M':
	case 'm':
		ret <<= 10;
	case 'K':
	case 'k':
		ret <<= 10;
		endptr++;
		break;
	case 'B':
	case 'b':
		ret *=blocksize;
		endptr++;
		break;
	default:
		break;
	}

	if (retptr)
		*retptr = endptr;

	return ret;
}

static void tc3162_newpart_set_other_parts(struct mtd_partition *part, char *name, int index, unsigned int blocksize){
	if(!part || !name){
		printk("tc3162_newpart_set_other_parts fail, input NULL\n");
		return;
	}
	if(!strcmp(name, TCLINUX)){
		part[0].name = KERNEL_PART;
		part[0].size = SIZE_TO_GET;
		part[0].offset = OFFSET_CONTINUOUS;
		part[1].name = ROOTFS_PART;
		part[1].size = SIZE_TO_GET;
		part[1].offset = OFFSET_CONTINUOUS;
		kernel_part_index = index;
	}
	else if(!strcmp(name, TCLINUX_SLAVE)){
		part[0].name = KERNEL_SLAVE_PART;
		part[0].size = SIZE_TO_GET;
		part[0].offset = OFFSET_CONTINUOUS;
		part[1].name = ROOTFS_SLAVE_PART;
		part[1].size = SIZE_TO_GET;
		part[1].offset = OFFSET_CONTINUOUS;
		kernel_slave_part_index = index;
	}
	else if(!strcmp(name, RESERVEAREA)){
#ifdef TCSUPPORT_NAND_FLASH
/*#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_OPENWRT)*/
		blocksize = 0x40000;
/*#endif*/
#endif
		part->name = RESERVEAREA;
		part->offset = OFFSET_BACK_FORWARD;
		part->size = (blocksize*TCSUPPORT_RESERVEAREA_BLOCK);
	}
}
/*
 * Parse one partition definition for an MTD. Since there can be many
 * comma separated partition definitions, this function calls itself
 * recursively until no more partition definitions are found. Nice side
 * effect: the memory to keep the mtd_partition structs and the names
 * is allocated upon the last definition being found. At that point the
 * syntax has been verified ok.
 */
static struct mtd_partition * tc3162_newpart(char *s, char **retptr, int *num_parts,
                                      int this_part, unsigned char **extra_mem_ptr,
                                      int extra_mem_size, unsigned int blocksize)
{
	struct mtd_partition *parts;
	unsigned long size = SIZE_TO_GET;
	unsigned long offset = OFFSET_CONTINUOUS;
	char *name;
	int name_len;
	unsigned char *extra_mem;
	char delim;
	unsigned int mask_flags;

	/* fetch the partition size */
	if (*s == '-'){	
		if(has_remaining_part_flag == 0){
			/* assign all remaining space to this partition */
			size = SIZE_REMAINING;
			has_remaining_part_flag = 1;
			s++;
		}
		else{
			printk(KERN_ERR ERRP "no fill-up partitions allowed after already having a fill-up partition\n");
			return NULL;
		}
	}
	
	else{
		size = tc3162_memparse(s, &s, blocksize);
		if(size == 0)
			size = SIZE_TO_GET;
		if (size < PAGE_SIZE)
		{
			printk(KERN_ERR ERRP "partition size too small (%lx)\n", size);
			return NULL;
		}
	}

	/* fetch partition name and flags */
	mask_flags = 0; /* this is going to be a regular partition */
	delim = 0;
	
        /* now look for name */
	if (*s == '['){
		delim = ']';
	}

	if (delim){
		char *p;

	    	name = ++s;
		p = strchr(name, delim);
		if (!p)
		{
			printk(KERN_ERR ERRP "no closing %c found in partition name\n", delim);
			return NULL;
		}
		name_len = p - name;
		s = p + 1;
	}
	else{
	    	name = NULL;
		name_len = 13; /* Partition_000 */
	}

	/* record name length for memory allocation later */
	extra_mem_size += name_len + 1;

	/* offset type is append */
        if (strncmp(s, "a", 1) == 0){
		offset = OFFSET_CONTINUOUS;
		s += 1;
        }

        /* offset type is back forward*/
        if (strncmp(s, "end", 3) == 0){
		offset = OFFSET_BACK_FORWARD;
		s += 3;
        }

	/* test if more partitions are following */
	if (*s == ',')
	{
#if !defined(TCSUPPORT_FH_ENV)	
		if(!strncmp(name, TCLINUX, name_len)){
			this_part += 2;
			extra_mem_size +=strlen(KERNEL_PART)+strlen(ROOTFS_PART)+2;
		}
		else if(!strncmp(name, TCLINUX_SLAVE, name_len)){
			this_part += 2;
			extra_mem_size +=strlen(KERNEL_SLAVE_PART)+strlen(ROOTFS_SLAVE_PART)+2;
		}
#endif
		/* more partitions follow, parse them */
		parts = tc3162_newpart(s + 1, &s, num_parts, this_part + 1,
				&extra_mem, extra_mem_size, blocksize);
		if (!parts)
			return NULL;
	}
	else
	{	/* this is the last partition: allocate space for all */
		int alloc_size;
#if !defined(TCSUPPORT_FH_ENV)
		if(!strncmp(name, TCLINUX, name_len)){
			this_part += 2;
			extra_mem_size +=strlen(KERNEL_PART)+strlen(ROOTFS_PART)+2;
		}
		else if(!strncmp(name, TCLINUX_SLAVE, name_len)){
			this_part += 2;
			extra_mem_size +=strlen(KERNEL_SLAVE_PART)+strlen(ROOTFS_SLAVE_PART)+2;
		}
		
		*num_parts = this_part + 2; /*add reservearea partition*/
		extra_mem_size += strlen(RESERVEAREA)+1;
#endif
		alloc_size = *num_parts * sizeof(struct mtd_partition) +
			     extra_mem_size;
		parts = kzalloc(alloc_size, GFP_KERNEL);
		if (!parts){
			printk(KERN_ERR ERRP "out of memory\n");
			return NULL;
		}
		extra_mem = (unsigned char *)(parts + *num_parts);
	}
	/* enter this partition (offset will be calculated later if it is zero at this point) */
	parts[this_part].size = size;
	parts[this_part].offset = offset;
	parts[this_part].mask_flags = mask_flags;
	if (name){
		strlcpy(extra_mem, name, name_len + 1);
	}
	else{
		sprintf(extra_mem, "Partition_%03d", this_part);
	}
	parts[this_part].name = extra_mem;
	extra_mem += name_len + 1;
#if !defined(TCSUPPORT_FH_ENV)	
	if(!strcmp(parts[this_part].name, TCLINUX)){
		tc3162_newpart_set_other_parts(&parts[this_part-2],TCLINUX, this_part-2, blocksize);
	}
	else if(!strcmp(parts[this_part].name, TCLINUX_SLAVE)){
		tc3162_newpart_set_other_parts(&parts[this_part-2],TCLINUX_SLAVE, this_part-2, blocksize);
	}
	if(this_part == (*num_parts -2)){
		tc3162_newpart_set_other_parts(&parts[*num_parts -1], RESERVEAREA, *num_parts -1, blocksize);
	}
#endif
	printk("partition %d: name <%s>, offset %llx, size %llx, mask flags %x\n",this_part, parts[this_part].name,
	     parts[this_part].offset, parts[this_part].size,parts[this_part].mask_flags);

	/* return (updated) pointer to extra_mem memory */
	if (extra_mem_ptr)
	  *extra_mem_ptr = extra_mem;

	/* return (updated) pointer command line string */
	*retptr = s;

	/* return partition table */
	return parts;
}

/******************************************************************************
 Function:		
 Description:	Parse the command line.
 Input:		
 Return:	    	
******************************************************************************/
static int tc3162_mtdpart_setup(char *s, unsigned int blocksize)
{
	char cmdline[STR_LEN];
	unsigned char *extra_mem;
	char *p;

	if(s == NULL){
		panic("tc3162_mtdpart_setup(), mtd partition cmdline is NULL\n");
	}
#if !defined(TCSUPPORT_FH_ENV)
	if(strlen(s) + strlen(BOOTLOADER_PART_STR)+strlen(ROMFILE_PART_STR) > STR_LEN){
		panic("tc3162_mtdpart_setup(), string length outof size\n");
	}
	
	tc3162_check_mtdpart_str(cmdline, s);
#endif
	p = cmdline;
	printk("\nparsing <%s>\n", p);

	/*
	 * parse one mtd. have it reserve memory for the
	 * struct cmdline_mtd_partition and the mtd-id string.
	 */
	ecnt_parts = tc3162_newpart(p,		/* cmdline */
			&s,		/* out: updated cmdline ptr */
			&num_parts,	/* out: number of parts */
			0,		/* first partition */
			&extra_mem, /* out: extra mem */
			 0, blocksize);
	if(!ecnt_parts)
	{
		/*
		 * An error occurred. We're either:
		 * a) out of memory, or
		 * b) in the middle of the partition spec
		 * Either way, this mtd is hosed and we're
		 * unlikely to succeed in parsing any more
		 */
		 return 0;
	 }

	return 1;
}

#ifdef TCSUPPORT_CPU_ARMV8
int get_kernel_rootfs_arm(unsigned int *header, unsigned int *header_slave, uint64_t *tclinux_real_size, uint64_t *tclinux_slave_real_size)
{
	int ret = 0;
	uint64_t size, offset, trxLen;

	/* offset trx header */
	trxLen = sizeof(struct trx_header); 
	
	ret = get_partition_info(TCLINUX_INFO_MASTER_REAL_SIZE, tclinux_real_size);
	if(ret != 0) {
		printk("Get tclinux size error.\n");
		return -1;
	}

	ret = get_partition_info(TCLINUX_INFO_MASTER_KERNEL_OFFSET, &offset);
	if(ret != 0) {
		printk("Get kernel offset error.\n");
		return -1;
	}
	ret = get_partition_info(TCLINUX_INFO_MASTER_KERNEL_SIZE, &size);
	if(ret != 0) {
		printk("Get kernel size error.\n");
		return -1;
	}
	ecnt_parts[kernel_part_index].size = offset + size + trxLen;

	ret = get_partition_info(TCLINUX_INFO_MASTER_ROOTFS_OFFSET, &offset);
	if(ret != 0) {
		printk("Get rootfs offset error.\n");
		return -1;
	}
	ecnt_parts[kernel_part_index+1].offset = ecnt_parts[kernel_part_index].offset + offset + trxLen;

	ret = get_partition_info(TCLINUX_INFO_MASTER_ROOTFS_SIZE, &size);
	if(ret != 0) {
		printk("Get rootfs size error.\n");
		return -1;
	}
	ecnt_parts[kernel_part_index+1].size = ROOTFS_PARTITION(size);
	
	if(kernel_slave_part_index > 0) {
		ret = get_partition_info(TCLINUX_INFO_SLAVE_REAL_SIZE, tclinux_slave_real_size);
		if(ret != 0) {
			printk("Get tclinux_slave size error.\n");
			return -1;
		}

		ret = get_partition_info(TCLINUX_INFO_SLAVE_KERNEL_OFFSET, &offset);
		if(ret != 0) {
			printk("Get kernel_slave offset error.\n");
			return -1;
		}
		ret = get_partition_info(TCLINUX_INFO_SLAVE_KERNEL_SIZE, &size);
		if(ret != 0) {
			printk("Get kernel_slave size error.\n");
			return -1;
		}
		ecnt_parts[kernel_slave_part_index].size = offset + size + trxLen;

		ret = get_partition_info(TCLINUX_INFO_SLAVE_ROOTFS_OFFSET, &offset);
		if(ret != 0) {
			printk("Get rootfs_slave offset error.\n");
			return -1;
		}
		ecnt_parts[kernel_slave_part_index+1].offset = ecnt_parts[kernel_slave_part_index].offset + offset + trxLen;

		ret = get_partition_info(TCLINUX_INFO_SLAVE_ROOTFS_SIZE, &size);
		if(ret != 0) {
			printk("Get rootfs_slave size error.\n");
			return -1;
		}
		ecnt_parts[kernel_slave_part_index+1].size = ROOTFS_PARTITION(size);
	}

	return 0;
}
#else
int get_kernel_rootfs_mips(unsigned int *header, unsigned int *header_slave, uint64_t *tclinux_real_size, uint64_t *tclinux_slave_real_size)
{
	unsigned int addr;
	int sHeaderLen = 0;
#if defined(TCSUPPORT_SECURE_BOOT_V2)
	SECURE_HEADER_V2 sHeader;
	sHeaderLen = sizeof(SECURE_HEADER_V2);
#elif defined(TCSUPPORT_SECURE_BOOT_V1) || defined(TCSUPPORT_SECURE_BOOT_FLASH_OTP)
	SECURE_HEADER_V1 sHeader;
	sHeaderLen = sizeof(SECURE_HEADER_V1);
#endif

	addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 20 * sizeof(unsigned int)));
	ecnt_parts[kernel_part_index].size = KERNEL_PARTITION(addr) + sHeaderLen;
	addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 21 * sizeof(unsigned int)));
	*tclinux_real_size = ecnt_parts[kernel_part_index].size + addr;
	ecnt_parts[kernel_part_index+1].size = ROOTFS_PARTITION(addr);
	ecnt_parts[kernel_part_index+1].offset = ecnt_parts[kernel_part_index].offset + ecnt_parts[kernel_part_index].size;
	
	if(kernel_slave_part_index > 0){
		addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header_slave + 20 * sizeof(unsigned int)));
		ecnt_parts[kernel_slave_part_index].size = KERNEL_PARTITION(addr) + sHeaderLen;
		addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header_slave + 21 * sizeof(unsigned int)));
		*tclinux_slave_real_size = ecnt_parts[kernel_slave_part_index].size + addr;
		ecnt_parts[kernel_slave_part_index+1].size = ROOTFS_PARTITION(addr);
		ecnt_parts[kernel_slave_part_index+1].offset = ecnt_parts[kernel_slave_part_index].offset + ecnt_parts[kernel_slave_part_index].size;
	}

	return 0;
}
#endif

void tc3162_set_kernel_rootfs_part(void){
	unsigned int addr;
	unsigned int *header;
	unsigned int *header_slave;
	int i;	
	uint64_t tclinux_real_size = 0;
	uint64_t tclinux_slave_real_size = 0;
	int sHeaderLen = 0;
	int ret;
#if defined(TCSUPPORT_SECURE_BOOT_V2)
	SECURE_HEADER_V2 sHeader;
	sHeaderLen = sizeof(SECURE_HEADER_V2);
#elif defined(TCSUPPORT_SECURE_BOOT_V1) || defined(TCSUPPORT_SECURE_BOOT_FLASH_OTP)
	SECURE_HEADER_V1 sHeader;
	sHeaderLen = sizeof(SECURE_HEADER_V1);
#endif

	if(kernel_part_index < 0)
		return;
	
	if(tclinux_part_offset != OFFSET_CONTINUOUS){
		header = (unsigned int *)(flash_base + tclinux_part_offset);
	}
	else{	
		panic("tc3162_set_kernel_rootfs_part(), tclinux partition offset error\n");
	}
	
	if((tclinux_part_size !=0) && (tclinux_part_size != SIZE_REMAINING)){
		header_slave = (unsigned int *)(flash_base + tclinux_part_offset + tclinux_part_size);
	}
	else{
		panic("tc3162_set_kernel_rootfs_part(), tclinux partition size error\n");		
	}

#ifdef TCSUPPORT_CPU_ARMV8
	ret = get_kernel_rootfs_arm(header, header_slave, &tclinux_real_size, &tclinux_slave_real_size);
#else
	ret = get_kernel_rootfs_mips(header, header_slave, &tclinux_real_size, &tclinux_slave_real_size);
#endif

	if(ret != 0) {
		panic("tc3162_set_kernel_rootfs_part(), init tclinux partition size error\n");
	}
	/*check whether tclinux/tclinux_slave partition enough*/
	for(i=0; i <num_parts; i++){
		if(!strcmp(ecnt_parts[i].name,TCLINUX)){
			if(ecnt_parts[i].size < tclinux_real_size){
				printk("tclinux partition size = %llx, real size = %llx\n", ecnt_parts[i].size, tclinux_real_size);
				printk("tclinux partition size < its real size!!!\n");
			}
		}
		else if(!strcmp(ecnt_parts[i].name,TCLINUX_SLAVE)){
			if(ecnt_parts[i].size < tclinux_slave_real_size){
				printk("tclinux_slave partition size = %llx, real size = %llx\n", ecnt_parts[i].size, tclinux_slave_real_size);
				printk("tclinux_slave partition size < its real size!!!\n");
			}
		}
	}
}

void tc3162_check_mtdpart_str(char *dst, char *src){
	int have_bootloader_part = 0;
	int have_romfile_part = 0;
	char *bootloader_p = NULL;
	char *bootloader_end_p = NULL;
	
	if(bootloader_p = strstr(src, BOOTLOADER_PART)){
		have_bootloader_part = 1;
	}
	if(strstr(src, ROMFILE_PART)){
		have_romfile_part = 1;
	}
#ifdef TCSUPPORT_OPENWRT
	have_romfile_part = 1;
#endif

	if(have_romfile_part && have_bootloader_part){
		strcpy(dst, src);
	}
	else if(!have_romfile_part && !have_bootloader_part){
		strcpy(dst, BOOTLOADER_PART_STR);
		strcat(dst, ROMFILE_PART_STR);
		strcat(dst, src);
	}
	else if(have_romfile_part && !have_bootloader_part){
		strcpy(dst, BOOTLOADER_PART_STR);
		strcat(dst, src);
	}
	else if(!have_romfile_part && have_bootloader_part){
		bootloader_end_p = strchr(bootloader_p, ',');
		if(!bootloader_end_p)
			panic("cmdline, bootloader partition error!\n");
		memcpy(dst, src, bootloader_end_p - src+1);
		strcat(dst, ROMFILE_PART_STR);
		strcat(dst, bootloader_end_p+1);
	}
	
}

uint64_t tc3162_get_bootloader_romfile_size(char *name, unsigned int blocksize){
#ifdef TCSUPPORT_BOOTROM_LARGE_SIZE
	uint64_t bootloader_size = 0x00020000;
#else
	uint64_t bootloader_size = 0x00010000;
#endif
	uint64_t romfile_size = 0x00010000;
#if defined(TCSUPPORT_BLX_MULTICOPY) || defined(TCSUPPORT_ARM_MULTIBOOT)
	bootloader_size = 0x80000;
#if defined(TCSUPPORT_ARM_MULTIBOOT)
	bootloader_size += 0x00100000;
#endif
#if defined(TCSUPPORT_BLX_MULTICOPY)
	bootloader_size += 0x00180000;
#endif

#elif defined(TCSUPPORT_OPENWRT)
	bootloader_size = 0x00080000;
#endif

#if defined(TCSUPPORT_NAND_FLASH) || defined(TCSUPPORT_NEW_SPIFLASH) || defined(TCSUPPORT_EMMC)
#ifndef TCSUPPORT_DSL_PHYMODE
	blocksize = 0x40000;
#endif
#endif

	if(!strcmp(name, BOOTLOADER_PART)){
		if(bootloader_size < blocksize)
#ifdef TCSUPPORT_CPU_ARMV8
			bootloader_size = blocksize * 2;
#else
			bootloader_size = blocksize;
#endif
		return bootloader_size;
	}
	else if(!strcmp(name, ROMFILE_PART)){
		if(romfile_size < blocksize)
			romfile_size = blocksize;
		return romfile_size;
	}
	return SIZE_TO_GET;
	
}
/******************************************************************************
 Function:		tc3162_parse_cmdline_partitions
 Description:	It's used to init tc3162_parts by cmdline partiotion string
 Input:		
 Return:	    	
******************************************************************************/
int tc3162_parse_cmdline_partitions(struct mtd_info *master){
	unsigned long offset;
	int i;
	int size_remaining_index = -1;
	int first_back_forward_index = -1;
	unsigned long size_remaining_offset_start = 0;
	unsigned long back_forward_total_size = 0;
	int is_tclinux_remaining_flag = 0;
	unsigned int blocksize = master->erasesize;
	unsigned long total_size = master->size;

#if defined(TCSUPPORT_NAND_FLASH) || defined(TCSUPPORT_EMMC)
#if defined(TCSUPPORT_EMMC)
	if(GET_NP_SCU_EMMC()) {
		total_size = tc3162_mtd_info->size;
	} else
#endif
#if defined(TCSUPPORT_NAND_FLASH)
	if (IS_NANDFLASH) {
/*#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_OPENWRT)*/
		total_size = nand_flash_avalable_size;
/*#endif*/
#else	//#if defined(TCSUPPORT_NAND_FLASH)
	{}
#endif	//#if defined(TCSUPPORT_NAND_FLASH)
	}
#endif
	/* parse command line */
	if(tc3162_mtdpart_setup(TCSUPPORT_PARTITIONS_CMDLINE_STR, blocksize) == 0){
		printk("tc3162_mtdpart_setup fail\n");
		return 0;
	}
	
	for(i = 0, offset = 0; i < num_parts; i++)
	{
		if(ecnt_parts[i].offset == OFFSET_BACK_FORWARD){
			first_back_forward_index = i;
			break;
		}
		if(ecnt_parts[i].size == SIZE_TO_GET){
			ecnt_parts[i].offset = offset;
			if(!strcmp(ecnt_parts[i].name,BOOTLOADER_PART) || !strcmp(ecnt_parts[i].name, ROMFILE_PART))
				ecnt_parts[i].size = tc3162_get_bootloader_romfile_size(ecnt_parts[i].name, blocksize);
			else	
				continue;
		}
		
		ecnt_parts[i].offset = offset;				
		
		if(ecnt_parts[i].size == SIZE_REMAINING){
			size_remaining_index = i;
			size_remaining_offset_start = ecnt_parts[i].offset;
			if((i+1) == num_parts){
				ecnt_parts[i].size = total_size - offset;
			}
			else{
				if(!strcmp(ecnt_parts[i].name, TCLINUX)){
					is_tclinux_remaining_flag = 1;
				}
				first_back_forward_index = i+1;
				break;
			}
		}
		if (offset + ecnt_parts[i].size > total_size)
		{
			panic(KERN_WARNING ERRP" part %d: partitioning exceeds flash size, truncating\n", i);
		}
		offset +=ecnt_parts[i].size;
		/*offset align*/
		offset = ALIGN(offset, master->erasesize);
		if(!strcmp(ecnt_parts[i].name, TCLINUX)){
			tclinux_part_offset = ecnt_parts[i].offset;
			tclinux_part_size = ecnt_parts[i].size;
		}
	}
	/*offset type is back forward*/
	for(i = (num_parts -1), offset = total_size; i >=first_back_forward_index && first_back_forward_index != -1; i--){
		ecnt_parts[i].size = ALIGN(ecnt_parts[i].size, master->erasesize);
		back_forward_total_size += ecnt_parts[i].size;
		ecnt_parts[i].offset = offset-ecnt_parts[i].size;
		offset -= ecnt_parts[i].size;
		
		if (offset + back_forward_total_size > total_size)
		{
			panic(KERN_WARNING ERRP"partitioning exceeds flash size, partition index = %d\n",i);
		}
	}
	/*calculate remaining size*/
	if(size_remaining_index != -1){
		if(offset < size_remaining_offset_start)
			panic(KERN_WARNING ERRP"partition size < 0, index = %d \n", size_remaining_index);
		ecnt_parts[size_remaining_index].size = offset - size_remaining_offset_start;
		if(is_tclinux_remaining_flag){
			tclinux_part_offset = ecnt_parts[size_remaining_index].offset;
			tclinux_part_size = ecnt_parts[size_remaining_index].size;
		}
	}
#if !defined(TCSUPPORT_FH_ENV)	
	tc3162_set_kernel_rootfs_part();
#endif
	return num_parts;
}

#endif

#if 0
/*#ifdef TCSUPPORT_OPENWRT*/
static int __init tc3162_mtd_init(void)
{
	struct mtd_info *mtd;
	unsigned int *header;
	unsigned int addr;
	int rootfs_size = 0;
	#if defined(CONFIG_DUAL_IMAGE) || defined(TCSUPPORT_MTD_ENCHANCEMENT) || defined(TCSUPPORT_MULTI_BOOT)
	int i = 0;
	#endif

	int sHeaderLen = 0;
#if defined(TCSUPPORT_SECURE_BOOT_V2)
	SECURE_HEADER_V2 sHeader;
	sHeaderLen = sizeof(SECURE_HEADER_V2);
#elif defined(TCSUPPORT_SECURE_BOOT_V1) || defined(TCSUPPORT_SECURE_BOOT_FLASH_OTP)
	SECURE_HEADER_V1 sHeader;
	sHeaderLen = sizeof(SECURE_HEADER_V1);
#endif

#if defined(TCSUPPORT_MTD_ENCHANCEMENT) || defined(TCSUPPORT_MULTI_BOOT)
	u_int32_t tclinux_size = 0;
#endif

	/* frankliao added 20101223 */
	//header = (unsigned int *)(flash_base + 0x30000);
	header = (unsigned int *)(flash_base + 0x80000);


#if 1//def TCSUPPORT_ADDR_MAPPING
	/*add address mapping on 7510. Pork*/
	if(isMT751020){
		uint32 tmpVal;
		tmpVal = regRead32(0xbfb00038);
		tmpVal &= 0xffe0e0e0;
		tmpVal |= 0x80070f00;
		regWrite32(0xbfb00038,tmpVal);
		//VPint(0xbfb00038) |= 0x80070F00;
		printk("tc3162: flash device 0x%08x at 0x%08x\n", 0x1000000, 0x1c000000);
		tc3162_map.virt = ioremap_nocache(0x1c000000, 0x1000000);
		tc3162_map.phys = 0x1c000000;
		tc3162_map.size = 0x1000000;
		ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);
	}
	/*add 8M 16M flash support. shnwind*/
	else if (isTC3162U || isTC3182 || isRT65168 || isRT63165 || isRT63365 || isRT63260  || isMT751020){
#else
	if (isTC3162U || isTC3182 || isRT65168 || isRT63165 || isRT63365 || isRT63260 || isMT751020){
#endif //TCSUPPORT_ADDR_MAPPING
//		header = (unsigned int *)0xb0020000;
		/*enable addr bigger than 4M support.*/
		VPint(0xbfb00038) |= 0x80000000;
		printk("tc3162: flash device 0x%08x at 0x%08x\n", 0x1000000, 0x10000000);
		tc3162_map.virt = ioremap_nocache(0x10000000, 0x1000000);
		tc3162_map.phys = 0x10000000;
		tc3162_map.size = 0x1000000;
		ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);
	}else{
	
//		header = (unsigned int *)0xbfc20000;
		printk("tc3162: flash device 0x%08x at 0x%08x\n", WINDOW_SIZE, WINDOW_ADDR);
		tc3162_map.virt = ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);

	}
	if (!tc3162_map.virt) {
   		printk("tc3162: Failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(&tc3162_map);

	/* check if boot from SPI flash */
	if (IS_NANDFLASH) {
		tc3162_mtd_info = do_map_probe("nandflash_probe", &tc3162_map);
		printk("tc3162: do_map_probe spi nand flash_probe===>\n\n");
	} else if (IS_SPIFLASH) {
		printk("tc3162: do_map_probe spiflash_probe\n");
		tc3162_mtd_info = do_map_probe("spiflash_probe", &tc3162_map);
	} else {
		tc3162_mtd_info = do_map_probe("cfi_probe", &tc3162_map);
	}

	if (!tc3162_mtd_info) {
		iounmap(tc3162_map.virt);
		return -ENXIO;
	}

  	tc3162_mtd_info->owner = THIS_MODULE;

#ifdef TCSUPPORT_CPU_ARMV8
	int ret = 0;
	uint64_t size, offset, trxLen;
	
	/* offset trx header */
	trxLen = sizeof(struct trx_header); 
	
	ret = get_partition_info(TCLINUX_INFO_MASTER_KERNEL_OFFSET, &offset);
	if(ret != 0) {
		printk("Get kernel offset error.\n");
		return -1;
	}
	ret = get_partition_info(TCLINUX_INFO_MASTER_KERNEL_SIZE, &size);
	if(ret != 0) {
		printk("Get kernel size error.\n");
		return -1;
	}
	tc3162_parts[1].size = offset + size + trxLen;

	ret = get_partition_info(TCLINUX_INFO_MASTER_ROOTFS_OFFSET, &offset);
	if(ret != 0) {
		printk("Get rootfs offset error.\n");
		return -1;
	}
	tc3162_parts[2].offset = tc3162_parts[1].offset + offset + trxLen;

	ret = get_partition_info(TCLINUX_INFO_MASTER_ROOTFS_SIZE, &size);
	if(ret != 0) {
		printk("Get rootfs size error.\n");
		return -1;
	}
	tc3162_parts[2].size = ROOTFS_PARTITION(size);
#else
	addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 20 * sizeof(unsigned int)));
	tc3162_parts[1].size = KERNEL_PARTITION( addr ) + sHeaderLen;// kernel size refill
	addr = READ_FLASH_DWORD((unsigned long long)((unsigned int)header + 21 * sizeof(unsigned int)));
	rootfs_size = ROOTFS_PARTITION( addr );

	/* boot loader is 0x80000, art is 0x80000 */
	tc3162_parts[2].size = tc3162_mtd_info->size - 0x80000 - tc3162_parts[1].size - 0x80000;// rootfs size refill
#endif
	printk("Flash Size is %llx, erase size is %08x\r\n", tc3162_mtd_info->size, tc3162_mtd_info->erasesize);
	{
		//int leftsize = tc3162_mtd_info->size - 0x80000 - tc3162_parts[1].size - rootfs_size;
		int leftsize = tc3162_mtd_info->size - 0x2000000 - 0x80000;
		int leftblocknum = leftsize / tc3162_mtd_info->erasesize;
		//int rootfs_data_off = 0x80000 + tc3162_parts[1].size + rootfs_size ; 
		/* from 8M offset */
		int rootfs_data_off = 0x2000000;
		int rootfs_data_off_blocknum = rootfs_data_off / tc3162_mtd_info->erasesize;
		printk("Leftsize size is %08x, Left block is %08x\r\n", leftsize, leftblocknum);
		printk("rootfs_data off is %08x, off block is %08x\r\n", rootfs_data_off, rootfs_data_off_blocknum);
		tc3162_parts[3].size = tc3162_mtd_info->erasesize * leftblocknum;//rootfs_data size and offset refill
		tc3162_parts[3].offset = tc3162_mtd_info->erasesize * rootfs_data_off_blocknum;

		tc3162_parts[5].size = tc3162_mtd_info->size - 0x80000 - 0x80000; // refill firmware size
	}

#if 0
	if (IS_NANDFLASH) {
		/* frankliao added 20110112, for 128K block size NAND Flash */
		if (tc3162_mtd_info->erasesize == 0x20000) {
			/* the last block store nand flash romfile */
			tc3162_parts[4].size = tc3162_mtd_info->size - 6*tc3162_mtd_info->erasesize - 0x20000;
		}
	}
#endif

#if 0
	/* frank added 20110111 for 128K block size NAND Flash*/
	if (tc3162_mtd_info->erasesize == 0x20000) {
		/* reset the bootloader partition size to 0x20000 which is a blocksize of 128K NAND Flash */
		tc3162_parts[0].size = tc3162_mtd_info->erasesize; 
		/* 
		 * reset the romfile partition offset. 
		 * the romfile partition starts from the last block address
		 */
		tc3162_parts[1].offset = tc3162_mtd_info->size - 6*tc3162_mtd_info->erasesize;
		/* reset the romfile partition size */
		tc3162_parts[1].size = tc3162_mtd_info->erasesize;
	} 
#endif

	add_mtd_partitions(tc3162_mtd_info, tc3162_parts, tc3162_parts_size);
	#if 1
	#ifdef TCSUPPORT_NAND_FLASH
	if (IS_SPIFLASH) {
		ra_nand_init();
	}	
	#endif
	#endif
		mtd = get_mtd_named("rootfs");

	if (mtd) {
		ROOT_DEV = MKDEV(MTD_BLOCK_MAJOR, mtd->index);
		put_mtd_device(mtd);
	}

	return 0;
}

#else
int art_dts_offset_backforward(struct device_node *node)
{	
	int len = 0;
	struct device_node *art_node = NULL;
	struct property *art_reg_prop = NULL;	
	__be32 tmp;
	__be32 *reg = NULL;
	static int *reg_modify = NULL;
	
	if (node == NULL)
		return -1;

	/*openwrt:"art", linux:"reservearea", both backforward*/
	art_node = of_find_node_by_name(node, RESERVEAREA);

	/*
	 * The reservearea/art partition is optional. Layouts that keep the
	 * calibration data inside a UBI volume (e.g. "ubi-volume-art") have
	 * no such node, and in that case there is nothing to relocate, so the
	 * device-tree partition table must still be used.
	 */
	if (art_node == NULL) {
		pr_debug("%s: no \"%s\" node, skip reservearea relocation\n",
			 __func__, RESERVEAREA);
		return 0;
	}

	art_reg_prop = of_find_property(art_node, "reg", &len);
	if (art_reg_prop == NULL) {
		pr_debug("%s: \"%s\" has no reg, skip reservearea relocation\n",
			 __func__, RESERVEAREA);
		return 0;
	}

	reg = art_reg_prop->value;
	
	if (reg == NULL) {
		printk("\nERROR(%s)  missing reg property\n", __func__);
		return -1;
	}
	if (be32_to_cpu(*(reg)) == 0xffffffff) {
		/*art offset = (flash logic size) - (art size)*/
		tmp = cpu_to_be32(nand_flash_avalable_size - be32_to_cpu(*(reg+1)));
		if (reg_modify == NULL) {
			reg_modify = kmalloc(len, GFP_ATOMIC);
			if (reg_modify == NULL){
				printk("\nERROR(%s) new reg property kmalloc fail\n", __func__);
				return -1;
			}
		}
		memcpy(reg_modify, reg, len);
		memcpy(reg_modify, &tmp, sizeof(tmp));
		art_reg_prop->value = reg_modify;
	}
	return 0;
}

int airoha_dts_mtd_init(void)
{
	int ret;
	struct device_node *node=NULL;
	struct platform_device *pdev=NULL;
	
	node = of_find_node_by_path("/partitions");

	if (node == NULL) {
		return -1;
	}
	/*if dts node exsit, use kernel mtd subsystem*/
	ret = art_dts_offset_backforward(node);
	if(ret) {
		printk("\nERROR(%s) DTS partition error \n", __func__);
		return -1;
	}
#ifdef TCSUPPORT_EMMC
	int nparts;
	int i = 0;
	struct mtd_partition *airoha_parts;
	struct device_node *part_node;
	const char *mtd_name;
	unsigned int offset, size;
	nparts = of_get_child_count(node);
    if(!nparts)
    {
        printk("\nERROR(%s) nparts==0\n", __func__);
        return -1;
    }
	printk("Found %d partitions in dtsi\n", nparts);
	airoha_parts = kcalloc(nparts, sizeof(*airoha_parts), GFP_KERNEL);
    if(!airoha_parts)
    {
        printk("\nERROR(%s) Allocate parts memory failed\n", __func__);
        return -ENOMEM;
    }
	for_each_child_of_node(node, part_node)
    {
        if(of_property_read_u32_index(part_node, "reg", 0, &offset))
            return -1;
        if(of_property_read_u32_index(part_node, "reg", 1, &size))
            return -1;
        mtd_name = of_get_property(part_node, "label", NULL);
        if(!mtd_name)
            return -1;
        airoha_parts[i].name = mtd_name;
        airoha_parts[i].offset = offset;
        airoha_parts[i].size = size;
        if(!strcmp(mtd_name, "rootfs_data") || !strcmp(mtd_name, "config"))
            airoha_parts[i].mask_flags |= (MTD_BIT_WRITEABLE);
        i++;
    }
	mtd_device_parse_register(tc3162_mtd_info, NULL, NULL, airoha_parts, nparts);
#else
	pdev = of_find_device_by_node(node);
	if (pdev==NULL) {
		printk("\nERROR(%s) NULL pdev\n", __func__);	
		return -1;
	}
	tc3162_mtd_info->dev.parent = &pdev->dev;
	mtd_set_of_node(tc3162_mtd_info, pdev->dev.of_node);
		
	mtd_device_parse_register(tc3162_mtd_info, NULL, NULL, NULL,0);
#endif
	return 0;
}

static int __init tc3162_mtd_init(void)
{
	int ret = 0;

	struct mtd_partition *mtd_parts = 0;
	int mtd_parts_nb = 0;

	if(ret = tc3162_map_init()){
		printk("tc3162_map_init() fail\n");
		return ret;
	}
	if(ret = tc3162_mtd_info_init()){
		printk("tc3162_mtd_info_init() fail\n");
		return ret;
	}
	ret = airoha_dts_mtd_init();
	if (ret != 0) {
		/*use compile option "TCSUPPORT_MTD_PARTITIONS_CMDLINE" */
#ifdef TCSUPPORT_MTD_PARTITIONS_CMDLINE
	if((mtd_parts_nb = tc3162_parse_cmdline_partitions(tc3162_mtd_info))<= 0){
		printk("tc3162_parse_cmdline_partitions() fail\n");
		return -1;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	INIT_LIST_HEAD(&tc3162_mtd_info->partitions);
#endif
	add_mtd_partitions(tc3162_mtd_info, ecnt_parts, mtd_parts_nb);
#else
	if(ret = tc3162_add_partitions()){
		printk("tc3162_add_partitions() fail\n");
		return ret;
	}
#endif
	}
#ifdef TCSUPPORT_NAND_FLASH
	if (IS_SPIFLASH) {
		ra_nand_init();
	}	
#endif
#if !defined(TCSUPPORT_FH_UNIFIED_PLATFORM) 
	tc3162_put_rootfs();
#endif

	return 0;
}
#endif
static void __exit tc3162_mtd_cleanup(void)
{
	if (tc3162_mtd_info) {
    	del_mtd_partitions(tc3162_mtd_info);
       	map_destroy(tc3162_mtd_info);
	}

#if 1
	#ifdef TCSUPPORT_NAND_FLASH
	if (IS_SPIFLASH) {
		ra_nand_remove();
	}	
	#endif
#endif	
   	if (tc3162_map.virt) {
   		iounmap(tc3162_map.virt);
       	tc3162_map.virt = 0;
	}
}

#ifdef TCSUPPORT_EMMC
late_initcall_sync(tc3162_mtd_init);
#else
module_init(tc3162_mtd_init);
#endif
module_exit(tc3162_mtd_cleanup);

#ifndef TCSUPPORT_CPU_ARMV8
int get_ethaddr(unsigned char *ethaddr, int len)
{
	u32 ptr_rtn_len;
	u32 ret;
	int i;
	unsigned int mac_offset;
	
	if(len != 6) {
		printk("Error get ethaddr length!\n");
		return -1;
	}

	if(ethaddr == NULL) {
		printk("Error! ethaddr is NULL.\n");
		return -1;
	}

#ifdef TCSUPPORT_BB_256KB
	mac_offset = 0x3fe48;
#else
	mac_offset = 0xff48;
#endif

	for (i = 0; i < 6; i++) {
		ethaddr[i] = READ_FLASH_BYTE(flash_base + mac_offset + i);
	}

	return 0;
}
EXPORT_SYMBOL(get_ethaddr);

char get_onutype(void)
{
	char onutype;
	unsigned int mac_offset;

#ifdef TCSUPPORT_BB_256KB
	mac_offset = 0x3fe9c;
#else
	mac_offset = 0xff9c;
#endif

	onutype = READ_FLASH_BYTE(mac_offset);
	
	return onutype;
}
EXPORT_SYMBOL(get_onutype);
#endif


