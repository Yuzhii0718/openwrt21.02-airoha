include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=$(ECNT)hw_nat
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

# Provides BSP_CFLAGS with the TCSUPPORT_* defines the driver expects.
include $(INCLUDE_DIR)/ecnt-bspflags.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/hw_nat
  CATEGORY:=Econet Properties
  SUBMENU:=ECNT_Drivers
  TITLE:=Econet hw_nat driver
  DEPENDS:=kmod-ether kmod-fe kmod-tcledctrl
  FILES:=$(PKG_BUILD_DIR)/hw_nat.ko
  KCONFIG:=
endef

define KernelPackage/hw_nat/description
  Kernel module for hw_nat driver
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
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

$(eval $(call KernelPackage,hw_nat))
