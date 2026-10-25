#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)tso
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS (-DTCSUPPORT_* switch set).
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define KernelPackage/tso
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet TSO v2 driver
  DEPENDS:=+kmod-qdma_lan +kmod-qdma_wan
  FILES:=$(PKG_BUILD_DIR)/tso/tso2.ko
  KCONFIG:=
endef

define KernelPackage/tso/description
  TSO offload (tso2.ko) built from this package's src/. qdma_lan/qdma_wan reuse
  the same sources in-driver for the V1 path (they pick them up from here at
  build time), while this package provides the module preinit expects for
  TCSUPPORT_TSO_V2_ENABLE boards.
endef

# tso.h reaches the qdma register headers through
# "../qdma/qdma_wan/qdma_reg.h" (TCSUPPORT_OPENWRT), so stage the qdma headers
# next to tso/ in the build dir. Only the headers are shared - the module itself
# owns its sources.
TSO_QDMA_SRC:=$(TOPDIR)/package/airoha/drivers/qdma/src

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)/tso $(PKG_BUILD_DIR)/qdma/qdma_wan
	$(CP) ./src/. $(PKG_BUILD_DIR)/tso/
	$(CP) $(TSO_QDMA_SRC)/qdma/common/. $(PKG_BUILD_DIR)/qdma/qdma_wan/
	# tso2.ko is the TCSUPPORT_TSO_V2_ENABLE provider; tso.h pulls
	# <ecnt_hook/ecnt_hook_qdma_type.h> (which is where DBG_ST lives) only under
	# that macro. The board mak keeps it off so the data path stays on V1.
	echo 'ccflags-y += -DTCSUPPORT_TSO_V2_ENABLE' >> $(PKG_BUILD_DIR)/tso/Makefile
endef

define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)/tso" \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,tso))
