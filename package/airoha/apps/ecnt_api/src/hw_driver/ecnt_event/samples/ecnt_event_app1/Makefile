#CROSS_COMPILE=mips-linux-
#CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
STRIP=$(CROSS_COMPILE)strip
RANLIB=$(CROSS_COMPILE)ranlib
SIZE=$(CROSS_COMPILE)size


#CFLAGS = -mips1 -msoft-float -O2 -fomit-frame-pointer
CFLAGS = -fomit-frame-pointer

ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
else
CFLAGS += -O2
endif

LIBS += $(BSPAPP_CFLAG0) -lecntevent

EXEC = ecnt_event_app1
OBJS = ecnt_event_app1.o

all: ecnt_event_lib $(EXEC)

ecnt_event_lib:

.c.o:
	$(CC) -c $(CFLAGS) $(LIBS) $(INCLUDE) $<

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@
	$(STRIP) -s $@ 

clean:
	-rm -f $(EXEC) *.elf *.o

