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
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/etherdevice.h>
#include <linux/proc_fs.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_qdma.h>

#include "qdma_bmgr.h"
#include "qdma_verify.h"
#include "qdma_ic_dis.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define SWITCH_PROT6_VLAN_CONTROL		(0xbfb5a610)
#define SWITCH_PORT_STAG				(1<<5)
#define DPORT_GDMA1						(1)
#define DPORT_GDMA2						(2)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/


/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int qdma_pkt_test_timer_deinit(void);


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/


/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static int verifyTestFlag = 0;
static int fPort = DPORT_GDMA1;
static int macFormatEn = 0;
static int specialTag = 0;
static int forceRingIdx = 0;

static int qdma_pkt_test_flag = 0;	 /*0: stop generate pkts 1:start generate pkts*/
static unsigned int qdma_pkt_burst_num = 300;
static struct timer_list qdma_pkt_test_timer;

static int dramTestFlag = 0;
#define DRAM_TEST_LEN 128
static unsigned char* dramTestDataP = NULL;
struct sk_buff *skb_rx_dram_test = NULL;
unchar dramTestRxDone = 0;

struct sk_buff *skb_qdma_test = NULL;
atomic_t qdma_pkt_send_cnt;
atomic_t qdma_pkt_recycle_cnt;

static unsigned int qdma_pkt_test_chnl = 0;
static unsigned int qdma_pkt_test_ring = 0;
static unsigned int qdma_pkt_test_sptag = 8;

static unsigned int qdma_pkt_test_wifi_bit = 0;
static unsigned int qdma_pkt_test_tx_mode = 0; /* 0: continues; 1:burst */
static unsigned int qdma_pkt_test_fport = 1;


static unsigned char qdma_pkt_test_udp[68] = 
	{0x00,0x22,0x22,0x00,0x22,0x00,0x00,0x11,0x11,0x00,0x11,0x00,0x81,0x00,0x00,0x01,0x08,0x00,0x45,0x00,
	 0x00,0x32,0x00,0x00,0x00,0x00,0x80,0x11,0x9a,0x20,0xc0,0xa8,0x0f,0x6e,0xc0,0xa8,
	 0x0f,0xdc,0x30,0x3a,0x30,0x39,0x00,0x1e,0xe0,0x7a,0xa1,0xa2,0xa3,0xa4,0x65,0x66,
	 0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76};


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

static void __dump_skb(struct sk_buff *skb, uint pktLen) 
{
	unchar n, *p = (unchar *)skb->data ;
	int i ;

	for(i=0 ; i<pktLen ; i++) {
		n = i & 0x0f ;
		
		if(n == 0x00) 		printk(" \n%.4x: ", i) ;
		else if(n == 0x08) 	printk(" ") ;

		printk("%.2x ", *p++) ; 
	}
	printk("\n") ;
}

static void __dump_skb_for_crx_scatter(struct sk_buff *skb, uint pktLen) 
{
	unchar n, *p = (unchar *)skb->data ;
	int i ;

    p = p-2; /*for 2Byte offset*/

	for(i=0 ; i<pktLen ; i++) {
		n = i & 0x0f ;
		
		if(n == 0x00) 		printk(" \n%.4x: ", i) ;
		else if(n == 0x08) 	printk(" ") ;

		printk("%.2x ", *p++) ; 
	}
	printk("\n") ;
}

int qdma_driver_receive_and_transmit_test(struct QDMA_DscpInfo_S *dscpInfoPtr, QDMA_DMA_DSCP_T *rxDscpPtr, int ringIdx){
	uint base = gpQdmaPriv->csrBaseAddr ;
	txMsgWord0_t txMsgW0;
	txMsgWord1_t txMsgW1;
	txMsgWord2_t txMsgW2;
	int error = 0 ;
	txMsgWord_t txMsg;

	txMsg.pTxMsgW0 = &txMsgW0;
	txMsg.pTxMsgW1 = &txMsgW1;
	txMsg.pTxMsgW2 = &txMsgW2;
	
	if(verifyTestFlag == 1) {			
		skb_put(dscpInfoPtr->skb, rxDscpPtr->ctrl.pkt_len);
#ifdef CONFIG_DEBUG
		if(gpQdmaPriv->devCfg.dbgLevel >= DBG_ST) {
			printk("\n--------QDMA Verify Test RX Start--------\n");
#if 1
			__dump_skb(dscpInfoPtr->skb, rxDscpPtr->ctrl.pkt_len);
#else
			__dump_skb_for_crx_scatter(dscpInfoPtr->skb, rxDscpPtr->ctrl.pkt_len);
#endif
			printk("RXDSCP: done:%d, nls:%d, pkt_len:%d, ",(uint)rxDscpPtr->ctrl.done, (uint)rxDscpPtr->ctrl.nls, (uint)rxDscpPtr->ctrl.pkt_len);
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528) || defined(TCSUPPORT_CPU_EN7517)
			printk("overflow:%d\n", (uint)rxDscpPtr->ctrl.drop_pkt);
#else
			printk("overflow:%d\n", (uint)rxDscpPtr->ctrl.overflow);
#endif
			printk("msg0=%.8X, msg1=%.8X , msg2=%.8X\n", (uint)rxDscpPtr->msg[0], (uint)rxDscpPtr->msg[1], (uint)rxDscpPtr->msg[2]);
			printk("--------QDMA Verify Test RX End--------\n");
		}
#endif			
		/**************** prepare Tx Msg *****************/ 
		txMsgW0.word=0;
		txMsgW1.word=0;
		txMsgW1.fport = fPort;	//default: DPORT_GDMA1 
		if(macFormatEn){
			/* srcmac format: channel:queue:tx_ring:XX:XX:XX */
			txMsgW0.channel = dscpInfoPtr->skb->data[6]&0x1f;
			txMsgW0.queue = dscpInfoPtr->skb->data[7]&0x07;
			forceRingIdx = dscpInfoPtr->skb->data[8]&(TX_RING_NUM-1);
		}else{
			/* default setting */
			txMsgW0.channel = 0;
			txMsgW0.queue = 0;
			forceRingIdx = 0;
		}
#ifdef TCSUPPORT_CPU_EN7580
		if(macFormatEn){
			/* dstmac format: XX:no_drop:meter_g:acnt_g1:acnt_g0:sp_tag */
			if(SUPPORT_NO_DROP_IN_TX_MSG1){
				if(dscpInfoPtr->skb->data[1] > 0)
					txMsgW1.no_drop = 1;
				else
					txMsgW1.no_drop = 0;
			}

			txMsgW1.mtr_g = dscpInfoPtr->skb->data[2]&0x7f;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
			txMsgW1.acnt_g1 = dscpInfoPtr->skb->data[3]&0x1f;
			txMsgW1.acnt_g0 = dscpInfoPtr->skb->data[4]&0x3f;
#else
			txMsgW1.acnt_g1 = dscpInfoPtr->skb->data[3]&0x1f;
			txMsgW1.acnt_g0 = dscpInfoPtr->skb->data[4]&0x1f;
#endif
			txMsgW1.nboq = dscpInfoPtr->skb->data[9]&0x1f;
			if(specialTag)
				txMsgW0.gem_port_id = specialTag;	/*sp_tag*/
			else
				txMsgW0.gem_port_id = dscpInfoPtr->skb->data[5]&0x3f;	/*sp_tag*/
		}else{
			/* default setting */
			txMsgW1.no_drop = 0;
			txMsgW1.mtr_g = 0x7f;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
			txMsgW1.acnt_g1 = 0x1f;
			txMsgW1.acnt_g0 = 0x3f;
#else
			txMsgW1.acnt_g1 = 0x1f;
			txMsgW1.acnt_g0 = 0x1f;
#endif
			txMsgW1.nboq = 0;
			txMsgW0.gem_port_id = 0x4;	/*sp_tag, default to port-2*/
		}
#endif
		/* recycle TX DSCP when send packets in tx polling mode */
		if(gpQdmaPriv->devCfg.flags.isTxPolling == QDMA_ENABLE) {
			if(qdmaGetIrqEntryLen(base) >= gpQdmaPriv->devCfg.txRecycleThreshold) {
				qdma_bm_transmit_done(gpQdmaPriv->devCfg.txRecycleThreshold<<2) ;
			}
		}
		
		error = qdma_bm_transmit_packet(dscpInfoPtr->skb, forceRingIdx, &txMsg);
		if(error){
			dev_kfree_skb_any(dscpInfoPtr->skb);
			gpQdmaPriv->counters.rxPktErrs[ringIdx]++ ;
			return 1;
		}
		gpQdmaPriv->counters.rxCounts[ringIdx]++ ;
		return 1;
	}if(verifyTestFlag == 2) {	
		skb_put(dscpInfoPtr->skb, rxDscpPtr->ctrl.pkt_len);
#ifdef CONFIG_DEBUG
		if(gpQdmaPriv->devCfg.dbgLevel >= DBG_ST) {
			printk("\n--------QDMA Verify-2 Test RX Start--------\n");
			__dump_skb(dscpInfoPtr->skb, rxDscpPtr->ctrl.pkt_len);
			printk("RXDSCP: done:%d, nls:%d, pkt_len:%d, ",(uint)rxDscpPtr->ctrl.done, (uint)rxDscpPtr->ctrl.nls, (uint)rxDscpPtr->ctrl.pkt_len);
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528) || defined(TCSUPPORT_CPU_EN7517)
			printk("overflow:%d\n", (uint)rxDscpPtr->ctrl.drop_pkt);
#else
			printk("overflow:%d\n", (uint)rxDscpPtr->ctrl.overflow);
#endif
			printk("msg0=%.8X, msg1=%.8X , msg2=%.8X\n", (uint)rxDscpPtr->msg[0], (uint)rxDscpPtr->msg[1], (uint)rxDscpPtr->msg[2]);
			printk("--------QDMA Verify-2 Test RX End--------\n");
		}
#endif

		skb_rx_dram_test = dscpInfoPtr->skb;
		dramTestRxDone = 1;

		return 1;
	}

	return 0;

}

int qdma_driver_packet_test_check(struct sk_buff *skb){
	if(skb == skb_qdma_test){
		atomic_inc(&qdma_pkt_recycle_cnt);
		return 1;
	}

	return 0;
}

int qdma_driver_build_and_transmit_packet_test(int ringIdx, int channel_id){
	struct QDMA_DscpInfo_S *pNewDscpInfo = NULL;
	QDMA_DMA_DSCP_T *pTxDscp = NULL;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags = 0 ;
	int ret = 0 ;

	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);	
	/* Get unused TX DSCP from TX unused DSCP link list */	
	pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
	if(unlikely(pNewDscpInfo == NULL)) {
		gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
		//dev_kfree_skb_any(skb_test);
		return -ENOSR ;
	}
	
	atomic_inc(&qdma_pkt_send_cnt);

#if defined(TCSUPPORT_CPU_ARMV8) || defined(TCSUPPORT_CPU_NDS32)
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif

	pTxDscp->msg[0] = ((qdma_pkt_test_wifi_bit&0x1) << 31) | (channel_id << 3) | (qdma_pkt_test_sptag<<14) | 0x7;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	pTxDscp->msg[1] = 0x7f0007ff | ((qdma_pkt_test_fport & 0xf) << 20);
#else 
	pTxDscp->msg[1] = 0x7f2003ff; /* fport -> 1 */
#endif

	pTxDscp->ctrl.nls = 0 ;
	pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8) || defined(TCSUPPORT_CPU_NDS32)
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb_qdma_test->data);
	ecnt_dcache_wback_inv(((unsigned long)(skb_qdma_test->data)), skb_qdma_test->len);
#else
	pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb_qdma_test->data);
	dma_cache_wback_inv((unsigned long)(skb_qdma_test->data), skb_qdma_test->len);
#endif
	
	pTxDscp->ctrl.pkt_len = skb_qdma_test->len ; 
	pTxDscp->ctrl.done = 0 ;
#if defined(TCSUPPORT_CPU_EN7580)
	pTxDscp->ctrl.drop_pkt = 0 ;
#endif
	
#if defined(TCSUPPORT_CPU_ARMV8) || defined(TCSUPPORT_CPU_NDS32)
	ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
	dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
	gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb_qdma_test ;
	gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;

#if !defined(TCSUPPORT_CPU_ARMV8) && !defined(TCSUPPORT_CPU_NDS32)
	SYNC_TYPE4(); 
#endif

	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;
	gpQdmaPriv->counters.txCounts[ringIdx]++ ;
	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

	return ret ;
}

static int qdma_dram_test_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+index, "echo [1~5] > /proc/%s\n", qdma_dram_test_path) ;
	index += sprintf(buf+index, " 1: config, 2:enable, 3: get rx done, 4:get rx data, 5:free buffer \n") ;
	index += sprintf(buf+index, " dramTestDataP = %08X, DRAM_TEST_LEN = %d \n", (unsigned int)dramTestDataP, DRAM_TEST_LEN) ;
	index += sprintf(buf+index, " skb_rx_dram_test = %08X, dramTestRxDone = %d \n", (unsigned int)skb_rx_dram_test, dramTestRxDone) ;

	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_dram_test_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128];
	QDMA_DramTestCfg_T dramTestCfg;
	struct ECNT_QDMA_Data qdma_data;
#if 0
	ECNT_TSO_Data_s tso_data; 
	tsoTransTestInfo_t tso_dramTestCfg;
#endif
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

 	sscanf(val_string, "%d ", &dramTestFlag) ;

	if( dramTestFlag == 1 ){
		if( dramTestDataP == NULL ){
			dramTestDataP = (unchar *)kzalloc(DRAM_TEST_LEN, GFP_KERNEL) ;
			*(unsigned char *)(dramTestDataP) = 0x11;
			*(unsigned char *)(dramTestDataP+1) = 0x22;
			*(unsigned char *)(dramTestDataP+2) = 0x33;
			*(unsigned char *)(dramTestDataP+3) = 0x44;
		}

		memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
		memset(&dramTestCfg, 0, sizeof(QDMA_DramTestCfg_T));
		dramTestCfg.dataP = dramTestDataP;
		dramTestCfg.dataLen = DRAM_TEST_LEN;
		qdma_data.qdma_private.dramTestCfgPtr = &dramTestCfg;
		qdma_dram_test_dma_config(&qdma_data);
	}else if( dramTestFlag == 2 ){
		if( dramTestDataP == NULL ){
			printk("Data = NULL, config first\n");
			return count ;
		}
		memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
		qdma_dram_test_dma_enable(&qdma_data);
	}else if( dramTestFlag == 3 ){
		memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
		memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
		qdma_data.qdma_private.dramTestCfgPtr = &dramTestCfg;
		qdma_dram_test_is_rx_done(&qdma_data);
		printk("rx down = %d\n", dramTestCfg.rxDone);
	}else if( dramTestFlag == 4 ){
		memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
		memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
		qdma_data.qdma_private.dramTestCfgPtr = &dramTestCfg;
		qdma_dram_test_dma_skb_get(&qdma_data);

		__dump_skb(dramTestCfg.skb, dramTestCfg.skb->len);

		dev_kfree_skb_any(dramTestCfg.skb);
	}else if( dramTestFlag == 5 ){
		if(dramTestDataP){
			kfree(dramTestDataP);
			dramTestDataP = NULL;
		}
	}
#if 0 //defined(QDMA_LAN)
	else if( dramTestFlag == 11 ){
		if( dramTestDataP == NULL ){
			dramTestDataP = (unchar *)kzalloc(DRAM_TEST_LEN, GFP_KERNEL) ;
			*(unsigned char *)(dramTestDataP) = 0x11;
			*(unsigned char *)(dramTestDataP+1) = 0x22;
			*(unsigned char *)(dramTestDataP+2) = 0x33;
			*(unsigned char *)(dramTestDataP+3) = 0x44;
		}

		memset(&tso_data, 0, sizeof(ECNT_TSO_Data_s));
		memset(&tso_dramTestCfg, 0, sizeof(tsoTransTestInfo_t));
		tso_dramTestCfg.dataP = dramTestDataP;
		tso_dramTestCfg.dataLen = DRAM_TEST_LEN;
		tso_data.tso_private.dramTestCfgPtr = &tso_dramTestCfg;
		TSO_TDMA_DMA_CONFIG(&tso_dramTestCfg);
		//qdma_dram_test_dma_config(&qdma_data);
	}else if( dramTestFlag == 12 ){
		if( dramTestDataP == NULL ){
			printk("Data = NULL, config first\n");
			return count ;
		}
		
		TSO_TDMA_DMA_ENABLE();
	}
#endif

	return count ;
}


/*timer_out function to generate pkts */
static void qdma_pkt_test_monitor(TIMER_FUN_PAAM data){
	int k = 0;
	
    if(qdma_pkt_test_flag == 1){
        for(k = 0;k < qdma_pkt_burst_num; k++){
			qdma_driver_build_and_transmit_packet_test(qdma_pkt_test_ring, qdma_pkt_test_chnl);/*tx ring-0, channel-0 */
        }

		if(qdma_pkt_test_tx_mode == 1){/* burst once*/
			qdma_pkt_test_flag = 0;
			qdma_pkt_test_timer_deinit();
		}else{
			qdma_pkt_test_timer.expires = jiffies + 1;
  	    	add_timer(&qdma_pkt_test_timer);
		}
    }else{
        qdma_pkt_test_timer.expires = jiffies + 100;
        add_timer(&qdma_pkt_test_timer);
    }
}

/*init 32 skb to prepare to transmit*/
static int qdma_pkt_test_init_skb(void){
    unsigned int tx_len = 1518;
    uint8 *tx_data = NULL;
    unsigned short chksum = 0;

    if(skb_qdma_test == NULL){
        skb_qdma_test = skbmgr_dev_alloc_skb2k() ;
        if(skb_qdma_test !=NULL){
            tx_data = skb_put(skb_qdma_test, tx_len);
            memcpy(tx_data, qdma_pkt_test_udp, 68);

            tx_data[28] = 0;
			tx_data[29] = 0;
#if defined(TCSUPPORT_CPU_EN7517)
			chksum = 0;
#else
			chksum = htons(ip_fast_csum((unsigned short *) (tx_data + 18), 5));
#endif
			tx_data[28] = (chksum >> 8) & 0xff;
			tx_data[29] = chksum & 0xff;
            
        }else{		
    		printk("allocate skb fail\n");
        }
    }

    return 0;

}

/*deinit skb we init in function tcont_init_skb*/
static int qdma_pkt_test_deinit_skb(void){

    if(skb_qdma_test != NULL){
        dev_kfree_skb_any(skb_qdma_test);
        skb_qdma_test = NULL;
    }

    return 0;
}

/*init timer to generate pkts*/
static int qdma_pkt_test_timer_init(void)
{
    if(!timer_pending(&qdma_pkt_test_timer)){
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
        init_timer(&qdma_pkt_test_timer);
        qdma_pkt_test_timer.expires = jiffies + 100;
      	qdma_pkt_test_timer.function = qdma_pkt_test_monitor;
        qdma_pkt_test_timer.data = 0;
#else
		timer_setup(&qdma_pkt_test_timer,&qdma_pkt_test_monitor,0);
		qdma_pkt_test_timer.expires = jiffies + 100;
#endif
        add_timer(&qdma_pkt_test_timer);
    }
    return 0;
}

/*deinit timer*/
static int qdma_pkt_test_timer_deinit(void)
{
    if(timer_pending(&qdma_pkt_test_timer))
        del_timer_sync(&qdma_pkt_test_timer);
    return 0;
}

static int qdma_pkt_test_read_proc(char *buf, char **start, off_t off,
	int count, int *eof, void *data)
{
    int index=0 ;

	printk("echo [pkt_test_flag:0~1] [burst_num] [channel] [tx_ring] [specialTag] > /proc/%s\n", qdma_send_pkt_test_path) ;  

	printk("pkt_test_flag = %d, burst_num = %d, pkt_send_cnt = %d, pkt_recycle_cnt = %d\n", 
		qdma_pkt_test_flag, qdma_pkt_burst_num, qdma_pkt_send_cnt.counter, qdma_pkt_recycle_cnt.counter);
	printk("pkt_test_chnl = %d, pkt_test_ring = %d, pkt_test_sptag = %d\n", 
		qdma_pkt_test_chnl, qdma_pkt_test_ring, qdma_pkt_test_sptag);


	printk("echo mode [tx_mode] [fport] [wifi_bit] > /proc/%s\n", qdma_send_pkt_test_path) ;	
	printk("qdma_pkt_test_tx_mode = %d, qdma_pkt_test_fport = %d, pkt_test_wifi_bit = %d\n", 
		qdma_pkt_test_tx_mode, qdma_pkt_test_fport, qdma_pkt_test_wifi_bit);
    
    return index;	
}

static int qdma_pkt_test_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[128];
	char cmd_buf[32];
	int  len = 0; 
	
	if (count > 127)
		len = 127;
	else 
		len = count;
	
	memset(get_buf, 0, sizeof(get_buf));

	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;

	get_buf[len] = '\0';

 	sscanf(get_buf, "%s ", cmd_buf);

	if(0 == strncmp(cmd_buf, "mode", 4)){
		sscanf(get_buf, "%s %d %d %d", cmd_buf, &qdma_pkt_test_tx_mode, &qdma_pkt_test_fport, &qdma_pkt_test_wifi_bit);
		
		printk("qdma_pkt_test_tx_mode = %d, qdma_pkt_test_fport = %d, pkt_test_wifi_bit = %d\n", 
			qdma_pkt_test_tx_mode, qdma_pkt_test_fport, qdma_pkt_test_wifi_bit);
	}else{
		sscanf(get_buf, "%d %d %d %d %d",&qdma_pkt_test_flag, &qdma_pkt_burst_num, &qdma_pkt_test_chnl, &qdma_pkt_test_ring, 
			&qdma_pkt_test_sptag);
	 	printk("pkt_test_flag = %d, burst_num = %d\n", qdma_pkt_test_flag, qdma_pkt_burst_num);
		printk("pkt_test_chnl = %d, pkt_test_ring = %d, pkt_test_sptag = %d\n", qdma_pkt_test_chnl, qdma_pkt_test_ring, qdma_pkt_test_sptag);

		if(qdma_pkt_test_flag == 1){
			atomic_set(&qdma_pkt_send_cnt, 0);
			atomic_set(&qdma_pkt_recycle_cnt, 0);
	        qdma_pkt_test_timer_init();
	    }else{
	        qdma_pkt_test_timer_deinit();
	    }
	}
    
	return len;
}


/******************************************************************************
******************************************************************************/
int qdamSetSwitchSpecialTag(QDMA_Mode_t mode)
{
	if(mode == QDMA_DISABLE) {
		IO_CBITS(SWITCH_PROT6_VLAN_CONTROL, SWITCH_PORT_STAG);
	} else {
		IO_SBITS(SWITCH_PROT6_VLAN_CONTROL, SWITCH_PORT_STAG);
	}
	return 0;
}

static int qdma_bm_verify_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+index, "echo [verifyTestFlag:0~1] [fport] [macFormatEn] [specialTag]") ;
	index += sprintf(buf+index, " > /proc/%s\n", qdma_verify_path) ;  	
	index += sprintf(buf+index, "tips: (dstmac format: XX:no_drop:meter_g:acnt_g1:acnt_g0:sp_tag)\n") ;  
	index += sprintf(buf+index, "tips: (srcmac format: channel:queue:tx_ring:nbq:XX:XX )\n\n") ;  

	index += sprintf(buf+index, "verifyTestFlag         = 0x%d\n", verifyTestFlag);
	index += sprintf(buf+index, "fPort                  = 0x%d\n", fPort);
	index += sprintf(buf+index, "macFormatEn            = 0x%d\n", macFormatEn);
	index += sprintf(buf+index, "specialTag             = 0x%x\n", specialTag);

	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_bm_verify_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128];
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

 	sscanf(val_string, "%d %d %d %x", &verifyTestFlag, &fPort, &macFormatEn, &specialTag) ;

    /*EN7580: support setting sp_tag, no need disable sp_tag*/
#ifndef TCSUPPORT_CPU_EN7580
	if(verifyTestFlag) {
		verifyTestFlag = 1;
		qdamSetSwitchSpecialTag(QDMA_DISABLE);
	} else {
		qdamSetSwitchSpecialTag(QDMA_ENABLE);
	}
#endif

	if(fPort == 0)
		fPort = DPORT_GDMA1;
    
	if(macFormatEn)
		macFormatEn = 1;
	else
		macFormatEn = 0;

	return count ;
}

/******************************************************************************
******************************************************************************/
int qdma_verify_init(void)
{   
	struct proc_dir_entry *qdma_proc=NULL;

	qdma_proc = create_proc_entry(qdma_verify_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_bm_verify_read_proc ;
		qdma_proc->write_proc = qdma_bm_verify_write_proc ;
	}

	qdma_proc = create_proc_entry(qdma_send_pkt_test_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_pkt_test_read_proc ;
		qdma_proc->write_proc = qdma_pkt_test_write_proc ;
	}

	qdma_proc = create_proc_entry(qdma_dram_test_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_dram_test_read_proc ;
		qdma_proc->write_proc = qdma_dram_test_write_proc ;
	}

	atomic_set(&qdma_pkt_send_cnt, 0);
	atomic_set(&qdma_pkt_recycle_cnt, 0);
	qdma_pkt_test_init_skb();
	
    return 0;
}

void qdma_verify_deinit(void)
{
	remove_proc_entry(qdma_verify_path, NULL);
	remove_proc_entry(qdma_send_pkt_test_path, NULL);
	remove_proc_entry(qdma_dram_test_path, NULL);
	qdma_pkt_test_deinit_skb();  
}


