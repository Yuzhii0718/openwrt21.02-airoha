/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _FOE_FDB_7580_H
#define _FOE_FDB_7580_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include "hwnat_ioctl.h"
#include "frame_engine.h"
#include "../fe/en7512/fe_api.h"

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define PPE_TYPE_L2B_ONLY       1
#define PPE_TYPE_L2B_IPV4       2
#define PPE_TYPE_L2B_IPV4_IPV6  3

#define PPE_FOE_SIZE_32 32
#define PPE_FOE_SIZE_64 64
#define PPE_FOE_SIZE_80 80

#define IS_64_BYTE_MODE 0
#define IS_80_BYTE_MODE 1

#define PPE_FOE_SIZE_BYTE_MODE IS_64_BYTE_MODE // 0 means foe size is 64byte

#define PPE_FOE_ENTRY_NUM	16

#define PPE_TYPE_DEFAULT	PPE_TYPE_L2B_IPV4
#define PPE_FOE_SIZE_DEFAULT	PPE_FOE_SIZE_64
#define DRAM_HASH_IDX_BASE	(0x4000)

#define PPE_FOE_NUM_256  256
#define PPE_FOE_NUM_512  512
#define PPE_FOE_NUM_1K  1024
#define PPE_FOE_NUM_2K  2048
#define PPE_FOE_NUM_4K  4096
#define PPE_FOE_NUM_8K  8192
#define PPE_FOE_NUM_16K 16384
#define PPE_FOE_NUM_32K 32768
#define PPE_FOE_NUM_48K 49152
#define PPE_FOE_NUM_56K 57344
#define PPE_FOE_NUM_63DOT5K 65024

#define FOE_NUM_SIZE_16 16
#define FOE_NUM_SIZE_48 48

#define FOE_NUM_DEFAULT FOE_NUM_SIZE_16
#define SRAM_FOE_NUM_DEFAULT FOE_NUM_SIZE_16

#define FOE_ENTRY_SIZ		128	/* for ipv6 backward compatible */

#define PPE_CLEAR_OFFSET1 16
#define PPE_CLEAR_OFFSET2 40
#define PPE_CLEAR_OFFSET3 40
#define PPE_CLEAR_OFFSET4 24

/* force port define */ 
#define FP_PDMA				(0)
#define FP_GDMA1			(1)
#define FP_GDMA2			(2)
#define FP_GDMA3			(3)  /* EN7580 add this port */
#define FP_GDMA4			(9)  /* EN7581 add this port */
#define FP_PPE				(4)
#define FP_PPE2				(8)  /* EN7581 add this port */
#define FP_QDMA_SW			(5)
#define FP_QDMA1_HW			(1)  /* need set fqos bit to goto fwd path */
#define FP_QDMA_HW			(2)  /* need set fqos bit to goto fwd path */
#define FP_DROP				(7)	
#define FP_NPU				(6)
#define FP_TDMA				(7)
#define FP_QDMA_MCAST       (0xe)/* EN7523 add this port & need set fqos bit to goto fwd path*/
#define FP_WDMA				FP_GDMA3
#define FP_ETH_SERDES       (0) /*not exist*/ 
#define WDMA_RX0_OQ		0
#define WDMA_RX1_OQ		1

#define METER_REG_PKT_OFF_MODE  0
#define METER_REG_PKT_OFF_TRTCM 1
#define METER_REG_PKT_OFF_TICK  2
#define METER_REG_PKT_OFF_RATE  4
#define METER_REG_PKT_OFF_BUCK  15

#define METER_REG_BYTE_OFF_MODE     0
#define METER_REG_BYTE_OFF_TRTCM    1
#define METER_REG_BYTE_OFF_TICK     2
#define METER_REG_BYTE_OFF_RATE     3
#define METER_REG_BYTE_OFF_BUCK     17

#define TUNNEL_TABLE_NUM (10)
#define TUNNEL_DOWNSTART (TUNNEL_TABLE_NUM/2)

#define IFC_ID_IN_HASH        0x3f
#define NPU_BRIDGE_CHNL7	0

/**********************
*            Private Register
**********************/
#define PPE_CRSN_MASK	(RALINK_PPE_BASE + 0x3F0)
#define AC_BASE		    (RALINK_FRAME_ENGINE_BASE + 0x2000)
#define METER_BASE2	    (RALINK_FRAME_ENGINE_BASE + 0x200C)
#define PPE_GRP_Meter_CNT(index)   (METER_BASE2+((index)<<4))

#define SRAM_TB_EN							(1<<31)
#define DRAM_TB_EN							(1<<30)
#define SRAM_HASH1_EN						(1<<29)

#define SRAM_HASH1_MODE_SHIFT				22
#define SRAM_HASH1_MODE_MASK				(0x3<<SRAM_HASH1_MODE_SHIFT)
#define SRAM_HASH0_MODE_SHIFT				20
#define SRAM_HASH0_MODE_MASK				(0x3<<SRAM_HASH0_MODE_SHIFT)
#define DRAM_HASH_MODE_SHIFT				14
#define DRAM_HASH_MODE_MASK					(0x3<<DRAM_HASH_MODE_SHIFT)

#define sramFlowTableEnable()				IO_SBITS(PPE_TB_CFG, SRAM_TB_EN)
#define sramFlowTableDisable()				IO_CBITS(PPE_TB_CFG, SRAM_TB_EN)
#define isSramFlowTableEnable()				(IO_GREG(PPE_TB_CFG) & SRAM_TB_EN)
#define dramFlowTableEnable()				IO_SBITS(PPE_TB_CFG, DRAM_TB_EN)
#define dramFlowTableDisable()				IO_CBITS(PPE_TB_CFG, DRAM_TB_EN)
#define isDramFlowTableEnable()				(IO_GREG(PPE_TB_CFG) & DRAM_TB_EN)
#define sramHash1ModeEnable()				IO_SBITS(PPE_TB_CFG, SRAM_HASH1_EN)
#define sramHash1ModeDisable()				IO_CBITS(PPE_TB_CFG, SRAM_HASH1_EN)
#define isSramHash1ModeEnable()				(IO_GREG(PPE_TB_CFG) & SRAM_HASH1_EN)

#define ppeSetSramHash1Mode(val)			IO_SMASK(PPE_TB_CFG, SRAM_HASH1_MODE_MASK, SRAM_HASH1_MODE_SHIFT, val)
#define ppeGetSramHash1Mode()				IO_GMASK(PPE_TB_CFG, SRAM_HASH1_MODE_MASK, SRAM_HASH1_MODE_SHIFT)
#define ppeSetSramHash0Mode(val)			IO_SMASK(PPE_TB_CFG, SRAM_HASH0_MODE_MASK, SRAM_HASH0_MODE_SHIFT, val)
#define ppeGetSramHash0Mode()				IO_GMASK(PPE_TB_CFG, SRAM_HASH0_MODE_MASK, SRAM_HASH0_MODE_SHIFT)
#define ppeSetDramHashMode(val)				IO_SMASK(PPE_TB_CFG, DRAM_HASH_MODE_MASK, DRAM_HASH_MODE_SHIFT, val)
#define ppeGetDramHashMode()				IO_GMASK(PPE_TB_CFG, DRAM_HASH_MODE_MASK, DRAM_HASH_MODE_SHIFT)
/************************************************************************
*               M A C R O S
*************************************************************************
*/
/* differential parts from foe_fdb_7510.h */
/**** Start **************************/
#define FOE_4TB_SIZ_SRAM    gPpeSramFoeNum
#define FOE_4TB_SIZ_DRAM    gPpeFoeNum
#define FOE_4TB_SIZ_TOTAL   (gPpeFoeNum+DRAM_HASH_IDX_BASE)
#define SIZE_OF_FOE_ENTRY   gPpeFoeSize
/**** End ***************************/

#ifdef __BIG_ENDIAN
#define IP_FORMAT(addr) \
	((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]
#else
#define IP_FORMAT(addr) \
	((unsigned char *)&addr)[3], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[0]
#endif

#define IS_IPV4_HNAPT(x)	(((x)->bfib1.pkt_type == IPV4_HNAPT) ? 1: 0)
#define IS_IPV4_HNAT(x)		(((x)->bfib1.pkt_type == IPV4_HNAT) ? 1 : 0)
#define IS_L2_RRIDGE(x)	(((x)->bfib1.pkt_type == L2_BRIDGE) ? 1 : 0)
#define IS_IPV4_DSLITE(x)	(((x)->bfib1.pkt_type == IPV4_DSLITE) ? 1 : 0)
#define IS_IPV6_3T_ROUTE(x)	(((x)->bfib1.pkt_type == IPV6_3T_ROUTE) ? 1 : 0)
#define IS_IPV6_5T_ROUTE(x)	(((x)->bfib1.pkt_type == IPV6_5T_ROUTE) ? 1 : 0)
#define IS_IPV6_6RD(x)		(((x)->bfib1.pkt_type == IPV6_6RD) ? 1: 0)
#define IS_IPV4_GRP(x)		(IS_IPV4_HNAPT(x) | IS_IPV4_HNAT(x) | IS_IPV4_DSLITE(x))
#define IS_IPV6_GRP(x)		(IS_IPV6_3T_ROUTE(x) | IS_IPV6_5T_ROUTE(x) | IS_IPV6_6RD(x))
        
/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
enum FoeTblSize {
	FoeTblSize_1K = 0,
	FoeTblSize_2K,
	FoeTblSize_4K,
	FoeTblSize_8K,
	FoeTblSize_16K,
	FoeTblSize_32K,
	FoeTblSize_48K,
	FoeTblSize_512
};

enum VlanAction {
	NO_ACT = 0,
	MODIFY = 1,
	INSERT = 2,
	DELETE = 3
};

enum FoeEntryState {
	INVALID = 0,
	UNBIND = 1,
	BIND = 2,
	FIN = 3
};

enum FoeTblTcpUdp {
	TCP = 0,
	UDP = 1,
	ANY = 2
};

enum FoeTblEE {
	NOT_ENTRY_END = 0,
	ENTRY_END_FP = 1,
	ENTRY_END_FOE = 2
};

enum FoeLinkType {
	LINK_TO_FOE = 0,
	LINK_TO_FP = 1
};

enum FoeIPAct {
	IPV4_HNAPT = 0,
	IPV4_HNAT = 1,
	L2_BRIDGE = 2,	
	IPV4_DSLITE = 3,
	IPV6_3T_ROUTE = 4,
	IPV6_5T_ROUTE = 5,
	IPV6_MC_ROUTE = 6,	
	IPV6_6RD = 7,
};

enum ppeLengthMode {
    POST_MODE = 0,
    PRE_MODE
};

struct offload_qdma_epon{
	unsigned long int txq:4;
	unsigned long int resv0:6;
	unsigned long int tsid:5;
	unsigned long int tse:1;
	unsigned long int resv1:12;
	unsigned long int llid:4;
};

struct offload_qdma_gpon{
	unsigned long int txq:4;
	unsigned long int resv0:6;
	unsigned long int tsid:5;
	unsigned long int tse:1;
	unsigned long int gemid:12;
	unsigned long int tcon:4;
};

struct offload_info{
	union {
			struct offload_qdma_epon qepon;
			struct offload_qdma_gpon qgpon;
			unsigned long int word;
		};
};

typedef struct {
#ifdef __BIG_ENDIAN
	uint16_t MAGIC_TAG;
	uint32_t RESV:1;
	uint32_t UDF:6;
	uint32_t SPORT:5;
	uint32_t CRSN:5;
	uint32_t FOE_Entry:15;
#else
	uint16_t MAGIC_TAG;
	uint32_t FOE_Entry:15;
	uint32_t CRSN:5;
	uint32_t SPORT:5;
	uint32_t UDF:6;
	uint32_t RESV:1;
#endif
    uint16_t ifc_hit:1;
    uint16_t ifc_idx:1;
    uint16_t RESV1:14;
}  __attribute__ ((packed)) PdmaRxDescInfo4;

/* state = unbind & dynamic */
struct ud_info_blk1 {
#ifdef __BIG_ENDIAN
	uint32_t sta:1;		/* static entry */
	uint32_t udp:1;
	uint32_t state:2;
	uint32_t pkt_type:3;
	uint32_t preb:1;
	uint32_t pcnt:16;	/* packet count */
	uint32_t time_stamp:8;
#else
	uint32_t time_stamp:8;
	uint32_t pcnt:16;	/* packet count */
	uint32_t preb:1;
	uint32_t pkt_type:3;
	uint32_t state:2;
	uint32_t udp:1;
	uint32_t sta:1;		/* static entry */
#endif
};

/* state = bind & fin */
struct bf_info_blk1 {
#ifdef __BIG_ENDIAN
	uint32_t sta:1;		/* static entry */
	uint32_t udp:1;
	uint32_t state:2;
	uint32_t pkt_type:3;
	uint32_t ttl:1;
	uint32_t rmt:1;		/* remove tunnel ip header (6rd/dslite only) */
	uint32_t psn:1;		/* egress packet has PPPoE session */
	uint32_t vpm:2;		/* first TPID set by registerr */
	uint32_t vlan_layer:4;
	uint32_t ka:1;		/* keep alive */
	uint32_t time_stamp:15;
#else
	uint32_t time_stamp:15;
	uint32_t ka:1;		/* keep alive */
	uint32_t vlan_layer:4;
	uint32_t vpm:2;		/* first TPID set by registerr */
	uint32_t psn:1;		/* egress packet has PPPoE session */
	uint32_t rmt:1;		/* remove tunnel ip header (6rd/dslite only) */
	uint32_t ttl:1;
	uint32_t pkt_type:3;
	uint32_t state:2;
	uint32_t udp:1;
	uint32_t sta:1;		/* static entry */
#endif
};

#if 0
struct _info_blk2 {
	uint32_t dscp:8;	/* DSCP value */
	uint32_t port_ag:6;	/* port account group */
	uint32_t port_mg:6;	/* port meter group */
#if 0
	uint32_t alen:1;		/* use pre account ot post account */
	uint32_t mlen:1;		/* use pre account ot post meter */
	uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
	uint32_t mcast:1;		/* multicast to CPU */
	uint32_t fpidx:3;		/* force to  PSE port */
	uint32_t fqos:1;		/* force to  PSE qos port */
	uint32_t qid:4;			/* QID for port */
#else
	uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
	uint32_t mcast:1;		/* multicast to CPU */
	uint32_t fast:1;		/* fast path or slow path */
	uint32_t fqos:1;		/* force to  PSE qos port */
	uint32_t fpidx:3;		/* force to  PSE port */
	uint32_t nbq:5;			/* non-blocking queue */
#endif
};
#endif

typedef struct  {
#ifdef __BIG_ENDIAN
	uint32_t dscp:8;	/* DSCP value */
	uint32_t port_ag:6;	/* port account group */
	uint32_t port_mg:6;	/* port meter group */
#if 0
	uint32_t alen:1;		/* use pre account ot post account */
	uint32_t mlen:1;		/* use pre account ot post meter */
	uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
	uint32_t mcast:1;		/* multicast to CPU */
	uint32_t fpidx:3;		/* force to  PSE port */
	uint32_t fqos:1;		/* force to  PSE qos port */
	uint32_t qid:4;			/* QID for port */
#else
	uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
	uint32_t mcast:1;		/* multicast to CPU */
	uint32_t fast:1;		/* fast path or slow path */
	uint32_t fqos:1;		/* force to  PSE qos port */
	uint32_t fpidx:3;		/* force to  PSE port */
	uint32_t nbq:5;			/* non-blocking queue */
#endif
#else
	uint32_t nbq:5;			/* non-blocking queue */
	uint32_t fpidx:3;		/* force to  PSE port */
	uint32_t fqos:1;		/* force to  PSE qos port */
	uint32_t fast:1;		/* fast path or slow path */
	uint32_t mcast:1;		/* multicast to CPU */
	uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
	uint32_t port_mg:6;	/* port meter group */
	uint32_t port_ag:6;	/* port account group */
	uint32_t dscp:8;	/* DSCP value */
#endif
} _info_blk2;

/*
 * Foe Entry (64B)
 *
 *      IPV4:			     IPV6:
 *	+-----------------------+    +-----------------------+
 *	|  Information Block 1  |    |  Information Block 1  |
 *	+-----------------------+    +-----------------------+
 *	|	SIP(4B)		|    |     IPv6_DIP0(4B)     |
 *	+-----------------------+    +-----------------------+
 *	|	DIP(4B)		|    |     IPv6_DIP1(4B)     |
 *	+-----------------------+    +-----------------------+
 *	| SPORT(2B) | DPORT(2B) |    |	      Rev(4B)        |
 *	+-----------+-----------+    +-----------------------+
 *	| Information Block 2   |    |	Information Block 2  | 
 *	+-----------------------+    +-----------------------+
 *	|      New SIP(4B)	|    |     IPv6_DIP2(4B)     |
 *	+-----------------------+    +-----------------------+
 *	|      New DIP(4B)	|    |     IPv6_DIP3(4B)     |
 *	+-----------------------+    +-----------------------+
 *	| New SPORT | New DPORT |    |	       Rev(4B)       |
 *	+-----------+-----------+    +-----------------------+
 *	| VLAN1(2B) |DMAC[47:32]|    | VLAN1(2B) |DMAC[47:32]|
 *	+-----------|-----------+    +-----------|-----------+
 *	|	DMAC[31:0]      |    |       DMAC[31:0]      |
 *	+-----------------------+    +-----------------------+
 *	| PPPoE_ID  |SMAC[47:32]|    | PPPoE_ID  |SMAC[47:32]|
 *	+-----------+-----------+    +-----------+-----------+
 *	|       SMAC[31:0]      |    |       SMAC[31:0]      | 
 *	+-----------------------+    +-----------------------+
 *	| Rev |  SNAP_Ctrl(3B)  |    | Rev |  SNAP_Ctrl(3B)  |
 *	+-----------------------+    +-----------------------+
 *	|    Rev    | VLAN2(2B) |    |   Rev     | VLAN2(2B) |
 *	+-----------------------+    +-----------------------+
 *	|     Rev(4B)           |    |       Rev(4B)         |
 *	+-----------------------+    +-----------------------+
 *	|     tmp_buf(4B)       |    |       tmp_buf(4B)     |
 *	+-----------------------+    +-----------------------+
 *
 * Foe Entry (80)
 *
 *      IPV4 HNAPT:			     IPV4:
 *	+-----------------------+    +-----------------------+
 *	|  Information Block 1  |    |  Information Block 1  |
 *	+-----------------------+    +-----------------------+
 *	|	SIP(4B)		|    |		SIP(4B)      |
 *	+-----------------------+    +-----------------------+
 *	|	DIP(4B)		|    |		DIP(4B)      |
 *	+-----------------------+    +-----------------------+
 *	| SPORT(2B) | DPORT(2B) |    |	      Rev(4B)        |
 *	+-----------+-----------+    +-----------------------+
 *	| EG DSCP| Info Block 2 |    |	Information Block 2  | 
 *	+-----------------------+    +-----------------------+
 *	|      New SIP(4B)	|    |     New SIP (4B)      |
 *	+-----------------------+    +-----------------------+
 *	|      New DIP(4B)	|    |     New DIP (4B)      |
 *	+-----------------------+    +-----------------------+
 *	| New SPORT | New DPORT |    | New SPORT | New DPORT |
 *	+-----------+-----------+    +-----------------------+
 *	|          REV          |    |		REV	     |
 *	+-----------------------+    +-----------------------+
 *	|Act_dp|   REV          |    |Act_dp|	REV	     |
 *	+-----------------------+    +-----------------------+
 *	|      tmp_buf(4B)      |    |	     temp_buf(4B)    | 
 *	+-----------------------+    +-----------|-----------+
 *	| ETYPE     | VLAN1 ID  |    | ETYPE     |  VLAN1    |
 *	+-----------+-----------+    +-----------+-----------+
 *	|       DMAC[47:16]     |    |       SMAC[47:16]     | 
 *	+-----------------------+    +-----------------------+
 *	| DMAC[15:0]| VLAN2 ID  |    | DMAC[15:0]|  VLAN2    |
 *	+-----------------------+    +-----------------------+
 *	|       SMAC[47:16]     |    |       SMAC[47:16]     | 
 *	+-----------------------+    +-----------------------+
 *	| SMAC[15:0]| PPPOE ID  |    | SMAC[15:0]| PPPOE ID  |
 *	+-----------------------+    +-----------------------+
 *
 */

/* differential parts from foe_fdb_7510.h */
/**** Start **************************/
struct hsk_l2 {
#ifdef __BIG_ENDIAN
    uint32_t etype:16;
    uint32_t dscp:8;
    uint32_t ip:1;
    uint32_t tls:1;
    uint32_t stag_vld:1;
    uint32_t ctag_vld:1;
    uint32_t uni_port:4;
#else
    uint32_t uni_port:4;
    uint32_t ctag_vld:1;
    uint32_t stag_vld:1;
    uint32_t tls:1;
    uint32_t ip:1;
    uint32_t dscp:8;
    uint32_t etype:16;
#endif
};

struct _l2_bridge32 { /* 64 bytes */
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint8_t in_dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t in_dmac_lo[2];
	uint16_t in_vlan;
#else
	uint16_t in_vlan;
	uint8_t in_dmac_lo[2];
#endif

	union {
		struct hsk_l2 hsk_l2;
		uint32_t info_hsk_l2;
	};

	uint8_t in_smac_hi[4];
    
#ifdef __BIG_ENDIAN
	uint8_t in_smac_lo[2];
	uint8_t resv0[2];
#else
	uint8_t resv0[2];
	uint8_t in_smac_lo[2];
#endif

	uint32_t resv1;
	uint32_t resv2;
	uint32_t resv3;
	uint32_t resv4;
	
#ifdef __BIG_ENDIAN
	uint32_t act_dp:8;	/* UDF */
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t channel:5;
	uint32_t qid:3;
	uint32_t :8;
#else
	uint32_t :8;
	uint32_t qid:3;
	uint32_t channel:5;
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t act_dp:8;	/* UDF */
#endif

	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};
	
#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint32_t resv5;
	
#ifdef __BIG_ENDIAN
	uint16_t resv6;
	uint16_t vlan2;	
#else
	uint16_t vlan2;	
	uint16_t resv6;
#endif

	uint32_t resv7;
};
/**** End **************************/


struct _l2_bridge { /* 64 bytes */
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint8_t in_dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t in_dmac_lo[2];
	uint8_t in_smac_hi[2];
#else
	uint8_t in_smac_hi[2];
	uint8_t in_dmac_lo[2];
#endif
	uint8_t in_smac_lo[4];

	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};

	uint32_t resv0;
	uint32_t resv1;
	uint32_t resv2;
	uint32_t resv3;
	uint32_t resv4;
	
#ifdef __BIG_ENDIAN
	uint32_t act_dp:8;	/* UDF */
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t chn:8;
	uint32_t resv5:8;
#else
	uint32_t resv5:8;
	uint32_t chn:8;
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t act_dp:8;	/* UDF */
#endif
	
#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint8_t dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t dmac_lo[2];
	uint16_t vlan2;
#else
	uint16_t vlan2;
	uint8_t dmac_lo[2];
#endif

	uint8_t smac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t smac_lo[2];
	uint16_t pppoe_id;
#else
	uint16_t pppoe_id;
	uint8_t smac_lo[2];
#endif
};


struct _ipv4_hnapt {
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint32_t sip;
	uint32_t dip;
	
#ifdef __BIG_ENDIAN
	uint16_t sport;
	uint16_t dport;
#else
	uint16_t dport;
	uint16_t sport;
#endif
	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};
	uint32_t new_sip;
	uint32_t new_dip;
	
#ifdef __BIG_ENDIAN
	uint16_t new_sport;
	uint16_t new_dport;
#else
	uint16_t new_dport;
	uint16_t new_sport;
#endif
	
 	uint32_t resv1;
 
	uint32_t resv2; 

#ifdef __BIG_ENDIAN
	uint32_t act_dp:8;	/* UDF */
	uint32_t ts_id:8;	/* traffic shaper id*/
    uint32_t channel:5;
    uint32_t qid:3;
    uint32_t resv3:8;
#else
    uint32_t resv3:8;
    uint32_t qid:3;
    uint32_t channel:5;
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t act_dp:8;	/* UDF */
#endif
	
#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint8_t dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t dmac_lo[2];
	uint16_t vlan2;
#else
	uint16_t vlan2;
	uint8_t dmac_lo[2];
#endif

	uint8_t smac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t smac_lo[2];
	uint16_t pppoe_id;
#else
	uint16_t pppoe_id;
	uint8_t smac_lo[2];
#endif
};


/********************************************************************************************
	struct ipv4_hnapt_dual_info1 and struct ipv4_hnapt_dual_info2 
	is used for only compile successfully
********************************************************************************************/
struct ipv4_hnapt_dual_info1 {
	union {
		//_info_blk2 ;
		struct {
			#ifdef __BIG_ENDIAN
			uint32_t new_dscp:8;		/* DSCP value */
			uint32_t new_port_ag:11;	/* port account group */
			uint32_t new_pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t new_mcast:1;		/* multicast to CPU */
			uint32_t new_fast:1;		/* fast path or slow path */
			uint32_t new_fqos:1;		/* force to  PSE qos port */
			uint32_t new_fpidx:4;		/* force to  PSE port */
			uint32_t new_nbq:5;			/* non-blocking queue */
			#else
			uint32_t new_nbq:5;			/* non-blocking queue */
			uint32_t new_fpidx:4;		/* force to  PSE port */
			uint32_t new_fqos:1;		/* force to  PSE qos port */
			uint32_t new_fast:1;		/* fast path or slow path */
			uint32_t new_mcast:1;		/* multicast to CPU */
			uint32_t new_pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t new_port_ag:11;	/* port account group */
			uint32_t new_dscp:8;		/* DSCP value */
			#endif
		} ;
		uint32_t new_info_blk2;
	};

	/* information of other direction */
	#ifdef __BIG_ENDIAN
	uint32_t new_udf:8;
	uint32_t new_ts_id:8;
    uint32_t new_channel:5;
    uint32_t new_qid:3;
    uint32_t new_resv:8;
	#else
    uint32_t new_resv:8;
    uint32_t new_qid:3;
    uint32_t new_channel:5;
	uint32_t new_ts_id:8;
	uint32_t new_udf:8;
	#endif
};

struct ipv4_hnapt_dual_info2 {
	/* information of other direction */
	uint8_t new_dmac_hi[4];
	#ifdef __BIG_ENDIAN
	uint8_t new_dmac_lo[2];
	uint16_t new_sp_tag;
	#else
	uint16_t new_sp_tag;
	uint8_t new_dmac_lo[2];
	#endif

	uint8_t new_smac_hi[4];
	#ifdef __BIG_ENDIAN
	uint8_t new_smac_lo[2];
	uint16_t new_pppoe_id;
	#else
	uint16_t new_pppoe_id;
	uint8_t new_smac_lo[2];
	#endif
};


struct _ipv4_dslite {
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint32_t sip;
	uint32_t dip;
	
#ifdef __BIG_ENDIAN
	uint16_t sport;
	uint16_t dport;
#else
	uint16_t dport;
	uint16_t sport;
#endif
	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};
	
#ifdef __BIG_ENDIAN
	uint16_t resv:4;
	uint16_t priority:4;	/* in order to consist with Linux kernel (should be 8bits) */
	uint8_t flow_lbl[3];	/* in order to consist with Linux kernel (should be 20bits) */
#else
	uint8_t flow_lbl[3];	/* in order to consist with Linux kernel (should be 20bits) */
	uint16_t priority:4;	/* in order to consist with Linux kernel (should be 8bits) */
	uint16_t resv:4;
#endif
	
	uint32_t resv1;
	uint32_t resv2;
	uint32_t resv3;
	uint32_t resv4;
	
#ifdef __BIG_ENDIAN
	uint32_t act_dp:8;	/* UDF */
	uint32_t ts_id:8;	/* traffic shaper id*/
    //uint32_t channel:8; /* shared with qos_que_index:4 for TCSUPPORT_CT_HWNAT_OFFLOAD */
    uint32_t channel:5;
    uint32_t qid:3;
	uint32_t :4;
	uint32_t ip6_idx:4;
#else
	uint32_t ip6_idx:4;
	uint32_t :4;
    uint32_t qid:3;
    //uint32_t channel:8; /* shared with qos_que_index:4 for TCSUPPORT_CT_HWNAT_OFFLOAD */
    uint32_t channel:5;
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t act_dp:8;	/* UDF */
#endif
	
#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint8_t dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t dmac_lo[2];
	uint16_t vlan2;
#else
	uint16_t vlan2;
	uint8_t dmac_lo[2];
#endif

	uint8_t smac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t smac_lo[2];
	uint16_t pppoe_id;
#else
	uint16_t pppoe_id;
	uint8_t smac_lo[2];
#endif
};


struct _ipv6_1t_route {
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint32_t ipv6_dip0;
	uint32_t ipv6_dip1;
	uint32_t resv;

	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};

	uint32_t ipv6_dip2;
	uint32_t ipv6_dip3;
	uint32_t resv1;

	uint32_t act_dp:6;	/* UDF */
	
#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint8_t dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t dmac_lo[2];
	uint16_t vlan2;
#else
	uint16_t vlan2;
	uint8_t dmac_lo[2];
#endif

	uint8_t smac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t smac_lo[2];
	uint16_t pppoe_id;
#else
	uint16_t pppoe_id;
	uint8_t smac_lo[2];
#endif
};

struct _ipv6_3t_route {
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint32_t ipv6_sip0;
	uint32_t ipv6_sip1;
	uint32_t ipv6_sip2;
	uint32_t ipv6_sip3;
	uint32_t ipv6_dip0;
	uint32_t ipv6_dip1;
	uint32_t ipv6_dip2;
	uint32_t ipv6_dip3;
#ifdef __BIG_ENDIAN
	uint32_t resv:24;
	uint32_t prot:8;
#else
	uint32_t prot:8;
	uint32_t resv:24;
#endif
	
#ifdef __BIG_ENDIAN
	uint32_t act_dp:8;	/* UDF */
	uint32_t ts_id:8;	/* traffic shaper id*/
    //uint32_t channel:8; /* shared with qos_que_index:4 for TCSUPPORT_CT_HWNAT_OFFLOAD */
    uint32_t channel:5;
    uint32_t qid:3;
    uint32_t resv4:8;
#else
    uint32_t resv4:8;
    uint32_t qid:3;
    uint32_t channel:5;
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t act_dp:8;	/* UDF */
#endif

	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};
	
#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint8_t dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t dmac_lo[2];
	uint16_t vlan2;
#else
	uint16_t vlan2;
	uint8_t dmac_lo[2];
#endif
	
#ifdef __BIG_ENDIAN
	uint32_t :11;
	uint32_t smac_idx:5;
	uint16_t pppoe_id;
#else
	uint16_t pppoe_id;
	uint32_t smac_idx:5;
	uint32_t :11;
#endif
};

struct _ipv6_5t_route {
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint32_t ipv6_sip0;
	uint32_t ipv6_sip1;
	uint32_t ipv6_sip2;
	uint32_t ipv6_sip3;
	uint32_t ipv6_dip0;
	uint32_t ipv6_dip1;
	uint32_t ipv6_dip2;
	uint32_t ipv6_dip3;
	
#ifdef __BIG_ENDIAN
	uint16_t sport;
	uint16_t dport;
#else
	uint16_t dport;
	uint16_t sport;
#endif


#if 0
	uint32_t resv1:30;
	/* pkt_dir: ask kevin, used for packet direction */
	uint32_t pkt_dir:2;
	uint32_t resv2;
	uint32_t resv3:27;
	/* classify_index: sw classify, now we have IFC, could we discard this function? */
    uint32_t classify_index:5;/*TCSUPPORT_XPON_HAL_API_EXT*/
#endif

#ifdef __BIG_ENDIAN
	uint32_t act_dp:8;	/* UDF */
	uint32_t ts_id:8;	/* traffic shaper id*/
    //uint32_t channel:8; /* shared with qos_que_index:4 for TCSUPPORT_CT_HWNAT_OFFLOAD */
    uint32_t channel:5;
    uint32_t qid:3;
    uint32_t resv1:8;
#else
    uint32_t resv1:8;
    uint32_t qid:3;
    //uint32_t channel:8; /* shared with qos_que_index:4 for TCSUPPORT_CT_HWNAT_OFFLOAD */
    uint32_t channel:5;
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t act_dp:8;	/* UDF */
#endif

	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};

#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint8_t dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t dmac_lo[2];
	uint16_t vlan2;
#else
	uint16_t vlan2;
	uint8_t dmac_lo[2];
#endif
	
#ifdef __BIG_ENDIAN
	uint32_t :11;
	uint32_t smac_idx:5;
	uint16_t pppoe_id;
#else
	uint16_t pppoe_id;
	uint32_t smac_idx:5;
 	uint32_t :11;
#endif
};

struct _ipv6_6rd {
	union {
		struct ud_info_blk1 udib1;
		struct bf_info_blk1 bfib1;
		uint32_t info_blk1;
	};
	uint32_t ipv6_sip0;
	uint32_t ipv6_sip1;
	uint32_t ipv6_sip2;
	uint32_t ipv6_sip3;
	uint32_t ipv6_dip0;
	uint32_t ipv6_dip1;
	uint32_t ipv6_dip2;
	uint32_t ipv6_dip3;

#ifdef __BIG_ENDIAN
	uint16_t sport;
	uint16_t dport;
#else
	uint16_t dport;
	uint16_t sport;
#endif
	
#ifdef __BIG_ENDIAN
	uint32_t act_dp:8;	/* UDF */
	uint32_t ts_id:8;	/* traffic shaper id*/
    //uint32_t channel:8; /* shared with qos_que_index:4 for TCSUPPORT_CT_HWNAT_OFFLOAD */
    uint32_t channel:5;
    uint32_t qid:3;
    uint32_t resv1:4;
	uint32_t ip4_idx:4;
#else
	uint32_t ip4_idx:4;
    uint32_t resv1:4;
    uint32_t qid:3;
    //uint32_t channel:8; /* shared with qos_que_index:4 for TCSUPPORT_CT_HWNAT_OFFLOAD */
    uint32_t channel:5;
	uint32_t ts_id:8;	/* traffic shaper id*/
	uint32_t act_dp:8;	/* UDF */
#endif

	union {
		//_info_blk2 ;
		struct {
		#ifdef __BIG_ENDIAN
			uint32_t dscp:8;	/* DSCP value */
			uint32_t port_ag:6;	/* port account group */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t nbq:5;			/* non-blocking queue */
		#else
			uint32_t nbq:5;			/* non-blocking queue */
			uint32_t fpidx:3;		/* force to  PSE port */
			uint32_t fqos:1;		/* force to  PSE qos port */
			uint32_t fast:1;		/* fast path or slow path */
			uint32_t mcast:1;		/* multicast to CPU */
			uint32_t pcp:1;			/* keep first ingress 1st and 2nd p-bit and DEI */
			uint32_t port_mg:6;	/* port meter group */
			uint32_t port_ag:6;	/* port account group */
			uint32_t dscp:8;	/* DSCP value */
		#endif
		} ;
		uint32_t info_blk2;
	};
	
#ifdef __BIG_ENDIAN
	uint16_t etype;
	uint16_t vlan1;
#else
	uint16_t vlan1;
	uint16_t etype;
#endif

	uint8_t dmac_hi[4];
#ifdef __BIG_ENDIAN
	uint8_t dmac_lo[2];
	uint16_t vlan2;
#else
	uint16_t vlan2;
	uint8_t dmac_lo[2];
#endif

#ifdef __BIG_ENDIAN
	uint32_t tunnel_dscp:8;
	uint32_t flag:3;
	uint32_t smac_idx:5;
	uint16_t pppoe_id;
#else
	uint16_t pppoe_id;
	uint32_t smac_idx:5;
	uint32_t flag:3;
	uint32_t tunnel_dscp:8;
#endif
};

/* differential parts from foe_fdb_7510.h */
/**** Start **************************/
struct FoeEntry {
	union {
		struct bf_info_blk1 bfib1;
		struct ud_info_blk1 udib1;
        struct _l2_bridge32 l2_bridge32; /* New added for EN7512 */	
		struct _l2_bridge l2_bridge;
		struct _ipv4_hnapt ipv4_hnapt;
		struct _ipv4_dslite ipv4_dslite;
		struct _ipv6_1t_route ipv6_1t_route;
		struct _ipv6_3t_route ipv6_3t_route;
		struct _ipv6_5t_route ipv6_5t_route;
		struct _ipv6_6rd ipv6_6rd;
	};
};
/**** End **************************/


/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/
#define GetInfo2Cache(bfib1)  (0)
#define SetInfo2Cache(bfib1, value)
#define GetInfo2Qid(info2)  (0)
#define GetInfo2Alen(info2)  (0)
#define GetInfo2Mlen(info2)  (0)
#define GetInfo2Mgrp(iblk2)	 (iblk2->port_mg)
#define SetInfo2Mgrp(iblk2, value)	(iblk2->port_mg = value)


static inline void set_ppe_entry_dpi (struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{
	return;
}

static inline void set_info2_mg (struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned char value) 
{
	if(type == IPV4_HNAPT)
        foe_entry->ipv4_hnapt.port_mg = value;
    else if(type == IPV6_5T_ROUTE)
        foe_entry->ipv6_5t_route.port_mg = value;
    else if(type == L2_BRIDGE)
        foe_entry->l2_bridge32.port_mg = value;
    else if(type == IPV4_DSLITE)
        foe_entry->ipv4_dslite.port_mg = value;
	
    return;
}

static inline void set_mac(struct FoeEntry *foe_entry, unsigned char *Src)
{
	return;
}

static inline void set_dualEntry_info2(struct ipv4_hnapt_dual_info2 *dual_info2, struct FoeEntry *foe_entry)
{
	return;
}

static inline void set_channel_info (struct FoeEntry * foe_entry, enum FoeIPAct type, unsigned char value) {
	/*
	1. channel 1~4 -- 7530 internal switch port 1~4
	*/
    if(type == IPV4_HNAPT)
        foe_entry->ipv4_hnapt.channel = value;
    else if(type == IPV6_5T_ROUTE)
        foe_entry->ipv6_5t_route.channel = value;
    else if(type == L2_BRIDGE)
        foe_entry->l2_bridge32.channel = value;
    else if(type == IPV4_DSLITE)
        foe_entry->ipv4_dslite.channel = value;
    return;
}

static inline int get_info2_nbq (struct FoeEntry * foe_entry, enum FoeIPAct type) {
    if(type == IPV4_HNAPT)
        return foe_entry->ipv4_hnapt.nbq;
    else if(type == IPV6_5T_ROUTE)
        return foe_entry->ipv6_5t_route.nbq;
    else if(type == L2_BRIDGE)
        return foe_entry->l2_bridge32.nbq;
    else
        return 0;
}

static inline void set_info2_nbq (struct FoeEntry * foe_entry, enum FoeIPAct type, unsigned char value) {
    if(type == IPV4_HNAPT)
        foe_entry->ipv4_hnapt.nbq = value;
    else if(type == IPV6_5T_ROUTE)
        foe_entry->ipv6_5t_route.nbq = value;
    else if(type == L2_BRIDGE)
        foe_entry->l2_bridge32.nbq = value;
    else if(type == IPV4_DSLITE)
        foe_entry->ipv4_dslite.nbq = value;
    return;
}

static inline void set_info2_fast (struct FoeEntry * foe_entry, enum FoeIPAct type, unsigned char value) {
    if(type == IPV4_HNAPT)
        foe_entry->ipv4_hnapt.fast = value;
    else if(type == IPV6_5T_ROUTE)
        foe_entry->ipv6_5t_route.fast = value;
    else if(type == L2_BRIDGE)
        foe_entry->l2_bridge32.fast = value;
    return;
}

static inline int get_ppe_entry_smac_index (struct FoeEntry * foe_entry, enum FoeIPAct type) {
    if(type == IPV6_3T_ROUTE)
        return foe_entry->ipv6_3t_route.smac_idx;
    else if(type == IPV6_5T_ROUTE)
        return foe_entry->ipv6_5t_route.smac_idx;
    else if(type == IPV6_6RD)
        return foe_entry->ipv6_6rd.smac_idx;
    return 0;
}

static inline void set_ppe_entry_smac_index (struct FoeEntry * foe_entry, enum FoeIPAct type, unsigned char value) {
    if(type == IPV6_3T_ROUTE)
        foe_entry->ipv6_3t_route.smac_idx = value;
    else if(type == IPV6_5T_ROUTE)
        foe_entry->ipv6_5t_route.smac_idx = value;
    else if(type == IPV6_6RD)
        foe_entry->ipv6_6rd.smac_idx = value;
    return;
}

static inline int get_ppe_entry_tunnel_ip_index (struct FoeEntry * foe_entry, enum FoeIPAct type) {
    if(type == IPV6_6RD)
        return foe_entry->ipv6_6rd.ip4_idx;
    else if(type == IPV4_DSLITE)
        return foe_entry->ipv4_dslite.ip6_idx;
    return 0;
}

static inline void set_ppe_entry_tunnel_ip_index (struct FoeEntry * foe_entry, enum FoeIPAct type, unsigned char value) {
    if(type == IPV6_6RD)
        foe_entry->ipv6_6rd.ip4_idx = value;
    else if(type == IPV4_DSLITE)
        foe_entry->ipv4_dslite.ip6_idx= value;
    return;
}

static inline uint8_t *get_ppe_entry_dmac_addr(struct FoeEntry * foe_entry, enum FoeIPAct type) {
    return NULL;
}

static inline void get_ppe_entry_tunnel_ip (struct FoeEntry * foe_entry, enum FoeIPAct type,  struct hwnat_tuple * opt) {
    return;
}

static inline void set_ppe_entry_tunnel_ip (struct FoeEntry * foe_entry, enum FoeIPAct type, void * valPtr, int vlaue_flag) {
    return;
}

static inline void set_ppe_entry_ttl(struct FoeEntry * foe_entry, enum FoeIPAct type, uint value) {
    return;
}

static inline void set_ppe_entry_chksum(struct FoeEntry * foe_entry, enum FoeIPAct type, uint value) {
    return;
}

static inline void set_ppe_acnt_meter_len_mode(_info_blk2 *iblk2, enum ppeLengthMode acntMode, enum ppeLengthMode meterMode) {
    return;
}

static inline void set_ppe_nptv6_info_mode0(struct FoeEntry * foe_entry,unsigned char snpt,unsigned char dnpt,unsigned int foe_num) 
{
	return;
}

static inline void set_ppe_nptv6_info_mode1(struct FoeEntry * foe_entry,unsigned char snpt,unsigned char dnpt,unsigned char prefix_id,unsigned prefix_len) 
{
	return;
}

static inline void set_ppe_entry_acnt_grp3(struct FoeEntry *foe_entry,enum FoeIPAct type,unsigned int value)
{
	return;
}

static inline void set_ppe_entry_mtr_grp2(struct FoeEntry *foe_entry,enum FoeIPAct type,unsigned int value)
{
	return;
}

static inline void set_ppe_entry_mtr_grp3(struct FoeEntry *foe_entry,enum FoeIPAct type,unsigned int value)
{
	return;
}

static inline void set_ppe_entry_snpt(struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{  
    return;
}

static inline void set_ppe_entry_dnpt(struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{
    return;
}

static inline void set_ppe_entry_nptv6_mode0_foe_num(struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{
    return;
}

static inline void set_ppe_entry_nptv6_mode1_prefix_id(struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{
    return;
}

static inline void set_ppe_entry_nptv6_mode1_prefix_len(struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{
    return;
}

static inline void set_ppe_entry_tunnel_info (struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned char tunnel, unsigned char tunnel_id) 
{
	return;
}


static inline void set_ppe_entry_hop_info (struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned char hop0,  unsigned char hop1, unsigned char hop2,  unsigned char hop3) 
{
	return;
}

static inline void set_ppe_entry_tunnel_mtu (struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned short tunnel_mtu) 
{
	return;
}

static inline void set_l2b_dscp_remark_enable(struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{
    return;
}

static inline void set_l2tp_sessionid(struct FoeEntry *foe_entry, enum FoeIPAct type, unsigned int value) 
{	
	return;
}

int is_request_done(void);
int FoeAddEntry(struct hwnat_tuple *opt);
int32_t FoeHashFun(struct FoePriKey *key, enum FoeEntryState TargetState, enum HashSelect hashSel);
uint32_t FoeInsEntry(struct FoeEntry *entry);
void FoeSetMacInfo(uint8_t * dst, uint8_t * src);
void FoeGetMacInfo(uint8_t * dst, uint8_t * src);
void FoeSetEntryMac(uint8_t * Dst, uint8_t * Src_hi, uint8_t * Src_lo);
void FoeGetEntryMac(uint8_t * Dst, uint8_t * Src_hi, uint8_t * Src_lo);
void FoeDumpEntry(uint32_t Index);
int FoeGetAllEntries(struct hwnat_args *opt);
int FoeBindEntry(struct hwnat_args *opt);
int FoeUnBindEntry(struct hwnat_args *opt);
int FoeDelEntryByNum(uint32_t entry_num);
void FoeTblClean(void);
void ppeCacheClearOne(unsigned int entry_num);
void ppeCacheClearAll(void);
int FoeClearCache(unsigned int entry_num);
int FoeDumpCacheData(unsigned char cache_type);
int FoeDumpCacheEntryNew(unsigned int entry_num);
int FoeDumpCacheEntry(void);
extern unsigned int gPpeType;
extern unsigned int gPpeFoeNum;
extern unsigned int gPpeSramFoeNum;
extern unsigned int gPpeFoeSize;
#endif
