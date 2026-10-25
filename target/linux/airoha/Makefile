#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

#ARCH:=mips
BOARD:=airoha
BOARDNAME:=Airoha EN75xx
SUBTARGETS:=en7523 en7562 en7581 en7566 en7551 en7552 en7563
FEATURES:=squashfs usb

KERNEL_PATCHVER:=5.4
KERNEL_TESTING_PATCHVER:=5.4

include $(INCLUDE_DIR)/kernel-version.mk
include $(INCLUDE_DIR)/target.mk
include $(PLATFORM_SUBDIR)/*.mak
include $(PLATFORM_DIR)/dir.mak
include $(PLATFORM_DIR)/rule.mak
#include $(TOPDIR)/target/linux/ecnt75xx/en7521_kernel_3_18.profile
#include $(TOPDIR)/target/linux/ecnt75xx/rule.mak

define Target/Description
	Build firmware images for Airoha MT751x2x based boards.
endef

$(eval $(call BuildTarget))
