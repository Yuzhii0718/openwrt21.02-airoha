include $(TOPDIR)/rules.mk

PKG_NAME:=uhttpd_server
PKG_VERSION:=2020-5-13
PKG_RELEASE:=1

PKG_SOURCE:=uhttpd_server-2020-5-13-5e9c23c6.tar.xz
PKG_SOURCE_URL:=
PKG_HASH:=4b3b8ee93681b035d45388dd9825a9baa0680fe91ed5efc47e6f1807b6b71224
PKG_SOURCE_VERSION:=5e9c23c6f40ff26209ef22cfeeda4904a5918f3d
PKG_SOURCE_DATE:=2020-5-13
PKG_BUILD_DIR:=$(BUILD_DIR)/uhttpd_server-2020-5-13-5e9c23c6

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/uhttpd_server
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=uhttpd_server
	MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
	DEPENDS:=+libubox +libblobmsg-json +libjson-script +libjson-c +ngr_uhttpd
	SUBMENU:=Applications
	MENU:=1
endef

# The prebuilt payload carried a 42 KB stripped uhttpd built with TLS, Lua and
# ubus disabled (its NEEDED list has only libubox, libjson_script,
# libblobmsg_json and libjson-c), so the source build keeps exactly those
# three features off.
CMAKE_OPTIONS += \
	-DTLS_SUPPORT=OFF \
	-DLUA_SUPPORT=OFF \
	-DUBUS_SUPPORT=OFF

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include
endef

define Package/uhttpd_server/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd $(1)/usr/sbin/uhttpd_server
endef

$(eval $(call BuildPackage,uhttpd_server))
