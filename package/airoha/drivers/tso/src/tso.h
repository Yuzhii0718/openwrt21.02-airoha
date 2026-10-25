#ifndef _TSO_H_
#define _TSO_H_


/************************************************************************
*               I N C L U D E S
*************************************************************************
*/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <asm/tc3162/tc3162.h>

#include <linux/if_vlan.h>
#include <net/sock.h>
#include <net/inet_sock.h>
#include <net/ip.h>

#ifdef TCSUPPORT_TSO_V2_ENABLE
#include <ecnt_hook/ecnt_hook_qdma_type.h>
#endif

#include <linux/netdevice.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_tso.h>

#if defined(TCSUPPORT_OPENWRT)
#include "../qdma/qdma_wan/qdma_reg.h"
#elif defined(RDKB_BUILD)
#include "../qdma_reg.h"
#else
#include "../qdma/EN7516/qdma_reg.h"
#endif



/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/



#define PDMA_DSCP_MSG_WORD_LENS  4
#define PDMA_TX_RING_NUM         2
#define PMDA_TXDSCP_NUM          1024
#define PDMA_TX_RING_0           0
#define PDMA_TX_RING_1           1
#define DEFAULT_RECYCLE_THREAD   2

/* reg addr*/
#define PDMA_REG_BASE       0xbfb50000
#define PDMA_TX0_BASE_PTR   (PDMA_REG_BASE + 0x0800)     /*TX Ring 0 Base pointer*/
#define PDMA_TX0_MAX_CNT    (PDMA_REG_BASE + 0x0804)
#define PDMA_TX0_CPU_IDX    (PDMA_REG_BASE + 0x0808)
#define PDMA_TX0_DMA_IDX    (PDMA_REG_BASE + 0x080C)

#define PDMA_TX1_BASE_PTR   (PDMA_REG_BASE + 0x0810)     /*TX Ring 1 Base pointer*/
#define PDMA_TX1_MAX_CNT    (PDMA_REG_BASE + 0x0814)
#define PDMA_TX1_CPU_IDX    (PDMA_REG_BASE + 0x0818)
#define PDMA_TX1_DMA_IDX    (PDMA_REG_BASE + 0x081C)

#define PDMA_TX_SPAN         0x10

#define PDMA_GLO_CFG        (PDMA_REG_BASE + 0x0A04)
#define PDMA_DELAY_INT      (PDMA_REG_BASE + 0x0A0C)
#define PDMA_INT_STATUS     (PDMA_REG_BASE + 0x0A20)
#define PDMA_INT_MASK       (PDMA_REG_BASE + 0x0A28)


#define PDMA_INT_STA_VAL    IO_GREG(PDMA_INT_STATUS)
#define PDMA_INT_MASK_VAL   IO_GREG(PDMA_INT_MASK)
#define PDMA_SET_TX_IDX(ring,val)    IO_SREG(PDMA_TX0_CPU_IDX+ring*PDMA_TX_SPAN,val)

/* interrupt */
#define CONFIG_PDMA_IRQ_1           (PDMA_INTR_1)
#define pdma_isr1_name              "pdma_int1"

#define PDMA_INT_TX0_DONE           (1 << 0)
#define PDMA_INT_TX1_DONE           (1 << 1)

#define TSO_MAX_MSS_COUNT           46
#define TSO_RECORD_DEBUG_ON         1

#define CDM4_TSO_FLAG_MASK          0xbfb52490
/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/


typedef struct {
    uint    resv1 ;
    struct {
#ifdef __BIG_ENDIAN
        uint done               : 1 ;
        uint drop_pkt           : 1 ;   /*drop_pkt for TX_Dscp, overflow for RX_Dscp*/
        uint nls                : 1 ;	/*not last packet, for scatter*/
        uint resv2              : 13 ;
        uint pkt_len            : 16 ;
#else
        uint pkt_len            : 16 ;
        uint resv2              : 13 ;
        uint nls                : 1 ;	/*not last packet, for scatter*/
        uint drop_pkt           : 1 ;   /*drop_pkt for TX_Dscp, overflow for RX_Dscp*/
        uint done               : 1 ;
#endif /* __BIG_ENDIAN */
    } ctrl ;
    uint 	pkt_addr ;
    struct {
#ifdef __BIG_ENDIAN
    uint resv3                  : 20 ;
    uint next_idx               : 12 ;
#else
    uint next_idx               : 12 ;
    uint resv3                  : 20 ;
#endif /* __BIG_ENDIAN */
    } next;
    uint msg[PDMA_DSCP_MSG_WORD_LENS] ;
} PDMA_DMA_DSCP_T ;

typedef union {
	struct {
#ifdef __BIG_ENDIAN
        uint            : 1 ;
        uint mic_idx    : 1 ;
        uint sp_tag     : 16 ;
        uint ico        : 1;
        uint uco        : 1;
        uint tco        : 1;
        uint tso        : 1;
        uint fast       : 1 ;
        uint oam        : 1 ;
        uint channel    : 5 ;
        uint queue      : 3 ;
#else
        uint queue      : 3 ;
        uint channel    : 5 ;
        uint oam        : 1 ;
        uint fast       : 1 ;
        uint tso        : 1;
        uint tco        : 1;
        uint uco        : 1;
        uint ico        : 1;
        uint sp_tag     : 16 ;
        uint mic_idx    : 1 ;
        uint            : 1 ;
#endif
	
#ifdef __BIG_ENDIAN
        uint no_drop    : 1;	/*means not be dropped by QDMA*/
        uint mtr_g      : 7;	/*0x7f means not use meter*/
        uint fPort      : 9;
        uint hwf        : 1;
        uint resv       : 3;
        uint act_grp    : 11;

#else
        uint act_grp    : 11;
        uint resv       : 3;
        uint hwf        : 1;
        uint fPort      : 9;
        uint mtr_g      : 7;
        uint no_drop    : 1;
#endif
    } raw ;
    uint msg[2] ;
} tsoTxMsg_t ;



struct PDMA_DscpInfo_S {
    PDMA_DMA_DSCP_T          *dscpPtr ;
    uint                     dscpIdx ;
    struct sk_buff           *skb ;
    struct PDMA_DscpInfo_S   *next ;
} ;

struct TSO_MODE_S {
    unchar      isTxPolling         :1 ;
    unchar      isRxPolling         :1 ;
    unchar      isRxNapi            :1 ;
    unchar      isIsrRequest        :1 ;
    unchar      resv                :4 ;
};

typedef struct{
    unsigned long               csrBaseAddr ;               /* reg Base */
    spinlock_t                  txLock[PDMA_TX_RING_NUM] ;       /* spin lock for Tx */
    spinlock_t                  irqLock ;                   /* spin lock for IRQ */
    spinlock_t                  txDoneLock ;                /* spin lock for tx transmit done */
    ushort                      txDscpNum[PDMA_TX_RING_NUM] ;    /* Total TX DSCP number */
    unsigned long               dscpInfoAddr ;              /* Start pointer for DSCP information node */
    unsigned long               txBaseAddr[PDMA_TX_RING_NUM] ;
    uint                        txCounts[PDMA_TX_RING_NUM]   ;
    uint                        recycleIdx[PDMA_TX_RING_NUM] ;
    struct PDMA_DscpInfo_S      *txHeadPtr[PDMA_TX_RING_NUM] ;   /* Head node for unused tx desc. */
    struct PDMA_DscpInfo_S      *txTailPtr[PDMA_TX_RING_NUM] ;   /* Tail node for unused tx desc. */
    struct PDMA_DscpInfo_S      *txUsingPtr[PDMA_TX_RING_NUM] ;  /* TXDMA using DSCP node. */
    struct TSO_MODE_S           flags;
    atomic_t                    free_txd_num[PDMA_TX_RING_NUM];
    int                         recycleThr[PDMA_TX_RING_NUM];	
    int                         dbgLevel;
}PDMA_Private_T;

#ifdef TCSUPPORT_TSO_VERIFY
typedef union {
#ifdef TCSUPPORT_CPU_EN7580
    struct {
#ifdef __BIG_ENDIAN
        uint            : 1;
        uint mic_idx    : 1;
        uint sp_tag     : 16;
        uint ico        : 1;
        uint uco        : 1;
        uint tco        : 1;
        uint tso        : 1;
        uint fast       : 1;
        uint oam        : 1;
        uint channel    : 5;
        uint queue      : 3;
#else
        uint queue      : 3;
        uint channel    : 5;
        uint oam        : 1;
        uint fast       : 1;
        uint tso        : 1;
        uint tco        : 1;
        uint uco        : 1;
        uint ico        : 1;
        uint sp_tag     : 16;
        uint mic_idx    : 1 ;
        uint            : 1 ;
#endif

#ifdef __BIG_ENDIAN
        uint no_drop    : 1;	/*means not be dropped by QDMA*/
        uint mtr_g      : 7;	/*0x7f means not use meter*/
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)	
		uint fPort		: 4;
		uint nboq		: 5;
		uint resv		: 5;
#else
		uint fPort		: 3;
		uint nboq		: 5;
		uint resv		: 6;
#endif

        uint udf_pmap   : 6;
        uint ico_2      : 1;
        uint uco_2      : 1;
        uint tco_2      : 1;
        uint tso_2      : 1;
#else
        uint tso_2      : 1;
        uint tco_2      : 1;
        uint uco_2      : 1;
        uint ico_2      : 1;
        uint udf_pmap   : 6;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)			
		uint resv		: 5;
		uint nboq		: 5;
		uint fPort		: 4;
#else
		uint resv		: 6;
		uint nboq		: 5;
		uint fPort		: 3;
#endif

        uint mtr_g      : 7;	/*0x7f means not use meter*/
        uint no_drop    : 1;	/*means not be dropped by QDMA*/
#endif
    } raw ;
#else
    struct {
#ifdef __BIG_ENDIAN
        uint resv       : 4;
        uint sp_tag     : 16;
        uint oam        : 1;
        uint channel    : 8;
        uint queue      : 3;
#else
        uint queue      : 3;
        uint channel    : 8;
        uint oam        : 1;
        uint sp_tag     : 16;
        uint resv       : 4;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
        uint ico        : 1;
        uint uco        : 1;
        uint tco        : 1;
        uint tso        : 1;
        uint udf_pmap   : 6;
        uint fPort      : 3;
        uint vlanEn     : 1;
        uint vlanTpID   : 2;
        uint vlanTag    : 16;
#else
        uint vlanTag    : 16;
        uint vlanTpID   : 2;
        uint vlanEn     : 1;
        uint fPort      : 3;
        uint udf_pmap   : 6;
        uint tso        : 1;
        uint tco        : 1;
        uint uco        : 1;
        uint ico        : 1;
#endif /* __BIG_ENDIAN */
    } raw ;
#endif
    uint msg[2] ;
} tsoTestTxMsg_t ;
#endif


#endif
