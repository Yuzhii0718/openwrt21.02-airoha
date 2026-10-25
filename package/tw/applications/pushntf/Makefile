include $(TOPDIR)/rules.mk
PKG_NAME:=pushntf
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
include $(INCLUDE_DIR)/package.mk

define Package/pushntf
	SECTION:=pushntf
	CATEGORY:=Netgear
	TITLE:=PNTF
	DEPENDS:=+libpthread +librt +libc +d2 +dil
endef

define Package/pushntf/description
	Netgear Data Abstraction Layer 2
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	
	$(CP) -rp ./files/include/* $(1)/usr/include
	
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/pushntf/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,pushntf))
