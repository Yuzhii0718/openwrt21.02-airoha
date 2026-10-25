ROOTPATH=$(shell pwd)

KERNELDIR := $(KERNEL_DIR)

EXTRA_CFLAGS += -O0
EXTRA_CFLAGS += -D CONFIG_DEBUG
EXTRA_CFLAGS += $(BSP_CFLAGS)

obj-m += ifc.o

# ifc_tcam.o ifc_util.o ifc_api.o ifc_dev.o are the vendor's closed objects
# (UNOPEN_FILES); the vendor build links them from
# BSP/critical_ip_bsp/dp_bsp/ifc. ifc_gen.c is the open part.
ifc-objs := ifc_tcam.o ifc_util.o ifc_api.o ifc_dev.o ifc_gen.o

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded ifc.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
