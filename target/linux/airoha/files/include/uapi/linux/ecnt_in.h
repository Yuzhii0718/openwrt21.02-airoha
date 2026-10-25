#ifndef _LINUX_ECNT_IN_H
#define _LINUX_ECNT_IN_H

#include <linux/version.h>

/*
#define IP_MINTTL       21
#define IP_NODEFRAG     22
*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,9,263)	
#define	IP_CUSTOM_DEFINE_START  25
#elif LINUX_VERSION_CODE > KERNEL_VERSION(3,19,8)	
#define	IP_CUSTOM_DEFINE_START  24 
#else
#define	IP_CUSTOM_DEFINE_START  22 
#endif
#define	IP_SKB_MARK_FLAG	IP_CUSTOM_DEFINE_START+1
#define	IP_SKB_VLAN_ID_FLAG	IP_SKB_MARK_FLAG+1



/*
#define SO_DETACH_REUSEPORT_BPF 68
*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,9,263)	
#define	SO_TYPE_CUSTOM_DEFINE_START		69
#elif LINUX_VERSION_CODE > KERNEL_VERSION(3,19,8)	
#define	SO_TYPE_CUSTOM_DEFINE_START		51
#else
#define	SO_TYPE_CUSTOM_DEFINE_START		49
#endif

/* defined in kernel socket.h*/
#define SO_TYPE_RESERVED_TRAFFIC		(SO_TYPE_CUSTOM_DEFINE_START)
#define SO_TYPE_RESERVED_COPY_SKB_MARK	(SO_TYPE_RESERVED_TRAFFIC + 1)
/* define for all kernel. */
#define SO_TYPE_COPY_SRC_MAC			(SO_TYPE_RESERVED_COPY_SKB_MARK + 1)
#define SO_TYPE_SRC_MAC					(SO_TYPE_COPY_SRC_MAC + 1)


/*
#define IP_MULTICAST_ALL		49
#define IP_UNICAST_IF			50
*/
#define	IP_SKB_MARK			51
#define	IP_SKB_VLAN_ID		52

#define IP_SKB_PBIT 		54
#define NF_DSCP_DEF			0xFF

/* interface type */
#define 	IF_TYPE_WAN_ROUTE			(1<<0)
#define		IF_TYPE_WAN_BRIDE			(1<<1)
#define		IF_TYPE_LAN					(1<<2)
#define		IF_TYPE_INTERNET			(1<<3)
#define		IF_TYPE_OTHER				(1<<4)
#define 	IF_TYPE_HAS_REGISTER		(1<<5)
#define 	IF_TYPE_LAN_BIND_INTERNET	(1<<6)
#define 	IF_TYPE_OTHER_WAN_BRIDE		(IF_TYPE_WAN_BRIDE | IF_TYPE_OTHER)

#define 	MAX_PVC_NUM 			8
#define		MAX_SMUX_NUM			8

#define		VBIND_INVALID_VLANID	4096
#define		VBIND_DNSPORT			53

#if defined(TCSUPPORT_TEST_SAMBA_SHORTCUT)
#define 	SAMBA_SHORTCUT_PATH "tc3162/samba_shortcut"
#endif
#define 	APP_SHORTCUT_PATH    "tc3162/app_shortcut"
#define 	APP_SHORTCUT_MAX_NUM 8

#if defined(TCSUPPORT_CT_JOYME4)
#define 	CAPABLE_USER_ROOT_SWITCH_PATH  "tc3162/capable_user_root_switch"
#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#define 	WIFI_CHIP_PATH "tc3162/wifi_chip_info"
#define 	WIFI_CHIP_PATH_BAND0 "tc3162/wifi_chip_2g"
#define 	WIFI_CHIP_PATH_BAND1 "tc3162/wifi_chip_5g"
#endif
#define 	DBG_MSG_PATH "tc3162/dbg_msg"

#endif /* _LINUX_ECNT_IN_H */
