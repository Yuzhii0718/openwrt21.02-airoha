# 
# Copyright (C) 2012, 2013 Cisco Systems
#
# This Makefile for Safe c library
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsafec3
#PKG_VERSION:=03032018.0-g570fa5
PKG_RELEASE:=3.3.0

#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL:=https://github.com/rurban/safeclib.git
#PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
#PKG_SOURCE_VERSION:=v03032018
#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MAINTAINER:=Bo Berry <bberryusa@users.sourceforge.net>
PKG_MIRROR_HASH:=350067d7c3307d6cc05a76071bb68ed1337986113b9036ffbc374655dce4f9be

PKG_LICENSE:=CISCO SYSTEMS
PKG_LICENSE_FILES:=COPYING

#PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/target.mk

CONFIGURE_ARGS += --disable-nullslack

define Package/safeclibs3/default
  TITLE:=Safec library build
  SECTION:=libs
  CATEGORY:=Libraries
  PROVIDES:=libsafec3
  URL:=http://sourceforge.net/projects/safeclib
  DEPENDS:=@!PACKAGE_libsafec
endef

define Package/safeclibs3
  $(call Package/safeclibs3/default)
  TITLE+= (default safec library)
endef

define Package/safeclibs3/description
 This package contains the safec  shared library.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/libsafec
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libsafec/* $(1)/usr/include/libsafec/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsafec-3.3.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/safec-3.3.pc $(1)/usr/lib/pkgconfig/
endef

define Package/safeclibs3/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsafec-3.3.so* $(1)/usr/lib/
endef


$(eval $(call BuildPackage,safeclibs3))
