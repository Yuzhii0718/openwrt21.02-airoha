#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ecnt_prepare
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ecnt_prepare
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=ecnt_prepare
  DEPENDS:=
endef

define Package/ecnt_prepare/description
	This package installs the BSP profile (build switch set) that userspace
	tools query to find out how the firmware was built
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define Package/ecnt_prepare/install
	$(INSTALL_DIR) $(1)/userfs
	$(CP) ./files/profile.cfg $(1)/userfs/profile.cfg
	$(SED) 's/^BSP =y/BSP=y/' $(1)/userfs/profile.cfg
endef

$(eval $(call BuildPackage,ecnt_prepare))
