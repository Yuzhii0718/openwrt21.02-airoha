include $(TOPDIR)/rules.mk
PKG_NAME:=xagent
PKG_VERSION:=3.2.7
include $(INCLUDE_DIR)/package.mk

define Package/xagent
	SECTION:=dal
	CATEGORY:=dal
	TITLE:=Netgear xagent
	DEPENDS:=+libpthread +curl +libopenssl +d2
endef

define Package/xagent/description
	Netgear dal xagent
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/xagent/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,xagent))
