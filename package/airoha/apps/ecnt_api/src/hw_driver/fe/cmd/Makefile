ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= -I$(KERNEL_DIR)/include \
					$(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lapi_lib_fe

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
FE_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) -DCONFIG_HNAT_V2 -DBIG_ENDIAN

##################################
########Configured output#########
##################################
OBJS	=	fe.o
TARGETS	=	femgr

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(FE_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(FE_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

