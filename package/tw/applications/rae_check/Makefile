include $(TOPDIR)/rules.mk
PKG_NAME:=rae_check
PKG_RELEASE:=1
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=Routing analysis engine management
	DEPENDS:= +fwk +libkvcutil +libuci +dai +libjson-c +libcurl +libiconv +libubox +libubus
	SUBMENU:=Applications
	MENU:=1
endef

define Build/InstallDev
	mkdir -p $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libfwk.so"; echo "libkvcutil.so"; echo "libuci.so"; echo "libdai.so"; echo "libjson-c.so"; echo "libcurl.so"; echo "libcrypto.so.1.0.0"; echo "libssl.so.1.0.0"; echo "libiconv.a"; echo "libblobmsg_json.so.20210516"; echo "libubox.so"; echo "libubus.so";
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	[ ./files/rae_check ] && $(CP) ./files/rae_check $(PKG_BUILD_DIR)
endef
define Package/rae_check/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,rae_check))
