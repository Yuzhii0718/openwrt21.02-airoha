#ifndef __WIFI_H__
#define __WIFI_H__

#include "fwk/fwk.h"

typedef enum
{
    ACTION_LIST = 1,
    ACTION_COUNT,
    ACTION_GET,
    ACTION_SET,
    ACTION_UNSET,
    ACTION_BATCH,
    ACTION_DEL,
} WLAN_ACTION_E;

typedef enum{
    ENCRYPTION_NONE    = 0x01,
    ENCRYPTION_WEP     = 0x02,
    ENCRYPTION_WPAPSK  = 0x04,
    ENCRYPTION_WPA2PSK = 0x08,
    ENCRYPTION_WPA3PSK = 0x10,
    ENCRYPTION_WPA3SAE = 0x20,
    ENCRYPTION_WPA     = 0x40,
    ENCRYPTION_WPA2    = 0x80,
    ENCRYPTION_WPA3    = 0x100,
}WLAN_ENCRYPTTYPE_E;

#define STR_ENCRYPTION_OPEN             "OFF"
#define STR_ENCRYPTION_NONE             "None"
#define STR_ENCRYPTION_SHARED           "SHARE"
#define STR_ENCRYPTION_WPAPSK           "WPA-PSK"
#define STR_ENCRYPTION_WPA2PSK          "WPA2-PSK"
#define STR_ENCRYPTION_WPAWPA2PSK       "WPA/WPA2-PSK"
#define STR_ENCRYPTION_WPAPSKWPA2PSK    "WPA-PSK/WPA2-PSK"
#define STR_ENCRYPTION_WEP              "WEP"
#define STR_ENCRYPTION_WPA3SAE          "WPA3-Personal"
#define STR_ENCRYPTION_WPA2PSKWPA3SAE   "WPA2/WPA3-Personal"
#define STR_ENCRYPTION_WPA              "WPA-Enterprise"
#define STR_ENCRYPTION_WPA2             "WPA2-Enterprise"
#define STR_ENCRYPTION_WPAWPA2          "WPA/WPA2-Enterprise"


#define WIFI_FASTLANE_MODE_24G   "2.4G"
#define WIFI_FASTLANE_MODE_5G    "5G"
#define WLAN_2G_5G 3

typedef enum{
    WIFI_CONF_MODE_AP        = 0x00,
    WIFI_CONF_MODE_EXT       = 0x02,
}WIFI_CONF_MODE;

/*
  b^00000001  - TKIP
  b^00000010  - AES
  b^00000100  - SAE
*/
typedef enum{
    CIPHER_TKIP      = 0x01,
    CIPHER_AES       = 0x02,
    CIPHER_SAE       = 0x04,
}WLAN_CIPHERTYPE_E;

#define STR_CIPHER_TKIP             "TKIP"
#define STR_CIPHER_AES              "AES"
#define STR_CIPHER_TKIPAES          "TKIP+AES"
#define STR_CIPHER_NONE             "NONE"
#define STR_CIPHER_SAE              "SAE"
#define STR_CIPHER_AESSAE           "AES+SAE"

typedef struct
{
    char ifName[BUFLEN_32];
    char ssid[BUFLEN_64];
    char bssid[BUFLEN_32];
    char channel[BUFLEN_8];
    char mlo[BUFLEN_8];
    WLAN_ENCRYPTTYPE_E encryption;
    WLAN_CIPHERTYPE_E cipher;
    char signal[BUFLEN_32];
    char mode[BUFLEN_32];
    char bw[BUFLEN_32];
    char vie[256];
    char vieHex[128];
    int  ngrExtAvailable;
    int  hopCnt;
    int  iInValid;
}WLAN_SCANRET_T;

typedef struct
{
    UBOOL8 enbale;
    char ssid[BUFLEN_64];
    char security[BUFLEN_32];
    UINT32 channel;
    char Password[BUFLEN_128];
}WLAN_CONNECT_T;

typedef struct
{
    UINT32 channel;
    char wirelssMode[BUFLEN_16];
    char macAddress[BUFLEN_32];
}WLAN_DEV_CONFIG_T;

typedef struct
{
    UBOOL8 isEanble;
    char ssid[BUFLEN_64];
    UBOOL8 isBroadcast;
    char security[BUFLEN_32];
    char password[BUFLEN_128];
    UINT32 wepAuthType;
    char status[BUFLEN_32];
}WLAN_IF_CONFIG_T;

typedef struct
{
    char wifiCountry[BUFLEN_8];
    UBOOL8 smartConnect;
    UINT32 mode;
    UBOOL8 axEnable;
    UBOOL8 mu_Mimo;
    UBOOL8 fastLane;
    char  fastLaneBand[BUFLEN_8];
    UBOOL8 oneWifiName;
    UBOOL8 syncConfigFromNtgrRouter;
    UBOOL8 wifi6;
    UBOOL8 beEnable;
    UBOOL8 wifi7;
    UBOOL8 ofdmaEnable;
}WLAN_CONFIG_T;

typedef struct
{
    UBOOL8 apModeEnable;
    char frequencyBand[BUFLEN_8];
    char ssid[BUFLEN_64];
    char bssid[BUFLEN_32];
    UINT32 channel;
    char security[BUFLEN_32]; //"WPA-PSK-TKIP" "WPA-PSK-AES" "WPA2-PSK-TKIP" "WPA-PSK-TKIP|WPA2-PSK-AES"...
    char securityType[BUFLEN_32]; // "OFF" "WEP" WPA-PSK" "WPA2-PSK" "WPA/WPA2-PSK" "WPA-PSK/WPA2-PSK" ...
    char linkRate[BUFLEN_32];
    SINT32 signalStrength;
    UBOOL8 apcliExistOption;
    char password[BUFLEN_128];
}WLAN_ROOTAP_CONFIG_T;

#define WLAN_SCANRET_MAX (128)

typedef enum
{
    WLAN_2G = 1,
    WLAN_5G,
} WLAN_BAND_E;

typedef enum {
    MAIN_SSID_2G = 0,
    MAIN_SSID_5G,
    GUEST_SSID_2G,
    GUEST_SSID_5G,
}SSID_IDX_MAP_E;

#define WLAN_ENABLE TRUE
#define WLAN_DISABLE FALSE

#define WLAN2G_0 0
#define WLAN5G_0 0
#define WLAN2G_1 1
#define WLAN5G_1 1

#define WLAN_MLD_MODE_AP    1
#define WLAN_MLD_MODE_STA   2

#define SSID_WEP_AUTH_OPEN                  "Open"
#define SSID_WEP_AUTH_SHARED                "Shared"
#define SSID_WEP_AUTH_AUTO                  "Automatic"

#define SSID_SCURTYSTR_NONE                 "None"
#define SSID_SCURTYSTR_WEP                  "WEP"
#define SSID_SCURTYSTR_OFF                  "OFF"

//if firmware supports WPA3
#define SSID_SCURTYSTR_WPA2_PERS            "WPA2-Personal"
#define SSID_SCURTYSTR_WPA3_PERS            "WPA3-Personal"
#define SSID_SCURTYSTR_WPAWPA2_PERS         "WPA/WPA2-Personal"
#define SSID_SCURTYSTR_WPA2WPA3_PERS        "WPA2/WPA3-Personal"
#define SSID_SCURTYSTR_WPAWPA2_ENT          "WPA/WPA2-Enterprise" 

#define SSID_SCURTYSTR_WPA_PSK_TKIP         "WPA-PSK-TKIP"
#define SSID_SCURTYSTR_WPA_PSK_AES          "WPA-PSK-AES"
#define SSID_SCURTYSTR_WPA2_PSK_TKIP        "WPA2-PSK-TKIP"
#define SSID_SCURTYSTR_WPA2_PSK_AES         "WPA2-PSK-AES"
#define SSID_SCURTYSTR_WPAWPA2_PSK_MIXED2   "WPA-PSK-TKIP|WPA2-PSK-AES"

#define SCURTYSTR_NONE                      "none"  
#define SCURTYSTR_PSK_TKIP                  "psk+tkip"
#define SCURTYSTR_PSK_CCMP                  "psk+ccmp"
#define SCURTYSTR_PSK2_TKIP                 "psk2+tkip"
#define SCURTYSTR_PSK2_CCMP                 "psk2+ccmp"
#define SCURTYSTR_PSK2_MIXED                "psk-mixed+tkip+ccmp"
#define SCURTYSTR_SAE                       "sae"
#define SCURTYSTR_SAE_MIXED                 "sae-mixed"
#define SCURTYSTR_WPAWPA2_ENT               "wpa-mixed+tkip+ccmp"

// get from wpa_supplicant.conf by wps
#define WPS_WPA_VERSION_WPA3SAE             "WPA3-SAE"
#define WPS_WPA_VERSION_WPA3PSK             "WPA3-PSK"
#define WPS_WPA_VERSION_WPA2PSK             "WPA2-PSK"
#define WPS_WPA_VERSION_WPA2PSK_SHA256      "WPA2-PSK-SHA256"
#define WPS_WPA_VERSION_WPAPSK              "WPA-PSK"
#define WPS_WPA_VERSION_NONE                "NONE"
#define WPS_WPA_VERSION_WPAWPA2PSK_MIXED    "WPAWPA2_PSK_MIXED"
#define WPS_WPA_VERSION_WPA2WPA3PSK_MIXED   "WPA2WPA3_PSK_MIXED"

#define RETRY_SCANRESULT_CNT                10

typedef enum {
    SCURTY_INVALID = 0,
    SCURTY_OPEN = 1,
    SCURTY_WEP = 2,
    SCURTY_WPA_PSK_TKIP = 3,
    SCURTY_WPA_PSK_AES = 4,
    SCURTY_WPA2_PSK_TKIP = 5,
    SCURTY_WPA2_PSK_AES = 6, 
    SCURTY_WPA_WPA2_PSK = 7,
    SCURTY_WPA2_Personal = 8,
    SCURTY_WPA_WPA2_Personal = 9,
    SCURTY_WPA3_Personal = 10,
    SCURTY_WPA2_WPA3_Personal = 11,
    SCURTY_WPA_WPA2_Enterprise = 12
} SSID_SCURTY_MODE_E;

typedef struct {
    SSID_SCURTY_MODE_E      mode;
    char                    name[BUFLEN_32]; // "WPA-PSK-TKIP" "WPA-PSK-AES" "WPA2-PSK-TKIP" "WPA-PSK-TKIP|WPA2-PSK-AES"...
    char                    scurty_str[BUFLEN_32]; // "none" "psk+tkip" "psk+ccmp" "psk2+tkip" "psk2+ccmp" "psk-mixed+tkip+ccmp" "sae" "sae-mixed"...
} SSID_SCURTY_T;

typedef enum {
    REGION_ZAF = 0,     /* Africa */
    REGION_THA,         /* Asia */
    REGION_AUS,         /* Australia */
    REGION_CAN,         /* Canada */
    REGION_DEU,         /* Europe */
    REGION_ISR,         /* Israel (MUST be removed from tri-band extender) */
    REGION_JPN,         /* Japan */
    REGION_KOR,         /* Korea */
    REGION_MEX,         /* Mexico */
    REGION_BRA,         /* South America */
    REGION_USA,         /* United States */
    REGION_CHN,         /* China */
    REGION_IND,         /* India */
    REGION_MYS,         /* Malaysia */
    REGION_TUR = 16,    /* Turkey */
    REGION_SAU,         /* Saudi Arabia */
    REGION_ARE,         /* United Arab Emirates */
    REGION_RUS,         /* Russia */
    REGION_SGP,         /* Singapore */
    REGION_TWN,         /* Taiwan */
    REGION_LBN,         /* Middle East (Iran/Lebanon/Qatar)  (MUST be removed from tri-band extender) */
    REGION_DZA,         /* Middle East (Algeria/Syria/Yemen)  (MUST be removed from tri-band extender) */
    REGION_EGY,         /* Middle East (Egypt/Tunisia/Kuwait)  (MUST be removed from tri-band extender) */
    REGION_HKG,         /* Hong Kong*/
} WIFI_REGION_ID_E;
    

typedef struct {
    WIFI_REGION_ID_E  id;
    char            thr_code[4];
    char            two_code[4];
    char            full_name[64];
} WIFI_REGION_T;

typedef struct {
    char            restrict_name[64];
    UINT32          list_len;
    WIFI_REGION_ID_E  region_list[64];
} WIFI_REGION_RESTRICT_T;


#define LINK_RATE_AUTO  "Auto"
#define LINK_RATE_54M   "54Mbps"
#define LINK_RATE_150M  "150Mbps"
#define LINK_RATE_216M  "216Mbps"
#define LINK_RATE_450M  "450Mbps"
#define LINK_RATE_542M  "542Mbps"
#define LINK_RATE_300M  "300Mbps"
#define LINK_RATE_600M  "600Mbps"
#define LINK_RATE_1200M "1200Mbps"
#define LINK_RATE_1300M "1300Mbps"

#define LINK_RATE_173M  "173Mbps"
#define LINK_RATE_286M  "286Mbps" 
#define LINK_RATE_400M  "400Mbps"
#define LINK_RATE_867M  "867Mbps" 

#define LINK_RATE_1733M  "1733Mbps" 
#define LINK_RATE_2400M  "2400Mbps"

/* wifi5 3*3*/
#define LINK_RATE_2340M  "2340Mbps" // VHT160
#define LINK_RATE_288M  "288Mbps" 

/* wifi6 3x3*/
#define LINK_RATE_573M  "573Mbps"

#define LINK_RATE_430M  "430Mbps"
#define LINK_RATE_860M  "860Mbps"
#define LINK_RATE_1800M  "1800Mbps"
#define LINK_RATE_3600M  "3600Mbps"

/* wifi7 3x3*/
#define LINK_RATE_344M  "344Mbps"
#define LINK_RATE_688M  "688Mbps"

#define LINK_RATE_516M  "516Mbps"
#define LINK_RATE_1032M  "1032Mbps"
#define LINK_RATE_2160M  "2160Mbps"
#define LINK_RATE_4323M  "4323Mbps"

/* wifi7 2x2*/
#define LINK_RATE_1440M  "1440Mbps"
#define LINK_RATE_2882M  "2882Mbps"

// htmode for 5G
#define HTMODE_5G_EHT160 "EHT160"
#define HTMODE_5G_EHT80  "EHT80"
#define HTMODE_5G_EHT40  "EHT40"
#define HTMODE_5G_EHT20  "EHT20"

// htmode for 2G
#define HTMODE_2G_EHT40 "EHT40"
#define HTMODE_2G_EHT20 "EHT20"

// htmode for 54Mbps
#define HTMODE_NOHT "NOHT"


#define PROJECT_NAME_BE5000 "BE5000"
#define PROJECT_NAME_BE3600 "BE3600"
#define PROJECT_NAME_AX3000 "AX3000"
#define PROJECT_NAME_AX1800 "AX1800"
#define PROJECT_NAME_AX1500 "AX1500"
#define PROJECT_NAME_AX1600 "AX1600"


typedef enum {
    RADIO_INVALID = 0,  /* Invalid Radio band */
    RADIO_2G,           /* DualBand extender: 2G Band radio */
    RADIO_5G,           /* DualBand extender: 5G Band radio; Triband extender: For 5G High Band radio */
    RADIO_5G1,          /* Triband extender: For 5G Low Band radio */
    RADIO_60G,          /* 60G radio */
} WIFI_RADIO_TYPE_E;

typedef struct {
    WIFI_RADIO_TYPE_E radio_type;
    char            band_name[8];
} WIFI_RADIO_T;

typedef enum {
    RATE_INVALID = 0,
    RATE_54M,
    RATE_150M,
    RATE_450M,
    RATE_542M,
    RATE_300M = 7,
    RATE_600M,
    RATE_1300M,
    //RATE_1134M,
    RATE_1200M,
    RATE_AUTO,
    RATE_173M = 15,
    RATE_286M,
    RATE_400M,
    RATE_867M,
    RATE_1733M,
    RATE_2400M,     //20
    RATE_573M,
    RATE_344M,
    RATE_688M,
    RATE_2340M,
    RATE_430M,      //25
    RATE_860M,
    RATE_1800M,
    RATE_3600M,
    RATE_516M,
    RATE_1032M,     //30
    RATE_2160M,
    RATE_4323M,
    RATE_288M,
    RATE_1440M,
    RATE_2882M,     //35
} WIFI_RATE_MODE_E;

typedef struct {
    WIFI_RADIO_TYPE_E radio;
    WIFI_RATE_MODE_E  rate;
    char              rate_name[BUFLEN_16];
    char              htmode[BUFLEN_16];
    int               stream_num;
} WIFI_MODE_T;

enum wlan_ifindex
{
    WLAN_IFINDEX_NONE = 0,
    WLAN_IFINDEX_APCLI2G = 1,
    WLAN_IFINDEX_APCLI5G = 2,
    WLAN_IFINDEX_AP12G = 3,
    WLAN_IFINDEX_AP15G = 4,
    WLAN_IFINDEX_APMLD1 = 5,
    WLAN_IFINDEX_STAMLD2 = 6,
};

// SSID and psk size
#define WLIF_SSID_MAX_SZ		32
#define WLIF_PSK_MAX_SZ			64
#define WLIF_SECURITY_MAX_SZ	32

typedef struct wps_nw_creds {
	char ssid[WLIF_SSID_MAX_SZ + /* '\0' */ 1];	// SSID.
	char nw_key[WLIF_PSK_MAX_SZ + /* '\0' */ 1];	// Network Key.
    char wpa_version[WLIF_SECURITY_MAX_SZ + /* '\0' */ 1];
	UINT8 akm;		// Key mgmt.
	UINT8 auth_type;	//  1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa-psk2
	UINT8 encr;		// 1: none, 2: wep, 4: tkip, 8: aes
	UINT8 invalid;		// Check for the validity of credentials
    UINT8 bss_ml_capable;
} wps_nw_creds_t;

typedef struct wlan_profile
{
    char ssid[WLIF_SSID_MAX_SZ+1];
    char security[WLIF_SECURITY_MAX_SZ+1];
    char key[WLIF_PSK_MAX_SZ+1];
}wlan_profile_t;
typedef struct __attribute__ ((__packed__))  _NDIS80211SSID
{
	UINT32	SsidLength;   // length of SSID field below, in bytes;
								  // this can be zero.
	UINT8   Ssid[32+1]; // SSID information field
} NDIS80211SSID;

// WSC configured credential
typedef struct  _WSC_CREDENTIAL
{
	NDIS80211SSID     SSID;       // mandatory
	UINT16            AuthType;   // mandatory, 1: open, 2: wpa-psk, 4: shared, 8:wpa, 0x10: wpa2, 0x20: wpa-psk2
	UINT16            EncrType;   // mandatory, 1: none, 2: wep, 4: tkip, 8: aes
	UINT8             Key[64];    // mandatory, Maximum 64 byte
	UINT16            KeyLength;
	UINT8             MacAddr[6];  // mandatory, AP MAC address
	UINT8             KeyIndex;    // optional, default is 1
	UINT8             bFromUPnP;   /* TRUE: This credential is from external UPnP registrar */
	UINT8             bss_role;	   /*0-Fronthaul, 1-Backhaul*/
	UINT8             DevPeerRole; /* Device role for the peer device sending M8 */
	UINT8             Rsvd[28];	
}   WSC_CREDENTIAL, *PWSC_CREDENTIAL;

// WSC configured profiles
typedef struct  _WSC_PROFILE
{
	UINT32          ProfileCnt;
	UINT32          ApplyProfileIdx;  // add by johnli, fix WPS test plan 5.1.1
	WSC_CREDENTIAL  Profile[8];			 // Support up to 8 profiles
}WSC_PROFILE, *PWSC_PROFILE;

#define WLAN_2G_DEV             "wireless.MT7992_1_1"
#define WLAN_5G_DEV             "wireless.MT7992_1_2"
#define WLAN_2G5G_DEV           "wireless.2g_5g"
#define WLAN_2G_APCLI_DEV       "wireless.apcli0"
#define WLAN_5G_APCLI_DEV       "wireless.apclii0"
#define WLAN_2G_FIRST_IF        "wireless.ra0"
#define WLAN_2G_SECOND_IF       "wireless.ra1"
#define WLAN_5G_FIRST_IF        "wireless.rai0"
#define WLAN_5G_SECOND_IF       "wireless.rai1"
#define WLAN_2G_IF_PREFIX       "wireless.ra"
#define WLAN_5G_IF_PREFIX       "wireless.rai"
#define WLAN_AP_MLD             "wireless.apmld1"
#define WLAN_STA_MLD            "wireless.stamld2"

//for rae
typedef struct
{
    UBOOL8 enable;                  // 使能
    UBOOL8 broadcast;               // broadcase使能
    UINT32 speed;                   // 速度 '单位Mbps'
    char ssid[BUFLEN_64];           // ssid
    char interface[BUFLEN_32];      // 接口名 'ra0' / 'ra1' / 'rax0' / 'rax1'
    char channel[BUFLEN_32];        // 信道 'AUTO' / '0' / '1' / ...
    char mode[BUFLEN_32];           // 工作模式 'AP' / 'Client' / 'STA'
    char securityType[BUFLEN_32];   // 加密模式 'None' / 'WEP64' / 'WEP128' / 'WPA-PSK' / 'WPA2-PSK' / 'WPA-PSK/WPA2-PSK' / 'WPA2-Personal' / 'WPA3-Personal' / 'WPA/WPA2-Personal' / 'WPA2/WPA3-Personal' / 'WPA/WPA2-Enterprise'
    char status[BUFLEN_32];         // 接口状态, 'up'/'down'
} WIFI_INTERFACE_INFO;


int wifiCfgDoAction(WLAN_ACTION_E action, char *key, char *value, UINT32 valueLen);
void dai_wifiReload();
void dai_wifiReloadAp(UINT32 band);
UINT32 dai_getWifiMode(char *modeString);
UINT32 dai_getWfiBw(char *bwString);
int dai_doScanAp(SINT32 band, UINT32 scanTime);
void dai_getScanApResult(SINT32 band, WLAN_SCANRET_T *scanRet, int maxBss);
void dai_wifiUpdateCfg();
void dai_connectRootAp(SINT32 band, SINT32 wlan2GIdx, SINT32 wlan5GIdx);
void dai_wifiStartWps(UINT32 method, char *pinNum, SINT32 band, SINT32 wlan2GIdx, SINT32 wlan5GIdx);
void dai_wifiStopWps(SINT32 band, SINT32 wlan2GIdx, SINT32 wlan5GIdx);
char *dai_convertEncryptTypeToStr(WLAN_ENCRYPTTYPE_E et);
char *dai_convertCipherTypeToStr(WLAN_CIPHERTYPE_E ci);
void dai_wifiACLCfg(char *strItem);
void dai_wifiClearACL();
UINT32 dai_setEnable(UINT32 band, UBOOL8 isEnable);
UINT32 dai_getAvailableChannel(UINT32 band);
UINT32 dai_getApCliChannel(UINT32 band);
int dai_getAvailableExtChannel(UINT32 band, char *extChanel, int *bandwith, int len);
int dai_getAvailableBandwith(UINT32 band, char *uciBandwith, char *curBandwith);

void dai_getWifiConfig(WLAN_CONFIG_T *wlanConfig);
int dai_setWifiConfig(WLAN_CONFIG_T wlanConfig);
void dai_getWifiDevConfig(UINT32 band, WLAN_DEV_CONFIG_T *wlanDevConfig);
int  dai_setWifiDevConfig(UINT32 band, WLAN_DEV_CONFIG_T wlanDevConfig);
void dai_getWifiIfConfig(UINT32 band, WLAN_IF_CONFIG_T *wlanIfConfig);
int  dai_setWifiIfConfig(UINT32 band, WLAN_IF_CONFIG_T wlanIfConfig);
void dai_getRootapConfig(UINT32 band, WLAN_ROOTAP_CONFIG_T *rootapConfig);
int dai_setRootapConfig(UINT32 band, WLAN_ROOTAP_CONFIG_T rootapConfig);
void dai_getAllSupportWirelessRegion(char *regionilst, UINT32 listLen);
UINT32 dai_getBandByString(char *bandString);
void dai_getAllSupportMode(UINT32 band, char *mode, UINT32 modeLen);
int dai_convertRegionCode2To3(const char *twoCode, char *thrCode, UINT32 thrCodeLen);
int dai_convertRegionCode3To2(const char *thrCode, char *twoCode, UINT32 twoCodeLen);
int dai_convertRegionCode2ToId(const char *twoCode, UINT32 *id);
int dai_convertRegionCodeIdTo2(UINT32 id, char *twoCode, UINT32 twoCodeLen);
int dai_convertRegionCode2ToFull(const char *twoCode, char *fullCode, unsigned int fullCodeLen);
void dai_getApcliLinkRate(SINT32 band, char *rate, UBOOL8 *isGbs);
int dai_setAPMode(void);

UBOOL8 dai_getLinkRootApStatus(SINT32 band);
void hal_getWlanIfName(SINT32 band, char *ifName, SINT32 ifNameLen, SINT32 wlan2GIdx, SINT32 wlan5GIdx);
int dai_wifiDevConfigGet(int band, const char *keySuffix, char *value, UINT32 valueLen);
void dai_wifiGuestACLCfg(char *strItem);
int dai_getWifiInterfaceInfo(int ssidIdx, WIFI_INTERFACE_INFO *conf);
void dai_countryCodeToRegion(char *pCountryCode, int pCountryCode_len,
                             char *pRegion, int pRegion_len,
                             char *pRegionCode, int pRegionCode_len);
void dai_translateUciSecurity(char *security, int *encryption, int *cipher);
int dai_getAPMode(void);
int dai_getFastlaneMode(char *fastlaneMode, int len);
int dai_getFastlaneBand(char *fastlaneBand, int len);
int dai_getSameSsid(void);
int dai_setSameSsid(int enable);
int dai_getSmartConnect(void);
int dai_setSmartConnect(int enable);
int dai_getWifi6(void);
int dai_setWifi6(int enable);
int dai_getWlanProfile(int band, wlan_profile_t *profile);
int dai_activeWlanProfile(int band, wps_nw_creds_t* wps_creds);
int dai_getWpsCreds(int band, wps_nw_creds_t* creds);
int dai_wlanIfIsUp(int band);
int dai_ifNameToIdx(char* ifname);
int dai_wpsPbcStart(int ifidx);
int dai_wpsPbcStop(int ifidx);
int hal_wpsPbcTrigger(char *ifname, int enable);
void hal_getWlanApCliName(SINT32 band, char *ifName, SINT32 ifNameLen);
int hal_getWpsCreds(char* ifname, wps_nw_creds_t* creds);
int dai_getNgrVIEByBssid(int band, char *rootApSsid, char* rootApBssid, char *vieHex, int vieLen);
int dai_deleteNgrVIE(int band);
int dai_updateNgrVIE(int band, char* rootApVie, int len, int connState, UBOOL8 apCliLinkUpHappen);
int dai_getApCliExistOption(int band);
int dai_apcliDown(int band);
UINT32 dai_setSameSec(UINT32 band, UBOOL8 isEnable);
void dai_getSameSSIDKeyStatus(char *pchSameSSID, char *pch5GSameSSID, char *pchSameKey,
                              char *pch5GSameKey, char *smartConnect);
void dai_getWlanSSIDKey(int ssidIdx, char *pchSSID, int ssidLen, char *pchEnc, int encLen, char *pchKey, int keyLen);
void dai_setWlanSSIDKey(int ssidIdx, char *pchSSID, char *pchEnc, char *pchKey);
void dai_getChannelList(UINT32 band, char *country, char *channelList, int listLen);
int dai_apcliAutoConnect(int band, int enable);
int dai_getRootApChannelAndBssidBySsid(SINT32 band, char *ssid, int ssidLen, int maxBss, int *channel,
                                       char *bssid, int bssidSize, int *mloCap);
void dai_setWifiChannelListByRegion(const char *region);
UBOOL8 dai_isApIfLinkUp(int ssidIdx);
int dai_getRootApBssid(int band, char* rootApSsid, int ssidLen, char *rootApBssid, int bssidLen);
UBOOL8 dai_is2GAnd5GSecuritySame();

void dai_getWiFiInfStatus(char* ifname, int* status);
void dai_wifiIfConfigGet(int ssidIdx, const char *keySuffix, char *value, UINT32 valueLen);
int dai_isMldEnabled(int mldMode);
int dai_configApMld(int ssidIdx);
int dai_configStaMld(int band);
int dai_mldEnableDisable(int mldMode, int enOrDis);
int dai_setApcliIntfaceEnable(UINT32 band, UINT32 EnableFlag);
int dai_getWifi7(void);
int dai_setWifi7(int enable);

void wifiIfConfigGet(int ssidIdx, const char *keySuffix, char *value, UINT32 valueLen);
void wifiIfConfigSet(int ssidIdx, const char *keySuffix, char *value);
void wifiConfigSet(const char *keySuffix, char *value);
void wifiConfigGet(const char *keySuffix, char *value, UINT32 valueLen);
void wifiDevConfigGet(int band, const char *keySuffix, char *value, UINT32 valueLen);
void wifiDevConfigSet(int band, const char *keySuffix, char *value);
void wifiApcliConfigGet(int band, const char *keySuffix, char *value, UINT32 valueLen);
void wifiApcliConfigSet(int band, const char *keySuffix, char *value);
void wifiDevConfigSet(int band, const char *keySuffix, char *value);
void wifiMldConfigGet(int mldMode, const char *keySuffix, char *value, UINT32 valueLen);
void wifiMldConfigSet(int mldMode, const char *keySuffix, char *value);
int dai_convertUciScurtyToNgrScurtyStr(char *uciScurtyStr, char *ngrScurtyStr, int StrLen);
#endif
