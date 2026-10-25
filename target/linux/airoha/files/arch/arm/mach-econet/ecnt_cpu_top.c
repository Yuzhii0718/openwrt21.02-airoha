#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/dma-direct.h>
#endif
#define ECNT_CPU_TOP_AXI2BUS				0x0
#define ECNT_CPU_TOP_RDBYPASS_CFG			0x4
#define RDBYPASS_ENABLE						0x1
#define RDBYPASS_DISABLE					0x0
#define RDBYPASS_ENABLE_MASK				0x1
#define RDBYPASS_CMD_FIFO_SHIFT				0x1
#define RDBYPASS_CMD_FIFO_MASK				0xF
#define RDBYPASS_CMD_FIFO(x)				((x & RDBYPASS_CMD_FIFO_MASK) << RDBYPASS_CMD_FIFO_SHIFT)
#define RDBYPASS_RCMD_FIFO_SHIFT			0x5
#define RDBYPASS_RCMD_FIFO_MASK				0x7
#define RDBYPASS_RCMD_FIFO(x)				((x & RDBYPASS_RCMD_FIFO_MASK) << RDBYPASS_RCMD_FIFO_SHIFT)
 
#define ECNT_CPU_TOP_RDBYPASS_MASK			0x8
#define ECNT_CPU_TOP_RDBYPASS_CNT			0xC
#define CR_RBUS_PENDING_CNT                 (0x10)
#define RBUS_PENDING_ADDR_SHIFT             (24)
#define RBUS_PENDING_ADDR_BITS              (0xff)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#define dma_zalloc_coherent(dev, size, dma_handle, flag) dma_alloc_coherent(dev, size, dma_handle, flag | __GFP_ZERO)
#endif
struct ecnt_cpu_top
{
	struct device	*dev;
	void __iomem	*base;
};
struct ecnt_cpu_top *cpu_top = NULL;
static u32 ecnt_cpu_top_read(u32 reg)
{
    return readl(cpu_top->base + reg);
}
static void ecnt_cpu_top_write(u32 reg, u32 val)
{
    writel(val, cpu_top->base + reg); 
}
static void ecnt_cpu_top_rmw(u32 reg, u32 mask, u32 set)
{
	u32 val = 0;
	val = ecnt_cpu_top_read(reg);
	val &= ~mask;
	val |= set;
	ecnt_cpu_top_write(reg, val);
}
void ecnt_set_cmd_fifo(u8 val)
{
    ecnt_cpu_top_rmw(ECNT_CPU_TOP_RDBYPASS_CFG, RDBYPASS_CMD_FIFO(RDBYPASS_CMD_FIFO_MASK), RDBYPASS_CMD_FIFO(val));
}
EXPORT_SYMBOL(ecnt_set_cmd_fifo);
void ecnt_set_rcmd_fifo(u8 val)
{
    ecnt_cpu_top_rmw(ECNT_CPU_TOP_RDBYPASS_CFG, RDBYPASS_RCMD_FIFO(RDBYPASS_RCMD_FIFO_MASK), RDBYPASS_RCMD_FIFO(val));
}
EXPORT_SYMBOL(ecnt_set_rcmd_fifo);
void ecnt_set_rdbypass_mask(u32 mask)
{
    ecnt_cpu_top_write(ECNT_CPU_TOP_RDBYPASS_MASK, mask);
}
EXPORT_SYMBOL(ecnt_set_rdbypass_mask);
void ecnt_disable_rdbypass(void)
{
    ecnt_cpu_top_rmw(ECNT_CPU_TOP_RDBYPASS_CFG, RDBYPASS_ENABLE_MASK, RDBYPASS_DISABLE);
}
EXPORT_SYMBOL(ecnt_disable_rdbypass);
void ecnt_enable_rdbypass(void)
{
    ecnt_cpu_top_rmw(ECNT_CPU_TOP_RDBYPASS_CFG, RDBYPASS_ENABLE_MASK, RDBYPASS_ENABLE);
}
EXPORT_SYMBOL(ecnt_enable_rdbypass);
u32 ecnt_get_rdbypass_cfg(void)
{
    return ecnt_cpu_top_read(ECNT_CPU_TOP_RDBYPASS_CFG);
}
EXPORT_SYMBOL(ecnt_get_rdbypass_cfg);
u32 ecnt_get_rdbypass_mask(void)
{
    return ecnt_cpu_top_read(ECNT_CPU_TOP_RDBYPASS_MASK);
}
EXPORT_SYMBOL(ecnt_get_rdbypass_mask);
u32 ecnt_get_rdbypass_cnt(void)
{
    return ecnt_cpu_top_read(ECNT_CPU_TOP_RDBYPASS_CNT);
}
EXPORT_SYMBOL(ecnt_get_rdbypass_cnt);
void ecnt_enable_rd_bypass_wt(int en, u32 mask, u8 cmd_fifo, u8 rcmd_fifo)
{
    if (en) {
        ecnt_set_rdbypass_mask(mask);
        ecnt_set_cmd_fifo(cmd_fifo);
        ecnt_set_rcmd_fifo(rcmd_fifo);
        ecnt_enable_rdbypass();
    }
    else { /* disable */
        ecnt_disable_rdbypass();
    }
    return;
}
EXPORT_SYMBOL(ecnt_enable_rd_bypass_wt);
void enable_bufferable(int enable)
{
    u32 val;
    u32 reg = ECNT_CPU_TOP_AXI2BUS;
    
    val = ecnt_cpu_top_read(reg);
    
    if (enable)
        val |= (1);
    else /* disable */
        val &= (~(1)); 
    ecnt_cpu_top_write(reg, val);
    return;
}
EXPORT_SYMBOL(enable_bufferable);
void enable_rbus_pending(int enable)
{
    u32 val;
    u32 reg = ECNT_CPU_TOP_AXI2BUS;
    
    val = ecnt_cpu_top_read(reg);
    
    if (enable)
        val |= (1<<1);
    else /* disable */
        val &= (~(1<<1)); 
    ecnt_cpu_top_write(reg, val);
    return;
}
EXPORT_SYMBOL(enable_rbus_pending);
int is_rbus_pending_enabled(void)
{
    return ((ecnt_cpu_top_read(ECNT_CPU_TOP_AXI2BUS)>>1)&0x1);
}
EXPORT_SYMBOL(is_rbus_pending_enabled);
static u32 get_rbus_pending_addr(void)
{
    return (((ecnt_cpu_top_read(ECNT_CPU_TOP_AXI2BUS)>>2)&RBUS_PENDING_ADDR_BITS)<<RBUS_PENDING_ADDR_SHIFT);
}
void set_rbus_pending_addr(u32 addr)
{
    u32 val, val2;
    u32 reg = ECNT_CPU_TOP_AXI2BUS;
    
    val = ecnt_cpu_top_read(reg);
    val2 = val;
    /* set addr's highest 8 bits to ECNT_CPU_TOP_AXI2BUS reg's bit2~10 */
    val &= (~(RBUS_PENDING_ADDR_BITS<<2));
    val |= (((addr>>RBUS_PENDING_ADDR_SHIFT)&RBUS_PENDING_ADDR_BITS)<<2);
    ecnt_cpu_top_write(reg, val);
    
    printk("old rbus_pending_addr:0x%lx  new rbus_pending_addr:0x%lx\n",
                ((val2>>2)&RBUS_PENDING_ADDR_BITS)<<RBUS_PENDING_ADDR_SHIFT, get_rbus_pending_addr());
    return;
}
EXPORT_SYMBOL(set_rbus_pending_addr);
static u32 get_rbus_pending_cnt(void)
{
    return ecnt_cpu_top_read(CR_RBUS_PENDING_CNT);
}
void set_rbus_pending_cnt(u32 cnt_val)
{
    u32 val;
    u32 reg = CR_RBUS_PENDING_CNT;
    
    val = ecnt_cpu_top_read(reg);
    
    ecnt_cpu_top_write(reg, cnt_val);
    
    //printk("old rbus_pending_cnt:0x%lx  new rbus_pending_cnt:0x%lx\n", val, get_rbus_pending_cnt());
    return;
}
EXPORT_SYMBOL(set_rbus_pending_cnt);
static int ecnt_cpu_top_probe(struct platform_device *pdev)
{
	int err = 0;
	struct resource *res = NULL;
	unsigned long phys_addr = 0;
	dma_addr_t dma_addr = 0;
	void *test_uc = NULL, *test_c = NULL;
	cpu_top = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_cpu_top), GFP_KERNEL);
	if (!cpu_top)
		return -ENOMEM;
	platform_set_drvdata(pdev, cpu_top);
	/* get RBUS base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	cpu_top->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(cpu_top->base))
	{
		err = PTR_ERR(cpu_top->base);
		devm_kfree(&(pdev->dev), cpu_top);
		return err;
	}
	cpu_top->dev = &pdev->dev;
	platform_set_drvdata(pdev, cpu_top);

	test_uc = dma_zalloc_coherent(cpu_top->dev, 64, &(dma_addr), GFP_NOWAIT);
	phys_addr = dma_to_phys(cpu_top->dev, dma_addr);
	test_c = phys_to_virt(phys_addr);

	printk("test_uc:%p test_c:%p cpu_top->base:%p\n", test_uc, test_c, cpu_top->base);
	return err;
}
static int ecnt_cpu_top_remove(struct platform_device *pdev)
{
	devm_iounmap(&(pdev->dev), cpu_top->base);
	devm_kfree(&(pdev->dev), cpu_top);
	return 0;
}
static const struct of_device_id ecnt_cpu_top_of_id[] = {
	{ .compatible = "econet,ecnt-cpu_top"},
	{ /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_cpu_top_of_id);

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_cpu_top_driver = {
	.probe = ecnt_cpu_top_probe,
	.remove = ecnt_cpu_top_remove,
	.driver = {
		.name = "ecnt-cpu_top",
		.of_match_table = ecnt_cpu_top_of_id
	},
};
module_platform_driver(ecnt_cpu_top_driver);
MODULE_ALIAS("platform:ecnt-cpu_top");
MODULE_DESCRIPTION("ECONET CPU_TOP Driver");
MODULE_LICENSE("GPL v2");
