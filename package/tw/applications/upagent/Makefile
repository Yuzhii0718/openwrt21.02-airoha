include $(TOPDIR)/rules.mk
PKG_NAME:=upagent
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
include $(INCLUDE_DIR)/package.mk

define Package/upagent
	SECTION:=upagent
	CATEGORY:=Netgear
	TITLE:=UpAgent
	DEPENDS:=+libpthread +libc +d2 +libopenssl +libcurl +dil
endef

define Package/upagent/description
	UpAgent for Orbi
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/upagent/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,upagent))
