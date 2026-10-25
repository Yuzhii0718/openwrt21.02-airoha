
ifneq ($(BSP_CFLAGS), )
CFLAGS+=$(BSP_CFLAGS)
endif

ecnt_wl_e2p_chk:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ ecnt_wl_e2p_chk.c 

clean:
	-rm -f *.o *.elf *.gbd
