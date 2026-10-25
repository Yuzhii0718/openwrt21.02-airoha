#ifndef _RA_NAT_MULTICAST_H
#define _RA_NAT_MULTICAST_H
#include <ecnt_hook/ecnt_hook_ppe.h>

#define PPE_MULTICAST_HWNATENTRY_AGE_TIME 30

enum
{
	PPE_MULTICAST_HWNATENTRY_STATE_UNBIND=0,
	PPE_MULTICAST_HWNATENTRY_STATE_BINDED,
	PPE_MULTICAST_HWNATENTRY_STATE_DROP,
};

enum
{
	PPE_MULTICAST_FORWARD_STATE_LAN_ONLY = 0,
	PPE_MULTICAST_FORWARD_STATE_WLAN_ONLY,
	PPE_MULTICAST_FORWARD_STATE_LAN_WLAN,
	PPE_MULTICAST_FORWARD_STATE_LAN_XSI,
	PPE_MULTICAST_FORWARD_STATE_XSI_ONLY,
	PPE_MULTICAST_FORWARD_STATE_XSI_WLAN,
	PPE_MULTICAST_FORWARD_STATE_LAN_HSGMII_1toN,
	PPE_MULTICAST_FORWARD_STATE_UNKNOWN,
};

typedef struct
{
	struct list_head list;
	PPE_MULTICAST_INFO_t ppe_multicast_info;
	unsigned int foe_index;
	unsigned int port_mask;/*bit0-3 for eth0.1-eth0.4;bit16-23 for ra0-ra7;bit24-31 for rai0-rai7;other reserve*/
    unsigned int local;
	unsigned char state;
	struct timer_list age_timer;
}PPE_MULTICAST_HWNATENTRY_t;

typedef struct
{
	struct list_head list;
	PPE_MULTICAST_INFO_t ppe_multicast_info;
	unsigned int ref_cnt;
}PPE_MULTICAST_LOCALENTRY_t;

typedef struct
{
	unsigned int fwd_state;
	int (*handler_func)(unsigned int foe_index,unsigned int port_mask);
}PPE_MULTICAST_FWD_STATE_HANDLER;

#endif
