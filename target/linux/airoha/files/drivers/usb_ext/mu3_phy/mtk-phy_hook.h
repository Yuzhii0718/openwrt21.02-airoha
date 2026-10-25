#ifndef MTK_PHY_HOOK_H
#define MTK_PHY_HOOK_H

struct mtk_usb_phy_ops {
	int (*init)(void);
	int (*PowerDown)(unsigned int port_num);
	int (*PowerUp)(unsigned int port_num);
};

int mtk_usb_phy_ops_init(const struct mtk_usb_phy_ops *ops_ptr);

#endif /*MTK-PHY_HOOK_H*/