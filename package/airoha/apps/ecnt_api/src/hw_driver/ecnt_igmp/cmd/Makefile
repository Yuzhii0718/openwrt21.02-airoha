ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lapi_igmpmgr

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
ECNT_IGMP_CFLAGS = $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) 

##################################
########Configured output#########
##################################
OBJS	=	ecnt_igmp_cmd.o
TARGETS	=	ecnt_igmp_cmd

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(ECNT_IGMP_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(ECNT_IGMP_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

