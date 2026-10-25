#ifndef __MTK7629_H__
#define __MTK7629_H__
#include <syslog.h>
#define WIFILEDCTL_DEBUG    LOG_DEBUG
#define WIFILEDCTL_WARNING  LOG_WARNING
#define WIFILEDCTL_ERROR    LOG_ERR

#define UINT64 unsigned long long

#define MAX_STA_NUM 128
#define GET_INTO_TIMES 5
#define GPIO_ROUTER_LINK_LED_RED 12   /*kLedNTGRBackhaulRed index   21*/
#define GPIO_ROUTER_LINK_LED_GREEN 14 /*kLedNTGRBackhaulGreen index 22*/

#define GPIO_CLIENT_LINK_LED_RED 5   /*kLedNTGRFornthaulRed index  19*/
#define GPIO_CLIENT_LINK_LED_GREEN 7 /*kLedNTGRFornthaulGreen index 20*/

#define PREFIX_IFNAME_2G "ra0" /*2G interface name prefix*/
#define PREFIX_IFNAME_5G "rai0" /*5G interface name prefix*/

#define APCLI_2G_IFNAME "apcli0" /*apcli  network 2G*/
#define AP1_2G_IFNAME "ra0" /*rootap network 2G*/
#define AP2_2G_IFNAME "ra1" /*guest  network 2G*/

#define APCLI_5G_IFNAME "apclii0" /*apcli  network 5G*/
#define AP1_5G_IFNAME "rai0" /*rootap network 5G*/
#define AP2_5G_IFNAME "rai1" /*guest  network 5G*/

#define MODE_802_11A 0x01
#define MODE_802_11B 0x02
#define MODE_802_11G 0x03
#define MODE_802_11N 0x04
#define MODE_802_11AC 0x05
#define MODE_802_11AX 0x06
#define MODE_802_11BE 0x07

//linkMode
#define MODE_11BE_HT160_3X3 ((MODE_802_11BE << 16) + (3 << 8) + 160)
#define MODE_11BE_HT80_3X3 ((MODE_802_11BE << 16) + (3 << 8) + 80)
#define MODE_11BE_HT40_3X3 ((MODE_802_11BE << 16) + (3 << 8) + 40)
#define MODE_11BE_HT20_3X3 ((MODE_802_11BE << 16) + (3 << 8) + 20)

#define MODE_11BE_HT160_2X2 ((MODE_802_11BE << 16) + (2 << 8) + 160)
#define MODE_11BE_HT80_2X2 ((MODE_802_11BE << 16) + (2 << 8) + 80)
#define MODE_11BE_HT40_2X2 ((MODE_802_11BE << 16) + (2 << 8) + 40)
#define MODE_11BE_HT20_2X2 ((MODE_802_11BE << 16) + (2 << 8) + 20)

#define MODE_11BE_HT160_1X1 ((MODE_802_11BE << 16) + (1 << 8) + 160)
#define MODE_11BE_HT80_1X1 ((MODE_802_11BE << 16) + (1 << 8) + 80)
#define MODE_11BE_HT40_1X1 ((MODE_802_11BE << 16) + (1 << 8) + 40)
#define MODE_11BE_HT20_1X1 ((MODE_802_11BE << 16) + (1 << 8) + 20)

#define MODE_11AX_HT160_3X3 ((MODE_802_11AX << 16) + (3 << 8) + 160)
#define MODE_11AX_HT80_3X3 ((MODE_802_11AX << 16) + (3 << 8) + 80)
#define MODE_11AX_HT40_3X3 ((MODE_802_11AX << 16) + (3 << 8) + 40)
#define MODE_11AX_HT20_3X3 ((MODE_802_11AX << 16) + (3 << 8) + 20)

#define MODE_11AX_HT160_2X2 ((MODE_802_11AX << 16) + (2 << 8) + 160)
#define MODE_11AX_HT80_2X2 ((MODE_802_11AX << 16) + (2 << 8) + 80)
#define MODE_11AX_HT40_2X2 ((MODE_802_11AX << 16) + (2 << 8) + 40)
#define MODE_11AX_HT20_2X2 ((MODE_802_11AX << 16) + (2 << 8) + 20)

#define MODE_11AX_HT160_1X1 ((MODE_802_11AX << 16) + (1 << 8) + 160)
#define MODE_11AX_HT80_1X1 ((MODE_802_11AX << 16) + (1 << 8) + 80)
#define MODE_11AX_HT40_1X1 ((MODE_802_11AX << 16) + (1 << 8) + 40)
#define MODE_11AX_HT20_1X1 ((MODE_802_11AX << 16) + (1 << 8) + 20)

#define MODE_11AC_HT160_3X3 ((MODE_802_11AC << 16) + (3 << 8) + 160)
#define MODE_11AC_HT80_3X3 ((MODE_802_11AC << 16) + (3 << 8) + 80)
#define MODE_11AC_HT40_3X3 ((MODE_802_11AC << 16) + (3 << 8) + 40)
#define MODE_11AC_HT20_3X3 ((MODE_802_11AC << 16) + (3 << 8) + 20)

#define MODE_11AC_HT160_2X2 ((MODE_802_11AC << 16) + (2 << 8) + 160)
#define MODE_11AC_HT80_2X2 ((MODE_802_11AC << 16) + (2 << 8) + 80)
#define MODE_11AC_HT40_2X2 ((MODE_802_11AC << 16) + (2 << 8) + 40)
#define MODE_11AC_HT20_2X2 ((MODE_802_11AC << 16) + (2 << 8) + 20)

#define MODE_11AC_HT160_1X1 ((MODE_802_11AC << 16) + (1 << 8) + 160)
#define MODE_11AC_HT80_1X1 ((MODE_802_11AC << 16) + (1 << 8) + 80)
#define MODE_11AC_HT40_1X1 ((MODE_802_11AC << 16) + (1 << 8) + 40)
#define MODE_11AC_HT20_1X1 ((MODE_802_11AC << 16) + (1 << 8) + 20)

#define MODE_11N_HT40_3X3 ((MODE_802_11N << 16) + (3 << 8) + 40)
#define MODE_11N_HT20_3X3 ((MODE_802_11N << 16) + (3 << 8) + 20)

#define MODE_11N_HT40_2X2 ((MODE_802_11N << 16) + (2 << 8) + 40)
#define MODE_11N_HT20_2X2 ((MODE_802_11N << 16) + (2 << 8) + 20)

#define MODE_11N_HT40_1X1 ((MODE_802_11N << 16) + (1 << 8) + 40)
#define MODE_11N_HT20_1X1 ((MODE_802_11N << 16) + (1 << 8) + 20)

#define MODE_11G_HT40_2X2 ((MODE_802_11G << 16) + (2 << 8) + 40)
#define MODE_11G_HT20_2X2 ((MODE_802_11G << 16) + (2 << 8) + 20)

#define MODE_11G_HT40_1X1 ((MODE_802_11G << 16) + (1 << 8) + 40)

#define MODE_11B_HT40_2X2 ((MODE_802_11B << 16) + (2 << 8) + 40)
#define MODE_11B_HT20_2X2 ((MODE_802_11B << 16) + (2 << 8) + 20)

#define MODE_11B_HT40_1X1 ((MODE_802_11B << 16) + (1 << 8) + 40)

#define MODE_11G ((MODE_802_11G << 16) + (1 << 8) + 20)
#define MODE_11B ((MODE_802_11B << 16) + (1 << 8) + 20)
#define MODE_11A ((MODE_802_11A << 16) + (1 << 8) + 20)

struct apclilinkinfo
{
    int txRate;
    int rxRate;
    UINT64 txPkts;
    UINT64 rxPkts;
    int linkMode;
    int reservd; /*for 8byte Bytes Alignment*/
};

//sizeof(g_apclilinkinfo2G)=160
//struct apclilinkinfo g_apclilinkinfo2G[GET_INTO_TIMES];
//struct apclilinkinfo g_apclilinkinfo5G[GET_INTO_TIMES];

struct stalinkinfo
{
    char stamac[6];
    int txRate[GET_INTO_TIMES];
    int rxRate[GET_INTO_TIMES];
    UINT64 txPkts[GET_INTO_TIMES];
    UINT64 rxPkts[GET_INTO_TIMES];
    UINT64 ttPkts;
    int linkMode;
    int reservd; /*for 8byte Bytes Alignment*/
};

#if 1
#define WIFILEDCTL_LOG(level, fmt, ...) \
    do { \
        if(0 == access("/tmp/wifileddbg", F_OK)) {\
            syslog(level, "[%s:%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); }\
    } while (0) 

#else
#define WIFILEDCTL_LOG(level, fmt, ...)

#endif

#define LOG_PRINT(level, fmt, ...) \
    do { \
        if(0 == access("/tmp/wifileddbg", F_OK)) {\
            syslog(level, "[%s:%d]: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); }\
    } while (0) 

#define WIFI_LED_NAME_RLINK  "rlink"
#define WIFI_LED_NAME_CLINK  "clink"

#define WIFI_LED_COLOR_AMBER "amber"
#define WIFI_LED_COLOR_RED "red"
#define WIFI_LED_COLOR_GREEN "green"

int get_ap_txrx_info(char *interface, int times);
int get_apcli_txrx_info(char *interface, int times);
int wifiLedOn(const char *ledName, const char *color);
int wifiLedOff(const char *ledName, const char *color);
#endif
