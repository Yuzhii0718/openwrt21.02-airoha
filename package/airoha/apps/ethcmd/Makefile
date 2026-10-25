#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ethcmd
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set); the vendor Makefile uses
# TCSUPPORT_MT7530_SWITCH_API to decide whether libmtkswitch is built.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define Package/ethcmd
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=ethcmd
  DEPENDS:=+compileoption_lib
endef

define Package/ethcmd/description
	This package contains ethcmd command for Ecnt 752x
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

# Build the shared library before the command: ethphxcmd links against
# libmtkswitch through -L. -lmtkswitch, and the .so symlinks are only created
# by the libmtkswitch.so.1.0 rule.
define Build/Compile
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BSP_ROOTDIR=$(TOPDIR)/include \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -Wall $(BSP_CFLAGS)" \
		libmtkswitch.so.1.0 ethphxcmd
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmtkswitch.so* $(1)/usr/lib/
endef

define Package/ethcmd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ethphxcmd $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libmtkswitch.so.1.0 $(1)/lib/
	$(LN) libmtkswitch.so.1.0 $(1)/lib/libmtkswitch.so
	$(LN) libmtkswitch.so.1.0 $(1)/lib/libmtkswitch.so.1
endef

$(eval $(call BuildPackage,ethcmd))
