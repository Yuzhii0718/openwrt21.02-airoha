/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/tc3162/tc3162.h>
#include <linux/version.h> 
#include <linux/kthread.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19) 
#include "qdma_bmgr.h"
#include "qdma_api.h"
#else
#include "qdma_api.h"
#include "qdma_bmgr.h"
#endif
#include "qdma_dev.h"
#include "qdma_dvt.h"
#include <ecnt_hook/ecnt_hook_fe.h>
#include "qdma_ic_dis.h"


#ifdef CONFIG_SUPPORT_SELF_TEST

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define DBG_CNTR_LOOP_NUM 3
#define PACKET_NUM 2000  

#ifdef QDMA_LAN
#define CSR3_ROUND_CNT	100
#else
#define CSR3_ROUND_CNT	500
#endif

#if defined(TCSUPPORT_CPU_ARMV8)
#define TIMER1_LDV_GET	(get_ldv(1))
#define TIMER1_VLR_GET	(get_vlr(1))
#else
#define TIMER1_LDV_GET	(VPint(CR_TIMER1_LDV))
#define TIMER1_VLR_GET	(VPint(CR_TIMER1_VLR))
#endif

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
int (*bmRecvCallbackFunction)(void *, uint, struct sk_buff *, uint) ;
int (*bmEventCallbackFunction)(void *, uint, struct sk_buff *, uint) ;

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern const uint32 CONFIG_TX_DSCP_NUM[];
extern const uint32 CONFIG_RX_DSCP_NUM[];

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int qdma_dvt_netdev_open(struct net_device *dev);
static int qdma_dvt_netdev_close(struct net_device *dev);
static int qdma_dvt_netdev_set_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
static int qdma_dvt_netdev_start(struct net_device *dev);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
uint selfTestFlag=0;
uint umacTestFlag=0;
uint loopCnt=0;
uint delayTime=0, nlen=0;
uint msec=0, aveTime=0;
QDMA_DbgCounters_T counters[CONFIG_QDMA_CHANNEL][CONFIG_QDMA_QUEUE] ;
QDMA_DbgCounters_T chain_counter[2];
uint rxCounter[RX_RING_NUM];
struct net_device *en7512NapiDev = NULL;
uint8 defMacAddr[] = {0x00, 0x00, 0xaa, 0xbb, 0xcc, 0xff};

#if defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_MT7520)
int pon_loopback_result = 0;
EXPORT_SYMBOL(pon_loopback_result);
#endif


/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static DEFINE_SPINLOCK(napiLock);

static const struct net_device_ops qdmaDvt_netdev_ops = {
	.ndo_init               = qdma_dvt_netdev_start,
	.ndo_open               = qdma_dvt_netdev_open,
	.ndo_stop               = qdma_dvt_netdev_close,
	.ndo_do_ioctl           = qdma_dvt_netdev_set_ioctl,
	.ndo_start_xmit         = NULL,
	.ndo_change_mtu         = NULL,
	.ndo_validate_addr      = NULL,
};


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/******************************************************************************
******************************************************************************/

static int dumpMemory(unsigned long addr, unsigned long len)
{
	register int n, m, c, r;
	unsigned char temp[16];

	for( n = len; n > 0; ){
			printk("%.8lx ", addr);
			r = n < 16? n: 16;
			memcpy((void *) temp, (void *) addr, r);
			addr += r;
			for( m = 0; m < r; ++m ){
					printk("%c", (m & 3) == 0 && m > 0? '.': ' ');
					printk("%.2x", temp[m]);
			}
			for(; m < 16; ++m )
					printk("   ");
			printk("  |");
			for( m = 0; m < r; ++m ){
				c = temp[m];
				printk("%c", ' ' <= c && c <= '~'? c: '.');
			}
			n -= r;
			for(; m < 16; ++m )
					printk(" ");
			printk("|\n");
	}

	return 0;
}

static void __st_dump_skb(struct sk_buff *skb) 
{
	unchar n=0, *p = (unchar *)skb->data ;
	int i=0 ;

	for(i=0 ; i<skb->len ; i++) {
		n = i & 0x0f ;
		
		if(n == 0x00) 		printk("%.4x: ", i) ;
		else if(n == 0x08) 	printk(" ") ;

		printk("%.2x ", *p++) ; 
		
		if(n == 0x0F)	printk("\n") ;
	}
	
	if(n != 0x0F) 	printk("\n") ;
}

/******************************************************************************
******************************************************************************/
static ushort __st_in_csum(unsigned short *ptr, int nbytes)
{
	register int			sum=0;		/* assumes long == 32 bits */
	unsigned short			oddbyte=0;
	register unsigned short	answer=0; 	/* assumes u_short == 16 bits */

	/*
	 * Our algorithm is simple, using a 32-bit accumulator (sum),
	 * we add sequential 16-bit words to it, and at the end, fold back
	 * all the carry bits from the top 16 bits into the lower 16 bits.
	 */

	sum = 0;

	while (nbytes > 1)	{
		sum += *ptr++;
		nbytes -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nbytes == 1) {
		oddbyte = 0;		/* make sure top half is zero */
		*((unsigned char *) &oddbyte) = *(unsigned char *)ptr;   /* one byte only */
		sum += oddbyte;
	}

	/*
	 * Add back carry outs from top 16 bits to low 16 bits.
	 */
	sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
	sum += (sum >> 16); 		/* add carry */
	answer = ~sum;		/* ones-complement, then truncate to 16 bits */
 
	return(answer);
}

/******************************************************************************
******************************************************************************/
static void __st_print_counters(void)
{
	int i=0, j=0 ;
	uint tx=0, rx=0, rx_err=0 ;
	
	msleep(2000) ;
	
	printk(" C   Q     Tx Frames    Rx OK Frames    Rx Err Frames \n") ;
	for(i=0 ; i<CONFIG_QDMA_CHANNEL ; i++) {
		for(j=0 ; j<CONFIG_QDMA_QUEUE ; j++) {
			tx += counters[i][j].tx_frames ;
			rx += counters[i][j].rx_frames ;
			rx_err += counters[i][j].rx_err_frames ;
			
			if(counters[i][j].tx_frames || counters[i][j].rx_frames) {
				printk(" %2d  %d   %12d    %12d    %12d\n", i, j, counters[i][j].tx_frames, 
															counters[i][j].rx_frames, 
															counters[i][j].rx_err_frames) ;
			}
		}
	}
	printk(" Total   %12d    %12d    %12d\n", tx, rx, rx_err) ;
	printk("\n") ;
}

/******************************************************************************
******************************************************************************/
static int __st_check_counters(void)
{
	int i=0, j=0 ;
	
	msleep(1000) ;
	
	for(i=0 ; i<CONFIG_QDMA_CHANNEL ; i++) {
		for(j=0 ; j<CONFIG_QDMA_QUEUE ; j++) {
			if(counters[i][j].rx_err_frames!=0 || counters[i][j].tx_frames!=counters[i][j].rx_frames) {
				return -1 ;
			}
		}
	}

	return 0 ;
}

static int qdma_polling_tx_recycle(void) 
{
	while(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE) {
		qdma_bm_transmit_done(0) ;
		msleep(500) ;
	}
	
	return 0 ;
}


static int qdma_dvt_netdev_open(struct net_device *dev)
{
	QDMA_Adapter_T *qdmaApt = netdev_priv(dev);	
	
	napi_enable(&qdmaApt->napi);

	netif_start_queue(dev);

  	return 0;
}


static int qdma_dvt_netdev_close(struct net_device *dev)
{
	QDMA_Adapter_T *qdmaApt = netdev_priv(dev);
	
	printk("mt7510_netdev_close\n");

	printk("%s: stoping interface.\n", dev->name);

	netif_stop_queue(dev);

	napi_disable(&qdmaApt->napi);

	return 0;
}


static int qdma_dvt_netdev_set_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	/* Not implemented yet */
  	return 0;
}

static int qdma_dvt_netdev_start(struct net_device *dev)
{
#ifdef TCSUPPORT_CPU_ARMV8
	if(get_ethaddr(defMacAddr, sizeof(defMacAddr)) != 0){
		printk("func:%s get ethaddr error!!!\n",__func__);
	}
#else
	uint8 *flashMacAddr = (uint8 *) (0xbc00ff48);

	if (
		flashMacAddr[0] == 0 && flashMacAddr[1] == 0 && flashMacAddr[2] == 0 &&
		flashMacAddr[3] == 0 && flashMacAddr[4] == 0 && flashMacAddr[5] == 0
	)
		printk(KERN_INFO "\nThe MAC address in flash is null!\n");	    
	else    
		memcpy(defMacAddr, flashMacAddr, 6);
#endif

	memcpy(dev->dev_addr, defMacAddr, 6);
	dev->addr_len = 6;

	return 0;
}

void
delay1ms(
	int ms
)
{
	volatile uint32 timer_now=0, timer_last=0;
	volatile uint32 tick_acc=0;
	uint32 one_tick_unit = 1 * SYS_HCLK * 1000 / 2;
	volatile uint32 tick_wait = ms * one_tick_unit;
	volatile uint32 timer1_ldv = TIMER1_LDV_GET;

	tick_acc = 0;
 	timer_last = TIMER1_VLR_GET;
	do {
   		timer_now = TIMER1_VLR_GET;
       	if (timer_last >= timer_now)
       		tick_acc += timer_last - timer_now;
      	else
       		tick_acc += timer1_ldv - timer_now + timer_last;
     	timer_last = timer_now;
	} while (tick_acc < tick_wait);
}

static int qdma_st_cb_EventHandler(QDMA_EventType_t qdmaEventType)
{
	unsigned long flags=0;
	QDMA_Adapter_T *qdmaApt = netdev_priv(en7512NapiDev);
	struct ECNT_QDMA_Data qdma_data ;

	if ((qdmaEventType == QDMA_EVENT_RECV_PKTS) ||
        (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER))
	{
        if (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER)
            QDMA_MSG(DBG_WARN, "QDMA_EVENT_NO_RX_BUFFER\n");
    
		spin_lock_irqsave(&napiLock, flags);
		QDMA_MSG(DBG_MSG, "qdma_st_cb_EventHandler come in, qdmaEventType is %s\n"
			, ((qdmaEventType == QDMA_EVENT_RECV_PKTS)?"QDMA_EVENT_RECV_PKTS":"QDMA_EVENT_NO_RX_BUFFER"));

		/* because no dev info available, we always
		 * use nas8's poll function. */
		if (napi_schedule_prep(&qdmaApt->napi))
		{
			QDMA_MSG(DBG_MSG, "napi_schedule_prep come in\n");
			qdma_disable_rxpkt_int(&qdma_data); /* disable RX interrupt. */
			__napi_schedule(&qdmaApt->napi);
		}
		
		spin_unlock_irqrestore(&napiLock, flags);
	}
	else if (qdmaEventType == QDMA_EVENT_TX_CROWDED)
	{
		QDMA_MSG(DBG_WARN, "QDMA_EVENT_TX_CROWDED\n");
	}
	else
		QDMA_MSG(DBG_WARN, "Wrong QDMA Event Type: %d\n", qdmaEventType);

	return 0;
}

static int mt7510_qdma_napiPoll(struct napi_struct *napi, int budget)
{
	int n=0;
	unsigned long flags=0;
	struct ECNT_QDMA_Data qdma_data ;

	/* call mt7510_ptm_rx to receive a packet 
	 * until all packets are received */
	qdma_data.qdma_private.cnt = budget ;
	n = qdma_receive_packets(&qdma_data);
	QDMA_MSG(DBG_MSG, "mt7510_qdma_napiPoll come in, n is %d, budget is %d\n", n, budget);

	if (n < budget)
	{
		spin_lock_irqsave(&napiLock, flags);
	
		__napi_complete(napi);
		qdma_enable_rxpkt_int(&qdma_data);

		spin_unlock_irqrestore(&napiLock, flags);
	}
	
	return n;
}

/******************************************************************************
******************************************************************************/
static int qdma_st_packet_gen(uint channel, uint queue, uint npackets, uint lens, unchar pattern, int ringIdx)
{
	int i=0, k=0;
	struct sk_buff *skb=NULL;
	int tx_len=0;
	unchar *tx_data=NULL;
	unchar tx_seed=0;
	int offset=0;
	ushort chksum=0;
	QDMA_TxMsg_T pTxMsg ;
	int times=0 ;
	struct ECNT_QDMA_Data qdma_data;
	int maxLen=0;

	if(umacTestFlag == 1)
		channel = channel % CONFIG_QDMA_RX_CHANNEL;

	QDMA_MSG(DBG_ST, "Loopback test packets=%d, lens=%d, jiffies:%d, channel:%d, queue:%d\n", npackets, lens, (uint)jiffies, channel, queue);

	for (i=0 ; i<npackets ; i++) {
		times = 0 ;
		memset(pTxMsg.msg, 0, sizeof(QDMA_TxMsg_T)) ;
#if defined(TCSUPPORT_AUTOBENCH)
		pTxMsg.raw.channel =0;
		pTxMsg.raw.queue = 0;
#else
		pTxMsg.raw.channel = (channel == CONFIG_QDMA_CHANNEL)?(((unsigned int)random32()) % CONFIG_QDMA_CHANNEL):channel;
		pTxMsg.raw.queue = (queue == CONFIG_QDMA_QUEUE)?(((unsigned int)random32()) % CONFIG_QDMA_QUEUE):queue;
#endif
#ifdef QDMA_LAN
		pTxMsg.raw.fport = 0x1 ;
#else
		pTxMsg.raw.fport = 0x2 ;
#endif
        pTxMsg.raw.mtr_g = 0x7f ;
        pTxMsg.raw.acnt_g1 = 0x1f ;
        pTxMsg.raw.acnt_g0 = 0x1f ;

		if(loopCnt != 0) {
            pTxMsg.raw.loopcnt= loopCnt & 0xFF;
        }
		
		skb = dev_alloc_skb(CONFIG_LOOPBACK_MAX_PKT_LENS+2);
		if (skb == NULL) {			
			QDMA_MSG(DBG_ST, "skb == NULL, continue;\n") ;
			continue;
		}
		skb->dropcount = 0xdeadbeef;

		if (lens == 0) {
			tx_len = ((unsigned int)random32()) % (CONFIG_LOOPBACK_MAX_PKT_LENS+1);
		} else if(lens == 1) {
			tx_len = (48+i) % (CONFIG_LOOPBACK_MAX_PKT_LENS+1);
		} else {
			tx_len = lens % (CONFIG_LOOPBACK_MAX_PKT_LENS+1);
		}
		if(tx_len > maxLen)
			maxLen = tx_len;
#if defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_MT7520)
		tx_len = (tx_len<64)?64:tx_len ;
#else
		tx_len = (tx_len<48)?48:tx_len ;
#endif
		tx_data = skb_put(skb, tx_len);
		for (k = 0; k < 6; k++)
			tx_data[k] = 0x11;
		for (k = 6; k < 12; k++)
			tx_data[k] = 0x22;

		tx_data[12] = 0x08;
		tx_data[13] = 0x00;

		tx_data[14] = pTxMsg.raw.channel;
		tx_data[15] = pTxMsg.raw.queue;
		
		tx_data[16] = (tx_len)>>8;
		tx_data[17] = (tx_len);

		tx_data[18] = (i+1)>>24;
		tx_data[19] = (i+1)>>16;
		tx_data[20] = (i+1)>>8;
		tx_data[21] = (i+1);

		tx_data[22] = 0;
		tx_data[23] = 0;

		if(pattern == 0x01) {
			tx_data[24] = 0x01 ;	
			tx_seed = (unchar) ((unsigned int)random32()) ;
			tx_data[25] = tx_seed;
            tx_data[26] = 0xAA;
            tx_data[27] = 0xBB;
            tx_data[28] = ringIdx;

			for(k=29 ; k<tx_len ; k++) {
				tx_seed++;
				tx_data[k] = (unchar) (tx_seed & 0xff);
			}
		} else {
			tx_data[24] = 0x00;
			tx_data[25] = pattern;

			for(k=26 ; k<tx_len ; k++) {
				tx_data[k] = pattern;
			}
		}

		chksum = __st_in_csum((unsigned short *) (skb->data), tx_len-4);
#ifdef __BIG_ENDIAN
        tx_data[22] = (chksum >> 8) & 0xff;
        tx_data[23] = chksum & 0xff;
#else
		tx_data[22] = chksum & 0xff;
		tx_data[23] = (chksum >> 8) & 0xff;
#endif

        /*use data[28] transmit tx ringIdx to QDMA tx API*/
		tx_data[28] = ringIdx;

		do {
			qdma_data.qdma_private.qdma_transmit.pTxMsg->txmsg0 = pTxMsg.msg[0];
			qdma_data.qdma_private.qdma_transmit.pTxMsg->txmsg1 = pTxMsg.msg[1];
			qdma_data.qdma_private.qdma_transmit.skb = skb;
			qdma_data.qdma_private.qdma_transmit.pMacInfo = NULL;
			if(qdma_transmit_packet(&qdma_data) == 0) {
				break;
			}
		} while(times--);
		if(times < 0) {
			dev_kfree_skb_any(skb);
		}
		if(delayTime) {
			udelay(delayTime);
		}
	}
	QDMA_MSG(DBG_ST, "TX PKT: there are %d packets has been sent.\n", i);

	return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_st_cb_rx_packet(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len)
{
	int k=0, len=0;
	unchar *rx_data=NULL;
	unchar rx_seed=0, pattern=0;
	int idx, channel=0, queue=0, lens=0 ;
    int ring_index=0;
	ushort chksum=0;
	volatile uint timer_now=0, timer_last=0, passtime=0;

	skb_put(skb, rx_len) ;
	rx_data = skb->data;
	if(rx_len > nlen)
		QDMA_MSG(DBG_ST, "rx_len error: rx_len is %d\n", rx_len);
		
	for (k = 0; k < 6; k++) {
		if (rx_data[k] != 0x11) {
			QDMA_ERR("loopback fail: dst mac unmatch\n");
			goto err;
		}
	}
	
	for (k = 6; k < 12; k++) {
		if (rx_data[k] != 0x22) {
			QDMA_ERR("loopback fail: src mac unmatch\n");
			goto err;
		}
	}
	
	if ((rx_data[12] != 0x08) || (rx_data[13] != 0x00)) {
		QDMA_ERR("loopback fail: ether type unmatch\n");
		goto err;
	}
	
	channel = rx_data[14] % CONFIG_QDMA_CHANNEL;
	queue = rx_data[15] % CONFIG_QDMA_QUEUE;
	lens = (rx_data[16]<<8) | rx_data[17];
	idx = (rx_data[18]<<24) | (rx_data[19]<<16) | (rx_data[20]<<8) | rx_data[21];
	chksum = (rx_data[22]<<8) | rx_data[23];
	if(msec) {
		delay1ms(msec);
	} else {
	 	timer_last = TIMER1_VLR_GET;
		
		if(rx_data[24] == 0x01) {
			rx_seed = rx_data[25];
			len = rx_len;
            if((rx_data[26] == 0xAA) && (rx_data[27] == 0xBB)){
                ring_index = rx_data[28];
                rxCounter[ring_index]++;
            }
			for(k=29 ; k<len ; k++) {
				rx_seed++;
				if (rx_data[k] != (unchar) (rx_seed & 0xff)) {
					QDMA_ERR("loopback fail: random payload unmatch pos=%04x\n", k);
					goto err;
				}
			}
		}
		else {
			pattern = rx_data[25] ;
			for(k=26 ; k<rx_len ; k++) {
				if (rx_data[k] != pattern) {
					QDMA_ERR("loopback fail: payload unmatch pos=%04x, pattern=%.2x\n", k, pattern);
					goto err;
				} 
			}		
		}
		
   		timer_now = TIMER1_VLR_GET;
       	if (timer_last >= timer_now)
       		passtime = timer_last - timer_now;
      	else
       		passtime = TIMER1_LDV_GET - timer_now + timer_last;
		aveTime += passtime;
		if(passtime > (TIMER1_LDV_GET/5))
			QDMA_MSG(DBG_ST, "too long passtime is 0x%08x\n", passtime);
	}

	if (__st_in_csum((unsigned short *) (skb->data), lens-4) != 0) {
		QDMA_ERR("loopback fail: ip checksum unmatch\n");
		goto err;
	}
	counters[channel][queue].rx_frames++ ;
	
	if((idx%10000) == 0) {
		QDMA_MSG(DBG_ST, "RX PKT: the %d packet has been received. jiffies:%d\n", idx, (uint)jiffies);
	}
	dev_kfree_skb_any(skb);
	return 0;
	
err:
	printk("Raw data for receive packet, skb->len:%d, rxDscp.ctrl.pkt_len:%d, lens:%d\n", skb->len, rx_len, lens) ;
	counters[channel][queue].rx_err_frames++ ;
	__st_dump_skb(skb);
	dev_kfree_skb_any(skb);
	return -1;
}


/*****************************************************************************
******************************************************************************/
#if 0
static int qdma_st_cb_tx_finished(void *msg_p)
{
	QDMA_TxMsg_T *pTxMsg = (QDMA_TxMsg_T *)msg_p ;
	uint channel=0, queue=0 ;
	
	if (pTxMsg == NULL)
		QDMA_ERR("unexpected error: pTxMsg is NULL, pTxMsg is 0x%08x\r\n", (uint)pTxMsg);
	
	channel = pTxMsg->raw.channel ;
	queue = pTxMsg->raw.queue ;
	counters[channel][queue].tx_frames++;
	
	return 0 ;
}
#endif

#ifndef CONFIG_TX_POLLING_BY_MAC
/******************************************************************************
******************************************************************************/
void qdma_st_rx_polling(void) 
{
	struct ECNT_QDMA_Data qdma_data ;

	while(gpQdmaPriv->devCfg.flags.isRxPolling == QDMA_ENABLE) {
		qdma_data.qdma_private.cnt = 128 ;
		qdma_receive_packets(&qdma_data) ;
		msleep(((unsigned int)random32())%10) ;
	}

}
#endif /* CONFIG_TX_POLLING_BY_MAC */

/****************************************************************************
 to verify read/write int mask register
*****************************************************************************/
static void qdma_st_csr_intmask(uint times)
{
	uint value = 0xA215A ;
	uint reg = 0 ;
	uint i = 0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	
	while(times--) {
		iowrite32(value, (void __iomem *)(QDMA_CSR_INT_EN(base, QDMA_INT1, QDMA_INT_ENABLE1))) ;
		
		reg = ioread32((void __iomem *)(QDMA_CSR_INT_EN(base, QDMA_INT1, QDMA_INT_ENABLE1))) ;
		if(reg != value) {
			printk("========>Get the INT_ENABLE register error: Write:%.8x, Read:%.8x\n", value, reg) ;
			i++ ;
		}
	}
	
	if(i) {
		printk("Failed to verify the INT_ENABLE register read/write, %d\n", i) ;
	} else {
		printk("Successful to verify the INT_ENABLE register read/write\n") ;
	}
	
	return ;	
}

/******************************************************************************
******************************************************************************/
static int qdma_st_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0, j = 0;
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

	index += sprintf(buf+index, " C   Q     Tx Frames    Rx OK Frames    Rx Err Frames \n");
	CHK_BUF();
	for(i=0 ; i<CONFIG_QDMA_CHANNEL ; i++) {
		for(j=0 ; j<CONFIG_QDMA_QUEUE ; j++) {
			if(counters[i][j].tx_frames) {
				index += sprintf(buf+index, " %2d  %d   %12d    %12d    %12d\n", i, j, counters[i][j].tx_frames, 
																						counters[i][j].rx_frames, 
																						counters[i][j].rx_err_frames) ;
				CHK_BUF();
			}
		}
	}

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

/******************************************************************************
 Descriptor:	It's used to configure dbg counter, which has 40 groups to set.
 Input Args:		- cntrIdx:
 				- cntrEn:
 				- cntrSrc:
 				- chnlIdx:
 				- queueIdx:
 				- dscpRingIdx:
 				- isChnlAll:
 				- isQueueAll:
 				- isDscpRingAll:
 Ret Value:		0: successful, otherwise failed.
******************************************************************************/
int qdma_set_dbg_cntr_info(QDMA_DBG_CNTR_T *dbgCntrPtr)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	unchar idx = dbgCntrPtr->cntrIdx;
	
	if(dbgCntrPtr->cntrIdx>=CONFIG_MAX_CNTR_NUM) {
		QDMA_ERR("Fault: dbg Cntr counter index shoule between 0 and %d\n", CONFIG_MAX_CNTR_NUM-1) ; 
		return -EINVAL ;
	}
	if(dbgCntrPtr->cntrSrc<0 || dbgCntrPtr->cntrSrc>=QDMA_DBG_CNTR_SRC_ITEMS) {
		QDMA_ERR("Fault: dbg Cntr source shoule between 0 and %d\n", QDMA_DBG_CNTR_SRC_ITEMS-1) ; 
		return -EINVAL ;
	}
	if((dbgCntrPtr->isChnlAll == QDMA_DISABLE) && (dbgCntrPtr->chnlIdx>=CONFIG_QDMA_CHANNEL)) {
		QDMA_ERR("Fault: Channel index shoule between 0 and %d\n", CONFIG_QDMA_CHANNEL-1) ; 
		return -EINVAL ;
	}
	if((dbgCntrPtr->isQueueAll == QDMA_DISABLE) && (dbgCntrPtr->queueIdx>=CONFIG_QDMA_QUEUE)) {
		QDMA_ERR("Fault: Queue index shoule between 0 and %d\n", CONFIG_QDMA_QUEUE-1) ; 
		return -EINVAL ;
	}
	if((dbgCntrPtr->isDscpRingAll == QDMA_DISABLE) && (dbgCntrPtr->dscpRingIdx>=RX_RING_NUM)) {
		QDMA_ERR("Fault: ringIdx shoule be correct\n") ; 
		return -EINVAL ;
	}

	qdmaSetCntrSrc(base, idx, dbgCntrPtr->cntrSrc);
	qdmaSetCntrChnlIdx(base, idx, dbgCntrPtr->chnlIdx);
	qdmaSetCntrQueueIdx(base, idx, dbgCntrPtr->queueIdx);
	qdmaSetCntrDscpIdx(base, idx, dbgCntrPtr->dscpRingIdx);
	
	if(dbgCntrPtr->isChnlAll == QDMA_DISABLE) {
		qdmaSetCntrChnlSingle(base,idx);
	} else {
		qdmaSetCntrChnlAll(base,idx);
	}
	
	if(dbgCntrPtr->isQueueAll == QDMA_DISABLE) {
		qdmaSetCntrQueueSingle(base,idx);
	} else {
		qdmaSetCntrQueueAll(base,idx);
	}
	
	if(dbgCntrPtr->isDscpRingAll == QDMA_DISABLE) {
		qdmaSetCntrDscpSingle(base,idx);
	} else {
		qdmaSetCntrDscpAll(base,idx);
	}
	
	if(dbgCntrPtr->cntrEn == QDMA_DISABLE) {
		qdmaDisableCntrCfg(base, idx);
		qdmaClearCntrCounter(base, idx);
	} else {
		qdmaEnableCntrCfg(base, idx);
	}
	
	return 0 ;
}

int qdma_clear_and_enable_dbg_cntr_info(QDMA_DBG_CNTR_T *dbgCntrPtr)
{
	uint base = gpQdmaPriv->csrBaseAddr ;
	unchar idx = dbgCntrPtr->cntrIdx;
	
	if(dbgCntrPtr->cntrIdx>=CONFIG_MAX_CNTR_NUM) {
		QDMA_ERR("Fault: dbg Cntr counter index shoule between 0 and %d\n", CONFIG_MAX_CNTR_NUM-1) ; 
		return -EINVAL ;
	}
	
	qdmaDisableCntrCfg(base, idx);
	qdmaClearCntrCounter(base, idx);
	dbgCntrPtr->cntrEn = QDMA_ENABLE ;
	qdma_set_dbg_cntr_info(dbgCntrPtr);

	return 0 ;
}

/******************************************************************************
 Description:	Input a self test command and start the QDMA self testing. 
 				This test process is generated packets by QDMA, send to 
 				TXDMA, receive from RXDMA and check the receive contents.
 Proc Command:	"init": 			Init the QDMA TX/RX DSCP and driver
 				"csr1 pkts lens":	Verification for NO_RX_CPU_DSCP_INT and NO_TX_CPU_DSCP_INT
 				"csr2 pkts lens":	Verification for NO_LINK_DSCP interrupt
 				"csr3":				Verification for CHCCK_DONE bit enable/disable
 				"csr4":				Verification for for RX_DONE_INT and RX_DLY_INT
 				"csr5":				Verification for for TX_DONE_INT and TX_DLY_INT
 				"tc1 pkts lens":	Process the test case 1
 				"tc2":				Process the test case 2
 				"tc3 pkts lens":	Process the test case 3
 				"tc4 pkts lens":	Process the test case 4
 				"tc5 pkts lens":	Process the test case 5
 				"tc6 pkts lens":	Process the test case 6
 				"tcall pkts lens":	Process all test case in sequential. (tc6, tc1, tc2~tc5)
******************************************************************************/
static int tc_init(char subcmd[32], uint base)
{
	struct net_device *dev=NULL;
	QDMA_Adapter_T *qdmaApt=NULL;
    uint dbg=0 ;
    QDMA_InitCfg_t initCfg ;
	int error=0;
	int ringIdx=0;
	struct ECNT_QDMA_Data qdma_data;
	
    qdmaSetIrqPtime(base, 0x64);
    msleep(200);
    memset(&initCfg, 0, sizeof(QDMA_InitCfg_t)) ;
    //initCfg.cbXmitFinish = qdma_st_cb_tx_finished ;
    initCfg.cbRecvPkts = qdma_st_cb_rx_packet ;
    initCfg.cbEventHandler = qdma_st_cb_EventHandler;
	qdma_data.qdma_private.pInitCfg = &initCfg;
	qdma_init(&qdma_data) ;
    umacTestFlag = 0;
    if(!strcmp(subcmd, "umac")) {
		qdma_data.qdma_private.lbMode = QDMA_LOOPBACK_UMAC ;
		qdma_loopback_mode(&qdma_data) ;
    	FE_API_SET_DROP_UDP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_TCP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_IP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_CRC_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_RUNT_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_LONG_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	umacTestFlag = 1;
#if 0 // defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_MT7520)
    } else if(!strcmp(subcmd, "eponmac")) {
		qdma_data.qdma_private.lbMode = QDMA_LOOPBACK_DISABLE ;
		qdma_loopback_mode(&qdma_data) ;
    	 /* switch to epon mac */
    	IO_SREG(0xbfb00070,1); 
    	 /* set epon mac loopback */
    	IO_SREG(0xbfb66000,0x46);	
    	/* disable epon mac isr */
    	qdmaDisableInt(base, INT_MASK_EPON_MAC, QDMA_INT2) ;  
    	IO_SREG(0xbfb66008,0);  
    	
    	FE_API_SET_DROP_UDP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_TCP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_IP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    } else if(!strcmp(subcmd, "eponphy")) {
		qdma_data.qdma_private.lbMode = QDMA_LOOPBACK_DISABLE ;
		qdma_loopback_mode(&qdma_data) ;
    	/* switch to epon mac */
    	IO_SREG(0xbfb00070,1);
    	/* set epon phy */
    	IO_SREG(0xbfaf0124,0x7f000000);
    	IO_SREG(0xbfaf0108,0xcd810110);
    	mdelay(1);
    	IO_SREG(0xbfaf0108,0x45810110);
    	IO_SREG(0xbfaf0428,0x1);
    	
    	/* disable epon mac isr */
    	qdmaDisableInt(base, INT_MASK_EPON_MAC, QDMA_INT2) ;  
    	IO_SREG(0xbfb66008,0);  

    	IO_SREG(0xbfb66000,0x43);

    	FE_API_SET_DROP_UDP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_TCP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
    	FE_API_SET_DROP_IP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
#endif
    } else{
		qdma_data.qdma_private.lbMode = QDMA_LOOPBACK_QDMA ;
		qdma_loopback_mode(&qdma_data) ;
    }
	qdma_data.qdma_private.mode = QDMA_ENABLE ;
	qdma_tx_dma_mode(&qdma_data) ;
	qdma_data.qdma_private.mode = QDMA_ENABLE ;
	qdma_rx_dma_mode(&qdma_data) ;
    if(QDMA_TX_MODE == QDMA_TX_POLLING) {
    	kernel_thread((int (*)(void *))qdma_polling_tx_recycle, NULL, 0) ;
    }
    if(QDMA_RX_MODE == QDMA_RX_NAPI) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19)      
        dev = alloc_netdev(sizeof(QDMA_Adapter_T), test_dev, NET_NAME_USER, ether_setup);
#else
    	dev = alloc_netdev(sizeof(QDMA_Adapter_T), test_dev, ether_setup);
#endif
    	if (!dev)
    	{
    		printk("\n\nFAILED: qdma_dvt net_device allocation\n\n");
    		return -ENOMEM;
    	}

    	en7512NapiDev = dev;
    	qdmaApt = netdev_priv(dev);
    	memset(qdmaApt, 0, sizeof(QDMA_Adapter_T));
    	
    	en7512NapiDev->netdev_ops = &qdmaDvt_netdev_ops;
    	qdmaApt->napi.weight = QDMA_NAPI_WEIGHT;
    	netif_napi_add(dev, &qdmaApt->napi, mt7510_qdma_napiPoll, qdmaApt->napi.weight);
    	error = register_netdev(dev);
    	if (error)
    	{
    		printk("\n\nFAILED: qdma_dvt net_device register fail\n\n");
    		free_netdev(dev);
    		return error;
    	}
    	set_bit(__LINK_STATE_START, &en7512NapiDev->state);
    	
    	if (en7512NapiDev->netdev_ops->ndo_open){
    		error = en7512NapiDev->netdev_ops->ndo_open(en7512NapiDev);
    		if (error)
    			clear_bit(__LINK_STATE_START, &en7512NapiDev->state);
    	}
    }
    QDMA_MSG(DBG_ST, "Prepare %d receive packet buffers in QDMA init stage.\n", dbg) ;
    QDMA_MSG(DBG_ST, "CSR info: INFO:%.8x, GLG:%.8x, INT1_MASK:%.8x, INT2_MASK:%.8x\n", 
    														qdmaGetQdmaInfo(base), 
    														qdmaGetGlbCfg(base), 
    														qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1),
    														qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2)) ;
    for(ringIdx=0; ringIdx<TX_RING_NUM; ringIdx++) {
    	QDMA_MSG(DBG_ST, "CSR info_%d: TX_DSCP_BASE:%.8x, TX_CPU_IDX:%d, TX_DMA_IDX:%d\n",
    															ringIdx, 
    															qdmaGetTxDscpBase(base, ringIdx), 
    															qdmaGetTxCpuIdx(base, ringIdx), 
    															qdmaGetTxDmaIdx(base, ringIdx)) ;
    }
    for(ringIdx=0; ringIdx<RX_RING_NUM; ringIdx++) {
    	QDMA_MSG(DBG_ST, "CSR info_%d: RX_DSCP_BASE:%.8x, RX_CPU_IDX:%d, RX_DMA_IDX:%d\n", 
    															ringIdx, 
    															qdmaGetRxDscpBase(base, ringIdx), 
    															qdmaGetRxCpuIdx(base, ringIdx), 
    															qdmaGetRxDmaIdx(base, ringIdx)) ;
    }
	
    return 0;
}

/* CPU  send packet , test ingress ratelimit*/
/* CPU TX 8000 packet , RX 8000 packet , (tx ring size + rx ring size) > 10000*/
static int tc_ratelimit_test( uint base  , uint sleep )
{
	uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
	uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ; 
    uint channel = 0 ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
    uint rx_done_int_cnt = 0 ;
	uint txRingIdx = 0 , rxRingIdx = 0 ;
	uint sleepcnt = 0 ;

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, INT_STATUS_IRQ_INT);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, INT_STATUS_RX_DONE);
	qdmaSetLpbkToRXRing(base, rxRingIdx);/*default: Tx Ring0 -> Rx ring0*/
				
    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
	gpQdmaPriv->counters.intRxDone[rxRingIdx] = 0 ;
	
	printk("start timer: %lu\n" , jiffies);
    qdma_st_packet_gen(channel, queue, 10000000, 128 , 0x5A , txRingIdx) ; /*8000 packets , packet len =128 , total = 10.24Mbit*/

	while( counters[channel][queue].rx_frames < 100000 ) /*wait at least 15s*/
	{
    	msleep(1000) ;
		sleepcnt ++ ;
		if( sleepcnt >= sleep )
			break ;
	}
	
	rx_done_int_cnt = gpQdmaPriv->counters.intRxDone[rxRingIdx] ;
    printk( "Tx OK Pkts:%d, Rx OK Pkts:%d, Rx_DONE_INT:%d\n", 
		counters[channel][queue].tx_frames, counters[channel][queue].rx_frames, rx_done_int_cnt ) ;

	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt1);
	
    return 0;
}

/*7580, NO_RX_DSCP_INT, 7580 delete NO_TX_DSCP_INT*/
#if 1 /* test by txRing & rxRing */
static int tc_csr1(uint base, uint packets, uint lens,int txRingIdx, int rxRingIdx)
{
    uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
	uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ; 
#if defined(TCSUPPORT_CPU_EN7581)
	uint oldInt3 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3) ; 
	uint oldInt4 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4) ; 
	uint oldInt5 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5) ; 
#endif
    uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
    uint ring_idx = 0 ;

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, INT_STATUS_IRQ_INT);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, INT_MASK_NO_RX_CPU_DSCP);
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3, INT_MASK_NO_RX_CPU_DSCP);
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4, 0);
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5, 0);
#endif
	printk("int_enable1 mask:0x%08x\n", qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1));
	printk("int_enable2 mask:0x%08x\n", qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2));
#if defined(TCSUPPORT_CPU_EN7581)
	printk("int_enable3 mask:0x%08x\n", qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3));
	printk("int_enable4 mask:0x%08x\n", qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4));
	printk("int_enable5 mask:0x%08x\n", qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5));
#endif

    //for( ring_idx=0 ; ring_idx<RX_RING_NUM ; ring_idx++ )
    //{
    	printk("TestPath: Tx ring%d , Rx ring%d\n", txRingIdx , rxRingIdx);
    	qdmaSetLpbkToRXRing(base, rxRingIdx);

        memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
        gpQdmaPriv->counters.intNoRxDscp[rxRingIdx] = 0 ;
        gpQdmaPriv->counters.intNoTxDscp[txRingIdx] = 0 ;
        gpQdmaPriv->devCfg.waitTxMaxNums = 0 ;
        //gpQdmaPriv->devCfg.countDown = (((unsigned int)random32())%(gpQdmaPriv->txDscpNum[txRingIdx]-1))+1 ;
        gpQdmaPriv->devCfg.countDown = gpQdmaPriv->rxDscpNum[0] ;
        printk("1st countDown  %d \n",gpQdmaPriv->devCfg.countDown);
        gpQdmaPriv->devCfg.countDownRound = 0 ;
        qdma_st_packet_gen(channel, queue, packets, lens, 0x01, txRingIdx) ;
        msleep(10000) ;

        /* in order to set the TX_CPU_PTR to HW register */
        gpQdmaPriv->devCfg.countDown = 1 ;
        gpQdmaPriv->devCfg.waitTxMaxNums = 1 ;
        qdma_st_packet_gen(channel, queue, 1, lens, 0x01, txRingIdx) ;
    	printk("GK--before_receive: NO_RX_DSCP_INT:%d\n", gpQdmaPriv->counters.intNoRxDscp[rxRingIdx]);

    	msleep(1000) ;
        qdma_bm_receive_packets(0, rxRingIdx) ;
        msleep(1000) ;
        printk("INT_CSR:%.8x %.8x, Packets:%d, NO_TX_DSCP_INT Cnt:%d, NO_RX_DSCP_INT Cnt:%d, CntDownRound:%d\n\n", 
            qdmaGetIntStatus1(base),qdmaGetIntStatus2(base), packets, gpQdmaPriv->counters.intNoTxDscp[txRingIdx], 
            gpQdmaPriv->counters.intNoRxDscp[rxRingIdx], gpQdmaPriv->devCfg.countDownRound) ;
        __st_print_counters() ;

        if((__st_check_counters() == 0) && (gpQdmaPriv->counters.intNoRxDscp[rxRingIdx] > 0)) {
            QDMA_LOG("csr1: Test Result:Pass\n\n") ;
        } else {
            QDMA_LOG("csr1: Test Result:Failed\n\n") ;
        }
    //}

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);
#if defined(TCSUPPORT_CPU_EN7581)	
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3, oldInt3);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4, oldInt4);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5, oldInt5);
#endif

    return 0;
}

#else
static int tc_csr1(uint base, uint packets, uint lens,int txRingIdx, int rxRingIdx)
{
    uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
	uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ; 
    uint channel = random32() % CONFIG_QDMA_CHANNEL ;
    uint queue = random32() % CONFIG_QDMA_QUEUE ;
    uint ring_idx = 0 ;

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, INT_STATUS_IRQ_INT);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, INT_MASK_NO_RX_CPU_DSCP);
	printk("int_enable1 mask:0x%08x\n", qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1));
	printk("int_enable2 mask:0x%08x\n", qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2));

    for( ring_idx=0 ; ring_idx<RX_RING_NUM ; ring_idx++ )
    {
    	printk("TestPath: Tx ring%d , Rx ring%d\n", ring_idx%TX_RING_NUM , ring_idx);
    	qdmaSetLpbkToRXRing(base, ring_idx);

        memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
        gpQdmaPriv->counters.intNoRxDscp[ring_idx] = 0 ;
        gpQdmaPriv->counters.intNoTxDscp[ring_idx%TX_RING_NUM] = 0 ;
        gpQdmaPriv->devCfg.waitTxMaxNums = 0 ;
        gpQdmaPriv->devCfg.countDown = (random32()%(gpQdmaPriv->txDscpNum[ring_idx%TX_RING_NUM]-1))+1 ;
        printk("1st countDown  %d \n",gpQdmaPriv->devCfg.countDown);
        gpQdmaPriv->devCfg.countDownRound = 0 ;
        qdma_st_packet_gen(channel, queue, packets, lens, 0x01, ring_idx%TX_RING_NUM) ;
        msleep(10000) ;

        /* in order to set the TX_CPU_PTR to HW register */
        gpQdmaPriv->devCfg.countDown = 1 ;
        gpQdmaPriv->devCfg.waitTxMaxNums = 1 ;
        qdma_st_packet_gen(channel, queue, 1, lens, 0x01, ring_idx%TX_RING_NUM) ;
    	printk("GK--before_receive: NO_RX_DSCP_INT:%d\n", gpQdmaPriv->counters.intNoRxDscp[ring_idx]);

    	msleep(1000) ;
        qdma_bm_receive_packets(0, ring_idx) ;
        msleep(1000) ;
        printk("INT_CSR:%.8x %.8x, Packets:%d, NO_TX_DSCP_INT Cnt:%d, NO_RX_DSCP_INT Cnt:%d, CntDownRound:%d\n\n", 
            qdmaGetIntStatus1(base),qdmaGetIntStatus2(base), packets, gpQdmaPriv->counters.intNoTxDscp[ring_idx%TX_RING_NUM], 
            gpQdmaPriv->counters.intNoRxDscp[ring_idx], gpQdmaPriv->devCfg.countDownRound) ;
        __st_print_counters() ;

        if((__st_check_counters() == 0) && (gpQdmaPriv->counters.intNoRxDscp[ring_idx] > 0)) {
            QDMA_LOG("csr1: Test Result:Pass\n\n") ;
        } else {
            QDMA_LOG("csr1: Test Result:Failed\n\n") ;
        }
    }

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);

    return 0;
}
#endif

/*7580 HWF_DSCP_EMPTY_INT*/
static int tc_csr2(uint base, uint packets, uint lens, int ringIdx)
{
	/*TX: ringIdx   RX:0*/
    uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
	uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ;
	uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
	uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
	uint intNums = 0;

	if(gpQdmaPriv->txDscpNum[0] < gpQdmaPriv->hwFwdDscpNum) {
		printk("Failed: the number of TX DSCP must larger than HWFWD_DSCP_NUM.\n") ;
		return 0 ;
	}
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, (INT_STATUS_IRQ_INT|INT_STATUS_HWFWD_DSCP_EMPTY));
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, (~(INT_MASK_NO_RX_CPU_DSCP|INT_MASK_RX_DONE)));
	
	qdmaClearIntStatus1(base, 0xFFFFFFFF) ;
	qdmaClearIntStatus2(base, 0xFFFFFFFF) ;
	printk("INT1:%.8x  INT2:%.8x\n",qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1),qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2));
	printk("INT_STATUS1:%.8x  INT_STATUS2:%.8x\n",qdmaGetIntStatus1(base),qdmaGetIntStatus2(base));
	qdmaSetLpbkToRXRing(base, 0);

	memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
	gpQdmaPriv->counters.intNoLinkDscp = 0 ;
	qdma_st_packet_gen(channel, queue, packets, lens, 0x01, 0) ;
	msleep(1000) ;
	printk("receive %d packet\n",qdma_bm_receive_packets(0,0)) ;
	msleep(1000) ;

	intNums = packets/(gpQdmaPriv->hwFwdDscpNum+CONFIG_RX_DSCP_NUM[ringIdx]);
	if(CONFIG_TX_DSCP_NUM[ringIdx] <= gpQdmaPriv->hwFwdDscpNum)
		intNums = 0;
	printk("INT_CSR:%.8x %.8x, Packets:%d, NO_LINK_DSCP_INT Count:%d intNums_expect:%d\n\n", 
	    qdmaGetIntStatus1(base), qdmaGetIntStatus2(base), 
	    packets, gpQdmaPriv->counters.intNoLinkDscp,intNums) ;
	__st_print_counters() ;

	if((__st_check_counters() == 0) && (gpQdmaPriv->counters.intNoLinkDscp >= intNums) && 
        (gpQdmaPriv->counters.intNoLinkDscp <= (intNums*2))) {
		QDMA_LOG("csr2: Test Result:Pass\n\n") ;
	} else {
		QDMA_LOG("csr2: Test Result:Failed\n\n") ;
	}

	qdmaClearIntStatus1(base, 0xFFFFFFFF) ;
	qdmaClearIntStatus2(base, 0xFFFFFFFF) ;
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);

    return 0;
}

/*7580 HWF_DSCP_LOW_INT*/
static int tc_csr3(uint base, uint packets, uint lens, int ringIdx)
{
	/*TX: ringIdx   RX:0*/
    uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
	uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ;
	uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
	uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
	uint intNums = 0;
	uint lowThreshold = 0;

	if(gpQdmaPriv->txDscpNum[0] < gpQdmaPriv->hwFwdDscpNum) {
		printk("Failed: the number of TX DSCP must larger than HWFWD_DSCP_NUM.\n") ;
		return 0 ;
	}

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, (INT_STATUS_IRQ_INT|INT_STATUS_HWFWD_DSCP_LOW));
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, (~(INT_MASK_NO_RX_CPU_DSCP|INT_MASK_RX_DONE)));
	qdmaClearIntStatus1(base, 0xFFFFFFFF) ;
	qdmaClearIntStatus2(base, 0xFFFFFFFF) ;
	printk("INT1:%.8x  INT2:%.8x\n",qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1),qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2));
	printk("INT_STATUS1:%.8x  INT_STATUS2:%.8x\n",qdmaGetIntStatus1(base),qdmaGetIntStatus2(base));
	qdmaSetLpbkToRXRing(base, 0);

	memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
	gpQdmaPriv->counters.intLowLinkDscp = 0 ;
	qdma_st_packet_gen(channel, queue, packets, lens, 0x01, 0) ;
	msleep(1000) ;
	printk("receive %d packet\n",qdma_bm_receive_packets(0,0)) ;
	msleep(1000) ;

	lowThreshold = qdmaGetHwLowThrshld(base);
	printk("Tx0 Dscp Num:%d, Rx0 Dscp Num:%d, HWF Dscp Num:%d, lowThr:%d\n",
		CONFIG_TX_DSCP_NUM[0],CONFIG_RX_DSCP_NUM[0],gpQdmaPriv->hwFwdDscpNum,lowThreshold);
	
	intNums = packets/(gpQdmaPriv->hwFwdDscpNum-lowThreshold+CONFIG_RX_DSCP_NUM[ringIdx]);
	if(CONFIG_TX_DSCP_NUM[ringIdx] <= (gpQdmaPriv->hwFwdDscpNum-lowThreshold))
		intNums = 0;
	printk("INT_CSR:%.8x %.8x, Packets:%d, NO_LINK_DSCP_INT Count:%d intNums_expect:%d\n\n", 
	    					qdmaGetIntStatus1(base), qdmaGetIntStatus2(base), 
	    					packets, gpQdmaPriv->counters.intLowLinkDscp,intNums) ;
	__st_print_counters() ;

	if((__st_check_counters() == 0) && (gpQdmaPriv->counters.intLowLinkDscp >= intNums) && 
        (gpQdmaPriv->counters.intLowLinkDscp <= (intNums*2))) {
		QDMA_LOG("csr3: Test Result:Pass\n\n") ;
	} else {
		QDMA_LOG("csr3: Test Result:Failed\n\n") ;
	}

	qdmaClearIntStatus1(base, 0xFFFFFFFF) ;
	qdmaClearIntStatus2(base, 0xFFFFFFFF) ;
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);

    return 0;
}


static int tc_done_bit_check_test(uint base)
{
    uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
	uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ;
#if defined(TCSUPPORT_CPU_EN7581)
	uint oldInt3 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3) ; 
	uint oldInt4 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4) ;
	uint oldInt5 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5) ; 
#endif
    uint oldGlb = qdmaGetGlbCfg(base) ;
    uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
	uint round_cnt = 0 ;
    uint i=0, j=0, flag=0, packets=0, totalDscpNum=0 ;
	int txRingIdx=0, rxRingIdx=0, result=0 ;
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, (INT_MASK_TX_COHERENT|INT_MASK_RX_COHERENT|INT_MASK_IRQ_INT));
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, 0);
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3, 0);
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4, INT_MASK_RX_COHERENT_HIGH_16_RING);
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5, INT_MASK_TX_COHERENT_HIGH_24_RING);
#endif
    qdmaSetGlbCfg(base, (oldGlb&=~GLB_CFG_CHECK_DONE)) ;

	printk("wait for all dscp's ctrl done set to 1...\n");
    /* set all DSCP done bit to 1 */
	totalDscpNum=0;
	for(txRingIdx=0; txRingIdx<TX_RING_NUM; txRingIdx++)
		totalDscpNum += gpQdmaPriv->txDscpNum[txRingIdx] ;
	for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++)
		totalDscpNum += gpQdmaPriv->rxDscpNum[rxRingIdx] ;
    for(i=0 ; i<totalDscpNum ; i++) {
        ((struct QDMA_DscpInfo_S *)gpQdmaPriv->dscpInfoAddr + i)->dscpPtr->ctrl.done = 1 ;
    }
    gpQdmaPriv->devCfg.rxDscpDoneBit = 1 ;
    gpQdmaPriv->devCfg.txDscpDoneBit = 1 ;

	printk("ctrl done set to 1 finished, done bit check test begin\n");
	for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) 
	{
		qdmaSetLpbkToRXRing(base, rxRingIdx);
		txRingIdx = rxRingIdx % TX_RING_NUM;
		memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
	    for(i=0 ; i<2 ; i++) 
		{
			if(i==0)
			{
				qdmaDisableCheckDone(base);
			}
			else
			{	
				qdmaEnableCheckDone(base);
			}
	        gpQdmaPriv->counters.intRxCoherent[rxRingIdx] = 0 ;
	        gpQdmaPriv->counters.intTxCoherent[txRingIdx] = 0 ;
			packets = 1 + ((unsigned int)random32()) % (min(CONFIG_RX_DSCP_NUM[rxRingIdx]-3, CONFIG_TX_DSCP_NUM[txRingIdx])-1) ;
			round_cnt = 1 + ((unsigned int)random32()) % 10 ;
	        for(j=0 ; j<round_cnt ; j++) {
	            qdma_st_packet_gen(channel, queue, packets, 0, 0x01, txRingIdx) ;
	            qdma_bm_receive_packets(packets, rxRingIdx) ;
	        }
	        printk("rxRingIdx:%d, txRingIdx:%d, CHECK_DONE:%s, packets:%d*%d=%d, RX_COHERENT_INT Count:%d, TX_COHERENT_INT Count:%d" \
				, rxRingIdx, txRingIdx, ((i==0)?("Disable"):("Enable")), packets, round_cnt, packets*round_cnt \
	            , gpQdmaPriv->counters.intRxCoherent[rxRingIdx], gpQdmaPriv->counters.intTxCoherent[txRingIdx]) ;
			if(((i == 0) && (gpQdmaPriv->counters.intRxCoherent[rxRingIdx] == 0) 
						&& (gpQdmaPriv->counters.intTxCoherent[txRingIdx] == 0))
			|| ((i == 1) && (gpQdmaPriv->counters.intRxCoherent[rxRingIdx] == packets*round_cnt) 
						&& (gpQdmaPriv->counters.intTxCoherent[txRingIdx] == packets*round_cnt*2))) {
				printk(", Test Result:Pass\n");
				} else {
				printk(", Test Result:Fail!!****\n");
				result = -1;
				goto done_bit_out ;
			}
	    }
	    __st_print_counters() ;
	}

done_bit_out:
	printk("done bit check test finished, wait for ctrl done clear to 0...\n");
    /* clear all DSCP done bit to 0 */
    for(i=0 ; i<totalDscpNum ; i++) {
        ((struct QDMA_DscpInfo_S *)gpQdmaPriv->dscpInfoAddr + i)->dscpPtr->ctrl.done = 0 ;
    }
    gpQdmaPriv->devCfg.rxDscpDoneBit = 0 ;
    gpQdmaPriv->devCfg.txDscpDoneBit = 0 ;
    delayTime = 0;
	printk("ctrl done clear to 0 finished\n");
	
    qdmaClearIntStatus1(base, 0xFFFFFFFF) ;
	qdmaClearIntStatus2(base, 0xFFFFFFFF) ;
#if defined(TCSUPPORT_CPU_EN7581)
    qdmaClearIntStatus3(base, 0xFFFFFFFF) ;
	qdmaClearIntStatus4(base, 0xFFFF0000) ;
    qdmaClearIntStatus5(base, 0xFFFFFF00) ;
#endif

    qdmaSetGlbCfg(base, oldGlb) ;
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);
#if defined(TCSUPPORT_CPU_EN7581)
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE3, oldInt3);
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE4, oldInt4);
	qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE5, oldInt5);
#endif	
	if(result == 0){
		QDMA_LOG("done bit check test Result: Pass\n\n") ;
   		return 0;
	} else {
		QDMA_LOG("done bit check test Result: Failed\n\n") ;
   		return -1;
	}
}

static int tc_csr4(uint base, uint packets, uint lens, int ringIdx)
{
    uint oldRxDly = 0 ;
    uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
    uint prevIntRxDone = 0 ;
    uint itemMax[2] = {5, 10} ;
    uint ring_idx=0, i=0, j=0 ;
    uint tx_ring_idx=0;
	uint result=0 ;
    uint csr[11] = {0, ((25<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((50<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((100<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((127<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                    0, ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (40<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (80<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (200<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (255<<DLY_INT_RXMAX_PTIME_SHIFT))} ;

    for(ring_idx=0 ; ring_idx < RX_RING_NUM ; ring_idx++){
        if( (ringIdx != RX_RING_NUM) && (ringIdx != ring_idx) )
            continue ;
        qdmaSetLpbkToRXRing(base, ring_idx);
        oldRxDly = qdmaGetRxDelayIntCfg(base, ring_idx) ;
        prevIntRxDone = 0 ;
        printk("\n--------- RX_RING-%d ---------\n", ring_idx);
        for(i=0, j=0 ; j<2 ; j++) {
            if(j == 0)
                printk("RX_PTIME Test: PINT incress, RX_INT Count should decress!\n");
            else
                printk("RX_PINT Test: PTIME incress, RX_INT Count should decress!\n");
            for( ; i<itemMax[j] ; i++) {
                qdmaSetRxDelayIntCfg(base, ring_idx, csr[i]) ;
                gpQdmaPriv->counters.intRxDone[ring_idx] = 0 ;
                qdma_st_packet_gen(channel, queue, packets, 0, 0x01, tx_ring_idx) ;
                msleep(100) ;
                printk("TX-%d Rx-%d Delay Pkt:%d, Time:%d*20us, DlyCSR(%.8x):%.8x, Packets:%d, RX_INT Count:%d\n", 
                                            tx_ring_idx, ring_idx,
                                            ((qdmaGetRxDelayIntCfg(base, ring_idx)&DLY_INT_RXMAX_PINT_MASK)>>DLY_INT_RXMAX_PINT_SHIFT) ,
                                            ((qdmaGetRxDelayIntCfg(base, ring_idx)&DLY_INT_RXMAX_PTIME_MASK)>>DLY_INT_RXMAX_PTIME_SHIFT),
                                            QDMA_CSR_RX_DELAY_INT_CFG(base, ring_idx),
                                            qdmaGetRxDelayIntCfg(base, ring_idx), packets, gpQdmaPriv->counters.intRxDone[ring_idx]) ;
                if((i != 0) && (i != 5)) {
                    if(gpQdmaPriv->counters.intRxDone[ring_idx] > prevIntRxDone){
                        result = -1;
                        qdmaSetRxDelayIntCfg(base, ring_idx, oldRxDly) ;
                        goto exit;
                    }
                }               
                prevIntRxDone = gpQdmaPriv->counters.intRxDone[ring_idx] ;
                msleep(2000) ;
            }
        }
        qdmaSetRxDelayIntCfg(base, ring_idx, oldRxDly) ;
    }

exit:
    if(result == 0)
        QDMA_LOG("csr4: Test Result:Pass\n\n") ;
    else
        QDMA_LOG("csr4: Test Result:Failed\n\n") ;
    
    return 0;
}

static int tc_tk1(uint base, uint packets, uint lens, int ringIdx)
{
    uint channel = 0 ;
    uint queue = 0;

    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    for(channel=0; channel<CONFIG_QDMA_CHANNEL; channel++) { 
        for(queue=0; queue<CONFIG_QDMA_QUEUE; queue++) {
            qdma_st_packet_gen(channel, queue, packets, lens, 0x01, ringIdx) ;
            msleep(100);
        }
    }

    __st_print_counters() ;
    if(__st_check_counters() == 0) {
        QDMA_LOG("tk1: Test Result:Pass\n\n") ;
    } else {
        QDMA_LOG("tk1: Test Result:Failed\n\n") ;
    }
    return 0;
}

static int tc_irq_test(uint base, uint packets, uint lens, int ringIdx)
{
	uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
	uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
	uint entryLen = 0, headIdx = 0, headIdxBase = 0;
	uint thrshldLen = 0, depthLen = 0;
	uint thrshldLenPrev = 0, ptimePrev = 0, depthLenPrev = 0;
	uint irqQueueAddr = 0, irqDepth = 0;
	uint npacket = 0;
	uint result = 0 ;
	dma_addr_t irqDmaAddrPrev = 0 ;
	
	irqDmaAddrPrev = qdmaGetIrqBase(base);
	depthLenPrev = qdmaGetIrqDepth(base);
	ptimePrev = qdmaGetIrqPtime(base);
	thrshldLenPrev = qdmaGetIrqThreshold(base);
	irqQueueAddr = 0;
	
	/* enable IRQ_EN */
	qdmaEnableIrqEn(base);
	depthLen = depthLenPrev;
    /*EN7580: fix tx ring as 0*/
    ringIdx = 0 ;
	
	for(thrshldLen=packets; thrshldLen<=32; thrshldLen=thrshldLen<<1) {
		if(depthLen < thrshldLen)
			continue;
		QDMA_ERR("\r\ndepthLen is %d, thrshldLen is %d.\n", depthLen, thrshldLen) ;
		
		qdmaSetIrqThreshold(base, (thrshldLen));
		qdmaSetIrqPtime(base, 0);
		QDMA_ERR("IRQ_EN_1: Base:0x%08x, Depth:%d, Threshold:%d, EntryLen:%d, HeadIdx:%d, Ptime:%d\n", 
			qdmaGetIrqBase(base), qdmaGetIrqDepth(base), qdmaGetIrqThreshold(base), 
			qdmaGetIrqEntryLen(base), qdmaGetIrqHeadIdx(base), qdmaGetIrqPtime(base)) ;
		
		/*  send thrshldLen*25 packet, expect intTxDone:25 */
		npacket = thrshldLen*25;
		gpQdmaPriv->counters.intIrqcnt=0;
		gpQdmaPriv->counters.txCounts[ringIdx]=0;
		qdma_st_packet_gen(channel, queue, npacket, lens, 0x01, ringIdx) ;
		msleep(100);
		if(gpQdmaPriv->counters.intIrqcnt == npacket*2/thrshldLen)
		{
			QDMA_ERR("IRQ_EN_1 test pass: npacket:%d, intTxDone:%d, txCounts:%d\r\n",
			npacket, gpQdmaPriv->counters.intIrqcnt, gpQdmaPriv->counters.txCounts[ringIdx]);
		} else {
			QDMA_ERR("IRQ_EN_1 test fail: npacket:%d, intTxDone:%d, txCounts:%d\r\n",
			npacket, gpQdmaPriv->counters.intIrqcnt, gpQdmaPriv->counters.txCounts[ringIdx]);
			result = -1;
			goto tk2_finish_1;
		}
		
		if(thrshldLen==1)
			continue;

		/*  send thrshldLen-1 packet, expect intTxDone:0, entryLen:thrshldLen-1 */
		gpQdmaPriv->counters.intIrqcnt=0;	
		qdma_st_packet_gen(channel, queue, thrshldLen-1, lens, 0x01, ringIdx) ;
		msleep(3000);
		entryLen = qdmaGetIrqEntryLen(base);
		headIdxBase = qdmaGetIrqHeadIdx(base);
		if((gpQdmaPriv->counters.intIrqcnt == 0) && (entryLen == thrshldLen-1))
			QDMA_ERR("IRQ_EN_2 test pass.\r\n");
		else {
			QDMA_ERR("IRQ_EN_2 test fail: npacket:%d, gpQdmaPriv->counters.intTxDone:%d, entryLen:%d\r\n", 
                npacket, gpQdmaPriv->counters.intIrqcnt, entryLen);
			result = -1;
			goto tk2_finish_1;
		}
		
		/*  clear (thrshldLen+1)/2 Irq, expect entryLen:(thrshldLen-1)-(thrshldLen-1)/2, headIdx: increace (thrshldLen+1)/2 */
		qdmaSetIrqClearLen(base, (thrshldLen+1)/2);
		entryLen = qdmaGetIrqEntryLen(base);
		headIdx = qdmaGetIrqHeadIdx(base);
		if((entryLen == (thrshldLen-1)-(thrshldLen+1)/2) && (headIdx == (headIdxBase+(thrshldLen+1)/2)%512))
			QDMA_ERR("IRQ_EN_3 test pass.\r\n");
		else {
			QDMA_ERR("IRQ_EN_3 test fail: entryLen:%d, headIdxBase:%d, headIdx:%d\r\n", entryLen, headIdxBase, headIdx);
			result = -1;
			goto tk2_finish_1;
		}
		
		/*  clear (thrshldLen+1)/2 Irq, expect entryLen:0 headIdx: increace  thrshldLen-1 */
		qdmaSetIrqClearLen(base, (thrshldLen+1)/2);
		entryLen = qdmaGetIrqEntryLen(base);
		headIdx = qdmaGetIrqHeadIdx(base);
		if((entryLen == 0) && (headIdx == (headIdxBase+thrshldLen-1)%512))
			QDMA_ERR("IRQ_EN_4 test pass.\r\n");
		else {
			QDMA_ERR("IRQ_EN_4 test fail: entryLen:%d, headIdxBase:%d, headIdx:%d\r\n", 
                entryLen, headIdxBase, headIdx);
			result = -1;
			goto tk2_finish_1;
		}

		/*  set Ptime as 1, send thrshldLen-1 packet, expect intTxDone:(thrshldLen-1)*/
		qdmaSetIrqPtime(base, 1);
		gpQdmaPriv->counters.intIrqcnt=0;	
		qdma_st_packet_gen(channel, queue, thrshldLen-1, lens, 0x01, ringIdx) ;
		msleep(3000);
		if(gpQdmaPriv->counters.intIrqcnt == (thrshldLen-1)*2)
			QDMA_ERR("IRQ_EN_5 test pass: gpQdmaPriv->counters.intTxDone:%d\r\n", 
			gpQdmaPriv->counters.intIrqcnt);
		else {
			QDMA_ERR("IRQ_EN_5 test fail: gpQdmaPriv->counters.intTxDone:%d\r\n", 
                gpQdmaPriv->counters.intIrqcnt);
			result = -1;
			goto tk2_finish_1;
		}
		
		/*  set Ptime as 0xFFFF, send thrshldLen-1 packet, expect intTxDone:1 */
		qdmaSetIrqPtime(base, 0xFFFF);
		gpQdmaPriv->counters.intIrqcnt=0;	
		qdma_st_packet_gen(channel, queue, thrshldLen-1, lens, 0x01, ringIdx) ;
		msleep(3000);
		if(gpQdmaPriv->counters.intIrqcnt == 2)
			QDMA_ERR("IRQ_EN_6 test pass: gpQdmaPriv->counters.intTxDone:%d\r\n", 
			gpQdmaPriv->counters.intIrqcnt);
		else {
			QDMA_ERR("IRQ_EN_6 test fail: gpQdmaPriv->counters.intTxDone:%d\r\n", 
                gpQdmaPriv->counters.intIrqcnt);
			result = -1;
			goto tk2_finish_1;
		}
	}

tk2_finish_1:
	if(irqQueueAddr) {
		QDMA_ERR("irqQueueAddr free\r\n");
		dma_free_coherent(NULL, 4*irqDepth, (void *)irqQueueAddr, qdmaGetIrqBase(gpQdmaPriv->csrBaseAddr)) ;
		irqQueueAddr = 0 ;
	}

	if(result == 0)
		QDMA_LOG("\r\nirq test pass.\r\n");
	else
		QDMA_LOG("\r\nirq test fail.\r\n");
	
	delayTime = 0;
	qdmaSetIrqPtime(base, ptimePrev);
	qdmaSetIrqThreshold(base, thrshldLenPrev);
	//dumpMemory(QDMA_REG_BASE, 0x1000);
    return 0;
}


/*  return 0 pass ,return 1 fail */
static int loopcnt_test(uint base, uint lens, int ringIdx, uint channel, uint queue)
{
    uint8 index,i=0;
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
    uint loop_test[4] = {0x00,0x40,0x80,0xc0};  /* 00: 3 cycles, 40: 0 cycle, 80: 1 cycles, c0: 2 cycles,*/
	uint loop_cycle[4] = {3,0,1,2};
#else
    uint loop_test[2] = {100,255};
#endif
	uint cpuTxCnt = 0, cpuRxCnt = 0, fwdTxCnt = 0, fwdRxCnt = 0 ;
    QDMA_DBG_CNTR_T dbgCntr;

#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
    for(index=0;index<4;index++)
#else
    for(index=0;index<2;index++)
#endif
	{
		QDMA_LOG("Test-%d, loopCnt value = 0x%02X\r\n", index, loop_test[index]);
    	dbgCntr.cntrEn = DBG_CNTR_ENABLE ;
    	dbgCntr.isChnlAll = QDMA_ENABLE ;
    	dbgCntr.isQueueAll = QDMA_ENABLE ;
    	dbgCntr.isDscpRingAll = QDMA_ENABLE ;
    	dbgCntr.chnlIdx = channel ;
    	dbgCntr.queueIdx = queue ;
    	dbgCntr.dscpRingIdx = 0 ;
    	for(i=0;i<CONFIG_MAX_CNTR_NUM;i+=4){
        	dbgCntr.cntrIdx = i;
        	dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_TX;
        	qdma_clear_and_enable_dbg_cntr_info(&dbgCntr);
        	dbgCntr.cntrIdx = i + 1;
        	dbgCntr.cntrSrc = DBG_CNTR_SRC_CPU_RX;
        	qdma_clear_and_enable_dbg_cntr_info(&dbgCntr);
        	dbgCntr.cntrIdx = i + 2;
        	dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_TX;
        	qdma_clear_and_enable_dbg_cntr_info(&dbgCntr);
        	dbgCntr.cntrIdx = i + 3;
        	dbgCntr.cntrSrc = DBG_CNTR_SRC_FWD_RX;
        	qdma_clear_and_enable_dbg_cntr_info(&dbgCntr);
        }
        loopCnt = loop_test[index];
    	qdmaSetIrqPtime(base, 0);
    	qdmaSetIrqThreshold(base, 100);
    	qdma_st_packet_gen(channel, queue, 1, lens, 0x01, ringIdx) ;
        msleep(200);
    	qdmaSetIrqPtime(base, 100);
    	/*  cpu tx=1, cpu rx=1, hw_fwd = 99 */
    	for(i=0;i<CONFIG_MAX_CNTR_NUM;i+=4){
        	dbgCntr.cntrIdx = i ;
        	qdma_get_dbg_cntr_info(&dbgCntr) ;
        	cpuTxCnt = dbgCntr.cntrVal;
                
        	dbgCntr.cntrIdx = i + 1 ;
        	qdma_get_dbg_cntr_info(&dbgCntr) ;
        	cpuRxCnt = dbgCntr.cntrVal ;
                
        	dbgCntr.cntrIdx = i + 2 ;
        	qdma_get_dbg_cntr_info(&dbgCntr) ;
        	fwdTxCnt = dbgCntr.cntrVal ;
                
        	dbgCntr.cntrIdx = i + 3 ;
        	qdma_get_dbg_cntr_info(&dbgCntr) ;
        	fwdRxCnt = dbgCntr.cntrVal ;
            /*EN7580: fwd rx include cpu tx*/
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
			if((loopCnt == 0x00) || (loopCnt == 0x40) || (loopCnt == 0x80) || (loopCnt == 0xc0)){
				if((cpuTxCnt == 1) && (cpuRxCnt == 1) && (fwdTxCnt == (loop_cycle[index]+1)) && (fwdRxCnt == (loop_cycle[index]+1)))
	    			QDMA_LOG("%2d loopcnt_%d test pass.cpuTxCnt:%d, cpuRxCnt:%d, fwdTxCnt:%d, fwdRxCnt:%d\r\n",
	    			    i, loopCnt, cpuTxCnt, cpuRxCnt, fwdTxCnt, fwdRxCnt);
	    	    else {
	    			QDMA_LOG("%2d loopcnt_%d test fail: cpuTxCnt:%d, cpuRxCnt:%d, fwdTxCnt:%d, fwdRxCnt:%d\r\n", 
	                    i, loopCnt, cpuTxCnt, cpuRxCnt, fwdTxCnt, fwdRxCnt);
	    		    return 1;
	    		}
			}else{
				QDMA_LOG("test fail: wrong loopCnt = 0x%02X\r\n", loopCnt);
    		    return 1;
			}
#else
    		if((cpuTxCnt == 1) && (cpuRxCnt == 1) && (fwdTxCnt == loopCnt) && ((fwdRxCnt-cpuTxCnt) == (loopCnt-1)))
    			QDMA_LOG("%2d loopcnt_%d test pass.cpuTxCnt:%d, cpuRxCnt:%d, fwdTxCnt:%d, fwdRxCnt:%d\r\n",
    			    i, loopCnt, cpuTxCnt, cpuRxCnt, fwdTxCnt, fwdRxCnt);
    	    else {
    			QDMA_LOG("%2d loopcnt_%d test fail: cpuTxCnt:%d, cpuRxCnt:%d, fwdTxCnt:%d, fwdRxCnt:%d\r\n", 
                    i, loopCnt, cpuTxCnt, cpuRxCnt, fwdTxCnt, fwdRxCnt);
    		    return 1;
    		}
#endif
		}	
    }
    return 0;
}

static int dbg_cntr_config(
	unchar				cntrIdx,
	QDMA_DbgCntrSrc_t	cntrSrc,
	unchar				isChnlAll,
	unchar				isQueueAll,
	unchar				isDscpRingAll,
	unchar				chnlIdx,
	unchar				queueIdx,
	unchar				dscpRingIdx)
{
    QDMA_DBG_CNTR_T dbgCntr;
    dbgCntr.cntrIdx = cntrIdx;
    dbgCntr.cntrSrc = cntrSrc;
    dbgCntr.isChnlAll = isChnlAll ;
    dbgCntr.isQueueAll = isQueueAll ;
    dbgCntr.isDscpRingAll = isDscpRingAll ;
    dbgCntr.chnlIdx = chnlIdx ;
    dbgCntr.queueIdx = queueIdx ;
    dbgCntr.dscpRingIdx = dscpRingIdx ;
    qdma_clear_and_enable_dbg_cntr_info(&dbgCntr);

    return 0;
}

static int dbg_cntr_init(void) {
	/*  CPU TX Config */
	dbg_cntr_config(0, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_ENABLE, -1, -1, -1); 	/*  0: CPU TX ALL */
	dbg_cntr_config(1, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 0); 	/*  1: CPU_TX Ring0 All */
	dbg_cntr_config(2, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 1); 	/*  2: CPU_TX Ring1 All */
	dbg_cntr_config(3, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_ENABLE, QDMA_ENABLE, 0, -1, -1);		/*  3: CPU_TX Channel_0 All */
	dbg_cntr_config(4, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_ENABLE, QDMA_ENABLE, 1, -1, -1);		/*  4: CPU_TX Channel_1 All */
	dbg_cntr_config(5, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_ENABLE, QDMA_ENABLE, 7, -1, -1);		/*  5: CPU_TX Channel_7 All */
	dbg_cntr_config(6, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_DISABLE, QDMA_ENABLE, -1, 0, -1);		/*  6: CPU_TX Queue_0 All */
	dbg_cntr_config(7, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_DISABLE, QDMA_ENABLE, -1, 1, -1);		/*  7: CPU_TX Queue_1 All */
	dbg_cntr_config(8, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_DISABLE, QDMA_ENABLE, -1, 7, -1);		/*  8: CPU_TX Queue_7 All */

	dbg_cntr_config(9, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 0, 0, -1); 	/*  9: CPU_TX channel_0  queue_0 */
	dbg_cntr_config(10, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 0, 7, -1);	/*  10: CPU_TX channel_0  queue_7 */
	dbg_cntr_config(11, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 0, -1);	/*  11: CPU_TX channel_1  queue_0 */
	dbg_cntr_config(12, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 7, -1);	/*  12: CPU_TX channel_1  queue_7 */
	dbg_cntr_config(13, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 7, 0, -1);	/*  13: CPU_TX channel_7  queue_0 */
	dbg_cntr_config(14, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 7, 1, -1);	/*  14: CPU_TX channel_7  queue_1 */
	dbg_cntr_config(15, DBG_CNTR_SRC_CPU_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 7, 7, -1);	/*  15: CPU_TX channel_7  queue_7 */
	
	/*  FWD TX Config */
	dbg_cntr_config(16, DBG_CNTR_SRC_FWD_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_ENABLE, -1, -1, -1);	/*  16: FWD TX ALL */
	dbg_cntr_config(17, DBG_CNTR_SRC_FWD_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 0);	/*  17: FWD_TX Ring0 All */
	dbg_cntr_config(18, DBG_CNTR_SRC_FWD_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 1);	/*  18: FWD_TX Ring1 All */
	dbg_cntr_config(19, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_ENABLE, QDMA_ENABLE, 0, -1, -1); 	/*  19: FWD_TX Channel_0 All */
	dbg_cntr_config(20, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_ENABLE, QDMA_ENABLE, 1, -1, -1); 	/*  20: FWD_TX Channel_1 All */
	dbg_cntr_config(21, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_ENABLE, QDMA_ENABLE, 7, -1, -1); 	/*  21: FWD_TX Channel_7 All */
	dbg_cntr_config(22, DBG_CNTR_SRC_FWD_TX, QDMA_ENABLE, QDMA_DISABLE, QDMA_ENABLE, -1, 0, -1); 	/*  22: FWD_TX Queue_0 All */
	dbg_cntr_config(23, DBG_CNTR_SRC_FWD_TX, QDMA_ENABLE, QDMA_DISABLE, QDMA_ENABLE, -1, 1, -1); 	/*  23: FWD_TX Queue_1 All */
	dbg_cntr_config(24, DBG_CNTR_SRC_FWD_TX, QDMA_ENABLE, QDMA_DISABLE, QDMA_ENABLE, -1, 7, -1); 	/*  24: FWD_TX Queue_7 All */
	
	dbg_cntr_config(25, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 0, 0, -1);	/*  25: FWD_TX channel_0  queue_0 */
	dbg_cntr_config(26, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 0, 7, -1);	/*  26: FWD_TX channel_0  queue_7 */
	dbg_cntr_config(27, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 0, -1);	/*  27: FWD_TX channel_1  queue_0 */
	dbg_cntr_config(28, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 7, -1);	/*  28: FWD_TX channel_1  queue_7 */
	dbg_cntr_config(29, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 7, 0, -1);	/*  29: FWD_TX channel_7  queue_0 */
	dbg_cntr_config(30, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 7, 1, -1);	/*  30: FWD_TX channel_7  queue_1 */
	dbg_cntr_config(31, DBG_CNTR_SRC_FWD_TX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 7, 7, -1);	/*  31: FWD_TX channel_7  queue_7 */
	
	/*  CPU & FWD RX Config */
	dbg_cntr_config(32, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_ENABLE, -1, -1, -1);	/*  32: CPU RX ALL */
	dbg_cntr_config(33, DBG_CNTR_SRC_FWD_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_ENABLE, -1, -1, -1);	/*  33: FWD RX ALL */
	dbg_cntr_config(34, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 0);	/*  34: CPU_RX Ring0 All */
	dbg_cntr_config(35, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 1);	/*  35: CPU_RX Ring1 All */
	dbg_cntr_config(36, DBG_CNTR_SRC_CPU_RX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 7, -1);	/*  36: CPU_RX channel_1  queue_7 */ /*EN7580: => CPU RX ALL*/
	dbg_cntr_config(37, DBG_CNTR_SRC_CPU_RX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 0, -1);	/*  37: CPU_RX channel_1  queue_0 */ /*EN7580: => CPU RX ALL*/
	dbg_cntr_config(38, DBG_CNTR_SRC_FWD_RX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 7, -1);	/*  38: FWD_RX channel_1  queue_7 */
	dbg_cntr_config(39, DBG_CNTR_SRC_FWD_RX, QDMA_DISABLE, QDMA_DISABLE, QDMA_ENABLE, 1, 0, -1);	/*  39: FWD_RX channel_1  queue_0 */

    /*new add for EN7580*/
    dbg_cntr_config(40, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 2);     /*  40: CPU_TX Ring2 All */
    dbg_cntr_config(41, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 3);     /*  41: CPU_TX Ring3 All */
    dbg_cntr_config(42, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 4);     /*  42: CPU_TX Ring4 All */
    dbg_cntr_config(43, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 5);     /*  43: CPU_TX Ring5 All */
    dbg_cntr_config(44, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 6);     /*  44: CPU_TX Ring6 All */
    dbg_cntr_config(45, DBG_CNTR_SRC_CPU_TX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 7);     /*  45: CPU_TX Ring7 All */

    dbg_cntr_config(46, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 2);	/*  34: CPU_RX Ring2 All */
	dbg_cntr_config(47, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 3);	/*  35: CPU_RX Ring3 All */
    dbg_cntr_config(48, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 4);	/*  34: CPU_RX Ring4 All */
	dbg_cntr_config(49, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 5);	/*  35: CPU_RX Ring5 All */
    dbg_cntr_config(50, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 6);	/*  34: CPU_RX Ring6 All */
	dbg_cntr_config(51, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 7);	/*  35: CPU_RX Ring7 All */
    dbg_cntr_config(52, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 8);	/*  34: CPU_RX Ring8 All */
	dbg_cntr_config(53, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 9);	/*  35: CPU_RX Ring9 All */
    dbg_cntr_config(54, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 10);	/*  34: CPU_RX Ring10 All */
	dbg_cntr_config(55, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 11);	/*  35: CPU_RX Ring11 All */
    dbg_cntr_config(56, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 12);	/*  34: CPU_RX Ring12 All */
	dbg_cntr_config(57, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 13);	/*  35: CPU_RX Ring13 All */
    dbg_cntr_config(58, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 14);	/*  34: CPU_RX Ring14 All */
	dbg_cntr_config(59, DBG_CNTR_SRC_CPU_RX, QDMA_ENABLE, QDMA_ENABLE, QDMA_DISABLE, -1, -1, 15);	/*  35: CPU_RX Ring15 All */

	return 0;
}

/*  return 0 pass ,return 1 fail */
static int dbg_cntr_test(uint base, uint packets, uint lens)
{
    uint8 index=0, i=0;
	uint8 txRingIdx=0, rxRingIdx=0 ;
	uint channel=0, queue=0 ;
	uint npackets=0, tx_cpu_total_cnt=0, tx_cpu_channel_cnt=0, tx_cpu_queue_cnt=0 ;
	uint tx_fwd_total_cnt=0, tx_fwd_channel_cnt=0, tx_fwd_queue_cnt=0, tx_fwd_stream_cnt=0 ;
	uint rx_cpu_total_cnt=0, rx_fwd_total_cnt=0, rx_fwd_stream_cnt=0 ;
	uint dbgCntrVal[CONFIG_MAX_CNTR_NUM], expectedVal[CONFIG_MAX_CNTR_NUM];
    uint loop_test[DBG_CNTR_LOOP_NUM] = {1, 2, 100};
    uint tx_cpu_ring_cnt[TX_RING_NUM] = {0};
    uint rx_cpu_ring_cnt[RX_RING_NUM] = {0};

	for(index=0; index<DBG_CNTR_LOOP_NUM; index++) {
		loopCnt = loop_test[index];
		//for(txRingIdx=0; txRingIdx<TX_RING_NUM; txRingIdx++) {
			for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) {
                txRingIdx = rxRingIdx % TX_RING_NUM;
				qdmaSetLpbkToRXRing(base, rxRingIdx);
                memset(tx_cpu_ring_cnt, 0, sizeof(uint)*TX_RING_NUM) ;
                memset(rx_cpu_ring_cnt, 0, sizeof(uint)*RX_RING_NUM) ;
				
				npackets = packets;
				tx_cpu_total_cnt = npackets * CONFIG_QDMA_CHANNEL * CONFIG_QDMA_QUEUE;
				tx_cpu_channel_cnt = npackets * CONFIG_QDMA_QUEUE;
				tx_cpu_queue_cnt = npackets * CONFIG_QDMA_CHANNEL;
				tx_cpu_ring_cnt[txRingIdx] = tx_cpu_total_cnt;
				
				tx_fwd_total_cnt = loopCnt * tx_cpu_total_cnt;
				tx_fwd_channel_cnt = loopCnt * tx_cpu_channel_cnt;
				tx_fwd_queue_cnt = loopCnt * tx_cpu_queue_cnt;
				tx_fwd_stream_cnt = loopCnt * npackets;
				
				rx_cpu_total_cnt = tx_cpu_total_cnt;
				rx_cpu_ring_cnt[rxRingIdx] = rx_cpu_total_cnt;
				rx_fwd_total_cnt = (loopCnt-1) * tx_cpu_total_cnt + tx_cpu_total_cnt;   /*EN7580: rx_fwd cnt = tx cpu + rx fwd*/
				rx_fwd_stream_cnt = (loopCnt-1) * npackets + npackets;                  /*EN7580: rx_fwd cnt = tx cpu + rx fwd*/
				
				dbg_cntr_init();
				printk("\r\nsending Packets.");
				for(channel=0; channel<CONFIG_QDMA_CHANNEL; channel++){
					for(queue=0; queue<CONFIG_QDMA_QUEUE; queue++){
						qdma_st_packet_gen(channel, queue, npackets, lens, 0x01, txRingIdx) ;
						printk(".");
        				msleep(500);
			       	}
			    }
				
				//for(i=0; i<CONFIG_MAX_CNTR_NUM; i++) {
				/*only use 60 dbg cnt*/
                for(i=0; i<60; i++) {
					dbgCntrVal[i] = qdmaGetCntrCounter(base, i);
				}

				(expectedVal[0] = tx_cpu_total_cnt); 
                (expectedVal[1] = tx_cpu_ring_cnt[0]); 
                (expectedVal[2] = tx_cpu_ring_cnt[1]); 
                (expectedVal[3] = tx_cpu_channel_cnt);
                
				(expectedVal[4] = tx_cpu_channel_cnt); 
                (expectedVal[5] = tx_cpu_channel_cnt); 
                (expectedVal[6] = tx_cpu_queue_cnt); 
                (expectedVal[7] = tx_cpu_queue_cnt);
                
				(expectedVal[8] = tx_cpu_queue_cnt); 
                (expectedVal[9] = npackets); 
                (expectedVal[10] = npackets); 
                (expectedVal[11] = npackets);
                
				(expectedVal[12] = npackets); 
                (expectedVal[13] = npackets); 
                (expectedVal[14] = npackets); 
                (expectedVal[15] = npackets);
                
				(expectedVal[16] = tx_fwd_total_cnt); 
                (expectedVal[17] = tx_fwd_total_cnt);
                (expectedVal[18] = tx_fwd_total_cnt); 
                (expectedVal[19] = tx_fwd_channel_cnt);
                
				(expectedVal[20] = tx_fwd_channel_cnt); 
                (expectedVal[21] = tx_fwd_channel_cnt); 
                (expectedVal[22] = tx_fwd_queue_cnt); 
                (expectedVal[23] = tx_fwd_queue_cnt);
                
				(expectedVal[24] = tx_fwd_queue_cnt); 
                (expectedVal[25] = tx_fwd_stream_cnt); 
                (expectedVal[26] = tx_fwd_stream_cnt); 
                (expectedVal[27] = tx_fwd_stream_cnt);
                
				(expectedVal[28] = tx_fwd_stream_cnt); 
                (expectedVal[29] = tx_fwd_stream_cnt); 
                (expectedVal[30] = tx_fwd_stream_cnt); 
                (expectedVal[31] = tx_fwd_stream_cnt);

                (expectedVal[32] = rx_cpu_total_cnt); 
                (expectedVal[33] = rx_fwd_total_cnt); 
                (expectedVal[34] = rx_cpu_ring_cnt[0]); 
                (expectedVal[35] = rx_cpu_ring_cnt[1]);
                
				(expectedVal[36] = rx_cpu_total_cnt); /*EN7580: => CPU RX ALL*/
                (expectedVal[37] = rx_cpu_total_cnt); /*EN7580: => CPU RX ALL*/
                (expectedVal[38] = rx_fwd_stream_cnt); 
                (expectedVal[39] = rx_fwd_stream_cnt);

                /*new add for EN7580*/
                for(i=0; i<6; i++) {
                    expectedVal[40+i] = tx_cpu_ring_cnt[2+i];
                }
                for(i=0; i<14; i++) {
                    expectedVal[46+i] = rx_cpu_ring_cnt[2+i];
                }

				//for(i=0; i<CONFIG_MAX_CNTR_NUM; i++){
				/*only use 60 dbg cnt*/
				for(i=0; i<60; i++){
				 	if(dbgCntrVal[i] != expectedVal[i]) {
						printk("\r\nloopcnt:%d, txRingIdx:%d, rxRingIdx:%d, test Fail!\r\n", loopCnt, txRingIdx, rxRingIdx);
						for(i=0; (i+3)<CONFIG_MAX_CNTR_NUM; i+=4)
							printk("dbgCntrVal[%d]:%d, dbgCntrVal[%d]:%d, dbgCntrVal[%d]:%d, dbgCntrVal[%d]:%d\r\n", i, dbgCntrVal[i], i+1, dbgCntrVal[i+1], i+2, dbgCntrVal[i+2], i+3, dbgCntrVal[i+3]);
						printk("########## expected counter as below ##########\n");
						for(i=0; (i+3)<CONFIG_MAX_CNTR_NUM; i+=4)
							printk("expectedVal[%d]:%d, expectedVal[%d]:%d, expectedVal[%d]:%d, expectedVal[%d]:%d\r\n", i, expectedVal[i], i+1, expectedVal[i+1], i+2, expectedVal[i+2], i+3, expectedVal[i+3]);
						return 1;
					}
				}
				printk("\r\nloopcnt:%d, txRingIdx:%d, rxRingIdx:%d, test Pass!\r\n", loopCnt, txRingIdx, rxRingIdx);
			}
		//}
	}

	return 0;
}

static int tc_tk3(uint base, uint packets, uint lens, int ringIdx)
{
	uint channel = 0;
	uint queue = 0;	
	uint result1=0, result2=0;
    
	/*  enable LOOPCNT_EN */
	qdmaEnableLoopCnt(base);

    if(packets == 0){/*loopcnt enable test*/
        printk("\n\nloopcnt enable test begin\n");
        for(channel = 0;channel<CONFIG_QDMA_CHANNEL;channel++){
            for(queue = 0;queue<CONFIG_QDMA_QUEUE;queue++){
                printk("loopcnt test channel %d,queue %d \n",channel,queue);
                result1 = loopcnt_test(base, lens, ringIdx, channel, queue);
                if(result1){
                    printk("loopcnt test failed!\r\n");
                    goto tk3_finish;
                }
            }
        }
    }else{/*debug cnt test*/
    	printk("\n\ndbg counter test begin\n");
    	result2 = dbg_cntr_test(base, packets, lens);
    	if(result2){
    		printk("dbg cntr test failed!\r\n");
    	}
    }
	
tk3_finish:
	if((result1 == 0) && (result2 == 0))
		QDMA_LOG("tk3 test pass.\r\n");
	else
		QDMA_LOG("tk3 test fail.\r\n");
		
	/*  disable LOOPCNT_EN */
	qdmaDisableLoopCnt(base);
    return 0;
}

static int tc_tk4(uint base, uint packets, uint lens, int ringIdx)
{
    uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
    uint round = 0 ;
    uint ring_idx = 0 ;
	
    /* round-0: tx 0~7 => rx 0~7 ; round-1: tx 0~7 => rx 8~15 */
    for(round=0;round<SUPPORT_ROUNT_CNT;round++)
    {
        for( ring_idx=0 ; ring_idx<TX_RING_NUM ; ring_idx++ )
        {
            memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
            gpQdmaPriv->counters.txCounts[ring_idx]=0;
            gpQdmaPriv->counters.rxCounts[ring_idx]=0;
            rxCounter[ring_idx]=0;
            
            packets = ((unsigned int)random32()) % 100;
            qdmaSetLpbkToRXRing(base, ring_idx+(round<<3));
            qdma_st_packet_gen(channel, queue, packets, lens, 0x01, ring_idx) ;
            msleep(500);

            __st_print_counters() ;
            printk("TX-%d %d \t RX-%d %d \n",
            ring_idx, gpQdmaPriv->counters.txCounts[ring_idx], ring_idx+(round<<3), rxCounter[ring_idx]);

            if(gpQdmaPriv->counters.txCounts[ring_idx] != rxCounter[ring_idx]){
                goto fail;
            }
        }
    }

    QDMA_LOG("tk4 test pass.\r\n");
    return 0;

fail:
    QDMA_LOG("tk4 test fail.\r\n");
    return 0;
}

static int tc_tk5(uint base, uint packets, uint lens, int ringIdx)
{
    uint round = 0;
    uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
    uint oldHxqDyn = qdmaGetTxqDynTotalMinThrh(base);
    uint oldInt = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1);
	struct ECNT_QDMA_Data qdma_data ;
	
    /*  clear count */
    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    gpQdmaPriv->counters.txCounts[RING_IDX_0]=0;
    gpQdmaPriv->counters.txCounts[RING_IDX_1]=0;
    rxCounter[RING_IDX_0]=0;
    rxCounter[RING_IDX_1]=0;

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, INT_STATUS_IRQ_INT);
    qdmaSetTxqDynTotalMinThrh(base, 10);  /*  small enough to block tx low chain */
    /*  fill until rx chain full(512 dscps), linkmgr(1000 dscps) almost full */
    qdma_st_packet_gen(channel, queue, 1000, lens, 0x01, RING_IDX_1);

    printk("\n::TX0 %d, TX1 %d, RX0 %d, RX1 %d\n",gpQdmaPriv->counters.txCounts[RING_IDX_0],
        gpQdmaPriv->counters.txCounts[RING_IDX_1],rxCounter[RING_IDX_0],rxCounter[RING_IDX_1]);
    msleep(1000);

    /*  send 2 packet, recv 1 packet */
    printk("Sending %d packets\n",PACKET_NUM);
    for(round=0;round<PACKET_NUM;round++){
        qdma_st_packet_gen(channel, queue, 1, lens, 0x01, RING_IDX_0);
        qdma_st_packet_gen(channel, queue, 1, lens, 0x01, RING_IDX_1);
        if(round & 0x10){
            msleep(10);
        }
		qdma_data.qdma_private.cnt = 1 ;
        qdma_receive_packets(&qdma_data);
        printk(".");
    }
    /*  tx0 packet was block by linkmgr */
    printk("\nTX0 %d, TX1 %d, RX0 %d, RX1 %d\n",gpQdmaPriv->counters.txCounts[RING_IDX_0],
        gpQdmaPriv->counters.txCounts[RING_IDX_1],rxCounter[RING_IDX_0],rxCounter[RING_IDX_1]);
    msleep(1000);
    /*  should get 2000 from TX1, get 0 from TX0 */
    if((rxCounter[RING_IDX_0] == 0) && (rxCounter[RING_IDX_1] == PACKET_NUM)){
        QDMA_LOG("tk5 test pass.\r\n");
    }else{
        QDMA_LOG("tk5 test fail.\r\n");
    }
    /*  recv the rest packet */
    qdma_bm_receive_packets(0,0);
    qdmaClearIntStatus1(base, 0xFFFFFFFF) ;
    qdmaSetTxqDynTotalMinThrh(base,oldHxqDyn);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt);
    return 0;
}

#if 0
static int tc_tk6(uint base, uint packets, uint lens, int ringIdx)
{
    uint channel = random32() % CONFIG_QDMA_CHANNEL ;
    uint queue = random32() % CONFIG_QDMA_QUEUE ;
    uint round=0 ;
    
    /*  clear count */
    gpQdmaPriv->counters.txCounts[RING_IDX_0]=0;
    gpQdmaPriv->counters.txCounts[RING_IDX_1]=0;
    gpQdmaPriv->counters.rxCounts[RING_IDX_0]=0;
    gpQdmaPriv->counters.rxCounts[RING_IDX_1]=0;
    qdmaSetTxBufChnnelThreshold(base, 0x10);
    qdmaSetTxBufTotalThreshold(base,0x80);

    for(round=0;round<100;round++){
        printk("Sending packet!\n");
        qdma_st_packet_gen(channel, queue, 1000, lens, 0x01, RING_IDX_0);
        if(round & 0x10){
            msleep(10);
        }
    }
    printk("Tx %d, Rx %d \n",gpQdmaPriv->counters.txCounts[RING_IDX_0],
        gpQdmaPriv->counters.rxCounts[RING_IDX_0]);

    qdmaSetTxBufChnnelThreshold(base, 0x10);
    qdmaSetTxBufTotalThreshold(base,0x80);

    for(round=0;round<100;round++){
        qdma_st_packet_gen(channel, queue, 1000, lens, 0x01, RING_IDX_0);
        printk("Sending packet!\n");
        if(round & 0x10){
            msleep(10);
        }
    }
    printk("Tx %d, Rx %d \n",gpQdmaPriv->counters.txCounts[RING_IDX_0],
        gpQdmaPriv->counters.rxCounts[RING_IDX_0]);
    return 0;
}
#endif

/*EN7580: qos weight changed to 16 bit*/
static int tc_tk7(uint base, uint packets, uint lens, int ringIdx)
{
	int result=0;
	ushort weight[CONFIG_QDMA_QUEUE];
	uint channelIdx=0, queueIdx=0;
	uint weightReg0=0, weightReg1=0, weightReg2=0, weightReg3=0;
	uint weightMemXs0=0, weightMemXs1=0, weightMemXs2=0, weightMemXs3=0;
	QDMA_TxQosScheduler_T TxQos;
	struct ECNT_QDMA_Data qdma_data;

	/*  Write: general register;   Read: dbg mem xs */
	/*  0x2: QDMA QoS Weight */
	for(channelIdx=0; channelIdx<CONFIG_QDMA_CHANNEL; channelIdx++){
		/*  Write: general register */
		TxQos.channel = channelIdx;
		TxQos.qosType = QDMA_TXQOS_TYPE_WRR;
		for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++){
			TxQos.queue[queueIdx].weight = ((unsigned int)random32())%100;
            if(TxQos.queue[queueIdx].weight == 0)
                TxQos.queue[queueIdx].weight = 1 ;
        }
		qdma_data.qdma_private.qdma_tx_qos.pTxQos = &TxQos;
		qdma_set_tx_qos(&qdma_data) ;
		if(result<0)
			goto tk7_finish;
		
		/*  Read: dbg mem xs */
		result = qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_QOS_WEIGHT_COUNTER, 0, channelIdx, &weightMemXs0);
		if(result<0)
			goto tk7_finish;
		result = qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_QOS_WEIGHT_COUNTER, 1, channelIdx, &weightMemXs1);
		if(result<0)
			goto tk7_finish;
        result = qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_QOS_WEIGHT_COUNTER, 2, channelIdx, &weightMemXs2);
		if(result<0)
			goto tk7_finish;
		result = qdmaGetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_QOS_WEIGHT_COUNTER, 3, channelIdx, &weightMemXs3);
		if(result<0)
			goto tk7_finish;

        weight[0] = weightMemXs0 & 0xFFFF;
        weight[1] = (weightMemXs0>>16) & 0xFFFF;
        weight[2] = weightMemXs1 & 0xFFFF;
        weight[3] = (weightMemXs1>>16) & 0xFFFF;
        weight[4] = weightMemXs2 & 0xFFFF;
        weight[5] = (weightMemXs2>>16) & 0xFFFF;
        weight[6] = weightMemXs3 & 0xFFFF;
        weight[7] = (weightMemXs3>>16) & 0xFFFF;
        weightReg0 = ((TxQos.queue[1].weight)<<16) | (TxQos.queue[0].weight);
		weightReg1 = ((TxQos.queue[3].weight)<<16) | (TxQos.queue[2].weight);
        weightReg2 = ((TxQos.queue[5].weight)<<16) | (TxQos.queue[4].weight);
		weightReg3 = ((TxQos.queue[7].weight)<<16) | (TxQos.queue[6].weight);
		printk("\nchannel:%d-QoS_Reg_Write, queue_7_6_Reg:0x%08x, queue_5_4_Reg:0x%08x, queue_3_2_Reg:0x%08x, queue_1_0_Reg:0x%08x\n", 
                    channelIdx, weightReg3, weightReg2, weightReg1, weightReg0);
        printk("channel:%d-MEM_XS_Read, queue_7_6_Mem_XS:0x%08x, queue_5_4_Mem_XS:0x%08x, queue_3_2_Mem_XS:0x%08x, queue_1_0_Mem_XS:0x%08x\n", 
                    channelIdx, weightMemXs3, weightMemXs2, weightMemXs1, weightMemXs0);

		/*  check read value right or not */
		for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++) {
			if(TxQos.queue[queueIdx].weight != weight[queueIdx]) {
                result = -1 ;
                goto tk7_finish;
			}
		}
	}
		

	/*  Write: dbg mem xs;   Read: general register */
	/*  0x2: QDMA QoS Weight	 */
	for(channelIdx=0; channelIdx<CONFIG_QDMA_CHANNEL; channelIdx++){
		/*  Write: general register */
		for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++) {
			weight[queueIdx] = ((unsigned int)random32())%100;
            if(weight[queueIdx] == 0)
                weight[queueIdx] = 1 ;
			result = qdmaSetDbgMemXsConfig(QDMA_DBG_MEM_XS_MEMSEL_QOS_WEIGHT_COUNTER, queueIdx, channelIdx, weight[queueIdx], 0);
			if(result<0)
				goto tk7_finish;
		}
		
		/*  Read: dbg mem xs */
		TxQos.channel = channelIdx;
		qdma_data.qdma_private.qdma_tx_qos.pTxQos = &TxQos;
		qdma_get_tx_qos(&qdma_data) ;
		if(result<0)
			goto tk7_finish;

        weightMemXs0 = (weight[1]<<16) | weight[0];
        weightMemXs1 = (weight[3]<<16) | weight[2];
        weightMemXs2 = (weight[5]<<16) | weight[4];
        weightMemXs3 = (weight[7]<<16) | weight[6];
        weightReg0 = ((TxQos.queue[1].weight)<<16) | (TxQos.queue[0].weight);
        weightReg1 = ((TxQos.queue[3].weight)<<16) | (TxQos.queue[2].weight);
        weightReg2 = ((TxQos.queue[5].weight)<<16) | (TxQos.queue[4].weight);
        weightReg3 = ((TxQos.queue[7].weight)<<16) | (TxQos.queue[6].weight);
        printk("\nchannel:%d-MEM_XS_Write, queue_7_6_Mem_XS:0x%08x, queue_5_4_Mem_XS:0x%08x, queue_3_2_Mem_XS:0x%08x, queue_1_0_Mem_XS:0x%08x\n", 
                                    channelIdx, weightMemXs3, weightMemXs2, weightMemXs1, weightMemXs0);
        printk("channel:%d-QoS_Reg_Read, queue_7_6_Reg:0x%08x, queue_5_4_Reg:0x%08x, queue_3_2_Reg:0x%08x, queue_1_0_Reg:0x%08x\n", 
                                    channelIdx, weightReg3, weightReg2, weightReg1, weightReg0);

		/*  check read value right or not */
		for(queueIdx=0; queueIdx<CONFIG_QDMA_QUEUE; queueIdx++) {
			if(TxQos.queue[queueIdx].weight != weight[queueIdx]) {
                result = -1 ;
                goto tk7_finish;
			}
		}
	}
		
tk7_finish:
	if(result == 0)
		QDMA_LOG("tk7 test pass.\r\n");
	else
		QDMA_LOG("tk7 test fail.\r\n");
    return 0;
}

static int tc_tc0(uint base, uint packets, uint lens, int ringIdx)
{
    uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;

    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    aveTime=0;
    qdma_st_packet_gen(channel, queue, packets, lens, 0x01, ringIdx) ;
    __st_print_counters() ;
    aveTime = aveTime / packets;
    printk("aveTime is 0x%08x\r\n\r\n", aveTime);
    if(__st_check_counters() == 0)
        QDMA_LOG("tc0 test pass.\r\n");
    else
        QDMA_LOG("tc0 test fail.\r\n");
    return 0;
}

static struct task_struct *polling_test_task;
static int tc_tc1(uint base, uint packets, uint lens, int ringIdx)
{
    uint oldInt1=0, oldInt2=0  ;
    oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ;

    qdmaSetLpbkToRXRing(base, ringIdx);
    qdma_recycle_packet_mode(QDMA_TX_POLLING, 32) ;     
    qdma_receive_packet_mode(QDMA_RX_POLLING) ;
    printk("Case 1: Polling Mode for TX/RX, Pattern:RAND, Lens:%d, Packets:%d\n", lens, packets) ;

    polling_test_task = kthread_run(qdma_st_rx_polling, NULL, "polling_test");
    if(IS_ERR(polling_test_task)){
       printk("create qdma_st_rx_polling thread failed\n");
    }

    //kernel_thread((int (*)(void *))qdma_st_rx_polling, NULL, 0) ;
    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    qdma_st_packet_gen(CONFIG_QDMA_CHANNEL, CONFIG_QDMA_QUEUE, packets, lens, 0x01, ringIdx) ;

    /*  recycle the rest dscp because lower than txThrh */
    msleep(1000) ;
    qdma_bm_transmit_done(0) ;
    msleep(3000) ;
    __st_print_counters() ;
    if(__st_check_counters() == 0) {
        QDMA_LOG("tc1 test pass.\r\n");
    }else{
        QDMA_LOG("tc1 test fail.\r\n");
    }
    qdmaClearIntStatus1(base, 0xFFFFFFFF) ;
	qdmaClearIntStatus2(base, 0xFFFFFFFF) ;
    qdma_recycle_packet_mode(QDMA_TX_INTERRUPT, 0) ;        
    qdma_receive_packet_mode(QDMA_RX_NAPI) ;
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);
    return 0;
}

static int tc_loopback_basic_test_by_ring_idx(uint base, uint txRingIdx , uint rxRingIdx)
{
    uint randomValue = 0 ;
    uint channel = 0 ;
    uint queue = 0 ;
    unchar curPattern=0, pattern[5] = {0x00, 0xFF, 0x5A, 0xA5, 0x01} ;
    uint i=1, curPktCnt=0, rx_done_int_cnt=0 ;
	uint result=0 ;
    uint rx_idx=0,tx_idx=0;

    printk("\n----------------LoopBack AutoTest Start----------------\n");
    if( (txRingIdx>TX_RING_NUM) || (rxRingIdx>RX_RING_NUM) )
        QDMA_ERR("Error RingIdx: TX-0~%d, RX-0~%d\n", TX_RING_NUM, RX_RING_NUM) ;

    randomValue = ((unsigned int)random32());
    channel = randomValue % CONFIG_QDMA_CHANNEL ;
    randomValue = ((unsigned int)random32());
    queue = randomValue % CONFIG_QDMA_QUEUE ;
    printk("channel = 0x%X, queue = 0x%X\n",channel,queue);

    for( tx_idx=0; tx_idx<TX_RING_NUM; tx_idx++)
    {
        if( (tx_idx!=txRingIdx) && (txRingIdx!=TX_RING_NUM) )
            continue;

        for( rx_idx=0; rx_idx<RX_RING_NUM; rx_idx++)
        {
            if( (rx_idx!=rxRingIdx) && (rxRingIdx!=RX_RING_NUM) )
                continue;
            
        	qdmaSetLpbkToRXRing(base, rx_idx);
            memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    		gpQdmaPriv->counters.intRxDone[rx_idx] = 0 ;
            randomValue = ((unsigned int)random32());
            curPktCnt = 16;
    		//curPktCnt = 1+(randomValue%100) ;
            randomValue = ((unsigned int)random32());
    		curPattern = randomValue%5 ;
            printk("curPktCnt = %X, curPattern_idx = %X\n",curPktCnt,curPattern);
            qdma_st_packet_gen(channel, queue, curPktCnt, 0, pattern[curPattern], tx_idx) ; 
            msleep(1000) ;
    		rx_done_int_cnt = gpQdmaPriv->counters.intRxDone[rx_idx] ;
            printk("%s: txRing:%d, rxRing:%d, Pattern:%.2x, Lens:48~256, Tx Pkts:%d, Tx OK Pkts:%d, Rx OK Pkts:%d, Rx_DONE_INT:%d, tx_cpu_idx:%.8x, tx_dma_idx:%.8x", 
               ((i==0)?("Delay"):("No Delay")), tx_idx, rx_idx , pattern[curPattern], curPktCnt, counters[channel][queue].tx_frames, counters[channel][queue].rx_frames, rx_done_int_cnt, IO_GREG(QDMA_CSR_TX_CPU_IDX(base, tx_idx)), IO_GREG(QDMA_CSR_TX_DMA_IDX(base, tx_idx))) ;
    		if((curPktCnt == counters[channel][queue].rx_frames)
    			&& ((i==0)?(curPktCnt == rx_done_int_cnt):(curPktCnt >= rx_done_int_cnt))){
                printk(", Test Result:Pass\n\n") ;
            } else {
                printk(", Test Result:Fail!!****\n\n") ;
                result = -1;
                goto out_result;
            }
        	
        }
    }

out_result:
    if(result == 0)
        QDMA_LOG("loopback basic test pass.\r\n");
    else
        QDMA_LOG("loopback basic test fail.\r\n");

    return 0;
}

static int tc_interrupt_test(uint base, uint mask)
{
    uint oldInt = 0, oldRxDly = 0, prevIntRxDone = 0 ;
    uint channel = ((unsigned int)random32()) % CONFIG_QDMA_CHANNEL ;
    uint queue = ((unsigned int)random32()) % CONFIG_QDMA_QUEUE ;
    unchar pattern[5] = {0x00, 0xFF, 0x5A, 0xA5, 0x01} ;
    int curPktCnt=0, intNums=0 ;
    uint itemMax[2] = {5, 10} ;
    uint i=0, j=0, txRingIdx=0, rxRingIdx=0 ;
	int flag=0, result=0, finalResult=0 ;
    uint csr[11] = {0, ((12<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((25<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((50<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((63<<DLY_INT_RXMAX_PINT_SHIFT) | (0<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                    0, ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (40<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (80<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (200<<DLY_INT_RXMAX_PTIME_SHIFT)), 
                      ((0<<DLY_INT_RXMAX_PINT_SHIFT) | (255<<DLY_INT_RXMAX_PTIME_SHIFT))} ;

	printk("mask:%d\n", mask);
	if((mask & 0x8) == 0)
		goto next2;
	/* RX Delay Interrupt Test */
	printk("\n/******************************  RX Delay Interrupt Test Begin  ******************************/\n");
	for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) {
		qdmaSetLpbkToRXRing(base, rxRingIdx);
		oldRxDly = qdmaGetRxDelayIntCfg(base, rxRingIdx) ;
		for(txRingIdx=0; txRingIdx<TX_RING_NUM; txRingIdx++) {
			flag=0;
			memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
		    for(i=0, j=0 ; j<2 ; j++) {
				printk("\nrxRingIdx:%d, txRingIdx:%d, %s Test: %s incress, RX_INT Count should decress!\n" \
		            , rxRingIdx, txRingIdx, ((j==0)?("RX_PINT"):("RX_PTIME")), ((j==0)?("PINT"):("PTIME")));
				curPktCnt = 500+((unsigned int)random32())%500;
		        for( ; i<itemMax[j] ; i++) {
		            qdmaSetRxDelayIntCfg(base, rxRingIdx, csr[i]) ;
		            gpQdmaPriv->counters.intRxDone[rxRingIdx] = 0 ;
		            qdma_st_packet_gen(channel, queue, curPktCnt, 0, pattern[((unsigned int)random32())%5], txRingIdx) ;
		            msleep(100) ;
		            printk("Rx Delay Pkt:%d, Time:%d*20us, DlyCSR:%.8x, tx Packets:%d, RX_INT Count:%d\n", 
		                                        ((qdmaGetRxDelayIntCfg(base, rxRingIdx)&DLY_INT_RXMAX_PINT_MASK)>>DLY_INT_RXMAX_PINT_SHIFT) ,
		                                        ((qdmaGetRxDelayIntCfg(base, rxRingIdx)&DLY_INT_RXMAX_PTIME_MASK)>>DLY_INT_RXMAX_PTIME_SHIFT),
		                                        qdmaGetRxDelayIntCfg(base, rxRingIdx), curPktCnt, gpQdmaPriv->counters.intRxDone[rxRingIdx]) ;
		            if((i != 0) && (i != 5)) {
		                if((gpQdmaPriv->counters.intRxDone[rxRingIdx] > prevIntRxDone) || (gpQdmaPriv->counters.intRxDone[rxRingIdx]==0))
		                   flag = -1;
		            }               
		            prevIntRxDone = gpQdmaPriv->counters.intRxDone[rxRingIdx] ;
		            msleep(2000) ;
		        }
		    }
		    __st_print_counters() ;
			if(flag == 0) {
				printk("****rxRingIdx:%d, txRingIdx:%d, RX Delay Interrupt Test Pass!!****\n\n", rxRingIdx, txRingIdx);
			} else {
				printk("****rxRingIdx:%d, txRingIdx:%d, RX Delay Interrupt Test Fail!!****\n\n", rxRingIdx, txRingIdx);
				finalResult = result = -1;
			}
		}
   		qdmaSetRxDelayIntCfg(base, rxRingIdx, oldRxDly) ;
	}

    if(result == 0)
        QDMA_LOG("interrupt test: RX Delay Test Pass\n\n") ;
    else
        QDMA_LOG("interrupt test: RX Delay Test Fail\n\n") ;


next2:
	if((mask & 0x4) == 0)
		goto next3;
	/* Tx Done Interrupt & Rx Done Interrupt Test */
	printk("\n/******************************  TX/RX Done Interrupt Test Begin  ******************************/\n\n");
    oldInt=qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	result=0 ;	
	for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) {
		qdmaSetLpbkToRXRing(base, rxRingIdx);
		for(txRingIdx=0; txRingIdx<TX_RING_NUM; txRingIdx++) {
			for(i=0; i<2; i++) {
				if(i==0) {
					qdmaEnableInt(base, (INT_MASK_IRQ_INT|(1<<(16+rxRingIdx))), QDMA_INT1, QDMA_INT_ENABLE1) ;
				} else {
					qdmaDisableInt(base, (INT_MASK_IRQ_INT|(1<<(16+rxRingIdx))), QDMA_INT1, QDMA_INT_ENABLE1) ;
				}
				
	            gpQdmaPriv->counters.intRxDone[rxRingIdx] = 0 ;
	            gpQdmaPriv->counters.intTxDone[RING_IDX_0] = 0 ;
	            qdma_st_packet_gen(channel, queue, 1, 0, pattern[((unsigned int)random32())%5], txRingIdx) ;
	            msleep(100) ;
				printk("rxRingIdx:%d, txRingIdx:%d, Done Interrupt %s: intTxDone:%d, intRxDone:%d", rxRingIdx, txRingIdx \
					, ((i==0)?("Enable"):("Disable")), gpQdmaPriv->counters.intTxDone[RING_IDX_0], gpQdmaPriv->counters.intRxDone[rxRingIdx]);
				if(((i==0) && (gpQdmaPriv->counters.intRxDone[rxRingIdx]==1) && (gpQdmaPriv->counters.intTxDone[RING_IDX_0]==2)) 
				|| ((i==1) && (gpQdmaPriv->counters.intRxDone[rxRingIdx]==0) && (gpQdmaPriv->counters.intTxDone[RING_IDX_0]==0))) {
					printk(", Test Result:Pass\r\n");
				} else {
					printk(", Test Result:Fail!!****\r\n");
					finalResult = result = -1;
				}
			}
		}		
	}
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt);
	
    if(result == 0)
        QDMA_LOG("interrupt test: TX/RX Done Interrupt Test Pass\n\n") ;
    else
        QDMA_LOG("interrupt test: TX/RX Done Interrupt Test Fail\n\n") ;

#if 1
next3:
	if((mask & 0x2) == 0)
		goto next4;
	printk("\n/******************************  TX/RX NO DSCP Interrupt Test Begin  ******************************/\n\n");
	/* Tx No DSCP Interrupt & Rx No DSCP Interrupt Test */
    oldInt=qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	qdmaDisableInt(base, INT_MASK_RX_DONE, QDMA_INT1, QDMA_INT_ENABLE1) ;
	qdmaEnableInt(base, INT_MASK_IRQ_INT, QDMA_INT1, QDMA_INT_ENABLE1) ;
	result=0 ;
	for(rxRingIdx=0; rxRingIdx<RX_RING_NUM; rxRingIdx++) {
		qdmaSetLpbkToRXRing(base, rxRingIdx);
		for(txRingIdx=0; txRingIdx<TX_RING_NUM; txRingIdx++) {
			for(i=0; i<2; i++) {
				if(i==0) {
					qdmaEnableInt(base, INT_MASK_NO_RX_CPU_DSCP, QDMA_INT1, QDMA_INT_ENABLE1) ;
				} else {
					qdmaDisableInt(base, INT_MASK_NO_RX_CPU_DSCP, QDMA_INT1, QDMA_INT_ENABLE1) ;
				}
				
				memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
				gpQdmaPriv->counters.intNoRxDscp[rxRingIdx] = 0 ;
				gpQdmaPriv->counters.intNoTxDscp[txRingIdx] = 0 ;
				gpQdmaPriv->devCfg.waitTxMaxNums = 0 ;
				gpQdmaPriv->devCfg.countDown = (((unsigned int)random32())%(gpQdmaPriv->txDscpNum[txRingIdx]-1))+1 ;
				printk("\n1st countDown: %d \n",gpQdmaPriv->devCfg.countDown);
				gpQdmaPriv->devCfg.countDownRound = 0 ;
				curPktCnt = (((unsigned int)random32())%((gpQdmaPriv->rxDscpNum[rxRingIdx]+gpQdmaPriv->txDscpNum[txRingIdx])))+1;
				qdma_st_packet_gen(channel, queue, curPktCnt, 0, pattern[((unsigned int)random32())%5], txRingIdx) ;
				
				/* in order to set the TX_CPU_PTR to HW register */
				gpQdmaPriv->devCfg.countDown = 1 ;
				gpQdmaPriv->devCfg.waitTxMaxNums = 1 ;
				qdma_st_packet_gen(channel, queue, 1, 0, pattern[((unsigned int)random32())%5], txRingIdx) ;
				qdma_bm_receive_packets(0, rxRingIdx) ;
				msleep(1000) ;
				intNums = max(0, (int)(curPktCnt+1-(CONFIG_RX_DSCP_NUM[rxRingIdx]-3)+1));
				printk("rxRingIdx:%d, txRingIdx:%d, NO_TX_RX_DSCP_INT:%s, Packets:%d, expected rxIntNums:%d, NO_RX_DSCP_INT Cnt:%d, NO_TX_DSCP_INT Cnt:%d, CntDownRound:%d" \
					, rxRingIdx, txRingIdx, ((i==0)?("Enable"):("Disable")), curPktCnt+1, intNums, gpQdmaPriv->counters.intNoRxDscp[rxRingIdx] \
					, gpQdmaPriv->counters.intNoTxDscp[txRingIdx], gpQdmaPriv->devCfg.countDownRound) ;

				if(((i==0) && (__st_check_counters() == 0) && (gpQdmaPriv->counters.intNoRxDscp[rxRingIdx] == intNums) && 
					(gpQdmaPriv->counters.intNoTxDscp[txRingIdx] == gpQdmaPriv->devCfg.countDownRound))
				 || ((i==1) && (__st_check_counters() == 0) && (gpQdmaPriv->counters.intNoRxDscp[rxRingIdx] == 0) && 
					(gpQdmaPriv->counters.intNoTxDscp[txRingIdx] == 0))) {
					printk(", Test Result:Pass\n\n");
				} else {
					printk(", Test Result:Fail!!****\n\n");
					finalResult = result = -1;
				}
				__st_print_counters() ;
			}
		}
	}
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt);
#endif
	
    if(result == 0)
        QDMA_LOG("interrupt test: TX/RX No DSCP Interrupt Test Pass\n\n") ;
    else
        QDMA_LOG("interrupt test: TX/RX No DSCP Interrupt Test Fail\n\n") ;
	
next4:
	if((mask & 0x1) == 0)
		goto next5;

	/* Tx Coherent Interrupt & Rx Coherent Interrupt Test */
	printk("\n/******************************  TX/RX Coherent Interrupt Test Begin  ******************************/\n\n");
	if(tc_done_bit_check_test(base) != 0) {
		finalResult = -1;
	}
	
next5:
    if(finalResult == 0)
        QDMA_LOG("interrupt test pass.\r\n");
    else
        QDMA_LOG("interrupt test fail.\r\n");
	
    return 0;
}

static int tc_stress_test(void)
{
    unchar pattern[5] = {0x00, 0xFF, 0x5A, 0xA5, 0x01} ;
    uint i=0, j=0, k=0 ;
	uint ringIdx=0, result=0 ;
    printk("Case 2: (Channel, Queue, Pattern, Lens) Any Combination, Packets:%d\n", 
        100000*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE*5) ;
    for(i=0 ; i<CONFIG_QDMA_CHANNEL ; i++) {
        for(j=0 ; j<CONFIG_QDMA_QUEUE ; j++) {
            for(k=0 ; k<5 ; k++) {
                for(ringIdx=0 ; ringIdx<TX_RING_NUM ; ringIdx++) {
	                memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
	                qdma_st_packet_gen(i, j, 100000, 1, pattern[k], ringIdx) ;
	                msleep(1000) ;
                    printk("Channel:%d, Queue:%d, ring:%d, Pattern:%.2x, Lens:48~2000, Tx OK Packets:%d, Rx OK Packets:%d", 
                        i, j, ringIdx, pattern[k], counters[i][j].tx_frames, counters[i][j].rx_frames) ;
	                if((counters[i][j].tx_frames == 100000) && (counters[i][j].rx_frames == 100000)) {
	                    printk(", Test Result:Pass\n\n") ;
	                } else {
	                    printk(", Test Result:Failed****\n\n") ;
	                    result = -1;
	                }
                }
            }
        }
    }
    if(result == 0)
        QDMA_LOG("stress test pass.\r\n");
    else
        QDMA_LOG("stress test fail.\r\n");
    return 0;
}

static int tc_tc2(uint base, uint packets, uint lens, int ringIdx)
{
    unchar pattern[5] = {0x00, 0xFF, 0x5A, 0xA5, 0x01} ;
    uint i=0, j=0, k=0 ;
	uint result=0 ;

    printk("Case 2: (Channel, Queue, Pattern, Lens) Any Combination, Packets:%d\n", packets*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE*3) ;
    for(i=0 ; i<CONFIG_QDMA_CHANNEL ; i++) {
        for(j=0 ; j<CONFIG_QDMA_QUEUE ; j++) {
            for(k=0 ; k<3 ; k++) {
                memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
                qdma_st_packet_gen(i, j, packets, 1, pattern[k], ringIdx) ;
                msleep(1000) ;
                if(counters[i][j].tx_frames == counters[i][j].rx_frames) {
                    printk("Channel:%d, Queue:%d, Pattern:%.2x, Lens:48~256, Packets:%d, Test Result:Pass\n\n", 
                        i, j, pattern[k], counters[i][j].tx_frames) ;
                } else {
                    printk("Channel:%d, Queue:%d, Pattern:%.2x, Lens:48~256, Packets:%d, Test Result:Failed\n\n", 
                        i, j, pattern[k], counters[i][j].tx_frames) ;
                    result = -1;
                }
            }
        }
    }
    if(result == 0)
        QDMA_LOG("tc2 test pass.\r\n");
    else
        QDMA_LOG("tc2 test fail.\r\n");
    return 0;
}

#if defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_MT7520)
static int tc_tc2_epon(uint base, uint packets, uint lens, int ringIdx)
{
    unchar pattern[5] = {0x00, 0xFF, 0x5A, 0xA5, 0x01} ;
    uint i=0, j=0, k=0 ;
	uint result=0 ;
	int status=0 ;

    printk("Case 2_epon: (Channel, Queue, Pattern, Lens) Any Combination, Packets:%d\n", 100000*16*9*5) ;
    status = 0;
    pon_loopback_result = 0;
    for(i=0 ; i<1 ; i++) {
        for(j=0 ; j<1 ; j++) {
            for(k=0 ; k<5 ; k++) {
                memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
                qdma_st_packet_gen(i, j, packets, lens, pattern[k], ringIdx) ;
                if(packets>100)
                    msleep(2000) ;
                else
                    msleep(100);
                if((counters[i][j].tx_frames == counters[i][j].rx_frames)
                    && (counters[i][j].tx_frames != 0)){
                    printk("Channel:%d, Queue:%d, Pattern:%.2x, Lens:64~2000, Packets:%d, Test Result:Pass\n\n", 
                        i, j, pattern[k], counters[i][j].tx_frames) ;
                } else {
                    status ++;
                    printk("Channel:%d, Queue:%d, Pattern:%.2x, Lens:64~2000, TxPackets:%d,RxPacktes:%d Test Result:Failed\n\n", 
                        i, j, pattern[k], counters[i][j].tx_frames,counters[i][j].rx_frames) ;
                }
            }
        }
    }
    if(status == 0){
        QDMA_LOG("pon loopback test pass\n");
        pon_loopback_result = 1 ;
    }
    else{
        QDMA_LOG("pon loopback test fail\n");
        pon_loopback_result = 0;
    }
    return 0;
}
#endif 

static int tc_tc3(uint base, uint packets, uint lens, int ringIdx)
{
    unchar pattern[5] = {0x00, 0xFF, 0x5A, 0xA5, 0x01} ;
    uint i = 0 ;
    
    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    printk("Case 3: RX_2B_Offset:%s, Channel:RAND, Queue:RAND, Lens:Sequential, Pattern:0x00, 0xFF, 0x5A, 0xA5, RAND, Packets:%d\n", 
        (qdmaIsSetRx2bOffset(base))?"Enable":"Disable", packets) ;
    for(i=0 ; i<5 ; i++) {
        qdma_st_packet_gen((((unsigned int)random32()) % CONFIG_QDMA_CHANNEL), (((unsigned int)random32()) % CONFIG_QDMA_QUEUE), (packets/5), 1, pattern[i], ringIdx) ;
        msleep(1500) ;
        if(__st_check_counters() == 0) {
            printk("Pattern:%.2x, Packets:%d, Test Result:Pass\n\n", pattern[i], packets/5) ;
        } else {
            printk("Pattern:%.2x, Packets:%d, Test Result:Failed\n\n", pattern[i], packets/5) ;
        }
    }
    __st_print_counters() ;
    if( __st_check_counters() == 0 )
        QDMA_LOG("tc3 test pass.\r\n");
    else
        QDMA_LOG("tc3 test fail.\r\n");
    return 0;
}


#if 0
static int tc_tc4(uint base, uint packets, uint lens, int ringIdx)
{
    unchar bs[4] = {VAL_BST_4_DWORD, VAL_BST_8_DWORD, VAL_BST_16_DWARD, VAL_BST_32_DWARD} ;
    uint i = 0 ;

    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    printk("Case 4: Pattern:RAND, Lens:RAND, Burst Size:4, 8, 16, 32, Packets:%d\n", packets) ;
    for(i=0 ; i<4 ; i++) {
        qdmaSetBurstSize(base, bs[i])   ;
        msleep(1000) ;
        
        qdma_st_packet_gen(CONFIG_QDMA_CHANNEL, CONFIG_QDMA_QUEUE, packets/4, 0, 0x01, ringIdx) ;
        msleep(1000) ;
        if(__st_check_counters() == 0) {
            printk("Burst Size:%d, Packets:%d, Test Result:Pass\n\n", qdmaGetBurstSize(base), packets/4) ;
        } else {
            printk("Burst Size:%d, Packets:%d, Test Result:Failed\n\n", qdmaGetBurstSize(base), packets/4) ;
        }
    }
    __st_print_counters() ;
    if(__st_check_counters() == 0)
        QDMA_LOG("tc4 test pass.\r\n");
    else
        QDMA_LOG("tc4 test fail.\r\n");
    return 0;
}
#endif

static int tc_tc5(uint base, uint packets, uint lens, int ringIdx)
{
    uint grade[5] = {100, 200, 400, 1000, 0} ;
    uint i = 0 ;

    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    for(i=0 ; i<5 ; i++) {
    	if(grade[i] >= gpQdmaPriv->txDscpNum[ringIdx])
    		continue ;
    		 
    	gpQdmaPriv->devCfg.waitTxMaxNums = (grade[i]>=gpQdmaPriv->txDscpNum[ringIdx]) ? (gpQdmaPriv->txDscpNum[ringIdx]-1) : (grade[i]) ;
    	gpQdmaPriv->devCfg.countDown = (gpQdmaPriv->devCfg.waitTxMaxNums) ? (gpQdmaPriv->devCfg.waitTxMaxNums) : ((((unsigned int)random32())%(gpQdmaPriv->txDscpNum[ringIdx]-1))+1) ;
    	printk("Case 5-%d: TX_DSCP Nums:%d, Pattern:RAND, Lens:RAND, Packets:%d, waitTxMaxNums:%d, countDown:%d\n", i+1, grade[i], packets, gpQdmaPriv->devCfg.waitTxMaxNums, gpQdmaPriv->devCfg.countDown) ;
    	qdma_st_packet_gen(CONFIG_QDMA_CHANNEL, CONFIG_QDMA_QUEUE, packets/5, lens, 0x01, ringIdx) ;

    	/* in order to set the TX_CPU_PTR to HW register */
    	gpQdmaPriv->devCfg.countDown = 1 ;
    	gpQdmaPriv->devCfg.waitTxMaxNums = 1 ;
    	qdma_st_packet_gen(CONFIG_QDMA_CHANNEL, CONFIG_QDMA_QUEUE, 1, lens, 0x01, ringIdx) ;
    	msleep(1000) ;
    	if(__st_check_counters() == 0) {
    		printk("Grade Nums:%d, Packets:%d, Test Result:Pass\n\n", grade[i], packets/5) ;
    	} else {
    		printk("Grade Nums:%d, Packets:%d, Test Result:Failed\n\n", grade[i], packets/5) ;
    	}
    }
    __st_print_counters() ;
    if(__st_check_counters() == 0)
        QDMA_LOG("tc5 test pass.\r\n");
    else
        QDMA_LOG("tc5 test fail.\r\n");

    gpQdmaPriv->devCfg.countDown = 1 ;
    gpQdmaPriv->devCfg.waitTxMaxNums = 1 ;
    return 0;
}

#if 1 /*modify for EN7580, IRQ overflow verify*/
static int tc_tc6(uint base, uint packets, uint lens, int ringIdx)
{
    uint depth=0, entryIndex=0 ;
    uint i=0, j=0 ;
    uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
    uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ; 

    depth = qdmaGetIrqDepth(base) ;
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, INT_MASK_IRQ_FULL);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, INT_MASK_RX_DONE);
    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;

    printk("Case 6: IRQ Queue Depth:%d, Pattern:RAND, Lens:RAND, Packets:%d\n", depth, packets) ;
    qdma_st_packet_gen(CONFIG_QDMA_CHANNEL, CONFIG_QDMA_QUEUE, packets, 0, 0x01, ringIdx) ;
    msleep(1000) ;
    printk("IRQ_Entry_Len: %d\n", qdmaGetIrqEntryLen(base)) ;
    printk("recycle the rest tx dscp\n") ;
    qdma_bm_transmit_done(0) ;
    printk("IRQ_Entry_Len: %d\n", qdmaGetIrqEntryLen(base)) ;
   
    __st_print_counters() ;
    if(__st_check_counters() == 0)
        QDMA_LOG("tc6 test pass.\r\n");
    else
        QDMA_LOG("tc6 test fail.\r\n");

    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);
    return 0;
}
#else
static int tc_tc6(uint base, uint packets, uint lens, int ringIdx)
{
    uint grade[3] = {128, 300, 512} ;
    uint depth=0, entryIndex=0 ;
    uint i=0, j=0 ;
    uint oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ; 
    uint oldInt2 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2) ; 

    depth = qdmaGetIrqDepth(base) ;
    printk("----IRQ depth : %d----\n", depth);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, INT_MASK_IRQ_FULL);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, INT_MASK_RX_DONE);

    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    for(i=0 ; i<1 ; i++) {
        if(grade[i] >= gpQdmaPriv->txDscpNum[ringIdx])
            continue ;
        /*make sure entryIndex not overflow*/
        entryIndex = qdmaGetIrqEntryLen(base);
        if(entryIndex >= grade[i]) {
            j=0;
            while(qdmaGetIrqEntryLen(base) < grade[i]) {
                qdma_st_packet_gen(CONFIG_QDMA_CHANNEL, CONFIG_QDMA_QUEUE, 1, 0, 0x01, ringIdx) ;
                msleep(100);
                j++;
            }
            printk("entryIndex:%d, grade[%d]:%d, sent Packet:%d\n", entryIndex, i, grade[i], j);
        }
            
        qdmaSetIrqDepth(base, grade[i]) ;
        printk("Case 6-%d: IRQ Queue Depth:%d, Pattern:RAND, Lens:RAND, Packets:%d\n", i+1, grade[i], packets) ;
        qdma_st_packet_gen(CONFIG_QDMA_CHANNEL, CONFIG_QDMA_QUEUE, packets/3, 0, 0x01, ringIdx) ;
        msleep(1000) ;
        printk("IRQ_Entry_Len: %d\n", qdmaGetIrqEntryLen(base)) ;
        qdma_bm_transmit_done(0) ;
        __st_print_counters() ;
        printk("IRQ_Entry_Len: %d\n", qdmaGetIrqEntryLen(base)) ;
        if(__st_check_counters() == 0) {
            printk("Grade Nums:%d, Packets:%d, Test Result:Pass\n", grade[i], packets/3) ;
        } else {
            printk("Grade Nums:%d, Packets:%d, Test Result:Failed\n", grade[i], packets/3) ;
        }
    }
    __st_print_counters() ;
    printk("IRQ_Entry_Len: %d\n", qdmaGetIrqEntryLen(base)) ;
    if(__st_check_counters() == 0)
        QDMA_LOG("tc6 test pass.\r\n");
    else
        QDMA_LOG("tc6 test fail.\r\n");

    qdmaSetIrqDepth(base, depth) ;
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE2, oldInt2);
    return 0;
}
#endif

static int tc_tc7(uint base, uint packets, uint lens, int ringIdx, QDMA_InterruptNo_t intNum)
{
    uint oldInt1=0, oldInt2=0, oldInt3=0, oldInt4=0;
	uint curInt1=0, curInt2=0, curInt3=0, curInt4=0;

	if((intNum < QDMA_INT1) || (intNum > QDMA_INT4))
		intNum = QDMA_INT1;

	/* store old INT mask */
    oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	oldInt2 = qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1) ;
	oldInt3 = qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1) ;
	oldInt4 = qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1) ;

	/* clean IRQ mask */
	curInt1 = oldInt1 & (~(INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT));
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, curInt1);
	curInt2 = oldInt2 & (~(INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT));
    qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, curInt2);
	curInt3 = oldInt3 & (~(INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT));
    qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, curInt3);
	curInt4 = oldInt4 & (~(INT_MASK_IRQ_FULL | INT_MASK_IRQ_INT));
    qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, curInt4);

	/* generate packets */
    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    qdma_st_packet_gen((((unsigned int)random32()) % CONFIG_QDMA_CHANNEL), (((unsigned int)random32()) % CONFIG_QDMA_QUEUE), packets, 1, 1, ringIdx) ;
    msleep(1000) ;	
    printk("Packets:%d sent done\n\n", packets) ;

	/* enable IRQ FULL INT */
	switch(intNum) {
		case QDMA_INT1:
			curInt1 = oldInt1 | INT_MASK_IRQ_FULL;
    		qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, curInt1);
			break;
		case QDMA_INT2:
			curInt2 = oldInt2 | INT_MASK_IRQ_FULL;
    		qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, curInt2);
			break;
		case QDMA_INT3:
			curInt3 = oldInt3 | INT_MASK_IRQ_FULL;
    		qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, curInt3);
			break;
		case QDMA_INT4:
			curInt4 = oldInt4 | INT_MASK_IRQ_FULL;
    		qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, curInt4);
			break;
		default:
			curInt1 = oldInt1 | INT_MASK_IRQ_FULL;
    		qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, curInt1);
			break;
	}
	printk("INT1_ENABLE1:0x%08X, INT2_ENABLE1:0x%08X, INT3_ENABLE1:0x%08X, INT4_ENABLE1:0x%08X\n", curInt1, curInt2, curInt3, curInt4) ;
    msleep(1000) ;
    printk("INT_MASK_IRQ_FULL interrupt trigger done\n\n") ;

	/* check the result */
    if(__st_check_counters() == 0) {
        printk("Pattern:%.2x, Packets:%d, Test Result:Pass\n\n", 1, packets) ;
    } else {
        printk("Pattern:%.2x, Packets:%d, Test Result:Failed\n\n", 1, packets) ;
    }
    
    __st_print_counters() ;
    if(__st_check_counters() == 0)
        QDMA_LOG("tc7 test pass.\r\n");
    else
        QDMA_LOG("tc7 test fail.\r\n");

	/* restore old INT mask */
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
	qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, oldInt2);
	qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, oldInt3);
	qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, oldInt4);
    return 0;
}

#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
static int tc_tc7_irq2(uint base, uint packets, uint lens, int ringIdx, QDMA_InterruptNo_t intNum)
{
    uint oldInt1=0, oldInt2=0, oldInt3=0, oldInt4=0;
	uint curInt1=0, curInt2=0, curInt3=0, curInt4=0;

	if((intNum < QDMA_INT1) || (intNum > QDMA_INT4))
		intNum = QDMA_INT1;

	/* store old INT mask */
    oldInt1 = qdmaGetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1) ;
	oldInt2 = qdmaGetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1) ;
	oldInt3 = qdmaGetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1) ;
	oldInt4 = qdmaGetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1) ;

	/* clean IRQ mask */
	curInt1 = oldInt1 & (~(INT_MASK_IRQ2_FULL | INT_MASK_IRQ2_INT));
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, curInt1);
	curInt2 = oldInt2 & (~(INT_MASK_IRQ2_FULL | INT_MASK_IRQ2_INT));
    qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, curInt2);
	curInt3 = oldInt3 & (~(INT_MASK_IRQ2_FULL | INT_MASK_IRQ2_INT));
    qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, curInt3);
	curInt4 = oldInt4 & (~(INT_MASK_IRQ2_FULL | INT_MASK_IRQ2_INT));
    qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, curInt4);

	/* generate packets */
    memset(counters, 0, sizeof(QDMA_DbgCounters_T)*CONFIG_QDMA_CHANNEL*CONFIG_QDMA_QUEUE) ;
    qdma_st_packet_gen((((unsigned int)random32()) % CONFIG_QDMA_CHANNEL), (((unsigned int)random32()) % CONFIG_QDMA_QUEUE), packets, 1, 1, ringIdx) ;
    msleep(1000) ;	
    printk("Packets:%d sent done\n\n", packets) ;

	/* enable IRQ FULL INT */
	switch(intNum) {
		case QDMA_INT1:
			curInt1 = oldInt1 | INT_MASK_IRQ2_FULL;
    		qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, curInt1);
			break;
		case QDMA_INT2:
			curInt2 = oldInt2 | INT_MASK_IRQ2_FULL;
    		qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, curInt2);
			break;
		case QDMA_INT3:
			curInt3 = oldInt3 | INT_MASK_IRQ2_FULL;
    		qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, curInt3);
			break;
		case QDMA_INT4:
			curInt4 = oldInt4 | INT_MASK_IRQ2_FULL;
    		qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, curInt4);
			break;
		default:
			curInt1 = oldInt1 | INT_MASK_IRQ2_FULL;
    		qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, curInt1);
			break;
	}
	printk("INT1_ENABLE1:0x%08X, INT2_ENABLE1:0x%08X, INT3_ENABLE1:0x%08X, INT4_ENABLE1:0x%08X\n", curInt1, curInt2, curInt3, curInt4) ;
    msleep(1000) ;
    printk("INT_MASK_IRQ_FULL interrupt trigger done\n\n") ;

	/* check the result */
    if(__st_check_counters() == 0) {
        printk("Pattern:%.2x, Packets:%d, Test Result:Pass\n\n", 1, packets) ;
    } else {
        printk("Pattern:%.2x, Packets:%d, Test Result:Failed\n\n", 1, packets) ;
    }
    
    __st_print_counters() ;
    if(__st_check_counters() == 0)
        QDMA_LOG("tc7 test pass.\r\n");
    else
        QDMA_LOG("tc7 test fail.\r\n");

	/* restore old INT mask */
    qdmaSetIntMask(base, QDMA_INT1, QDMA_INT_ENABLE1, oldInt1);
	qdmaSetIntMask(base, QDMA_INT2, QDMA_INT_ENABLE1, oldInt2);
	qdmaSetIntMask(base, QDMA_INT3, QDMA_INT_ENABLE1, oldInt3);
	qdmaSetIntMask(base, QDMA_INT4, QDMA_INT_ENABLE1, oldInt4);
    return 0;
}
#endif

static int qdma_st_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64], cmd[32], subcmd[32] ;
	uint value=0 ;
	uint packets=0, lens=0 ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	uint ptime=0 ;
	int ringIdx=0 ;
	QDMA_InterruptNo_t intNum = QDMA_INT1;
	int rxRingIdx = 0 , txRingIdx = 0 ;
	struct ECNT_QDMA_Data qdma_data;
#if defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_MT7520)
	int status=0 ;
#endif

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
    
	sscanf(val_string, "%s %s %d %d %d %d", cmd, subcmd, &value, &msec, &ringIdx, &intNum) ;
	packets = simple_strtol(subcmd, NULL, 0) ; 
	nlen = lens = value ;
	printk("msec is %d\n", msec);
	txRingIdx = msec ;
	rxRingIdx = ringIdx ;

	selfTestFlag=1;	
	bmRecvCallbackFunction = gpQdmaPriv->devCfg.bmRecvCallbackFunction;
	bmEventCallbackFunction = gpQdmaPriv->devCfg.bmEventCallbackFunction;
	if(strcmp(cmd, "init") != 0) {
		//qdma_register_callback_function(ECNT_QDMA, QDMA_CALLBACK_TX_FINISHED, qdma_st_cb_tx_finished) ;
		qdma_data.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RX_PACKET;
		qdma_data.qdma_private.qdma_reg_cb.pCbFun = qdma_st_cb_rx_packet;
		qdma_register_callback_function(&qdma_data) ;
		qdma_data.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EVENT_HANDLER;
		qdma_data.qdma_private.qdma_reg_cb.pCbFun = qdma_st_cb_EventHandler;
		qdma_register_callback_function(&qdma_data) ;
	}
	
	ptime = qdmaGetIrqPtime(base);
	if(!strcmp(cmd, "init")) {
		tc_init(subcmd, base);
	} 

    else if(!strcmp(cmd, "udelay")){
		delayTime = packets;  /*us*/
        printk("delayTime = %d us.\r\n", delayTime);
	}

    /* for other csr verification */
    else if(!strcmp(cmd, "csr")) {
        if(!strcmp(subcmd, "intmask")) {
            qdma_st_csr_intmask(value) ;    
        }
    }

	/* for ratelimit test */
    else if(!strcmp(cmd, "ratelimit_test")) {
		tc_ratelimit_test(base , packets) ;    
    }
    
	/* for NO_RX_CPU_DSCP_INT and NO_TX_CPU_DSCP_INT verification */
	else if(!strcmp(cmd, "csr1")) {
		tc_csr1(base, packets, lens, txRingIdx, rxRingIdx);
	}
    
	/* for NO_LINK_DSCP verification */
	else if(!strcmp(cmd, "csr2")) {
		tc_csr2(base, packets, lens, ringIdx);
	}

	/* for LOW_LINK_DSCP verification */
	else if(!strcmp(cmd, "csr3")) {
		tc_csr3(base, packets, lens, ringIdx);
	}

	/* CHECK_DONE bit enable/disable verification  */
	else if(!strcmp(cmd, "done_bit_check_test")) {
        tc_done_bit_check_test(base);
	} 
    
	/* for RX_DONE_INT and RX_DLY_INT verification */
	else if(!strcmp(cmd, "csr4")) {
        tc_csr4(base, packets, lens, ringIdx);
	} 

#if 0
    /* for TX_DONE_INT and TX_DLY_INT verification */
    else if(!strcmp(cmd, "csr5")) {
        tc_csr5(base, packets, lens, ringIdx);
    }
#endif

	else if(!strcmp(cmd, "tk0")) {	/* QDMA Info Test */
		uint qdma_info = 0 ;
		qdma_info = IO_GREG(QDMA_CSR_INFO(base));
		
#if	defined(TCSUPPORT_CPU_EN7581)
		if(qdma_info == 0x107) /*EN7581*/
#elif defined(TCSUPPORT_CPU_EN7523)
		if(qdma_info == 0x106) /*EN7523*/
#else
		if(qdma_info == 0x105) /*EN7580*/
#endif
			printk("tk0 test pass.\r\n");
		else
			printk("tk0 test fail.\r\n");
	}
	else if(!strcmp(cmd, "tk1")) {	/* 32 channel test */
	    tc_tk1(base, packets, lens, ringIdx);
	}
	else if(!strcmp(cmd, "irq_test")) {	/* IRQ_EN test */
	    tc_irq_test(base, packets, lens, ringIdx);
	}
	else if(!strcmp(cmd, "tk3")) {	/* loop_cnt & dbg_cntr test */
	    tc_tk3(base, packets, lens, ringIdx);
	}
    else if(!strcmp(cmd, "tk4")) {  /* CPU TX/RX chain basic test */
        tc_tk4(base, packets, lens, ringIdx);
    }
    else if(!strcmp(cmd, "tk5")) {  /* TX high and low chain */
        tc_tk5(base, packets, lens, ringIdx);
    }
#if 0
    else if(!strcmp(cmd, "tk6")) {  /* TX high and low chain */
        tc_tk6(base, packets, lens, ringIdx);
    }
#endif
    else if(!strcmp(cmd, "tk7")) {  /* dbg_mem_xs test */
        tc_tk7(base, packets, lens, ringIdx);
    }
	else if(!strcmp(cmd, "tc0")) {  
        tc_tc0(base, packets, lens, ringIdx);
	} 
	else {
		if(!strcmp(cmd, "tc6") || !strcmp(cmd, "tcall")) {
            tc_tc6(base, packets, lens, ringIdx);
		}  
#ifndef CONFIG_TX_POLLING_BY_MAC
		if(!strcmp(cmd, "tc1") || !strcmp(cmd, "tcall")) {
            tc_tc1(base, packets, lens, ringIdx);
		}  
#endif /* CONFIG_TX_POLLING_BY_MAC */
		if(!strcmp(cmd, "loopback_basic_test") || !strcmp(cmd, "tcall")) {
            tc_loopback_basic_test_by_ring_idx(base, packets, value);
		}
		if(!strcmp(cmd, "interrupt_test") || !strcmp(cmd, "tcall")) {
            tc_interrupt_test(base, packets);
		}
		if(!strcmp(cmd, "stress_test") || !strcmp(cmd, "tcall")) {
            tc_stress_test();
		}
        
		if(!strcmp(cmd, "tc2") || !strcmp(cmd, "tcall")) {
            tc_tc2(base, packets, lens, ringIdx);
		}

#if defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_MT7520)
		if(!strcmp(cmd, "tc2_epon") || !strcmp(cmd, "tcall")) {
            tc_tc2_epon(base, packets, lens, ringIdx);
		}
#endif
		if(!strcmp(cmd, "tc3") || !strcmp(cmd, "tcall")) {
            tc_tc3(base, packets, lens, ringIdx);
		}
#if 0
		if(!strcmp(cmd, "tc4") || !strcmp(cmd, "tcall")) {
            tc_tc4(base, packets, lens, ringIdx);
		}
#endif
		if(!strcmp(cmd, "tc5") || !strcmp(cmd, "tcall")) {
			tc_tc5(base, packets, lens, ringIdx);
		}
		if(!strcmp(cmd, "tc7") || !strcmp(cmd, "tcall")) {
			tc_tc7(base, packets, lens, ringIdx, intNum);
		}
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
		if(!strcmp(cmd, "tc7_irq2")) {
			tc_tc7_irq2(base, packets, lens, ringIdx, intNum);
		}
#endif
	}
	
	if(strcmp(cmd, "init") != 0) {
		qdmaSetIrqPtime(base, 0x64);
		msleep(1000);
		qdmaSetIrqPtime(base, ptime);
	}

	selfTestFlag=0;	
	qdma_data.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_RX_PACKET;
	qdma_data.qdma_private.qdma_reg_cb.pCbFun = bmRecvCallbackFunction;
	qdma_register_callback_function(&qdma_data) ;
	qdma_data.qdma_private.qdma_reg_cb.type = QDMA_CALLBACK_EVENT_HANDLER;
	qdma_data.qdma_private.qdma_reg_cb.pCbFun = bmEventCallbackFunction;
	qdma_register_callback_function(&qdma_data) ;

	return count ;
}

#endif /* CONFIG_SUPPORT_SELF_TEST */


/******************************************************************************
******************************************************************************/
int qdma_dvt_init(void) 
{
#ifdef CONFIG_SUPPORT_SELF_TEST
	struct proc_dir_entry *qdma_proc = NULL ;
	/* create proc node */
	qdma_proc = create_proc_entry(self_test_path, 0, NULL);
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_st_read_proc;
		qdma_proc->write_proc = qdma_st_write_proc;
	}
#endif /* CONFIG_SUPPORT_SELF_TEST */

	return 0 ;
}


/******************************************************************************
******************************************************************************/
int qdma_dvt_deinit(void) 
{
#ifdef CONFIG_SUPPORT_SELF_TEST
	remove_proc_entry(self_test_path, NULL);
#endif /* CONFIG_SUPPORT_SELF_TEST */

	return 0 ;
}
