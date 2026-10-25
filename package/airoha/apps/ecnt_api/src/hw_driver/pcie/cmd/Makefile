ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) \
				  $(BSPAPI_INTCFLAGS) \
				  $(BSPAPP_CFLAG1)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lapi_lib_pcie

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
PCIE_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) 


##################################
########Configured output#########
##################################
OBJS	=	pcie.o
TARGETS	=	pcie

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(PCIE_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(PCIE_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)	
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	$(call BLApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

