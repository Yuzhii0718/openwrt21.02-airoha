ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

INCLUDE	= $(BSPAPI_INTCFLAGS)

COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lapi_lib_qosmgr
USRBINDIR		= $(FILESYSTEM_DIR)/userfs/bin

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
QOSMGR_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS)

##################################
########Configured output#########
##################################
OBJS	=	qosmgr.o 
TARGETS	=	qosmgr

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(QOSMGR_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(QOSMGR_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))