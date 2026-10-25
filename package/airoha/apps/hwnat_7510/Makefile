#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hwnat_7510
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set).
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define Package/hwnat_7510
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=hwnat_7510
  DEPENDS:=+ecnt_api
endef

define Package/hwnat_7510/description
	Hardware NAT v2 control utility of Ecnt 752x/751x
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BSP_ROOTDIR=$(TOPDIR)/include \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DCONFIG_HNAT_V2 -DBIG_ENDIAN $(BSP_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		COMPILE_LIBS="-lapi_lib_utility -lapi_lib_ppe -lcompileoption_bsp"
endef

define Package/hwnat_7510/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hw_nat $(1)/usr/bin/
endef

$(eval $(call BuildPackage,hwnat_7510))
