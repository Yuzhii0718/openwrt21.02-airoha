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
#ifndef _FE_H
#define _FE_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/


#define KERNEL_2_6_36 		(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,31))
    
#include <asm/tc3162/tc3162.h>
#include <linux/version.h>
#include "fe_reg_en7512.h"

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define fe_isr_name  "FE"


/************************************************************************
*               M A C R O S
*************************************************************************
*/

#define CHK_BUF() pos = begin + index; if (pos < off) { index = 0; begin = pos; }; if (pos > off + count) goto done;
#define CHK_BUF_READ_UNLOCK(lock) pos = begin + index; if (pos < off) { index = 0; begin = pos; }; if (pos > off + count) { read_unlock_bh(lock); goto done;};
#define CHK_BUF_WRITE_UNLOCK(lock) pos = begin + index; if (pos < off) { index = 0; begin = pos; }; if (pos > off + count) { write_unlock_irq(lock); goto done;};
#ifdef TCSUPPORT_CPU_ARMV8
#define read_reg_word(reg) 		get_frame_engine_data(reg)
#define write_reg_word(reg, wdata) 	set_frame_engine_data(reg, wdata)
#else
#define read_reg_word(reg) 		regRead32(reg)
#define write_reg_word(reg, wdata) 	regWrite32(reg, wdata)
#endif

#if defined(TCSUPPORT_XPON_HAL_API_EXT) || defined(TCSUPPORT_XPON_HAL_API)
#define WAN_ITF_NAME_PRE         "pon0."
#define WAN_ITF_NAME_PRE_LEN     (5)
#else
#define WAN_ITF_NAME_PRE         "nas"
#define WAN_ITF_NAME_PRE_LEN     (3)
#endif

#define CHANNEL_RETIRE 1
#define CHANNEL_DROP 0

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523) 
#define WAN_ITF_ACNT_NUM		(16)/*one itf need two ,uni and multi pkt counter, 8*2= 16*/
#define OLT_CTL_NUM         	(32)/*meter reserved for olt ctrl*/

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
#define ACNT0_DEFAULT           (0x3f)
#define ACNT1_DEFAULT           (0x1f)
#else
#define ACNT0_DEFAULT           (0x1f)
#define ACNT1_DEFAULT           (0x1f)
#endif
#define ACNT2_DEFAULT           (0x7f)

#define MULTICAST_ACNTGRP       (0x1e)

#define meter_qdmawan_mapping_path  "tc3162/meter_qdmawan_mapping"
#define meter_qdmalan_mapping_path  "tc3162/meter_qdmalan_mapping"
#else
#define WAN_ITF_ACNT_NUM		(8)
#define OLT_CTL_NUM         	(0)/*meter reserved for olt ctrl*/

#define GRP0_MAX_NUM            (0x3f)
#define GRP1_MAX_NUM            (0x20)
#define GRP2_MAX_NUM            (0x40)

#define ACNT0_DEFAULT           (0x3f)
#define ACNT1_DEFAULT           (0x0)
#define ACNT2_DEFAULT           (0x0)

#define MULTICAST_ACNTGRP       (0x3d)

#endif

#define FE_CNT_MINUTES 15
#define FE_CNT_SECONDS (FE_CNT_MINUTES * 60)
#define FE_CNT_THR (0xFF000000)
#define FE_BYTE_THR (0xF8000000)

#define ANI_UNI_PORT_MAX (5)    /* 0~3 for lan swith port, 4 for wan port*/

/* fragment and assemble force port config */
#if defined(TCSUPPORT_CPU_EN7581)
#define FRAG_FP_NBQ		3
#define ASSEM_FP_NBQ	22
#else
#define FRAG_FP_NBQ		1
#define ASSEM_FP_NBQ	15
#endif

/*
l2space unit is 16B,before 7581 l2space is 48B(register value is 3,3*16B)
after 7581 l2sapce is changed to 32B because pse info is extended to 32B
*/
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define L2SPACE_VAL	2
#else
#define L2SPACE_VAL	3
#endif


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
typedef enum {
	ACCOUNT_TYPE_WAN_INTERFACE = 0 ,
	ACCOUNT_TYPE_DEV_BANDWIDTH = 1
} FE_Account_Type_t;

typedef struct {
    char valid;
    char wanIndex;
    char is_vr_wan;
    struct net_device *wan_dev;
}Wan_itf_name;

typedef struct {
    FE_Account_Type_t account_type;
    int wan_itf_index;
    unsigned char mac[6];
    unsigned int upstream_total_byte_lo;
    unsigned int upstream_total_byte_hi;
    unsigned int upstream_total_packet;
    unsigned int downstream_total_byte_lo;
    unsigned int downstream_total_byte_hi;
    unsigned int downstream_total_packet;
    struct list_head total_account_node;
}wan_itf_or_dev_bandwidth_total_account_t;

typedef struct {
	int ip_type; /* 0 means ipv4, 1 means ipv6 */
	union {
		unsigned int ip4;
		struct in6_addr ip6;
	};
	struct list_head stb_src_ip_node;
}stb_src_ip_t;


/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern unsigned long flash_base;
extern int tls_enable;
extern int tls_debug_on;

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/
extern unsigned int (*ranand_read_byte)(unsigned long long);
extern void (*get_wan_index_info_hook)(char *wan_index);
int fe_add_dev_to_total_account(unsigned char *mac);

extern void fe_tls_entry_list_init(void);
extern void fe_tls_tag_vlan_list_init(void);
#endif /* _ETH_LAN_H */


