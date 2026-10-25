#ifndef __FOTA_H__
#define __FOTA_H__

#include "fota_platform.h"
#include "fw_check.h"

#define FOTA_QUERY_LIMITS  3

#define ONLINE_CHECK_FINISH_FLAG        "/var/online_check_finish_flag"
#define STRINGTABLE_FLAG_LOCALPATH      "/var/release_note_flag_file"
#define STRINGTABLE_HTM_LOCALPATH       "/var/release_notes.htm"
#define TMP_STRINGTABLE_LOCALPATH       "/var/tmp_stringtable.dat"
#define NEW_SW_VERSION_LOCALPATH        "/tmp/upg_new_version"
#define QUERY_NO_NEW_VERSION_FLAG_PATH  "/tmp/upg_no_new_version_cause"  /* 10000: No internet. 10001: No new version. 10002: Download file fail. 10003: download file incorrect */
#define TMP_PERCENT_LOCALPATH           "/tmp/percent"
#define FILEINFO_LOCALPATH              "/var/fileinfo"
#define STRINGTABLE_LOCALPATH           "/var/stringtable.dat"
#define IMAGE_DOWNLOAD_STATUS           "/var/image_download_status"  /* 0: success  1: download fw fail  2: download fw incorrect */

#define CFU_CACHED_INFO_DIR                        "/var/cfu_cache_info"
#define CFU_CACHED_FILEINFO                        "/var/cfu_cache_info/fileinfo"
#define CFU_CACHED_STRINGTABLE                     "/var/cfu_cache_info/stringtable.dat"
#define CFU_CACHED_USE_DIR_WW_FLAG                 "/var/cfu_cache_info/use_dir_ww_flag"
#define CFU_CACHED_AUTO_CHECK_FILEINFO             "/var/cfu_cache_info/auto_check_fileinfo"
#define CFU_CACHED_AUTO_CHECK_STRINGTABLE          "/var/cfu_cache_info/auto_check_stringtable.dat"
#define CFU_CACHED_AUTO_CHECK_USE_DIR_WW_FLAG      "/var/cfu_cache_info/auto_check_use_dir_ww_flag"

#define CFU_SYSTEM_REBOOT_FLAG_FILE                "/var/cfu_reboot_flag"
#define CFU_LOG_FILE                               "/tmp/cloud_backend_log"


#define CURLINFO_RESPONSE_CODE_200 200   /* HTTPS return success code */
#define CURLINFO_RESPONSE_CODE_226 226   /* FTP return success code */

#endif

