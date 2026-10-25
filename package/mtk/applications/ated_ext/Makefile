
#
# hua.shao@mediatek.com
#
# MTK Property Software.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ated_ext
PKG_VERSION:=1
PKG_RELEASE:=2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

TARGET_CFLAGS += -DCONFIG_SUPPORT_OPENWRT -D_GNU_SOURCE
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/libnl-tiny
MAKE_FLAGS += CONFIG_SUPPORT_OPENWRT=y
MAKE_FLAGS += NL80211_INCLUDE_PATH=$(LINUX_DIR)/include/uapi/linux/mtk_nl80211_inc
ifeq ($(CONFIG_MTK_WIFI7_CFG80211_SUPPORT),y)
MAKE_FLAGS += CONFIG_CFG80211_SUPPORT=y
endif

CFLAGS += \
	-I$(LINUX_DIR)/include

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -frd ./src/* $(PKG_BUILD_DIR)
endef

define Package/ated_ext
  SECTION:=MTK Properties
  CATEGORY:=MTK Properties
  DEPENDS:=+libblkid +libnl-tiny
  TITLE:=ated_ext
  SUBMENU:=Applications
endef

define Package/ated_ext/description
  ated_ext Daemon
endef


define Package/ated_ext/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ated_ext $(1)/usr/bin/ated
endef


$(eval $(call BuildPackage,ated_ext))
