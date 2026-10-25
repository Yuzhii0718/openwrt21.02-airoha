/*
 * Compatibility shim for building the reference-SDK qdma driver on top of the
 * en7563 kernel shipped in this tree.
 *
 * The driver sources come from a newer SDK than the kernel headers we have.
 * qdma_bmgr.c marks TR-471 speed test traffic with two PPE magics that the
 * newer include/linux/foe_hook.h defines as 0x729c and 0x729d.
 *
 * In this kernel 0x729b/0x729c are already assigned to
 * FOE_MAGIC_PPTP_VPN_UPSTREAM / FOE_MAGIC_PPTP_VPN_DOWNSTREAM, and the
 * still-closed hw_nat.ko was built with those values. Reusing 0x729c for TR-471
 * would make the two features collide, so park the TR-471 magics on the next
 * free values instead (0x729d/0x729e).
 *
 * Consequence: the closed hw_nat.ko does not know these magics, so TR-471
 * marked packets take the normal (non offloaded) path - the same behaviour as
 * before, since this kernel never supported TR-471.
 */

#ifndef FOE_MAGIC_TR471_TEST_UPSTREAM
#define FOE_MAGIC_TR471_TEST_UPSTREAM		0x729d
#endif

#ifndef FOE_MAGIC_TR471_HW_TEST_UPSTREAM
#define FOE_MAGIC_TR471_HW_TEST_UPSTREAM	0x729e
#endif

/*
 * The newer ecnt_hook_bbf247.h gives ECNT_API_BBF247_SET_DS_TXQ() a third
 * argument (QDMA_TxQosScheduler_T *txQos) and forwards it to the BBF247 hook
 * through data.para.txQos. This kernel's hook payload has no txQos member, and
 * the still-closed hw_nat.ko plus the in-tree bbf247 handler were built
 * against that payload, so the extra field cannot be added here.
 *
 * Pull the real declaration in first and then wrap the 3-argument call into the
 * 2-argument helper this kernel provides. The QoS pointer is simply not
 * forwarded: on this kernel the hook has no way to consume it, which matches
 * the behaviour before the replacement.
 */
#include <ecnt_hook/ecnt_hook_bbf247.h>

static inline int ECNT_API_BBF247_SET_DS_TXQ_2ARG(struct sk_buff *skb,
		unsigned int *txq, void *txQos)
{
	(void)txQos;
	return ECNT_API_BBF247_SET_DS_TXQ(skb, txq);
}

#define ECNT_API_BBF247_SET_DS_TXQ(skb, txq, txqos) \
	ECNT_API_BBF247_SET_DS_TXQ_2ARG(skb, txq, txqos)
