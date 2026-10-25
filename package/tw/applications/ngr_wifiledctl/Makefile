include $(TOPDIR)/rules.mk
PKG_NAME:=wifiledctl
PKG_RELEASE:=1
PKG_VERSION:=2020-06-04
include $(INCLUDE_DIR)/package.mk

define Package/wifiledctl
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=Netgear wifi led control
	DEPENDS:=+ubus +ubox +fwk +dai
	SUBMENU:=Applications
	MENU:=1
endef

define Package/wifiledctl/description
	This package is a demonstration of generate-package
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	[ ./files/wifiledctl ] && $(CP) ./files/wifiledctl $(PKG_BUILD_DIR)
endef
define Package/wifiledctl/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,wifiledctl))
