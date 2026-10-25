/*
 *	Serial driver for TC3162 SoC
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/console.h>
#include <asm/div64.h>

#include <asm/tc3162/tc3162.h>

#ifdef TCSUPPORT_CPU_ARMV8
#include <modules/serial/ecnt_uart1.h>
#include <linux/of_platform.h>
#include <asm/io.h>
#include <modules/scu/ecnt_scu.h>
#endif


#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527)
#define TC3162_NR_PORTS				2	//UART2, UART3
#elif defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7581)
#define TC3162_NR_PORTS				1	//UART2
#elif defined(TCSUPPORT_CPU_EN7580) && !defined(TCSUPPORT_CPU_EN7581)
#define TC3162_NR_PORTS				4	//UART2, UART3, UART4, UART5
#elif defined(TCSUPPORT_CPU_EN7581)
#define TC3162_NR_PORTS				4	//UART2, HSUART3, UART4, UART5
#else
#define TC3162_NR_PORTS				1 	// set 1 UART by default
#endif


typedef enum UART_NUM {
	UART_2 = 1,
#if TC3162_NR_PORTS > 1
	UART_3,
#if TC3162_NR_PORTS > 2
	UART_4,
#if TC3162_NR_PORTS > 3
	UART_5,
#endif
#endif
#endif
} uart_num_e;

typedef enum UART_TYPE {
	UART_NORM = 1,
    UART_HIGH_SPEED,    /* identify high speed uart. */
} uart_type_e;

struct uart_desp_t {
    uint32_t uart_id;
    uint32_t uart_type;
#ifdef TCSUPPORT_CPU_ARMV8 /* for address mapping in ARM. */
    struct device *dev;
    void __iomem *base;	/* spi controller base address */
    int irq;
#endif
};

struct uart_desp_t ecnt_uarts[TC3162_NR_PORTS + 1] = {0};

#ifdef TCSUPPORT_HIGH_SPEED_UART
uint32_t high_speed_uarts[] = {UART_3};
#endif

#ifdef TCSUPPORT_CPU_ARMV8 // if ARM
static const struct of_device_id ecnt_uart2_of_ids[] = {
	{ .compatible = "econet,ecnt-uart2"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ecnt_uart2_of_ids);

#define UART_WRL(reg, data)	(writel(data, reg))
#define UART_RDL(reg)		(readl(reg))
#define TIOCSER_TEMT      0x01    /* Transmitter physically empty */

#else // if not ARM
	
#define	UART_WRL(reg, data)	(VPint(reg ) = data)
#define	UART_RDL(reg)		(VPint(reg))

#if defined(TCSUPPORT_CPU_EN7580)
#define NP_SCU_IOMUX_SHARE_REG		0xbfa20218
#elif defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527)
#define NP_SCU_IOMUX_SHARE_REG		0xbfa2015c
#endif

#endif // #ifdef TCSUPPORT_CPU_ARMV8


#define TC3162_UART_SIZE			0x30

#define PORT_TC3162					3162
#if 0 // def CONFIG_TC3162_ADSL
void (*send_uart_msg)(char* msg, int len);
EXPORT_SYMBOL(send_uart_msg);
static char tuart_buf[1024];
#endif

#define UART_BAUDRATE_MIN	110
#ifndef TCSUPPORT_HIGH_SPEED_UART
#define UART_BAUDRATE_MAX	115200
#else
#define UART_BAUDRATE_MAX	921600
#endif
#define UART_BAUDRATE		9600

#define	UART_BRDL_20M		0x01
#define	UART_BRDH_20M		0x00
#define UART_CRYSTAL_CLK_20M	20000000
#define UART_CRYSTAL_CLK_DIV	10

#define	UART_IER_MSTS		0x08

#define UART_MCR_RTS		0x02
#define UART_MCR_LOOP		0x10

#define UART_MSR_CTS		0x10

#define UART_LCR_DLAB		0x80
#define UART_LCR_BCON		0x40
#define UART_LCR_SPBEN		0x20
#define UART_LCR_EOPCON		0x10
#define UART_LCR_PCEN		0x08
#define UART_LCR_SB		0x04
#define UART_LCR_CLEN_MASK	0x03
#define UART_LCR_CLEN_C8	0x03
#define UART_LCR_CLEN_C7	0x02
#define UART_LCR_CLEN_C6	0x01
#define UART_LCR_CLEN_C5	0x00

#define UART_MISCC_CTSHWFC	0x08
#define UART_MISCC_RTSHWFC	0x04

#define UART_CFLAG_DEBUG	0x8000
#define UART_CFLAG_DEVON	0x10000


/* crystal clock is 20Mhz */
/*---------------------
| uclk_20M | baudrate |
|---------------------|
| 59904    | 115200   |
| 29952    | 57600    |
| 19968    | 38400    |
| 14976    | 28800    |
| 9984     | 19200    |
| 7488     | 14400    |
| 4992     | 9600     |
| 2496     | 4800     |
| 1248     | 2400     |
| 624      | 1200     |
| 312      | 600      |
| 156      | 300      |
| 57       | 110      |
---------------------*/
static unsigned long tc3162_get_uclk_20M(unsigned int baud)
{
#if (defined(TCSUPPORT_CPU_ARMV8) && !defined(TCSUPPORT_CPU_ARMV8_64))
	unsigned long long uclk_20M = 0;
#else
	unsigned long uclk_20M = 0;
#endif
	unsigned long long baud_tmp = 0;
#if (defined(TCSUPPORT_CPU_ARMV8) && !defined(TCSUPPORT_CPU_ARMV8_64))
	unsigned long long uclk_r = 0;
	unsigned long long uart_crystal_clk_20M = UART_CRYSTAL_CLK_20M;
#endif
	baud_tmp = baud;

#if (defined(TCSUPPORT_CPU_ARMV8) && !defined(TCSUPPORT_CPU_ARMV8_64))
	uclk_r = do_div(uart_crystal_clk_20M , UART_CRYSTAL_CLK_DIV);
	uclk_20M = (baud_tmp * UART_XYD_Y * (UART_BRDH_20M << 8 | UART_BRDL_20M) * 16);
	uclk_r = do_div(uclk_20M,uart_crystal_clk_20M);
#else
	uclk_20M = (baud_tmp * UART_XYD_Y * (UART_BRDH_20M << 8 | UART_BRDL_20M) * 16) / (UART_CRYSTAL_CLK_20M / UART_CRYSTAL_CLK_DIV);
#endif

	return uclk_20M;
}

#ifdef TCSUPPORT_HIGH_SPEED_UART
static uint32_t tc3162_get_uclk_20M_high_speed(uint32_t baud)
{
    uint32_t uclk_20M = 0;
    
	if (baud <= 115200) {
    	uclk_20M = (((baud / 100) * UART_XYD_Y / ((UART_CRYSTAL_CLK_20M / 100) / UART_CRYSTAL_CLK_DIV_HIGH_SPEED)) * 
                    (UART_BRDH_20M << 8 | UART_BRDL_20M) * 8);
    } else {
        /* divided by 100 to aviod uint32_t overflow */
        uclk_20M = (((baud / 100) * UART_XYD_Y_HIGH_SPEED / ((UART_CRYSTAL_CLK_20M / 100) / UART_CRYSTAL_CLK_DIV_HIGH_SPEED)) * 
                    (UART_BRDH_20M << 8 | UART_BRDL_20M) * 8);
    }
	
    return uclk_20M;
}
#endif


#ifdef TCSUPPORT_MT7510_E1
#define READ_OTHER(x) ((x & 0xc) + 0xbfb003a0)
#endif
static void tc3162ser_stop_tx(struct uart_port *port)
{
	UART_DPRINT_MSG();
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
	tmp = VPint(READ_OTHER(CR_UART2_IER));
	wmb();
	VPint(CR_UART2_IER + port->iobase) &= ~IER_THRE_INTERRUPT_ENABLE;	
	wmb();
#else
	UART_WRL(CR_UART2_IER + port->iobase, UART_RDL(CR_UART2_IER + port->iobase) & (~IER_THRE_INTERRUPT_ENABLE));
#endif
}

static void tc3162ser_irq_rx(struct uart_port *port)
{
	struct tty_struct *tty = port->state->port.tty;
	unsigned int ch, flg;

#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;

	while (1) {
		tmp = VPint(READ_OTHER(CR_UART2_LSR + port->iobase));
		wmb();
		if(!(VPint(CR_UART2_LSR + port->iobase)) & LSR_RECEIVED_DATA_READY)){
			wmb();
			break;
		}
#else
	while (UART_RDL(CR_UART2_LSR + port->iobase) & LSR_RECEIVED_DATA_READY) {
#endif
		/* 
		 * We need to read rds before reading the 
		 * character from the fifo
		 */
#ifdef TCSUPPORT_MT7510_E1
		tmp = VPint(READ_OTHER(CR_UART2_RBR + port->iobase));
		wmb();
		ch = VPint(CR_UART2_RBR + port->iobase);
		wmb();
#else
		ch = UART_RDL(CR_UART2_RBR  + port->iobase);
#endif
		port->icount.rx++;

		if (tty->port->low_latency)
			tty_flip_buffer_push(&port->state->port);

		flg = TTY_NORMAL;
		tty_insert_flip_char(&port->state->port, ch, flg);
	}
	tty_flip_buffer_push(&port->state->port);
}

static void tc3162ser_irq_tx(struct uart_port *port)
{
	struct circ_buf *xmit = &port->state->xmit;
	int count;

#if 0 // def CONFIG_TC3162_ADSL
	int len=0;
	memset(tuart_buf, 0, sizeof(tuart_buf));
#endif
	if (port->x_char) {
		UART_WRL(CR_UART2_THR + port->iobase, port->x_char);
#ifdef TCSUPPORT_MT7510_E1
		wmb();
#endif
		port->icount.tx++;
		port->x_char = 0;
		return;
	}
	if (uart_circ_empty(xmit) || uart_tx_stopped(port)) {
		tc3162ser_stop_tx(port);
		return;
	}

	count = port->fifosize;
	do {
		UART_WRL(CR_UART2_THR + port->iobase, xmit->buf[xmit->tail]);
#ifdef TCSUPPORT_MT7510_E1
		wmb();
#endif
#if 0 // def CONFIG_TC3162_ADSL
		if((void *)send_uart_msg){
			tuart_buf[len] = xmit->buf[xmit->tail];
			len++;
		}
#endif
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (--count > 0);
#if 0 // def CONFIG_TC3162_ADSL
	if((void *)send_uart_msg){
		send_uart_msg(tuart_buf, len);
	}
#endif

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(port);

	if (uart_circ_empty(xmit))
		tc3162ser_stop_tx(port);
}

static irqreturn_t tc3162ser_irq(int irq, void *dev_id)
{
	struct uart_port *port = dev_id;
	unsigned int iir;

#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
	tmp = VPint(READ_OTHER(CR_UART2_IIR + port->iobase));
	wmb();
	iir = VPint(CR_UART2_IIR + port->iobase);
	wmb();
#else
	iir = UART_RDL(CR_UART2_IIR + port->iobase);
#endif

	if (((iir & IIR_RECEIVED_DATA_AVAILABLE) == IIR_RECEIVED_DATA_AVAILABLE) ||
		((iir & IIR_RECEIVER_IDLE_TRIGGER) == IIR_RECEIVER_IDLE_TRIGGER)) 
    	{
		tc3162ser_irq_rx(port);
    	}
	if ((iir & IIR_TRANSMITTED_REGISTER_EMPTY) == IIR_TRANSMITTED_REGISTER_EMPTY) 
	{
		tc3162ser_irq_tx(port);    
	}

	/* TODO: Handle Modem Status Interrupt */

	return IRQ_HANDLED;
}

static unsigned int tc3162ser_tx_empty(struct uart_port *port)
{
	UART_DPRINT_MSG();
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART2_IIR + port->iobase));
	wmb();
#endif
	unsigned int ret;
	ret = ((UART_RDL(CR_UART2_LSR + port->iobase) & LSR_THRE) ? TIOCSER_TEMT : 0);
#ifdef TCSUPPORT_MT7510_E1
	wmb();
#endif	
    return ret;
}

static void tc3162ser_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
/* Do nothing, UART flow control is in hardware */
/* Chip hasn't support modem control yet */
/* TODO: Just pesudo code. Not yet tested */
#if 0
/*=====================================================================*/
	unsigned char mcr = 0;

	/* Don't need spinlock */
	mcr = tc_inb(CR_UART2_MCR);
	wmb();

	if (!(mctrl & TIOCM_RTS))
		mcr |= UART_MCR_RTS;
	else
		mcr &= ~UART_MCR_RTS;

	if (mctrl & TIOCM_LOOP)
		mcr |= UART_MCR_LOOP;
	else
		mcr &= ~UART_MCR_LOOP;

	tc_outb(CR_UART2_MCR, mcr);
	wmb();
/*=====================================================================*/
#endif
}

static unsigned int tc3162ser_get_mctrl(struct uart_port *port)
{
/* Chip hasn't support modem control yet */
/* TODO: Just pesudo code. Not yet tested */
#if 0
/*=====================================================================*/
	unsigned int mctrl = 0;
	unsigned char msr = 0;

	/* Don't need spinlock */
	msr = tc_inb(CR_UART2_MSR);
	wmb();

	if (msr & UART_MSR_CTS)
		mctrl |= TIOCM_CTS;

	return mctrl;
/*=====================================================================*/
#else
	/* Always return clear to send, because UART flow control is in hardware */
	return TIOCM_CTS;
#endif
}

static void tc3162ser_start_tx(struct uart_port *port)
{
	UART_DPRINT_MSG();
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART2_IER + port->iobase));
	wmb();
	VPint(CR_UART2_IER + port->iobase) |= IER_THRE_INTERRUPT_ENABLE;
	wmb();
#else
	UART_WRL(CR_UART2_IER + port->iobase, (UART_RDL(CR_UART2_IER + port->iobase) | IER_THRE_INTERRUPT_ENABLE));
#endif
}

static void tc3162ser_stop_rx(struct uart_port *port)
{
	UART_DPRINT_MSG();
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART2_IER + port->iobase));
	wmb();
	VPint(CR_UART2_IER + port->iobase) |= IER_THRE_INTERRUPT_ENABLE;
	wmb();
#else
	UART_WRL(CR_UART2_IER + port->iobase, (UART_RDL(CR_UART2_IER + port->iobase) & (~IER_RECEIVED_DATA_INTERRUPT_ENABLE)));
#endif
}

static void tc3162ser_enable_ms(struct uart_port *port)
{
/* Chip hasn't support modem status yet */
/* TODO: Just pesudo code. Not yet tested */
#if 0
/*=====================================================================*/
	unsigned char ier = 0;

	/* Don't need spinlock */
	ier = tc_inb(CR_UART2_IER);
	wmb();

	ier |= UART_IER_MSTS;
	tc_outb(CR_UART2_IER, ier);
 	wmb();
/*=====================================================================*/
#endif
}

static void tc3162ser_break_ctl(struct uart_port *port, int break_state)
{
	UART_DPRINT_MSG();
/* TODO: Just pesudo code. Not yet tested */
/*=====================================================================*/
	unsigned long flags, lcr = 0;

	spin_lock_irqsave(&port->lock, flags);
	lcr = UART_RDL(CR_UART2_LCR + port->iobase);

	wmb();
	if (break_state)
		lcr |= UART_LCR_BCON;
	else
		lcr &=~UART_LCR_BCON;

	UART_WRL(CR_UART2_LCR + port->iobase, lcr);
	wmb();

	spin_unlock_irqrestore(&port->lock, flags);
/*=====================================================================*/
}


static int tc3162ser_startup(struct uart_port *port)
{
	int ret;
	UART_DPRINT_MSG();

#ifdef TCSUPPORT_CPU_ARMV8
	if (port->unused[0] == UART_2) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART2", port);
    }
#if TC3162_NR_PORTS > 1
	else if (port->unused[0] == UART_3) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART3", port);
    }
#endif
#if TC3162_NR_PORTS > 2
	else if (port->unused[0] == UART_4) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART4", port);
    }
#endif
#if TC3162_NR_PORTS > 3
	else if (port->unused[0] == UART_5) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART5", port);
    }
#endif
#else
	if(port->iobase == CR_UART2_BASE) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART2", port);
    }
#if TC3162_NR_PORTS > 1
	else if(port->iobase == CR_UART3_BASE) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART3", port);
    }
#endif
#if TC3162_NR_PORTS > 2
	else if(port->iobase == CR_UART4_BASE) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART4", port);
    }
#endif
#if TC3162_NR_PORTS > 3
	else if(port->iobase == CR_UART5_BASE) {
		ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART5", port);
    }
#endif
#endif // TCSUPPORT_CPU_ARMV8
	
	if (ret) {
		printk(KERN_ERR "Couldn't get irq %d ret=%d\n", port->irq, ret);
		return ret;
	}
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
	tmp = VPint(READ_OTHER(CR_UART2_IER + port->iobase));
	wmb();
	VPint(CR_UART2_IER + port->iobase) |= IER_RECEIVED_DATA_INTERRUPT_ENABLE;
	wmb();
#else
	UART_WRL(CR_UART2_IER + port->iobase, UART_RDL(CR_UART2_IER + port->iobase) | IER_RECEIVED_DATA_INTERRUPT_ENABLE);
#endif

	return 0;
}

static void tc3162ser_shutdown(struct uart_port *port)
{
	UART_DPRINT_MSG();
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART2_IER + port->iobase));
	wmb();
	VPint(CR_UART2_IER + port->iobase) &= ~IER_RECEIVED_DATA_INTERRUPT_ENABLE;
	wmb();
#else
	UART_WRL(CR_UART2_IER + port->iobase, (UART_RDL(CR_UART2_IER + port->iobase) & (~IER_RECEIVED_DATA_INTERRUPT_ENABLE)));
#endif
	free_irq(port->irq, port);
}

static void tc3162ser_set_termios(struct uart_port *port,
    struct ktermios *termios, struct ktermios *old)
{
	UART_DPRINT_MSG();
	unsigned int baud = 0, reg = 0;
	unsigned long flags;
	unsigned long div_x = 0, div_y = 0, word = 0, lcr = 0;
	unsigned char lcr_tmp = 0, miscc = 0;
	
	switch (termios->c_cflag & CSIZE)
	{
	case CS5:
		lcr_tmp = UART_LCR_CLEN_C5;
		break;
	case CS6:
		lcr_tmp = UART_LCR_CLEN_C6;
		break;
	case CS7:
		lcr_tmp = UART_LCR_CLEN_C7;
		break;
	default:
	case CS8:
		lcr_tmp = UART_LCR_CLEN_C8;
		break;
	}

	if (termios->c_cflag & CSTOPB)
		lcr_tmp |= UART_LCR_SB;
	if (termios->c_cflag & PARENB)
		lcr_tmp |= UART_LCR_PCEN;
	if (!(termios->c_cflag & PARODD))
		lcr_tmp |= UART_LCR_EOPCON;
#ifdef CMSPAR
	if (termios->c_cflag & CMSPAR)
		lcr_tmp |= UART_LCR_SPBEN;
#endif

	if (termios->c_cflag & CRTSCTS)
	{
		if(port->unused[1] & UART_HWFC_ENABLE)
			miscc |= (UART_MISCC_CTSHWFC | UART_MISCC_RTSHWFC);
		else {
			termios->c_cflag &= ~CRTSCTS;
			printk("Chip hasn't support HW flow control yet\n");
		}
	}

	if (termios->c_cflag & UART_CFLAG_DEBUG) {
		port->unused[1] |= UART_DEBUG;
	} else {
		port->unused[1] &= ~UART_DEBUG;
	}

#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7580)
	if (termios->c_cflag & UART_CFLAG_DEVON) {
		if(!isFPGA) {
#ifdef TCSUPPORT_CPU_ARMV8
#if defined(TCSUPPORT_CPU_EN7581)
			if (port->unused[0] == UART_2) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART2_MODE, 1);
				if(port->unused[1] == UART_HWFC_ENABLE) {
					SET_IOMUX_CTRL_BIT(RG_GPIO_UART2_CTS_RTS_MODE, 1);
				}
			} else if(port->unused[0] == UART_3) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART3_MODE, 1);
				if(port->unused[1] == UART_HWFC_ENABLE) {
					SET_IOMUX_CTRL_BIT(RG_GPIO_UART3_CTS_RTS_MODE, 1);
				}
			} else if(port->unused[0] == UART_4) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART4_MODE, 1);
			} else if(port->unused[0] == UART_5) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART5_MODE, 1);
			}
#else
			SET_IOMUX_CTRL_BIT(RG_GPIO_UART2_MODE, 1);
#endif
#else
			reg = UART_RDL(NP_SCU_IOMUX_SHARE_REG); //GPIO_SHR_SCH 
#if defined(TCSUPPORT_CPU_EN7580)
			if(port->iobase == CR_UART2_BASE) {
				reg |= (0x1 << 3) | (0x1 << 4); //Turn on UART2_MODE, UART2_CTSRTS
			}else if(port->iobase == CR_UART3_BASE) {
				reg |= (0x1 << 5) | (0x1 << 6); //Turn on UART3_MODE, UART3_CTSRTS			
			}else if(port->iobase == CR_UART4_BASE){
				reg |= (0x1 << 7); //Turn on UART4_MODE
			}else if(port->iobase == CR_UART5_BASE){
				reg |= (0x1 << 8); //Turn on UART5_MODE 			
			}			
#else	
			if(port->iobase == CR_UART2_BASE) {
				reg |= (0x1 << 24) | (0x1 << 25); //Turn on UART2_MODE, UART2_CTSRTS
			} else if(port->iobase == CR_UART3_BASE) {
				reg |= (0x1 << 26); //Turn on UART3_MODE
			}
#endif
			UART_WRL(NP_SCU_IOMUX_SHARE_REG, reg);
#endif
		}
	} else {
		if(!isFPGA) {
#ifdef TCSUPPORT_CPU_ARMV8
#if defined(TCSUPPORT_CPU_EN7581)
			if (port->unused[0] == UART_2) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART2_MODE, 0);
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART2_CTS_RTS_MODE, 0);
			} else if(port->unused[0] == UART_3) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART3_MODE, 0);
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART3_CTS_RTS_MODE, 0);
			} else if(port->unused[0] == UART_4) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART4_MODE, 0);
			} else if(port->unused[0] == UART_5) {
				SET_IOMUX_CTRL_BIT(RG_GPIO_UART5_MODE, 0);
			}
#else
			SET_IOMUX_CTRL_BIT(RG_GPIO_UART2_MODE, 0);
#endif
#else
			reg = UART_RDL(NP_SCU_IOMUX_SHARE_REG); //GPIO_SHR_SCH 
#if defined(TCSUPPORT_CPU_EN7580)
			if(port->iobase == CR_UART2_BASE) { 		
				reg &= ~((0x1 << 3) | (0x1 << 4)); //Turn off UART2_MODE, UART2_CTSRTS			
			} else if(port->iobase == CR_UART3_BASE) {
				reg &= ~((0x1 << 5) | (0x1 << 6)); //Turn off UART3_MODE, UART3_CTSRTS			
			} else if(port->iobase == CR_UART4_BASE) {
				reg &= ~(0x1 << 7); //Turn off UART4_MODE			
			} else if(port->iobase == CR_UART5_BASE) {
				reg &= ~(0x1 << 8); //Turn off UART5_MODE						
			}			
#else
			if(port->iobase == CR_UART2_BASE) {
				reg &= ~((0x1 << 24) | (0x1 << 25)); //Turn off UART2_MODE, UART2_CTSRTS
			} else if(port->iobase == CR_UART3_BASE) {
				reg &= ~(0x1 << 26); //Turn off UART3_MODE
			}
#endif
			UART_WRL(NP_SCU_IOMUX_SHARE_REG, reg); 
#endif
		}
	}
#endif

	/*
	 * Ask the core to calculate the divisor for us.
	 */
	baud = uart_get_baud_rate(port, termios, old, UART_BAUDRATE_MIN, UART_BAUDRATE_MAX);

	div_y = UART_XYD_Y;
#ifdef TCSUPPORT_HIGH_SPEED_UART
    if (ecnt_uarts[port->unused[0]].uart_type == UART_HIGH_SPEED) {
        div_x = tc3162_get_uclk_20M_high_speed(baud);
        if (baud > 115200) {
            
div_y = UART_XYD_Y_HIGH_SPEED;
        }
    } else {
        div_x = tc3162_get_uclk_20M(baud);
    }
#else
	div_x = tc3162_get_uclk_20M(baud);
#endif

	word = ((div_x<<16) | div_y);

	termios->c_cflag |= CREAD;

	spin_lock_irqsave(&port->lock, flags);

	UART_WRL(CR_UART2_XYD + port->iobase, word);
	wmb();

	lcr = UART_RDL(CR_UART2_LCR + port->iobase);
	wmb();

	lcr &= ~(UART_LCR_SPBEN | UART_LCR_EOPCON | UART_LCR_PCEN | UART_LCR_SB | UART_LCR_CLEN_MASK);
	lcr |= lcr_tmp;

	UART_WRL(CR_UART2_LCR + port->iobase, lcr);
	wmb();

	UART_WRL(CR_UART2_MISCC + port->iobase, miscc);
	wmb();

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	/*
	 * Characters to ignore
	 */
	port->ignore_status_mask = 0;

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *tc3162ser_type(struct uart_port *port)
{
	UART_DPRINT_MSG();
	return port->type == PORT_TC3162 ? "TC3162" : NULL;
}

static void tc3162ser_config_port(struct uart_port *port, int flags)
{
	UART_DPRINT_MSG();
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_TC3162;
}

static void tc3162ser_release_port(struct uart_port *port)
{
	UART_DPRINT_MSG();
	release_mem_region(port->iobase, TC3162_UART_SIZE);
}

static int tc3162ser_request_port(struct uart_port *port)
{
	UART_DPRINT_MSG();

#ifdef TCSUPPORT_CPU_ARMV8
	if(port->unused[0] == UART_2)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart2") != NULL ? 0 : -EBUSY;
#if TC3162_NR_PORTS > 1
	else if(port->unused[0] == UART_3)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart3") != NULL ? 0 : -EBUSY;
#endif
#if TC3162_NR_PORTS > 2
	else if(port->unused[0] == UART_4)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart4") != NULL ? 0 : -EBUSY;
#endif
#if TC3162_NR_PORTS > 3
	else if(port->unused[0] == UART_5)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart5") != NULL ? 0 : -EBUSY;
#endif
#else
	if(port->iobase == CR_UART2_BASE)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart2") != NULL ? 0 : -EBUSY; 
#if TC3162_NR_PORTS > 1
	else if(port->iobase == CR_UART3_BASE)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart3") != NULL ? 0 : -EBUSY; 
#endif
#if TC3162_NR_PORTS > 2
	else if(port->iobase == CR_UART4_BASE)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart4") != NULL ? 0 : -EBUSY; 
#endif
#if TC3162_NR_PORTS > 3
	else if(port->iobase == CR_UART5_BASE)
		return request_mem_region(port->iobase, TC3162_UART_SIZE, "tc3162-uart5") != NULL ? 0 : -EBUSY; 
#endif 
#endif
}


static struct uart_ops tc3162ser_ops = {
	.tx_empty =		tc3162ser_tx_empty,
	.set_mctrl =	tc3162ser_set_mctrl,
	.get_mctrl =	tc3162ser_get_mctrl,
	.stop_tx =		tc3162ser_stop_tx,
	.start_tx =		tc3162ser_start_tx,
	.stop_rx =		tc3162ser_stop_rx,
	.enable_ms =	tc3162ser_enable_ms,
	.break_ctl =	tc3162ser_break_ctl,
	.startup =		tc3162ser_startup,
	.shutdown =		tc3162ser_shutdown,
	.set_termios =	tc3162ser_set_termios,
	.type =			tc3162ser_type,
	.config_port =	tc3162ser_config_port,
	.release_port =	tc3162ser_release_port,
	.request_port =	tc3162ser_request_port,
};

#if 0
static void tc3162_console_put(const char c)
{
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
	while (1){
		tmp = VPint(READ_OTHER(CR_UART2_IER));
		wmb();
		if((LSR_INDICATOR2 & LSR_THRE)){
			wmb();
			break;
		}
	}
	VPchar(CR_UART2_THR) = c;
	wmb();
#else
	while (!(LSR_INDICATOR2 & LSR_THRE))
		;
	VPchar(CR_UART2_THR) = c;
#endif
}

static void tc3162_console_write(struct console *con, const char *s,
    unsigned int count)
{
#if 0 // def CONFIG_TC3162_ADSL
	/*The prink message is hook this funcion.*/
	if((void *)send_uart_msg){
		send_uart_msg((char *)s, count);
	}
#endif
	while (count--) {
		if (*s == '\n')
			tc3162_console_put('\r');
		tc3162_console_put(*s);
		s++;
	}
}

static int tc3162_console_setup(struct console *con, char *options)
{
	return 0;
}

static struct uart_driver tc3162ser_reg;

static struct console tc3162_serconsole = {
	.name =		"ttyS2",
	.write =	tc3162_console_write,
	.device =	uart_console_device,
	.setup =	tc3162_console_setup,
	.flags =	CON_PRINTBUFFER,
	.cflag =	B115200 | CS8 | CREAD,
	.index =	-1,
	.data =		&tc3162ser_reg,
};

static int __init tc3162_console_init(void)
{
	register_console(&tc3162_serconsole);
	return 0;
}

console_initcall(tc3162_console_init);
#endif

static struct uart_port tc3162ser_ports[] = {
	{
        .iobase =	CR_UART2_BASE,
        .unused[0] = UART_2,
#ifndef TCSUPPORT_CPU_ARMV8
        .irq =		UART2_INT,
#endif
		.uartclk =	115200,
		.fifosize =	1,
		.ops =		&tc3162ser_ops,
		.line =		0,
		.flags =	ASYNC_BOOT_AUTOCONF,
		.unused[1] =	UART_HWFC_ENABLE,
	},
#if TC3162_NR_PORTS > 1
	{
        .iobase =	CR_UART3_BASE,
        .unused[0] = UART_3,
#ifndef TCSUPPORT_CPU_ARMV8
        .irq =		UART3_INT,
#endif
#ifdef TCSUPPORT_HIGH_SPEED_UART
		.uartclk =	921600,
		.fifosize =	128,
#else
		.uartclk =	115200,
		.fifosize =	1,
#endif
		.ops =		&tc3162ser_ops,
		.line =		0,
		.flags =	ASYNC_BOOT_AUTOCONF,
		.unused[1] =	UART_HWFC_ENABLE,
	},
#endif
#if TC3162_NR_PORTS > 2
	{
        .iobase =	CR_UART4_BASE,
        .unused[0] = UART_4,
#ifndef TCSUPPORT_CPU_ARMV8
        .irq =		UART4_INT,
#endif
		.uartclk =	115200,
		.fifosize =	1,
		.ops =		&tc3162ser_ops,
		.line =		0,
		.flags =	ASYNC_BOOT_AUTOCONF,
		.unused[1] =	UART_HWFC_DISABLE,
	},
#endif
#if TC3162_NR_PORTS > 3
	{
        .iobase =	CR_UART5_BASE,
        .unused[0] = UART_5,
#ifndef TCSUPPORT_CPU_ARMV8
        .irq =		UART5_INT,
#endif
		.uartclk =	115200,
		.fifosize =	1,
		.ops =		&tc3162ser_ops,
		.line =		0,
		.flags =	ASYNC_BOOT_AUTOCONF,
		.unused[1] =	UART_HWFC_DISABLE,
	},
#endif
};

static struct uart_driver tc3162ser_reg[] = {
	{
		.owner	=		THIS_MODULE,
		.driver_name =	"ttyS2",
		.dev_name =		"ttyS2",
		.major =		TTY_MAJOR,
		.minor =		66,
		.nr =			1,
	},
#if TC3162_NR_PORTS > 1
	{
		.owner	=		THIS_MODULE,
		.driver_name =	"ttyS3",
		.dev_name =		"ttyS3",
		.major =		TTY_MAJOR,
		.minor =		67,
		.nr =			1,
	},
#endif
#if TC3162_NR_PORTS > 2
	{
		.owner	=		THIS_MODULE,
		.driver_name =	"ttyS4",
		.dev_name =		"ttyS4",
		.major =		TTY_MAJOR,
		.minor =		68,
		.nr =			1,
	},
#endif
#if TC3162_NR_PORTS > 3

	{
		.owner	=		THIS_MODULE,
		.driver_name =	"ttyS5",
		.dev_name =		"ttyS5",
		.major =		TTY_MAJOR,
		.minor =		69,
		.nr =			1,
	},
#endif
};

static int __init tc3162ser_init(void)
{
	int ret;

	/* UART2 Initial Start*/
	unsigned long div_x = 0, div_y = 0;
	unsigned long word = 0;
	unsigned int reg = 0;
	struct uart_port *port;

#ifdef TCSUPPORT_CPU_ARMV8
	if (isEN7523 || isEN7581)
		printk("Init UART(s), still need to trun on for using.");
#else
	if(isEN751221) {
		reg = UART_RDL(0xbfa20104); //GPIO_SHR_SCH
		reg |= (0x1 << 18); //UART2_MODE
		reg &= ~((0x01 << 3) | (0x01 << 7)); /* Disable Lan0_LED GE_Led */
		UART_WRL(0xbfa20104, reg);
	}
	else if (isMT751020) {
		reg = UART_RDL(0xbfb00860); //GPIO_SHR_SCH
		reg |= (0x1 << 6); //UART2_MODE
		reg &= ~(0x01 << 26); /* Disable GSW PHY MDIO MODE */
		UART_WRL(0xbfb00860, reg);
	}
	else if (isEN751627) {
		printk("Init UART2 and UART3, still need to trun on for using.");
	}
	else if (isEN7580) {
		printk("Init UART2/UART3/UART4/UART5, still need to trun on for using.");
	}
	else {
		printk("Not support UART2!!!!\n");
		return -ENODEV;
	}
#endif

	return ret;
}

void uart2_loopback_test(void){
	int i;
	unsigned int reg1, reg2;
	for(i = 0; i < TC3162_NR_PORTS; i++) {
		/* Clear RBR */
		reg1 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_RBR);
		/* Set MCR bit[4] = 1 for loopback */
		reg1 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_MCR);
		reg1 |= 0x10;
		UART_WRL(tc3162ser_ports[i].iobase+CR_UART2_MCR, reg1);

		/* Write data to THR */
		UART_WRL(tc3162ser_ports[i].iobase+CR_UART2_THR, 0x5a);
		msleep(10);

		/* Read data from RBR */
		reg2 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_RBR);

		/* Compare data is ox5a or not */ 
		if( reg2 == 0x5a )
			printk("UART%d data loopback test success!\r\n", i+2);
		else
			printk("UART%d data loopback test fail! Tx:0x5a Rx:0x%02x\r\n", i+2, reg2);

		/* Set MCR bit[1] = 1 to enable RTS signal */
		reg1 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_MCR);
		reg1 |= 0x2;
		UART_WRL(tc3162ser_ports[i].iobase+CR_UART2_MCR, reg1);

		/* Read MSR to check bit[4] is 1 or not */
		reg1 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_MSR);

		/* Set MCR bit[1] = 0 to disable RTS signal */
		reg2 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_MCR);
		reg2 &= ~0x2;
		UART_WRL(tc3162ser_ports[i].iobase+CR_UART2_MCR, reg2);
		
		/* Read MSR to check bit[4] is 1 or not */
		reg2 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_MSR);
		if( ((reg1 & 0x10 ) == 0x10) &&  ((reg2 & 0x10 ) == 0))
			printk("UART%d RTS loopback test success!\r\n", i+2);
		else
			printk("UART%d RTS loopback test fail! MSR=0x%02x(RTS:1), MSR=0x%02x(RTS:0)\r\n", i+2, reg1, reg2);


		/* Set IER bit[0] = 0 to disable Rx interrupt */
		reg1 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_IER);
		reg1 &= ~1;
		UART_WRL(tc3162ser_ports[i].iobase+CR_UART2_IER, reg1);
	
		/* Write data(0x5a) to THR */
		UART_WRL(tc3162ser_ports[i].iobase+CR_UART2_THR, 0x5a);
		msleep(10);
	
		/* Read LSR to confirm RX FIFO is not empty */
		reg1 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_LSR);
	
		/* Set FCR bit[1] = 1 to clear RX FIFO*/
		reg2 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_FCR);
		reg2 |= 0x2;
		UART_WRL(tc3162ser_ports[i].iobase+CR_UART2_FCR, reg2);
//		msleep(100);
	
		/* Read LSR to confirm RX FIFO is empty */
		reg2 = UART_RDL(tc3162ser_ports[i].iobase+CR_UART2_LSR);
			
		if( ((reg1 & 0x01) != 0) && ((reg2 & 0x01) == 0) )
			printk("UART%d RX FIFO reset test success!\r\n", i+2);
		else
			printk("UART%d RX FIFO reset test fail! Rx FIFO reset before/after:0x%02x/0x%02x\r\n", i+2, reg1, reg2);			

		printk("\r\n");
									
	}
}


#if 0 // def CONFIG_TC3162_ADSL
/*_____________________________________________________________________________
**      function name: tcconsole_cmd
**      descriptions:
**             This function is used the send command to uart drivers that is used
**		tcconsole utility.
**
**      parameters:
**             cmd: 	Specify the command line strings.
**             len: 	The length of command.
**
**      global:
**             tc3162ser_ports
**
**      return:
**             none
**
**      call:
**      	tty_flip_buffer_push
**      	tty_insert_flip_char
**
**      revision:
**      1. Here 2010/9/23
**____________________________________________________________________________
*/
void 
tcconsole_cmd(char* cmd, int len){
	struct uart_port *port= &tc3162ser_ports[0];
	struct tty_struct *tty = port->state->port.tty;
	unsigned int ch, flg;
	int i;
    
	/*Ignore the line  feed character*/	
	for(i=0; i<len-1; i++){
		ch = cmd[i];
		port->icount.rx++;

		if (tty->low_latency)
			tty_flip_buffer_push(tty);

		flg = TTY_NORMAL;

		tty_insert_flip_char(tty, ch, flg);
	}
	tty_flip_buffer_push(tty);
}/*end tcconsole_cmd*/
EXPORT_SYMBOL(tcconsole_cmd);
#endif 

__initcall(tc3162ser_init);

#ifdef TCSUPPORT_CPU_ARMV8
static void ecnt_uart_write(struct uart_port *port, const char c)
{
	while (!(UART_RDL(CR_UART_LSR + port->iobase) & LSR_THRE));
	UART_WRL(CR_UART_THR + port->iobase, c);
}

static void __init ecnt_earlycon_write(struct console *console,
					const char *s, unsigned int count)
{
	struct earlycon_device *device = console->data;
	struct uart_port *port = &device->port;

	uart_console_write(port, s, count, ecnt_uart_write);
}

static int __init ecnt_earlycon_setup(struct earlycon_device *device,
					const char *options)
{
	if (!device->port.membase)
		return -ENODEV;

	ecnt_uarts[device->port.unused[0]].base = device->port.membase;

	device->con->write = ecnt_earlycon_write;

	return 0;
}
OF_EARLYCON_DECLARE(ecnt, "econet,ecnt-uart2", ecnt_earlycon_setup);

#ifdef TCSUPPORT_HIGH_SPEED_UART
static void set_uart_types(void)
{
    int i = 0;
    if (high_speed_uarts != 0 && sizeof(high_speed_uarts) != 0) {
        for (i = 0; i < sizeof(high_speed_uarts)/sizeof(high_speed_uarts[0]); ++i) {
            ecnt_uarts[high_speed_uarts[i]].uart_type = UART_HIGH_SPEED;
        }
    }
}
#endif

static int ecnt_uart2_drv_probe(struct platform_device *pdev)
{
	struct resource *res = NULL;
	int ret = 0;
	int i;

	/* UART2 Initial Start*/
	unsigned long div_x = 0, div_y = 0;
	unsigned long word = 0;
	// unsigned int reg = 0;
	struct uart_port *port;

	for(i = 0; i < TC3162_NR_PORTS; i++) {
        ecnt_uarts[UART_2 + i].uart_id = UART_2 + i;
        ecnt_uarts[UART_2 + i].uart_type = UART_NORM;

		platform_set_drvdata(pdev, &ecnt_uarts[UART_2 + i]);
		res = platform_get_resource(pdev, IORESOURCE_MEM, i);

		ecnt_uarts[i + 1].base = devm_ioremap_resource(&pdev->dev, res);
		if (IS_ERR(ecnt_uarts[i + 1].base)) {
			return PTR_ERR(ecnt_uarts[i + 1].base);
		}

		ecnt_uarts[i + 1].dev = &pdev->dev;

		ecnt_uarts[i + 1].irq  = platform_get_irq(pdev, i);

		if (ecnt_uarts[i + 1].irq <= 0)
			return ecnt_uarts[i + 1].irq;

		tc3162ser_ports[i].irq = ecnt_uarts[i + 1].irq ;
		tc3162ser_ports[i].iobase = ecnt_uarts[i + 1].base;
	}
#ifdef TCSUPPORT_HIGH_SPEED_UART
    set_uart_types();
#endif
	for(i = 0; i < TC3162_NR_PORTS; i++) {
		port = &tc3162ser_ports[i];

		/* Set FIFO control enable, reset RFIFO, TFIFO, 16550 mode, watermark=0x00 (1 byte) */
		UART_WRL(CR_UART2_FCR + port->iobase, UART_FCR|UART_WATERMARK);

		/* Set modem control to 0 */
		UART_WRL(CR_UART2_MCR + port->iobase, UART_MCR);

		/* Disable IRDA, Disable Power Saving Mode, RTS , CTS flow control */
		UART_WRL(CR_UART2_MISCC + port->iobase, UART_MISCC); 

		/* Set interrupt Enable to, enable Tx, Rx and Line status */
		UART_WRL(CR_UART2_IER + port->iobase, UART_IER);

		/* access the bardrate divider */
		UART_WRL(CR_UART2_LCR + port->iobase, UART_BRD_ACCESS);
        
		div_y = UART_XYD_Y;
#ifdef TCSUPPORT_HIGH_SPEED_UART
        if (ecnt_uarts[UART_2 + i].uart_type == UART_HIGH_SPEED) {
            div_x = tc3162_get_uclk_20M_high_speed(UART_BAUDRATE);
        } else {
            div_x = tc3162_get_uclk_20M(UART_BAUDRATE);
        }
#else
        div_x = tc3162_get_uclk_20M(UART_BAUDRATE);
#endif

		word = (div_x<<16)|div_y;
		UART_WRL(CR_UART2_XYD + port->iobase, word);
		/* Set Baud Rate Divisor to 1*16 */
		UART_WRL(CR_UART2_BRDL + port->iobase, UART_BRDL_20M);
		UART_WRL(CR_UART2_BRDH + port->iobase, UART_BRDH_20M);
		/* Set DLAB = 0, clength = 8, stop =1, no parity check */
		UART_WRL(CR_UART2_LCR + port->iobase, UART_LCR);
		/* UART2 Initial End */

		ret = uart_register_driver(&tc3162ser_reg[i]);
		if (!ret)
			uart_add_one_port(&tc3162ser_reg[i], &tc3162ser_ports[i]);
		else
			printk("port %d uart_register_driver fail!!\n", i);
	}
	
	return ret;
}

static int ecnt_uart2_drv_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver ecnt_uart_driver = {
	.probe = ecnt_uart2_drv_probe,
	.remove = ecnt_uart2_drv_remove,
	.driver = {
		.name = "ecnt-uart2",
		//.pm = MTK_NOR_DEV_PM_OPS,
		.of_match_table = ecnt_uart2_of_ids,
	},
};
module_platform_driver(ecnt_uart_driver);
MODULE_DESCRIPTION("EcoNet UART Driver");
#endif
