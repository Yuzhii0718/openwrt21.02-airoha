#ifndef _RA_NAT_GEN_H
#define _RA_NAT_GEN_H

#include "foe_fdb.h"
#include <linux/soft_dma.h>

#define ACNT_GRP_SMUX_CNT_START		3
#define TRAFFIC_FLOW_MAX_NUM    (8)

#define ACNT_GRP_SMUX_CNT(index, dir, is_multi)	(ACNT_GRP_SMUX_CNT_START+(index<<2)+(dir<<1)+is_multi)
#define ACNT_GRP_SMUX_Frame_CNT(index, dir, is_multi) (AC_BASE+8+((ACNT_GRP_SMUX_CNT(index, dir, is_multi))<<4))
#define ACNT_GRP_SMUX_BYTE_CNT_HIGH(index, dir, is_multi) (AC_BASE+4+((ACNT_GRP_SMUX_CNT(index, dir, is_multi))<<4))
#define ACNT_GRP_SMUX_BYTE_CNT_LOW(index, dir, is_multi) (AC_BASE+((ACNT_GRP_SMUX_CNT(index, dir, is_multi))<<4))  

#define CHK_BUF() 		pos = begin + index ; \
						if(pos < off) { \
							index = 0 ; \
							begin = pos ; \
						} \
						if(pos > off + count) \
							goto done ;


#define LAYER2_HEADER(skb)		skb_mac_header(skb)
#define LAYER3_HEADER(skb)		skb_network_header(skb)
#define LAYER4_HEADER(skb)		skb_transport_header(skb)

typedef struct
{
	unsigned long long uniTxCnt;
	unsigned long long uniRxCnt;	
	unsigned long long mulTxCnt;
	unsigned long long mulRxCnt;
	unsigned long long uniTxBytes;
	unsigned long long uniRxBytes;
	unsigned long long mulTxBytes;
	unsigned long long mulRxBytes;
	unsigned long long txRate;
	unsigned long long rxRate;
	unsigned long jiffies;
}ppeAcntGrp_t;


struct hwnat_wan_acnt_args {
    unsigned char wanid;
    unsigned char action;
    ppeAcntGrp_t wan_acnt;
};

/*TCSUPPORT_HWNAT_L2VID*/
#define L2LU_ADD	"add"
#define L2LU_DEL	"del"

#define L2LU_NONTYPE	0
#define L2LU_UNICAST	1
#define L2LU_MULCAST	2

#define L2LU_KEY_ENTRY_NUM 15

#define S_PCP_DTC_MASK (1<<13)
#define S_VID_DTC_MASK (1<<12)
#define C_PCP_DTC_MASK (1<<9)
#define C_VID_DTC_MASK (1<<8)
#define DSCP_MASK (0xff)

#define L2LU_UNBIND		0
#define L2LU_BIND		1
#define L2LU_DELETE		2

typedef struct {
	uint16_t	cvid;
	uint8_t		cpcp;
	uint8_t		dscp;
	uint16_t	svid;
	uint8_t		spcp;
	uint16_t	mask;
	uint8_t		ip_ver;     /* 0: ipv4, 1: ipv6*/
	uint8_t		priority;
	uint16_t	ppe_index;
	uint8_t		bind_status;
	unsigned long	start_time;
}L2LU_key_table_s;

typedef struct {
	uint8_t	priority;
	uint8_t dscp;
	uint32_t ctci;
	uint32_t stci;
}L2LU_key_s;

/* bit0~bit15 port link speed + protocol*/
#define DS_MULTICAST 	(0x01<<0)
#define DS_FTP 			(0x01<<1)
#define DS_HTTP80 		(0x01<<2)
#define DS_HTTP8080 	(0x01<<3)
#define DS_TCP 			(0x01<<4)
#define DS_UDP 			(0x01<<5)
#define DS_MAC 			(0x01<<6)

#define DS_UDFPORT      (0x01<<15)

/* bit16~bit31 protocol only  */
#define RTSP 			(0x01<<16)

#define RTSP_OFFLOAD	0xff
#define RTSP_OFFLOAD_WLAN	0xfe
#define RTSP_PORT   554

#define FP_QDMA1_SW	0

typedef struct 
{
	unsigned short ipv6_sip[8];
	unsigned short ipv6_dip[8];
	unsigned short sport;
	unsigned short dport;
}ppeNatV6Info_t;

typedef struct ifc_hit_info_s
{
    unsigned char  match_action;
    unsigned char  chn;
    unsigned char  txq;
    unsigned char  dir;
    unsigned char  ifc_hit;
    unsigned short ifc_id;
    unsigned short stag;
}ifc_hit_info_t;

typedef struct 
{
	struct iphdr iph;
	unsigned short sport;
	unsigned short dport;
}maptV4IpPort;

typedef struct 
{
	union{
		struct ipv6hdr ip6h;
		maptV4IpPort v4IpPort;
	};
}ppeMapt;

struct FoeEntryExt
{
	unsigned int rx_info;
	struct nf_conntrack* nf_ct;
	unsigned int fe_resource_mark;
    void * dev_info;
	struct net_device	*tx_dev;
	union
	{
		struct natv6_s
		{
			ppeNatV6Info_t natv6_info; 
			unsigned char chn;
			unsigned short stag;
			unsigned char txq;
			unsigned int natv6_attr:6;
			unsigned int natv6_dir:3;
			unsigned int natv6_rtsp:1;
			unsigned int natv6_src_wlan:1; 
			unsigned int pppoe:1;
			unsigned int rsv:20;
		}natv6;
		struct VxlanFlow_kernel vxlan;
        ifc_hit_info_t ifc_hit_info;
	};
	unsigned char forward_left_to_right_type:3;
	unsigned char vxlan_up_2_flag:1;
	unsigned char map_pingpong:1;
	unsigned char rx_dev:1;
    unsigned char resv:2;
    unsigned char dp;
    unsigned char dlf;	
	int gemport;

	unsigned int classify_index:5;	
	unsigned int ds_offload:8;
	unsigned int qos_que_index:4;	
	unsigned int pkt_dir:2;
	unsigned int sp:5;
	unsigned int asym_wlan_dp:8;
	unsigned char smac[ETH_ALEN];
	unsigned char dmac[ETH_ALEN];
    unsigned char npu_pingpong; 
    union
    {
        struct
        {
            unsigned short udf:8;
            unsigned short npu_pingpong_rsv:8;
        };
        unsigned short raw;
    }npu_pingpong_data;
}; 

#ifdef CONFIG_TP_IMAGE
struct FoeEntryIntf
{
	unsigned int act_sp; /* src ifindex */
	unsigned int act_dp; /* dst ifindex */
};
#endif /* CONFIG_TP_IMAGE */

typedef struct {
	unsigned int ratelimit;
	char soft_queue_id;
	unsigned char enable;
}NATV6_WAN_RATE_s;

typedef struct {
	uint16_t	cvid;
	uint16_t	dscp;
	uint8_t		cpcp;
	uint8_t		mask;
}L2LU_key0_table_s;

typedef struct {
	uint16_t	svid;
	uint8_t		spcp;
	uint8_t		mask;
}L2LU_key1_table_s;
/*endif*/

typedef struct {
    uint16_t force_bind:1;
    uint16_t flow_acnt_en:1;
    uint16_t flow_acnt_idx:4;
    uint16_t wan_meter_en:1;
    uint16_t wan_idx:3;  
    uint16_t resv:6;
} foeExtendInfo;

struct BlackHwnatIp {
	struct list_head list;
	unsigned int ipv4;
	unsigned char u6_addr8[16];
	unsigned int mask;
	unsigned int type;
	unsigned int sign;
	unsigned int ipv6_mode;
};

struct BlackHwnatIpPortProtocol{
	struct list_head list;
	unsigned int src_ip_from_wan; /*0xFFFFFFFF means NULL*/
	unsigned int src_port_from_wan; /*0~65535, 65536:NULL*/
	unsigned int protocol; /*1:TCP, 2:UDP, 3:ICMP, 4:ALL*/
};

typedef struct{
	int linkState;	/* 0: down, 1: up */
	int linkSpeed;	/* 0: 10m, 1: 100m , 2: 1g */
}portInfo;

typedef struct 
{
	unsigned int sip;
	unsigned int dip;
	unsigned short sport;
	unsigned short dport;
}udpSessionInfo_t;
typedef struct 
{
	unsigned char enable;
	unsigned char start_flag;
	unsigned short count;
	unsigned short restore_time;
	udpSessionInfo_t info[2];
}udpSessionTest_t;

struct hwnat_tuple_list
{
	struct list_head  list;
	struct hwnat_tuple tuple;
};

#define FOE_EXTEND_START    6

#define FOE_FORCE_BIND(skb)   ((foeExtendInfo *)(&(skb)->foe[FOE_EXTEND_START]))->force_bind
#define FOE_FLOW_ACNT_EN(skb)   ((foeExtendInfo *)(&(skb)->foe[FOE_EXTEND_START]))->flow_acnt_en
#define FOE_FLOW_ACNT_IDX(skb)   ((foeExtendInfo *)(&(skb)->foe[FOE_EXTEND_START]))->flow_acnt_idx
#define FOE_WAN_METER_EN(skb)   ((foeExtendInfo *)(&(skb)->foe[FOE_EXTEND_START]))->wan_meter_en
#define FOE_WAN_IDX(skb)   ((foeExtendInfo *)(&(skb)->foe[FOE_EXTEND_START]))->wan_idx

#define FOE_FLOW_ACNT(skb)      ((FOE_FLOW_ACNT_EN(skb)<<4) + FOE_FLOW_ACNT_IDX(skb))

typedef struct
{
	unsigned int used_num;
	unsigned int total_num;
}HWNAT_RESOURCE_ALLOC_t;

#define HWNAT_RESOURCE_METER_NUM 63
#define HWNAT_RESOURCE_ACCOUNT1 63
#define HWNAT_RESOURCE_ACCOUNT2 16

typedef enum
{
	HWNAT_RESOURCE_TYPE_METER=0,
	HWNAT_RESOURCE_TYPE_ACCOUNT1,
	HWNAT_RESOURCE_TYPE_ACCOUNT2,
	HWNAT_RESOURCE_TYPE_MAX,
}HWNAT_RESOURCE_TYPE;

typedef enum
{
    ACNT_GRP3,
    MTR_GRP2,
    MTR_GRP3,
    DPI,
    UDF, 
    SNPT,
    DNPT,
    NPTV6_MODE0_FOE_NUM,
    NPTV6_MODE1_PREFIX_ID,
    NPTV6_MODE1_PREFIX_LEN,
    L2B_DSCP_REMARK_EN,
    L2TP_SESSIONID,
    EXT_PARA_MAX_NUM
}STATIC_RULE_EXT_PARA;

typedef struct
{
    unsigned char enable[EXT_PARA_MAX_NUM];
    unsigned int value[EXT_PARA_MAX_NUM];
}STATIC_RULE_EXT_t;

typedef enum{
    HWNAT_STATUS_OFF = 0,
    HWNAT_STATUS_ON = 1
}hwnat_status_t;
extern hwnat_status_t hwnat_enable_status;
#define HWNAT_IS_ENABLE()    (hwnat_enable_status)
#define HENAT_STATUS_DISABLE()      (hwnat_enable_status = HWNAT_STATUS_OFF)
#define HENAT_STATUS_ENABLE()      (hwnat_enable_status = HWNAT_STATUS_ON)
#define HENAT_PRINT_ENABLE_STATUS() (printk("current hwnat enable status: %s.\n", hwnat_enable_status ? "on" : "off"))

int PpeDLFRxHandler(struct sk_buff * skb);
/* TCSUPPORT_VLAN_ACCESS_TRUNK_VAL start */
int multicast_vlan_packet_handle(struct sk_buff *skb, uint32_t dest_port);
/* TCSUPPORT_VLAN_ACCESS_TRUNK_VAL end */
u_int16_t PpeGetMacMatchAcntGrpIndex(struct sk_buff * skb);
int PpeHitBindForceToCpuHandler(struct sk_buff *skb, struct FoeEntry *foe_entry);
int setPPeRxACNTGRP(struct sk_buff * skb);
int setPPeTxACNTGRP(struct sk_buff * skb, struct FoeEntry * foe_entry);
uint32_t PpeExtIfRxHandler(struct sk_buff * skb);
uint32_t PpeExtIfRxFastHandler(struct sk_buff * skb);

#endif

