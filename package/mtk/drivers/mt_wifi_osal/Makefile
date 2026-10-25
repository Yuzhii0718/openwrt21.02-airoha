# All rights reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# MediaTek WiFi OS abstraction layer. In the 2024-09 "Kite" drop the same code
# was compiled into mt_wifi_cmn.ko out of mt_wifi/os/linux/mt_linux.c; since the
# 2025-06-13 mt799x drop it is a standalone module (mt_wifi_osal.ko) whose
# headers mt_wifi7 pulls in through staging_dir/usr/include/mt_wifi/os.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mt_wifi_osal
PKG_RELEASE:=1
PKG_VERSION:=a53418
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=
PKG_HASH:=dfa4b178f198504a707dec957242f8ede1a732823e940faa6304c723ac3224ad
# The tarball's top directory is wifi_osal/ and the default unpack extracts into
# the parent of PKG_BUILD_DIR, so point PKG_BUILD_DIR one level down.
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)/wifi_osal

include $(INCLUDE_DIR)/package.mk

define KernelPackage/mt_wifi_osal
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=MediaTek WiFi OS Abstraction Layer
  DEPENDS:=+MTK_WIFI_OSAL_CFG80211_SUPPORT:kmod-cfg80211
  FILES:=$(PKG_BUILD_DIR)/mt_wifi_osal.ko
  AUTOLOAD:=$(call AutoLoad,60,$(if $(CONFIG_MTK_WIFI_OSAL_CFG80211_SUPPORT), cfg80211) mt_wifi_osal,1)
endef

define KernelPackage/mt_wifi_osal/config
	source "$(SOURCE)/Config.in"
endef

define KernelPackage/mt_wifi_osal/description
  MediaTek WiFi OS Abstraction Layer (mt_wifi_osal.ko), required by mt_wifi7
endef

# Virtual package kept from the old mt_wifi_cmn wrapper: it only pins the
# cfg80211 load order (59) below mt_wifi_osal (60) and mt_wifi (61).
define KernelPackage/mt_cfg80211
  CATEGORY:=MTK Properties
  TITLE:=Virtual kernel package for cfg80211
  SUBMENU:=Drivers
  MENU:=1
  AUTOLOAD:=$(call AutoLoad,59,cfg80211)
  DEPENDS:=+kmod-cfg80211
endef

define KernelPackage/mt_cfg80211/description
  Virtual kernel package for cfg80211
endef

BACKPORT_VERSION:=$(lastword $(subst /, , $(shell find $(KERNEL_BUILD_DIR) -type 'd' -name 'backports*')))

PKG_EXTRA_CFLAGS:= $(if $(CONFIG_MTK_WIFI_OSAL_CFG80211_SUPPORT), -DCFG80211_SUPPORT) \
		$(if $(CONFIG_MTK_WIFI_OSAL_DEBUG), -DMT_DEBUG) \
		$(if $(CONFIG_MTK_WIFI_OSAL_CUSTOM_VPRINTK), -DCUSTOM_FACILITY=$(CONFIG_MTK_WIFI_OSAL_CUSTOM_VPRINTK_FACILITY)) \
		$(if $(CONFIG_MTK_WIFI7_VLAN_GTK_SUPPORT), -DCONFIG_VLAN_GTK_SUPPORT) \
		$(if $(CONFIG_MTK_WIFI7_CCN67_SPLIT_MAC_SUPPORT), -DCCN67_SPLIT_MAC_SUPPORT) \
		$(if $(CONFIG_MTK_WIFI_OSAL_NETIF_RX_DISPATCH), -DCONFIG_NETIF_RX_DISPATCH)

PKG_EXTRA_CFLAGS += -Wno-error=missing-prototypes -Wno-error=implicit-function-declaration -Wno-empty-body

NOSTDINC_FLAGS :=
ifeq ($(CONFIG_MTK_WIFI_OSAL_CFG80211_SUPPORT),y)
NOSTDINC_FLAGS += \
	$(KERNEL_NOSTDINC_FLAGS) \
	-DBACKPORT_NOSTDINC \
	-I$(PKG_BUILD_DIR) \
	-I$(PKG_BUILD_DIR)/inc \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-include backport/backport.h
endif

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		EXTRA_CFLAGS="$(PKG_EXTRA_CFLAGS) $(EXTRA_CFLAGS)" \
		CFG_CFG80211_VERSION:=$(word 2,$(subst -, ,$(BACKPORT_VERSION))) \
		modules
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/mt_wifi/os
	$(CP) $(PKG_BUILD_DIR)/inc/* $(1)/usr/include/mt_wifi/os
endef

$(eval $(call KernelPackage,mt_wifi_osal))
$(eval $(call KernelPackage,mt_cfg80211))
