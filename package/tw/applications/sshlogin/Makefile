include $(TOPDIR)/rules.mk
PKG_NAME:=sshlogin
PKG_RELEASE:=1
PKG_VERSION:=2021-12-14
include $(INCLUDE_DIR)/package.mk

define Package/sshlogin
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=Netgear sshlogin
	DEPENDS:=+ubus +ubox +fwk +dai
	SUBMENU:=Applications
	MENU:=1
endef

define Package/$(PKG_NAME)/extra_provides
	echo "libfwk.so"; echo "libdai.so"
endef

define Package/sshlogin/description
	This package is a demonstration of generate-package
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	[ ./files/sshlogin ] && $(CP) ./files/sshlogin $(PKG_BUILD_DIR)
endef
define Package/sshlogin/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,sshlogin))
