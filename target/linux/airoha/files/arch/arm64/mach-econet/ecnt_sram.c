/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include <asm/io.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef TCSUPPORT_CPU_ARMV8
#define GDMP_SRAM_BASE              (0x00000000)
#else
#define GDMP_SRAM_BASE				(0xBFA40000)
#endif

#define GDMP_SRAM_MASK              (0x7fff)

#define S_128K              (0x20000)
#define S_256K              (0x40000)

#define I2C_SLAVE_SRAM_SIZE		(0x100)
#define rdMeml(addr)			*(volatile unsigned long*)(addr)
#define wtMeml(addr,val)        *(volatile unsigned long*)(addr)=(unsigned long)(val)
#define rdMemB(addr)			*(volatile unsigned char*)(addr)
#define wtMemB(addr,val)        *(volatile unsigned char*)(addr)=(unsigned char)(val)

//#define SRAM_DEBUG_ON   1

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_sram {
	struct device *gdmp_dev;
	void __iomem *gdmp_base;
    void __iomem *l2c_i_base;
    void __iomem *l2c_e_base;
	void __iomem *l2c_rbus_base;
	void __iomem *i2c_slave_base;
};

/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_sram *ecnt_sram = NULL;


static const struct of_device_id ecnt_sram_of_id[] = {
    { .compatible = "econet,ecnt-sram"},
    { /* sentinel */}
};
MODULE_DEVICE_TABLE(of, ecnt_sram_of_id);

unsigned char testBytePat[] = {0x5a, 0xa5, 0xff, 0x00};
unsigned long testWordPat[] = {0x5a5a5a5a, 0xa5a5a5a5, 0xff00ff00, 0x00ff00ff};

unsigned long gdump_sram_size = 0;

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
#if !defined(TCSUPPORT_CPU_ARMV8_64)
/* ecnt_dma_xxx_range funcs are in linux-ecnt/arch/arm/mm/cache-v7.S */
extern void ecnt_dma_flush_range(unsigned long start, unsigned long end);
extern void ecnt_dma_inv_range(unsigned long start, unsigned long end);
EXPORT_SYMBOL(ecnt_dma_flush_range);
EXPORT_SYMBOL(ecnt_dma_inv_range);
#endif
extern unsigned int get_l2c_sram_size(void);

#ifdef CONFIG_RCU_STALL_COMMON
extern int rcu_cpu_stall_suppress __read_mostly;
#endif

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
u32 get_gdmpSram_data(u32 reg, u32 *ret_val)
{
	if (reg > gdump_sram_size)
	{
		printk("ERROR: Trying to access %x is over GDUMP SRAM ! \r\n",reg);
		printk("Note: the gdump sram base addr should be start at 0x0 !\r\n");
		return -1;
	}
	else
	{
		*ret_val = readl(ecnt_sram->gdmp_base + reg);
    	return 0;
	}
}
EXPORT_SYMBOL(get_gdmpSram_data);
/* don't EXPORT this function. Create API for your purpose instead. */
void set_gdmpSram_data(u32 reg, u32 val)
{
	if (reg > gdump_sram_size)
	{
		printk("ERROR: Trying to access %x is over GDUMP SRAM !\r\n",reg);
		printk("Note: the gdump sram base addr should be start at 0x0 !\r\n");
	}
	else
	{
    	writel(val, ecnt_sram->gdmp_base + reg); 
	}
}

struct device* get_gdmpSram_dev(void)
{
    if ((ecnt_sram) && (ecnt_sram->gdmp_dev))
        return ecnt_sram->gdmp_dev;
    else
        return NULL;
}
EXPORT_SYMBOL(get_gdmpSram_dev);

void __iomem * get_gdmpSram_base(void)
{
    return ecnt_sram->gdmp_base;
}
EXPORT_SYMBOL(get_gdmpSram_base);

void __iomem * get_l2cSramPbus_base(void)
{
    return ecnt_sram->l2c_e_base;
}
EXPORT_SYMBOL(get_l2cSramPbus_base);

int ecnt_rw_ram_test (unsigned long startAddr, unsigned int size, unsigned long patAdd)
{
    int i, j;
    int cpuId = smp_processor_id();
    unsigned long tmpAddr, tmpWord;
    unsigned char tmpByte;
    unsigned char patAddB = (unsigned char)(patAdd&0xff);
    unsigned int wordLen = sizeof(unsigned long);
    unsigned int testWordNum = size/wordLen;
    unsigned int wordPatNum = sizeof(testWordPat)/sizeof(testWordPat[0]);
    unsigned int byteLen = 1;
    unsigned int testByteNum = size;
    unsigned int bytePatNum = sizeof(testBytePat)/sizeof(testBytePat[0]);

        
    for (j=0; j<wordPatNum; j++) {

        tmpAddr = startAddr;
        tmpWord=testWordPat[j]+patAdd;

        /* CPU writes words to RAM */
        for (i=0; i<testWordNum; i++) {
            wtMeml(tmpAddr,tmpWord);
            tmpAddr += wordLen;
        }


        tmpAddr = startAddr;
        tmpWord=testWordPat[j]+patAdd;

        /* CPU reads words from RAM and compare */
        for (i=0; i<testWordNum; i++) {
            if (rdMeml(tmpAddr) != tmpWord) {
                printk("\n[H%d]ERROR1 rdMeml(0x%lx):0x%lx != tmpWord:0x%lx at word:%d\n", 
                                    cpuId, tmpAddr, rdMeml(tmpAddr), tmpWord, i);
                return -1;
            }
            tmpAddr += wordLen;
        }

        tmpAddr = startAddr;
        tmpWord=testWordPat[j]+patAdd+1;

        /* CPU writes a word then reads it back and compare */
        for (i=0; i<testWordNum; i++) {
            wtMeml(tmpAddr,tmpWord);
            if (rdMeml(tmpAddr) != tmpWord) {
                printk("\n[H%d]ERROR2 rdMeml(0x%lx):0x%lx != tmpWord:0x%lx at word:%d\n", 
                                    cpuId, tmpAddr, rdMeml(tmpAddr), tmpWord, i);
                return -1;
            }
            tmpAddr += wordLen;
        }
    }


    for (j=0; j<bytePatNum; j++) {
    
        tmpAddr = startAddr;
        tmpByte=testBytePat[j]+patAddB;

        /* CPU writes bytes to RAM */
        for (i=0; i<testByteNum; i++) {
            wtMemB(tmpAddr,tmpByte);
            tmpAddr += byteLen;
        }
        
        tmpAddr = startAddr;
        tmpByte=testBytePat[j]+patAddB;

        /* CPU reads bytes from RAM and compare */
        for (i=0; i<testByteNum; i++) {
            if (rdMemB(tmpAddr) != tmpByte) {
                printk("\n[H%d]ERROR3 rdMemB(0x%lx):0x%x != tmpByte:0x%x at byte:%d\n", 
                                    cpuId, tmpAddr, rdMemB(tmpAddr), tmpByte, i);
                return -1;
            }
            tmpAddr += byteLen;
        }

        tmpAddr = startAddr;
        tmpByte=testBytePat[j]+patAddB+1;

        /* CPU writes a byte then reads it back and compare */
        for (i=0; i<testByteNum; i++) {
            wtMemB(tmpAddr,tmpByte);
            if (rdMemB(tmpAddr) != tmpByte) {
                printk("\n[H%d]ERROR4 rdMemB(0x%lx):0x%x != tmpByte:0x%x at byte:%d\n", 
                                    cpuId, tmpAddr, rdMemB(tmpAddr), tmpByte, i);
                return -1;
            }
            tmpAddr += byteLen;
        }
    }

    return 0;
}
EXPORT_SYMBOL(ecnt_rw_ram_test);

int RW_GDMP_SRAM_TEST(unsigned long start, unsigned int size, unsigned long patAdd)
{
    return ecnt_rw_ram_test(((unsigned long)ecnt_sram->gdmp_base)+start, size, patAdd);
}
EXPORT_SYMBOL(RW_GDMP_SRAM_TEST);

void set_i2c_slave_sram_byte( u32 addr,unsigned char data)
{
	wtMemB((ecnt_sram->i2c_slave_base + addr), data);

}
EXPORT_SYMBOL(set_i2c_slave_sram_byte);

unsigned char get_i2c_slave_sram_byte(u32 addr)
{
	return rdMemB((ecnt_sram->i2c_slave_base + addr));

}
EXPORT_SYMBOL(get_i2c_slave_sram_byte);


int i2c_slave_sram_test(void)
{
	return ecnt_rw_ram_test((unsigned long)ecnt_sram->i2c_slave_base, I2C_SLAVE_SRAM_SIZE*2,0);
}
	EXPORT_SYMBOL(i2c_slave_sram_test);

void SET_RCU_CPU_STALL_SUPRESS(void)
{
    rcu_cpu_stall_suppress=1;
    return;
}
EXPORT_SYMBOL(SET_RCU_CPU_STALL_SUPRESS);

int l2c_sram_test(unsigned long offset, unsigned int t_size, unsigned long patAdd)
{    
    int j;
    unsigned long startAddr;



    for (j=0; j<3; j++) {

        if (j==0)
            startAddr = (unsigned long)ecnt_sram->l2c_i_base;
        else if(j==1)
            startAddr = (unsigned long)ecnt_sram->l2c_e_base;
		else /* j==2 */
			startAddr = (unsigned long)ecnt_sram->l2c_rbus_base;
        
        startAddr += offset;

        //printk("l2c_sram_test base:0x%lx, size:0x%x, j==%d\n", startAddr, t_size, j);

        if (ecnt_rw_ram_test(startAddr, t_size, patAdd))
            return -1;
    }

    return 0;
}
EXPORT_SYMBOL(l2c_sram_test);
int l2c_sram_read_test (unsigned int l2cSram_off, unsigned int size, unsigned long patAdd)
{
    int i;
    unsigned long tmpAddr, tmpWord;

    unsigned int wordLen = sizeof(unsigned long);
    unsigned int testWordNum = size/wordLen;
	unsigned long data;
	unsigned long l2cSram_pbus_virt_addr = (unsigned long)ecnt_sram->l2c_e_base + l2cSram_off;

	if (get_l2c_sram_size() < (l2cSram_off + size)){
		printk("\nERROR: Test target is out of L2C SRAM size!\n");
		return -1;
	}

    /* CPU reads words from RAM and compare */
	tmpAddr = l2cSram_pbus_virt_addr;
	tmpWord=patAdd;
	for (i=0; i<testWordNum; i++) {
		data = rdMeml(tmpAddr);
		if (data != (tmpWord+i)) {
			printk("\nERROR: data:0x%lx != tmpWord:0x%lx at word:%d\n", 
               	 data, tmpWord, i);
			return -1;
		}
		tmpAddr += wordLen;
	}

    return 0;
}
EXPORT_SYMBOL(l2c_sram_read_test);

int l2c_sram_write_test (unsigned int l2cSram_off, unsigned int size, unsigned long patAdd)
{
    int i;
    unsigned long tmpAddr, tmpWord;
    unsigned int wordLen = sizeof(unsigned long);
    unsigned int testWordNum = size/wordLen;
	unsigned long l2cSram_pbus_virt_addr = (unsigned long)ecnt_sram->l2c_e_base + l2cSram_off;

	if (get_l2c_sram_size() < (l2cSram_off + size)){
		printk("\nERROR: Test target is out of L2C SRAM size!\n");
		return -1;
	}
        
    /* CPU writes words to RAM */
    tmpAddr = l2cSram_pbus_virt_addr;
    tmpWord=patAdd;
    for (i=0; i<testWordNum; i++) {
	    wtMeml(tmpAddr, (tmpWord+i));
		tmpAddr += wordLen;
	}

    return 0;
}
EXPORT_SYMBOL(l2c_sram_write_test);


static int ecnt_sram_drv_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;

    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No sram DT node found");
        return -EINVAL;
    }

    ecnt_sram = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_sram), GFP_KERNEL);
    if (!ecnt_sram)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_sram);

    /* get GDMP SRAM base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    ecnt_sram->gdmp_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_sram->gdmp_base))
        return PTR_ERR(ecnt_sram->gdmp_base);

	/* get GDMP SRAM size*/
	gdump_sram_size = resource_size(res);

    ecnt_sram->gdmp_dev = &pdev->dev;
    
    /* get L2C SRAM base address (for CPU internal access) */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    ecnt_sram->l2c_i_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_sram->l2c_i_base))
        return PTR_ERR(ecnt_sram->l2c_i_base);

    #ifdef SRAM_DEBUG_ON
	printk("[sram] (L2C_internal) res->start:0x%llx  res->end:0x%llx\n", res->start, res->end);
    #endif

    /* get L2C SRAM base address (for CPU/NPU external access) */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
    #ifdef TCSUPPORT_CPU_EN7581
    res->start -= 0xC0000;
    res->end -= 0xC0000;
    #endif
    ecnt_sram->l2c_e_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_sram->l2c_e_base))
        return PTR_ERR(ecnt_sram->l2c_e_base);

    #ifdef SRAM_DEBUG_ON
	printk("[sram] (L2C_pbus) res->start:0x%llx  res->end:0x%llx\n", res->start, res->end);
    #endif

	/* get L2C SRAM base address (for CPU/NPU external access by npu_rbus) */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
	ecnt_sram->l2c_rbus_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ecnt_sram->l2c_rbus_base))
		return PTR_ERR(ecnt_sram->l2c_rbus_base);

    #ifdef SRAM_DEBUG_ON
	printk("[sram] (L2C_npuRbus) res->start:0x%llx  res->end:0x%llx\n", res->start, res->end);
    #endif

	/* get I2C slave SRAM base address */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 4);
	ecnt_sram->i2c_slave_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ecnt_sram->i2c_slave_base))
		return PTR_ERR(ecnt_sram->i2c_slave_base);
    
#ifdef SRAM_DEBUG_ON
	printk("[sram] res->name:%s\n", res->name);
	printk("[sram] res->start:0x%llx ===\n", res->start);
	printk("[sram] res->end:0x%llx ===\n", res->end);
	printk("[sram] ecnt_sram->gdmp_base:0x%lx\n", (unsigned long)ecnt_sram->gdmp_base);
    printk("[sram] ecnt_sram->l2c_i_base:0x%lx\n", (unsigned long)ecnt_sram->l2c_i_base);
    printk("[sram] ecnt_sram->l2c_e_base:0x%lx\n", (unsigned long)ecnt_sram->l2c_e_base);
#endif

    return 0;
}

static int ecnt_sram_drv_remove(struct platform_device *pdev)
{
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/
static struct platform_driver ecnt_sram_driver = {
    .probe = ecnt_sram_drv_probe,
    .remove = ecnt_sram_drv_remove,
    .driver = {
        .name = "ecnt-sram",
        .of_match_table = ecnt_sram_of_id
    },
};
module_platform_driver(ecnt_sram_driver);


MODULE_DESCRIPTION("EcoNet SRAM Driver");

