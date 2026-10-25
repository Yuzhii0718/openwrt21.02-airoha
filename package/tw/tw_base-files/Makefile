include $(TOPDIR)/rules.mk

PKG_NAME:=tw-base
PKG_VERSION:=2021-01-01
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1
PKG_CONFIG_DEPENDS := CONFIG_TW_PRODUCT_NAME

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=TW Properties
  CATEGORY:=TW Properties
  SUBMENU:=Configuration
  TITLE:=base files for TW
  DEPENDS:=
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include -fpie -I$(STAGING_DIR)/include
TARGET_LDFLAGS += -pie

ifeq ($(CONFIG_TW_PRODUCT_NAME),"")
CFG_PRODUCT_H = cfg_product_def_general.h
else
CFG_PRODUCT_H = cfg_product_def_$(CONFIG_TW_PRODUCT_NAME).h
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/include $(PKG_BUILD_DIR)/
	$(CP) ./src/Makefile.orig $(PKG_BUILD_DIR)/Makefile
	mv $(PKG_BUILD_DIR)/include/$(CFG_PRODUCT_H) $(PKG_BUILD_DIR)/include/cfg_product.h
	rm -f $(PKG_BUILD_DIR)/include/cfg_product_def_*.h

	mkdir -p $(PKG_BUILD_DIR)/rootfs
	@if [ -d ./files/$(CONFIG_TW_PRODUCT_NAME) ]; then \
		$(CP) -L ./files/$(CONFIG_TW_PRODUCT_NAME)/* $(PKG_BUILD_DIR)/rootfs; \
	else \
		$(CP) -L ./files/General/* $(PKG_BUILD_DIR)/rootfs; \
	fi;

	@if [ -f $(PKG_BUILD_DIR)/rootfs/etc/preinit ]; then \
		$(SED) "s#%PATH%#$(TARGET_INIT_PATH)#g" $(PKG_BUILD_DIR)/rootfs/etc/preinit; \
	fi;

	@if [ -d $(PKG_BUILD_DIR)/rootfs/etc/dropbear ]; then \
		chmod -R 755 $(PKG_BUILD_DIR)/rootfs/etc/dropbear; \
	fi;
endef

# Regenerate the rootfs overlay from the *currently selected* product variant on
# every compile. Build/Prepare only copies ./files/$(CONFIG_TW_PRODUCT_NAME) into
# PKG_BUILD_DIR once and its stamp does not depend on CONFIG_TW_PRODUCT_NAME, so
# reusing PKG_BUILD_DIR/rootfs here would keep overlaying files of a previously
# selected variant onto the image (e.g. EXS27 ports/dhcp config on EXS27-nowifi).
define Build/Compile
	rm -rf $(PKG_BUILD_DIR)/rootfs
	mkdir -p $(PKG_BUILD_DIR)/rootfs
	@if [ -d ./files/$(CONFIG_TW_PRODUCT_NAME) ]; then \
		$(CP) -L ./files/$(CONFIG_TW_PRODUCT_NAME)/* $(PKG_BUILD_DIR)/rootfs; \
	else \
		$(CP) -L ./files/General/* $(PKG_BUILD_DIR)/rootfs; \
	fi;

	@if [ -f $(PKG_BUILD_DIR)/rootfs/etc/preinit ]; then \
		$(SED) "s#%PATH%#$(TARGET_INIT_PATH)#g" $(PKG_BUILD_DIR)/rootfs/etc/preinit; \
	fi;

	@if [ -d $(PKG_BUILD_DIR)/rootfs/etc/dropbear ]; then \
		chmod -R 755 $(PKG_BUILD_DIR)/rootfs/etc/dropbear; \
	fi;

	rm -rf $(TOPDIR)/files
	mkdir -p $(TOPDIR)/files

	$(CP) -L $(PKG_BUILD_DIR)/rootfs/* $(TOPDIR)/files/;
endef

# The build system does not track the variant files below, so editing them would
# otherwise leave a stale overlay. Rebuild whenever anything under ./files changes.
$(STAMP_BUILT): $(shell find ./files -type f 2>/dev/null)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/include/cfg* $(1)/usr/include/

	#$(INSTALL_DIR) $(1)/etc/
	#$(CP) $(PKG_BUILD_DIR)/rootfs/etc/* $(1)/etc/

	#$(INSTALL_DIR) $(1)/lib/
	#$(CP) $(PKG_BUILD_DIR)/rootfs/lib/* $(1)/lib/

	#$(INSTALL_DIR) $(1)/cfg/
	#$(CP) $(PKG_BUILD_DIR)/rootfs/cfg/* $(1)/cfg/
endef

define Package/$(PKG_NAME)/install
#	$(INSTALL_DIR) $(1)/etc/
#	$(CP) $(PKG_BUILD_DIR)/rootfs/etc/* $(1)/etc/

#	$(INSTALL_DIR) $(1)/lib/
#	$(CP) $(PKG_BUILD_DIR)/rootfs/lib/* $(1)/lib/

#	$(INSTALL_DIR) $(1)/cfg/
#	$(CP) $(PKG_BUILD_DIR)/rootfs/cfg/* $(1)/cfg/

#	$(INSTALL_DIR) $(1)/rootfs/data/persistent_data
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
