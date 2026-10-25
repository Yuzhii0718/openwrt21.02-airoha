ROOTPATH=$(shell pwd)
include $(BSP_ROOTDIR)/ecnt-function.mk
##################################
#####Configured directories#######
##################################
INCLUDE		= $(BSPAPP_CFLAG0)
				  
LIBDIR			= $(BSPAPI_INTLDFLAGS) -lapi_lib_utility -lecnt_lib_dbg

#ifneq ($(TCSUPPORT_CFG_NG),)
#LIBDIR += -L $(APP_CFG_NG_DIR)/install/lib -lcfg_clisvc


#ifneq ($(strip $(TCSUPPORT_CMCCV2)),)
#LIBDIR += -lapi_lib_ppe -lapi_lib_flowmgr
#endif
#endif

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
DBGMGR_CFLAGS	= $(ARHT_SECURE_OBJ_FLAGS) -O2 -Wall $(INCLUDE) $(BSP_CFLAGS)
#DBGMGR_CFLAGS	+= -D API_CMD_VDSLMGR_DBG
#DBGMGR_CFLAGS	+= -D API_CMD_QDMAMGR_DBG
#DBGMGR_CFLAGS	+= -D API_CMD_EMI_DBG
DBGMGR_CFLAGS	+= -D API_CMD_GPONMGR_DBG=0
#DBGMGR_CFLAGS += -D API_CMD_SPI_FLASH_DBG
##################################
########Configured output#########
##################################
OBJS	=   dbgcmd.o

##################################
##########interface config########
##################################
ifneq ($(strip $(TCSUPPORT_WAN_PTM)),)
INCLUDE  += -I$(MODULES_VDSL_WAN_DIR)
INCLUDE += -I$(GLOBAL_INC_DIR)/modules
OBJS	+= ptm.o
LIBDIR   += -lapi_lib_vdslmgr

ifneq ($(strip $(TCSUPPORT_BONDING)),)
OBJS	+= bonding.o
endif

endif

ifneq ($(strip $(TCSUPPORT_WAN_ATM)),)
OBJS	+= atm.o
LIBDIR   += -lapi_lib_dbg_atmmgr
endif

ifneq ($(strip $(TCSUPPORT_QDMA_BUFMGR)),)
OBJS	+= qdma.o qdma_get.o qdma_set.o
LIBDIR   += -lapi_lib_qdmamgr
endif

ifneq ($(strip $(TCSUPPORT_UART2)),)
OBJS	+= uart2.o
endif
OBJS	+= system.o

OBJS	+= spi_flash.o spi_flash_get.o spi_flash_set.o
LIBDIR	+= -lpthread

TARGETS	=	dbgmgr

all: $(TARGETS) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(DBGMGR_CFLAGS) -c $<
	
$(TARGETS): clean $(OBJS)
	$(CC) $(ARHT_SECURE_APP_FLAGS) $(DBGMGR_CFLAGS) $(PATH_LINK) -o $(TARGETS) $(OBJS) $(LIBDIR)
	$(STRIP) --strip-unneeded $(TARGETS)

install:
	#install $(TARGETS) $(USRBINDIR)
	$(call EcntApiUsrBinInstall, $(ROOTPATH), $(TARGETS), $(MAKEVAR))
	
clean:
	$(RM) $(OBJS) $(TARGETS)
	$(call EcntApiUsrBinClean, $(TARGETS))

