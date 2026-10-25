KERNELDIR=$(KERNEL_DIR)

EXTRA_CFLAGS += $(BSP_CFLAGS)

econet_bob-objs := ecnt_ready_bob.o
obj-m += econet_bob.o

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded econet_bob.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
