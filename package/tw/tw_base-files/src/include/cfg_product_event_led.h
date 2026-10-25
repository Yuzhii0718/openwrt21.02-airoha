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
 * @brief The general define for the led event
 */
#ifndef __CFG_PRODUCT_EVENT_LED_H__
#define __CFG_PRODUCT_EVENT_LED_H__
/**
 * LED Event Variables: not only for some packages
 * (Should be for some behaviors)
 */
#define LEDEVENT_CLI(x, arg...)
#define LEDEVENT_DAEMON(isdbg, event)
#define LEDEVENT_CLI_CMD                    CMD_ECHO

#if IS_ENABLED(CFG_IS_LEDEVENT)
    #if IS_ENABLED(CFG_IS_LEDEVENT_SYSLEDD)
    #undef  LEDEVENT_CLI_CMD
    #define LEDEVENT_CLI_CMD                "led_ctrl"
    #if IS_ENABLED(CFG_SUPPORT_DAEMONIZE_SYSLEDD)
    #undef  LEDEVENT_DAEMON
    #define LEDEVENT_DAEMON(isdbg, state)   ((state == 'F' || state == 'T') ? CMD_SYSLEDD " " STR(LED_GO_FACTORY) "&" \
                                                            : CMD_SYSLEDD " " STR(LED_GO_NORMAL)  "&")
    #else
    #undef  LEDEVENT_DAEMON
    #define LEDEVENT_DAEMON(isdbg, state)   ((state == 'F' || state == 'T') ? LEDEVENT_CLI_CMD " " STR(LED_GO_FACTORY) "&" \
                                                            : LEDEVENT_CLI_CMD " " STR(LED_GO_NORMAL)  "&")
    #endif
    #endif

    #if IS_ENABLED(CFG_IS_LEDEVENT_SYSCLID)
    #undef  LEDEVENT_DAEMON
    #define LEDEVENT_DAEMON(isdbg, state)   (isdbg ? ((state == 'F' || state == 'T') ? CMD_LEDEVENT " -d && " LEDEVENT_CLI_GO(LED_GO_FACTORY) \
                                                                   : CMD_LEDEVENT " -d && " LEDEVENT_CLI_GO(LED_GO_NORMAL) ) \
                                                   : ((state == 'F' || state == 'T') ? CMD_LEDEVENT    " && " LEDEVENT_CLI_GO(LED_GO_FACTORY) \
                                                                   : CMD_LEDEVENT    " && " LEDEVENT_CLI_GO(LED_GO_NORMAL) ) \
                                            )
    #undef  LEDEVENT_CLI_CMD
    #define LEDEVENT_CLI_CMD                CMD_LEDEVENT "_cli"
    #endif

    #undef  LEDEVENT_CLI
    // TODO(SOMEONE): should we use "&"?
    #define LEDEVENT_CLI_GO(x, arg...)      LEDEVENT_CLI_CMD " " STR(x) arg " &"
    #define LEDEVENT_CLI(x, arg...)         CMD(FD_NONE, LEDEVENT_CLI_GO(x, ##arg))
#endif

// TODO(SOMEONE): Why LED_ETH_PORT3_UP and LED_ETH_PORT?
#define LED_ETH_PORT "LED_ETH_PORT" // for LED_ETH_PORT%d_%s gerneric use

typedef enum {
    /* LED Events for boot-up */
    LED_GO_FACTORY      = EVENT_MAX,
    LED_GO_NORMAL,
    LED_STATE_NORMAL,
    /* LED Events for factory */
    LED_ALL_ON, // light on all leds
    LED_ALL_OFF, // light off all leds
    LED_COLOR_RED,
    LED_COLOR_GREEN,
    LED_COLOR_BLUE,
    LED_COLOR_ORANGE,
    LED_COLOR_WHITE,
    /* LED Events for normal mode */
    LED_UNKNOWN, // sysledd not use, TODO(SOMEONE): what's the LED_UNKNOWN?
#if IS_ENABLED(CFG_IS_LED_SUPPORT_DISABLE)
    LED_ENABLE, // temporaily enable led
    LED_DISABLE, // temporaily disable led
#endif
    LED_FW_GET,
    LED_FW_GET_FAIL,
    LED_FW_UPGRADE,
    LED_FW_UPGRADE_FAIL,
    LED_TOKEN_RESTORE,
    LED_TOKEN_RESTORE_FAIL,
    LED_READY_TO_RESET_DEFAULT,
    LED_RESET_DEFAULT,
    LED_RESET_DEFAULT_CANCEL,
    LED_RESET_DEFAULT_FAIL,
    LED_REBOOT,
#if IS_ENABLED(CFG_IS_ETHERNET)
    LED_ETH_PORT3_UP,
    LED_ETH_PORT3_DOWN,
    LED_ETH_PORT4_UP,
    LED_ETH_PORT4_DOWN,
#endif
#if IS_ENABLED(CFG_IS_SRV_WAN)
    LED_WAN_CONNECTED,
    LED_WAN_DISCONNECTED,
#endif
#if IS_ENABLED(CFG_IS_LED_LAN)
    LED_LAN_CONNECTED,
    LED_LAN_DISCONNECTED,
#endif
#if IS_ENABLED(CFG_IS_RADIO1)
    LED_WLAN_RADIO1_UP,
    LED_WLAN_RADIO1_DOWN,
    LED_WLAN_RADIO1_GUEST_UP,
#endif
#if IS_ENABLED(CFG_IS_RADIO2)
    LED_WLAN_RADIO2_UP,
    LED_WLAN_RADIO2_DOWN,
    LED_WLAN_RADIO2_GUEST_UP,
#endif
#if IS_ENABLED(CFG_IS_RADIO3)
    LED_WLAN_RADIO3_UP,
    LED_WLAN_RADIO3_DOWN,
    LED_WLAN_RADIO3_GUEST_UP,
#endif
#if IS_ENABLED(CFG_IS_SUPPORT_WPS)
    LED_WLAN_WPS_READY,
    LED_WLAN_WPS_TRIGGERRING,
    LED_WLAN_WPS_SUCCESS,
    LED_WLAN_WPS_TIMEOUT,
#endif
#if IS_ENABLED(CFG_IS_PKG_QCA_WIFI_SON)
    LED_WIFISON_RE_DISCONNECT,
    LED_WIFISON_RE_CONNECT,
    LED_WIFISON_CAP_NOT_READY,
    LED_WIFISON_CAP_READY,
    LED_WIFISON_INIT_MODE,
    LED_WIFISON_RE_SYNC_SUCCESS,
    LED_WIFISON_RE_CONNECT_WEAK,
#endif
#if IS_ENABLED(CFG_IS_BLUETOOTH)
	LED_BT_ON,
	LED_BT_OFF,
#endif
#if IS_ENABLED(CFG_IS_VOICE)
	LED_PHONE_ON,
	LED_PHONE_OFF,
	LED_PHONE_BLINK,	
#endif
    LEDEVENT_MAX
} TYPE_LEDEVENT;

#endif /* __CFG_PRODUCT_EVENT_LED_H__ */
