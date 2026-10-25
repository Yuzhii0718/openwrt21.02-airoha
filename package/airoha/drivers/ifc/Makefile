include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/ecnt-release.mk

PKG_NAME:=$(ECNT)ifc
PKG_RELEASE:=1
PKG_VERSION:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS with the TCSUPPORT_* defines the driver expects.
include $(INCLUDE_DIR)/ecnt-bspflags.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/ifc
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet Ifc driver
  DEPENDS:=+kmod-fe
  FILES:=$(PKG_BUILD_DIR)/ifc.ko
  KCONFIG:=
endef

define KernelPackage/ifc/description
  Kernel module for ifc driver
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(CP) ./files/*.h $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) -C "$(LINUX_DIR)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		M="$(PKG_BUILD_DIR)" \
		BSP_ROOTDIR=$(TOPDIR)/include \
		BSP_CFLAGS="$(BSP_CFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,ifc))
