ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) \
				  $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS			= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lapi_lib_xdslmgr -lecnt_lib_dbg

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
XDSLMGR_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS)
XDSLMGR_CFLAGS	+= -D API_CMD_XDSLMGR_DBG=0

##################################
########Configured output#########
##################################
OBJS	=	xdslmgr.o
TARGETS	=	xdsl_cmd

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(XDSLMGR_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(XDSLMGR_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)	
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

