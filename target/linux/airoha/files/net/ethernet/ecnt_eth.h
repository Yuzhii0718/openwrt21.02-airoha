#ifndef _LINUX_ECNT_ETH_H
#define _LINUX_ECNT_ETH_H

#include <ecnt_hook/ecnt_hook.h>

static inline int ecnt_eth_change_mtu_inline_hook
	(struct net_device *dev, int new_mtu)

{
#ifdef TCSUPPORT_MAX_PACKET_2000
	if (new_mtu < 68 || new_mtu > 2000)
		return ECNT_CONTINUE;
	dev->mtu = new_mtu;
	return ECNT_RETURN;
#endif
	return ECNT_CONTINUE;
}

static inline int ecnt_ether_setup_inline_hook
	(struct net_device *dev)

{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#ifdef TCSUPPORT_MAX_PACKET_2000
	dev->max_mtu		= 2000;
#endif
#endif
	return ECNT_CONTINUE;
}

#endif

