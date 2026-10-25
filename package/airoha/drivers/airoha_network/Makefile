#
# Copyright (C) 2010-2013 airoha.com
#
# AIROHA Property Software.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=airoha_network
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/airoha_network
  SECTION:=Airoha Properties
  CATEGORY:=Airoha Properties
  SUBMENU:=Applications
  TITLE:=airoha_network
  DEPENDS:=+libc uci
endef

define Package/airoha_network/description
	This package contains airoha_network command for Arht 752x
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DTRENDCHIP \
		-I$(LINUX_DIR)/include/global_inc -I$(LINUX_DIR)/include/global_inc/trx \
		-c -o $(PKG_BUILD_DIR)/uci.o $(PKG_BUILD_DIR)/uci.c
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DTRENDCHIP \
		-I$(LINUX_DIR)/include/global_inc -I$(LINUX_DIR)/include/global_inc/trx \
		-c -o $(PKG_BUILD_DIR)/airoha_network.o $(PKG_BUILD_DIR)/airoha_network.c
	$(TARGET_CC) $(TARGET_LDFLAGS) -o $(PKG_BUILD_DIR)/airoha_network \
		$(PKG_BUILD_DIR)/uci.o $(PKG_BUILD_DIR)/airoha_network.o -luci
endef

define Package/airoha_network/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/airoha_network $(1)/sbin/airoha_network
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/pon.config $(1)/etc/config/pon
	$(INSTALL_DIR) $(1)/etc/init.d
ifeq ($(CONFIG_prplos_target),y)
	$(INSTALL_BIN) ./files/pon_prplos.init $(1)/etc/init.d/pon
else
	$(INSTALL_BIN) ./files/pon.init $(1)/etc/init.d/pon
endif
endef

$(eval $(call BuildPackage,airoha_network))
