include $(TOPDIR)/rules.mk
PKG_NAME:=ngr_uhttpd
PKG_RELEASE:=31
include $(INCLUDE_DIR)/package.mk

define Package/ngr_uhttpd
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=ngr_uhttpd
	SUBMENU:=Applications
	MENU:=1
	DEPENDS:=+fwk +dai
endef

define Package/ngr_uhttpd/description
	uHTTPd is a tiny single threaded HTTP server with TLS, CGI and Lua
	support. It is intended as a drop-in replacement for the Busybox
	HTTP daemon.
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libubox.so.20210516"; echo "libuci.so"; echo "libfwk.so"; echo "libdai.so"; echo "libkvcutil.so.1.0.0.0"; echo "libcrypto.so.1.1"; echo "libssl.so.1.1"; echo "libwolfssl.so.4.8.1.66253b90"; echo "libblobmsg_json.so.20210516"; echo "libubus.so.20210630"
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	$(CP) ./files/*.so $(PKG_BUILD_DIR)
endef
define Package/ngr_uhttpd/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,ngr_uhttpd))
