ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= ../inc

##################################
########Configured output#########
##################################
OBJS		=	pt.o
HEADER		=	lib_pt.h ../inc/inic_common.h
LINK_NAME	=	libpt.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			+=-fPIC -Wall -I$(INCLUDE) $(BSP_CFLAGS)
#CFLAGS			+= -D API_LIB_DBG
API_LIB_CFLAGS	= -shared -Wl,-soname,$(SONAME)

all: $(REAL_NAME) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(REAL_NAME): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS) -o $(REAL_NAME) $(OBJS)
	$(STRIP) --strip-unneeded $(REAL_NAME)

install:
	echo $(MAKEVAR)
	$(LN) $(REAL_NAME) $(LINK_NAME)
	$(LN) $(REAL_NAME) $(SONAME)
	$(call EcntApiIncInstall, $(ROOTPATH), $(HEADER), $(MAKEVAR))
	$(call EcntApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)
	$(call BLApiLibInstall, $(ROOTPATH), $(LINK_NAME)*, 1)
	
clean:
	$(RM) $(OBJS) $(LINK_NAME)*
	$(call EcntApiIncClean, $(HEADER))
	$(call EcntApiLibClean, $(LINK_NAME)*)
	$(call BLApiLibClean, $(LINK_NAME)*)
.PHONY : $(EXT_INSTALL_TARGERT) clean install
