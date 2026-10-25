ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(BSPAPI_INTCFLAGS) $(KERNELHEAD_CFLAGS)
INCLUDE			+= -I$(KERNEL_DIR)/include
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lecnt_lib_dbg
##################################
########Configured output#########
##################################
OBJS		=	api_lib_pcie.o
HEADER		=	libapi_lib_pcie.h
LINK_NAME	=	libapi_lib_pcie.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

LINK_NAME_EXTERNAL	=	libapi_lib_pcie_user.so
SONAME_EXTERNAL			=	$(LINK_NAME_EXTERNAL).1
REAL_NAME_EXTERNAL	=	$(SONAME_EXTERNAL).0.0

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			+= -O2 -fPIC -Wall $(INCLUDE) $(BSP_CFLAGS)
API_LIB_CFLAGS	= $(ARHT_SECURE_SO_FLAGS) -shared -Wl,-soname,$(SONAME)
API_LIB_CFLAGS_EXTERNAL	= $(ARHT_SECURE_SO_FLAGS) -shared -Wl,-soname,$(SONAME_EXTERNAL)

all: $(REAL_NAME) $(REAL_NAME_EXTERNAL) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(REAL_NAME): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS) -o $(REAL_NAME) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(REAL_NAME)

$(REAL_NAME_EXTERNAL): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS_EXTERNAL) -o $(REAL_NAME_EXTERNAL) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(REAL_NAME_EXTERNAL)
	
install:
	$(LN) $(REAL_NAME) $(LINK_NAME)
	$(LN) $(REAL_NAME) $(SONAME)
	$(LN) $(REAL_NAME_EXTERNAL) $(LINK_NAME_EXTERNAL)
	$(LN) $(REAL_NAME_EXTERNAL) $(SONAME_EXTERNAL)

	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), $(MAKEVAR))
	$(call EcntApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)
	$(call EcntApiLibInstall, $(ROOTPATH), $(LINK_NAME_EXTERNAL)*, 1)

	
clean:
	$(RM) $(OBJS) $(LINK_NAME)* $(LINK_NAME_EXTERNAL)*
	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiLibClean, $(LINK_NAME)*)
	$(call EcntApiLibClean, $(LINK_NAME_EXTERNAL)*)


