include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk
PKG_NAME:=$(ECNT)diag_tools
PKG_RELEASE:=1
PKG_VERSION:=1
include $(INCLUDE_DIR)/package.mk

define KernelPackage/diag_tools
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet diag_tools driver
  DEPENDS:=kmod-qdma_lan
  KCONFIG:=
endef

define KernelPackage/diag_tools/description
  Kernel module for diag_tools driver (vendor prebuilt, ARM EABI5 / vermagic 5.4.55 SMP mod_unload ARMv7 p2v8)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

# prebuilt module: nothing to compile
define Build/Compile
	true
endef

define KernelPackage/diag_tools/install
	tar -zxvf files/data.tar.gz -C $(1)/../
endef

$(eval $(call KernelPackage,diag_tools))
