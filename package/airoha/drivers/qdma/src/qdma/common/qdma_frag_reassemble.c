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
#include <linux/etherdevice.h>
#include <linux/proc_fs.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/protocol.h>
#include <net/ipv6_frag.h>
#include <net/inet_ecn.h>

#include "qdma_bmgr.h"
#include "qdma_frag_reassemble.h"
#include "qdma_ic_dis.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define FRAG_TX_CHANNEL  0
#define FRAG_TX_QUEUE    0

#define IPV4_HDR_LEN     20
#define IPV6_HDR_LEN     40
#define IPV6_FHDR_LEN    8
#define QDMA_DSCP_LEN    (sizeof(QDMA_DMA_DSCP_T))
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
extern atomic_t free_txd_count[];

extern atomic_t chnlLimit[CONFIG_QDMA_CHANNEL];
extern atomic_t queueLimit[CONFIG_QDMA_CHANNEL][CONFIG_QDMA_QUEUE];

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

#if SUPPORT_FRAGMENT_REASSEMBLE
static struct sk_buff *skb_frag_tmp = NULL;
unsigned int frag_tmp_l2_len = 0;
static struct sk_buff *skb6_frag_tmp = NULL;
unsigned int frag6_tmp_l2_len = 0;

unsigned int frag_pkt_eth_err_cnt = 0;
unsigned int frag_pkt_loss_cnt_1 = 0;
unsigned int frag_pkt_loss_cnt_2 = 0;
unsigned int frag_id_err_cnt = 0;
unsigned int frag_offset_err_cnt = 0;
unsigned int frag_dscp_empty_cnt = 0;
unsigned int frag_dscp_null_cnt_1 = 0;
unsigned int frag_dscp_null_cnt_2 = 0;

unsigned int frag_fast = 0;
unsigned int ipv6_frag_id = 0;
#endif
/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
static inline u8 qdma_ip6_frag_ecn(const struct ipv6hdr *ipv6h)
{
	return 1 << (ipv6_get_dsfield(ipv6h) & INET_ECN_MASK);
}

static inline struct frag_queue *qdma_fq_find(struct net *net, __be32 id, const struct ipv6hdr *hdr, int iif)
{
	struct frag_v6_compare_key key = {
		.id = id,
		.saddr = hdr->saddr,
		.daddr = hdr->daddr,
		.user = IP6_DEFRAG_LOCAL_DELIVER,
		.iif = iif,
	};
	struct inet_frag_queue *q;

	if (!(ipv6_addr_type(&hdr->daddr) & (IPV6_ADDR_MULTICAST |
					    IPV6_ADDR_LINKLOCAL)))
		key.iif = 0;

	q = inet_frag_find(net->ipv6.fqdir, &key);
	if (!q)
		return NULL;

	return container_of(q, struct frag_queue, q);
}

static inline int qdma_ip6_frag_reasm(struct frag_queue *fq, struct sk_buff *skb, struct sk_buff *prev_tail, struct net_device *dev)
{
	struct net *net = fq->q.fqdir->net;
	unsigned int nhoff;
	void *reasm_data;
	int payload_len;
	u8 ecn;

	inet_frag_kill(&fq->q);

	ecn = ip_frag_ecn_table[fq->ecn];
	if (unlikely(ecn == 0xff))
		goto out_fail;

	reasm_data = inet_frag_reasm_prepare(&fq->q, skb, prev_tail);
	if (!reasm_data)
		goto out_oom;

	payload_len = ((skb->data - skb_network_header(skb)) -
		       sizeof(struct ipv6hdr) + fq->q.len -
		       sizeof(struct frag_hdr));
	if (payload_len > IPV6_MAXPLEN)
		goto out_oversize;

	/* We have to remove fragment header from datagram and to relocate
	 * header in order to calculate ICV correctly. */
	nhoff = fq->nhoffset;
	skb_network_header(skb)[nhoff] = skb_transport_header(skb)[0];
	memmove(skb->head + sizeof(struct frag_hdr), skb->head,
		(skb->data - skb->head) - sizeof(struct frag_hdr));
	if (skb_mac_header_was_set(skb))
		skb->mac_header += sizeof(struct frag_hdr);
	skb->network_header += sizeof(struct frag_hdr);

	skb_reset_transport_header(skb);

	inet_frag_reasm_finish(&fq->q, skb, reasm_data, true);

	skb->dev = dev;
	ipv6_hdr(skb)->payload_len = htons(payload_len);
	ipv6_change_dsfield(ipv6_hdr(skb), 0xff, ecn);
	IP6CB(skb)->nhoff = nhoff;
	IP6CB(skb)->flags |= IP6SKB_FRAGMENTED;
	IP6CB(skb)->frag_max_size = fq->q.max_size;

	/* Yes, and fold redundant checksum back. 8) */
	skb_postpush_rcsum(skb, skb_network_header(skb),
			   skb_network_header_len(skb));

	rcu_read_lock();
	__IP6_INC_STATS(net, __in6_dev_stats_get(dev, skb), IPSTATS_MIB_REASMOKS);
	rcu_read_unlock();
	fq->q.rb_fragments = RB_ROOT;
	fq->q.fragments_tail = NULL;
	fq->q.last_run_head = NULL;
	return 1;

out_oversize:
	net_dbg_ratelimited("ip6_frag_reasm: payload len = %d\n", payload_len);
	goto out_fail;
out_oom:
	net_dbg_ratelimited("ip6_frag_reasm: no memory for reassembly\n");
out_fail:
	rcu_read_lock();
	__IP6_INC_STATS(net, __in6_dev_stats_get(dev, skb), IPSTATS_MIB_REASMFAILS);
	rcu_read_unlock();
	inet_frag_kill(&fq->q);
	return -1;
}

static inline int qdma_ip6_frag_queue(struct frag_queue *fq, struct sk_buff *skb, struct frag_hdr *fhdr, int nhoff, u32 *prob_offset)
{
	int offset, end, fragsize;
	struct sk_buff *prev_tail;
	struct net_device *dev;
	int err = -ENOENT;
	u8 ecn;

	if (fq->q.flags & INET_FRAG_COMPLETE)
		goto err;

	err = -EINVAL;
	offset = ntohs(fhdr->frag_off) & ~0x7;
	end = offset + (ntohs(ipv6_hdr(skb)->payload_len) -
			((u8 *)(fhdr + 1) - (u8 *)(ipv6_hdr(skb) + 1)));

	if ((unsigned int)end > IPV6_MAXPLEN) {
		*prob_offset = (u8 *)&fhdr->frag_off - skb_network_header(skb);
		/* note that if prob_offset is set, the skb is freed elsewhere,
		 * we do not free it here.
		 */
		return -1;
	}

	ecn = qdma_ip6_frag_ecn(ipv6_hdr(skb));

	if (skb->ip_summed == CHECKSUM_COMPLETE) {
		const unsigned char *nh = skb_network_header(skb);
		skb->csum = csum_sub(skb->csum,
					 csum_partial(nh, (u8 *)(fhdr + 1) - nh,
						  0));
	}

	/* Is this the final fragment? */
	if (!(fhdr->frag_off & htons(IP6_MF))) {
		/* If we already have some bits beyond end
		 * or have different end, the segment is corrupted.
		 */
		if (end < fq->q.len ||
			((fq->q.flags & INET_FRAG_LAST_IN) && end != fq->q.len))
			goto discard_fq;
		fq->q.flags |= INET_FRAG_LAST_IN;
		fq->q.len = end;
	} else {
		/* Check if the fragment is rounded to 8 bytes.
		 * Required by the RFC.
		 */
		if (end & 0x7) {
			/* RFC2460 says always send parameter problem in
			 * this case. -DaveM
			 */
			*prob_offset = offsetof(struct ipv6hdr, payload_len);
			return -1;
		}
		if (end > fq->q.len) {
			/* Some bits beyond end -> corruption. */
			if (fq->q.flags & INET_FRAG_LAST_IN)
				goto discard_fq;
			fq->q.len = end;
		}
	}

	if (end == offset)
		goto discard_fq;

	err = -ENOMEM;
	/* Point into the IP datagram 'data' part. */
	if (!pskb_pull(skb, (u8 *) (fhdr + 1) - skb->data))
		goto discard_fq;

	err = pskb_trim_rcsum(skb, end - offset);
	if (err)
		goto discard_fq;

	/* Note : skb->rbnode and skb->dev share the same location. */
	dev = skb->dev;
	/* Makes sure compiler wont do silly aliasing games */
	barrier();

	prev_tail = fq->q.fragments_tail;
	err = inet_frag_queue_insert(&fq->q, skb, offset, end);
	if (err)
		goto insert_error;

	if (dev)
		fq->iif = dev->ifindex;

	fq->q.stamp = skb->tstamp;
	fq->q.meat += skb->len;
	fq->ecn |= ecn;
	add_frag_mem_limit(fq->q.fqdir, skb->truesize);

	fragsize = -skb_network_offset(skb) + skb->len;
	if (fragsize > fq->q.max_size)
		fq->q.max_size = fragsize;

	/* The first fragment.
	 * nhoffset is obtained from the first fragment, of course.
	 */
	if (offset == 0) {
		fq->nhoffset = nhoff;
		fq->q.flags |= INET_FRAG_FIRST_IN;
	}

	if (fq->q.flags == (INET_FRAG_FIRST_IN | INET_FRAG_LAST_IN) &&
		fq->q.meat == fq->q.len) {
		unsigned long orefdst = skb->_skb_refdst;

		skb->_skb_refdst = 0UL;
		err = qdma_ip6_frag_reasm(fq, skb, prev_tail, dev);
		skb->_skb_refdst = orefdst;
		return err;
	}

	skb_dst_drop(skb);
	return -EINPROGRESS;

insert_error:
	if (err == IPFRAG_DUP) {
		kfree_skb(skb);
		return -EINVAL;
	}
	err = -EINVAL;
discard_fq:
	inet_frag_kill(&fq->q);
err:
	kfree_skb(skb);
	return err;
}

static inline int qdma_ipv6_frag_rcv(struct sk_buff *skb)
{
	struct frag_hdr *fhdr;
	struct frag_queue *fq;
	const struct ipv6hdr *hdr = ipv6_hdr(skb);
	struct net *net = dev_net(skb->dev);
	int iif;

	if (IP6CB(skb)->flags & IP6SKB_FRAGMENTED)
		goto fail_hdr;

	/* Jumbo payload inhibits frag. header */
	if (hdr->payload_len == 0)
		goto fail_hdr;

	if (!pskb_may_pull(skb, (skb_transport_offset(skb) +
				 sizeof(struct frag_hdr))))
		goto fail_hdr;

	hdr = ipv6_hdr(skb);
	fhdr = (struct frag_hdr *)skb_transport_header(skb);

	if (!(fhdr->frag_off & htons(0xFFF9))) {
		/* It is not a fragmented frame */
		skb->transport_header += sizeof(struct frag_hdr);

		IP6CB(skb)->nhoff = (u8 *)fhdr - skb_network_header(skb);
		IP6CB(skb)->flags |= IP6SKB_FRAGMENTED;
		return 1;
	}

	iif = skb->dev ? skb->dev->ifindex : 0;
	fq = qdma_fq_find(net, fhdr->identification, hdr, iif);
	if (fq) {
		u32 prob_offset = 0;
		int ret;

		spin_lock(&fq->q.lock);

		fq->iif = iif;
		ret = qdma_ip6_frag_queue(fq, skb, fhdr, IP6CB(skb)->nhoff,
					 &prob_offset);

		spin_unlock(&fq->q.lock);
		inet_frag_put(&fq->q);
		if (prob_offset) {
			kfree_skb(skb);
		}
		return ret;
	}

	kfree_skb(skb);
	return -1;

fail_hdr:
	kfree_skb(skb);
	return -1;
}

#else

static inline int qdma_ipv6_frag_rcv(struct sk_buff *skb)
{
	QDMA_MSG(DBG_ERR, "Not support\n");
	kfree_skb(skb);
	return -1;
}

#endif

static inline void qdma_frag_update_ppp_payload_length(struct sk_buff *skb, uint frag_Len)
{
	uint16 ether_type = *(uint16*)(skb->data + QDMA_DSCP_LEN + 12);

	if(ether_type == htons(0x8864))
	{
		*(uint16*)(skb->data + QDMA_DSCP_LEN + 18) = htons(skb->len - QDMA_DSCP_LEN - 20 + frag_Len);
		return;
	} 
	else if(ether_type == htons(0x8100))
	{
		ether_type = *(uint16*)(skb->data + QDMA_DSCP_LEN + 16);

		if(ether_type == htons(0x8864))
		{
			*(uint16*)(skb->data + QDMA_DSCP_LEN + 22) = htons(skb->len - QDMA_DSCP_LEN - 24 + frag_Len);
			return;
		}
	}
}

static int qdma_reassemble_handler_v4_slow(struct sk_buff *skb, uint l2_Len, void *dscp_p) 
{
	ulong flags = 0;
	unsigned int base = gpQdmaPriv->csrBaseAddr;
	struct net *net = NULL;
	struct QDMA_DscpInfo_S *pNewDscpInfo = NULL;
	QDMA_DMA_DSCP_T *pTxDscp = NULL;
	uint32 txmsg0 = (0x2<<14)|0x2000|(FRAG_TX_CHANNEL<<3)|FRAG_TX_QUEUE;
	uint32 txmsg1 = 0x102000;  /* hop bit */
	unsigned int frag_cnt = 0, i = 0;
	struct sk_buff *frag_ptr = NULL;
	int ret = 0;
    txMsgWord_t txMsg;

    memset(&txMsg,0,sizeof(txMsgWord_t));

	skb->dev = dev_get_by_name(&init_net, lan_itf);
	skb_reset_mac_header(skb);
	skb_pull_inline(skb, l2_Len);
	skb->dev->last_rx = jiffies;
	skb_reset_network_header(skb);
	net = dev_net(skb->dev);

	if (ip_is_fragment(ip_hdr(skb))) {
		if (ip_defrag(net, skb, IP_DEFRAG_LOCAL_DELIVER)) {
			return 0;
		}
		
		skb_push(skb, l2_Len);
	
		/* get fragment num */
		frag_ptr = skb_shinfo(skb)->frag_list;
		while(1) {
			if(frag_ptr) {
				frag_cnt ++;
				frag_ptr = frag_ptr->next;
			} else {
				break;
			}
		}
		
		if(frag_cnt == 0) {
			/* Prepare HOP info */
			skb_push(skb, QDMA_DSCP_LEN);
			memmove(skb->data, dscp_p, QDMA_DSCP_LEN);

			qdma_frag_update_ppp_payload_length(skb, 0);
            txMsg.pTxMsgW0 = (txMsgWord0_t *)&txmsg0;
            txMsg.pTxMsgW1 = (txMsgWord0_t *)&txmsg1;
			ret = qdma_bm_transmit_packet(skb, FRAG_TX_RING_IDX, &txMsg);
			if(ret){
				QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet error\n");
			}
			return ret;
		}

		if(atomic_read(&free_txd_count[FRAG_TX_RING_IDX]) < (frag_cnt+1)) {
			QDMA_MSG(DBG_MSG,"There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", 
				FRAG_TX_RING_IDX, atomic_read(&free_txd_count[FRAG_TX_RING_IDX]), frag_cnt);
			return -EFAULT ;
		}

		frag_ptr = skb_shinfo(skb)->frag_list;
		spin_lock_irqsave(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags); 
		for(i = 0; i <= frag_cnt; i++) {
			/* Get unused TX DSCP from TX unused DSCP link list */	
			pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(FRAG_TX_RING_IDX);

			if(unlikely(pNewDscpInfo == NULL)) {
				gpQdmaPriv->counters.noTxDscps[FRAG_TX_RING_IDX]++;
				QDMA_MSG(DBG_MSG, "qdma_fragment_assemble_handler_v4_slow pNewDscpInfo is NULL\n"); 
				spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
				return -ENOSR ;
			}

#ifdef TCSUPPORT_CPU_ARMV8
			pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#else
			pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#endif
			pTxDscp->msg[0] = txmsg0;
			pTxDscp->msg[1] = txmsg1;
			pTxDscp->next_idx = pNewDscpInfo->dscpIdx;

			/* first data block in the head block, and the subsequent from each fragment. */
			if(i == 0) {
				/* Prepare HOP info */
				skb_push(skb, QDMA_DSCP_LEN);
				memmove(skb->data, dscp_p, QDMA_DSCP_LEN);
				qdma_frag_update_ppp_payload_length(skb, 0);
				
#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
				ecnt_dcache_wback_inv(((unsigned long)(skb->data)), (skb->len - skb->data_len));
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
				dma_cache_wback_inv((unsigned long)(skb->data), (skb->len - skb->data_len));
#endif 
				pTxDscp->ctrl.pkt_len = (skb->len - skb->data_len);
			} else {
#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)frag_ptr->data);
				ecnt_dcache_wback_inv(((unsigned long)(frag_ptr->data)), frag_ptr->len);
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)frag_ptr->data);
				dma_cache_wback_inv((unsigned long)(frag_ptr->data), frag_ptr->len);
#endif 
				pTxDscp->ctrl.pkt_len = frag_ptr->len;
				frag_ptr = frag_ptr->next; /* for next fragment */
			}

			pTxDscp->ctrl.done = 0;
#if SUPPORT_TX_DROP_BIT
			pTxDscp->ctrl.drop_pkt = 0;
#endif

			/* not last segment flag, last block set to 0, and others set to 1. */
			if(i == frag_cnt){
				pTxDscp->ctrl.nls = 0;
				gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = skb ;
				/* chnlLimit & queueLimit is for skb count. */
				atomic_inc(&chnlLimit[FRAG_TX_CHANNEL]);
				atomic_inc(&queueLimit[FRAG_TX_CHANNEL][FRAG_TX_QUEUE]);
			} else {
				pTxDscp->ctrl.nls = 1;
				gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = NULL ;
			}

#if defined(TCSUPPORT_CPU_ARMV8)
			ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
			dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
			protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
			gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX] = pNewDscpInfo ;				
			gpQdmaPriv->counters.txCounts[FRAG_TX_RING_IDX]++ ;
		}
			
#if !defined(TCSUPPORT_CPU_ARMV8)
		SYNC_TYPE4();
#endif
		qdmaSetTxCpuIdx(base, FRAG_TX_RING_IDX, pNewDscpInfo->dscpIdx) ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
		
		return 0;
	} else {
		QDMA_MSG(DBG_ERR, "Not IPv4 fragment\n");
		return -1;
	}

	return 0;
}

static int qdma_reassemble_handler_v4_fast(struct sk_buff *skb, uint l2_Len, void *dscp_p) 
{
	ulong flags = 0;
	uint base = gpQdmaPriv->csrBaseAddr;
	struct iphdr* iph_first = NULL;
	struct iphdr* iph = (struct iphdr*)(skb->data + l2_Len);
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL;
	QDMA_DMA_DSCP_T *pTxDscp=NULL;
	uint32 txmsg0 = (0x2<<14)|0x2000|(FRAG_TX_CHANNEL<<3)|FRAG_TX_QUEUE;
	uint32 txmsg1 = 0x102000;  /* hop bit */

	if((iph->frag_off & htons(IP_MF)) != 0) /* receive a first fragment */
	{
		if(skb_frag_tmp != NULL) /* packet loss */
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet loss 1\n");
			dev_kfree_skb_any(skb_frag_tmp);
			skb_frag_tmp = NULL;

			frag_pkt_loss_cnt_1 ++;
		}

		skb_frag_tmp = skb;
		frag_tmp_l2_len = l2_Len;
		iph->frag_off = iph->frag_off & htons(~IP_MF); /* clear more fragment flag */
	}
	else /* receive a second fragment */
	{
		if(skb_frag_tmp == NULL)
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet loss 2\n");

			frag_pkt_loss_cnt_2 ++;
			return -1;
		}

		/* check the IP Identification */
		iph_first = (struct iphdr*)(skb_frag_tmp->data + frag_tmp_l2_len);
		if(iph_first->id != iph->id)
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet ip identification error\n");

			frag_id_err_cnt ++;
			return -1;
		}

		/* check the frag offset */
		if( ((ntohs(iph->frag_off) & IP_OFFSET) << 3) != (ntohs(iph_first->tot_len) - (iph_first->ihl << 2)) )
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet ip frag offset error\n");

			frag_offset_err_cnt ++;
			return -1;
		}

		/* update IP->total_len */
		iph_first->tot_len = htons(ntohs(iph_first->tot_len) + skb->len - (l2_Len+20));

		/* Prepare HOP info */
		skb_push(skb_frag_tmp, QDMA_DSCP_LEN);
		memmove(skb_frag_tmp->data, dscp_p, QDMA_DSCP_LEN);
		qdma_frag_update_ppp_payload_length(skb_frag_tmp, skb->len - (l2_Len+20));

		spin_lock_irqsave(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
		if(atomic_read(&free_txd_count[FRAG_TX_RING_IDX]) < 3){
			dev_kfree_skb_any(skb_frag_tmp);
			skb_frag_tmp = NULL;
			frag_tmp_l2_len = 0;
			QDMA_MSG(DBG_ST, "fragment_assemble_handler dscp is not enough\n"); 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);

			frag_dscp_empty_cnt++;
			return -ENOSR ;
		}
		
		/* transmit the first fragment */
		pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(FRAG_TX_RING_IDX);
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[FRAG_TX_RING_IDX]++;
			QDMA_MSG(DBG_ST, "fragment_assemble_handler pNewDscpInfo is NULL 1\n"); 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);

			frag_dscp_null_cnt_1 ++;
			return -ENOSR ;
		}

		atomic_inc(&chnlLimit[FRAG_TX_CHANNEL]);
		atomic_inc(&queueLimit[FRAG_TX_CHANNEL][FRAG_TX_QUEUE]);

#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#else
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#endif
		pTxDscp->msg[0] = txmsg0;
		pTxDscp->msg[1] = txmsg1;
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb_frag_tmp->data);
		ecnt_dcache_wback_inv(((unsigned long)(skb_frag_tmp->data)), skb_frag_tmp->len);
#else
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb_frag_tmp->data);
		dma_cache_wback_inv((unsigned long)(skb_frag_tmp->data), skb_frag_tmp->len);
#endif
		pTxDscp->ctrl.pkt_len = skb_frag_tmp->len ;
		pTxDscp->ctrl.done = 0;
#if SUPPORT_TX_DROP_BIT
		pTxDscp->ctrl.drop_pkt = 0;
#endif
		pTxDscp->ctrl.nls = 1; /* *****Notice***** */
		
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = skb_frag_tmp;
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX] = pNewDscpInfo;

		gpQdmaPriv->counters.txCounts[FRAG_TX_RING_IDX]++;
		skb_frag_tmp = NULL;
		frag_tmp_l2_len = 0;

		/* transmit the second fragment */
		pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(FRAG_TX_RING_IDX);
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[FRAG_TX_RING_IDX]++;
			QDMA_MSG(DBG_ST, "fragment_assemble_handler pNewDscpInfo is NULL 2\n"); 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);

			frag_dscp_null_cnt_2 ++;
			return -ENOSR ;
		}

		atomic_inc(&chnlLimit[FRAG_TX_CHANNEL]);
		atomic_inc(&queueLimit[FRAG_TX_CHANNEL][FRAG_TX_QUEUE]);

#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#else
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#endif
		pTxDscp->msg[0] = txmsg0;
		pTxDscp->msg[1] = txmsg1;
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)(skb->data+l2_Len+20));
		ecnt_dcache_wback_inv(((unsigned long)(skb->data+l2_Len+20)), skb->len - (l2_Len+20));
#else
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)(skb->data+l2_Len+20));
		dma_cache_wback_inv((unsigned long)(skb->data+l2_Len+20), skb->len - (l2_Len+20));
#endif
		pTxDscp->ctrl.pkt_len = skb->len - (l2_Len+20);
		pTxDscp->ctrl.done = 0;
#if SUPPORT_TX_DROP_BIT
		pTxDscp->ctrl.drop_pkt = 0;
#endif
		pTxDscp->ctrl.nls = 0; /* *****Notice***** */
		
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = skb;
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX] = pNewDscpInfo;

		gpQdmaPriv->counters.txCounts[FRAG_TX_RING_IDX]++;

		/* Two fragments have been transmited, set the CPU index */
#if !defined(TCSUPPORT_CPU_ARMV8)
		SYNC_TYPE4();
#endif
		qdmaSetTxCpuIdx(base, FRAG_TX_RING_IDX, pNewDscpInfo->dscpIdx);
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
		
	}

	return 0;
}

static int qdma_reassemble_handler_v6_slow(struct sk_buff *skb, uint l2_Len, void *dscp_p) 
{
	ulong flags = 0;
	unsigned int base = gpQdmaPriv->csrBaseAddr;
	struct QDMA_DscpInfo_S *pNewDscpInfo = NULL;
	QDMA_DMA_DSCP_T *pTxDscp = NULL;
	uint32 txmsg0 = (0x2<<14)|0x2000|(FRAG_TX_CHANNEL<<3)|FRAG_TX_QUEUE;
	uint32 txmsg1 = 0x102000;  /* hop bit */
	unsigned int frag_cnt = 0, i = 0;
	struct sk_buff *frag_ptr = NULL;
	struct ipv6hdr *hdr = NULL;
	struct frag_hdr *fhdr;
	uint8 version = 0, priority = 0, nexthdr=0;
	int ret = 0;
    txMsgWord_t txMsg;

    memset(&txMsg,0,sizeof(txMsgWord_t));

	skb->dev = dev_get_by_name(&init_net, lan_itf);
	skb_reset_mac_header(skb);
	skb_pull_inline(skb, l2_Len);
	skb->dev->last_rx = jiffies;
	skb_reset_network_header(skb);
	skb->transport_header = skb->network_header + sizeof(*hdr);

	hdr = ipv6_hdr(skb);
	if (hdr->nexthdr != NEXTHDR_FRAGMENT) {
		QDMA_MSG(DBG_ERR, "Not IPv6 fragment\n");
		return -1;
	}

	fhdr = (struct frag_hdr *)skb_transport_header(skb);
	/* store the header info */
	version = hdr->version;
	priority = hdr->priority;
	nexthdr = fhdr->nexthdr;

	if (1 == qdma_ipv6_frag_rcv(skb)) { /* start to send packet */
		/* restore the header info */
		hdr = (struct ipv6hdr *)skb->data;
		hdr->version = version;
		hdr->priority = priority;
		hdr->nexthdr = nexthdr;
		skb_push(skb, l2_Len);

		/* get fragment num */
		frag_ptr = skb_shinfo(skb)->frag_list;
		while(1) {
			if(frag_ptr) {
				frag_cnt ++;
				frag_ptr = frag_ptr->next;
			} else {
				break;
			}
		}
		
		if(frag_cnt == 0) {
			/* Prepare HOP info */
			skb_push(skb, QDMA_DSCP_LEN);
			memmove(skb->data, dscp_p, QDMA_DSCP_LEN);
			qdma_frag_update_ppp_payload_length(skb, 0);

            txMsg.pTxMsgW0 = (txMsgWord0_t *)&txmsg0;
            txMsg.pTxMsgW1 = (txMsgWord1_t *)&txmsg1;
			ret = qdma_bm_transmit_packet(skb, FRAG_TX_RING_IDX, &txMsg);
			if(ret){
				QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet error\n");
			}
			return ret;
		}

		if(atomic_read(&free_txd_count[FRAG_TX_RING_IDX]) < (frag_cnt+1)) {
			QDMA_MSG(DBG_MSG,"There are not enough TxDs! free_txd_count[%d]:%d nr_frags:%d\n", 
				FRAG_TX_RING_IDX, atomic_read(&free_txd_count[FRAG_TX_RING_IDX]), frag_cnt);
			return -EFAULT ;
		}

		frag_ptr = skb_shinfo(skb)->frag_list;
		spin_lock_irqsave(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags); 
		for(i = 0; i <= frag_cnt; i++) {
			/* Get unused TX DSCP from TX unused DSCP link list */	
			pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(FRAG_TX_RING_IDX);

			if(unlikely(pNewDscpInfo == NULL)) {
				gpQdmaPriv->counters.noTxDscps[FRAG_TX_RING_IDX]++;
				QDMA_MSG(DBG_MSG, "qdma_fragment_assemble_handler_v4_slow pNewDscpInfo is NULL\n"); 
				spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
				return -ENOSR ;
			}

#ifdef TCSUPPORT_CPU_ARMV8
			pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#else
			pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#endif
			pTxDscp->msg[0] = txmsg0;
			pTxDscp->msg[1] = txmsg1;
			pTxDscp->next_idx = pNewDscpInfo->dscpIdx;

			/* first data block in the head block, and the subsequent from each fragment. */
			if(i == 0) {
				/* Prepare HOP info */
				skb_push(skb, QDMA_DSCP_LEN);
				memmove(skb->data, dscp_p, QDMA_DSCP_LEN);
				qdma_frag_update_ppp_payload_length(skb, 0);
				
#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb->data);
				ecnt_dcache_wback_inv(((unsigned long)(skb->data)), (skb->len - skb->data_len));
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb->data);
				dma_cache_wback_inv((unsigned long)(skb->data), (skb->len - skb->data_len));
#endif 
				pTxDscp->ctrl.pkt_len = (skb->len - skb->data_len);
			} else {
#if defined(TCSUPPORT_CPU_ARMV8)
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)frag_ptr->data);
				ecnt_dcache_wback_inv(((unsigned long)(frag_ptr->data)), frag_ptr->len);
#else
				pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)frag_ptr->data);
				dma_cache_wback_inv((unsigned long)(frag_ptr->data), frag_ptr->len);
#endif 
				pTxDscp->ctrl.pkt_len = frag_ptr->len;
				frag_ptr = frag_ptr->next; /* for next fragment */
			}

			pTxDscp->ctrl.done = 0;
#if SUPPORT_TX_DROP_BIT
			pTxDscp->ctrl.drop_pkt = 0;
#endif

			/* not last segment flag, last block set to 0, and others set to 1. */
			if(i == frag_cnt){
				pTxDscp->ctrl.nls = 0;
				gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = skb ;
				/* chnlLimit & queueLimit is for skb count. */
				atomic_inc(&chnlLimit[FRAG_TX_CHANNEL]);
				atomic_inc(&queueLimit[FRAG_TX_CHANNEL][FRAG_TX_QUEUE]);
			} else {
				pTxDscp->ctrl.nls = 1;
				gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = NULL ;
			}

#if defined(TCSUPPORT_CPU_ARMV8)
			ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
			dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
			protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
			gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX] = pNewDscpInfo ;				
			gpQdmaPriv->counters.txCounts[FRAG_TX_RING_IDX]++ ;

		}
			
#if !defined(TCSUPPORT_CPU_ARMV8)
		SYNC_TYPE4();
#endif
		qdmaSetTxCpuIdx(base, FRAG_TX_RING_IDX, pNewDscpInfo->dscpIdx) ;
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
		
		return 0;			
	}
	
	return 0;
}

static int qdma_reassemble_handler_v6_fast(struct sk_buff *skb, uint l2_Len, void *dscp_p) 
{
	ulong flags = 0;
	uint base = gpQdmaPriv->csrBaseAddr;
	struct ipv6hdr* ip6h_first = NULL;
	struct frag_hdr* ip6fh_first = NULL;
	struct ipv6hdr* ip6h = (struct ipv6hdr*)(skb->data + l2_Len);
	struct frag_hdr* ip6fh = (struct frag_hdr*)(skb->data + l2_Len + 40);
	struct QDMA_DscpInfo_S *pNewDscpInfo=NULL;
	QDMA_DMA_DSCP_T *pTxDscp=NULL;
	uint32 txmsg0 = (0x2<<14)|0x2000|(FRAG_TX_CHANNEL<<3)|FRAG_TX_QUEUE;
	uint32 txmsg1 = 0x102000;  /* hop bit */

	if(ip6h->nexthdr != NEXTHDR_FRAGMENT)
	{
		QDMA_MSG(DBG_ERR, "IPv6 next header error\n");
		return -1;
	}

	if((ip6fh->frag_off & htons(IP6_MF)) != 0) /* receive a first fragment */
	{
		if(skb6_frag_tmp != NULL) /* packet loss */
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet loss 1\n");
			dev_kfree_skb_any(skb6_frag_tmp);
			skb6_frag_tmp = NULL;

			frag_pkt_loss_cnt_1 ++;
		}

		
		skb6_frag_tmp = skb;
		frag6_tmp_l2_len = l2_Len;
		ip6h->nexthdr = ip6fh->nexthdr; /* update next header info */
	}
	else /* receive a second fragment */
	{
		if(skb6_frag_tmp == NULL)
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet loss 2\n");

			frag_pkt_loss_cnt_2 ++;
			return -1;
		}

		/* check the IP Identification */
		ip6h_first = (struct ipv6hdr*)(skb6_frag_tmp->data + frag6_tmp_l2_len);
		ip6fh_first = (struct frag_hdr*)(skb6_frag_tmp->data + frag6_tmp_l2_len + 40);
		if(ip6fh_first->identification != ip6fh->identification)
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet ip identification error\n");

			frag_id_err_cnt ++;
			return -1;
		}

		/* check the frag offset */
		if( (ntohs(ip6fh->frag_off) & IP6_OFFSET) != (ntohs(ip6h_first->payload_len) - 8) )
		{
			QDMA_MSG(DBG_ST, "fragment_assemble_handler packet ip frag offset error\n");

			frag_offset_err_cnt ++;
			return -1;
		}

		/* update IP->payload_len */
		ip6h_first->payload_len = htons(ntohs(ip6h_first->payload_len) - 8 + skb->len - (l2_Len+40+8));
		/* delete the IPv6 fragment header */
		memmove(skb6_frag_tmp->data + 8, skb6_frag_tmp->data, frag6_tmp_l2_len + 40);

		/* Prepare HOP info */
		skb_push(skb6_frag_tmp, QDMA_DSCP_LEN - 8);
		memmove(skb6_frag_tmp->data, dscp_p, QDMA_DSCP_LEN);
		qdma_frag_update_ppp_payload_length(skb6_frag_tmp, skb->len - (l2_Len+48));

		spin_lock_irqsave(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
		if(atomic_read(&free_txd_count[FRAG_TX_RING_IDX]) < 3){
			dev_kfree_skb_any(skb6_frag_tmp);
			skb6_frag_tmp = NULL;
			frag6_tmp_l2_len = 0;
			QDMA_MSG(DBG_ST, "fragment_assemble_handler dscp is not enough\n"); 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);

			frag_dscp_empty_cnt++;
			return -ENOSR ;
		}
		
		/* transmit the first fragment */
		pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(FRAG_TX_RING_IDX);
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[FRAG_TX_RING_IDX]++;
			QDMA_MSG(DBG_ST, "fragment_assemble_handler pNewDscpInfo is NULL 1\n"); 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);

			frag_dscp_null_cnt_1 ++;
			return -ENOSR ;
		}

		atomic_inc(&chnlLimit[FRAG_TX_CHANNEL]);
		atomic_inc(&queueLimit[FRAG_TX_CHANNEL][FRAG_TX_QUEUE]);

#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#else
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#endif
		pTxDscp->msg[0] = txmsg0;
		pTxDscp->msg[1] = txmsg1;
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)skb6_frag_tmp->data);
		ecnt_dcache_wback_inv(((unsigned long)(skb6_frag_tmp->data)), skb6_frag_tmp->len);
#else
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)skb6_frag_tmp->data);
		dma_cache_wback_inv((unsigned long)(skb6_frag_tmp->data), skb6_frag_tmp->len);
#endif
		pTxDscp->ctrl.pkt_len = skb6_frag_tmp->len;
		pTxDscp->ctrl.done = 0;
#if SUPPORT_TX_DROP_BIT
		pTxDscp->ctrl.drop_pkt = 0;
#endif
		pTxDscp->ctrl.nls = 1; /* *****Notice***** */
		
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = skb6_frag_tmp;
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX] = pNewDscpInfo;

		gpQdmaPriv->counters.txCounts[FRAG_TX_RING_IDX]++;
		skb6_frag_tmp = NULL;
		frag6_tmp_l2_len = 0;

		/* transmit the second fragment */
		pNewDscpInfo = (struct QDMA_DscpInfo_S *)qdma_bm_pop_tx_dscp(FRAG_TX_RING_IDX);
		if(unlikely(pNewDscpInfo == NULL)) {
			gpQdmaPriv->counters.noTxDscps[FRAG_TX_RING_IDX]++;
			QDMA_MSG(DBG_ST, "fragment_assemble_handler pNewDscpInfo is NULL 2\n"); 
			spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);

			frag_dscp_null_cnt_2 ++;
			return -ENOSR ;
		}

		atomic_inc(&chnlLimit[FRAG_TX_CHANNEL]);
		atomic_inc(&queueLimit[FRAG_TX_CHANNEL][FRAG_TX_QUEUE]);

#ifdef TCSUPPORT_CPU_ARMV8
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((unsigned long)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#else
		pTxDscp = (QDMA_DMA_DSCP_T *)K1_TO_K0((uint32)gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->dscpPtr);
#endif
		pTxDscp->msg[0] = txmsg0;
		pTxDscp->msg[1] = txmsg1;
		pTxDscp->next_idx = pNewDscpInfo->dscpIdx ;
#if defined(TCSUPPORT_CPU_ARMV8)
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((unsigned long)(skb->data+l2_Len+48));
		ecnt_dcache_wback_inv(((unsigned long)(skb->data+l2_Len+48)), skb->len - (l2_Len+48));
#else
		pTxDscp->pkt_addr = K0_TO_PHYSICAL((uint)(skb->data+l2_Len+48));
		dma_cache_wback_inv((unsigned long)(skb->data+l2_Len+48), skb->len - (l2_Len+48));
#endif
		pTxDscp->ctrl.pkt_len = skb->len - (l2_Len+48);
		pTxDscp->ctrl.done = 0;
#if SUPPORT_TX_DROP_BIT
		pTxDscp->ctrl.drop_pkt = 0;
#endif
		pTxDscp->ctrl.nls = 0; /* *****Notice***** */
		
#if defined(TCSUPPORT_CPU_ARMV8)
		ecnt_dcache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#elif defined(TCSUPPORT_MIPS_1004K)
		dma_cache_wback_inv(((unsigned long)(pTxDscp)), sizeof(QDMA_DMA_DSCP_T));
#else
		protected_cache_op(Hit_Writeback_Inv_D, ((unsigned long)(pTxDscp)));
#endif
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX]->skb = skb;
		gpQdmaPriv->txUsingPtr[FRAG_TX_RING_IDX] = pNewDscpInfo;

		gpQdmaPriv->counters.txCounts[FRAG_TX_RING_IDX]++;

		/* Two fragments have been transmited, set the CPU index */
#if !defined(TCSUPPORT_CPU_ARMV8)
		SYNC_TYPE4();
#endif
		qdmaSetTxCpuIdx(base, FRAG_TX_RING_IDX, pNewDscpInfo->dscpIdx);
		spin_unlock_irqrestore(&gpQdmaPriv->txLock[FRAG_TX_RING_IDX], flags);
		
	}

	return 0;
}

static int qdma_fragment_handler_v4(struct sk_buff *skb, uint l2_Len, uint mtu, void *dscp_p) 
{
	int ret = 0;
	uint32 txmsg0 = (0x2<<14)|0x2000|(FRAG_TX_CHANNEL<<3)|FRAG_TX_QUEUE; 
	uint32 txmsg1 = 0x102000;  /* hop bit */
	uint16 frag_off = 0;
	uint32 offset = 0, offset2=0;
	struct iphdr* iph = (struct iphdr*)(skb->data + l2_Len);
	struct sk_buff *skb2 = allocate_skb_buffer();
    txMsgWord_t txMsg;

    memset(&txMsg,0,sizeof(txMsgWord_t));

	if(skb2 == NULL){
		return -1;
	}

	/*first fragment*/
	frag_off = (mtu - IPV4_HDR_LEN)/8;
	offset = frag_off * 8;

	/* Prepare HOP info */
	skb_push(skb, QDMA_DSCP_LEN);
	memmove(skb->data, dscp_p, QDMA_DSCP_LEN);

	/*modify tot_len&flags, send pkt data from l2_header to ip->flags*/
	iph->tot_len = htons(IPV4_HDR_LEN + offset);
	iph->frag_off = htons(IP_MF);

	/*second fragment*/
	skb_put(skb2, skb->len - offset);
	memmove(skb2->data, skb->data, QDMA_DSCP_LEN + l2_Len + IPV4_HDR_LEN);
	memmove(skb2->data + QDMA_DSCP_LEN + l2_Len + IPV4_HDR_LEN, 
		skb->data + QDMA_DSCP_LEN + l2_Len + IPV4_HDR_LEN + offset,  
		skb->len - (QDMA_DSCP_LEN + l2_Len + IPV4_HDR_LEN + offset));

	offset2 = skb->len - (QDMA_DSCP_LEN + l2_Len + IPV4_HDR_LEN + offset);
	skb->len -= offset2;
	skb->tail -= offset2;
	iph = (struct iphdr*)(skb2->data + QDMA_DSCP_LEN + l2_Len);
	iph->tot_len = htons(skb2->len - QDMA_DSCP_LEN - l2_Len);
	iph->frag_off = htons(frag_off);

	/* Start to Transmit */
	qdma_frag_update_ppp_payload_length(skb, 0);
    txMsg.pTxMsgW0 = (txMsgWord0_t *)&txmsg0;
    txMsg.pTxMsgW1 = (txMsgWord1_t *)&txmsg1;
	ret = qdma_bm_transmit_packet(skb, FRAG_TX_RING_IDX, &txMsg);
	if(ret){
		QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet error\n");
		dev_kfree_skb_any(skb);
	}

	qdma_frag_update_ppp_payload_length(skb2, 0);
    txMsg.pTxMsgW0 = (txMsgWord0_t *)&txmsg0;
    txMsg.pTxMsgW1 = (txMsgWord1_t *)&txmsg1;
	ret = qdma_bm_transmit_packet(skb2, FRAG_TX_RING_IDX, &txMsg);
	if(ret){
		QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet error\n");
		dev_kfree_skb_any(skb2);
	}

	return 0;
}

static int qdma_fragment_handler_v6(struct sk_buff *skb, uint l2_Len, uint mtu, void *dscp_p) 
{
	int ret = 0;
	uint32 txmsg0 = (0x2<<14)|0x0000|(FRAG_TX_CHANNEL<<3)|FRAG_TX_QUEUE; 
	uint32 txmsg1 = 0x102000;  /* hop bit */
	uint16 frag_off = 0;
	uint32 offset = 0, offset2=0;
	struct ipv6hdr* ip6h = NULL;
	struct frag_hdr *ip6fh = NULL;
	struct sk_buff *skb2 = allocate_skb_buffer();
	uint16 nxt_hdr = 0;
    txMsgWord_t txMsg;

    memset(&txMsg,0,sizeof(txMsgWord_t));

	if(skb2 == NULL){
		return -1;
	}
	
	/*first fragment*/
	frag_off = (mtu - IPV6_HDR_LEN - IPV6_FHDR_LEN)/8;
	offset = frag_off * 8;

	/* Prepare HOP info */
	skb_push(skb, IPV6_FHDR_LEN + QDMA_DSCP_LEN);
	memmove(skb->data, dscp_p, QDMA_DSCP_LEN);

	memmove(skb->data + QDMA_DSCP_LEN, skb->data + QDMA_DSCP_LEN + IPV6_FHDR_LEN, l2_Len + IPV6_HDR_LEN);

	ip6h = (struct ipv6hdr*)(skb->data + QDMA_DSCP_LEN + l2_Len);
	/* update fragment header's next header */
	nxt_hdr = ip6h->nexthdr;
	ip6h->nexthdr = NEXTHDR_FRAGMENT;
	/* modify payload_len */
	ip6h->payload_len = htons(IPV6_FHDR_LEN + offset);
	/* modify fragment header */
	ip6fh = (struct frag_hdr*)(skb->data + QDMA_DSCP_LEN + l2_Len + IPV6_HDR_LEN);
	ip6fh->nexthdr = nxt_hdr;
	ip6fh->reserved = 0;
	ip6fh->frag_off = htons(IP6_MF);
	ip6fh->identification = htonl(ipv6_frag_id ++);

	/*second fragment*/
	skb_put(skb2, skb->len - offset);
	memmove(skb2->data, skb->data, QDMA_DSCP_LEN + l2_Len + IPV6_HDR_LEN + IPV6_FHDR_LEN);
	memmove(skb2->data + QDMA_DSCP_LEN + l2_Len + IPV6_HDR_LEN + IPV6_FHDR_LEN, 
		skb->data + QDMA_DSCP_LEN + l2_Len + IPV6_HDR_LEN + IPV6_FHDR_LEN + offset,  
		skb->len - (QDMA_DSCP_LEN + l2_Len + IPV6_HDR_LEN + IPV6_FHDR_LEN + offset));

	offset2 = skb->len - (QDMA_DSCP_LEN + l2_Len + IPV6_HDR_LEN + IPV6_FHDR_LEN + offset);
	skb->len -= offset2;
	skb->tail -= offset2;
	ip6h = (struct ipv6hdr*)(skb2->data + QDMA_DSCP_LEN + l2_Len);
	ip6h->payload_len = htons(skb2->len - QDMA_DSCP_LEN - l2_Len - IPV6_HDR_LEN);
	ip6fh = (struct frag_hdr*)(skb2->data + QDMA_DSCP_LEN + l2_Len + IPV6_HDR_LEN);
	ip6fh->frag_off = htons(frag_off<<3);

	/* Start to Transmit */
	qdma_frag_update_ppp_payload_length(skb, 0);
    txMsg.pTxMsgW0 = (txMsgWord0_t *)&txmsg0;
    txMsg.pTxMsgW1 = (txMsgWord1_t *)&txmsg1;
	ret = qdma_bm_transmit_packet(skb, FRAG_TX_RING_IDX, &txMsg);
	if(ret){
		QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet error\n");
		dev_kfree_skb_any(skb);
	}

	qdma_frag_update_ppp_payload_length(skb2, 0);
    txMsg.pTxMsgW0 = (txMsgWord0_t *)&txmsg0;
    txMsg.pTxMsgW1 = (txMsgWord1_t *)&txmsg1;
	ret = qdma_bm_transmit_packet(skb2, FRAG_TX_RING_IDX, &txMsg);
	if(ret){
		QDMA_MSG(DBG_MSG,"qdma_bm_transmit_packet error\n");
		dev_kfree_skb_any(skb2);
	}

	return 0;
}

int qdma_fragment_reassemble_handler(struct sk_buff *skb, uint rx_len, void *dscp_p, int ringIdx) 
{
	QDMA_DMA_DSCP_T *rxDscp = (QDMA_DMA_DSCP_T *)dscp_p;
	qdmaRxMsgHop_t *curRxMsg = (qdmaRxMsgHop_t *)rxDscp->msg;
	uint ip6 = curRxMsg->rxMsgHopW1.raw.ip6;
	uint ip4 = curRxMsg->rxMsgHopW1.raw.ip4;
	uint l2_len = curRxMsg->rxMsgHopW2.raw.l2_len;
	uint mtu = curRxMsg->rxMsgHopW2.raw.mtu;

	if(unlikely(skb == NULL)) {
		QDMA_ERR("Error Skb NULL, fail to receive!\n");
		return 1;
	}
	skb_put(skb, rx_len);
	
	/* ========= Code for Fragement ========= */
	if( (FRAG_RX_RING_IDX == ringIdx) && (HOP_TO_FRAGMENT == curRxMsg->rxMsgHopW0.raw.hop_flags) ) {
		if (skb->len <= (l2_len + mtu)) {
			QDMA_MSG(DBG_ERR, "skb length error, skb->len=%d, l2_len=%d, mtu=%d\n", skb->len, l2_len, mtu);
			frag_pkt_eth_err_cnt++;
			goto handle_err;
		}
		
		if(ip4 == 1) { /* ipv4 */
			if(0 > qdma_fragment_handler_v4(skb, l2_len, mtu, dscp_p)) {
				goto handle_err;
			} else {
				goto handle_done;
			}
		} else if (ip6 == 1) { /* ipv6 */
			if(0 > qdma_fragment_handler_v6(skb, l2_len, mtu, dscp_p)) {
				goto handle_err;
			} else {
				goto handle_done;
			}
		} else {
			QDMA_MSG(DBG_ERR, "error ether type\n");
			frag_pkt_eth_err_cnt++;
			goto handle_err;
		}
	}
	/* ========= Code for Reassemble ========= */
	else if( (ASSE_RX_RING_IDX == ringIdx) && (HOP_TO_ASSEMBLE == curRxMsg->rxMsgHopW0.raw.hop_flags) ) {
		if(ip4 == 1) { /* ipv4 */
			if (frag_fast) {
				if(0 > qdma_reassemble_handler_v4_fast(skb, l2_len, dscp_p)) {
					goto handle_err;
				} else {
					goto handle_done;
				}
			} else {
				if(0 > qdma_reassemble_handler_v4_slow(skb, l2_len, dscp_p)) {
					goto handle_err;
				} else {
					goto handle_done;
				}
			}
		} else if (ip6 == 1) { /* ipv6 */
			if (frag_fast) {
				if(0 > qdma_reassemble_handler_v6_fast(skb, l2_len, dscp_p)) {
					goto handle_err;
				} else {
					goto handle_done;
				}
			} else {
				if(0 > qdma_reassemble_handler_v6_slow(skb, l2_len, dscp_p)) {
					goto handle_err;
				} else {
					goto handle_done;
				}
			}
		} else {
			QDMA_MSG(DBG_ERR, "error ether type\n");
			frag_pkt_eth_err_cnt++;
			goto handle_err;
		}
	}

handle_err:
	QDMA_MSG(DBG_ERR, "handle_err\n");
	QDMA_ERR("msg0=%.8X, msg1=%.8X, msg2=%.8X, msg3=%.8X\n",
			curRxMsg->rxMsgHopW0.word, curRxMsg->rxMsgHopW1.word, curRxMsg->rxMsgHopW2.word, curRxMsg->rxMsgHopW3.word);
	dev_kfree_skb_any(skb);
	return 1;

handle_done:
	return 0;

}

static int qdma_frag_reassemble_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+index, "frag_pkt_eth_err_cnt = %d\n", frag_pkt_eth_err_cnt);
	index += sprintf(buf+index, "frag_pkt_loss_cnt_1 = %d\n", frag_pkt_loss_cnt_1);
	index += sprintf(buf+index, "frag_pkt_loss_cnt_2 = %d\n", frag_pkt_loss_cnt_2);
	index += sprintf(buf+index, "frag_id_err_cnt = %d\n", frag_id_err_cnt);
	index += sprintf(buf+index, "frag_offset_err_cnt = %d\n", frag_offset_err_cnt);
	index += sprintf(buf+index, "frag_dscp_empty_cnt = %d\n", frag_dscp_empty_cnt);
	index += sprintf(buf+index, "frag_dscp_null_cnt_1 = %d\n", frag_dscp_null_cnt_1);
	index += sprintf(buf+index, "frag_dscp_null_cnt_2 = %d\n", frag_dscp_null_cnt_2);
	index += sprintf(buf+index, "frag_fast enable = %d\n", frag_fast);

	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_frag_reassemble_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128];
	char cmd_string[16];
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

 	sscanf(val_string, "%15s", cmd_string);

	if (!strcmp(cmd_string, "clear")) 
	{
		frag_pkt_eth_err_cnt = 0;
		frag_pkt_loss_cnt_1 = 0;
		frag_pkt_loss_cnt_2 = 0;
		frag_id_err_cnt = 0;
		frag_offset_err_cnt = 0;
		frag_dscp_empty_cnt = 0;
		frag_dscp_null_cnt_1 = 0;
		frag_dscp_null_cnt_2 = 0;
	} 
	else if (!strcmp(cmd_string, "fast"))
	{
		frag_fast = 1;
	}
	else if (!strcmp(cmd_string, "slow"))
	{
		frag_fast = 0;
		if(skb_frag_tmp) {
			dev_kfree_skb_any(skb_frag_tmp);
		}
		if(skb6_frag_tmp) {
			dev_kfree_skb_any(skb6_frag_tmp);
		}
	}

	return count ;
}

/******************************************************************************
******************************************************************************/
int qdma_frag_reassemble_init(void)
{
#if SUPPORT_FRAGMENT_REASSEMBLE
    struct proc_dir_entry *qdma_proc=NULL;

	qdma_proc = create_proc_entry(qdma_frag_reassemble_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_frag_reassemble_read_proc ;
		qdma_proc->write_proc = qdma_frag_reassemble_write_proc ;
	}
#endif

    return 0;
}

void qdma_frag_reassemble_deinit(void)
{
#if SUPPORT_FRAGMENT_REASSEMBLE
	remove_proc_entry("qdma_lan/frag_assemble", NULL) ;
#endif
}

