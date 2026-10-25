#ifndef __RAE_CHECK_H_
#define __RAE_CHECK_H_

#include "fwk/fwk.h"
#include "dai/dai.h"

#define RAE_QUERY_LIMITS  3
#define MAX_ERROR_MSG_PRINTOUT 5

#define FILEINFO_LEAST_SIZE 80

#define RAE_CHECK_TIMER_DELAY_RUN      30
#define RAE_CHECK_TIMER_INTERVAL       20
#define RAE_CHECK_RAESTATUS_INTERVAL   60
#define RAE_CHECK_RAEUPDATE_DEALWITH   20
#define RAE_CHECK_RAECONF_CHANGE       20
#define RAE_CHECK_UPGRADE_RETRY_TIME   60

#define MIN_START_TIME                 60
#define DEFAULT_START_TIME             90
#define DEFAULT_INTERVAL_TIME          1440 * 60

#define RAE_POLICY_FILE             "/rae/config/RAE_Policy.json"
#define RAE_UPDATE_TIME             "/rae/config/rae_update_time"
#define RAE_UPDATE_CONF             "/rae/config/rae_update_conf.json"
#define RAE_FILEINFO                "/rae/config/fileinfo.txt"
#define RAE_EXE_FOLDER              "/rae/bin/"

// #define RAE_POLICY_FILE_BK          "/etc/RAE_Policy.json"
// #define RAE_UPDATE_TIME_BK          "/etc/rae_update_time"
// #define RAE_UPDATE_CONF_BK          "/etc/rae_update_conf.json"
#define RAE_EXE_BK                  "/usr/sbin/rae"

#define RAE_FOLDER_BK               "/etc/rae/"
#define RAE_UPDATE_TIME_NAME        "rae_update_time"
#define RAE_UPDATE_CONF_NAME        "rae_update_conf.json"
#define RAE_POLICY_NAME             "RAE_Policy.json"

#define UPDATE_INTERNAL_KEY         "updaterInternal"
#define UPDATE_HTTPS_URL_KEY        "httpsUrl"
#define UPDATE_FTP_URL_KEY          "ftpUrl"
#define UPDATE_RAE_VERSION_KEY      "raeVersion"

#define POLICY_VERSION_KEY          "raePolicyVersion"

// #define RAE_HTTPS_URL               "https://http.fw.updates1.netgear.com/sw-apps/router-analytics/"
// #define RAE_FTP_URL                 "ftp://updates1.netgear.com/sw-apps/router-analytics/"
// #define RAE_DEFAULT_VERSION         "V1.5.0.0"

#define REBOOTINFO_FILE             "/rae/config/rebootinfo"

#define FILE_INFO                   "fileinfo.txt"
#define RAE_POLICY                  "RAE_Policy.json"

#define DEST_RAE_PATH               "/var/rae/"
#define DEST_RAE_INFO               "/var/rae/fileinfo.txt"
#define DEST_POLICY_FILE            "/var/rae/RAE_Policy.json"
#define DEST_RAE_INFO_UTF8          "/var/rae/fileinfo_utf8.txt"
#define RAE_LINK_FILE               "/rae/bin/rae"

#define FILE_INFO_HEADKEY           "[Major1]"

#define RAESTAGE_DEFAULT_VAL        "prod"
#define RAESTAGE_QA                 "qa"
#define RAESTAGE_DEV                "dev"

#define POINTER_CHECK_RET(p, ret) \
    if (!(p)) \
    {\
        syslog(LOG_ERR, "'%s' is null pointer.\n", #p); \
        return (ret); \
    }

#define POINTER_CHECK(p) \
    if (!(p)) \
    {\
        syslog(LOG_ERR, "'%s' is null pointer.\n", #p); \
        return; \
    }

VOS_RET_E restore_rae_form_fwupdate();
VOS_RET_E set_rae_version(char * raeVersion);

void restore_rae_conf(const char * destConf, const char *srcConf);

typedef enum {
    CHECK_SUCCESS = 0,
    CHECK_SIZE_FAIL = 1,
    CHECK_MD5_FAIL = 2,
    CHECK_VERSION_FAIL = 3,
    CHECK_INVALID_PARAMS = 4,
 }RaeCheckRes;

#endif
