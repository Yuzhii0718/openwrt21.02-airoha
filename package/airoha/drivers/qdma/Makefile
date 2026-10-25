#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)qdma
PKG_RELEASE:=1

# Provides BSP_CFLAGS and the TCSUPPORT_* make variables used by the vendor
# Kbuild files (TCSUPPORT_TSO_ENABLE, TCSUPPORT_CPU_AN7552, ...).
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qdma_wan
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet qdma wan driver
  FILES:=$(PKG_BUILD_DIR)/qdma/qdma_wan/qdma_wan.ko
  KCONFIG:=
endef

define KernelPackage/qdma_wan/description
  Kernel module for qdma wan driver
endef

define KernelPackage/qdma_lan
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet qdma lan driver
  DEPENDS:=kmod-qdma_wan
  FILES:=$(PKG_BUILD_DIR)/qdma/qdma_lan/qdma_lan.ko
  KCONFIG:=
endef

define KernelPackage/qdma_lan/description
  Kernel module for qdma lan driver
endef

# qdma_lan and qdma_wan share the sources in src/qdma/common and differ only by
# the vendor Makefile (Makefile_lan adds -DQDMA_LAN and the lan object list,
# Makefile_wan the wan one). The per-direction directories with 32 symlinks each
# are gone; both views are staged here from the single source tree.
#
# The TSO sources belong to package/airoha/drivers/tso (which also builds
# tso2.ko from them); the V1 path compiles them in-driver, so stage a copy at
# $(PKG_BUILD_DIR)/tso where Makefile_lan/wan reach them via ../../tso/tso.c.
define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/qdma/qdma_lan $(PKG_BUILD_DIR)/qdma/qdma_wan
	$(CP) ./src/qdma/common/. $(PKG_BUILD_DIR)/qdma/qdma_lan/
	$(CP) ./src/qdma/common/. $(PKG_BUILD_DIR)/qdma/qdma_wan/
	$(CP) ./src/qdma/Makefile_lan $(PKG_BUILD_DIR)/qdma/qdma_lan/Makefile
	$(CP) ./src/qdma/Makefile_wan $(PKG_BUILD_DIR)/qdma/qdma_wan/Makefile
	$(CP) $(TOPDIR)/package/airoha/drivers/tso/src $(PKG_BUILD_DIR)/tso
	$(CP) ./files/*.h $(PKG_BUILD_DIR)/
endef

# qdma_lan/qdma_wan locate the shared TSO sources at ../../tso, which resolves
# to $(PKG_BUILD_DIR)/tso with this layout.
define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)/qdma/qdma_wan" \
		BSP_ROOTDIR=$(TOPDIR)/include \
		BSP_CFLAGS="$(BSP_CFLAGS) -include $(PKG_BUILD_DIR)/qdma_compat.h" \
		modules
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)/qdma/qdma_lan" \
		BSP_ROOTDIR=$(TOPDIR)/include \
		BSP_CFLAGS="$(BSP_CFLAGS) -include $(PKG_BUILD_DIR)/qdma_compat.h" \
		modules
endef

$(eval $(call KernelPackage,qdma_wan))
$(eval $(call KernelPackage,qdma_lan))
