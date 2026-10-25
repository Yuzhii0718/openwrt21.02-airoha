/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _IFC_DEV_H_
#define _IFC_DEV_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include "ifc_glb.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define atoi(x)  simple_strtol(x, NULL,10)
#define CHK_BUF() pos = begin + index; if (pos < off) { index = 0; begin = pos; }; if (pos > off + count) goto done;

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
#define LAYER2_HEADER(skb)		skb_mac_header(skb)
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21)
#define LAYER2_HEADER(skb)		(skb)->mac_header
#else
#define LAYER2_HEADER(skb)		(skb)->mac.raw
#endif
#endif
        
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
#define LAYER3_HEADER(skb)		skb_network_header(skb)
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21)
#define LAYER3_HEADER(skb)		(skb)->network_header
#else
#define LAYER3_HEADER(skb)		(skb)->nh.raw
#endif
#endif
        
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
#define LAYER4_HEADER(skb)		skb_transport_header(skb)
#else
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,21)
#define LAYER4_HEADER(skb)		(skb)->transport_header
#else
#define LAYER4_HEADER(skb)		(skb)->h.raw
#endif
#endif

#define IFC_MAJOR 241
#define IFC_MAJOR_NAME "/dev/ifc"


/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
#if defined(TCSUPPORT_CPU_EN7581)
	typedef union {
		struct {
		#ifdef __BIG_ENDIAN
			uint32		:1;
			uint32 no_mic	:1;
			uint32 gem	:16;
			uint32 longf	:1;
			uint32 runtf	:1;
			uint32 crcer	:1;
			uint32		:1;
			uint32 ptp	:1;
			uint32 oam	:1;
			uint32 channel	:5;
			uint32 hopflag	:3;
		#else
			uint32 hopflag	:3;
			uint32 channel	:5;
			uint32 oam	:1;
			uint32 ptp	:1;
			uint32		:1;
			uint32 crcer	:1;
			uint32 runtf	:1;
			uint32 longf	:1;
			uint32 gem	:16;
			uint32 no_mic	:1;
			uint32		:1;
		#endif
	
		#ifdef __BIG_ENDIAN
			uint32 dei		:1;
			uint32 ip6		:1;
			uint32 ip4		:1;
			uint32 ip4f 		:1;
			uint32 l4vld		:1;
			uint32 l4f		:1;
			uint32 sport		:5;
			uint32 crsn 		:5;
			uint32 ppe_entry	:16;
		#else
			uint32 ppe_entry	:16;
			uint32 crsn 		:5;
			uint32 sport		:5;
			uint32 l4f		:1;
			uint32 l4vld		:1;
			uint32 ip4f 		:1;
			uint32 ip4		:1;
			uint32 ip6		:1;
			uint32 dei		:1;
		#endif
	
		#ifdef __BIG_ENDIAN
			uint32		:16;
			uint32 ifc_fid	:8;
			uint32 ifc_hit	:1;
			uint32 l2_len	:7;
		#else
			uint32 l2_len	:7;
			uint32 ifc_hit	:1;
			uint32 ifc_fid	:8;
			uint32		:16;
		#endif
	
			uint32	resv;
		} ;
		uint32 word[4] ;
	} FERxMsg_T ;

#elif defined(TCSUPPORT_CPU_EN7523)
typedef union {
	struct {
		#ifdef __BIG_ENDIAN
		uint32			:1;
		uint32 no_mic		:1;
		uint32 gem		:16;
		uint32 longf		:1;
		uint32 runtf		:1;
		uint32 crcer		:1;
		uint32			:2;
		uint32 oam		:1;
		uint32 channel		:5;
		uint32			:3;
		#else
		uint32			:3;
		uint32 channel		:5;
		uint32 oam		:1;
		uint32			:2;
		uint32 crcer		:1;
		uint32 runtf		:1;
		uint32 longf		:1;
		uint32 gem		:16;
		uint32 no_mic		:1;
		uint32			:1;
		#endif

		#ifdef __BIG_ENDIAN
		uint32			:1;
		uint32 dei		:1;
		uint32 ip6		:1;
		uint32 ip4		:1;
		uint32 ip4f 		:1;
		uint32 l4vld		:1;
		uint32 l4f		:1;
		uint32 sport		:5;
		uint32 crsn 		:5;
		uint32 ppe_entry	:15;
		#else
		uint32 ppe_entry	:15;
		uint32 crsn 		:5;
		uint32 sport		:5;
		uint32 l4f		:1;
		uint32 l4vld		:1;
		uint32 ip4f 		:1;
		uint32 ip4		:1;
		uint32 ip6		:1;
		uint32 dei		:1;
		uint32			:1;
		#endif

		#ifdef __BIG_ENDIAN
		uint32			:16;
		uint32 ifc_fid		:8;
		uint32 ifc_hit		:1;
		uint32 l2_len		:7;
		#else
		uint32 l2_len		:7;
		uint32 ifc_hit		:1;
		uint32 ifc_fid		:8;
		uint32			:16;
		#endif

		uint32  resv;
	} ;
	uint32 word[4] ;
} FERxMsg_T ;

#elif defined(TCSUPPORT_CPU_EN7580)
typedef union {
	struct {
#ifdef __BIG_ENDIAN
		uint 					: 8 ;
		uint gem				: 12 ;
		uint oam				: 1 ;
		uint channel			: 8 ;
		uint longf				: 1 ;
		uint runtf				: 1 ;
		uint crcer				: 1 ;
#else
		uint crcer				: 1 ;
		uint runtf				: 1 ;
		uint longf				: 1 ;
		uint channel			: 8 ;
		uint oam				: 1 ;
		uint gem				: 12 ;
		uint 					: 8 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint 					: 1 ;
		uint l2b				: 1 ;
		uint ipv6				: 1 ;
		uint ipv4				: 1 ;
		uint ipv4f				: 1 ;
		uint l4vld				: 1 ;
		uint l4f				: 1 ;
		uint sport				: 5 ;
		uint crsn				: 5 ;
		uint 					: 1 ;
		uint ifc_fid			: 8 ;
		uint ifc_hit			: 1 ;
		uint 					: 5 ;
#else
		uint 					: 5 ;
		uint ifc_hit			: 1 ;
		uint ifc_fid			: 8 ;
		uint 					: 1 ;
		uint crsn				: 5 ;
		uint sport				: 5 ;
		uint l4f				: 1 ;
		uint l4vld				: 1 ;
		uint ipv4f				: 1 ;
		uint ipv4				: 1 ;
		uint ipv6				: 1 ;
		uint l2b				: 1 ;
		uint 					: 1 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint 					: 13 ;
		uint vlan				: 1 ;
		uint tpid				: 2 ;
		uint vid				: 16 ;
#else
		uint vid				: 16 ;
		uint tpid				: 2 ;
		uint vlan				: 1 ;
		uint 					: 13 ;
#endif /* __BIG_ENDIAN */

		uint timestamp ;
	} ;
	uint word[4] ;
} FERxMsg_T ;

#elif defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)

typedef union {
	struct {
#ifdef __BIG_ENDIAN
		uint 					: 8 ;
		uint gem				: 12 ;
		uint oam				: 1 ;
		uint channel			: 8 ;
		uint longf				: 1 ;
		uint runtf				: 1 ;
		uint crcer				: 1 ;
#else
		uint crcer				: 1 ;
		uint runtf				: 1 ;
		uint longf				: 1 ;
		uint channel			: 8 ;
		uint oam				: 1 ;
		uint gem				: 12 ;
		uint 					: 8 ;
#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint 					: 3 ;
		//uint l2b				: 1 ;
		uint ipv6				: 1 ;
		uint ipv4				: 1 ;
		uint ipv4f				: 1 ;
		uint tack				: 1 ;
		uint l4vld				: 1 ;
		uint l4f				: 1 ;
		uint sport				: 4 ; //5->4
		uint crsn				: 5 ;
		//uint 					: 1 ;
		uint ifc_fid			: 8 ;
		uint ifc_hit			: 1 ;
		uint pppoe				: 1 ;
		uint snap				: 1 ;
		uint vln				: 3 ;
#else
		uint vln				: 3 ;
		uint snap				: 1 ;
		uint pppoe				: 1 ;
		uint ifc_hit			: 1 ;
		uint ifc_fid			: 8 ;
		//uint					: 1 ;
		uint crsn				: 5 ;
		uint sport				: 4 ; //5->4
		uint l4f				: 1 ;
		uint l4vld				: 1 ;
		uint tack				: 1 ;
		uint ipv4f				: 1 ;
		uint ipv4				: 1 ;
		uint ipv6				: 1 ;
		//uint l2b				: 1 ;
		uint					: 3 ;

#endif /* __BIG_ENDIAN */

#ifdef __BIG_ENDIAN
		uint 					: 13 ;
		uint vlan				: 1 ;
		uint tpid				: 2 ;
		uint vid				: 16 ;
#else
		uint vid				: 16 ;
		uint tpid				: 2 ;
		uint vlan				: 1 ;
		uint 					: 13 ;
#endif /* __BIG_ENDIAN */

		uint timestamp ;
	} ;
	uint word[4] ;
} FERxMsg_T ;
#endif

typedef struct {
	//enable bit
	uint8_t snap_en;		//valid
	uint8_t vlan_layer;		//valid
	uint8_t pppoe_en;		//valid
	uint8_t ip_type;		//valid
	uint8_t tcp_udp_type;		//valid
	uint8_t fix_seed;
	
	//layer2 header
	uint8_t dmac[6];	//valid
	uint8_t smac[6];	//valid
	uint16_t eth_type;	//valid
	
	//vlan header 
	uint16_t vlan1_tag;	
	uint16_t vlan1;
	uint16_t vlan2_tag;	
	uint16_t vlan2;
	//7516 ifc support 4 vlan layers
	uint16_t vlan3_tag;	
	uint16_t vlan3;
	uint16_t vlan4_tag;	
	uint16_t vlan4;

	//pppoe header
	uint16_t ppp_tag;		//valid
	uint16_t pppoe_sid;		//valid
	
	//layer3 header
	struct iphdr iph;	//valid
	struct ipv6hdr ip6h;	//valid
	
	//layer4 header
	struct tcphdr th;	//valid
	struct udphdr uh;	//valid
	
	struct sk_buff *skb;
} pktInfo;

typedef struct {
	//layer2 header
	uint8_t dmac[6];	//valid
	uint8_t smac[6];	//valid
	uint16_t eth_type;	//valid
	uint16_t dmac_type;	//valid	need to parser
	
	//vlan header 
	uint16_t vlan1_gap;
	uint16_t vlan1_tag;	
	uint16_t vlan1;
	uint16_t vlan2_gap;
	uint16_t vlan2_tag;	
	uint16_t vlan2;
	//7516 ifc support 4 vlan layers
	uint16_t vlan3_gap;
	uint16_t vlan3_tag;	
	uint16_t vlan3;
	uint16_t vlan4_gap;
	uint16_t vlan4_tag;	
	uint16_t vlan4;
	uint16_t vlan_layer;	//valid
	
	//pppoe header
	uint32_t pppoe_gap;
	uint16_t ppp_tag;		//valid
	uint16_t pppoe_sid;		//valid
	
	//layer3 header
	struct iphdr iph;	//valid
	struct ipv6hdr ip6h;	//valid
	
	//layer4 header
	struct tcphdr th;	//valid
	struct udphdr uh;	//valid
	
	//enable bit
	uint8_t pppoe_en;		//valid
	uint8_t iph_en;			//valid
	uint8_t ip6h_en;		//valid
	uint8_t tcp_udp_en;		//valid
	
	//L2/L3/L4 offset
	uint8_t l2_offset;		//valid
	uint8_t l3_offset;		//valid
	uint8_t l4_offset;		//valid
	
	//uint32_t pkt_type;
	uint16_t sinf; 		//valid	from rx msg
	uint16_t gem_port_id;	//valid	from rx msg
	uint16_t frame_type;	//valid	need to parser
} IfcPktParseResult;


/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int get_ifc_index(struct sk_buff *skb, FERxMsg_T *rxMsgPtr);

#endif /* _IFC_DEV_H_ */
