/*
 *  Interrupt service routines for Trendchip board
 */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/tc3162/tc3162.h>
#include <linux/sched.h>
#include <asm/setup.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>

#ifdef CONFIG_MIPS_TC3262
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/kernel_stat.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include <asm/mipsmtregs.h>
#else
#include <linux/io.h>
#include <asm/irq_cpu.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/linkage.h>
#include <linux/irqdomain.h>
#endif


#define PHY_TO_K1(x)    (((unsigned int)x) | 0xa0000000)

#if defined (CONFIG_IRQ_GIC)
#include <asm/gic.h>
#include <asm/mips-cm.h>
#include <linux/proc_fs.h>
#include <ecnt_hook/ecnt_hook_irq_num.h>
#include <ecnt_hook/ecnt_hook_cpu_interrupt_type.h>
#include <linux/mtd/rt_flash.h>
#include "spi/spi_controller.h"

extern unsigned int gic_present;

#ifdef TCSUPPORT_MIPS_1004K
#define irq_num_hook_name "irq_num_hook"
#define CPU_INTERRUPT_MAJOR 224

#define TIMER0_INTSRC   30
#define TIMER1_INTSRC   29
#define TIMER2_INTSRC   37
#define TIMER3_INTSRC   36

#define IPI_CALL0_INTSRC 34
#define IPI_CALL1_INTSRC 61
#define IPI_CALL2_INTSRC 62
#define IPI_CALL3_INTSRC 63

#define IPI_RESCHED0_INTSRC 7
#define IPI_RESCHED1_INTSRC 8
#define IPI_RESCHED2_INTSRC 12
#define IPI_RESCHED3_INTSRC 13

#define IRQ_NUM_MAX_VALUE   63

typedef int (*cpu_interrupt_api_op_t)(struct ecnt_cpu_interrupt_data * data);

unsigned int gicVecPlus1_to_intSrc_arr[IRQ_NUM_MAX_VALUE+1];
ecnt_ret_val ecnt_irq_num_hook(struct ecnt_data *in_data);
int cpu_interrupt_api_get_irqnum(struct ecnt_cpu_interrupt_data *data);
int cpu_interrupt_api_show_interrupts(struct ecnt_cpu_interrupt_data *data);
int cpu_interrupt_api_check_intrName(struct ecnt_cpu_interrupt_data *data);
static long cpu_interrupt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

struct file_operations cpu_interrupt_fops = {
	.owner 			= THIS_MODULE,
	.unlocked_ioctl	= cpu_interrupt_ioctl,
};

struct ecnt_hook_ops ecnt_irq_num_op = {
    .name = irq_num_hook_name,
    .hookfn = ecnt_irq_num_hook,
    .maintype = ECNT_IRQ_NUM,
    .is_execute = 1,
    .subtype = ECNT_DRIVER_API,
    .priority = 1
};

/* Warning: same sequence with enum 'IRQ_NUM_HookFunction_t' in ecnt_hook_irq_num.h */
unsigned char* irq_name_arr[] = {
    IRQ_NAME_DMT,
    IRQ_NAME_PCM1,
    IRQ_NAME_PCM2,
};

static cpu_interrupt_api_op_t cpu_interrupt_operation[] = {
	cpu_interrupt_api_get_irqnum,
    cpu_interrupt_api_show_interrupts,
    cpu_interrupt_api_check_intrName,
};

int timers_intSrcNum[NR_CPUS] = {TIMER0_INTSRC, TIMER1_INTSRC, TIMER2_INTSRC, TIMER3_INTSRC};
int ipi_call_intSrcNum[NR_CPUS] = {IPI_CALL0_INTSRC, IPI_CALL1_INTSRC, IPI_CALL2_INTSRC, IPI_CALL3_INTSRC};
int ipi_resched_intSrcNum[NR_CPUS] = {IPI_RESCHED0_INTSRC, IPI_RESCHED1_INTSRC, IPI_RESCHED2_INTSRC, IPI_RESCHED3_INTSRC};
extern SPI_CONTROLLER_RTN_T SPI_CONTROLLER_Enable_Auto_Mode( void );
extern void disable_all_interrupts(void);
#endif

#define X GIC_UNUSED

/* When vector is changed, the "smp_affinity" settings in rcS may need to be changed */
static struct gic_intr_map gic_intr_map[GIC_NUM_INTRS] = 
{/*  cpu,   irqNum-1,           polarity,       triggerType,	flags,          name,               Src     fullname        */
    {cpu0,  CPU_CM_ERR-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, NULL },           /*    0     CPU Coherence Manager Error */
    {cpu0,  CPU_CM_PCINT-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, NULL },           /*    1     CPU CM Perf Cnt overflow */
    {cpu0,  UART_INT-1,         GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_UART },  /*    2     uart  */
    {X,     DRAM_PROTECTION-1,  GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_DRAM_PROTECT},/* 3   dram illegal access */
    {cpu0,  TIMER0_INT-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_TIMER0}, /*    4     timer 0 */
    {cpu0,  TIMER1_INT-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_TIMER1 },/*    5     timer 1 */
    {cpu0,  TIMER2_INT-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_TIMER2 },/*    6     timer 2 */
    {cpu0,  IPI_RESCHED_INT0-1, GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    7     ipi resched 0 */
    {cpu1,  IPI_RESCHED_INT1-1, GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    8     ipi resched 1 */
    {cpu0,  TIMER5_INT-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WATCHDOG },/*  9     timer 3 for wdog */
    {cpu0,  GPIO_INT-1,         GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_GPIO },  /*    10     GPIO */
    {cpu0,  PCM1_INT-1,         GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_PCM1 },  /*    11     PCM 1 */
    {cpu2,  IPI_RESCHED_INT2-1, GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    12     ipi resched 2 */
    {cpu3,  IPI_RESCHED_INT3-1, GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    13     ipi resched 3 */
    {cpu0,  GDMA_INTR-1,        GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_GDMA },  /*    14     GDMA */
    {cpu0,  MAC1_INT-1,         GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_GIGA_SWITCH},/*15    LAN Giga Switch */
    {cpu0,  UART2_INT-1,        GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_UART2 }, /*    16     uart 2 */
    {cpu0,  IRQ_RT3XXX_USB-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_USB },   /*    17     USB host */
    {cpu0,  DYINGGASP_INT-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_DYING_GASP},/* 18    Dying gasp */
    {cpu0,  DMT_INT-1,          GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_DMT },   /*    19     xDSL DMT */
    {cpu0,  GIC_EDGE_NMI-1,     GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    20     gic edge NMI */
    {cpu0,  QDMA_LAN0_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_LAN0},/*  21     QDMA LAN 0 */
    {cpu0,  QDMA_WAN0_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_WAN0},/*  22     QDMA WAN 0 */
    {cpu0,  PCIE_0_INT-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_PCIE0 }, /*    23     PCIE port 0 */
    {cpu0,  PCIE_A_INT-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_PCIE1 }, /*    24     PCIE port 1 */
    {cpu0,  PCIE_SERR_INT-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_PCIE_ERR},/*   25     PCIE error */
    {cpu0,  XPON_MAC_INTR-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_XPON_MAC },/*  26     XPON MAC */
    {cpu0,  XPON_PHY_INTR-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_XPON_PHY },/*  27     XPON PHY */
    {cpu0,  CRYPTO_INT-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_CRYPTO },/*    28     Crypto engine */
    {cpu1,  SI_TIMER_INT-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, NULL },           /*    29     external CPU timer 1 when bfbf0400[1]=1*/
    {cpu0,  SI_TIMER_INT-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, NULL },           /*    30     external CPU timer 0 when bfbf0400[0]=1*/
    {cpu0,  BUS_TOUT_INT-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_PBUS_TIMEOUT},/* 31   Pbus timeout */
    {cpu0,  PCM2_INT-1,         GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_PCM2 },  /*    32     PCM 2 */
    {cpu0,  FE_ERR_INTR-1,      GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_FE_ERR },/*    33     Frame Engine Error */
    {cpu0,  IPI_CALL_INT0-1,    GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    34     ipi call 0 */
    {cpu0,  AUTO_MANUAL_INT-1,  GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_SPI },   /*    35     SPI */
    {cpu3,  SI_TIMER_INT-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, NULL },           /*    36     external CPU timer 3 when bfbe0000[1]=1*/
    {cpu2,  SI_TIMER_INT-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, NULL },           /*    37     external CPU timer 2 when bfbe0000[1]=1*/    
    {cpu0,  UART3_INT-1,        GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_UART3 }, /*    38     UART3 */
    {cpu0,  QDMA_LAN1_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_LAN1},/*  39     QDMA LAN 1 */
    {cpu0,  QDMA_LAN2_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_LAN2 },/* 40     QDMA LAN 2 */
    {cpu0,  QDMA_LAN3_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_LAN3 },/* 41     QDMA LAN 3 */
    {cpu0,  QDMA_WAN1_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_WAN1},/*  42     QDMA WAN 1 */
    {cpu0,  QDMA_WAN2_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_WAN2},/*  43     QDMA WAN 2 */
    {cpu0,  QDMA_WAN3_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_QDMA_WAN3},/*  44     QDMA WAN 3 */
    {cpu0,  UART4_INT-1,        GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_UART4 }, /*    45     UART 4 */
    {cpu0,  UART5_INT-1,        GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_UART5 }, /*    46     UART 5 */
    {cpu0,  HSDMA_INTR-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_HSDMA},  /*    47     High Speed DMA */
    {cpu0,  USB_HOST_2-1,       GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_USB2 },  /*    48     USB host 2 (port1) */
    {cpu0,  XSI_MAC_INTR-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_XSI_MAC},/*    49     XFI/HGSMII MAC interface */
    {cpu0,  XSI_PHY_INTR-1,     GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_XSI_PHY},/*    50     XFI/HGSMII PHY interface */
    {cpu0,  WOE0_INTR-1,        GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WOE0},   /*    51     WIFI Offload Engine 0 */
    {cpu0,  WOE1_INTR-1,        GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WOE1},   /*    52     WIFI Offload Engine 1 */
    {cpu0,  WDMA0_P0_INTR-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WDMA0_P0},/*   53     WIFI DMA 0 port 0 */
    {cpu0,  WDMA0_P1_INTR-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WDMA0_P1},/*   54     WIFI DMA 0 port 1 */
    {cpu0,  WDMA0_WOE_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WDMA0_WOE},/*  55     WIFI DMA 0 for WOE */
    {cpu0,  WDMA1_P0_INTR-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WDMA1_P0},/*   56     WIFI DMA 1 port 0 */
    {cpu0,  WDMA1_P1_INTR-1,    GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WDMA1_P1},/*   56     WIFI DMA 1 port 1 */
    {cpu0,  WDMA1_WOE_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_WDMA1_WOE},/*  58     WIFI DMA 1 for WOE */
    #ifdef TCSUPPORT_CPU_EN7528
    {cpu2,  RBUS_TOUT_INTR-1,   GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_RBUS_TOUT}, /* 59     rbus timeout interrupt */
    #else
    {cpu0,  EFUSE_ERR0_INTR-1,  GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_EFUSE_ERR0},/* 59     efuse error for not setting key */
    #endif
    {cpu0,  EFUSE_ERR1_INTR-1,  GIC_POL_POS,    GIC_TRIG_LEVEL, GIC_FLAG_IPI, IRQ_NAME_EFUSE_ERR1},/* 60     efuse error for prev action not finished */
    {cpu1,  IPI_CALL_INT1-1,    GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    61     ipi call 1 */
    {cpu2,  IPI_CALL_INT2-1,    GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    62     ipi call 2 */
    {cpu3,  IPI_CALL_INT3-1,    GIC_POL_POS,    GIC_TRIG_EDGE,  GIC_FLAG_IPI, NULL },           /*    63     ipi call 3 */

};
#undef X

#ifdef TCSUPPORT_MIPS_1004K
static void flash_auto_mode_set(void) 
{
    /* disable all interrupts */
    disable_all_interrupts();

    /* read flash's 1st page */
    READ_FLASH_BYTE(flash_base);
    
    /* Switch to auto mode*/
    SPI_CONTROLLER_Enable_Auto_Mode();
    return;
}

void interrupt_nmi_set (unsigned int intSrc)
{
    unsigned int cpu_mask, cpu_num;

    if (intSrc>=GIC_NUM_INTRS) {
        printk("\nError(%s): wrong intSrc%d\n", __func__, intSrc);
        return;
    }

    /* get intSrc's CPU affninty */
    cpu_mask = GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_VPE_REG_OFF(intSrc, 0));

    /* Core0's exception base is 0xbfc00000 which is flash's start addr.
     * Under NAND flash, if CPU accesses flash in manual mode, system will hang.
     * So NMI can't be sent to CPU0 and CPU1 in Core0 */
    if (cpu_mask&0x3) { /* cpu 0 or 1*/
        GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_VPE_REG_OFF(intSrc, 0)) = 0x4; /* cpu2 */
        cpu_num = (cpu_mask&0x1) ? 0 : 1;
        printk("\nre-bind intSrc%d from CPU%d to CPU2 due to NMI unable to be sent to Core0\n", intSrc, cpu_num);
    }

    /* set intSrc as NMI */
    GICWRITE(GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(intSrc)), GIC_MAP_TO_NMI_MSK);

    return;
}
EXPORT_SYMBOL(interrupt_nmi_set);

void gic_edge_nmi_send (int cpu) 
{
    unsigned int intSrc;

    
    if (cpu<0 || 3<cpu) {
        printk("\nError(%s): wrong CPU%d\n", __func__, cpu);
        return;
    }

    /* NMI is sent via intSrc:20 by GIC EDGE_WRITE register  */
    intSrc = gicVecPlus1_to_intSrc_arr[GIC_EDGE_NMI];

    printk("\nNMI is sent to CPU%d\n", cpu);

    /* CPU 0 & 1 belong to Core0 whose exception base is 0xbfc00000. 
     * When NMI is sent CPU 0 or 1, CPU will go to 0xbfc00000 to fetch instruction.
     * Flash needs to be set as auto mode before CPU can access it */
    if (cpu<2)
        flash_auto_mode_set();

    /* set intSrc20 as NMI */
    GICWRITE(GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(intSrc)), GIC_MAP_TO_NMI_MSK);

    /* set intSrc20 affinity to CPU */
    GIC_SH_MAP_TO_VPE_SMASK(intSrc, cpu);

    /* enable int mask for intSrc20 */
    GIC_SET_INTR_MASK(intSrc);

    /* GIC EDGE_WRITE register simulates intSrc20 to send an interrupt to CPU */
    GICWRITE(GIC_REG(SHARED, GIC_SH_WEDGE), 0x80000000 | intSrc);

   /* disable int mask for intSrc20 */
    GIC_CLR_INTR_MASK(intSrc);

    return;
}
EXPORT_SYMBOL(gic_edge_nmi_send);

int get_irqNum_by_name(int *irqNumP, char *irqName) 
{
    int i;

    for (i=0; i<(GIC_NUM_INTRS); i++) {
        if (gic_intr_map[i].name == NULL)
            continue;
        
        if (strncmp(gic_intr_map[i].name, irqName, strlen(gic_intr_map[i].name)) == 0) {
            *(irqNumP) = gic_intr_map[i].pin+1;
            return 0;
        }
    }
    
    return -1;
}

void ecnt_register_cpu_interrupt_fops(void)
{
    if (register_chrdev(CPU_INTERRUPT_MAJOR, "/dev/cpu_interrupt", &cpu_interrupt_fops) < 0) {
        printk(KERN_WARNING "cpu_interrupt: can't get major %d\n", CPU_INTERRUPT_MAJOR);
    }
    return;
}

static long cpu_interrupt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) 
{
	int ret = 0 ;
	struct ecnt_cpu_interrupt_data data;
	struct ecnt_cpu_interrupt_data* puser = (struct ecnt_cpu_interrupt_data*)arg;

	if (cmd >= CPU_INTERRUPT_FUNCTION_MAX_NUM) {
        printk("\nError: cmd:%d is too large. Max is %d\n", cmd, CPU_INTERRUPT_FUNCTION_MAX_NUM);
		return -1;
    }
	
	memset(&data,0,sizeof(struct ecnt_cpu_interrupt_data));
	copy_from_user(&data, puser ,sizeof(struct ecnt_cpu_interrupt_data));	
	ret = cpu_interrupt_operation[data.function_id](&data);
	copy_to_user(puser,&data,sizeof(struct ecnt_cpu_interrupt_data));
	
	return ret;
}

int cpu_interrupt_api_get_irqnum(struct ecnt_cpu_interrupt_data *data)
{
    if (get_irqNum_by_name(&(data->irqNum), data->irqString)==0)
        data->retValue = 0;
    else
        data->retValue = -1;

    return data->retValue;
}

int cpu_interrupt_api_show_interrupts(struct ecnt_cpu_interrupt_data *data)
{
    int i;
    unsigned int cpu, value;

    printk("\n");
    printk("hwIntr  swIntr  CPU     Name\n");

    for (i=0; i<(GIC_NUM_INTRS); i++) {
        
        if (gic_intr_map[i].name == NULL)
            continue;

        value = GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_VPE_REG_OFF(i, 0));
        
        for (cpu=0; cpu<4; cpu++) {
            if ((value>>cpu) & 0x1)
                break;
        }

        printk("%d\t%d\t%d\t%s\n", i, gic_intr_map[i].pin+1, cpu, gic_intr_map[i].name);     
    }

    printk("\n");

    data->retValue = 0;
    return data->retValue;
}

int cpu_interrupt_api_check_intrName(struct ecnt_cpu_interrupt_data *data)
{
    return cpu_interrupt_api_get_irqnum(data);
}


void ecnt_register_irq_num_op(void)
{
    if(ecnt_register_hook(&ecnt_irq_num_op)) {
        printk("ecnt_irq_num_op register fail\n");
    }
    return;
}

ecnt_ret_val ecnt_irq_num_hook(struct ecnt_data *in_data)
{
	struct ECNT_IRQ_NUM_Data *irq_num_data = (struct ECNT_IRQ_NUM_Data *)in_data ;	


	if(irq_num_data->function_id >= IRQ_NUM_FUNCTION_MAX_NUM) {
		printk("irq_num_data->function_id is %d, exceed max number: %d", irq_num_data->function_id, IRQ_NUM_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}

    if (get_irqNum_by_name(&(irq_num_data->irqNum), irq_name_arr[irq_num_data->function_id])==0)
        return ECNT_RETURN;

    printk("\n%s can't get correct irq number\n", irq_name_arr[irq_num_data->function_id]);
    return ECNT_HOOK_ERROR;
}

void ecnt_register_cpu_interrupts(void) {
    ecnt_register_irq_num_op();
    ecnt_register_cpu_interrupt_fops();
    return;
}
EXPORT_SYMBOL(ecnt_register_cpu_interrupts);

#endif /*TCSUPPORT_MIPS_1004K*/

void init_gicVecPlus1_to_intSrc(void)
{
    int gicVec, intSrc;

    /*irqVec: 1 ~ 63 -> gicVec: 0 ~ 62*/
    for (gicVec=0; gicVec<IRQ_NUM_MAX_VALUE; gicVec++) {
        for (intSrc = 0; intSrc < GIC_NUM_INTRS; intSrc++) {
            if (gicVec == (gic_intr_map[intSrc].pin)) { /*use GIC vector to find intSrc*/
                gicVecPlus1_to_intSrc_arr[gicVec+1] = intSrc;
                break;
            }
        }
    }

    return;
}

/* translate gic_irq_controller's functions' "gicVec+1" into
 * intSrc, because GIC uses intSrc number to set its registers */
unsigned int gicVecPlus1_to_intSrc (unsigned int gicVecPlus1)
{
    return gicVecPlus1_to_intSrc_arr[gicVecPlus1];
}
EXPORT_SYMBOL(gicVecPlus1_to_intSrc);

static int get_intSrc_by_irqNum(int irq) 
{
    if (irq == SI_TIMER_INT)
        irq = timers_intSrcNum[smp_processor_id()];
    else
        irq = gicVecPlus1_to_intSrc(irq);

    return irq;
}

static void ecnt_gic_mask_irq(struct irq_data *d)
{
    int irq = (d->irq - gic_irq_base);

    /* disable this interrupt */

    GIC_CLR_INTR_MASK(get_intSrc_by_irqNum(irq));
}

static void ecnt_gic_unmask_irq(struct irq_data *d)
{
    int irq = (d->irq - gic_irq_base);

    /* used to enable int mask during setup_irq */

    GIC_SET_INTR_MASK(get_intSrc_by_irqNum(irq));
}

void gic_irq_ack(struct irq_data *d)
{
	int irq = (d->irq - gic_irq_base);

    irq = get_intSrc_by_irqNum(irq);
    
    GIC_CLR_INTR_MASK(irq);
    if (gic_irq_flags[irq] & GIC_TRIG_EDGE)
        GICWRITE(GIC_REG(SHARED, GIC_SH_WEDGE), irq);
}

void gic_finish_irq(struct irq_data *d)
{
    int irq = (d->irq - gic_irq_base);
   
    /* Enable interrupts. */
    GIC_SET_INTR_MASK(get_intSrc_by_irqNum(irq));
}

#ifdef CONFIG_SMP
static DEFINE_SPINLOCK(gic_lock);

static int ecnt_gic_set_affinity(struct irq_data *d, const struct cpumask *cpumask,
			    bool force)
{
	unsigned int irq = (d->irq - gic_irq_base);
	cpumask_t	tmp = CPU_MASK_NONE;
	unsigned long	flags;
	int		i;

	cpumask_and(&tmp, cpumask, cpu_online_mask);

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	if (cpumask_empty(&tmp))
		return -EINVAL;
#else		
	if (cpus_empty(tmp))
		return -1;
#endif		

	/* Assumption : cpumask refers to a single CPU */
	spin_lock_irqsave(&gic_lock, flags);

	/* Re-route this IRQ */
#ifdef TCSUPPORT_MIPS_1004K
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	GIC_SH_MAP_TO_VPE_SMASK(gicVecPlus1_to_intSrc(irq), cpumask_first(&tmp));
#else
	GIC_SH_MAP_TO_VPE_SMASK(gicVecPlus1_to_intSrc(irq), first_cpu(tmp));
#endif
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	GIC_SH_MAP_TO_VPE_SMASK(irq, cpumask_first(&tmp));
#else
	GIC_SH_MAP_TO_VPE_SMASK(irq, first_cpu(tmp));
#endif
#endif
#if 0
	/* Update the pcpu_masks */
	for (i = 0; i < NR_CPUS; i++)
		clear_bit(irq, pcpu_masks[i].pcpu_mask);
	set_bit(irq, pcpu_masks[first_cpu(tmp)].pcpu_mask);
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	cpumask_copy(d->common->affinity, cpumask);
#else
	cpumask_copy(d->affinity, cpumask);
#endif
	spin_unlock_irqrestore(&gic_lock, flags);

	return IRQ_SET_MASK_OK_NOCOPY;
}
#endif

/* ecnt_gic_irq_controller is originated from gic_irq_controller in irq-gic.c */
static struct irq_chip ecnt_gic_irq_controller = {
	.name			=	"ECNT MIPS GIC",
	.irq_ack		=   gic_irq_ack,
	.irq_mask		=	ecnt_gic_mask_irq,
	.irq_mask_ack	=	ecnt_gic_mask_irq,
	.irq_unmask		=	ecnt_gic_unmask_irq, /*used to enable int mask during setup_irq*/
	.irq_eoi		=	gic_finish_irq,
#ifdef CONFIG_SMP
	.irq_set_affinity	=	ecnt_gic_set_affinity,
#endif
};


void __init gic_platform_init(int irqs, struct irq_chip *irq_controller)
{
	int i;

    /*irqVec starts from 1 and ends at 63*/
	for (i = gic_irq_base+1; i < (gic_irq_base + irqs); i++){
		irq_set_chip(i, &ecnt_gic_irq_controller);
    }
}                                

#ifdef CONFIG_MIPS_MT_SMP
static irqreturn_t ipi_resched_interrupt(int irq, void *dev_id)
{
    scheduler_ipi();
    return IRQ_HANDLED;
}

static irqreturn_t ipi_call_interrupt(int irq, void *dev_id)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	generic_smp_call_function_interrupt();
#else
	smp_call_function_interrupt();
#endif	
	return IRQ_HANDLED;
}

static struct irqaction irq_resched = {
        .handler        = ipi_resched_interrupt,
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	.flags			= IRQF_PERCPU,
#else
        .flags          = IRQF_DISABLED|IRQF_PERCPU,
#endif     
        .name           = "IPI_resched"
};

static struct irqaction irq_call = {
        .handler        = ipi_call_interrupt,
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	.flags			= IRQF_PERCPU,
#else
        .flags          = IRQF_DISABLED|IRQF_PERCPU,
#endif        
        .name           = "IPI_call"
};

unsigned int plat_ipi_call_int_xlate(unsigned int cpu)
{
    return ipi_call_intSrcNum[cpu];
}

unsigned int plat_ipi_resched_int_xlate(unsigned int cpu)
{
    return ipi_resched_intSrcNum[cpu];
}

void __init arch_init_ipiirq(int irq, struct irqaction *action)
{
	setup_irq(irq, action);
	irq_set_handler(irq, handle_percpu_irq);
}
#endif /* CONFIG_MIPS_MT_SMP */
#endif /*CONFIG_IRQ_GIC*/

void disable_all_interrupts(void)
{
    #ifdef TCSUPPORT_MIPS_1004K
    VPint(PHY_TO_K1(GIC_BASE_ADDR)+GIC_SH_RMASK_OFS) = 0xffffffff;
    VPint(PHY_TO_K1(GIC_BASE_ADDR)+GIC_SH_RMASK_OFS+4) = 0xffffffff;
    #else
    VPint(CR_INTC_IMR) = 0x0;
    VPint(CR_INTC_IMR_1) = 0x0;
    #endif
}
EXPORT_SYMBOL(disable_all_interrupts);

void disable_interrupt_by_intSrc(unsigned int intSrc)
{
    #ifdef TCSUPPORT_MIPS_1004K
    GIC_CLR_INTR_MASK(intSrc);
    #else
    if (intSrc<=31)
        VPint(CR_INTC_IMR) &=~(1<<intSrc);
    else
        VPint(CR_INTC_IMR_1) &=~((unsigned long long)1<<intSrc);
    #endif
}
EXPORT_SYMBOL(disable_interrupt_by_intSrc);

void enable_interrupt_by_intSrc(unsigned int intSrc)
{
    #ifdef TCSUPPORT_MIPS_1004K
    GIC_SET_INTR_MASK(intSrc);
    #else
    if (intSrc<=31)
	    VPint(CR_INTC_IMR) |=(1<<intSrc);
    else
        VPint(CR_INTC_IMR_1) |=((unsigned long long)1<<intSrc);
    #endif
}
EXPORT_SYMBOL(enable_interrupt_by_intSrc);

#define ALLINTS (IE_SW0 | IE_SW1 | IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5)

#ifdef CONFIG_MIPS_TC3262

static DEFINE_SPINLOCK(tc3162_irq_lock);

#ifndef TCSUPPORT_MIPS_1004K
//static inline void unmask_mips_mt_irq(unsigned int irq)
static inline void unmask_mips_mt_irq(struct irq_data *d)
{
	unsigned int irq = d->irq;
	unsigned int vpflags = dvpe();
	int cpu_irq = 0;

	if ((irq == SI_SWINT1_INT1) || (irq == SI_SWINT_INT1))  
		cpu_irq = 1;

	set_c0_status(0x100 << cpu_irq);
	irq_enable_hazard();
	evpe(vpflags);
}

//static inline void mask_mips_mt_irq(unsigned int irq)
static inline void mask_mips_mt_irq(struct irq_data *d)
{
	unsigned int irq = d->irq;
	unsigned int vpflags = dvpe();
	int cpu_irq = 0;

	if ((irq == SI_SWINT1_INT1) || (irq == SI_SWINT_INT1))  
		cpu_irq = 1;

	clear_c0_status(0x100 << cpu_irq);
	irq_disable_hazard();
	evpe(vpflags);
}

//static unsigned int mips_mt_cpu_irq_startup(unsigned int irq)
static unsigned int mips_mt_cpu_irq_startup(struct irq_data *d)
{
	unsigned int irq = d->irq;
	unsigned int vpflags = dvpe();
	unsigned long int tmp;
	int cpu_irq = 0;

	if ((irq == SI_SWINT1_INT1) || (irq == SI_SWINT_INT1))  
		cpu_irq = 1;
#ifdef TCSUPPORT_MT7510_E1
	READ_E1(CR_INTC_IMR);
#endif
	tmp = regRead32(CR_INTC_IMR);
	tmp |= (1 << (irq-1));

	if (irq == SI_SWINT_INT0){
		tmp |= (1 << (SI_SWINT1_INT0-1));
	}else if (irq == SI_SWINT_INT1){
		tmp |= (1 << (SI_SWINT1_INT1-1));
	}
	regWrite32(CR_INTC_IMR, tmp);

	clear_c0_cause(0x100 << cpu_irq);
	evpe(vpflags);
	unmask_mips_mt_irq(d);

	return 0;
}

/*
 * While we ack the interrupt interrupts are disabled and thus we don't need
 * to deal with concurrency issues.  Same for mips_cpu_irq_end.
 */
//static void mips_mt_cpu_irq_ack(unsigned int irq)
static void mips_mt_cpu_irq_ack(struct irq_data *d)
{	
	unsigned int irq = d->irq;
	unsigned int vpflags = dvpe();
	int cpu_irq = 0;

	if ((irq == SI_SWINT1_INT1) || (irq == SI_SWINT_INT1))  
		cpu_irq = 1;

	clear_c0_cause(0x100 << cpu_irq);
	evpe(vpflags);
	mask_mips_mt_irq(d);
}

static struct irq_chip mips_mt_cpu_irq_controller = {
	.name		= "MIPS",
	.irq_startup	= mips_mt_cpu_irq_startup,
	.irq_ack		= mips_mt_cpu_irq_ack,
	.irq_mask		= mask_mips_mt_irq,
	.irq_mask_ack	= mips_mt_cpu_irq_ack,
	.irq_unmask		= unmask_mips_mt_irq,
	.irq_eoi		= unmask_mips_mt_irq,
};
#endif /* ifndef TCSUPPORT_MIPS_1004K */

#define __BUILD_IRQ_DISPATCH(irq_n) \
static void __tc3262_irq_dispatch##irq_n(void) \
{								\
	do_IRQ(irq_n);				\
}	

#define __BUILD_IRQ_DISPATCH_FUNC(irq_n)  __tc3262_irq_dispatch##irq_n 

/* pre-built 64 irq dispatch function */
__BUILD_IRQ_DISPATCH(0)
__BUILD_IRQ_DISPATCH(1)
__BUILD_IRQ_DISPATCH(2)
__BUILD_IRQ_DISPATCH(3)
__BUILD_IRQ_DISPATCH(4)
__BUILD_IRQ_DISPATCH(5)
__BUILD_IRQ_DISPATCH(6)
__BUILD_IRQ_DISPATCH(7)
__BUILD_IRQ_DISPATCH(8)
__BUILD_IRQ_DISPATCH(9)
__BUILD_IRQ_DISPATCH(10)
__BUILD_IRQ_DISPATCH(11)
__BUILD_IRQ_DISPATCH(12)
__BUILD_IRQ_DISPATCH(13)
__BUILD_IRQ_DISPATCH(14)
__BUILD_IRQ_DISPATCH(15)
__BUILD_IRQ_DISPATCH(16)
__BUILD_IRQ_DISPATCH(17)
__BUILD_IRQ_DISPATCH(18)
__BUILD_IRQ_DISPATCH(19)
__BUILD_IRQ_DISPATCH(20)
__BUILD_IRQ_DISPATCH(21)
__BUILD_IRQ_DISPATCH(22)
__BUILD_IRQ_DISPATCH(23)
__BUILD_IRQ_DISPATCH(24)
__BUILD_IRQ_DISPATCH(25)
__BUILD_IRQ_DISPATCH(26)
__BUILD_IRQ_DISPATCH(27)
__BUILD_IRQ_DISPATCH(28)
__BUILD_IRQ_DISPATCH(29)
__BUILD_IRQ_DISPATCH(30)
__BUILD_IRQ_DISPATCH(31)
__BUILD_IRQ_DISPATCH(32)
__BUILD_IRQ_DISPATCH(33)
__BUILD_IRQ_DISPATCH(34)
__BUILD_IRQ_DISPATCH(35)
__BUILD_IRQ_DISPATCH(36)
__BUILD_IRQ_DISPATCH(37)
__BUILD_IRQ_DISPATCH(38)
__BUILD_IRQ_DISPATCH(39)
__BUILD_IRQ_DISPATCH(40)
__BUILD_IRQ_DISPATCH(41)
__BUILD_IRQ_DISPATCH(42)
__BUILD_IRQ_DISPATCH(43)
__BUILD_IRQ_DISPATCH(44)
__BUILD_IRQ_DISPATCH(45)
__BUILD_IRQ_DISPATCH(46)
__BUILD_IRQ_DISPATCH(47)
__BUILD_IRQ_DISPATCH(48)
__BUILD_IRQ_DISPATCH(49)
__BUILD_IRQ_DISPATCH(50)
__BUILD_IRQ_DISPATCH(51)
__BUILD_IRQ_DISPATCH(52)
__BUILD_IRQ_DISPATCH(53)
__BUILD_IRQ_DISPATCH(54)
__BUILD_IRQ_DISPATCH(55)
__BUILD_IRQ_DISPATCH(56)
__BUILD_IRQ_DISPATCH(57)
__BUILD_IRQ_DISPATCH(58)
__BUILD_IRQ_DISPATCH(59)
__BUILD_IRQ_DISPATCH(60)
__BUILD_IRQ_DISPATCH(61)
__BUILD_IRQ_DISPATCH(62)
__BUILD_IRQ_DISPATCH(63)

/* register pre-built 64 irq dispatch function */
static void (*irq_dispatch_tab[])(void) =
{
__BUILD_IRQ_DISPATCH_FUNC(0),
__BUILD_IRQ_DISPATCH_FUNC(1),
__BUILD_IRQ_DISPATCH_FUNC(2),
__BUILD_IRQ_DISPATCH_FUNC(3),
__BUILD_IRQ_DISPATCH_FUNC(4),
__BUILD_IRQ_DISPATCH_FUNC(5),
__BUILD_IRQ_DISPATCH_FUNC(6),
__BUILD_IRQ_DISPATCH_FUNC(7),
__BUILD_IRQ_DISPATCH_FUNC(8),
__BUILD_IRQ_DISPATCH_FUNC(9),
__BUILD_IRQ_DISPATCH_FUNC(10),
__BUILD_IRQ_DISPATCH_FUNC(11),
__BUILD_IRQ_DISPATCH_FUNC(12),
__BUILD_IRQ_DISPATCH_FUNC(13),
__BUILD_IRQ_DISPATCH_FUNC(14),
__BUILD_IRQ_DISPATCH_FUNC(15),
__BUILD_IRQ_DISPATCH_FUNC(16),
__BUILD_IRQ_DISPATCH_FUNC(17),
__BUILD_IRQ_DISPATCH_FUNC(18),
__BUILD_IRQ_DISPATCH_FUNC(19),
__BUILD_IRQ_DISPATCH_FUNC(20),
__BUILD_IRQ_DISPATCH_FUNC(21),
__BUILD_IRQ_DISPATCH_FUNC(22),
__BUILD_IRQ_DISPATCH_FUNC(23),
__BUILD_IRQ_DISPATCH_FUNC(24),
__BUILD_IRQ_DISPATCH_FUNC(25),
__BUILD_IRQ_DISPATCH_FUNC(26),
__BUILD_IRQ_DISPATCH_FUNC(27),
__BUILD_IRQ_DISPATCH_FUNC(28),
__BUILD_IRQ_DISPATCH_FUNC(29),
__BUILD_IRQ_DISPATCH_FUNC(30),
__BUILD_IRQ_DISPATCH_FUNC(31),
__BUILD_IRQ_DISPATCH_FUNC(32),
__BUILD_IRQ_DISPATCH_FUNC(33),
__BUILD_IRQ_DISPATCH_FUNC(34),
__BUILD_IRQ_DISPATCH_FUNC(35),
__BUILD_IRQ_DISPATCH_FUNC(36),
__BUILD_IRQ_DISPATCH_FUNC(37),
__BUILD_IRQ_DISPATCH_FUNC(38),
__BUILD_IRQ_DISPATCH_FUNC(39),
__BUILD_IRQ_DISPATCH_FUNC(40),
__BUILD_IRQ_DISPATCH_FUNC(41),
__BUILD_IRQ_DISPATCH_FUNC(42),
__BUILD_IRQ_DISPATCH_FUNC(43),
__BUILD_IRQ_DISPATCH_FUNC(44),
__BUILD_IRQ_DISPATCH_FUNC(45),
__BUILD_IRQ_DISPATCH_FUNC(46),
__BUILD_IRQ_DISPATCH_FUNC(47),
__BUILD_IRQ_DISPATCH_FUNC(48),
__BUILD_IRQ_DISPATCH_FUNC(49),
__BUILD_IRQ_DISPATCH_FUNC(50),
__BUILD_IRQ_DISPATCH_FUNC(51),
__BUILD_IRQ_DISPATCH_FUNC(52),
__BUILD_IRQ_DISPATCH_FUNC(53),
__BUILD_IRQ_DISPATCH_FUNC(54),
__BUILD_IRQ_DISPATCH_FUNC(55),
__BUILD_IRQ_DISPATCH_FUNC(56),
__BUILD_IRQ_DISPATCH_FUNC(57),
__BUILD_IRQ_DISPATCH_FUNC(58),
__BUILD_IRQ_DISPATCH_FUNC(59),
__BUILD_IRQ_DISPATCH_FUNC(60),
__BUILD_IRQ_DISPATCH_FUNC(61),
__BUILD_IRQ_DISPATCH_FUNC(62),
__BUILD_IRQ_DISPATCH_FUNC(63)
};

#endif
#ifndef TCSUPPORT_MIPS_1004K
//__IMEM static inline void unmask_mips_irq(unsigned int irq)
__IMEM static inline void unmask_mips_irq(struct irq_data *data)
{
	//pr_info("\nUNMASK_mips_irq");
	unsigned int irq = data->irq;
#ifdef CONFIG_MIPS_TC3262
	unsigned long flags;
	unsigned long int tmp;
	int cpu = smp_processor_id();

	//printk("unmask_mips_irq: 1! irq is %d, \r\n", irq);

	spin_lock_irqsave(&tc3162_irq_lock, flags);
#ifdef CONFIG_MIPS_MT_SMTC
	if (cpu_data[cpu].vpe_id != 0) {
#else
	if (cpu != 0) {
#endif
		if (irq == SI_TIMER_INT)
			irq = SI_TIMER1_INT;
	}

	//printk("unmask_mips_irq: 2! irq is %d, \r\n", irq);

	if (irq <= 32)
	{
#ifdef TCSUPPORT_MT7510_E1
        	READ_E1(CR_INTC_IMR);
#endif
		tmp = regRead32(CR_INTC_IMR);
		tmp |=  (1 << (irq-1));
		regWrite32(CR_INTC_IMR, tmp);

		//printk("unmask_mips_irq: entered! irq is %d, CR_INTC_IMR %08x write value is [%08x]\r\n", irq, CR_INTC_IMR, tmp);
		//tmp = regRead32(CR_INTC_IMR);
		//printk("unmask_mips_irq: entered! irq is %d, REREAD CR_INTC_IMR %08x write value is [%08x]\r\n", irq, CR_INTC_IMR, tmp);

	}else
	{
#ifdef TCSUPPORT_MT7510_E1
       		READ_E1(CR_INTC_IMR_1);
#endif
		tmp = regRead32(CR_INTC_IMR_1);
		tmp |=  (1 << (irq-33));
		regWrite32(CR_INTC_IMR_1, tmp);
		//printk("unmask_mips_irq: entered! irq is %d, CR_INTC_IMR_1 %08x write value is [%08x]\r\n", irq, CR_INTC_IMR_1, tmp);
		//tmp = regRead32(CR_INTC_IMR_1);
		//printk("unmask_mips_irq: entered! irq is %d, REREAD CR_INTC_IMR_1 %08x write value is [%08x]\r\n", irq, CR_INTC_IMR_1, tmp);

	}
	spin_unlock_irqrestore(&tc3162_irq_lock, flags);
#else
	VPint(CR_INTC_IMR) |=  (1 << irq);
#endif
}

//__IMEM static inline void mask_mips_irq(unsigned int irq)
__IMEM static inline void mask_mips_irq(struct irq_data *data)
{
	unsigned int irq = data->irq;
#ifdef CONFIG_MIPS_TC3262
	unsigned long flags;
	unsigned long int tmp;
	int cpu = smp_processor_id();
	//printk("mask_mips_irq: 1! irq is %d, \r\n", irq);

	spin_lock_irqsave(&tc3162_irq_lock, flags);
#ifdef CONFIG_MIPS_MT_SMTC
	if (cpu_data[cpu].vpe_id != 0) {
#else
	if (cpu != 0) {
#endif
		if (irq == SI_TIMER_INT)
			irq = SI_TIMER1_INT;
	}
	//printk("mask_mips_irq: 2! irq is %d, \r\n", irq);

	if (irq <= 32){
#ifdef TCSUPPORT_MT7510_E1
        	READ_E1(CR_INTC_IMR);
#endif
		tmp = regRead32(CR_INTC_IMR);
		tmp &= ~(1 << (irq-1));
		regWrite32(CR_INTC_IMR, tmp);

		//printk("mask_mips_irq: entered! irq is %d, CR_INTC_IMR %08x write value is [%08x]\r\n", irq, CR_INTC_IMR, tmp);
		//tmp = regRead32(CR_INTC_IMR);
		//printk("mask_mips_irq: entered! irq is %d, REREAD CR_INTC_IMR %08x write value is [%08x]\r\n", irq, CR_INTC_IMR, tmp);

	}else{
#ifdef TCSUPPORT_MT7510_E1
        	READ_E1(CR_INTC_IMR_1);
#endif
		tmp = regRead32(CR_INTC_IMR_1);
		tmp &= ~(1 << (irq-33));
		regWrite32(CR_INTC_IMR_1, tmp);
		//printk("mask_mips_irq: entered! irq is %d, CR_INTC_IMR_1 %08x write value is [%08x]\r\n", irq, CR_INTC_IMR_1, tmp);
		//tmp = regRead32(CR_INTC_IMR_1);
		//printk("mask_mips_irq: entered! irq is %d, REREAD CR_INTC_IMR_1 %08x write value is [%08x]\r\n", irq, CR_INTC_IMR_1, tmp);

	}
	spin_unlock_irqrestore(&tc3162_irq_lock, flags);
#else
	VPint(CR_INTC_IMR) &= ~(1 << irq);
#endif
}
#endif /* ifndef TCSUPPORT_MIPS_1004K */

void tc3162_enable_irq(unsigned int irq) /* the irq means intSrc+1 in 34K, but means intVec+1 in 1004K */
{
#ifdef CONFIG_MIPS_TC3262
	unsigned long flags;
	unsigned long int tmp;

	spin_lock_irqsave(&tc3162_irq_lock, flags);
#ifdef TCSUPPORT_MIPS_1004K
    GIC_SET_INTR_MASK(get_intSrc_by_irqNum(irq));    

#else    
	if (irq <= 32){
#ifdef TCSUPPORT_MT7510_E1
        	READ_E1(CR_INTC_IMR);
#endif
		tmp = regRead32(CR_INTC_IMR);
		tmp |=  (1 << (irq-1));
		regWrite32(CR_INTC_IMR, tmp);
	}else{
#ifdef TCSUPPORT_MT7510_E1
        	READ_E1(CR_INTC_IMR_1);
#endif
		tmp = regRead32(CR_INTC_IMR_1);
		tmp |=  (1 << (irq-33));
		regWrite32(CR_INTC_IMR_1, tmp);
	}
#endif
	spin_unlock_irqrestore(&tc3162_irq_lock, flags);
#else
	VPint(CR_INTC_IMR) |=  (1 << irq);
#endif
}
EXPORT_SYMBOL(tc3162_enable_irq);

void tc3162_disable_irq(unsigned int irq) /* the irq means intSrc+1 in 34K, but means intVec+1 in 1004K */
{
#ifdef CONFIG_MIPS_TC3262
	unsigned long flags;
	unsigned long int tmp;

	spin_lock_irqsave(&tc3162_irq_lock, flags);
#ifdef TCSUPPORT_MIPS_1004K
    GIC_CLR_INTR_MASK(get_intSrc_by_irqNum(irq));    

#else    
	if (irq <= 32){
#ifdef TCSUPPORT_MT7510_E1
        	READ_E1(CR_INTC_IMR);
#endif
		tmp = regRead32(CR_INTC_IMR);
		tmp &= ~(1 << (irq-1));
		regWrite32(CR_INTC_IMR, tmp);
	}else{
#ifdef TCSUPPORT_MT7510_E1
        	READ_E1(CR_INTC_IMR_1);
#endif
		tmp = regRead32(CR_INTC_IMR_1);
		tmp &= ~(1 << (irq-33));
		regWrite32(CR_INTC_IMR_1, tmp);
	}
#endif
	spin_unlock_irqrestore(&tc3162_irq_lock, flags);
#else
	VPint(CR_INTC_IMR) &= ~(1 << (irq-1));
#endif
}
EXPORT_SYMBOL(tc3162_disable_irq);
#ifndef TCSUPPORT_MIPS_1004K 
#ifdef CONFIG_MIPS_MT_SMP
extern int plat_set_irq_affinity(unsigned int irq,
				  const struct cpumask *affinity);
#endif

static struct irq_chip tc3162_irq_chip = {
	.name		= "MIPS",
	.irq_ack		= mask_mips_irq,
	.irq_mask		= mask_mips_irq,
	.irq_mask_ack	= mask_mips_irq,
	.irq_unmask		= unmask_mips_irq,
	.irq_eoi		= unmask_mips_irq,

#ifdef CONFIG_MIPS_MT_SMTC_IRQAFF
	.irq_set_affinity	= plat_set_irq_affinity,
#else
#ifdef CONFIG_MIPS_MT_SMP
	.irq_set_affinity	= plat_set_irq_affinity,
#endif
#endif /* CONFIG_MIPS_MT_SMTC_IRQAFF */
};
#endif /* ifndef TCSUPPORT_MIPS_1004K */

extern void vsmp_int_init(void);

#ifdef CONFIG_OF
static int ecnt_irq_domain_map(struct irq_domain *d, unsigned int irq,irq_hw_number_t hw)
{
	return 0;
}

static const struct irq_domain_ops econet_irq_domain_ops = 
{
	.map = ecnt_irq_domain_map
};

static int __init ecnt_of_irq_init(struct device_node *node,struct device_node *parent)
{
	irq_domain_add_legacy(node,NR_IRQS,0,0,&econet_irq_domain_ops,NULL);
	return 0;
}

static struct of_device_id  ecnt_irq_hw[] = {
	{ .compatible = "en751221,intc" , .data = ecnt_of_irq_init },
	{ },
};
#endif
void __init arch_init_irq(void)
{
	unsigned int i;

	/* Disable all hardware interrupts */
	clear_c0_status(ST0_IM);
	clear_c0_cause(CAUSEF_IP);

#ifdef CONFIG_IRQ_GIC
#if LINUX_VERSION_CODE >=  KERNEL_VERSION(5,4,0)
extern	void __iomem *mips_gcr_base;
#define CM_GCR_GIC_BASE_GICEN_MSK 	(0x1)
#define mips_cm_base 	mips_gcr_base
#endif
	if (mips_cm_base)  {
        write_gcr_gic_base(GIC_BASE_ADDR | CM_GCR_GIC_BASE_GICEN_MSK);
        gic_present = 1;
	}

	if (gic_present) {
    #ifdef TCSUPPORT_MIPS_1004K /*do this after everyone has filled the gic_intr_map table*/
        init_gicVecPlus1_to_intSrc();
    #endif
        /* replace all interrupts' irq_chip with gic_irq_controller and set interrupts' properties */
		gic_init(GIC_BASE_ADDR, GIC_ADDRSPACE_SZ, gic_intr_map,
				ARRAY_SIZE(gic_intr_map), MIPS_GIC_IRQ_BASE);

#if defined(CONFIG_MIPS_MT_SMP)
		set_c0_status(STATUSF_IP7 | STATUSF_IP6 | STATUSF_IP5 | STATUSF_IP4 | STATUSF_IP3 | STATUSF_IP2);
                
		/* set up ipi interrupts */
        for (i = 0; i < NR_CPUS; i++) {
            arch_init_ipiirq(IPI_RESCHED_INT0 + i, &irq_resched);
            arch_init_ipiirq(IPI_CALL_INT0 + i, &irq_call);
        }
#else
		set_c0_status(STATUSF_IP7 | STATUSF_IP6 | STATUSF_IP5 | STATUSF_IP2);
#endif
	}
#endif /*CONFIG_IRQ_GIC*/
	/* Initialize IRQ action handlers */
#ifdef TCSUPPORT_MIPS_1004K
    for (i = 1; i < NR_IRQS; i++) /* irqVec: 1~63 */
    {
        /* Note: irq_chip has already been assigned in gic_init() */
        
		if (i == SI_TIMER_INT) {
            irq_set_handler(i, handle_percpu_irq);
        }
        else if (i >= IPI_RESCHED_INT0) {
            /* handle_percpu_irq has already been assigned for IPI resched/call interrupts earlier */
            continue;
        }
		else {
            irq_set_handler(i, handle_level_irq);
        }
	}

#else
	for (i = 0; i < NR_IRQS; i++) {
#ifdef CONFIG_MIPS_TC3262
		/*
	 	 * Only MT is using the software interrupts currently, so we just
	 	 * leave them uninitialized for other processors.
	 	 */
		if (cpu_has_mipsmt) {
			if ((i == SI_SWINT1_INT0) || (i == SI_SWINT1_INT1) ||
				(i == SI_SWINT_INT0) || (i == SI_SWINT_INT1)) { 
				irq_set_chip(i, &mips_mt_cpu_irq_controller);
				continue;
			}
		}

		if ((i == SI_TIMER_INT) || (i == SI_TIMER1_INT))
			irq_set_chip_and_handler(i, &tc3162_irq_chip,
					 handle_percpu_irq);
		else
			irq_set_chip_and_handler(i, &tc3162_irq_chip,
					 handle_level_irq);
#else
		irq_set_chip_and_handler(i, &tc3162_irq_chip,
					 handle_level_irq);
#endif
	}
#endif /* TCSUPPORT_MIPS_1004K */

#ifdef CONFIG_MIPS_TC3262
	if (cpu_has_veic || cpu_has_vint) {
		write_c0_status((read_c0_status() & ~ST0_IM ) |
			                (STATUSF_IP0 | STATUSF_IP1)); 

		/* register irq dispatch functions */
		for (i = 0; i < NR_IRQS; i++)
			set_vi_handler(i, irq_dispatch_tab[i]);
	} else {
		change_c0_status(ST0_IM, ALLINTS);
	}
#else
	/* Enable all interrupts */
	change_c0_status(ST0_IM, ALLINTS);
#endif
#ifndef TCSUPPORT_MIPS_1004K /* vsmp_int_init's content has been moved to this file */
#ifdef CONFIG_MIPS_MT_SMP
	vsmp_int_init();
#endif
#endif
#ifdef CONFIG_OF
	of_irq_init(ecnt_irq_hw);
#endif
}

__IMEM asmlinkage void plat_irq_dispatch(void)
{
#ifdef CONFIG_MIPS_TC3262
	int irq = ((read_c0_cause() & ST0_IM) >> 10);
	pr_info("\nplat_irq_dispatch, irq = %d", irq);
	do_IRQ(irq);
#else
	do_IRQ(VPint(CR_INTC_IVR));
#endif
}

