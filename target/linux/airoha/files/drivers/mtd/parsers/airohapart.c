// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2023 AIROHA Inc.
 * Author: Ray Liu <ray.liu@airoha.com>
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/memblock.h>
#include <linux/module.h>
#include <uapi/linux/magic.h>

#include <linux/mtd/mtk_bmt.h>

#define _SPI_CONTROLLER_REGS_BASE                   0x1FA10000
#define _SPI_CONTROLLER_REGS_SFC_STRAP              0x0114


#ifdef TCSUPPORT_OPENWRT
#define RESERVEAREA "art"
#else
#define RESERVEAREA "reservearea"
#endif 

static int airoha_map_init(void)
{
    void __iomem *SCU_SMB0ALS;
    unsigned int tmpVal;
    struct map_info airoha_map;

    SCU_SMB0ALS = ioremap_nocache(0x1fb00038, 4);
    tmpVal = readl(SCU_SMB0ALS);
    tmpVal &= 0xffe0e0e0;
    tmpVal |= 0x80070f00;
    writel(tmpVal, SCU_SMB0ALS);

    airoha_map.name = "airoha_map";
    airoha_map.bankwidth = 2;
    airoha_map.phys = 0x00000000;
    airoha_map.size = 0x02000000;
    printk("airoha_map: flash device 0x%08x at 0x%08x\n", airoha_map.size, airoha_map.phys);
    airoha_map.virt = ioremap_nocache(airoha_map.phys, airoha_map.size);

    ioremap_nocache(0x1fc00000, 0x400000);

    simple_map_init(&airoha_map);
    return 0;
}

static bool is_nor(void) { 	
	void __iomem *reg = ioremap((_SPI_CONTROLLER_REGS_BASE + _SPI_CONTROLLER_REGS_SFC_STRAP),4);
	u32 val = readl(reg); 
	iounmap(reg);
	if (val & 0x2) { 
		return false; 
	}
	return true; 
}


static bool is_emmc(void) {
	void __iomem *reg = ioremap(0x1fb000b8,4); 
	u32 val = readl(reg); 
	iounmap(reg);
	return (val & ( 0x01 << 6)); 
}

static int art_dts_offset_backforward(struct mtd_info* mtd, struct device_node *node)
{	
	int len = 0;
	struct device_node *art_node = NULL;
	struct property *art_reg_prop = NULL;	
	__be32 tmp;
	__be32 *reg = NULL;
	int *reg_modify = NULL;
	
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
		tmp = cpu_to_be32(mtd->size - be32_to_cpu(*(reg+1)));
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

static int airoha_create_mtd_partitions(struct mtd_info *master, const struct mtd_partition **pparts, struct mtd_part_parser_data *data)
{
    struct mtd_partition *airoha_parts;
    struct device_node *node;
    struct device_node *part_node;
    unsigned int offset, size;
    const char *mtd_name;
    int i = 0;
    int nparts;
    int ret;

    airoha_map_init();

	if (is_emmc()) { 
		node = of_find_node_by_path("/partitions");
	} else if (is_nor()) { 
		node = of_find_node_by_path("/partition-nor@1fa10000");
	} else {  // nand case
		mtk_bmt_attach(master);
		node = of_find_node_by_path("/partitions");
	}

    if(!node)
    {
        printk("\nERROR(%s) node==NULL\n", __func__);
        return -1;
    }
	ret = art_dts_offset_backforward(master,node);
	if(ret) {
		printk("\nERROR(%s) DTS partition error \n", __func__);
		return -1;
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
	if (!strcmp(mtd_name,"rootfs_data") || !(strcmp(mtd_name, "config"))) { 
		airoha_parts[i].mask_flags  |= (MTD_BIT_WRITEABLE);
	}
        i++;
    }

    *pparts = airoha_parts;
    return nparts;
}

static const struct of_device_id parse_airoha_match_table[] = {
    { .compatible = "airoha,airoha-partitions" },
    {},
};
MODULE_DEVICE_TABLE(of, parse_airoha_match_table);

static struct mtd_part_parser airoha_parser = {
    .parse_fn = airoha_create_mtd_partitions,
    .name = "airohapart",
    .of_match_table = parse_airoha_match_table,
};

static int __init airoha_parser_init(void) { 
	register_mtd_parser(&airoha_parser); 
}
static void __exit airoha_parser_exit(void)  { 
	deregister_mtd_parser(&airoha_parser);
}

module_init(airoha_parser_init); 
module_exit(airoha_parser_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ray Liu <ray.liu@airoha.com>");
MODULE_DESCRIPTION("MTD partitioning for AIROHA SoC");
