#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ecnt_wl_e2p_chk
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

# The WiFi interface variant selects which gen_*_e2p_w_compile_option() helpers
# get compiled in ecnt_wl_e2p_chk.c. In the vendor build these switches reached
# the compiler through BSP_CFLAGS (target/linux/airoha/rule.mak); packages do not
# see that variable here, so read the same switches from the subtarget profile.
-include $(wildcard $(TOPDIR)/target/linux/airoha/$(SUBTARGET)/*.mak)

WIFI_CFLAGS:=
ifeq ($(strip $(TCSUPPORT_WLAN_MT7915N)),y)
  WIFI_CFLAGS += -DMT7915N -DBBUTOWBU
endif
ifeq ($(strip $(TCSUPPORT_DUAL_WLAN_MT7915E)),y)
  WIFI_CFLAGS += -DMT7915E -DBBUTOWBU -DTCSUPPORT_DUAL_WLAN -DTCSUPPORT_WLAN_AC -DTCSUPPORT_WLAN_AX
endif
ifeq ($(strip $(TCSUPPORT_WLAN_MT7915D)),y)
  WIFI_CFLAGS += -DMT7915D -DTCSUPPORT_WLAN_MT7915D -DMT7915N -DMT7915E -DBBUTOWBU \
                 -DTCSUPPORT_DUAL_WLAN -DTCSUPPORT_WLAN_AC -DTCSUPPORT_WLAN_AX -DTCSUPPORT_WLAN_WPA3
endif
ifeq ($(strip $(TCSUPPORT_WLAN_MT7916D)),y)
  WIFI_CFLAGS += -DMT7916D -DTCSUPPORT_WLAN_MT7916D -DMT7916N -DMT7916E -DBBUTOWBU \
                 -DTCSUPPORT_DUAL_WLAN -DTCSUPPORT_WLAN_AC -DTCSUPPORT_WLAN_AX -DTCSUPPORT_WLAN_WPA3
endif

define Package/ecnt_wl_e2p_chk
  SECTION:=Econet Properties
  CATEGORY:=Econet Properties
  SUBMENU:=Applications
  TITLE:=ecnt_wl_e2p_chk is app for checking the wifi eeprom table is correct or not
  DEPENDS:=
endef

define Package/ecnt_wl_e2p_chk/description
	App for checking the wifi eeprom table is correct or not
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) $(WIFI_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/ecnt_wl_e2p_chk/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ecnt_wl_e2p_chk $(1)/sbin/
endef

$(eval $(call BuildPackage,ecnt_wl_e2p_chk))
