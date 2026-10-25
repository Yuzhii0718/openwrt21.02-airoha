include $(TOPDIR)/rules.mk
PKG_NAME:=wps_pbcd
PKG_RELEASE:=1
include $(INCLUDE_DIR)/package.mk

define Package/wps_pbcd
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=wps pbcd
	DEPENDS:=+dai +ubus
	SUBMENU:=Applications
	MENU:=1
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	[ ./files/wps_pbcd ] && $(CP) ./files/wps_pbcd $(PKG_BUILD_DIR)
endef
define Package/wps_pbcd/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,wps_pbcd))
