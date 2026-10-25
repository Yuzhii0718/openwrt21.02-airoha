#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "fwk/fwk.h"
#include <hiredis/hiredis.h>

#define NGR_LOGIN_TABLE             "ngr_login_table"
#define NGR_LOGIN_FAIL_NUM          "login_fail_num"
#define NGR_LOGIN_BlOCK_TIME        "login_block_time"
#define NGR_LOGIN_ENTER_SN_AGAIN    "enter_sn_again"
#define NGR_LOGIN_ENTER_SERIAL_NUM  "tmp_serial_num"

#define NGR_LOGIN_FIRST_BLOCK_TIME  60 /* 1 min */
#define NGR_LOGIN_SECOND_BLOCK_TIME  300 /* 5 min */


#define DAI_FENV_MAC  "mac"
#define DAI_FENV_SN  "sn"
#define DAI_FENV_MODEL  "model"
#define DAI_FENV_AXMODEL  "axmodel"
#define DAI_FENV_REGION  "region"
#define DAI_FENV_HARDWARE  "hardware"
#define DAI_FENV_FACTORY_MODE  "factory_mode"
#define DAI_FENV_DEFAULT_REGION  "default_region"
#define DAI_FENV_PCBA_SN  "pcba_sn"
#define DAI_FENV_HARDWARE_ID  "hardware_id"

#define DAI_UENV_LANGUAGE  "language"

#define DAI_INSTALL_METHOD_UNKNOWN  "unknown"
#define DAI_INSTALL_METHOD_APP      "APP"
#define DAI_INSTALL_METHOD_GUI      "GUI"

#define GUI_INSTALL_HIJACK_NA                          "NA"
#define GUI_INSTALL_HIJACK_PAGE_SEEN                   "Page Seen"
#define GUI_INSTALL_HIJACK_CONTINUE_GUI_INSTALL        "Continue GUI install"

#define SOAP_CONF_FILE    "/etc/soap/soap.conf"

#define DAI_LED_NAME_POWER  "power"
#define DAI_LED_NAME_RLINK  "rlink"
#define DAI_LED_NAME_CLINK  "clink"
#define DAI_LED_NAME_WPS  "wps"
#define DAI_LED_NAME_SPEED  "speed"
#define DAI_LED_ALL "all"
#define DAI_LED_NTGR_ALL "ntgr_all" // for NTGR should not close power led.

#define DAI_LED_COLOR_AMBER "amber"
#define DAI_LED_COLOR_RED "red"
#define DAI_LED_COLOR_GREEN "green"
#define DAI_LED_COLOR_YELLOW "yellow"

#define TWP_KEY_256     "he8-3+M!)d7=m~we2,q3a4d2n&3*Z^%8$"
#define TWP_DATA_FILE   "/etc/config/data/ajsatk"
#define TELNET_STATUS_FILE "/tmp/enable_telnet_debug"
#define UBOOT_UPGRADE_FAILSTR  "uboot upgrade fail"

#if defined(SUPPORT_MT7915)
#define SENSOR_TRIGGER_2G_ADDR  "820E02F0"
#define SENSOR_TRIGGER_5G_ADDR  "820F02F0"
#elif defined(SUPPORT_MT7981)
#define SENSOR_TRIGGER_2G_ADDR  "820E0030"
#define SENSOR_TRIGGER_5G_ADDR  "820F0030"
#elif defined(SUPPORT_MT7991)
#define SENSOR_TRIGGER_2G_ADDR  "820E0030"
#define SENSOR_TRIGGER_5G_ADDR  "820F0030"
#endif

typedef enum
{
    REBOOT_UNKNOWN = 0,
    REBOOT_KERNEL_CRASH,
    REBOOT_BUTTON,
    REBOOT_BUTTON_FACTORY,
    REBOOT_GUI_FACTORY,
    REBOOT_GUI,
    REBOOT_UPGRADE,
    REBOOT_IMPORT_CONFIG,
    REBOOT_SOAP
} DAI_REBOOT;

typedef struct {
    char answer[BUFLEN_128];
    int  type;
} ANSWER_CONFIG_T;

typedef struct {
    unsigned int total;
    unsigned free;
} DAI_MEM_INFO;

typedef struct {
    unsigned int all;
    unsigned idle;
} DAI_CPU_INFO;

typedef struct
{
    char    fwLastUpateTime[BUFLEN_32];
    char    fwLastCheckTime[BUFLEN_32];
    char    fwLastAutoUpgradeTime[BUFLEN_32];
    char    fwEventType[BUFLEN_32];
    char    fwUpgradeTriggered[BUFLEN_32];
    char    fwDownloadUrl[BUFLEN_128];
    char    fwDownloadUrlFromCloud[BUFLEN_128];
    char    fwConfigSetting[BUFLEN_128];
    char    routerCurrentVersion[BUFLEN_32];
    char    routerNewVersion[BUFLEN_32];
    char    fwCheckResult[BUFLEN_32];
    char    fwUpgradeMethod[BUFLEN_16];
    char    fwUpgradeLog[BUFLEN_128];
    UBOOL8  fwUpgRebootFlag;
    UBOOL8  fwAutoUpgradeEnable;
    UBOOL8  fwUpgradeLatest;
} DAI_UPG_INFO_T;

typedef struct
{
    char ntpResult[BUFLEN_16];
    char ntpFailReason[BUFLEN_128];
    char ntpPortNumber[BUFLEN_16];
    char timeZoneConfig[BUFLEN_16];
    char timeZoneOffset[BUFLEN_16];
} DAI_NTP_INFO;

typedef enum
{
    NET_WIZARD_STATE_BLANK,
    NET_WIZARD_STATE_NETWORK_CREATED,
    NET_WIZARD_STATE_CONFIGURED
} NET_WIZARD_STATE_E;

/**
 * \brief          AES context structure
 */
typedef struct
{
    unsigned long erk[64];     /*!< encryption round keys */
    unsigned long drk[64];     /*!< decryption round keys */
    int nr;                    /*!< number of rounds      */
} aes_context_t;


typedef  struct _image_head
{
    char model[32];        /*model name*/
    char region[32];       /*region*/
    char version[64];      /*version*/
    char dateTime[64];     /*date*/
    unsigned int productHwModel;  /*product hardware model*/
    char modelIndex;       /*model index - default 0:don't change model in nmrp upgrade - others: change model by index in nmrp upgrade*/
    char hwIdNum;          /*hw id list num*/
    char modelNum;         /*model list num*/
    char secure;           /* secure boot state */
    char reserved0[12];    /*reserved*/
    char modelHwInfo[200]; /*save hw id list and model list*/
    char reserved[100];    /*reserved space, if add struct member, please adjust this reserved size to keep the head total size is 512 bytes*/
} __attribute__((__packed__)) IMAGE_HEAD;

typedef  struct _image_tail
{
    char checkSum[4];       /*checkSum crc*/
} __attribute__((__packed__)) IMAGE_TAIL;

#define REDIS_UNIX_SOCK "/tmp/.redis.sock"

typedef struct
{
    char field[BUFLEN_64];
    char *value;
} REDIS_FIELD_ELEMENT_T;

int dai_getWebLoginUsername(char *username);
int dai_getWebLoginPassword(char *password);
int dai_setWebLoginUserPasswd(char *username, char *password);
int dai_getWebLoginRememberMe(char *rememberMe);
int dai_setWebLoginRememberMe(char *rememberMe);
int dai_getUserPasswdEnable(char *enable, int len);
int dai_setUserPasswdEnable(char *enable, int len);
int dai_getUserAnswerConfig(ANSWER_CONFIG_T *answer1, ANSWER_CONFIG_T *answer2);
int dai_setUserAnswerConfig(ANSWER_CONFIG_T *answer1, ANSWER_CONFIG_T *answer2);
int dai_judgeAnswerConfig(char *answer1, char *answer2);

time_t dai_getPasswordRecoveredTimestamp(void);
int dai_setPasswordRecoveredTimestamp(time_t ts);

int dai_sysGetLanguage(char* lang, int len);
int dai_sysSetLanguage(const char* lang);

int dai_sysSetLanguagesetLedEnable(UBOOL8 enable);
UBOOL8 dai_sysGetLanguagesetLedEnable(void);

int dai_sysCreateBackup(const char* path);
int dai_sysRestoreBackup(const char* path);

UBOOL8 dai_sysGetLedEnable(void);
int dai_sysSetLedEnable(UBOOL8 enable);
int dai_sysSetSingLedOnoff(const char *ledName, const char *color, const char *state);
int dai_sysSetLedOn(const char *ledName, const char *color);
int dai_sysSetLedOff(const char *ledName, const char *color);

/*Netgear POT api*/

int dai_sysSetPotValue(void);
int dai_sysGetPotValue(unsigned int *potvalue);

int dai_sysSetPotStamac(const char *stamac);
int dai_sysGetPotStamac(unsigned char *stamac);

int dai_sysSetPotNtptime(void);
int dai_sysGetPotNtptime(char *ntptime);
unsigned int dai_sysGetPotMaxValue(void);
UBOOL8 dai_sysGetScheduleEnable(void);
int dai_sysIsEnableSchedule(void);

int dai_sysGetModel(char *value, int valueLen);
int dai_sysGetModelSeries(char *value, int valueLen);
int dai_sysGetAxModel(char *value, int valueLen);
int dai_sysGetRegion(char *value, int valueLen);
int dai_sysGetSN(char *sn, int len);

int dai_sysGetFactoryEnv(const char *name, char *value, int valueLen);
int dai_sysSetFactoryEnv(const char *name, const char *value);

int dai_sysGetUserEnv(const char *name, char *value, int valueLen);
int dai_sysSetUserEnv(const char *name, const char *value);

int dai_sysRestoreFactory(void);
int dai_sysGetRestoreFactoryStatus(void);
int dai_sysIsOrNotRestoreStatus(void);

int dai_sysGetBootSeconds(void);
int dai_sysGetMemInfo(DAI_MEM_INFO *info);
int dai_sysGetCpuInfo(DAI_CPU_INFO *info, int idx);
int dai_setNetRaeAnalyticsInfoCfg(DAI_UPG_INFO_T *upgInfo);
int dai_getNetRaeAnalyticsInfoCfg(DAI_UPG_INFO_T *upgInfo);

int dai_getNtpInfo(DAI_NTP_INFO *ntp);
int dai_sysGetSoftwareVersion(char *ver, int len);

UBOOL8 dai_sysGetManageByGuiStatus(void);
int dai_sysSetManageByGuiStatus(UBOOL8 status);

UBOOL8 dai_sysGetManageByAppStatus(void);
int dai_sysSetManageByAppStatus(UBOOL8 status);

int dai_getGuiInstallHijack(char *method, int len);
int dai_setGuiInstallHijack(const char *method);

int dai_sysGetInstallMethod(char *method, int len);
int dai_sysSetInstallMethod(const char *method);

UBOOL8 dai_getInstallByWPS(void);
int dai_setInstallByWPS(UBOOL8 status);

int dai_sysSetFirstSetup(UBOOL8 firstSetup);
UBOOL8 dai_sysGetIsFirstSetup(void);
int dai_sysSetCaFinish(UBOOL8 caFinish);
UBOOL8 dai_sysGetCaFinish(void);
int dai_sysSetnetFirstSetRegion(UBOOL8 netFirstSetRegion);
UBOOL8 dai_sysGetnetFirstSetRegion(void);

int dai_sysSetAgreeCollectTC(UBOOL8 agree);
UBOOL8 dai_sysGetAgreeCollectTC(void);

int dai_sysSetNetWizardState(NET_WIZARD_STATE_E state);
NET_WIZARD_STATE_E dai_sysGetNetWizardState(void);
int dai_sysSetRegisterFlag(UBOOL8 state);
UBOOL8 dai_sysGetRegisterFlag(void);

int dai_sysGetRebootTime(void);

int dai_sysGetHostName(char *devName, int len);
int dai_sysSetHostName(char *devName);

int dai_sysGetProductFamily(char *model, int len);

int dai_sysGetSoapVersion(char *soapVer, int len);
int dai_sysGetSoapLoginMethod(char *soapLoginMethod, int len);

void dai_sysReboot(DAI_REBOOT reboot, int delay);

int dai_setFwLastUpateTime(char *upateTime);
int dai_getFwLastUpateTime(char *upateTime, int len);
int dai_setFwLastCheckTime(char *checkTime);
int dai_getFwLastCheckTime(char *checkTime, int len);
int dai_setFwLastAutoUpgradeTime(char *upgradeTime);
int dai_getFwLastAutoUpgradeTime(char *upgradeTime, int len);
int dai_setFwEventType(char *eventType);
int dai_getFwEventType(char *eventType, int len);
int dai_setFwUpgradeTriggerStatus(char *trigger);
int dai_getFwUpgradeTriggerStatus(char *trigger, int len);
int dai_getFwDownloadUrl(char *url, int len);
int dai_setFwConfigSetting(char *setting);
int dai_getFwConfigSetting(char *setting, int len);
int dai_setFwCurrentVersion(char *version);
int dai_getFwCurrentVersion(char *version, int len);
int dai_setFwNewVersion(char *version);
int dai_getFwNewVersion(char *version, int len);
int dai_setFwCheckResult(char *result);
int dai_getFwCheckResult(char *result, int len);
int dai_setFwUpgradeMethod(char *fwUpgradeMethod);
int dai_getFwUpgradeMethod(char *fwUpgradeMethod, int len);
int dai_setFwUpgradeLog(char *log);
int dai_getFwUpgradeLog(char *log, int len);
int dai_setFwUpgRebootFlag(UBOOL8 rebootFlag);
int dai_getFwUpgRebootFlag(char *rebootFlag, int len);
int dai_setAutoUpgrade(UBOOL8 enable);
int dai_getAutoUpgrade(char *enable, int len);
int dai_setFwUpgradeLatest(UBOOL8 fwUpgradeLatest);
int dai_getFwUpgradeLatest(UBOOL8 *fwUpgradeLatest, int len);
int dai_setPopupFlag(UBOOL8 enable);
int dai_getPopupFlag(char *enable, int len);
int dai_setFotaParametersChangeFlag(UBOOL8 changeFlag);
int dai_getFotaParametersChangeFlag(char *changeFlag, int len);
UBOOL8 dai_isHaveNewFwVersion(void);
void dai_getNewSwExStatus(UINT32 *pdwStatus, UINT32 *pdwCheckTime);
void dai_getNewSwExVerion(char *varValue);
void dai_getUpgradeInfo(char *varValue);
int dai_decryptImage(void);
int tbsMatch(const char *pszString, const char *pszPattern);

int dai_sysSetDefaultRegion(char *region);

int dai_sysGetDefaultRegion(char *region, int regionLen);

int dai_sysGetMac(char *mac, int macLen);

int dai_sysGetHardwareVersion(char *ver, int verLen);

int dai_sysGetHardwareId(char *hwid, int hwidLen);

int dai_sysGetRegionFlag(void);

int dai_sysSetTelnetEnable(UBOOL8 enable);

UBOOL8 dai_sysGetTelnetEnable(void);
int dai_setWebLogCapture(UBOOL8 logreadCapture);
int dai_getWebLogCapture(char *logreadCapture, int len);

unsigned int dai_calcSingleByteCrc32(char data, unsigned int preCrc);
void dai_stringHashSha256(char *in, char *out);

int dai_getSensorTriggered(const char *intf, UBOOL8 *triggered);

int dai_setPdata(char* keyStr, int len);

int dai_getPdata(char* keyStr, int len);

UBOOL8 dai_sysGetIsFirstSetupDone(void);
UBOOL8 dai_sysSetIsFirstSetupDone(UBOOL8 done);

int dai_getDebugFlag(char* status);
int dai_setDebugFlag(UBOOL8 status);
int dai_getCFUAutoCheckURL(char *url, int len);

/**
 * @brief Create or return a rediscontext
 * 
 * @return redisContext* 
 */
redisContext* dai_redisConnectInit();

/**
 * CMD> GET key
 * @param key redis key
 * @param value value
 * @param len  vaule length
*/
int dai_redisGet(char *key, char *value, int len);

/**
 * CMD> SET key value
 * CMD> EXPIRE key seconds
 * @param key redis key
 * @param value value
 * @param timeout timeout
*/
int dai_redisSet(char *key, char *value, int timeout);

/**
 * CMD> DEL key
 * @param key redis key
*/
int dai_redisDelete(char *key);

/**
 * CMD> HMGET key filed [field ...]
 * @param num        -- field num
 * @param fieldTable -- field, need to free the value.
 * @param key        -- redis key
 * @param ...        -- the other fields
*/
int dai_redisHmget(int num, REDIS_FIELD_ELEMENT_T *fieldTable, char *key, ...);

/**
 * Free the field value
 * @param num        -- field num
 * @param fieldTable -- field, need to free the value.
*/
void dai_redisFreeField(int num, REDIS_FIELD_ELEMENT_T *fieldTable);

/**
 * CMD> HMSET key filed value [field value ...]
 * @param num        -- (field, value) num
 * @param key        -- redis key
*/
int dai_redisHmset(int num, char *key, ...);

/**
 * CMD> HDEL key filed [field ...]
 * @param num        -- field num
 * @param key        -- redis key
*/
int dai_redisHdel(int num, char *key, ...);

int dai_sysGetWebloginfailedNum(char *value, int valueLen);
int dai_sysSetWebloginfailedNum(int num);
int dai_sysSetWebloginblocktimer(long time);
int dai_sysSetWebResetpwd_enter_sn_again(int enter);
int dai_sysGetWebResetpwd_enter_sn_again(char *value, int valueLen);
UBOOL8 dai_sysGetGuiLogin_Redirect(void);
int dai_sysGetWeb_try_serial_num(char *value, int valueLen);
int dai_sysSetWeb_try_serial_num(char *val);
int dai_DelGuiLogin_table();

#endif /* __SYSTEM_H__ */
