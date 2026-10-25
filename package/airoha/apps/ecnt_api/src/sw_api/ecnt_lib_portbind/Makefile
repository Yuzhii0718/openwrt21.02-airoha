ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) \
					$(BSPAPI_CFLAGS)

COMPILE_LIBS		= $(BSPAPI_LDFLAGS) -lapi_lib_utility

##################################
########Configured output#########
##################################
OBJS		=	api_lib_portbind.o
HEADER		=	libapi_lib_portbind.h
LINK_NAME	=	libapi_lib_portbind.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS+=$(BSP_CFLAGS)
CFLAGS 			+=-fPIC -Wall $(INCLUDE)
#CFLAGS			+= -D API_LIB_DBG
API_LIB_CFLAGS	= $(ARHT_SECURE_SO_FLAGS) -shared -Wl,-soname,$(SONAME)

all: $(REAL_NAME) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(REAL_NAME): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS) -o $(REAL_NAME) $(OBJS) $(COMPILE_LIBS) 
	$(STRIP) --strip-unneeded $(REAL_NAME)

install:
	echo $(MAKEVAR)
	$(LN) $(REAL_NAME) $(LINK_NAME)
	$(LN) $(REAL_NAME) $(SONAME)
	$(call EcntApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)
	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(LINK_NAME)*
	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiLibClean, $(LINK_NAME)*)

.PHONY : $(EXT_INSTALL_TARGERT) clean install

