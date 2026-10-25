/*
 * Copyright (c) [2020], MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary to
 * MediaTek Inc. and/or its licensors.
 * Except as otherwise provided in the applicable licensing terms with
 * MediaTek Inc. and/or its licensors, any reproduction, modification, use or
 * disclosure of MediaTek Software, and information contained herein, in whole
 * or in part, shall be strictly prohibited.
*/
/*
 ***************************************************************************
 ***************************************************************************

	Module Name: wifi_offload
	woe_client_jedi.h
*/


#ifndef _WOE_CLIENT_JEDI_H_
#define _WOE_CLIENT_JEDI_H_

//#include "rt_config.h"
//#include "rtmp_comm.h"
//#include "rt_os_util.h"
//#include "rt_os_net.h"
//#include <os/rt_linux_txrx_hook.h>
#include <linux/foe_hook.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/leds.h>
#include <linux/usb.h>
#include <linux/average.h>
#include <net/mac80211.h>
#include <linux/dma-mapping.h>
#include <linux/module.h>
#include <linux/pci.h>



#define MT7915
#ifdef MT7915

typedef struct pci_dev		*PPCI_DEV;
#define __le32  unsigned int
#define dma_addr_t  unsigned int
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned short USHORT;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned char	BOOLEAN;
typedef unsigned long	ULONG;
typedef unsigned char	UCHAR;
typedef void 		VOID;
typedef VOID		*PVOID;
typedef int 		INT;
typedef UCHAR * PUCHAR;
#define RTMP_OS_PID						ULONG /* value or pointer */

struct mt76_desc {
	__le32 buf0;
	__le32 ctrl;
	__le32 buf1;
	__le32 info;
} __packed __aligned(4);

struct os_cookie
{
//#ifdef RTMP_MAC_PCI
	PPCI_DEV pci_dev;
	PPCI_DEV parent_pci_dev;
	USHORT DeviceID;
//#endif /* RTMP_MAC_PCI */
	struct device *pDev;
	UINT32 pAd_va;
#if defined(RTMP_MAC_PCI) || defined(RTMP_MAC_USB)
#ifdef UAPSD_SUPPORT
	RTMP_NET_TASK_STRUCT uapsd_eosp_sent_task;
#endif /* UAPSD_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
	RTMP_NET_TASK_STRUCT carrier_sense_task;
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
#endif
	RTMP_OS_PID			apd_pid; /*802.1x daemon pid */
	unsigned long			apd_pid_nr;
#ifdef CONFIG_AP_SUPPORT
#ifdef IAPP_SUPPORT
	/*	RT_SIGNAL_STRUC			RTSignal; */
	RTMP_OS_PID			IappPid; /*IAPP daemon pid */
	unsigned long			IappPid_nr;
#endif /* IAPP_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
	INT						ioctl_if_type;
	INT					ioctl_if;
	struct _SECURITY_CONFIG *pSecConfig;
#ifdef FW_DUMP_SUPPORT
	struct proc_dir_entry *proc_fwdump_dir;
	struct proc_dir_entry *proc_fwdump_file;
	CHAR fwdump_dir_name[11];
#endif
};


struct mt76_queue_entry {
	union {
		void *buf;
		struct sk_buff *skb;
	};
	union {
		struct mt76_txwi_cache *txwi;
		struct urb *urb;
		int buf_sz;
	};
	u32 dma_addr[2];
	u16 dma_len[2];
	u16 wcid;
	bool skip_buf0:1;
	bool skip_buf1:1;
	bool done:1;
};

struct mt76_queue_regs {
	u32 desc_base;
	u32 ring_size;
	u32 cpu_idx;
	u32 dma_idx;
} __packed __aligned(4);

//#define MT76_MP23
#if defined(TCSUPPORT_WLAN_MT76_MP23)
#define MT76_MP23
#else	
#undef MT76_MP23
#endif

#ifdef MT76_MP23
enum {
	/* Per dev counters*/
	MT_RX_DROP_DMAD_RRO_REPEAT,
	MT_RX_DROP_DMAD_RRO_OLDPKT,
	MT_RX_DROP_DMAD_RRO_PN_CHK_FAIL,
	MT_RX_DROP_DMAD_WO_FRAG,
	MT_RX_DROP_DMAD_WO_DROP,
	MT_RX_DROP_DMAD_ADDR_NOT_FOUND,
	MT_RX_DROP_DMAD_TOKEN_NOT_FOUND,
	MT_RX_DROP_DMAD_GET_TOKEN_FAIL,
	MT_RX_DROP_DMAD_GET_RXWI_FAIL,
	MT_RX_DROP_DMAD_NOMEM,
	MT_RX_DROP_DMAD_DMA_MAPPING_FAIL,
	MT_RX_DROP_BUILD_SKB_FAIL,

	MT_RX_DROP_PER_Q_MAX,

	/* Per phy counters */
	MT_RX_DROP_RXD_ERR = 0,
	MT_RX_DROP_STATE_ERR,
	MT_RX_DROP_RFC_PKT,
	MT_RX_DROP_AGG_SN_LESS,
	MT_RX_DROP_AGG_DUP,

	MT_RX_DROP_PER_PHY_MAX,
};


struct mt76_queue {
	struct mt76_queue_regs __iomem *regs;

	spinlock_t lock;
	spinlock_t cleanup_lock;
	struct mt76_queue_entry *entry;
	struct mt76_rro_desc *rro_desc;
	struct mt76_desc *desc;

	u16 first;
	u16 head;
	u16 tail;
	u8 hw_idx;
	u8 ep;
	int ndesc;
	int queued;
	int buf_size;
	bool stopped;
	bool blocked;

	u8 buf_offset;
	u16 flags;

	struct mtk_wed_device *wed;
	u32 wed_regs;

	dma_addr_t desc_dma;
	struct sk_buff *rx_head;
	struct page_frag_cache rx_page;

	u32 rx_drop[MT_RX_DROP_PER_Q_MAX];
};
#else
struct mt76_queue {
	struct mt76_queue_regs __iomem *regs;

	spinlock_t lock;
	spinlock_t cleanup_lock;
	struct mt76_queue_entry *entry;
	struct mt76_desc *desc;

	u16 first;
	u16 head;
	u16 tail;
	int ndesc;
	int queued;
	int buf_size;
	bool stopped;
	bool blocked;

	u8 buf_offset;
	u8 hw_idx;
	u8 qid;
	u8 flags;
	u32 wed_regs;
	dma_addr_t desc_dma;
	struct sk_buff *rx_head;
	struct page_frag_cache rx_page;
};

#endif

struct mt76_queue_woe {
	struct mt76_queue_regs __iomem *regs;
	int ndesc;
};
struct woe_need{
	unsigned int ChipID;
	unsigned int tx_res_num;
	BOOLEAN whnat_en;
	//struct hif_pci_tx_ring_desc tx_ring_layout[2];
	struct mt76_queue_woe mt76_queue_info;
	UINT32 int_enable_mask;
	UINT32 token_tx_cnt;
	UINT32 hw_tx_token_cnt;
	unsigned long CSRBaseAddress;
	VOID (*writel_api)(unsigned int value, void *addr,struct mt76_queue *q);
	INT32 (*readl_api)(void *addr, struct mt76_queue *q);
	struct mt76_queue_regs __iomem *regs_phy0 ;
	struct mt76_queue_regs __iomem *regs_phy1 ;
	UINT32 wpdma_base_woe;
	UINT32 wed_int_msk;
	UINT32 irqmask;
	UINT32 intr_return;
	BOOLEAN wed_running;
	UINT32 ring_rx_val;
	UINT32 ring_rx_index;
	UINT32 pAd_index;
	VOID *OS_Cookie;
};
#include "woe_mt7915.h"
#endif
//extern int (*ra_sw_nat_hook_tx)(struct sk_buff * skb, struct port_info * pinfo, int magic);
#if 0
#ifndef BB_SOC
extern int (*ra_sw_nat_hook_tx)(struct sk_buff *skb, int gmac_no);
#else
extern int (*ra_sw_nat_hook_tx)(struct sk_buff * skb, struct port_info * pinfo, int magic)
#endif
#endif
//extern struct _RTMP_CHIP_CAP *hc_get_chip_cap(void *hdev_ctrl);
#ifdef MULTI_INF_SUPPORT
/*EXPORT symbol from wifi drvier*/
extern int multi_inf_get_idx(VOID *pAd);
#endif /*MULTI_INF_SUPPORT*/
#define TXD_SIZE		16	/* TXD_SIZE = TxD + TxInfo */
#define WIFI_RING_OFFSET		0x10
#define WIFI_TX_RING_SIZE		(2048)
#define WIFI_PDMA_TXD_SIZE		(TXD_SIZE)
#define WIFI_TX_1ST_BUF_SIZE	128
#define WIFI_RX1_RING_SIZE		(512)
#define WIFI_TX_BUF_SIZE		1900

#define WIFI_TXD_INIT(_txd) (((struct _TXD_STRUC *) _txd)->DMADONE = DMADONE_DONE)

#endif /*_WOE_CLIENT_JEDI_H_*/
