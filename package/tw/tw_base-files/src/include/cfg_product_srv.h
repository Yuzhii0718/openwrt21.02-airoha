/*******************************************************************************
 *        Copyright (c) 2017 TWin Advanced Technology Corp., Ltd.
 *        All rights reserved.
 *
 *       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
 *       PROPRIETARY AND TRADESECRET INFORMATION OF T&W INCORPORATED.
 *       ACCESS TO THIS WORK IS RESTRICTED TO (I) T&W EMPLOYEES WHO HAVE A
 *       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
 *       AND (II) ENTITIES OTHER THAN T&W WHO HAVE ENTERED INTO APPROPRIATE
 *       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
 *       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
 *       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 *       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF T&W.
 *       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
 *       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
 ******************************************************************************/
 /**
 * @brief The general define for the service
 */
#ifndef __CFG_PRODUCT_SRV_H__
#define __CFG_PRODUCT_SRV_H__

/*>>>>>-----------------------------------------------------------------<<<<<*/
#define TYPE_SRV_EXTRA \
    S(SRV_TOKEN) \
    S(SRV_VNT) \
// Service Enum
typedef enum {
    SRV_DUMMY   = 0,
    #define S(srv)  srv,
    TYPE_SRV_EXTRA
    #undef  S
    SRV_MAX
} TYPE_SRV;
/*>>>>>-----------------------------------------------------------------<<<<<*/

/*>>>>>-----------------------------------------------------------------<<<<<*/
// TODO(SOMEONE): Please add definition of your sysifd module here
typedef enum {
    MID_NO_ID   =   SRV_MAX-1,
    MID_LOG_ID,
    MID_LAN_ID,
    MID_HW_NAT_ID,
    MID_NET_ID,
    MID_DHCPD_ID,
    MID_IPV6_PASSTHROUGH_ID,
    MID_WAN_ID,
    MID_IPV6_ID,
    MID_IPV6_ROUTER_ID,
    MID_QOS_TC_ID,
    MID_IPTABLES_ID,
    MID_UWCD_ID,
    MID_DNSPROXY_ID,
    MID_VEN_APPS_ID,
    MID_UPNPD_ID,
    MID_IGMPPROXY_ID,
    MID_TELNETD_ID,
    MID_HTTPD_ID,
	MID_HIDDEN_PAGE_ID,
    MID_DDNS_ID,
    MID_SYSHOTPLUGD_ID,
    MID_SCHED_ID,
    MID_LOG_SCHED_ID,
    MID_IPT_PORT_FORWARDING_SCHED_ID,
    MID_IPT_MAC_FILTER_SCHED_ID,
    MID_IPT_IPV4_FIREWALL_SCHED_ID,
    MID_IPT_IPV6_FIREWALL_SCHED_ID,
    MID_IPT_VIRTUAL_SERVER_SCHED_ID,
    MID_WLAN_SCHED_ID,
    MID_TRENDMICRO_ID,
	MID_TRENDMICRO_DBUPG_ID,
    MID_CUSTOMER_APPS_ID,
    MID_FWUPG_ID,
    MID_FW_UPG_ONLINE_ID,
    MID_REBOOT_ID,
    MID_SMART_ANTENNA_ID,
    MID_DT_ID,
    MID_CC_ID,
    MID_HOSTNAME_ID,
    MID_LLTD_ID,
    MID_LLMNR_ID,
    MID_MDNS_ID,
    MID_HNAT_ID,
    MID_HNAP_AGENTD_ID,
    MID_ACS_ID,
    MID_SAMBA_ID,
    MID_TQ_ID,
    MID_USB_ID,
	MID_LTE_ID,
    MID_VPN_ID,
	MID_HTTPD_GUEST_ID,
	MID_VOICE_ID,
	MID_MAX_ID
} MID_ID;

#define MID(idx)                (1LL << (idx))
#define MID_IS_SET(s,idx)       (s & MID(idx))
/*>>>>>-----------------------------------------------------------------<<<<<*/

/*>>>>>-----------------------------------------------------------------<<<<<*/
// NOTICE: if you want to change the definitions
//          , use OPMODE_STYLE to define the OP groups for backward compability
//#define __DEFINE_TYPE_OP__
typedef enum {
    OP_NA       = 0x00,
    OP_AR       = 0x01, /* DO NOT change the bit definition for backward compability */
    OP_AP       = 0x02, /* DO NOT change the bit definition for backward compability */
    OP_UR       = 0x04, /* DO NOT change the bit definition for backward compability */
    OP_INIT     = 0x08, /* DO NOT change the bit definition for backward compability */
    OP_CL       = 0x10, /* DO NOT change the bit definition for backward compability */
    OP_FACTORY  = 0x80, /* DO NOT change the bit definition for backward compability */
    OP_ARAPUR   = 0x07,
    OP_URCL     = 0x14,
    OP_ALL      = 0xFF,
    OP_MAX
} TYPE_OP;

/*>>>>>-----------------------------------------------------------------<<<<<*/

typedef enum {
    STA_TYPE_LAN = 0,
    STA_TYPE_WIFI_24G,
    STA_TYPE_WIFI_5G,
    STA_TYPE_WIFI_5G_2,
    STA_TYPE_WIFI_24G_GUEST,
    STA_TYPE_WIFI_5G_GUEST,
    STA_TYPE_WIFI_24G_RE,
    STA_TYPE_WIFI_5G_RE,
} STA_TYPE;

#if IS_ENABLED(CFG_IS_SUPPORT_VLAN)
typedef enum{
                VLAN_INTERNET = 0,
                VLAN_IPTV,
                VLAN_VOIP,
                VLAN_BRIDGE
} TYPE_VLAN_SERVICE_MAPPING;
#endif

#endif /* __CFG_PRODUCT_SRV_H__ */
