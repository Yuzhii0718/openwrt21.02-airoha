include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk
PKG_NAME:=$(ECNT)soft_ratelimit
PKG_RELEASE:=1
PKG_VERSION:=1
include $(INCLUDE_DIR)/package.mk

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

# prebuilt module: nothing to compile
define Build/Compile
	true
endef

define KernelPackage/soft_ratelimit
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet software ratelimit driver
  DEPENDS:=kmod-hw_nat
  KCONFIG:=
endef

define KernelPackage/soft_ratelimit/description
  Kernel module for software ratelimit driver (vendor prebuilt, ARM EABI5 / vermagic 5.4.55 SMP mod_unload ARMv7 p2v8)
endef

define KernelPackage/soft_ratelimit/install
	tar -zxvf files/data.tar.gz -C $(1)/../
endef

$(eval $(call KernelPackage,soft_ratelimit))
