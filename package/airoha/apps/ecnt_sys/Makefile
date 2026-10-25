#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ecnt_sys
PKG_RELEASE:=1

# Provides BSP_CFLAGS and the GLOBAL_INC_DIR based include paths.
include $(INCLUDE_DIR)/ecnt-bspflags.mk

include $(INCLUDE_DIR)/package.mk

define Package/ecnt_sys
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=ecnt_sys
  DEPENDS:=+ecnt_api
endef

define Package/ecnt_sys/description
	System information utility of Ecnt 752x/751x
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	# the vendor rule passes "-o" to ar, which GNU ar rejects
	sed -i 's|$$(AR) rcs $$(ARHT_SECURE_APP_FLAGS) -o $$\@ $$^|$$(AR) rcs $$\@ $$^|' $(PKG_BUILD_DIR)/ecnt_mmc/Makefile
endef

# ecnt_mmc builds the small static helper library that ecnt_sys links against;
# only the library is needed, its own binary is not part of this package.
define Build/Compile
	mkdir -p $(BSP_EXT_LIB) $(BSP_EXT_INC) $(BSP_INT_LIB) $(BSP_INT_INC)
	+$(MAKE) -C $(PKG_BUILD_DIR)/ecnt_mmc \
		$(TARGET_CONFIGURE_OPTS) \
		BSP_ROOTDIR=$(TOPDIR)/include \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DTRENDCHIP $(BSP_CFLAGS)" \
		INCLUDE="-I$(TOOLS_TRX_DIR) -I$(GLOBAL_FLASH_LAYOUT_INC_DIR)" \
		libecnt_mmc.a
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BSP_ROOTDIR=$(TOPDIR)/include \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -DTRENDCHIP $(BSP_CFLAGS)" \
		INCLUDE="-I$(TOOLS_TRX_DIR) -I$(GLOBAL_FLASH_LAYOUT_INC_DIR) -I$(PKG_BUILD_DIR)/ecnt_mmc" \
		LIB="-L$(PKG_BUILD_DIR)/ecnt_mmc $(TARGET_LDFLAGS) -lapi_lib_utility -lecnt_mmc"
endef

define Package/ecnt_sys/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ecnt_sys $(1)/sbin/
endef

$(eval $(call BuildPackage,ecnt_sys))
