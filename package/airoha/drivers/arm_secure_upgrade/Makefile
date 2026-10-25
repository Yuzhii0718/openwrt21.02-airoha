#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)arm_secure_upgrade
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set); the vendor Makefile picks
# smccc-call-32.S vs -64.S from TCSUPPORT_CPU_ARMV8_64, i.e. from these flags.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define KernelPackage/arm_secure_upgrade
  CATEGORY:=Airoha Properties
  SUBMENU:=ARHT_Drivers
  TITLE:=Airoha arm_secure_upgrade driver
  FILES:=$(PKG_BUILD_DIR)/secureupgrade.ko
  KCONFIG:=
endef

define KernelPackage/arm_secure_upgrade/description
  SMCCC based secure-upgrade helper (secureupgrade.ko). preinit does not load it
  on purpose ("secureupgrade.ko is intentionally not loaded"); the module is
  packaged so it is available when a product enables secure boot upgrades.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	echo 'EXTRA_CFLAGS += $$(BSP_CFLAGS)' >> $(PKG_BUILD_DIR)/Makefile
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

$(eval $(call KernelPackage,arm_secure_upgrade))
