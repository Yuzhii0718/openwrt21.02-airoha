include $(TOPDIR)/rules.mk
PKG_NAME:=ntpot
PKG_RELEASE:=1
PKG_VERSION:=2020-05-10
include $(INCLUDE_DIR)/package.mk

define Package/ntpot
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=Netgear POT
	DEPENDS:=+ubus +ubox +fwk +dai
	SUBMENU:=Applications
	MENU:=1
endef

define Package/ntpot/description
	This package is a demonstration of generate-package
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	[ ./files/ntpot ] && $(CP) ./files/ntpot $(PKG_BUILD_DIR)
endef
define Package/ntpot/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,ntpot))
