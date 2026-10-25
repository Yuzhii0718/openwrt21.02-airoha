ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) \
				  $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS	= $(BSPAPI_INTLDFLAGS) $(BSP_BLLIB_UTILITY) -lapi_lib_cputempmgr $(BSP_BLLIB_DBG)

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
CMD_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS)
CMD_CFLAGS	+= -D API_CMD_CPUTEMPMGR_DBG=0

##################################
########Configured output#########
##################################
OBJS	=	cputempmgr.o
TARGETS	=	cputemp_cmd

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CMD_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(CMD_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)	
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))
