#ifndef __CGI_SOAP_H__
#define __CGI_SOAP_H__

#include <stdio.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <net/if.h>
#include "webs.h"
#include "uhttpd.h"
#include "wsIntrn.h"
#include "soaputils.h"

typedef struct {
   int berState;
   unsigned long berTime;
} WEB_TEST_VAR, *PWEB_TEST_VAR;

typedef void (*CGI_GET_HDLR) (int argc, char **argv, char *varValue);

typedef struct {
   char *cgiGetName;
   CGI_GET_HDLR cgiGetHdlr;
} CGI_GET_VAR, *PCGI_GET_VAR;

typedef void (*CGI_FNC_HDLR) (void);

typedef struct {
   char *cgiFncName;
   CGI_FNC_HDLR cgiFncHdlr;
} CGI_FNC_CMD, *PCGI_FNC_CMD;

typedef void (*CGI_GET_OBJ_HDLR) (int argc, char **argv, char *varValue);

typedef struct {
   char *cgiGetObjectName;
   CGI_GET_OBJ_HDLR cgiGetObjectHdlr;
} CGI_GET_OBJ_VAR, *PCGI_GET_OBJ_VAR;

typedef enum {
   CGI_STS_OK = 0,
   CGI_STS_ERR_GENERAL,
   CGI_STS_ERR_MEMORY,
   CGI_STS_ERR_FIND
} CGI_STATUS;

typedef enum {
   CGI_TYPE_NONE = 0,
   CGI_TYPE_STR,
   CGI_TYPE_MARK_STR,
   CGI_TYPE_NUM,
   CGI_TYPE_SYS_VERSION,
   CGI_TYPE_CFE_VERSION,
   CGI_TYPE_BOARD_ID,
   CGI_TYPE_ADSL_FLAG,
   CGI_TYPE_EXIST_PROTOCOL,
   CGI_TYPE_NUM_ENET,
   CGI_TYPE_NUM_PVC,
   CGI_TYPE_DHCP_LEASES,
   CGI_TYPE_PORT,
   CGI_TYPE_VPI,
   CGI_TYPE_VCI,
   CGI_TYPE_IPSEC_SETTINGS,
   CGI_TYPE_IPSEC_LIST,
   CGI_TYPE_CERT_LIST,
   CGI_TYPE_SPI_LIST,
   CGI_TYPE_ENET_DIAG,
   CGI_TYPE_VDSL_VERSION,
   CGI_TYPE_BOOL,
   CGI_TYPE_SHORT,
   CGI_TYPE_GROUP_LIST,
   CGI_TYPE_MOCA_LIST,
} CGI_TYPE;

typedef struct {
   char *variable;
   void *value;
   CGI_TYPE type;
} CGI_ITEM;

void do_soap(struct client *cl);
int cgibin_parse_request(webs_t wp);
const char* webGetVarString(webs_t wp, char_t *name);
void SoapApi_GetSrvMet(webs_t wp, char *szSrv, unsigned int lenSrv, char *szMet, unsigned int lenMet);
void get_soap_upath(webs_t wp, char *szPath, unsigned int len);
void send_soap_headers(webs_t wp, int status, char *title, char *extra_header, char *mime_type, char *header_cookie);
int webGetVarNodeExt(webs_t wp, char_t *name);
void uh_soap_request_done(struct client *cl);
void uh_soap_chunk_write(struct client *cl, const void *data, int len);
void Category_SoapBuff_Init(void);
void initWebsWp(void);
#endif
