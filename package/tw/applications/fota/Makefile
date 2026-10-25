include $(TOPDIR)/rules.mk
PKG_NAME:=fota
PKG_RELEASE:=0
include $(INCLUDE_DIR)/package.mk

define Package/fota
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=custom lib for data interface
	DEPENDS:=+fwk +curl +dai +ubus 
	SUBMENU:=Applications
	MENU:=1
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libfwcheck.so"; echo "libmbedcrypto.so.3"; echo "libmbedtls.so.12"; echo "libcjson.so.1"; echo "libd2lib.so"; echo "libdjson.so"; echo "libmbedx509.so.0"; echo "libntgr_util.so"
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	[ -d ./files/include ] && $(CP) -rf ./files/include $(PKG_BUILD_DIR)
	$(CP) ./files/*.so $(PKG_BUILD_DIR)
	[ ./files/fota ] && $(CP) ./files/fota $(PKG_BUILD_DIR)
endef
define Package/fota/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,fota))
