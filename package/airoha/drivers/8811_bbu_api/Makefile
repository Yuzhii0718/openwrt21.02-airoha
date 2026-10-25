#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)8811_bbu_api
PKG_RELEASE:=1
# Keep this out of $(KERNEL_BUILD_DIR)/an8811: the an8811 package wipes its
# own build directory during Prepare, which would delete a nested one.
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS with the TCSUPPORT_CPU_* defines the driver expects.
include $(INCLUDE_DIR)/ecnt-bspflags.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/8811_bbu_api
  CATEGORY:=Airoha Properties
  SUBMENU:=ARHT_Drivers
  TITLE:=ARHT 8811_bbu_api driver
  # This module resolves en8811_phy_lock from air_en8811h.ko and mdio_priv_p
  # from mdio_arht.ko, so it is only usable on boards with the AN8811 PHY.
  # @PACKAGE_kmod-air_en8811h hides the option there (e.g. the ubi-nowifi
  # board has no AN8811) instead of failing package/install later.
  DEPENDS:=@PACKAGE_kmod-air_en8811h +kmod-air_en8811h +kmod-mdio_arht
  FILES:=$(PKG_BUILD_DIR)/8811_bbu_api.ko
  KCONFIG:=
endef

define KernelPackage/8811_bbu_api/description
  Kernel module for 8811_bbu_api driver
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
		BSP_ROOTDIR=$(TOPDIR)/include \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,8811_bbu_api))
