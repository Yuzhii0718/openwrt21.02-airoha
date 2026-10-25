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
 * @file cfg_product_variables.h
 * @brief The general define for the system
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */
#ifndef __CFG_PRODUCT_VARIABLES_H__
#define __CFG_PRODUCT_VARIABLES_H__

#include "x_ostypes.h"

#define _FUNCTION_ENTRY(fmt, arg...)        S_DBG("[ENTRY] " fmt, ##arg);

#if 1   /* Shell Commands */
    #define CMD_ARP                             "arp"
    #define CMD_ARPING                          "arping"
    #define CMD_AWK                             "awk"
    #define CMD_BRCTL                           "brctl"
    #define CMD_BTMW_TEST                       "btmw-test"	
    #define CMD_CAT                             "cat"
    #define CMD_CHARON                          "charon"
    #define CMD_CHECK_CWMPD                     "check_cwmpd.sh"
    #define CMD_CHECK_DLMAD                     "dlma_checker.sh"
    #define CMD_CHMOD                           "chmod"
    #define CMD_CLI_FACTORY                     "cli_factory"
    #define CMD_CP                              "cp"
    #define CMD_CLI_GENERATED_SSL_FILE          "cli_generated_ssl_file"
#if IS_ENABLED(CFG_IS_PKG_CONNECTED_CLIENT)
    #define CMD_CONNECTED_CLIENT                "cc"
#endif    
    #define CMD_CROND                           "crond"
    #define CMD_CRONTAB                         "crontab"
    #define CMD_CURL                            "curl"
    #define CMD_CUT                             "cut"
    #define CMD_CWMPD                           "cwmpd"
    #define CMD_DATE                            "date"
    #define CMD_DATE_STR                        "date \"+%%Y/%%m/%%d %%T GMT%%z\"" // 2019/12/25 18:26:45 GMT+0800
    #define CMD_DD                              "dd"
    #define CMD_DHCP6                           "dhcp6-multi"
    #define CMD_DHCPC6                          CMD_DHCP6 " dhcp6c"
    #define CMD_DHCPS6                          CMD_DHCP6 " dhcp6s"
    #define CMD_DHCPC                           "udhcpc"
    #define CMD_DHCPD                           "udhcpd"
    #define CMD_DINIUTIL                        "diniutil"
    #define CMD_DJSONUTIL                       "djsonutil"
    #define CMD_DLMAD                           "dlmad"
    #define CMD_DMESG                           "dmesg"
    #define CMD_DNSMASQ                         "dnsmasq"
    #define CMD_DTA_UPGRADE                     "dta_upgrade"
    #define CMD_DUMPLEASES                      "dumpleases"
    #define CMD_EBTABLES                        "ebtables"
    #define CMD_ECHO                            "echo"
    #define CMD_EZ_IPUPDATE                     "ez-ipupdate"
    #define CMD_ETHTOOL                         "ethtool"
    #define CMD_FLASHCP                         "flashcp"
    #define CMD_FLASHERASE                      "flash_erase"
    #define CMD_FWHANDLE                        "fwhandle"
    #define CMD_FWUPGRADE                       "fwupgrade"
    #define CMD_GREP                            "grep"
    #define CMD_HOSTAPD_CLI                     "hostapd_cli"
    #define CMD_IFCONFIG                        "ifconfig"
    #define CMD_INADYN                          "inadyn"
    #define CMD_INSMOD                          "insmod"
    #define CMD_IP                              "ip"
    #define CMD_IPTABLES                        "iptables"
    #define CMD_IP6TABLES                       "ip6tables"
    #define CMD_IPSEC                           "ipsec"
    #define CMD_IWCONFIG                        "iwconfig"
    #define CMD_IWLIST                          "iwlist"
    #define CMD_IWPRIV                          "iwpriv"
    #define CMD_KLOGD                           "klogd"
    #define CMD_KILL                            "kill"
    #define CMD_KILLALL                         "killall"
    #define CMD_LEDEVENT                        "ledevent"
    #define CMD_LLMNRD                          "llmnrd"
    #define CMD_LN                              "ln"
    #define CMD_LSMOD                           "lsmod"
    #define CMD_LOSETUP                         "losetup"
    #define CMD_MD5SUM                          "md5sum"
    #define CMD_MKDIR                           "mkdir"
    #define CMD_MKNOD                           "mknod"
    #define CMD_MOUNT                           "mount"
    #define CMD_NMBLOOKUP                       "nmblookup"
    #define CMD_UMOUNT                          "umount"
    #define CMD_MV                              "mv"
    #define CMD_NDISC6                          "ndisc6"
    #define CMD_NDPPD                           "ndppd"
    #define CMD_NETSTAT                         "netstat"
    #define CMD_NGINX                           "nginx"
    #define CMD_NSLOOKUP                        "nslookup"
    #define CMD_NTPCLIENT                       "ntpclient"
#if IS_ENABLED(CFG_IS_3RD_OOKLA)
    #define CMD_OOKLA                            "ookla"
#endif
    #define CMD_OPENSSL                         "openssl"
    #define CMD_PING                            "ping"
    #define CMD_PPPD                            "pppd"
    #define CMD_PPPDV4                          "pppdv4"
    #define CMD_PPPDV6                          "pppdv6"
    #define CMD_PPPOE_RELAY                     "pppoe-relay"
    #define CMD_PPTP                            "pptp"
    #define CMD_PS                              "ps"
    #define CMD_PIDOF                           "pidof"
    #define CMD_OPENL2TP                        "openl2tpd"
    #define CMD_RADVD                           "radvd"
    #define CMD_REBOOT                          "reboot"
    #define CMD_RENICE                          "renice"
    #define CMD_RNGD                            "rngd"
    #define CMD_RDISC6                          "rdisc6"
    #define CMD_RM                              "rm"
    #define CMD_RMMOD                           "rmmod"
    #define CMD_ROUTE                           "route"
    #define CMD_SED                             "sed"
    #define CMD_SENDMAIL                        "sendmail"
    #define CMD_SH                              "sh"
    #define CMD_SLEEP                           "sleep"
    #define CMD_SPAWN_FCGI                      "spawn-fcgi"
    #define CMD_SSDK_SH                         "ssdk_sh"
    #define CMD_SYNCSYNCSYNC                    "sync;sync;sync;"
    #define CMD_SYSCTL                          "sysctl"
    #define CMD_SYSHOTPLUGD                     "syshotplugd"
    #define CMD_SYSIFD                          "sysifd"
    #define CMD_SYSIFDL                         "sysifdl"
    #define CMD_SYSLEDD                         "sysledd"
    #define CMD_SYSLOGD                         "syslogd"
    #define CMD_STARTER                         "starter"
    #define CMD_STATE_CFG                       "state_cfg"
    #define CMD_STATE_GO                        "state_go"
    #define CMD_SWCONFIG                        "swconfig"
    #define CMD_SWITCH                          "switch"
    #define CMD_TAIL                            "tail"
    #define CMD_TAR                             "tar"
    #define CMD_TC                              "tc"
    #define CMD_TELNETD                         "telnetd"
    #define CMD_TFTP                            "tftp"
    #define CMD_THTTPD                          "thttpd"
    #define CMD_TOKEN_INIT                      "tok_init"
    #define CMD_TOUCH                           "touch"
    #define CMD_TR                              "tr"
    #define CMD_VCONFIG                         "vconfig"
    #define CMD_WC                              "wc"
    #define CMD_WGET                            "wget"
    #define CMD_WLANCONFIG                      "wlanconfig"
    #define CMD_WPA_CLI                         "wpa_cli"
    #define CMD_ZIC                             "zic"
    #define CMD_PRINTF                          "printf"
    #define CMD_HNAPAGENTD                      "agentd"
 #if IS_ENABLED(CFG_IS_PKG_HNAP_AGENT)//TODO(S), set flag for MGT_WEB_P_HNAP_HNAPAGENT_SUPPORT
    #define CMD_HNAPAGENT                       "hnapagent"
 #endif
 #if IS_ENABLED(CFG_IS_PKG_HNAP_AGENT)
    #define CMD_AGENT                           "hnapagent"
 #else // CMD_AGENT is currently only used by hnap
    #define CMD_AGENT                           "enapagent"
 #endif
 #if IS_ENABLED(CFG_IS_PKG_CWMPC)
    #define CMD_CWMPC                           "cwmpc"
 #endif
    #define CMD_XL2TPD                          "xl2tpd"
    #define CMD_MKE2FS                          "mke2fs"
    #define CMD_TUNE2FS                         "tune2fs"
    #define CMD_FATLABEL                        "fatlabel"
    #define CMD_MKFS_VFAT                       "mkfs.vfat"
    #define CMD_LSBLK                           "lsblk"
    #define CMD_HD_IDLE                         "hd-idle"
    #define CMD_FUSER                           "fuser"
    #define CMD_WAN_AUTODETECT                  "wan_autodetect"
#if IS_ENABLED(CFG_IS_PKG_BNDSTRG_PLUS)
    #define CMD_BNDSTRG                         "bndstrg2"
#endif
    #define CMD_UNZIP                           "unzip"
#endif
#if IS_ENABLED(CFG_IS_PKG_LINK_DETECTOR)
    #define CMD_LINK_DETECTOR                   "link_detector"
#endif

#define FILE_ETC_NAMES                      "/etc/hosts"
#define FILE_ETC_HOSTNAME                   "/etc/hostname"
#define FILE_ETC_RESOLV_CONF                "/etc/resolv.conf"

// TODO(S): refactory the /etc/resolv.conf
#if IS_ENABLED(CFG_IS_IPV6)
#define RESOLV_IPV6_CONF_PATH               "/etc/resolv_ipv6.conf"
#endif
#define RESOLV_WAN_CONF_PATH                "/etc/resolv_wan.conf"
#define FILE_ETC_RESOLV_LAN                 "/etc/resolv_lan.conf"
#define FILE_DNS_PPP_RESOLV_CONF            "/etc/ppp/resolv.conf"

#define FILE_CRONTAB_ROOT                   "/var/spool/cron/crontabs/root"
#define FILE_DNSMASQ_PID                    "/var/run/dnsmasq.pid"
#define FILE_DNSMASQ_CONF                   "/etc/dnsmasq.conf"
#define FILE_PING_RESULT                    "/tmp/ping"
#define FILE_DHCPC_PID                      "/var/run/udhcpc.pid"
#define FILE_TMP_DHCPC_PID                  "/var/run/udhcpc%d.pid"
#define FILE_DHCPD_CONF                     "/etc/udhcpd.conf"
#define FILE_DHCPD_PID                      "/var/run/udhcpd.pid"
#define FILE_DHCPD_LEASES                   "/etc/udhcpd.leases"
#if IS_ENABLED(CFG_SUPPORT_DHCPD_LEASE_SAVE_IN_STORAGE)
    #define FILE_DHCPD_LEASES_SAVED             "/storage/udhcpd.leases"
#endif
#define FILE_DHCPD_LEASES_CACHE             "/tmp/dhcp.cache"
#define FILE_DNS_SUFFIX                     "/tmp/dns.suffix"
#if IS_ENABLED(CFG_IS_SUPPORT_PPPOE) || IS_ENABLED(CFG_IS_SUPPORT_PPTP)
#define FILE_PPP_PID                        "/var/run/ppp%d.pid"
#endif
#if IS_ENABLED(CFG_IS_SUPPORT_L2TP)
#if IS_ENABLED(CFG_IS_PKG_OPENL2TP)
#define FILE_L2TP_PID                       "/var/run/openl2tpd.pid"
#endif
#if IS_ENABLED(CFG_IS_PKG_XL2TPD)
#define FILE_L2TP_PID                       "/var/run/xl2tpd.pid"
#endif
#endif
#define FILE_WFADEVDESC                     "/etc/WFADeviceDesc.xml"
#define FILE_UWCD_WSC_UPNP_SH_CLOSE         "/tmp/uwcd_wsc_upnp_close.sh"
#define FILE_WBOX_GUEST_SH_OPEN             "/tmp/wbox_guest_open.sh"

#define TMP_FILE_WBOX_GUEST_SH_OPEN         "/tmp/wbox_guest%d_open.sh"
#define TMP_FILE_WBOX_GUEST_SH_CLOSE        "/tmp/wbox_guest%d_close.sh"

#if IS_ENABLED(CFG_WLAN_GUEST_COMBINE)
#define CMD_DHCPD_GUEST                     "udhcpd_guest"
#define FILE_DHCPD_CONF_GUEST               "/etc/udhcpd_guest.conf"
#define PATH_DHCPD_DUMPLEASES_GUEST         "/etc/udhcpd_guest.leases"
#define FILE_DHCPD_PID_GUEST                "/var/run/udhcpd_guest.pid"
#endif

#if IS_ENABLED(CFG_WLAN_GUEST_SEPARATE)
#if IS_ENABLED(CFG_RADIO1_IS_WLAN_GUEST)
#define CMD_DHCPD_GUEST_R1                  "udhcpd_guest_r1"
#define FILE_DHCPD_CONF_GUEST_R1            "/etc/udhcpd_guest_r1.conf"
#define PATH_DHCPD_DUMPLEASES_GUEST_R1      "/etc/udhcpd_guest_r1.leases"
#define FILE_DHCPD_LEASES_CACHE_GUEST_R1   "/tmp/dhcp_guest_r1.cache"
#define FILE_DHCPD_PID_GUEST_R1             "/var/run/udhcpd_guest_r1.pid"
#endif

#if IS_ENABLED(CFG_RADIO2_IS_WLAN_GUEST)
#define CMD_DHCPD_GUEST_R2                  "udhcpd_guest_r2"
#define FILE_DHCPD_CONF_GUEST_R2            "/etc/udhcpd_guest_r2.conf"
#define PATH_DHCPD_DUMPLEASES_GUEST_R2      "/etc/udhcpd_guest_r2.leases"
#define FILE_DHCPD_LEASES_CACHE_GUEST_R2   "/tmp/dhcp_guest_r2.cache"
#define FILE_DHCPD_PID_GUEST_R2             "/var/run/udhcpd_guest_r2.pid"
#endif
#endif

#define CMD_WSCD                            "wscd"
#define FILE_WSCD_DEFAULT_PID_FILE_PATH     "/var/run/wscd.pid"

#if IS_ENABLED(CFG_IS_PKG_QCA_HYD)
#define CMD_HYD                             "hyd"
#define CMD_HYCTL                           "hyctl"
#define FILE_HYD_CMD_OPEN                   "/tmp/hyd_cmd_open.sh"
#define FILE_HYD_CMD_CLOSE                  "/tmp/hyd_cmd_close.sh"
#define FILE_HYD_CONF                       "/tmp/hyd-lan.conf"
#define FILE_HYD_LOCK                       "/var/run/hyd.lock"
#endif
#if IS_ENABLED(CFG_IS_PKG_QCA_WSPLCD)
#define CMD_WSPLCD                          "wsplcd"
#define FILE_WSPLCD_CONF                    "/tmp/wsplcd-lan.conf"
#define FILE_WSPLCD_LOCK                    "/var/run/wsplcd.lock"
#endif
#if IS_ENABLED(CFG_IS_SUPPORT_BANDSTEERING)
#define CMD_LBD                             "lbd"
#define FILE_LBD_CONFIG                     "/tmp/lbd.conf"
#if IS_ENABLED(CFG_IS_PKG_BNDSTRG_PLUS)
#define FILE_BNDSTRG_CONFIG                 "/etc/bndstrg.conf"
#endif
#endif

#if IS_ENABLED(CFG_IS_PKG_SMART_ANTENNA)
#define CMD_SMART_ANTENNA                   "smart_antenna"
#endif

#if IS_ENABLED(CFG_IS_SUPPORT_UPNP)
#define CMD_UPNPD                           "miniupnpd"
#define FILE_UPNPD_PID_PATH                 "/var/run/miniupnpd.pid"
#define FILE_UPNPD_CONF_PATH                "/etc/miniupnpd.conf"
#define FILE_UPNPD_LEASES_PATH              "/var/log/upnp.leases"
#endif // IS_ENABLED(CFG_IS_SUPPORT_UPNP)

#if IS_ENABLED(CFG_IS_PKG_IGMPPROXY)
#define CMD_IGMPPROXY                       "igmpproxy"
#define PATH_IGMPPROXY_CONF                 "/etc/igmpproxy.conf"
#endif
#if IS_ENABLED(CFG_IS_PKG_MCPROXY)
#define CMD_MCPROXY                         "mcproxy"
#define PATH_MCPROXY_V4_CONF                "/etc/mcproxy_v4.conf"
#define PATH_MCPROXY_V6_CONF                "/etc/mcproxy_v6.conf"
#endif
#if IS_ENABLED(CFG_IS_PKG_OMCPROXY)
#define CMD_OMCPROXY                         "omcproxy"
#endif

#if IS_ENABLED(CFG_IS_SUPPORT_LLTD)
#define CMD_LLTD                            "lltd_daemon"
#endif

#if IS_ENABLED(CFG_IS_SUPPORT_MDNS)
#define CMD_MDNS                       		"mDNSResponder"
#define CMD_MDNS_RESPONDER_POSIX        		"mDNSResponderPosix"
#define FILE_MDNS_PID_PATH                 	"/var/run/mDNSResponder.pid"
#define FILE_MDNS_CONF_PATH                 "/etc/mdnsresponder.conf"
#endif

#if IS_ENABLED(CFG_IS_PKG_FCGIWRAP)
#define CMD_FCGIWRAP                        "fcgiwrap"
#endif

#if IS_ENABLED(CFG_IS_SAMBA)
#define CMD_SMBD                            "smbd"
#define CMD_NMBD                            "nmbd"
#define CMD_SMBPASSWD                       "smbpasswd"
#define FILE_SMBD_PID_PATH                  "/var/run/smbd-smb.conf.pid"
#define FILE_NMBD_PID_PATH                  "/var/run/nmbd.pid"
#define SAMBA_DIRECTORY                     "/etc/samba"
#define FILE_SMBD_CONF_PATH                 SAMBA_DIRECTORY"/smb.conf"
#define FILE_SMBD_SECRETS_PATH              SAMBA_DIRECTORY"/secrets.tdb"
#define FILE_SMBD_PASSWD_PATH               SAMBA_DIRECTORY"/smbpasswd"
#define CMD_ADDGROUP                        "addgroup"
#define CMD_DELGROUP                        "delgroup"
#define CMD_ADDUSER                         "adduser"
#define CMD_DELUSER                         "deluser"
#endif

#if IS_ENABLED(CFG_IS_IPTABLES_MAC_FILTER_TIME_QUOTA)
#define CMD_TQD                            "tqd"
#define FILE_TQD_PID_PATH                  "/var/run/tqd.pid"
#endif

#if IS_ENABLED(CFG_IS_SUPPORT_WLAN_GUEST_REDIRECT)
#define CMD_GUEST_WIFI_HTTPD                "httpd_guest"
#endif

#if IS_ENABLED(CFG_IS_3RD_TRENDMICRO)
#define DEVICE_LIST_FILE                    "/storage/device.user.list"
#endif
#if IS_ENABLED(CFG_IS_PKG_F_CONTROL)
#define CMD_F_CONTROL                       "f_control"
#endif
#if IS_ENABLED(CFG_IS_PKG_PROC_MANAGER)
#define CMD_PROCMANAGER                     "procmanager"
#endif
#define FILE_LINK_APPS_SH 	                "/tmp/link_apps.sh"
#define PATH_WEB_ROOT                       "/www/html"
#define PATH_CGI                            "/www/cgi-bin"
#define STATE_CFG_TEMP_FILE                 "/tmp/config.tmp"
#define FACTORY_TEMP_FILE                   "/tmp/Factory.tmp"

#if IS_ENABLED(CFG_SUPPORT_ETC_TZ)
#define FILE_TIMEZONE                       "/etc/TZ"
#elif IS_ENABLED(CFG_SUPPORT_ETC_LOCALTIME)
#define FILE_TIMEZONE                       "/etc/localtime"
#endif

#if IS_ENABLED(CFG_IS_PKG_PROSLIC)
#define CMD_UPBXD                           "upbxd"
#define FILE_PCMCLK_READY_PATH              "/tmp/pcmclk_ready"
#endif

#define FILE_FSTAB                          "/etc/fstab"

#define VALUE(x)                            x
#define STR_HELPER(x)                       #x
#define STR(x)                              STR_HELPER(x)


// TODO(SOMEONE): refactory the SYS_EVENT
// TODO(SOMEONE): change the SYS_EVENT to enum, and use the define of SYSIFD_CLI

#define SYSIFD_FW_UPG                    "fw_upg"
#define SYSIFD_UPDATE_FLASHCP_PROGRESS   "update_flashcp_progress"
#define SYSIFD_TOKEN_SAVE                "token_save"
#if IS_ENABLED(CFG_IS_FW_UPG_BY_URL)
    #define SYSIFD_FW_UPG_BY_URL         "fw_upg_by_url"
#endif
#define SYSIFD_TOKEN_RESTORE             "token_restore"
#define SYSIFD_READY_TO_RESET_DEFAULT    "token_ready_to_reset_default"
#define SYSIFD_TOKEN_RESET_DEFAULT       "token_reset_default"
#define SYSIFD_REBOOT                    "reboot"

#define SYSIFD_ETHERNET_LINK             "ethernet_link"
#define SYSIFD_INTERFACE_LINK            "inf_link"
#define SYSIFD_USB_STORAGE               "usb_storage"
#define SYSIFD_WAN_IP_OBTAINED           "wan_ip_obtained"
#if IS_ENABLED(CFG_IS_SUPPORT_PPPOE) || IS_ENABLED(CFG_IS_SUPPORT_PPTP) || IS_ENABLED(CFG_IS_SUPPORT_L2TP)
#define SYSIFD_WAN_PPP_IFUP              "wan_ppp_ifup"
#define SYSIFD_WAN_PPP_IP_OBTAINED       "wan_ppp_ip_obtained"
#define SYSIFD_WAN_PPP_ONDEMAND_TIMEOUT  "wan_ppp_ondemand_timeout"
#define SYSIFD_WAN_PPP_ONDEMAND_CONNECTED  "wan_ppp_ondemand_connected"
#define SYSIFD_WAN_PPP_ONDEMAND_CONNECT_FAIL "wan_ppp_ondemand_connect_fail"
#endif
#define SYSIFD_LAN_IP_OBTAINED           "lan_ip_obtained"
#define SYSIFD_SET_DEFAULT_GATEWAY       "set_default_gateway"
#define SYSIFD_DDNS_RESULT_OK            "ddns_result_ok"
#define SYSIFD_DDNS_RESULT_FAIL          "ddns_result_fail"
#define SYSIFD_SW_MODULE_FAIL            "failed"
#define SYSIFD_READY_TO_TRIGGER_WPS      "wlan_ready_to_trigger_wps"
#define SYSIFD_WLAN_TRIGGER_WPS          "wlan_trigger_wps"
#define SYSIFD_WPS_SUCCESS               "wlan_wps_success"
#define SYSIFD_WPS_TIMEOUT               "wlan_wps_timeout"
#if IS_ENABLED(CFG_WLAN_WPS_WIFI_CLONE)
#define SYSIFD_WLAN_TRIGGER_WIFI_CLONE          "wlan_trigger_wifi_clone"
#endif
#define SYSIFD_ENABLE_CLI_FACTORY        "en_cli_factory"
#define SYSIFD_RELEASE_MEMORY            "release_memory"
#if IS_ENABLED(CFG_IS_IPV6)
#define SYSIFD_IPV6_RA_OBTAINED          "ipv6_ra_obtained"
#define SYSIFD_IPV6_DHCPC_OBTAINED       "ipv6_dhcpc_obtained"
#endif
#if IS_ENABLED(CFG_IS_IPV6_ROUTER)
#define SYSIFD_IPV6_DHCPC_PD_OBTAINED    "ipv6_dhcpc_pd_obtained"
#endif
#if IS_ENABLED(CFG_IS_IPV6_NOPREFIX_PASSTHROUGH)
#define SYSIFD_IPV6_DHCPC_PD_UNAVAILABLE "ipv6_dhcpc_pd_unavailable"
#endif

#if IS_ENABLED(CFG_IS_IPV6_6RD)
#define SYSIFD_IPV6_6RD_OBTAINED         "6rd_obtained"

#define SYSIFD_IPV6_6RD_OPTION_FILE             "/etc/dhcp_6rd_info.txt"
#define SYSIFD_IPV6_6RD_IPV4_MASK_LENGTH_KEY_WORD "IPv4MaskLen"
#define SYSIFD_IPV6_6RD_IPV6_PLENGTH_LENGTH_KEY_WORD "6rdPrefixLen"
#define SYSIFD_IPV6_6RD_IPV6_PREFIX_KEY_WORD    "6rdPrefix"
#define SYSIFD_IPV6_6RD_BOARDER_IP_KEY_WORD     "6rdBRIPv4Address"
#endif
#if IS_ENABLED(CFG_IS_IPV6_DSLITE)
#define SYSIFD_IPV6_DHCPC_AFTR_OBTAINED  "ipv6_dhcpc_aftr_obtained"
#endif
#if IS_ENABLED(CFG_IS_IPV6_PPPOE)
#define SYSIFD_IPV6_PPP_IPV6CPUP         "ipv6cp_up"
#define SYSIFD_IPV6_PPP_IPV6CPDOWN       "ipv6cp_down"
#endif
#if IS_ENABLED(CFG_IS_HW_NAT_ENABLE)
#define SYSIFD_HNAT_DISABLE             "disable_hnat"
#define SYSIFD_HNAT_ENABLE             "enable_hnat"
#endif
#if IS_ENABLED(CFG_IS_CUSTOMER_APPS_DLINK_MYDLINK)
#define SYSIFD_NETWORK_RESTART "network_restart"
#endif
#if IS_ENABLED(CFG_IS_SUPPORT_VPN)
#define SYSIFD_VPN_IP_OBTAINED           "vpn_ip_obtained"
#define SYSIFD_VPN_CLIENT_IP_OBTAINED    "vpn_clien_ip_obtained"
#endif
#if IS_ENABLED(CFG_IS_USB)
#define SYSIFD_USB_STORAGE               "usb_storage"
#define SYSIFD_USB_ACTION                "usb_action"
#endif

#if IS_ENABLED(CFG_IS_3RD_TRENDMICRO)
#define SYSIFD_TM_DB_UPG_TIME            "tm_db_upgrade_time"
#define SYSIFD_TM_DB_UPG                 "tm_db_upgrade"
#endif

#if IS_ENABLED(CFG_IS_ELECOM_EASY_SETUP_4)
#define SYSIFD_AUTO_WAN_RESULT           "auto_wan_result"
#endif

#define SYSLOGD_PID                                 "/var/run/" CMD_SYSLOGD ".pid"
#define SYSLOGD_CONF                                "/etc/syslogd.conf"

#if IS_ENABLED(CFG_IS_SUPPORT_PPPOE) || IS_ENABLED(CFG_IS_SUPPORT_PPTP) || IS_ENABLED(CFG_IS_SUPPORT_L2TP)
#define PPP_ONDEMAND_TIMEOUT                          "/tmp/ppp_ondemand_timeout"
#endif

#undef  SRV_STATE_INIT_NAME
#undef  SRV_STATE_NORMAL_NAME
#undef  SRV_STATE_RELOAD_NAME
#undef  SRV_STATE_UPGRADE_NAME
#undef  SRV_STATE_TO_REBOOT_NAME
#undef  SRV_STATE_REBOOT_NAME
#define SRV_STATE_INIT_NAME         "INIT"
#define SRV_STATE_NORMAL_NAME       "NORMAL"
#define SRV_STATE_RELOAD_NAME       "RELOAD"
#define SRV_STATE_UPGRADE_NAME      "UPGRADE"
#define SRV_STATE_TO_REBOOT_NAME    "TOREBOOT"
#define SRV_STATE_REBOOT_NAME       "REBOOT"

    /* Wifi Command From UWCD Used By The Packages Outside Sysifd */
#if IS_ENABLED(CFG_IS_SRV_WIFI)
    /** Radio (Control - debug used) *****/
    #define SYSIFD_WIFI_RELOAD_RADIO_FORCE           "reload_radio_f"
    #define SYSIFD_WIFI_APPLY_CHANGE                 "apply_wifi_change"
    /** MISC *****************************/
    #define SYSIFD_WIFI_PREINIT_RADIO                "preinit_radio"
    /** WLAN (Notify) ********************/
    #define SYSIFD_WIFI_NOTIFY_WLAN                  "notify_wlan"
#endif

#if IS_ENABLED(CFG_IS_WLAN_SCHED)
    #define SYSIFD_WLAN_SCHED_START              "wlan_sched_start"
    #define SYSIFD_WLAN_SCHED_END                "wlan_sched_end"
#endif

#if IS_ENABLED(CFG_IS_IPTABLES_PORT_FORWARDING_SCHED)
    #define SYSIFD_IPT_PORT_FORWARDING_SCHED_START  "ipt_port_forwarding_sched_start"
    #define SYSIFD_IPT_PORT_FORWARDING_SCHED_END    "ipt_port_forwarding_sched_end"
#endif

#if IS_ENABLED(CFG_IS_IPTABLES_MAC_FILTER_SCHED)
    #define SYSIFD_IPT_MAC_FILTER_SCHED_START     "ipt_mac_filter_sched_start"
    #define SYSIFD_IPT_MAC_FILTER_SCHED_END       "ipt_mac_filter_sched_end"
#endif

#if IS_ENABLED(CFG_IS_IPTABLES_IPV4_FIREWALL_SCHED)
    #define SYSIFD_IPT_IPV4_FIREWALL_SCHED_START     "ipt_ipv4_firewall_sched_start"
    #define SYSIFD_IPT_IPV4_FIREWALL_SCHED_END       "ipt_ipv4_firewall_sched_end"
#endif

#if IS_ENABLED(CFG_IS_IPTABLES_IPV6_FIREWALL_SCHED)
    #define SYSIFD_IPT_IPV6_FIREWALL_SCHED_START  "ipt_ipv6_firewall_sched_start"
    #define SYSIFD_IPT_IPV6_FIREWALL_SCHED_END    "ipt_ipv6_firewall_sched_end"
#endif

#if IS_ENABLED(CFG_IS_IPTABLES_VIRTUAL_SERVER_SCHED)
    #define SYSIFD_IPT_VIRTUAL_SERVER_SCHED_START    "ipt_virtual_server_sched_start"
    #define SYSIFD_IPT_VIRTUAL_SERVER_SCHED_END      "ipt_virtual_server_sched_end"
#endif

#define FW_WITH_HEADER_NAME                     "fw_with_header"
#define FW_WITH_HEADER                          "/tmp/"FW_WITH_HEADER_NAME
#define FW_WITHOUT_HEADER                       FW_WITH_HEADER".bin"

#if IS_ENABLED(CFG_IS_FW_UPG_BY_URL)
#define LATEST_FW_INFO_TMP                      "/tmp/fw_info_tmp"
#define LATEST_FW_INFO                          "/tmp/fw_info"
#if IS_ENABLED(CFG_IS_FW_UPG_URL_BY_ELECOM1701)
#define ENC_FW                                  "/tmp/fw_enc"
#define ENC_COMMONKEY                           "/tmp/common_key_enc"
#define DEC_COMMONKEY                           "/tmp/common_key_dec"
#define SIGNEDFILE                              "/tmp/signed_file"
#define PUBLICKEY                               "/etc/public.pem"
#endif
#if IS_ENABLED(CFG_IS_FW_UPG_URL_BY_DLINK1801) || IS_ENABLED(CFG_IS_DATETIME_TZDATA_BY_REMOTE)
#define REFRESH_TOKEN_DAT                       "/storage/REFRESH.dat"
#endif
#endif

#define NODE_EXCHANGE_PATH
#define NODE_EXCHANGE_PROC_PATH
#define NODE_EXCHANGE_REGISTER(x)
#define NODE_EXCHANGE_UNREGISTER(x)
#define NODE_EXCHANGE_WRITE(name, fmt, value)
#define SYSTEM_EXCHANGE_WRITE(name, value)

#if IS_ENABLED(CFG_IS_NODE_EXCHANGE)
    #undef  NODE_EXCHANGE_PATH
    #undef  NODE_EXCHANGE_PROC_PATH
    #undef  NODE_EXCHANGE_REGISTER
    #undef  NODE_EXCHANGE_UNREGISTER
    #undef  NODE_EXCHANGE_WRITE
    #undef  SYSTEM_EXCHANGE_WRITE
    #define NODE_EXCHANGE_NAME                      "cli_exchange"
#if 0	
    #define NODE_EXCHANGE_PATH                      "/" NODE_EXCHANGE_NAME "/"
    #define NODE_EXCHANGE_PROC_PATH                 "/proc/" NODE_EXCHANGE_NAME "/"
    // TODO(SOMEONE): change to use API FILE read/write, instead of using the shell echo
    #define NODE_EXCHANGE_REGISTER(x)               CMD(FD_NONE, CMD_ECHO " add " STR(x) " > " NODE_EXCHANGE_PATH NODE_EXCHANGE_REGISTER_NAME)
    #define NODE_EXCHANGE_UNREGISTER(x)             CMD(FD_NONE, CMD_ECHO " del " STR(x) " > " NODE_EXCHANGE_PATH NODE_EXCHANGE_REGISTER_NAME)
#else	
	#define NODE_EXCHANGE_PATH						"/var/run/" NODE_EXCHANGE_NAME "/"
	#define NODE_EXCHANGE_PROC_PATH 				"/proc/" NODE_EXCHANGE_NAME "/"
	// TODO(SOMEONE): change to use API FILE read/write, instead of using the shell echo
	#define NODE_EXCHANGE_REGISTER(x)				CMD(FD_NONE, CMD_TOUCH " " NODE_EXCHANGE_PATH STR(x)";"CMD_CHMOD " 666 " NODE_EXCHANGE_PATH STR(x))
	#define NODE_EXCHANGE_UNREGISTER(x) 			CMD(FD_NONE, CMD_RM " -f " NODE_EXCHANGE_PATH"/" STR(x))
#endif
    #define NODE_EXCHANGE_WRITE(name, fmt, value)   CMD(FD_NONE, CMD_ECHO " " fmt " > " NODE_EXCHANGE_PATH STR(name), value)
    #define SYSTEM_EXCHANGE_WRITE(name, value)      system(CMD_ECHO " " STR(value) " > " NODE_EXCHANGE_PATH STR(name))
#endif

#define NODE_EXCHANGE_REGISTER_NAME             "reg"
#define NODE_EXCHANGE_STATE_IF                  "state"
#define NODE_EXCHANGE_OPMODE                    "opmode"
#if IS_ENABLED(CFG_IS_SRV_LOG2EMAIL)
    #define NODE_EXCHANGE_SEND_MAIL_STATUS      "send_log_mail_status"
#endif
#define NODE_EXCHANGE_PROGRESS_FLASH            "flashcp_progress"
#define NODE_EXCHANGE_PRE_STATICS               "pre_statistics"
#define NODE_EXCHANGE_READY_PERCENTAGE          "ready_percentage"
#define NODE_EXCHANGE_DOWNLOAD_PERCENTAGE       "download_percentage"
#define NODE_EXCHANGE_RELOAD_TIME               "reload_time"
#define NODE_EXCHANGE_ERRNO                     "errno"
#define NODE_EXCHANGE_PRE_IP                    "pre_ip"
#define NODE_EXCHANGE_PRE_DOMAIN                "pre_domain"
#if IS_ENABLED(CFG_IS_SRV_VNT)
    #define NODE_EXCHANGE_VNT                   "vnt"
#endif
#if IS_ENABLED(CFG_IS_DATETIME)
    #define NODE_EXCHANGE_DT                    "dt"
#endif
#if IS_ENABLED(CFG_IS_SUPPORT_CONNECTED_CLIENT)
    #define NODE_EXCHANGE_CC                    "cc"
    #define TMP_CLIENT_LIST_PATH                "/tmp/online_client_list"
    #define FREEZE_CLIENT_LIST_PATH             "/tmp/online_client_list_freeze"
    #if IS_ENABLED(CFG_CLIENT_LIST_SUPPORT_IPV6)
    #define TMP_IPV6_CLIENT_LIST_PATH           "/tmp/ipv6_client_list"
    #define IPV6_CLIENT_LIST_DBG_MSG            "/tmp/ipv6_cc_dbg_msg"
    #endif
    #define TMP_DHCPD_CLIENT_LIST_PATH          "/tmp/dhcpd_client_list"
    #define TMP_ARP_CLIENT_LIST_PATH            "/tmp/arp_client_list"
    #define TMP_WIFI_CLIENT_LIST_PATH           "/tmp/wifi_client_list"
    #define TMP_WIFI_CLIENT_LIST_PATH_HISTORY   "/tmp/wifi_client_list_history"
    #if IS_ENABLED(CFG_IS_3RD_MCAFEE)
    #define TMP_PC_LIST_DUMP_PATH                "/tmp/pc_list_dump"
        #if IS_ENABLED(CFG_IS_MCAFEE_SUPPORT_MAC_FILTER)
        #define MCAFEE_MAC_FILTER_CFG "/etc/shgw/mac_filter.cfg"
        #endif
	#endif
    #if IS_ENABLED(CFG_CONNECTED_CLIENT_SUPPORT_HOSTNAME_CACHE)
    #define CLIENT_HOSTNAME_CACHE "/tmp/client_hostname_cache"
    #endif
#endif
#if IS_ENABLED(CFG_IS_DDNS)
#define NODE_EXCHANGE_DDNS_STATUS               "ddns_status"
#endif
#define NODE_EXCHANGE_ETH_PORT                  "eth_port"

#define SYSIFD_NETWORK_NOCAP                    "network_nocap"
#define SYSIFD_NETWORK_HASCAP                   "network_hascap"

#if IS_ENABLED(CFG_IS_USB)
    #define USB_EXCHANGE_PATH "/tmp/"
    #define USB_EXCHANGE_WRITE(name, fmt, value)    CMD(FD_NONE, CMD_ECHO " " fmt " > " USB_EXCHANGE_PATH STR(name), value)
    #define NODE_EXCHANGE_SYSHOTPLUGD_STATUS    "syshotplugd_status"
    #define NODE_EXCHANGE_USB_ACTION            "usb_action"
#endif
#define SYSIFD_CHANGE_INIT_TO_AR              "opmode_init_to_ar"
#define SYSIFD_CHANGE_INIT_TO_UR              "opmode_init_to_ur"
#define SYSIFD_CHANGE_CAP_TO_RE               "opmode_cap_to_re"
#define SYSIFD_CHANGE_OPMODE                  "opmode_change"
#if IS_ENABLED(CFG_IS_PKG_QCA_WIFI_SON_AUTO_CAP_SUPPORT)
#define SYSIFD_CHANGE_INIT_TO_CAP              "opmode_init_to_cap"
#define SYSIFD_CHANGE_CAP_TO_RE                "opmode_cap_to_re"
#endif

#if IS_ENABLED(CFG_IS_PKG_HNAP_AGENT) || IS_ENABLED(IS_PKG_ENAPAGENT)
#define SYSIFD_HNAP_AGENTD_UPDATE               "sysifd_hnap_agentd_update"
#endif

#if IS_ENABLED(CFG_IS_SUPPORT_WPS)
#if IS_ENABLED(CFG_IS_RADIO1)
#define WPS_STATUS_RADIO1_NAME                  "wps_status_radio1"
#define FILE_WPS_STATUS_RADIO1_NAME             NODE_EXCHANGE_PATH WPS_STATUS_RADIO1_NAME
#endif
#if IS_ENABLED(CFG_IS_RADIO2)
#define WPS_STATUS_RADIO2_NAME                  "wps_status_radio2"
#define FILE_WPS_STATUS_RADIO2_NAME             NODE_EXCHANGE_PATH WPS_STATUS_RADIO2_NAME
#endif
#if IS_ENABLED(CFG_IS_RADIO3)
#define WPS_STATUS_RADIO3_NAME                  "wps_status_radio3"
#define FILE_WPS_STATUS_RADIO3_NAME             NODE_EXCHANGE_PATH WPS_STATUS_RADIO3_NAME
#endif
#endif

#if IS_ENABLED(CFG_IS_SUPPORT_WPS)
#if IS_ENABLED(CFG_RADIO1_WPS_FUNCTION) || IS_ENABLED(CFG_RADIO2_WPS_FUNCTION) || IS_ENABLED(CFG_RADIO3_WPS_FUNCTION)
#define RADIO_ALL_WPS_PBC		"radio_all_wps_pbc"
#define RADIO_ALL_WPS_PIN		"radio_all_wps_pin"
#endif
#if IS_ENABLED(CFG_RADIO1_WPS_FUNCTION)
#define RADIO_1_WPS_PBC			"radio_1_wps_pbc"
#define RADIO_1_WPS_PIN			"radio_1_wps_pin"
#endif
#if IS_ENABLED(CFG_RADIO2_WPS_FUNCTION)
#define RADIO_2_WPS_PBC			"radio_2_wps_pbc"
#define RADIO_2_WPS_PIN			"radio_2_wps_pin"
#endif
#if IS_ENABLED(CFG_RADIO3_WPS_FUNCTION)
#define RADIO_3_WPS_PBC			"radio_3_wps_pbc"
#define RADIO_3_WPS_PIN			"radio_3_wps_pin"
#endif
#endif

#if IS_ENABLED(CFG_IS_DDNS)
#define DDNS_STATUS_NAME                  "ddns_status"
#define FILE_DDNS_STATUS_NAME             NODE_EXCHANGE_PATH DDNS_STATUS_NAME
#endif



/* GPIO Class */
#if IS_ENABLED(CFG_IS_GPIO_KEYS)
    #define GPIO_KEYS_BTN_DEVICE                "/sys/devices/platform/gpio-keys-polled"
    #if IS_ENABLED(CFG_IS_BTN_OPMODE_1)
    #define GPIO_KEYS_BTN_OPMODE_1              GPIO_KEYS_BTN_DEVICE "/op1"
    #endif
    #if IS_ENABLED(CFG_IS_BTN_OPMODE_2)
    #define GPIO_KEYS_BTN_OPMODE_2              GPIO_KEYS_BTN_DEVICE "/op2"
    #endif
    #if IS_ENABLED(CFG_IS_BTN_OPMODE_3)
    #define GPIO_KEYS_BTN_OPMODE_3				GPIO_KEYS_BTN_DEVICE "/op3"
    #endif
    #if IS_ENABLED(CFG_IS_BTN_OPMODE_4)
    #define GPIO_KEYS_BTN_OPMODE_4				GPIO_KEYS_BTN_DEVICE "/op4"
    #endif
    #define GPIO_KEYS_LED_DEVICE                "/sys/devices/platform/leds-gpio/leds/"
#endif // #if IS_ENABLED(CFG_IS_GPIO_KEYS)

#if IS_ENABLED(CFG_IS_NAT)
#define FILE_IPTABLES_SH                        "/tmp/iptables_open.sh"
#define FILE_IPTABLES_SH_CLOSE                  "/tmp/iptables_close.sh"

#if IS_ENABLED(CFG_IS_IPTABLES_STATIC_ROUTE_IPV4)
#define FILE_STATIC_ROUTE_IPV4_SH_OPEN          "/tmp/static_route_ipv4_open.sh"
#define FILE_STATIC_ROUTE_IPV4_SH_CLOSE         "/tmp/static_route_ipv4_close.sh"
#endif

#if IS_ENABLED(CFG_IS_IPTABLES_STATIC_ROUTE_IPV6)
#define FILE_STATIC_ROUTE_IPV6_SH_OPEN          "/tmp/static_route_ipv6_open.sh"
#define FILE_STATIC_ROUTE_IPV6_SH_CLOSE         "/tmp/static_route_ipv6_close.sh"
#endif

#endif // #if IS_ENABLED(CFG_IS_NAT)

#if IS_ENABLED(CFG_IS_QOS)
#define FILE_QOS_TC_SH_OPEN                     "/tmp/qos_tc_open.sh"
#define FILE_QOS_TC_SH_CLOSE                    "/tmp/qos_tc_close.sh"
#endif

#if IS_ENABLED(CFG_IS_PKG_WAN_PROBE)
#define CMD_WAN_DETECTOR                        "wan_detector"
#endif

#define TOKEN_WITH_HEADER_NAME                  "token.tmp"
#define TOKEN_WITH_HEADER                       "/tmp/"TOKEN_WITH_HEADER_NAME
#define TOKEN_WITHOUT_HEADER                    TOKEN_WITH_HEADER ".bin"

#define TIME_BOOTUP                             30

#define FILE_SH_STATE_INIT                      "/tmp/state_init.sh"

#if IS_ENABLED(CFG_IS_JP_V6_PLUS)
#define V6_PLUS_WGET_RESPONSE_CODE_PATH         "/tmp/v6plus_response_code.txt"
#endif

#if IS_ENABLED(CFG_IS_SUPPORT_VLAN)
#define FILE_VLAN_SWCONFIG_SH                   "/tmp/vlan_swconfig_init.sh"
#endif

#if IS_ENABLED(CFG_IS_LTE)
    #define FILE_LTE_LINK_STATUS                 "/tmp/lte_link_status"
#endif

#define IS_ALREADY_RESET "/tmp/is_already_reset"
#if IS_ENABLED(CFG_IS_CUSTOMER_APPS_DLINK_MYDLINK)
    #define MYDLINK_PATH                         "/mydlink"
    #define MYDLINK_CERT_PATH                    "/mydlink/cert"
    #define MYDLINK_CONFIG_PATH                  "/mydlink/config"
    #define MYDLINK_CONFIG_SRC                   CFG_MTD_PARTITION_STORAGE_PATH "/mydlink_config"
    #define MYDLINK_CERT_SUB_SECTION_SIZE        4096
    #define MYDLINK_PRIVATE_KEY_PATH             "/mydlink/cert/client.crt.key"
    #define MYDLINK_PUBLIC_KEY_PATH              "/mydlink/cert/client.crt.pem"
    #define MYDLINK_PRIVATE_KEY_CHK_STR1         "-----BEGIN RSA PRIVATE KEY-----"
    #define MYDLINK_PRIVATE_KEY_CHK_STR2         "-----END RSA PRIVATE KEY-----"
    #define MYDLINK_PUBLIC_KEY_CHK_STR1          "-----BEGIN CERTIFICATE-----"
    #define MYDLINK_PUBLIC_KEY_CHK_STR2          "-----END CERTIFICATE-----"
#endif

#if IS_ENABLED(CFG_IS_CUSTOMER_APPS_DLINK_MYDLINK)
#define MYDLINK_DISABLE_TAG "/storage/disable_mydlink"
/* send signal USR2 to da_adaptor */
#define SEND_SIGUSR2_TO_MYDLINK() { \
                                        T_PRINTF_WARN("[MYDLINK] Reset mydlink\n"); \
                                        SIGNAL_PROCNAME("da_adaptor", 1, 100000, SIGUSR2); \
                                  }
/* remove mydlink config, not MYDLINK_CONFIG_PATH */
#define CLEAN_MYDLINK_CONFIG() { \
                                        T_PRINTF_WARN("[MYDLINK] Clean mydlink config\n"); \
                                        CMD(FD_NONE, CMD_RM " -rf " MYDLINK_CONFIG_SRC "/*"); \
                               }
#endif // #if IS_ENABLED(CFG_IS_CUSTOMER_APPS_DLINK_MYDLINK)

#if IS_ENABLED(CFG_IS_3RD_MCAFEE)
#define MCAFEE_DISABLE_TAG "/storage/disable_mcafee"
#define MCAFEE_SHGW_IS_START "/tmp/mcafee_is_start"
/* McAfee path */
#define CMD_SHGW_WATCHDOGD "shgw_watchdogd"
#define CMD_SHGW "shgw"
#define SHGW_WATCHDOGD_CONF "/etc/shgw/shgw_watchdogd.conf"
#define SHGW_LOG "/var/McAfee/.shgw/shgw.log"
#define SHGW_STARTUP_LOG "/var/McAfee/.shgw_startup.log"
/* reset McAfee shgw */
#define RESET_MCAFEE_SHGW() { \
                                    if( osapi_is_file_existed("/bin/shgw") ) \
                                    { \
                                        T_PRINTF_WARN("[MCAFEE] Reset McAfee shgw\n"); \
                                        CMD(FD_NONE, "/bin/shgw reset"); \
                                    } \
                              }
/* remove McAfee config */
#if IS_ENABLED(CFG_IS_MTD_PARTITION_MCAFEE)
#define CLEAN_MCAFEE_CONFIG() { \
                                        T_PRINTF_WARN("[MCAFEE] Clean McAfee config\n"); \
                                        CMD(FD_NONE, CMD_RM " -rf " CFG_MTD_PARTITION_MCAFEE_PATH "/*"); \
                              }
/* prepare McAfee binaries */
/*
        // cd /
        // tar -xvf /etc/McAfee_bin.tgz
        // chmod 775 /McAfee_bin/bin/ *
        // chmod 775 /McAfee_bin/etc/shgw/ *
        // awk 'BEGIN { cmd="cp -pri /McAfee_bin/bin/ * /bin/."; print "n" |cmd; }'
        // awk 'BEGIN { cmd="cp -pri /McAfee_bin/etc/shgw/ * /etc/shgw/."; print "n" |cmd; }'
        // rm -rf /McAfee_bin
        do not overwrite
*/
#define CREATE_MCAFEE_CONFIG() { \
    if (osapi_is_file_existed("/etc/McAfee_bin.tgz")) \
    { \
        CMD(FD_NONE, "cd /"); \
        CMD(FD_NONE, CMD_TAR " -xvf /etc/McAfee_bin.tgz > /dev/null 2>&1"); \
        CMD(FD_NONE, CMD_CHMOD " 775 /McAfee_bin/bin/*"); \
        CMD(FD_NONE, CMD_CHMOD " 775 /McAfee_bin/etc/shgw/*"); \
        CMD(FD_NONE, CMD_AWK " 'BEGIN { cmd=\"" CMD_CP " -pri /McAfee_bin/bin/* /bin/.\"; print \"n\" |cmd; }' > /dev/null 2>&1"); \
        CMD(FD_NONE, CMD_AWK " 'BEGIN { cmd=\"" CMD_CP " -pri /McAfee_bin/etc/shgw/* /etc/shgw/.\"; print \"n\" |cmd; }' > /dev/null 2>&1"); \
        CMD(FD_NONE, CMD_RM " -rf /McAfee_bin"); \
    } \
                               }
#endif // #if IS_ENABLED(CFG_IS_MTD_PARTITION_MCAFEE)
#endif // #if IS_ENABLED(CFG_IS_3RD_MCAFEE)

#if IS_ENABLED(CFG_IS_3RD_OOKLA)
#define SPEEDTEST_REPORT "/tmp/speedtest_report"
#define SPEEDTEST_TMP_RESULT "/tmp/speedtest_tmp_result"
#endif

#if IS_ENABLED(CFG_CLIENT_LIST_SUPPORT_ARP_SCAN)
#define REGX_GREP_GENERALIZED_IP    "'[0-9]\\{1,3\\}\\.[0-9]\\{1,3\\}\\.[0-9]\\{1,3\\}\\.[0-9]\\{1,3\\}'"
#define FILE_TMP_ARP_CLIENT_LIST    "/tmp/arp_client_list"
#define CMD_ARPSCAN                 "arp-scan"
#endif

#define FILE_TMP_RE_BSSID_LIST      "/tmp/re_bssid_list" //[wsplcd] apac_hyfi20_mib_tw.c

#if IS_ENABLED(CFG_IS_PKG_AGENTD)
#define FILE_HNAP_AGENTD_PID        "/var/run/hnap_agentd.pid"
#endif

#define SYSIFD_CMD_LOG "/tmp/sysifd_cmd_log"
#define SYSIFD_MODULE_LOG "/tmp/sysifd_module_log"
#define PROCMANAGER_TRAFFIC_LOG "/tmp/procmanager_traffic_log"
#define INSMOD_WIFI_DRIVER_DONE "/tmp/insmod_wifi"
//
//
#define FOTA_SEND_FOTA_EVENT "/storage/send_fota_event" // DO NOT rename for backoff support
#define FOTA_IS_AUTO "/tmp/fota_is_auto"
#define FOTA_LAST_QUERY_FW_TIME "/tmp/fota_last_query_fw_time"
#if IS_ENABLED(CFG_SUPPORT_ALSO_TRIGGER_RE_QUERY_FW_INFO_IN_CAP_GETFIRMWARESTATUS)
#define FOTA_RE_UNDER_QUERY_FW_STATUS "/tmp/fota_re_under_query_fw_status"
#define FOTA_RE_FW_STATUS_LIST "/tmp/fota_re_fw_status_list"
#endif
#define FOTA_RE_HAS_NEW_FW_LIST "/tmp/fota_re_has_new_fw_list"
#define FOTA_RE_FIRST_POLLING_TIME "/tmp/fota_re_first_polling_time"
#define FOTA_RE_DL_PROGRESS "/tmp/fota_dl_progress"
#define FOTA_DURING_AUTO_FOTA "/tmp/fota_during_auto_fota"
//
//
#define FAKE_FW_UPGRADE_ENABLE_TAG "/storage/fake_fw_upgrade"
#define FAKE_FW_UPGRADE "/tmp/fake_fw_upgrade"
#if IS_ENABLED(CFG_SUPPORT_RE_READY_CHECK)
#define RE_READY_TAG "/tmp/re_ready"
#endif
//
#if IS_ENABLED(CFG_IS_UPLINK_CHECK_BY_DNS_QUERY) && IS_ENABLED(CFG_IS_UPLINK_CHECK_URL_LIST)
#define UPLINK_CHECK_URL_LIST "/etc/uplink_check_url_list"
#define UPLINK_CHECK_URL_LIST_SAVE "/storage/uplink_check_url_list"
#endif
//
#define LED_FADER_ENABLE_TAG "/storage/enable_led_fader"
//
#define STA_TYPE_WIFI_24G_STR "WiFi_2.4G"
#define STA_TYPE_WIFI_5G_STR "WiFi_5G"
#define STA_TYPE_WIFI_5G_2_STR "WiFi_5G_2"
#define STA_TYPE_WIFI_24G_GUEST_STR "WiFi_2.4G_Guest"
#define STA_TYPE_WIFI_5G_GUEST_STR "WiFi_5G_Guest"
#define STA_TYPE_WIFI_24G_RE_STR "WiFi_2.4G_MeshExtender"
//#define STA_TYPE_WIFI_5G_RE_STR "WiFi_5G_MeshExtender" // not used
#define STA_TYPE_LAN_STR "LAN"
#define UNKNOWN_STR "UnKnown"

/* Device name defined of REs for client list display */
#define COVR_DEVICE_NAME_PREFIX "COVR-"
#define COVR_C1200_DEVICE_NAME "COVR-C1200"
#define COVR_C1210_DEVICE_NAME "COVR-C1210"
#define COVR_P2500_DEVICE_NAME "COVR-P2500"

/*>>>>>-----------------------------------------------------------------<<<<<*/
#define LEN_BUF_16                                16
#define LEN_BUF_32                                32
#define LEN_BUF_64                                64
#define LEN_BUF_128                              128
#define LEN_BUF_256                              256
#define LEN_BUF_512                              512
#define LEN_BUF_1024                            1024

#define MAX_MAC_ADDR_BIN_SIZE                      6
#define MAX_MAC_ADDR_STR_SIZE                     17 // (2*6+5)
#define MAX_IP_STR_SIZE                           15 // (4*3+3)
#define MAX_SUBMASK_STR_SIZE                      15
#define MAX_IPV6_STR_SIZE                         47 // INET6_ADDRSTRLEN (48)
/*>>>>>-----------------------------------------------------------------<<<<<*/

/*>>>>>-----------------------------------------------------------------<<<<<*/
/**
 * Event Variables:
 */
#define EVENT_REPLY_OK                      "200 "
#define EVENT_REPLY_ERROR                   "400 "
#define EVENT_REPLY_UNKNOWN                 "500 "
#define EVENT_REPLY_LEN_MAX                 LEN_BUF_256
typedef enum {
    EVENT_HELP                              = 0,
    #if 0
    EVENT_PINGPONG,
    #endif
    EVENT_MAX
} TYPE_EVENT;
#include <cfg_product_event_led.h>
#include <cfg_product_event_sys.h>
#include <cfg_product_srv.h>

#endif /* __CFG_PRODUCT_VARIABLES_H__ */
