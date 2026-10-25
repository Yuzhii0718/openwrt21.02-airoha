ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk

##################################
#####Configured directories#######
##################################
INCLUDE			= -I$(GLOBAL_INC_DIR)
				   
COMPILE_LIBS   += $(BSPAPI_INTLDFLAGS) -lapi_lib_prolinecmd  
##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
CFLAGS	= -fPIC -O2 -Wall $(INCLUDE) $(BSP_CFLAGS) $(BSPAPI_CFLAGS)
##################################
########Configured output#########
##################################
OBJS	=	prolinecmd.o
TARGETS	=	prolinecmd

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(CFLAGS) -o $(TARGETS) $(OBJS) $(COMPILE_LIBS) 
	$(STRIP) --strip-unneeded $(TARGETS)
	
install:
	$(call BspAppUsrBinInstall, $(ROOTPATH), $(TARGETS))

clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call BspAppUsrBinClean, $(TARGETS))

