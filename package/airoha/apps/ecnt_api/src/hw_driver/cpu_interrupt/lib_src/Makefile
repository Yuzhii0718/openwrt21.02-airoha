ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= -I$(KERNEL_DIR)/include
##################################
########Configured output#########
##################################
OBJS		=	api_lib_cpu_interrupt.o
HEADER		=	libapi_lib_cpu_interrupt.h
LINK_NAME	=	libapi_lib_cpu_interrupt.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

##################################
########Configured flags##########
##################################
RM			= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			+=-fPIC -Wall $(INCLUDE) $(BSP_CFLAGS)
API_LIB_CFLAGS	= $(ARHT_SECURE_SO_FLAGS) -shared -Wl,-soname,$(SONAME)

all: $(REAL_NAME) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(REAL_NAME): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS) -o $(REAL_NAME) $(OBJS)
	$(STRIP) --strip-unneeded $(REAL_NAME)

install:
	$(LN) $(REAL_NAME) $(LINK_NAME)
	$(LN) $(REAL_NAME) $(SONAME)

	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), $(MAKEVAR))
	$(call EcntApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)

clean:
	$(RM) $(OBJS) $(LINK_NAME)*
	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiLibClean, $(LINK_NAME)*)
	
.PHONY : $(EXT_INSTALL_TARGERT) clean install

