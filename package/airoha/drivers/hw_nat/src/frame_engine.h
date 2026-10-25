/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _FE_WANTED
#define _FE_WANTED

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <linux/version.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/gre.h>
#include <net/vxlan.h>



/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define IN
#define OUT
#define INOUT
    
#define FOE_DRAM_HASH_WAY	(2)
#define FOE_SRAM_HASH_WAY	(4)

#define SMACT_SIZE	(0x800)
#define UPDMEM_NUM	(16)
//#define UPDMEM_SMAC_LINE	(2)
#define UPDMEM_SMAC_CNT		(6)
#define UPDMEM_IPV4_LINE	(2)
#define UPDMEM_IPV6_LINE	(8)

#define PPE_CACHE_CMD_SEARCH        0x100
#define PPE_CACHE_CMD_SEARCH_LOCK   0x1100
#define PPE_CACHE_CMD_READ          0x2100
#define PPE_CACHE_CMD_WRITE         0x3100
#define PPE_CACHE_CMD_CLEAR_ALL     0x4100

#define CHIPID		    RALINK_SYSCTL_BASE + 0x00
#define REVID		    RALINK_SYSCTL_BASE + 0x0C

#define RALINK_SYSCTL_BASE			0xBFB00000
#define RALINK_ETH_SW_BASE 			0xBFB58000
#define RALINK_FRAME_ENGINE_BASE 	0xBFB50000
#define RALINK_PPE_BASE				0xBFB50c00

#define PFC		    RALINK_ETH_SW_BASE + 0x0004
#define TPF0		    RALINK_ETH_SW_BASE + 0x2030
#define TPF1		    RALINK_ETH_SW_BASE + 0x2130
#define TPF2		    RALINK_ETH_SW_BASE + 0x2230
#define TPF3		    RALINK_ETH_SW_BASE + 0x2330
#define TPF4		    RALINK_ETH_SW_BASE + 0x2430
#define TPF5		    RALINK_ETH_SW_BASE + 0x2530
#define TPF6		    RALINK_ETH_SW_BASE + 0x2630

#define PMCR_P7		    RALINK_ETH_SW_BASE + 0x3700
#define PSC_P7		    RALINK_ETH_SW_BASE + 0x270c
#define FOE_TS		    RALINK_FRAME_ENGINE_BASE + 0x0010

#define PPE_FQFC_CFG	    RALINK_PPE_BASE + 0x00
#define PPE_IQ_CFG	    RALINK_PPE_BASE + 0x04
#define PPE_QUE_STA	    RALINK_PPE_BASE + 0x08
#define GDM1_LEN_CFG			RALINK_FRAME_ENGINE_BASE + 0x514
#define GDM1_FWD_CFG_OFFSET	    0x500
#define GDM2_FWD_CFG_OFFSET	    0x1500
#define L2_BR_CONFIG_OFFSET		0x280
#define L2_BR_ETYPE_EN_OFFSET	0x284
#define L2_ETHTYPE_0_OFFSET		0x290
#define L2_ETHTYPE_1_OFFSET		0x294
#define L2_ETHTYPE_2_OFFSET		0x298
#define L2_ETHTYPE_3_OFFSET		0x29c

#define PPE_EN  (1<<0)
#define PPE_GLO_CFG	    	RALINK_PPE_BASE + 0x200
#define PPE_FLOW_CFG	    RALINK_PPE_BASE + 0x204
#define IPv6_HASH_GREK      20
#define IPv4_HASH_GREK      19


#define PPE_FLOW_SET	    PPE_FLOW_CFG
#define PPE_IP_PROT_CHK	    RALINK_PPE_BASE + 0x208
#define PPE_IP_PROT_0	    RALINK_PPE_BASE + 0x20C
#define PPE_IP_PROT_1	    RALINK_PPE_BASE + 0x210
#define PPE_IP_PROT_2	    RALINK_PPE_BASE + 0x214
#define PPE_IP_PROT_3	    RALINK_PPE_BASE + 0x218
#define PPE_TB_CFG	    	RALINK_PPE_BASE + 0x21C
#define PPE_FOE_CFG	    	PPE_TB_CFG
#define PPE_TB_BASE	    	RALINK_PPE_BASE + 0x220
#define PPE_FOE_BASE	    PPE_TB_BASE
#define PPE_TB_USED	    	RALINK_PPE_BASE + 0x224
#define PPE_BNDR	    	RALINK_PPE_BASE + 0x228
#define PPE_FOE_BNDR	    PPE_BNDR
#define PPE_BIND_LMT_0	    RALINK_PPE_BASE + 0x22C
#define PPE_FOE_LMT1	    PPE_BIND_LMT_0
#define PPE_BIND_LMT_1	    RALINK_PPE_BASE + 0x230
#define PPE_FOE_LMT2	    PPE_BIND_LMT_1
#define PPE_KA		    	RALINK_PPE_BASE + 0x234
#define PPE_FOE_KA	    	PPE_KA
#define PPE_UNB_AGE	    	RALINK_PPE_BASE + 0x238
#define PPE_FOE_UNB_AGE	    PPE_UNB_AGE
#define PPE_BND_AGE_0	    RALINK_PPE_BASE + 0x23C
#define PPE_FOE_BND_AGE0    PPE_BND_AGE_0
#define PPE_BND_AGE_1	    RALINK_PPE_BASE + 0x240
#define PPE_FOE_BND_AGE1    PPE_BND_AGE_1
#define PPE_HASH_SEED	    RALINK_PPE_BASE + 0x244
#define PPE_DFP_CPORT	    RALINK_PPE_BASE + 0x248
#define PPE_DFP_CPORT1	    RALINK_PPE_BASE + 0x24c
#define PPE_TB_HASH_CFG	    RALINK_PPE_BASE + 0X250
#define PPE_FCNT_CTRL	    RALINK_PPE_BASE + 0X250
#define PPE_FCNT_PCNT	    RALINK_PPE_BASE + 0X254

#define MIRROR_EN		    (1 << 31)
#define MIRROR_FP_SHIFT		(16)
#define MIRROR_FP_MASK		(0x1FF<<MIRROR_FP_SHIFT)
#define MIRROR_SPTAG_SHIFT	(0)
#define MIRROR_SPTAG_MASK	(0xFFFF<<MIRROR_SPTAG_SHIFT)
#define PPE_MIRROR			RALINK_PPE_BASE + 0X254

#define MIRROR_DEC_MODE         (1<<24)
#define MIRROR_DST_PRE_EN       (1<<23)
#define MIRROR_DST_POST_EN      (1<<22)
#define MIRROR_DST_Q_EN         (1<<21)
#define MIRROR_DST_SHIFT        8
#define MIRROR_DST_MASK         (0x1FF<<MIRROR_DST_SHIFT)
#define MIRROR_SRC_PRE_EN       (1<<7)
#define MIRROR_SRC_POST_EN      (1<<6)
#define MIRROR_SRC_SHIFT        0
#define MIRROR_SRC_MASK         (0x1F<<MIRROR_SRC_SHIFT)
#define PPE_MIRROR_DEC		RALINK_PPE_BASE + 0X258

#define PPE_FCNT_BCNT_L	    RALINK_PPE_BASE + 0X258
#define PPE_FCNT_BCNT_H	    RALINK_PPE_BASE + 0X25C
#define PPE_MCAST_PPSE	    RALINK_PPE_BASE + 0x284

#define PPE_L2B_CFG         RALINK_PPE_BASE + 0x288
#define PPE_TLS_CFG0        RALINK_PPE_BASE + 0x290
#define PPE_TLS_CFG1        RALINK_PPE_BASE + 0x294
#define PPE_TLS_CFG2        RALINK_PPE_BASE + 0x298
#define PPE_TLS_CFG3        RALINK_PPE_BASE + 0x29c
#define PPE_TLS_CFG4        RALINK_PPE_BASE + 0x2a0
#define PPE_TLS_CFG5        RALINK_PPE_BASE + 0x2a4
#define PPE_TLS_VID0        RALINK_PPE_BASE + 0x2b0
#define PPE_TLS_VID1        RALINK_PPE_BASE + 0x2b4
#define PPE_TLS_VID2        RALINK_PPE_BASE + 0x2b8
#define PPE_TLS_VID3        RALINK_PPE_BASE + 0x2bc
#define PPE_TLS_VID4        RALINK_PPE_BASE + 0x2c0
#define PPE_TLS_VID5        RALINK_PPE_BASE + 0x2c4
#define PPE_TLS_VID6        RALINK_PPE_BASE + 0x2c8
#define PPE_TLS_VID7        RALINK_PPE_BASE + 0x2cc

#define PPE_MTR_BBS0        RALINK_PPE_BASE + 0x2F0
#define PPE_MTR_BBS1        RALINK_PPE_BASE + 0x2F4
#define PPE_MTR_PBS0        RALINK_PPE_BASE + 0x2F8
#define PPE_MTR_PBS1        RALINK_PPE_BASE + 0x2FC
#define PPE_VLAN_TPID	    RALINK_PPE_BASE + 0x318

#define PPE_MTU_DROP	    RALINK_PPE_BASE + 0x300
#define PPE_MTU_VLYR0	    RALINK_PPE_BASE + 0x304
#define PPE_MTU_VLYR1	    RALINK_PPE_BASE + 0x308
#define PPE_MTU_VLYR2	    RALINK_PPE_BASE + 0x30c

#define PPE_MTU_DRP	    	RALINK_PPE_BASE + 0x308
#define PPE_MTU_VLYR_0	    RALINK_PPE_BASE + 0x30C
#define PPE_MTU_VLYR_1	    RALINK_PPE_BASE + 0x310
#define PPE_MTU_VLYR_2	    RALINK_PPE_BASE + 0x314

#define PPE_MTU_VLYR_3	    RALINK_PPE_BASE + 0x310


#define PPE_TPID	    RALINK_PPE_BASE + 0x318
#define PPE_CRSN_MSK	    RALINK_PPE_BASE + 0x3F0

#define WAN_IP6_NUM 8
#define WAN_IP6_CFG_IP_SHIFT    (8)
#define WAN_IP6_CFG_IP_MASK		(0x7<<WAN_IP6_CFG_IP_SHIFT)
#define WAN_IP6_CFG_IP_OFST_SHIFT   (4)
#define WAN_IP6_CFG_IP_OFST_MASK    (0x7<<WAN_IP6_CFG_IP_OFST_SHIFT)
#define WAN_IP6_CFG    RALINK_FRAME_ENGINE_BASE+0x2040 
#define WAN_IP6_WDATA    RALINK_FRAME_ENGINE_BASE+0x2044
#define WAN_IP6_RDATA    RALINK_FRAME_ENGINE_BASE+0x2048
/****************  EN7580 Start  ********************/
#define PPE_L2B_ETYPE_EN        (RALINK_PPE_BASE + 0x28C)
#define PPE_L2B_ETYPE_N(idx)    (RALINK_PPE_BASE + 0x2D0 + ((idx/2)<<2))

#define SRAM_CTRL           (RALINK_PPE_BASE + 0x31C)
#define SRAM_ENTRY_BASE     (RALINK_PPE_BASE + 0x320)

#define PPE_UPDMEM_CTRL     (RALINK_PPE_BASE + 0x370)
#define PPE_UPDMEM_DATA     (RALINK_PPE_BASE + 0x374)
/****************  EN7580 End  ********************/

/*********  7520/7512/7522/7516 Start  *************/
#define CAH_CTRL	    (RALINK_PPE_BASE + 0x320)
#define CAH_TAG_SRH	    (RALINK_PPE_BASE + 0x324)
#define CAH_LINE_RW	    (RALINK_PPE_BASE + 0x328)
#define CAH_WDATA	    (RALINK_PPE_BASE + 0x32C)
/* 
 * CAH_RDATA[17:16]
 *  0: invalid
 *  1: valid
 *  2: dirty
 *  3: lock
 *
 * CAH_RDATA[15:0]: entry num 
 */
#define CAH_RDATA	    (RALINK_PPE_BASE + 0x330)
#define CAH_CTRL_2	    (RALINK_PPE_BASE + 0x334)
/*********  7520/7512/7522/7516 End  *************/

/********** 7528 Start ****************************/
#define PPE_ACNT_BASE			(RALINK_FRAME_ENGINE_BASE + 0x2400)
#define PPE_ACNT_BCNT_L(x,y)		(PPE_ACNT_BASE + ((x) * 0x400) + ((y) << 4))
#define PPE_ACNT_BCNT_H(x,y)		(PPE_ACNT_BASE + ((x) * 0x400) + ((y) << 4) + 0x04)
#define PPE_ACNT_PCNT_L(x,y)		(PPE_ACNT_BASE + ((x) * 0x400) + ((y) << 4) + 0x08)
#define PPE_ACNT_PCNT_H(x,y)		(PPE_ACNT_BASE + ((x) * 0x400) + ((y) << 4) + 0x0c)
/********** 7528 End ****************************/

/****************  EN7580 Start  ********************/
#define FE_IFC_BASE				(RALINK_FRAME_ENGINE_BASE + 0x200)
#define FE_IFC_CFG				(FE_IFC_BASE+0x0000)
#define FE_IFC_CTRL				(FE_IFC_BASE+0x0004)
#define FE_IFC_DATA0			(FE_IFC_BASE+0x0010)
#define FE_IFC_DATA1			(FE_IFC_BASE+0x0014)
#define FE_IFC_DATA2			(FE_IFC_BASE+0x0018)
#define FE_IFC_DATA3			(FE_IFC_BASE+0x001C)
#define FE_IFC_UDF(i)			(FE_IFC_BASE+0x0020+(i<<2))
/****************  EN7580 End  ********************/

/* PPE_L2B_CFG */
#define L2B_DMAC_EN         (1 << 0)
#define L2B_SMAC_EN         (1 << 1)
#define L2B_VID_EN          (1 << 2)
#define L2B_PBITS_EN        (1 << 3)
#define L2B_SPORT_EN        (1 << 4)
#define L2B_ETYPR_EN        (1 << 5)
#define L2B_DSCP_EN         (1 << 6)
#define TLS_DIS_DMAC        (1 << 8)
#define TLS_DIS_PBITS       (1 << 9)
#define TLS_DIS_OTHER       (1 << 10)
#define TLS_DIS_SMAC		(1 << 11)
#define L2B_VLAN1_EN        (1 << 11)
//en7580 add
#define L2B_ETYPE_BLIST     (1 << 16)
#define L2B_MC_EN           (1 << 17)
//EN7523 add
#define L2B_NO_IP_EN		(1 << 18)
#define L2B_BC_EN			(1 << 19)
#define L2B_SECOND_VLAN_EN	(1 << 20)

#define GDM1_OFRC_P_CPU     (0 << 0)
#define GDM1_MFRC_P_CPU     (0 << 4)
#define GDM1_BFRC_P_CPU     (0 << 8)
#define GDM1_UFRC_P_CPU     (0 << 12)

#define GDM1_OFRC_P_QDMA     (5 << 0)
#define GDM1_MFRC_P_QDMA     (5 << 4)
#define GDM1_BFRC_P_QDMA     (5 << 8)
#define GDM1_UFRC_P_QDMA     (5 << 12)


#define GDM1_OFRC_P_PPE     (4 << 0)
#define GDM1_MFRC_P_PPE     (4 << 4)
#define GDM1_BFRC_P_PPE     (4 << 8)
#define GDM1_UFRC_P_PPE     (4 << 12)

/* source port define */ 
#define SP_PDMA				(0)
#define SP_GDMA1			(1)
#define SP_GDMA2			(2)
#define SP_PPE				(4)
#define SP_QDMA				(5)
#define SP_NPU				(6)
#define SP_TDMA				(7)
#define SP_GSW_PORT0		(8)
#define SP_GSW_PORT1		(9)
#define SP_GSW_PORT2		(10)
#define SP_GSW_PORT3		(11)
#define SP_GSW_PORT4		(12)
#define SP_GSW_PORT5		(13)
#define SP_GSW_PORT6		(14)
#define SP_GSW_PORT7		(15)		

#define WHITE_LIST			(0)
#define BLACK_LIST			(1)


/* TO PPE */
#define IPV4_PPE_MYUC	    (1 << 0) /* my mac */
#define IPV4_PPE_MC	    (1 << 1) /* multicast */
#define IPV4_PPE_IPM	    (1 << 2) /* ip multicast */
#define IPV4_PPE_BC	    (1 << 3) /* broadcast */
#define IPV4_PPE_UC	    (1 << 4) /* ipv4 learned UC frame */
#define IPV4_PPE_UN	    (1 << 5) /* ipv4 unknown  UC frame */

#define IPV6_PPE_MYUC	    (1 << 8) /* my mac */
#define IPV6_PPE_MC	    (1 << 9) /* multicast */
#define IPV6_PPE_IPM	    (1 << 10) /* ipv6 multicast */
#define IPV6_PPE_BC	    (1 << 11) /* broadcast */
#define IPV6_PPE_UC	    (1 << 12) /* ipv6 learned UC frame */
#define IPV6_PPE_UN	    (1 << 13) /* ipv6 unknown  UC frame */

#define METER_BASE	    RALINK_FRAME_ENGINE_BASE + 0x1200

/* EN7523 WAN_MTU base */
#define WAN_MY_MAC_HIG	(RALINK_FRAME_ENGINE_BASE + 0x30)
#define WAN_MY_MAC_MIN	(RALINK_FRAME_ENGINE_BASE + 0x34)
#define WAN_MY_MAC_MAX	(RALINK_FRAME_ENGINE_BASE + 0x38)
#define WAN_MTU_N(idx)	(RALINK_FRAME_ENGINE_BASE + 0x2300 + ((idx/2)<<2))

/*EN7523 NPTV6 PREFIX*/
#define PREFIX_WR_REQ_SHIFT 0
#define PREFIX_DATA_SEL_SHIFT 2
#define PREFIX_ID_SHIFT 4
#define PPE_NPTV6_PREFIX_CTRL	(RALINK_PPE_BASE+0x378)
#define PPE_NPTV6_PREFIX_DATA	(RALINK_PPE_BASE+0x37c)

/*********  7520/7512/7522/7516 Start  *************/
#define L2LU_KEY0_BASE		    RALINK_FRAME_ENGINE_BASE + 0x200
#define L2LU_KEY1_BASE		    L2LU_KEY0_BASE + 0x04
/*********  7520/7512/7522/7516 End  *************/
#define IFC_VLAN_KEY_BASE       82
#define FE_MISC_CFG_7528         (RALINK_FRAME_ENGINE_BASE + 0x124)
#define FE_MISC_CFG_7580         (RALINK_FRAME_ENGINE_BASE + 0x194)

#define CONFIG_RA_NAT_HW
#define CONFIG_RALINK_MT7620

#define CONFIG_RA_HW_NAT_LAN_VLANID 1
#define CONFIG_RA_HW_NAT_WAN_VLANID 5

#define CONFIG_RA_HW_NAT_BINDING_THRESHOLD	30
#define CONFIG_RA_HW_NAT_QURT_LMT		4000
#define CONFIG_RA_HW_NAT_HALF_LMT		4000
#define CONFIG_RA_HW_NAT_FULL_LMT		8000

/* #define CONFIG_RA_HW_NAT_TBL_1K
 * #define CONFIG_RA_HW_NAT_TBL_2K
 * #define CONFIG_RA_HW_NAT_TBL_4K
 * #define CONFIG_RA_HW_NAT_TBL_16K */
#define CONFIG_RA_HW_NAT_TBL_8K


#define CONFIG_RA_HW_NAT_TCP_KA			1
#define CONFIG_RA_HW_NAT_UDP_KA			1
#define CONFIG_RA_HW_NAT_NTU_KA			1
/* #define CONFIG_RA_HW_NAT_PREBIND		1 */

#define CONFIG_RA_HW_NAT_UNB_DLTA		3
#define CONFIG_RA_HW_NAT_UNB_MNP		1000
#define CONFIG_RA_HW_NAT_UDP_DLTA		15
#define CONFIG_RA_HW_NAT_TCP_DLTA		15
#define CONFIG_RA_HW_NAT_FIN_DLTA		5
#define CONFIG_RA_HW_NAT_NTU_DLTA		15

#define CONFIG_RA_HW_NAT_L2B
#define CONFIG_RA_HW_NAT_IP_FRAG

#define CONFIG_RA_HW_NAT_WIFI
#define WLAN_IF_NUM		    8 /* 8 is maximum */
#define WLAN_IF_I_NUM		8 /* 8 is maximum, foe rai */

#define CONFIG_RA_HW_NAT_VPN_PASSTHROUGH
#define CONFIG_RA_HW_NAT_IGMP

/* PPE_GLO_CFG, Offset=0x200 */
#define DFL_TTL0_DRP		(1)	/* 1:Drop, 0: Alert CPU */
#define PPE_PAD_EN          (0) /* 1:Padding, 0: Disable */

/* 
 * PPE Flow Set 
 */
#define BIT_IPV6_HASH_GRE_EN		(1<<20)	
#define BIT_IPV4_HASH_GRE_EN		(1<<19)	
#define BIT_L2_BRIDGE_EN			(1<<15)	
#define BIT_IPV4_NAT_FRAG_UDP_EN	(1<<7)
#define BIT_IPV4_NAT_FRAG_TCP_EN	(1<<6)	
#define BIT_IPV6_3T_ROUTE_EN	(1<<8)	/* IPv6 3-tuple route */
#define BIT_IPV6_5T_ROUTE_EN	(1<<9)	/* IPv6 5-tuple route */
#define BIT_IPV6_6RD_EN		(1<<10)	/* IPv6 6RD */
#define BIT_IPV4_NAT_EN		(1<<12)	/* IPv4 NAT */
#define BIT_IPV4_NAPT_EN	(1<<13)	/* IPv4 NAPT */
#define BIT_IPV4_DSL_EN		(1<<14)	/* IPv4 DS-Lite */
#define BIT_IP_PROT_CHK_BLIST	(1<<16)	/* IP protocol check is black/white list */
#define BIT_IPV4_NAT_FRAG_EN	(1<<17)	/* Enable fragment support for IPv4 NAT flow */
#define BIT_IPV6_HASH_FLAB	(1<<18)	/* For IPv6 5-tuple and 6RD flow, using flow label instead of sport and dport to do HASH */
/* when enable, PPE use HSK_MC instead of SP/DP for hash */
#define BIT_IPV4_MC_EN      (1<<21)
#define BIT_IPV4_DSL_MC_EN  (1<<22)
#define BIT_IPV6_MC_EN      (1<<23)
#define BIT_IPV6_6RD_MC_EN  (1<<24)
#define BIT_IPV6_MC_HPRI_EN (1<<25)
#define BIT_IP_MC_HPIT_EN   (1<<26)
#define BIT_IPV4_ESP_EN     (1<<27)
#define BIT_IPV6_ESP_EN     (1<<28)
#define BIT_IPV6_HASH_L2TP_EN     (1<<3)
#define BIT_IPV4_HASH_L2TP_EN     (1<<2)
#define BIT_IPV6_HASH_VXLAN_EN     (1<<1)
#define BIT_IPV4_HASH_VXLAN_EN     (1<<0)

/* 
 * PPE FOE Bind Rate 
 */
/* packet in a time stamp unit */
#define DFL_FOE_BNDR		CONFIG_RA_HW_NAT_BINDING_THRESHOLD

/* 
 * PPE_FOE_LMT 
 */
/* smaller than 1/4 of total entries */
#define DFL_FOE_QURT_LMT	CONFIG_RA_HW_NAT_QURT_LMT

/* between 1/2 and 1/4 of total entries */
#define DFL_FOE_HALF_LMT	CONFIG_RA_HW_NAT_HALF_LMT

/* between full and 1/2 of total entries */
#define DFL_FOE_FULL_LMT	CONFIG_RA_HW_NAT_FULL_LMT

/* 
 * PPE_FOE_KA 
 */
/* visit a FOE entry every FOE_KA_T * 1 msec */
#define DFL_FOE_KA_T		1

/* FOE_TCP_KA * FOE_KA_T * FOE_4TB_SIZ */
#define DFL_FOE_TCP_KA		CONFIG_RA_HW_NAT_TCP_KA

/* FOE_UDP_KA * FOE_KA_T * FOE_4TB_SIZ */
#define DFL_FOE_UDP_KA		CONFIG_RA_HW_NAT_UDP_KA

/* FOE_NTU_KA * FOE_KA_T * FOE_4TB_SIZ */
#define DFL_FOE_NTU_KA		CONFIG_RA_HW_NAT_NTU_KA

/* 
 * PPE_FOE_CFG 
 */
 
/* #define DFL_FOE_HASH_MODE	0
 * #define DFL_FOE_HASH_MODE	1
 * #define DFL_FOE_HASH_MODE	2 
 */
/* means SRAM Hash0 Mode and DRAM Hash Mode for EN7580 */
#define DFL_FOE_HASH_MODE	3
/* means SRAM Hash1 Mode for EN7580 */
#define DFL_FOE_HASH1_MODE	1

#define DFL_FOE_HASH_MODE_EN751221 0
#define DFL_FOE_HASH_MODE_1			1

#define HASH_SEED		0x12345678
#define DFL_FOE_UNB_AGE		1	/* Unbind state age enable */
#define DFL_FOE_TCP_AGE		1	/* Bind TCP age enable */
#define DFL_FOE_NTU_AGE		1	/* Bind TCP age enable */
#define DFL_FOE_UDP_AGE		1	/* Bind UDP age enable */
#define DFL_FOE_FIN_AGE		1	/* Bind TCP FIN age enable */
#define DFL_FOE_PBIND_AGE		1	/* Bind Pre-Bind age enable */

#define DFL_BYTE_SWAP      (1)  /* 1:Enable byte swap */
#define BYTE_SWAP_DIS  (0)  /* 0: Disable byte swap */

#define DFL_FOE_KA		3	/* 0:disable 1:unicast old 2: multicast new 3. duplicate old */

/* 
 * PPE_FOE_UNB_AGE 
 */
/*The min threshold of packet count for aging out at unbind state */
#define DFL_FOE_UNB_MNP		CONFIG_RA_HW_NAT_UNB_MNP
/* Delta time for aging out an ACL link to FOE entry */
#define DFL_FOE_ACL_DLTA        CONFIG_RA_HW_NAT_ACL_DLTA
/* Delta time for aging out an unbind FOE entry */
#define DFL_FOE_UNB_DLTA	CONFIG_RA_HW_NAT_UNB_DLTA


/* 
 * PPE_FOE_BND_AGE1 
 */
/* Delta time for aging out an bind UDP FOE entry */
#define DFL_FOE_UDP_DLTA	CONFIG_RA_HW_NAT_UDP_DLTA

/* 
 * PPE_FOE_BND_AGE2
 */
/* Delta time for aging out an bind TCP FIN entry */
#define DFL_FOE_FIN_DLTA 	CONFIG_RA_HW_NAT_FIN_DLTA
/* Delta time for aging out an bind TCP entry */
#define DFL_FOE_TCP_DLTA	CONFIG_RA_HW_NAT_TCP_DLTA
/* Delta time for aging out an bind Non-TCP/UDP FOE entry */
#define DFL_FOE_NTU_DLTA	CONFIG_RA_HW_NAT_NTU_DLTA

#define DFL_FOE_TTL_REGEN	1	/* TTL = TTL -1 */



/* PPE_GLO_CFG */
#define FID_HASH_EN							(1<<6)
#define TUN_HASH_EN							(1<<7)
#define L2TP_PARSER_EN						(1<<16)
#define NPTV6_MODE							(1<<17)
#define NPTV6_MODE_SHIFT					17
#define NPTV6_MODE_MASK					(1<<NPTV6_MODE_SHIFT)


/* PPE_TB_CFG */
#define DRAM_HASH1_EN						(1<<24)
#define VLAN_OP_MODE						(1<<28)
#define FIN_FAST_RP							(1<<27)
#define SRAM_TB_ETRY_NUM_SHIFT				(24)
#define SRAM_TB_ETRY_NUM_MASK				(0x7<<SRAM_TB_ETRY_NUM_SHIFT)
#define HASH_SHIFT_VECTOR_SHIFT				(16)
#define HASH_SHIFT_VECTOR_MASK				(0xF<<HASH_SHIFT_VECTOR_SHIFT)
#define DRAM_HASH1_MODE_SHIFT				(28)
#define DRAM_HASH1_MODE_MASK				(0xF<<DRAM_HASH1_MODE_SHIFT)
#define KA_CFG_SHIFT						(12)
#define KA_CFG_MASK							(0x3<<KA_CFG_SHIFT)
#define SEARCH_MISS_ACTION_SHIFT			(4)
#define SEARCH_MISS_ACTION_MASK				(0x3<<SEARCH_MISS_ACTION_SHIFT)
#define TB_ENTRY_SIZE						(1<<3)
#define DRAM_TB_ETRY_NUM_SHIFT				(0)
#define DRAM_TB_ETRY_NUM_MASK				(0x7<<DRAM_TB_ETRY_NUM_SHIFT)

/* SRAM_CTRL */
#define SRAM_CTRL_ACK						(1<<31)
#define SRAM_CTRL_DUAL_SUCESS				(1<<30)
#define SRAM_CTRL_ENTRY_SHIFT				(8)
#define SRAM_CTRL_ENTRY_MASK				(0xFFFF<<SRAM_CTRL_ENTRY_SHIFT)
#define WR_DUAL_DIRECTION					(1<<2)
#define SRAM_CTRL_WR						(1<<1)
#define SRAM_CTRL_REQ						(1<<0)

/* PPE_UPDMEM_CTRL */
#define PPE_UPDMEM_ACK						(1<<31)
#define PPE_UPDMEM_ADDR_SHIFT				(8)
#define PPE_UPDMEM_ADDR_MASK				(0xF<<PPE_UPDMEM_ADDR_SHIFT)
#define PPE_UPDMEM_OFST_SHIFT				(4)
#define PPE_UPDMEM_OFST_MASK				(0xF<<PPE_UPDMEM_OFST_SHIFT)
#define PPE_UPDMEM_SEL_SMAC			(0)
#define PPE_UPDMEM_SEL_IPv6			(1)
#define PPE_UPDMEM_SEL_IPv4			(2)
#define PPE_UPDMEM_SEL_SHIFT				(2)
#define PPE_UPDMEM_SEL_MASK					(0x3<<PPE_UPDMEM_SEL_SHIFT)
#define PPE_UPDMEM_CTRL_READ		(0)
#define PPE_UPDMEM_CTRL_WRITE		(1)
#define PPE_UPDMEM_WR						(1<<1)
#define PPE_UPDMEM_REQ						(1<<0)


#define fidIntoHashKeyEnable()				IO_SBITS(PPE_GLO_CFG, FID_HASH_EN)
#define fidIntoHashKeyDisable()				IO_CBITS(PPE_GLO_CFG, FID_HASH_EN)
#define isFidIntoHashKeyEnable()			(IO_GREG(PPE_GLO_CFG) & FID_HASH_EN)
#define isTunOFsetIntoHashKeyEnable()		(IO_GREG(PPE_GLO_CFG) & TUN_HASH_EN)
#define ppeSetL2tpParserEnable()			IO_SBITS(PPE_GLO_CFG, L2TP_PARSER_EN)
#define ppeSetL2tpParserDisable()			IO_CBITS(PPE_GLO_CFG, L2TP_PARSER_EN)
#define ppeGetNptV6Mode()					((IO_GREG(PPE_GLO_CFG) & NPTV6_MODE)>>NPTV6_MODE_SHIFT)
#define ppeSetNptV6Mode(val)				IO_SMASK(PPE_GLO_CFG, NPTV6_MODE_MASK, NPTV6_MODE_SHIFT, val)

#define dramHash1ModeEnable()				IO_SBITS(PPE_TB_HASH_CFG, DRAM_HASH1_EN)
#define dramHash1ModeDisable()				IO_CBITS(PPE_TB_HASH_CFG, DRAM_HASH1_EN)
#define isDramHash1ModeEnable()				(IO_GREG(PPE_TB_HASH_CFG) & DRAM_HASH1_EN)

#define ppeSetVlanOperationMode()			IO_SBITS(PPE_TB_CFG, VLAN_OP_MODE)
#define ppeSetVlanLayerMode()				IO_CBITS(PPE_TB_CFG, VLAN_OP_MODE)
#define ppeSetFINFastRp()					IO_SBITS(PPE_TB_CFG, FIN_FAST_RP)
#define ppeSetSramTableEntryNum(val)		IO_SMASK(PPE_TB_CFG, SRAM_TB_ETRY_NUM_MASK, SRAM_TB_ETRY_NUM_SHIFT, val)
#define ppeGetSramTableEntryNum()			IO_GMASK(PPE_TB_CFG, SRAM_TB_ETRY_NUM_MASK, SRAM_TB_ETRY_NUM_SHIFT)
#define ppeSetDramTableEntryNum(val)		IO_SMASK(PPE_TB_CFG, DRAM_TB_ETRY_NUM_MASK, DRAM_TB_ETRY_NUM_SHIFT, val)
#define ppeGetDramTableEntryNum()			IO_GMASK(PPE_TB_CFG, DRAM_TB_ETRY_NUM_MASK, DRAM_TB_ETRY_NUM_SHIFT)
#define ppeSetDramHash1Mode(val)			IO_SMASK(PPE_TB_HASH_CFG, DRAM_HASH1_MODE_MASK, DRAM_HASH1_MODE_SHIFT, val)
#define ppeGetDramHash1Mode()				IO_GMASK(PPE_TB_HASH_CFG, DRAM_HASH1_MODE_MASK, DRAM_HASH1_MODE_SHIFT)

#define ppeSetHashShiftVector(val)			IO_SMASK(PPE_TB_CFG, HASH_SHIFT_VECTOR_MASK, HASH_SHIFT_VECTOR_SHIFT, val)
#define ppeGetHashShiftVector()				IO_GMASK(PPE_TB_CFG, HASH_SHIFT_VECTOR_MASK, HASH_SHIFT_VECTOR_SHIFT)
#define ppeSetKeepAliveCfg(val)				IO_SMASK(PPE_TB_CFG, KA_CFG_MASK, KA_CFG_SHIFT, val)
#define ppeGetKeepAliveCfg()				IO_GMASK(PPE_TB_CFG, KA_CFG_MASK, KA_CFG_SHIFT)
#define ppeSetSearchMissAction(val)			IO_SMASK(PPE_TB_CFG, SEARCH_MISS_ACTION_MASK, SEARCH_MISS_ACTION_SHIFT, val)
#define ppeGetSearchMissAction()			IO_GMASK(PPE_TB_CFG, SEARCH_MISS_ACTION_MASK, SEARCH_MISS_ACTION_SHIFT)
#define ppeSetTableEntrySize80Byte()		IO_SBITS(PPE_TB_CFG, TB_ENTRY_SIZE)
#define ppeSetTableEntrySize64Byte()		IO_CBITS(PPE_TB_CFG, TB_ENTRY_SIZE)

#define L2bNoIPEnable()						IO_SBITS(PPE_L2B_CFG, L2B_NO_IP_EN)
#define L2bNoIPDisable()					IO_CBITS(PPE_L2B_CFG, L2B_NO_IP_EN)
#define L2bBroadcastEnable()				IO_SBITS(PPE_L2B_CFG, L2B_BC_EN)
#define L2bBroadcastDisable()				IO_CBITS(PPE_L2B_CFG, L2B_BC_EN)
#define L2bMulticastEnable()				IO_SBITS(PPE_L2B_CFG, L2B_MC_EN)
#define L2bMulticastDisable()				IO_CBITS(PPE_L2B_CFG, L2B_MC_EN)
#define isL2bMulticastEnable()				(IO_GREG(PPE_L2B_CFG) & L2B_MC_EN)
#define L2bSetEtypeBlackList()				IO_SBITS(PPE_L2B_CFG, L2B_ETYPE_BLIST)
#define L2bSetEtypeWhiteList()				IO_CBITS(PPE_L2B_CFG, L2B_ETYPE_BLIST)
#define isL2bEtypeBlackList()				(IO_GREG(PPE_L2B_CFG) & L2B_ETYPE_BLIST)
#define L2bSmacEnable()						IO_SBITS(PPE_L2B_CFG, L2B_SMAC_EN)
#define L2bSmacDisable()					IO_CBITS(PPE_L2B_CFG, L2B_SMAC_EN)
#define isL2bSmacEnable()					(IO_GREG(PPE_L2B_CFG) & L2B_SMAC_EN)

#define sramSetCtrlEntryVal(val)			IO_SMASK(SRAM_CTRL, SRAM_CTRL_ENTRY_MASK, SRAM_CTRL_ENTRY_SHIFT, val)
#define sramGetCtrlEntryVal()				IO_GMASK(SRAM_CTRL, SRAM_CTRL_ENTRY_MASK, SRAM_CTRL_ENTRY_SHIFT)

#define ppeMirrorEnable()                   IO_SBITS(PPE_MIRROR, MIRROR_EN)
#define ppeMirrorDisable()                  IO_CBITS(PPE_MIRROR, MIRROR_EN)
#define ppeSetMirrorFP(val)			        IO_SMASK(PPE_MIRROR, MIRROR_FP_MASK, MIRROR_FP_SHIFT, val)
#define ppeSetMirrorSPTAG(val)			    IO_SMASK(PPE_MIRROR, MIRROR_SPTAG_MASK, MIRROR_SPTAG_SHIFT, val)

#define ppeMirrorDecModeOr()               IO_SBITS(PPE_MIRROR_DEC, MIRROR_DEC_MODE)
#define ppeMirrorDecModeAnd()              IO_CBITS(PPE_MIRROR_DEC, MIRROR_DEC_MODE)
#define ppeMirrorDstPreEnable()            IO_SBITS(PPE_MIRROR_DEC, MIRROR_DST_PRE_EN) 
#define ppeMirrorDstPreDisable()           IO_CBITS(PPE_MIRROR_DEC, MIRROR_DST_PRE_EN) 
#define ppeMirrorDstPostEnable()           IO_SBITS(PPE_MIRROR_DEC, MIRROR_DST_POST_EN) 
#define ppeMirrorDstPostDisable()          IO_CBITS(PPE_MIRROR_DEC, MIRROR_DST_POST_EN) 
#define ppeMirrorDstQEnable()              IO_SBITS(PPE_MIRROR_DEC, MIRROR_DST_POST_EN) 
#define ppeMirrorDstQDisable()             IO_CBITS(PPE_MIRROR_DEC, MIRROR_DST_POST_EN) 
#define ppeSetMirrorDst(val)			   IO_SMASK(PPE_MIRROR_DEC, MIRROR_DST_MASK, MIRROR_DST_SHIFT, val)
#define ppeMirrorSrcPreEnable()            IO_SBITS(PPE_MIRROR_DEC, MIRROR_SRC_PRE_EN) 
#define ppeMirrorSrcPreDisable()           IO_CBITS(PPE_MIRROR_DEC, MIRROR_SRC_PRE_EN) 
#define ppeMirrorSrcPostEnable()           IO_SBITS(PPE_MIRROR_DEC, MIRROR_SRC_POST_EN) 
#define ppeMirrorSrcPostDisable()          IO_CBITS(PPE_MIRROR_DEC, MIRROR_SRC_POST_EN) 
#define ppeSetMirrorSrc(val)			   IO_SMASK(PPE_MIRROR_DEC, MIRROR_SRC_MASK, MIRROR_SRC_SHIFT, val)
#define ppeMirrorDecClear()                IO_SREG(PPE_MIRROR_DEC, 0)

#define ppe2MirrorEnable()                   IO_SBITS(PPE2_MIRROR, MIRROR_EN)
#define ppe2MirrorDisable()                  IO_CBITS(PPE2_MIRROR, MIRROR_EN)
#define ppe2SetMirrorFP(val)			        IO_SMASK(PPE2_MIRROR, MIRROR_FP_MASK, MIRROR_FP_SHIFT, val)
#define ppe2SetMirrorSPTAG(val)			    IO_SMASK(PPE2_MIRROR, MIRROR_SPTAG_MASK, MIRROR_SPTAG_SHIFT, val)

#if 0
#define ppeSetUpdateMemAddress(val)			IO_SMASK(PPE_UPDMEM_CTRL, PPE_UPDMEM_ADDR_MASK, PPE_UPDMEM_ADDR_SHIFT, val)
#define ppeGetUpdateMemAddress()			IO_GMASK(PPE_UPDMEM_CTRL, PPE_UPDMEM_ADDR_MASK, PPE_UPDMEM_ADDR_SHIFT)
#define ppeSetUpdateMemOffset(val)			IO_SMASK(PPE_UPDMEM_CTRL, PPE_UPDMEM_OFST_MASK, PPE_UPDMEM_OFST_SHIFT, val)
#define ppeGetUpdateMemOffset()				IO_GMASK(PPE_UPDMEM_CTRL, PPE_UPDMEM_OFST_MASK, PPE_UPDMEM_OFST_SHIFT)
#define ppeSetUpdateMemSelect(val)			IO_SMASK(PPE_UPDMEM_CTRL, PPE_UPDMEM_SEL_MASK, PPE_UPDMEM_SEL_SHIFT, val)
#define ppeGetUpdateMemSelect()				IO_GMASK(PPE_UPDMEM_CTRL, PPE_UPDMEM_SEL_MASK, PPE_UPDMEM_SEL_SHIFT)
#endif
#define ppeSetUpdMemData(val)				IO_SREG(PPE_UPDMEM_DATA, val)
#define ppeGetUpdMemData()					IO_GREG(PPE_UPDMEM_DATA)

#define ppeEnable()                            IO_SBITS(PPE_GLO_CFG, PPE_EN)
#define ppeDisable()                           IO_CBITS(PPE_GLO_CFG, PPE_EN)

#define vxlanv4hashEnable()                           IO_SBITS(PPE_FLOW_CFG, BIT_IPV4_HASH_VXLAN_EN)
#define vxlanv4hashDisable()                          IO_CBITS(PPE_FLOW_CFG, BIT_IPV4_HASH_VXLAN_EN)
#define isvxlanv4hashEnable()						  (IO_GREG(PPE_FLOW_CFG) & BIT_IPV4_HASH_VXLAN_EN)
#define vxlanv6hashEnable()                           IO_SBITS(PPE_FLOW_CFG, BIT_IPV6_HASH_VXLAN_EN)
#define vxlanv6hashDisable()                          IO_CBITS(PPE_FLOW_CFG, BIT_IPV6_HASH_VXLAN_EN)
#define isvxlanv6hashEnable()						  (IO_GREG(PPE_FLOW_CFG) & BIT_IPV6_HASH_VXLAN_EN)
#define l2tpv4hashEnable()                            IO_SBITS(PPE_FLOW_CFG, BIT_IPV4_HASH_L2TP_EN)
#define l2tpv4hashDisable()                           IO_CBITS(PPE_FLOW_CFG, BIT_IPV4_HASH_L2TP_EN)
#define isl2tpv4hashEnable()						  (IO_GREG(PPE_FLOW_CFG) & BIT_IPV4_HASH_L2TP_EN)
#define l2tpv6hashEnable()                            IO_SBITS(PPE_FLOW_CFG, BIT_IPV6_HASH_L2TP_EN)
#define l2tpv6hashDisable()                           IO_CBITS(PPE_FLOW_CFG, BIT_IPV6_HASH_L2TP_EN)
#define isl2tpv6hashEnable()						  (IO_GREG(PPE_FLOW_CFG) & BIT_IPV6_HASH_L2TP_EN)

/************************************************************************
*               M A C R O S
*************************************************************************
*/
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2], \
                       ((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]

#define IPV6_ADDR(x) ntohs(x[0]),ntohs(x[1]),ntohs(x[2]),ntohs(x[3]),ntohs(x[4]),\
		     ntohs(x[5]),ntohs(x[6]),ntohs(x[7])

#define NAT_PRINT(fmt, args...) printk(KERN_INFO fmt, ## args)

#define IS_IPV6_FLAB_EBL()	((RegRead(PPE_FLOW_SET) & BIT_IPV6_HASH_FLAB) ? 1 : 0)
#define IS_FLAB_EBL_HIGH_PRIO()	((RegRead(PPE_FLOW_SET) & BIT_IPV6_MC_HPRI_EN) ? 0 : 1)


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
enum HashSelect {
	HashMode_Sram0 = 0,
	HashMode_Sram1,
	HashMode_Dram,
	HashMode_Dram1
};

enum FoeSma {
	DROP = 0,		/* Drop the packet */
	DROP2 = 1,		/* Drop the packet */
	ONLY_FWD_CPU = 2,	/* Only Forward to CPU */
	FWD_CPU_BUILD_ENTRY = 3	/* Forward to CPU and build new FOE entry */
};

enum FoeCpuReason {
	IPTU_CSUMF = 0x1, /* ipv4, tcp udp checksum fail */
	TTL_0 = 0x02, /* IPv4(IPv6) TTL(hop limit) = 0 */
	HAS_OPTION_HEADER = 0x03, /* IPv4(IPv6) has option(extension) header */
	NO_FLOW_IS_ASSIGNED = 0x07,	/* No flow is assigned */
	IPV4_WITH_FRAGMENT = 0x08,	/* IPv4 HNAT doesn't support IPv4 /w fragment */
	IPV4_HNAPT_DSLITE_WITH_FRAGMENT = 0x09,	/* IPv4 HNAPT/DS-Lite doesn't support IPv4 /w fragment */
	IPV4_HNAPT_DSLITE_WITHOUT_TCP_UDP = 0x0A,	/* IPv4 HNAPT/DS-Lite can't find TCP/UDP sport/dport */
	IPV6_5T_6RD_WITHOUT_TCP_UDP = 0x0B,	/* IPv6 5T-route/6RD can't find TCP/UDP sport/dport */
	TCP_FIN_SYN_RST = 0x0C,	/* Ingress packet is TCP fin/syn/rst (for IPv4 NAPT/DS-Lite or IPv6 5T-route/6RD) */
	UN_HIT = 0x0D,		/* FOE Un-hit */
	HIT_UNBIND = 0x0E,	/* FOE Hit unbind */
	HIT_UNBIND_RATE_REACH = 0x0F,	/* FOE Hit unbind & rate reach */
	HIT_BIND_TCP_FIN = 0x10,	/* Hit bind PPE TCP FIN entry */
	HIT_BIND_TTL_1 = 0x11,	/* Hit bind PPE entry and TTL(hop limit) = 1 and TTL(hot limit) - 1 */
	HIT_BIND_WITH_VLAN_VIOLATION = 0x12,	/* Hit bind and VLAN replacement violation
						   (Ingress 1(0) VLAN layers and egress 4(3 or 4) VLAN layers) */
	HIT_BIND_KEEPALIVE_UC_OLD_HDR = 0x13,	/* Hit bind and keep alive with unicast old-header packet */
	HIT_BIND_KEEPALIVE_MC_NEW_HDR = 0x14,	/* Hit bind and keep alive with multicast new-header packet */
	HIT_BIND_KEEPALIVE_DUP_OLD_HDR = 0x15,	/* Hit bind and keep alive with duplicate old-header packet */
	HIT_BIND_FORCE_TO_CPU = 0x16,	/* FOE Hit bind & force to CPU */
	HIT_BIND_WITH_OPTION_HEADER = 0x17, /* Hit bind and remove tunnel IP header, but inner IP has option/next header */
	HIT_BIND_MUL_CPU = 0x18, /*  Hit Bind and Multicast to CPU*/
	HIT_BIND_MUL_CPUR = 0x19, /*  Hit Bind and Multicast to CPU force to CPU*/
	HIT_PREBIND = 0x1A, /*  Hit Pre-Bind*/
	UNHIT_CLASS = 0x1B, /*  UnHit CLASS Packet*/
	HIT_BIND_EXCEED_MTU = 0x1C,	/* Hit bind and exceed MTU */
	NOT_THROUGH_PPE = 0x1E /* Packet not go through PPE */
};

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

struct hwnat_shrink_field {
	//unsigned int smac[UPDMEM_SMAC_LINE];		// 0:smac[31~0]; 1:smac[48~32];
	unsigned char smac[UPDMEM_SMAC_CNT];		// 0:smac[31~0]; 1:smac[48~32];
	unsigned int eg_ipv4[UPDMEM_IPV4_LINE]; 	// 0:dipv4; 1:sipv4
	unsigned int eg_ipv6[UPDMEM_IPV6_LINE]; 	// 0:dipv6[31:0], ....., 7:sipv6[127:96]
};

struct hwnat_shrink_table {
	//unsigned int smac[UPDMEM_SMAC_LINE];		// 0:smac[31~0]; 1:smac[48~32];
	unsigned char smac[UPDMEM_SMAC_CNT];		// 0:smac[47:40], .... ,5:smac[7:0];
	unsigned int eg_ipv4[UPDMEM_IPV4_LINE]; 	// 0:dipv4; 1:sipv4
	unsigned int eg_ipv6[UPDMEM_IPV6_LINE]; 	// 0:dipv6[31:0], ....., 7:sipv6[127:96]
	unsigned int valid[3];
	unsigned long timestamp[3];
};

struct hwnat_wan_ip6_table {
	unsigned int wan_ip6[4];
	unsigned int used;
	unsigned long timestamp;
};

typedef struct {
	//layer2 header
	uint8_t dmac[6];
	uint8_t smac[6];

	//vlan header 
	uint16_t vlan_tag;
	uint16_t vlan1_gap;
	uint16_t vlan1;
	uint16_t vlan2_gap;
	uint16_t vlan2;
	uint16_t vlan_layer;

	//pppoe header
	uint32_t pppoe_gap;
	uint16_t ppp_tag;
	uint16_t pppoe_sid;

	//layer3 header
	uint16_t eth_type;
	struct iphdr iph;
	struct ipv6hdr ip6h;

	//layer4 header
	struct tcphdr th;
	struct udphdr uh;
	uint32_t gre_call_id;

	uint32_t pkt_type;
	uint8_t rmt:1;
	uint8_t psn:1;
	uint8_t map_e:1;	
	uint8_t resv:5;




	struct gre_base_hdr greh;
	struct vxlanhdr vxlanh;
	uint16_t l2tp_tunnelid;
	uint16_t l2tp_sessionid;
	uint16_t l2tp_ppp_tag;


	//layer2 header
	uint8_t dmac_intunnel[6];
	uint8_t smac_intunnel[6];

	//vlan header 
	uint16_t vlan_tag_intunnel;
	uint16_t vlan1_gap_intunnel;
	uint16_t vlan1_intunnel;
	uint16_t vlan2_gap_intunnel;
	uint16_t vlan2_intunnel;
	uint16_t vlan_layer_intunnel;

	//pppoe header
	uint32_t pppoe_gap_intunnel;
	uint16_t ppp_tag_intunnel;
	uint16_t pppoe_sid_intunnel;

	//layer3 header
	uint16_t eth_type_intunnel;
	struct iphdr iph_intunnel;
	struct ipv6hdr ip6h_intunnel;

	//layer4 header
	struct tcphdr th_intunnel;
	struct udphdr uh_intunnel;
	uint32_t pkt_type_intunnel;

    uint32_t vxlan_vni;
	uint32_t is_L2_tunnel:1;// layer 2 tunnel need parse second packer,
	uint32_t is_L3_tunnel:1;// layer 2 tunnel need parse second packer,
	uint32_t is_vxlan_tunnel:1;
	uint32_t is_l2tp_tunnel:1;
	uint32_t hop_0:3;//6 add runnel 7 rm tunnel 0 none 
	uint32_t hop_1:3;
	uint32_t hop_2:3;
	uint32_t hop_3:3;
	uint32_t tunnel_en:1;
	uint32_t tunnel_id:6;
	uint32_t resv_tun:7;

	union {
		struct 
		{			
			
			uint32_t is_l2_tunnel:1;	/* is layer2 tunnel */
			uint32_t use_inner_ttl:1;	/* use table ttl */
			uint32_t use_inner_DSCP:1;	/* use table DSCP */
			uint32_t gre_key_flag:1;	/* gre key flag */
			uint32_t l2tp_over_ip:1;	/* l2tp_over_ip */
			uint32_t l2tp_v3:1;			/* l2tpv3 */
			uint32_t ipv4_id:1;			/* ipv4 id inc */
			uint32_t rsv0:4;			/*  */
			uint32_t udp_cks:1;			/* udp cks*/
			uint32_t pppoe_en:1;		/* pppoe_en*/
			uint32_t vlan_en:1;			/* vlan_en*/
			uint32_t vpm:2;				/* vpm*/
			uint32_t tun0:4;			/* tun0*/
			uint32_t tun1:4;			/* tun1*/
			uint32_t tun2:4;			/* tun2*/
			uint32_t tun3:4;			/* tun3*/
		};
		uint32_t tunnelcfg_word;
	};
	 
	
} PktParseResult;

typedef struct smac_table_entry
{
	uint32_t smac_hi;
	uint32_t smac_lo;
	union {
		struct 
		{			
			uint VID0         : 12;
			uint timeStamp    : 12;
			uint srcPort      : 5;
			uint drop         : 1;
			uint sta          : 1;
			uint valid        : 1;
		};
		uint32_t word;
	};
} Smac_Table_Entry_T ;

struct FoePriKey {
	/* TODO: add new primary key to support dslite, 6rd */

	/* Ipv4 */
	struct {
		uint32_t sip;
		uint32_t dip;
		uint16_t dport;
		uint16_t sport;
        uint16_t gre_call_id;
		uint8_t prot;
		uint8_t is_udp;
	} ipv4_hnapt;

	struct {
		uint32_t sip;
		uint32_t dip;
		/* TODO */
	} ipv4_hnat;

	struct {
		uint32_t sip;
		uint32_t dip;
		/* TODO */
	} ipv4_dslite;

	/* IPv6 */
	struct {
		uint32_t sip0;
		uint32_t sip1;
		uint32_t sip2;
		uint32_t sip3;
		
		uint32_t dip0;
		uint32_t dip1;
		uint32_t dip2;
		uint32_t dip3;
		
		uint16_t dport;
		uint16_t sport;
        uint16_t gre_call_id;
		uint8_t nexth;
		uint8_t is_udp;	
	} ipv6_routing;

	struct {
		uint8_t in_smac[6];
		uint8_t in_dmac[6];
	}l2_bridge;

	struct {
		/* TODO */
	} ipv6_6rd;

	uint16_t in_vlan;
	uint32_t hsk_l2;
	uint32_t ifc_idx;
	uint32_t sport_mask;
	uint32_t pkt_type;	/* entry format */
    uint8_t ppe2;
};



/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

#endif
