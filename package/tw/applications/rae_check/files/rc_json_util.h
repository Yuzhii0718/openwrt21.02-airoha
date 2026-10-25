#ifndef __JSON_UTIL_H_
#define __JSON_UTIL_H_

#include "json-c/json.h"
#include "fwk/fwk.h"

typedef struct rae_config
{
    char httpsUrl[BUFLEN_512];
#ifdef ENFTP
    char ftpUrl[BUFLEN_512];
#endif
    char bkupRaeVersion[BUFLEN_32];
} RaeConfig;

VOS_RET_E get_update_internal(unsigned long *intervalTime, unsigned long *startTime);

VOS_RET_E get_rae_conf(RaeConfig * raeConfig);

VOS_RET_E get_rae_policy_version(const char * fileName, char * raePolicyVersion, unsigned int  strLen);

#endif
