# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/target.mk
-include $(INCLUDE_DIR)/ecnt-release.mk

PKG_NAME:=$(ECNT)legacy_api
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ecnt_legacy_api
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet legacy_api driver
  FILES:=$(PKG_BUILD_DIR)/ecnt_legacy_api.ko
  KCONFIG:=
endef

define KernelPackage/ecnt_legacy_api/description
  Kernel module for ecnt_legacy_api driver
endef

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	M="$(PKG_BUILD_DIR)"

define Build/EcntPrepare
	rm -rf ./src/$(RELEASE_SRC_PATH)
	$(INSTALL_DIR) ./src/$(RELEASE_SRC_PATH)
	$(CP) $(MODULES_PRIV_SRC_DIR)/legacy_api/* ./src/$(RELEASE_SRC_PATH)
endef

define Build/Prepare
	$(if $(REBUILDCODE),$(call Build/EcntPrepare))
	mkdir -p $(PKG_BUILD_DIR)
	if [ -d ./src/$(SUBTARGET) ]; then \
		$(CP) ./src/$(SUBTARGET)/* $(PKG_BUILD_DIR)/; \
	else \
		$(CP) ./src/$(RELEASE_SRC_PATH)/* $(PKG_BUILD_DIR)/;\
	fi
endef

define Build/Compile
@( \
	if [ -f ./src/$(SUBTARGET)/Makefile ] || [ -f ./src/$(RELEASE_SRC_PATH)/Makefile ]; then \
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules; \
	 else \
	 echo "NO"; \
	 fi; \
)
	$(if $(RELEASECODE),$(call EcntModules/release))
	$(if $(RELEASECODE),$(call EcntModules/release_feeds))
endef

$(eval $(call KernelPackage,ecnt_legacy_api))
