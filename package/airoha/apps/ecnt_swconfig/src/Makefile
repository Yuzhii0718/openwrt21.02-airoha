KERNELDIR=$(KERNEL_DIR)

SWCONFIG_SRC += mt753x_swconfig.c
SWCONFIG_OBJ := $(SWCONFIG_SRC:%.c=%.o)

ecnt_swconfig-objs 	:= $(SWCONFIG_OBJ)
obj-m		+= ecnt_swconfig.o


all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded ecnt_swconfig.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
