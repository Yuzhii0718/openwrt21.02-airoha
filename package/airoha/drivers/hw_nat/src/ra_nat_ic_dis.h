#ifndef _RA_NAT_IC_DIS_H
#define _RA_NAT_IC_DIS_H

#define SUPPORT_HASH_MODE4				(isEN7581 || isAN7552 || isAN7583)	/*hash mode support hash mode 4*/
#define SUPPORT_ALLOC_DMA_BUF_IN_KERNEL	(isEN7581 || isAN7583)	/*alloc ppe buffer in kernel in advance*/
#define SUPPORT_SRAM_ENTRY              (isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)  /* Support PPE entry in SRAM */
#define SUPPORT_FAST_PATH               (isEN7580 || isEN7581 || isAN7583)              /* Support fast path accelerate */
#define SUPPORT_QDMA_HQOS               (isEN7523 || isEN7581 || isAN7552 || isAN7583)              /* Support QDMA HQoS mode */
#define SUPPORT_VPM_FROM_PARSER         (isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)  /* Get VPM from parser info */
#define SUPPORT_STAG_FROM_INFO          (isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)  /* Get sp-tag from mac info */
#define SUPPORT_PSE_NBQ                 (isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)  /* Support PSE NBQ setting */
#define SUPPORT_XSI_INTERFACE           (isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)  /* Support XSI interface */
#define SUPPORT_RX_RING_FROM_QID        (isEN751627)                        /* EN7527/EN7528, Get Rx Ring info from Qid */
#define SUPPORT_METER_ACNT_IN_QDMA      (isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)  /* Support meter & account function in QDMA */
#define SUPPORT_OLT_INTERFACE           (isAN7583)  /* Support OLT interface */

#define PPE_CACHE_NOT_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define ACNT_GRP_SUPPORT		(isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_QID_CFG_SUPPORT		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_SIZE_CFG_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define PPE_L2B_ENTRY_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define PPE_SHNK_TABLE_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_MC_CFG_SUPPORT		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_GRE_CFG_SUPPORT		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_CHNL_CFG_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_NBQ_CFG_SUPPORT		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_PORT_CFG_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define SRAM_DRAM_HASH_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_HASH_SHIFT_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_HASH_CRC32_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FID_INTO_HASH_SUPPORT	(isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* IFC ID into PPE hash */
#define PPE_MIRROR_SUPPORT		(isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* mirror hwnat packet to assigned dest port */
#define SUPPORT_PPE_MIRROR_EXTENTION	(isEN7581)		/* mirror hwnat packet extention */
#define SUPPORT_PPE_MIRROR_DEC	(isAN7552 || isAN7583)		/* ppe mirror dscision */
#define PPE_WAN_MTU_SUPPORT 	(isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define FOE_BINDR_SUPPORT		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* Set reach bind rate */
#define PPE_DRAM_HASH1_SUPPORT	(isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define PPE_SECOND_VLAN_SUPPORT	(isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* support L2B second vlan into hash */
#define PPE_SMAC_HASH_SUPPORT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583) 
#define SUPPORT_PPE_FP_MTU_SETTING 		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583) /* Support PPE ForcePort mtu setting */
#define SUPPORT_NEW_REGIS_PPE_FP_MTU	(isEN7523 || isEN7581 || isAN7552 || isAN7583) /*FP mtu resgister modified after EN7523*/ 
#define SUPPORT_FOE_TS_CFG		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* time_stamp cfg */		
#define SUPPORT_FOE_SMAC_CFG	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* source mac cfg */
#define SUPPORT_ENTRY_FILTER	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* filter unavailable PPE entry */
#define SUPPORT_ENTRY_PRINT		(isEN7523 || isEN7581 || isAN7552 || isAN7583)				/* print ppe entry in kernel task */
#define SUPPORT_METER_RATELIMIT	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* meter ratelimit */
#define SUPPORT_FLOW_ACNT		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* flow accounting */
#define SUPPORT_FOE_FAST_CFG	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* info2 fast bit cfg */
#define SUPPORT_ETYPE_CFG		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* PPE_L2B_ETYPE_EN cfg */
#define SUPPORT_MAC_BANDWIDTH	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* hwnat mac bandwidth */
#define SUPPORT_PASS_THRH_CFG	(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* 6RD and DS-Lite pass through */
#define SUPPORT_NPTV6			(isEN7523 || isEN7581 || isAN7552 || isAN7583)
#define SUPPORT_L2B_BC			(isEN7523 || isEN7581 || isAN7552 || isAN7583)				/* PPE support L2B broadcast */
#define SUPPORT_L2_INFO_CFG		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* set Layer2 information */
#define SUPPORT_WDMA			(isEN7580 || isEN7523 || isEN7581 || isEN7528 || isAN7552 || isAN7583)
#define SUPPORT_DS_NTO_QDMA		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* downstream not to qdma */
#define SUPPORT_NEW_TXMSG		(isEN7580 || isEN7523 || isEN7581 || isAN7552 || isAN7583)	/* new tx msg format */
#define SUPPORT_TUNNEL_HW_ACCELE	(isEN7581 || isAN7552 || isAN7583) /**/
#define PPE_FIN_FAST_RP_NOT_SUPPORT	(isAN7552) /*7552 need to close FIN_FAST_RP*/

#define SUPPORT_L2B_DSCP_REMARK	(isAN7552 || isAN7583) /* support L2B flow dscp remark*/
#define SUPPORT_SRAM_MAX_512	(isEN7523 || isAN7552)                          /*sram max entry is 512*/
#define SUPPORT_QDMA_MULTICAST_1toN     (isEN7523 || isEN7581 || isAN7552 || isAN7583)      /*support qdma multicast 1toN*/
/*Change the dport (after grecallid) to 0x5a00 (origin is 0xa500) in static rule when grecallid
is enable in 7581*/
#define GreCallIdEn		(isEN7581 || isAN7583)	

#define SUPPORT_ETHERNET_SERDES	(isEN7581 || isAN7552 || isAN7583)							/* support ethernet serdes */
#define SUPPORT_GDMA4			(isEN7581 || isAN7583)
#define SUPPORT_QDMA_32_RX_RING	(isEN7581 || isAN7583)							/* support QDMA rx 32 ring */

/*
    disable speical cpu reason: bit 1/2/3/4/8/9/0xa/0xb/0x17
    OSBNB00146531
*/
#define DISABLE_SPECIAL_CRSN_FOR_IFC		         (isEN7580 || isEN7523)
#define SUPPORT_DSLITE_HSK_16B	(isEN7580 || isEN7523 || isEN7581 || isAN7552)		/* support ds-lite new hsk */	 
#define SUPPORT_QDMA_3S_ACCOUNT_METER		(isAN7552 || isAN7583)

#define SUPPORT_HIT_IFC_ACTION		                 (isEN7523 || isEN7581 || isAN7552)
#define SUPPORT_PPPOE_LEN_CAL_USE_IP_LEN	(isEN7581 || isAN7552)				/*support hwnat pppoe length cal according to ipheader len*/


#define SUPPORT_NAT66_CPU_OFFLOAD		(isAN7552)

#define SUPPORT_L2TP_IN_HASH	(isAN7583)
#define TUN_OFFSET_IN_HASH_SUPPORT	(isAN7583)	/* tunnel offset into PPE hash */

#define TDMA_WDAM_SAME_PORT	(isAN7583)
#define SUPPORT_QDMA_AUTOMODE		(isAN7583)
#endif
