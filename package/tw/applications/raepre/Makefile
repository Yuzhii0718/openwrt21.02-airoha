include $(TOPDIR)/rules.mk
PKG_NAME:=raepre
PKG_RELEASE:=1
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=Routing analysis engine
	DEPENDS:= +fwk +libkvcutil +libuci +dai +libjson-c +libxml2
	SUBMENU:=Applications
	MENU:=1
endef

define Build/InstallDev
	@echo "InstallDev $(PKG_NAME) path =$(1)"
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/install/usr/sbin/rae $(1)/usr/sbin/rae
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libfwk.so"; echo "libkvcutil.so"; echo "libuci.so"; echo "libdai.so"; echo "libjson-c.so"; echo "libcrypto.so.1.0.0"; echo "libssl.so.1.0.0"; echo "libxml2.so"; echo "libblobmsg_json.so"; echo "libspeedtest.so"; echo "libubus.so";
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/raepre/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,raepre))
