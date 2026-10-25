

#ifndef _LIBAPI_LIB_XDSLMGR_H_
#define _LIBAPI_LIB_XDSLMGR_H_

#define XDSL_END_NEAR		0
#define XDSL_END_FAR		1

#define XDSL_STREAM_DOWN	0
#define XDSL_STREAM_UP		1

#define XDSL_DIR_RX			0
#define XDSL_DIR_TX			1

#define XDSL_MAX_FWVER_LEN	64

typedef enum
{
	XDSL_EVEN_NONE = 0,
	XDSL_EVEN_A2P,
	XDSL_EVEN_P2A,

	XDSL_EVENT_DEF_NO
} XDSL_EVENT;

typedef struct
{
	unsigned char	TCM;
	unsigned char	K[2];
	unsigned char	R[2];
	unsigned int	S[2];
	unsigned int	D[2];
	unsigned char	M[2];
	unsigned char	T[2];
	unsigned char	msgc[2];
	unsigned int	L[2];
	unsigned int	delay[2];
	unsigned int	RSWords[2];
	unsigned int	SuperFE[2];
} XDSL_PMS;

typedef struct
{
	unsigned int Block[2];
	unsigned int CellDelin;
	unsigned int LinkRetrain;
	unsigned int InitErrors;
	unsigned int InitTimeouts;
	unsigned int LossOfFraming;
	unsigned int ErroredSecs;
	unsigned int SeverelyErroredSecs;
	unsigned int ATURChanRxBlks;
	unsigned int ATURChanTxBlks;
	unsigned int ATURChanCorrectBlks;
	unsigned int ATURChanUnCorrectBlks;
	unsigned int TotalCells[2];
	unsigned int DropCells[2];
	unsigned int Ocd[2];
	unsigned int Lcd[2];
	unsigned int Es[2];
	unsigned int Ses[2];
 	unsigned int SuperFrameCnt[2];
 	unsigned int UAS[2];
 	unsigned int BitErrorCnt[2];
} XDSL_SHOWTIME;

typedef enum
{
	ECNT_XDSLMGR_SUCCESS = 0,
	ECNT_XDSLMGR_FAIL,
	ECNT_XDSLMGR_NOT_LINKUP,

	ECNT_XDSLMGR_DEF_NO
} ECNT_XDSLMGR_RET;

typedef enum
{
	XDSL_LINE_NO_DEFECT = 0,
	XDSL_LINE_LOF,
	XDSL_LINE_LOSQ,
	XDSL_LINE_LOS,
	XDSL_LINE_LOP,
	XDSL_LINE_UNKNOW,
	XDSL_LINE_NOT_LINKUP,

	XDSL_LINE_DEF_NO
} XDSL_LINE_STATE;

typedef struct
{
	unsigned int rx_rate;
	unsigned int tx_rate;
} XDSL_ATTAIN_RATE;

typedef struct
{
	unsigned int rel_cap_occup[2];
	unsigned int snr[2];
	unsigned int pwr[2];
	unsigned int attn[2];
	unsigned char carrierLoad[256]; 
} XDSL_LINEDATA;

typedef struct
{
	unsigned int interleaved_fec[2];
	unsigned int interleaved_crc[2];
	unsigned int interleaved_hec[2];
	unsigned int fast_fec[2];
	unsigned int fast_crc[2];
	unsigned int fast_hec[2];
} XDSL_PERFDATA;

typedef struct
{
	unsigned int interleaved_rate[2];
	unsigned int fast_rate[2];
} XDSL_CHANDATA;

typedef enum
{
	XDSL_TPSTC_DISABLE = 0,
	XDSL_TPSTC_ATM,
	XDSL_TPSTC_PTM,

	XDSL_TPSTC_DEF_NO
} XDSL_TPSTC;

typedef enum
{
	XDSL_LINK_DOWN = 0,
	XDSL_LINK_WAIT_INIT,
	XDSL_LINK_INIT,
	XDSL_LINK_UP,

	XDSL_LINK_DEF_NO
} XDSL_LINK_STATUS;

typedef enum
{
	XDSL_ANNEX_A = 0,
	XDSL_ANNEX_B,
	XDSL_ANNEX_I,
	XDSL_ANNEX_J,
	XDSL_ANNEX_AL,
	XDSL_ANNEX_M,
	XDSL_ANNEX_BJ,
	XDSL_ANNEX_AIJLM,

	XDSL_ANNEX_DEF_NO
} XDSL_ANNEX;

typedef enum
{
	XDSL_MODE_GLITE = 0,
	XDSL_MODE_T1_413,
	XDSL_MODE_GDMT,
	XDSL_MODE_MULTIMODE,
	XDSL_MODE_ADSL2,
	XDSL_MODE_ADSL2PLUS,
	XDSL_MODE_VDSL2,
	XDSL_MODE_ADSL2PLUS_MULTI,

	XDSL_MODE_DEF_NO
} XDSL_MODE;


ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_event(XDSL_EVENT *p_event);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_pms(XDSL_PMS *p_pms);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_showtime(XDSL_SHOWTIME *p_showtime);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_linestate(XDSL_LINE_STATE *p_state);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_uptime(unsigned int *p_ms);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_attain_rate(XDSL_ATTAIN_RATE *p_rate);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_linedata(XDSL_LINEDATA *p_linedata);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_perfdata(XDSL_PERFDATA *p_perfdata);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_chandata(XDSL_CHANDATA *p_chandata);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_tpstc(XDSL_TPSTC *p_tpstc);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_annex(XDSL_ANNEX *p_annex);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_mode(XDSL_MODE *p_mode);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_linkstatus(XDSL_LINK_STATUS *p_status);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_get_fwver(char *p_fwver);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_set_annex(char *annex);
ECNT_XDSLMGR_RET ecnt_xdslmgr_lib_set_mode(char *mode);


#define ecnt_xdslmgr_lib_get_event xdslmgr_lib_get_event
#define ecnt_xdslmgr_lib_get_pms xdslmgr_lib_get_pms
#define ecnt_xdslmgr_lib_get_showtime xdslmgr_lib_get_showtime
#define ecnt_xdslmgr_lib_get_linestate xdslmgr_lib_get_linestate
#define ecnt_xdslmgr_lib_get_uptime xdslmgr_lib_get_uptime
#define ecnt_xdslmgr_lib_get_attain_rate xdslmgr_lib_get_attain_rate
#define ecnt_xdslmgr_lib_get_linedata xdslmgr_lib_get_linedata
#define ecnt_xdslmgr_lib_get_perfdata xdslmgr_lib_get_perfdata
#define ecnt_xdslmgr_lib_get_chandata xdslmgr_lib_get_chandata
#define ecnt_xdslmgr_lib_get_tpstc xdslmgr_lib_get_tpstc
#define ecnt_xdslmgr_lib_get_annex xdslmgr_lib_get_annex
#define ecnt_xdslmgr_lib_get_mode xdslmgr_lib_get_mode
#define ecnt_xdslmgr_lib_get_linkstatus xdslmgr_lib_get_linkstatus
#define ecnt_xdslmgr_lib_get_fwver xdslmgr_lib_get_fwver
#define ecnt_xdslmgr_lib_set_annex xdslmgr_lib_set_annex
#define ecnt_xdslmgr_lib_set_mode xdslmgr_lib_set_mode
#endif
