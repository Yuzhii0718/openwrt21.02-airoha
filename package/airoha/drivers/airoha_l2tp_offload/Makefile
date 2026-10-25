include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk
PKG_NAME:=$(ECNT)airoha_l2tp_offload
PKG_RELEASE:=1
PKG_VERSION:=1
include $(INCLUDE_DIR)/package.mk

define KernelPackage/airoha_l2tp_offload
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet l2tp_offload driver
  DEPENDS:=
  KCONFIG:=
endef

define KernelPackage/airoha_l2tp_offload/description
  Kernel module for airoha_l2tp_offload driver
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define KernelPackage/airoha_l2tp_offload/install
	tar -zxvf files/data.tar.gz -C $(1)/../
endef
$(eval $(call KernelPackage,airoha_l2tp_offload))
