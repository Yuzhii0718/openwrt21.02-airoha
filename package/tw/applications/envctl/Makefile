include $(TOPDIR)/rules.mk
PKG_NAME:=envctl
PKG_RELEASE:=1
include $(INCLUDE_DIR)/package.mk

define Package/envctl
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=Read/Write Factory/User ENV
	DEPENDS:=+dai +fwk +libnvram
	SUBMENU:=Applications
	MENU:=1
endef

define Package/envctl/description
	Read/Write MTK nvram profiles
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	[ ./files/envctl ] && $(CP) ./files/envctl $(PKG_BUILD_DIR)
endef
define Package/envctl/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,envctl))
