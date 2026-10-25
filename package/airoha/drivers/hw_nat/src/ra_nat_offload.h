#ifndef _RA_NAT_OFFLOAD_H
#define _RA_NAT_OFFLOAD_H

struct dsOffloadFtp_t{
	u_int32_t ipaddr;
	u_int16_t port;
};

#define USERDEFINEDNUM	16
struct SoftQdmaUDFTcpPort {
	uint8_t udfPortNum;
	uint16_t udfTcpPort[USERDEFINEDNUM];
};

#define DLF_UP_METER_INDEX		61
#define DLF_DOWN_METER_INDEX	62

#define SMB_PORT1 445
#define SMB_PORT2 139

#define IP_VER_V4	4
#define IP_VER_V6	6

#define PROTOCOL_WHITE_LIST	0xFFFF0000

typedef enum
{
	FORCE_TO_CPU_WLAN = 0,
	FORCE_TO_CPU_IPSEC,
	FORCE_TO_CPU_DS_OFFLOAD,
	FORCE_TO_CPU_LOCAL_IN,
	FORCE_TO_CPU_MULTICAST,
	FORCE_TO_CPU_SOFT_RATELIMIT,	
	FORCE_TO_CPU_GRE_VXLAN_WAN_OFFLOAD,	
	FORCE_TO_CPU_GRE_TUNNEL_OFFLOAD,
    	FORCE_TO_CPU_VXLAN_TUNNEL_OFFLOAD,
    	FORCE_TO_CPU_ASYM_OFFLOAD,
	FORCE_TO_CPU_DEFAULT_OFFLOAD,		
    	FORCE_TO_CPU_NATV6,
    FORCE_TO_CPU_HIT_IFC,
    FORCE_TO_CPU_OOKLA,
	FORCE_TO_CPU_INVALID,
}FORCE_TO_CPU_IF;

#define WIFI_RATELIMIT_RX 0
#define WIFI_RATELIMIT_TX 1
#if defined(TCSUPPORT_WLAN_MT7990)
#define SINGLE_WLAN_SSID_NUM            4
#define WLAN_BAND_NUM					3
#else
#define SINGLE_WLAN_SSID_NUM            8
#define WLAN_BAND_NUM					2
#endif
#define SINGLE_WDS_NUM  4
#define SINGLE_APCLI_NUM  1


#define ETHERWAN_ITF "nas10"
#define XDSL_ITF_PRE "nas"
#define PON_DEV_NAME    "pon"
/*
 * TYPEDEFS AND STRUCTURES
 */
enum DstPort {
	DP_RA0 = 6,
	DP_RA1 = 7,
	DP_RA2 = 8,
	DP_RA3 = 9,
	DP_RA4 = 10,
	DP_RA5 = 11,
	DP_RA6 = 12,
	DP_RA7 = 13,
	DP_RAI0 = 15,
	DP_RAI1 = 16,
	DP_RAI2 = 17,
	DP_RAI3 = 18,
	DP_RAI4 = 19,
	DP_RAI5 = 20,
	DP_RAI6 = 21,
	DP_RAI7 = 22,
	DP_GMAC = 25,
	DP_PTM0 = 26,
	DP_PTM1 = 27,
	DP_PON = 28,
	DP_WDS0 = 29,
	DP_WDS1 = 30,
	DP_WDS2 = 31,
	DP_WDS3 = 32,
	DP_WDSI0 = 33,
	DP_WDSI1 = 34,
	DP_WDSI2 = 35,
	DP_WDSI3 = 36,
	DP_GMAC1 = 37,	//eth0.1
	DP_GMAC2 = 38,	//eth0.2
	DP_GMAC3 = 39,	//eth0.3
	DP_GMAC4 = 40,	//eth0.4
	DP_GMAC5 = 41,	//eth0.5
	DP_GMAC6 = 42,	//eth0.6
	DP_GMAC7 = 43,	//eth0.7
	DP_GMAC8 = 44,	//eth0.8
	DP_NAS0 = 45,
	DP_NAS1 = 46,
	DP_NAS2 = 47,
	DP_NAS3 = 48,
	DP_NAS4 = 49,
	DP_NAS5 = 50,
	DP_NAS6 = 51,
	DP_NAS7 = 52,
	DP_PPP0 = 53,
	DP_PPP1 = 54,
	DP_PPP2 = 55,
	DP_PPP3 = 56,
	DP_PPP4 = 57,
	DP_PPP5 = 58,
	DP_PPP6 = 59,
	DP_PPP7 = 60,
	DP_MCAST_NAS0 = 61,
	DP_MCAST_NAS1 = 62,
	DP_MCAST_NAS2 = 63,
	DP_MCAST_NAS3 = 64,
	DP_MCAST_NAS4 = 65,
	DP_MCAST_NAS5 = 66,
	DP_MCAST_NAS6 = 67,
	DP_MCAST_NAS7 = 68,
	DP_MCAST_PPP0 = 69,
	DP_MCAST_PPP1 = 70,
	DP_MCAST_PPP2 = 71,
	DP_MCAST_PPP3 = 72,
	DP_MCAST_PPP4 = 73,
	DP_MCAST_PPP5 = 74,
	DP_MCAST_PPP6 = 75,
	DP_MCAST_PPP7 = 76,
	DP_ASYM_D1 = 77,
	DP_ASYM_D2 = 78,
	DP_ASYM_D3 = 79,
	DP_ASYM_D4 = 80,
	DP_ASYM_D5 = 81,
	DP_ASYM_U1 = 82,
	DP_ASYM_U2 = 83,
	DP_ASYM_U3 = 84,
	DP_ASYM_U4 = 85,
	DP_ASYM_U5 = 86,
	DP_XSI = 87,
	DP_APCLI0  = 88,
	DP_APCLII0 = 89,
	DP_USBNET0 = 90,
	DP_USBNET1 = 91,
	DP_USBNET2 = 92,
        /*DP_OOKLA_RX = 93,
	DP_OOKLA_TX = 94,  these 2 are reserved for ookla offload*/ 	
	#if 0/*move to kernel,do not define other macro 100~116 and 200~216 in hwnat*/
	/*below for crypto mapping*/
	DP_CRYPTO_E_0 = 100,
   	DP_CRYPTO_E_1 = 101,
   	DP_CRYPTO_E_2 = 102,
   	DP_CRYPTO_E_3 = 103,
   	DP_CRYPTO_E_4 = 104,
   	DP_CRYPTO_E_5 = 105,
   	DP_CRYPTO_E_6 = 106,
   	DP_CRYPTO_E_7 = 107,
   	DP_CRYPTO_E_8 = 108,
   	DP_CRYPTO_E_9 = 109,
   	DP_CRYPTO_E_10 = 110,
   	DP_CRYPTO_E_11 = 111,
   	DP_CRYPTO_E_12 = 112,
   	DP_CRYPTO_E_13 = 113,
	DP_CRYPTO_E_14 = 114,
	DP_CRYPTO_E_15 = 115,
	DP_CRYPTO_E_MAX = 116,	//modify when ipsec max entry index changes
	/*117~199 reserved for vpn request for the coming future*/
  	DP_CRYPTO_D_0 = 200,
   	DP_CRYPTO_D_1 = 201,
   	DP_CRYPTO_D_2 = 202,
   	DP_CRYPTO_D_3 = 203,
   	DP_CRYPTO_D_4 = 204,
   	DP_CRYPTO_D_5 = 205,
   	DP_CRYPTO_D_6 = 206,
   	DP_CRYPTO_D_7 = 207,
   	DP_CRYPTO_D_8 = 208,
   	DP_CRYPTO_D_9 = 209,
   	DP_CRYPTO_D_10 = 210,
   	DP_CRYPTO_D_11 = 211,
   	DP_CRYPTO_D_12 = 212,
   	DP_CRYPTO_D_13 = 213,
	DP_CRYPTO_D_14 = 214,
	DP_CRYPTO_D_15 = 215,	
	DP_CRYPTO_D_MAX = 216,//modify when ipsec max entry index changes
	#endif
	DP_GRE_L2_PRI_TUNNEL_0 = 117,
	DP_GRE_L2_PRI_TUNNEL_1 = 118,
	DP_GRE_L2_PRI_TUNNEL_2 = 119,
	DP_GRE_L2_PRI_TUNNEL_3 = 120,
	DP_GRE_L2_PRI_TUNNEL_4 = 121,
	DP_GRE_L2_PRI_TUNNEL_5 = 122,
	DP_GRE_L2_PRI_TUNNEL_6 = 123,
	DP_GRE_L2_PRI_TUNNEL_7 = 124,
	DP_GRE_L2_SEC_TUNNEL_0 = 125,
	DP_GRE_L2_SEC_TUNNEL_1 = 126,
	DP_GRE_L2_SEC_TUNNEL_2 = 127,
	DP_GRE_L2_SEC_TUNNEL_3 = 128,
	DP_GRE_L2_SEC_TUNNEL_4 = 129,
	DP_GRE_L2_SEC_TUNNEL_5 = 130,
	DP_GRE_L2_SEC_TUNNEL_6 = 131,
	DP_GRE_L2_SEC_TUNNEL_7 = 132,
	DP_L2TP_HWDOWN = 133,
	DP_RAX0 = 134,
	DP_RAX1 = 135,
	DP_RAX2 = 136,
	DP_RAX3 = 137,
	DP_RAX4 = 138,
	DP_RAX5 = 139,
	DP_RAX6 = 140,
	DP_RAX7 = 141,
	DP_APCLIX0 = 142,
	DP_LOCAL_HOST = 217,
	/*217~299 reserved for vpn request for the coming future*/
	DP_GRE_TUNNEL_0 = 218,
	DP_GRE_TUNNEL_1 = 219,
	DP_GRE_TUNNEL_2 = 220,
	DP_GRE_TUNNEL_3 = 221,
	DP_GRE_TUNNEL_4 = 222,
	DP_GRE_TUNNEL_5 = 223,
	DP_GRE_TUNNEL_6 = 224,
	DP_GRE_TUNNEL_7 = 225,
    DP_VXLAN_0      = 226,
    DP_VXLAN_1      = 227,
    DP_VXLAN_2      = 228,
    DP_VXLAN_3      = 229,
    DP_VXLAN_4      = 230,
    DP_VXLAN_5      = 231,
    DP_VXLAN_6      = 232,
    DP_VXLAN_7      = 233,
    DP_VXLAN_8      = 234,
    DP_VXLAN_9      = 235,
    DP_VXLAN_10     = 236,
    DP_VXLAN_11     = 237,
    DP_VXLAN_12     = 238,
    DP_VXLAN_13     = 239,
    DP_VXLAN_14     = 240,
    DP_VXLAN_15     = 241,
    DP_VXLAN_16     = 242,
    DP_VXLAN_17     = 243,
    DP_VXLAN_18     = 244,
    DP_VXLAN_19     = 245,
    DP_MAP_E_US = 246,
    DP_MAP_E_DS = 247,
	DP_L2TP_DOWN_2 = 300,
	DP_L2TP_UP_2 = 301,
	DP_GRE_DOWN_2 = 302,
	DP_GRE_UP_2 = 303,
	DP_SPEED_UP = 304,
    DP_VXLAN_DOWN_2 = 305,
	DP_VXLAN_UP_2_0 = 306,
	DP_VXLAN_UP_2_1 = 307,
	DP_VXLAN_UP_2_2 = 308,
	DP_VXLAN_UP_2_3 = 309,
	DP_VXLAN_UP_2_4 = 310,
	DP_VXLAN_UP_2_5 = 311,
	DP_VXLAN_UP_2_6 = 312,
	DP_VXLAN_UP_2_7 = 313,
	DP_VXLAN_UP_2_8 = 314,
	DP_VXLAN_UP_2_9 = 315,
	DP_VXLAN_UP_2_10 = 316,
	DP_VXLAN_UP_2_11 = 317,
	DP_VXLAN_UP_2_12 = 318,
	DP_VXLAN_UP_2_13 = 319,
	DP_VXLAN_UP_2_14 = 320,
	DP_VXLAN_UP_2_15 = 321,
	DP_VXLAN_UP_2_16 = 322,
	DP_VXLAN_UP_2_17 = 323,
	DP_VXLAN_UP_2_18 = 324,
	DP_VXLAN_UP_2_19 = 325,
	DP_TSO_LOCALOUT = 326, /* should keep the same MACRO value in /module/private/tso/tso.c */
    DP_APP_SHORTCUT_RA0 = 327,
    DP_APP_SHORTCUT_RA1 = 328,
    DP_APP_SHORTCUT_RA2 = 329,
    DP_APP_SHORTCUT_RA3 = 330, 
    DP_APP_SHORTCUT_RA4 = 331,
    DP_APP_SHORTCUT_RA5 = 332,
    DP_APP_SHORTCUT_RA6 = 333,
    DP_APP_SHORTCUT_RA7 = 334, 
    DP_APP_SHORTCUT_RAI0 = 335,
    DP_APP_SHORTCUT_RAI1 = 336,
    DP_APP_SHORTCUT_RAI2 = 337,
    DP_APP_SHORTCUT_RAI3 = 338, 
    DP_APP_SHORTCUT_RAI4 = 339,
    DP_APP_SHORTCUT_RAI5 = 340,
    DP_APP_SHORTCUT_RAI6 = 341,
    DP_APP_SHORTCUT_RAI7 = 342, 
    DP_APP_SHORTCUT_GMAC1 = 343,
    DP_APP_SHORTCUT_GMAC2 = 344,
    DP_APP_SHORTCUT_GMAC3 = 345,
    DP_APP_SHORTCUT_GMAC4 = 346, 
    DP_APP_SHORTCUT_GMAC5 = 347,
    DP_APP_SHORTCUT_GMAC6 = 348,
    DP_APP_SHORTCUT_GMAC7 = 349,
    DP_APP_SHORTCUT_GMAC8 = 350, 
	DP_FORWARD_LEFT_TO_RIGHT = 351,
        DP_MAP_T = 352,
        DP_TR471_UP = 353,
	MAX_IF_NUM
};

struct force_to_cpu_para
{
	unsigned int ds_offload;
	unsigned int natv6_dir;
	unsigned int natv6_attr;
    unsigned int natv6_src_wlan;
};

typedef struct
{
	int foe_idx;
	struct dst_entry *dst;
	struct net_device *dev;
}l2tp_important_info;

struct DLFHwEntry {
	int entryIndex;
	unsigned char dstMac[6];

	struct list_head  list;
};

struct DLFNFHwEntry {
	int entryIndex;
    uint32_t dstIp;   
	struct list_head  list;
};

typedef struct
{
	struct list_head  list;
	unsigned short dport;
}PpeLocalInDportList_t;

typedef struct
{
	int foe_idx;
	struct dst_entry *dst;
	struct net_device *dev;
}gre_tx_info;

typedef struct
{
    struct list_head  list;
    unsigned short vid;
}HwnatWanMcastVlan_t;

typedef struct
{
	int foe_idx;
	struct dst_entry *dst;
	struct net_device *dev;
}vxlan_tx_info;

struct VxlanLink
{
    int vni;
    void * dev;
};

#define VXLAN_MAX_LINK_NUM 20

typedef enum
{
	NATV6_DIR_GPON = 1,
	NATV6_DIR_EPON,
	NATV6_DIR_GE,
	NATV6_DIR_WLAN,
	NATV6_DIR_AE_WAN,
	NATV6_DIR_USB,
	NATV6_DIR_XSI_GDM4,
}NATV6_DIR;

typedef enum
{
	NATV6_ATTR_SIP = 0,
	NATV6_ATTR_DIP,
	NATV6_ATTR_UDP_SPORT,	
	NATV6_ATTR_UDP_DPORT,
	NATV6_ATTR_TCP_SPORT,	
	NATV6_ATTR_TCP_DPORT
}NATV6_ATTR;

typedef enum
{
	NAT66 = 0,
	NAT64,
	NAT44,
}NATV6_TYPE;

typedef enum
{
    IFC_HIT_DIR_GPON = 1,
    IFC_HIT_DIR_EPON,
    IFC_HIT_DIR_GE,
    IFC_HIT_DIR_WLAN
}IFC_HIT_DIR;

#endif


