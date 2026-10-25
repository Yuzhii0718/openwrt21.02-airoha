#
# Copyright (C) 2009 OpenWrt.org
#

ARCH:=arm
SUBTARGET:=en7563
BOARDNAME:=en7563 based boards
CPU_TYPE:=cortex-a7
FEATURES:=squashfs nand ramdisk

KERNEL_PATCHVER:=5.4

define Target/Description
	Build firmware images for Airoha en7552 ARM based boards.
endef

