#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(INCLUDE_DIR)/kernel.mk

FEEDS_PATH=$(TOPDIR)/../airoha_feeds

ifneq ($(strip $(MSDK)),)
    COMPILE_PROFILE_NAME=$(shell echo $(TOPDIR)/target/linux/airoha/$(SUBTARGET)/*.mak)
    COMPILE_PROFILE=$(patsubst %.mak,%, $(notdir $(wildcard $(COMPILE_PROFILE_NAME))))
    RELEASE_SRC_PATH=$(COMPILE_PROFILE)
else
	ifneq ($(strip $(RELEASE_PROFILE)),)
                RELEASE_SRC_PATH=$(RELEASE_PROFILE)
	else
                RELEASE_SRC_PATH=$(RELEASE_PROFILE_DEFAULT)
	endif
endif

define EcntModules/release
	rm -rf ./src/$(RELEASE_SRC_PATH)/*
	$(CP) $(FILES) ./src/$(RELEASE_SRC_PATH)/
endef

define AirohaMapd/release
	echo "==>>>[$(CP) ./src $(TOPDIR)/../../openwrt-wifi/package_public/mtk/applications/mapd/]"
	$(CP) ./src $(TOPDIR)/../../openwrt-wifi/package_public/mtk/applications/mapd/
endef

define EcntModules/release_feeds
	RELEASE_PATH=$(shell echo $(PKG_NAME))
	MENU=$(shell echo $(SUBMENU))
ifneq ($(findstring BSP,$(shell pwd)),)
	$(CP) ./* $(FEEDS_PATH)/package/airoha/BSP/$(PKG_NAME)/
else
ifeq ($(strip $(SUBMENU)),Applications)
	$(CP) ./* $(FEEDS_PATH)/package/airoha/apps/$(PKG_NAME)/
endif
endif
 
ifeq ($(strip $(SUBMENU)),tcboot)
	$(CP) ./* $(FEEDS_PATH)/package/airoha/bootloader/
endif
ifeq ($(strip $(SUBMENU)),ECNT_Drivers)
	$(CP) ./* $(FEEDS_PATH)/package/airoha/drivers/$(PKG_NAME)/
endif
ifeq ($(strip $(PKG_NAME)),firmware-utils)
	mkdir -p $(FEEDS_PATH)/tools/$(PKG_NAME)/
	$(CP) ./* $(FEEDS_PATH)/tools/$(PKG_NAME)/
endif
endef

