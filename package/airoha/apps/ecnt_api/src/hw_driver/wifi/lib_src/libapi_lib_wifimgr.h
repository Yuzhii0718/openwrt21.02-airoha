/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

#ifndef LIBAPI_LIB_WIFIMGR_HEAD
#define LIBAPI_LIB_WIFIMGR_HEAD
/**
* \file  libapi_lib_wifimgr.h  
* \brief This file is wifimgr api header file of business logic APIs that will be exported for others to use.
* \author Yifan.Chen
* \date     2020-9-28
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

#include <stdio.h>
#include "wifi_comm.h"

#define WPS_FILE				"/tmp/wpsInfo"

#define SVC_WLAN_SCRIPT_PATH		"/etc/Wireless/RT2860AP/WLAN_exec.sh"
#define SVC_WLAN11AC_SCRIPT_PATH	"/etc/Wireless/RT2860AP_AC/WLAN_exec.sh"

#define MAX_DESCRIPTION_LEN  64

#if defined(TCSUPPORT_WLAN_WPA3)
#define AUTH_MODE_NUM 	    11
#else
#define AUTH_MODE_NUM 	    9
#endif
#define ENCRYP_TYPE_NUM     6
#define MAC_ADDR_LENGTH		6
#define MAX_LEN_OF_MAC_TABLE 	32
#define MAX_NUMBER_OF_ACL		64

#if !defined(TCSUPPORT_MULTI_USER_ITF)
#if defined(TCSUPPORT_WLAN_AC)
#define MAX_DUALBAND_BSSID_NUM 8
#define MAX_ECNT_WALN_PORT_NUM              (4)
#define MAX_ECNT_WALNAC_PORT_NUM            (4)
#else
#define MAX_DUALBAND_BSSID_NUM 4
#define     MAX_ECNT_WALN_PORT_NUM              (4)
#endif
#endif

#define ENTRY_CAT_AP			0x002

#ifdef LINUX
#if WIRELESS_EXT <= 11
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE                              0x8BE0
#endif
#define SIOCIWFIRSTPRIV								SIOCDEVPRIVATE
#endif
#endif 
#define RT_PRIV_IOCTL								(SIOCIWFIRSTPRIV + 0x01)
#define RTPRIV_IOCTL_SET							(SIOCIWFIRSTPRIV + 0x02)
#define RTPRIV_IOCTL_STATISTICS                     (SIOCIWFIRSTPRIV + 0x09)
#define RTPRIV_IOCTL_ADD_PMKID_CACHE                (SIOCIWFIRSTPRIV + 0x0A)
#define RTPRIV_IOCTL_RADIUS_DATA                    (SIOCIWFIRSTPRIV + 0x0C)
#define RTPRIV_IOCTL_GSITESURVEY					(SIOCIWFIRSTPRIV + 0x0D)
#define RTPRIV_IOCTL_ADD_WPA_KEY                    (SIOCIWFIRSTPRIV + 0x0E)
#define RTPRIV_IOCTL_GET_MAC_TABLE					(SIOCIWFIRSTPRIV + 0x0F)
#define RTPRIV_IOCTL_STATIC_WEP_COPY                (SIOCIWFIRSTPRIV + 0x10)
#define RTPRIV_IOCTL_QUERY_BATABLE                  (SIOCIWFIRSTPRIV + 0x16)
#define RTPRIV_IOCTL_GET_AR9_SHOW   				(SIOCIWFIRSTPRIV + 0x17)
#define RTPRIV_IOCTL_SET_WSCOOB						(SIOCIWFIRSTPRIV + 0x19)
#define RTPRIV_IOCTL_GET_PROCESS_INFO		(SIOCIWFIRSTPRIV + 0x1E)

#define RTPRIV_IOCTL_AP_COUNT						(SIOCIWFIRSTPRIV + 0x1C)

#if defined(TCSUPPORT_WLAN_APCLIENT)
#define RTPRIV_IOCTL_GET_APCLI_STATUS				(SIOCIWFIRSTPRIV + 0x14)
#endif

#define OID_SET_DIAG_TZ                                      0x2015

#ifdef  DOT11_VHT_AC 
#define MAX_NUM_OF_CHS             		(54 + 5)	/* 5 channels for central channel of VHT 80MHz */
#else
#define MAX_NUM_OF_CHS             		59/* modify original 54*/
#endif /* DOT11_VHT_AC*/
/* 14 channels @2.4G +  12@UNII + 4 @MMAC + 11 @HiperLAN2 + 7 @Japan + 1 as NULL termination */
#define MAX_NUM_OF_CHANNELS             MAX_NUM_OF_CHS
#define OID_802_11_GET_ACS_CHANNEL_SCORE                0x2014
#define OID_802_11_GET_WPS_STA_INFO											0x2044

#ifdef TCSUPPORT_CT_UBUS
#define OID_GET_PARTIAL_SCAN_STATUS		                0x1805
#endif

#define RT_OID_802_11_GET_NEIGHBOUR_INFO			0x0683

typedef enum _reload_flag{
	NO_NEED_RELOAD = 0,
	NEED_RELOAD,
	NEED_REBOOT
}reload_flag;

typedef struct _wps_info{
	int wpsMode;
	unsigned int wpsPinCode;
}wps_info;

#ifdef TCSUPPORT_CMCCV2
#define OID_802_11_GET_NF					0x069c
#define OID_802_11_GET_CHANNEL_BUSY_TIME	0x069d
#define RT_OID_GET_CHANLOAD                 0x0765
#define OID_802_11_GET_RADIO_STATISTICS     0x0766
#endif

#ifdef TCSUPPORT_CMCCV2
typedef struct _RADIO_STATISTICS {
	char CurrentChannelWidth;
	unsigned long RadioRecvPackets;
	unsigned long RadioSendPackets;
	unsigned long BcnFail;
	unsigned long RadioRecvErrorPackets;
} RADIO_STATISTICS;
#endif


typedef struct _NDIS_NEIGHBOUR_INFO{
	unsigned int No; 
	unsigned char Channel;
	unsigned char SSID[32 + 1];
	unsigned char Bssid[MAC_ADDR_LEN];
	unsigned char Security[32];
	unsigned char Rssi;
	unsigned char WiressMode[16];
	unsigned char ExtCh[16];
	unsigned char NetworkType[8];
	unsigned char SSID_Len;
	unsigned char WPS[8];
	unsigned char DPID[8];
	unsigned char FromBcnReport[8];
}NDIS_NEIGHBOUR_INFO, *PNDIS_NEIGHBOUR_INFO;

typedef struct _NDIS_802_11_GET_NEIGHBOUR_INFO {
	unsigned int num;
	NDIS_NEIGHBOUR_INFO entry[128];
} NDIS_802_11_GET_NEIGHBOUR_INFO, *PNDIS_802_11_GET_NEIGHBOUR_INFO;


#if defined(TCSUPPORT_CMCCV2)
#define RT_OID_802_11_PROBE_REQ_STATISTICS			0x0682
#define MAX_LEN_OF_SSID                 32
#define VENDOR_SPECIFIC_DATA_SIZE 15
#define MAX_PROBEREQ_ENTRY_NUM 30
#define PROBE_REQ_LINE_LEN (20+20+33+20+8+5+15)  //{"RecvTime","MacAddr","SSIDName","BSSID","Channel","RSSI","VendorData"}

/*Econet add for ioctl used*/
typedef struct _PROBE_REQ_IOCTL_ENTRY{
	long long RecvTime; //unit:1ms
	unsigned char MacAddr[MAC_ADDR_LENGTH];
	unsigned char SSIDName[MAX_LEN_OF_SSID];
	unsigned char BSSID[MAC_ADDR_LENGTH];
	unsigned char Channel;
	char RSSI;
	unsigned char VendorData[VENDOR_SPECIFIC_DATA_SIZE+1];
} PROBE_REQ_IOCTL_ENTRY, *PPROBE_REQ_IOCTL_ENTRY;

typedef struct _PROBE_REQ_IOCTL_ROOT{
	unsigned int entry_num; 
	PROBE_REQ_IOCTL_ENTRY entry[MAX_PROBEREQ_ENTRY_NUM];
} PROBE_REQ_IOCTL_ROOT, *PPROBE_REQ_IOCTL_ROOT;
#endif

typedef union _HTTRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
	struct {
		unsigned short  MODE:3;	/* Use definition MODE_xxx. */
		unsigned short  iTxBF:1;
		unsigned short  eTxBF:1;
		unsigned short  STBC:1;	/* only support in HT/VHT mode with MCS0~7 */
		unsigned short  ShortGI:1;
		unsigned short  BW:2;	/* channel bandwidth 20MHz/40/80 MHz */
		unsigned short  ldpc:1;
		unsigned short  MCS:6;	/* MCS */
	} field;
#else
	struct {
		unsigned short  MCS:6;
		unsigned short  ldpc:1;
		unsigned short  BW:2;
		unsigned short  ShortGI:1;
		unsigned short  STBC:1;
		unsigned short  eTxBF:1;
		unsigned short  iTxBF:1;
		unsigned short  MODE:3;
	} field;
#endif
	unsigned short  word;
} HTTRANSMIT_SETTING, *PHTTRANSMIT_SETTING;

typedef enum _wps_action{
	WPS_RESETOOB = 0,
	WPS_GENPINCODE,
	WPS_STOP,
	WPS_START
}wps_action;

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
typedef struct _chip_info{
	struct
	{
		unsigned int chipcap;
		unsigned int resverd[1];
	}chipcapinfo;
	struct
	{
		unsigned int funccap;
		unsigned int resverd[1];
	}funccapinfo;
}chip_info;

typedef struct 
{
	int chipindex;
	void (*chipcap_api)(wifi_type,chip_info*);
}chip_cap_fun;
#endif

typedef struct _AUTO_CH_SEL_SCORE{
	unsigned int Score;
	unsigned int Channel;
} AUTO_CH_SEL_SCORE, *PAUTO_CH_SEL_SCORE;;

typedef struct _ACS_CHANNEL_SCORE{
 AUTO_CH_SEL_SCORE AcsChannelScore[MAX_NUM_OF_CHANNELS+1];
 unsigned int AcsAlg;
} ACS_CHANNEL_SCORE , *PACS_CHANNEL_SCORE;

typedef struct _RT_AP_COUNT
{
	unsigned long txPackets;
	unsigned long rxPackets;
	unsigned long long txBytes;
	unsigned long long rxBytes;
	unsigned long txErrPkts;
	unsigned long txDropPkts;
	unsigned long rxErrPkts;
	unsigned long rxDropPkts;
	unsigned char wpsTimerStatus_24[8];
	unsigned char wpsTimerStatus_58[8];
}RT_AP_COUNT;

typedef struct mac_entry_s {
	unsigned char Addr[MAC_ADDR_LENGTH];
#if defined(TCSUPPORT_CT_PHONEAPP) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
	signed char 	AvgRssi0;
	signed char 	AvgRssi1;
	signed char 	AvgRssi2;

#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
	unsigned int	DataRate;
	unsigned char PortSecured;
	signed char 	AvgRssi;

#endif
#endif
#if defined(TCSUPPORT_CT_UBUS)
unsigned int EntryType;
#endif
}mac_entry_t;

typedef struct mac_table_s {
	unsigned long Num;
	mac_entry_t macEntry[MAX_LEN_OF_MAC_TABLE];
}mac_table_t;

struct PRE_WLAN_STATE{
    int wlan_Bssid_num;/*WLan 2.4G bssid_num */
	int wlan11ac_Bssid_num; /*WLan 5G bssid num */ 
};
extern struct PRE_WLAN_STATE pre_wlan_state;

typedef struct _NEIFHBOR_INFO_ENTRY{	 
	char band[5];
    char ssid[33];
    char bssid[18];
    char channel[4];
    char rssi[6];
    char wMode[16];
	char bandwidth[12];

}NEIFHBOR_INFO_ENTRY, *PNEIFHBOR_INFO_ENTRY;

typedef struct _NEIFHBOR_INFO{	 
	int num;
	NEIFHBOR_INFO_ENTRY info_entry[256];
}NEIFHBOR_INFO, *PNEIFHBOR_INFO;
typedef struct GNU_PACKED _WSC_CONFIGURED_VALUE {
	unsigned short WscConfigured; /* 1 un-configured; 2 configured*/
	unsigned char	WscSsid[32 + 1];
	unsigned short WscAuthMode;	/* mandatory, 0x01: open, 0x02: wpa-psk, 0x04: shared, 0x08:wpa, 0x10: wpa2, 0x20: wpa2-psk*/
	unsigned short WscEncrypType;	/* 0x01: none, 0x02: wep, 0x04: tkip, 0x08: aes*/
	unsigned char	DefaultKeyIdx;
	unsigned char	WscWPAKey[64 + 1];
} WSC_CONFIGURED_VALUE;

#ifdef WSC_AP_SUPPORT
typedef struct _RT_802_11_ACL_ENTRY {
    unsigned char   Addr[MAC_ADDR_LENGTH];
    unsigned short  Rsv;
#ifdef TCSUPPORT_WLAN_ACL_BLK_COUNT     
	unsigned long Reject_Count;    
#endif 
} RT_802_11_ACL_ENTRY, *PRT_802_11_ACL_ENTRY;

typedef struct GNU_PACKED _RT_802_11_ACL {
    unsigned long   Policy;      /* 0-disable, 1-positive list, 2-negative list*/
    unsigned long   Num;
    RT_802_11_ACL_ENTRY Entry[MAX_NUMBER_OF_ACL];
} RT_802_11_ACL, *PRT_802_11_ACL;
#endif

struct diag_wifi_process_entry{	
	unsigned char name[16];	
	int pid;	
	unsigned char is_process;
};

struct diag_wifi_process_info{
#if 0
	unsigned char result;	
	unsigned char errDesc[32];
#endif
	unsigned char num;	
	struct diag_wifi_process_entry entry[8];
};

/**
 * \brief This function set the level of debug.
 *
 * This function set the level of debug. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the level of debug.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the level of debug
 */

int wifimgr_lib_set_dbg_level(char *str);

/**
 * \brief This function set the path of the dat file.
 *
 * This function set the path of the dat file. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the path of the dat file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the path of the dat file
 */

int wifimgr_lib_set_wifi_path(wifi_type type, char *str); 

/**
 * \brief This function write calibration infomation to flash.
 *
 * This function write calibration infomation to flash. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Write calibration infomation to flash.
 * \par Example
 * \snippet Api_lib_wifimgr.c Write calibration infomation to flash.
 */

int wifimgr_lib_set_wifi_calibration(wifi_type type);

/**
 * \brief This function read calibration infomation from flash.
 *
 * This function read calibration infomation from flash. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Read calibration infomation from flash.
 * \par Example
 * \snippet Api_lib_wifimgr.c Read calibration infomation from flash.
 */

int wifimgr_lib_get_wifi_calibration(wifi_type type);

/**
 * \brief This function ready to set or get parameters.
 *
 * This function ready to set or get parameters. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Ready to set or get parameters.
 * \par Example
 * \snippet Api_lib_wifimgr.c Ready to set or get parameters.
 */

int wifimgr_lib_init(wifi_type type);

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
/**
 * \brief This function ready to do pre init(eeprom dat,etc for multi chip ).
 *
 * This function ready to do pre init. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Ready to do pre init.
 * \par Example
 * \snippet Api_lib_wifimgr.c Ready to do pre init.
 */

int wifimgr_lib_preinit();

/**
 * \brief This function ready to do multichip init.
 *
 * This function ready to do multichip init. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Ready to do multichip init.
 * \par Example
 * \snippet Api_lib_wifimgr.c Ready to do multichip init.
 */
void wifimgr_lib_multichip_init(void);
#endif
/**
 * \brief This function print shared memory data.
 *
 * This function print shared memory data. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Print shared memory data.
 * \par Example
 * \snippet Api_lib_wifimgr.c Print shared memory data.
 */

int wifimgr_lib_dump(wifi_type type);

/**
 * \brief This function get the channel of the wifi.
 *
 * This function get the channel of the wifi. submit by 
 * \param[out]	channel of the wifi.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the channel of the wifi.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the channel of the wifi.
 */

int wifimgr_lib_get_channel(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the auto channel of the wifi.
 *
 * This function get the auto channel of the wifi. submit by 
 * \param[out]	channel of the wifi.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the auto channel of the wifi.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the auto channel of the wifi.
 */

int wifimgr_lib_get_autoChannelSelect(wifi_type type, int index, char *output, unsigned int size);
#ifdef TCSUPPORT_WLAN_ACS

/**
 * \brief This function get the score of the channel scan.
 *
 * This function get the score of the channel scan. submit by 
 * \param[out]	the score of the channel scan.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the score of the channel scan.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the score of the channel scan.
 */

int wifimgr_lib_get_channelScanScore(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the best channel.
 *
 * This function get the best channel. submit by 
 * \param[out]	the number of the best channel.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the best channel.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the best channel.
 */

int wifimgr_lib_get_bestChannel(wifi_type type, int index, char* output, unsigned int size);
#endif

/**
 * \brief This function get the number of the bssid.
 *
 * This function get the number of the bssid. submit by 
 * \param[out]	the number of bssid.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the number of the bssid.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the number of the bssid.
 */

int wifimgr_lib_get_bssidnum(wifi_type type,int index, char* saved,unsigned int size);

/**
 * \brief This function get 2.4g channel range of the country region.
 *
 * This function get 2.4g channel range of the country region. submit by 
 * \param[out]	channel range of the country region
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get 2.4g channel range of the country region.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get 2.4g channel range of the country region.
 */

int wifimgr_lib_get_countryRegion(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get 5g channel range of the country region.
 *
 * This function get 5g channel range of the country region. submit by 
 * \param[out]	channel range of the country region
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get 5g channel range of the country region.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get 5g channel range of the country region.
 */

int wifimgr_lib_get_countryRegionABand(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get code of the country.
 *
 * This function get code of the country. submit by 
 * \param[out]	code of the country
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get code of the country.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get code of the country.
 */

int wifimgr_lib_get_countryCode(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the beacon period.
 *
 * This function get the beacon period. submit by 
 * \param[out]	the the beacon period
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the beacon period.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the beacon period.
 */

int wifimgr_lib_get_beaconPeriod(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the threshold of rts.
 *
 * This function get the threshold of rts. submit by 
 * \param[out]	 the threshold of rts
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the threshold of rts.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the threshold of rts.
 */

int wifimgr_lib_get_rtsThreshold(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the threshold of frag.
 *
 * This function get the threshold of frag. submit by 
 * \param[out]	 the threshold of frag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the threshold of frag.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the threshold of frag.
 */

int wifimgr_lib_get_fragThreshold(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the dtim period.
 *
 * This function get the dtim period. submit by 
 * \param[out]	the dtim period
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the dtim period.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the dtim period.
 */

int wifimgr_lib_get_dtimPeriod(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the strength of the channel.
 *
 * This function get the strength of the channel. submit by 
 * \param[out]	 the strength of the channel
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the dtim period.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the strength of the channel.
 */

int wifimgr_lib_get_txPower(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the bgProtection from RT2860.
 *
 * This function get the bgProtection from RT2860. submit by 
 * \param[out]	 the bgProtection
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the bgProtection from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the bgProtection from RT2860.
 */

int wifimgr_lib_get_bgProtection(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the disableOLBC from RT2860.
 *
 * This function get the disableOLBC from RT2860. submit by 
 * \param[out]	 the disableOLBC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the disableOLBC from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the disableOLBC from RT2860.
 */
 
int wifimgr_lib_get_disableOLBC(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the txPreamble from RT2860.
 *
 * This function get the txPreamble from RT2860. submit by 
 * \param[out]	 the txPreamble
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the txPreamble from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the txPreamble from RT2860.
 */

int wifimgr_lib_get_txPreamble(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the txBurst from RT2860.
 *
 * This function get the txBurst from RT2860. submit by 
 * \param[out]	 the txBurst
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the txBurst from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the txBurst from RT2860.
 */

int wifimgr_lib_get_txBurst(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the pktAggregate from RT2860.
 *
 * This function get the pktAggregate from RT2860. submit by 
 * \param[out]	 the pktAggregate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the pktAggregate from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the pktAggregate from RT2860.
 */

int wifimgr_lib_get_pktAggregate(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the turboRate from RT2860.
 *
 * This function get the turboRate from RT2860. submit by 
 * \param[out]	 the turboRate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the turboRate from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the turboRate from RT2860.
 */

int wifimgr_lib_get_turboRate(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the noForwardingBTNBSSID from RT2860.
 *
 * This function get the noForwardingBTNBSSID from RT2860. submit by 
 * \param[out]	 the noForwardingBTNBSSID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the noForwardingBTNBSSID from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the noForwardingBTNBSSID from RT2860.
 */

int wifimgr_lib_get_noForwardingBTNBSSID(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the shortSlot from RT2860.
 *
 * This function get the shortSlot from RT2860. submit by 
 * \param[out]	 the shortSlot
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the shortSlot from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the shortSlot from RT2860.
 */

int wifimgr_lib_get_shortSlot(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the wirelessEvent from RT2860.
 *
 * This function get the wirelessEvent from RT2860. submit by 
 * \param[out]	 the wirelessEvent
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wirelessEvent from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wirelessEvent from RT2860.
 */

int wifimgr_lib_get_wirelessEvent(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the rekeyInterval from RT2860.
 *
 * This function get the rekeyInterval from RT2860. submit by 
 * \param[out]	 the rekeyInterval
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the rekeyInterval from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the rekeyInterval from RT2860.
 */

int wifimgr_lib_get_rekeyInterval(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the rekeyMethod from RT2860.
 *
 * This function get the rekeyMethod from RT2860. submit by 
 * \param[out]	 the rekeyMethod
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the rekeyMethod from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the rekeyMethod from RT2860.
 */

int wifimgr_lib_get_rekeyMethod(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the pmkCachePeriod from RT2860.
 *
 * This function get the pmkCachePeriod from RT2860. submit by 
 * \param[out]	 the pmkCachePeriod
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the pmkCachePeriod from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the pmkCachePeriod from RT2860.
 */

int wifimgr_lib_get_pmkCachePeriod(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the wirelessMode from RT2860.
 *
 * This function get the wirelessMode from RT2860. submit by 
 * \param[out]	 the wirelessMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wirelessMode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wirelessMode from RT2860.
 */

int wifimgr_lib_get_wirelessMode(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the basicRate from RT2860.
 *
 * This function get the basicRate from RT2860. submit by 
 * \param[out]	 the basicRate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the basicRate from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the basicRate from RT2860.
 */

int wifimgr_lib_get_basicRate(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_BW from RT2860.
 *
 * This function get the HT_BW from RT2860. submit by 
 * \param[out]	 the HT_BW
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_BW from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_BW from RT2860.
 */

int wifimgr_lib_get_HT_BW(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the VHT_BW from RT2860.
 *
 * This function get the VHT_BW from RT2860. submit by 
 * \param[out]	 the VHT_BW
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the VHT_BW from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the VHT_BW from RT2860.
 */

int wifimgr_lib_get_VHT_BW(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the VHT_SGI from RT2860.
 *
 * This function get the VHT_SGI from RT2860. submit by 
 * \param[out]	 the VHT_SGI
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the VHT_SGI from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the VHT_SGI from RT2860.
 */

int wifimgr_lib_get_VHT_SGI(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_EXTCHA from RT2860.
 *
 * This function get the HT_EXTCHA from RT2860. submit by 
 * \param[out]	 the HT_EXTCHA
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_EXTCHA from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_EXTCHA from RT2860.
 */

int wifimgr_lib_get_HT_EXTCHA(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_OpMode from RT2860.
 *
 * This function get the HT_OpMode from RT2860. submit by 
 * \param[out]	 the HT_OpMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_OpMode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_OpMode from RT2860.
 */

int wifimgr_lib_get_HT_OpMode(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_BSSCoexistence from RT2860.
 *
 * This function get the HT_BSSCoexistence from RT2860. submit by 
 * \param[out]	 the HT_BSSCoexistence
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_BSSCoexistence from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_BSSCoexistence from RT2860.
 */

int wifimgr_lib_get_HT_BSSCoexistence(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_TXStream from RT2860.
 *
 * This function get the HT_TXStream from RT2860. submit by 
 * \param[out]	 the HT_TXStream
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_TXStream from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_TXStream from RT2860.
 */

int wifimgr_lib_get_HT_TXStream(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_RXStream from RT2860.
 *
 * This function get the HT_RXStream from RT2860. submit by 
 * \param[out]	 the HT_RXStream
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_RXStream from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_RXStream from RT2860.
 */

int wifimgr_lib_get_HT_RXStream(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_GI from RT2860.
 *
 * This function get the HT_GI from RT2860. submit by 
 * \param[out]	 the HT_GI
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_GI from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_GI from RT2860.
 */

int wifimgr_lib_get_HT_GI(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_MCS from RT2860.
 *
 * This function get the HT_MCS from RT2860. submit by 
 * \param[out]	 the HT_MCS
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_MCS from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_MCS from RT2860.
 */

int wifimgr_lib_get_HT_MCS(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_AutoBA from RT2860.
 *
 * This function get the HT_AutoBA from RT2860. submit by 
 * \param[out]	 the HT_AutoBA
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_AutoBA from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_AutoBA from RT2860.
 */

int wifimgr_lib_get_HT_AutoBA(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_RDG from RT2860.
 *
 * This function get the HT_RDG from RT2860. submit by 
 * \param[out]	 the HT_RDG
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_RDG from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_RDG from RT2860.
 */

int wifimgr_lib_get_HT_RDG(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_MpduDensity from RT2860.
 *
 * This function get the HT_MpduDensity from RT2860. submit by 
 * \param[out]	 the HT_MpduDensity
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_MpduDensity from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_MpduDensity from RT2860.
 */

int wifimgr_lib_get_HT_MpduDensity(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_BAWinSize from RT2860.
 *
 * This function get the HT_BAWinSize from RT2860. submit by 
 * \param[out]	 the HT_BAWinSize
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_BAWinSize from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_BAWinSize from RT2860.
 */

int wifimgr_lib_get_HT_BAWinSize(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_HTC from RT2860.
 *
 * This function get the HT_HTC from RT2860. submit by 
 * \param[out]	 the HT_HTC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_HTC from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_HTC from RT2860.
 */

int wifimgr_lib_get_HT_HTC(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_LinkAdapt from RT2860.
 *
 * This function get the HT_LinkAdapt from RT2860. submit by 
 * \param[out]	 the HT_LinkAdapt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_LinkAdapt from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_LinkAdapt from RT2860.
 */

int wifimgr_lib_get_HT_LinkAdapt(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_AMSDU from RT2860.
 *
 * This function get the HT_AMSDU from RT2860. submit by 
 * \param[out]	 the HT_AMSDU
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_AMSDU from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_AMSDU from RT2860.
 */

int wifimgr_lib_get_HT_AMSDU(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the ssid from RT2860.
 *
 * This function get the ssid from RT2860. submit by 
 * \param[out]	 the ssid
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the ssid from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the ssid from RT2860.
 */

int wifimgr_lib_get_ssid(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the wepAuthType from RT2860.
 *
 * This function get the wepAuthType from RT2860. submit by 
 * \param[out]	 the wepAuthType
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wepAuthType from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wepAuthType from RT2860.
 */

int wifimgr_lib_get_wepAuthType(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the authMode from RT2860.
 *
 * This function get the authMode from RT2860. submit by 
 * \param[out]	 the authMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the authMode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the authMode from RT2860.
 */

int wifimgr_lib_get_authMode(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the txRate from RT2860.
 *
 * This function get the txRate from RT2860. submit by 
 * \param[out]	 the txRate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the txRate from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the txRate from RT2860.
 */

int wifimgr_lib_get_txRate(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the noForwarding from RT2860.
 *
 * This function get the noForwarding from RT2860. submit by 
 * \param[out]	 the noForwarding
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the noForwarding from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the noForwarding from RT2860.
 */

int wifimgr_lib_get_noForwarding(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the PMFMFPC from RT2860.
 *
 * This function get the PMFMFPC from RT2860. submit by 
 * \param[out]	 the PMFMFPC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the PMFMFPC from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the PMFMFPC from RT2860.
 */

int wifimgr_lib_get_PMFMFPC(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the PMFMFPR from RT2860.
 *
 * This function get the PMFMFPR from RT2860. submit by 
 * \param[out]	 the PMFMFPR
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the PMFMFPR from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the PMFMFPR from RT2860.
 */

int wifimgr_lib_get_PMFMFPR(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the PMFSHA256 from RT2860.
 *
 * This function get the PMFSHA256 from RT2860. submit by 
 * \param[out]	 the PMFSHA256
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the PMFSHA256 from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the PMFSHA256 from RT2860.
 */

int wifimgr_lib_get_PMFSHA256(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the defaultKeyID from RT2860.
 *
 * This function get the defaultKeyID from RT2860. submit by 
 * \param[out]	 the defaultKeyID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the defaultKeyID from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the defaultKeyID from RT2860.
 */

int wifimgr_lib_get_defaultKeyID(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the WPAPSK from RT2860.
 *
 * This function get the WPAPSK from RT2860. submit by 
 * \param[out]	 the WPAPSK
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WPAPSK from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WPAPSK from RT2860.
 */

int wifimgr_lib_get_WPAPSK(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the maxStaNum from RT2860.
 *
 * This function get the maxStaNum from RT2860. submit by 
 * \param[out]	 the maxStaNum
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the maxStaNum from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the maxStaNum from RT2860.
 */

int wifimgr_lib_get_maxStaNum(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the mbssMaxStaNum from RT2860.
 *
 * This function get the mbssMaxStaNum from RT2860. submit by 
 * \param[out]	 the mbssMaxStaNum
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the mbssMaxStaNum from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the mbssMaxStaNum from RT2860.
 */

int wifimgr_lib_get_mbssMaxStaNum(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the keyType from RT2860.
 *
 * This function get the keyType from RT2860. submit by 
 * \param[out]	 the keyType
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the keyType from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the keyType from RT2860.
 */

int wifimgr_lib_get_keyType(wifi_type type, int index, int num, char *output, unsigned int size);

/**
 * \brief This function get the key1Type from RT2860.
 *
 * This function get the key1Type from RT2860. submit by 
 * \param[out]	 the key1Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key1Type from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key1Type from RT2860.
 */

int wifimgr_lib_get_key1Type(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the key2Type from RT2860.
 *
 * This function get the key2Type from RT2860. submit by 
 * \param[out]	 the key2Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key2Type from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key2Type from RT2860.
 */

int wifimgr_lib_get_key2Type(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the key3Type from RT2860.
 *
 * This function get the key3Type from RT2860. submit by 
 * \param[out]	 the key3Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key3Type from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key3Type from RT2860.
 */

int wifimgr_lib_get_key3Type(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the key4Type from RT2860.
 *
 * This function get the key4Type from RT2860. submit by 
 * \param[out]	 the key4Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key4Type from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key4Type from RT2860.
 */

int wifimgr_lib_get_key4Type(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the keystr from RT2860.
 *
 * This function get the keystr from RT2860. submit by 
 * \param[out]	 the keystr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the keystr from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the keystr from RT2860.
 */

int wifimgr_lib_get_keyStr(wifi_type type, int index, int num, char *output, unsigned int size);

/**
 * \brief This function get the key1str from RT2860.
 *
 * This function get the key1str from RT2860. submit by 
 * \param[out]	 the key1str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key1str from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key1str from RT2860.
 */

int wifimgr_lib_get_key1Str(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the key2str from RT2860.
 *
 * This function get the key2str from RT2860. submit by 
 * \param[out]	 the key2str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key2str from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key2str from RT2860.
 */

int wifimgr_lib_get_key2Str(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the key3str from RT2860.
 *
 * This function get the key3str from RT2860. submit by 
 * \param[out]	 the key3str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key3str from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key3str from RT2860.
 */

int wifimgr_lib_get_key3Str(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the key4str from RT2860.
 *
 * This function get the key4str from RT2860. submit by 
 * \param[out]	 the key4str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the key4str from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the key4str from RT2860.
 */

int wifimgr_lib_get_key4Str(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the encrypType from RT2860.
 *
 * This function get the encrypType from RT2860. submit by 
 * \param[out]	 the encrypType
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the encrypType from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the encrypType from RT2860.
 */

int wifimgr_lib_get_encrypType(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the preAuth from RT2860.
 *
 * This function get the preAuth from RT2860. submit by 
 * \param[out]	 the preAuth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the preAuth from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the preAuth from RT2860.
 */

int wifimgr_lib_get_preAuth(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the hideSSID from RT2860.
 *
 * This function get the hideSSID from RT2860. submit by 
 * \param[out]	 the hideSSID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the hideSSID from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the hideSSID from RT2860.
 */

int wifimgr_lib_get_hideSSID(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the wmmCapable from RT2860.
 *
 * This function get the wmmCapable from RT2860. submit by 
 * \param[out]	 the wmmCapable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wmmCapable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wmmCapable from RT2860.
 */

int wifimgr_lib_get_wmmCapable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the fixedTxMode from RT2860.
 *
 * This function get the fixedTxMode from RT2860. submit by 
 * \param[out]	 the fixedTxMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the fixedTxMode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the fixedTxMode from RT2860.
 */

int wifimgr_lib_get_fixedTxMode(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the apsdCapable from RT2860.
 *
 * This function get the apsdCapable from RT2860. submit by 
 * \param[out]	 the apsdCapable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the apsdCapable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the apsdCapable from RT2860.
 */

int wifimgr_lib_get_apsdCapable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the dlsCapable from RT2860.
 *
 * This function get the dlsCapable from RT2860. submit by 
 * \param[out]	 the dlsCapable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the dlsCapable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the dlsCapable from RT2860.
 */

int wifimgr_lib_get_dlsCapable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the apAifsn from RT2860.
 *
 * This function get the apAifsn from RT2860. submit by 
 * \param[out]	 the apAifsn
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the apAifsn from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the apAifsn from RT2860.
 */

int wifimgr_lib_get_apAifsn(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the apCwmin from RT2860.
 *
 * This function get the apCwmin from RT2860. submit by 
 * \param[out]	 the apCwmin
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the apCwmin from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the apCwmin from RT2860.
 */

int wifimgr_lib_get_apCwmin(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the apCwmax from RT2860.
 *
 * This function get the apCwmax from RT2860. submit by 
 * \param[out]	 the apCwmax
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the apCwmax from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the apCwmax from RT2860.
 */

int wifimgr_lib_get_apCwmax(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the apTxop from RT2860.
 *
 * This function get the apTxop from RT2860. submit by 
 * \param[out]	 the apTxop
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the apTxop from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the apTxop from RT2860.
 */

int wifimgr_lib_get_apTxop(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the apACM from RT2860.
 *
 * This function get the apACM from RT2860. submit by 
 * \param[out]	 the apACM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the apACM from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the apACM from RT2860.
 */

int wifimgr_lib_get_apACM(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the bssAifsn from RT2860.
 *
 * This function get the bssAifsn from RT2860. submit by 
 * \param[out]	 the bssAifsn
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the bssAifsn from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the bssAifsn from RT2860.
 */

int wifimgr_lib_get_bssAifsn(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the bssCwmin from RT2860.
 *
 * This function get the bssCwmin from RT2860. submit by 
 * \param[out]	 the bssCwmin
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the bssCwmin from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the bssCwmin from RT2860.
 */

int wifimgr_lib_get_bssCwmin(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the bssCwmax from RT2860.
 *
 * This function get the bssCwmax from RT2860. submit by 
 * \param[out]	 the bssCwmax
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the bssCwmax from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the bssCwmax from RT2860.
 */

int wifimgr_lib_get_bssCwmax(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the bssTxop from RT2860.
 *
 * This function get the bssTxop from RT2860. submit by 
 * \param[out]	 the bssTxop
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the bssTxop from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the bssTxop from RT2860.
 */

int wifimgr_lib_get_bssTxop(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the bssACM from RT2860.
 *
 * This function get the bssACM from RT2860. submit by 
 * \param[out]	 the bssACM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the bssACM from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the bssACM from RT2860.
 */

int wifimgr_lib_get_bssACM(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the ackPolicy from RT2860.
 *
 * This function get the ackPolicy from RT2860. submit by 
 * \param[out]	 the ackPolicy
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the ackPolicy from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the ackPolicy from RT2860.
 */

int wifimgr_lib_get_ackPolicy(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the macAddress from RT2860.
 *
 * This function get the macAddress from RT2860. submit by 
 * \param[out]	 the macAddress
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the macAddress from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the macAddress from RT2860.
 */

int wifimgr_lib_get_macAddress(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the radiusserver from RT2860.
 *
 * This function get the radiusserver from RT2860. submit by 
 * \param[out]	 the radiusserver
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the radiusserver from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the radiusserver from RT2860.
 */

int wifimgr_lib_get_radiusserver(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the radiusport from RT2860.
 *
 * This function get the radiusport from RT2860. submit by 
 * \param[out]	 the radiusport
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the radiusport from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the radiusport from RT2860.
 */

int wifimgr_lib_get_radiusport(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the radiuskey from RT2860.
 *
 * This function get the radiuskey from RT2860. submit by 
 * \param[out]	 the radiuskey
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the radiuskey from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the radiuskey from RT2860.
 */

int wifimgr_lib_get_radiuskey(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the ieee8021x from RT2860.
 *
 * This function get the ieee8021x from RT2860. submit by 
 * \param[out]	 the ieee8021x
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the ieee8021x from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the ieee8021x from RT2860.
 */

int wifimgr_lib_get_ieee8021x(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the session_timeout_interval from RT2860.
 *
 * This function get the session_timeout_interval from RT2860. submit by 
 * \param[out]	 the session_timeout_interval
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the session_timeout_interval from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the session_timeout_interval from RT2860.
 */

int wifimgr_lib_get_session_timeout_interval(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the wdsenable from RT2860.
 *
 * This function get the wdsenable from RT2860. submit by 
 * \param[out]	 the wdsenable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wdsenable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wdsenable from RT2860.
 */

int wifimgr_lib_get_wdsenable(wifi_type type, int index, char * output, unsigned int size);

/**
 * \brief This function get the wdslist from RT2860.
 *
 * This function get the wdslist from RT2860. submit by 
 * \param[out]	 the wdslist
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wdslist from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wdslist from RT2860.
 */

int wifimgr_lib_get_wdslist(wifi_type type, int index, char * output, unsigned int size);

/**
 * \brief This function get the wdsencryptype from RT2860.
 *
 * This function get the wdsencryptype from RT2860. submit by 
 * \param[out]	 the wdslist
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wdsencryptype from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wdsencryptype from RT2860.
 */

int wifimgr_lib_get_wdsencryptype(wifi_type type, int index, char * output, unsigned int size);

/**
 * \brief This function get the wdskey from RT2860.
 *
 * This function get the wdskey from RT2860. submit by 
 * \param[out]	 the wdslist
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wdskey from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wdskey from RT2860.
 */

int wifimgr_lib_get_wdskey(wifi_type type, int index, char * output, unsigned int size);

/**
 * \brief This function get the wdsphymode from RT2860.
 *
 * This function get the wdsphymode from RT2860. submit by 
 * \param[out]	 the wdsphymode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wdsphymode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wdsphymode from RT2860.
 */

int wifimgr_lib_get_wdsphymode(wifi_type type, int index, char * output, unsigned int size);

/**
 * \brief This function get the ownip from RT2860.
 *
 * This function get the ownip from RT2860. submit by 
 * \param[out]	 the ownip
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the ownip from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the ownip from RT2860.
 */

int wifimgr_lib_get_ownip(wifi_type type, int index, char * output, unsigned int size);

/**
 * \brief This function get the wscPinCode from RT2860.
 *
 * This function get the wscPinCode from RT2860. submit by 
 * \param[out]	 the wscPinCode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wscPinCode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wscPinCode from RT2860.
 */

int wifimgr_lib_get_wscPinCode(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the accessPolicy from RT2860.
 *
 * This function get the accessPolicy from RT2860. submit by 
 * \param[out]	 the accessPolicy
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the accessPolicy from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the accessPolicy from RT2860.
 */

int wifimgr_lib_get_accessPolicy(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the accessControlList from RT2860.
 *
 * This function get the accessControlList from RT2860. submit by 
 * \param[out]	 the accessControlList
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the accessControlList from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the accessControlList from RT2860.
 */

int wifimgr_lib_get_accessControlList(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the wscDefaultSSID from RT2860.
 *
 * This function get the wscDefaultSSID from RT2860. submit by 
 * \param[out]	 the wscDefaultSSID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wscDefaultSSID from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wscDefaultSSID from RT2860.
 */

int wifimgr_lib_get_wscDefaultSSID(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the WscV2Support from RT2860.
 *
 * This function get the WscV2Support from RT2860. submit by feiyu.zhu
 * \param[out]	 the WscV2Support
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WscV2Support from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the WscV2Support from RT2860.
 */

int wifimgr_lib_get_wscV2Support(wifi_type type, int index, char* output, unsigned int size);

/**
 * \brief This function get the reloadFlag from RT2860.
 *
 * This function get the reloadFlag from RT2860. submit by 
 * \param[out]	 the reloadFlag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the reloadFlag from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the reloadFlag from RT2860.
 */

int wifimgr_lib_get_reloadFlag(wifi_type type, char* output, unsigned int size);

/**
 * \brief This function get the EfuseBufferMode from RT2860.
 *
 * This function get the EfuseBufferMode from RT2860. submit by 
 * \param[out]	 the EfuseBufferMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the EfuseBufferMode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the EfuseBufferMode from RT2860.
 */

int wifimgr_lib_get_EfuseBufferMode(wifi_type type,int index, char* output, unsigned int size);

/**
 * \brief This function get the BndstrgChanload from ioctl.
 *
 * This function get the BndstrgChanload from ioctl. submit by 
 * \param[out]	 the BndstrgChanload
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the BndstrgChanload from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the BndstrgChanload from ioctl.
 */

int wifimgr_lib_get_BndstrgChanLoad(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the igmpSnEnable from RT2860.
 *
 * This function get the igmpSnEnable from RT2860. submit by 
 * \param[out]	 the igmpSnEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the igmpSnEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the igmpSnEnable from RT2860.
 */

int wifimgr_lib_get_igmpSnEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the Sta11kCap from RT2860.
 *
 * This function get the Sta11kCap from RT2860. submit by 
 * \param[out]	 the Sta11kCap
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the Sta11kCap from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the Sta11kCap from RT2860.
 */

int wifimgr_lib_get_Sta11kCap(wifi_type type, int index, unsigned char *mac, unsigned char* output, unsigned int size);

/**
 * \brief This function get the Sta11vCap.
 *
 * This function get the Sta11vCap. submit by 
 * \param[out]	 the Sta11vCap
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the Sta11vCap.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the Sta11vCap .
 */

int wifimgr_lib_get_Sta11vCap(wifi_type type, int index, unsigned char *mac, unsigned char* output, unsigned int size);

/**
 * \brief This function get the HT_LDPC.
 *
 * This function get the HT_LDPC. submit by 
 * \param[out]	 the HT_LDPC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_LDPC.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HT_LDPC.
 */

int wifimgr_lib_get_HT_LDPC(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the AMSDU_NUM from RT2860.
 *
 * This function get the AMSDU_NUM from RT2860. submit by 
 * \param[out]	 the AMSDU_NUM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the AMSDU_NUM from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the AMSDU_NUM from RT2860.
 */

int wifimgr_lib_get_AMSDU_NUM(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MapEnable from RT2860.
 *
 * This function get the MapEnable from RT2860. submit by 
 * \param[out]	 the MapEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MapEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MapEnable from RT2860.
 */

int wifimgr_lib_get_MapEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MAP_Turnkey from RT2860.
 *
 * This function get the MAP_Turnkey from RT2860. submit by 
 * \param[out]	 the MAP_Turnkey
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MAP_Turnkey from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MAP_Turnkey from RT2860.
 */

int wifimgr_lib_get_MAP_Turnkey(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MAP_Ext from RT2860.
 *
 * This function get the MAP_Ext from RT2860. submit by 
 * \param[out]	 the MAP_Ext
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MAP_Ext from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MAP_Ext from RT2860.
 */

int wifimgr_lib_get_MAP_Ext(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MuOfdmaDlEnable from RT2860.
 *
 * This function get the MuOfdmaDlEnable from RT2860. submit by 
 * \param[out]	 the MuOfdmaDlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MuOfdmaDlEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MuOfdmaDlEnable from RT2860.
 */

int wifimgr_lib_get_MuOfdmaDlEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MuOfdmaUlEnable from RT2860.
 *
 * This function get the MuOfdmaUlEnable from RT2860. submit by 
 * \param[out]	 the MuOfdmaUlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MuOfdmaUlEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MuOfdmaUlEnable from RT2860.
 */

int wifimgr_lib_get_MuOfdmaUlEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MuOfdmaUlEnable from RT2860.
 *
 * This function get the MuOfdmaUlEnable from RT2860. submit by 
 * \param[out]	 the MuOfdmaUlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MuOfdmaUlEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MuOfdmaUlEnable from RT2860.
 */

int wifimgr_lib_get_MuMimoDlEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MuMimoUlEnable from RT2860.
 *
 * This function get the MuMimoUlEnable from RT2860. submit by 
 * \param[out]	 the MuMimoUlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MuMimoUlEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MuMimoUlEnable from RT2860.
 */

int wifimgr_lib_get_MuMimoUlEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the SREnable from RT2860.
 *
 * This function get the SREnable from RT2860. submit by 
 * \param[out]	 the SREnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the SREnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the SREnable from RT2860.
 */

int wifimgr_lib_get_SREnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the SRMode from RT2860.
 *
 * This function get the SRMode from RT2860. submit by 
 * \param[out]	 the SRMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the SRMode from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the SRMode from RT2860.
 */

int wifimgr_lib_get_SRMode(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the SRSDEnable from RT2860.
 *
 * This function get the SRSDEnable from RT2860. submit by 
 * \param[out]	 the SRSDEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the SRSDEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the SRSDEnable from RT2860.
 */

int wifimgr_lib_get_SRSDEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the PPEnable from RT2860.
 *
 * This function get the PPEnable from RT2860. submit by 
 * \param[out]	 the PPEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the PPEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the PPEnable from RT2860.
 */

int wifimgr_lib_get_PPEnable(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the PPEnable from RT2860.
 *
 * This function get the PPEnable from RT2860. submit by 
 * \param[out]	 the PPEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the PPEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the PPEnable from RT2860.
 */

int wifimgr_lib_get_TWTSupport(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HeraStbcPriority from RT2860.
 *
 * This function get the HeraStbcPriority from RT2860. submit by 
 * \param[out]	 the HeraStbcPriority
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HeraStbcPriority from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the HeraStbcPriority from RT2860.
 */

int wifimgr_lib_get_HeraStbcPriority(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MaxRuOfdma from RT2860.
 *
 * This function get the MaxRuOfdma from RT2860. submit by 
 * \param[out]	 the MaxRuOfdma
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MaxRuOfdma from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MaxRuOfdma from RT2860.
 */

int wifimgr_lib_get_MaxRuOfdma(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MaxDLMuMimo from RT2860.
 *
 * This function get the MaxDLMuMimo from RT2860. submit by 
 * \param[out]	 the MaxDLMuMimo
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MaxDLMuMimo from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MaxDLMuMimo from RT2860.
 */

int wifimgr_lib_get_MaxDLMuMimo(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the MaxULMuMimo from RT2860.
 *
 * This function get the MaxULMuMimo from RT2860. submit by 
 * \param[out]	 the MaxULMuMimo
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the MaxULMuMimo from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the MaxULMuMimo from RT2860.
 */

int wifimgr_lib_get_MaxULMuMimo(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the Disable160RuMu from RT2860.
 *
 * This function get the Disable160RuMu from RT2860. submit by 
 * \param[out]	 the Disable160RuMu
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the Disable160RuMu from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the Disable160RuMu from RT2860.
 */

int wifimgr_lib_get_Disable160RuMu(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the Vht1024QamSupport from RT2860.
 *
 * This function get the Vht1024QamSupport from RT2860. submit by 
 * \param[out]	 the Vht1024QamSupport
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the Vht1024QamSupport from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the Vht1024QamSupport from RT2860.
 */

int wifimgr_lib_get_Vht1024QamSupport(wifi_type type, int index, char *output, unsigned int size);

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
int wifimgr_lib_get_cap(wifi_type type,chip_info* chipinfo);
#endif


/**
 * \brief This function get the wifimgr_lib_get_DscpPriMapEnable from RT2860.
 *
 * This function get the wifimgr_lib_get_DscpPriMapEnable from RT2860. submit by 
 * \param[out]	 the wifimgr_lib_get_DscpPriMapEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wifimgr_lib_get_DscpPriMapEnable from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wifimgr_lib_get_DscpPriMapEnable from RT2860.
 */


int wifimgr_lib_get_DscpPriMapEnable(wifi_type type, int index, char *output, unsigned int size);


/**
 * \brief This function set the wifimgr_lib_set_DscpPriMapEnable to RT2860.
 *
 * This function set the wifimgr_lib_get_DscpPriMapEnable to RT2860. submit by 
 * \param[int]	 the wifimgr_lib_get_DscpPriMapEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wifimgr_lib_set_DscpPriMapEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wifimgr_lib_get_DscpPriMapEnable to RT2860.
 */

int wifimgr_lib_set_DscpPriMapEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the ssid to RT2860.
 *
 * This function set the ssid to RT2860. submit by 
 * \param[int]	 the ssid
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ssid to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ssid to RT2860.
 */

int wifimgr_lib_set_ssid(wifi_type type, int index, char* name);

/**
 * \brief This function set the countryRegion to RT2860.
 *
 * This function set the countryRegion to RT2860. submit by 
 * \param[int]	 the countryRegion
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the countryRegion to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the countryRegion to RT2860.
 */

int wifimgr_lib_set_countryRegion(wifi_type type, int index, char* input);

/**
 * \brief This function set the countryRegionABand to RT2860.
 *
 * This function set the countryRegionABand to RT2860. submit by 
 * \param[int]	 the countryRegionABand
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the countryRegionABand to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the countryRegionABand to RT2860.
 */

int wifimgr_lib_set_countryRegionABand(wifi_type type, int index, char* input);

/**
 * \brief This function set the countryCode to RT2860.
 *
 * This function set the countryCode to RT2860. submit by 
 * \param[int]	 the countryCode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the countryCode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the countryCode to RT2860.
 */

int wifimgr_lib_set_countryCode(wifi_type type, int index, char* input);

/**
 * \brief This function set the beaconPeriod to RT2860.
 *
 * This function set the beaconPeriod to RT2860. submit by 
 * \param[int]	 the beaconPeriod
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the beaconPeriod to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the beaconPeriod to RT2860.
 */

int wifimgr_lib_set_beaconPeriod(wifi_type type, int index, char *input);

/**
 * \brief This function set the rtsThreshold to RT2860.
 *
 * This function set the rtsThreshold to RT2860. submit by 
 * \param[int]	 the rtsThreshold
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the rtsThreshold to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the rtsThreshold to RT2860.
 */

int wifimgr_lib_set_rtsThreshold(wifi_type type, int index, char *input);

/**
 * \brief This function set the fragThreshold to RT2860.
 *
 * This function set the fragThreshold to RT2860. submit by 
 * \param[int]	 the fragThreshold
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the fragThreshold to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the fragThreshold to RT2860.
 */

int wifimgr_lib_set_fragThreshold(wifi_type type, int index, char *input);

/**
 * \brief This function set the dtimPeriod to RT2860.
 *
 * This function set the dtimPeriod to RT2860. submit by 
 * \param[int]	 the dtimPeriod
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the dtimPeriod to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the dtimPeriod to RT2860.
 */

int wifimgr_lib_set_dtimPeriod(wifi_type type, int index, char *input);

/**
 * \brief This function set the txPower to RT2860.
 *
 * This function set the txPower to RT2860. submit by 
 * \param[int]	 the txPower
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the txPower to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the txPower to RT2860.
 */

int wifimgr_lib_set_txPower(wifi_type type, int index, char *input);

/**
 * \brief This function set the PERCENTAGEenable to RT2860.
 *
 * This function set the PERCENTAGEenable to RT2860. submit by 
 * \param[int]	 the PERCENTAGEenable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the PERCENTAGEenable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the PERCENTAGEenable to RT2860.
 */

int wifimgr_lib_set_PERCENTAGEenable(wifi_type type, int index, char *input);

/**
 * \brief This function set the bgProtection to RT2860.
 *
 * This function set the bgProtection to RT2860. submit by 
 * \param[int]	 the bgProtection
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the bgProtection to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the bgProtection to RT2860.
 */

int wifimgr_lib_set_bgProtection(wifi_type type, int index, char *input);

/**
 * \brief This function set the disableOLBC to RT2860.
 *
 * This function set the disableOLBC to RT2860. submit by 
 * \param[int]	 the disableOLBC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the disableOLBC to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the disableOLBC to RT2860.
 */

int wifimgr_lib_set_disableOLBC(wifi_type type, int index, char *input);

/**
 * \brief This function set the txPreamble to RT2860.
 *
 * This function set the txPreamble to RT2860. submit by 
 * \param[int]	 the txPreamble
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the txPreamble to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the txPreamble to RT2860.
 */

int wifimgr_lib_set_txPreamble(wifi_type type, int index, char *input);

/**
 * \brief This function set the txBurst to RT2860.
 *
 * This function set the txBurst to RT2860. submit by 
 * \param[int]	 the txBurst
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the txBurst to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the txBurst to RT2860.
 */

int wifimgr_lib_set_txBurst(wifi_type type, int index, char *input);

/**
 * \brief This function set the pktAggregate to RT2860.
 *
 * This function set the pktAggregate to RT2860. submit by 
 * \param[int]	 the pktAggregate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the pktAggregate to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the pktAggregate to RT2860.
 */

int wifimgr_lib_set_pktAggregate(wifi_type type, int index, char *input);

/**
 * \brief This function set the turboRate to RT2860.
 *
 * This function set the turboRate to RT2860. submit by 
 * \param[int]	 the turboRate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the turboRate to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the turboRate to RT2860.
 */

int wifimgr_lib_set_turboRate(wifi_type type, int index, char *input);

/**
 * \brief This function set the noForwardingBTNBSSID to RT2860.
 *
 * This function set the noForwardingBTNBSSID to RT2860. submit by 
 * \param[int]	 the noForwardingBTNBSSID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the noForwardingBTNBSSID to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the noForwardingBTNBSSID to RT2860.
 */

int wifimgr_lib_set_noForwardingBTNBSSID(wifi_type type, int index, char *input);

/**
 * \brief This function set the shortSlot to RT2860.
 *
 * This function set the shortSlot to RT2860. submit by 
 * \param[int]	 the shortSlot
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the shortSlot to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the shortSlot to RT2860.
 */

int wifimgr_lib_set_shortSlot(wifi_type type, int index, char *input);

/**
 * \brief This function set the wirelessEvent to RT2860.
 *
 * This function set the wirelessEvent to RT2860. submit by 
 * \param[int]	 the wirelessEvent
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wirelessEvent to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wirelessEvent to RT2860.
 */

int wifimgr_lib_set_wirelessEvent(wifi_type type, int index, char *input);

/**
 * \brief This function set the rekeyInterval to RT2860.
 *
 * This function set the rekeyInterval to RT2860. submit by 
 * \param[int]	 the rekeyInterval
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the rekeyInterval to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the rekeyInterval to RT2860.
 */

int wifimgr_lib_set_rekeyInterval(wifi_type type, int index, char *input);

/**
 * \brief This function set the rekeyMethod to RT2860.
 *
 * This function set the rekeyMethod to RT2860. submit by 
 * \param[int]	 the rekeyMethod
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the rekeyMethod to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the rekeyMethod to RT2860.
 */

int wifimgr_lib_set_rekeyMethod(wifi_type type, int index, char *input);

/**
 * \brief This function set the pmkCachePeriod to RT2860.
 *
 * This function set the pmkCachePeriod to RT2860. submit by 
 * \param[int]	 the pmkCachePeriod
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the pmkCachePeriod to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the pmkCachePeriod to RT2860.
 */

int wifimgr_lib_set_pmkCachePeriod(wifi_type type, int index, char *input);

/**
 * \brief This function set the wirelessMode to RT2860.
 *
 * This function set the wirelessMode to RT2860. submit by 
 * \param[int]	 the wirelessMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wirelessMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wirelessMode to RT2860.
 */

int wifimgr_lib_set_wirelessMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the basicRate to RT2860.
 *
 * This function set the basicRate to RT2860. submit by 
 * \param[int]	 the basicRate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the basicRate to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the basicRate to RT2860.
 */

int wifimgr_lib_set_basicRate(wifi_type type, int index, char *input);

/**
 * \brief This function set the wirelessMode to RT2860.
 *
 * This function set the wirelessMode to RT2860. submit by 
 * \param[int]	 the wirelessMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wirelessMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wirelessMode to RT2860.
 */

int wifimgr_lib_set_wirelessMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the wscOOB to RT2860.
 *
 * This function set the wscOOB to RT2860. submit by 
 * \param[int]	 the wirelessMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscOOB to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscOOB to RT2860.
 */
 
int wifimgr_lib_set_wscOOB(wifi_type type, int index);

/**
 * \brief This function set the radiusserver to RT2860.
 *
 * This function set the radiusserver to RT2860. submit by 
 * \param[int]	 the radiusserver
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the radiusserver to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the radiusserver to RT2860.
 */

int wifimgr_lib_set_radiusserver(wifi_type type, int index, char* input);

/**
 * \brief This function set the radiusport to RT2860.
 *
 * This function set the radiusport to RT2860. submit by 
 * \param[int]	 the radiusport
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the radiusport to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the radiusport to RT2860.
 */

int wifimgr_lib_set_radiusport(wifi_type type, int index, char* input);

/**
 * \brief This function set the radiuskey to RT2860.
 *
 * This function set the radiuskey to RT2860. submit by 
 * \param[int]	 the radiuskey
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the radiuskey to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the radiuskey to RT2860.
 */

int wifimgr_lib_set_radiuskey(wifi_type type, int index, char* input);

/**
 * \brief This function set the ieee8021x to RT2860.
 *
 * This function set the ieee8021x to RT2860. submit by 
 * \param[int]	 the ieee8021x
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ieee8021x to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ieee8021x to RT2860.
 */

int wifimgr_lib_set_ieee8021x(wifi_type type, int index, char* input);

/**
 * \brief This function set the session_timeout_interval to RT2860.
 *
 * This function set the session_timeout_interval to RT2860. submit by 
 * \param[int]	 the session_timeout_interval
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the session_timeout_interval to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the session_timeout_interval to RT2860.
 */

int wifimgr_lib_set_session_timeout_interval(wifi_type type, int index, char* input);

/**
 * \brief This function set the wdsenable to RT2860.
 *
 * This function set the wdsenable to RT2860. submit by 
 * \param[int]	 the wdsenable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wdsenable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wdsenable to RT2860.
 */

int wifimgr_lib_set_wdsenable(wifi_type type, int index, char* input);

/**
 * \brief This function set the wdslist to RT2860.
 *
 * This function set the wdslist to RT2860. submit by 
 * \param[int]	 the wdslist
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wdslist to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wdslist to RT2860.
 */

int wifimgr_lib_set_wdslist(wifi_type type, int index, char* input);

/**
 * \brief This function set the wdsencryptype to RT2860.
 *
 * This function set the wdsencryptype to RT2860. submit by 
 * \param[int]	 the wdsencryptype
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wdsencryptype to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wdsencryptype to RT2860.
 */

int wifimgr_lib_set_wdsencryptype(wifi_type type, int index, char* input);

/**
 * \brief This function set the wdskey to RT2860.
 *
 * This function set the wdskey to RT2860. submit by 
 * \param[int]	 the wdskey
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wdskey to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wdskey to RT2860.
 */

int wifimgr_lib_set_wdskey(wifi_type type, int index, char* input);

/**
 * \brief This function set the wdsphymode to RT2860.
 *
 * This function set the wdsphymode to RT2860. submit by 
 * \param[int]	 the wdsphymode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wdsphymode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wdsphymode to RT2860.
 */

int wifimgr_lib_set_wdsphymode(wifi_type type, int index, char* input);

/**
 * \brief This function set the ownip to RT2860.
 *
 * This function set the ownip to RT2860. submit by 
 * \param[int]	 the ownip
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ownip to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ownip to RT2860.
 */

int wifimgr_lib_set_ownip(wifi_type type, int index, char* input);

/**
 * \brief This function set the MacAddress to RT2860.
 *
 * This function set the MacAddress to RT2860. submit by 
 * \param[int]	 the MacAddress
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MacAddress to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MacAddress to RT2860.
 */

int wifimgr_lib_set_MacAddress(wifi_type type, int index, char* input);

/**
 * \brief This function set the HT_BW to RT2860.
 *
 * This function set the HT_BW to RT2860. submit by 
 * \param[int]	 the HT_BW
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_BW to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_BW to RT2860.
 */

int wifimgr_lib_set_HT_BW(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_STBC to RT2860.
 *
 * This function set the HT_STBC to RT2860. submit by 
 * \param[int]	 the HT_STBC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_STBC to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_STBC to RT2860.
 */

int wifimgr_lib_set_HT_STBC(wifi_type type, int index, char *input);

/**
 * \brief This function set the IEEE80211H to RT2860.
 *
 * This function set the IEEE80211H to RT2860. submit by 
 * \param[int]	 the IEEE80211H
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the IEEE80211H to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the IEEE80211H to RT2860.
 */

int wifimgr_lib_set_IEEE80211H(wifi_type type, int index, char *input);

/**
 * \brief This function set the G_BAND_256QAM to RT2860.
 *
 * This function set the G_BAND_256QAM to RT2860. submit by 
 * \param[int]	 the G_BAND_256QAM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the G_BAND_256QAM to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the G_BAND_256QAM to RT2860.
 */

int wifimgr_lib_set_G_BAND_256QAM(wifi_type type, int index, char *input);

/**
 * \brief This function set the WHNAT to RT2860.
 *
 * This function set the WHNAT to RT2860. submit by 
 * \param[int]	 the WHNAT
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the WHNAT to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the WHNAT to RT2860.
 */

int wifimgr_lib_set_WHNAT(wifi_type type, int index, char *input);


/**
 * \brief This function set the WOEOFF to RT2860.
 *
 * This function set the SHORTGIFLAG to RT2860. submit by 
 * \param[int]	 the WOEOFF
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the SHORTGIFLAG to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the SHORTGIFLAG to RT2860.
 */

int wifimgr_lib_set_WOEOFF(wifi_type type, int index, char *input);

/**
 * \brief This function set the SHORTGIFLAG to RT2860.
 *
 * This function set the SHORTGIFLAG to RT2860. submit by 
 * \param[int]	 the SHORTGIFLAG
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the SHORTGIFLAG to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the SHORTGIFLAG to RT2860.
 */

int wifimgr_lib_set_SHORTGIFLAG(wifi_type type, int index, char *input);

/**
 * \brief This function set the StaNumDetect to RT2860.
 *
 * This function set the StaNumDetect to RT2860. submit by 
 * \param[int]	 the StaNumDetect
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the StaNumDetect to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the StaNumDetect to RT2860.
 */

int wifimgr_lib_set_StaNumDetect(wifi_type type, int index, char *input);

/**
 * \brief This function set the ProbeHideSSID to RT2860.
 *
 * This function set the ProbeHideSSID to RT2860. submit by 
 * \param[int]	 the ProbeHideSSID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ProbeHideSSID to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ProbeHideSSID to RT2860.
 */

int wifimgr_lib_set_ProbeHideSSID(wifi_type type, int index, char *input);

/**
 * \brief This function set the AutoChannelSkipList to RT2860.
 *
 * This function set the AutoChannelSkipList to RT2860. submit by 
 * \param[int]	 the AutoChannelSkipList
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the AutoChannelSkipList to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the AutoChannelSkipList to RT2860.
 */

int wifimgr_lib_set_AutoChannelSkipList(wifi_type type, int index, char *input);

/**
 * \brief This function set the VHT_BW to RT2860.
 *
 * This function set the VHT_BW to RT2860. submit by 
 * \param[int]	 the VHT_BW
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the VHT_BW to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the VHT_BW to RT2860.
 */

int wifimgr_lib_set_VHT_BW(wifi_type type, int index, char *input);

/**
 * \brief This function set the VHT_SGI to RT2860.
 *
 * This function set the VHT_SGI to RT2860. submit by 
 * \param[int]	 the VHT_SGI
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the VHT_SGI to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the VHT_SGI to RT2860.
 */

int wifimgr_lib_set_VHT_SGI(wifi_type type, int index, char *input);

/**
 * \brief This function set the VHT_LDPC to RT2860.
 *
 * This function set the VHT_LDPC to RT2860. submit by 
 * \param[int]	 the VHT_LDPC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the VHT_LDPC to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the VHT_LDPC to RT2860.
 */

int wifimgr_lib_set_VHT_LDPC(wifi_type type, int index, char *input);

/**
 * \brief This function set the VHT_STBC to RT2860.
 *
 * This function set the VHT_STBC to RT2860. submit by 
 * \param[int]	 the VHT_STBC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the VHT_STBC to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the VHT_STBC to RT2860.
 */

int wifimgr_lib_set_VHT_STBC(wifi_type type, int index, char *input);

/**
 * \brief This function set the VHT_BW_SIGNAL to RT2860.
 *
 * This function set the VHT_BW_SIGNAL to RT2860. submit by 
 * \param[int]	 the VHT_BW_SIGNAL
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the VHT_BW_SIGNAL to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the VHT_BW_SIGNAL to RT2860.
 */

int wifimgr_lib_set_VHT_BW_SIGNAL(wifi_type type, int index, char *input);

/**
 * \brief This function get VHT_BW_SIGNAL.
 *
 * This function get VHT_BW_SIGNAL. submit by 
 * \param[out]	 the VHT_BW_SIGNAL
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get VHT_BW_SIGNAL.
 * \par Example
 * \snippet Api_lib_wifimgr.c get VHT_BW_SIGNAL
 */
 
int wifimgr_lib_get_VHT_BW_SIGNAL(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function set the HT_EXTCHA to RT2860.
 *
 * This function set the HT_EXTCHA to RT2860. submit by 
 * \param[int]	 the HT_EXTCHA
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_EXTCHA to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_EXTCHA to RT2860.
 */

int wifimgr_lib_set_HT_EXTCHA(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_OpMode to RT2860.
 *
 * This function set the HT_OpMode to RT2860. submit by 
 * \param[int]	 the HT_OpMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_OpMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_OpMode to RT2860.
 */

int wifimgr_lib_set_HT_OpMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the BSSCoexistence to RT2860.
 *
 * This function set the BSSCoexistence to RT2860. submit by 
 * \param[int]	 the BSSCoexistence
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the BSSCoexistence to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the BSSCoexistence to RT2860.
 */

int wifimgr_lib_set_HT_BSSCoexistence(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_GI to RT2860.
 *
 * This function set the HT_GI to RT2860. submit by 
 * \param[int]	 the HT_GI
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_GI to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_GI to RT2860.
 */

int wifimgr_lib_set_HT_GI(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_MCS to RT2860.
 *
 * This function set the HT_MCS to RT2860. submit by 
 * \param[int]	 the HT_MCS
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_MCS to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_MCS to RT2860.
 */

int wifimgr_lib_set_HT_MCS(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_AutoBA to RT2860.
 *
 * This function set the HT_AutoBA to RT2860. submit by 
 * \param[int]	 the HT_AutoBA
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_AutoBA to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_AutoBA to RT2860.
 */

int wifimgr_lib_set_HT_AutoBA(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_RDG to RT2860.
 *
 * This function set the HT_RDG to RT2860. submit by 
 * \param[int]	 the HT_RDG
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_RDG to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_RDG to RT2860.
 */

int wifimgr_lib_set_HT_RDG(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_MpduDensity to RT2860.
 *
 * This function set the HT_MpduDensity to RT2860. submit by 
 * \param[int]	 the HT_MpduDensity
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_MpduDensity to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_MpduDensity to RT2860.
 */

int wifimgr_lib_set_HT_MpduDensity(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_BAWinSize to RT2860.
 *
 * This function set the HT_BAWinSize to RT2860. submit by 
 * \param[int]	 the HT_BAWinSize
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_BAWinSize to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_BAWinSize to RT2860.
 */

int wifimgr_lib_set_HT_BAWinSize(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_HTC to RT2860.
 *
 * This function set the HT_HTC to RT2860. submit by 
 * \param[int]	 the HT_HTC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_HTC to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_HTC to RT2860.
 */

int wifimgr_lib_set_HT_HTC(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_LinkAdapt to RT2860.
 *
 * This function set the HT_LinkAdapt to RT2860. submit by 
 * \param[int]	 the HT_LinkAdapt
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_LinkAdapt to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_LinkAdapt to RT2860.
 */

int wifimgr_lib_set_HT_LinkAdapt(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_AMSDU to RT2860.
 *
 * This function set the HT_AMSDU to RT2860. submit by 
 * \param[int]	 the HT_AMSDU
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_AMSDU to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_AMSDU to RT2860.
 */

int wifimgr_lib_set_HT_AMSDU(wifi_type type, int index, char *input);

/**
 * \brief This function set the HT_TxStream to RT2860.
 *
 * This function set the HT_TxStream to RT2860. submit by 
 * \param[int]	 the HT_TxStream
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_TxStream to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_TxStream to RT2860.
 */

int wifimgr_lib_set_HT_TxStream(wifi_type type, int index, char *input);

/**
 * \brief This function get the HT_TxStream from RT2860.
 *
 * This function get the HT_TxStream from RT2860. submit by 
 * \param[out]	 the HT_TxStream
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_TxStream from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the HT_TxStream from RT2860.
 */
 
int wifimgr_lib_get_HT_TxStream(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function get the HT_RxStream from RT2860.
 *
 * This function get the HT_RxStream from RT2860. submit by 
 * \param[out]	 the HT_RxStream
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the HT_RxStream from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the HT_RxStream from RT2860.
 */
 
int wifimgr_lib_get_HT_RxStream(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function set the HT_RxStream to RT2860.
 *
 * This function set the HT_RxStream to RT2860. submit by 
 * \param[int]	 the HT_RxStream
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_RxStream to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_RxStream to RT2860.
 */

int wifimgr_lib_set_HT_RxStream(wifi_type type, int index, char *input);

/**
 * \brief This function set the ITxBfEn to RT2860.
 *
 * This function set the ITxBfEn to RT2860. submit by 
 * \param[int]	 the ITxBfEn
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ITxBfEn to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ITxBfEn to RT2860.
 */

int wifimgr_lib_set_ITxBfEn(wifi_type type, int index, char *input);

/**
 * \brief This function set the ETxBfEnCond to RT2860.
 *
 * This function set the ETxBfEnCond to RT2860. submit by 
 * \param[int]	 the ETxBfEnCond
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ETxBfEnCond to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ETxBfEnCond to RT2860.
 */

int wifimgr_lib_set_ETxBfEnCond(wifi_type type, int index, char *input);

/**
 * \brief This function set the ETxBfIncapable to RT2860.
 *
 * This function set the ETxBfIncapable to RT2860. submit by 
 * \param[int]	 the ETxBfIncapable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ETxBfIncapable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ETxBfIncapable to RT2860.
 */

int wifimgr_lib_set_ETxBfIncapable(wifi_type type, int index, char *input);

/**
 * \brief This function set the MUTxRxEnable to RT2860.
 *
 * This function set the MUTxRxEnable to RT2860. submit by 
 * \param[int]	 the MUTxRxEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MUTxRxEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MUTxRxEnable to RT2860.
 */

int wifimgr_lib_set_MUTxRxEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the raloadFlag to RT2860.
 *
 * This function set the raloadFlag to RT2860. submit by 
 * \param[int]	 the raloadFlag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the raloadFlag to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the raloadFlag to RT2860.
 */

int wifimgr_lib_set_reloadFlag(wifi_type type, char *str);

/**
 * \brief This function set the bssidNum to RT2860.
 *
 * This function set the bssidNum to RT2860. submit by 
 * \param[int]	 the bssidNum
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the bssidNum to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the bssidNum to RT2860.
 */

int wifimgr_lib_set_bssidNum(wifi_type type, int index, char *input);

/**
 * \brief This function set the channel to RT2860.
 *
 * This function set the channel to RT2860. submit by 
 * \param[int]	 the channel
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the channel to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the channel to RT2860.
 */

int wifimgr_lib_set_channel(wifi_type type, int index, char *input);

/**
 * \brief This function set the autoChannelSelect to RT2860.
 *
 * This function set the autoChannelSelect to RT2860. submit by 
 * \param[int]	 the autoChannelSelect
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the autoChannelSelect to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the autoChannelSelect to RT2860.
 */

int wifimgr_lib_set_autoChannelSelect(wifi_type type, int index, char *input);
#ifdef TCSUPPORT_WLAN_ACS

/**
 * \brief This function set the autoChannelSel.
 *
 * This function set the autoChannelSel. submit by 
 * \param[int]	 the autoChannelSel
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the autoChannelSel.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the autoChannelSel.
 */

int wifimgr_lib_set_autoChannelSel(wifi_type type, int index, char *input);
#endif

/**
 * \brief This function set the authMode to RT2860.
 *
 * This function set the authMode to RT2860. submit by 
 * \param[int]	 the authMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the authMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the authMode to RT2860.
 */

int wifimgr_lib_set_authMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the txRate to RT2860.
 *
 * This function set the txRate to RT2860. submit by 
 * \param[int]	 the txRate
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the txRate to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the txRate to RT2860.
 */

int wifimgr_lib_set_txRate(wifi_type type, int index, char *input);

/**
 * \brief This function set the noForwarding to RT2860.
 *
 * This function set the noForwarding to RT2860. submit by 
 * \param[int]	 the noForwarding
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the noForwarding to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the noForwarding to RT2860.
 */

int wifimgr_lib_set_noForwarding(wifi_type type, int index, char *input);

/**
 * \brief This function set the PMFMFPC to RT2860.
 *
 * This function set the PMFMFPC to RT2860. submit by 
 * \param[int]	 the PMFMFPC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the PMFMFPC to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the PMFMFPC to RT2860.
 */

int wifimgr_lib_set_PMFMFPC(wifi_type type, int index, char *input);

/**
 * \brief This function set the PMFMFPR to RT2860.
 *
 * This function set the PMFMFPR to RT2860. submit by 
 * \param[int]	 the PMFMFPR
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the PMFMFPR to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the PMFMFPR to RT2860.
 */

int wifimgr_lib_set_PMFMFPR(wifi_type type, int index, char *input);

/**
 * \brief This function set the PMFSHA256 to RT2860.
 *
 * This function set the PMFSHA256 to RT2860. submit by 
 * \param[int]	 the PMFSHA256
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the PMFSHA256 to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the PMFSHA256 to RT2860.
 */

int wifimgr_lib_set_PMFSHA256(wifi_type type, int index, char *input);

/**
 * \brief This function set the defaultKeyID to RT2860.
 *
 * This function set the defaultKeyID to RT2860. submit by 
 * \param[int]	 the defaultKeyID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the defaultKeyID to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the defaultKeyID to RT2860.
 */

int wifimgr_lib_set_defaultKeyID(wifi_type type, int index, char *input);

/**
 * \brief This function set the WPAPSK to RT2860.
 *
 * This function set the WPAPSK to RT2860. submit by 
 * \param[int]	 the WPAPSK
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the WPAPSK to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the WPAPSK to RT2860.
 */

int wifimgr_lib_set_WPAPSK(wifi_type type, int index, char *input);

/**
 * \brief This function set the maxStaNum to RT2860.
 *
 * This function set the maxStaNum to RT2860. submit by 
 * \param[int]	 the maxStaNum
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the maxStaNum to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the maxStaNum to RT2860.
 */

int wifimgr_lib_set_maxStaNum(wifi_type type, int index, char *input);

 /**
 * \brief This function set the mbssMaxStaNum to RT2860.
 *
 * This function set the mbssMaxStaNum to RT2860. submit by 
 * \param[int]	 the mbssMaxStaNum
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the mbssMaxStaNum to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the mbssMaxStaNum to RT2860.
 */
 
int wifimgr_lib_set_mbssMaxStaNum(wifi_type type, int index, char *input);

/**
 * \brief This function set the keyType to RT2860.
 *
 * This function set the keyType to RT2860. submit by 
 * \param[int]	 the keyType
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the keyType to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the keyType to RT2860.
 */

int wifimgr_lib_set_keyType(wifi_type type, int index, int num, char *input);

/**
 * \brief This function set the key1Type to RT2860.
 *
 * This function set the key1Type to RT2860. submit by 
 * \param[int]	 the key1Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key1Type to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key1Type to RT2860.
 */

int wifimgr_lib_set_key1Type(wifi_type type, int index,  char *input);

/**
 * \brief This function set the key2Type to RT2860.
 *
 * This function set the key2Type to RT2860. submit by 
 * \param[int]	 the key2Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key2Type to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key2Type to RT2860.
 */

int wifimgr_lib_set_key2Type(wifi_type type, int index, char *input);

/**
 * \brief This function set the key3Type to RT2860.
 *
 * This function set the key3Type to RT2860. submit by 
 * \param[int]	 the key3Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key3Type to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key3Type to RT2860.
 */

int wifimgr_lib_set_key3Type(wifi_type type, int index, char *input);

/**
 * \brief This function set the key4Type to RT2860.
 *
 * This function set the key4Type to RT2860. submit by 
 * \param[int]	 the key4Type
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key4Type to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key4Type to RT2860.
 */

int wifimgr_lib_set_key4Type(wifi_type type, int index, char *input);

/**
 * \brief This function set the keyStr to RT2860.
 *
 * This function set the keyStr to RT2860. submit by 
 * \param[int]	 the keyStr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the keyStr to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the keyStr to RT2860.
 */

int wifimgr_lib_set_keyStr(wifi_type type, int index, int num, char *input);

/**
 * \brief This function set the key1Str to RT2860.
 *
 * This function set the key1Str to RT2860. submit by 
 * \param[int]	 the key1Str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key1Str to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key1Str to RT2860.
 */

int wifimgr_lib_set_key1Str(wifi_type type, int index, char *input); 

/**
 * \brief This function set the key2Str to RT2860.
 *
 * This function set the key2Str to RT2860. submit by 
 * \param[int]	 the key2Str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key2Str to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key2Str to RT2860.
 */

int wifimgr_lib_set_key2Str(wifi_type type, int index, char *input); 

/**
 * \brief This function set the key3Str to RT2860.
 *
 * This function set the key3Str to RT2860. submit by 
 * \param[int]	 the key3Str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key3Str to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key3Str to RT2860.
 */

int wifimgr_lib_set_key3Str(wifi_type type, int index, char *input); 

/**
 * \brief This function set the key4Str to RT2860.
 *
 * This function set the key4Str to RT2860. submit by 
 * \param[int]	 the key4Str
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the key4Str to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the key4Str to RT2860.
 */

int wifimgr_lib_set_key4Str(wifi_type type, int index, char *input); 

/**
 * \brief This function set the encrypType to RT2860.
 *
 * This function set the encrypType to RT2860. submit by 
 * \param[int]	 the encrypType
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the encrypType to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the encrypType to RT2860.
 */

int wifimgr_lib_set_encrypType(wifi_type type, int index, char *input);

/**
 * \brief This function set the preAuth to RT2860.
 *
 * This function set the preAuth to RT2860. submit by 
 * \param[int]	 the preAuth
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the preAuth to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the preAuth to RT2860.
 */

int wifimgr_lib_set_preAuth(wifi_type type, int index, char *input);

/**
 * \brief This function set the hideSSID to RT2860.
 *
 * This function set the hideSSID to RT2860. submit by 
 * \param[int]	 the hideSSID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the hideSSID to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the hideSSID to RT2860.
 */

int wifimgr_lib_set_hideSSID(wifi_type type, int index, char *input);

/**
 * \brief This function set the ftSupport to RT2860.
 *
 * This function set the ftSupport to RT2860. submit by 
 * \param[int]	 the ftSupport
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ftSupport to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ftSupport to RT2860.
 */

int wifimgr_lib_set_ftSupport(wifi_type type, int index, char *input);

/**
 * \brief This function set the ftotd to RT2860.
 *
 * This function set the ftotd to RT2860. submit by 
 * \param[int]	 the ftotd
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ftotd to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ftotd to RT2860.
 */
int wifimgr_lib_set_ftotd(wifi_type type, int index, char *input);
/**
 * \brief This function set the ApCliMac Manual Flag to RT2860.
 *
 * This function set the ApCliMac Manual Flag to RT2860. submit by 
 * \param[int]	 the ApCliMac Manual Flag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ApCliMac Manual Flag to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ApCliMac Manual Flag to RT2860.
 */

int wifimgr_lib_set_ApCliMac_Manual_Flag(wifi_type type, int index, char *input);

/**
 * \brief This function set the ApCliMac Manual to RT2860.
 *
 * This function set the ApCliMac Manual to RT2860. submit by 
 * \param[int]	 the ApCliMac Manual
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ApCliMac Manual to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ApCliMac Manual to RT2860.
 */

int wifimgr_lib_set_ApCliMac_Manual(wifi_type type, int index, char *input);

/**
 * \brief This function set the rrmEnable to RT2860.
 *
 * This function set the rrmEnable to RT2860. submit by 
 * \param[int]	 the rrmEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the rrmEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the rrmEnable to RT2860.
 */

int wifimgr_lib_set_rrmEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the wnmEnable to RT2860.
 *
 * This function set the wnmEnable to RT2860. submit by 
 * \param[int]	 the wnmEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wnmEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wnmEnable to RT2860.
 */
int wifimgr_lib_set_wnmEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the wmmCapable to RT2860.
 *
 * This function set the wmmCapable to RT2860. submit by 
 * \param[int]	 the wmmCapable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wmmCapable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wmmCapable to RT2860.
 */

int wifimgr_lib_set_wmmCapable(wifi_type type, int index, char *input);

/**
 * \brief This function set the fixedTxMode to RT2860.
 *
 * This function set the fixedTxMode to RT2860. submit by 
 * \param[int]	 the fixedTxMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the fixedTxMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the fixedTxMode to RT2860.
 */

int wifimgr_lib_set_fixedTxMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the apsdCapable to RT2860.
 *
 * This function set the apsdCapable to RT2860. submit by 
 * \param[int]	 the apsdCapable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the apsdCapable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the apsdCapable to RT2860.
 */

int wifimgr_lib_set_apsdCapable(wifi_type type, int index, char *input);

/**
 * \brief This function set the dlsCapable to RT2860.
 *
 * This function set the dlsCapable to RT2860. submit by 
 * \param[int]	 the dlsCapable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the dlsCapable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the dlsCapable to RT2860.
 */

int wifimgr_lib_set_dlsCapable(wifi_type type, int index, char *input);

/**
 * \brief This function set the apAifsn to RT2860.
 *
 * This function set the apAifsn to RT2860. submit by 
 * \param[int]	 the apAifsn
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the apAifsn to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the apAifsn to RT2860.
 */

int wifimgr_lib_set_apAifsn(wifi_type type, int index, char *input);

/**
 * \brief This function set the apCwmin to RT2860.
 *
 * This function set the apCwmin to RT2860. submit by 
 * \param[int]	 the apCwmin
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the apCwmin to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the apCwmin to RT2860.
 */

int wifimgr_lib_set_apCwmin(wifi_type type, int index, char *input);

/**
 * \brief This function set the apCwmax to RT2860.
 *
 * This function set the apCwmax to RT2860. submit by 
 * \param[int]	 the apCwmax
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the apCwmax to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the apCwmax to RT2860.
 */

int wifimgr_lib_set_apCwmax(wifi_type type, int index, char *input);

/**
 * \brief This function set the apTxop to RT2860.
 *
 * This function set the apTxop to RT2860. submit by 
 * \param[int]	 the apTxop
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the apTxop to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the apTxop to RT2860.
 */

int wifimgr_lib_set_apTxop(wifi_type type, int index, char *input);

/**
 * \brief This function set the apACM to RT2860.
 *
 * This function set the apACM to RT2860. submit by 
 * \param[int]	 the apACM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the apACM to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the apACM to RT2860.
 */

int wifimgr_lib_set_apACM(wifi_type type, int index, char *input);

/**
 * \brief This function set the bssAifsn to RT2860.
 *
 * This function set the bssAifsn to RT2860. submit by 
 * \param[int]	 the bssAifsn
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the bssAifsn to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the bssAifsn to RT2860.
 */

int wifimgr_lib_set_bssAifsn(wifi_type type, int index, char *input);

/**
 * \brief This function set the bssCwmin to RT2860.
 *
 * This function set the bssCwmin to RT2860. submit by 
 * \param[int]	 the bssCwmin
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the bssCwmin to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the bssCwmin to RT2860.
 */

int wifimgr_lib_set_bssCwmin(wifi_type type, int index, char *input);

/**
 * \brief This function set the bssCwmax to RT2860.
 *
 * This function set the bssCwmax to RT2860. submit by 
 * \param[int]	 the bssCwmax
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the bssCwmax to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the bssCwmax to RT2860.
 */

int wifimgr_lib_set_bssCwmax(wifi_type type, int index, char *input);

/**
 * \brief This function set the bssTxop to RT2860.
 *
 * This function set the bssTxop to RT2860. submit by 
 * \param[int]	 the bssTxop
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the bssTxop to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the bssTxop to RT2860.
 */

int wifimgr_lib_set_bssTxop(wifi_type type, int index, char *input);

/**
 * \brief This function set the bssACM to RT2860.
 *
 * This function set the bssACM to RT2860. submit by 
 * \param[int]	 the bssACM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the bssACM to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the bssACM to RT2860.
 */

int wifimgr_lib_set_bssACM(wifi_type type, int index, char *input);

/**
 * \brief This function set the ackPolicy to RT2860.
 *
 * This function set the ackPolicy to RT2860. submit by 
 * \param[int]	 the ackPolicy
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ackPolicy to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ackPolicy to RT2860.
 */

int wifimgr_lib_set_ackPolicy(wifi_type type, int index, char *input);

/**
 * \brief This function set the wscGetConf to RT2860.
 *
 * This function set the wscGetConf to RT2860. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscGetConf to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscGetConf to RT2860.
 */

int wifimgr_lib_set_wscGetConf(wifi_type type, int index);

/**
 * \brief This function set the wscStop to RT2860.
 *
 * This function set the wscStop to RT2860. submit by 
 * \param[int]	 the wscStop
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscStop to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscStop to RT2860.
 */

int wifimgr_lib_set_wscStop(wifi_type type, int index);

/**
 * \brief This function set the wscConfMode to RT2860.
 *
 * This function set the wscConfMode to RT2860. submit by 
 * \param[int]	 the wscConfMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscConfMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscConfMode to RT2860.
 */

int wifimgr_lib_set_wscConfMode(wifi_type type, int index, char* input);

/**
 * \brief This function set the wscConfStatus to RT2860.
 *
 * This function set the wscConfStatus to RT2860. submit by 
 * \param[int]	 the wscConfStatus
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscConfStatus to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscConfStatus to RT2860.
 */

int wifimgr_lib_set_wscConfStatus(wifi_type type, int index, char* input);

/**
 * \brief This function set the wscV2Support to RT2860.
 *
 * This function set the wscV2Support to RT2860. submit by 
 * \param[int]	 the wscV2Support
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscV2Support to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscV2Support to RT2860.
 */

int wifimgr_lib_set_wscV2Support(wifi_type type, int index, char* input);

/**
 * \brief This function set the wscMaxPinAttack to RT2860.
 *
 * This function set the wscMaxPinAttack to RT2860. submit by 
 * \param[int]	 the wscMaxPinAttack
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscMaxPinAttack to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscMaxPinAttack to RT2860.
 */

int wifimgr_lib_set_wscMaxPinAttack(wifi_type type, int index, char *input);

/**
 * \brief This function set the wscSetupLockTime to RT2860.
 *
 * This function set the wscSetupLockTime to RT2860. submit by 
 * \param[int]	 the wscSetupLockTime
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscSetupLockTime to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscSetupLockTime to RT2860.
 */

int wifimgr_lib_set_wscSetupLockTime(wifi_type type, int index, char *input);

/**
 * \brief This function set the wscMode to RT2860.
 *
 * This function set the wscMode to RT2860. submit by 
 * \param[int]	 the wscMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscMode to RT2860.
 */

int wifimgr_lib_set_wscMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the wscPinCode to RT2860.
 *
 * This function set the wscPinCode to RT2860. submit by 
 * \param[int]	 the wscPinCode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscPinCode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscPinCode to RT2860.
 */

int wifimgr_lib_set_wscPinCode(wifi_type type, int index, char *input);

/**
 * \brief This function set the wscGenPinCode to RT2860.
 *
 * This function set the wscGenPinCode to RT2860. submit by 
 * \param[int]	 the wscGenPinCode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscGenPinCode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscGenPinCode to RT2860.
 */

int wifimgr_lib_set_wscGenPinCode(wifi_type type, int index, char *input);

/**
 * \brief This function set the wscKeyASCII to RT2860.
 *
 * This function set the wscKeyASCII to RT2860. submit by 
 * \param[int]	 the wscKeyASCII
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscKeyASCII to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscKeyASCII to RT2860.
 */

int wifimgr_lib_set_wscKeyASCII(wifi_type type, int index, char *input);

/**
 * \brief This function set the WscUUIDE to RT2860.
 *
 * This function set the WscUUIDE to RT2860. submit by 
 * \param[int]	 the WscUUIDE
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the WscUUIDE to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the WscUUIDE to RT2860.
 */
int wifimgr_lib_set_WscUUIDE(wifi_type type, int index, char *input);

/**
 * \brief This function set the accessPolicy to RT2860.
 *
 * This function set the accessPolicy to RT2860. submit by 
 * \param[int]	 the accessPolicy
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the accessPolicy to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the accessPolicy to RT2860.
 */

int wifimgr_lib_set_accessPolicy(wifi_type type, int index, char* input);

/**
 * \brief This function set the accessControlList to RT2860.
 *
 * This function set the accessControlList to RT2860. submit by 
 * \param[int]	 the accessControlList
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the accessControlList to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the accessControlList to RT2860.
 */

int wifimgr_lib_set_accessControlList(wifi_type type, int index, char* input);

/**
 * \brief This function set the wscDefaultSSID to RT2860.
 *
 * This function set the wscDefaultSSID to RT2860. submit by 
 * \param[int]	 the wscDefaultSSID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wscDefaultSSID to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wscDefaultSSID to RT2860.
 */

int wifimgr_lib_set_wscDefaultSSID(wifi_type type, int index, char* input);

/**
 * \brief This function set the WEPAuthType to RT2860.
 *
 * This function set the WEPAuthType to RT2860. submit by 
 * \param[int]	 the WEPAuthType
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the WEPAuthType to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the WEPAuthType to RT2860.
 */

int wifimgr_lib_set_wepAuthType(wifi_type type, int index, char* input);

/**
 * \brief This function set the EfuseBufferMode to RT2860.
 *
 * This function set the EfuseBufferMode to RT2860. submit by 
 * \param[int]	 the EfuseBufferMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the EfuseBufferMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the EfuseBufferMode to RT2860.
 */

int wifimgr_lib_set_EfuseBufferMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the igmpSnEnable to RT2860.
 *
 * This function set the igmpSnEnable to RT2860. submit by 
 * \param[int]	 the igmpSnEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the igmpSnEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the igmpSnEnable to RT2860.
 */

int wifimgr_lib_set_igmpSnEnable(wifi_type type, int index, char *input);
#if defined(TCSUPPORT_ECNT_MAP)

/**
 * \brief This function set the MapEnable to RT2860.
 *
 * This function set the MapEnable to RT2860. submit by 
 * \param[int]	 the MapEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MapEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MapEnable to RT2860.
 */

int wifimgr_lib_set_MapEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the MAP_Turnkey to RT2860.
 *
 * This function set the MAP_Turnkey to RT2860. submit by 
 * \param[int]	 the MAP_Turnkey
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MAP_Turnkey to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MAP_Turnkey to RT2860.
 */

int wifimgr_lib_set_MAP_Turnkey(wifi_type type, int index, char *input);

/**
 * \brief This function set the MAP_Ext to RT2860.
 *
 * This function set the MAP_Ext to RT2860. submit by 
 * \param[int]	 the MAP_Ext
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MAP_Ext to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MAP_Ext to RT2860.
 */

int wifimgr_lib_set_MAP_Ext(wifi_type type, int index, char *input);
#endif
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))

/**
 * \brief This function restart interface.
 *
 * This function restart interface. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  restart interface.
 * \par Example
 * \snippet Api_lib_wifimgr.c restart interface.
 */

int wifimgr_lib_restartInterface(WLan_info wlan_info,int nodeid, int resetFlag);
#else

/**
 * \brief This function restart interface.
 *
 * This function restart interface. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  restart interface.
 * \par Example
 * \snippet Api_lib_wifimgr.c restart interface.
 */

int wifimgr_lib_restartInterface(WLan_info wlan_info);
#endif

/**
 * \brief This function commit.
 *
 * This function commit. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  commit.
 * \par Example
 * \snippet Api_lib_wifimgr.c commit.
 */

int wifimgr_lib_commit(wifi_type type,int index);

/**
 * \brief This function save data to file.
 *
 * This function save data to file. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  save data to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c save data to file.
 */

int wifimgr_lib_save_dataFile(wifi_type type);
#if defined(TCSUPPORT_WLAN_APCLIENT)

/**
 * \brief This function get apcli status.
 *
 * This function get apcli status. submit by 
 * \param[out]	 the apcli status
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get apcli status.
 * \par Example
 * \snippet Api_lib_wifimgr.c get apcli status.
 */

int wifimgr_lib_get_apcli_status(wifi_type type);

/**
 * \brief This function set the SiteSurvery to RT2860.
 *
 * This function set the SiteSurvery to RT2860. submit by 
 * \param[int]	 the SiteSurvery
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the SiteSurvery to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the SiteSurvery to RT2860.
 */

int wifimgr_lib_set_SiteSurvery(wifi_type type, char *SSID);

/**
 * \brief This function get the SiteSurvery from RT2860.
 *
 * This function get the SiteSurvery from RT2860. submit by 
 * \param[out]	 the SiteSurvery
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the SiteSurvery from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the SiteSurvery from RT2860.
 */

int wifimgr_lib_get_SiteSurvery(wifi_type type, char *SSID, char *data, int len);

/**
 * \brief This function get the Statistics from RT2860.
 *
 * This function get the Statistics from RT2860. submit by 
 * \param[out]	 the Statistics
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the Statistics from RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the Statistics from RT2860.
 */

int wifimgr_lib_get_Statistics(wifi_type type, char *data, int len);
#endif

/**
 * \brief This function set the HT_LDPC to RT2860.
 *
 * This function set the HT_LDPC to RT2860. submit by 
 * \param[int]	 the HT_LDPC
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HT_LDPC to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HT_LDPC to RT2860.
 */

int wifimgr_lib_set_HT_LDPC(wifi_type type, int index, char *input);

/**
 * \brief This function set the AMSDU_NUM to RT2860.
 *
 * This function set the AMSDU_NUM to RT2860. submit by 
 * \param[int]	 the AMSDU_NUM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the AMSDU_NUM to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the AMSDU_NUM to RT2860.
 */

int wifimgr_lib_set_AMSDU_NUM(wifi_type type, int index, char *input);

/**
 * \brief This function set the MuOfdmaDlEnable to RT2860.
 *
 * This function set the MuOfdmaDlEnable to RT2860. submit by 
 * \param[int]	 the MuOfdmaDlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MuOfdmaDlEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MuOfdmaDlEnable to RT2860.
 */

int wifimgr_lib_set_MuOfdmaDlEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the MuOfdmaUlEnable to RT2860.
 *
 * This function set the MuOfdmaUlEnable to RT2860. submit by 
 * \param[int]	 the MuOfdmaUlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MuOfdmaUlEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MuOfdmaUlEnable to RT2860.
 */

int wifimgr_lib_set_MuOfdmaUlEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the MuMimoDlEnable to RT2860.
 *
 * This function set the MuMimoDlEnable to RT2860. submit by 
 * \param[int]	 the MuMimoDlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MuMimoDlEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MuMimoDlEnable to RT2860.
 */

int wifimgr_lib_set_MuMimoDlEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the MuMimoUlEnable to RT2860.
 *
 * This function set the MuMimoUlEnable to RT2860. submit by 
 * \param[int]	 the MuMimoUlEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MuMimoUlEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MuMimoUlEnable to RT2860.
 */

int wifimgr_lib_set_MuMimoUlEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the SREnable to RT2860.
 *
 * This function set the SREnable to RT2860. submit by 
 * \param[int]	 the SREnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the SREnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the SREnable to RT2860.
 */

int wifimgr_lib_set_SREnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the PPEnable to RT2860.
 *
 * This function set the PPEnable to RT2860. submit by 
 * \param[int]	 the PPEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the PPEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the PPEnable to RT2860.
 */

int wifimgr_lib_set_PPEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the TWTSupport to RT2860.
 *
 * This function set the TWTSupport to RT2860. submit by 
 * \param[int]	 the TWTSupport
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the TWTSupport to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the TWTSupport to RT2860.
 */

int wifimgr_lib_set_TWTSupport(wifi_type type, int index, char *input);

/**
 * \brief This function set the SRMode to RT2860.
 *
 * This function set the SRMode to RT2860. submit by 
 * \param[int]	 the SRMode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the SRMode to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the SRMode to RT2860.
 */

int wifimgr_lib_set_SRMode(wifi_type type, int index, char *input);

/**
 * \brief This function set the SRSDEnable to RT2860.
 *
 * This function set the SRSDEnable to RT2860. submit by 
 * \param[int]	 the SRSDEnable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the SRSDEnable to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the SRSDEnable to RT2860.
 */

int wifimgr_lib_set_SRSDEnable(wifi_type type, int index, char *input);

/**
 * \brief This function set the HeraStbcPriority to RT2860.
 *
 * This function set the HeraStbcPriority to RT2860. submit by 
 * \param[int]	 the HeraStbcPriority
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the HeraStbcPriority to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the HeraStbcPriority to RT2860.
 */

int wifimgr_lib_set_HeraStbcPriority(wifi_type type, int index, char *input);

/**
 * \brief This function set the MaxRuOfdma to RT2860.
 *
 * This function set the MaxRuOfdma to RT2860. submit by 
 * \param[int]	 the MaxRuOfdma
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MaxRuOfdma to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MaxRuOfdma to RT2860.
 */

int wifimgr_lib_set_MaxRuOfdma(wifi_type type, int index, char *input);


/**
 * \brief This function set the MaxDLMuMimo to RT2860.
 *
 * This function set the MaxDLMuMimo to RT2860. submit by 
 * \param[int]	 the MaxDLMuMimo
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MaxDLMuMimo to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MaxDLMuMimo to RT2860.
 */

int wifimgr_lib_set_MaxDLMuMimo(wifi_type type, int index, char *input);

/**
 * \brief This function set the MaxULMuMimo to RT2860.
 *
 * This function set the MaxULMuMimo to RT2860. submit by 
 * \param[int]	 the MaxULMuMimo
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MaxULMuMimo to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MaxULMuMimo to RT2860.
 */

int wifimgr_lib_set_MaxULMuMimo(wifi_type type, int index, char *input);

/**
 * \brief This function set the Disable160RuMu to RT2860.
 *
 * This function set the Disable160RuMu to RT2860. submit by 
 * \param[int]	 the Disable160RuMu
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the Disable160RuMu to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the Disable160RuMu to RT2860.
 */

int wifimgr_lib_set_Disable160RuMu(wifi_type type, int index, char *input);

/**
 * \brief This function set the Vht1024QamSupport to RT2860.
 *
 * This function set the Vht1024QamSupport to RT2860. submit by 
 * \param[int]	 the Vht1024QamSupport
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the Vht1024QamSupport to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the Vht1024QamSupport to RT2860.
 */

int wifimgr_lib_set_Vht1024QamSupport(wifi_type type, int index, char *input);

/*int wifimgr_lib_exit(void);*/


/**
 * \brief This function get the mac entry by index.
 *
 * This function get the mac entry by index. submit by 
 * \param[out]	 the mac
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the mac entry by index.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the mac entry by index.
 */
void getMacEntryByIndex(mac_table_t *pMacTable, int index,int maxnum);

/**
 * \brief This function to reload wlan driver.
 *
 * This function to reload wlan driver. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  reload wlan driver.
 * \par Example
 * \snippet Api_lib_wifimgr.c  Reload wlan driver.
 */

void reload_WlanAC_Driver(void);

/**
 * \brief This function get value of ap count.
 *
 * This function get value of ap count. submit by 
 * \note
 *  get value of ap count.
 * \par Example
 * \snippet Api_lib_wifimgr.c get value of ap count.
 */

int getAPCount(int rfband, RT_AP_COUNT *apcnt_out);
 
 /**
 * \brief This function get test_itf_status.
 *
 * This function get test_itf_status. submit by 
 * \note
 *  get test_itf_status.
 * \par Example
 * \snippet Api_lib_wifimgr.c get test_itf_status.
 */
 
int wifimgr_lib_get_test_itf_status(const char *_iface, short if_flag) ;

/**
 * \brief This function open or close 5G interface.
 *
 * This function open or close 5G interface. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Open or close 5G interface.
 * \par Example
 * \snippet Api_lib_wifimgr.c Open or close 5G interface.
 */

int wifimgr_lib_set_5G_PCIE(int flag);


#define WLAN_AC_SCRIPT_PREFIX 	"/userfs/bin/iwpriv rai%d set %s=%s\n"
#define WLAN_SCRIPT_PREFIX 	 	"/userfs/bin/iwpriv ra%d set %s=%s\n"
#define SVC_WLAN_ANT_PATH			"/etc/Wireless/RT2860AP/WLAN_ant.sh"

#define ATTR_NAME_LENGTH 			64

#define WIFI_INTERFACE_BSSIDNUM_CHANGE 			1
#define WIFI_INTERFACE_DOWN						2
#define WIFI_INTERFACE_UP 						3
#define WIFI_INTERFACE_INIT						4

typedef struct 
{
	char attr_name[ATTR_NAME_LENGTH];
	int (*wifi_api)(wifi_type, int, char*);
}wifi_attr_fun;

/**
 * \brief This function get the version .
 *
 * This function get the version . submit by 
 * \param[out]	 the version
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the version.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the version .
 */

int wifimgr_lib_get_version(int type, char *version, int len);

/**
 * \brief This function set the TxBurst to file.
 *
 * This function set the TxBurst to file. submit by 
 * \param[int]	 the TxBurst
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the TxBurst to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the TxBurst to file.
 */

int wifimgr_lib_set_TxBurst_to_file(char *buf, int len, int index, char *value, int flag);

/**
 * \brief This function set the MaxStaNum to file.
 *
 * This function set the MaxStaNum to file. submit by 
 * \param[int]	 the MaxStaNum
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the MaxStaNum to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the MaxStaNum to file.
 */

int wifimgr_lib_set_MaxStaNum(wifi_type type, int index, char *input);

/**
 * \brief This function get the ixia5gflag .
 *
 * This function get the ixia5gflag . submit by 
 * \param[out]	 the ixia5gflag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the ixia5gflag.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the ixia5gflag .
 */

int wifimgr_lib_get_ixia5gflag();

/**
 * \brief This function set the ixia5gflag.
 *
 * This function set the ixia5gflag. submit by 
 * \param[int]	 the ixia5gflag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the ixia5gflag to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the ixia5gflag to file.
 */

int wifimgr_lib_set_ixia5gflag(int val);

/**
 * \brief This function do check ixia5gflag.
 *
 * This function do check ixia5gflag. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  do check ixia5gflag.
 * \par Example
 * \snippet Api_lib_wifimgr.c do check ixia5gflag.
 */

int wifimgr_lib_check_wifi_ixia_test();

/**
 * \brief This function do reboot.
 *
 * This function do reboot. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  do reboot.
 * \par Example
 * \snippet Api_lib_wifimgr.c do reboot.
 */

int wifimgr_lib_reboot(int type, char *oldNum, int newNum, int wdsNum, char *bssidNum, char *changeName);

/**
 * \brief This function get the wlan_script_attr .
 *
 * This function get the wlan_script_attr . submit by 
 * \param[out]	 the wlan_script_attr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wlan_script_attr.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wlan_script_attr .
 */

int wifimgr_lib_get_wlan_script_attr(char output[][20]);

/**
 * \brief This function get the wlan_attr .
 *
 * This function get the wlan_attr . submit by 
 * \param[out]	 the wlan_attr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wlan_attr.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wlan_attr .
 */

int wifimgr_lib_get_wlan_attr(int type, wifi_attr_fun *output);

/**
 * \brief This function get the 11AC_CMDflag .
 *
 * This function get the 11AC_CMDflag . submit by 
 * \param[out]	 the 11AC_CMDflag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the 11AC_CMDflag.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the 11AC_CMDflag .
 */

int wifimgr_lib_get_11AC_CMDflag();

/**
 * \brief This function set the 11AC_CMDflag.
 *
 * This function set the 11AC_CMDflag. submit by 
 * \param[int]	 the 11AC_CMDflag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the 11AC_CMDflag to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the 11AC_CMDflag to file.
 */

int wifimgr_lib_set_11AC_CMDflag(int val);

/**
 * \brief This function get the 11N_CMDflag .
 *
 * This function get the 11N_CMDflag . submit by 
 * \param[out]	 the 11N_CMDflag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the 11N_CMDflag.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the 11N_CMDflag .
 */

int wifimgr_lib_get_11N_CMDflag();

/**
 * \brief This function set the 11N_CMDflag.
 *
 * This function set the 11N_CMDflag. submit by 
 * \param[int]	 the 11N_CMDflag
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the 11N_CMDflag to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the 11N_CMDflag to file.
 */

int wifimgr_lib_set_11N_CMDflag(int val);

/**
 * \brief This function do execute wlan script.
 *
 * This function do execute wlan script. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  do execute wlan script.
 * \par Example
 * \snippet Api_lib_wifimgr.c do execute wlan script.
 */

int wifimgr_lib_execute_wlan_script(int type, char *filename);

/**
 * \brief This function do restart wlan interface.
 *
 * This function do restart wlan interface. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  do restart wlan interface.
 * \par Example
 * \snippet Api_lib_wifimgr.c do restart wlan interface.
 */

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
int wifimgr_lib_restart_wlan_Interface(WLan_info wlan_info, WLan_info wlan_info_other, char *flag, int type, int nodeid, int resetFlag);
#else
int wifimgr_lib_restart_wlan_Interface(WLan_info wlan_info, WLan_info wlan_info_other, char *flag, int type);
#endif

/**
 * \brief This function use global mac.
 *
 * This function use global mac. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   use global mac.
 * \par Example
 * \snippet Api_lib_wifimgr.c  use global mac.
 */

int wifimgr_lib_use_global_mac(unsigned char *mac_addr);

/**
 * \brief This function set the WifiTxQueue.
 *
 * This function set the WifiTxQueue. submit by 
 * \param[int]	 the WifiTxQueue
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the WifiTxQueue to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the WifiTxQueue to file.
 */

int wifimgr_lib_set_WifiTxQueue(int BssidNum,int len);

/**
 * \brief This function get the button_state .
 *
 * This function get the button_state . submit by 
 * \param[out]	 the button_state
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the button_state.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the button_state .
 */

int wifimgr_lib_get_button_state(int type, char *file, char *output, int len);

/**
 * \brief This function do restart wlan interface.
 *
 * This function do restart wlan interface. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  do restart wlan interface.
 * \par Example
 * \snippet Api_lib_wifimgr.c do restart wlan interface.
 */

#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
int wifimgr_lib_do_restartInterface(WLan_info wlan_info, int nodeid, int resetFlag);
#else
int wifimgr_lib_do_restartInterface(WLan_info wlan_info);
#endif

/**
 * \brief This function set the iswps5gstart.
 *
 * This function set the iswps5gstart. submit by 
 * \param[int]	 the iswps5gstart
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the iswps5gstart to file.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the iswps5gstart.
 */

int wifimgr_lib_set_iswps5gstart_val(int *output, int val);

/**
 * \brief This function set the AP2040Rescan.
 *
 * This function set the AP2040Rescan. submit by 
 * \param[int]	 the AP2040Rescan
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the AP2040Rescan .
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the AP2040Rescan.
 */

int wifimgr_lib_set_AP2040Rescan(int wlan_id, char *wlan_bw, char *wlan_bssconexist, FILE *fp);

/**
 * \brief This function get the whether support WHNAT.
 *
 * This function get the whether support WHNAT. submit by 
 * \param[out]	 thewhether support WHNAT
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the whether support WHNAT.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the whether support WHNAT.
 */

int wifimgr_lib_is_WHNAT_support(int type, char *val, int len);

/**
 * \brief This function get the whether support TX_RX_STREAM.
 *
 * This function get the whether support TX_RX_STREAM. submit by 
 * \param[out]	 thewhether support TX_RX_STREAM
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the whether support TX_RX_STREAM.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the whether support TX_RX_STREAM.
 */

int wifimgr_lib_is_Tx_Rx_Stream_support(char *val, int len);

/**
 * \brief This function get the max start numble .
 *
 * This function get the max start numble . submit by 
 * \param[out]	 the max start numble
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the max start numble.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the max start numble .
 */

int wifimgr_lib_get_max_sta_num_ui(char *val, int len);

/**
 * \brief This function get the path of wlan11ac button .
 *
 * This function get the path of wlan11ac button . submit by 
 * \param[out]	 the path of wlan11ac button
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the path of wlan11ac button.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the path of wlan11ac button .
 */
 
int wifimgr_lib_get_WLAN11AC_BUTTON_PATH(char *val, int len);

/**
 * \brief This function get the path of wlan button .
 *
 * This function get the path of wlan button . submit by 
 * \param[out]	 the path of wlan button
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the path of wlan button.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the path of wlan button .
 */

int wifimgr_lib_get_WLAN_BUTTON_PATH(char *val, int len);

/**
 * \brief This function get the wlan max start number name .
 *
 * This function get the wlan max start number name . submit by 
 * \param[out]	 the wlan max start number name 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wlan max start number name .
 * \par Example
 * \snippet Api_lib_wifimgr.c Get wlan max start number name .
 */

int wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(int type, char *val, int len);

/**
 * \brief This function get the the def of channel select.
 *
 * This function get the def of channel select . submit by 
 * \param[out]	 the def of channel select 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the def of channel select.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the def of channel select.
 */

int wifimgr_lib_get_CHANNEL_SELECT_DEF(char *val, int len);

/**
 * \brief This function get the  waitting time of mesh  .
 *
 * This function get the waitting time of mesh . submit by 
 * \param[out]	 the  waitting time of mesh 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the  waitting time of mesh .
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the  waitting time of mesh  .
 */

int wifimgr_lib_get_MESH_TRIGGER_WAIT(int *time);

/**
 * \brief This function get the IEEE8021X .
 *
 * This function get the IEEE8021X . submit by 
 * \param[out]	 the IEEE8021X
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the IEEE8021X.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the IEEE8021X .
 */

int wifimgr_lib_get_IEEE8021X_VAL(char *val, int len);

/**
 * \brief This function get the wlan_entry_attr .
 *
 * This function get the wlan_entry_attr . submit by 
 * \param[out]	 the wlan_entry_attr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wlan_entry_attr.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wlan_entry_attr .
 */

int wifimgr_lib_get_wlan_entry_attr(int type, wifi_attr_fun *output);

/**
 * \brief This function get the wlan_igmp_snooping_support .
 *
 * This function get the wlan_igmp_snooping_support . submit by 
 * \param[out]	 the wlan_igmp_snooping_support
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wlan_igmp_snooping_support.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wlan_igmp_snooping_support .
 */
 
 int wifimgr_lib_get_wlan_igmp_snooping_support();
 
/**
 * \brief This function get the wlan_script_com_attr_config .
 *
 * This function get the wlan_script_com_attr_config . submit by 
 * \param[out]	 the wlan_script_com_attr_config
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the wlan_script_com_attr_config.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the wlan_script_com_attr_config .
 */

int wifimgr_lib_get_wlan_script_com_attr_config(int type, wifi_attr_fun *output);

/**
 * \brief This function get the mac of the mobile phone which connected with WPS.
 *
 * This function get the mac of of the mobile phone which connected with WPS submit by 
 * \param[out]	mac of mobile phone.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the the mac of of the mobile phone which connected with WPS.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get the mac of of the mobile phone which connected with WPS
 */

int wifimgr_lib_get_WPS_STA_MAC(wifi_type type, unsigned char *output);

/**
 * \brief This function get the WPS_BUTTON_PATH .
 *
 * This function get the WPS_BUTTON_PATH . submit by 
 * \param[out]	 the WPS_BUTTON_PATH
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WPS_BUTTON_PATH.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WPS_BUTTON_PATH .
 */

int wifimgr_lib_get_WPS_BUTTON_PATH(char *val, int len);

/**
 * \brief This function set the 5g wps button.
 *
 * This function set the 5g wps button. submit by 
 * \param[int]	 the 5g wps button
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the 5g wps button .
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the 5g wps button.
 */

int wifimgr_lib_set_WPS_5G_BUTTON(char *cmd);

/**
 * \brief This function get the WPS_BUTTON .
 *
 * This function get the WPS_BUTTON . submit by 
 * \param[out]	 the WPS_BUTTON
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WPS_BUTTON.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WPS_BUTTON .
 */

int wifimgr_lib_get_WPS_BUTTON(char *string, int len, int *ret);

/**
 * \brief This function set the wps button.
 *
 * This function set the wps button. submit by 
 * \param[int]	 the wps button
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wps button .
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wps button.
 */

int wifimgr_lib_set_WPS_BUTTON(char *cmd);

/**
 * \brief This function get the WPS_5G_BUTTON_PATH .
 *
 * This function get the WPS_5G_BUTTON_PATH . submit by 
 * \param[out]	 the WPS_5G_BUTTON_PATH
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WPS_5G_BUTTON_PATH.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WPS_5G_BUTTON_PATH .
 */

int wifimgr_lib_get_WPS_5G_BUTTON_PATH(char *val, int len);

/**
 * \brief This function get the AHB_STATUS .
 *
 * This function get the AHB_STATUS . submit by 
 * \param[out]	 the AHB_STATUS
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the AHB_STATUS.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the AHB_STATUS .
 */

int wifimgr_lib_get_AHB_STATUS(char *string, int len, int *ret);

/**
 * \brief This function set the AHB STATUS.
 *
 * This function set the AHB STATUS. submit by 
 * \param[int]	 the AHB STATUS
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the AHB STATUS.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the AHB STATUS.
 */

int wifimgr_lib_set_AHB_STATUS(char *cmd);

/**
 * \brief This function set the wlan wps button.
 *
 * This function set the wlan wps button. submit by 
 * \param[int]	 the wlan wps button
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the wlan wps button.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the wlan wps button.
 */

int wifimgr_lib_set_WLAN_WPS_BUTTON(char *cmd);

/**
 * \brief This function set the LED WLAN.
 *
 * This function set the LED WLAN. submit by 
 * \param[int]	 the LED WLAN
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the LED WLAN.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the LED WLAN.
 */

int wifimgr_lib_set_LED_WLAN(char *cmd);

/**
 * \brief This function get the WLAN11AC_BUTTON .
 *
 * This function get the WLAN11AC_BUTTON . submit by 
 * \param[out]	 the WLAN11AC_BUTTON
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WLAN11AC_BUTTON.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WLAN11AC_BUTTON .
 */

int wifimgr_lib_get_WLAN11AC_BUTTON(char *string, int len, int *ret);

/**
 * \brief This function get the WPS_STATUS from ioctl.
 *
 * This function get the WPS_STATUS from ioctl. submit by 
 * \param[out]	 the WPS_STATUS
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WPS_STATUS from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WPS_STATUS from ioctl.
 */

int wifimgr_lib_get_WPS_STATUS(wifi_type type, unsigned short *output);

/**
 * \brief This function get the interface mac .
 *
 * This function get the interface mac . submit by 
 * \param[out]	 the interface mac 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the interface mac .
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the interface mac .
 */

int wifimgr_lib_get_interface_mac(unsigned char *br_itf_name, unsigned char *mac);

/**
 * \brief This function get the itf_hwaddr from ioctl.
 *
 * This function get the itf_hwaddr from ioctl. submit by 
 * \param[out]	 the itf_hwaddr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the itf_hwaddr from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the itf_hwaddr from ioctl.
 */

int wifimgr_lib_get_itf_hwaddr(char *br_itf_name, char *mac, int len);

/**
 * \brief This function get the interface mac from ioctl.
 *
 * This function get the interface mac from ioctl. submit by 
 * \param[out]	 the interface mac
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the interface mac from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the interface mac from ioctl.
 */

int wifimgr_lib_get_interface_mac0(char *br_itf_name, char *mac, int len);

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_BUTTONDETECT)
#include<linux/ioctl.h>
#define LEDBUTTON_IOCTL_MAGIC 0x99
typedef enum e_ledbutton_ioctl
{
	LEDBUTTON_IOCTL_SELECT = _IOW(LEDBUTTON_IOCTL_MAGIC, 0, int[2]),
	LEDBUTTON_MAX_IOCTL		/* please define I/O controls above this item */	
	
} E_LEDBUTTON_IOCTL;

/**
 * \brief This function get LED BUTTON STATUS.
 *
 * This function get LED BUTTON STATUS. submit by 
 * \param[out]	 the LED BUTTON STATUS
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get LED BUTTON STATUS.
 * \par Example
 * \snippet Api_lib_wifimgr.c  Get LED BUTTON STATUS.
 */

int wifimgr_lib_get_LED_BUTTON_STATUS(int args[2]);
#endif/*TCSUPPORT_COMPILE*/

/**
 * \brief This function get wps acl info.
 *
 * This function get wps acl info. submit by 
 * \param[out]	 the wps acl info
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get wps acl info.
 * \par Example
 * \snippet Api_lib_wifimgr.c  Get wps acl info.
 */

#ifdef WSC_AP_SUPPORT
int wifimgr_lib_get_WPS_ACL_INFO(wifi_type type, int index, RT_802_11_ACL * output);
#endif

/**
 * \brief This function to report message.
 *
 * This function to report message. submit by 
 * \param[in]	 the oui
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Report message.
 * \par Example
 * \snippet Api_lib_wifimgr.c Report message.
 */

int wifimgr_lib_set_OUI_FILTER(wifi_type type, char *input, int dataLen);

/**
 * \brief This function get the value from ioctl.
 *
 * This function get the value from ioctl. submit by 
 * \param[out]	 the value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the value from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the value from ioctl.
 */

int wifimgr_lib_OidQueryInformation(unsigned long OidQueryCode, char *DeviceName, void *ptr, unsigned long PtrLength);

/**
 * \brief This function get the CHANNEL from ioctl.
 *
 * This function get the CHANNEL from ioctl. submit by 
 * \param[out]	 the CHANNEL
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the CHANNEL from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the CHANNEL from ioctl.
 */

int wifimgr_lib_get_CHANNEL(wifi_type type, char* outchannel);

/**
 * \brief This function get the WPS_PIN_CODE from ioctl.
 *
 * This function get the WPS_PIN_CODE from ioctl. submit by 
 * \param[out]	 the WPS_PIN_CODE
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WPS_PIN_CODE from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WPS_PIN_CODE from ioctl.
 */

int wifimgr_lib_get_WPS_PIN_CODE(wifi_type type, int index, char* outPinCode);

/**
 * \brief This function get the WpsStatus.
 *
 * This function get the WpsStatus. submit by 
 * \param[out]	 the WpsStatus
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WpsStatus.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WpsStatus.
 */

#ifdef TCSUPPORT_WLAN_MULTI_WPS
#define GNU_PACKED __attribute__ ((packed))
typedef struct GNU_PACKED _WSC_PEER_DEV_INFO {
		unsigned char WscPeerDeviceName[32];
		unsigned char WscPeerManufacturer[64];
		unsigned char WscPeerModelName[32];
		unsigned char WscPeerModelNumber[32];
		unsigned char WscPeerSerialNumber[32];
		unsigned char WscPeerMAC[6];
} WSC_PEER_DEV_INFO;
#define RT_OID_WSC_QUERY_PEER_INFO_ON_RUNNING		0x075C

int wifimgr_lib_get_WpsStatus(wifi_type type, int *outWpsStatus, char *outWpsMac, int i);
#else
int wifimgr_lib_get_WpsStatus(wifi_type type, int * outWpsStatus);
#endif

/**
 * \brief This function get the BndStrgChanload from ioctl.
 *
 * This function get the BndStrgChanload from ioctl. submit by 
 * \param[out]	 the BndStrgChanload
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the BndStrgChanload from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the BndStrgChanload from ioctl.
 */

#ifdef TCSUPPORT_WLAN_BNDSTRG
int wifimgr_lib_get_BndStrgChanload(char* outStatus,unsigned int len);
#endif

/**
 * \brief This function get the WIFI_STATS from ioctl.
 *
 * This function get the WIFI_STATS from ioctl. submit by 
 * \param[out]	 the WIFI_STATS
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WIFI_STATS from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WIFI_STATS from ioctl.
 */

int wifimgr_lib_get_WIFI_STATS(char *ifname, char *buffer, int size);

/**
 * \brief This function set the WIFI CMD.
 *
 * This function set the WIFI CMD. submit by 
 * \param[int]	 the WIFI CMD
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the WIFI CMD.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the WIFI CMD.
 */

int wifimgr_lib_set_WIFI_CMD(char *ifr, int index, int len, char *input, char *output, int type);

/**
 * \brief This function get the WLAN_SCRIPT.
 *
 * This function get the WLAN_SCRIPT. submit by 
 * \param[out]	 the WLAN_SCRIPT
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the WLAN_SCRIPT.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the WLAN_SCRIPT.
 */

int wifimgr_lib_get_WLAN_SCRIPT(wifi_type type, int len, char *output);

typedef struct GNU_PACKED vsie_probresp_set_cfg_s{
	unsigned char stamac[6];
	unsigned char band;
	char IEDATA[252];
	unsigned int length;
}vsie_probresp_set_cfg_t;
typedef struct GNU_PACKED vsie_probresp_del_cfg_s{
	unsigned char stamac[6];
	unsigned char band;
}vsie_probresp_del_cfg_t;
#define RT_OID_AP_PROBE_RSP_VENDOR_IE_SET           0x9204
#define RT_OID_AP_PROBE_RSP_VENDOR_IE_DEL           0x9205

/**
 * \brief This function reply with vsie.
 *
 * This function  reply with vsie. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   reply with vsie.
 * \par Example
 * \snippet Api_lib_wifimgr.c  reply with vsie.
 */

int wifimgr_lib_proberesp_execute(wifi_type type, vsie_probresp_set_cfg_t *buffer, int size, int ssidEnable);

/**
 * \brief This function reply without vsie.
 *
 * This function  reply without vsie. submit by 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   reply without vsie.
 * \par Example
 * \snippet Api_lib_wifimgr.c  reply without vsie.
 */

int wifimgr_lib_proberesp_delete(wifi_type type, vsie_probresp_del_cfg_t *buffer, int size);

/**
 * \brief This function get the CHIP_ID from ioctl.
 *
 * This function get the CHIP_ID from ioctl. submit by 
 * \param[out]	 the CHIP_ID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the CHIP_ID from ioctl.
 * \par Example
 * \snippet Api_lib_wifimgr.c Get  the CHIP_ID from ioctl.
 */

int wifimgr_lib_get_CHIP_ID(unsigned int* chidIdx);

#if defined(TCSUPPORT_CT_WLAN_JOYME3)
typedef struct _diag_TZ_parameter {
	signed char diag_tz_flag;
	unsigned char diag_tz_hour;
	unsigned char diag_tz_minute;

}diag_TZ_parameter;

/**
 * \brief This function set the diag_tz.
 *
 * This function set the diag_tz. submit by 
 * \param[int]	 the diag_tz
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the diag_tz.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the diag_tz.
 */

int wifimgr_lib_set_Diag_Tz(char *ifname, diag_TZ_parameter *buffer, int size);
#endif

/**
 * \brief This function get the br0 ipv6 address.
 *
 * This function get the br0 ipv6 address. submit by
 * \param[out]	 the br0 ipv6 address
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the br0 ipv6 address
 * \par Example
 * \snippet api_lib_wifimgr_user.c get the br0 ipv6 address.
 */
 
int wifimgr_lib_get_br0_v6addr(unsigned char *ifid);

/**
 * \brief This function get the ifaddr.
 *
 * This function get the ifaddr. submit by
 * \param[out]	 the ifaddr
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the ifaddr
 * \par Example
 * \snippet api_lib_wifimgr_user.c get the ifaddr.
 */
 
int wifimgr_lib_get_ifaddr(int sock, char *mac);

/**
 * \brief This function get the WLANSHARE_CMD.
 *
 * This function get the WLANSHARE_CMD. submit by
 * \param[out]	 the WLANSHARE_CMD
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the WLANSHARE_CMD
 * \par Example
 * \snippet api_lib_wifimgr_user.c get the WLANSHARE_CMD.
 */

int wifimgr_lib_get_WLANSHARE_CMD(char *cmd, int actType, int index, char *enableUserId, char *userId);

/**
 * \brief This function get the cpu temperature.
 *
 * This function get the cpu temperature. submit by ruichang.Chen
 * \param[int]	 wifi type {0:2.4G,1:5G}
 * \param[out]	 the cpu temperature
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the cpu temperature
 * \par Example
 * \snippet api_lib_wifimgr_user.c get the cpu temperature.
 */
int wifimgr_lib_get_CPU_TEMPERATURE(wifi_type type, int *output);

/**
 * \brief This function get the cpe mac address.
 *
 * This function get the cpe mac address. submit by Miao.Yu
 * \param[out]	 the cpe mac address
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the cpe mac address
 * \par Example
 * \snippet api_lib_wifimgr_user.c get the cpe mac address.
 */
int wifimgr_lib_get_ethaddr(unsigned char *mac);

/**
 * \brief This function get img_commit_flag_offset.
 *
 * This function get img_commit_flag_offset. submit by
 * \param[out]	 the img_commit_flag_offset
 * \retval	0	Success.
 * \note
 *  get img_commit_flag_offset
 * \par Example
 * \snippet api_lib_wifimgr_user.c get img_commit_flag_offset.
 */
int wifimgr_lib_get_img_commit_flag_offset(unsigned int *result);

/**
 * \brief This function get img_boot_flag_offset.
 *
 * This function get img_boot_flag_offset. submit by
 * \param[out]	 the img_boot_flag_offset
 * \retval	0	Success.
 * \note
 *  get img_boot_flag_offset
 * \par Example
 * \snippet api_lib_wifimgr_user.c get img_boot_flag_offset.
 */
int wifimgr_lib_get_img_boot_flag_offset(unsigned int *result);

#endif

/**
 * \brief This function set PartialScan value.
 *
 * This function set PartialScan.submit by
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set PartialScan value.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set PartialScan value.
 */
int wifimgr_lib_set_PartialScan(wifi_type type);

/**
 * \brief This function get wifi neighbour info.
 *
 * This function get neighbourinfo .submit by
 * \param[in]	type:0-24G 1-5G
 * \param[out]	neifhbor_info : wifi neighbour info
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get wifi neighbour info.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get wifi neighbour info.
 */
int wifimgr_lib_get_neighbourinfo(wifi_type type, NEIFHBOR_INFO *neifhbor_info);   
/**
 * \brief This function get the information of wps.
 *
 * This function get the information of wps. submit by Zhen.Xu 2019.12.27
 * \param[in]	ioctlCmd: ioctl cmd like get_wps_regs_dyn_info.
 * \param[in]	keyword: get value accoding to this keyword like DeviceName =
 * \param[in]	info: the information of wps.
 * \param[in]	infoLen: the length of information.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * get the information of wps.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get the information of wps.
 */
int wifimgr_lib_get_wps_info( char *ioctlCmd,  /*ioctl cmd like get_wps_regs_dyn_info*/
				 char *keyword,   /*get value accoding to this keyword like DeviceName = */
				 char *info,       /*return value*/
				 int infoLen,	  /*the length of info*/
				 char *nodename	 /*the node name of wlan*/
			   );

/**
 * \brief This function set the status of needRebootFlag.
 *
 * This function set the status of needRebootFlag. submit by Zhen.Xu 2019.12.27
 * \param[in]	needRebootFlag:the status of needRebootFlag.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 * set the status of needRebootFlag.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set the status of needRebootFlag.
 */

int wifimgr_lib_set_needRebootFlag(int *needRebootFlag);

/**
 * \brief This function get wifi process info .
 *
 * This function  get wifi process info .submit by Lei.Zhang 2020.1.8
 * \param[out]	process_info: wifi process info
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get wifi process info.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get wifi process info.
 */
int wifimgr_lib_get_wifi_process_info(struct diag_wifi_process_info *process_info);

/**
 * \brief This function insmod different ko according to compile options .
 *
 * This function  insmod different ko according to compile options.submit by ShengQian.Wang 2020.4.8
 * \retval	0	insmod ko success.
 * \note
 *  insmod different ko according to compile options .
 * \par Example
 * \snippet api_lib_wifimgr_user.c insmod different ko according to compile options .
 */
int wifimgr_lib_insmod_ko();  
                                                                                                                                                                                                                                                         /**
 * \brief This function set EAPifname .
 *
 * This function  set EAPifname .submit by Yifan.chen 2021.1.13
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	meaning update wifi_common
 * \param[in]	EAPifname value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set EAPifname.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set EAPifname.
 */
int wifimgr_lib_set_EAPifname(wifi_type type, int index, char *input);

/**
 * \brief This function set PreAuthifname .
 *
 * This function  set PreAuthifname .submit by Yifan.chen 2021.1.13
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	meaning update wifi_common
 * \param[in]	EAPifname value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  set EAPifname.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set PreAuthifname.
 */
int wifimgr_lib_set_PreAuthifname(wifi_type type, int index, char *input);

#if defined(TCSUPPORT_CMCCV2)
/**
 * \brief This function get wifi Sta Rssi Info .
 *
 * This function  get wifiStaRssiInfo .submit by RuiChang.Chen 2021.4.23
 * \param[out]	IoctlData : wifi Sta Rssi Info
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get wifi Sta Rssi Info.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get wifi Sta Rssi Info.
 */
int wifimgr_lib_get_wifiStaRssiInfo(PROBE_REQ_IOCTL_ROOT *IoctlData, wifi_type type);
#endif

/**
 * \brief This function get wifi neighbour info.
 *
 * This function  get neighbourinfo .submit by RuiChang.Chen 2021.4.23
 * \param[out]	IoctlData : wifi neighbour info
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get wifi neighbour info.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get wifi neighbour info.
 */
int wifimgr_lib_get_neighbourinfo_v1(PNDIS_802_11_GET_NEIGHBOUR_INFO IoctlData, wifi_type type);

/**
 * \brief This function set DfsDedicatedZeroWait.
 *
 * This function set DfsDedicatedZeroWait. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[in]	the DfsDedicatedZeroWait value.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set DfsDedicatedZeroWait.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set DfsDedicatedZeroWait.
 */
int wifimgr_lib_set_DfsDedicatedZeroWait(wifi_type type, int index, char *input);

/**
 * \brief This function get DfsDedicatedZeroWait.
 *
 * This function get DfsDedicatedZeroWait. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[out]	output: DfsDedicatedZeroWait value
 * \param[in]	size of output.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get DfsDedicatedZeroWait.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get DfsDedicatedZeroWait.
 */
int wifimgr_lib_get_DfsDedicatedZeroWait(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function set DfsZeroWaitDefault.
 *
 * This function set DfsZeroWaitDefault. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[in]	the DfsZeroWaitDefault value.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set DfsZeroWaitDefault.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set DfsZeroWaitDefault.
 */
int wifimgr_lib_set_DfsZeroWaitDefault(wifi_type type, int index, char *input);

/**
 * \brief This function get DfsZeroWaitDefault.
 *
 * This function get DfsZeroWaitDefault. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[out]	output: DfsZeroWaitDefault value
 * \param[in]	size of output.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get DfsZeroWaitDefault.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get DfsZeroWaitDefault.
 */
int wifimgr_lib_get_DfsZeroWaitDefault(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function set MboSupport.
 *
 * This function set MboSupport. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[in]	the MboSupport value.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set MboSupport.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set MboSupport.
 */
int wifimgr_lib_set_MboSupport(wifi_type type, int index, char *input);

/**
 * \brief This function get MboSupport.
 *
 * This function get MboSupport. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[out]	output: MboSupport value
 * \param[in]	size of output.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get MboSupport.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get MboSupport.
 */
int wifimgr_lib_get_MboSupport(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function set MapMode.
 *
 * This function set MapMode. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[in]	the MapMode value.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set MapMode.
 * \par Example
 * \snippet api_lib_wifimgr_user.c set MapMode.
 */
int wifimgr_lib_set_MapMode(wifi_type type, int index, char *input);

/**
 * \brief This function get MapMode.
 *
 * This function get MapMode. submit by
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[out]	output: MapMode value
 * \param[in]	size of output.
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  get MapMode.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get MapMode.
 */
int wifimgr_lib_get_MapMode(wifi_type type, int index, char *output, unsigned int size);

/**
 * \brief This function set the enableSsid to RT2860.
 *
 * This function set the enableSsid to RT2860. submit by 
 * \param[in]	type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[int]	 the enableSsid
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Set the enableSsid to RT2860.
 * \par Example
 * \snippet Api_lib_wifimgr.c Set the enableSsid to RT2860.
 */
int wifimgr_lib_set_enableSsid(wifi_type type, int index, char *input);
#ifdef TCSUPPORT_CMCCV2
/**
 * \brief This function get info UpdateRadioStatics.
 *
 * This function  get info UpdateRadioStatics .submit by RuiChang.Chen 2021.5.17
 * \param[out]	IoctlData : info UpdateRadioStatics
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get info UpdateRadioStatics.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get info UpdateRadioStatics.
 */
int wifimgr_lib_get_info_UpdateRadioStatics(RADIO_STATISTICS* pRadioStatics, wifi_type type);

/**
 * \brief This function get info UpdateNoise.
 *
 * This function  get info UpdateNoise .submit by RuiChang.Chen 2021.5.17
 * \param[out]	IoctlData : info UpdateNoise
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get info UpdateNoise.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get info UpdateNoise.
 */
int wifimgr_lib_get_info_UpdateNoise(char* Noise, wifi_type type);

/**
 * \brief This function get info UpdateChanLoad.
 *
 * This function  get info UpdateChanLoad .submit by RuiChang.Chen 2021.5.17
 * \param[out]	IoctlData : info UpdateChanLoad
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get info UpdateChanLoad.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get info UpdateChanLoad.
 */
int wifimgr_lib_get_info_UpdateChanLoad(char* ChanLoad, wifi_type type);

/**
 * \brief This function get info UpdateChanIdle.
 *
 * This function  get info UpdateChanIdle .submit by RuiChang.Chen 2021.5.17
 * \param[out]	IoctlData : info UpdateChanIdle
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get info UpdateChanIdle.
 * \par Example
 * \snippet api_lib_wifimgr_user.c get info UpdateChanIdle.
 */
int wifimgr_lib_get_info_UpdateChanIdle(char* ChanIdle, wifi_type type);
#endif

/**
 * \brief This function check wifi dat value.
 *
 * This function  check wifi dat value .submit by RuiChang.Chen 2021.9.14
 * \param[in]	type:0-24G 1-5G
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  check wifi dat value.
 * \par Example
 * \snippet api_lib_wifimgr.c check wifi dat value.
 */
int wifimgr_lib_check_wifi_value_befor_write_dat(wifi_type type);

/**
 * \brief This function do wifi execute.
 *
 * This function  do wifi execute.submit by RuiChang.Chen 2021.9.14
 * \param[in]	type:0-24G 1-5G
 * \param[in]	downup:0-down 1-up
 * \param[in]	downup_other:0-down 1-up
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  do wifi execute.
 * \par Example
 * \snippet api_lib_wifimgr.c do wifi execute.
 */
int wifimgr_lib_wifi_execute(wifi_type type, int downup, int downup_other);

/**
 * \brief This function wps start wifi upnp.
 *
 * This function  wps start wifi upnp.submit by RuiChang.Chen 2021.9.14
 * \param[in]	type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[int]	 the upnp active
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  wps start wifi upnp.
 * \par Example
 * \snippet api_lib_wifimgr.c wps start wifi upnp.
 */
int wifimgr_lib_service_wpsstart_wifiupnp(wifi_type type, int index, char *active);

/**
 * \brief This function check wifi config.
 *
 * This function  check wifi config.submit by RuiChang.Chen 2021.9.14
 * \param[in]	type:0-24G 1-5G
 * \param[in]	action:0-store 1-compare
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  check wifi config.
 * \par Example
 * \snippet api_lib_wifimgr.c check wifi config.
 */
int wifimgr_lib_wifi_config_oper(wifi_type type, int action);

/**
 * \brief This function write wifi execute sh.
 *
 * This function  write wifi execute sh.submit by RuiChang.Chen 2021.9.14
 * \param[in]	type:0-24G 1-5G
 * \param[in]	ssid index
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  write wifi execute sh.
 * \par Example
 * \snippet api_lib_wifimgr.c write wifi execute sh.
 */
int wifimgr_lib_write_wifi_execute_sh(wifi_type type, int index);
#if 0
/**
 * \brief This function write wps execute sh.
 *
 * This function  write wps execute sh.submit by RuiChang.Chen 2021.9.14
 * \param[in]	execute sh file name
 * \param[in]	type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[in]	action:0-run wps 1-reset oob 2-general enrollee pin code
 * \param[in]	is wps running
 * \param[in]	wps mode
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  write wps execute sh.
 * \par Example
 * \snippet api_lib_wifimgr.c write wps execute sh.
 */
int wifimgr_lib_service_wps_set_script(unsigned char *filename, wifi_type type, int index, int action, unsigned char iswpsrunning, char *wpsMode);
#endif

/**
 * \brief This function stop wps.
 *
 * This function stop wps. submit by
 * \param[in]	filename of script
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  stop wps.
 * \par Example
 * \snippet api_lib_wifimgr.c stop wps.
 */
int wifimgr_lib_service_stop_wps(unsigned char *filename, wifi_type type, int index);

/**
 * \brief This function start wps.
 *
 * This function start wps. submit by
 * \param[in]	filename of script
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \param[in]	wps info
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  start wps.
 * \par Example
 * \snippet api_lib_wifimgr.c start wps.
 */
int wifimgr_lib_service_start_wps(unsigned char *filename, wifi_type type, int index,wps_info *wpsinfo);

/**
 * \brief This function set WscGenPinCode.
 *
 * This function set WscGenPinCode. submit by
 * \param[in]	filename of script
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set WscGenPinCode.
 * \par Example
 * \snippet api_lib_wifimgr.c set WscGenPinCode.
 */
int wifimgr_lib_service_genpincode(unsigned char *filename, wifi_type type, int index);

/**
 * \brief This function set WscOOB.
 *
 * This function set WscOOB. submit by
 * \param[in]	filename of script
 * \param[in]	wifi type:0-24G 1-5G
 * \param[in]	ssid index
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  set WscOOB.
 * \par Example
 * \snippet api_lib_wifimgr.c set WscOOB.
 */
int wifimgr_lib_service_resetoob(unsigned char *filename, wifi_type type, int index);

