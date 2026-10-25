##################################
#####Configured directories#######
##################################
INCLUDE			= 
##################################
########Configured output#########
##################################
OBJS		  =	ecnt_lib_printf.o
HEADER		=	ecnt_lib_printf.h
LINK_NAME	=	libecnt_lib_printf.so
SONAME		=	$(LINK_NAME).1
REAL_NAME	=	$(SONAME).0.0

##################################
########Configured flags##########
##################################
RM				= /bin/rm -f
LN      		= /bin/ln -s
CFLAGS 			+=-fPIC -Wall $(INCLUDE) $(BSP_CFLAGS)
API_LIB_CFLAGS	= -fPIC $(ARHT_SECURE_SO_FLAGS) -shared -Wl,-soname,$(SONAME)

all: $(REAL_NAME) install

.SUFFIXES:  .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<
	
$(REAL_NAME): clean $(OBJS)
	$(CC) $(API_LIB_CFLAGS) -o $(REAL_NAME) $(OBJS)
	$(STRIP) --strip-unneeded $(REAL_NAME)

install:
	install $(REAL_NAME) $(APP_BSP_LIB_DIR)
	$(LN) $(REAL_NAME) $(APP_BSP_LIB_DIR)/$(LINK_NAME)
	$(LN) $(REAL_NAME) $(APP_BSP_LIB_DIR)/$(SONAME)
	cp $(HEADER) $(APP_BSP_LIB_DIR)
	cp  $(APP_BSP_LIB_DIR)/$(LINK_NAME)* $(FILESYSTEM_DIR)/lib/
	
clean:
	$(RM) $(OBJS) $(REAL_NAME)
	$(RM) $(APP_BSP_LIB_DIR)/$(LINK_NAME)
	$(RM) $(APP_BSP_LIB_DIR)/$(SONAME)
	$(RM) $(APP_BSP_LIB_DIR)/$(REAL_NAME)
	$(RM) $(APP_BSP_LIB_DIR)/$(HEADER)
	$(RM) $(FILESYSTEM_DIR)/lib/$(LINK_NAME)*