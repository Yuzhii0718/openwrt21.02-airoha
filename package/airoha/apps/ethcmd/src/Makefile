ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

#
#Configured directories
#
#
#Install commands
#
INSTALL=	/usr/bin/install -c
INSTALL_DIR	=	$(INSTALL) -d
INSTALL_LIB	=	$(LIBTOOL)	$(INSTALL) -m 755
LN = /bin/ln -s
RM = /bin/rm -f

AR=$(CROSS)ar
LD=$(CROSS)ld
STRIP=$(CROSS)strip
RANLIB=$(CROSS)ranlib
SIZE=$(CROSS)size
LIBDIR=$(CROSS_PATH)/lib

ifeq ($(TCSUPPORT_ETH4_WAN_PORT),)
  	CFLAGS += -DVPORT
endif

ifneq ($(BSP_CFLAGS), )
        CFLAGS+=$(BSP_CFLAGS)
endif

ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
endif

CFLAGS+= $(BSPAPP_CFLAGS)

OBJS= ethcmd.o libcmd.o ethercmd.o libmii.o ether.o
EXECS= ethphxcmd

ifneq ($(strip $(TCSUPPORT_MT7530_SWITCH_API)),)
LIBSRCS = mtkswitch_api.c
HEADER	= mtkswitch_api.h
LIBMTKSWITCH = libmtkswitch
LIBOBJS = $(LIBMTKSWITCH).o
LIBSO = $(LIBMTKSWITCH).so.1.0
CFLAGS_LIB = -fPIC -Os -g -Wall
DSOFLAGS=	-Wl,-soname,$(LIBSO) -shared
SWAPI_LIBKRLDIR=$(TRUNK_DIR)/modules/private/tcphy
CFLAGS_LINK_SO = -L. -lmtkswitch
else
LIBOBJS =
LIBSO =
CFLAGS_LINK_SO =
endif

all: $(LIBSO) ethphxcmd install

.c.o:
	$(CC) -c $(ARHT_SECURE_OBJ_FLAGS) $(CFLAGS) $(INCLUDE) $(CFLAGS_LINK_SO) $<

ethphxcmd: $(OBJS) 
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(OBJS) $(INCLUDE) $(CFLAGS_LINK_SO) -o $@
	$(STRIP) --strip-unneeded $(EXECS)

#static library
mtkswitch_api.a: mtkswitch_api.o
	$(AR) cr $@ $^

#dynamically linked shared library
$(LIBMTKSWITCH).so.1.0 : $(LIBSRCS)
	$(CC) $(ARHT_SECURE_SO_FLAGS) $(CFLAGS) $(CFLAGS_LIB) -c $< -o $(LIBOBJS)
	$(CC) $(ARHT_SECURE_SO_FLAGS) $(CFLAGS) $(CFLAGS_LIB) $(DSOFLAGS) -o $@ $(LIBOBJS)
	$(STRIP) --strip-unneeded $@
	$(RM) $(LIBMTKSWITCH).so $(LIBMTKSWITCH).so.1
	$(LN) $(LIBMTKSWITCH).so.1.0 $(LIBMTKSWITCH).so
	$(LN) $(LIBMTKSWITCH).so.1.0 $(LIBMTKSWITCH).so.1
	
install:
	$(call BLApiLibInstall, $(ROOTPATH), $(LIBMTKSWITCH).so*, 1)

	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), 1)
	$(call BspAppUserFsBinInstall, $(ROOTPATH), $(EXECS))
	
clean:	
	rm -f *.o *.a $(EXECS)
	$(call BspAppUserFsBinClean, $(EXECS))

ifneq ($(strip $(TCSUPPORT_MT7530_SWITCH_API)),)					
	rm -f $(LIBMTKSWITCH).so*
	rm -f $(LIBOBJS)
	$(call EcntApiLibClean, $(LIBMTKSWITCH).so*)
endif

ifneq ($(strip $(TCSUPPORT_MT7530_SWITCH_API)),)
lib_clean:
	rm -f $(LIBMTKSWITCH).so
	rm -f $(LIBMTKSWITCH).so.1
	rm -f $(LIBMTKSWITCH).so.1.0
	rm -f $(LIBOBJS)
endif
  
