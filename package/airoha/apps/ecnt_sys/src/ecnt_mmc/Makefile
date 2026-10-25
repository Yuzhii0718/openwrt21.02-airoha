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

INCLUDE = -I$(KERNEL_HEADER) $(BSPAPP_CFLAG0)
LIB = -L$(BSP_INT_LIB) -lapi_lib_utility -lecnt_mmc 
LIB_TARGET=libecnt_mmc.a
BIN_TARGET=ecnt_mmc
LIB_OBJS=ecnt_mmc_api.o
BIN_OBJS=ecnt_mmc.o
HEADER=ecnt_mmc.h

all: $(LIB_TARGET) $(BIN_TARGET) install

%.o: %.c
	$(CC) $(ARHT_SECURE_OBJ_FLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) -c -o $@ $^

$(LIB_TARGET): $(LIB_OBJS)
	$(AR) rcs $(ARHT_SECURE_APP_FLAGS) -o $@ $^
	$(call EcntApiLibInstall, $(ROOTPATH), $(LIB_TARGET)*, 1)
	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), 1)
	
$(BIN_TARGET): $(BIN_OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) -o $@ $(BIN_OBJS) $(LIB)

install:
	$(call BspAppUserFsBinInstall, $(ROOTPATH), $(BIN_TARGET))
	
clean:
	$(RM) *.o $(BIN_TARGET) $(LIB_TARGET)
	$(call BspAppUserFsBinClean, $(BIN_TARGET))
	$(call EcntApiLibClean, $(LIB_TARGET)*)
	$(call EcntApiIncClean, $(HEADER))
