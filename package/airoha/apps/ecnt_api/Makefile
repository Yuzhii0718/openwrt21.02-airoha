#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ecnt_api
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/ecnt-bspflags.mk
include $(INCLUDE_DIR)/package.mk

define Package/ecnt_api
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=ecnt_api
  DEPENDS:=+ethcmd
endef

define Package/ecnt_api/description
	This package contains some APIs and CMDs for Ecnt 752x/751x
endef

ECNT_API_BINS:=acl blapi_cmd blapi_demo femgr ifc npu_bridge_cmd ppemgr \
	qdmamgr_lan qdmamgr_wan qosmgr qosrule switchmgr uart

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(CP) $(PKG_BUILD_DIR)/hw_driver/ecnt_lib_prolinecmd/src/openwrt/prolinecmd.c $(PKG_BUILD_DIR)/hw_driver/ecnt_lib_prolinecmd/src/cmd/
	$(CP) $(PKG_BUILD_DIR)/hw_driver/ecnt_lib_prolinecmd/src/openwrt/libapi_lib_prolinecmd_flag.h $(PKG_BUILD_DIR)/hw_driver/ecnt_lib_prolinecmd/src/lib_src/
	sed -i '/BLAPI_HEADER += blapi_dslimit.h/d' $(PKG_BUILD_DIR)/ecnt_blapi/traffic/Makefile
endef

# The vendor makefiles install their output into the BSP staging directories
# defined by target/linux/airoha/dir.mak.
define Build/Compile
	mkdir -p $(BSP_EXT_LIB) $(BSP_EXT_INC) $(KERNEL_BUILD_DIR)/install_bsp/fs/userfs/bin $(BSP_EXT_FS_USR) \
		$(BSP_INT_LIB) $(BSP_INT_INC) $(BSP_INT_FS_USEFS) $(BSP_INT_FS_USR)
	+$(MAKE) -C $(PKG_BUILD_DIR) -f $(PKG_BUILD_DIR)/hw_driver/make_hw_driver \
		CC="$(TARGET_CC)" STRIP="$(TARGET_CROSS)strip" AR="$(TARGET_CROSS)ar" \
		BSP_ROOTDIR=$(TOPDIR)/include KERNEL_DIR="$(LINUX_DIR)" \
		KERNEL_BUILD_DIR="$(KERNEL_BUILD_DIR)" BSP_CFLAGS="$(BSP_CFLAGS)" -k hw_api
	+$(MAKE) -C $(PKG_BUILD_DIR) -f $(PKG_BUILD_DIR)/sw_api/make_sw_api \
		CC="$(TARGET_CC)" STRIP="$(TARGET_CROSS)strip" AR="$(TARGET_CROSS)ar" \
		BSP_ROOTDIR=$(TOPDIR)/include KERNEL_DIR="$(LINUX_DIR)" \
		KERNEL_BUILD_DIR="$(KERNEL_BUILD_DIR)" BSP_CFLAGS="$(BSP_CFLAGS)" -k sw_api
	+$(MAKE) -C $(PKG_BUILD_DIR) -f $(PKG_BUILD_DIR)/ecnt_blapi/make_blapi \
		CC="$(TARGET_CC)" STRIP="$(TARGET_CROSS)strip" AR="$(TARGET_CROSS)ar" \
		BSP_ROOTDIR=$(TOPDIR)/include KERNEL_DIR="$(LINUX_DIR)" \
		KERNEL_BUILD_DIR="$(KERNEL_BUILD_DIR)" BSP_CFLAGS="$(BSP_CFLAGS)" \
		BSPAPI_INCDIR="$(BSP_EXT_INC):$(BSP_INT_INC):$(LINUX_DIR)/include/global_inc:$(LINUX_DIR)/include/global_inc/trx:$(PKG_BUILD_DIR)/ecnt_blapi/system:$(PKG_BUILD_DIR)/ecnt_blapi/traffic:$(PKG_BUILD_DIR)/ecnt_blapi/perform:$(PKG_BUILD_DIR)/ecnt_blapi/tc_partition" \
		BSPAPI_LIBDIR="$(BSP_EXT_LIB):$(BSP_INT_LIB)" -k blapi
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(KERNEL_BUILD_DIR)/install_bsp/fs/usr/include/*.h $(1)/usr/include/ 2>/dev/null || true
	-$(CP) $(PKG_BUILD_DIR)/hw_driver/*/*.h $(1)/usr/include/
	-$(CP) $(PKG_BUILD_DIR)/hw_driver/*/lib_src/*.h $(1)/usr/include/
	-$(CP) $(PKG_BUILD_DIR)/ecnt_blapi/*/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(KERNEL_BUILD_DIR)/install_bsp/fs/lib/lib*.so* $(1)/usr/lib/
endef

define Package/ecnt_api/install
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(KERNEL_BUILD_DIR)/install_bsp/fs/lib/lib*.so* $(1)/lib/
	$(INSTALL_DIR) $(1)/userfs/bin
	for b in $(ECNT_API_BINS); do \
		if [ -f $(KERNEL_BUILD_DIR)/install_bsp/fs/userfs/bin/$$$$b ]; then $(INSTALL_BIN) $(KERNEL_BUILD_DIR)/install_bsp/fs/userfs/bin/$$$$b $(1)/userfs/bin/; fi; \
	done
endef

$(eval $(call BuildPackage,ecnt_api))
