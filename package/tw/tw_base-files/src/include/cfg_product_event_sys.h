/*******************************************************************************
 *        Copyright (c) 2017 TWin Advanced Technology Corp., Ltd.
 *        All rights reserved.
 *
 *       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
 *       PROPRIETARY AND TRADESECRET INFORMATION OF T&W INCORPORATED.
 *       ACCESS TO THIS WORK IS RESTRICTED TO (I) T&W EMPLOYEES WHO HAVE A
 *       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
 *       AND (II) ENTITIES OTHER THAN T&W WHO HAVE ENTERED INTO APPROPRIATE
 *       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
 *       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
 *       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 *       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF T&W.
 *       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
 *       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
 ******************************************************************************/
 /**
 * @brief The general define for the system interface daemon event
 */
#ifndef __CFG_PRODUCT_EVENT_SYS_H__
#define __CFG_PRODUCT_EVENT_SYS_H__

#ifndef IS_ENABLED
    #define __ARG_PLACEHOLDER_1 0,
    #define config_enabled(cfg) _config_enabled(cfg)
    #define _config_enabled(value) __config_enabled(__ARG_PLACEHOLDER_##value)
    #define __config_enabled(arg1_or_junk) ___config_enabled(arg1_or_junk 1, 0)
    #define ___config_enabled(__ignored, val, ...) val
    #define IS_ENABLED(option) config_enabled(option)
#endif // #ifndef IS_ENABLED




#include "cfg_product.h"
#include "cfg_product_variables.h"


/**
 * Service Event Variables: not only for some packages
 * (could be for some behaviors)
 */
#define SYSIFD_CLI(x)
#define SYSIFD_DAEMON
#define SYSIFD_CLI_CMD                    CMD_ECHO
#define SYSIFD_DAEMON_CONF_PATH           "/var/run/" SYSIFD_DAEMON
#define SYSIFD_CLI_IF                     SYSIFD_DAEMON_CONF_PATH "/" SYSIFD_DAEMON "if0"

#if IS_ENABLED(CFG_IS_SYSIFD)
    #if IS_ENABLED(CFG_IS_SYSIFD_STATIC)
    #undef  SYSIFD_DAEMON
    #define SYSIFD_DAEMON                 CMD_SYSIFD
    #undef  SYSIFD_CLI_CMD
    #define SYSIFD_CLI_CMD                CMD_SYSIFD "_cli"
    #endif

    #if IS_ENABLED(CFG_IS_SYSIFD_DYNAMIC)
    #undef  SYSIFD_DAEMON
    #define SYSIFD_DAEMON                 CMD_SYSIFDL
    #undef  SYSIFD_CLI_CMD
    #define SYSIFD_CLI_CMD                SYSIFD_DAEMON "_cli"
    #endif

    #undef  SYSIFD_CLI
    #define SYSIFD_CLI(x)                 system(SYSIFD_CLI_CMD " " STR(x))
#endif

#define TYPE_SYSEVENT_EXTRA \
    S(SYS_GO_FACTORY) \
    S(SYS_GO_TEST) \
    S(SYS_GO_NORMAL) \
    S(SYS_GO_RESUME) \
    S(SYS_TOK_CHANGE) \
    S(SYS_CC_RELOAD) \
    S(SYS_FW_QUERY) \
    S(SYS_FW_UPG_BY_URL) \
    S(SYS_FW_UPG_BY_URL_DL) \
    S(SYS_FW_UPG_BY_URL_VALID) \
    S(SYS_TIME_OBTAIN) \
    S(SYS_LOG2EMAIL) \
    S(SYS_LOG2EMAIL_LOG_READY) \
    S(SYS_MYDLINK_RELOAD) \
    S(SYS_MYDLINK_REGISTER) \
    S(SYS_MYDLINK_RESET) \
    S(SYS_MCAFEE_RELOAD) \
    S(SYS_MCAFEE_SHGW_START) \
    S(SYS_MCAFEE_SHGW_STOP) \
    S(SYS_MCAFEE_SHGW_RESET) \
    S(SYS_OOKLA_SPEEDTEST) \
    S(SYS_LANIP_OBTAIN) \
    S(SYS_WANIP_OBTAIN) \
    S(SYS_ETH_LOOP) \
    S(SYS_PINGPONG_ALIVE) \
    S(SYS_MODULES) \
    S(SYS_TELNETD) \

typedef enum {
    SYS_DUMMY          = EVENT_MAX - 1,
    #define S(event)  event,
    TYPE_SYSEVENT_EXTRA
    #undef  S
    SYSEVENT_MAX
} TYPE_SYSEVENT;

#if IS_ENABLED(CFG_IS_SRV_LOG2EMAIL)
    #define CMD_SRV_LOG_SIGNAL_CLI(x)       system(CMD_KILL " -" x " `" CMD_CAT " " SYSLOGD_PID " 2> /dev/null` 2>/dev/null")
    #define SIGNAL_LOG2EMAIL_BY_THR_ENABLE  "30"
    #define SIGNAL_LOG2EMAIL_PREPARE        "31"
#endif // #if IS_ENABLED(CFG_IS_SRV_LOG2EMAIL)

#if IS_ENABLED(CFG_IS_PKG_SYSHOTPLUGD)
    #define SIGANL_STOP_PORT_LINK_UP            30
    #define SIGANL_START_PORT_LINK_UP           31
#endif // #if IS_ENABLED(CFG_IS_PKG_SYSHOTPLUGD)

#endif /* __CFG_PRODUCT_EVENT_SYS_H__ */
