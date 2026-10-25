KERNELDIR=$(KERNEL_DIR)

EXTRA_CFLAGS := -I.

obj-m += ecnt_legacy_api.o
ecnt_legacy_api-objs += legacy_api.o ledctrl_kernel_api.o ledctrl_orig.o ecnt_gpio.o

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded ecnt_legacy_api.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean

KERNELDIR=$(KERNEL_DIR)

