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
#include "tso.h"
#if defined(TCSUPPORT_OPENWRT)
#include "../qdma/qdma_wan/qdma_bmgr.h"
#include "../qdma/qdma_wan/qdma_dev.h"
#elif defined(RDKB_BUILD)
#include "../qdma_bmgr.h"
#include "../qdma_dev.h"
#else
#include "../qdma/EN7516/qdma_bmgr.h"
#include "../qdma/EN7516/qdma_dev.h"
#endif

//#include "../ra_hwnat_v3/ra_nat_offload.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
unsigned long tso_expires = 5000;  /* 5 s */
unsigned long tso_refresh_time = 0;	 /* 400jiffies */
int tso_refresh_time_pending = 0;


/***************************************
    proc name
***************************************/
	
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



/************************************************************************
*                  M A C R O S
*************************************************************************
*/	

#define TSO_MAX_MSS_COUNT 46
#define TSO_RECORD_DEBUG_ON 1

#define IP_VER_V4	4
#define IP_VER_V6	6

#define DP_TSO_LOCALOUT 326 /* copy from /module/private/ra_hwnat_v3/ra_nat_offload.h to avoid to use an unopen head file, should keep coherence. */

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

typedef struct{
	struct list_head list;
	tsoRuleEntry_v4_t ruleEntry;
	struct sock *sk;
	struct flowi *fl;
}tsoSessionListEntry_v4_t;


/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int tsoDbgRecord;
extern int skb_count;
extern int tsoShortcutPpe;

extern uint tsoMssCount[TSO_MAX_MSS_COUNT];
extern QDMA_Private_T *gpQdmaPriv;
extern int tso_match_v4(struct ECNT_TSO_Data *tso_data);
extern int tso_match_v6(struct ECNT_TSO_Data *tso_data);
extern void* tsoLocaloutListAlloc(int size);
extern void tsoLocaloutListFree(void* ptr);


extern struct list_head tsoSessionList_v4;

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void tso_qos_weight_timer(unsigned long data);
static int tso_send_pingpong_v4(tsoRuleEntry_v4_t* skbEntry, struct sk_buff *skb);
static int tso_localout_offload(struct ECNT_TSO_Data *tso_data);
static int tso_pingpong_v4(struct ECNT_TSO_Data *tso_data);
inline int tso_session_destroy_v4(struct sock *sk);
static int tso_destroy_v4(struct ECNT_TSO_Data *tso_data);



int tso_transmit_packet(struct ECNT_TSO_Data *tso_data);
int tso_tdma_dma_config(struct ECNT_TSO_Data *tso_data);
int tso_tdma_dma_enable(struct ECNT_TSO_Data *tso_data);


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
extern atomic_t free_txd_count[];

extern atomic_t chnlLimit[CONFIG_QDMA_CHANNEL];
extern atomic_t queueLimit[CONFIG_QDMA_CHANNEL][CONFIG_QDMA_QUEUE];

#if TCSUPPORT_OPENWRT
/* only for OpenWRT , because compile error in OpenWRT new toolchain for EN7527. */
//extern QDMA_Private_T *gpQdmaPriv ;
static inline struct QDMA_DscpInfo_S *qdma_bm_pop_tx_dscp_for_tso(int ringIdx)
{
	struct QDMA_DscpInfo_S *diPtr=NULL ;
	
	diPtr = gpQdmaPriv->txHeadPtr[ringIdx] ;
	if(gpQdmaPriv->txHeadPtr[ringIdx] == gpQdmaPriv->txTailPtr[ringIdx]) {
		gpQdmaPriv->txHeadPtr[ringIdx] = NULL ;
		gpQdmaPriv->txTailPtr[ringIdx] = NULL ;
	} else {
		gpQdmaPriv->txHeadPtr[ringIdx] = gpQdmaPriv->txHeadPtr[ringIdx]->next ;
	}

	if(diPtr) {
		diPtr->next = NULL ;
		
#if defined(TCSUPPORT_TSO_ENABLE)
		atomic_dec(&free_txd_count[ringIdx]);
#endif
	}
	
	return diPtr ;
}
#else
inline struct QDMA_DscpInfo_S *qdma_bm_pop_tx_dscp(int ringIdx);
#endif

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

#if defined(TCSUPPORT_TSO_ENABLE)
struct timer_list tso_qos_timer ;
#endif


/* used for tso white list. */

/* ------------------ ECNT_Hook ------------------ */
typedef int (*tso_api_op_t)(struct ECNT_TSO_Data *tso_data);

/* Warning: same sequence with enum 'ETHER_HookFunction_t' */
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


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/




void tso_qos_weight_timer(unsigned long data)
{
	uint base = gpQdmaPriv->csrBaseAddr ;

	if (jiffies < tso_refresh_time)/*jiffies reverse*/
	{
		tso_refresh_time = jiffies;
	}
	if((jiffies - tso_refresh_time) < 400 )
	{
		QDMA_START_TIMER(tso_qos_timer);
		tso_refresh_time_pending = 1;
		return;
	}

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
	qdmaSetTxQosWeightScale1(base) ;
#endif
	
	tso_refresh_time_pending = 0;

}


/*___________________________________________________________________________
**      function name: match_ipv4_record
**      descriptions:
**      	it's used to handle small packets pingpong back from PPE.
** 
**      input parameters:
**      	skb: skb data should point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	0	always 0 to make eth_rx don't go through.
**___________________________________________________________________________
*/

int tso_send_pingpong_v4(tsoRuleEntry_v4_t* skbEntry, struct sk_buff *skb)
{
	tsoSessionListEntry_v4_t *in_entry_v4 = NULL;
	if(tsoDbgRecord == 2)
		printk("[%s][%d]\n", __FUNCTION__, __LINE__);
	//dump_skb(skb);
	if(skbEntry == NULL || skb == NULL)
	{
		return 0;
	}
	list_for_each_entry(in_entry_v4, &tsoSessionList_v4, list)	 
	{
		
		if(tsoDbgRecord == 2)
			printk("[%s][%d]sport:%x %x %x %x, %x %x %x %x\n", __FUNCTION__, __LINE__, skbEntry->sport, skbEntry->dport, skbEntry->saddr_v4, skbEntry->daddr_v4,
				in_entry_v4->ruleEntry.sport, in_entry_v4->ruleEntry.dport, in_entry_v4->ruleEntry.saddr_v4, in_entry_v4->ruleEntry.daddr_v4);
		if(!memcmp(skbEntry, &(in_entry_v4->ruleEntry), sizeof(tsoRuleEntry_v4_t))){
			skb_pull(skb, sizeof(struct iphdr));
			skb_reset_transport_header(skb);
			skb->tso_mark |= TSO_ENABLE_MARK;
			ip_queue_xmit(in_entry_v4->sk, skb, in_entry_v4->fl);
			if(tsoDbgRecord == 2)
				printk("[%s][%d]\n", __FUNCTION__, __LINE__);
				return 1;
		}
	}
	
	//dev_kfree_skb_any(skb);

	return 0;
}

/*___________________________________________________________________________
**      function name: tso_session_record
**      descriptions:
**      	it's used to record the neccesory session info, which is used to forward the packets to ip tx.
** 
**      input parameters:
**      	skb: skb data should point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	IPv4 or IPv6
**___________________________________________________________________________
*/
static inline void fill_header_v4(struct sk_buff *skb)
{
	struct iphdr *iph = NULL;
	struct sock *sk = skb->sk;
	const struct flowi4 *fl4 = NULL;	
	struct inet_sock *inet = NULL;

	
	inet = inet_sk(sk);
	fl4 = &inet->cork.fl.u.ip4;	
	/* fill ip header. */
	skb->protocol = htons(ETH_P_IP);
	/*add layer3 header*/	
	skb_push(skb, sizeof(struct iphdr));
	skb_reset_network_header(skb);
	iph = ip_hdr(skb);
	iph->version = 4;
	iph->ihl = 5;
	iph->tos = 0;
	iph->tot_len = htons(skb->len);
	if (sk && inet_sk(sk)->inet_daddr) {
		iph->id = htons(inet_sk(sk)->inet_id);
//		inet_sk(sk)->inet_id += skb_shinfo(skb)->gso_segs ?: 1;
	} else {
		iph->id = 0;
	}
	iph->frag_off = 0;
	iph->ttl = 254;
	iph->protocol = IPPROTO_TCP;
	memcpy(&iph->saddr, &fl4->saddr, sizeof(fl4->saddr));	
	memcpy(&iph->daddr, &fl4->daddr, sizeof(fl4->daddr));

}

/*___________________________________________________________________________
**      function name: tso_session_record
**      descriptions:
**      	it's used to record the neccesory session info, which is used to forward the packets to ip tx.
** 
**      input parameters:
**      	skb: skb data should point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	IPv4 or IPv6
**___________________________________________________________________________
*/
inline int tso_session_record_v4(struct sk_buff *skb)
{
	struct sock *cur_sk = skb->sk;	
	tsoSessionListEntry_v4_t *in_entry_v4 = NULL;
	tsoSessionListEntry_v4_t *new_entry_v4 = NULL;	
	struct inet_sock *inet = NULL;

	list_for_each_entry(in_entry_v4, &tsoSessionList_v4, list)	 
	{
		if(in_entry_v4->sk == cur_sk){
			return 0;
		}
	}

	new_entry_v4 = (tsoSessionListEntry_v4_t*)tsoLocaloutListAlloc(sizeof(tsoSessionListEntry_v4_t));
	if(new_entry_v4)
	{
		inet = inet_sk(cur_sk);
		if(inet != NULL)
		{
			new_entry_v4->ruleEntry.dport = inet->inet_dport;
			new_entry_v4->ruleEntry.sport = inet->inet_sport;
			new_entry_v4->ruleEntry.daddr_v4 = inet->inet_daddr;
			new_entry_v4->ruleEntry.saddr_v4 = inet->inet_saddr;
			new_entry_v4->sk = cur_sk;
			new_entry_v4->fl = &inet->cork.fl;
			list_add_tail(&new_entry_v4->list, &tsoSessionList_v4);
			return 0;
		}
		else{
			kfree(new_entry_v4);
			new_entry_v4 = NULL;
		}
	}
	
	return -1;

}


/*___________________________________________________________________________
**      function name: tso_session_record
**      descriptions:
**      	it's used to record the neccesory session info, which is used to forward the packets to ip tx.
** 
**      input parameters:
**      	skb: skb data should point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	IPv4 or IPv6
**___________________________________________________________________________
*/
inline int tso_session_destroy_v4(struct sock *sk)
{	
	tsoSessionListEntry_v4_t *in_entry_v4 = NULL;

	list_for_each_entry(in_entry_v4, &tsoSessionList_v4, list)	 
	{
		if(in_entry_v4->sk == sk){
			list_del(&in_entry_v4->list);
			tsoLocaloutListFree(in_entry_v4);
			return 0;
		}
	}
	
	return 0;
}

/*___________________________________________________________________________
**      function name: ppe_get_ipver
**      descriptions:
**      	it's used to get the ip version
** 
**      input parameters:
**      	skb: skb data should point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	IPv4 or IPv6
**___________________________________________________________________________
*/
inline int ppe_get_ipver(struct sk_buff *skb)
{
	uint8 ip_ver = 0;
	unsigned char *cp = NULL;
	
	cp = skb->data;
	ip_ver = ((*cp) >> 4);
	
	return ip_ver;
}

/*___________________________________________________________________________
**      function name: prepare_skb
**      descriptions:
**      	insert a vlan tag of DP_TSO_LOCALOUT; and prepare the skb data.
** 
**      input parameters:
**      	skb:	skb data point to ip header
**
**      output parameters:
**      	N/A
**
**      return:
**      	success:	1
**		failure:	0
**___________________________________________________________________________
*/
int prepare_skb(struct sk_buff * skb)
{

    int i;
	u16 VirIfIdx = 0;
	uint8 ip_ver = 0;
	uint8 tmp_src_mac_down[] = {0x00,0x00,0x00,0xdd,0xee,0xff};
	uint8 tmp_src_mac_up[] = {0x00,0x00,0x00,0xff,0xee,0xdd};
	struct vlan_ethhdr *veth = NULL;
	int paddingLength, skbLenTmp;
	struct sk_buff * skb2;
	
	VirIfIdx = DP_TSO_LOCALOUT;
	if (!skb) {
		return 0;
	}
	
	fill_header_v4(skb);
	ip_ver = ppe_get_ipver(skb);
	//printk("ip_ver:%d", ip_ver);

	/* 1. add virtual vlan */
	//skb = skb_unshare(skb, GFP_ATOMIC);
	
	/* dmac+smac+vlan_proto+vlan+proto */
	veth = (struct vlan_ethhdr *)skb_push(skb, ETH_HLEN+VLAN_HLEN);

	/* Move the mac addresses to the beginning of the new header. */
	skb_reset_mac_header(skb);

	/* first, the ethernet type */
	veth->h_vlan_proto = htons(ETH_P_8021Q);

	/* now, the TCI */
	veth->h_vlan_TCI = htons(VirIfIdx);

	skb->protocol = htons(ETH_P_8021Q);

	/* 2. fill in layer2 information */
	memcpy(skb->data, tmp_src_mac_up, ETH_ALEN);
	memcpy(skb->data + ETH_ALEN, tmp_src_mac_down, ETH_ALEN);

	if (IP_VER_V4 == ip_ver){
		veth->h_vlan_encapsulated_proto = htons(ETH_P_IP);
	}else if (IP_VER_V6 == ip_ver){
		veth->h_vlan_encapsulated_proto = htons(ETH_P_IPV6);
	}

	#if 0
	if (skb->len < 64)
	{
		paddingLength = 64 - skb->len;
		skbLenTmp = skb->len;
	
		if(skb_tailroom(skb) < paddingLength){
			skb2 = skb_copy_expand(skb, skb_headroom(skb), paddingLength, GFP_ATOMIC);
			if (skb2) {
				kfree_skb(skb);
				skb = skb2;
				skb_put(skb, paddingLength);
			}
			else {
				printk("\nskb_copy_expand failed at SendToPpe\n");
				return 0;
			}
		}
		else {
			skb_put(skb, paddingLength);
		}
		memset(skb->data + skbLenTmp, 0, paddingLength);
	}
	#endif
//	dump_skb(skb);
	return 0;
}

/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_send_to_ppe_tso(struct sk_buff *skb)
{
	
#if defined(TCSUPPORT_CPU_EN7580)
	int ringIdx = 7;
#else
	int ringIdx = 0;
#endif
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags = 0 ;
	int ret = 0 ;
	uint channel = 0, queue = 0, i = 0, offset = 0, size = 0;
	int mss_count;
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	struct bio_vec *frag;
#else
	struct skb_frag_struct *frag;
#endif

	unsigned int nr_frags = skb_shinfo(skb)->nr_frags;
	unsigned short *checksum;
	uint msg0 = 0, msg1 = 0;	
	txMsgWord_t txMsg;

	memset(&txMsg, 0, sizeof(txMsgWord_t));
	txMsg.pTxMsgW0 = (txMsgWord0_t *)&msg0;
	txMsg.pTxMsgW1 = (txMsgWord1_t *)&msg1;


//	if(tsoDbgRecord == 2)
//		printk("[%s][%d]\n", __FUNCTION__, __LINE__);

	if(unlikely(!skb || skb->len<=0 || skb->len>65535))
	{
		QDMA_ERR("qdma_bm_transmit_packet_tso The input arguments are wrong, skb:%.8x, skbLen:%d.\n", (uint)skb, skb->len) ; 
		return -EFAULT ;
	}
	
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
	tso_session_record_v4(skb);
	prepare_skb(skb);

	qdma_set_txmsg_ico(&txMsg,1);
	qdma_set_txmsg_tco(&txMsg,1);

	txMsg.pTxMsgW1->fport = 4;
	txMsg.pTxMsgW0->channel = 7;
	txMsg.pTxMsgW0->queue = 0;

	QDMA_MSG(DBG_ST,"Tx msg0:%x, msg1:%x, nr_frags:%d, gso_size:%d\n", msg0, msg1, nr_frags, skb_shinfo(skb)->gso_size);

	if(atomic_read(&free_txd_count[ringIdx]) < nr_frags+1){
		QDMA_MSG(DBG_MSG,"There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", ringIdx, atomic_read(&free_txd_count[ringIdx]), nr_frags);
		return -EFAULT ;
	}
	
#if !defined(TCSUPPORT_TSO_VERIFY)

	if(unlikely(!skb->transport_header))
	{
		QDMA_ERR("skb transport_header is invalid.\n") ; 
		return -EFAULT ;
	}

	/* fill MSS in checksum field for HW TSO, and enable HW TSO.*/
	if(skb_shinfo(skb)->gso_size){
		checksum = (unsigned short*)(skb_transport_header(skb)+16);
		*checksum = htons(skb_shinfo(skb)->gso_size);
#if defined(TCSUPPORT_CPU_EN7580)
		txMsg.pTxMsgW0->tso = 1;
#else /*EN7516*/
		txMsg.pTxMsgW1->tso = 1;
#endif
	}
	else{
		checksum = (unsigned short*)(skb_transport_header(skb)+16);
		*checksum = 0;
	}
#endif
#if 0 //1.tso has no use in qos 2.affect performace.
	/*set wrr weight 16Byte*/
	if(isEN7528||isEN7580)
	{
		if(tso_refresh_time == 0)
		{
			tso_qos_timer.expires = jiffies+(tso_expires*HZ)/1000;
			tso_qos_timer.data = tso_expires;
			tso_qos_timer.function = tso_qos_weight_timer;
			add_timer(&tso_qos_timer);
			qdmaSetTxQosWeightScale16(base);
		}
		tso_refresh_time = jiffies;
	}
#endif
	QDMA_MSG(DBG_ST,"Tx22 msg0:%x, msg1:%x, nr_frags:%d, gso_size:%d\n", msg0, msg1, nr_frags, skb_shinfo(skb)->gso_size);


	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags); 
	for(i = 0; i <= nr_frags; i++){
		/* Get unused TX DSCP from TX unused DSCP link list */	
		pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(ringIdx) ;
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
			QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet_tso pNewDscpInfo is NULL\n") ; 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
			return -ENOSR ;
		}

	
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
		pTxDscp->msg[0] = msg0;
		pTxDscp->msg[1] = msg1;
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;

		/* first data block in the head block, and the subsequent from each fragment. */
		if(i==0){
#if defined(TCSUPPORT_CPU_ARMV8)
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
			ecnt_dcache_wback_inv(((unsigned long)(skb->data)), (skb->len-skb->data_len));
#else
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
			dma_cache_wback_inv((unsigned long)(skb->data), (skb->len-skb->data_len));
#endif

			pTxDscp->ctrl.pkt_len = (skb->len-skb->data_len);  
		}
		else{
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
		
#if defined(TCSUPPORT_TSO_VERIFY) && defined(TCSUPPORT_CPU_EN7528)
		pTxDscp->ctrl.no_drop = 0 ;
#endif

		/* not last segment flag, last block set to 0, and others set to 1. */
		if(i == nr_frags){
			pTxDscp->ctrl.nls = 0;
			gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
		}
		else{
			pTxDscp->ctrl.nls = 1;
			gpQdmaPriv->txUsingPtr[ringIdx]->skb = NULL ;
		}
		
		//dma_cache_wback_inv(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
		
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#endif

		
#if defined(TCSUPPORT_TSO_VERIFY)	
		/* msg1 for tso verify test11. */
		tso_verify_case11_callback(msg0, msg1, ringIdx);
#endif
		gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;
		
#if defined(TCSUPPORT_TSO_VERIFY)
		/* msg1 for tso verify test14, this function must be called after "gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo". */
		tso_verify_case14_callback(msg0, msg1, ringIdx, pNewDscpInfo->dscpIdx);

#endif
			
		gpQdmaPriv->counters.txCounts[ringIdx]++ ;

	}
	

	wmb() ;
	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;
	
	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

//	if(tsoDbgRecord == 2)
//		printk("[%s][%d]\n", __FUNCTION__, __LINE__);

	QDMA_MSG(DBG_MSG, "qdma_bm_transmit_packet_tso RingIdx:%d, GLG:%.8x, IRQStatus:%.8x, CSR info: RX_CPU_IDX:%u, RX_DMA_IDX:%u, TX_CPU_IDX:%u, TX_DMA_IDX:%u\n", 
																ringIdx, 
																qdmaGetGlbCfg(base), 
																qdmaGetIrqStatus(base), 
																qdmaGetRxCpuIdx(base, ringIdx), 
																qdmaGetRxDmaIdx(base, ringIdx), 
																qdmaGetTxCpuIdx(base, ringIdx), 
																qdmaGetTxDmaIdx(base, ringIdx)) ;
	return ret ;

}

/******************************************************************************
******************************************************************************/
__IMEM int qdma_bm_transmit_packet_tso(struct sk_buff *skb, int ringIdx, uint msg0, uint msg1)
{
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	txMsgWord_t txMsg;
	uint channel = 0, queue = 0, i = 0, offset = 0, size = 0;
	unsigned long flags = 0;
	int ret = 0 ;
	int mss_count;

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	struct bio_vec *frag;
#else
	struct skb_frag_struct *frag;
#endif
	unsigned int nr_frags = skb_shinfo(skb)->nr_frags;
	unsigned short *checksum;

	memset(&txMsg,0,sizeof(txMsgWord_t));

	txMsg.pTxMsgW0 = (txMsgWord0_t *)&msg0;
	txMsg.pTxMsgW1 = (txMsgWord1_t *)&msg1;
	

	if(unlikely(!skb || skb->len<=0 || skb->len>65535))
	{
		QDMA_ERR("qdma_bm_transmit_packet_tso The input arguments are wrong, skb:%.8x, skbLen:%d.\n", (uint)skb, skb->len) ; 
		return -EFAULT ;
	}
	
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
	
	if(skb->tso_mark & TSO_ENABLE_MARK){
        qdma_set_txmsg_ico(&txMsg,1);
        qdma_set_txmsg_tco(&txMsg,1);
	}
	
	QDMA_MSG(DBG_ST,"Tx msg0:%x, msg1:%x, nr_frags:%d, gso_size:%d\n", msg0, msg1, nr_frags, skb_shinfo(skb)->gso_size);
	if((skb_shinfo(skb)->gso_size == 0) && (nr_frags == 0))
	{
		ret = qdma_bm_transmit_packet(skb, ringIdx, &txMsg);
		if(ret){
			QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet error\n");
		}
		return ret;
	}

	if(atomic_read(&free_txd_count[ringIdx]) < nr_frags+1){
		QDMA_MSG(DBG_MSG,"There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", ringIdx, atomic_read(&free_txd_count[ringIdx]), nr_frags);
		return -EFAULT ;
	}
	
#if !defined(TCSUPPORT_TSO_VERIFY)

	if(unlikely(!skb->transport_header))
	{
		QDMA_ERR("skb transport_header is invalid.\n") ; 
		return -EFAULT ;
	}

	/* fill MSS in checksum field for HW TSO, and enable HW TSO.*/
	if(skb_shinfo(skb)->gso_size){
		checksum = (unsigned short*)(skb_transport_header(skb)+16);
		*checksum = htons(skb_shinfo(skb)->gso_size);

        qdma_set_txmsg_ico(&txMsg,1);
        qdma_set_txmsg_tco(&txMsg,1);
        qdma_set_txmsg_tso(&txMsg,1);
	}
	else{
		checksum = (unsigned short*)(skb_transport_header(skb)+16);
		*checksum = 0;
        qdma_set_txmsg_ico(&txMsg,1);
        qdma_set_txmsg_tco(&txMsg,1);
	}
#endif
#if 0 //1.tso has no use in qos 2.affect performace.
	/*set wrr weight 16Byte*/
	if(isEN7528||isEN7580)
	{
		if(tso_refresh_time_pending == 0)
		{
			tso_qos_timer.expires = jiffies+(tso_expires*HZ)/1000;
			tso_qos_timer.data = tso_expires;
			tso_qos_timer.function = tso_qos_weight_timer;
			QDMA_START_TIMER(tso_qos_timer);
			tso_refresh_time_pending = 1;
			qdmaSetTxQosWeightScale16(base);
		}
		tso_refresh_time = jiffies;
	}
#endif
	QDMA_MSG(DBG_ST,"Tx22 msg0:%x, msg1:%x, nr_frags:%d, gso_size:%d\n", msg0, msg1, nr_frags, skb_shinfo(skb)->gso_size);

	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);	
	for(i = 0; i <= nr_frags; i++){
		/* Get unused TX DSCP from TX unused DSCP link list */	
#if TCSUPPORT_OPENWRT
		pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp_for_tso(ringIdx) ;
#else
		pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(ringIdx) ;
#endif
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
			QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet_tso pNewDscpInfo is NULL\n") ; 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
			return -ENOSR ;
		}
		
		channel = txMsg.pTxMsgW0->channel ;
		queue = txMsg.pTxMsgW0->queue ;


#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#else
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
#endif

		pTxDscp->msg[0] = msg0;
		pTxDscp->msg[1] = msg1;
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;

		/* first data block in the head block, and the subsequent from each fragment. */
		if(i==0){
#if defined(TCSUPPORT_CPU_ARMV8)
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
			ecnt_dcache_wback_inv(((unsigned long)(skb->data)), (skb->len-skb->data_len));
#else
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
			dma_cache_wback_inv((unsigned long)(skb->data), (skb->len-skb->data_len));
#endif

			pTxDscp->ctrl.pkt_len = (skb->len-skb->data_len);  
		}
		else{
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
		
#if defined(TCSUPPORT_TSO_VERIFY) && defined(TCSUPPORT_CPU_EN7528)
		pTxDscp->ctrl.no_drop = 0 ;
#endif

		/* not last segment flag, last block set to 0, and others set to 1. */
		if(i == nr_frags){
			pTxDscp->ctrl.nls = 0;
			gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
			/* chnlLimit &  queueLimit is for skb count. */
			atomic_inc(&chnlLimit[channel]);
			atomic_inc(&queueLimit[channel][queue]);
		}
		else{
			pTxDscp->ctrl.nls = 1;
			gpQdmaPriv->txUsingPtr[ringIdx]->skb = NULL ;
		}
		
		//dma_cache_wback_inv(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		
#if defined(TCSUPPORT_TSO_VERIFY)	
		/* msg1 for tso verify test11. */
		tso_verify_case11_callback(msg0, msg1, ringIdx);
#endif
		gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;
		
#if defined(TCSUPPORT_TSO_VERIFY)
		/* msg1 for tso verify test14, this function must be called after "gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo". */
		tso_verify_case14_callback(msg0, msg1, ringIdx, pNewDscpInfo->dscpIdx);

#endif
			
		gpQdmaPriv->counters.txCounts[ringIdx]++ ;

	}
	

#if !defined(TCSUPPORT_CPU_ARMV8)
	SYNC_TYPE4();
#endif
	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;
	
	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

	QDMA_MSG(DBG_MSG, "qdma_bm_transmit_packet_tso RingIdx:%d, GLG:%.8x, IRQStatus:%.8x, CSR info: RX_CPU_IDX:%u, RX_DMA_IDX:%u, TX_CPU_IDX:%u, TX_DMA_IDX:%u\n", 
																ringIdx, 
																qdmaGetGlbCfg(base), 
																qdmaGetIrqStatus(base), 
																qdmaGetRxCpuIdx(base, ringIdx), 
																qdmaGetRxDmaIdx(base, ringIdx), 
																qdmaGetTxCpuIdx(base, ringIdx), 
																qdmaGetTxDmaIdx(base, ringIdx)) ;
	return ret ;

}
__IMEM int qdma_bm_transmit_packet_tso_offload_fast_hook(struct sk_buff *skb, int channel)
{
#if defined(TCSUPPORT_CPU_EN7580)
	int ringIdx = 7;
#else
	int ringIdx = 0;
#endif
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	txMsgWord_t txMsg;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags = 0 ;
	int ret = 0 , i = 0, ico = 0, tco = 0, tso = 0;
    uint offset = 0, size = 0;
	int mss_count;
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	struct bio_vec *frag;
#else
	struct skb_frag_struct *frag;
#endif

	unsigned int nr_frags = skb_shinfo(skb)->nr_frags;
	unsigned short *checksum;

	memset(&txMsg, 0, sizeof(txMsgWord_t));

    if(unlikely(!skb || skb->len>65535))
    {
        QDMA_ERR("qdma_bm_transmit_packet_tso The input arguments are wrong, skb:%.8x, skbLen:%d.\n", (uint)skb, skb->len) ;
        return -EFAULT ;
    }

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

    QDMA_MSG(DBG_ST,"nr_frags:%d, gso_size:%d\n", nr_frags, skb_shinfo(skb)->gso_size);

    if(atomic_read(&free_txd_count[ringIdx]) < nr_frags+1){
        QDMA_MSG(DBG_MSG,"There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", ringIdx, atomic_read(&free_txd_count[ringIdx]), nr_frags);
        return -EFAULT ;
    }

    if(unlikely(!skb->transport_header))
    {
        QDMA_ERR("skb transport_header is invalid.\n") ;
        return -EFAULT ;
    }

    ico = 1;
    tco = 1;
    /* fill MSS in checksum field for HW TSO, and enable HW TSO.*/
    if(skb_shinfo(skb)->gso_size){
        checksum = (unsigned short*)(skb_transport_header(skb)+16);
        *checksum = htons(skb_shinfo(skb)->gso_size);
        tso = 1;
    }
    else{
        checksum = (unsigned short*)(skb_transport_header(skb)+16);
        *checksum = 0;
    }
#if 0 //1.tso has no use in qos 2.affect performace.
    /*set wrr weight 16Byte*/
    if(isEN7528||isEN7580)
    {
        if(tso_refresh_time == 0)
        {
            tso_qos_timer.expires = jiffies+(tso_expires*HZ)/1000;
            tso_qos_timer.data = tso_expires;
            tso_qos_timer.function = tso_qos_weight_timer;
            add_timer(&tso_qos_timer);
            qdmaSetTxQosWeightScale16(base);
        }
        tso_refresh_time = jiffies;
    }
#endif
    QDMA_MSG(DBG_ST,"nr_frags:%d, gso_size:%d\n", nr_frags, skb_shinfo(skb)->gso_size);

	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);
	for(i = 0; i <= nr_frags; i++){
    	/* Get unused TX DSCP from TX unused DSCP link list */
    	pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
    	if(unlikely(pNewDscpInfo == NULL)) {
    		gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
    		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
			if((skb->ppe_magic == FOE_MAGIC_SPEED_TEST_UPSTREAM) && (skb->tso_mark & TSO_STATIC_SKB)) {
				skb->tso_mark &= (~TSO_SKB_USING);
			}else{
				dev_kfree_skb_any(skb);
			}

    		return -ENOSR ;
    	}

    	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
    	pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		pTxDscp->msg[1] = 0x7f4007ff;
#elif defined(TCSUPPORT_CPU_EN7580)
		pTxDscp->msg[1] = 0x7f8003ff;
#else  
		pTxDscp->msg[1] = 0x200000;
#endif

        {
			txMsg.pTxMsgW0= &pTxDscp->msg[0];
			txMsg.pTxMsgW1 = &pTxDscp->msg[1];
			qdma_set_txmsg_tso(&txMsg,tso);
			qdma_set_txmsg_ico(&txMsg,ico);
			qdma_set_txmsg_tco(&txMsg,tco);
        }

        /* first data block in the head block, and the subsequent from each fragment. */
        if(i==0){
#if defined(TCSUPPORT_CPU_ARMV8)
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
			ecnt_dcache_wback_inv(((unsigned long)(skb->data)), (skb->len-skb->data_len));
#else
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
			dma_cache_wback_inv((unsigned long)(skb->data), (skb->len-skb->data_len));
#endif
            pTxDscp->ctrl.pkt_len = (skb->len-skb->data_len);
        }
        else{
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
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
    	pTxDscp->ctrl.drop_pkt = 0 ;
#endif

        /* not last segment flag, last block set to 0, and others set to 1. */
        if(i == nr_frags){
            pTxDscp->ctrl.nls = 0;
            gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
        }
        else{
            pTxDscp->ctrl.nls = 1;
            gpQdmaPriv->txUsingPtr[ringIdx]->skb = NULL ;
        }
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
    	dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
    	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif

    	gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;
    	gpQdmaPriv->counters.txCounts[ringIdx]++ ;
    }
#if !defined(TCSUPPORT_CPU_ARMV8)

	SYNC_TYPE4();
#endif
	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;

	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

	return ret ;
}
int qdma_bm_transmit_packet_tso_speedtest(struct sk_buff *skb) __attribute__((optimize("O2")));
int qdma_bm_transmit_packet_tso_speedtest(struct sk_buff *skb)
{
#if defined(TCSUPPORT_CPU_EN7580)
#define ringIdx  7
#else
#define ringIdx  0
#endif
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL ;
	QDMA_DMA_DSCP_T *pTxDscp=NULL ;
	uint base = gpQdmaPriv->csrBaseAddr ;
	ulong flags = 0 ;
	int ret = 0 , i = 0, tso = 0;
    uint offset = 0, size = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	struct bio_vec *frag;
#else
	struct skb_frag_struct *frag;
#endif
	txMsgWord_t txMsg;

	unsigned int nr_frags = skb_shinfo(skb)->nr_frags;
	unsigned short *checksum;
	memset(&txMsg, 0, sizeof(txMsgWord_t));

    QDMA_MSG(DBG_ST,"nr_frags:%d, gso_size:%d\n", nr_frags, skb_shinfo(skb)->gso_size);
	
    if(unlikely(atomic_read(&free_txd_count[ringIdx]) < nr_frags+1)){
        QDMA_MSG(DBG_MSG,"There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", ringIdx, atomic_read(&free_txd_count[ringIdx]), nr_frags);
        return -EFAULT ;
    }
	
    /* fill MSS in checksum field for HW TSO, and enable HW TSO.*/
    if(likely(skb_shinfo(skb)->gso_size)){
        checksum = (unsigned short*)(skb_transport_header(skb)+16);
        *checksum = htons(skb_shinfo(skb)->gso_size);
        tso = 1;
    }
    else{
        checksum = (unsigned short*)(skb_transport_header(skb)+16);
        *checksum = 0;
    }
	
	spin_lock_irqsave(&gpQdmaPriv->txLock[ringIdx], flags);
	for(i = 0; i <= nr_frags; i++){
    	/* Get unused TX DSCP from TX unused DSCP link list */
    	pNewDscpInfo = qdma_bm_pop_tx_dscp(ringIdx) ;
    	if(unlikely(pNewDscpInfo == NULL)) {
    		gpQdmaPriv->counters.noTxDscps[ringIdx]++ ;
    		spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);
			skb->tso_mark &= (~TSO_SKB_USING);
    		return -ENOSR ;
    	}
		
    	pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[ringIdx]->dscpPtr);
    	pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
		
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
		pTxDscp->msg[1] = 0x7f4007ff;
#elif defined(TCSUPPORT_CPU_EN7580)
		pTxDscp->msg[1] = 0x7f8003ff;
#else  
		pTxDscp->msg[1] = 0x200000;
#endif

        {
			txMsg.pTxMsgW0= &pTxDscp->msg[0];
			txMsg.pTxMsgW1 = &pTxDscp->msg[1];
        	qdma_set_txmsg_tso(&txMsg,tso);
			qdma_set_txmsg_ico(&txMsg,1);
			qdma_set_txmsg_tco(&txMsg,1);
        }

        /* first data block in the head block, and the subsequent from each fragment. */
        if(i==0){
#if defined(TCSUPPORT_CPU_ARMV8)
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
			ecnt_dcache_wback_inv(((unsigned long)(skb->data)), 70);
#else
			pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
			dma_cache_wback_inv((unsigned long)(skb->data), 70);
#endif
            pTxDscp->ctrl.pkt_len = (skb->len-skb->data_len);
        }
        else{
            frag = &skb_shinfo(skb)->frags[i-1];
            if(frag){
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
				
				offset = frag->bv_offset;
				size = frag->bv_len;


#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)(page_address(frag->bv_page)+offset));
				ecnt_dcache_wback_inv(((unsigned long)(page_address(frag->bv_page)+offset)), 1);
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)(page_address(frag->bv_page)+offset));
				dma_cache_wback_inv((unsigned long)(page_address(frag->bv_page)+offset), (1));
#endif

#else			
				offset = frag->page_offset;
				size = frag->size;
#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)(page_address(frag->page.p)+offset));
				ecnt_dcache_wback_inv(((unsigned long)(page_address(frag->page.p)+offset)), 1);
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)(page_address(frag->page.p)+offset));
				dma_cache_wback_inv((unsigned long)(page_address(frag->page.p)+offset), (1));
#endif
#endif
                pTxDscp->ctrl.pkt_len = size;
            }
        }
		
    	pTxDscp->ctrl.done = 0 ;
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
    	pTxDscp->ctrl.drop_pkt = 0 ;
#endif
		
        /* not last segment flag, last block set to 0, and others set to 1. */
        if(i == nr_frags){
            pTxDscp->ctrl.nls = 0;
            gpQdmaPriv->txUsingPtr[ringIdx]->skb = skb ;
        }
        else{
            pTxDscp->ctrl.nls = 1;
            gpQdmaPriv->txUsingPtr[ringIdx]->skb = NULL ;
        }
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
    	dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
    	protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		
    	gpQdmaPriv->txUsingPtr[ringIdx] = pNewDscpInfo ;
    	gpQdmaPriv->counters.txCounts[ringIdx]++ ;
    }
#if !defined(TCSUPPORT_CPU_ARMV8)

	SYNC_TYPE4();
#endif
	qdmaSetTxCpuIdx(base, ringIdx, pNewDscpInfo->dscpIdx) ;

	spin_unlock_irqrestore(&gpQdmaPriv->txLock[ringIdx], flags);

	return ret ;
}
#if  defined(TCSUPPORT_TSO_VERIFY)	

/******************************************************************************
******************************************************************************/

void tso_verify_case11_callback(uint msg0, uint msg1, int ringidx){
	tsoTestTxMsg_t tsoTxMsg;
	tsoTxMsg.msg[0] = msg0;
	tsoTxMsg.msg[1] = msg1;
	if(tsoTxMsg.raw.resv == 1){/* SPECIAL_DONT_FREESPECIAL_DONT_FREE */
		/* set this flag for marking the skb shouldn't be freed. */
		gpQdmaPriv->txUsingPtr[ringidx]->skb = NULL ;
	}
}

/* for random interval insert between neighber TxDs. */
void tso_verify_case14_callback(uint msg0, uint msg1, int ringidx, uint dscpIdx){
	tsoTestTxMsg_t tsoTxMsg;
	tsoTxMsg.msg[0] = msg0;
	tsoTxMsg.msg[1] = msg1;
	unsigned int interval=0;
	
	if(tsoTxMsg.raw.resv == 2){/* SPECIAL_RANDOM_INTERVAL */
		interval = random32()%((unsigned int)10000);
		udelay(interval);
		wmb() ;
		qdmaSetTxCpuIdx(gpQdmaPriv->csrBaseAddr, ringidx, dscpIdx) ;
	}
}
#endif


static int tso_localout_offload(struct ECNT_TSO_Data *tso_data)
{
	struct sk_buff* tso_skb;

	if(0 == tsoShortcutPpe)
		return ECNT_CONTINUE;
	
	tso_skb = tso_data->tso_private.tso_skb;
	if(tso_skb){
		tso_data->retValue = qdma_bm_send_to_ppe_tso(tso_skb);
	}
	
	return ECNT_CONTINUE;
}

static int tso_pingpong_v4(struct ECNT_TSO_Data *tso_data)
{
	struct sk_buff* tso_skb;
	tsoRuleEntry_v4_t* skbEntry;

	skbEntry = tso_data->tso_private.pingpong_v4.flow;
	tso_skb = tso_data->tso_private.pingpong_v4.tso_skb;
	if(tso_skb){
		tso_data->retValue = tso_send_pingpong_v4(skbEntry, tso_skb);
	}
	
	return ECNT_CONTINUE;
}

static int tso_destroy_v4(struct ECNT_TSO_Data *tso_data)
{
	struct sock *sk = NULL;

	sk = tso_data->tso_private.sk;
	if(sk){
		tso_data->retValue = tso_session_destroy_v4(sk);
	}
	
	return ECNT_CONTINUE;
}


int tso_transmit_packet(struct ECNT_TSO_Data *tso_data){
	return 0;
}
int tso_tdma_dma_config(struct ECNT_TSO_Data *tso_data){
	return 0;
}
int tso_tdma_dma_enable(struct ECNT_TSO_Data *tso_data){
	return 0;
}



