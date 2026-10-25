#ifndef __NETWORK_H__
#define __NETWORK_H__

#define IP_PROTO_STATIC    0
#define IP_PROTO_DHCP      1
#define LINK_CHANGED_STATUS_FILE "/tmp/link_changed_status"
#define FIRST_LINK_CHANGED     "/tmp/first_link_changed"
#define MAC_BCAST_ADDR "\xff\xff\xff\xff\xff\xff"
#define ARPOP_REQUEST   1    /* ARP request*/
#define ETH_P_ARP  0x0806    /* Address Resolution packet    */
#define ARPHRD_ETHER  1
#define ETH_P_IP 0x0800
#define IFNAMSIZ 16

typedef enum
{
    NTP_GET,
    NTP_SET,
    NTP_DEL,
    NTP_DELLIST,
    NTP_ADDLIST,
} NTP_ACTION_E;

typedef struct
{
    char timeZones[BUFLEN_128];
    char TimeZone[BUFLEN_32];
    char firstServer[BUFLEN_128];
    char secondServer[BUFLEN_128];
    char zoneName[BUFLEN_128];
}ZoneTime_T;

typedef enum
{
    NETWORK_ACTION_LIST = 1,
    NETWORK_ACTION_COUNT,
    NETWORK_ACTION_GET,
    NETWORK_ACTION_SET,
    NETWORK_ACTION_UNSET,
    NETWORK_ACTION_BATCH,
    NETWORK_ACTION_DEL,
} NETWORK_ACTION_E;

typedef enum
{
    DHCP_ACTION_LIST = 1,
    DHCP_ACTION_COUNT,
    DHCP_ACTION_GET,
    DHCP_ACTION_SET,
    DHCP_ACTION_UNSET,
    DHCP_ACTION_BATCH,
    DHCP_ACTION_DEL,
} DHCP_ACTION_E;


typedef struct{
    char lanIpAddr[BUFLEN_256];
    char lanSubnet[BUFLEN_256];
    char lanGateway[BUFLEN_256];
    char lanDns1[BUFLEN_256];
    char lanDns2[BUFLEN_256];
    char proto[BUFLEN_32];
    UINT32 totalPort;
    UINT32 usedPort;
}NET_LAN_IP_T;


typedef enum
{
    DEV_TYPE_ETH,
    DEV_TYPE_RA0,
    DEV_TYPE_RAX0
} DEV_TYPE_E;

typedef struct{
    char ip[BUFLEN_32];
    char name[BUFLEN_128];
    char mac[BUFLEN_64];
    char vmac[BUFLEN_64];
    char channel[BUFLEN_32];
    char rssi[BUFLEN_32];
    char rate[BUFLEN_64];
    char id[BUFLEN_32];
    char block[BUFLEN_32];
    char radio[BUFLEN_64];
    char ssid[BUFLEN_128];
    char relateDev[BUFLEN_32]; //brLan, ra0, rax0, extender
    char ipProto[BUFLEN_32];   //0 static, 1 dhcp
    char connType[BUFLEN_32];
    char devModel[BUFLEN_32];
    char devType[BUFLEN_32];
    char up[BUFLEN_8];          // 1: up, 0: down
}NET_ATTACH_DEV_T;

#define ATTACH_DEV_MAX      253
#define BR_DEV              "br-lan"
#define BR_ETH_DEV          "eth1"
#define BR_2G_DEV           "ra0"
#define BR_2G_GUEST_DEV     "ra1"
#define BR_5G_DEV           "rai0"
#define BR_5G_GUEST_DEV     "rai1"
#define BR_EXD_DEV          "extender"
#define BR_EXD_WAN_PRE      "apcli"

typedef struct
{
    char    name[BUFLEN_64];	/**< Name */
    char    minute[BUFLEN_64];	/**< Minute */
    char    hour[BUFLEN_64];	/**< Hour */
    char    day[BUFLEN_64];	/**< Day */
    char    month[BUFLEN_64];	/**< Month */
    char    week[BUFLEN_64];	/**< Week */
    char    cmd[BUFLEN_64];	/**< Cmd */
    char    cmdParam1[BUFLEN_128];	/**< CmdParam1 */
    char    cmdParam2[BUFLEN_128];	/**< CmdParam2 */
} crondListObject;

typedef struct
{
    char status[BUFLEN_32];         // 连接状态, 取值范围 'Online' / 'Offline'
    char connType[BUFLEN_32];       // 连接类型, 取值范围 'wired' / '2.4G' / '5G' / '5G-2'
    char deviceName[BUFLEN_64];     // 设备名称 比如 'Jack-iPhone'
                                    // 1. 如果用户设置了设备名，就用用户设置的
                                    // 2. 如果用户没有指定名称，就用系统自动获取到的
                                    // 3. 如果用户没有指定，且系统没有自动获取到，为 'unknown'
    char deviceModel[BUFLEN_64];    // 设备型号 比如 'iPhone-X'
                                    // 1. 如果用户设置了设备名，就用用户设置的
                                    // 2. 如果用户没有指定名称，就用系统自动获取到的
                                    // 3. 如果用户没有指定，且系统没有自动获取到，为 'unknown'
    char deviceType[BUFLEN_64];     // 设备类型      PC/Mobile/STB
    char ip[BUFLEN_32];             // IP地址 '192.168.1.100'
    char mac[BUFLEN_32];            // MAC地址 '00:11:22:33:AA:BB'
    UINT32 rate;                    // 连接速率 单位 Mbps
    char rssi[BUFLEN_32];           // 无线设备RSSI, 比如: -46
    char allowOrBlock[BUFLEN_32];   // 允许连接或不允许连接, 取值范围 'allow' / 'block'
} LAN_DEVICE_INFO;

typedef struct
{
    char scheduleName[BUFLEN_64];	/**< ScheduleName */
    char fromTime[BUFLEN_32];	/**< FromTime */
    char toTime[BUFLEN_32];	/**< ToTime */
    char week[BUFLEN_64];	/**< Week */
} scheduleListObject;

extern ZoneTime_T zoneTime[];

int dai_getLanProto(int *protoStatus);
int dai_setLanProto(int proto);
int dai_getLanIpPara(NET_LAN_IP_T *lanIpPara);
int dai_setLanIpPara(NET_LAN_IP_T lanIpPara);
void dai_clearDHCPRecvAck(void);
UBOOL8 dai_getLanip(char *ipaddr, int len);
int dai_networkCfgDoAction(NETWORK_ACTION_E action,char *key, char *value, UINT32 valueLen);
int dai_dhcpCfgDoAction(DHCP_ACTION_E action,char *key, char *value, UINT32 valueLen);

char tbsCheckTimeZone(char *pszTimeZone);
void dai_NetNtpZoneDoAction(NTP_ACTION_E action, char *value, UINT32 valueLen);
void dai_NetNtpDstDoAction(NTP_ACTION_E action, char *value, UINT32 valueLen);
void dai_NetNtpDayLightSaving(NTP_ACTION_E action, char *value, UINT32 valueLen);
void dai_NetNtpIndexValue(NTP_ACTION_E action, char *value, UINT32 valueLen);
void dai_NetNtpCfgDoAction(NTP_ACTION_E action, char *key, char *value, UINT32 valueLen);
void dai_getAclStatus(char *aclStatus);
void dai_getGuestAclStatus(char *aclStatus);
void dai_setGuestAclStatus(char *aclStatus);
void dai_getAttachDev(NET_ATTACH_DEV_T *devList);
void dai_wifiGetBlackList(NET_ATTACH_DEV_T *blackList);
void dai_wifiGetGuestBlackList(NET_ATTACH_DEV_T *blackList);
int dai_getLanDeviceInfo(const char *mac, LAN_DEVICE_INFO *info);
int dai_getNextLanDeviceInfo(UINT32 idx, LAN_DEVICE_INFO *info);
void dai_updateBrIp(char *rtIp, char *subNet, char *ipAddr);
int dai_setDeviceName(char *mac, char *name);

int dai_netScheduleListDoAction(NTP_ACTION_E action, scheduleListObject * scheduleList, int num);
void dai_netScheduleCfg(NTP_ACTION_E action, char *key, char *value, UINT32 valueLen);

void dai_linkRootApStatus(int status, int apMode, int lanProt, int lanProtLast, int *linkTimes);
UBOOL8 dai_getNetworkLinkRootApStatus(void);
void dai_getGateWay(char *gateWay);
void dai_changeEbtables(char isadd);
UBOOL8 dai_getNetworkEtherPortLinksatus(char *interface);
int dai_getNetworkEtherPortLinkspeed(char *interface);
int dai_getEN8811Linkspeed(char *speed);
int dai_procIsExist(char *procName);
int dai_udhcpcPID(void);
int dai_getUplinkDhcpStatus(void);
int dai_getUCILinkRootApStatus(void);
int dai_isExistDefaultRt(void);
void dai_setNetworkPingOK(int status);
int dai_isFilterArpEbExist(void);
int dai_GraArpMsg(char * yiaddr, char *senderAddr, char *numMac, char *senderDev);

void dai_wifiGetGuestBlackList(NET_ATTACH_DEV_T *blackList);
void dai_initTimezone(char *regionCode, int len);
void dai_setAclStatus(char *aclStatus);
UBOOL8 cgiMatch(const char *pszString, const char *pszPattern);
void updateCrondFile(void);
void dai_ntpReload(void);
void dai_networkReload(void);
void dai_updateLanDns(char *dnsAddr, char *dnsAddr2);
int dai_getUbuslanStatus(char *status, int len);
int dai_getbr_dns_hijackv4ip(char *ipaddr, int len);
int dai_getbripv4Addr(char *ipaddr, int len);
void dai_udhcpc_renew();

char* strrpl(char* s, const char* s1, const char* s2);

void dai_getIpByMacFromArp(unsigned char *mac, char *ip);

#endif
