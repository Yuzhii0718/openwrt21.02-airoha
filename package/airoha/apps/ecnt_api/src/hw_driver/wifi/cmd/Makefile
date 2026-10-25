ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) \
				  $(BSPAPI_INTCFLAGS) \
				  $(BSPAPP_CFLAG1)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) $(BSP_BLLIB_UTILITY) $(BSP_BLLIB_DBG) -lapi_lib_wifimgr

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
WIFIMGR_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS)  -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) 
WIFIMGR_CFLAGS+= -D API_CMD_WIFIMGR_DBG=0

##################################
########Configured output#########
##################################
OBJS	=	wifimgr.o wifimgr_get.o wifimgr_set.o
TARGETS	=	wifimgr

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(WIFIMGR_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(WIFIMGR_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS) 
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)	
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	$(call BLApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

