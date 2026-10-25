#ifndef __WLANCONFIG_H_
#define __WLANCONFIG_H_

#define M_WLANCFG_SETENABLE "SetEnable"
#define M_WLANCFG_SET5GENABLE "Set5GEnable"
#define M_WLANCFG_SET5G1ENABLE "Set5G1Enable"
#define M_WLANCFG_IS5GSUPPORTED "Is5GSupported"
#define M_WLANCFG_SETWLANNOSECURITY "SetWLANNoSecurity"
#define M_WLANCFG_SETWLANWPAPSKBYPASSPHRASE "SetWLANWPAPSKByPassphrase"
#define M_WLANCFG_SET5GWLANNOSECURITY "Set5GWLANNoSecurity"
#define M_WLANCFG_SET5G1WLANNOSECURITY "Set5G1WLANNoSecurity"
#define M_WLANCFG_SET5GWLANWPAPSKBYPASSPHRASE "Set5GWLANWPAPSKByPassphrase"
#define M_WLANCFG_SET5G1WLANWPAPSKBYPASSPHRASE "Set5G1WLANWPAPSKByPassphrase"
#define M_WLANCFG_GETINFO "GetInfo"
#define M_WLANCFG_GETWPASECURITYKEYS "GetWPASecurityKeys"
#define M_WLANCFG_GET5GWPASECURITYKEYS "Get5GWPASecurityKeys"
#define M_WLANCFG_GET5G1WPASECURITYKEYS "Get5G1WPASecurityKeys"
#define M_WLANCFG_GET5GINFO "Get5GInfo"
#define M_WLANCFG_GET5G1INFO "Get5G1Info"
#define M_WLANCFG_GETAVAILABLECHANNEL "GetAvailableChannel"
#define M_WLANCFG_GETSUPPORTMODE "GetSupportMode"
#define M_WLANCFG_SETEXTENDERMODE "SetExtenderMode"
#define M_WLANCFG_GETEXTENDERMODE "GetExtenderMode"
#define M_WLANCFG_SETWIRELESSREGION "SetWirelessRegion"
#define M_WLANCFG_GETWIRELESSREGION "GetWirelessRegion"
#define M_WLANCFG_GETWIRELESSREGIONLIST "GetWirelessRegionList"
#define M_WLANCFG_GETAPINFOBYRADIOSSTART "GetAPInfoByRadiosStart"
#define M_WLANCFG_GETAPINFOBYRADIOSRESULT "GetAPInfoByRadiosResult"
#define M_WLANCFG_SETROUTERWLANNOSECURITY "SetRouterWLANNoSecurity"
#define M_WLANCFG_SETROUTERWLANWPAPSKBYPASSPHRASE "SetRouterWLANWPAPSKByPassphrase"
#define M_WLANCFG_GETROUTERWLANINFO "GetRouterWLANInfo"
#define M_WLANCFG_GETROUTERWLANVERIFYRESULT "GetRouterWLANVerifyResult"
#define M_WLANCFG_GETROUTERWLANWPAINFO "GetRouterWLANWPAInfo"
#define M_WLANCFG_SETAXMODEENABLED "SetAXModeEnabled"
#define M_WLANCFG_GETAXMODEENABLEDSTATUS "GetAXModeEnabledStatus"
#define M_WLANCFG_ISSMARTCONNECTENABLED "IsSmartConnectEnabled"
#define M_WLANCFG_SETSMARTCONNECTENABLED "SetSmartConnectEnable"

#define RADIO_CHANNEL_AUTO 0
#define RADIO_CHANNEL_2G_MIN 1
#define RADIO_CHANNEL_2G_MAX 14

#define RADIO_CHANNEL_5G_MIN 32
#define RADIO_CHANNEL_5G_MAX 165

#define APINFO_RADIO_LEN_MAX 32

typedef enum
{
    ZERO_BAND = 0, /* Not supported */
    ONE_BAND,      /* Supported (one band) */
    TWO_BAND,      /* Supported (two bands) */
} BAND_5G_NUM;

#define RADIO_BW_20M "20MHz"
#define RADIO_BW_40M "40MHz"
#define RADIO_BW_80M "80MHz"
#define RADIO_BW_160M "160MHz"
#define RADIO_BW_AUTO "Auto"

#define REGION_LEN_MAX 64

#define SSID_NAME_LEN_MAX 32

#define SSID_WPA_PWD_LEN_MIN 8
#define SSID_WPA_PWD_LEN_MAX 64

#define SSID_WEP_AUTH_OPEN "Open"
#define SSID_WEP_AUTH_SHARED "Shared"
#define SSID_WEP_AUTH_AUTO "Automatic"
#define SSID_ENCMODE_NONE "None"
#define SSID_ENCMODE_WEP "WEP"
#define SSID_ENCMODE_WPA_PSK "WPA-PSK"
#define SSID_ENCMODE_WPA2_PSK "WPA2-PSK"
#define SSID_ENCMODE_WPAWPA2_PSK_MIXED "WPA-PSK/WPA2-PSK"
//if firmware supports WPA3
#define SSID_ENCMODE_WPA2_PERS "WPA2-Personal"
#define SSID_ENCMODE_WPA3_PERS "WPA3-Personal"
#define SSID_ENCMODE_WPAWPA2_PERS "WPA/WPA2-Personal"
#define SSID_ENCMODE_WPA2WPA3_PERS "WPA2/WPA3-Personal"
#define SSID_ENCMODE_WPAWPA2_ENT "WPA/WPA2-Enterprise"

#define SSID_ENCMODE_WPA_PSK_TKIP "WPA-PSK-TKIP"
#define SSID_ENCMODE_WPA_PSK_AES "WPA-PSK-AES"
#define SSID_ENCMODE_WPA2_PSK_TKIP "WPA2-PSK-TKIP"
#define SSID_ENCMODE_WPA2_PSK_AES "WPA2-PSK-AES"
#define SSID_ENCMODE_WPAWPA2_PSK_MIXED2 "WPA-PSK-TKIP|WPA2-PSK-AES"

typedef enum
{
    EXT_INVALID = 0,
    EXT_IS_2GEXT_5GEXT_ETH5G,
    EXT_IS_2GEXT_5GAP,
    EXT_IS_2GAP_5GEXT,
    EXT_FL_2GSTA_5GAP,
    EXT_FL_2GAP_5GSTA,
} WIFI_EXTENDER_TYPE;

int soapPlt_set2GEnable(int iSSIDEnable);
int soapPlt_set5GEnable(int iSSIDEnable);
int soapPlt_set5G1Enable(int iSSIDEnable);
int soapPlt_setWLANNoSecurity(char *pNewSSID, char *pNewRegion, char *pNewChannel, char *pNewWirelessMode);
int soapPlt_set5GWLANNoSecurity(char *pNewSSID, char *pNewRegion, char *pNewChannel, char *pNewWirelessMode);
int soapPlt_setWLANWPAPSKByPassphrase(char *pNewSSID, char *pNewRegion, char *pNewChannel, char *pNewWirelessMode, char *pNewWPAEncryptMode, char *pNewWPAPwd);
int soapPlt_set5GWLANWPAPSKByPassphrase(char *pNewSSID, char *pNewRegion, char *pNewChannel, char *pNewWirelessMode, char *pNewWPAEncryptMode, char *pNewWPAPwd);

typedef struct
{
    int iEnable;
    int iBroadcast;
    char ssidStatus[16];
    char ssidName[256];
    int iRgnCode;
    char rgnName[8];
    char channel[8];
    char wirelssMode[32];
    char basicEncrpMode[32];
    char wepAuthType[16];
    char wpaEncrpMode[32];
    char macAddr[32];
} SOAPPLT_WIFI_INFO;

int soapPlt_getWifiInfo(SOAPPLT_WIFI_INFO *pWifiInfo);
int soapPlt_get5GWifiInfo(SOAPPLT_WIFI_INFO *pWifiInfo);

int soapPlt_getWPASecurityKeys(char *wpaKey, int bufLen);
int soapPlt_get5GWPASecurityKeys(char *wpaKey, int bufLen);

int soapPlt_getAvailableChannel(char *pNewBand, char *avlCh, int bufLen);
int soapPlt_getSupportMode(char *pNewBand, char *spMode, int bufLen);

#define WIFI_MODE_EXT "Extender"
#define WIFI_MODE_AP "AP"
#define WIFI_MODE_STA "Station"

#define EXT_MODE_INTSURFING "Internet Surfing"
#define EXT_MODE_FASTLANE "FastLane"
#define EXT_MODE_BASIC "Basic"
#define EXT_MODE_AP "AP"

typedef struct
{
    char radio2GMode[16];
    char radio5GMode[16];
    char radio5G1Mode[16];
    char radio6GMode[16];
    char bondEth[16];
} SOAPPLT_EXTENDERMODE_INFO;

int soapPlt_setExtenderMode(int iExtType);
int soapPlt_getExtenderMode(SOAPPLT_EXTENDERMODE_INFO *pExtModeInfo);

int soapPlt_setWirelessRegion(char *pNewRegion);
int soapPlt_getWirelessRegion(char *rgnName, int bufLen);
int soapPlt_getWirelessRegionList(char *rgnList, int bufLen);

typedef struct
{
    int i2GRadio;
    int i5GRadio;
} SOAPPLT_APINFORADIOS_INFO;

int soapPlt_getAPInfoByRadiosStart(SOAPPLT_APINFORADIOS_INFO *pRadios, int *pScanTime);
int soapPlt_getAPInfoByRadiosResult(SOAPPLT_APINFORADIOS_INFO *pRadios, char *pScanResult, int bufLen);

int soapPlt_setRouterWlanInfo(char *pNewRadio, char *pNewSSID, char *pNewChannel, char *pNewWirelessMode, char *pNewWPAEncryptMode, char *pNewWPAPwd, char *pNewVerify, int *pVerifyCode);

typedef struct
{
    int iEnable;
    char ssidName[256];
    int iNewRate;
    char conStatus[32];
    char channel[8];
    char wirelssMode[16];
    char encryMode[32];
    char wlanMac[32];
} SOAPPLT_ROUTERWLAN_INFO;

int soapPlt_getRouterWLANInfo(char *pNewRadio, SOAPPLT_ROUTERWLAN_INFO *pRouterWlanInfo);

typedef struct
{
    int iEnable;
    char ssidName[256];
    int iNewRate;
    char conStatus[32];
    char channel[8];
    char wirelssMode[16];
    char encryMode[32];
    char wlanMac[32];
} SOAPPLT_ROUTERWLANVERIFY_INFO;
int soapPlt_getRouterWLANVerifyResult(char *pNewRadio, SOAPPLT_ROUTERWLANVERIFY_INFO *pRouterWlanVerify);

int soapPlt_getRouterWLANWPAInfo(char *pNewRadio, char *pWpaKey, int bufLen);

typedef struct 
{
    int iEnable;
    int i2GOFDMA;
    int i5GOFDMA;
} SOAPPLT_AXMODE_INFO;
int soapPlt_getAXModeEnabledStatus(SOAPPLT_AXMODE_INFO *pAxModeInfo);
int soapPlt_setAXModeEnabledStatus(SOAPPLT_AXMODE_INFO *pAxModeInfo);

int soapPlt_setSmartConnectEnable(int iEnable);
int soapPlt_isSmartConnectEnabled(int *pEnable);

#endif
