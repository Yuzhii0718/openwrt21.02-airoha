

#ifndef _ATMMGR_H
#define _ATMMGR_H

int do_cnt_show_mib(int argc, char *argv[], void *p);
int do_cnt_show_wan(int argc, char *argv[], void *p);
int do_cnt_show(int argc, char *argv[], void *p);
int do_cnt(int argc, char *argv[], void *p);
int do_oam_loopback_show(int argc, char *argv[], void *p);
int do_oam_loopback_reset(int argc, char *argv[], void *p);
int do_oam_loopback_start(int argc, char *argv[], void *p);
int do_oam_loopback(int argc, char *argv[], void *p);
int do_vcc_show(int argc, char *argv[], void *p);
int do_vcc_delete(int argc, char *argv[], void *p);
int do_vcc_create(int argc, char *argv[], void *p);
int do_vcc(int argc, char *argv[], void *p);
int do_stop(int argc, char *argv[], void *p);
int do_start(int argc, char *argv[], void *p);


#endif
