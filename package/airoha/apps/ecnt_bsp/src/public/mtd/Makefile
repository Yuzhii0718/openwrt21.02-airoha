ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

# $Id$

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

EXT_LIBS=
ifneq ($(strip $(TCSUPPORT_OPENWRT)),)
EXT_LIBS += -Wl,-Bdynamic -lgcc_s
endif

INCLUDE = -I$(KERNEL_HEADER)
TARGET=mtd
OBJS=mtd.o

all: $(TARGET) install

%.o: %.c
	$(CC) $(ARHT_SECURE_OBJ_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) -c -o $@ $^

$(TARGET): $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) -o $@ $^
	$(CC) -static -o mtd-static $^ $(EXT_LIBS)
	$(STRIP) $@
	$(STRIP) mtd-static

install:
	$(call BspAppUserFsBinInstall, $(ROOTPATH), $(TARGET))
	
clean:
	$(RM) $(OBJS) $(TARGET)
	$(call BspAppUserFsBinClean, $(TARGET))
