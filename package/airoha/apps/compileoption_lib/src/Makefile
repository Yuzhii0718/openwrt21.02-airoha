#
#Configured directories
#
#prefix	=	/usr/local
#exec_prefix	=	/usr/local
#
#Install commands
#
INSTALL=	/usr/bin/install -c
INSTALL_DIR	=	$(INSTALL) -d
INSTALL_LIB	=	$(LIBTOOL)	$(INSTALL) -m 755
ROOTPATH=$(shell pwd)

#CROSS=mips-linux-gnu-

#CC=$(CROSS)gcc -mips32r2 -msoft-float -muclibc
AR=$(CROSS)ar
LD=$(CROSS)ld 
STRIP=$(CROSS)strip
RANLIB=$(CROSS)ranlib
SIZE=$(CROSS)size
LIBDIR=$(CROSS_PATH)/lib

LIBOBJS =       libcompileoption.o
TARGETS =       libcompileoption_bsp.so
HEADER  =       libcompileoption.h
LN      =	/bin/ln -s
RM      =	/bin/rm -f


#INCLUDE = -I$(KERNEL_DIR)/include 

#CFLAGS= -mips1 -msoft-float 
CFLAGS += -fPIC -Os -g -Wall $(ARHT_SECURE_SO_FLAGS) -shared $(BSP_CFLAGS)

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<

all:  $(TARGETS)

$(LIBOBJS): libcompileoption.h
libcompileoption.o:libcompileoption.h

$(TARGETS): $(LIBOBJS)
	$(CC) $(CFLAGS) $(DSOFLAGS) -o $(TARGETS) $(LIBOBJS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	cp -f $(TARGETS) $(BSP_INT_LIB)
	cp -f $(TARGETS) $(BSP_EXT_LIB)
	cp -f $(HEADER) $(BSP_INT_INC)
	cp -f $(HEADER) $(BSP_EXT_INC)

clean:
	$(RM) $(OBJS) $(TARGETS) $(LIBOBJS) $(SOFTLINK)
	$(RM) -f $(BSP_INT_LIB)/$(TARGETS)
	$(RM) -f $(BSP_EXT_LIB)/$(TARGETS)
	$(RM) -f $(BSP_INT_INC)/$(HEADER)
	$(RM) -f $(BSP_EXT_INC)/$(HEADER)
