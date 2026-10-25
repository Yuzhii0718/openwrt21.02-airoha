#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=compileoption_lib
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set). The library records those
# switches into the TCSUPPORT_*_MVAL tables, so they must match the kernel.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define Package/compileoption_lib
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  TITLE:=compileoption_lib
  DEPENDS:= +libc +ecnt_prepare
  SUBMENU:=Applications
endef

define Package/compileoption_lib/description
	This package contains some APIs and CMDs for Ecnt 752x/751x
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -fPIC -Os -g -Wall -shared $(BSP_CFLAGS)" \
		DSOFLAGS="$(TARGET_LDFLAGS)"
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/libcompileoption.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcompileoption_bsp.so* $(1)/usr/lib/
endef

define Package/compileoption_lib/install
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libcompileoption_bsp.so $(1)/lib/
endef

$(eval $(call BuildPackage,compileoption_lib))
