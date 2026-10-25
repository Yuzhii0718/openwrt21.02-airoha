#ifndef RSU_APP_CHECK_H_
#define RSU_APP_CHECK_H_
/************
 *
 * Filename: rsu_app_check.h
 *
 * Purpose: Public API calls of RSU (Router Software Update) Application Check library.
 *
 * Copyright:	(c) Netgear Inc.
 *		2017, 2023 All rights reserved
 *
 ************/

#include <cfu_log.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Supported Error Types
  */
enum rsu_app_error {
    ERROR_OK,
    ERROR_DAL,
    ERROR_INIT,
    ERROR_TIMEOUT,
    ERROR_CONNECT,
    ERROR_INVALID_INPUT,
    ERROR_INTERNAL,
    ERROR_NO_URL,
    ERROR_NO_APPS,
    ERROR_UNKNOWN,
};

extern void (* fw_debug)(enum logLev ll, const char* fmt, ...);

/**
  * Supported reasons for library call
  */
typedef enum call_reason {

    CHECK_UNSPECIFIED,
    CHECK_AUTO,
    CHECK_ON_DEMAND_UPAPP,
    CHECK_ON_DEMAND_GUI,
    CHECK_ON_DEMAND_INSTALLATION,
} reason_type;

/**
 * Map enum rsu_app_error to an error message
 * @param err
 * @return Printable error message
 */
const char* error_message(enum rsu_app_error);

/**
 * Initialize librsuappcheck library
 * @return ERROR_OK on success
 */
enum rsu_app_error rsu_app_check_init(void);

/**
 * Uninitialize librsuappcheck library
 * @return ERROR_OK on success
 */
enum rsu_app_error rsu_app_check_uninit(void);

/**
 * Performs checkApplication RSU cloud API call with info from DAL
 * @param mode reason to call enum
 * @param beta_acceptance
 * @param applications_json input "applications" json string
 * @param applications_json_resp output "applications" json string from RSU
 * @return enum rsu_app_error type
 */
enum rsu_app_error rsu_check_app(const enum call_reason mode,
                                 bool beta_acceptance, const char *applications_json,
                                 char **applications_json_resp);

/**
 * Performs notifyApplication API of RSU cloud with info from DAL
 * @param applications_json input "applications" json string
 * @return enum rsu_app_error type
 */
enum rsu_app_error rsu_notify_app(const char *applications_json);

#ifdef __cplusplus
};
#endif
#endif
