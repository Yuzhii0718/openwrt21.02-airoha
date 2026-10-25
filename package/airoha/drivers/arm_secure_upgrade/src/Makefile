KERNELDIR=$(KERNEL_DIR)


ifneq ($(strip $(TCSUPPORT_CPU_ARMV8_64)),)
secureupgrade-objs = smccc-call-64.o
else
secureupgrade-objs = smccc-call-32.o
endif

secureupgrade-objs += secure_upgrade.o
obj-m := secureupgrade.o

all:
	echo "KERNELDIR " $(KERNELDIR)
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded *.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean

