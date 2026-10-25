#ifndef _LINUX_ECNT_XFRM_H
#define _LINUX_ECNT_XFRM_H

#include <linux/skbuff.h>
#include <ecnt_hook/ecnt_hook.h>

static inline int ecnt_xfrm_policy_check_inline_hook(struct sk_buff *skb)
{
	if(skb->ipsec_pt_flag & L2TP_OFFLOAD_PACKET)
		return ECNT_RETURN;

    return ECNT_CONTINUE;
}

#endif /* _XFRM_XFRM_H */
