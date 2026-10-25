include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ecnt_bsp
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/ecnt-bspflags.mk
include $(INCLUDE_DIR)/package.mk

define Package/ecnt_bsp
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=ecnt_bsp
  DEPENDS:=+ecnt_api
endef

define Package/ecnt_bsp/description
	BSP tools of Ecnt 752x/751x (mtd, brctl, tcci, sys, tce, sifm)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	mkdir -p $(PKG_BUILD_DIR)/bsp_fs $(PKG_BUILD_DIR)/bsp_lib $(KERNEL_BUILD_DIR)/install_bspapp_exclusive/fs/{usr/bin,usr/sbin,sbin,lib}
	+$(MAKE) -C $(PKG_BUILD_DIR) -f $(PKG_BUILD_DIR)/MakeFile_OpenWRT_BSP \
		BSP_ROOTDIR=$(TOPDIR)/include \
		PKG_BUILD_DIR=$(PKG_BUILD_DIR) \
		APP_BSP_DIR=$(PKG_BUILD_DIR) \
		APP_BSP_EXCLUSIVE_PRIVATE_DIR=$(PKG_BUILD_DIR)/private_exclusive \
		APP_BSP_EXCLUSIVE_PUBLIC_DIR=$(PKG_BUILD_DIR)/public_exclusive \
		BSP_PUBLICDIR=$(PKG_BUILD_DIR)/public \
		BSP_PRIVATEDIR=$(PKG_BUILD_DIR)/private \
		KERNEL_ROOT=$(KERNEL_BUILD_DIR) \
		CC="$(TARGET_CC)" AR="$(TARGET_CROSS)ar" STRIP="$(TARGET_CROSS)strip" \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -I$(LINUX_DIR)/include/global_inc -I$(LINUX_DIR)/include/global_inc/trx $(BSP_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		openwrt_ecnt_bsp
endef

define Package/ecnt_bsp/install
	$(INSTALL_DIR) $(1)/sbin $(1)/usr/sbin $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/public/mtd/mtd $(1)/sbin/ecnt_mtd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/private_exclusive/tcci_bsp/tcci $(1)/usr/bin/tcci
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/public_exclusive/bridge-utils-1.5/brctl/brctl $(1)/usr/sbin/brctl
	ln -sf /usr/bin/tcci $(1)/usr/bin/sys
	ln -sf /usr/bin/tcci $(1)/usr/bin/sifm
	ln -sf /usr/bin/tcci $(1)/usr/bin/tce
endef

$(eval $(call BuildPackage,ecnt_bsp))
