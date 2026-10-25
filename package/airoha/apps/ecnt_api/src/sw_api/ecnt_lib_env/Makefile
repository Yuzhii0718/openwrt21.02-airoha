ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) $(BSPAPI_CFLAGS)
##################################
########Configured output#########
##################################
OBJS		  =	crc32.o ctype.o env_attr.o env_flags.o fw_env.o linux_string.o

#HEADER		=	ecnt_lib_dbg.h
HEADER      = 

LINK_NAME	=	libecnt_env.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			+=-fPIC -Wall $(INCLUDE) $(BSP_CFLAGS) -I./include -I./arch/arm/include -DUSE_HOSTCC
API_LIB_CFLAGS	= $(ARHT_SECURE_SO_FLAGS) -shared -fPIC -Wl,-soname,$(SONAME)

all: $(REAL_NAME) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(REAL_NAME): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS) -o $(REAL_NAME) $(OBJS) $(BSPAPI_LDFLAGS) -lapi_lib_utility
	$(STRIP) --strip-unneeded $(REAL_NAME)

install:
	$(LN) $(REAL_NAME) $(LINK_NAME)
	$(LN) $(REAL_NAME) $(SONAME)
	$(call EcntApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)
#	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(LINK_NAME)*
#	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiLibClean, $(LINK_NAME)*)
	
.PHONY : $(EXT_INSTALL_TARGERT) clean install
