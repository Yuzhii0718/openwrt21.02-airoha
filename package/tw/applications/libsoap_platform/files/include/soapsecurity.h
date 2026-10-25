#ifndef __SOAPSECURITY_H__
#define __SOAPSECURITY_H__


#define SOAPAUTH_TABLE_SIZE 256

#define LOGIN_LOCK_TIME (30)
#define LOGIN_IDLE_TIME (300)
#define LOGIN_EXPIRE_TIME (3600)
#define LOGIN_TIMES (5)

#define LOGIN_DEFAULT_NAME "admin"
#define LOGIN_DEFAULT_PASSWD "password"

#define SOAP_JWT_LOCAL 		 "jwt_local"
#define SOAP_SPLIT_CHAR_SET  "&;"

#define HEADER_AUTH "{\"typ\":\"JWT\",\"alg\":\"HS256\"}"
#define PAYLOAD_AUTH "{\"exp\":\"%ld\",\"iss\":\"www.netgear.com\",\"sub\":\"%s\"}"
#define PAYLOAD_AUTH_SUB "Nighthawk/2.4.12.4 CFNetwork/808.2.16 Darwin/16.3.0"

typedef enum
{
	AUTH_LOGIN_SUCCESS				=0,
	AUTH_LOGOUT_SUCCESS				=1,
	AUTH_TOKEN_SUCCESS				=2,
	AUTH_LOGIN_ACCOUNT_BLOCKED		=3,
	AUTH_FAILED_ERROR_NAME_PWD		=4,
	AUTH_FAILED_ACCOUNT_BLOCKED		=5,
	AUTH_FAILED_TOKEN_ERROR			=6,
	AUTH_FAILED_IDLE_TIMEOUT		=7,
	AUTH_FAILED_EXPT_TIMEOUT		=8,
	AUTH_FAILED_SERVER_BUSY			=9,
	AUTH_OLDAPI_FAILED_ERROR_NAME_PWD	= 10,

} AuthFailedEnum;


#define NULL_PTR_FREE_BUF(m,p) \
   do { \
      if ((m) == NULL) {free((p)); (p) = NULL; return 1;}   \
   } while (0)

#define MEM_FREE_BUF_AND_NULL_PTR(p) \
   do { \
      if ((p) != NULL) {free((p)); (p) = NULL;}   \
   } while (0)


typedef struct tagST_SOAP_AUTH
{
	UBOOL8 	 auth;
	char 	*pmac;
	char 	*ptoken;
	time_t   tmLock;
	time_t   tmIdle;
	time_t   tmExpire;
    UINT32   uErrorCouter;
	
	char 	*pip;
	char 	*psoapurl;
}ST_SOAP_AUTH;

typedef struct soap_auth_entry {
    struct tagST_SOAP_AUTH *auth_mem;
} SOAP_AUTH_Entry;

int soapSec_Auth1Handler(webs_t wp);
void soapSec_OutputHandler(webs_t wp);
int soapSec_AuthHandler(webs_t wp);
void soapSec_ParseCookieJwt(webs_t wp);
int soapSec_AuthInfoHandler(webs_t wp, int iAuthRet);
int soapSec_Auth404Handler(webs_t wp, char *upath);
UBOOL8 soap_auth_table_init(void);
#endif // __SOAPSECURITY_H__

