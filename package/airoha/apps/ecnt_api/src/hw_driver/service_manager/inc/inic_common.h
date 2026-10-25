


#ifndef INIC_COMMON_H
#define INIC_COMMON_H

#if 0
#ifndef _BLAPI_XDSL_H_
#include "blapi_xdsl.h"
#endif

#ifndef _BLAPI_TRAFFIC_H
#include "blapi_traffic.h"
#endif
#endif



	
/*******************************************************************
 * D E F I N E   M A R C O
 *******************************************************************/

#define OP_BLAPI	(0x1)
#define OP_BLAPI_REPLY	(0x2)
#define OP_CMD		(0x3)
#define OP_CMD_REPLY	(0x4)
#define OP_PROC		(0x5)

#define ETH_TYPE_SRC	0xbeea
#define ETH_TYPE_WAN	0xbeeb
#define ETH_TYPE_EVENT	0xbeec
#define ETH_TYPE_PROC	0xbeed
#define ETH_TYPE_INIC	0xbeef

#define BUF_SIZ 1400
#define HEARTBEAT_SIZE	20

/* === event report MACRO ==== */
#define EVENT_MAGIC			(0x7517beec)
#define EVENT_HEARTBEAT		(0x01)
#define EVENT_WAN_CHANGE	(0x02)
#define EVENT_OAM_PING		(0x03)

#define SRC_TIMEOUT		3
#define INIC_TIMEOUT	2

#define READ_PROC_OP 	1
#define WRITE_PROC_OP	2

#define SRC_EN			(0x1)<<0
#define INIC_EN			(0x1)<<1
#define HEARTBEAT_EN	(0x1)<<2
#define WAN_CHG			(0x1)<<3

	enum pt_err_code
	{
		PT_SUCCESS=0,
		SOCKET_FAIL,
		CLIENT_NOT_READY,
		RECEIVE_TIMEOUT,
		CONFIG_TIMEOUT,
		NO_PROC,
		CRC_FAIL,
		UNHANDLE_FAIL
	};

enum src_blapi_id
{
	BLAPI_XDSL_RELOAD_KO = 0,
	BLAPI_XDSL_GET_FWVER,
	BLAPI_XDSL_GET_TSARM,
	BLAPI_XDSL_PTM_DO_RESET_SEQUENCE,
	BLAPI_XDSL_GET_INTERFACE_CONFIG_FILE,
	BLAPI_XDSL_ATM_CREATE_INTERFACE,
	BLAPI_XDSL_ATM_DELETE_INTERFACE,
	BLAPI_XDSL_CREATE_DEVICE,
	BLAPI_TRAFFIC_GET_ETHER_WAN_STATE,
	BLAPI_TRAFFIC_GET_WAN_TX_TRAFFIC,
	BLAPI_TRAFFIC_GET_WAN_RX_TRAFFIC,
	BLAPI_TRAFFIC_GET_WAN_TX_RATE,
	BLAPI_TRAFFIC_GET_WAN_RX_RATE,
	BLAPI_TRAFFIC_GET_WAN_TX_OCTETS,
	BLAPI_TRAFFIC_GET_WAN_RX_OCTETS,
	BLAPI_TRAFFIC_GET_WAN_TX_DISCARD_COUNTER,
	BLAPI_TRAFFIC_GET_WAN_RX_DISCARD_COUNTER,
	BLAPI_TRAFFIC_GET_WAN_TX_ERROR_COUNTER,
	BLAPI_TRAFFIC_GET_WAN_RX_ERROR_COUNTER,
	BLAPI_TRAFFIC_GET_WAN_TX_STATISTICS,
	BLAPI_TRAFFIC_GET_WAN_RX_STATISTICS,
	BLAPI_GET_WAN_LINK_MODE,
	BLAPI_SRC_CMD,
	BLAPI_TRAFFIC_GET_WAN_PORT_STATISTICS,
	BLAPI_TRAFFIC_GET_WAN_PORT_LINK_STATISTICS,
	BLAPI_TRAFFIC_SET_WAN_PORT_LINK_MODE,
	BLAPI_CONSOLE_AND_PRINT_SWITCH,
	BLAPI_SET_WAN_MODE,
	BLAPI_XDSL_GET_ADSL_MODE,
	BLAPI_XDSL_GET_ADSL_ANNEX,
	BLAPI_XDSL_GET_ADSL_RX_BEAR_TPSTC_TYPE,
	BLAPI_XDSL_GET_ADSL_ATTAIN_RATE,
	BLAPI_XDSL_GET_ADSL_NEAR_OP_DATA,
	BLAPI_XDSL_GET_ADSL_NEAR_ITUID,
	BLAPI_XDSL_GET_ADSL_STATISTIC_2,
	BLAPI_XDSL_GET_ADSL_LAST_DROP_REASON,
	BLAPI_XDSL_GET_ADSL_FAR_ITUID,
	BLAPI_XDSL_SET_POWER_UP_DOWN,
	BLAPI_SFP_SET_POWER_UP_DOWN,
	BLAPI_XDSL_SET_ADSL_ANNEX,
	BLAPI_XDSL_SET_ADSL_LOW_POWER,
	BLAPI_XDSL_SET_ADSL_AELEM,
	BLAPI_XDSL_SET_ADSL_TESTLAB,
	BLAPI_XDSL_SET_ADSL_SYSVID,
	BLAPI_XDSL_SET_ADSL_VERSION,
	BLAPI_XDSL_SET_ADSL_GINP,
	BLAPI_XDSL_SET_ADSL_SRA,
	BLAPI_XDSL_SET_ADSL_GVECTOR,
	BLAPI_XDSL_SET_ADSL_BITSWAP,
	BLAPI_XDSL_SET_ADSL_SOS_ROC,
	BLAPI_XDSL_SET_ADSL_US0,
	BLAPI_XDSL_SET_ADSL_DYING_GASP,
	BLAPI_XDSL_SET_ADSL_VIRTUAL_NOISE,
	BLAPI_XDSL_SET_ADSL_MODE,
	BLAPI_XDSL_SET_ADSL_PROFILE,
	BLAPI_XDSL_GET_OAM_PING_STATUS,
	SRC_BLAPI_ID_NUM,

};

/*******************************************************************
 * S T R U C T U R E S
 *******************************************************************/

#ifndef _BLAPI_XDSL_H_
typedef struct blapi_atm_interface_obj
{
	unsigned char idx;
	char transMode[16];
	char str_qos[16];
	char str_encap[32];
	int vpi;
	int vci;
	char str_pcr[16];
	char str_scr[16];
	char str_mbs[16];
	char isp[4];

}blapi_atm_interface_obj_t;

typedef struct blapi_atm_device_obj
{
	unsigned char idx;
	char transMode[16];
	char dev[8];
	char mac[32];
}blapi_atm_device_obj_t;
#endif

#ifndef _BLAPI_TRAFFIC_BSP_H_
typedef struct
{
    unsigned long long oversize;           /**< over sized cnt */
    unsigned long long  undersize;          /**< under sized cnt */
    unsigned long long  frame_cnt;          /**< packet cnt */
    unsigned long long  frame_len;          /**< byte cnt */
    unsigned long long  drop_cnt;           /**< drop cnt */
    unsigned long long  broadcast;          /**< broadcast cnt */
    unsigned long long  multicast;          /**< multicast cnt */
    unsigned long long  crc;                /**< crc cnt */
    unsigned long long  fragment;           /**< fragment cnt */
    unsigned long long  jabber;             /**< jabber cnt */
    unsigned long long  less_64;            /**< packet length < 64*/
    unsigned long long  more_1518;          /**< packet length > 1518 */
    unsigned long long  eq_64;              /**< packet length = 64*/
    unsigned long long  from_65_to_127;     /**< packet length between 65-127 */
    unsigned long long  from_128_to_255;    /**< packet length between 128-255 */
    unsigned long long  from_256_to_511;    /**< packet length between 256-511 */
    unsigned long long  from_512_to_1023;   /**< packet length between 512-1023 */
    unsigned long long  from_1024_to_1518;  /**< packet length between 1024-1518*/
}ECNT_TRAFFIC_WAN_RX_STATISTICS;

typedef struct
{
    unsigned long long frame_cnt;          /**< packet cnt */
    unsigned long long frame_len;          /**< byte cnt */
    unsigned long long drop_cnt;           /**< drop cnt */
    unsigned long long broadcast;          /**< broadcast cnt */
    unsigned long long multicast;          /**< multicast cnt */
    unsigned long long less_64;            /**< packet length < 64*/
    unsigned long long more_1518;          /**< packet length > 1518 */
    unsigned long long eq_64;              /**< packet length = 64*/
    unsigned long long from_65_to_127;     /**< packet length between 65-127 */
    unsigned long long from_128_to_255;    /**< packet length between 128-255 */
    unsigned long long from_256_to_511;    /**< packet length between 256-511 */
    unsigned long long from_512_to_1023;   /**< packet length between 512-1023 */
    unsigned long long from_1024_to_1518;  /**< packet length between 1024-1518*/
}ECNT_TRAFFIC_WAN_TX_STATISTICS;

typedef struct
{
	unsigned long long inBytes;
	unsigned long long inPkts;
	unsigned long long inUnicast;
	unsigned long long inMulticast;
	unsigned long long inBroadcast;
	unsigned long long inError;
	unsigned long long inDiscard;
	unsigned long long outBytes;
	unsigned long long outPkts;
	unsigned long long outUnicast;
	unsigned long long outMulticast;
	unsigned long long outBroadcast;
	unsigned long long outError;
	unsigned long long outDiscard;
}ECNT_TRAFFIC_GMAC_ETH_STA;

typedef struct
{
	unsigned char link_status;//0:down 1:up
	unsigned char auto_ng_status; //0:auto 1:force
	unsigned char duplex_status; //0:half-duplex 1:full-duplex
	unsigned char speed_status; //0:10Mbps 1:100Mbps 2:1000Mbps 3:2.5G bps
}ECNT_TRAFFIC_GMAC_ETH_LINKST;

typedef enum
{
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_AUTONEG    = 1, /**< auto negotiation */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_10M_HALF   = 2, /**< 10M Half Duplex  */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_10M_FULL   = 3, /**< 10M Full Duplex  */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_100M_HALF  = 4, /**< 100M Half Duplex */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_100M_FULL  = 5, /**< 100M Full Duplex */
    ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE_1000M_FULL = 6, /**< 1000M Full Duplex>*/
}ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE;
#endif

typedef union 
{
	int i;
	unsigned int iu;
	blapi_atm_interface_obj_t atm_i;
	blapi_atm_device_obj_t atm_d;
	ECNT_TRAFFIC_WAN_RX_STATISTICS rx_s;
	ECNT_TRAFFIC_WAN_TX_STATISTICS tx_s;
	ECNT_TRAFFIC_GMAC_ETH_STA port_s;
	ECNT_TRAFFIC_GMAC_ETH_LINKST link_s;
	ECNT_TRAFFIC_GMAC_ETH_SPEED_MODE mode_s;
	char ch[160];
}blapi_var_t;

typedef union 
{
	int i;
	unsigned int iu;
}blapi_var_int_t;


typedef struct {
	int	ret;
	unsigned int	blapi_id;
	unsigned int	argc;
	blapi_var_t  argv1;
	blapi_var_int_t  argv2;
	blapi_var_int_t  argv3;
	blapi_var_int_t  argv4;
	blapi_var_int_t  argv5;
} blapi_hdr_t;

struct romhdr {
	uint32_t	magic_no;		
	uint16_t  cmd_opcode;
	uint16_t  cmd_id;
	uint16_t  length;
	uint16_t  checksum;
	uint16_t  block_no;
	uint16_t  file_checksum;
	uint16_t  restart_code;
	uint16_t  rom_ver;
	uint32_t	mem_addr;		
	uint32_t	mem_value;		
	uint8_t   reserved[4];
	uint8_t	data[0];			
} __attribute__((packed)); 

typedef struct {
	uint32_t	magic_no;		
	uint16_t  cmd_opcode;
	uint16_t  length;
	uint16_t  checksum;
	uint8_t   reserved[2];
		
} src_hdr_t;

typedef struct {
	uint32_t ret;
	uint16_t rw_op;
	uint8_t   reserved[2];
} src_proc_hdr_t;

typedef struct {
	uint32_t pt_enable;
	uint32_t len;
	uint16_t inter_magic;
	uint32_t interval;
	char itf_name[10];
}share_data_t;

typedef struct {
	uint32_t	magic_no;		
	uint8_t		event_type;	
	uint8_t		event_content[59];
} event_hdr_t;

#endif


