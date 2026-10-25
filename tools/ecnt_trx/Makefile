#
# Copyright (C) 2006-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/ecnt-release.mk


PKG_NAME := ecnt_trx
PKG_RELEASE := 1

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/kernel.mk

HOST_CFLAGS += -DTRENDCHIP
ifeq ($(TCSUPPORT_LITTLE_ENDIAN), 1)
HOST_CFLAGS += -DTCSUPPORT_LITTLE_ENDIAN
endif
ifeq ($(TCSUPPORT_SECURE_BOOT), 1)
HOST_CFLAGS += -DTCSUPPORT_SECURE_BOOT
endif
ifeq ($(TCSUPPORT_SECURE_BOOT_V1), 1)
HOST_CFLAGS += -DTCSUPPORT_SECURE_BOOT_V1
endif
ifeq ($(TCSUPPORT_SECURE_BOOT_V2), 1)
HOST_CFLAGS += -DTCSUPPORT_SECURE_BOOT_V2
endif
ifeq ($(TCSUPPORT_BL2_OPTIMIZATION), 1)
HOST_CFLAGS += -DTCSUPPORT_BL2_OPTIMIZATION
endif

define cc
	$(HOSTCC) \
		$(HOST_CFLAGS) \
		-Wno-unused-parameter \
		-include endian.h $(HOST_LDFLAGS) \
		-o $(HOST_BUILD_DIR)/bin/$(firstword ecnt_$(1)) \
		$(foreach src,$(1),src/$(src).c) \
		$(2)
endef

define Host/Compile
	mkdir -p $(HOST_BUILD_DIR)/bin
	$(call cc,trx)
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/bin/* $(STAGING_DIR_HOST)/bin/
endef

$(eval $(call HostBuild))
