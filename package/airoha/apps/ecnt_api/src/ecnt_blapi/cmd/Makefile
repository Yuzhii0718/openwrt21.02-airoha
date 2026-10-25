#
#Configured directories
#
ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

RM      =	/bin/rm -rf
OPTIM	=	-fPIC -Os -g

CFLAGS += -fPIC -Os -g  -Wall $(BSP_CFLAGS) $(BSPAPI_CFLAGS) $(KERNELHEAD_CFLAGS)

LIB_OBJS = 
TARGETS  =

BLAPI_CMD_SRCS = $(wildcard *.c)
BLAPI_CMD_OBJS = $(patsubst %.c,%.o,$(BLAPI_CMD_SRCS))
BLAPI_CMD_EXECS = blapi_cmd

LINK_FLAGS += $(BSPAPI_LDFLAGS)  -lpthread -lapi_lib_utility -lblapi_system -lblapi_perform -lblapi_traffic -lblapi_tc_partition

ifneq ($(strip $(TCSUPPORT_IFC_EN)),) 
LINK_FLAGS += -lapi_lib_ifc
endif

ifneq ($(strip $(TCSUPPORT_SISM))$(strip $(TCSUPPORT_WAN_ATM))$(strip $(TCSUPPORT_WAN_PTM)),)
LINK_FLAGS += -lblapi_xdsl
endif

all:  $(BLAPI_CMD_EXECS) install


$(BLAPI_CMD_EXECS): $(BLAPI_CMD_OBJS) 
	echo "SUCCESS"
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(BLAPI_CMD_OBJS) $(CFLAGS) $(INCLUDE) $(CFLAGS_LINK_SO) $(LINK_FLAGS) -o $@
	$(STRIP) --strip-unneeded $(BLAPI_CMD_EXECS)
	
install:
	$(call BLApiBinInstall, $(ROOTPATH), $(BLAPI_CMD_EXECS), $(MAKEVAR))
	
clean:
	$(RM) *.o *.so* $(BLAPI_CMD_EXECS)
	$(call BLApiBinClean, $(BLAPI_CMD_EXECS))
	