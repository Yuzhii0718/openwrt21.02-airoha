#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)tc3162l2hp2h
PKG_RELEASE:=1

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set) that the driver sources and
# its Kbuild Makefile depend on. See include/ecnt-bspflags.mk.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

ifeq ($(strip $(TCSUPPORT_KERNEL_API)),)
TC3162_FILES:=$(PKG_BUILD_DIR)/module_sel.ko $(PKG_BUILD_DIR)/tcledctrl.ko
else
TC3162_FILES:=$(PKG_BUILD_DIR)/module_sel.ko
endif

define KernelPackage/tcledctrl
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet tc3162l2hp2h(ledctrl)
  FILES:=$(TC3162_FILES)
  KCONFIG:=
endef

define KernelPackage/tcledctrl/description
  Kernel module for econet tcledctrl
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

$(eval $(call KernelPackage,tcledctrl))
