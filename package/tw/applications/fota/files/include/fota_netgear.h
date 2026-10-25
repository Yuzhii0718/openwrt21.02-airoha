#ifndef __FOTA_NETGEAR_H__
#define __FOTA_NETGEAR_H__


typedef enum _FOTA_ACTION_TYPE{
    FOTA_AUTO_CHECK_DOWNLOAD_UPGRADE,
    FOTA_SOAP_CHECK_NEW_VERSION,
    FOTA_SOAP_DOWNLOAD_UPGRADE_IMAGE,
    FOTA_MANUAL_CHECK_NEW_VERSION,
    FOTA_MANUAL_DOWNLOAD_IMAGE,
    FOTA_MANUAL_UPGRADE_IMAGE,
    FOTA_CFU_AUTO_CHECK_NEW_VERSION,
    FOTA_CFU_AUTO_DOWNLOAD_UPGRADE_IMAGE,
    FOTA_MAX
}FOTA_ACTION_TYPE;

typedef struct
{
    int second;           /* second: 0 - 59 */
    int minute;           /* minute: 0 - 59 */
    int hour;             /* hours:   0 - 23 */
    int weekday;          /* day of the month: 0 - 6 */
} FOTA_UPG_TIMESTAMP;

void removeAllLocalFile(void);
void NTGR_FwUpgradeMachine_HTTPS(FOTA_ACTION_TYPE action);
void NTGR_AutoUpgDeal(void);
void NTGR_FotaInit(void);


#endif

