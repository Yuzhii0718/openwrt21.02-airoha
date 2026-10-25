include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk
PKG_NAME:=$(ECNT)sif
PKG_RELEASE:=1
PKG_VERSION:=1
include $(INCLUDE_DIR)/package.mk

define KernelPackage/sif
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet sif driver
  DEPENDS:= kmod-tccicmd
  KCONFIG:=
endef

define KernelPackage/sif/description
  Kernel module for sif driver
endef
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
define KernelPackage/sif/install
	tar -zxvf files/data.tar.gz -C $(1)/../
endef
$(eval $(call KernelPackage,sif))
