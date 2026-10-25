// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
#include <warp.h>
#include <warp_hw.h>
#include <linux/proc_fs.h>
#include <linux/pci.h>
#include "warp_hw_airoha.h"

/*
 *
 */
u8
warp_get_pcie_slot(struct pci_dev *pdev)
{
	u8 id;

	id = pci_domain_nr(pdev->bus);
	warp_dbg(WARP_DBG_INF, "[ARHT]%s() id:%d (%d)\n",
		__func__, id, (pdev->bus->self->devfn >> 3) & 0x1f);

	return id;
}

/*
 *
 */
void
bus_setup(struct warp_bus *bus)
{
#if defined(TCSUPPORT_CPU_EN7581)
#define PCIE_BASE_ADDR0 0x1FC00000
#define PCIE_BASE_ADDR1 0x1FC20000
#elif defined(TCSUPPORT_CPU_EN7523)
#define PCIE_BASE_ADDR0 0x1FA91000
#define PCIE_BASE_ADDR1 0x1FA92000
#else
#define PCIE_BASE_ADDR0 0x1FB81000
#define PCIE_BASE_ADDR1 0x1FB83000
#endif
	bus->pcie_base[0] = PCIE_BASE_ADDR0;
	bus->pcie_base[1] = PCIE_BASE_ADDR1;

	bus->pcie_intm[0] = bus->pcie_base[0] | 0x420;
	bus->pcie_intm[1] = bus->pcie_base[1] | 0x420;
	bus->pcie_ints[0] = bus->pcie_base[0] | 0x424;
	bus->pcie_ints[1] = bus->pcie_base[1] | 0x424;

	/*MT7622 MSI PCIE setting need to check*/
	bus->pcie_msim[0] = 0;
	bus->pcie_msim[1] = 0;
	bus->pcie_msis[0] = 0;
	bus->pcie_msis[1] = 0;

	/*default valule will run time overlap it*/
	bus->wpdma_base[0] = WPDMA_BASE_ADDR0;
	bus->wpdma_base[1] = WPDMA_BASE_ADDR1;

#if defined(TCSUPPORT_CPU_EN7581)
	bus->pcie_ints_offset = (1 << 24);
#else
	bus->pcie_ints_offset = (1 << 16);
#endif
	bus->pcie_msis_offset = (1 << 8);
	bus->trig_flag = IRQF_TRIGGER_LOW;
}

/*
 *
 */
void
warp_bus_set_hw(struct wed_entry *wed, struct warp_bus *bus,
			int idx, bool msi_enable, u32 hif_type)
{
	u32 value = bus->pcie_base[idx];

	warp_io_write32(wed, WED_PCIE_CFG_BASE, value);
	/*pcie interrupt agent control*/
#ifdef WED_WORK_AROUND_INT_POLL
	value = (PCIE_POLL_MODE_ALWAYS << WED_PCIE_INT_CTRL_FLD_POLL_EN);
	warp_io_write32(wed, WED_PCIE_INT_CTRL, value);
#endif /*WED_WORK_AROUND_INT_POLL*/
#if defined(TCSUPPORT_CPU_EN7581)
	warp_io_read32(wed, WED_PCIE_INT_CTRL, &value);
	//Added by Yuanli
	value |= (1<<20);
	warp_io_write32(wed, WED_PCIE_INT_CTRL, value);
#endif
#if defined(TCSUPPORT_CPU_AN7552)
#define  WED_PCIE_INT_CTRL2 (WED_REG_BASE + 0x00000b90)
	if (idx == 1) {
		value |= (1 << 16);
		warp_dbg(WARP_DBG_OFF, "%s(): WED_PCIE_INT_CTRL value=%x\n", __func__, value);
		warp_io_write32(wed, WED_PCIE_INT_CTRL, value);

		warp_io_read32(wed, WED_PCIE_INT_CTRL2, &value);
		warp_dbg(WARP_DBG_OFF, "%s(): WED_PCIE_INT_CTRL2 value=%x\n", __func__, value);
		value |= (1 << 0);
		warp_dbg(WARP_DBG_OFF, "%s(): WED_PCIE_INT_CTRL2 value=%x\n", __func__, value);
		warp_io_write32(wed, WED_PCIE_INT_CTRL2, value);
	}
#endif
}

/*
 *
 */
static void
wdma_dma_ctrl(struct wdma_entry *wdma, u8 txrx)
{
	u32 wdma_cfg = 0;

	warp_io_read32(wdma, WDMA_GLO_CFG, &wdma_cfg);
	warp_dbg(WARP_DBG_OFF, "%s(): WDMA_GLO_CFG=%x\n",  __func__, wdma_cfg);

	if (txrx) {
		/*reset wdma*/
		wdma_cfg |= (1 << WDMA_GLO_CFG_RX_INFO1_PRERESERVE);
		wdma_cfg |= (1 << WDMA_GLO_CFG_RX_INFO2_PRERESERVE);
		wdma_cfg |= (1 << WDMA_GLO_CFG_RX_INFO3_PRERESERVE);
		warp_io_write32(wdma, WDMA_GLO_CFG, wdma_cfg);
	} else {
		wdma_cfg &= ~(1 << WDMA_GLO_CFG_RX_INFO1_PRERESERVE);
		wdma_cfg &= ~(1 << WDMA_GLO_CFG_RX_INFO2_PRERESERVE);
		warp_io_write32(wdma, WDMA_GLO_CFG, wdma_cfg);
	}
}

/*
 *
 */
void
warp_wdma_init_hw(struct wed_entry *wed, struct wdma_entry *wdma, int idx)
{
	u32 value = 0;
	/*Apply WDMA  related setting*/
	wdma_dma_ctrl(wdma, WARP_DMA_TXRX);
	value = (idx) ? WDMA1_CFG_BASE : WDMA0_CFG_BASE;
	warp_dbg(WARP_DBG_INF, "[%s] (wdma%d)cfg base: %x\n", __func__, idx, value);
	warp_io_write32(wed, WED_WDMA_CFG_BASE, value);
	/*offset 0*/
	value = (idx) ? WDMA1_OFST0 : WDMA0_OFST0;
	warp_io_write32(wed, WED_WDMA_OFST0, value);
	/*offset 1*/
	value = (idx) ? WDMA1_OFST1 : WDMA0_OFST1;
	warp_io_write32(wed, WED_WDMA_OFST1, value);
}

/*
 *
 */
void
warp_wed_init_hw(struct wed_entry *wed, struct wdma_entry *wdma)
{
	u32 wed_wdma_cfg;
	/*cfg wdma recycle*/
	warp_io_read32(wed, WED_WDMA_GLO_CFG, &wed_wdma_cfg);
	wed_wdma_cfg &= ~(WED_WDMA_GLO_CFG_FLD_WDMA_BT_SIZE_MASK <<
			  WED_WDMA_GLO_CFG_FLD_WDMA_BT_SIZE);
	wed_wdma_cfg &= ~((1 << WED_WDMA_GLO_CFG_FLD_IDLE_STATE_DMAD_SUPPLY_EN) |
			  (1 << WED_WDMA_GLO_CFG_FLD_DYNAMIC_SKIP_DMAD_PREPARE) |
			  (1 << WED_WDMA_GLO_CFG_FLD_DYNAMIC_DMAD_RECYCLE));
	/*disable auto idle*/
	wed_wdma_cfg &= ~(1 << WED_WDMA_GLO_CFG_FLD_RX_DRV_DISABLE_FSM_AUTO_IDLE);
	/*Set to 16 DWORD for 64bytes*/
	wed_wdma_cfg |= (0x2 << WED_WDMA_GLO_CFG_FLD_WDMA_BT_SIZE);
	wed_wdma_cfg |= ((1 << WED_WDMA_GLO_CFG_FLD_AXI_W_AFTER_AW_EN) |
			 (1 << WED_WDMA_GLO_CFG_FLD_WCOMPLETE_SEL));
#ifdef WED_HW_TX_SUPPORT
	warp_io_write32(wed, WED_WDMA_GLO_CFG, wed_wdma_cfg);
#endif /*WED_HW_TX_SUPPORT*/
}


