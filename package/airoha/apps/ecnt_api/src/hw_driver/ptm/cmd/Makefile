ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) \
				  $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lapi_lib_ptmmgr -lecnt_lib_dbg

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
PTMMGR_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS)
PTMMGR_CFLAGS	+= -D API_CMD_PTMMGR_DBG=0

##################################
########Configured output#########
##################################
OBJS	=	ptmmgr.o
TARGETS	=	ptm_cmd
SCRIPTS	=	wan_start_ptm.sh

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(PTMMGR_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(PTMMGR_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)
	#cp wan_start_ptm.sh $(USRSCRDIR)
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	$(call EcntApiBinInstall, $(ROOTPATH), $(SCRIPTS), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))
	$(call EcntApiBinClean, $(SCRIPTS))

