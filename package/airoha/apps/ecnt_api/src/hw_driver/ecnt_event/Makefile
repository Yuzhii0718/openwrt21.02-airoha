ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk
#
#Configured directories
#
#prefix	=	/usr/local
#exec_prefix	=	/usr/local
#libdir	=	$(exec_prefix)/lib
#
#Install commands
#
INSTALL=	/usr/bin/install -c
INSTALL_DIR	=	$(INSTALL) -d
INSTALL_LIB	=	$(LIBTOOL)	$(INSTALL) -m 755

#CROSS=mips-linux-gnu-

#CC=$(CROSS)gcc -mips32r2 -msoft-float -muclibc
AR=$(CROSS)ar
LD=$(CROSS)ld 
STRIP=$(CROSS)strip
RANLIB=$(CROSS)ranlib
SIZE=$(CROSS)size
LIBDIR=$(CROSS_PATH)/lib

DSOFLAGS=	$(ARHT_SECURE_SO_FLAGS) -shared $(OPTIM)
LN      =	/bin/ln -s
RM      =	/bin/rm -f
OPTIM	=	-fPIC -Os -g

INCLUDE = $(KERNELHEAD_CFLAGS) 

#CFLAGS= -mips1 -msoft-float 
CFLAGS +=-fPIC -Os -g  -Wall $(BSP_CFLAGS)
LDFLAGS=-lpthread

ifeq ($(WITHVOIP),1)
CFLAGS += -DWITHVOIP
endif #WITHVOIP

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

LIBOBJS =       libecntevent.o
LIBECNTEVENT=   libecntevent.so
TARGETS =       $(LIBECNTEVENT)
HEADER =	libecntevent.h

all:  $(TARGETS) install

$(TARGETS): libecntevent.o
	$(CC) $(CFLAGS) $(DSOFLAGS) -o libecntevent.so $(LIBOBJS) $(LDFLAGS)
	$(STRIP) --strip-unneeded $(LIBECNTEVENT)

install:
	echo $(MAKEVAR)
	$(call EcntApiLibInstall, $(ROOTPATH), $(TARGETS), 1)
	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), $(MAKEVAR))
		
clean:
	$(RM) $(TARGETS) $(LIBOBJS)
	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiLibClean, $(TARGETS))
	
.PHONY : $(EXT_INSTALL_TARGERT) clean install
