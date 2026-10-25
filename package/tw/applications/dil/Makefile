include $(TOPDIR)/rules.mk
PKG_NAME:=dil
PKG_RELEASE:=1
PKG_VERSION:=0.1.0
include $(INCLUDE_DIR)/package.mk

define Package/dil
	SECTION:=dil
	CATEGORY:=Netgear
	TITLE:=Dil
	DEPENDS:=+libpthread +librt +libc +d2 +libopenssl +libcurl
endef

define Package/dil/description
	Netgear Data Abstraction Layer 2
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/dil/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,dil))
