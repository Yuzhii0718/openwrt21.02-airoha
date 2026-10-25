include $(TOPDIR)/rules.mk
PKG_NAME:=fwcheck
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
include $(INCLUDE_DIR)/package.mk

define Package/fwcheck
	SECTION:=fwcheck
	CATEGORY:=Netgear
	TITLE:=FwCheck
	DEPENDS:=+libpthread +libc +d2 +libopenssl +libcurl +dil
endef

define Package/fwcheck/description
	Controlled FW rollout
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA) ./files/include/* $(1)/usr/include
	
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) ./files/lib/*.so $(1)/usr/lib
	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/bin/caller $(1)/usr/bin
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define Package/fwcheck/install
	tar -zxvf files/data.tar.gz -C $(1)/
endef
$(eval $(call BuildPackage,fwcheck))
