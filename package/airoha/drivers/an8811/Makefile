#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk

PKG_NAME:=$(ECNT)an8811
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/air_en8811h
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=ECNT air_en8811h driver
  FILES:=$(PKG_BUILD_DIR)/air_en8811h/air_en8811h.ko
  KCONFIG:=
endef

define KernelPackage/air_en8811h/description
  Kernel module for air_en8811h driver
endef

define KernelPackage/mdio_arht
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=ECNT mdio_arht driver
  FILES:=$(PKG_BUILD_DIR)/mdio_arht/mdio_arht.ko
  KCONFIG:=
endef

define KernelPackage/mdio_arht/description
  Kernel module for mdio_arht driver
endef

MAKE_OPTS_AIR8811:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	M="$(PKG_BUILD_DIR)/air_en8811h/"

MAKE_OPTS_ARHT:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	M="$(PKG_BUILD_DIR)/mdio_arht/"

define Build/EcntPrepare
	@echo copy src files from tclinux to openwrt
	rm -rf ./src

ifeq ($(MODULES_ENB_BSP_PHY_AN8811_DIR),)
	@echo error, MODULES_ENB_BSP_PHY_AN8811_DIR is null!
	exit "$$?"
endif
ifeq ($(MODULES_XSI_AN8811_BBU_API_DIR),)
	@echo error, MODULES_XSI_AN8811_BBU_API_DIR is null!
	exit "$$?"
endif
ifeq ($(MODULES_XSI_AN8811_BBU_API_MDIO_ARHT_DIR),)
	@echo error, MODULES_XSI_AN8811_BBU_API_MDIO_ARHT_DIR is null!
	exit "$$?"
endif

	mkdir -p ./src/air_en8811h
	mkdir -p ./src/mdio_arht
	

	@echo copy enb_phy code
	$(CP) $(MODULES_ENB_BSP_PHY_AN8811_DIR)/* ./src/air_en8811h/
	@echo copy mdio_arht code
	$(CP) $(MODULES_XSI_AN8811_BBU_API_MDIO_ARHT_DIR)/*  ./src/mdio_arht/
endef	

define Build/Prepare
	$(if $(REBUILDCODE),$(call Build/EcntPrepare))
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef


define Build/Compile
@( \
	if [ -f ./src/air_en8811h/Makefile ]; then \
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS_AIR8811) \
		modules; \
	else \
	echo "Failed to compile air_en8811h"; \
	fi; \
	if [ -f ./src/mdio_arht/Makefile ]; then \
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS_ARHT) \
		modules; \
	else \
	echo "Failed to compile mdio_arht"; \
	fi; \
)

	$(if $(RELEASECODE),$(call EcntModules/release_feeds))
endef

define KernelPackage/air_en8811h/install
	@echo copy firmware of air_en8811h
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(CP) $(PKG_BUILD_DIR)/air_en8811h/*.bin  $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,air_en8811h))
$(eval $(call KernelPackage,mdio_arht))
