#ifndef _FE_IC_DIS_H_
#define _FE_IC_DIS_H_

/* FE VIP, except EN7580 */
#define SUPPORT_VIP	(!isEN7580)

#define SUPPORT_PSE_FC_CFG (isEN7581)
/*workround for fragment packet GDM checksum error hw bug,disable GDM TCP/UDP checksum error drop,only in chip 7580*/
#define FRAG_PKT_CKS_ERROR_WORKROUND (isEN7580)

/* use gdmp to expand pse buffer, support EN7523, AN7552 */
#define SUPPORT_PSE_USE_GDMP	(isEN7523 || isAN7552)

/* default pse share threshold, support EN7523, AN7552 */
#define SUPPORT_PSE_SHARE_THRESHOLD	(isEN7523 || isAN7552)

/* support GDMA2 */
#define SUPPORT_GDMA2			(isEN7580 || isEN7523 || isEN7581 || isAN7583)

/* support GDMA3 */
#define SUPPORT_GDMA3			(isEN7580 || isEN7523 || isEN7581 || isAN7583)

/* support GDMA4 */
#define SUPPORT_GDMA4			(isEN7581 || isAN7583)

/* support PPE2 */
#define	SUPPORT_PPE2			(isEN7581)

#define SUPPORT_AEWAN_FWDFQ     ( isEN7523 || isEN7581 || isAN7583)

/*support detect GDM2 IQ counter*/
#define SUPPORT_GDM2_IQ_MONITOR		(isEN7581 || isAN7583)		

/* FE OQ CNT TYPE */
#define SUPPORT_FE_OQ_CNT_ENHANCE	(isAN7583)

/* FE and NPU bridge MBI enhance */
#define SUPPORT_CDM3_MBI_ENHANCE	(isAN7583)

/* 7581_E1 serdes EOF */
#define SERDES_EOF	(isEN7581 && (1 == GET_PDIDR()))
#define ARBITER_SUPPORT  (isEN7581 || isAN7583)

/* pse page size = 128byte */
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_PSE_PAGE_128	1
#else
#define SUPPORT_PSE_PAGE_128	0
#endif

/* qdma fc mapping, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define SUPPORT_QDMA_FC_MAP	1
#else
#define SUPPORT_QDMA_FC_MAP	0
#endif

/* cdm oq mapping, support EN7523, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_CDM_OQ_MAP	1
#else
#define SUPPORT_CDM_OQ_MAP	0
#endif

/* fragment and assemble, support EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define SUPPORT_FRAG_ASSEM	1
#else
#define SUPPORT_FRAG_ASSEM	0
#endif

/* tunnel config table, support EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define SUPPORT_TUNNEL_OFFLOAD	1
#else
#define SUPPORT_TUNNEL_OFFLOAD	0
#endif

/* FE SRAM Power down, support EN7581 */
#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_FE_SRAM_POWER_DOWN	1
#else
#define SUPPORT_FE_SRAM_POWER_DOWN	0
#endif

/* pse copy engine, support EN7523, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
#define COPY_ENGINE_SUPPORT	1
#else
#define COPY_ENGINE_SUPPORT	0
#endif

/* GDM3, support EN7580 */
#if defined(TCSUPPORT_CPU_EN7580)
#define GDM3_SUPPORT	1
#else
#define GDM3_SUPPORT	0
#endif

/* GDM4, support EN7581 */
#if defined(TCSUPPORT_CPU_EN7581)
#define GDM4_SUPPORT	1
#else
#define GDM4_SUPPORT	0
#endif

/* traffic class, support EN7580, EN7523, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7580)
#define IFC_TC_SUPPORT	1
#else
#define IFC_TC_SUPPORT	0
#endif

/* cpu reason queue selection, support EN7580, EN7523, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7580)
#define CRSN_QSEL_SUPPORT	1
#else
#define CRSN_QSEL_SUPPORT	0
#endif

/* FE MAC range, support EN7580, EN7523, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7580)
#define FE_MAC_RANGE_SUPPORT	1
#else
#define FE_MAC_RANGE_SUPPORT	0
#endif

/* GDM2 RX eth MIB, support EN7523, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
#define GDM2_RX_ETH_MIB_SUPPORT	1
#else
#define GDM2_RX_ETH_MIB_SUPPORT	0
#endif

/* GDM2 TX eth MIB, support EN7523, EN7581, AN7552 */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
#define GDM2_TX_ETH_MIB_SUPPORT	1
#else
#define GDM2_TX_ETH_MIB_SUPPORT	0
#endif

/* pse buffer in oq intead of iq,start from 7580 */
#if defined(TCSUPPORT_CPU_EN7580)
#define SUPPORT_PSE_BUFFER_IN_OQ	1
#else
#define SUPPORT_PSE_BUFFER_IN_OQ	0
#endif

/* pse oq&iq buffer cfg, start from 7523 */
#if defined(TCSUPPORT_CPU_EN7523)
#define SUPPORT_PSE_QUEUE_CFG	1
#else
#define SUPPORT_PSE_QUEUE_CFG	0
#endif

/*set pon serdes as lan*/
#if defined(TCSUPPORT_CPU_AN7552)
#define SUPPORT_AEWAN_LAN_MODE 1
#else
#define SUPPORT_AEWAN_LAN_MODE 0
#endif

#if defined(TCSUPPORT_CPU_AN7583)
#define HWF_QDMA_SEL_SUPPORT	1
#else
#define HWF_QDMA_SEL_SUPPORT	0
#endif

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
#define SUPPORT_QDMA_3S_ACCOUNT_METER 1
#else
#define SUPPORT_QDMA_3S_ACCOUNT_METER 0
#endif

#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_VIP_IFC_EN 1       /*support enable or disable of vip and ifc*/
#else
#define SUPPORT_VIP_IFC_EN 0
#endif

#if defined(TCSUPPORT_CPU_EN7523)
#define SUPPORT_EVENT_SYSTEM  	1
#else
#define SUPPORT_EVENT_SYSTEM  	0
#endif

/*for 7563, but 7563 the same as 7563*/
#define SUPPORT_DISABLE_UNUSE_OQ  ((isAN7552) && (0))
#define SUPPORT_CDM_HW_OQ_CONF    ((isAN7552) && (0))

#if defined(TCSUPPORT_SWITCH_8851)
#define SUPPORT_EN8851 1
#else
#define SUPPORT_EN8851 0
#endif

#if defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_FE_IPV6_EXT 			1
#else
#define SUPPORT_FE_IPV6_EXT 			0
#endif

#define SUPPORT_DETECT_PSE_EXCEPTION		(isAN7552)

#if defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_FAQ_AGGRE			1
#else
#define SUPPORT_FAQ_AGGRE			0
#endif

#if defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_FORCE_TO_SLOW	1	    
#else
#define SUPPORT_FORCE_TO_SLOW	0
#endif

/*qdma channe6&7 for woe,default fc is mapping to gdm3(P3) oq0&1 for 7581*/
/*reconfig mapping to cdm4(P7) oq10&11 for 7552&7583*/
#if defined(TCSUPPORT_CPU_AN7583)||defined(TCSUPPORT_CPU_AN7552)
#define RECONFIG_WOE_FC 1
#else
#define RECONFIG_WOE_FC 0
#endif

#if defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_FTTR_ON_GDM3 1
#else
#define SUPPORT_FTTR_ON_GDM3 0
#endif

#if defined(TCSUPPORT_CPU_EN7581)
#define SUPPORT_OQ_RSV_ADJUST 1
#else
#define SUPPORT_OQ_RSV_ADJUST 0
#endif

#define SUPPORT_IFC_NUM_NOT_ENOUGH	 (isEN7523)

#if defined(TCSUPPORT_CPU_AN7583)
#define SUPPORT_SLOWPATH_OQ_ADJUST 1
#else
#define SUPPORT_SLOWPATH_OQ_ADJUST 0
#endif

#endif /* _FE_IC_DIS_H_*/