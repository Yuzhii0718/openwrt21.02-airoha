#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)lro
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/lro

# Provides BSP_CFLAGS with the TCSUPPORT_* defines the driver expects.
include $(INCLUDE_DIR)/ecnt-bspflags.mk
include $(INCLUDE_DIR)/package.mk

# lro_lan and lro_wan share byte-identical sources; only the vendor Makefile
# differs (Makefile_lan adds -DLRO_LAN and the qdma_lan include paths,
# Makefile_wan only the qdma_wan ones). Both are built here, each in its own
# sub-directory, so the source tree exists once.
define KernelPackage/lro_lan
  CATEGORY:=Airoha Properties
  SUBMENU:=ARHT_Drivers
  TITLE:=Airoha lro lan driver
  DEPENDS:=kmod-qdma_lan
  FILES:=$(PKG_BUILD_DIR)/lan/lro_lan.ko
  KCONFIG:=
endef

define KernelPackage/lro_lan/description
  Large receive offload for the LAN path, built from the shared lro sources with
  -DLRO_LAN and the qdma_lan headers.
endef

define KernelPackage/lro_wan
  CATEGORY:=Airoha Properties
  SUBMENU:=ARHT_Drivers
  TITLE:=Airoha lro wan driver
  DEPENDS:=kmod-qdma_wan
  FILES:=$(PKG_BUILD_DIR)/wan/lro_wan.ko
  KCONFIG:=
endef

define KernelPackage/lro_wan/description
  Large receive offload for the WAN path, built from the shared lro sources with
  the qdma_wan headers.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/lan $(PKG_BUILD_DIR)/wan
	$(CP) ./src/. $(PKG_BUILD_DIR)/lan/
	$(CP) ./src/. $(PKG_BUILD_DIR)/wan/
	$(CP) $(PKG_BUILD_DIR)/lan/Makefile_lan $(PKG_BUILD_DIR)/lan/Makefile
	$(CP) $(PKG_BUILD_DIR)/wan/Makefile_wan $(PKG_BUILD_DIR)/wan/Makefile
	$(CP) $(PKG_BUILD_DIR)/lan/lro_7552.h $(PKG_BUILD_DIR)/lan/lro.h
	$(CP) $(PKG_BUILD_DIR)/wan/lro_7552.h $(PKG_BUILD_DIR)/wan/lro.h
	echo 'EXTRA_CFLAGS += $$(BSP_CFLAGS)' >> $(PKG_BUILD_DIR)/lan/Makefile
	echo 'EXTRA_CFLAGS += $$(BSP_CFLAGS)' >> $(PKG_BUILD_DIR)/wan/Makefile
endef

define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)/lan" \
		BSP_ROOTDIR=$(TOPDIR)/include \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		OPENWRT_BUILD_ROOT=$(KERNEL_BUILD_DIR)/qdma \
		OPENWRT_BUILD_LRO_LAN=$(KERNEL_BUILD_DIR)/qdma \
		modules
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)/wan" \
		BSP_ROOTDIR=$(TOPDIR)/include \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		OPENWRT_BUILD_ROOT=$(KERNEL_BUILD_DIR)/qdma \
		OPENWRT_BUILD_LRO_WAN=$(KERNEL_BUILD_DIR)/qdma \
		modules
endef

$(eval $(call KernelPackage,lro_lan))
$(eval $(call KernelPackage,lro_wan))
