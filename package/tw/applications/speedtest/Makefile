include $(TOPDIR)/rules.mk
PKG_NAME:=speedtest
PKG_RELEASE:=1
include $(INCLUDE_DIR)/package.mk

define Package/speedtest
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=speedtest interface
	DEPENDS:=+fwk
	SUBMENU:=Applications
	MENU:=1
endef

define Package/speedtest/description
	Speed Test
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/$(PKG_NAME)
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libspeedtest.so $(1)/usr/lib/
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libstdc++.so.6"
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	$(CP) ./files/*.so $(PKG_BUILD_DIR)
endef
define Package/speedtest/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,speedtest))
