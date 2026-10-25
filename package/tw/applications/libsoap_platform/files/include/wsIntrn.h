/*
 *	wsIntrn.h -- Internal GoAhead Web server header
 *
 * Copyright (c) GoAhead Software Inc., 1992-2000. All Rights Reserved.
 *
 *	See the file "license.txt" for information on usage and redistribution
 *
 * $Id: //WIFI_SOC/release/SDK_4_1_0_0/source/user/goahead/src/wsIntrn.h#1 $
 */

#ifndef _h_WEBS_INTERNAL
#define _h_WEBS_INTERNAL 1
#define WEBS_IF_MODIFIED_SUPPORT 0
//#define WEBS_LOG_SUPPORT 0
//#define WEBS_PROXY_SUPPORT 0
//#define WEBS_PAGE_ROM 0
//#define B_STATS 0
/******************************** Description *********************************/

/*
 *	Internal GoAhead Web Server header. This defines the Web private APIs
 *	Include this header when you want to create URL handlers.
 */

/*********************************** Defines **********************************/

/*
 *	Define this to enable logging of web accesses to a file
 *		#define WEBS_LOG_SUPPORT 1
 *
 *	Define this to enable HTTP/1.1 keep alive support
 *		#define WEBS_KEEP_ALIVE_SUPPORT 1
 *
 *	Define this to enable if-modified-since support
 *		#define WEBS_IF_MODIFIED_SUPPORT 1
 *
 *	Define this to support proxy capability and track local vs remote request
 *		Note: this is not yet fully implemented.
 *		#define WEBS_PROXY_SUPPORT 1
 *
 *	Define this to support reading pages from ROM
 *		#define WEBS_PAGE_ROM 1
 *
 *	Define this to enable memory allocation and stack usage tracking
 *		#define B_STATS 1
 */

/********************************** Includes **********************************/

#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdarg.h>
#include    <regex.h>
#include    <syslog.h>

#ifdef NETWARE
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<io.h>
#endif

#ifdef WIN
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<io.h>
#endif
#include	<sys/stat.h>

#ifdef CE
#ifndef UEMF
	#include	<io.h>
#endif
#endif

#ifdef NW
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif

#ifdef SCOV5
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
#endif

#ifdef LYNX
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
#endif

#ifdef UNIX
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
#endif

#ifdef QNX4
	#include	<fcntl.h>
	#include	<sys/stat.h>
	#include	<signal.h>
	#include	<unistd.h>
	#include	<unix.h>
#endif

#ifdef UW
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif

#ifdef VXWORKS
	#include	<vxWorks.h>
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif

#ifdef SOLARIS
	#include	<macros.h>
	#include	<fcntl.h>
	#include	<sys/stat.h>
#endif
#if 1
#ifdef UEMF
	#include	"uemf.h"
	#include	"ejIntrn.h"
#else
	#include	"emf/emfInternal.h"
	#include	"ej/ejIntrn.h"
#endif
#endif
//#include "../../../../config/autoconf.h"

#ifdef CONFIG_USER_SUPERMESH
#include "nbapi.h"
#endif
#ifdef CONFIG_AP_REPEATER_SUPPORT
#define WIRELESS_ROUTER 	"WirelessRouter"
#define WIRELESS_AP 		"WirelessAp"
#define WIRELESS_REPEATER 	"WirelessRepeaterExtender"
#define WIRELESS_EXTENDER 	"WirelessExtender"
#endif

#ifdef CONFIG_USE_NAND_FLASH
#define SIZE_MAX_FILE (60*1024*1024)
#define OFFSET_OF_CHECKSUM	0x180000
#else
#define SIZE_MAX_FILE (60*1024*1024)
#define OFFSET_OF_CHECKSUM	0x60000
#endif

#define FIRMWARE_UPLOAD "FirmwareUpload"
#define CONFIG_FILE_UPLOAD "ConfigFileUpload"
#define CONFIG_DOWNLOAD "dlcfg.cgi"
#define SYSTEM_LOG_DOWNLOAD "dllog.cgi"
#define QUICK_VPN_DOWNLOAD "dlquickvpnsettings.cgi"

//extern char downloadurls[3][64];

//#define SIZE_DOWNLOADURL  (sizeof(downloadurls)/sizeof(downloadurls[0]))


//#include 	<shutils.h>
//#include 	<signal.h>

#include "webs.h"
//#define FCGI_HAVE_CONFIG_H
#if 0
#ifdef FCGI_HAVE_CONFIG_H
#include "fcgi_config.h"
#endif
#ifdef HAVE_FASTCGI_FASTCGI_H
#include <fastcgi/fcgi_stdio.h>
#else
#include <fcgi_stdio.h>
#endif

#include <json.h>
#include <json_object_private.h>
#endif
#include <cJSON.h>
//#include "uci.h"
#ifdef MEMWATCH
#include "memwatch.h"
#endif
/********************************** Defines ***********************************/
/*
 *	Read handler flags and state
 */
#define WEBS_BEGIN			0x1			/* Beginning state */
#define WEBS_HEADER			0x2			/* Ready to read first line */
#define WEBS_POST			0x4			/* POST without content */
#define WEBS_POST_CLEN		0x8			/* Ready to read content for POST */
#define WEBS_PROCESSING		0x10		/* Processing request */
#define WEBS_KEEP_TIMEOUT	15000		/* Keep-alive timeout (15 secs) */
#define WEBS_TIMEOUT		60000		/* General request timeout (60) */

#define PAGE_READ_BUFSIZE	10240		/* bytes read from page files */
#define MAX_PORT_LEN		10			/* max digits in port number */
#define WEBS_SYM_INIT		64			/* initial # of sym table entries */


// #if defined(WIFI_DBDC_SUPPORT)
// #define INTF_WLAN_24 "rax0"
// #define INTF_WLAN_58 "ra0"
// #define INTERFACE_2G			"rax0"
// #define INTERFACE_5G			"ra0"
// #define APCLI_IF_2G 			"apclix0"
// #define APCLI_IF_5G 			"apcli0"
// #define INTERFACE_2G_GUEST1		"rax1"
// #define	INTERFACE_5G_GUEST1		"ra1"
// #define	INTERFACE_5G_GUEST2		"ra2"
// #define	INTERFACE_5G_GUEST3		"ra3"
// #else
// #define INTF_WLAN_24 "ra0"
// #define INTF_WLAN_58 "rai0"
// #define INTERFACE_2G			"ra0"
// #define INTERFACE_5G			"rai0"
// #define APCLI_IF_2G 			"apcli0"
// #define APCLI_IF_5G 			"apclii0"
// #define INTERFACE_2G_GUEST1		"ra1"
// #define	INTERFACE_5G_GUEST1		"rai1"
// #define	INTERFACE_5G_GUEST2		"rai2"
// #define	INTERFACE_5G_GUEST3		"rai3"
// #endif

/*
 *	URL handler structure. Stores the leading URL path and the handler
 *	function to call when the URL path is seen.
 */
typedef struct {
	int		(*handler)(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
			char_t *url, char_t *path,
			char_t *query);					/* Callback URL handler function */
	char_t	*webDir;						/* Web directory if required */
	char_t	*urlPrefix;						/* URL leading prefix */
	int		len;							/* Length of urlPrefix for speed */
	int		arg;							/* Argument to provide to handler */
	int		flags;							/* Flags */
} websUrlHandlerType;

/*
 *	Webs statistics
 */
typedef struct {
	long			errors;					/* General errors */
	long			redirects;
	long			net_requests;
	long			activeNetRequests;
	long			activeBrowserRequests;
	long 			timeouts;
	long			access;					/* Access violations */
	long 			localHits;
	long 			remoteHits;
	long 			formHits;
	long 			cgiHits;
	long 			handlerHits;
} websStatsType;

extern websStatsType websStats;				/* Web access stats */

/*
 *	Error code list
 */
typedef struct {
	int		code;							/* HTTP error code */
	char_t	*msg;							/* HTTP error message */
} websErrorType;

/*
 *	Mime type list
 */
typedef struct {
	char_t	*type;							/* Mime type */
	char_t	*ext;							/* File extension */
} websMimeType;

/*
 *	File information structure.
 */
typedef struct {
	unsigned long	size;					/* File length */
	int				isDir;					/* Set if directory */
	time_t			mtime;					/* Modified time */
} websStatType;

/*
 *	Compiled Rom Page Index
 */
typedef struct {
	char_t			*path;					/* Web page URL path */
	unsigned char	*page;					/* Web page data */
	int				size;					/* Size of web page in bytes */
	int				pos;					/* Current read position */
} websRomPageIndexType;

/*
 *	Defines for file open.
 */
#ifndef CE
#define	SOCKET_RDONLY	O_RDONLY
#define	SOCKET_BINARY	O_BINARY
#else /* CE */
#define	SOCKET_RDONLY	0x1
#define	SOCKET_BINARY	0x2
#endif /* CE */

extern websRomPageIndexType	websRomPageIndex[];
extern websMimeType		websMimeList[];		/* List of mime types */
//extern sym_fd_t			websMime;			/* Set of mime types */
extern webs_t*			webs;				/* Session list head */
extern int				websMax;			/* List size */
extern char_t			websHost[64];		/* Name of this host */
extern char_t			websIpaddr[64];		/* IP address of this host */
extern char_t			*websHostUrl;		/* URL for this host */
extern char_t			*websIpaddrUrl;		/* URL for this host */
extern int				websPort;			/* Port number */

/******************************** Prototypes **********************************/

extern int		 websAspOpen();
extern void		 websAspClose();
extern void		 websFormOpen();
extern void		 websFormClose();
extern int		 websAspWrite(int ejid, webs_t wp, int argc, char_t **argv);
extern void  	 websDefaultClose();

extern int 		 websShareFileHandler(webs_t wp, char_t *urlPrefix,
					char_t *webDir, int arg, char_t *url, char_t *path,
					char_t *query);

extern int 		 websDefaultHandler(webs_t wp, char_t *urlPrefix,
					char_t *webDir, int arg, char_t *url, char_t *path,
					char_t *query);
extern int 		 websFormHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
					int arg, char_t *url, char_t *path, char_t *query);
extern int 		 websCgiHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
					int arg, char_t *url, char_t *path, char_t *query);
extern void		 websCgiCleanup();
extern int		 websCheckCgiProc(int handle, int *status);
extern char_t	 *websGetCgiCommName();

extern int		 websLaunchCgiProc(char_t *cgiPath, char_t **argp,
					char_t **envp, char_t *stdIn, char_t *stdOut);
extern int 		 websOpen(int sid);
extern void 	 websResponse(webs_t wp, int code, char_t *msg,
					char_t *redirect);
extern void      websRspNotAuth(webs_t wp);
extern int 		 websJavaScriptEval(webs_t wp, char_t *script);
extern int 		 websPageReadData(webs_t wp, char *buf, int nBytes);
extern int		 websPageOpen(webs_t wp, char_t *lpath, char_t *path, int mode,
					int perm);
extern void		 websPageClose(webs_t wp);
extern void		 websPageSeek(webs_t wp, long offset);
extern int 	 	 websPageStat(webs_t wp, char_t *lpath, char_t *path,
					websStatType *sbuf);
extern int		 websPageIsDirectory(char_t *lpath);
extern int 		 websRomOpen();
extern void		 websRomClose();
extern int 		 websRomPageOpen(webs_t wp, char_t *path, int mode, int perm);
extern void 	 websRomPageClose(int fd);
extern int 		 websRomPageReadData(webs_t wp, char *buf, int len);
extern int 	 	 websRomPageStat(char_t *path, websStatType *sbuf);
extern long		 websRomPageSeek(webs_t wp, long offset, int origin);
extern void 	 websSetRequestSocketHandler(webs_t wp, int mask,
					void (*fn)(webs_t wp));
extern int 		 websSolutionHandler(webs_t wp, char_t *urlPrefix,
					char_t *webDir, int arg, char_t *url, char_t *path,
					char_t *query);
extern void 	 websUrlHandlerClose();
extern int 		 websUrlHandlerOpen();
extern int 		 websOpenServer(int port, int retries);
extern void 	 websCloseServer();
extern char_t*	 websGetDateString(websStatType* sbuf);
extern int 		 websGetCSRFToken(webs_t wp);
extern int		strcmpci(char_t* s1, char_t* s2);
#if 0
extern size_t FCGI_fwrite(void *ptr, size_t size, size_t nmemb, FILE *fp);
extern int FCGI_puts(const char *str);
extern int FCGI_fflush(FILE *fp);
extern FILE *fcgi_conn;


#define FCGI_stdin	(fcgi_conn)
#define FCGI_stdout	(fcgi_conn)
#define FCGI_stderr	(fcgi_conn)
#endif
/************************************************************/
#define LOGFILE	"/dev/console"

#define HOME_PAGE_HTML "Index.html"//"index.html"
#define LOGCGIFILE	"/var/log/fcgi.log"
//static FILE *logcgi_fp;
#define DEBUG_SYSLOG	0
#if 0
#if DEBUG_SYSLOG
static FILE *logcgi_fp;
#define DBG_MSG(fmt, arg...)	do { \
						syslog(LOG_LOCAL0 | LOG_DEBUG, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
					} while(0)
#else
#define DBG_MSG(fmt, arg...)	do {	FILE *log_fp = fopen(LOGFILE, "w"); \
						fprintf(log_fp, "[prog_%d]%s:%s:%d:" fmt "\n", getpid(), __FILE__, __func__, __LINE__, ##arg); \
						fclose(log_fp); \
					} while(0)
#endif

#define LOG_MSG(fmt, arg...)	do {	FILE *logcgi_fp = fopen(LOGCGIFILE, "a"); \
						fprintf(logcgi_fp, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
						fclose(logcgi_fp); \
					} while(0)
#define SESSION_MSG(fmt, arg...)	do {	FILE *log_fp = fopen(LOGFILE, "w"); \
						fprintf(log_fp, "[prog_%d]%s:%s:%d:" fmt "\n", getpid(), __FILE__, __func__, __LINE__, ##arg); \
						fclose(log_fp); \
					} while(0)

#define MESH_MSG(fmt, arg...)	do {	FILE *log_fp = fopen(LOGFILE, "w"); \
							fprintf(log_fp, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
							fclose(log_fp); \
						} while(0)
#else
#define DBG_MSG(fmt, arg...)	do {;\
					} while(0)

#define LOG_MSG(fmt, arg...)	do {;\
					} while(0)


#define SESSION_MSG(fmt, arg...)	do {;\
					} while(0)

#define MESH_MSG(fmt, arg...)	do {;\
					} while(0)
#endif


#define BOOL_CHECK_ERROR_OK(r,v,g) {if(r!=TRUE){r=v; soapLog_error("conflict check fail : "#v); goto g;}}
#define RETURN_VAR_ERROR(r,v,g) {r=v; if(r!=RESULT_OK){soapLog_error("return ret error : "#v); goto g;}}
#define RETURN_ERROR_OK(r,g) {if(r!=RESULT_OK){soapLog_error("return fail");goto g;}}
#define CHECK_ERROR_OK(r,g) {if(r!=TRUE){soapLog_error("check fail");goto g;}}

#define RETURN_SOAP_ERROR(r,v,g) {r=v; if(r!=SOAP_RESP_OK){soapLog_error("error : "#v); goto g;}}

/* 安全释放内存*/
#define safe_free(pointer) \
    { \
        if ( pointer ) \
        { \
            free(pointer); \
            pointer = NULL; \
        } \
    }
#define BUFF_MAX_LEN 	1024
typedef enum
{
	MODULE_LAN,
	MODULE_DNSMASQ,
	MODULE_WLAN,
	MODULE_WLAN_GUEST,
	MODULE_WAN,
	MODULE_WAN6,
	MODULE_DHCPS,
	MODULE_PLC,
	MODULE_MLD,
	MODULE_IGMP,
	MODULE_ALG,
	MODULE_UPNP,
	MODULE_DOS,
	MODULE_MAC,
	MODULE_AUTH,
	MODULE_DNS,
	MODULE_QOS,
	MODULE_IPT,
	MODULE_URLFILTER,
	MODULE_FIREWALL,
	MODULE_SNTP,
	MODULE_UPG,
	MODULE_AUTOUPG,
	MODULE_REBOOT,
	MODULE_ACL,
	MODULE_LOG,
	MODULE_DSL_ATM,
	MODULE_DSL_PTM,
	MODULE_DSL_MODULATION,
	MODULE_QUICKVPN,
	MODULE_STATICROUTE_IPV4,
	MODULE_STATICROUTE_IPV6,
	MODULE_SYSTEMLOG,
	MODULE_SCHEDULE,
	MODULE_FTP,
	MODULE_SAMBA,
	MODULE_DDNS_IPV4,
	MODULE_DDNS_IPV6,
	MODULE_ADMIN,
	MODULE_UPNP_AND_IGMP,
	MODULE_EMAIL,
	MODULE_MDNS,
	MODULE_DLNA,
	MODULE_USB_SERVICE,
#ifdef CONFIG_AP_REPEATER_SUPPORT
	MODULE_MODE,
#endif
	MODULE_MULTIPLE_ACTION,

	/*
	* ...
	*/
	MODULE_ALL
} Fastcgi_Module_t;

typedef enum
{
	SAVE_CONFIG,
	SERVICE_APPLY
} Fastcgi_Service_t;
typedef enum{
	RESULT_OK,
	RESULT_ERROR,
	RESULT_REBOOT,
	SET_JSON_FORMAT_ERROR,
	SET_JSON_TYPE_ERROR,
	SET_CONFIG_ERROR,
	SET_DATA_TYPE_ERROR,
	SET_DATA_VALUE_ERROR,
	SET_CHECK_CONFLICT_ERROR,
	GET_JSON_FORMAT_ERROR,
	GET_JSON_TYPE_ERROR,
	GET_JSON_NULL_ERROR,
	GET_JSON_VALUE_ERROR,
	GET_LIB_CONFIG_ERROR,
	GET_DATA_TYPE_ERROR,
	GET_DATA_VALUE_ERROR,
	GET_CONFIG_ERROR,
	GET_CONFIG_RECUR_DEEP_ERROR,
	SET_CONFIG_RECUR_DEEP_ERROR,
	SAVE_CONFIG_ERROR,
	GET_NEW_JSON_OBJECT_ERROR,
	RESULT_WIZARD_FINISH,
	XMLDB_FORMAT_ERROR,
	RESULT_HNAP_ERROR,
	MALLOC_FAIL_ERROR,
	SERVER_ERROR,
	SET_REMOTE_PLC_ERROR,
	RESULT_RESTART,
	WAIT_FOR_PROCESS
} Fastcgi_Result_t;


/*
 *	Prototypes for functions available when running as part of the
 *	GoAhead Embedded Management Framework (EMF)
 */
#ifdef EMF
extern int 		 websEmfOpen();
extern void 	 websEmfClose();
extern void 	 websSetEmfEnvironment(webs_t wp);
#endif

#ifdef CE
extern int writeUniToAsc(int fid, void *buf, unsigned int len);
extern int readAscToUni(int fid, void **buf, unsigned int len);
#endif
#define FCGI_HAVE_CONFIG_H

extern int downloadFile(char *cgiName);

extern int mac_convert(const char *macColon, unsigned char *mac);


#endif /* _h_WEBS_INTERNAL */

/******************************************************************************/

