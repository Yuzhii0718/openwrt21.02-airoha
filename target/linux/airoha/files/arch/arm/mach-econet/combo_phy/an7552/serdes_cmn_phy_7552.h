#ifndef _COMMON_PHY_7552
#define _COMMON_PHY_7552
struct an7552_serdes_common_phy {
	struct device *dev;
	void __iomem *Ana_csr_base; /* PON_HSGMII_PHYA physical address */
	void __iomem *Dig_csr_base; /* PON_HSGMII_PHYD physical address */
	void __iomem *Multi_sgmii_base; /* PON_HSGMII_multi PHY physical address */
	
};
#endif
