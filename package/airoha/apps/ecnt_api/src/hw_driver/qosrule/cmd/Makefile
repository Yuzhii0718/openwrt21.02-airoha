ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(KERNELHEAD_CFLAGS) $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) $(COMPILEOPTION_LIB) -lapi_lib_utility -lapi_lib_fe -lapi_lib_switchmgr -lapi_lib_qosrule

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
QOSRULE_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) -DBIG_ENDIAN

##################################
########Configured output#########
##################################
OBJS	=	qosrule.o
TARGETS	=	qosrule

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(QOSRULE_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(QOSRULE_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)	
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

