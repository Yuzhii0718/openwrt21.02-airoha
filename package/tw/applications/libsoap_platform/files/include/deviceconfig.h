#ifndef __DEVICECONFIG_H_
#define __DEVICECONFIG_H_

#define DEVCFG_RESPTIME_RESPFMT "<ResponseTime>%d</ResponseTime>\r\n"

#define DEVCFG_REBOOTTIME_RESPFMT "<RebootTime>%d</RebootTime>\r\n"

#define DEVCFG_GETINFO_RESPFMT "<BlankState>%d</BlankState>\r\n\
<NewTimeZone>%s</NewTimeZone>\r\n\
<NewDaylightSaving>%d</NewDaylightSaving>\r\n"

#define DEVCFG_CONFIGINFO_RESPFMT "<NewConfigFile>%s</NewConfigFile>\r\n"

#define DEVCFG_LEDSP_RESPFMT "<LightingLEDSupported>%d</LightingLEDSupported>\r\n"

#define DEVCFG_LEDSTATUS_RESPFMT "<CurrentLightingLEDOnOffStatus>%d</CurrentLightingLEDOnOffStatus>\r\n\
<LightingLEDBrightnessStatus>%d</LightingLEDBrightnessStatus>\r\n"

#define DEVCFG_TIMEZONEINFO_RESPFMT "<NewTimeZone>%s</NewTimeZone>\r\n\
<NewDaylightSaving>%d</NewDaylightSaving>\r\n\
<NewIndexValue>%d</NewIndexValue>\r\n"

#define DEVCFG_DEVICEIP_RESPFMT "<NewAddressMode>%s</NewAddressMode>\r\n\
<NewStaticAddress>%s</NewStaticAddress>\r\n\
<NewStaticSubnet>%s</NewStaticSubnet>\r\n\
<NewStaticGateway>%s</NewStaticGateway>\r\n\
<NewStaticDNS>%s</NewStaticDNS>\r\n"

#define DEVCFG_CHECKNEWFIRMWARE_RESPFMT "<CurrentVersion>%s</CurrentVersion>\r\n\
<NewVersion>%s</NewVersion>\r\n\
<ReleaseNote>%s</ReleaseNote>\r\n"

#define DEVCFG_UPDATENEWFIRMWARE_RESPFMT "<RebootTime>%d</RebootTime>\r\n"

#define M_DEVCFG_CONFIGURATIONSTARTED "ConfigurationStarted"
#define M_DEVCFG_CONFIGURATIONFINISHED "ConfigurationFinished"
#define M_DEVCFG_SETENABLE "SetEnable"
#define M_DEVCFG_REBOOT "Reboot"
#define M_DEVCFG_SETCONFIGURATION "SetConfiguration"
#define M_DEVCFG_LOADDEFAULT "Loaddefault"
#define M_DEVCFG_SETTIMEZONE "SetTimeZone"
#define M_DEVCFG_GETTIMEZONEINFO "GetTimeZoneInfo"
#define M_DEVCFG_GETINFO "GetInfo"
#define M_DEVCFG_GETCONFIGINFO "GetConfigInfo"
#define M_DEVCFG_CHECKAPPNEWFIRMWARE "CheckAppNewFirmware"
#define M_DEVCFG_CHECKNEWFIRMWARE "CheckNewFirmware"
#define M_DEVCFG_UPDATENEWFIRMWARE "UpdateNewFirmware"
#define M_DEVCFG_CREATEADMIN "CreateAdmin"
#define M_DEVCFG_UPDATEADMINPASSWORD "UpdateAdminPassword"
#define M_DEVCFG_ISLIGHTINGLEDSUPPORTED "IsLightingLEDSupported"
#define M_DEVCFG_GETCURRENTLIGHTINGLEDSTATUS "GetCurrentLightingLEDStatus"
#define M_DEVCFG_GETCURRENTLIGHTINGLEDSTATUSALLSATELLITES "GetCurrentLightingLEDStatusAllSatellites"
#define M_DEVCFG_GETCONFIGLIGHTINGLEDSETTINGSALLSATELLITES "GetConfigLightingLEDSettingsAllSatellites"
#define M_DEVCFG_SETLIGHTINGLEDSETTINGS "SetLightingLEDSettings"
#define M_DEVCFG_GETCURRENTLIGHTINGLEDSCHEDULEALL "GetCurrentLightingLEDScheduleAll"
#define M_DEVCFG_GETCURRENTLIGHTINGLEDSCHEDULEALLSATELLITES "GetCurrentLightingLEDScheduleAllSatellites"
#define M_DEVCFG_SETLIGHTINGLEDSONBYSCHEDULE "SetLightingLEDsOnBySchedule"
#define M_DEVCFG_DELETELIGHTINGLEDONALLPERIOD "DeleteLightingLEDOnAllPeriod"
#define M_DEVCFG_SETLIGHTINGLEDONPERIODBYINDEX "SetLightingLEDOnPeriodByIndex"
#define M_DEVCFG_DELETELIGHTINGLEDONPERIODBYINDEX "DeleteLightingLEDOnPeriodByIndex"
#define M_DEVCFG_SOAPLOGIN "SOAPLogin"
#define M_DEVCFG_SOAPLOGOUT "SOAPLogout"
#define M_DEVCFG_SETDEVICEIPADDRESS "SetDeviceIPAddress"
#define M_DEVCFG_GETDEVICEIPADDRESS "GetDeviceIPAddress"

#define CFG_FIN_STS_APPLY "ChangesApplied"
#define CFG_FIN_STS_REBOOT "RebootRequired"

typedef enum
{
    BRGHT_STAT_OFF = 0,    /* OFF */
    BRGHT_STAT_QR = 25,    /* 25% */
    BRGHT_STAT_HALF = 50,  /* 50% */
    BRGHT_STAT_THR = 75,   /* 75% */
    BRGHT_STAT_FULL = 100, /* 100 */
} NTGR_BRGHT_STAT;

#define TIMEZONE_MIN -12
#define TIMEZONE_MAX 14

int soapPlt_configurationStarted(char *pSessionId);
int soapPlt_configurationFinished(char *pStatus, int *pRspTime);
int soapPlt_setEnable(int iEnable);
int soapPlt_setConfiguration(char *pNewConfigFile);
int soapPlt_loaddefault(int iDefault);
int soapPlt_getTimeZoneInfo(char *timeZone, int timeZoneLen, int *iDST, int *iNewIndexValue);
int soapPlt_setTimeZoneInfo(char *pNewTimeZone, char *pNewDaylightSaving);
int soapPlt_getInfo(int *iBlankState, char *timeZone, int timeZoneLen, int *iDST);
int soapPlt_getConfigInfo(char *bodyResp, int bufferLen);
int soapPlt_checkNewFirmware(int checkDurationSecond, char *currentVersion, char *newVersion, char *releaseNote);
int soapPlt_UpdateNewFirmware(int upgEnable, int *rebootTime);
int soapPlt_createAdmin(char *pNewUsername);
int soapPlt_updateAdminPassword(char *pNewEnableRecovery, char *pNewPassword, char *pNewSecurityQuestion1, char *pNewAnswer1, char *pNewSecurityQuestion2, char *pNewAnswer2);
int soapPlt_getRebootTime();
int soapPlt_reboot();
int soapPlt_setLightingLEDSettings(char *pMacAddr, int iLEDBright, int iLEDEnable);
int soapPlt_setLanSettings(int addressMode, char *staticAddr, char *subnet, char *gateway, char *dns);
int soapPlt_getLanSettings(char *lanIpAddr, int lanIpAddrLen, char *lanSubnet, int lanSubnetLen, char *lanGateway,
                           int lanGatewayLen, char *lanDns1, int lanDns1Len, char *addressMode, int addressModeLen);

#endif
