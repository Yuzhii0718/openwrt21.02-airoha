#ifndef __UTIL_PLUGIN_H__
#define __UTIL_PLUGIN_H__

#define UTIL_PLUGIN_JSON_KEY_CLIENT_MAC     "CLIENT_MAC"

#define UTIL_PLUGIN_INFO_RELA_PATH "../Info.plugin"
#define UTIL_PLUGIN_UPGRADE_FLAG_PATH "/tmp/upgradeFlag"
#define UTIL_PLUGIN_JSON_KEY_RET_PARAM "return_Parameter"
#define UTIL_PLUGIN_CMD_PLUGIN_NAME "Plugin_Name"
#define UTIL_PLUGIN_CMD_INSTALL_VERSION "Version"
#define UTIL_PLUGIN_CMD_INSTALL_DOWNLOAD_URL "Download_url"
#define UTIL_PLUGIN_CMD_INSTALL_SIZE "Plugin_size"
#define UTIL_PLUGIN_CMD_INSTALL_OS "OS"
#define UTIL_PLUGIN_CMD_INSTALL_UPGRADEID "upgrade_ID"
#define UTIL_PLUGIN_CMD_PLUGIN_STATE "Run"

#define UTIL_PLUGIN_PARTION_PATH "/usr/plugin"


#define UTIL_PLUGIN_NAME_LENGTH 128


/* define the c plugin command */
typedef enum {
    UTIL_PLUGIN_CMD_NONE = 0,
    UTIL_PLUGIN_CMD_INSTALL,
    UTIL_PLUGIN_CMD_PREINSTALL,
    UTIL_PLUIGN_CMD_CLEAR_INSTALLING_INFO,
    UITL_PLUGIN_CMD_INSTALL_QUERY,
    UITL_PLUGIN_CMD_INSTALL_CANCEL,
    UTIL_PLUGIN_CMD_UNINSTALL,
    UTIL_PLUGIN_CMD_RUN,
    UTIL_PLUGIN_CMD_STOP,
    UTIL_PLUGIN_CMD_STOPALL, /* Stop all thirty part apps */
    UTIL_PLUGIN_CMD_SHOW,
    UTIL_PLUGIN_CMD_LIST,
    UTIL_PLUGIN_CMD_FACKTORY,
    UTIL_PLUGIN_CMD_RECOVER,
    UTIL_PLUGIN_CMD_LOAD_JSONCONF,
    UTIL_PLUGIN_CMD_IS_UNINSTALL,
    UTIL_PLUGIN_CMD_UPGRADE_IMAGE
} UTIL_PLUGIN_CMD_E;


typedef enum{
    UTIL_PLUGIN_RESET_MODE_DEFAULT = 0,
    UTIL_PLUGIN_RESET_MODE_LONG_PRESS,
    UTIL_PLUGIN_RESET_MODE_SHORT_PRESS,
    UTIL_PLUGIN_RESET_MODE_FROM_HTTPD,
    UTIL_PLUGIN_RESET_MODE_FROM_ITMS,
    UTIL_PLUGIN_RESET_MODE_FROM_PLATFORM
}UTIL_PLUGIN_RESET_MODE_E;


typedef struct {
    UTIL_PLUGIN_CMD_E cmdCode;
    UINT32 ID;
    UINT32 source;
    UINT32 sourceEid;
    char *pluginName;
    char *ctx;
} UTIL_PLUGIN_REQ_MSG_T;


typedef struct {
    UTIL_PLUGIN_CMD_E cmdCode;
    UINT32 ID;
    UINT32 source;
    UINT32 sourceEid;
    SINT32 retCode;
    char *pluginName;
    char *retCtx;
} UTIL_PLUGIN_RESP_MSG_T;


typedef struct
{
    char pluginName[UTIL_PLUGIN_NAME_LENGTH];
    char message[BUFLEN_512];
} UTIL_PLUGIN_INFORM_INFO_T;


typedef struct
{
    char macAddr[32];
} UTIL_LAN_DEV_UP_OR_DOWN_EVENT_T;


/** Event handler type definition
 */
typedef char *(*UtilPluginSetParamHandler)(const char *param);



/** Get pluginName from plugin's Info.plugin
 * @Param(OUTPUT) : pluginName -
 * 
 * Return VOS_RET_E;
 */
VOS_RET_E UTIL_replacePluginGetNamebase(char **pluginName);

VOS_RET_E UTIL_getCustomerName(char *customerCN, UINT32 len);

VOS_RET_E UTIL_pluginGetInfoName(char *infoPath,char *pluginName, UINT32 size);

void UTIL_pluginCustormerNameToBase(char **customerPluginName);

void UTIL_pluginSetPluginNameByCustomer(char **pluginName, const char *customer);

void UTIL_pluginGetCustomer(const char *pluginName, char **customer);

VOS_RET_E UTIL_pluginGetName(char *pluginName, UINT32 size);

VOS_RET_E UTIL_pluginGetImportInterface(char *importInterface, UINT32 size);


VOS_RET_E UTIL_pluginSendReqMsg(void *msgHandle, VosMsgHeader *msg, UTIL_PLUGIN_REQ_MSG_T *req, UTIL_PLUGIN_RESP_MSG_T *resp);
VOS_RET_E UTIL_pluginRecvReqMsg(VosMsgHeader *msg, UTIL_PLUGIN_REQ_MSG_T *req);
VOS_RET_E UTIL_pluginSendRespMsg(void *msgHandle, VosMsgHeader *msg, UTIL_PLUGIN_RESP_MSG_T *resp);
VOS_RET_E UTIL_pluginRecvRespMsg(VosMsgHeader *msg, UTIL_PLUGIN_RESP_MSG_T *resp);


VOS_RET_E UTIL_pluginRegisterSetParamEvent(void *msgHandle, UtilPluginSetParamHandler func);
VOS_RET_E UTIL_pluginExecuteSetParamEvent(VosMsgHeader *msg);


VOS_RET_E UTIL_pluginGetReqMsg(VosMsgHeader *msg, char **param);
VOS_RET_E UTIL_pluginSetRespMsg(VosMsgHeader *msg, char **return_params);


#endif /* __UTIL_PLUGIN_H__ */
