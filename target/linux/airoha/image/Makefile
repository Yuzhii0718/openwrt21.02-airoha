#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define CompressLzma
		echo "\r\n\r\n Win847: CompressLzma $(1) $(2) \r\n\r\n"
	$(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

#
# All intermediate artifacts (vmlinux*, uImage*, padded rootfs, TRX payload)
# stay in $(KDIR). Only flashable images are written to $(BIN_DIR):
#   tclinux.bin / tclinux.img  and the device *.itb files built below.
#
# The legacy tclinux (TRX-wrapped) image is the EN7563 default. When the
# EN7563 FIT image is selected (CONFIG_TARGET_AIROHA_EN7563_FIT_IMAGE), the
# Device framework below builds the .itb instead and these legacy macros must
# not run, otherwise both image types are emitted. Gated at parse time.
ifneq ($(CONFIG_TARGET_AIROHA_EN7563_FIT_IMAGE),y)

define Image/BuildKernel
	echo "\r\n\r\n Win847: BuildKernel\r\n\r\n"
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
endef

define Image/Build
	echo "\r\n\r\n Win847: Image Build $(1) PROFILE=$(PROFILE) KDIR=$(KDIR) PLATFORM_DIR=$(PLATFORM_DIR)\r\n\r\n"
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(KDIR)/root.$(1).padded bs=128k conv=sync

	$(eval size_k=`stat -c%s "$(KDIR)/vmlinux.bin.lzma"`)
	$(eval size_r=`stat -c%s "$(KDIR)/root.$(1).padded"`)

	cat $(KDIR)/vmlinux.bin.lzma $(KDIR)/root.$(1).padded > $(BIN_DIR)/tclinux
ifneq ($(strip $(TCSUPPORT_CPU_EN7523)),)
	cp -a $(PLATFORM_DIR)/$(SUBTARGET)/uImage.its $(KDIR)/
ifneq ($(strip $(TCSUPPORT_CPU_EN7581)),)
	cp -a $(KDIR)/linux-$(LINUX_VERSION)/arch/arm64/boot/dts/econet/en7581_evb.dtb $(KDIR)/ecnt.dtb
else
ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)
	cp -a $(KDIR)/linux-$(LINUX_VERSION)/arch/arm/boot/dts/en7552_evb.dtb $(KDIR)/ecnt.dtb
else
ifneq ($(strip $(TCSUPPORT_KERNEL_UPSTREAM)),)
	cp -a $(KDIR)/linux-$(LINUX_VERSION)/arch/arm/boot/dts/en7523-evb.dtb $(KDIR)/ecnt.dtb
else
	cp -a $(KDIR)/linux-$(LINUX_VERSION)/arch/arm/boot/dts/en7523_evb.dtb $(KDIR)/ecnt.dtb
endif
endif
endif

ifneq ($(strip $(TCSUPPORT_NEW_SPI)),)
ifneq ($(strip $(TCSUPPORT_CPU_EN7581)),)
	cp -a $(KDIR)/linux-$(LINUX_VERSION)/arch/arm64/boot/dts/econet/en7581_new_spi.dtb $(KDIR)/ecnt.dtb
else ifneq ($(strip $(TCSUPPORT_CPU_AN7552)),)
	cp -a $(KDIR)/linux-$(LINUX_VERSION)/arch/arm/boot/dts/en7552_new_spi.dtb $(KDIR)/ecnt.dtb
else ifneq ($(strip $(TCSUPPORT_CPU_EN7523)),)
	cp -a $(KDIR)/linux-$(LINUX_VERSION)/arch/arm/boot/dts/en7523_new_spi.dtb $(KDIR)/ecnt.dtb
else 
	echo "none of the boards are selected"
endif
endif 
	cp $(KDIR)/vmlinux.bin.lzma $(KDIR)/linux.7z
	cp $(KDIR)/root.$(1).padded $(KDIR)/rootfs
	cd $(KDIR) && $(STAGING_DIR_HOST)/bin/mkimage -f uImage.its $(BIN_DIR)/tclinux
	$(STAGING_DIR_HOST)/bin/ecnt_trx -k $(size_k) -r $(size_r) -f $(BIN_DIR)/tclinux -o $(KDIR)/tclinux.trx -c $(PLATFORM_DIR)/image/trx_config
else
	$(STAGING_DIR_HOST)/bin/trx -k $(size_k) -r $(size_r) -u 0x80002000 -f $(BIN_DIR)/tclinux -o $(KDIR)/tclinux.trx -c $(PLATFORM_DIR)/image/trx_config
endif
	cp $(KDIR)/tclinux.trx $(BIN_DIR)/tclinux.bin

	cp -rf $(BIN_DIR)/tclinux $(BIN_DIR)/tclinux.img
	rm -f $(BIN_DIR)/tclinux
endef

endif # !CONFIG_TARGET_AIROHA_EN7563_FIT_IMAGE

FIT_KERNEL_LOADADDR := 0x80088000

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := vmlinux
  KERNEL_LOADADDR := $(FIT_KERNEL_LOADADDR)
  DEVICE_DTS_DIR := $(DTS_DIR)
  FILESYSTEMS := squashfs
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
	append-metadata
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | pad-to 128k
endef

define Device/airoha_an7552-evb-ubi
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7552-EVB
  DEVICE_VARIANT := UBI
  DEVICE_DTS := en7552_evb-ubi
  SUPPORTED_DEVICES += airoha,an7552-evb
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | pad-to 128k
  KERNEL_INITRAMFS_SUFFIX := -recovery.itb
  IMAGES := sysupgrade.itb
  IMAGE/sysupgrade.itb := append-kernel | \
	fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
	append-metadata
  DEVICE_PACKAGES += fitblk nand-utils ubi-utils
endef

# Only register the FIT device when the EN7563 FIT image is selected. With the
# switch off (the default) TARGET_DEVICES stays empty, so the legacy tclinux
# image above is the only output.
ifeq ($(CONFIG_TARGET_AIROHA_EN7563_FIT_IMAGE),y)
  TARGET_DEVICES += airoha_an7552-evb-ubi
endif

$(eval $(call BuildImage))
