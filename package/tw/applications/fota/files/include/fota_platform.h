#ifndef __FOTA_PLATFORM_H__
#define __FOTA_PLATFORM_H__

#include "fwk/fwk.h"
#include "dai/dai.h"

#define IMAGE_LOCALPATH                 "/tmp/image.bin"
#define SOAP_CHECK_RESULT_SAVE_DIR      "/etc"
#define LAST_AUTO_UPGRADE_TIMESTAMP     "/etc/last_auto_upgrade_timestamp" 
#define STRINGTABLE_SOAP_HTM_LOCALPATH  "/etc/release_notes.htm"


typedef void (*fota_msg_handle)(int, void *);
typedef void (*fota_periodical_task)(void);
typedef void (*timeout_handle)(void *);


typedef enum _FOTA_MSG_TYPE{
    FOTA_MSG_UPG_AUTO,                  /* auto upgrade fw online */
    FOTA_MSG_UPG_MANUAL_QUERY,          /* manual upgrade, query fw info */
    FOTA_MSG_UPG_MANUAL_DOWNLOAD,       /* manual upgrade, download fw */
    FOTA_MSG_UPG_MANUAL_UPGRADE,        /* manual upgrade, upgrade fw */
    FOTA_MSG_UPG_MANUAL_CANCLE,         /* manual upgrade, cancle upgrade fw */
    FOTA_MSG_SOAP_CHECK_NEW_FW,         /* soap check new firmware and download */
    FOTA_MSG_SOAP_UPGRADE_NEW_FW_YES,   /* soap update new firmware enable*/
    FOTA_MSG_SOAP_UPGRADE_NEW_FW_NO,    /* soap update new firmware disable*/
    FOTA_MSG_UPG_INFO_NOTICE,           /* tell other module that  upgrade info changes */
    FOTA_MSG_UPG_REBOOT_NOTICE,         /* tell other module that  upgrade reboot occurs */
    FOTA_MSG_MAX
}FOTA_MSG_TYPE;


void platform_send_fw_manual_query_response(int state);
void platform_send_fw_manual_download_response(int state);
void platform_send_fw_upgrade_status_to_soap(int state);
void platform_send_fw_upgrade_info_notice(void);
void platform_send_fw_upgrade_Reboot_notice(void);
int platform_save_firmware_to_flash(void);
void platform_save_config_to_flash(void);
int platform_check_local_upgrade_processing_flag(void);
int platform_getAutoUpgrade(char *enable, int len);
int platform_check_secure_boot(IMAGE_HEAD imageHead);
int platform_check_model(IMAGE_HEAD imageHead);
int platform_check_hardwareID(IMAGE_HEAD imageHead);
int platform_create_upgRebootInfoNotice_timer(int timeoutMs, timeout_handle handle, char *timerName);
int platform_check_update_new_version_boot(void);
int platform_check_is_blank_state(void);
void platform_save_cached_cfu_auto_check_url(char *url);
int platform_get_cached_cfu_auto_check_url(char *url, int len);
void platform_loop(fota_msg_handle fotaMsgHandle, fota_periodical_task fotaPeriodicalTask);
void platform_init(void);
void platform_exit(void);


#endif
