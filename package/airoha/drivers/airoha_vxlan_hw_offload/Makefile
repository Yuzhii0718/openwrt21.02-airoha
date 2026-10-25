include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk
PKG_NAME:=$(ECNT)airoha_vxlan_hw_offload
PKG_RELEASE:=1
PKG_VERSION:=1
include $(INCLUDE_DIR)/package.mk

define KernelPackage/airoha_vxlan_hw_offload
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet airoha_vxlan_hw_offload driver
  DEPENDS:=
  KCONFIG:=
endef

define KernelPackage/airoha_vxlan_hw_offload/description
  Kernel module for airoha_vxlan_hw_offload driver (vendor prebuilt, ARM EABI5 / vermagic 5.4.55 SMP mod_unload ARMv7 p2v8)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

# prebuilt module: nothing to compile
define Build/Compile
	true
endef

define KernelPackage/airoha_vxlan_hw_offload/install
	tar -zxvf files/data.tar.gz -C $(1)/../
endef

$(eval $(call KernelPackage,airoha_vxlan_hw_offload))
