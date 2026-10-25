/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
#ifndef _WARP_AIROHA_HW_H_
#define _WARP_AIROHA_HW_H_

#ifdef ARHT_SOC


/* wdma define */
#define WDMA0_OFST0 0x62046220
#define WDMA0_OFST1 0x61006000
#define WDMA1_OFST0 0x66046620
#define WDMA1_OFST1 0x65006400

#define WDMA0_CFG_BASE 0x1fa00000
#define WDMA1_CFG_BASE 0x1fa00000


/*default usage, should get from pcie device*/
#define WPDMA_BASE_ADDR0 0x20004000
#define WPDMA_BASE_ADDR1 0x20104000
#endif /* ARHT_SOC */

#endif /* _WARP_AIROHA_H_ */
