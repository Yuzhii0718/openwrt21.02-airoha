#
# sam.shih@mediatek.com
#
# MTK Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dps368_conveter
PKG_RELEASE:=1.0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/dps368_conveter
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  TITLE:=dps368_conveter
  SUBMENU:=Applications
  TARGET_LDFLAGS+=-lm
endef

define Package/dps368_conveter/description
  dps368_conveter:
  - dps368_attitude_example
endef

define Build/Configure
endef

define Package/dps368_conveter/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dps368_attitude_example $(1)/usr/bin
endef

$(eval $(call BuildPackage,dps368_conveter))
