#ifndef _RA_NAT_NPU_H
#define _RA_NAT_NPU_H

#define FE_BASE                 0xBFB50000
#define CDMA3_BASE              (FE_BASE + 0x1800)
#define CDMA3_FWD_CFG           (CDMA3_BASE + 0x08)
#define CDMA3_TSO_SHPR_CFG      (CDMA3_BASE + 0x8c)
#define CDMA3_TSO_FLAG_MASK     (CDMA3_BASE + 0x90)
#define CDMA3_TX_OK_CNT         (CDMA3_BASE + 0x180)
#define CDMA3_RXCPU_KA_CNT      (CDMA3_BASE + 0x18c)
#define CDMA3_RX_OQ4_OK_CNT     (CDMA3_BASE + 0x194)
#define CDMA3_RX_OQ5_OK_CNT     (CDMA3_BASE + 0x19c)
#define CDMA3_RX_OQ4_DROP_CNT   (CDMA3_BASE + 0x1a4)
#define CDMA3_RX_OQ5_DROP_CNT   (CDMA3_BASE + 0x1ac)
#define CDMA3_RX_OQ0_OK_CNT     (CDMA3_BASE + 0x1b0)
#define CDMA3_RX_OQ1_OK_CNT     (CDMA3_BASE + 0x1b4)
#define CDMA3_RX_OQ2_OK_CNT     (CDMA3_BASE + 0x1b8)
#define CDMA3_RX_OQ3_OK_CNT     (CDMA3_BASE + 0x1bc)
#define CDMA3_RX_OQ0_DROP_CNT   (CDMA3_BASE + 0x1d0)
#define CDMA3_RX_OQ1_DROP_CNT   (CDMA3_BASE + 0x1d4)
#define CDMA3_RX_OQ2_DROP_CNT   (CDMA3_BASE + 0x1d8)
#define CDMA3_RX_OQ3_DROP_CNT   (CDMA3_BASE + 0x1dc)
#define CDMA3_RX_OQ6_OK_CNT     (CDMA3_BASE + 0x2d0)
#define CDMA3_RX_OQ7_OK_CNT     (CDMA3_BASE + 0x2d4)
#define CDMA3_RX_OQ6_DROP_CNT   (CDMA3_BASE + 0x2e0)
#define CDMA3_RX_OQ7_DROP_CNT   (CDMA3_BASE + 0x2e4)


#define NPU_OFFLOAD_VXLAN   1<<0
extern unsigned int npu_tunnel_offload_proto;

#endif

