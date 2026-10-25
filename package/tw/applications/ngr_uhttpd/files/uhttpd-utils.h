/*
 * uhttpd - Tiny single-threaded httpd - Utility header
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef _UHTTPD_UTILS_h_
#define _UHTTPD_UTILS_h_

#include <stdarg.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>

#ifdef HAVE_SHADOW
#include <shadow.h>
#endif

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

#define LOGIN_SESSION "/tmp/AUTH_login_session"
#define TEMPORARY_SESSION "/tmp/temporary_session"
#define PASSWD_RESET_SESSION "/tmp/passwd_reset_session"
#define NO_NEEDAUTH	"/tmp/AUTH_no_need_auth"
#define LOGIN_TIME	"/tmp/AUTH_login_time"
#define LOGIN_IP	"/tmp/AUTH_login_ip"
#define LOGIN_DEV	"/tmp/AUTH_login_dev"
#define LOGIN_MAC	"/tmp/AUTH_login_mac"
#define LOGIN_TIMEOUT	300 /* 5 min */
#define DEFAULT_SESSION_ID "00000000000000000000000000000000"

#define array_size(x) \
	(sizeof(x) / sizeof(x[0]))

#define foreach_header(i, h) \
	for( i = 0; (i + 1) < (sizeof(h) / sizeof(h[0])) && h[i]; i += 2 )

#define fd_cloexec(fd) \
	fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC)

#define ensure_out(x) \
	do { if((x) < 0) goto out; } while(0)

#define ensure_ret(x) \
	do { if((x) < 0) return -1; } while(0)


struct path_info {
	char *root;
	char *phys;
	char *name;
	char *info;
	char *query;
	int redirected;
	struct stat stat;
};


const char * sa_straddr(void *sa);
void sa_pton(void *sa, const char *strsa);
const char * sa_strport(void *sa);
int sa_port(void *sa);
int sa_rfc1918(void *sa);

char *strfind(char *haystack, int hslen, const char *needle, int ndlen);

int select_intr(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *t);

int uh_tcp_send(struct client *cl, const char *buf, int len);
int uh_tcp_send_lowlevel(struct client *cl, const char *buf, int len);
int uh_tcp_peek(struct client *cl, char *buf, int len);
int uh_tcp_recv(struct client *cl, char *buf, int len);
int uh_tcp_recv_lowlevel(struct client *cl, char *buf, int len);

int uh_http_sendhf(
	struct client *cl, int code, const char *summary,
	const char *fmt, ...
);

#define uh_http_response(cl, code, message) \
	uh_http_sendhf(cl, code, message, message)

int uh_http_sendc(struct client *cl, const char *data, int len);

int uh_http_sendf(
	struct client *cl, struct http_request *req,
	const char *fmt, ...
);

int uh_http_send(
	struct client *cl, struct http_request *req,
	const char *buf, int len
);


int uh_urldecode(char *buf, int blen, const char *src, int slen);
int uh_urlencode(char *buf, int blen, const char *src, int slen);
int uh_b64decode(char *buf, int blen, const unsigned char *src, int slen);
int Base64_Encode(unsigned char* input, const int inputlen, unsigned char** output);

struct auth_realm * uh_auth_add(char *path, char *user, char *pass);

int uh_auth_check(
	struct client *cl, struct http_request *req, struct path_info *pi
	);

int uh_cgi_auth_check(
	struct client *cl, struct http_request *req, struct path_info *pi
);

struct path_info * uh_path_lookup(struct client *cl, const char *url);

struct listener * uh_listener_add(int sock, struct config *conf);
struct listener * uh_listener_lookup(int sock);

struct client * uh_client_add(int sock, struct listener *serv);
struct client * uh_client_lookup(int sock);
void uh_client_remove(int sock);

#ifdef HAVE_CGI
struct interpreter * uh_interpreter_add(const char *extn, const char *path);
struct interpreter * uh_interpreter_lookup(const char *path);
#endif

int getCookieByStr(const char* origin, const char* name, char* buf, uint16_t len);
int uh_getCookieByEnv(const char* name, char *buf, uint16_t len);
int uh_scanCookies(const char* origin, char *cookies[], int num);
char* uh_warpSign(const char* str, char* buf, UINT16 len);
char* uh_unWarpSign(const char* str, char* buf, UINT16 len);
void uh_getCookieSession(char *cookieSession, struct http_request *req);
int uh_destroy_login();
void srand_random(char *pBuff, int len);

void uh_initTokenKey();
int uh_generateToken(const char* session, char* outBuf, int bufLen);
UBOOL8 uh_verifyToken(const char* pToken, const char* pSession);
int uh_getSessionFromToken(const char* token, char *session);
int saveCookieSession(char *cookieSession, struct http_request *req);
#endif
