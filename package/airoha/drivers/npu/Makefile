#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)npu
PKG_RELEASE:=1

# wlan_hwifi/bus/en_npu.c includes
#   "../../../npu/UNION_EN7563_MULTI_CHIP_KERNEL_5_4_demo/host/npu_rv32.h"
# i.e. it expects the vendor ko_module layout, one directory per board mak under
# $(KERNEL_BUILD_DIR)/npu. Build there so the relative include resolves without
# patching the vendor driver.
NPU_MAK_NAME:=$(basename $(notdir $(firstword $(wildcard $(TOPDIR)/target/linux/$(BOARD)/$(SUBTARGET)/*.mak))))
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/npu/$(NPU_MAK_NAME)

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set).
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define KernelPackage/npu
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet npu driver
  FILES:=$(PKG_BUILD_DIR)/host/npu.ko
  KCONFIG:=
endef

define KernelPackage/npu/description
  Kernel module for npu driver
endef

define KernelPackage/npu_wifi_offload
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet npu wifi offload driver
  DEPENDS:=kmod-npu +ecnt_prepare
  FILES:=$(PKG_BUILD_DIR)/hostadpt/hostadpt.ko
  KCONFIG:=
endef

define KernelPackage/npu_wifi_offload/description
  Kernel module for npu_wifi_offload driver
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)/host" \
		GLOBAL_INC_DIR="$(LINUX_DIR)/include/global_inc" \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		modules
ifneq ($(CONFIG_PACKAGE_kmod-npu_wifi_offload),)
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)/hostadpt" \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		modules
endif
endef

# npu_data.bin / npu_rv32.bin are the NPU runtime images consumed by the
# driver; they are vendor blobs, not buildable from source.
define KernelPackage/npu/install
	$(INSTALL_DIR) $(1)/userfs
	$(INSTALL_BIN) ./files/npu_data.bin ./files/npu_rv32.bin $(1)/userfs/
endef

$(eval $(call KernelPackage,npu))
$(eval $(call KernelPackage,npu_wifi_offload))
