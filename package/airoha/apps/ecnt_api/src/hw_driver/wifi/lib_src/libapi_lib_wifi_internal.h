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
#ifndef LIBAPI_LIB_WIFIINTER_HEAD
#define LIBAPI_LIB_WIFIINTER_HEAD
#include "libapi_lib_wifimgr.h"

#define OID_SET_OUI_FILTER							0x1220
#define OID_GET_SET_TOGGLE							0x8000
#define OID_GET_CPU_TEMPERATURE 					0x09A1

#define WPS_BUTTON_PATH						"/proc/tc3162/wps_button"
#define WPS_5G_BUTTON_PATH					"/proc/tc3162/wps_5g_button"
#define AHB_STATUS_PATH						"/proc/tc3162/ahb_status"
#define WLAN_WPS_BUTTON_PATH				"/proc/tc3162/wlan_wps_button"
#define WLAN_BUTTON_PATH					"/proc/tc3162/wlan_button"
#define WLAN11AC_BUTTON_PATH 				"/proc/tc3162/wlan11ac_button"
#define LED_WLAN_PATH 						"/proc/tc3162/led_wlan"
#define CHIP_ID_PATH 						"/proc/tc3162/chip_id"
#define WLANSHARE_PROC_PATH 				"/proc/tc3162/wlanshare"


/***********************************************************/
/********************	FUNC CAP BEGIN	********************/
/***********************************************************/

#define ECNT_MT7592_CHIP_CAP	(ECNT_CHIP_CAP_SUPPORT_24G | ECNT_CHIP_CAP_HT_SUPPORT | (BW40 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								NSS2 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET)
								
#define ECNT_MT7615N_CHIP_CAP	(ECNT_CHIP_CAP_SUPPORT_24G | ECNT_CHIP_CAP_HT_SUPPORT | (BW40 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								(NSS4 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | (EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| \
												ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | DLMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET)										

#define ECNT_MT7915D_2G_CHIP_CAP (ECNT_CHIP_CAP_SUPPORT_24G | ECNT_CHIP_CAP_HT_SUPPORT | (BW80 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_HE_SUPPORT | (NSS2 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | \
								(EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | \
								(DLULMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET) | (DLULOFDMA << ECNT_CHIP_CAP_OFDMA_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_SR_SUPPORT | ECNT_CHIP_CAP_TWT_SUPPORT | ECNT_CHIP_CAP_PPE_SUPPORT | ECNT_CHIP_CAP_ER_SUPPORT)

#define ECNT_MT7916D_2G_CHIP_CAP (ECNT_CHIP_CAP_SUPPORT_24G | ECNT_CHIP_CAP_HT_SUPPORT | (BW80 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_HE_SUPPORT | (NSS2 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | \
								(EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | \
								(DLULMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET) | (DLULOFDMA << ECNT_CHIP_CAP_OFDMA_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_SR_SUPPORT | ECNT_CHIP_CAP_TWT_SUPPORT | ECNT_CHIP_CAP_PPE_SUPPORT | ECNT_CHIP_CAP_ER_SUPPORT)


#define ECNT_MT7915N_CHIP_CAP (ECNT_CHIP_CAP_SUPPORT_24G | ECNT_CHIP_CAP_HT_SUPPORT | (BW80 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_HE_SUPPORT | (NSS4 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | \
								(EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | \
								(DLULMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET) | (DLULOFDMA << ECNT_CHIP_CAP_OFDMA_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_SR_SUPPORT | ECNT_CHIP_CAP_TWT_SUPPORT | ECNT_CHIP_CAP_PPE_SUPPORT | ECNT_CHIP_CAP_ER_SUPPORT)



#define ECNT_MT7613E_CHIP_CAP	(ECNT_CHIP_CAP_SUPPORT_5G | ECNT_CHIP_CAP_VHT_SUPPORT | (BW80 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								(NSS2 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | (EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| \
								ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | DLMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET)

#define ECNT_MT7615E_CHIP_CAP	(ECNT_CHIP_CAP_SUPPORT_5G | ECNT_CHIP_CAP_VHT_SUPPORT | (BW80 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								(NSS4 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | (EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| \
								ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | DLMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET)


#define ECNT_MT7915D_5G_CHIP_CAP (ECNT_CHIP_CAP_SUPPORT_5G | ECNT_CHIP_CAP_VHT_SUPPORT | \
								ECNT_CHIP_CAP_HE_SUPPORT | (BW80 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								(NSS2 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | (EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| \
								ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | (DLULMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET) | \
								(DLULOFDMA << ECNT_CHIP_CAP_OFDMA_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_SR_SUPPORT | ECNT_CHIP_CAP_TWT_SUPPORT | ECNT_CHIP_CAP_PPE_SUPPORT | ECNT_CHIP_CAP_ER_SUPPORT)

#define ECNT_MT7916D_5G_CHIP_CAP (ECNT_CHIP_CAP_SUPPORT_5G | ECNT_CHIP_CAP_VHT_SUPPORT | \
								ECNT_CHIP_CAP_HE_SUPPORT | (BW160 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								(NSS2 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | (EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| \
								ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | (DLULMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET) | \
								(DLULOFDMA << ECNT_CHIP_CAP_OFDMA_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_SR_SUPPORT | ECNT_CHIP_CAP_TWT_SUPPORT | ECNT_CHIP_CAP_PPE_SUPPORT | ECNT_CHIP_CAP_ER_SUPPORT)


#define ECNT_MT7915E_CHIP_CAP (ECNT_CHIP_CAP_SUPPORT_5G | ECNT_CHIP_CAP_VHT_SUPPORT | \
								ECNT_CHIP_CAP_HE_SUPPORT | (BW80 << ECNT_CHIP_CAP_BW_SUPPORT_OFFSET) | \
								(NSS4 << ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET) | (EBFIBF << ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET)| \
								ECNT_CHIP_CAP_LDPC_SUPPORT | ECNT_CHIP_CAP_STBC_SUPPORT | (DLULMIMO << ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET) | \
								(DLULOFDMA << ECNT_CHIP_CAP_OFDMA_SUPPORT_OFFSET) | \
								ECNT_CHIP_CAP_SR_SUPPORT | ECNT_CHIP_CAP_TWT_SUPPORT | ECNT_CHIP_CAP_PPE_SUPPORT | ECNT_CHIP_CAP_ER_SUPPORT)

/***********************************************************/
/********************	FUNC CAP END	********************/
/***********************************************************/

int wdskey_check(int wdskeylen, char* input);
int wdskey_check_for_wep(int wdskeylen, char* input);
int doHexCheck(char ch);
int doStrHexCheck(char* input);
int checkV4(char* p);
int doStrDecCheck(char *input);
int IPV4check(char *input);
int MacCheck(char* input);
char * strextract(char *output, char * input);
char * rstrtok(char * s,const char * ct);
char rtstrcasecmp(char *s1, char *s2);
int charCnt(char* input, char character);

void UpdateChannel(char * outchannel);
#ifdef WSC_AP_SUPPORT
void UpdatePinCode(char * outPinCode);
#ifdef TCSUPPORT_WLAN_MULTI_WPS
void UpdateWpsStatus(int * outWpsStatus, char *outWpsMac, int i);
#else
void UpdateWpsStatus(int * outWpsStatus);
#endif
#endif

#if defined(TCSUPPORT_WLAN_AC)
void UpdateChannelAC(char * outchannel);
#ifdef WSC_AP_SUPPORT
void UpdatePinCodeAC(char * outPinCode);
#ifdef TCSUPPORT_WLAN_MULTI_WPS
void UpdateWpsStatusAC(int * outWpsStatus, char *outWpsMac, int i);
#else
void UpdateWpsStatusAC(int * outWpsStatus);
#endif
#endif
#endif

typedef union  _MACHTTRANSMIT_SETTING {
	struct	{
	unsigned short   	MCS:7;  /* MCS*/
	unsigned short		BW:1;	/*channel bandwidth 20MHz or 40 MHz*/
	unsigned short		ShortGI:1;
	unsigned short		STBC:2;	/*SPACE*/ 
	unsigned short		rsv:3;	 
	unsigned short		MODE:2;	/* Use definition MODE_xxx.  */
	}	field;
	unsigned short		word;
 } MACHTTRANSMIT_SETTING, *PMACHTTRANSMIT_SETTING;

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
typedef struct _RT_802_11_MAC_MT7592{	 
		unsigned char       ApIdx;
	    unsigned char       Addr[MAC_ADDR_LENGTH];
	    unsigned char       Aid;
	    unsigned char       Psm;     /*used for ssidindex*/
	    unsigned char		MimoPs;  /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled*/
	   	signed char		AvgRssi0;
		signed char		AvgRssi1;
		signed char		AvgRssi2;
		unsigned int		ConnectedTime;
	  	MACHTTRANSMIT_SETTING	TxRate;
		unsigned int		LastRxRate;	
		signed short StreamSnr[3];			/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
		signed short SoundingRespSnr[3];	/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
		unsigned int DataRate;
		unsigned char PortSecured;
		signed char AvgRssi;
#endif
	}RT_802_11_MAC_MT7592_t;


typedef struct _RT_802_11_MAC_MT7613E{
	unsigned char ApIdx;
	unsigned char	 Addr[MAC_ADDR_LENGTH];
	unsigned char	 Aid;
	unsigned char	 Psm;		/* 0:PWR_ACTIVE, 1:PWR_SAVE */
	unsigned char	 MimoPs;	/* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
	signed char 	 AvgRssi0;
	signed char 	 AvgRssi1;
	signed char 	 AvgRssi2;
	unsigned int ConnectedTime;
	HTTRANSMIT_SETTING TxRate;
	unsigned int		LastRxRate;	
	signed short StreamSnr[3];				/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
	signed short SoundingRespSnr[3];			/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
	unsigned int DataRate;
	unsigned char PortSecured;
	signed char AvgRssi;
#endif
	}RT_802_11_MAC_MT7613E_t;


typedef struct _RT_802_11_MAC_MT7615E{	 
		unsigned char       ApIdx;
	    unsigned char       Addr[MAC_ADDR_LENGTH];
	    unsigned char       Aid;
	    unsigned char       Psm;     /*used for ssidindex*/
	    unsigned char		MimoPs;  /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled*/
	   	signed char		AvgRssi0;
		signed char		AvgRssi1;
		signed char		AvgRssi2;
		unsigned int		ConnectedTime;
	    MACHTTRANSMIT_SETTING	TxRate;
		unsigned int		LastRxRate;
		signed short StreamSnr[3];			/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
		signed short SoundingRespSnr[3];	/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
		unsigned int DataRate;
		unsigned char PortSecured;
		signed char 	AvgRssi;
#endif
}RT_802_11_MAC_MT7615E_t;

typedef struct _RT_802_11_MAC_MT7915E{	 
		unsigned char       ApIdx;
	    unsigned char       Addr[MAC_ADDR_LENGTH];
	    unsigned short  	Aid;
	    unsigned char       Psm;     /*used for ssidindex*/
	    unsigned char		MimoPs;  /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled*/
	   	signed char		AvgRssi0;
		signed char		AvgRssi1;
		signed char		AvgRssi2;
		unsigned int		ConnectedTime;
	    MACHTTRANSMIT_SETTING	TxRate;
		unsigned int		LastRxRate;
		signed short StreamSnr[3];			/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
		signed short SoundingRespSnr[3];	/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
		#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
		unsigned int DataRate;
		unsigned char PortSecured;
		signed char		AvgRssi;
		#endif/*TCSUPPORT_COMPILE*/
		#if defined(TCSUPPORT_CT_UBUS)
		unsigned int EntryType;
		#endif
}RT_802_11_MAC_MT7915E_t;

typedef struct _RT_802_11_MAC_TABLE_7592 {
	unsigned long Num;
	RT_802_11_MAC_MT7592_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7592_t, *PRT_802_11_MAC_TABLE_7592;

typedef struct _RT_802_11_MAC_TABLE_7613E {
    unsigned long       Num;
	RT_802_11_MAC_MT7613E_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7613E_t, *PRT_802_11_MAC_TABLE_7613E;

typedef struct _RT_802_11_MAC_TABLE_7615E {
    unsigned long       Num;
	RT_802_11_MAC_MT7615E_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7615E_t, *PRT_802_11_MAC_TABLE_7615E;

typedef struct _RT_802_11_MAC_TABLE_7915E {
    unsigned long       Num;
	RT_802_11_MAC_MT7915E_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7915E_t, *PRT_802_11_MAC_TABLE_7915E;

typedef union _RT_802_11_MAC_TABLE {
	RT_802_11_MAC_TABLE_7592_t table_7592;
	RT_802_11_MAC_TABLE_7613E_t table_7613;
	RT_802_11_MAC_TABLE_7615E_t table_7615;
	RT_802_11_MAC_TABLE_7915E_t table_7915;
} RT_802_11_MAC_TABLE, *PRT_802_11_MAC_TABLE;
#else
#ifdef WLAN_MULTIDRIVER

typedef struct _RT_802_11_MAC_MT7592{
	unsigned char ApIdx;
	unsigned char Addr[MAC_ADDR_LENGTH];
	unsigned char Aid;
	unsigned char Psm;		/* 0:PWR_ACTIVE, 1:PWR_SAVE */
	unsigned char MimoPs;	/* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
	signed char AvgRssi0;
	signed char AvgRssi1;
	signed char AvgRssi2;
	unsigned int ConnectedTime;
	MACHTTRANSMIT_SETTING TxRate;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_JOYME2)
#if defined (MT7615N) || defined (MT7615E) ||defined (MT7915N) || defined (MT7915E) || defined (MT7916N) || defined (MT7916E)
	MACHTTRANSMIT_SETTING RxRate;
#endif
#if defined (MT7592) || defined (MT7612E)
	unsigned int		LastRxRate;
#endif
#else/*TCSUPPORT_COMPILE*/
	unsigned int		LastRxRate;	
#endif/*TCSUPPORT_COMPILE*/
	signed short 		StreamSnr[3];		/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
	signed short 		SoundingRespSnr[3];	/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
	unsigned int DataRate;
	unsigned char PortSecured;
	signed char		AvgRssi;

#endif
#if defined(TCSUPPORT_CT_UBUS)
	unsigned int EntryType;
#endif
	}RT_802_11_MAC_MT7592_t;
#else
typedef struct _RT_802_11_MAC_MT7592{	 
#if defined(RT5392) || defined(MT7601E)|| defined(MT7592) || defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
		unsigned char       ApIdx;
#endif
	    unsigned char       Addr[MAC_ADDR_LENGTH];
	    unsigned char       Aid;
	    unsigned char       Psm;     /*used for ssidindex*/
	    unsigned char		MimoPs;  /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled*/
	   	signed char		AvgRssi0;
		signed char		AvgRssi1;
		signed char		AvgRssi2;
		unsigned int		ConnectedTime;
	  MACHTTRANSMIT_SETTING	TxRate;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_JOYME2)
#if defined (MT7615N) || defined (MT7615E) ||defined (MT7915N) || defined (MT7915E) || defined(MT7916N) || defined (MT7916E)
		MACHTTRANSMIT_SETTING RxRate;
#endif
#if defined (MT7592) || defined (MT7612E)
		unsigned int		LastRxRate;
#endif
#else/*TCSUPPORT_COMPILE*/
		unsigned int		LastRxRate;	
#endif/*TCSUPPORT_COMPILE*/
#if defined(MT7592)|| defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
		signed short StreamSnr[3];			/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
		signed short SoundingRespSnr[3];	/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#endif
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
		unsigned int DataRate;
	unsigned char PortSecured;
	signed char		AvgRssi;
#endif
#if defined(TCSUPPORT_CT_UBUS)
	unsigned int EntryType;
#endif
	}RT_802_11_MAC_MT7592_t;
#endif

#if defined(TCSUPPORT_WLAN_AC)
#ifdef MT7612E
typedef struct GNU_PACKED _WSC_CONFIGURED_VALUE_MT7612 {
	unsigned short  WscConfigured; /* 1 un-configured; 2 configured */
	unsigned char	WscSsid[32 + 1];
	unsigned short  WscAuthMode;	/* mandatory, 0x01: open, 0x02: wpa-psk, 0x04: shared, 0x08:wpa, 0x10: wpa2, 0x20: wpa2-psk */
	unsigned short 	WscEncrypType;	/* 0x01: none, 0x02: wep, 0x04: tkip, 0x08: aes */
	unsigned char	DefaultKeyIdx;
	unsigned char	WscWPAKey[64 + 1];
	unsigned char	WscSsidLen;
	unsigned char	WscWPAKeyLen;
} WSC_CONFIGURED_VALUE_MT7612;
#endif
#endif

typedef struct _RT_802_11_MAC_MT7612E{
	unsigned char ApIdx;
	unsigned char	 Addr[MAC_ADDR_LENGTH];
	unsigned char	 Aid;
	unsigned char	 Psm;		/* 0:PWR_ACTIVE, 1:PWR_SAVE */
	unsigned char	 MimoPs;	/* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
	signed char 	 AvgRssi0;
	signed char 	 AvgRssi1;
	signed char 	 AvgRssi2;
	unsigned int ConnectedTime;
#if defined(MT7613E)
	HTTRANSMIT_SETTING TxRate;
#else
	MACHTTRANSMIT_SETTING TxRate;
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_JOYME2)
#if defined (MT7615N) || defined (MT7615E) ||defined (MT7915N) || defined (MT7915E) ||defined (MT7916N) || defined (MT7916E)
		MACHTTRANSMIT_SETTING RxRate;
#elif defined (MT7592) || defined (MT7612E)
		unsigned int		LastRxRate;
#endif
#else/*TCSUPPORT_COMPILE*/
		unsigned int		LastRxRate;	
#endif/*TCSUPPORT_COMPILE*/

	#if 0/*must corresponding with driver struct*/
	unsigned int AvgSnr;
	#endif

#if defined(MT7612E)
	int StreamSnr[3];				/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
	int SoundingRespSnr[3];			/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#else
	signed short StreamSnr[3];				/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
	signed short SoundingRespSnr[3];			/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#endif
/*	SHORT TxPER;	*/					/* TX PER over the last second. Percent */
/*	SHORT reserved;*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_IS_CF_PON)
	int IsWscMac; /*0:means not wps mac 1:means wps mac*/
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP)
	unsigned int DataRate;
	unsigned char PortSecured;
	signed char AvgRssi;
#endif
#if defined(TCSUPPORT_CT_UBUS)
		unsigned int EntryType;
#endif
	}RT_802_11_MAC_MT7612E_t;

typedef struct _RT_802_11_MAC_MT7615E{	 
		unsigned char       ApIdx;
	    unsigned char       Addr[MAC_ADDR_LENGTH];
	    unsigned char       Aid;
	    unsigned char       Psm;     /*used for ssidindex*/
	    unsigned char		MimoPs;  /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled*/
	   	signed char		AvgRssi0;
		signed char		AvgRssi1;
		signed char		AvgRssi2;
		unsigned int		ConnectedTime;
	    MACHTTRANSMIT_SETTING	TxRate;
		unsigned int		LastRxRate;
		signed short StreamSnr[3];			/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
		signed short SoundingRespSnr[3];	/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#if defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(MT7615E) || defined(TCSUPPORT_NP)
		unsigned int DataRate;
	unsigned char PortSecured;
	signed char		AvgRssi;
#endif
}RT_802_11_MAC_MT7615E_t;

typedef struct _RT_802_11_MAC_MT7915E{	 
		unsigned char       ApIdx;
	    unsigned char       Addr[MAC_ADDR_LENGTH];
	    unsigned short  	Aid;
	    unsigned char       Psm;     /*used for ssidindex*/
	    unsigned char		MimoPs;  /* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled*/
	   	signed char		AvgRssi0;
		signed char		AvgRssi1;
		signed char		AvgRssi2;
		unsigned int		ConnectedTime;
	    MACHTTRANSMIT_SETTING	TxRate;
		unsigned int		LastRxRate;
		signed short StreamSnr[3];			/* BF SNR from RXWI. Units=0.25 dB. 22 dB offset removed */
		signed short SoundingRespSnr[3];	/* SNR from Sounding Response. Units=0.25 dB. 22 dB offset removed */
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
		unsigned int DataRate;
	unsigned char PortSecured;
	signed char		AvgRssi;
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_CT_UBUS)
		unsigned int EntryType;
#endif
}RT_802_11_MAC_MT7915E_t;

typedef struct _RT_802_11_MAC_TABLE_7592 {
	unsigned long Num;
	RT_802_11_MAC_MT7592_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7592_t, *PRT_802_11_MAC_TABLE_7592;
	
typedef struct _RT_802_11_MAC_TABLE_7612E {
    unsigned long       Num;
	RT_802_11_MAC_MT7612E_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7612E_t, *PRT_802_11_MAC_TABLE_7612E;

typedef struct _RT_802_11_MAC_TABLE_7615E {
    unsigned long       Num;
	RT_802_11_MAC_MT7615E_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7615E_t, *PRT_802_11_MAC_TABLE_7615E;

typedef struct _RT_802_11_MAC_TABLE_7915E {
    unsigned long       Num;
	RT_802_11_MAC_MT7915E_t Entry[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE_7915E_t, *PRT_802_11_MAC_TABLE_7915E;

typedef union _RT_802_11_MAC_TABLE {
	RT_802_11_MAC_TABLE_7592_t table_7592;
	RT_802_11_MAC_TABLE_7612E_t table_7612;
	RT_802_11_MAC_TABLE_7615E_t table_7615;
	RT_802_11_MAC_TABLE_7915E_t table_7915;
} RT_802_11_MAC_TABLE, *PRT_802_11_MAC_TABLE;
#endif

#ifndef MAX_KEYWORD_LEN
#define MAX_KEYWORD_LEN    		    160
#endif

#define MTKINTERFACEPREFIX "ra"
#define MTKINTERFACE5GPREFIX "rai"

typedef enum _action_code{
	INIT_PARA = 0,
	COMPARE_PARA,
}action_code;

typedef struct _wifi_common_config{
	unsigned int BssidNum;
	unsigned int WHNAT;
	unsigned int channel;
	unsigned int CountryRegion;
	unsigned int CountryRegionABand;
	unsigned int BeaconPeriod;
	unsigned int DtimPeriod;
	unsigned int HtTxstream;
	unsigned int HtRxstream;
	unsigned int HtBssCoex;
	unsigned int WirelessMode;
	unsigned int HtExchar;
	unsigned int HTBw;
	unsigned int HtGi;
	unsigned int VHTBw;
	unsigned int VHtGi;
	unsigned int txPower;
#if defined(TCSUPPORT_WLAN_AX)
	unsigned int MuOfdmaDlEnable;
	unsigned int MuOfdmaUlEnable;
	unsigned int MuMimoDlEnable;
	unsigned int MuMimoUlEnable;
	unsigned int TWTSupport;
	unsigned int SREnable;
	unsigned int SRMode;
	unsigned int SRSDEnable;
#endif
	unsigned int FragThreshold;
}wifi_common_config;

typedef struct _wifi_entry_config{
	unsigned char APSDCapable[8];
	unsigned char WscV2Support[8];
	unsigned char EnableSSID[8];
}wifi_entry_config;

typedef struct _wifi_wds_config{
	unsigned char WdsEnable;
	unsigned char WdsEncrypType[32];
	unsigned char WdsKey[65];
	unsigned char Wds_MAC[80];
	unsigned char WepKeyStr[4][27];
}wifi_wds_config;

typedef struct _wifi_reload_config{
	char itf[16];
	wifi_common_config common;
	wifi_wds_config wds;
	wifi_entry_config entry;
}wifi_reload_config;
typedef struct 
{
	char cap_name[32];
	void (*wifi_store)(wifi_reload_config*, wifi_type, int);
	int (*wifi_compare)(wifi_reload_config*, wifi_type, int);
}wifi_check_config_fun;

typedef struct 
{
	char attr_name[32];
	int (*wifi_api)(wifi_type, int, char*, unsigned int);
	unsigned int *buf;
}wifi_get_config_fun;

typedef struct _wps_para
{
	int wpsConfMode;
	int wpsMode;
	unsigned char wpsConfStatus;
	unsigned int wpsPinCode;
}wps_para;

typedef struct _interface_para
{
	char band;
	unsigned short index;
}interface_para;

int wps_service_action_internal(unsigned char *filename, int action,interface_para*interconfig,wps_para*config);


#endif
