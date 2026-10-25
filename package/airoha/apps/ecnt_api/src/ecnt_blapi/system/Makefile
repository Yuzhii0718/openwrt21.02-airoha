#
#Configured directories
#
ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

TARGET_NAME=system
BLAPI_HEADER = blapi_$(TARGET_NAME).h
BLAPI_HEADER += blapi_$(TARGET_NAME)_bsp.h
BLAPI_HEADER += blapi_$(TARGET_NAME)_cmd.h
BLAPI_SO  = libblapi_$(TARGET_NAME).so

OPTIM =-fPIC -Os -g
CFLAGS += $(OPTIM) -Wall $(BSP_CFLAGS) $(BSPAPI_CFLAGS) $(KERNELHEAD_CFLAGS)
BLAPI_DSOFLAGS = $(ARHT_SECURE_SO_FLAGS) -shared

TARGETS = $(BLAPI_SO)

# disable BSP target, all the source files are compiled. 
# BLAPI_BSP_SRCS = $(wildcard *_bsp.c) $(wildcard *_cmd.c)
BLAPI_BSP_SRCS = $(wildcard *.c)
BLAPI_APP_SRCS = $(wildcard *.c)

ifneq ($(strip $(AIROHA_BSPSDK)),)
BLAPI_SRCS = $(BLAPI_BSP_SRCS)
else
BLAPI_SRCS = $(BLAPI_APP_SRCS)
endif

BLAPI_OBJS = $(patsubst %.c,%.o,$(BLAPI_SRCS))

LINK_FLAGS += $(BSPAPI_LDFLAGS) -lpthread -L$(BSP_INT_LIB) -lapi_lib_switchmgr -lapi_lib_fe -lecnt_lib_dbg -lecnt_env -lapi_lib_utility

ifneq ($(strip $(TCSUPPORT_SISM))$(strip $(TCSUPPORT_WAN_ATM))$(strip $(TCSUPPORT_PTM)),)
LINK_FLAGS += -lblapi_xdsl
endif

ifneq ($(strip $(TCSUPPORT_SISM)),)
LINK_FLAGS += -lpt
endif

ifneq ($(strip $(TCSUPPORT_WAN_GPON) $(TCSUPPORT_WAN_EPON)),)
ifeq ($(strip $(TCSUPPORT_NP)),)
LINK_FLAGS += -lxpon
endif
endif

ifneq ($(strip $(TCSUPPORT_MIPS_1004K)),)
LINK_FLAGS += -lapi_lib_cpu_interrupt
endif

all:  $(TARGETS) install

$(TARGETS) : $(BLAPI_OBJS)
	$(CC) $(CFLAGS) $(BLAPI_DSOFLAGS) $(BLAPI_OBJS) $(LINK_FLAGS) -o $(BLAPI_SO)
	$(STRIP) --strip-unneeded $(BLAPI_SO)
	
install:
	$(call BLApiLibInstall, $(ROOTPATH), $(BLAPI_SO), $(MAKEVAR))
	$(call BLApiIncInstall, $(ROOTPATH), $(BLAPI_HEADER), $(MAKEVAR))

clean:
	$(RM) *.o *.so*
	$(call BLApiIncClean, $(BLAPI_HEADER))
	$(call BLApiLibClean, $(BLAPI_SO))
