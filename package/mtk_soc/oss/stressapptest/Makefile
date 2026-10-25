include $(TOPDIR)/rules.mk

PKG_NAME:=stressapptest
PKG_VERSION:=1.0.9
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/stressapptest/stressapptest
PKG_SOURCE_VERSION:=master
PKG_MIRROR_HASH:=66d99dd02c1ae06896651bca1ec8c1f288f4746d06cc99be807fa51a61672a5f

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/stressapptest
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Stressful Application Test
  URL:=https://github.com/stressapptest/stressapptest
  DEPENDS:=+libstdcpp
endef

define Package/stressapptest/description
 Stressful Application Test (or stressapptest, its unix name) is a memory
interface test. It tries to maximize randomized traffic to memory from
processor and I/O, with the intent of creating a realistic high load
situation in order to test the existing hardware devices in a computer.
It has been used at Google for some time and now it is available under the
apache 2.0 license.
endef

ifeq ($(ARCH),arm)
CONFIGURE_ARGS += \
        --host=armv7a
output_name := stressapptest32
endif
ifeq ($(ARCH),aarch64)
CONFIGURE_ARGS += \
        --host=aarch64
output_name := stressapptest64
endif

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/stressapptest/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/stressapptest $(1)/usr/sbin/$(output_name)
endef

$(eval $(call BuildPackage,stressapptest))
