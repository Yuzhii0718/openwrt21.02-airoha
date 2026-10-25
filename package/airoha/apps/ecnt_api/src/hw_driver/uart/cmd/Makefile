ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= $(BSPAPI_INTCFLAGS)
				  
COMPILE_LIBS		= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
UART_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) 


##################################
########Configured output#########
##################################
OBJS	=	uart.o
TARGETS	=	uart

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(UART_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(UART_CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)
	$(call EcntApiUsrBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiBinClean, $(TARGETS))
