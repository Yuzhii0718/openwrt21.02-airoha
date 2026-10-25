ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= -I$(KERNEL_DIR)/arch/mips/include \
				  $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lapi_lib_ppe 

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
PPE_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) -DCONFIG_HNAT_V2 -DBIG_ENDIAN  $(SDK_FLAGS)

##################################
########Configured output#########
##################################
OBJS	=	ppe_main.o
TARGETS	=	ppemgr

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(PPE_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(PPE_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)	
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

