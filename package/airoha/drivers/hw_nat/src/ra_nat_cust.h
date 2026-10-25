#ifndef _RA_NAT_CUST_H
#define _RA_NAT_CUST_H

#define MAX_DEV_BANDWIDTH_NUM 30

#define HWNAT_MULTI_ACNTGRP ((isEN7580)?0x1E:0x3D)

/* Account Group: */
/* 0~15 for wan interface */
/* 16~31 for traffic flow */
/* 32~55 for FHT ai wifi Lan Ip */
/* Traffic qos, Up & Down, use hwnat meter and counter idx from 10 to 25 */
#define UP_QUEUE_CNT_START		16
#define DOWN_QUEUE_CNT_START	(UP_QUEUE_CNT_START+8)
#define UP_QUEUE_NUM			8
#define NO_TRAFFIC_QOS			-1
#define BYTE_HIGH_UNIT			0x100000000

#define LIMIT_GROUP_REMARK_MASK   	(0x3F)
#define LIMIT_GROUP_REMARK_OFFSET 	(5)
#define FWC_START_ACNTGRP_ID (9)

#define UNKNOW_MC_ACNT_ID       60
#define UNKNOW_MC_METER_ID      60

#define LANIP_CNT_START 26

#define lanIpMaxAcNum 15
#define AWIFI_ITF "br-lan"
typedef struct
{
	uint8_t brlan_exist;
	uint8_t ip_exist;
	uint32_t ipddr;
	uint32_t mask;
}br_ip;

typedef struct
{
	uint32_t ipaddr;
	unsigned long long  up_cnt;
	unsigned long long  down_cnt;
}lanip_acnt;

typedef struct
{
	lanip_acnt acnt[lanIpMaxAcNum];
	uint8_t currentIdx;
}lanip_account_info;

typedef struct
{
    uint32_t mulCnt;
    unsigned long long mulBytes;
}ppeMulAcntGrp_t;

typedef struct devBandwidth_s{
	unsigned char mac[6];
	unsigned long long upBytes;
	unsigned long long downBytes;
	unsigned int upRate;
	unsigned int downRate;
	int valid;
}devBandwidth_t;

typedef struct devBandwidthList_s{	
	int enable;
	int portMultiMacEn;
	struct devBandwidth_s bandwidthList[MAX_DEV_BANDWIDTH_NUM];
}devBandwidthList_t;

enum GetFlowId_Mode {
	NOT_WORK_MODE	= 0,
	CMCCV2_MODE		= 1,
	XPON_HAL_MODE	= 2,
	CMCC_MODE		= 3,
	CSC_EEUROPE_MODE= 4
};

struct Ppe_VIP_t{
	struct list_head list;

	int hash_index;
	unsigned int  unbind_sip;
	unsigned short sport;
	unsigned short dport;
	unsigned char tcp_flag;
	
	int count;
	struct timer_list mul_vip_timer;
	struct timer_list VIP_expire_timer;
};

typedef enum {
	METER_MODE_NULL,
	METER_MODE_DEV_MAC,
	METER_MODE_WAN
} PPE_METER_MODE;

#endif
