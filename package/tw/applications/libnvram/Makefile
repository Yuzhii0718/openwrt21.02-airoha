include $(TOPDIR)/rules.mk

PKG_NAME:=libnvram
PKG_VERSION:=0.9.28
PKG_SOURCE:=libnvram-9d068860.tar.gz
PKG_SOURCE_URL:=
PKG_HASH:=7100a8cfccbf8e6c3f987bb5efaf1f4e802e7f594e0d26c3744a0bd725842f24
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/libnvram
	SECTION:=TW Properties
	CATEGORY:=TW Properties
	TITLE:=mtk nvram library
	SUBMENU:=Applications
	DEPENDS:=+zlib
endef

# All three objects are compiled from the vendor tarball. The vendor build
# environment is not part of the GPL drop, so the pieces it generated are
# reconstructed from the prebuilt objects that shipped in files/ (see patches):
#
#  001  nvram.h / nvram_env.c included the BSP-generated <linux/autoconf.h>,
#       which does not exist here. The patch drops the include and freezes
#       FLASH_BLOCK_NUM=2, recovered from the prebuilt nvram_env.o (its
#       getNvramNum() returns 2).
#  002  the fb[] block table: the prebuilt nvram_env.o carries two entries in
#       .data.rel.local, "FENV" at offset 0x20000 and "UENV" at 0x40000, both
#       32 KB (ENV_BLK_SIZE*32). The patch replaces the generator-side table
#       (dev1/dev2/dev3/cert) with the product one.
#  003  flash_api.c: on UBI layouts the factory data and the NVRAM environment
#       live in the "areaconf" UBI volume rather than raw MTD partitions, so
#       "facenv"/"Factory" are resolved through /dev/ubiblock0_<vol_id>.
#
# The object set matches the vendor shared library: crc32.o + nvram_env.o +
# flash_api.o.
define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -DUSE_HOSTCC -fPIC -I$(PKG_BUILD_DIR) \
		-c $(PKG_BUILD_DIR)/crc32.c -o $(PKG_BUILD_DIR)/crc32.o
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -fPIC -I$(PKG_BUILD_DIR) \
		-c $(PKG_BUILD_DIR)/nvram_env.c -o $(PKG_BUILD_DIR)/nvram_env.o
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -fPIC -I$(PKG_BUILD_DIR) \
		-c $(PKG_BUILD_DIR)/flash_api.c -o $(PKG_BUILD_DIR)/flash_api.o
	$(TARGET_AR) rcs $(PKG_BUILD_DIR)/libnvram.a \
		$(PKG_BUILD_DIR)/crc32.o $(PKG_BUILD_DIR)/nvram_env.o $(PKG_BUILD_DIR)/flash_api.o
	$(TARGET_CC) -shared -Wl,-soname,libnvram.so.0 -s \
		-o $(PKG_BUILD_DIR)/libnvram.so.0.9.28 \
		$(PKG_BUILD_DIR)/crc32.o $(PKG_BUILD_DIR)/nvram_env.o $(PKG_BUILD_DIR)/flash_api.o -lc
	$(LN) libnvram.so.0.9.28 $(PKG_BUILD_DIR)/libnvram.so.0
	$(LN) libnvram.so.0.9.28 $(PKG_BUILD_DIR)/libnvram.so
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/flash_api.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/linux_socket.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/nvram{,_env,_sock}.h $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/sys_debug.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libnvram.a $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libnvram.so* $(1)/usr/lib/
endef

define Package/libnvram/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libnvram.so.0.9.28 $(1)/usr/lib/
	$(LN) libnvram.so.0.9.28 $(1)/usr/lib/libnvram.so.0
	$(LN) libnvram.so.0.9.28 $(1)/usr/lib/libnvram.so
endef

$(eval $(call BuildPackage,libnvram))
