#include <linux/proc_fs.h>
#include "hostadpt.h"

//extern struct device* get_gdmpSram_dev(void);
extern struct device* get_ecnt_npu_dev(void);
volatile unsigned short int reTryTimes;
unsigned int hostaptDebug = DBG_OFF;

HOSTADPT_DMA_DSCP_T *pHostadptRX_BASE[HOSTADPT_RX_RING_NUM] = {NULL};
u32 hostadpt_rx_dscp_max_num[HOSTADPT_RX_RING_NUM] = {HOSTADPT_RX0_DSCP_NUM, HOSTADPT_RX1_DSCP_NUM};
u32 hostadpt_rx_dma_idx[HOSTADPT_RX_RING_NUM] = {0};
u32 hostadpt_rx_cpu_idx[HOSTADPT_RX_RING_NUM] = {0};
unsigned long int *HOSTADPT_CNT_Base, *HOSTADPT_CNT_2GBase;
#ifdef TCSUPPORT_NPU_V2
#define HOSTADPT_COUNTER(addr,i)		(*((unsigned long int *)((unsigned long int)addr + (i << 3))))
#else 
#define HOSTADPT_COUNTER(addr,i)		(*((unsigned long int *)((unsigned long int)addr + (i << 2))))
#endif

extern int get_npu_irq(int index);

#if defined(TCSUPPORT_WLAN_INODE) || defined(TCSUPPORT_NPU_WIFI_TX)
u32 hostadpt_tx_dma_idx[2] = {0};
u32 hostadpt_tx_cpu_idx[2] = {0};
u32 hostadpt_tx_skbin_cnt[2] = {0};
u32 hostadpt_tx_skbfree_cnt[2] = {0};
int host_apd_debug[2] = {0};
static DEFINE_SPINLOCK(hostadpt_lock_2G);
#endif

static DEFINE_SPINLOCK(hostadpt_lock_test);
static int hostadpt_dbg_counter(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{

	//printk("------NPU Hostadpt Counter (5G)----------\n");
	printk("------NPU Hostadpt Counter (6G)----------\n");
	printk("GET_NORMAL_SIZE_PKT:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,GET_NORMAL_PKT));
	printk("GET_BIGGER_PKT:\t\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,GET_BIGGER_PKT));
	printk("BIGPKT_CHECK_PKT_LOSE:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,BIGPKT_CHECK_PKT_LOSE));
	printk("BIGPKT_CHECK_PKT_LOSE1:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,BIGPKT_CHECK_PKT_LOSE1));
	printk("BIGPKT_CHECK_PKT_LOSE2:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,BIGPKT_CHECK_PKT_LOSE2));
	printk("BIGPKT_CHECK_PKT_LOSE3:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,BIGPKT_CHECK_PKT_LOSE3));
	printk("BIGPKT_CHECK_PKT_OVER_RETRY:\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,BIGPKT_CHECK_PKT_OVER_RETRY));
	printk("BIGPKT_COMBINE_PKT_FAIL:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,BIGPKT_COMBINE_PKT_FAIL));
	printk("BIGPKT_COMBINE_PKT_DONE:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,BIGPKT_COMBINE_PKT_DONE));
	printk("ALLOC_NEW_SKB_FAIL:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,ALLOC_NEW_SKB_FAIL));
	printk("SEND_PKT_TO_DRIVER:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,SEND_PKT_TO_DRIVER));
	printk("TX_GET_PKT_FROM_TXRING:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,TX_GET_PKT_FROM_TXRING));
	printk("TX_Q_IS_FULL:\t\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_Base,TX_Q_IS_FULL));

	//printk("------NPU Hostadpt Counter (2.4G)----------\n");
	printk("------NPU Hostadpt Counter (2.4G/5G)----------\n");
	printk("GET_NORMAL_SIZE_PKT:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,GET_NORMAL_PKT));
	printk("GET_BIGGER_PKT:\t\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,GET_BIGGER_PKT));
	printk("BIGPKT_CHECK_PKT_LOSE:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,BIGPKT_CHECK_PKT_LOSE));
	printk("BIGPKT_CHECK_PKT_LOSE1:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,BIGPKT_CHECK_PKT_LOSE1));
	printk("BIGPKT_CHECK_PKT_LOSE2:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,BIGPKT_CHECK_PKT_LOSE2));
	printk("BIGPKT_CHECK_PKT_LOSE3:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,BIGPKT_CHECK_PKT_LOSE3));
	printk("BIGPKT_CHECK_PKT_OVER_RETRY:\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,BIGPKT_CHECK_PKT_OVER_RETRY));
	printk("BIGPKT_COMBINE_PKT_FAIL:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,BIGPKT_COMBINE_PKT_FAIL));
	printk("BIGPKT_COMBINE_PKT_DONE:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,BIGPKT_COMBINE_PKT_DONE));
	printk("ALLOC_NEW_SKB_FAIL:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,ALLOC_NEW_SKB_FAIL));
	printk("SEND_PKT_TO_DRIVER:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,SEND_PKT_TO_DRIVER));
	printk("TX_GET_PKT_FROM_TXRING:\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,TX_GET_PKT_FROM_TXRING));
	printk("TX_Q_IS_FULL:\t\t\t%lu\n",HOSTADPT_COUNTER(HOSTADPT_CNT_2GBase,TX_Q_IS_FULL));
	return 0;
}
static int retry_times_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;

	len += sprintf(buf +len, "%d\n", reTryTimes);
	if(len > PAGE_SIZE)
	{
		return -ENOBUFS;
	}
	return len;
}

static int retry_times_proc_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char buf[16];
	unsigned long int len = count;
	unsigned short int n;

	//printk();
	if(len >= sizeof(buf))
	{
		len = sizeof(buf) - 1;
	}
	
	if(copy_from_user(buf, buffer, len))
	{
		return -EFAULT;
	}
	buf[len] = "\n";

	n = simple_strtol(buf, NULL, 10);
	if(n > 0)
	{
		reTryTimes = n;
		printk("[HOSTADPT]retry times is %d\n", reTryTimes);
	}
	else
		printk("retry times should be greater than 0\n");
	
	return len;
}

static int hostapt_dbg_flag_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;

	len += sprintf(buf +len, "%d\n", hostaptDebug);
	if(len > PAGE_SIZE)
	{
		return -ENOBUFS;
	}
	return len;
}

static int hostapt_dbg_flag_proc_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char buf[16];
	unsigned long int len = count;
	unsigned short int n;

	//printk();
	if(len >= sizeof(buf))
	{
		len = sizeof(buf) - 1;
	}
	
	if(copy_from_user(buf, buffer, len))
	{
		return -EFAULT;
	}
	buf[len] = "\n";

	n = simple_strtol(buf, NULL, 10);
	if(n > 0)
	{
		if(n >= DBG_MAX)
			n = DBG_MAX - 1;
		
		hostaptDebug = n;	
		printk("[HOSTADPT]hostaptDebug is %d\n", hostaptDebug);
	}
	else
		printk("hostaptDebug should be greater than 0\n");
	
	return len;
}


void HOST_PKT_COUNTER(unsigned long int offset, unsigned char ring_idx)
{
	unsigned long int *COUNTER_ADDR = NULL;
	if(offset >= HADPT_CNT_BOTTOM)
	{
		return;
	}
	if(ring_idx)
	{
#ifdef TCSUPPORT_NPU_V2
		COUNTER_ADDR = (unsigned long int *)((unsigned long int)HOSTADPT_CNT_Base + (offset << 3));
#else 
		COUNTER_ADDR = (unsigned long int *)((unsigned long int)HOSTADPT_CNT_Base + (offset << 2));
#endif
	}
	else
	{
#ifdef TCSUPPORT_NPU_V2
		COUNTER_ADDR = (unsigned long int *)((unsigned long int)HOSTADPT_CNT_2GBase + (offset << 3));
#else 
		COUNTER_ADDR = (unsigned long int *)((unsigned long int)HOSTADPT_CNT_2GBase + (offset << 2));
#endif
	}
	//if(*COUNTER_ADDR == 0xffffffff) (*COUNTER_ADDR) = 0;
	//else
	(*COUNTER_ADDR) += 1;
	return;

}

static int hostadpt_rxdscp_init(void)
{
	dma_addr_t dscpDmaAddr=0;
	int i;
	u32 dmaIdx = 0;
	struct device *dev = NULL ;
	unsigned long dscpBaseAddr=0 ;
	HOSTADPT_DMA_DSCP_T *dscp = NULL;
	struct sk_buff *skb = NULL;
	struct proc_dir_entry *entry, *dbg_entry;
	struct proc_dir_entry *entry_debug;
	unsigned long int hostadpt_cnt_pa, hostadpt_cnt_pa2;

	/******************************************
	* Allocate descriptor DMA memory		  *
	*******************************************/
	if( (dev=(struct device *)get_ecnt_npu_dev()) == NULL ) {
		HOSTADPT_MSG(DBG_ERR, "Get device failed.\n") ; 
		return -ENOMEM ;
	}
	//alloc for dbg counter
	HOSTADPT_CNT_Base = dma_alloc_coherent(dev, sizeof(unsigned long int) * HADPT_CNT_BOTTOM, &hostadpt_cnt_pa, GFP_KERNEL);
	memset(HOSTADPT_CNT_Base, 0x0, (HADPT_CNT_BOTTOM*sizeof(unsigned long int)));
	HOSTADPT_CNT_2GBase = dma_alloc_coherent(dev, sizeof(unsigned long int) * HADPT_CNT_BOTTOM, &hostadpt_cnt_pa2, GFP_KERNEL);
	memset(HOSTADPT_CNT_2GBase, 0x0, (HADPT_CNT_BOTTOM*sizeof(unsigned long int)));
	//printk("%d alloc size:%d*%d ", __LINE__, sizeof(HOSTADPT_DMA_DSCP_T), HOSTADPT_RX_TOTAL_DSCP_NUM);

#ifdef TCSUPPORT_NPU_V2
	dscpBaseAddr = (unsigned long)dma_alloc_coherent(dev, sizeof(HOSTADPT_DMA_DSCP_T)*HOSTADPT_RX_TOTAL_DSCP_NUM, &dscpDmaAddr, (GFP_KERNEL|__GFP_NOWARN|__GFP_DMA)) ;
#else
	dscpBaseAddr = (unsigned long)dma_alloc_coherent(dev, sizeof(HOSTADPT_DMA_DSCP_T)*HOSTADPT_RX_TOTAL_DSCP_NUM, &dscpDmaAddr, GFP_KERNEL) ;
#endif
	if(!dscpBaseAddr) {
		HOSTADPT_MSG(DBG_ERR, "Allocate memory for TX/RX DSCP failed.\n") ; 
		return -ENOMEM ;
	}

	pHostadptRX_BASE[0] = (HOSTADPT_DMA_DSCP_T *)dscpBaseAddr ;
	pHostadptRX_BASE[1] = pHostadptRX_BASE[0] + HOSTADPT_RX0_DSCP_NUM;

	HOSTADPT_MSG(DBG_LOG, "dscpDmaAddr =%x\n",dscpDmaAddr) ; 

	/***************************************************
	* Initialization first DSCP for Rx DMA			  *
	****************************************************/
	dscp = pHostadptRX_BASE[0];
	for(i=0; i<HOSTADPT_RX_TOTAL_DSCP_NUM; i++) {		
		skb = alloc_skb(HOSTAPD_BUFFER_LEN, GFP_KERNEL); 
		if(skb == NULL)
		{
			HOSTADPT_MSG(DBG_ERR, "alloc skb faild!");
			return -ENOMEM ;
		}
#if defined(TCSUPPORT_CPU_ARMV8_64)
		dscp->skb_p = (u64)skb;
#else
		dscp->skb_p = (u32)skb;
#endif
		dscp->pkt_addr = dma_map_single(dev, (void *)((unsigned long)skb->data), HOSTAPD_BUFFER_LEN, DMA_TO_DEVICE);
		dscp->pkt_info.pkt_idx = 0;
		dscp->pkt_ctrl.is_last = 0;
		dscp->pkt_ctrl.done = 0;
		//dma_unmap_single(dev, dscp->pkt_addr, HOSTAPD_BUFFER_LEN, DMA_FROM_DEVICE);
		dscp++;	
	}
	
	
	//write rx0/rx1 num to reg
	set_npu_hostadpt_reg(HOSTADPT_RX0_MAX_CNT, HOSTADPT_RX0_DSCP_NUM);
	set_npu_hostadpt_reg(HOSTADPT_RX1_MAX_CNT, HOSTADPT_RX1_DSCP_NUM);
	
	//set rx0/rx1 hostadpt_rx_cpu_idx/hostadpt_rx_dma_idx 
	set_npu_hostadpt_reg(HOSTADPT_RX0_CPU_IDX, 0);
	set_npu_hostadpt_reg(HOSTADPT_RX1_CPU_IDX, 0);
	
	set_npu_hostadpt_reg(HOSTADPT_RX0_DMA_IDX, 0);
	set_npu_hostadpt_reg(HOSTADPT_RX1_DMA_IDX, 0);
	
	//write rx0/rx1 base to reg
	//Please add other code before this line which is about initialization. 
	set_npu_hostadpt_reg(HOSTADPT_RX0_BASE_PTR, dscpDmaAddr);
	set_npu_hostadpt_reg(HOSTADPT_RX1_BASE_PTR, dscpDmaAddr + sizeof(HOSTADPT_DMA_DSCP_T)*HOSTADPT_RX0_DSCP_NUM);

	// create a proc for show debug counter
	dbg_entry = create_proc_entry("tc3162/hostadpt_dbg_counter", 0666, NULL);
	if(dbg_entry == NULL)
	{
		printk(KERN_WARNING"proc: unable to create proc(hostadpt_dbg_counter) entry\n");

	}
	else
	{
		dbg_entry->read_proc = hostadpt_dbg_counter;
	}

	// create a proc for setting retry times
	entry = create_proc_entry("tc3162/hostadpt_set_retry_times", 0666, NULL);
	if(entry == NULL)
	{
		printk(KERN_WARNING"proc: unable to create proc entry\n");
	}
	else
	{
		entry->read_proc = retry_times_proc_read;
		entry->write_proc = retry_times_proc_write;
		reTryTimes = 3;
	}

	// create a proc for setting retry times
	entry_debug = create_proc_entry("tc3162/hostadpt_set_dbg_flag", 0666, NULL);
	if(entry_debug == NULL)
	{
		printk(KERN_WARNING"proc: unable to create proc entry\n");
	}
	else
	{
		entry_debug->read_proc = hostapt_dbg_flag_proc_read;
		entry_debug->write_proc = hostapt_dbg_flag_proc_write;
	}

	return 0;

}


int rxdone_cb(struct sk_buff *skb)
{
	u32 j = 0, len = skb->len;
	u32 *data = skb->data;
	printk("host adaptor received data:\n");
	for(j = 0; j < len; j++, data++)
	{
		if(j % 20 == 0) printk("\n");
		printk("%d\t", *data);			
	}
	printk("\n");
	//free 
	kfree_skb(skb);
	return 0;
}
extern int wifiqueue_start_idx;
extern int (*getWifiQueueIndex_hook)(struct net_device *dev,unsigned char direction);
extern int (*soft_ratelimit_check_queue_hook)(unsigned int queue_idx);
#define WIFI_RATELIMIT_RX 0

struct sk_buff *hostadpt_rx_handler(u32 ring_idx, unsigned char* preschedule, unsigned int* vnd_spec)
{	
	HOSTADPT_DMA_DSCP_T *pHostadptRX = NULL, *pHostadptRX_tmp= NULL;  //*pHostadptRX_tmp2=NULL;
	struct sk_buff *skb = NULL;
	char scatter_err_flag = 0;
	struct sk_buff *skb_tmp = NULL;
	struct sk_buff *skb_all = NULL;
	struct sk_buff *skb_new = NULL;
	struct device *dev = NULL ;
	unsigned char pkt_idx=0, pkt_cnt=0, allPkt_done=0, scatter_cnt = 0;//, last_cnt = 0;
	unsigned char pkt_cnt_dbg = 0;
	u32 cpu_idx_tmp = 0, try_times=0;// ,pkt_len;
	static unsigned short cur_pktSize = 0,total_pktSize = 0;
	
        unsigned char checkfail = 0;
	unsigned int realcopylen = 0;
	u32 	pkt_addr = 0;
	u32 	scatter_pkt_len = 0;
	u32 cur_len,single_total_len,all_total_len;
	u8 is_last,ddonebit;
	HOSTADPT_MSG(DBG_LOG, "RX%d enter hostadpt_rx_handler...\n", ring_idx);
 
	if(ring_idx >= HOSTADPT_RX_RING_NUM)
	{
		HOSTADPT_MSG(DBG_ERR, "ring idx error!\n");
		return NULL;
	}
	
	pHostadptRX =pHostadptRX_BASE[ring_idx] + hostadpt_rx_cpu_idx[ring_idx];
	
	if(pHostadptRX->pkt_ctrl.done == 0) 
	{
		HOSTADPT_MSG(DBG_LOG, "no data to receive!\n");
		return NULL;
	}
	HOSTADPT_MSG(DBG_LOG, "old addr: %x\n",pHostadptRX->pkt_addr);
	HOSTADPT_MSG(DBG_LOG, "pkt len: %d\n",pHostadptRX->pkt_ctrl.len);
	HOSTADPT_MSG(DBG_LOG, "recv done: %d\n",pHostadptRX->pkt_ctrl.done);
	HOSTADPT_MSG(DBG_LOG, "foe_number: %d\n",pHostadptRX->pkt_info.foe_number);
	HOSTADPT_MSG(DBG_LOG, "crsn: %d\n",pHostadptRX->pkt_info.crsn);

	*vnd_spec = pHostadptRX->vend_specific;
	if(1)
	{
		if(pHostadptRX->pkt_ctrl.len != pHostadptRX->pkt_ctrl.cur_len)
		{
			HOST_PKT_COUNTER(GET_BIGGER_PKT, ring_idx);
			
			cpu_idx_tmp = hostadpt_rx_cpu_idx[ring_idx];
			pkt_cnt = (pHostadptRX->pkt_info.pkt_idx >> 3);
			scatter_pkt_len = pHostadptRX->pkt_ctrl.len;
			//scatter_cnt = 0;	
			all_total_len = 0;
			HOSTADPT_MSG(DBG_CATTER, "scatter handle\n");
			HOSTADPT_MSG(DBG_CATTER, "scatter_pkt_len: %d,pkt_cnt=%d\n",scatter_pkt_len,pkt_cnt);
			while(1)
			{
				pHostadptRX_tmp =pHostadptRX_BASE[ring_idx] + cpu_idx_tmp;
				ddonebit = pHostadptRX_tmp->pkt_ctrl.done;
				rmb();
				pkt_idx = pHostadptRX_tmp->pkt_info.pkt_idx & 0x7;
				cur_len = pHostadptRX_tmp->pkt_ctrl.cur_len;
				single_total_len = pHostadptRX_tmp->pkt_ctrl.len;
				is_last = pHostadptRX_tmp->pkt_ctrl.is_last;
				pkt_cnt_dbg = (pHostadptRX_tmp->pkt_info.pkt_idx >> 3);
				HOSTADPT_MSG(DBG_CATTER, "pkt_idx: %d,cur_len: %d,len:%d,is_last: %d,ddonebit: %d\n",pkt_idx,cur_len,single_total_len,is_last,ddonebit);
				if(ddonebit == 0)
				{
					try_times++;
					udelay(1000);
				}
				else
				{
					try_times = 0;
					scatter_cnt = scatter_cnt+1;	
					all_total_len += cur_len;
					if(cur_len == single_total_len)
					{
						/*normal packet within scatter packet*/
						scatter_cnt = scatter_cnt - 1;
						if(scatter_cnt != pkt_cnt || (pkt_idx == 0 && is_last == 0x1))
						{
							//printk("desc_pkt_cnt:%d cur_len:%d single_total_len:%d desc_cur_len:%d desc_single_total_len:%d\n",
							//	(pHostadptRX->pkt_info.pkt_idx >> 3), cur_len, single_total_len,
							//	pHostadptRX_tmp->pkt_ctrl.cur_len, pHostadptRX_tmp->pkt_ctrl.len);
							//printk("cur CIDX:%d ori CIDX:%d cur pkt_cnt:%d is_last:%d\n",
							//	cpu_idx_tmp, hostadpt_rx_cpu_idx[ring_idx], pkt_cnt_dbg, is_last);
							//printk("scatter_cnt:%d pkt_cnt:%d pkt_idx:%d cur_len:%d scatter_pkt_len:%d\n\n",
							//	scatter_cnt, pkt_cnt, pkt_idx, cur_len, scatter_pkt_len);
							checkfail = 1;
							HOST_PKT_COUNTER(BIGPKT_CHECK_PKT_LOSE1, ring_idx);
							HOSTADPT_MSG(DBG_CATTER, "BIGPKT_CHECK_PKT_LOSE1\n");
							goto error;
						}
					}
					else
					{
						/*scatter packet handle*/
						if(scatter_cnt != pkt_idx || scatter_pkt_len !=single_total_len)
						{
							checkfail = 1;
							//printk("desc_pkt_idx:%d desc_scatter_pkt_len:%d desc_single_total_len:%d \n",
							//	(pHostadptRX_tmp->pkt_info.pkt_idx & 0x7), pHostadptRX->pkt_ctrl.len,
							//	pHostadptRX_tmp->pkt_ctrl.len);
							//printk("cur CIDX:%d ori CIDX:%d cur pkt_cnt:%d is_last:%d\n",
							//	cpu_idx_tmp, hostadpt_rx_cpu_idx[ring_idx], pkt_cnt_dbg, is_last);
							//printk("scatter_cnt:%d pkt_cnt:%d pkt_idx:%d scatter_pkt_len:%d single_total_len:%d\n\n",
							//	scatter_cnt, pkt_cnt, pkt_idx, scatter_pkt_len, single_total_len);
							HOST_PKT_COUNTER(BIGPKT_CHECK_PKT_LOSE2, ring_idx);
							HOSTADPT_MSG(DBG_CATTER, "BIGPKT_CHECK_PKT_LOSE2\n");
							goto error;
						}

						if(is_last == 0x1)
						{
							allPkt_done = 1;
							if(scatter_cnt != pkt_cnt || scatter_pkt_len !=single_total_len || scatter_pkt_len !=all_total_len)
							{
									checkfail = 1;
									if(all_total_len != scatter_pkt_len)
									{
											HOSTADPT_MSG(DBG_CATTER, "hostapterror check:temptotallen=%d,scatter_pkt_len=%d,pkt_cnt=%d,scatter_cnt=%d,len=%d,cur_len=%d!\n",all_total_len,scatter_pkt_len,pkt_cnt,scatter_cnt,scatter_pkt_len,single_total_len);
									}
									HOSTADPT_MSG(DBG_CATTER, "BIGPKT_CHECK_PKT_LOSE3\n");
									HOST_PKT_COUNTER(BIGPKT_CHECK_PKT_LOSE3, ring_idx);
									goto error;
							}
							break;
						}						
					}
											
					cpu_idx_tmp++;
					if(cpu_idx_tmp >=  hostadpt_rx_dscp_max_num[ring_idx])
						cpu_idx_tmp = 0;
						

				}
error:				
				if((try_times > reTryTimes) || checkfail/*|| (scatter_cnt > pkt_cnt)*/)
				{
					//printk("[Hostadpt] get all scatter packets fail.\n");
					HOSTADPT_MSG(DBG_CATTER, " get all scatter packets fail,checkfail=%d,current try_times=%d\n",checkfail,try_times);
					allPkt_done = 0;
					try_times = 0;
					if(checkfail)
						HOST_PKT_COUNTER(BIGPKT_CHECK_PKT_LOSE, ring_idx);
					else
						HOST_PKT_COUNTER(BIGPKT_CHECK_PKT_OVER_RETRY, ring_idx);
					//scatter_cnt = 0;
					break;
				}
				
			}

			while(scatter_cnt > 0)
			{
				HOSTADPT_MSG(DBG_CATTER, "allPkt_done =% d,scatter_cnt=%d\n",allPkt_done,scatter_cnt);
				realcopylen = 0;
				pHostadptRX =pHostadptRX_BASE[ring_idx] + hostadpt_rx_cpu_idx[ring_idx];
				if(allPkt_done)
				{
					HOSTADPT_MSG(DBG_CATTER, "[Hostadpt]scatter handle begin\n");
					pkt_idx = (pHostadptRX->pkt_info.pkt_idx & 0x7);
					pkt_cnt = (pHostadptRX->pkt_info.pkt_idx >> 3);
					skb_tmp = (struct sk_buff*)pHostadptRX->skb_p;
					cur_len = pHostadptRX->pkt_ctrl.cur_len;
					single_total_len = pHostadptRX->pkt_ctrl.len;
					is_last = pHostadptRX->pkt_ctrl.is_last;
					HOSTADPT_MSG(DBG_CATTER, "pkt_idx: %d,pkt_cnt: %d,cur_len; %d,len:%d,is_last: %d\n",pkt_idx,pkt_cnt,cur_len,single_total_len,pHostadptRX->pkt_ctrl.is_last);
					if(is_last == 0x0)
					{
						if(pkt_idx == 1)
						{
							//skb_all = dev_alloc_skb(pHostadptRX->pkt_ctrl.len);
							skb_all = alloc_skb(single_total_len+1, GFP_KERNEL); 
							total_pktSize = single_total_len;
							cur_pktSize = 0;
							*vnd_spec = pHostadptRX->vend_specific;
						}
						if(skb_all)
						{
							if(cur_pktSize+cur_len <= total_pktSize)
								realcopylen = cur_len;
							else
								realcopylen = total_pktSize - cur_pktSize;

							if(realcopylen)
							{
								
								if(skb_all->tail + realcopylen > skb_all->end){
								/*	printk("000len:%d head:%p data:%p tail:%#lx end:%#lx dev:%s\n",
		 		 						realcopylen , skb_all->head, skb_all->data,
		 								(unsigned long)skb_all->tail, (unsigned long)skb_all->end,
			 							skb_all->dev ? skb_all->dev->name : "<NULL>"); */
									scatter_err_flag = 1;
								}else
								{
								memcpy(skb_all->data+cur_pktSize, skb_tmp->data, realcopylen);
								skb_put(skb_all, realcopylen);
							}
						}
						}
						else
						{
							HOST_PKT_COUNTER(BIGPKT_COMBINE_PKT_FAIL, ring_idx);
							HOSTADPT_MSG(DBG_CATTER, "[Hostadpt] Alloc skb for scatter packet(%d/%d/%d), but skb is NULL\n",pkt_idx, pkt_cnt,scatter_cnt);
						}
						//isLastPkt = 0;
						cur_pktSize = cur_pktSize+realcopylen;		
					}
					else if(is_last == 0x1)
					{
						if(skb_all)
						{
							if(cur_pktSize+cur_len <= total_pktSize)
								realcopylen = cur_len;
							else{
								realcopylen = total_pktSize - cur_pktSize;
							}
							
							if(realcopylen)
							{
								if(skb_all->tail + realcopylen > skb_all->end){
								/*	printk("111len:%d head:%p data:%p tail:%#lx end:%#lx dev:%s\n",
		 		 						realcopylen , skb_all->head, skb_all->data,
		 								(unsigned long)skb_all->tail, (unsigned long)skb_all->end,
			 							skb_all->dev ? skb_all->dev->name : "<NULL>");*/
									scatter_err_flag = 1;
								}else
								{
								memcpy(skb_all->data+cur_pktSize, skb_tmp->data, realcopylen);
								skb_put(skb_all,realcopylen);
							}
							}
							
							skb = skb_all;
							HOST_PKT_COUNTER(BIGPKT_COMBINE_PKT_DONE, ring_idx);
							HOSTADPT_MSG(DBG_CATTER, "[Hostadpt]scatter handle end\n");
						}
						else
						{
							HOST_PKT_COUNTER(BIGPKT_COMBINE_PKT_FAIL, ring_idx);
							HOSTADPT_MSG(DBG_CATTER, "[Hostadpt] Alloc skb for scatter packet(%d/%d/%d), but skb is NULL,is_last=%d\n",pkt_idx, pkt_cnt,scatter_cnt,pHostadptRX->pkt_ctrl.is_last);
							skb = NULL;
						}
						cur_pktSize = 0;	
						//isLastPkt = 1;
					}
				}

				scatter_cnt--;
				pHostadptRX->pkt_ctrl.is_last = 0;
				pHostadptRX->pkt_info.pkt_idx = 0;
				pHostadptRX->pkt_ctrl.done = 0;
				hostadpt_rx_cpu_idx[ring_idx]++;
				if(hostadpt_rx_cpu_idx[ring_idx] >= hostadpt_rx_dscp_max_num[ring_idx]){
					hostadpt_rx_cpu_idx[ring_idx] = 0;			
			}

				if(1 == scatter_err_flag){		//scatter err handle
					if(skb_all){						
						dev_kfree_skb_any(skb_all);
					}
					skb = NULL;
					break;
				}					
			}
		}
		else
		{
			HOST_PKT_COUNTER(GET_NORMAL_PKT, ring_idx);
			skb = (struct sk_buff *)pHostadptRX->skb_p;
	                //skb_put(skb,pHostadptRX->pkt_ctrl.len);

			skb_new = skbmgr_dev_alloc_skb4k();	
			if(skb_new == NULL)
			{
					HOST_PKT_COUNTER(ALLOC_NEW_SKB_FAIL, ring_idx);
				HOSTADPT_MSG(DBG_ERR, "alloc new skb failed!\n");
				skb_new = skb;
				skb = NULL;
					pHostadptRX->pkt_ctrl.done = 0;
					pHostadptRX->pkt_info.pkt_idx = 0;
					pHostadptRX->pkt_ctrl.is_last = 0;
			}
			else
			{
				if ((dev=(struct device *)get_ecnt_npu_dev())==NULL) 
				{
					HOSTADPT_MSG(DBG_ERR, "\nget dev failed\n");
							dev_kfree_skb_any(skb_new);
							return NULL;
				}
				pkt_addr = dma_map_single(dev, (void *)((unsigned long)skb_new->data), HOSTAPD_BUFFER_LEN, DMA_FROM_DEVICE);
					if(dma_mapping_error(dev, pkt_addr)){
						HOSTADPT_MSG(DBG_ERR, "!!!! Hostadpt dma_mapping_error!!!!\n");
						dev_kfree_skb_any(skb_new);
						return NULL;
				}else{

					// unmap old skb->data
					dma_unmap_single(dev, pHostadptRX->pkt_addr, HOSTAPD_BUFFER_LEN, DMA_FROM_DEVICE);
					pHostadptRX->pkt_addr = pkt_addr; 
								HOSTADPT_MSG(DBG_LOG, "new addr: %x\n",pHostadptRX->pkt_addr);
					pHostadptRX->pkt_info.pkt_idx = 0;
					pHostadptRX->pkt_ctrl.is_last = 0;
					pHostadptRX->pkt_ctrl.done = 0;	
#if defined(TCSUPPORT_CPU_ARMV8_64)
					pHostadptRX->skb_p = (u64)skb_new;
#else
					pHostadptRX->skb_p = (u32)skb_new;
#endif
			 	}
			}
			
			if(skb != NULL){
				//skb_put should be here,avoiding multi put happens if skb_put exed before "return NULL"
				if(skb->tail + pHostadptRX->pkt_ctrl.len > skb->end){
				/*	printk("222 len:%d head:%p data:%p tail:%#lx end:%#lx dev:%s\n",
		 		 		pHostadptRX->pkt_ctrl.len , skb->head, skb->data,
		 				(unsigned long)skb->tail, (unsigned long)skb->end,
			 			skb->dev ? skb->dev->name : "<NULL>");*/
					dev_kfree_skb_any(skb);
					skb = NULL;
				}else{ 
				   skb_put(skb, pHostadptRX->pkt_ctrl.len);
				 }
			}
			
			wmb();

			hostadpt_rx_cpu_idx[ring_idx]++;
			if(hostadpt_rx_cpu_idx[ring_idx] >= hostadpt_rx_dscp_max_num[ring_idx])
				hostadpt_rx_cpu_idx[ring_idx] = 0;
		}	
	}
	if(ring_idx == 0)
		set_npu_hostadpt_reg(HOSTADPT_RX0_CPU_IDX, hostadpt_rx_cpu_idx[ring_idx]);	
	else if(ring_idx == 1)
		set_npu_hostadpt_reg(HOSTADPT_RX1_CPU_IDX, hostadpt_rx_cpu_idx[ring_idx]);	
		
	if(skb != NULL){
		//skb_put(skb, pHostadptRX->pkt_ctrl.len);
		//*preschedule = pHostadptRX->pkt_ctrl.reschedule;
		if (likely(ra_sw_nat_hook_rxinfo))
		{
#ifdef TCSUPPORT_SOFT_RATELIMIT		
			if(pHostadptRX->vend_specific==1){//for softratelimit of mt7916
				return skb;
			}
			else
			{
#endif			
			pHostadptRX->pkt_info.src_port = 1;
#if (defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7551)) && (defined(TCSUPPORT_WLAN_MT7990) || defined(TCSUPPORT_WLAN_MT7992) || defined(TCSUPPORT_WLAN_MT7993))
            //#define SKB_FROM_WIFI_BIT (1<<15), defined in skb
            skb->mark2 |= SKB_FROM_WIFI_BIT; // Bit will be checked in hw_nat, if this bit was set, packet from wlan to lan/wan will send to QDMA fast path.
#endif
			ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GE, (char*)&pHostadptRX->pkt_info, 4);
#ifdef TCSUPPORT_SOFT_RATELIMIT			
			}
#endif			
		}			
		HOST_PKT_COUNTER(SEND_PKT_TO_DRIVER, ring_idx);
	}else{
		*preschedule = 0;
	}
	return skb;
}



void (*tasklet_schedule_WIFI2G)(void);
void (*tasklet_schedule_WIFI5G)(void);
static irqreturn_t rxdone0_isr(int irq, void *dev_id)
{
	unsigned long int status, tmp;
	//HOSTADPT_MSG(DBG_LOG, "rxdone0 interrput......\n"); 
	status = get_npu_hostadpt_reg(0x0030);
	//printk("host apd 0 interrput status: %x\n",status);
	set_npu_hostadpt_reg(0x0030,0x00010000);
	//status = get_npu_hostadpt_reg(0x0030);
	//printk("host apd 0 interrput status: %x\n",status);
	if(tasklet_schedule_WIFI2G){
		//printk("host apd 0 tasklet_schedule_WIFI2G: %x\n",tasklet_schedule_WIFI2G);
		tasklet_schedule_WIFI2G();
	
		tmp = get_npu_hostadpt_reg(HOSTADPT_TXRXDONE0_INT_MASK);
		tmp &= ~(0x00010000);
		set_npu_hostadpt_reg(HOSTADPT_TXRXDONE0_INT_MASK,tmp);
	}	
	//if(temp_rx_data_done){
	//	printk("Hostadp : temp_rx_data_done %x \n", temp_rx_data_done);
	//	tasklet_hi_schedule(temp_rx_data_done);
	//}	
    return IRQ_HANDLED;
}

static irqreturn_t rxdone1_isr(int irq, void *dev_id)
{
	unsigned long int status, tmp;
	//status = get_npu_hostadpt_reg(0x0030);
	//printk("Hostadp : rxdone1_isr interrput status: %x\n",status);
	set_npu_hostadpt_reg(0x0030,0x00020000);
	//status = get_npu_hostadpt_reg(0x0030);
	//printk("host apd 1 interrput status: %x\n",status);
	if(tasklet_schedule_WIFI5G){
		//printk("Hostadp : rxdone1_isr tasklet_schedule_WIFI5G: %x\n",tasklet_schedule_WIFI5G);
		tasklet_schedule_WIFI5G();

		tmp = get_npu_hostadpt_reg(HOSTADPT_TXRXDONE1_INT_MASK);
		tmp &= ~(0x00020000);
		set_npu_hostadpt_reg(HOSTADPT_TXRXDONE1_INT_MASK,tmp);
	}	
	//if(temp_rx_data_done5G){
	//	printk("Hostadp : temp_rx_data_done5G %x\n", temp_rx_data_done5G);
	//	tasklet_hi_schedule(temp_rx_data_done5G);
	//}	
    return IRQ_HANDLED;
}

void hostadpt_regiter_interrupt(u32 irq_num, irqreturn_t (*int_isr)(int, void*), char *irq_name)
{
	if(irq_num > 5)
	{
		HOSTADPT_MSG(DBG_ERR, "irq num error, valid irq num : 0 - 5 \n");
		return ;
	}
	if(request_irq(get_npu_irq(irq_num), int_isr, 0, irq_name, NULL)!=0)
	{
		printk("request_irq failed.\n");
	}
	HOSTADPT_MSG(DBG_LOG, "interrupt register!\n");
	return ;
}

void hostadpt_free_interrupt(u32 irq_num)
{
	if(irq_num > 5)
	{
		HOSTADPT_MSG(DBG_ERR, "irq num error, valid irq num : 0 - 5 \n");
		return ;
	}
	free_irq(get_npu_irq(irq_num), NULL);
	return ;
}
void hostdapt_enable_interrupt(unsigned int ringIdx)
{
	unsigned long int tmp;

	//printk("hostdapt_enable_interrupt %d\n", ringIdx);
	if(ringIdx == 0){
		tmp = get_npu_hostadpt_reg(HOSTADPT_TXRXDONE0_INT_MASK);
		tmp |= (0x00010000);
		set_npu_hostadpt_reg(HOSTADPT_TXRXDONE0_INT_MASK,tmp);
	}else{
		tmp = get_npu_hostadpt_reg(HOSTADPT_TXRXDONE1_INT_MASK);
		tmp |= (0x00020000);
		set_npu_hostadpt_reg(HOSTADPT_TXRXDONE1_INT_MASK,tmp);
	}
}
void hostdapt_disable_interrupt(unsigned int ringIdx)
{
	unsigned long int tmp;

	
	//printk("hostdapt_diable_interrupt %d\n", ringIdx);
	if(ringIdx == 0){
		tmp = get_npu_hostadpt_reg(HOSTADPT_TXRXDONE0_INT_MASK);
		tmp &= ~(0x00010000);
		set_npu_hostadpt_reg(HOSTADPT_TXRXDONE0_INT_MASK,tmp);
	}else{
		tmp = get_npu_hostadpt_reg(HOSTADPT_TXRXDONE1_INT_MASK);
		tmp &= ~(0x00020000);
		set_npu_hostadpt_reg(HOSTADPT_TXRXDONE1_INT_MASK,tmp);
	}
}

void hostdapt_registe_wifitask(unsigned int ringIdx, void *func)
{
	printk("hostdapt_registe_wifitask %d\n", ringIdx);
	if(ringIdx == 0){
		tasklet_schedule_WIFI2G = func;
	}else{
		tasklet_schedule_WIFI5G = func;
	}
}


/********* wifi tx part  *************/
#if defined(TCSUPPORT_NPU_WIFI_TX) || defined(TCSUPPORT_WLAN_INODE)
void enque_to_hostapdt_tx(struct device *dev, HOSTADPT_DMA_TX_DSCP_T *pHostadptTX,
	struct sk_buff *skb, unsigned char *vnd, unsigned short vnd_len)
{
	/*just only get vaild dev*/
	if (dev == NULL) {
		printk("\n%s() device failed\n", __func__);
		return ;
	}

	if(vnd_len > HOSTADPT_TX_VEND_SPEC_LEN){
		printk("[hostadpt][ERROR] vender len is too long %d!\n", vnd_len);
		vnd_len = HOSTADPT_TX_VEND_SPEC_LEN;
	}
	memcpy(pHostadptTX->vnd_spec, vnd, vnd_len);
	pHostadptTX->pkt_ctrl.vnd_len = vnd_len;

	pHostadptTX->pkt_ctrl.len = skb->len;
#if defined(TCSUPPORT_CPU_ARMV8_64)
	pHostadptTX->skb_p = (u64)skb;
#else
	pHostadptTX->skb_p = (u32)skb;
#endif

	if(skb->len != 0)
		pHostadptTX->pkt_addr = dma_map_single(dev, (void *)((unsigned long)skb->data), skb->len, DMA_TO_DEVICE);
	else
		pHostadptTX->pkt_addr = 0;

	pHostadptTX->pkt_ctrl.done = 1;
}

HOSTADPT_DMA_TX_DSCP_T *pHostadptTX_BASE[HOSTADPT_TX_RING_NUM] = {NULL};

#define SHOW_18BYTES " %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x "
#define PRINT_18BYTES(a) a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8],a[9],a[10],a[11],a[12],a[13],a[14],a[15],a[16],a[17]

static unsigned char hostadpt_is_ring_available(u32 dma_idx, u32 cpu_idx, u32 ring_size)
{
	unsigned short res;

	if (dma_idx >= cpu_idx)
		res = ring_size - dma_idx + cpu_idx;
	else
		res = cpu_idx - dma_idx;

	if(res > 5)
		return 1;
	else
		return 0;

}

int hostadpt_tx_handler(unsigned char band, struct sk_buff *skb, unsigned char *vnd, unsigned short vnd_len)
{
	u32 dmaIdx0 = 0, dmaIdx1 = 0;
	int ret = 0;
	u32 cpuIdx0 = 0, cpuIdx1 = 0;
	HOSTADPT_DMA_TX_DSCP_T *pHostadptTX0 = NULL;
	HOSTADPT_DMA_TX_DSCP_T *pHostadptTX1 = NULL;
	unsigned long int flags=0;
	HOSTADPT_MSG(DBG_LOG, "+++++++++++++enter hostadpt_send API++++++++++++\n");
	struct device *dev = NULL;

	if((skb == NULL) || (vnd == NULL)) {
		printk("[hostadpt][%d] invalid put !\n", __LINE__);
		return 0;
	}

	/*just only get vaild dev*/
	if ((dev = get_ecnt_npu_dev()) == NULL) {
		printk("\nget_ecnt_npu_dev failed\n");
		return 0;
	}
	if(band == 0)
	{
		//disable isr
		spin_lock_irqsave(&hostadpt_lock_2G, flags);
		HOSTADPT_MSG(DBG_LOG, "Send data to RX0 ... \n");
		cpuIdx0 = get_npu_hostadpt_reg(HOSTADPT_TX2_CPU_IDX);//this will update by isr in npu
		HOSTADPT_MSG(DBG_LOG, "HOSTADPT_TX2_CPU_IDX = %d\n", dmaIdx0);
		pHostadptTX0 = pHostadptTX_BASE[0] + hostadpt_tx_dma_idx[0];
		if(pHostadptTX0->pkt_ctrl.done == 0 && hostadpt_is_ring_available(hostadpt_tx_dma_idx[0],
			cpuIdx0, HOSTADPT_TX0_DSCP_NUM))
		{
			HOST_PKT_COUNTER(TX_GET_PKT_FROM_TXRING, 0);
			enque_to_hostapdt_tx(dev,pHostadptTX0, skb, vnd, vnd_len);
			hostadpt_tx_skbin_cnt[0]++;

			hostadpt_tx_dma_idx[0] = (hostadpt_tx_dma_idx[0] + 1);
			if(hostadpt_tx_dma_idx[0] >= HOSTADPT_TX0_DSCP_NUM)
				hostadpt_tx_dma_idx[0] = 0;
			set_npu_hostadpt_reg(HOSTADPT_TX2_DMA_IDX, hostadpt_tx_dma_idx[0]);
		}
		else
		{
			HOST_PKT_COUNTER(TX_Q_IS_FULL, 0);
			//printk("%s L%d dmaIdx0=%d,cpuIdx0=%d \n",__func__, __LINE__, hostadpt_tx_dma_idx[0], cpuIdx0);
			dev_kfree_skb_any(skb);
			ret = -1;
		}

		while(hostadpt_tx_cpu_idx[0] != cpuIdx0){
			pHostadptTX0 = pHostadptTX_BASE[0] + hostadpt_tx_cpu_idx[0];
			if(pHostadptTX0->skb_p){
				if(pHostadptTX0->pkt_addr) {
					dma_unmap_single(dev, pHostadptTX0->pkt_addr, pHostadptTX0->pkt_ctrl.len, DMA_TO_DEVICE);
				}
				dev_kfree_skb_any(pHostadptTX0->skb_p);
				hostadpt_tx_skbfree_cnt[0]++;
				pHostadptTX0->skb_p = NULL;
			}
			pHostadptTX0->pkt_ctrl.done = 0;
			hostadpt_tx_cpu_idx[0]++;
			if(hostadpt_tx_cpu_idx[0] >= HOSTADPT_TX0_DSCP_NUM)
				hostadpt_tx_cpu_idx[0] = 0;
			if(hostadpt_tx_cpu_idx[0] == cpuIdx0)
				break;
		}
		//enable isr
		spin_unlock_irqrestore(&hostadpt_lock_2G, flags);
	}
	else if(band == 1)
	{
		spin_lock_irqsave(&hostadpt_lock_test, flags);
		HOSTADPT_MSG(DBG_LOG, "Send data to RX0 ... \n");
		cpuIdx1 = get_npu_hostadpt_reg(HOSTADPT_TX3_CPU_IDX);
		HOSTADPT_MSG(DBG_LOG, "HOSTADPT_TX3_DMA_IDX = %d\n", dmaIdx1);
		pHostadptTX1 = pHostadptTX_BASE[1] + hostadpt_tx_dma_idx[1];
		if(pHostadptTX1->pkt_ctrl.done == 0 && hostadpt_is_ring_available(hostadpt_tx_dma_idx[1],
			cpuIdx1, HOSTADPT_TX1_DSCP_NUM))
		{
			HOST_PKT_COUNTER(TX_GET_PKT_FROM_TXRING, 1);
			enque_to_hostapdt_tx(dev,pHostadptTX1, skb, vnd, vnd_len);
			hostadpt_tx_skbin_cnt[1]++;

			hostadpt_tx_dma_idx[1] = (hostadpt_tx_dma_idx[1] + 1);
			if(hostadpt_tx_dma_idx[1] >= HOSTADPT_TX1_DSCP_NUM)
				hostadpt_tx_dma_idx[1] = 0;
			set_npu_hostadpt_reg(HOSTADPT_TX3_DMA_IDX, hostadpt_tx_dma_idx[1]);
		}
		else 
		{
			HOST_PKT_COUNTER(TX_Q_IS_FULL, 1);
			//printk("%s L%d dmaIdx1=%d,cpuIdx1=%d \n",__func__,__LINE__,hostadpt_tx_dma_idx[1],cpuIdx1);
			dev_kfree_skb_any(skb);
			ret = -1;
		}

		while(hostadpt_tx_cpu_idx[1] != cpuIdx1){
			
			pHostadptTX1 = pHostadptTX_BASE[1] + hostadpt_tx_cpu_idx[1];
			if(pHostadptTX1->skb_p){
				if(pHostadptTX1->pkt_addr) {
					dma_unmap_single(dev, pHostadptTX1->pkt_addr, pHostadptTX1->pkt_ctrl.len, DMA_TO_DEVICE);
				}
				dev_kfree_skb_any(pHostadptTX1->skb_p);
				hostadpt_tx_skbfree_cnt[1]++;
				pHostadptTX1->skb_p = NULL;
			}
			pHostadptTX1->pkt_ctrl.done = 0;

			hostadpt_tx_cpu_idx[1]++;
			if(hostadpt_tx_cpu_idx[1] >= HOSTADPT_TX1_DSCP_NUM)
				hostadpt_tx_cpu_idx[1] = 0;
			if(hostadpt_tx_cpu_idx[1] == cpuIdx1)
				break;
		}
		spin_unlock_irqrestore(&hostadpt_lock_test, flags);
	}
	else
	{
		HOSTADPT_MSG(DBG_LOG, "band Index error!\n");
		dev_kfree_skb_any(skb);
		ret = -1;
	}
	
	return ret;
}
#endif

#if defined(TCSUPPORT_WLAN_INODE) || defined(TCSUPPORT_NPU_WIFI_TX)
static int hostadpt_txdscp_init(void)
{
	dma_addr_t dscpDmaAddr=0;
	int i;
	u32 dmaIdx = 0;
	struct device *dev = NULL ;
	unsigned long dscpBaseAddr=0 ;
	HOSTADPT_DMA_TX_DSCP_T *dscp = NULL;

	struct sk_buff *skb = NULL;
	//printk("host adaptor init:L%d", __LINE__);
	/******************************************
	* Allocate descriptor DMA memory		  *
	*******************************************/
	if( (dev=(struct device *)get_ecnt_npu_dev()) == NULL ) {
		HOSTADPT_MSG(DBG_ERR, "Get device failed.\n");
		return -ENOMEM ;
	}

	//printk("test alloc size:%d*%d \n", sizeof(HOSTADPT_DMA_TX_DSCP_T), HOSTADPT_TX_TOTAL_DSCP_NUM);
	dscpBaseAddr = (unsigned long)dma_alloc_coherent(dev,
		sizeof(HOSTADPT_DMA_TX_DSCP_T)*HOSTADPT_TX_TOTAL_DSCP_NUM, &dscpDmaAddr, GFP_KERNEL);

	if(!dscpBaseAddr) {
		HOSTADPT_MSG(DBG_ERR, "Allocate memory for TX/RX DSCP failed.\n") ; 
		return -ENOMEM ;
	}

	pHostadptTX_BASE[0] = (HOSTADPT_DMA_TX_DSCP_T *)dscpBaseAddr;
	pHostadptTX_BASE[1] = pHostadptTX_BASE[0] + HOSTADPT_TX0_DSCP_NUM;

	HOSTADPT_MSG(DBG_LOG, "dscpDmaAddr = %x [0]%x [1]%x\n",dscpDmaAddr, pHostadptTX_BASE[0], pHostadptTX_BASE[1]) ; 
	printk("dscpDmaAddr =%x pHostadptTX_BASE[0]%x pHostadptTX_BASE[1]%x %d\n",
		dscpDmaAddr, pHostadptTX_BASE[0], pHostadptTX_BASE[1], sizeof(HOSTADPT_DMA_TX_DSCP_T));

	/***************************************************
	* Initialization first DSCP for Rx DMA			  *
	****************************************************/
	dscp = pHostadptTX_BASE[0];
	for(i = 0; i < HOSTADPT_TX_TOTAL_DSCP_NUM; i++) {
	//init at npu
		dscp->pkt_ctrl.done = 0;
		dscp++;	
	}
	//write rx0/rx1 num to reg
	set_npu_hostadpt_reg(HOSTADPT_TX2_MAX_CNT, HOSTADPT_TX0_DSCP_NUM);
	set_npu_hostadpt_reg(HOSTADPT_TX3_MAX_CNT, HOSTADPT_TX1_DSCP_NUM);
	
	//set rx0/rx1 hostadpt_rx_cpu_idx/hostadpt_rx_dma_idx 
	set_npu_hostadpt_reg(HOSTADPT_TX2_CPU_IDX, 0);
	set_npu_hostadpt_reg(HOSTADPT_TX3_CPU_IDX, 0);
	
	set_npu_hostadpt_reg(HOSTADPT_TX2_DMA_IDX, 0);
	set_npu_hostadpt_reg(HOSTADPT_TX3_DMA_IDX, 0);

	//write rx0/rx1 base to reg
	set_npu_hostadpt_reg(HOSTADPT_TX2_BASE_PTR, dscpDmaAddr);
	set_npu_hostadpt_reg(HOSTADPT_TX3_BASE_PTR, dscpDmaAddr + sizeof(HOSTADPT_DMA_TX_DSCP_T)*HOSTADPT_TX0_DSCP_NUM);
	
	return 0;
}

static int hostadpt_debug_read_proc(char *page, char **start, off_t off, 
	int count, int *eof, void *data)
{
	int len=0;
	len += sprintf(page+len, "band 0 tx skb cnt=%d, free cnt=%d\r\n", hostadpt_tx_skbin_cnt[0],hostadpt_tx_skbfree_cnt[0]);
	len += sprintf(page+len, "band 1 tx skb cnt=%d, free cnt=%d\r\n",hostadpt_tx_skbin_cnt[1],hostadpt_tx_skbfree_cnt[1]);
	len += sprintf(page+len, "band 0 tx dmaIdx=%d cpuidx=%d,%d\r\n",hostadpt_tx_dma_idx[0],hostadpt_tx_cpu_idx[0],get_npu_hostadpt_reg(HOSTADPT_TX2_CPU_IDX));
	len += sprintf(page+len, "band 1 tx dmaIdx=%d cpuidx=%d,%d\r\n",hostadpt_tx_dma_idx[1],hostadpt_tx_cpu_idx[1],get_npu_hostadpt_reg(HOSTADPT_TX3_CPU_IDX));

	len += sprintf(page+len, "band 0 rx dmaIdx=%d cpuidx=%d,%d\r\n",
		hostadpt_rx_dma_idx[0],hostadpt_rx_cpu_idx[0],get_npu_hostadpt_reg(HOSTADPT_RX0_CPU_IDX));
	len += sprintf(page+len, "band 1 rx dmaIdx=%d cpuidx=%d,%d\r\n",
		hostadpt_rx_dma_idx[1],hostadpt_rx_cpu_idx[1],get_npu_hostadpt_reg(HOSTADPT_RX1_CPU_IDX));
	return len;
}

static int hostadpt_debug_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	int i=0 ;
	char get_buf[120];
	int band=0;
	int flag=0;

	HOSTADPT_DMA_TX_DSCP_T *pHostadptTX = NULL;
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';
	if(sscanf(get_buf, "%d %d ", &band, &flag)==2){
			if(band==0 || band==1){
			host_apd_debug[band]=flag;
		}
	}
	else if(sscanf(get_buf, "%d ", &band)==1){// dump done bit=1 's txD index
		if(band < 0 || band > 1){
			printk("error: band must be 0/1\n");
			return count;
			}
			printk("hostadpt_tx_dma_idx[0]=%d [1]=%d\n", hostadpt_tx_dma_idx[0], hostadpt_tx_dma_idx[1]);
		for(i=0;i<1024;i++){
			pHostadptTX = pHostadptTX_BASE[band] + i;
			if(pHostadptTX->pkt_ctrl.done==1){
				printk("%d: done=1 \n",i);
			}
		}
	}
	return count;

}

static void hostadpt_dump_debug_create(void) 
{
	struct proc_dir_entry *npu_proc = NULL;

	npu_proc = create_proc_entry("hostadpt_debug", 0, NULL);
	npu_proc->read_proc = hostadpt_debug_read_proc;
	npu_proc->write_proc = hostadpt_debug_write_proc;

	return;
}
#endif

static int hostadpInit(void)
{
	HOSTADPT_MSG(DBG_LOG, "host adaptor init :\n");
	//unsigned long flags;

	if(hostadpt_rxdscp_init())
	{
		HOSTADPT_MSG(DBG_ERR, "hostadpt_rxdscp_init failed!\n");
		return -ENODEV;
	}
	
	hostadpt_regiter_interrupt(0, rxdone0_isr, "rxdone0");
	set_npu_hostadpt_reg(HOSTADPT_TXRXDONE0_INT_MASK,0x00010000);

	hostadpt_regiter_interrupt(1, rxdone1_isr, "rxdone1");
	set_npu_hostadpt_reg(HOSTADPT_TXRXDONE1_INT_MASK,0x00020000);


	rcu_assign_pointer(fromHostadptPktHandle_hook, hostadpt_rx_handler);
	rcu_assign_pointer(hostdapt_enable_int_hook, hostdapt_enable_interrupt);
	rcu_assign_pointer(hostdapt_disable_int_hook, hostdapt_disable_interrupt);
	rcu_assign_pointer(hostdapt_registe_wifitask_hook, hostdapt_registe_wifitask);
	
#if defined(TCSUPPORT_NPU_WIFI_TX) || defined(TCSUPPORT_WLAN_INODE)
	hostadpt_dump_debug_create();

	if(hostadpt_txdscp_init())
	{
		printk("hostadpt_txdscp_init failed!\n");
		return -ENODEV;
	}

	rcu_assign_pointer(toHostadptPktHandle_hook, hostadpt_tx_handler);
#endif

	return 0;
}

static void __exit hostadpExit(void)
{
	hostadpt_free_interrupt(0);
	hostadpt_free_interrupt(1);
	
#if defined(TCSUPPORT_NPU_WIFI_TX) || defined(TCSUPPORT_WLAN_INODE)
	remove_proc_entry("hostadpt_debug",  NULL);
	rcu_assign_pointer(toHostadptPktHandle_hook, NULL);
#endif

	rcu_assign_pointer(fromHostadptPktHandle_hook, NULL);
	rcu_assign_pointer(hostdapt_enable_int_hook, NULL);
	rcu_assign_pointer(hostdapt_disable_int_hook, NULL);
	rcu_assign_pointer(hostdapt_registe_wifitask_hook, NULL);
	HOSTADPT_MSG(DBG_LOG, "host adaptor exit .\n");
}

module_init(hostadpInit);
module_exit(hostadpExit);

MODULE_DESCRIPTION("EcoNet Host Adaptor Driver");
