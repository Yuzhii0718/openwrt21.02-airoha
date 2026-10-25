include $(TOPDIR)/rules.mk
PKG_NAME:=bst
PKG_RELEASE:=1
PKG_VERSION:=0.1.0
include $(INCLUDE_DIR)/package.mk

define Package/bst
	SECTION:=bst
	CATEGORY:=Netgear
	TITLE:=BST
	DEPENDS:=+libpthread +librt +libc +d2 +libopenssl +libcurl +dil
endef

define Package/bst/description
	Netgear Data Abstraction Layer 2
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	
	$(CP) -rp ./files/include/* $(1)/usr/include
	$(CP) -rp ./files/usr/lib/*.so $(1)/usr/lib
	
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/bst/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,bst))
