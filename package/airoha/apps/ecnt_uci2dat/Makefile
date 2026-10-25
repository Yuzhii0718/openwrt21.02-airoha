#
# Copyright (C) 2010-2013 hua.shao@mediatek.com
#
# MTK Property Software.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ecnt_uci2dat
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ecnt_uci2dat
  SECTION:=Airoha Properties
  CATEGORY:=Airoha Properties
  SUBMENU:=Applications
  TITLE:=ecnt_uci2dat
  DEPENDS:=+libc uci
endef

define Package/ecnt_uci2dat/description
	Translate uci config into ralink wifi dat format
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DTRENDCHIP \
		-c -o $(PKG_BUILD_DIR)/uci2dat.o $(PKG_BUILD_DIR)/uci2dat.c
	$(TARGET_CC) $(TARGET_LDFLAGS) -o $(PKG_BUILD_DIR)/ecnt_uci2dat \
		$(PKG_BUILD_DIR)/uci2dat.o -luci
endef

define Package/ecnt_uci2dat/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ecnt_uci2dat $(1)/sbin/ecnt_uci2dat
endef

$(eval $(call BuildPackage,ecnt_uci2dat))
