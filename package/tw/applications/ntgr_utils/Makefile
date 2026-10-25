include $(TOPDIR)/rules.mk
PKG_NAME:=ntgr_utils
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
include $(INCLUDE_DIR)/package.mk

define Package/ntgr_utils
	SECTION:=ntgr_utils
	CATEGORY:=Netgear
	TITLE:=Utilities
	#DEPENDS:=+libpthread +libc +d2 +libopenssl +libcurl +dil
endef

define Package/ntgr_utils/description
	NTGR Utilities library for DAL based SKUs
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/ntgr_utils/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,ntgr_utils))
