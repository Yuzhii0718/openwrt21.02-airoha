ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) \
				  $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) $(BSP_BLLIB_UTILITY) -lapi_lib_atmmgr $(BSP_BLLIB_DBG)
USRSCRDIR		= $(BSP_EXT_FS)/usr/script

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
ATMMGR_CFLAGS	= -O2 -Wall $(INCLUDE) $(BSP_CFLAGS)
ATMMGR_CFLAGS	+= -D API_CMD_ATMMGR_DBG=0

##################################
########Configured output#########
##################################
OBJS	=	atmmgr.o
TARGETS	=	atm_cmd

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(ATMMGR_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(ATMMGR_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	cp wan_start_atm.sh $(USRSCRDIR)
	cp wan_stop_atm.sh $(USRSCRDIR)
	
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))
	$(RM) $(USRSCRDIR)/wan_start_atm.sh
	$(RM) $(USRSCRDIR)/wan_stop_atm.sh
