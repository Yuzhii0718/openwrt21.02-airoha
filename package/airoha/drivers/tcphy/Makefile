#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)tcphy
PKG_RELEASE:=1

# Provides BSP_CFLAGS and the TCSUPPORT_* make variables (TCSUPPORT_CPU_AN7552,
# TCSUPPORT_ETHPHY, ...) that select the PHY variant in the vendor Kbuild file.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define KernelPackage/tcphy
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet tcphy driver
  DEPENDS:=kmod-ether kmod-tccicmd
  FILES:=$(PKG_BUILD_DIR)/eth_ephy.ko
  KCONFIG:=
endef

define KernelPackage/tcphy/description
  Kernel module for tcphy driver
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

$(eval $(call KernelPackage,tcphy))
