EXTRA_CFLAGS+= -DCWMP
EXTRA_CFLAGS+= -DTR068_LED

ifeq ($(strip $(TCSUPPORT_KERNEL_API)),)
obj-m += tcledctrl.o
tcledctrl-objs := ledchain.o ledtrig-dev.o ledctrl.o
endif

obj-m += module_sel.o



all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded *.ko


clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean

