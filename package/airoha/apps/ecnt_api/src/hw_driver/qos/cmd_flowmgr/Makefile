ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk
##################################
#####Configured directories#######
##################################
INCLUDE			= $(BSPAPI_INTCFLAGS)
				  
LIBDIR			= $(BSPAPI_INTLDFLAGS) -lapi_lib_flowmgr -lecnt_lib_dbg

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
QOS_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) 

##################################
########Configured output#########
##################################
OBJS	=	api_flow_cmd.o
TARGETS	=	acl

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(QOS_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(QOS_CFLAGS) -o $(TARGETS) $(OBJS) $(LIBDIR) 
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(TARGETS)	
	$(call EcntApiBinClean, $(TARGETS))
	