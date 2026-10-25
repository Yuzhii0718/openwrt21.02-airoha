#ifndef __TCCI_COMMON_H__
#define __TCCI_COMMON_H__

int dummy(int argc, char *argv[]);
int doSysMac(int argc, char *argv[]);

int doSysOnuType(int argc, char *argv[]);
int doSysBobCarlibration(int argc, char *argv[]);
int doSysBootFlag(int argc, char *argv[]);

int doSysSifm(int argc, char *argv[]);

#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#ifndef TCSUPPORT_NP
int doSysPONMode(int argc, char *argv[]);
#endif
#endif

int doSysQdmaInit(int argc, char *argv[]);

int doSysSerdesSel(int argc, char *argv[]);

int doSysBBF247(int argc, char *argv[]);

int doSysComboPon(int argc, char *argv[]);

#ifdef TCSUPPORT_BOARD_SELECT
int doSysBoardInfo(int argc, char *argv[]);
#endif

int doSysHybrid(int argc, char *argv[]);

#endif







