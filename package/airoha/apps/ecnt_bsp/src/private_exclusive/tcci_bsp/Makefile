ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

#CROSS=mips-linux-gnu-

#CC=$(CROSS)gcc
AR=$(CROSS)ar
LD=$(CROSS)ld
STRIP=$(CROSS)strip
RANLIB=$(CROSS)ranlib
SIZE=$(CROSS)size

ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
endif

CFLAGS +=$(BSPEXCL_CFLAGS)

LIBS= $(BSPEXCL_LDFLAGS) $(BSP_BLLIB_SYSTEM) $(BSP_BLLIB_DBG) $(BSP_BLLIB_TC_PARTITION)  $(BSP_BLLIB_UTILITY) $(COMPILEOPTION_LIB)

ifneq ($(BSP_CFLAGS), )
CFLAGS += $(BSP_CFLAGS)
endif

OBJS=tcci.o common.o
EXECS=tcci

all: $(EXECS) install 

.c.o:
	$(CC) -c $(ARHT_SECURE_OBJ_FLAGS) $(CFLAGS)  $<

$(EXECS): $(OBJS) 	
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(CFLAGS) $(OBJS) -o $@ $(LIBS)
	$(STRIP) $(EXECS)
	
install:
	$(call BspExclUsrBinInstall, $(ROOTPATH), $(EXECS))

clean:
	rm -f *.o *.a $(EXECS)
	$(call BspExclUsrBinClean, $(EXECS))
