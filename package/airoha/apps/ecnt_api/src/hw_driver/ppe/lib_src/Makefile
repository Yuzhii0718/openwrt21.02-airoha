ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################

##################################
########Configured output#########
##################################
OBJS		=	api_lib_ppe.o
HEADER		=	libapi_lib_ppe.h
LINK_NAME	=	libapi_lib_ppe.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			+=-fPIC -Wall $(BSP_CFLAGS) -DCONFIG_HNAT_V2 -DBIG_ENDIAN

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

	$(call EcntApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)
	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER))
	
clean:
	$(RM) $(OBJS) $(LINK_NAME)*
	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiLibClean, $(LINK_NAME)*)

