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
#include <asm/setup.h>
#include <asm/irq.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <modules/serial/ecnt_uart1.h>

#ifdef TCSUPPORT_CPU_ARMV8
#include <linux/of_platform.h>
#include <asm/io.h>

static const struct of_device_id ecnt_uart1_of_ids[] = {
	{ .compatible = "econet,ecnt-uart1"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ecnt_uart1_of_ids);

struct ecnt_uart1 {
	struct device *dev;
	void __iomem *base;	/* spi controller base address */
	int irq;
};

struct ecnt_uart1 ecnt_uart1;

#define UART_WRL(reg, data)	(writel(data, reg + ecnt_uart1.base))
#define UART_RDL(reg)		(readl(reg + ecnt_uart1.base))
#else
#define	UART_WRL(reg, data)	(VPint(reg) = data)
#define	UART_RDL(reg)		(VPint(reg))
#endif


#include <asm/tc3162/tc3162.h>
#ifdef TCSUPPORT_UART1_ENHANCE
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <asm/errno.h>
#include <linux/random.h>
#endif

#define CRITICAL_CONDITION_LOGLEVEL 2

//#define TCSUPPORT_UART_DISABLE_DBG

#define TC3162_NR_PORTS				1

#define TC3162_UART_SIZE			0x30

#define PORT_TC3162					3162
#ifdef CONFIG_TC3162_ADSL
void (*send_uart_msg)(char* msg, int len);
EXPORT_SYMBOL(send_uart_msg);
static char tuart_buf[1024];
#endif

#ifdef TCSUPPORT_UART1_ENHANCE
#define ENABLE 							(1)
#define DISABLE 						(0)
#define SUCCESS							(0)
#define FALSE							(1)

#ifndef TCSUPPORT_UART1_RINGBUFFER_KB_SIZE
#define TCSUPPORT_UART1_RINGBUFFER_KB_SIZE 4
#endif
#define PRINT_BUFFER_INIT_LEN_KB		(TCSUPPORT_UART1_RINGBUFFER_KB_SIZE << 10)
#define INSUFFICIENT_BUF_CNT_THRESHOLD	(5)

static unsigned char			isRingBufferMode = DISABLE;
static unsigned char			real_state = DISABLE;
static unsigned char			auto_mode = ENABLE;
static wait_queue_head_t		printQueue;
static struct task_struct		*printTask;
static int						printFlag;
static struct circ_buf			ringBuffer;
static int						ringBufferDbg = DISABLE;
static char buffer[PRINT_BUFFER_INIT_LEN_KB];
#endif

//int tc3162_uart_installed = 0;

#ifdef TCSUPPORT_MT7510_E1
#define READ_OTHER(x) ((x & 0xc) + 0xbfb003a0)
#endif

#if defined(TCSUPPORT_UART_DISABLE) && defined(TCSUPPORT_UART_TX_DISABLE)
static int isUartTxDis = 1;
#else
static int isUartTxDis = 0;
#endif
#if defined(TCSUPPORT_UART_RX_DISABLE)
static int isUartRxDis = 0;
#endif

#ifdef TCSUPPORT_UART1_ENHANCE
extern int uart_print_task_wakeup(const char *str, unsigned int str_len);
#endif
extern void msleep(unsigned int msecs);

void console_put_string(const char *str);
static void tc3162_console_put(const char c);

static void tc3162ser_stop_tx(struct uart_port *port)
{
#if 0
	if(tc3162_uart_installed == 1)
	{
		printk("tc3162ser_stop_tx: Can not stop tx return\r\n");
		return;	
	}
#endif
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
	tmp = VPint(READ_OTHER(CR_UART_IER));
	wmb();
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) & ~IER_THRE_INTERRUPT_ENABLE));
	wmb();
#else
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) & ~IER_THRE_INTERRUPT_ENABLE));
#endif
}

static void tc3162ser_irq_rx(struct uart_port *port)
{
	//struct tty_port *tty_port = port->state->port;
	struct tty_struct *tty ;
	unsigned int ch, flg;
	
	tty = port->state->port.tty;
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;

	while (1) {
		tmp = VPint(READ_OTHER(CR_UART_LSR));
		wmb();
		if(!(LSR_INDICATOR & LSR_RECEIVED_DATA_READY)){
			wmb();
			break;
		}
#else
	while (LSR_INDICATOR & LSR_RECEIVED_DATA_READY) {
#endif
		/* 
		 * We need to read rds before reading the 
		 * character from the fifo
		 */
#ifdef TCSUPPORT_MT7510_E1
		tmp = VPint(READ_OTHER(CR_UART_RBR));
		wmb();
		ch = UART_RDL(CR_UART_RBR);
		wmb();
#else
		ch = UART_RDL(CR_UART_RBR);
#endif

#if defined(TCSUPPORT_UART_RX_DISABLE)
		if(isUartRxDis) {
#ifdef TCSUPPORT_UART_DISABLE_DBG
			UART_WRL(CR_UART_THR,'r');
#endif
			ch = 0;
		}
#endif

		port->icount.rx++;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
		if (tty->port->low_latency){
			tty_flip_buffer_push(&port->state->port);
		}
#else
//		if (tty && tty->port->low_latency){
			tty_flip_buffer_push(&port->state->port);
//		}
#endif

		flg = TTY_NORMAL;
		tty_insert_flip_char(&port->state->port, ch, flg);
	}
	tty_flip_buffer_push(&port->state->port);
}

static void tc3162ser_irq_tx(struct uart_port *port)
{
	struct circ_buf *xmit = &port->state->xmit;
	int count;

#ifdef CONFIG_TC3162_ADSL
	int len=0;
	memset(tuart_buf, 0, sizeof(tuart_buf));
#endif
	if (port->x_char) {
		tc3162_console_put(port->x_char);
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
		tc3162_console_put(xmit->buf[xmit->tail]);
#ifdef TCSUPPORT_MT7510_E1
		wmb();
#endif
#ifdef CONFIG_TC3162_ADSL
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
#ifdef CONFIG_TC3162_ADSL
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
	uint8 iir;
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
	tmp = VPint(READ_OTHER(CR_UART_IIR));
	wmb();
	iir = IIR_INDICATOR;
	wmb();
#else
	iir = IIR_INDICATOR;
#endif
	
	if (((iir & IIR_RECEIVED_DATA_AVAILABLE) == IIR_RECEIVED_DATA_AVAILABLE) ||
		((iir & IIR_RECEIVER_IDLE_TRIGGER) == IIR_RECEIVER_IDLE_TRIGGER)) {
		tc3162ser_irq_rx(port);
	}
	if ((iir & IIR_TRANSMITTED_REGISTER_EMPTY) == IIR_TRANSMITTED_REGISTER_EMPTY) {
		tc3162ser_irq_tx(port);
	}
	return IRQ_HANDLED;
}

static unsigned int tc3162ser_tx_empty(struct uart_port *port)
{
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART_IIR));
	wmb();
#endif
	unsigned int ret;

        ret = (LSR_INDICATOR & LSR_THRE) ? TIOCSER_TEMT : 0;
#ifdef TCSUPPORT_MT7510_E1
	wmb();
#endif	
        return ret;
}

static void tc3162ser_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static unsigned int tc3162ser_get_mctrl(struct uart_port *port)
{
	unsigned int result = 0;
	return result;
}

static void tc3162ser_start_tx(struct uart_port *port)
{
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART_IER));
	wmb();
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) | IER_THRE_INTERRUPT_ENABLE));
	wmb();
#else
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) | IER_THRE_INTERRUPT_ENABLE));
#endif
}

static void tc3162ser_stop_rx(struct uart_port *port)
{
#if 0
	if(tc3162_uart_installed == 1)
	{
		printk("tc3162ser_stop_rx: Can not stop rx return\r\n");
		return;	
	}
#endif
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART_IER));
	wmb();
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) | IER_THRE_INTERRUPT_ENABLE));
	wmb();
#else
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) & ~IER_RECEIVED_DATA_INTERRUPT_ENABLE));
#endif
}

static void tc3162ser_enable_ms(struct uart_port *port)
{
}

static void tc3162ser_break_ctl(struct uart_port *port, int break_state)
{
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);
	spin_unlock_irqrestore(&port->lock, flags);
}

#ifdef CONFIG_MIPS_TC3262
static void tc3162ser_irq_dispatch(void)
{
	do_IRQ(UART_INT);
}	
#endif


static int tc3162ser_startup(struct uart_port *port)
{
	int ret;
#if 0
	if(tc3162_uart_installed != 0)
	{
		printk("tc3162ser_startup: Already startup return\r\n");
		return 0;
	}
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)
	tc3162_uart_installed = 1;
#endif
#endif
#ifdef CONFIG_MIPS_TC3262
	if (cpu_has_vint)
		set_vi_handler(port->irq, tc3162ser_irq_dispatch);
#endif
	ret = request_irq(port->irq, tc3162ser_irq, 0, "TC3162 UART", port);
	if (ret) {
		printk(KERN_ERR "Couldn't get irq %d ret=%d\n", port->irq, ret);
		return ret;
	}
	printk("%s \n", __FUNCTION__);
#ifdef TCSUPPORT_MT7510_E1
	unsigned int tmp;
	tmp = VPint(READ_OTHER(CR_UART_IER));
	wmb();
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) | IER_THRE_INTERRUPT_ENABLE));
	wmb();
#else
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) | IER_RECEIVED_DATA_INTERRUPT_ENABLE));
#endif
	return 0;
}

static void tc3162ser_shutdown(struct uart_port *port)
{
#if 0
	if(tc3162_uart_installed == 1)
	{
		printk("tc3162ser_shutdown: Can not shut down return\r\n");
		return;	
	}
#endif
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
        tmp = VPint(READ_OTHER(CR_UART_IER));
	wmb();
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) & ~IER_RECEIVED_DATA_INTERRUPT_ENABLE));
	wmb();
#else
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) & ~IER_RECEIVED_DATA_INTERRUPT_ENABLE));
#endif
	free_irq(port->irq, port);
}

static void tc3162ser_set_termios(struct uart_port *port,
    struct ktermios *termios, struct ktermios *old)
{
	unsigned int baud, quot;
	unsigned long flags;
	
	termios->c_cflag |= CREAD;

	baud = 115200;
	quot = uart_get_divisor(port, baud);

	spin_lock_irqsave(&port->lock, flags);

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
	return port->type == PORT_TC3162 ? "TC3162" : NULL;
}

static void tc3162ser_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_TC3162;
}

static void tc3162ser_release_port(struct uart_port *port)
{
	release_mem_region(port->iobase, TC3162_UART_SIZE);
}

static int tc3162ser_request_port(struct uart_port *port)
{
	return request_mem_region(port->iobase, TC3162_UART_SIZE,
	    "tc3162-uart") != NULL ? 0 : -EBUSY; 
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

static void tc3162_console_put(const char c)
{
#ifdef TCSUPPORT_MT7510_E1
        unsigned int tmp;
	while (1){
		tmp = VPint(READ_OTHER(CR_UART_IER));
		wmb();
		if((LSR_INDICATOR & LSR_THRE)){
			wmb();
			break;
		}
	}
	UART_WRL(CR_UART_THR,c);
	wmb();
#else
	while (!(LSR_INDICATOR & LSR_THRE))
		;

#if defined(TCSUPPORT_UART_TX_DISABLE)
	if(isUartTxDis) {
#ifdef TCSUPPORT_UART_DISABLE_DBG
		UART_WRL(CR_UART_THR,'t');
#endif
		return;
	} else 
#endif
	{
	UART_WRL(CR_UART_THR, c);
	}
#endif
}

static void tc3162_console_write(struct console *con, const char *s,
    unsigned int count)
{
#ifdef CONFIG_TC3162_ADSL
	/*The prink message is hook this funcion.*/
	if((void *)send_uart_msg){
		send_uart_msg((char *)s, count);
	}
#endif
#ifdef TCSUPPORT_UART1_ENHANCE
	/* before enter tc3162_console_write function, kernel has 
	  * call spin_lock_irqsave() for prevent printk.
	  */

	if(real_state == ENABLE) {
		uart_print_task_wakeup(s, count);
	} 
	else 
#endif
	{
		while (count--) {
			if (*s == '\n')
				tc3162_console_put('\r');
			tc3162_console_put(*s);
			s++;
		}
	}
}

static int tc3162_console_setup(struct console *con, char *options)
{
	return 0;
}

static struct uart_driver tc3162ser_reg;

static struct console tc3162_serconsole = {
	.name =		"ttyS",
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

static struct uart_port tc3162ser_ports[] = {
	{
		.iobase =	0xbfbf0000,
#ifndef TCSUPPORT_CPU_ARMV8
		.irq =		UART_INT,
#endif
		.uartclk =	115200,
		.fifosize =	1,
		.ops =		&tc3162ser_ops,
		.line =		0,
		.flags =	ASYNC_BOOT_AUTOCONF,
	}
};

static struct uart_driver tc3162ser_reg = {
	.owner	=		THIS_MODULE,
	.driver_name =	"ttyS",
	.dev_name =		"ttyS",
	.major =		TTY_MAJOR,
	.minor =		64,
	.nr =			1,
};

#ifdef TCSUPPORT_UART1_ENHANCE
/* The print message uses circular buffer.
 * A 'head' index - the point at which the producer insert items into the
 *                  buffer. 
 * A 'tail' index - the point at which the consumer finds the next item in
 *                  the buffer.
 *
 * example:
 * array[2:5] = "abc\n"
 *  -------------------------------
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 *  --------------------------------
 * | 0 | 1 | a | b | c | \n | 6 | 7 |
 *  --------------------------------
 *                                  
 *              tail                head
 *
 * The tail pointer is equal to the head pointer, the buffer is empty.
 * The buffer is full when the head pointer is one less than the tail pointer.
 * The head index is incremented when items are added, and the tail index
 * when items are removed.
 * The tail index should never jump the head index, and both indices should
 * wrapped to 0 when they reach the end of the buffer.
 */

/******************************************************************************
 Descriptor:	It's used to get message length in ring buffer.
 Input Args:	none.
 Ret Value:	Message length in ring buffer.
******************************************************************************/
static int get_ring_buffer_cnt(void)
{
	return CIRC_CNT(ringBuffer.head, ringBuffer.tail, PRINT_BUFFER_INIT_LEN_KB);
}

/******************************************************************************
 Descriptor:	It's used to get free space in ring buffer.
 Input Args:	none.
 Ret Value:	Free space in ring buffer.
******************************************************************************/
static int get_ring_buffer_space(void)
{
	return CIRC_SPACE(ringBuffer.head, ringBuffer.tail, PRINT_BUFFER_INIT_LEN_KB);
}

/******************************************************************************
 Descriptor:	It's used to add string to ring buffer, if the ring buffer
                has enough free space. On the contrary, if the ring buffer has
                no enough free space, it will show error message to console.
 Input Args:	str: The string pointer.
                str_len: The string length
 Ret Value:	none.
******************************************************************************/
static void add_msg_to_ring_buffer(char *str, int str_len)
{
	int offset, start, pos;
	static int insufficient_buf_cnt = 0;
	int buf_space = 0;
	char msg[64] = {0};

	if(ringBufferDbg == ENABLE) {
		buf_space = get_ring_buffer_space();
		snprintf(msg, sizeof(msg), "before add(%d) buffer space:%d\n", str_len, buf_space);
		console_put_string(msg);
	}

	if(str_len > get_ring_buffer_space()) {
		/* Has no enough ring buffer space to stroe message */
		insufficient_buf_cnt++;
		/* print insufficient ring buffer message */
		if(insufficient_buf_cnt > INSUFFICIENT_BUF_CNT_THRESHOLD) {
			console_put_string("insufficient ring buffer.\n");
			/* clear cnt */
			insufficient_buf_cnt = 0;
		}
		return;
	}

	/* store log message to buffer */
	start = ringBuffer.head;
	for(offset = 0; offset < str_len; offset++) {
		/* pos = (start + offset) % PRINT_BUFFER_INIT_LEN_KB */
		pos = (start + offset) & (PRINT_BUFFER_INIT_LEN_KB - 1);
		ringBuffer.buf[pos] = str[offset];
	}

	/* shift head pointer */
	ringBuffer.head = ((ringBuffer.head + str_len) & (PRINT_BUFFER_INIT_LEN_KB - 1));

	if(ringBufferDbg == ENABLE) {
		buf_space = get_ring_buffer_space();
		snprintf(msg, sizeof(msg), "after add(%d) buffer space:%d\n", str_len, buf_space);
		console_put_string(msg);
	}
}

/******************************************************************************
 Descriptor:	It's used to print string to console(UART1).
 Input Args:	str: The string pointer.
 Ret Value:	none.
******************************************************************************/
void console_put_string(const char *str)
{
	while(*str) {
		if (*str == '\n')
			tc3162_console_put('\r');
		tc3162_console_put(*str);
		str++;
	}
}

/******************************************************************************
 Descriptor:	It's used to flush data from ring buffer to console(UART1).
 Input Args:	none.
 Ret Value:	SUCCESS: flush ring buffer to console success.
                      FALSE: flush ring buffer to console failed.
******************************************************************************/
static int flush_ring_buffer(void)
{
	int offset, start, pos;
	int ring_buffer_cnt;
	int buf_space = 0;
	char msg[64] = {0};
	unsigned long flags;

	if(ringBufferDbg == ENABLE) {
		buf_space = get_ring_buffer_space();
		snprintf(msg, sizeof(msg), "before flush buffer space:%d\n", buf_space);
		console_put_string(msg);
	}

	if(isEN7516G) {
		local_irq_save(flags);
	}

	ring_buffer_cnt = get_ring_buffer_cnt();

	if(ring_buffer_cnt == 0) {
		if(isEN7516G) {
			local_irq_restore(flags);
		}
		return SUCCESS;
	} else if(ring_buffer_cnt < 0) {
		console_put_string("ring buffer count error.\n");
		if(isEN7516G) {
			local_irq_restore(flags);
		}
		return FALSE;
	}

	start = ringBuffer.tail;
	
	for(offset = 0; offset < ring_buffer_cnt; offset++) {
		/* pos = (start + offset) % PRINT_BUFFER_INIT_LEN_KB */
		pos = (start + offset) & (PRINT_BUFFER_INIT_LEN_KB - 1);
		if (ringBuffer.buf[pos] == '\n')
			tc3162_console_put('\r');
		tc3162_console_put(ringBuffer.buf[pos]);

		
		/* shift tail pointer */
		ringBuffer.tail = ((ringBuffer.tail + 1) & (PRINT_BUFFER_INIT_LEN_KB - 1));
	}
			
	if(isEN7516G) {
		local_irq_restore(flags);
	}

	if(ringBufferDbg == ENABLE) {
		buf_space = get_ring_buffer_space();
		snprintf(msg, sizeof(msg), "after flush buffer space:%d\n", buf_space);
		console_put_string(msg);
	}

	return SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to flush data from ring buffer to console(UART1).
 Input Args:	data: Never used.
 Ret Value:	    0: Success.
******************************************************************************/
static int uart_print_task_wait(void *data)
{
	while(!kthread_should_stop()) {	
		wait_event_interruptible(printQueue, (printFlag == ENABLE || kthread_should_stop()));
		/* clear condition */
		printFlag = DISABLE;

		/* print debug msg */
		if(flush_ring_buffer() == FALSE) {
			/* reset head and tail pointer */
			ringBuffer.head = 0;
			ringBuffer.tail = 0;
		}
	}

	return 0;
}

/******************************************************************************
 Descriptor:	It's used to insert string to ring buffer.
 Input Args:	data: Never used.
 Ret Value:	    0: Success.
******************************************************************************/
int uart_print_task_wakeup(const char *str, unsigned int str_len)
{
	/* store log message to buffer */
	add_msg_to_ring_buffer(str, str_len);

	if(isRingBufferMode == DISABLE) {
		/* Disable is called, flush buffer and set real_state to DISABLE */
		flush_ring_buffer();
		real_state = DISABLE;
		/* reset head and tail pointer */
		ringBuffer.head = 0;
		ringBuffer.tail = 0;
	} else {
		/* wake up task */
		printFlag = ENABLE;
		wake_up_interruptible(&printQueue);
	}
	return 0;
}

/******************************************************************************
 Descriptor:	It's used to enable or disable ring buffer mode with flush
                ring buffer.
 Input Args:	enable: Enable or disable ring buffer mode.
 			flush_buffer: Enable or disable flush ring buffer when disable
 				              ring buffer mode.
 Ret Value:	none.
******************************************************************************/
void set_ring_buffer_mode(int enable)
{
	if(enable == DISABLE) {
		if(isRingBufferMode == ENABLE) {
			isRingBufferMode = DISABLE;
		}
	} else {
		if(isRingBufferMode == DISABLE) {
			isRingBufferMode = ENABLE;
			real_state = ENABLE;
		}
	}
}

/******************************************************************************
 Descriptor:	It's used to set ring buffer mode in auto mode.
 Input Args:	enalbe : enable/disable ring buffer mode.
 Ret Value:	none.
******************************************************************************/
void set_ring_buffer_mode_auto(int enable)
{
	/* Set ring buffer mode automatically if auto mode is enable (default enable) */
	if(auto_mode == ENABLE) {
		if(enable == DISABLE) {
			set_ring_buffer_mode(DISABLE);
		} else {
			set_ring_buffer_mode(ENABLE);
		}
	}
}
EXPORT_SYMBOL(set_ring_buffer_mode_auto);

/******************************************************************************
 Descriptor:	It's used to test ring buffer mode. This function will printk a message with random
                 length.
                 Tester uses console information to check whether the
                 ring buffer mode correct or not.
 Input Args:	times: The times of printk.
 Ret Value:	none.
******************************************************************************/
static void random_printk(int times)
{
	/* printk buffer size are 1024  */
#define PRINTK_SIZE (1024)	
#if GCC_VERSION >= 40600
	char msg_pattern[1023] = {0};
	char msg_printk[1023] = {0};
#else
	char msg_pattern[PRINTK_SIZE - 1] = {0};
	char msg_printk[PRINTK_SIZE - 1] = {0};
#endif
	int printk_len;
	char msg_printk_len[32] = {0};
	const int ascii_start = 0x21;
	const int ascii_end = 0x7E;
	const int ascii_range = ascii_end - ascii_start + 1;
	int i;

	if(isRingBufferMode == DISABLE) {
		console_put_string("ring buffer mode is disabled.");
		return;
	}

	/* init msg */
	for(i = 0; i < sizeof(msg_pattern); i++) {
		/* assign ASCII from 0x21(!) to 0x7E(~) */
		msg_pattern[i] = (i % ascii_range) + ascii_start;
	}

	while(times--) {
		get_random_bytes(&printk_len, sizeof(printk_len)) ;
		/* minus '\n' length */
		printk_len = (printk_len & (PRINTK_SIZE - 1 - strlen("\n")));

		/* show to console for check printk length */
		/* plus "\r\n" length */
		snprintf(msg_printk_len, sizeof(msg_printk_len), "times:%d printk len:%d\n", (times + 1), printk_len + strlen("\r\n"));
		console_put_string(msg_printk_len);

		/* print to console */
		memcpy(msg_printk, msg_pattern, printk_len);
		msg_printk[printk_len] = '\n';
		msg_printk[printk_len + 1] = '\0';
		printk(msg_printk);

		/* When UART baudrate are 115200,
		 * the costs of printk to console is about 1ms with 12 characters.
		 */
		msleep((PRINTK_SIZE / 12) + 1);
	}
}

static int uart_dbg_msg_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;

	index += sprintf(buf + index, "isRingBufferMode     : %s\n", ((isRingBufferMode == ENABLE) ? "enable" : "disable"));
	index += sprintf(buf + index, "ring buffer mode size: %dKB\n", (PRINT_BUFFER_INIT_LEN_KB >> 10));
	index += sprintf(buf + index, "ringBufferDbg        : %s\n", ((ringBufferDbg == ENABLE) ? "enable" : "disable"));
	index += sprintf(buf + index, "auto mode            : %s\n", ((auto_mode == ENABLE) ? "enable" : "disable"));

	index += sprintf(buf + index, "\nusage:\n");
	index += sprintf(buf + index, "ring_buffer_mode <0:disable, 1:enable>\n");
	index += sprintf(buf + index, "busy_loop_test\n");
	index += sprintf(buf + index, "crash_test\n");
	index += sprintf(buf + index, "ring_buffer_dbg <0:disable, 1:enable>\n");
	index += sprintf(buf + index, "ring_buffer_test <times>\n");
	index += sprintf(buf + index, "ring_buffer_stop_test\n");

	*eof = 1;

	return index;
}

static int uart_dbg_msg_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64], cmd[32];
	int value, ret ;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %d", cmd, &value) ;

	if(!strcmp(cmd, "ring_buffer_mode")) {
		/* Disable auto mode once ring buffer is enable/disable manually (command line) */
		auto_mode = DISABLE;
		if(value == 0) {
			set_ring_buffer_mode(DISABLE);
		} else {
			set_ring_buffer_mode(ENABLE);
		}
		printk("isRingBufferMode:%s\n", ((isRingBufferMode == ENABLE) ? "enable" : "disable"));
	} else if(!strcmp(cmd, "busy_loop_test")) {
		/* use for test printk after occuring watchdog interrupt */
		printk("before busy loop test.\n");
		while(1);
	} else if(!strcmp(cmd, "crash_test")) {
		printk("before crash test.\n");
		/* use for test printk when read error register crash */
		UART_RDL(0x1234567);
		printk("finished crash test.\n");
	} else if(!strcmp(cmd, "ring_buffer_dbg")) {
		if(value == 0) {
			ringBufferDbg = DISABLE;
		} else {
			ringBufferDbg = ENABLE;
		}
		printk("ringBufferDbg:%s\n", ((ringBufferDbg == ENABLE) ? "enable" : "disable"));
	} else if(!strcmp(cmd, "ring_buffer_test")) {
		random_printk(value);
	} else if(!strcmp(cmd, "ring_buffer_stop_test")) {
		printk("before ring buffer stop.\n");
		set_ring_buffer_mode(DISABLE);
		printk("finished ring buffer stop.\n");
#ifdef TCSUPPORT_UART2
	} else if(!strcmp(cmd, "uart2_loopback_test")){
		uart2_loopback_test();
#endif
	}
	
	return count ;
}

#if defined(TCSUPPORT_UART_DISABLE)
static int uart_disable_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;

#if defined(TCSUPPORT_UART_RX_DISABLE)
	index += sprintf(buf + index, "rx disable : %d\n", isUartRxDis);
#endif
#if defined(TCSUPPORT_UART_TX_DISABLE)
	index += sprintf(buf + index, "tx disable : %d\n", isUartTxDis);
#endif
	
	*eof = 1;

	return index;
}

static int uart_disable_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64], cmd[32];
	int value, ret ;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %d", cmd, &value) ;

#if defined(TCSUPPORT_UART_RX_DISABLE)
	if(!strcmp(cmd, "rxdis")) {
		isUartRxDis = value;
	} 
#endif
#if defined(TCSUPPORT_UART_TX_DISABLE)
	if(!strcmp(cmd, "txdis")) {
		isUartTxDis = value;
		/* Set NPU tx uart */
		set_npu_uart_tx_disable(isUartTxDis);
	}
#endif

	return count ;
}
#endif

/******************************************************************************
 Descriptor:	It's used to deinit ring buffer.
 Input Args:	none.
 Ret Value:	none.
******************************************************************************/
void uart_print_buffer_deinit(void)
{
	remove_proc_entry("uart/uart", 0);
	remove_proc_entry("uart", 0);

	if(printTask) {
		kthread_stop(printTask);
	}

	if(ringBuffer.buf) {
		kfree(ringBuffer.buf);
	}
}
#endif

/******************************************************************************
 Descriptor:	It's used to disable ring buffer mode with flush ring buffer.
 Input Args:	none.
 Ret Value:	none.
******************************************************************************/
void disable_ring_buffer_mode(int *level)
{
#ifdef TCSUPPORT_UART1_ENHANCE
	int msg_level = 0;
	if(level != NULL) {
		msg_level = level;
		if(msg_level >= 0 && msg_level <= CRITICAL_CONDITION_LOGLEVEL) {
			set_ring_buffer_mode(DISABLE);
		}
	}
#endif
}
EXPORT_SYMBOL(disable_ring_buffer_mode);

static int __init tc3162ser_init(void)
{
	int ret, i;
#ifdef TCSUPPORT_UART1_ENHANCE
	struct proc_dir_entry *uart_proc = NULL;
	struct proc_dir_entry *uart_proc_dir = NULL;
	int errno;
	char errMsg[64] = {0};
#endif

	ret = uart_register_driver(&tc3162ser_reg);
	if (!ret) {
		for (i = 0; i < TC3162_NR_PORTS; i++)
			uart_add_one_port(&tc3162ser_reg, &tc3162ser_ports[i]);
	}

#ifdef TCSUPPORT_UART1_ENHANCE
	/* init */
	init_waitqueue_head(&printQueue);
	printFlag = DISABLE;
	ringBuffer.head = 0;
	ringBuffer.tail = 0;
	
	/* Initial proc file node */
	uart_proc_dir = proc_mkdir("uart", NULL);

	if(uart_proc_dir) {
		uart_proc = create_proc_entry("uart", 0, uart_proc_dir) ;
		if(uart_proc) {
			uart_proc->read_proc = uart_dbg_msg_read_proc ;
			uart_proc->write_proc = uart_dbg_msg_write_proc ;
		}
	}

	if(isEN7516G)
		printTask = kthread_create(uart_print_task_wait, NULL, "uart_print_task");
	else
		printTask = kthread_run(uart_print_task_wait, NULL, "uart_print_task");

	if(IS_ERR(printTask)) {
		errno = PTR_ERR(printTask);
		printTask = NULL;
		snprintf(errMsg, sizeof(errMsg), "uart create kernel thread for ring buffer error:%d.\n", errno);
		console_put_string(errMsg);
		remove_proc_entry("uart/uart", 0);
		remove_proc_entry("uart", 0);
	} else {
		if(isEN7516G) {
			kthread_bind(printTask, 0);
			wake_up_process(printTask);
		} 
		ringBuffer.buf = buffer;
		console_put_string("ECNT ring buffer init success\n");
	}
#endif

#if defined(TCSUPPORT_UART_DISABLE)
#if defined(TCSUPPORT_UART_TX_DISABLE)
	isUartTxDis = 1;
#endif
#if defined(TCSUPPORT_UART_RX_DISABLE)
	isUartRxDis = 1;
#endif
	uart_proc = create_proc_entry("uart/disable", 0, NULL) ;
	if(uart_proc) {
		uart_proc->read_proc = uart_disable_read_proc ;
		uart_proc->write_proc = uart_disable_write_proc ;
	}
#endif

	return ret;
}

#ifdef CONFIG_TC3162_ADSL
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

		if (tty->port->low_latency)
			//tty_flip_buffer_push(tty);
			tty_flip_buffer_push(&port->state->port);

		flg = TTY_NORMAL;

		//tty_insert_flip_char(tty, ch, flg);
		tty_insert_flip_char(&port->state->port, ch, flg);
	}
	//tty_flip_buffer_push(tty);
	tty_flip_buffer_push(&port->state->port);
}/*end tcconsole_cmd*/
EXPORT_SYMBOL(tcconsole_cmd);
#endif 

__initcall(tc3162ser_init);

#ifdef TCSUPPORT_CPU_ARMV8
static void ecnt_uart_write(struct uart_port *port, const char c)
{
	while (!(UART_RDL(CR_UART_LSR) & LSR_THRE));
	UART_WRL(CR_UART_THR, c);
}

static void __init ecnt_earlycon_write(struct console *console,
					const char *s, unsigned int count)
{
#if defined(TCSUPPORT_UART_DISABLE) && defined(TCSUPPORT_UART_TX_DISABLE)
	/* Disable early console when enable TCSUPPORT_UART_DISABLE and TCSUPPORT_UART_TX_DISABLE. */
	return;
#else
	struct earlycon_device *device = console->data;
	struct uart_port *port = &device->port;

	uart_console_write(port, s, count, ecnt_uart_write);
#endif
}

static int __init ecnt_earlycon_setup(struct earlycon_device *device,
					const char *options)
{
	if (!device->port.membase)
		return -ENODEV;

	ecnt_uart1.base = device->port.membase;

	device->con->write = ecnt_earlycon_write;

	return 0;
}
OF_EARLYCON_DECLARE(ecnt, "econet,ecnt-uart1", ecnt_earlycon_setup);

static int ecnt_uart1_drv_shutdown(struct platform_device *pdev)
{
	unregister_console(&tc3162_serconsole);
	uart_unregister_driver(&tc3162ser_reg);
	UART_WRL(CR_UART_IER, (UART_RDL(CR_UART_IER) & ~IER_RECEIVED_DATA_INTERRUPT_ENABLE));
	return 0;
}

static int ecnt_uart1_drv_probe(struct platform_device *pdev)
{
	struct resource *res = NULL;
	int ret = 0;
	int irq = -1;

	platform_set_drvdata(pdev, &ecnt_uart1);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	ecnt_uart1.base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ecnt_uart1.base)) {
		return PTR_ERR(ecnt_uart1.base);
	}

	ecnt_uart1.dev = &pdev->dev;

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
		return irq;
	ecnt_uart1.irq = irq;
	tc3162ser_ports[0].irq = irq;
	
#ifdef TCSUPPORT_DYING_GASP
	dying_gasp_handler_register("uart", (void *)ecnt_uart1_drv_shutdown, 0);
#endif

	return ret;
}

static int ecnt_uart1_drv_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver ecnt_uart_driver = {
	.probe = ecnt_uart1_drv_probe,
	.remove = ecnt_uart1_drv_remove,
	.driver = {
		.name = "ecnt-uart1",
		//.pm = MTK_NOR_DEV_PM_OPS,
		.of_match_table = ecnt_uart1_of_ids,
	},
};
module_platform_driver(ecnt_uart_driver);
MODULE_DESCRIPTION("EcoNet UART Driver");
#endif
