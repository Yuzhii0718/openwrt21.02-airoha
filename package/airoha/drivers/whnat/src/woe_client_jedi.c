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

	Module Name: whnat
	whnat_mt7615.c
*/
#define MT7915
#if defined(MT7615) || defined(MT7915)
#include "woe_client_jedi.h"
#include "woe.h"
#ifndef BB_SOC
#include <net/ra_nat.h>
#endif
#include <linux/pci.h>
#include <net/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#ifdef BB_SOC
#include <linux/foe_hook.h>
#endif

/*Local function part*/
/*
*
*/
extern unsigned int wpdma_base_woe;
static void wifi_pcie_match(struct wifi_entry *wifi)
{
	unsigned char idx = 0;
	idx = wifi_slot_get(wifi->cookie);
	wifi->slot_id = idx;
	wifi->wpdma_base = wpdma_base_woe;
}



/*Gloable function*/
/*
*
*/
void wifi_fbuf_init(unsigned char *fbuf, unsigned int pkt_pa, unsigned int tkid)
{
	wifi_card_fbuf_init(fbuf, pkt_pa, tkid);
}

/*
*
*/
#ifdef WHNAT_DBG_EN
/*
*
*/
static void wifi_dump_skb(
	struct whnat_entry *entry,
	struct wlan_tx_info *info,
	struct sk_buff *skb)
{
	struct iphdr *hdr = ip_hdr(skb);

	WHNAT_DBG(WHNAT_DBG_INF,
		"%s(): add entry: wdma=%d,ringId=%d,wcid=%d,bssid=%d\n",
		__func__,
		entry->idx,
		info->ringidx,
		info->wcid,
		info->bssidx);

	if (hdr->version != 4)
		return;

	WHNAT_DBG(WHNAT_DBG_INF,
		"%s():src=%d.%d.%d.%d\n",
		__func__,
		(0xff & hdr->saddr),
		(0xff00 & hdr->saddr) >> 8,
		(0xff0000 & hdr->saddr) >> 16,
		(0xff000000 & hdr->saddr) >> 24);

	WHNAT_DBG(WHNAT_DBG_INF,
		"%s(): dst=%d.%d.%d.%d\n",
		__func__,
		(0xff & hdr->daddr),
		(0xff00 & hdr->daddr) >> 8,
		(0xff0000 & hdr->daddr) >> 16,
		(0xff000000 & hdr->daddr) >> 24);

	if (hdr->protocol == IPPROTO_TCP) {
		struct tcphdr *tcph = tcp_hdr(skb);
		WHNAT_DBG(WHNAT_DBG_INF,
			"%s(): protocol=TCP,sport=%d,dstport=%d\n",
			__func__,
			tcph->source,
			tcph->dest);
	}

	if (hdr->protocol == IPPROTO_UDP) {
		struct udphdr *udph = udp_hdr(skb);
		WHNAT_DBG(WHNAT_DBG_INF,
			"%s(): protocol=UDP,sport=%d,dstport=%d\n",
			__func__,
			udph->source,
			udph->dest);
	}
}
#endif /*WHNAT_DBG_EN*/

/*
*
*/
#define BB_SOC
void wifi_tx_tuple_add(void *entry, unsigned char *tx_info)
{
	struct whnat_entry *whnat = (struct whnat_entry *)entry;
	struct wlan_tx_info t, *info =  &t;

	memset(info, 0, sizeof(*info));
	info = (struct wlan_tx_info *)tx_info;
	if(info->ringidx != 0 && info->ringidx != 1)
		return;
#ifdef BB_SOC
    struct port_info wifi_info;
	memset(&wifi_info, 0, sizeof(struct port_info));
#endif 

	if (whnat && ra_sw_nat_hook_tx && whnat->cfg.hw_tx_en
	) {
		struct sk_buff *skb = (struct sk_buff *)info->pkt;
		
#ifdef BB_SOC
    	wifi_info.magic = FOE_MAGIC_WLAN_7615;
#if defined(TCSUPPORT_CPU_AN7552)
	wifi_info.nbq = 10 + (info->ringidx&0x1);
	wifi_info.channel = 6 + (info->ringidx&0x1);
#else		
#if defined(TCSUPPORT_CPU_EN7580)
        wifi_info.tsid = 0x7f;
#if defined(TCSUPPORT_CPU_EN7523)
	wifi_info.nbq = 0+((whnat->idx&0x1)<<1) | (info->ringidx&0x1);
	wifi_info.channel = 6 + (((whnat->idx&0x1)<<1) | (info->ringidx&0x1));		
#else
        wifi_info.nbq = ((whnat->idx&0x1)<<1) | (info->ringidx&0x1);
	wifi_info.channel = 8 + (((whnat->idx&0x1)<<1) | (info->ringidx&0x1));
#endif
#endif
#if defined(TCSUPPORT_CPU_EN7528)
		if((((whnat->idx&0x1)<<1) | (info->ringidx&0x1)) == 3){
			wifi_info.channel = 0;
		}
		else{
			wifi_info.channel = 5 + (((whnat->idx&0x1)<<1) | (info->ringidx&0x1));
		}
#endif
#endif
        wifi_info.stag = ((info->wcid&0xFF)<<6) | (info->bssidx&0x3F);
		if (ra_sw_nat_hook_tx) {
			ra_sw_nat_hook_tx(skb, &wifi_info, wifi_info.magic);
			/*
			if (ra_sw_nat_hook_tx(skb, &wifi_info, wifi_info.magic) == 0) {
				//txblk->DropPkt = TRUE;
				//printk("%s:%d drop !!!!\n",__func__,__LINE__);
			}
			*/
		}
#else
		if ((FOE_AI_HEAD(skb) == HIT_UNBIND_RATE_REACH) || (FOE_AI_TAIL(skb) == HIT_UNBIND_RATE_REACH)) {
			if (IS_SPACE_AVAILABLE_HEAD(skb)) {
				/*WDMA idx*/
				FOE_WDMA_ID_HEAD(skb) = whnat->idx;
				/*Ring idx*/
				FOE_RX_ID_HEAD(skb) = info->ringidx;
				/*wtable Idx*/
				FOE_WC_ID_HEAD(skb) = info->wcid;
				/*Bssidx*/
				FOE_BSS_ID_HEAD(skb) = info->bssidx;
			}
			if (IS_SPACE_AVAILABLE_TAIL(skb)) {
				/*WDMA idx*/
				FOE_WDMA_ID_TAIL(skb) = whnat->idx;
				/*Ring idx*/
				FOE_RX_ID_TAIL(skb) = info->ringidx;
				/*wtable Idx*/
				FOE_WC_ID_TAIL(skb) = info->wcid;
				/*Bssidx*/
				FOE_BSS_ID_TAIL(skb) = info->bssidx;
			}
		}

		/*use port for specify which hw_nat architecture*/
		if (ra_sw_nat_hook_tx) {
			ra_sw_nat_hook_tx(skb, WHNAT_WDMA_PORT);
			/*
			if (ra_sw_nat_hook_tx(skb, WHNAT_WDMA_PORT) == 0) {
				//txblk->DropPkt = TRUE;
				//printk("%s:%d drop !!!!\n",__func__,__LINE__);
			}
			*/
		}
#endif
#ifdef WHNAT_DBG_EN
		wifi_dump_skb(whnat, info, skb);
#endif /*WHNAT_DBG_EN*/
	}
}

/*
*
*/
enum {
	DMA_TX,
	DMA_RX,
	DMA_TX_RX,
};

void wifi_dma_cfg_wrapper(int wifi_cfg, unsigned char *dma_cfg)
{
	if (wifi_cfg == -1)
		*dma_cfg = WHNAT_DMA_DISABLE;
	else {
		switch (wifi_cfg) {
		case DMA_TX_RX:
			*dma_cfg = WHNAT_DMA_TXRX;
			break;

		case DMA_TX:
			*dma_cfg = WHNAT_DMA_TX;
			break;

		case DMA_RX:
			*dma_cfg = DMA_TX_RX;
			break;
		}
	}
}

/*
*
*/
void wifi_tx_tuple_reset(void)
{
	/* FoeTblClean(); */
}

/*
*
*/
unsigned int wifi_ser_status(void *ser_ctrl)
{
#ifdef ERR_RECOVERY
	ERR_RECOVERY_CTRL_T *ctrl = (ERR_RECOVERY_CTRL_T *)ser_ctrl;

	if (ctrl)
		return ctrl->errRecovStage;

#endif /*ERR_RECOVERY*/
	return WIFI_ERR_RECOV_NONE;
}


/*
*
*/
void dump_wifi_value(struct wifi_entry *wifi, char *name, unsigned int addr)
{
	unsigned int value;

	WHNAT_IO_READ32(wifi, addr, &value);
	WHNAT_DBG(WHNAT_DBG_OFF, "%s\t:%x\n", name, value);
}

/*
*
*/
unsigned int wifi_chip_id_get(void *cookie)
{
	struct woe_need *ad = (struct woe_need *)cookie;
	
	return ad->ChipID;
}

/*
*
*/
unsigned int wifi_whnat_en_get(void *cookie)
{
	struct woe_need *ad = (struct woe_need *)cookie;
	return ad->whnat_en;
	//return ad->CommonCfg.whnat_en;
}

/*
*
*/
void wifi_whnat_en_set(void *cookie, unsigned int en)
{
	struct woe_need *ad = (struct woe_need *)cookie;
	ad->whnat_en = en;
	//ad->CommonCfg.whnat_en = en;
}

#define WED_REG_BASE 0
#define  WED_TX0_CTRL0 (WED_REG_BASE + 0x00000300)
#define  WED_TX0_CTRL1 (WED_REG_BASE + 0x00000304)
#define  WED_TX0_CTRL2 (WED_REG_BASE + 0x00000308)
#define  WED_TX0_CTRL3 (WED_REG_BASE + 0x0000030C)
#define  WED_TX1_CTRL0 (WED_REG_BASE + 0x00000310)
#define  WED_TX1_CTRL1 (WED_REG_BASE + 0x00000314)
#define  WED_TX1_CTRL2 (WED_REG_BASE + 0x00000318)
#define  WED_TX1_CTRL3 (WED_REG_BASE + 0x0000031C)
/*
*
*/
extern void __iomem * wed_base_addr;
extern void __iomem * wed_base_addr_1;
void writel_woe(unsigned int value, void __iomem *addr,struct mt76_queue *q){
	void __iomem * addr_w;
	//if(q->hw_idx == MT_TXQ_ID(0))
	if(q->hw_idx == 18){
		if(addr == &q->regs->desc_base)
			addr_w = wed_base_addr + WED_TX0_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_w = wed_base_addr + WED_TX0_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_w = wed_base_addr + WED_TX0_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_w = wed_base_addr + WED_TX0_CTRL3;
	}
	//else if(q->hw_idx == MT_TXQ_ID(1))
	else if(q->hw_idx == 19)	{
		if(addr == &q->regs->desc_base)
			addr_w = wed_base_addr + WED_TX1_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_w = wed_base_addr + WED_TX1_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_w = wed_base_addr + WED_TX1_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_w = wed_base_addr + WED_TX1_CTRL3;
	}
	writel(value, (void *)addr_w);
}

void writel_woe_1(unsigned int value, void __iomem *addr,struct mt76_queue *q){
	void __iomem *addr_w;
	//if(q->hw_idx == MT_TXQ_ID(0))
	if(q->hw_idx == 18){
		if(addr == &q->regs->desc_base)
			addr_w = wed_base_addr_1 + WED_TX0_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_w = wed_base_addr_1 + WED_TX0_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_w = wed_base_addr_1 + WED_TX0_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_w = wed_base_addr_1 + WED_TX0_CTRL3;
	}
	//else if(q->hw_idx == MT_TXQ_ID(1))
	else if(q->hw_idx == 19)	{
		if(addr == &q->regs->desc_base)
			addr_w = wed_base_addr_1 + WED_TX1_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_w = wed_base_addr_1 + WED_TX1_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_w = wed_base_addr_1 + WED_TX1_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_w = wed_base_addr_1 + WED_TX1_CTRL3;
	}
	writel(value, (void *)addr_w);
}



int readl_woe(void __iomem *addr, struct mt76_queue *q)
{
	void __iomem * addr_r;
	//if(q->hw_idx == MT_TXQ_ID(0))
	if(q->hw_idx == 18){
		if(addr == &q->regs->desc_base)
			addr_r = wed_base_addr + WED_TX0_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_r = wed_base_addr + WED_TX0_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_r = wed_base_addr + WED_TX0_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_r = wed_base_addr + WED_TX0_CTRL3;
	}
	//else if(q->hw_idx == MT_TXQ_ID(1))
	if(q->hw_idx == 19){
		if(addr == &q->regs->desc_base)
			addr_r = wed_base_addr + WED_TX1_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_r = wed_base_addr + WED_TX1_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_r = wed_base_addr + WED_TX1_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_r = wed_base_addr + WED_TX1_CTRL3;
	}
	return readl((void *)addr_r);
}

int readl_woe_1(void __iomem *addr, struct mt76_queue *q)
{
	void __iomem * addr_r;
	//if(q->hw_idx == MT_TXQ_ID(0))
	if(q->hw_idx == 18){
		if(addr == &q->regs->desc_base)
			addr_r = wed_base_addr_1 + WED_TX0_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_r = wed_base_addr_1 + WED_TX0_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_r = wed_base_addr_1 + WED_TX0_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_r = wed_base_addr_1 + WED_TX0_CTRL3;
	}
	//else if(q->hw_idx == MT_TXQ_ID(1))
	if(q->hw_idx == 19){
		if(addr == &q->regs->desc_base)
			addr_r = wed_base_addr_1 + WED_TX1_CTRL0;
		if(addr == &q->regs->ring_size)
			addr_r = wed_base_addr_1 + WED_TX1_CTRL1;
		if(addr == &q->regs->cpu_idx)
			addr_r = wed_base_addr_1 + WED_TX1_CTRL2;
		if(addr == &q->regs->dma_idx)
			addr_r = wed_base_addr_1 + WED_TX1_CTRL3;
	}
	return readl((void *)addr_r);
}

/*
*
*/
void wifi_chip_cr_mirror_set(struct wifi_entry *wifi, unsigned char enable)
{
	struct woe_need *ad = (struct woe_need *)wifi->cookie;
	unsigned char idx = 0;
	idx = wifi_slot_get(wifi->cookie);
	if (enable) {
		printk("%s:%d no need to map \n",__func__,__LINE__);
	} else {
		if(idx == 0)
		{
			ad->readl_api = readl_woe;
			ad->writel_api = writel_woe;			
		}
		else
		{
			ad->readl_api = readl_woe_1;
			ad->writel_api = writel_woe_1;			
		}
	}
}

/*
* CHIP related setting
*/
#define FALSE   		0
#define TRUE    		1

void wifi_chip_probe(struct wifi_entry *wifi, unsigned int irq)
{
	//struct _RTMP_ADAPTER *ad = (struct _RTMP_ADAPTER *)wifi->cookie;
	struct woe_need *ad = (struct woe_need *)wifi->cookie;
	struct os_cookie *os_cookie = (struct os_cookie *)ad->OS_Cookie;
	struct pci_dev *pci_dev = os_cookie->pci_dev;
	WHNAT_DBG(WHNAT_DBG_OFF, "%s(): Chang CHIP IRQ: %d to WHNAT IRQ: %d\n", __func__, pci_dev->irq, irq);
	wifi->irq = pci_dev->irq;
	pci_dev->irq = irq;
	/*always disable hw cr mirror first */
	wifi_chip_cr_mirror_set(wifi, FALSE);
	wifi->int_mask = &(ad->int_enable_mask);
	wifi_pcie_match(wifi);
	wifi->base_addr = ad->CSRBaseAddress;
}

/*
*
*/
void wifi_chip_remove(struct wifi_entry *wifi)
{
	struct woe_need *ad = (struct woe_need *)wifi->cookie;
	struct os_cookie *os_cookie = (struct os_cookie *)ad->OS_Cookie;
	struct pci_dev *pci_dev = os_cookie->pci_dev;
	//struct _RTMP_CHIP_OP *ops = hc_get_chip_ops(ad->hdev_ctrl);

	WHNAT_DBG(WHNAT_DBG_OFF, "%s(): Chang WED IRQ: %d to CHIP IRQ: %d\n", __func__, pci_dev->irq, wifi->irq);
	/*revert pci irq as original irq*/
	pci_dev->irq = wifi->irq;
	wifi->irq = 0;
	wifi_chip_cr_mirror_set(wifi, TRUE);
	wifi->base_addr = 0;
	wifi->int_mask = NULL;
}

/*
*
*/
int wifi_slot_get(void *cookie)
{
	struct woe_need *ad = (struct woe_need *) cookie;
	struct os_cookie *os_cookie = (struct os_cookie *)ad->OS_Cookie;
	struct pci_dev *pci_dev = os_cookie->pci_dev;
	unsigned int id = 1;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
	id=pci_domain_nr(pci_dev->bus);
	WHNAT_DBG(WHNAT_DBG_OFF, "%s(): get slot id=%d\n",__func__,id);
#else	
	if (pci_dev->bus) {
		id = (pci_dev->bus->self->devfn >> 3) & 0x1f;
		WHNAT_DBG(WHNAT_DBG_OFF, "%s(): bus name=%s, funid=%d, get slot id=%d\n",
			__func__,
			pci_dev->bus->name,
			pci_dev->bus->self->devfn,
			id);
	}
#endif	
	return id;
}

unsigned int wifi_wpdma_base_get(void *cookie)
{
	//struct _RTMP_ADAPTER *ad = (RTMP_ADAPTER *) cookie;
	struct woe_need *ad = (struct woe_need *) cookie;
	struct os_cookie *os_cookie = (struct os_cookie *)ad->OS_Cookie;
	struct pci_dev *pci_dev = os_cookie->pci_dev;
	unsigned int wpdma_base = 0;

	if (pci_dev->bus) {
		wpdma_base = (unsigned int) pci_resource_start(pci_dev, 0);
		wpdma_base |= WPDMA_OFFSET;
	}
	return wpdma_base ;
}
/*
*
*/

void wifi_cap_get(struct wifi_entry *wifi)
{
	struct woe_need *ad = (struct woe_need *)wifi->cookie;
	unsigned int data_tx_ring_num = 0;
	unsigned int data_tx_ring_len = 0;
	data_tx_ring_num = ad->tx_res_num;
	data_tx_ring_len = ad->mt76_queue_info.ndesc;
	wifi->tx_ring_num = data_tx_ring_num;
	wifi->tx_ring_len = data_tx_ring_len;
	wifi->tx_token_nums = ad->token_tx_cnt;
	wifi->sw_tx_token_nums = wifi->tx_token_nums - ad->hw_tx_token_cnt;
}

#endif /*MT7615*/
