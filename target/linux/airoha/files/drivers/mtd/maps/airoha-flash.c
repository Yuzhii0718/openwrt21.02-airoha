// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2021-2023 Airoha Inc.

/*
* MTD implementation based on Airoha SDK
* 
* Author: Simran Preet Kaur <simranpreet.kaur@airoha.com>
*
*/


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
#include <linux/libfdt.h>


/************************************************************************
*                  M A C R O S
*************************************************************************
*/

#define WINDOW_ADDR 0x1fc00000
#define WINDOW_SIZE 0x400000
#define BUSWIDTH 	2


extern int nand_flash_avalable_size;
extern int ra_nand_init(void);
extern void ra_nand_remove(void);
static int *reg_modify = NULL;
static struct mtd_info *airoha_mtd_info;


static struct map_info airoha_map = {
       .name = "airoha",
       .size = WINDOW_SIZE,
       .bankwidth = BUSWIDTH,
       .phys = WINDOW_ADDR,
};


/******************************************************************************
 Function:	airoha_map_init
 Description:	It's used to init airoha_map
 Input:		void
 Return:	0: Success,  -EIO: fail
******************************************************************************/
int airoha_map_init(void){
 	uint32 tmpVal;
	tmpVal = GET_SMB0ALS();
	tmpVal &= 0xffe0e0e0;
	tmpVal |= 0x80070f00;
	SET_SMB0ALS(tmpVal);
	airoha_map.phys = 0x00000000;
	airoha_map.size = 0x2000000;
	printk("\nAiroha: flash device 0x%08x at 0x%08x\n", airoha_map.size, airoha_map.phys);
	airoha_map.virt = ioremap_nocache(airoha_map.phys, airoha_map.size);
	ioremap_nocache(WINDOW_ADDR, WINDOW_SIZE);

	if (!airoha_map.virt) {
          	printk("\nERROR(%s) Failed to ioremap\n", __func__);
          	return -EIO;
	}

	simple_map_init(&airoha_map);
  	return 0;
}

static int airoha_mtd_info_init(void){

	if(GET_NP_SCU_EMMC()) {
		airoha_mtd_info = get_mtd_device(NULL, 0);
		airoha_mtd_info->usecount = 0;
		mtd_device_unregister(airoha_mtd_info);
	} 
	/* check if boot from SPI flash */
	else if (IS_NANDFLASH) {
		airoha_mtd_info = do_map_probe("nandflash_probe", &airoha_map);
		} 
        else if (IS_SPIFLASH) {
		airoha_mtd_info = do_map_probe("spiflash_probe", &airoha_map);} 
        else {
		airoha_mtd_info = do_map_probe("cfi_probe", &airoha_map);}

	if (!airoha_mtd_info) {
		iounmap(airoha_map.virt);
		return -ENXIO;
	}

	airoha_mtd_info->owner = THIS_MODULE;
        return 0;
}

int art_dts_offset_backforward(struct device_node *node)
{	
	int len = 0;
	struct device_node *art_node = NULL;
	struct property *art_reg_prop = NULL;	
	__be32 tmp;
	__be32 *reg = NULL;
	
	if (node == NULL)
		return -1;

	/*openwrt:"art", linux:"reservearea", both backforward*/
	art_node = of_find_node_by_name(node, RESERVEAREA);
	
	if (art_node == NULL) {
		printk("\nERROR(%s) art node == NULL\n", __func__);
		return -1;
	}

	art_reg_prop = of_find_property(art_node, "reg", &len);
	if(art_reg_prop == NULL) {
		printk("\nERROR(%s)  missing art reg prop\n", __func__);
		return -1;
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

static int __init airoha_mtd_init(void)
{
	int ret = 0;
	struct device_node *node=NULL;
	struct platform_device *pdev=NULL;
	int nparts;
	int i = 0;
	int is_ubi = 0;
	struct mtd_partition *airoha_parts;
	struct device_node *part_node;
	const char *mtd_name;
	unsigned int offset, size;

	/* parse the dts node by path */
	node = of_find_node_by_path("/partitions");
	if (node==NULL) {
        	printk("\nERROR(%s) node==NULL\n", __func__);
        	return -1;
    	}

	/*
	 * UBI layout: /partitions holds "bl2" plus a "ubi" container marked
	 * compatible = "linux,ubi". art/romfile live inside UBI volumes, so there
	 * is no top-level "art" MTD node; skip art_dts_offset_backforward() below.
	 * The whole-flash MTD and the bl2/ubi partitions are still created here
	 * (this legacy map driver is what registers the SPI-NAND MTD via
	 * do_map_probe); otherwise ubi.mtd=2 finds no device.
	 */
	for_each_child_of_node(node, part_node) {
		if (of_device_is_compatible(part_node, "linux,ubi")) {
			printk("Airoha: UBI layout detected\n");
			is_ubi = 1;
			break;
		}
	}

	if(ret = airoha_map_init()){
		printk("\nERROR(%s) airoha_map_init() fail\n", __func__);
		return ret;
	}
	if(ret = airoha_mtd_info_init()){
		printk("\nERROR(%s) airoha_mtd_info_init() fail\n", __func__);
		iounmap(airoha_map.virt);
		return ret;
	}
 	 /*if dts node exsit, use kernel mtd subsystem; UBI has no top-level art
	  * node, so skip the art offset backforward in that case. */
	if (!is_ubi) {
		ret = art_dts_offset_backforward(node);
		if(ret) {
			printk("\nERROR(%s) DTS partition error \n", __func__);
			iounmap(airoha_map.virt);
			return -1;
		}
	}

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
	mtd_device_parse_register(airoha_mtd_info, NULL, NULL, airoha_parts, nparts);

	if (IS_SPIFLASH) {
		ra_nand_init();
	}
	return 0;
}

static void __exit airoha_mtd_cleanup(void)
{
	if (airoha_mtd_info) {
		del_mtd_partitions(airoha_mtd_info);
		map_destroy(airoha_mtd_info);
	}
	if (IS_SPIFLASH) {
		ra_nand_remove();
	}

   	if (airoha_map.virt) {
   		iounmap(airoha_map.virt);
		airoha_map.virt = 0;
	}
}

device_initcall(airoha_mtd_init);
module_exit(airoha_mtd_cleanup);


