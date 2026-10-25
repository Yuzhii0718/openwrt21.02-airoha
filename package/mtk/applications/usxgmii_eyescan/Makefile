#
# bc-bocun.chen@mediatek.com
#
# MTK Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=usxgmii_eyescan
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/usxgmii_eyescan
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  TITLE:=an program to sweep eye from usxgmii
  SUBMENU:=Applications
  DEPENDS:=+@KERNEL_DEVMEM
endef

define Package/usxgmii_eyescan/description
  Simple program to sweep eye from usxgmii
endef

define Build/Configure
endef

define Package/usxgmii_eyescan/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usxgmii_eyescan $(1)/usr/bin
endef


$(eval $(call BuildPackage,usxgmii_eyescan))

