ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

CFLAGS += -fPIC -O2   -I../install/include 

OBJS = host_service_manager.o mdio_api.o host_bp1.o host_bp2.o

HSM_EXE = hsm 
CFLAGS += $(BSP_CFLAGS)

CFLAGS += $(BSPAPP_CFLAGS)
#EXE_LIBS = -L../ -lpt -lblapi_xdsl -lblapi_traffic
#LDFLAGS = $(BSPAPP_LDFLAGS) -lm -lpt -lblapi_system $(EXE_LIBS) -lapi_lib_utility $(COMPILEOPTION_LIB)
LDFLAGS = $(BSPAPP_LDFLAGS) -lm -lpt
ifeq ($(strip $(TCSUPPORT_SISM_HOST)),)
LDFLAGS += -lblapi_system -lblapi_xdsl -lblapi_traffic -lapi_lib_utility
endif
TARGET	=   $(HSM_EXE) 

.SUFFIXES:  .c .o

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<


all: $(TARGET) install

	

$(TARGET): $(OBJS)
	@echo $(BSPAPP_LDFLAGS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)
	$(STRIP) --strip-unneeded $(TARGET)

install:
	#install $(TARGET) $(USRBINDIR)
	$(call BspAppUserFsBinInstall, $(ROOTPATH), $(TARGET))
	$(call EcntBSPReleaseUsrfsBinInstall, $(ROOTPATH), $(TARGET))


clean:
	$(RM) $(OBJS) $(TARGET)
	$(call BspAppUserFsBinClean, $(TARGET))
