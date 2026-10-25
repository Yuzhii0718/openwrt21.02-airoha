include $(TOPDIR)/rules.mk
PKG_NAME:=llmnrd
PKG_RELEASE:=1
PKG_VERSION:=2020-05-25
include $(INCLUDE_DIR)/package.mk

define Package/llmnrd
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=llmnrd mDNS
	SUBMENU:=Applications
	MENU:=1
endef

define Package/llmnrd/description
	This package is a demonstration of generate-package
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	[ ./files/llmnrd ] && $(CP) ./files/llmnrd $(PKG_BUILD_DIR)
endef
define Package/llmnrd/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,llmnrd))
