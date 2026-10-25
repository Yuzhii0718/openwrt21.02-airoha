ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

EXEC = hw_nat

COMPILE_LIBS = $(BSPAPP_LDFLAGS) -lapi_lib_utility -lapi_lib_ppe $(COMPILEOPTION_LIB)
INCLUDE			= $(BSPAPP_CFLAGS)

HWNAT_OBJS = hw_nat.o hwnat_api.o
CFLAGS += $(ARHT_SECURE_OBJ_FLAGS) -DCONFIG_HNAT_V2 $(BSP_CFLAGS) -DBIG_ENDIAN

CFLAGS+=$(INCLUDE) 


all: $(EXEC) install

hw_nat: $(HWNAT_OBJS) 
	
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(LDFLAGS) $(INCLUDE)  -o $@ $(HWNAT_OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded hw_nat

install:
	$(call BspAppUserFsBinInstall, $(ROOTPATH), $(EXEC))
	
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
	$(call BspAppUserFsBinClean, $(EXEC))
