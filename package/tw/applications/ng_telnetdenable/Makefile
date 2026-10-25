include $(TOPDIR)/rules.mk
PKG_NAME:=telnetdEnable
PKG_RELEASE:=1
include $(INCLUDE_DIR)/package.mk

define Package/telnetdEnable
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=Connected telnetdEnable
	DEPENDS:=+libubus +libubox +uci +fwk +dai
	SUBMENU:=Applications
	MENU:=1
endef

define Package/telnetdEnable/description
	Connected device probe
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libblobmsg_json.so.20210516"; echo "libfwk.so";
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files/*.h $(PKG_BUILD_DIR)
	[ ./files/telnetdEnable ] && $(CP) ./files/telnetdEnable $(PKG_BUILD_DIR)
endef
define Package/telnetdEnable/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,telnetdEnable))
