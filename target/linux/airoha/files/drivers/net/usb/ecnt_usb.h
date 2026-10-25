#ifndef _LINUX_ECNT_USB_H
#define _LINUX_ECNT_USB_H

#include <linux/skbuff.h>
#include <ecnt_hook/ecnt_hook.h>
#include <linux/foe_hook.h>
#include <asm/tc3162/tc3162.h>

extern int (*ra_sw_nat_hook_rx) (struct sk_buff * skb);
extern int (*ra_sw_nat_hook_set_magic) (struct sk_buff * skb, int magic);
extern int (*ra_sw_nat_hook_tx) (struct sk_buff * skb, struct port_info * pinfo, int magic);

static inline int ecnt_usbnet_skb_return_inline_hook(struct sk_buff *skb)
{
#ifdef TCSUPPORT_RA_HWNAT
	if (ra_sw_nat_hook_set_magic)  
			ra_sw_nat_hook_set_magic(skb, FOE_MAGIC_USBNET);
		
	if (ra_sw_nat_hook_rx != NULL) {
		if (ra_sw_nat_hook_rx(skb) == 0) 
			return ECNT_RETURN;
	}
#endif /*add for usb dongle*/

	return ECNT_CONTINUE;
}

static inline int ecnt_usbnet_start_xmit_inline_hook(struct sk_buff *skb)
{
	struct port_info pinfo;

#ifdef TCSUPPORT_RA_HWNAT
	if(ra_sw_nat_hook_tx)
	{
		memset(&pinfo,0,sizeof(struct port_info));
		if (isEN7580 || isEN7581)
		{
			pinfo.nbq=1;
			pinfo.tsid=0x7f;
		}
		if(0 == ra_sw_nat_hook_tx(skb, &pinfo, FOE_MAGIC_USBNET))
			return ECNT_RETURN_DROP;
		
	}
#endif
    return ECNT_CONTINUE;
}

#endif /* _XFRM_USB_H */
