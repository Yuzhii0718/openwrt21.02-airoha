ccflags-y += $(MODULE_FLAGS) -Inet/nat

MODULE_NAME=mt_whnat
KERNELDIR=$(KERNEL_DIR)
OBJS:= woe_main.o
OBJS+= woe_basic.o
OBJS+= wed.o
OBJS+= wdma.o
OBJS+= woe_proc.o
OBJS+= woe_hw.o
OBJS+= woe_client_jedi.o
OBJS+= woe_hif.o
OBJS+= woe_ser.o
OBJS+= rt_txrx_hook.o

#######################################
# For integrating to kernel source
######################################
obj-m += $(MODULE_NAME).o


$(MODULE_NAME)-objs += $(OBJS)

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded *.ko
	
install:

clean:
	rm -rf *.o *.ko *.modules .*.cmd .*.o *.order *.builtin $(MODULE_NAME).mod.c
