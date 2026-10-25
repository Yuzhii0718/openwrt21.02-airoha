// SPDX-License-Identifier: GPL-2.0
/*
 * MediaTek PCIe host controller driver.
 *
 * Copyright (c) 2020 MediaTek Inc.
 * Author: Jianjun Wang <jianjun.wang@mediatek.com>
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/msi.h>
#include <linux/pci.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/pm_runtime.h>
#include <linux/reset.h>
#include <linux/of_pci.h>

#include "../pci.h"
#include "asm/tc3162/tc3162.h"

#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#ifdef TCSUPPORT_CPU_ARMV8
#include <asm/uaccess.h>
#else
#include <asm/addrspace.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif

#include <linux/kthread.h>

#define PCIE_MSI_SET_NUM		8
#define PCIE_MSI_IRQS_PER_SET		32
#define PCIE_MSI_IRQS_NUM \
	(PCIE_MSI_IRQS_PER_SET * PCIE_MSI_SET_NUM)

static DECLARE_WAIT_QUEUE_HEAD(pcie_int13_dbg_wait);
static struct task_struct *pcie_int13_dbg_thread;
static bool kthread_running_int13 = false;

/*===========for PCIe pbus setting begin============================================ */
extern u32 GET_PBUS_PCIE0_BASE(void);
extern void SET_PBUS_PCIE0_BASE(u32 val);
extern u32 GET_PBUS_PCIE0_MASK(void);
extern void SET_PBUS_PCIE0_MASK(u32 val);
extern u32 GET_PBUS_PCIE1_BASE(void);
extern void SET_PBUS_PCIE1_BASE(u32 val);
extern u32 GET_PBUS_PCIE1_MASK(void);
extern void SET_PBUS_PCIE1_MASK(u32 val);
/*===========for PCIe pbus setting end============================================ */

/*===========for PCIe pad open drain setting begin============================================ */
extern u32 GET_SCU_RGS_OPEN_DRAIN(void);
extern void SET_SCU_RGS_OPEN_DRAIN(u32 val);
extern u32 GET_IOMUX_CTRL_3(void);

/*===========for PCIe pad open drain setting end============================================ */

/**********************************************************/
struct ecnt_pcie{
	struct device *dev;
	int irq_pcie0;
	int irq_pcie1;
	int irq_err0;
	int irq_err1;

	void __iomem *host_base; /* PCIe Mac0 base virtual address */
	void __iomem *mac0_base; /* PCIe Mac0 base virtual address */
	void __iomem *mac1_base; /* PCIe Mac1 base virtual address */

};
static struct ecnt_pcie ECNT_pcie;
/**********************************************************/
int base_num=0;

/*====================regs==================*/
u32 get_pcie_host_data(u32 reg)
{
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;
	//printk("\n=====get_pcie_host_data=%lx======\n",reg);

	return readl(ecnt_pcie->host_base+ reg);
}

u32 get_pcie_mac0_data(u32 reg)
{
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;

	return readl(ecnt_pcie->mac0_base+ reg);
}

void set_pcie_mac0_data(u32 reg, u32 val)
{
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;

	writel(val, ecnt_pcie->mac0_base + reg); 
}
u32 get_pcie_mac1_data(u32 reg)
{
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;

	return readl(ecnt_pcie->mac1_base+ reg);
}

void set_pcie_mac1_data(u32 reg, u32 val)
{
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;

	writel(val, ecnt_pcie->mac1_base + reg); 
}

u32 regRead_PCIe(u32 reg)		
{	
	u32 val;

	switch(reg&0xfffff000)
		{
		case 0x1fb00000:
			val=get_np_scu_data(reg&0xfff);
			break;
		case 0x1fa90000:
			val=get_pcie_host_data(reg&0xfff);
			break;
		case 0x1fa91000:
			val=get_pcie_mac0_data(reg&0xfff);
			break;
		case 0x1fa92000:
			val=get_pcie_mac1_data(reg&0xfff);
			break;
		}
	return val;		  
}		
void regWrite_PCIe(u32 reg, u32 val)	
{                                                	
    
	switch(reg&0xfffff000)
		{
		case 0x1fb00000:
			set_np_scu_data(reg&0xfff,val);
			break;
		case 0x1fa91000:
			set_pcie_mac0_data(reg&0xfff,val);
			break;
		case 0x1fa92000:
			set_pcie_mac1_data(reg&0xfff,val);
			break;
		}
}

EXPORT_SYMBOL(regRead_PCIe);
EXPORT_SYMBOL(regWrite_PCIe);


/*====================regs==================*/
/*====================conf access ecnt==================*/
#define isRC0_LINKUP		((regRead_PCIe(0x1fa91804) & 0x400) ? 1 : 0)  
#define isRC1_LINKUP		((regRead_PCIe(0x1fa92804) & 0x400) ? 1 : 0)

int get_rc_port(unsigned char bus,unsigned char dev)
{
	int rc = 4;

	if ((bus == 0) && (dev < 2))
    {
    	rc = dev;
    }
    else if ((bus == 1) && (dev == 0))
    {           
        rc = 0;
    }
    else if ((bus == 2) && (dev == 0) )
    {
        rc = 1;
    }
    
	return rc;
}

int pcie_write_config_word_extend(unsigned char bus, unsigned char dev,unsigned char func, unsigned int reg, unsigned long int value)
{
	unsigned int val,rc;
	unsigned long flags;
	void __iomem *offset=NULL;	
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;

	rc = get_rc_port(bus,dev);

	if (rc == 0){
		offset = ecnt_pcie->mac0_base;
	}else if(rc == 1){
		offset = ecnt_pcie->mac1_base;
	}else{
		return 0xffffffff;
	}

	//7552 bus/dev remap 
	if (0 == bus)
		dev = 0;
	else
		bus = 1;

	/*fmt=2|type=4|length=1 */
	val = (2 << 29) | (4 << 24) | 1;  

	 /* write TLP Header offset 0-3 */
	 writel_relaxed(val, 0x460+offset); 


	/*write requester ID */
	val = (rc<<19) | 0x070f;						

	/*write TLP Header offset 4-7 */
	writel_relaxed(val, 0x464+offset); 
	
	val = (bus << 24) | (dev << 19) |(func << 16) | reg;

	 /*write TLP Header offset 8-11 */
	 writel_relaxed(val, 0x468+offset); 

	/*write TLP data */
	writel_relaxed(value, 0x470+offset);    

	/*start TLP Requuest */
	writel_relaxed(1, 0x488+offset); 

	mdelay(1);

	val = 0;

	/*polling TLP Request status */
	while((val++)<10)             
	{
	 	/*TLP Request finished or timeout */
		if ((readl_relaxed(0x488+offset)&0x1)==0)  
			break;
		mdelay(1);
	}

	if (val==10)
		printk("\nPCIE Write Err: bus = %d, dev = %d Reg = %d",bus,dev,reg);

 	return 0;

}
EXPORT_SYMBOL(pcie_write_config_word_extend);

unsigned int pcie_read_config_word_extend(unsigned char bus,unsigned char dev,unsigned char func ,unsigned int reg)
{
	unsigned long flags;
	unsigned int val,rc;
	struct ecnt_pcie *ecnt_pcie=NULL;
	void __iomem *offset=NULL; 
	ecnt_pcie=&ECNT_pcie;
	
	rc = get_rc_port(bus,dev);


	if (rc == 0){
		offset = ecnt_pcie->mac0_base;
	}else if(rc == 1){
		offset = ecnt_pcie->mac1_base;
	}else{
		return 0xffffffff;
	}
#if 1
	if (0 == bus)
		dev = 0;
	else
		bus = 1;
#endif
	/* iniitialize the data reg */
	//writel(0xffffffff, 0x48c+offset); 


	/*fmt=2|type=4|length=1 */
	if((bus==0&&dev==0)||(bus==0&&dev==1)||(bus==3&&dev==0)||(bus==3&&dev==1))
	val = (4 << 24) | 1;  
	else
	val = (5 << 24) | 1;  	
	/*write TLP Header offset 0-3 */
	writel_relaxed(val, 0x460+offset); 

	/*write requester ID */
	val = (rc<<19) | 0x070f;//RC0/RC1											
	/*write TLP Header offset 4-7*/
	writel_relaxed(val, 0x464+offset); 

	val = (bus << 24) | (dev << 19) | (func << 16) | reg;
	/*write TLP Header offset 8-11*/
	writel_relaxed(val, 0x468+offset); 

	/*start TLP Requuest*/
	writel_relaxed(1, 0x488+offset); 

	mdelay(1);

	val = 0;

	/*polling TLP Request status */
	while((val++)<10)           
	{
		/*TLP Request finished or timeout*/
		if ((readl_relaxed(0x488+offset)&0x1)==0)  
			break;
		mdelay(1);
	}

	if (val==10)
	{
		printk("\n pcie_read_timeout: bus = %d, dev = %d, func = %d,reg = %x val = %x\n",bus,dev,func,reg,0xffffffff);
		return 0xffffffff;
	}

	/*return the data from data reg*/
	val = readl_relaxed(0x48c+offset);      


	return val;
}
EXPORT_SYMBOL(pcie_read_config_word_extend);

/*====================conf access ecnt==================*/


/**
 * struct mtk_msi_set - MSI information for each set
 * @base: IO mapped register base
 * @msg_addr: MSI message address
 * @saved_irq_state: IRQ enable state saved at suspend time
 */
struct mtk_msi_set {
	void __iomem *base;
	phys_addr_t msg_addr;
	u32 saved_irq_state;
};


/**
 * struct mtk_pcie_port - PCIe port information
 * @dev: pointer to PCIe device
 * @base: IO mapped register base
 * @reg_base: physical register base
 * @irq: PCIe controller interrupt number
 * @saved_irq_state: IRQ enable state saved at suspend time
 * @irq_lock: lock protecting IRQ register access
 * @intx_domain: legacy INTx IRQ domain
 * @msi_domain: MSI IRQ domain
 * @msi_bottom_domain: MSI IRQ bottom domain
 * @msi_sets: MSI sets information
 * @lock: lock protecting IRQ bit map
 * @msi_irq_in_use: bit map for assigned MSI IRQ
 */
struct mtk_pcie_port {
	struct device *dev;
	void __iomem *base;
	//struct list_head list;
	phys_addr_t reg_base;
	unsigned int busnr;
	//struct mtk_pcie *pcie;
	//u32 slot;

	int irq;
	int irq_err;
	u32 saved_irq_state;
	raw_spinlock_t irq_lock;
	struct irq_domain *intx_domain;
	struct irq_domain *msi_domain;
	struct irq_domain *msi_bottom_domain;
	struct mtk_msi_set msi_sets[PCIE_MSI_SET_NUM];
	struct mutex lock;
	DECLARE_BITMAP(msi_irq_in_use, PCIE_MSI_IRQS_NUM);
};

/* PCIe V2 configuration transaction header */
#define PCIE_CFG_HEADER0	0x460
#define PCIE_CFG_HEADER1	0x464
#define PCIE_CFG_HEADER2	0x468
#define PCIE_CFG_WDATA		0x470
#define PCIE_APP_TLP_REQ	0x488
#define PCIE_CFG_RDATA		0x48c
#define APP_CFG_REQ		BIT(0)
#define APP_CPL_STATUS		GENMASK(7, 5)

#define CFG_WRRD_TYPE_0		4
#define CFG_WR_FMT		2
#define CFG_RD_FMT		0

#define CFG_DW0_LENGTH(length)	((length) & GENMASK(9, 0))
#define CFG_DW0_TYPE(type)	(((type) << 24) & GENMASK(28, 24))
#define CFG_DW0_FMT(fmt)	(((fmt) << 29) & GENMASK(31, 29))
#define CFG_DW2_REGN(regn)	((regn) & GENMASK(11, 2))
#define CFG_DW2_FUN(fun)	(((fun) << 16) & GENMASK(18, 16))
#define CFG_DW2_DEV(dev)	(((dev) << 19) & GENMASK(23, 19))
#define CFG_DW2_BUS(bus)	(((bus) << 24) & GENMASK(31, 24))
#define CFG_HEADER_DW0(type, fmt) \
	(CFG_DW0_LENGTH(1) | CFG_DW0_TYPE(type) | CFG_DW0_FMT(fmt))
#define CFG_HEADER_DW1(where, size) \
	(GENMASK(((size) - 1), 0) << ((where) & 0x3))
#define CFG_HEADER_DW2(regn, fun, dev, bus) \
	(CFG_DW2_REGN(regn) | CFG_DW2_FUN(fun) | \
	CFG_DW2_DEV(dev) | CFG_DW2_BUS(bus))

static int mtk_pcie_check_cfg_cpld(void __iomem * offset)
{
	u32 val;
	int err;

	err = readl_poll_timeout_atomic(offset + PCIE_APP_TLP_REQ, val,
					!(val & APP_CFG_REQ), 10,
					100 * USEC_PER_MSEC);
	if (err)
		return PCIBIOS_SET_FAILED;

	if (readl_relaxed(offset + PCIE_APP_TLP_REQ) & APP_CPL_STATUS)
		return PCIBIOS_SET_FAILED;

	return PCIBIOS_SUCCESSFUL;
}

static int mtk_pcie_hw_rd_cfg(void __iomem * offset, u32 bus, u32 devfn,
			      int where, int size, u32 *val)
{
	u32 tmp;

	/* Write PCIe configuration transaction header for Cfgrd */
	writel_relaxed(CFG_HEADER_DW0(CFG_WRRD_TYPE_0, CFG_RD_FMT),
	       offset + PCIE_CFG_HEADER0);
	writel_relaxed(CFG_HEADER_DW1(where, size), offset + PCIE_CFG_HEADER1);
	writel_relaxed(CFG_HEADER_DW2(where, PCI_FUNC(devfn), PCI_SLOT(devfn), bus),
	       offset + PCIE_CFG_HEADER2);

	/* Trigger h/w to transmit Cfgrd TLP */
	tmp = readl_relaxed(offset + PCIE_APP_TLP_REQ);
	tmp |= APP_CFG_REQ;
	writel_relaxed(tmp, offset + PCIE_APP_TLP_REQ);

	/* Check completion status */
	if (mtk_pcie_check_cfg_cpld(offset))
		return PCIBIOS_SET_FAILED;

	/* Read cpld payload of Cfgrd */
	*val = readl_relaxed(offset + PCIE_CFG_RDATA);

	if (size == 1)
		*val = (*val >> (8 * (where & 3))) & 0xff;
	else if (size == 2)
		*val = (*val >> (8 * (where & 3))) & 0xffff;

	return PCIBIOS_SUCCESSFUL;
}

static int mtk_pcie_hw_wr_cfg(void __iomem * offset, u32 bus, u32 devfn,
			      int where, int size, u32 val)
{
	/* Write PCIe configuration transaction header for Cfgwr */
	writel_relaxed(CFG_HEADER_DW0(CFG_WRRD_TYPE_0, CFG_WR_FMT),
	       offset + PCIE_CFG_HEADER0);
	writel_relaxed(CFG_HEADER_DW1(where, size), offset + PCIE_CFG_HEADER1);
	writel_relaxed(CFG_HEADER_DW2(where, PCI_FUNC(devfn), PCI_SLOT(devfn), bus),
	       offset + PCIE_CFG_HEADER2);

	/* Write Cfgwr data */
	val = val << 8 * (where & 3);
	writel_relaxed(val, offset + PCIE_CFG_WDATA);

	/* Trigger h/w to transmit Cfgwr TLP */
	val = readl_relaxed(offset + PCIE_APP_TLP_REQ);
	val |= APP_CFG_REQ;
	writel_relaxed(val, offset + PCIE_APP_TLP_REQ);

	/* Check completion status */
	return mtk_pcie_check_cfg_cpld(offset);
}

static void __iomem *mtk_pcie_find_port_offset(struct pci_bus * bus,	unsigned int devfn)
{
	unsigned int rc;
	void __iomem *offset=NULL; 
	struct mtk_pcie_port *port = bus->sysdata;		
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;

	offset=port->base;

	return offset;
}

static int mtk_pcie_config_read(struct pci_bus *bus, unsigned int devfn,
				int where, int size, u32 *val)
{
	u32 bn = bus->number;
	int ret;
	void __iomem *offset=NULL; 

	if(PCI_SLOT(devfn)>0)
		return 0;

	offset = mtk_pcie_find_port_offset(bus, devfn);
	//printk("\n===========mtk_pcie_config_read=====bn===%x==dev=%x====offset=%x========\n",bn,PCI_SLOT(devfn),offset);
	
	if (0xffffffff==offset) {
		*val = ~0;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	ret = mtk_pcie_hw_rd_cfg(offset, bn, devfn, where, size, val);
	if (ret)
		*val = ~0;

	return ret;
}

static int mtk_pcie_config_write(struct pci_bus *bus, unsigned int devfn,
				 int where, int size, u32 val)
{
	u32 bn = bus->number;
	void __iomem *offset=NULL; 

	offset = mtk_pcie_find_port_offset(bus, devfn);
	if (0xffffffff==offset) {
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	return mtk_pcie_hw_wr_cfg(offset, bn, devfn, where, size, val);
}

static struct pci_ops mtk_pcie_ops = {
	.read  = mtk_pcie_config_read,
	.write = mtk_pcie_config_write,
};

void mt7512_pcie_reset(void)
{
	unsigned int tmp;
	printk("\n===mt7512_pcie_reset mdelay 600===\n");

	/*before reset host,need to pull device low*/
	//port0 port1 0x1fb00088 bit29 bit 26
	tmp = regRead_PCIe(0x1fb00088);
	regWrite_PCIe(0x1fb00088, (tmp & (~((1<<29) | (1<<26)))));
	mdelay(1);

	/* enabled PCIe port 1 */
	tmp = regRead_PCIe(0x1fb00088);
	regWrite_PCIe(0x1fb00088, (tmp | (1<<22)));
	mdelay(1);

    tmp = regRead_PCIe(0x1fb00834);
    regWrite_PCIe(0x1fb00834, (tmp & (~( (1<<26) | (1<<27) | (1<<29)))));
    mdelay(1);

    tmp = regRead_PCIe(0x1fb00834);
    regWrite_PCIe(0x1fb00834, (tmp | ( (1<<26) | (1<<27) | (1<<29))));
    mdelay(100);



	
	/*first reset to default*///0xbfb00834-pulse-0000_11111_0000
    
    //hostcontroller port0 port1 0x1fb00834 bit29 bit26 bit27
    tmp = regRead_PCIe(0x1fb00834);
    regWrite_PCIe(0x1fb00834, (tmp & (~( (1<<26) | (1<<27) | (1<<29)))));
    mdelay(600);

    /*release device*///0xbfb00088-0000_11111
    tmp = regRead_PCIe(0x1fb00088);
    regWrite_PCIe(0x1fb00088, (tmp & (~((1<<29) | (1<<26)))));
    mdelay(1);

    tmp = regRead_PCIe(0x1fb00088);
    regWrite_PCIe(0x1fb00088, (tmp | ((1<<29) | (1<<26))));
	mdelay(1);


	/*wait link up*/
	//mdelay(10000); 
	mdelay(250);

	return ;
}

#define K_GBL_1			0x000
#define K_CONF_FUNC0_1		0x104
#define PCIE_INT_MASK		0x420
#define PCIE_INT_STATUS		0x424

#define INTX_MASK		GENMASK(19, 16)
#define PCIE_LINK_STATUS_V2	0x804
#define PCIE_PORT_LINKUP_V2	BIT(10)
#define PCIE_AHB_TRANS_BASE0_L	0x438
#define PCIE_AHB_TRANS_BASE0_H	0x43c
#define PCIE_AXI_WINDOW0	0x448

#define PCIE_RSTCR	0x510

#define PCIE_MMIO_CTRL	0x564

#define AHB2PCIE_SIZE(x)	((x) & GENMASK(4, 0))


static int mtk_pcie_startup_port(struct mtk_pcie_port *port)
{
	struct resource_entry *entry;
	struct pci_host_bridge *host = pci_host_bridge_from_priv(port);
	unsigned int table_index = 0;
	int err;
	u32 val;

		/* Set as RC mode */
		val = 0x804201;
		writel_relaxed(val, port->base + K_GBL_1);;//set as RC Mode
			
		/* Set class code */
		val = 0x06040001;
		writel_relaxed(val, port->base + K_CONF_FUNC0_1);//class code &reversion id

		/* Set INTx mask */
		val = readl_relaxed(port->base + PCIE_INT_MASK);
		val &= ~INTX_MASK;
		val &= ~(1<<13);
		writel_relaxed(val, port->base + PCIE_INT_MASK);

	/* 100ms timeout value should be enough for Gen1/2 training */
	err = readl_poll_timeout(port->base + PCIE_LINK_STATUS_V2, val,
				 !!(val & PCIE_PORT_LINKUP_V2), 20,
				 100 * USEC_PER_MSEC);
	if (err)
	{
		dev_err(port->dev, "PCIe link down\n");
		return -ETIMEDOUT;
	}
	
	dev_info(port->dev, "pcie rc %d linkup success\n", base_num);

	/* Set PCIe translation windows */
	resource_list_for_each_entry(entry, &host->windows) {
		struct resource *res = entry->res;
		unsigned long type = resource_type(res);
		resource_size_t cpu_addr;
		resource_size_t pci_addr;
		resource_size_t size;
		const char *range_type;

		if (type == IORESOURCE_IO) {
			cpu_addr = pci_pio_to_address(res->start);
			range_type = "IO";
		} else if (type == IORESOURCE_MEM) {
			cpu_addr = res->start;
			range_type = "MEM";
		} else {
			continue;
		}

		/* Set AHB to PCIe translation windows */
		size = resource_size(res);
		val = lower_32_bits(cpu_addr) | AHB2PCIE_SIZE(fls(size));
		writel_relaxed(val, port->base + PCIE_AHB_TRANS_BASE0_L);
		val = upper_32_bits(cpu_addr);
		writel_relaxed(val, port->base + PCIE_AHB_TRANS_BASE0_H);


		pci_addr = res->start - entry->offset;

		dev_info(port->dev, "set %s trans window[%d]: cpu_addr = %#llx, pci_addr = %#llx, size = %#llx\n",
			range_type, table_index, (unsigned long long)cpu_addr,
			(unsigned long long)pci_addr, (unsigned long long)size);

		table_index++;
	}

	/* Set PCIe to AXI translation memory space.*/
	val=0x80;
	writel_relaxed(val, port->base + PCIE_AXI_WINDOW0);

	/* 7563+kite coherence bugfix.*/
	val = readl_relaxed(port->base + PCIE_MMIO_CTRL);
	dev_info(port->dev, "======orig===PCIE_MMIO_CTRL %x\n", val);
	val |= 1;
	writel_relaxed(val, port->base + PCIE_MMIO_CTRL);
	val = readl_relaxed(port->base + PCIE_MMIO_CTRL);
	dev_info(port->dev, "======after===PCIE_MMIO_CTRL %x\n", val);

	return 0;
}

static int pcie_err_irq_handler(int irq)
{
	u32 val,val1;
	u32 base;
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;
	if (!ecnt_pcie)
		return -ENOMEM;

	//printk("\n====pcie_err_irq_handler=11=irq=%x=======\n",irq);

	val = regRead_PCIe(0x1fa90068);
	val1 = regRead_PCIe(0x1fa9006c);
	printk("\n 11pcie_err_irq_handler 0x1fa90068 = %x  6c = %x \n", val,val1);
	val = regRead_PCIe(0x1fa915b0);
	val1 = regRead_PCIe(0x1fa925b0);
	printk("\n 11pcie_err_irq_handler 0x1fa915b0 = %x  25b0 = %x \n", val,val1);
	val = regRead_PCIe(0x1fa90068);
	val1 = regRead_PCIe(0x1fa9006c);
	printk("\n 22pcie_err_irq_handler 0x1fa90068 = %x  6c = %x \n", val,val1);
	val = regRead_PCIe(0x1fa915b0);
	val1 = regRead_PCIe(0x1fa925b0);
	printk("\n 22pcie_err_irq_handler 0x1fa915b0 = %x  25b0 = %x \n", val,val1);


	if(irq==ecnt_pcie->irq_err0)
		base = ecnt_pcie->mac0_base;
	else if(irq==ecnt_pcie->irq_err1)
		base = ecnt_pcie->mac1_base;
	printk("\n====pcie_err_irq_handler=11=irq=%x=base=%x===\n",irq,base);

	val = readl_relaxed(base + PCIE_INT_STATUS);
	printk("\n===pcie_err_irq_handler=11=INT_STATUS=%x===============\n",val);

	writel_relaxed(val,base + PCIE_INT_STATUS);//W1C
	
	val = readl_relaxed(base + PCIE_INT_STATUS);
	printk("\n===pcie_err_irq_handler=12=INT_STATUS=%x===============\n",val);

	//mask int13
	val = readl_relaxed(base + PCIE_INT_MASK);
	val &= ~INTX_MASK;
	val |= (1<<13);
	writel_relaxed(val, base + PCIE_INT_MASK);

	kthread_running_int13 = true;	
	wake_up_interruptible(&pcie_int13_dbg_wait);

	return IRQ_HANDLED;
}


static int mtk_pcie_setup_irq(struct mtk_pcie_port *port)
{
	struct device *dev = port->dev;
	struct platform_device *pdev = to_platform_device(dev);
	int err;
	int retval=0;

	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;
	if (!ecnt_pcie)
		return -ENOMEM;

	port->irq = platform_get_irq(pdev, 0);
	port->irq_err = platform_get_irq(pdev, 1);
	printk("\n==mtk_pcie_setup_irq==port->irq %lx==,port->irq_err %lx=====\n",port->irq,port->irq_err);
	
	switch(base_num)
	{
		case 0:
			ecnt_pcie->irq_pcie0 = port->irq;
			ecnt_pcie->irq_err0 = port->irq_err;
			break;
		case 1:
			ecnt_pcie->irq_pcie1 = port->irq;
			ecnt_pcie->irq_err1 = port->irq_err;
			break;
	}

	retval = devm_request_irq(&pdev->dev,port->irq_err,pcie_err_irq_handler,0,"pcie_err",NULL);
	if(retval!=0)
		printk("\n====mtk_pcie_setup_irq=request_irq ERROR(%d)=====\n",retval);
	
	return 0;
}

static int mtk_pcie_parse_port(struct mtk_pcie_port *port)
{
	struct device *dev = port->dev;
	struct platform_device *pdev = to_platform_device(dev);
	struct resource *regs;

	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;
	if (!ecnt_pcie)
		return -ENOMEM;

	regs = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pcie-mac");
	port->base = devm_ioremap_resource(dev, regs);
	if (IS_ERR(port->base)) {
		dev_err(dev, "failed to map register base\n");
		return PTR_ERR(port->base);
	}

	port->reg_base = regs->start;

	if((port->reg_base&0xfffff000)==0x1fa90000)
		base_num= 0;
	else if((port->reg_base&0xfffff000)==0x1fa92000)
		base_num= 1;
	
	printk("\n=====pcie_parse_port=11===base_num=%d===port->reg_base=%x==port->base=%x========",base_num,port->reg_base,port->base);

	switch(base_num)
	{
		case 0:
			printk("\n=====pcie_parse_port=33===base_num=%d===port->reg_base=%x==port->base=%x========",base_num,port->reg_base,port->base);
			ecnt_pcie->host_base = port->base;
			ecnt_pcie->mac0_base = port->base + 0x1000;
			port->base = port->base + 0x1000;
			port->reg_base =port->reg_base + 0x1000;
			printk("\n=====pcie_parse_port=44===base_num=%d===port->reg_base=%x==port->base=%x========",base_num,port->reg_base,port->base);
			break;
		case 1:
			ecnt_pcie->mac1_base = port->base;
			break;
	}

	printk("\n=====pcie_parse_port=22==ecnt_pcie->mac0_base=%lx=%lx==%lx==========\n",ecnt_pcie->host_base,ecnt_pcie->mac0_base,ecnt_pcie->mac1_base);

	return 0;
}

static int mtk_pcie_setup(struct mtk_pcie_port *port)
{
	struct device *dev = port->dev;
	struct pci_host_bridge *host = pci_host_bridge_from_priv(port);
	struct list_head *windows = &host->windows;
	struct resource *bus;
	int err;
	u32 tmp_reg;

	err = pci_parse_request_of_pci_ranges(dev, windows, &bus);
	if (err)
		return err;

	port->busnr = bus->start;

	err = mtk_pcie_parse_port(port);
	if (err)
		return err;

	/* Try link up */

	/*===========for PCIe pad open drain setting begin============================================ */
#ifndef TCSUPPORT_WLAN_MT7916D
	tmp_reg = GET_SCU_RGS_OPEN_DRAIN();
	printk("\n========pcie_probe==before==0x1fa2018c=%x===========",tmp_reg);
	SET_SCU_RGS_OPEN_DRAIN(0x7);//bit[2:0] map port2/1/0,1:open drain
	tmp_reg = GET_SCU_RGS_OPEN_DRAIN();
	printk("\n========pcie_probe==after==0x1fa2018c=%x===========\n",tmp_reg);
#endif	
	/*===========for PCIe pad open drain setting end============================================ */

	if(0==base_num)
	{	
		printk("\n===EN7552 PCIe init===\n");
		mt7512_pcie_reset();
	}

	err = mtk_pcie_startup_port(port);
	if (err)
		return err;

	err = mtk_pcie_setup_irq(port);
	if (err)
		return err;

	return 0;
}



int mt7512_pcie_get_pos(char bus,char dev)
{
	unsigned int val,pos;

	val = pcie_read_config_word_extend(bus,dev,0,0x34);
	pos = val&0xff;
	while(pos && pos != 0xff)
	{
		val = pcie_read_config_word_extend(bus,dev,0,pos);
		if ( (val&0xff) == 0x10)
			return pos;
		pos = (val >> 0x08) & 0xff;
	}
	return 0;
}

int  mt7512_pcie_rc0_retrain(void)
{
	unsigned int pos = 0, ppos = 0,bus;
	unsigned int  linkcap, plinkcap,plinksta;

	ppos = mt7512_pcie_get_pos(0,0);//bus0 dev0 rc0
	bus =  1;                     //bus1 dev0 
	
	pos = mt7512_pcie_get_pos(bus,0);  //bus1 dev0
	
	if (pos <0x40 || ppos < 0x40)
		return 0;
	
	plinkcap =  pcie_read_config_word_extend(0,0,0,ppos+0x0c);//bus0 dev0 rc0
	linkcap = pcie_read_config_word_extend(bus,0,0,pos+0x0c);//bus1 dev0 ep0
	
	printk("\n mt7512_pcie_rc0_retrain: %x = %08x %x = %08x \n",pos,linkcap,ppos,plinkcap);
	
	if ((linkcap&0x0f)== 1 || (plinkcap&0x0f)==1)
		return 0;
	
	plinksta = pcie_read_config_word_extend(0,0,0,ppos+0x10);//bus0 dev0 rc0
	if( ((plinksta>>16)&0x0f) ==  (plinkcap&0x0f))
		return 0;

	plinksta =  pcie_read_config_word_extend(0,0,0,ppos+0x10);//bus0 dev0 rc0
	plinksta |= 0x20;
	pcie_write_config_word_extend(0,0,0,ppos+0x10,plinksta);//bus0 dev0 rc0
	
	mdelay(1000); 
	
	plinksta =  pcie_read_config_word_extend(0,0,0,ppos+0x10);//bus0 dev0 rc0
	
	printk("\nRC0 Link Traing Result: %08x \n",plinksta);//rc0
	
	return 1;
}


int  mt7512_pcie_rc1_retrain(void)
{
	unsigned int pos = 0, ppos = 0,bus;
	unsigned int  linkcap, plinkcap,plinksta;

	ppos = mt7512_pcie_get_pos(0,1);
	bus =  2;
	
	pos = mt7512_pcie_get_pos(bus,0);
	
	if (pos <0x40 || ppos < 0x40)
		return 0;
	
	plinkcap =  pcie_read_config_word_extend(0,1,0,ppos+0x0c);
	linkcap = pcie_read_config_word_extend(bus,0,0,pos+0x0c);
	
	printk("\n mt7512_pcie_rc1_retrain: %x = %08x %x = %08x \n",pos,linkcap,ppos,plinkcap);
	
	if ((linkcap&0x0f)== 1 || (plinkcap&0x0f)==1)
		return 0;
	
	plinksta = pcie_read_config_word_extend(0,1,0,ppos+0x10);
	if( ((plinksta>>16)&0x0f) ==  (plinkcap&0x0f))
		return 0;

	plinksta =  pcie_read_config_word_extend(0,1,0,ppos+0x10);
	plinksta |= 0x20;
	pcie_write_config_word_extend(0,1,0,ppos+0x10,plinksta);
	
	mdelay(1000); 
	
	plinksta =  pcie_read_config_word_extend(0,1,0,ppos+0x10);
	
	printk("\nRC1 Link Traing Result: %08x \n",plinksta);
	
	return 1;
}

void mt7512_pcie_fixup_rc0(void)
{
	unsigned int val = 0,tmp = 0 ,i = 0;
		
	if (isRC0_LINKUP)
	{
		val =  pcie_read_config_word_extend(0,0,0,0x20);
		tmp = ((val&0xffff)<<16);
		val = (val&0xffff0000) + 0x100000;
		val = val - tmp;
		i = 0;
		while(i < 32)
		{
			if((1<<i) >= val)
				break;
			i++;
		}
		regWrite_PCIe(0x1fa91438,tmp | i);   //config RC0 to EP Addr window
		mdelay(1);
		regWrite_PCIe(0x1fa91448,0x80);     //enable EP to RC0 access
		printk("\n mt7512_pcie_fixup: 0x1438 = %x ",tmp | i);

		mt7512_pcie_rc0_retrain();
	}

	return ;
}


void mt7512_pcie_fixup_rc1(void)
{
	unsigned int val = 0,tmp = 0 ,i = 0;
	
	if (isRC1_LINKUP)
	{
	
		val =  pcie_read_config_word_extend(0,1,0,0x20);
		tmp = ((val&0xffff)<<16);
		val = (val&0xffff0000) + 0x100000;
		val = val - tmp;
		i = 0;
		while(i < 32)
		{
			if((1<<i) >= val)
				break;
			i++;
		}
		regWrite_PCIe(0x1fa92438,tmp | i);   //config RC1 to EP Addr window
		mdelay(1);
		regWrite_PCIe(0x1fa92448,0x80);     //enable EP to RC1 access
		printk("\n mt7512_pcie_fixup: 0x2438 = %x ",tmp | i);
		
		mt7512_pcie_rc1_retrain();
	}

	return ;
}


void mt7512_pcie_fixup(void)
{
	unsigned int val = 0,tmp = 0 ,i = 0;
	
	if (isRC0_LINKUP)
	{
		val =  pcie_read_config_word_extend(0,0,0,0x20);
		tmp = ((val&0xffff)<<16);
		val = (val&0xffff0000) + 0x100000;
		val = val - tmp;
		i = 0;
		while(i < 32)
		{
			if((1<<i) >= val)
				break;
			i++;
		}
		regWrite_PCIe(0x1fa91438,tmp | i);   //config RC0 to EP Addr window
		mdelay(1);
		regWrite_PCIe(0x1fa91448,0x80);     //enable EP to RC0 access
		printk("\n mt7512_pcie_fixup: 0x1438 = %x ",tmp | i);

		mt7512_pcie_rc0_retrain();	
	}
	
	if (isRC1_LINKUP)
	{
		val =  pcie_read_config_word_extend(0,1,0,0x20);
		tmp = ((val&0xffff)<<16);
		val = (val&0xffff0000) + 0x100000;
		val = val - tmp;
		i = 0;
		while(i < 32)
		{
			if((1<<i) >= val)
				break;
			i++;
		}
		regWrite_PCIe(0x1fa92438,tmp | i);   //config RC1 to EP Addr window
		mdelay(1);
		regWrite_PCIe(0x1fa92448,0x80);     //enable EP to RC1 access
		printk("\n mt7512_pcie_fixup: 0x2438 = %x ",tmp | i);
		
		mt7512_pcie_rc1_retrain();
	}

	return ;
}

EXPORT_SYMBOL(mt7512_pcie_fixup);

int pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{

	int irq0=0,irq1=0;
	struct ecnt_pcie *ecnt_pcie=NULL;
	ecnt_pcie=&ECNT_pcie;
	
	unsigned int id = pci_domain_nr(dev->bus);
	
	
	irq0=ecnt_pcie->irq_pcie0;
	irq1=ecnt_pcie->irq_pcie1;	
	
	printk("\n========pcibios_map_irq === id=%d, irq0=%d, irq1=%d ===========", id, irq0, irq1);		
	
	if (id == 0)
		return irq0;
	else if(id == 1)
		return irq1;	
}

/*
printk("reg name:{ECRC, 	TLP CRC,   DLLP CRC,  Replay Timeout,Replay Rollover,TLP Unexpectd}\n");
printk("RC0 reg:{0x1fa90054,0x1fa901d8,0x1fa901dc,0x1fa901e0,	 0x1fa901e4,	 0x1fa901e8}\n");
printk("RC1 reg:{0x1fa90058,0x1fa901ec,0x1fa901f0,0x1fa901f4,	 0x1fa901f8,	 0x1fa901fc}\n");	
*/

void print_dbg_regs_pcie(void)
{
	u32 linksta=0,linksta1=0;

	printk("\n============print pcie registers begin============\n");	
	linksta = GET_PBUS_PCIE0_MASK();
	printk("\n========pcie_mask-port0====0x1fbe3404=%x===========",linksta);
	linksta = GET_PBUS_PCIE1_MASK();
	printk("\n========pcie_mask-port1====0x1fbe340c=%x===========\n",linksta);
	
	linksta = regRead_PCIe(0x1fb00088);
	printk(" pcie reset reg 0x1fb00088 port0/port1,bit29/26, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fb00834);
	printk(" pcie reset reg 0x1fb00834 host/port0/port1,bit29/26/27, value = %x \n", linksta);
	
	linksta = GET_SCU_RGS_OPEN_DRAIN();
	printk(" pcie opendrain reg 0x1fa2018c bit[1:0] map port1/0,1:open drain, value = %x \n", linksta);
	
	linksta = GET_IOMUX_CTRL_3();
	printk(" pcie MUX gpio reg 0x1fa2021c bit[10:9] map port1/0,1:gpio, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa90050);
	printk(" pcie linkstatus bit1/bit2:port0/port1 0x1fa90050, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa90054);
	printk(" pcie port0 ECRC 0x1fa90054, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901d8);
	printk(" pcie port0 TLP CRC 0x1fa901d8, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901dc);
	printk(" pcie port0 DLLP CRC 0x1fa901dc, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901e0);
	printk(" pcie port0 Replay Timeout 0x1fa901e0, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901e4);
	printk(" pcie port0 Replay Rollover 0x1fa901e4, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901e8);
	printk(" pcie port0 TLP Unexpectd 0x1fa901e8, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa90058);
	printk(" pcie port1 ECRC 0x1fa90058, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901ec);
	printk(" pcie port1 TLP CRC 0x1fa901ec, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901f0);
	printk(" pcie port1 DLLP CRC 0x1fa901f0, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901f4);
	printk(" pcie port1 Replay Timeout 0x1fa901f4, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901f8);
	printk(" pcie port1 Replay Rollover 0x1fa901f8, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa901fc);
	printk(" pcie port1 TLP Unexpectd 0x1fa901fc, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa90060);
	printk(" pcie port0 err_status :0x1fa90060, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa90064);
	printk(" pcie port1 err_status :0x1fa90064, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa90068);
	printk(" pcie port0 ltssm bit[4:0]:0x1fa90068, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa9006c);
	printk(" pcie port1 ltssm bit[4:0]:0x1fa9006c, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa901A0);
	linksta1 = regRead_PCIe(0x1fa901A4);
	printk(" pcie 11 0x1fa901A0 value = %x 0x1fa901A4 value = %x \n", linksta,linksta1);

	linksta = regRead_PCIe(0x1fa901D0);
	linksta1 = regRead_PCIe(0x1fa901D4);
	printk(" pcie 11 0x1fa901D0 value = %x 0x1fa901D4 value = %x \n", linksta,linksta1);
	
	linksta = regRead_PCIe(0x1fa901A0);
	linksta1 = regRead_PCIe(0x1fa901A4);
	printk(" pcie 22 0x1fa901A0 value = %x 0x1fa901A4 value = %x \n", linksta,linksta1);

	linksta = regRead_PCIe(0x1fa901D0);
	linksta1 = regRead_PCIe(0x1fa901D4);
	printk(" pcie 22 0x1fa901D0 value = %x 0x1fa901D4 value = %x \n", linksta,linksta1);

	linksta = regRead_PCIe(0x1fa901A0);
	linksta1 = regRead_PCIe(0x1fa901A4);
	printk(" pcie 33 0x1fa901A0 value = %x 0x1fa901A4 value = %x \n", linksta,linksta1);

	linksta = regRead_PCIe(0x1fa901D0);
	linksta1 = regRead_PCIe(0x1fa901D4);
	printk(" pcie 33 0x1fa901D0 value = %x 0x1fa901D4 value = %x \n", linksta,linksta1);
	
	linksta = regRead_PCIe(0x1fa90404);
	printk(" pcie port0 rerr_addr 0x1fa90404, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa90408);
	printk(" pcie port1 rerr_addr 0x1fa90408, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa91424);
	printk(" pcie port0 int status:0x1fa91424, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa92424);
	printk(" pcie port1 int status:0x1fa92424, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa915c8);
	printk(" pcie port0 error statistic count3 bit[7:0]:0x1fa915c8, value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa925c8);
	printk(" pcie port1 error statistic count3 bit[7:0]:0x1fa925c8, value = %x \n", linksta);
	
	linksta = regRead_PCIe(0x1fa915b0);
	printk(" pcie port0 0x1fa915b0 value = %x \n", linksta);
	linksta = regRead_PCIe(0x1fa925b0);
	printk(" pcie port1 0x1fa925b0 value = %x \n", linksta);
	
	printk("============print pcie registers end============\n");
	return ;
}
EXPORT_SYMBOL(print_dbg_regs_pcie);

static void kthread_pcie_int13_dbg(void)
{
	u32 linksta=0,linksta1=0;

linksta = regRead_PCIe(0x1fa90068);
	linksta1 = regRead_PCIe(0x1fa9006c);
	printk("\n 11kthread_pcie_int13_dbg 0x1fa90068 = %x  6c = %x \n", linksta,linksta1);

	linksta = regRead_PCIe(0x1fa915b0);
	linksta1 = regRead_PCIe(0x1fa925b0);
	printk("\n 11kthread_pcie_int13_dbg 0x1fa915b0 = %x  25b0 = %x \n", linksta,linksta1);


	print_dbg_regs_pcie();

	linksta = regRead_PCIe(0x1fa90068);
	linksta1 = regRead_PCIe(0x1fa9006c);
	printk("\n 22kthread_pcie_int13_dbg 0x1fa90068 = %x  6c = %x \n", linksta,linksta1);

	linksta = regRead_PCIe(0x1fa915b0);
	linksta1 = regRead_PCIe(0x1fa925b0);
	printk("\n 22kthread_pcie_int13_dbg 0x1fa915b0 = %x  25b0 = %x \n", linksta,linksta1);

	/* TODO */
        //do reboot

    return ;
}

static int pcie_int13_thread_act(void *arg)
{
    while (1)
    {
        wait_event_interruptible(pcie_int13_dbg_wait, kthread_running_int13 || kthread_should_stop());

        kthread_pcie_int13_dbg();
		
	kthread_running_int13 = false;
    }

    return 0;
}

static int mtk_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mtk_pcie_port *port;
	struct pci_host_bridge *host;
	int err;
	int value=0;
	u32 check_reg;
	
	u32 tmp_reg,val;

    void *virtAddr;
	static u32 count = 0;

	if (isAN7552ST){		
		if(0 == count){
			virtAddr = ioremap((phys_addr_t)0x1fa90044, 4);
			val = readl(virtAddr);
			val = val | ((0xff<<16)|0xff);
			writel(val, virtAddr);
			
			val = readl(virtAddr);
			printk("\n========mtk_pcie_probe=after===0x1fa90044=%x===========",val);		
			iounmap(virtAddr);	
			
			val=get_chip_scu_data(0x1e4);	
			val = val & (~(0xf<<6));
			set_chip_scu_data(0x1e4, val);
			
			val=get_chip_scu_data(0x1e4);	
			printk("\n========mtk_pcie_probe=after===0x1fa201e4=%x===========",val);		
		}		
		count++;
		return 1;
	}

	host = devm_pci_alloc_host_bridge(dev, sizeof(*port));
	if (!host)
		return -ENOMEM;

	port = pci_host_bridge_priv(host);

	port->dev = dev;
	platform_set_drvdata(pdev, port);

	err = mtk_pcie_setup(port);
  	//work around for accidental calltrace when NPU access port1 mac register but only port0 linkup
	//if (err)
	//	goto release_resource;

	//check whether PCIe exsits or not
	if(0==base_num)
	{
		check_reg = regRead_PCIe(0x1fa91100);
		printk("\n===============PCIe check===tmp_reg=========%x===========\n",check_reg);

		if(0x081014c3!=check_reg)
			return 0;
	}
	else if(1==base_num)
	{
		check_reg = regRead_PCIe(0x1fa92100);
		printk("\n===============PCIe check===tmp_reg=========%x===========\n",check_reg);

		if(0x081114c3!=check_reg)
			return 0;
	}

	host->busnr = port->busnr;
	host->dev.parent = port->dev;
	host->ops = &mtk_pcie_ops;
//	host->map_irq = of_irq_parse_and_map_pci;
	
	host->map_irq = pcibios_map_irq;
	host->swizzle_irq = pci_common_swizzle;
	host->sysdata = port;

	err = pci_host_probe(host);

	if(0==base_num)
	{
		//port0
		mt7512_pcie_fixup_rc0();

		//for irq
		value=pcie_read_config_word_extend(0,0,0,0x3c);
		value|= port->irq;
		pcie_write_config_word_extend(0,0,0, 0x3c,value);
		pcie_write_config_word_extend(1,0,0, 0x3c,value);

		//for PCIe pbus setting
		printk("===EN7552 Pbus 0 for PCIe init===");
		val = regRead_PCIe(0x1fa91000 + PCIE_AHB_TRANS_BASE0_L);
		tmp_reg = GET_PBUS_PCIE0_BASE();
		printk("\n========pcie_probe====0x1fbe3400=%x===========",tmp_reg);
		SET_PBUS_PCIE0_BASE(val & 0xffffff80);
		tmp_reg = GET_PBUS_PCIE0_MASK();
		printk("\n========pcie_probe====0x1fbe3404=%x===========",tmp_reg);
		val = val & 0x1f;
		SET_PBUS_PCIE0_MASK(0xffffffff << val);
		printk("\n========pcie_probe====after pbus setting=======");
		tmp_reg = GET_PBUS_PCIE0_BASE();
		printk("\n========pcie_probe====0x1fbe3400=%x===========",tmp_reg);
		tmp_reg = GET_PBUS_PCIE0_MASK();
		printk("\n========pcie_probe====0x1fbe3404=%x===========",tmp_reg);
		//for debug detect linkdown
		val = regRead_PCIe(0x1fa91000 + PCIE_RSTCR);	
		printk("\n========pcie_probe====0x1fa91510=%x===========",val);
		regWrite_PCIe(0x1fa91000 + PCIE_RSTCR,val & (~(7<<13)));	
	}
	else if(1==base_num)
	{
		//port1
		mt7512_pcie_fixup_rc1();

		//for irq
		value=pcie_read_config_word_extend(0,1,0,0x3c);
		value|= port->irq;
		pcie_write_config_word_extend(0,1,0, 0x3c,value);
		pcie_write_config_word_extend(2,0,0, 0x3c,value);

		//for PCIe pbus setting
		printk("===EN7552 Pbus 1 for PCIe init===");
		val = regRead_PCIe(0x1fa92000 + PCIE_AHB_TRANS_BASE0_L);
		tmp_reg = GET_PBUS_PCIE1_BASE();
		printk("\n========pcie_probe====0x1fbe3408=%x===========",tmp_reg);
		SET_PBUS_PCIE1_BASE(val & 0xffffff80);
		tmp_reg = GET_PBUS_PCIE1_MASK();
		printk("\n========pcie_probe====0x1fbe340c=%x===========",tmp_reg);
		val = val & 0x1f;
		SET_PBUS_PCIE1_MASK(0xffffffff << val);
		printk("\n========pcie_probe====after pbus setting=======");
		tmp_reg = GET_PBUS_PCIE1_BASE();
		printk("\n========pcie_probe====0x1fbe3408=%x===========",tmp_reg);
		tmp_reg = GET_PBUS_PCIE1_MASK();
		printk("\n========pcie_probe====0x1fbe340c=%x===========",tmp_reg);
		//for debug detect linkdown
		val = regRead_PCIe(0x1fa92000 + PCIE_RSTCR);	
		printk("\n========pcie_probe====0x1fa92510=%x===========",val);
		regWrite_PCIe(0x1fa92000 + PCIE_RSTCR,val & (~(7<<13)));
	}


	if (err)
		goto release_resource;

	pcie_api_init();

	/*------------create kthread----------------------------------------------------------*/
	if(0==base_num)
	{
    	printk("\nkthread_run(pcie_int13_thread_act \n");
    	pcie_int13_dbg_thread = kthread_run(pcie_int13_thread_act, NULL, "pcie_int13_dbg");
	}
	/*------------------------------------------------------------------------------------*/	

	return 0;

release_resource:
	pci_free_resource_list(&host->windows);

	return err;
}

static const struct of_device_id mtk_pcie_of_match[] = {
	{ .compatible = "ecnt,pcie-ecnt" },
	{},
};

static struct platform_driver mtk_pcie_driver = {
	.probe = mtk_pcie_probe,
	.driver = {
		.name = "mtk-pcie",
		.of_match_table = mtk_pcie_of_match,
	},
};

builtin_platform_driver(mtk_pcie_driver);
