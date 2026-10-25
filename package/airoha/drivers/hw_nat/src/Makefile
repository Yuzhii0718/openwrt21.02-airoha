ROOTPATH=$(shell pwd)

KERNELDIR := $(KERNEL_DIR)

obj-m = hw_nat.o

EXTRA_CFLAGS += -DCONFIG_HNAT_V2 -DCONFIG_RA_HW_NAT_IPV6 -DTCSUPPORT_RA_HWNAT_ENHANCE_HOOK
EXTRA_CFLAGS += -Wall -Wframe-larger-than=5120
EXTRA_CFLAGS += $(BSP_CFLAGS)

# ra_nat_dev.o foe_fdb.o util.o ra_nat_ety.o are the vendor's closed objects
# (UNOPEN_FILES), built for this SoC and the same SDK revision. The vendor
# links them the same way from release_bsp/<platform>/modules/private/ra_hwnat_v3.
# hw_nat_compat.o supplies the symbols that only the newer vendor objects carry.
hw_nat-objs := hw_nat_compat.o \
	ra_nat_dev.o foe_fdb.o util.o hwnat_ioctl.o ppe_api.o \
	ra_nat_multicast.o ra_nat_ety.o ra_nat_offload.o ra_nat_gen.o \
	ra_nat_cust.o ra_nat_adp.o ra_nat_npu.o ra_nat_wifi.o ra_nat_entry.o

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded hw_nat.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
