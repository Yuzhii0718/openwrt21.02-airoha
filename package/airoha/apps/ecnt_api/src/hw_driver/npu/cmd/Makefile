##################################
#####Configured directories#######
##################################
ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) $(BSP_BLLIB_UTILITY)

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
NPU_CMD_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(BSP_CFLAGS) $(BSPAPI_INTCFLAGS)

##################################
########Configured output#########
##################################
OBJS	=	npu_bridge_cmd.o
TARGETS	=	npu_bridge_cmd

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(NPU_CMD_CFLAGS) -c $<
	

$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(NPU_CMD_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	$(call EcntApiBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
		
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))

