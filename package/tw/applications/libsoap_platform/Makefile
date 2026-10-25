include $(TOPDIR)/rules.mk
PKG_NAME:=libsoap_platform
PKG_VERSION:=20201206
include $(INCLUDE_DIR)/package.mk

define Package/libsoap_platform
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=library for soap interface(only platform related)
	DEPENDS:=+fwk +dai +speedtest +curl +libubus +libblobmsg-json +libjson-c +libjson-script +uhttpd_server
	SUBMENU:=Applications
	MENU:=1
endef

define Package/libsoap_platform/description
	Provides the basic RPC infrastructure to run soap.
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libblobmsg_json.so"; echo "libblobmsg_json.so.20210516"; echo "libjson-c.so"; echo "libjson-c.so.5"; echo "libmbedcrypto.so"; echo "libmbedcrypto.so.3"; echo "libmbedtls.so"; echo "libmbedtls.so.12"; echo "libssl.so.1.1"; echo "libmbedx509.so.0" ; echo "libubox.so" ; echo "libubus.so" ; echo "libubox.so.20210516"
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	[ -d ./files/include ] && $(CP) -rf ./files/include $(PKG_BUILD_DIR)
	$(CP) ./files/*.so $(PKG_BUILD_DIR)
endef
# /usr/sbin/uhttpd_server used to be installed from a second copy of the
# prebuilt binary that was floating around in files/. The same binary is the
# whole point of the uhttpd_server package (now built from source), so depend on
# it instead of shipping a stale duplicate.
define Package/libsoap_platform/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,libsoap_platform))
