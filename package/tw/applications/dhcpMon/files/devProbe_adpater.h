#ifndef __DEV_PROBE_ADPATER_H__
#define __DEV_PROBE_ADPATER_H__

typedef struct{
    char channel[32];
    char rssi[32];
    char rate[32];
    char reptMac[32];
}DP_WIFI_DEV_T;

#define MAC_ADDR_LEN    6     //!< Mac address len in an array of 6 bytes
#define MAC_STR_LEN     17    //!< Mac String len with ":". eg: xx:xx:xx:xx:xx:xx
#define ALLOC_ZEROIZE          0x01

typedef unsigned int UINT32;
typedef int32_t    SINT32;
typedef unsigned char UBOOL8;
typedef unsigned char UINT8;
typedef unsigned int UINT32;
#define TRUE  (1)
#define FALSE  (0)

extern void dp_getDevName(const char *macAddr, char *devName, UINT32 strLen);
extern void dp_getDevWifiInfo(const char *mac, const char *ifName, DP_WIFI_DEV_T *devInfo);
extern int dp_macStrToNum(const char *macStr, UINT8 *macNum);
extern int dp_getIntfLinkRate(const char *ifName);
UBOOL8 dp_getReptMac(const char *mac, UINT8 *reptMac, int macLen);
#endif /* __DEV_PROBE_ADPATER_H__ */
