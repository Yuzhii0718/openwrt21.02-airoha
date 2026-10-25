#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ecnt_wan
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS and the GLOBAL_INC_DIR based include paths.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define Package/ecnt_wan
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=ecnt_wan
  DEPENDS:=+libuci ecnt_api
endef

define Package/ecnt_wan/description
	This package contains wan mode helper of Ecnt 752x/751x
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BSP_ROOTDIR=$(TOPDIR)/include \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DTRENDCHIP $(BSP_CFLAGS)" \
		INCLUDE="-I$(TOOLS_TRX_DIR) -I$(GLOBAL_FLASH_LAYOUT_INC_DIR)"
endef

define Package/ecnt_wan/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ecnt_wan $(1)/sbin/
endef

$(eval $(call BuildPackage,ecnt_wan))
