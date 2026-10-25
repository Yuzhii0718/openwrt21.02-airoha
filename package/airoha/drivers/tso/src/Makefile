KERNELDIR=$(KERNEL_DIR)

EXTRA_CFLAGS += -O0 
#-save-temps
EXTRA_CFLAGS += -D CONFIG_DEBUG 

TSO_SRC += tso_v2.c tso_common.c
TSO_OBJ := $(TSO_SRC:%.c=%.o)


tso2-objs 	:= $(TSO_OBJ)
obj-m		:= tso2.o


all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded tso2.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean


