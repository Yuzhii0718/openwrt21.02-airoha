KERNELDIR=$(KERNEL_DIR)

EXTRA_CFLAGS += -DXSI_DEBUG -DWAN2LAN

XSIMAC_SRC += ae_wan_mac.c ae_wan_api.c ae_wan_proc.c
XSIMAC_OBJ := $(XSIMAC_SRC:%.c=%.o)

ae_wan-objs 	:= $(XSIMAC_OBJ)
obj-m		+= ae_wan.o


all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded ae_wan.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean