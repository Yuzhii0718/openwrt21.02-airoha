#ifndef __FOTA_CURL_H__
#define __FOTA_CURL_H__

enum fota_progress_type
{
    /* Update the progress by percentage */
    PROGRESS_TYPE_PERCENT = 1,
    /* Update the progress by time */
    PROGRESS_TYPE_TIME
};

struct fota_progress_measure
{
    enum fota_progress_type iType;
    /* Update the progress by per 'iPercent' percentage, used by 'percentage' method */
    int iPercent;
    /* Update the progress by per 'iInterval' seconds, used by 'time' method */
    int iInterval;
    /* Real-time progress output */
    FILE * pProgress;
};


struct fota_progress_config
{
    char * pProgress_file;
    int iProgress_type;
    int iProgress_percent;
    int iProgress_time;
};

enum http_chunk_type
{
    /* save http data to dynamic memory */
    CHUNK_TYPE_MEM = 1,
    /* save http data to file */
    CHUNK_TYPE_FILE
};

struct http_chunk
{
    enum http_chunk_type iType;
    int iStatus_code;
    size_t iSize;
    void * pData;
};


#define FREE_POINTER(p) \
    if (p) \
    { \
        free(p); \
        p = NULL; \
    }

#define RETURN_IF_NULL(p, ret) \
    if (!(p)) \
    { \
        vosLog_debug("'%s' is null pointer.\n", #p); \
        return ret; \
    } \
    else

#define RETURN_IF_EQUAL(result, value, ret) \
    if ((result) == (value)) \
    { \
        vosLog_debug("The value of '%s' is not OK.\n", #result); \
        return ret; \
    } \
    else

#define GOTO_IF_NULL(p, label) \
    if (!(p)) \
    { \
        vosLog_debug("'%s' is null pointer.\n", #p); \
        goto label; \
    } \
    else

#define GOTO_IF_NOT_EQUAL(result, value, label) \
    if ((result) != (value)) \
    { \
        vosLog_debug("The value of '%s' is not OK.\n", #result); \
        goto label; \
    } \
    else

#define GOTO_IF_EQUAL(result, value, label) \
    if ((result) == (value)) \
    { \
        vosLog_debug("The value of '%s' is not OK.\n", #result); \
        goto label; \
    } \
    else

int curl_download_file_ftp(char * pUrl, char *pFw_local_path);
int curl_download_file_https(char * pUrl, char *pFw_local_path);
int curl_download_file_http(char * pUrl, char *pFw_local_path);

#endif

