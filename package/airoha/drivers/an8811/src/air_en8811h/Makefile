KERNELDIR=$(KERNEL_DIR)

EXTRA_CFLAGS += -I$(MODULES_XSI_AN8811_ENB_PHY_DIR)/ -O0 
#-save-temps

AIR8811_SRC += air_en8811h.c  
AIR8811_OBJ := $(AIR8811_SRC:%.c=%.o)

TOTAL_OBJ	:= $(AIR8811_OBJ)

air8811-objs 	:= $(TOTAL_OBJ)
obj-m		:= air_en8811h.o

all:
	@echo $(MODULES_XSI_AN8811_ENB_PHY_DIR)
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded air_en8811h.ko 

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean