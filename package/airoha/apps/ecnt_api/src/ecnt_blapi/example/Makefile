#
#Configured directories
#
ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

RM      =	/bin/rm -rf
OPTIM	=	-fPIC -Os -g

CFLAGS +=  -Wall -fPIC -Os -g $(BSP_CFLAGS) $(BSPAPI_CFLAGS) $(KERNELHEAD_CFLAGS)

LIB_OBJS = 
TARGETS  =

TARGET_NAME=example

BLAPI_DSOFLAGS = $(ARHT_SECURE_SO_FLAGS) -shared $(OPTIM)
BLAPI_HEADER = blapi_$(TARGET_NAME).h
BLAPI_SRCS = $(wildcard *.c)
BLAPI_OBJS = $(patsubst %.c,%.o,$(BLAPI_SRCS))
BLAPI_SO  = libblapi_$(TARGET_NAME).so
LIB_OBJS += $(BLAPI_OBJS) 
LINK_FLAGS += $(BSPAPI_LDFLAGS) -lpthread 
TARGETS  += $(BLAPI_SO) 

all:  $(TARGETS) install

$(BLAPI_SO) : $(BLAPI_OBJS)
	$(CC) $(CFLAGS) $(BLAPI_DSOFLAGS) $(LIB_OBJS) $(LINK_FLAGS) -o $(BLAPI_SO)
	$(STRIP) --strip-unneeded $(BLAPI_SO)
	
install:
	$(call BLApiLibInstall, $(ROOTPATH), $(BLAPI_SO), $(MAKEVAR))
	$(call BLApiIncInstall, $(ROOTPATH), $(BLAPI_HEADER))
	
clean:
	$(RM) *.o *.so*
	$(call BLApiIncClean, $(BLAPI_HEADER))
	$(call BLApiLibClean, $(BLAPI_SO))
