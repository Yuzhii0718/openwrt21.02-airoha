KERNELDIR=$(KERNEL_DIR)

EXTRA_CFLAGS += -I$(MODULES_XSI_AIR8811_MDIO_ARHT_DIR)/ -O0 
#-save-temps

MDIO-ARHT_SRC += mdio-arht.c
MDIO-ARHT_OBJ := $(MDIO-ARHT_SRC:%.c=%.o)

TOTAL_OBJ	:= $(MDIO-ARHT_OBJ)

mdio_arht-objs 	:= $(MDIO-ARHT_OBJ)
obj-m		+= mdio_arht.o


all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	$(STRIP) --strip-unneeded mdio_arht.ko

clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean