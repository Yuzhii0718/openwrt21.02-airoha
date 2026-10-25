#ifndef __ETH_RTL8372N_H__
#define __ETH_RTL8372N_H__
int rtl8372n_switch_init(void);
void rtl8372_mdio_write(unsigned int phy_addr, unsigned int phy_reg, unsigned int phy_data);
void rtl8372_mdio_read(unsigned int phy_addr, unsigned int phy_reg, unsigned int* pData);
void rtl8372_mdio_read_h(unsigned int phy_addr, unsigned int phy_reg, unsigned short *pData);
#endif
