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



BLAPI_DEMO_SRCS = $(wildcard *.c)
BLAPI_DEMO_OBJS = $(patsubst %.c,%.o,$(BLAPI_DEMO_SRCS))
BLAPI_DEMO_EXECS = blapi_demo

LINK_FLAGS += $(BSPAPI_LDFLAGS)  -lpthread -lapi_lib_utility -lblapi_example -lblapi_os
 

all:  $(BLAPI_DEMO_EXECS) install


$(BLAPI_DEMO_EXECS): $(BLAPI_DEMO_OBJS) 
	echo "SUCCESS"
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(BLAPI_DEMO_OBJS) $(CFLAGS) $(INCLUDE) $(CFLAGS_LINK_SO) $(LINK_FLAGS) -o $@
	$(STRIP) --strip-unneeded $(BLAPI_DEMO_EXECS)
	
install:
	$(call BLApiBinInstall, $(ROOTPATH), $(BLAPI_DEMO_EXECS), $(MAKEVAR))
	
clean:
	$(RM) *.o *.so* $(BLAPI_DEMO_EXECS)
	$(call BLApiBinClean, $(BLAPI_DEMO_EXECS))
	