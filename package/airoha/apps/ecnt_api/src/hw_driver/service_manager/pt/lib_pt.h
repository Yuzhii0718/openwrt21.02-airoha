#ifndef LIB_PT_H
#define LIB_PT_H
#include <stdint.h>
#ifdef TCSUPPORT_SISM_HOST
#include "../inc/inic_common.h"
#endif

#define BUF_SIZ 1400

#define LOG_DEBUG  0
#define LOG_INFO   1
#define LOG_ERR    2
#define LOG_PRINTF 3

#ifdef TCSUPPORT_SISM_HOST
#define SISM_DBG_MSG(level, ...) sismDbgMsg(level, __func__, __LINE__, __VA_ARGS__)
extern int sism_log_level;
extern void sismDbgMsg(int level, const char *func, int line, const char *format, ...);
#else
#define SISM_DBG_MSG(level, ...) {if(level>=LOG_INFO){printf("[%s:L%d] ", __func__, __LINE__);printf(__VA_ARGS__);printf("\n");}}
#endif

void htonll(uint64_t val, uint64_t * target);
void ntohll(uint64_t val, uint64_t * target);
int pt_fd_open(unsigned short ethertype, int timeout);
int pt_fd_close(unsigned short ethertype);
uint8_t * req_pt_pkt_buf(int size);
int pt_pkt_send(unsigned short ethertype, uint8_t * s_buf,int len);
int pt_pkt_recv(unsigned short ethertype, uint8_t* payload);
void free_pt_pkt_buf(uint8_t* buf);
#ifdef TCSUPPORT_SISM_HOST
int SRC_blapi(blapi_hdr_t* blapi_var);
#endif
#endif


