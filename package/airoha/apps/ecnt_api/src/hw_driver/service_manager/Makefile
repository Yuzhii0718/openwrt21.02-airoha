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


ifneq ($(BSP_CFLAGS), )
CFLAGS+=$(BSP_CFLAGS)
endif

ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
endif

ifneq ($(strip $(TCSUPPORT_SISM_HOST)),)
EXECS= hsm hps
else
EXECS= csm
endif

ifneq ($(strip $(TCSUPPORT_SISM_HOST)),)
EXECS_CLEAN= hsm_clean hps_clean
else
EXECS_CLEAN= csm_clean
endif

CFLAGS+= $(BSPAPP_CFLAGS) -I./inc/ -I$(BSP_EXT_INC)

HEADER	= ../pt/lib_pt.h ../inc/inic_common.h

all: $(EXECS) install

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $(CFLAGS_LINK_SO) $<

csm:
	$(MAKE) -C ./client_service_manager  all

csm_clean:
	$(MAKE) -C ./client_service_manager  clean

hsm:
	$(MAKE) -C ./host_service_manager  all

hsm_clean:
	$(MAKE) -C ./host_service_manager  clean
	
hps:
	$(MAKE) -C ./host_print_service  all

hps_clean:
	$(MAKE) -C ./host_print_service  clean
	
install:
	$(call EcntApiIncInstall, $(ROOTPATH)/inc, $(HEADER), 1)
	
clean:	$(EXECS_CLEAN)
	rm -f *.o *.a $(EXECS)
	$(call EcntApiIncClean, $(HEADER))


  
