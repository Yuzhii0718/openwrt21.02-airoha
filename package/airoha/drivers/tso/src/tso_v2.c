#include "tso.h"

/************************************************************************
*				  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#if defined(TCSUPPORT_TSO_V2_ENABLE)
#define QDMA_LAN 1
#endif


#if defined(QDMA_LAN)
#define TSO_PATH "tso_lan"
#define TSO_DEBUG_PATH "tso_lan/debug"
#define TSO_RULE_PATH "tso_lan/rule"
#define ECNT_TSO ECNT_TSO_LAN
#else
#define TSO_PATH "tso_wan"
#define TSO_DEBUG_PATH "tso_wan/debug"
#define TSO_RULE_PATH "tso_wan/rule"
#define ECNT_TSO ECNT_TSO_WAN
#endif

#define PDMA_MSG(level, F, B...)	{ \
											if(tsoPdmaPriv->dbgLevel >= level)	\
												printk("%s: %s [%d]: " F, TSO_PATH, strrchr(__FILE__, '/')+1, __LINE__, ##B) ; \
										}
#define PDMA_ERR(F, B...)		   printk("%s: %s [%d]: " F, TSO_PATH, strrchr(__FILE__, '/')+1, __LINE__, ##B)
#define PDMA_LOG(F, B...)		   printk("%s: %s [%d]: " F, TSO_PATH, strrchr(__FILE__, '/')+1, __LINE__, ##B)

//#define K0_TO_K1(x)	  				((uint)(x) | 0x20000000)  /* kseg0 to kseg1 */
//#define K1_TO_K0(x)	  				((uint)(x) & 0x9fffffff)  /* kseg1 to kseg0 */


typedef int (*tso_api_op_t)(struct ECNT_TSO_Data *tso_data);

#ifdef TCSUPPORT_CPU_ARMV8
extern struct device* get_frame_engine_dev(void);
#endif


/************************************************************************
*				  P U B L I C   D A T A
*************************************************************************
*/
extern int tso_match_v4(struct ECNT_TSO_Data *tso_data);
extern int tso_match_v6(struct ECNT_TSO_Data *tso_data);
int tso_transmit_packet(struct ECNT_TSO_Data *tso_data);
int tso_tdma_dma_config(struct ECNT_TSO_Data *tso_data);
int tso_tdma_dma_enable(struct ECNT_TSO_Data *tso_data);

int tso_localout_offload(struct ECNT_TSO_Data *tso_data);
int tso_pingpong_v4(struct ECNT_TSO_Data *tso_data);
int tso_destroy_v4(struct ECNT_TSO_Data *tso_data);


static unchar *tsodramTestDataP = NULL;
static int tsodramTestDataLen = 0;
static uint tsodramTestRingIdx = 0;
static uint tsodramTestDscpIdx = 0;


tso_api_op_t
tso_operation[]=
{
	tso_match_v4,
	tso_match_v6,
	tso_localout_offload,
	tso_pingpong_v4,
	tso_destroy_v4,
	tso_transmit_packet,
	tso_tdma_dma_config,
	tso_tdma_dma_enable,
};

PDMA_Private_T *tsoPdmaPriv = NULL;
extern int tsoDbgRecord;
extern int skb_count;
extern uint tsoMssCount[TSO_MAX_MSS_COUNT];
extern int fast_path;
extern int not_drop;


/************************************************************************
*				  P R I V A T E   D A T A
*************************************************************************
*/
const uint PDMA_TX_DSCP_NUM[PDMA_TX_RING_NUM] = {PMDA_TXDSCP_NUM,PMDA_TXDSCP_NUM};


/************************************************************************
*				  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int tso_transmit_packet_done(void);
static inline struct PDMA_DscpInfo_S *pdma_pop_tx_dscp(int ringIdx);
static inline struct PDMA_DscpInfo_S *pdma_push_tx_dscp(struct PDMA_DscpInfo_S *diPtr, int ringIdx);

#if  defined(TCSUPPORT_TSO_VERIFY)	
	
/******************************************************************************
******************************************************************************/

/* do not kfree skb , for stress testing*/
void tso_verify_keep_skb_alive(uint msg0, uint msg1, uint rinIdx){
	tsoTestTxMsg_t tsoTxMsg;
	tsoTxMsg.msg[0] = msg0;
	tsoTxMsg.msg[1] = msg1;
	if(tsoTxMsg.raw.resv & 0x1){/* SPECIAL_DONT_FREESPECIAL_DONT_FREE */
		/* set this flag for marking the skb shouldn't be freed. */
		tsoPdmaPriv->txUsingPtr[rinIdx]->skb = NULL;
	}
}

/* for random interval insert between neighber TxDs. */
void tso_verify_delay_betwenn_txds(uint msg0, uint msg1, uint dscpIdx,uint ringIdx){
	tsoTestTxMsg_t tsoTxMsg;
	tsoTxMsg.msg[0] = msg0;
	tsoTxMsg.msg[1] = msg1;
	unsigned int interval=0;
	
	if(tsoTxMsg.raw.resv & 0x2 ){/* SPECIAL_RANDOM_INTERVAL */
		interval = random32()%((unsigned int)10000);
		printk("delay interval \n");
		udelay(interval);
		wmb() ;
		PDMA_SET_TX_IDX(ringIdx,dscpIdx);
	}
}
#endif


__IMEM static irqreturn_t pdma_isr(int irq, void *dev_id)
{
	uint intStatus = 0,ringIdx=0 ;
	
	intStatus = IO_GREG(PDMA_INT_STATUS);

	//printk("pdma_isr\n");
	/*Handle TX-DONE INT*/
	if(intStatus & (PDMA_INT_TX0_DONE | PDMA_INT_TX1_DONE))
	{
		IO_SREG(PDMA_INT_STATUS, (PDMA_INT_TX0_DONE | PDMA_INT_TX1_DONE));
		tso_transmit_packet_done();
	}
	
	return IRQ_HANDLED ;
}


static int tdma_txdscp_init(void)
{
	struct PDMA_DscpInfo_S *diPtr = NULL;
	dma_addr_t dscpDmaAddr=0, irqDmaAddr=0, hwFwdDmaAddr=0, hwFwdBuffAddr=0 ;
	uint hwTotalDscpSize=0, hwTotalPktSize=0 , curDscpNum;
	int ringIdx,i,txDscpTotal=0,tmpReg;
	uint dmaIdx = 0;
	tsoTxMsg_t *txmsg;
#ifdef TCSUPPORT_CPU_ARMV8
	struct device *dev = NULL ;
	unsigned long dscpBaseAddr=0 ;
#else
	uint dscpBaseAddr=0 ;
#endif

	/*PDMA REG INIT*/
	IO_SREG(PDMA_TX0_MAX_CNT,0x400);
	IO_SREG(PDMA_TX1_MAX_CNT,0x400);
	tmpReg = IO_GREG(PDMA_GLO_CFG);
	tmpReg |= (0x1 << 0);
	IO_SREG(PDMA_GLO_CFG,tmpReg);
#ifdef TCSUPPORT_CPU_ARMV8
	IO_SREG(PDMA_INT_MASK, (PDMA_INT_TX0_DONE | PDMA_INT_TX1_DONE));
#else
	IO_SREG(PDMA_INT_MASK,0x0);
#endif

	
	for(ringIdx=0; ringIdx<PDMA_TX_RING_NUM; ringIdx++){
		tsoPdmaPriv->txDscpNum[ringIdx] = PDMA_TX_DSCP_NUM[ringIdx];
		txDscpTotal += tsoPdmaPriv->txDscpNum[ringIdx];
	}
	
	/******************************************
	* Allocate descriptor DMA memory		  *
	*******************************************/
#ifdef TCSUPPORT_CPU_ARMV8
	if( (dev=get_frame_engine_dev()) == NULL ) {
		printk("Get device failed.\n") ; 
		return -ENOMEM ;
	}
	dscpBaseAddr = (unsigned long)dma_alloc_coherent(dev, sizeof(PDMA_DMA_DSCP_T)*(txDscpTotal), &dscpDmaAddr, GFP_KERNEL) ;
#else
	dscpBaseAddr = (uint)dma_alloc_coherent(NULL, sizeof(PDMA_DMA_DSCP_T)*(txDscpTotal), &dscpDmaAddr, GFP_KERNEL) ;
#endif
	if(!dscpBaseAddr) {
		printk("Allocate memory for TX/RX DSCP failed.\n") ; 
		return -ENOMEM ;
	}

	PDMA_LOG("dscpDmaAddr =%x\n",dscpDmaAddr) ; 

	/* Set the TX_DSCP_BASE  address */
	curDscpNum = 0 ;
	for(ringIdx = 0; ringIdx < PDMA_TX_RING_NUM; ringIdx++) {
		IO_SREG(PDMA_TX0_BASE_PTR + PDMA_TX_SPAN * ringIdx,dscpDmaAddr + sizeof(PDMA_DMA_DSCP_T)*curDscpNum);
		curDscpNum += tsoPdmaPriv->txDscpNum[ringIdx];
	}

		/***************************************************
	* Allocate memory for TX/RX DSCP Information node  *
	****************************************************/
	tsoPdmaPriv->dscpInfoAddr = (unsigned long)kzalloc(sizeof(struct PDMA_DscpInfo_S)*(txDscpTotal), GFP_KERNEL) ;
	if(!tsoPdmaPriv->dscpInfoAddr) {
		printk("Alloc memory for TX/RX DSCP information node failed\n") ;
		return -ENOMEM ;
	}

	/* Set the TX_DSCP_BASE and RX_DSCP_BASE address */
	curDscpNum = 0 ;
	for(ringIdx = 0; ringIdx < PDMA_TX_RING_NUM; ringIdx++) {
		tsoPdmaPriv->txBaseAddr[ringIdx] = tsoPdmaPriv->dscpInfoAddr + sizeof(struct PDMA_DscpInfo_S)*curDscpNum ;
		curDscpNum += tsoPdmaPriv->txDscpNum[ringIdx];
	}

	/* Create unused tx descriptor link list */
	for(i = 0 ; i < txDscpTotal ; i++)
	{
		diPtr = (struct PDMA_DscpInfo_S *)tsoPdmaPriv->dscpInfoAddr + i ;
		diPtr->dscpPtr = (PDMA_DMA_DSCP_T *)dscpBaseAddr + i ;

		diPtr->skb = NULL;
		diPtr->next = NULL ;
		diPtr->dscpPtr->ctrl.done = 1 ;
		diPtr->dscpPtr->ctrl.drop_pkt = 0 ;
		diPtr->dscpPtr->ctrl.nls = 0;
		
		txmsg = diPtr->dscpPtr->msg;
		txmsg->raw.fast = 1;/**/
		txmsg->raw.hwf = 1;

		for(curDscpNum = 0, ringIdx = 0; ringIdx < PDMA_TX_RING_NUM; ringIdx++) {
			if((i-curDscpNum) < tsoPdmaPriv->txDscpNum[ringIdx]) {
				diPtr->dscpIdx = i-curDscpNum;
				pdma_push_tx_dscp(diPtr, ringIdx) ;
				break;
			}
			curDscpNum += tsoPdmaPriv->txDscpNum[ringIdx];
		}
	}

	
	/***************************************************
	* Initialization first DSCP for Tx DMA			  *
	****************************************************/
	for(ringIdx=0; ringIdx<PDMA_TX_RING_NUM; ringIdx++) {
		diPtr = pdma_pop_tx_dscp(ringIdx) ;
		if(!diPtr) {
			PDMA_ERR("There is not any free TX%d DSCP.\n", ringIdx) ; 
			return -ENOSR ;
		}
		tsoPdmaPriv->txUsingPtr[ringIdx] = diPtr ;
		PDMA_LOG("diPtr->dscpIdx = %d\n",diPtr->dscpIdx);
		IO_SREG(PDMA_TX0_CPU_IDX + 0x0010*ringIdx,diPtr->dscpIdx);
		IO_SREG(PDMA_TX0_DMA_IDX + 0x0010*ringIdx,diPtr->dscpIdx);
		tsoPdmaPriv->recycleIdx[ringIdx] = diPtr->dscpIdx;
	}

#ifdef TCSUPPORT_CPU_ARMV8
	/*set tx interrupt mode*/
	tsoPdmaPriv->flags.isTxPolling = 0;
#else
	/*set tx polling mode*/
	tsoPdmaPriv->flags.isTxPolling = 1;
#endif

	/*set txdscp recycle thredhold*/
	for(ringIdx=0; ringIdx < PDMA_TX_RING_NUM; ringIdx++){
		tsoPdmaPriv->recycleThr[ringIdx] = DEFAULT_RECYCLE_THREAD;
	}
	

	return 0;

}


static inline struct PDMA_DscpInfo_S *pdma_pop_tx_dscp(int ringIdx)
{
	struct PDMA_DscpInfo_S *diPtr=NULL ;
	diPtr = tsoPdmaPriv->txHeadPtr[ringIdx];
	if(tsoPdmaPriv->txHeadPtr[ringIdx] == tsoPdmaPriv->txTailPtr[ringIdx]){
		tsoPdmaPriv->txHeadPtr[ringIdx] = NULL;
		tsoPdmaPriv->txTailPtr[ringIdx] = NULL;
	}else{
		tsoPdmaPriv->txHeadPtr[ringIdx] = tsoPdmaPriv->txHeadPtr[ringIdx]->next;
	}
	if(diPtr) {
		diPtr->next = NULL ;	
		atomic_dec(&tsoPdmaPriv->free_txd_num[ringIdx]);
	}else{
	   PDMA_ERR("pop tx dscp failed diPtr is null!\n");	
	}
	
	return diPtr ;
}

static inline struct PDMA_DscpInfo_S *pdma_push_tx_dscp(struct PDMA_DscpInfo_S *diPtr, int ringIdx)
{
	if(diPtr->next != NULL) {
		printk("The TX DSCP is not return from tx used pool\n") ;
		return -1 ;
	}

	diPtr->skb = NULL ;
	if(!tsoPdmaPriv->txHeadPtr[ringIdx]) {
		tsoPdmaPriv->txHeadPtr[ringIdx] = diPtr ;
		tsoPdmaPriv->txTailPtr[ringIdx] = diPtr ;
	} else {
		tsoPdmaPriv->txTailPtr[ringIdx]->next = diPtr ;
		tsoPdmaPriv->txTailPtr[ringIdx] = tsoPdmaPriv->txTailPtr[ringIdx]->next ;
	}

	atomic_inc(&tsoPdmaPriv->free_txd_num[ringIdx]);

	return 0 ;

}

int tso_tdma_dma_config(struct ECNT_TSO_Data *tso_data)
{
	tsoTransTestInfo_t	* dramTestCfg = tso_data->tso_private.dramTestCfgPtr;


	//struct sk_buff *skb = tso_data->tso_private.transInfo->skb;
	struct PDMA_DscpInfo_S *pNewDscpInfo = NULL ;
	struct sk_buff *skb_dram_test = NULL;
//	uint base = tsoPdmaPriv->csrBaseAddr ;
	PDMA_DMA_DSCP_T *pTxDscp=NULL ;
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	struct bio_vec *frag;
#else
	struct skb_frag_struct *frag;
#endif
	uint nr_frags = 0;
	uint msg0 =0;
	uint msg1 =0;
	uint offset=0,size=0;
	//unsigned short *checksum;
	//unsigned int *seq = (unsigned int*)(skb_transport_header(skb)+4);
	unsigned long flags = 0;
	tsoTxMsg_t txMsg;
	int mss_count,i , ringIdx = 0;
	uint8 *tx_data = NULL;

	skb_dram_test = skbmgr_dev_alloc_skb2k() ;

	if(!skb_dram_test){
		printk("The input arguments are wrong, skb is NULL") ; 
		return ECNT_HOOK_ERROR ;
	}

	tsodramTestDataP = dramTestCfg->dataP;
	tsodramTestDataLen = dramTestCfg->dataLen;

	tx_data = skb_put(skb_dram_test, tsodramTestDataLen);
	//memcpy(tx_data, tsodramTestDataP, tsodramTestDataLen);

	//printk("jiff1:%u, seq:%u\n", jiffies, htonl(*seq));
	txMsg.msg[0] = 0;
	txMsg.msg[1] = 0;
	ringIdx = tsodramTestRingIdx;
	PDMA_MSG(DBG_ST,"ENTER tso_transmit_packet,ringIdx=%d\n",ringIdx);	
	PDMA_MSG(DBG_ST,"PDMA_INT_STA_VAL = %x\ntsoPdmaPriv->flags.isTxPolling = %d\n",PDMA_INT_STA_VAL,tsoPdmaPriv->flags.isTxPolling);

	if(tsoPdmaPriv->flags.isTxPolling){
		if(PDMA_INT_STA_VAL & (PDMA_INT_TX0_DONE|PDMA_INT_TX1_DONE )) {
			tso_transmit_packet_done();
		}	
	}

	if(atomic_read(&tsoPdmaPriv->free_txd_num[ringIdx]) < nr_frags+1){
		printk("There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", ringIdx, atomic_read(&tsoPdmaPriv->free_txd_num[ringIdx]), nr_frags);
		dev_kfree_skb_any(skb_dram_test) ;
		return -EFAULT ;
	}

	
	/* record */

	/*config tso bit*/
#if !defined(TCSUPPORT_TSO_VERIFY)
	txMsg.raw.ico = 1;	
	txMsg.raw.tso = 0;
	
	txMsg.raw.uco = 1;
	txMsg.raw.tco = 1;
#endif
	
	txMsg.raw.no_drop = 0;
	txMsg.raw.fast = 0;
	txMsg.raw.hwf = 0;
	txMsg.raw.fPort = 0;


	PDMA_MSG(DBG_ST,"tx msg fport:%x\n", txMsg.raw.fPort);


	spin_lock_irqsave(&tsoPdmaPriv->txLock[ringIdx],flags);

	for(i = 0;i <= nr_frags; i++){
		/* Get unused TX DSCP from TX unused DSCP link list */
		pNewDscpInfo = (struct PDMA_DscpInfo_S *)pdma_pop_tx_dscp(ringIdx);
		if(unlikely(pNewDscpInfo == NULL)) {
			printk("qdma_bm_transmit_packet_tso pNewDscpInfo is NULL\n") ; 
			spin_unlock_irqrestore(&tsoPdmaPriv->txLock[ringIdx], flags);
			return -ENOSR ;
		}
		
#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (PDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)tsoPdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
		pTxDscp = (PDMA_DMA_DSCP_T *)K1_TO_K0((uint32)tsoPdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif
		pTxDscp->msg[0] = txMsg.msg[0];
		pTxDscp->msg[1] = txMsg.msg[1];
		pTxDscp->next.next_idx = pNewDscpInfo->dscpIdx;

		if(i == 0){
#if defined(TCSUPPORT_CPU_ARMV8)
			pTxDscp->pkt_addr = (unsigned long)(tsodramTestDataP);
			//pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)(tsodramTestDataP));
			//ecnt_dcache_wback_inv(((unsigned long)(tsodramTestDataP)), tsodramTestDataLen);

#else
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)(tsodramTestDataP));
			dma_cache_wback_inv(((unsigned long)(tsodramTestDataP)), tsodramTestDataLen);
#endif

			pTxDscp->ctrl.pkt_len = tsodramTestDataLen; 
		}
		pTxDscp->ctrl.done = 0 ;
		
#if defined(TCSUPPORT_TSO_VERIFY)
		pTxDscp->ctrl.drop_pkt = 0 ;
#endif

		/* not last segment flag, last block set to 0, and others set to 1. */
		if(i == nr_frags){
			pTxDscp->ctrl.nls = 0;
			tsoPdmaPriv->txUsingPtr[ringIdx]->skb = skb_dram_test ;
			/* chnlLimit &	queueLimit is for skb count. */
//				atomic_inc(&chnlLimit[channel]);
//				atomic_inc(&queueLimit[channel][queue]);
		}
		else{
			pTxDscp->ctrl.nls = 1;
			tsoPdmaPriv->txUsingPtr[ringIdx]->skb = NULL ;
		}

		//dma_cache_wback_inv(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
		/*dump tx DSCP*/
		PDMA_MSG(DBG_ST,"tx DSCP dump:%x %x %x %x\n%x %x %x %x\n",pTxDscp->resv1,pTxDscp->ctrl,pTxDscp->pkt_addr,pTxDscp->next, \
		pTxDscp->msg[0],pTxDscp->msg[1],pTxDscp->msg[2],pTxDscp->msg[3]);
		
		//printk("jiff3:%u\n", jiffies);
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(PDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(PDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif

#if defined(TCSUPPORT_TSO_VERIFY)
		tso_verify_keep_skb_alive(msg0, msg1, ringIdx);
#endif

		tsoPdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;

#if defined(TCSUPPORT_TSO_VERIFY)
		/* msg1 for tso verify test14, this function must be called after "gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo". */
		tso_verify_delay_betwenn_txds(msg0, msg1,pNewDscpInfo->dscpIdx,ringIdx);
#endif

	}
	
	wmb() ;
	//printk("jiff4:%u\n", jiffies);
	if(pNewDscpInfo){
		PDMA_MSG(DBG_ST,"ringIdx=%d pNewDscpInfo->dscpIdx = %d\n",ringIdx,pNewDscpInfo->dscpIdx);
	}else{
		PDMA_ERR("error!,pNewDscpInfo is NULL!\n");
	}
	
	//PDMA_SET_TX_IDX(ringIdx,pNewDscpInfo->dscpIdx);
	tsodramTestDscpIdx = pNewDscpInfo->dscpIdx;
	
	//printk("jiff5:%u\n", jiffies);
	spin_unlock_irqrestore(&tsoPdmaPriv->txLock[ringIdx], flags);
	return 0 ;
			

}


int tso_localout_offload(struct ECNT_TSO_Data *tso_data){

	return 0;
}
int tso_pingpong_v4(struct ECNT_TSO_Data *tso_data){


	return 0;
}
int tso_destroy_v4(struct ECNT_TSO_Data *tso_data){

	return 0;
}


/* send data to qdma_lan cpu*/
int tso_tdma_dma_enable(struct ECNT_TSO_Data *tso_data)
{
	PDMA_SET_TX_IDX(tsodramTestRingIdx,tsodramTestDscpIdx);

	return 0;
}


int tso_transmit_packet(struct ECNT_TSO_Data *tso_data)
{
	struct sk_buff *skb = tso_data->tso_private.transInfo->skb;
	struct PDMA_DscpInfo_S *pNewDscpInfo = NULL ;
//	uint base = tsoPdmaPriv->csrBaseAddr ;
	PDMA_DMA_DSCP_T *pTxDscp=NULL ;
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	struct bio_vec *frag;
#else
	struct skb_frag_struct *frag;
#endif

	uint nr_frags = 0;
	uint msg0 = tso_data->tso_private.transInfo->msg0;
	uint msg1 = tso_data->tso_private.transInfo->msg1;
	uint offset=0,size=0;
	unsigned short *checksum;
	unsigned int *seq = NULL;
	unsigned long flags = 0;
	tsoTxMsg_t txMsg;
	int mss_count,i , ringIdx = 0;

	if(!skb){
		printk("The input arguments are wrong, skb is NULL") ; 
		return ECNT_HOOK_ERROR ;
	}
	nr_frags = skb_shinfo(skb)->nr_frags;
	seq = (unsigned int*)(skb_transport_header(skb)+4);

	//printk("jiff1:%u, seq:%u\n", jiffies, htonl(*seq));
	txMsg.msg[0] = tso_data->tso_private.transInfo->msg0;
	txMsg.msg[1] = tso_data->tso_private.transInfo->msg1;
	ringIdx = tso_data->tso_private.transInfo->txRing;
	PDMA_MSG(DBG_ST,"ENTER tso_transmit_packet,ringIdx=%d\n",ringIdx);	
	PDMA_MSG(DBG_ST,"PDMA_INT_STA_VAL = %x\ntsoPdmaPriv->flags.isTxPolling = %d\n",PDMA_INT_STA_VAL,tsoPdmaPriv->flags.isTxPolling);

	if(tsoPdmaPriv->flags.isTxPolling){
		if(PDMA_INT_STA_VAL & (PDMA_INT_TX0_DONE|PDMA_INT_TX1_DONE )) {
			tso_transmit_packet_done();
		}	
	}

	if(atomic_read(&tsoPdmaPriv->free_txd_num[ringIdx]) < nr_frags+1){
		printk("There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", ringIdx, atomic_read(&tsoPdmaPriv->free_txd_num[ringIdx]), nr_frags);
		return -EFAULT ;
	}

	PDMA_MSG(DBG_ST,"skb->len = %d\nskb_shinfo(skb)->gso_size = %d\n",skb->len,skb_shinfo(skb)->gso_size);
	/* record */
	if(TSO_RECORD_DEBUG_ON == tsoDbgRecord){
		skb_count++;
		if(skb_shinfo(skb)->gso_size){
			mss_count = skb->len / skb_shinfo(skb)->gso_size;
			if(mss_count <= TSO_MAX_MSS_COUNT && mss_count > 0){
				tsoMssCount[mss_count-1]++;
			}
			if(mss_count == 0){
				tsoMssCount[0]++;
			}
		}
		else{
			mss_count = skb->len / 1448;
			if(mss_count <= TSO_MAX_MSS_COUNT && mss_count > 0){
				tsoMssCount[mss_count-1]++;
			}
			if(mss_count == 0){
				tsoMssCount[0]++;
			}
		}
	}

	/*config tso bit*/
#if !defined(TCSUPPORT_TSO_VERIFY)
	txMsg.raw.ico = 1;	
	if(skb_shinfo(skb)->gso_size != 0){
		txMsg.raw.tso = 1;
		checksum = (unsigned short*)(skb_transport_header(skb)+16);
		*checksum = htons(skb_shinfo(skb)->gso_size);
	}
	else 
		txMsg.raw.tso = 0;
	
	txMsg.raw.uco = 1;
	txMsg.raw.tco = 1;
#endif
	txMsg.raw.no_drop = not_drop;
	txMsg.raw.fast = fast_path;
	txMsg.raw.hwf = 1;
	PDMA_MSG(DBG_ST, "pTxMsg.msg[0]:%x, pTxMsg.msg[1]:%x,tso:%d,uco:%x,tco:%x,hwf:%x,fport:%x,resv:%x\n", txMsg.msg[0], txMsg.msg[1], txMsg.raw.tso,txMsg.raw.uco,txMsg.raw.tco,txMsg.raw.hwf,txMsg.raw.fPort,txMsg.raw.resv);


	PDMA_MSG(DBG_ST,"tx msg fport:%x\n", txMsg.raw.fPort);
#if 0
	if(tso_data->tso_private.transInfo->mainType){
		/* to QDMA_LAN*/
		txMsg.raw.fPort = 0x20;
	}else{
		/* to QDMA_WAN*/
		txMsg.raw.fPort = 0x40;
	}
#endif

	spin_lock_irqsave(&tsoPdmaPriv->txLock[ringIdx],flags);

	for(i = 0;i <= nr_frags; i++){
		/* Get unused TX DSCP from TX unused DSCP link list */
		pNewDscpInfo = (struct PDMA_DscpInfo_S *)pdma_pop_tx_dscp(ringIdx);
		if(unlikely(pNewDscpInfo == NULL)) {
			printk("qdma_bm_transmit_packet_tso pNewDscpInfo is NULL\n") ; 
			spin_unlock_irqrestore(&tsoPdmaPriv->txLock[ringIdx], flags);
			return -ENOSR ;
		}
		
#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (PDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)tsoPdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
		pTxDscp = (PDMA_DMA_DSCP_T *)K1_TO_K0((uint32)tsoPdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif
		pTxDscp->msg[0] = txMsg.msg[0];
		pTxDscp->msg[1] = txMsg.msg[1];
		pTxDscp->next.next_idx = pNewDscpInfo->dscpIdx;

		if(i == 0){
#if defined(TCSUPPORT_CPU_ARMV8)
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
			ecnt_dcache_wback_inv(((unsigned long)(skb->data)), (skb->len-skb->data_len));
#else
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
			dma_cache_wback_inv((unsigned long)(skb->data), (skb->len-skb->data_len));
#endif

			pTxDscp->ctrl.pkt_len = (skb->len-skb->data_len); 
		}else{
			frag = &skb_shinfo(skb)->frags[i-1];
			if(frag){

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)

				offset = frag->bv_offset;
				size = frag->bv_len;
#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)(page_address(frag->bv_page)+offset));
				ecnt_dcache_wback_inv(((unsigned long)(page_address(frag->bv_page)+offset)), size);
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)(page_address(frag->bv_page)+offset));
				dma_cache_wback_inv((unsigned long)(page_address(frag->bv_page)+offset), (size));
#endif

#else
				offset = frag->page_offset;
				size = frag->size;
#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)(page_address(frag->page.p)+offset));
				ecnt_dcache_wback_inv(((unsigned long)(page_address(frag->page.p)+offset)), size);
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)(page_address(frag->page.p)+offset));
				dma_cache_wback_inv((unsigned long)(page_address(frag->page.p)+offset), (size));
#endif
#endif
				pTxDscp->ctrl.pkt_len = size; 
			}
		}
		pTxDscp->ctrl.done = 0 ;
		
#if defined(TCSUPPORT_TSO_VERIFY)
		pTxDscp->ctrl.drop_pkt = 0 ;
#endif

		/* not last segment flag, last block set to 0, and others set to 1. */
		if(i == nr_frags){
			pTxDscp->ctrl.nls = 0;
			tsoPdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
			/* chnlLimit &  queueLimit is for skb count. */
//				atomic_inc(&chnlLimit[channel]);
//				atomic_inc(&queueLimit[channel][queue]);
		}
		else{
			pTxDscp->ctrl.nls = 1;
			tsoPdmaPriv->txUsingPtr[ringIdx]->skb = NULL ;
		}

		//dma_cache_wback_inv(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
		/*dump tx DSCP*/
		PDMA_MSG(DBG_ST,"tx DSCP dump:%x %x %x %x\n%x %x %x %x\n",pTxDscp->resv1,pTxDscp->ctrl,pTxDscp->pkt_addr,pTxDscp->next, \
		pTxDscp->msg[0],pTxDscp->msg[1],pTxDscp->msg[2],pTxDscp->msg[3]);
		
		//printk("jiff3:%u\n", jiffies);
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(PDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(PDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif

#if defined(TCSUPPORT_TSO_VERIFY)
		tso_verify_keep_skb_alive(msg0, msg1, ringIdx);
#endif

		tsoPdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;

#if defined(TCSUPPORT_TSO_VERIFY)
		/* msg1 for tso verify test14, this function must be called after "gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo". */
		tso_verify_delay_betwenn_txds(msg0, msg1,pNewDscpInfo->dscpIdx,ringIdx);
#endif

	}
	
	wmb() ;
	//printk("jiff4:%u\n", jiffies);
	if(pNewDscpInfo){
		PDMA_MSG(DBG_ST,"ringIdx=%d pNewDscpInfo->dscpIdx = %d\n",ringIdx,pNewDscpInfo->dscpIdx);
	}else{
		PDMA_ERR("error!,pNewDscpInfo is NULL!\n");
	}
	PDMA_SET_TX_IDX(ringIdx,pNewDscpInfo->dscpIdx);
		
	//printk("jiff5:%u\n", jiffies);
	spin_unlock_irqrestore(&tsoPdmaPriv->txLock[ringIdx], flags);
	return 0 ;
			
}

inline static int tso_transmit_packet_done(void)
{
	uint recycleIdx=0;
	int i=0,curDscpNum=0,recycleNum=0,ringIdx;
	int txDmaIdx[PDMA_TX_RING_NUM] = {0};
	tsoTxMsg_t *txmsg;
	struct PDMA_DscpInfo_S  *diPtr = NULL;
	PDMA_DMA_DSCP_T *ptxDscp = NULL;
	int ret = 0;
	unsigned long flags = 0;

	for(ringIdx = 0;ringIdx < PDMA_TX_RING_NUM; ringIdx++){
		txDmaIdx[ringIdx] = IO_GREG(PDMA_TX0_DMA_IDX+ringIdx*PDMA_TX_SPAN);
		recycleIdx = tsoPdmaPriv->recycleIdx[ringIdx]; /*default use txring0*/
		recycleNum = txDmaIdx[ringIdx] - recycleIdx;
		if(recycleNum == 0){
			continue;
		}else if(recycleNum < 0){
			recycleNum += PMDA_TXDSCP_NUM;
		}
	
		//printk("recycleIdx:%d, dma idx:%d, cpu idx:%d\n", recycleIdx, txDmaIdx[ringIdx], regRead(PDMA_TX0_CPU_IDX+ringIdx*PDMA_TX_SPAN));
		if(tsoPdmaPriv->recycleThr[ringIdx] > recycleNum){
			continue;
		}
	
		PDMA_MSG(DBG_ST,"ENTER tso_transmit_packet_done\n");
	
		for(i = 0; i < recycleNum; i++){
			diPtr = (struct PDMA_DscpInfo_S *)tsoPdmaPriv->txBaseAddr[ringIdx] + (recycleIdx + i)%PMDA_TXDSCP_NUM;
			PDMA_MSG(DBG_ST,"diPtr addr=%p\n",diPtr);
			if(diPtr->dscpIdx != (recycleIdx + i)%PMDA_TXDSCP_NUM || diPtr->next != NULL){
				printk("The content of the TX  is incorrect. txDmaIdx:%d, recycleIdx:%d, i:%d\ndiPtr->dscpIdx=%d\n",txDmaIdx[ringIdx],recycleIdx,i,\
					diPtr->dscpIdx);
				ret = -EFAULT ;
				continue;
			}
			ptxDscp = diPtr->dscpPtr;
			if(!ptxDscp->ctrl.done){
				printk("The done/drop bit of TX DSCP is incorrect\n");
				ret = -EFAULT ;
				continue;
			}

			ptxDscp->ctrl.done = 1 ;
			ptxDscp->ctrl.drop_pkt = 0 ;
			ptxDscp->ctrl.nls = 0;
			ptxDscp->msg[0] = 0;
			ptxDscp->msg[1] = 0;
			txmsg = ptxDscp->msg;
			txmsg->raw.fast = 1;
			txmsg->raw.hwf = 1;

			//dma_unmap_single(NULL, ptxDscp->pkt_addr, ptxDscp->ctrl.pkt_len, DMA_TO_DEVICE) ;
			if(diPtr->skb != NULL){
#if defined(TCSUPPORT_TSO_VERIFY)
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
				struct bio_vec *frag;
#else
				struct skb_frag_struct *frag;
#endif

				unsigned int nr_frags;
				int frag_idx;
				nr_frags = skb_shinfo(diPtr->skb)->nr_frags;
				if(nr_frags > 0){
					for(frag_idx=0; frag_idx<nr_frags; frag_idx++){				 
						frag = &skb_shinfo(diPtr->skb)->frags[frag_idx];
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
						if(frag->bv_page){
							__free_pages(frag->bv_page ,get_order(frag->bv_len));
						diPtr->skb->len 		-= frag->bv_len;
						diPtr->skb->data_len	-= frag->bv_len;
						diPtr->skb->truesize	-= frag->bv_len;
						frag->bv_offset = 0;
						frag->bv_len = 0;
						frag->bv_page =  NULL;
#else 
						if(frag->page.p){
							__free_pages(frag->page.p ,get_order(frag->size));
							
						diPtr->skb->len 		-= frag->size;
						diPtr->skb->data_len	-= frag->size;
						diPtr->skb->truesize	-= frag->size;
						frag->page_offset = 0;
						frag->size = 0;
						frag->page.p = NULL;
#endif
						}
					}
					skb_shinfo(diPtr->skb)->nr_frags = 0;
				}
#endif
				dev_kfree_skb_any(diPtr->skb);
				//dev_kfree_skb(diPtr->skb);
				diPtr->skb = NULL;
			}

			spin_lock_irqsave(&tsoPdmaPriv->txLock[ringIdx],flags) ;
			pdma_push_tx_dscp(diPtr, ringIdx) ;
			spin_unlock_irqrestore(&tsoPdmaPriv->txLock[ringIdx],flags) ;
		}
		tsoPdmaPriv->recycleIdx[ringIdx] = txDmaIdx[ringIdx];

	}
	
	PDMA_MSG(DBG_ST,"Leave tso_transmit_packet_done\n");

	return 0;
}


static int tso_init(void)
{
#ifdef TCSUPPORT_CPU_ARMV8
	struct device *dev = NULL ;
#endif

	
	tsoPdmaPriv = kzalloc(sizeof(PDMA_Private_T), GFP_ATOMIC);

	if(tdma_txdscp_init())
	{
		printk("tdma_txdscp_init failed!/n");
		return -ENODEV;
	}

	
#if 0
	if(tso_reg_init())
	{
		printk("tso_reg_init failed!/n");
		return -ENODEV;
	}

	if(tdma_txdscp_init())
	{
		printk("tdma_txdscp_init failed!/n");
		return -ENODEV;
	}
	

	if(request_irq(CONFIG_PDMA_IRQ_1, qdma_isr_1, 0, pdma_isr1_name, NULL) != 0) {
		printk("Request the interrupt service routine fail, irq:%d.\n", CONFIG_PDMA_IRQ_1) ;
		return -ENODEV ;
	}
#endif

	if(tso_proc_init())
	{
		printk("tsp_proc_init failed!/n");
		return -ENODEV;
	}

#ifdef TCSUPPORT_CPU_ARMV8
	if( (dev=get_frame_engine_dev()) == NULL ) {
		printk("Get device failed.\n") ; 
		return -ENODEV ;
	}
	if(request_irq(get_pdma_irq(), pdma_isr, 0, "pdma_int", dev) != 0) {
		printk("Request the interrupt service routine fail, irq:%d.\n", get_pdma_irq()) ;
		return -ENODEV ;
	}
#endif

}

static int tso_exit(void)
{
	struct PDMA_DscpInfo_S *diPtr=NULL ;
	uint totalDscpNum=0, base=0 ;
	int i=0, ringIdx=0 ;

	tso_proc_deinit();
	PDMA_LOG("tso_proc_deinit  success.\n") ; 

	for(ringIdx=0; ringIdx<PDMA_TX_RING_NUM; ringIdx++)
		totalDscpNum += tsoPdmaPriv->txDscpNum[ringIdx] ;

#ifdef TCSUPPORT_CPU_ARMV8
	free_irq(get_pdma_irq(), NULL) ;
#endif

	if(tsoPdmaPriv->dscpInfoAddr) {
		for(i=0 ; i<totalDscpNum ; i++) {
			diPtr = (struct PDMA_DscpInfo_S *)tsoPdmaPriv->dscpInfoAddr + i ;

			if(diPtr->skb && diPtr->dscpPtr->pkt_addr) {
				dma_unmap_single(NULL, diPtr->dscpPtr->pkt_addr, (size_t)diPtr->dscpPtr->ctrl.pkt_len, DMA_BIDIRECTIONAL) ;
			}
		}
		PDMA_LOG("tso_proc_deinit  step1.\n") ;
		diPtr = (struct PDMA_DscpInfo_S *)tsoPdmaPriv->dscpInfoAddr ;
		if(diPtr->dscpPtr) {
			dma_free_coherent(NULL, sizeof(struct PDMA_DscpInfo_S)*totalDscpNum, (void *)diPtr->dscpPtr, IO_GREG(PDMA_TX0_BASE_PTR)) ;
			diPtr->dscpPtr = NULL ;
		}
		PDMA_LOG("tso_proc_deinit  step2.\n") ;	
		kfree(diPtr) ;
		tsoPdmaPriv->dscpInfoAddr = 0 ;
	}

	kfree(tsoPdmaPriv);
	PDMA_LOG("kfree(tsoPdmaPriv)  success.\n") ; 
	return 0;
}


module_init(tso_init);
module_exit(tso_exit);

