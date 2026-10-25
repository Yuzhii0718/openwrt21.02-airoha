#ifndef __DEVICEINFO_H_
#define __DEVICEINFO_H_

#define M_DEVINFO_GETINFO "GetInfo"
#define M_DEVINFO_GETSYSUPTIME "GetSysUpTime"
#define M_DEVINFO_GETSUPPORTFEATURELISTXML "GetSupportFeatureListXML"
#define M_DEVINFO_GETATTACHDEVICE2 "GetAttachDevice2"
#define M_DEVINFO_GETATTACHDEVICEALL "GetAttachDeviceAll"
#define M_DEVINFO_SETNETGEARDEVICENAME "SetNetgearDeviceName"

#define DEVINFO_ATTACHDEVICE2 "<Device>\r\n<IP>%s</IP>\r\n\
<Name>%s</Name>\r\n<MAC>%s</MAC>\r\n<ConnectionType>%s</ConnectionType >\r\n\
<SSID>%s</SSID>\r\n<Linkspeed>%s</Linkspeed>\r\n<SignalStrength>%s</SignalStrength>\r\n\
</Device>\r\n"

typedef enum
{
    DEV_TYPE_ROUTER = 0,   /* Router */
    DEV_TYPE_AP = 1,       /* AP */
    DEV_TYPE_BRIDGE = 2,   /* Bridge */
    DEV_TYPE_EXTENDER = 3, /* Extender */
    DEV_TYPE_OTHERS = 9,   /* Others */
} NTGR_DEVICE_TYPE;

typedef struct
{
    char modelName[128];
    char desc[128];
    char sn[128];
    char fwVer[64];
    char hwVer[64];
    char useDate[128];
    char devName[128];
    int iDevMode;
} SOAPPLT_DEVICE_INFO;

int soapPlt_getDeviceInfo(SOAPPLT_DEVICE_INFO *pDeviceInfo);
int soapPlt_getSysUpTime(char *stmp, int stmpLen);
int soapPlt_getSupportFeatureListXML(char *bodyResp);
int soapPlt_getAttachDevice(char *bodyRespTemp, int bufferLen);
int soapPlt_setNetgearDeviceName(char *pMacAddr, char *pName);

#endif
