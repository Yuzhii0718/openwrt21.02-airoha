
#ifndef _LIBAPI_LIB_PTMMGR_H_
#define _LIBAPI_LIB_PTMMGR_H_

#define PTM_MAX_BEARER	2

typedef enum
{
	ECNT_PTMMGR_SUCCESS = 0,
	ECNT_PTMMGR_FAIL,

	ECNT_PTMMGR_DEF_NO
} ECNT_PTMMGR_RET;


typedef struct ptm_wan_cnt
{
	unsigned int rxPkts;
	unsigned int txPkts;
	unsigned int rxDropPkts;
	unsigned int txDropPkts;
	unsigned int rxBytes;
	unsigned int txBytes;
} PTM_WAN_CNT;

typedef struct ptm_mib_cnt
{
	unsigned char bearer_max;
	unsigned int rxBytes[PTM_MAX_BEARER];
	unsigned int rxPkts[PTM_MAX_BEARER];
	unsigned int rxDiscards[PTM_MAX_BEARER];
	unsigned int rxErrors[PTM_MAX_BEARER];
	unsigned int txBytes[PTM_MAX_BEARER];
	unsigned int txPkts[PTM_MAX_BEARER];
	unsigned int txDiscards[PTM_MAX_BEARER];
	unsigned int rxErrCrc[PTM_MAX_BEARER];
	unsigned int rxErrLong[PTM_MAX_BEARER];
	unsigned int rxErrRunt[PTM_MAX_BEARER];
	unsigned int rxErrIp4Cs[PTM_MAX_BEARER];
	unsigned int rxErrL4Cs[PTM_MAX_BEARER];
} PTM_MIB_CNT;

ECNT_PTMMGR_RET ecnt_ptmmgr_lib_start(char *name);
ECNT_PTMMGR_RET ecnt_ptmmgr_lib_stop(void);
ECNT_PTMMGR_RET ecnt_ptmmgr_lib_show_wan_counter(PTM_WAN_CNT *p_wan_cnt);
ECNT_PTMMGR_RET ecnt_ptmmgr_lib_show_mib_counter(PTM_MIB_CNT *p_mib_cnt);

#define ecnt_ptmmgr_lib_start ptmmgr_lib_start
#define ecnt_ptmmgr_lib_stop ptmmgr_lib_stop
#define ecnt_ptmmgr_lib_show_wan_counter ptmmgr_lib_show_wan_counter
#define ecnt_ptmmgr_lib_show_mib_counter ptmmgr_lib_show_mib_counter
#endif