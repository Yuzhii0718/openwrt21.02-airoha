ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk
##################################
#####Configured directories#######
##################################


##################################
########Configured output#########
##################################
OBJS		=	api_lib_prolinecmd.o
HEADER		=	libapi_lib_prolinecmd.h 
LINK_NAME	=	libapi_lib_prolinecmd.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			= -fPIC -Wall $(BSPAPI_INTCFLAGS) $(BSP_CFLAGS)
API_LIB_CFLAGS	= -fPIC $(ARHT_SECURE_SO_FLAGS) -shared -Wl,-soname,$(SONAME)

all: $(REAL_NAME) install
	
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(REAL_NAME): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS) -o $(REAL_NAME) $(OBJS) $(BSPAPI_INTLDFLAGS) -lapi_lib_utility
	$(STRIP) --strip-unneeded $(REAL_NAME)
	$(LN) libapi_lib_prolinecmd.so.1.0 libapi_lib_prolinecmd.so
	$(LN) libapi_lib_prolinecmd.so.1.0 libapi_lib_prolinecmd.so.1
	
install:
	$(call BLApiIncInstall, $(ROOTPATH), $(HEADER), 1)
	$(call BLApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)
ifeq ($(strip $(TCSUPPORT_OPENWRT) $(RDKB_BUILD)),)
	cp -f $(BSP_PROLINECMD_DIR)/src/lib_src/libapi_lib_prolinecmd.h $(INSTALL_GLOBALINC)/uapi/flash_layout/
	cp -f $(BSP_PROLINECMD_DIR)/src/lib_src/libapi_lib_prolinecmd_flag.h $(INSTALL_GLOBALINC)/uapi/flash_layout/
endif
	
clean:
	$(RM) $(OBJS) $(REAL_NAME) $(LINK_NAME)*
	$(call BLApiIncClean, $(HEADER))
	$(call BLApiLibClean, $(LINK_NAME)*)
	rm -f $(INSTALL_GLOBALINC)/uapi/flash_layout/libapi_lib_prolinecmd.h
	rm -f $(INSTALL_GLOBALINC)/uapi/flash_layout/libapi_lib_prolinecmd_flag.h

