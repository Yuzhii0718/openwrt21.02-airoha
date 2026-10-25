#ifndef MTK_ESP_H
#define MTK_ESP_H

#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/kernel.h>
#include <asm/tc3162/tc3162.h>

#define HASH_DIGEST_OUT		0
#define HASH_DIGEST_IN			1
#define CRYPTO_ENCRYPTION		1
#define CRYPTO_DECRYPTION		2

#ifdef TCSUPPORT_IPSEC_PASSTHROUGH
#define CONFIG_VPN_PASSTHROUGH_LOG
#define CONFIG_VPN_PASSTHROUGH_INFO
#endif

#define VPN_DEFAULT_MAX_MTU 1500
#define DECRIPTION_DIRECTION 0
#define ENCRIPTION_DIRECTION 1

#define VPN_PASSTHROUGH_SWITCH_ON 1
#define VPN_PASSTHROUGH_SWITCH_OFF 0

#define IPESC_EIP93_ADAPTERS	32
#define SKB_QUEUE_MAX_SIZE		100

#define	IPSEC_MAX_FOE_NUM		(16)
#define	UNAVAILABLE_FOE_IDX		(-1)


/************************************************************************
*      E X T E R N E L    S T R U C T U R E    D E F I N I T I O N
*************************************************************************
*/
#if 1 /*def TCSUPPORT_IPSEC_PASSTHROUGH*/
enum vpn_passthrough_para{
	VPN_PASSTHROUGH_DEBUG_INFO = 0,
	VPN_PASSTHROUGH_DEBUG_LEVER,
	VPN_PASSTHROUGH_SWITCH,
	VPN_PASSTHROUGH_SKB_QUEUE_MAX_NUM,
};

#ifdef CONFIG_VPN_PASSTHROUGH_LOG
#ifndef VPN_LOG
typedef enum
{
	VPN_PASSTHROUGH_LOG_OFF = 0,
    VPN_PASSTHROUGH_LOG_ERROR,
    VPN_PASSTHROUGH_LOG_WARNING , 
    VPN_PASSTHROUGH_LOG_DBG,
    VPN_PASSTHROUGH_LOG_TRACE,  
    VPN_PASSTHROUGH_LOG_DUMP_DATA, 
    VPN_PASSTHROUGH_LOG_MAX_LEVER, 
} SIF_LOG_LEVER;
#endif
#endif

typedef struct entrynum_s
{
	int32 firstperiod;
	int32 secondperiod;
}entrynum_t;

typedef struct d_info_s
{
	uint32 d_phase1_normal_counts;
	uint32 d_phase1_speed_counts;
	uint32 d_phase1_drop_counts;
	uint32 d_phase2_normal_counts;
	uint32 d_phase2_speed_counts;
	uint32 d_phase2_drop_counts;
} d_info_t;

typedef struct e_info_s
{
	uint32 e_phase1_normal_counts;
	uint32 e_phase1_speed_counts;
	uint32 e_phase1_drop_counts;
	uint32 e_phase1_drop_counts_1;
	uint32 e_phase1_drop_counts_2;
	uint32 e_phase2_normal_counts;
	uint32 e_phase2_speed_counts;
	uint32 e_phase2_drop_counts;
} e_info_t;

typedef struct vpnPassThrough_Stats_s
{
	struct d_info_s dinfo;
	struct e_info_s einfo;
}vpnPassThrough_Stats_t;
			
typedef struct vpnPassThrough_Global_Para
{
	vpnPassThrough_Stats_t statistics;
	uint32 vpndebuglevel;
	uint32 vpnpassthroughswitch;
	uint32 vpnskbqueuemaxnum;
}vpnPassThrough_Global_Para_t;

#endif

typedef union
{
	struct
	{
	#ifdef TCSUPPORT_LITTLE_ENDIAN
        unsigned int hostReady      : 1;
        unsigned int peReady        : 1;
        unsigned int reserved       : 1;
        unsigned int initArc4       : 1;
        unsigned int hashFinal      : 1;
        unsigned int haltMode       : 1;
        unsigned int prngMode       : 2;
        unsigned int padValue       : 8;
        unsigned int errStatus      : 8;
        unsigned int padCrtlStat    : 8;
    #else
		unsigned int padCrtlStat	: 8;
		unsigned int errStatus		: 8;
		unsigned int padValue		: 8;
		unsigned int prngMode		: 2;
		unsigned int haltMode		: 1;
		unsigned int hashFinal		: 1;
		unsigned int initArc4		: 1;
		unsigned int reserved		: 1;
		unsigned int peReady		: 1;
		unsigned int hostReady		: 1;
    #endif
	} bits;
	unsigned int word;
		
} peCrtlStat_t;

typedef union
{
	struct
	{
	#ifdef TCSUPPORT_LITTLE_ENDIAN
        unsigned int length         : 20;
        unsigned int reserved       : 2;
        unsigned int hostReady      : 1;
        unsigned int peReady        : 1;
        unsigned int byPass         : 8;    
    #else
		unsigned int byPass			: 8;
		unsigned int peReady		: 1;
		unsigned int hostReady		: 1;
		unsigned int reserved		: 2;
		unsigned int length			: 20;
    #endif
	} bits;	
	unsigned int word;
		
} peLength_t;

typedef struct addrHandler_s
{
	unsigned long addr;
	dma_addr_t	 phyAddr;

} addrHandler_t;

typedef struct eip93DescpHandler_s
{
	peCrtlStat_t	peCrtlStat;
	addrHandler_t	srcAddr;
	addrHandler_t	dstAddr;
	addrHandler_t	saAddr;
	addrHandler_t	stateAddr;
	addrHandler_t	arc4Addr;
	unsigned int	userId;
	peLength_t		peLength;

} eip93DescpHandler_t;

typedef struct addrsDigestPreCompute_s
{
	unsigned int 		*hashKeyTank;
	addrHandler_t 		ipadHandler;
	addrHandler_t 		opadHandler;
	unsigned int 		blkSize;
	eip93DescpHandler_t *cmdHandler;
	addrHandler_t 		saHandler;
	addrHandler_t 		stateHandler;
	addrHandler_t 		stateHandler2;
	unsigned int 		digestWord;
	unsigned int 		*pIDigest;
	unsigned int 		*pODigest;

} addrsDigestPreCompute_t;

typedef struct ipsecEip93Adapter_s
{
	unsigned int 				spi; //every ipsec flow has a unique spi
	struct xfrm_state 			*x; //the SA
	unsigned int 				isHashPreCompute; //0:pre-compute init, 1:inner digest done, 2:inner digest done, 3:pre-compute done
	unsigned int 				isEncryptOrDecrypt; //1:encrypt, 2:decrypt
	struct sk_buff_head 		skbQueue;
	addrsDigestPreCompute_t		*addrsPreCompute; //for hash pre-compute
	eip93DescpHandler_t 		*cmdHandler; //for encrypt/decrypt
	spinlock_t 					lock;
	unsigned int 				addedLen; //refer to ssh_hwaccel_alloc_combined() in safenet_la.c
#if 1 /* def TCSUPPORT_IPSEC_PASSTHROUGH */	
	struct entrynum_s foeentryindex[IPSEC_MAX_FOE_NUM];
	//below two parameters for input
	struct net_device *dev;
    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,55)) && defined(CONFIG_SKB_EXTENSIONS)
    unsigned char		active_extensions;
	/* only useable after checking ->active_extensions != 0 */
	struct skb_ext		*extensions;
    #else
	struct sec_path *sp;
    #endif
	//below  one parameter for output
	struct dst_entry *xdst;
	//below one parameter for both
	struct dst_entry *dst; 
    unsigned int escape_ip;
#endif

} ipsecEip93Adapter_t;

#if 1 /* def TCSUPPORT_IPSEC_PASSTHROUGH */
//default open for hw_nat
typedef struct adapterlistpara_s
{
	int para;//0:dst
	struct sk_buff *skb;
    unsigned int escape_ip;
}adapterlistpara_t;

/*below define is for ipsec passthrough period II
	because input finish handle is the same as the output finish handle,so use the same strcuture
*/
typedef struct pushinfo_s
	{
		eip93DescpHandler_t *resHandler;
		ipsecEip93Adapter_t *currentAdapter;
	}pushinfo_t;
	
	typedef struct pullinfo_s
	{
		struct sk_buff *skb;
		unsigned int  entry_index;
	}pullinfo_t;

	typedef struct ipsec_finishpara_s
	{
		union
		{
			struct pushinfo_s learn_push;
			struct pullinfo_s learn_pull;	
		}data;
	
		int flag;
	}ipsec_finishpara_t;


/*below define is for ipsec passthrough period I
	because input handle is the same as the output handle,so use the same strcuture
*/
	typedef struct learninfo_s
	{
		struct sk_buff *skb;
		struct xfrm_state *x;
	}learninfo_t;

	typedef struct speedinfo_s
	{
		struct sk_buff *skb;
		unsigned int entry_idx;
	}speedinfo_t;

	typedef struct ipsec_para_s
	{
		union
		{
			struct learninfo_s learn;
			struct speedinfo_s speed;	
		}data;
	
		int flag;
	}ipsec_para_t;
#endif

/************************************************************************
*      E X T E R N E L     F U N C T I O N    D E C L A R A T I O N
*************************************************************************
*/
void ipsec_set_phase1_foe_idx(ipsecEip93Adapter_t *AdapterPtr,struct sk_buff *skb);
void ipsec_set_phase2_foe_idx(ipsecEip93Adapter_t *AdapterPtr,struct sk_buff *skb);


#endif

