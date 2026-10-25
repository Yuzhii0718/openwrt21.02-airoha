include $(TOPDIR)/rules.mk
PKG_NAME:=d2
PKG_RELEASE:=1
PKG_VERSION:=0.1.0
include $(INCLUDE_DIR)/package.mk

define Package/d2
	SECTION:=d2
	CATEGORY:=Netgear
	TITLE:=D2
	DEPENDS:=+libpthread +libnghttp2 +librt +libc +zlib +ntgr_utils
endef

define Package/d2/description
	Netgear Data Abstraction Layer 2
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	
	$(CP) -rp ./files/include/* $(1)/usr/include
	$(CP) -rp ./files/lib/*.so $(1)/usr/lib
	
endef

define Package/d2/extra_provides
	echo "libssl.so.1.0.0"; echo "libdjson.so"; echo "libd2dynlog.so"; echo "libntgr_util.so";
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/d2/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,d2))
