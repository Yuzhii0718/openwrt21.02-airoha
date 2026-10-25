#for OpenWRT and RDKB
ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

#CROSS=mips-linux-gnu-
#CROSS=mips-linux-
#CC=$(CROSS)gcc
AR=$(CROSS)ar
LD=$(CROSS)ld
STRIP=$(CROSS)strip
RANLIB=$(CROSS)ranlib
SIZE=$(CROSS)size

CFLAGS= -DTRENDCHIP
ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
endif
#CFLAGS= -mips32r2 -msoft-float -DTRENDCHIP -muclibc
ifneq ($(BSP_CFLAGS), )
        CFLAGS+=$(BSP_CFLAGS)
endif

INCLUDE = -I$(TOOLS_TRX_DIR) -I$(GLOBAL_FLASH_LAYOUT_INC_DIR) $(BSPAPP_CFLAG0)
OBJS= ecnt_sys.o ecnt_sys_mmc.o ecnt_sys_mtd.o
all: ecnt_sys

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) -c -o $@ $^

LIB= -L$(BSP_INT_LIB) -lapi_lib_utility -lecnt_mmc 
ecnt_sys: $(OBJS)
	$(CC) -o $@ $^ $(LIB)
	$(STRIP) $@

clean:
	rm -f *.o ecnt_sys
