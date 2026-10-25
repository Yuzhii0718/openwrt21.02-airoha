#ifndef _LINUX_ECNT_SKBUFF_H
#define _LINUX_ECNT_SKBUFF_H
#include <linux/version.h>
#include <linux/kernel.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <linux/kmemcheck.h>
#endif
#include <linux/compiler.h>
#include <linux/time.h>
#include <linux/bug.h>
#include <linux/cache.h>

#include <linux/atomic.h>
#include <asm/types.h>
#include <linux/spinlock.h>
#include <linux/net.h>
#include <linux/textsearch.h>
#include <net/checksum.h>
#include <linux/rcupdate.h>
#include <linux/hrtimer.h>
#include <linux/dma-mapping.h>
#include <linux/netdev_features.h>
#include <linux/sched.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
#include <net/flow_keys.h>
#endif
#if defined(TCSUPPORT_RA_HWNAT)
#include <linux/foe_hook.h>
#endif
#include <linux/ecnt_skb_mark_info.h>

extern void skbmgr_4k_pool_init(void);
extern void skbmgr_pool_init(void);

extern atomic_t g_used_skb_num;
extern int g_max_skb_num;
extern int peak_skb_num;

#ifdef TCSUPPORT_DOWNSTREAM_QOS
#define VOIP_RX_PORT_NUM 4
#endif

#define MODE_HGU 0
#define MODE_SFU 1

#if defined(CONFIG_CPU_TC3162) || defined(CONFIG_MIPS_TC3262) || defined(TCSUPPORT_ECNT_SKBMGR)
//only one skbmgr pool for every CPU. shnwind 20101215.
#define SKBMGR_SINGLE_QUEUE 

#ifdef SKBMGR_SINGLE_QUEUE
#define SKBMGR_QUEUE_ID				0
#define SKBMGR_MAX_QUEUE			1
#else
#define SKBMGR_QUEUE_ID				smp_processor_id()
#define SKBMGR_MAX_QUEUE			NR_CPUS
#endif

extern atomic_t skbmgr_alloc_no;
extern atomic_t skbmgr_4k_alloc_no;

#define SKBMGR_INDICATION        	1
#define SKBMGR_4K_INDICATION       	2
#if defined(TCSUPPORT_LRO_ENABLE)
extern atomic_t skbmgr_lro_alloc_no;
#define SKBMGR_LRO_INDICATION       	3
#endif
#endif


#define MULTICAST_MASK 		0xf0000

#if 1//def CONFIG_QOS

#if 0//!defined(TCSUPPORT_MULTI_USER_ITF)
#define LANIF_MASK					0xf0000000
#endif
	
#if defined(TCSUPPORT_CT)
	//#define WANIF_MASK					0x7f0000
#else
#endif
	
#define MULTICAST_MASK 		0xf0000

#if defined(TCSUPPORT_IPV6_CEROUTER)
#define POLICY_FAIL_MASK 			(1 << 25)
#endif
#if !defined(TCSUPPORT_MULTI_USER_ITF)
#define SKBUF_TCCONSOLE				(1 << 27)
#endif

	/* the last bit is used as route policy mask */
#define ROUTE_POLICY_MASK			(1 << 24)
	//#define QOS_WANIF_MARK			0xff000
	//#define QOS_DSCP_MARK 			0x3f00000
#endif


/******************************************************************************************************************
*pon_mark(32bi)                                                                                                   
*31...............| 14..12 | 11 | 10 | 9..8 | 7 | 6 | 5 | 4...0 |                                                 
*                   |                     |     |   |   |    |   |    |                                           
*                   |                     |     |   |   |    |   |    |----------0x1f QOS_TSID_MARK               
*                   |                     |     |   |   |    |   |-----------0x20 QOS_TSE_MARK                    
*                   |                     |     |   |   |    |------------0x40 DS_TRTCM_ENABLE_MARK               
*                   |                     |     |   |   |-------------0x80 DS_QUEUE_ENABLE_MARK                   
*                   |                     |     |   |--------------0x100 DS_PKT_MAPPING_TO_ONE  |                 
*                   |                     |     |---------------0x200 DS_PKT_MAPPING_TO_MULTI  |0x300 DS_PKT_MAPPING_MARK 
*                   |                     | --------------0x400 DS_PKT_FORM_WAN                                           
*                   |
*                   |-----------------------0x7000 DS_QUEUE_ID_MARK                                                   
*                                                                                                                  
*                                                                                                                  
*                                                                                                                  
*******************************************************************************************************************/



#define QOS_TSID_MARK					0x0000001f
#define QOS_TSE_MARK					0x00000020
#define DS_TRTCM_ENABLE_MARK			0x00000040
#define DS_QUEUE_ENABLE_MARK            0x00000080
#define DS_PKT_MAPPING_TO_ONE			0x00000100
#define DS_PKT_MAPPING_TO_MULTI		    0x00000200
#define DS_PKT_MAPPING_MARK			    0x00000300
#define DS_PKT_FORM_WAN				    0x00000400
#define DS_QUEUE_ID_MARK				0x00007000
#define DS_TRTCM_ID_MARK				QOS_TSID_MARK
#define setDownQueueID(x,y)             do{(x) &= (~DS_QUEUE_ID_MARK); (x)  |= ((y << 12)&DS_QUEUE_ID_MARK);}while(0)         
#define getDownQueueID(x)               ( ( (x) & DS_QUEUE_ID_MARK ) >> 12 )
#define setDownQueueEnable(x,y)                 (x = (y ? (x|DS_QUEUE_ENABLE_MARK) : (x & ~DS_QUEUE_ENABLE_MARK)))
#define getDownQueueEnable(x)                   ((x & DS_QUEUE_ENABLE_MARK) ? 1 : 0)

#define PON_PKT_FROM_CPE		(1<<0)
#define PON_PKT_FROM_LAN		(1<<1)
#define PON_PKT_FROM_WLAN		(1<<2)
#define PON_PKT_FROM_WAN		(1<<3)
#define PON_PKT_FROM_USB		(1<<4)
#define PON_PKT_FROM_IGMP		(1<<5)
#define PON_PKT_INSERT_FLAG		(1<<6)
#define PON_PKT_ROUTING_FLAG	(1<<7)
#define PON_PKT_SEND_TO_WAN		(1<<8)
#define PON_VLAN_RX_CALL_HOOK	(1<<9)
#define PON_VLAN_TX_CALL_HOOK	(1<<10)
#define PON_USER_GROUP_FLAG		(1<<11)
#define PON_PKT_VOIP_RX			(1<<12)
#define PON_PKT_VOIP_TX			(1<<13)
#define PON_MULTICAST_ANI_FILTER_FLAG (1<<14)
#define PON_LEAVE_PKT_DEAL		      (1<<15)

#define PON_PKT_TR69_RX			(1<<17)
#define PON_PKT_TR69_TX			(1<<18)
#define PON_PKT_DROP_FLAG		(1<<19)
#define PON_PKT_TRACE_FLAG      (1<<20)

#define PON_PKT_FROM_HYBRID_PPTP      (1<<21)  
#define PON_PKT_FROM_HYBRID_VEIP      (1<<22) 
#define PON_PKT_FROM_HYBRID_SFU_WAN   (1<<23) 

#if defined(TCSUPPORT_PON_MAC_FILTER)
#define PKT_FROM_LAN		(1<<0)
#define PKT_FROM_WAN		(1<<1)
#define PKT_SEND_TO_WAN		(1<<2)
#define PKT_FILTER_FLAG		(1<<3)
#define PON_MAC_FILTER_RX_CALL_HOOK		(1<<4)
#define PON_MAC_FILTER_TX_CALL_HOOK		(1<<5)
#define PKT_SEND_TO_LAN				(1<<6)
#endif

#define		VLAN_PACKET				(1<<0)
#define		VLAN_2TAGS_PACKET		(1<<1)
#define		ROUTING_MODE_PACKET		(1<<2)
#define		VLAN_TAG_FROM_INDEV		(1<<3)	
#define		VLAN_TAG_INSERT_FLAG	(1<<4)
#define		VLAN_TAG_CHECK_FLAG		(1<<5)
#define		VLAN_TAG_FROM_WAN		(1<<6)
#define		VLAN_TAG_FOR_DNS		(1<<7)
#ifdef TCSUPPORT_LAN_VLAN
#define		VLAN_TAG_FOR_DHCP		(1<<8)
#endif

#define		VLAN_TAG_PBIT_RESERVE0	(1<<8)
#define		VLAN_TAG_PBIT_RESERVE1	(1<<9)
#define		VLAN_TAG_PBIT_RESERVE2	(1<<10)
#define		VLAN_TAG_PBIT_RESERVE3	(1<<11)

#if defined(TCSUPPORT_CT_PORTSLIMIT)
#define		VLAN_TAG_IGMP_QUERYFLAG	(1<<12)
#endif
#if defined(TCSUPPORT_CT_DS_LIMIT)
#define		DATASPEED_LIMIT_ENABLE	(1<<13)
#endif
#ifdef TCSUPPORT_LAN_VLAN
#define     VLAN_TAG_FOR_BOARDCAST  (1<<12)
#endif
#define     VLAN_TAG_FOR_CFI        (1<<14)

#define 	VLAN_CFI_MASK		0x1000 /* Canonical Format Indicator */

#define	VLAN_TAG_PBIT_REMARK_ENABLE	(1<<15)
#define	VLAN_TAG_PBIT_REMARK_BIT1	(1<<16)
#define	VLAN_TAG_PBIT_REMARK_BIT2	(1<<17)
#define	VLAN_TAG_PBIT_REMARK_BIT3	(1<<18)

#define	VLAN_TAG_MULTICAST_VLAN 	(1<<19)

#define		XPON_IGMP_IS_MULTICAST	(1<<0)
#define		XPON_IGMP_UPSTREAM_RESTORE	 (1<<1)
#define		XPON_IGMP_UPSTREAM_RECOVERY	(1<<2)
#define		XPON_IGMP_DOWNSTREAM_VLAN_HANDLE (1<<3)

#if defined(TCSUPPORT_CT)
#ifdef TCSUPPORT_PORTBIND//CONFIG_PORT_BINDING
#define MASK_ORIGIN_DEV   		0x1 	/* flag for port bind set origin dev name */
#define MASK_OUT_DEV   			0x2 	/* flag for port bind set origin dev name */
#define	IFNAMSIZ				16
#endif
#else
#ifdef TCSUPPORT_PORTBIND /*CONFIG_PORT_BINDING*/
#define MASK_ORIGIN_DEV   		0x1 	/* flag for port bind set origin dev name */
#define MASK_OUT_DEV   			0x2 	/* flag for port bind set origin dev name */
#define	IFNAMSIZ				16
#endif
#endif

/*simulation unmatch flag in skb->mark,downstream only*/
#define DOWNSTREAM_SIMULATION_MASK	(1<<31)

//#ifdef CONFIG_SMUX		/*include/linux/if.h*/
/* smux calls */
#define SIOCSIFSMUX     0x89c0 /* add or rem smux interface */
//#endif

#ifdef TCSUPPORT_XPON_HAL_API_EXT
#define PKTQOS_QUEUE_LENGTH  (256)
#else
#define PKTQOS_QUEUE_LENGTH  (64)  //(128) /*  16*8  */
#endif

#if defined(TCSUPPORT_FWC_ENV)
/*
*define skb->mark for FHWC
*
*|31...29|28...17| 16 | 15 | 14 |
*    |       |      |    |    | 
*    |       |      |    |    |----> L2, L3 data flow flag, 0:L3, 1:L2
*    |       |      |    |---------> Pbit remark flag, set if pbit is remarked
*    |       |      |--------------> Queue remark flag, if set, see bits 13-11
*    |       |---------------------> reserve
*    |-----------------------------> FH reserve for Bind
*
*|13 12 11|10...5|4...1| 0 |
*     |       |     |    |
*     |       |     |    |---------> XPON reserve
*     |       |     |--------------> From port,0~3:Lan, 4-7:wifi, 8:USB, 15:PON
*     |       |--------------------> 
*     |----------------------------> Queue number
*     
*/

#define FHWC_L2_L3_MARK_POS (14)

#define FHWC_MARK_ROUTE_PKT(skb)  (skb->mark &= ~(1<<FHWC_L2_L3_MARK_POS))
#define FHWC_MARK_BRIDGE_PKT(skb) (skb->mark |= 1<<FHWC_L2_L3_MARK_POS)

#define FHWC_IS_MARKED_ROUTE_PKT(skb)  (!FHWC_IS_MARKED_BRIDGE_PKT(skb))
#define FHWC_IS_MARKED_BRIDGE_PKT(skb) ((skb->mark >> FHWC_L2_L3_MARK_POS) & 0x01)
#endif

#define L2TP_OFFLOAD_PACKET (1<<1)

#define TSO_ENABLE_MARK	0x1
#define TSO_STATIC_SKB	(0x1 << 1)
#define TSO_SKB_USING	(0x1 << 2)
extern int check_lanport_type(int index);
extern unsigned char get_lan_info(int index);
extern int get_hsgmii_idx(int serdes);
extern int get_board_flag(void);

extern int itf_start_idx;
extern char *lan_itf;
extern char *sep_itf;
extern char *ae_wan_itf;
extern int aewan_idx;

extern int isLANInterface(struct net_device *dev);
extern int is24GWDSInterface(struct net_device *dev);
extern int is5GWDSInterface(struct net_device * dev);
extern int is24GWiFiInterface(struct net_device *dev);
extern int is5GWiFiInterface(struct net_device *dev);
extern int is6GWiFiInterface(struct net_device *dev);
extern int is24GAPCLIInterface(struct net_device *dev);
extern int is5GAPCLIInterface(struct net_device *dev);
extern int is6GAPCLIInterface(struct net_device *dev);
extern int isAPCLIInterface(struct net_device *dev);
extern int isWiFiInterface(struct net_device *dev);
extern int isUSBInterface(struct net_device *dev);
extern int isPONInterface(struct net_device *dev);
extern int isWANInterface(struct net_device *dev);
extern int isBridgeInterface(struct net_device *dev);
extern int isXSIInterface(struct net_device *dev);
extern int isMAPInterface(struct net_device *dev);
extern int isGREInterface(struct net_device *dev);
extern int isPPTPInterface(struct net_device *dev);
extern int isL2TPInterface(struct net_device *dev);
extern int getSwitchLANIndex(struct net_device *dev);
extern int getSwitchLANIndexByName(char *pname);
extern int getLogicLANIndex(struct net_device *dev);
extern int getLogicLANIndexByName(char *pname);
extern char *lanNamePre(void);
extern int get24GWifiIndex(struct net_device *dev);
extern int get5GWifiIndex(struct net_device *dev);
extern int get6GWifiIndex(struct net_device *dev);
extern int get24GWDSIndex(struct net_device *dev);
extern int get5GWDSIndex(struct net_device *dev);
extern int get24GLogicWifiIndex(struct net_device *dev);
extern int get5GLogicWifiIndex(struct net_device *dev);
extern int get6GLogicWifiIndex(struct net_device *dev);
extern int getUSBIndex(struct net_device *dev);
extern int get_aewan_idx(void);
extern int ecnt_global_vlan_allowed_egress(const struct net_device *dst_dev,
							const struct net_device *src_dev);

#if defined(TCSUPPORT_CMCCV2)
extern __be32 ecnt_ipv4_dev_get_saddr_by_index(
	struct net *net, int if_index);

extern int ecnt_ipv6_dev_get_saddr_by_index(
	struct net *net, int if_index, struct in6_addr *ifa_saddr, int max_addr);
#endif
extern struct net_device *get24GWifiName(int index);
extern struct sk_buff *__vlan_put_tag(struct sk_buff *skb, u16 vlan_tci);

#if defined(TCSUPPORT_WAN_GPON)
#define IS_PKT_FROM_DS_UNICAST_GEM(skb)   \
    ({ \
        typecheck(struct sk_buff *, skb); \
        ((skb->pon_mark & DS_PKT_FORM_WAN) && (0 == skb->gem_type) ); \
    })
#define IS_PKT_FROM_DS_MULTICAST_GEM(skb)  \
    ({ \
        typecheck(struct sk_buff *, skb); \
        ((skb->pon_mark & DS_PKT_FORM_WAN) && (1 == skb->gem_type) ); \
    })
#endif

#if (defined(TCSUPPORT_WAN_GPON) || defined (TCSUPPORT_WAN_EPON))
#define SKB_GET_PON_MARK(skb) \
    ({ \
        typecheck(struct sk_buff *, skb); \
        (skb->pon_mark); \
    })
#else
#define SKB_GET_PON_MARK(skb)	(0)
#endif

#define FOE_FREE_STACK_MAX_DEPTH 10
struct ecnt_sk_buff {
#if defined(TCSUPPORT_XPON_HYBIRD)
    unsigned char wan_if;// 0:gpon0, 1:wan0
#endif
	unsigned char stag_inserted;
	unsigned int foe_index;
};
typedef struct ecnt_sk_buff ecnt_sk_buff_t;

#ifdef TCSUPPORT_XPON_HAL_API_MCST
typedef struct IGMP_HWNATEntry_s
{
	struct list_head  list;
        #ifdef TCSUPPORT_MULTICAST_SPEED
        struct rcu_head rcu;
        #endif
	int proto;   //ipv6 or ipv4
	int index;   //get from skb buff
	int vlan_tag_num;
	int outer_vlan;
	int inner_vlan;
	unsigned int  m_vlan;  //source VLAN info, TPID + TCI
	short int  hwnat_vid; 
	short int wifinum; //wifi ra counts
	//short int lannum; //lan counts
	// bit0-->bit3 for eth0.1-->eth0.4  bit8-->bit11 for ra0-->ra3 bit12~bit15 for rai1~rai4
	unsigned long long  mask;   //port mask
	short int bindvid;
	struct net_device* orig_dev;
	unsigned char grp_addr[16];
	unsigned char src_addr[16];
	unsigned int local;
	struct timer_list age_timer;
}IGMP_HWNATEntry_t;

typedef struct 
{
	struct list_head list;
	int proto;
	unsigned char grp_addr[16];
	unsigned int ref_cnt;
}IGMP_LOCALEntry_t;
#else
/*move from br_private.h*/
/*this struct is also for hw nat using, not add compile option*/
typedef struct IGMP_HWNATEntry_s
{
	struct list_head  list;
	#ifdef TCSUPPORT_MULTICAST_SPEED
	struct rcu_head rcu;
	#endif
	int proto;
	int index;
	unsigned int  m_vlan;  //source VLAN info, TPID + TCI
	
// bit0-->bit3 for eth0.1-->eth0.4  bit8-->bit11 for ra0-->ra3 bit12~bit15 for rai1~rai4
	unsigned long long	mask;	//port mask
	unsigned char wifinum;
	struct net_device* orig_dev;
	unsigned char grp_addr[16];
	unsigned char src_addr[16];
	unsigned int local;
	struct timer_list age_timer;
	unsigned char snoop_off;
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	unsigned short br_vid;
#endif
}IGMP_HWNATEntry_t;

typedef struct 
{
	struct list_head list;
	int proto;
	unsigned char grp_addr[16];
	unsigned int ref_cnt;
}IGMP_LOCALEntry_t;
#endif

typedef struct 
{       
    struct list_head list; 
    int              index;
    unsigned long long    port_mask;
}multicast_flood_hwentry_t;

#endif
