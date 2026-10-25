#ifndef __SOAPUTILS_H__
#define __SOAPUTILS_H__

#include "shutils.h"

#define SOAP_METHOD_SMALL_LEN 1024 * 10

#define SOAP_METHOD_MEDIUM_LEN 1024 * 20

#define SOAP_METHOD_MAX_LEN 1024 * 30

#define SOAP_METHOD_LARGE_LEN 1024 * 236 //GetConfigInfo - the size of file about 200k

#define SOAP_RESP_MAX_LEN 1024 * 256 //GetConfigInfo - the size of file about 200k

#define SOAP_SRV_DEV_CFG "DeviceConfig"
#define SOAP_SRV_VOIP_CTRL "VoiceControl"
#define SOAP_SRV_DEV_INFO "DeviceInfo"
#define SOAP_SRV_WAN_IP "WANIPConnection"
#define SOAP_SRV_WAN_3G "WAN3GinterfaceConfig"
#define SOAP_SRV_WAN_ETH "WANEthernetLinkConfig"
#define SOAP_SRV_LAN_SEC "LANConfigSecurity"
#define SOAP_SRV_WLAN_CFG "WLANConfiguration"
#define SOAP_SRV_WLAN6_CFG "60GWLANConfiguration"
#define SOAP_SRV_WLAN_ARLO "Arlo in WLANConfiguration"
#define SOAP_SRV_TIME "Time"
#define SOAP_SRV_PC "ParentalControl"
#define SOAP_SRV_SMESH "SmartMesh"
#define SOAP_SRV_AVS_AYC "AVS Analytic"
#define SOAP_SRV_APP_SYS "AppSystem"
#define SOAP_SRV_ADV_QOS "AdvancedQoS"
#define SOAP_SRV_UOP_TC "UserOptionsTC"

#define UNDEFINED_SERVICE "UndefinedService"
#define UNDEFINED_METHOD "UndefinedMethod"

#define SOAPACTION_SERVICE ":service:" //"urn:NETGEAR-ROUTER:service:DeviceInfo:1#GetInfo"

typedef enum
{
    SOAP_RESP_OK = 0,                /* Success */
    SOAP_RESP_ERROR = 1,             /* Unknown Error */
    SOAP_RESP_INVALID_PARAMETER = 2, /* invalid parameter */
    SOAP_RESP_INVALID = 10,          /* Invalid Argument (Extender only) */
    SOAP_RESP_400 = 400,             /* Operation or configuration is not available in current device mode */
    SOAP_RESP_401 = 401,             /* Unauthenticated */
    SOAP_RESP_402 = 402,             /* For Extender: Authentication Timeout; For other products: Invalid argument */
    SOAP_RESP_404 = 404,             /* API not support */
    SOAP_RESP_405 = 405,             /* Invalid MAC address argument */
    SOAP_RESP_409 = 409,             /*Timezone auto configured by active timezone detection, so, reject this passive configuration flow */
    SOAP_RESP_423 = 423,             /* Too many invalid login attempts, need to wait 30 seconds to re-login */
    SOAP_RESP_501 = 501,             /* Action failed */
    SOAP_RESP_503 = 503,             /* Server busy */
    SOAP_RESP_702 = 702,             /* Value specified is over max rule count */
    SOAP_RESP_705 = 705,             /* Value specified is over max rule count */
    SOAP_RESP_707 = 707,             /* String argument is conflict */
    SOAP_RESP_708 = 708,             /* String arguments are the same */
    SOAP_RESP_710 = 710,             /* System time is not synchronized or not available for this configuration */
    SOAP_RESP_900 = 900,             /* If incorrect admin or password, or any other failures (for old api v1.6 v1.3) */
} NTGR_SOAP_RESP;

typedef enum
{
    SOAPPLT_RET_SUCCESS = 0,              /**<Success. */
    SOAPPLT_RET_RESOURCE_EXCEEDED = 9004, /**< Resource exceeded.*/

} SOAPPLT_RET_STATUS;

typedef enum
{
    BOOL_FALSE = 0,
    BOOL_TRUE,
} NTGR_BOOL;

typedef uint8_t UBOOL8;
typedef uint32_t UINT32;
typedef int32_t SINT32;

#define BUFLEN_1 1       //!< buffer length 1
#define BUFLEN_4 4       //!< buffer length 4
#define BUFLEN_6 6       //!< buffer length 8
#define BUFLEN_8 8       //!< buffer length 8
#define BUFLEN_12 12     //!< buffer length 12
#define BUFLEN_16 16     //!< buffer length 16
#define BUFLEN_18 18     //!< buffer length 18 -- for ppp session id
#define BUFLEN_24 24     //!< buffer length 24 -- mostly for password
#define BUFLEN_32 32     //!< buffer length 32
#define BUFLEN_40 40     //!< buffer length 40
#define BUFLEN_48 48     //!< buffer length 48
#define BUFLEN_64 64     //!< buffer length 64
#define BUFLEN_80 80     //!< buffer length 80
#define BUFLEN_128 128   //!< buffer length 128
#define BUFLEN_256 256   //!< buffer length 256
#define BUFLEN_264 264   //!< buffer length 264
#define BUFLEN_512 512   //!< buffer length 512
#define BUFLEN_1024 1024 //!< buffer length 1024
#define BUFLEN_1518 1518 //!< buffer length 1518
#define BUFLEN_2048 2048 //!< buffer length 2048
#define BUFLEN_4096 4096 //!< buffer length 4096

void SoapApi_BuildBodyResp(webs_t wp, int respCode, const char *soapSrv, const char *methName, char *respMsg);

void SoapApi_BuildBodyNewResp(webs_t wp, int respCode, int newRespCode, const char *soapSrv, const char *methName, char *respMsg);

void CategoryInit_DeviceConfig(void);
void CategoryInit_DeviceInfo(void);
void CategoryInit_WLANConfiguration(void);
void CategoryInit_AdvancedQoS(void);
void CategoryInit_UserOptionsTC(void);
void CategoryInit_ParentalControl(void);

int soap_xml_special_char_convert(char *src, char *dst, int length);

UBOOL8 soap_first_smart_setup_enable(void);
void soap_response_time_init(void);
void soap_response_time_set(int nResponseTime);
int soap_response_time_get(void);
void srand_random(char *pBuff, int len);

// ===== platform related apis. =======
// soapsecurity.c
int soapPlt_compareWebLoginPassword(char *password);
int soapPlt_compareWebLoginUsername(char *username);
int soapPlt_getSHA256(unsigned char *buf, char *key, unsigned char *token, int tokenLen);
int soapPlt_resetPasswordHandler(char *pSerialNumber, int iNewSecurityQuestion1, char *pNewAnswer1,
                                 int iNewSecurityQuestion2, char *pNewAnswer2, char *pNewPassword);

// soap_platform_xxx
int soapPlt_init(int iShmID, int loglevel);
void soapPlt_cleanup();

// cgi_soap.c
int soapPlt_requestPrepare(webs_t wp);
int soapPlt_takeEffect();
int soapPlt_requestCacheHandle(webs_t wp, int init);
int soapPlt_raeSetManageByApp(UBOOL8 byApp);

// log, 用之前一定要对该log_func进行初始化，否则会出问题

typedef enum
{
    SOAPPLT_LOG_LEVEL_ERR = 3,    /**< Message at error level. */
    SOAPPLT_LOG_LEVEL_NOTICE = 5, /**< Message at notice level. */
    SOAPPLT_LOG_LEVEL_DEBUG = 7   /**< Message at debug level. */
} SOAPPLT_LOG_LEVEL;

typedef struct
{
    int loglevel;
    void (*logfunc)(int level, const char *func, UINT32 lineNum, const char *fmt, ...);
} SOAPPLT_LOG;

extern SOAPPLT_LOG soapLog;

#define soapLog_error(args...) soapLog.logfunc(SOAPPLT_LOG_LEVEL_ERR, __FUNCTION__, __LINE__, args)
#define soapLog_notice(args...) soapLog.logfunc(SOAPPLT_LOG_LEVEL_NOTICE, __FUNCTION__, __LINE__, args)
#define soapLog_debug(args...) soapLog.logfunc(SOAPPLT_LOG_LEVEL_DEBUG, __FUNCTION__, __LINE__, args)
// #define soapLog_custom(fmt, ...)
// #define soapLog_error(fmt, ...) printf("[%s.%s.%d] ====== " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
// #define soapLog_notice(fmt, ...) printf("[%s.%s.%d] ====== " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
// #define soapLog_debug(fmt, ...) printf("[%s.%s.%d] ====== " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define soapLog_custom(fmt, ...) printf("[%s.%s.%d] ====== " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

//#define DBG_MSG(args...) soapLog.logfunc(SOAPPLT_LOG_LEVEL_DEBUG, __FUNCTION__, __LINE__, args)

//just for abstract, it should be realized by util_xxx or cmsUtl_xxx
SINT32 soapPlt_strcmp(const char *s1, const char *s2);
SINT32 soapPlt_strncmp(const char *s1, const char *s2, SINT32 n);
SINT32 soapPlt_strlen(const char *src);
char *soapPlt_strncpy(const char *func, UINT32 line, char *dest, const char *src, UINT32 dlen);
char *soapPlt_strncat(const char *func, UINT32 line, char *dest, const char *src, SINT32 dlen);
//SINT32 soapPlt_snprintf(const char *func, UINT32 line, char *str, UINT32 size, const char *format, ...);

#define SOAPPLT_STRNCPY(dest, src, n) soapPlt_strncpy(__FUNCTION__, __LINE__, dest, src, n)
#define SOAPPLT_STRNCAT(dest, src, n) soapPlt_strncat(__FUNCTION__, __LINE__, dest, src, n)
//#define SOAPPLT_SNPRINTF(str, size, format, ...) soapPlt_snprintf(__FUNCTION__, __LINE__, str, size, format, ##__VA_ARGS__)
#define SOAPPLT_SNPRINTF snprintf

#define SOAPPLT_ALLOC_ZEROIZE 0x01

void *soapPlt_alloc(UINT32 size, UINT32 allocFlags, const char *function, UINT32 line);
#define SOAPPLT_MALLOC(size) soapPlt_alloc(size, SOAPPLT_ALLOC_ZEROIZE, __FUNCTION__, __LINE__)
#define SOAPPLT_MALLOC_FLAGS(size, flags) soapPlt_alloc(size, SOAPPLT_ALLOC_ZEROIZE, __FUNCTION__, __LINE__)

void soapPlt_free(void *buf, const char *function, UINT32 line);
#define SOAPPLT_FREE(buf) soapPlt_free(buf, __FUNCTION__, __LINE__)

#endif // __SOAPUTILS_H__
