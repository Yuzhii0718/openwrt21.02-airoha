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

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <asm/io.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include <modules/ptp/ptp.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
void __iomem *ecnt_gsw_ptp_base = NULL;
void (*ecnt_gsw_ptp_isr_func)(uint32_t int_status);
void (*ecnt_xfi_ptp_isr_func)(uint32_t int_status);
void (*ecnt_xfi_pon_ptp_isr_func)(uint32_t int_status);

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define RG_CKGCR 0x1FB5B0F0

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct ecnt_ptp {
	struct device  *dev;
	void __iomem *gsw_ptp_base;
	void __iomem *xfi_ptp_base;
	void __iomem *xfi_pon_ptp_base;
	u32          irq[3];
};

//0x00
typedef union
{
	struct
	{
		unsigned int ptp_rx_en 								: 1;
		unsigned int ptp_tx_en 								: 1;
		unsigned int sw_rst_n  								: 1; //no such bit in XFI_PTP
		unsigned int add_4byte0								: 1; //no such bit in XFI_PTP
		unsigned int gen_sync								: 1;
		unsigned int gen_delay_req							: 1;
		unsigned int gen_pdelay_req							: 1;
		unsigned int gen_announce							: 1;
		unsigned int gen_follow_up							: 1;
		unsigned int ts_insert								: 1;
		unsigned int sync_rx_filter_v1						: 1;
		unsigned int sync_rx_filter_v2						: 1;
		unsigned int sync_rx_filter_ether					: 1;
		unsigned int sync_rx_filter_ipv4					: 1;
		unsigned int sync_rx_filter_ipv6					: 1;
		unsigned int sync_rx_filter_multi					: 1;
		unsigned int sync_rx_filter_uni						: 1;
		unsigned int ptp_rx_ts_comp_p_m						: 1;
		unsigned int ptp_tx_ts_comp_p_m						: 1;
		unsigned int load_sync_follow_up_delay_req_seqid	: 1;
		unsigned int load_pdelay_req_seqid					: 1;
		unsigned int load_announce_seqid					: 1;
		unsigned int resv2									: 10;
	} bits;
	unsigned int word;
} ptp_glb_cfg_t;

//0x04
typedef union
{
	struct
	{
		unsigned int sync_rx_rate_limit_set_en		: 1;
		unsigned int sync_rx_rate_limit				: 23;
		unsigned int sync_rx_bucket_size			: 8;
	} bits;
	unsigned int word;
} sync_rx_rate_limit_set_t;

//0x08
typedef union
{
	struct
	{
		unsigned int rx_drop_pkt             : 1;
		unsigned int tx_to_sw_info           : 1;
		unsigned int tx_to_sw_info_cnt_full  : 1;
		unsigned int tx_auto_gen_sync        : 1;
		unsigned int tx_auto_gen_delay_req   : 1;
		unsigned int tx_auto_gen_pdelay_req  : 1;
		unsigned int tx_auto_gen_announce    : 1;
		unsigned int tx_auto_gen_follow_up   : 1;
		unsigned int resv                    : 24;
	} bits;
	unsigned int word;
} ptp_int_sts_t;

//0x0c
typedef union
{
	struct
	{
		unsigned int rx_drop_pkt_int_en          	: 1;
		unsigned int tx_to_sw_info_int_en           : 1;
		unsigned int tx_to_sw_info_cnt_full_int_en  : 1;
		unsigned int tx_auto_gen_sync_int_en        : 1;
		unsigned int tx_auto_gen_delay_req_int_en   : 1;
		unsigned int tx_auto_gen_pdelay_req_int_en  : 1;
		unsigned int tx_auto_gen_announce_int_en    : 1;
		unsigned int tx_auto_gen_follow_up_int_en   : 1;
		unsigned int resv                     		: 24;
	} bits;
	unsigned int word;
} ptp_int_en_t;

//0x4c
typedef union
{
	struct
	{
		unsigned int sync_delay_req_seqid			: 16;
		unsigned int pdelay_req_seqid				: 16;
	} bits;
	unsigned int word;
} ptp_pkt_seqid_set0_t;

//0x50
typedef union
{
	struct
	{
		unsigned int anounce_follow_up_seqid		: 16;
		unsigned int resv 							: 16;
	} bits;
	unsigned int word;
} ptp_pkt_seqid_set1_t;

//0x54
typedef union
{
	struct
	{
		unsigned int sync_delay_req_ptp_header_offset	: 8;
		unsigned int pdelay_req_ptp_header_offset		: 8;
		unsigned int announce_ptp_header_offset			: 8;
		unsigned int follow_up_ptp_header_offset		: 8;
	} bits;
	unsigned int word;
} ptp_autogen_header_offset_t;

//0x58
typedef union
{
	struct
	{
		unsigned int sync_delay_req_len	: 8;
		unsigned int pdelay_req_len		: 8;
		unsigned int announce_len		: 8;
		unsigned int follow_up_len		: 8;
	} bits;
	unsigned int word;
} ptp_autogen_length_t;

//0x5c
typedef union
{
	struct
	{
		unsigned int ptp_msg_type	: 2; //sync, delay_req, and so on
		unsigned int ptp_pkt_type	: 2; //ethernet, over IPv4, and so on
		unsigned int res			: 28;
	} bits;
	unsigned int word;
} ptp_autogen_type_set_t;

//0x60
typedef union
{
	struct
	{
		unsigned int ptp_pkt_wr		: 1;
		unsigned int ptp_pkt_rd		: 1;
		unsigned int res			: 6;
		unsigned int ptp_pkt_offset	: 8;
		unsigned int res1			: 16;
	} bits;
	unsigned int word;
} ptp_autogen_pkt_set_t;


/* [XFI PTP] */

//0x0000
typedef union
{
	struct{
		unsigned int txmbi_stop              : 1;
		unsigned int txmpi_stop              : 1;
		unsigned int rxmbi_stop              : 1;
		unsigned int rxmpi_stop              : 1;
		unsigned int rx_fc_en                : 1;
		unsigned int tx_fc_en                : 1;
		unsigned int rx_pause_lenchk         : 1;
		unsigned int tx_use_rxfault_realtime : 1;
		unsigned int fcs_err_fwd             : 1;
		unsigned int txmpi_mask              : 1;
		unsigned int ipg_num                 : 6; //bit 10 ~ bit 15
		unsigned int xgmii_lpbk_en_when_tdc  : 1;
		unsigned int tx_frag_len             : 5;
		unsigned int rx_frag_len             : 5;
		unsigned int mem_power_dowm          : 1;
		unsigned int tx_send_remote_fault_en : 1;
		unsigned int rx_local_fault_chk_en   : 1;
		unsigned int rx_remote_fault_chk_en  : 1;
		unsigned int resv                    : 1;
	} bits;
	unsigned int word;
} xfi_glb_cfg_t;

//0x0550
typedef union
{
	struct
	{
		unsigned int ptp_pkt_wr		: 1;
		unsigned int ptp_pkt_rd		: 1;
		unsigned int ptp_msg_type	: 2; //sync, delay_req, and so on
		unsigned int ptp_pkt_type	: 2; //ethernet, over IPv4, and so on
		unsigned int vlan_num       : 2;
		unsigned int ptp_pkt_offset : 8;
		unsigned int resv			: 16;
	} bits;
	unsigned int word;
} xfi_ptp_autogen_pkt_set_t;


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
struct ecnt_ptp *ecnt_ptp = NULL;
static const struct of_device_id ecnt_ptp_of_id[] = {
    { .compatible = "econet,ecnt-1588v2ptp"},
    {},
};
MODULE_DEVICE_TABLE(of, ecnt_ptp_of_id);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern u32 get_gdma_special_fp(uint8_t pdev_id);
extern void set_gdma_special_fp(uint8_t pdev_id, u32 val);
extern u32 GET_WAN_CONF(void);
extern u32 GET_SSR3(void);
extern u32 GET_NP_SCU_SSTR(void);
extern void SET_WAN_CONF(u32 val);
extern void SET_SSR3(u32 val);
extern void SET_NP_SCU_SSTR(u32 val);
void get_gsw_ptp_base(void __iomem **ecnt_gsw_ptp_base);
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) || defined(TCSUPPORT_HSGMII_LAN)
extern void set_xsi_data(u32 reg, u32 val);
extern u32 get_xsi_data(u32 reg);
#endif

/************************************************************************
*                  S T A T I C   F U N C T I O N
*************************************************************************
*/
/* don't EXPORT this function. Create API for your purpose instead. */
static u32 get_ptp_data(uint8_t pdev_id, u32 reg)
{
	if(pdev_id == PDEV_GSW_PTP_ID) {
		return readl(ecnt_ptp->gsw_ptp_base + reg);
	} else if (pdev_id == PDEV_XFI_PTP_ID) {
		if (reg >= XFI_PTP_GLB_CFG) {
			return readl(ecnt_ptp->xfi_ptp_base + (reg - XFI_MAC_PTP_ADDR_BASE));
		} else {
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) || defined(TCSUPPORT_HSGMII_LAN)
			return get_xsi_data(reg);
#else
			printk("\n[Error] Do not support XFI MAC\n");
			return 0x0;
#endif
		}
	} else if (pdev_id == PDEV_XFI_PON_PTP_ID) {
		if (reg >= XFI_PTP_GLB_CFG) {
			return readl(ecnt_ptp->xfi_pon_ptp_base + (reg - XFI_MAC_PTP_ADDR_BASE));
		} else {
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) || defined(TCSUPPORT_HSGMII_LAN)
			return get_xsi_data(reg-XFI_PON_MAC_ADDR_OFFSET);
#else
			printk("\n[Error] Do not support XFI MAC\n");
			return 0x0;
#endif
		}
	} else {
		printk("\n[Error] incorrect pdev_id! (%s)\n", __func__);
		return -1;
	}
}

/* don't EXPORT this function. Create API for your purpose instead. */
static void set_ptp_data(uint8_t pdev_id, u32 reg, u32 val)
{
	if (pdev_id == PDEV_GSW_PTP_ID) {
		writel(val, ecnt_ptp->gsw_ptp_base + reg);
	} else if (pdev_id == PDEV_XFI_PTP_ID) {
		if (reg >= XFI_PTP_GLB_CFG) {
			writel(val, ecnt_ptp->xfi_ptp_base + (reg - XFI_MAC_PTP_ADDR_BASE));
		} else {
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) || defined(TCSUPPORT_HSGMII_LAN)
			set_xsi_data(reg, val);
#else
			printk("\n[Error] Do not support XFI MAC\n");
#endif
		}
	} else if(pdev_id == PDEV_XFI_PON_PTP_ID) {
		if (reg >= XFI_PTP_GLB_CFG) {
			writel(val, ecnt_ptp->xfi_pon_ptp_base + (reg - XFI_MAC_PTP_ADDR_BASE));
		} else {
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN) || defined(TCSUPPORT_HSGMII_LAN)
			set_xsi_data(reg-XFI_PON_MAC_ADDR_OFFSET, val);
#else
			printk("\n[Error] Do not support XFI MAC\n");
#endif
		}
	} else {
		printk("\n[Error] incorrect pdev_id!  (%s)\n", __func__);
	}
}

void configure_ptp_sptag(ptp_sptag_cfg_t ptp_sptag_cfg)
{
	set_ptp_data(PDEV_GSW_PTP_ID, PTP_SPTAG_CFG, ptp_sptag_cfg.word);
}
EXPORT_SYMBOL(configure_ptp_sptag);

void set_packet_filter(uint8_t pdev_id, uint8_t onoff, uint8_t type)
{
	uint32_t val, reg[3] = {PTP_GLB_CFG, XFI_PTP_GLB_CFG, XFI_PTP_GLB_CFG};

	val = get_ptp_data(pdev_id, reg[pdev_id]);

	if(onoff == ENABLE)
	{
		val |= (1 << type);
	}else if(onoff == DISABLE)
	{
		val &= ~(1 << type);
	}else
	{
		printk("\n[Error] incorrect onoff mode!  (%s)\n", __func__);
	}

	set_ptp_data(pdev_id, reg[pdev_id], val);
}
EXPORT_SYMBOL(set_packet_filter);

unsigned int ecnt_get_ptp_data(uint8_t pdev_id, u32 reg)
{
	switch(reg)
	{
		case PTP_GLB_CFG:                 //0x0000
		case SYNC_RX_RATE_LIMIT_SET:      //0x0004
		case PTP_INT_STS:
		case PTP_INT_EN:
		case PTP_RX_TS_HIGH_16:
		case PTP_SPTAG_CFG:
		case PTP_TX_TO_SW_TYPE_SEQID:
		case PTP_RX_NOT_PTP_CNT:
		case PTP_RX_PTP_CNT:
		case PTP_RXMBI_CNT:
		case PTP_RX_SYNC_DROP_CNT:
		case PTP_TX_MBI_CNT:              //0x00c8
			return get_ptp_data(pdev_id, reg);
		case XFI_PTP_GLB_CFG:             //0x0500
		case XFI_SYNC_RX_RATE_LIMIT_SET:  //0x0504
		case XFI_PTP_INT_STS:
		case XFI_PTP_INT_EN:
		case XFI_PTP_RX_TS_HIGH_16:
		case XFI_PTP_TX_TO_SW_TYPE_SEQID:
		case XFI_PTP_RXMBI_CNT:
		case XFI_PTP_TX_MBI_CNT:
		case XFI_RX_PARSER_DROP_NUM:      //0x062c
			return get_ptp_data(pdev_id, reg);
		default:
			printk("\n[Error] Not support to read this register! (%s)\n", __func__);
			break;
	}
	return -1;
}
EXPORT_SYMBOL(ecnt_get_ptp_data);

void xfi_count_clear(void)
{
	set_ptp_data(PDEV_XFI_PTP_ID, XFI_CNT_CLR, 0x1);
}
EXPORT_SYMBOL(xfi_count_clear);

void set_tstamp_comp(uint8_t pdev_id, u32 reg, u32 val)
{
	switch(reg)
	{
		case PTP_TX_TS_COMP:      //0x0038
		case PTP_RX_TS_COMP:      //0x0034
			set_ptp_data(pdev_id, reg, val);
			break;
		case XFI_PTP_TX_TS_COMP:  //0x052c
		case XFI_PTP_RX_TS_COMP:  //0x0528
			set_ptp_data(pdev_id, reg, val);
			break;
		default:
			printk("\n[Error] Not support to set this register! (%s)\n", __func__);
			break;
	}
}
EXPORT_SYMBOL(set_tstamp_comp);

void get_tx_feedback_sw_info(uint8_t pdev_id, ptp_tx_feedback_sw_info_t *sw_info)
{
	u32 reg_swInfo[3]  = {PTP_TX_TO_SW_INFO_RD, XFI_PTP_TX_TO_SW_INFO_RD, XFI_PTP_TX_TO_SW_INFO_RD};
	u32 reg_swTsH[3]   = {PTP_TX_TO_SW_TS_H, XFI_PTP_TX_TO_SW_TS_H, XFI_PTP_TX_TO_SW_TS_H};
	u32 reg_swTsL[3]   = {PTP_TX_TO_SW_TS_L, XFI_PTP_TX_TO_SW_TS_L, XFI_PTP_TX_TO_SW_TS_L};
	u32 reg_swSeqId[3] = {PTP_TX_TO_SW_TYPE_SEQID, XFI_PTP_TX_TO_SW_TYPE_SEQID, XFI_PTP_TX_TO_SW_TYPE_SEQID};

	set_ptp_data(pdev_id, reg_swInfo[pdev_id], 0x1);
	sw_info->tx_timestamp_byte_7_4 = get_ptp_data(pdev_id, reg_swTsH[pdev_id]);
	sw_info->tx_timestamp_byte_3_0 = get_ptp_data(pdev_id, reg_swTsL[pdev_id]);
	sw_info->tx_seqId_msgType_infoCnt = get_ptp_data(pdev_id, reg_swSeqId[pdev_id]);
}
EXPORT_SYMBOL(get_tx_feedback_sw_info);

/* Interrupt Service Routine */
static irqreturn_t gsw_ptp_interrupt_handler(int irq, void *dev_instance)
{
	ptp_int_sts_t ptp_int_sts;

	ptp_int_sts.word = get_ptp_data(PDEV_GSW_PTP_ID, PTP_INT_STS);

	if(ecnt_gsw_ptp_isr_func != NULL)
		ecnt_gsw_ptp_isr_func(ptp_int_sts.word);

	set_ptp_data(PDEV_GSW_PTP_ID, PTP_INT_STS, ptp_int_sts.word);

	return IRQ_HANDLED;
}

static irqreturn_t xfi_ptp_interrupt_handler(int irq, void *dev_instance)
{
	ptp_int_sts_t ptp_int_sts;

	ptp_int_sts.word = get_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_INT_STS);

	if(ecnt_xfi_ptp_isr_func != NULL)
		ecnt_xfi_ptp_isr_func(ptp_int_sts.word);

	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_INT_STS, ptp_int_sts.word);

	return IRQ_HANDLED;
}

static irqreturn_t xfi_pon_ptp_interrupt_handler(int irq, void *dev_instance)
{
	ptp_int_sts_t ptp_int_sts;

	ptp_int_sts.word = get_ptp_data(PDEV_XFI_PON_PTP_ID, XFI_PTP_INT_STS);

	if(ecnt_xfi_pon_ptp_isr_func != NULL)
		ecnt_xfi_pon_ptp_isr_func(ptp_int_sts.word);

	set_ptp_data(PDEV_XFI_PON_PTP_ID, XFI_PTP_INT_STS, ptp_int_sts.word);

	return IRQ_HANDLED;
}

void ptp_interrupt_init(uint8_t pdev_id, void (*isr_func)(uint32_t status))
{
	if(isr_func != NULL)
	{
		if(pdev_id == PDEV_GSW_PTP_ID)
		{
			ecnt_gsw_ptp_isr_func = isr_func;
		}else if(pdev_id == PDEV_XFI_PTP_ID)
		{
			ecnt_xfi_ptp_isr_func = isr_func;
		}else if(pdev_id == PDEV_XFI_PON_PTP_ID)
		{
			ecnt_xfi_pon_ptp_isr_func = isr_func;
		}else
		{
			printk("\n[Error] incorrect pdev_id! (%s)\n", __func__);
		}
	}
}
EXPORT_SYMBOL(ptp_interrupt_init);

void set_ptp_interrupt(uint8_t pdev_id, uint8_t onoff, uint8_t bit)
{
	ptp_int_en_t ptp_int_en;
	u32 reg[3] = {PTP_INT_EN, XFI_PTP_INT_EN, XFI_PTP_INT_EN};

	ptp_int_en.word = get_ptp_data(pdev_id, reg[pdev_id]);

	if(onoff == ENABLE)
	{
		ptp_int_en.word |= (1 << bit);
	}else if(onoff == DISABLE)
	{
		ptp_int_en.word &= (~(1 << bit));
	}else
	{
		printk("\n[Error] Not support on this onoff mode! (%s)\n", __func__);
	}

	set_ptp_data(pdev_id, reg[pdev_id], ptp_int_en.word);
}
EXPORT_SYMBOL(set_ptp_interrupt);


void ptp_run(uint8_t pdev_id, uint8_t onoff, uint8_t xfi_lpbk_mode)
{
	u32 val, val_xfipon[3], val_ckg;
	ptp_glb_cfg_t ptp_glb_cfg;
	xfi_glb_cfg_t xfi_glb_cfg;
	u32 reg[3] = {PTP_GLB_CFG, XFI_PTP_GLB_CFG, XFI_PTP_GLB_CFG};

	val = get_gdma_special_fp(pdev_id);
#if 0 // FPGA Verification for XFI PON MAC
	val_xfipon[0] = GET_WAN_CONF();
	val_xfipon[1] = GET_SSR3();
	val_xfipon[2] = GET_NP_SCU_SSTR();
#endif

	memset(&ptp_glb_cfg.word, 0, sizeof(ptp_glb_cfg_t));

	if(onoff == ENABLE)
	{
		if (pdev_id == PDEV_GSW_PTP_ID) {
			/* while port link down, need to set to trigger clock. */
			val_ckg = get_frame_engine_data(RG_CKGCR);
			val_ckg &= (~(1<<1));
			set_frame_engine_data(RG_CKGCR, val_ckg);
		}
		set_ptp_data(pdev_id, reg[pdev_id], ptp_glb_cfg.word);
		ptp_glb_cfg.bits.ptp_rx_en = 0b1;
		ptp_glb_cfg.bits.ptp_tx_en = 0b1;
		ptp_glb_cfg.bits.sw_rst_n  = 0b1;
		set_ptp_data(pdev_id, reg[pdev_id], ptp_glb_cfg.word);

		/* Set up frame engine PTP port and unblock queue */
		// Based on GageChen's opinion, write 0 to send packet flowing into qdma rx ring 0
		// GDM_SPECIAL_FP: [Bit25:16] GDM_RXPTP_FP
		// According to DP's suggestion, ptp packet should forward into rx ring 2.
		set_gdma_special_fp(pdev_id, (1<<17));

		/* Set up loopback mode */
		if((pdev_id == PDEV_XFI_PTP_ID || pdev_id == PDEV_XFI_PON_PTP_ID) &&
														xfi_lpbk_mode == XFI_PTP_LPBK_ENABLE)
		{
			xfi_glb_cfg.word = get_ptp_data(pdev_id, XFI_GLB_CFG);
			xfi_glb_cfg.bits.xgmii_lpbk_en_when_tdc = 1;
			printk("\nxfi_glb_cfg.word: 0x%08x\n", xfi_glb_cfg.word);
			set_ptp_data(pdev_id, XFI_GLB_CFG, xfi_glb_cfg.word);
		}else if((pdev_id == PDEV_XFI_PTP_ID || pdev_id == PDEV_XFI_PON_PTP_ID) &&
														xfi_lpbk_mode == XFI_PTP_LPBK_DISABLE)
		{
			xfi_glb_cfg.word = get_ptp_data(pdev_id, XFI_GLB_CFG);
			xfi_glb_cfg.bits.xgmii_lpbk_en_when_tdc = 0;
			printk("\nxfi_glb_cfg.word: 0x%08x\n", xfi_glb_cfg.word);
			set_ptp_data(pdev_id, XFI_GLB_CFG, xfi_glb_cfg.word);
		}

#if 0 // FPGA Verification for XFI PON MAC
		if(pdev_id == PDEV_XFI_PON_PTP_ID)
		{
			//Configuration from Sunny for XFIMAC_PON
			SET_WAN_CONF(0xe0000011);
			SET_SSR3(0xe0004820);
			SET_NP_SCU_SSTR(0x00005509);

			printk("\nCR_NP_WAN_CONF : 0x%08x\n", GET_WAN_CONF());
			printk("\nCR_NP_SCU_SSR3 : 0x%08x\n", GET_SSR3());
			printk("\nCR_NP_SCU_SSTR : 0x%08x\n", GET_NP_SCU_SSTR());
		}
#endif
	}else if(onoff == DISABLE)
	{
		set_ptp_data(pdev_id, reg[pdev_id], ptp_glb_cfg.word);
		set_gdma_special_fp(pdev_id, val);

#if 0 // FPGA Verification for XFI PON MAC
		if(pdev_id == PDEV_XFI_PON_PTP_ID)
		{
			//restore configuration from Sunny for XFIMAC_PON
			SET_WAN_CONF(val_xfipon[0]);
			SET_SSR3(val_xfipon[1]);
			SET_NP_SCU_SSTR(val_xfipon[2]);
		}
#endif
	}else
	{
		printk("\n[Error] Not support on this onoff mode! \n");
	}
}
EXPORT_SYMBOL(ptp_run);

void show_gsw_ptp_register(void)
{
	printk("ptp_glb_cfg                 : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_GLB_CFG));
	printk("sync_rx_rate_limit_set      : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, SYNC_RX_RATE_LIMIT_SET));

	printk("\n");
	printk("ptp_int_sts                 : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_INT_STS));
	printk("ptp_int_en                  : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_INT_EN));

	printk("\n");
	printk("vlan_tag_set                : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, VLAN_TAG_SET));
	printk("ptp_rx_ts_high_16           : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RX_TS_HIGH_16));
	printk("ptp_tx_to_sw_info_rd        : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_TO_SW_INFO_RD));
	printk("ptp_tx_to_sw_ts_h           : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_TO_SW_TS_H));
	printk("ptp_tx_to_sw_ts_l           : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_TO_SW_TS_L));
	printk("ptp_tx_to_sw_type_seqid     : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_TO_SW_TYPE_SEQID));
	printk("ptp_rx_ts_comp              : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RX_TS_COMP));
	printk("ptp_tx_ts_comp              : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_TS_COMP));

	printk("\n");
	printk("ptp_tx_sync_follow_interval : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_SYNC_FOLLOW_INTERVAL));
	printk("sync_delay_req_tx_freq      : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, SYNC_DELAY_REQ_TX_FREQ));
	printk("pdelay_req_tx_freq          : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PDELAY_REQ_TX_FREQ));
	printk("announce_tx_freq            : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, ANNOUNCE_TX_FREQ));

	printk("\n");
	printk("ptp_pkt_seqid_set0          : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_PKT_SEQID_SET0));
	printk("ptp_pkt_seqid_set1          : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_PKT_SEQID_SET1));
	printk("ptp_autogen_header_offset   : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_HEADER_OFFSET));
	printk("ptp_autogen_length          : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_LENGTH));
	printk("ptp_autogen_type_set        : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_TYPE_SET));
	printk("ptp_autogen_pkt_set         : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_PKT_SET));
	printk("ptp_autogen_pkt_data        : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_PKT_DATA));

	printk("\n");
	printk("ptp_rx_not_ptp_cnt          : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RX_NOT_PTP_CNT));
	printk("ptp_rx_ptp_cnt              : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RX_PTP_CNT));
	printk("ptp_rx_ptp_byte_cnt         : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RX_PTP_BYTE_CNT));
	printk("ptp_rxmbi_cnt               : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RXMBI_CNT));
	printk("ptp_rxmbi_byte_cnt          : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RXMBI_BYTE_CNT));
	printk("ptp_rx_sync_drop_cnt        : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RX_SYNC_DROP_CNT));
	printk("ptp_rx_drop_cnt             : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_RX_DROP_CNT));

	printk("\n");
	printk("ptp_tx_cnt                  : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_CNT));
	printk("ptp_tx_autog_sync_cnt       : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_AUTOG_SYNC_CNT));
	printk("ptp_tx_autog_delay_req_cnt  : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_AUTOG_DELAY_REQ_CNT));
	printk("ptp_tx_autog_pdelay_req_cnt : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_AUTOG_PDELAY_REQ_CNT));
	printk("ptp_tx_autog_announce_cnt   : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_AUTOG_ANNOUNCE_CNT));
	printk("ptp_tx_autog_follow_up_cnt  : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_AUTOG_FOLLOW_UP_CNT));
	printk("ptp_tx_mbi_cnt              : 0x%08x \n", get_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_MBI_CNT));
}
EXPORT_SYMBOL(show_gsw_ptp_register);

void show_xfi_ptp_register(uint8_t pdev_id)
{
	if (pdev_id == PDEV_XFI_PTP_ID) {
		printk("Display XFI PTP Reg:\n");
	} else {
		printk("Display XFI PON PTP Reg:\n");
	}
	printk("xfi_glb_cfg                     : 0x%08x \n", get_ptp_data(pdev_id, XFI_GLB_CFG));
	printk("xfi_int_sts                     : 0x%08x \n", get_ptp_data(pdev_id, XFI_INT_STS));
	printk("xfi_int_en                      : 0x%08x \n", get_ptp_data(pdev_id, XFI_INT_EN));
	printk("\n");

	printk("xfi_tx_octets_cnt               : 0x%08x \n", get_ptp_data(pdev_id, XFI_TX_OCTETS_CNT));
	printk("xfi_tx_pkt_cnt                  : 0x%08x \n", get_ptp_data(pdev_id, XFI_TX_PKT_CNT));
	printk("xfi_txmbi_eth_cnt               : 0x%08x \n", get_ptp_data(pdev_id, XFI_TXMBI_ETH_CNT));
	printk("xfi_txmbi_uceth_cnt             : 0x%08x \n", get_ptp_data(pdev_id, XFI_TXMBI_UCETH_CNT));
	printk("xfi_txmbi_mceth_cnt             : 0x%08x \n", get_ptp_data(pdev_id, XFI_TXMBI_MCETH_CNT));
	printk("xfi_txmbi_pause_cnt             : 0x%08x \n", get_ptp_data(pdev_id, XFI_TXMBI_PAUSE_CNT));
	printk("\n");

	printk("xfi_xgmii_tx_sof_eof_cnt        : 0x%08x \n", get_ptp_data(pdev_id, XFI_XGMII_TX_SOF_EOF_CNT));
	printk("xfi_xgmii_tx_bytes_cnt          : 0x%08x \n", get_ptp_data(pdev_id, XFI_XGMII_TX_BYTES_CNT));
	printk("xfi_tx_normal_pkt_bytes_cnt     : 0x%08x \n", get_ptp_data(pdev_id, XFI_TX_NORMAL_PKT_BYTES_CNT));
	printk("xfi_tx_deq_check_cnt1           : 0x%08x \n", get_ptp_data(pdev_id, XFI_TX_DEQ_CHECK_CNT1));
	printk("xfi_tx_deq_check_cnt2           : 0x%08x \n", get_ptp_data(pdev_id, XFI_TX_DEQ_CHECK_CNT2));
	printk("\n");

	printk("xfi_rx_frame_cnt                : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_FRAME_CNT));
	printk("xfi_rx_octets_cnt               : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_OCTETS_CNT));
	printk("xfi_rx_pkt_cnt                  : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_PKT_CNT));
	printk("xfi_rx_eth_cnt                  : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_ETH_CNT));
	printk("xfi_rx_pause_cnt                : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_PAUSE_CNT));
	printk("xfi_rx_lenerr_cnt               : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_LENERR_CNT));
	printk("xfi_rx_crcerr_cnt               : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_CRCERR_CNT));
	printk("xfi_rxmbi_pkt_cnt               : 0x%08x \n", get_ptp_data(pdev_id, XFI_RXMBI_PKT_CNT));
	printk("xfi_rxmbi_drop_cnt              : 0x%08x \n", get_ptp_data(pdev_id, XFI_RXMBI_DROP_CNT));
	printk("\n");

	printk("xfi_xgmii_rx_sof_eof_cnt        : 0x%08x \n", get_ptp_data(pdev_id, XFI_XGMII_RX_SOF_EOF_CNT));
	printk("xfi_rx_mpi_sop_eop_cnb          : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_MPI_SOP_EOP_CNT));
	printk("xfi_rx_normal_pkt_bytes_cnt     : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_NORMAL_PKT_BYTES_CNT));
	printk("xfi_enq_check_cnt1              : 0x%08x \n", get_ptp_data(pdev_id, XFI_ENQ_CHECK_CNT1));
	printk("xfi_enq_check_cnt2              : 0x%08x \n", get_ptp_data(pdev_id, XFI_ENQ_CHECK_CNT2));
	printk("xfi_rx_mbi_sop_eop_cnt          : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_MBI_SOP_EOP_CNT));
	printk("\n");

	printk("xfi_ptp_glb_cfg                 : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_GLB_CFG));
	printk("xfi_sync_rx_rate_limit_set      : 0x%08x \n", get_ptp_data(pdev_id, XFI_SYNC_RX_RATE_LIMIT_SET));
	printk("\n");

	printk("xfi_ptp_int_sts                 : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_INT_STS));
	printk("xfi_ptp_int_en                  : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_INT_EN));
	printk("\n");

	printk("xfi_ptp_tx_to_sw_info_rd        : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_TO_SW_INFO_RD));
	printk("xfi_ptp_tx_to_sw_ts_h           : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_TO_SW_TS_H));
	printk("xfi_ptp_tx_to_sw_ts_l           : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_TO_SW_TS_L));
	printk("xfi_ptp_tx_to_sw_type_seqid     : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_TO_SW_TYPE_SEQID));
	printk("xfi_ptp_rx_ts_comp              : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_RX_TS_COMP));
	printk("xfi_ptp_tx_ts_comp              : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_TS_COMP));
	printk("\n");

	printk("xfi_ptp_tx_sync_follow_interval : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_SYNC_FOLLOW_INTERVAL));
	printk("xfi_sync_delay_req_tx_freq      : 0x%08x \n", get_ptp_data(pdev_id, XFI_SYNC_DELAY_REQ_TX_FREQ));
	printk("xfi_pdelay_req_tx_freq          : 0x%08x \n", get_ptp_data(pdev_id, XFI_PDELAY_REQ_TX_FREQ));
	printk("xfi_announce_tx_freq            : 0x%08x \n", get_ptp_data(pdev_id, XFI_ANNOUNCE_TX_FREQ));
	printk("\n");

	printk("xfi_ptp_pkt_seqid_set0          : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_PKT_SEQID_SET0));
	printk("xfi_ptp_pkt_seqid_set1          : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_PKT_SEQID_SET1));
	printk("xfi_ptp_autogen_header_offset   : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_AUTOGEN_HEADER_OFFSET));
	printk("xfi_ptp_autogen_length          : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_AUTOGEN_LENGTH));
	printk("xfi_ptp_autogen_pkt_set         : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_AUTOGEN_PKT_SET));
	printk("xfi_ptp_autogen_pkt_data        : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_AUTOGEN_PKT_DATA));
	printk("\n");

	printk("xfi_vlan_tag_set                : 0x%08x \n", get_ptp_data(pdev_id, XFI_VLAN_TAG_SET));
	printk("xfi_ptp_rx_ts_high_16           : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_RX_TS_HIGH_16));
	printk("\n");

	printk("xfi_ptp_tx_mbi_cnt              : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_MBI_CNT));
	printk("xfi_ptp_rx_mbi_cnt              : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_RXMBI_CNT));
	printk("xfi_ptp_tx_autog_sync_cnt       : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_AUTOG_SYNC_CNT));
	printk("xfi_ptp_tx_autog_delay_req_cnt  : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_AUTOG_DELAY_REQ_CNT));
	printk("xfi_ptp_tx_autog_pdelay_req_cnt : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_AUTOG_PDELAY_REQ_CNT));
	printk("xfi_ptp_tx_autog_announce_cnt   : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_AUTOG_ANNOUNCE_CNT));
	printk("xfi_ptp_tx_autog_follow_up_cnt  : 0x%08x \n", get_ptp_data(pdev_id, XFI_PTP_TX_AUTOG_FOLLOW_UP_CNT));
	printk("\n");

	printk("xfi_rx_parser_eth_num           : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_PARSER_ETH_NUM));
	printk("xfi_rx_parser_ipv4_num          : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_PRRSER_IPV4_NUM));
	printk("xfi_rx_parser_ipv6_num          : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_PARSER_IPV6_NUM));
	printk("xfi_rx_parser_drop_num          : 0x%08x \n", get_ptp_data(pdev_id, XFI_RX_PARSER_DROP_NUM));
	printk("\n");
}
EXPORT_SYMBOL(show_xfi_ptp_register);

void hw_autogen_packet_basic_config(hw_autogen_basic_config_t *hw_autogen_config)
{
	ptp_glb_cfg_t ptp_glb_cfg;
	ptp_autogen_header_offset_t ptp_autogen_header_offset;
	ptp_autogen_length_t ptp_autogen_length;
	ptp_autogen_type_set_t ptp_autogen_type_set;
	ptp_pkt_seqid_set0_t ptp_pkt_seqid_set0;
	ptp_pkt_seqid_set1_t ptp_pkt_seqid_set1;

	memset(&ptp_glb_cfg.word, 0, sizeof(ptp_glb_cfg.word));
	memset(&ptp_autogen_header_offset.word, 0, sizeof(ptp_autogen_header_offset.word));
	memset(&ptp_autogen_length.word, 0, sizeof(ptp_autogen_length.word));
	memset(&ptp_autogen_type_set.word, 0, sizeof(ptp_autogen_type_set.word));
	memset(&ptp_pkt_seqid_set0.word, 0, sizeof(ptp_pkt_seqid_set0.word));

	printk("\n");
	printk("hw_autogen_config->ts_insert        : 0x%x\n", hw_autogen_config->ts_insert);
	printk("hw_autogen_config->tx_freq          : 0x%x\n", hw_autogen_config->tx_freq);
	printk("hw_autogen_config->header_offset    : 0x%x\n", hw_autogen_config->header_offset);
	printk("hw_autogen_config->pkt_len          : 0x%x\n", hw_autogen_config->pkt_len);
	printk("hw_autogen_config->pkt_type         : 0x%x\n", hw_autogen_config->pkt_type);
	printk("hw_autogen_config->autogen_msg_type : 0x%x\n", hw_autogen_config->autogen_msg_type);
	printk("hw_autogen_config->load_seqid       : 0x%x\n", hw_autogen_config->load_seqid);
	printk("\n");

	/* 1. write ts_insert */
	ptp_glb_cfg.bits.ts_insert = hw_autogen_config->ts_insert;

	/* 2. write sync_delay_req_tx_freq  def: 62500 us to transmit a sync or delay-req packet */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE){
		set_ptp_data(PDEV_GSW_PTP_ID, SYNC_DELAY_REQ_TX_FREQ, hw_autogen_config->tx_freq);
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		set_ptp_data(PDEV_GSW_PTP_ID, PDELAY_REQ_TX_FREQ, hw_autogen_config->tx_freq);
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE ){
		set_ptp_data(PDEV_GSW_PTP_ID, ANNOUNCE_TX_FREQ, hw_autogen_config->tx_freq);
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		/* Configure PTP Tx Sync Follow_up interval */
		set_ptp_data(PDEV_GSW_PTP_ID, PTP_TX_SYNC_FOLLOW_INTERVAL, hw_autogen_config->tx_freq);
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	/* 3. write ptp_tx_tx_comp : follow default value */


	/* 4. write sync_delay_req_seqid */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE){
		ptp_pkt_seqid_set0.bits.sync_delay_req_seqid = 0;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		ptp_pkt_seqid_set0.bits.pdelay_req_seqid = 0;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE ||
				hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		ptp_pkt_seqid_set1.bits.anounce_follow_up_seqid = 0;
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	/* 5. write ptp header offset
	      12(da/sa mac) + 4(vlan) + 2(len/type) + 20(ipv4 header) + 8(udp header) */
	/* 6. write ptp message length */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE){
		ptp_autogen_header_offset.bits.sync_delay_req_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.sync_delay_req_len = hw_autogen_config->pkt_len;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		ptp_autogen_header_offset.bits.pdelay_req_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.pdelay_req_len = hw_autogen_config->pkt_len;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE){
		ptp_autogen_header_offset.bits.announce_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.announce_len = hw_autogen_config->pkt_len;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		/* Sync */
		ptp_autogen_header_offset.bits.sync_delay_req_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.sync_delay_req_len = hw_autogen_config->pkt_len;
		/* Follow-Up */
		ptp_autogen_header_offset.bits.follow_up_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.follow_up_len = hw_autogen_config->pkt_len;
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	/* 7. write ptp_packet_type ex. ethernet, over udp/ipv4 */
	ptp_autogen_type_set.bits.ptp_pkt_type = hw_autogen_config->pkt_type;

	/* 8. write ptp_tx_ts_comp_p_m (add timestamp compensation to timestamp) */

	/* 9. write load_sync_follow_up_delay_req_seqid */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE ||
		hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		ptp_glb_cfg.bits.load_sync_follow_up_delay_req_seqid = hw_autogen_config->load_seqid;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		ptp_glb_cfg.bits.load_pdelay_req_seqid = hw_autogen_config->load_seqid;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE){
		ptp_glb_cfg.bits.load_announce_seqid = hw_autogen_config->load_seqid;
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	set_ptp_data(PDEV_GSW_PTP_ID, PTP_GLB_CFG, ptp_glb_cfg.word);
	set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_HEADER_OFFSET, ptp_autogen_header_offset.word);
	set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_LENGTH, ptp_autogen_length.word);
	set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_TYPE_SET, ptp_autogen_type_set.word);
	set_ptp_data(PDEV_GSW_PTP_ID, PTP_PKT_SEQID_SET0, ptp_pkt_seqid_set0.word);
}
EXPORT_SYMBOL(hw_autogen_packet_basic_config);

void hw_autogen_packet_fill_in_data(unsigned char *sync_buf, uint8_t pkt_len, uint8_t autogen_msg_type)
{
	ptp_autogen_type_set_t ptp_autogen_type_set;
	ptp_autogen_pkt_set_t ptp_autogen_pkt_set;
	unsigned int temp32 = 0x0, i;
	unsigned short temp16 = 0x0;

	/* 1. write ptp_msg_type */
	ptp_autogen_type_set.word = get_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_TYPE_SET);
	ptp_autogen_type_set.bits.ptp_msg_type = autogen_msg_type;
	set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_TYPE_SET, ptp_autogen_type_set.word);

	/* 2. write ptp_pkt_data   */
	/* 3. write ptp_pkt_offset */
	memset(&ptp_autogen_pkt_set.word, 0, sizeof(ptp_autogen_pkt_set.word));
	ptp_autogen_pkt_set.bits.ptp_pkt_wr = 1;

	for(i=0; i<pkt_len; i=i+4){
		if(i+4 > pkt_len){
			if(((i+4) - pkt_len) == 2){
				temp16 = ntohs(*(unsigned short *)(&sync_buf[i]));
				set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_PKT_DATA, (unsigned int)temp16);
				set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_PKT_SET, ptp_autogen_pkt_set.word);
				ptp_autogen_pkt_set.bits.ptp_pkt_offset = ptp_autogen_pkt_set.bits.ptp_pkt_offset + 2;
				break;
			}else{
				printk("\n[Error] not expected packet length!\n");
			}
		}

		temp32 = ntohl(*(unsigned int *)(&sync_buf[i]));
		set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_PKT_DATA, temp32);
		set_ptp_data(PDEV_GSW_PTP_ID, PTP_AUTOGEN_PKT_SET, ptp_autogen_pkt_set.word);
		ptp_autogen_pkt_set.bits.ptp_pkt_offset = ptp_autogen_pkt_set.bits.ptp_pkt_offset + 4;
	}
}
EXPORT_SYMBOL(hw_autogen_packet_fill_in_data);

void hw_autogen_packet_send(hw_autogen_basic_config_t *hw_autogen_config)
{
	ptp_glb_cfg_t ptp_glb_cfg;

	/* 1. trigger hw gen sync packet (enable ptp_tx and gen sync packet) */
	ptp_glb_cfg.word = get_ptp_data(PDEV_GSW_PTP_ID, PTP_GLB_CFG);
	ptp_glb_cfg.bits.ptp_rx_en = 0b1;
	ptp_glb_cfg.bits.ptp_tx_en = 0b1;
	ptp_glb_cfg.bits.sw_rst_n = 0b1;

	if(hw_autogen_config->msg_type == PTP_SYNC){
		ptp_glb_cfg.bits.gen_sync = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_DELAY_REQ){
		ptp_glb_cfg.bits.gen_delay_req = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_PDELAY_REQ){
		ptp_glb_cfg.bits.gen_pdelay_req = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_ANNOUNCE){
		ptp_glb_cfg.bits.gen_announce = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_FOLLOW_UP){
		ptp_glb_cfg.bits.gen_sync = 0b1;
		ptp_glb_cfg.bits.gen_follow_up = 0b1;
	}else{
		printk("\n[Error] PTP autogen does not support on this msg_type!\n");
	}

	set_ptp_data(PDEV_GSW_PTP_ID, PTP_GLB_CFG, ptp_glb_cfg.word);
}
EXPORT_SYMBOL(hw_autogen_packet_send);


void xfi_hw_autogen_packet_basic_config(hw_autogen_basic_config_t *hw_autogen_config)
{
	ptp_glb_cfg_t ptp_glb_cfg;
	ptp_autogen_header_offset_t ptp_autogen_header_offset;
	ptp_autogen_length_t ptp_autogen_length;
	xfi_ptp_autogen_pkt_set_t xfi_ptp_autogen_pkt_set;
	ptp_pkt_seqid_set0_t ptp_pkt_seqid_set0;
	ptp_pkt_seqid_set1_t ptp_pkt_seqid_set1;

	memset(&ptp_glb_cfg.word, 0, sizeof(ptp_glb_cfg.word));
	memset(&ptp_autogen_header_offset.word, 0, sizeof(ptp_autogen_header_offset.word));
	memset(&ptp_autogen_length.word, 0, sizeof(ptp_autogen_length.word));
	memset(&xfi_ptp_autogen_pkt_set.word, 0, sizeof(xfi_ptp_autogen_pkt_set.word));
	memset(&ptp_pkt_seqid_set0.word, 0, sizeof(ptp_pkt_seqid_set0.word));

	printk("\n");
	printk("hw_autogen_config->ts_insert        : 0x%x\n", hw_autogen_config->ts_insert);
	printk("hw_autogen_config->tx_freq          : 0x%x\n", hw_autogen_config->tx_freq);
	printk("hw_autogen_config->header_offset    : 0x%x\n", hw_autogen_config->header_offset);
	printk("hw_autogen_config->pkt_len          : 0x%x\n", hw_autogen_config->pkt_len);
	printk("hw_autogen_config->pkt_type         : 0x%x\n", hw_autogen_config->pkt_type);
	printk("hw_autogen_config->autogen_msg_type : 0x%x\n", hw_autogen_config->autogen_msg_type);
	printk("hw_autogen_config->load_seqid       : 0x%x\n", hw_autogen_config->load_seqid);
	printk("\n");

	/* 1. write ts_insert */
	ptp_glb_cfg.bits.ts_insert = hw_autogen_config->ts_insert;

	/* 2. write sync_delay_req_tx_freq  def: 62500 us to transmit a sync or delay-req packet */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE){
		set_ptp_data(PDEV_XFI_PTP_ID, XFI_SYNC_DELAY_REQ_TX_FREQ, hw_autogen_config->tx_freq);
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		set_ptp_data(PDEV_XFI_PTP_ID, XFI_PDELAY_REQ_TX_FREQ, hw_autogen_config->tx_freq);
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE ){
		set_ptp_data(PDEV_XFI_PTP_ID, XFI_ANNOUNCE_TX_FREQ, hw_autogen_config->tx_freq);
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		/* Configure PTP Tx Sync Follow_up interval */
		set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_TX_SYNC_FOLLOW_INTERVAL, hw_autogen_config->tx_freq);
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	/* 3. write ptp_tx_tx_comp : follow default value */


	/* 4. write sync_delay_req_seqid */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE){
		ptp_pkt_seqid_set0.bits.sync_delay_req_seqid = 0;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		ptp_pkt_seqid_set0.bits.pdelay_req_seqid = 0;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE ||
				hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		ptp_pkt_seqid_set1.bits.anounce_follow_up_seqid = 0;
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	/* 5. write ptp header offset
	      12(da/sa mac) + 4(vlan) + 2(len/type) + 20(ipv4 header) + 8(udp header) */
	/* 6. write ptp message length */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE){
		ptp_autogen_header_offset.bits.sync_delay_req_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.sync_delay_req_len = hw_autogen_config->pkt_len;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		ptp_autogen_header_offset.bits.pdelay_req_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.pdelay_req_len = hw_autogen_config->pkt_len;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE){
		ptp_autogen_header_offset.bits.announce_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.announce_len = hw_autogen_config->pkt_len;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		/* Sync */
		ptp_autogen_header_offset.bits.sync_delay_req_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.sync_delay_req_len = hw_autogen_config->pkt_len;
		/* Follow-Up */
		ptp_autogen_header_offset.bits.follow_up_ptp_header_offset = hw_autogen_config->header_offset;
		ptp_autogen_length.bits.follow_up_len = hw_autogen_config->pkt_len;
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	/* 7. write ptp_packet_type ex. ethernet, over udp/ipv4 */
	xfi_ptp_autogen_pkt_set.bits.ptp_pkt_type = hw_autogen_config->pkt_type;

	/* 8. write ptp_tx_ts_comp_p_m (add timestamp compensation to timestamp) */

	/* 9. write load_sync_follow_up_delay_req_seqid */
	if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_SYNC_DELAYREQ_MSG_TYPE ||
		hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_FOLLOWUP_MSG_TYPE){
		ptp_glb_cfg.bits.load_sync_follow_up_delay_req_seqid = hw_autogen_config->load_seqid;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_PDELAYREQ_MSG_TYPE){
		ptp_glb_cfg.bits.load_pdelay_req_seqid = hw_autogen_config->load_seqid;
	}else if(hw_autogen_config->autogen_msg_type == PTP_AUTOGEN_ANNOUNCE_MSG_TYPE){
		ptp_glb_cfg.bits.load_announce_seqid = hw_autogen_config->load_seqid;
	}else{
		printk("\n[Error] Not support on this autogen_msg_type!\n");
	}

	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_GLB_CFG, ptp_glb_cfg.word);
	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_HEADER_OFFSET, ptp_autogen_header_offset.word);
	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_LENGTH, ptp_autogen_length.word);
	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_PKT_SET, xfi_ptp_autogen_pkt_set.word);
	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_PKT_SEQID_SET0, ptp_pkt_seqid_set0.word);
}
EXPORT_SYMBOL(xfi_hw_autogen_packet_basic_config);

void xfi_hw_autogen_packet_fill_in_data(unsigned char *sync_buf, uint8_t pkt_len, uint8_t autogen_msg_type)
{
	xfi_ptp_autogen_pkt_set_t xfi_ptp_autogen_pkt_set;

	unsigned int temp32 = 0x0, i;
	unsigned short temp16 = 0x0;

	/* 1. write ptp_msg_type */
	xfi_ptp_autogen_pkt_set.word = get_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_PKT_SET);
	xfi_ptp_autogen_pkt_set.bits.ptp_msg_type = autogen_msg_type;
	xfi_ptp_autogen_pkt_set.bits.ptp_pkt_offset = 0;
	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_PKT_SET, xfi_ptp_autogen_pkt_set.word);

	/* 2. write ptp_pkt_data   */
	/* 3. write ptp_pkt_offset */

	xfi_ptp_autogen_pkt_set.bits.ptp_pkt_wr = 1;

	for(i=0; i<pkt_len; i=i+4){
		if(i+4 > pkt_len){
			if(((i+4) - pkt_len) == 2){
				temp16 = ntohs(*(unsigned short *)(&sync_buf[i]));
				set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_PKT_DATA, (unsigned int)temp16);
				set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_PKT_SET, xfi_ptp_autogen_pkt_set.word);
				xfi_ptp_autogen_pkt_set.bits.ptp_pkt_offset = xfi_ptp_autogen_pkt_set.bits.ptp_pkt_offset + 2;
				break;
			}else{
				printk("\n[Error] not expected packet length!\n");
			}
		}

		temp32 = ntohl(*(unsigned int *)(&sync_buf[i]));
		set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_PKT_DATA, temp32);
		set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_AUTOGEN_PKT_SET, xfi_ptp_autogen_pkt_set.word);
		xfi_ptp_autogen_pkt_set.bits.ptp_pkt_offset = xfi_ptp_autogen_pkt_set.bits.ptp_pkt_offset + 4;
	}
}
EXPORT_SYMBOL(xfi_hw_autogen_packet_fill_in_data);

void xfi_hw_autogen_packet_send(hw_autogen_basic_config_t *hw_autogen_config)
{
	ptp_glb_cfg_t ptp_glb_cfg;

	/* 1. trigger hw gen sync packet (enable ptp_tx and gen sync packet) */
	ptp_glb_cfg.word = get_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_GLB_CFG);
	ptp_glb_cfg.bits.ptp_rx_en = 0b1;
	ptp_glb_cfg.bits.ptp_tx_en = 0b1;

	if(hw_autogen_config->msg_type == PTP_SYNC){
		ptp_glb_cfg.bits.gen_sync = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_DELAY_REQ){
		ptp_glb_cfg.bits.gen_delay_req = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_PDELAY_REQ){
		ptp_glb_cfg.bits.gen_pdelay_req = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_ANNOUNCE){
		ptp_glb_cfg.bits.gen_announce = 0b1;
	}else if(hw_autogen_config->msg_type == PTP_FOLLOW_UP){
		ptp_glb_cfg.bits.gen_sync = 0b1;
		ptp_glb_cfg.bits.gen_follow_up = 0b1;
	}else{
		printk("\n[Error] PTP autogen does not support on this msg_type!\n");
	}

	set_ptp_data(PDEV_XFI_PTP_ID, XFI_PTP_GLB_CFG, ptp_glb_cfg.word);
}
EXPORT_SYMBOL(xfi_hw_autogen_packet_send);

void set_ptp_pkt_ratelimit(uint8_t pdev_id, uint32_t val)
{
	if (pdev_id == PDEV_GSW_PTP_ID) {
		set_ptp_data(pdev_id, SYNC_RX_RATE_LIMIT_SET, val);
	} else {
		set_ptp_data(pdev_id, XFI_SYNC_RX_RATE_LIMIT_SET, val);
	}
	return;
}
EXPORT_SYMBOL(set_ptp_pkt_ratelimit);

static irqreturn_t (*ptp_isr_fp[3])(int irq, void *dev_instance) =
{
	gsw_ptp_interrupt_handler,
	xfi_ptp_interrupt_handler,
	xfi_pon_ptp_interrupt_handler
};

static int ecnt_ptp_drv_probe(struct platform_device *pdev)
{
	int ret = 0, idx = 0;
	struct resource *res = NULL;
	char *isr_str[3] = {
		"gsw_ptp_interrupt",
		"xfi_ptp_interrupt",
		"xfi_pon_ptp_interrupt"
	};

    printk("[gsw ptp] ecnt_ptp_drv_probe\n");
    if (!pdev->dev.of_node) {
        dev_err(&pdev->dev, "No ptp DT node found");
        return -EINVAL;
    }

    /* gsw ptp allocate memory */
    ecnt_ptp = devm_kzalloc(&pdev->dev, sizeof(struct ecnt_ptp), GFP_KERNEL);
    if (!ecnt_ptp)
        return -ENOMEM;

    platform_set_drvdata(pdev, ecnt_ptp);

    /* get gsw_ptp_base address */
	get_gsw_ptp_base(&ecnt_gsw_ptp_base);
    ecnt_ptp->gsw_ptp_base = ecnt_gsw_ptp_base;
    if (IS_ERR(ecnt_ptp->gsw_ptp_base))
        return PTR_ERR(ecnt_ptp->gsw_ptp_base);

    /* get xfi_ptp_base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, PDEV_XFI_PTP_ID);
    ecnt_ptp->xfi_ptp_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_ptp->xfi_ptp_base))
        return PTR_ERR(ecnt_ptp->xfi_ptp_base);

    /* get xfi_pon_ptp_base address */
    res = platform_get_resource(pdev, IORESOURCE_MEM, PDEV_XFI_PON_PTP_ID);
    ecnt_ptp->xfi_pon_ptp_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(ecnt_ptp->xfi_pon_ptp_base))
        return PTR_ERR(ecnt_ptp->xfi_pon_ptp_base);

    ecnt_ptp->dev = &pdev->dev;

	for(idx = 0; idx < 3; idx++)
	{
		/* 1588v2 ptp interrupt setting */
		ecnt_ptp->irq[idx] = platform_get_irq(pdev, idx);
		if(ecnt_ptp->irq[idx] <= 0) {
			printk("\n[1588v2 PTP] get irq number failed (idx = %d)\n", idx);
			return ecnt_ptp->irq[idx];
		}
		ret = request_irq(ecnt_ptp->irq[idx], *ptp_isr_fp[idx], 0, isr_str[idx],
						ecnt_ptp->dev);
		if(ret) {
			printk("\n request_irq() (irq number: %d) failed (ret: %d)\n", ecnt_ptp->irq[idx], ret);
			return (ret);
		}
	}

#if 1
	printk("[1588v2 ptp] ecnt_gsw_ptp_base          : 0x%lx\n", (unsigned long)ecnt_gsw_ptp_base);
	printk("[1588v2 ptp] ecnt_ptp->gsw_ptp_base     : 0x%lx\n", (unsigned long)ecnt_ptp->gsw_ptp_base);
	printk("[1588v2 ptp] ecnt_ptp->xfi_ptp_base     : 0x%lx\n", (unsigned long)ecnt_ptp->xfi_ptp_base);
	printk("[1588v2 ptp] ecnt_ptp->xfi_pon_ptp_base : 0x%lx\n", (unsigned long)ecnt_ptp->xfi_pon_ptp_base);

    printk("[1588v2 ptp] ecnt_ptp->irq[%d] : 0x%lx \n", PDEV_GSW_PTP_ID, (unsigned long)ecnt_ptp->irq[PDEV_GSW_PTP_ID]);
    printk("[1588v2 ptp] ecnt_ptp->irq[%d] : 0x%lx \n", PDEV_XFI_PTP_ID, (unsigned long)ecnt_ptp->irq[PDEV_XFI_PTP_ID]);
    printk("[1588v2 ptp] ecnt_ptp->irq[%d] : 0x%lx \n", PDEV_XFI_PON_PTP_ID, (unsigned long)ecnt_ptp->irq[PDEV_XFI_PON_PTP_ID]);
#endif

    return 0;
}

static int ecnt_ptp_drv_remove(struct platform_device *pdev)
{
    printk("[ptp] ecnt_ptp_drv_remove\n");
    return 0;
}

/************************************************************************
*      P L A T F O R M   D R I V E R S   D E C L A R A T I O N S
*************************************************************************
*/

static struct platform_driver ecnt_ptp_driver = {
    .probe = ecnt_ptp_drv_probe,
    .remove = ecnt_ptp_drv_remove,
    .driver = {
        .name = "ecnt-1588v2ptp",
        .of_match_table = ecnt_ptp_of_id
    },
};
module_platform_driver(ecnt_ptp_driver);
MODULE_DESCRIPTION("EcoNet ptp Driver");


