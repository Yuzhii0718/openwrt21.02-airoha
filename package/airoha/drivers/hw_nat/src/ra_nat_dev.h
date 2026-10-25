#ifndef _RA_NAT_DEV
#define _RA_NAT_DEV

#define FOE_INFO_LEN		    7

#define FOE_MAGIC_PCI		    0x7273
#define FOE_MAGIC_WLAN		    0x7274
#define FOE_MAGIC_GE		    0x7275
#define FOE_MAGIC_PPE		    0x7276

#define IS_SPACE_AVAILABLED(skb)    1
#define FOE_INFO_START_ADDR(skb)    (&(skb)->foe[0])
#define FOE_MAGIC_TAG(skb)          ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->MAGIC_TAG
#define FOE_ENTRY_NUM(skb)          ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->FOE_Entry
#define FOE_FVLD(skb)               ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->FVLD
#define FOE_UDF(skb)                ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->UDF
#define FOE_AI(skb)                 ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->CRSN
#define FOE_SP(skb)                 ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->SPORT //src_port or user priority
#define FOE_AIS(skb)                ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->AIS
#define FOE_Hit_IFC(skb)            ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->ifc_hit
#define FOE_IFC_IDX(skb)            ((PdmaRxDescInfo4 *)(&(skb)->foe[0]))->ifc_idx

#define HWNAT_DEFAULT_MTRGRP 0x7F
#define HWNAT_DEFAULT_ACNT0GRP 0x1F
#define HWNAT_DEFAULT_ACNT1GRP 0x1F
#define HWNAT_DEFAULT_GRP_ID 0x3F

#define IS_MAGIC_TAG_VALID(skb)     ((FOE_MAGIC_TAG(skb) == FOE_MAGIC_GE)   || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_PTM_LAN) ||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_XSI) ||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN_7615) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_WLAN_TX) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_ATM) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_PTM) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_EPON) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_E_1) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_D_1) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_E_2) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_CRYPTO_D_2) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_L2TP_VPN_UPSTREAM) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_L2TP_VPN_DOWNSTREAM) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_LOCAL) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_LOCAL_MCAST) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_UP_1) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_DOWN_1) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_UP_2) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_DOWN_2) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_UP_1) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_DOWN_1) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_UP_2) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_DOWN_2) || \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_GPON)|| \
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_AE_WAN)||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_UP_FRAG)||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_GRE_HWDOWN_1)||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_VXLAN_HWDOWN_1)||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_L2TP_HWDOWN_1)||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_XSI_GDM4) ||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_TR471_TEST_UPSTREAM) ||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_TR471_HW_TEST_UPSTREAM) ||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_OLT) ||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_TR471_NPU_DOWNSTREAM) ||\
                    (FOE_MAGIC_TAG(skb) == FOE_MAGIC_TR471_NPU_UPSTREAM))


#define HWNAT_BINDFAIL_DPRINT(x, args...) if((FOE_ENTRY_NUM(skb) == hwnat_dbg_bindfail_idx)&&printk_ratelimit()) printk("%s,%d:" x, __FUNCTION__,__LINE__,##args)

#define PPESETPREBIND(x)	((x)->udib1.preb = 1)

typedef enum
{
    FAST_PATH_DIR_WAN = 0,
    FAST_PATH_DIR_LAN,
    FAST_PATH_DIR_XSI_PCIE0,
    FAST_PATH_DIR_XSI_PCIE1,
    FAST_PATH_DIR_XSI_USB,
    FAST_PATH_DIR_XSI_ETHER,
    FAST_PATH_DIR_XSI_PON,
    FAST_PATH_DIR_OLT,
    FAST_PATH_DIR_MAX_NUM,
}FAST_PATH_DIR;

#define QDMAINIT_FAST_PATH_OFFSET  8

#endif
