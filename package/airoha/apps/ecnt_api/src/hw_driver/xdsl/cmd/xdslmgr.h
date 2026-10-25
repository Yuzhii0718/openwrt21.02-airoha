

#ifndef _ATMMGR_H
#define _ATMMGR_H

int do_get_event(int argc, char *argv[], void *p);
int do_get_pms(int argc, char *argv[], void *p);
int do_get_showtime(int argc, char *argv[], void *p);
int do_get_linestate(int argc, char *argv[], void *p);
int do_get_uptime(int argc, char *argv[], void *p);
int do_get_attain_rate(int argc, char *argv[], void *p);
int do_get_linedata(int argc, char *argv[], void *p);
int do_get_perfdata(int argc, char *argv[], void *p);
int do_get_chandata(int argc, char *argv[], void *p);
int do_get_tpstc(int argc, char *argv[], void *p);
int do_get_annex(int argc, char *argv[], void *p);
int do_get_mode(int argc, char *argv[], void *p);
int do_get_linkstatus(int argc, char *argv[], void *p);
int do_get_fwver(int argc, char *argv[], void *p);
int do_get(int argc, char *argv[], void *p);
int do_set_annex(int argc, char *argv[], void *p);
int do_set_mode(int argc, char *argv[], void *p);
int do_set(int argc, char *argv[], void *p);


#endif
