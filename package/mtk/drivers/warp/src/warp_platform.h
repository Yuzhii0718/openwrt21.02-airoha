/* SPDX-License-Identifier: <SPDX License Expression> */
/*
 ***************************************************************************
 * MediaTek Inc.
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attempt
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

	Module Name: wifi_offload
	warp_platform.h
*/

#ifndef _WARP_PLATFORM_H
#define _WARP_PLATFORM_H

#ifdef CONFIG_WARP_PLATFORM_MTK
#include <warp_platform/warp_mtk.h>
#endif

#ifdef ARHT_SOC
#include <warp_platform/warp_airoha.h>
#endif

extern const struct of_device_id warp_of_ids[];

int wifi_tx_tuple_add(struct wifi_entry *wifi, u8 idx, u8 *tx_info, u32 wdma_rx_port);
void wifi_tx_tuple_reset(struct wifi_entry *wifi);
void wifi_rx_tuple_add(struct wifi_entry *wifi, u8 idx, unsigned char *rx_info);

#endif
