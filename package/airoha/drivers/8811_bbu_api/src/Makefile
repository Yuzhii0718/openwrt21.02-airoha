KERNELDIR=$(KERNEL_DIR)

EXTRA_CFLAGS += -O0
EXTRA_CFLAGS += $(BSP_CFLAGS)
#-save-temps

AIR8811_SRC += air_en8811h_api.c en8811_api.c en8811_hook.c en8811_proc.c en8811.c
AIR8811_OBJ := $(AIR8811_SRC:%.c=%.o)

TOTAL_OBJ	:= $(AIR8811_OBJ)

8811_bbu_api-objs 	:= $(TOTAL_OBJ)
obj-m		:= 8811_bbu_api.o

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded 8811_bbu_api.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
