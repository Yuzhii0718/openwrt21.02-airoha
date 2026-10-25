/************
 *
 * Filename:    dalh_def.h
 * Purpose:     definition for dal structures
 * Copyright:   (c) Netgear Inc.
 * 				2019 All rights reserved
 * Author:      @ VVDN TECHNOLOGIES
 *
 ************/

/* Define to prevent recursive inclusion---------------------*/

#ifndef _DALH_DEF_H_
#define _DALH_DEF_H_

#include <d2lib/d2api.h>

#ifdef __cplusplus
extern "C" {
#endif

/*Define buffer size */

#define BUFF_LEN_8 8
#define BUFFER_SIZE_16 16
#define BUFFER_SIZE_18 18
#define BUFFER_SIZE_32 32
#define BUFFER_SIZE_64 64
#define BUFFER_SIZE_128 128
#define BUFFER_SIZE_256 256
#define MSG_BUFFER_SIZE 64
#define MAX_MAC_LEN 18
#define DEFAULT_MAC "00:00:00:00:00:00"
#define DEFAULT_INDEX_VALUE 256 //!< Stores the value for DAL table index initialization
#define MAX_INDEX_VALUE 255 //!< Stores the last index value for DAL table
#define CD_UPDATE_CLOUD_PASS_ID 10 //!< Stores the value of cloud pass id for CD update
#define STATE_LEN 16 //!< Buffer to store the event state i.e. complete/incomplete
#define MAX_STATUS_LEN 16 //!< Maximum length of status
#define ND_SUPPORT_SWITCH 0 //!< Switch to enable/disable the state of Netduma feature

/* Defines for the return values */
#define FAIL -1
#define ERROR_FWUPGRADE_TIMER -17
#define ERROR_REFRESH_TIMER -18
#define ERROR_RWUPGRADE_TIMER  -19
#define MODULENAME "Dalh"
#define UNUSED_PARAM(x) (void)(x)

/* Define for the Table Value */
#define RA_DEBUG_TABLE_NAME "RADebugLog"

/*
 * Supported Error Types
 */
enum check_d2_error {

    ERROR_OK_DALH,
    ERROR_TIMEOUT,
    ERROR_SHUTDOWN,
    ERROR_NOSUCH_ITEM,
    ERROR_TOO_LONG,
    ERROR_INVALID_INPUT_DIL,
    ERROR_WRONG_TYPE,
    ERROR_ENCODING,
    ERROR_D2,
    ERROR_UNKNOWN,
    ERROR_MAC_ADDRESS_MISSING,
    ERROR_NOTHING_TO_UPDATE,
};

enum processId {
    UPAGENT = 1,
    SOAP = 2,
    WEB_GUI = 3,
    DIL_POSTPROCESSOR = 4,
    DIL_PREPROCESSOR = 5,
};

enum featureId {
    NDN = 1,
    DTN_TAG = 2,
    DTN_UNTAG = 3,
    BST_ON_DEMAND = 4,
    BST_SCHEDULE = 5,
    ALLOW_MAC = 6,
    BLOCK_MAC = 7,
    CDIL = 8,
    DEV_CUSTOM = 9,
    REMOVE_MAC = 10,
    PAUSE_PROFILE = 11,
    UNPAUSE_PROFILE = 12,
    DEV_STATE_CHANGE = 13,
    DEV_UPDATE_CD_CUSTOM = 14,
};

//CDIL checkpoints index information to debug CDIL failure
enum cdilCheckpointIndex {
    CDIL_PROCESS = 0,
    CDIL_INITIATE = 1,
    CDIL_ENQUEUE = 2,
    CDIL_DALHSUCESS = 3,
    CDIL_DALHFAIL = 4,
};

// return codes for dalh module
typedef enum dalh_ret_codes {
    DALH_RET_SUCCESS = 0,
    DALH_RET_FAILURE = 1,
    DALH_RET_D2_ERROR = 2,
    DALH_RET_SET_GET_ERROR = 3,
    DALH_RET_INVALID_INPUT = 4
} dalh_ret_t;

//enum values for satellite setup feature cases
typedef enum sat_setup {
    AT_BOOTUP,
    AT_CLAIM_STATE,
    AT_EVENT_NTF
} sat_setup_t;

struct Queue {
    char TagMAC[BUFFER_SIZE_32];
    char TagStatus[BUFFER_SIZE_16];
    char cdil_command[BUFFER_SIZE_16];
    uint32_t fId; //stores the feature Id
    uint32_t ndnIdx; //stores the NDN index
    uint32_t devStateChangeIdx;/*store the device state (online/offline) index*/
    uint32_t cdIndexChanged; //!< stores the customised index
    char bstStatus[10];
    char AllowMAC[BUFFER_SIZE_32];
    char BlockMAC[BUFFER_SIZE_32];
    char GenericMAC[BUFFER_SIZE_32];
    char customname[BUFFER_SIZE_64 + 1];
    char customtype[BUFFER_SIZE_32 + 1];
    char custombrand[BUFFER_SIZE_32 + 1];
    char custommodel[BUFFER_SIZE_32 + 1];
}__attribute__((packed));

/* DALHConfig data attributes */
struct getDalhCfgInfo {
    char TaggedMAC[BUFFER_SIZE_32];
    char TaggedStatus[BUFFER_SIZE_16];
} __attribute__((packed));

/* DALHConfig data attributes */
struct getAccessControlInfo {
    char AllowedMAC[BUFFER_SIZE_32];
    char BlockedMAC[BUFFER_SIZE_32];
} __attribute__((packed));

/* DALHConfig custom device data information */
struct getcustumdeviceinfo {
    char custom_mac[BUFFER_SIZE_32];
    char custom_name[BUFFER_SIZE_64 + 1];
    char custom_type[BUFFER_SIZE_32 + 1];
    char custom_brand[BUFFER_SIZE_32 + 1];
    char custom_model[BUFFER_SIZE_32 + 1];
} __attribute__((packed));

/* DAL */
bool aclEnableFlag;
uint32_t deviceCount;
char** profileMAC;
char remove_mac[BUFFER_SIZE_32];
char DtnTagMac[BUFFER_SIZE_32];
char DtnTagStatus[BUFFER_SIZE_16];
unsigned int TagStatusSize;
uint32_t ndnIndex;
uint32_t cdUpdateIndex;
uint32_t featureId;
uint32_t processId;
char bstStatusdata[10];
char cdil_command_info[BUFFER_SIZE_16];
char AllowCdMACdata[BUFFER_SIZE_32];
char BlockCdMACdata[BUFFER_SIZE_32];
char AllowBlockStatus[BUFFER_SIZE_16];
int get_cdil_command();
int signal_user2_handler();
void get_ndn_attributes(uint32_t* indx);
void notify_upa_for_ndn(uint32_t indx);
void notify_dil_for_dtn(void);
void get_dtn_attributes(void);
void notify_daemon_for_bst(void);
void get_bst_attributes(void);
void get_cdil_attributes(void);
void execute_cdil_request(void);
void get_allow_attributes(void);
void get_block_attributes(void);
void notify_dil_for_allow(void);
void notify_dil_for_block(void);
void get_profile_attributes(void);
void notify_dil_for_profile_operation(uint32_t profileState);
/*Func defs to get CDIL debug checkpoints message*/
const char* getCdilCheckpointMessage(const enum cdilCheckpointIndex cdilMsg);
/* //will be used further
enum check_d2_error get_DilDeviceMAC(char *MacAddress, unsigned int idx);
*/
// Passing the valid index in function means the change request is for only particular index, Index range is 0-255
dalh_ret_t notify_nd_for_evt(uint32_t indx, const char* event);
void get_state_change_attributes(uint32_t* index);
int check_index(uint32_t ix);

#ifdef __cplusplus
}
#endif
#endif //_DALH_DEF_H_
