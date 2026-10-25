ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

CFLAGS += -fPIC -O2   -I../install/include 


EXE_LIBS = -L../ -lpt -lblapi_xdsl -lblapi_traffic

OBJS = client_service_manager.o

CSM_EXE = csm 
CFLAGS += $(BSP_CFLAGS)

CFLAGS += $(BSPAPP_CFLAGS)
LDFLAGS = $(BSPAPP_LDFLAGS) -lm -lblapi_system $(EXE_LIBS) -lapi_lib_utility $(COMPILEOPTION_LIB) -lpthread

TARGET	=   $(CSM_EXE) 

.SUFFIXES:  .c .o

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<


all: $(TARGET) install

	

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)
	$(STRIP) --strip-unneeded $(TARGET)

install:
	#install $(TARGET) $(USRBINDIR)
	$(call BspAppUserFsBinInstall, $(ROOTPATH), $(TARGET))
	$(call EcntBSPReleaseUsrfsBinInstall, $(ROOTPATH), $(TARGET))


clean:
	$(RM) $(OBJS) $(TARGET)
	$(call BspAppUserFsBinClean, $(TARGET))