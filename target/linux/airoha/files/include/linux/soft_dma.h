#ifndef _LINUX_ECNT_OFFLOAD_SHORT_CUT_H
#define _LINUX_ECNT_OFFLOAD_SHORT_CUT_H
#include <linux/netdevice.h>

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
#define TWO_WAY_LEFT_TO_RIGHT   1
#define LEFT_TO_RIGHT_QDMA_WAN_TX_2   1
#endif

#define DATA_ADDR_OFFSET    96 /* must be n*32 */

#ifdef TWO_WAY_LEFT_TO_RIGHT
#define OFFLOAD_LOOP_NUM    2

/*first way, qdma lan rx, qdma lan tx*/
#define OFFLOAD_DATA_SIZE   2048 /* must be n*32 */

#define OFFLOAD_RX_RING_SIZE    2048
#define OFFLOAD_TX_RING_SIZE    2048
#define OFFLOAD_RX_RING_MASK    (OFFLOAD_RX_RING_SIZE - 1)
#define OFFLOAD_TX_RING_MASK    (OFFLOAD_TX_RING_SIZE - 1)
#define OFFLOAD_DATA_NUM (OFFLOAD_RX_RING_SIZE+OFFLOAD_TX_RING_SIZE)
#define OFFLOAD_DATA_MASK (OFFLOAD_DATA_NUM - 1)


#if defined(TCSUPPORT_CPU_EN7580)
#define OFFLOAD_RX_RING     10
#define OFFLOAD_TX_RING     2
#define QDMA_RX_IDX (0xBFB54208+((OFFLOAD_RX_RING)<<5))
#define QDMA_TX_IDX (0xBFB54108+((OFFLOAD_TX_RING)<<5))
#else
#define OFFLOAD_RX_RING     10
#define OFFLOAD_TX_RING     1
#define QDMA_RX_IDX (0xBFB54808+((OFFLOAD_RX_RING-2)<<5))
#define QDMA_TX_IDX 0xBFB54110
#endif
/* second way */
#define OFFLOAD_DATA_SIZE_2   2048 /* must be n*32 */

#define OFFLOAD_RX_RING_SIZE_2    2048
#define OFFLOAD_TX_RING_SIZE_2    2048
#define OFFLOAD_RX_RING_MASK_2    (OFFLOAD_RX_RING_SIZE_2 - 1)
#define OFFLOAD_TX_RING_MASK_2    (OFFLOAD_TX_RING_SIZE_2 - 1)
#define OFFLOAD_DATA_NUM_2 (OFFLOAD_RX_RING_SIZE_2+OFFLOAD_TX_RING_SIZE_2)
#define OFFLOAD_DATA_MASK_2 (OFFLOAD_DATA_NUM_2 - 1)

#if defined(TCSUPPORT_CPU_EN7580)
#define OFFLOAD_RX_RING_2     10
#define OFFLOAD_TX_RING_2     2
#define QDMA_RX_IDX_2 (0xBFB56208+((OFFLOAD_RX_RING_2)<<5))
#define QDMA_TX_IDX_2 (0xBFB56108+((OFFLOAD_TX_RING_2)<<5))
#else
#define OFFLOAD_RX_RING_2     10
#define OFFLOAD_TX_RING_2     1
#define QDMA_RX_IDX_2 (0xBFB55808+((OFFLOAD_RX_RING_2-2)<<5))
#define QDMA_TX_IDX_2 0xBFB55110
#endif

extern int two_way_left_to_right_flag;
#else/* TWO_WAY_LEFT_TO_RIGHT */
#define OFFLOAD_DATA_SIZE   2048 /* must be n*32 */

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
#define OFFLOAD_RX_RING_SIZE    2048
#define OFFLOAD_TX_RING_SIZE    1024
#define OFFLOAD_RX_RING_MASK    (OFFLOAD_RX_RING_SIZE - 1)
#define OFFLOAD_TX_RING_MASK    (OFFLOAD_TX_RING_SIZE - 1)
#define OFFLOAD_DATA_NUM (OFFLOAD_RX_RING_SIZE+OFFLOAD_TX_RING_SIZE)
#define OFFLOAD_DATA_MASK (OFFLOAD_DATA_NUM - 1)

#define OFFLOAD_RX_RING     10

#if defined(TCSUPPORT_CPU_EN7580)
#define OFFLOAD_TX_RING     2
#define QDMA_RX_IDX (0xBFB54208+((OFFLOAD_RX_RING)<<5))
#define QDMA_TX_IDX (0xBFB54108+((OFFLOAD_TX_RING)<<5))
#else
#define OFFLOAD_TX_RING     1
#define QDMA_RX_IDX (0xBFB54808+((OFFLOAD_RX_RING-2)<<5))
#define QDMA_TX_IDX 0xBFB54110
#endif

#define OFFLOAD_LOOP_NUM    2

#else
#define OFFLOAD_RX_RING_SIZE    1024
#define OFFLOAD_TX_RING_SIZE    1024
#define OFFLOAD_RX_RING_MASK    (OFFLOAD_RX_RING_SIZE - 1)
#define OFFLOAD_TX_RING_MASK    (OFFLOAD_TX_RING_SIZE - 1)
#define OFFLOAD_DATA_NUM (OFFLOAD_RX_RING_SIZE+OFFLOAD_TX_RING_SIZE)
#define OFFLOAD_DATA_MASK (OFFLOAD_DATA_NUM - 1)

#define QDMA_RX_IDX 0xBFB54118
#define OFFLOAD_RX_RING     1

#define QDMA_TX_IDX 0xBFB55110
#define OFFLOAD_TX_RING     1

#define OFFLOAD_LOOP_NUM    1000

#endif
#endif  /* TWO_WAY_LEFT_TO_RIGHT */

#define K1_TO_PHY(x)			(((uint)(x)) & 0x1fffffff)

#define QDMA_WAN_CHANNEL_FOR_LAN    25
#define QDMA_WAN_CHANNEL_FOR_LAN_BASE    24

#define IP_HEADER_LEN	20
#define UDP_HEADER_LEN	8
#define VXLAN_LEN	8

#define LEFT_TO_RIGHT_NPTV6_UP_METER    61
#define LEFT_TO_RIGHT_NPTV6_DOWN_METER    62

typedef struct OFFLOAD_Data_s {
	unsigned long data[OFFLOAD_DATA_SIZE/sizeof(unsigned long)];
} OFFLOAD_Data_t;

typedef enum
{
	TYPE_NATV6 = 0,
	TYPE_VXLAN
}FORWARD_LEFT_TO_RIGHT_TYPE;

typedef enum
{
	DIR_UP_SUCCESS = 0,
	DIR_DOWN_SUCCESS,
	DIR_UP_FRAG_SUCCESS,
	LEFT_TO_RIGHT_RET_FAIL
}FORWARD_LEFT_TO_RIGHT_RET;

typedef enum
{
	UP_RATELIMIT = 1,
    DOWN_RATELIMIT 
}RATELIMIT_MODE;

typedef enum
{
	TEST_MODE_NATV6 = 1,
	TEST_MODE_VXLAN
}FORWARD_LEFT_TO_RIGHT_TEST_MODE;

typedef struct 
{
	unsigned short ipv6_sip[8];
	unsigned short ipv6_dip[8];
	unsigned short sport;
	unsigned short dport;
}ppeNatV6InfoKernel_t;

struct natv6_kernel
{
    ppeNatV6InfoKernel_t natv6_info; 
    unsigned char chn;
    unsigned short stag;
    unsigned char txq;
    unsigned int natv6_attr:6;
    unsigned int natv6_dir:3;
    unsigned int natv6_rtsp:1;
	unsigned int natv6_src_wlan:1;
	unsigned int pppoe:1;
    unsigned int rsv:20;
};

struct VxlanFlow_kernel
{
	/* IP header */
	unsigned char iph[IP_HEADER_LEN];

	/* UDP header */
	unsigned char uh[UDP_HEADER_LEN];

    /* Vxlan header */
    unsigned char vxlanhdr[VXLAN_LEN];

    char store_flag;
};

struct wan_rate_s_kernel
{
    unsigned char wan_rate_bind:1;      /* binded to wan ratelimit ? */
    unsigned char wan_dir:3;
    unsigned char wan_meter_id:4;
    unsigned char chnl;
    unsigned short stag;
    unsigned char txq;
};
struct meter_s_kernel
{
    char meterIsOn;
    char upMeterId;
    char downMeterId;
};

#if !defined(TCSUPPORT_HWNAT_V3)
struct FoeEntryExtKernel
{
	unsigned int rx_info;
	unsigned int rx_dev;
	struct nf_conntrack* nf_ct;
	unsigned int fe_resource_mark;
	int gemport;
	char dir;
	int wan_itf;
	int ip_type;    /* 0 means ipv4, 1 means ipv6*/
	union {
		__be32 ip4;
		struct in6_addr ip6;
	} u;
	unsigned char mac[6];
	int acntGrpGeneralIdx;
	int acntGrpWanItfIdx;
	int acntGrpMulticastIdx;
	unsigned char dlf;
	void * dev_info;
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	unsigned short mul_br_vid;
#endif
    union{
        struct natv6_kernel natv6;
        struct VxlanFlow_kernel vxlan;
    };
    unsigned char forward_left_to_right_type:3;
    unsigned char resv:5;
    struct wan_rate_s_kernel wan_rate;
    unsigned char dp;
	union{
		struct meter_s_kernel meter;		
	};
};
#else
struct FoeEntryExtKernel
{
	unsigned int rx_info;
	unsigned int rx_dev;
	struct nf_conntrack* nf_ct;
	unsigned int fe_resource_mark;
	void * dev_info;
	union{
		struct natv6_kernel natv6;
		struct VxlanFlow_kernel vxlan;
	};
	unsigned char forward_left_to_right_type:3;
	unsigned char vxlan_up_2_flag:1;
    unsigned char resv:4;
    unsigned char dp;
    unsigned char dlf;	
	int gemport;

	unsigned int classify_index:5;	
	unsigned int ds_offload:8;
	unsigned int qos_que_index:4;	
	unsigned int pkt_dir:2;
	unsigned int sp:5;
	unsigned int rsv:8;
	unsigned char smac[ETH_ALEN];
	unsigned char dmac[ETH_ALEN];
}; 
#endif

typedef struct 
{
	int offset;
	unsigned short len;
    unsigned char flag;
}data_len_offset_t;

extern int aggressive_offload_short_cut_mode;
extern int left_to_right_down_mode;
extern int left_to_right_test_mode;

static inline int get_offload_short_cut_mode(void)
{
    return aggressive_offload_short_cut_mode;
}

static inline int is_natv6_offload_short_cut(void)
{
    return (aggressive_offload_short_cut_mode & (1<<TYPE_NATV6));
}

static inline int is_vxlan_offload_short_cut(void)
{
    return (aggressive_offload_short_cut_mode & (1<<TYPE_VXLAN));
}

static inline int is_natv6_left_to_right_down_mode(void)
{
    return left_to_right_down_mode;
}

extern int (*vxlan_left_to_right_handle_hook)(char *data_ptr, unsigned short *data_len, unsigned short foe_index, int *data_offset, unsigned short *VirIfIdx);
extern int (*vxlan_left_to_right_xmit_hook)(char *data_ptr, unsigned short *data_len, unsigned short foe_index, struct net_device *dev);
extern void (*update_vxlan_up_info_hook)(unsigned int tcont, unsigned int gem, unsigned int vlan, unsigned char* smac, unsigned char* dmac);
extern void (*update_nptv6_info_hook)(unsigned int chn, unsigned int stag, unsigned int *ipv6_ip, int dir);
#endif

