#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)whnat
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set); the vendor Makefile applies it
# through MODULE_FLAGS.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define KernelPackage/whnat
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet whnat (SWNAT / WOE) driver
  DEPENDS:=+kmod-qdma_lan
  FILES:=$(PKG_BUILD_DIR)/mt_whnat.ko
  KCONFIG:=
endef

define KernelPackage/whnat/description
  Wireless offload engine (mt_whnat.ko): hooks the wifi tx/rx path and the WDMA
  queues so SWNAT traffic is handed to hardware. preinit loads it whenever the
  board mak has TCSUPPORT_SWNAT set.
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
		MODULE_FLAGS="$(BSP_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,whnat))
