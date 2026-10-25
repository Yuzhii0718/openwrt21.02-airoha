ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

KERNELDIR=$(KERNEL_DIR)
obj-m += tccicmd.o
ifeq ($(TCSUPPORT_CPU_ARMV8),1)
tccicmd-objs := tccimain.o tcsyscmd.o version.o
else
tccicmd-objs := tccimain.o tcadslcmd.o tcsyscmd.o version.o
endif

#TCSUPPORT_BUS_TEST=1
ifeq ($(TCSUPPORT_BUS_TEST),1)
tccicmd-objs += cpu_bus_test.o ramTest.o
ifeq ($(TCSUPPORT_CPU_ARMV8),1)
tccicmd-objs += cpu_bus_arm_test.o
endif
EXTRA_CFLAGS+=-DTCSUPPORT_BUS_TEST
endif

EXTRA_CFLAGS+=-DDMA_API
ifneq ($(TCSUPPORT_OPENWRT),)
ifeq ($(__DATE__),)
EXTRA_CFLAGS+=-D__DATE__=\""$(shell date +%Y:%m:%d)"\"
endif
ifeq ($(__TIME__),)
EXTRA_CFLAGS+=-D__TIME__=\""$(shell date +%T)"\"
endif
endif
#ifeq ($(TC3262),1)
#STRIP=mips-linux-uclibc-strip
#else
#STRIP=mips-linux-strip
#endif
ifeq ($(TC3262_FPGA),1)
EXTRA_CFLAGS+=-DTC3262_FPGA
endif
ifeq ($(SAR_VERIFY),1)
EXTRA_CFLAGS+= -DSAR_VERIFY
tc3162l2sar-objs += sar_verify.o
endif

EXTRA_CFLAGS+= -DL2_AUTOPVC
tc3162l2sar-objs += autoPVC.o

ifeq ($(PURE_BRIDGE),1)
EXTRA_CFLAGS += -DPURE_BRIDGE
endif

ifeq ($(WSC_AP_SUPPORT), 1)
EXTRA_CFLAGS += -DWSC_AP_SUPPORT
endif

ifeq ($(LED_WPSSPEC_COMPLY),1)
EXTRA_CFLAGS += -DLED_WPSSPEC_COMPLY
endif

EXTRA_CFLAGS+=-DMII_INTERFACE

ifeq ($(TC2031_SUPPORT),1)
#The orginal etherd root ci-cmd is changed to tce ci-cmd.
#If you want to IOT test.We must to add -DTCPHY_DEBUG compiler option 
EXTRA_CFLAGS+= -DTCPHY_SUPPORT -DTCPHY_1PORT
tc3162l2mac-objs += ../tcetherphy.o tcethercmd.o
endif


#Default the mac driver is supported TC2206 Switch IC.
#ifeq ($(TC2206_SUPPORT),1)
EXTRA_CFLAGS+= -DTCPHY_SUPPORT -DTCPHY_4PORT -DWAN2LAN
tc3162l2mac-objs += ../tcetherphy.o tcethercmd.o
#endif

ifneq ($(strip $(TCSUPPORT_LED_BTN_CHECK)),)
EXTRA_CFLAGS+= -DTCSUPPORT_LED_BTN_CHECK
endif

ifeq ($(SAR_POLLING_MODE),1)
EXTRA_CFLAGS+= -DSAR_POLLING
endif

ifeq ($(EEE_SUPPORT),1)
EXTRA_CFLAGS+= -DEEE_SUPPORT -DTCPHY_SUPPORT -DTCPHY_1PORT
tc3162l2mac-objs += ../tcetherphy.o tcethercmd.o psm_verify.o 
endif
ifeq ($(CMD_API),1)
EXTRA_CFLAGS+= -DCMD_API
endif
EXTRA_CFLAGS+= -DCWMP

ifeq ($(TR068_LED),1)
EXTRA_CFLAGS+= -DTR068_LED
endif

EXTRA_CFLAGS+= $(call cc-option,-Wno-error=date-time)

all:
#ifeq ($(SAR_VERIFY),1)
#	cp tc3162l2sar.c sar.c
#endif
#ifeq ($(TC2031_SUPPORT),1)
#	cp tc3162l2mac.c mac.c
#endif
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded *.ko
	$(call EcntBSPReleaseInstall,  $(ROOTPATH), version.o, $(RELEASE_BSP_DIR)/$(TCPLATFORM)/modules/private/tcci)

#	$(STRIP) --strip-unneeded tc3162l2sar.ko
#	$(STRIP) --strip-unneeded tccicmd.ko	
#	$(STRIP) --strip-unneeded tcledctrl.ko
#ifneq ($(TC3262),1)
#	$(STRIP) --strip-unneeded tc3162l2mac.ko
#	$(STRIP) --strip-unneeded tc3162_udc.ko
#	$(STRIP) --strip-unneeded tc3162_udc20.ko
#endif

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean

