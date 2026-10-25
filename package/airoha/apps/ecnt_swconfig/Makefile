#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk

PKG_NAME:=$(ECNT)ecnt_swconfig
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ecnt_swconfig
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet ecnt_swconfig driver
  DEPENDS:=kmod-tccicmd
  FILES:=$(PKG_BUILD_DIR)/ecnt_swconfig.ko
  KCONFIG:=
endef

define KernelPackage/ecnt_swconfig/description
  Kernel module for ecnt_swconfig driver, depends on tccicmd
endef

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	M="$(PKG_BUILD_DIR)"

define Build/EcntPrepare
	rm -rf ./src
	$(INSTALL_DIR) ./src
	$(CP) $(MODULES_ETHER_DIR)/* ./src
endef

define Build/Prepare
	$(if $(REBUILDCODE),$(call Build/EcntPrepare))
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	if [ -d ./src/$(RELEASE_SRC_PATH) ]; then \
		$(CP) ./src/$(RELEASE_SRC_PATH)/*  $(PKG_BUILD_DIR)/; \
	fi
endef

define Build/etherRelease
	rm -rf ./src/soft_qdma/soft_qdma.c
ifeq ($(strip $(TCSUPPORT_5_4_KERNEL) $(TCSUPPORT_5_10_KERNEL)),)
ifneq ($(TCSUPPORT_DS_HWNAT_OFFLOAD),)
	$(INSTALL_DIR) ./src/$(RELEASE_SRC_PATH)/soft_qdma
	$(CP) $(PKG_BUILD_DIR)/soft_qdma/soft_qdma.o ./src/$(RELEASE_SRC_PATH)/soft_qdma/
endif
endif
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) BSP_ROOTDIR=$(INCLUDE_DIR)\
		modules
	$(if $(RELEASECODE),$(call Build/etherRelease))
	$(if $(RELEASECODE),$(call EcntModules/release_feeds))
endef

$(eval $(call KernelPackage,ecnt_swconfig))
