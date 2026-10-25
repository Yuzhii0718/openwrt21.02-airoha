#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)ae_wan
PKG_RELEASE:=1

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set).
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ae_wan
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet ae_wan driver
  DEPENDS:=kmod-ether kmod-tccicmd
  FILES:=$(PKG_BUILD_DIR)/ae_wan.ko
  KCONFIG:=
endef

define KernelPackage/ae_wan/description
  Kernel module for ae_wan driver
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)" \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,ae_wan))
