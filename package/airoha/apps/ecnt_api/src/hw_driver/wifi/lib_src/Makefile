ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(BSPAPI_INTCFLAGS) $(KERNELHEAD_CFLAGS)
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lecnt_lib_dbg
ifneq ($(strip $(TCSUPPORT_WLAN_MULTI_CHIP)),)
ifneq ($(strip $(TCSUPPORT_PROLINECMD_NG)),)
COMPILE_LIBS+=  -lapi_lib_prolinecmd
endif
endif
##################################
########Configured output#########
##################################
OBJS		=	wifi_file.o api_lib_wifimgr.o api_lib_wifimgr_user.o
HEADER		=	libapi_lib_wifimgr.h 
HEADER_COMM		=	wifi_comm.h
LINK_NAME	=	libapi_lib_wifimgr.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

LINK_NAME_EXTERNAL	=	libapi_lib_wifimgr_user.so
SONAME_EXTERNAL			=	$(LINK_NAME_EXTERNAL).1
REAL_NAME_EXTERNAL	=	$(SONAME_EXTERNAL).0.0
##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      =	/bin/ln -s
CFLAGS 			+= -O2 -fPIC -Wall $(INCLUDE) $(BSP_CFLAGS) -I$(INSTALL_DIR)/include $(LIBINSTALL_CFLAGS) $(SDK_FLAGS)
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
	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER_COMM), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(LINK_NAME)* $(LINK_NAME_EXTERNAL)*
	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiIncClean, $(HEADER_COMM))
	$(call EcntApiLibClean, $(LINK_NAME)*)
	$(call EcntApiLibClean, $(LINK_NAME_EXTERNAL)*)


